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

/*****************************************************************************
 Copyright (C) 2015-2018 Microchip Technology Inc. and its subsidiaries.

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


// message search descriptor
typedef struct
{
    void*                           pOptBuff;       // current option buffer for next searches
    uint16_t                        optBuffLen;     // current length of the option buffer
    uint16_t                        remBuffLen;     // length of the buffer remaining after this option
}TCPIP_DHCPV6_MSG_SEARCH_DCPT;

// message validation mask
typedef enum
{
    TCPIP_DHCPV6_VALID_MASK_NONE            = 0x00,
    TCPIP_DHCPV6_VALID_MASK_TYPE            = 0x01,
    TCPIP_DHCPV6_VALID_MASK_OPT_LEN         = 0x02,
    TCPIP_DHCPV6_VALID_MASK_CLIENT_DUID     = 0x04,
    TCPIP_DHCPV6_VALID_MASK_SERVER_DUID     = 0x08,
    TCPIP_DHCPV6_VALID_MASK_CLIENT_THIS     = 0x10,
    TCPIP_DHCPV6_VALID_MASK_SERVER_THIS     = 0x20,


    TCPIP_DHCPV6_MSG_VALIDATION_MASK        = (TCPIP_DHCPV6_VALID_MASK_TYPE | TCPIP_DHCPV6_VALID_MASK_OPT_LEN |
                                               TCPIP_DHCPV6_VALID_MASK_CLIENT_DUID | TCPIP_DHCPV6_VALID_MASK_SERVER_DUID | TCPIP_DHCPV6_VALID_MASK_CLIENT_THIS),

}TCPIP_DHCPV6_MSG_VALID_MASK;

// write options functions
// returns >= 0 the size of the option added
// returns < 0 for failure
typedef int (*_DHCPV6Option_WriteFnc)(TCPIP_DHCPV6_IA_DCPT* pIa);


// local data
static int                          dhcpv6InitCount = 0;      // DHCP module initialization count

static UDP_PORT                     dhcpv6ClientPort;
static UDP_PORT                     dhcpv6ServerPort;

static tcpipSignalHandle            dhcpv6SignalHandle = 0;

static const void*                  dhcpv6MemH = 0;        // memory handle

#if (_TCPIP_DHCPV6_USER_NOTIFICATION != 0)
static PROTECTED_SINGLE_LIST        dhcpv6RegisteredUsers = { {0} };
#endif  // (_TCPIP_DHCPV6_USER_NOTIFICATION != 0)


static uint32_t                     dhcpv6SecondCount = 0;    // DHCP time keeping, in seconds

#if ((TCPIP_DHCPV6_DEBUG_LEVEL & TCPIP_DHCPV6_DEBUG_MASK_BASIC) != 0)
TCPIP_DHCPV6_CLIENT_DCPT*           dhcpv6ClientDcpt = 0;   
#else
static TCPIP_DHCPV6_CLIENT_DCPT*    dhcpv6ClientDcpt = 0;    // DHCPv6 client per interface
#endif  // (TCPIP_DHCPV6_DEBUG_LEVEL)

// The IPv6 All_DHCP_Relay_Agents_and_Servers address
const IPV6_ADDR     DHCPV6_Servers_Address = {{0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02}};

static int              _DHCPV6Option_WriteClientId(TCPIP_DHCPV6_IA_DCPT* pIa);
static int              _DHCPV6Option_WriteServerId(TCPIP_DHCPV6_IA_DCPT* pIa);
static int              _DHCPV6Option_WriteIA_NA(TCPIP_DHCPV6_IA_DCPT* pIa);
static int              _DHCPV6Option_WriteIA_TA(TCPIP_DHCPV6_IA_DCPT* pIa);
static int              _DHCPV6Option_WriteOptionRequest(TCPIP_DHCPV6_IA_DCPT* pIa);
static int              _DHCPV6Option_WriteElapsedTime(TCPIP_DHCPV6_IA_DCPT* pIa);
static int              _DHCPV6Option_WriteRapidCommit(TCPIP_DHCPV6_IA_DCPT* pIa);


static int              _DHCPV6Option_Write(TCPIP_DHCPV6_OPTION_CODE optCode, TCPIP_DHCPV6_IA_DCPT* pIa);

// functions performing options write
static const _DHCPV6Option_WriteFnc   _DHCPV6Option_WriteFncTbl[TCPIP_DHCPV6_OPT_CODES_SUPPORTED] = 
{
    0,                                  // TCPIP_DHCPV6_OPT_CODE_NONE
    _DHCPV6Option_WriteClientId,         // TCPIP_DHCPV6_OPT_CODE_CLIENT_ID
    _DHCPV6Option_WriteServerId,         // TCPIP_DHCPV6_OPT_CODE_SERVER_ID
    _DHCPV6Option_WriteIA_NA,            // TCPIP_DHCPV6_OPT_CODE_IA_NA 
    _DHCPV6Option_WriteIA_TA,            // TCPIP_DHCPV6_OPT_CODE_IA_TA
    0,                                  // TCPIP_DHCPV6_OPT_CODE_IA_ADDR         - called by IA_TA/IA_NA
    _DHCPV6Option_WriteOptionRequest,    // TCPIP_DHCPV6_OPT_CODE_OPTION_REQ
    0,                                  // TCPIP_DHCPV6_OPT_CODE_PREFERENCE      - server side only
    _DHCPV6Option_WriteElapsedTime,      // TCPIP_DHCPV6_OPT_CODE_ELAPSED_TIME
    0,                                  // TCPIP_DHCPV6_OPT_CODE_RELAY_MSG       - OPTION_NOT_SUPPORTED; relay, server
    0,                                  // TCPIP_DHCPV6_OPT_CODE_INVALID
    0,                                  // TCPIP_DHCPV6_OPT_CODE_AUTHENTICATE    - OPTION_NOT_SUPPORTED in this implementation
    0,                                  // TCPIP_DHCPV6_OPT_CODE_UNICAST         - server side only
    0,                                  // TCPIP_DHCPV6_OPT_CODE_STATUS_CODE     - server side only
    _DHCPV6Option_WriteRapidCommit,      // TCPIP_DHCPV6_OPT_CODE_RAPID_COMMIT
    0,                                  // TCPIP_DHCPV6_OPT_CODE_USER_CLASS      - OPTION_NOT_SUPPORTED in this implementation
    0,                                  // TCPIP_DHCPV6_OPT_CODE_VENDOR_CLASS    - OPTION_NOT_SUPPORTED in this implementation
    0,                                  // TCPIP_DHCPV6_OPT_CODE_VENDOR_OPTS     - OPTION_NOT_SUPPORTED in this implementation
    0,                                  // TCPIP_DHCPV6_OPT_CODE_INTERFACE_ID    - OPTION_NOT_SUPPORTED; relay messages; unused
    0,                                  // TCPIP_DHCPV6_OPT_CODE_RECONF_MSG      - OPTION_NOT_SUPPORTED; server sent option
    0,                                  // TCPIP_DHCPV6_OPT_CODE_RECONF_ACCEPT   - OPTION_NOT_SUPPORTED 
};


// translation table: TCPIP_DHCPV6_CLIENT_MSG_TYPE -> TCPIP_DHCPV6_MSG_TYPE
static const uint8_t    dhcpv6ClientToMessageTypeTbl[TCPIP_DHCPV6_CLIENT_MSG_TYPE_NUMBER] = 
{
    (uint8_t)TCPIP_DHCPV6_MSG_TYPE_SOLICIT,               // ->TCPIP_DHCPV6_CLIENT_MSG_TYPE_SOLICIT
                                                                          
    (uint8_t)TCPIP_DHCPV6_MSG_TYPE_REQUEST,               // ->TCPIP_DHCPV6_CLIENT_MSG_TYPE_REQUEST
                                                                          
    (uint8_t)TCPIP_DHCPV6_MSG_TYPE_CONFIRM,               // ->TCPIP_DHCPV6_CLIENT_MSG_TYPE_CONFIRM
                                                                          
    (uint8_t)TCPIP_DHCPV6_MSG_TYPE_RENEW,                 // ->TCPIP_DHCPV6_CLIENT_MSG_TYPE_RENEW
                                                                          
    (uint8_t)TCPIP_DHCPV6_MSG_TYPE_REBIND,                // ->TCPIP_DHCPV6_CLIENT_MSG_TYPE_REBIND
                                                                          
    (uint8_t)TCPIP_DHCPV6_MSG_TYPE_RELEASE,               // ->TCPIP_DHCPV6_CLIENT_MSG_TYPE_RELEASE
                                                                          
    (uint8_t)TCPIP_DHCPV6_MSG_TYPE_DECLINE,               // ->TCPIP_DHCPV6_CLIENT_MSG_TYPE_DECLINE
                                                                          
    (uint8_t)TCPIP_DHCPV6_MSG_TYPE_INFO_REQUEST,          // ->TCPIP_DHCPV6_CLIENT_MSG_TYPE_INFO_REQUEST
};


// some of these values could be overwritten by the DHCP server!
static const TCPIP_DHCPV6_MSG_TRANSMIT_BOUNDS dhcpv6MessageBoundsTbl[TCPIP_DHCPV6_CLIENT_MSG_TYPE_NUMBER] =
{
    // iDelay                           irt                         mrt                          mrc                                 mrd     
    {  TCPIP_DHCPV6_SOLICIT_IDELAY,    TCPIP_DHCPV6_SOLICIT_IRT,    TCPIP_DHCPV6_SOLICIT_MRT,    TCPIP_DHCPV6_SOLICIT_MRC,           TCPIP_DHCPV6_SOLICIT_MRD },      // TCPIP_DHCPV6_CLIENT_MSG_TYPE_SOLICIT 
    {  TCPIP_DHCPV6_REQUEST_IDELAY,    TCPIP_DHCPV6_REQUEST_IRT,    TCPIP_DHCPV6_REQUEST_MRT,    TCPIP_DHCPV6_REQUEST_MRC,           TCPIP_DHCPV6_REQUEST_MRD },      // TCPIP_DHCPV6_CLIENT_MSG_TYPE_REQUEST
    {  TCPIP_DHCPV6_CONFIRM_IDELAY,    TCPIP_DHCPV6_CONFIRM_IRT,    TCPIP_DHCPV6_CONFIRM_MRT,    TCPIP_DHCPV6_CONFIRM_MRC,           TCPIP_DHCPV6_CONFIRM_MRD },      // TCPIP_DHCPV6_CLIENT_MSG_TYPE_CONFIRM
    {  TCPIP_DHCPV6_RENEW_IDELAY,      TCPIP_DHCPV6_RENEW_IRT,      TCPIP_DHCPV6_RENEW_MRT,      TCPIP_DHCPV6_RENEW_MRC,             TCPIP_DHCPV6_RENEW_MRD },        // TCPIP_DHCPV6_CLIENT_MSG_TYPE_RENEW
    {  TCPIP_DHCPV6_REBIND_IDELAY,     TCPIP_DHCPV6_REBIND_IRT,     TCPIP_DHCPV6_REBIND_MRT,     TCPIP_DHCPV6_REBIND_MRC,            TCPIP_DHCPV6_REBIND_MRD },       // TCPIP_DHCPV6_CLIENT_MSG_TYPE_REBIND
    {  TCPIP_DHCPV6_RELEASE_IDELAY,    TCPIP_DHCPV6_RELEASE_IRT,    TCPIP_DHCPV6_RELEASE_MRT,    TCPIP_DHCPV6_RELEASE_MRC,           TCPIP_DHCPV6_RELEASE_MRD },      // TCPIP_DHCPV6_CLIENT_MSG_TYPE_RELEASE
    {  TCPIP_DHCPV6_DECLINE_IDELAY,    TCPIP_DHCPV6_DECLINE_IRT,    TCPIP_DHCPV6_DECLINE_MRT,    TCPIP_DHCPV6_DECLINE_MRC,           TCPIP_DHCPV6_DECLINE_MRD },      // TCPIP_DHCPV6_CLIENT_MSG_TYPE_DECLINE
    {  TCPIP_DHCPV6_INFO_REQ_IDELAY,   TCPIP_DHCPV6_INFO_REQ_IRT,   TCPIP_DHCPV6_INFO_REQ_IRT,   TCPIP_DHCPV6_INFO_REQ_IRT,          TCPIP_DHCPV6_INFO_REQ_IRT },     // TCPIP_DHCPV6_CLIENT_MSG_TYPE_INFO_REQUEST
};

// complete DHCPV6 messages option mask
// contains all the options that could be part of the messages
static const TCPIP_DHCPV6_OPTION_MASK_SET_0 DHCPV6_MSG_OPTION_MASK_TBL[TCPIP_DHCPV6_CLIENT_MSG_TYPE_NUMBER] =
{
    {                       // TCPIP_DHCPV6_CLIENT_MSG_TYPE_SOLICIT
        1,
        {
            (
             (1 << TCPIP_DHCPV6_OPT_CODE_CLIENT_ID) |
             (1 << TCPIP_DHCPV6_OPT_CODE_IA_NA) |
             (1 << TCPIP_DHCPV6_OPT_CODE_IA_TA) |
             (1 << TCPIP_DHCPV6_OPT_CODE_OPTION_REQ) |
             (1 << TCPIP_DHCPV6_OPT_CODE_ELAPSED_TIME) |
             (1 << TCPIP_DHCPV6_OPT_CODE_RAPID_COMMIT) 
            ),
        }
    },

    {                       // TCPIP_DHCPV6_CLIENT_MSG_TYPE_REQUEST
        1,
        {
            (
             (1 << TCPIP_DHCPV6_OPT_CODE_CLIENT_ID) |
             (1 << TCPIP_DHCPV6_OPT_CODE_SERVER_ID) |
             (1 << TCPIP_DHCPV6_OPT_CODE_IA_NA) |
             (1 << TCPIP_DHCPV6_OPT_CODE_IA_TA) |
             (1 << TCPIP_DHCPV6_OPT_CODE_OPTION_REQ) |
             (1 << TCPIP_DHCPV6_OPT_CODE_ELAPSED_TIME)
            ),
        }
    },
    
    {                       // TCPIP_DHCPV6_CLIENT_MSG_TYPE_CONFIRM
        1,
        {
            (
             (1 << TCPIP_DHCPV6_OPT_CODE_CLIENT_ID) |
             (1 << TCPIP_DHCPV6_OPT_CODE_IA_NA) |
             (1 << TCPIP_DHCPV6_OPT_CODE_IA_TA) |
             (1 << TCPIP_DHCPV6_OPT_CODE_OPTION_REQ) |
             (1 << TCPIP_DHCPV6_OPT_CODE_ELAPSED_TIME)
            ),
        }
    },

    {                       // TCPIP_DHCPV6_CLIENT_MSG_TYPE_RENEW
        1,
        {
            (
             (1 << TCPIP_DHCPV6_OPT_CODE_CLIENT_ID) |
             (1 << TCPIP_DHCPV6_OPT_CODE_SERVER_ID) |
             (1 << TCPIP_DHCPV6_OPT_CODE_IA_NA) |
             (1 << TCPIP_DHCPV6_OPT_CODE_IA_TA) |
             (1 << TCPIP_DHCPV6_OPT_CODE_OPTION_REQ) |
             (1 << TCPIP_DHCPV6_OPT_CODE_ELAPSED_TIME)
            ),
        }
    },


    {                       // TCPIP_DHCPV6_CLIENT_MSG_TYPE_REBIND
        1,
        {
            (
             (1 << TCPIP_DHCPV6_OPT_CODE_CLIENT_ID) |
             (1 << TCPIP_DHCPV6_OPT_CODE_IA_NA) |
             (1 << TCPIP_DHCPV6_OPT_CODE_IA_TA) |
             (1 << TCPIP_DHCPV6_OPT_CODE_OPTION_REQ) |
             (1 << TCPIP_DHCPV6_OPT_CODE_ELAPSED_TIME)
            ),
        }
    },

    {                       // TCPIP_DHCPV6_CLIENT_MSG_TYPE_RELEASE
        1,
        {
            (
             (1 << TCPIP_DHCPV6_OPT_CODE_CLIENT_ID) |
             (1 << TCPIP_DHCPV6_OPT_CODE_SERVER_ID) |
             (1 << TCPIP_DHCPV6_OPT_CODE_IA_NA) |
             (1 << TCPIP_DHCPV6_OPT_CODE_IA_TA) |
             (1 << TCPIP_DHCPV6_OPT_CODE_OPTION_REQ) |
             (1 << TCPIP_DHCPV6_OPT_CODE_ELAPSED_TIME)
            ),
        }
    },

    {                       // TCPIP_DHCPV6_CLIENT_MSG_TYPE_DECLINE
        1,
        {
            (
             (1 << TCPIP_DHCPV6_OPT_CODE_CLIENT_ID) |
             (1 << TCPIP_DHCPV6_OPT_CODE_SERVER_ID) |
             (1 << TCPIP_DHCPV6_OPT_CODE_IA_NA) |
             (1 << TCPIP_DHCPV6_OPT_CODE_IA_TA) |
             (1 << TCPIP_DHCPV6_OPT_CODE_OPTION_REQ) |
             (1 << TCPIP_DHCPV6_OPT_CODE_ELAPSED_TIME)
            ),
        }
    },

    {                       // TCPIP_DHCPV6_CLIENT_MSG_TYPE_INFO_REQUEST
        1,
        {
            (
             (1 << TCPIP_DHCPV6_OPT_CODE_CLIENT_ID) |
             (1 << TCPIP_DHCPV6_OPT_CODE_SERVER_ID) |
             (1 << TCPIP_DHCPV6_OPT_CODE_OPTION_REQ) |
             (1 << TCPIP_DHCPV6_OPT_CODE_ELAPSED_TIME)
            ),
        }
    },

};


// default TCPIP_DHCPV6_OPT_CODE_OPTION_REQ option request mask;
// could be changed by the user
static TCPIP_DHCPV6_OPTION_MASK_SET_3       DHCPV6_ORO_OPTION_MASK_TBL = 
{
    3,
    {
        (
            (1 << TCPIP_DHCPV6_OPT_CODE_DNS_SERVERS) |
            (1 << TCPIP_DHCPV6_OPT_CODE_DOMAIN_LIST)
        ),
        ( 0 ),
        (
            (1 << (TCPIP_DHCPV6_OPT_CODE_MAX_RT % 64)) |
            (1 << (TCPIP_DHCPV6_OPT_CODE_INFO_MAX_RT % 64))
        ),
    },
};



// default/current  DHCPV6 messages option mask
// could be changed by the user
static TCPIP_DHCPV6_OPTION_MASK_SET_0 DHCPV6_MSG_OPTION_CURR_MASK_TBL[TCPIP_DHCPV6_CLIENT_MSG_TYPE_NUMBER] = {};


// state processing function
//
typedef void (*TCPIP_DHCPV6_CLIENT_STATE_PROC_FNC)(TCPIP_DHCPV6_CLIENT_DCPT* pClient);

static void   _DHCPV6Client_StateProcInit(TCPIP_DHCPV6_CLIENT_DCPT* pClient);
static void   _DHCPV6Client_StateProcIdle(TCPIP_DHCPV6_CLIENT_DCPT* pClient);
static void   _DHCPV6Client_StateProcRun(TCPIP_DHCPV6_CLIENT_DCPT* pClient);
static void   _DHCPV6Client_StateProcWaitLink(TCPIP_DHCPV6_CLIENT_DCPT* pClient);

static const TCPIP_DHCPV6_CLIENT_STATE_PROC_FNC    _DHCPV6Client_StateFncTbl[TCPIP_DHCPV6_CLIENT_STATE_NUMBER] = 
{
    _DHCPV6Client_StateProcInit,                  // TCPIP_DHCPV6_CLIENT_STATE_INIT
    _DHCPV6Client_StateProcIdle,                  // TCPIP_DHCPV6_CLIENT_STATE_IDLE
    _DHCPV6Client_StateProcRun,                   // TCPIP_DHCPV6_CLIENT_STATE_RUN
    _DHCPV6Client_StateProcWaitLink,              // TCPIP_DHCPV6_CLIENT_STATE_WAIT_LINK
};


// run state processing functions
//
// run substate processing function
// it returns a substate result and UPDATES the pIa->iaSubState!
typedef TCPIP_DHCPV6_IA_SUBSTATE_RESULT (*TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC)(TCPIP_DHCPV6_IA_DCPT* pIa);

// standard substate processing functions
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   _DHCPV6Ia_SubStateIDelay(TCPIP_DHCPV6_IA_DCPT* pIa);
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   _DHCPV6Ia_SubStateTransmit(TCPIP_DHCPV6_IA_DCPT* pIa);


// SOLICIT substate processing function table
// most run states that send messages uses these states 
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   _DHCPV6Ia_SubStateSolicitStart(TCPIP_DHCPV6_IA_DCPT* pIa);
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   _DHCPV6Ia_SubStateSolicitationWaitReply(TCPIP_DHCPV6_IA_DCPT* pIa);

static const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC _DHCPV6Ia_SubStateSolicitFTbl[] =
{
    _DHCPV6Ia_SubStateSolicitStart,            // TCPIP_DHCPV6_IA_SUBSTATE_START    
    _DHCPV6Ia_SubStateIDelay,                  // TCPIP_DHCPV6_IA_SUBSTATE_IDELAY   
    _DHCPV6Ia_SubStateTransmit,                // TCPIP_DHCPV6_IA_SUBSTATE_TRANSMIT 
    _DHCPV6Ia_SubStateSolicitationWaitReply,   // TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY
};

// REQUEST substate processing function table
// most run states that send messages uses these states 
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   _DHCPV6Ia_SubStateRequestStart(TCPIP_DHCPV6_IA_DCPT* pIa);
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   _DHCPV6Ia_SubStateWaitReplyMsg(TCPIP_DHCPV6_IA_DCPT* pIa);

static const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC _DHCPV6Ia_SubStateRequestFTbl[] =
{
    _DHCPV6Ia_SubStateRequestStart,            // TCPIP_DHCPV6_IA_SUBSTATE_START    
    _DHCPV6Ia_SubStateIDelay,                  // TCPIP_DHCPV6_IA_SUBSTATE_IDELAY   
    _DHCPV6Ia_SubStateTransmit,                // TCPIP_DHCPV6_IA_SUBSTATE_TRANSMIT 
    _DHCPV6Ia_SubStateWaitReplyMsg,            // TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY
};


// DAD substate processing function table
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   _DHCPV6Ia_SubStateDadStart(TCPIP_DHCPV6_IA_DCPT* pIa);
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   _DHCPV6Ia_SubStateDadWait(TCPIP_DHCPV6_IA_DCPT* pIa);


static const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC _DHCPV6Ia_SubStateDadFTbl[] =
{
    _DHCPV6Ia_SubStateDadStart,     
    _DHCPV6Ia_SubStateDadWait, 
};

// DECLINE substate processing function table
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   _DHCPV6Ia_SubStateDeclineStart(TCPIP_DHCPV6_IA_DCPT* pIa);

static const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC _DHCPV6Ia_SubStateDeclineFTbl[] =
{
    _DHCPV6Ia_SubStateDeclineStart,            // TCPIP_DHCPV6_IA_SUBSTATE_START    
    _DHCPV6Ia_SubStateIDelay,                  // TCPIP_DHCPV6_IA_SUBSTATE_IDELAY   
    _DHCPV6Ia_SubStateTransmit,                // TCPIP_DHCPV6_IA_SUBSTATE_TRANSMIT 
    _DHCPV6Ia_SubStateWaitReplyMsg,            // TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY
};


// BOUND substate processing function table
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   _DHCPV6Ia_SubStateBoundWait(TCPIP_DHCPV6_IA_DCPT* pIa);
static const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC _DHCPV6Ia_SubStateBoundFTbl[] =
{
    _DHCPV6Ia_SubStateBoundWait,        // substate 0
};

// RENEW substate processing function table
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   _DHCPV6Ia_SubStateRenewStart(TCPIP_DHCPV6_IA_DCPT* pIa);

static const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC _DHCPV6Ia_SubStateRenewFTbl[] =
{
    _DHCPV6Ia_SubStateRenewStart,               // TCPIP_DHCPV6_IA_SUBSTATE_START    
    _DHCPV6Ia_SubStateIDelay,                   // TCPIP_DHCPV6_IA_SUBSTATE_IDELAY   
    _DHCPV6Ia_SubStateTransmit,                 // TCPIP_DHCPV6_IA_SUBSTATE_TRANSMIT 
    _DHCPV6Ia_SubStateWaitReplyMsg,             // TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY
};

// REBIND substate processing function table
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   _DHCPV6Ia_SubStateRebindStart(TCPIP_DHCPV6_IA_DCPT* pIa);

static const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC _DHCPV6Ia_SubStateRebindFTbl[] =
{
    _DHCPV6Ia_SubStateRebindStart,              // TCPIP_DHCPV6_IA_SUBSTATE_START    
    _DHCPV6Ia_SubStateIDelay,                   // TCPIP_DHCPV6_IA_SUBSTATE_IDELAY   
    _DHCPV6Ia_SubStateTransmit,                 // TCPIP_DHCPV6_IA_SUBSTATE_TRANSMIT 
    _DHCPV6Ia_SubStateWaitReplyMsg,             // TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY
};


// CONFIRM substate processing function table
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   _DHCPV6Ia_SubStateConfirmStart(TCPIP_DHCPV6_IA_DCPT* pIa);

static const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC _DHCPV6Ia_SubStateConfirmFTbl[] =
{
    _DHCPV6Ia_SubStateConfirmStart,             // TCPIP_DHCPV6_IA_SUBSTATE_START    
    _DHCPV6Ia_SubStateIDelay,                   // TCPIP_DHCPV6_IA_SUBSTATE_IDELAY   
    _DHCPV6Ia_SubStateTransmit,                 // TCPIP_DHCPV6_IA_SUBSTATE_TRANSMIT 
    _DHCPV6Ia_SubStateWaitReplyMsg,             // TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY
};

// RELEASE substate processing function table
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   _DHCPV6Ia_SubStateReleaseStart(TCPIP_DHCPV6_IA_DCPT* pIa);

static const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC _DHCPV6Ia_SubStateReleaseFTbl[] =
{
    _DHCPV6Ia_SubStateReleaseStart,             // TCPIP_DHCPV6_IA_SUBSTATE_START    
    _DHCPV6Ia_SubStateIDelay,                   // TCPIP_DHCPV6_IA_SUBSTATE_IDELAY   
    _DHCPV6Ia_SubStateTransmit,                 // TCPIP_DHCPV6_IA_SUBSTATE_TRANSMIT 
    _DHCPV6Ia_SubStateWaitReplyMsg,             // TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY
};


// ERROR_TRANSIENT substate processing function table
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   _DHCPV6Ia_SubStateErrorTransient(TCPIP_DHCPV6_IA_DCPT* pIa);

static const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC _DHCPV6Ia_SubStateTransientFTbl[] =
{
    _DHCPV6Ia_SubStateErrorTransient,
};
// ERROR_FATAL substate processing function table
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   _DHCPV6Ia_SubStateErrorFatal(TCPIP_DHCPV6_IA_DCPT* pIa);

static const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC _DHCPV6Ia_SubStateFatalFTbl[] =
{
    _DHCPV6Ia_SubStateErrorFatal,
};



// table with substate processing functions for each run state
//
static const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC*    _DHCPV6Ia_StateProcTbl[TCPIP_DHCPV6_IA_STATE_NUMBER] = 
{
    // *subStateFnc
    // 
    _DHCPV6Ia_SubStateSolicitFTbl,        // TCPIP_DHCPV6_IA_STATE_SOLICIT
    _DHCPV6Ia_SubStateRequestFTbl,        // TCPIP_DHCPV6_IA_STATE_REQUEST   
    _DHCPV6Ia_SubStateDadFTbl,            // TCPIP_DHCPV6_IA_STATE_DAD
    _DHCPV6Ia_SubStateDeclineFTbl,        // TCPIP_DHCPV6_IA_STATE_DECLINE
    _DHCPV6Ia_SubStateBoundFTbl,          // TCPIP_DHCPV6_IA_STATE_BOUND
    _DHCPV6Ia_SubStateRenewFTbl,          // TCPIP_DHCPV6_IA_STATE_RENEW
    _DHCPV6Ia_SubStateRebindFTbl,         // TCPIP_DHCPV6_IA_STATE_REBIND
    _DHCPV6Ia_SubStateConfirmFTbl,        // TCPIP_DHCPV6_IA_STATE_CONFIRM
    _DHCPV6Ia_SubStateReleaseFTbl,        // TCPIP_DHCPV6_IA_STATE_RELEASE
    _DHCPV6Ia_SubStateTransientFTbl,      // TCPIP_DHCPV6_IA_STATE_ERROR_TRANSIENT
    _DHCPV6Ia_SubStateFatalFTbl,          // TCPIP_DHCPV6_IA_STATE_ERROR_FATAL
};


// task execution function for client
// for errors it updates the pClient->state
typedef void (*TCPIP_DHCPV6_CLIENT_TASK_FNC)(TCPIP_DHCPV6_CLIENT_DCPT* pClient);

static void         _DHCPV6Client_TransmitTask(TCPIP_DHCPV6_CLIENT_DCPT* pClient);
static void         _DHCPV6Client_ReceiveTask(TCPIP_DHCPV6_CLIENT_DCPT* pClient);
static void         _DHCPV6Client_ProcessTask(TCPIP_DHCPV6_CLIENT_DCPT* pClient);


// table with the client tasks funtions
static const TCPIP_DHCPV6_CLIENT_TASK_FNC _DHCPV6Client_TaskTbl[] = 
{
    _DHCPV6Client_TransmitTask,
    _DHCPV6Client_ReceiveTask,
    _DHCPV6Client_ProcessTask,
};


// prototypes
// general functions
static void         TCPIP_DHCPV6_Process(bool isTmo);

static void         _DHCPV6Close(TCPIP_NET_IF* pNetIf, bool disable, bool release);

static UDP_SOCKET   _DHCPV6OpenSocket(TCPIP_NET_IF* pNetIf);

static void         _DHCPV6Enable(TCPIP_NET_IF* pNetIf, int opType);

// client specific functions
static bool         _DHCPV6Client_Init(TCPIP_DHCPV6_CLIENT_DCPT* pClient);

static void         _DHCPV6Client_LinkUp(TCPIP_DHCPV6_CLIENT_DCPT* pClient);

static bool         _DHCPV6Client_CheckConnEvent(TCPIP_DHCPV6_CLIENT_DCPT* pClient);

static void         _DHCPV6Client_Notify(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_IA_DCPT* pIa, bool iaSubNotify);

static void         _DHCPV6Client_SetState(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_CLIENT_STATE cliState);

static bool         _DHCPV6Client_FindIA(TCPIP_DHCPV6_CLIENT_DCPT* pClient, uint32_t iaid, TCPIP_DHCPV6_IA_TYPE type, TCPIP_DHCPV6_IA_STATE srchState);

// IA specific functions
static void         _DHCPV6Ia_TaskExecute(TCPIP_DHCPV6_CLIENT_DCPT* pClient);

static TCPIP_DHCPV6_IA_SUBSTATE_RESULT _DHCPV6Ia_TxMsgSetup(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_CLIENT_MSG_TYPE cliMsgType);

static int          _DHCPV6Ia_MessageWrite(TCPIP_DHCPV6_IA_DCPT* pIa);

static TCPIP_DHCPV6_MSG_TX_RESULT   _DHCPV6Ia_CheckMsgTransmitStatus(TCPIP_DHCPV6_IA_DCPT* pIa);

static void         _DHCPV6Ia_IdGenerate(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_NET_IF* pIaidIf, TCPIP_DHCPV6_IA_ROUND* pIaRound);

static TCPIP_DHCPV6_IA_SUBSTATE_RESULT  _DHCPV6Ia_ReplyMsgSrvMatchProcess(TCPIP_DHCPV6_IA_DCPT* pIa);

static TCPIP_DHCPV6_IA_SUBSTATE_RESULT  _DHCPV6Ia_ReplyConfirmProcess(TCPIP_DHCPV6_IA_DCPT* pIa);

static TCPIP_DHCPV6_IA_SUBSTATE_RESULT  _DHCPV6Ia_ReplyRebindProcess(TCPIP_DHCPV6_IA_DCPT* pIa);

static TCPIP_DHCPV6_IA_SUBSTATE_RESULT  _DHCPV6Ia_AdvertiseSelect(TCPIP_DHCPV6_IA_DCPT* pIa, bool isTmo);

static TCPIP_DHCPV6_IA_SUBSTATE_RESULT  _DHCPV6Ia_AdvertiseCopy(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_IA_DCPT* pIa);

static void         _DHCPV6Ia_SolicitInit(TCPIP_DHCPV6_IA_DCPT* pIa);

static void         _DHCPV6Ia_ListPurge(SINGLE_LIST* pL, TCPIP_DHCPV6_IA_DCPT* pIa, bool forcePurge);

static void         _DHCPV6Ia_SetState(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_IA_STATE iaState, TCPIP_DHCPV6_IA_SUBSTATE iaSubState);

static void         _DHCPV6Ia_SetTimeValues(TCPIP_DHCPV6_IA_DCPT* pIa, bool setAcqTime);

static void         _DHCPV6Ia_RestoreTimeValues(TCPIP_DHCPV6_IA_DCPT* pIa);

static void         _DHCPV6Ia_SetBoundTimes(TCPIP_DHCPV6_IA_DCPT* pIa);

static bool         _DHCPV6Ia_AddressIsExpired(TCPIP_DHCPV6_IA_DCPT* pIa, bool checkPrefLTime);

static void         _DHCPV6Ia_Remove(TCPIP_DHCPV6_IA_DCPT* pIa);

static void*        _DHCPV6Ia_InMsgBuffer(TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, TCPIP_DHCPV6_IA_DCPT* pIa, bool serverMatch);

static void         _DHCPV6Ia_MsgInvalidate(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer);


// IA Options specific functions
static int          _DHCPV6Option_WriteDuid(TCPIP_DHCPV6_DUID_DCPT* pDuid, TCPIP_DHCPV6_MSG_WRITE_DCPT* pWrDcpt);

static int          _DHCPV6Option_WriteId(TCPIP_DHCPV6_DUID_DCPT* pDuid, TCPIP_DHCPV6_OPTION_CODE optCode, TCPIP_DHCPV6_MSG_WRITE_DCPT* pWrDcpt);

static int          _DHCPV6Option_WriteIA_Addr(TCPIP_DHCPV6_OPTION_IA_ADDR_BODY* pASpec, TCPIP_DHCPV6_MSG_WRITE_DCPT* pWrDcpt);


// Option sets

static TCPIP_DHCPV6_OPTION_CODE _DHCPV6OptionSet_ExtractCode(const uint32_t* pOptionSet, int nSets, const uint32_t** ppCurrSet, int* pCurrMask);

static int          _DHCPV6OptionSet_CodesNo(const uint32_t* pOptionSet, int nSets);


// Generic Option find functions
// 
// find match function
// returns true when the passed in option matches its criteria
// the optCode and optLen should be ntoh converted
typedef bool (*_DHCPV6OptionMatchFnc)(TCPIP_DHCPV6_OPTION_GENERIC* pOptG, TCPIP_DHCPV6_OPTION_CODE srchCode, uint16_t optCode, uint16_t optLen);

static TCPIP_DHCPV6_OPTION_GENERIC*     _DHCPV6OptionFind_OptCode(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_OPTION_CODE srchCode, _DHCPV6OptionMatchFnc findFnc);
static TCPIP_DHCPV6_OPTION_GENERIC*     _DHCPV6OptionFind_OptCode_Iter(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_OPTION_CODE srchCode, _DHCPV6OptionMatchFnc findFnc, TCPIP_DHCPV6_MSG_SEARCH_DCPT* pSrchDcpt);

// Specific Option match functions

// match for any IA type: IATA or IANA
static bool                             _DHCPV6OptionMatchFnc_IA(TCPIP_DHCPV6_OPTION_GENERIC* pOptG, TCPIP_DHCPV6_OPTION_CODE srchCode, uint16_t optCode, uint16_t optLen);

// Specific Option find functions

static void*                            _DHCPV6OptionFind_Ia(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_IA_DCPT* pIa, bool serverMatch);

static TCPIP_DHCPV6_OPTION_GENERIC*     _DHCPV6OptionFind_IaOpt(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_OPTION_CODE srchOpt);

static bool                             _DHCPV6OptionFind_RapidCommit(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer);


static TCPIP_DHCPV6_OPTION_UNICAST*     _DHCPV6OptionFind_ServerUnicast(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer);



// Option get
// 
static bool         _DHCPV6OptionGet_Duid(TCPIP_DHCPV6_DUID_DCPT* pDstDuidDcpt, TCPIP_DHCPV6_OPTION_ID* pSrcOptId);

static bool         _DHCPV6OptionGet_IaAddress(TCPIP_DHCPV6_OPTION_IA_ADDR_BODY* pAddBody, TCPIP_DHCPV6_OPTION_IA_ADDR* pSrcIaAddr);

static bool         _DHCPV6OptionGet_IaBody(TCPIP_DHCPV6_IA_BODY* pIaBody, void* pSrcOptIa, TCPIP_DHCPV6_IA_TYPE type);



// Option get from messages
static bool         _DHCPV6MsgGet_Duid(TCPIP_DHCPV6_DUID_DCPT* pDstDuid, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, bool isClient);

static bool         _DHCPV6MsgGet_IaBody(TCPIP_DHCPV6_IA_BODY* pIaBody, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, TCPIP_DHCPV6_IA_DCPT* pIa, bool serverMatch);

static bool         _DHCPV6MsgGet_IaOptBuffer(TCPIP_DHCPV6_MSG_BUFFER* pDstBuffer, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, TCPIP_DHCPV6_IA_DCPT* pIa);

static bool         _DHCPV6MsgGet_IaAddress(TCPIP_DHCPV6_OPTION_IA_ADDR_BODY* pAddBody, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, TCPIP_DHCPV6_IA_DCPT* pIa);

static int16_t      _DHCPV6MsgGet_ServerPref(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer);

static uint32_t     _DHCPV6MsgGet_TransactionId(TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer);

static TCPIP_DHCPV6_SERVER_STATUS_CODE _DHCPV6MsgGet_StatusCode(uint8_t* statusBuffer, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, uint16_t statusBufferSize, uint16_t* pStatusMsgSize);

static void         _DHCPV6MsgGet_Options(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer);

static TCPIP_DHCPV6_SERVER_STATUS_CODE _DHCPV6MsgGet_IaOptStatusCode(uint8_t* statusBuffer, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, TCPIP_DHCPV6_IA_DCPT* pIa, uint16_t statusBufferSize, uint16_t* pStatusMsgSize);

static bool         _DHCPV6MsgGet_LeaseParams(TCPIP_DHCPV6_IA_DCPT* pDstIa, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, bool serverMatch);

// retrieving options from a server message


typedef struct _tag_DHCPV6MsgGet_OptionEntry
{
    TCPIP_DHCPV6_OPTION_CODE    optCode;
    void(*optFnc)(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, const struct _tag_DHCPV6MsgGet_OptionEntry* pEntry);
}_DHCPV6MsgGet_OptionEntry;

static void _DHCPV6MsgGet_DnsServers(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, const _DHCPV6MsgGet_OptionEntry* pEntry);
static void _DHCPV6MsgGet_DomainList(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, const _DHCPV6MsgGet_OptionEntry* pEntry);
static void _DHCPV6MsgGet_SolMaxRt(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, const _DHCPV6MsgGet_OptionEntry* pEntry);
static void _DHCPV6MsgGet_InfoMaxRt(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, const _DHCPV6MsgGet_OptionEntry* pEntry);


// table with server transmitted options that this client processes
// if retrieveing an option fails, the option is silently ignored
// Table entries having optFnc == 0 are unused
static const _DHCPV6MsgGet_OptionEntry    _DHCPV6MsgGet_OptionRetrieveTbl[] = 
{
    // optCode                              // optFnc
    {TCPIP_DHCPV6_OPT_CODE_DNS_SERVERS,     _DHCPV6MsgGet_DnsServers},
    {TCPIP_DHCPV6_OPT_CODE_DOMAIN_LIST,     _DHCPV6MsgGet_DomainList},
    {TCPIP_DHCPV6_OPT_CODE_MAX_RT,          _DHCPV6MsgGet_SolMaxRt},
    {TCPIP_DHCPV6_OPT_CODE_INFO_MAX_RT,     _DHCPV6MsgGet_InfoMaxRt},

};


// Other helpers
//
static int          _DHCPV6MsgFind_InUseIAs(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_IA_DCPT* pIa);

static bool         _DHCPV6Duid_Generate(TCPIP_DHCPV6_DUID_TYPE duidType, TCPIP_DHCPV6_DUID_DCPT* pDuid, TCPIP_NET_IF* pDuidIf);

static bool         _DHCPV6Duid_Compare(const TCPIP_DHCPV6_DUID_DCPT* pDuid1, const TCPIP_DHCPV6_DUID_DCPT* pDuid2);

static bool         _DHCPV6MsgValidate(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_CLIENT_DCPT* pClient);


// general utilities
//
static __inline__ uint32_t __attribute__((always_inline)) _DHCPV6SecondCountGet(void)
{
    return dhcpv6SecondCount;
}

static __inline__ void __attribute__((always_inline)) _DHCPV6SecondCountSet(void)
{
    // use a 64 bit count to avoid roll over
    dhcpv6SecondCount = SYS_TMR_SystemCountGet() / SYS_TMR_SystemCountFrequencyGet(); 
}


// basic level  debugging
#if ((TCPIP_DHCPV6_DEBUG_LEVEL & TCPIP_DHCPV6_DEBUG_MASK_BASIC) != 0)
volatile int _DHCPV6StayAssertLoop = 0;
static void _DHCPV6AssertCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("DHCPv6 Assert: %s, in line: %d, \r\n", message, lineNo);
        while(_DHCPV6StayAssertLoop != 0);
    }
}
// a debug condition, not really assertion
volatile int _DHCPV6StayCondLoop = 0;
static void _DHCPV6DbgCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("DHCPv6 Cond: %s, in line: %d, \r\n", message, lineNo);
        while(_DHCPV6StayCondLoop != 0);
    }
}

#else
#define _DHCPV6AssertCond(cond, message, lineNo)
#define _DHCPV6DbgCond(cond, message, lineNo)
#endif  // (TCPIP_DHCPV6_DEBUG_LEVEL)

// level In debugging
#if ((TCPIP_DHCPV6_DEBUG_LEVEL & TCPIP_DHCPV6_DEBUG_MASK_IN) != 0)

static void _DHCPV6DbgDUID_Print(TCPIP_DHCPV6_DUID_DCPT* pDuid)
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
                pBuff = duidBuff + sprintf(duidBuff, "%s", hdr);
                TCPIP_Helper_MACAddressToString((TCPIP_MAC_ADDR*)pDuid->duidBody.duidLL.ll_address, pBuff, endBuff - pBuff);
            }
            break;


        case TCPIP_DHCPV6_DUID_TYPE_LLT:
            {
                const char hdr[] = "DUID LLT - ";
                pBuff = duidBuff + sprintf(duidBuff, "%s", hdr);
                TCPIP_Helper_MACAddressToString((TCPIP_MAC_ADDR*)pDuid->duidBody.duidLL.ll_address, pBuff, endBuff - pBuff);
            }
            break;

        case TCPIP_DHCPV6_DUID_TYPE_EN:
            {
                int ix;
                int identLen = pDuid->duidLen - sizeof(((TCPIP_DHCPV6_DUID_EN*)0)->duid_type) - sizeof(((TCPIP_DHCPV6_DUID_EN*)0)->ent_number);
                pBuff = duidBuff + sprintf(duidBuff, "DUID EN: no: 0x%8x, ident: 0x", pDuid->duidBody.duidEN.ent_number);

                uint8_t* pIdent = pDuid->duidBody.duidEN.identifier;
                for(ix = 0; ix < identLen; ix++)
                {
                    pBuff += sprintf(pBuff, "%02x", *pIdent++);
                }
            }
            break;

        default:
            sprintf(duidBuff, "DUID: invalid");
            break;
    }

    SYS_CONSOLE_PRINT("%s", duidBuff);
}

static void _DHCPV6DbgValidate_Print(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_MSG_VALID_MASK msgMask, TCPIP_DHCPV6_MSG_TYPE msgType, TCPIP_DHCPV6_DUID_DCPT* pSrvDuid)
{
    char dhcpBuff[80];

    if(pSrvDuid != 0)
    {
        if((msgMask & TCPIP_DHCPV6_VALID_MASK_TYPE) != 0)
        {
            sprintf(dhcpBuff, "DHCPV6_V Msg: 0x%8x, type: %d from server: ", (uint32_t)pMsgBuffer, msgType);
        }
        else
        {
            sprintf(dhcpBuff, "DHCPV6_V Msg: 0x%8x type: unknown from server: ", (uint32_t)pMsgBuffer);
        }
        SYS_CONSOLE_PRINT("%s", dhcpBuff);
        _DHCPV6DbgDUID_Print(pSrvDuid);
    }

    if(msgMask == TCPIP_DHCPV6_MSG_VALIDATION_MASK)
    {   // all's well
        sprintf(dhcpBuff, " Msg valid - mask: 0x%2x\r\n", msgMask);
    }
    else
    {   // failed
        sprintf(dhcpBuff, " Msg invalid - mask: 0x%2x\r\n", msgMask);
    }
    SYS_CONSOLE_PRINT("%s", dhcpBuff);
} 

static void _DHCPV6DbgMsgIn_PrintPassed(const char* task, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_MSG_TYPE msgType)
{
    char dhcpBuff[80];
    sprintf(dhcpBuff, "DHCPV6 task: %s, Msg: 0x%8x, type: %d, passed\r\n", task, (uint32_t)pMsgBuffer, msgType);
    SYS_CONSOLE_PRINT("%s", dhcpBuff);
}

static void _DHCPV6DbgMsgIn_PrintFailed(const char* task, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_MSG_TYPE msgType, const char* reason)
{
    char dhcpBuff[80];
    sprintf(dhcpBuff, "DHCPV6 task: %s, Msg: 0x%8x, type: %d, failed: %s\r\n", task, (uint32_t)pMsgBuffer, msgType, reason);
    SYS_CONSOLE_PRINT("%s", dhcpBuff);
}

static void _DHCPV6DbgIAIn_PrintPassed(const char* task, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_IA_DCPT* pIa, uint32_t param)
{
    char dhcpBuff[80];
    sprintf(dhcpBuff, "DHCPV6 task: %s, Msg: 0x%8x, IA: %d, state: %d, param: 0x%x\r\n", task, (uint32_t)pMsgBuffer, pIa->parentIx, pIa->iaState, param);
    SYS_CONSOLE_PRINT("%s", dhcpBuff);
}

static void _DHCPV6DbgIAIn_PrintFailed(const char* task, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_IA_DCPT* pIa, const char* reason)
{
    char dhcpBuff[80];
    sprintf(dhcpBuff, "DHCPV6 task: %s, Msg: 0x%8x, IA: %d, state: %d, failed: %s\r\n", task, (uint32_t)pMsgBuffer, pIa->parentIx, pIa->iaState, reason);
    SYS_CONSOLE_PRINT("%s", dhcpBuff);
}

#define     _DHCPV6DbgDeclareFailReason(reason) const char* reason = 0
#define     _DHCPV6DbgSetFailReason(reason, msg) do{reason = msg;}while(0)


#else
#define     _DHCPV6DbgValidate_Print(pMsgBuffer, msgMask, msgType, pSrvDuid) 
#define     _DHCPV6DbgMsgIn_PrintPassed(task, pMsgBuffer, msgType)
#define     _DHCPV6DbgMsgIn_PrintFailed(task, pMsgBuffer, msgType, reason)
#define     _DHCPV6DbgIAIn_PrintPassed(task, pMsgBuffer, pIa, param)
#define     _DHCPV6DbgIAIn_PrintFailed(task, pMsgBuffer, pIa, reason)
#define     _DHCPV6DbgDeclareFailReason(reason)
#define     _DHCPV6DbgSetFailReason(reason, msg)
#endif  // (TCPIP_DHCPV6_DEBUG_LEVEL)

// level Out debugging
#if ((TCPIP_DHCPV6_DEBUG_LEVEL & TCPIP_DHCPV6_DEBUG_MASK_OUT) != 0)
static void _DHCPV6DbgMsgOut_PrintPassed(const char* task, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_MSG_TYPE msgType, TCPIP_DHCPV6_IA_DCPT* pIa)
{
    char dhcpBuff[80];
    sprintf(dhcpBuff, "DHCPV6 task: %s, Msg: 0x%8x, type: %d, IA ix: %d, state: %d, passed\r\n", task, (uint32_t)pMsgBuffer, msgType, pIa->parentIx, pIa->iaState);
    SYS_CONSOLE_PRINT("%s", dhcpBuff);
}

static void _DHCPV6DbgMsgOut_PrintFailed(const char* task, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_MSG_TYPE msgType, TCPIP_DHCPV6_IA_DCPT* pIa, const char* reason)
{
    char dhcpBuff[128];
    sprintf(dhcpBuff, "DHCPV6 task: %s, Msg: 0x%8x, type: %d, IA ix: %d, state: %d, failed: %s\r\n", task, (uint32_t)pMsgBuffer, msgType, pIa->parentIx, pIa->iaState, reason);
    SYS_CONSOLE_PRINT("%s", dhcpBuff);
}
#else
#define _DHCPV6DbgMsgOut_PrintPassed(task, pMsgBuffer, msgType, pIa)
#define _DHCPV6DbgMsgOut_PrintFailed(task, pMsgBuffer, msgType, pIa, reason)
#endif  // (TCPIP_DHCPV6_DEBUG_LEVEL)


// client state debugging
#if ((TCPIP_DHCPV6_DEBUG_LEVEL & TCPIP_DHCPV6_DEBUG_MASK_CLIENT_STATE) != 0) || (((TCPIP_DHCPV6_DEBUG_LEVEL & TCPIP_DHCPV6_DEBUG_MASK_CLIENT_NOTIFY_STATE) != 0) && (_TCPIP_DHCPV6_USER_NOTIFICATION != 0))
static const char* _DHCPV6_CLIENT_STATE_NAME[TCPIP_DHCPV6_CLIENT_STATE_NUMBER] = 
{
    "init",         // TCPIP_DHCPV6_CLIENT_STATE_INIT
    "idle",         // TCPIP_DHCPV6_CLIENT_STATE_IDLE    
    "run",          // TCPIP_DHCPV6_CLIENT_STATE_RUN
    "wlink"         // TCPIP_DHCPV6_CLIENT_STATE_WAIT_LINK
};

static void     _DHCPV6DbgStatePrint_Client(TCPIP_DHCPV6_CLIENT_DCPT* pClient, bool ignoreCurrent)
{
    if(ignoreCurrent || pClient->state != pClient->prevState)
    {
        char dhcpBuff[80];
        uint32_t currTime = _DHCPV6SecondCountGet();
        sprintf(dhcpBuff, "DHCPV6 Client - state: %s, time: %d\r\n", _DHCPV6_CLIENT_STATE_NAME[pClient->state], currTime);
        SYS_CONSOLE_PRINT("%s", dhcpBuff);

#if ((TCPIP_DHCPV6_DEBUG_LEVEL & TCPIP_DHCPV6_DEBUG_MASK_CLIENT_STATS) != 0)
        int nFree = TCPIP_Helper_SingleListCount(&pClient->buffFreeList);
        int nRx = TCPIP_Helper_SingleListCount(&pClient->rxMsgList);
        int nTx = TCPIP_Helper_SingleListCount(&pClient->txMsgList);
        int nAdv = TCPIP_Helper_SingleListCount(&pClient->advertiseList);
        int nReply = TCPIP_Helper_SingleListCount(&pClient->replyList);
        sprintf(dhcpBuff, "DHCPV6 buffers: %d, free: %d, rx: %d, tx: %d, adv: %d, reply: %d\r\n", pClient->nMsgBuffers, nFree, nRx, nTx, nAdv, nReply);
        SYS_CONSOLE_PRINT("%s", dhcpBuff);

        sprintf(dhcpBuff, "DHCPV6 stats - tx Fail: %d, tx Space: %d, tx Flush: %d, rx Fail: %d, rx Small: %d\r\n", pClient->txBuffFailCnt, pClient->txSktSpaceCnt, pClient->txSktFlushFailCnt, pClient->rxBuffFailCnt, pClient->rxBuffSmallCnt);    
        SYS_CONSOLE_PRINT("%s", dhcpBuff);
#endif  // ((TCPIP_DHCPV6_DEBUG_LEVEL & TCPIP_DHCPV6_DEBUG_MASK_CLIENT_STATS) != 0)

        pClient->prevState = pClient->state;
    }
    
}

#else
#define         _DHCPV6DbgStatePrint_Client(pClient, ignoreCurrent)
#endif  // ((TCPIP_DHCPV6_DEBUG_LEVEL & TCPIP_DHCPV6_DEBUG_MASK_CLIENT_STATE) != 0) || (((TCPIP_DHCPV6_DEBUG_LEVEL & TCPIP_DHCPV6_DEBUG_MASK_CLIENT_NOTIFY_STATE) != 0) && (_TCPIP_DHCPV6_USER_NOTIFICATION != 0))


// IA state debugging
#if ((TCPIP_DHCPV6_DEBUG_LEVEL & (TCPIP_DHCPV6_DEBUG_MASK_IA_STATE | TCPIP_DHCPV6_DEBUG_MASK_IA_SUBSTATE)) != 0) && (_TCPIP_DHCPV6_USER_NOTIFICATION != 0)
static const char* _DHCPV6_IA_STATE_NAME[TCPIP_DHCPV6_IA_STATE_NUMBER] = 
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

static const char* _DHCPV6_IA_TYPE_NAME[TCPIP_DHCPV6_IA_TYPE_NUMBER] = 
{
    "none",         // TCPIP_DHCPV6_IA_TYPE_NONE
    "iana",         // TCPIP_DHCPV6_IA_TYPE_IANA
    "iata",         // TCPIP_DHCPV6_IA_TYPE_IATA
};

#if ((TCPIP_DHCPV6_DEBUG_LEVEL & TCPIP_DHCPV6_DEBUG_MASK_IA_SUBSTATE) != 0)
static const char* _DHCPV6_IA_SUBSTATE_NAME[TCPIP_DHCPV6_IA_SUBSTATE_NUMBER] = 
{
    "start",        // TCPIP_DHCPV6_IA_SUBSTATE_START
    "idelay",       // TCPIP_DHCPV6_IA_SUBSTATE_IDELAY
    "transmit",     // TCPIP_DHCPV6_IA_SUBSTATE_TRANSMIT
    "wait-reply",   // TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY
};
#endif  // ((TCPIP_DHCPV6_DEBUG_LEVEL & TCPIP_DHCPV6_DEBUG_MASK_IA_SUBSTATE) != 0)

static void     _DHCPV6DbgStatePrint_Ia(TCPIP_DHCPV6_IA_DCPT* pIa, bool iaSubNotify)
{
    if(pIa != 0)
    {
        char dhcpBuff[80];
#if ((TCPIP_DHCPV6_DEBUG_LEVEL & TCPIP_DHCPV6_DEBUG_MASK_IA_SUBSTATE) != 0)
        sprintf(dhcpBuff, "DHCPV6 IA: %s, ix: %d, state: %s, sub-state: %s\r\n",_DHCPV6_IA_TYPE_NAME[pIa->iaBody.type], pIa->parentIx, _DHCPV6_IA_STATE_NAME[pIa->iaState], _DHCPV6_IA_SUBSTATE_NAME[pIa->iaSubState]);
        SYS_CONSOLE_PRINT("%s", dhcpBuff);
#else
        if(iaSubNotify == 0)
        {   // report only state changes
            sprintf(dhcpBuff, "DHCPV6 IA: %s, ix: %d, state: %s\r\n",_DHCPV6_IA_TYPE_NAME[pIa->iaBody.type], pIa->parentIx, _DHCPV6_IA_STATE_NAME[pIa->iaState]);
            SYS_CONSOLE_PRINT("%s", dhcpBuff);
        }
#endif  // TCPIP_DHCPV6_DEBUG_LEVEL
    }
}

#else
#define         _DHCPV6DbgStatePrint_Ia(pIa, iaSubNotify)
#endif  // ((TCPIP_DHCPV6_DEBUG_LEVEL & (TCPIP_DHCPV6_DEBUG_MASK_IA_STATE | TCPIP_DHCPV6_DEBUG_MASK_IA_SUBSTATE)) != 0) && (_TCPIP_DHCPV6_USER_NOTIFICATION != 0)

// additional status prints
#if ((TCPIP_DHCPV6_DEBUG_LEVEL & TCPIP_DHCPV6_DEBUG_MASK_ADD_STATE) != 0)
#define         _DHCPV6DbgStatePrint_Add(fmt, ...)  SYS_CONSOLE_PRINT(fmt, ##__VA_ARGS__)
#else
#define         _DHCPV6DbgStatePrint_Add(fmt, ...)
#endif // (TCPIP_DHCPV6_DEBUG_LEVEL)

// static lists debugging
#if ((TCPIP_DHCPV6_DEBUG_LEVEL & TCPIP_DHCPV6_DEBUG_MASK_LISTS) != 0)
SINGLE_LIST otherIaList = { 0 };
SINGLE_LIST thisIaList = { 0 };
SINGLE_LIST thisIaPriList = { 0 };
SINGLE_LIST discardIaList = { 0 };
#define _DHCPV6DbgDeclareList(aList)
#define _DHCPV6DbgCheckEmptyList(pList) do{_DHCPV6DbgCond(TCPIP_Helper_SingleListIsEmpty(pList), __func__, __LINE__);}while(0)
#else
#define _DHCPV6DbgDeclareList(aList) SINGLE_LIST aList = { 0 } 
#define _DHCPV6DbgCheckEmptyList(pList)
#endif  // (TCPIP_DHCPV6_DEBUG_LEVEL)


// buffer trace debugging
#if ((TCPIP_DHCPV6_DEBUG_LEVEL & TCPIP_DHCPV6_DEBUG_MASK_BUFF_TRACE) != 0)
#define _DHCPV6_BUFF_STATE_TRACE_SIZE  20
#define _DHCPV6_BUFFERS_TO_TRACE        2

typedef struct
{
    TCPIP_DHCPV6_IA_STATE    iaState;
    TCPIP_DHCPV6_IA_SUBSTATE iaSubState;
}_DHCPV6_BUFF_TRACE_STATE;


typedef struct
{
    TCPIP_DHCPV6_MSG_BUFFER* pBuffer;
    int                      stateIx;   // current slot
    _DHCPV6_BUFF_TRACE_STATE traceState[_DHCPV6_BUFF_STATE_TRACE_SIZE];
}_DHCPV6_BUFF_TRACE_DCPT;

_DHCPV6_BUFF_TRACE_DCPT _dhcpv6BuffTrace[_DHCPV6_BUFFERS_TO_TRACE];

static volatile int _dhcpv6TraceCond = 1;

static void _DHCPV6TraceBuffInit(int buffIx, TCPIP_DHCPV6_MSG_BUFFER* pBuffer)
{
    memset(_dhcpv6BuffTrace + buffIx, 0, sizeof(_DHCPV6_BUFF_TRACE_DCPT));
    _dhcpv6BuffTrace[buffIx].pBuffer = pBuffer;
} 


void _DHCPV6TraceBuff(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_MSG_BUFFER* pBuffer)
{
    int buffIx;
    _DHCPV6_BUFF_TRACE_DCPT* pTrace = _dhcpv6BuffTrace;
    for(buffIx = 0; buffIx < sizeof(_dhcpv6BuffTrace)/sizeof(*_dhcpv6BuffTrace); buffIx++, pTrace++)
    {
        if(pTrace->pBuffer == pBuffer)
        {   // found it
            _DHCPV6_BUFF_TRACE_STATE* pTraceState = pTrace->traceState + pTrace->stateIx;
            pTraceState->iaState = pIa->iaState;
            pTraceState->iaSubState = pIa->iaSubState;
            pTrace->stateIx++;
            if(pTrace->stateIx == _DHCPV6_BUFF_STATE_TRACE_SIZE)
            {
                pTrace->stateIx = 0;
            }
            return;
        }
    }

    while(_dhcpv6TraceCond);

}
#else
#define     _DHCPV6TraceBuff(pIa, pBuffer)
#define     _DHCPV6TraceBuffInit(buffIx, pBuffer)
#endif  // (TCPIP_DHCPV6_DEBUG_LEVEL)





#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void _DHCPV6Cleanup(int nClients)
{
    if(dhcpv6ClientDcpt)
    {
        int ix;

        TCPIP_DHCPV6_CLIENT_DCPT* pClient = dhcpv6ClientDcpt;
        for(ix = 0; ix < nClients; ix++, pClient++)
        {
            if(pClient->msgBuffers)
            {
                TCPIP_HEAP_Free(dhcpv6MemH, pClient->msgBuffers);
            }

            if(pClient->iaArray)
            {
                TCPIP_HEAP_Free(dhcpv6MemH, pClient->iaArray);
            }
        }

        TCPIP_HEAP_Free(dhcpv6MemH, dhcpv6ClientDcpt);
        dhcpv6ClientDcpt = 0;
    }

#if (_TCPIP_DHCPV6_USER_NOTIFICATION != 0)
    TCPIP_Notification_Deinitialize(&dhcpv6RegisteredUsers, dhcpv6MemH);
#endif  // (_TCPIP_DHCPV6_USER_NOTIFICATION != 0)

    if(dhcpv6SignalHandle)
    {
        _TCPIPStackSignalHandlerDeregister(dhcpv6SignalHandle);
        dhcpv6SignalHandle = 0;
    }
}
#else
#define _DHCPV6Cleanup(nClients)
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)



// API
//
bool TCPIP_DHCPV6_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_DHCPV6_MODULE_CONFIG* pDhcpConfig)
{
    int     netIx;
    TCPIP_DHCPV6_CLIENT_DCPT*   pClient;
    
    if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        {
            _DHCPV6Enable(stackCtrl->pNetIf, 0);
        }
        return true;
    }

    // stack init
    
    // check init data is not missing
    if(pDhcpConfig == 0)
    {
        return false;
    }

    // sanity checks
    //
    if(pDhcpConfig->duidType != TCPIP_DHCPV6_DUID_TYPE_LL)
    {
        return false;
    }

    if(pDhcpConfig->nIanaDcpts == 0 && pDhcpConfig->nIataDcpts == 0)
    {
        return false;
    }

    if(pDhcpConfig->nMsgBuffers == 0 || pDhcpConfig->msgBufferSize < TCPIP_DHCPV6_MESSAGE_MIN_BUFFER_SIZE)
    {
        return false;
    }

    if(pDhcpConfig->defaultIanaT1 > 0 && pDhcpConfig->defaultIanaT2 > 0)
    {
        if(pDhcpConfig->defaultIanaT1 > pDhcpConfig->defaultIanaT2)
        {
            return false;
        }
    }

    if(pDhcpConfig->defaultIataT1 > 0 && pDhcpConfig->defaultIataT2 > 0)
    {
        if(pDhcpConfig->defaultIataT1 > pDhcpConfig->defaultIataT2)
        {
            return false;
        }
    }

    if(pDhcpConfig->ianaSolicitT1 > 0 && pDhcpConfig->ianaSolicitT2 > 0)
    {
        if(pDhcpConfig->ianaSolicitT1 > pDhcpConfig->ianaSolicitT2)
        {
            return false;
        }
    }
    
    const TCPIP_DHCPV6_OPTION_MASK_SET_0* pMsgMaskOpt = DHCPV6_MSG_OPTION_MASK_TBL;
    TCPIP_DHCPV6_OPTION_MASK_SET_0* pMsgMaskCurrOpt = DHCPV6_MSG_OPTION_CURR_MASK_TBL;
    
    // copying the default DHCPV6_MSG_OPTION_MASK_TBL content to DHCPV6_MSG_OPTION_CURR_MASK_TBL
    memcpy(pMsgMaskCurrOpt,pMsgMaskOpt,sizeof(DHCPV6_MSG_OPTION_CURR_MASK_TBL));
    

    if(dhcpv6InitCount == 0)
    {   // first time we're run
        // store the memory allocation handle

        dhcpv6MemH = stackCtrl->memH;

        dhcpv6ClientDcpt = (TCPIP_DHCPV6_CLIENT_DCPT*)TCPIP_HEAP_Calloc(dhcpv6MemH,  stackCtrl->nIfs, sizeof(TCPIP_DHCPV6_CLIENT_DCPT));
        if(dhcpv6ClientDcpt == 0)
        {   // failed
            return false;
        }

        // create the DHCP signal handler
        dhcpv6SignalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_DHCPV6_Task, TCPIP_DHCPV6_TASK_TICK_RATE);
        if(dhcpv6SignalHandle == 0)
        {   // cannot create the DHCP timer
            TCPIP_HEAP_Free(dhcpv6MemH, dhcpv6ClientDcpt);
            return false;
        }
            
        pClient = dhcpv6ClientDcpt;
        for(netIx = 0; netIx < stackCtrl->nIfs; netIx++, pClient++)
        {
            pClient->nIanaDcpts = pDhcpConfig->nIanaDcpts;
            pClient->nIataDcpts = pDhcpConfig->nIataDcpts;
            pClient->nFreeDcpts = pDhcpConfig->nFreeDcpts;
            pClient->nIaDcpts = pClient->nIanaDcpts + pClient->nIataDcpts + pClient->nFreeDcpts;
            if((pClient->defaultIanaT1 = pDhcpConfig->defaultIanaT1) == 0)
            {
                pClient->defaultIanaT1 = TCPIP_DHCPV6_TIMEOUT_INFINITE;
            }
            if((pClient->defaultIanaT2 = pDhcpConfig->defaultIanaT2) == 0)
            {
                pClient->defaultIanaT2 = TCPIP_DHCPV6_TIMEOUT_INFINITE;
            }
            if((pClient->defaultIataT1 = pDhcpConfig->defaultIataT1) == 0)
            {
                pClient->defaultIataT1 = TCPIP_DHCPV6_TIMEOUT_INFINITE;
            }
            if((pClient->defaultIataT2 = pDhcpConfig->defaultIataT2) == 0)
            {
                pClient->defaultIataT2 = TCPIP_DHCPV6_TIMEOUT_INFINITE;
            }
            pClient->ianaSolicitT1 = pDhcpConfig->ianaSolicitT1;
            pClient->ianaSolicitT2 = pDhcpConfig->ianaSolicitT2; 
            pClient->solicitPrefLTime = pDhcpConfig->solicitPrefLTime;  
            pClient->solicitValidLTime = pDhcpConfig->solicitValidLTime; 

            pClient->nMsgBuffers = pDhcpConfig->nMsgBuffers;
            pClient->msgBufferSize =  pDhcpConfig->msgBufferSize;
            
            // allocate the IA
            pClient->iaArray = (TCPIP_DHCPV6_IA_DCPT*)TCPIP_HEAP_Calloc(dhcpv6MemH, pClient->nIaDcpts, sizeof(TCPIP_DHCPV6_IA_DCPT));
            // allocate the message buffers
            pClient->msgBuffers = (TCPIP_DHCPV6_MSG_BUFFER*)TCPIP_HEAP_Calloc(dhcpv6MemH, pClient->nMsgBuffers, sizeof(TCPIP_DHCPV6_MSG_BUFFER) + pClient->msgBufferSize);
#if (_TCPIP_DHCPV6_USER_NOTIFICATION != 0)
            // initialize the registration
            bool iniRes = TCPIP_Notification_Initialize(&dhcpv6RegisteredUsers);
            if(pClient->iaArray == 0 || pClient->msgBuffers == 0 || iniRes == false)
#else
            if(pClient->iaArray == 0 || pClient->msgBuffers == 0)
#endif  // (_TCPIP_DHCPV6_USER_NOTIFICATION != 0)
            {
                _DHCPV6Cleanup(stackCtrl->nIfs);
                return false;
            }

            pClient->prevState = -1;
        }
    }
            
    dhcpv6ClientPort = pDhcpConfig->dhcpCliPort;
    dhcpv6ServerPort = pDhcpConfig->dhcpSrvPort;

    pClient = dhcpv6ClientDcpt + stackCtrl->netIx;
    pClient->clientDuid.duidType = pDhcpConfig->duidType;
    pClient->pDhcpIf = stackCtrl->pNetIf;
    pClient->configFlags = pDhcpConfig->configFlags;


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
        _DHCPV6Close(stackCtrl->pNetIf, true, true);

        //  the registered users for this interface are not removed
        //  since this interface is closed there won't be any event generated on it anyway
        //  deallocation will wait for the whole stack to deinit 

        if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--dhcpv6InitCount == 0)
            {   // all closed
                // release resources
                _DHCPV6Cleanup(stackCtrl->nIfs);
                dhcpv6MemH = 0;
            }
        }
    }

}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)


void TCPIP_DHCPV6_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;
    bool                isTmo = false;

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if((sigPend & TCPIP_MODULE_SIGNAL_TMO) != 0)
    { // regular TMO occurred
        isTmo = true;
    }

    if(isTmo || (sigPend & TCPIP_MODULE_SIGNAL_RX_PENDING) != 0)
    {   // execute either for TMO or RX signal
        TCPIP_DHCPV6_Process(isTmo);
    }
}

// send a signal to the DHCP module that data is available
// no manager alert needed since this normally results as a higher layer (UDP) signal
static void _DHCPV6SocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_UDP_SIGNAL_RX_DATA)
    {
        _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}


static void TCPIP_DHCPV6_Process(bool isTmo)
{
    int                 netIx, nNets;
    TCPIP_NET_IF*       pNetIf;
    TCPIP_DHCPV6_CLIENT_DCPT* pClient;
    
    if(isTmo)
    {   // update DHCP time keeping
        _DHCPV6SecondCountSet();
    }

    nNets = TCPIP_STACK_NumberOfNetworksGet();
    for(netIx = 0; netIx < nNets; netIx++) 
    {
        pNetIf = (TCPIP_NET_IF*)TCPIP_STACK_IndexToNet (netIx);
        if(!TCPIP_STACK_NetworkIsUp(pNetIf))
        {   // inactive interface
            continue;
        }

        pClient = dhcpv6ClientDcpt + netIx; 

        (*_DHCPV6Client_StateFncTbl[pClient->state])(pClient);
        _DHCPV6DbgStatePrint_Client(pClient, false);
    }
}


// state processing functions

// TCPIP_DHCPV6_CLIENT_STATE_INIT
static void   _DHCPV6Client_StateProcInit(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    if(_DHCPV6Client_Init(pClient))
    {   // advance
        _DHCPV6Client_SetState(pClient, TCPIP_DHCPV6_CLIENT_STATE_IDLE);
    }
    // else wait some more
}

// TCPIP_DHCPV6_CLIENT_STATE_IDLE
static void   _DHCPV6Client_StateProcIdle(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    _DHCPV6Enable(pClient->pDhcpIf, 0);
}

// iaState processing functions
// TCPIP_DHCPV6_CLIENT_STATE_RUN
static void   _DHCPV6Client_StateProcRun(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    int     taskIx;

    // execute the client tasks
    for(taskIx = 0; taskIx < sizeof(_DHCPV6Client_TaskTbl)/sizeof(*_DHCPV6Client_TaskTbl) && pClient->state == TCPIP_DHCPV6_CLIENT_STATE_RUN; taskIx++)
    {
        (*_DHCPV6Client_TaskTbl[taskIx])(pClient);
    }

    // check the connection events
    if(_DHCPV6Client_CheckConnEvent(pClient))
    {   // execute the IA tasks
        _DHCPV6Ia_TaskExecute(pClient);
    }
}

static void _DHCPV6Client_StateProcWaitLink(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    // wait for the link to go up
    _DHCPV6Client_CheckConnEvent(pClient);
}

// processes the connection related events
// returns true if execution needs to continue (link is up)
// or false if the run state machine should not be executed (link is down)
static bool _DHCPV6Client_CheckConnEvent(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    // check the connection events
    TCPIP_MAC_EVENT currEvent = pClient->connEvent & (TCPIP_MAC_EV_CONN_LOST | TCPIP_MAC_EV_CONN_ESTABLISHED);
    bool taskExec = false;
    
    if(currEvent == 0)
    {   // no event
        taskExec = pClient->flags.connLost == 0;
    }
    else if(currEvent == TCPIP_MAC_EV_CONN_LOST)
    {
        if(pClient->flags.connLost == 0)
        {
            _DHCPV6Client_SetState(pClient, TCPIP_DHCPV6_CLIENT_STATE_WAIT_LINK);
        }
        pClient->flags.connLost = 1;
    }
    else if(currEvent == TCPIP_MAC_EV_CONN_ESTABLISHED)
    {
        // we should have received a CONN lost event
        _DHCPV6AssertCond(pClient->flags.connLost != 0, __func__, __LINE__);
        pClient->flags.connLost = 0;
        _DHCPV6Client_LinkUp(pClient);
        taskExec = true;
    }
    else
    {   // should not happen
        _DHCPV6AssertCond(false, __func__, __LINE__);
    }


    pClient->connEvent &= ~currEvent;
    return taskExec;
}

// connection established event occurred and the link is up
// All IAs in bound state move to CONFIRM
static void _DHCPV6Client_LinkUp(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    TCPIP_DHCPV6_IA_DCPT* pIa, *pIaNext;
    DOUBLE_LIST* pBoundList = pClient->iaStateList + TCPIP_DHCPV6_IA_STATE_BOUND;

    for(pIa = (TCPIP_DHCPV6_IA_DCPT*)pBoundList->head; pIa != 0; pIa = pIaNext)
    {
        pIaNext = pIa->next;

        _DHCPV6Ia_SetState(pIa, TCPIP_DHCPV6_IA_STATE_CONFIRM, 0);
    } 

    _DHCPV6Client_SetState(pClient, TCPIP_DHCPV6_CLIENT_STATE_RUN);
}


// generic
static void _DHCPV6Ia_TaskExecute(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    int listIx;
    TCPIP_DHCPV6_IA_DCPT* pIa, *pIaNext;


    for(listIx = 0; listIx < sizeof(pClient->iaStateList) / sizeof(*pClient->iaStateList); listIx++)
    {
        for(pIa = (TCPIP_DHCPV6_IA_DCPT*)pClient->iaStateList[listIx].head; pIa != 0; pIa = pIaNext)
        {
            pIaNext = pIa->next;
            // sanity check
            if(pIa->iaState != listIx)
            {
                _DHCPV6DbgCond(false, __func__, __LINE__);
                _DHCPV6Ia_SetState(pIa, TCPIP_DHCPV6_IA_STATE_ERROR_FATAL, 0);
                continue;
            }
            // dispatch based on the iaSubState 
            const TCPIP_DHCPV6_IA_SUBSTATE_PROC_FNC* pSubProcFnc = _DHCPV6Ia_StateProcTbl[pIa->iaState];

            // dispatch according to run substate! 
            TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes;

            subRes = (*pSubProcFnc[pIa->iaSubState])(pIa);
            switch(subRes)
            {
                case TCPIP_DHCPV6_IA_SUBSTATE_RES_NO_ACTION:
                    break;

                case TCPIP_DHCPV6_IA_SUBSTATE_RES_OK:
                    // ok, can advance substate
                    _DHCPV6Ia_SetState(pIa, pIa->iaState, pIa->iaSubState + 1);
                    break;

                case TCPIP_DHCPV6_IA_SUBSTATE_RES_PENDING:
                    // wait some more
                    break;

                case TCPIP_DHCPV6_IA_SUBSTATE_RES_RETRANSMIT:
                    // need to retransmit
                    _DHCPV6Ia_SetState(pIa, pIa->iaState, TCPIP_DHCPV6_IA_SUBSTATE_TRANSMIT);
                    break;

                case TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_NEXT:
                    // need to advance
                    _DHCPV6Ia_SetState(pIa, pIa->iaState + 1, 0);
                    break;

                case TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_JUMP:
                    // jump to a new state handled at the substate level
                    break;

                case TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_RESTART:
                    // restart discovery procedure
                    _DHCPV6Ia_SetState(pIa, TCPIP_DHCPV6_IA_STATE_SOLICIT, 0);
                    break;

                case TCPIP_DHCPV6_IA_SUBSTATE_RES_ERROR_TRANSIENT:
                    // transient error; abort
                    _DHCPV6Ia_SetState(pIa, TCPIP_DHCPV6_IA_STATE_ERROR_TRANSIENT, 0);
                    break;

                default:    // TCPIP_DHCPV6_IA_SUBSTATE_RES_ERROR_FATAL:
                    // fatal error; abort
                    _DHCPV6Ia_SetState(pIa, TCPIP_DHCPV6_IA_STATE_ERROR_FATAL, 0);
                    break;
            }
        } 
    }

}

// generic substate functions
//
// TCPIP_DHCPV6_IA_SUBSTATE_IDELAY
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT _DHCPV6Ia_SubStateIDelay(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_MSG_TX_RESULT txRes;

    txRes = _DHCPV6Ia_CheckMsgTransmitStatus(pIa);

    if(txRes < 0)
    {   // some error ocurred
        return TCPIP_DHCPV6_IA_SUBSTATE_RES_ERROR_TRANSIENT;
    }

    if(txRes != TCPIP_DHCPV6_MSG_TX_RES_PENDING)
    {   // can move on
        return TCPIP_DHCPV6_IA_SUBSTATE_RES_OK; 
    }

    return TCPIP_DHCPV6_IA_SUBSTATE_RES_PENDING; 
}

// TCPIP_DHCPV6_IA_SUBSTATE_TRANSMIT
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT _DHCPV6Ia_SubStateTransmit(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    // write the message into the buffer
    // set the wrDcpt

    TCPIP_DHCPV6_MSG_BUFFER* pTxBuffer = pIa->msgBuffer;
    if(pTxBuffer == 0)
    {   // shouldn't happen
        _DHCPV6AssertCond(false, __func__, __LINE__);
        return TCPIP_DHCPV6_IA_SUBSTATE_RES_ERROR_FATAL;
    }

    TCPIP_DHCPV6_MSG_WRITE_DCPT* pWrDcpt = &pIa->wrDcpt;

    pWrDcpt->pWrite = pTxBuffer->pMsgData;
    pWrDcpt->writeSpace = pTxBuffer->bufferSize;
    pWrDcpt->writeBytes = 0;
    
    
    int msgSize = _DHCPV6Ia_MessageWrite(pIa);
    if(msgSize == -1)
    {   // shouldn't happen
        _DHCPV6DbgCond(false, __func__, __LINE__);
        return TCPIP_DHCPV6_IA_SUBSTATE_RES_ERROR_FATAL;
    }

    // set the message fields
    pTxBuffer->msgLen = pIa->wrDcpt.writeBytes;

    // insert it into the txMsgList
    TCPIP_Helper_SingleListTailAdd(&pIa->pParent->txMsgList, (SGL_LIST_NODE*)pTxBuffer);

    // we keep the pTxBuffer for this IA in case there are retries

    _DHCPV6TraceBuff(pIa, pTxBuffer);

    return TCPIP_DHCPV6_IA_SUBSTATE_RES_OK;
}


// substate functions
// TCPIP_DHCPV6_IA_STATE_SOLICIT
//

// solicit substate functions
// (TCPIP_DHCPV6_IA_STATE_SOLICIT, TCPIP_DHCPV6_IA_SUBSTATE_START)
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT _DHCPV6Ia_SubStateSolicitStart(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes = _DHCPV6Ia_TxMsgSetup(pIa, TCPIP_DHCPV6_CLIENT_MSG_TYPE_SOLICIT);

    if(subRes == TCPIP_DHCPV6_IA_SUBSTATE_RES_OK)
    {
        _DHCPV6Ia_ListPurge(&pIa->pParent->advertiseList, pIa, false);
        _DHCPV6Ia_SolicitInit(pIa);
    }

    return subRes; 
}



static void _DHCPV6Ia_SolicitInit(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    bool addrValid;
    TCPIP_DHCPV6_CLIENT_DCPT* pParent = pIa->pParent;

    if(pIa->iaBody.type == TCPIP_DHCPV6_IA_TYPE_IANA)
    {
        pIa->iaBody.ianaBody.t1 = pParent->ianaSolicitT1;
        pIa->iaBody.ianaBody.t2 = pParent->ianaSolicitT2;
    } 

    pIa->flags.val = 0;
    addrValid = TCPIP_Helper_StringToIPv6Address(TCPIP_DHCPV6_IANA_SOLICIT_DEFAULT_ADDRESS,  &pIa->addBody.ipv6Addr);

    if(addrValid && memcmp(pIa->addBody.ipv6Addr.v, &IPV6_FIXED_ADDR_UNSPECIFIED, sizeof(IPV6_ADDR)) != 0)
    {   // generate an address option for non zero address
        pIa->flags.addInTx = 1;
        pIa->addBody.prefLTime = pParent->solicitPrefLTime;
        pIa->addBody.validLTime = pParent->solicitValidLTime;
    }
}



// (TCPIP_DHCPV6_IA_STATE_SOLICIT, TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY)
// waiting for advertisments
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT _DHCPV6Ia_SubStateSolicitationWaitReply(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_MSG_TX_RESULT txRes;
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes;
    bool isTmo;


    txRes = _DHCPV6Ia_CheckMsgTransmitStatus(pIa);

    isTmo = (txRes < 0) || txRes == TCPIP_DHCPV6_MSG_TX_RES_OK;

    while(true)
    {
        subRes = _DHCPV6Ia_AdvertiseSelect(pIa, isTmo);

        if(subRes == TCPIP_DHCPV6_IA_SUBSTATE_RES_OK)
        {   // found server for this IA;
            // wait condition granted; move on;
            subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_NEXT;
            break;
        }
        else if(subRes < 0)
        {   // an error occurred
            break;
        }

        // check if re-transmission needed
        if(txRes < 0)
        {   // exhausted the retries
            subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_ERROR_TRANSIENT;
        }
        else if(txRes == TCPIP_DHCPV6_MSG_TX_RES_PENDING)
        {   // keep waiting
            subRes =  TCPIP_DHCPV6_IA_SUBSTATE_RES_PENDING;
        }
        else
        {   // retransmit the Solicit message
            subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_RETRANSMIT;
        }

        break;
    }

    _DHCPV6DbgStatePrint_Add("IA solicit res: %d\r\n", subRes);

     return subRes;
}


// substate functions
// TCPIP_DHCPV6_IA_STATE_REQUEST
//

// (TCPIP_DHCPV6_IA_STATE_REQUEST, TCPIP_DHCPV6_IA_SUBSTATE_START)
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT _DHCPV6Ia_SubStateRequestStart(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes = _DHCPV6Ia_TxMsgSetup(pIa, TCPIP_DHCPV6_CLIENT_MSG_TYPE_REQUEST);
    if(subRes == TCPIP_DHCPV6_IA_SUBSTATE_RES_OK)
    {
        _DHCPV6Ia_ListPurge(&pIa->pParent->replyList, pIa, false);
    }
    

    return subRes; 

}

// (TCPIP_DHCPV6_IA_STATE_REQUEST/TCPIP_DHCPV6_IA_STATE_RENEW, TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY)
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT _DHCPV6Ia_SubStateWaitReplyMsg(TCPIP_DHCPV6_IA_DCPT* pIa)
{

    TCPIP_DHCPV6_MSG_TX_RESULT txRes;
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes;

    _DHCPV6AssertCond((pIa->iaState == TCPIP_DHCPV6_IA_STATE_REQUEST ||
                    pIa->iaState == TCPIP_DHCPV6_IA_STATE_RENEW ||
                    pIa->iaState == TCPIP_DHCPV6_IA_STATE_DECLINE ||
                    pIa->iaState == TCPIP_DHCPV6_IA_STATE_REBIND ||
                    pIa->iaState == TCPIP_DHCPV6_IA_STATE_CONFIRM ||
                    pIa->iaState == TCPIP_DHCPV6_IA_STATE_RELEASE) , __func__, __LINE__);

    bool serverMatch = (pIa->iaState == TCPIP_DHCPV6_IA_STATE_REQUEST ||
            pIa->iaState == TCPIP_DHCPV6_IA_STATE_RENEW ||
            pIa->iaState == TCPIP_DHCPV6_IA_STATE_DECLINE ||
            pIa->iaState == TCPIP_DHCPV6_IA_STATE_RELEASE); 

    if(serverMatch)
    {
        subRes = _DHCPV6Ia_ReplyMsgSrvMatchProcess(pIa);
    }
    else if(pIa->iaState == TCPIP_DHCPV6_IA_STATE_CONFIRM)
    {
        subRes = _DHCPV6Ia_ReplyConfirmProcess(pIa);
    }
    else
    {
        subRes = _DHCPV6Ia_ReplyRebindProcess(pIa);
    }


    if(subRes == TCPIP_DHCPV6_IA_SUBSTATE_RES_PENDING)
    {   // couldn't get any reply
        // check if re-transmission needed
        txRes = _DHCPV6Ia_CheckMsgTransmitStatus(pIa);
        if(txRes < 0)
        {   // exhausted the retries
            if(pIa->iaState == TCPIP_DHCPV6_IA_STATE_REQUEST)
            {   // user intervention needed
                subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_ERROR_TRANSIENT;
            }
            else if(pIa->iaState == TCPIP_DHCPV6_IA_STATE_RENEW)
            {   // go to REBIND
                subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_NEXT;
            }
            else if(pIa->iaState == TCPIP_DHCPV6_IA_STATE_REBIND)
            {   // go to SOLICIT
                TCPIP_IPV6_AddressUnicastRemove(pIa->pParent->pDhcpIf, &pIa->addBody.ipv6Addr);
                subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_RESTART;
            }
            else if(pIa->iaState == TCPIP_DHCPV6_IA_STATE_CONFIRM)
            {   // lack of a reply means we could use the IA as it is
                _DHCPV6Ia_RestoreTimeValues(pIa);
                _DHCPV6Ia_SetState(pIa, TCPIP_DHCPV6_IA_STATE_BOUND, 0);
                subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_JUMP;
            }
            else if(pIa->iaState == TCPIP_DHCPV6_IA_STATE_RELEASE || pIa->iaState == TCPIP_DHCPV6_IA_STATE_DECLINE)
            {   // lack of a reply: we give up
                _DHCPV6Ia_Remove(pIa);
                subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_NO_ACTION;
            }
            else
            {   // should not happen
                _DHCPV6AssertCond(false, __func__, __LINE__);
            }
        }
        else if(txRes == TCPIP_DHCPV6_MSG_TX_RES_PENDING)
        {   // keep waiting
            subRes =  TCPIP_DHCPV6_IA_SUBSTATE_RES_PENDING;
        }
        else
        {   // retransmit the message
            subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_RETRANSMIT;
        }
    }

    _DHCPV6DbgStatePrint_Add("IA - state: %d, substate: %d, wait res: %d\r\n", pIa->iaState, pIa->iaSubState, subRes);
    return subRes; 
}

// substate functions
// TCPIP_DHCPV6_IA_STATE_DAD
//

// (TCPIP_DHCPV6_IA_STATE_DAD , TCPIP_DHCPV6_IA_SUBSTATE_START)
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT _DHCPV6Ia_SubStateDadStart(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    IPV6_ADDR_STRUCT* pAddS;
    IPV6_ADDR_STRUCT * addrPtr;
    bool skipDad;
    
    // check if already known good address (RENEW, REBIND)
    pAddS = TCPIP_IPV6_AddressFind(pIa->pParent->pDhcpIf, &pIa->addBody.ipv6Addr, IPV6_ADDR_TYPE_UNICAST);
    if(pAddS != 0)
    {   // good address
        _DHCPV6Ia_SetState(pIa, TCPIP_DHCPV6_IA_STATE_BOUND, 0);
        return TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_JUMP;
    }
    
    // insert it for DAD
    skipDad = (pIa->pParent->configFlags & TCPIP_DHCPV6_FLAG_DAD_DISABLE) != 0;
    addrPtr = TCPIP_IPV6_UnicastAddressAdd (pIa->pParent->pDhcpIf, &pIa->addBody.ipv6Addr, 0, skipDad);
    if(addrPtr == 0)
    {   // failed inserting new address; try again next time
        _DHCPV6DbgCond(false, __func__, __LINE__);
        return TCPIP_DHCPV6_IA_SUBSTATE_RES_PENDING;
    }

    return TCPIP_DHCPV6_IA_SUBSTATE_RES_OK;
}


// (TCPIP_DHCPV6_IA_STATE_DAD , TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY)
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT _DHCPV6Ia_SubStateDadWait(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    IPV6_ADDR_STRUCT* pAddS;
    TCPIP_DHCPV6_CLIENT_DCPT* pParent = pIa->pParent;

    // search if it's still ongoing
    pAddS = TCPIP_IPV6_AddressFind(pParent->pDhcpIf, &pIa->addBody.ipv6Addr, IPV6_ADDR_TYPE_UNICAST_TENTATIVE);
    if(pAddS != 0)
    {   // in process
        return TCPIP_DHCPV6_IA_SUBSTATE_RES_PENDING;
    }

    // not in tentative; check if already promoted
    pAddS = TCPIP_IPV6_AddressFind(pParent->pDhcpIf, &pIa->addBody.ipv6Addr, IPV6_ADDR_TYPE_UNICAST);
    if(pAddS != 0)
    {   // good address
        _DHCPV6Ia_SetState(pIa, TCPIP_DHCPV6_IA_STATE_BOUND, 0);
        return TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_JUMP;
    }

    // this address has failed; probably duplicate
    // move to next state which is decline!
    _DHCPV6Ia_SetState(pIa, TCPIP_DHCPV6_IA_STATE_DECLINE, 0);
    return TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_JUMP;
}

// substate functions
// TCPIP_DHCPV6_IA_STATE_DECLINE
//

// (TCPIP_DHCPV6_IA_STATE_DECLINE , TCPIP_DHCPV6_IA_SUBSTATE_START)
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT _DHCPV6Ia_SubStateDeclineStart(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    _DHCPV6Ia_ListPurge(&pIa->pParent->replyList, pIa, false);  
    
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes = _DHCPV6Ia_TxMsgSetup(pIa, TCPIP_DHCPV6_CLIENT_MSG_TYPE_DECLINE);

    if(subRes == TCPIP_DHCPV6_IA_SUBSTATE_RES_OK)
    {
        // mark address ready for TX
        pIa->flags.addInTx = 1;
    }

    return subRes; 
}

// TCPIP_DHCPV6_IA_STATE_BOUND
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT _DHCPV6Ia_SubStateBoundWait(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    uint32_t    secCurr = _DHCPV6SecondCountGet();
    uint32_t    iaDeadLine;

    if(pIa->iaBody.type == TCPIP_DHCPV6_IA_TYPE_IANA)
    {   // check the T1 expiration
        iaDeadLine = pIa->iaBody.ianaBody.t1;
    }
    else
    {   // IATA use global values
        iaDeadLine = pIa->pParent->defaultIataT1;
    }

    if((pIa->pParent->configFlags & TCPIP_DHCPV6_FLAG_IA_IGNORE_RENEW_LTIME) == 0)
    {   // select minimum
        iaDeadLine = (iaDeadLine > pIa->addBody.prefLTime) ? pIa->addBody.prefLTime : iaDeadLine;
    }

    if(iaDeadLine != TCPIP_DHCPV6_TIMEOUT_INFINITE)
    {
        if((secCurr - pIa->iaBody.tAcquire) >= iaDeadLine)
        {   // timeout;
            return TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_NEXT; 
        }
    }

    return TCPIP_DHCPV6_IA_SUBSTATE_RES_PENDING;
}

// TCPIP_DHCPV6_IA_STATE_RENEW, TCPIP_DHCPV6_IA_SUBSTATE_START
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT _DHCPV6Ia_SubStateRenewStart(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes = _DHCPV6Ia_TxMsgSetup(pIa, TCPIP_DHCPV6_CLIENT_MSG_TYPE_RENEW);
    if(subRes == TCPIP_DHCPV6_IA_SUBSTATE_RES_OK)
    {
        _DHCPV6Ia_ListPurge(&pIa->pParent->replyList, pIa, false);
        // mark address ready for TX
        pIa->flags.addInTx = 1;
    }
    
    return subRes; 
}

// TCPIP_DHCPV6_IA_STATE_REBIND, TCPIP_DHCPV6_IA_SUBSTATE_START
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT _DHCPV6Ia_SubStateRebindStart(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes = _DHCPV6Ia_TxMsgSetup(pIa, TCPIP_DHCPV6_CLIENT_MSG_TYPE_RENEW);
    if(subRes == TCPIP_DHCPV6_IA_SUBSTATE_RES_OK)
    {
        _DHCPV6Ia_ListPurge(&pIa->pParent->replyList, pIa, false);
        // mark address ready for TX
        pIa->flags.addInTx = 1;
        // make sure we send the rebind to all servers!
        pIa->flags.iaUnicast = 0;
    }
    
    return subRes; 
}

// TCPIP_DHCPV6_IA_STATE_CONFIRM, TCPIP_DHCPV6_IA_SUBSTATE_START
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT _DHCPV6Ia_SubStateConfirmStart(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes;

    // check the IA has not expired while w/o a valid link
    if(_DHCPV6Ia_AddressIsExpired(pIa, false))
    {
        subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_RESTART;
    }
    else
    {
        subRes = _DHCPV6Ia_TxMsgSetup(pIa, TCPIP_DHCPV6_CLIENT_MSG_TYPE_CONFIRM);
        if(subRes == TCPIP_DHCPV6_IA_SUBSTATE_RES_OK)
        {
            _DHCPV6Ia_ListPurge(&pIa->pParent->replyList, pIa, false);
            // mark address ready for TX
            pIa->flags.addInTx = 1;
            // make sure we send the confirm to all servers!
            pIa->flags.iaUnicast = 0;
            // set the T1, T2, lifetimes to 0
            pIa->iaBody.ianaBody.t1 = pIa->iaBody.ianaBody.t2 = 0;
            pIa->addBody.prefLTime = pIa->addBody.validLTime = 0;
        }
    }

    return subRes; 
}

// TCPIP_DHCPV6_IA_STATE_RELEASE, TCPIP_DHCPV6_IA_SUBSTATE_START
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT _DHCPV6Ia_SubStateReleaseStart(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes;

    subRes = _DHCPV6Ia_TxMsgSetup(pIa, TCPIP_DHCPV6_CLIENT_MSG_TYPE_RELEASE);
    if(subRes == TCPIP_DHCPV6_IA_SUBSTATE_RES_OK)
    {
        TCPIP_IPV6_AddressUnicastRemove(pIa->pParent->pDhcpIf, &pIa->addBody.ipv6Addr);
        _DHCPV6Ia_ListPurge(&pIa->pParent->replyList, pIa, false);
        // mark address ready for TX
        pIa->flags.addInTx = 1;
    }

    return subRes; 
}

static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   _DHCPV6Ia_SubStateErrorTransient(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    return TCPIP_DHCPV6_IA_SUBSTATE_RES_PENDING;
}


static TCPIP_DHCPV6_IA_SUBSTATE_RESULT   _DHCPV6Ia_SubStateErrorFatal(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    return TCPIP_DHCPV6_IA_SUBSTATE_RES_PENDING;
}

static void _DHCPV6Close(TCPIP_NET_IF* pNetIf, bool disable, bool release)
{
}

static void _DHCPV6Enable(TCPIP_NET_IF* pNetIf, int opType)
{
    if(dhcpv6ClientDcpt)
    {
        if(TCPIP_IPV6_InterfaceIsReady(pNetIf))
        {
            TCPIP_DHCPV6_CLIENT_DCPT* pClient = dhcpv6ClientDcpt + TCPIP_STACK_NetIxGet(pNetIf);
            if((pClient->configFlags & TCPIP_DHCPV6_FLAG_START_ENABLE) != 0 && pClient->flags.dhcpEnabled == 0)
            {
                pClient->flags.connLost = 1;
                pClient->flags.dhcpEnabled = 1;
                _DHCPV6Client_SetState(pClient, TCPIP_DHCPV6_CLIENT_STATE_WAIT_LINK);
            }
        }
    }
}

#if (_TCPIP_DHCPV6_USER_NOTIFICATION != 0)
TCPIP_DHCPV6_HANDLE TCPIP_DHCPV6_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_DHCPV6_EVENT_HANDLER handler, const void* hParam)
{
    if(handler && dhcpv6MemH)
    {
        TCPIP_DHCPV6_LIST_NODE dhcpNode;
        dhcpNode.handler = handler;
        dhcpNode.hParam = hParam;
        dhcpNode.hNet = hNet;

        return (TCPIP_DHCPV6_LIST_NODE*)TCPIP_Notification_Add(&dhcpv6RegisteredUsers, dhcpv6MemH, &dhcpNode, sizeof(dhcpNode));
    }

    return 0;
}

// deregister the event handler
bool TCPIP_DHCPV6_HandlerDeRegister(TCPIP_DHCPV6_HANDLE hDhcp)
{
    if(hDhcp && dhcpv6MemH)
    {
        if(TCPIP_Notification_Remove((SGL_LIST_NODE*)hDhcp, &dhcpv6RegisteredUsers, dhcpv6MemH))
        {
            return true;
        }
    }

    return false;
}
#endif  // (_TCPIP_DHCPV6_USER_NOTIFICATION != 0)

static void _DHCPV6Client_Notify(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_IA_DCPT* pIa, bool iaSubNotify)
{
    (void)pClient;
    (void)pIa;

    // call with both pClient and pIa == 0 is invalid
    _DHCPV6AssertCond(pClient != 0 || pIa != 0 , __func__, __LINE__);

    if(pClient == 0)
    {
        pClient = pIa->pParent;
    }

#if (_TCPIP_DHCPV6_USER_NOTIFICATION != 0)
    const TCPIP_DHCPV6_IA_EVENT* pDhcpIaEv = 0;
    TCPIP_DHCPV6_LIST_NODE* dNode;

    TCPIP_DHCPV6_CLIENT_STATE clientState = pClient->state;
    TCPIP_NET_IF* pNetIf = pClient->pDhcpIf;

    if(pIa != 0)
    {
        TCPIP_DHCPV6_IA_EVENT iaEvent;

        iaEvent.iaType = pIa->iaBody.type;
        iaEvent.iaSubState = pIa->iaSubState;
        iaEvent.iaState = pIa->iaState;
        iaEvent.iaIndex = pIa->parentIx;

        pDhcpIaEv = &iaEvent;
    }

    TCPIP_Notification_Lock(&dhcpv6RegisteredUsers);
    for(dNode = (TCPIP_DHCPV6_LIST_NODE*)dhcpv6RegisteredUsers.list.head; dNode != 0; dNode = dNode->next)
    {
        if(dNode->hNet == 0 || dNode->hNet == pNetIf)
        {   // trigger event
            (*dNode->handler)(pNetIf, clientState, pDhcpIaEv, dNode->hParam);
        }
    }
    TCPIP_Notification_Unlock(&dhcpv6RegisteredUsers);
#endif  // (_TCPIP_DHCPV6_USER_NOTIFICATION != 0)

    _DHCPV6DbgStatePrint_Client(pClient, true);
    _DHCPV6DbgStatePrint_Ia(pIa, iaSubNotify);
}

static UDP_SOCKET _DHCPV6OpenSocket(TCPIP_NET_IF* pNetIf)
{
    bool     udpSuccess;
    uint16_t txBuffSize;
    TCPIP_UDP_SIGNAL_HANDLE sigHandle;

    UDP_SOCKET dhcpSkt = TCPIP_UDP_OpenClientSkt(IP_ADDRESS_TYPE_IPV6, dhcpv6ServerPort, 0, UDP_OPEN_CLIENT | UDP_OPEN_CONFIG_SERVICE);

    if(dhcpSkt != INVALID_UDP_SOCKET)
    {   
        // make sure the socket is created with enough TX space
        TCPIP_UDP_OptionsGet(dhcpSkt, UDP_OPTION_TX_BUFF, (void*)&txBuffSize);
        if(txBuffSize < TCPIP_DHCPV6_MIN_UDP_TX_BUFFER_SIZE)
        {
            TCPIP_UDP_OptionsSet(dhcpSkt, UDP_OPTION_TX_BUFF, (void*)TCPIP_DHCPV6_MIN_UDP_TX_BUFFER_SIZE);
        }

        // bind to the DHCP local port
        udpSuccess = TCPIP_UDP_Bind(dhcpSkt, IP_ADDRESS_TYPE_IPV6, dhcpv6ClientPort,  0);
        if(udpSuccess)
        {   // bind to this interface
            TCPIP_UDP_SocketNetSet(dhcpSkt, pNetIf);
            sigHandle = TCPIP_UDP_SignalHandlerRegister(dhcpSkt, TCPIP_UDP_SIGNAL_RX_DATA, _DHCPV6SocketRxSignalHandler, 0);
            if(!sigHandle)
            {
                udpSuccess = 0;
            }
        }

        if(!udpSuccess)
        {
            TCPIP_UDP_Close(dhcpSkt);
            dhcpSkt = INVALID_UDP_SOCKET;
        }
		TCPIP_UDP_OptionsSet(dhcpSkt, UDP_OPTION_STRICT_PORT, (void*)false);

    }


    return dhcpSkt;
}



// status reporting
bool TCPIP_DHCPV6_ClientInfoGet(TCPIP_NET_HANDLE hNet, TCPIP_DHCPV6_CLIENT_INFO* pClientInfo)
{
    size_t copyBytes;
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(hNet);

    if(dhcpv6ClientDcpt && pNetIf && pClientInfo)
    {
        TCPIP_DHCPV6_CLIENT_DCPT* pClient = dhcpv6ClientDcpt + TCPIP_STACK_NetIxGet(pNetIf);
        pClientInfo->clientState = pClient->state;
        pClientInfo->nIanas = pClient->nIanaDcpts;
        pClientInfo->nIatas = pClient->nIataDcpts;
        pClientInfo->nFreeIas = TCPIP_Helper_DoubleListCount(&pClient->iaFreeList);
        pClientInfo->dhcpTime = _DHCPV6SecondCountGet();
        pClientInfo->lastStatusCode = pClient->lastStatusCode;
        if(sizeof(pClient->lastStatusMsg) != 0 && pClientInfo->statusBuff != 0)
        {
            copyBytes = sizeof(pClient->lastStatusMsg) < pClientInfo->statusBuffSize ? sizeof(pClient->lastStatusMsg) : pClientInfo->statusBuffSize;
            memcpy(pClientInfo->statusBuff, pClient->lastStatusMsg, copyBytes);
        }
        pClientInfo->nDnsServers = pClient->nDnsServers;
        pClientInfo->domainSearchListSize = pClient->domainSearchListSize;
        if(sizeof(pClient->dnsServersAdd) != 0 && pClientInfo->dnsBuff != 0)
        {
            copyBytes = sizeof(pClient->dnsServersAdd) < pClientInfo->dnsBuffSize ? sizeof(pClient->dnsServersAdd) : pClientInfo->dnsBuffSize;
            memcpy(pClientInfo->dnsBuff, pClient->dnsServersAdd, copyBytes);
        }
        if(sizeof(pClient->domainSearchList) != 0 && pClientInfo->domainBuff != 0)
        {
            copyBytes = sizeof(pClient->domainSearchList) < pClientInfo->domainBuffSize ? sizeof(pClient->domainSearchList) : pClientInfo->domainBuffSize;
            memcpy(pClientInfo->domainBuff, pClient->domainSearchList, copyBytes);
        }
        return true;
    } 

    return false;
}

bool TCPIP_DHCPV6_IaInfoGet(TCPIP_NET_HANDLE hNet, int iaIx, TCPIP_DHCPV6_IA_INFO* pIaInfo)
{
    size_t copyBytes;
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(hNet);
    TCPIP_DHCPV6_IA_DCPT* pFoundIa = 0;

    if(dhcpv6ClientDcpt && pNetIf && pIaInfo)
    {
        TCPIP_DHCPV6_CLIENT_DCPT* pClient = dhcpv6ClientDcpt + TCPIP_STACK_NetIxGet(pNetIf);
        TCPIP_DHCPV6_IA_DCPT* pIa;
        int listIx;

        for(listIx = 0; listIx < sizeof(pClient->iaStateList) / sizeof(*pClient->iaStateList); listIx++)
        {
            for(pIa = (TCPIP_DHCPV6_IA_DCPT*)pClient->iaStateList[listIx].head; pIa != 0; pIa = pIa->next)
            {
                if(pIa->parentIx == iaIx)
                {
                    pFoundIa = pIa;
                    break;
                }
            }
        }
    }

    if(pFoundIa)
    {
        pIaInfo->iaType = pFoundIa->iaBody.type;
        pIaInfo->iaSubState = pFoundIa->iaSubState;
        pIaInfo->iaState = pFoundIa->iaState;
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
        pIaInfo->ipv6Addr = pFoundIa->addBody.ipv6Addr;
        pIaInfo->prefLTime = pFoundIa->addBody.prefLTime;
        pIaInfo->validLTime = pFoundIa->addBody.validLTime;

        pIaInfo->lastStatusCode = pFoundIa->lastStatusCode;
        if(sizeof(pFoundIa->lastStatusMsg) != 0 && pIaInfo->statusBuff != 0)
        {
            copyBytes = sizeof(pFoundIa->lastStatusMsg) < pIaInfo->statusBuffSize ? sizeof(pFoundIa->lastStatusMsg) : pIaInfo->statusBuffSize;
            memcpy(pIaInfo->statusBuff, pFoundIa->lastStatusMsg, copyBytes);
        }
        return true;
    }

    return false;
}

// generates a client DUID 
static bool _DHCPV6Duid_Generate(TCPIP_DHCPV6_DUID_TYPE duidType, TCPIP_DHCPV6_DUID_DCPT* pDuid, TCPIP_NET_IF* pDuidIf)
{
    if(duidType != TCPIP_DHCPV6_DUID_TYPE_LL)
    {   
        return false;
    }

    // generate a TCPIP_DHCPV6_DUID_TYPE_LL

    pDuid->duidBody.duidLL.duid_type = TCPIP_Helper_htons(TCPIP_DHCPV6_DUID_TYPE_LL);
    pDuid->duidBody.duidLL.hw_type = TCPIP_Helper_htons(TCPIP_DHCPV6_HW_TYPE);
    memcpy(pDuid->duidBody.duidLL.ll_address, _TCPIPStack_NetMACAddressGet(pDuidIf), sizeof(pDuid->duidBody.duidLL.ll_address));
    
    pDuid->duidType = TCPIP_DHCPV6_DUID_TYPE_LL; 
    pDuid->duidLen = sizeof(pDuid->duidBody.duidLL); 
    return true;
} 


// pDcpt should be populated
static TCPIP_DHCPV6_MSG_TX_RESULT _DHCPV6Ia_CheckMsgTransmitStatus(TCPIP_DHCPV6_IA_DCPT* pIa)
{

    uint32_t    tBase;
    uint32_t    tFuzz, tFuzzMinus, tFuzzPlus;
    uint32_t    tickCurr;  // current sys tick
    uint32_t    secCurr;  // current second
    uint32_t    sysFreq = SYS_TMR_TickCounterFrequencyGet();

    tickCurr = SYS_TMR_TickCountGet();
    secCurr = _DHCPV6SecondCountGet();
    
    TCPIP_DHCPV6_MSG_TRANSMIT_DCPT* pDcpt = &pIa->msgTxDcpt;


    if(pDcpt->rc != 0 && (int32_t)(tickCurr - pDcpt->waitTick) < 0)
    {   // not yet time for retransmission
        return TCPIP_DHCPV6_MSG_TX_RES_PENDING;
    }

    while(pDcpt->rc == 0)
    {   // message just starts to be transmitted
        if(pDcpt->bounds.iDelay == 0)
        {   // no initial delay, go straight ahead
            break;
        }

        if(pDcpt->waitTick == 0)
        {   // set up the waiting period
            pDcpt->waitTick = tickCurr + (((pDcpt->bounds.iDelay * 1000 - TCPIP_DHCPV6_EXP_RAND_FUZZ) * sysFreq) + SYS_RANDOM_PseudoGet() % ((2 * TCPIP_DHCPV6_EXP_RAND_FUZZ) * sysFreq)) / 1000;
            return TCPIP_DHCPV6_MSG_TX_RES_PENDING;
        }

        // wait period already started
        if((int32_t)(tickCurr - pDcpt->waitTick) < 0)
        {   // wait some more
            return TCPIP_DHCPV6_MSG_TX_RES_PENDING;
        }

        // iDelay passed; go ahead and transmit
        break;
    }



    // message needs retransmission
    if(pDcpt->bounds.mrc != 0)
    {
        if(pDcpt->rc >= pDcpt->bounds.mrc)
        {   // message exceeded the number of retries
            return TCPIP_DHCPV6_MSG_TX_RES_RETRY_EXCEEDED;
        }
    }

    if(pDcpt->rc != 0 && pDcpt->bounds.mrd != 0)
    {   // avoid doing the test for the 1st transmission
        if(secCurr - pDcpt->iTime >= pDcpt->bounds.mrd)
        {   // message exceeded the allocated time
            return TCPIP_DHCPV6_MSG_TX_RES_TIME_EXCEEDED;
        }
    }

    // adjust tFuzzMinus, tFuzzPlus to get a delay (-1, 1) * TCPIP_DHCPV6_EXP_RAND_FUZZ
    tFuzzMinus = TCPIP_DHCPV6_EXP_RAND_FUZZ;
    tFuzzPlus = 2 * TCPIP_DHCPV6_EXP_RAND_FUZZ;

    // calculate the next retransmission time/timeouts
    if(pDcpt->rc == 0)
    {   // first message
        tBase = tFuzz = pDcpt->bounds.irt;
        pDcpt->iTime = secCurr;
        pDcpt->iTickTime = tickCurr;
        pDcpt->elapsedTime = 0;
        if(pIa->iaState == TCPIP_DHCPV6_IA_STATE_SOLICIT)
        {   // solicit has to be greater than IRT! 
            tFuzzMinus = 0;
            tFuzzPlus = TCPIP_DHCPV6_EXP_RAND_FUZZ;
        }
    }
    else
    {   // subsequent retry
        tBase = 2 * pDcpt->rt;
        tFuzz = pDcpt->rt;
        pDcpt->elapsedTime = ((tickCurr - pDcpt->iTickTime) * 100) / sysFreq;   // convert to 10^-2 sec
    }

    if(pDcpt->bounds.mrt != 0 && tBase > pDcpt->bounds.mrt)
    {   // limit the rt
        tBase = tFuzz = pDcpt->bounds.mrt;
    }


    pDcpt->waitTick = tickCurr + (tBase * sysFreq) + (((tFuzz * 1000 - tFuzzMinus) * sysFreq) + SYS_RANDOM_PseudoGet() % (tFuzzPlus * sysFreq)) / 1000;
    pDcpt->rt = tBase;
    pDcpt->rc++;

    // message needs to be transmitted
    return TCPIP_DHCPV6_MSG_TX_RES_OK;

}

// copies a DUID in the corresponding buffer
static int _DHCPV6Option_WriteDuid(TCPIP_DHCPV6_DUID_DCPT* pDuid, TCPIP_DHCPV6_MSG_WRITE_DCPT* pWrDcpt)
{
    if(pDuid->duidType == TCPIP_DHCPV6_DUID_TYPE_NONE || pDuid->duidLen == 0)
    {   // invalid ?
        return -1;
    }

    if(pWrDcpt->writeSpace < pDuid->duidLen)
    {   // not enough room in this buffer
        return -1;
    }

    memcpy(pWrDcpt->pWrite, &pDuid->duidBody.duidLL, pDuid->duidLen);
    return pDuid->duidLen;
} 


// returns size of the message written
// -1 if some error
static int _DHCPV6Ia_MessageWrite(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_MSG_WRITE_DCPT* pWrDcpt = &pIa->wrDcpt;
    
    if(pWrDcpt->writeSpace < sizeof(TCPIP_DHCPV6_MESSAGE_HEADER))
    {   // not enough room space
        return -1;
    }
    
    int32_t iniBytes = pWrDcpt->writeBytes; 
    _DHCPV6DbgCond(iniBytes == 0, __func__, __LINE__);  // should be 0

    TCPIP_DHCPV6_CLIENT_MSG_TYPE cliMsgType = pIa->cliMsgType;

    TCPIP_DHCPV6_MESSAGE_HEADER* pHdr = (TCPIP_DHCPV6_MESSAGE_HEADER*)pWrDcpt->pWrite;

    memcpy(pHdr, pIa->transactionId.v, sizeof(TCPIP_DHCPV6_MESSAGE_HEADER));
    pHdr->msg_type = dhcpv6ClientToMessageTypeTbl[cliMsgType];

    pWrDcpt->writeSpace -= sizeof(TCPIP_DHCPV6_MESSAGE_HEADER);
    pWrDcpt->writeBytes += sizeof(TCPIP_DHCPV6_MESSAGE_HEADER);
    pWrDcpt->pWrite += sizeof(TCPIP_DHCPV6_MESSAGE_HEADER);

    // start writing message options


    TCPIP_DHCPV6_OPTION_MASK_SET_0* pMsgMask = DHCPV6_MSG_OPTION_CURR_MASK_TBL + cliMsgType;
    TCPIP_DHCPV6_OPTION_CODE optCode;    
    const uint32_t* pCurrOpt = 0;
    int currOptMask = 0;

    while(true)
    {
        optCode = _DHCPV6OptionSet_ExtractCode(pMsgMask->optionSet, pMsgMask->nSets, &pCurrOpt, &currOptMask);
        if(optCode == 0)
        {   // done
            break;
        }

        if(_DHCPV6Option_Write(optCode, pIa) == -1)
        {   // failed to write an option
            return -1;
        }
    }
    

    return pWrDcpt->writeBytes - iniBytes;
} 


static int _DHCPV6Option_Write(TCPIP_DHCPV6_OPTION_CODE optCode, TCPIP_DHCPV6_IA_DCPT* pIa)
{
    int optSize = -1;

    if(0 <= optCode && optCode < sizeof(_DHCPV6Option_WriteFncTbl)/sizeof(*_DHCPV6Option_WriteFncTbl))
    {
        _DHCPV6Option_WriteFnc optFnc = _DHCPV6Option_WriteFncTbl[optCode];
        if(optFnc)
        {   // option has associated function
            optSize = (*optFnc)(pIa);
            if(optSize != -1)
            {
                TCPIP_DHCPV6_MSG_WRITE_DCPT* pWrDcpt = &pIa->wrDcpt;
                pWrDcpt->writeSpace -= optSize;
                pWrDcpt->writeBytes += optSize;
                pWrDcpt->pWrite += optSize;
            }
        }
    } 


    return optSize;
}


// TCPIP_DHCPV6_OPT_CODE_CLIENT_ID
static int _DHCPV6Option_WriteClientId(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_CLIENT_DCPT* pParent = pIa->pParent;
    return _DHCPV6Option_WriteId(&pParent->clientDuid, TCPIP_DHCPV6_OPT_CODE_CLIENT_ID, &pIa->wrDcpt);
}


// TCPIP_DHCPV6_OPT_CODE_SERVER_ID
static int _DHCPV6Option_WriteServerId(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    return _DHCPV6Option_WriteId(&pIa->serverDuid, TCPIP_DHCPV6_OPT_CODE_SERVER_ID, &pIa->wrDcpt);
}

static int _DHCPV6Option_WriteId(TCPIP_DHCPV6_DUID_DCPT* pDuid, TCPIP_DHCPV6_OPTION_CODE optCode, TCPIP_DHCPV6_MSG_WRITE_DCPT* pWrDcpt)
{

    if(pWrDcpt->writeSpace < sizeof(TCPIP_DHCPV6_OPTION_ID))
    {
        return -1;
    }

    TCPIP_DHCPV6_MSG_WRITE_DCPT  duidWrDcpt;
    duidWrDcpt.writeSpace = pWrDcpt->writeSpace - sizeof(TCPIP_DHCPV6_OPTION_ID);
    duidWrDcpt.pWrite = pWrDcpt->pWrite + sizeof(TCPIP_DHCPV6_OPTION_ID);

    int duidSize = _DHCPV6Option_WriteDuid(pDuid, &duidWrDcpt);

    if(duidSize != -1)
    {
        TCPIP_DHCPV6_OPTION_ID* pId = (TCPIP_DHCPV6_OPTION_ID*)pWrDcpt->pWrite;

        pId->optCode = TCPIP_Helper_htons(optCode);
        pId->optLen = TCPIP_Helper_htons(duidSize);
        duidSize += sizeof(TCPIP_DHCPV6_OPTION_ID);
    }

    return duidSize;
}




// TCPIP_DHCPV6_OPT_CODE_IA_NA
// write a IA_NA and the associated address
// and addresses marked addInTx are written!
static int _DHCPV6Option_WriteIA_NA(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    int addrOptSize;
    TCPIP_DHCPV6_OPTION_IANA* pOptIana;
    TCPIP_DHCPV6_MSG_WRITE_DCPT  wrDcpt;

    if(pIa->iaBody.type != TCPIP_DHCPV6_IA_TYPE_IANA)
    {   // nothing to write
        return 0;
    }

    // local copy descriptor
    wrDcpt =  pIa->wrDcpt;

    if(wrDcpt.writeSpace < sizeof(TCPIP_DHCPV6_OPTION_IANA))
    {
        return -1;
    }

    wrDcpt.writeSpace -= sizeof(TCPIP_DHCPV6_OPTION_IANA);
    wrDcpt.pWrite += sizeof(TCPIP_DHCPV6_OPTION_IANA);

    // add the IA associated address
    addrOptSize = 0;
    if(pIa->flags.addInTx)
    {
        addrOptSize = _DHCPV6Option_WriteIA_Addr(&pIa->addBody, &wrDcpt);
        if(addrOptSize == -1)
        {   // failed
            return -1;
        }

    }
#if defined(_TCPIP_DHCPV6_WRITE_IA_WITH_ADDRESSES_ONLY)
    if(addrOptSize == 0)
    {   // no addresses to be tx-ed in this IA; ignore
        return 0;
    }
#endif  // defined(_TCPIP_DHCPV6_WRITE_IA_WITH_ADDRESSES_ONLY)

    // write the IANA
    pOptIana = (TCPIP_DHCPV6_OPTION_IANA*)pIa->wrDcpt.pWrite;
    pOptIana->optCode = TCPIP_Helper_htons(TCPIP_DHCPV6_OPT_CODE_IA_NA);
    pOptIana->optLen = TCPIP_Helper_htons(addrOptSize + sizeof(pOptIana->body));
    pOptIana->body.iaid = TCPIP_Helper_htonl(pIa->iaBody.genId);
    pOptIana->body.t1 = TCPIP_Helper_htonl(pIa->iaBody.ianaBody.t1);
    pOptIana->body.t2 = TCPIP_Helper_htonl(pIa->iaBody.ianaBody.t2);

    return addrOptSize + sizeof(*pOptIana);

}

// TCPIP_DHCPV6_OPT_CODE_IA_TA
// write a IA_TA and the associated address
// and addresses marked addInTx are written!
static int _DHCPV6Option_WriteIA_TA(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    int addrOptSize;
    TCPIP_DHCPV6_OPTION_IATA* pIata;
    TCPIP_DHCPV6_MSG_WRITE_DCPT  wrDcpt;

    if(pIa->iaBody.type != TCPIP_DHCPV6_IA_TYPE_IATA)
    {   // nothing to write
        return 0;
    }

    // local copy descriptor
    wrDcpt =  pIa->wrDcpt;

    if(wrDcpt.writeSpace < sizeof(TCPIP_DHCPV6_OPTION_IATA))
    {
        return -1;
    }

    wrDcpt.writeSpace -= sizeof(TCPIP_DHCPV6_OPTION_IATA);
    wrDcpt.pWrite += sizeof(TCPIP_DHCPV6_OPTION_IATA);

    // add the IA associated address
    addrOptSize = 0;
    if(pIa->flags.addInTx)
    {
        addrOptSize = _DHCPV6Option_WriteIA_Addr(&pIa->addBody, &wrDcpt);
        if(addrOptSize == -1)
        {   // failed
            return -1;
        }
    }

#if defined(_TCPIP_DHCPV6_WRITE_IA_WITH_ADDRESSES_ONLY)
    if(addrOptSize == 0)
    {   // no addresses to be tx-ed in this IA; ignore
        return 0;
    }
#endif  // defined(_TCPIP_DHCPV6_WRITE_IA_WITH_ADDRESSES_ONLY)

    // write the IATA
    pIata = (TCPIP_DHCPV6_OPTION_IATA*)pIa->wrDcpt.pWrite;
    pIata->optCode = TCPIP_Helper_htons(TCPIP_DHCPV6_OPT_CODE_IA_TA);
    pIata->optLen = TCPIP_Helper_htons(addrOptSize + sizeof(pIata->body));
    pIata->body.iaid = TCPIP_Helper_htonl(pIa->iaBody.genId);

    return addrOptSize + sizeof(*pIata);
}

static int _DHCPV6Option_WriteIA_Addr(TCPIP_DHCPV6_OPTION_IA_ADDR_BODY* pASpec, TCPIP_DHCPV6_MSG_WRITE_DCPT* pWrDcpt)
{
    if(pWrDcpt->writeSpace < sizeof(TCPIP_DHCPV6_OPTION_IA_ADDR))
    {
        return -1;
    }

    TCPIP_DHCPV6_OPTION_IA_ADDR* pIaAddr = (TCPIP_DHCPV6_OPTION_IA_ADDR*)pWrDcpt->pWrite;

    pIaAddr->optCode = TCPIP_Helper_htons(TCPIP_DHCPV6_OPT_CODE_IA_ADDR);
    pIaAddr->optLen = TCPIP_Helper_htons(sizeof(pIaAddr->body));

    pIaAddr->body.prefLTime = TCPIP_Helper_htonl(pASpec->prefLTime);
    pIaAddr->body.validLTime = TCPIP_Helper_htonl(pASpec->validLTime);
    memcpy(&pIaAddr->body.ipv6Addr, &pASpec->ipv6Addr, sizeof(pIaAddr->body.ipv6Addr));


   return sizeof(TCPIP_DHCPV6_OPTION_IA_ADDR); 
}


// TCPIP_DHCPV6_OPT_CODE_OPTION_REQ
static int _DHCPV6Option_WriteOptionRequest(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    int nReqCodes;

    TCPIP_DHCPV6_MSG_WRITE_DCPT* pWrDcpt = &pIa->wrDcpt;

    TCPIP_DHCPV6_OPTION_MASK_SET* pMaskSet = (TCPIP_DHCPV6_OPTION_MASK_SET*)pIa->pOroOptMask;


    nReqCodes = _DHCPV6OptionSet_CodesNo(pMaskSet->optionSet, pMaskSet->nSets);

    if(nReqCodes == 0)
    {   // no requests to write; shouldn't happen
        return 0;
    }

    // calculate needed size
    // each requested option is 2 bytes
    int reqCodeSize = nReqCodes << 1;
    int optReqSize = sizeof(TCPIP_DHCPV6_OPTION_OPTION_REQ) + reqCodeSize;

    if(pWrDcpt->writeSpace < optReqSize)
    {   // not enough room
        return -1;
    }
    
    TCPIP_DHCPV6_OPTION_OPTION_REQ* pOptReq = (TCPIP_DHCPV6_OPTION_OPTION_REQ*)pWrDcpt->pWrite;
    pOptReq->optCode = TCPIP_Helper_htons(TCPIP_DHCPV6_OPT_CODE_OPTION_REQ);
    pOptReq->optLen = TCPIP_Helper_htons(reqCodeSize);

    uint16_t* pReqDest = pOptReq->reqCode;
    const uint32_t* pCurrReq = 0;
    int currReqMask = 0;

    TCPIP_DHCPV6_OPTION_CODE reqCode;    
    while(true)
    {
        reqCode = _DHCPV6OptionSet_ExtractCode(pMaskSet->optionSet, pMaskSet->nSets, &pCurrReq, &currReqMask);
        if(reqCode == 0)
        {   // done
            break;
        }

        *pReqDest++ = TCPIP_Helper_htons((uint16_t)reqCode);
    }


    return optReqSize;
}

// TCPIP_DHCPV6_OPT_CODE_ELAPSED_TIME
static int _DHCPV6Option_WriteElapsedTime(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_MSG_WRITE_DCPT* pWrDcpt = &pIa->wrDcpt;

    if(pWrDcpt->writeSpace < sizeof(TCPIP_DHCPV6_OPTION_ELAPSED_TIME))
    {   // not enough room
        return -1;
    }

    TCPIP_DHCPV6_OPTION_ELAPSED_TIME* pElTime = (TCPIP_DHCPV6_OPTION_ELAPSED_TIME*)pWrDcpt->pWrite;
    pElTime->optCode = TCPIP_Helper_htons(TCPIP_DHCPV6_OPT_CODE_ELAPSED_TIME);
    pElTime->optLen = TCPIP_Helper_htons(sizeof(pElTime->elapsedTime));

    uint32_t tVal = pIa->msgTxDcpt.elapsedTime;
    if(tVal >= 0x10000)
    {
        tVal = 0xffff;
    }
    pElTime->elapsedTime = TCPIP_Helper_htons((uint16_t)tVal);

    return sizeof(TCPIP_DHCPV6_OPTION_ELAPSED_TIME);
}

// TCPIP_DHCPV6_OPT_CODE_RAPID_COMMIT
static int _DHCPV6Option_WriteRapidCommit(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_MSG_WRITE_DCPT* pWrDcpt = &pIa->wrDcpt;

    if(pWrDcpt->writeSpace < sizeof(TCPIP_DHCPV6_OPTION_RAPID_COMMIT))
    {   // not enough room
        return -1;
    }
    
    TCPIP_DHCPV6_OPTION_RAPID_COMMIT* pRCommit = (TCPIP_DHCPV6_OPTION_RAPID_COMMIT*)pWrDcpt->pWrite;
    pRCommit->optCode = TCPIP_Helper_htons(TCPIP_DHCPV6_OPT_CODE_RAPID_COMMIT);
    pRCommit->optLen = 0;
    return sizeof(TCPIP_DHCPV6_OPTION_RAPID_COMMIT);
}

// generates a IA_TA/IA_NA IAID for a interface
// only 16 bits counters/rounds per interface are supported
static void _DHCPV6Ia_IdGenerate(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_NET_IF* pIaidIf, TCPIP_DHCPV6_IA_ROUND* pIaRound)
{
    uint16_t    round;
    TCPIP_UINT64_VAL    genId;    // construct the IAID here
    
    bool isIana = pIa->iaBody.type == TCPIP_DHCPV6_IA_TYPE_IANA;


    memcpy(genId.v, _TCPIPStack_NetMACAddressGet(pIaidIf), sizeof(TCPIP_MAC_ADDR));

    if(isIana)
    {
        round = pIaRound->ianaIaidRound;
    }
    else
    {
        round = pIaRound->iataIaidRound;
    }


    genId.w[3] = ++round;

    genId.d[0] ^= genId.d[1];

    if(isIana)
    {   // NA address space
        genId.v[1] = 0xfd;
        pIaRound->ianaIaidRound = round;
    }
    else
    {   // TA address space
        genId.v[1] = 0xfe;
        pIaRound->iataIaidRound = round;
    }

    pIa->iaBody.genId = genId.d[0]; 
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
// returns  - a TCPIP_DHCPV6_OPTION_CODE value extracted from the set
//          - TCPIP_DHCPV6_OPT_CODE_NONE (0) if the sets exhausted 
static TCPIP_DHCPV6_OPTION_CODE _DHCPV6OptionSet_ExtractCode(const uint32_t* pOptionSet, int nSets, const uint32_t** ppCurrSet, int* pCurrMask)
{
    const uint32_t* pCurrSet = *ppCurrSet;
    if(pCurrSet == 0)
    {
        pCurrSet = pOptionSet;
    }

    int currMask = *pCurrMask;

    for(; pCurrSet != pOptionSet + nSets; pCurrSet++)
    {
        int currOptBase = (pCurrSet - pOptionSet) * 32;
        uint32_t currSet = *pCurrSet;
        for(; currMask < 32; currMask++)
        {
            if((currSet & (1 << currMask)) != 0)
            {   // valid value
                *ppCurrSet = pCurrSet;
                *pCurrMask = currMask + 1;
                return (TCPIP_DHCPV6_OPTION_CODE)(currOptBase + currMask);
            }
        }
        // go to next set
        currMask = 0;
    }

    // nothing found; end
    return TCPIP_DHCPV6_OPT_CODE_NONE;
}

// return the number of codes that are set in pOptionSet
static int _DHCPV6OptionSet_CodesNo(const uint32_t* pOptionSet, int nSets)
{
    const uint32_t* pCurrSet;
    uint32_t    currMask;
    int         setIx, currIx, nMasks;

    nMasks = 0;
    pCurrSet = pOptionSet;
    for(setIx = 0; setIx < nSets; setIx++, pCurrSet++)
    {
        uint32_t currSet = *pCurrSet;
        currMask = 1;
        for(currIx = 0; currIx < 32; currIx++)
        {
            if((currSet & currMask) != 0)
            {   // valid value
                nMasks++;
            }
            currMask <<= 1;
        }
    }

    return nMasks;
}

static bool _DHCPV6Client_Init(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    int dcptIx;
    int buffIx;
    TCPIP_DHCPV6_IA_DCPT* pIaDcpt;
    TCPIP_DHCPV6_MSG_BUFFER* pBuffer;

    TCPIP_NET_IF* pNetIf = pClient->pDhcpIf;
    UDP_SOCKET dhcpSkt = _DHCPV6OpenSocket(pNetIf);

    if(dhcpSkt == INVALID_SOCKET)
    {
        return false;
    }

    pClient->dhcpSkt = dhcpSkt;

    _DHCPV6Duid_Generate(pClient->clientDuid.duidType, &pClient->clientDuid, pNetIf);
    
    // initialize the IAs
    // generate the iaid for IA-NA, IA_TA
    pIaDcpt = pClient->iaArray;
    for(dcptIx = 0; dcptIx < pClient->nIaDcpts; dcptIx++, pIaDcpt++)
    {
        pIaDcpt->pParent = pClient;
        pIaDcpt->parentIx = dcptIx;

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

            _DHCPV6Ia_IdGenerate(pIaDcpt, pNetIf, &pClient->iaIaidRound);
            // mark all in use IAs as starting the solicit
            pIaDcpt->iaState = TCPIP_DHCPV6_IA_STATE_SOLICIT;
            pIaDcpt->iaSubState = 0;
            TCPIP_Helper_DoubleListTailAdd(pClient->iaStateList + TCPIP_DHCPV6_IA_STATE_SOLICIT, (DBL_LIST_NODE*)pIaDcpt);
        }
        else
        {   // not used yet
            pIaDcpt->iaBody.type = TCPIP_DHCPV6_IA_TYPE_NONE;
            TCPIP_Helper_DoubleListTailAdd(&pClient->iaFreeList, (DBL_LIST_NODE*)pIaDcpt);
        }
    }

    // initialize client message buffers
    pBuffer = pClient->msgBuffers;
    for(buffIx = 0; buffIx < pClient->nMsgBuffers; buffIx++)
    {
        pBuffer->bufferSize = pClient->msgBufferSize;
        pBuffer->pMsgData = pBuffer->msgData;
        TCPIP_Helper_SingleListTailAdd(&pClient->buffFreeList, (SGL_LIST_NODE*)pBuffer);
        _DHCPV6TraceBuffInit(buffIx, pBuffer);
        pBuffer = (TCPIP_DHCPV6_MSG_BUFFER*)((uint8_t*)pBuffer + sizeof(TCPIP_DHCPV6_MSG_BUFFER) + pClient->msgBufferSize);
    }

    pClient->connEvent = 0;

    // update the MRT overrides
    pClient->solMaxRt = (dhcpv6MessageBoundsTbl + TCPIP_DHCPV6_CLIENT_MSG_TYPE_SOLICIT)->mrt;
    pClient->infoMaxRt = (dhcpv6MessageBoundsTbl + TCPIP_DHCPV6_CLIENT_MSG_TYPE_INFO_REQUEST)->mrt;

    // additional client initialization here that cannot fail, if needed
    //

    return true;
}

// initial set up of a DHCP message;
// sets the message type
// returns:
//       < 0  - error
//      TCPIP_DHCPV6_IA_SUBSTATE_RES_PENDING - pending operation, retry
//      TCPIP_DHCPV6_IA_SUBSTATE_RES_OK - success
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT _DHCPV6Ia_TxMsgSetup(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_CLIENT_MSG_TYPE cliMsgType)
{
    TCPIP_DHCPV6_CLIENT_DCPT* pParent = pIa->pParent;

    if(pIa->msgBuffer == 0)
    {   // try to get a message buffer for this TX op
        TCPIP_DHCPV6_MSG_BUFFER* pBuffer = (TCPIP_DHCPV6_MSG_BUFFER*)TCPIP_Helper_SingleListHeadRemove(&pParent->buffFreeList);

        if(pBuffer == 0)
        {   // failed
            pParent->txBuffFailCnt++;
            return TCPIP_DHCPV6_IA_SUBSTATE_RES_PENDING;
        }
        pBuffer->owner = pIa;
        pIa->msgBuffer = pBuffer;
    }
    else
    {   // shouldn't happen ?
        _DHCPV6DbgCond(false, __func__, __LINE__);
        _DHCPV6AssertCond(pIa->msgBuffer->owner == pIa, __func__, __LINE__);
    }

    pIa->msgBuffer->msgLen = 0;

    _DHCPV6TraceBuff(pIa, pIa->msgBuffer);

    // common messages initialization
    pIa->cliMsgType = cliMsgType;
    // generate new transaction
    pIa->transactionId.Val = SYS_RANDOM_PseudoGet(); 
    pIa->transactionId.v[0] = 0;    // use the 3 MSB only
    //
    pIa->pOroOptMask = &DHCPV6_ORO_OPTION_MASK_TBL; 
    memset(&pIa->msgTxDcpt, 0, sizeof(pIa->msgTxDcpt));
    _DHCPV6Ia_SetBoundTimes(pIa);

    return TCPIP_DHCPV6_IA_SUBSTATE_RES_OK;
}


// selects a server based on received advertisments
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT _DHCPV6Ia_AdvertiseSelect(TCPIP_DHCPV6_IA_DCPT* pIa, bool isTmo)
{
    int16_t maxPref, currPref, serverPref = 0;
    TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer;
    TCPIP_DHCPV6_MSG_BUFFER* pMaxMsg;       // max preference message
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes;
    TCPIP_DHCPV6_SERVER_STATUS_CODE serverStatCode;
    TCPIP_DHCPV6_MSG_BUFFER* pSelMsg = 0;   // the selected message
    _DHCPV6DbgDeclareList(otherIaList);
    _DHCPV6DbgDeclareList (thisIaList);
    _DHCPV6DbgDeclareList (thisIaPriList);


    _DHCPV6DbgCheckEmptyList(&otherIaList);
    _DHCPV6DbgCheckEmptyList(&thisIaList);
    _DHCPV6DbgCheckEmptyList(&thisIaPriList);

    TCPIP_DHCPV6_CLIENT_DCPT* pParent = pIa->pParent;
    
    if(TCPIP_Helper_SingleListIsEmpty(&pParent->advertiseList))
    {   // nothing to look for
        return TCPIP_DHCPV6_IA_SUBSTATE_RES_PENDING;
    }

    // check the TCPIP_DHCPV6_OPT_CODE_RAPID_COMMIT option
    // cliMsgType should be TCPIP_DHCPV6_CLIENT_MSG_TYPE_SOLICIT! 
    TCPIP_DHCPV6_OPTION_MASK_SET_0* pMsgMask = DHCPV6_MSG_OPTION_CURR_MASK_TBL + pIa->cliMsgType;
    // when RAPID_COMMIT set, both ADV and REPLY messages need to be processed!
    bool isRapidCommit = (pMsgMask->optionSet[0] & (1 << TCPIP_DHCPV6_OPT_CODE_RAPID_COMMIT)) != 0;

    
    // check the received advertisments

    while((pMsgBuffer = (TCPIP_DHCPV6_MSG_BUFFER*)TCPIP_Helper_SingleListHeadRemove(&pParent->advertiseList)) != 0)
    {
        // check this message concerns this IA
        if(_DHCPV6Ia_InMsgBuffer(pMsgBuffer, pIa, false) == 0)
        {   // not this IA
            TCPIP_Helper_SingleListTailAdd(&otherIaList, (SGL_LIST_NODE*)pMsgBuffer);
            continue;
        }

        // belongs to this IA 
        // check the IA for a status code option
        serverStatCode = _DHCPV6MsgGet_IaOptStatusCode(0, pMsgBuffer, pIa, 0, 0);
        if(serverStatCode != TCPIP_DHCPV6_SERVER_STAT_EXT_ERROR)
        {   // got a server code
            if(serverStatCode == TCPIP_DHCPV6_SERVER_STAT_NO_ADDRS_AVAIL)
            {   // no valid addresses for this IA
                _DHCPV6Ia_MsgInvalidate(pIa, pMsgBuffer);
                int nIas = _DHCPV6MsgFind_InUseIAs(pMsgBuffer, pParent, pIa);
                if(nIas == 0)
                {   // nothing else in this message
                    TCPIP_Helper_SingleListTailAdd(&pParent->buffFreeList, (SGL_LIST_NODE*)pMsgBuffer);
                }
                else
                {
                    TCPIP_Helper_SingleListTailAdd(&otherIaList, (SGL_LIST_NODE*)pMsgBuffer);
                }
                _DHCPV6DbgIAIn_PrintFailed("Ad Select", pMsgBuffer, pIa, "IA No Addrs");
                continue;
            }
        }
        
        // process this advertisment
        TCPIP_Helper_SingleListTailAdd(&thisIaList, (SGL_LIST_NODE*)pMsgBuffer);

        if((serverPref = _DHCPV6MsgGet_ServerPref(pMsgBuffer)) >= 0)
        {
            if(serverPref >= TCPIP_DHCPV6_FORCED_SERVER_PREFERENCE)
            {   // found server; copy all the info from this message to the IA
                pSelMsg = pMsgBuffer;
                break;
            }
        }
        else if(isRapidCommit)
        {
            if(_DHCPV6OptionFind_RapidCommit(pMsgBuffer))
            {
                pSelMsg = pMsgBuffer;
                break;
            }
        }
    }

    if(pSelMsg == 0)
    {   // couldn't find any server; check if we must select one!
        if(isTmo || pIa->msgTxDcpt.rc > 1)
        {   // either tmo or it's a retry; for a retry any advertisment will do!
            pMaxMsg = 0;   // server with max preference
            maxPref = -1;

            while((pMsgBuffer = (TCPIP_DHCPV6_MSG_BUFFER*)TCPIP_Helper_SingleListHeadRemove(&thisIaList)) != 0)
            {
                serverPref = _DHCPV6MsgGet_ServerPref(pMsgBuffer);
                currPref = serverPref >= 0 ? serverPref : 0;
                if(currPref >= maxPref)
                {
                    maxPref = currPref;
                    pMaxMsg = pMsgBuffer;
                }
                TCPIP_Helper_SingleListTailAdd(&thisIaPriList, (SGL_LIST_NODE*)pMsgBuffer);
            }

            if(pMaxMsg)
            {
                pSelMsg = pMaxMsg;
            }
        }
    }


    if(pSelMsg)
    {   // found our server
        subRes = _DHCPV6Ia_AdvertiseCopy(pSelMsg, pIa);
        // done with this IA
        _DHCPV6Ia_ListPurge(&pParent->advertiseList, pIa, false);  
        _DHCPV6Ia_ListPurge(&thisIaList, pIa, false);
        _DHCPV6Ia_ListPurge(&thisIaPriList, pIa, false);
        _DHCPV6DbgIAIn_PrintPassed("Ad Selected", pMsgBuffer, pIa, serverPref);
    }
    else
    {
        subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_NO_ACTION;
    }

    // re-add the remaining messages
    TCPIP_Helper_SingleListAppend(&pParent->advertiseList, &otherIaList);
    TCPIP_Helper_SingleListAppend(&pParent->advertiseList, &thisIaList);
    TCPIP_Helper_SingleListAppend(&pParent->advertiseList, &thisIaPriList);


    _DHCPV6DbgCheckEmptyList(&otherIaList);
    _DHCPV6DbgCheckEmptyList(&thisIaList);
    _DHCPV6DbgCheckEmptyList(&thisIaPriList);

    if(subRes < 0)
    {   // some error
        return subRes;
    }

    return (pSelMsg == 0) ? TCPIP_DHCPV6_IA_SUBSTATE_RES_PENDING : TCPIP_DHCPV6_IA_SUBSTATE_RES_OK;
}


// copy the advertise info from this selected message buffer to the IA
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT _DHCPV6Ia_AdvertiseCopy(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_IA_DCPT* pIa)
{

    // get the server ID

    if(!_DHCPV6MsgGet_Duid(&pIa->serverDuid, pMsgBuffer, false))
    {
        return TCPIP_DHCPV6_IA_SUBSTATE_RES_ERROR_FATAL;
    }

    // update the IA params
    if(!_DHCPV6MsgGet_IaBody(&pIa->iaBody, pMsgBuffer, pIa, true))
    {
        return TCPIP_DHCPV6_IA_SUBSTATE_RES_ERROR_FATAL;
    }


    // update the IA address and lifetimes
    if(!_DHCPV6MsgGet_IaAddress(&pIa->addBody, pMsgBuffer, pIa))
    {
        return TCPIP_DHCPV6_IA_SUBSTATE_RES_ERROR_FATAL;
    }

    return TCPIP_DHCPV6_IA_SUBSTATE_RES_OK;
}

// processes the server reply when in request/renew/decline/release state
// (TCPIP_DHCPV6_IA_STATE_REQUEST/TCPIP_DHCPV6_IA_STATE_RENEW/TCPIP_DHCPV6_IA_STATE_DECLINE/TCPIP_DHCPV6_IA_STATE_RELEASE , TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY)
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT _DHCPV6Ia_ReplyMsgSrvMatchProcess(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes;
    TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer;
    TCPIP_DHCPV6_SERVER_STATUS_CODE serverStatCode;
    bool processMsg;
    void*   pOptIa;
    _DHCPV6DbgDeclareList(otherIaList);
    _DHCPV6DbgDeclareList (discardIaList);
    _DHCPV6DbgDeclareFailReason(failReason);

    _DHCPV6DbgCheckEmptyList(&otherIaList);
    _DHCPV6DbgCheckEmptyList(&discardIaList);

    TCPIP_DHCPV6_CLIENT_DCPT* pParent = pIa->pParent;

    
    subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_PENDING;  // in case we don't find any reply to this IA

    while((pMsgBuffer = (TCPIP_DHCPV6_MSG_BUFFER*)TCPIP_Helper_SingleListHeadRemove(&pParent->replyList)) != 0)
    {
        // check this message concerns this IA
        if((pOptIa = _DHCPV6Ia_InMsgBuffer(pMsgBuffer, pIa, true)) == 0)
        {   // not this IA
            TCPIP_Helper_SingleListTailAdd(&otherIaList, (SGL_LIST_NODE*)pMsgBuffer);
            continue;
        }

        // message belonging to this IA 
        processMsg = false;
        if(pIa->iaState == TCPIP_DHCPV6_IA_STATE_RELEASE || pIa->iaState == TCPIP_DHCPV6_IA_STATE_DECLINE)
        {   // any reply is just fine
            _DHCPV6Ia_Remove(pIa);
            subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_NO_ACTION;
            _DHCPV6DbgIAIn_PrintPassed("Reply Rel/Decl", pMsgBuffer, pIa, 0);
        }
        else if((serverStatCode = _DHCPV6MsgGet_StatusCode(0, pMsgBuffer, 0, 0)) != TCPIP_DHCPV6_SERVER_STAT_EXT_ERROR)
        {   // check if we have a server status code
            pParent->lastStatusCode = serverStatCode;
            if(serverStatCode == TCPIP_DHCPV6_SERVER_STAT_UNSPEC_FAIL)
            {
                subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_RETRANSMIT;
                _DHCPV6DbgSetFailReason(failReason, "Unspec Fail");
            }
            else if(serverStatCode == TCPIP_DHCPV6_SERVER_STAT_USE_MULTICAST)
            {
                pIa->flags.iaUnicast = 0;
                subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_RETRANSMIT;
                _DHCPV6DbgSetFailReason(failReason, "Unicast Fail");
            }
            else if(serverStatCode == TCPIP_DHCPV6_SERVER_STAT_NOT_ON_LINK)
            {   // for REQUEST
                subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_RESTART;
                _DHCPV6DbgSetFailReason(failReason, "Not on Link");
            }
            else
            {   // can process further
                processMsg = true;
            }
        }
        else
        {   // if no status code then process further
            processMsg = true;
        }

        while(processMsg)
        {
            // check the IA for a status code option
            serverStatCode = _DHCPV6MsgGet_IaOptStatusCode(pIa->lastStatusMsg, pMsgBuffer, pIa, sizeof(pIa->lastStatusMsg), 0);
            if(serverStatCode != TCPIP_DHCPV6_SERVER_STAT_EXT_ERROR)
            {
                pIa->lastStatusCode = serverStatCode;
                if(serverStatCode == TCPIP_DHCPV6_SERVER_STAT_NO_ADDRS_AVAIL)
                {
                    subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_RESTART;
                    _DHCPV6DbgSetFailReason(failReason, "No IA Addrs");
                    processMsg = false;
                    break;
                }
                else if(pIa->iaState == TCPIP_DHCPV6_IA_STATE_RENEW)
                {
                    if(serverStatCode == TCPIP_DHCPV6_SERVER_STAT_NO_BINDING)
                    {
                        _DHCPV6Ia_SetState(pIa, TCPIP_DHCPV6_IA_STATE_REQUEST, 0);
                        subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_JUMP;
                        processMsg = false;
                        _DHCPV6DbgSetFailReason(failReason, "No Binding");
                        break;
                    }
                }
            }

            if(!_DHCPV6MsgGet_LeaseParams(pIa, pMsgBuffer, true))
            {   // failed
                subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_ERROR_FATAL;
                processMsg = false;
                _DHCPV6DbgSetFailReason(failReason, "No Lease Params");
            }
            else
            {
                _DHCPV6Ia_SetState(pIa, TCPIP_DHCPV6_IA_STATE_DAD, 0);
                subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_JUMP;
                // acquired the lease, we're done
            }
            break;
        }

        // done with this message
        TCPIP_Helper_SingleListTailAdd(&discardIaList, (SGL_LIST_NODE*)pMsgBuffer);

        if(processMsg)
        {
            _DHCPV6DbgIAIn_PrintPassed("Reply Match", pMsgBuffer, pIa, serverStatCode);
        }
        else
        {
            _DHCPV6DbgIAIn_PrintFailed("Reply Match", pMsgBuffer, pIa, failReason);
        }

        break;
    }

    if(subRes != TCPIP_DHCPV6_IA_SUBSTATE_RES_PENDING)
    {   // done with this IA
        _DHCPV6Ia_ListPurge(&pParent->replyList, pIa, false);  
    }
    _DHCPV6Ia_ListPurge(&discardIaList, pIa, true);

    // re-add the remaining messages
    TCPIP_Helper_SingleListAppend(&pParent->replyList, &otherIaList);

    _DHCPV6DbgCheckEmptyList(&otherIaList);
    _DHCPV6DbgCheckEmptyList(&discardIaList);

    return subRes;
    
}

static bool _DHCPV6MsgGet_LeaseParams(TCPIP_DHCPV6_IA_DCPT* pDstIa, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, bool serverMatch)
{
    TCPIP_DHCPV6_OPTION_UNICAST* pOptUnicast;

    // get the IA and address 
    if(!_DHCPV6MsgGet_IaAddress(&pDstIa->addBody, pSrcBuffer, pDstIa) || !_DHCPV6MsgGet_IaBody(&pDstIa->iaBody, pSrcBuffer, pDstIa, serverMatch))
    {
        _DHCPV6DbgCond(false, __func__, __LINE__);
        return false;
    }

    // process the message
    // save the last known life times
    pDstIa->lastPrefLTime = pDstIa->addBody.prefLTime;
    pDstIa->lastValidLTime = pDstIa->addBody.validLTime;

    // check server unicast option!
    if((pOptUnicast = _DHCPV6OptionFind_ServerUnicast(pSrcBuffer)) != 0)
    {
        pDstIa->flags.iaUnicast = 1;
        pDstIa->serverUcastAddr = pOptUnicast->serverAddr;
    }

    _DHCPV6Ia_SetTimeValues(pDstIa, true);


    return true;
}


// processes the server reply when in confirm state
// (TCPIP_DHCPV6_IA_STATE_CONFIRM , TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY)
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT _DHCPV6Ia_ReplyConfirmProcess(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes;
    TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer;
    TCPIP_DHCPV6_SERVER_STATUS_CODE serverStatCode;
    bool stopProcess;
    int  nProcMsgs;
    _DHCPV6DbgDeclareList(otherIaList);
    _DHCPV6DbgDeclareList (discardIaList);

    _DHCPV6DbgCheckEmptyList(&otherIaList);
    _DHCPV6DbgCheckEmptyList(&discardIaList);

    TCPIP_DHCPV6_CLIENT_DCPT* pParent = pIa->pParent;


    stopProcess = false;
    nProcMsgs = 0;
    
    subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_PENDING;  // in case we don't find any reply to this IA

    while((pMsgBuffer = (TCPIP_DHCPV6_MSG_BUFFER*)TCPIP_Helper_SingleListHeadRemove(&pParent->replyList)) != 0)
    {
        // check this message concerns this IA
        if(_DHCPV6MsgGet_TransactionId(pMsgBuffer) != pIa->transactionId.Val)
        {   // not this IA
            TCPIP_Helper_SingleListTailAdd(&otherIaList, (SGL_LIST_NODE*)pMsgBuffer);
            continue;
        }

        // message belonging to this IA 
        nProcMsgs++;
        serverStatCode = _DHCPV6MsgGet_StatusCode(0, pMsgBuffer, 0, 0);
        if(serverStatCode == TCPIP_DHCPV6_SERVER_STAT_SUCCESS)
        {   // addresses still valid
            // get this server ID
            if(!_DHCPV6MsgGet_Duid(&pIa->serverDuid, pMsgBuffer, false))
            {
                subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_ERROR_FATAL;
            }
            else
            {
                _DHCPV6Ia_RestoreTimeValues(pIa);
                _DHCPV6Ia_SetState(pIa, TCPIP_DHCPV6_IA_STATE_BOUND, 0);
                subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_JUMP;
            }
            stopProcess = true;
        }
        else if(serverStatCode != TCPIP_DHCPV6_SERVER_STAT_NOT_ON_LINK)
        {   
            subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_RETRANSMIT;
        }

        // done with this message
        TCPIP_Helper_SingleListTailAdd(&discardIaList, (SGL_LIST_NODE*)pMsgBuffer);

        if(stopProcess)
        {   // got what we wanted; there shouldn't be other messages of interest here
            break;
        }
    }

    if(nProcMsgs)
    {   // done with this IA
        _DHCPV6Ia_ListPurge(&pParent->replyList, pIa, false);  
    }
    _DHCPV6Ia_ListPurge(&discardIaList, pIa, true);

    // re-add the remaining messages
    TCPIP_Helper_SingleListAppend(&pParent->replyList, &otherIaList);

    _DHCPV6DbgCheckEmptyList(&otherIaList);
    _DHCPV6DbgCheckEmptyList(&discardIaList);

    return subRes;
    
}

// processes the server reply when in rebind state
// (TCPIP_DHCPV6_IA_STATE_REBIND , TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY)
static TCPIP_DHCPV6_IA_SUBSTATE_RESULT _DHCPV6Ia_ReplyRebindProcess(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_IA_SUBSTATE_RESULT subRes;
    TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer;
    TCPIP_DHCPV6_SERVER_STATUS_CODE serverStatCode;
    TCPIP_DHCPV6_OPTION_IA_ADDR* pIaAddr;
    TCPIP_DHCPV6_OPTION_IA_ADDR_BODY addBody;
    bool procMessage, stopProcess;
    int  nProcMsgs;
    void*   pOptIa;
    _DHCPV6DbgDeclareList(otherIaList);
    _DHCPV6DbgDeclareList (discardIaList);

    _DHCPV6DbgCheckEmptyList(&otherIaList);
    _DHCPV6DbgCheckEmptyList(&discardIaList);

    TCPIP_DHCPV6_CLIENT_DCPT* pParent = pIa->pParent;


    stopProcess = false;
    nProcMsgs = 0;
    
    subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_PENDING;  // in case we don't find any reply to this IA

    while((pMsgBuffer = (TCPIP_DHCPV6_MSG_BUFFER*)TCPIP_Helper_SingleListHeadRemove(&pParent->replyList)) != 0)
    {
        // check this message concerns this IA
        if((pOptIa = _DHCPV6Ia_InMsgBuffer(pMsgBuffer, pIa, false)) == 0)
        {   // not this IA
            TCPIP_Helper_SingleListTailAdd(&otherIaList, (SGL_LIST_NODE*)pMsgBuffer);
            continue;
        }

        // message belonging to this IA 
        nProcMsgs++;
        procMessage = true;

        if((serverStatCode = _DHCPV6MsgGet_StatusCode(0, pMsgBuffer, 0, 0)) != TCPIP_DHCPV6_SERVER_STAT_EXT_ERROR)
        {   // check if we have a server status code
            pParent->lastStatusCode = serverStatCode;
            if(serverStatCode != TCPIP_DHCPV6_SERVER_STAT_SUCCESS)
            {
                subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_RESTART;
                procMessage = false;
            }
        }

        while(procMessage)
        {
            // check the IA for a status code option
            serverStatCode = _DHCPV6MsgGet_IaOptStatusCode(pIa->lastStatusMsg, pMsgBuffer, pIa, sizeof(pIa->lastStatusMsg), 0);
            if(serverStatCode != TCPIP_DHCPV6_SERVER_STAT_EXT_ERROR)
            {
                pIa->lastStatusCode = serverStatCode;
                if(serverStatCode == TCPIP_DHCPV6_SERVER_STAT_NO_BINDING)
                {
                    _DHCPV6Ia_SetState(pIa, TCPIP_DHCPV6_IA_STATE_REQUEST, 0);
                    subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_JUMP;
                    break;
                }
            }

            // get this server ID
            if(!_DHCPV6MsgGet_Duid(&pIa->serverDuid, pMsgBuffer, false))
            {
                subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_ERROR_FATAL;
                break;
            }

            pIaAddr = (TCPIP_DHCPV6_OPTION_IA_ADDR*)_DHCPV6OptionFind_IaOpt(pMsgBuffer, pIa, TCPIP_DHCPV6_OPT_CODE_IA_ADDR);
            if(pIaAddr == 0)
            {   // should not happen; ignore this message
                _DHCPV6DbgCond(false, __func__, __LINE__);
                break;
            }

            stopProcess = true;
            if(!_DHCPV6OptionGet_IaAddress(&addBody, pIaAddr))
            {   // rebind with life times == 0! explicit message that we have to start all over
                subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_RESTART;
                break;
            }

            if(!_DHCPV6MsgGet_LeaseParams(pIa, pMsgBuffer, false))
            {  
                subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_ERROR_FATAL;
            }
            else
            {
                _DHCPV6Ia_SetState(pIa, TCPIP_DHCPV6_IA_STATE_DAD, 0);
                subRes = TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_JUMP;
            }

            break;
        }

        // done with this message
        TCPIP_Helper_SingleListTailAdd(&discardIaList, (SGL_LIST_NODE*)pMsgBuffer);

        if(stopProcess)
        {   // acquired the lease; there shouldn't be other messages of interest here
            break;
        }
    }

    if(nProcMsgs)
    {   // done with this IA
        _DHCPV6Ia_ListPurge(&pParent->replyList, pIa, false);  
    }
    _DHCPV6Ia_ListPurge(&discardIaList, pIa, true);

    // re-add the remaining messages
    TCPIP_Helper_SingleListAppend(&pParent->replyList, &otherIaList);

    _DHCPV6DbgCheckEmptyList(&otherIaList);
    _DHCPV6DbgCheckEmptyList(&discardIaList);

    return subRes;
    
}
// gets the IA address from a IA Address Option
static bool _DHCPV6OptionGet_IaAddress(TCPIP_DHCPV6_OPTION_IA_ADDR_BODY* pAddBody, TCPIP_DHCPV6_OPTION_IA_ADDR* pSrcIaAddr)
{
    *pAddBody = pSrcIaAddr->body;
    pAddBody->prefLTime = TCPIP_Helper_ntohl(pAddBody->prefLTime);
    pAddBody->validLTime = TCPIP_Helper_ntohl(pAddBody->validLTime);

    // discard addresses which have a invalid lifetimes
    return (pAddBody->validLTime != 0 && pAddBody->prefLTime <= pAddBody->validLTime);
} 

// gets the IA body from a IA Option
static bool _DHCPV6OptionGet_IaBody(TCPIP_DHCPV6_IA_BODY* pIaBody, void* pSrcOptIa, TCPIP_DHCPV6_IA_TYPE type)
{
    if(type == TCPIP_DHCPV6_IA_TYPE_IANA)
    {
        uint32_t    t1, t2;
        pIaBody->ianaBody.iaid = TCPIP_Helper_ntohl(((TCPIP_DHCPV6_OPTION_IANA*)pSrcOptIa)->body.iaid);

        t1 = TCPIP_Helper_ntohl(((TCPIP_DHCPV6_OPTION_IANA*)pSrcOptIa)->body.t1);
        t2 = TCPIP_Helper_ntohl(((TCPIP_DHCPV6_OPTION_IANA*)pSrcOptIa)->body.t2);
        // mimimal sanity check
        if(t1 > 0 && t2 > 0)
        {
            if(t1 > t2)
            {   // will use defaults if nonvalid
                t1 = t2 = 0;
            }
        }

        pIaBody->ianaBody.t1 = t1;
        pIaBody->ianaBody.t2 = t2;
    }
    else
    {
        pIaBody->iataBody.iaid = ((TCPIP_DHCPV6_OPTION_IATA*)pSrcOptIa)->body.iaid;
    }

    return true;
}

// sets the IA default times
static void _DHCPV6Ia_SetTimeValues(TCPIP_DHCPV6_IA_DCPT* pIa, bool setAcqTime)
{
    if(setAcqTime)
    {
        pIa->iaBody.tAcquire = _DHCPV6SecondCountGet(); 
    }
    
    if(pIa->iaBody.type == TCPIP_DHCPV6_IA_TYPE_IANA)
    {
        if(pIa->iaBody.ianaBody.t1 == 0)
        {   // use the default value
            pIa->iaBody.ianaBody.t1 = pIa->pParent->defaultIanaT1;
        }
        if(pIa->iaBody.ianaBody.t2 == 0)
        {   // use the default value
            pIa->iaBody.ianaBody.t2 = pIa->pParent->defaultIanaT2;
        }

        pIa->lastT1 = pIa->iaBody.ianaBody.t1;
        pIa->lastT2 = pIa->iaBody.ianaBody.t2;
    }
    // else IATA uses global values

}

static void _DHCPV6Ia_RestoreTimeValues(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    pIa->iaBody.ianaBody.t1 = pIa->lastT1;
    pIa->iaBody.ianaBody.t2 = pIa->lastT2;
    pIa->addBody.prefLTime = pIa->lastPrefLTime;
    pIa->addBody.validLTime = pIa->lastValidLTime;

}



static void _DHCPV6Ia_SetBoundTimes(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    uint32_t tMrdExpireSec = 0;

    TCPIP_DHCPV6_CLIENT_MSG_TYPE cliMsgType = pIa->cliMsgType;

    pIa->msgTxDcpt.bounds = dhcpv6MessageBoundsTbl[cliMsgType];
    // use the MRT overrides, if needed
    if(cliMsgType == TCPIP_DHCPV6_CLIENT_MSG_TYPE_SOLICIT)
    {
        pIa->msgTxDcpt.bounds.mrt = pIa->pParent->solMaxRt;
    }
    else if(cliMsgType == TCPIP_DHCPV6_CLIENT_MSG_TYPE_INFO_REQUEST)
    {
        pIa->msgTxDcpt.bounds.mrt = pIa->pParent->infoMaxRt;
    }

    if(pIa->iaState == TCPIP_DHCPV6_IA_STATE_RENEW)
    {   // update MRD to the time until T2
        if(pIa->iaBody.type == TCPIP_DHCPV6_IA_TYPE_IANA)
        {
            tMrdExpireSec = pIa->iaBody.ianaBody.t2;
        }
        else
        {   // IATA use global values
            tMrdExpireSec = pIa->pParent->defaultIataT2;
        }

        if((pIa->pParent->configFlags & TCPIP_DHCPV6_FLAG_IA_IGNORE_REBIND_LTIME) == 0)
        {   // select minimum
            tMrdExpireSec = (tMrdExpireSec > pIa->addBody.validLTime) ? pIa->addBody.validLTime : tMrdExpireSec;
        }

        if(tMrdExpireSec == TCPIP_DHCPV6_TIMEOUT_INFINITE)
        {
            tMrdExpireSec = 0;
        }
    }
    else if(pIa->iaState == TCPIP_DHCPV6_IA_STATE_REBIND)
    {
        if(pIa->addBody.validLTime != TCPIP_DHCPV6_TIMEOUT_INFINITE)
        {
            tMrdExpireSec =  pIa->addBody.validLTime;
        }
    }

    if(tMrdExpireSec != 0)
    {
        uint32_t secCurr = _DHCPV6SecondCountGet();
        uint32_t tMrdDeadLine = pIa->iaBody.tAcquire + tMrdExpireSec;

        _DHCPV6DbgCond(((int32_t)(secCurr - tMrdDeadLine) < 0), __func__, __LINE__);
        pIa->msgTxDcpt.bounds.mrd = tMrdDeadLine - secCurr;
    }
    else
    {
        pIa->msgTxDcpt.bounds.mrd = 0;
    }
}

static bool _DHCPV6Ia_AddressIsExpired(TCPIP_DHCPV6_IA_DCPT* pIa, bool checkPrefLTime)
{   
    uint32_t lTime = checkPrefLTime ? pIa->addBody.prefLTime : pIa->addBody.validLTime; 
    
    if(lTime != TCPIP_DHCPV6_TIMEOUT_INFINITE)
    {
        uint32_t tMrdDeadLine = pIa->iaBody.tAcquire + lTime;
        return (int32_t)(_DHCPV6SecondCountGet() - tMrdDeadLine) >= 0;
    }

    return false;
}

static void _DHCPV6Ia_Remove(TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_CLIENT_DCPT* pClient = pIa->pParent;
    // remove it from the iaStateList
    TCPIP_Helper_DoubleListNodeRemove(pClient->iaStateList + pIa->iaState, (DBL_LIST_NODE*)pIa);
    // and add it to the free list
    TCPIP_Helper_DoubleListTailAdd(&pClient->iaFreeList, (DBL_LIST_NODE*)pIa);
}

// gets the DUID from a Server/Client ID Option
static bool _DHCPV6OptionGet_Duid(TCPIP_DHCPV6_DUID_DCPT* pDstDuidDcpt, TCPIP_DHCPV6_OPTION_ID* pSrcOptId)
{
    int duidLen;
    TCPIP_DHCPV6_DUID_TYPE duidType;

    duidType = (TCPIP_DHCPV6_DUID_TYPE)(int)TCPIP_Helper_ntohs(pSrcOptId->duid[0]);
    duidLen = TCPIP_Helper_ntohs(pSrcOptId->optLen);
    // the TCPIP_DHCPV6_DUID_EN has variable size
    // make sure we have enough space for it
    if((duidType == TCPIP_DHCPV6_DUID_TYPE_EN && duidLen > sizeof(TCPIP_DHCPV6_DUID_EN)) || 
       (duidType == TCPIP_DHCPV6_DUID_TYPE_LLT && duidLen > sizeof(TCPIP_DHCPV6_DUID_LLT)) ||
       (duidType == TCPIP_DHCPV6_DUID_TYPE_LL && duidLen > sizeof(TCPIP_DHCPV6_DUID_LL)))
    {
        _DHCPV6AssertCond(false, __func__, __LINE__);
        return false;
    }

    memcpy(&pDstDuidDcpt->duidBody, pSrcOptId->duid, duidLen);
    pDstDuidDcpt->duidType = duidType;
    pDstDuidDcpt->duidLen = duidLen;

    return true;

}


static bool _DHCPV6MsgGet_Duid(TCPIP_DHCPV6_DUID_DCPT* pDstDuid, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, bool isClient)
{
    // this should be valid; already checked in Receive/Process task!
    TCPIP_DHCPV6_OPTION_CODE optCode = isClient ? TCPIP_DHCPV6_OPT_CODE_CLIENT_ID : TCPIP_DHCPV6_OPT_CODE_SERVER_ID;

    TCPIP_DHCPV6_OPTION_ID* pOptId = (TCPIP_DHCPV6_OPTION_ID*)_DHCPV6OptionFind_OptCode(pSrcBuffer, optCode, 0);
    if(pOptId == 0 || !_DHCPV6OptionGet_Duid(pDstDuid, pOptId))
    {
        _DHCPV6DbgCond(false, __func__, __LINE__);
        return false;
    }

    return true;
}


static bool _DHCPV6MsgGet_IaBody(TCPIP_DHCPV6_IA_BODY* pIaBody, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, TCPIP_DHCPV6_IA_DCPT* pIa, bool serverMatch)
{
    // get the updated T1, T2

    void* pOptIa = _DHCPV6OptionFind_Ia(pSrcBuffer, pIa, serverMatch);
    if(pOptIa == 0 || !_DHCPV6OptionGet_IaBody(pIaBody, pOptIa, pIa->iaBody.type))
    {
        _DHCPV6DbgCond(false, __func__, __LINE__);
        return false;
    }

    return true;
}

static bool _DHCPV6MsgGet_IaAddress(TCPIP_DHCPV6_OPTION_IA_ADDR_BODY* pAddBody, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, TCPIP_DHCPV6_IA_DCPT* pIa)
{
    // get the address with the updated lifetimes
    TCPIP_DHCPV6_OPTION_IA_ADDR* pIaAddr = (TCPIP_DHCPV6_OPTION_IA_ADDR*)_DHCPV6OptionFind_IaOpt(pSrcBuffer, pIa, TCPIP_DHCPV6_OPT_CODE_IA_ADDR);
    if(pIaAddr == 0 || !_DHCPV6OptionGet_IaAddress(pAddBody, pIaAddr))
    {
        _DHCPV6DbgCond(false, __func__, __LINE__);
        return false;
    }

    return true;
}




// client tasks
// 

// transmit procedure
// transmits messages in the pClient->txMsgList
// sets pClient->state for error
static void _DHCPV6Client_TransmitTask(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    TCPIP_DHCPV6_MSG_BUFFER* pTxBuffer;
    TCPIP_DHCPV6_IA_DCPT*    pIa;
    UDP_SOCKET s = pClient->dhcpSkt;
    int     nTxPkts = 0;
    

    while(!TCPIP_Helper_SingleListIsEmpty(&pClient->txMsgList))
    {   // there's something to transmit
        uint16_t nBytes1, nBytes2;

        nBytes1 = TCPIP_UDP_PutIsReady(s);
        if(nBytes1 == 0)
        {   // out of buffer space
            _DHCPV6DbgCond(nTxPkts != 0, __func__, __LINE__);
            break;  // abort
        }

        // get message to transmit
        pTxBuffer = (TCPIP_DHCPV6_MSG_BUFFER*)TCPIP_Helper_SingleListHeadRemove(&pClient->txMsgList);
        pIa = pTxBuffer->owner;
        _DHCPV6AssertCond(pClient == pIa->pParent, __func__, __LINE__);
        // discard whatever is in the socket
        TCPIP_UDP_TxOffsetSet(s, 0, false);
        // make sure there's enough room
        if(nBytes1 < pTxBuffer->msgLen)
        {   // shouldn't happen
            pClient->txSktSpaceCnt++;
            _DHCPV6DbgMsgOut_PrintFailed("Transmit", pTxBuffer, ((TCPIP_DHCPV6_MESSAGE_HEADER*)pTxBuffer->pMsgData)->msg_type, pIa, "Small TX Buffer");
            _DHCPV6DbgCond(false, __func__, __LINE__);
        }
        else
        {   // write the DHCP message
            if(pIa->flags.iaUnicast)
            {
                TCPIP_UDP_DestinationIPAddressSet(s, IP_ADDRESS_TYPE_IPV6, (IP_MULTI_ADDRESS*)pIa->serverUcastAddr.v);
            }
            else
            {
                TCPIP_UDP_DestinationIPAddressSet(s, IP_ADDRESS_TYPE_IPV6, (IP_MULTI_ADDRESS*)DHCPV6_Servers_Address.v);
            }
            nBytes1 = TCPIP_UDP_ArrayPut(s, pTxBuffer->pMsgData, pTxBuffer->msgLen);
            nBytes2 = TCPIP_UDP_Flush(s);

            // Set the Strict PORT option to false and user can receive the 
            // packet from different port number other than 547.
            TCPIP_UDP_OptionsSet(s, UDP_OPTION_STRICT_PORT, (void*)false);

            if(nBytes1 != nBytes2 || nBytes1 != pTxBuffer->msgLen)
            {   // should not happen
                _DHCPV6DbgCond(false, __func__, __LINE__);
                pClient->txSktFlushFailCnt++;
                _DHCPV6DbgMsgOut_PrintFailed("Transmit", pTxBuffer, ((TCPIP_DHCPV6_MESSAGE_HEADER*)pTxBuffer->pMsgData)->msg_type, pIa, "Flush Fail");
            }
            else
            {
                _DHCPV6DbgMsgOut_PrintPassed("Transmit", pTxBuffer, ((TCPIP_DHCPV6_MESSAGE_HEADER*)pTxBuffer->pMsgData)->msg_type , pIa);
            }
            nTxPkts++;
        }

        // done with this buffer
        // but it still belongs to the owner IA
    }
}

// receive procedure
// receives valid DHCP messages and pushes them in the RX queue: pClient->rxMsgList
// sets pClient->state for error
static void _DHCPV6Client_ReceiveTask(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    uint16_t pktSize;
    TCPIP_DHCPV6_MESSAGE_HEADER* pHdr;
    UDP_SOCKET dhcpSkt = pClient->dhcpSkt;
    bool      rxAbort = false;
    TCPIP_DHCPV6_MSG_BUFFER* pRxBuffer = 0;

    while(rxAbort == false && (pktSize = TCPIP_UDP_GetIsReady(dhcpSkt)) != 0)
    {   // UDP message pending
        while(true)
        {
            // try to get a new message buffer for this RX op
            if(pRxBuffer == 0)
            {
                pRxBuffer = (TCPIP_DHCPV6_MSG_BUFFER*)TCPIP_Helper_SingleListHeadRemove(&pClient->buffFreeList);
            }

            if(pRxBuffer == 0)
            {   // failed
                pClient->rxBuffFailCnt++;
                rxAbort = true;
                break;
            }

            if(pktSize > pRxBuffer->bufferSize)
            {   // can't handle this packet
                pClient->rxBuffSmallCnt++;           
                break;
            }

            // get the message
            TCPIP_UDP_ArrayGet(dhcpSkt, pRxBuffer->pMsgData, pktSize);

            // set the message buffer and options
            pHdr = (TCPIP_DHCPV6_MESSAGE_HEADER*)pRxBuffer->pMsgData;
            pRxBuffer->msgLen = pktSize; 
            pRxBuffer->optLen = pRxBuffer->msgLen - sizeof(*pHdr);
            pRxBuffer->pOptData = pHdr->options;

            // minimum sanity check
            if(!_DHCPV6MsgValidate(pRxBuffer, pClient))
            {   // failed
                break;
            }
                    
            // success
            TCPIP_Helper_SingleListTailAdd(&pClient->rxMsgList, (SGL_LIST_NODE*)pRxBuffer);
            pRxBuffer = 0;
            
            // Set the port Server number to 547 , so DHCP V6 Client transmit 
            // the request message with port number 547.
            TCPIP_UDP_RemoteBind(pClient->dhcpSkt,IP_ADDRESS_TYPE_IPV6,dhcpv6ServerPort,0);

            break;
        }
    
        // We are done with this packet
        TCPIP_UDP_Discard(dhcpSkt);
    }

    if(pRxBuffer != 0)
    {   // put it back to free buffers list
        TCPIP_Helper_SingleListTailAdd(&pClient->buffFreeList, (SGL_LIST_NODE*)pRxBuffer);
    }
    
}

static bool _DHCPV6MsgValidate(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    TCPIP_DHCPV6_DUID_DCPT checkDuid;
    TCPIP_DHCPV6_DUID_DCPT* pSrvDuid = 0;
    TCPIP_DHCPV6_MSG_VALID_MASK checkMask = TCPIP_DHCPV6_MSG_VALIDATION_MASK;
    TCPIP_DHCPV6_MSG_VALID_MASK msgMask = TCPIP_DHCPV6_VALID_MASK_NONE;
    
    TCPIP_DHCPV6_MESSAGE_HEADER* pHdr = (TCPIP_DHCPV6_MESSAGE_HEADER*)pMsgBuffer->pMsgData;

    // allow only server originating message types
    if(pHdr->msg_type == TCPIP_DHCPV6_MSG_TYPE_ADVERTISE || pHdr->msg_type == TCPIP_DHCPV6_MSG_TYPE_REPLY)
    {
        msgMask |= TCPIP_DHCPV6_VALID_MASK_TYPE;
        if(pMsgBuffer->optLen > 0)
        {
            msgMask |= TCPIP_DHCPV6_VALID_MASK_OPT_LEN;
            // check for our client ID
            if(_DHCPV6MsgGet_Duid(&checkDuid, pMsgBuffer, true))
            {
                msgMask |= TCPIP_DHCPV6_VALID_MASK_CLIENT_DUID;
                if(_DHCPV6Duid_Compare(&checkDuid, &pClient->clientDuid))
                {
                    msgMask |= TCPIP_DHCPV6_VALID_MASK_CLIENT_THIS;
                    // check for a server ID
                    if(_DHCPV6MsgGet_Duid(&checkDuid, pMsgBuffer, false))
                    {
                        pSrvDuid = pSrvDuid;    // hush xc32 warning
                        pSrvDuid = &checkDuid;
                        msgMask |= TCPIP_DHCPV6_VALID_MASK_SERVER_DUID;
                    }
                }
            }
        } 
    }

    _DHCPV6DbgValidate_Print(pMsgBuffer, msgMask, pHdr->msg_type, pSrvDuid); 
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
static void _DHCPV6Client_ProcessTask(TCPIP_DHCPV6_CLIENT_DCPT* pClient)
{
    TCPIP_DHCPV6_SERVER_STATUS_CODE serverStatCode;
    TCPIP_DHCPV6_MESSAGE_HEADER* pHdr;
    TCPIP_DHCPV6_MSG_BUFFER* pRxBuffer;
    SINGLE_LIST*    pInsertList;
    bool discardMsg;

    while((pRxBuffer = (TCPIP_DHCPV6_MSG_BUFFER*)TCPIP_Helper_SingleListHeadRemove(&pClient->rxMsgList)) != 0)
    {
        discardMsg = false;
        pHdr = (TCPIP_DHCPV6_MESSAGE_HEADER*)pRxBuffer->pMsgData;
        // process the message options!
        _DHCPV6MsgGet_Options(pClient, pRxBuffer);

        // check if we have a server status code
        serverStatCode = _DHCPV6MsgGet_StatusCode(pClient->lastStatusMsg, pRxBuffer, sizeof(pClient->lastStatusMsg), 0);
        if(serverStatCode != TCPIP_DHCPV6_SERVER_STAT_EXT_ERROR)
        {
            pClient->lastStatusCode = serverStatCode;
            if(pHdr->msg_type == TCPIP_DHCPV6_MSG_TYPE_ADVERTISE)
            {
                if(serverStatCode == TCPIP_DHCPV6_SERVER_STAT_NO_ADDRS_AVAIL)
                {   // no addresses for this IA or any other that may be part of this message! discard;
                    discardMsg = true;
                }
            }
        } 

        if(pHdr->msg_type == TCPIP_DHCPV6_MSG_TYPE_ADVERTISE)
        {   // let it through
            pInsertList = &pClient->advertiseList;
        }
        else if(pHdr->msg_type == TCPIP_DHCPV6_MSG_TYPE_REPLY)
        {   // let it through
            pInsertList = &pClient->replyList;
        }
        else
        {   // discard it; shouldn't happen
            _DHCPV6AssertCond(false, __func__, __LINE__);
            discardMsg = true;
        }

        if(discardMsg)
        {
            TCPIP_Helper_SingleListTailAdd(&pClient->buffFreeList, (SGL_LIST_NODE*)pRxBuffer);
            _DHCPV6DbgMsgIn_PrintFailed("Process", pRxBuffer, pHdr->msg_type, "No Addrs Avail");
        }
        else
        {
            TCPIP_Helper_SingleListTailAdd(pInsertList, (SGL_LIST_NODE*)pRxBuffer);
            _DHCPV6DbgMsgIn_PrintPassed("Process", pRxBuffer, pHdr->msg_type);
        }

    }

}

static void _DHCPV6Ia_SetState(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_IA_STATE iaState, TCPIP_DHCPV6_IA_SUBSTATE iaSubState)
{
    bool stateNotify = false;
    bool subNotify = false;
    TCPIP_DHCPV6_CLIENT_DCPT* pClient = pIa->pParent;

    TCPIP_DHCPV6_IA_STATE oldState = pIa->iaState;
    TCPIP_DHCPV6_IA_SUBSTATE oldSubState = pIa->iaSubState;

    _DHCPV6AssertCond((TCPIP_Helper_DoubleListFind(pClient->iaStateList + oldState, (DBL_LIST_NODE*)pIa) != 0), __func__, __LINE__);

    if(iaState != oldState)
    {
        TCPIP_Helper_DoubleListNodeRemove(pClient->iaStateList + oldState, (DBL_LIST_NODE*)pIa);
        TCPIP_Helper_DoubleListTailAdd(pClient->iaStateList + iaState, (DBL_LIST_NODE*)pIa);
    }

    pIa->iaState = iaState;
    pIa->iaSubState = iaSubState;

    if(iaState != oldState)
    {
        TCPIP_DHCPV6_MSG_BUFFER* pTxBuffer = pIa->msgBuffer;
        if(pTxBuffer != 0)
        {   //  only states having a transmit sub-state should have a buffer
            TCPIP_Helper_SingleListTailAdd(&pClient->buffFreeList, (SGL_LIST_NODE*)pTxBuffer);
            pIa->msgBuffer = 0; // a new buffer get when new status involving transmission begins
        }
    }

    if(iaState != oldState)
    {
        stateNotify = true;
    }
    else if((pClient->configFlags & TCPIP_DHCPV6_FLAG_IA_NOTIFY_SUB_STATE) != 0)
    {
        subNotify = iaSubState != oldSubState;
    }

    if(stateNotify | subNotify)
    {
        _DHCPV6Client_Notify(0, pIa, subNotify);
    }
}

static void _DHCPV6Client_SetState(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_CLIENT_STATE cliState)
{

    TCPIP_DHCPV6_CLIENT_STATE oldState = pClient->state;
    if(oldState != cliState)
    {
        pClient->state = cliState;
        _DHCPV6Client_Notify(pClient, 0, 0);
    }
}


// marks as invalid the IAs in this TCPIP_DHCPV6_MSG_BUFFER list that match the IA id.
// if forcePurge is set, then it will purge irrespecive of IA id. 
// if possible, returns the TCPIP_DHCPV6_MSG_BUFFER list to the free list
static void _DHCPV6Ia_ListPurge(SINGLE_LIST* pL, TCPIP_DHCPV6_IA_DCPT* pIa, bool forcePurge)
{
    int nIas;

    TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer;
    TCPIP_DHCPV6_CLIENT_DCPT* pParent = pIa->pParent;
    SINGLE_LIST newList = { 0 };

    while((pMsgBuffer = (TCPIP_DHCPV6_MSG_BUFFER*)TCPIP_Helper_SingleListHeadRemove(pL)) != 0)
    {
        nIas = 0;
        if(forcePurge == false)
        {   // check for the IA belonging to this list
            _DHCPV6Ia_MsgInvalidate(pIa, pMsgBuffer);

            // get the number of used IAs in this message
            nIas = _DHCPV6MsgFind_InUseIAs(pMsgBuffer, pParent, pIa);
        }

        if(forcePurge || nIas == 0)
        {   // we're done with this message
            TCPIP_Helper_SingleListTailAdd(&pParent->buffFreeList, (SGL_LIST_NODE*)pMsgBuffer);
        }
        else
        {   // keep it
            TCPIP_Helper_SingleListTailAdd(&newList, (SGL_LIST_NODE*)pMsgBuffer);
        }
    }

    // append the nodes still in there
    TCPIP_Helper_SingleListAppend(pL, &newList);
}

// invalidates a IA in a message so that a search operation won't find it again
static void _DHCPV6Ia_MsgInvalidate(TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer)
{
    void* pOptIa;

    pOptIa = _DHCPV6OptionFind_Ia(pMsgBuffer, pIa, false);
    if(pOptIa)
    {
        if(pIa->iaBody.type == TCPIP_DHCPV6_IA_TYPE_IANA)
        {
            ((TCPIP_DHCPV6_OPTION_IANA*)pOptIa)->body.iaid = 0;    // mark it as invalid
        }
        else
        {
            ((TCPIP_DHCPV6_OPTION_IATA*)pOptIa)->body.iaid = 0;    // mark it as invalid
        }
    }

}


// match find function for any IA type: IATA or IANA
static bool _DHCPV6OptionMatchFnc_IA(TCPIP_DHCPV6_OPTION_GENERIC* pOptG, TCPIP_DHCPV6_OPTION_CODE srchCode, uint16_t optCode, uint16_t optLen)
{
    return (optCode == TCPIP_DHCPV6_OPT_CODE_IA_NA || optCode == TCPIP_DHCPV6_OPT_CODE_IA_TA);
}

// searches the message buffer for valid IAs belonging to client
// the search occurs only for the IAs being in the same state with the pIa parameter
// returns the number of valid IAs (iaid != 0) belonging to this client
static int _DHCPV6MsgFind_InUseIAs(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_IA_DCPT* pIa)
{
    TCPIP_DHCPV6_OPTION_GENERIC* pOptG;
    uint32_t    srchId;
    TCPIP_DHCPV6_IA_TYPE srchType;
    TCPIP_DHCPV6_MSG_SEARCH_DCPT srchDcpt = { 0 };

    int iaCount = 0;

    while((pOptG = _DHCPV6OptionFind_OptCode_Iter(pMsgBuffer, 0, _DHCPV6OptionMatchFnc_IA, &srchDcpt)) != 0)
    {
        pMsgBuffer = 0;
        if(TCPIP_Helper_ntohs(pOptG->optCode) == TCPIP_DHCPV6_OPT_CODE_IA_NA)
        {
            srchId = TCPIP_Helper_htonl(((TCPIP_DHCPV6_OPTION_IANA*)pOptG)->body.iaid);
            srchType = TCPIP_DHCPV6_IA_TYPE_IANA;
        }
        else
        {
            srchId = TCPIP_Helper_htonl(((TCPIP_DHCPV6_OPTION_IATA*)pOptG)->body.iaid);
            srchType = TCPIP_DHCPV6_IA_TYPE_IATA;
        }

        if(_DHCPV6Client_FindIA(pClient, srchId, srchType, pIa->iaState))
        {
            iaCount++;
        }
    }


    return iaCount;
}

// returns true if the IA client has an IA with this ID and type and in the corresponding state 
static bool _DHCPV6Client_FindIA(TCPIP_DHCPV6_CLIENT_DCPT* pClient, uint32_t iaid, TCPIP_DHCPV6_IA_TYPE type, TCPIP_DHCPV6_IA_STATE srchState)
{
    TCPIP_DHCPV6_IA_DCPT    *pNode;
    DOUBLE_LIST* pSrchList = pClient->iaStateList + srchState;

    for(pNode = (TCPIP_DHCPV6_IA_DCPT*)pSrchList->head; pNode != 0; pNode = pNode->next)
    {
        if(pNode->iaBody.type == type && pNode->iaBody.genId == iaid)
        {
            return true;
        }
    }
    
    return false;
}                

// searches the message buffer to find a vaild IA (IANA/IATA) with this iaid
// returns !0 if found, 0 if not
static void* _DHCPV6OptionFind_Ia(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_IA_DCPT* pIa, bool serverMatch)
{
    if(serverMatch)
    {
        TCPIP_DHCPV6_DUID_DCPT srvDuid;

        if(!_DHCPV6MsgGet_Duid(&srvDuid, pMsgBuffer, false))
        {   // couldn't find a server ID in the message although requested!
            _DHCPV6DbgCond(false, __func__, __LINE__);
            return 0;
        }

        if(!_DHCPV6Duid_Compare(&srvDuid, &pIa->serverDuid))
        {   // ID mismatch
            _DHCPV6DbgCond(false, __func__, __LINE__);
            return false;
        }

        // ok seems to be the correct server
    }

    if(pIa->iaBody.type == TCPIP_DHCPV6_IA_TYPE_IANA)
    {
        TCPIP_DHCPV6_OPTION_IANA* pOptIana = (TCPIP_DHCPV6_OPTION_IANA*)_DHCPV6OptionFind_OptCode(pMsgBuffer, TCPIP_DHCPV6_OPT_CODE_IA_NA, 0);
        if(pOptIana)
        {
            if(TCPIP_Helper_htonl(pOptIana->body.iaid) == pIa->iaBody.genId)
            {
                return pOptIana;
            }
        }
    }
    else
    {
        TCPIP_DHCPV6_OPTION_IATA* pOptIata = (TCPIP_DHCPV6_OPTION_IATA*)_DHCPV6OptionFind_OptCode(pMsgBuffer, TCPIP_DHCPV6_OPT_CODE_IA_TA, 0);
        if(pOptIata)
        {
            if(TCPIP_Helper_htonl(pOptIata->body.iaid) == pIa->iaBody.genId)
            {
                return pOptIata;
            }
        }
    }

    return 0;
}

// compares 2 DUIDs and returns true if they match
static bool _DHCPV6Duid_Compare(const TCPIP_DHCPV6_DUID_DCPT* pDuid1, const TCPIP_DHCPV6_DUID_DCPT* pDuid2)
{
    return (pDuid1->duidType == pDuid2->duidType && pDuid1->duidLen == pDuid2->duidLen && memcmp(&pDuid1->duidBody, &pDuid2->duidBody, pDuid1->duidLen) == 0);
}


// searches for an server status code within the message
// returns TCPIP_DHCPV6_SERVER_STAT_EXT_ERROR if an error,
// otherwise a valid status code
// statusBuffer could be 0 if message not needed
// the message is not NULL terminated!
static TCPIP_DHCPV6_SERVER_STATUS_CODE _DHCPV6MsgGet_StatusCode(uint8_t* statusBuffer, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, uint16_t statusBufferSize, uint16_t* pStatusMsgSize)
{
    TCPIP_DHCPV6_OPTION_STATUS_CODE* pOptStat;

    pOptStat = (TCPIP_DHCPV6_OPTION_STATUS_CODE*)_DHCPV6OptionFind_OptCode(pSrcBuffer, TCPIP_DHCPV6_OPT_CODE_STATUS_CODE, 0);
    if(pOptStat)
    {
        uint16_t optLen = TCPIP_Helper_ntohs(pOptStat->optLen);
        TCPIP_DHCPV6_SERVER_STATUS_CODE statusCode = (TCPIP_DHCPV6_SERVER_STATUS_CODE)(int)TCPIP_Helper_ntohs(pOptStat->statusCode);
        if(optLen >= sizeof(pOptStat->statusCode) && 0 <= statusCode && statusCode <= TCPIP_DHCPV6_SERVER_STAT_MAX_CODE)
        {   // valid
            uint16_t msgLen = optLen - sizeof(pOptStat->statusCode);
            if(pStatusMsgSize)
            {
                *pStatusMsgSize = msgLen;
            }

            if(statusBuffer != 0 && statusBufferSize != 0)
            {
                memset(statusBuffer, 0, statusBufferSize);
                if(msgLen > statusBufferSize)
                {   // truncate
                    msgLen = statusBufferSize;
                }
                memcpy(statusBuffer, pOptStat->statusMsg, msgLen);
            }
            return statusCode;
        }
    }
    return TCPIP_DHCPV6_SERVER_STAT_EXT_ERROR;
}

// searches for an server status code within the IA
// returns TCPIP_DHCPV6_SERVER_STAT_EXT_ERROR if an error,
// otherwise a valid status code
// statusBuffer could be 0 if message not needed
// the message is not NULL terminated!
static TCPIP_DHCPV6_SERVER_STATUS_CODE _DHCPV6MsgGet_IaOptStatusCode(uint8_t* statusBuffer, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, TCPIP_DHCPV6_IA_DCPT* pIa, uint16_t statusBufferSize, uint16_t* pStatusMsgSize)
{
    TCPIP_DHCPV6_MSG_BUFFER optMsgBuffer;
    if(_DHCPV6MsgGet_IaOptBuffer(&optMsgBuffer, pSrcBuffer, pIa))
    {
        return _DHCPV6MsgGet_StatusCode(statusBuffer, &optMsgBuffer, statusBufferSize, pStatusMsgSize);
    }

    return 0;
}


// returns -1 if not found/error
// >= 0 if success
static int16_t _DHCPV6MsgGet_ServerPref(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer)
{
    TCPIP_DHCPV6_OPTION_PREFERENCE* pOptPref;

    pOptPref = (TCPIP_DHCPV6_OPTION_PREFERENCE*)_DHCPV6OptionFind_OptCode(pMsgBuffer, TCPIP_DHCPV6_OPT_CODE_PREFERENCE, 0);
    if(pOptPref)
    {
        uint16_t optLen = TCPIP_Helper_ntohs(pOptPref->optLen);
        if(optLen == sizeof(pOptPref->prefValue))
        {
            return (int16_t)pOptPref->prefValue;
        }
    }

    return -1;
}

// search for a rapid commit option in the message
// returns true if found, false otherwise
static bool _DHCPV6OptionFind_RapidCommit(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer)
{
    TCPIP_DHCPV6_OPTION_RAPID_COMMIT * pRapCommit;

    pRapCommit = (TCPIP_DHCPV6_OPTION_RAPID_COMMIT*)_DHCPV6OptionFind_OptCode(pMsgBuffer, TCPIP_DHCPV6_OPT_CODE_RAPID_COMMIT, 0);
    if(pRapCommit && pRapCommit->optLen == 0)
    {
        return true;
    }

    return false;
}

static TCPIP_DHCPV6_OPTION_UNICAST* _DHCPV6OptionFind_ServerUnicast(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer)
{

    TCPIP_DHCPV6_OPTION_UNICAST* pUnicast;
    pUnicast = (TCPIP_DHCPV6_OPTION_UNICAST*)_DHCPV6OptionFind_OptCode(pMsgBuffer, TCPIP_DHCPV6_OPT_CODE_UNICAST, 0);
    if(pUnicast)
    {
        if(TCPIP_Helper_ntohs(pUnicast->optLen) == sizeof(pUnicast->serverAddr))
        {
            return pUnicast;
        }
    }

    return 0;
}


// find a generic option within IANA/IATA
static TCPIP_DHCPV6_OPTION_GENERIC* _DHCPV6OptionFind_IaOpt(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_IA_DCPT* pIa, TCPIP_DHCPV6_OPTION_CODE srchOpt)
{
    TCPIP_DHCPV6_MSG_BUFFER optMsgBuffer;
    if(_DHCPV6MsgGet_IaOptBuffer(&optMsgBuffer, pMsgBuffer, pIa))
    {
        return _DHCPV6OptionFind_OptCode(&optMsgBuffer, srchOpt, 0);
    }
    return 0;
}

// updates the destination buffer with the options within a IANA/IATA
// Only optLen and pOptData are updated!
static bool _DHCPV6MsgGet_IaOptBuffer(TCPIP_DHCPV6_MSG_BUFFER* pDstBuffer, TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, TCPIP_DHCPV6_IA_DCPT* pIa)
{
    void* pOptIa = _DHCPV6OptionFind_Ia(pSrcBuffer, pIa, false);
    if(pOptIa)
    {   // found the IA
        uint16_t optLen;
        uint8_t* optBuff;

        if(pIa->iaBody.type == TCPIP_DHCPV6_IA_TYPE_IANA)
        {
            TCPIP_DHCPV6_OPTION_IANA* pIana = (TCPIP_DHCPV6_OPTION_IANA*)pOptIa;
            optLen = TCPIP_Helper_ntohs(pIana->optLen) - sizeof(pIana->body);
            optBuff = pIana->ianaOpt;
        }
        else
        {
            TCPIP_DHCPV6_OPTION_IATA* pIata = (TCPIP_DHCPV6_OPTION_IATA*)pOptIa;
            optLen = TCPIP_Helper_ntohs(pIata->optLen) - sizeof(pIata->body);
            optBuff = pIata->iataOpt;
        }

        if(optLen != 0)
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
    TCPIP_DHCPV6_CLIENT_DCPT* pClient = dhcpv6ClientDcpt + TCPIP_STACK_NetIxGet(pNetIf);

    pClient->connEvent |= connEvent;
}

// searches for an option with the specified code inside the message buffer
// returns a valid pointer within the message if found, 0 otherwise
// If the findFnc != 0, it uses this function for comparison
// else it makes a simple comparison using the srchCode
// Note: no data is changed within the message byffer itself - like ntohs/ntohl/etc translations!
// The corresponding _DHCPV6OptionGet_... copies data and adjusts the fields!
//
static TCPIP_DHCPV6_OPTION_GENERIC* _DHCPV6OptionFind_OptCode(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_OPTION_CODE srchCode, _DHCPV6OptionMatchFnc findFnc)
{
    uint8_t* pOptBuff;
    TCPIP_DHCPV6_OPTION_CODE optCode;
    uint16_t optBuffLen;
    uint16_t optDataLen;
    uint16_t optGenLen;
    bool     optFound;

    TCPIP_DHCPV6_OPTION_GENERIC* pOptG = 0;

    if(pMsgBuffer != 0)
    {
        pOptBuff = pMsgBuffer->pOptData;
        optBuffLen = pMsgBuffer->optLen;

        pOptG = (TCPIP_DHCPV6_OPTION_GENERIC*)pOptBuff;
        while(optBuffLen > 0)
        {
            optCode = (TCPIP_DHCPV6_OPTION_CODE)(int)TCPIP_Helper_ntohs(pOptG->optCode);
            optDataLen = TCPIP_Helper_ntohs(pOptG->optLen);
            optGenLen =  sizeof(*pOptG) + optDataLen;

            if(optGenLen > optBuffLen)
            {   // ill formatted option ?
                _DHCPV6DbgCond(false, __func__, __LINE__);
                break;
            }

            if(findFnc != 0)
            {
                optFound = (*findFnc)(pOptG, optCode, optDataLen, srchCode);
            }
            else 
            {
                optFound = optCode == srchCode;
            }

            if(optFound)
            {   // found it
                return pOptG;
            }

            // go to the next option
            pOptG = (TCPIP_DHCPV6_OPTION_GENERIC*)((uint8_t*)pOptG + optGenLen);
            optBuffLen -= optGenLen;
        }
    }

    return 0;
}    

// iterative search for an option code
// searches for an option with the specified code inside the message buffer
// first search needs pMsgBuffer != 0;
// ulterior searches use the updated pSrchDcpt
// pSrchDcpt could be NULL if an iterative search is not needed
// pMsgBuffer == 0 && pSrchDcpt == 0 is an invalid combination
// return a valid pointer within the message if found, 0 otherwise
// If the findFnc != 0, it uses this function for comparison
// else it makes a simple comparison using the srchCode
//
// Note: no data is changed within the option itself (like ntohs/ntohl/etc translations!
// The corresponding _DHCPV6OptionGet_... copies data and adjusts the fields!
//
static TCPIP_DHCPV6_OPTION_GENERIC* _DHCPV6OptionFind_OptCode_Iter(TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, TCPIP_DHCPV6_OPTION_CODE srchCode, _DHCPV6OptionMatchFnc findFnc, TCPIP_DHCPV6_MSG_SEARCH_DCPT* pSrchDcpt)
{
    uint8_t* pOptBuff;
    TCPIP_DHCPV6_OPTION_CODE optCode;
    uint16_t optBuffLen;
    uint16_t optDataLen;
    uint16_t optGenLen;
    bool     optFound;

    TCPIP_DHCPV6_OPTION_GENERIC* pOptG = 0;

    if(pMsgBuffer != 0)
    {
        pOptBuff = pMsgBuffer->pOptData;
        optBuffLen = pMsgBuffer->optLen;
    }
    else if(pSrchDcpt != 0)
    {
        pOptBuff = pSrchDcpt->pOptBuff;
        optBuffLen = pSrchDcpt->optBuffLen;
    }
    else
    {   // invalid
        return 0;
    }

    pOptG = (TCPIP_DHCPV6_OPTION_GENERIC*)pOptBuff;
    while(optBuffLen > 0)
    {
        optCode = (TCPIP_DHCPV6_OPTION_CODE)(int)TCPIP_Helper_ntohs(pOptG->optCode);
        optDataLen = TCPIP_Helper_ntohs(pOptG->optLen);
        optGenLen =  sizeof(*pOptG) + optDataLen;

        if(optGenLen > optBuffLen)
        {   // ill formatted option ?
            _DHCPV6DbgCond(false, __func__, __LINE__);
            break;
        }

        if(findFnc != 0)
        {
            optFound = (*findFnc)(pOptG, optCode, optDataLen, srchCode);
        }
        else 
        {
            optFound = optCode == srchCode;
        }
        
        if(optFound)
        {   // found it
            if(pSrchDcpt)
            {
                pSrchDcpt->pOptBuff = pOptG;
                pSrchDcpt->optBuffLen = optBuffLen;
                pSrchDcpt->remBuffLen = optBuffLen - optDataLen;
            }
            return pOptG;
        }

        // go to the next option
        pOptG = (TCPIP_DHCPV6_OPTION_GENERIC*)((uint8_t*)pOptG + optGenLen);
        optBuffLen -= optGenLen;
    }

    return 0;

}    

static uint32_t _DHCPV6MsgGet_TransactionId(TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer)
{
    TCPIP_UINT32_VAL transId;
    TCPIP_DHCPV6_MESSAGE_HEADER* pHdr = (TCPIP_DHCPV6_MESSAGE_HEADER*)pSrcBuffer->pMsgData;

    memcpy(transId.v, pHdr, sizeof(TCPIP_DHCPV6_MESSAGE_HEADER));
    transId.v[0] = 0;

    return transId.Val;
}


static void* _DHCPV6Ia_InMsgBuffer(TCPIP_DHCPV6_MSG_BUFFER* pSrcBuffer, TCPIP_DHCPV6_IA_DCPT* pIa, bool serverMatch)
{
    if(_DHCPV6MsgGet_TransactionId(pSrcBuffer) == pIa->transactionId.Val)
    {
        return _DHCPV6OptionFind_Ia(pSrcBuffer, pIa, serverMatch);
    }

    return 0;
}

static void _DHCPV6MsgGet_Options(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer)
{
    int ix;
    const _DHCPV6MsgGet_OptionEntry* pEntry;

    pEntry = _DHCPV6MsgGet_OptionRetrieveTbl;
    for(ix = 0; ix < sizeof(_DHCPV6MsgGet_OptionRetrieveTbl) / sizeof(*_DHCPV6MsgGet_OptionRetrieveTbl); ix++, pEntry++)
    {
        if(pEntry->optFnc != 0)
        {
            (*pEntry->optFnc)(pClient, pMsgBuffer, pEntry);
        }
    }
}

static void _DHCPV6MsgGet_DnsServers(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, const _DHCPV6MsgGet_OptionEntry* pEntry)
{
    TCPIP_DHCPV6_OPTION_DNS_SERVERS* pOptDns;

    // process SOL_MAX_RT, if available
    pOptDns = (TCPIP_DHCPV6_OPTION_DNS_SERVERS*)_DHCPV6OptionFind_OptCode(pMsgBuffer, TCPIP_DHCPV6_OPT_CODE_DNS_SERVERS, 0);
    if(pOptDns)
    {
        uint16_t optLen = TCPIP_Helper_ntohs(pOptDns->optLen);
        if( optLen != 0 && (optLen & 0x0f) == 0)
        {
            int dnsIx;
            uint16_t nDnsServers = optLen >> 4;
            if(nDnsServers > sizeof(pClient->dnsServersAdd) / sizeof(*pClient->dnsServersAdd))
            {
                nDnsServers = sizeof(pClient->dnsServersAdd) / sizeof(*pClient->dnsServersAdd);
            }
            IPV6_ADDR* pDst = pClient->dnsServersAdd;
            IPV6_ADDR* pSrc = pOptDns->dnsAddr;

            for(dnsIx = 0; dnsIx < nDnsServers; dnsIx++)
            {
                memcpy(pDst, pSrc, sizeof(*pSrc));
                pDst++; pSrc++;
            }
            pClient->nDnsServers = nDnsServers;
        }
    }

}

static void _DHCPV6MsgGet_DomainList(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, const _DHCPV6MsgGet_OptionEntry* pEntry)
{
    TCPIP_DHCPV6_OPTION_DOMAIN_SEARCH_LIST* pDomList;

    // process SOL_MAX_RT, if available
    pDomList = (TCPIP_DHCPV6_OPTION_DOMAIN_SEARCH_LIST*)_DHCPV6OptionFind_OptCode(pMsgBuffer, TCPIP_DHCPV6_OPT_CODE_DOMAIN_LIST, 0);
    if(pDomList)
    {
        uint16_t optLen = TCPIP_Helper_ntohs(pDomList->optLen);
        if( optLen != 0)
        {
            if(optLen > sizeof(pClient->domainSearchList))
            {
                optLen = sizeof(pClient->domainSearchList);
                _DHCPV6DbgCond(false, __func__, __LINE__);
            }
            memcpy(pClient->domainSearchList, pDomList->searchList, optLen);
            pClient->domainSearchListSize = optLen;
        }
    }

}

static void _DHCPV6MsgGet_SolMaxRt(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, const _DHCPV6MsgGet_OptionEntry* pEntry)
{
    TCPIP_DHCPV6_OPTION_MAX_RT* pOptMaxRt;

    // process SOL_MAX_RT, if available
    pOptMaxRt = (TCPIP_DHCPV6_OPTION_MAX_RT*)_DHCPV6OptionFind_OptCode(pMsgBuffer, TCPIP_DHCPV6_OPT_CODE_MAX_RT, 0);
    if(pOptMaxRt)
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

static void _DHCPV6MsgGet_InfoMaxRt(TCPIP_DHCPV6_CLIENT_DCPT* pClient, TCPIP_DHCPV6_MSG_BUFFER* pMsgBuffer, const _DHCPV6MsgGet_OptionEntry* pEntry)
{
    TCPIP_DHCPV6_OPTION_INFO_MAX_RT* pInfoMaxRt;

    pInfoMaxRt = (TCPIP_DHCPV6_OPTION_INFO_MAX_RT*)_DHCPV6OptionFind_OptCode(pMsgBuffer, TCPIP_DHCPV6_OPT_CODE_INFO_MAX_RT, 0);
    if(pInfoMaxRt)
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


#endif  // defined TCPIP_STACK_USE_IPV6 && defined(TCPIP_STACK_USE_DHCPV6_CLIENT)

