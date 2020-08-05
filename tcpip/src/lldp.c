/*******************************************************************************
  Link Layer Discovery Protocol (LLDP)

  Summary:
    Module for Microchip TCP/IP Stack

  Description:
    - LLDP implementation for Microchip TCP/IP stack
*******************************************************************************/

/*****************************************************************************
 Copyright (C) 2014-2018 Microchip Technology Inc. and its subsidiaries.

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









#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_LLDP

#include "tcpip/src/tcpip_private.h"
#include "lldp_private.h"
#include "lldp_tlv.h"
#include "tcpip_module_manager.h"


#if defined(TCPIP_STACK_USE_LLDP)



static tcpipSignalHandle lldpTimerHandle = 0;
static int              lldpInitCount = 0;

static bool lldpMcastFilterSet = false;

#ifdef _LLDP_DEBUG
uint32_t    lldpTxSizeDealloc = 0;
uint32_t    lldpTxCtorErrors = 0;
uint32_t    lldpAllocErrors = 0;
uint32_t    lldpTxErrors = 0;
uint32_t    lldpTxSuccess = 0;
uint32_t    lldpTxAttempts = 0;
uint32_t    lldpTxLinkDown = 0;

uint32_t lldpRxAttempts = 0;
uint32_t lldpRxSuccess = 0;
uint32_t lldpRxErrors = 0;

#endif

static const TCPIP_MAC_ADDR lldpMcastAddr = { {0X01,0X80,0XC2,0X00,0X00,0X0E} }; // unless the other 2 allowed multicast addresses are defined by the user

static TCPIP_MAC_ADDR       destMacAddr;            // current LLDP multiCast address
static TCPIP_NET_HANDLE     lldpLocalNetIf = 0;     //  local interface of the LLDPDU packet


static lldp_tx_timers_t     lldp_tx_timers;

static lldp_txTimerStates_t txTimerState;
static lldp_txStates_t      txState;

static TCPIP_MAC_ADDR       userDestAddress = { {0} };
static bool                 validUserDestAddress = false;


static TCPIP_MAC_PACKET*   pLldpPkt = 0;     // packet that we use to send LLDPDU
                                             // only ONE packet is used for now even if there are multiple interfaces!!!
static uint16_t             lldpPktSize = 0; // size of the currently allocated packet   

static lldp_rxStates_t      rxState = RX_IDLE;

static lldp_per_port_t      lldp_port;

// local prototypes
#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void TCPIP_LLDP_Cleanup(void);
#else
#define TCPIP_LLDP_Cleanup()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static TCPIP_MAC_PKT_ACK_RES rxProcessFrame(TCPIP_MAC_PACKET* pRxPkt);
static void rxStateMachine(void);
static void rxInitializeLLDP(void);
static void somethingChangedRemote(void);
static void mibDeleteObjects(void);

static void txTimerStateMachine(void);
static void txTimerInitializeLLDP(void);
static void txStateMachine(void);
static bool txFrame(void);
static void txInitializeLLDP(void);

static bool TCPIP_LLDP_SetMulticastFilter(void);

static TCPIP_MAC_PACKET* LldpAllocateTxPacket(uint16_t pktSize);
static bool LldpTxAckFnc (TCPIP_MAC_PACKET * pPkt, const void * param);

static void TCPIP_LLDP_Timeout(void);

static void TCPIP_LLDP_Process(void);


// implementation

bool TCPIP_LLDP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_LLDP_MODULE_CONFIG* pLLDPConfig)
{

    if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        // a LLDP TX message may be needed here! localChange!
        return true;
    }

    // stack init
    while(lldpInitCount == 0)
    {   // first time we run
        // create the LLDP timer
        lldpTimerHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_LLDP_Task, TCPIP_LLDP_TASK_TICK_RATE); 
        if(lldpTimerHandle == 0)
        {   // cannot create the LLDP timer
            TCPIP_LLDP_Cleanup();
            return false;
        }


        break;
    }

    lldp_port.rx.rxInfoAge = false;
    lldp_port.portEnabled = false;
    rxState = LLDP_WAIT_PORT_OPERATIONAL;
    txTimerState = TX_TIMER_INITIALIZE;
    lldp_port.tx.txFast = 0;
    txInitializeLLDP();
    TCPIP_LLDP_TlvInit();
    lldpMcastFilterSet = false;
    pLldpPkt = 0;
    lldpPktSize = 0;

#ifdef _LLDP_DEBUG
        lldpTxSizeDealloc = 0;
        lldpTxCtorErrors = 0;
        lldpAllocErrors = 0;
        lldpTxErrors = 0;
        lldpTxSuccess = 0;
        lldpTxAttempts = 0;
        lldpTxLinkDown = 0;
#endif

    lldpInitCount++;
    return true;

}


#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_LLDP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{

    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN)
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down

    if(lldpInitCount > 0)
    {   // we're up and running
        // interface is going down one way or another

        if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--lldpInitCount == 0)
            {   // all closed
                // release resources
                TCPIP_LLDP_Cleanup();
            }
        }
    }

}

static void TCPIP_LLDP_Cleanup(void)
{
    if(lldpTimerHandle)
    {
        _TCPIPStackSignalHandlerDeregister(lldpTimerHandle);
        lldpTimerHandle = 0;
    }

}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

void TCPIP_LLDP_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if((sigPend & TCPIP_MODULE_SIGNAL_RX_PENDING) != 0)
    { //  RX signal occurred
        TCPIP_LLDP_Process();
    }

    if((sigPend & TCPIP_MODULE_SIGNAL_TMO) != 0)
    { // regular TMO occurred
        TCPIP_LLDP_Timeout();
    }

}




static void TCPIP_LLDP_Timeout(void)
{
    if(lldpMcastFilterSet == false)
    { 
        if(!TCPIP_LLDP_SetMulticastFilter())
        {   // not ready yet
            return;
        }
        // update the default interface
        lldpLocalNetIf = TCPIP_STACK_NetDefaultGet(); 
    }

    lldp_tx_timers.txTick = true;   // setLLDPTick
    if(lldp_tx_timers.txTTR > 0)
    {   // decTTR
       lldp_tx_timers.txTTR--;
    }

    if (lldp_port.portEnabled)
    {
        if (lldp_port.adminStatus == enabledTxOnly || lldp_port.adminStatus == enabledRxTx)
        {
            txTimerStateMachine();
            txStateMachine();
        }

        if (lldp_port.adminStatus == enabledRxOnly || lldp_port.adminStatus == enabledRxTx)
        {
            rxStateMachine();
        }
    }
}


static bool TCPIP_LLDP_SetMulticastFilter(void)
{
    while(lldpMcastFilterSet == false)
    {   
        int netIx;
        TCPIP_NET_IF* pNetIf;
        const TCPIP_MAC_OBJECT*  pMacObj;
        SYS_MODULE_OBJ macObjHandle;
        TCPIP_MAC_HANDLE hIfMac;
        bool macReady = false;

        // Register an RX MAC filter for the LLDP multicast addresses
        for(netIx = 0; netIx < TCPIP_STACK_NumberOfNetworksGet(); netIx++)
        {
            // Check 1st that the MAC is ready
            pNetIf = (TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(netIx);
            pMacObj = _TCPIP_STACK_GetMacObject(pNetIf);
            macObjHandle = _TCPIP_STACK_GetMacObjectHandle(pNetIf);
            if(pMacObj && macObjHandle)
            {
                SYS_STATUS macStat = (*pMacObj->TCPIP_MAC_Status)(macObjHandle);
                if(macStat == SYS_STATUS_READY)
                {   // MAC ready
                    macReady = true;
                    break;
                }
            }
        }

        if(!macReady)
        {   // wait some more
            break;
        }

        // set the MCAST filters
        TCPIP_MAC_ADDR bridge_addr              = { {0x01, 0x80, 0xc2, 0x00, 0x00, 0x0e} };
        TCPIP_MAC_ADDR non_TPMR_bridge_addr     = { {0x01, 0x80, 0xc2, 0x00, 0x00, 0x03} };
        TCPIP_MAC_ADDR customer_bridge_addr     = { {0x01, 0x80, 0xc2, 0x00, 0x00, 0x00} };

        for(netIx = 0; netIx < TCPIP_STACK_NumberOfNetworksGet(); netIx++)
        {
            pNetIf = (TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(netIx);
            pMacObj = _TCPIP_STACK_GetMacObject(pNetIf);
            hIfMac = _TCPIP_STACK_GetMacClientHandle(pNetIf);
            if(pMacObj && hIfMac)
            {
                (*pMacObj->TCPIP_MAC_RxFilterHashTableEntrySet)(hIfMac, &bridge_addr);
                (*pMacObj->TCPIP_MAC_RxFilterHashTableEntrySet)(hIfMac, &non_TPMR_bridge_addr);
                (*pMacObj->TCPIP_MAC_RxFilterHashTableEntrySet)(hIfMac, &customer_bridge_addr);
            }
        }

        lldpMcastFilterSet = true;
        break;
    }

    return lldpMcastFilterSet; 
}


/*********************RECEIVE STATE MACHINE AND PROCESSING********************/

static TCPIP_MAC_PKT_ACK_RES rxProcessFrame(TCPIP_MAC_PACKET* pRxPkt)
{
    TCPIP_LLDP_TLV* pTlv;           // current processed TLV
    uint16_t    tlvLen;             // current length
    uint16_t    frameLength;        // bytes left in frame
    TCPIP_LLDP_TLV_TYPE lastType;   // last processed type
    TCPIP_LLDP_TLV_PROCESS_RES tlvRes;
    

    pTlv = (TCPIP_LLDP_TLV*)pRxPkt->pNetLayer;
    lastType = TCPIP_LLDP_TLV_TYPE_FRAME_START;
    frameLength = TCPIP_PKT_PayloadLen(pRxPkt);

#ifdef _LLDP_DEBUG
        lldpRxAttempts++;
#endif

    tlvRes = TLV_PROCESS_CONTINUE;

    while(tlvRes == TLV_PROCESS_CONTINUE)
    {
        pTlv->type_length = TCPIP_Helper_ntohs(pTlv->type_length);
        tlvLen = pTlv->length;

        if(tlvLen > frameLength)
        {   // TLV structure error
            tlvRes = TLV_PROCESS_ERROR;
        }
        else
        {
            tlvRes = TCPIP_LLDP_TlvProcess(pTlv, lastType);
        }
        // adjust the descriptor for the next round
        lastType = pTlv->type;
        frameLength -= sizeof(*pTlv) + tlvLen;
        pTlv = (TCPIP_LLDP_TLV*)((uint8_t*)(pTlv + 1) + tlvLen);
    }

    if(tlvRes != TLV_PROCESS_END)
    {
#ifdef _LLDP_DEBUG
        lldpRxErrors++;
#endif        
        return TCPIP_MAC_PKT_ACK_STRUCT_ERR;
    }
#ifdef _LLDP_DEBUG
    else
    {
        lldpRxSuccess++;
    }
#endif

    // signal the RX state machine
    lldp_port.rx.rcvFrame = true;
    return TCPIP_MAC_PKT_ACK_RX_OK;
}

static void rxStateMachine(void)
{
    if (lldp_port.portEnabled == false && lldp_port.rx.rxInfoAge == false)
    {
        rxState=LLDP_WAIT_PORT_OPERATIONAL;
    }

    switch(rxState)
    {
        case LLDP_WAIT_PORT_OPERATIONAL:
            if(lldp_port.rx.rxInfoAge)
            {
                rxState = DELETE_AGED_INFO;
            }
            else if (lldp_port.portEnabled)
            {
                rxState = RX_LLDP_INITIALIZE;
            }
            break;

        case RX_LLDP_INITIALIZE:
            rxInitializeLLDP();
            lldp_port.rx.rcvFrame = false;
            if(lldp_port.adminStatus == (lldp_admin_status)enabledRxTx || lldp_port.adminStatus == (lldp_admin_status)enabledRxOnly)
            {
                rxState=RX_WAIT_FOR_FRAME;
            }
            break;

        case DELETE_AGED_INFO:
            mibDeleteObjects();
            lldp_port.rx.rxInfoAge = false;
            somethingChangedRemote();
            rxState = LLDP_WAIT_PORT_OPERATIONAL;
            break;

        case RX_WAIT_FOR_FRAME:
            lldp_port.rx.rxInfoAge = false;
            if(lldp_port.rx.rcvFrame == true)
            {
                bool rxChanges = lldp_port.rxChanges;   
                lldp_port.rx.rcvFrame = false;
                lldp_port.rxChanges = false;
                if(lldp_port.rx.timers.rxTTL == 0)
                {
                    rxState = DELETE_INFO;
                }
                else if(rxChanges)
                {   
                    rxState = UPDATE_INFO;
                }
                // else if(!lldp_port.rxChanges) {rxState = RX_WAIT_FOR_FRAME;}
            }
            else if(lldp_port.rx.rxInfoAge)
            {
                rxState = DELETE_INFO;
            }
            else if(lldp_port.adminStatus == (lldp_admin_status)disabled || lldp_port.adminStatus == (lldp_admin_status)enabledTxOnly)
            {
                rxState = RX_LLDP_INITIALIZE;
            }
            break;

        case UPDATE_INFO:
            mibDeleteObjects();
            somethingChangedRemote();
            rxState = RX_LLDP_INITIALIZE;
            break;

        case DELETE_INFO:
            mibDeleteObjects();
            somethingChangedRemote();
            rxState = RX_WAIT_FOR_FRAME;
            break;


        default:
            break;
    }
}


static void TCPIP_LLDP_Process(void)
{
    TCPIP_NET_IF* pNetIf;
    TCPIP_MAC_PACKET* pRxPkt;
    TCPIP_MAC_PKT_ACK_RES ackRes;
    bool                lldpProcess;

    if(lldp_port.adminStatus == (lldp_admin_status)enabledRxOnly || lldp_port.adminStatus == (lldp_admin_status)enabledRxTx)
    {
        lldpProcess = true;
    }
    else
    {
        lldpProcess = false;
    }

    // extract queued LLDP packets
    while((pRxPkt = _TCPIPStackModuleRxExtract(TCPIP_THIS_MODULE_ID)) != 0)
    {
        TCPIP_PKT_FlightLogRx(pRxPkt, TCPIP_THIS_MODULE_ID);
        pNetIf = (TCPIP_NET_IF*)pRxPkt->pktIf;

        ackRes = TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR;

        if(lldpProcess)
        {
            lldpLocalNetIf = pNetIf;

            ackRes = rxProcessFrame(pRxPkt);
        }

        TCPIP_PKT_PacketAcknowledge(pRxPkt, ackRes);    
    }

}

static void rxInitializeLLDP(void)
{
    /**IEE 802.1AB section 9.2.7.6*/
    lldp_port.rx.tooManyNeighbors = false;
    mibDeleteObjects();
}

static void somethingChangedRemote(void)
{
    
}

static void mibDeleteObjects(void)
{

}


// TX state machine
//

static void txTimerInitializeLLDP(void)
{
    lldp_tx_timers.txTick=false;
    lldp_port.tx.txNow=false;
    lldp_port.tx.localChange=false;
    lldp_tx_timers.txTTR=0;
    lldp_port.tx.txFast=0;
    lldp_tx_timers.txShutdownWhile=0;
    lldp_port.newNeighbor=false;
    lldp_port.tx.txCredit= txCreditMax;
}

//Transmit Timer State Machine

static void txTimerStateMachine(void)
{
    switch(txTimerState)
    {
//        ////printf("I am in the TX TIMER Statemachine\n");
        case TX_TIMER_INITIALIZE:
            txTimerInitializeLLDP();
            if(lldp_port.adminStatus == (lldp_admin_status)enabledRxTx || lldp_port.adminStatus == (lldp_admin_status)enabledTxOnly) \
            {
               txTimerState=TX_TIMER_IDLE;
            }
            break;
            
        case TX_TIMER_IDLE:
            if(lldp_tx_timers.txTick)
            {
                txTimerState=TX_TICK;
            }
            if (lldp_tx_timers.txTTR == 0)
            {
                txTimerState=TX_TIMER_EXPIRES;
            }
            else if(lldp_port.tx.localChange)
            {
                txTimerState=SIGNAL_TX;
            }
            else if(lldp_port.newNeighbor)
            {
                txTimerState=TX_FAST_START;
            }
            break;

        case TX_TIMER_EXPIRES:
            // dec(txFast);
            ////printf("TIME TO SEND ANOTHER PACKET!!!!!!!!!\n");//db
            if(lldp_port.tx.txFast > 0)
            {
                lldp_port.tx.txFast--;
            }
            txTimerState = SIGNAL_TX;
            break;
            
        case TX_TICK:
            lldp_tx_timers.txTick = false;      // clrLLDPTick
            // Implementation of txAddCredit() as per IEEE spec:
            if(lldp_port.tx.txCredit < txCreditMax)
            {
                lldp_port.tx.txCredit++;
            }
            txTimerState = TX_TIMER_IDLE;
            break;

        case SIGNAL_TX:
            lldp_port.tx.txNow = true;
            lldp_port.tx.localChange = false;
            lldp_tx_timers.txTTR = (lldp_port.tx.txFast) ? msgFastTx : msgTxInterval;
            txTimerState = TX_TIMER_IDLE;
            break;

        case TX_FAST_START:
            lldp_port.newNeighbor = false;
            if(lldp_port.tx.txFast == 0)
            {
                lldp_port.tx.txFast=txFastInit;
            }
            break;

    }
}


//Transmit State Machine

static void txStateMachine(void)
{

    switch (txState)
    {
        case TX_LLDP_INITIALIZE:
            if(lldp_port.adminStatus == (lldp_admin_status)enabledRxTx || lldp_port.adminStatus == (lldp_admin_status)enabledTxOnly)
            {
                txState=TX_IDLE;
            }
            break;

        case TX_IDLE:
            lldp_port.tx.txTTL = (65535 < (msgTxHold * msgTxInterval) + 1) ? 65535 : ((msgTxHold * msgTxInterval)+1);
            if(lldp_port.tx.txNow && lldp_port.tx.txCredit > 0)
            {
                txState=TX_INFO_FRAME;
            }
            else if (lldp_port.adminStatus == (lldp_admin_status)disabled || lldp_port.adminStatus == (lldp_admin_status)enabledRxOnly)
            {
                txState=TX_SHUTDOWN_FRAME;
            }
            break;

        case TX_INFO_FRAME:
            if(txFrame())
            {
                lldp_port.tx.txCredit--;
                lldp_port.tx.txNow = false;
                txState=TX_IDLE;
            }
            break;

        case TX_SHUTDOWN_FRAME:
            lldp_tx_timers.txShutdownWhile = reinitDelay;
            if(!lldp_tx_timers.txShutdownWhile)
            {
               txState=TX_LLDP_INITIALIZE;
            }
            break;

    }
}

static void txInitializeLLDP(void)
{
    validUserDestAddress = false;
    memset(userDestAddress.v, 0, sizeof(userDestAddress));
    txState = TX_LLDP_INITIALIZE;

    memcpy(&destMacAddr, &lldpMcastAddr, sizeof(TCPIP_MAC_ADDR));
    lldpLocalNetIf = 0; 
}

static bool txFrame(void)
{
    TCPIP_MAC_PACKET*       pTxPkt;
    size_t                  lldpFrameSize;
    TCPIP_NET_IF*           pLldpIf;
    const TCPIP_MAC_ADDR*   pDestMac;

#ifdef _LLDP_DEBUG
    lldpTxAttempts++;
#endif
   
    // make sure the LLDP interface is valid
    pLldpIf = _TCPIPStackHandleToNetLinked(lldpLocalNetIf);
    if(pLldpIf == 0)
    {   // cannot transmit over dead interface
#ifdef _LLDP_DEBUG
        lldpTxLinkDown++;
#endif
        return false;
    }
    

    lldpFrameSize = TCPIP_LLDP_ConstructMibLDPDU(0, 0);

    if(lldpFrameSize == -1)
    {   // failed to construct the frame
#ifdef _LLDP_DEBUG
        lldpTxCtorErrors++;
#endif
        return false;
    }

    pTxPkt = LldpAllocateTxPacket(lldpFrameSize);
    if(pTxPkt == 0)
    {   // failed to alloc packet
#ifdef _LLDP_DEBUG
        lldpAllocErrors++;
#endif
        return false;
    }

    // dump the data in the packet
    TCPIP_LLDP_ConstructMibLDPDU(pTxPkt->pNetLayer, lldpFrameSize);

    // format the MAC packet
    pTxPkt->pDSeg->segLen = lldpFrameSize;

    pDestMac = TCPIP_LLDP_MacDestAddressGet(); 

    if(TCPIP_PKT_PacketMACFormat(pTxPkt, pDestMac, (const TCPIP_MAC_ADDR*)TCPIP_STACK_NetUpMACAddressGet(pLldpIf), TCPIP_ETHER_TYPE_LLDP))
    {
        pTxPkt->pktIf = pLldpIf;
        TCPIP_PKT_FlightLogTx(pTxPkt, TCPIP_THIS_MODULE_ID);
        if(_TCPIPStackPacketTx(pLldpIf, pTxPkt) >= 0)
        {   
#ifdef _LLDP_DEBUG
            lldpTxSuccess++;
#endif
            return true;
        }
        TCPIP_PKT_FlightLogAcknowledge(pTxPkt, TCPIP_THIS_MODULE_ID, TCPIP_MAC_PKT_ACK_MAC_REJECT_ERR);
    }

#ifdef _LLDP_DEBUG
        lldpTxErrors++;
#endif
    // packet transmission failed
    return false;
}


static TCPIP_MAC_PACKET* LldpAllocateTxPacket(uint16_t pktSize)
{
    TCPIP_MAC_PACKET*   pPkt;

    if(pLldpPkt != 0 && (pLldpPkt->pktFlags & TCPIP_MAC_PKT_FLAG_QUEUED) == 0)
    {   // we already have a packet
        if(lldpPktSize >= pktSize)
        {   // good enough
            return pLldpPkt;
        }
        // need a bigger one
        TCPIP_PKT_PacketFree(pLldpPkt);
        pLldpPkt = 0;
#ifdef _LLDP_DEBUG
        lldpTxSizeDealloc++;
#endif
    }

    // either no packet (available) or not big enough
    // have to allocate another one
    // no packet flag for LLDP; use generic, low level ARP one
    pPkt = TCPIP_PKT_PacketAlloc(sizeof(TCPIP_MAC_PACKET), pktSize, TCPIP_MAC_PKT_FLAG_ARP | TCPIP_MAC_PKT_FLAG_TX);

    if(pPkt)
    {
        TCPIP_PKT_PacketAcknowledgeSet(pPkt, LldpTxAckFnc, 0);
        pLldpPkt = pPkt;
        lldpPktSize = pktSize;
    }

    return pPkt;
}


static bool LldpTxAckFnc (TCPIP_MAC_PACKET * pPkt, const void * param)
{
    if(pLldpPkt != pPkt)
    {   // another one allocated
        TCPIP_PKT_PacketFree(pPkt);
        return false;
    }
    // else we should be OK
    return true;
}




// Public API
// 
void TCPIP_LLDP_RxEnable(void)
{
    lldp_port.adminStatus = (lldp_admin_status)enabledRxOnly;
    lldp_port.portEnabled = true;
}

void TCPIP_LLDP_TxEnable(void)
{
    lldp_port.adminStatus = (lldp_admin_status)enabledTxOnly;
    lldp_port.portEnabled = true;
}

void TCPIP_LLDP_RxTxEnable(void)
{
    lldp_port.adminStatus = (lldp_admin_status)enabledRxTx;
    lldp_port.portEnabled = true;
}

void TCPIP_LLDP_PortDisable(void)
{
    lldp_port.adminStatus = (lldp_admin_status)disabled;
    lldp_port.portEnabled = false;
}

void TCPIP_LLDP_FastTxCounterSet(uint8_t num)
{
    lldp_port.tx.txFast = num;
}

void TCPIP_LLDP_MacDestAddressSet(TCPIP_MAC_ADDR* pAddr)
{
    bool isZeroAdd = false;
    if(pAddr != 0)
    {
        TCPIP_MAC_ADDR zeroAdd = { {0} };
        memcpy(userDestAddress.v, pAddr->v, sizeof(userDestAddress));
        if(memcmp(userDestAddress.v, zeroAdd.v, sizeof(TCPIP_MAC_ADDR)) == 0)
        {
            isZeroAdd = true;
        }
    }

    if(pAddr == 0 || isZeroAdd)
    {
        validUserDestAddress = false;
    }
    else
    {
        validUserDestAddress = true;
    }
} 

// check to see if a MAC address has been provided by the user
// else use the destination MAC from the last packet received
const TCPIP_MAC_ADDR* TCPIP_LLDP_MacDestAddressGet(void)
{
    return validUserDestAddress ? &userDestAddress : &destMacAddr; 
}


const uint8_t* TCPIP_LLDP_LocalIfAddressGet(void)
{
    if(lldpLocalNetIf)
    {
        return TCPIP_STACK_NetAddressMac(lldpLocalNetIf);
    }

    return 0;
}

const lldp_per_port_t*  TCPIP_LLDP_PortGet(void)
{
    return &lldp_port;
}

// set the allocated power
void TCPIP_LLDP_AllocatedPowerSet(uint16_t allocatedPower)
{
    lldp_port.allocatedPower = allocatedPower;
}

void TCPIP_LLDP_DesiredPowerSet(uint16_t desiredPower)
{
    lldp_port.desiredPower = desiredPower;
}

uint16_t TCPIP_LLDP_AllocatedPowerGet(void)
{
    return lldp_port.allocatedPower;
}



bool TCPIP_LLDP_UPoePowerIsEnabled(void)
{
    TCPIP_LLDP_ORG_FLAGS orgFlags;
    
    TCPIP_LLDP_OrgProcessFlagsGet(&orgFlags);
    
    return orgFlags.uPoeEnabledPower != 0;

}

bool TCPIP_LLDP_PoePlusPowerIsEnabled(void)
{
    TCPIP_LLDP_ORG_FLAGS orgFlags;

    TCPIP_LLDP_OrgProcessFlagsGet(&orgFlags);

    return orgFlags.poePlusEnabledPower != 0;
}

bool TCPIP_LLDP_PoeMinPowerIsEnabled(void)
{
    TCPIP_LLDP_ORG_FLAGS orgFlags;

    TCPIP_LLDP_OrgProcessFlagsGet(&orgFlags);

    return orgFlags.poeEnabledMinPower != 0;
}
#endif  //if defined(TCPIP_STACK_USE_LLDP)

