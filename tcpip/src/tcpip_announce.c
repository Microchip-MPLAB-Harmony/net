/*******************************************************************************
  TCPIP Announce Client and Server

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    - Provides device hostname and IP address discovery on a local 
      Ethernet subnet (same broadcast domain)
    - Reference: None.  Hopefully AN833 in the future.
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








#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_ANNOUNCE

#include "tcpip/src/tcpip_private.h"

#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_ANNOUNCE)

// enable Announce debugging features
#define TCPIP_ANNOUNCE_DEBUG_MASK_NOTIFY_EVENTS     0x01    // enable notification events display
#define TCPIP_ANNOUNCE_DEBUG_MASK_SEND_EVENTS       0x02    // enable send events display

#define TCPIP_ANNOUNCE_DEBUG_MASK                   (0)


typedef struct
{
    UDP_SOCKET  skt;        // associated socket
    uint16_t    txBuffSize; // size of the socket/announce TX buffer
    uint16_t    nIfs;    // number of interfaces running on
    uint16_t    currIx;     // current interface number when counting a failing sequence
    UDP_SOCKET_BCAST_TYPE bcastType;   // type of broadcast currently used
    uint32_t    requestMask;    // request mask per interface: bit x for interface x
    uint32_t    readyMask;      // ready mask per interface: bit x for interface x
    const void* memH;           // memory handle
    TCPIP_SIGNAL_HANDLE   sigHandle;      // asynchronous handle
    TCPIP_ANNOUNCE_MESSAGE_CALLBACK usrCback;   // user callback
#if defined (TCPIP_STACK_USE_DHCP_CLIENT)
    TCPIP_DHCP_HANDLE dhcpHandler;
#endif
#if defined (TCPIP_STACK_USE_IPV6)
    IPV6_HANDLE ipv6Handler;
#endif
}TCPIP_ANNOUNCE_DCPT;

// event that triggered an announce message
typedef enum
{
    TCPIP_ANNOUNCE_EVENT_IPV4_DHCP   = 0,   // IPv4 DHCP lease occurred
    TCPIP_ANNOUNCE_EVENT_IPV6_ADDRESS,      // IPv6 event address
    TCPIP_ANNOUNCE_EVENT_REMOTE_REQUEST,    // some host on the network requested the message
    TCPIP_ANNOUNCE_EVENT_USER_REQUEST,      // user requested the message
}TCPIP_ANNOUNCE_EVENT_TYPE;
    
static TCPIP_ANNOUNCE_DCPT announceDcpt;


static int                  announceInitCount = 0;      // module initialization count

// a minimum message that should be always supported
typedef struct __attribute__((packed))
{
    uint8_t     type;       // TCPIP_ANNOUNCE_FIELD_MAC_NAME
    char        macName[10];// MAC interface name, ASCII;
                            // at least 10 characters space guaranteed 
    char        separator[2];   // CRLF terminator
}TCPIP_ANNOUNCE_MIN_MAC_NAME_DCPT;


typedef struct __attribute__((packed))
{
    uint8_t     type;           // TCPIP_ANNOUNCE_FIELD_HOST_NAME
    char        hostName[16];   // interface host name, ASCII
                                // at least 16 characters space guaranteed 
    char        separator[2];   // CRLF terminator
}TCPIP_ANNOUNCE_MIN_HOST_NAME_DCPT;



#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_IPV6)
typedef struct __attribute__((packed))
{
    TCPIP_ANNOUNCE_TRUNC_DCPT           truncDcpt; 
    TCPIP_ANNOUNCE_MAC_ADDR_DCPT        macAddDcpt;
    TCPIP_ANNOUNCE_MIN_MAC_NAME_DCPT    macNameDcpt;
    TCPIP_ANNOUNCE_MIN_HOST_NAME_DCPT   hostNameDcpt;
    TCPIP_ANNOUNCE_IPV4_ADDR_DCPT       ipv4AddDcpt;
    TCPIP_ANNOUNCE_IPV6_ADDR_DCPT       ipv6UnicastDcpt[4];
}S_TCPIP_ANNOUNCE_MIN_MESSAGE;
#elif defined(TCPIP_STACK_USE_IPV6)
typedef struct __attribute__((packed))
{
    TCPIP_ANNOUNCE_TRUNC_DCPT           truncDcpt; 
    TCPIP_ANNOUNCE_MAC_ADDR_DCPT        macAddDcpt;
    TCPIP_ANNOUNCE_MIN_MAC_NAME_DCPT    macNameDcpt;
    TCPIP_ANNOUNCE_MIN_HOST_NAME_DCPT   hostNameDcpt;
    TCPIP_ANNOUNCE_IPV6_ADDR_DCPT       ipv6UnicastDcpt[6];
}S_TCPIP_ANNOUNCE_MIN_MESSAGE;
#else
typedef struct __attribute__((packed))
{
    TCPIP_ANNOUNCE_TRUNC_DCPT           truncDcpt; 
    TCPIP_ANNOUNCE_MAC_ADDR_DCPT        macAddDcpt;
    TCPIP_ANNOUNCE_MIN_MAC_NAME_DCPT    macNameDcpt;
    TCPIP_ANNOUNCE_MIN_HOST_NAME_DCPT   hostNameDcpt;
    TCPIP_ANNOUNCE_IPV4_ADDR_DCPT       ipv4AddDcpt;
}S_TCPIP_ANNOUNCE_MIN_MESSAGE;
#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_IPV6)


#define M_ANNOUNCE_SEPARATOR_1   0x0d
#define M_ANNOUNCE_SEPARATOR_2   0x0a

// prototypes
#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void             F_TCPIP_AnnounceCleanup(void);
#else
#define                 F_TCPIP_AnnounceCleanup()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static void             ANNOUNCE_Notify(TCPIP_NET_HANDLE hNet, TCPIP_DHCP_EVENT_TYPE dhcpEvType, const void * param);

#if defined (TCPIP_STACK_USE_IPV6)
static void             IPv6_Announce_Notify(TCPIP_NET_HANDLE hNet, IPV6_EVENT_TYPE addEvType, const void * param, const void* usrParam);
#endif // defined (TCPIP_STACK_USE_IPV6)

static void             TCPIP_ANNOUNCE_Send(void);

static bool             TCPIP_ANNOUNCE_SendIf(size_t netIx, TCPIP_ANNOUNCE_DCPT* pDcpt, uint8_t* pAnnBuffer);

static void             TCPIP_ANNOUNCE_Timeout(void);

static void             F_TCPIP_AnnounceSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param);

typedef struct
{
    TCPIP_ANNOUNCE_MESSAGE*         pMsg;       // pointer to the message currently assembled
    TCPIP_ANNOUNCE_FIELD_PAYLOAD    fieldType;  // current processed field
    const TCPIP_NET_IF*             pNetIf;     // interface for which this message is prepared
    uint16_t                        msgLen;     // current assembled message length
    uint16_t                        leftLen;    // current space left in the write buffer 
    union
    {
        uint8_t*                        pWrPtr;     // current write pointer to write buffer
        TCPIP_ANNOUNCE_TRUNC_DCPT*      pTDcpt;
        TCPIP_ANNOUNCE_MAC_ADDR_DCPT*   pMDcpt;
        TCPIP_ANNOUNCE_MAC_NAME_DCPT*   pNDcpt;
        TCPIP_ANNOUNCE_HOST_NAME_DCPT*  pHNDcpt;
        TCPIP_ANNOUNCE_IPV4_ADDR_DCPT*  pIpDcpt;
        TCPIP_ANNOUNCE_IPV6_ADDR_DCPT*  pIp6Dcpt;
    };
#if defined (TCPIP_STACK_USE_IPV6)
    const IPV6_ADDR_STRUCT*         currAddress;    // current procesed IPv6 address
    const IPV6_HEAP_NDP_DR_ENTRY*   currRouter;     // current procesed router
    const IPV6_ADDR*                currGateway;    // current procesed gateway
#endif  //  defined (TCPIP_STACK_USE_IPV6)
    TCPIP_ANNOUNCE_MESSAGE_CALLBACK usrCback;   // user callback, if present in this call
}TCPIP_ANNOUNCE_SEND_DCPT;


static bool             F_AnnounceFieldProcess(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt);

// function that sets a specific Announce field
// Returns:
//  > 0 - the number of bytes written into the message descriptor, including the terminator!
//    0 - if there is no data to write
//   -1 - if not enough space; This stops the processing and sets the truncated bit!
// It always check for the terminator space available (CRLF) but it may not set it
// (taken care when the individual function is called)
typedef int            (*F_AnnounceFieldFnc)(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt);

static int F_FieldSetNone(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt);
static int F_FieldSetTruncated(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt);
static int F_FieldSetMacAddr(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt);
static int F_FieldSetMacName(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt);
static int F_FieldSetHostName(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt);
#if defined (TCPIP_STACK_USE_IPV4)
static int F_FieldSetIpv4Addr(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt);
#endif  //  defined (TCPIP_STACK_USE_IPV4)
#if defined (TCPIP_STACK_USE_IPV6)
static int F_FieldSetIpv6Address(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt);

const IPV6_ADDR* F_FieldIpV6NextAddress(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt);
#endif  //  defined (TCPIP_STACK_USE_IPV6)
static int F_FieldSetUserStart(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt);

static const F_AnnounceFieldFnc    T_AnnounceSendTbl[] = 
{
    &F_FieldSetNone,             // TCPIP_ANNOUNCE_FIELD_NONE
    &F_FieldSetTruncated,        // TCPIP_ANNOUNCE_FIELD_TRUNCATED,
    &F_FieldSetMacAddr,          // TCPIP_ANNOUNCE_FIELD_MAC_ADDR,
    &F_FieldSetMacName,          // TCPIP_ANNOUNCE_FIELD_MAC_NAME,
    &F_FieldSetHostName,         // TCPIP_ANNOUNCE_FIELD_HOST_NAME,
#if defined (TCPIP_STACK_USE_IPV4)
    &F_FieldSetIpv4Addr,         // TCPIP_ANNOUNCE_FIELD_IPV4_ADDRESS,
#endif  //  defined (TCPIP_STACK_USE_IPV4)
#if defined (TCPIP_STACK_USE_IPV6)
    &F_FieldSetIpv6Address,      // TCPIP_ANNOUNCE_FIELD_IPV6_UNICAST,
    &F_FieldSetIpv6Address,      // TCPIP_ANNOUNCE_FIELD_IPV6_MULTICAST,
    &F_FieldSetIpv6Address,      // TCPIP_ANNOUNCE_FIELD_IPV6_ROUTER,
    &F_FieldSetIpv6Address,      // TCPIP_ANNOUNCE_FIELD_IPV6_GATEWAY,
#endif  //  defined (TCPIP_STACK_USE_IPV6)
    &F_FieldSetUserStart,         // TCPIP_ANNOUNCE_FIELD_HOST_NAME,
};

// conversion functions/helpers
//
static __inline__ TCPIP_ANNOUNCE_MESSAGE* __attribute__((always_inline)) FC_U8Ptr2AnnMsg(uint8_t* u8Ptr)
{
    union
    {
        uint8_t* u8Ptr; 
        TCPIP_ANNOUNCE_MESSAGE* msgPtr;
    }U_U8_PTR_ANN_MSG;

    U_U8_PTR_ANN_MSG.u8Ptr = u8Ptr;
    return U_U8_PTR_ANN_MSG.msgPtr;
}

// debug functions
#if (TCPIP_ANNOUNCE_DEBUG_MASK & TCPIP_ANNOUNCE_DEBUG_MASK_NOTIFY_EVENTS) != 0
static void F_AnnounceDbgNotifyEvent(TCPIP_NET_HANDLE hNet, TCPIP_ANNOUNCE_EVENT_TYPE annEvType, uint32_t reqMask, int callEvent)
{
    char   ifBuff[20];
    char    evBuff[20];
    if(hNet != NULL)
    {
        (void)TCPIP_STACK_NetAliasNameGet(hNet, ifBuff, sizeof(ifBuff));
    }
    else
    {
        (void)strcpy(ifBuff, "all ifs");
    }
    switch(annEvType)
    {
        case TCPIP_ANNOUNCE_EVENT_IPV6_ADDRESS:
            (void)strcpy(evBuff, "ipv6");
            break;
        case TCPIP_ANNOUNCE_EVENT_REMOTE_REQUEST:
            (void)strcpy(evBuff, "remote");
            break;
        case TCPIP_ANNOUNCE_EVENT_USER_REQUEST:
            (void)strcpy(evBuff, "user");
            break;
        default:    // TCPIP_ANNOUNCE_EVENT_IPV4_DHCP:
            (void)strcpy(evBuff, "dhcp");
            break;
    }

    SYS_CONSOLE_PRINT("Announce if: %s, event: %s, req: 0x%8x, call event: %d\r\n", ifBuff, evBuff, reqMask, callEvent);
}
#else
#define F_AnnounceDbgNotifyEvent(hNet, annEvType, reqMask, callEvent)
#endif  // (TCPIP_ANNOUNCE_DEBUG_MASK & TCPIP_ANNOUNCE_DEBUG_MASK_NOTIFY_EVENTS) != 0

#if (TCPIP_ANNOUNCE_DEBUG_MASK & TCPIP_ANNOUNCE_DEBUG_MASK_SEND_EVENTS) != 0
static void F_AnnounceDbgSendEvent(size_t netIx, uint32_t reqMask, bool success)
{
    char   ifBuff[20];

    TCPIP_NET_HANDLE hNet = TCPIP_STACK_IndexToNet(netIx);
    (void)TCPIP_STACK_NetAliasNameGet(hNet, ifBuff, sizeof(ifBuff));

    SYS_CONSOLE_PRINT("Announce send on if: %s, mask: 0x%8x, %s\r\n", ifBuff, reqMask, success ? "Success" : "Failed!");
}
#else
#define F_AnnounceDbgSendEvent(netIx, reqMask, success)
#endif  // (TCPIP_ANNOUNCE_DEBUG_MASK & TCPIP_ANNOUNCE_DEBUG_MASK_SEND_EVENTS) != 0

// implementation; API functions

bool TCPIP_ANNOUNCE_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* initData)
{
    UDP_SOCKET  s;
    TCPIP_UDP_SIGNAL_HANDLE sigHandle;
    uint16_t currBuffSize;
    bool        initFail = false;

    if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }
    
    // stack init    
    while(announceInitCount == 0)
    {   // first time we're run
        initFail = true;
        // initstatus
        (void)memset(&announceDcpt, 0, sizeof(announceDcpt));
        announceDcpt.skt = INVALID_UDP_SOCKET;

        // store the memory allocation handle
        announceDcpt.memH = stackCtrl->memH;
        announceDcpt.nIfs = stackCtrl->nIfs;
#if defined (TCPIP_STACK_USE_DHCP_CLIENT)
        announceDcpt.dhcpHandler = TCPIP_DHCP_HandlerRegister(NULL, &ANNOUNCE_Notify, (const void*)TCPIP_ANNOUNCE_EVENT_IPV4_DHCP);
        if (announceDcpt.dhcpHandler == NULL)
        {
            break;
        }
#endif
#if defined (TCPIP_STACK_USE_IPV6)
        announceDcpt.ipv6Handler = TCPIP_IPV6_HandlerRegister(NULL, &IPv6_Announce_Notify, NULL);
        if (announceDcpt.ipv6Handler == NULL)
        {
            break;
        }
#endif

        announceDcpt.sigHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_ANNOUNCE_Task, TCPIP_ANNOUNCE_TASK_RATE); 
        if(announceDcpt.sigHandle == NULL)
        {
            break;
        }

        // Open a UDP socket for inbound and outbound transmission
        // Allow receive on any interface 
        s = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_IPV4, TCPIP_ANNOUNCE_PORT, NULL);

        if(s == INVALID_UDP_SOCKET)
        {
            break;
        }
        announceDcpt.skt = s;

        if(!TCPIP_UDP_RemoteBind(s, IP_ADDRESS_TYPE_IPV4, TCPIP_ANNOUNCE_PORT,  NULL))
        {
            break;
        }
        // locally bind to the same port
        if(!TCPIP_UDP_Bind(s, IP_ADDRESS_TYPE_IPV4, TCPIP_ANNOUNCE_PORT,  NULL))
        {
            break;
        }

        if(!TCPIP_UDP_OptionsSet(s, UDP_OPTION_STRICT_PORT, FC_Uint2VPtr(1UL)))
        {
            break;
        }

        // make sure the socket is created with enough TX space
        uint16_t minMsgSize = (uint16_t)sizeof(S_TCPIP_ANNOUNCE_MIN_MESSAGE);
        announceDcpt.txBuffSize = (uint16_t)TCPIP_ANNOUNCE_MAX_PAYLOAD < minMsgSize ? minMsgSize : (uint16_t)TCPIP_ANNOUNCE_MAX_PAYLOAD;
        (void)TCPIP_UDP_OptionsGet(s, UDP_OPTION_TX_BUFF, (void*)&currBuffSize);
        if(currBuffSize < announceDcpt.txBuffSize)
        {
            if(!TCPIP_UDP_OptionsSet(s, UDP_OPTION_TX_BUFF, FC_Uint2VPtr((uint32_t)announceDcpt.txBuffSize)))
            {
                break;
            }
        }

#if (TCPIP_ANNOUNCE_NETWORK_DIRECTED_BCAST != 0)
        announceDcpt.bcastType = UDP_BCAST_NETWORK_DIRECTED;
#else
        announceDcpt.bcastType = UDP_BCAST_NETWORK_LIMITED;
#endif

        (void)TCPIP_UDP_OptionsSet(s, UDP_OPTION_BROADCAST, FC_Uint2VPtr((uint32_t)announceDcpt.bcastType));

        sigHandle = TCPIP_UDP_SignalHandlerRegister(s, TCPIP_UDP_SIGNAL_RX_DATA, &F_TCPIP_AnnounceSocketRxSignalHandler, NULL);
        if(sigHandle == NULL)
        {
            break;
        }


        initFail = false;
        break;
    }

    if(initFail)    
    {
        F_TCPIP_AnnounceCleanup();
        return false;
    }
    
    announceInitCount++;

    return true;
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_ANNOUNCE_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{

    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN) // interface down

    if(announceInitCount > 0)
    {   // we're up and running
        if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_DOWN)
        {
            announceDcpt.requestMask &= ~(1UL << stackCtrl->netIx);
            announceDcpt.readyMask &= ~(1UL << stackCtrl->netIx);
        }
        else if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--announceInitCount == 0)
            {   // all closed
                // release resources
                F_TCPIP_AnnounceCleanup();
                announceDcpt.memH = NULL;
            }
        }
        else
        {
            // do nothing
        }
    }

}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)






// if many interfaces and sending over an interface fails,
// remember where left off, so next time resume the sequence
// with that interface
static void TCPIP_ANNOUNCE_Send(void)
{
    UDP_SOCKET  annSkt;
    uint16_t    nIter;
    bool        sendOk;
    uint8_t*    pAnnBuffer;

    pAnnBuffer = (uint8_t*)TCPIP_HEAP_Malloc(announceDcpt.memH, announceDcpt.txBuffSize);
    if(pAnnBuffer == NULL)
    {   // failed allocation; retry next time
        return;
    }

    annSkt = announceDcpt.skt;

    // reset the broadcast type, it may have changed
    (void)TCPIP_UDP_OptionsSet(annSkt, UDP_OPTION_BROADCAST, FC_Uint2VPtr((uint32_t)announceDcpt.bcastType));

    nIter = 0U;
    while(true)
    {
        // reply to the request on the interface it arrived on
        if((announceDcpt.requestMask & (1UL << announceDcpt.currIx)) != 0U)
        {
            sendOk = TCPIP_ANNOUNCE_SendIf((size_t)announceDcpt.currIx, &announceDcpt, pAnnBuffer);
            F_AnnounceDbgSendEvent((size_t)announceDcpt.currIx, announceDcpt.requestMask, sendOk);
            
            if(!sendOk)
            {   // failed to send; try next time
                break;
            }

            announceDcpt.requestMask &= ~(1UL << announceDcpt.currIx);   // clear requests on this interface
        }

        if(++announceDcpt.currIx == announceDcpt.nIfs)
        {
            announceDcpt.currIx = 0;
        }

        if(++nIter == announceDcpt.nIfs)
        {   // done
            break;
        }
    }

    // unbind socket so it listens on all networks
    (void)TCPIP_UDP_OptionsSet(annSkt, UDP_OPTION_STRICT_NET, NULL);

    (void)TCPIP_HEAP_Free(announceDcpt.memH, pAnnBuffer);
}

// send a message on the specified interface
static bool TCPIP_ANNOUNCE_SendIf(size_t netIx, TCPIP_ANNOUNCE_DCPT* pDcpt, uint8_t* pAnnBuffer)
{
    UDP_SOCKET  annSkt;
    const TCPIP_NET_IF *pNetIf;
    IP_MULTI_ADDRESS ifAdd;
    TCPIP_ANNOUNCE_SEND_DCPT annDcpt;
    TCPIP_ANNOUNCE_TRUNC_DCPT* pTDcpt;
    uint8_t*    outData;
    bool        isTruncated;

    annSkt = pDcpt->skt;

    pNetIf = (const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(netIx);

    if(TCPIP_STACK_NetworkIsLinked(pNetIf))
    {   // reply only if this interface is up and running

        if (TCPIP_UDP_PutIsReady(annSkt) < pDcpt->txBuffSize)
        {   // couldn't allocate buffer; try later
            return false;
        }

        annDcpt.pMsg = FC_U8Ptr2AnnMsg(pAnnBuffer);
        annDcpt.pNetIf = pNetIf;
        annDcpt.msgLen = 0U;
        annDcpt.leftLen = pDcpt->txBuffSize;
        annDcpt.pWrPtr = pAnnBuffer;
        annDcpt.usrCback = pDcpt->usrCback;

        pTDcpt = &annDcpt.pMsg->truncDcpt;
        if(F_AnnounceFieldProcess(&annDcpt))
        {   // everything went well
            // skip the truncation field
            annDcpt.msgLen -= (uint16_t)sizeof(*pTDcpt);
            outData = (uint8_t*)(pTDcpt + 1);
            isTruncated = false;
        }
        else
        {   // it's truncated, not everything went through
            outData = (uint8_t*)pTDcpt;
            isTruncated = true;
        }

        (void)TCPIP_UDP_SocketNetSet (annSkt, pNetIf);
        ifAdd.v4Add.Val = TCPIPStackNetAddress(pNetIf);
        (void)TCPIP_UDP_SourceIPAddressSet(annSkt, IP_ADDRESS_TYPE_IPV4, &ifAdd);
        (void)TCPIP_UDP_ArrayPut (annSkt, outData, annDcpt.msgLen);

        if(!isTruncated && pDcpt->usrCback != NULL)
        {   // call the user handler
            (*pDcpt->usrCback)(pNetIf, annSkt);
        }
        (void)TCPIP_UDP_Flush (annSkt);
        return true;
    }

    return false;
}

void TCPIP_ANNOUNCE_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(((uint16_t)sigPend & (uint16_t)TCPIP_MODULE_SIGNAL_TMO) != 0U)
    { // regular TMO occurred
        TCPIP_ANNOUNCE_Timeout();
    }

    if(((uint16_t)sigPend & (uint16_t)TCPIP_MODULE_SIGNAL_ASYNC) != 0U)
    { // async message occurred
        // clear the ASYNC request
        (void)TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_ASYNC);
        TCPIP_ANNOUNCE_Send();
    }

}

// send a signal to the Announce module that data is available
// no manager alert needed since this normally results as a higher layer (UDP) signal
static void F_TCPIP_AnnounceSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_UDP_SIGNAL_RX_DATA)
    {
        (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}



static void TCPIP_ANNOUNCE_Timeout(void)
{
    uint8_t         discQuery;
    UDP_SOCKET      s;
    UDP_SOCKET_INFO sktInfo;
    
    s = announceDcpt.skt;

    while(true)
    {   // consume all queued packets
        if(TCPIP_UDP_GetIsReady(s) == 0U)
        {
            break;
        }
            
        // See if this is a discovery query or reply
        (void)TCPIP_UDP_Get(s, &discQuery);
        if(discQuery == (uint8_t)'D')
        {   // We received a discovery request, reply
            (void)memset(&sktInfo, 0, sizeof(sktInfo));
            (void)TCPIP_UDP_SocketInfoGet(s, &sktInfo);
            // fake a legitimate DHCP event on that interface   
            ANNOUNCE_Notify (sktInfo.hNet, DHCP_EVENT_BOUND, FC_Uint2VPtr((uint32_t)TCPIP_ANNOUNCE_EVENT_REMOTE_REQUEST));
        }
        (void)TCPIP_UDP_Discard(s);
    }   

    const TCPIP_NET_IF *pNetIf;
    size_t netIx;
    for(netIx = 0; netIx < announceDcpt.nIfs; netIx++)
    {
        pNetIf = (const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(netIx);
        if(!TCPIP_STACK_NetIsReady(pNetIf))
        {   // network not ready
            announceDcpt.readyMask &= ~(1UL << netIx);
        }
        else if((announceDcpt.readyMask & (1UL << netIx)) == 0U)
        {   // now it becomes ready
            announceDcpt.readyMask |= (1UL << netIx);
            ANNOUNCE_Notify (pNetIf, DHCP_EVENT_BOUND, (const void*)TCPIP_ANNOUNCE_EVENT_IPV4_DHCP);
        }
        else
        {
            // do nothing
        }
    }

}


// local functions

// announce notification
// if hNet != 0 it uses that network to request service
// otherwise relies on what's already in there
// param is an TCPIP_ANNOUNCE_EVENT_TYPE indicating the notification origin
// When param == TCPIP_ANNOUNCE_EVENT_REMOTE_REQUEST the aliases
// need to have their request mask updated too
// That's because broadcast messages are not propagated on aliases (yet)
static void ANNOUNCE_Notify(TCPIP_NET_HANDLE hNet, TCPIP_DHCP_EVENT_TYPE dhcpEvType, const void * param)
{
    if(announceDcpt.memH != NULL)
    {
        if(dhcpEvType == DHCP_EVENT_BOUND || dhcpEvType == DHCP_EVENT_CONN_LOST || dhcpEvType == DHCP_EVENT_SERVICE_DISABLED)
        {
            if(hNet != NULL)
            {
                TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNet(hNet);
                announceDcpt.requestMask |= (1UL << pNetIf->netIfIx);

#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
                TCPIP_ANNOUNCE_EVENT_TYPE annEvType = (TCPIP_ANNOUNCE_EVENT_TYPE)param;
                if(annEvType == TCPIP_ANNOUNCE_EVENT_REMOTE_REQUEST && TCPIPStackNetIsPrimary(pNetIf))
                {
                    TCPIP_NET_IF* pIf;
                    for(pIf = TCPIPStackNetGetAlias(pNetIf); pIf != NULL; pIf = TCPIPStackNetGetAlias(pIf)) 
                    {
                        announceDcpt.requestMask |= (1UL << pIf->netIfIx);
                    }
                }
#endif  // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
            }
            (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_ASYNC, false); 
            F_AnnounceDbgNotifyEvent(hNet, (TCPIP_ANNOUNCE_EVENT_TYPE)param, announceDcpt.requestMask, (int)dhcpEvType);
        }
    }
}

#if defined (TCPIP_STACK_USE_IPV6)
static void IPv6_Announce_Notify(TCPIP_NET_HANDLE hNet, IPV6_EVENT_TYPE addEvType, const void * param, const void* usrParam)
{
    if(announceDcpt.memH != NULL)
    {
        if(addEvType == IPV6_EVENT_ADDRESS_ADDED || addEvType == IPV6_EVENT_ADDRESS_REMOVED || addEvType == IPV6_EVENT_ULA_ADDRESS_GENERATED)                
        {
            TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNet(hNet);
            announceDcpt.requestMask |= (1UL << pNetIf->netIfIx);
            (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_ASYNC, false); 
            F_AnnounceDbgNotifyEvent(hNet, TCPIP_ANNOUNCE_EVENT_IPV6_ADDRESS, announceDcpt.requestMask, (int)addEvType);
        }
    }
}
#endif    //TCPIP_STACK_USE_IPV6 

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void F_TCPIP_AnnounceCleanup(void)
{
#if defined   ( TCPIP_STACK_USE_DHCP_CLIENT)
    if (announceDcpt.dhcpHandler != NULL)
    {
        (void)TCPIP_DHCP_HandlerDeRegister(announceDcpt.dhcpHandler);
        announceDcpt.dhcpHandler = NULL;
    }
#endif
#if defined (TCPIP_STACK_USE_IPV6)
    if (announceDcpt.ipv6Handler != NULL)
    {
        (void)TCPIP_IPV6_HandlerDeregister(announceDcpt.ipv6Handler);
        announceDcpt.ipv6Handler = NULL;
    }
#endif

    if(announceDcpt.sigHandle != NULL)
    {
        TCPIPStackSignalHandlerDeregister(announceDcpt.sigHandle);
        announceDcpt.sigHandle = NULL;
    }

    if(announceDcpt.skt != INVALID_UDP_SOCKET)
    {
        (void)TCPIP_UDP_Close(announceDcpt.skt);
        announceDcpt.skt = INVALID_UDP_SOCKET;
    }

    announceDcpt.requestMask = 0;
    announceDcpt.readyMask = 0;
    announceDcpt.nIfs = 0;

}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

// returns true if manages to put all message in the pSDcpt buffer
// false if there was not enough room
// number of bytes set in the buffer is updated in pSDcpt
static bool F_AnnounceFieldProcess(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt)
{
    size_t ix;
    int    fieldBytes;
    const F_AnnounceFieldFnc* pFieldFnc;

    pFieldFnc = T_AnnounceSendTbl;
    for(ix = 0; ix < sizeof(T_AnnounceSendTbl) / sizeof(*T_AnnounceSendTbl); ix++)
    {
        pSDcpt->fieldType = (TCPIP_ANNOUNCE_FIELD_PAYLOAD)ix; 
#if defined (TCPIP_STACK_USE_IPV6)
        pSDcpt->currAddress = NULL;
        pSDcpt->currRouter = NULL;
        pSDcpt->currGateway = NULL;
#endif  //  defined (TCPIP_STACK_USE_IPV6)
        fieldBytes = (*pFieldFnc)(pSDcpt);
        if(fieldBytes < 0)
        {   // failed
            return false;
        }

        // success
        pSDcpt->msgLen += (uint16_t)fieldBytes;
        pSDcpt->leftLen -= (uint16_t)fieldBytes;
        pSDcpt->pWrPtr += fieldBytes;

        if(fieldBytes != 0)
        {   // set the terminator
            *(pSDcpt->pWrPtr - 2) = M_ANNOUNCE_SEPARATOR_1;
            *(pSDcpt->pWrPtr - 1) = M_ANNOUNCE_SEPARATOR_2;
        }
        pFieldFnc++;
    }

    return true;
}

// dummy function
// TCPIP_ANNOUNCE_FIELD_NONE
static int F_FieldSetNone(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt)
{
    return 0;
}

// just make it not truncated for now; it will be set only when needed
// TCPIP_ANNOUNCE_FIELD_TRUNCATED
static int F_FieldSetTruncated(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt)
{
    TCPIP_ANNOUNCE_TRUNC_DCPT* pTDcpt = pSDcpt->pTDcpt;
    
    if(pSDcpt->leftLen >= (uint16_t)sizeof(*pTDcpt))
    {
        pTDcpt->type = (uint8_t)TCPIP_ANNOUNCE_FIELD_TRUNCATED;
        return FC_UI322I32((uint32_t)sizeof(*pTDcpt));
    }

    // failed
    return -1;
}

// insert a terminator only if there's user data
static int F_FieldSetUserStart(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt)
{
    return (pSDcpt->usrCback != NULL) ? FC_UI322I32((uint32_t)sizeof(TCPIP_ANNOUNCE_USER_START)) : 0;
}

// TCPIP_ANNOUNCE_FIELD_MAC_ADDR
static int F_FieldSetMacAddr(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt)
{
    TCPIP_ANNOUNCE_MAC_ADDR_DCPT* pMDcpt = pSDcpt->pMDcpt;
    
    if(pSDcpt->leftLen >= (uint16_t)sizeof(*pMDcpt))
    {
        pMDcpt->type = (uint8_t)TCPIP_ANNOUNCE_FIELD_MAC_ADDR;
        (void)memcpy(pMDcpt->macAddr, TCPIPStack_NetMACAddressGet(pSDcpt->pNetIf), sizeof(pMDcpt->macAddr));
        return FC_UI322I32((uint32_t)sizeof(*pMDcpt));
    }

    // failed
    return -1;
}

// TCPIP_ANNOUNCE_FIELD_MAC_NAME
static int F_FieldSetMacName(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt)
{
    TCPIP_ANNOUNCE_MAC_NAME_DCPT* pNDcpt = pSDcpt->pNDcpt;
    const char* macName = TCPIP_STACK_MACIdToString((TCPIP_STACK_MODULE)pSDcpt->pNetIf->macId);
    size_t nameLen = strlen(macName);

    
    if(pSDcpt->leftLen >= (uint16_t)sizeof(*pNDcpt) + nameLen)
    {
        pNDcpt->type = (uint8_t)TCPIP_ANNOUNCE_FIELD_MAC_NAME;
        (void)strcpy(pNDcpt->macName, macName);
        return FC_UI322I32((uint32_t)sizeof(*pNDcpt) + nameLen);
    }

    // failed
    return -1;
}

// TCPIP_ANNOUNCE_FIELD_HOST_NAME
static int F_FieldSetHostName(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt)
{
    TCPIP_ANNOUNCE_HOST_NAME_DCPT* pHNDcpt = pSDcpt->pHNDcpt;
    const char* hostName = TCPIPStack_NetBIOSNameGet(pSDcpt->pNetIf);
    size_t hostLen = strlen(hostName);

    
    if(pSDcpt->leftLen >= (uint16_t)sizeof(*pHNDcpt) + hostLen)
    {
        pHNDcpt->type = (uint8_t)TCPIP_ANNOUNCE_FIELD_HOST_NAME;
        (void)strcpy(pHNDcpt->hostName, hostName);
        return FC_UI322I32((uint32_t)sizeof(*pHNDcpt) + hostLen);
    }

    // failed
    return -1;
}

// TCPIP_ANNOUNCE_FIELD_IPV4_ADDRESS
#if defined (TCPIP_STACK_USE_IPV4)
static int F_FieldSetIpv4Addr(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt)
{
    TCPIP_ANNOUNCE_IPV4_ADDR_DCPT* pIpDcpt = pSDcpt->pIpDcpt;
    IPV4_ADDR ipAdd;

    
    if(pSDcpt->leftLen >= (uint16_t)sizeof(*pIpDcpt))
    {
        pIpDcpt->type = (uint8_t)TCPIP_ANNOUNCE_FIELD_IPV4_ADDRESS;
        ipAdd.Val = TCPIPStackNetAddress(pSDcpt->pNetIf);
        (void)memcpy(pIpDcpt->ipv4Addr, ipAdd.v, sizeof(ipAdd));
        return FC_UI322I32((uint32_t)sizeof(*pIpDcpt));
    }

    // failed
    return -1;
}
#endif  //  defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV6)

const IPV6_ADDR* F_FieldIpV6NextAddress(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt)
{
    IPV6_INTERFACE_CONFIG*  pIpv6Config;

    if(pSDcpt->fieldType == TCPIP_ANNOUNCE_FIELD_IPV6_UNICAST || pSDcpt->fieldType == TCPIP_ANNOUNCE_FIELD_IPV6_MULTICAST)
    {
        IPV6_ADDR_STRUCT* addressPtr;

        if(pSDcpt->currAddress == NULL)
        {
            pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pSDcpt->pNetIf);
            DOUBLE_LIST* pHead = pSDcpt->fieldType == TCPIP_ANNOUNCE_FIELD_IPV6_UNICAST ? &pIpv6Config->listIpv6UnicastAddresses : &pIpv6Config->listIpv6MulticastAddresses;
            addressPtr = FC_DblNode2AddStruct(pHead->head);
        }
        else
        {
            addressPtr = pSDcpt->currAddress->next;
        }

        return (pSDcpt->currAddress = addressPtr) == NULL ? NULL :  &addressPtr->address;
    }

    if(pSDcpt->fieldType == TCPIP_ANNOUNCE_FIELD_IPV6_ROUTER)
    {
        IPV6_HEAP_NDP_DR_ENTRY* routerPtr;

        if(pSDcpt->currRouter == NULL)
        {
            pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pSDcpt->pNetIf);
            routerPtr = pIpv6Config->currentDefaultRouter;
        }
        else
        {
            routerPtr = pSDcpt->currRouter->next;
        }

        pSDcpt->currRouter = routerPtr;

        if(routerPtr == NULL || routerPtr->neighborInfo == NULL)
        {
           return NULL;
        }

        return  &routerPtr->neighborInfo->remoteIPAddress;

    }

    if(pSDcpt->fieldType == TCPIP_ANNOUNCE_FIELD_IPV6_GATEWAY)
    {
        if(pSDcpt->currGateway == NULL)
        {
            return (pSDcpt->currGateway = TCPIP_STACK_NetDefaultIPv6GatewayGet(pSDcpt->pNetIf));
        }

        // return only one default gateway, no list!
        return NULL;
    }

    // unknown field type
    return NULL;
}


// TCPIP_ANNOUNCE_FIELD_IPV6_UNICAST
// TCPIP_ANNOUNCE_FIELD_IPV6_MULTICAST
// TCPIP_ANNOUNCE_FIELD_IPV6_ROUTER
// TCPIP_ANNOUNCE_FIELD_IPV6_GATEWAY
static int F_FieldSetIpv6Address(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt)
{
    const IPV6_ADDR*    pCurrAddress;
    uint16_t    addressLen, leftLen, nAddresses;


    // calculate the needed size
    addressLen = nAddresses = 0U;
    leftLen = pSDcpt->leftLen;

    TCPIP_ANNOUNCE_IPV6_ADDR_DCPT* pIp6Dcpt = pSDcpt->pIp6Dcpt;
    for(pCurrAddress = F_FieldIpV6NextAddress(pSDcpt); pCurrAddress != NULL; pCurrAddress = F_FieldIpV6NextAddress(pSDcpt))
    {
        nAddresses++;
        if(leftLen < addressLen + (uint16_t)sizeof(*pIp6Dcpt))
        {   // no more room, done
            break;
        }

        // set this entry
        pIp6Dcpt->type = (uint8_t)pSDcpt->fieldType;
        (void)memcpy(pIp6Dcpt->ipv6Addr, pCurrAddress->v, sizeof(pIp6Dcpt->ipv6Addr));
        pIp6Dcpt->separator[0] = (char)M_ANNOUNCE_SEPARATOR_1;
        pIp6Dcpt->separator[1] = (char)M_ANNOUNCE_SEPARATOR_2;

        addressLen += (uint16_t)sizeof(*pIp6Dcpt);
        pIp6Dcpt++;
    }

    // return -1 if no space, 0 if no IPv6 addresses
    return addressLen != 0U ? (int)addressLen : nAddresses == 0U ? 0 : -1;
}

#endif  //  defined (TCPIP_STACK_USE_IPV6)

bool TCPIP_ANNOUNCE_MessageRequest(TCPIP_NET_HANDLE hNet, TCPIP_ANNOUNCE_BROADCAST_TYPE bcastType)
{
    uint32_t reqMask;
    OSAL_CRITSECT_DATA_TYPE status;

    while(announceDcpt.nIfs != 0U)
    {   // up and running
        if(hNet != NULL)
        {
            TCPIP_NET_IF* pIf = TCPIPStackHandleToNetLinked(hNet);
            if(pIf == NULL)
            {
                break;
            }
            reqMask = 1UL << pIf->netIfIx; 
        }
        else
        {
            reqMask = (1UL << announceDcpt.nIfs) - 1UL;
        }


        status =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        if(bcastType != TCPIP_ANNOUNCE_BCAST_NONE)
        {
            announceDcpt.bcastType = bcastType == (TCPIP_ANNOUNCE_BCAST_NET_DIRECTED) ? UDP_BCAST_NETWORK_DIRECTED : UDP_BCAST_NETWORK_LIMITED;
        }
        announceDcpt.requestMask |= reqMask;
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
        // force notification
        ANNOUNCE_Notify (NULL, DHCP_EVENT_BOUND, FC_Uint2VPtr((uint32_t)TCPIP_ANNOUNCE_EVENT_USER_REQUEST));


        return true;
    }

    return false;
}

bool TCPIP_ANNOUNCE_CallbackRegister(TCPIP_ANNOUNCE_MESSAGE_CALLBACK callback)
{
    while(announceDcpt.nIfs != 0U)
    {   // up and running
        announceDcpt.usrCback = callback;
        return true; 
    }

    return false;
}

#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_ANNOUNCE)

