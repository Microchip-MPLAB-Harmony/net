/*******************************************************************************
  Internet Control Message Protocol (ICMP) Server

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    - Provides "ping" diagnostics
    - Reference: RFC 792
*******************************************************************************/

/*******************************************************************************
File Name:  ICMP.c
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
#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_ICMP

#include "tcpip/src/tcpip_private.h"


#if defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_ICMP_SERVER) || defined(TCPIP_STACK_USE_ICMP_CLIENT)


// ICMP Packet Structure
typedef struct
{
	uint8_t vType;
	uint8_t vCode;
	uint16_t wChecksum;
	uint16_t wIdentifier;
	uint16_t wSequenceNumber;
	uint32_t wData[0];      // payload
} ICMP_PACKET;

#define ICMP_TYPE_ECHO_REQUEST      8   // ICMP server is requested echo - type
#define ICMP_CODE_ECHO_REQUEST      0   // ICMP server is requested echo - code


#define ICMP_TYPE_ECHO_REPLY        0   // ICMP client echo reply - type
#define ICMP_CODE_ECHO_REPLY        0   // ICMP client echo reply - code



static int                  icmpInitCount = 0;  // ICMP module initialization count

static const void*          icmpMemH = 0;       // memory handle

static tcpipSignalHandle    signalHandle = 0;   // registered signal handler   

#if defined(TCPIP_STACK_USE_ICMP_CLIENT)

// Callback function for informing the upper-layer protocols about ICMP events
typedef void (*icmpCallback) (TCPIP_NET_HANDLE hNetIf, IPV4_ADDR * remoteIP, void * data);

#if (TCPIP_ICMP_CLIENT_USER_NOTIFICATION != 0)
static PROTECTED_SINGLE_LIST      icmpRegisteredUsers = { {0} };
#endif  // (TCPIP_ICMP_CLIENT_USER_NOTIFICATION != 0)
//
// ICMP callback registration
typedef struct  _TAG_ICMP_LIST_NODE
{
	struct _TAG_ICMP_LIST_NODE* next;		// next node in list
                                            // makes it valid SGL_LIST_NODE node
    icmpCallback                callback;   // handler to be called for ICMP event
}ICMP_LIST_NODE;


static TCPIP_ICMP_ECHO_REQUEST     icmpEchoRequest;            // one and only request (for now)
static TCPIP_ICMP_ECHO_REQUEST*    pIcmpEchoRequest = 0;       // one and only request (for now)
static uint32_t                    icmpEchoStart;              // tick when the request started 
static uint32_t                    icmpEchoTmo;                // # of ticks for a request to time out 

#endif

// local prototypes
static void  TCPIP_ICMP_Process(void);
#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void TCPIP_ICMP_Cleanup(void);
#else
#define TCPIP_ICMP_Cleanup()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

#if defined(TCPIP_STACK_USE_ICMP_SERVER)
static bool _ICMPProcessEchoRequest(TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pRxPkt, uint32_t destAdd, uint32_t srcAdd);
#endif // defined(TCPIP_STACK_USE_ICMP_SERVER)

#if defined(TCPIP_STACK_USE_ICMP_CLIENT)
static IPV4_PACKET * _ICMPAllocateTxPacketStruct (uint16_t totICMPLen);
static bool _ICMPTxPktAcknowledge(TCPIP_MAC_PACKET* pkt, const void* ackParam);
static void  TCPIP_ICMP_Timeout(void);
#if (TCPIP_ICMP_CLIENT_USER_NOTIFICATION != 0)
static void _ICMPNotifyClients(TCPIP_NET_HANDLE hNetIf, IPV4_ADDR * remoteIP, void * data);
#else
#define _ICMPNotifyClients(hNetIf, remoteIP, data)
#endif  // (TCPIP_ICMP_CLIENT_USER_NOTIFICATION != 0)
#endif  // defined(TCPIP_STACK_USE_ICMP_CLIENT) && (TCPIP_ICMP_CLIENT_USER_NOTIFICATION != 0)


#if (TCPIP_IPV4_FRAGMENTATION != 0)
static void     _ICMPRxPktAcknowledge(TCPIP_MAC_PACKET* pRxPkt, TCPIP_MAC_PKT_ACK_RES ackRes);
#else
static __inline__ void __attribute__((always_inline)) _ICMPRxPktAcknowledge(TCPIP_MAC_PACKET* pRxPkt, TCPIP_MAC_PKT_ACK_RES ackRes)
{
    TCPIP_PKT_PacketAcknowledge(pRxPkt, ackRes);
}
#endif  // (TCPIP_IPV4_FRAGMENTATION != 0)



bool TCPIP_ICMP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_ICMP_MODULE_CONFIG* const pIcmpInit)
{
    if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }

    // stack start up
    if(icmpInitCount == 0)
    {   // first time we're run
        bool iniRes;

        icmpMemH = stackCtrl->memH;
        while(true)
        {
#if defined(TCPIP_STACK_USE_ICMP_CLIENT)
            iniRes = (signalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_ICMP_Task, TCPIP_ICMP_TASK_TICK_RATE)) != 0;
#else
            iniRes = (signalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_ICMP_Task, 0)) != 0;
#endif  // defined(TCPIP_STACK_USE_ICMP_CLIENT)
            if(iniRes == false)
            {
                break;
            }
#if defined(TCPIP_STACK_USE_ICMP_CLIENT)
            pIcmpEchoRequest = 0;        // one and only request (for now)
            icmpEchoTmo = 0;
#if (TCPIP_ICMP_CLIENT_USER_NOTIFICATION != 0)
            iniRes = TCPIP_Notification_Initialize(&icmpRegisteredUsers);
#endif  // (TCPIP_ICMP_CLIENT_USER_NOTIFICATION != 0)
#endif  // defined(TCPIP_STACK_USE_ICMP_CLIENT)
            break;
        }
        if(iniRes == false)
        {
            TCPIP_ICMP_Cleanup();
            return false;
        }
    }

    // interface init
    // postpone packet allocation until the TCPIP_ICMP_EchoRequestSend is called
    //

    icmpInitCount++;
    return true;
}


#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_ICMP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN) // interface down

    if(icmpInitCount > 0)
    {   // we're up and running
        // one way or another this interface is going down

        if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
        {   // stack shut down
            if(--icmpInitCount == 0)
            {   // all closed. release resources
                TCPIP_ICMP_Cleanup();
            }
        }
    }
}

static void TCPIP_ICMP_Cleanup(void)
{
#if defined(TCPIP_STACK_USE_ICMP_CLIENT) && (TCPIP_ICMP_CLIENT_USER_NOTIFICATION != 0)
    TCPIP_Notification_Deinitialize(&icmpRegisteredUsers, icmpMemH);
#endif  // defined(TCPIP_STACK_USE_ICMP_CLIENT) && (TCPIP_ICMP_CLIENT_USER_NOTIFICATION != 0)
    if(signalHandle)
    {
        _TCPIPStackSignalHandlerDeregister(signalHandle);
        signalHandle = 0;
    }
    icmpMemH = 0;
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

#if defined(TCPIP_STACK_USE_ICMP_CLIENT) && (TCPIP_ICMP_CLIENT_USER_NOTIFICATION != 0)
ICMP_HANDLE TCPIP_ICMP_CallbackRegister (void (*callback)(TCPIP_NET_HANDLE hNetIf, IPV4_ADDR * remoteIP, void * data))
{
    if(callback && icmpMemH)
    {
        ICMP_LIST_NODE* newNode = (ICMP_LIST_NODE*)TCPIP_Notification_Add(&icmpRegisteredUsers, icmpMemH, sizeof(*newNode));
        if(newNode)
        {
            newNode->callback = callback;
            return newNode;
        }
    }

    return 0;
}


bool TCPIP_ICMP_CallbackDeregister(ICMP_HANDLE hIcmp)
{
    if(hIcmp && icmpMemH)
    {
        if(TCPIP_Notification_Remove((SGL_LIST_NODE*)hIcmp, &icmpRegisteredUsers, icmpMemH))
        {
            return true;
        }
    }

    return false;


}
#endif  // defined(TCPIP_STACK_USE_ICMP_CLIENT) && (TCPIP_ICMP_CLIENT_USER_NOTIFICATION != 0)


#if defined (TCPIP_STACK_USE_ICMP_CLIENT)

static IPV4_PACKET * _ICMPAllocateTxPacketStruct (uint16_t totICMPLen)
{
    IPV4_PACKET * ptrPacket;

    ptrPacket = (IPV4_PACKET*)TCPIP_PKT_SocketAlloc(sizeof(IPV4_PACKET), totICMPLen, 0, TCPIP_MAC_PKT_FLAG_ICMPV4 | TCPIP_MAC_PKT_FLAG_IPV4 | TCPIP_MAC_PKT_FLAG_TX);

    if (ptrPacket != 0)
    {
        TCPIP_PKT_PacketAcknowledgeSet(&ptrPacket->macPkt, _ICMPTxPktAcknowledge, 0);
    }

    return ptrPacket;
}

// packet deallocation function
// packet was transmitted by the IP layer
static bool _ICMPTxPktAcknowledge(TCPIP_MAC_PACKET* pTxPkt, const void* ackParam)
{
    TCPIP_PKT_PacketFree(pTxPkt);
    return false;
}

ICMP_ECHO_RESULT TCPIP_ICMP_EchoRequest (TCPIP_ICMP_ECHO_REQUEST* pEchoRequest, TCPIP_ICMP_REQUEST_HANDLE* pHandle)
{
    IPV4_PACKET*    pTxPkt;
    ICMP_PACKET*    pICMPPkt;
    uint16_t        pktSize;
    ICMP_ECHO_RESULT res;

    if(pHandle)
    {
        *pHandle = 0;
    }

    if(pIcmpEchoRequest != 0)
    {   // already going on
        return ICMP_ECHO_BUSY;
    }

    if(pEchoRequest == 0)
    {
        return ICMP_ECHO_PARAMETER_ERROR;
    }

    while(true)
    {
        // allocate TX packet
        pktSize = sizeof(ICMP_PACKET) + pEchoRequest->dataSize;
        pTxPkt = _ICMPAllocateTxPacketStruct(pktSize);
        if(pTxPkt == 0)
        {
            res = ICMP_ECHO_ALLOC_ERROR;
            break;
        }

        pICMPPkt = (ICMP_PACKET*)pTxPkt->macPkt.pTransportLayer;

        pICMPPkt->vType = ICMP_TYPE_ECHO_REQUEST; 
        pICMPPkt->vCode = ICMP_CODE_ECHO_REQUEST;
        pICMPPkt->wChecksum = 0x0000;
        pICMPPkt->wIdentifier = pEchoRequest->identifier;
        pICMPPkt->wSequenceNumber = pEchoRequest->sequenceNumber;
        memcpy(pICMPPkt->wData, pEchoRequest->pData, pEchoRequest->dataSize);
        pICMPPkt->wChecksum = TCPIP_Helper_CalcIPChecksum((uint8_t*)pICMPPkt, pktSize, 0);
        pTxPkt->destAddress.Val = pEchoRequest->targetAddr.Val;

        pTxPkt->netIfH = TCPIP_IPV4_SelectSourceInterface(pEchoRequest->netH, &pTxPkt->destAddress, &pTxPkt->srcAddress, false);
        if(pTxPkt->netIfH == 0)
        {   // could not find an route
            res = ICMP_ECHO_ROUTE_ERROR;
            break;
        }

        icmpEchoRequest = *pEchoRequest;
        pTxPkt->macPkt.pDSeg->segLen += pktSize;
        TCPIP_IPV4_PacketFormatTx(pTxPkt, IP_PROT_ICMP, pktSize, 0);
        TCPIP_PKT_FlightLogTx(&pTxPkt->macPkt, TCPIP_THIS_MODULE_ID);
        if(!TCPIP_IPV4_PacketTransmit(pTxPkt))
        {
            TCPIP_PKT_FlightLogAcknowledge(&pTxPkt->macPkt, TCPIP_THIS_MODULE_ID, TCPIP_MAC_PKT_ACK_IP_REJECT_ERR);
            res = ICMP_ECHO_TRANSMIT_ERROR;
            break;
        }

        pIcmpEchoRequest = &icmpEchoRequest; // mark as busy
        icmpEchoStart = SYS_TMR_TickCountGet();

        if(pHandle)
        {
            *pHandle = pIcmpEchoRequest;
        }

        res = ICMP_ECHO_OK;
        break;
    }

    if(res < 0 && pTxPkt != 0)
    {
        TCPIP_PKT_PacketFree(&pTxPkt->macPkt);
    }

    return res;
}

// cancel a echo request
// Currently only 1 request at a time is supported 
// so the icmpHandle is unused
ICMP_ECHO_RESULT TCPIP_ICMP_EchoRequestCancel (TCPIP_ICMP_REQUEST_HANDLE icmpHandle)
{
    if(icmpHandle == pIcmpEchoRequest && pIcmpEchoRequest != 0)
    {   // we have an extended  request in place
        pIcmpEchoRequest = 0;    // free
        return ICMP_ECHO_OK;
    }

    return ICMP_ECHO_BAD_HANDLE;
}

ICMP_ECHO_RESULT TCPIP_ICMP_EchoRequestSend (TCPIP_NET_HANDLE netH, IPV4_ADDR * targetAddr, uint16_t sequenceNumber, uint16_t identifier)
{
    IPV4_PACKET*    pTxPkt;
    ICMP_PACKET*    pICMPPkt;
    uint32_t        payload = 0x44332211;
    uint16_t        pktSize = sizeof(ICMP_PACKET) + sizeof(payload);
    ICMP_ECHO_RESULT res;


    while(true)
    {
        // allocate TX packet
        pTxPkt = _ICMPAllocateTxPacketStruct(pktSize);
        if(pTxPkt == 0)
        {
            res = ICMP_ECHO_ALLOC_ERROR;
            break;
        }

        pICMPPkt = (ICMP_PACKET*)pTxPkt->macPkt.pTransportLayer;

        pICMPPkt->vType = ICMP_TYPE_ECHO_REQUEST; 
        pICMPPkt->vCode = ICMP_CODE_ECHO_REQUEST;
        pICMPPkt->wChecksum = 0x0000;
        pICMPPkt->wIdentifier = identifier;
        pICMPPkt->wSequenceNumber = sequenceNumber;
        pICMPPkt->wData[0] = payload;
        pICMPPkt->wChecksum = TCPIP_Helper_CalcIPChecksum((uint8_t*)pICMPPkt, pktSize, 0);
        pTxPkt->destAddress.Val = targetAddr->Val;

        pTxPkt->netIfH = TCPIP_IPV4_SelectSourceInterface(netH, targetAddr, &pTxPkt->srcAddress, false);
        if(pTxPkt->netIfH == 0)
        {   // could not find an route
            res = ICMP_ECHO_ROUTE_ERROR;
            break;
        }

        pTxPkt->macPkt.pDSeg->segLen += pktSize;
        TCPIP_IPV4_PacketFormatTx(pTxPkt, IP_PROT_ICMP, pktSize, 0);
        TCPIP_PKT_FlightLogTx(&pTxPkt->macPkt, TCPIP_THIS_MODULE_ID);
        if(!TCPIP_IPV4_PacketTransmit(pTxPkt))
        {
            TCPIP_PKT_FlightLogAcknowledge(&pTxPkt->macPkt, TCPIP_THIS_MODULE_ID, TCPIP_MAC_PKT_ACK_IP_REJECT_ERR);
            res = ICMP_ECHO_TRANSMIT_ERROR;
            break;
        }

        res = ICMP_ECHO_OK;
        break;
    }

    if(res < 0 && pTxPkt != 0)
    {
        TCPIP_PKT_PacketFree(&pTxPkt->macPkt);
    }

    return res;
}


#endif

void  TCPIP_ICMP_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if((sigPend & TCPIP_MODULE_SIGNAL_RX_PENDING) != 0)
    { //  RX signal occurred
        TCPIP_ICMP_Process();
    }

#if defined(TCPIP_STACK_USE_ICMP_CLIENT)
    if((sigPend & TCPIP_MODULE_SIGNAL_TMO) != 0)
    { // regular TMO occurred
        TCPIP_ICMP_Timeout();
    }
#endif  // defined(TCPIP_STACK_USE_ICMP_CLIENT)

}

// Processes an ICMP packet and generates an echo reply, if requested
static void  TCPIP_ICMP_Process(void)
{
    TCPIP_MAC_PACKET        *pRxPkt;
    IPV4_HEADER*            pIpv4Header;
    uint32_t                srcAdd;
    ICMP_PACKET*            pRxHdr;
    uint16_t                icmpTotLength;
    uint16_t                checksum;
    TCPIP_MAC_PKT_ACK_RES   ackRes;



    // extract queued ICMP packets
    while((pRxPkt = _TCPIPStackModuleRxExtract(TCPIP_THIS_MODULE_ID)) != 0)
    {
        TCPIP_PKT_FlightLogRx(pRxPkt, TCPIP_THIS_MODULE_ID);
        pRxHdr = (ICMP_PACKET*)pRxPkt->pTransportLayer;
        ackRes = TCPIP_MAC_PKT_ACK_RX_OK;

        pIpv4Header = (IPV4_HEADER*)pRxPkt->pNetLayer;
        srcAdd =  pIpv4Header->SourceAddress.Val;


        while(true)
        {
            icmpTotLength = pRxPkt->totTransportLen;    // length of the 1st segment (if fragmented)

            if(icmpTotLength < sizeof(*pRxHdr))
            {
                ackRes = TCPIP_MAC_PKT_ACK_STRUCT_ERR;
                break;
            }

            // Validate the checksum
            // The checksum data includes the precomputed checksum in the header
            // so a valid packet will always have a checksum of 0x0000
            // do it across all fragment segments
#if (TCPIP_IPV4_FRAGMENTATION != 0)
            TCPIP_MAC_PACKET* pFragPkt;
            checksum = 0;
            for(pFragPkt = pRxPkt; pFragPkt != 0; pFragPkt = pFragPkt->pkt_next)
            {
                checksum = ~TCPIP_Helper_PacketChecksum(pFragPkt, pFragPkt->pTransportLayer, pFragPkt->totTransportLen, checksum);
            }
            checksum = ~checksum;
#else
            checksum = TCPIP_Helper_PacketChecksum(pRxPkt, (uint8_t*)pRxHdr, icmpTotLength, 0);
#endif  // (TCPIP_IPV4_FRAGMENTATION != 0)

            if(checksum != 0)
            {
                ackRes = TCPIP_MAC_PKT_ACK_CHKSUM_ERR;
                break;
            }

#if defined(TCPIP_STACK_USE_ICMP_SERVER)
            if(pRxHdr->vType == ICMP_TYPE_ECHO_REQUEST && pRxHdr->vCode == ICMP_CODE_ECHO_REQUEST)
            {   // echo request
                _ICMPProcessEchoRequest((TCPIP_NET_IF*)pRxPkt->pktIf, pRxPkt, pIpv4Header->DestAddress.Val, srcAdd);
                ackRes = TCPIP_MAC_PKT_ACK_NONE;
                break;
            }
#endif // defined(TCPIP_STACK_USE_ICMP_SERVER)

#if defined(TCPIP_STACK_USE_ICMP_CLIENT)
            if(pRxHdr->vType == ICMP_TYPE_ECHO_REPLY && pRxHdr->vCode == ICMP_CODE_ECHO_REPLY)
            {   // echo reply; check if our own
                // Get the sequence number and identifier fields
                if(pIcmpEchoRequest != 0)
                {   // we have an extended  request in place
                    if(pIcmpEchoRequest->identifier == pRxHdr->wIdentifier && pIcmpEchoRequest->sequenceNumber == pRxHdr->wSequenceNumber &&
                            pIcmpEchoRequest->targetAddr.Val == srcAdd)
                    {   // our reply
                        if(pIcmpEchoRequest->callback)
                        {
                            (*pIcmpEchoRequest->callback)(pIcmpEchoRequest, pIcmpEchoRequest, TCPIP_ICMP_ECHO_REQUEST_RES_OK);
                        }
                        pIcmpEchoRequest = 0;    // free
                        ackRes = TCPIP_MAC_PKT_ACK_RX_OK;
                        break;
                    }
                }
                else
                {   // it must be a non-extended query 
#if (TCPIP_ICMP_CLIENT_USER_NOTIFICATION != 0)
                    TCPIP_UINT32_VAL userData;
                    IPV4_ADDR remoteIPAddr;

                    userData.w[0] = pRxHdr->wIdentifier;
                    userData.w[1] = pRxHdr->wSequenceNumber;
                    remoteIPAddr.Val = srcAdd;

                    // Send a message to the application-level Ping driver that we've received an Echo Reply
                    _ICMPNotifyClients((TCPIP_NET_IF*)pRxPkt->pktIf, &remoteIPAddr, (void *)userData.v);
#endif  // (TCPIP_ICMP_CLIENT_USER_NOTIFICATION != 0)
                    ackRes = TCPIP_MAC_PKT_ACK_RX_OK;
                    break;
                }
            }
#endif
            // unknown type
            ackRes = TCPIP_MAC_PKT_ACK_TYPE_ERR;
            break;
        }

        if(ackRes != TCPIP_MAC_PKT_ACK_NONE)
        {
            _ICMPRxPktAcknowledge(pRxPkt, ackRes); 
        }
    }
}

// process timeouts: ping requests sent, for which there was no answer
#if defined(TCPIP_STACK_USE_ICMP_CLIENT)
static void  TCPIP_ICMP_Timeout(void)
{
    if(icmpEchoTmo == 0)
    {
        icmpEchoTmo = (TCPIP_ICMP_ECHO_REQUEST_TIMEOUT * SYS_TMR_TickCounterFrequencyGet() + 999) / 1000;
    }

    if(pIcmpEchoRequest != 0)
    {   // some request in progress
        if((SYS_TMR_TickCountGet() - icmpEchoStart) >= icmpEchoTmo) 
        {   // expired: mark it as invalid
            if(pIcmpEchoRequest->callback)
            {
                (*pIcmpEchoRequest->callback)(pIcmpEchoRequest, pIcmpEchoRequest, TCPIP_ICMP_ECHO_REQUEST_RES_TMO);
            }
            pIcmpEchoRequest = 0;    // free
        }
    }
}
#endif  // defined(TCPIP_STACK_USE_ICMP_CLIENT)


#if defined(TCPIP_STACK_USE_ICMP_SERVER)
// echo request
// use the same packet for reply
// it will be acknowledged by the MAC after transmission
static bool _ICMPProcessEchoRequest(TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pRxPkt, uint32_t destAdd, uint32_t srcAdd)
{
    ICMP_PACKET* pTxHdr;
    TCPIP_UINT16_VAL checksum;

    // adjust the checksum
    pTxHdr = (ICMP_PACKET*)pRxPkt->pTransportLayer;

    pTxHdr->vType = ICMP_TYPE_ECHO_REPLY;
    pTxHdr->vCode = ICMP_CODE_ECHO_REPLY;
    checksum.Val = pTxHdr->wChecksum;
    checksum.v[0] += 8;	// Subtract 0x0800 from the checksum
    if(checksum.v[0] < 8u)
    {
        checksum.v[1]++;
        if(checksum.v[1] == 0u)
        {
            checksum.v[0]++;
        }
    }

    pTxHdr->wChecksum = checksum.Val;
    pRxPkt->next = 0; // single packet

#if (TCPIP_IPV4_FRAGMENTATION != 0)
    TCPIP_MAC_PACKET* pFragPkt;
    for(pFragPkt = pRxPkt; pFragPkt != 0; pFragPkt = pFragPkt->pkt_next)
    {
        TCPIP_PKT_FlightLogTx(pFragPkt, TCPIP_THIS_MODULE_ID);
        TCPIP_IPV4_MacPacketSwitchTxToRx(pFragPkt); 
        pFragPkt->next = 0; // single packet
    }

    if(!TCPIP_IPV4_MacPacketTransmit(pRxPkt, pNetIf, (IPV4_ADDR*)&srcAdd))
    {   // failed; discard the segments
        for(pFragPkt = pRxPkt; pFragPkt != 0; pFragPkt = pFragPkt->pkt_next)
        {
            if((pFragPkt->pktFlags & TCPIP_MAC_PKT_FLAG_QUEUED) == 0)
            {
                TCPIP_PKT_PacketAcknowledge(pFragPkt, TCPIP_MAC_PKT_ACK_MAC_REJECT_ERR);
            }
        }
        return false;
    }
#else
    TCPIP_IPV4_MacPacketSwitchTxToRx(pRxPkt); 
    TCPIP_PKT_FlightLogTx(pRxPkt, TCPIP_THIS_MODULE_ID);
    if(!TCPIP_IPV4_MacPacketTransmit(pRxPkt, pNetIf, (IPV4_ADDR*)&srcAdd))
    {
        TCPIP_PKT_PacketAcknowledge(pRxPkt, TCPIP_MAC_PKT_ACK_MAC_REJECT_ERR);
        return false;
    }
#endif  // (TCPIP_IPV4_FRAGMENTATION != 0)

    // went through
    return true;
}
#endif // defined(TCPIP_STACK_USE_ICMP_SERVER)


#if defined(TCPIP_STACK_USE_ICMP_CLIENT) && (TCPIP_ICMP_CLIENT_USER_NOTIFICATION != 0)

static void _ICMPNotifyClients(TCPIP_NET_HANDLE hNetIf, IPV4_ADDR * remoteIP, void * data)
{
    ICMP_LIST_NODE* dNode;

    TCPIP_Notification_Lock(&icmpRegisteredUsers);
    for(dNode = (ICMP_LIST_NODE*)icmpRegisteredUsers.list.head; dNode != 0; dNode = dNode->next)
    {
        (*dNode->callback)(hNetIf, remoteIP, data);
    }
    TCPIP_Notification_Unlock(&icmpRegisteredUsers);

}

#endif  // defined(TCPIP_STACK_USE_ICMP_CLIENT) && (TCPIP_ICMP_CLIENT_USER_NOTIFICATION != 0)

#if (TCPIP_IPV4_FRAGMENTATION != 0)

static void _ICMPRxPktAcknowledge(TCPIP_MAC_PACKET* pRxPkt, TCPIP_MAC_PKT_ACK_RES ackRes)
{
    TCPIP_MAC_PACKET *pFragPkt, *pFragNext;

    for(pFragPkt = pRxPkt; pFragPkt != 0; pFragPkt = pFragNext)
    {
        pFragNext = pFragPkt->pkt_next;
        TCPIP_PKT_PacketAcknowledge(pFragPkt, ackRes); 
    }
}

#endif  // (TCPIP_IPV4_FRAGMENTATION != 0)

#endif //#if defined(TCPIP_STACK_USE_ICMP_SERVER) || defined(TCPIP_STACK_USE_ICMP_CLIENT)
#endif  // defined(TCPIP_STACK_USE_IPV4)


