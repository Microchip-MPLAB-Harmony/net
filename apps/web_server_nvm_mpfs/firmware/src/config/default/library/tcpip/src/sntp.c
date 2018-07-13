/*******************************************************************************
  Simple Network Time Protocol (SNTP) Client Version 3

  Summary:
    Module for Microchip TCP/IP Stack

  Description:
    -Locates an NTP Server from public site using DNS
    -Requests UTC time using SNTP and updates SNTPTime structure
     periodically, according to TCPIP_NTP_QUERY_INTERVAL value
    -Reference: RFC 1305
*******************************************************************************/

/*******************************************************************************
File Name:  SNTP.c
Copyright © 2012 released Microchip Technology Inc.  All rights
reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_SNTP

#include "tcpip/src/tcpip_private.h"

#if defined(TCPIP_STACK_USE_SNTP_CLIENT)

// debug symbols
#define TCPIP_SNTP_DEBUG_MASK_BASIC         (0x0001)
#define TCPIP_SNTP_DEBUG_MASK_STATE         (0x0002)
#define TCPIP_SNTP_DEBUG_MASK_ERROR         (0x0004)
#define TCPIP_SNTP_DEBUG_MASK_TIME_STAMP    (0x0008)
#define TCPIP_SNTP_DEBUG_MASK_DNS           (0x0010)

// enable SNTP debugging levels
// #define TCPIP_SNTP_DEBUG_LEVEL  (TCPIP_SNTP_DEBUG_MASK_BASIC | TCPIP_SNTP_DEBUG_MASK_STATE | TCPIP_SNTP_DEBUG_MASK_ERROR | TCPIP_SNTP_DEBUG_MASK_TIME_STAMP | TCPIP_SNTP_DEBUG_MASK_DNS)
#define TCPIP_SNTP_DEBUG_LEVEL  (0)


// Defines the structure of an NTP packet
typedef struct
{
    struct
    {
        uint8_t mode            : 3;  // NTP mode
        uint8_t versionNumber   : 3;  // SNTP version number
        uint8_t leapIndicator   : 2;  // Leap second indicator
    } flags;                          // Flags for the packet

    uint8_t stratum;                  // Stratum level of local clock
    int8_t poll;                      // Poll interval
    int8_t precision;                 // Precision (seconds to nearest power of 2)
    uint32_t root_delay;              // Root delay between local machine and server
    uint32_t root_dispersion;         // Root dispersion (maximum error)
    uint32_t ref_identifier;          // Reference clock identifier
    uint32_t ref_ts_secs;             // Reference timestamp (in seconds)
    uint32_t ref_ts_fraq;             // Reference timestamp (fractions)
    uint32_t orig_ts_secs;            // Origination timestamp (in seconds)
    uint32_t orig_ts_fraq;            // Origination timestamp (fractions)
    uint32_t recv_ts_secs;            // Time at which request arrived at sender (seconds)
    uint32_t recv_ts_fraq;            // Time at which request arrived at sender (fractions)
    uint32_t tx_ts_secs;              // Time at which request left sender (seconds)
    uint32_t tx_ts_fraq;              // Time at which request left sender (fractions)

} NTP_PACKET;

static TCPIP_NET_IF*    pSntpIf = 0;    // we use only one interface for SNTP (for now at least)
static TCPIP_NET_IF*    pSntpDefIf = 0;    // default SNTP interface

static UDP_SOCKET       sntpSocket = INVALID_UDP_SOCKET;    // UDP socket we use

static tcpipSignalHandle sntpSignalHandle = 0;
static int              sntpInitCount = 0;

static char             sntpServerName[TCPIP_NTP_SERVER_MAX_LENGTH + 1];

static uint32_t         sntp_reply_timeout;
static uint32_t         sntp_tstamp_timeout;
static uint32_t         sntp_query_interval;
static uint32_t         sntp_error_interval;

typedef enum
{
    SM_INIT = 0,
    SM_HOME,
    SM_WAIT_DNS,
    SM_DNS_RESOLVED,
    SM_UDP_SEND,
    SM_UDP_RECV,
    SM_SHORT_WAIT,
    SM_WAIT,
    SM_END,         

}TCPIP_SNTP_STATE;


static TCPIP_SNTP_STATE     sntpState = SM_INIT;

// the server address
static IP_MULTI_ADDRESS     ntpServerIP;
static IP_ADDRESS_TYPE      ntpConnection;

static TCPIP_SNTP_EVENT_TIME_DATA   ntpData;    // storing the timestamp, tick and user report data

static TCPIP_SNTP_RESULT    ntpLastError;

static TCPIP_SNTP_EVENT_HANDLER ntpEventHandler;    // the (only) sntp module event handler

// local prototypes

static uint32_t TCPIP_SNTP_CurrTime(uint32_t* pMs);

static void     TCPIP_SNTP_Event(TCPIP_SNTP_EVENT evType, const void* param);

#if (TCPIP_SNTP_DEBUG_LEVEL != 0)
static uint32_t _SntpSecond(void)
{
    static uint32_t sysFreq = 0;

    if(sysFreq == 0)
    {
        sysFreq = SYS_TMR_SystemCountFrequencyGet();
    }

    if(sysFreq != 0)
    {
        return SYS_TMR_SystemCountGet() / sysFreq; 
    }

    return 0;
}
#endif

#if ((TCPIP_SNTP_DEBUG_LEVEL & TCPIP_SNTP_DEBUG_MASK_BASIC) != 0)
volatile int _SNTPStayAssertLoop = 0;
static void _SNTPAssertCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("SNTP Assert: %s, in line: %d\r\n", message, lineNo);
        while(_SNTPStayAssertLoop != 0);
    }
}
// a debug condition, not really assertion
volatile int _SNTPStayCondLoop = 0;
static void _SNTPDbgCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("SNTP Cond: %s, in line: %d\r\n", message, lineNo);
        while(_SNTPStayCondLoop != 0);
    }
}

#else
#define _SNTPAssertCond(cond, message, lineNo)
#define _SNTPDbgCond(cond, message, lineNo)
#endif  // (TCPIP_SNTP_DEBUG_LEVEL & TCPIP_SNTP_DEBUG_MASK_BASIC)

#if ((TCPIP_SNTP_DEBUG_LEVEL & TCPIP_SNTP_DEBUG_MASK_STATE) != 0)
static const char* const _SNTP_DbgState_Tbl[] = 
{
    "init",            //    SM_INIT,
    "home",            //    SM_HOME,
    "wait_dns",        //    SM_WAIT_DNS,
    "dns_solved",      //    SM_DNS_RESOLVED,
    "send",            //    SM_UDP_SEND,
    "recv",            //    SM_UDP_RECV,
    "s_wait",          //    SM_SHORT_WAIT,
    "l_wait",          //    SM_WAIT,
    "end",             //    SM_END,         
};



static TCPIP_SNTP_STATE oldState =  -1;
static void _SNTP_DbgNewState(TCPIP_SNTP_STATE newState)
{
    if(newState != oldState)
    {
        uint32_t sntpSecond = _SntpSecond(); 
        oldState = newState;
        if(newState >= 0 && newState < sizeof(_SNTP_DbgState_Tbl) / sizeof(*_SNTP_DbgState_Tbl))
        {
            SYS_CONSOLE_PRINT("SNTP State: %d - %s, time: %d\r\n", newState, _SNTP_DbgState_Tbl[newState], sntpSecond);
        }
        else
        {
            SYS_CONSOLE_PRINT("SNTP State: %d - %s, time: %d\r\n", newState, "Unknown", sntpSecond);
        }
    }
}

#else
#define _SNTP_DbgNewState(newState)
#endif  // (TCPIP_SNTP_DEBUG_LEVEL & TCPIP_SNTP_DEBUG_MASK_STATE)

#if ((TCPIP_SNTP_DEBUG_LEVEL & TCPIP_SNTP_DEBUG_MASK_ERROR) != 0)
static const char* const _SNTP_DbgResOk_Tbl[] = 
{
    "ok",         // SNTP_RES_OK,            
    "progress",   // SNTP_RES_PROGRESS,
};

static const char* const _SNTP_DbgResError_Tbl[] = 
{
    "Unknown",    // None            
    "busy",       // SNTP_RES_BUSY           
    "t_stale",    // SNTP_RES_TSTAMP_STALE   
    "skt_err",    // SNTP_RES_SKT_ERR        
    "skt_bind",   // SNTP_RES_SKT_BIND_ERR   
    "srv_tmo",    // SNTP_RES_NTP_SERVER_TMO 
    "v_err",      // SNTP_RES_NTP_VERSION_ERR
    "t_err",      // SNTP_RES_NTP_TSTAMP_ERR   
    "sync_err",   // SNTP_RES_NTP_SYNC_ERR        
    "kod_err",    // SNTP_RES_NTP_KOD_ERR        
    "dns_err",    // SNTP_RES_NTP_DNS_ERR        
    "if_err",     // SNTP_RES_NTP_IF_ERR        
    "conn_err",   // SNTP_RES_NTP_CONN_ERR        
};
    
static void _SNTP_DbgNewError(TCPIP_SNTP_RESULT newError)
{
    const char* const* pTbl;
    size_t tblEntries;

    if(newError >= 0)
    {
        pTbl = _SNTP_DbgResOk_Tbl;
        tblEntries = sizeof(_SNTP_DbgResOk_Tbl) / sizeof(*_SNTP_DbgResOk_Tbl);
    }
    else
    {
        newError = -newError;
        pTbl = _SNTP_DbgResError_Tbl;
        tblEntries = sizeof(_SNTP_DbgResError_Tbl) / sizeof(*_SNTP_DbgResError_Tbl);
    }

    uint32_t sntpSecond = _SntpSecond(); 
    if(newError >= 0 && newError < tblEntries)
    {
        SYS_CONSOLE_PRINT("SNTP Error: %d - %s, time: %d\r\n", newError, pTbl[newError], sntpSecond);
    }
    else
    {
        SYS_CONSOLE_PRINT("SNTP Error: %d - %s, time: %d\r\n", newError, "Unknown", sntpSecond);
    }
}
#else
#define _SNTP_DbgNewError(newError)
#endif  // ((TCPIP_SNTP_DEBUG_LEVEL & TCPIP_SNTP_DEBUG_MASK_ERROR) != 0)

#if ((TCPIP_SNTP_DEBUG_LEVEL & TCPIP_SNTP_DEBUG_MASK_TIME_STAMP) != 0)
static __inline__ void __attribute__((always_inline)) _SNTP_DbgNewTimeStamp(uint32_t tStamp)
{
    uint32_t sntpSecond = _SntpSecond(); 
    SYS_CONSOLE_PRINT("SNTP new TStamp: %d, time: %d\r\n", tStamp, sntpSecond);
}
#else
#define _SNTP_DbgNewTimeStamp(tStamp)
#endif  // ((TCPIP_SNTP_DEBUG_LEVEL & TCPIP_SNTP_DEBUG_MASK_TIME_STAMP) != 0)

#if ((TCPIP_SNTP_DEBUG_LEVEL & TCPIP_SNTP_DEBUG_MASK_DNS) != 0)
static void _SNTP_DbgNewDns(const char* srvName, const IP_MULTI_ADDRESS* srvAdd)
{
    char addBuff[42];

    if(srvAdd)
    {
#if defined (TCPIP_STACK_USE_IPV6)           
        if(ntpConnection == IP_ADDRESS_TYPE_IPV6)
        {
            if(!TCPIP_Helper_IPv6AddressToString (&srvAdd->v6Add, addBuff, sizeof(addBuff)))
            {
                strcpy(addBuff, "Invalid IPv6 Address");
            }
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)           
#if defined (TCPIP_STACK_USE_IPV4)           
        if(ntpConnection == IP_ADDRESS_TYPE_IPV4)
        {
            if(!TCPIP_Helper_IPAddressToString (&srvAdd->v4Add, addBuff, sizeof(addBuff)))
            {
                strcpy(addBuff, "Invalid IPv4 Address");
            }
        }
#endif  // defined (TCPIP_STACK_USE_IPV4)           
    }

    uint32_t sntpSecond = _SntpSecond(); 
    if(srvAdd)
    {
        SYS_CONSOLE_PRINT("SNTP solved DNS: %s, address: %s, time: %d\r\n", srvName, addBuff, sntpSecond);
    }
    else
    {
        SYS_CONSOLE_PRINT("SNTP solving %s DNS: %s, time: %d\r\n", (ntpConnection == IP_ADDRESS_TYPE_IPV6) ? "IPv6" : "IPv4", srvName, sntpSecond);
    }
}
#else
#define _SNTP_DbgNewDns(srvName, srvAdd)
#endif  // ((TCPIP_SNTP_DEBUG_LEVEL & TCPIP_SNTP_DEBUG_MASK_DNS) != 0)

static __inline__ void __attribute__((always_inline)) TCPIP_SNTP_SetNewState(TCPIP_SNTP_STATE newState)
{
    _SNTP_DbgNewState(newState);
    sntpState = newState;
}

static __inline__ void __attribute__((always_inline)) TCPIP_SNTP_SetError(TCPIP_SNTP_RESULT newError, TCPIP_SNTP_EVENT evType)
{
    _SNTP_DbgNewError(newError);
    ntpLastError = newError;
    TCPIP_SNTP_Event(evType, (const void*)SYS_TMR_TickCountGet());
}



#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void TCPIP_SNTP_Cleanup(void);
#else
#define TCPIP_SNTP_Cleanup()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static bool TCPIP_SNTP_ProcessPkt(void);

static void TCPIP_SNTP_Process(void);

static void     TCPIP_SNTP_SocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param);

static __inline__ void __attribute__((always_inline)) TCPIP_SNTP_SetIdleState(TCPIP_SNTP_STATE newState)
{
    pSntpIf = 0;
    TCPIP_SNTP_SetNewState(newState);
}


// returns true if the module is idle
// and a new connection can be started
static bool _SntpCanStart(void)
{
    return (sntpState == SM_HOME || sntpState == SM_SHORT_WAIT || sntpState == SM_WAIT);
}

// returns true if the module is idle
// and SNTP parameters changed
static bool _SntpCanChangeParams(void)
{
    return (sntpState == SM_HOME || sntpState == SM_SHORT_WAIT || sntpState == SM_WAIT || sntpState == SM_END);
}

bool TCPIP_SNTP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_SNTP_MODULE_CONFIG* pSNTPConfig)
{

    if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        if(stackCtrl->pNetIf == pSntpIf)
        {   // this interface is going away/re-initialized, etc
            TCPIP_SNTP_SetIdleState(SM_HOME);
        }
        return true;
    }

    // stack init
    while(sntpInitCount == 0)
    {   // first time we run
        // basic sanity check
        if(pSNTPConfig == 0 || (pSNTPConfig->ntp_connection_type != IP_ADDRESS_TYPE_IPV4 && pSNTPConfig->ntp_connection_type != IP_ADDRESS_TYPE_IPV6))
        {
            return false;
        }

#if !defined (TCPIP_STACK_USE_IPV6)
        if(pSNTPConfig->ntp_connection_type == IP_ADDRESS_TYPE_IPV6)
        {   // not supported
            return false;
        }
#endif  // !defined (TCPIP_STACK_USE_IPV6)

#if !defined (TCPIP_STACK_USE_IPV4)
        if(pSNTPConfig->ntp_connection_type == IP_ADDRESS_TYPE_IPV4)
        {   // not supported
            return false;
        }
#endif  // !defined (TCPIP_STACK_USE_IPV4)

        memset(&ntpData, 0, sizeof(ntpData));
        strncpy(sntpServerName, pSNTPConfig->ntp_server, sizeof(sntpServerName) - 1);
        sntpServerName[sizeof(sntpServerName) - 1] = 0;
        ntpConnection = pSNTPConfig->ntp_connection_type;
        sntp_reply_timeout = pSNTPConfig->ntp_reply_timeout;
        sntp_tstamp_timeout = pSNTPConfig->ntp_stamp_timeout;
        sntp_query_interval = pSNTPConfig->ntp_success_interval;
        sntp_error_interval = pSNTPConfig->ntp_error_interval;

        pSntpDefIf = (TCPIP_NET_IF*)TCPIP_STACK_NetHandleGet(pSNTPConfig->ntp_interface);

        // allow changing the connection type at run time
        sntpSocket = TCPIP_UDP_ClientOpen(IP_ADDRESS_TYPE_ANY, TCPIP_NTP_SERVER_REMOTE_PORT, 0);
        if(sntpSocket == INVALID_UDP_SOCKET)
        {
            return false;
        }
        // receiving from multiple SNTP servers
        TCPIP_UDP_OptionsSet(sntpSocket, UDP_OPTION_STRICT_ADDRESS, (void*)false);
        TCPIP_UDP_SignalHandlerRegister(sntpSocket, TCPIP_UDP_SIGNAL_RX_DATA, TCPIP_SNTP_SocketRxSignalHandler, 0);

        // create the SNTP timer
        sntpSignalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_SNTP_Task, TCPIP_NTP_TASK_TICK_RATE); 
        if(sntpSignalHandle == 0)
        {   // cannot create the SNTP timer
            TCPIP_SNTP_Cleanup();
            return false;
        }

        ntpEventHandler = 0;
        break;
    }

    // Reset per interface state machine and flags to default values
    TCPIP_SNTP_SetIdleState(SM_INIT);

    ntpLastError = SNTP_RES_OK; 

    sntpInitCount++;
    return true;

}


#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_SNTP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{

    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN)
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down

    if(sntpInitCount > 0)
    {   // we're up and running
        // interface is going down one way or another
        if(stackCtrl->pNetIf == pSntpIf)
        {   // this interface is going away/re-initialized, etc
            TCPIP_SNTP_SetIdleState(SM_END);
        }

        if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--sntpInitCount == 0)
            {   // all closed
                // release resources
                TCPIP_SNTP_Cleanup();
            }
        }
    }

}

static void TCPIP_SNTP_Cleanup(void)
{
    if(sntpSocket != INVALID_UDP_SOCKET)
    {
        TCPIP_UDP_Close(sntpSocket);
        sntpSocket = INVALID_UDP_SOCKET;
    }

    if(sntpSignalHandle)
    {
        _TCPIPStackSignalHandlerDeregister(sntpSignalHandle);
        sntpSignalHandle = 0;
    }

}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

void TCPIP_SNTP_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != 0)
    { // TMO or RX occurred
        TCPIP_SNTP_Process();
    }

}

// send a signal to the SNTP module that data is available
// no manager alert needed since this normally results as a higher layer (UDP) signal
static void TCPIP_SNTP_SocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_UDP_SIGNAL_RX_DATA)
    {
        _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}


static void TCPIP_SNTP_Process(void)
{
    NTP_PACKET          pkt;
    TCPIP_DNS_RESULT    dnsRes;
    static uint32_t     SNTPTimer;
    TCPIP_NET_IF*       pNetIf;
    bool                dataAvlbl;
    bool                bindRes;


    switch(sntpState)
    {
        case SM_INIT:
            // perform delayed initialization
            // convert seconds to sys ticks
            sntp_reply_timeout *= SYS_TMR_TickCounterFrequencyGet();
            sntp_tstamp_timeout *= SYS_TMR_TickCounterFrequencyGet();
            sntp_query_interval *= SYS_TMR_TickCounterFrequencyGet();
            sntp_error_interval *= SYS_TMR_TickCounterFrequencyGet();
            TCPIP_SNTP_SetNewState(SM_HOME);
            break;

        case SM_HOME:
            pNetIf = _TCPIPStackAnyNetLinked(false);
            if(pNetIf == 0 || _TCPIPStackIsConfig(pNetIf) != 0)
            {   // not yet up and running
                break;
            }

#if defined (TCPIP_STACK_USE_IPV6)           
            if(ntpConnection == IP_ADDRESS_TYPE_IPV6)
            {
                if(TCPIP_Helper_StringToIPv6Address (sntpServerName, &ntpServerIP.v6Add))
                {   // IPv6 address provided
                    TCPIP_SNTP_SetNewState(SM_DNS_RESOLVED);
                    break;
                }
            }
#endif  // defined (TCPIP_STACK_USE_IPV6)
          
#if defined (TCPIP_STACK_USE_IPV4)
            if(ntpConnection == IP_ADDRESS_TYPE_IPV4)
            {
                if(TCPIP_Helper_StringToIPAddress(sntpServerName, &ntpServerIP.v4Add))
                {   // IPv4 address provided
                    TCPIP_SNTP_SetNewState(SM_DNS_RESOLVED);
                    break;
                }
            }
#endif  // defined (TCPIP_STACK_USE_IPV4)

            TCPIP_DNS_Resolve(sntpServerName, ntpConnection == IP_ADDRESS_TYPE_IPV6 ? TCPIP_DNS_TYPE_AAAA : TCPIP_DNS_TYPE_A);
            TCPIP_SNTP_SetNewState(SM_WAIT_DNS);
            break;

        case SM_WAIT_DNS:
            _SNTP_DbgNewDns(sntpServerName, 0);
            dnsRes = TCPIP_DNS_IsResolved(sntpServerName, &ntpServerIP, ntpConnection);
            if(dnsRes == TCPIP_DNS_RES_PENDING)
            {   // ongoing operation;
                break;
            }
            else if(dnsRes < 0)
            {   // some DNS error occurred; retry after waiting a while
                SNTPTimer = SYS_TMR_TickCountGet();
                TCPIP_SNTP_SetNewState(SM_SHORT_WAIT);
                TCPIP_SNTP_SetError(SNTP_RES_NTP_DNS_ERR, TCPIP_SNTP_EVENT_DNS_ERROR);
            }
            else
            {
                TCPIP_SNTP_SetNewState(SM_DNS_RESOLVED);
            }
            break;

        case SM_DNS_RESOLVED:
            _SNTP_DbgNewDns(sntpServerName, &ntpServerIP);
            // select a running interface
            pSntpIf = pSntpDefIf;
            if(!TCPIP_STACK_NetworkIsLinked(pSntpIf))
            {
                pSntpIf = _TCPIPStackAnyNetLinked(true);
            }

            if(pSntpIf == 0)
            {   // wait some more
                TCPIP_SNTP_SetError(SNTP_RES_NTP_IF_ERR, TCPIP_SNTP_EVENT_IF_ERROR); 
                break;
            }
            // bind socket to the (new) connection type 
            TCPIP_UDP_Disconnect(sntpSocket, true);
            bindRes = TCPIP_UDP_RemoteBind(sntpSocket, ntpConnection, TCPIP_NTP_SERVER_REMOTE_PORT, &ntpServerIP);
            if(bindRes)
            {
                bindRes = TCPIP_UDP_SocketNetSet(sntpSocket, pSntpIf);
                TCPIP_UDP_OptionsSet(sntpSocket, UDP_OPTION_STRICT_ADDRESS, (void*)false);
            }

            SNTPTimer = SYS_TMR_TickCountGet();
            if(bindRes == true)
            {
                TCPIP_SNTP_SetNewState(SM_UDP_SEND);
            }
            else
            {
                TCPIP_SNTP_SetNewState(SM_SHORT_WAIT);
                TCPIP_SNTP_SetError(SNTP_RES_NTP_DNS_ERR, TCPIP_SNTP_EVENT_SKT_ERROR);
            }
            break;

        case SM_UDP_SEND:
            // Open up the sending UDP socket
            // Make certain the socket can be written to
            if(!TCPIP_UDP_TxPutIsReady(sntpSocket, sizeof(pkt)))
            {   // Wait no more than 1 sec
                if((SYS_TMR_TickCountGet() - SNTPTimer > 1*SYS_TMR_TickCounterFrequencyGet()))
                {
                    TCPIP_SNTP_SetNewState(SM_DNS_RESOLVED);
                    TCPIP_SNTP_SetError(SNTP_RES_SKT_ERR, TCPIP_SNTP_EVENT_SKT_ERROR); 
                    break;
                }
            }

            // Success
            // Transmit a time request packet
            memset(&pkt, 0, sizeof(pkt));
            pkt.flags.versionNumber = TCPIP_NTP_VERSION;
            pkt.flags.mode = 3;             // NTP Client
            pkt.orig_ts_secs = TCPIP_Helper_htonl(TCPIP_NTP_EPOCH);
            // enable packets RX
            TCPIP_UDP_OptionsSet(sntpSocket, UDP_OPTION_RX_QUEUE_LIMIT, (void*)TCPIP_NTP_RX_QUEUE_LIMIT);
            TCPIP_UDP_ArrayPut(sntpSocket, (uint8_t*) &pkt, sizeof(pkt));
            TCPIP_UDP_Flush(sntpSocket);

            SNTPTimer = SYS_TMR_TickCountGet();
            TCPIP_SNTP_SetNewState(SM_UDP_RECV);
            break;

        case SM_UDP_RECV:
            // Look for a response time packet
            if(!TCPIP_UDP_GetIsReady(sntpSocket))
            {
                if((SYS_TMR_TickCountGet()) - SNTPTimer <= sntp_reply_timeout )
                {   // no timeout yet
                    break;
                }
            }

            // either we have data or timeout
            // set the error state in case the process is not successful
            TCPIP_SNTP_SetNewState(SM_SHORT_WAIT);
            dataAvlbl = false;

            // consume all available data
            while(TCPIP_UDP_GetIsReady(sntpSocket))
            {
                dataAvlbl = true;
                if(TCPIP_SNTP_ProcessPkt())
                {   // successful SNTP packet
                    TCPIP_SNTP_SetNewState(SM_WAIT);
                    break;
                }
            }

            if(!dataAvlbl)
            {
                TCPIP_SNTP_SetError(SNTP_RES_NTP_SERVER_TMO, TCPIP_SNTP_EVENT_SERVER_TMO); 
            }

            // disable RX of further packets
            TCPIP_UDP_OptionsSet(sntpSocket, UDP_OPTION_RX_QUEUE_LIMIT, (void*)0);
            // flush any pending data
            TCPIP_UDP_Disconnect(sntpSocket, true);

            SNTPTimer = SYS_TMR_TickCountGet();
            break;

        case SM_SHORT_WAIT:
            // Attempt to requery the NTP server after a specified timeout
            if(SYS_TMR_TickCountGet() - SNTPTimer > sntp_error_interval)
            {
                TCPIP_SNTP_SetNewState(SM_HOME);
            }
            break;

        case SM_WAIT:
            // Requery the NTP server after a specified timeout
            if(SYS_TMR_TickCountGet() - SNTPTimer > sntp_query_interval)
            {
                TCPIP_SNTP_SetNewState(SM_HOME);
            }

            break;

        default:
            break;
    }

}

// returns true if successful packet
// and current time updated
static bool TCPIP_SNTP_ProcessPkt(void)
{
    NTP_PACKET          pkt;
    TCPIP_SNTP_TIME_STAMP msStamp;
    uint16_t            w;


    // Get the response time packet
    w = TCPIP_UDP_ArrayGet(sntpSocket, (uint8_t*) &pkt, sizeof(pkt));

    // sanity packet check
    if(w != sizeof(pkt) || pkt.flags.versionNumber != TCPIP_NTP_VERSION )
    {
        TCPIP_SNTP_SetError(SNTP_RES_NTP_VERSION_ERR, TCPIP_SNTP_EVENT_VER_ERROR); 
        return false;
    }
    if((pkt.tx_ts_secs == 0 && pkt.tx_ts_fraq == 0))
    {
        TCPIP_SNTP_SetError(SNTP_RES_NTP_TSTAMP_ERR, TCPIP_SNTP_EVENT_TSTAMP_ERROR); 
        return false;
    }
    if(pkt.stratum == 0 )
    {
        TCPIP_SNTP_SetError(SNTP_RES_NTP_KOD_ERR, TCPIP_SNTP_EVENT_KOD_ERROR); 
        return false;
    }
    if(pkt.stratum >= TCPIP_NTP_MAX_STRATUM || pkt.flags.leapIndicator == 3 )
    {
        TCPIP_SNTP_SetError(SNTP_RES_NTP_SYNC_ERR, TCPIP_SNTP_EVENT_SYNC_ERROR); 
        return false;
    }

    // success
    // get the last timestamp
    ntpData.tStamp.tStampSeconds = TCPIP_Helper_ntohl(pkt.tx_ts_secs);
    ntpData.tStamp.tStampFraction = TCPIP_Helper_ntohl(pkt.tx_ts_fraq);
    ntpData.tStampTick = SYS_TMR_TickCountGetLong();
    ntpData.tUnixSeconds = ntpData.tStamp.tStampSeconds - TCPIP_NTP_EPOCH;
    msStamp.llStamp = (uint64_t)ntpData.tStamp.tStampFraction * 1000;
    ntpData.tMilliseconds = msStamp.tStampSeconds;
    ntpData.nUpdates++;

    TCPIP_SNTP_Event(TCPIP_SNTP_EVENT_TSTAMP_OK, (const void*)&ntpData);

    _SNTP_DbgNewTimeStamp(ntpData.tUnixSeconds);
    return true;
}

// tStamp should be valid here!
// we calculate the tick difference time in NTP format
// and add to original NTP timestamp to get the current one
// returns the current second and millisecond
static uint32_t TCPIP_SNTP_CurrTime(uint32_t* pMs)
{

    TCPIP_SNTP_TIME_STAMP deltaStamp, fractStamp;
    
    uint32_t ticksPerSec = SYS_TMR_TickCounterFrequencyGet();
    uint64_t deltaTick = SYS_TMR_TickCountGetLong() - ntpData.tStampTick;
    
    // calculate seconds = deltaTick / ticksPerSec;
    deltaStamp.tStampSeconds = (uint32_t)(deltaTick / ticksPerSec);

    // calculate fract part: (deltaTick % ticksPerSec) / ticksPerSec) * 2^32 ; 
    fractStamp.tStampSeconds = deltaTick - deltaStamp.tStampSeconds * ticksPerSec;
    fractStamp.tStampFraction = 0;

    deltaStamp.tStampFraction = (uint32_t)(fractStamp.llStamp / ticksPerSec);

    // 64 bit addition gets us the new time stamp
    deltaStamp.llStamp += ntpData.tStamp.llStamp;


    // calculate milliseconds: (fract / 2 ^ 32) * 1000;
    if(pMs)
    {
        fractStamp.llStamp = (uint64_t)deltaStamp.tStampFraction * 1000;
        *pMs = fractStamp.tStampSeconds;
    }

    return deltaStamp.tStampSeconds - TCPIP_NTP_EPOCH;
}

uint32_t TCPIP_SNTP_UTCSecondsGet(void)
{

    uint32_t currSec;
    TCPIP_SNTP_RESULT res = TCPIP_SNTP_TimeStampStatus();

    if(res == SNTP_RES_TSTAMP_ERROR)
    {
        currSec = 0;
    }
    else
    {   // we could get the time
        currSec = TCPIP_SNTP_CurrTime(0);
    }


    return currSec;
}

TCPIP_SNTP_RESULT TCPIP_SNTP_ConnectionParamSet(TCPIP_NET_HANDLE netH, IP_ADDRESS_TYPE ntpConnType, const char* ntpServer)
{
    if(!_SntpCanChangeParams())
    {
        return SNTP_RES_BUSY;
    }

    if(ntpConnType != IP_ADDRESS_TYPE_ANY)
    {
        if(ntpConnType != IP_ADDRESS_TYPE_IPV4 && ntpConnType != IP_ADDRESS_TYPE_IPV6)
        {   // unknown
            return SNTP_RES_NTP_CONN_ERR;
        }

#if !defined (TCPIP_STACK_USE_IPV6)
        if(ntpConnType == IP_ADDRESS_TYPE_IPV6)
        {   // not supported
            return SNTP_RES_NTP_CONN_ERR;
        }
#endif  // !defined (TCPIP_STACK_USE_IPV6)

#if !defined (TCPIP_STACK_USE_IPV4)
        if(ntpConnType == IP_ADDRESS_TYPE_IPV4)
        {   // not supported
            return SNTP_RES_NTP_CONN_ERR;
        }
#endif  // !defined (TCPIP_STACK_USE_IPV4)

        ntpConnection = ntpConnType;
    }

    if(netH)
    {
        pSntpDefIf = _TCPIPStackHandleToNet(netH);
    }

    if(ntpServer)
    {
        strncpy(sntpServerName, ntpServer, sizeof(sntpServerName) - 1);
        sntpServerName[sizeof(sntpServerName) - 1] = 0;
    }

    return SNTP_RES_OK;
}


TCPIP_SNTP_RESULT TCPIP_SNTP_ConnectionInitiate(void)
{
    if(!_SntpCanStart())
    {
        return SNTP_RES_PROGRESS;
    }

    TCPIP_SNTP_SetNewState(SM_HOME);
    return SNTP_RES_OK;
}

TCPIP_SNTP_RESULT TCPIP_SNTP_TimeStampStatus(void)
{
    TCPIP_SNTP_RESULT res;

    if(ntpData.nUpdates == 0)
    {   // no data available
        res = SNTP_RES_TSTAMP_ERROR;
    }
    else if(SYS_TMR_TickCountGetLong() - ntpData.tStampTick >  sntp_tstamp_timeout)
    {
        res = SNTP_RES_TSTAMP_STALE;
    }
    else
    {
        res = SNTP_RES_OK;
    }

    return res;
}


TCPIP_SNTP_RESULT TCPIP_SNTP_TimeStampGet(TCPIP_SNTP_TIME_STAMP* pTStamp, uint32_t* pLastUpdate)
{
    TCPIP_SNTP_RESULT res = TCPIP_SNTP_TimeStampStatus();

    if(pTStamp)
    {
        pTStamp->llStamp = res == SNTP_RES_TSTAMP_ERROR ? 0ull : ntpData.tStamp.llStamp;
    }
    if(pLastUpdate)
    {
        *pLastUpdate = res == SNTP_RES_TSTAMP_ERROR ? 0 : (uint32_t)ntpData.tStampTick;
    }

    return res;
}

TCPIP_SNTP_RESULT TCPIP_SNTP_TimeGet(uint32_t* pUTCSeconds, uint32_t* pMs)
{
    uint32_t currSec, currMs;
    TCPIP_SNTP_RESULT res = TCPIP_SNTP_TimeStampStatus();

    if(res == SNTP_RES_TSTAMP_ERROR)
    {
        currSec = currMs = 0;
    }
    else
    {   // we could get the time
        currSec = TCPIP_SNTP_CurrTime(&currMs);
    }

    if(pUTCSeconds)
    {
        *pUTCSeconds = currSec;
    }

    if(pMs)
    {
        *pMs = currMs;
    }

    return res;
}

TCPIP_SNTP_RESULT TCPIP_SNTP_LastErrorGet(void)
{
    // keep compiler happy
    _SNTPAssertCond(true, 0, 0);
    _SNTPDbgCond(true, 0, 0);

    TCPIP_SNTP_RESULT res = ntpLastError;
    ntpLastError = SNTP_RES_OK;
    return res; 
}

TCPIP_SNTP_HANDLE TCPIP_SNTP_HandlerRegister(TCPIP_SNTP_EVENT_HANDLER handler)
{
    if(handler == 0 || ntpEventHandler != 0)
    {
        return 0;
    }

    ntpEventHandler = handler;
    return ntpEventHandler;
}

bool TCPIP_SNTP_HandlerDeRegister(TCPIP_SNTP_HANDLE hSntp)
{
    if(hSntp != ntpEventHandler)
    {
        return false;
    }

    ntpEventHandler = 0;
    return true;
}
    
// signals an SNTP event
static void TCPIP_SNTP_Event(TCPIP_SNTP_EVENT evType, const void* param)
{
    if(ntpEventHandler != 0)
    {
        (*ntpEventHandler)(evType, param);
    }
}

#endif  //if defined(TCPIP_STACK_USE_SNTP_CLIENT)
