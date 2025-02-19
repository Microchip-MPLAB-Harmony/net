/*******************************************************************************
  Internet Control Message Protocol (ICMP) Server

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    - Provides "ping" diagnostics
    - Reference: RFC 792
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
    // uint8_t wData[];      // payload
} ICMP_PACKET_BARE;

typedef struct
{
    uint8_t vType;
    uint8_t vCode;
    uint16_t wChecksum;
    uint16_t wIdentifier;
    uint16_t wSequenceNumber;
    uint8_t wData[1];      // payload
} ICMP_PACKET;

#define ICMP_TYPE_ECHO_REQUEST      8U   // ICMP server is requested echo - type
#define ICMP_CODE_ECHO_REQUEST      0U   // ICMP server is requested echo - code


#define ICMP_TYPE_ECHO_REPLY        0U   // ICMP client echo reply - type
#define ICMP_CODE_ECHO_REPLY        0U   // ICMP client echo reply - code



static int                  icmpInitCount = 0;  // ICMP module initialization count

static const void*          icmpMemH = NULL;       // memory handle

static TCPIP_SIGNAL_HANDLE    signalHandle = NULL;   // registered signal handler   

#if defined(TCPIP_STACK_USE_ICMP_CLIENT)

typedef struct S_tag_ICMP_ECHO_REQUEST_NODE
{
    struct S_tag_ICMP_ECHO_REQUEST_NODE* next;       // safe cast to SGL_LIST_NODE
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
static bool F_ICMPProcessEchoRequest(const TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pRxPkt, uint32_t destAdd, uint32_t srcAdd);
#endif // defined(TCPIP_STACK_USE_ICMP_SERVER)

#if defined(TCPIP_STACK_USE_ICMP_CLIENT)
static IPV4_PACKET * F_ICMPAllocateTxPacketStruct (uint16_t totICMPLen);
static void F_ICMPTxPktAcknowledge(TCPIP_MAC_PACKET* pTxPkt, const void* ackParam);
static void  TCPIP_ICMP_Timeout(void);

static __inline__ OSAL_CRITSECT_DATA_TYPE __attribute__((always_inline)) F_ICMPRequestListLock(void)
{
    return OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
}

static __inline__ void __attribute__((always_inline)) F_ICMPRequestListUnlock(OSAL_CRITSECT_DATA_TYPE lock)
{
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, lock);   
}
#endif  // defined(TCPIP_STACK_USE_ICMP_CLIENT)

#if defined(TCPIP_IPV4_FRAGMENTATION) && (TCPIP_IPV4_FRAGMENTATION != 0)
#define M_TCPIP_IPV4_FRAGMENTATION 1
#else
#define M_TCPIP_IPV4_FRAGMENTATION 0
#endif


#if (M_TCPIP_IPV4_FRAGMENTATION != 0)
static void     F_ICMPRxPktAcknowledge(TCPIP_MAC_PACKET* pRxPkt, TCPIP_MAC_PKT_ACK_RES ackRes);
#else
static __inline__ void __attribute__((always_inline)) F_ICMPRxPktAcknowledge(TCPIP_MAC_PACKET* pRxPkt, TCPIP_MAC_PKT_ACK_RES ackRes)
{
    TCPIP_PKT_PacketAcknowledge(pRxPkt, ackRes);
}
#endif  // (M_TCPIP_IPV4_FRAGMENTATION != 0)


// conversion functions/helpers
//
#if defined(TCPIP_STACK_USE_ICMP_CLIENT)
static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_ReqNode2SglNode(ICMP_ECHO_REQUEST_NODE* pReqNode)
{
    union
    {
        ICMP_ECHO_REQUEST_NODE* pReqNode;
        SGL_LIST_NODE*  node;
    }U_REQ_NODE_SGL_NODE;

    U_REQ_NODE_SGL_NODE.pReqNode = pReqNode;
    return U_REQ_NODE_SGL_NODE.node;
}

static __inline__ ICMP_ECHO_REQUEST_NODE* __attribute__((always_inline)) FC_SglNode2ReqNode(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        ICMP_ECHO_REQUEST_NODE* pReqNode;
    }U_SGL_NODE_REQ_NODE;

    U_SGL_NODE_REQ_NODE.node = node;
    return U_SGL_NODE_REQ_NODE.pReqNode;
}

static __inline__ ICMP_ECHO_REQUEST_NODE* __attribute__((always_inline)) FC_IcmpH2ReqNode(TCPIP_ICMP_REQUEST_HANDLE icmpHandle)
{
    union
    {
        TCPIP_ICMP_REQUEST_HANDLE icmpHandle;
        ICMP_ECHO_REQUEST_NODE* pReqNode;
    }U_ICMP_H_REQ_NODE;

    U_ICMP_H_REQ_NODE.icmpHandle = icmpHandle;
    return U_ICMP_H_REQ_NODE.pReqNode;
}
#endif

static __inline__ IPV4_PACKET* __attribute__((always_inline)) FC_MacPkt2Ip4Pkt(TCPIP_MAC_PACKET* pMacPkt)
{
    union
    {
        TCPIP_MAC_PACKET* pMacPkt;
        IPV4_PACKET*  pIpPkt;
    }U_MAC_PKT_IP4_PKT;

    U_MAC_PKT_IP4_PKT.pMacPkt = pMacPkt;
    return U_MAC_PKT_IP4_PKT.pIpPkt;
}

static __inline__ ICMP_PACKET* __attribute__((always_inline)) FC_U8Ptr2IcmpPkt(uint8_t* pBuff)
{
    union
    {
        uint8_t*      pBuff;
        ICMP_PACKET*  pIcmpPkt;
    }U_PTR8_ICMP_PKT;

    U_PTR8_ICMP_PKT.pBuff = pBuff;
    return U_PTR8_ICMP_PKT.pIcmpPkt;
}

static __inline__ IPV4_HEADER* __attribute__((always_inline)) FC_U8Ptr2Ip4Hdr(uint8_t* pBuff)
{
    union
    {
        uint8_t*      pBuff;
        IPV4_HEADER*  pIp4Hdr;
    }U_PTR8_IP4_HDR;

    U_PTR8_IP4_HDR.pBuff = pBuff;
    return U_PTR8_IP4_HDR.pIp4Hdr;
}

///////////////////////////////////////
//

bool TCPIP_ICMP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* initData)
{
    if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
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
            iniRes = (signalHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_ICMP_Task, TCPIP_ICMP_TASK_TICK_RATE)) != NULL;
#else
            iniRes = (signalHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_ICMP_Task, 0)) != NULL;
#endif  // defined(TCPIP_STACK_USE_ICMP_CLIENT)
            if(iniRes == false)
            {
                break;
            }
#if defined(TCPIP_STACK_USE_ICMP_CLIENT)
            TCPIP_Helper_SingleListInitialize(&echoRequestFreeList);
            TCPIP_Helper_SingleListInitialize(&echoRequestBusyList);
            size_t ix;
            ICMP_ECHO_REQUEST_NODE* pNode = echoRequestPool;
            for(ix = 0; ix < sizeof(echoRequestPool) / sizeof(*echoRequestPool); ix++)
            {
                TCPIP_Helper_SingleListTailAdd(&echoRequestFreeList, FC_ReqNode2SglNode(pNode));
                pNode++;
            }

            icmpEchoTmo = ((uint32_t)TCPIP_ICMP_ECHO_REQUEST_TIMEOUT * SYS_TMR_TickCounterFrequencyGet() + 999U) / 1000U;
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

        if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
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
    if(signalHandle != NULL)
    {
        TCPIPStackSignalHandlerDeregister(signalHandle);
        signalHandle = NULL;
    }
    icmpMemH = NULL;
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

#if defined (TCPIP_STACK_USE_ICMP_CLIENT)

static IPV4_PACKET * F_ICMPAllocateTxPacketStruct (uint16_t totICMPLen)
{
    IPV4_PACKET * ptrPacket;

    TCPIP_MAC_PACKET* pMacPkt = TCPIP_PKT_SocketAlloc((uint16_t)sizeof(IPV4_PACKET), totICMPLen, 0U, (TCPIP_MAC_PACKET_FLAGS)(TCPIP_MAC_PKT_FLAG_ICMPV4 | TCPIP_MAC_PKT_FLAG_IPV4 | (uint32_t)TCPIP_MAC_PKT_FLAG_TX));
    ptrPacket = FC_MacPkt2Ip4Pkt(pMacPkt);

    if (ptrPacket != NULL)
    {
        TCPIP_PKT_PacketAcknowledgeSet(&ptrPacket->macPkt, &F_ICMPTxPktAcknowledge, NULL);
    }

    return ptrPacket;
}

// packet deallocation function
// packet was transmitted by the IP layer
static void F_ICMPTxPktAcknowledge(TCPIP_MAC_PACKET* pTxPkt, const void* ackParam)
{
    TCPIP_PKT_PacketFree(pTxPkt);
}

ICMP_ECHO_RESULT TCPIP_ICMP_EchoRequest (TCPIP_ICMP_ECHO_REQUEST* pEchoRequest, TCPIP_ICMP_REQUEST_HANDLE* pHandle)
{
    IPV4_PACKET*    pTxPkt;
    ICMP_PACKET*    pICMPPkt;
    uint16_t        pktSize;
    ICMP_ECHO_RESULT res;

    if(pHandle != NULL)
    {
        *pHandle = NULL;
    }

    if(pEchoRequest == NULL)
    {
        return ICMP_ECHO_PARAMETER_ERROR;
    }

    // get an echo request from pool
    OSAL_CRITSECT_DATA_TYPE lock = F_ICMPRequestListLock();
    ICMP_ECHO_REQUEST_NODE* pReqNode = FC_SglNode2ReqNode(TCPIP_Helper_SingleListHeadRemove(&echoRequestFreeList));
    F_ICMPRequestListUnlock(lock);
    
    if(pReqNode == NULL)
    {   // too many going on
        return ICMP_ECHO_BUSY;
    }

    while(true)
    {
        // allocate TX packet
        pktSize = (uint16_t)sizeof(ICMP_PACKET_BARE) + pEchoRequest->dataSize;
        pTxPkt = F_ICMPAllocateTxPacketStruct(pktSize);
        if(pTxPkt == NULL)
        {
            res = ICMP_ECHO_ALLOC_ERROR;
            break;
        }

        pICMPPkt = FC_U8Ptr2IcmpPkt(pTxPkt->macPkt.pTransportLayer);

        pICMPPkt->vType = ICMP_TYPE_ECHO_REQUEST; 
        pICMPPkt->vCode = ICMP_CODE_ECHO_REQUEST;
        pICMPPkt->wChecksum = 0x0000;
        pICMPPkt->wIdentifier = pEchoRequest->identifier;
        pICMPPkt->wSequenceNumber = pEchoRequest->sequenceNumber;
        
        (void)memcpy(pICMPPkt->wData, pEchoRequest->pData, pEchoRequest->dataSize);
        pICMPPkt->wChecksum = TCPIP_Helper_CalcIPChecksum((uint8_t*)pICMPPkt, pktSize, 0);
        pTxPkt->destAddress.Val = pEchoRequest->targetAddr.Val;

        pTxPkt->netIfH = TCPIP_IPV4_SelectSourceInterface(pEchoRequest->netH, &pTxPkt->destAddress, &pTxPkt->srcAddress, false);
        if(pTxPkt->netIfH == NULL)
        {   // could not find an route
            res = ICMP_ECHO_ROUTE_ERROR;
            break;
        }

        pReqNode->request = *pEchoRequest;
        pTxPkt->macPkt.pDSeg->segLen += pktSize;
        TCPIP_IPV4_PacketFormatTx(pTxPkt, (uint8_t)IP_PROT_ICMP, pktSize, NULL);
        TCPIP_PKT_FlightLogTx(&pTxPkt->macPkt, TCPIP_THIS_MODULE_ID);
        if(!TCPIP_IPV4_PacketTransmit(pTxPkt))
        {
            TCPIP_PKT_FlightLogAcknowledge(&pTxPkt->macPkt, TCPIP_THIS_MODULE_ID, TCPIP_MAC_PKT_ACK_IP_REJECT_ERR);
            res = ICMP_ECHO_TRANSMIT_ERROR;
            break;
        }

        pReqNode->reqStart = SYS_TMR_TickCountGet();

        // success; mark as busy
        lock = F_ICMPRequestListLock();
        TCPIP_Helper_SingleListTailAdd(&echoRequestBusyList, FC_ReqNode2SglNode(pReqNode));
        F_ICMPRequestListUnlock(lock);
    
        if(pHandle != NULL)
        {
            *pHandle = pReqNode;
        }

        return ICMP_ECHO_OK;
    }

    // some error
    lock = F_ICMPRequestListLock();
    TCPIP_Helper_SingleListTailAdd(&echoRequestFreeList, FC_ReqNode2SglNode(pReqNode));
    F_ICMPRequestListUnlock(lock);

    if(pTxPkt != NULL)
    {
        TCPIP_PKT_PacketFree(&pTxPkt->macPkt);
    }

    return res;
}

// cancel a echo request
ICMP_ECHO_RESULT TCPIP_ICMP_EchoRequestCancel (TCPIP_ICMP_REQUEST_HANDLE icmpHandle)
{
    if(icmpHandle != NULL)
    {
        // traverse the busy list
        OSAL_CRITSECT_DATA_TYPE lock = F_ICMPRequestListLock();

        ICMP_ECHO_REQUEST_NODE* pNode, *prev, *pFound;
        pFound = prev = NULL;
        pNode = FC_SglNode2ReqNode(echoRequestBusyList.head);
        while(pNode != NULL)
        {
            if(pNode == FC_IcmpH2ReqNode(icmpHandle))
            {   // found it
                pFound = pNode;
                break;
            }
            prev = pNode;
            pNode = pNode->next;
        }

        if(pFound != NULL)
        {
            (void)TCPIP_Helper_SingleListNextRemove(&echoRequestBusyList, FC_ReqNode2SglNode(prev));
            TCPIP_Helper_SingleListTailAdd(&echoRequestFreeList, FC_ReqNode2SglNode(pFound));
        }

        F_ICMPRequestListUnlock(lock);

        return pFound != NULL? ICMP_ECHO_OK : ICMP_ECHO_BAD_HANDLE;
    }

    return ICMP_ECHO_BAD_HANDLE;
}


#else
ICMP_ECHO_RESULT TCPIP_ICMP_EchoRequest (TCPIP_ICMP_ECHO_REQUEST* pEchoRequest, TCPIP_ICMP_REQUEST_HANDLE* pHandle)
{
    return ICMP_ECHO_BAD_HANDLE;
}
ICMP_ECHO_RESULT TCPIP_ICMP_EchoRequestCancel (TCPIP_ICMP_REQUEST_HANDLE icmpHandle)
{
    return ICMP_ECHO_BAD_HANDLE;
}
#endif

void  TCPIP_ICMP_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(((uint16_t)sigPend & (uint16_t)TCPIP_MODULE_SIGNAL_RX_PENDING) != 0U)
    { //  RX signal occurred
        TCPIP_ICMP_Process();
    }

#if defined(TCPIP_STACK_USE_ICMP_CLIENT)
    if(((uint16_t)sigPend & (uint16_t)TCPIP_MODULE_SIGNAL_TMO) != 0U)
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
    while((pRxPkt = TCPIPStackModuleRxExtract(TCPIP_THIS_MODULE_ID)) != NULL)
    {
        TCPIP_PKT_FlightLogRx(pRxPkt, TCPIP_THIS_MODULE_ID);
        pRxHdr = FC_U8Ptr2IcmpPkt(pRxPkt->pTransportLayer);

        pIpv4Header = FC_U8Ptr2Ip4Hdr(pRxPkt->pNetLayer);
        srcAdd =  pIpv4Header->SourceAddress.Val;


        while(true)
        {
            icmpTotLength = pRxPkt->totTransportLen;    // length of the 1st segment (if fragmented)

            if(icmpTotLength < sizeof(ICMP_PACKET_BARE))
            {
                ackRes = TCPIP_MAC_PKT_ACK_STRUCT_ERR;
                break;
            }

            // Validate the checksum
            // The checksum data includes the precomputed checksum in the header
            // so a valid packet will always have a checksum of 0x0000
            // do it across all fragment segments
#if (M_TCPIP_IPV4_FRAGMENTATION != 0)
            TCPIP_MAC_PACKET* pFragPkt;
            checksum = 0;
            for(pFragPkt = pRxPkt; pFragPkt != NULL; pFragPkt = pFragPkt->pkt_next)
            {
                checksum = ~TCPIP_Helper_PacketChecksum(pFragPkt, pFragPkt->pTransportLayer, pFragPkt->totTransportLen, checksum);
            }
            checksum = ~checksum;
#else
            checksum = TCPIP_Helper_PacketChecksum(pRxPkt, (uint8_t*)pRxHdr, icmpTotLength, 0);
#endif  // (M_TCPIP_IPV4_FRAGMENTATION != 0)

            if(checksum != 0U)
            {
                ackRes = TCPIP_MAC_PKT_ACK_CHKSUM_ERR;
                break;
            }

#if defined(TCPIP_STACK_USE_ICMP_SERVER)
            if(pRxHdr->vType == ICMP_TYPE_ECHO_REQUEST && pRxHdr->vCode == ICMP_CODE_ECHO_REQUEST)
            {   // echo request
#if (TCPIP_ICMP_ECHO_ALLOW_BROADCASTS == 0)
                if(TCPIP_STACK_IsBcastAddress((const TCPIP_NET_IF*)pRxPkt->pktIf, &pIpv4Header->DestAddress))
                {
                    ackRes = TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR;  // ignore request
                    break;  
                }
#endif  // (TCPIP_ICMP_ECHO_ALLOW_BROADCASTS == 0)

                (void)F_ICMPProcessEchoRequest((const TCPIP_NET_IF*)pRxPkt->pktIf, pRxPkt, pIpv4Header->DestAddress.Val, srcAdd);
                ackRes = TCPIP_MAC_PKT_ACK_NONE;
                break;
            }
#endif // defined(TCPIP_STACK_USE_ICMP_SERVER)

#if defined(TCPIP_STACK_USE_ICMP_CLIENT)
            if(pRxHdr->vType == ICMP_TYPE_ECHO_REPLY && pRxHdr->vCode == ICMP_CODE_ECHO_REPLY)
            {   // echo reply; check if our own

                OSAL_CRITSECT_DATA_TYPE lock = F_ICMPRequestListLock();

                ICMP_ECHO_REQUEST_NODE* pNode, *prev, *pFound;
                pFound = prev = NULL;
                pNode = FC_SglNode2ReqNode(echoRequestBusyList.head);
                while(pNode != NULL)
                {   // Get the sequence number and identifier fields
                    if(pNode->request.identifier == pRxHdr->wIdentifier && pNode->request.sequenceNumber == pRxHdr->wSequenceNumber)
                    {   // our reply
                        pFound = pNode;
                        break;
                    }
                    prev = pNode;
                    pNode = pNode->next;
                }

                if(pFound != NULL)
                {
                    echoResult = pFound->request;

                    (void)TCPIP_Helper_SingleListNextRemove(&echoRequestBusyList, FC_ReqNode2SglNode(prev));
                    TCPIP_Helper_SingleListTailAdd(&echoRequestFreeList, FC_ReqNode2SglNode(pFound));
                }

                F_ICMPRequestListUnlock(lock);

                if(pFound != NULL)
                {
                    echoResult.pData = pRxHdr->wData;
                    echoResult.dataSize = icmpTotLength - (uint16_t)sizeof(ICMP_PACKET_BARE);
                    echoResult.targetAddr.Val = srcAdd; // update the target
                    if(echoResult.callback != NULL)
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
            F_ICMPRxPktAcknowledge(pRxPkt, ackRes); 
        }
    }
}

// process timeouts: ping requests sent, for which there was no answer
#if defined(TCPIP_STACK_USE_ICMP_CLIENT)
static void  TCPIP_ICMP_Timeout(void)
{

    ICMP_ECHO_REQUEST_NODE* pNode, *prev;
    SINGLE_LIST expiredList;

    prev = NULL;
    uint32_t currTick = SYS_TMR_TickCountGet();
    TCPIP_Helper_SingleListInitialize(&expiredList);

    OSAL_CRITSECT_DATA_TYPE lock = F_ICMPRequestListLock();
    pNode = FC_SglNode2ReqNode(echoRequestBusyList.head);
    while(pNode != NULL)
    {
        if((currTick - pNode->reqStart) >= icmpEchoTmo) 
        {   // expired: mark it as invalid
            (void)TCPIP_Helper_SingleListNextRemove(&echoRequestBusyList, FC_ReqNode2SglNode(prev));
            TCPIP_Helper_SingleListTailAdd(&expiredList, FC_ReqNode2SglNode(pNode));
        }
        prev = pNode;
        pNode = pNode->next;
    }

    F_ICMPRequestListUnlock(lock);

    // traverse the expired list
    while((pNode = FC_SglNode2ReqNode(TCPIP_Helper_SingleListHeadRemove(&expiredList))) != NULL)
    {
        if(pNode->request.callback != NULL)
        {
            (pNode->request.callback)(&pNode->request, pNode, TCPIP_ICMP_ECHO_REQUEST_RES_TMO, pNode->request.param);
        }
        TCPIP_Helper_SingleListTailAdd(&echoRequestFreeList, FC_ReqNode2SglNode(pNode));
    }
}
#endif  // defined(TCPIP_STACK_USE_ICMP_CLIENT)


#if defined(TCPIP_STACK_USE_ICMP_SERVER)
// echo request
// use the same packet for reply
// it will be acknowledged by the MAC after transmission
static bool F_ICMPProcessEchoRequest(const TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pRxPkt, uint32_t destAdd, uint32_t srcAdd)
{
    ICMP_PACKET* pTxHdr;
    TCPIP_UINT16_VAL checksum;
    IPV4_PACKET ipv4Pkt;
    IPV4_HEADER* pIpv4Hdr;

    // adjust the checksum
    pTxHdr = FC_U8Ptr2IcmpPkt(pRxPkt->pTransportLayer);

    pTxHdr->vType = ICMP_TYPE_ECHO_REPLY;
    pTxHdr->vCode = ICMP_CODE_ECHO_REPLY;
    checksum.Val = pTxHdr->wChecksum;
    checksum.v[0] += 8U; // Subtract 0x0800 from the checksum
    if(checksum.v[0] < 8u)
    {
        checksum.v[1]++;
        if(checksum.v[1] == 0u)
        {
            checksum.v[0]++;
        }
    }

    pTxHdr->wChecksum = checksum.Val;
    pRxPkt->next = NULL; // single packet

#if (M_TCPIP_IPV4_FRAGMENTATION != 0)
    TCPIP_MAC_PACKET* pFragPkt;
    for(pFragPkt = pRxPkt; pFragPkt != NULL; pFragPkt = pFragPkt->pkt_next)
    {
        TCPIP_PKT_FlightLogTx(pFragPkt, TCPIP_THIS_MODULE_ID);
        TCPIP_IPV4_MacPacketSwitchTxToRx(pFragPkt, true, pFragPkt != pRxPkt); 
        pFragPkt->next = NULL; // single packet
    }

    // set proper address fields
    pIpv4Hdr = FC_U8Ptr2Ip4Hdr(pRxPkt->pNetLayer);
    ipv4Pkt.srcAddress.Val = pIpv4Hdr->SourceAddress.Val;
    ipv4Pkt.destAddress.Val = pIpv4Hdr->DestAddress.Val;
    ipv4Pkt.netIfH = pNetIf;
    
    if(!TCPIP_IPV4_PktTx(&ipv4Pkt, pRxPkt, false))
    {   // failed; discard the segments (these are RX MAC buffers)
        for(pFragPkt = pRxPkt; pFragPkt != NULL; pFragPkt = pFragPkt->pkt_next)
        {
            TCPIP_PKT_PacketAcknowledge(pFragPkt, TCPIP_MAC_PKT_ACK_MAC_REJECT_ERR);
        }
        return false;
    }
#else
    TCPIP_IPV4_MacPacketSwitchTxToRx(pRxPkt, true, false); 

    // set proper address fields
    pIpv4Hdr = FC_U8Ptr2Ip4Hdr(pRxPkt->pNetLayer);
    ipv4Pkt.srcAddress.Val = pIpv4Hdr->SourceAddress.Val;
    ipv4Pkt.destAddress.Val = pIpv4Hdr->DestAddress.Val;
    ipv4Pkt.netIfH = pNetIf;
    
    TCPIP_PKT_FlightLogTx(pRxPkt, TCPIP_THIS_MODULE_ID);
    if(!TCPIP_IPV4_PktTx(&ipv4Pkt, pRxPkt, false))
    {
        TCPIP_PKT_PacketAcknowledge(pRxPkt, TCPIP_MAC_PKT_ACK_MAC_REJECT_ERR);
        return false;
    }
#endif  // (M_TCPIP_IPV4_FRAGMENTATION != 0)

    // went through
    return true;
}
#endif // defined(TCPIP_STACK_USE_ICMP_SERVER)


#if (M_TCPIP_IPV4_FRAGMENTATION != 0)

static void F_ICMPRxPktAcknowledge(TCPIP_MAC_PACKET* pRxPkt, TCPIP_MAC_PKT_ACK_RES ackRes)
{
    TCPIP_MAC_PACKET *pFragPkt, *pFragNext;

    pFragPkt = pRxPkt;
    while(pFragPkt != NULL)
    {
        pFragNext = pFragPkt->pkt_next;
        TCPIP_PKT_PacketAcknowledge(pFragPkt, ackRes); 
        pFragPkt = pFragNext;
    }
}

#endif  // (M_TCPIP_IPV4_FRAGMENTATION != 0)

#endif //#if defined(TCPIP_STACK_USE_ICMP_SERVER) || defined(TCPIP_STACK_USE_ICMP_CLIENT)
#endif  // defined(TCPIP_STACK_USE_IPV4)


