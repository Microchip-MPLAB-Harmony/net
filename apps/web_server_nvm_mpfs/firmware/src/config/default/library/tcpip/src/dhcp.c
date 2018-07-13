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

/*******************************************************************************
File Name:  DHCP.c
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
#include <ctype.h>

#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_DHCP_CLIENT

#include "tcpip/src/tcpip_private.h"
#include "tcpip/src/dhcp_private.h"

#if defined(TCPIP_STACK_USE_IPV4)
#if !defined(TCPIP_STACK_USE_DHCP_CLIENT)

// provide basic/common functions for DHCP access when disabled
bool TCPIP_DHCP_IsEnabled(TCPIP_NET_HANDLE hNet)
{
    return false;
}

bool TCPIP_DHCP_IsActive(TCPIP_NET_HANDLE hNet)
{
    return false;
}

bool TCPIP_DHCP_Disable(TCPIP_NET_HANDLE hNet)
{
    return false;
}

bool TCPIP_DHCP_Enable(TCPIP_NET_HANDLE hNet)
{
    return false;
}

bool TCPIP_DHCP_Renew(TCPIP_NET_HANDLE hNet)
{
    return false;
}


bool TCPIP_DHCP_Request(TCPIP_NET_HANDLE hNet, IPV4_ADDR reqAddress)
{
    return false;
}

bool TCPIP_DHCP_InfoGet(TCPIP_NET_HANDLE hNet, TCPIP_DHCP_INFO* pDhcpInfo)
{
    return false;
}

#else   // defined(TCPIP_STACK_USE_DHCP_CLIENT)


// Unique variables per interface
typedef struct
{
    uint32_t                waitTicks;      // # of ticks to wait for the current DHCP operation
    uint32_t                startWait;      // the time the wait is starting; could be seconds or ticks,
                                            // operation dependent                
                                            // wait operations are implemented as difference between the 
                                            // current tick/second and startWait to avoid counter overflows
	uint32_t			    tRequest;		// the time when the DHCP request was sent - seconds
	uint32_t			    tRenewRequest;  // the time when the DHCP renew request was sent - seconds
    uint32_t                t1Seconds;      // # of seconds to wait until we get to RENEW state (T1 timer)
    uint32_t                t2Seconds;      // # of seconds to wait until we get to REBIND state (T2 timer)
    uint32_t                tExpSeconds;    // # of seconds to wait until the lease completely expired
                                            // (lease Time, Texp)    
                                            // T1 < T2 < Texp
    uint32_t                t3Seconds;      // # of seconds to wait until reissuing a REQUEST in RENEW/REBIND state
    uint32_t                tOpStart;       // time at which a lease operation is started
	uint32_t				dwServerID;		// DHCP Server ID cache
	IPV4_ADDR				serverAddress;	// DHCP Server that grant the lease
	IPV4_ADDR				dhcpIPAddress;	// DHCP obtained IP address
	IPV4_ADDR				dhcpGateway;	// DHCP obtained gateway
	IPV4_ADDR				dhcpMask;		// DHCP obtained mask
	#if defined(TCPIP_STACK_USE_DNS)
	IPV4_ADDR				dhcpDNS;		// DHCP obtained primary DNS server
	IPV4_ADDR				dhcpDNS2;		// DHCP obtained secondary DNS server
	#endif	
    TCPIP_UINT32_VAL        transactionID;  // current transaction ID
    // unaligned data
    uint16_t                dhcpTmo;        // current DHCP timeout        
    uint16_t                dhcpTmoBase;    // DHCP base timeout
    uint16_t                tOpFailTmo;     // operation failure timeout: initialization, etc.
    uint16_t                tLeaseCheck;    // time to wait for a lease check
    uint16_t                dhcpOp;         // DHCP current operation: TCPIP_DHCP_OPERATION_TYPE
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_STATUS) != 0
	uint8_t		            smState;		// DHCP client state machine variable: TCPIP_DHCP_STATUS
	uint8_t		            prevState;		// DHCP client previous state machine variable: TCPIP_DHCP_STATUS
#else
	uint16_t		        smState;		// DHCP client state machine variable: TCPIP_DHCP_STATUS
#endif  // TCPIP_DHCP_DEBUG_MASK
    TCPIP_DHCP_HOST_NAME_CALLBACK   nameCallback;   // get host name callback
	union
	{
	    struct
	    {
	        uint8_t bDHCPEnabled        : 1;	// Whether or not DHCP is currently enabled
	        uint8_t bIsBound            : 1;	// Whether or not DHCP is currently bound
	        uint8_t bOfferReceived      : 1;	// Whether or not an offer has been received
			uint8_t bDHCPServerDetected : 1;	// Indicates if a DCHP server has been detected
            uint8_t bWasBound           : 1;    // successfully held a lease
            uint8_t bReportFail         : 1;    // report run time failure flag
            uint8_t bWriteBack          : 1;    // write back the resulting host name
            uint8_t reserved            : 1;    // not used
	    };
	    uint8_t val;
	} flags;
	// Indicates which DHCP values are currently valid
	union
	{
		struct
		{
			uint8_t IPAddress      : 1;     // Leased IP address is valid
			uint8_t Gateway        : 1;	    // Gateway address is valid
			uint8_t Mask           : 1;	    // Subnet mask is valid
			uint8_t DNS            : 1;	    // Primary DNS is valid
			uint8_t DNS2           : 1;	    // Secondary DNS is valid
			uint8_t reserved       : 3;	    // unused
		};
		uint8_t val;
	} validValues;

#if defined TCPIP_DHCP_STORE_BOOT_FILE_NAME
    uint8_t     bootFileName[TCPIP_DHCP_BOOT_FILE_NAME_SIZE];
#endif // defined TCPIP_DHCP_STORE_BOOT_FILE_NAME

} DHCP_CLIENT_VARS;

static void     TCPIP_DHCP_Process(bool isTmo);

static void     _DHCPSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param);
static unsigned int  _DHCPReceive(DHCP_CLIENT_VARS* pClient, TCPIP_NET_IF* pNetIf);
static bool     _DHCPSend(DHCP_CLIENT_VARS* pClient, TCPIP_NET_IF* pNetIf, int messageType, TCPIP_DHCP_OPERATION_FLAGS dhcpFlags);
static void     _DHCPNotifyClients(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_EVENT_TYPE evType);
static bool     _DHCPStartOperation(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPERATION_REQ opReq, uint32_t reqAddress);

static bool     _DHCPPacketFilter(TCPIP_MAC_PACKET* pRxPkt, const void* param);
static UDP_SOCKET _DHCPOpenSocket(void);

static void     _DHCPSetRunFail(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_STATUS newState, bool expBackoff);
static void     _DHCPSetTimeout(DHCP_CLIENT_VARS* pClient);
static void     _DHCPSetLeaseTimeParams(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pDhcpData);
static void     _DHCPSetNewLease(DHCP_CLIENT_VARS* pClient, TCPIP_NET_IF* pNetIf);
static unsigned int     _DHCPProcessReceiveData(DHCP_CLIENT_VARS* pClient, TCPIP_NET_IF* pNetIf);
static void     _DHCPCheckRunFailEvent(TCPIP_NET_IF* pNetIf, DHCP_CLIENT_VARS* pClient);
static void     _DHCPSetBoundState(DHCP_CLIENT_VARS* pClient);

static TCPIP_DHCP_OPTION_RESULT _DHCPOptionProcess(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);
static int                      _DHCPOptionProcessMsgType(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);
static int                      _DHCPOptionProcessSubNet(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);
static int                      _DHCPOptionProcessRouter(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);
static int                      _DHCPOptionProcessDns(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);
static int                      _DHCPOptionProcessSrvIdent(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);
static int                      _DHCPOptionProcessLease(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);
static int                      _DHCPOptionProcessRenewT(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);
static int                      _DHCPOptionProcessRebindT(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);

static int                      _DHCPOptionProcessEnd(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);


static TCPIP_DHCP_OPTION_RESULT _DHCPOptionsWrite(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pOptData);
static int                      _DHCPOptionWriteMsgType(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData);
static int                      _DHCPOptionWriteSrvIdent(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData);
static int                      _DHCPOptionWriteParamRequest(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData);
static int                      _DHCPOptionWriteIPRequest(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData);
static int                      _DHCPOptionHostName(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData);
static int                      _DHCPOptionClientId(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData);
static int                      _DHCPOptionWriteEnd(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData);

static int                      _DHCPFormatHostName(char* destBuffer, const char* srcBuffer, int destSize);


#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAKE_TMO) != 0
static void     _DHCPDbgProcessTmo(DHCP_CLIENT_VARS* pClient);
#endif

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_LEASE_DECLINE) != 0
static bool     _DHCPDbgProcessAckDecline(DHCP_CLIENT_VARS* pClient, TCPIP_NET_IF* pNetIf);
#endif  // TCPIP_DHCP_DEBUG_MASK

static DHCP_CLIENT_VARS*	DHCPClients = 0;    // DHCP client per interface 

static int                  dhcpInterfaces = 0; // number of interfaces present
static int                  dhcpInitCount = 0;  // DHCP module initialization count

static UDP_SOCKET    	    dhcpClientSocket;   // Handle to DHCP client socket
static UDP_PORT             dhcpClientPort;
static UDP_PORT             dhcpServerPort;

static tcpipSignalHandle     dhcpSignalHandle = 0;

static IPV4_FILTER_HANDLE   dhcpFilterHandle = 0;       // IPv4 packet filter

static const uint8_t    dhcpMagicCookie[4] = {0x63, 0x82, 0x53, 0x63};      // DHCP cookie, network order
// DHCP event registration

static const void*      dhcpMemH = 0;        // memory handle

static PROTECTED_SINGLE_LIST      dhcpRegisteredUsers = { {0} };


static uint32_t         dhcpSecondCount = 0;    // DHCP time keeping, in seconds
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TIME_RES_MS) != 0
static uint32_t         dhcpMillisecCount = 0;    // DHCP time keeping, in milli seconds
#endif  // (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TIME_RES_MS) != 0


#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAKE_TMO) != 0
// these values override the parameters received from the server 
volatile uint32_t    _dhcpDbgBaseTmo =  2;    // DHCP base timeout; >= TCPIP_DHCP_EXP_BACKOFF_FUZZ + 1 ! 

volatile uint16_t    _dhcpDbgInitTmo =  8;    // DHCP initialization timeout

volatile uint32_t    _dhcpDbgGetOfferTmo = 0; // # of times to fake timeout for GET_OFFER
volatile uint32_t    _dhcpDbgGetOfferTmoCnt;  // run time counter

volatile uint32_t    _dhcpDbgGetRequestTmo = 0; //  # of times to fake timeout for GET_REQUEST_ACK
volatile uint32_t    _dhcpDbgGetRequestTmoCnt;  // run time counter

volatile uint32_t    _dhcpDbgGetRenewTmo = 0; //  # of times to fake timeout for GET_RENEW_ACK
volatile uint32_t    _dhcpDbgGetRenewTmoCnt;  // run time counter

volatile uint32_t    _dhcpDbgGetRebindTmo = 0; //  # of times to fake timeout for GET_RBIND_ACK
volatile uint32_t    _dhcpDbgGetRebindTmoCnt;  // run time counter
#endif  // TCPIP_DHCP_DEBUG_MASK

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_LEASE_DECLINE) != 0
volatile uint32_t    _dhcpDbgLeaseRequestDecline = 0; //  # of times to decline a lease for GET_REQUEST_ACK
volatile uint32_t    _dhcpDbgLeaseRequestDeclineCnt;  //  run time counter

volatile uint32_t    _dhcpDbgLeaseRenewDecline = 0; //  # of times to decline a lease for GET_RENEW_ACK
volatile uint32_t    _dhcpDbgLeaseRenewDeclineCnt;  //  run time counter

volatile uint32_t    _dhcpDbgLeaseRebindDecline = 0; //  # of times to decline a lease for GET_REBIND_ACK
volatile uint32_t    _dhcpDbgLeaseRebindDeclineCnt;  //  run time counter

#endif  // TCPIP_DHCP_DEBUG_MASK

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAIL_ARP) != 0
volatile int        _dhcpDbgFailArpCheck = 0;      // if !0, the ARP verification will fail
volatile int        _dhcpDbgFailArpCheckCnt;      // run time counter

char*               _dhcpDbgFakeArpAddressStr = 0; // "192.168.1.109";   // if !0, use this address for ARP check!
volatile uint32_t   _dhcpDbgFakeArpAddress = 0;    // run time value
#endif  // TCPIP_DHCP_DEBUG_MASK

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAKE_HOST_NAME) != 0
char*               _dhcpDbgHostName = 0; // if !0, use this name as DHCP host name!
#endif  // TCPIP_DHCP_DEBUG_MASK

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAKE_LEASE_TIME) != 0
volatile uint32_t    _dhcpDbgLeaseTime = 0;        // if !0, lease expiration time override
#endif  // TCPIP_DHCP_DEBUG_MASK


// receive dispatch function, processing functions
// returns > 0 the size of the option processed
// returns 0 if end found
// returns < 0 for error
typedef int (*_DHCPDispatchFnc)(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData);

typedef struct
{
    unsigned int        option;         // supported/processed option
    _DHCPDispatchFnc    dhcpProcFnc;    // processing function
}TCPIP_DHCP_OPTION_PROC_ENTRY;

static const TCPIP_DHCP_OPTION_PROC_ENTRY   _DHCPOptProcTbl[] = 
{
    // option                           // process function 
    {  TCPIP_DHCP_MESSAGE_TYPE,         _DHCPOptionProcessMsgType },
    {  TCPIP_DHCP_SUBNET_MASK,          _DHCPOptionProcessSubNet },
    {  TCPIP_DHCP_ROUTER,               _DHCPOptionProcessRouter },
    {  TCPIP_DHCP_DNS,                  _DHCPOptionProcessDns },
    {  TCPIP_DHCP_SERVER_IDENTIFIER,    _DHCPOptionProcessSrvIdent },
    {  TCPIP_DHCP_IP_LEASE_TIME,        _DHCPOptionProcessLease },
    {  TCPIP_DHCP_RENEW_TIME,           _DHCPOptionProcessRenewT },
    {  TCPIP_DHCP_REBIND_TIME,          _DHCPOptionProcessRebindT },
    {  TCPIP_DHCP_END_OPTION,           _DHCPOptionProcessEnd },

};


// send dispatch function, write options functions
// returns >= 0 the size of the option added
// returns < 0 for failure
typedef int (*_DHCPWriteOptFnc)(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData);

typedef struct
{
    // unsigned int        option;         // implicit write option
    _DHCPWriteOptFnc    dhcpWriteFnc;   // option write function
}TCPIP_DHCP_OPTION_WRITE_ENTRY;

static const TCPIP_DHCP_OPTION_WRITE_ENTRY   _DHCPOptWriteTbl[] = 
{
    // Note: this table entries are processed in sequrntial order!
    // option                                     // write option function 
    {  /* TCPIP_DHCP_MESSAGE_TYPE */             _DHCPOptionWriteMsgType },  // always the 1st!
    {  /* TCPIP_DHCP_SERVER_IDENTIFIER */        _DHCPOptionWriteSrvIdent },
    {  /* TCPIP_DHCP_PARAM_REQUEST_LIST */       _DHCPOptionWriteParamRequest },
    {  /* TCPIP_DHCP_PARAM_REQUEST_IP_ADDRESS */ _DHCPOptionWriteIPRequest },
    {  /* TCPIP_DHCP_HOST_NAME */                _DHCPOptionHostName },
    {  /* TCPIP_DHCP_PARAM_REQUEST_CLIENT_ID */  _DHCPOptionClientId },
    {  /* TCPIP_DHCP_END_OPTION */               _DHCPOptionWriteEnd },      // always the last!
};

// the list of the requested parameters in a DHCP message to the server
// each requested parameter/option code takes one byte
// Add more parameters here if needed but always < 256 !
static const uint8_t _DHCPRequestParamsTbl[] = 
{
    TCPIP_DHCP_SUBNET_MASK,         // request a subnet mask
    TCPIP_DHCP_ROUTER,              // request for a router
    TCPIP_DHCP_DNS,                 // request for DNS
    // Add more parameters here if needed
};


static __inline__ uint32_t __attribute__((always_inline)) _DHCPSecondCountGet(void)
{
    return dhcpSecondCount;
}

static __inline__ void __attribute__((always_inline)) _DHCPSecondCountSet(void)
{
    // use a 64 bit count to avoid roll over
    dhcpSecondCount = SYS_TMR_SystemCountGet() / SYS_TMR_SystemCountFrequencyGet(); 
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TIME_RES_MS) != 0
    dhcpMillisecCount = SYS_TMR_SystemCountGet() / (SYS_TMR_SystemCountFrequencyGet() / 1000); 
#endif  // (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TIME_RES_MS) != 0
}

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TIME_RES_MS) != 0
static __inline__ uint32_t __attribute__((always_inline)) _DHCPMillisecCountGet(void)
{
    return dhcpMillisecCount;
}
#endif  // (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TIME_RES_MS) != 0

static __inline__ void __attribute__((always_inline)) _DHCPSetFailTimeout(DHCP_CLIENT_VARS* pClient, bool resetTmo, bool isRunTime)
{
    pClient->flags.bReportFail = 1;
    if(resetTmo || pClient->tOpStart == 0)
    {
        pClient->tOpStart = _DHCPSecondCountGet();
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAIL_TMO_EVENT) != 0
        SYS_CONSOLE_PRINT("DHCP cli: %d, set fail tmo: %d, xid: 0x%8x, at: %s, reset: %s\r\n", pClient - DHCPClients, pClient->tOpStart, TCPIP_Helper_htonl(pClient->transactionID.Val), isRunTime ? "run" : "init", resetTmo ? "y" : "n");
#endif  // TCPIP_DHCP_DEBUG_MASK
    }
}

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_STATUS) != 0
static const char* _DHCPStatTbl[] = 
{
    "idle ",         // TCPIP_DHCP_IDLE 
    "wlink",         // TCPIP_DHCP_WAIT_LINK,         
    "discs",         // TCPIP_DHCP_SEND_DISCOVERY,	   
    "offrw",         // TCPIP_DHCP_GET_OFFER,		   
    "requs",         // TCPIP_DHCP_SEND_REQUEST,	 
    "requw",         // TCPIP_DHCP_GET_REQUEST_ACK,	
    "lchek",         // TCPIP_DHCP_WAIT_LEASE_CHECK,
    "lrtry",         // TCPIP_DHCP_WAIT_LEASE_RETRY,
    "bound",         // TCPIP_DHCP_BOUND,			   
    "renes",         // TCPIP_DHCP_SEND_RENEW,	
    "renew",         // TCPIP_DHCP_GET_RENEW_ACK,	   
    "rbnds",         // TCPIP_DHCP_SEND_REBIND,
    "rbndw",         // TCPIP_DHCP_GET_REBIND_ACK,     
};

static void _DHCPDbgStatus(DHCP_CLIENT_VARS* pClient)
{
    if(pClient->smState != pClient->prevState)
    {
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TIME_RES_MS) != 0
        SYS_CONSOLE_PRINT("DHCP cli: %d, state: %s, xid: 0x%8x, time: %d.%d\r\n", pClient - DHCPClients, _DHCPStatTbl[pClient->smState], TCPIP_Helper_htonl(pClient->transactionID.Val), _DHCPSecondCountGet(), _DHCPMillisecCountGet());
#else
        SYS_CONSOLE_PRINT("DHCP cli: %d, state: %s, xid: 0x%8x, time: %d\r\n", pClient - DHCPClients, _DHCPStatTbl[pClient->smState], TCPIP_Helper_htonl(pClient->transactionID.Val), _DHCPSecondCountGet());
#endif  // (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TIME_RES_MS) != 0
        pClient->prevState = pClient->smState;
    }
}
#else
#define _DHCPDbgStatus(pClient)
#endif  // TCPIP_DHCP_DEBUG_MASK

static __inline__ void __attribute__((always_inline)) _DHCPClientStateSet(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_STATUS newState)
{
    pClient->smState = newState;
    _DHCPDbgStatus(pClient);
}

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_ADDRESS_EVENTS) != 0
static void _DHCPDbgAddServiceEvent(DHCP_CLIENT_VARS* pClient, TCPIP_STACK_ADDRESS_SERVICE_EVENT addEv, const char* extraMsg)
{
    const char* evMsg;

    switch(addEv)
    {
        case TCPIP_STACK_ADDRESS_SERVICE_EVENT_NONE:
            evMsg = "none";
            break;

        case TCPIP_STACK_ADDRESS_SERVICE_EVENT_RUN_FAIL:
            evMsg = "runfail";
            break;

        case TCPIP_STACK_ADDRESS_SERVICE_EVENT_CONN_LOST:
            evMsg = "connlost";
            break;

        case TCPIP_STACK_ADDRESS_SERVICE_EVENT_USER_STOP:
            evMsg = "uerstop";
            break;

        case TCPIP_STACK_ADDRESS_SERVICE_EVENT_RUN_RESTORE:
            evMsg = "restore";
            break;

        default:
            evMsg = "unknown";
            break;
    }

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TIME_RES_MS) != 0
    SYS_CONSOLE_PRINT("DHCP cli: %d, address event: %s, extra: %s, time: %d.%d\r\n", pClient - DHCPClients, evMsg, extraMsg ? extraMsg : "none", _DHCPSecondCountGet(), _DHCPMillisecCountGet());
#else
    SYS_CONSOLE_PRINT("DHCP cli: %d, address event: %s, extra: %s, time: %d\r\n", pClient - DHCPClients, evMsg, extraMsg ? extraMsg : "none", _DHCPSecondCountGet());
#endif  // (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TIME_RES_MS) != 0
}

#else
#define _DHCPDbgAddServiceEvent(pClient, ev, extra)

#endif  // TCPIP_DHCP_DEBUG_MASK

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_RX_RES_ENABLE) != 0 || (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TX_MSG_ENABLE) != 0
static const char* _DHCPMsgTypeTbl[] = 
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
#endif  // (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_RX_RES_ENABLE) != 0 || (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TX_MSG_ENABLE) != 0


#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_RX_RES_ENABLE) != 0
static void _DHCPDbgRxDisplay(DHCP_CLIENT_VARS* pClient, unsigned int msgType, int rxErrCode)
{
    if(msgType >= sizeof(_DHCPMsgTypeTbl) / sizeof(*_DHCPMsgTypeTbl))
    {
        msgType = 0;    // this is unknown
    }

    const char* dhcpMsg = _DHCPMsgTypeTbl[msgType];

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TIME_RES_MS) != 0
    SYS_CONSOLE_PRINT("DHCP cli: %d, Rx: %s, xid: 0x%8x, error: %d, time: %d.%d\r\n", pClient - DHCPClients, dhcpMsg, TCPIP_Helper_htonl(pClient->transactionID.Val), rxErrCode, _DHCPSecondCountGet(), _DHCPMillisecCountGet());
#else
    SYS_CONSOLE_PRINT("DHCP cli: %d, Rx: %s, xid: 0x%8x, error: %d, time: %d\r\n", pClient - DHCPClients, dhcpMsg, TCPIP_Helper_htonl(pClient->transactionID.Val), rxErrCode, _DHCPSecondCountGet());
#endif  // (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TIME_RES_MS) != 0
}

#else
#define _DHCPDbgRxDisplay(pClient, msgType, rxErrCode) do{rxErrCode = rxErrCode;}while(0)
#endif  // TCPIP_DHCP_DEBUG_MASK

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TX_MSG_ENABLE) != 0
static void _DHCPDebugTxDisplay(DHCP_CLIENT_VARS* pClient, unsigned int msgType, bool success)
{
    if(msgType >= sizeof(_DHCPMsgTypeTbl) / sizeof(*_DHCPMsgTypeTbl))
    {
        msgType = 0;    // this is unknown
    }

    const char* dhcpMsg = _DHCPMsgTypeTbl[msgType];

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TIME_RES_MS) != 0
    SYS_CONSOLE_PRINT("DHCP cli: %d, Tx: %s %s, xid: 0x%8x, time: %d.%d\r\n", pClient - DHCPClients, dhcpMsg, success ? "Sent" : "Failed", TCPIP_Helper_htonl(pClient->transactionID.Val), _DHCPSecondCountGet(), _DHCPMillisecCountGet());
#else
    SYS_CONSOLE_PRINT("DHCP cli: %d, Tx: %s %s, xid: 0x%8x, time: %d\r\n", pClient - DHCPClients, dhcpMsg, success ? "Sent" : "Failed", TCPIP_Helper_htonl(pClient->transactionID.Val), _DHCPSecondCountGet());
#endif  // (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TIME_RES_MS) != 0
}
#else
#define _DHCPDebugTxDisplay(pClient, msgType, success)
#endif  // (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TX_MSG_ENABLE) != 0


static void _DHCPClientClose(TCPIP_NET_IF* pNetIf, bool disable, bool release)
{
    DHCP_CLIENT_VARS* pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf);
    
    if(pClient->flags.bDHCPEnabled != 0)
    {
        if(release && pClient->smState >= TCPIP_DHCP_BOUND)
        {  
            _DHCPSend(pClient, pNetIf, TCPIP_DHCP_RELEASE_MESSAGE, 0);
        }

		pClient->flags.bIsBound = false;
        pClient->flags.bDHCPServerDetected = false;
        pClient->flags.bReportFail = 1;	
        pClient->tOpStart = 0; 
        pClient->dhcpTmo = pClient->dhcpTmoBase;
        
        if(disable)
        {
            pClient->flags.bWasBound = false;	
            pClient->flags.bDHCPEnabled = false;
            pNetIf->Flags.bIsDHCPEnabled = false;
        }
        else
        {   // let it active
            _DHCPClientStateSet(pClient, TCPIP_DHCP_IDLE);
        }
    }
}

static void _DHCPEnable(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPERATION_TYPE opType)
{
    DHCP_CLIENT_VARS* pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf);
    
    if(TCPIP_STACK_NetworkIsLinked(pNetIf) || !_TCPIPStackNetIsPrimary(pNetIf))
    {   // the idea is that if the link is down we'll get an connection up event anyway
        // to wake us up from TCPIP_DHCP_IDLE
        // aliased interfaces don't get an event!
        _DHCPClientStateSet(pClient, TCPIP_DHCP_WAIT_LINK);
    }
    else
    {
        _DHCPClientStateSet(pClient, TCPIP_DHCP_IDLE);
    }
    
    pClient->flags.bDHCPEnabled = true;
    pClient->dhcpOp = opType;
    pNetIf->Flags.bIsDHCPEnabled = true;


    if(dhcpFilterHandle == 0)
    {
        dhcpFilterHandle = IPv4RegisterFilter(_DHCPPacketFilter, 0);
        if(dhcpFilterHandle == 0)
        {
            SYS_ERROR(SYS_ERROR_WARNING, "DHCP: Failed to register IPv4 filter! \r\n");
        }
    }

    if(opType == TCPIP_DHCP_OPER_INIT || opType == TCPIP_DHCP_OPER_INIT_REBOOT)
    {   // reinit the time base
        pClient->dhcpTmo = pClient->dhcpTmoBase;
        _DHCPSetFailTimeout(pClient, true, false);

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAKE_TMO) != 0
        // re-init the debug counters
        _dhcpDbgGetOfferTmoCnt = _dhcpDbgGetOfferTmo;
        _dhcpDbgGetRequestTmoCnt = _dhcpDbgGetRequestTmo;
        _dhcpDbgGetRenewTmoCnt = _dhcpDbgGetRenewTmo;
        _dhcpDbgGetRebindTmoCnt = _dhcpDbgGetRebindTmo;
#endif  // TCPIP_DHCP_DEBUG_MASK

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_LEASE_DECLINE) != 0
        _dhcpDbgLeaseRequestDeclineCnt = _dhcpDbgLeaseRequestDecline;
        _dhcpDbgLeaseRenewDeclineCnt = _dhcpDbgLeaseRenewDecline;
        _dhcpDbgLeaseRebindDeclineCnt = _dhcpDbgLeaseRebindDecline;
#endif  // TCPIP_DHCP_DEBUG_MASK

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAIL_ARP) != 0
        _dhcpDbgFailArpCheckCnt = _dhcpDbgFailArpCheck;
#endif  // TCPIP_DHCP_DEBUG_MASK
    }
    
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void _DHCPCleanup(void)
{
    if(dhcpClientSocket != INVALID_UDP_SOCKET)
    {
        TCPIP_UDP_Close(dhcpClientSocket);
        dhcpClientSocket = INVALID_UDP_SOCKET;
    }

    TCPIP_HEAP_Free(dhcpMemH, DHCPClients);
    DHCPClients = 0;

    TCPIP_Notification_Deinitialize(&dhcpRegisteredUsers, dhcpMemH);
        
    if(dhcpFilterHandle != 0)
    {
        Ipv4DeRegisterFilter(dhcpFilterHandle);
        dhcpFilterHandle = 0;
    }

    if(dhcpSignalHandle)
    {
        _TCPIPStackSignalHandlerDeregister(dhcpSignalHandle);
        dhcpSignalHandle = 0;
    }

    dhcpInterfaces = 0;
}
#else
#define _DHCPCleanup()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static void _DHCPSetRunFail(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_STATUS newState, bool expBackoff) 
{
    _DHCPClientStateSet(pClient, newState);
    pClient->dhcpOp = TCPIP_DHCP_OPER_INIT;       // failure forces a brand new lease acquisition
    if(expBackoff)
    {   // exponential backoff the DHCP timeout
        pClient->dhcpTmo <<= 1;
        if(pClient->dhcpTmo > TCPIP_DHCP_EXP_BACKOFF_LIMIT)
        {
            pClient->dhcpTmo = TCPIP_DHCP_EXP_BACKOFF_LIMIT;
        }
    }

    if(pClient->flags.bReportFail)
    {
        _DHCPSetFailTimeout(pClient, false, true);
    }
}

static void _DHCPSetTimeout(DHCP_CLIENT_VARS* pClient)
{
    // add a random variation [-TCPIP_DHCP_EXP_BACKOFF_FUZZ, TCPIP_DHCP_EXP_BACKOFF_FUZZ] seconds
    uint32_t sysFreq = SYS_TMR_TickCounterFrequencyGet();
    pClient->waitTicks = (pClient->dhcpTmo - TCPIP_DHCP_EXP_BACKOFF_FUZZ) * sysFreq + SYS_RANDOM_PseudoGet() % ((2 * TCPIP_DHCP_EXP_BACKOFF_FUZZ) * sysFreq);
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
bool TCPIP_DHCP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_DHCP_MODULE_CONFIG* pDhcpConfig)
{
    DHCP_CLIENT_VARS*   pClient;
    bool    iniRes;
	
    if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        if(stackCtrl->pNetIf->Flags.bIsDHCPEnabled != 0)
        {
            _DHCPEnable(stackCtrl->pNetIf, TCPIP_DHCP_OPER_INIT);
        }
        return true;
    }

    // stack init
    if(dhcpInitCount == 0)
    {   // first time we're run
        // check configuration data is not missing
        if(pDhcpConfig == 0)
        {
            return false;
        }
        // store the memory allocation handle
        dhcpMemH = stackCtrl->memH;
        
        dhcpClientSocket = INVALID_UDP_SOCKET;

        DHCPClients = (DHCP_CLIENT_VARS*)TCPIP_HEAP_Calloc(dhcpMemH,  stackCtrl->nIfs, sizeof(DHCP_CLIENT_VARS));
        if(DHCPClients == 0)
        {   // failed
            return false;
        }

        dhcpFilterHandle = 0;

        // create the DHCP timer
        dhcpSignalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_DHCP_Task, TCPIP_DHCP_TASK_TICK_RATE);
        iniRes = TCPIP_Notification_Initialize(&dhcpRegisteredUsers);
        dhcpClientPort = pDhcpConfig->dhcpCliPort;
        dhcpServerPort = pDhcpConfig->dhcpSrvPort;
        dhcpClientSocket = _DHCPOpenSocket();


        if(dhcpSignalHandle == 0 || iniRes == 0 || dhcpClientSocket == INVALID_UDP_SOCKET)
        {   // cannot create the DHCP timer/registration list or socket
            _DHCPCleanup();
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
    pClient->tOpFailTmo = (pDhcpConfig->dhcpTmo < 1) ? TCPIP_DHCP_INIT_FAIL_TMO : (uint16_t)pDhcpConfig->dhcpTmo;

    // set a proper timeout base
    pClient->dhcpTmoBase = (TCPIP_DHCP_EXP_BACKOFF_BASE < TCPIP_DHCP_EXP_BACKOFF_FUZZ + 1) ? TCPIP_DHCP_EXP_BACKOFF_FUZZ + 1 : TCPIP_DHCP_EXP_BACKOFF_BASE;
    pClient->tLeaseCheck = TCPIP_DHCP_LEASE_CHECK_TMO;
    
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAKE_TMO) != 0
    pClient->dhcpTmoBase = _dhcpDbgBaseTmo;
    pClient->tOpFailTmo = _dhcpDbgInitTmo;
#endif  // TCPIP_DHCP_DEBUG_MASK

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAIL_ARP) != 0
    if(_dhcpDbgFakeArpAddressStr != 0)
    {
        TCPIP_Helper_StringToIPAddress(_dhcpDbgFakeArpAddressStr, (IPV4_ADDR*)&_dhcpDbgFakeArpAddress);
    }
    else
    {
        _dhcpDbgFakeArpAddress = 0;
    }
#endif  // TCPIP_DHCP_DEBUG_MASK

    if(stackCtrl->pNetIf->Flags.bIsDHCPEnabled != 0)
    {   // override the pDhcpConfig->dhcpEnable passed with the what the stack manager says
        _DHCPEnable(stackCtrl->pNetIf, TCPIP_DHCP_OPER_INIT);
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

    if(dhcpInitCount > 0)
    {   // we're up and running
        // one way or another this interface is going down
        _DHCPClientClose(stackCtrl->pNetIf, true, true);

        //  the registered users for this interface are not removed
        //  since this interface is closed there won't be any event generated on it anyway
        //  deallocation will wait for the whole stack to deinit 

        if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--dhcpInitCount == 0)
            {   // all closed
                // release resources
                _DHCPCleanup();
                dhcpMemH = 0;
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
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(hNet);
    if(DHCPClients && pNetIf)
    {
        DHCP_CLIENT_VARS* pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf);
        if(pClient->flags.bDHCPEnabled != 0)
        {
            _DHCPClientClose(pNetIf, true, true);
            TCPIP_STACK_AddressServiceEvent(pNetIf, TCPIP_STACK_ADDRESS_SERVICE_DHCPC, TCPIP_STACK_ADDRESS_SERVICE_EVENT_USER_STOP);
            _DHCPDbgAddServiceEvent(pClient, TCPIP_STACK_ADDRESS_SERVICE_EVENT_USER_STOP, 0);
            _DHCPNotifyClients(pNetIf, DHCP_EVENT_SERVICE_DISABLED);
        }
        return true;
    }

    return false;
}


bool TCPIP_DHCP_Enable(TCPIP_NET_HANDLE hNet)
{
    return _DHCPStartOperation(_TCPIPStackHandleToNetUp(hNet), TCPIP_DHCP_OP_REQ_ENABLE, 0);
}

bool TCPIP_DHCP_Renew(TCPIP_NET_HANDLE hNet)
{
    return _DHCPStartOperation(_TCPIPStackHandleToNetUp(hNet), TCPIP_DHCP_OP_REQ_RENEW, 0);
}


bool TCPIP_DHCP_Request(TCPIP_NET_HANDLE hNet, IPV4_ADDR reqAddress)
{
    return _DHCPStartOperation(_TCPIPStackHandleToNetUp(hNet), TCPIP_DHCP_OP_REQ_REQUEST, reqAddress.Val);
}

static bool _DHCPStartOperation(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPERATION_REQ opReq, uint32_t reqAddress)
{
    TCPIP_DHCP_OPERATION_TYPE opType = TCPIP_DHCP_OPER_NONE;
    if(DHCPClients == 0 || pNetIf == 0)
    {
        return false;
    }

    DHCP_CLIENT_VARS* pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf);

    switch(opReq)
    {
        case TCPIP_DHCP_OP_REQ_ENABLE:
            if(pClient->flags.bDHCPEnabled != 0)
            {   // already enabled
                return true;
            }

            if(TCPIP_STACK_AddressServiceCanStart(pNetIf, TCPIP_STACK_ADDRESS_SERVICE_DHCPC))
            {
                opType = TCPIP_DHCP_OPER_INIT;
            }
            break;

        case TCPIP_DHCP_OP_REQ_RENEW:
            // DHCP should be running and bound
            if(pClient->flags.bDHCPEnabled != 0 && pClient->flags.bIsBound != 0)
            {
                opType = TCPIP_DHCP_OPER_RENEW;
            }
            break;

        case TCPIP_DHCP_OP_REQ_REQUEST:
            // if DHCP not running, then it needs to be enabled
            if(reqAddress != 0)
            {
                if(pClient->flags.bDHCPEnabled != 0)
                {   // depending on the client state
                    if(pClient->smState == TCPIP_DHCP_GET_REQUEST_ACK || 
                       pClient->smState == TCPIP_DHCP_GET_RENEW_ACK ||
                       pClient->smState == TCPIP_DHCP_GET_REBIND_ACK)
                    {   // already in a request
                       return false;
                    }
                    // avoid replying to these requests while changing the address
                    IPV4_ADDR zeroAdd = {0};
                    _TCPIPStackSetConfigAddress(pNetIf, &zeroAdd, &zeroAdd, true);
                }
                else if(!TCPIP_STACK_AddressServiceCanStart(pNetIf, TCPIP_STACK_ADDRESS_SERVICE_DHCPC))
                {
                    return false;
                }
                
                pClient->dhcpIPAddress.Val = reqAddress;
                opType = TCPIP_DHCP_OPER_INIT_REBOOT;
            }

            break;
        
        default:
            break;
    }

    if(opType != TCPIP_DHCP_OPER_NONE)
    {
        _DHCPEnable(pNetIf, opType);
        return true;
    }

    return false;
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
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(hNet);
    if(DHCPClients && pNetIf)
    {
        DHCP_CLIENT_VARS* pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf);
        return pClient->flags.bDHCPEnabled != 0;
    }
    return false;
}

bool TCPIP_DHCP_IsActive(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(hNet);
    if(DHCPClients && pNetIf)
    {
        return pNetIf->Flags.bIsDHCPEnabled != 0;
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
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(hNet);
    if(DHCPClients && pNetIf)
    {
        return (DHCPClients + TCPIP_STACK_NetIxGet(pNetIf))->flags.bIsBound;
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
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(hNet);
    if(DHCPClients && pNetIf)
    {
        return (DHCPClients + TCPIP_STACK_NetIxGet(pNetIf))->flags.bDHCPServerDetected;
    }
    return false;
}

// adjust the DHCP timeout
// how long to wait before a DHCP lease in the initialization
// and base time out; seconds
bool TCPIP_DHCP_RequestTimeoutSet(TCPIP_NET_HANDLE hNet, uint16_t initTmo, uint16_t dhcpBaseTmo)
{   
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(hNet);
    if(DHCPClients && pNetIf && initTmo)
    {
        DHCP_CLIENT_VARS* pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf);
        pClient->tOpFailTmo = initTmo;
        dhcpBaseTmo = (dhcpBaseTmo < TCPIP_DHCP_EXP_BACKOFF_FUZZ + 1) ? TCPIP_DHCP_EXP_BACKOFF_FUZZ + 1 : dhcpBaseTmo; 
        pClient->dhcpTmoBase = dhcpBaseTmo;
        return true;
    }
    return false;
}


void TCPIP_DHCP_Task(void)
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
        TCPIP_DHCP_Process(isTmo);
    }
}

// send a signal to the DHCP module that data is available
// no manager alert needed since this normally results as a higher layer (UDP) signal
static void _DHCPSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_UDP_SIGNAL_RX_DATA)
    {
        _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}


static void TCPIP_DHCP_Process(bool isTmo)
{
    unsigned int        recvMsg;
    DHCP_CLIENT_VARS*   pClient;
    int                 netIx, nNets;
    TCPIP_NET_IF*       pNetIf;

    if(isTmo)
    {   // update DHCP time keeping
        _DHCPSecondCountSet();
    }

    nNets = TCPIP_STACK_NumberOfNetworksGet();
    for(netIx = 0; netIx < nNets; netIx++) 
    {
        pNetIf = (TCPIP_NET_IF*)TCPIP_STACK_IndexToNet (netIx);
        if(!TCPIP_STACK_NetworkIsUp(pNetIf))
        {   // inactive interface
            continue;
        }

        pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf); 

        if(pClient->flags.bDHCPEnabled == false)
        {   // not enabled on this interface
            continue;
        }

        // check loss of lease
        _DHCPCheckRunFailEvent(pNetIf, pClient);

        switch(pClient->smState)
        {
            case TCPIP_DHCP_IDLE:
                break;

            case TCPIP_DHCP_WAIT_LINK:
                if(!TCPIP_STACK_NetworkIsLinked(pNetIf))
                {   // no connection yet
                    break;
                }


                // advance the state machine according to the DHCP operation
                if(pClient->dhcpOp == TCPIP_DHCP_OPER_INIT_REBOOT)
                {
                    _DHCPClientStateSet(pClient, TCPIP_DHCP_SEND_REQUEST);
                }
                else if(pClient->dhcpOp == TCPIP_DHCP_OPER_RENEW)
                {
                    _DHCPClientStateSet(pClient, TCPIP_DHCP_SEND_RENEW);
                }
                else
                {
                    _DHCPClientStateSet(pClient, TCPIP_DHCP_SEND_DISCOVERY);
                }

                break;
              

            case TCPIP_DHCP_SEND_DISCOVERY:
                // set a default lease just in case the server won't specify one
                _DHCPSetLeaseTimeParams(pClient, 0);
                pClient->validValues.val = 0x00;
                pClient->flags.bIsBound = false;	
                pClient->flags.bOfferReceived = false;
                pClient->serverAddress.Val = 0;

                // Send the DHCP Discover broadcast
                if(!_DHCPSend(pClient, pNetIf, TCPIP_DHCP_DISCOVER_MESSAGE, TCPIP_DHCP_FLAG_SEND_ZERO_ADD | TCPIP_DHCP_FLAG_SEND_BCAST))
                {
                    break;
                }

                _DHCPNotifyClients(pNetIf, DHCP_EVENT_DISCOVER);
                // Start a timer and begin looking for a response
                _DHCPSetTimeout(pClient);
                _DHCPClientStateSet(pClient, TCPIP_DHCP_GET_OFFER);
                break;

            case TCPIP_DHCP_GET_OFFER:
                // process received data
                recvMsg = _DHCPProcessReceiveData(pClient, pNetIf);
                if(recvMsg == TCPIP_DHCP_TIMEOUT_MESSAGE)
                {   // no data available
                    // Go back and transmit a new discovery if we didn't get an offer within the requested time
                    if((SYS_TMR_TickCountGet() - pClient->startWait) >= pClient->waitTicks)
                    {
                        _DHCPSetRunFail(pClient, TCPIP_DHCP_SEND_DISCOVERY, true);  
                        _DHCPNotifyClients(pNetIf, DHCP_EVENT_TIMEOUT);
                    }
                    break;
                }
                else if(recvMsg != TCPIP_DHCP_UNKNOWN_MESSAGE)
                {   // Got some reply
                    // Let the DHCP server module know that there is a DHCP server 
                    // on this network
                    pClient->flags.bDHCPServerDetected = true;
                }

                // However we're waiting for an offer
                if(recvMsg != TCPIP_DHCP_OFFER_MESSAGE)
                {
                    break;
                }

                _DHCPClientStateSet(pClient, TCPIP_DHCP_SEND_REQUEST);
                // No break

            case TCPIP_DHCP_SEND_REQUEST:
                // Send the DHCP request message
                if(!_DHCPSend(pClient, pNetIf, TCPIP_DHCP_REQUEST_MESSAGE, (pClient->dhcpOp == TCPIP_DHCP_OPER_INIT_REBOOT) ? TCPIP_DHCP_FLAG_SEND_BCAST : TCPIP_DHCP_FLAG_SEND_ZERO_ADD | TCPIP_DHCP_FLAG_SEND_BCAST))
                {
                    break;
                }
                // store the request time
                pClient->tRequest = _DHCPSecondCountGet();
                _DHCPNotifyClients(pNetIf, DHCP_EVENT_REQUEST);
                // Start a timer and begin looking for a response
                _DHCPSetTimeout(pClient);
                _DHCPClientStateSet(pClient, TCPIP_DHCP_GET_REQUEST_ACK);
                break;

            case TCPIP_DHCP_GET_REQUEST_ACK:
                // process received data
                if(_DHCPProcessReceiveData(pClient, pNetIf) == TCPIP_DHCP_TIMEOUT_MESSAGE)
                {   // no data available
                    // Go back and retransmit a new discovery if we didn't get an ACK
                    if((SYS_TMR_TickCountGet() - pClient->startWait) >= pClient->waitTicks)
                    {
                        _DHCPSetRunFail(pClient, TCPIP_DHCP_SEND_DISCOVERY, true);
                        _DHCPNotifyClients(pNetIf, DHCP_EVENT_TIMEOUT);
                    }
                }
                break;

            case TCPIP_DHCP_WAIT_LEASE_CHECK:
                // check for an ARP entry
                {
                    bool arpChkFail = false;
                    bool arpChkSuccess = false;
                    IPV4_ADDR arpCheck;

                    arpCheck.Val = pClient->dhcpIPAddress.Val;
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAIL_ARP) != 0
                    if(_dhcpDbgFakeArpAddress != 0)
                    {
                        arpCheck.Val = _dhcpDbgFakeArpAddress;
                    }
#endif  // TCPIP_DHCP_DEBUG_MASK

                    if(TCPIP_ARP_IsResolved(pNetIf, &arpCheck, 0))
                    {   // oooops, someone else with this address!
                        arpChkFail = true;
                    }
                    else if((_DHCPSecondCountGet() - pClient->startWait) >= pClient->tLeaseCheck)
                    {   // no ARP conflict
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAIL_ARP) != 0
                        if(_dhcpDbgFailArpCheckCnt != 0)
                        {
                            _dhcpDbgFailArpCheckCnt--;
                            arpChkFail = true;
                        }
                        else
#endif  // TCPIP_DHCP_DEBUG_MASK
                        {   // validation success
                            _DHCPSetNewLease(pClient, pNetIf);
                            arpChkSuccess = true;
                        }
                    }
                    // else no timeout yet
                    
                    if(arpChkFail || arpChkSuccess)
                    {   // remove ARP entry so that we can probe it again if DHCP is disabled/enabled
                        // or the lease is lost quicker than the ARP entry expiration timeout    
                        TCPIP_ARP_EntryRemove(pNetIf,  &arpCheck);
                    }

                    if(arpChkFail)
                    {   // fail
                        _DHCPSend(pClient, pNetIf, TCPIP_DHCP_DECLINE_MESSAGE, TCPIP_DHCP_FLAG_SEND_ZERO_ADD | TCPIP_DHCP_FLAG_SEND_BCAST);
                        _DHCPClientStateSet(pClient, TCPIP_DHCP_WAIT_LEASE_RETRY);
                        pClient->startWait = _DHCPSecondCountGet();
                        _DHCPNotifyClients(pNetIf, DHCP_EVENT_DECLINE);
                    }
                }
                break;

            case TCPIP_DHCP_WAIT_LEASE_RETRY:
                // wait before we can attempt another retry
                if((_DHCPSecondCountGet() - pClient->startWait) >= TCPIP_DHCP_WAIT_FAIL_CHECK_TMO)
                {
                    _DHCPSetRunFail(pClient, TCPIP_DHCP_SEND_DISCOVERY, false);
                }
                break;

            case TCPIP_DHCP_BOUND:
                // check for T1 timeout
                if((_DHCPSecondCountGet() - pClient->tRequest) < pClient->t1Seconds)
                {   // within BOUND state
                    break;
                }
                // time for renew
                _DHCPClientStateSet(pClient, TCPIP_DHCP_SEND_RENEW);
                // No break

            case TCPIP_DHCP_SEND_RENEW:
                // Send the DHCP request message
                if(!_DHCPSend(pClient, pNetIf, TCPIP_DHCP_REQUEST_RENEW_MESSAGE, 0))
                {
                    break;
                }
                // store the request time
                pClient->tRenewRequest = _DHCPSecondCountGet();
                _DHCPNotifyClients(pNetIf, DHCP_EVENT_REQUEST_RENEW);
                pClient->flags.bOfferReceived = false;

                // Set as timeout half the time remaining to T2 (rebind)
                pClient->t3Seconds = ((pClient->t2Seconds - pClient->t1Seconds) - (pClient->tRenewRequest - pClient->tRequest)) >> 1;
                if( pClient->t3Seconds < TCPIP_DHCP_RENEW_RETRY_TIMEOUT)
                {
                    pClient->t3Seconds = TCPIP_DHCP_RENEW_RETRY_TIMEOUT;
                }
                pClient->startWait = _DHCPSecondCountGet();
                _DHCPClientStateSet(pClient, TCPIP_DHCP_GET_RENEW_ACK);
                break;

            case TCPIP_DHCP_GET_RENEW_ACK:
                // process received data
                if(_DHCPProcessReceiveData(pClient, pNetIf) == TCPIP_DHCP_TIMEOUT_MESSAGE)
                {   // no data available
                    // check first for T2 timeout
                    if((_DHCPSecondCountGet() - pClient->tRequest) >= pClient->t2Seconds)
                    {   // RENEWING state expired; move to REBIND!
                        _DHCPClientStateSet(pClient, TCPIP_DHCP_SEND_REBIND);
                    }
                    else if((_DHCPSecondCountGet() - pClient->startWait) >= pClient->t3Seconds)
                    {   // check if there's time for retransmission of another DHCP request
                        _DHCPClientStateSet(pClient, TCPIP_DHCP_SEND_RENEW);
                    }
                    // else no time for retry yet;
                }

                break;

            case TCPIP_DHCP_SEND_REBIND:
                // Send the DHCP request message
                if(!_DHCPSend(pClient, pNetIf, TCPIP_DHCP_REQUEST_RENEW_MESSAGE, TCPIP_DHCP_FLAG_SEND_BCAST))
                {
                    break;
                }
                // store the request time
                pClient->tRenewRequest = _DHCPSecondCountGet();
                _DHCPNotifyClients(pNetIf, DHCP_EVENT_REQUEST_REBIND);
                pClient->flags.bOfferReceived = false;

                // Set as timeout half the time remaining to Texp (lease expiration)
                pClient->t3Seconds = ((pClient->tExpSeconds - pClient->t2Seconds) - (pClient->tRenewRequest - pClient->tRequest)) >> 1;
                if( pClient->t3Seconds < TCPIP_DHCP_REBIND_RETRY_TIMEOUT)
                {
                    pClient->t3Seconds = TCPIP_DHCP_REBIND_RETRY_TIMEOUT;
                }
                pClient->startWait = _DHCPSecondCountGet();
                _DHCPClientStateSet(pClient, TCPIP_DHCP_GET_REBIND_ACK);
                break;

            case TCPIP_DHCP_GET_REBIND_ACK:
                // process received data
                if(_DHCPProcessReceiveData(pClient, pNetIf) == TCPIP_DHCP_TIMEOUT_MESSAGE)
                {   // no data available
                    // check first for Texp timeout
                    if((_DHCPSecondCountGet() - pClient->tRequest) >= pClient->tExpSeconds)
                    {   // REBIND state expired; restart
                        _DHCPSetRunFail(pClient, TCPIP_DHCP_SEND_DISCOVERY, false);
                        _DHCPNotifyClients(pNetIf, DHCP_EVENT_TIMEOUT);
                    }
                    else if((_DHCPSecondCountGet() - pClient->startWait) >= pClient->t3Seconds)
                    {   // check if there's time for retransmission of another DHCP request
                        _DHCPClientStateSet(pClient, TCPIP_DHCP_SEND_REBIND);
                    }
                    // else no time for retry yet;
                }
                break;
        }
    }

    // everything that needed processing was done; discard leftovers
    TCPIP_UDP_Discard(dhcpClientSocket);
}



/*****************************************************************************
Function:
  unsigned int _DHCPReceive(DHCP_CLIENT_VARS* pClient, TCPIP_NET_IF* pNetIf)

Description:
  Receives and parses a DHCP message.

Precondition:
  A DHCP message is waiting in the UDP buffer.

Parameters:
  pClient - client descriptor
  pNetIf - interface to use

Returns:
  One of the DCHP_TYPE* contants.
***************************************************************************/
static unsigned int _DHCPReceive(DHCP_CLIENT_VARS* pClient, TCPIP_NET_IF* pNetIf)
{
                    /*********************************************************************
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

	********************************************************************/

    int  netIx, nNets;
    TCPIP_DHCP_FRAME_HEADER    dhcpHdr;
    TCPIP_DHCP_FRAME_OPTIONS   dhcpOpt;
    TCPIP_DHCP_OPTION_PROCESS_DATA  dhcpOptData;
    int32_t optSize;    // size of the option in this DHCP packet
    TCPIP_DHCP_OPTION_RESULT   dspRes;
    DHCP_CLIENT_VARS *pOwner, *pSrchClient;
    TCPIP_NET_IF* pIf;

    unsigned int               msgType;
    UDP_SOCKET                 s;


    int rxErrCode = 0;
    s = dhcpClientSocket;
    pOwner = 0;
    msgType = TCPIP_DHCP_UNKNOWN_MESSAGE;

    while(true)
    {
        // get the HDCP header
        TCPIP_UDP_ArrayGet(s, &dhcpHdr.op, sizeof(dhcpHdr));

        // Make sure this is TCPIP_BOOT_REPLY.
        if ( dhcpHdr.op != TCPIP_BOOT_REPLY )
        {
            rxErrCode = 1;  
            break;
        }

        // ignore the unused part of the header: server host name and boot file
        if(TCPIP_UDP_ArrayGet(s, 0, sizeof(TCPIP_DHCP_FRAME_OPT_HEADER)) != sizeof(TCPIP_DHCP_FRAME_OPT_HEADER))
        {
            rxErrCode = 2;  
            break;
        }

        // get the variable options
        optSize = (int32_t)TCPIP_UDP_ArrayGet(s, dhcpOpt.cookie, sizeof(dhcpOpt));
        if(optSize < sizeof(dhcpOpt.cookie) || (memcmp(dhcpOpt.cookie, dhcpMagicCookie, sizeof(dhcpOpt.cookie)) != 0))
        {
            rxErrCode = 3;  
            break;
        }

        // check that this message belongs to one of our clients
        nNets = TCPIP_STACK_NumberOfNetworksGet();
        pSrchClient = DHCPClients; 
        for(netIx = 0; netIx < nNets; netIx++, pSrchClient++) 
        {
            pIf = (TCPIP_NET_IF*)TCPIP_STACK_IndexToNet (netIx);
            if(!TCPIP_STACK_NetworkIsUp(pIf) || pSrchClient->flags.bDHCPEnabled == false)
            {   // not enabled on this interface
                continue;
            }

            if(pSrchClient->smState == TCPIP_DHCP_GET_OFFER || pSrchClient->smState == TCPIP_DHCP_GET_REQUEST_ACK ||
                    pSrchClient->smState == TCPIP_DHCP_GET_RENEW_ACK || pSrchClient->smState == TCPIP_DHCP_GET_REBIND_ACK)
            {
                if(dhcpHdr.xid == pSrchClient->transactionID.Val && memcmp(dhcpHdr.chaddr, _TCPIPStack_NetMACAddressGet(pIf), sizeof(TCPIP_MAC_ADDR)) == 0)
                {   // seems to be the right one
                    pOwner = pSrchClient; 
                    break;
                }
            }
        }

        if(pClient != pOwner)
        {   // not ours
            rxErrCode =  pOwner == 0 ? 4 : 5;
            break;
        }

        pOwner = 0; // signal that message needs to be discarded

        // ok it's ours, we can process this one
        // if this is the first offer record yiaddr value/client IP address
        // so that we can REQUEST it.
        if(!pClient->flags.bOfferReceived)
        {
            pClient->dhcpIPAddress.Val = dhcpHdr.yiaddr;
            pClient->validValues.IPAddress = 1;
        }

        memset(&dhcpOptData, 0, sizeof(dhcpOptData));
        dhcpOptData.pOpt = dhcpOpt.options;
        dhcpOptData.optSize = optSize - sizeof(dhcpMagicCookie);   // eat up the cookie
        dhcpOptData.msgType = TCPIP_DHCP_UNKNOWN_MESSAGE;
        // so far so good
        // Dispatch by the Option number
        dspRes = _DHCPOptionProcess(pClient, &dhcpOptData);

        if(dspRes == TCPIP_DHCP_OPTION_RES_FAIL)
        {   // failed
            rxErrCode = 6;
            break;
        }
        else if(dspRes != TCPIP_DHCP_OPTION_RES_END)
        {   // failed
            rxErrCode = 7;
            break;
        }

        // check what we got
        if ( dhcpOptData.msgType == TCPIP_DHCP_OFFER_MESSAGE || (dhcpOptData.msgType == TCPIP_DHCP_ACK_MESSAGE && pClient->dhcpOp == TCPIP_DHCP_OPER_INIT_REBOOT))
        {   // store the current server ID
            pClient->dwServerID = dhcpOptData.serverID.Val;
            pClient->flags.bOfferReceived = true;
        }
        else if(pClient->dwServerID != dhcpOptData.serverID.Val)
        {   // Fail if the server id doesn't match
            rxErrCode = 8;
            break;
        }

        // success
        msgType = dhcpOptData.msgType;

        if(msgType == TCPIP_DHCP_ACK_MESSAGE)
        {   // received a lease; get values
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAKE_LEASE_TIME) != 0
            if(_dhcpDbgLeaseTime)
            {
                dhcpOptData.leaseTime.Val = _dhcpDbgLeaseTime; 
                dhcpOptData.renewTime.Val = 0; 
                dhcpOptData.rebindTime.Val = 0; 
            }
#endif  // TCPIP_DHCP_DEBUG_MASK
            _DHCPSetLeaseTimeParams(pClient, &dhcpOptData);
#if defined TCPIP_DHCP_STORE_BOOT_FILE_NAME
            TCPIP_UDP_RxOffsetSet(s, sizeof(dhcpHdr) + sizeof(((TCPIP_DHCP_FRAME_OPT_HEADER*)0)->sname));
            TCPIP_UDP_ArrayGet(s, pClient->bootFileName, sizeof(pClient->bootFileName));
            pClient->bootFileName[sizeof(pClient->bootFileName) - 1] = 0;   // make sure is null terminated
#endif // defined TCPIP_DHCP_STORE_BOOT_FILE_NAME
        }

        break;
    }

    if(pOwner == 0)
    {   // not ours, invalid or processed; simply discard
        TCPIP_UDP_Discard(s);                     
    }
    else 
    {   // save it for another client
        TCPIP_UDP_RxOffsetSet(s, 0);
    }

    _DHCPDbgRxDisplay(pClient, msgType, rxErrCode);
    return msgType;
}

static TCPIP_DHCP_OPTION_RESULT _DHCPOptionProcess(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData)
{
    int      optIx;
    int      optLen;
    uint8_t  opt;
    bool     optFound;
    const TCPIP_DHCP_OPTION_PROC_ENTRY* pEntry;


    while(pOptData->optSize > 0)
    {
        opt = *pOptData->pOpt;
        pEntry = _DHCPOptProcTbl;
        optFound = false;
        for(optIx = 0; optIx < sizeof(_DHCPOptProcTbl) / sizeof(*_DHCPOptProcTbl); optIx++, pEntry++)
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
        }

        if(optFound == false)
        {   // not found option
            // unprocessed options are silently ignored
            optLen = *(pOptData->pOpt + 1);
            pOptData->pOpt += optLen + 2;
            pOptData->optSize -= optLen + 2;
        }
    }

    // if we got here there's an error
    return TCPIP_DHCP_OPTION_RES_FAIL;
}

static int _DHCPOptionProcessMsgType(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData)
{
    if(pOptData->optSize >= sizeof(TCPIP_DHCP_OPTION_DATA_MSG_TYPE))
    {
        TCPIP_DHCP_OPTION_DATA_MSG_TYPE* pMsgType = (TCPIP_DHCP_OPTION_DATA_MSG_TYPE*)pOptData->pOpt;

        if(pMsgType->len == sizeof(pMsgType->type))
        {
            pOptData->msgType = pMsgType->type;
            if(pOptData->msgType != TCPIP_DHCP_OFFER_MESSAGE || pClient->flags.bOfferReceived == 0)
            {
                return sizeof(TCPIP_DHCP_OPTION_DATA_MSG_TYPE);
            }
            // else throw away the packet if another offer when we already have one)
        }
    }

    return -1;
}

static int _DHCPOptionProcessSubNet(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData)
{
    if(pOptData->optSize >= sizeof(TCPIP_DHCP_OPTION_DATA_SUBNET_MASK))
    {
        TCPIP_DHCP_OPTION_DATA_SUBNET_MASK* pSubMask = (TCPIP_DHCP_OPTION_DATA_SUBNET_MASK*)pOptData->pOpt;

        if(pSubMask->len == sizeof(pSubMask->mask))
        {
            if(pClient->flags.bOfferReceived == 0)
            {   // the offered IP mask is needed
                memcpy(&pClient->dhcpMask, pSubMask->mask, sizeof(pClient->dhcpMask)); 
                pClient->validValues.Mask = 1;
            }
            return sizeof(TCPIP_DHCP_OPTION_DATA_SUBNET_MASK);
        }
    }

    return -1;
}

static int _DHCPOptionProcessRouter(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData)
{
    if(pOptData->optSize >= sizeof(TCPIP_DHCP_OPTION_DATA_ROUTER))
    {
        TCPIP_DHCP_OPTION_DATA_ROUTER* pRouter = (TCPIP_DHCP_OPTION_DATA_ROUTER*)pOptData->pOpt;
        if(pRouter->len >= sizeof(pRouter->gateway))
        {
            if(pClient->flags.bOfferReceived == 0)
            {   // the offered gateway IP is needed
                memcpy(&pClient->dhcpGateway, pRouter->gateway, sizeof(pClient->dhcpGateway)); 
                pClient->validValues.Gateway = 1;
            }
            return 2 + pRouter->len;
        }
    }

    return -1;
}

static int _DHCPOptionProcessDns(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData)
{
    if(pOptData->optSize >= sizeof(TCPIP_DHCP_OPTION_DATA_DNS1))
    {
        TCPIP_DHCP_OPTION_DATA_DNS* pDns = (TCPIP_DHCP_OPTION_DATA_DNS*)pOptData->pOpt;
        if((pDns->len & 0x3) == 0 && pDns->len >= sizeof(pDns->dns1))
        {
#if defined(TCPIP_STACK_USE_DNS)
            if(pClient->flags.bOfferReceived == 0)
            {   // the offered DNS IP is needed
                memcpy(&pClient->dhcpDNS, pDns->dns1, sizeof(pClient->dhcpDNS)); 
                pClient->validValues.DNS = 1;

                if(pDns->len >= sizeof(pDns->dns1) + sizeof(pDns->dns2))
                {   // 2nd DNS
                    memcpy(&pClient->dhcpDNS2, pDns->dns2, sizeof(pClient->dhcpDNS2)); 
                    pClient->validValues.DNS2 = 1;
                }
            }
#endif  // defined(TCPIP_STACK_USE_DNS)
            return 2 + pDns->len;
        }
    }

    return -1;
}

static int _DHCPOptionProcessSrvIdent(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData)
{
    if(pOptData->optSize >= sizeof(TCPIP_DHCP_OPTION_DATA_SERVER_ID))
    {
        TCPIP_DHCP_OPTION_DATA_SERVER_ID* pServId = (TCPIP_DHCP_OPTION_DATA_SERVER_ID*)pOptData->pOpt;
        if(pServId->len == sizeof(pServId->serverId))
        {   // update the ID
            memcpy( pOptData->serverID.v, pServId->serverId, sizeof(pServId->serverId));
            return sizeof(TCPIP_DHCP_OPTION_DATA_SERVER_ID);
        }
    }

    return -1;
}

static int _DHCPOptionProcessLease(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData)
{
    if(pOptData->optSize >= sizeof(TCPIP_DHCP_OPTION_DATA_LEASE_TIME))
    {
        TCPIP_DHCP_OPTION_DATA_LEASE_TIME* pLease = (TCPIP_DHCP_OPTION_DATA_LEASE_TIME*)pOptData->pOpt;
        if(pLease->len == sizeof(pLease->lease))
        {
            // update the lease
            pOptData->leaseTime.v[3] = pLease->lease[0];
            pOptData->leaseTime.v[2] = pLease->lease[1];
            pOptData->leaseTime.v[1] = pLease->lease[2];
            pOptData->leaseTime.v[0] = pLease->lease[3];
            return sizeof(TCPIP_DHCP_OPTION_DATA_LEASE_TIME);
        }
    }

    return -1;
}



static int _DHCPOptionProcessRenewT(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData)
{
    if(pOptData->optSize >= sizeof(TCPIP_DHCP_OPTION_DATA_RENEW_TIME))
    {
        TCPIP_DHCP_OPTION_DATA_RENEW_TIME* pRenew = (TCPIP_DHCP_OPTION_DATA_RENEW_TIME*)pOptData->pOpt;
        if(pRenew->len == sizeof(pRenew->tRenew))
        {
            // update the renew time
            pOptData->renewTime.v[3] = pRenew->tRenew[0];
            pOptData->renewTime.v[2] = pRenew->tRenew[1];
            pOptData->renewTime.v[1] = pRenew->tRenew[2];
            pOptData->renewTime.v[0] = pRenew->tRenew[3];
            return sizeof(TCPIP_DHCP_OPTION_DATA_RENEW_TIME);
        }
    }

    return -1;
}

static int _DHCPOptionProcessRebindT(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData)
{
    if(pOptData->optSize >= sizeof(TCPIP_DHCP_OPTION_DATA_REBIND_TIME))
    {
        TCPIP_DHCP_OPTION_DATA_REBIND_TIME* pRebind = (TCPIP_DHCP_OPTION_DATA_REBIND_TIME*)pOptData->pOpt;
        if(pRebind->len == sizeof(pRebind->tRebind))
        {
            // update the lease
            pOptData->rebindTime.v[3] = pRebind->tRebind[0];
            pOptData->rebindTime.v[2] = pRebind->tRebind[1];
            pOptData->rebindTime.v[1] = pRebind->tRebind[2];
            pOptData->rebindTime.v[0] = pRebind->tRebind[3];
            return sizeof(TCPIP_DHCP_OPTION_DATA_REBIND_TIME);
        }
    }

    return -1;
}


static int _DHCPOptionProcessEnd(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pOptData)
{
    return 0;   // end detected
}


// Process the received offer/data
static unsigned int _DHCPProcessReceiveData(DHCP_CLIENT_VARS* pClient, TCPIP_NET_IF* pNetIf)
{
    unsigned int recvRes;
    bool    dhcpRecvFail;
    TCPIP_DHCP_EVENT_TYPE dhcpEv;


#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAKE_TMO) != 0
    _DHCPDbgProcessTmo(pClient);
#endif  // TCPIP_DHCP_DEBUG_MASK

    if(TCPIP_UDP_GetIsReady(dhcpClientSocket) == 0)
    {
        return TCPIP_DHCP_TIMEOUT_MESSAGE;
    }

    dhcpRecvFail = false;
    dhcpEv = DHCP_EVENT_NONE;
    recvRes = _DHCPReceive(pClient, pNetIf);

    switch(recvRes)
    {
        case TCPIP_DHCP_ACK_MESSAGE:
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_LEASE_DECLINE) != 0
            if(_DHCPDbgProcessAckDecline(pClient, pNetIf))
            {
                dhcpRecvFail = true;
                dhcpEv = DHCP_EVENT_DECLINE;
                break;
            }
#endif  // TCPIP_DHCP_DEBUG_MASK

            if( pClient->smState == TCPIP_DHCP_GET_REQUEST_ACK)
            {
                if(pClient->validValues.IPAddress == 0 || pClient->validValues.Mask == 0)
                {   // having a new address without a valid mask seems weird
                    dhcpRecvFail = true;
                    dhcpEv = DHCP_EVENT_ACK_INVALID;
                    break;
                }

                // seems we received a new valid lease
                // make sure it's not in use
                IPV4_ADDR arpCheck;
                IPV4_ADDR zeroAdd = { 0 };
                arpCheck.Val = pClient->dhcpIPAddress.Val;
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAIL_ARP) != 0
                if(_dhcpDbgFakeArpAddress != 0)
                {
                    arpCheck.Val = _dhcpDbgFakeArpAddress;
                    // make sure it's not already cached
                    TCPIP_ARP_EntryRemove(pNetIf,  &arpCheck);
                }
#endif  // TCPIP_DHCP_DEBUG_MASK
                // not really  ARP_OPERATION_GRATUITOUS but only one single probe needs to go out
                TCPIP_ARP_Probe(pNetIf, &arpCheck, &zeroAdd, ARP_OPERATION_REQ | ARP_OPERATION_CONFIGURE |  ARP_OPERATION_GRATUITOUS);
                pClient->startWait = _DHCPSecondCountGet();
                _DHCPClientStateSet(pClient, TCPIP_DHCP_WAIT_LEASE_CHECK);
                dhcpEv = DHCP_EVENT_ACK;
            }
            else if( pClient->smState == TCPIP_DHCP_GET_RENEW_ACK || pClient->smState == TCPIP_DHCP_GET_REBIND_ACK)
            {
                pClient->tRequest = pClient->tRenewRequest;
                // update the bound state
                _DHCPSetBoundState(pClient);
                dhcpEv = DHCP_EVENT_BOUND;
            }

            break;

        case TCPIP_DHCP_NAK_MESSAGE:
            dhcpRecvFail = true;
            dhcpEv = DHCP_EVENT_NACK;
            break;

        default:
            // remain in the same state; do nothing
            break;
    }

    if(dhcpRecvFail)
    {
        _DHCPSetRunFail(pClient, TCPIP_DHCP_SEND_DISCOVERY, false);
    }

    if(dhcpEv != DHCP_EVENT_NONE)
    {
        _DHCPNotifyClients(pNetIf, dhcpEv);
    }

    return recvRes;
}

// a new valid lease has been obtained
// make it active
static void _DHCPSetNewLease(DHCP_CLIENT_VARS* pClient, TCPIP_NET_IF* pNetIf)
{
    IPV4_ADDR oldNetIp;
    IPV4_ADDR oldNetMask;

    oldNetIp.Val = TCPIP_STACK_NetAddressGet(pNetIf);
    oldNetMask.Val = TCPIP_STACK_NetMaskGet(pNetIf);

    _TCPIPStackSetConfigAddress(pNetIf, &pClient->dhcpIPAddress, &pClient->dhcpMask, false);
    if(pClient->validValues.Gateway)
    {
        TCPIP_STACK_GatewayAddressSet(pNetIf, &pClient->dhcpGateway);
    }
#if defined(TCPIP_STACK_USE_DNS)
    if(pNetIf->Flags.bIsDNSServerAuto != 0)
    {
        if(pClient->validValues.DNS)
        {
            TCPIP_STACK_PrimaryDNSAddressSet(pNetIf, &pClient->dhcpDNS);
        }
        if(pClient->validValues.DNS2)
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
    TCPIP_STACK_AddressServiceEvent(pNetIf, TCPIP_STACK_ADDRESS_SERVICE_DHCPC, TCPIP_STACK_ADDRESS_SERVICE_EVENT_RUN_RESTORE); 
    _DHCPDbgAddServiceEvent(pClient, TCPIP_STACK_ADDRESS_SERVICE_EVENT_RUN_RESTORE, 0);
    // inform other hosts of this host new address
    // probably this should be ARP_OPERATION_GRATUITOUS; but PROBE_ONLY doesn't pollute the cache
#if (_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING == 0)
    TCPIP_ARP_Probe(pNetIf, &pClient->dhcpIPAddress, &pClient->dhcpIPAddress, ARP_OPERATION_RESP | ARP_OPERATION_CONFIGURE | ARP_OPERATION_PROBE_ONLY);
#endif  // (_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING == 0)

    if((pClient->dhcpIPAddress.Val & pClient->dhcpMask.Val) != (oldNetIp.Val & oldNetMask.Val))
    {   // changed networks
        TCPIP_ARP_EntryRemoveNet(pNetIf, &oldNetIp, &oldNetMask, ARP_ENTRY_TYPE_ANY);
    } 

    _DHCPSetBoundState(pClient);

    _DHCPNotifyClients(pNetIf, DHCP_EVENT_BOUND);

}

static void _DHCPSetBoundState(DHCP_CLIENT_VARS* pClient)
{
    UDP_SOCKET_INFO sktInfo;

    // store the address of the server that gave us the lease
    TCPIP_UDP_SocketInfoGet(dhcpClientSocket , &sktInfo);
    pClient->serverAddress.Val = sktInfo.sourceIPaddress.v4Add.Val;

    _DHCPClientStateSet(pClient, TCPIP_DHCP_BOUND);
    pClient->flags.bIsBound = true;	
    pClient->flags.bWasBound = true;	
    pClient->flags.bReportFail = true; 
}


static void _DHCPSetLeaseTimeParams(DHCP_CLIENT_VARS* pClient, TCPIP_DHCP_OPTION_PROCESS_DATA* pDhcpData)
{
    uint32_t    leaseTime, t1Time, t2Time;

    // set lease time
    if(pDhcpData == 0 || pDhcpData->leaseTime.Val == 0)
    {
        leaseTime = TCPIP_DHCP_LEASE_TIME_DEFAULT;
    }
    else
    {
        leaseTime = pDhcpData->leaseTime.Val;
    }

    if(leaseTime < 2 * TCPIP_DHCP_LEASE_EXPIRE_FUZZ)
    {   // normally we shouldn't get such short leases
        leaseTime = 2 * TCPIP_DHCP_LEASE_EXPIRE_FUZZ;
    }

    // set renew time T1
    if(pDhcpData == 0 || pDhcpData->renewTime.Val == 0)
    {
        t1Time = leaseTime >> 1;
    }
    else
    {
        t1Time = pDhcpData->renewTime.Val;
    }

    // set rebind time T2
    if(pDhcpData == 0 || pDhcpData->rebindTime.Val == 0)
    {
        if(leaseTime >= 0xffffffff / 7)
        {
            t2Time = (leaseTime >> 3) * 7 ;
        }
        else
        {
            t2Time = (leaseTime * 7) >> 3 ;
        }
    }
    else
    {
        t2Time = pDhcpData->rebindTime.Val;
    }


    // apply random fuzz
    pClient->t1Seconds = (t1Time - TCPIP_DHCP_LEASE_EXPIRE_FUZZ) + SYS_RANDOM_PseudoGet() %  (2 * TCPIP_DHCP_LEASE_EXPIRE_FUZZ);
    pClient->t2Seconds = (t2Time - TCPIP_DHCP_LEASE_EXPIRE_FUZZ) + SYS_RANDOM_PseudoGet() %  (2 * TCPIP_DHCP_LEASE_EXPIRE_FUZZ);

    // finally set the expire seconds
    pClient->tExpSeconds = leaseTime;

}



/*****************************************************************************
  Function:
        static bool _DHCPSend(DHCP_CLIENT_VARS* pClient, TCPIP_NET_IF* pNetIf, int messageType, TCPIP_DHCP_OPERATION_FLAGS dhcpFlags)

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

static bool _DHCPSend(DHCP_CLIENT_VARS* pClient, TCPIP_NET_IF* pNetIf, int messageType, TCPIP_DHCP_OPERATION_FLAGS dhcpFlags)
{
    UDP_SOCKET s;
    IPV4_ADDR sourceIp;
    bool newTransaction;

    // the TCPIP_DHCP_FRAME_OPTIONS size is greater than
    // both TCPIP_DHCP_FRAME_HEADER and TCPIP_DHCP_FRAME_OPT_HEADER
    // use it for construction of the DHCP complete frame
    TCPIP_DHCP_FRAME_OPTIONS    dhcpOptions;

    TCPIP_DHCP_FRAME_HEADER*     pDhcpHdr;
    TCPIP_DHCP_FRAME_OPT_HEADER* pOptHdr;
    TCPIP_DHCP_FRAME_OPTIONS*    pDOpt;
    TCPIP_DHCP_OPTION_WRITE_DATA dhcpWriteData;

    s = dhcpClientSocket;
    if(TCPIP_UDP_PutIsReady(s) < TCPIP_DHCP_MIN_UDP_TX_AVLBL_SIZE)
    {   // not enough room in the buffer (?); shouldn't happen; 
        TCPIP_UDP_Flush(s); // discard old data that may be there
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TX_MSG_ENABLE) != 0
    _DHCPDebugTxDisplay(pClient, messageType, false);
#endif  // (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TX_MSG_ENABLE) != 0
        return false;
    }

    // bind to the proper interface so message uses correct IP address (request, for example)
    TCPIP_UDP_SocketNetSet(s, pNetIf);
    // allow receiving on any interface
    TCPIP_UDP_OptionsSet(s, UDP_OPTION_STRICT_NET, (void*)0);

    if((dhcpFlags & TCPIP_DHCP_FLAG_SEND_BCAST) != 0 || pClient->serverAddress.Val == 0)
    {   // Ensure that we transmit to the broadcast IP and MAC addresses
        // The UDP Socket remembers who it was last talking to
        TCPIP_UDP_BcastIPV4AddressSet(s, UDP_BCAST_NETWORK_LIMITED, pNetIf);
    }
    else
    {
        TCPIP_UDP_DestinationIPAddressSet(s, IP_ADDRESS_TYPE_IPV4, (IP_MULTI_ADDRESS*)&pClient->serverAddress);
    }

    if((dhcpFlags & TCPIP_DHCP_FLAG_SEND_ZERO_ADD) != 0)
    {   // Make sure we advertise a 0.0.0.0 IP address so all DHCP servers will respond.
        // A static IP address outside the DHCP server's scope it may be simply ignored.
        sourceIp.Val = 0;
    }    
    else
    {
        sourceIp.Val = _TCPIPStackNetAddress(pNetIf);
    }

    TCPIP_UDP_SourceIPAddressSet(s, IP_ADDRESS_TYPE_IPV4, (IP_MULTI_ADDRESS*) & sourceIp);

    newTransaction = (messageType == TCPIP_DHCP_DISCOVER_MESSAGE || messageType == TCPIP_DHCP_REQUEST_RENEW_MESSAGE || (messageType == TCPIP_DHCP_REQUEST_MESSAGE && pClient->dhcpOp == TCPIP_DHCP_OPER_INIT_REBOOT));
    if (newTransaction)
    {
        // generate a new transaction ID
        pClient->transactionID.Val = SYS_RANDOM_PseudoGet(); 
        // Reset offered flag so we know to act upon the next valid offer
        pClient->flags.bOfferReceived = false;
    }

    pDhcpHdr = (TCPIP_DHCP_FRAME_HEADER*)&dhcpOptions;

    memset(pDhcpHdr, 0, sizeof(*pDhcpHdr));
    pDhcpHdr->op = TCPIP_BOOT_REQUEST;
    pDhcpHdr->htype = TCPIP_BOOT_HW_TYPE;
    pDhcpHdr->hlen = TCPIP_BOOT_LEN_OF_HW_TYPE;
    pDhcpHdr->xid = pClient->transactionID.Val;
    // set ciaddr
    // If this is DHCP REQUEST message, use previously allocated IP address.
    if( messageType == TCPIP_DHCP_RELEASE_MESSAGE || messageType == TCPIP_DHCP_REQUEST_RENEW_MESSAGE) 
    {
        pDhcpHdr->ciaddr = pClient->dhcpIPAddress.Val;
    }
    // set chaddr - Client hardware address.
    memcpy(pDhcpHdr->chaddr, _TCPIPStack_NetMACAddressGet(pNetIf), sizeof(TCPIP_MAC_ADDR));
    // write the DHCP header
    TCPIP_UDP_ArrayPut(s, (uint8_t*)pDhcpHdr, sizeof (*pDhcpHdr));

    // write the DHCP optional header
    pOptHdr = (TCPIP_DHCP_FRAME_OPT_HEADER*)&dhcpOptions;
    memset(pOptHdr, 0, sizeof(*pOptHdr));
    TCPIP_UDP_ArrayPut(s, (uint8_t*)pOptHdr, sizeof(*pOptHdr));

    // set the options themselves
    pDOpt = &dhcpOptions;
    memset(pDOpt, 0, sizeof(*pDOpt));

    // set the cookie
    memcpy(pDOpt->cookie, dhcpMagicCookie, sizeof(pDOpt->cookie)); 

    // start writing the options
    dhcpWriteData.pOpt = pDOpt->options;
    dhcpWriteData.writeSpace = TCPIP_UDP_PutIsReady(s) - sizeof(pDOpt->cookie);
    dhcpWriteData.writeBytes = sizeof(pDOpt->cookie);
    dhcpWriteData.msgType = messageType;

    if(_DHCPOptionsWrite(pNetIf, &dhcpWriteData) != TCPIP_DHCP_OPTION_RES_OK)
    {   // failed
        // clear the Tx buffer, don't send ill formatted packets
        TCPIP_UDP_TxOffsetSet(s , 0, false);
        return false;
    }

    // Add zero padding to ensure compatibility with old BOOTP relays
    // that may discard small packets
    if(sizeof (*pDhcpHdr) + sizeof(*pOptHdr) + dhcpWriteData.writeBytes < TCPIP_DHCP_MIN_TX_FRAME_SIZE)
    {
        dhcpWriteData.writeBytes = TCPIP_DHCP_MIN_TX_FRAME_SIZE - sizeof (*pDhcpHdr) - sizeof(*pOptHdr);
    }

    // write the data and send it
    TCPIP_UDP_ArrayPut(s, pDOpt->cookie, dhcpWriteData.writeBytes);
    TCPIP_UDP_Flush(s);
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TX_MSG_ENABLE) != 0
    _DHCPDebugTxDisplay(pClient, messageType, true);
#endif  // (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TX_MSG_ENABLE) != 0

    return true;
}


static TCPIP_DHCP_OPTION_RESULT _DHCPOptionsWrite(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData)
{
    int     optIx;
    int     optSize;

    const TCPIP_DHCP_OPTION_WRITE_ENTRY* pEntry = _DHCPOptWriteTbl;
    for(optIx = 0; optIx < sizeof(_DHCPOptWriteTbl) / sizeof(*_DHCPOptWriteTbl); optIx++, pEntry++)
    {
        optSize = (*pEntry->dhcpWriteFnc)(pNetIf, pSendData);
        if(optSize < 0)
        {   // failed
            return TCPIP_DHCP_OPTION_RES_FAIL;
        }

        pSendData->writeSpace -= optSize;
        pSendData->writeBytes += optSize;
        pSendData->pOpt += optSize;
    }

    return TCPIP_DHCP_OPTION_RES_OK;
}

static int _DHCPOptionWriteMsgType(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData)
{
    TCPIP_DHCP_OPTION_DATA_MSG_TYPE* pDhcpOptType;

    if(pSendData->writeSpace >= sizeof(*pDhcpOptType))
    {
        pDhcpOptType = (TCPIP_DHCP_OPTION_DATA_MSG_TYPE*)pSendData->pOpt;
        pDhcpOptType->opt = TCPIP_DHCP_MESSAGE_TYPE;
        pDhcpOptType->len = sizeof(pDhcpOptType->type);
        // TCPIP_DHCP_REQUEST_RENEW_MESSAGE is just an internal message code
        pDhcpOptType->type = (pSendData->msgType == TCPIP_DHCP_REQUEST_RENEW_MESSAGE) ? TCPIP_DHCP_REQUEST_MESSAGE : (uint8_t)pSendData->msgType;
        return sizeof(*pDhcpOptType);
    }
    
    return -1;
}

static int _DHCPOptionWriteSrvIdent(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData)
{
    TCPIP_DHCP_OPTION_DATA_SERVER_ID* pSrvId;
    unsigned int msgType = pSendData->msgType;
    DHCP_CLIENT_VARS* pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf); 
 
    if( (msgType == TCPIP_DHCP_DECLINE_MESSAGE) ||  (msgType == TCPIP_DHCP_RELEASE_MESSAGE) ||
            (msgType == TCPIP_DHCP_REQUEST_MESSAGE && pClient->dhcpOp != TCPIP_DHCP_OPER_INIT_REBOOT) )
    {   // need to use a server ID
        if(pSendData->writeSpace >= sizeof(*pSrvId))
        {
            pSrvId = (TCPIP_DHCP_OPTION_DATA_SERVER_ID*)pSendData->pOpt;
            pSrvId->opt = TCPIP_DHCP_SERVER_IDENTIFIER;
            pSrvId->len = sizeof(pSrvId->serverId);
            memcpy(pSrvId->serverId, &pClient->dwServerID, sizeof(pSrvId->serverId));
            return sizeof(*pSrvId);
        }

        // not enough room?
        return -1;
    }

    // no need to write the server ID
    return 0;
}

static int _DHCPOptionWriteParamRequest(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData)
{
    TCPIP_DHCP_OPTION_DATA_REQUEST_LIST* pReqL;

    int nOptions = sizeof(_DHCPRequestParamsTbl) / sizeof(*_DHCPRequestParamsTbl);

    if(pSendData->writeSpace >= sizeof(*pReqL) + nOptions)
    {
        pReqL = (TCPIP_DHCP_OPTION_DATA_REQUEST_LIST*)pSendData->pOpt;
        pReqL->opt = TCPIP_DHCP_PARAM_REQUEST_LIST;
        pReqL->len = (uint8_t)nOptions;
        memcpy(pReqL->optList, _DHCPRequestParamsTbl, nOptions);
        return sizeof(*pReqL) + nOptions;
    }

    // not enough room?
    return -1;
}

static int _DHCPOptionWriteIPRequest(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData)
{
    TCPIP_DHCP_OPTION_DATA_REQUEST_IP_ADDRESS* pReqAdd;
    unsigned int msgType = pSendData->msgType;
    DHCP_CLIENT_VARS* pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf); 

    if ( (msgType == TCPIP_DHCP_DECLINE_MESSAGE) || (msgType == TCPIP_DHCP_REQUEST_MESSAGE) ||  
            ((msgType == TCPIP_DHCP_DISCOVER_MESSAGE) && pClient->dhcpIPAddress.Val))
    {   // Add requested IP address to DHCP Request Message
        if(pSendData->writeSpace >= sizeof(*pReqAdd))
        {
            pReqAdd = (TCPIP_DHCP_OPTION_DATA_REQUEST_IP_ADDRESS*)pSendData->pOpt;
            pReqAdd->opt = TCPIP_DHCP_PARAM_REQUEST_IP_ADDRESS;
            pReqAdd->len = sizeof(pReqAdd->reqIpAddr);
            memcpy(pReqAdd->reqIpAddr, pClient->dhcpIPAddress.v, sizeof(pReqAdd->reqIpAddr));
            return sizeof(*pReqAdd);
        }

        // not enough room?
        return -1;
    }

    // no need to request the IP address
    return 0;
}

static int _DHCPOptionHostName(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData)
{
    int hostLen;
    TCPIP_DHCP_OPTION_DATA_HOST_NAME* pHost;
    DHCP_CLIENT_VARS    *pClient;
    char*   cbName;
    char    hostName[TCPIP_DHCP_HOST_NAME_SIZE + 1];
    int     writeBackLen = 0;
    char*   srcName = 0;

    // get a host name
    pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf);

    if(pClient->nameCallback != 0)
    {
        if((cbName = (*pClient->nameCallback)(pNetIf)) != 0)
        {
            if(strlen(cbName) >= 2)
            {   // valid string
                srcName = cbName;
                if(pClient->flags.bWriteBack != 0)
                {
                    writeBackLen = strlen(cbName) + 1;
                }
            }
        }
    }

    if(srcName == 0)
    {   // use the default NBNS name
        srcName = (char*)TCPIP_STACK_NetBIOSName(pNetIf);
    }

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAKE_HOST_NAME) != 0
    // override
    if(_dhcpDbgHostName != 0)
    {
        srcName = _dhcpDbgHostName;
    }
#endif  // TCPIP_DHCP_DEBUG_MASK

    hostLen = _DHCPFormatHostName(hostName, srcName, sizeof(hostName));

    if(writeBackLen)
    {
        srcName[0] = 0;
    }

    if(hostLen == 0)
    {   // no valid host name; skip option
        return 0;
    }

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAKE_HOST_NAME) != 0
    SYS_CONSOLE_PRINT("DHCP cli: %d, host format Name: %s\r\n", pClient - DHCPClients, hostName);
#endif  // TCPIP_DHCP_DEBUG_MASK

    if(pSendData->writeSpace >= sizeof(*pHost) + hostLen)
    {
        pHost = (TCPIP_DHCP_OPTION_DATA_HOST_NAME*)pSendData->pOpt;
        pHost->opt = TCPIP_DHCP_HOST_NAME;
        pHost->len = hostLen;
        memcpy(pHost->hostname, hostName, hostLen);

        if(writeBackLen)
        {
            if(writeBackLen > hostLen + 1)
            {
                writeBackLen = hostLen + 1;
            }
            memcpy(srcName, hostName, writeBackLen);
            srcName[writeBackLen - 1] = 0;
        }

        return sizeof(*pHost) + hostLen;
    }

    // not enough room?
    return -1;
}

static int _DHCPFormatHostName(char* destBuffer, const char* srcBuffer, int destSize)
{
    int     ix, len;
    char*   d;
    char    currC;

    memset(destBuffer, 0x0, destSize);

    // strip spaces
    d = destBuffer;
    for(ix = 0; ix < destSize - 1; ix++)
    {
        currC = *srcBuffer++;
        if(!isspace(currC))
        {
            *d++ = currC;
        }
    }

    len = strlen(destBuffer);

    if(len)
    {
        d = destBuffer;
        for(ix = 0; ix < len; ix++)
        {
            currC = *d;
            if(currC != '-' && currC != '.' && !isalnum(currC))
            {
                *d = TCPIP_DHCP_HOST_REPLACE_CHAR;
            }
            d++;
        }

        // finally check the 1st and last chars
        currC = destBuffer[0];
        if(!isalpha(currC))
        {
            destBuffer[0] = TCPIP_DHCP_HOST_REPLACE_CHAR;
        }
        currC = destBuffer[len - 1];
        if(!isalnum(currC))
        {
            destBuffer[len - 1] = TCPIP_DHCP_HOST_REPLACE_CHAR;
        }
    }

    return len;
}

                
static int _DHCPOptionClientId(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData)
{
    TCPIP_DHCP_OPTION_DATA_CLIENT_ID* pClientOpt;

    if(pSendData->writeSpace >= sizeof(*pClientOpt))
    {
        pClientOpt = (TCPIP_DHCP_OPTION_DATA_CLIENT_ID*)pSendData->pOpt;
        pClientOpt->opt = TCPIP_DHCP_PARAM_REQUEST_CLIENT_ID;
        pClientOpt->len = sizeof(pClientOpt->cliId);
        pClientOpt->cliId.type =  TCPIP_BOOT_HW_TYPE;
        memcpy(pClientOpt->cliId.id, _TCPIPStack_NetMACAddressGet(pNetIf), sizeof(pClientOpt->cliId.id) - 2);
        uint16_t netIx = TCPIP_STACK_NetIxGet(pNetIf);
        pClientOpt->cliId.id[sizeof(pClientOpt->cliId.id) - 2] = (uint8_t)(netIx >> 8);
        pClientOpt->cliId.id[sizeof(pClientOpt->cliId.id) - 1] = (uint8_t)netIx;
        return sizeof(*pClientOpt);
    }

    // not enough room?
    return -1;

}

                      
static int _DHCPOptionWriteEnd(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_OPTION_WRITE_DATA* pSendData)
{
    TCPIP_DHCP_OPTION_DATA_END* pEnd;
    if(pSendData->writeSpace >= sizeof(*pEnd))
    {
        pEnd = (TCPIP_DHCP_OPTION_DATA_END*)pSendData->pOpt;
        pEnd->opt = TCPIP_DHCP_END_OPTION;
        return sizeof(*pEnd);
    }

    return -1;
}


// DHCP connection handler
//
void TCPIP_DHCP_ConnectionHandler(TCPIP_NET_IF* pNetIf, TCPIP_MAC_EVENT connEvent)
{
    DHCP_CLIENT_VARS* pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf);

    if (pClient->flags.bDHCPEnabled != 0)
    {
        if (connEvent & TCPIP_MAC_EV_CONN_LOST)
        {
            // let it wait for the connection
            _DHCPClientClose(pNetIf, false, false);
            _TCPIPStackSetConfigAddress(pNetIf, 0, 0, true);
            TCPIP_STACK_AddressServiceEvent(pNetIf, TCPIP_STACK_ADDRESS_SERVICE_DHCPC, TCPIP_STACK_ADDRESS_SERVICE_EVENT_CONN_LOST);
            _DHCPDbgAddServiceEvent(pClient, TCPIP_STACK_ADDRESS_SERVICE_EVENT_CONN_LOST, 0);
            _DHCPNotifyClients(pNetIf, DHCP_EVENT_CONN_LOST);
        }
        else if (connEvent & TCPIP_MAC_EV_CONN_ESTABLISHED)
        {
            // put it in wait connection mode
            // should be in this state anyway
            // but just in case we've missed the link down event
            _DHCPEnable(pNetIf, pClient->flags.bWasBound ? TCPIP_DHCP_OPER_INIT_REBOOT : TCPIP_DHCP_OPER_INIT);
            _DHCPNotifyClients(pNetIf, DHCP_EVENT_CONN_ESTABLISHED);
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
    if(handler && dhcpMemH)
    {
        TCPIP_DHCP_LIST_NODE* newNode = (TCPIP_DHCP_LIST_NODE*)TCPIP_Notification_Add(&dhcpRegisteredUsers, dhcpMemH, sizeof(*newNode));
        if(newNode)
        {
            newNode->handler = handler;
            newNode->hParam = hParam;
            newNode->hNet = hNet;
            return newNode;
        }
    }

    return 0;
}

// deregister the event handler
bool TCPIP_DHCP_HandlerDeRegister(TCPIP_DHCP_HANDLE hDhcp)
{
    if(hDhcp && dhcpMemH)
    {
        if(TCPIP_Notification_Remove((SGL_LIST_NODE*)hDhcp, &dhcpRegisteredUsers, dhcpMemH))
        {
            return true;
        }
    }

    return false;
}

static void _DHCPNotifyClients(TCPIP_NET_IF* pNetIf, TCPIP_DHCP_EVENT_TYPE evType)
{
    TCPIP_DHCP_LIST_NODE* dNode;
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_LEASE_EVENTS) != 0 || (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_CONN_EVENTS) != 0
    const char* message;
#endif  // (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_LEASE_EVENTS) != 0 || (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_CONN_EVENTS) != 0


    TCPIP_Notification_Lock(&dhcpRegisteredUsers);
    for(dNode = (TCPIP_DHCP_LIST_NODE*)dhcpRegisteredUsers.list.head; dNode != 0; dNode = dNode->next)
    {
        if(dNode->hNet == 0 || dNode->hNet == pNetIf)
        {   // trigger event
            (*dNode->handler)(pNetIf, evType, dNode->hParam);
        }
    }
    TCPIP_Notification_Unlock(&dhcpRegisteredUsers);

#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_LEASE_EVENTS) != 0
    if(evType != DHCP_EVENT_CONN_LOST && evType != DHCP_EVENT_CONN_ESTABLISHED)
    {
        switch (evType)
        {
            case DHCP_EVENT_DISCOVER:
                message = "dcv";
                break;
            case DHCP_EVENT_REQUEST:
                message = "req";
                break;
            case DHCP_EVENT_ACK:
                message = "ack";
                break;
            case DHCP_EVENT_ACK_INVALID:
                message = "inv";
                break;
            case DHCP_EVENT_DECLINE:
                message = "dcl";
                break;
            case DHCP_EVENT_NACK:
                message = "nak";
                break;
            case DHCP_EVENT_TIMEOUT:
                message = "tmo";
                break;
            case DHCP_EVENT_BOUND:
                message = "bnd";
                break;
            case DHCP_EVENT_REQUEST_RENEW:
                message = "ren";
                break;
            case DHCP_EVENT_REQUEST_REBIND:
                message = "rbn";
                break;
            case DHCP_EVENT_SERVICE_DISABLED:
                message = "off";
                break;
            default:
                message = "unk";
                break;
        }
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TIME_RES_MS) != 0
        SYS_CONSOLE_PRINT("DHCP cli: %d event: %s, time: %d.%d\r\n", TCPIP_STACK_NetIndexGet(pNetIf), message, _DHCPSecondCountGet(), _DHCPMillisecCountGet());
#else
        SYS_CONSOLE_PRINT("DHCP cli: %d event: %s, time: %d\r\n", TCPIP_STACK_NetIndexGet(pNetIf), message, _DHCPSecondCountGet());
#endif  // (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TIME_RES_MS) != 0
    }
#endif  // (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_LEASE_EVENTS) != 0


#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_CONN_EVENTS) != 0
    if(evType == DHCP_EVENT_CONN_LOST)
    {
        message = "lost";
    }
    else if(evType == DHCP_EVENT_CONN_ESTABLISHED)
    {
        message = "est";
    }
    else
    {
        message = 0;
    }

    if(message)
    {
#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TIME_RES_MS) != 0
        SYS_CONSOLE_PRINT("DHCP cli: %d connection: %s, time: %d.%d\r\n", TCPIP_STACK_NetIndexGet(pNetIf), message, _DHCPSecondCountGet(), _DHCPMillisecCountGet());
#else
        SYS_CONSOLE_PRINT("DHCP cli: %d connection: %s, time: %d\r\n", TCPIP_STACK_NetIndexGet(pNetIf), message, _DHCPSecondCountGet());
#endif  // (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_TIME_RES_MS) != 0
    }

#endif


}


// enable unicast DHCP packets
// this should be an IPv4 packet
static bool _DHCPPacketFilter(TCPIP_MAC_PACKET* pRxPkt, const void* param)
{
    TCPIP_MAC_ADDR bcastAdd = { {0xff, 0xff, 0xff, 0xff, 0xff, 0xff} };
    TCPIP_MAC_ETHERNET_HEADER* macHdr = (TCPIP_MAC_ETHERNET_HEADER*)pRxPkt->pMacLayer;
    const uint8_t* netMacAddr = TCPIP_STACK_NetMACAddressGet((TCPIP_NET_IF*)pRxPkt->pktIf);

    if(netMacAddr)
    {
        if(memcmp(netMacAddr, macHdr->DestMACAddr.v, sizeof(macHdr->DestMACAddr)) == 0 || 
           memcmp(netMacAddr, bcastAdd.v, sizeof(bcastAdd)) == 0 )
        {   // unicast to me;
            // allow also broadcast because some DHCP servers seem to ignore the unicast request
            IPV4_HEADER* pHeader = (IPV4_HEADER*)pRxPkt->pNetLayer;
            if(pHeader->Protocol == IP_PROT_UDP)
            {   // UDP packet
                UDP_HEADER* pUDPHdr = (UDP_HEADER*)pRxPkt->pTransportLayer;
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


static UDP_SOCKET _DHCPOpenSocket(void)
{
    uint16_t txBuffSize;
    TCPIP_UDP_SIGNAL_HANDLE sigHandle;

    UDP_SOCKET dhcpSkt = TCPIP_UDP_OpenClientSkt(IP_ADDRESS_TYPE_IPV4, dhcpServerPort, 0, UDP_OPEN_CLIENT | UDP_OPEN_CONFIG_SERVICE);

    bool udpSuccess = false;
    while(dhcpSkt != INVALID_UDP_SOCKET)
    {   
        // make sure the socket is created with enough TX space
        TCPIP_UDP_OptionsGet(dhcpSkt, UDP_OPTION_TX_BUFF, (void*)&txBuffSize);
        if(txBuffSize < TCPIP_DHCP_MIN_UDP_TX_BUFFER_SIZE)
        {
            txBuffSize = TCPIP_DHCP_MIN_UDP_TX_BUFFER_SIZE;
            if(!TCPIP_UDP_OptionsSet(dhcpSkt, UDP_OPTION_TX_BUFF, (void*)&txBuffSize))
            {
                break;
            }
        }

        // bind to the DHCP local port
        if(!TCPIP_UDP_Bind(dhcpSkt, IP_ADDRESS_TYPE_IPV4, dhcpClientPort,  0))
        {
            break;
        }

        // allow receiving from any server on this interface
        TCPIP_UDP_OptionsSet(dhcpSkt, UDP_OPTION_STRICT_ADDRESS, 0);

        sigHandle = TCPIP_UDP_SignalHandlerRegister(dhcpSkt, TCPIP_UDP_SIGNAL_RX_DATA, _DHCPSocketRxSignalHandler, 0);
        if(sigHandle == 0)
        {
            break;
        }

        udpSuccess = true;
        break;
    }



    if(!udpSuccess && dhcpSkt != INVALID_UDP_SOCKET)
    {
        TCPIP_UDP_Close(dhcpSkt);
        dhcpSkt = INVALID_UDP_SOCKET;
    }

    return dhcpSkt;
}

bool TCPIP_DHCP_InfoGet(TCPIP_NET_HANDLE hNet, TCPIP_DHCP_INFO* pDhcpInfo)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(hNet);
    if(DHCPClients && pNetIf && pDhcpInfo)
    {
        DHCP_CLIENT_VARS* pClient = DHCPClients + TCPIP_STACK_NetIxGet(pNetIf);

        memset(pDhcpInfo, 0, sizeof(*pDhcpInfo));

        pDhcpInfo->dhcpTime = _DHCPSecondCountGet();
        if(pClient->flags.bDHCPEnabled == true)
        {
            if((pDhcpInfo->status = pClient->smState) >= TCPIP_DHCP_BOUND)
            {
                pDhcpInfo->leaseStartTime = pClient->tRequest;
                pDhcpInfo->leaseDuration = pClient->tExpSeconds;
                pDhcpInfo->renewTime = pClient->tRequest + pClient->t1Seconds;
                pDhcpInfo->rebindTime = pClient->tRequest + pClient->t2Seconds;
                pDhcpInfo->dhcpAddress.Val = pClient->dhcpIPAddress.Val;
                pDhcpInfo->serverAddress.Val = pClient->serverAddress.Val;
#if defined TCPIP_DHCP_STORE_BOOT_FILE_NAME
                pDhcpInfo->bootFileName = (const char*)pClient->bootFileName;
#else
                pDhcpInfo->bootFileName = 0;
#endif // defined TCPIP_DHCP_STORE_BOOT_FILE_NAME
            }
        }
        
        return true;
    }

    return false;

}

// checks and reports loss of lease
static void _DHCPCheckRunFailEvent(TCPIP_NET_IF* pNetIf, DHCP_CLIENT_VARS* pClient)
{

    if(pClient->smState > TCPIP_DHCP_WAIT_LINK && pClient->smState < TCPIP_DHCP_BOUND)
    {   // don't have a current lease
        if(pClient->flags.bReportFail && pClient->tOpStart != 0)
        {
            if((_DHCPSecondCountGet() - pClient->tOpStart) >= pClient->tOpFailTmo)
            {   // initialization time out
                TCPIP_STACK_AddressServiceEvent(pNetIf, TCPIP_STACK_ADDRESS_SERVICE_DHCPC, TCPIP_STACK_ADDRESS_SERVICE_EVENT_RUN_FAIL);
                _DHCPDbgAddServiceEvent(pClient, TCPIP_STACK_ADDRESS_SERVICE_EVENT_RUN_FAIL, "init tmo");
                pClient->flags.bReportFail = 0;   // reported
            }
        }
    }
}


bool TCPIP_DHCP_HostNameCallbackRegister(TCPIP_NET_HANDLE hNet, TCPIP_DHCP_HOST_NAME_CALLBACK nameCallback, bool writeBack)
{
    int     startIx, stopIx, ix;
    TCPIP_NET_IF        *pNetIf;
    DHCP_CLIENT_VARS    *pClient;

    if(!DHCPClients)
    {   // not initialized?
        return false;
    }

    if(hNet == 0)
    {
        startIx = 0;
        stopIx = dhcpInterfaces;
    }
    else if(!(pNetIf = _TCPIPStackHandleToNet(hNet)))
    {   // invalid interface
        return false;
    }
    else
    {
        startIx = TCPIP_STACK_NetIxGet(pNetIf);
        stopIx = startIx + 1;
    }


    pClient = DHCPClients + startIx;
    for(ix = startIx; ix < stopIx; ix++, pClient++)
    {
        pClient->nameCallback = nameCallback;
        pClient->flags.bWriteBack = writeBack;
    }

    return true;

}



#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_FAKE_TMO) != 0
static void _DHCPDbgProcessTmo(DHCP_CLIENT_VARS* pClient)
{
    if(!TCPIP_UDP_GetIsReady(dhcpClientSocket))
    {   // no data anyway
        return;
    }

    bool discardPkt = false;

    switch(pClient->smState)
    {
        case TCPIP_DHCP_GET_OFFER:
            if(_dhcpDbgGetOfferTmoCnt != 0)
            {
                _dhcpDbgGetOfferTmoCnt--;
                discardPkt = true;
            }
            break;

        case TCPIP_DHCP_GET_REQUEST_ACK:
            if(_dhcpDbgGetRequestTmoCnt != 0)
            {
                _dhcpDbgGetRequestTmoCnt--;
                discardPkt = true;
            }
            break;

        case TCPIP_DHCP_GET_RENEW_ACK:
            if(_dhcpDbgGetRenewTmoCnt != 0)
            {
                _dhcpDbgGetRenewTmoCnt--;
                discardPkt = true;
            }
            break;

        case TCPIP_DHCP_GET_REBIND_ACK:
            if(_dhcpDbgGetRebindTmoCnt != 0)
            {
                _dhcpDbgGetRebindTmoCnt--;
                discardPkt = true;
            }
            break;

        default:
            break;
    }

    if(discardPkt)
    {
        do
        {
            TCPIP_UDP_Discard(dhcpClientSocket);
        }while(TCPIP_UDP_GetIsReady(dhcpClientSocket));
    }
}
#endif  // TCPIP_DHCP_DEBUG_MASK



#if (TCPIP_DHCP_DEBUG_MASK & TCPIP_DHCP_DEBUG_MASK_LEASE_DECLINE) != 0

static bool _DHCPDbgProcessAckDecline(DHCP_CLIENT_VARS* pClient, TCPIP_NET_IF* pNetIf)
{
    bool decline = false;

    switch(pClient->smState)
    {
        case TCPIP_DHCP_GET_REQUEST_ACK:
            if(_dhcpDbgLeaseRequestDeclineCnt != 0)
            {
                _dhcpDbgLeaseRequestDeclineCnt--;
                decline = true;
            }
            break;

        case TCPIP_DHCP_GET_RENEW_ACK:
            if(_dhcpDbgLeaseRenewDeclineCnt != 0)
            {
                _dhcpDbgLeaseRenewDeclineCnt--;
                decline = true;
            }
            break;

        case TCPIP_DHCP_GET_REBIND_ACK:
            if(_dhcpDbgLeaseRebindDeclineCnt != 0)
            {
                _dhcpDbgLeaseRebindDeclineCnt--;
                decline = true;
            }
            break;

        default:
            break;
    }

    if(decline)
    {
        _DHCPSend(pClient, pNetIf, TCPIP_DHCP_DECLINE_MESSAGE, TCPIP_DHCP_FLAG_SEND_ZERO_ADD | TCPIP_DHCP_FLAG_SEND_BCAST);
    }

    return decline;
}

#endif  // TCPIP_DHCP_DEBUG_MASK



#endif	//#if defined(TCPIP_STACK_USE_DHCP_CLIENT)
#endif  // defined(TCPIP_STACK_USE_IPV4)


