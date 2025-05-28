/*******************************************************************************
  Dynamic Host Configuration Protocol (DHCPv6) Client for IPv6 networks

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    - Provides automatic IP address, subnet mask, gateway address, 
      DNS server address, and other configuration parameters on DHCP 
      enabled networks.
    - Reference: RFC 3315
*******************************************************************************/

/*
Copyright (C) 2015-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#include <string.h>

#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_DHCPV6_CLIENT

#include "tcpip/src/tcpip_private.h"


#if defined TCPIP_STACK_USE_IPV6 && defined(TCPIP_STACK_USE_DHCPV6_CLIENT)
// and IPV6, of course
//
// definitions
//

#include "tcpip/src/dhcpv6_private.h"

// checking a message re-transmit status
typedef enum
{
    TCPIP_DHCPV6_MSG_TX_RES_OK,          // OK, go ahead and transmit the message
    TCPIP_DHCPV6_MSG_TX_RES_PENDING,     // no timeout yet, wait some more

    // errors

    TCPIP_DHCPV6_MSG_TX_RES_TIME_EXCEEDED   = -1,    // message time exceeded
    TCPIP_DHCPV6_MSG_TX_RES_RETRY_EXCEEDED  = -2,    // message retries exceeded

}TCPIP_DHCPV6_MSG_TX_RESULT;


// find match function
// returns true when the passed in option matches its criteria
// the optCode and optLen should be ntoh converted
typedef bool (*F_OptionMatchFnc)(TCPIP_DHCPV6_OPTION_GENERIC* pOptG, uint32_t srchCode);

// message search descriptor
typedef struct
{
    void*               pOptBuff;       // current option buffer for next searches
    int16_t             optBuffLen;     // current length of the option buffer
    uint16_t            reserved;       // padding, not used
    F_OptionMatchFnc    matchFnc;       // option match function
}TCPIP_DHCPV6_MSG_SEARCH_DCPT;

// message validation mask
// 16 bit
#define DHCPV6_VALID_MASK_NONE             0x0000U
#define DHCPV6_VALID_MASK_TYPE             0x0001U
#define DHCPV6_VALID_MASK_OPT_LEN          0x0002U
#define DHCPV6_VALID_MASK_CLIENT_DUID      0x0004U
#define DHCPV6_VALID_MASK_SERVER_DUID      0x0008U
#define DHCPV6_VALID_MASK_CLIENT_THIS      0x0010U
#define DHCPV6_VALID_MASK_SERVER_THIS      0x0020U


#define TCPIP_DHCPV6_MSG_VALIDATION_MASK        (DHCPV6_VALID_MASK_TYPE | DHCPV6_VALID_MASK_OPT_LEN | \
                                               DHCPV6_VALID_MASK_CLIENT_DUID | DHCPV6_VALID_MASK_SERVER_DUID | DHCPV6_VALID_MASK_CLIENT_THIS)


// write options functions
// returns >= 0 the size of the option added
// returns < 0 for failure
typedef int (*F_DHCPV6Option_WriteFnc)(TCPIP_DHCPV6_IA_DCPT* pIa);


// local data
static int                          dhcpv6InitCount = 0;      // DHCP module initialization count

static UDP_PORT                     dhcpv6ClientPort;
static UDP_PORT                     dhcpv6ServerPort;

static TCPIP_SIGNAL_HANDLE            dhcpv6SignalHandle = NULL;

static const void*                  dhcpv6MemH = NULL;        // memory handle

#if (M_TCPIP_DHCPV6_USER_NOTIFICATION != 0)
static PROTECTED_SINGLE_LIST        dhcpv6RegisteredUsers = { {0} };
#endif  // (M_TCPIP_DHCPV6_USER_NOTIFICATION != 0)


static TCPIP_DHCPV6_CLIENT_DCPT*    dhcpv6ClientDcpt = NULL;    // DHCPv6 client per interface

// The IPv6 All_DHCP_Relay_Agents_and_Servers address
// const IPV6_ADDR     DHCPV6_Servers_Address = {{0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02}};
static const IP_MULTI_ADDRESS     DHCPV6_Servers_Address =
{
    .v6Add = {{0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02}}
};

static int              F_DHCPV6Option_WriteClientId(TCPIP_DHCPV6_IA_DCPT* pIa);
static int              F_DHCPV6Option_WriteServerId(TCPIP_DHCPV6_IA_DCPT* pIa);
static int              F_DHCPV6Option_WriteIA_NA(TCPIP_DHCPV6_IA_DCPT* pIa);
static int              F_DHCPV6Option_WriteIA_TA(TCPIP_DHCPV6_IA_DCPT* pIa);
static int              F_DHCPV6Option_WriteOptionRequest(TCPIP_DHCPV6_IA_DCPT* pIa);
static int              F_DHCPV6Option_WriteElapsedTime(TCPIP_DHCPV6_IA_DCPT* pIa);
static int              F_DHCPV6Option_WriteRapidCommit(TCPIP_DHCPV6_IA_DCPT* pIa);


static int              F_DHCPV6Option_Write(uint32_t optCode, TCPIP_DHCPV6_IA_DCPT* pIa);

// functions performing options write
static const F_DHCPV6Option_WriteFnc   T_DHCPV6Option_WriteFncTbl[DHCPV6_OPT_CODES_SUPPORTED] = 
{
    NULL,                                  // DHCPV6_OPT_CODE_NONE
    &F_DHCPV6Option_WriteClientId,      // DHCPV6_OPT_CODE_CLIENT_ID
    &F_DHCPV6Option_WriteServerId,      // DHCPV6_OPT_CODE_SERVER_ID
    &F_DHCPV6Option_WriteIA_NA,         // DHCPV6_OPT_CODE_IA_NA 
    &F_DHCPV6Option_WriteIA_TA,         // DHCPV6_OPT_CODE_IA_TA
    NULL,                               // DHCPV6_OPT_CODE_IA_ADDR         - called by IA_TA/IA_NA
    &F_DHCPV6Option_WriteOptionRequest, // DHCPV6_OPT_CODE_OPTION_REQ
    NULL,                               // DHCPV6_OPT_CODE_PREFERENCE      - server side only
    &F_DHCPV6Option_WriteElapsedTime,   // DHCPV6_OPT_CODE_ELAPSED_TIME
    NULL,                               // DHCPV6_OPT_CODE_RELAY_MSG       - OPTION_NOT_SUPPORTED; relay, server
    NULL,                               // DHCPV6_OPT_CODE_INVALID
    NULL,                               // DHCPV6_OPT_CODE_AUTHENTICATE    - OPTION_NOT_SUPPORTED in this implementation
    NULL,                               // DHCPV6_OPT_CODE_UNICAST         - server side only
    NULL,                               // DHCPV6_OPT_CODE_STATUS_CODE     - server side only
    &F_DHCPV6Option_WriteRapidCommit,   // DHCPV6_OPT_CODE_RAPID_COMMIT
    NULL,                               // DHCPV6_OPT_CODE_USER_CLASS      - OPTION_NOT_SUPPORTED in this implementation
    NULL,                               // DHCPV6_OPT_CODE_VENDOR_CLASS    - OPTION_NOT_SUPPORTED in this implementation
    NULL,                               // DHCPV6_OPT_CODE_VENDOR_OPTS     - OPTION_NOT_SUPPORTED in this implementation
    NULL,                               // DHCPV6_OPT_CODE_INTERFACE_ID    - OPTION_NOT_SUPPORTED; relay messages; unused
    NULL,                               // DHCPV6_OPT_CODE_RECONF_MSG      - OPTION_NOT_SUPPORTED; server sent option
    NULL,                               // DHCPV6_OPT_CODE_RECONF_ACCEPT   - OPTION_NOT_SUPPORTED 
};


// translation table: TCPIP_DHCPV6_CLIENT_MSG_TYPE -> TCPIP_DHCPV6_MSG_TYPE
static const uint8_t    dhcpv6ClientToMessageTypeTbl[DHCPV6_CLIENT_MSG_TYPE_NUMBER] = 
{
    (uint8_t)DHCPV6_MSG_TYPE_SOLICIT,               // ->DHCPV6_CLIENT_MSG_TYPE_SOLICIT
                                                                          
    (uint8_t)DHCPV6_MSG_TYPE_REQUEST,               // ->DHCPV6_CLIENT_MSG_TYPE_REQUEST
                                                                          
    (uint8_t)DHCPV6_MSG_TYPE_CONFIRM,               // ->DHCPV6_CLIENT_MSG_TYPE_CONFIRM
                                                                          
    (uint8_t)DHCPV6_MSG_TYPE_RENEW,                 // ->DHCPV6_CLIENT_MSG_TYPE_RENEW
                                                                          
    (uint8_t)DHCPV6_MSG_TYPE_REBIND,                // ->DHCPV6_CLIENT_MSG_TYPE_REBIND
                                                                          
    (uint8_t)DHCPV6_MSG_TYPE_RELEASE,               // ->DHCPV6_CLIENT_MSG_TYPE_RELEASE
                                                                          
    (uint8_t)DHCPV6_MSG_TYPE_DECLINE,               // ->DHCPV6_CLIENT_MSG_TYPE_DECLINE
                                                                          
    (uint8_t)DHCPV6_MSG_TYPE_INFO_REQUEST,          // ->DHCPV6_CLIENT_MSG_TYPE_INFO_REQUEST
};


// some of these values could be overwritten by the DHCP server!
static const TCPIP_DHCPV6_MSG_TRANSMIT_BOUNDS dhcpv6MessageBoundsTbl[DHCPV6_CLIENT_MSG_TYPE_NUMBER] =
{
    // iDelay                           irt                         mrt                          mrc                                 mrd     
    {  TCPIP_DHCPV6_SOLICIT_IDELAY,    TCPIP_DHCPV6_SOLICIT_IRT,    TCPIP_DHCPV6_SOLICIT_MRT,    TCPIP_DHCPV6_SOLICIT_MRC,           TCPIP_DHCPV6_SOLICIT_MRD },      // DHCPV6_CLIENT_MSG_TYPE_SOLICIT 
    {  TCPIP_DHCPV6_REQUEST_IDELAY,    TCPIP_DHCPV6_REQUEST_IRT,    TCPIP_DHCPV6_REQUEST_MRT,    TCPIP_DHCPV6_REQUEST_MRC,           TCPIP_DHCPV6_REQUEST_MRD },      // DHCPV6_CLIENT_MSG_TYPE_REQUEST
    {  TCPIP_DHCPV6_CONFIRM_IDELAY,    TCPIP_DHCPV6_CONFIRM_IRT,    TCPIP_DHCPV6_CONFIRM_MRT,    TCPIP_DHCPV6_CONFIRM_MRC,           TCPIP_DHCPV6_CONFIRM_MRD },      // DHCPV6_CLIENT_MSG_TYPE_CONFIRM
    {  TCPIP_DHCPV6_RENEW_IDELAY,      TCPIP_DHCPV6_RENEW_IRT,      TCPIP_DHCPV6_RENEW_MRT,      TCPIP_DHCPV6_RENEW_MRC,             TCPIP_DHCPV6_RENEW_MRD },        // DHCPV6_CLIENT_MSG_TYPE_RENEW
    {  TCPIP_DHCPV6_REBIND_IDELAY,     TCPIP_DHCPV6_REBIND_IRT,     TCPIP_DHCPV6_REBIND_MRT,     TCPIP_DHCPV6_REBIND_MRC,            TCPIP_DHCPV6_REBIND_MRD },       // DHCPV6_CLIENT_MSG_TYPE_REBIND
    {  TCPIP_DHCPV6_RELEASE_IDELAY,    TCPIP_DHCPV6_RELEASE_IRT,    TCPIP_DHCPV6_RELEASE_MRT,    TCPIP_DHCPV6_RELEASE_MRC,           TCPIP_DHCPV6_RELEASE_MRD },      // DHCPV6_CLIENT_MSG_TYPE_RELEASE
    {  TCPIP_DHCPV6_DECLINE_IDELAY,    TCPIP_DHCPV6_DECLINE_IRT,    TCPIP_DHCPV6_DECLINE_MRT,    TCPIP_DHCPV6_DECLINE_MRC,           TCPIP_DHCPV6_DECLINE_MRD },      // DHCPV6_CLIENT_MSG_TYPE_DECLINE
    {  TCPIP_DHCPV6_INFO_REQ_IDELAY,   TCPIP_DHCPV6_INFO_REQ_IRT,   TCPIP_DHCPV6_INFO_REQ_IRT,   TCPIP_DHCPV6_INFO_REQ_IRT,          TCPIP_DHCPV6_INFO_REQ_IRT },     // DHCPV6_CLIENT_MSG_TYPE_INFO_REQUEST
};

// complete DHCPV6 messages option mask
// contains all the options that could be part of the messages
static const TCPIP_DHCPV6_OPTION_MASK_SET_0 DHCPV6_MSG_OPTION_MASK_TBL[DHCPV6_CLIENT_MSG_TYPE_NUMBER] =
{
    {                       // DHCPV6_CLIENT_MSG_TYPE_SOLICIT
        1,
        {
            (
             (1 << DHCPV6_OPT_CODE_CLIENT_ID) |
             (1 << DHCPV6_OPT_CODE_IA_NA) |
             (1 << DHCPV6_OPT_CODE_IA_TA) |
             (1 << DHCPV6_OPT_CODE_OPTION_REQ) |
             (1 << DHCPV6_OPT_CODE_ELAPSED_TIME) |
             (1 << DHCPV6_OPT_CODE_RAPID_COMMIT) 
            ),
        }
    },

    {                       // DHCPV6_CLIENT_MSG_TYPE_REQUEST
        1,
        {
            (
             (1 << DHCPV6_OPT_CODE_CLIENT_ID) |
             (1 << DHCPV6_OPT_CODE_SERVER_ID) |
             (1 << DHCPV6_OPT_CODE_IA_NA) |
             (1 << DHCPV6_OPT_CODE_IA_TA) |
             (1 << DHCPV6_OPT_CODE_OPTION_REQ) |
             (1 << DHCPV6_OPT_CODE_ELAPSED_TIME)
            ),
        }
    },
    
    {                       // DHCPV6_CLIENT_MSG_TYPE_CONFIRM
        1,
        {
            (
             (1 << DHCPV6_OPT_CODE_CLIENT_ID) |
             (1 << DHCPV6_OPT_CODE_IA_NA) |
             (1 << DHCPV6_OPT_CODE_IA_TA) |
             (1 << DHCPV6_OPT_CODE_OPTION_REQ) |
             (1 << DHCPV6_OPT_CODE_ELAPSED_TIME)
            ),
        }
    },

    {                       // DHCPV6_CLIENT_MSG_TYPE_RENEW
        1,
        {
            (
             (1 << DHCPV6_OPT_CODE_CLIENT_ID) |
             (1 << DHCPV6_OPT_CODE_SERVER_ID) |
             (1 << DHCPV6_OPT_CODE_IA_NA) |
             (1 << DHCPV6_OPT_CODE_IA_TA) |
             (1 << DHCPV6_OPT_CODE_OPTION_REQ) |
             (1 << DHCPV6_OPT_CODE_ELAPSED_TIME)
            ),
        }
    },


    {                       // DHCPV6_CLIENT_MSG_TYPE_REBIND
        1,
        {
            (
             (1 << DHCPV6_OPT_CODE_CLIENT_ID) |
             (1 << DHCPV6_OPT_CODE_IA_NA) |
             (1 << DHCPV6_OPT_CODE_IA_TA) |
             (1 << DHCPV6_OPT_CODE_OPTION_REQ) |
             (1 << DHCPV6_OPT_CODE_ELAPSED_TIME)
            ),
        }
    },

    {                       // DHCPV6_CLIENT_MSG_TYPE_RELEASE
        1,
        {
            (
             (1 << DHCPV6_OPT_CODE_CLIENT_ID) |
             (1 << DHCPV6_OPT_CODE_SERVER_ID) |
             (1 << DHCPV6_OPT_CODE_IA_NA) |
             (1 << DHCPV6_OPT_CODE_IA_TA) |
             (1 << DHCPV6_OPT_CODE_OPTION_REQ) |
             (1 << DHCPV6_OPT_CODE_ELAPSED_TIME)
            ),
        }
    },

    {                       // DHCPV6_CLIENT_MSG_TYPE_DECLINE
        1,
        {
            (
             (1 << DHCPV6_OPT_CODE_CLIENT_ID) |
             (1 << DHCPV6_OPT_CODE_SERVER_ID) |
             (1 << DHCPV6_OPT_CODE_IA_NA) |
             (1 << DHCPV6_OPT_CODE_IA_TA) |
             (1 << DHCPV6_OPT_CODE_OPTION_REQ) |
             (1 << DHCPV6_OPT_CODE_ELAPSED_TIME)
            ),
        }
    },

    {                       // DHCPV6_CLIENT_MSG_TYPE_INFO_REQUEST
        1,
        {
            (
             (1 << DHCPV6_OPT_CODE_CLIENT_ID) |
             (1 << DHCPV6_OPT_CODE_SERVER_ID) |
             (1 << DHCPV6_OPT_CODE_OPTION_REQ) |
             (1 << DHCPV6_OPT_CODE_ELAPSED_TIME)
            ),
        }
    },

};


// default DHCPV6_OPT_CODE_OPTION_REQ option request mask;
// could be changed by the user
static TCPIP_DHCPV6_OPTION_MASK_SET_3       DHCPV6_ORO_OPTION_MASK_TBL = 
{
    3,
    {
        (
            (1 << DHCPV6_OPT_CODE_DNS_SERVERS) |
            (1 << DHCPV6_OPT_CODE_DOMAIN_LIST)
        ),
        ( 0 ),
        (
            (1 << (DHCPV6_OPT_CODE_MAX_RT % 64)) |
            (1 << (DHCPV6_OPT_CODE_INFO_MAX_RT % 64))
        ),
    },
};



// default/current  DHCPV6 messages option mask
// could be changed by the user
static TCPIP_DHCPV6_OPTION_MASK_SET_0 DHCPV6_MSG_OPTION_CURR_MASK_TBL[DHCPV6_CLIENT_MSG_TYPE_NUMBER] = {};


// state processing function
//
typedef void (*TCPIP_DHCPV6_CLIENT_STATE_PROC_FNC)(TCPIP_DHCPV6_CLIENT_DCPT* pClient);

static void   F_DHCPV6Client_StateProcInit(TCPIP_DHCPV6_CLIENT_DCPT* pClient);
static void   F_DHCPV6Client_StateProcIdle(TCPIP_DHCPV6_CLIENT_DCPT* pClient);
static void   F_DHCPV6Client_StateProcStart(TCPIP_DHCPV6_CLIENT_DCPT* pClient);
static void   F_DHCPV6Client_StateProcRun(TCPIP_DHCPV6_CLIENT_DCPT* pClient);
static void   F_DHCPV6Client_StateProcWaitLink(TCPIP_DHCPV6_CLIENT_DCPT* pClient);
static void   F_DHCPV6Client_StateProcReinit(TCPIP_DHCPV6_CLIENT_DCPT* pClient);

static const TCPIP_DHCPV6_CLIENT_STATE_PROC_FNC    T_DHCPV6Client_StateFncTbl[TCPIP_DHCPV6_CLIENT_STATE_NUMBER] = 
{
    &F_DHCPV6Client_StateProcInit,                  // TCPIP_DHCPV6_CLIENT_STATE_INIT
    &F_DHCPV6Client_StateProcIdle,                  // TCPIP_DHCPV6_CLIENT_STATE_IDLE
    &F_DHCPV6Client_StateProcStart,                 // TCPIP_DHCPV6_CLIENT_STATE_START
    &F_DHCPV6Client_StateProcRun,                   // TCPIP_DHCPV6_CLIENT_STATE_RUN
    &F_DHCPV6Client_StateProcWaitLink,              // TCPIP_DHCPV6_CLIENT_STATE_WAIT_LINK
    &F_DHCPV6Client_StateProcReinit,                // TCPIP_DHCPV6_CLIENT_STATE_REINIT
};

// conversion functions/helpers
//
static __inline__ TCPIP_DHCPV6_IA_DCPT* __attribute__((always_inline)) FC_DblNode2IaDcpt(DBL_LIST_NODE* node)
{
    union
    {
        DBL_LIST_NODE* node;
        TCPIP_DHCPV6_IA_DCPT*   pDcpt;
    }U_DBL_NODE_IA_DCPT;

    U_DBL_NODE_IA_DCPT.node = node;
    return U_DBL_NODE_IA_DCPT.pDcpt;
}

static __inline__ DBL_LIST_NODE* __attribute__((always_inline)) FC_IaDcpt2DblNode(TCPIP_DHCPV6_IA_DCPT* pDcpt)
{
    union
    {
        TCPIP_DHCPV6_IA_DCPT*   pDcpt;
        DBL_LIST_NODE* node;
    }U_IA_DCPT_DBL_NODE;

    U_IA_DCPT_DBL_NODE.pDcpt = pDcpt;
    return U_IA_DCPT_DBL_NODE.node;
}

static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_MsgBuff2SglNode(TCPIP_DHCPV6_MSG_BUFFER* msgBuffer)
{
    union
    {
        TCPIP_DHCPV6_MSG_BUFFER*   msgBuffer;
        SGL_LIST_NODE* node;
    }U_MSG_BUFF_SGL_NODE;

    U_MSG_BUFF_SGL_NODE.msgBuffer = msgBuffer;
    return U_MSG_BUFF_SGL_NODE.node;
}

static __inline__ TCPIP_DHCPV6_MSG_BUFFER* __attribute__((always_inline)) FC_SglNode2MsgBuff(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE* node;
        TCPIP_DHCPV6_MSG_BUFFER*   msgBuffer;
    }U_SGL_NODE_MSG_BUFF;

    U_SGL_NODE_MSG_BUFF.node = node;
    return U_SGL_NODE_MSG_BUFF.msgBuffer;
}

static __inline__ TCPIP_DHCPV6_MSG_BUFFER* __attribute__((always_inline)) FC_U8Ptr2MsgBuff(uint8_t* u8Ptr)
{
    union
    {
        uint8_t* u8Ptr;
        TCPIP_DHCPV6_MSG_BUFFER*   msgBuffer;
    }U_U8_PTR_MSG_BUFF;

    U_U8_PTR_MSG_BUFF.u8Ptr = u8Ptr;
    return U_U8_PTR_MSG_BUFF.msgBuffer;
}

static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_AddNode2SglNode(TCPIP_DHCPV6_ADDR_NODE* addNode)
{
    union
    {
        TCPIP_DHCPV6_ADDR_NODE*   addNode;
        SGL_LIST_NODE* node;
    }U_ADD_NODE_SGL_NODE;

    U_ADD_NODE_SGL_NODE.addNode = addNode;
    return U_ADD_NODE_SGL_NODE.node;
}

static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_DhcpHandle2SglNode(TCPIP_DHCPV6_HANDLE hDhcp)
{
    union
    {
        TCPIP_DHCPV6_HANDLE hDhcp;
        SGL_LIST_NODE* node;
    }U_DHCP_HNDL_SGL_NODE;

    U_DHCP_HNDL_SGL_NODE.hDhcp = hDhcp;
    return U_DHCP_HNDL_SGL_NODE.node;
}

static __inline__ TCPIP_DHCPV6_ADDR_NODE* __attribute__((always_inline)) FC_SglNode2AddNode(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE* node;
        TCPIP_DHCPV6_ADDR_NODE* addNode;
    }U_SGL_NODE_ADD_NODE;

    U_SGL_NODE_ADD_NODE.node = node;
    return U_SGL_NODE_ADD_NODE.addNode;
}

static __inline__ TCPIP_DHCPV6_LIST_NODE* __attribute__((always_inline)) FC_SglNode2ListNode(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE* node;
        TCPIP_DHCPV6_LIST_NODE* listNode;
    }U_SGL_NODE_LIST_NODE;

    U_SGL_NODE_LIST_NODE.node = node;
    return U_SGL_NODE_LIST_NODE.listNode;
}


static __inline__ TCPIP_DHCPV6_MESSAGE_HEADER* __attribute__((always_inline)) FC_U8Ptr2MsgHdr(uint8_t* u8Ptr)
{
    union
    {
        uint8_t* u8Ptr;
        TCPIP_DHCPV6_MESSAGE_HEADER* pHdr;
    }U_UPTR_MSG_HDR;

    U_UPTR_MSG_HDR.u8Ptr = u8Ptr;
    return U_UPTR_MSG_HDR.pHdr;
}

static __inline__ TCPIP_DHCPV6_OPTION_IANA* __attribute__((always_inline)) FC_U8Ptr2OptIana(uint8_t* u8Ptr)
{
    union
    {
        uint8_t* u8Ptr;
        TCPIP_DHCPV6_OPTION_IANA* pOptIana;
    }U_UPTR_OPT_IANA;

    U_UPTR_OPT_IANA.u8Ptr = u8Ptr;
    return U_UPTR_OPT_IANA.pOptIana;
}

static __inline__ TCPIP_DHCPV6_OPTION_IANA* __attribute__((always_inline)) FC_VPtr2OptIana(void* vPtr)
{
    union
    {
        void* vPtr;
        TCPIP_DHCPV6_OPTION_IANA* pOptIana;
    }U_VPTR_OPT_IANA;

    U_VPTR_OPT_IANA.vPtr = vPtr;
    return U_VPTR_OPT_IANA.pOptIana;
}

static __inline__ TCPIP_DHCPV6_OPTION_IATA* __attribute__((always_inline)) FC_U8Ptr2OptIata(uint8_t* u8Ptr)
{
    union
    {
        uint8_t* u8Ptr;
        TCPIP_DHCPV6_OPTION_IATA* pOptIata;
    }U_UPTR_OPT_IATA;

    U_UPTR_OPT_IATA.u8Ptr = u8Ptr;
    return U_UPTR_OPT_IATA.pOptIata;
}

static __inline__ TCPIP_DHCPV6_OPTION_IATA* __attribute__((always_inline)) FC_VPtr2OptIata(void* vPtr)
{
    union
    {
        void* vPtr;
        TCPIP_DHCPV6_OPTION_IATA* pOptIata;
    }U_VPTR_OPT_IATA;

    U_VPTR_OPT_IATA.vPtr = vPtr;
    return U_VPTR_OPT_IATA.pOptIata;
}

static __inline__ TCPIP_DHCPV6_OPTION_ID_TYPE* __attribute__((always_inline)) FC_OptId2OptType(TCPIP_DHCPV6_OPTION_ID* pOptId)
{
    union
    {
        TCPIP_DHCPV6_OPTION_ID* pOptId;
        TCPIP_DHCPV6_OPTION_ID_TYPE* pOptType;
    }U_OPT_ID_OPT_TYPE;

    U_OPT_ID_OPT_TYPE.pOptId = pOptId;
    return U_OPT_ID_OPT_TYPE.pOptType;
}

static __inline__ TCPIP_DHCPV6_IA_SUBSTATE __attribute__((always_inline)) FC_U162IaSubst(uint16_t val)
{
    union
    {
        uint16_t val;
        TCPIP_DHCPV6_IA_SUBSTATE subState;
    }U_U16_IA_SUBST;

    U_U16_IA_SUBST.val = val;
    return U_U16_IA_SUBST.subState;
}


static __inline__ TCPIP_DHCPV6_IA_STATE __attribute__((always_inline)) FC_U162IaState(uint16_t val)
{
    union
    {
        uint16_t val;
        TCPIP_DHCPV6_IA_STATE state;
    }U_U16_IA_STATE;

    U_U16_IA_STATE.val = val;
    return U_U16_IA_STATE.state;
}


// run state processing functions
//
// run substate processing function
// it returns a substate result and UPDATES the pIa->iaSubState!
typedef TCPIP_DHCPV6_IA_SUBSTATE_RESULT (*TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC)(TCPIP_DHCPV6_IA_DCPT* pIa);

// standard substate processing functions
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   F_DHCPV6Ia_SubStateIDelay(TCPIP_DHCPV6_IA_DCPT* pIa);
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   F_DHCPV6Ia_SubStateTransmit(TCPIP_DHCPV6_IA_DCPT* pIa);


// SOLICIT substate processing function table
// most run states that send messages uses these states 
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   F_DHCPV6Ia_SubStateSolicitStart(TCPIP_DHCPV6_IA_DCPT* pIa);
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   F_DHCPV6Ia_SubStateSolicitationWaitReply(TCPIP_DHCPV6_IA_DCPT* pIa);

static const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC T_DHCPV6Ia_SubStateSolicitFTbl[] =
{
    &F_DHCPV6Ia_SubStateSolicitStart,            // TCPIP_DHCPV6_IA_SUBSTATE_START    
    &F_DHCPV6Ia_SubStateIDelay,                  // TCPIP_DHCPV6_IA_SUBSTATE_IDELAY   
    &F_DHCPV6Ia_SubStateTransmit,                // TCPIP_DHCPV6_IA_SUBSTATE_TRANSMIT 
    &F_DHCPV6Ia_SubStateSolicitationWaitReply,   // TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY
};

// REQUEST substate processing function table
// most run states that send messages uses these states 
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   F_DHCPV6Ia_SubStateRequestStart(TCPIP_DHCPV6_IA_DCPT* pIa);
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   F_DHCPV6Ia_SubStateWaitReplyMsg(TCPIP_DHCPV6_IA_DCPT* pIa);

static const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC T_DHCPV6Ia_SubStateRequestFTbl[] =
{
    &F_DHCPV6Ia_SubStateRequestStart,            // TCPIP_DHCPV6_IA_SUBSTATE_START    
    &F_DHCPV6Ia_SubStateIDelay,                  // TCPIP_DHCPV6_IA_SUBSTATE_IDELAY   
    &F_DHCPV6Ia_SubStateTransmit,                // TCPIP_DHCPV6_IA_SUBSTATE_TRANSMIT 
    &F_DHCPV6Ia_SubStateWaitReplyMsg,            // TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY
};


// DAD substate processing function table
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   F_DHCPV6Ia_SubStateDadStart(TCPIP_DHCPV6_IA_DCPT* pIa);
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   F_DHCPV6Ia_SubStateDadWait(TCPIP_DHCPV6_IA_DCPT* pIa);


static const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC T_DHCPV6Ia_SubStateDadFTbl[] =
{
    &F_DHCPV6Ia_SubStateDadStart,     
    &F_DHCPV6Ia_SubStateDadWait, 
};

// DECLINE substate processing function table
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   F_DHCPV6Ia_SubStateDeclineStart(TCPIP_DHCPV6_IA_DCPT* pIa);

static const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC T_DHCPV6Ia_SubStateDeclineFTbl[] =
{
    &F_DHCPV6Ia_SubStateDeclineStart,            // TCPIP_DHCPV6_IA_SUBSTATE_START    
    &F_DHCPV6Ia_SubStateIDelay,                  // TCPIP_DHCPV6_IA_SUBSTATE_IDELAY   
    &F_DHCPV6Ia_SubStateTransmit,                // TCPIP_DHCPV6_IA_SUBSTATE_TRANSMIT 
    &F_DHCPV6Ia_SubStateWaitReplyMsg,            // TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY
};


// BOUND substate processing function table
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   F_DHCPV6Ia_SubStateBoundWait(TCPIP_DHCPV6_IA_DCPT* pIa);
static const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC T_DHCPV6Ia_SubStateBoundFTbl[] =
{
    &F_DHCPV6Ia_SubStateBoundWait,        // substate 0
};

// RENEW substate processing function table
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   F_DHCPV6Ia_SubStateRenewStart(TCPIP_DHCPV6_IA_DCPT* pIa);

static const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC T_DHCPV6Ia_SubStateRenewFTbl[] =
{
    &F_DHCPV6Ia_SubStateRenewStart,               // TCPIP_DHCPV6_IA_SUBSTATE_START    
    &F_DHCPV6Ia_SubStateIDelay,                   // TCPIP_DHCPV6_IA_SUBSTATE_IDELAY   
    &F_DHCPV6Ia_SubStateTransmit,                 // TCPIP_DHCPV6_IA_SUBSTATE_TRANSMIT 
    &F_DHCPV6Ia_SubStateWaitReplyMsg,             // TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY
};

// REBIND substate processing function table
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   F_DHCPV6Ia_SubStateRebindStart(TCPIP_DHCPV6_IA_DCPT* pIa);

static const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC T_DHCPV6Ia_SubStateRebindFTbl[] =
{
    &F_DHCPV6Ia_SubStateRebindStart,              // TCPIP_DHCPV6_IA_SUBSTATE_START    
    &F_DHCPV6Ia_SubStateIDelay,                   // TCPIP_DHCPV6_IA_SUBSTATE_IDELAY   
    &F_DHCPV6Ia_SubStateTransmit,                 // TCPIP_DHCPV6_IA_SUBSTATE_TRANSMIT 
    &F_DHCPV6Ia_SubStateWaitReplyMsg,             // TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY
};


// CONFIRM substate processing function table
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   F_DHCPV6Ia_SubStateConfirmStart(TCPIP_DHCPV6_IA_DCPT* pIa);

static const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC T_DHCPV6Ia_SubStateConfirmFTbl[] =
{
    &F_DHCPV6Ia_SubStateConfirmStart,             // TCPIP_DHCPV6_IA_SUBSTATE_START    
    &F_DHCPV6Ia_SubStateIDelay,                   // TCPIP_DHCPV6_IA_SUBSTATE_IDELAY   
    &F_DHCPV6Ia_SubStateTransmit,                 // TCPIP_DHCPV6_IA_SUBSTATE_TRANSMIT 
    &F_DHCPV6Ia_SubStateWaitReplyMsg,             // TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY
};

// RELEASE substate processing function table
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   F_DHCPV6Ia_SubStateReleaseStart(TCPIP_DHCPV6_IA_DCPT* pIa);

static const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC T_DHCPV6Ia_SubStateReleaseFTbl[] =
{
    &F_DHCPV6Ia_SubStateReleaseStart,             // TCPIP_DHCPV6_IA_SUBSTATE_START    
    &F_DHCPV6Ia_SubStateIDelay,                   // TCPIP_DHCPV6_IA_SUBSTATE_IDELAY   
    &F_DHCPV6Ia_SubStateTransmit,                 // TCPIP_DHCPV6_IA_SUBSTATE_TRANSMIT 
    &F_DHCPV6Ia_SubStateWaitReplyMsg,             // TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY
};


// ERROR_TRANSIENT substate processing function table
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   F_DHCPV6Ia_SubStateErrorTransient(TCPIP_DHCPV6_IA_DCPT* pIa);

static const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC T_DHCPV6Ia_SubStateTransientFTbl[] =
{
    &F_DHCPV6Ia_SubStateErrorTransient,
};
// ERROR_FATAL substate processing function table
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   F_DHCPV6Ia_SubStateErrorFatal(TCPIP_DHCPV6_IA_DCPT* pIa);

static const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC T_DHCPV6Ia_SubStateFatalFTbl[] =
{
    &F_DHCPV6Ia_SubStateErrorFatal,
};



// table with substate processing functions for each run state
//
static const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC*    T_DHCPV6Ia_StateProcTbl[TCPIP_DHCPV6_IA_STATE_NUMBER] = 
{
    // *subStateFnc
    // 
    T_DHCPV6Ia_SubStateSolicitFTbl,        // TCPIP_DHCPV6_IA_STATE_SOLICIT
    T_DHCPV6Ia_SubStateRequestFTbl,        // TCPIP_DHCPV6_IA_STATE_REQUEST   
    T_DHCPV6Ia_SubStateDadFTbl,            // TCPIP_DHCPV6_IA_STATE_DAD
    T_DHCPV6Ia_SubStateDeclineFTbl,        // TCPIP_DHCPV6_IA_STATE_DECLINE
    T_DHCPV6Ia_SubStateBoundFTbl,          // TCPIP_DHCPV6_IA_STATE_BOUND
    T_DHCPV6Ia_SubStateRenewFTbl,          // TCPIP_DHCPV6_IA_STATE_RENEW
    T_DHCPV6Ia_SubStateRebindFTbl,         // TCPIP_DHCPV6_IA_STATE_REBIND
    T_DHCPV6Ia_SubStateConfirmFTbl,        // TCPIP_DHCPV6_IA_STATE_CONFIRM
    T_DHCPV6Ia_SubStateReleaseFTbl,        // TCPIP_DHCPV6_IA_STATE_RELEASE
    T_DHCPV6Ia_SubStateTransientFTbl,      // TCPIP_DHCPV6_IA_STATE_ERROR_TRANSIENT
    T_DHCPV6Ia_SubStateFatalFTbl,          // TCPIP_DHCPV6_IA_STATE_ERROR_FATAL
};


// task execution function for client
// for errors it updates the pClient->state
typedef void (*TCPIP_DHCPV6_CLIENT_TASK_FNC)(TCPIP_DHCPV6_CLIENT_DCPT* pClient);

static void         F_DHCPV6Client_TransmitTask(TCPIP_DHCPV6_CLIENT_DCPT* pClient);
static void         F_DHCPV6Client_ReceiveTask(TCPIP_DHCPV6_CLIENT_DCPT* pClient);
static void         F_DHCPV6Client_ProcessTask(TCPIP_DHCPV6_CLIENT_DCPT* pClient);


// table with the client tasks funtions
static const TCPIP_DHCPV6_CLIENT_TASK_FNC T_DHCPV6Client_TaskTbl[] = 
{
    &F_DHCPV6Client_TransmitTask,
    &F_DHCPV6Client_ReceiveTask,
    &F_DHCPV6Client_ProcessTask,
};


// prototypes
// general functions
static void         TCPIP_DHCPV6_Process(bool isTmo);

static void         F_DHCPV6Close(TCPIP_DHCPV6_CLIENT_DCPT* pClient, uint16_t cFlags);

static void         F_DHCPV6_Release(TCPIP_DHCPV6_CLIENT_DCPT* pClient);
static void         F_DHCPV6_LeasesCleanup(TCPIP_DHCPV6_CLIENT_DCPT* pClient);

static UDP_SOCKET   F_DHCPV6OpenSocket(TCPIP_DHCPV6_CLIENT_DCPT* pClient);

static void         F_DHCPv6FlushSocket(TCPIP_DHCPV6_CLIENT_DCPT* pClient);

// client specific functions
static bool         F_DHCPV6Client_Init(TCPIP_DHCPV6_CLIENT_DCPT* pClient);

static bool         F_DHCPV6Client_Reinit(TCPIP_DHCPV6_CLIENT_DCPT* pClient);

static void         F_DHCPV6Client_LinkConfirm(TCPIP_DHCPV6_CLIENT_DCPT* pClient);

static bool         F_DHCPV6Client_CheckLink(TCPIP_DHCPV6_CLIENT_DCPT* pClient);

static void         F_DHCPV6Client_Notify(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_IA_DCPT* pIa, bool iaSubNotify);

static void         F_DHCPV6Client_SetStateNotify(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_CLIENT_STATE cliState);

static bool         F_DHCPV6Client_FindIaById(TCPIP_DHCPV6_CLIENT_DCPT* pClient, uint32_t iaid, TCPIP_DHCPV6_IA_TYPE type, TCPIP_DHCPV6_IA_STATE srchState);

static TCPIP_DHCPV6_IA_DCPT* F_DHCPV6Client_FindIaByValidAddr(TCPIP_DHCPV6_CLIENT_DCPT* pClient, const IPV6_ADDR* addr);

// IA specific functions
static void         F_DHCPV6Ia_TaskExecute(TCPIP_DHCPV6_CLIENT_DCPT* pClient);

static TCPIP_DHCPV6_IA_SUBSTATE_RESULT F_DHCPV6Ia_TxMsgSetup(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_CLIENT_MSG_TYPE cliMsgType);

static int          F_DHCPV6Ia_MessageWrite(TCPIP_DHCPV6_IA_DCPT* pIa);

static TCPIP_DHCPV6_MSG_TX_RESULT   F_DHCPV6Ia_CheckMsgTransmitStatus(TCPIP_DHCPV6_IA_DCPT* pIa);

static void         F_DHCPV6Ia_IdGenerate(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_NET_IF* pIaidIf, TCPIP_DHCPV6_IA_ROUND* pIaRound);

static TCPIP_DHCPV6_IA_SUBSTATE_RESULT  F_DHCPV6Ia_ReplyMsgSrvMatchProcess(TCPIP_DHCPV6_IA_DCPT* pIa);

static TCPIP_DHCPV6_IA_SUBSTATE_RESULT F_DHCPV6Ia_ProcessSrvMatchMsg(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer);

static TCPIP_DHCPV6_IA_SUBSTATE_RESULT  F_DHCPV6Ia_ReplyConfirmProcess(TCPIP_DHCPV6_IA_DCPT* pIa);

static TCPIP_DHCPV6_IA_SUBSTATE_RESULT  F_DHCPV6Ia_ReplyRebindProcess(TCPIP_DHCPV6_IA_DCPT* pIa);

static TCPIP_DHCPV6_IA_SUBSTATE_RESULT  F_DHCPV6Ia_AdvertiseSelect(TCPIP_DHCPV6_IA_DCPT* pIa, bool isTmo);

static TCPIP_DHCPV6_IA_SUBSTATE_RESULT  F_DHCPV6Ia_AdvertiseCopy(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_IA_DCPT* pIa);

static void         F_DHCPV6Ia_SolicitInit(TCPIP_DHCPV6_IA_DCPT* pIa);

static void         F_DHCPV6Ia_MsgListPurge(SINGLE_LIST* pL, TCPIP_DHCPV6_IA_DCPT* pIa);

static void         F_DHCPV6Ia_SetRunState(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_IA_STATE iaState, TCPIP_DHCPV6_IA_SUBSTATE iaSubState);

static void         F_DHCPV6Ia_SetTimeValues(TCPIP_DHCPV6_IA_DCPT* pIa, bool setAcqTime);

static void         F_DHCPV6Ia_RestoreTimeValues(TCPIP_DHCPV6_IA_DCPT* pIa);

static void         F_DHCPV6Ia_SetBoundTimes(TCPIP_DHCPV6_IA_DCPT* pIa);

static bool         F_DHCPV6Ia_AddressIsExpired(TCPIP_DHCPV6_IA_DCPT* pIa, bool checkPrefLTime);

static void         F_DHCPV6Ia_Remove(TCPIP_DHCPV6_IA_DCPT* pIa);

static void         F_DHCPV6Ia_ReleaseMsgBuffer(TCPIP_DHCPV6_IA_DCPT* pIa);

static void         F_DHCPV6Ia_MsgInvalidate(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer);

static void         F_DHCPV6Ia_AddressRemove(TCPIP_DHCPV6_IA_DCPT* pIa);

// IA Options specific functions
static int          F_DHCPV6Option_WriteDuid(TCPIP_DHCPV6_DUID_DCPT* pDuid, TCPIP_DHCPV6_MSG_WRITE_DCPT* pWrDcpt);

static int          F_DHCPV6Option_WriteId(TCPIP_DHCPV6_DUID_DCPT* pDuid, uint32_t optCode, TCPIP_DHCPV6_MSG_WRITE_DCPT* pWrDcpt);

static int          F_DHCPV6Option_WriteIA_Addr(TCPIP_DHCPV6_OPTION_IA_ADDR_BODY* pASpec, TCPIP_DHCPV6_MSG_WRITE_DCPT* pWrDcpt);


// Option sets

static uint32_t     F_DHCPV6OptionSet_ExtractCode(const uint32_t* pOptionSet, uint32_t nSets, const uint32_t** ppCurrSet, uint32_t* pCurrMask);

static uint32_t     F_DHCPV6OptionSet_CodesNo(const uint32_t* pOptionSet, uint32_t nSets);


// Generic Option find functions
// 
static void         F_DHCPV6_MsgListForcePurge(TCPIP_DHCPV6_CLIENT_DCPT* pClient, SINGLE_LIST* pL);

static void         F_DHCPV6_MsgListForceRemove(TCPIP_DHCPV6_CLIENT_DCPT* pClient, SINGLE_LIST* pL);

static void         F_DHCPV6_MsgListPurgeAll(TCPIP_DHCPV6_CLIENT_DCPT* pClient);

static DHCPV6_OPTION_GEN_UNION_PTR  F_DHCPV6OptionFind_OptCode(TCPIP_DHCPV6_MSG_SEARCH_DCPT* pSrchDcpt, uint32_t srchCode);

// Specific Option match functions

// match for any IA type: IATA or IANA
static bool         F_DHCPV6OptionMatchFnc_IA(TCPIP_DHCPV6_OPTION_GENERIC* pOptG, uint32_t srchCode);

// Specific Option find functions

static void*        F_DHCPV6OptionFind_Ia(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_IA_DCPT* pIa, bool serverMatch);

static bool         F_DHCPV6OptionFind_RapidCommit(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer);


static bool         F_DHCPV6OptionFind_ServerUnicast(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, IPV6_ADDR* pUniAdd);



// Option get
// 
static bool         F_DHCPV6OptionGet_Duid(TCPIP_DHCPV6_DUID_DCPT* pDstDuidDcpt, TCPIP_DHCPV6_OPTION_ID* pSrcOptId);

static bool         F_DHCPV6OptionGet_IaAddress(TCPIP_DHCPV6_OPTION_IA_ADDR_BODY* pAddBody, TCPIP_DHCPV6_OPTION_IA_ADDR* pSrcIaAddr);

static bool         F_DHCPV6OptionGet_IaBody(TCPIP_DHCPV6_IA_BODY* pIaBody, void* pSrcOptIa, TCPIP_DHCPV6_IA_TYPE type);



// Option get from messages
static bool         F_DHCPV6MsgGet_Duid(TCPIP_DHCPV6_DUID_DCPT* pDstDuid, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, bool isClient);

static bool         F_DHCPV6MsgGet_IaBody(TCPIP_DHCPV6_IA_BODY* pIaBody, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, TCPIP_DHCPV6_IA_DCPT* pIa, bool serverMatch);

static bool         F_DHCPV6MsgGet_IaOptBuffer(TCPIP_DHCPV6_MSG_BUFFER* pDstBuffer, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, TCPIP_DHCPV6_IA_DCPT* pIa);

static bool         F_DHCPV6MsgGet_IaAddresses(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer);

static uint16_t     F_DHCPV6MsgGet_ServerPref(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer);

static bool         F_DHCPV6MsgCheck_TransactionId(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer);

static TCPIP_DHCPV6_SERVER_STATUS_CODE F_DHCPV6MsgGet_StatusCode(uint8_t* statusBuffer, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, TCPIP_DHCPV6_IA_DCPT* pIa, uint16_t statusBufferSize, uint16_t* pStatusMsgSize);

static void         F_DHCPV6MsgGet_Options(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer);

static TCPIP_DHCPV6_SERVER_STATUS_CODE F_DHCPV6MsgGet_IaOptStatusCode(uint8_t* statusBuffer, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, TCPIP_DHCPV6_IA_DCPT* pIa, uint16_t statusBufferSize, uint16_t* pStatusMsgSize);

static bool         F_DHCPV6MsgGet_LeaseParams(TCPIP_DHCPV6_IA_DCPT* pDstIa, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, bool serverMatch);

// retrieving options from a server message


typedef struct S_tag_DHCPV6MsgGet_OptionEntry
{
    uint32_t    optCode;
    void(*optFnc)(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, const struct S_tag_DHCPV6MsgGet_OptionEntry* pEntry);
}S_DHCPV6MsgGet_OptionEntry;

static void F_DHCPV6MsgGet_DnsServers(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, const S_DHCPV6MsgGet_OptionEntry* pEntry);
static void F_DHCPV6MsgGet_DomainList(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, const S_DHCPV6MsgGet_OptionEntry* pEntry);
static void F_DHCPV6MsgGet_SolMaxRt(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, const S_DHCPV6MsgGet_OptionEntry* pEntry);
static void F_DHCPV6MsgGet_InfoMaxRt(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, const S_DHCPV6MsgGet_OptionEntry* pEntry);


// table with server transmitted options that this client processes
// if retrieveing an option fails, the option is silently ignored
// Table entries having optFnc == 0 are unused
static const S_DHCPV6MsgGet_OptionEntry    T_DHCPV6MsgGet_OptionRetrieveTbl[] = 
{
    // optCode                              // optFnc
    {DHCPV6_OPT_CODE_DNS_SERVERS,     &F_DHCPV6MsgGet_DnsServers},
    {DHCPV6_OPT_CODE_DOMAIN_LIST,     &F_DHCPV6MsgGet_DomainList},
    {DHCPV6_OPT_CODE_MAX_RT,          &F_DHCPV6MsgGet_SolMaxRt},
    {DHCPV6_OPT_CODE_INFO_MAX_RT,     &F_DHCPV6MsgGet_InfoMaxRt},

};


// Other helpers
//
static int          F_DHCPV6MsgFind_InUseIAs(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_IA_DCPT* pIa);

static bool         F_DHCPV6Duid_Generate(TCPIP_DHCPV6_DUID_TYPE duidType, TCPIP_DHCPV6_DUID_DCPT* pDuid, TCPIP_NET_IF* pDuidIf);

static bool         F_DHCPV6Duid_Compare(const TCPIP_DHCPV6_DUID_DCPT* pDuid1, const TCPIP_DHCPV6_DUID_DCPT* pDuid2);

static bool         F_DHCPV6MsgValidate(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_CLIENT_DCPT* pClient);

static uint32_t     F_DHCPv6_FindIAsInList(DOUBLE_LIST* pSrchList, bool strict);

// general utilities
//

// DHCPv6 lock for shared access
static __inline__ OSAL_CRITSECT_DATA_TYPE __attribute__((always_inline)) F_DHCPv6_Lock(void)
{
    return OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
}

static __inline__ void __attribute__((always_inline)) F_DHCPv6_Unlock(OSAL_CRITSECT_DATA_TYPE lock)
{
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, lock);   
}

// sets the IA state, sub state
static __inline__ void __attribute__((always_inline)) F_DHCPV6Ia_SetState(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_IA_STATE iaState, TCPIP_DHCPV6_IA_SUBSTATE iaSubState)
{
    pIa->iaState = (uint16_t)iaState;
    pIa->iaSubState = (uint16_t)iaSubState;
}

// updates the state and the Flags
// returns the old state
static TCPIP_DHCPV6_CLIENT_STATE F_DHCPV6Client_SetStateFlags(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_CLIENT_STATE cliState, uint16_t runFlags)
{
    pClient->flags.val = runFlags;

    TCPIP_DHCPV6_CLIENT_STATE oldState = (TCPIP_DHCPV6_CLIENT_STATE)pClient->state;
    pClient->state = (uint16_t)cliState;

    return oldState;
}

// level In debugging
#if ((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_IN) != 0)

static void F_DHCPV6DbgDUID_Print(TCPIP_DHCPV6_DUID_DCPT* pDuid)
{
    // 20 bytes print header + EN identifier or  MAC address 
    char    duidBuff[20 + ((TCPIP_DHCPV6_DUID_EN_IDENTIFIER_LENGTH + 8 ) < 18 ? 18 : TCPIP_DHCPV6_DUID_EN_IDENTIFIER_LENGTH + 8)];
    char*   pBuff;
    char*   endBuff = duidBuff + sizeof(duidBuff);

    switch(pDuid->duidType)
    {

        case TCPIP_DHCPV6_DUID_TYPE_LL:
            {
                const char hdr[] = "DUID LL - ";
                pBuff = duidBuff + FC_sprintf(duidBuff, sizeof(duidBuff), "%s", hdr);
                TCPIP_Helper_MACAddressToString((TCPIP_MAC_ADDR*)pDuid->duidBody.duidLL.ll_address, pBuff, endBuff - pBuff);
            }
            break;


        case TCPIP_DHCPV6_DUID_TYPE_LLT:
            {
                const char hdr[] = "DUID LLT - ";
                pBuff = duidBuff + FC_sprintf(duidBuff, sizeof(duidBuff), "%s", hdr);
                TCPIP_Helper_MACAddressToString((TCPIP_MAC_ADDR*)pDuid->duidBody.duidLL.ll_address, pBuff, endBuff - pBuff);
            }
            break;

        case TCPIP_DHCPV6_DUID_TYPE_EN:
            {
                int ix;
                int identLen = pDuid->duidLen - sizeof(((TCPIP_DHCPV6_DUID_EN*)0)->duid_type) - sizeof(((TCPIP_DHCPV6_DUID_EN*)0)->ent_number);
                pBuff = duidBuff + FC_sprintf(duidBuff, sizeof(duidBuff), "DUID EN: no: 0x%8x, ident: 0x", (unsigned int)pDuid->duidBody.duidEN.ent_number);

                uint8_t* pIdent = pDuid->duidBody.duidEN.identifier;
                for(ix = 0; ix < identLen; ix++)
                {
                    pBuff += FC_sprintf(pBuff, duidBuff + sizeof(duidBuff) - pBuff, "%02x", *pIdent++);
                }
            }
            break;

        default:
            FC_sprintf(duidBuff, sizeof(duidBuff), "DUID: invalid");
            break;
    }

    SYS_CONSOLE_PRINT("%s", duidBuff);
}

static void F_DHCPV6DbgValidate_Print(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, uint16_t msgMask, TCPIP_DHCPV6_MSG_TYPE msgType, TCPIP_DHCPV6_DUID_DCPT* pSrvDuid)
{
    char dhcpBuff[160];
    uint32_t currTime = TCPIP_MsecCountGet();

    if(pSrvDuid != NULL)
    {
        if((msgMask & DHCPV6_VALID_MASK_TYPE) != 0U)
        {
            FC_sprintf(dhcpBuff, sizeof(dhcpBuff),  "DHCPV6_V Msg: 0x%8x, type: %d from server: ", (unsigned int)pMsgBuffer, msgType);
        }
        else
        {
            FC_sprintf(dhcpBuff, sizeof(dhcpBuff), "DHCPV6_V Msg: 0x%8x type: unknown from server: ", (unsigned int)pMsgBuffer);
        }
        SYS_CONSOLE_PRINT("%s", dhcpBuff);
        F_DHCPV6DbgDUID_Print(pSrvDuid);
    }

    if(msgMask == TCPIP_DHCPV6_MSG_VALIDATION_MASK)
    {   // all's well
        FC_sprintf(dhcpBuff, sizeof(dhcpBuff), " DHCPV6_V Msg valid - mask: 0x%2x, time: %zu\r\n", (unsigned int)msgMask, (size_t)currTime);
    }
    else
    {   // failed
        FC_sprintf(dhcpBuff, sizeof(dhcpBuff), " DHCPV6_V Msg invalid - mask: 0x%2x, time: %zu\r\n", (unsigned int)msgMask, (size_t)currTime);
    }
    SYS_CONSOLE_PRINT("%s", dhcpBuff);
} 

static void F_DHCPV6DbgMsgIn_PrintPassed(const char* task, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_MSG_TYPE msgType)
{
    char dhcpBuff[160];
    uint32_t currTime = TCPIP_MsecCountGet();
    FC_sprintf(dhcpBuff, sizeof(dhcpBuff), "DHCPV6 %s success - , Msg: 0x%8x, type: %d, passed, time: %zu\r\n", task, (unsigned int)pMsgBuffer, msgType, (size_t)currTime);
    SYS_CONSOLE_PRINT("%s", dhcpBuff);
}

static void F_DHCPV6DbgMsgIn_PrintFailed(const char* task, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_MSG_TYPE msgType, const char* reason)
{
    char dhcpBuff[160];
    uint32_t currTime = TCPIP_MsecCountGet();
    FC_sprintf(dhcpBuff, sizeof(dhcpBuff), "DHCPV6 %s FAIL - Msg: 0x%8x, type: %d, reason: %s, time: %zu\r\n", task, (unsigned int)pMsgBuffer, msgType, reason, (size_t)currTime);
    SYS_CONSOLE_PRINT("%s", dhcpBuff);
}


#else
#define     F_DHCPV6DbgValidate_Print(pMsgBuffer, msgMask, msgType, pSrvDuid) 
#define     F_DHCPV6DbgMsgIn_PrintPassed(task, pMsgBuffer, msgType)
#define     F_DHCPV6DbgMsgIn_PrintFailed(task, pMsgBuffer, msgType, reason)
#endif  // ((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_IN) != 0)

#if ((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_IA_IN) != 0)
static void F_DHCPV6DbgIAIn_PrintPassed(const char* task, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_IA_DCPT* pIa, uint32_t param)
{
    char dhcpBuff[160];
    uint32_t currTime = TCPIP_MsecCountGet();
    FC_sprintf(dhcpBuff, sizeof(dhcpBuff), "DHCPV6 in - IA success: %s, Msg: 0x%8x, IA: %d, state: %d, param: 0x%x, time: %zu\r\n", task, (unsigned int)pMsgBuffer, pIa->parentIx, pIa->iaState, (unsigned int)param, (size_t)currTime);
    SYS_CONSOLE_PRINT("%s", dhcpBuff);
}

static void F_DHCPV6DbgIAIn_PrintFailed(const char* task, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_IA_DCPT* pIa, const char* reason)
{
    char dhcpBuff[160];
    uint32_t currTime = TCPIP_MsecCountGet();
    FC_sprintf(dhcpBuff, sizeof(dhcpBuff), "DHCPV6 in - IA FAIL: %s, Msg: 0x%8x, IA: %d, state: %d, reason: %s, time: %zu\r\n", task, (unsigned int)pMsgBuffer, pIa->parentIx, pIa->iaState, reason, (size_t)currTime);
    SYS_CONSOLE_PRINT("%s", dhcpBuff);
}

#define     F_DHCPV6DbgDeclare_IAFailReason(reason) const char* reason = 0
#define     F_DHCPV6DbgSet_IAFailReason(reason, msg) do{reason = msg;}while(0)

#else
#define     F_DHCPV6DbgIAIn_PrintPassed(task, pMsgBuffer, pIa, param)
#define     F_DHCPV6DbgIAIn_PrintFailed(task, pMsgBuffer, pIa, reason)
#define     F_DHCPV6DbgDeclare_IAFailReason(reason)
#define     F_DHCPV6DbgSet_IAFailReason(reason, msg)
#endif  // ((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_IA_IN) != 0)

// level Out debugging
#if ((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_OUT) != 0)
static void F_DHCPV6DbgMsgOut_PrintPassed(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_MSG_TYPE msgType, TCPIP_DHCPV6_IA_DCPT* pIa)
{
    char dhcpBuff[160];
    uint32_t currTime = TCPIP_MsecCountGet();
    FC_sprintf(dhcpBuff, sizeof(dhcpBuff), "DHCPV6 transmit success - Msg: 0x%8x, type: %d, IA ix: %d, state: %d, time: %zu\r\n", (unsigned int)pMsgBuffer, msgType, pIa->parentIx, pIa->iaState, (size_t)currTime);
    SYS_CONSOLE_PRINT("%s", dhcpBuff);
}

static void F_DHCPV6DbgMsgOut_PrintFailed(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_MSG_TYPE msgType, TCPIP_DHCPV6_IA_DCPT* pIa, const char* reason)
{
    char dhcpBuff[160];
    uint32_t currTime = TCPIP_MsecCountGet();
    FC_sprintf(dhcpBuff, sizeof(dhcpBuff), "DHCPV6 transmit FAIL - Msg: 0x%8x, type: %d, IA ix: %d, state: %d, reason: %s, time: %zu\r\n", (unsigned int)pMsgBuffer, msgType, pIa->parentIx, pIa->iaState, reason, (size_t)currTime);
    SYS_CONSOLE_PRINT("%s", dhcpBuff);
}
#else
#define F_DHCPV6DbgMsgOut_PrintPassed(pMsgBuffer, msgType, pIa)
#define F_DHCPV6DbgMsgOut_PrintFailed(pMsgBuffer, msgType, pIa, reason)
#endif  // (TCPIP_DHCPV6_DEBUG_LEVEL)


// client state debugging
#if ((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_CLIENT_STATE) != 0) || (((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_CLI_NOTIFY_STATE) != 0) && (M_TCPIP_DHCPV6_USER_NOTIFICATION != 0))
static const char* T_DHCPV6_CLIENT_STATE_NAME[TCPIP_DHCPV6_CLIENT_STATE_NUMBER] = 
{
    "init",         // TCPIP_DHCPV6_CLIENT_STATE_INIT
    "idle",         // TCPIP_DHCPV6_CLIENT_STATE_IDLE
    "start",        // TCPIP_DHCPV6_CLIENT_STATE_START    
    "run",          // TCPIP_DHCPV6_CLIENT_STATE_RUN
    "wlink",        // TCPIP_DHCPV6_CLIENT_STATE_WAIT_LINK
    "reinit",       // TCPIP_DHCPV6_CLIENT_STATE_REINIT
};

static void     F_DHCPV6DbgStatePrint_Client(TCPIP_DHCPV6_CLIENT_DCPT* pClient, bool ignoreCurrent)
{
    if(ignoreCurrent || pClient->state != pClient->prevState)
    {
        char dhcpBuff[160];
        uint32_t currTime = TCPIP_MsecCountGet();
        FC_sprintf(dhcpBuff, sizeof(dhcpBuff), "DHCPV6 Client - state: %s, time: %zu\r\n", T_DHCPV6_CLIENT_STATE_NAME[pClient->state], (size_t)currTime);
        SYS_CONSOLE_PRINT("%s", dhcpBuff);

        pClient->prevState = pClient->state;
    }
    
}

#else
#define         F_DHCPV6DbgStatePrint_Client(pClient, ignoreCurrent)
#endif  // ((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_CLIENT_STATE) != 0) || (((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_CLI_NOTIFY_STATE) != 0) && (M_TCPIP_DHCPV6_USER_NOTIFICATION != 0))


// IA state debugging
#if ((TCPIP_DHCPV6_DEBUG_LEVEL & (DHCPV6_DEBUG_MASK_IA_STATE | DHCPV6_DEBUG_MASK_IA_TMO | DHCPV6_DEBUG_MASK_IA_RTMO)) != 0) || (((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_CLI_NOTIFY_EVENT) != 0) && (M_TCPIP_DHCPV6_USER_NOTIFICATION != 0))
static const char* T_DHCPV6_IA_STATE_NAME[TCPIP_DHCPV6_IA_STATE_NUMBER] = 
{
    "solicit",      // TCPIP_DHCPV6_IA_STATE_SOLICIT,         
    "request",      // TCPIP_DHCPV6_IA_STATE_REQUEST,         
    "dad",          // TCPIP_DHCPV6_IA_STATE_DAD,             
    "decline",      // TCPIP_DHCPV6_IA_STATE_DECLINE,         
    "bound",        // TCPIP_DHCPV6_IA_STATE_BOUND,           
    "renew",        // TCPIP_DHCPV6_IA_STATE_RENEW,           
    "rebind",       // TCPIP_DHCPV6_IA_STATE_REBIND,          
    "confirm",      // TCPIP_DHCPV6_IA_STATE_CONFIRM,         
    "release",      // TCPIP_DHCPV6_IA_STATE_RELEASE,         
    "err-trans",    // TCPIP_DHCPV6_IA_STATE_ERROR_TRANSIENT, 
    "err-fatal",    // TCPIP_DHCPV6_IA_STATE_ERROR_FATAL,     
};
#endif  // ((TCPIP_DHCPV6_DEBUG_LEVEL & (DHCPV6_DEBUG_MASK_IA_STATE | DHCPV6_DEBUG_MASK_IA_TMO | DHCPV6_DEBUG_MASK_IA_RTMO)) != 0) || (((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_CLI_NOTIFY_EVENT) != 0) && (M_TCPIP_DHCPV6_USER_NOTIFICATION != 0))

#if ((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_IA_STATE) != 0) || (((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_CLI_NOTIFY_EVENT) != 0) && (M_TCPIP_DHCPV6_USER_NOTIFICATION != 0))

static const char* T_DHCPV6_IA_TYPE_NAME[TCPIP_DHCPV6_IA_TYPE_NUMBER] = 
{
    "none",         // TCPIP_DHCPV6_IA_TYPE_NONE
    "iana",         // TCPIP_DHCPV6_IA_TYPE_IANA
    "iata",         // TCPIP_DHCPV6_IA_TYPE_IATA
};

static const char* T_DHCPV6_IA_SUBSTATE_NAME[TCPIP_DHCPV6_IA_SUBSTATE_NUMBER] = 
{
    "start",        // TCPIP_DHCPV6_IA_SUBSTATE_START
    "idelay",       // TCPIP_DHCPV6_IA_SUBSTATE_IDELAY
    "transmit",     // TCPIP_DHCPV6_IA_SUBSTATE_TRANSMIT
    "wait-reply",   // TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY
};

static void     F_DHCPV6DbgStatePrint_Ia(TCPIP_DHCPV6_IA_DCPT* pIa, bool iaSubNotify)
{
    (void)iaSubNotify;
    if(pIa != NULL)
    {
        char dhcpBuff[160];
        uint32_t currTime = TCPIP_MsecCountGet();
        FC_sprintf(dhcpBuff, sizeof(dhcpBuff), "DHCPV6 IA: %s, ix: %d, state: %s, sub-state: %s, time: %zu\r\n",T_DHCPV6_IA_TYPE_NAME[pIa->iaBody.type], pIa->parentIx, T_DHCPV6_IA_STATE_NAME[pIa->iaState], T_DHCPV6_IA_SUBSTATE_NAME[pIa->iaSubState], (size_t)currTime);
        SYS_CONSOLE_PRINT("%s", dhcpBuff);
    }
}

#else
#define         F_DHCPV6DbgStatePrint_Ia(pIa, iaSubNotify)
#endif  // ((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_IA_STATE) != 0) || (((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_CLI_NOTIFY_EVENT) != 0) && (M_TCPIP_DHCPV6_USER_NOTIFICATION != 0))

#if ((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_SRV_STATUS_CODE) != 0)
static void F_DHCPV6DbgMsg_ServerStatus(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_SERVER_STATUS_CODE statCode)
{
    if(statCode != TCPIP_DHCPV6_SERVER_STAT_SUCCESS && pIa != NULL)
    {
        uint32_t currTime = TCPIP_MsecCountGet();
        SYS_CONSOLE_PRINT("Server Code: %d, IA ix: %d, IA state: %d, time: %zu\r\n", statCode, pIa->parentIx, pIa->iaState, (size_t)currTime);
    }
}

#else
#define F_DHCPV6DbgMsg_ServerStatus(pIa, statCode)
#endif // ((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_SRV_STATUS_CODE) != 0)

#if ((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_IA_TMO) != 0)
static void F_DHCPV6DbgMsg_IaTxExceed(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_MSG_TX_RESULT txResult, uint32_t iTime)
{
    const char* resMsg = txResult == TCPIP_DHCPV6_MSG_TX_RES_TIME_EXCEEDED ? "timeXcs" : txResult == TCPIP_DHCPV6_MSG_TX_RES_RETRY_EXCEEDED ? "retryXcs" : "unkXcs";
    uint32_t currTime = TCPIP_MsecCountGet();
    SYS_CONSOLE_PRINT("IA TX exceed - result: %s, IA ix: %d, IA state: %s, iTime: %zu, time: %zu\r\n", resMsg, pIa->parentIx, T_DHCPV6_IA_STATE_NAME[pIa->iaState], (size_t)iTime, (size_t)currTime);
}

#else
#define F_DHCPV6DbgMsg_IaTxExceed(pIa, txResult, iTime)
#endif // ((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_IA_TMO) != 0)

#if ((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_IA_RTMO) != 0)
static void F_DHCPV6DbgMsg_IaRTmo(TCPIP_DHCPV6_IA_DCPT* pIa, uint32_t rtmoMs,  uint32_t tPrev, TCPIP_DHCPV6_MSG_TRANSMIT_DCPT* pDcpt)
{
    uint32_t currTime = TCPIP_MsecCountGet();
    SYS_CONSOLE_PRINT("IA RTMO - IA ix: %d, IA state: %s, rtmoMs: %zu, tPrev: %zu, rc: %zu, elapsed: %zu, time: %zu\r\n", pIa->parentIx, T_DHCPV6_IA_STATE_NAME[pIa->iaState], (size_t)rtmoMs, (size_t)tPrev, (size_t)pDcpt->rc, (size_t)pDcpt->elapsedTime, (size_t)currTime);
}

static void F_DHCPV6DbgMsg_IaIDelay(TCPIP_DHCPV6_IA_DCPT* pIa, uint32_t idelayMs)
{
    uint32_t currTime = TCPIP_MsecCountGet();
    SYS_CONSOLE_PRINT("IA set IDELAY - IA ix: %d, IA state: %s, idelayMs: %zu, time: %zu\r\n", pIa->parentIx, T_DHCPV6_IA_STATE_NAME[pIa->iaState], (size_t)idelayMs, (size_t)currTime);
}

static void F_DHCPV6DbgMsg_IaIDelayTmo(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    uint32_t currTime = TCPIP_MsecCountGet();
    SYS_CONSOLE_PRINT("IA IDELAY tmo - IA ix: %d, IA state: %s, time: %zu\r\n", pIa->parentIx, T_DHCPV6_IA_STATE_NAME[pIa->iaState], (size_t)currTime);
}

#else
#define F_DHCPV6DbgMsg_IaRTmo(pIa, rtmoMs, tPrev, pDcpt)
#define F_DHCPV6DbgMsg_IaIDelay(pIa, idelayMs)
#define F_DHCPV6DbgMsg_IaIDelayTmo(pIa)
#endif // ((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_IA_RTMO) != 0)

// additional status prints
#if ((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_IA_ADD_STATE) != 0)
#define         F_DHCPV6DbgStatePrint_IAAdd(fmt, ...)  SYS_CONSOLE_PRINT(fmt, ##__VA_ARGS__)
#else
#define         F_DHCPV6DbgStatePrint_IAAdd(fmt, ...)
#endif  // ((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_IA_ADD_STATE) != 0)

// buffer trace debugging
#if ((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_BUFF_TRACE) != 0)
#define M_DHCPV6_BUFF_STATE_TRACE_SIZE  20
#define M_DHCPV6_BUFFERS_TO_TRACE        2

typedef struct
{
    TCPIP_DHCPV6_IA_STATE    iaState;
    TCPIP_DHCPV6_IA_SUBSTATE iaSubState;
}S_DHCPV6_BUFF_TRACE_STATE;


typedef struct
{
    TCPIP_DHCPV6_MSG_BUFFER* pBuffer;
    int                      stateIx;   // current slot
    S_DHCPV6_BUFF_TRACE_STATE traceState[M_DHCPV6_BUFF_STATE_TRACE_SIZE];
}S_DHCPV6_BUFF_TRACE_DCPT;

S_DHCPV6_BUFF_TRACE_DCPT a_dhcpv6BuffTrace[M_DHCPV6_BUFFERS_TO_TRACE];

static volatile int v_dhcpv6TraceCond = 1;

static void F_DHCPV6TraceBuffInit(int buffIx, TCPIP_DHCPV6_MSG_BUFFER* pBuffer)
{
    (void)memset(a_dhcpv6BuffTrace + buffIx, 0, sizeof(S_DHCPV6_BUFF_TRACE_DCPT));
    a_dhcpv6BuffTrace[buffIx].pBuffer = pBuffer;
} 


void F_DHCPV6TraceBuff(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_MSG_BUFFER* pBuffer)
{
    int buffIx;
    S_DHCPV6_BUFF_TRACE_DCPT* pTrace = a_dhcpv6BuffTrace;
    for(buffIx = 0; buffIx < sizeof(a_dhcpv6BuffTrace)/sizeof(*a_dhcpv6BuffTrace); buffIx++, pTrace++)
    {
        if(pTrace->pBuffer == pBuffer)
        {   // found it
            S_DHCPV6_BUFF_TRACE_STATE* pTraceState = pTrace->traceState + pTrace->stateIx;
            pTraceState->iaState = pIa->iaState;
            pTraceState->iaSubState = pIa->iaSubState;
            pTrace->stateIx++;
            if(pTrace->stateIx == M_DHCPV6_BUFF_STATE_TRACE_SIZE)
            {
                pTrace->stateIx = 0;
            }
            return;
        }
    }

    while(v_dhcpv6TraceCond);

}
#else
#define     F_DHCPV6TraceBuff(pIa, pBuffer)
#define     F_DHCPV6TraceBuffInit(buffIx, pBuffer)
#endif  // (TCPIP_DHCPV6_DEBUG_LEVEL)


#if defined(TCPIP_DHCPV6_STATISTICS_ENABLE) && (TCPIP_DHCPV6_STATISTICS_ENABLE != 0)
static void F_DHCPv6_StatIncrement(TCPIP_DHCPV6_CLIENT_DCPT* pClient, int memberOffset)
{
    TCPIPStack_Assert((memberOffset & 0x3) == 0,__FILE__,  __func__, __LINE__);

    uint32_t* pStat = pClient->statArray + memberOffset / sizeof(uint32_t);
    (*pStat)++;
}
#else
#define F_DHCPv6_StatIncrement(pClient, memberOffset)
#endif  // defined(TCPIP_DHCPV6_STATISTICS_ENABLE) && (TCPIP_DHCPV6_STATISTICS_ENABLE != 0)

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void F_DHCPV6Cleanup(size_t nClients)
{
    if(dhcpv6ClientDcpt != NULL)
    {
        size_t ix;

        TCPIP_DHCPV6_CLIENT_DCPT* pClient = dhcpv6ClientDcpt;
        for(ix = 0; ix < nClients; ix++)
        {
            if(pClient->dhcpSkt != INVALID_UDP_SOCKET)
            {
                (void)TCPIP_UDP_Close(pClient->dhcpSkt);
            }

            if(pClient->msgBuffers != NULL)
            {
                (void)TCPIP_HEAP_Free(dhcpv6MemH, pClient->msgBuffers);
            }

            if(pClient->iaArray != NULL)
            {
                (void)TCPIP_HEAP_Free(dhcpv6MemH, pClient->iaArray);
            }
            pClient++;
        }

        (void)TCPIP_HEAP_Free(dhcpv6MemH, dhcpv6ClientDcpt);
        dhcpv6ClientDcpt = NULL;
    }

#if (M_TCPIP_DHCPV6_USER_NOTIFICATION != 0)
    TCPIP_Notification_Deinitialize(&dhcpv6RegisteredUsers, dhcpv6MemH);
#endif  // (M_TCPIP_DHCPV6_USER_NOTIFICATION != 0)

    if(dhcpv6SignalHandle != NULL)
    {
        TCPIPStackSignalHandlerDeregister(dhcpv6SignalHandle);
        dhcpv6SignalHandle = NULL;
    }
}
#else
#define F_DHCPV6Cleanup(nClients)
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

// API
//
bool TCPIP_DHCPV6_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* initData)
{
    size_t      netIx;
    TCPIP_DHCPV6_CLIENT_DCPT*   pClient;
    
    if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        pClient = dhcpv6ClientDcpt + TCPIPStackNetIxGet(stackCtrl->pNetIf);
        // we should be idle
        TCPIPStack_Assert(pClient->state == (uint16_t)TCPIP_DHCPV6_CLIENT_STATE_IDLE, __FILE__, __func__, __LINE__);
        if(pClient->flags.wasEnabled != 0U)
        {   // DHCP was up when interface went down, restart it 
            (void)F_DHCPV6Client_SetStateFlags(pClient, TCPIP_DHCPV6_CLIENT_STATE_REINIT, TCPIP_DHCPV6_RUN_FLAG_WAS_ENABLED);
        }
        return true;
    }

    // stack init
    
    // check init data is not missing
    if(initData == NULL)
    {
        return false;
    }
    const TCPIP_DHCPV6_MODULE_CONFIG* pDhcpConfig = (const TCPIP_DHCPV6_MODULE_CONFIG*)initData;

    // sanity checks
    //
    if(pDhcpConfig->duidType != (uint16_t)TCPIP_DHCPV6_DUID_TYPE_LL)
    {
        return false;
    }

    if(pDhcpConfig->nIanaDcpts == 0U && pDhcpConfig->nIataDcpts == 0U)
    {
        return false;
    }

    if(pDhcpConfig->defaultIanaT1 > 0U && pDhcpConfig->defaultIanaT2 > 0U)
    {
        if(pDhcpConfig->defaultIanaT1 > pDhcpConfig->defaultIanaT2)
        {
            return false;
        }
    }

    if(pDhcpConfig->defaultIataT1 > 0U && pDhcpConfig->defaultIataT2 > 0U)
    {
        if(pDhcpConfig->defaultIataT1 > pDhcpConfig->defaultIataT2)
        {
            return false;
        }
    }

    if(pDhcpConfig->ianaSolicitT1 > 0U && pDhcpConfig->ianaSolicitT2 > 0U)
    {
        if(pDhcpConfig->ianaSolicitT1 > pDhcpConfig->ianaSolicitT2)
        {
            return false;
        }
    }
    
    const TCPIP_DHCPV6_OPTION_MASK_SET_0* pMsgMaskOpt = DHCPV6_MSG_OPTION_MASK_TBL;
    TCPIP_DHCPV6_OPTION_MASK_SET_0* pMsgMaskCurrOpt = DHCPV6_MSG_OPTION_CURR_MASK_TBL;
    
    // copying the default DHCPV6_MSG_OPTION_MASK_TBL content to DHCPV6_MSG_OPTION_CURR_MASK_TBL
    (void)memcpy(pMsgMaskCurrOpt, pMsgMaskOpt, sizeof(DHCPV6_MSG_OPTION_CURR_MASK_TBL));
    

    if(dhcpv6InitCount == 0)
    {   // first time we're run
        // store the memory allocation handle

        dhcpv6MemH = stackCtrl->memH;

        dhcpv6ClientDcpt = (TCPIP_DHCPV6_CLIENT_DCPT*)TCPIP_HEAP_Calloc(dhcpv6MemH,  stackCtrl->nIfs, sizeof(TCPIP_DHCPV6_CLIENT_DCPT));
        if(dhcpv6ClientDcpt == NULL)
        {   // failed
            return false;
        }

        // create the DHCP signal handler
        dhcpv6SignalHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_DHCPV6_Task, TCPIP_DHCPV6_TASK_TICK_RATE);
        if(dhcpv6SignalHandle == NULL)
        {   // cannot create the DHCP timer
            (void)TCPIP_HEAP_Free(dhcpv6MemH, dhcpv6ClientDcpt);
            return false;
        }
            
        pClient = dhcpv6ClientDcpt;
        for(netIx = 0; netIx < stackCtrl->nIfs; netIx++)
        {
            pClient->nIanaDcpts = pDhcpConfig->nIanaDcpts;
            pClient->nIataDcpts = pDhcpConfig->nIataDcpts;
            pClient->nFreeDcpts = pDhcpConfig->nFreeDcpts;
            pClient->nIaDcpts = pClient->nIanaDcpts + pClient->nIataDcpts + pClient->nFreeDcpts;
            if((pClient->defaultIanaT1 = pDhcpConfig->defaultIanaT1) == 0U)
            {
                pClient->defaultIanaT1 = TCPIP_DHCPV6_TIMEOUT_INFINITE;
            }
            if((pClient->defaultIanaT2 = pDhcpConfig->defaultIanaT2) == 0U)
            {
                pClient->defaultIanaT2 = TCPIP_DHCPV6_TIMEOUT_INFINITE;
            }
            if((pClient->defaultIataT1 = pDhcpConfig->defaultIataT1) == 0U)
            {
                pClient->defaultIataT1 = TCPIP_DHCPV6_TIMEOUT_INFINITE;
            }
            if((pClient->defaultIataT2 = pDhcpConfig->defaultIataT2) == 0U)
            {
                pClient->defaultIataT2 = TCPIP_DHCPV6_TIMEOUT_INFINITE;
            }
            pClient->ianaSolicitT1 = pDhcpConfig->ianaSolicitT1;
            pClient->ianaSolicitT2 = pDhcpConfig->ianaSolicitT2; 
            pClient->solicitPrefLTime = pDhcpConfig->solicitPrefLTime;  
            pClient->solicitValidLTime = pDhcpConfig->solicitValidLTime; 

            pClient->nMsgBuffers = (uint16_t)pDhcpConfig->nMsgBuffers;
            // make sure there's enough buffers to send and receive all IAs
            uint16_t minBuffers =  (pClient->nIanaDcpts +  pClient->nIataDcpts) * 2U;
            if(pClient->nMsgBuffers < minBuffers)
            {
                pClient->nMsgBuffers = minBuffers;
            }

            pClient->msgBufferSize = (uint16_t)pDhcpConfig->msgBufferSize;
            if(pClient->msgBufferSize < (uint16_t)TCPIP_DHCPV6_MESSAGE_MIN_BUFFER_SIZE)
            {
                pClient->msgBufferSize = (uint16_t)TCPIP_DHCPV6_MESSAGE_MIN_BUFFER_SIZE;
            }

            // allocate the IA
            pClient->iaArray = (TCPIP_DHCPV6_IA_DCPT*)TCPIP_HEAP_Calloc(dhcpv6MemH, pClient->nIaDcpts, sizeof(TCPIP_DHCPV6_IA_DCPT));
            // allocate the message buffers
            pClient->msgBuffers = (TCPIP_DHCPV6_MSG_BUFFER*)TCPIP_HEAP_Calloc(dhcpv6MemH, pClient->nMsgBuffers, sizeof(TCPIP_DHCPV6_MSG_BUFFER_BARE) + pClient->msgBufferSize);
#if (M_TCPIP_DHCPV6_USER_NOTIFICATION != 0)
            // initialize the registration
            bool iniRes = TCPIP_Notification_Initialize(&dhcpv6RegisteredUsers);
            if(pClient->iaArray == NULL || pClient->msgBuffers == NULL || iniRes == false)
#else
            if(pClient->iaArray == NULL || pClient->msgBuffers == NULL)
#endif  // (M_TCPIP_DHCPV6_USER_NOTIFICATION != 0)
            {
                F_DHCPV6Cleanup(stackCtrl->nIfs);
                return false;
            }

            pClient->prevState = (uint16_t)-1;
            pClient->dhcpSkt = INVALID_UDP_SOCKET;

            pClient->cliStatusMsgSize = M_DHCPV6_CLI_STATUS_USR_MSG_LEN;

            pClient++;
        }
    }
            
    dhcpv6ClientPort = pDhcpConfig->dhcpCliPort;
    dhcpv6ServerPort = pDhcpConfig->dhcpSrvPort;

    pClient = dhcpv6ClientDcpt + stackCtrl->netIx;
    pClient->clientDuid.duidType = pDhcpConfig->duidType;
    pClient->pDhcpIf = stackCtrl->pNetIf;
    pClient->configFlags = pDhcpConfig->configFlags;

    // initialize the service
    (void)F_DHCPV6Client_SetStateFlags(pClient, TCPIP_DHCPV6_CLIENT_STATE_INIT, TCPIP_DHCPV6_RUN_FLAG_NONE);

    dhcpv6InitCount++;


    return true;
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_DHCPV6_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN) // interface down

    if(dhcpv6InitCount > 0)
    {   // we're up and running
        // one way or another this interface is going down
        TCPIP_DHCPV6_CLIENT_DCPT* pClient = dhcpv6ClientDcpt + TCPIPStackNetIxGet(stackCtrl->pNetIf);
        if(pClient->state != (uint16_t)TCPIP_DHCPV6_CLIENT_STATE_IDLE)
        {   // we're doing something
            (void)F_DHCPV6Client_SetStateFlags(pClient, TCPIP_DHCPV6_CLIENT_STATE_IDLE, TCPIP_DHCPV6_RUN_FLAG_BUSY | TCPIP_DHCPV6_RUN_FLAG_RX_DISABLED);
            F_DHCPV6Close(pClient, TCPIP_DHCPV6_CLOSE_FLAG_CLOSE_ALL);
            // remember we were up when the interface went down
            (void)F_DHCPV6Client_SetStateFlags(pClient, TCPIP_DHCPV6_CLIENT_STATE_IDLE, TCPIP_DHCPV6_RUN_FLAG_WAS_ENABLED);
        }

        //  the registered users for this interface are not removed
        //  since this interface is closed there won't be any event generated on it anyway
        //  deallocation will wait for the whole stack to deinit 

        if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--dhcpv6InitCount == 0)
            {   // all closed
                // release resources
                F_DHCPV6Cleanup(stackCtrl->nIfs);
                dhcpv6MemH = NULL;
            }
        }
    }

}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)


void TCPIP_DHCPV6_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;
    bool                isTmo = false;

    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(((uint16_t)sigPend & (uint16_t)TCPIP_MODULE_SIGNAL_TMO) != 0U)
    { // regular TMO occurred
        isTmo = true;
    }

    if(isTmo || ((uint16_t)sigPend & (uint16_t)TCPIP_MODULE_SIGNAL_RX_PENDING) != 0U)
    {   // execute either for TMO or RX signal
        TCPIP_DHCPV6_Process(isTmo);
    }
}

// send a signal to the DHCP module that data is available
// no manager alert needed since this normally results as a higher layer (UDP) signal
static void F_DHCPV6SocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_UDP_SIGNAL_RX_DATA)
    {
        (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}


static void TCPIP_DHCPV6_Process(bool isTmo)
{
    size_t              netIx, nNets;
    const TCPIP_NET_IF* pNetIf;
    TCPIP_DHCPV6_CLIENT_DCPT* pClient;
    
    nNets = TCPIP_STACK_NumberOfNetworksGet();
    for(netIx = 0; netIx < nNets; netIx++) 
    {
        pNetIf = (const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet (netIx);
        if(!TCPIP_STACK_NetworkIsUp(pNetIf))
        {   // inactive interface
            continue;
        }

        pClient = dhcpv6ClientDcpt + netIx; 

        F_DHCPV6DbgStatePrint_Client(pClient, false);
        (*T_DHCPV6Client_StateFncTbl[pClient->state])(pClient);
        F_DHCPV6DbgStatePrint_Client(pClient, false);
    }
}


// state processing functions
//
// TCPIP_DHCPV6_CLIENT_STATE_INIT
static void   F_DHCPV6Client_StateProcInit(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    if(TCPIP_IPV6_InterfaceIsReady(pClient->pDhcpIf))
    {
        if(F_DHCPV6Client_Init(pClient))
        {   // advance
            TCPIP_DHCPV6_CLIENT_STATE newState;
            uint16_t newFlags;
            
            // use the start flag to know where to advance
            if(((uint16_t)pClient->configFlags & (uint16_t)TCPIP_DHCPV6_FLAG_START_ENABLE) != 0U)
            {
                newState = TCPIP_DHCPV6_CLIENT_STATE_START;
                newFlags = TCPIP_DHCPV6_RUN_FLAG_WAS_ENABLED;
            }
            else
            {   // wait
                newState = TCPIP_DHCPV6_CLIENT_STATE_IDLE;
                newFlags = TCPIP_DHCPV6_RUN_FLAG_NONE;
            }
            (void)F_DHCPV6Client_SetStateFlags(pClient, newState, newFlags);
            F_DHCPV6Client_SetStateNotify(pClient, newState);
        }
    }
    // else wait some more
}

// TCPIP_DHCPV6_CLIENT_STATE_IDLE
static void   F_DHCPV6Client_StateProcIdle(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    // inactive
    F_DHCPv6FlushSocket(pClient);
}

// TCPIP_DHCPV6_CLIENT_STATE_REINIT
static void   F_DHCPV6Client_StateProcReinit(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    if(F_DHCPV6Client_Reinit(pClient))
    {   // advance
        F_DHCPV6Client_SetStateNotify(pClient, TCPIP_DHCPV6_CLIENT_STATE_START);
    }
    // else wait some more
}

// TCPIP_DHCPV6_CLIENT_STATE_START
static void   F_DHCPV6Client_StateProcStart(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    // wait in this state until the interface is ready
    if(TCPIP_IPV6_InterfaceIsReady(pClient->pDhcpIf))
    {
        F_DHCPV6Client_SetStateNotify(pClient, TCPIP_DHCPV6_CLIENT_STATE_WAIT_LINK);
    }
}

// iaState processing functions
// TCPIP_DHCPV6_CLIENT_STATE_RUN
static void   F_DHCPV6Client_StateProcRun(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    size_t  taskIx;

    // execute the client tasks
    for(taskIx = 0; taskIx < sizeof(T_DHCPV6Client_TaskTbl)/sizeof(*T_DHCPV6Client_TaskTbl) && pClient->state == (uint16_t)TCPIP_DHCPV6_CLIENT_STATE_RUN; taskIx++)
    {
        (*T_DHCPV6Client_TaskTbl[taskIx])(pClient);
    }

    // check the link
    if(F_DHCPV6Client_CheckLink(pClient))
    {   // execute the IA tasks
        F_DHCPV6Ia_TaskExecute(pClient);
    }
}

// TCPIP_DHCPV6_CLIENT_STATE_WAIT_LINK
static void F_DHCPV6Client_StateProcWaitLink(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    // wait for the link to go up
    (void)F_DHCPV6Client_CheckLink(pClient);
}

// checks the link status
// returns true if execution needs to continue (link is up)
// or false if the run state machine should not be executed (link is down)
static bool F_DHCPV6Client_CheckLink(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{

    TCPIP_DHCPV6_CLIENT_STATE newState = TCPIP_DHCPV6_CLIENT_STATE_INIT;
    bool linkUp = TCPIP_STACK_NetworkIsLinked(pClient->pDhcpIf);

    if(linkUp)
    {
        if(pClient->state == (uint16_t)TCPIP_DHCPV6_CLIENT_STATE_WAIT_LINK)
        {
            F_DHCPV6Client_LinkConfirm(pClient);
            newState = TCPIP_DHCPV6_CLIENT_STATE_RUN; 
#if ((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_LINK_STAT) != 0)
            SYS_CONSOLE_PRINT("DHCPV6 Link Up - time: %zu\r\n", (size_t)TCPIP_MsecCountGet());
#endif  // ((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_LINK_STAT) != 0)
        } 
    }
    else
    {
        if(pClient->state != (uint16_t)TCPIP_DHCPV6_CLIENT_STATE_WAIT_LINK)
        {
            newState = TCPIP_DHCPV6_CLIENT_STATE_WAIT_LINK; 
#if ((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_LINK_STAT) != 0)
            SYS_CONSOLE_PRINT("DHCPV6 Link Down - time: %zu\r\n", (size_t)TCPIP_MsecCountGet());
#endif  // ((TCPIP_DHCPV6_DEBUG_LEVEL & DHCPV6_DEBUG_MASK_LINK_STAT) != 0)
        }
    }

    if(newState != TCPIP_DHCPV6_CLIENT_STATE_INIT)
    {
        F_DHCPV6Client_SetStateNotify(pClient, newState);
    }

    return linkUp;
}

// connection established event occurred and the link is up
// All IAs in bound state move to CONFIRM
static void F_DHCPV6Client_LinkConfirm(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    TCPIP_DHCPV6_IA_DCPT* pIa, *pIaNext;
    DOUBLE_LIST* pBoundList = pClient->iaStateList + (int)TCPIP_DHCPV6_IA_STATE_BOUND;

    pIa = FC_DblNode2IaDcpt(pBoundList->head);
    while(pIa != NULL)
    {
        pIaNext = pIa->next;
        F_DHCPV6Ia_SetRunState(pIa, TCPIP_DHCPV6_IA_STATE_CONFIRM, TCPIP_DHCPV6_IA_SUBSTATE_START);
        pIa = pIaNext;
    } 

}

// generic
static void F_DHCPV6Ia_TaskExecute(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    size_t listIx;
    TCPIP_DHCPV6_IA_DCPT* pIa, *pIaNext;


    for(listIx = 0; listIx < sizeof(pClient->iaStateList) / sizeof(*pClient->iaStateList); listIx++)
    {
        pIa = FC_DblNode2IaDcpt(pClient->iaStateList[listIx].head);
        while(pIa != NULL)
        {
            pIaNext = pIa->next;
            // sanity check
            if(pIa->iaState != listIx)
            {
                TCPIPStack_Condition(false, __FILE__, __func__, __LINE__);
                F_DHCPV6Ia_SetRunState(pIa, TCPIP_DHCPV6_IA_STATE_ERROR_FATAL, TCPIP_DHCPV6_IA_SUBSTATE_START);
                pIa = pIaNext;
                continue;
            }
            // dispatch based on the iaSubState 
            const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC* pSubProcFnc = T_DHCPV6Ia_StateProcTbl[pIa->iaState];

            // dispatch according to run substate! 
            TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes;

            subRes = (*pSubProcFnc[pIa->iaSubState])(pIa);
            switch(subRes)
            {
                case DHCPV6_IA_SSTATE_RES_NO_ACTION:
                    break;

                case DHCPV6_IA_SSTATE_RES_OK:
                    // ok, can advance substate
                    F_DHCPV6Ia_SetRunState(pIa, (TCPIP_DHCPV6_IA_STATE)pIa->iaState, FC_U162IaSubst(pIa->iaSubState + 1U));
                    break;

                case DHCPV6_IA_SSTATE_RES_PENDING:
                    // wait some more
                    break;

                case DHCPV6_IA_SSTATE_RES_RETRANSMIT:
                    // need to retransmit
                    F_DHCPV6Ia_SetRunState(pIa, (TCPIP_DHCPV6_IA_STATE)pIa->iaState, TCPIP_DHCPV6_IA_SUBSTATE_TRANSMIT);
                    break;

                case DHCPV6_IA_SSTATE_RES_RUN_NEXT:
                    // need to advance
                    F_DHCPV6Ia_SetRunState(pIa, FC_U162IaState(pIa->iaState + 1U), TCPIP_DHCPV6_IA_SUBSTATE_START);
                    break;

                case DHCPV6_IA_SSTATE_RES_RUN_JUMP:
                    // jump to a new state handled at the substate level
                    break;

                case DHCPV6_IA_SSTATE_RES_RUN_RESTART:
                    // restart discovery procedure
                    F_DHCPV6Ia_SetRunState(pIa, TCPIP_DHCPV6_IA_STATE_SOLICIT, TCPIP_DHCPV6_IA_SUBSTATE_START);
                    break;

                case DHCPV6_IA_SSTATE_RES_ERR_TRANS:
                    // transient error; abort
                    F_DHCPV6Ia_SetRunState(pIa, TCPIP_DHCPV6_IA_STATE_ERROR_TRANSIENT, TCPIP_DHCPV6_IA_SUBSTATE_START);
                    break;

                default:    // DHCPV6_IA_SSTATE_RES_ERROR_FATAL:
                    // fatal error; abort
                    F_DHCPV6Ia_SetRunState(pIa, TCPIP_DHCPV6_IA_STATE_ERROR_FATAL, TCPIP_DHCPV6_IA_SUBSTATE_START);
                    break;
            }
            pIa = pIaNext;
        } 
    }

}

// generic substate functions
//
// TCPIP_DHCPV6_IA_SUBSTATE_IDELAY
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT F_DHCPV6Ia_SubStateIDelay(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_MSG_TX_RESULT txRes;

    txRes = F_DHCPV6Ia_CheckMsgTransmitStatus(pIa);

    if((int)txRes < 0)
    {   // some error ocurred
        return DHCPV6_IA_SSTATE_RES_ERR_TRANS;
    }

    if(txRes != TCPIP_DHCPV6_MSG_TX_RES_PENDING)
    {   // can move on
        return DHCPV6_IA_SSTATE_RES_OK; 
    }

    return DHCPV6_IA_SSTATE_RES_PENDING; 
}

// TCPIP_DHCPV6_IA_SUBSTATE_TRANSMIT
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT F_DHCPV6Ia_SubStateTransmit(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    // write the message into the buffer
    // set the wrDcpt

    TCPIP_DHCPV6_MSG_BUFFER* pTxBuffer = pIa->msgBuffer;
    if(pTxBuffer == NULL)
    {   // shouldn't happen
        TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
        return DHCPV6_IA_SSTATE_RES_ERROR_FATAL;
    }

    TCPIP_DHCPV6_MSG_WRITE_DCPT* pWrDcpt = &pIa->wrDcpt;

    pWrDcpt->pWrite = pTxBuffer->pMsgData;
    pWrDcpt->writeSpace = (int16_t)pTxBuffer->bufferSize;
    pWrDcpt->writeBytes = 0;
    
    
    int msgSize = F_DHCPV6Ia_MessageWrite(pIa);
    if(msgSize == -1)
    {   // shouldn't happen
        TCPIPStack_Condition(false, __FILE__, __func__, __LINE__);
        return DHCPV6_IA_SSTATE_RES_ERROR_FATAL;
    }

    // set the message fields
    pTxBuffer->msgLen = (uint16_t)pIa->wrDcpt.writeBytes;

    // insert it into the txMsgList
    TCPIP_Helper_SingleListTailAdd(&pIa->pParent->txMsgList, FC_MsgBuff2SglNode(pTxBuffer));

    // we keep the pTxBuffer for this IA in case there are retries

    F_DHCPV6TraceBuff(pIa, pTxBuffer);

    return DHCPV6_IA_SSTATE_RES_OK;
}


// substate functions
// TCPIP_DHCPV6_IA_STATE_SOLICIT
//

// solicit substate functions
// (TCPIP_DHCPV6_IA_STATE_SOLICIT, TCPIP_DHCPV6_IA_SUBSTATE_START)
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT F_DHCPV6Ia_SubStateSolicitStart(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes = F_DHCPV6Ia_TxMsgSetup(pIa, DHCPV6_CLIENT_MSG_TYPE_SOLICIT);

    if(subRes == DHCPV6_IA_SSTATE_RES_OK)
    {
        F_DHCPV6Ia_MsgListPurge(&pIa->pParent->advertiseList, pIa);
        F_DHCPV6Ia_SolicitInit(pIa);
    }

    return subRes; 
}



static void F_DHCPV6Ia_SolicitInit(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    bool addrValid;
    TCPIP_DHCPV6_CLIENT_DCPT* pParent = pIa->pParent;

    if(pIa->iaBody.type == TCPIP_DHCPV6_IA_TYPE_IANA)
    {
        pIa->iaBody.ianaBody.t1 = pParent->ianaSolicitT1;
        pIa->iaBody.ianaBody.t2 = pParent->ianaSolicitT2;
    } 

    pIa->flags.val = 0U;
    TCPIP_DHCPV6_ADDR_NODE* addNode = FC_SglNode2AddNode(pIa->addList.head);
    TCPIPStack_Assert(addNode->inUse == false, __FILE__, __func__, __LINE__);
    addrValid = TCPIP_Helper_StringToIPv6Address(TCPIP_DHCPV6_IANA_SOLICIT_DEFAULT_ADDRESS,  &addNode->addBody.ipv6Addr);

    const IPV6_ADDR* pUnspec = &IPV6_FIXED_ADDR_UNSPECIFIED;
    if(addrValid && memcmp(addNode->addBody.ipv6Addr.v, pUnspec->v, sizeof(IPV6_ADDR)) != 0)
    {   // generate an address option for non zero address
        pIa->flags.addInTx = 1U;
        addNode->addBody.prefLTime = pParent->solicitPrefLTime;
        addNode->addBody.validLTime = pParent->solicitValidLTime;
        addNode->inUse = true;
    }
}



// (TCPIP_DHCPV6_IA_STATE_SOLICIT, TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY)
// waiting for advertisements
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT F_DHCPV6Ia_SubStateSolicitationWaitReply(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_MSG_TX_RESULT txRes;
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes;
    bool isTmo;


    txRes = F_DHCPV6Ia_CheckMsgTransmitStatus(pIa);

    isTmo = ((int)txRes < 0) || txRes == TCPIP_DHCPV6_MSG_TX_RES_OK;

    while(true)
    {
        subRes = F_DHCPV6Ia_AdvertiseSelect(pIa, isTmo);

        if(subRes == DHCPV6_IA_SSTATE_RES_OK)
        {   // found server for this IA;
            // wait condition granted; move on;
            subRes = DHCPV6_IA_SSTATE_RES_RUN_NEXT;
            break;
        }
        else if((int)subRes < 0)
        {   // an error occurred
            break;
        }
        else
        {
            // do nothing
        }

        // check if re-transmission needed
        if((int)txRes < 0)
        {   // exhausted the retries
            subRes = DHCPV6_IA_SSTATE_RES_ERR_TRANS;
        }
        else if(txRes == TCPIP_DHCPV6_MSG_TX_RES_PENDING)
        {   // keep waiting
            subRes =  DHCPV6_IA_SSTATE_RES_PENDING;
        }
        else
        {   // retransmit the Solicit message
            subRes = DHCPV6_IA_SSTATE_RES_RETRANSMIT;
        }

        break;
    }

    F_DHCPV6DbgStatePrint_IAAdd("IA solicit res: %d\r\n", subRes);

     return subRes;
}


// substate functions
// TCPIP_DHCPV6_IA_STATE_REQUEST
//

// (TCPIP_DHCPV6_IA_STATE_REQUEST, TCPIP_DHCPV6_IA_SUBSTATE_START)
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT F_DHCPV6Ia_SubStateRequestStart(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes = F_DHCPV6Ia_TxMsgSetup(pIa, DHCPV6_CLIENT_MSG_TYPE_REQUEST);
    if(subRes == DHCPV6_IA_SSTATE_RES_OK)
    {
        F_DHCPV6Ia_MsgListPurge(&pIa->pParent->replyList, pIa);
    }
    

    return subRes; 

}

// TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY:
//      TCPIP_DHCPV6_IA_STATE_REQUEST/TCPIP_DHCPV6_IA_STATE_RENEW/TCPIP_DHCPV6_IA_STATE_DECLINE/
//      /TCPIP_DHCPV6_IA_STATE_REBIND/TCPIP_DHCPV6_IA_STATE_CONFIRM/TCPIP_DHCPV6_IA_STATE_RELEASE
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT F_DHCPV6Ia_SubStateWaitReplyMsg(TCPIP_DHCPV6_IA_DCPT* pIa)
{

    TCPIP_DHCPV6_MSG_TX_RESULT txRes;
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes;

    TCPIPStack_Assert((pIa->iaState == (uint16_t)TCPIP_DHCPV6_IA_STATE_REQUEST ||
                    pIa->iaState == (uint16_t)TCPIP_DHCPV6_IA_STATE_RENEW ||
                    pIa->iaState == (uint16_t)TCPIP_DHCPV6_IA_STATE_DECLINE ||
                    pIa->iaState == (uint16_t)TCPIP_DHCPV6_IA_STATE_REBIND ||
                    pIa->iaState == (uint16_t)TCPIP_DHCPV6_IA_STATE_CONFIRM ||
                    pIa->iaState == (uint16_t)TCPIP_DHCPV6_IA_STATE_RELEASE) , __FILE__, __func__, __LINE__);

    bool serverMatch = (pIa->iaState == (uint16_t)TCPIP_DHCPV6_IA_STATE_REQUEST ||
            pIa->iaState == (uint16_t)TCPIP_DHCPV6_IA_STATE_RENEW ||
            pIa->iaState == (uint16_t)TCPIP_DHCPV6_IA_STATE_DECLINE ||
            pIa->iaState == (uint16_t)TCPIP_DHCPV6_IA_STATE_RELEASE); 

    if(serverMatch)
    {
        subRes = F_DHCPV6Ia_ReplyMsgSrvMatchProcess(pIa);
    }
    else if(pIa->iaState == (uint16_t)TCPIP_DHCPV6_IA_STATE_CONFIRM)
    {
        subRes = F_DHCPV6Ia_ReplyConfirmProcess(pIa);
    }
    else
    {
        subRes = F_DHCPV6Ia_ReplyRebindProcess(pIa);
    }


    if(subRes == DHCPV6_IA_SSTATE_RES_PENDING)
    {   // couldn't get any reply
        // check if re-transmission needed
        txRes = F_DHCPV6Ia_CheckMsgTransmitStatus(pIa);
        if((int)txRes < 0)
        {   // exhausted the retries
            if(pIa->iaState == (uint16_t)TCPIP_DHCPV6_IA_STATE_REQUEST)
            {   // user intervention needed
                subRes = DHCPV6_IA_SSTATE_RES_ERR_TRANS;
            }
            else if(pIa->iaState == (uint16_t)TCPIP_DHCPV6_IA_STATE_RENEW)
            {   // go to REBIND
                subRes = DHCPV6_IA_SSTATE_RES_RUN_NEXT;
            }
            else if(pIa->iaState == (uint16_t)TCPIP_DHCPV6_IA_STATE_REBIND)
            {   // go to SOLICIT
                F_DHCPV6Ia_AddressRemove(pIa);
                subRes = DHCPV6_IA_SSTATE_RES_RUN_RESTART;
            }
            else if(pIa->iaState == (uint16_t)TCPIP_DHCPV6_IA_STATE_CONFIRM)
            {   // lack of a reply means we could use the IA as it is
                F_DHCPV6Ia_RestoreTimeValues(pIa);
                F_DHCPV6Ia_SetRunState(pIa, TCPIP_DHCPV6_IA_STATE_BOUND, TCPIP_DHCPV6_IA_SUBSTATE_START);
                subRes = DHCPV6_IA_SSTATE_RES_RUN_JUMP;
            }
            else if(pIa->iaState == (uint16_t)TCPIP_DHCPV6_IA_STATE_RELEASE || pIa->iaState == (uint16_t)TCPIP_DHCPV6_IA_STATE_DECLINE)
            {   // lack of a reply: we give up
                F_DHCPV6Ia_Remove(pIa);
                subRes = DHCPV6_IA_SSTATE_RES_NO_ACTION;
            }
            else
            {   // should not happen
                TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
            }
        }
        else if(txRes == TCPIP_DHCPV6_MSG_TX_RES_PENDING)
        {   // keep waiting
            subRes =  DHCPV6_IA_SSTATE_RES_PENDING;
        }
        else
        {   // retransmit the message
            subRes = DHCPV6_IA_SSTATE_RES_RETRANSMIT;
        }
    }

    F_DHCPV6DbgStatePrint_IAAdd("IA - state: %d, substate: %d, wait res: %d\r\n", pIa->iaState, pIa->iaSubState, subRes);
    return subRes; 
}

// substate functions
// TCPIP_DHCPV6_IA_STATE_DAD
//

// (TCPIP_DHCPV6_IA_STATE_DAD , TCPIP_DHCPV6_IA_SUBSTATE_START)
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT F_DHCPV6Ia_SubStateDadStart(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    IPV6_ADDR_STRUCT* pAddS;
    IPV6_ADDR_STRUCT* addrPtr;
    int nSchedDad = 0;  // # of addresses for DAD
    int nPendDad =  0;  // # of addresses still pending DAD
    int nOkDad = 0;     // # of addresses that don't need DAD

    uint8_t skipDad = ((uint8_t)pIa->pParent->configFlags & (uint8_t)TCPIP_DHCPV6_FLAG_DAD_DISABLE);

    TCPIP_DHCPV6_ADDR_NODE* addNode;
    for(addNode = FC_SglNode2AddNode(pIa->addList.head); addNode != NULL; addNode = addNode->next)
    {
        if(!addNode->inUse)
        {
            continue;
        }

        IPV6_ADDR* pAdd6 =  &addNode->addBody.ipv6Addr;
        pAddS = TCPIP_IPV6_AddressFind(pIa->pParent->pDhcpIf, pAdd6, IPV6_ADDR_TYPE_UNICAST);
        if(pAddS != NULL)
        {   // good address
            nOkDad++;
        }
        else
        {
            addrPtr = TCPIP_IPV6_UnicastAddressAdd (pIa->pParent->pDhcpIf, pAdd6, 0, skipDad);
            if(addrPtr == NULL)
            {   // failed inserting new address; try again next time
                TCPIPStack_Condition(false, __FILE__, __func__, __LINE__);
                nPendDad++;
            }
            else
            {
                nSchedDad++;
            }
        }
    }


    if(nPendDad != 0)
    {
        return DHCPV6_IA_SSTATE_RES_PENDING; 
    }
    else if(nSchedDad != 0)
    {
        return DHCPV6_IA_SSTATE_RES_OK;
    }
    else if(nOkDad != 0)
    {
        F_DHCPV6Ia_SetRunState(pIa, TCPIP_DHCPV6_IA_STATE_BOUND, TCPIP_DHCPV6_IA_SUBSTATE_START);
        return DHCPV6_IA_SSTATE_RES_RUN_JUMP;
    }
    else
    {
        // do nothing
    }


    TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
    return DHCPV6_IA_SSTATE_RES_ERROR_FATAL;
    
}


// (TCPIP_DHCPV6_IA_STATE_DAD , TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY)
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT F_DHCPV6Ia_SubStateDadWait(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    IPV6_ADDR_STRUCT* pAddS;
    TCPIP_DHCPV6_CLIENT_DCPT* pParent = pIa->pParent;
    int nPendDad =  0;  // # of addresses still pending DAD
    int nOkDad = 0;     // # of addresses that passed DAD
    int nFailDad = 0;  // # of addresses that failed DAD

    TCPIP_DHCPV6_ADDR_NODE* addNode;
    for(addNode = FC_SglNode2AddNode(pIa->addList.head); addNode != NULL; addNode = addNode->next)
    {
        if(!addNode->inUse)
        {
            continue;
        }

        IPV6_ADDR* pAdd6 =  &addNode->addBody.ipv6Addr;
        // search if it's still ongoing
        pAddS = TCPIP_IPV6_AddressFind(pParent->pDhcpIf, pAdd6, IPV6_ADDR_TYPE_UNICAST_TENTATIVE);
        if(pAddS != NULL)
        {   // in process
            nPendDad++;
        }
        else
        {   // not in tentative; check if already promoted
            pAddS = TCPIP_IPV6_AddressFind(pParent->pDhcpIf, pAdd6, IPV6_ADDR_TYPE_UNICAST);
            if(pAddS != NULL)
            {   // good address
                nOkDad++;
            }
            else
            {   // this address has failed; probably duplicate
                nFailDad++;
                break;
            }
        }
    }



    if(nFailDad != 0)
    {   // move to next state which is decline!
        F_DHCPV6Ia_SetRunState(pIa, TCPIP_DHCPV6_IA_STATE_DECLINE, TCPIP_DHCPV6_IA_SUBSTATE_START);
        return DHCPV6_IA_SSTATE_RES_RUN_JUMP;
    }

    if(nPendDad != 0)
    {   // still in process
        return DHCPV6_IA_SSTATE_RES_PENDING;
    }

    if(nOkDad != 0)
    {   // all good addresses
        F_DHCPV6Ia_SetRunState(pIa, TCPIP_DHCPV6_IA_STATE_BOUND, TCPIP_DHCPV6_IA_SUBSTATE_START);
        return DHCPV6_IA_SSTATE_RES_RUN_JUMP;
    }

    TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
    return DHCPV6_IA_SSTATE_RES_ERROR_FATAL;

}

// substate functions
// TCPIP_DHCPV6_IA_STATE_DECLINE
//

// (TCPIP_DHCPV6_IA_STATE_DECLINE , TCPIP_DHCPV6_IA_SUBSTATE_START)
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT F_DHCPV6Ia_SubStateDeclineStart(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    F_DHCPV6Ia_MsgListPurge(&pIa->pParent->replyList, pIa);  
    
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes = F_DHCPV6Ia_TxMsgSetup(pIa, DHCPV6_CLIENT_MSG_TYPE_DECLINE);

    if(subRes == DHCPV6_IA_SSTATE_RES_OK)
    {
        // mark address ready for TX
        pIa->flags.addInTx = 1U;
    }

    return subRes; 
}

// TCPIP_DHCPV6_IA_STATE_BOUND
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT F_DHCPV6Ia_SubStateBoundWait(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    uint32_t    secCurr = TCPIP_SecCountGet();
    uint32_t    iaDeadLine;

    if(pIa->iaBody.type == TCPIP_DHCPV6_IA_TYPE_IANA)
    {   // check the T1 expiration
        iaDeadLine = pIa->iaBody.ianaBody.t1;
    }
    else
    {   // IATA use global values
        iaDeadLine = pIa->pParent->defaultIataT1;
    }

    TCPIP_DHCPV6_ADDR_NODE* addNode;
    for(addNode = FC_SglNode2AddNode(pIa->addList.head); addNode != NULL; addNode = addNode->next)
    {
        if(!addNode->inUse)
        {
            continue;
        }


        if(((uint16_t)pIa->pParent->configFlags & (uint16_t)TCPIP_DHCPV6_FLAG_IA_IGNORE_RENEW_LTIME) == 0U)
        {   // select minimum
            iaDeadLine = (iaDeadLine > addNode->addBody.prefLTime) ? addNode->addBody.prefLTime : iaDeadLine;
        }

        if(iaDeadLine != TCPIP_DHCPV6_TIMEOUT_INFINITE)
        {
            if((secCurr - pIa->iaBody.tAcquire) >= iaDeadLine)
            {   // timeout;
                return DHCPV6_IA_SSTATE_RES_RUN_NEXT; 
            }
        }
    }

    return DHCPV6_IA_SSTATE_RES_PENDING;
}

// TCPIP_DHCPV6_IA_STATE_RENEW, TCPIP_DHCPV6_IA_SUBSTATE_START
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT F_DHCPV6Ia_SubStateRenewStart(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes = F_DHCPV6Ia_TxMsgSetup(pIa, DHCPV6_CLIENT_MSG_TYPE_RENEW);
    if(subRes == DHCPV6_IA_SSTATE_RES_OK)
    {
        F_DHCPV6Ia_MsgListPurge(&pIa->pParent->replyList, pIa);
        // mark address ready for TX
        pIa->flags.addInTx = 1U;
    }
    
    return subRes; 
}

// TCPIP_DHCPV6_IA_STATE_REBIND, TCPIP_DHCPV6_IA_SUBSTATE_START
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT F_DHCPV6Ia_SubStateRebindStart(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes = F_DHCPV6Ia_TxMsgSetup(pIa, DHCPV6_CLIENT_MSG_TYPE_REBIND);
    if(subRes == DHCPV6_IA_SSTATE_RES_OK)
    {
        F_DHCPV6Ia_MsgListPurge(&pIa->pParent->replyList, pIa);
        // mark address ready for TX
        pIa->flags.addInTx = 1U;
        // make sure we send the rebind to all servers!
        pIa->flags.iaUnicast = 0U;
    }
    
    return subRes; 
}

// TCPIP_DHCPV6_IA_STATE_CONFIRM, TCPIP_DHCPV6_IA_SUBSTATE_START
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT F_DHCPV6Ia_SubStateConfirmStart(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes;

    // check the IA has not expired while w/o a valid link
    if(F_DHCPV6Ia_AddressIsExpired(pIa, false))
    {
        subRes = DHCPV6_IA_SSTATE_RES_RUN_RESTART;
    }
    else
    {
        subRes = F_DHCPV6Ia_TxMsgSetup(pIa, DHCPV6_CLIENT_MSG_TYPE_CONFIRM);
        if(subRes == DHCPV6_IA_SSTATE_RES_OK)
        {
            F_DHCPV6Ia_MsgListPurge(&pIa->pParent->replyList, pIa);
            // mark address ready for TX
            pIa->flags.addInTx = 1U;
            // make sure we send the confirm to all servers!
            pIa->flags.iaUnicast = 0U;
            // set the T1, T2, lifetimes to 0
            pIa->iaBody.ianaBody.t1 = pIa->iaBody.ianaBody.t2 = 0;

            TCPIP_DHCPV6_ADDR_NODE* addNode;
            for(addNode = FC_SglNode2AddNode(pIa->addList.head); addNode != NULL; addNode = addNode->next)
            {
                addNode->addBody.prefLTime = addNode->addBody.validLTime = 0;
            }
        }
    }

    return subRes; 
}

// TCPIP_DHCPV6_IA_STATE_RELEASE, TCPIP_DHCPV6_IA_SUBSTATE_START
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT F_DHCPV6Ia_SubStateReleaseStart(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes;

    subRes = F_DHCPV6Ia_TxMsgSetup(pIa, DHCPV6_CLIENT_MSG_TYPE_RELEASE);
    if(subRes == DHCPV6_IA_SSTATE_RES_OK)
    {
        F_DHCPV6Ia_AddressRemove(pIa);
        F_DHCPV6Ia_MsgListPurge(&pIa->pParent->replyList, pIa);
        // mark address ready for TX
        pIa->flags.addInTx = 1U;
    }

    return subRes; 
}

static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   F_DHCPV6Ia_SubStateErrorTransient(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    // remove the address in error
    F_DHCPV6Ia_AddressRemove(pIa);
    // start all over
    return DHCPV6_IA_SSTATE_RES_RUN_RESTART;
}


// normally we should NOT get here!
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   F_DHCPV6Ia_SubStateErrorFatal(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
    // just in case
    F_DHCPV6Ia_AddressRemove(pIa);
    // start all over
    return DHCPV6_IA_SSTATE_RES_RUN_RESTART;
}

static void F_DHCPV6Close(TCPIP_DHCPV6_CLIENT_DCPT* pClient, uint16_t cFlags)
{
    if((cFlags & TCPIP_DHCPV6_CLOSE_FLAG_RELEASE) != 0U)
    {
        F_DHCPV6_Release(pClient);
    }

    if((cFlags & TCPIP_DHCPV6_CLOSE_FLAG_CLEANUP) != 0U)
    {
        F_DHCPV6_LeasesCleanup(pClient);
    }
}

static void F_DHCPV6_Release(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    size_t listIx;
    TCPIP_DHCPV6_IA_DCPT* pIaDcpt;

    // purge/abort all messages
    F_DHCPV6_MsgListPurgeAll(pClient);

    // send release message to the server
    // first release all pending messages so we can TX the release messages
    for(listIx = 0; listIx < sizeof(pClient->iaStateList) / sizeof(*pClient->iaStateList); listIx++)
    {
        for(pIaDcpt = FC_DblNode2IaDcpt(pClient->iaStateList[listIx].head); pIaDcpt != NULL; pIaDcpt = pIaDcpt->next)
        {
            TCPIPStack_Assert(pIaDcpt->pParent == pClient, __FILE__, __func__, __LINE__);

            // discard any pending msgBuffer so we can TX the release message
            F_DHCPV6Ia_ReleaseMsgBuffer(pIaDcpt);
        }
    }

    for(listIx = 0; listIx < sizeof(pClient->iaStateList) / sizeof(*pClient->iaStateList); listIx++)
    {
        for(pIaDcpt = FC_DblNode2IaDcpt(pClient->iaStateList[listIx].head); pIaDcpt != NULL; pIaDcpt = pIaDcpt->next)
        {
            if((uint16_t)TCPIP_DHCPV6_IA_STATE_BOUND <= pIaDcpt->iaState && pIaDcpt->iaState < (uint16_t)TCPIP_DHCPV6_IA_STATE_ERROR_TRANSIENT)
            {
                TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes = F_DHCPV6Ia_SubStateReleaseStart(pIaDcpt); // pIaDcpt->msgBuffer = from (pParent->buffFreeList)
                TCPIPStack_Assert(subRes == DHCPV6_IA_SSTATE_RES_OK, __FILE__, __func__, __LINE__);
                if(subRes == DHCPV6_IA_SSTATE_RES_OK)
                {
                    subRes = F_DHCPV6Ia_SubStateTransmit(pIaDcpt);   // pIaDcpt->msgBuffer inserted to -> pIaDcpt->pParent->txMsgList
                    TCPIPStack_Assert(subRes == DHCPV6_IA_SSTATE_RES_OK, __FILE__, __func__, __LINE__);
                    F_DHCPV6Client_TransmitTask(pClient); // remove from pClient->txMsgList but leave it as pIaDcpt->msgBuffer...
                } 
                // remove any address we may have acquired
                F_DHCPV6Ia_AddressRemove(pIaDcpt);
            }
        }
    }
}

// removes all the leases and clean-up
static void F_DHCPV6_LeasesCleanup(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    size_t listIx;
    TCPIP_DHCPV6_IA_DCPT* pIaDcpt;

    // purge all messages
    F_DHCPV6_MsgListPurgeAll(pClient);

    // release all IAs and append them to the free list
    for(listIx = 0; listIx < sizeof(pClient->iaStateList) / sizeof(*pClient->iaStateList); listIx++)
    {
        while((pIaDcpt = FC_DblNode2IaDcpt(pClient->iaStateList[listIx].head)) != NULL)
        {
            TCPIPStack_Assert(pIaDcpt->pParent == pClient, __FILE__, __func__, __LINE__);
            F_DHCPV6Ia_Remove(pIaDcpt);
        } 
    }
}

// releases an IP address
static void F_DHCPV6Ia_AddressRemove(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_NET_IF* pDhcpIf = pIa->pParent->pDhcpIf;

    IPV6_ADDR_STRUCT* pAddS;

    TCPIP_DHCPV6_ADDR_NODE* addNode;
    for(addNode = FC_SglNode2AddNode(pIa->addList.head); addNode != NULL; addNode = addNode->next)
    {
        if(!addNode->inUse)
        {
            continue;
        }

        IPV6_ADDR* pAdd6 =  &addNode->addBody.ipv6Addr;

        pAddS = TCPIP_IPV6_AddressFind(pDhcpIf, pAdd6, IPV6_ADDR_TYPE_UNICAST);
        if(pAddS == NULL)
        {   // try tentative
            pAddS = TCPIP_IPV6_AddressFind(pDhcpIf, pAdd6, IPV6_ADDR_TYPE_UNICAST_TENTATIVE);
        }

        if(pAddS != NULL)
        {
            TCPIP_IPV6_AddressUnicastRemove(pDhcpIf, pAdd6);
            addNode->inUse = false;
        }
    }
}

#if (M_TCPIP_DHCPV6_USER_NOTIFICATION != 0)
TCPIP_DHCPV6_HANDLE TCPIP_DHCPV6_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_DHCPV6_EVENT_HANDLER handler, const void* hParam)
{
    if(handler != NULL && dhcpv6MemH != NULL)
    {
        TCPIP_DHCPV6_LIST_NODE dhcpNode;
        dhcpNode.next = NULL;
        dhcpNode.handler = handler;
        dhcpNode.hParam = hParam;
        dhcpNode.hNet = hNet;

        return FC_SglNode2ListNode(TCPIP_Notification_Add(&dhcpv6RegisteredUsers, dhcpv6MemH, &dhcpNode, sizeof(dhcpNode)));
    }

    return NULL;
}

// deregister the event handler
bool TCPIP_DHCPV6_HandlerDeRegister(TCPIP_DHCPV6_HANDLE hDhcp)
{
    if(hDhcp != NULL && dhcpv6MemH != NULL)
    {
        if(TCPIP_Notification_Remove(FC_DhcpHandle2SglNode(hDhcp), &dhcpv6RegisteredUsers, dhcpv6MemH))
        {
            return true;
        }
    }

    return false;
}
#else
TCPIP_DHCPV6_HANDLE TCPIP_DHCPV6_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_DHCPV6_EVENT_HANDLER handler, const void* hParam)
{
    return NULL;
}
bool TCPIP_DHCPV6_HandlerDeRegister(TCPIP_DHCPV6_HANDLE hDhcp)
{
    return false;
}
#endif  // (M_TCPIP_DHCPV6_USER_NOTIFICATION != 0)

static void F_DHCPV6Client_Notify(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_IA_DCPT* pIa, bool iaSubNotify)
{
    // call with both pClient and pIa == 0 is invalid
    TCPIPStack_Assert(pClient != NULL || pIa != NULL, __FILE__, __func__, __LINE__);

    if(pClient == NULL)
    {
        pClient = pIa->pParent;
    }

#if (M_TCPIP_DHCPV6_USER_NOTIFICATION != 0)
    TCPIP_DHCPV6_IA_EVENT iaEvent;
    const TCPIP_DHCPV6_IA_EVENT* pDhcpIaEv = NULL;
    TCPIP_DHCPV6_LIST_NODE* dNode;

    TCPIP_DHCPV6_CLIENT_STATE clientState = (TCPIP_DHCPV6_CLIENT_STATE)pClient->state;
    TCPIP_NET_IF* pNetIf = pClient->pDhcpIf;

    if(pIa != NULL)
    {
        iaEvent.iaType = (uint8_t)pIa->iaBody.type;
        iaEvent.iaSubState = (uint8_t)pIa->iaSubState;
        iaEvent.iaState = (uint8_t)pIa->iaState;
        iaEvent.iaIndex = (uint8_t)pIa->parentIx;
        pDhcpIaEv = &iaEvent;
    }

    TCPIP_Notification_Lock(&dhcpv6RegisteredUsers);
    for(dNode = FC_SglNode2ListNode(dhcpv6RegisteredUsers.list.head); dNode != NULL; dNode = dNode->next)
    {
        if(dNode->hNet == NULL || dNode->hNet == pNetIf)
        {   // trigger event
            (*dNode->handler)(pNetIf, clientState, pDhcpIaEv, dNode->hParam);
        }
    }
    TCPIP_Notification_Unlock(&dhcpv6RegisteredUsers);
#endif  // (M_TCPIP_DHCPV6_USER_NOTIFICATION != 0)

    F_DHCPV6DbgStatePrint_Client(pClient, true);
    F_DHCPV6DbgStatePrint_Ia(pIa, iaSubNotify);
}

TCPIP_DHCPV6_CLIENT_RES TCPIP_DHCPV6_Disable(TCPIP_NET_HANDLE hNet)
{
    TCPIP_DHCPV6_CLIENT_RES res = TCPIP_DHCPV6_CLIENT_RES_OK;

    while(true)
    {
        TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);
        if(pNetIf == NULL)
        {
            res = TCPIP_DHCPV6_CLIENT_RES_IF_DOWN;
            break;
        }

        bool doClose = false;
        TCPIP_DHCPV6_CLIENT_DCPT* pClient = NULL;

        OSAL_CRITSECT_DATA_TYPE lock = F_DHCPv6_Lock();

        if(dhcpv6ClientDcpt == NULL)
        {
            res = TCPIP_DHCPV6_CLIENT_RES_DOWN;
        }
        else
        {
            pClient = dhcpv6ClientDcpt + TCPIPStackNetIxGet(pNetIf);

            if(pClient->state == (uint16_t)TCPIP_DHCPV6_CLIENT_STATE_INIT || pClient->flags.busy != 0U)
            {
                res = TCPIP_DHCPV6_CLIENT_RES_BUSY;
            }
            else if(pClient->state == (uint16_t)TCPIP_DHCPV6_CLIENT_STATE_IDLE)
            {   // already disabled...
                res = TCPIP_DHCPV6_CLIENT_RES_OK;
            }
            else
            {   // disable the module; stop the Task state
                (void)F_DHCPV6Client_SetStateFlags(pClient, TCPIP_DHCPV6_CLIENT_STATE_IDLE, TCPIP_DHCPV6_RUN_FLAG_BUSY | TCPIP_DHCPV6_RUN_FLAG_RX_DISABLED);
                doClose = true;
            }
        }

        F_DHCPv6_Unlock(lock);

        if(!doClose)
        {
            break;
        }
    
        F_DHCPV6Close(pClient, TCPIP_DHCPV6_CLOSE_FLAG_CLOSE_ALL);

        pClient->flags.val = TCPIP_DHCPV6_RUN_FLAG_NONE;

        res = TCPIP_DHCPV6_CLIENT_RES_OK; 
        break;
    }

    return res;
}

TCPIP_DHCPV6_CLIENT_RES TCPIP_DHCPV6_Enable(TCPIP_NET_HANDLE hNet)
{
    TCPIP_DHCPV6_CLIENT_RES res;

    while(true)
    {
        TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);
        if(pNetIf == NULL)
        {
            res = TCPIP_DHCPV6_CLIENT_RES_IF_DOWN;
            break;
        }

        TCPIP_DHCPV6_CLIENT_DCPT* pClient = NULL;

        OSAL_CRITSECT_DATA_TYPE lock = F_DHCPv6_Lock();

        if(dhcpv6ClientDcpt == NULL)
        {
            res = TCPIP_DHCPV6_CLIENT_RES_DOWN;
        }
        else
        {
            pClient = dhcpv6ClientDcpt + TCPIPStackNetIxGet(pNetIf);

            if(pClient->state == (uint16_t)TCPIP_DHCPV6_CLIENT_STATE_INIT || pClient->flags.busy != 0U)
            {
                res = TCPIP_DHCPV6_CLIENT_RES_BUSY;
            }
            else
            {
                if(pClient->state == (uint16_t)TCPIP_DHCPV6_CLIENT_STATE_IDLE)
                {   // module disabled; start it
                    (void)F_DHCPV6Client_SetStateFlags(pClient, TCPIP_DHCPV6_CLIENT_STATE_REINIT, TCPIP_DHCPV6_RUN_FLAG_WAS_ENABLED);
                }
                // else already doing something...
                res = TCPIP_DHCPV6_CLIENT_RES_OK;
            }
        }

        F_DHCPv6_Unlock(lock);

        break;

    }

    return res;
}

TCPIP_DHCPV6_CLIENT_RES TCPIP_DHCPV6_AddrRelease(TCPIP_NET_HANDLE hNet, const IPV6_ADDR* addr)
{
    TCPIP_DHCPV6_CLIENT_RES res;

    while(true)
    {
        TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);
        if(pNetIf == NULL)
        {
            res = TCPIP_DHCPV6_CLIENT_RES_IF_DOWN;
            break;
        }

        TCPIP_DHCPV6_CLIENT_DCPT* pClient = NULL;

        OSAL_CRITSECT_DATA_TYPE lock = F_DHCPv6_Lock();

        if(dhcpv6ClientDcpt == NULL)
        {
            res = TCPIP_DHCPV6_CLIENT_RES_DOWN;
        }
        else
        {
            pClient = dhcpv6ClientDcpt + TCPIPStackNetIxGet(pNetIf);

            if(pClient->state == (uint16_t)TCPIP_DHCPV6_CLIENT_STATE_INIT || pClient->flags.busy != 0U)
            {
                res = TCPIP_DHCPV6_CLIENT_RES_BUSY;
            }
            else if(pClient->state == (uint16_t)TCPIP_DHCPV6_CLIENT_STATE_IDLE)
            {   // already disabled...
                res = TCPIP_DHCPV6_CLIENT_RES_DISABLED;
            }
            else
            {   // up and running; get the IA for this address
                TCPIP_DHCPV6_IA_DCPT* pIa = F_DHCPV6Client_FindIaByValidAddr(pClient, addr);
                if(pIa == NULL)
                {
                    res = TCPIP_DHCPV6_CLIENT_RES_NO_ADDR; 
                }
                else
                {
                    F_DHCPV6Ia_SetRunState(pIa, TCPIP_DHCPV6_IA_STATE_RELEASE, TCPIP_DHCPV6_IA_SUBSTATE_START);
                    res = TCPIP_DHCPV6_CLIENT_RES_OK; 
                }
            }
        }

        F_DHCPv6_Unlock(lock);

        break;
    }

    return res;
}
static UDP_SOCKET F_DHCPV6OpenSocket(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    bool     udpSuccess;
    uint16_t txBuffSize;
    TCPIP_UDP_SIGNAL_HANDLE sigHandle;
    TCPIP_NET_IF* pNetIf = pClient->pDhcpIf;

    UDP_SOCKET dhcpSkt = TCPIP_UDP_OpenClientSkt(IP_ADDRESS_TYPE_IPV6, dhcpv6ServerPort, NULL, (UDP_OPEN_TYPE)((uint16_t)UDP_OPEN_CLIENT | (uint16_t)UDP_OPEN_CONFIG_SERVICE));

    if(dhcpSkt != INVALID_UDP_SOCKET)
    {   
        // make sure the socket is created with enough TX space
        (void)TCPIP_UDP_OptionsGet(dhcpSkt, UDP_OPTION_TX_BUFF, (void*)&txBuffSize);
        if(txBuffSize < (uint16_t)TCPIP_DHCPV6_MIN_UDP_TX_BUFFER_SIZE)
        {
            (void)TCPIP_UDP_OptionsSet(dhcpSkt, UDP_OPTION_TX_BUFF, FC_Uint2VPtr((uint32_t)TCPIP_DHCPV6_MIN_UDP_TX_BUFFER_SIZE));
        }

        // bind to the DHCP local port
        udpSuccess = TCPIP_UDP_Bind(dhcpSkt, IP_ADDRESS_TYPE_IPV6, dhcpv6ClientPort,  NULL);
        if(udpSuccess)
        {   // bind to this interface
            (void)TCPIP_UDP_SocketNetSet(dhcpSkt, pNetIf);
            sigHandle = TCPIP_UDP_SignalHandlerRegister(dhcpSkt, TCPIP_UDP_SIGNAL_RX_DATA, &F_DHCPV6SocketRxSignalHandler, NULL);
            if(sigHandle == NULL)
            {
                udpSuccess = false;
            }
        }

        if(!udpSuccess)
        {
            (void)TCPIP_UDP_Close(dhcpSkt);
            dhcpSkt = INVALID_UDP_SOCKET;
        }
        (void)TCPIP_UDP_OptionsSet(dhcpSkt, (UDP_SOCKET_OPTION)((uint16_t)UDP_OPTION_STRICT_PORT | (uint16_t)UDP_OPTION_STRICT_SET_STICKY), (void*)false);
        // make sure we can receive all the server advertise messages 
        // enforce a minimum value
        uint16_t rxQLimit = TCPIP_DHCPV6_MIN_RX_QUEUE_LIMIT;
        if(pClient->nIanaDcpts + pClient->nIataDcpts > rxQLimit)
        {
            rxQLimit = pClient->nIanaDcpts + pClient->nIataDcpts; 
        }
        (void)TCPIP_UDP_OptionsSet(dhcpSkt, UDP_OPTION_RX_QUEUE_LIMIT, FC_Uint2VPtr((uint32_t)rxQLimit));
    }


    return dhcpSkt;
}

static void F_DHCPv6FlushSocket(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    UDP_SOCKET dhcpSkt = pClient->dhcpSkt;

    // simply discard any messages
    if(dhcpSkt != INVALID_SOCKET)
    {
        while(TCPIP_UDP_GetIsReady(dhcpSkt) != 0U)
        {
            (void)TCPIP_UDP_Discard(dhcpSkt);
        }
    }
}



// status reporting
TCPIP_DHCPV6_CLIENT_RES TCPIP_DHCPV6_ClientInfoGet(TCPIP_NET_HANDLE hNet, TCPIP_DHCPV6_CLIENT_INFO* pClientInfo)
{
    TCPIP_DHCPV6_CLIENT_RES res;

    while(true)
    {
        if(pClientInfo == NULL)
        {
            res = TCPIP_DHCPV6_CLIENT_RES_ARG_ERR;
            break;
        }

        if(dhcpv6ClientDcpt == NULL)
        {
            res = TCPIP_DHCPV6_CLIENT_RES_DOWN;
            break;
        }

        TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);
        if(pNetIf == NULL)
        {
            res = TCPIP_DHCPV6_CLIENT_RES_IF_DOWN;
            break;
        }

        size_t copyBytes;
        TCPIP_DHCPV6_CLIENT_DCPT* pClient = dhcpv6ClientDcpt + TCPIPStackNetIxGet(pNetIf);
        pClientInfo->clientState = (TCPIP_DHCPV6_CLIENT_STATE)pClient->state;
        pClientInfo->totIanas = (size_t)pClient->nIanaDcpts;
        pClientInfo->totIatas = (size_t)pClient->nIataDcpts;

        TCPIP_UINT32_VAL nIas;
        DOUBLE_LIST* pSrchList = pClient->iaStateList + (int)TCPIP_DHCPV6_IA_STATE_BOUND;
        nIas.Val = F_DHCPv6_FindIAsInList(pSrchList, true);
        pClientInfo->nIanas = nIas.w[0];
        pClientInfo->nIatas = nIas.w[1];
        pClientInfo->nFreeIas = TCPIP_Helper_DoubleListCount(&pClient->iaFreeList);

        pClientInfo->solIas = TCPIP_Helper_DoubleListCount(pClient->iaStateList + (int)TCPIP_DHCPV6_IA_STATE_SOLICIT);
        pClientInfo->reqIas = TCPIP_Helper_DoubleListCount(pClient->iaStateList + (int)TCPIP_DHCPV6_IA_STATE_REQUEST);
        pClientInfo->dadIas = TCPIP_Helper_DoubleListCount(pClient->iaStateList + (int)TCPIP_DHCPV6_IA_STATE_DAD);
        pClientInfo->declineIas = TCPIP_Helper_DoubleListCount(pClient->iaStateList + (int)TCPIP_DHCPV6_IA_STATE_DECLINE);
        pClientInfo->boundIas = TCPIP_Helper_DoubleListCount(pClient->iaStateList + (int)TCPIP_DHCPV6_IA_STATE_BOUND);
        pClientInfo->renewIas = TCPIP_Helper_DoubleListCount(pClient->iaStateList + (int)TCPIP_DHCPV6_IA_STATE_RENEW);
        pClientInfo->rebindIas = TCPIP_Helper_DoubleListCount(pClient->iaStateList + (int)TCPIP_DHCPV6_IA_STATE_REBIND);
        pClientInfo->confirmIas = TCPIP_Helper_DoubleListCount(pClient->iaStateList + (int)TCPIP_DHCPV6_IA_STATE_CONFIRM);
        pClientInfo->releaseIas = TCPIP_Helper_DoubleListCount(pClient->iaStateList + (int)TCPIP_DHCPV6_IA_STATE_RELEASE);
        pClientInfo->transIas = TCPIP_Helper_DoubleListCount(pClient->iaStateList + (int)TCPIP_DHCPV6_IA_STATE_ERROR_TRANSIENT);

        pClientInfo->totBuffers = pClient->nMsgBuffers;
        pClientInfo->freeBuffers = TCPIP_Helper_SingleListCount(&pClient->buffFreeList);

        pClientInfo->dhcpTime = TCPIP_MsecCountGet();
        pClientInfo->lastStatusCode = (TCPIP_DHCPV6_SERVER_STATUS_CODE)pClient->cliLastStatusCode;
        if(pClient->cliStatusMsgSize != 0U && pClientInfo->statusBuff != NULL)
        {
            copyBytes = pClient->cliStatusMsgSize < pClientInfo->statusBuffSize ? pClient->cliStatusMsgSize : pClientInfo->statusBuffSize;
            (void)memcpy(pClientInfo->statusBuff, pClient->cliLastStatusMsg, copyBytes);
        }
        pClientInfo->nDnsServers = pClient->nDnsServers;
        pClientInfo->domainSearchListSize = pClient->domainSearchListSize;
#if defined(TCPIP_DHCPV6_DNS_SERVERS_NO) && (TCPIP_DHCPV6_DNS_SERVERS_NO != 0)
        if(pClientInfo->dnsBuff != NULL)
        {
            copyBytes = sizeof(pClient->dnsServersAdd) < pClientInfo->dnsBuffSize ? sizeof(pClient->dnsServersAdd) : pClientInfo->dnsBuffSize;
            (void)memcpy(pClientInfo->dnsBuff, pClient->dnsServersAdd, copyBytes);
        }
#endif  // defined(TCPIP_DHCPV6_DNS_SERVERS_NO) && (TCPIP_DHCPV6_DNS_SERVERS_NO != 0)
#if defined(TCPIP_DHCPV6_DOMAIN_SEARCH_LIST_SIZE) && (TCPIP_DHCPV6_DOMAIN_SEARCH_LIST_SIZE != 0)
        if(pClientInfo->domainBuff != NULL)
        {
            copyBytes = sizeof(pClient->domainSearchList) < pClientInfo->domainBuffSize ? sizeof(pClient->domainSearchList) : pClientInfo->domainBuffSize;
            (void)memcpy(pClientInfo->domainBuff, pClient->domainSearchList, copyBytes);
        }
#endif  // defined(TCPIP_DHCPV6_DOMAIN_SEARCH_LIST_SIZE) && (TCPIP_DHCPV6_DOMAIN_SEARCH_LIST_SIZE != 0)

        res = TCPIP_DHCPV6_CLIENT_RES_OK;
        break;
    } 

    return res;
}

TCPIP_DHCPV6_CLIENT_RES TCPIP_DHCPV6_IaInfoGet(TCPIP_NET_HANDLE hNet, TCPIP_DHCPV6_IA_INFO* pIaInfo)
{
    TCPIP_NET_IF* pNetIf = NULL;
    int iaState, iaIndex;
    
    TCPIP_DHCPV6_CLIENT_RES res = TCPIP_DHCPV6_CLIENT_RES_OK;

    // sanity check
    while(true)
    {
        if(pIaInfo == NULL || (int)pIaInfo->iaState >= (int)TCPIP_DHCPV6_IA_STATE_NUMBER)
        {
            res = TCPIP_DHCPV6_CLIENT_RES_ARG_ERR;
            break;
        }

        if(dhcpv6ClientDcpt == NULL)
        {
            res = TCPIP_DHCPV6_CLIENT_RES_DOWN;
            break;
        }

        pNetIf = TCPIPStackHandleToNetUp(hNet);
        if(pNetIf == NULL)
        {
            res = TCPIP_DHCPV6_CLIENT_RES_IF_DOWN;
            break;
        }

        break;
    }
    
    if(res != TCPIP_DHCPV6_CLIENT_RES_OK)
    {
        return res;
    }

    DOUBLE_LIST* srchList;
    TCPIP_DHCPV6_CLIENT_DCPT* pClient = dhcpv6ClientDcpt + TCPIPStackNetIxGet(pNetIf);
    iaState = (int)pIaInfo->iaState;
    iaIndex = (int)pIaInfo->iaIndex;

    if(iaState < 0)
    {   // search the free IA list
        srchList = &pClient->iaFreeList;
    }
    else
    {
        srchList = pClient->iaStateList + iaState;
    }

    // search

    TCPIP_DHCPV6_IA_DCPT* pFoundIa = NULL;
    TCPIP_DHCPV6_IA_DCPT* pIa;

    for(pIa = FC_DblNode2IaDcpt(srchList->head); pIa != NULL; pIa = pIa->next)
    {
        if(iaIndex < 0 || iaIndex == pIa->parentIx)
        {   // found it
            pFoundIa = pIa;
            break;
        }
    }

    if(pFoundIa == NULL)
    {
        return TCPIP_DHCPV6_CLIENT_RES_IX_ERR;
    }

    pIaInfo->iaType = pFoundIa->iaBody.type;
    pIaInfo->iaSubState = (TCPIP_DHCPV6_IA_SUBSTATE)pFoundIa->iaSubState;
    pIaInfo->iaState = (TCPIP_DHCPV6_IA_STATE)pFoundIa->iaState;
    pIaInfo->iaIndex = pFoundIa->parentIx;
    pIaInfo->iaId = pFoundIa->iaBody.genId;
    pIaInfo->tAcquire = pFoundIa->iaBody.tAcquire;
    if(pFoundIa->iaBody.type == TCPIP_DHCPV6_IA_TYPE_IANA)
    {
        pIaInfo->t1 = pFoundIa->iaBody.ianaBody.t1;
        pIaInfo->t2 = pFoundIa->iaBody.ianaBody.t2;
    }
    else
    {
        pIaInfo->t1 = 0;
        pIaInfo->t2 = 0;
    }

    TCPIP_DHCPV6_ADDR_NODE* addNode;
    for(addNode = FC_SglNode2AddNode(pFoundIa->addList.head); addNode != NULL; addNode = addNode->next)
    {
        if(!addNode->inUse)
        {
            continue;
        }

        pIaInfo->ipv6Addr = addNode->addBody.ipv6Addr;
        pIaInfo->prefLTime = addNode->addBody.prefLTime;
        pIaInfo->validLTime = addNode->addBody.validLTime;
        break;
    }

    pIaInfo->lastStatusCode = (TCPIP_DHCPV6_SERVER_STATUS_CODE)pFoundIa->iaLastStatusCode;
    if(pFoundIa->iaStatusMsgSize != 0U && pIaInfo->statusBuff != NULL)
    {
        size_t copyBytes;
        copyBytes = pFoundIa->iaStatusMsgSize < pIaInfo->statusBuffSize ? pFoundIa->iaStatusMsgSize : pIaInfo->statusBuffSize;
        (void)memcpy(pIaInfo->statusBuff, pFoundIa->iaLastStatusMsg, copyBytes);
    }

    pIaInfo->msgBuffer = pFoundIa->msgBuffer;
    pIaInfo->nextIndex = (pFoundIa->next != NULL) ? pFoundIa->next->parentIx: -1;

    return TCPIP_DHCPV6_CLIENT_RES_OK;
}

#if defined(TCPIP_DHCPV6_STATISTICS_ENABLE) && (TCPIP_DHCPV6_STATISTICS_ENABLE != 0)
TCPIP_DHCPV6_CLIENT_RES TCPIP_DHCPV6_Statistics(TCPIP_NET_HANDLE hNet, TCPIP_DHCPV6_CLIENT_STATISTICS* pStat)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);
    if(pNetIf == NULL)
    {
        return TCPIP_DHCPV6_CLIENT_RES_IF_DOWN;
    }

    if(dhcpv6ClientDcpt == NULL)
    {
        return TCPIP_DHCPV6_CLIENT_RES_DOWN;
    }

    TCPIP_DHCPV6_CLIENT_DCPT* pClient = dhcpv6ClientDcpt + TCPIPStackNetIxGet(pNetIf);


    if(pStat)
    {
        pClient->stat.msgBuffers = pClient->nMsgBuffers;
        pClient->stat.freeBuffers = TCPIP_Helper_SingleListCount(&pClient->buffFreeList);
        pClient->stat.rxMessages = TCPIP_Helper_SingleListCount(&pClient->rxMsgList);
        pClient->stat.txMessages = TCPIP_Helper_SingleListCount(&pClient->txMsgList);
        pClient->stat.advMessages = TCPIP_Helper_SingleListCount(&pClient->advertiseList);
        pClient->stat.replyMessages = TCPIP_Helper_SingleListCount(&pClient->replyList);

        *pStat = pClient->stat;
    }

    return TCPIP_DHCPV6_CLIENT_RES_OK;

}
#else
TCPIP_DHCPV6_CLIENT_RES TCPIP_DHCPV6_Statistics(TCPIP_NET_HANDLE hNet, TCPIP_DHCPV6_CLIENT_STATISTICS* pStat)
{
    return TCPIP_DHCPV6_CLIENT_RES_NO_SRVC;
}
#endif  // defined(TCPIP_DHCPV6_STATISTICS_ENABLE) && (TCPIP_DHCPV6_STATISTICS_ENABLE != 0)



// generates a client DUID 
static bool F_DHCPV6Duid_Generate(TCPIP_DHCPV6_DUID_TYPE duidType, TCPIP_DHCPV6_DUID_DCPT* pDuid, TCPIP_NET_IF* pDuidIf)
{
    if(duidType != TCPIP_DHCPV6_DUID_TYPE_LL)
    {   
        return false;
    }

    // generate a TCPIP_DHCPV6_DUID_TYPE_LL

    pDuid->duidBody.duidLL.duid_type = TCPIP_Helper_htons((uint16_t)TCPIP_DHCPV6_DUID_TYPE_LL);
    pDuid->duidBody.duidLL.hw_type = TCPIP_Helper_htons((uint16_t)TCPIP_DHCPV6_HW_TYPE);
    (void)memcpy(pDuid->duidBody.duidLL.ll_address, TCPIPStack_NetMACAddressGet(pDuidIf), sizeof(pDuid->duidBody.duidLL.ll_address));
    
    pDuid->duidType = (uint16_t)TCPIP_DHCPV6_DUID_TYPE_LL; 
    pDuid->duidLen = (uint16_t)sizeof(pDuid->duidBody.duidLL); 
    return true;
} 

static TCPIP_DHCPV6_MSG_TX_RESULT F_DHCPV6Ia_CheckMsgTransmitStatus(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    int32_t     tBaseMs, tPrevMs;   // milliseconds
    uint16_t    randMin, randMax;   // range of random values
    int16_t     randOffset;         // applied offset
    uint32_t    tickCurr;           // current sys tick
    uint32_t    sysFreq = SYS_TMR_TickCounterFrequencyGet();

    tickCurr = SYS_TMR_TickCountGet();
    
    TCPIP_DHCPV6_MSG_TRANSMIT_DCPT* pDcpt = &pIa->msgTxDcpt;

    // check if first attempt
    while(pDcpt->rc == 0U)
    {   // message just starts to be transmitted
        if(pDcpt->bounds.iDelay == 0U)
        {   // no initial delay, go straight ahead
            pDcpt->waitTick = 0U;
            break;
        }

        if(pDcpt->waitTick == 0U)
        {   // set up the waiting period: RAND [0, iDelay] 
            uint32_t idelayMs = SYS_RANDOM_PseudoGet() % (pDcpt->bounds.iDelay * 1000U + 1U);
            pDcpt->waitTick = tickCurr + (idelayMs * sysFreq) / 1000U;
            F_DHCPV6DbgMsg_IaIDelay(pIa, idelayMs);
            return TCPIP_DHCPV6_MSG_TX_RES_PENDING;
        }

        // wait period already started
        if(((int32_t)tickCurr - (int32_t)pDcpt->waitTick) < 0)
        {   // wait some more
            return TCPIP_DHCPV6_MSG_TX_RES_PENDING;
        }

        // iDelay passed; go ahead: calculate the next tmo and transmit
        pDcpt->waitTick = 0U;
        F_DHCPV6DbgMsg_IaIDelayTmo(pIa);
        break;
    }

    if(pDcpt->waitTick != 0U)
    {   // we're waiting for timeout
        while(((int32_t)tickCurr - (int32_t)pDcpt->waitTick) < 0)
        {   // not yet time for retransmission
            return TCPIP_DHCPV6_MSG_TX_RES_PENDING;
        }
    }

    // retransmission count: [1, MRC]
    pDcpt->rc++;

    // message needs (re)transmission
    // set the timeouts
    uint32_t secCurr = TCPIP_SecCountGet(); // current second
    uint32_t msecCurr = TCPIP_MsecCountGet();  // current ms
    
    if(pDcpt->rc > 1U && pDcpt->bounds.mrc != 0U)
    {
        if(pDcpt->rc > pDcpt->bounds.mrc)
        {   // message exceeded the number of retries
            F_DHCPV6DbgMsg_IaTxExceed(pIa, TCPIP_DHCPV6_MSG_TX_RES_RETRY_EXCEEDED, pDcpt->iTime);
            return TCPIP_DHCPV6_MSG_TX_RES_RETRY_EXCEEDED;
        }
    }

    if(pDcpt->rc > 1U && pDcpt->bounds.mrd != 0U)
    {   // avoid doing the test for the 1st transmission
        if(secCurr - pDcpt->iTime >= pDcpt->bounds.mrd)
        {   // message exceeded the allocated time
            F_DHCPV6DbgMsg_IaTxExceed(pIa, TCPIP_DHCPV6_MSG_TX_RES_TIME_EXCEEDED, pDcpt->iTime);
            return TCPIP_DHCPV6_MSG_TX_RES_TIME_EXCEEDED;
        }
    }

    // random ranges
    randMin = TCPIP_DHCPV6_RAND_MIN_RANGE;
    randMax = TCPIP_DHCPV6_RAND_MAX_RANGE;
    randOffset = TCPIP_DHCPV6_RAND_OFSSET; 

    // calculate the next retransmission time/timeouts
    if(pDcpt->rc == 1U)
    {   // first message
        tBaseMs = tPrevMs = (int32_t)pDcpt->bounds.irt * 1000;
        pDcpt->iTime = secCurr;
        pDcpt->iTimeMs = msecCurr;
        pDcpt->elapsedTime = 0U;
        if(pIa->iaState == (uint16_t)TCPIP_DHCPV6_IA_STATE_SOLICIT)
        {   // solicit RT has to be greater than IRT! 
            randMin = TCPIP_DHCPV6_SOL_RAND_MIN_RANGE;
            randMax = TCPIP_DHCPV6_SOL_RAND_MAX_RANGE;
            randOffset = TCPIP_DHCPV6_SOL_RAND_OFSSET; 
        }
    }
    else
    {   // subsequent retry
        tBaseMs = 2 * pDcpt->rt;
        tPrevMs = pDcpt->rt;
        pDcpt->elapsedTime = (msecCurr - pDcpt->iTimeMs) / 10U;   // convert to 10^-2 sec 
    }

    if(pDcpt->bounds.mrt != 0U && tBaseMs > (int32_t)pDcpt->bounds.mrt * 1000)
    {   // limit the rt
        tBaseMs = tPrevMs = (int32_t)pDcpt->bounds.mrt * 1000;
    }

    // get a random value [randMin, randMax]
    uint16_t randVal = randMin + (uint16_t)(SYS_RANDOM_PseudoGet() % ((uint32_t)randMax - (uint32_t)randMin + 1U));
    // get the (negative) multiplication factor by adjusting with randOffset
    int16_t randMul = (int16_t)randVal - randOffset;
    int32_t rtmoMs = tBaseMs + (tPrevMs * randMul) / (int16_t)TCPIP_DHCPV6_RAND_DIV;

    if(pDcpt->bounds.mrd != 0U)
    {
        uint32_t currMs = TCPIP_MsecCountGet(); 
        uint32_t tExpMs = (pDcpt->iTime + pDcpt->bounds.mrd) * 1000U; 
        if(((int32_t)currMs + rtmoMs - (int32_t)tExpMs) > 0) 
        {   // don't exceed the MRD
            rtmoMs = (int32_t)tExpMs - (int32_t)currMs;
        }
    } 

    pDcpt->waitTick = tickCurr + ((uint32_t)rtmoMs * sysFreq) / 1000U;

    F_DHCPV6DbgMsg_IaRTmo(pIa, rtmoMs, tPrevMs, pDcpt);

    pDcpt->rt = rtmoMs;

    // message needs to be transmitted
    return TCPIP_DHCPV6_MSG_TX_RES_OK;

}

// copies a DUID in the corresponding buffer
static int F_DHCPV6Option_WriteDuid(TCPIP_DHCPV6_DUID_DCPT* pDuid, TCPIP_DHCPV6_MSG_WRITE_DCPT* pWrDcpt)
{
    if(pDuid->duidType == (uint16_t)TCPIP_DHCPV6_DUID_TYPE_NONE || pDuid->duidLen == 0U)
    {   // invalid ?
        return -1;
    }

    if((uint32_t)pWrDcpt->writeSpace < pDuid->duidLen)
    {   // not enough room in this buffer
        return -1;
    }

    (void)memcpy(pWrDcpt->pWrite, pDuid->duidBody.v8, pDuid->duidLen);
    return (int)pDuid->duidLen;
} 


// returns size of the message written
// -1 if some error
static int F_DHCPV6Ia_MessageWrite(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_MSG_WRITE_DCPT* pWrDcpt = &pIa->wrDcpt;
    
    if((uint32_t)pWrDcpt->writeSpace < sizeof(TCPIP_DHCPV6_MESSAGE_HEADER_BARE))
    {   // not enough room space
        return -1;
    }
    
    int32_t iniBytes = pWrDcpt->writeBytes; 
    TCPIPStack_Condition(iniBytes == 0, __FILE__, __func__, __LINE__);  // should be 0

    TCPIP_DHCPV6_CLIENT_MSG_TYPE cliMsgType = pIa->cliMsgType;

    TCPIP_DHCPV6_MESSAGE_HEADER* pHdr = pWrDcpt->uOptPtr.pHdr;

    (void)memcpy(pHdr->transId, pIa->transactionId.v, sizeof(pHdr->transId));
    pHdr->msg_type = dhcpv6ClientToMessageTypeTbl[cliMsgType];

    pWrDcpt->writeSpace -= (int16_t)sizeof(TCPIP_DHCPV6_MESSAGE_HEADER_BARE);
    pWrDcpt->writeBytes += (int16_t)sizeof(TCPIP_DHCPV6_MESSAGE_HEADER_BARE);
    pWrDcpt->pWrite += sizeof(TCPIP_DHCPV6_MESSAGE_HEADER_BARE);

    // start writing message options


    TCPIP_DHCPV6_OPTION_MASK_SET_0* pMsgMask = DHCPV6_MSG_OPTION_CURR_MASK_TBL + (int)cliMsgType;
    uint32_t optCode;    
    const uint32_t* pCurrOpt = NULL;
    uint32_t currOptMask = 0U;

    while(true)
    {
        optCode = F_DHCPV6OptionSet_ExtractCode(pMsgMask->optionSet, pMsgMask->nSets, &pCurrOpt, &currOptMask);
        if(optCode == 0U)
        {   // done
            break;
        }

        if(F_DHCPV6Option_Write(optCode, pIa) == -1)
        {   // failed to write an option
            return -1;
        }
    }
    

    return pWrDcpt->writeBytes - iniBytes;
} 


static int F_DHCPV6Option_Write(uint32_t optCode, TCPIP_DHCPV6_IA_DCPT* pIa)
{
    int optSize = -1;

    if(optCode < sizeof(T_DHCPV6Option_WriteFncTbl)/sizeof(*T_DHCPV6Option_WriteFncTbl))
    {
        F_DHCPV6Option_WriteFnc optFnc = T_DHCPV6Option_WriteFncTbl[optCode];
        if(optFnc != NULL)
        {   // option has associated function
            optSize = (*optFnc)(pIa);
            if(optSize != -1)
            {
                TCPIP_DHCPV6_MSG_WRITE_DCPT* pWrDcpt = &pIa->wrDcpt;
                pWrDcpt->writeSpace -= (int16_t)optSize;
                pWrDcpt->writeBytes += (int16_t)optSize;
                pWrDcpt->pWrite += optSize;
            }
        }
    } 


    return optSize;
}


// DHCPV6_OPT_CODE_CLIENT_ID
static int F_DHCPV6Option_WriteClientId(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_CLIENT_DCPT* pParent = pIa->pParent;
    return F_DHCPV6Option_WriteId(&pParent->clientDuid, DHCPV6_OPT_CODE_CLIENT_ID, &pIa->wrDcpt);
}


// DHCPV6_OPT_CODE_SERVER_ID
static int F_DHCPV6Option_WriteServerId(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    return F_DHCPV6Option_WriteId(&pIa->serverDuid, DHCPV6_OPT_CODE_SERVER_ID, &pIa->wrDcpt);
}

static int F_DHCPV6Option_WriteId(TCPIP_DHCPV6_DUID_DCPT* pDuid, uint32_t optCode, TCPIP_DHCPV6_MSG_WRITE_DCPT* pWrDcpt)
{

    if(pWrDcpt->writeSpace < (int16_t)sizeof(TCPIP_DHCPV6_OPTION_ID))
    {
        return -1;
    }

    TCPIP_DHCPV6_MSG_WRITE_DCPT  duidWrDcpt;
    duidWrDcpt.writeSpace = pWrDcpt->writeSpace - (int16_t)sizeof(TCPIP_DHCPV6_OPTION_ID);
    duidWrDcpt.pWrite = pWrDcpt->pWrite + sizeof(TCPIP_DHCPV6_OPTION_ID);

    int duidSize = F_DHCPV6Option_WriteDuid(pDuid, &duidWrDcpt);

    if(duidSize != -1)
    {
        TCPIP_DHCPV6_OPTION_ID* pId = pWrDcpt->uOptPtr.pOptId;

        pId->optCode = TCPIP_Helper_htons((uint16_t)optCode);
        pId->optLen = TCPIP_Helper_htons((uint16_t)duidSize);
        duidSize += (int)sizeof(TCPIP_DHCPV6_OPTION_ID);
    }

    return duidSize;
}




// DHCPV6_OPT_CODE_IA_NA
// write a IA_NA and the associated address
// and addresses marked addInTx are written!
static int F_DHCPV6Option_WriteIA_NA(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    int addrOptSize, optSize;
    TCPIP_DHCPV6_OPTION_IANA* pOptIana;
    TCPIP_DHCPV6_MSG_WRITE_DCPT  wrDcpt;

    if(pIa->iaBody.type != TCPIP_DHCPV6_IA_TYPE_IANA)
    {   // nothing to write
        return 0;
    }

    // local copy descriptor
    wrDcpt =  pIa->wrDcpt;

    if(wrDcpt.writeSpace < (int16_t)sizeof(TCPIP_DHCPV6_OPTION_IANA_BARE))
    {
        return -1;
    }

    wrDcpt.writeSpace -= (int16_t)sizeof(TCPIP_DHCPV6_OPTION_IANA_BARE);
    wrDcpt.pWrite += sizeof(TCPIP_DHCPV6_OPTION_IANA_BARE);

    // add the IA associated address
    addrOptSize = 0;
    if(pIa->flags.addInTx != 0U)
    {
        TCPIP_DHCPV6_ADDR_NODE* addNode;
        for(addNode = FC_SglNode2AddNode(pIa->addList.head); addNode != NULL; addNode = addNode->next)
        {
            if(!addNode->inUse)
            {
                continue;
            }

            optSize = F_DHCPV6Option_WriteIA_Addr(&addNode->addBody, &wrDcpt);
            if(optSize == -1)
            {   // failed
                return -1;
            }
            
            addrOptSize += optSize;
            wrDcpt.writeSpace -= (int16_t)optSize;
            wrDcpt.pWrite += optSize;
        }
    }
#if defined(M_TCPIP_DHCPV6_WRITE_IA_WITH_ADDRESSES_ONLY)
    if(addrOptSize == 0)
    {   // no addresses to be tx-ed in this IA; ignore
        return 0;
    }
#endif  // defined(M_TCPIP_DHCPV6_WRITE_IA_WITH_ADDRESSES_ONLY)

    // write the IANA
    pOptIana = FC_U8Ptr2OptIana(pIa->wrDcpt.pWrite);
    pOptIana->optCode = TCPIP_Helper_htons((uint16_t)DHCPV6_OPT_CODE_IA_NA);
    pOptIana->optLen = TCPIP_Helper_htons((uint16_t)addrOptSize + (uint16_t)sizeof(pOptIana->body));
    pOptIana->body.iaid = TCPIP_Helper_htonl(pIa->iaBody.genId);
    pOptIana->body.t1 = TCPIP_Helper_htonl(pIa->iaBody.ianaBody.t1);
    pOptIana->body.t2 = TCPIP_Helper_htonl(pIa->iaBody.ianaBody.t2);

    return addrOptSize + (int)sizeof(TCPIP_DHCPV6_OPTION_IANA_BARE);

}

// DHCPV6_OPT_CODE_IA_TA
// write a IA_TA and the associated address
// and addresses marked addInTx are written!
static int F_DHCPV6Option_WriteIA_TA(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    int addrOptSize, optSize;
    TCPIP_DHCPV6_OPTION_IATA* pOptIata;
    TCPIP_DHCPV6_MSG_WRITE_DCPT  wrDcpt;

    if(pIa->iaBody.type != TCPIP_DHCPV6_IA_TYPE_IATA)
    {   // nothing to write
        return 0;
    }

    // local copy descriptor
    wrDcpt =  pIa->wrDcpt;

    if(wrDcpt.writeSpace < (int16_t)sizeof(TCPIP_DHCPV6_OPTION_IATA_BARE))
    {
        return -1;
    }

    wrDcpt.writeSpace -= (int16_t)sizeof(TCPIP_DHCPV6_OPTION_IATA_BARE);
    wrDcpt.pWrite += sizeof(TCPIP_DHCPV6_OPTION_IATA_BARE);

    // add the IA associated address
    addrOptSize = 0;
    if(pIa->flags.addInTx != 0U)
    {
        TCPIP_DHCPV6_ADDR_NODE* addNode;
        for(addNode = FC_SglNode2AddNode(pIa->addList.head); addNode != NULL; addNode = addNode->next)
        {
            if(!addNode->inUse)
            {
                continue;
            }

            optSize = F_DHCPV6Option_WriteIA_Addr(&addNode->addBody, &wrDcpt);
            if(optSize == -1)
            {   // failed
                return -1;
            }
            
            addrOptSize += optSize;
            wrDcpt.writeSpace -= (int16_t)optSize;
            wrDcpt.pWrite += optSize;
        }
    }

#if defined(M_TCPIP_DHCPV6_WRITE_IA_WITH_ADDRESSES_ONLY)
    if(addrOptSize == 0)
    {   // no addresses to be tx-ed in this IA; ignore
        return 0;
    }
#endif  // defined(M_TCPIP_DHCPV6_WRITE_IA_WITH_ADDRESSES_ONLY)

    // write the IATA
    pOptIata = pIa->wrDcpt.uOptPtr.pOptIata;
    pOptIata->optCode = TCPIP_Helper_htons((uint16_t)DHCPV6_OPT_CODE_IA_TA);
    pOptIata->optLen = TCPIP_Helper_htons((uint16_t)addrOptSize + (uint16_t)sizeof(pOptIata->body));
    pOptIata->body.iaid = TCPIP_Helper_htonl(pIa->iaBody.genId);

    return addrOptSize + (int)sizeof(TCPIP_DHCPV6_OPTION_IATA_BARE);
}

static int F_DHCPV6Option_WriteIA_Addr(TCPIP_DHCPV6_OPTION_IA_ADDR_BODY* pASpec, TCPIP_DHCPV6_MSG_WRITE_DCPT* pWrDcpt)
{
    if(pWrDcpt->writeSpace < (int16_t)sizeof(TCPIP_DHCPV6_OPTION_IA_ADDR))
    {
        return -1;
    }

    TCPIP_DHCPV6_OPTION_IA_ADDR* pIaAddr = pWrDcpt->uOptPtr.pIaAddr;

    pIaAddr->optCode = TCPIP_Helper_htons((uint16_t)DHCPV6_OPT_CODE_IA_ADDR);
    pIaAddr->optLen = TCPIP_Helper_htons((uint16_t)sizeof(pIaAddr->body));

    pIaAddr->body.prefLTime = TCPIP_Helper_htonl(pASpec->prefLTime);
    pIaAddr->body.validLTime = TCPIP_Helper_htonl(pASpec->validLTime);
    (void)memcpy(&pIaAddr->body.ipv6Addr, &pASpec->ipv6Addr, sizeof(pIaAddr->body.ipv6Addr));


   return (int)sizeof(TCPIP_DHCPV6_OPTION_IA_ADDR); 
}


// DHCPV6_OPT_CODE_OPTION_REQ
static int F_DHCPV6Option_WriteOptionRequest(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    uint32_t nReqCodes;

    TCPIP_DHCPV6_MSG_WRITE_DCPT* pWrDcpt = &pIa->wrDcpt;

    TCPIP_DHCPV6_OPTION_MASK_SET* pMaskSet = pIa->oroOptMask.pMaskSet;


    uint32_t* optionSet = pMaskSet->optionSet;
    nReqCodes = F_DHCPV6OptionSet_CodesNo(optionSet, pMaskSet->nSets);

    if(nReqCodes == 0U)
    {   // no requests to write; shouldn't happen
        return 0;
    }

    // calculate needed size
    // each requested option is 2 bytes
    uint32_t reqCodeSize = nReqCodes << 1U;
    TCPIPStack_Assert(reqCodeSize < 0xffffU, __FILE__, __func__, __LINE__);
    int optReqSize = (int)sizeof(TCPIP_DHCPV6_OPTION_OPTION_REQ_BARE) + (int)reqCodeSize;

    if(pWrDcpt->writeSpace < optReqSize)
    {   // not enough room
        return -1;
    }
    
    TCPIP_DHCPV6_OPTION_OPTION_REQ* pOptReq = pWrDcpt->uOptPtr.pOptReq;
    pOptReq->optCode = TCPIP_Helper_htons((uint16_t)DHCPV6_OPT_CODE_OPTION_REQ);
    pOptReq->optLen = TCPIP_Helper_htons((uint16_t)reqCodeSize);

    const uint32_t* pCurrReq = NULL;
    uint32_t currReqMask = 0U;

    uint32_t reqCode;    
    int reqIx = 0;
    while(true)
    {
        reqCode = F_DHCPV6OptionSet_ExtractCode(optionSet, pMaskSet->nSets, &pCurrReq, &currReqMask);
        if(reqCode == 0U)
        {   // done
            break;
        }

        pOptReq->reqCode[reqIx++] = TCPIP_Helper_htons((uint16_t)reqCode);
    }


    return optReqSize;
}

// DHCPV6_OPT_CODE_ELAPSED_TIME
static int F_DHCPV6Option_WriteElapsedTime(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_MSG_WRITE_DCPT* pWrDcpt = &pIa->wrDcpt;

    if(pWrDcpt->writeSpace < (int16_t)sizeof(TCPIP_DHCPV6_OPTION_ELAPSED_TIME))
    {   // not enough room
        return -1;
    }

    TCPIP_DHCPV6_OPTION_ELAPSED_TIME* pElTime = pWrDcpt->uOptPtr.pElTime;
    pElTime->optCode = TCPIP_Helper_htons((uint16_t)DHCPV6_OPT_CODE_ELAPSED_TIME);
    pElTime->optLen = TCPIP_Helper_htons((uint16_t)sizeof(pElTime->elapsedTime));

    uint32_t tVal = pIa->msgTxDcpt.elapsedTime;
    if(tVal >= 0x10000U)
    {
        tVal = 0xffffU;
    }
    pElTime->elapsedTime = TCPIP_Helper_htons((uint16_t)tVal);

    return (int)sizeof(TCPIP_DHCPV6_OPTION_ELAPSED_TIME);
}

// DHCPV6_OPT_CODE_RAPID_COMMIT
static int F_DHCPV6Option_WriteRapidCommit(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_MSG_WRITE_DCPT* pWrDcpt = &pIa->wrDcpt;

    if(pWrDcpt->writeSpace < (int16_t)sizeof(TCPIP_DHCPV6_OPTION_RAPID_COMMIT))
    {   // not enough room
        return -1;
    }
    
    TCPIP_DHCPV6_OPTION_RAPID_COMMIT* pRCommit = pWrDcpt->uOptPtr.pRCommit;
    pRCommit->optCode = TCPIP_Helper_htons((uint16_t)DHCPV6_OPT_CODE_RAPID_COMMIT);
    pRCommit->optLen = 0;
    return (int)sizeof(TCPIP_DHCPV6_OPTION_RAPID_COMMIT);
}

// generates a IA_TA/IA_NA IAID for a interface
// only 16 bits counters/rounds per interface are supported
static void F_DHCPV6Ia_IdGenerate(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_NET_IF* pIaidIf, TCPIP_DHCPV6_IA_ROUND* pIaRound)
{
    uint16_t    roundCnt;
    TCPIP_UINT64_VAL    genId;    // construct the IAID here
    
    bool isIana = pIa->iaBody.type == TCPIP_DHCPV6_IA_TYPE_IANA;


    (void)memcpy(genId.v, TCPIPStack_NetMACAddressGet(pIaidIf), sizeof(TCPIP_MAC_ADDR));

    if(isIana)
    {
        roundCnt = pIaRound->ianaIaidRound;
    }
    else
    {
        roundCnt = pIaRound->iataIaidRound;
    }


    genId.w[3] = ++roundCnt;

    genId.v32[0] ^= genId.v32[1];

    if(isIana)
    {   // NA address space
        genId.v[1] = 0xfd;
        pIaRound->ianaIaidRound = roundCnt;
    }
    else
    {   // TA address space
        genId.v[1] = 0xfe;
        pIaRound->iataIaidRound = roundCnt;
    }

    pIa->iaBody.genId = genId.v32[0]; 
}


// extracts all the options from a set
// pOptionSet - pointer to array of sets: [pOptionSet, pOptionSet + nSets)
// nSets  - the number of sets present in the pOptionSet
// ppCurrSet - address to store the current set:
//      - normally should be set to 0 for the 1st invocation
//      - else valid member in the [pOptionSet, pOptionSet + nSets) array
// pCurrMask - current mask to be processed
//      -  normally should be 0 for the 1st invocation
//      -  else valid value [0, 32) in the current set
//
// returns  - a DHCPV6_OPTION_CODE value extracted from the set
//          - DHCPV6_OPT_CODE_NONE (0) if the sets exhausted 
static uint32_t F_DHCPV6OptionSet_ExtractCode(const uint32_t* pOptionSet, uint32_t nSets, const uint32_t** ppCurrSet, uint32_t* pCurrMask)
{
    const uint32_t* pCurrSet = *ppCurrSet;
    if(pCurrSet == NULL)
    {
        pCurrSet = pOptionSet;
    }

    uint32_t currMask = *pCurrMask;

    for(; pCurrSet != pOptionSet + nSets; pCurrSet++)
    {
        ptrdiff_t currOptBase = FC_CU32PtrDiff(pCurrSet, pOptionSet) * 32;
        uint32_t currSet = *pCurrSet;
        for(; currMask < 32U; currMask++)
        {
            if((currSet & (uint32_t)(1UL << currMask)) != 0UL)
            {   // valid value
                *ppCurrSet = pCurrSet;
                *pCurrMask = currMask + 1UL;
                return (uint32_t)currOptBase + currMask;
            }
        }
        // go to next set
        currMask = 0U;
    }

    // nothing found; end
    return DHCPV6_OPT_CODE_NONE;
}

// return the number of codes that are set in pOptionSet
static uint32_t F_DHCPV6OptionSet_CodesNo(const uint32_t* pOptionSet, uint32_t nSets)
{
    const uint32_t* pCurrSet;
    uint32_t    currMask;
    uint32_t    setIx, currIx;
    uint32_t    nMasks = 0U;

    pCurrSet = pOptionSet;
    for(setIx = 0U; setIx < nSets; setIx++)
    {
        uint32_t currSet = *pCurrSet;
        currMask = 1U;
        for(currIx = 0U; currIx < 32U; currIx++)
        {
            if((currSet & currMask) != 0U)
            {   // valid value
                nMasks++;
            }
            currMask <<= 1U;
        }
        pCurrSet++;
    }

    return nMasks;
}

static bool F_DHCPV6Client_Init(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    uint16_t dcptIx;
    size_t addIx, buffIx;
    TCPIP_DHCPV6_IA_DCPT* pIaDcpt;
    TCPIP_DHCPV6_MSG_BUFFER* pBuffer;

    TCPIP_NET_IF* pNetIf = pClient->pDhcpIf;
    UDP_SOCKET dhcpSkt = F_DHCPV6OpenSocket(pClient);

    if(dhcpSkt == INVALID_SOCKET)
    {
        return false;
    }

    pClient->dhcpSkt = dhcpSkt;

    (void)F_DHCPV6Duid_Generate((TCPIP_DHCPV6_DUID_TYPE)pClient->clientDuid.duidType, &pClient->clientDuid, pNetIf);
    
    // initialize the IAs
    // generate the iaid for IA-NA, IA_TA
    pIaDcpt = pClient->iaArray;
    for(dcptIx = 0; dcptIx < pClient->nIaDcpts; dcptIx++)
    {
        pIaDcpt->pParent = pClient;
        pIaDcpt->parentIx = (int)dcptIx;

        // init the addList
        SINGLE_LIST* pAddList = &pIaDcpt->addList;
        TCPIP_Helper_SingleListInitialize(pAddList);
        TCPIP_DHCPV6_ADDR_NODE* addNode = pIaDcpt->addNodes;
        for(addIx = 0; addIx < sizeof(pIaDcpt->addNodes) / sizeof(*pIaDcpt->addNodes); addIx++)
        {
            addNode->inUse = false;
            TCPIP_Helper_SingleListTailAdd(pAddList, FC_AddNode2SglNode(addNode));
            addNode++;
        }

        if(dcptIx < pClient->nIanaDcpts + pClient->nIataDcpts)
        {   // update the in use IA descriptors
            // set the proper descriptor types
            if(dcptIx < pClient->nIanaDcpts)
            {
                pIaDcpt->iaBody.type = TCPIP_DHCPV6_IA_TYPE_IANA;
            }
            else 
            {
                pIaDcpt->iaBody.type = TCPIP_DHCPV6_IA_TYPE_IATA;
            }

            F_DHCPV6Ia_IdGenerate(pIaDcpt, pNetIf, &pClient->iaIaidRound);
            // mark all in use IAs as starting the solicit
            F_DHCPV6Ia_SetState(pIaDcpt, TCPIP_DHCPV6_IA_STATE_SOLICIT, TCPIP_DHCPV6_IA_SUBSTATE_START);
            TCPIP_Helper_DoubleListTailAdd(pClient->iaStateList + (int)TCPIP_DHCPV6_IA_STATE_SOLICIT, FC_IaDcpt2DblNode(pIaDcpt));
        }
        else
        {   // not used yet
            pIaDcpt->iaBody.type = TCPIP_DHCPV6_IA_TYPE_NONE;
            TCPIP_Helper_DoubleListTailAdd(&pClient->iaFreeList, FC_IaDcpt2DblNode(pIaDcpt));
        }
        pIaDcpt->iaStatusMsgSize = M_DHCPV6_IA_STATUS_USR_MSG_LEN; 
        pIaDcpt++;
    }

    // initialize client message buffers
    pBuffer = pClient->msgBuffers;
    for(buffIx = 0; buffIx < pClient->nMsgBuffers; buffIx++)
    {
        pBuffer->bufferSize = pClient->msgBufferSize;
        pBuffer->pMsgData = pBuffer->msgData;
        TCPIP_Helper_SingleListTailAdd(&pClient->buffFreeList, FC_MsgBuff2SglNode(pBuffer));
        F_DHCPV6TraceBuffInit(buffIx, pBuffer);
        pBuffer = FC_U8Ptr2MsgBuff((uint8_t*)pBuffer + sizeof(TCPIP_DHCPV6_MSG_BUFFER_BARE) + pClient->msgBufferSize);
    }

    pClient->connEvent = TCPIP_MAC_EV_NONE;

    // update the MRT overrides
    pClient->solMaxRt = (dhcpv6MessageBoundsTbl + (int)DHCPV6_CLIENT_MSG_TYPE_SOLICIT)->mrt;
    pClient->infoMaxRt = (dhcpv6MessageBoundsTbl + (int)DHCPV6_CLIENT_MSG_TYPE_INFO_REQUEST)->mrt;

    // additional client initialization here that cannot fail, if needed
    //

    return true;
}

// reinitializes the DHCPv6 client
// initialization was already done
static bool F_DHCPV6Client_Reinit(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    TCPIP_DHCPV6_IA_DCPT* pIaDcpt;

    if(pClient->dhcpSkt == INVALID_SOCKET)
    {
        TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
        return false;
    }

    // clean up any old pending messages
    F_DHCPv6FlushSocket(pClient);

    // clean-up in case we don't get here from TCPIP_DHCPV6_Disable
    F_DHCPV6_LeasesCleanup(pClient);

    // check message buffers
    size_t nFree = TCPIP_Helper_SingleListCount(&pClient->buffFreeList);
    if(nFree !=  pClient->nMsgBuffers)
    {
        TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
        return false;
    }
    
    // re-init the IAs
    // check that all the IAs have been freed
    size_t nFreeIa = TCPIP_Helper_DoubleListCount(&pClient->iaFreeList); 
    if(nFreeIa != pClient->nIaDcpts)
    {
        TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
        return false;
    }

    DOUBLE_LIST newFreeList;
    TCPIP_Helper_DoubleListInitialize(&newFreeList);


    while((pIaDcpt = FC_DblNode2IaDcpt(TCPIP_Helper_DoubleListHeadRemove(&pClient->iaFreeList))) != NULL)
    {
        TCPIPStack_Assert(pIaDcpt->pParent == pClient, __FILE__, __func__, __LINE__);

          TCPIP_DHCPV6_ADDR_NODE* addNode;
          for(addNode = FC_SglNode2AddNode(pIaDcpt->addList.head); addNode != NULL; addNode = addNode->next)
          {
              addNode->inUse = false;
          }

        if(pIaDcpt->iaBody.type == TCPIP_DHCPV6_IA_TYPE_IANA || pIaDcpt->iaBody.type == TCPIP_DHCPV6_IA_TYPE_IATA)
        {
            // mark all in use IAs as starting the solicit
            F_DHCPV6Ia_SetState(pIaDcpt, TCPIP_DHCPV6_IA_STATE_SOLICIT, TCPIP_DHCPV6_IA_SUBSTATE_START);
            TCPIP_Helper_DoubleListTailAdd(pClient->iaStateList + (int)TCPIP_DHCPV6_IA_STATE_SOLICIT, FC_IaDcpt2DblNode(pIaDcpt));
        }
        else
        {   // not used yet
            TCPIP_Helper_DoubleListTailAdd(&newFreeList, FC_IaDcpt2DblNode(pIaDcpt));
        }
    }
    pClient->iaFreeList = newFreeList;

    size_t nSolicits = TCPIP_Helper_DoubleListCount(pClient->iaStateList + (int)TCPIP_DHCPV6_IA_STATE_SOLICIT);
    nFreeIa = TCPIP_Helper_DoubleListCount(&pClient->iaFreeList); 
    if(nSolicits + nFreeIa != pClient->nIaDcpts)
    {
        TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
        return false;
    }
    if(nFreeIa != pClient->nFreeDcpts)
    {
        TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
        return false;
    }

    pClient->connEvent = TCPIP_MAC_EV_NONE;

    return true;
}

// initial set up of a DHCP message;
// sets the message type
// returns:
//       < 0  - error
//      DHCPV6_IA_SSTATE_RES_PENDING - pending operation, retry
//      DHCPV6_IA_SSTATE_RES_OK - success
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT F_DHCPV6Ia_TxMsgSetup(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_CLIENT_MSG_TYPE cliMsgType)
{
    TCPIP_DHCPV6_CLIENT_DCPT* pParent = pIa->pParent;

    if(pIa->msgBuffer == NULL)
    {   // try to get a message buffer for this TX op
        TCPIP_DHCPV6_MSG_BUFFER* pBuffer = FC_SglNode2MsgBuff(TCPIP_Helper_SingleListHeadRemove(&pParent->buffFreeList));

        if(pBuffer == NULL)
        {   // failed
            F_DHCPv6_StatIncrement(pParent, offsetof(TCPIP_DHCPV6_CLIENT_STATISTICS, txBuffFailCnt));
            return DHCPV6_IA_SSTATE_RES_PENDING;
        }
        pBuffer->txOwner = pIa;
        pIa->msgBuffer = pBuffer;
    }
    else
    {   // shouldn't happen ?
        TCPIPStack_Condition(false, __FILE__, __func__, __LINE__);
        TCPIPStack_Assert(pIa->msgBuffer->txOwner == pIa, __FILE__, __func__, __LINE__);
    }

    pIa->msgBuffer->msgLen = 0U;

    F_DHCPV6TraceBuff(pIa, pIa->msgBuffer);

    // common messages initialization
    pIa->cliMsgType = cliMsgType;
    // generate new transaction
    pIa->transactionId.Val = SYS_RANDOM_PseudoGet(); 
    pIa->transactionId.v[3] = 0;    // use the 3 MSB only
    //
    pIa->oroOptMask.pMaskSet3 = &DHCPV6_ORO_OPTION_MASK_TBL; 
    (void)memset(&pIa->msgTxDcpt, 0, sizeof(pIa->msgTxDcpt));
    F_DHCPV6Ia_SetBoundTimes(pIa);

    return DHCPV6_IA_SSTATE_RES_OK;
}


// selects a server based on received advertisements
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT F_DHCPV6Ia_AdvertiseSelect(TCPIP_DHCPV6_IA_DCPT* pIa, bool isTmo)
{
    int16_t maxPref, currPref, serverPref = 0;
    TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer;
    TCPIP_DHCPV6_MSG_BUFFER* pMaxMsg;       // max preference message
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes;
    TCPIP_DHCPV6_SERVER_STATUS_CODE serverStatCode;
    TCPIP_DHCPV6_MSG_BUFFER* pSelMsg = NULL;   // the selected message
    SINGLE_LIST otherIaList = {0};
    SINGLE_LIST thisIaList = {0};
    SINGLE_LIST thisIaPriList = {0};
    SINGLE_LIST discardIaList = {0};


    TCPIP_DHCPV6_CLIENT_DCPT* pParent = pIa->pParent;
    
    if(TCPIP_Helper_SingleListIsEmpty(&pParent->advertiseList))
    {   // nothing to look for
        return DHCPV6_IA_SSTATE_RES_PENDING;
    }

    // check the DHCPV6_OPT_CODE_RAPID_COMMIT option
    // cliMsgType should be DHCPV6_CLIENT_MSG_TYPE_SOLICIT! 
    TCPIP_DHCPV6_OPTION_MASK_SET_0* pMsgMask = DHCPV6_MSG_OPTION_CURR_MASK_TBL + (int)pIa->cliMsgType;
    // when RAPID_COMMIT set, both ADV and REPLY messages need to be processed!
    bool isRapidCommit = (pMsgMask->optionSet[0] & (1UL << DHCPV6_OPT_CODE_RAPID_COMMIT)) != 0UL;

    
    // check the received advertisements

    while((pMsgBuffer = FC_SglNode2MsgBuff(TCPIP_Helper_SingleListHeadRemove(&pParent->advertiseList))) != NULL)
    {
        if(!F_DHCPV6MsgCheck_TransactionId(pIa, pMsgBuffer))
        {   // not this IA
            TCPIP_Helper_SingleListTailAdd(&otherIaList, FC_MsgBuff2SglNode(pMsgBuffer));
            continue;
        }

        // find the IA in the message
        if(F_DHCPV6OptionFind_Ia(pMsgBuffer, pIa, false) == NULL)
        {   // no IA in the message?
            TCPIP_Helper_SingleListTailAdd(&discardIaList, FC_MsgBuff2SglNode(pMsgBuffer));
            continue;
        }


        // belongs to this IA 
        // check the IA for a status code option
        serverStatCode = F_DHCPV6MsgGet_IaOptStatusCode(NULL, pMsgBuffer, pIa, 0, NULL);
        if(serverStatCode != TCPIP_DHCPV6_SERVER_STAT_EXT_ERROR)
        {   // got a server code
            if(serverStatCode == TCPIP_DHCPV6_SERVER_STAT_NO_ADDRS_AVAIL)
            {   // no valid addresses for this IA
                F_DHCPV6Ia_MsgInvalidate(pIa, pMsgBuffer);
                int nIas = F_DHCPV6MsgFind_InUseIAs(pMsgBuffer, pParent, pIa);
                if(nIas == 0)
                {   // nothing else in this message
                    TCPIP_Helper_SingleListTailAdd(&pParent->buffFreeList, FC_MsgBuff2SglNode(pMsgBuffer));
                }
                else
                {
                    TCPIP_Helper_SingleListTailAdd(&otherIaList, FC_MsgBuff2SglNode(pMsgBuffer));
                }
                F_DHCPV6DbgIAIn_PrintFailed("AdSelect", pMsgBuffer, pIa, "IA No Addrs");
                continue;
            }
        }
        
        // process this advertisement
        TCPIP_Helper_SingleListTailAdd(&thisIaList, FC_MsgBuff2SglNode(pMsgBuffer));

        serverPref = (int16_t)F_DHCPV6MsgGet_ServerPref(pMsgBuffer);
        if(serverPref >= TCPIP_DHCPV6_FORCED_SERVER_PREFERENCE)
        {   // found server; copy all the info from this message to the IA
            pSelMsg = pMsgBuffer;
            break;
        }
        else if(isRapidCommit)
        {
            if(F_DHCPV6OptionFind_RapidCommit(pMsgBuffer))
            {
                pSelMsg = pMsgBuffer;
                break;
            }
        }
        else
        {
            // do nothing
        }
    }

    if(pSelMsg == NULL)
    {   // couldn't find any server; check if we must select one!
        if(isTmo || pIa->msgTxDcpt.rc > 1U)
        {   // either tmo or it's a retry; for a retry any advertisement will do!
            pMaxMsg = NULL;   // server with max preference
            maxPref = -1;

            while((pMsgBuffer = FC_SglNode2MsgBuff(TCPIP_Helper_SingleListHeadRemove(&thisIaList))) != NULL)
            {
                currPref = (int16_t)F_DHCPV6MsgGet_ServerPref(pMsgBuffer);
                if(currPref >= maxPref)
                {
                    maxPref = currPref;
                    serverPref = currPref;
                    (void)serverPref;
                    pMaxMsg = pMsgBuffer;
                }
                TCPIP_Helper_SingleListTailAdd(&thisIaPriList, FC_MsgBuff2SglNode(pMsgBuffer));
            }

            if(pMaxMsg != NULL)
            {
                pSelMsg = pMaxMsg;
            }
        }
    }


    if(pSelMsg != NULL)
    {   // found our server
        subRes = F_DHCPV6Ia_AdvertiseCopy(pSelMsg, pIa);
        // done with this IA
        F_DHCPV6Ia_MsgListPurge(&pParent->advertiseList, pIa);  
        F_DHCPV6Ia_MsgListPurge(&thisIaList, pIa);
        F_DHCPV6Ia_MsgListPurge(&thisIaPriList, pIa);
        F_DHCPV6DbgIAIn_PrintPassed("AdSelect - Selected", pSelMsg, pIa, serverPref);
    }
    else
    {
        subRes = DHCPV6_IA_SSTATE_RES_NO_ACTION;
    }

    // re-add the remaining messages
    F_DHCPV6_MsgListForcePurge(pParent, &discardIaList);
    TCPIP_Helper_SingleListAppend(&pParent->advertiseList, &otherIaList);
    TCPIP_Helper_SingleListAppend(&pParent->advertiseList, &thisIaList);
    TCPIP_Helper_SingleListAppend(&pParent->advertiseList, &thisIaPriList);


    if((int)subRes < 0)
    {   // some error
        return subRes;
    }

    return (pSelMsg == NULL) ? DHCPV6_IA_SSTATE_RES_PENDING : DHCPV6_IA_SSTATE_RES_OK;
}


// copy the advertise info from this selected message buffer to the IA
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT F_DHCPV6Ia_AdvertiseCopy(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_IA_DCPT* pIa)
{

    // get the server ID

    if(!F_DHCPV6MsgGet_Duid(&pIa->serverDuid, pMsgBuffer, false))
    {
        return DHCPV6_IA_SSTATE_RES_ERR_TRANS;
    }

    // update the IA params
    if(!F_DHCPV6MsgGet_IaBody(&pIa->iaBody, pMsgBuffer, pIa, true))
    {
        return DHCPV6_IA_SSTATE_RES_ERR_TRANS;
    }


    // update the IA address and lifetimes
    if(!F_DHCPV6MsgGet_IaAddresses(pIa, pMsgBuffer))
    {
        return DHCPV6_IA_SSTATE_RES_ERR_TRANS;
    }

    return DHCPV6_IA_SSTATE_RES_OK;
}

// processes the server reply when in request/renew/decline/release state
// (TCPIP_DHCPV6_IA_STATE_REQUEST/TCPIP_DHCPV6_IA_STATE_RENEW/TCPIP_DHCPV6_IA_STATE_DECLINE/TCPIP_DHCPV6_IA_STATE_RELEASE , TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY)
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT F_DHCPV6Ia_ReplyMsgSrvMatchProcess(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes;
    TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer;
    SINGLE_LIST otherIaList = {0};
    SINGLE_LIST discardIaList = {0};

    TCPIP_DHCPV6_CLIENT_DCPT* pParent = pIa->pParent;

    
    subRes = DHCPV6_IA_SSTATE_RES_PENDING;  // in case we don't find any reply to this IA

    while((pMsgBuffer = FC_SglNode2MsgBuff(TCPIP_Helper_SingleListHeadRemove(&pParent->replyList))) != NULL)
    {
        // check is this IA id
        if(!F_DHCPV6MsgCheck_TransactionId(pIa, pMsgBuffer))
        {   // not ours
            TCPIP_Helper_SingleListTailAdd(&otherIaList, FC_MsgBuff2SglNode(pMsgBuffer));
            continue;
        }

        subRes = F_DHCPV6Ia_ProcessSrvMatchMsg(pIa, pMsgBuffer);

        // done with this message
        TCPIP_Helper_SingleListTailAdd(&discardIaList, FC_MsgBuff2SglNode(pMsgBuffer));

        if(subRes != DHCPV6_IA_SSTATE_RES_PENDING)
        {   // done with this IA
            break;
        }
    }

    if(subRes != DHCPV6_IA_SSTATE_RES_PENDING)
    {   // done with this IA
        F_DHCPV6Ia_MsgListPurge(&pParent->replyList, pIa);  
    }
    F_DHCPV6_MsgListForcePurge(pParent, &discardIaList);

    // re-add the remaining messages
    TCPIP_Helper_SingleListAppend(&pParent->replyList, &otherIaList);

    return subRes;
    
}

// processes the message received from the server for this IA
// correct transaction ID: message belongs to this IA!
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT F_DHCPV6Ia_ProcessSrvMatchMsg(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer)
{
    // find the IA in the message
    void*   pOptIa = F_DHCPV6OptionFind_Ia(pMsgBuffer, pIa, true);
    if(pOptIa == NULL)
    {   // could not find the IA?
        return DHCPV6_IA_SSTATE_RES_PENDING;
    }

    // special case for RELEASE or DECLINE: any reply is fine!
    if(pIa->iaState == (uint16_t)TCPIP_DHCPV6_IA_STATE_RELEASE || pIa->iaState == (uint16_t)TCPIP_DHCPV6_IA_STATE_DECLINE)
    {   // any reply is just fine
        F_DHCPV6Ia_Remove(pIa);
        F_DHCPV6DbgIAIn_PrintPassed("Reply Rel/Decl", pMsgBuffer, pIa, 0);
        return DHCPV6_IA_SSTATE_RES_NO_ACTION;
    }

    // check the server status code
    TCPIP_DHCPV6_SERVER_STATUS_CODE serverStatCode = F_DHCPV6MsgGet_StatusCode(NULL, pMsgBuffer, pIa, 0, NULL);
    TCPIP_DHCPV6_CLIENT_DCPT* pParent = pIa->pParent;

    if(serverStatCode != TCPIP_DHCPV6_SERVER_STAT_EXT_ERROR)
    {   // found
        pParent->cliLastStatusCode = (uint16_t)serverStatCode;
    }
    else
    {   // check the IA
        serverStatCode = F_DHCPV6MsgGet_IaOptStatusCode(pIa->iaLastStatusMsg, pMsgBuffer, pIa, pIa->iaStatusMsgSize, NULL);
        if(serverStatCode != TCPIP_DHCPV6_SERVER_STAT_EXT_ERROR)
        {
            pIa->iaLastStatusCode = (uint16_t)serverStatCode;
        }
    }

    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes = DHCPV6_IA_SSTATE_RES_NO_ACTION;
    bool checkMsg, okMsg;
    F_DHCPV6DbgDeclare_IAFailReason(failReason);

    if(serverStatCode == TCPIP_DHCPV6_SERVER_STAT_EXT_ERROR)
    {   // if no status code then process the message
        checkMsg = false;
        okMsg = true;
    }
    else
    {   // we have a status code that needs analysis
        checkMsg = true;
        okMsg = false;
    }

    while(checkMsg) 
    {   // check the status code
        if(serverStatCode == TCPIP_DHCPV6_SERVER_STAT_UNSPEC_FAIL)
        {
            F_DHCPV6DbgSet_IAFailReason(failReason, "Unspec Fail");
            subRes = DHCPV6_IA_SSTATE_RES_RETRANSMIT;
            break;
        }

        if(serverStatCode == TCPIP_DHCPV6_SERVER_STAT_USE_MULTICAST)
        {
            pIa->flags.iaUnicast = 0U;
            F_DHCPV6DbgSet_IAFailReason(failReason, "Unicast Fail");
            subRes = DHCPV6_IA_SSTATE_RES_RETRANSMIT;
            break;
        }

        if(serverStatCode == TCPIP_DHCPV6_SERVER_STAT_NO_ADDRS_AVAIL)
        {
            F_DHCPV6DbgSet_IAFailReason(failReason, "No IA Addrs");
            subRes = DHCPV6_IA_SSTATE_RES_RUN_RESTART;
            break;
        }

        if(serverStatCode == TCPIP_DHCPV6_SERVER_STAT_NOT_ON_LINK && pIa->iaState == (uint16_t)TCPIP_DHCPV6_IA_STATE_REQUEST)
        {
            F_DHCPV6DbgSet_IAFailReason(failReason, "Not on Link");
            subRes = DHCPV6_IA_SSTATE_RES_RUN_RESTART;
            break;
        }

        if(serverStatCode == TCPIP_DHCPV6_SERVER_STAT_NO_BINDING && pIa->iaState == (uint16_t)TCPIP_DHCPV6_IA_STATE_RENEW)
        {
            F_DHCPV6Ia_SetRunState(pIa, TCPIP_DHCPV6_IA_STATE_REQUEST, TCPIP_DHCPV6_IA_SUBSTATE_START);
            subRes = DHCPV6_IA_SSTATE_RES_RUN_JUMP;
            F_DHCPV6DbgSet_IAFailReason(failReason, "No Binding");
            break;
        }

        // we've made it so far, we should be good
        okMsg = true;
        break;
    }

    if(okMsg)
    {
        if(!F_DHCPV6MsgGet_LeaseParams(pIa, pMsgBuffer, true))
        {   // failed
            subRes = DHCPV6_IA_SSTATE_RES_ERR_TRANS;
            okMsg = false;
            F_DHCPV6DbgSet_IAFailReason(failReason, "No Lease Params");
        }
        else
        {
            F_DHCPV6Ia_SetRunState(pIa, TCPIP_DHCPV6_IA_STATE_DAD, TCPIP_DHCPV6_IA_SUBSTATE_START);
            subRes = DHCPV6_IA_SSTATE_RES_RUN_JUMP;
            // acquired the lease, we're done
        }
    }

    // print the process result
    if(okMsg)
    {
        F_DHCPV6DbgIAIn_PrintPassed("Reply Match", pMsgBuffer, pIa, serverStatCode);
    }
    else
    {
        F_DHCPV6DbgIAIn_PrintFailed("Reply Match", pMsgBuffer, pIa, failReason);
    }

    return subRes;
}

static bool F_DHCPV6MsgGet_LeaseParams(TCPIP_DHCPV6_IA_DCPT* pDstIa, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, bool serverMatch)
{
    // get the IA and address 

    if(!F_DHCPV6MsgGet_IaBody(&pDstIa->iaBody, pSrcBuffer, pDstIa, serverMatch))
    {
        return false;
    }

    // get all the addresses that apply to this IA
    if(!F_DHCPV6MsgGet_IaAddresses(pDstIa, pSrcBuffer))
    {
        return false;
    }
    
    // check server unicast option!
    if((F_DHCPV6OptionFind_ServerUnicast(pSrcBuffer, &pDstIa->serverUcastAddr.v6Add)))
    {
        pDstIa->flags.iaUnicast = 1U;
    }

    F_DHCPV6Ia_SetTimeValues(pDstIa, true);


    return true;
}


// processes the server reply when in confirm state
// (TCPIP_DHCPV6_IA_STATE_CONFIRM , TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY)
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT F_DHCPV6Ia_ReplyConfirmProcess(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes;
    TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer;
    TCPIP_DHCPV6_SERVER_STATUS_CODE serverStatCode;
    bool stopProcess;
    int  nProcMsgs;
    SINGLE_LIST otherIaList = {0};
    SINGLE_LIST discardIaList = {0};

    TCPIP_DHCPV6_CLIENT_DCPT* pParent = pIa->pParent;


    stopProcess = false;
    nProcMsgs = 0;
    
    subRes = DHCPV6_IA_SSTATE_RES_PENDING;  // in case we don't find any reply to this IA

    while((pMsgBuffer = FC_SglNode2MsgBuff(TCPIP_Helper_SingleListHeadRemove(&pParent->replyList))) != NULL)
    {
        // check this message concerns this IA
        if(!F_DHCPV6MsgCheck_TransactionId(pIa, pMsgBuffer))
        {   // not this IA
            TCPIP_Helper_SingleListTailAdd(&otherIaList, FC_MsgBuff2SglNode(pMsgBuffer));
            continue;
        }

        // message belonging to this IA 
        nProcMsgs++;
        serverStatCode = F_DHCPV6MsgGet_StatusCode(NULL, pMsgBuffer, pIa, 0, NULL);
        if(serverStatCode != TCPIP_DHCPV6_SERVER_STAT_EXT_ERROR)
        {
            pParent->cliLastStatusCode = (uint16_t)serverStatCode;
        }
        else
        {   // no status code; check the IA
            serverStatCode = F_DHCPV6MsgGet_IaOptStatusCode(pIa->iaLastStatusMsg, pMsgBuffer, pIa, pIa->iaStatusMsgSize, NULL);
            if(serverStatCode != TCPIP_DHCPV6_SERVER_STAT_EXT_ERROR)
            {
                pIa->iaLastStatusCode = (uint16_t)serverStatCode;
            }
        }

        if(serverStatCode == TCPIP_DHCPV6_SERVER_STAT_SUCCESS)
        {   // addresses still valid
            // get this server ID
            if(!F_DHCPV6MsgGet_Duid(&pIa->serverDuid, pMsgBuffer, false))
            {
                subRes = DHCPV6_IA_SSTATE_RES_ERR_TRANS;
            }
            else
            {
                F_DHCPV6Ia_RestoreTimeValues(pIa);
                F_DHCPV6Ia_SetRunState(pIa, TCPIP_DHCPV6_IA_STATE_BOUND, TCPIP_DHCPV6_IA_SUBSTATE_START);
                subRes = DHCPV6_IA_SSTATE_RES_RUN_JUMP;
            }
            stopProcess = true;
        }
        else if(serverStatCode == TCPIP_DHCPV6_SERVER_STAT_NOT_ON_LINK)
        {   // link address has changed...restart discovery
            F_DHCPV6Ia_AddressRemove(pIa);
            subRes = DHCPV6_IA_SSTATE_RES_RUN_RESTART;
        }
        else
        {   // some other issue   
            subRes = DHCPV6_IA_SSTATE_RES_RETRANSMIT;
        }

        // done with this message
        TCPIP_Helper_SingleListTailAdd(&discardIaList, FC_MsgBuff2SglNode(pMsgBuffer));

        if(stopProcess)
        {   // got what we wanted; there shouldn't be other messages of interest here
            break;
        }
    }

    if(nProcMsgs != 0)
    {   // done with this IA
        F_DHCPV6Ia_MsgListPurge(&pParent->replyList, pIa);  
    }
    F_DHCPV6_MsgListForcePurge(pParent, &discardIaList);

    // re-add the remaining messages
    TCPIP_Helper_SingleListAppend(&pParent->replyList, &otherIaList);

    return subRes;
    
}

// processes the server reply when in rebind state
// (TCPIP_DHCPV6_IA_STATE_REBIND , TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY)
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT F_DHCPV6Ia_ReplyRebindProcess(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes;
    TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer;
    TCPIP_DHCPV6_SERVER_STATUS_CODE serverStatCode;
    TCPIP_DHCPV6_OPTION_IA_ADDR* pIaAddr;
    TCPIP_DHCPV6_OPTION_IA_ADDR_BODY addBody;
    TCPIP_DHCPV6_MSG_SEARCH_DCPT srchDcpt;
    bool procMessage, stopProcess;
    int  nProcMsgs;
    void*   pOptIa;
    SINGLE_LIST otherIaList = {0};
    SINGLE_LIST discardIaList = {0};

    TCPIP_DHCPV6_CLIENT_DCPT* pParent = pIa->pParent;


    stopProcess = false;
    nProcMsgs = 0;
    
    subRes = DHCPV6_IA_SSTATE_RES_PENDING;  // in case we don't find any reply to this IA

    while((pMsgBuffer = FC_SglNode2MsgBuff(TCPIP_Helper_SingleListHeadRemove(&pParent->replyList))) != NULL)
    {
        if(!F_DHCPV6MsgCheck_TransactionId(pIa, pMsgBuffer))
        {   // not this IA
            TCPIP_Helper_SingleListTailAdd(&otherIaList, FC_MsgBuff2SglNode(pMsgBuffer));
            continue;
        }

        // correct transaction ID: message belonging to this IA 
        // find the IA in the message
        if((pOptIa = F_DHCPV6OptionFind_Ia(pMsgBuffer, pIa, false)) != NULL)
        {   // message belonging to this IA 
            nProcMsgs++;
            procMessage = true;

            if((serverStatCode = F_DHCPV6MsgGet_StatusCode(NULL, pMsgBuffer, pIa, 0, NULL)) != TCPIP_DHCPV6_SERVER_STAT_EXT_ERROR)
            {   // check if we have a server status code
                pParent->cliLastStatusCode = (uint16_t)serverStatCode;
                if(serverStatCode != TCPIP_DHCPV6_SERVER_STAT_SUCCESS)
                {
                    subRes = DHCPV6_IA_SSTATE_RES_RUN_RESTART;
                    procMessage = false;
                }
            }
        }
        else
        {   // not our IA ? 
            procMessage = false;
        }

        while(procMessage)
        {
            // check the IA for a status code option
            serverStatCode = F_DHCPV6MsgGet_IaOptStatusCode(pIa->iaLastStatusMsg, pMsgBuffer, pIa, pIa->iaStatusMsgSize, NULL);
            if(serverStatCode != TCPIP_DHCPV6_SERVER_STAT_EXT_ERROR)
            {
                pIa->iaLastStatusCode = (uint16_t)serverStatCode;
                if(serverStatCode == TCPIP_DHCPV6_SERVER_STAT_NO_BINDING)
                {
                    F_DHCPV6Ia_SetRunState(pIa, TCPIP_DHCPV6_IA_STATE_REQUEST, TCPIP_DHCPV6_IA_SUBSTATE_START);
                    subRes = DHCPV6_IA_SSTATE_RES_RUN_JUMP;
                    break;
                }
            }

            // get this server ID
            if(!F_DHCPV6MsgGet_Duid(&pIa->serverDuid, pMsgBuffer, false))
            {
                subRes = DHCPV6_IA_SSTATE_RES_ERR_TRANS;
                break;
            }

            // find option within IANA/IATA
            TCPIP_DHCPV6_MSG_BUFFER optMsgBuffer;
            if(!F_DHCPV6MsgGet_IaOptBuffer(&optMsgBuffer, pMsgBuffer, pIa))
            {   // should not happen; ignore this message
                TCPIPStack_Condition(false, __FILE__, __func__, __LINE__);
                break;
            }

            srchDcpt.pOptBuff = optMsgBuffer.pOptData;
            srchDcpt.optBuffLen = (int16_t)optMsgBuffer.optLen;
            srchDcpt.matchFnc = NULL;

            pIaAddr = F_DHCPV6OptionFind_OptCode(&srchDcpt, DHCPV6_OPT_CODE_IA_ADDR).pIaAddr;
            if(pIaAddr == NULL)
            {   // should not happen; ignore this message
                TCPIPStack_Condition(false, __FILE__, __func__, __LINE__);
                break;
            }

            stopProcess = true;
            if(!F_DHCPV6OptionGet_IaAddress(&addBody, pIaAddr))
            {   // rebind with life times == 0! explicit message that we have to start all over
                subRes = DHCPV6_IA_SSTATE_RES_RUN_RESTART;
                break;
            }

            if(!F_DHCPV6MsgGet_LeaseParams(pIa, pMsgBuffer, false))
            {  
                subRes = DHCPV6_IA_SSTATE_RES_ERR_TRANS;
            }
            else
            {
                F_DHCPV6Ia_SetRunState(pIa, TCPIP_DHCPV6_IA_STATE_DAD, TCPIP_DHCPV6_IA_SUBSTATE_START);
                subRes = DHCPV6_IA_SSTATE_RES_RUN_JUMP;
            }

            break;
        }

        // done with this message
        TCPIP_Helper_SingleListTailAdd(&discardIaList, FC_MsgBuff2SglNode(pMsgBuffer));

        if(stopProcess)
        {   // acquired the lease; there shouldn't be other messages of interest here
            break;
        }
    }

    if(nProcMsgs != 0)
    {   // done with this IA
        F_DHCPV6Ia_MsgListPurge(&pParent->replyList, pIa);  
    }
    F_DHCPV6_MsgListForcePurge(pParent, &discardIaList);

    // re-add the remaining messages
    TCPIP_Helper_SingleListAppend(&pParent->replyList, &otherIaList);

    return subRes;
    
}
// gets the IA address from a IA Address Option
static bool F_DHCPV6OptionGet_IaAddress(TCPIP_DHCPV6_OPTION_IA_ADDR_BODY* pAddBody, TCPIP_DHCPV6_OPTION_IA_ADDR* pSrcIaAddr)
{
    *pAddBody = pSrcIaAddr->body;
    pAddBody->prefLTime = TCPIP_Helper_ntohl(pAddBody->prefLTime);
    pAddBody->validLTime = TCPIP_Helper_ntohl(pAddBody->validLTime);

    // discard addresses which have a invalid lifetimes
    return (pAddBody->validLTime != 0U && pAddBody->prefLTime != 0U && pAddBody->prefLTime <= pAddBody->validLTime);
} 

// gets the IA body from a IA Option
static bool F_DHCPV6OptionGet_IaBody(TCPIP_DHCPV6_IA_BODY* pIaBody, void* pSrcOptIa, TCPIP_DHCPV6_IA_TYPE type)
{
    if(type == TCPIP_DHCPV6_IA_TYPE_IANA)
    {
        uint32_t    t1, t2;
        TCPIP_DHCPV6_OPTION_IANA* pOptIana = FC_VPtr2OptIana(pSrcOptIa);
        pIaBody->ianaBody.iaid = TCPIP_Helper_ntohl(pOptIana->body.iaid);

        t1 = TCPIP_Helper_ntohl(pOptIana->body.t1);
        t2 = TCPIP_Helper_ntohl(pOptIana->body.t2);
        // mimimal sanity check
        if(t1 > 0U && t2 > 0U)
        {
            if(t1 > t2)
            {   // RFC 3315 pg 74/101: invalid, discard the IANA option
                return false;
            }
        }

        pIaBody->ianaBody.t1 = t1;
        pIaBody->ianaBody.t2 = t2;
    }
    else
    {
        TCPIP_DHCPV6_OPTION_IATA* pOptIata = FC_VPtr2OptIata(pSrcOptIa);
        pIaBody->iataBody.iaid = pOptIata->body.iaid;
    }

    return true;
}

// sets the IA default times
static void F_DHCPV6Ia_SetTimeValues(TCPIP_DHCPV6_IA_DCPT* pIa, bool setAcqTime)
{
    if(setAcqTime)
    {
        pIa->iaBody.tAcquire = TCPIP_SecCountGet(); 
    }
    
    if(pIa->iaBody.type == TCPIP_DHCPV6_IA_TYPE_IANA)
    {
        if(pIa->iaBody.ianaBody.t1 == 0U)
        {   // use the default value
            pIa->iaBody.ianaBody.t1 = pIa->pParent->defaultIanaT1;
        }
        if(pIa->iaBody.ianaBody.t2 == 0U)
        {   // use the default value
            pIa->iaBody.ianaBody.t2 = pIa->pParent->defaultIanaT2;
        }

        pIa->lastT1 = pIa->iaBody.ianaBody.t1;
        pIa->lastT2 = pIa->iaBody.ianaBody.t2;
    }
    // else IATA uses global values

}

static void F_DHCPV6Ia_RestoreTimeValues(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    pIa->iaBody.ianaBody.t1 = pIa->lastT1;
    pIa->iaBody.ianaBody.t2 = pIa->lastT2;

    TCPIP_DHCPV6_ADDR_NODE* addNode;
    for(addNode = FC_SglNode2AddNode(pIa->addList.head); addNode != NULL; addNode = addNode->next)
    {
        if(!addNode->inUse)
        {
            continue;
        }

        addNode->addBody.prefLTime = addNode->lastPrefLTime;
        addNode->addBody.validLTime = addNode->lastValidLTime;
    }

}



static void F_DHCPV6Ia_SetBoundTimes(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_ADDR_NODE* addNode;
    uint32_t tMrdExpireSec = 0U;

    TCPIP_DHCPV6_CLIENT_MSG_TYPE cliMsgType = pIa->cliMsgType;

    pIa->msgTxDcpt.bounds = dhcpv6MessageBoundsTbl[cliMsgType];
    // use the MRT overrides, if needed
    if(cliMsgType == DHCPV6_CLIENT_MSG_TYPE_SOLICIT)
    {
        pIa->msgTxDcpt.bounds.mrt = pIa->pParent->solMaxRt;
    }
    else if(cliMsgType == DHCPV6_CLIENT_MSG_TYPE_INFO_REQUEST)
    {
        pIa->msgTxDcpt.bounds.mrt = pIa->pParent->infoMaxRt;
    }
    else
    {
        // do nothing
    }

    if(pIa->iaState == (uint16_t)TCPIP_DHCPV6_IA_STATE_RENEW)
    {   // update MRD to the time until T2
        if(pIa->iaBody.type == TCPIP_DHCPV6_IA_TYPE_IANA)
        {
            tMrdExpireSec = pIa->iaBody.ianaBody.t2;
        }
        else
        {   // IATA use global values
            tMrdExpireSec = pIa->pParent->defaultIataT2;
        }

        if(((uint16_t)pIa->pParent->configFlags & (uint16_t)TCPIP_DHCPV6_FLAG_IA_IGNORE_REBIND_LTIME) == 0U)
        {   // select minimum
            for(addNode = FC_SglNode2AddNode(pIa->addList.head); addNode != NULL; addNode = addNode->next)
            {
                if(!addNode->inUse)
                {
                    continue;
                }

                tMrdExpireSec = (tMrdExpireSec > addNode->addBody.validLTime) ? addNode->addBody.validLTime : tMrdExpireSec;
            }
        }

        if(tMrdExpireSec == TCPIP_DHCPV6_TIMEOUT_INFINITE)
        {
            tMrdExpireSec = 0U;
        }
    }
    else if(pIa->iaState == (uint16_t)TCPIP_DHCPV6_IA_STATE_REBIND)
    {
        for(addNode = FC_SglNode2AddNode(pIa->addList.head); addNode != NULL; addNode = addNode->next)
        {
            if(!addNode->inUse)
            {
                continue;
            }

            if(addNode->addBody.validLTime != TCPIP_DHCPV6_TIMEOUT_INFINITE)
            {
                if(tMrdExpireSec == 0U || tMrdExpireSec > addNode->addBody.validLTime)
                {   // select minimum
                    tMrdExpireSec =  addNode->addBody.validLTime;
                }
            }
        }
    }
    else
    {
        // do nothing
    }

    if(tMrdExpireSec != 0U)
    {
        uint32_t secCurr = TCPIP_SecCountGet();
        uint32_t tMrdDeadLine = pIa->iaBody.tAcquire + tMrdExpireSec;

        TCPIPStack_Condition(((int32_t)secCurr - (int32_t)tMrdDeadLine) <= 0, __FILE__, __func__, __LINE__);
        pIa->msgTxDcpt.bounds.mrd = tMrdDeadLine - secCurr;
    }
}

static bool F_DHCPV6Ia_AddressIsExpired(TCPIP_DHCPV6_IA_DCPT* pIa, bool checkPrefLTime)
{   
    
    TCPIP_DHCPV6_ADDR_NODE* addNode;
    for(addNode = FC_SglNode2AddNode(pIa->addList.head); addNode != NULL; addNode = addNode->next)
    {
        if(!addNode->inUse)
        {
            continue;
        }

        uint32_t lTime = checkPrefLTime ? addNode->addBody.prefLTime : addNode->addBody.validLTime; 


        if(lTime != TCPIP_DHCPV6_TIMEOUT_INFINITE)
        {
            uint32_t tMrdDeadLine = pIa->iaBody.tAcquire + lTime;
            if(((int32_t)TCPIP_SecCountGet() - (int32_t)tMrdDeadLine) >= 0)
            {
                return true;
            }
        }
    }

    return false;
}

static void F_DHCPV6Ia_Remove(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_CLIENT_DCPT* pClient = pIa->pParent;
    // remove it from the iaStateList
    TCPIP_Helper_DoubleListNodeRemove(pClient->iaStateList + pIa->iaState, FC_IaDcpt2DblNode(pIa));
    
    // discard any pending pIaDcpt->msgBuffer
    F_DHCPV6Ia_ReleaseMsgBuffer(pIa);
    // and add it to the free list
    TCPIP_Helper_DoubleListTailAdd(&pClient->iaFreeList, FC_IaDcpt2DblNode(pIa));
}

static void F_DHCPV6Ia_ReleaseMsgBuffer(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_MSG_BUFFER* pTxBuffer = pIa->msgBuffer;
    if(pTxBuffer != NULL)
    {   //  only states having a transmit sub-state should have a buffer
        TCPIP_DHCPV6_CLIENT_DCPT* pClient = pIa->pParent;
        TCPIP_Helper_SingleListTailAdd(&pClient->buffFreeList, FC_MsgBuff2SglNode(pTxBuffer));
        pIa->msgBuffer = NULL; // a new buffer get when new status involving transmission begins
    }
}

// gets the DUID from a Server/Client ID Option
static bool F_DHCPV6OptionGet_Duid(TCPIP_DHCPV6_DUID_DCPT* pDstDuidDcpt, TCPIP_DHCPV6_OPTION_ID* pSrcOptId)
{
    uint16_t duidLen;
    TCPIP_DHCPV6_DUID_TYPE duidType;

    TCPIP_DHCPV6_OPTION_ID_TYPE* pSrcOptIdType = FC_OptId2OptType(pSrcOptId);
    duidType = (TCPIP_DHCPV6_DUID_TYPE)(int)TCPIP_Helper_ntohs(pSrcOptIdType->duid[0]);
    duidLen = TCPIP_Helper_ntohs(pSrcOptIdType->optLen);
    // the TCPIP_DHCPV6_DUID_EN has variable size
    // make sure we have enough space for it
    if((duidType == TCPIP_DHCPV6_DUID_TYPE_EN && duidLen > sizeof(TCPIP_DHCPV6_DUID_EN)) || 
       (duidType == TCPIP_DHCPV6_DUID_TYPE_LLT && duidLen > sizeof(TCPIP_DHCPV6_DUID_LLT)) ||
       (duidType == TCPIP_DHCPV6_DUID_TYPE_LL && duidLen > sizeof(TCPIP_DHCPV6_DUID_LL)))
    {
        TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
        return false;
    }

    (void)memcpy(pDstDuidDcpt->duidBody.v16, pSrcOptIdType->duid, duidLen);
    pDstDuidDcpt->duidType = (uint16_t)duidType;
    pDstDuidDcpt->duidLen = duidLen;

    return true;

}


static bool F_DHCPV6MsgGet_Duid(TCPIP_DHCPV6_DUID_DCPT* pDstDuid, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, bool isClient)
{
    // this should be valid; already checked in Receive/Process task!
    uint32_t optCode = isClient ? DHCPV6_OPT_CODE_CLIENT_ID : DHCPV6_OPT_CODE_SERVER_ID;

    TCPIP_DHCPV6_MSG_SEARCH_DCPT srchDcpt;

    srchDcpt.pOptBuff = pSrcBuffer->pOptData;
    srchDcpt.optBuffLen = (int16_t)pSrcBuffer->optLen;
    srchDcpt.matchFnc = NULL;
    TCPIP_DHCPV6_OPTION_ID* pOptId = F_DHCPV6OptionFind_OptCode(&srchDcpt, optCode).pOptId;

    bool dhcpCond;
    if(pOptId == NULL)
    {
        dhcpCond = true;
    }
    else if(!F_DHCPV6OptionGet_Duid(pDstDuid, pOptId))
    {
        dhcpCond = true;
    }
    else
    {
        dhcpCond = false;
    }

    if(dhcpCond)
    {
        TCPIPStack_Condition(false, __FILE__, __func__, __LINE__);
        return false;
    }

    return true;
}


static bool F_DHCPV6MsgGet_IaBody(TCPIP_DHCPV6_IA_BODY* pIaBody, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, TCPIP_DHCPV6_IA_DCPT* pIa, bool serverMatch)
{
    // get the updated T1, T2

    void* pOptIa = F_DHCPV6OptionFind_Ia(pSrcBuffer, pIa, serverMatch);
    if(pOptIa == NULL)
    {
        return false;
    }
    else if(!F_DHCPV6OptionGet_IaBody(pIaBody, pOptIa, pIa->iaBody.type))
    {
        return false;
    }
    else
    {
        return true;
    }
}

// searches for an IA option (DHCPV6_OPT_CODE_IA_ADDR == 5) 
// and updates all the addresses and the lifetimes, if found
// returns:     true if everything OK
//             false invalid lifetimes 
static bool F_DHCPV6MsgGet_IaAddresses(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer)
{
    // get all the addresses that apply to this IA
    // iterate for DHCPV6_OPT_CODE_IA_ADDR!
    TCPIP_DHCPV6_OPTION_IA_ADDR_BODY addBody;
    TCPIP_DHCPV6_OPTION_IA_ADDR* pOptAdd;
    TCPIP_DHCPV6_MSG_SEARCH_DCPT srchDcpt;
    TCPIP_DHCPV6_MSG_BUFFER optMsgBuffer;

    // point to the beginning of the IA options
    if(F_DHCPV6MsgGet_IaOptBuffer(&optMsgBuffer, pSrcBuffer, pIa))
    {
        srchDcpt.pOptBuff = optMsgBuffer.pOptData;
        srchDcpt.optBuffLen = (int16_t)optMsgBuffer.optLen;
        srchDcpt.matchFnc = NULL;

        while((pOptAdd = F_DHCPV6OptionFind_OptCode(&srchDcpt, DHCPV6_OPT_CODE_IA_ADDR).pIaAddr) != NULL)
        {   // found an address
            if(F_DHCPV6OptionGet_IaAddress(&addBody, pOptAdd))
            {   // valid
                // add this address to the IA
                TCPIP_DHCPV6_ADDR_NODE* addNode, *emptyNode, *foundNode;
                emptyNode = foundNode = NULL;
                for(addNode = FC_SglNode2AddNode(pIa->addList.head); addNode != NULL; addNode = addNode->next)
                {
                    if(!addNode->inUse && emptyNode == NULL)
                    {
                        emptyNode = addNode;
                    }
                    else
                    {   // used node; check if we have this address and need to update
                        if(memcmp(addBody.ipv6Addr.v, addNode->addBody.ipv6Addr.v, sizeof(addBody.ipv6Addr.v)) == 0)
                        {   // we have this add; update
                            foundNode = addNode;
                            break;
                        }
                    }
                }

                if(foundNode == NULL)
                {   // some new address
                    foundNode = emptyNode;
                }

                if(foundNode != NULL)
                {   // copy the valid address
                    foundNode->addBody = addBody;    // copy the valid address
                    // save the last known life times
                    foundNode->lastPrefLTime = foundNode->addBody.prefLTime;
                    foundNode->lastValidLTime = foundNode->addBody.validLTime;
                    // all good
                    foundNode->inUse = true;
                } 
                else
                {   // we should have room to add the new addresses
                    TCPIPStack_Condition(false, __FILE__, __func__, __LINE__);
                }
            }
            else
            {   // invalid IA address?
                return false;
            }
        }
    }

    return true;
}


// client tasks
// 

// transmit procedure
// transmits messages in the pClient->txMsgList
// sets pClient->state for error
static void F_DHCPV6Client_TransmitTask(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    TCPIP_DHCPV6_MSG_BUFFER* pTxBuffer;
    TCPIP_DHCPV6_IA_DCPT*    pIa;
    UDP_SOCKET s = pClient->dhcpSkt;
    int     nTxPkts = 0;
    

    while(!TCPIP_Helper_SingleListIsEmpty(&pClient->txMsgList))
    {   // there's something to transmit
        uint16_t nBytes1, nBytes2;

        nBytes1 = TCPIP_UDP_PutIsReady(s);
        if(nBytes1 == 0U)
        {   // out of buffer space
            TCPIPStack_Condition(nTxPkts != 0, __FILE__, __func__, __LINE__);
            break;  // abort
        }

        // get message to transmit
        pTxBuffer = FC_SglNode2MsgBuff(TCPIP_Helper_SingleListHeadRemove(&pClient->txMsgList));
        pIa = pTxBuffer->txOwner;
        TCPIPStack_Assert(pClient == pIa->pParent, __FILE__, __func__, __LINE__);
        // discard whatever is in the socket
        (void)TCPIP_UDP_TxOffsetSet(s, 0, false);
        // make sure there's enough room
        if(nBytes1 < pTxBuffer->msgLen)
        {   // shouldn't happen
            F_DHCPv6_StatIncrement(pClient, offsetof(TCPIP_DHCPV6_CLIENT_STATISTICS, txSpaceFailCnt));
            F_DHCPV6DbgMsgOut_PrintFailed(pTxBuffer, (FC_U8Ptr2MsgHdr(pTxBuffer->pMsgData))->msg_type, pIa, "Small TX Buffer");
            TCPIPStack_Condition(false, __FILE__, __func__, __LINE__);
        }
        else
        {   // write the DHCP message
            if(pIa->flags.iaUnicast != 0U)
            {
                (void)TCPIP_UDP_DestinationIPAddressSet(s, IP_ADDRESS_TYPE_IPV6, &pIa->serverUcastAddr);
            }
            else
            {
                (void)TCPIP_UDP_DestinationIPAddressSet(s, IP_ADDRESS_TYPE_IPV6, &DHCPV6_Servers_Address);
            }
            nBytes1 = TCPIP_UDP_ArrayPut(s, pTxBuffer->pMsgData, pTxBuffer->msgLen);
            nBytes2 = TCPIP_UDP_Flush(s);

            // Set the Strict PORT option to false and user can receive the 
            // packet from different port number other than 547.

            if(nBytes1 != nBytes2 || nBytes1 != pTxBuffer->msgLen)
            {   // should not happen
                TCPIPStack_Condition(false, __FILE__, __func__, __LINE__);
                F_DHCPv6_StatIncrement(pClient, offsetof(TCPIP_DHCPV6_CLIENT_STATISTICS, txSktFlushFailCnt));
                F_DHCPV6DbgMsgOut_PrintFailed(pTxBuffer, (FC_U8Ptr2MsgHdr(pTxBuffer->pMsgData))->msg_type, pIa, "Flush Fail");
            }
            else
            {
                F_DHCPV6DbgMsgOut_PrintPassed(pTxBuffer, (FC_U8Ptr2MsgHdr(pTxBuffer->pMsgData))->msg_type , pIa);
            }
            nTxPkts++;
        }

        // done with this buffer
        // but it still belongs to the txOwner IA
    }
}

// receive procedure
// receives valid DHCP messages and pushes them in the RX queue: pClient->rxMsgList
// sets pClient->state for error
static void F_DHCPV6Client_ReceiveTask(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    if(pClient->flags.rxDisabled != 0U)
    {   // simply discard messages
        F_DHCPv6FlushSocket(pClient);
        return;
    }

    // else process packets
    uint16_t pktSize;
    UDP_SOCKET dhcpSkt = pClient->dhcpSkt;

    TCPIP_DHCPV6_MESSAGE_HEADER* pHdr;
    bool      rxAbort = false;
    TCPIP_DHCPV6_MSG_BUFFER* pRxBuffer = NULL;

    while(rxAbort == false)
    {
        pktSize = TCPIP_UDP_GetIsReady(dhcpSkt);
        if(pktSize == 0U)
        {   // no UDP message pending
            break;
        }
        while(true)
        {
            // try to get a new message buffer for this RX op
            if(pRxBuffer == NULL)
            {
                pRxBuffer = FC_SglNode2MsgBuff(TCPIP_Helper_SingleListHeadRemove(&pClient->buffFreeList));
            }

            if(pRxBuffer == NULL)
            {   // failed
                F_DHCPv6_StatIncrement(pClient, offsetof(TCPIP_DHCPV6_CLIENT_STATISTICS, rxBuffFailCnt));
                rxAbort = true;
                break;
            }

            if(pktSize > pRxBuffer->bufferSize)
            {   // can't handle this packet
                F_DHCPv6_StatIncrement(pClient, offsetof(TCPIP_DHCPV6_CLIENT_STATISTICS, rxBuffSpaceFailCnt));
                break;
            }

            // get the message
            (void)TCPIP_UDP_ArrayGet(dhcpSkt, pRxBuffer->pMsgData, pktSize);

            // set the message buffer and options
            pHdr = FC_U8Ptr2MsgHdr(pRxBuffer->pMsgData);
            pRxBuffer->msgLen = pktSize; 
            pRxBuffer->optLen = pRxBuffer->msgLen - (uint16_t)sizeof(TCPIP_DHCPV6_MESSAGE_HEADER_BARE);
            pRxBuffer->pOptData = pHdr->options;

            // minimum sanity check
            if(!F_DHCPV6MsgValidate(pRxBuffer, pClient))
            {   // failed
                break;
            }
                    
            // success
            TCPIP_Helper_SingleListTailAdd(&pClient->rxMsgList, FC_MsgBuff2SglNode(pRxBuffer));
            pRxBuffer = NULL;
            
            // Set the port Server number to 547 , so DHCP V6 Client transmit 
            // the request message with port number 547.
            (void)TCPIP_UDP_RemoteBind(pClient->dhcpSkt,IP_ADDRESS_TYPE_IPV6, dhcpv6ServerPort, NULL);

            break;
        }
    
        // We are done with this packet
        (void)TCPIP_UDP_Discard(dhcpSkt);
    }

    if(pRxBuffer != NULL)
    {   // put it back to free buffers list
        TCPIP_Helper_SingleListTailAdd(&pClient->buffFreeList, FC_MsgBuff2SglNode(pRxBuffer));
    }
    
}

static bool F_DHCPV6MsgValidate(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    TCPIP_DHCPV6_DUID_DCPT checkDuid;
    TCPIP_DHCPV6_DUID_DCPT* pSrvDuid = NULL;
    uint16_t checkMask = TCPIP_DHCPV6_MSG_VALIDATION_MASK;
    uint16_t msgMask = DHCPV6_VALID_MASK_NONE;
    
    TCPIP_DHCPV6_MESSAGE_HEADER* pHdr = FC_U8Ptr2MsgHdr(pMsgBuffer->pMsgData);

    // allow only server originating message types
    if(pHdr->msg_type == (uint8_t)DHCPV6_MSG_TYPE_ADVERTISE || pHdr->msg_type == (uint8_t)DHCPV6_MSG_TYPE_REPLY)
    {
        msgMask |= DHCPV6_VALID_MASK_TYPE;
        if(pMsgBuffer->optLen > 0U)
        {
            msgMask |= DHCPV6_VALID_MASK_OPT_LEN;
            // check for our client ID
            if(F_DHCPV6MsgGet_Duid(&checkDuid, pMsgBuffer, true))
            {
                msgMask |= DHCPV6_VALID_MASK_CLIENT_DUID;
                if(F_DHCPV6Duid_Compare(&checkDuid, &pClient->clientDuid))
                {
                    msgMask |= DHCPV6_VALID_MASK_CLIENT_THIS;
                    // check for a server ID
                    if(F_DHCPV6MsgGet_Duid(&checkDuid, pMsgBuffer, false))
                    {
                        pSrvDuid = pSrvDuid;    // hush xc32 warning
                        pSrvDuid = &checkDuid;
                        msgMask |= DHCPV6_VALID_MASK_SERVER_DUID;
                    }
                }
            }
        } 
    }

    F_DHCPV6DbgValidate_Print(pMsgBuffer, msgMask, pHdr->msg_type, pSrvDuid); 
    return msgMask == checkMask;
}

// processes messages in the RX queue: rxMsgList
//
// Notes: To minimize processing time and traversing these lists: 
//      - Addresses with invalid lifetimes are NOT discarded here!
//        Validation is done when the iAs are used
//      - IAs with invalid T1, T2 are not invalidated here.
//        Validation is done when the iAs are used
//      - The message is not searched for IAs belonging to the client
//        Not worth the trouble, and probably most of the messages that reach the client
//        are intended for it anyway.
// 
static void F_DHCPV6Client_ProcessTask(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    TCPIP_DHCPV6_SERVER_STATUS_CODE serverStatCode;
    TCPIP_DHCPV6_MESSAGE_HEADER* pHdr;
    TCPIP_DHCPV6_MSG_BUFFER* pRxBuffer;
    TCPIP_DHCPV6_MSG_SEARCH_DCPT srchDcpt;
    SINGLE_LIST*    pInsertList = NULL;
    bool discardMsg;

    while((pRxBuffer = FC_SglNode2MsgBuff(TCPIP_Helper_SingleListHeadRemove(&pClient->rxMsgList))) != NULL)
    {
        discardMsg = false;
        pHdr = FC_U8Ptr2MsgHdr(pRxBuffer->pMsgData);
        // process the message options!
        F_DHCPV6MsgGet_Options(pClient, pRxBuffer);

        // check if we have a server status code
        serverStatCode = F_DHCPV6MsgGet_StatusCode(pClient->cliLastStatusMsg, pRxBuffer, NULL, pClient->cliStatusMsgSize, NULL);
        if(serverStatCode != TCPIP_DHCPV6_SERVER_STAT_EXT_ERROR)
        {
            pClient->cliLastStatusCode = (uint16_t)serverStatCode;
            if(pHdr->msg_type == (uint8_t)DHCPV6_MSG_TYPE_ADVERTISE)
            {
                if(serverStatCode == TCPIP_DHCPV6_SERVER_STAT_NO_ADDRS_AVAIL)
                {   // no addresses for this IA or any other that may be part of this message! discard;
                    discardMsg = true;
                }
            }
        } 

        if(pHdr->msg_type == (uint8_t)DHCPV6_MSG_TYPE_ADVERTISE)
        {
            // check 1st it has IANA/IATA in it; otherwise it's useless

            srchDcpt.pOptBuff = pRxBuffer->pOptData;
            srchDcpt.optBuffLen = (int16_t)pRxBuffer->optLen;
            srchDcpt.matchFnc = &F_DHCPV6OptionMatchFnc_IA;
            if(F_DHCPV6OptionFind_OptCode(&srchDcpt, DHCPV6_OPT_CODE_IA_NA).pOptG == NULL)
            {
                discardMsg = true;
            }
            else
            {   // ok, let it through
                pInsertList = &pClient->advertiseList;
            }
        }
        else if(pHdr->msg_type == (uint8_t)DHCPV6_MSG_TYPE_REPLY)
        {   // let it through
            pInsertList = &pClient->replyList;
        }
        else
        {   // discard it; shouldn't happen
            TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
            discardMsg = true;
        }

        if(discardMsg)
        {
            TCPIP_Helper_SingleListTailAdd(&pClient->buffFreeList, FC_MsgBuff2SglNode(pRxBuffer));
            F_DHCPV6DbgMsgIn_PrintFailed("Process", pRxBuffer, pHdr->msg_type, "No Addrs Avail");
        }
        else
        {
            TCPIP_Helper_SingleListTailAdd(pInsertList, FC_MsgBuff2SglNode(pRxBuffer));
            F_DHCPV6DbgMsgIn_PrintPassed("Process", pRxBuffer, pHdr->msg_type);
        }

    }
}

static void F_DHCPV6Ia_SetRunState(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_IA_STATE iaState, TCPIP_DHCPV6_IA_SUBSTATE iaSubState)
{
    bool stateNotify = false;
    bool subNotify = false;
    TCPIP_DHCPV6_CLIENT_DCPT* pClient = pIa->pParent;

    TCPIP_DHCPV6_IA_STATE oldState = (TCPIP_DHCPV6_IA_STATE)pIa->iaState;
    TCPIP_DHCPV6_IA_SUBSTATE oldSubState = (TCPIP_DHCPV6_IA_SUBSTATE)pIa->iaSubState;

    TCPIPStack_Assert((TCPIP_Helper_DoubleListFind(pClient->iaStateList + (int)oldState, FC_IaDcpt2DblNode(pIa)) != false), __FILE__, __func__, __LINE__);

    if(iaState != oldState)
    {
        TCPIP_Helper_DoubleListNodeRemove(pClient->iaStateList + (int)oldState, FC_IaDcpt2DblNode(pIa));
        TCPIP_Helper_DoubleListTailAdd(pClient->iaStateList + (int)iaState, FC_IaDcpt2DblNode(pIa));
    }

    F_DHCPV6Ia_SetState(pIa, iaState, iaSubState);

    if(iaState != oldState)
    {
        F_DHCPV6Ia_ReleaseMsgBuffer(pIa);
        stateNotify = true;
    }
    else if(((uint16_t)pClient->configFlags & (uint16_t)TCPIP_DHCPV6_FLAG_IA_NOTIFY_SUB_STATE) != 0U)
    {
        subNotify = iaSubState != oldSubState;
    }
    else
    {
        // do nothing
    }

    if(stateNotify || subNotify)
    {
        F_DHCPV6Client_Notify(NULL, pIa, subNotify);
    }
}

static void F_DHCPV6Client_SetStateNotify(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_CLIENT_STATE cliState)
{

    TCPIP_DHCPV6_CLIENT_STATE oldState = (TCPIP_DHCPV6_CLIENT_STATE)pClient->state;
    if(oldState != cliState)
    {
        pClient->state = (uint16_t)cliState;
        F_DHCPV6Client_Notify(pClient, NULL, false);
    }
}


// marks as invalid the IAs in this TCPIP_DHCPV6_MSG_BUFFER list that match the IA id.
// if possible, returns the TCPIP_DHCPV6_MSG_BUFFER list to the free list
static void F_DHCPV6Ia_MsgListPurge(SINGLE_LIST* pL, TCPIP_DHCPV6_IA_DCPT* pIa)
{
    int nIas;

    TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer;
    TCPIP_DHCPV6_CLIENT_DCPT* pParent = pIa->pParent;
    SINGLE_LIST newList = { 0 };

    while((pMsgBuffer = FC_SglNode2MsgBuff(TCPIP_Helper_SingleListHeadRemove(pL))) != NULL)
    {
        // check for the IA belonging to this list
        F_DHCPV6Ia_MsgInvalidate(pIa, pMsgBuffer);

        // get the number of used IAs in this message
        nIas = F_DHCPV6MsgFind_InUseIAs(pMsgBuffer, pParent, pIa);

        if(nIas == 0)
        {   // we're done with this message
            TCPIP_Helper_SingleListTailAdd(&pParent->buffFreeList, FC_MsgBuff2SglNode(pMsgBuffer));
        }
        else
        {   // keep it
            TCPIP_Helper_SingleListTailAdd(&newList, FC_MsgBuff2SglNode(pMsgBuffer));
        }
    }

    // append the nodes still in there
    TCPIP_Helper_SingleListAppend(pL, &newList);
}

// forcibly purges a list of messages
// the messages are added back to the buffFreeList
static void F_DHCPV6_MsgListForcePurge(TCPIP_DHCPV6_CLIENT_DCPT* pClient, SINGLE_LIST* pL)
{
    TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer;

    while((pMsgBuffer = FC_SglNode2MsgBuff(TCPIP_Helper_SingleListHeadRemove(pL))) != NULL)
    {
        TCPIP_Helper_SingleListTailAdd(&pClient->buffFreeList, FC_MsgBuff2SglNode(pMsgBuffer));
    }
}

// forcibly removes a list of messages
// this is used for the TCPIP_DHCPV6_CLIENT_DCPT::txMsgList
//      the buffers still belong to the IaDcpt!
static void F_DHCPV6_MsgListForceRemove(TCPIP_DHCPV6_CLIENT_DCPT* pClient, SINGLE_LIST* pL)
{
    TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer;

    while((pMsgBuffer = FC_SglNode2MsgBuff(TCPIP_Helper_SingleListHeadRemove(pL))) != NULL)
    {
        // do nothing
    }
}

// forcibly purges all client IA lists 
static void F_DHCPV6_MsgListPurgeAll(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    F_DHCPV6_MsgListForcePurge(pClient, &pClient->advertiseList);
    F_DHCPV6_MsgListForcePurge(pClient, &pClient->replyList);
    F_DHCPV6_MsgListForceRemove(pClient, &pClient->txMsgList);
}

// invalidates a IA in a message so that a search operation won't find it again
static void F_DHCPV6Ia_MsgInvalidate(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer)
{
    void* pOptIa;

    pOptIa = F_DHCPV6OptionFind_Ia(pMsgBuffer, pIa, false);
    if(pOptIa != NULL)
    {
        if(pIa->iaBody.type == TCPIP_DHCPV6_IA_TYPE_IANA)
        {
            TCPIP_DHCPV6_OPTION_IANA* pOptIana = FC_VPtr2OptIana(pOptIa);
            pOptIana->body.iaid = 0U;    // mark it as invalid
        }
        else
        {
            TCPIP_DHCPV6_OPTION_IATA* pOptIata = FC_VPtr2OptIata(pOptIa);
            pOptIata->body.iaid = 0U;    // mark it as invalid
        }
    }

}


// match find function for any IA type: IATA or IANA
// Note: pOptG is not translated
static bool F_DHCPV6OptionMatchFnc_IA(TCPIP_DHCPV6_OPTION_GENERIC* pOptG, uint32_t srchCode)
{
    uint32_t optCode = (uint32_t)(int)TCPIP_Helper_ntohs(pOptG->optCode);
    return (optCode == DHCPV6_OPT_CODE_IA_NA || optCode == DHCPV6_OPT_CODE_IA_TA);
}

// searches the message buffer for valid IAs belonging to client
// the search occurs only for the IAs being in the same state with the pIa parameter
// returns the number of valid IAs (iaid != 0) belonging to this client
static int F_DHCPV6MsgFind_InUseIAs(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_IA_DCPT* pIa)
{
    DHCPV6_OPTION_GEN_UNION_PTR uOptPtr;
    uint32_t    srchId;
    TCPIP_DHCPV6_IA_TYPE srchType;
    TCPIP_DHCPV6_MSG_SEARCH_DCPT srchDcpt;

    int iaCount = 0;
    srchDcpt.pOptBuff = pMsgBuffer->pOptData;
    srchDcpt.optBuffLen = (int16_t)pMsgBuffer->optLen;
    srchDcpt.matchFnc = &F_DHCPV6OptionMatchFnc_IA;

    while(true)
    {
        uOptPtr = F_DHCPV6OptionFind_OptCode(&srchDcpt, 0);
        TCPIP_DHCPV6_OPTION_GENERIC* pOptG = uOptPtr.pOptG;
        if(pOptG == NULL)
        {
            break;
        }
    
        pMsgBuffer = NULL;
        if(TCPIP_Helper_ntohs(pOptG->optCode) == DHCPV6_OPT_CODE_IA_NA)
        {
            TCPIP_DHCPV6_OPTION_IANA* pOptIana = uOptPtr.pOptIana;
            srchId = TCPIP_Helper_htonl(pOptIana->body.iaid);
            srchType = TCPIP_DHCPV6_IA_TYPE_IANA;
        }
        else
        {
            TCPIP_DHCPV6_OPTION_IATA* pOptIata = uOptPtr.pOptIata;
            srchId = TCPIP_Helper_htonl(pOptIata->body.iaid);
            srchType = TCPIP_DHCPV6_IA_TYPE_IATA;
        }

        if(F_DHCPV6Client_FindIaById(pClient, srchId, srchType, (TCPIP_DHCPV6_IA_STATE)pIa->iaState))
        {
            iaCount++;
        }
    }

    return iaCount;
}

// returns true if the IA client has an IA with this ID and type and in the corresponding state 
static bool F_DHCPV6Client_FindIaById(TCPIP_DHCPV6_CLIENT_DCPT* pClient, uint32_t iaid, TCPIP_DHCPV6_IA_TYPE type, TCPIP_DHCPV6_IA_STATE srchState)
{
    TCPIP_DHCPV6_IA_DCPT    *pNode;
    DOUBLE_LIST* pSrchList = pClient->iaStateList + (int)srchState;

    for(pNode = FC_DblNode2IaDcpt(pSrchList->head); pNode != NULL; pNode = pNode->next)
    {
        if(pNode->iaBody.type == type && pNode->iaBody.genId == iaid)
        {
            return true;
        }
    }
    
    return false;
}                

// will find any IA type: IANA/IATA!
// that has a valid/confirmed IPv6 address [BOUND, RELEASE) matching the parameter
static TCPIP_DHCPV6_IA_DCPT* F_DHCPV6Client_FindIaByValidAddr(TCPIP_DHCPV6_CLIENT_DCPT* pClient, const IPV6_ADDR* addr)
{
    TCPIP_DHCPV6_IA_DCPT    *pIaNode;
    TCPIP_DHCPV6_IA_STATE startState = TCPIP_DHCPV6_IA_STATE_BOUND;
    TCPIP_DHCPV6_IA_STATE endState = TCPIP_DHCPV6_IA_STATE_RELEASE;
    size_t stateIx;

    for(stateIx = (size_t)startState; stateIx < (size_t)endState; stateIx++)
    {
        DOUBLE_LIST* pSrchList = pClient->iaStateList + stateIx;
        for(pIaNode = FC_DblNode2IaDcpt(pSrchList->head); pIaNode != NULL; pIaNode = pIaNode->next)
        {
            TCPIP_DHCPV6_ADDR_NODE* addNode;
            for(addNode = FC_SglNode2AddNode(pIaNode->addList.head); addNode != NULL; addNode = addNode->next)
            {
                if(!addNode->inUse)
                {
                    continue;
                }

                if(memcmp(addNode->addBody.ipv6Addr.v, addr->v, sizeof(*addr)) == 0)
                {
                    return pIaNode;
                }
            }
        }
    }
    
    return NULL;
}                

// searches the message buffer to find a vaild IA (IANA/IATA) with this iaid
// returns !0 if found, 0 if not
static void* F_DHCPV6OptionFind_Ia(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_IA_DCPT* pIa, bool serverMatch)
{
    TCPIP_DHCPV6_MSG_SEARCH_DCPT srchDcpt;

    if(serverMatch)
    {
        TCPIP_DHCPV6_DUID_DCPT srvDuid;

        if(!F_DHCPV6MsgGet_Duid(&srvDuid, pMsgBuffer, false))
        {   // couldn't find a server ID in the message although requested!
            TCPIPStack_Condition(false, __FILE__, __func__, __LINE__);
            return NULL;
        }

        if(!F_DHCPV6Duid_Compare(&srvDuid, &pIa->serverDuid))
        {   // ID mismatch
            TCPIPStack_Condition(false, __FILE__, __func__, __LINE__);
            return NULL;
        }

        // ok seems to be the correct server
    }


    srchDcpt.pOptBuff = pMsgBuffer->pOptData;
    srchDcpt.optBuffLen = (int16_t)pMsgBuffer->optLen;
    srchDcpt.matchFnc = NULL;
    
    if(pIa->iaBody.type == TCPIP_DHCPV6_IA_TYPE_IANA)
    {
        TCPIP_DHCPV6_OPTION_IANA* pOptIana = F_DHCPV6OptionFind_OptCode(&srchDcpt, DHCPV6_OPT_CODE_IA_NA).pOptIana;
        if(pOptIana != NULL)
        {
            if(TCPIP_Helper_htonl(pOptIana->body.iaid) == pIa->iaBody.genId)
            {
                return pOptIana;
            }
        }
    }
    else
    {
        TCPIP_DHCPV6_OPTION_IATA* pOptIata = F_DHCPV6OptionFind_OptCode(&srchDcpt, DHCPV6_OPT_CODE_IA_TA).pOptIata;
        if(pOptIata != NULL)
        {
            if(TCPIP_Helper_htonl(pOptIata->body.iaid) == pIa->iaBody.genId)
            {
                return pOptIata;
            }
        }
    }

    return NULL;
}

// compares 2 DUIDs and returns true if they match
static bool F_DHCPV6Duid_Compare(const TCPIP_DHCPV6_DUID_DCPT* pDuid1, const TCPIP_DHCPV6_DUID_DCPT* pDuid2)
{
    if(pDuid1->duidType == pDuid2->duidType)
    {
        if(pDuid1->duidLen >= sizeof(TCPIP_DHCPV6_DUID_LL_MIN_BARE) && pDuid1->duidLen <= pDuid2->duidLen)
        {
            if(memcmp(pDuid1->duidBody.v8, pDuid2->duidBody.v8, pDuid1->duidLen) == 0)
            {
                return true;
            }
        }
    }
    return false;    
}


// searches for an server status code within the message
// pIa could be 0!
// returns TCPIP_DHCPV6_SERVER_STAT_EXT_ERROR if no status code found,
// otherwise a valid status code
// statusBuffer could be 0 if message not needed
// the message is not NULL terminated!
static TCPIP_DHCPV6_SERVER_STATUS_CODE F_DHCPV6MsgGet_StatusCode(uint8_t* statusBuffer, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, TCPIP_DHCPV6_IA_DCPT* pIa, uint16_t statusBufferSize, uint16_t* pStatusMsgSize)
{
    TCPIP_DHCPV6_MSG_SEARCH_DCPT srchDcpt;
    TCPIP_DHCPV6_OPTION_STATUS_CODE* pOptStat;
    TCPIP_DHCPV6_SERVER_STATUS_CODE statusCode = TCPIP_DHCPV6_SERVER_STAT_EXT_ERROR;


    srchDcpt.pOptBuff = pSrcBuffer->pOptData;
    srchDcpt.optBuffLen = (int16_t)pSrcBuffer->optLen;
    srchDcpt.matchFnc = NULL;
    pOptStat = F_DHCPV6OptionFind_OptCode(&srchDcpt, DHCPV6_OPT_CODE_STATUS_CODE).pOptStat;
    if(pOptStat != NULL)
    {
        uint16_t optLen = TCPIP_Helper_ntohs(pOptStat->optLen);
        statusCode = (TCPIP_DHCPV6_SERVER_STATUS_CODE)(int)TCPIP_Helper_ntohs(pOptStat->statusCode);
        if(optLen >= sizeof(pOptStat->statusCode) && 0 <= (int16_t)statusCode && (int16_t)statusCode <= (int16_t)TCPIP_DHCPV6_SERVER_STAT_MAX_CODE)
        {   // valid
            uint16_t msgLen = optLen - (uint16_t)sizeof(pOptStat->statusCode);
            if(pStatusMsgSize != NULL)
            {
                *pStatusMsgSize = msgLen;
            }

            if(statusBuffer != NULL && statusBufferSize != 0U)
            {
                (void)memset(statusBuffer, 0, statusBufferSize);
                if(msgLen > statusBufferSize)
                {   // truncate
                    msgLen = statusBufferSize;
                }
                uint8_t* statusMsg = pOptStat->statusMsg; 
                (void)memcpy(statusBuffer, statusMsg, msgLen);
            }
        }
    }

    F_DHCPV6DbgMsg_ServerStatus(pIa, statusCode);
    return statusCode;
}

// searches for an server status code within the IA
// returns TCPIP_DHCPV6_SERVER_STAT_EXT_ERROR if no status code found,
// otherwise a valid status code
// statusBuffer could be 0 if message not needed
// the message is not NULL terminated!
static TCPIP_DHCPV6_SERVER_STATUS_CODE F_DHCPV6MsgGet_IaOptStatusCode(uint8_t* statusBuffer, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, TCPIP_DHCPV6_IA_DCPT* pIa, uint16_t statusBufferSize, uint16_t* pStatusMsgSize)
{
    TCPIP_DHCPV6_MSG_BUFFER optMsgBuffer;
    if(F_DHCPV6MsgGet_IaOptBuffer(&optMsgBuffer, pSrcBuffer, pIa))
    {
        return F_DHCPV6MsgGet_StatusCode(statusBuffer, &optMsgBuffer, pIa, statusBufferSize, pStatusMsgSize);
    }

    return TCPIP_DHCPV6_SERVER_STAT_SUCCESS;
}


// >= 0 if success
// returns 0 if preference not found (RFC 3315 pg 34/101)
static uint16_t F_DHCPV6MsgGet_ServerPref(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer)
{
    TCPIP_DHCPV6_MSG_SEARCH_DCPT srchDcpt;

    TCPIP_DHCPV6_OPTION_PREFERENCE* pOptPref;

    srchDcpt.pOptBuff = pMsgBuffer->pOptData;
    srchDcpt.optBuffLen = (int16_t)pMsgBuffer->optLen;
    srchDcpt.matchFnc = NULL;

    pOptPref = F_DHCPV6OptionFind_OptCode(&srchDcpt, DHCPV6_OPT_CODE_PREFERENCE).pOptPref;
    if(pOptPref != NULL)
    {
        uint16_t optLen = TCPIP_Helper_ntohs(pOptPref->optLen);
        if(optLen == sizeof(pOptPref->prefValue))
        {
            return (uint16_t)pOptPref->prefValue;
        }
    }

    return 0U;
}

// search for a rapid commit option in the message
// returns true if found, false otherwise
static bool F_DHCPV6OptionFind_RapidCommit(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer)
{
    TCPIP_DHCPV6_MSG_SEARCH_DCPT srchDcpt;

    TCPIP_DHCPV6_OPTION_RAPID_COMMIT * pRapCommit;

    srchDcpt.pOptBuff = pMsgBuffer->pOptData;
    srchDcpt.optBuffLen = (int16_t)pMsgBuffer->optLen;
    srchDcpt.matchFnc = NULL;
    pRapCommit = F_DHCPV6OptionFind_OptCode(&srchDcpt, DHCPV6_OPT_CODE_RAPID_COMMIT).pRCommit;
    if(pRapCommit != NULL && pRapCommit->optLen == 0U)
    {
        return true;
    }

    return false;
}

// returns true if Unicast option found
//      if pUniAdd is provided, it will be updated
// false otherwise
static bool F_DHCPV6OptionFind_ServerUnicast(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, IPV6_ADDR* pUniAdd)
{
    TCPIP_DHCPV6_MSG_SEARCH_DCPT srchDcpt;
    TCPIP_DHCPV6_OPTION_UNICAST* pUnicast;


    srchDcpt.pOptBuff = pMsgBuffer->pOptData;
    srchDcpt.optBuffLen = (int16_t)pMsgBuffer->optLen;
    srchDcpt.matchFnc = NULL;
    pUnicast = F_DHCPV6OptionFind_OptCode(&srchDcpt, DHCPV6_OPT_CODE_UNICAST).pUnicast;
    if(pUnicast != NULL)
    {
        if(TCPIP_Helper_ntohs(pUnicast->optLen) == sizeof(pUnicast->serverAddr))
        {   // all good
            if(pUniAdd != NULL)
            {
                *pUniAdd = pUnicast->serverAddr; 
            }
            return true;
        }
    }

    return false;
}

// updates the destination buffer with the options within a IANA/IATA
// Only optLen and pOptData are updated!
static bool F_DHCPV6MsgGet_IaOptBuffer(TCPIP_DHCPV6_MSG_BUFFER* pDstBuffer, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, TCPIP_DHCPV6_IA_DCPT* pIa)
{
    void* pOptIa = F_DHCPV6OptionFind_Ia(pSrcBuffer, pIa, false);
    if(pOptIa != NULL)
    {   // found the IA
        uint16_t optLen;
        uint8_t* optBuff;

        if(pIa->iaBody.type == TCPIP_DHCPV6_IA_TYPE_IANA)
        {
            TCPIP_DHCPV6_OPTION_IANA* pOptIana = FC_VPtr2OptIana(pOptIa);
            optLen = TCPIP_Helper_ntohs(pOptIana->optLen) - (uint16_t)sizeof(pOptIana->body);
            optBuff = pOptIana->ianaOpt;
        }
        else
        {
            TCPIP_DHCPV6_OPTION_IATA* pIata = FC_VPtr2OptIata(pOptIa);
            optLen = TCPIP_Helper_ntohs(pIata->optLen) - (uint16_t)sizeof(pIata->body);
            optBuff = pIata->iataOpt;
        }

        if(optLen != 0U)
        {
            pDstBuffer->optLen = optLen;
            pDstBuffer->pOptData = optBuff;
            return true;
        }
    }

    return false;
}


// DHCPV6 connection handler
//
void TCPIP_DHCPV6_ConnectionHandler(TCPIP_NET_IF* pNetIf, TCPIP_MAC_EVENT connEvent)
{
    TCPIP_DHCPV6_CLIENT_DCPT* pClient = dhcpv6ClientDcpt + TCPIPStackNetIxGet(pNetIf);

    uint16_t event16 = (uint16_t)pClient->connEvent;
    event16 |= (uint16_t)connEvent;
    pClient->connEvent = (TCPIP_MAC_EVENT)event16;
}

// iterative search for an option code
// searches for an option with the specified code inside the message buffer
// the pSrchDcpt needs to be properly set with (pOptBuff, optBuffLen) for the 1st search!
// The function updates the pSrchDcpt as it goes
// return a valid pointer within the message if found, 0 otherwise
// If the pSrchDcpt->matchFnc != 0, it uses this function for comparison
// else it makes a simple comparison using the srchCode
//
// Notes:
//      - no data is changed within the option itself (like ntohs/ntohl/etc translations!
//        The corresponding F_DHCPV6OptionGet_... copies data and adjusts the fields!
//      - The returned option pointer could be unaligned!
//
static DHCPV6_OPTION_GEN_UNION_PTR F_DHCPV6OptionFind_OptCode(TCPIP_DHCPV6_MSG_SEARCH_DCPT* pSrchDcpt, uint32_t srchCode)
{
    uint8_t* pOptBuff;
    uint32_t optCode;
    int16_t  optBuffLen;
    int16_t  optDataLen;
    int16_t  optGenLen;
    bool     optFound;

    
    pOptBuff = pSrchDcpt->pOptBuff;
    optBuffLen = (int16_t)pSrchDcpt->optBuffLen;

    DHCPV6_OPTION_GEN_UNION_PTR uOptPtr;
    uOptPtr.optBuffer = pOptBuff;

    while(optBuffLen > 0)
    {
        TCPIP_DHCPV6_OPTION_GENERIC* pOptG = uOptPtr.pOptG;
        optCode = (uint32_t)(int)TCPIP_Helper_ntohs(pOptG->optCode);
        optDataLen = (int16_t)TCPIP_Helper_ntohs(pOptG->optLen);
        optGenLen =  (int16_t)sizeof(*pOptG) + optDataLen;

        if(optGenLen > optBuffLen)
        {   // ill formatted option ?
            TCPIPStack_Condition(false, __FILE__, __func__, __LINE__);
            break;
        }

        if(pSrchDcpt->matchFnc != NULL)
        {
            optFound = pSrchDcpt->matchFnc(pOptG, srchCode);
        }
        else 
        {
            optFound = optCode == srchCode;
        }
        
        if(optFound)
        {   // found it
            pSrchDcpt->pOptBuff = uOptPtr.optBuffer + optGenLen;
            pSrchDcpt->optBuffLen = optBuffLen - optGenLen;
            return uOptPtr;
        }

        // go to the next option
        uOptPtr.optBuffer += optGenLen;
        optBuffLen -= optGenLen;
    }

    uOptPtr.pOptG = NULL;
    return uOptPtr;

}    

static bool F_DHCPV6MsgCheck_TransactionId(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer)
{
    TCPIP_UINT32_VAL transId;
    TCPIP_DHCPV6_MESSAGE_HEADER* pHdr = FC_U8Ptr2MsgHdr(pSrcBuffer->pMsgData);

    (void)memcpy(transId.v, pHdr->transId, sizeof(pHdr->transId));
    transId.v[3] = 0;

    return (transId.Val == pIa->transactionId.Val);
}

static void F_DHCPV6MsgGet_Options(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer)
{
    size_t entryIx;
    const S_DHCPV6MsgGet_OptionEntry* pEntry;

    pEntry = T_DHCPV6MsgGet_OptionRetrieveTbl;
    for(entryIx = 0; entryIx < sizeof(T_DHCPV6MsgGet_OptionRetrieveTbl) / sizeof(*T_DHCPV6MsgGet_OptionRetrieveTbl); entryIx++)
    {
        if(pEntry->optFnc != NULL)
        {
            (*pEntry->optFnc)(pClient, pMsgBuffer, pEntry);
        }
        pEntry++;
    }
}

static void F_DHCPV6MsgGet_DnsServers(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, const S_DHCPV6MsgGet_OptionEntry* pEntry)
{
    TCPIP_DHCPV6_MSG_SEARCH_DCPT srchDcpt;
    TCPIP_DHCPV6_OPTION_DNS_SERVERS* pOptDns;


    srchDcpt.pOptBuff = pMsgBuffer->pOptData;
    srchDcpt.optBuffLen = (int16_t)pMsgBuffer->optLen;
    srchDcpt.matchFnc = NULL;
    // process SOL_MAX_RT, if available
    pOptDns = F_DHCPV6OptionFind_OptCode(&srchDcpt, DHCPV6_OPT_CODE_DNS_SERVERS).pOptDns;
    if(pOptDns != NULL)
    {
        uint16_t optLen = TCPIP_Helper_ntohs(pOptDns->optLen);
        if( optLen != 0U && (optLen & 0x0fU) == 0U)
        {
            size_t dnsIx;
            uint16_t nDnsServers = optLen >> 4;
            if(nDnsServers > sizeof(pClient->dnsServersAdd) / sizeof(*pClient->dnsServersAdd))
            {
                nDnsServers = sizeof(pClient->dnsServersAdd) / sizeof(*pClient->dnsServersAdd);
            }
            IPV6_ADDR* pDst = pClient->dnsServersAdd;
            IPV6_ADDR* pSrc = pOptDns->dnsAddr;

            for(dnsIx = 0; dnsIx < nDnsServers; dnsIx++)
            {
                (void)memcpy(pDst, pSrc, sizeof(*pSrc));
                pDst++; pSrc++;
            }
            pClient->nDnsServers = nDnsServers;
        }
    }

}

static void F_DHCPV6MsgGet_DomainList(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, const S_DHCPV6MsgGet_OptionEntry* pEntry)
{
    TCPIP_DHCPV6_MSG_SEARCH_DCPT srchDcpt;
    TCPIP_DHCPV6_OPTION_DOMAIN_SEARCH_LIST* pDomList;

    srchDcpt.pOptBuff = pMsgBuffer->pOptData;
    srchDcpt.optBuffLen = (int16_t)pMsgBuffer->optLen;
    srchDcpt.matchFnc = NULL;

    // process SOL_MAX_RT, if available
    pDomList = F_DHCPV6OptionFind_OptCode(&srchDcpt, DHCPV6_OPT_CODE_DOMAIN_LIST).pDomList;
    if(pDomList != NULL)
    {
        uint16_t optLen = TCPIP_Helper_ntohs(pDomList->optLen);
        if( optLen != 0U)
        {
            if(optLen > sizeof(pClient->domainSearchList))
            {
                optLen = (uint16_t)sizeof(pClient->domainSearchList);
                TCPIPStack_Condition(false, __FILE__, __func__, __LINE__);
            }
            uint8_t* searchList = pDomList->searchList;
            (void)memcpy(pClient->domainSearchList, searchList, optLen);
            pClient->domainSearchListSize = optLen;
        }
    }

}

static void F_DHCPV6MsgGet_SolMaxRt(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, const S_DHCPV6MsgGet_OptionEntry* pEntry)
{
    TCPIP_DHCPV6_MSG_SEARCH_DCPT srchDcpt;
    TCPIP_DHCPV6_OPTION_MAX_RT* pOptMaxRt;

    srchDcpt.pOptBuff = pMsgBuffer->pOptData;
    srchDcpt.optBuffLen = (int16_t)pMsgBuffer->optLen;
    srchDcpt.matchFnc = NULL;

    // process SOL_MAX_RT, if available
    pOptMaxRt = F_DHCPV6OptionFind_OptCode(&srchDcpt, DHCPV6_OPT_CODE_MAX_RT).pOptMaxRt;
    if(pOptMaxRt != NULL)
    {
        if(TCPIP_Helper_ntohs(pOptMaxRt->optLen) == sizeof(*pOptMaxRt))
        {
            uint32_t solMaxRt = TCPIP_Helper_ntohl(pOptMaxRt->solMaxRt);
            if(TCPIP_DHCPV6_SOLICIT_MRT_MIN <= solMaxRt && solMaxRt <= TCPIP_DHCPV6_SOLICIT_MRT_MAX)
            {   // override
                pClient->solMaxRt = solMaxRt;
            }
        }
    }
}

static void F_DHCPV6MsgGet_InfoMaxRt(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, const S_DHCPV6MsgGet_OptionEntry* pEntry)
{
    TCPIP_DHCPV6_MSG_SEARCH_DCPT srchDcpt;
    TCPIP_DHCPV6_OPTION_INFO_MAX_RT* pInfoMaxRt;

    srchDcpt.pOptBuff = pMsgBuffer->pOptData;
    srchDcpt.optBuffLen = (int16_t)pMsgBuffer->optLen;
    srchDcpt.matchFnc = NULL;

    pInfoMaxRt = F_DHCPV6OptionFind_OptCode(&srchDcpt, DHCPV6_OPT_CODE_INFO_MAX_RT).pInfoMaxRt;
    if(pInfoMaxRt != NULL)
    {
        if(TCPIP_Helper_ntohs(pInfoMaxRt->optLen) == sizeof(*pInfoMaxRt))
        {
            uint32_t infoMaxRt = TCPIP_Helper_ntohl(pInfoMaxRt->infoMaxRt);
            if(TCPIP_DHCPV6_INFO_REQ_MRT_MIN <= infoMaxRt && infoMaxRt <= TCPIP_DHCPV6_INFO_REQ_MRT_MAX)
            {   // override
                pClient->infoMaxRt = infoMaxRt;
            }
        }
    }
}

// helper to count the # of IANAs and IATAs in a list (of IAs!)
// if strict == true, then only IANAs and IATAs are to be part of the list!
// returns (iatas << 16 | ianas)
static uint32_t F_DHCPv6_FindIAsInList(DOUBLE_LIST* pSrchList, bool strict)
{
    uint16_t nIanas = 0U, nIatas = 0U;

    TCPIP_DHCPV6_IA_DCPT    *pNode;
    for(pNode = FC_DblNode2IaDcpt(pSrchList->head); pNode != NULL; pNode = pNode->next)
    {
        if(pNode->iaBody.type == TCPIP_DHCPV6_IA_TYPE_IANA)
        {
            nIanas++;
        }
        else if(pNode->iaBody.type == TCPIP_DHCPV6_IA_TYPE_IATA)
        {
            nIatas++;
        }
        else if(strict)
        {
            TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
        }
        else
        {
            // do nothing
        }
    }

    return (uint32_t)nIatas << 16U | nIanas;
}

#endif  // defined TCPIP_STACK_USE_IPV6 && defined(TCPIP_STACK_USE_DHCPV6_CLIENT)

