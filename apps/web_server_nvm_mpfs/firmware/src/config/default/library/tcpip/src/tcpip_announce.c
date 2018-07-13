/*******************************************************************************
  TCPIP Announce Client and Server

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    - Provides device hostname and IP address discovery on a local 
      Ethernet subnet (same broadcast domain)
    - Reference: None.  Hopefully AN833 in the future.
*******************************************************************************/

/*******************************************************************************
File Name:  tcpip_announce.c
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
#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_ANNOUNCE

#include "tcpip/src/tcpip_private.h"

#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_ANNOUNCE)
#include <alloca.h>

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
    const void* memH;           // memory handle
    tcpipSignalHandle   sigHandle;      // asynchronous handle
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
    TCPIP_ANNOUNCE_IPV4_ADDR_DCPT       ipv6UnicastDcpt[4];
}_TCPIP_ANNOUNCE_MIN_MESSAGE;
#elif defined(TCPIP_STACK_USE_IPV6)
typedef struct __attribute__((packed))
{
    TCPIP_ANNOUNCE_TRUNC_DCPT           truncDcpt; 
    TCPIP_ANNOUNCE_MAC_ADDR_DCPT        macAddDcpt;
    TCPIP_ANNOUNCE_MIN_MAC_NAME_DCPT    macNameDcpt;
    TCPIP_ANNOUNCE_MIN_HOST_NAME_DCPT   hostNameDcpt;
    TCPIP_ANNOUNCE_IPV4_ADDR_DCPT       ipv6UnicastDcpt[6];
}_TCPIP_ANNOUNCE_MIN_MESSAGE;
#else
typedef struct __attribute__((packed))
{
    TCPIP_ANNOUNCE_TRUNC_DCPT           truncDcpt; 
    TCPIP_ANNOUNCE_MAC_ADDR_DCPT        macAddDcpt;
    TCPIP_ANNOUNCE_MIN_MAC_NAME_DCPT    macNameDcpt;
    TCPIP_ANNOUNCE_MIN_HOST_NAME_DCPT   hostNameDcpt;
    TCPIP_ANNOUNCE_IPV4_ADDR_DCPT       ipv4AddDcpt;
}_TCPIP_ANNOUNCE_MIN_MESSAGE;
#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_IPV6)


#define _ANNOUNCE_SEPARATOR_1   0x0d
#define _ANNOUNCE_SEPARATOR_2   0x0a

// prototypes
#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void             _TCPIP_AnnounceCleanup(void);
#else
#define                 _TCPIP_AnnounceCleanup()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static void             ANNOUNCE_Notify(TCPIP_NET_HANDLE hNet, uint8_t dhcpEvType, const void * param);

#if defined (TCPIP_STACK_USE_IPV6)
static void  			IPv6_Announce_Notify(TCPIP_NET_HANDLE hNet, uint8_t evType, const void * param);
#endif // defined (TCPIP_STACK_USE_IPV6)

static void             TCPIP_ANNOUNCE_Send(void);

static bool             TCPIP_ANNOUNCE_SendIf(int netIx, TCPIP_ANNOUNCE_DCPT* pDcpt, uint8_t* pAnnBuffer);

static void             TCPIP_ANNOUNCE_Timeout(void);

static void             _TCPIP_AnnounceSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param);

typedef struct
{
    TCPIP_ANNOUNCE_MESSAGE*         pMsg;       // pointer to the message currently assembled
    TCPIP_ANNOUNCE_FIELD_PAYLOAD    fieldType;  // current processed field
    TCPIP_NET_IF*                   pNetIf;     // interface for which this message is prepared
    int                             msgLen;     // current assembled message length
    int                             leftLen;    // current space left in the write buffer 
    uint8_t*                        pWrPtr;     // current write pointer to write buffer
#if defined (TCPIP_STACK_USE_IPV6)
    const IPV6_ADDR_STRUCT*         currAddress;    // current procesed IPv6 address
    const IPV6_HEAP_NDP_DR_ENTRY*   currRouter;     // current procesed router
    const IPV6_ADDR*                currGateway;    // current procesed gateway
#endif  //  defined (TCPIP_STACK_USE_IPV6)
    TCPIP_ANNOUNCE_MESSAGE_CALLBACK usrCback;   // user callback, if present in this call
}TCPIP_ANNOUNCE_SEND_DCPT;


static bool             _AnnounceFieldProcess(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt);

// function that sets a specific Announce field
// Returns:
//  > 0 - the number of bytes written into the message descriptor, including the terminator!
//    0 - if there is no data to write
//   -1 - if not enough space; This stops the processing and sets the truncated bit!
// It always check for the terminator space available (CRLF) but it may not set it
// (taken care when the individual function is called)
typedef int            (*_AnnounceFieldFnc)(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt);

int _FieldSetNone(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt);
int _FieldSetTruncated(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt);
int _FieldSetMacAddr(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt);
int _FieldSetMacName(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt);
int _FieldSetHostName(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt);
#if defined (TCPIP_STACK_USE_IPV4)
int _FieldSetIpv4Addr(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt);
#endif  //  defined (TCPIP_STACK_USE_IPV4)
#if defined (TCPIP_STACK_USE_IPV6)
int _FieldSetIpv6Address(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt);

const IPV6_ADDR* _FieldIpV6NextAddress(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt);
#endif  //  defined (TCPIP_STACK_USE_IPV6)
int _FieldSetUserStart(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt);

static const _AnnounceFieldFnc    _AnnounceSendTbl[] = 
{
    _FieldSetNone,             // TCPIP_ANNOUNCE_FIELD_NONE
    _FieldSetTruncated,        // TCPIP_ANNOUNCE_FIELD_TRUNCATED,
    _FieldSetMacAddr,          // TCPIP_ANNOUNCE_FIELD_MAC_ADDR,
    _FieldSetMacName,          // TCPIP_ANNOUNCE_FIELD_MAC_NAME,
    _FieldSetHostName,         // TCPIP_ANNOUNCE_FIELD_HOST_NAME,
#if defined (TCPIP_STACK_USE_IPV4)
    _FieldSetIpv4Addr,         // TCPIP_ANNOUNCE_FIELD_IPV4_ADDRESS,
#endif  //  defined (TCPIP_STACK_USE_IPV4)
#if defined (TCPIP_STACK_USE_IPV6)
    _FieldSetIpv6Address,      // TCPIP_ANNOUNCE_FIELD_IPV6_UNICAST,
    _FieldSetIpv6Address,      // TCPIP_ANNOUNCE_FIELD_IPV6_MULTICAST,
    _FieldSetIpv6Address,      // TCPIP_ANNOUNCE_FIELD_IPV6_DEFAULT_ROUTER,
    _FieldSetIpv6Address,      // TCPIP_ANNOUNCE_FIELD_IPV6_DEFAULT_GATEWAY,
#endif  //  defined (TCPIP_STACK_USE_IPV6)
    _FieldSetUserStart,         // TCPIP_ANNOUNCE_FIELD_HOST_NAME,
};


// debug functions
#if (TCPIP_ANNOUNCE_DEBUG_MASK & TCPIP_ANNOUNCE_DEBUG_MASK_NOTIFY_EVENTS) != 0
static void _AnnounceDbgNotifyEvent(TCPIP_NET_HANDLE hNet, TCPIP_ANNOUNCE_EVENT_TYPE annEvType, uint32_t reqMask, int callEvent)
{
    char   ifBuff[20];
    char    evBuff[20];
    if(hNet != 0)
    {
        TCPIP_STACK_NetAliasNameGet(hNet, ifBuff, sizeof(ifBuff));
    }
    else
    {
        strcpy(ifBuff, "all ifs");
    }
    switch(annEvType)
    {
        case TCPIP_ANNOUNCE_EVENT_IPV6_ADDRESS:
            strcpy(evBuff, "ipv6");
            break;
        case TCPIP_ANNOUNCE_EVENT_REMOTE_REQUEST:
            strcpy(evBuff, "remote");
            break;
        case TCPIP_ANNOUNCE_EVENT_USER_REQUEST:
            strcpy(evBuff, "user");
            break;
        default:    // TCPIP_ANNOUNCE_EVENT_IPV4_DHCP:
            strcpy(evBuff, "dhcp");
            break;
    }

    SYS_CONSOLE_PRINT("Announce if: %s, event: %s, req: 0x%8x, call event: %d\r\n", ifBuff, evBuff, reqMask, callEvent);
}
#else
#define _AnnounceDbgNotifyEvent(hNet, annEvType, reqMask, callEvent)
#endif  // (TCPIP_ANNOUNCE_DEBUG_MASK & TCPIP_ANNOUNCE_DEBUG_MASK_NOTIFY_EVENTS) != 0

#if (TCPIP_ANNOUNCE_DEBUG_MASK & TCPIP_ANNOUNCE_DEBUG_MASK_SEND_EVENTS) != 0
static void _AnnounceDbgSendEvent(int netIx, uint32_t reqMask, bool success)
{
    char   ifBuff[20];

    TCPIP_NET_HANDLE hNet = TCPIP_STACK_IndexToNet(netIx);
    TCPIP_STACK_NetAliasNameGet(hNet, ifBuff, sizeof(ifBuff));

    SYS_CONSOLE_PRINT("Announce send on if: %s, mask: 0x%8x, %s\r\n", ifBuff, reqMask, success ? "Success" : "Failed!");
}
#else
#define _AnnounceDbgSendEvent(netIx, reqMask, success)
#endif  // (TCPIP_ANNOUNCE_DEBUG_MASK & TCPIP_ANNOUNCE_DEBUG_MASK_SEND_EVENTS) != 0

// implementation; API functions

bool TCPIP_ANNOUNCE_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_ANNOUNCE_MODULE_CONFIG* announceData)
{
    UDP_SOCKET  s;
    TCPIP_UDP_SIGNAL_HANDLE sigHandle;
    uint16_t currBuffSize;
    bool        initFail = false;

    if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }
    
    // stack init    
    while(announceInitCount == 0)
    {   // first time we're run
        initFail = true;
        // initstatus
        memset(&announceDcpt, 0, sizeof(announceDcpt));
        announceDcpt.skt = INVALID_UDP_SOCKET;

        // store the memory allocation handle
        announceDcpt.memH = stackCtrl->memH;
        announceDcpt.nIfs = stackCtrl->nIfs;
        announceDcpt.requestMask = 0;
#if defined (TCPIP_STACK_USE_DHCP_CLIENT)
        announceDcpt.dhcpHandler = TCPIP_DHCP_HandlerRegister(0, (TCPIP_DHCP_EVENT_HANDLER)ANNOUNCE_Notify, (const void*)TCPIP_ANNOUNCE_EVENT_IPV4_DHCP);
        if (announceDcpt.dhcpHandler == NULL)
        {
            break;
        }
#endif
#if defined (TCPIP_STACK_USE_IPV6)
        announceDcpt.ipv6Handler = TCPIP_IPV6_HandlerRegister(0, (IPV6_EVENT_HANDLER)IPv6_Announce_Notify, 0);
        if (announceDcpt.ipv6Handler == NULL)
        {
            break;
        }
#endif

        announceDcpt.sigHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_ANNOUNCE_Task, TCPIP_ANNOUNCE_TASK_RATE); 
        if(announceDcpt.sigHandle == 0)
        {
            break;
        }

        // Open a UDP socket for inbound and outbound transmission
        // Allow receive on any interface 
        s = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_IPV4, TCPIP_ANNOUNCE_PORT, 0);

        if(s == INVALID_UDP_SOCKET)
        {
            break;
        }
        announceDcpt.skt = s;

        if(!TCPIP_UDP_RemoteBind(s, IP_ADDRESS_TYPE_IPV4, TCPIP_ANNOUNCE_PORT,  0))
        {
            break;
        }
        // locally bind to the same port
        if(!TCPIP_UDP_Bind(s, IP_ADDRESS_TYPE_IPV4, TCPIP_ANNOUNCE_PORT,  0))
        {
            break;
        }

        if(!TCPIP_UDP_OptionsSet(s, UDP_OPTION_STRICT_PORT, (void*)true))
        {
            break;
        }

        // make sure the socket is created with enough TX space
        announceDcpt.txBuffSize = TCPIP_ANNOUNCE_MAX_PAYLOAD < sizeof(_TCPIP_ANNOUNCE_MIN_MESSAGE) ? sizeof(_TCPIP_ANNOUNCE_MIN_MESSAGE) : TCPIP_ANNOUNCE_MAX_PAYLOAD;
        TCPIP_UDP_OptionsGet(s, UDP_OPTION_TX_BUFF, (void*)&currBuffSize);
        if(currBuffSize < announceDcpt.txBuffSize)
        {
            if(!TCPIP_UDP_OptionsSet(s, UDP_OPTION_TX_BUFF, (void*)&announceDcpt.txBuffSize))
            {
                break;
            }
        }

#if (TCPIP_ANNOUNCE_NETWORK_DIRECTED_BCAST != 0)
        announceDcpt.bcastType = UDP_BCAST_NETWORK_DIRECTED;
#else
        announceDcpt.bcastType = UDP_BCAST_NETWORK_LIMITED;
#endif

        TCPIP_UDP_OptionsSet(s, UDP_OPTION_BROADCAST, (void*)(intptr_t)announceDcpt.bcastType);

        sigHandle = TCPIP_UDP_SignalHandlerRegister(s, TCPIP_UDP_SIGNAL_RX_DATA, _TCPIP_AnnounceSocketRxSignalHandler, 0);
        if(!sigHandle)
        {
            break;
        }


        initFail = false;
        break;
    }

    if(initFail)    
    {
        _TCPIP_AnnounceCleanup();
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
        if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN)
        {
            announceDcpt.requestMask &= ~(1 << stackCtrl->netIx);
        }
        else if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--announceInitCount == 0)
            {   // all closed
                // release resources
                _TCPIP_AnnounceCleanup();
                announceDcpt.memH = 0;
            }
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
    int         nIter;
    bool        sendOk;
    uint8_t*    pAnnBuffer;

    pAnnBuffer = (uint8_t*)alloca(announceDcpt.txBuffSize);

    annSkt = announceDcpt.skt;

    // reset the broadcast type, it may have changed
    TCPIP_UDP_OptionsSet(annSkt, UDP_OPTION_BROADCAST, (void*)(intptr_t)announceDcpt.bcastType);

    nIter = 0;
    while(true)
    {
        // reply to the request on the interface it arrived on
        if((announceDcpt.requestMask & (1 << announceDcpt.currIx)) != 0)
        {
            sendOk = TCPIP_ANNOUNCE_SendIf(announceDcpt.currIx, &announceDcpt, pAnnBuffer);
            _AnnounceDbgSendEvent(announceDcpt.currIx, announceDcpt.requestMask, sendOk);
            
            if(!sendOk)
            {   // failed to send; try next time
                break;
            }

            announceDcpt.requestMask &= ~(1 << announceDcpt.currIx);   // clear requests on this interface
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
    TCPIP_UDP_OptionsSet(annSkt, UDP_OPTION_STRICT_NET, 0);
}

// send a message on the specified interface
static bool TCPIP_ANNOUNCE_SendIf(int netIx, TCPIP_ANNOUNCE_DCPT* pDcpt, uint8_t* pAnnBuffer)
{
    UDP_SOCKET  annSkt;
    TCPIP_NET_IF *pNetIf;
    IPV4_ADDR ifAdd;
    TCPIP_ANNOUNCE_SEND_DCPT annDcpt;
    TCPIP_ANNOUNCE_TRUNC_DCPT* pTDcpt;
    uint8_t*    outData;
    bool        isTruncated;

    annSkt = pDcpt->skt;

    pNetIf = (TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(netIx);

    if(TCPIP_STACK_NetworkIsLinked(pNetIf))
    {   // reply only if this interface is up and running

        if (TCPIP_UDP_PutIsReady(annSkt) < pDcpt->txBuffSize)
        {   // couldn't allocate buffer; try later
            return false;
        }

        annDcpt.pMsg = (TCPIP_ANNOUNCE_MESSAGE*)pAnnBuffer;
        annDcpt.pNetIf = pNetIf;
        annDcpt.msgLen = 0;
        annDcpt.leftLen = pDcpt->txBuffSize;
        annDcpt.pWrPtr = pAnnBuffer;
        annDcpt.usrCback = pDcpt->usrCback;

        pTDcpt = &annDcpt.pMsg->truncDcpt;
        if(_AnnounceFieldProcess(&annDcpt))
        {   // everything went well
            // skip the truncation field
            annDcpt.msgLen -= sizeof(*pTDcpt);
            outData = (uint8_t*)(pTDcpt + 1);
            isTruncated = false;
        }
        else
        {   // it's truncated, not everything went through
            outData = (uint8_t*)pTDcpt;
            isTruncated = true;
        }

        TCPIP_UDP_SocketNetSet (annSkt, pNetIf);
        ifAdd.Val = _TCPIPStackNetAddress(pNetIf);
        TCPIP_UDP_SourceIPAddressSet(annSkt, IP_ADDRESS_TYPE_IPV4, (IP_MULTI_ADDRESS*)&ifAdd);
        TCPIP_UDP_ArrayPut (annSkt, outData, annDcpt.msgLen);

        if(!isTruncated && pDcpt->usrCback != 0)
        {   // call the user handler
            (*pDcpt->usrCback)(pNetIf, annSkt);
        }
        TCPIP_UDP_Flush (annSkt);
    }

    return true;

}

void TCPIP_ANNOUNCE_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if((sigPend & TCPIP_MODULE_SIGNAL_TMO) != 0)
    { // regular TMO occurred
        TCPIP_ANNOUNCE_Timeout();
    }

    if((sigPend & TCPIP_MODULE_SIGNAL_ASYNC) != 0)
    { // async message occurred
        // clear the ASYNC request
        _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_ASYNC);
        TCPIP_ANNOUNCE_Send();
    }

}

// send a signal to the Announce module that data is available
// no manager alert needed since this normally results as a higher layer (UDP) signal
static void _TCPIP_AnnounceSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_UDP_SIGNAL_RX_DATA)
    {
        _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
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
        if(!TCPIP_UDP_GetIsReady(s))
        {
            return;
        }
			
        // See if this is a discovery query or reply
        TCPIP_UDP_Get(s, &discQuery);
        if(discQuery == 'D')
        {   // We received a discovery request, reply
            TCPIP_UDP_SocketInfoGet(s, &sktInfo);
            // fake a legitimate DHCP event on that interface	
            ANNOUNCE_Notify (sktInfo.hNet, DHCP_EVENT_BOUND, (const void*)TCPIP_ANNOUNCE_EVENT_REMOTE_REQUEST);
        }
        TCPIP_UDP_Discard(s);
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
static void ANNOUNCE_Notify(TCPIP_NET_HANDLE hNet, uint8_t dhcpEvType, const void * param)
{
    if(announceDcpt.memH)
    {
        if(dhcpEvType == DHCP_EVENT_BOUND || dhcpEvType == DHCP_EVENT_CONN_LOST || dhcpEvType == DHCP_EVENT_SERVICE_DISABLED)
        {
            if(hNet != 0)
            {
                TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNet(hNet);
                announceDcpt.requestMask |= (1 << pNetIf->netIfIx);

#if (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
                TCPIP_ANNOUNCE_EVENT_TYPE annEvType = (TCPIP_ANNOUNCE_EVENT_TYPE)param;
                if(annEvType == TCPIP_ANNOUNCE_EVENT_REMOTE_REQUEST && _TCPIPStackNetIsPrimary(pNetIf))
                {
                    TCPIP_NET_IF* pIf;
                    for(pIf = _TCPIPStackNetGetAlias(pNetIf); pIf != 0; pIf = _TCPIPStackNetGetAlias(pIf)) 
                    {
                        announceDcpt.requestMask |= (1 << pIf->netIfIx);
                    }
                }
#endif  // (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
            }
            _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_ASYNC, 0); 
            _AnnounceDbgNotifyEvent(hNet, (TCPIP_ANNOUNCE_EVENT_TYPE)param, announceDcpt.requestMask, dhcpEvType);
        }
    }
}

#if defined (TCPIP_STACK_USE_IPV6)
static void IPv6_Announce_Notify(TCPIP_NET_HANDLE hNet, uint8_t addEvType, const void * param)
{
    if(announceDcpt.memH)
    {
        if(addEvType == IPV6_EVENT_ADDRESS_ADDED || addEvType == IPV6_EVENT_ADDRESS_REMOVED || addEvType == IPV6_EVENT_ULA_ADDRESS_GENERATED)                
        {
            TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNet(hNet);
            announceDcpt.requestMask |= (1 << pNetIf->netIfIx);
            _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_ASYNC, 0); 
            _AnnounceDbgNotifyEvent(hNet, TCPIP_ANNOUNCE_EVENT_IPV6_ADDRESS, announceDcpt.requestMask, addEvType);
        }
    }
}
#endif    //TCPIP_STACK_USE_IPV6 

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void _TCPIP_AnnounceCleanup(void)
{
#if defined   ( TCPIP_STACK_USE_DHCP_CLIENT)
    if (announceDcpt.dhcpHandler != NULL)
    {
        TCPIP_DHCP_HandlerDeRegister(announceDcpt.dhcpHandler);
        announceDcpt.dhcpHandler = NULL;
    }
#endif
#if defined (TCPIP_STACK_USE_IPV6)
    if (announceDcpt.ipv6Handler != NULL)
    {
        TCPIP_IPV6_HandlerDeregister(announceDcpt.ipv6Handler);
        announceDcpt.ipv6Handler = NULL;
    }
#endif

    if(announceDcpt.sigHandle != 0)
    {
        _TCPIPStackSignalHandlerDeregister(announceDcpt.sigHandle);
        announceDcpt.sigHandle = 0;
    }

    if(announceDcpt.skt != INVALID_UDP_SOCKET)
    {
        TCPIP_UDP_Close(announceDcpt.skt);
        announceDcpt.skt = INVALID_UDP_SOCKET;
    }

    announceDcpt.requestMask = 0;
    announceDcpt.nIfs = 0;

}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

// returns true if manages to put all message in the pSDcpt buffer
// false if there was not enough room
// number of bytes set in the buffer is updated in pSDcpt
static bool _AnnounceFieldProcess(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt)
{
    int ix;
    int fieldBytes;
    const _AnnounceFieldFnc* pFieldFnc;

    pFieldFnc = _AnnounceSendTbl;
    for(ix = 0; ix < sizeof(_AnnounceSendTbl) / sizeof(*_AnnounceSendTbl); ix++, pFieldFnc++)
    {
        pSDcpt->fieldType = (TCPIP_ANNOUNCE_FIELD_PAYLOAD)ix; 
#if defined (TCPIP_STACK_USE_IPV6)
        pSDcpt->currAddress = 0;
        pSDcpt->currRouter = 0;
        pSDcpt->currGateway = 0;
#endif  //  defined (TCPIP_STACK_USE_IPV6)
        fieldBytes = (*pFieldFnc)(pSDcpt);
        if(fieldBytes < 0)
        {   // failed
            return false;
        }

        // success
        pSDcpt->msgLen += fieldBytes;
        pSDcpt->leftLen -= fieldBytes;
        pSDcpt->pWrPtr += fieldBytes;

        if(fieldBytes != 0)
        {   // set the terminator
            *(pSDcpt->pWrPtr - 2) = _ANNOUNCE_SEPARATOR_1;
            *(pSDcpt->pWrPtr - 1) = _ANNOUNCE_SEPARATOR_2;
        }
    }

    return true;
}

// dummy function
// TCPIP_ANNOUNCE_FIELD_NONE
int _FieldSetNone(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt)
{
    return 0;
}

// just make it not truncated for now; it will be set only when needed
// TCPIP_ANNOUNCE_FIELD_TRUNCATED
int _FieldSetTruncated(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt)
{
    TCPIP_ANNOUNCE_TRUNC_DCPT* pTDcpt = (TCPIP_ANNOUNCE_TRUNC_DCPT*)pSDcpt->pWrPtr;
    
    if(pSDcpt->leftLen >= sizeof(*pTDcpt))
    {
        pTDcpt->type = TCPIP_ANNOUNCE_FIELD_TRUNCATED;
        return sizeof(*pTDcpt);
    }

    // failed
    return -1;
}

// insert a terminator only if there's user data
int _FieldSetUserStart(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt)
{
    return (pSDcpt->usrCback != 0) ? sizeof(TCPIP_ANNOUNCE_USER_START) : 0;
}

// TCPIP_ANNOUNCE_FIELD_MAC_ADDR
int _FieldSetMacAddr(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt)
{
    TCPIP_ANNOUNCE_MAC_ADDR_DCPT* pMDcpt = (TCPIP_ANNOUNCE_MAC_ADDR_DCPT*)pSDcpt->pWrPtr;
    
    if(pSDcpt->leftLen >= sizeof(*pMDcpt))
    {
        pMDcpt->type = TCPIP_ANNOUNCE_FIELD_MAC_ADDR;
        memcpy(pMDcpt->macAddr, _TCPIPStack_NetMACAddressGet(pSDcpt->pNetIf), sizeof(pMDcpt->macAddr));
        return sizeof(*pMDcpt);
    }

    // failed
    return -1;
}

// TCPIP_ANNOUNCE_FIELD_MAC_NAME
int _FieldSetMacName(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt)
{
    TCPIP_ANNOUNCE_MAC_NAME_DCPT* pNDcpt = (TCPIP_ANNOUNCE_MAC_NAME_DCPT*)pSDcpt->pWrPtr;
    const char* macName = TCPIP_STACK_MACIdToString(pSDcpt->pNetIf->macId);
    int nameLen = strlen(macName);

    
    if(pSDcpt->leftLen >= sizeof(*pNDcpt) + nameLen)
    {
        pNDcpt->type = TCPIP_ANNOUNCE_FIELD_MAC_NAME;
        strcpy(pNDcpt->macName, macName);
        return sizeof(*pNDcpt) + nameLen;
    }

    // failed
    return -1;
}

// TCPIP_ANNOUNCE_FIELD_HOST_NAME
int _FieldSetHostName(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt)
{
    TCPIP_ANNOUNCE_HOST_NAME_DCPT* pHNDcpt = (TCPIP_ANNOUNCE_HOST_NAME_DCPT*)pSDcpt->pWrPtr;
    const char* hostName = _TCPIPStack_NetBIOSName(pSDcpt->pNetIf);
    int hostLen = strlen(hostName);

    
    if(pSDcpt->leftLen >= sizeof(*pHNDcpt) + hostLen)
    {
        pHNDcpt->type = TCPIP_ANNOUNCE_FIELD_HOST_NAME;
        strcpy(pHNDcpt->hostName, hostName);
        return sizeof(*pHNDcpt) + hostLen;
    }

    // failed
    return -1;
}

// TCPIP_ANNOUNCE_FIELD_IPV4_ADDRESS
#if defined (TCPIP_STACK_USE_IPV4)
int _FieldSetIpv4Addr(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt)
{
    TCPIP_ANNOUNCE_IPV4_ADDR_DCPT* pIpDcpt = (TCPIP_ANNOUNCE_IPV4_ADDR_DCPT*)pSDcpt->pWrPtr;
    IPV4_ADDR ipAdd;

    
    if(pSDcpt->leftLen >= sizeof(*pIpDcpt))
    {
        pIpDcpt->type = TCPIP_ANNOUNCE_FIELD_IPV4_ADDRESS;
        ipAdd.Val = _TCPIPStackNetAddress(pSDcpt->pNetIf);
        memcpy(pIpDcpt->ipv4Addr, ipAdd.v, sizeof(ipAdd));
        return sizeof(*pIpDcpt);
    }

    // failed
    return -1;
}
#endif  //  defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV6)

const IPV6_ADDR* _FieldIpV6NextAddress(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt)
{
    IPV6_INTERFACE_CONFIG*  pIpv6Config;

    if(pSDcpt->fieldType == TCPIP_ANNOUNCE_FIELD_IPV6_UNICAST || pSDcpt->fieldType == TCPIP_ANNOUNCE_FIELD_IPV6_MULTICAST)
    {
        IPV6_ADDR_STRUCT* addressPtr;

        if(pSDcpt->currAddress == 0)
        {
            pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pSDcpt->pNetIf);
            DOUBLE_LIST* pHead = pSDcpt->fieldType == TCPIP_ANNOUNCE_FIELD_IPV6_UNICAST ? &pIpv6Config->listIpv6UnicastAddresses : &pIpv6Config->listIpv6MulticastAddresses;
            addressPtr = (IPV6_ADDR_STRUCT *)pHead->head;
        }
        else
        {
            addressPtr = pSDcpt->currAddress->next;
        }

        return (pSDcpt->currAddress = addressPtr) == 0 ? 0 : &addressPtr->address;
    }

    if(pSDcpt->fieldType == TCPIP_ANNOUNCE_FIELD_IPV6_DEFAULT_ROUTER)
    {
        IPV6_HEAP_NDP_DR_ENTRY* routerPtr;

        if(pSDcpt->currRouter == 0)
        {
            pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pSDcpt->pNetIf);
            routerPtr = pIpv6Config->currentDefaultRouter;
        }
        else
        {
            routerPtr = pSDcpt->currRouter->next;
        }

        pSDcpt->currRouter = routerPtr;

        if(routerPtr == 0 || routerPtr->neighborInfo == 0)
        {
           return 0;
        }

        return  &routerPtr->neighborInfo->remoteIPAddress;

    }

    if(pSDcpt->fieldType == TCPIP_ANNOUNCE_FIELD_IPV6_DEFAULT_GATEWAY)
    {
        if(pSDcpt->currGateway == 0)
        {
            return (pSDcpt->currGateway = TCPIP_STACK_NetDefaultIPv6GatewayGet(pSDcpt->pNetIf));
        }

        // return only one default gateway, no list!
        return 0;
    }

    // unknown field type
    return 0;
}


// TCPIP_ANNOUNCE_FIELD_IPV6_UNICAST
// TCPIP_ANNOUNCE_FIELD_IPV6_MULTICAST
// TCPIP_ANNOUNCE_FIELD_IPV6_DEFAULT_ROUTER
// TCPIP_ANNOUNCE_FIELD_IPV6_DEFAULT_GATEWAY
int _FieldSetIpv6Address(TCPIP_ANNOUNCE_SEND_DCPT* pSDcpt)
{
    TCPIP_ANNOUNCE_IPV6_ADDR_DCPT*  pIp6Dcpt;
    const IPV6_ADDR*    pCurrAddress;
    int                 addressLen, leftLen, nAddresses;


    // calculate the needed size
    addressLen = nAddresses = 0;
    leftLen = pSDcpt->leftLen;

    pIp6Dcpt = (TCPIP_ANNOUNCE_IPV6_ADDR_DCPT*)pSDcpt->pWrPtr;
    for(pCurrAddress = _FieldIpV6NextAddress(pSDcpt); pCurrAddress != 0; pCurrAddress = _FieldIpV6NextAddress(pSDcpt), pIp6Dcpt++)
    {
        nAddresses++;
        if(leftLen < addressLen + sizeof(*pIp6Dcpt))
        {   // no more room, done
            break;
        }

        // set this entry
        pIp6Dcpt->type = (uint8_t)pSDcpt->fieldType;
        memcpy(pIp6Dcpt->ipv6Addr, pCurrAddress->v, sizeof(pIp6Dcpt->ipv6Addr));
        pIp6Dcpt->separator[0] = _ANNOUNCE_SEPARATOR_1;
        pIp6Dcpt->separator[1] = _ANNOUNCE_SEPARATOR_2;

        addressLen += sizeof(*pIp6Dcpt);
    }

    // return -1 if no space, 0 if no IPv6 addresses
    return addressLen != 0 ? addressLen : nAddresses == 0 ? 0 : -1;
}

#endif  //  defined (TCPIP_STACK_USE_IPV6)

bool TCPIP_ANNOUNCE_MessageRequest(TCPIP_NET_HANDLE hNet, TCPIP_ANNOUNCE_BROADCAST_TYPE bcastType)
{
    uint32_t reqMask;
    OSAL_CRITSECT_DATA_TYPE status;

    while(announceDcpt.nIfs != 0)
    {   // up and running
        if(hNet != 0)
        {
            TCPIP_NET_IF* pIf = _TCPIPStackHandleToNetLinked(hNet);
            if(pIf == 0)
            {
                break;
            }
            reqMask = 1 << pIf->netIfIx; 
        }
        else
        {
            reqMask = (1 << announceDcpt.nIfs) - 1;
        }


        status =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        if(bcastType != TCPIP_ANNOUNCE_BCAST_NONE)
        {
            announceDcpt.bcastType = bcastType == (TCPIP_ANNOUNCE_BCAST_NET_DIRECTED) ? UDP_BCAST_NETWORK_DIRECTED : UDP_BCAST_NETWORK_LIMITED;
        }
        announceDcpt.requestMask |= reqMask;
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
        // force notification
        ANNOUNCE_Notify (0, DHCP_EVENT_BOUND, (const void*)TCPIP_ANNOUNCE_EVENT_USER_REQUEST);


        return true;
    }

    return false;
}

bool TCPIP_ANNOUNCE_CallbackRegister(TCPIP_ANNOUNCE_MESSAGE_CALLBACK callback)
{
    while(announceDcpt.nIfs != 0)
    {   // up and running
        announceDcpt.usrCback = callback;
        return true; 
    }

    return false;
}

#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_ANNOUNCE)

