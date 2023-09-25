/*******************************************************************************
  Internet Control Message Protocol (ICMP) Server

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    - Provides "ping" diagnostics
    - Reference: RFC 792
*******************************************************************************/

/*
Copyright (C) 2012-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
    uint32_t wData[];      // payload
} ICMP_PACKET;

#define ICMP_TYPE_ECHO_REQUEST      8   // ICMP server is requested echo - type
#define ICMP_CODE_ECHO_REQUEST      0   // ICMP server is requested echo - code


#define ICMP_TYPE_ECHO_REPLY        0   // ICMP client echo reply - type
#define ICMP_CODE_ECHO_REPLY        0   // ICMP client echo reply - code



static int                  icmpInitCount = 0;  // ICMP module initialization count

static const void*          icmpMemH = 0;       // memory handle

static tcpipSignalHandle    signalHandle = 0;   // registered signal handler   

#if defined(TCPIP_STACK_USE_ICMP_CLIENT)

typedef struct _tag_ICMP_ECHO_REQUEST_NODE
{
    struct _tag_ICMP_ECHO_REQUEST_NODE* next;       // safe cast to SGL_LIST_NODE
    uint32_t                            reqStart;   // tick when the request started 
    TCPIP_ICMP_ECHO_REQUEST             request;    // associated request
}ICMP_ECHO_REQUEST_NODE;

static ICMP_ECHO_REQUEST_NODE       echoRequestPool[TCPIP_STACK_MAX_CLIENT_ECHO_REQUESTS];   // the pool of available requests
static SINGLE_LIST                  echoRequestFreeList;        // list of available requests
static SINGLE_LIST                  echoRequestBusyList;        // list of in use requests

static uint32_t                     icmpEchoTmo;                // # of ticks for a request to time out 

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
static void _ICMPTxPktAcknowledge(TCPIP_MAC_PACKET* pkt, const void* ackParam);
static void  TCPIP_ICMP_Timeout(void);

static __inline__ OSAL_CRITSECT_DATA_TYPE __attribute__((always_inline)) _ICMPRequestListLock(void)
{
    return OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
}

static __inline__ void __attribute__((always_inline)) _ICMPRequestListUnlock(OSAL_CRITSECT_DATA_TYPE lock)
{
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, lock);   
}
#endif  // defined(TCPIP_STACK_USE_ICMP_CLIENT)

#if defined(TCPIP_IPV4_FRAGMENTATION) && (TCPIP_IPV4_FRAGMENTATION != 0)
#define _TCPIP_IPV4_FRAGMENTATION 1
#else
#define _TCPIP_IPV4_FRAGMENTATION 0
#endif


#if (_TCPIP_IPV4_FRAGMENTATION != 0)
static void     _ICMPRxPktAcknowledge(TCPIP_MAC_PACKET* pRxPkt, TCPIP_MAC_PKT_ACK_RES ackRes);
#else
static __inline__ void __attribute__((always_inline)) _ICMPRxPktAcknowledge(TCPIP_MAC_PACKET* pRxPkt, TCPIP_MAC_PKT_ACK_RES ackRes)
{
    TCPIP_PKT_PacketAcknowledge(pRxPkt, ackRes);
}
#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)



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

        (void)icmpMemH;
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
            TCPIP_Helper_SingleListInitialize(&echoRequestFreeList);
            TCPIP_Helper_SingleListInitialize(&echoRequestBusyList);
            int ix;
            ICMP_ECHO_REQUEST_NODE* pNode = echoRequestPool;
            for(ix = 0; ix < sizeof(echoRequestPool) / sizeof(*echoRequestPool); ix++, pNode++)
            {
                TCPIP_Helper_SingleListTailAdd(&echoRequestFreeList, (SGL_LIST_NODE*)pNode);
            }

            icmpEchoTmo = (TCPIP_ICMP_ECHO_REQUEST_TIMEOUT * SYS_TMR_TickCounterFrequencyGet() + 999) / 1000;
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
    // postpone packet allocation until the request to send is made
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
    if(signalHandle)
    {
        _TCPIPStackSignalHandlerDeregister(signalHandle);
        signalHandle = 0;
    }
    icmpMemH = 0;
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

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
static void _ICMPTxPktAcknowledge(TCPIP_MAC_PACKET* pTxPkt, const void* ackParam)
{
    TCPIP_PKT_PacketFree(pTxPkt);
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

    if(pEchoRequest == 0)
    {
        return ICMP_ECHO_PARAMETER_ERROR;
    }

    // get an echo request from pool
    OSAL_CRITSECT_DATA_TYPE lock = _ICMPRequestListLock();
    ICMP_ECHO_REQUEST_NODE* pReqNode = (ICMP_ECHO_REQUEST_NODE*)TCPIP_Helper_SingleListHeadRemove(&echoRequestFreeList);
    _ICMPRequestListUnlock(lock);
    
    if(pReqNode == 0)
    {   // too many going on
        return ICMP_ECHO_BUSY;
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

        pReqNode->request = *pEchoRequest;
        pTxPkt->macPkt.pDSeg->segLen += pktSize;
        TCPIP_IPV4_PacketFormatTx(pTxPkt, IP_PROT_ICMP, pktSize, 0);
        TCPIP_PKT_FlightLogTx(&pTxPkt->macPkt, TCPIP_THIS_MODULE_ID);
        if(!TCPIP_IPV4_PacketTransmit(pTxPkt))
        {
            TCPIP_PKT_FlightLogAcknowledge(&pTxPkt->macPkt, TCPIP_THIS_MODULE_ID, TCPIP_MAC_PKT_ACK_IP_REJECT_ERR);
            res = ICMP_ECHO_TRANSMIT_ERROR;
            break;
        }

        pReqNode->reqStart = SYS_TMR_TickCountGet();

        // success; mark as busy
        lock = _ICMPRequestListLock();
        TCPIP_Helper_SingleListTailAdd(&echoRequestBusyList, (SGL_LIST_NODE*)pReqNode);
        _ICMPRequestListUnlock(lock);
    
        if(pHandle)
        {
            *pHandle = pReqNode;
        }

        return ICMP_ECHO_OK;
    }

    // some error
    lock = _ICMPRequestListLock();
    TCPIP_Helper_SingleListTailAdd(&echoRequestFreeList, (SGL_LIST_NODE*)pReqNode);
    _ICMPRequestListUnlock(lock);

    if(pTxPkt != 0)
    {
        TCPIP_PKT_PacketFree(&pTxPkt->macPkt);
    }

    return res;
}

// cancel a echo request
ICMP_ECHO_RESULT TCPIP_ICMP_EchoRequestCancel (TCPIP_ICMP_REQUEST_HANDLE icmpHandle)
{
    if(icmpHandle != 0)
    {
        // traverse the busy list
        OSAL_CRITSECT_DATA_TYPE lock = _ICMPRequestListLock();

        ICMP_ECHO_REQUEST_NODE* pNode, *prev, *pFound;
        for(pFound = 0, prev = 0, pNode = (ICMP_ECHO_REQUEST_NODE*)echoRequestBusyList.head; pNode != 0; prev = pNode, pNode = pNode->next)
        {
            if(pNode == (ICMP_ECHO_REQUEST_NODE*)icmpHandle)
            {   // found it
                pFound = pNode;
                break;
            }
        }

        if(pFound)
        {
            TCPIP_Helper_SingleListNextRemove(&echoRequestBusyList, (SGL_LIST_NODE*) prev);
            TCPIP_Helper_SingleListTailAdd(&echoRequestFreeList, (SGL_LIST_NODE*)pFound);
        }

        _ICMPRequestListUnlock(lock);

        return pFound? ICMP_ECHO_OK : ICMP_ECHO_BAD_HANDLE;
    }

    return ICMP_ECHO_BAD_HANDLE;
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
#if defined(TCPIP_STACK_USE_ICMP_CLIENT)
    TCPIP_ICMP_ECHO_REQUEST echoResult;
#endif  // defined(TCPIP_STACK_USE_ICMP_CLIENT)



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
#if (_TCPIP_IPV4_FRAGMENTATION != 0)
            TCPIP_MAC_PACKET* pFragPkt;
            checksum = 0;
            for(pFragPkt = pRxPkt; pFragPkt != 0; pFragPkt = pFragPkt->pkt_next)
            {
                checksum = ~TCPIP_Helper_PacketChecksum(pFragPkt, pFragPkt->pTransportLayer, pFragPkt->totTransportLen, checksum);
            }
            checksum = ~checksum;
#else
            checksum = TCPIP_Helper_PacketChecksum(pRxPkt, (uint8_t*)pRxHdr, icmpTotLength, 0);
#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)

            if(checksum != 0)
            {
                ackRes = TCPIP_MAC_PKT_ACK_CHKSUM_ERR;
                break;
            }

#if defined(TCPIP_STACK_USE_ICMP_SERVER)
            if(pRxHdr->vType == ICMP_TYPE_ECHO_REQUEST && pRxHdr->vCode == ICMP_CODE_ECHO_REQUEST)
            {   // echo request
#if (TCPIP_ICMP_ECHO_ALLOW_BROADCASTS == 0)
                if(TCPIP_STACK_IsBcastAddress((TCPIP_NET_IF*)pRxPkt->pktIf, &pIpv4Header->DestAddress))
                {
                    ackRes = TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR;  // ignore request
                    break;  
                }
#endif  // (TCPIP_ICMP_ECHO_ALLOW_BROADCASTS == 0)

                _ICMPProcessEchoRequest((TCPIP_NET_IF*)pRxPkt->pktIf, pRxPkt, pIpv4Header->DestAddress.Val, srcAdd);
                ackRes = TCPIP_MAC_PKT_ACK_NONE;
                break;
            }
#endif // defined(TCPIP_STACK_USE_ICMP_SERVER)

#if defined(TCPIP_STACK_USE_ICMP_CLIENT)
            if(pRxHdr->vType == ICMP_TYPE_ECHO_REPLY && pRxHdr->vCode == ICMP_CODE_ECHO_REPLY)
            {   // echo reply; check if our own

                OSAL_CRITSECT_DATA_TYPE lock = _ICMPRequestListLock();

                ICMP_ECHO_REQUEST_NODE* pNode, *prev, *pFound;
                for(pFound = 0, prev = 0, pNode = (ICMP_ECHO_REQUEST_NODE*)echoRequestBusyList.head; pNode != 0; prev = pNode, pNode = pNode->next)
                {   // Get the sequence number and identifier fields
                    if(pNode->request.identifier == pRxHdr->wIdentifier && pNode->request.sequenceNumber == pRxHdr->wSequenceNumber)
                    {   // our reply
                        pFound = pNode;
                        break;
                    }
                }

                if(pFound)
                {
                    echoResult = pFound->request;

                    TCPIP_Helper_SingleListNextRemove(&echoRequestBusyList, (SGL_LIST_NODE*) prev);
                    TCPIP_Helper_SingleListTailAdd(&echoRequestFreeList, (SGL_LIST_NODE*)pFound);
                }

                _ICMPRequestListUnlock(lock);

                if(pFound)
                {
                    echoResult.pData = (uint8_t*)pRxHdr->wData;
                    echoResult.dataSize = icmpTotLength - sizeof(*pRxHdr);
                    echoResult.targetAddr.Val = srcAdd; // update the target
                    if(echoResult.callback != 0)
                    {
                        (echoResult.callback)(&echoResult, pFound, TCPIP_ICMP_ECHO_REQUEST_RES_OK, echoResult.param);
                    }
                    ackRes = TCPIP_MAC_PKT_ACK_RX_OK;
                    break;
                }
            }
            // else not ours; ignore
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

    ICMP_ECHO_REQUEST_NODE* pNode, *prev;
    SINGLE_LIST expiredList;

    prev = 0;
    uint32_t currTick = SYS_TMR_TickCountGet();
    TCPIP_Helper_SingleListInitialize(&expiredList);

    OSAL_CRITSECT_DATA_TYPE lock = _ICMPRequestListLock();
    for(pNode = (ICMP_ECHO_REQUEST_NODE*)echoRequestBusyList.head; pNode != 0; prev = pNode, pNode = pNode->next)
    {
        if((currTick - pNode->reqStart) >= icmpEchoTmo) 
        {   // expired: mark it as invalid
            TCPIP_Helper_SingleListNextRemove(&echoRequestBusyList, (SGL_LIST_NODE*) prev);
            TCPIP_Helper_SingleListTailAdd(&expiredList, (SGL_LIST_NODE*)pNode);
        }
    }

    _ICMPRequestListUnlock(lock);

    // traverse the expired list
    while((pNode = (ICMP_ECHO_REQUEST_NODE*)TCPIP_Helper_SingleListHeadRemove(&expiredList)) != 0)
    {
        if(pNode->request.callback != 0)
        {
            (pNode->request.callback)(&pNode->request, pNode, TCPIP_ICMP_ECHO_REQUEST_RES_TMO, pNode->request.param);
        }
        TCPIP_Helper_SingleListTailAdd(&echoRequestFreeList, (SGL_LIST_NODE*)pNode);
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
    IPV4_PACKET ipv4Pkt;
    IPV4_HEADER* pIpv4Hdr;

    // adjust the checksum
    pTxHdr = (ICMP_PACKET*)pRxPkt->pTransportLayer;

    pTxHdr->vType = ICMP_TYPE_ECHO_REPLY;
    pTxHdr->vCode = ICMP_CODE_ECHO_REPLY;
    checksum.Val = pTxHdr->wChecksum;
    checksum.v[0] += 8; // Subtract 0x0800 from the checksum
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

#if (_TCPIP_IPV4_FRAGMENTATION != 0)
    TCPIP_MAC_PACKET* pFragPkt;
    for(pFragPkt = pRxPkt; pFragPkt != 0; pFragPkt = pFragPkt->pkt_next)
    {
        TCPIP_PKT_FlightLogTx(pFragPkt, TCPIP_THIS_MODULE_ID);
        TCPIP_IPV4_MacPacketSwitchTxToRx(pFragPkt, true, pFragPkt != pRxPkt); 
        pFragPkt->next = 0; // single packet
    }

    // set proper address fields
    pIpv4Hdr = (IPV4_HEADER*)pRxPkt->pNetLayer;
    ipv4Pkt.srcAddress.Val = pIpv4Hdr->SourceAddress.Val;
    ipv4Pkt.destAddress.Val = pIpv4Hdr->DestAddress.Val;
    ipv4Pkt.netIfH = pNetIf;
    
    if(!TCPIP_IPV4_PktTx(&ipv4Pkt, pRxPkt, false))
    {   // failed; discard the segments (these are RX MAC buffers)
        for(pFragPkt = pRxPkt; pFragPkt != 0; pFragPkt = pFragPkt->pkt_next)
        {
            TCPIP_PKT_PacketAcknowledge(pFragPkt, TCPIP_MAC_PKT_ACK_MAC_REJECT_ERR);
        }
        return false;
    }
#else
    TCPIP_IPV4_MacPacketSwitchTxToRx(pRxPkt, true, false); 

    // set proper address fields
    pIpv4Hdr = (IPV4_HEADER*)pRxPkt->pNetLayer;
    ipv4Pkt.srcAddress.Val = pIpv4Hdr->SourceAddress.Val;
    ipv4Pkt.destAddress.Val = pIpv4Hdr->DestAddress.Val;
    ipv4Pkt.netIfH = pNetIf;
    
    TCPIP_PKT_FlightLogTx(pRxPkt, TCPIP_THIS_MODULE_ID);
    if(!TCPIP_IPV4_PktTx(&ipv4Pkt, pRxPkt, false))
    {
        TCPIP_PKT_PacketAcknowledge(pRxPkt, TCPIP_MAC_PKT_ACK_MAC_REJECT_ERR);
        return false;
    }
#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)

    // went through
    return true;
}
#endif // defined(TCPIP_STACK_USE_ICMP_SERVER)


#if (_TCPIP_IPV4_FRAGMENTATION != 0)

static void _ICMPRxPktAcknowledge(TCPIP_MAC_PACKET* pRxPkt, TCPIP_MAC_PKT_ACK_RES ackRes)
{
    TCPIP_MAC_PACKET *pFragPkt, *pFragNext;

    for(pFragPkt = pRxPkt; pFragPkt != 0; pFragPkt = pFragNext)
    {
        pFragNext = pFragPkt->pkt_next;
        TCPIP_PKT_PacketAcknowledge(pFragPkt, ackRes); 
    }
}

#endif  // (_TCPIP_IPV4_FRAGMENTATION != 0)

#endif //#if defined(TCPIP_STACK_USE_ICMP_SERVER) || defined(TCPIP_STACK_USE_ICMP_CLIENT)
#endif  // defined(TCPIP_STACK_USE_IPV4)


