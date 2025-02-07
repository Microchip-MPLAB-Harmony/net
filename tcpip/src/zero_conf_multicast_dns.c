/*******************************************************************************
  Zero Configuration (Zeroconf) Multicast DNS and
  Service Discovery Module for Microchip TCP/IP Stack

  Summary:

  Description: The mDNS servers listen on a standardized multicast IP address of 224.0.0.251
               (or ff02::fb for IPv6 link-local addressing).
*******************************************************************************/

/*
Copyright (C) 2012-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/









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
typedef union U_MDNS_MSG_HEADER_FLAGS
{

    struct __attribute__((packed))
    {
        unsigned        rcode:4;
        unsigned        z:3;
        unsigned        ra:1;
        unsigned        rd:1;
        unsigned        tc:1;
        unsigned        aa:1;
        unsigned        opcode:4;
        unsigned        qr:1;
    }bits;
    uint16_t Val;
    uint8_t v[2];
} MDNS_MSG_HEADER_FLAGS;

/* MDNS Message-Header Format */
typedef struct S_MDNS_MSG_HEADER
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

struct S_mDNSProcessCtx_sd;// mdnsd_struct
struct S_mDNSProcessCtx_common;

typedef struct S_mDNSResourceRecord
{
    char*              name;
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
   char *rdata;      // for PTR, SRV and TXT records.

    /* House-Keeping Stuff */

   // pointer to the header Ctx of the process that "owns" this resource record.
   struct S_mDNSProcessCtx_common *pOwnerCtx;

    uint8_t valid; /* indicates whether rr is valid */
   bool bNameAndTypeMatched;
   bool bResponseRequested;
   bool bResponseSuppressed;
} mDNSResourceRecord;

/* DNS-SD Specific Data-Structures */

typedef enum E_MDNS_STATE
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

typedef enum E_MDNS_RR_GROUP
{
   MDNS_RR_GROUP_QD, // Quuery count
   MDNS_RR_GROUP_AN, // Answer count
   MDNS_RR_GROUP_NS, // Authority record count
   MDNS_RR_GROUP_AR  // Addition Record Count
} MDNS_RR_GROUP;

typedef struct S_mDNSResponderCtx
{
   mDNSResourceRecord   rr_list[MAX_RR_NUM];   // Our resource records.

   bool                 bLastMsgIsIncomplete;   // Last DNS msg was truncated
   TCPIP_UINT16_VAL     query_id;            // mDNS Query transaction ID
   IPV4_ADDR            prev_ipaddr;         // To keep track of changes in IP-addr
} mDNSResponderCtx;

typedef enum E_MDNS_CTX_TYPE
{
   MDNS_CTX_TYPE_HOST = 0,
   MDNS_CTX_TYPE_SD
} MDNS_CTX_TYPE;

typedef struct S_mDNSProcessCtx_common
{
    MDNS_CTX_TYPE   type;      // Is owner mDNS ("HOST") or mDNS-SD ("SD")?
    MDNS_STATE      state;      // PROBE, ANNOUNCE, DEFEND, ...

    uint8_t nProbeCount;
    uint8_t nProbeConflictCount;
    uint8_t nClaimCount;
    uint8_t bProbeConflictSeen;
    uint8_t bLateConflictSeen;

    uint8_t bConflictSeenInLastProbe;
    uint8_t nInstanceId;
    uint8_t time_recorded; // Flag to indicate event_time is loaded

    uint32_t event_time;   // Internal Timer, to keep track of events
    uint32_t random_delay;


} mDNSProcessCtx_common;

typedef struct S_mDNSProcessCtx_host
{
   mDNSProcessCtx_common common;

   mDNSResponderCtx *pResponderCtx;

   // other host name related info

   char szUserChosenHostName[MAX_HOST_NAME_SIZE];   // user chosen host name
   char szHostName[MAX_HOST_NAME_SIZE];               // mDNS chosen Host-Name

} mDNSProcessCtx_host;

typedef struct S_mDNSProcessCtx_sd
{
   mDNSProcessCtx_common common;

   mDNSResponderCtx *pResponderCtx;

   // info specific to SD
    char srv_name[MAX_SRV_NAME_SIZE];
    char srv_type[MAX_SRV_TYPE_SIZE];
    char sd_qualified_name[MAX_RR_NAME_SIZE];
    uint8_t used; /* Spinlock to protect Race-cond. */

    uint8_t sd_auto_rename;         // Flag to show auto-Rename is enabled
    uint8_t sd_service_advertised;  // Flag to show whether service is advertised
    uint8_t service_registered;     // Flag to indicate that user has registered this service

    uint16_t sd_port; /* Port number in Local-sys where Service is being offered */
    uint16_t sd_txt_rec_len;
    char sd_txt_rec[MAX_TXT_DATA_SIZE];

    void (*sd_call_back)(char* srv_name, MDNSD_ERR_CODE errCode , void* context);
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
    struct __attribute__((packed))
    {
        unsigned    mcastFilterSet      :  1;       // Multi cast filter enabled on this interface
        unsigned    reserved            : 15;       // future use
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


// conversion functions/helpers
//
static __inline__ mDNSProcessCtx_sd* __attribute__((always_inline)) FC_CtxCom2CtxSd(mDNSProcessCtx_common* comCtx)
{
    union
    {
        mDNSProcessCtx_common*    comCtx;
        mDNSProcessCtx_sd*  sdPtr;
    }U_OWN_CTX_SD_CTX;

    U_OWN_CTX_SD_CTX.comCtx = comCtx;
    return U_OWN_CTX_SD_CTX.sdPtr;
}

static __inline__ mDNSProcessCtx_common* __attribute__((always_inline)) FC_CtxSd2CtxCom(mDNSProcessCtx_sd* sdPtr)
{
    union
    {
        mDNSProcessCtx_sd*  sdPtr;
        mDNSProcessCtx_common*    comCtx;
    }U_SD_CTX_OWN_CTX;

    U_SD_CTX_OWN_CTX.sdPtr = sdPtr;
    return U_SD_CTX_OWN_CTX.comCtx;
}

static __inline__ mDNSProcessCtx_common* __attribute__((always_inline)) FC_CtxHost2CtxCom(mDNSProcessCtx_host* hCtx)
{
    union
    {
        mDNSProcessCtx_host*  hCtx;
        mDNSProcessCtx_common*    comCtx;
    }U_HOST_CTX_COM_CTX;

    U_HOST_CTX_COM_CTX.hCtx = hCtx;
    return U_HOST_CTX_COM_CTX.comCtx;
}


/* Forward declarations */
static void F_mDNSSetAddresses(DNSDesc_t *pDNSdesc);
static void F_mDNSResponder(DNSDesc_t *pDNSdesc);
static uint16_t F_mDNSDeCompress(uint16_t wPos, char *pcString, bool bFollowPtr, uint8_t cElement, uint8_t cDepth, DNSDesc_t *pDNSdesc);
static size_t F_mDNSSDFormatServiceInstance(char *string, size_t strSize );
static MDNSD_ERR_CODE F_mDNSHostRegister( char *host_name,DNSDesc_t *pDNSdesc);
static void F_mDNSFillHostRecord(DNSDesc_t *pDNSdesc);
static void F_mDNSSDFillResRecords(mDNSProcessCtx_sd *sd,DNSDesc_t *pDNSdesc);
static void F_mDNSAnnounce(mDNSResourceRecord *pRR, DNSDesc_t *pDNSdesc);
static void F_mDNSProcessInternal(mDNSProcessCtx_common *pCtx, DNSDesc_t *pDNSdesc);

static void TCPIP_MDNS_Process(void);
static void F_mDNSSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param);

static DNSDesc_t *gDNSdesc = NULL;
static int  mDNSInitCount = 0;      // mDNS module initialization count
static TCPIP_SIGNAL_HANDLE   mdnsSignalHandle = NULL;  // mDNS asynchronous handle
/////////////////////////////////////////




/************* Local String Functions ******************/



/***************************************************************
  Function:
   static int8_t F_strcmp_local_ignore_case(char *string_1, char *string_2)

  Summary:
   Compares two strings by ignoring the case.

  Parameters:
   string_1 & string_2 - Two strings

  Returns:
    Zero: If two strings are equal.
    Non-Zero: If both strings are not equal or on error case
  **************************************************************/
static int8_t F_strcmp_local_ignore_case(const char *str_1, const char *str_2)
{
    if(str_1 == NULL || str_2 == NULL)
    {
        WARN_MDNS_PRINT("strmcmp_local_ignore_case: String is NULL \r\n");
        return -1;
    }

    while(*str_1 != '\0' && *str_2 != '\0')
    {
        if(*str_1 == *str_2 || (*str_1 - 32) == *str_2 || *str_1 == (*str_2 - 32))
        {
            str_1++;
            str_2++;
            continue;
        }
        else
        {
            return 1;
        }

    }
    if(*str_1 == '\0' && *str_2 == '\0')
    {
        return 0;
    }
    else
    {
        return 1;
    }

}


static void F_mDNSCountersReset(mDNSProcessCtx_common *pHeader, bool bResetProbeConflictCount)
{
   if (bResetProbeConflictCount)
   {
        pHeader->nProbeConflictCount = 0U;
   }

   pHeader->nProbeCount = 0U;
   pHeader->nClaimCount = 0U;
   pHeader->bLateConflictSeen = 0U;
   pHeader->bProbeConflictSeen = 0U;
}

/***************************************************************
  Function:
   static void F_mDNSRename(char *str, uint8_t max_len)

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
static void F_mDNSRename(char *strLabel, uint8_t nLabelId, char *strBase, char *strTarget, uint8_t nMaxLen)
{
    size_t  targetLen;
    uint8_t n = nLabelId;
#define mDNSRename_ID_LEN 6
    char str_n[mDNSRename_ID_LEN]; //enough for "-255." + '\0'.
    uint8_t i = mDNSRename_ID_LEN - 1 ;

    str_n[i--] = '\0';
    str_n[i--] = '.';

    // construct str_n from n
    while (i != 0U)
    {
        str_n[i--] = '0'+ n % 10U;
        if (n < 10U)
        {
            break;
        }
        n = n / 10U;
    }
    str_n[i] = '-';

    targetLen = strncpy_m(strTarget, nMaxLen, 3, strLabel, &(str_n[i]), strBase);


    if ( targetLen == nMaxLen )
    {
        WARN_MDNS_PRINT("F_mDNSRename: label too long - truncated\r\n");
    }


}

/***************************************************************
  Function:
   static void F_mDNSPutString(char* String)

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
static void F_mDNSPutString(char* string, DNSDesc_t * pDNSdesc)
{
   char *right_ptr,*label_ptr;
   char label[MAX_LABEL_SIZE];
   char ch;
   uint8_t len;

   right_ptr = string;

   while(true)
   {
       label_ptr = label;
       len = 0;
       while(*right_ptr != '\0')
       {
           ch = *right_ptr;

           if(ch == '.' || ch == '/' || ch == ',' || ch == '>' || ch == '\\')
           {
               /* Formatted Serv-Instance will have '\.'
                * instead of just '.' */
               if(ch == '\\')
               {
                   right_ptr++;
               }
               else
               {
                   break;
               }
           }
           *label_ptr++ = *right_ptr;
           len++;
           right_ptr++;
       }
       ch = *right_ptr++;

       // Put the length and data
       // Also, skip over the '.' in the input string
       (void)TCPIP_UDP_Put(pDNSdesc->mDNS_socket, len);
       (void)TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, (uint8_t*)label, (uint16_t)len);
       string =  right_ptr;

       if(ch == '\0' || ch == '/' || ch == ',' || ch == '>')
       {
           break;
       }
   }

   // Put the string null terminator character
   (void)TCPIP_UDP_Put(pDNSdesc->mDNS_socket, 0x00);
}

static uint16_t F_mDNSStringLength(char* string)
{
    char *right_ptr,*label_ptr;
    char label[MAX_LABEL_SIZE];
    char ch;
    uint8_t len;
    uint16_t retVal = 0;

    right_ptr = string;

    while(true)
    {
        label_ptr = label;
        len = 0;
        while(*right_ptr != '\0')
        {
            ch = *right_ptr;

            if(ch == '.' || ch == '/' || ch == ',' || ch == '>' || ch == '\\')
            {
                /* Formatted Serv-Instance will have '\.'
                 * instead of just '.' */
                if(ch == '\\')
                {
                    right_ptr++;
                }
                else
                {
                    break;
                }
            }
            *label_ptr++ = *right_ptr;
            len++;
            right_ptr++;
        }
        ch = *right_ptr++;

        // Put the length and data
        // Also, skip over the '.' in the input string
        retVal ++;
        retVal += len;
        string =  right_ptr;

        if(ch == '\0' || ch == '/' || ch == ',' || ch == '>')
        {
            break;
        }
    }
    retVal ++;
    return retVal;
}

/***************************************************************
  Function:
   static void F_mDNSProbe(uint8_t *name, MDNS_QTYPE q_type)

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
static bool F_mDNSProbe(mDNSProcessCtx_common *pCtx, DNSDesc_t *pDNSdesc)
{
    MDNS_MSG_HEADER mDNS_header;

    // Abort operation if no UDP sockets are available

    if(pDNSdesc->mDNS_socket == INVALID_UDP_SOCKET)
    {
        WARN_MDNS_PRINT("F_mDNSProbe: Opening UDP Socket Failed \r\n");
        return false;
    }

    // Make certain the socket can be written to
    if(TCPIP_UDP_TxPutIsReady(pDNSdesc->mDNS_socket, 256U) < 256U)
    {
        WARN_MDNS_PRINT("F_mDNSProbe: UDP Socket TX Busy \r\n");
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
    (void)TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, (uint8_t *) &mDNS_header, (uint16_t)sizeof(MDNS_MSG_HEADER));

    // Start of the QD section
    switch (pCtx->type)
    {
        case MDNS_CTX_TYPE_HOST:
            F_mDNSPutString(pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].name,pDNSdesc);
            break;

        case MDNS_CTX_TYPE_SD:
            F_mDNSPutString(pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].name,pDNSdesc);
            break;

        default:
            // do nothing
            break;
    }

    {
        uint8_t mdnsinfo[] = {0x00U, (uint8_t)QTYPE_ANY, 0x80U, 0x01U};
        (void)TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, mdnsinfo, (uint16_t)sizeof(mdnsinfo));
    }
    // Start of the NS section
    switch (pCtx->type)
    {
        case MDNS_CTX_TYPE_HOST:
            F_mDNSPutString(pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].name,pDNSdesc);
            break;

        case MDNS_CTX_TYPE_SD:
            F_mDNSPutString(pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].name,pDNSdesc);
            break;

        default:
            // do nothing
            break;
    }

    {
        uint8_t nsInfo[] = {
            0x00, 0x00, 0x00, 0x01,
            0x00, 0x00, 0x00, 0x78
        };
        switch (pCtx->type)
        {
            case MDNS_CTX_TYPE_HOST:
                nsInfo[1] = (uint8_t)QTYPE_A;
                break;

            case MDNS_CTX_TYPE_SD:
                nsInfo[1] = (uint8_t)QTYPE_SRV;
                break;

            default:
                // do nothing
                break;
        }
        (void)TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, nsInfo, (uint16_t)sizeof(nsInfo));

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
                (void)TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, hostInfo, (uint16_t)sizeof(hostInfo));


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
                (void)TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, sdInfo, (uint16_t)sizeof(sdInfo));

                F_mDNSPutString(pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].rdata,pDNSdesc);

                break;
            }

        default:
            // do nothing
            break;
    }

    F_mDNSSetAddresses(pDNSdesc);
    (void)TCPIP_UDP_Flush(pDNSdesc->mDNS_socket);

    return true;
}

/***************************************************************
  Function:
   static bool F_mDNSSendRR(struct mDNSResourceRecord *record,
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

    In Defend-Phase, when F_mDNSResponder receives a query for
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

static bool F_mDNSSendRR(mDNSResourceRecord *pRecord
          ,uint16_t query_id
          ,uint8_t cFlush
          ,uint16_t nAnswersInMsg
          ,bool bIsFirstRR
          ,bool bIsLastRR
          ,DNSDesc_t *pDNSdesc)
{
    MDNS_MSG_HEADER mDNS_header;
    TCPIP_UINT32_VAL ttl;
    uint16_t rec_length;
    uint8_t record_type;
    UDP_SOCKET_INFO sktInfo;

    record_type = (uint8_t)pRecord->type.Val;

    if(pDNSdesc->mDNS_socket == INVALID_UDP_SOCKET)
    {
        WARN_MDNS_PRINT("F_mDNSSendRR: Opening UDP Socket Failed \r\n");
        return false;
    }

    (void)memset(&sktInfo, 0, sizeof(sktInfo));
    (void)TCPIP_UDP_SocketInfoGet( pDNSdesc->mDNS_socket , &sktInfo);

    if (bIsFirstRR)
    {
        if(TCPIP_UDP_TxPutIsReady(pDNSdesc->mDNS_socket, (uint16_t)sizeof(MDNS_MSG_HEADER)) <  (uint16_t)sizeof(MDNS_MSG_HEADER))
        {
            WARN_MDNS_PRINT("F_mDNSSendRR: UDP Socket TX Busy \r\n");
            return false;
        }
        (void)memset(&mDNS_header, 0, sizeof(MDNS_MSG_HEADER));

        mDNS_header.query_id.Val = TCPIP_Helper_htons(query_id);

        mDNS_header.flags.bits.qr = 1; // this is a Response,
        mDNS_header.flags.bits.aa = 1; // and we are authoritative
        mDNS_header.flags.Val = TCPIP_Helper_htons(mDNS_header.flags.Val);

        mDNS_header.nAnswers.Val = TCPIP_Helper_htons(nAnswersInMsg);

        // Put out the mDNS message header
        (void)TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, (uint8_t *) &mDNS_header, (uint16_t)sizeof(MDNS_MSG_HEADER));
    }

    ttl.Val = pRecord->ttl.Val;

    F_mDNSPutString(pRecord->name,pDNSdesc);


    {

        uint8_t resourceRecord[] = {
            0x00, record_type, 0x00, 0x01,
            ttl.v[3], ttl.v[2], ttl.v[1], ttl.v[0]
        };
        if (sktInfo.remotePort == (uint16_t)MDNS_PORT)
        {
            resourceRecord[2] = cFlush;
        }
        (void)TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, resourceRecord, (uint16_t)sizeof(resourceRecord));

    }

    switch (record_type)
    {
        case (uint8_t)QTYPE_A:
            {
                uint8_t aRecord[] = {
                    0x00, 0x04,
                    pRecord->ip.v[0],
                    pRecord->ip.v[1],
                    pRecord->ip.v[2],
                    pRecord->ip.v[3]
                };
                (void)TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, aRecord, (uint16_t)sizeof(aRecord));
            }
            break;

        case (uint8_t)QTYPE_PTR:
            {
                /* 2 bytes extra. One for Prefix Length for first-label.
                 * Other one for NULL terminator */
                pRecord->rdlength.Val = (uint16_t)strlen((char*)pRecord->rdata) + 2U;

                {
                    uint8_t ptrRecord[] = {
                        pRecord->rdlength.v[1],
                        pRecord->rdlength.v[0]
                    };
                    (void)TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, ptrRecord, (uint16_t)sizeof(ptrRecord));
                }
                F_mDNSPutString( FC_CtxCom2CtxSd(pRecord->pOwnerCtx)->sd_qualified_name, pDNSdesc); //0x97
            }
            break;

        case (uint8_t)QTYPE_SRV:
            {
                /* 2 bytes extra. One for Prefix Length for first-label.
                 * Other one for NULL terminator */
                pRecord->rdlength.Val = (uint16_t)strlen((char*)pRecord->rdata) + 2U;
                pRecord->rdlength.Val += 6U;               // for priority, weight, and port
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
                    (void)TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, srvRecord, (uint16_t)sizeof(srvRecord));
                }
                F_mDNSPutString(pRecord->rdata,pDNSdesc); // 0x120

            }
            break;

        case (uint8_t)QTYPE_TXT:

            rec_length = (uint16_t)strlen((char*)pRecord->rdata);

            pRecord->rdlength.Val = rec_length + 1U;

            {
                uint8_t txtRecord[] = {
                    pRecord->rdlength.v[1],
                    pRecord->rdlength.v[0],
                    (uint8_t)pRecord->rdlength.Val - 1U
                };
                (void)TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, txtRecord, (uint16_t)sizeof(txtRecord));
            }

            if(rec_length > 0U)
            {
                (void)TCPIP_UDP_ArrayPut(pDNSdesc->mDNS_socket, (uint8_t*)pRecord->rdata, (uint16_t)rec_length); 
            }
            break;

        default:
            // not dupported type
            WARN_MDNS_PRINT("RR Type not supported \n");
            break;
    }

    if (bIsLastRR)
    {
        F_mDNSSetAddresses(pDNSdesc);
        (void)TCPIP_UDP_Flush(pDNSdesc->mDNS_socket);
    }

    return true;
}

static uint16_t F_mDNSSendRRSize(mDNSResourceRecord *pRecord ,bool bIsFirstRR)
{

    uint16_t rec_length;
    uint16_t record_type;

    uint16_t retValue = 0U;

    record_type = pRecord->type.Val;

    if (bIsFirstRR)
    {
        retValue += (uint8_t)sizeof(MDNS_MSG_HEADER);
    }

    retValue += F_mDNSStringLength(pRecord->name);

    // Resource Record Type
    retValue += 10U;

    switch (record_type)
    {
        case (uint16_t)QTYPE_A:
            retValue += 6U;
            break;

        case (uint16_t)QTYPE_PTR:
            retValue += 2U;
            retValue += F_mDNSStringLength(FC_CtxCom2CtxSd(pRecord->pOwnerCtx)->sd_qualified_name); //0x97
            break;

        case (uint16_t)QTYPE_SRV:
            retValue += 8U;
            retValue += F_mDNSStringLength(pRecord->rdata); // 0x120
            break;

        case (uint16_t)QTYPE_TXT:
            rec_length = (uint16_t)strlen((char*)pRecord->rdata);
            retValue += 3U + rec_length;
            break;

        default:
            // not dupported type
            WARN_MDNS_PRINT("RR Type not supported \n");
            break;
    }

    return retValue;
}
/***************************************************************
  Function:
   size_t F_mDNSSDFormatServiceInstance(char *string, size_t strSize )

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
static size_t F_mDNSSDFormatServiceInstance(char *string, size_t strSize )
{
    char *temp;
    char output[MAX_LABEL_SIZE];
    char ch;
    char *right_ptr,*str_token;
    uint8_t len;

    temp = output;
    right_ptr = string;
    str_token = string;
    while(true)
    {
        do
        {
            ch = *right_ptr++;
        } while((ch != '\0') && (ch != '\\') && (ch != '.') );


        /* Prefix '\' for every occurance of '.' & '\' */
        len = (uint8_t)(FC_ChPtrDiff2UI16(right_ptr, str_token) - 1U);

        (void)memcpy(temp,str_token,len);
        temp += len;
        str_token +=  len;
        if(ch == '.' || ch == '\\')
        {
            *temp = '\\';
            temp++;
            *temp++ = ch;
            str_token += 1;

        }
        else
        {
            // ch == '\0'
            break;
        }
    }
    *temp++ = '\0';
    return strncpy_m((char*)string, strSize, 1, output);
}

/***************************************************************
  Function:
   void F_mDNSSDFillResRecords(mdnsd_struct *sd)

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
static void F_mDNSSDFillResRecords(mDNSProcessCtx_sd *sd,DNSDesc_t *pDNSdesc)
{
    size_t srv_name_len,srv_type_len, qual_len;
    mDNSResourceRecord *rr_list;
    uint16_t serv_port;

    srv_name_len = strlen((char*)sd->srv_name);
    srv_type_len = strlen((char*)sd->srv_type);
    serv_port = pDNSdesc->mSDCtx.sd_port;

    (void)memset(&(pDNSdesc->mResponderCtx.rr_list[QTYPE_PTR_INDEX]),0,(sizeof(mDNSResourceRecord)));
    (void)memset(&(pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX]),0,(sizeof(mDNSResourceRecord)));
    (void)memset(&(pDNSdesc->mResponderCtx.rr_list[QTYPE_TXT_INDEX]),0,(sizeof(mDNSResourceRecord)));


    /* Formatting Service-Instance name.
     * And preparing a fully qualified
     * Service-instance record . */


    (void)strncpy((char*)sd->sd_qualified_name, (char*)sd->srv_name, sizeof(sd->sd_qualified_name));
    qual_len= F_mDNSSDFormatServiceInstance(sd->sd_qualified_name, sizeof(sd->sd_qualified_name));
    (void)strncpy_m((char*)&sd->sd_qualified_name[qual_len], sizeof(sd->sd_qualified_name) - qual_len, 2, ".", sd->srv_type);
    sd->sd_port = pDNSdesc->mSDCtx.sd_port = serv_port;

    /* Fill-up PTR Record */
    rr_list = &pDNSdesc->mResponderCtx.rr_list[QTYPE_PTR_INDEX];
    rr_list->type.Val = (uint16_t)QTYPE_PTR;
    rr_list->name = sd->srv_type;

    /* Res Record Name is
     * Service_Instance_name._srv-type._proto.domain */
    rr_list->rdata = sd->sd_qualified_name;

    (void)strncpy_m((char*)rr_list->rdata + srv_name_len, strlen((char*)sd->sd_qualified_name) - srv_name_len, 2, ".", sd->srv_type);

    /* 3 bytes extra. One for dot added between
     * Serv-Name and Serv-Type. One for length byte.
     * added for first-label in fully qualified name
     * Other one for NULL terminator */
    rr_list->rdlength.Val = (uint16_t)srv_name_len+ (uint16_t)srv_type_len + 3U;
    rr_list->ttl.Val = RESOURCE_RECORD_TTL_VAL; /* Seconds. Not sure ! Need to check */
    rr_list->pOwnerCtx = FC_CtxSd2CtxCom(sd); /* Save back ptr */
    rr_list->valid = 1; /* Mark as valid */



    /* Fill-up SRV Record */
    rr_list = &pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX]; /* Move onto next entry */
    rr_list->name = sd->sd_qualified_name;
    rr_list->type.Val = (uint16_t)QTYPE_SRV;
    rr_list->ttl.Val = RESOURCE_RECORD_TTL_VAL;

    //rdlength is calculated/assigned last
    rr_list->srv.priority.Val = 0;
    rr_list->srv.weight.Val = 0;
    rr_list->srv.port.Val = pDNSdesc->mSDCtx.sd_port;

    /* Res Record Name is
     * Service_Instance_name._srv-type._proto.domain */
    rr_list->rdata = pDNSdesc->mHostCtx.szHostName;


    /* 2 bytes extra. One for Prefix Length for first-label.
     * Other one for NULL terminator */
    // then, add 6-byte extra: for priority, weight, and port

    rr_list->rdlength.Val = (uint16_t)strlen(rr_list->rdata) + 2U + 6U;
    rr_list->pOwnerCtx = FC_CtxSd2CtxCom(sd); /* Save back ptr */
    rr_list->valid = 1; /* Mark as valid */    




    /* Fill-up TXT Record with NULL data*/
    rr_list = &pDNSdesc->mResponderCtx.rr_list[QTYPE_TXT_INDEX]; /* Move onto next entry */
    rr_list->type.Val = (uint16_t)QTYPE_TXT;
    rr_list->name = sd->sd_qualified_name;

    /* Res Record data is what defined by the user */
    rr_list->rdata = sd->sd_txt_rec;

    /* Extra byte for Length-Byte of TXT string */
    rr_list->rdlength.Val = pDNSdesc->mSDCtx.sd_txt_rec_len + 1U;
    rr_list->ttl.Val = RESOURCE_RECORD_TTL_VAL;
    rr_list->pOwnerCtx = FC_CtxSd2CtxCom(sd); /* Save back ptr */
    rr_list->valid = 1; /* Mark as valid */
}

MDNSD_ERR_CODE TCPIP_MDNS_ServiceUpdate(TCPIP_NET_HANDLE netH, uint16_t port, const uint8_t* txt_record)
{
    mDNSProcessCtx_sd *sd;
    DNSDesc_t *pDNSdesc;
    const TCPIP_NET_IF* pNetIf;

    pNetIf = TCPIPStackHandleToNetUp(netH);
    if(pNetIf != NULL)
    {
        pDNSdesc = gDNSdesc + TCPIP_STACK_NetIxGet(pNetIf);
        sd = &pDNSdesc->mSDCtx;

        if( sd->used != 0U)
        {
            sd->service_registered = 0U;
            sd->sd_port = port;
            /* Update Port Value in SRV Resource-record */
            pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].srv.port.Val = port;

            if(txt_record != NULL)
            {
                sd->sd_txt_rec_len = (uint16_t)strncpy_m(sd->sd_txt_rec, sizeof(sd->sd_txt_rec), 1, txt_record );

                /* Update Resource-records for this
                 * Service-instance, in MDNS-SD state-
                 * -machine */
                F_mDNSSDFillResRecords(sd,pDNSdesc);
                sd->common.state = MDNS_STATE_NOT_READY;
            }

            /* Notify MDNS Stack about Service-Registration
             * to get a time-slot for its own processing */
            sd->service_registered = 1U;
            return MDNSD_SUCCESS;
        }
    }

    return MDNSD_ERR_INVAL;
}

MDNSD_ERR_CODE TCPIP_MDNS_ServiceDeregister(TCPIP_NET_HANDLE netH)
{
    DNSDesc_t *pDNSdesc;
    mDNSProcessCtx_sd *sd;
    const TCPIP_NET_IF* pNetIf;

    pNetIf = TCPIPStackHandleToNetUp(netH);
    if(pNetIf != NULL)
    {
        pDNSdesc = gDNSdesc + TCPIP_STACK_NetIxGet(pNetIf);
        sd = &pDNSdesc->mSDCtx;

        if(sd->used != 0U)
        {
            if(sd->sd_service_advertised == 1U)
            {
                /* Send GoodBye Packet */
                pDNSdesc->mResponderCtx.rr_list[QTYPE_PTR_INDEX].ttl.Val = 0;
                pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].ttl.Val = 0;
                pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX].ttl.Val = 0;

                (void)F_mDNSSendRR(&pDNSdesc->mResponderCtx.rr_list[QTYPE_PTR_INDEX], 0, 0x00, 3, true,false,pDNSdesc);
                (void)F_mDNSSendRR(&pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX], 0, 0x80, 3, false,false,pDNSdesc);
                (void)F_mDNSSendRR(&pDNSdesc->mResponderCtx.rr_list[QTYPE_SRV_INDEX], 0, 0x80, 3, false,true,pDNSdesc);
            }
            /* Clear mSDCtx struct */
            sd->service_registered = 0U;
            (void)memset(sd,0,sizeof(mDNSProcessCtx_sd));
            return MDNSD_SUCCESS;
        }
    }

    return MDNSD_ERR_INVAL; /* Invalid Parameter */
}


MDNSD_ERR_CODE TCPIP_MDNS_ServiceRegister( TCPIP_NET_HANDLE netH
                    ,const char *srv_name
                    ,const char *srv_type
                    ,uint16_t port
                    ,const uint8_t *txt_record
                    ,uint8_t auto_rename
                    ,void (*call_back)(char *name, MDNSD_ERR_CODE err, void *context)
                    ,void *context)
{
    DNSDesc_t *desc;
    const TCPIP_NET_IF* pNetIf;

    if ( (srv_name == NULL) || (srv_type == NULL) || (txt_record == NULL) )
    {
        return MDNSD_ERR_INVAL; // Invalid Parameter
    }

    pNetIf = TCPIPStackHandleToNetUp(netH);
    if(pNetIf != NULL)
    {
        desc = gDNSdesc + TCPIP_STACK_NetIxGet(pNetIf);

        if(desc->mSDCtx.used != 0U)
        {
            return MDNSD_ERR_BUSY;
        }

        /* Clear the State-Machine */
        (void)memset(&desc->mSDCtx,0,sizeof(mDNSProcessCtx_sd));
        desc->mSDCtx.used = 1U; /* Mark it as used */
        desc->mSDCtx.sd_auto_rename = auto_rename;
        desc->mSDCtx.sd_port = port;
        desc->mSDCtx.sd_service_advertised = 0;

        (void)strncpy(desc->mSDCtx.srv_name , srv_name , sizeof(desc->mSDCtx.srv_name) - 1U);

        (void)strncpy(desc->mSDCtx.srv_type , srv_type , sizeof(desc->mSDCtx.srv_type) - 1U);

        desc->mSDCtx.sd_call_back = call_back;
        desc->mSDCtx.sd_context   = context;

        desc->mSDCtx.sd_txt_rec_len = (uint16_t)strncpy_m(desc->mSDCtx.sd_txt_rec , sizeof(desc->mSDCtx.sd_txt_rec), 1, txt_record);

        /* Fill up Resource-records for this
         * Service-instance, in MDNS-SD state-
         * -machine */
        F_mDNSSDFillResRecords(&desc->mSDCtx,desc);

        desc->mSDCtx.common.type  = MDNS_CTX_TYPE_SD;
        desc->mSDCtx.common.state = MDNS_STATE_NOT_READY;
        desc->mSDCtx.common.nInstanceId = 0U;

        /* Notify MDNS Stack about Service-Registration
         * to get a time-slot for its own processing */
        desc->mSDCtx.service_registered = 1U;
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
    This function makes use of F_mDNSSendRR to send out DNS-Resource-
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
static void F_mDNSAnnounce(mDNSResourceRecord *pRR, DNSDesc_t *pDNSdesc)
{
    if( false == F_mDNSSendRR(pRR ,0 ,0x80 ,1 ,true ,true ,pDNSdesc))
    {
        WARN_MDNS_PRINT("F_mDNSAnnounce: Error in sending out Announce pkt \r\n");
    }
}


static uint16_t F_mDNSFetch(uint16_t wOffset, uint16_t wLen, uint8_t *pcString,DNSDesc_t *pDNSdesc)
{
    uint16_t rc;

    TCPIP_UDP_RxOffsetSet(pDNSdesc->mDNS_socket, wOffset);

    rc = TCPIP_UDP_ArrayGet(pDNSdesc->mDNS_socket, pcString, wLen);

    return rc;
}


/***************************************************************
  Function:
   static uint16_t F_mDNSDeCompress(uint16_t wPos, char *pcString, bool bFollowPtr, uint8_t cElement, uint8_t cDepth)

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

// Note: the resursive form of the F_mDNSDeCompress is currently maintained for comparison/debugging purposes
// it will be eventually removed
#define M_MDNS_DECOMP_ENABLE_RECURSION     0

#if (M_MDNS_DECOMP_ENABLE_RECURSION != 0)
static uint16_t F_mDNSDeCompress(uint16_t wPos, char *pcString, bool bFollowPtr, uint8_t cElement, uint8_t cDepth, DNSDesc_t *pDNSdesc)
{
    uint16_t rr_len = 0U; // As is in the packet. Could be in compressed format.
    uint16_t startOffset, endOffset;
    uint8_t temp8;
    uint16_t offset_in_ptr, substr_len;

    startOffset = wPos;

    while (true)
    {
        rr_len++;
        if(TCPIP_UDP_Get(pDNSdesc->mDNS_socket, &temp8) == 0U)
        {
            break;
        }

        substr_len = (uint16_t)temp8;
        if(substr_len == 0u)
        {
            if (pcString != NULL)
            {
                *pcString++ = '\0';
            }
            break;
        }

        if((substr_len & 0xC0U) == 0xC0U)   // b'11 at MSb indicates compression ptr
        {
            offset_in_ptr = (substr_len & 0x3FU); // the rest of 6 bits is part of offset_in_ptr.
            offset_in_ptr = offset_in_ptr << 8;

            /* Remove label-ptr byte */
            rr_len++;
            (void)TCPIP_UDP_Get(pDNSdesc->mDNS_socket, &temp8);
            offset_in_ptr += temp8;

            if (bFollowPtr)
            {
                cDepth++;

                TCPIP_UDP_RxOffsetSet(pDNSdesc->mDNS_socket, offset_in_ptr);
                (void)F_mDNSDeCompress(offset_in_ptr, pcString, bFollowPtr, cElement, cDepth,pDNSdesc);

                // compressed ptr is always the last element
                break;
            }

            break;
        }
        else
        {
            if (pcString != NULL)
            {
                if (cElement > 0U)
                {
                    // not the first element in name
                    *pcString++ = '.';
                }

                (void)TCPIP_UDP_ArrayGet(pDNSdesc->mDNS_socket, (uint8_t*)pcString, substr_len);
                pcString += substr_len;
            }
            else
            {   // discard
                (void)TCPIP_UDP_ArrayGet(pDNSdesc->mDNS_socket, NULL, substr_len);
            }

            cElement++;
            rr_len += substr_len;
        }
    }

    endOffset = startOffset + rr_len;
    TCPIP_UDP_RxOffsetSet(pDNSdesc->mDNS_socket, endOffset);

    return rr_len;
}
#else  // (M_MDNS_DECOMP_ENABLE_RECURSION == 0)
// implementation using no recursive calls
// multiple sequential decompress calls are pushed to a local stack/list

// strutcture used for the mDNS decompress function

// decompress operation steps 
// 8 bit only
typedef enum
{
    DNS_DECOMP_STEP_NONE = 0,       // no step/invalid
    DNS_DECOMP_STEP_PROC,           // normal process, 1st step of decompression
    DNS_DECOMP_STEP_ADJUST,         // 2nd step of decompression, adjust the socket offset
                                    // they are always pushed in this order: (PROC, ADJUST), (PROC, ADJUST), ...
                                    // But, as the PROC may push another (PROC, ADJUST), the ADJUST steps could accumulate: (PROC, ADJUST, ADJUST, ADJUST)
    DNS_DECOMP_STEP_ERROR,          // some error occurred
}DNS_DECOMP_STEP;

typedef union
{
    SGL_LIST_NODE*                  lNode;      // safe cast to SGL_LIST_NODE
    struct S_tag_DNS_DECOMP_DCPT*   dcptNode;   // DECOMP next/node
}U_DECOMP_DCPT_SGL_NODE;        

typedef struct S_tag_DNS_DECOMP_DCPT
{
    U_DECOMP_DCPT_SGL_NODE      uNode;      // for a PROC node, next is always ADJUST
                                            // for an ADJUST node, next is always ADJUST or NULL
    uint16_t    wPos;           // PROC/ADJUST: current offset position in the UDP RX buffer
    uint16_t    rr_len;         // PROC/ADJUST current RR len; this is used for the adjust offset stage
    UDP_SOCKET  mDNS_socket;    // PROC/ADJUST
    uint8_t     decompStep;     // PROC/ADJUST: current step to execute
                                // also used to signal an error if == DNS_DECOMP_STEP_ERROR
    uint8_t     cElement;       // PROC only
    uint8_t     cDepth;         // PROC only
    uint8_t     bFollowPtr;     // PROC only
    char*       pcString;       // PROC only
}DNS_DECOMP_DCPT;

#define M_MDNS_DECOMP_DEPTH     4   // maximum allowed decompression pool size
                                    // since the operations are pushed in pairs, max is 3 pending operations: (PROC, ADJUST, ADJUST, ADJUST)

static DNS_DECOMP_DCPT  dns_decomp_pool[M_MDNS_DECOMP_DEPTH];
// implemented with a list instead of a stack
static SINGLE_LIST decompFreeList;      // list of DNS_DECOMP_DCPT holding available decompress requests
static SINGLE_LIST decompBusyList;      // list of DNS_DECOMP_DCPT holding active decompress requests

static void F_Decomp_InitPool(void)
{
    size_t ix;
    TCPIP_Helper_SingleListInitialize(&decompFreeList);
    TCPIP_Helper_SingleListInitialize(&decompBusyList);

    // initialize the free list
    DNS_DECOMP_DCPT* pDcpt = dns_decomp_pool;
    for(ix = 0; ix < sizeof(dns_decomp_pool) / sizeof(*dns_decomp_pool); ix++)
    {
        TCPIP_Helper_SingleListTailAdd(&decompFreeList, pDcpt->uNode.lNode);
        pDcpt++;
    } 
}

// pushes a new (PROC, ADJUST) request to the stack
static bool F_Decomp_Push(uint16_t wPos, char *pcString, bool bFollowPtr, uint8_t cElement, uint8_t cDepth, UDP_SOCKET mDNS_socket)
{
    // get an adjust node
    U_DECOMP_DCPT_SGL_NODE uDcpt;
    uDcpt.lNode = TCPIP_Helper_SingleListHeadRemove(&decompFreeList);
    DNS_DECOMP_DCPT* pAdjust = uDcpt.dcptNode; 

    // get a proc node
    uDcpt.lNode = TCPIP_Helper_SingleListHeadRemove(&decompFreeList);
    DNS_DECOMP_DCPT* pProc = uDcpt.dcptNode;
    if(pAdjust == NULL || pProc == NULL)
    {   // maximum allowable depth exceeded
        TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
        return false;
    }

    // OK, push another request pair to the stack
    // (compressed ptr should always be the last element)
    pAdjust->decompStep = (uint8_t)DNS_DECOMP_STEP_ADJUST;
    pAdjust->wPos = 0U;   // this is updated by the proc step!
    pAdjust->rr_len = 0U;   // this is updated by the proc step!
    pAdjust->mDNS_socket = mDNS_socket;


    pProc->decompStep = (uint8_t)DNS_DECOMP_STEP_PROC;
    pProc->wPos = wPos; 
    pProc->pcString = pcString; 
    pProc->bFollowPtr = (uint8_t)bFollowPtr;
    pProc->cElement = cElement;
    pProc->cDepth = cDepth;
    pProc->mDNS_socket = mDNS_socket;

    TCPIP_Helper_SingleListHeadAdd(&decompBusyList, pAdjust->uNode.lNode);
    TCPIP_Helper_SingleListHeadAdd(&decompBusyList, pProc->uNode.lNode);

    return true;
}

// DNS_DECOMP_STEP_ADJUST step
static void F_DecompAdjust(DNS_DECOMP_DCPT* pDcpt)
{
    uint16_t endOffset = pDcpt->wPos + pDcpt->rr_len;
    TCPIP_UDP_RxOffsetSet(pDcpt->mDNS_socket, endOffset);
}

// DNS_DECOMP_STEP_PROC step
static void F_DecompProcess(DNS_DECOMP_DCPT* pDcpt)
{
    uint16_t rr_len = 0U; // As is in the packet. Could be in compressed format.
    uint8_t temp8;
    uint16_t offset_in_ptr, substr_len;

    TCPIPStack_Assert(pDcpt->uNode.dcptNode != NULL && pDcpt->uNode.dcptNode->decompStep == (uint8_t)DNS_DECOMP_STEP_ADJUST, __FILE__, __func__, __LINE__);

    while (true)
    {
        rr_len++;
        if(TCPIP_UDP_Get(pDcpt->mDNS_socket, &temp8) == 0U)
        {
            break;
        }

        substr_len = (uint16_t)temp8;
        if(substr_len == 0u)
        {
            if (pDcpt->pcString != NULL)
            {
                *pDcpt->pcString++ = '\0';
            }
            break;
        }

        if((substr_len & 0xC0U) == 0xC0U)   // b'11 at MSb indicates compression ptr
        {
            offset_in_ptr = (substr_len & 0x3FU); // the rest of 6 bits is part of offset_in_ptr.
            offset_in_ptr = offset_in_ptr << 8;

            /* Remove label-ptr byte */
            rr_len++;
            (void)TCPIP_UDP_Get(pDcpt->mDNS_socket, &temp8);
            offset_in_ptr += temp8;

            if (pDcpt->bFollowPtr != 0U)
            {
                TCPIP_UDP_RxOffsetSet(pDcpt->mDNS_socket, offset_in_ptr);
                // prepare the data for the ADJUST step!
                DNS_DECOMP_DCPT* pAdjust = pDcpt->uNode.dcptNode;
                pAdjust->wPos = pDcpt->wPos;
                pAdjust->rr_len = rr_len;
                // exec another proc step
                // compressed ptr is always the last element
                bool pushRes = F_Decomp_Push(offset_in_ptr, pDcpt->pcString, pDcpt->bFollowPtr != 0U, pDcpt->cElement, pDcpt->cDepth + 1U, pDcpt->mDNS_socket);
                if(pushRes == false)
                {   // too deep
                    TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
                    pDcpt->decompStep = (uint8_t)DNS_DECOMP_STEP_ERROR;
                }
            }
            break;
        }
        else
        {
            if (pDcpt->pcString != NULL)
            {
                if (pDcpt->cElement > 0U)
                {
                    // not the first element in name
                    *pDcpt->pcString++ = '.';
                }

                (void)TCPIP_UDP_ArrayGet(pDcpt->mDNS_socket, (uint8_t*)pDcpt->pcString, substr_len);
                pDcpt->pcString += substr_len;
            }
            else
            {   // discard
                (void)TCPIP_UDP_ArrayGet(pDcpt->mDNS_socket, NULL, substr_len);
            }

            pDcpt->cElement++;
            rr_len += substr_len;
        }
    }
}

static uint16_t F_mDNSDeCompress(uint16_t wPos, char *pcString, bool bFollowPtr, uint8_t cElement, uint8_t cDepth, DNSDesc_t *pDNSdesc)
{
    F_Decomp_InitPool();
    bool pushRes = F_Decomp_Push(wPos, pcString, bFollowPtr, cElement, cDepth, pDNSdesc->mDNS_socket);

    if(pushRes == false)
    {   // the 1st push should NOT fail
        TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
        return 0xffffU;
    }

    uint8_t lastStep = (uint8_t)DNS_DECOMP_STEP_NONE;   // DNS_DECOMP_STEP
    uint16_t last_rr = 0xffffU;

    DNS_DECOMP_DCPT* pDcpt;
    while(true)
    {
        U_DECOMP_DCPT_SGL_NODE uDcpt;
        uDcpt.lNode = TCPIP_Helper_SingleListHeadRemove(&decompBusyList);
        pDcpt = uDcpt.dcptNode;

        if(pDcpt == NULL)
        {   // done
            break;
        }

        if(pDcpt->decompStep == (uint8_t)DNS_DECOMP_STEP_ERROR)
        {   // some error occurred
            last_rr = 0xffffU;
            break;
        }
        // decompress
        else if(pDcpt->decompStep == (uint8_t)DNS_DECOMP_STEP_PROC)
        {
            F_DecompProcess(pDcpt);
        }
        else
        {
            F_DecompAdjust(pDcpt);
        }
        lastStep = pDcpt->decompStep;
        last_rr = pDcpt->rr_len;
        TCPIP_Helper_SingleListTailAdd(&decompFreeList, pDcpt->uNode.lNode);
    }

    // done
    TCPIPStack_Assert(lastStep == (uint8_t)DNS_DECOMP_STEP_ADJUST, __FILE__, __func__, __LINE__);
    return last_rr;
}

#endif  // (M_MDNS_DECOMP_ENABLE_RECURSION != 0)



static bool F_mDNSTieBreaker(mDNSResourceRecord *their, mDNSResourceRecord *our)
{
    bool WeWonTheTieBreaker = true;
    uint8_t i;

    if (their->type.Val == (uint16_t)QTYPE_A)
    {
        for (i = 0; i <= 3U; i++)
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
            else
            {
                // do nothing
            }
        }
    }
    else if (their->type.Val == (uint16_t)QTYPE_SRV)
    {
        if (their->srv.port.Val >= our->srv.port.Val)
        {
            WeWonTheTieBreaker = false;
        }
    }
    else
    {
        // do nothing
    }

    DEBUG0_MDNS_PRINT( (char *) (WeWonTheTieBreaker ? "   tie-breaker won\r\n" : "   tie-breaker lost\r\n") );

    return WeWonTheTieBreaker;
}


static uint8_t F_mDNSProcessIncomingRR(MDNS_RR_GROUP tag, MDNS_MSG_HEADER *pmDNSMsgHeader, uint16_t idxGroup, uint16_t idxRR, DNSDesc_t *pDNSdesc)
{
    mDNSResourceRecord res_rec;
    char name[2 * MAX_RR_NAME_SIZE];  
    uint8_t i,j;
    uint16_t len;
    mDNSProcessCtx_common *pOwnerCtx;
    mDNSResourceRecord      *pMyRR;
    bool WeWonTheTieBreaker = false;
    bool bMsgIsAQuery;         // QUERY or RESPONSE ?
    bool bSenderHasAuthority;   // Sender has the authority ?
    uint8_t rxBuffer[6] = {0};

    bMsgIsAQuery = (pmDNSMsgHeader->flags.bits.qr == 0U);
    bSenderHasAuthority = (pmDNSMsgHeader->flags.bits.aa == 1U);

    res_rec.name = name; // for temporary name storage.

    // NAME
    (void)memset(name, 0, sizeof(name));
    len = F_mDNSDeCompress(pDNSdesc->mDNS_offset, name, true, 0, 0,pDNSdesc);
    pDNSdesc->mDNS_offset += len;

    // TYPE & CLASS
    (void)TCPIP_UDP_ArrayGet(pDNSdesc->mDNS_socket, rxBuffer, 4U);
    res_rec.type.v[1] = rxBuffer[0];
    res_rec.type.v[0] = rxBuffer[1];
    res_rec.class.v[1] = rxBuffer[2];
    res_rec.class.v[0] = rxBuffer[3];

    pDNSdesc->mDNS_offset += 4U;

    // Do the first round name check
    for (i = 0; i < (uint8_t)MAX_RR_NUM; i++)
    {
        pDNSdesc->mResponderCtx.rr_list[i].bNameAndTypeMatched = false;

        if ( F_strcmp_local_ignore_case(name, pDNSdesc->mResponderCtx.rr_list[i].name) == 0 &&
                ((res_rec.type.Val == (uint16_t)QTYPE_ANY) || (res_rec.type.Val == pDNSdesc->mResponderCtx.rr_list[i].type.Val)))
        {
            pDNSdesc->mResponderCtx.rr_list[i].bNameAndTypeMatched = true;
        }
        else if ( (tag == MDNS_RR_GROUP_QD) && F_strcmp_local_ignore_case(name, "_services._dns-sd._udp.local") == 0
                && (res_rec.type.Val == (uint16_t)QTYPE_PTR))
        {
            pDNSdesc->mResponderCtx.rr_list[i].bNameAndTypeMatched = true;
        }
        else
        {
            // do nothing
        }
    }


    // Only AN, NS, AR records have extra fields
    if ( tag != MDNS_RR_GROUP_QD )
    {

        // Now retrieve those extra fields
        (void)TCPIP_UDP_ArrayGet(pDNSdesc->mDNS_socket, rxBuffer, 6U);
        res_rec.ttl.v[3] = rxBuffer[0];
        res_rec.ttl.v[2] = rxBuffer[1];
        res_rec.ttl.v[1] = rxBuffer[2];
        res_rec.ttl.v[0] = rxBuffer[3];
        res_rec.rdlength.v[1] = rxBuffer[4];
        res_rec.rdlength.v[0] = rxBuffer[5];

        pDNSdesc->mDNS_offset += 6U; 

        // The rest is record type dependent
        switch (res_rec.type.Val)
        {
            case (uint16_t)QTYPE_A:
                (void)TCPIP_UDP_ArrayGet(pDNSdesc->mDNS_socket, &res_rec.ip.v[0], 4U);

                pDNSdesc->mDNS_offset += 4U;

                break;

            case (uint16_t)QTYPE_PTR:

                (void)memset(name, 0 , sizeof(name));
                len = F_mDNSDeCompress(pDNSdesc->mDNS_offset, name, true, 0, 0,pDNSdesc);
                pDNSdesc->mDNS_offset += len;

                break;

            case (uint16_t)QTYPE_SRV:
                (void)TCPIP_UDP_ArrayGet(pDNSdesc->mDNS_socket, rxBuffer, 6U);
                res_rec.srv.priority.v[1] = rxBuffer[0];
                res_rec.srv.priority.v[0] = rxBuffer[1];
                res_rec.srv.weight.v[1] = rxBuffer[2];
                res_rec.srv.weight.v[0] = rxBuffer[3];
                res_rec.srv.port.v[1] = rxBuffer[4];
                res_rec.srv.port.v[0] = rxBuffer[5];

                pDNSdesc->mDNS_offset += 6U;

                (void)memset(name, 0 , sizeof(name));
                len = F_mDNSDeCompress(pDNSdesc->mDNS_offset, name, true, 0, 0,pDNSdesc);
                pDNSdesc->mDNS_offset += len;

                break;

            case (uint16_t)QTYPE_TXT:
            default:

                // Still needs to read it off
                (void)TCPIP_UDP_ArrayGet(pDNSdesc->mDNS_socket, NULL, res_rec.rdlength.Val);

                pDNSdesc->mDNS_offset += res_rec.rdlength.Val;
                break;
        }

        // We now have all info about this received RR.
    }

    // Do the second round
    for (i = 0; i < (uint8_t)MAX_RR_NUM; i++)
    {
        pMyRR = &(pDNSdesc->mResponderCtx.rr_list[i]);
        pOwnerCtx = pDNSdesc->mResponderCtx.rr_list[i].pOwnerCtx;

        if ( (!pMyRR->bNameAndTypeMatched) || (pOwnerCtx == NULL) )
        {
            // do nothing
            (void)TCPIP_UDP_Discard(pDNSdesc->mDNS_socket);
        }
        else if ( bMsgIsAQuery && (tag == MDNS_RR_GROUP_QD) && (pOwnerCtx->state == MDNS_STATE_DEFEND))
        {
            // Simple reply to an incoming DNS query.
            // Mark all of our RRs for reply.

            for (j = 0; j < (uint8_t)MAX_RR_NUM; j++)
            {
                pDNSdesc->mResponderCtx.rr_list[j].bResponseRequested = true;
            }
        }
        else if ( bMsgIsAQuery && (tag == MDNS_RR_GROUP_AN) && (pOwnerCtx->state == MDNS_STATE_DEFEND))
        {
            // An answer in the incoming DNS query.
            // Look for possible duplicate (known) answers suppression.
            if ((((res_rec.type.Val == (uint16_t)QTYPE_PTR) && (res_rec.ip.Val == pDNSdesc->mResponderCtx.rr_list[i].ip.Val)) ||
                    (F_strcmp_local_ignore_case(name, pDNSdesc->mResponderCtx.rr_list[i].rdata) == 0)) &&
                    (res_rec.ttl.Val > (pDNSdesc->mResponderCtx.rr_list[i].ttl.Val / 2U))
               )
            {
                pDNSdesc->mResponderCtx.rr_list[i].bResponseSuppressed = true;
                DEBUG_MDNS_PRINT("     rr suppressed\r\n");
            }
        }
        else if ( bMsgIsAQuery && (tag == MDNS_RR_GROUP_NS) && ((pOwnerCtx->state == MDNS_STATE_PROBE) || (pOwnerCtx->state == MDNS_STATE_ANNOUNCE)))
        {
            // Simultaneous probes by us and sender of this DNS query.
            // Mark as a conflict ONLY IF we lose the Tie-Breaker.

            WeWonTheTieBreaker = F_mDNSTieBreaker(&res_rec, &(pDNSdesc->mResponderCtx.rr_list[i]));

            if (!WeWonTheTieBreaker)
            {
                pOwnerCtx->bProbeConflictSeen = 1U;
                pOwnerCtx->nProbeConflictCount++;
            }

            (void)TCPIP_UDP_Discard(pDNSdesc->mDNS_socket);

            return 0;
        }
        else if ( !bMsgIsAQuery && bSenderHasAuthority && (tag == MDNS_RR_GROUP_AN) && ((pOwnerCtx->state == MDNS_STATE_PROBE) || (pOwnerCtx->state == MDNS_STATE_ANNOUNCE)))
        {
            // An authoritative DNS response to our probe/announcement.
            // Mark as a conflict. Effect a re-name, followed by a
            // re-probe.

            pOwnerCtx->bProbeConflictSeen = 1U;
            pOwnerCtx->nProbeConflictCount++;

            (void)TCPIP_UDP_Discard(pDNSdesc->mDNS_socket);

            return 0;
        }
        else if(bMsgIsAQuery && (tag == MDNS_RR_GROUP_NS) && (pOwnerCtx->state == MDNS_STATE_DEFEND))
        {
            // A probe by the sender conflicts with our established record.
            // Need to defend our record. Effect a DNS response.

            INFO_MDNS_PRINT("Defending RR: \r\n");

            pMyRR->bResponseRequested = true;

            (void)TCPIP_UDP_Discard(pDNSdesc->mDNS_socket);

            return 0;
        }
        else if ( !bMsgIsAQuery && bSenderHasAuthority && (tag == MDNS_RR_GROUP_AN) && (pMyRR->type.Val != (uint16_t)QTYPE_PTR ) && (pOwnerCtx->state == MDNS_STATE_DEFEND))
        {   // No one can claim authority on shared RR
            // Sender claims that it also has the authority on
            // a unique (non-shared) record that we have already established authority.
            // Effect a re-probe.

            pOwnerCtx->bLateConflictSeen = 1U;

            (void)TCPIP_UDP_Discard(pDNSdesc->mDNS_socket);

            return 0;
        }
        else
        {
            // do nothing
        }
    }
    return 0;
}


/***************************************************************
  Function:
   static void F_mDNSResponder(DNSDesc_t *pDNSdesc)

  Summary:
   Acts as Multicast-DNS responder & replies when it receives
    a query. Currenlty only supports IP_ADDRESS_TYPE_IPV4 addressing

  Description:
   This function is used as mDNS-Responder. On initialization of
    Multicast-DNS stack, this function Opens up pDNSdesc->mDNS_socket
    (UDP-Socket) for Mulitcast-Address (224.0.0.251).

    This function gets polled from TCPIP_MDNS_Task for every iteration.
    F_mDNSResponder constantly monitors the packets being sent to
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
static void F_mDNSResponder(DNSDesc_t *pDNSdesc)
{
    MDNS_MSG_HEADER mDNS_header;
    uint16_t len;
    uint16_t i,j,count;
    uint16_t rr_count[4];
    MDNS_RR_GROUP rr_group[4];
    bool bMsgIsComplete;
    uint16_t packetSize = 0;
    uint16_t bufferSize = 0;


    pDNSdesc->mDNS_offset = 0U;

    if(pDNSdesc->mDNS_socket == INVALID_UDP_SOCKET)
    {
        pDNSdesc->mDNS_responder_state = (uint16_t)MDNS_RESPONDER_INIT;
    }

    switch(pDNSdesc->mDNS_responder_state)
    {
        case (uint16_t)MDNS_RESPONDER_INIT:

            pDNSdesc->mDNS_socket = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_IPV4, MDNS_PORT, NULL);
            if(pDNSdesc->mDNS_socket == INVALID_UDP_SOCKET)
            {
                WARN_MDNS_PRINT("F_mDNSResponder: Can't open Multicast-DNS UDP-Socket \r\n");
                return;
            }
            else
            {
                (void)TCPIP_UDP_SocketNetSet(pDNSdesc->mDNS_socket, pDNSdesc->mTcpIpNetIf);
                (void)TCPIP_UDP_OptionsSet(pDNSdesc->mDNS_socket, UDP_OPTION_STRICT_NET, FC_Uint2VPtr(1UL));
                (void)TCPIP_UDP_SignalHandlerRegister(pDNSdesc->mDNS_socket, TCPIP_UDP_SIGNAL_RX_DATA, &F_mDNSSocketRxSignalHandler, NULL);
                pDNSdesc->mDNS_responder_state = (uint16_t)MDNS_RESPONDER_LISTEN ;
            }

            /* Called from TCPIP_MDNS_Initialize. So return immediately */
            break;

        case (uint16_t)MDNS_RESPONDER_LISTEN:

            // Do nothing if no data is waiting
            if(TCPIP_UDP_GetIsReady(pDNSdesc->mDNS_socket) == 0U)
            {
                break;
            }

            /* Reset the Remote-node information in UDP-socket */
            (void)TCPIP_UDP_RemoteBind(pDNSdesc->mDNS_socket, IP_ADDRESS_TYPE_ANY, MDNS_PORT, NULL); 
            (void)TCPIP_UDP_Bind(pDNSdesc->mDNS_socket, IP_ADDRESS_TYPE_ANY, MDNS_PORT, NULL); 

            // Retrieve the mDNS header
            len = F_mDNSFetch(0U, (uint16_t)sizeof(mDNS_header), (uint8_t *) &mDNS_header,pDNSdesc);
            mDNS_header.query_id.Val = TCPIP_Helper_ntohs(mDNS_header.query_id.Val);
            mDNS_header.flags.Val = TCPIP_Helper_ntohs(mDNS_header.flags.Val);
            mDNS_header.nQuestions.Val = TCPIP_Helper_ntohs(mDNS_header.nQuestions.Val);
            mDNS_header.nAnswers.Val = TCPIP_Helper_ntohs(mDNS_header.nAnswers.Val);
            mDNS_header.nAuthoritativeRecords.Val = TCPIP_Helper_ntohs(mDNS_header.nAuthoritativeRecords.Val);
            mDNS_header.nAdditionalRecords.Val = TCPIP_Helper_ntohs(mDNS_header.nAdditionalRecords.Val);

            pDNSdesc->mDNS_offset += len; // MUST BE 12

            if ( (mDNS_header.flags.bits.qr == 0U) )
            {
                DEBUG0_MDNS_PRINT("rx QUERY \r\n");
            }
            else
            {
                DEBUG0_MDNS_PRINT("rx RESPONSE \r\n");
            }

            bMsgIsComplete = (mDNS_header.flags.bits.tc == 0U);  // Message is not truncated.

            rr_count[0] = mDNS_header.nQuestions.Val;
            rr_group[0] = MDNS_RR_GROUP_QD;

            rr_count[1] = mDNS_header.nAnswers.Val;
            rr_group[1] = MDNS_RR_GROUP_AN;

            rr_count[2] = mDNS_header.nAuthoritativeRecords.Val;
            rr_group[2] = MDNS_RR_GROUP_NS;

            rr_count[3] = mDNS_header.nAdditionalRecords.Val;
            rr_group[3] = MDNS_RR_GROUP_AR;

            for (i = 0; i < (uint8_t)MAX_RR_NUM; i++)
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

            for (i = 0U; i < 4U; i++) // for all 4 groups: QD, AN, NS, AR
            {
                for(j=0; j < rr_count[i]; j++)
                {   // RR_count = {#QD, #AN, #NS, #AR}
                    (void)F_mDNSProcessIncomingRR(rr_group[i] ,&mDNS_header ,i ,j ,pDNSdesc);
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
                break;
            }

            // Count all RRs marked as "reply needed".
            count = 0U;
            for (i = 0; i < (uint16_t)MAX_RR_NUM; i++)
            {
                if ((pDNSdesc->mResponderCtx.rr_list[i].pOwnerCtx != NULL) && (pDNSdesc->mResponderCtx.rr_list[i].pOwnerCtx->state == MDNS_STATE_DEFEND) &&
                        (pDNSdesc->mResponderCtx.rr_list[i].bResponseRequested == true) && (pDNSdesc->mResponderCtx.rr_list[i].bResponseSuppressed == false))
                {
                    if (count == 0U)
                    {
                        packetSize = F_mDNSSendRRSize(&pDNSdesc->mResponderCtx.rr_list[i], true);
                        //SYS_CONSOLE_PRINT("Packet Size %d count = %d\r\n", packetSize, count);
                    }
                    else
                    {
                        packetSize += F_mDNSSendRRSize(&pDNSdesc->mResponderCtx.rr_list[i], false);
                        //SYS_CONSOLE_PRINT("Packet Size %d count = %d\r\n", packetSize, count);

                    }
                    count++;
                }
            }

            // Send all RRs marked as "reply needed".

            F_mDNSSetAddresses(pDNSdesc);

            if (TCPIP_UDP_OptionsGet(pDNSdesc->mDNS_socket, UDP_OPTION_TX_BUFF, &bufferSize))
            {
                //SYS_CONSOLE_PRINT("Buffer Size %d\r\n", bufferSize);
                if (bufferSize < packetSize)
                {
                    if (!TCPIP_UDP_OptionsSet(pDNSdesc->mDNS_socket, UDP_OPTION_TX_BUFF, FC_Uint2VPtr((uint32_t)packetSize + 10UL)))
                    {
                        DEBUG0_MDNS_PRINT("   buffer too small\r\n");
                        break;
                    }
                }
            }
            else
            {
                DEBUG0_MDNS_PRINT("   could not get buffer info\r\n");
                break;

            }


            j = 1;
            for (i = 0; (count > 0U) && (i < (uint16_t)MAX_RR_NUM); i++)
            {
                if ((pDNSdesc->mResponderCtx.rr_list[i].pOwnerCtx != NULL) && (pDNSdesc->mResponderCtx.rr_list[i].pOwnerCtx->state == MDNS_STATE_DEFEND) &&
                        (pDNSdesc->mResponderCtx.rr_list[i].bResponseRequested == true) && (pDNSdesc->mResponderCtx.rr_list[i].bResponseSuppressed == false) )
                {
                    (void)F_mDNSSendRR(&pDNSdesc->mResponderCtx.rr_list[i], mDNS_header.query_id.Val
                            ,(pDNSdesc->mResponderCtx.rr_list[i].type.Val == (uint16_t)QTYPE_PTR) ? (0x00U) : (0x80U) // flush, except for PTR; for Conformance Test.
                            ,count                                      // MAX_RR_NUM answers;
                            ,(j == 1U) ? true : false                    // Is this the first RR?
                            ,(j == count) ? true : false, pDNSdesc);    // Is this the last RR?

                    j++;
                }
            }

            if (bufferSize < packetSize)
            {
                (void)TCPIP_UDP_OptionsSet(pDNSdesc->mDNS_socket, UDP_OPTION_TX_BUFF, FC_Uint2VPtr((uint32_t)bufferSize));
            }

            // end of MDNS_RESPONDER_LISTEN
            break;

        default:
            // do nothing
            break;
    }

    return;
}




/***************************************************************
  Function:
   void void F_mDNSFillHostRecord(DNSDesc_t *pDNSdesc)

  Summary:


  Description:

  Precondition:
   None

  Parameters:
   None

  Returns:
     None
  **************************************************************/
static void F_mDNSFillHostRecord(DNSDesc_t *pDNSdesc)
{
    uint8_t i;

    // Fill the type A resource record
    pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].name     = pDNSdesc->mHostCtx.szHostName;
    pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].type.Val = (uint16_t)QTYPE_A; // Query Type is Answer
    // CLASS 1=INternet, 255=Any class etc
    pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].ttl.Val  = RESOURCE_RECORD_TTL_VAL;

    pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].rdlength.Val = 4u; // 4-byte for IP address

    // Fill in the data for an A RR record (IP address)
    for (i = 0; i <= 3U; i++)
    {
        pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].ip.v[i] = pDNSdesc->mTcpIpNetIf->netIPAddr.v[i];
    }

    pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].valid    = 1;
    pDNSdesc->mResponderCtx.rr_list[QTYPE_A_INDEX].pOwnerCtx = FC_CtxHost2CtxCom(&pDNSdesc->mHostCtx);
}



static MDNSD_ERR_CODE F_mDNSHostRegister( char *host_name,DNSDesc_t *pDNSdesc)
{

    (void)memcpy((char*)pDNSdesc->mHostCtx.szUserChosenHostName , host_name , sizeof(pDNSdesc->mHostCtx.szUserChosenHostName));

    (void)strncpy_m((char*)pDNSdesc->mHostCtx.szHostName
            , sizeof(pDNSdesc->mHostCtx.szHostName)
            , 3
            , pDNSdesc->mHostCtx.szUserChosenHostName
            , "."
            , pDNSdesc->CONST_STR_local);

    pDNSdesc->mHostCtx.szUserChosenHostName[MAX_HOST_NAME_SIZE-1] = '\0';
    pDNSdesc->mHostCtx.szHostName[MAX_HOST_NAME_SIZE-1] = '\0';

    F_mDNSCountersReset(FC_CtxHost2CtxCom(&pDNSdesc->mHostCtx), true);
    pDNSdesc->mHostCtx.common.type   = MDNS_CTX_TYPE_HOST;
    pDNSdesc->mHostCtx.common.state  = MDNS_STATE_INIT;
    pDNSdesc->mHostCtx.common.nInstanceId = 0U;

    // Now create a QTYPE_A record for later use when answering queries.
    F_mDNSFillHostRecord(pDNSdesc);
    pDNSdesc->mResponderCtx.bLastMsgIsIncomplete = false;

    pDNSdesc->mHostCtx.common.state = MDNS_STATE_INIT;

    return MDNSD_SUCCESS;
}


// Returns true on success
bool TCPIP_MDNS_Initialize( const TCPIP_STACK_MODULE_CTRL* const stackData, const void* initData)
{
    DNSDesc_t *desc;
    TCPIP_NET_IF*  pNetIf;
    char ServiceName[64];
    char*   hostName;

    if(stackData->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }

    // stack init

    if(mDNSInitCount == 0)
    {   // first time we're run

        // Alocate memory for a memory block of descriptors. Once descriptor per interface
        gDNSdesc = (DNSDesc_t*)TCPIP_HEAP_Calloc(stackData->memH, stackData->nIfs, sizeof(DNSDesc_t));
        if(gDNSdesc == NULL)
        {
            SYS_ERROR(SYS_ERROR_ERROR, "TCPIP_MDNS_Initialize: Failed to allocate memory\r\n");
            return false;
        }
        mdnsSignalHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_MDNS_Task, MDNS_TASK_TICK_RATE);
        if(mdnsSignalHandle == NULL)
        {   // cannot create the MDNS timer
            (void)TCPIP_HEAP_Free(stackData->memH, gDNSdesc);  // free the allocated memory
            gDNSdesc = NULL;
            return false;
        }
    }


    pNetIf = stackData->pNetIf;

    // Remove any trailing spaces from ther NetBiosName
    (void)strncpy(ServiceName, (char*)pNetIf->NetBIOSName, sizeof(ServiceName) - 1U);
    int32_t n = (int32_t)strlen((char*)pNetIf->NetBIOSName);
    ServiceName[n] = '\0'; // NULL Terminate in advance
    while(n >= 0)
    {
        if(isalnum((int)ServiceName[n]) != 0 || ispunct((int)ServiceName[n]) != 0)
        {
            break;
        }
        else
        {
            ServiceName[n] = '\0';
        }
        n--;
    }

    desc = gDNSdesc+ stackData->netIx;
    desc->mTcpIpNetIf = pNetIf;
    desc->mResponderCtx.query_id.Val = 0;
    desc->mResponderCtx.prev_ipaddr.Val = desc->mTcpIpNetIf->netIPAddr.Val;

    (void)strncpy(desc->CONST_STR_local,"local",6);

    hostName = NULL;
    if(ServiceName[0] != '\0')
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
    if(hostName != NULL)
    {
        (void)F_mDNSHostRegister(hostName, desc);
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
            (void)TCPIP_UDP_Close(pDNSdesc->mDNS_socket);
            pDNSdesc->mDNS_socket = INVALID_UDP_SOCKET;
        }
        pDNSdesc->MDNS_flags.val = 0;

        if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--mDNSInitCount == 0)
            {   // all closed
                // release resources
                (void)TCPIP_HEAP_Free(stackCtrl->memH, gDNSdesc);  // free the allocated memory
                gDNSdesc = NULL;
                if(mdnsSignalHandle != NULL)
                {
                    TCPIPStackSignalHandlerDeregister(mdnsSignalHandle);
                    mdnsSignalHandle = NULL;
                }
            }
        }
    }

}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)


static void F_mDNSProcessInternal(mDNSProcessCtx_common *pCtx, DNSDesc_t *pDNSdesc)
{
    uint8_t zgzc_action;
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
                break;
            }
            else
            {
                /* Multicast DNS is ready now */
                pCtx->state = MDNS_STATE_INIT;
                pCtx->time_recorded = 0U;
            }

            INFO_MDNS_PRINT("\r\nMDNS_STATE_NOT_READY --> MDNS_STATE_INIT \r\n");
            break;

        case MDNS_STATE_INTF_NOT_CONNECTED: // HOST starts from here. HOST only.
            if (!TCPIP_STACK_NetworkIsLinked(pDNSdesc->mTcpIpNetIf))
            {
                break;
            }
            else
            {
                /* Interface is connected now */
                pCtx->state = MDNS_STATE_IPADDR_NOT_CONFIGURED;
                pCtx->time_recorded = 0U;
            }

            break;

        case MDNS_STATE_IPADDR_NOT_CONFIGURED: // HOST only.
            // Wait until IP addr is configured ...
            if (pDNSdesc->mTcpIpNetIf->netIPAddr.Val == 0U)
            {
                break;
            }

            pCtx->state = MDNS_STATE_INIT;
            pCtx->time_recorded = 0U;

            INFO_MDNS_PRINT("MDNS_STATE_IPADDR_NOT_CONFIGURED --> MDNS_STATE_INIT \r\n");
            break;


        case MDNS_STATE_INIT:
            pCtx->bConflictSeenInLastProbe = 0U;

            zgzc_action = zgzc_wait_for(&(pCtx->random_delay), &(pCtx->event_time), &(pCtx->time_recorded));
            if (zgzc_action == ZGZC_STARTED_WAITING)
            {   // Need to choose Random time between 0-MDNS_PROBE_WAIT msec
                break;
            }
            else if (zgzc_action == ZGZC_KEEP_WAITING)
            {   // Not Completed the delay proposed
                break;
            }
            else
            {
                // Completed the delay required
            }

            // Clear all counters
            F_mDNSCountersReset(pCtx, true);

            pCtx->state = MDNS_STATE_PROBE;
            INFO_MDNS_PRINT("MDNS_STATE_INIT --> MDNS_STATE_PROBE \r\n");

            break;

        case MDNS_STATE_PROBE:
        case MDNS_STATE_ANNOUNCE:
            if (pCtx->bProbeConflictSeen != 0U)
            {
                pCtx->bConflictSeenInLastProbe = 1U;

                INFO_MDNS_PRINT("Conflict detected. Will rename\r\n");

                /* Conflict with selected name */
                pCtx->state = MDNS_STATE_PROBE;

                // Do not reset nProbeConflictCount if in PROBE state
                F_mDNSCountersReset( pCtx, (pCtx->state == MDNS_STATE_PROBE) ? false : true);

                if (bIsHost)
                {
                    // Rename host name
                    F_mDNSRename(pDNSdesc->mHostCtx.szUserChosenHostName , ++(pDNSdesc->mHostCtx.common.nInstanceId),
                            pDNSdesc->CONST_STR_local , pDNSdesc->mHostCtx.szHostName , MAX_HOST_NAME_SIZE);
                }
                else
                {
                    // Rename service instance name
                    if (pDNSdesc->mSDCtx.sd_auto_rename != 0U)
                    {
                        F_mDNSRename(pDNSdesc->mSDCtx.srv_name , ++pDNSdesc->mSDCtx.common.nInstanceId , pDNSdesc->mSDCtx.srv_type,
                                pDNSdesc->mSDCtx.sd_qualified_name , MAX_LABEL_SIZE);

                        /* Reset Multicast-UDP socket */
                        (void)TCPIP_UDP_Close(pDNSdesc->mDNS_socket);
                        pDNSdesc->mDNS_socket = INVALID_UDP_SOCKET;
                        F_mDNSResponder(pDNSdesc);
                    }
                    else
                    {
                        pDNSdesc->mSDCtx.service_registered = 0U;

                        pDNSdesc->mSDCtx.used = 0U;
                        if (pDNSdesc->mSDCtx.sd_call_back != NULL)
                        {
                            pDNSdesc->mSDCtx.sd_call_back((char *) pDNSdesc->mSDCtx.srv_name, MDNSD_ERR_CONFLICT, pDNSdesc->mSDCtx.sd_context);
                        }
                    }
                }
                break;
            }

            while (true)
            {
                zgzc_action = zgzc_wait_for(&(pCtx->random_delay), &(pCtx->event_time), &(pCtx->time_recorded));
                if (zgzc_action == ZGZC_STARTED_WAITING)
                {
                    if (pCtx->state == MDNS_STATE_PROBE)
                    {
                        if (((pCtx->nProbeCount >= (uint8_t)MDNS_PROBE_NUM) && pCtx->bConflictSeenInLastProbe == 0U) ||
                                (pCtx->nProbeConflictCount >= (uint8_t)MDNS_MAX_PROBE_CONFLICT_NUM))
                        {
                            /* Move onto Announce Step */
                            pCtx->state = MDNS_STATE_ANNOUNCE;
                            pCtx->bConflictSeenInLastProbe = 0U;

                            INFO_MDNS_PRINT("MDNS_STATE_PROBE --> MDNS_STATE_ANNOUNCE \r\n");
                            break;
                        }
                    }
                    else
                    {   // We are in MDNS_STATE_ANNOUNCE
                        if (pCtx->nClaimCount >= (uint8_t)MDNS_ANNOUNCE_NUM)
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

                                (void)F_mDNSSendRR(&pDNSdesc->mResponderCtx.rr_list[QTYPE_PTR_INDEX]
                                        , 0
                                        , 0x00
                                        , 1
                                        , true
                                        , true
                                        , pDNSdesc); // This produces a bad PTR rec for MCHPDEMO.local

                                pDNSdesc->mSDCtx.sd_service_advertised = 1;
                                if (pDNSdesc->mSDCtx.sd_call_back != NULL)
                                {
                                    pDNSdesc->mSDCtx.sd_call_back((char *) pDNSdesc->mSDCtx.srv_name , MDNSD_SUCCESS , pDNSdesc->mSDCtx.sd_context);
                                }
                            }
                            F_mDNSCountersReset(pCtx, true);

                            break;
                        }
                    }

                    if (pCtx->state == MDNS_STATE_PROBE)
                    {
                        // Send out Probe packet
                        (void)F_mDNSProbe(pCtx, pDNSdesc);

                        pCtx->nProbeCount++;
                        pCtx->bConflictSeenInLastProbe = 0U;

                        /* Need to set timeout for MDNS_PROBE_INTERVAL msec */
                        if (pCtx->nProbeConflictCount < 9U) // less-than-10 is required to pass Bonjour Conformance test.
                        {
                            pCtx->random_delay = ((uint32_t)MDNS_PROBE_INTERVAL * (SYS_TMR_TickCounterFrequencyGet() / 1000U));
                        }
                        else
                        {
                            pCtx->random_delay = (SYS_TMR_TickCounterFrequencyGet());
                        }

                        break;
                    }

                    // We are in MDNS_STATE_ANNOUNCE
                    /* Announce Name chosen on Local Network */

                    F_mDNSAnnounce(&pDNSdesc->mResponderCtx.rr_list[(bIsHost ? QTYPE_A_INDEX : QTYPE_SRV_INDEX)], pDNSdesc);

                    pCtx->nClaimCount++;

                    // Need to set timeout: ANNOUNCE_WAIT or INTERVAL

                    if (pCtx->nClaimCount == 1U)
                    {
                        /* Setup a delay of MDNS_ANNOUNCE_WAIT before announcing */

                        /* Need to wait for time MDNS_ANNOUNCE_WAIT msec */
                        pCtx->random_delay = ((uint32_t)MDNS_ANNOUNCE_WAIT * (SYS_TMR_TickCounterFrequencyGet() / 1000U));
                    }
                    else
                    {
                        pCtx->random_delay = ((uint32_t)MDNS_ANNOUNCE_INTERVAL * (SYS_TMR_TickCounterFrequencyGet() / 1000U));
                    }
                    break;
                }
                else if (zgzc_action == ZGZC_KEEP_WAITING)
                {   // Not Completed the delay proposed
                    break;
                }
                else
                {
                    // Completed the delay required
                    /* Set the timer for next announce */
                }
            }
            break;

        case MDNS_STATE_DEFEND:
            /* On detection of Conflict Move back to PROBE step */

            if (pCtx->bLateConflictSeen != 0U)
            {
                /* Clear the Flag */
                pCtx->bLateConflictSeen = 0U;
                INFO_MDNS_PRINT("CONFLICT DETECTED !!! \r\n");
                INFO_MDNS_PRINT("Re-probing the Host-Name because of Conflict \r\n");
                pCtx->state = MDNS_STATE_INIT;
                pCtx->time_recorded = 0U;

                INFO_MDNS_PRINT("MDNS_STATE_DEFEND --> MDNS_STATE_INIT \r\n");
            }
            break;

        default:
            // do nothing
            break;
    }
}

void TCPIP_MDNS_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != TCPIP_MODULE_SIGNAL_NONE)
    { // TMO or RX signal occurred
        TCPIP_MDNS_Process();
    }

}

// send a signal to the MDNS module that data is available
// no manager alert needed since this normally results as a higher layer (UDP) signal
static void F_mDNSSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_UDP_SIGNAL_RX_DATA)
    {
        (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}

static void TCPIP_MDNS_Process(void)
{
    size_t netIx;

    DNSDesc_t *pDNSdesc;


    for(netIx = 0; netIx < TCPIP_STACK_NumberOfNetworksGet(); netIx++)
    {
        pDNSdesc = gDNSdesc + netIx;

        if(pDNSdesc->MDNS_flags.mcastFilterSet == 0U)
        {   // Register an RX MAC fitler for the IP multicast group 224.0.0.251, which is mapped to 01:00:5E:00:00:FB
            //  Check that the MAC is ready
            TCPIP_NET_IF* pNetIf = pDNSdesc->mTcpIpNetIf;
            const TCPIP_MAC_OBJECT*  pMacObj = TCPIP_STACK_GetMacObject(pNetIf);
            SYS_MODULE_OBJ macObjHandle = TCPIP_STACK_GetMacObjectHandle(pNetIf);
            TCPIP_MAC_HANDLE hIfMac = TCPIP_STACK_GetMacClientHandle(pNetIf);
            if(pMacObj != NULL && macObjHandle != 0UL && hIfMac != 0UL)
            {
                SYS_STATUS macStat = (*pMacObj->MAC_Status)(macObjHandle);
                if(macStat == SYS_STATUS_READY)
                {   // MAC is ready
                    TCPIP_MAC_ADDR mcast_addr = { {0x01, 0x00, 0x5E, 0x00, 0x00, 0xFB} };
                    (void)(*pMacObj->MAC_RxFilterHashTableEntrySet)(hIfMac, &mcast_addr);
                    pDNSdesc->MDNS_flags.mcastFilterSet = 1;
                }
            }
        }

        if(!TCPIP_STACK_NetworkIsLinked(pDNSdesc->mTcpIpNetIf))
        {
            pDNSdesc->mHostCtx.common.state = MDNS_STATE_INTF_NOT_CONNECTED;
        }

        if(pDNSdesc->mTcpIpNetIf->netIPAddr.Val == 0UL)
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
            F_mDNSFillHostRecord(pDNSdesc);
        }

        /* Poll F_mDNSResponder to allow it to check for
         * incoming mDNS Quries/Responses */
        F_mDNSResponder(pDNSdesc);

        if(pDNSdesc->mSDCtx.service_registered != 0U)
        {

            // Application has registered some services.
            // We now need to start the service probe/announce/defend process.
            if (pDNSdesc->mHostCtx.common.state != MDNS_STATE_DEFEND)
            {
                pDNSdesc->mSDCtx.common.state = MDNS_STATE_NOT_READY;
            }
            else
            {
                F_mDNSProcessInternal(FC_CtxSd2CtxCom(&pDNSdesc->mSDCtx), pDNSdesc);
            }
        }
        F_mDNSProcessInternal(FC_CtxHost2CtxCom(&pDNSdesc->mHostCtx), pDNSdesc);
    }

}



static void F_mDNSSetAddresses(DNSDesc_t *pDNSdesc)
{
    IP_MULTI_ADDRESS Addr;

    /* Open a UDP socket for inbound and outbound transmission
     * Since we expect to only receive multicast packets and
     * only send multicast packets the remote NodeInfo
     * parameter is initialized to Multicast-IP (224.0.0.251)
     * corresponding Multicast MAC-Address (01:00:5E:00:00:FB) */

    Addr.v4Add.v[0] = 0xE0U;
    Addr.v4Add.v[1] = 0x0U;
    Addr.v4Add.v[2] = 0x0U;
    Addr.v4Add.v[3] = 0xFBU;

    (void)TCPIP_UDP_RemoteBind(pDNSdesc->mDNS_socket, IP_ADDRESS_TYPE_ANY, MDNS_PORT, NULL); 
    // Now, destination address needs 2b multicast address
    (void)TCPIP_UDP_DestinationIPAddressSet(pDNSdesc->mDNS_socket ,IP_ADDRESS_TYPE_IPV4 ,&Addr);

    Addr.v4Add=pDNSdesc->mTcpIpNetIf->netIPAddr;

    (void)TCPIP_UDP_SourceIPAddressSet(pDNSdesc->mDNS_socket ,IP_ADDRESS_TYPE_IPV4 ,&Addr);
}

#endif //#if defined (TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL) && defined(TCPIP_STACK_USE_ZEROCONF_MDNS_SD)
#endif // defined(TCPIP_STACK_USE_IPV4)





