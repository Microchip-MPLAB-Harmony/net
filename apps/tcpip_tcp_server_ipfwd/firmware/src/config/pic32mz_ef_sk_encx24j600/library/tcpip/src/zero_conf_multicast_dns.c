/*******************************************************************************
  Zero Configuration (Zeroconf) Multicast DNS and
  Service Discovery Module for Microchip TCP/IP Stack

  Summary:

  Description: The mDNS servers listen on a standardized multicast IP address of 224.0.0.251
               (or ff02::fb for IPv6 link-local addressing).
*******************************************************************************/

/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/









#include <ctype.h>

#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_MDNS

#include "tcpip_private.h"
#include "tcpip_module_manager.h"

#if defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL) && defined(TCPIP_STACK_USE_ZEROCONF_MDNS_SD)


#include "zero_conf_link_local_private.h"
#include "zero_conf_helper.h"

#define MDNS_TASK_TICK_RATE     TCPIP_ZC_MDNS_TASK_TICK_RATE     // task rate, ms

#define MDNS_PORT            TCPIP_ZC_MDNS_PORT
#define MAX_HOST_NAME_SIZE   TCPIP_ZC_MDNS_MAX_HOST_NAME_SIZE      //31+'\0'  Max Host name size
#define MAX_LABEL_SIZE       TCPIP_ZC_MDNS_MAX_LABEL_SIZE      //63+'\0'  Maximum size allowed for a label. RFC 1035 (2.3.4) == 63
#define MAX_RR_NAME_SIZE     TCPIP_ZC_MDNS_MAX_RR_NAME_SIZE   //255+'\0' Max Resource Recd Name size. RFC 1035 (2.3.4) == 255
#define MAX_SRV_TYPE_SIZE    TCPIP_ZC_MDNS_MAX_SRV_TYPE_SIZE      //31+'\0'  eg. "_http._tcp.local". Max could be 255, but is an overkill.
#define MAX_SRV_NAME_SIZE    TCPIP_ZC_MDNS_MAX_SRV_NAME_SIZE      //63+'\0'  eg. "My Web server". Max could be 255, but is an overkill.
#define MAX_TXT_DATA_SIZE    TCPIP_ZC_MDNS_MAX_TXT_DATA_SIZE   //127+'\0' eg. "path=/index.htm"
#define RESOURCE_RECORD_TTL_VAL     TCPIP_ZC_MDNS_RESOURCE_RECORD_TTL_VAL // Time-To-Live for a Resource-Record in seconds.

#define MAX_RR_NUM  TCPIP_ZC_MDNS_MAX_RR_NUM            // for A, PTR, SRV, and TXT  Max No.of Resource-Records/Service

/* Constants from mdns.txt (IETF Draft)*/
#define MDNS_PROBE_WAIT             TCPIP_ZC_MDNS_PROBE_WAIT // msecs  (initial random delay)
#define MDNS_PROBE_INTERVAL         TCPIP_ZC_MDNS_PROBE_INTERVAL // msecs (maximum delay till repeated probe)
#define MDNS_PROBE_NUM                TCPIP_ZC_MDNS_PROBE_NUM //      (number of probe packets)
#define MDNS_MAX_PROBE_CONFLICT_NUM  TCPIP_ZC_MDNS_MAX_PROBE_CONFLICT_NUM // max num of conflicts before we insist and move on to announce ...
#define MDNS_ANNOUNCE_NUM             TCPIP_ZC_MDNS_ANNOUNCE_NUM //      (number of announcement packets)
#define MDNS_ANNOUNCE_INTERVAL      TCPIP_ZC_MDNS_ANNOUNCE_INTERVAL // msecs (time between announcement packets)
#define MDNS_ANNOUNCE_WAIT          TCPIP_ZC_MDNS_ANNOUNCE_WAIT // msecs (delay before announcing)

/* Resource-Record Types from RFC-1035 */
/*
All RRs have the same top level format shown below:

  0  1  2  3  4 5  6  7  8  9  10 11 12 13 14 15
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
|                                               |
/                                               /
/                    NAME                       /
|                                               |
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
|                    TYPE                       |
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
|                    CLASS                      |
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
|                     TTL                       |
|                                               |
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
|                    RDLENGTH                   |
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--|
/                     RDATA                     /
/                                               /
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 */
typedef enum {
    QTYPE_A = 1,      //QUERY TYPE response = Address
    QTYPE_NS = 2,     //QUERY TYPE response = Authorative Name Server
    QTYPE_CNAME = 5,  //the canonical domain name for an alias
    QTYPE_PTR = 12,   // a domain name pointer
    QTYPE_TXT = 16,   // text strings
    QTYPE_SRV = 33,
    QTYPE_ANY = 255,
}MDNS_QTYPE;

/* Indexes in Resource-record list */
#define QTYPE_A_INDEX   0
#define QTYPE_PTR_INDEX 1
#define QTYPE_SRV_INDEX 2
#define QTYPE_TXT_INDEX 3

/* MDNS Message Fomrat, which is common
 * for Queries and Resource-Records. Taken
 * from RFC 1035
 */
/* MDNS Message Header Flags */
typedef union _MDNS_MSG_HEADER_FLAGS {

    struct {
      uint8_t        rcode:4;
      uint8_t        z:3;
        uint8_t        ra:1;
      uint8_t        rd:1;
      uint8_t        tc:1;
      uint8_t        aa:1;
      uint8_t        opcode:4;
        uint8_t        qr:1;
    }bits;
    uint16_t Val;
   uint8_t v[2];
} MDNS_MSG_HEADER_FLAGS;

/* MDNS Message-Header Format */
typedef struct _MDNS_MSG_HEADER
{
   TCPIP_UINT16_VAL query_id;
   MDNS_MSG_HEADER_FLAGS flags;
   TCPIP_UINT16_VAL nQuestions;
   TCPIP_UINT16_VAL nAnswers;
   TCPIP_UINT16_VAL nAuthoritativeRecords;
   TCPIP_UINT16_VAL nAdditionalRecords;
} MDNS_MSG_HEADER;

/* DNS-Query Format, which is prepended by
 * DNS-MESSAGE Header defined above */
struct question
{
    unsigned char *name;
    unsigned short int type, class;
};

/* DNS-Resource Record Format, which is
 * prepended by DNS-MESSAGE Header
 * defined above. This definition includes
 * all resource-record data formats, to have
 * small-memory foot print */

struct _mDNSProcessCtx_sd;// mdnsd_struct
struct _mDNSProcessCtx_common;

typedef struct _mDNSResourceRecord
{
    uint8_t            *name;
    TCPIP_UINT16_VAL   type;
    TCPIP_UINT16_VAL   class;
    TCPIP_UINT32_VAL   ttl;
    TCPIP_UINT16_VAL   rdlength;

   union {
      IPV4_ADDR ip;      // for A record

      struct {
         TCPIP_UINT16_VAL priority;
         TCPIP_UINT16_VAL weight;
         TCPIP_UINT16_VAL port;
      } srv;         // for SRV record
   };

   // DO NOT merge this into the union.
   uint8_t *rdata;      // for PTR, SRV and TXT records.

    /* House-Keeping Stuff */

   // pointer to the header Ctx of the process that "owns" this resource record.
   struct _mDNSProcessCtx_common *pOwnerCtx;

    uint8_t valid; /* indicates whether rr is valid */
   bool bNameAndTypeMatched;
   bool bResponseRequested;
   bool bResponseSuppressed;
} mDNSResourceRecord;

/* DNS-SD Specific Data-Structures */

typedef enum _MDNS_STATE
{
   MDNS_STATE_HOME = 0,
    MDNS_STATE_INTF_NOT_CONNECTED,
    MDNS_STATE_IPADDR_NOT_CONFIGURED,
   MDNS_STATE_NOT_READY,
   MDNS_STATE_INIT,
   MDNS_STATE_PROBE,
   MDNS_STATE_ANNOUNCE,
   MDNS_STATE_DEFEND,
} MDNS_STATE;

typedef enum _MDNS_RR_GROUP
{
   MDNS_RR_GROUP_QD, // Quuery count
   MDNS_RR_GROUP_AN, // Answer count
   MDNS_RR_GROUP_NS, // Authority record count
   MDNS_RR_GROUP_AR  // Addition Record Count
} MDNS_RR_GROUP;

typedef struct _mDNSResponderCtx
{
   mDNSResourceRecord   rr_list[MAX_RR_NUM];   // Our resource records.

   bool                 bLastMsgIsIncomplete;   // Last DNS msg was truncated
   TCPIP_UINT16_VAL     query_id;            // mDNS Query transaction ID
   IPV4_ADDR            prev_ipaddr;         // To keep track of changes in IP-addr
} mDNSResponderCtx;

typedef enum _MDNS_CTX_TYPE
{
   MDNS_CTX_TYPE_HOST = 0,
   MDNS_CTX_TYPE_SD
} MDNS_CTX_TYPE;

typedef struct _mDNSProcessCtx_common
{
   MDNS_CTX_TYPE   type;      // Is owner mDNS ("HOST") or mDNS-SD ("SD")?
   MDNS_STATE      state;      // PROBE, ANNOUNCE, DEFEND, ...

   uint8_t nProbeCount;
   uint8_t nProbeConflictCount;
    uint8_t nClaimCount;
    bool bProbeConflictSeen;
    bool bLateConflictSeen;

   bool bConflictSeenInLastProbe;
   uint8_t nInstanceId;

   uint32_t event_time;   // Internal Timer, to keep track of events
   uint8_t time_recorded; // Flag to indicate event_time is loaded
   uint32_t random_delay;


} mDNSProcessCtx_common;

typedef struct _mDNSProcessCtx_host
{
   mDNSProcessCtx_common common;

   mDNSResponderCtx *pResponderCtx;

   // other host name related info

   uint8_t szUserChosenHostName[MAX_HOST_NAME_SIZE];   // user chosen host name
   uint8_t szHostName[MAX_HOST_NAME_SIZE];               // mDNS chosen Host-Name

} mDNSProcessCtx_host;

typedef struct _mDNSProcessCtx_sd
{
   mDNSProcessCtx_common common;

   mDNSResponderCtx *pResponderCtx;

   // info specific to SD
    uint8_t srv_name[MAX_SRV_NAME_SIZE];
    uint8_t srv_type[MAX_SRV_TYPE_SIZE];
    uint8_t sd_qualified_name[MAX_RR_NAME_SIZE];
    uint8_t used; /* Spinlock to protect Race-cond. */

    uint8_t sd_auto_rename: 1,        /* Flag to show auto-Rename is enabled */
         sd_service_advertised: 1, /* Flag to show whether service is advertised */
       service_registered: 1;    /* Flag to indicate that user has registered this service */

    uint16_t sd_port; /* Port number in Local-sys where Service is being offered */
    uint8_t sd_txt_rec[MAX_TXT_DATA_SIZE];
    uint8_t sd_txt_rec_len;

    void (*sd_call_back)(char *, MDNSD_ERR_CODE , void *);
    void *sd_context;

} mDNSProcessCtx_sd;




/* DNS-SD State-Machine */



/* Multicast-DNS States defintion */

/************** Global Declarations ***************/
/* Remote Node info, which is Multicast-Node
 * whose IP-address is 224.0.0.251 & MAC-Address
 * is 01:00:5E:00:00:FB. Multicast-IP address for
 * mDNS is specified by mdns.txt (IETF). IP is
 * translated into Multicast-MAC address according
 * rules specified in Std.
 */


                         // mDNS Server/Client (Responder/Qurier)


/* Global declaration to support Message-Compression
 * defined in RFC-1035, Section 4.1.4 */



////////////////////////////////////
typedef enum
{
      MDNS_RESPONDER_INIT,
      MDNS_RESPONDER_LISTEN
}MDNS_RESPONDER_TYPE;

typedef union
{
    uint16_t    val;
    struct
    {
        uint16_t    mcastFilterSet      :  1;       // Multi cast filter enabled on this interface
        uint16_t    reserved            : 15;       // future use
    };
}MDNS_DESC_FLAGS;


typedef struct
{
    TCPIP_NET_IF*          mTcpIpNetIf;
    mDNSProcessCtx_host    mHostCtx;
    mDNSProcessCtx_sd      mSDCtx;
    mDNSResponderCtx       mResponderCtx;
    char                   CONST_STR_local[9];
    UDP_SOCKET             mDNS_socket;
    uint16_t               mDNS_offset;
    uint16_t               mDNS_responder_state;    // MDNS_RESPONDER_TYPE type
    MDNS_DESC_FLAGS        MDNS_flags;
} DNSDesc_t;



/* Forward declarations */
static void _mDNSSetAddresses(DNSDesc_t *pDNSdesc);
static void _mDNSResponder(DNSDesc_t *pDNSdesc);
static uint16_t _mDNSDeCompress(uint16_t wPos
                                   ,uint8_t *pcString
                                   ,bool bFollowPtr
                                   ,uint8_t cElement
                                   ,uint8_t cDepth
                                   ,DNSDesc_t *pDNSdesc);
static size_t _mDNSSDFormatServiceInstance(uint8_t *string, size_t strSize );
static MDNSD_ERR_CODE _mDNSHostRegister( char *host_name,DNSDesc_t *pDNSdesc);
static void _mDNSFillHostRecord(DNSDesc_t *pDNSdesc);
static void _mDNSSDFillResRecords(mDNSProcessCtx_sd *sd,DNSDesc_t *pDNSdesc);
static void _mDNSAnnounce(mDNSResourceRecord *pRR, DNSDesc_t *pDNSdesc);
static void _mDNSProcessInternal(mDNSProcessCtx_common *pCtx, DNSDesc_t *pDNSdesc);

static void TCPIP_MDNS_Process(void);
static void _mDNSSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param);

static DNSDesc_t *gDNSdesc;
static int  mDNSInitCount = 0;      // mDNS module initialization count
static tcpipSignalHandle   mdnsSignalHandle = 0;  // mDNS asynchronous handle
/////////////////////////////////////////




/************* Local String Functions ******************/



/***************************************************************
  Function:
   static uint8_t _strcmp_local_ignore_case(uint8_t *string_1, uint8_t *string_2)

  Summary:
   Compares two strings by ignoring the case.

  Parameters:
   string_1 & string_2 - Two strings

  Returns:
    Zero: If two strings are equal.
    Non-Zero: If both strings are not equal or on error case
  **************************************************************/
static uint8_t _strcmp_local_ignore_case(uint8_t *str_1, uint8_t *str_2)
{
    if(str_1 == NULL || str_2 == NULL)
    {
        WARN_MDNS_PRINT("strmcmp_local_ignore_case: String is NULL \r\n");
        return -1;
    }

    while(*str_1 && *str_2){
        if(*str_1 == *str_2 || (*str_1-32) == *str_2 ||
         *str_1 == (*str_2-32))
      {
         str_1++;
         str_2++;
            continue;
      }
      else
         return 1;

    }
    if(*str_1 == '\0' && *str_2 == '\0')
        return 0;
    else
        return 1;

}


static void
_mDNSCountersReset(mDNSProcessCtx_common *pHeader, bool bResetProbeConflictCount)
{
   if (bResetProbeConflictCount)
   {
        pHeader->nProbeConflictCount = 0;
   }

   pHeader->nProbeCount = 0;
   pHeader->nClaimCount = 0;
   pHeader->bLateConflictSeen = false;
   pHeader->bProbeConflictSeen = false;
}

/***************************************************************
  Function:
   static void _mDNSRename(uint8_t *str, uint8_t max_len)

  Summary:
   Renames a string with a numerical-extension.

  Description:
   This function is to rename host-name/Resource-Record Name,
    when a name-conflict is detected on local-network.
    For-Ex: "myhost" is chosen name and a conflict is detected
    this function renames as "myhost-2". Also ensures that string
    is properly formatted.

  Precondition:
   None

  Parameters:
   String - the string to be Renamed with Numerical-Extenstion.
    max_len - Maximum Length allowed for String

  Returns:
     None
  **************************************************************/
// strLabel:  the user registered name.
//            E.g., "Web Server", for service name (srv_name), or
//                 "My Host", for host name (taken from MY_DEFAULT_HOST_NAME)
// nLabelId:  instance number, to avoid conflict in the name space.
// strBase:   the base name for the appropriate name space.
//            E.g., "_http._tcp.local" for service name, or
//                 "local" for host name.
// strTarget: where the newly constructed fully-qualified-name will be stored.
// nMaxLen:   max length for the newly constructed label, which is the first portion of the
//            fully-qualified-name
//
// ("Web Server", 3, "_http._tcp.local", strTarget, 63) =>
//     stores "Web Server-3._http._tcp.local" to *strTarget.
// ("MyHost", 2, "local", strTarget, 63) =>
//     stores "MyHost-2.local" to *strTarget
//
static void _mDNSRename(uint8_t *strLabel, uint8_t nLabelId, uint8_t *strBase, uint8_t *strTarget, uint8_t nMaxLen)
{
    size_t  targetLen;
   uint8_t n = nLabelId;
#define mDNSRename_ID_LEN 6
   uint8_t str_n[mDNSRename_ID_LEN]; //enough for "-255." + '\0'.
   uint8_t i = mDNSRename_ID_LEN - 1 ;

   str_n[i--] = 0;
   str_n[i--] = '.';

   // construct str_n from n
   while (i != 0)
   {
      str_n[i--] = '0'+ n%10;
      if (n < 10) break;
      n = n/10;
   }
   str_n[i] = '-';

    targetLen = strncpy_m((char*)strTarget, nMaxLen, 3, strLabel, &(str_n[i]), strBase);


   if ( targetLen == nMaxLen )
   {
#ifdef MDNS_WARN
      MDNS_WARN("_mDNSRename: label too long - truncated\r\n");
#endif
   }


}

/***************************************************************
  Function:
   static void _mDNSPutString(uint8_t* String)

  Summary:
   Writes a string to the Multicast-DNS socket.

  Description:
   This function writes a string to the Multicast-DNS socket,
    ensuring that it is properly formatted.

  Precondition:
   UDP socket is obtained and ready for writing.

  Parameters:
   String - the string to write to the UDP socket.

  Returns:
     None
  **************************************************************/
static void _mDNSPutString(uint8_t* string, DNSDesc_t * pDNSdesc)
{
   uint8_t *right_ptr,*label_ptr;
   uint8_t label[MAX_LABEL_SIZE];
   uint8_t i;
   uint8_t len;

   right_ptr = string;

   while(1)
   {
        label_ptr = label;
        len = 0;
        while(*right_ptr)
        {
            i = *right_ptr;

            if(i == '.' || i == '/' ||
               i == ',' || i == '>' || i == '\\')
            {
             /* Formatted Serv-Instance will have '\.'
                * instead of just '.' */
                if(i == '\\')
                {
                    right_ptr++;
                }
                else
                    break;
            }
            *label_ptr++ = *right_ptr;
            len++;
            right_ptr++;
        }
        i = *right_ptr++;

      // Put the length and data
      // Also, skip over the '.' in the input string
      TCPIP_UDP_Put(pDNSdesc->mDNS_socket, len);
      TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, label, len);
      string =  right_ptr;

      if(i == 0x00u || i == '/' || i == ',' || i == '>')
         break;
   }

   // Put the string null terminator character
   TCPIP_UDP_Put(pDNSdesc->mDNS_socket, 0x00);
}

static uint16_t _mDNSStringLength(uint8_t* string)
{
       uint8_t *right_ptr,*label_ptr;
   uint8_t label[MAX_LABEL_SIZE];
   uint8_t i;
   uint8_t len;
   uint16_t retVal = 0;

   right_ptr = string;

   while(1)
   {
        label_ptr = label;
        len = 0;
        while(*right_ptr)
        {
            i = *right_ptr;

            if(i == '.' || i == '/' ||
               i == ',' || i == '>' || i == '\\')
            {
             /* Formatted Serv-Instance will have '\.'
                * instead of just '.' */
                if(i == '\\')
                {
                    right_ptr++;
                }
                else
                    break;
            }
            *label_ptr++ = *right_ptr;
            len++;
            right_ptr++;
        }
        i = *right_ptr++;

      // Put the length and data
      // Also, skip over the '.' in the input string
      retVal ++;
      retVal += len;
      string =  right_ptr;

      if(i == 0x00u || i == '/' || i == ',' || i == '>')
         break;
   }
   retVal ++;
   return retVal;
}

/***************************************************************
  Function:
   static void _mDNSProbe(uint8_t *name, MDNS_QTYPE q_type)

  Summary:
   Sends out Multicast-DNS probe packet with Host-name

  Description:
   This function is used to send out mDNS-probe packet for
    checking uniqueness of selected host-name. This function
    sends out DNS-Query with chosen host-name to Multicast-Address.

    If any other machine is using same host-name, it responds with
    a reply and this host has to select different name.

  Precondition:
   None

  Parameters:
 *
 *

  Returns:
     None
  **************************************************************/
static bool _mDNSProbe(mDNSProcessCtx_common *pCtx, DNSDesc_t *pDNSdesc)
{
   MDNS_MSG_HEADER mDNS_header;

    // Abort operation if no UDP sockets are available

   if(pDNSdesc->mDNS_socket == INVALID_UDP_SOCKET)
    {
        WARN_MDNS_PRINT("_mDNSProbe: Opening UDP Socket Failed \r\n");
      return false;
    }

   // Make certain the socket can be written to
    if(TCPIP_UDP_TxPutIsReady(pDNSdesc->mDNS_socket, 256) < 256)
    {
        WARN_MDNS_PRINT("_mDNSProbe: UDP Socket TX Busy \r\n");
      return false;
    }

    // Put DNS query here
   pDNSdesc->mResponderCtx.query_id.Val++;

   mDNS_header.query_id.Val = TCPIP_Helper_htons(pDNSdesc->mResponderCtx.query_id.Val);   // User chosen transaction ID
   mDNS_header.flags.Val = 0;                              // Standard query with recursion
   mDNS_header.nQuestions.Val = TCPIP_Helper_htons(((uint16_t)1u));               // 1 entry in the question section
   mDNS_header.nAnswers.Val = 0;                           // 0 entry in the answer section
   mDNS_header.nAuthoritativeRecords.Val = TCPIP_Helper_htons(((uint16_t)1u));      // 1 entry in name server section
   mDNS_header.nAdditionalRecords.Val = 0;                     // 0 entry in additional records section

   // Put out the mDNS message header
   TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, (uint8_t *) &mDNS_header, sizeof(MDNS_MSG_HEADER));

   // Start of the QD section
   switch (pCtx->type)
   {
   case MDNS_CTX_TYPE_HOST:
      _mDNSPutString(pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].name,pDNSdesc);
      break;

   case MDNS_CTX_TYPE_SD:
      _mDNSPutString(pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].name,pDNSdesc);
      break;
   }

   {
   //TCPIP_UDP_Put(pDNSdesc->mDNS_socket, 0x00);         // Type: Always QTYPE_ANY
   //TCPIP_UDP_Put(pDNSdesc->mDNS_socket, QTYPE_ANY);

   //TCPIP_UDP_Put(pDNSdesc->mDNS_socket, 0x80);         // Class: Cache-Flush
   //TCPIP_UDP_Put(pDNSdesc->mDNS_socket, 0x01);         //        IN (Internet)
     uint8_t mdnsinfo[] = {0x00, QTYPE_ANY, 0x80, 0x01};
     TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, mdnsinfo, sizeof(mdnsinfo));


   }
   // Start of the NS section
   switch (pCtx->type)
   {
   case MDNS_CTX_TYPE_HOST:
      _mDNSPutString(pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].name,pDNSdesc);
      break;

   case MDNS_CTX_TYPE_SD:
      _mDNSPutString(pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].name,pDNSdesc);
      break;
   }

   {
       uint8_t nsInfo[] = {
           0x00, 0x00, 0x00, 0x01,
           0x00, 0x00, 0x00, 0x78
       };
       //TCPIP_UDP_Put(pDNSdesc->mDNS_socket, 0x00);      // Type: A or SRV
       switch (pCtx->type)
       {
       case MDNS_CTX_TYPE_HOST:
           nsInfo[1] = QTYPE_A;
          //TCPIP_UDP_Put(pDNSdesc->mDNS_socket, QTYPE_A);
          break;

       case MDNS_CTX_TYPE_SD:
           nsInfo[1] = QTYPE_SRV;
          //TCPIP_UDP_Put(pDNSdesc->mDNS_socket, QTYPE_SRV);
          break;
       }
       //TCPIP_UDP_Put(pDNSdesc->mDNS_socket, 0x00);      // Class: Cache-Flush bit MUST NOT be set
       //TCPIP_UDP_Put(pDNSdesc->mDNS_socket, 0x01);      //IN (Internet)

       //TCPIP_UDP_Put(pDNSdesc->mDNS_socket, 0x00);      // 0x00000078 Time To Live, 2 minutes
       //TCPIP_UDP_Put(pDNSdesc->mDNS_socket, 0x00);
       //TCPIP_UDP_Put(pDNSdesc->mDNS_socket, 0x00);
       //TCPIP_UDP_Put(pDNSdesc->mDNS_socket, 0x78);
       TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, nsInfo, sizeof(nsInfo));

   }

   switch (pCtx->type)
   {
   case MDNS_CTX_TYPE_HOST:
      {
          uint8_t hostInfo[] = {
              pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].rdlength.v[1],
              pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].rdlength.v[0],
              pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].ip.v[0],
              pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].ip.v[1],
              pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].ip.v[2],
              pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].ip.v[3]
          };
         /*TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].rdlength.v[1]);
         TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].rdlength.v[0]);

         TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].ip.v[0]);
         TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].ip.v[1]);
         TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].ip.v[2]);
         TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].ip.v[3]);
         */
          TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, hostInfo, sizeof(hostInfo));


         break;
      }

   case MDNS_CTX_TYPE_SD:
      {
          uint8_t sdInfo[] = {
              pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].rdlength.v[1],
              pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].rdlength.v[0],
              pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].srv.priority.v[1],
              pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].srv.priority.v[0],
              pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].srv.weight.v[1],
              pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].srv.weight.v[0],
              pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].srv.port.v[1],
              pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].srv.port.v[0],
          };
         /*TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].rdlength.v[1]);
         TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].rdlength.v[0]);
         TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].srv.priority.v[1]);
         TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].srv.priority.v[0]);
         TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].srv.weight.v[1]);
         TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].srv.weight.v[0]);
         TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].srv.port.v[1]);
         TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].srv.port.v[0]);*/
          TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, sdInfo, sizeof(sdInfo));

         _mDNSPutString(pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].rdata,pDNSdesc);

         break;
      }
   }

   _mDNSSetAddresses(pDNSdesc);
   TCPIP_UDP_Flush(pDNSdesc->mDNS_socket);

    return true;
}

/***************************************************************
  Function:
   static bool _mDNSSendRR(struct mDNSResourceRecord *record,
                   uint8_t record_type, uint32_t ttl_val,uint16_t query_id)

  Summary:
   Sends out a Multicast-DNS-Answer (Resource-Record) to
    Multicast-Address through pDNSdesc->mDNS_socket (UDP Socket).

  Description:
   This function is used in Announce-phase & Defend-Phase.

    In announce-phase the Host-Name or Resource-Record (Service)
    will be announced in local-network, so that neighbors can
    detect new-service or update their caches with new host-name
    to IP-Address mapping.

    In Defend-Phase, when _mDNSResponder receives a query for
    Host-name or Resounce-record for which this holds authority.

  Precondition:
   UDP socket (pDNSdesc->mDNS_socket) is obtained and ready for writing.

  Parameters:
   record - Resource-Record filled up with required info
    type   - Type of Res-Rec
    ttl_val - Time-To-Live value for Res-Record
    query_id - Query-ID for which this mDNS-answer (Res-Rec)
               corresponds to

  Returns:
     true - On Success
    false - On Failure (If UDP-Socket is invalid)
  **************************************************************/

static bool
_mDNSSendRR(mDNSResourceRecord *pRecord
          ,uint16_t query_id
          ,uint8_t cFlush
          ,uint16_t nAnswersInMsg
          ,bool bIsFirstRR
          ,bool bIsLastRR
          ,DNSDesc_t *pDNSdesc)
{
    MDNS_MSG_HEADER mDNS_header;
    TCPIP_UINT32_VAL ttl;
    uint8_t rec_length;
    uint8_t record_type;
    UDP_SOCKET_INFO sktInfo;

   record_type = pRecord->type.Val;

    if(pDNSdesc->mDNS_socket == INVALID_UDP_SOCKET)
    {
        WARN_MDNS_PRINT("_mDNSSendRR: Opening UDP Socket Failed \r\n");
      return false;
    }

   TCPIP_UDP_SocketInfoGet( pDNSdesc->mDNS_socket , &sktInfo);
   
   if (bIsFirstRR)
   {
      if(TCPIP_UDP_TxPutIsReady(pDNSdesc->mDNS_socket,sizeof(MDNS_MSG_HEADER)) < sizeof(MDNS_MSG_HEADER))
      {
          WARN_MDNS_PRINT("_mDNSSendRR: UDP Socket TX Busy \r\n");
        return false;
      }
      memset(&mDNS_header, 0, sizeof(MDNS_MSG_HEADER));

      mDNS_header.query_id.Val = TCPIP_Helper_htons(query_id);

      mDNS_header.flags.bits.qr = 1; // this is a Response,
      mDNS_header.flags.bits.aa = 1; // and we are authoritative
      mDNS_header.flags.Val = TCPIP_Helper_htons(mDNS_header.flags.Val);

      mDNS_header.nAnswers.Val = TCPIP_Helper_htons(nAnswersInMsg);

      // Put out the mDNS message header
      TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, (uint8_t *) &mDNS_header, sizeof(MDNS_MSG_HEADER));
   }

   ttl.Val = pRecord->ttl.Val;

   _mDNSPutString(pRecord->name,pDNSdesc);


   {

    uint8_t resourceRecord[] = {
        0x00, record_type, 0x00, 0x01,
        ttl.v[3], ttl.v[2], ttl.v[1], ttl.v[0]
    };
    if (sktInfo.remotePort == MDNS_PORT)
    {
      resourceRecord[2] = cFlush;
    }
    TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, resourceRecord, sizeof(resourceRecord));

   }
   switch (record_type)
   {
   case QTYPE_A:
   {
       uint8_t aRecord[] = {
           0x00, 0x04,
           pRecord->ip.v[0],
           pRecord->ip.v[1],
           pRecord->ip.v[2],
           pRecord->ip.v[3]
       };
        /*TCPIP_UDP_Put(pDNSdesc->mDNS_socket, 0x00);   // 0x0004 Data length
        TCPIP_UDP_Put(pDNSdesc->mDNS_socket, 0x04);
        TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pRecord->ip.v[0]);   // Put out IP address
        TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pRecord->ip.v[1]);
        TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pRecord->ip.v[2]);
        TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pRecord->ip.v[3]);*/
       TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, aRecord, sizeof(aRecord));
   }
      break;

   case QTYPE_PTR:
   {
        /* 2 bytes extra. One for Prefix Length for first-label.
         * Other one for NULL terminator */
        pRecord->rdlength.Val = strlen((char*)pRecord->rdata) + 2 ;

        //TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pRecord->rdlength.v[1]);
        //TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pRecord->rdlength.v[0]); // Res-Data Length. 0x4f
        {
            uint8_t ptrRecord[] = {
               pRecord->rdlength.v[1],
               pRecord->rdlength.v[0]
            };
            TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, ptrRecord, sizeof(ptrRecord));
        }
        _mDNSPutString(((mDNSProcessCtx_sd *) (pRecord->pOwnerCtx))->sd_qualified_name,pDNSdesc); //0x97
   }
      break;

   case QTYPE_SRV:
   {
        /* 2 bytes extra. One for Prefix Length for first-label.
         * Other one for NULL terminator */
        pRecord->rdlength.Val = strlen((char*)pRecord->rdata) + 2;
        pRecord->rdlength.Val += 6;               // for priority, weight, and port

        /*        TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pRecord->rdlength.v[1]);  // 0xee
        TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pRecord->rdlength.v[0]);      // Res-Data Length

        TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pRecord->srv.priority.v[1]);   // Put Priority
        TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pRecord->srv.priority.v[0]);
        TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pRecord->srv.weight.v[1]);
        TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pRecord->srv.weight.v[0]);
        TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pRecord->srv.port.v[1]);
        TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pRecord->srv.port.v[0]);
        */
        {
       uint8_t srvRecord[] = {
        pRecord->rdlength.v[1],  // 0xee
        pRecord->rdlength.v[0],      // Res-Data Length

        pRecord->srv.priority.v[1],   // Put Priority
        pRecord->srv.priority.v[0],
        pRecord->srv.weight.v[1],
        pRecord->srv.weight.v[0],
        pRecord->srv.port.v[1],
        pRecord->srv.port.v[0],
       };
            TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, srvRecord, sizeof(srvRecord));
        }
        _mDNSPutString(pRecord->rdata,pDNSdesc); // 0x120

   }
      break;

   case QTYPE_TXT:

        rec_length = strlen((char*)pRecord->rdata);

        pRecord->rdlength.Val = rec_length + 1;

        //TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pRecord->rdlength.v[1]); // 0x178
        //TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pRecord->rdlength.v[0]); // Res-Data Length
        //TCPIP_UDP_Put(pDNSdesc->mDNS_socket, pRecord->rdlength.Val-1); // As of now only single TXT string supported!!

        {
          uint8_t txtRecord[] = {
              pRecord->rdlength.v[1],
              pRecord->rdlength.v[0],
              pRecord->rdlength.Val-1
          };
          TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, txtRecord, sizeof(txtRecord));
        }

        if(rec_length>0)
        {
           TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, pRecord->rdata,rec_length); 
        }
      break;

   default:

        WARN_MDNS_PRINT("RR Type not supported \n");
   }

   if (bIsLastRR)
   {
       _mDNSSetAddresses(pDNSdesc);
       TCPIP_UDP_Flush(pDNSdesc->mDNS_socket);
   }

    return true;
}

static uint16_t _mDNSSendRRSize(mDNSResourceRecord *pRecord
          ,bool bIsFirstRR
)
{

    uint8_t rec_length;
    uint8_t record_type;

    uint16_t retValue = 0;

   record_type = pRecord->type.Val;

   if (bIsFirstRR)
   {
      retValue += sizeof(MDNS_MSG_HEADER);
   }

   retValue += _mDNSStringLength(pRecord->name);

   // Resource Record Type
   retValue += 10;

   switch (record_type)
   {
   case QTYPE_A:
        retValue += 6;
      break;

   case QTYPE_PTR:
        retValue += 2;
        retValue += _mDNSStringLength(((mDNSProcessCtx_sd *) (pRecord->pOwnerCtx))->sd_qualified_name); //0x97
      break;

   case QTYPE_SRV:
        retValue += 8;
        retValue += _mDNSStringLength(pRecord->rdata); // 0x120
      break;

   case QTYPE_TXT:
        rec_length = strlen((char*)pRecord->rdata);
        retValue += 3 + rec_length;
      break;

   default:

        WARN_MDNS_PRINT("RR Type not supported \n");
   }

    return retValue;
}
/***************************************************************
  Function:
   size_t _mDNSSDFormatServiceInstance(uint8_t *string, size_t strSize )

  Summary:
   Formats the Service-Instance name according to DNS-SD standard
    specification.

  Description:
   This function is used to format the Service-Instance name, if
    it contains 'dots' and 'backslashes'

    As the service-instance name will be merged with service-type &
    to distinguish the 'dots' seperating the service-type words and
    'dots' within service-instance name, the 'dots' within service-
    instance name will be replaced with '\.' in place of '.' Even the
    '\' are replaced with '\\'.

    When the resource-record containing service-instance name is
    pushed out, the formatted dots '\.' are sentout as '.' and the
    'dots' sperating the service-type & service-instances are replaced
    with length bytes, as specified in RFC 1035.

  Precondition:
   None

  Parameters:
   String - Service-Instance name to be formatted
    strSize - available size for the formatted string, not to be exceeded

  Returns:
     size of the formatted string
  **************************************************************/
static size_t _mDNSSDFormatServiceInstance(uint8_t *string, size_t strSize )
{
   uint8_t *temp;
   uint8_t output[MAX_LABEL_SIZE];
   uint8_t i;
   uint8_t *right_ptr,*str_token;
   uint8_t len;

   temp = output;
   right_ptr = string;
   str_token = string;
   while(1)
   {
      do
      {
         i = *right_ptr++;
      } while((i != 0x00u) && (i != '\\') && (i != '.') );


      /* Prefix '\' for every occurance of '.' & '\' */
      len = (uint8_t)(right_ptr-str_token-1);

      memcpy(temp,str_token,len);
      temp += len;
      str_token +=  len;
      if(i == '.' || i == '\\')
      {
         *temp = '\\';
         temp++;
         *temp++ = i;
         str_token += 1;

      }
      else if(i == 0x00u || i == '/' || i == ',' || i == '>')
         break;

   }
   *temp++ = '\0';
   return strncpy_m((char*)string, strSize, 1, output);
}

/***************************************************************
  Function:
   void _mDNSSDFillResRecords(mdnsd_struct *sd)

  Summary:
   Fills the resource-records with the information received from
    sd structure-instance, in which the information is filled from
    user input.

  Description:
   This function is used to fill the resource-records according to
    format specified in RFC 1035.

    In this context Service-Instance + Service-Type is called fully
    qualified name. For ex: Dummy HTTP Web-Server._http._tcp.local
    where Dummy HTTP Web-Server is Service-instance name
     and  _http._tcp.local is Service-Type

    Each service-instance that needs to be advertised contains three
    resource-reocrds.
    1) PTR Resource-Record: This is a shared record, with service-type
                           as rr-name and fully-qualified name as
                           rr-data.
    2) SRV Resource-Record: This is a unique record, with fully-
                            qualified name as rr-name and Host-name,
                            port-num as rr-data.
    3) TXT Resource-Record: This is a unique record, with fully-
                            qualified name as rr-name and additional
                            information as rr-data like default-page
                            name (For ex: "/index.htm")

  Precondition:
   None

  Parameters:
   sd - Service-Discovery structure instance for which Resource-
         records to be filled.

  Returns:
     None
  **************************************************************/
static void _mDNSSDFillResRecords(mDNSProcessCtx_sd *sd,DNSDesc_t *pDNSdesc)
{
    size_t srv_name_len,srv_type_len, qual_len;
    mDNSResourceRecord *rr_list;
    uint16_t serv_port;

    srv_name_len = strlen((char*)sd->srv_name);
    srv_type_len = strlen((char*)sd->srv_type);
    serv_port = pDNSdesc->mSDCtx.sd_port;

    memset(&(pDNSdesc->mResponderCtx.rr_list[QTYPE_PTR_INDEX]),0,(sizeof(mDNSResourceRecord)));
    memset(&(pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX]),0,(sizeof(mDNSResourceRecord)));
    memset(&(pDNSdesc->mResponderCtx.rr_list[QTYPE_TXT_INDEX]),0,(sizeof(mDNSResourceRecord)));


    /* Formatting Service-Instance name.
     * And preparing a fully qualified
     * Service-instance record . */


    strncpy((char*)sd->sd_qualified_name, (char*)sd->srv_name, sizeof(sd->sd_qualified_name));
    qual_len= _mDNSSDFormatServiceInstance(sd->sd_qualified_name, sizeof(sd->sd_qualified_name));
    strncpy_m((char*)&sd->sd_qualified_name[qual_len], sizeof(sd->sd_qualified_name) - qual_len, 2, ".", sd->srv_type);
    sd->sd_port = pDNSdesc->mSDCtx.sd_port = serv_port;

    /* Fill-up PTR Record */
    rr_list = &pDNSdesc->mResponderCtx.rr_list[QTYPE_PTR_INDEX];
    rr_list->type.Val = QTYPE_PTR;
    rr_list->name = (uint8_t *) (sd->srv_type);

    /* Res Record Name is
     * Service_Instance_name._srv-type._proto.domain */
   rr_list->rdata = (uint8_t *) (sd->sd_qualified_name);

    strncpy_m((char*)rr_list->rdata + srv_name_len, strlen((char*)sd->sd_qualified_name) - srv_name_len, 2, ".", sd->srv_type);

    /* 3 bytes extra. One for dot added between
     * Serv-Name and Serv-Type. One for length byte.
     * added for first-label in fully qualified name
     * Other one for NULL terminator */
    rr_list->rdlength.Val = srv_name_len+ srv_type_len + 3;
    rr_list->ttl.Val = RESOURCE_RECORD_TTL_VAL; /* Seconds. Not sure ! Need to check */
    rr_list->pOwnerCtx = (mDNSProcessCtx_common *) sd; /* Save back ptr */
    rr_list->valid = 1; /* Mark as valid */



      /* Fill-up SRV Record */
    rr_list = &pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX]; /* Move onto next entry */
    rr_list->name = (uint8_t *) (sd->sd_qualified_name);
    rr_list->type.Val = QTYPE_SRV;
    rr_list->ttl.Val = RESOURCE_RECORD_TTL_VAL;

    //rdlength is calculated/assigned last
    rr_list->srv.priority.Val = 0;
    rr_list->srv.weight.Val = 0;
    rr_list->srv.port.Val = pDNSdesc->mSDCtx.sd_port;

    /* Res Record Name is
     * Service_Instance_name._srv-type._proto.domain */
    rr_list->rdata = (uint8_t *) pDNSdesc->mHostCtx.szHostName;


    /* 2 bytes extra. One for Prefix Length for first-label.
     * Other one for NULL terminator */
   // then, add 6-byte extra: for priority, weight, and port

    rr_list->rdlength.Val = strlen((char*)rr_list->rdata)+2+6;
    rr_list->pOwnerCtx = (mDNSProcessCtx_common *) sd; /* Save back ptr */
    rr_list->valid = 1; /* Mark as valid */    




    /* Fill-up TXT Record with NULL data*/
    rr_list = &pDNSdesc->mResponderCtx.rr_list[QTYPE_TXT_INDEX]; /* Move onto next entry */
    rr_list->type.Val = QTYPE_TXT;
    rr_list->name = (uint8_t *) (sd->sd_qualified_name);

    /* Res Record data is what defined by the user */
    rr_list->rdata = (uint8_t *) (sd->sd_txt_rec);

    /* Extra byte for Length-Byte of TXT string */
    rr_list->rdlength.Val = pDNSdesc->mSDCtx.sd_txt_rec_len+1;
    rr_list->ttl.Val = RESOURCE_RECORD_TTL_VAL;
    rr_list->pOwnerCtx = (mDNSProcessCtx_common *) sd; /* Save back ptr */
    rr_list->valid = 1; /* Mark as valid */
}

MDNSD_ERR_CODE
TCPIP_MDNS_ServiceUpdate(TCPIP_NET_HANDLE netH, uint16_t port, const uint8_t* txt_record)
{
    mDNSProcessCtx_sd *sd;
    DNSDesc_t *pDNSdesc;
    TCPIP_NET_IF* pNetIf;

    pNetIf = _TCPIPStackHandleToNetUp(netH);
    if(pNetIf != 0)
    {
        pDNSdesc = gDNSdesc + TCPIP_STACK_NetIxGet(pNetIf);
        sd = &pDNSdesc->mSDCtx;

        if( sd->used)
        {
            sd->service_registered = 0;
            sd->sd_port = port;
            /* Update Port Value in SRV Resource-record */
            pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].srv.port.Val = port;

            if(txt_record != NULL)
            {
                sd->sd_txt_rec_len = strncpy_m((char*)sd->sd_txt_rec, sizeof(sd->sd_txt_rec), 1, (uint8_t *) txt_record );

                /* Update Resource-records for this
                 * Service-instance, in MDNS-SD state-
                 * -machine */
                _mDNSSDFillResRecords(sd,pDNSdesc);
                sd->common.state = MDNS_STATE_NOT_READY;
            }

            /* Notify MDNS Stack about Service-Registration
             * to get a time-slot for its own processing */
            sd->service_registered = 1;
            return MDNSD_SUCCESS;
        }
    }

    return MDNSD_ERR_INVAL;
}

MDNSD_ERR_CODE TCPIP_MDNS_ServiceDeregister(TCPIP_NET_HANDLE netH)
{
    DNSDesc_t *pDNSdesc;
    mDNSProcessCtx_sd *sd;
    TCPIP_NET_IF* pNetIf;

    pNetIf = _TCPIPStackHandleToNetUp(netH);
    if(pNetIf != 0)
    {
        pDNSdesc = gDNSdesc + TCPIP_STACK_NetIxGet(pNetIf);
        sd = &pDNSdesc->mSDCtx;

        if(sd->used)
        {
            if(sd->sd_service_advertised == 1)
            {
                /* Send GoodBye Packet */
                pDNSdesc->mResponderCtx.rr_list[QTYPE_PTR_INDEX].ttl.Val = 0;
                pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].ttl.Val = 0;
                pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].ttl.Val = 0;

                _mDNSSendRR(&pDNSdesc->mResponderCtx.rr_list[QTYPE_PTR_INDEX], 0, 0x00, 3, true,false,pDNSdesc);
                _mDNSSendRR(&pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX], 0, 0x80, 3, false,false,pDNSdesc);
                _mDNSSendRR(&pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX], 0, 0x80, 3, false,true,pDNSdesc);
            }
            /* Clear mSDCtx struct */
            sd->service_registered = 0;
            memset(sd,0,sizeof(mDNSProcessCtx_sd));
            return MDNSD_SUCCESS;
        }
    }

    return MDNSD_ERR_INVAL; /* Invalid Parameter */
}


MDNSD_ERR_CODE
TCPIP_MDNS_ServiceRegister( TCPIP_NET_HANDLE netH
                    ,const char *srv_name
                    ,const char *srv_type
                    ,uint16_t port
                    ,const uint8_t *txt_record
                    ,uint8_t auto_rename
                    ,void (*call_back)(char *name, MDNSD_ERR_CODE err, void *context)
                    ,void *context)
{
   DNSDesc_t *desc;
   TCPIP_NET_IF* pNetIf;

   if ( (srv_name == NULL) || (srv_type == NULL) || (txt_record == NULL) )
   {
       return MDNSD_ERR_INVAL; // Invalid Parameter
   }

    pNetIf = _TCPIPStackHandleToNetUp(netH);
    if(pNetIf != 0)
    {
        desc = gDNSdesc + TCPIP_STACK_NetIxGet(pNetIf);

        if(desc->mSDCtx.used)
        {
            return MDNSD_ERR_BUSY;
        }

        /* Clear the State-Machine */
        memset(&desc->mSDCtx,0,sizeof(mDNSProcessCtx_sd));
        desc->mSDCtx.used = 1; /* Mark it as used */
        desc->mSDCtx.sd_auto_rename = auto_rename;
        desc->mSDCtx.sd_port = port;
        desc->mSDCtx.sd_service_advertised = 0;

        strncpy((char*)desc->mSDCtx.srv_name
                , (char*)srv_name
                , sizeof(desc->mSDCtx.srv_name));

        strncpy((char*)desc->mSDCtx.srv_type
                , (char*)srv_type
                , sizeof(desc->mSDCtx.srv_type));

        desc->mSDCtx.sd_call_back = call_back;
        desc->mSDCtx.sd_context   = context;

        desc->mSDCtx.sd_txt_rec_len = strncpy_m((char*)desc->mSDCtx.sd_txt_rec
                ,sizeof(desc->mSDCtx.sd_txt_rec)
                ,1
                ,(uint8_t *) txt_record);

        /* Fill up Resource-records for this
         * Service-instance, in MDNS-SD state-
         * -machine */
        _mDNSSDFillResRecords(&desc->mSDCtx,desc);

        desc->mSDCtx.common.type  = MDNS_CTX_TYPE_SD;
        desc->mSDCtx.common.state = MDNS_STATE_NOT_READY;
        desc->mSDCtx.common.nInstanceId = 0;

        /* Notify MDNS Stack about Service-Registration
         * to get a time-slot for its own processing */
        desc->mSDCtx.service_registered = 1;
        return MDNSD_SUCCESS;
    }

   return MDNSD_ERR_INVAL; // unknown interface
}


/***************************************************************
  Function:
   void mDNSSDAnnounce(mdnsd_struct *sd)

  Summary:
   Sends out Multicast-DNS SD packet with SRV Resource-Record.

  Description:
   This function is used to send out DNS-SD SRV resource-record
    Announce packet for announcing the service-name on local network.
    This function makes use of _mDNSSendRR to send out DNS-Resource-
    Record with chosen service-name+service-type as rr-name and the
    host-name, port-number as rr-data.

    This announcement updates DNS-caches of neighbor machines on
    the local network.

  Precondition:
   None

  Parameters:
   sd - Service Discovery structure instance

  Returns:
     None
  **************************************************************/
static void _mDNSAnnounce(mDNSResourceRecord *pRR, DNSDesc_t *pDNSdesc)
{
    if( false ==
      _mDNSSendRR(pRR
                 ,0
                 ,0x80
                 ,1
                 ,true
                 ,true
                 ,pDNSdesc)
      )
   {
        WARN_MDNS_PRINT("_mDNSAnnounce: Error in sending out Announce pkt \r\n");
   }
}


static uint16_t _mDNSFetch(uint16_t wOffset, uint16_t wLen, uint8_t *pcString,DNSDesc_t *pDNSdesc)
{
   uint16_t rc;

   TCPIP_UDP_RxOffsetSet(pDNSdesc->mDNS_socket, wOffset);

   rc = TCPIP_UDP_ArrayGet(pDNSdesc->mDNS_socket, pcString, wLen);

   return rc;
}


/***************************************************************
  Function:
   static uint16_t _mDNSDeCompress(uint16_t wPos,
                               uint8_t *pcString,
                               bool bFollowPtr,
                               uint8_t cElement,
                               uint8_t cDepth)

  Summary:
   Read a string from a resource record, from the Multicast-DNS socket buffer.

  Description:
   This function reads a string to the Multicast-DNS socket,
    ensuring that it is properly formatted.

    String may be reconstructed through decompression if necessary.
   Decompression pointer traversal is done in place, recursively, in UDP's RxBuffer.

   cDepth represents the recursion depth, for debugging purpose.

   cElement represents the number of elements in the string. For example,
    "ezconfig._http._tcp.local" has 4 elements.

   bFollowPtr indicates if DNS compression offset needs to be followed. That is, if
   we should reconstruct a compressed string.

   The reconstructed string is placed in pcString, if it is not NULL.

   For DNS message compression format, see RFC 1035, section 4.1.4.

  Precondition:
   UDP socket is obtained and ready for writing.
    wPos correctly reflect the current position in the UDP RxBuffer.

  Parameters:
   String - the string to write to the UDP socket.

  Returns:
     Number of bytes in THIS resource record field (in RFC 1035's term, NAME or RDATA).
    UDP RxBuffer pointer is repositioned to the place right after THIS resource record field.

  **************************************************************/

static uint16_t _mDNSDeCompress(uint16_t wPos
                                   ,uint8_t *pcString
                                   ,bool bFollowPtr
                                   ,uint8_t cElement
                                   ,uint8_t cDepth
                                   ,DNSDesc_t *pDNSdesc)
{
   uint16_t rr_len = 0; // As is in the packet. Could be in compressed format.
   uint16_t startOffset, endOffset;
   uint8_t i, tmp;
   uint16_t offset_in_ptr;
   uint8_t substr_len;

   startOffset = wPos;

   while (1)
   {
      rr_len++;
      if(!TCPIP_UDP_Get(pDNSdesc->mDNS_socket, &substr_len))
         break;

      if(substr_len == 0u)
      {
         if (pcString)
         {
            *pcString++ = '\0';
         }
         break;
      }

      if((substr_len & 0xC0) == 0xC0)   // b'11 at MSb indicates compression ptr
      {
         offset_in_ptr = substr_len & 0x3F; // the rest of 6 bits is part of offset_in_ptr.
         offset_in_ptr = offset_in_ptr << 8;

         /* Remove label-ptr byte */
         rr_len++;
         TCPIP_UDP_Get(pDNSdesc->mDNS_socket, &i);
         offset_in_ptr += i;

         if (bFollowPtr)
         {
            cDepth++;

            TCPIP_UDP_RxOffsetSet(pDNSdesc->mDNS_socket, offset_in_ptr);
            _mDNSDeCompress(offset_in_ptr, pcString, bFollowPtr, cElement, cDepth,pDNSdesc);

            // compressed ptr is always the last element
            break;
         }

         break;
      }
      else
      {
         if (pcString)
         {
            if (cElement > 0)
            {
               // not the first element in name
               *pcString++ = '.';
            }

            TCPIP_UDP_ArrayGet(pDNSdesc->mDNS_socket, pcString, substr_len);
            pcString += substr_len;
         }
         else
         {
            i = substr_len;
            TCPIP_UDP_ArrayGet(pDNSdesc->mDNS_socket, &tmp, i);
            i = 0;
         }

         cElement++;
         rr_len += substr_len;
      }
   }

   endOffset = startOffset + rr_len;
   TCPIP_UDP_RxOffsetSet(pDNSdesc->mDNS_socket, endOffset);

   return rr_len;
}



static bool
_mDNSTieBreaker(mDNSResourceRecord *their, mDNSResourceRecord *our)
{
   bool WeWonTheTieBreaker = true;
   uint8_t i;

   if (their->type.Val == QTYPE_A)
   {
      for (i = 0; i<= 3; i++)
      {
         if (their->ip.v[i] < our->ip.v[i])
         {
            WeWonTheTieBreaker = true;
            break;
         }
         else if (their->ip.v[i] > our->ip.v[i])
         {
            WeWonTheTieBreaker = false;
            break;
         }
      }
   }
   else if (their->type.Val == QTYPE_SRV)
   {
      if (their->srv.port.Val >= our->srv.port.Val)
      {
         WeWonTheTieBreaker = false;
      }
   }

   DEBUG0_MDNS_PRINT( (char *) (WeWonTheTieBreaker ? "   tie-breaker won\r\n" : "   tie-breaker lost\r\n") );

   return WeWonTheTieBreaker;
}


static uint8_t
_mDNSProcessIncomingRR(MDNS_RR_GROUP     tag
                      ,MDNS_MSG_HEADER *pmDNSMsgHeader
                      ,uint16_t         idxGroup
                      ,uint16_t         idxRR
                      ,DNSDesc_t       *pDNSdesc)
{
   mDNSResourceRecord res_rec;
   uint8_t name[2 * MAX_RR_NAME_SIZE];  
   uint8_t i,j;
   uint16_t len;
   mDNSProcessCtx_common *pOwnerCtx;
   mDNSResourceRecord      *pMyRR;
   bool WeWonTheTieBreaker = false;
   bool bMsgIsAQuery;         // QUERY or RESPONSE ?
   bool bSenderHasAuthority;   // Sender has the authority ?
   uint8_t rxBuffer[6];

   bMsgIsAQuery = (pmDNSMsgHeader->flags.bits.qr == 0);
   bSenderHasAuthority = (pmDNSMsgHeader->flags.bits.aa == 1);

   res_rec.name = name; // for temporary name storage.

   // NAME
   memset(name, 0, sizeof(name));
   len = _mDNSDeCompress(pDNSdesc->mDNS_offset, name, true, 0, 0,pDNSdesc);
   pDNSdesc->mDNS_offset += len;

   // TYPE & CLASS
   TCPIP_UDP_ArrayGet(pDNSdesc->mDNS_socket, rxBuffer, 4);
   res_rec.type.v[1] = rxBuffer[0];
   res_rec.type.v[0] = rxBuffer[1];
   res_rec.class.v[1] = rxBuffer[2];
   res_rec.class.v[0] = rxBuffer[3];

   pDNSdesc->mDNS_offset += 4;

   // Do the first round name check
   for (i = 0; i < MAX_RR_NUM; i++)
   {
      pDNSdesc->mResponderCtx.rr_list[i].bNameAndTypeMatched = false;

      if (
         !_strcmp_local_ignore_case((void *)name, pDNSdesc->mResponderCtx.rr_list[i].name)
         &&
         ((res_rec.type.Val == QTYPE_ANY) ||
          (res_rec.type.Val == pDNSdesc->mResponderCtx.rr_list[i].type.Val))
         )
      {
         pDNSdesc->mResponderCtx.rr_list[i].bNameAndTypeMatched = true;
      }
      else if (
         (tag == MDNS_RR_GROUP_QD)
         &&
         !_strcmp_local_ignore_case(name,(uint8_t *) "_services._dns-sd._udp.local")
         &&
         (res_rec.type.Val == QTYPE_PTR)
         )
      {
         pDNSdesc->mResponderCtx.rr_list[i].bNameAndTypeMatched = true;
      }
   }


   // Only AN, NS, AR records have extra fields
   if ( tag != MDNS_RR_GROUP_QD )
   {

       // Now retrieve those extra fields
       TCPIP_UDP_ArrayGet(pDNSdesc->mDNS_socket, rxBuffer, 6);
       res_rec.ttl.v[3] = rxBuffer[0];
       res_rec.ttl.v[2] = rxBuffer[1];
       res_rec.ttl.v[1] = rxBuffer[2];
       res_rec.ttl.v[0] = rxBuffer[3];
       res_rec.rdlength.v[1] = rxBuffer[4];
       res_rec.rdlength.v[0] = rxBuffer[5];

       pDNSdesc->mDNS_offset += 6;

       // The rest is record type dependent
       switch (res_rec.type.Val)
       {
       case QTYPE_A:
          TCPIP_UDP_ArrayGet(pDNSdesc->mDNS_socket, &res_rec.ip.v[0], 4);

          pDNSdesc->mDNS_offset += 4;

          break;

       case QTYPE_PTR:

          memset(name, 0 , sizeof(name));
          len = _mDNSDeCompress(pDNSdesc->mDNS_offset, name, true, 0, 0,pDNSdesc);
          pDNSdesc->mDNS_offset += len;

          break;

       case QTYPE_SRV:
          TCPIP_UDP_ArrayGet(pDNSdesc->mDNS_socket, rxBuffer, 6);
          res_rec.srv.priority.v[1] = rxBuffer[0];
          res_rec.srv.priority.v[0] = rxBuffer[1];
          res_rec.srv.weight.v[1] = rxBuffer[2];
          res_rec.srv.weight.v[0] = rxBuffer[3];
          res_rec.srv.port.v[1] = rxBuffer[4];
          res_rec.srv.port.v[0] = rxBuffer[5];

          pDNSdesc->mDNS_offset += 6;

          memset(name, 0 , sizeof(name));
          len = _mDNSDeCompress(pDNSdesc->mDNS_offset, name, true, 0, 0,pDNSdesc);
          pDNSdesc->mDNS_offset += len;

          break;

       case QTYPE_TXT:
       default:

          // Still needs to read it off
          TCPIP_UDP_ArrayGet(pDNSdesc->mDNS_socket, NULL, res_rec.rdlength.Val);

          pDNSdesc->mDNS_offset += res_rec.rdlength.Val;
          break;
       }

       // We now have all info about this received RR.
   }

   // Do the second round
   for (i = 0; i < MAX_RR_NUM; i++)
   {
      pMyRR = &(pDNSdesc->mResponderCtx.rr_list[i]);
      pOwnerCtx = pDNSdesc->mResponderCtx.rr_list[i].pOwnerCtx;

      if ( (!pMyRR->bNameAndTypeMatched) || (pOwnerCtx == NULL) )
      {
         // do nothing
            TCPIP_UDP_Discard(pDNSdesc->mDNS_socket);
      }
      else if (
         bMsgIsAQuery &&
         (tag == MDNS_RR_GROUP_QD) &&
         (pOwnerCtx->state == MDNS_STATE_DEFEND)
         )
      {
         // Simple reply to an incoming DNS query.
         // Mark all of our RRs for reply.

         for (j = 0; j < MAX_RR_NUM; j++)
         {
            pDNSdesc->mResponderCtx.rr_list[j].bResponseRequested = true;
         }
      }
      else if (
         bMsgIsAQuery &&
         (tag == MDNS_RR_GROUP_AN) &&
         (pOwnerCtx->state == MDNS_STATE_DEFEND)
         )
      {
         // An answer in the incoming DNS query.
         // Look for possible duplicate (known) answers suppression.
         if ((((res_rec.type.Val == QTYPE_PTR) && (res_rec.ip.Val == pDNSdesc->mResponderCtx.rr_list[i].ip.Val))
             ||
            (!_strcmp_local_ignore_case(name, pDNSdesc->mResponderCtx.rr_list[i].rdata)))
            &&
            (res_rec.ttl.Val > (pDNSdesc->mResponderCtx.rr_list[i].ttl.Val/2))
            )
         {
            pDNSdesc->mResponderCtx.rr_list[i].bResponseSuppressed = true;
            DEBUG_MDNS_PRINT("     rr suppressed\r\n");
         }
      }
      else if (
         bMsgIsAQuery &&
         (tag == MDNS_RR_GROUP_NS) &&
         ((pOwnerCtx->state == MDNS_STATE_PROBE) ||
          (pOwnerCtx->state == MDNS_STATE_ANNOUNCE))
         )
      {
         // Simultaneous probes by us and sender of this DNS query.
         // Mark as a conflict ONLY IF we lose the Tie-Breaker.

         WeWonTheTieBreaker = _mDNSTieBreaker(&res_rec,
                                    &(pDNSdesc->mResponderCtx.rr_list[i]));

         if (!WeWonTheTieBreaker)
         {
            pOwnerCtx->bProbeConflictSeen = true;
            pOwnerCtx->nProbeConflictCount++;
         }

         TCPIP_UDP_Discard(pDNSdesc->mDNS_socket);

         return 0;
      }
      else if (
         !bMsgIsAQuery             &&
         bSenderHasAuthority       &&
         (tag == MDNS_RR_GROUP_AN) &&
         ((pOwnerCtx->state == MDNS_STATE_PROBE) ||
          (pOwnerCtx->state == MDNS_STATE_ANNOUNCE))
         )
      {
         // An authoritative DNS response to our probe/announcement.
         // Mark as a conflict. Effect a re-name, followed by a
         // re-probe.

         pOwnerCtx->bProbeConflictSeen = true;
         pOwnerCtx->nProbeConflictCount++;

         TCPIP_UDP_Discard(pDNSdesc->mDNS_socket);

         return 0;
      }
      else if(bMsgIsAQuery             &&
             (tag == MDNS_RR_GROUP_NS) &&
             (pOwnerCtx->state == MDNS_STATE_DEFEND)
         )
      {
         // A probe by the sender conflicts with our established record.
         // Need to defend our record. Effect a DNS response.

         INFO_MDNS_PRINT("Defending RR: \r\n");

         pMyRR->bResponseRequested = true;

         TCPIP_UDP_Discard(pDNSdesc->mDNS_socket);

         return 0;
      }
      else if (
         !bMsgIsAQuery &&
          bSenderHasAuthority &&
         (tag == MDNS_RR_GROUP_AN) &&
         (pMyRR->type.Val != QTYPE_PTR ) &&      // No one can claim authority on shared RR
         (pOwnerCtx->state == MDNS_STATE_DEFEND)
         )
      {
         // Sender claims that it also has the authority on
         // a unique (non-shared) record that we have already established authority.
         // Effect a re-probe.

         pOwnerCtx->bLateConflictSeen = true;

         TCPIP_UDP_Discard(pDNSdesc->mDNS_socket);

         return 0;
      }
   }
   return 0;
}


/***************************************************************
  Function:
   static void _mDNSResponder(DNSDesc_t *pDNSdesc)

  Summary:
   Acts as Multicast-DNS responder & replies when it receives
    a query. Currenlty only supports IP_ADDRESS_TYPE_IPV4 addressing

  Description:
   This function is used as mDNS-Responder. On initialization of
    Multicast-DNS stack, this function Opens up pDNSdesc->mDNS_socket
    (UDP-Socket) for Mulitcast-Address (224.0.0.251).

    This function gets polled from TCPIP_MDNS_Task for every iteration.
    _mDNSResponder constantly monitors the packets being sent to
    Multicast-Address, to check whether it is a conflict with
    its own host-name/resource-record names. It also verifies
    whether incoming query is for its own Host-name/Resource-
    Record, in which case it sends back a reply with corresponding
    Resource-Record.

  Precondition:
   UDP socket (pDNSdesc->mDNS_socket) is obtained and ready for writing.
    A UDP socket must be available before this function is called.
    UDP_MAX_SOCKETS may need to be increased if other modules use
    UDP sockets.

  Parameters:
   None

  Returns:
     None
  **************************************************************/
static void _mDNSResponder(DNSDesc_t *pDNSdesc)
{
   MDNS_MSG_HEADER mDNS_header;
   uint16_t len;
   uint16_t i,j,count;
   uint16_t rr_count[4];
   MDNS_RR_GROUP rr_group[4];
   bool bMsgIsComplete;
   uint16_t packetSize = 0;
   uint16_t bufferSize = 0;


   pDNSdesc->mDNS_offset = 0;

   if(pDNSdesc->mDNS_socket == INVALID_UDP_SOCKET)
   {
        pDNSdesc->mDNS_responder_state = MDNS_RESPONDER_INIT;
   }

   switch(pDNSdesc->mDNS_responder_state)
   {
      case MDNS_RESPONDER_INIT:

         pDNSdesc->mDNS_socket = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_IPV4, MDNS_PORT, 0);
         if(pDNSdesc->mDNS_socket == INVALID_UDP_SOCKET)
         {
            WARN_MDNS_PRINT("_mDNSResponder: Can't open Multicast-DNS UDP-Socket \r\n");
            return;
         }
         else
         {
             TCPIP_UDP_SocketNetSet(pDNSdesc->mDNS_socket, pDNSdesc->mTcpIpNetIf);
             TCPIP_UDP_OptionsSet(pDNSdesc->mDNS_socket, UDP_OPTION_STRICT_NET, (void*)true);
             TCPIP_UDP_SignalHandlerRegister(pDNSdesc->mDNS_socket, TCPIP_UDP_SIGNAL_RX_DATA, _mDNSSocketRxSignalHandler, 0);
             pDNSdesc->mDNS_responder_state = MDNS_RESPONDER_LISTEN ;
         }

            /* Called from TCPIP_MDNS_Initialize. So return immediately */
            break;

      case MDNS_RESPONDER_LISTEN:

         // Do nothing if no data is waiting
         if(!TCPIP_UDP_GetIsReady(pDNSdesc->mDNS_socket))
            return;

            /* Reset the Remote-node information in UDP-socket */
            TCPIP_UDP_RemoteBind(pDNSdesc->mDNS_socket, IP_ADDRESS_TYPE_ANY, MDNS_PORT, 0); 
            TCPIP_UDP_Bind(pDNSdesc->mDNS_socket, IP_ADDRESS_TYPE_ANY, MDNS_PORT, 0); 

         // Retrieve the mDNS header
         len = _mDNSFetch(0, sizeof(mDNS_header), (uint8_t *) &mDNS_header,pDNSdesc);
         mDNS_header.query_id.Val = TCPIP_Helper_ntohs(mDNS_header.query_id.Val);
         mDNS_header.flags.Val = TCPIP_Helper_ntohs(mDNS_header.flags.Val);
         mDNS_header.nQuestions.Val = TCPIP_Helper_ntohs(mDNS_header.nQuestions.Val);
         mDNS_header.nAnswers.Val = TCPIP_Helper_ntohs(mDNS_header.nAnswers.Val);
         mDNS_header.nAuthoritativeRecords.Val = TCPIP_Helper_ntohs(mDNS_header.nAuthoritativeRecords.Val);
         mDNS_header.nAdditionalRecords.Val = TCPIP_Helper_ntohs(mDNS_header.nAdditionalRecords.Val);

         pDNSdesc->mDNS_offset += len; // MUST BE 12

         if ( (mDNS_header.flags.bits.qr == 0) )
         {
            DEBUG0_MDNS_PRINT("rx QUERY \r\n");
         }
         else
         {
            DEBUG0_MDNS_PRINT("rx RESPONSE \r\n");
         }

         bMsgIsComplete = (mDNS_header.flags.bits.tc == 0);  // Message is not truncated.

         rr_count[0] = mDNS_header.nQuestions.Val;
         rr_group[0] = MDNS_RR_GROUP_QD;

         rr_count[1] = mDNS_header.nAnswers.Val;
         rr_group[1] = MDNS_RR_GROUP_AN;

         rr_count[2] = mDNS_header.nAuthoritativeRecords.Val;
         rr_group[2] = MDNS_RR_GROUP_NS;

         rr_count[3] = mDNS_header.nAdditionalRecords.Val;
         rr_group[3] = MDNS_RR_GROUP_AR;

         for (i = 0; i < MAX_RR_NUM; i++)
         {
            // Reset flags
            pDNSdesc->mResponderCtx.rr_list[i].bNameAndTypeMatched = false;

            if (pDNSdesc->mResponderCtx.bLastMsgIsIncomplete)
            {
               // Do nothing.
               // Whether a reply is needed is determined only when all parts
               // of the message are received.

               // Ideally, we want to verify that the current message is the
               // continuation of the previous message.
               // Don't have a cost-effective way to do this yet.
            }
            else
            {
               // Start of a new message

               pDNSdesc->mResponderCtx.rr_list[i].bResponseRequested = false;
               pDNSdesc->mResponderCtx.rr_list[i].bResponseSuppressed = false;
               pDNSdesc->mResponderCtx.rr_list[i].srv.port.Val=pDNSdesc->mSDCtx.sd_port;
            }
         }

         for (i=0; i<4; i++) // for all 4 groups: QD, AN, NS, AR
         {
            for(j=0; j < rr_count[i]; j++)      // RR_count = {#QD, #AN, #NS, #AR}
            {
               _mDNSProcessIncomingRR(rr_group[i]
                                    ,&mDNS_header
                                    ,i
                                    ,j
                                    ,pDNSdesc);
            }
         }

         // Record the fact, for the next incoming message.
         pDNSdesc->mResponderCtx.bLastMsgIsIncomplete = (bMsgIsComplete == false);

         // Do not reply any answer if the current message is not the last part of
         // the complete message.
         // Future parts of the message may request some answers be suppressed.

         if (!bMsgIsComplete)
         {
            DEBUG0_MDNS_PRINT("   truncated msg.\r\n");
            return;
         }

         // Count all RRs marked as "reply needed".
         count = 0;
         for (i = 0; i < MAX_RR_NUM; i++)
         {
            if ((pDNSdesc->mResponderCtx.rr_list[i].pOwnerCtx != NULL) &&
               (pDNSdesc->mResponderCtx.rr_list[i].pOwnerCtx->state == MDNS_STATE_DEFEND) &&
               (pDNSdesc->mResponderCtx.rr_list[i].bResponseRequested == true) &&
               (pDNSdesc->mResponderCtx.rr_list[i].bResponseSuppressed == false)
               )
            {
                if (count == 0)
                {
                    packetSize = _mDNSSendRRSize(&pDNSdesc->mResponderCtx.rr_list[i], true);
                    //SYS_CONSOLE_PRINT("Packet Size %d count = %d\r\n", packetSize, count);
                }
                else
                {
                    packetSize += _mDNSSendRRSize(&pDNSdesc->mResponderCtx.rr_list[i], false);
                    //SYS_CONSOLE_PRINT("Packet Size %d count = %d\r\n", packetSize, count);

                }
               count++;
            }
         }

         // Send all RRs marked as "reply needed".

        _mDNSSetAddresses(pDNSdesc);

        if (TCPIP_UDP_OptionsGet(pDNSdesc->mDNS_socket, UDP_OPTION_TX_BUFF, &bufferSize))
        {
            //SYS_CONSOLE_PRINT("Buffer Size %d\r\n", bufferSize);
            if (bufferSize < packetSize)
            {
                if (!TCPIP_UDP_OptionsSet(pDNSdesc->mDNS_socket, UDP_OPTION_TX_BUFF, (void*)(unsigned int)(packetSize + 10)))
                {
                    DEBUG0_MDNS_PRINT("   buffer too small\r\n");
                    return;
                }
            }
        }
        else
        {
            DEBUG0_MDNS_PRINT("   could not get buffer info\r\n");
            return;

        }


         j = 1;
         for (i = 0; (count > 0) && (i < MAX_RR_NUM); i++)
         {
            if ((pDNSdesc->mResponderCtx.rr_list[i].pOwnerCtx != NULL) &&
               (pDNSdesc->mResponderCtx.rr_list[i].pOwnerCtx->state == MDNS_STATE_DEFEND) &&
               (pDNSdesc->mResponderCtx.rr_list[i].bResponseRequested == true) &&
               (pDNSdesc->mResponderCtx.rr_list[i].bResponseSuppressed == false) )
            {
               _mDNSSendRR(&pDNSdesc->mResponderCtx.rr_list[i]
                         ,mDNS_header.query_id.Val
                         ,(pDNSdesc->mResponderCtx.rr_list[i].type.Val == QTYPE_PTR)?(0x00):(0x80) // flush, except for PTR; for Conformance Test.
                         ,count                           // MAX_RR_NUM answers;
                         ,(j==1)?true:false               // Is this the first RR?
                         ,(j==count)?true:false
                         ,pDNSdesc);         // Is this the last RR?

               j++;
            }
         }

          if (bufferSize < packetSize)
          {
            TCPIP_UDP_OptionsSet(pDNSdesc->mDNS_socket, UDP_OPTION_TX_BUFF, (void*)(unsigned int)(bufferSize));
          }

         // end of MDNS_RESPONDER_LISTEN
         break;

      default:
         break;
   }

   return;
}




/***************************************************************
  Function:
   void void _mDNSFillHostRecord(DNSDesc_t *pDNSdesc)

  Summary:


  Description:

  Precondition:
   None

  Parameters:
   None

  Returns:
     None
  **************************************************************/
static void _mDNSFillHostRecord(DNSDesc_t *pDNSdesc)
{
   uint8_t i;

   // Fill the type A resource record
   pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].name     = pDNSdesc->mHostCtx.szHostName;
   pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].type.Val = QTYPE_A; // Query Type is Answer
   // CLASS 1=INternet, 255=Any class etc
   pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].ttl.Val  = RESOURCE_RECORD_TTL_VAL;

   pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].rdlength.Val = 4u; // 4-byte for IP address

   // Fill in the data for an A RR record (IP address)
   for (i=0; i<=3; i++)
      pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].ip.v[i] = pDNSdesc->mTcpIpNetIf->netIPAddr.v[i];

   pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].valid    = 1;
   pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].pOwnerCtx = (mDNSProcessCtx_common *) &pDNSdesc->mHostCtx;
}



static MDNSD_ERR_CODE _mDNSHostRegister( char *host_name,DNSDesc_t *pDNSdesc)
{

   memcpy((char*)pDNSdesc->mHostCtx.szUserChosenHostName
          , host_name
          , sizeof(pDNSdesc->mHostCtx.szUserChosenHostName));

   strncpy_m((char*)pDNSdesc->mHostCtx.szHostName
            , sizeof(pDNSdesc->mHostCtx.szHostName)
            , 3
            , pDNSdesc->mHostCtx.szUserChosenHostName
            , "."
            , pDNSdesc->CONST_STR_local);

   pDNSdesc->mHostCtx.szUserChosenHostName[MAX_HOST_NAME_SIZE-1]=0;
   pDNSdesc->mHostCtx.szHostName[MAX_HOST_NAME_SIZE-1]=0;

   _mDNSCountersReset((mDNSProcessCtx_common *) &pDNSdesc->mHostCtx, true);
   pDNSdesc->mHostCtx.common.type   = MDNS_CTX_TYPE_HOST;
   pDNSdesc->mHostCtx.common.state  = MDNS_STATE_INIT;
   pDNSdesc->mHostCtx.common.nInstanceId = 0;

   // Now create a QTYPE_A record for later use when answering queries.
   _mDNSFillHostRecord(pDNSdesc);
   pDNSdesc->mResponderCtx.bLastMsgIsIncomplete = false;

   pDNSdesc->mHostCtx.common.state = MDNS_STATE_INIT;

   return MDNSD_SUCCESS;
}


// Returns true on success
bool TCPIP_MDNS_Initialize( const TCPIP_STACK_MODULE_CTRL* const stackCtrl
                    ,const TCPIP_DNS_CLIENT_MODULE_CONFIG* dnsData)
{
    DNSDesc_t *desc;
    TCPIP_NET_IF*  pNetIf;
    char ServiceName[64];
    char*   hostName;
    int32_t n;

    if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }

    // stack init

    if(mDNSInitCount == 0)
    {   // first time we're run

        // Alocate memory for a memory block of descriptors. Once descriptor per interface
        gDNSdesc = (DNSDesc_t*)TCPIP_HEAP_Calloc(stackCtrl->memH, stackCtrl->nIfs, sizeof(DNSDesc_t));
        if(gDNSdesc == (DNSDesc_t*)0)
        {
            SYS_ERROR(SYS_ERROR_ERROR, "TCPIP_MDNS_Initialize: Failed to allocate memory\r\n");
            return false;
        }
        mdnsSignalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_MDNS_Task, MDNS_TASK_TICK_RATE);
        if(mdnsSignalHandle == 0)
        {   // cannot create the MDNS timer
            TCPIP_HEAP_Free(stackCtrl->memH, gDNSdesc);  // free the allocated memory
            gDNSdesc = 0;
            return false;
        }
    }


    pNetIf = stackCtrl->pNetIf;

    // Remove any trailing spaces from ther NetBiosName
    strncpy(ServiceName, (char*)pNetIf->NetBIOSName, sizeof(ServiceName) - 1);
    n = strlen((char*)pNetIf->NetBIOSName);
    ServiceName[n] = 0; // NULL Terminate in advance
    for( ; n >= 0; n--)
    {
        if(isalnum(ServiceName[n]) || ispunct(ServiceName[n]))
        {
            break;
        }
        else
        {
           ServiceName[n]=0;
        }
    }

    desc = gDNSdesc+ stackCtrl->netIx;
    desc->mTcpIpNetIf = pNetIf;
    desc->mResponderCtx.query_id.Val = 0;
    desc->mResponderCtx.prev_ipaddr.Val = desc->mTcpIpNetIf->netIPAddr.Val;

    strncpy(desc->CONST_STR_local,"local",6);

    hostName = 0;
    if(ServiceName[0] != 0)
    {
        hostName = ServiceName;
    }
#if defined MDNS_DEFAULT_HOST_NAME
    else
    {
        hostName = MDNS_DEFAULT_HOST_NAME;
    }
#endif  // defined MDNS_DEFAULT_HOST_NAME
    // Register the hostname with each descriptor
    if(hostName)
    {
        _mDNSHostRegister(hostName, desc);
    }

    /* Initialize MDNS-Responder by opening up Multicast-UDP-Socket */
    desc->mHostCtx.common.state = MDNS_STATE_INIT;
    desc->mDNS_socket = INVALID_UDP_SOCKET;

    mDNSInitCount++;

    return true;
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_MDNS_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN) // interface down

    // one way or another this interface is going down
    if(mDNSInitCount > 0)
    {   // we're up and running
        DNSDesc_t *pDNSdesc = gDNSdesc + stackCtrl->netIx;
        
        if(pDNSdesc->mDNS_socket != INVALID_UDP_SOCKET)
        {
            TCPIP_UDP_Close(pDNSdesc->mDNS_socket);
            pDNSdesc->mDNS_socket = INVALID_UDP_SOCKET;
        }
        pDNSdesc->MDNS_flags.val = 0;

        if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--mDNSInitCount == 0)
            {   // all closed
                // release resources
                TCPIP_HEAP_Free(stackCtrl->memH, gDNSdesc);  // free the allocated memory
                gDNSdesc = 0;
                if(mdnsSignalHandle)
                {
                    _TCPIPStackSignalHandlerDeregister(mdnsSignalHandle);
                    mdnsSignalHandle = 0;
                }
            }
        }
    }

}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)


static void _mDNSProcessInternal(mDNSProcessCtx_common *pCtx, DNSDesc_t *pDNSdesc)
{
    bool bIsHost = (((void *) pCtx) == ((void *) &pDNSdesc->mHostCtx));

    switch (pCtx->state)
    {
        case MDNS_STATE_HOME:

            DEBUG_MDNS_PRINT("MDNS_STATE_HOME: Wrong state \r\n");
            break;

        case MDNS_STATE_NOT_READY: // SD starts from here. SD only.

            if (pDNSdesc->mHostCtx.common.state != MDNS_STATE_DEFEND)
            {
                /* Multicast DNS is not ready */
                return;
            }
            else
            {
                /* Multicast DNS is ready now */
                pCtx->state = MDNS_STATE_INIT;
                pCtx->time_recorded = 0;
            }

            INFO_MDNS_PRINT("\r\nMDNS_STATE_NOT_READY --> MDNS_STATE_INIT \r\n");
            break;

        case MDNS_STATE_INTF_NOT_CONNECTED: // HOST starts from here. HOST only.
            if (!TCPIP_STACK_NetworkIsLinked(pDNSdesc->mTcpIpNetIf))
                return;
            else
            {
                /* Interface is connected now */
                pCtx->state = MDNS_STATE_IPADDR_NOT_CONFIGURED;
                pCtx->time_recorded = 0;
            }

            // No break. Fall through

        case MDNS_STATE_IPADDR_NOT_CONFIGURED: // HOST only.
        {
            // Wait until IP addr is configured ...
            if (pDNSdesc->mTcpIpNetIf->netIPAddr.Val == 0)
                break;

            pCtx->state = MDNS_STATE_INIT;
            pCtx->time_recorded = 0;

            INFO_MDNS_PRINT("MDNS_STATE_IPADDR_NOT_CONFIGURED --> MDNS_STATE_INIT \r\n");

            // No break. Fall through
        }

        case MDNS_STATE_INIT:
        {
            pCtx->bConflictSeenInLastProbe = false;

            switch (zgzc_wait_for(&(pCtx->random_delay), &(pCtx->event_time), &(pCtx->time_recorded)))
            {
                case ZGZC_STARTED_WAITING:

                    // Need to choose Random time between 0-MDNS_PROBE_WAIT msec

                    // Intentional fall-through

                case ZGZC_KEEP_WAITING:

                    // Not Completed the delay proposed
                    return;
            }

            // Completed the delay required

            // Clear all counters
            _mDNSCountersReset(pCtx, true);

            pCtx->state = MDNS_STATE_PROBE;
            INFO_MDNS_PRINT("MDNS_STATE_INIT --> MDNS_STATE_PROBE \r\n");

            // No break. Fall through
        }

        case MDNS_STATE_PROBE:
        case MDNS_STATE_ANNOUNCE:
        {
            if (pCtx->bProbeConflictSeen)
            {
                pCtx->bConflictSeenInLastProbe = true;

                INFO_MDNS_PRINT("Conflict detected. Will rename\r\n");

                /* Conflict with selected name */
                pCtx->state = MDNS_STATE_PROBE;

                // Do not reset nProbeConflictCount if in PROBE state
                _mDNSCountersReset(
                        pCtx,
                        (pCtx->state == MDNS_STATE_PROBE) ? false : true
                        );

                if (bIsHost)
                {
                    // Rename host name
                    _mDNSRename(pDNSdesc->mHostCtx.szUserChosenHostName
                            , ++(pDNSdesc->mHostCtx.common.nInstanceId)
                            , (uint8_t *) pDNSdesc->CONST_STR_local
                            , pDNSdesc->mHostCtx.szHostName
                            , MAX_HOST_NAME_SIZE);

                }
                else
                {
                    // Rename service instance name
                    if (pDNSdesc->mSDCtx.sd_auto_rename)
                    {
                        _mDNSRename(pDNSdesc->mSDCtx.srv_name
                                , ++pDNSdesc->mSDCtx.common.nInstanceId
                                , pDNSdesc->mSDCtx.srv_type
                                , pDNSdesc->mSDCtx.sd_qualified_name
                                , MAX_LABEL_SIZE);

                        /* Reset Multicast-UDP socket */
                        TCPIP_UDP_Close(pDNSdesc->mDNS_socket);
                        pDNSdesc->mDNS_socket = INVALID_UDP_SOCKET;
                        _mDNSResponder(pDNSdesc);
                    }
                    else
                    {
                        pDNSdesc->mSDCtx.service_registered = 0;

                        pDNSdesc->mSDCtx.used = 0;
                        if (pDNSdesc->mSDCtx.sd_call_back != NULL)
                        {
                            pDNSdesc->mSDCtx.sd_call_back((char *) pDNSdesc->mSDCtx.srv_name,
                                    MDNSD_ERR_CONFLICT,
                                    pDNSdesc->mSDCtx.sd_context);
                        }
                    }
                }
                break;
            }


            while (1)
            {
                switch (zgzc_wait_for(&(pCtx->random_delay), &(pCtx->event_time), &(pCtx->time_recorded)))
                {
                    case ZGZC_STARTED_WAITING:

                        if (pCtx->state == MDNS_STATE_PROBE)
                        {
                            if (((pCtx->nProbeCount >= MDNS_PROBE_NUM) && !pCtx->bConflictSeenInLastProbe) ||
                                    (pCtx->nProbeConflictCount >= MDNS_MAX_PROBE_CONFLICT_NUM))
                            {
                                /* Move onto Announce Step */
                                pCtx->state = MDNS_STATE_ANNOUNCE;
                                pCtx->bConflictSeenInLastProbe = false;

                                INFO_MDNS_PRINT("MDNS_STATE_PROBE --> MDNS_STATE_ANNOUNCE \r\n");

                                return;
                            }
                        }
                        else
                        {
                            // We are in MDNS_STATE_ANNOUNCE

                            if (pCtx->nClaimCount >= MDNS_ANNOUNCE_NUM)
                            {
                                /* Finalize mDNS Host-name, Announced */
                                pCtx->state = MDNS_STATE_DEFEND;

                                if (bIsHost)
                                {
                                    INFO_MDNS_PRINT("MDNS_STATE_ANNOUNCE --> MDNS_STATE_DEFEND \r\n");
                                }
                                else
                                {
                                    INFO_MDNS_PRINT("\r\nZeroConf: Service = ");
                                    INFO_MDNS_PRINT((char*) pDNSdesc->mSDCtx.sd_qualified_name);
                                    INFO_MDNS_PRINT("\r\n");

                                    INFO_MDNS_PRINT("MDNS_STATE_ANNOUNCE --> MDNS_STATE_DEFEND \r\n");

                                    _mDNSSendRR(&pDNSdesc->mResponderCtx.rr_list[QTYPE_PTR_INDEX]
                                            , 0
                                            , 0x00
                                            , 1
                                            , true
                                            , true
                                            , pDNSdesc); // This produces a bad PTR rec for MCHPDEMO.local

                                    pDNSdesc->mSDCtx.sd_service_advertised = 1;
                                    if (pDNSdesc->mSDCtx.sd_call_back != NULL)
                                    {
                                        pDNSdesc->mSDCtx.sd_call_back((char *) pDNSdesc->mSDCtx.srv_name
                                                , MDNSD_SUCCESS
                                                , pDNSdesc->mSDCtx.sd_context);
                                    }
                                }
                                _mDNSCountersReset(pCtx, true);

                                return;
                            }
                        }

                        if (pCtx->state == MDNS_STATE_PROBE)
                        {
                            // Send out Probe packet
                            _mDNSProbe(pCtx, pDNSdesc);

                            pCtx->nProbeCount++;
                            pCtx->bConflictSeenInLastProbe = false;

                            /* Need to set timeout for MDNS_PROBE_INTERVAL msec */
                            if (pCtx->nProbeConflictCount < 9) // less-than-10 is required to pass Bonjour Conformance test.
                            {
                                pCtx->random_delay = (MDNS_PROBE_INTERVAL * (SYS_TMR_TickCounterFrequencyGet() / 1000));
                            }
                            else
                            {
                                pCtx->random_delay = (SYS_TMR_TickCounterFrequencyGet());
                            }

                            return;
                        }

                        // We are in MDNS_STATE_ANNOUNCE

                        /* Announce Name chosen on Local Network */

                        _mDNSAnnounce(&pDNSdesc->mResponderCtx.rr_list[(bIsHost ? QTYPE_A_INDEX : QTYPE_SRV_INDEX)], pDNSdesc);

                        pCtx->nClaimCount++;

                        // Need to set timeout: ANNOUNCE_WAIT or INTERVAL

                        if (pCtx->nClaimCount == 1)
                        {
                            /* Setup a delay of MDNS_ANNOUNCE_WAIT before announcing */

                            /* Need to wait for time MDNS_ANNOUNCE_WAIT msec */
                            pCtx->random_delay = (MDNS_ANNOUNCE_WAIT * (SYS_TMR_TickCounterFrequencyGet() / 1000));
                        }
                        else
                        {
                            pCtx->random_delay = (MDNS_ANNOUNCE_INTERVAL * (SYS_TMR_TickCounterFrequencyGet() / 1000));
                        }

                        // Intenional fall-through

                    case ZGZC_KEEP_WAITING:

                        // Not Completed the delay proposed
                        return;
                }

                // Completed the delay required
                /* Set the timer for next announce */
            }
        }

        case MDNS_STATE_DEFEND:
        {
            /* On detection of Conflict Move back to PROBE step */

            if (pCtx->bLateConflictSeen)
            {
                /* Clear the Flag */
                pCtx->bLateConflictSeen = false;
                INFO_MDNS_PRINT("CONFLICT DETECTED !!! \r\n");
                INFO_MDNS_PRINT("Re-probing the Host-Name because of Conflict \r\n");
                pCtx->state = MDNS_STATE_INIT;
                pCtx->time_recorded = 0;

                INFO_MDNS_PRINT("MDNS_STATE_DEFEND --> MDNS_STATE_INIT \r\n");
            }
            else
                return;
        }

        default:
            break;
    }
}

void TCPIP_MDNS_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != 0)
    { // TMO or RX signal occurred
        TCPIP_MDNS_Process();
    }

}

// send a signal to the MDNS module that data is available
// no manager alert needed since this normally results as a higher layer (UDP) signal
static void _mDNSSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_UDP_SIGNAL_RX_DATA)
    {
        _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}

static void TCPIP_MDNS_Process(void)
{
    int netIx;

    DNSDesc_t *pDNSdesc;


    for(netIx = 0; netIx < TCPIP_STACK_NumberOfNetworksGet(); netIx++)
    {
        pDNSdesc = gDNSdesc + netIx;

        if(pDNSdesc->MDNS_flags.mcastFilterSet == 0)
        {   // Register an RX MAC fitler for the IP multicast group 224.0.0.251, which is mapped to 01:00:5E:00:00:FB
            //  Check that the MAC is ready
            TCPIP_NET_IF* pNetIf = pDNSdesc->mTcpIpNetIf;
            const TCPIP_MAC_OBJECT*  pMacObj = _TCPIP_STACK_GetMacObject(pNetIf);
            SYS_MODULE_OBJ macObjHandle = _TCPIP_STACK_GetMacObjectHandle(pNetIf);
            TCPIP_MAC_HANDLE hIfMac = _TCPIP_STACK_GetMacClientHandle(pNetIf);
            if(pMacObj && macObjHandle && hIfMac)
            {
                SYS_STATUS macStat = (*pMacObj->TCPIP_MAC_Status)(macObjHandle);
                if(macStat == SYS_STATUS_READY)
                {   // MAC is ready
                    TCPIP_MAC_ADDR mcast_addr = { {0x01, 0x00, 0x5E, 0x00, 0x00, 0xFB} };
                    (*pMacObj->TCPIP_MAC_RxFilterHashTableEntrySet)(hIfMac, &mcast_addr);
                    pDNSdesc->MDNS_flags.mcastFilterSet = 1;
                }
            }
        }

        if(!TCPIP_STACK_NetworkIsLinked(pDNSdesc->mTcpIpNetIf))
        {
            pDNSdesc->mHostCtx.common.state = MDNS_STATE_INTF_NOT_CONNECTED;
        }

        if(pDNSdesc->mTcpIpNetIf->netIPAddr.Val == 0x00)
        {
            continue;
        }

        if (pDNSdesc->mTcpIpNetIf->netIPAddr.Val != pDNSdesc->mResponderCtx.prev_ipaddr.Val)
        {
            // IP address has been changed outside of Zeroconf.
            // Such change could be due to static IP assignment, or
            // a new dynamic IP lease.
            // Need to restart state-machine

            INFO_MDNS_PRINT("IP-Address change is detected \r\n");
            pDNSdesc->mResponderCtx.prev_ipaddr.Val = pDNSdesc->mTcpIpNetIf->netIPAddr.Val;
            pDNSdesc->mHostCtx.common.state = MDNS_STATE_IPADDR_NOT_CONFIGURED;

            // File in the host RR for the specified interface and mDNS desc
            _mDNSFillHostRecord(pDNSdesc);
        }

        /* Poll _mDNSResponder to allow it to check for
         * incoming mDNS Quries/Responses */
        _mDNSResponder(pDNSdesc);

        if(pDNSdesc->mSDCtx.service_registered)
        {

            // Application has registered some services.
            // We now need to start the service probe/announce/defend process.
            if (pDNSdesc->mHostCtx.common.state != MDNS_STATE_DEFEND)
            {
                pDNSdesc->mSDCtx.common.state = MDNS_STATE_NOT_READY;
            }
            else
            {
                _mDNSProcessInternal((mDNSProcessCtx_common *) &pDNSdesc->mSDCtx,pDNSdesc);
            }
        }
        _mDNSProcessInternal((mDNSProcessCtx_common *) &pDNSdesc->mHostCtx,pDNSdesc);
    }

}



static void _mDNSSetAddresses(DNSDesc_t *pDNSdesc)
{
   IP_MULTI_ADDRESS Addr;

   /* Open a UDP socket for inbound and outbound transmission
    * Since we expect to only receive multicast packets and
    * only send multicast packets the remote NodeInfo
    * parameter is initialized to Multicast-IP (224.0.0.251)
    * corresponding Multicast MAC-Address (01:00:5E:00:00:FB) */

    Addr.v4Add.v[0] = 0xE0;
    Addr.v4Add.v[1] = 0x0;
    Addr.v4Add.v[2] = 0x0;
    Addr.v4Add.v[3] = 0xFB;

    TCPIP_UDP_RemoteBind(pDNSdesc->mDNS_socket, IP_ADDRESS_TYPE_ANY, MDNS_PORT, 0); 

    TCPIP_UDP_DestinationIPAddressSet(pDNSdesc->mDNS_socket // Now, destination address needs 2b multicast address
                              ,IP_ADDRESS_TYPE_IPV4
                              ,&Addr);

    Addr.v4Add=pDNSdesc->mTcpIpNetIf->netIPAddr;

    TCPIP_UDP_SourceIPAddressSet(pDNSdesc->mDNS_socket
                              ,IP_ADDRESS_TYPE_IPV4
                              ,&Addr);

}

#endif //#if defined (TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL) && defined(TCPIP_STACK_USE_ZEROCONF_MDNS_SD)
#endif // defined(TCPIP_STACK_USE_IPV4)





