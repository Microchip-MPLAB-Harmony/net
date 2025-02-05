/*******************************************************************************
  Dynamic Host Configuration Protocol (DHCP) Client

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    - Provides automatic IP address, subnet mask, gateway address, 
      DNS server address, and other configuration parameters on DHCP 
      enabled networks.
    - Reference: RFC 2131, 2132
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
#include "tcpip/src/tcpip_private.h"

#include "tcpip/src/dhcp_private.h"
#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_DHCP_CLIENT

// Unique variables per interface
typedef struct
{
    uint32_t                waitTicks;      // # of ticks to wait for the current DHCP operation
    uint32_t                startWait;      // the time the wait is starting; could be seconds or ticks,
                                            // operation dependent                
                                            // wait operations are implemented as difference between the 
                                            // current tick/second and startWait to avoid counter overflows
    uint32_t                tRequest;       // the time when the DHCP request was sent - seconds
    uint32_t                tRenewRequest;  // the time when the DHCP renew request was sent - seconds
    uint32_t                t1Seconds;      // # of seconds to wait until we get to RENEW state (T1 timer)
    uint32_t                t2Seconds;      // # of seconds to wait until we get to REBIND state (T2 timer)
    uint32_t                tExpSeconds;    // # of seconds to wait until the lease completely expired
                                            // (lease Time, Texp)    
                                            // T1 < T2 < Texp
    uint32_t                t3Seconds;      // # of seconds to wait until reissuing a REQUEST in RENEW/REBIND state
    uint32_t                tOpStart;       // time at which a lease operation is started
    IPV4_ADDR               dwServerID;     // DHCP Server ID cache
    IPV4_ADDR               lastSrvAddress; // DHCP Server that sent the last message
    IPV4_ADDR               leaseSrvAddress;// DHCP Server that we try to bound to; to avoid using another server that interfered while we're doing ARP...
    IPV4_ADDR               boundSrvAddress;// DHCP Server that we're bound to (grant the lease)
    IPV4_ADDR               dhcpIPAddress;  // DHCP obtained IP address
    IPV4_ADDR               dhcpGateway;    // DHCP obtained gateway
    IPV4_ADDR               dhcpMask;       // DHCP obtained mask
    #if defined(TCPIP_STACK_USE_DNS)
    IPV4_ADDR               dhcpDNS;        // DHCP obtained primary DNS server
    IPV4_ADDR               dhcpDNS2;       // DHCP obtained secondary DNS server
    #endif  
#if (TCPIP_DHCP_USE_OPTION_TIME_SERVER != 0)
    IPV4_ADDR               timeServers[TCPIP_DHCP_TIME_SERVER_ADDRESSES];  // DHCP obtained time servers
#endif  // (TCPIP_DHCP_USE_OPTION_TIME_SERVER != 0)
#if (TCPIP_DHCP_USE_OPTION_NTP_SERVER != 0)
    IPV4_ADDR               ntpServers[TCPIP_DHCP_NTP_SERVER_ADDRESSES];    // DHCP obtained time servers
#endif  // (TCPIP_DHCP_USE_OPTION_NTP_SERVER != 0)
    uint32_t                transactionID;  // current transaction ID
    TCPIP_DHCP_HOST_NAME_CALLBACK   nameCallback;   // get host name callback
    IPV4_FILTER_HANDLE      dhcpFilterHandle;   // IPv4 packet filter    
    // unaligned data
    uint16_t                dhcpTmo;        // current DHCP timeout        
    uint16_t                dhcpTmoBase;    // DHCP base timeout
    uint16_t                tOpFailTmo;     // operation failure timeout: initialization, etc.
    uint16_t                tLeaseCheck;    // time to wait for a lease check
    uint16_t                dhcpOp;         // DHCP current operation: TCPIP_DHCP_OPERATION_TYPE
    uint16_t                smState;        // DHCP client state machine variable: TCPIP_DHCP_STATUS
    union
    {
        struct
        {
            unsigned int bDHCPEnabled        : 1;    // Whether or not DHCP is currently enabled
            unsigned int bIsBound            : 1;    // Whether or not DHCP is currently bound
            unsigned int bOfferReceived      : 1;    // Whether or not an offer has been received
            unsigned int bDHCPServerDetected : 1;    // Indicates if a DCHP server has been detected
            unsigned int bWasBound           : 1;    // successfully held a lease
            unsigned int bReportFail         : 1;    // report run time failure flag
            unsigned int bWriteBack          : 1;    // write back the resulting host name
            unsigned int bRetry              : 1;    // a new cycle/retry because of a failure
        };
        uint8_t val;
    } flags;
    // Indicates which DHCP values are currently valid
    union
    {
        struct
        {
            unsigned int IPAddress      : 1;     // Leased IP address is valid
            unsigned int Gateway        : 1;     // Gateway address is valid
            unsigned int Mask           : 1;     // Subnet mask is valid
            unsigned int DNS            : 1;     // Primary DNS is valid
            unsigned int DNS2           : 1;     // Secondary DNS is valid
            unsigned int tServer        : 1;     // Time Server addresses are valid
            unsigned int ntpServer      : 1;     // Time Server addresses are valid
            unsigned int reserved       : 1;     // unused
        };
        uint8_t val;
    } validValues;
#if (TCPIP_DHCP_USE_OPTION_TIME_SERVER != 0)
    uint8_t     tServerNo;          // number of stored valid time servers in timeServers 
#endif  // (TCPIP_DHCP_USE_OPTION_TIME_SERVER != 0)
#if (TCPIP_DHCP_USE_OPTION_NTP_SERVER != 0)
    uint8_t     ntpServerNo;          // number of stored valid time servers in ntpServers 
#endif  // (TCPIP_DHCP_USE_OPTION_NTP_SERVER != 0)

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_STATUS) != 0
    uint8_t                 prevState;      // DHCP client previous state machine variable: TCPIP_DHCP_STATUS
#endif  // TCPIP_DHCP_DEBUG_MASK

#if defined TCPIP_DHCP_STORE_BOOT_FILE_NAME
    uint8_t     bootFileName[TCPIP_DHCP_BOOT_FILE_NAME_SIZE];
#endif // defined TCPIP_DHCP_STORE_BOOT_FILE_NAME

} DHCP_CLIENT_VARS;

static void     TCPIP_DHCP_Process(bool isTmo);

static void     F_DHCPSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param);
static unsigned int  F_DHCPReceive(DHCP_CLIENT_VARS* pClient, const TCPIP_NET_IF* pNetIf);
static bool     F_DHCPSend(DHCP_CLIENT_VARS* pClient, const TCPIP_NET_IF* pNetIf, unsigned int messageType, uint32_t dhcpFlags);
static void     F_DHCPNotifyClients(const TCPIP_NET_IF* pNetIf, TCPIP_DHCP_EVENT_TYPE evType, uint32_t clientAdd, uint32_t serverAdd);
static bool     F_DHCPStartOperation(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPERATION_REQ opReq, uint32_t reqAddress);
static void     F_DHCPSetEventInfo(const TCPIP_NET_IF* pNetIf, TCPIP_DHCP_EVENT_TYPE evType, uint32_t clientAdd, uint32_t serverAdd, TCPIP_DHCP_EVENT_INFO* pEvInfo);

static bool     F_DHCPPacketFilter(TCPIP_MAC_PACKET* pRxPkt, uint8_t hdrlen);
static UDP_SOCKET F_DHCPOpenSocket(void);

static void     F_DHCPSetRunFail(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_STATUS newState, bool expBackoff);
static void     F_DHCPSetTimeout(DHCP_CLIENT_VARS* pClient);
static void     F_DHCPSetLeaseTimeParams(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pDhcpData);
static void     F_DHCPSetNewLease(DHCP_CLIENT_VARS* pClient, const TCPIP_NET_IF* cNetIf);
static unsigned int     F_DHCPProcessReceiveData(DHCP_CLIENT_VARS* pClient, const TCPIP_NET_IF* pNetIf);
static void     F_DHCPCheckRunFailEvent(const TCPIP_NET_IF* pNetIf, DHCP_CLIENT_VARS* pClient);
static void     F_DHCPSetBoundState(DHCP_CLIENT_VARS* pClient);

static TCPIP_DHCP_OPTION_RESULT F_DHCPOptionProcess(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);
static int                      F_DHCPOptionProcessMsgType(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);
static int                      F_DHCPOptionProcessSubNet(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);
static int                      F_DHCPOptionProcessRouter(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);
static int                      F_DHCPOptionProcessDns(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);
static int                      F_DHCPOptionProcessSrvIdent(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);
static int                      F_DHCPOptionProcessLease(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);
static int                      F_DHCPOptionProcessRenewT(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);
static int                      F_DHCPOptionProcessRebindT(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);
#if (TCPIP_DHCP_USE_OPTION_TIME_SERVER != 0)
static int                      F_DHCPOptionProcessTimeServer(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);
#endif  // (TCPIP_DHCP_USE_OPTION_TIME_SERVER != 0)
#if (TCPIP_DHCP_USE_OPTION_NTP_SERVER != 0)
static int                      F_DHCPOptionProcessNtpServer(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);
#endif  // (TCPIP_DHCP_USE_OPTION_NTP_SERVER != 0)

static int                      F_DHCPOptionProcessEnd(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);
static unsigned int             F_DHCPOptionGetMsgType(TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);


static TCPIP_DHCP_OPTION_RESULT F_DHCPOptionsWrite(const TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData);
static int                      F_DHCPOptionWriteMsgType(const TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData);
static int                      F_DHCPOptionWriteSrvIdent(const TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData);
static int                      F_DHCPOptionWriteParamRequest(const TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData);
static int                      F_DHCPOptionWriteIPRequest(const TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData);
static int                      F_DHCPOptionHostName(const TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData);
static int                      F_DHCPOptionClientId(const TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData);
static int                      F_DHCPOptionWriteEnd(const TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData);

static size_t                   F_DHCPFormatHostName(unsigned char* destBuffer, unsigned const char* srcBuffer, size_t destSize);

static unsigned int             F_DHCPCheckMsgType(TCPIP_NET_IF* pNetIf);
static TCPIP_DHCP_EVENT_TYPE    F_DHCPRxMsgType2Event(unsigned int msgType);
static TCPIP_DHCP_EV_SOURCE     F_DHCPEventSource(TCPIP_DHCP_EVENT_TYPE evType);
static void                     F_DHCPRxGetInfo(void);

// returns true if the client event requires broadcast address
static __inline__ bool __attribute__((always_inline)) F_DHCPEventUseBcast(TCPIP_DHCP_EVENT_TYPE evType)
{
    return (evType == DHCP_EVENT_DISCOVER || evType == DHCP_EVENT_REQUEST || evType == DHCP_EVENT_DECLINE || evType == DHCP_EVENT_REQUEST_REBIND);
}


#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAKE_TMO) != 0
static void     F_DHCPDbgProcessTmo(DHCP_CLIENT_VARS* pClient);
#endif

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_LEASE_DECLINE) != 0
static bool     F_DHCPDbgProcessAckDecline(DHCP_CLIENT_VARS* pClient, const TCPIP_NET_IF* pNetIf);
#endif  // TCPIP_DHCP_DEBUG_MASK

static DHCP_CLIENT_VARS*    DHCPClients = NULL;    // DHCP client per interface 

static size_t               dhcpInterfaces = 0U; // number of interfaces present
static size_t               dhcpInitCount = 0;  // DHCP module initialization count

static UDP_SOCKET           dhcpClientSocket;   // Handle to DHCP client socket
static UDP_PORT             dhcpClientPort;
static UDP_PORT             dhcpServerPort;

static TCPIP_SIGNAL_HANDLE     dhcpSignalHandle = NULL;

static const uint8_t    dhcpMagicCookie[4] = {0x63, 0x82, 0x53, 0x63};      // DHCP cookie, network order
// DHCP event registration

static const void*      dhcpMemH = NULL;        // memory handle

static PROTECTED_SINGLE_LIST      dhcpRegisteredUsers = { {0} };


#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAKE_TMO) != 0
// these values override the parameters received from the server 
static volatile uint16_t    dhcpDbgBaseTmo =  2;    // DHCP base timeout; >= TCPIP_DHCP_EXP_BACKOFF_FUZZ + 1 ! 

static volatile uint16_t    dhcpDbgInitTmo =  8;    // DHCP initialization timeout

static volatile uint32_t    dhcpDbgGetOfferTmo = 0; // # of times to fake timeout for GET_OFFER
static volatile uint32_t    dhcpDbgGetOfferTmoCnt;  // run time counter

static volatile uint32_t    dhcpDbgGetRequestTmo = 0; //  # of times to fake timeout for GET_REQUEST_ACK
static volatile uint32_t    dhcpDbgGetRequestTmoCnt;  // run time counter

static volatile uint32_t    dhcpDbgGetRenewTmo = 0; //  # of times to fake timeout for GET_RENEW_ACK
static volatile uint32_t    dhcpDbgGetRenewTmoCnt;  // run time counter

static volatile uint32_t    dhcpDbgGetRebindTmo = 0; //  # of times to fake timeout for GET_RBIND_ACK
static volatile uint32_t    dhcpDbgGetRebindTmoCnt;  // run time counter
#endif  // TCPIP_DHCP_DEBUG_MASK

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_LEASE_DECLINE) != 0
static volatile uint32_t    dhcpDbgLeaseRequestDecline = 0; //  # of times to decline a lease for GET_REQUEST_ACK
static volatile uint32_t    dhcpDbgLeaseRequestDeclineCnt;  //  run time counter

static volatile uint32_t    dhcpDbgLeaseRenewDecline = 0; //  # of times to decline a lease for GET_RENEW_ACK
static volatile uint32_t    dhcpDbgLeaseRenewDeclineCnt;  //  run time counter

static volatile uint32_t    dhcpDbgLeaseRebindDecline = 0; //  # of times to decline a lease for GET_REBIND_ACK
static volatile uint32_t    dhcpDbgLeaseRebindDeclineCnt;  //  run time counter

#endif  // TCPIP_DHCP_DEBUG_MASK

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAIL_ARP) != 0
static volatile int        dhcpDbgFailArpCheck = 0;      // if !0, the ARP verification will fail
static volatile int        dhcpDbgFailArpCheckCnt;      // run time counter

static char*               dhcpDbgFakeArpAddressStr = NULL; // "192.168.1.109";   // if !0, use this address for ARP check!
static IPV4_ADDR           dhcpDbgFakeArpAddress = {0U};    // run time value
#endif  // TCPIP_DHCP_DEBUG_MASK

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAKE_HOST_NAME) != 0
static unsigned char*       dhcpDbgHostName = NULL; // if !0, use this name as DHCP host name!
#endif  // TCPIP_DHCP_DEBUG_MASK

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAKE_LEASE_TIME) != 0
static volatile uint32_t    dhcpDbgLeaseTime = 0U;        // if !0, lease expiration time override
#endif  // TCPIP_DHCP_DEBUG_MASK


// receive dispatch function, processing functions
// returns > 0 the size of the option processed
// returns 0 if end found
// returns < 0 for error
typedef int (*DHCPDispatchFnc)(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);

typedef struct
{
    unsigned int    option;         // supported/processed option
    DHCPDispatchFnc dhcpProcFnc;    // processing function
}TCPIP_DHCP_OPTION_PROC_ENTRY;

static const TCPIP_DHCP_OPTION_PROC_ENTRY   t_DHCPOptProcTbl[] = 
{
    // option                           // process function 
    {  TCPIP_DHCP_MESSAGE_TYPE,         &F_DHCPOptionProcessMsgType },
    {  TCPIP_DHCP_SUBNET_MASK,          &F_DHCPOptionProcessSubNet },
    {  TCPIP_DHCP_ROUTER,               &F_DHCPOptionProcessRouter },
    {  TCPIP_DHCP_DNS,                  &F_DHCPOptionProcessDns },
    {  TCPIP_DHCP_SERVER_IDENTIFIER,    &F_DHCPOptionProcessSrvIdent },
    {  TCPIP_DHCP_IP_LEASE_TIME,        &F_DHCPOptionProcessLease },
    {  TCPIP_DHCP_RENEW_TIME,           &F_DHCPOptionProcessRenewT },
    {  TCPIP_DHCP_REBIND_TIME,          &F_DHCPOptionProcessRebindT },
#if (TCPIP_DHCP_USE_OPTION_TIME_SERVER != 0)
    {  TCPIP_DHCP_TIME_SERVER,          &F_DHCPOptionProcessTimeServer },
#endif  // (TCPIP_DHCP_USE_OPTION_TIME_SERVER != 0)
#if (TCPIP_DHCP_USE_OPTION_NTP_SERVER != 0)
    {  TCPIP_DHCP_NTP_SERVER,           &F_DHCPOptionProcessNtpServer },
#endif  // (TCPIP_DHCP_USE_OPTION_NTP_SERVER != 0)
    {  TCPIP_DHCP_END_OPTION,           &F_DHCPOptionProcessEnd },

};


// send dispatch function, write options functions
// returns >= 0 the size of the option added
// returns < 0 for failure
typedef int (*DHCPWriteOptFnc)(const TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData);

typedef struct
{
    // unsigned int        option;         // implicit write option
    DHCPWriteOptFnc    dhcpWriteFnc;   // option write function
}TCPIP_DHCP_OPTION_WRITE_ENTRY;

static const TCPIP_DHCP_OPTION_WRITE_ENTRY   t_DHCPOptWriteTbl[] = 
{
    // Note: this table entries are processed in sequrntial order!
    // option                                     // write option function 
    {  /* TCPIP_DHCP_MESSAGE_TYPE */             &F_DHCPOptionWriteMsgType },  // always the 1st!
    {  /* TCPIP_DHCP_SERVER_IDENTIFIER */        &F_DHCPOptionWriteSrvIdent },
    {  /* TCPIP_DHCP_PARAM_REQUEST_LIST */       &F_DHCPOptionWriteParamRequest },
    {  /* TCPIP_DHCP_PARAM_REQUEST_IP_ADDRESS */ &F_DHCPOptionWriteIPRequest },
    {  /* TCPIP_DHCP_HOST_NAME */                &F_DHCPOptionHostName },
    {  /* TCPIP_DHCP_PARAM_REQUEST_CLIENT_ID */  &F_DHCPOptionClientId },
    {  /* TCPIP_DHCP_END_OPTION */               &F_DHCPOptionWriteEnd },      // always the last!
};

// the list of the requested parameters in a DHCP message to the server
// each requested parameter/option code takes one byte
// Add more parameters here if needed but always < 256 !
static const uint8_t t_DHCPRequestParamsTbl[] = 
{
    TCPIP_DHCP_SUBNET_MASK,         // request a subnet mask
    TCPIP_DHCP_ROUTER,              // request for a router
    TCPIP_DHCP_DNS,                 // request for DNS
#if (TCPIP_DHCP_USE_OPTION_TIME_SERVER != 0)
    TCPIP_DHCP_TIME_SERVER,         // request for Time Servers 
#endif  // (TCPIP_DHCP_USE_OPTION_TIME_SERVER != 0)
#if (TCPIP_DHCP_USE_OPTION_NTP_SERVER != 0)
    TCPIP_DHCP_NTP_SERVER,         // request for NTP Servers 
#endif  // (TCPIP_DHCP_USE_OPTION_NTP_SERVER != 0)
    // Add more parameters here if needed
};

static __inline__ void __attribute__((always_inline)) F_DHCPSetFailTimeout(DHCP_CLIENT_VARS* pClient, bool resetTmo, bool isRunTime)
{
    pClient->flags.bReportFail = 1U;
    if(resetTmo || pClient->tOpStart == 0U)
    {
        pClient->tOpStart = TCPIP_SecCountGet();
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAIL_TMO_EVENT) != 0
        SYS_CONSOLE_PRINT("DHCP cli: %d, set fail tmo: %d, xid: 0x%8x, at: %s, reset: %s\r\n", pClient - DHCPClients, pClient->tOpStart, TCPIP_Helper_htonl(pClient->transactionID), isRunTime ? "run" : "init", resetTmo ? "y" : "n");
#endif  // TCPIP_DHCP_DEBUG_MASK
    }
}

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_STATUS) != 0
static const char* t_DHCPStatTbl[] = 
{
    "idle ",         // TCPIP_DHCP_IDLE 
    "wlink",         // TCPIP_DHCP_WAIT_LINK,         
    "discs",         // TCPIP_DHCP_SEND_DISCOVERY,     
    "offrw",         // TCPIP_DHCP_GET_OFFER,          
    "requs",         // TCPIP_DHCP_SEND_REQUEST,     
    "requw",         // TCPIP_DHCP_GET_REQUEST_ACK, 
    "lchek",         // TCPIP_DHCP_WAIT_LEASE_CHECK,
    "lrtry",         // TCPIP_DHCP_WAIT_LEASE_RETRY,
    "skip",          // TCPIP_DHCP_SKIP_LEASE_CHECK,
    "bound",         // TCPIP_DHCP_BOUND,              
    "renes",         // TCPIP_DHCP_SEND_RENEW,  
    "renew",         // TCPIP_DHCP_GET_RENEW_ACK,      
    "rbnds",         // TCPIP_DHCP_SEND_REBIND,
    "rbndw",         // TCPIP_DHCP_GET_REBIND_ACK,     
};

static void F_DHCPDbgStatus(DHCP_CLIENT_VARS* pClient)
{
    if(pClient->smState != (uint16_t)pClient->prevState)
    {
        SYS_CONSOLE_PRINT("DHCP cli: %d, state: %s, xid: 0x%8x, msec: %d\r\n", pClient - DHCPClients, t_DHCPStatTbl[pClient->smState], TCPIP_Helper_htonl(pClient->transactionID), TCPIP_MsecCountGet());
        pClient->prevState = (uint8_t)pClient->smState;
    }
}
#else
#define F_DHCPDbgStatus(pClient)
#endif  // TCPIP_DHCP_DEBUG_MASK

static __inline__ void __attribute__((always_inline)) F_DHCPClientStateSet(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_STATUS newState)
{
    pClient->smState = (uint16_t)newState;
    F_DHCPDbgStatus(pClient);
}

// conversion functions/helpers
static __inline__ TCPIP_DHCP_LIST_EV_NODE* __attribute__((always_inline)) FC_SLNode2DhcpNode(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE* node;
        TCPIP_DHCP_LIST_EV_NODE* dhcpNode;
    }U_SL_NODE_DHCP;

    U_SL_NODE_DHCP.node = node;
    return U_SL_NODE_DHCP.dhcpNode;
}

static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_DhcpH2SLNode(TCPIP_DHCP_HANDLE hDhcp)
{
    union
    {
        TCPIP_DHCP_HANDLE   hDhcp;
        SGL_LIST_NODE*      node;
    }U_DHCPH_SL_NODE;

    U_DHCPH_SL_NODE.hDhcp = hDhcp;
    return U_DHCPH_SL_NODE.node;
}

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_ADDRESS_EVENTS) != 0
static void F_DHCPDbgAddServiceEvent(DHCP_CLIENT_VARS* pClient, TCPIP_ADDR_SRVC_EV addEv, const char* extraMsg)
{
    const char* evMsg;

    switch(addEv)
    {
        case TCPIP_ADDR_SRVC_EV_NONE:
            evMsg = "none";
            break;

        case TCPIP_ADDR_SRVC_EV_RUN_FAIL:
            evMsg = "runfail";
            break;

        case TCPIP_ADDR_SRVC_EV_CONN_LOST:
            evMsg = "connlost";
            break;

        case TCPIP_ADDR_SRVC_EV_USER_STOP:
            evMsg = "uerstop";
            break;

        case TCPIP_ADDR_SRVC_EV_RUN_RESTORE:
            evMsg = "restore";
            break;

        default:
            evMsg = "unknown";
            break;
    }

    SYS_CONSOLE_PRINT("DHCP cli: %d, address event: %s, extra: %s, msec: %d\r\n", pClient - DHCPClients, evMsg, extraMsg != NULL ? extraMsg : "none", TCPIP_MsecCountGet());
}

#else
#define F_DHCPDbgAddServiceEvent(pClient, ev, extra)

#endif  // TCPIP_DHCP_DEBUG_MASK

#if (TCPIP_DHCP_DEBUG_MASK & (TCPIP_DHCP_DEBUG_MASK_RX_RES_ENABLE | TCPIP_DHCP_DEBUG_MASK_TX_MSG_ENABLE)) != 0
static const char* t_DHCPMsgTypeTbl[] = 
{
    "unkn",         // TCPIP_DHCP_UNKNOWN_MESSAGE
    "disc",         // TCPIP_DHCP_DISCOVER_MESSAGE
    "offer",        // TCPIP_DHCP_OFFER_MESSAGE
    "req",          // TCPIP_DHCP_REQUEST_MESSAGE
    "decli",        // TCPIP_DHCP_DECLINE_MESSAGE
    "ack",          // TCPIP_DHCP_ACK_MESSAGE
    "nak",          // TCPIP_DHCP_NAK_MESSAGE
    "relea",        // TCPIP_DHCP_RELEASE_MESSAGE
    "info",         // TCPIP_DHCP_INFORM_MESSAGE
};
#endif //  (TCPIP_DHCP_DEBUG_MASK & (TCPIP_DHCP_DEBUG_MASK_RX_RES_ENABLE | TCPIP_DHCP_DEBUG_MASK_TX_MSG_ENABLE)) != 0


#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_RX_RES_ENABLE) != 0
static void F_DHCPDbgRxDisplay(const TCPIP_NET_IF* pNetIf, unsigned int msgType, uint32_t rxErrCode)
{
    if(rxErrCode != 0)
    {
        if(msgType >= sizeof(t_DHCPMsgTypeTbl) / sizeof(*t_DHCPMsgTypeTbl))
        {
            msgType = 0;    // this is unknown
        }

        const char* dhcpMsg = t_DHCPMsgTypeTbl[msgType];
        DHCP_CLIENT_VARS* pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf);

        SYS_CONSOLE_PRINT("DHCP cli Rx error - if: %d, Rx: %s, xid: 0x%8x, error: %d, msec: %d\r\n", TCPIP_STACK_NetIxGet(pNetIf), dhcpMsg, TCPIP_Helper_htonl(pClient->transactionID), rxErrCode, TCPIP_MsecCountGet());
    }
}

#else
static __inline__ void __attribute__((always_inline)) F_DHCPDbgRxDisplay(const TCPIP_NET_IF* pNetIf, unsigned int msgType, uint32_t rxErrCode)
{
}
#endif  // TCPIP_DHCP_DEBUG_MASK

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TX_MSG_ENABLE) != 0
static void F_DHCPDebugTxDisplay(DHCP_CLIENT_VARS* pClient, unsigned int msgType, bool success)
{
    if(msgType >= sizeof(t_DHCPMsgTypeTbl) / sizeof(*t_DHCPMsgTypeTbl))
    {
        msgType = 0;    // this is unknown
    }

    const char* dhcpMsg = t_DHCPMsgTypeTbl[msgType];

    SYS_CONSOLE_PRINT("DHCP cli: %d, Tx: %s %s, xid: 0x%8x, msec: %d\r\n", pClient - DHCPClients, dhcpMsg, success ? "Sent" : "Failed", TCPIP_Helper_htonl(pClient->transactionID), TCPIP_MsecCountGet());
}
#else
#define F_DHCPDebugTxDisplay(pClient, msgType, success)
#endif  // (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TX_MSG_ENABLE) != 0


static __inline__ void __attribute__((always_inline)) F_DHCPSetIPv4Filter(DHCP_CLIENT_VARS* pClient, bool active)
{
    if(pClient->dhcpFilterHandle != NULL)
    {
        (void)Ipv4FilterSetActive(pClient->dhcpFilterHandle, active);
    }
}

static void F_DHCPClientClose(TCPIP_NET_IF* pNetIf, bool disable, bool release)
{
    DHCP_CLIENT_VARS* pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf);
    
    if(pClient->flags.bDHCPEnabled != 0U)
    {
        if(release && pClient->smState >= (uint16_t)TCPIP_DHCP_BOUND)
        {  
            (void)F_DHCPSend(pClient, pNetIf, TCPIP_DHCP_RELEASE_MESSAGE, 0U);
        }

        pClient->flags.bIsBound = 0U;
        pClient->flags.bDHCPServerDetected = 0U;
        pClient->flags.bReportFail = 1U; 
        pClient->tOpStart = 0; 
        pClient->dhcpTmo = pClient->dhcpTmoBase;
        F_DHCPSetIPv4Filter(pClient, false);
        
        if(disable)
        {
            pClient->flags.bWasBound = 0U;   
            pClient->flags.bDHCPEnabled = 0U;
            pNetIf->Flags.bIsDHCPEnabled = 0U;
        }
        else
        {   // let it active
            F_DHCPClientStateSet(pClient, TCPIP_DHCP_IDLE);
        }
    }
}

static void F_DHCPEnable(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPERATION_TYPE opType)
{
    DHCP_CLIENT_VARS* pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf);
    
    if(TCPIP_STACK_NetworkIsLinked(pNetIf) || !TCPIPStackNetIsPrimary(pNetIf))
    {   // the idea is that if the link is down we'll get an connection up event anyway
        // to wake us up from TCPIP_DHCP_IDLE
        // aliased interfaces don't get an event!
        F_DHCPClientStateSet(pClient, TCPIP_DHCP_WAIT_LINK);
    }
    else
    {
        F_DHCPClientStateSet(pClient, TCPIP_DHCP_IDLE);
    }
    
    pClient->flags.bDHCPEnabled = 1U;
    pClient->dhcpOp = (uint16_t)opType;
    pNetIf->Flags.bIsDHCPEnabled = 1U;


    if(pClient->dhcpFilterHandle == NULL)
    {
        pClient->dhcpFilterHandle = IPv4RegisterFilter(&F_DHCPPacketFilter, true);
        if(pClient->dhcpFilterHandle == NULL)
        {
            SYS_ERROR(SYS_ERROR_WARNING, "DHCP: Failed to register IPv4 filter! \r\n");
        }
    }

    if(opType == TCPIP_DHCP_OPER_INIT || opType == TCPIP_DHCP_OPER_INIT_REBOOT)
    {   // reinit the time base
        pClient->dhcpTmo = pClient->dhcpTmoBase;
        F_DHCPSetFailTimeout(pClient, true, false);

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAKE_TMO) != 0
        // re-init the debug counters
        dhcpDbgGetOfferTmoCnt = dhcpDbgGetOfferTmo;
        dhcpDbgGetRequestTmoCnt = dhcpDbgGetRequestTmo;
        dhcpDbgGetRenewTmoCnt = dhcpDbgGetRenewTmo;
        dhcpDbgGetRebindTmoCnt = dhcpDbgGetRebindTmo;
#endif  // TCPIP_DHCP_DEBUG_MASK

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_LEASE_DECLINE) != 0
        dhcpDbgLeaseRequestDeclineCnt = dhcpDbgLeaseRequestDecline;
        dhcpDbgLeaseRenewDeclineCnt = dhcpDbgLeaseRenewDecline;
        dhcpDbgLeaseRebindDeclineCnt = dhcpDbgLeaseRebindDecline;
#endif  // TCPIP_DHCP_DEBUG_MASK

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAIL_ARP) != 0
        dhcpDbgFailArpCheckCnt = dhcpDbgFailArpCheck;
#endif  // TCPIP_DHCP_DEBUG_MASK
    }
    
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void F_DHCPCleanup(void)
{
    size_t ix;

    if(dhcpClientSocket != INVALID_UDP_SOCKET)
    {
        (void)TCPIP_UDP_Close(dhcpClientSocket);
        dhcpClientSocket = INVALID_UDP_SOCKET;
    }

    if(DHCPClients != NULL)
    {
        DHCP_CLIENT_VARS* pClient = DHCPClients; 
        for(ix = 0; ix < dhcpInterfaces; ix++)
        {
            if(pClient->dhcpFilterHandle != NULL)
            {
                (void)Ipv4DeRegisterFilter(pClient->dhcpFilterHandle);
            }
            pClient++;
        }

        (void)TCPIP_HEAP_Free(dhcpMemH, DHCPClients);
        DHCPClients = NULL;
    }

    TCPIP_Notification_Deinitialize(&dhcpRegisteredUsers, dhcpMemH);
        
    if(dhcpSignalHandle != NULL)
    {
        TCPIPStackSignalHandlerDeregister(dhcpSignalHandle);
        dhcpSignalHandle = NULL;
    }

    dhcpInterfaces = 0U;
}
#else
#define F_DHCPCleanup()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static void F_DHCPSetRunFail(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_STATUS newState, bool expBackoff) 
{
    F_DHCPClientStateSet(pClient, newState);
    pClient->dhcpOp = (uint16_t)TCPIP_DHCP_OPER_INIT;       // failure forces a brand new lease acquisition
    if(expBackoff)
    {   // exponential backoff the DHCP timeout
        pClient->dhcpTmo <<= 1;
        if(pClient->dhcpTmo > TCPIP_DHCP_EXP_BACKOFF_LIMIT)
        {
            pClient->dhcpTmo = TCPIP_DHCP_EXP_BACKOFF_LIMIT;
        }
    }

    if(pClient->flags.bReportFail != 0U)
    {
        F_DHCPSetFailTimeout(pClient, false, true);
    }

    pClient->flags.bRetry = 1U;
}

static void F_DHCPSetTimeout(DHCP_CLIENT_VARS* pClient)
{
    // add a random variation [-TCPIP_DHCP_EXP_BACKOFF_FUZZ, TCPIP_DHCP_EXP_BACKOFF_FUZZ] seconds
    uint32_t sysFreq = SYS_TMR_TickCounterFrequencyGet();
    pClient->waitTicks = ((uint32_t)pClient->dhcpTmo - TCPIP_DHCP_EXP_BACKOFF_FUZZ) * sysFreq + SYS_RANDOM_PseudoGet() % ((2U * TCPIP_DHCP_EXP_BACKOFF_FUZZ) * sysFreq);
    pClient->startWait = SYS_TMR_TickCountGet();
}



/*****************************************************************************
  Function:
    bool TCPIP_DHCP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_DHCP_MODULE_CONFIG* pDhcpConfig);

  Summary:
    Resets the DHCP client module for the specified interface.

  Description:
    Resets the DHCP client module, giving up any current lease, knowledge of 
    DHCP servers, etc. for the specified interface.

  Precondition:
    None

  Parameters:
    stackCtrl - pointer to stack structure specifying the interface to initialize

  Returns:
    None

  Remarks:
    This function should be called internally just once per interface 
    by the stack manager.
***************************************************************************/
bool TCPIP_DHCP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* initData)
{
    DHCP_CLIENT_VARS*   pClient;
    bool    iniRes;
    
    if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        if(stackCtrl->pNetIf->Flags.bIsDHCPEnabled != 0U)
        {
            F_DHCPEnable(stackCtrl->pNetIf, TCPIP_DHCP_OPER_INIT);
        }
        return true;
    }

    const TCPIP_DHCP_MODULE_CONFIG* pDhcpConfig = (const TCPIP_DHCP_MODULE_CONFIG*)initData;

    // stack init
    if(dhcpInitCount == 0U)
    {   // first time we're run
        // check configuration data is not missing
        if(pDhcpConfig == NULL)
        {
            return false;
        }
        // store the memory allocation handle
        dhcpMemH = stackCtrl->memH;
        
        dhcpClientSocket = INVALID_UDP_SOCKET;

        DHCPClients = (DHCP_CLIENT_VARS*)TCPIP_HEAP_Calloc(dhcpMemH,  stackCtrl->nIfs, sizeof(DHCP_CLIENT_VARS));
        if(DHCPClients == NULL)
        {   // failed
            return false;
        }

        // create the DHCP timer
        dhcpSignalHandle = TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_DHCP_Task, TCPIP_DHCP_TASK_TICK_RATE);
        iniRes = TCPIP_Notification_Initialize(&dhcpRegisteredUsers);
        dhcpClientPort = pDhcpConfig->dhcpCliPort;
        dhcpServerPort = pDhcpConfig->dhcpSrvPort;
        dhcpClientSocket = F_DHCPOpenSocket();


        if(dhcpSignalHandle == NULL || iniRes == false || dhcpClientSocket == INVALID_UDP_SOCKET)
        {   // cannot create the DHCP timer/registration list or socket
            F_DHCPCleanup();
            return false;
        }
        
        // initialize the clients
        dhcpInterfaces = stackCtrl->nIfs;

    }
            
    pClient = DHCPClients + stackCtrl->netIx;

    // Reset state machine and flags to default values
    pClient->flags.val = 0;

    // update the DHCP parameters
    // make sure we have a valid timeout


    // use a safe value for the  fail timeout
    pClient->tOpFailTmo = (pDhcpConfig->dhcpTmo < 1U) ? (uint16_t)TCPIP_DHCP_INIT_FAIL_TMO : (uint16_t)pDhcpConfig->dhcpTmo;

    // set a proper timeout base
#if (TCPIP_DHCP_EXP_BACKOFF_BASE != (TCPIP_DHCP_EXP_BACKOFF_FUZZ + 1U))
    pClient->dhcpTmoBase = (TCPIP_DHCP_EXP_BACKOFF_BASE < TCPIP_DHCP_EXP_BACKOFF_FUZZ + 1U) ? (uint16_t)TCPIP_DHCP_EXP_BACKOFF_FUZZ + (uint16_t)1U : (uint16_t)TCPIP_DHCP_EXP_BACKOFF_BASE;
#else
    pClient->dhcpTmoBase = (uint16_t)TCPIP_DHCP_EXP_BACKOFF_BASE;
#endif// (TCPIP_DHCP_EXP_BACKOFF_BASE != (TCPIP_DHCP_EXP_BACKOFF_FUZZ + 1U))
    pClient->tLeaseCheck = TCPIP_DHCP_ARP_LEASE_CHECK_TMO;
    
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAKE_TMO) != 0
    pClient->dhcpTmoBase = dhcpDbgBaseTmo;
    pClient->tOpFailTmo = dhcpDbgInitTmo;
#endif  // TCPIP_DHCP_DEBUG_MASK

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAIL_ARP) != 0
    if(dhcpDbgFakeArpAddressStr != NULL)
    {
        (void)TCPIP_Helper_StringToIPAddress(dhcpDbgFakeArpAddressStr, &dhcpDbgFakeArpAddress);
    }
    else
    {
        dhcpDbgFakeArpAddress.Val = 0U;
    }
#endif  // TCPIP_DHCP_DEBUG_MASK

    if(stackCtrl->pNetIf->Flags.bIsDHCPEnabled != 0U)
    {   // override the pDhcpConfig->dhcpEnable passed with the what the stack manager says
        F_DHCPEnable(stackCtrl->pNetIf, TCPIP_DHCP_OPER_INIT);
    }

    dhcpInitCount++;


    return true;
}

/*****************************************************************************
  Function:
    void TCPIP_DHCP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)

  Summary:
    Turns off the DHCP client module for the specified interface.

  Description:
    DHCP client process when the specified interface or whole stack goes down

  Precondition:
    None

  Parameters:
    stackData - pointer to stack structure specifying the interface to deinitialize

  Returns:
    None

  Remarks:
    This function is called internally just once per interface 
    by the stack manager.
***************************************************************************/
#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_DHCP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN) // interface down

    if(dhcpInitCount > 0U)
    {   // we're up and running
        // one way or another this interface is going down
        F_DHCPClientClose(stackCtrl->pNetIf, true, true);

        //  the registered users for this interface are not removed
        //  since this interface is closed there won't be any event generated on it anyway
        //  deallocation will wait for the whole stack to deinit 
        //  same for IPv4 filters

        if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--dhcpInitCount == 0U)
            {   // all closed
                // release resources
                F_DHCPCleanup();
                dhcpMemH = NULL;
            }
        }
    }

}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)


/*****************************************************************************
  Function:
    void TCPIP_DHCP_Disable(TCPIP_NET_HANDLE hNet)

  Summary:
    Disables the DHCP Client for the specified interface.

  Description:
    Disables the DHCP client for the specified interface.

  Precondition:
    None

  Parameters:
    pNetIf - Interface to disable the DHCP client on.

  Returns:
    true if success
    false otherwise

  Remarks:
    The current lease will be released.
***************************************************************************/
bool TCPIP_DHCP_Disable(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);
    if(DHCPClients != NULL && pNetIf != NULL)
    {
        DHCP_CLIENT_VARS* pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf);
        if(pClient->flags.bDHCPEnabled != 0U)
        {
            F_DHCPClientClose(pNetIf, true, true);
            TCPIP_STACK_AddressServiceEvent(pNetIf, TCPIP_STACK_ADDR_SRVC_DHCPC, TCPIP_ADDR_SRVC_EV_USER_STOP);
            F_DHCPDbgAddServiceEvent(pClient, TCPIP_ADDR_SRVC_EV_USER_STOP, NULL);
            F_DHCPNotifyClients(pNetIf, DHCP_EVENT_SERVICE_DISABLED, 0, 0);
        }
        return true;
    }

    return false;
}


bool TCPIP_DHCP_Enable(TCPIP_NET_HANDLE hNet)
{
    return F_DHCPStartOperation(TCPIPStackHandleToNetUp(hNet), TCPIP_DHCP_OP_REQ_ENABLE, 0);
}

bool TCPIP_DHCP_Renew(TCPIP_NET_HANDLE hNet)
{
    return F_DHCPStartOperation(TCPIPStackHandleToNetUp(hNet), TCPIP_DHCP_OP_REQ_RENEW, 0);
}


bool TCPIP_DHCP_Request(TCPIP_NET_HANDLE hNet, IPV4_ADDR reqAddress)
{
    return F_DHCPStartOperation(TCPIPStackHandleToNetUp(hNet), TCPIP_DHCP_OP_REQ_REQUEST, reqAddress.Val);
}

static bool F_DHCPStartOperation(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPERATION_REQ opReq, uint32_t reqAddress)
{
    if(DHCPClients == NULL || pNetIf == NULL)
    {
        return false;
    }

    DHCP_CLIENT_VARS* pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf);

    bool res = false;
    TCPIP_DHCP_OPERATION_TYPE opType = TCPIP_DHCP_OPER_NONE;

    switch(opReq)
    {
        case TCPIP_DHCP_OP_REQ_ENABLE:
            if(pClient->flags.bDHCPEnabled != 0U)
            {   // already enabled
                res = true;
                break;
            }

            if(TCPIP_STACK_AddressServiceCanStart(pNetIf, TCPIP_STACK_ADDR_SRVC_DHCPC))
            {
                opType = TCPIP_DHCP_OPER_INIT;
                res = true;
            }
            break;

        case TCPIP_DHCP_OP_REQ_RENEW:
            // DHCP should be running and bound
            if(pClient->flags.bDHCPEnabled != 0U && pClient->flags.bIsBound != 0U)
            {
                opType = TCPIP_DHCP_OPER_RENEW;
                res = true;
            }
            break;

        case TCPIP_DHCP_OP_REQ_REQUEST:
            // if DHCP not running, then it needs to be enabled
            if(reqAddress != 0U)
            {
                if(pClient->flags.bDHCPEnabled != 0U)
                {   // depending on the client state
                    if(pClient->smState == (uint16_t)TCPIP_DHCP_GET_REQUEST_ACK || 
                       pClient->smState == (uint16_t)TCPIP_DHCP_GET_RENEW_ACK ||
                       pClient->smState == (uint16_t)TCPIP_DHCP_GET_REBIND_ACK)
                    {   // already in a request
                       res = false;
                       break;
                    }
                    // avoid replying to these requests while changing the address
                    IPV4_ADDR zeroAdd = {0};
                    TCPIPStackSetConfigAddress(pNetIf, &zeroAdd, &zeroAdd, NULL, true);
                }
                else if(!TCPIP_STACK_AddressServiceCanStart(pNetIf, TCPIP_STACK_ADDR_SRVC_DHCPC))
                {
                    res = false;
                    break;
                }
                else
                {
                    // do nothing
                }
                
                opType = TCPIP_DHCP_OPER_INIT_REBOOT;
                res = true;
            }

            break;
        
        default:
            // do nothing
            TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
            break;
    }

    if(opType != TCPIP_DHCP_OPER_NONE)
    {
        pClient->dhcpIPAddress.Val = reqAddress;
        F_DHCPEnable(pNetIf, opType);
    }

    return res;
}

/*****************************************************************************
  Function:
    bool TCPIP_DHCP_IsEnabled(CPIP_NET_HANDLE hNet)

  Summary:
    Determins if the DHCP client is enabled on the specified interface.

  Description:
    Determins if the DHCP client is enabled on the specified interface.

  Precondition:
    None

  Parameters:
     hNet- Interface to query.

  Returns:
    None
***************************************************************************/
bool TCPIP_DHCP_IsEnabled(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);
    if(DHCPClients != NULL && pNetIf != NULL)
    {
        DHCP_CLIENT_VARS* pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf);
        return pClient->flags.bDHCPEnabled != 0U;
    }
    return false;
}

bool TCPIP_DHCP_IsActive(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);
    if(DHCPClients != NULL && pNetIf != NULL)
    {
        return pNetIf->Flags.bIsDHCPEnabled != 0U;
    }
    return false;
}

/*****************************************************************************
  Function:
    bool TCPIP_DHCP_IsBound(TCPIP_NET_HANDLE hNet)

  Summary:
    Determins if the DHCP client has an IP address lease on the specified 
    interface.

  Description:
    Determins if the DHCP client has an IP address lease on the specified 
    interface.

  Precondition:
    None

  Parameters:
    hNet - Interface to query

  Returns:
    true - DHCP client has obtained an IP address lease (and likely other 
        parameters) and these values are currently being used.
    false - No IP address is currently leased
***************************************************************************/
bool TCPIP_DHCP_IsBound(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);
    if(DHCPClients != NULL && pNetIf != NULL)
    {
        return (DHCPClients + TCPIP_STACK_NetIxGet(pNetIf))->flags.bIsBound != 0U;
    }

    return false;
}


/*****************************************************************************
  Function:
    bool TCPIP_DHCP_IsServerDetected(TCPIP_NET_HANDLE hNet)

  Summary:
    Determins if the DHCP client on the specified interface has seen a DHCP 
    server.

  Description:
    Determins if the DHCP client on the specified interface has seen a DHCP 
    server.
    
  Precondition:
    None

  Parameters:
    hNet- Interface to query.

  Returns:
    true - At least one DHCP server is attached to the specified network 
        interface.
    false - No DHCP servers are currently detected on the specified network 
        interface.
***************************************************************************/
bool TCPIP_DHCP_IsServerDetected(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);
    if(DHCPClients != NULL && pNetIf != NULL)
    {
        return (DHCPClients + TCPIP_STACK_NetIxGet(pNetIf))->flags.bDHCPServerDetected != 0U;
    }
    return false;
}

// adjust the DHCP timeout
// how long to wait before a DHCP lease in the initialization
// and base time out; seconds
bool TCPIP_DHCP_RequestTimeoutSet(TCPIP_NET_HANDLE hNet, uint16_t initTmo, uint16_t dhcpBaseTmo)
{   
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);
    if(DHCPClients != NULL && pNetIf != NULL && initTmo != 0U)
    {
        DHCP_CLIENT_VARS* pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf);
        pClient->tOpFailTmo = initTmo;
        dhcpBaseTmo = (dhcpBaseTmo < (uint16_t)TCPIP_DHCP_EXP_BACKOFF_FUZZ + 1U) ? (uint16_t)TCPIP_DHCP_EXP_BACKOFF_FUZZ + 1U : dhcpBaseTmo; 
        pClient->dhcpTmoBase = dhcpBaseTmo;
        return true;
    }
    return false;
}


void TCPIP_DHCP_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;
    bool                isTmo = false;

    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(((uint32_t)sigPend & (uint32_t)TCPIP_MODULE_SIGNAL_TMO) != 0U)
    { // regular TMO occurred
        isTmo = true;
    }

    if(isTmo || ((uint32_t)sigPend & (uint32_t)TCPIP_MODULE_SIGNAL_RX_PENDING) != 0U)
    {   // execute either for TMO or RX signal
        TCPIP_DHCP_Process(isTmo);
    }
}

// send a signal to the DHCP module that data is available
// no manager alert needed since this normally results as a higher layer (UDP) signal
static void F_DHCPSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_UDP_SIGNAL_RX_DATA)
    {
        (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}


static void TCPIP_DHCP_Process(bool isTmo)
{
    // get incoming packet info and generate RX event
    F_DHCPRxGetInfo();

    unsigned int        recvMsg;
    DHCP_CLIENT_VARS*   pClient;
    size_t              netIx, nNets;
    const TCPIP_NET_IF* pNetIf;

    nNets = TCPIP_STACK_NumberOfNetworksGet();
    for(netIx = 0; netIx < nNets; netIx++) 
    {
        pNetIf = (const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet (netIx);
        if(!TCPIP_STACK_NetworkIsUp(pNetIf))
        {   // inactive interface
            continue;
        }

        pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf); 

        if(pClient->flags.bDHCPEnabled == 0U)
        {   // not enabled on this interface
            continue;
        }

        // check loss of lease
        F_DHCPCheckRunFailEvent(pNetIf, pClient);

        switch(pClient->smState)
        {
            case (uint16_t)TCPIP_DHCP_IDLE:
                break;

            case (uint16_t)TCPIP_DHCP_WAIT_LINK:
                if(!TCPIP_STACK_NetworkIsLinked(pNetIf))
                {   // no connection yet
                    break;
                }


                // advance the state machine according to the DHCP operation
                if(pClient->dhcpOp == (uint16_t)TCPIP_DHCP_OPER_INIT_REBOOT)
                {
                    F_DHCPClientStateSet(pClient, TCPIP_DHCP_SEND_REQUEST);
                }
                else if(pClient->dhcpOp == (uint16_t)TCPIP_DHCP_OPER_RENEW)
                {
                    F_DHCPClientStateSet(pClient, TCPIP_DHCP_SEND_RENEW);
                }
                else
                {
                    F_DHCPClientStateSet(pClient, TCPIP_DHCP_SEND_DISCOVERY);
                }

                // activate the IPv4 filter
                F_DHCPSetIPv4Filter(pClient, true);
                break;
              

            case (uint16_t)TCPIP_DHCP_SEND_DISCOVERY:
                // set a default lease just in case the server won't specify one
                F_DHCPSetLeaseTimeParams(pClient, NULL);
                pClient->validValues.val = 0x00;
                pClient->flags.bIsBound = 0U;    
                pClient->flags.bOfferReceived = 0U;
                pClient->boundSrvAddress.Val = 0U;

                // Send the DHCP Discover broadcast
                if(!F_DHCPSend(pClient, pNetIf, TCPIP_DHCP_DISCOVER_MESSAGE, (uint32_t)TCPIP_DHCP_FLAG_SEND_ZERO_ADD | (uint32_t)TCPIP_DHCP_FLAG_SEND_BCAST))
                {
                    break;
                }

                F_DHCPNotifyClients(pNetIf, DHCP_EVENT_DISCOVER, 0, 0);
                // Start a timer and begin looking for a response
                F_DHCPSetTimeout(pClient);
                F_DHCPClientStateSet(pClient, TCPIP_DHCP_GET_OFFER);
                break;

            case (uint16_t)TCPIP_DHCP_GET_OFFER:
                // process received data
                recvMsg = F_DHCPProcessReceiveData(pClient, pNetIf);
                if(recvMsg == TCPIP_DHCP_TIMEOUT_MESSAGE)
                {   // no data available
                    // Go back and transmit a new discovery if we didn't get an offer within the requested time
                    if((SYS_TMR_TickCountGet() - pClient->startWait) >= pClient->waitTicks)
                    {
                        F_DHCPSetRunFail(pClient, TCPIP_DHCP_SEND_DISCOVERY, true);  
                        F_DHCPNotifyClients(pNetIf, DHCP_EVENT_TIMEOUT, 0, 0);
                    }
                    break;
                }
                else if(recvMsg != TCPIP_DHCP_UNKNOWN_MESSAGE)
                {   // Got some reply
                    // Let the DHCP server module know that there is a DHCP server 
                    // on this network
                    pClient->flags.bDHCPServerDetected = 1U;
                }
                else
                {
                    // do nothing
                }

                // However we're waiting for an offer
                if(recvMsg != TCPIP_DHCP_OFFER_MESSAGE)
                {
                    break;
                }

                F_DHCPClientStateSet(pClient, TCPIP_DHCP_SEND_REQUEST);
                break;

            case (uint16_t)TCPIP_DHCP_SEND_REQUEST:
                // Send the DHCP request message
                if(!F_DHCPSend(pClient, pNetIf, TCPIP_DHCP_REQUEST_MESSAGE, (pClient->dhcpOp == (uint16_t)TCPIP_DHCP_OPER_INIT_REBOOT) ? (uint32_t)TCPIP_DHCP_FLAG_SEND_BCAST : (uint32_t)TCPIP_DHCP_FLAG_SEND_ZERO_ADD | (uint32_t)TCPIP_DHCP_FLAG_SEND_BCAST))
                {
                    break;
                }
                // store the request time
                pClient->tRequest = TCPIP_SecCountGet();
                F_DHCPNotifyClients(pNetIf, DHCP_EVENT_REQUEST, 0, 0);
                // Start a timer and begin looking for a response
                F_DHCPSetTimeout(pClient);
                F_DHCPClientStateSet(pClient, TCPIP_DHCP_GET_REQUEST_ACK);
                break;

            case (uint16_t)TCPIP_DHCP_GET_REQUEST_ACK:
                // process received data
                if(F_DHCPProcessReceiveData(pClient, pNetIf) == TCPIP_DHCP_TIMEOUT_MESSAGE)
                {   // no data available
                    // Go back and retransmit a new discovery if we didn't get an ACK
                    if((SYS_TMR_TickCountGet() - pClient->startWait) >= pClient->waitTicks)
                    {
                        F_DHCPSetRunFail(pClient, TCPIP_DHCP_SEND_DISCOVERY, true);
                        F_DHCPNotifyClients(pNetIf, DHCP_EVENT_TIMEOUT, 0, 0);
                    }
                }
                break;

            case (uint16_t)TCPIP_DHCP_WAIT_LEASE_CHECK:
                // check for an ARP entry
                {
                    bool arpChkFail = false;
                    bool arpChkSuccess = false;
                    IPV4_ADDR arpCheck;

                    arpCheck.Val = pClient->dhcpIPAddress.Val;
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAIL_ARP) != 0
                    if(dhcpDbgFakeArpAddress.Val != 0U)
                    {
                        arpCheck.Val = dhcpDbgFakeArpAddress.Val;
                    }
#endif  // TCPIP_DHCP_DEBUG_MASK

                    if(TCPIP_ARP_IsResolved(pNetIf, &arpCheck, NULL))
                    {   // oooops, someone else with this address!
                        arpChkFail = true;
                    }
                    else if((TCPIP_MsecCountGet() - pClient->startWait) >= pClient->tLeaseCheck)
                    {   // no ARP conflict
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAIL_ARP) != 0
                        if(dhcpDbgFailArpCheckCnt != 0)
                        {
                            dhcpDbgFailArpCheckCnt--;
                            arpChkFail = true;
                        }
                        else
#endif  // TCPIP_DHCP_DEBUG_MASK
                        {   // validation success
                            F_DHCPSetNewLease(pClient, pNetIf);
                            arpChkSuccess = true;
                        }
                    }
                    else
                    {
                        // do nothing; no timeout yet
                    }
                    
                    if(arpChkFail || arpChkSuccess)
                    {   // remove ARP entry so that we can probe it again if DHCP is disabled/enabled
                        // or the lease is lost quicker than the ARP entry expiration timeout    
                        (void)TCPIP_ARP_EntryRemove(pNetIf,  &arpCheck);
                    }

                    if(arpChkFail)
                    {   // fail
                        (void)F_DHCPSend(pClient, pNetIf, TCPIP_DHCP_DECLINE_MESSAGE, (uint32_t)TCPIP_DHCP_FLAG_SEND_ZERO_ADD | (uint32_t)TCPIP_DHCP_FLAG_SEND_BCAST);
                        F_DHCPClientStateSet(pClient, TCPIP_DHCP_WAIT_LEASE_RETRY);
                        pClient->startWait = TCPIP_SecCountGet();
                        F_DHCPNotifyClients(pNetIf, DHCP_EVENT_DECLINE, 0, 0);
                    }
                }
                break;

            case (uint16_t)TCPIP_DHCP_WAIT_LEASE_RETRY:
                // wait before we can attempt another retry
                if((TCPIP_SecCountGet() - pClient->startWait) >= (uint32_t)TCPIP_DHCP_WAIT_ARP_FAIL_CHECK_TMO)
                {
                    F_DHCPSetRunFail(pClient, TCPIP_DHCP_SEND_DISCOVERY, false);
                }
                break;

            case (uint16_t)TCPIP_DHCP_SKIP_LEASE_CHECK:
                // set bound directly, without an ARP check
                F_DHCPSetNewLease(pClient, pNetIf);
                break;

            case (uint16_t)TCPIP_DHCP_BOUND:
                // check for T1 timeout
                if((TCPIP_SecCountGet() - pClient->tRequest) < pClient->t1Seconds)
                {   // within BOUND state
                    break;
                }
                // time for renew
                F_DHCPClientStateSet(pClient, TCPIP_DHCP_SEND_RENEW);
                break;

            case (uint16_t)TCPIP_DHCP_SEND_RENEW:
                // Send the DHCP request message
                if(!F_DHCPSend(pClient, pNetIf, TCPIP_DHCP_REQUEST_RENEW_MESSAGE, 0U))
                {
                    break;
                }
                // store the request time
                pClient->tRenewRequest = TCPIP_SecCountGet();
                F_DHCPNotifyClients(pNetIf, DHCP_EVENT_REQUEST_RENEW, 0, 0);
                pClient->flags.bOfferReceived = 0U;

                // Set as timeout half the time remaining to T2 (rebind)
                pClient->t3Seconds = ((pClient->t2Seconds - pClient->t1Seconds) - (pClient->tRenewRequest - pClient->tRequest)) >> 1;
                if( pClient->t3Seconds < TCPIP_DHCP_RENEW_RETRY_TIMEOUT)
                {
                    pClient->t3Seconds = TCPIP_DHCP_RENEW_RETRY_TIMEOUT;
                }
                pClient->startWait = TCPIP_SecCountGet();
                F_DHCPClientStateSet(pClient, TCPIP_DHCP_GET_RENEW_ACK);
                break;

            case (uint16_t)TCPIP_DHCP_GET_RENEW_ACK:
                // process received data
                if(F_DHCPProcessReceiveData(pClient, pNetIf) == TCPIP_DHCP_TIMEOUT_MESSAGE)
                {   // no data available
                    // check first for T2 timeout
                    if((TCPIP_SecCountGet() - pClient->tRequest) >= pClient->t2Seconds)
                    {   // RENEWING state expired; move to REBIND!
                        F_DHCPClientStateSet(pClient, TCPIP_DHCP_SEND_REBIND);
                    }
                    else if((TCPIP_SecCountGet() - pClient->startWait) >= pClient->t3Seconds)
                    {   // check if there's time for retransmission of another DHCP request
                        F_DHCPClientStateSet(pClient, TCPIP_DHCP_SEND_RENEW);
                    }
                    else
                    {
                        // do nothing; no time for retry yet;
                    }
                }

                break;

            case (uint16_t)TCPIP_DHCP_SEND_REBIND:
                // Send the DHCP request message
                if(!F_DHCPSend(pClient, pNetIf, TCPIP_DHCP_REQUEST_RENEW_MESSAGE, (uint32_t)TCPIP_DHCP_FLAG_SEND_BCAST))
                {
                    break;
                }
                // store the request time
                pClient->tRenewRequest = TCPIP_SecCountGet();
                F_DHCPNotifyClients(pNetIf, DHCP_EVENT_REQUEST_REBIND, 0, 0);
                pClient->flags.bOfferReceived = 0U;

                // Set as timeout half the time remaining to Texp (lease expiration)
                pClient->t3Seconds = ((pClient->tExpSeconds - pClient->t2Seconds) - (pClient->tRenewRequest - pClient->tRequest)) >> 1;
                if( pClient->t3Seconds < TCPIP_DHCP_REBIND_RETRY_TIMEOUT)
                {
                    pClient->t3Seconds = TCPIP_DHCP_REBIND_RETRY_TIMEOUT;
                }
                pClient->startWait = TCPIP_SecCountGet();
                F_DHCPClientStateSet(pClient, TCPIP_DHCP_GET_REBIND_ACK);
                break;

            case (uint16_t)TCPIP_DHCP_GET_REBIND_ACK:
                // process received data
                if(F_DHCPProcessReceiveData(pClient, pNetIf) == TCPIP_DHCP_TIMEOUT_MESSAGE)
                {   // no data available
                    // check first for Texp timeout
                    if((TCPIP_SecCountGet() - pClient->tRequest) >= pClient->tExpSeconds)
                    {   // REBIND state expired; restart
                        F_DHCPSetRunFail(pClient, TCPIP_DHCP_SEND_DISCOVERY, false);
                        F_DHCPNotifyClients(pNetIf, DHCP_EVENT_TIMEOUT, 0, 0);
                    }
                    else if((TCPIP_SecCountGet() - pClient->startWait) >= pClient->t3Seconds)
                    {   // check if there's time for retransmission of another DHCP request
                        F_DHCPClientStateSet(pClient, TCPIP_DHCP_SEND_REBIND);
                    }
                    else
                    {
                        // do nothing; no time for retry yet;
                    }
                }
                break;

            default:
                TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
                break;
        }
    }

    // everything that needed processing was done; discard leftovers
    (void)TCPIP_UDP_Discard(dhcpClientSocket);
}



/*****************************************************************************
Function:
  unsigned int F_DHCPReceive(DHCP_CLIENT_VARS* pClient, TCPIP_NET_IF* pNetIf)

Description:
  Receives and parses a DHCP message.

Precondition:
  A DHCP message is waiting in the UDP buffer.

Parameters:
  pClient - client descriptor
  pNetIf - interface to use

Returns:
  One of the DCHP message types
  

Remarks:

    DHCP PACKET FORMAT AS PER RFC 1541

                    0                   1                   2                   3
                    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
                    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                    |     op (1)    |   htype (1)   |   hlen (1)    |   hops (1)    |
                    +---------------+---------------+---------------+---------------+
                    |                            xid (4)                            |
                    +-------------------------------+-------------------------------+
                    |           secs (2)            |           flags (2)           |
                    +-------------------------------+-------------------------------+
                    |                          ciaddr  (4)                          |
                    +---------------------------------------------------------------+
                    |                          yiaddr  (4)                          |
                    +---------------------------------------------------------------+
                    |                          siaddr  (4)                          |
                    +---------------------------------------------------------------+
                    |                          giaddr  (4)                          |
                    +---------------------------------------------------------------+
                    |                                                               |
                    |                          chaddr  (16)                         |
                    |                                                               |
                    |                                                               |
                    +---------------------------------------------------------------+
                    |                                                               |
                    |                          sname   (64)                         |
                    +---------------------------------------------------------------+
                    |                                                               |
                    |                          file    (128)                        |
m cookie offs: 236  +---------------------------------------------------------------+
                    |    0x63      |   0x82         |    0x53       |   0x63        |
options offs:  240  |                                                               |
                    |                          options - variable (312)             |
                    +---------------------------------------------------------------+

minimum packet: 240 bytes (no option except the magic cookie)
minimum packet that needs to be accepted: 236 + 312 = 548 bytes (RFC)

***************************************************************************/
static unsigned int F_DHCPReceive(DHCP_CLIENT_VARS* pClient, const TCPIP_NET_IF* pNetIf)
{

    TCPIP_DHCP_FRAME_HEADER    dhcpHdr;
    TCPIP_DHCP_FRAME_OPTIONS   dhcpOpt;
    TCPIP_DHCP_OPTION_PROCESS_DATA  dhcpOptData;
    int32_t optSize;    // size of the option in this DHCP packet
    TCPIP_DHCP_OPTION_RESULT   dspRes;

    unsigned int               msgType;
    UDP_SOCKET                 s;
    bool                       keepMessage;     // if the retrieved message should be discarded 
                                                // i.e. belongs to the client running on this interface and was processed


    uint32_t rxErrCode = 0;
    s = dhcpClientSocket;
    msgType = TCPIP_DHCP_UNKNOWN_MESSAGE;
    keepMessage = false;

    while(true)
    {
        // get the HDCP header
        dhcpHdr.op = 0;
        (void)TCPIP_UDP_ArrayGet(s, dhcpHdr.v, (uint16_t)sizeof(dhcpHdr));

        // Make sure this is TCPIP_BOOT_REPLY.
        if ( dhcpHdr.op != TCPIP_BOOT_REPLY )
        {
            rxErrCode = 1;  
            break;
        }

        // ignore the unused part of the header: server host name and boot file
        if(TCPIP_UDP_ArrayGet(s, NULL, (uint16_t)sizeof(TCPIP_DHCP_FRAME_OPT_HEADER)) != (uint16_t)sizeof(TCPIP_DHCP_FRAME_OPT_HEADER))
        {
            rxErrCode = 2;  
            break;
        }

        // get the variable options
        optSize = (int32_t)TCPIP_UDP_ArrayGet(s, dhcpOpt.cookie, (uint16_t)sizeof(dhcpOpt));
        if(optSize < (int32_t)sizeof(dhcpOpt.cookie) || (memcmp(dhcpOpt.cookie, dhcpMagicCookie, sizeof(dhcpOpt.cookie)) != 0))
        {
            rxErrCode = 3;  
            break;
        }

        // valid message
        // check that this message belongs to our client
        bool doProcess = false;     // if the message should be processed
        if(pClient->smState == (uint16_t)TCPIP_DHCP_GET_OFFER || pClient->smState == (uint16_t)TCPIP_DHCP_GET_REQUEST_ACK ||
                pClient->smState == (uint16_t)TCPIP_DHCP_GET_RENEW_ACK || pClient->smState == (uint16_t)TCPIP_DHCP_GET_REBIND_ACK)
        {   // client waiting for some DHCP message
            if(dhcpHdr.xid == pClient->transactionID && memcmp(dhcpHdr.chaddr, TCPIPStack_NetMACAddressGet(pNetIf), sizeof(TCPIP_MAC_ADDR)) == 0)
            {   // seems to be ours
                doProcess = true;   // i.e. ours, process message
            }
        }

        if(doProcess == false)
        {   // valid message but not ours
            rxErrCode =  4;
            keepMessage = true;     // needs to be processed further by client on other interface
            break;
        }

        // ok it's ours, we can process this one and then discard (keepMessage == false)
        // if this is the first offer record yiaddr value/client IP address
        // so that we can REQUEST it.
        if(pClient->flags.bOfferReceived == 0U)
        {
            pClient->dhcpIPAddress.Val = dhcpHdr.yiaddr;
            pClient->validValues.IPAddress = 1;
        }

        (void)memset(&dhcpOptData, 0, sizeof(dhcpOptData));
        dhcpOptData.pOpt = dhcpOpt.options;
        dhcpOptData.optSize = optSize - (int32_t)sizeof(dhcpMagicCookie);   // eat up the cookie
        dhcpOptData.msgType = TCPIP_DHCP_UNKNOWN_MESSAGE;
        // so far so good
        // Dispatch by the Option number
        dspRes = F_DHCPOptionProcess(pClient, &dhcpOptData);

        if(dspRes == TCPIP_DHCP_OPTION_RES_FAIL)
        {   // failed
            rxErrCode = 5;
            break;
        }
        else if(dspRes != TCPIP_DHCP_OPTION_RES_END)
        {   // failed
            rxErrCode = 6;
            break;
        }
        else
        {
            // do nothing
        }

        // check what we got
        if ( dhcpOptData.msgType == TCPIP_DHCP_OFFER_MESSAGE || (dhcpOptData.msgType == TCPIP_DHCP_ACK_MESSAGE && pClient->dhcpOp == (uint16_t)TCPIP_DHCP_OPER_INIT_REBOOT))
        {   // store the current server ID
            pClient->dwServerID.Val = dhcpOptData.serverID.Val;
            pClient->flags.bOfferReceived = 1U;
        }
        else if(pClient->dwServerID.Val != dhcpOptData.serverID.Val)
        {   // Fail if the server id doesn't match
            rxErrCode = 7;
            break;
        }
        else
        {
            // do nothing
        }

        // success
        msgType = dhcpOptData.msgType;

        if(msgType == TCPIP_DHCP_ACK_MESSAGE)
        {   // received a lease; get values
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAKE_LEASE_TIME) != 0
            if(dhcpDbgLeaseTime != 0U)
            {
                dhcpOptData.leaseTime.Val = dhcpDbgLeaseTime; 
                dhcpOptData.renewTime.Val = 0; 
                dhcpOptData.rebindTime.Val = 0; 
            }
#endif  // TCPIP_DHCP_DEBUG_MASK
            F_DHCPSetLeaseTimeParams(pClient, &dhcpOptData);
#if defined TCPIP_DHCP_STORE_BOOT_FILE_NAME
            TCPIP_UDP_RxOffsetSet(s, sizeof(dhcpHdr) + sizeof(((TCPIP_DHCP_FRAME_OPT_HEADER*)0)->sname));
            (void)TCPIP_UDP_ArrayGet(s, pClient->bootFileName, (uint16_t)sizeof(pClient->bootFileName));
            pClient->bootFileName[sizeof(pClient->bootFileName) - 1] = 0;   // make sure is null terminated
#endif // defined TCPIP_DHCP_STORE_BOOT_FILE_NAME
        }

        break;
    }

    if(keepMessage == false)
    {   // either invalid or processed; simply discard
        (void)TCPIP_UDP_Discard(s);                     
    }
    else 
    {   // save it for another client
        TCPIP_UDP_RxOffsetSet(s, 0U);
    }

    F_DHCPDbgRxDisplay(pNetIf, msgType, rxErrCode);

    return msgType;
}

// this routine just checks the message and returns the message type.
// Used for unhandled messages (of no interest to the client, for example the client is already bound)
// but user notification is needed.
static unsigned int F_DHCPCheckMsgType(TCPIP_NET_IF* pNetIf)
{
    TCPIP_DHCP_FRAME_HEADER    dhcpHdr;
    TCPIP_DHCP_FRAME_OPTIONS   dhcpOpt;
    TCPIP_DHCP_OPTION_PROCESS_DATA  dhcpOptData;
    int32_t optSize;    // size of the option in this DHCP packet

    unsigned int               msgType;
    UDP_SOCKET                 s;

    uint32_t rxErrCode = 0;
    s = dhcpClientSocket;
    msgType = TCPIP_DHCP_UNKNOWN_MESSAGE;

    while(true)
    {
        // get the HDCP header
        dhcpHdr.op = 0;
        (void)TCPIP_UDP_ArrayGet(s, dhcpHdr.v, (uint16_t)sizeof(dhcpHdr));

        // Make sure this is TCPIP_BOOT_REPLY.
        if ( dhcpHdr.op != TCPIP_BOOT_REPLY )
        {
            rxErrCode = 1;  
            break;
        }

        // ignore the unused part of the header: server host name and boot file
        if(TCPIP_UDP_ArrayGet(s, NULL, (uint16_t)sizeof(TCPIP_DHCP_FRAME_OPT_HEADER)) != (uint16_t)sizeof(TCPIP_DHCP_FRAME_OPT_HEADER))
        {
            rxErrCode = 2;  
            break;
        }

        // get the variable options
        optSize = (int32_t)TCPIP_UDP_ArrayGet(s, dhcpOpt.cookie, (uint16_t)sizeof(dhcpOpt));
        if(optSize < (int32_t)sizeof(dhcpOpt.cookie) || (memcmp(dhcpOpt.cookie, dhcpMagicCookie, sizeof(dhcpOpt.cookie)) != 0))
        {
            rxErrCode = 3;  
            break;
        }

        // we do not check that this message belongs to one of our clients
        // it can be any DHCP message on the network
        // just get the message type

        (void)memset(&dhcpOptData, 0, sizeof(dhcpOptData));
        dhcpOptData.pOpt = dhcpOpt.options;
        dhcpOptData.optSize = optSize - (int32_t)sizeof(dhcpMagicCookie);   // eat up the cookie
        msgType = F_DHCPOptionGetMsgType(&dhcpOptData);

        break;
    }


    F_DHCPDbgRxDisplay(pNetIf, msgType, rxErrCode);

    return msgType;
}

static TCPIP_DHCP_OPTION_RESULT F_DHCPOptionProcess(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData)
{
    size_t  optIx;
    int     optLen;
    uint8_t opt;
    bool    optFound;
    const TCPIP_DHCP_OPTION_PROC_ENTRY* pEntry;


    while(pOptData->optSize > 0)
    {
        opt = *pOptData->pOpt;
        pEntry = t_DHCPOptProcTbl;
        optFound = false;
        for(optIx = 0; optIx < sizeof(t_DHCPOptProcTbl) / sizeof(*t_DHCPOptProcTbl); optIx++)
        {
            if(opt == pEntry->option)
            {   // found entry
                optLen = (*pEntry->dhcpProcFnc)(pClient, pOptData);
                if(optLen > 0)
                {   // valid option
                    pOptData->pOpt += optLen;
                    pOptData->optSize -= optLen;
                    optFound = true;
                    break;      // option processed
                }
                else if(optLen == 0)
                {   // done
                    return TCPIP_DHCP_OPTION_RES_END;
                }
                else
                {   // some error has occurred
                    return TCPIP_DHCP_OPTION_RES_FAIL;
                }
            }
            pEntry++;
        }

        if(optFound == false)
        {   // not found option
            // unprocessed options are silently ignored
            optLen = (int)*(pOptData->pOpt + 1);
            pOptData->pOpt += optLen + 2;
            pOptData->optSize -= optLen + 2;
        }
    }

    // if we got here there's an error
    return TCPIP_DHCP_OPTION_RES_FAIL;
}

static int F_DHCPOptionProcessMsgType(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData)
{
    if(pOptData->optSize >= (int32_t)sizeof(TCPIP_DHCP_OPTION_DATA_MSG_TYPE))
    {
        TCPIP_DHCP_OPTION_DATA_MSG_TYPE* pMsgType = pOptData->pMsgType;

        if(pMsgType->len == sizeof(pMsgType->type))
        {
            pOptData->msgType = pMsgType->type;
            if(pOptData->msgType != TCPIP_DHCP_OFFER_MESSAGE || pClient->flags.bOfferReceived == 0U)
            {
                return (int)sizeof(TCPIP_DHCP_OPTION_DATA_MSG_TYPE);
            }
            // else throw away the packet if another offer when we already have one)
        }
    }

    return -1;
}

static int F_DHCPOptionProcessSubNet(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData)
{
    if(pOptData->optSize >= (int32_t)sizeof(TCPIP_DHCP_OPTION_DATA_SUBNET_MASK))
    {
        TCPIP_DHCP_OPTION_DATA_SUBNET_MASK* pSubMask = pOptData->pSubMask;

        if(pSubMask->len == sizeof(pSubMask->mask))
        {
            if(pClient->flags.bOfferReceived == 0U)
            {   // the offered IP mask is needed
                (void)memcpy(pClient->dhcpMask.v, pSubMask->mask, sizeof(pClient->dhcpMask)); 
                pClient->validValues.Mask = 1U;
            }
            return (int)sizeof(TCPIP_DHCP_OPTION_DATA_SUBNET_MASK);
        }
    }

    return -1;
}

static int F_DHCPOptionProcessRouter(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData)
{
    if(pOptData->optSize >= (int32_t)sizeof(TCPIP_DHCP_OPTION_DATA_ROUTER))
    {
        TCPIP_DHCP_OPTION_DATA_ROUTER* pRouter = pOptData->pRouter;
        if(pRouter->len >= sizeof(pRouter->gateway))
        {
            if(pClient->flags.bOfferReceived == 0U)
            {   // the offered gateway IP is needed
                (void)memcpy(pClient->dhcpGateway.v, pRouter->gateway, sizeof(pClient->dhcpGateway)); 
                pClient->validValues.Gateway = 1U;
            }
            return 2 + (int)pRouter->len;
        }
    }

    return -1;
}

#if (TCPIP_DHCP_USE_OPTION_TIME_SERVER != 0)
static int F_DHCPOptionProcessTimeServer(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData)
{
    if(pOptData->optSize >= (int32_t)sizeof(TCPIP_DHCP_OPTION_TIME_SERVER))
    {
        TCPIP_DHCP_OPTION_TIME_SERVER* pOptTServer = pOptData->pOptTServer;
        uint8_t nServers = pOptTServer->len / sizeof(pOptTServer->tServer);
        uint8_t nRem = pOptTServer->len - nServers * sizeof(pOptTServer->tServer);

        if(nRem == 0 && nServers > 0)
        {
            if(pClient->flags.bOfferReceived == 0U)
            {   // the offered time server IP is needed
                if(nServers > sizeof(pClient->timeServers) / sizeof(*pClient->timeServers))
                {
                    nServers = sizeof(pClient->timeServers) / sizeof(*pClient->timeServers);
                }

                (void)memcpy(pClient->timeServers, pOptTServer->tServer, nServers * sizeof*(pClient->timeServers)); 
                pClient->validValues.tServer = 1U;
                pClient->tServerNo = nServers;
            }
            return 2 + pOptTServer->len;
        }
    }

    return -1;
}
#endif  // (TCPIP_DHCP_USE_OPTION_TIME_SERVER != 0)

#if (TCPIP_DHCP_USE_OPTION_NTP_SERVER != 0)
static int F_DHCPOptionProcessNtpServer(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData)
{
    if(pOptData->optSize >= (int32_t)sizeof(TCPIP_DHCP_OPTION_NTP_SERVER))
    {
        TCPIP_DHCP_OPTION_NTP_SERVER* pOptNtpServer = pOptData->pOptNtpServer;
        uint8_t nServers = pOptNtpServer->len / sizeof(pOptNtpServer->ntpServer);
        uint8_t nRem = pOptNtpServer->len - nServers * sizeof(pOptNtpServer->ntpServer);

        if(nRem == 0U && nServers > 0U)
        {
            if(pClient->flags.bOfferReceived == 0U)
            {   // the offered time server IP is needed
                if(nServers > sizeof(pClient->ntpServers) / sizeof(*pClient->ntpServers))
                {
                    nServers = sizeof(pClient->ntpServers) / sizeof(*pClient->ntpServers);
                }

                (void)memcpy(pClient->ntpServers, pOptNtpServer->ntpServer, nServers * sizeof*(pClient->ntpServers)); 
                pClient->validValues.ntpServer = 1U;
                pClient->ntpServerNo = nServers;
            }
            return 2 + pOptNtpServer->len;
        }
    }

    return -1;
}
#endif  // (TCPIP_DHCP_USE_OPTION_NTP_SERVER != 0)

static int F_DHCPOptionProcessDns(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData)
{
    if(pOptData->optSize >= (int32_t)sizeof(TCPIP_DHCP_OPTION_DATA_DNS1))
    {
        TCPIP_DHCP_OPTION_DATA_DNS* pDns = pOptData->pDns;
        if((pDns->len & 0x3U) == 0U && pDns->len >= sizeof(pDns->dns1))
        {
#if defined(TCPIP_STACK_USE_DNS)
            if(pClient->flags.bOfferReceived == 0U)
            {   // the offered DNS IP is needed
                (void)memcpy(pClient->dhcpDNS.v, pDns->dns1, sizeof(pClient->dhcpDNS)); 
                pClient->validValues.DNS = 1U;

                if(pDns->len >= sizeof(pDns->dns1) + sizeof(pDns->dns2))
                {   // 2nd DNS
                    (void)memcpy(pClient->dhcpDNS2.v, pDns->dns2, sizeof(pClient->dhcpDNS2)); 
                    pClient->validValues.DNS2 = 1U;
                }
            }
#endif  // defined(TCPIP_STACK_USE_DNS)
            return 2 + (int)pDns->len;
        }
    }

    return -1;
}

static int F_DHCPOptionProcessSrvIdent(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData)
{
    if(pOptData->optSize >= (int32_t)sizeof(TCPIP_DHCP_OPTION_DATA_SERVER_ID))
    {
        TCPIP_DHCP_OPTION_DATA_SERVER_ID* pServId = pOptData->pServId;
        if(pServId->len == sizeof(pServId->serverId))
        {   // update the ID
            (void)memcpy( pOptData->serverID.v, pServId->serverId, sizeof(pServId->serverId));
            return (int)sizeof(TCPIP_DHCP_OPTION_DATA_SERVER_ID);
        }
    }

    return -1;
}

static int F_DHCPOptionProcessLease(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData)
{
    if(pOptData->optSize >= (int32_t)sizeof(TCPIP_DHCP_OPTION_DATA_LEASE_TIME))
    {
        TCPIP_DHCP_OPTION_DATA_LEASE_TIME* pLease = pOptData->pLease;
        if(pLease->len == sizeof(pLease->lease))
        {
            // update the lease
            pOptData->leaseTime.v[3] = pLease->lease[0];
            pOptData->leaseTime.v[2] = pLease->lease[1];
            pOptData->leaseTime.v[1] = pLease->lease[2];
            pOptData->leaseTime.v[0] = pLease->lease[3];
            return (int)sizeof(TCPIP_DHCP_OPTION_DATA_LEASE_TIME);
        }
    }

    return -1;
}



static int F_DHCPOptionProcessRenewT(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData)
{
    if(pOptData->optSize >= (int32_t)sizeof(TCPIP_DHCP_OPTION_DATA_RENEW_TIME))
    {
        TCPIP_DHCP_OPTION_DATA_RENEW_TIME* pRenew = pOptData->pRenew;
        if(pRenew->len == sizeof(pRenew->tRenew))
        {
            // update the renew time
            pOptData->renewTime.v[3] = pRenew->tRenew[0];
            pOptData->renewTime.v[2] = pRenew->tRenew[1];
            pOptData->renewTime.v[1] = pRenew->tRenew[2];
            pOptData->renewTime.v[0] = pRenew->tRenew[3];
            return (int)sizeof(TCPIP_DHCP_OPTION_DATA_RENEW_TIME);
        }
    }

    return -1;
}

static int F_DHCPOptionProcessRebindT(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData)
{
    if(pOptData->optSize >= (int32_t)sizeof(TCPIP_DHCP_OPTION_DATA_REBIND_TIME))
    {
        TCPIP_DHCP_OPTION_DATA_REBIND_TIME* pRebind = pOptData->pRebind;
        if(pRebind->len == sizeof(pRebind->tRebind))
        {
            // update the lease
            pOptData->rebindTime.v[3] = pRebind->tRebind[0];
            pOptData->rebindTime.v[2] = pRebind->tRebind[1];
            pOptData->rebindTime.v[1] = pRebind->tRebind[2];
            pOptData->rebindTime.v[0] = pRebind->tRebind[3];
            return (int)sizeof(TCPIP_DHCP_OPTION_DATA_REBIND_TIME);
        }
    }

    return -1;
}


static int F_DHCPOptionProcessEnd(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData)
{
    return 0;   // end detected
}

// return the message type from the option data
static unsigned int F_DHCPOptionGetMsgType(TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData)
{
    if(pOptData->optSize >= (int32_t)sizeof(TCPIP_DHCP_OPTION_DATA_MSG_TYPE))
    {
        TCPIP_DHCP_OPTION_DATA_MSG_TYPE* pMsgType = pOptData->pMsgType;

        if(pMsgType->len == sizeof(pMsgType->type))
        {
            return pMsgType->type;
        }
    }

    return TCPIP_DHCP_UNKNOWN_MESSAGE;
}



// Process the received offer/data
static unsigned int F_DHCPProcessReceiveData(DHCP_CLIENT_VARS* pClient, const TCPIP_NET_IF* pNetIf)
{
    unsigned int recvRes;
    bool    dhcpRecvFail;
    TCPIP_DHCP_EVENT_TYPE dhcpEv;


#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAKE_TMO) != 0
    F_DHCPDbgProcessTmo(pClient);
#endif  // TCPIP_DHCP_DEBUG_MASK

    if(TCPIP_UDP_GetIsReady(dhcpClientSocket) == 0U)
    {
        return TCPIP_DHCP_TIMEOUT_MESSAGE;
    }

    dhcpRecvFail = false;
    dhcpEv = DHCP_EVENT_NONE;
    recvRes = F_DHCPReceive(pClient, pNetIf);

    switch(recvRes)
    {
        case TCPIP_DHCP_ACK_MESSAGE:
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_LEASE_DECLINE) != 0
            if(F_DHCPDbgProcessAckDecline(pClient, pNetIf))
            {
                dhcpRecvFail = true;
                dhcpEv = DHCP_EVENT_DECLINE;
                break;
            }
#endif  // TCPIP_DHCP_DEBUG_MASK

            if( pClient->smState == (uint16_t)TCPIP_DHCP_GET_REQUEST_ACK)
            {
                if(pClient->validValues.IPAddress == 0U || pClient->validValues.Mask == 0U)
                {   // having a new address without a valid mask seems weird
                    dhcpRecvFail = true;
                    break;
                }

                // seems we received a new valid lease
                pClient->leaseSrvAddress.Val = pClient->lastSrvAddress.Val;     // this is the server that granted the lease
                TCPIP_DHCP_STATUS newState;
                if(pClient->tLeaseCheck == 0U)
                {   // skip the ARP check phase...
                    newState = TCPIP_DHCP_SKIP_LEASE_CHECK;
                }
                else
                {   // make sure it's not in use
                    IPV4_ADDR arpCheck;
                    IPV4_ADDR zeroAdd = { 0 };
                    arpCheck.Val = pClient->dhcpIPAddress.Val;
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAIL_ARP) != 0
                    if(dhcpDbgFakeArpAddress.Val != 0U)
                    {
                        arpCheck.Val = dhcpDbgFakeArpAddress.Val;
                        // make sure it's not already cached
                        (void)TCPIP_ARP_EntryRemove(pNetIf,  &arpCheck);
                    }
#endif  // TCPIP_DHCP_DEBUG_MASK
                    // not really  ARP_OPERATION_GRATUITOUS but only one single probe needs to go out
                    (void)TCPIP_ARP_Probe(pNetIf, &arpCheck, &zeroAdd, (TCPIP_ARP_OPERATION_TYPE)((uint16_t)ARP_OPERATION_REQ | (uint16_t)ARP_OPERATION_CONFIGURE | (uint16_t)ARP_OPERATION_GRATUITOUS));
                    pClient->startWait = TCPIP_MsecCountGet();
                    newState = TCPIP_DHCP_WAIT_LEASE_CHECK;
                }
                F_DHCPClientStateSet(pClient, newState);
            }
            else if( pClient->smState == (uint16_t)TCPIP_DHCP_GET_RENEW_ACK || pClient->smState == (uint16_t)TCPIP_DHCP_GET_REBIND_ACK)
            {
                pClient->tRequest = pClient->tRenewRequest;
                // update the bound state
                F_DHCPSetBoundState(pClient);
                dhcpEv = DHCP_EVENT_BOUND;
            }
            else
            {
                // do nothing
            }

            break;

        case TCPIP_DHCP_NAK_MESSAGE:
            dhcpRecvFail = true;
            break;

        default:
            // remain in the same state; do nothing
            break;
    }

    if(dhcpRecvFail)
    {
        F_DHCPSetRunFail(pClient, TCPIP_DHCP_SEND_DISCOVERY, false);
    }

    if(dhcpEv != DHCP_EVENT_NONE)
    {
        F_DHCPNotifyClients(pNetIf, dhcpEv, 0, 0);
    }

    return recvRes;
}

// returns the corresponding RX event for a received message type
static TCPIP_DHCP_EVENT_TYPE F_DHCPRxMsgType2Event(unsigned int msgType)
{
    TCPIP_DHCP_EVENT_TYPE dhcpEv;

    if(msgType == TCPIP_DHCP_OFFER_MESSAGE)
    {
        dhcpEv = DHCP_EVENT_OFFER;
    } 
    else if(msgType == TCPIP_DHCP_ACK_MESSAGE)
    {
        dhcpEv = DHCP_EVENT_ACK;
    } 
    else if(msgType == TCPIP_DHCP_NAK_MESSAGE)
    {
        dhcpEv = DHCP_EVENT_NACK;
    } 
    else
    {   // unknown RX message
        TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
        dhcpEv = DHCP_EVENT_UNKNOWN;
    } 

    return dhcpEv;
}

// returns the source of the event
static TCPIP_DHCP_EV_SOURCE F_DHCPEventSource(TCPIP_DHCP_EVENT_TYPE evType)
{
    TCPIP_DHCP_EV_SOURCE evSrc;

    static const uint8_t event_src_tbl[] = 
    {
        (uint8_t)TCPIP_DHCP_EV_SRC_CLIENT,       // DHCP_EVENT_DISCOVER
        (uint8_t)TCPIP_DHCP_EV_SRC_SERVER,       // DHCP_EVENT_OFFER
        (uint8_t)TCPIP_DHCP_EV_SRC_CLIENT,       // DHCP_EVENT_REQUEST
        (uint8_t)TCPIP_DHCP_EV_SRC_SERVER,       // DHCP_EVENT_ACK
        (uint8_t)TCPIP_DHCP_EV_SRC_CLIENT,       // DHCP_EVENT_DECLINE
        (uint8_t)TCPIP_DHCP_EV_SRC_SERVER,       // DHCP_EVENT_NACK
        (uint8_t)TCPIP_DHCP_EV_SRC_CLIENT,       // DHCP_EVENT_TIMEOUT
        (uint8_t)TCPIP_DHCP_EV_SRC_CLIENT,       // DHCP_EVENT_BOUND
        (uint8_t)TCPIP_DHCP_EV_SRC_CLIENT,       // DHCP_EVENT_REQUEST_RENEW
        (uint8_t)TCPIP_DHCP_EV_SRC_CLIENT,       // DHCP_EVENT_REQUEST_REBIND
        (uint8_t)TCPIP_DHCP_EV_SRC_SERVER,       // DHCP_EVENT_UNKNOWN
        (uint8_t)TCPIP_DHCP_EV_SRC_CONN,         // DHCP_EVENT_CONN_LOST
        (uint8_t)TCPIP_DHCP_EV_SRC_CONN,         // DHCP_EVENT_CONN_ESTABLISHED
        (uint8_t)TCPIP_DHCP_EV_SRC_CONN,         // DHCP_EVENT_SERVICE_DISABLED
    };

    uint8_t evIx = (uint8_t)evType - 1U;

    if(evIx >= sizeof(event_src_tbl) / sizeof(*event_src_tbl))
    {
        TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
        evSrc = TCPIP_DHCP_EV_SRC_NONE;
    }
    else
    {
        evSrc = (TCPIP_DHCP_EV_SOURCE)event_src_tbl[evIx];
    }

    return evSrc;
}

// Retrieves info from incoming DHCP packets
// It also generates RX notification if needed
static void F_DHCPRxGetInfo(void)
{
    TCPIP_DHCP_EVENT_TYPE dhcpEv;
    UDP_SOCKET_INFO sktInfo;
    TCPIP_NET_IF* pNetIf;

    UDP_SOCKET s = dhcpClientSocket;

    if(TCPIP_UDP_GetIsReady(s) == 0U)
    {   // no pending data
        return;
    }

    sktInfo.sourceIPaddress.v4Add.Val = 0U;
    sktInfo.destIPaddress.v4Add.Val = 0U;
    sktInfo.hNet = NULL;
    (void)TCPIP_UDP_SocketInfoGet(s , &sktInfo);
    pNetIf = TCPIPStackHandleToNet(sktInfo.hNet);

    DHCP_CLIENT_VARS* pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf); 
    // store the address of the server that sent the message
    pClient->lastSrvAddress.Val = sktInfo.sourceIPaddress.v4Add.Val;

    // generate the RX event
    unsigned int msgType = F_DHCPCheckMsgType(pNetIf); 
    dhcpEv = F_DHCPRxMsgType2Event(msgType); 

    F_DHCPNotifyClients(pNetIf, dhcpEv, sktInfo.destIPaddress.v4Add.Val, pClient->lastSrvAddress.Val);

    // restore the UDP packet for further processing
    TCPIP_UDP_RxOffsetSet(s, 0U);
}

// a new valid lease has been obtained
// make it active
static void F_DHCPSetNewLease(DHCP_CLIENT_VARS* pClient, const TCPIP_NET_IF* cNetIf)
{
    IPV4_ADDR oldNetIp;
    IPV4_ADDR oldNetMask;

    oldNetIp.Val = TCPIP_STACK_NetAddressGet(cNetIf);
    oldNetMask.Val = TCPIP_STACK_NetMaskGet(cNetIf);

    TCPIP_NET_IF* pNetIf = FC_CNetIf2NetIf(cNetIf);
    TCPIPStackSetConfigAddress(pNetIf, &pClient->dhcpIPAddress, &pClient->dhcpMask, NULL, false);
    if(pClient->validValues.Gateway != 0U)
    {
        TCPIP_STACK_GatewayAddressSet(pNetIf, &pClient->dhcpGateway);
    }
#if defined(TCPIP_STACK_USE_DNS)
    if(pNetIf->Flags.bIsDNSServerAuto != 0U)
    {
        if(pClient->validValues.DNS != 0U)
        {
            TCPIP_STACK_PrimaryDNSAddressSet(pNetIf, &pClient->dhcpDNS);
        }
        if(pClient->validValues.DNS2 != 0U)
        {
            TCPIP_STACK_SecondaryDNSAddressSet(pNetIf, &pClient->dhcpDNS2);
        }
        else
        {
            IPV4_ADDR zeroAdd = {0};
            TCPIP_STACK_SecondaryDNSAddressSet(pNetIf, &zeroAdd);
        }
    }
#endif
    TCPIP_STACK_AddressServiceEvent(pNetIf, TCPIP_STACK_ADDR_SRVC_DHCPC, TCPIP_ADDR_SRVC_EV_RUN_RESTORE); 
    F_DHCPDbgAddServiceEvent(pClient, TCPIP_ADDR_SRVC_EV_RUN_RESTORE, NULL);
    // inform other hosts of this host new address
    // probably this should be ARP_OPERATION_GRATUITOUS; but PROBE_ONLY doesn't pollute the cache
#if (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING == 0)
    (void)TCPIP_ARP_Probe(pNetIf, &pClient->dhcpIPAddress, &pClient->dhcpIPAddress, (TCPIP_ARP_OPERATION_TYPE)((uint16_t)ARP_OPERATION_RESP | (uint16_t)ARP_OPERATION_CONFIGURE | (uint16_t)ARP_OPERATION_PROBE_ONLY));
#endif  // (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING == 0)

    if((pClient->dhcpIPAddress.Val & pClient->dhcpMask.Val) != (oldNetIp.Val & oldNetMask.Val))
    {   // changed networks
        (void)TCPIP_ARP_EntryRemoveNet(pNetIf, &oldNetIp, &oldNetMask, ARP_ENTRY_TYPE_ANY);
    } 

    F_DHCPSetBoundState(pClient);

    F_DHCPNotifyClients(pNetIf, DHCP_EVENT_BOUND, 0, 0);

}

static void F_DHCPSetBoundState(DHCP_CLIENT_VARS* pClient)
{
    // store the address of the server that gave us the lease
    pClient->boundSrvAddress.Val = pClient->leaseSrvAddress.Val;

    F_DHCPClientStateSet(pClient, TCPIP_DHCP_BOUND);
    pClient->flags.bIsBound = 1U; 
    pClient->flags.bWasBound = 1U;    
    pClient->flags.bReportFail = 1U; 
    pClient->flags.bRetry = 0U; 
    // Note: normally we suppress RX 'spurious' messages except when need to notify the user...
    // F_DHCPSetIPv4Filter(pClient, false);
}


static void F_DHCPSetLeaseTimeParams(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pDhcpData)
{
    uint32_t    leaseTime, t1Time, t2Time;

    // set lease time
    if(pDhcpData == NULL || pDhcpData->leaseTime.Val == 0U)
    {
        leaseTime = TCPIP_DHCP_LEASE_TIME_DEFAULT;
    }
    else
    {
        leaseTime = pDhcpData->leaseTime.Val;
    }

    if(leaseTime < 2U * TCPIP_DHCP_LEASE_EXPIRE_FUZZ)
    {   // normally we shouldn't get such short leases
        leaseTime = 2U * TCPIP_DHCP_LEASE_EXPIRE_FUZZ;
    }

    // set renew time T1
    if(pDhcpData == NULL || pDhcpData->renewTime.Val == 0U)
    {
        t1Time = leaseTime >> 1;
    }
    else
    {
        t1Time = pDhcpData->renewTime.Val;
    }

    // set rebind time T2
    if(pDhcpData == NULL || pDhcpData->rebindTime.Val == 0U)
    {
        if(leaseTime >= 0xffffffffU / 7U)
        {
            t2Time = (leaseTime >> 3U) * 7U;
        }
        else
        {
            t2Time = (leaseTime * 7U) >> 3U;
        }
    }
    else
    {
        t2Time = pDhcpData->rebindTime.Val;
    }


    // apply random fuzz
    pClient->t1Seconds = (t1Time - TCPIP_DHCP_LEASE_EXPIRE_FUZZ) + SYS_RANDOM_PseudoGet() %  (2U * TCPIP_DHCP_LEASE_EXPIRE_FUZZ);
    pClient->t2Seconds = (t2Time - TCPIP_DHCP_LEASE_EXPIRE_FUZZ) + SYS_RANDOM_PseudoGet() %  (2U * TCPIP_DHCP_LEASE_EXPIRE_FUZZ);

    // finally set the expire seconds
    pClient->tExpSeconds = leaseTime;

}



/*****************************************************************************
  Function:
        static bool F_DHCPSend(DHCP_CLIENT_VARS* pClient, TCPIP_NET_IF* pNetIf, int messageType, uint32_t dhcpFlags)

  Description:
        Sends a DHCP message.

  Precondition:
        UDP is ready to write a DHCP packet.

  Parameters:
    pClient  - client descriptor
    pNetIf   - interface to use
    messageType - One of the DHCP_TYPE constants
    dhcpFlags   - send operation flags
  Returns:
        true if could send the message
        false otherwise

  Remarks:
 ***************************************************************************/

static bool F_DHCPSend(DHCP_CLIENT_VARS* pClient, const TCPIP_NET_IF* pNetIf, unsigned int messageType, uint32_t dhcpFlags)
{
    UDP_SOCKET s;
    IPV4_ADDR sourceIp;
    bool newTransaction;

    // the TCPIP_DHCP_FRAME_OPTIONS size is greater than
    // both TCPIP_DHCP_FRAME_HEADER and TCPIP_DHCP_FRAME_OPT_HEADER
    // use it for construction of the DHCP complete frame
    union
    {
        TCPIP_DHCP_FRAME_OPTIONS    dhcpOptions;    // generic
        TCPIP_DHCP_FRAME_HEADER     dhcpHdr;
        TCPIP_DHCP_FRAME_OPT_HEADER optHdr;
    }U_FRAME_OPTIONS;

    TCPIP_DHCP_FRAME_HEADER*     pDhcpHdr;
    TCPIP_DHCP_FRAME_OPT_HEADER* pOptHdr;
    TCPIP_DHCP_FRAME_OPTIONS*    pDOpt;
    TCPIP_DHCP_OPTION_WRITE_DATA dhcpWriteData;

    s = dhcpClientSocket;
    if(TCPIP_UDP_PutIsReady(s) < TCPIP_DHCP_MIN_UDP_TX_AVLBL_SIZE)
    {   // not enough room in the buffer (?); shouldn't happen; 
        (void)TCPIP_UDP_Flush(s); // discard old data that may be there
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TX_MSG_ENABLE) != 0
    F_DHCPDebugTxDisplay(pClient, messageType, false);
#endif  // (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TX_MSG_ENABLE) != 0
        return false;
    }

    // bind to the proper interface so message uses correct IP address (request, for example)
    (void)TCPIP_UDP_SocketNetSet(s, pNetIf);
    // allow receiving on any interface
    (void)TCPIP_UDP_OptionsSet(s, UDP_OPTION_STRICT_NET, NULL);

    if((dhcpFlags & (uint32_t)TCPIP_DHCP_FLAG_SEND_BCAST) != 0U || pClient->boundSrvAddress.Val == 0U)
    {   // Ensure that we transmit to the broadcast IP and MAC addresses
        // The UDP Socket remembers who it was last talking to
        (void)TCPIP_UDP_BcastIPV4AddressSet(s, UDP_BCAST_NETWORK_LIMITED, pNetIf);
    }
    else
    {
        (void)TCPIP_UDP_DestinationIPAddressSet(s, IP_ADDRESS_TYPE_IPV4, FC_Ip4Add2Multi(&pClient->boundSrvAddress));
    }

    if((dhcpFlags & (uint32_t)TCPIP_DHCP_FLAG_SEND_ZERO_ADD) != 0U)
    {   // Make sure we advertise a 0.0.0.0 IP address so all DHCP servers will respond.
        // A static IP address outside the DHCP server's scope may simply be ignored.
        sourceIp.Val = 0;
    }    
    else
    {
        sourceIp.Val = TCPIPStackNetAddress(pNetIf);
    }

    (void)TCPIP_UDP_SourceIPAddressSet(s, IP_ADDRESS_TYPE_IPV4, FC_Ip4Add2Multi(&sourceIp));

    newTransaction = (messageType == TCPIP_DHCP_DISCOVER_MESSAGE || messageType == TCPIP_DHCP_REQUEST_RENEW_MESSAGE || (messageType == TCPIP_DHCP_REQUEST_MESSAGE && pClient->dhcpOp == (uint16_t)TCPIP_DHCP_OPER_INIT_REBOOT));
    if (newTransaction)
    {
        if(pClient->flags.bRetry == 0U)
        {   // generate a new transaction ID
            pClient->transactionID = SYS_RANDOM_PseudoGet(); 
        }
        // Reset offered flag so we know to act upon the next valid offer
        pClient->flags.bOfferReceived = 0U;
    }

    pDhcpHdr = &U_FRAME_OPTIONS.dhcpHdr;

    (void)memset(pDhcpHdr, 0, sizeof(*pDhcpHdr));
    pDhcpHdr->op = TCPIP_BOOT_REQUEST;
    pDhcpHdr->htype = TCPIP_BOOT_HW_TYPE;
    pDhcpHdr->hlen = TCPIP_BOOT_LEN_OF_HW_TYPE;
    pDhcpHdr->xid = pClient->transactionID;
    // set ciaddr
    // If this is DHCP REQUEST message, use previously allocated IP address.
    if( messageType == TCPIP_DHCP_RELEASE_MESSAGE || messageType == TCPIP_DHCP_REQUEST_RENEW_MESSAGE) 
    {
        pDhcpHdr->ciaddr = pClient->dhcpIPAddress.Val;
    }
    // set chaddr - Client hardware address.
    (void)memcpy(pDhcpHdr->chaddr, TCPIPStack_NetMACAddressGet(pNetIf), sizeof(TCPIP_MAC_ADDR));
    // write the DHCP header
    (void)TCPIP_UDP_ArrayPut(s, (uint8_t*)pDhcpHdr, (uint16_t)sizeof (*pDhcpHdr));

    // write the DHCP optional header
    pOptHdr = &U_FRAME_OPTIONS.optHdr;
    (void)memset(pOptHdr, 0, sizeof(*pOptHdr));
    (void)TCPIP_UDP_ArrayPut(s, (uint8_t*)pOptHdr, (uint16_t)sizeof(*pOptHdr));

    // set the options themselves
    pDOpt = &U_FRAME_OPTIONS.dhcpOptions;
    (void)memset(pDOpt, 0, sizeof(*pDOpt));

    // set the cookie
    (void)memcpy(pDOpt->cookie, dhcpMagicCookie, sizeof(pDOpt->cookie)); 

    // start writing the options
    dhcpWriteData.pOpt = pDOpt->options;
    dhcpWriteData.writeSpace = (int32_t)TCPIP_UDP_PutIsReady(s) - (int32_t)sizeof(pDOpt->cookie);
    dhcpWriteData.writeBytes = (int32_t)sizeof(pDOpt->cookie);
    dhcpWriteData.msgType = messageType;

    if(F_DHCPOptionsWrite(pNetIf, &dhcpWriteData) != TCPIP_DHCP_OPTION_RES_OK)
    {   // failed
        // clear the Tx buffer, don't send ill formatted packets
        (void)TCPIP_UDP_TxOffsetSet(s , 0, false);
        return false;
    }

    // Add zero padding to ensure compatibility with old BOOTP relays
    // that may discard small packets
    if(sizeof (*pDhcpHdr) + sizeof(*pOptHdr) + (size_t)dhcpWriteData.writeBytes < TCPIP_DHCP_MIN_TX_FRAME_SIZE)
    {
        dhcpWriteData.writeBytes = (int32_t)(TCPIP_DHCP_MIN_TX_FRAME_SIZE - sizeof (*pDhcpHdr) - sizeof(*pOptHdr));
    }

    // write the data and send it
    (void)TCPIP_UDP_ArrayPut(s, pDOpt->cookie, (uint16_t)dhcpWriteData.writeBytes);
    (void)TCPIP_UDP_Flush(s);
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TX_MSG_ENABLE) != 0
    F_DHCPDebugTxDisplay(pClient, messageType, true);
#endif  // (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TX_MSG_ENABLE) != 0

    return true;
}


static TCPIP_DHCP_OPTION_RESULT F_DHCPOptionsWrite(const TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData)
{
    size_t  optIx;
    int     optSize;

    const TCPIP_DHCP_OPTION_WRITE_ENTRY* pEntry = t_DHCPOptWriteTbl;
    for(optIx = 0; optIx < sizeof(t_DHCPOptWriteTbl) / sizeof(*t_DHCPOptWriteTbl); optIx++)
    {
        optSize = (*pEntry->dhcpWriteFnc)(pNetIf, pSendData);
        if(optSize < 0)
        {   // failed
            return TCPIP_DHCP_OPTION_RES_FAIL;
        }

        pSendData->writeSpace -= optSize;
        pSendData->writeBytes += optSize;
        pSendData->pOpt += optSize;
    pEntry++;
    }

    return TCPIP_DHCP_OPTION_RES_OK;
}

static int F_DHCPOptionWriteMsgType(const TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData)
{
    TCPIP_DHCP_OPTION_DATA_MSG_TYPE* pDhcpOptType;

    if(pSendData->writeSpace >= (int32_t)sizeof(*pDhcpOptType))
    {
        pDhcpOptType = pSendData->pMsgType;
        pDhcpOptType->opt = TCPIP_DHCP_MESSAGE_TYPE;
        pDhcpOptType->len = (uint8_t)sizeof(pDhcpOptType->type);
        // TCPIP_DHCP_REQUEST_RENEW_MESSAGE is just an internal message code
        pDhcpOptType->type = (pSendData->msgType == TCPIP_DHCP_REQUEST_RENEW_MESSAGE) ? TCPIP_DHCP_REQUEST_MESSAGE : (uint8_t)pSendData->msgType;
        return (int)sizeof(*pDhcpOptType);
    }
    
    return -1;
}

static int F_DHCPOptionWriteSrvIdent(const TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData)
{
    TCPIP_DHCP_OPTION_DATA_SERVER_ID* pSrvId;
    unsigned int msgType = pSendData->msgType;
    DHCP_CLIENT_VARS* pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf); 
 
    if( (msgType == TCPIP_DHCP_DECLINE_MESSAGE) ||  (msgType == TCPIP_DHCP_RELEASE_MESSAGE) ||
            (msgType == TCPIP_DHCP_REQUEST_MESSAGE && pClient->dhcpOp != (uint16_t)TCPIP_DHCP_OPER_INIT_REBOOT) )
    {   // need to use a server ID
        if(pSendData->writeSpace >= (int32_t)sizeof(*pSrvId))
        {
            pSrvId = pSendData->pServId;
            pSrvId->opt = TCPIP_DHCP_SERVER_IDENTIFIER;
            pSrvId->len = (uint8_t)sizeof(pSrvId->serverId);
            (void)memcpy(pSrvId->serverId, pClient->dwServerID.v, sizeof(pSrvId->serverId));
            return (int)sizeof(*pSrvId);
        }

        // not enough room?
        return -1;
    }

    // no need to write the server ID
    return 0;
}

static int F_DHCPOptionWriteParamRequest(const TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData)
{
    TCPIP_DHCP_OPTION_DATA_REQ_LIST* pReqL;

    size_t nOptions = sizeof(t_DHCPRequestParamsTbl) / sizeof(*t_DHCPRequestParamsTbl);
    size_t reqSize = sizeof(*pReqL) - sizeof(pReqL->optList);

    if(pSendData->writeSpace >= ((int32_t)reqSize + (int32_t)nOptions))
    {
        pReqL = pSendData->pReqL;
        pReqL->opt = TCPIP_DHCP_PARAM_REQUEST_LIST;
        pReqL->len = (uint8_t)nOptions;
        (void)memcpy(pReqL->optList, t_DHCPRequestParamsTbl, nOptions);
        return FC_UI322I32((uint32_t)reqSize + (uint32_t)nOptions);
    }

    // not enough room?
    return -1;
}

static int F_DHCPOptionWriteIPRequest(const TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData)
{
    TCPIP_DHCP_OPTION_DATA_REQ_ADDRESS* pReqAdd;
    unsigned int msgType = pSendData->msgType;
    DHCP_CLIENT_VARS* pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf); 

    if ( (msgType == TCPIP_DHCP_DECLINE_MESSAGE) || (msgType == TCPIP_DHCP_REQUEST_MESSAGE) ||  
            ((msgType == TCPIP_DHCP_DISCOVER_MESSAGE) && pClient->dhcpIPAddress.Val != 0U))
    {   // Add requested IP address to DHCP Request Message
        if(pSendData->writeSpace >= (int32_t)sizeof(*pReqAdd))
        {
            pReqAdd = pSendData->pReqAdd;
            pReqAdd->opt = TCPIP_DHCP_PARAM_REQUEST_IP_ADDRESS;
            pReqAdd->len = (uint8_t)sizeof(pReqAdd->reqIpAddr);
            (void)memcpy(pReqAdd->reqIpAddr, pClient->dhcpIPAddress.v, sizeof(pReqAdd->reqIpAddr));
            return (int)sizeof(*pReqAdd);
        }

        // not enough room?
        return -1;
    }

    // no need to request the IP address
    return 0;
}

static int F_DHCPOptionHostName(const TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData)
{
    size_t  hostLen;
    TCPIP_DHCP_OPTION_DATA_HOST_NAME* pHost;
    DHCP_CLIENT_VARS    *pClient;
    char*   cbName;
    unsigned char hostName[TCPIP_DHCP_HOST_NAME_SIZE + 1];
    size_t  writeBackLen = 0;
    unsigned char*   srcName = NULL;

    // get a host name
    pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf);

    if(pClient->nameCallback != NULL)
    {
        if((cbName = (*pClient->nameCallback)(pNetIf)) != NULL)
        {
            if(strlen(cbName) >= 2U)
            {   // valid string
                srcName = (unsigned char*)cbName;
                if(pClient->flags.bWriteBack != 0U)
                {
                    writeBackLen = strlen(cbName) + 1U;
                }
            }
        }
    }

    if(srcName == NULL)
    {   // use the default NBNS name
        srcName = (unsigned char*)TCPIPStack_NetBIOSName(FC_CNetIf2NetIf(pNetIf));
    }

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAKE_HOST_NAME) != 0
    // override
    if(dhcpDbgHostName != NULL)
    {
        srcName = dhcpDbgHostName;
    }
#endif  // TCPIP_DHCP_DEBUG_MASK

    hostLen = F_DHCPFormatHostName(hostName, srcName, sizeof(hostName));

    if(writeBackLen != 0U)
    {
        srcName[0] = (uint8_t)'\0';
    }

    if(hostLen == 0U)
    {   // no valid host name; skip option
        return 0;
    }

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAKE_HOST_NAME) != 0
    SYS_CONSOLE_PRINT("DHCP cli: %d, host format Name: %s\r\n", pClient - DHCPClients, (char*)hostName);
#endif  // TCPIP_DHCP_DEBUG_MASK

    size_t hostSz = sizeof(*pHost) - sizeof(pHost->hostname);
    if(pSendData->writeSpace >= ((int32_t)sizeof(*pHost) + (int32_t)hostLen))
    {
        pHost = pSendData->pHost;
        pHost->opt = TCPIP_DHCP_HOST_NAME;
        pHost->len = (uint8_t)hostLen;
        (void)memcpy((void*)pHost->hostname, (void*)hostName, hostLen);

        if(writeBackLen != 0U)
        {
            if(writeBackLen > hostLen + 1U)
            {
                writeBackLen = hostLen + 1U;
            }
            (void)memcpy(srcName, hostName, writeBackLen);
            srcName[writeBackLen - 1U] = (uint8_t)'\0';
        }

        return FC_UI322I32((uint32_t)hostSz + (uint32_t)hostLen);
    }

    // not enough room?
    return -1;
}

/* MISRA C-2012 Rule 10.1 deviated:2 Deviation record ID -  H3_MISRAC_2012_R_10_1_NET_DR_11 */
/* MISRA C-2012 Rule 10.2 deviated:1 Deviation record ID -  H3_MISRAC_2012_R_10_2_NET_DR_12 */
/* MISRA C-2012 Rule 10.4 deviated:4 Deviation record ID -  H3_MISRAC_2012_R_10_4_NET_DR_13 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:1 "MISRA C-2012 Rule 10.1" "H3_MISRAC_2012_R_10_1_NET_DR_11" 
#pragma coverity compliance block deviate:1 "MISRA C-2012 Rule 10.2" "H3_MISRAC_2012_R_10_2_NET_DR_12" 
#pragma coverity compliance block deviate:4 "MISRA C-2012 Rule 10.4" "H3_MISRAC_2012_R_10_4_NET_DR_13" 
static size_t F_DHCPFormatHostName(unsigned char* destBuffer, const unsigned char* srcBuffer, size_t destSize)
{
    size_t  ix, len;
    unsigned char*   dstPtr;
    unsigned char    currC;

    (void)memset(destBuffer, 0x0, destSize);

    // strip spaces
    dstPtr = destBuffer;
    for(ix = 0U; ix < destSize - 1U; ix++)
    {
        currC = *srcBuffer++;
        if(isspace((int)currC) == 0)
        {
            *dstPtr++ = currC;
        }
    }

    len = strlen((char*)destBuffer);

    if(len != 0U)
    {
        dstPtr = destBuffer;
        for(ix = 0U; ix < len; ix++)
        {
            currC = *dstPtr;
            if(currC != (unsigned char)'-' && currC != (unsigned char)'.' && isalnum((int)currC) == 0)
            {
                *dstPtr = TCPIP_DHCP_HOST_REPLACE_CHAR;
            }
            dstPtr++;
        }

        // finally check the 1st and last chars
        currC = destBuffer[0U];
        if(isalpha((int)currC) == 0)
        {
            destBuffer[0U] = TCPIP_DHCP_HOST_REPLACE_CHAR;
        }
        currC = destBuffer[len - 1U];
        if(isalnum((int)currC) == 0)
        {
            destBuffer[len - 1U] = TCPIP_DHCP_HOST_REPLACE_CHAR;
        }
    }

    return len;
}
#pragma coverity compliance end_block "MISRA C-2012 Rule 10.1"
#pragma coverity compliance end_block "MISRA C-2012 Rule 10.2"
#pragma coverity compliance end_block "MISRA C-2012 Rule 10.4"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */

                
static int F_DHCPOptionClientId(const TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData)
{
    TCPIP_DHCP_OPTION_DATA_CLIENT_ID* pClientOpt;

    if(pSendData->writeSpace >= (int32_t)sizeof(*pClientOpt))
    {
        pClientOpt = pSendData->pClientId;
        pClientOpt->opt = TCPIP_DHCP_PARAM_REQUEST_CLIENT_ID;
        pClientOpt->len = (uint8_t)sizeof(pClientOpt->cliId);
#if defined(TCPIP_STACK_ALIAS_INTERFACE_SUPPORT) && (TCPIP_STACK_ALIAS_INTERFACE_SUPPORT != 0)
        pClientOpt->cliId.type = 0;    // using an extended hardware address
        (void)memcpy(pClientOpt->cliId.id, TCPIPStack_NetMACAddressGet(pNetIf), sizeof(pClientOpt->cliId.id) - 2U);
        uint16_t netIx = (uint16_t)TCPIP_STACK_NetIxGet(pNetIf);
        pClientOpt->cliId.id[sizeof(pClientOpt->cliId.id) - 2U] = (uint8_t)(netIx >> 8);
        pClientOpt->cliId.id[sizeof(pClientOpt->cliId.id) - 1U] = (uint8_t)netIx;
#else
        pClientOpt->cliId.type = TCPIP_BOOT_HW_TYPE;    // standard hardware address
        (void)memcpy(pClientOpt->cliId.id, TCPIPStack_NetMACAddressGet(pNetIf), sizeof(pClientOpt->cliId.id));
#endif  // defined(TCPIP_STACK_ALIAS_INTERFACE_SUPPORT) && (TCPIP_STACK_ALIAS_INTERFACE_SUPPORT != 0)
        return (int)sizeof(*pClientOpt);
    }

    // not enough room?
    return -1;

}

                      
static int F_DHCPOptionWriteEnd(const TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData)
{
    TCPIP_DHCP_OPTION_DATA_END* pEnd;
    if(pSendData->writeSpace >= (int32_t)sizeof(*pEnd))
    {
        pEnd = pSendData->pEnd;
        pEnd->opt = TCPIP_DHCP_END_OPTION;
        return (int)sizeof(*pEnd);
    }

    return -1;
}


// DHCP connection handler
//
void TCPIP_DHCP_ConnectionHandler(TCPIP_NET_IF* pNetIf, TCPIP_MAC_EVENT connEvent)
{
    DHCP_CLIENT_VARS* pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf);

    if (pClient->flags.bDHCPEnabled != 0U)
    {
        if(((uint16_t)connEvent & (uint16_t)TCPIP_MAC_EV_CONN_LOST) != 0U)
        {
            // let it wait for the connection
            F_DHCPClientClose(pNetIf, false, false);
            TCPIPStackSetConfigAddress(pNetIf, NULL, NULL, NULL, true);
            TCPIP_STACK_AddressServiceEvent(pNetIf, TCPIP_STACK_ADDR_SRVC_DHCPC, TCPIP_ADDR_SRVC_EV_CONN_LOST);
            F_DHCPDbgAddServiceEvent(pClient, TCPIP_ADDR_SRVC_EV_CONN_LOST, NULL);
            F_DHCPNotifyClients(pNetIf, DHCP_EVENT_CONN_LOST, 0, 0);
        }
        else if(((uint16_t)connEvent & (uint16_t)TCPIP_MAC_EV_CONN_ESTABLISHED) != 0U)
        {
            // put it in wait connection mode
            // should be in this state anyway
            // but just in case we've missed the link down event
            F_DHCPEnable(pNetIf, pClient->flags.bWasBound != 0U ? TCPIP_DHCP_OPER_INIT_REBOOT : TCPIP_DHCP_OPER_INIT);
            F_DHCPNotifyClients(pNetIf, DHCP_EVENT_CONN_ESTABLISHED, 0, 0);
        }
        else
        {
            // do nothing
        }
    }

}



// Register an DHCP event handler
// Use hNet == 0 to register on all interfaces available
// Returns a valid handle if the call succeeds,
// or a null handle if the call failed.
// Function has to be called after the DHCP is initialized
// The hParam is passed by the client and will be used by the DHCP when the notification is made.
// It is used for per-thread content or if more modules, for example, share the same handler
// and need a way to differentiate the callback.
TCPIP_DHCP_HANDLE TCPIP_DHCP_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_DHCP_EVENT_HANDLER handler, const void* hParam)
{
    if(handler != NULL && dhcpMemH != NULL)
    {
        TCPIP_DHCP_LIST_EV_NODE dhcpNode;
        dhcpNode.next = NULL;
        dhcpNode.handler = handler;
        dhcpNode.xhandler = NULL;
        dhcpNode.hParam = hParam;
        dhcpNode.hNet = hNet;

        return (TCPIP_DHCP_HANDLE)TCPIP_Notification_Add(&dhcpRegisteredUsers, dhcpMemH, &dhcpNode, sizeof(dhcpNode));
    }

    return NULL;
}

TCPIP_DHCP_HANDLE TCPIP_DHCP_HandlerRegisterEx(TCPIP_NET_HANDLE hNet, TCPIP_DHCP_EVENT_HANDLER_EX exHandler, const void* hParam)
{
    if(exHandler != NULL && dhcpMemH != NULL)
    {
        TCPIP_DHCP_LIST_EV_NODE dhcpNode;
        dhcpNode.next = NULL;
        dhcpNode.handler = NULL;
        dhcpNode.xhandler = exHandler;
        dhcpNode.hParam = hParam;
        dhcpNode.hNet = hNet;

        return (TCPIP_DHCP_HANDLE)TCPIP_Notification_Add(&dhcpRegisteredUsers, dhcpMemH, &dhcpNode, sizeof(dhcpNode));
    }

    return NULL;
}

// deregister the event handler
bool TCPIP_DHCP_HandlerDeRegister(TCPIP_DHCP_HANDLE hDhcp)
{
    if(hDhcp != NULL && dhcpMemH != NULL)
    {
        SGL_LIST_NODE* dhcpNode = FC_DhcpH2SLNode(hDhcp);
        if(TCPIP_Notification_Remove(dhcpNode, &dhcpRegisteredUsers, dhcpMemH))
        {
            return true;
        }
    }

    return false;
}

static void F_DHCPSetEventInfo(const TCPIP_NET_IF* pNetIf, TCPIP_DHCP_EVENT_TYPE evType, uint32_t clientAdd, uint32_t serverAdd, TCPIP_DHCP_EVENT_INFO* pEvInfo)
{
    DHCP_CLIENT_VARS* pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf);

    pEvInfo->dhcpTimeMs = TCPIP_MsecCountGet();

    TCPIP_DHCP_EV_SOURCE evSrc = F_DHCPEventSource(evType);

    if(evSrc == TCPIP_DHCP_EV_SRC_SERVER || evSrc == TCPIP_DHCP_EV_SRC_CLIENT)
    {
        pEvInfo->transactionId = pClient->transactionID;
    }

    if(evSrc == TCPIP_DHCP_EV_SRC_SERVER)
    {
        pEvInfo->clientAddress.Val = clientAdd;
        pEvInfo->serverAddress.Val = serverAdd;
    }
    else
    {   // client initiated or connection event
        pEvInfo->clientAddress.Val = pClient->dhcpIPAddress.Val;
        if(F_DHCPEventUseBcast(evType) || pClient->boundSrvAddress.Val == 0U)
        {
            pEvInfo->serverAddress.Val = 0xffffffffU;  // BCAST_NETWORK_LIMITED; 
        } 
        else
        {
            pEvInfo->serverAddress.Val = pClient->boundSrvAddress.Val;
        }
    }
}

static void F_DHCPNotifyClients(const TCPIP_NET_IF* pNetIf, TCPIP_DHCP_EVENT_TYPE evType, uint32_t clientAdd, uint32_t serverAdd)
{
    TCPIP_DHCP_LIST_EV_NODE* dNode;
    TCPIP_DHCP_EVENT_INFO   evInfo;

    (void)memset(&evInfo, 0x0, sizeof(evInfo));

    TCPIP_Notification_Lock(&dhcpRegisteredUsers);
    for(dNode = FC_SLNode2DhcpNode(dhcpRegisteredUsers.list.head); dNode != NULL; dNode = dNode->next)
    {
        if(dNode->hNet == NULL || dNode->hNet == pNetIf)
        {   // trigger event
            if(dNode->handler != NULL)
            {   // simple event
                dNode->handler(pNetIf, evType, dNode->hParam);
            }
            else if(dNode->xhandler != NULL)
            {   // extended event
                if(evInfo.dhcpTimeMs == 0U)
                {
                    F_DHCPSetEventInfo(pNetIf, evType, clientAdd, serverAdd, &evInfo);
                }

                dNode->xhandler(pNetIf, evType, &evInfo, dNode->hParam);
            }
            else
            {
                TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
                // do nothing
            }
        }
    }
    TCPIP_Notification_Unlock(&dhcpRegisteredUsers);

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_NOTIFY_EVENTS) != 0
    static const char* event_name_tbl[] = 
    {
        "none",     // DHCP_EVENT_NONE
        "dcv",      // DHCP_EVENT_DISCOVER
        "offer",    // DHCP_EVENT_OFFER
        "req",      // DHCP_EVENT_REQUEST
        "ack",      // DHCP_EVENT_ACK
        "dcl",      // DHCP_EVENT_DECLINE
        "nak",      // DHCP_EVENT_NACK
        "tmo",      // DHCP_EVENT_TIMEOUT
        "bnd",      // DHCP_EVENT_BOUND
        "ren",      // DHCP_EVENT_REQUEST_RENEW
        "rbn",      // DHCP_EVENT_REQUEST_REBIND
        "unk",      // DHCP_EVENT_UNKNOWN
        "lost",     // DHCP_EVENT_CONN_LOST
        "est",      // DHCP_EVENT_CONN_ESTABLISHED
        "off",      // DHCP_EVENT_SERVICE_DISABLED
    };
    char cliBuff[20], srvBuff[20];

    const char* message;
    if((int32_t)evType < 0 || (int32_t)evType >= (int32_t)(sizeof(event_name_tbl) / sizeof(*event_name_tbl)))
    {
        TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
        message = "illegal";
    }
    else
    {
        message = event_name_tbl[evType];
    }

    cliBuff[0] = '\0';
    srvBuff[0] = '\0';
    if(evInfo.dhcpTimeMs == 0U)
    {
        F_DHCPSetEventInfo(pNetIf, evType, clientAdd, serverAdd, &evInfo);
    }
    (void)TCPIP_Helper_IPAddressToString(&evInfo.clientAddress, cliBuff, sizeof(cliBuff));
    (void)TCPIP_Helper_IPAddressToString(&evInfo.serverAddress, srvBuff, sizeof(srvBuff));
    SYS_CONSOLE_PRINT("DHCP cli: %d event: %s, msec: %d, id: 0x%08x\r\n", TCPIP_STACK_NetIndexGet(pNetIf), message, evInfo.dhcpTimeMs, evInfo.transactionId);
    SYS_CONSOLE_PRINT("\tDHCP ev client address: %s, server address: %s\r\n", cliBuff, srvBuff);
#endif  // (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_NOTIFY_EVENTS) != 0

}


// enable unicast DHCP packets
// this should be an IPv4 packet
static bool F_DHCPPacketFilter(TCPIP_MAC_PACKET* pRxPkt, uint8_t hdrlen)
{
    TCPIP_MAC_ADDR bcastAdd = { {0xff, 0xff, 0xff, 0xff, 0xff, 0xff} };

    TCPIP_MAC_ETHERNET_HEADER* macHdr = FC_Uptr2MacEthHdr(pRxPkt->pMacLayer);
    const uint8_t* netMacAddr = TCPIP_STACK_NetUpMACAddressGet((const TCPIP_NET_IF*)pRxPkt->pktIf);

    if(netMacAddr != NULL)
    {
        if(memcmp(netMacAddr, macHdr->DestMACAddr.v, sizeof(macHdr->DestMACAddr)) == 0 || 
           memcmp(netMacAddr, bcastAdd.v, sizeof(bcastAdd)) == 0 )
        {   // unicast to me;
            // allow also broadcast because some DHCP servers seem to ignore the unicast request
            IPV4_HEADER* pHeader = FC_Uptr2IpHdr(pRxPkt->pNetLayer);
            if(pHeader->Protocol == (uint8_t)IP_PROT_UDP)
            {   // UDP packet
                UDP_HEADER* pUDPHdr = FC_U8Ptr2UdpHdr(pRxPkt->pNetLayer + hdrlen);
                UDP_PORT destPort = TCPIP_Helper_ntohs(pUDPHdr->DestinationPort);
                UDP_PORT srcPort = TCPIP_Helper_ntohs(pUDPHdr->SourcePort);
                if(destPort == dhcpClientPort && srcPort == dhcpServerPort)
                {   // valid DHCP packet
                    return true;
                }
            }
        }
    }

    return false;
}


static UDP_SOCKET F_DHCPOpenSocket(void)
{
    uint16_t txBuffSize;
    TCPIP_UDP_SIGNAL_HANDLE sigHandle;

    UDP_SOCKET dhcpSkt = TCPIP_UDP_OpenClientSkt(IP_ADDRESS_TYPE_IPV4, dhcpServerPort, NULL, (UDP_OPEN_TYPE)((uint16_t)UDP_OPEN_CLIENT | (uint16_t)UDP_OPEN_CONFIG_SERVICE));

    bool udpSuccess = false;
    while(dhcpSkt != INVALID_UDP_SOCKET)
    {   
        // make sure the socket is created with enough TX space
        (void)TCPIP_UDP_OptionsGet(dhcpSkt, UDP_OPTION_TX_BUFF, (void*)&txBuffSize);
        if(txBuffSize < TCPIP_DHCP_MIN_UDP_TX_BUFFER_SIZE)
        {
            txBuffSize = TCPIP_DHCP_MIN_UDP_TX_BUFFER_SIZE;
            if(!TCPIP_UDP_OptionsSet(dhcpSkt, UDP_OPTION_TX_BUFF, FC_Uint2VPtr((uint32_t)txBuffSize)))
            {
                break;
            }
        }

        // bind to the DHCP local port
        if(!TCPIP_UDP_Bind(dhcpSkt, IP_ADDRESS_TYPE_IPV4, dhcpClientPort, NULL))
        {
            break;
        }

        // allow receiving from any server on this interface
        (void)TCPIP_UDP_OptionsSet(dhcpSkt, UDP_OPTION_STRICT_ADDRESS, NULL);

        sigHandle = TCPIP_UDP_SignalHandlerRegister(dhcpSkt, TCPIP_UDP_SIGNAL_RX_DATA, &F_DHCPSocketRxSignalHandler, NULL);
        if(sigHandle == NULL)
        {
            break;
        }

        udpSuccess = true;
        break;
    }



    if(!udpSuccess && dhcpSkt != INVALID_UDP_SOCKET)
    {
        (void)TCPIP_UDP_Close(dhcpSkt);
        dhcpSkt = INVALID_UDP_SOCKET;
    }

    return dhcpSkt;
}

bool TCPIP_DHCP_InfoGet(TCPIP_NET_HANDLE hNet, TCPIP_DHCP_INFO* pDhcpInfo)
{
    if(pDhcpInfo != NULL)
    {
        (void)memset(pDhcpInfo, 0, sizeof(*pDhcpInfo));
    }

    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);
    if(DHCPClients != NULL && pNetIf != NULL)
    {
        DHCP_CLIENT_VARS* pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf);

        if(pClient->flags.bDHCPEnabled == 1U && pClient->smState >= (uint16_t)TCPIP_DHCP_BOUND)
        {
            if(pDhcpInfo != NULL)
            {
                pDhcpInfo->status = (TCPIP_DHCP_STATUS)pClient->smState;
                pDhcpInfo->dhcpTime = TCPIP_SecCountGet();
                pDhcpInfo->leaseStartTime = pClient->tRequest;
                pDhcpInfo->leaseDuration = pClient->tExpSeconds;
                pDhcpInfo->renewTime = pClient->tRequest + pClient->t1Seconds;
                pDhcpInfo->rebindTime = pClient->tRequest + pClient->t2Seconds;
                pDhcpInfo->dhcpAddress.Val = pClient->dhcpIPAddress.Val;
                pDhcpInfo->subnetMask.Val = pClient->dhcpMask.Val;
                pDhcpInfo->serverAddress.Val = pClient->boundSrvAddress.Val;
#if defined TCPIP_DHCP_STORE_BOOT_FILE_NAME
                pDhcpInfo->bootFileName = (const char*)pClient->bootFileName;
#else
                pDhcpInfo->bootFileName = NULL;
#if (TCPIP_DHCP_USE_OPTION_TIME_SERVER != 0)
                if(pClient->validValues.tServer != 0U)
                {
                    pDhcpInfo->timeServersNo = pClient->tServerNo;
                    pDhcpInfo->timeServers = pClient->timeServers;
                }
#endif  // (TCPIP_DHCP_USE_OPTION_TIME_SERVER != 0)
#if (TCPIP_DHCP_USE_OPTION_NTP_SERVER != 0)
                if(pClient->validValues.ntpServer != 0U)
                {
                    pDhcpInfo->ntpServersNo = pClient->ntpServerNo;
                    pDhcpInfo->ntpServers = pClient->ntpServers;
                }
#endif  // (TCPIP_DHCP_USE_OPTION_NTP_SERVER != 0)

#endif // defined TCPIP_DHCP_STORE_BOOT_FILE_NAME
            }

            return true;
        }
    }

    return false;

}

// checks and reports loss of lease
static void F_DHCPCheckRunFailEvent(const TCPIP_NET_IF* pNetIf, DHCP_CLIENT_VARS* pClient)
{

    if(pClient->smState > (uint16_t)TCPIP_DHCP_WAIT_LINK && pClient->smState < (uint16_t)TCPIP_DHCP_BOUND)
    {   // don't have a current lease
        if(pClient->flags.bReportFail != 0U && pClient->tOpStart != 0U)
        {
            if((TCPIP_SecCountGet() - pClient->tOpStart) >= pClient->tOpFailTmo)
            {   // initialization time out
                TCPIP_STACK_AddressServiceEvent(FC_CNetIf2NetIf(pNetIf), TCPIP_STACK_ADDR_SRVC_DHCPC, TCPIP_ADDR_SRVC_EV_RUN_FAIL);
                F_DHCPDbgAddServiceEvent(pClient, TCPIP_ADDR_SRVC_EV_RUN_FAIL, "init tmo");
                pClient->flags.bReportFail = 0U;   // reported
            }
        }
    }
}


bool TCPIP_DHCP_HostNameCallbackRegister(TCPIP_NET_HANDLE hNet, TCPIP_DHCP_HOST_NAME_CALLBACK nameCallback, bool writeBack)
{
    size_t startIx, stopIx, ix;
    TCPIP_NET_IF        *pNetIf;
    DHCP_CLIENT_VARS    *pClient;

    if(DHCPClients == NULL)
    {   // not initialized?
        return false;
    }

    if(hNet == NULL)
    {
        startIx = 0;
        stopIx = dhcpInterfaces;
    }
    else if((pNetIf = TCPIPStackHandleToNet(hNet)) == NULL)
    {   // invalid interface
        return false;
    }
    else
    {
        startIx = (size_t)TCPIP_STACK_NetIxGet(pNetIf);
        stopIx = startIx + 1U;
    }


    pClient = DHCPClients + startIx;
    for(ix = startIx; ix < stopIx; ix++)
    {
        pClient->nameCallback = nameCallback;
        pClient->flags.bWriteBack = writeBack == true ? 1U : 0U;
        pClient++;
    }

    return true;

}



#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAKE_TMO) != 0
static void F_DHCPDbgProcessTmo(DHCP_CLIENT_VARS* pClient)
{
    if(TCPIP_UDP_GetIsReady(dhcpClientSocket) == 0U)
    {   // no data anyway
        return;
    }

    bool discardPkt = false;

    switch(pClient->smState)
    {
        case (uint16_t)TCPIP_DHCP_GET_OFFER:
            if(dhcpDbgGetOfferTmoCnt != 0U)
            {
                dhcpDbgGetOfferTmoCnt--;
                discardPkt = true;
            }
            break;

        case (uint16_t)TCPIP_DHCP_GET_REQUEST_ACK:
            if(dhcpDbgGetRequestTmoCnt != 0U)
            {
                dhcpDbgGetRequestTmoCnt--;
                discardPkt = true;
            }
            break;

        case (uint16_t)TCPIP_DHCP_GET_RENEW_ACK:
            if(dhcpDbgGetRenewTmoCnt != 0U)
            {
                dhcpDbgGetRenewTmoCnt--;
                discardPkt = true;
            }
            break;

        case (uint16_t)TCPIP_DHCP_GET_REBIND_ACK:
            if(dhcpDbgGetRebindTmoCnt != 0U)
            {
                dhcpDbgGetRebindTmoCnt--;
                discardPkt = true;
            }
            break;

        default:
            // do nothing
            break;
    }

    if(discardPkt)
    {
        do
        {
            (void)TCPIP_UDP_Discard(dhcpClientSocket);
        }while(TCPIP_UDP_GetIsReady(dhcpClientSocket) != 0U);
    }
}
#endif  // TCPIP_DHCP_DEBUG_MASK



#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_LEASE_DECLINE) != 0

static bool F_DHCPDbgProcessAckDecline(DHCP_CLIENT_VARS* pClient, const TCPIP_NET_IF* pNetIf)
{
    bool decline = false;

    switch(pClient->smState)
    {
        case (uint16_t)TCPIP_DHCP_GET_REQUEST_ACK:
            if(dhcpDbgLeaseRequestDeclineCnt != 0U)
            {
                dhcpDbgLeaseRequestDeclineCnt--;
                decline = true;
            }
            break;

        case (uint16_t)TCPIP_DHCP_GET_RENEW_ACK:
            if(dhcpDbgLeaseRenewDeclineCnt != 0U)
            {
                dhcpDbgLeaseRenewDeclineCnt--;
                decline = true;
            }
            break;

        case (uint16_t)TCPIP_DHCP_GET_REBIND_ACK:
            if(dhcpDbgLeaseRebindDeclineCnt != 0U)
            {
                dhcpDbgLeaseRebindDeclineCnt--;
                decline = true;
            }
            break;

        default:
            // do nothing
            break;
    }

    if(decline)
    {
        (void)F_DHCPSend(pClient, pNetIf, TCPIP_DHCP_DECLINE_MESSAGE, (uint32_t)TCPIP_DHCP_FLAG_SEND_ZERO_ADD | (uint32_t)TCPIP_DHCP_FLAG_SEND_BCAST);
    }

    return decline;
}

#endif  // TCPIP_DHCP_DEBUG_MASK

