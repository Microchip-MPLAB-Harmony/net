/*******************************************************************************
  MAC Module (Microchip PIC32MX5-7) for Microchip TCP/IP Stack
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


#include "driver/ethmac/src/dynamic/drv_ethmac_lib.h"

/** D E F I N I T I O N S ****************************************************/

#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_MAC_PIC32INT

#define ETHER_IP    (0x00u)
#define ETHER_ARP   (0x06u)


// All pause capabilities our MAC supports
#define DRV_ETHMAC_PAUSE_CPBL_MASK     (TCPIP_ETH_PAUSE_TYPE_ALL)

// MAX frame defines
// round up to multiple of 16
#define M_TCPIP_EMAC_MAX_FRAME (((TCPIP_EMAC_MAX_FRAME + 15) / 16) * 16)

// MTU defines
#define TCPIP_EMAC_LINK_MTU_ETH_MIN    576                         // min: ETH default, shouldn't go below this value
#define TCPIP_EMAC_LINK_MTU_ETH_MAX    1500                        // max: ETH default, shouldn't go above this value

#if (M_TCPIP_EMAC_MAX_FRAME - 18) > TCPIP_EMAC_LINK_MTU_ETH_MAX
#define TCPIP_EMAC_LINK_MTU_MAX    TCPIP_EMAC_LINK_MTU_ETH_MAX    // max: the ETH default
#else 
#define TCPIP_EMAC_LINK_MTU_MAX    (M_TCPIP_EMAC_MAX_FRAME - 18) // max dictated by the max frame
#endif


#if TCPIP_EMAC_LINK_MTU == 0
#define M_TCPIP_EMAC_LINK_MTU TCPIP_EMAC_LINK_MTU_MAX           // use maximum value
#elif TCPIP_EMAC_LINK_MTU < TCPIP_EMAC_LINK_MTU_ETH_MIN
#define M_TCPIP_EMAC_LINK_MTU TCPIP_EMAC_LINK_MTU_ETH_MIN       // use minimum value
#elif TCPIP_EMAC_LINK_MTU > TCPIP_EMAC_LINK_MTU_MAX
#define M_TCPIP_EMAC_LINK_MTU TCPIP_EMAC_LINK_MTU_MAX           // use maximum value
#else
#define M_TCPIP_EMAC_LINK_MTU TCPIP_EMAC_LINK_MTU                // use supplied value
#endif

// Flow control defines
//
#define TCPIP_EMAC_QUANTA_PAUSE_BYTES      64  // mimimum flow control quanta PTV 

typedef enum
{
    DRV_ETHMAC_ADDBUFF_FLAG_NONE         = 0x0000,       // default value
    DRV_ETHMAC_ADDBUFF_FLAG_STICKY       = 0x0001,       // add sticky buffers
    DRV_ETHMAC_ADDBUFF_FLAG_SETSIZE      = 0x0002,       // set the RX size when adding buffers
    DRV_ETHMAC_ADDBUFF_FLAG_RX_LOCK      = 0x0004,       // request a synch lock when adding RX buffers

}DRV_ETHMAC_ADDBUFF_FLAGS;

/******************************************************************************
 * Prototypes
 ******************************************************************************/
static void*    F_MacCallocCallback( size_t nitems, size_t size, void* param );

#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
static void     F_MacTxFreeCallback(  void* ptr, void* param );
static void     F_MacRxFreeCallback(  void* ptr, void* param );
static void     F_MACCleanup(DRV_ETHMAC_INSTANCE_DCPT* pMacD );
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)

static void     F_MACDeinit(DRV_ETHMAC_INSTANCE_DCPT* pMacD );

static TCPIP_MAC_RES    F_MACTxPacket(DRV_ETHMAC_INSTANCE_DCPT* pMacD, TCPIP_MAC_PACKET * ptrPacket);
static void             F_MACTxAcknowledgeEth(DRV_ETHMAC_INSTANCE_DCPT* pMacD);
static void             F_MACTxPacketAckCallback(void* pBuff, void* fParam);
static void             F_MacTxPendingPackets(DRV_ETHMAC_INSTANCE_DCPT* pMacD, TCPIP_MAC_PACKET* pPkt);

static void             F_MacTxDiscardQueues(DRV_ETHMAC_INSTANCE_DCPT* pMacD, TCPIP_MAC_PKT_ACK_RES ackRes, bool synch);

static void             F_MacRxPacketAck(TCPIP_MAC_PACKET* pRxPkt,  const void* param);

static TCPIP_MAC_RES    DRV_ETHMAC_PIC32MACEventInit(DRV_HANDLE hMac, TCPIP_MAC_EventF eventF, const void* eventParam);
#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
static TCPIP_MAC_RES    DRV_ETHMAC_PIC32MACEventDeInit(DRV_HANDLE hMac);
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)

static uint32_t         F_DRV_ETHMAC_AddRxBuffers(DRV_ETHMAC_INSTANCE_DCPT* pMacD, uint32_t nBuffs, DRV_ETHMAC_ADDBUFF_FLAGS buffFlags);


typedef void (*F_DRV_ETHMAC_LinkStateF)(DRV_ETHMAC_INSTANCE_DCPT* pMacD);

static void F_DRV_ETHMAC_LinkStateStartLink(DRV_ETHMAC_INSTANCE_DCPT* pMacD);
static void F_DRV_ETHMAC_LinkStateGetLink(DRV_ETHMAC_INSTANCE_DCPT* pMacD);
static void F_DRV_ETHMAC_LinkStateWaitLinkUp(DRV_ETHMAC_INSTANCE_DCPT* pMacD);
static void F_DRV_ETHMAC_LinkStateNegComplete(DRV_ETHMAC_INSTANCE_DCPT* pMacD);
static void F_DRV_ETHMAC_LinkStateNegResult(DRV_ETHMAC_INSTANCE_DCPT* pMacD);


static uint32_t     F_DRV_ETHMAC_GetFrmTxOk(DRV_ETHERNET_REGISTERS* ethId);
static uint32_t     F_DRV_ETHMAC_GetFrmRxOk(DRV_ETHERNET_REGISTERS* ethId);
static uint32_t     F_DRV_ETHMAC_GetFrmPktCount(DRV_ETHERNET_REGISTERS* ethId);
static uint32_t     F_DRV_ETHMAC_GetFrmRxOvflow(DRV_ETHERNET_REGISTERS* ethId);
static uint32_t     F_DRV_ETHMAC_GetFrmFcsError(DRV_ETHERNET_REGISTERS* ethId);
static uint32_t     F_DRV_ETHMAC_GetAlignError(DRV_ETHERNET_REGISTERS* ethId);
static uint32_t     F_DRV_ETHMAC_GetSCollisionCount(DRV_ETHERNET_REGISTERS* ethId);
static uint32_t     F_DRV_ETHMAC_GetMCollisionCount(DRV_ETHERNET_REGISTERS* ethId);

static DRV_ETH_RX_FILTERS F_DRV_ETHMAC_MacToEthFilter(TCPIP_MAC_RX_FILTER_TYPE macFilter);

/******************************************************************************
 * PIC32 MAC object implementation
 ******************************************************************************/
// the embedded PIC32 MAC object
/*static*/ const TCPIP_MAC_OBJECT DRV_ETHMAC_PIC32MACObject = 
{
    .macId = (uint16_t)TCPIP_MODULE_MAC_PIC32INT,
    .macType = (uint8_t)TCPIP_MAC_TYPE_ETH,    
    .macName = "PIC32INT",
    .MAC_Initialize = &DRV_ETHMAC_PIC32MACInitialize,
#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
    .MAC_Deinitialize = &DRV_ETHMAC_PIC32MACDeinitialize,
    .MAC_Reinitialize = &DRV_ETHMAC_PIC32MACReinitialize,
#else
    .MAC_Deinitialize = NULL,
    .MAC_Reinitialize = NULL,
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)
    .MAC_Status = &DRV_ETHMAC_PIC32MACStatus,
    .MAC_Tasks = &DRV_ETHMAC_PIC32MACTasks,
    .MAC_Open = &DRV_ETHMAC_PIC32MACOpen,
    .MAC_Close = &DRV_ETHMAC_PIC32MACClose,
    .MAC_LinkCheck = &DRV_ETHMAC_PIC32MACLinkCheck,
    .MAC_RxFilterHashTableEntrySet = &DRV_ETHMAC_PIC32MACRxFilterHashTableEntrySet,
    .MAC_PowerMode = &DRV_ETHMAC_PIC32MACPowerMode,
    .MAC_PacketTx = &DRV_ETHMAC_PIC32MACPacketTx,
    .MAC_PacketRx = &DRV_ETHMAC_PIC32MACPacketRx,
    .MAC_Process = &DRV_ETHMAC_PIC32MACProcess,
    .MAC_StatisticsGet = &DRV_ETHMAC_PIC32MACStatisticsGet,
    .MAC_ParametersGet = &DRV_ETHMAC_PIC32MACParametersGet,
    .MAC_RegisterStatisticsGet = &DRV_ETHMAC_PIC32MACRegisterStatisticsGet,
    .MAC_ConfigGet = &DRV_ETHMAC_PIC32MACConfigGet,
    .MAC_EventMaskSet = &DRV_ETHMAC_PIC32MACEventMaskSet,
    .MAC_EventAcknowledge = &DRV_ETHMAC_PIC32MACEventAcknowledge,
    .MAC_EventPendingGet = &DRV_ETHMAC_PIC32MACEventPendingGet,
};

// the embedded PIC32 MAC descriptor
// to support multiple instances
// create an array/list of MAC_DCPT structures
// or allocate dynamically
#define DRV_ETHMAC_INSTANCES     1
static DRV_ETHMAC_INSTANCE_DCPT s_pic32_emb_mac_dcpt[DRV_ETHMAC_INSTANCES] = 
{
    {
        &DRV_ETHMAC_PIC32MACObject,
        // specific PIC32 MAC data 
        {
            0,
        }
    }
};

static const DRV_ETHMAC_HW_REG_DCPT macPIC32INTHwRegDcpt[] = 
{
    {"FRMTXOK ",    &F_DRV_ETHMAC_GetFrmTxOk},
    {"FRMRXOK ",    &F_DRV_ETHMAC_GetFrmRxOk},
    {"RXBUFCNT",    &F_DRV_ETHMAC_GetFrmPktCount},
    {"RXOVFLOW",    &F_DRV_ETHMAC_GetFrmRxOvflow},
    {"FCSERROR",    &F_DRV_ETHMAC_GetFrmFcsError},
    {"ALGNERR ",    &F_DRV_ETHMAC_GetAlignError},
    {"SCOLFRM ",    &F_DRV_ETHMAC_GetSCollisionCount},
    {"MCOLFRM ",    &F_DRV_ETHMAC_GetMCollisionCount},
};

// limited correspondence between a TCPIP_MAC_RX_FILTER_TYPE and a DRV_ETH_RX_FILTERS
static const DRV_ETH_RX_FILTERS DRV_ETHMAC_FiltConvTbl[] = 
{
    DRV_ETH_FILT_BCAST_ACCEPT,          // TCPIP_MAC_RX_FILTER_TYPE_BCAST_ACCEPT
    DRV_ETH_FILT_MCAST_ACCEPT,          // TCPIP_MAC_RX_FILTER_TYPE_MCAST_ACCEPT
    DRV_ETH_FILT_ME_UCAST_ACCEPT,       // TCPIP_MAC_RX_FILTER_TYPE_UCAST_ACCEPT
    DRV_ETH_FILT_NOTME_UCAST_ACCEPT,    // TCPIP_MAC_RX_FILTER_TYPE_UCAST_OTHER_ACCEPT
    DRV_ETH_FILT_RUNT_REJECT,           // TCPIP_MAC_RX_FILTER_TYPE_RUNT_REJECT
    DRV_ETH_FILT_RUNT_ACCEPT,           // TCPIP_MAC_RX_FILTER_TYPE_RUNT_ACCEPT
    DRV_ETH_FILT_CRC_ERR_REJECT,        // TCPIP_MAC_RX_FILTER_TYPE_CRC_ERROR_REJECT
    DRV_ETH_FILT_CRC_ERR_ACCEPT,        // TCPIP_MAC_RX_FILTER_TYPE_CRC_ERROR_ACCEPT
};


static const F_DRV_ETHMAC_LinkStateF DRV_ETHMAC_LinkStateTbl[] = 
{
    &F_DRV_ETHMAC_LinkStateStartLink,     // DRV_ETHMAC_LINK_CHECK_START_LINK
    &F_DRV_ETHMAC_LinkStateGetLink,       // DRV_ETHMAC_LINK_CHECK_GET_LINK
    &F_DRV_ETHMAC_LinkStateWaitLinkUp,       // DRV_ETHMAC_LINK_CHECK_WAIT_LINK_UP
    &F_DRV_ETHMAC_LinkStateNegComplete,   // DRV_ETHMAC_LINK_CHECK_NEG_COMPLETE
    &F_DRV_ETHMAC_LinkStateNegResult,     // DRV_ETHMAC_LINK_CHECK_NEG_RESULT
};


static __inline__ int __attribute__((always_inline)) F_PIC32MacIdToIx(TCPIP_MODULE_MAC_ID macId)
{
    int macIx = macId - TCPIP_MODULE_MAC_PIC32INT_0;
    if(macIx >= 0 && macIx < (int)(sizeof(s_pic32_emb_mac_dcpt) / sizeof(*s_pic32_emb_mac_dcpt)))
    {
        return macIx;
    }

    return -1;
}


#if (DRV_ETHMAC_INSTANCES == 1)
static __inline__ DRV_ETHMAC_INSTANCE_DCPT* __attribute__((always_inline)) PIC32HandleToMacInst(uintptr_t handle)
{
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)handle;
    return pMacD == s_pic32_emb_mac_dcpt ? pMacD : NULL;
}
#else
// multiple instances version
// could be refined more
static DRV_ETHMAC_INSTANCE_DCPT* PIC32HandleToMacInst(uintptr_t handle)
{
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)handle;
    int macIx = pMacD - s_pic32_emb_mac_dcpt;
    if(macIx >= 0 && macIx < sizeof(s_pic32_emb_mac_dcpt) / sizeof(*s_pic32_emb_mac_dcpt))
    {
        if(pMacD == s_pic32_emb_mac_dcpt + macIx)
        {
            return pMacD;
        }
    }

    return 0;
}
#endif  // (DRV_ETHMAC_INSTANCES == 1)

// conversion functions/helpers
static __inline__ TCPIP_MAC_INIT* __attribute__((always_inline)) CF_ModInit2MacInit(SYS_MODULE_INIT const * const init)
{
    union
    {
        SYS_MODULE_INIT const * pModinit;
        TCPIP_MAC_INIT *        pMacinit;
    }U_MAC_INIT;

    U_MAC_INIT.pModinit = init;
    return U_MAC_INIT.pMacinit;
}

static __inline__ SYS_MODULE_INIT* __attribute__((always_inline)) CF_PhyInit2ModInit(DRV_ETHPHY_INIT const * init)
{
    union
    {
        DRV_ETHPHY_INIT const * pPhyinit;
        SYS_MODULE_INIT * const pModinit;        
    }U_MOD_PHY;

    U_MOD_PHY.pPhyinit = init;
    return U_MOD_PHY.pModinit;
}

static __inline__ TCPIP_MAC_SEGMENT_GAP_DCPT * __attribute__((always_inline)) CF_Ptr8toSegGapDsc(uint8_t * pPkt)
{
    union
    {
        uint8_t * pPtr_8;; 
        TCPIP_MAC_SEGMENT_GAP_DCPT * pSegGapDsc;              
    }U_PTR8_SEGGAPDSC;

    U_PTR8_SEGGAPDSC.pPtr_8 = pPkt;
    return U_PTR8_SEGGAPDSC.pSegGapDsc;
}

static __inline__ TCPIP_MAC_PACKET_RX_STAT * __attribute__((always_inline)) CF_EthtoTcipRxStat(DRV_ETHMAC_PKT_STAT_RX const * pRxStat)
{
    union
    {
        DRV_ETHMAC_PKT_STAT_RX const * pEth_RxStat;; 
        TCPIP_MAC_PACKET_RX_STAT * pTcpip_RxStat;              
    }U_ETH_TCPIP_RXSTAT;

    U_ETH_TCPIP_RXSTAT.pEth_RxStat = pRxStat;
    return U_ETH_TCPIP_RXSTAT.pTcpip_RxStat;
}

static __inline__ DRV_ETHMAC_PKT_DCPT const * __attribute__((always_inline)) CF_MacDataSegtoPktDcpt(TCPIP_MAC_DATA_SEGMENT  * pDSeg)
{
    union
    {
        TCPIP_MAC_DATA_SEGMENT  * pMacDseg;; 
        DRV_ETHMAC_PKT_DCPT const * pPktDcpt;              
    }U_MAC_DSEG_PKTDCPT;

    U_MAC_DSEG_PKTDCPT.pMacDseg = pDSeg;
    return U_MAC_DSEG_PKTDCPT.pPktDcpt;
}

static __inline__ DRV_ETHMAC_SGL_LIST_NODE * __attribute__((always_inline)) CF_EthMacPkt2LstNode(TCPIP_MAC_PACKET * ptrPacket)
{
    union
    {
        TCPIP_MAC_PACKET * pPkt;
        DRV_ETHMAC_SGL_LIST_NODE  * pNode;
    }U_PKT_NODE;

    U_PKT_NODE.pPkt = ptrPacket;
    return U_PKT_NODE.pNode;
}
static __inline__ TCPIP_MAC_PACKET * __attribute__((always_inline)) CF_LstNode2EthMacPkt(DRV_ETHMAC_SGL_LIST_NODE  * lstNode)
{
    union
    {
        DRV_ETHMAC_SGL_LIST_NODE  * pNode;
        TCPIP_MAC_PACKET * pPkt;        
    }U_NODE_PKT;

    U_NODE_PKT.pNode = lstNode;
    return U_NODE_PKT.pPkt;
}

static __inline__ uint32_t __attribute__((always_inline)) CF_VoidPtr2Uint(void * pParam)
{
    union
    {
        void* pPtr;
        uint32_t uintAddr;
    }U_VPTR_UINT;

    U_VPTR_UINT.pPtr = pParam;
    return U_VPTR_UINT.uintAddr;
}

static __inline__ DRV_ETHMAC_INSTANCE_DCPT* __attribute__((always_inline)) CF_VoidPtr2EthDcpt(void const * pParam)
{
    union
    {
        const void* pPtr;
        DRV_ETHMAC_INSTANCE_DCPT * pEthDcpt;
    }U_VPTR_ETHDCPT;

    U_VPTR_ETHDCPT.pPtr = pParam;
    return U_VPTR_ETHDCPT.pEthDcpt;
}
#if defined(__PIC32MZ__)

// invalidates a range of addresses from the cache
static void F_DRV_ETHMAC_CacheInvalidate(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    DRV_ETHMAC_LIB_DataLineFlush(&pMacD->mData.EnetTxFreeList);
    DRV_ETHMAC_LIB_DataLineFlush(&pMacD->mData.EnetTxBusyList);
    DRV_ETHMAC_LIB_DataLineFlush(&pMacD->mData.EnetRxFreeList);
    DRV_ETHMAC_LIB_DataLineFlush(&pMacD->mData.EnetRxBusyList);

}
#endif  // defined(__PIC32MZ__)


/*
 * interface functions
 *
*/

/****************************************************************************
 * Function:        DRV_ETHMAC_PIC32MACInitialize
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *                
 * Side Effects:    None
 *
 * Overview:        This function initializes the Eth controller, the MAC and the associated PHY.
 *                  It should be called to be able to schedule any Eth transmit or receive operation.
 *
 * Note:            Only one client per MAC supported. 
 *****************************************************************************/
SYS_MODULE_OBJ DRV_ETHMAC_PIC32MACInitialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init)
{
    union
    {
        double      align;              // alignement
        uint8_t     addr[6];            // address itself
    }alignMacAddress;        // aligned MAC address

    uint32_t    nRxBuffs;
    DRV_ETHPHY_RESULT   phyInitRes;
    uint8_t     useFactMACAddr[6] = {0x00, 0x04, 0xa3, 0x00, 0x00, 0x00};       // to check if factory programmed MAC address needed
    uint8_t     unsetMACAddr[6] =   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};       // not set MAC address

    TCPIP_MAC_RES   initRes;
    int             macIx, phyIx;
    DRV_ETHERNET_REGISTERS*   ethId;
    DRV_ETH_RX_FILTERS ethRxFilt;
    DRV_ETHMAC_INSTANCE_DCPT* pMacD;
    SYS_MODULE_OBJ hPhySysObject;
    DRV_HANDLE     hPhyClient;
    uint16_t        nRxDedicatedBuffers, nRxInitBuffers;
    TCPIP_MAC_INIT* initMac = CF_ModInit2MacInit(init); 
    const TCPIP_MAC_MODULE_CTRL* const macControl = initMac->macControl; 
    const TCPIP_MODULE_MAC_PIC32INT_CONFIG* initData = (const TCPIP_MODULE_MAC_PIC32INT_CONFIG*)initMac->moduleData;

    macIx = F_PIC32MacIdToIx((TCPIP_MODULE_MAC_ID)index);

    if(macIx < 0 )
    {   
        return SYS_MODULE_OBJ_INVALID;      // no such type supported
    }
   
    pMacD = s_pic32_emb_mac_dcpt + macIx;
    
    if(pMacD->mData.macFlags.macInit != 0)
    {   // already initialized 
        return (SYS_MODULE_OBJ)pMacD;
    }
    
    if(pMacD->mData.macFlags.macOpen != 0)
    { 
        return SYS_MODULE_OBJ_INVALID;     // have a client connected
    }

    if(macControl->memH == NULL)
    {     // not possible without dynamic memory!        
        return SYS_MODULE_OBJ_INVALID;
    }
    
    if(initData == NULL)
    {
        return SYS_MODULE_OBJ_INVALID;     // not possible without init data!        
    }

    if((initData->pPhyBase == NULL) || (initData->pPhyInit == NULL))
    {
        return SYS_MODULE_OBJ_INVALID;     // not possible without a PHY!        
    }

    // init the MAC object
    (void)memset(&pMacD->mData, 0x0, sizeof(pMacD->mData)); 
    pMacD->mData.hPhyClient = DRV_HANDLE_INVALID;
    pMacD->mData.hPhySysObject = SYS_MODULE_OBJ_INVALID;
    pMacD->mData.sysStat = SYS_STATUS_UNINITIALIZED;
#if defined(__PIC32MZ__)
    F_DRV_ETHMAC_CacheInvalidate(pMacD);
#endif  //  defined(__PIC32MZ__)
    pMacD->mData.macIx = (uint16_t)macIx;
    phyIx = macIx;  
    pMacD->mData.phyIx = (uint16_t)macIx;    // use the same index for the associated PHY
    pMacD->mData.macFlags.linkPrev = 0;

    pMacD->mData.macIntSrc = DRV_ETHMAC_INTERRUPT_SOURCE;
    
    // use initialization data
    pMacD->mData.mac_AllocH = macControl->memH;
    pMacD->mData.mac_callocF = macControl->callocF;
    pMacD->mData.mac_freeF = macControl->freeF;
    pMacD->mData.gapDcptOffset = macControl->gapDcptOffset;    // Note: the driver assumes that the packet allocator sets the packet pointer for each allocated packet!
    pMacD->mData.gapDcptSize = macControl->gapDcptSize;


    pMacD->mData.pktAllocF = macControl->pktAllocF;
    pMacD->mData.pktFreeF = macControl->pktFreeF;
    pMacD->mData.pktAckF = macControl->pktAckF;

    pMacD->mData.macSynchF = macControl->synchF;

    pMacD->mData.dataOffsetMask = ((macControl->controlFlags & (uint16_t)TCPIP_MAC_CONTROL_PAYLOAD_OFFSET_2) != 0U) ? 0xfffffffcU : 0xffffffffU;
    pMacD->mData.controlFlags = macControl->controlFlags;

    // copy the configuration data
    pMacD->mData.macConfig = *initData; 
    // adjust parameters
    if(pMacD->mData.macConfig.nTxDescriptors < ETH_PIC32_INT_MAC_MIN_TX_DESCRIPTORS)
    {
        pMacD->mData.macConfig.nTxDescriptors = ETH_PIC32_INT_MAC_MIN_TX_DESCRIPTORS;
    }
    if(pMacD->mData.macConfig.rxBuffSize < ETH_PIC32_INT_MAC_MIN_RX_SIZE)
    {
        pMacD->mData.macConfig.rxBuffSize = ETH_PIC32_INT_MAC_MIN_RX_SIZE;  
    }
    else
    {
        pMacD->mData.macConfig.rxBuffSize = ((pMacD->mData.macConfig.rxBuffSize + 15U) / 16U) * 16U;   // adjust RX size
    }
    pMacD->mData.pEthReg = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;

#if (TCPIP_EMAC_RX_FRAGMENTS == 1)
    // there's only one descriptor
    pMacD->mData.rxPktDcpt[0].next = NULL;
#endif  // (TCPIP_EMAC_RX_FRAGMENTS == 1)

    if(!F_DRV_ETHMAC_RxCreate(pMacD))
    {
        return SYS_MODULE_OBJ_INVALID;     // failed to create synch lock
    }
    if(!F_DRV_ETHMAC_TxCreate(pMacD))
    {
        F_DRV_ETHMAC_RxDelete(pMacD);
        return SYS_MODULE_OBJ_INVALID;     // failed to create synch lock
    }
    
    initRes = TCPIP_MAC_RES_OK;
    ethId = pMacD->mData.pEthReg;

    while(true)
    {
        DRV_ETHPHY_SETUP phySetup;
        const DRV_ETHPHY_OBJECT_BASE* pPhyBase;   


        // start the initialization sequence    
        (void) SYS_INT_SourceDisable(pMacD->mData.macIntSrc);      // stop Eth ints
        DRV_ETHMAC_LibInit(pMacD);
        SYS_INT_SourceStatusClear(pMacD->mData.macIntSrc); // clear any pending interrupt flag
        DRV_ETH_MaxFrameLengthSet(ethId, M_TCPIP_EMAC_MAX_FRAME);

        pPhyBase =  pMacD->mData.macConfig.pPhyBase;

        hPhySysObject = pPhyBase->phy_Initialize(phyIx, CF_PhyInit2ModInit(pMacD->mData.macConfig.pPhyInit));
        if ( hPhySysObject == SYS_MODULE_OBJ_INVALID )
        {
            initRes = TCPIP_MAC_RES_PHY_INIT_FAIL;
            break;
        }

        pMacD->mData.hPhySysObject = hPhySysObject;
        hPhyClient = pPhyBase->phy_Open(phyIx, DRV_IO_INTENT_READWRITE);
        if ( DRV_HANDLE_INVALID == hPhyClient )
        {
            initRes = TCPIP_MAC_RES_PHY_INIT_FAIL;
            break;
        }

        pMacD->mData.hPhyClient = hPhyClient;

        phySetup.phyAddress = (int32_t)pMacD->mData.macConfig.pPhyInit->phyAddress;
        phySetup.openFlags = pMacD->mData.macConfig.ethFlags; 
        phySetup.configFlags = pMacD->mData.macConfig.pPhyInit->phyFlags;
        phySetup.macPauseType = DRV_ETHMAC_PAUSE_CPBL_MASK;
        phySetup.resetFunction = pMacD->mData.macConfig.pPhyInit->resetFunction;

        phyInitRes = pPhyBase->phy_Setup(hPhyClient, &phySetup, &pMacD->mData.linkResFlags);
        if((uint32_t)phyInitRes < 0U)
        {   // some error occurred
            initRes = TCPIP_MAC_RES_PHY_INIT_FAIL;
            break;
        }

        // OK, we can continue initalization
        // let the PHY initialization and auto-negotiation (if any) take place
        // and continue the initialization

        // Set the RX filters
        DRV_ETH_RxFiltersClr(ethId, DRV_ETH_FILT_ALL_FILTERS);
        uint32_t tmpRxfilter = TCPIP_EMAC_RX_FILTERS;
        ethRxFilt = F_DRV_ETHMAC_MacToEthFilter((TCPIP_MAC_RX_FILTER_TYPE)tmpRxfilter);
        DRV_ETH_RxFiltersSet(ethId, ethRxFilt);

#if (TCPIP_EMAC_AUTO_FLOW_CONTROL_ENABLE) 
        // sanity check for water marks
        uint8_t fullWMPkts = (uint8_t)TCPIP_EMAC_FLOW_CONTROL_FULL_WMARK;
        uint8_t emptyWMPkts = (uint8_t)TCPIP_EMAC_FLOW_CONTROL_EMPTY_WMARK;

        // enable the AutoFC
        uint32_t ptValue = (TCPIP_EMAC_FLOW_CONTROL_PAUSE_BYTES + TCPIP_EMAC_QUANTA_PAUSE_BYTES - 1) / TCPIP_EMAC_QUANTA_PAUSE_BYTES;
        DRV_ETH_PauseTimerSet(ethId, (uint16_t)ptValue);

        DRV_ETH_RxFullWmarkSet(ethId, fullWMPkts);
        DRV_ETH_RxEmptyWmarkSet(ethId, emptyWMPkts);

        DRV_ETH_TxPauseEnable(ethId);
        DRV_ETH_RxPauseEnable(ethId);
        DRV_ETH_AutoFlowControlEnable(ethId);
#endif  // (TCPIP_EMAC_AUTO_FLOW_CONTROL_ENABLE) 
        
        // set the MAC address
        (void) memcpy(alignMacAddress.addr, macControl->ifPhyAddress.v, sizeof(alignMacAddress.addr));
        if(memcmp(alignMacAddress.addr, useFactMACAddr, sizeof(useFactMACAddr)) !=0 && (memcmp(alignMacAddress.addr, unsetMACAddr, sizeof(unsetMACAddr)) !=0) )
        {   // use the supplied address
            DRV_ETH_MACSetAddress(ethId, alignMacAddress.addr);
        }
        // else use the factory programmed address existent in the MAC
                
        if(DRV_ETHMAC_LibDescriptorsPoolAdd(pMacD, (int32_t)pMacD->mData.macConfig.nTxDescriptors, DRV_ETHMAC_DCPT_TYPE_TX, &F_MacCallocCallback, (void*)pMacD)!=pMacD->mData.macConfig.nTxDescriptors)
        {
            initRes = TCPIP_MAC_RES_ALLOC_ERR;
            break;
        }

        if(DRV_ETHMAC_LibDescriptorsPoolAdd(pMacD, (int32_t)pMacD->mData.macConfig.nRxDescriptors, DRV_ETHMAC_DCPT_TYPE_RX, &F_MacCallocCallback, (void*)pMacD)!=pMacD->mData.macConfig.nRxDescriptors)
        {
            initRes = TCPIP_MAC_RES_ALLOC_ERR;
            break;
        }

        // add RX dedicated buffers
        if((nRxDedicatedBuffers = pMacD->mData.macConfig.nRxDedicatedBuffers) != 0U)
        {
            nRxBuffs = F_DRV_ETHMAC_AddRxBuffers(pMacD, (uint32_t)nRxDedicatedBuffers, (DRV_ETHMAC_ADDBUFF_FLAGS)(DRV_ETHMAC_ADDBUFF_FLAG_STICKY | DRV_ETHMAC_ADDBUFF_FLAG_SETSIZE));
            if(nRxBuffs != (uint32_t)nRxDedicatedBuffers)
            {   // failed to allocate all buffers
                initRes = TCPIP_MAC_RES_ALLOC_ERR;
                break;
            }
        }
        // add RX non-dedicated buffers
        if((nRxInitBuffers = pMacD->mData.macConfig.nRxInitBuffers) != 0U)
        {
            nRxBuffs = F_DRV_ETHMAC_AddRxBuffers(pMacD, (uint32_t)nRxInitBuffers, nRxDedicatedBuffers ? DRV_ETHMAC_ADDBUFF_FLAG_NONE : DRV_ETHMAC_ADDBUFF_FLAG_SETSIZE);
            if(nRxBuffs != (uint32_t)nRxInitBuffers)
            {   // failed to allocate all buffers
                initRes = TCPIP_MAC_RES_ALLOC_ERR;
                break;
            }
        }

        if(DRV_ETHMAC_PIC32MACEventInit((DRV_HANDLE)pMacD, macControl->eventF, macControl->eventParam) != TCPIP_MAC_RES_OK)
        {
            initRes = TCPIP_MAC_RES_EVENT_INIT_FAIL;
            break;
        }
        // end of initialization        
        break;
    }



    if(initRes != TCPIP_MAC_RES_OK)
    {   // failed; release what we've allocated
        F_MACDeinit(pMacD);        
        return SYS_MODULE_OBJ_INVALID;
    }


    // success
    // remaining initialization is done by DRV_ETHMAC_PIC32MACTasks
    
    pMacD->mData.macFlags.macInit = 1;
    pMacD->mData.sysStat = SYS_STATUS_BUSY;
    return (SYS_MODULE_OBJ)pMacD;
}

/****************************************************************************
 * Function:        DRV_ETHMAC_PIC32MACDeinitialize
 *
 * PreCondition:    None
 *
 * Input:           stackData - standard stack initialization structure
 *
 * Output:          TCPIP_MAC_RES
 *                
 * Side Effects:    None
 *
 * Overview:        This function de-initializes the Eth MAC controller.
 *
 * Note:            None 
 *****************************************************************************/
#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
void DRV_ETHMAC_PIC32MACDeinitialize(SYS_MODULE_OBJ object)
{
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = PIC32HandleToMacInst(object);

    if((pMacD != NULL) && (pMacD->mData.macFlags.macInit != 0))
    {
        F_MACDeinit(pMacD);
    }
}

void DRV_ETHMAC_PIC32MACReinitialize(SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init)
{
    // not supported
}
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)

SYS_STATUS DRV_ETHMAC_PIC32MACStatus (SYS_MODULE_OBJ object)
{
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = PIC32HandleToMacInst(object);

    if((pMacD != NULL) && (pMacD->mData.macFlags.macInit != 0))
    {
        return pMacD->mData.sysStat;
    }

    return SYS_STATUS_ERROR;
}

void DRV_ETHMAC_PIC32MACTasks(SYS_MODULE_OBJ object)
{
    TCPIP_ETH_PAUSE_TYPE        pauseType;
    DRV_ETHPHY_CLIENT_STATUS    phyStat;
    DRV_HANDLE                  hPhyClient;
    DRV_ETHPHY_RESULT           phyInitRes;
    const DRV_ETHPHY_OBJECT_BASE* pPhyBase;   
    DRV_ETHMAC_INSTANCE_DCPT*   pMacD = PIC32HandleToMacInst(object);

    if((pMacD == NULL) || (pMacD->mData.macFlags.macInit == 0))
    {   // nothing to do
        return;
    }

    pPhyBase =  pMacD->mData.macConfig.pPhyBase;
    pPhyBase->phy_Tasks(pMacD->mData.hPhySysObject);
    
    switch(pMacD->mData.sysStat)
    {
        case SYS_STATUS_BUSY:
            // perform the PHY initialization

            hPhyClient = pMacD->mData.hPhyClient;
            phyStat = pPhyBase->phy_ClientStatus(hPhyClient);
            if(phyStat == DRV_ETHPHY_CLIENT_STATUS_BUSY)
            {   // wait some more
                return;
            }

            phyInitRes = pPhyBase->phy_ClientOperationResult(hPhyClient);

            if ( phyInitRes != DRV_ETHPHY_RES_OK )
            {
                F_MACDeinit(pMacD);
                pMacD->mData.sysStat = SYS_STATUS_ERROR; 
                SYS_ERROR_PRINT(SYS_ERROR_ERROR, "DRV PHY init failed: %d\r\n", phyInitRes);
                break;
            }

            // PHY was detected properly
            pMacD->mData.macFlags.linkPresent = 1;
            if((pMacD->mData.macConfig.ethFlags & TCPIP_ETH_OPEN_AUTO) != 0)
            {   // we'll just wait for the negotiation to be done
                pMacD->mData.macFlags.linkNegotiation = 1;    // performing the negotiation
            }
            else
            {   // no need of negotiation results; just update the MAC
                pauseType = (pMacD->mData.macConfig.ethFlags & TCPIP_ETH_OPEN_FDUPLEX) ? DRV_ETHMAC_PAUSE_CPBL_MASK : TCPIP_ETH_PAUSE_TYPE_NONE;
                DRV_ETHMAC_LibMACOpen(pMacD, pMacD->mData.linkResFlags, pauseType);
            }

            pMacD->mData.linkUpTick = SYS_TMR_TickCountGet();     // the initialization time
            pMacD->mData.linkWaitTick = ((SYS_TMR_TickCounterFrequencyGet() * pMacD->mData.macConfig.linkInitDelay) + 999 ) / 1000;
            pMacD->mData.linkCheckState = DRV_ETHMAC_LINK_CHECK_START_LINK;

            pMacD->mData.sysStat = SYS_STATUS_READY; 
            break; 

        default:    // SYS_STATUS_READY
            break;
    }
}


size_t DRV_ETHMAC_PIC32MACConfigGet(DRV_HANDLE hMac, void* configBuff, size_t buffSize, size_t* pConfigSize)
{
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = PIC32HandleToMacInst(hMac);

    if(pMacD != NULL)
    {
        if(pConfigSize != NULL)
        {
            *pConfigSize =  sizeof(TCPIP_MODULE_MAC_PIC32INT_CONFIG);
        }

        if(configBuff && buffSize >= sizeof(TCPIP_MODULE_MAC_PIC32INT_CONFIG))
        {   // can copy the data
            TCPIP_MODULE_MAC_PIC32INT_CONFIG* pMacConfig = (TCPIP_MODULE_MAC_PIC32INT_CONFIG*)configBuff;

            *pMacConfig = pMacD->mData.macConfig;
            return sizeof(TCPIP_MODULE_MAC_PIC32INT_CONFIG);
        }
    }

    return 0;

}

DRV_HANDLE DRV_ETHMAC_PIC32MACOpen(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent)
{
    int                 macIx;
    DRV_ETHMAC_INSTANCE_DCPT* pMacD;
    DRV_HANDLE    hMac = DRV_HANDLE_INVALID;

    macIx = F_PIC32MacIdToIx((TCPIP_MODULE_MAC_ID)drvIndex);

    if(macIx >= 0 )
    {   
        pMacD = s_pic32_emb_mac_dcpt + macIx;
        if(pMacD->mData.macFlags.macInit == 1)
        {
            if(pMacD->mData.macFlags.macOpen == 0)
            {   // only one client for now
                pMacD->mData.macFlags.macOpen = 1;
                hMac = (DRV_HANDLE)pMacD;
            }
#if defined(DRV_ETHMAC_CLIENTS_NUMBER) && (DRV_ETHMAC_CLIENTS_NUMBER > 1)
            else
            {   // allow multiple clients
                hMac = (DRV_HANDLE)pMacD;
            }
#endif
        }
    }

    return hMac;
}


void DRV_ETHMAC_PIC32MACClose( DRV_HANDLE hMac )
{
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = PIC32HandleToMacInst(hMac);

    if((pMacD != NULL) && (pMacD->mData.macFlags.macInit == 1))
    {
        pMacD->mData.macFlags.macOpen = 0;
    }
}

/**************************
 * TX functions
 ***********************************************/

TCPIP_MAC_RES DRV_ETHMAC_PIC32MACPacketTx(DRV_HANDLE hMac, TCPIP_MAC_PACKET * ptrPacket)
{
    TCPIP_MAC_PACKET*   pPkt;
    TCPIP_MAC_DATA_SEGMENT* pSeg;
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = PIC32HandleToMacInst(hMac);

    if(pMacD == NULL)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

    F_MACTxAcknowledgeEth(pMacD);

    pPkt = ptrPacket;

    // check that packets are properly formatted and
    // have room to store the packet info
    while(pPkt != NULL)
    {
        pSeg = pPkt->pDSeg;
        if(pSeg == NULL)
        {   // cannot send this packet
            return TCPIP_MAC_RES_PACKET_ERR;
        }

        pPkt = pPkt->next;
    }

    // pkt OK; add it and transmit the pending packets
    // don't transmit out of order
    F_MacTxPendingPackets(pMacD, ptrPacket);

    // it's been scheduled...somehow
    return TCPIP_MAC_RES_PENDING;
}

/**************************
 * RX functions
 ***********************************************/

// returns a pending RX packet if exists
TCPIP_MAC_PACKET* DRV_ETHMAC_PIC32MACPacketRx (DRV_HANDLE hMac, TCPIP_MAC_RES* pRes, TCPIP_MAC_PACKET_RX_STAT* pPktStat)
{
    DRV_ETHMAC_RESULT       ethRes;
    DRV_ETHMAC_PKT_DCPT     *pRootDcpt, *pLastDcpt;
    TCPIP_MAC_RES           mRes;
#if (TCPIP_EMAC_RX_FRAGMENTS > 1)
    TCPIP_MAC_PACKET       *pCurrPkt, *pPrevPkt;
    TCPIP_MAC_DATA_SEGMENT *pCurrDSeg;
    DRV_ETHMAC_PKT_DCPT    *pCurrDcpt;
#endif  // (TCPIP_EMAC_RX_FRAGMENTS > 1)

    DRV_ETHMAC_INSTANCE_DCPT* pMacD = PIC32HandleToMacInst(hMac);
    if(pMacD == NULL)
    {
        return NULL;
    }

    TCPIP_MAC_PACKET* pRxPkt = NULL;
    const DRV_ETHMAC_PKT_STAT_RX* pRxPktStat = NULL;
    int               buffsPerRxPkt = 0;

    // prepare the receive packet descriptor
    // for receiving a new packet
    pLastDcpt = pRootDcpt = pMacD->mData.rxPktDcpt;
#if (TCPIP_EMAC_RX_FRAGMENTS > 1)
    int ix;
    for(ix = 0; ix < sizeof(pMacD->mData.rxPktDcpt)/sizeof(*pMacD->mData.rxPktDcpt); ix++)
    {
        pLastDcpt->next = pLastDcpt + 1;
        pLastDcpt++;
    }
    // adjust last descriptor
    pLastDcpt--;
    pLastDcpt->next = NULL;
#endif  // (TCPIP_EMAC_RX_FRAGMENTS > 1)

    F_DRV_ETHMAC_RxLock(pMacD);
    ethRes = DRV_ETHMAC_LibRxGetPacket (pMacD, pRootDcpt, &buffsPerRxPkt, &pRxPktStat );
    F_DRV_ETHMAC_RxUnlock(pMacD);

    if(ethRes == DRV_ETHMAC_RES_PACKET_QUEUED || ethRes == DRV_ETHMAC_RES_NO_PACKET)
    {   // done, no more packets
        mRes = TCPIP_MAC_RES_PENDING;
    }
    else if(ethRes == DRV_ETHMAC_RES_OK)
    {   // available packet; minimum check
        if(pRxPktStat->rxOk == 0 || pRxPktStat->runtPkt != 0 || pRxPktStat->crcError != 0)
        {   // corrupted packet; discrd/re-insert it
            mRes = TCPIP_MAC_RES_PACKET_ERR;
            pMacD->mData.rxStat.nRxErrorPackets++;
        }
        else
        {
            mRes = TCPIP_MAC_RES_OK;
            pMacD->mData.rxStat.nRxOkPackets++;
        }
    }
    else
    {   // DRV_ETHMAC_RES_RX_PKT_SPLIT_ERR; too many fragments 
        mRes = TCPIP_MAC_RES_FRAGMENT_ERR;
        pMacD->mData.rxStat.nRxFragmentErrors++;
    }

    if(pRes != NULL)
    {
        *pRes = mRes;
    }

    if(mRes == TCPIP_MAC_RES_OK)
    {   // valid ETH packet;
        // format properly and return the packet pointer
        // restore packet the buffer belongs to        
#if (TCPIP_EMAC_RX_FRAGMENTS > 1)
        pPrevPkt = NULL;
        pCurrDcpt = pRootDcpt;
        pLastDcpt = pRootDcpt;
        while((pCurrDcpt != NULL) && (pCurrDcpt->pBuff != NULL))
        {
            uint8_t* segBuff = (uint8_t*)(CF_VoidPtr2Uint(pCurrDcpt->pBuff) & pMacD->mData.dataOffsetMask);
            TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = CF_Ptr8toSegGapDsc(segBuff + pMacD->mData.gapDcptOffset);
            pCurrPkt = pGap->segmentPktPtr;
                
            pCurrDSeg = pCurrPkt->pDSeg;
            pCurrDSeg->segLen = pCurrDcpt->nBytes;
            pCurrDSeg->next = NULL;
            if(pPrevPkt == NULL)
            {
                pRxPkt = pCurrPkt;
            }
            else
            {
                pPrevPkt->pDSeg->next = pCurrDSeg;
            }
            pPrevPkt = pCurrPkt;
            pLastDcpt = pCurrDcpt;
            pCurrDcpt = pCurrDcpt->next;
        }
        // adjust the last segment for FCS size
        pPrevPkt->pDSeg->segLen -= 4;
        pLastDcpt->nBytes -= 4;

        if(buffsPerRxPkt > 1)
        {
            pRxPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_SPLIT;
        } 
#else
        uint8_t* segBuff = (uint8_t*)(CF_VoidPtr2Uint(pRootDcpt->pBuff) & pMacD->mData.dataOffsetMask);
        TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = CF_Ptr8toSegGapDsc(segBuff + pMacD->mData.gapDcptOffset);
        pRxPkt = pGap->segmentPktPtr;
        pRxPkt->pDSeg->next = NULL;
        // adjust the last segment for FCS size
        pLastDcpt->nBytes -= 4;
#endif  // (TCPIP_EMAC_RX_FRAGMENTS > 1)

        // no maintenance of the total packet length
        // could be obtained through pPktStat 
        pRxPkt->pDSeg->segLen = pRootDcpt->nBytes - (uint16_t)(sizeof(TCPIP_MAC_ETHERNET_HEADER));
        // Note: re-set pMacLayer and pNetLayer; IPv6 changes these pointers inside the packet!
        pRxPkt->pMacLayer = pRxPkt->pDSeg->segLoad;
        pRxPkt->pNetLayer = pRxPkt->pMacLayer + sizeof(TCPIP_MAC_ETHERNET_HEADER);

        pRxPkt->tStamp = SYS_TMR_TickCountGet();
        pRxPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_QUEUED;



        pRxPkt->pktFlags &= ~TCPIP_MAC_PKT_FLAG_CAST_MASK;
        if((pRxPktStat->bMatch) != 0U)
        {
            pRxPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_BCAST;
        }
        else if((pRxPktStat->mMatch) != 0U)
        {
            pRxPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_MCAST;
        }
        else
        {
            pRxPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_UNICAST;
        }

        if(pPktStat != NULL)
        {
            *pPktStat = *(CF_EthtoTcipRxStat(pRxPktStat)); 
        }
        // success
        return pRxPkt;
    }

    // failure; acknowledge the packet and re-insert
    if((pRootDcpt->pBuff) != NULL)
    {   // we have a failed packet available
        F_DRV_ETHMAC_RxLock(pMacD);
        (void)DRV_ETHMAC_LibRxAcknowledgePacket(pMacD, pRootDcpt);
        F_DRV_ETHMAC_RxUnlock(pMacD);
    }

    return NULL;

}


// verify the link status

bool DRV_ETHMAC_PIC32MACLinkCheck(DRV_HANDLE hMac)
{
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = PIC32HandleToMacInst(hMac);

    if((pMacD == NULL) || (pMacD->mData.macFlags.linkPresent == 0))
    {
        return false;
    }

    const DRV_ETHPHY_OBJECT_BASE* pPhyBase =  pMacD->mData.macConfig.pPhyBase;
    pPhyBase->phy_Tasks(pMacD->mData.hPhySysObject);

    (*DRV_ETHMAC_LinkStateTbl[pMacD->mData.linkCheckState])(pMacD);

    return pMacD->mData.macFlags.linkPrev == 1;
}

static void F_DRV_ETHMAC_LinkStateDown(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    // link is down; reset state flags, initialization time 
    pMacD->mData.macFlags.linkPrev = 0;
    pMacD->mData.macFlags.linkUpDone = 0;
    pMacD->mData.linkUpTick = SYS_TMR_TickCountGet();
    pMacD->mData.linkCheckState = DRV_ETHMAC_LINK_CHECK_START_LINK;
}

static void F_DRV_ETHMAC_LinkStateStartLink(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    DRV_ETHPHY_RESULT phyRes;
    
    
    phyRes = pMacD->mData.macConfig.pPhyBase->phy_LinkStatusGet(pMacD->mData.hPhyClient, DRV_ETHPHY_INF_IDX_ALL_EXTERNAL, &pMacD->mData.negResult.linkStatus, false);
    if(phyRes < 0)
    {   // some error occurred
        F_DRV_ETHMAC_LinkStateDown(pMacD);
        return;
    }

    pMacD->mData.linkCheckState = DRV_ETHMAC_LINK_CHECK_GET_LINK;

}

static void F_DRV_ETHMAC_LinkStateGetLink(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    DRV_ETHPHY_CLIENT_STATUS phyStat;
    DRV_ETHPHY_RESULT phyRes;
    const DRV_ETHPHY_OBJECT_BASE* pPhyBase;   
    bool linkCurrUp;

    
    pPhyBase =  pMacD->mData.macConfig.pPhyBase;
    phyStat = pPhyBase->phy_ClientStatus( pMacD->mData.hPhyClient );
    if(phyStat == DRV_ETHPHY_CLIENT_STATUS_BUSY)
    {   // not done yet
        return;
    }

    phyRes = pPhyBase->phy_ClientOperationResult(pMacD->mData.hPhyClient);
    if(phyRes != DRV_ETHPHY_RES_OK)
    {   // some error occurred
        F_DRV_ETHMAC_LinkStateDown(pMacD);
        return;
    }

    linkCurrUp = (pMacD->mData.negResult.linkStatus & DRV_ETHPHY_LINK_ST_UP) != 0;

    if(linkCurrUp == false)
    {   
        F_DRV_ETHMAC_LinkStateDown(pMacD);
        return;
    }

    // link up
    if(pMacD->mData.macFlags.linkPrev == linkCurrUp)
    {   // PHY state not changed 
        pMacD->mData.linkCheckState = DRV_ETHMAC_LINK_CHECK_START_LINK;
    }
    else
    {
        pMacD->mData.linkCheckState = DRV_ETHMAC_LINK_CHECK_WAIT_LINK_UP;
    }
}

// ok, the link is coming up
static void F_DRV_ETHMAC_LinkStateWaitLinkUp(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    DRV_ETHPHY_RESULT   phyRes;

    if(pMacD->mData.macFlags.linkUpDone == 0)
    {   // wait the init tmo before checking the link up
        uint32_t currTick = SYS_TMR_TickCountGet();
        if(currTick - pMacD->mData.linkUpTick < pMacD->mData.linkWaitTick)
        {
            return;
        }
        pMacD->mData.macFlags.linkUpDone = 1;
    }

    // initialization delay passed
    if(pMacD->mData.macFlags.linkNegotiation == 0)
    {   // no auto negotiation taking place; this is the link status
        pMacD->mData.macFlags.linkPrev = (uint8_t)true;
        pMacD->mData.linkCheckState = DRV_ETHMAC_LINK_CHECK_START_LINK;
        return;
    }

    // auto negotiation is turned on
    // we have to re-negotiate but the link is actually up only
    // after the re-negotiation is done
    // wait for negotiation complete
    phyRes = pMacD->mData.macConfig.pPhyBase->phy_NegotiationIsComplete(pMacD->mData.hPhyClient, DRV_ETHPHY_INF_IDX_ALL_EXTERNAL, false);
    if(phyRes < 0)
    {   // some error occurred
        F_DRV_ETHMAC_LinkStateDown(pMacD);
    }
    else
    {   // advance
        pMacD->mData.linkCheckState = DRV_ETHMAC_LINK_CHECK_NEG_COMPLETE;
    }
}

static void F_DRV_ETHMAC_LinkStateNegComplete(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    DRV_ETHPHY_CLIENT_STATUS phyStat;
    DRV_ETHPHY_RESULT phyRes;
    const DRV_ETHPHY_OBJECT_BASE* pPhyBase;   

    pPhyBase =  pMacD->mData.macConfig.pPhyBase;
    phyStat = pPhyBase->phy_ClientStatus( pMacD->mData.hPhyClient );
    if(phyStat == DRV_ETHPHY_CLIENT_STATUS_BUSY)
    {   // not done yet
        return;
    }

    phyRes = pPhyBase->phy_ClientOperationResult(pMacD->mData.hPhyClient);
    if(phyRes != DRV_ETHPHY_RES_OK)
    {   // some error occurred
        F_DRV_ETHMAC_LinkStateDown(pMacD);
        return;
    }

    phyRes = pPhyBase->phy_NegotiationResultGet(pMacD->mData.hPhyClient, DRV_ETHPHY_INF_IDX_ALL_EXTERNAL, &pMacD->mData.negResult);

    if(phyRes < 0)
    {   // some error occurred
        F_DRV_ETHMAC_LinkStateDown(pMacD);
    }
    else
    {   // advance; get negotiation result
        pMacD->mData.linkCheckState = DRV_ETHMAC_LINK_CHECK_NEG_RESULT;
    }
}


static void F_DRV_ETHMAC_LinkStateNegResult(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    DRV_ETHPHY_CLIENT_STATUS phyStat;
    DRV_ETHPHY_RESULT phyRes;
    const DRV_ETHPHY_OBJECT_BASE* pPhyBase;   
    TCPIP_ETH_PAUSE_TYPE        pauseType;
    DRV_ETHPHY_CONFIG_FLAGS phyCfgFlags = (DRV_ETHPHY_CONFIG_FLAGS)0;

    pPhyBase =  pMacD->mData.macConfig.pPhyBase;
    phyStat = pPhyBase->phy_ClientStatus( pMacD->mData.hPhyClient );
    if(phyStat == DRV_ETHPHY_CLIENT_STATUS_BUSY)
    {   // not done yet
        return;
    }

    phyRes = pPhyBase->phy_ClientOperationResult(pMacD->mData.hPhyClient);
    if(phyRes != DRV_ETHPHY_RES_OK)
    {   // some error occurred
        F_DRV_ETHMAC_LinkStateDown(pMacD);
        return;
    }

    
    if( (pMacD->mData.negResult.linkStatus & DRV_ETHPHY_LINK_ST_UP) != 0 )
    {   // negotiation succeeded; properly update the MAC
        (void)(pPhyBase->phy_HWConfigFlagsGet(pMacD->mData.hPhyClient, &phyCfgFlags));
        pMacD->mData.negResult.linkFlags |= (phyCfgFlags & DRV_ETHPHY_CFG_RMII) ? TCPIP_ETH_OPEN_RMII : TCPIP_ETH_OPEN_MII ;
        pauseType = pMacD->mData.negResult.pauseType;
        DRV_ETHMAC_LibMACOpen(pMacD, pMacD->mData.negResult.linkFlags, pauseType);

        pMacD->mData.macFlags.linkPrev = 1;
        pMacD->mData.linkCheckState = DRV_ETHMAC_LINK_CHECK_START_LINK;
    }
    else
    {
        F_DRV_ETHMAC_LinkStateDown(pMacD);
    }
}

/**************************
 * Filter functions
 ***********************************************/
TCPIP_MAC_RES DRV_ETHMAC_PIC32MACRxFilterHashTableEntrySet(DRV_HANDLE hMac, const TCPIP_MAC_ADDR* DestMACAddr)
{
    volatile uint32_t*    pHTSet;
    uint8_t                   hVal;
    uint32_t                  i, j;
    TCPIP_MAC_ADDR            destAddr;
    uint8_t                   nullMACAddr[6] =   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


    DRV_ETHMAC_INSTANCE_DCPT* pMacD = PIC32HandleToMacInst(hMac);
    if(pMacD == NULL)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

    DRV_ETHERNET_REGISTERS*   ethId = pMacD->mData.pEthReg;

    union
    {
        unsigned int Val;
        uint8_t     v[4];
        struct __attribute__((packed))
        {
            unsigned int b0_23: 24;

            unsigned int b24:1;
            unsigned int b25:1;
            unsigned int b26:1;
            unsigned int b27:1;
            unsigned int b28:1;
            unsigned int b29:1;
            unsigned int b30:1;
            unsigned int b31:1;
        } bits;
    } crc32_val;

    uint8_t macByte;

    crc32_val.Val = 0xFFFFFFFFU;



    // Clear the Hash Table bits and disable the Hash Table Filter if a special 
    // 00-00-00-00-00-00 destination MAC address is provided.
    if( DestMACAddr == NULL || (memcmp(DestMACAddr->v, nullMACAddr, sizeof(nullMACAddr))==0) )
    {
        // Disable the Hash Table receive filter and clear the hash table
        DRV_ETH_RxFiltersClr(ethId, DRV_ETH_FILT_HTBL_ACCEPT);
        DRV_ETH_RxFiltersHTSet(ethId, 0);
        return TCPIP_MAC_RES_OK;
    }


    // Calculate a CRC-32 over the 6 byte MAC address 
    // using polynomial 0x4C11DB7
    (void) memcpy(destAddr.v, DestMACAddr->v, sizeof(destAddr));
    for(i = 0; i < sizeof(TCPIP_MAC_ADDR); i++)
    {
        uint8_t  crcnext;

        // shift in 8 bits
        for(j = 0; j < 8U; j++)
        {
            crcnext = 0;
            if((crc32_val.v[3] & 0x80U) != 0U)
            {
                crcnext = 1;
            }
            macByte = destAddr.v[i];
            crcnext ^= macByte & 0x01U;

            crc32_val.Val <<= 1;
            if(crcnext != 0U)
            {
                crc32_val.Val ^= 0x4C11DB7U;
            }
            // next bit
            destAddr.v[i] >>= 1;
        }
    }

    // CRC-32 calculated, now extract bits 28:23
    // Bit 28 defines what HT register is affected: ETHHT0 or ETHHT1
    // Bits 27:23 define the bit offset within the ETHHT register
    pHTSet = ((crc32_val.bits.b28) != 0U)? &ETHHT1SET : &ETHHT0SET;
    hVal = (uint8_t)((crc32_val.Val >> 23)&0x1fU);
    *pHTSet = 1UL << hVal;

    // Enable that the Hash Table receive filter
    DRV_ETH_RxFiltersSet(ethId, DRV_ETH_FILT_HTBL_ACCEPT);

    return TCPIP_MAC_RES_OK;

}

bool DRV_ETHMAC_PIC32MACPowerMode(DRV_HANDLE hMac, TCPIP_MAC_POWER_MODE pwrMode)
{   // not supported
    return true;
}


// adds buffers to the internal hardware RX queue.
// returns the number of RX allocated/scheduled buffers
static uint32_t F_DRV_ETHMAC_AddRxBuffers(DRV_ETHMAC_INSTANCE_DCPT* pMacD, uint32_t nBuffs, DRV_ETHMAC_ADDBUFF_FLAGS buffFlags)
{
    uint32_t ix;
    TCPIP_MAC_PACKET*   pRxPkt;
    TCPIP_MAC_DATA_SEGMENT* pRSeg;
    DRV_ETHMAC_RESULT ethRes;
    bool    stickyBuff, setRxSize, synchLock;


    stickyBuff = ((uint32_t)buffFlags & (uint32_t)DRV_ETHMAC_ADDBUFF_FLAG_STICKY) != 0U; 
    setRxSize = ((uint32_t)buffFlags & (uint32_t)DRV_ETHMAC_ADDBUFF_FLAG_SETSIZE) != 0U; 
    synchLock = ((uint32_t)buffFlags & (uint32_t)DRV_ETHMAC_ADDBUFF_FLAG_RX_LOCK) != 0U; 

    // allocate the RX buffers
    pRxPkt = NULL;
    for(ix=0; ix < nBuffs; ix++)
    {   // the rxBuffSize is viewed as total packet size, including the ETH frame
        // the ETH frame header is added by the packet allocation
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)
        pRxPkt = (*pMacD->mData.pktAllocFDbg)(sizeof(*pRxPkt), pMacD->mData.macConfig.rxBuffSize - sizeof(TCPIP_MAC_ETHERNET_HEADER), 0, TCPIP_THIS_MODULE_ID);
#else
        pRxPkt = (*pMacD->mData.pktAllocF)(sizeof(*pRxPkt), pMacD->mData.macConfig.rxBuffSize - sizeof(TCPIP_MAC_ETHERNET_HEADER), 0);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)

        if(pRxPkt == NULL)
        {   // failed
            break;
        }
        // save packet info
        pRSeg = pRxPkt->pDSeg;
        if(stickyBuff)
        {
            pRSeg->segFlags |= (uint16_t)TCPIP_MAC_SEG_FLAG_RX_STICKY;
        }

        // set the packet acknowledgement
        pRxPkt->ackFunc = &F_MacRxPacketAck;
        pRxPkt->ackParam = pMacD;

        if(setRxSize)
        {
            pMacD->mData.macConfig.rxBuffSize = (pRxPkt->pDSeg->segSize / 16U) * 16U;   // set actual RX size
            (void) DRV_ETH_RxSetBufferSize(pMacD->mData.pEthReg, (int32_t)pMacD->mData.macConfig.rxBuffSize);
            setRxSize = false;  // just once
        }

        pRxPkt->next = NULL;
        if(synchLock)
        {
            F_DRV_ETHMAC_RxLock(pMacD);
        }
        ethRes = DRV_ETHMAC_LibRxBuffersAppend(pMacD, (void*)&pRxPkt->pDSeg->segLoad, 1, stickyBuff ? DRV_ETHMAC_BUFF_FLAG_RX_STICKY : (DRV_ETHMAC_BUFF_FLAGS)0);
        if(synchLock)
        {
            F_DRV_ETHMAC_RxUnlock(pMacD);
        }
        if(ethRes != DRV_ETHMAC_RES_OK)
        {   // failed; free
            // return it to the packet pool
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)
            (*pMacD->mData.pktFreeFDbg)(pRxPkt, TCPIP_THIS_MODULE_ID);
#else
            (*pMacD->mData.pktFreeF)(pRxPkt);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)

            break;
        }




    }


    return ix;
}



TCPIP_MAC_RES DRV_ETHMAC_PIC32MACProcess(DRV_HANDLE hMac)
{
    uint32_t rxLowThreshold;
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = PIC32HandleToMacInst(hMac);

    if(pMacD == NULL)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

    F_MACTxAcknowledgeEth(pMacD);

    F_MacTxPendingPackets(pMacD, NULL);

    // replenish RX buffers
    rxLowThreshold = (uint32_t)pMacD->mData.macConfig.rxLowThreshold;
    if(rxLowThreshold != 0U)
    {
        uint32_t nRxScheduled = 0; // buffers currently RX scheduled
        uint32_t rxLowFill;      // new buffers to add

        F_DRV_ETHMAC_RxLock(pMacD);
        (void) DRV_ETHMAC_LibRxScheduledBuffersGet(pMacD, &nRxScheduled);
        F_DRV_ETHMAC_RxUnlock(pMacD);

        if(nRxScheduled <= rxLowThreshold)
        {
            rxLowFill = (uint32_t)pMacD->mData.macConfig.rxLowFill; 
            if(rxLowFill == 0U)
            {   // gap fill
                rxLowFill = rxLowThreshold - nRxScheduled;
            }

            if(rxLowFill != 0U)
            {
                (void) F_DRV_ETHMAC_AddRxBuffers(pMacD, rxLowFill, DRV_ETHMAC_ADDBUFF_FLAG_RX_LOCK);
            }
        }
    }

    return TCPIP_MAC_RES_OK;
}

TCPIP_MAC_RES DRV_ETHMAC_PIC32MACStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics)
{
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = PIC32HandleToMacInst(hMac);
    if(pMacD == NULL)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

    if(pRxStatistics != NULL)
    {
        F_DRV_ETHMAC_RxLock(pMacD);
        (void) DRV_ETHMAC_LibRxPendingBuffersGet(pMacD, &pMacD->mData.rxStat.nRxPendBuffers);
        (void) DRV_ETHMAC_LibRxScheduledBuffersGet(pMacD, &pMacD->mData.rxStat.nRxSchedBuffers);
        F_DRV_ETHMAC_RxUnlock(pMacD);
        *pRxStatistics = pMacD->mData.rxStat;
    }
    if(pTxStatistics != NULL)
    {
        F_DRV_ETHMAC_TxLock(pMacD);
        (void) DRV_ETHMAC_LibTxPendingBuffersGet(pMacD, &pMacD->mData.txStat.nTxPendBuffers);
        pMacD->mData.txStat.nTxPendBuffers += DRV_ETHMAC_SingleListCount(&pMacD->mData.TxQueue);
        F_DRV_ETHMAC_TxUnlock(pMacD);
        *pTxStatistics = pMacD->mData.txStat;
    }

    return TCPIP_MAC_RES_OK;
}

TCPIP_MAC_RES DRV_ETHMAC_PIC32MACRegisterStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntries, size_t nEntries, size_t* pHwEntries)
{
    const DRV_ETHMAC_HW_REG_DCPT*   pHwRegDcpt;
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = PIC32HandleToMacInst(hMac);
    if(pMacD == NULL)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

    size_t nHwEntries = sizeof(macPIC32INTHwRegDcpt) / sizeof(*macPIC32INTHwRegDcpt);

    if(pHwEntries != NULL)
    {
        *pHwEntries = nHwEntries; 
    }

    if(pRegEntries != NULL)
    {   // fill data
        size_t ix, regLim;

        regLim = (nEntries > nHwEntries) ? nHwEntries : nEntries;
        pHwRegDcpt = macPIC32INTHwRegDcpt;
        for(ix = 0; ix < regLim; ix++)
        {
            (void) strncpy(pRegEntries->registerName, pHwRegDcpt->regName, sizeof(pRegEntries->registerName));
            pRegEntries->registerValue = (*pHwRegDcpt->regFunc)(pMacD->mData.pEthReg); 
            pRegEntries++; 
            pHwRegDcpt++;
        }
    }

    return TCPIP_MAC_RES_OK;
}

TCPIP_MAC_RES DRV_ETHMAC_PIC32MACParametersGet(DRV_HANDLE hMac, TCPIP_MAC_PARAMETERS* pMacParams)
{
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = PIC32HandleToMacInst(hMac);
    if(pMacD == NULL)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

    if(pMacD->mData.sysStat == SYS_STATUS_READY)
    { 
        if(pMacParams != NULL)
        {
            DRV_ETH_MACGetAddress(pMacD->mData.pEthReg, pMacParams->ifPhyAddress.v); 
            pMacParams->processFlags = (TCPIP_MAC_PROCESS_FLAGS)((uint32_t)TCPIP_MAC_PROCESS_FLAG_RX | (uint32_t)TCPIP_MAC_PROCESS_FLAG_TX); 
            pMacParams->macType = TCPIP_MAC_TYPE_ETH;
            pMacParams->linkMtu = (TCPIP_MAC_LINK_MTU)M_TCPIP_EMAC_LINK_MTU;
        }

        return TCPIP_MAC_RES_OK;
    }

    return TCPIP_MAC_RES_IS_BUSY;
}

/**************************
 * local functions and helpers
 ***********************************************/

static TCPIP_MAC_RES F_MACTxPacket(DRV_ETHMAC_INSTANCE_DCPT* pMacD, TCPIP_MAC_PACKET * ptrPacket)
{
    DRV_ETHMAC_RESULT ethRes;

    // Note: the TCPIP_MAC_DATA_SEGMENT is defined to be a perfect match for DRV_ETHMAC_PKT_DCPT !!!
    ethRes = DRV_ETHMAC_LibTxSendPacket(pMacD, CF_MacDataSegtoPktDcpt(ptrPacket->pDSeg));

    if(ethRes == DRV_ETHMAC_RES_OK)
    {
        return TCPIP_MAC_RES_OK;
    }
    else if(ethRes == DRV_ETHMAC_RES_NO_DESCRIPTORS)
    {
        pMacD->mData.txStat.nTxQueueFull++;
        return TCPIP_MAC_RES_PENDING;
    }
    else
    {
        /* Do Nothing */
    }
    
    pMacD->mData.txStat.nTxErrorPackets++;
    return TCPIP_MAC_RES_PACKET_ERR;
}


// acknowledge the ETHC packets
static void F_MACTxAcknowledgeEth(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    (void) DRV_ETHMAC_LibTxAcknowledgePacket(pMacD, NULL, &F_MACTxPacketAckCallback, pMacD);
}

static void F_MACTxPacketAckCallback(void* pBuff, void* fParam)
{
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)fParam;

    // restore packet the buffer belongs to
    uint8_t* segBuff = (uint8_t*)(CF_VoidPtr2Uint(pBuff) & pMacD->mData.dataOffsetMask);
    TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = CF_Ptr8toSegGapDsc(segBuff + pMacD->mData.gapDcptOffset);
    TCPIP_MAC_PACKET* ptrPacket = pGap->segmentPktPtr;

    // acknowledge the packet
    (*pMacD->mData.pktAckF)(ptrPacket, TCPIP_MAC_PKT_ACK_TX_OK, TCPIP_THIS_MODULE_ID);
    pMacD->mData.txStat.nTxOkPackets++;
}


// transmits pending packets, if any
// if the link is down the TX queued packets are discarded
static void F_MacTxPendingPackets(DRV_ETHMAC_INSTANCE_DCPT* pMacD, TCPIP_MAC_PACKET* pPkt)
{
    TCPIP_MAC_RES     pktRes;

    TCPIP_MAC_PACKET* pNext;
    // add the packets to the TX queue
    if(pPkt != NULL)
    {
        F_DRV_ETHMAC_TxLock(pMacD);
        while (pPkt != NULL)
        {
            pNext = pPkt->next;
            pPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_QUEUED;
            DRV_ETHMAC_SingleListTailAdd(&pMacD->mData.TxQueue, CF_EthMacPkt2LstNode(pPkt));
            pPkt = pNext;
        }
        F_DRV_ETHMAC_TxUnlock(pMacD);
    }


    if((pMacD->mData.controlFlags & TCPIP_MAC_CONTROL_NO_LINK_CHECK) == 0 && pMacD->mData.macFlags.linkPrev == false)
    {   // discard the TX queues
        F_MacTxDiscardQueues(pMacD, TCPIP_MAC_PKT_ACK_LINK_DOWN, true); 
        // no need to try to schedule for TX
        return;
    }


    // transmit the pending packets
    // the F_MACTxPacket() operation is lengthy: to avoid keeping the system locked for too long
    // we make our own copy of packets to TX:
    DRV_ETHMAC_SGL_LIST txList;
    DRV_ETHMAC_SingleListInitialize(&txList);
    
    F_DRV_ETHMAC_TxLock(pMacD);
    while((pPkt = CF_LstNode2EthMacPkt(DRV_ETHMAC_SingleListHeadRemove(&pMacD->mData.TxQueue))) != NULL)
    {
        DRV_ETHMAC_SingleListTailAdd(&txList, CF_EthMacPkt2LstNode(pPkt));
    }
    F_DRV_ETHMAC_TxUnlock(pMacD);

    // now transmit the packets from the list
    while( (pPkt = CF_LstNode2EthMacPkt((txList.head))) != NULL)
    {
        pktRes = F_MACTxPacket(pMacD, pPkt);
        if(pktRes == TCPIP_MAC_RES_PENDING)
        {   // not enough room in the hw queue
            break;
        }

        // packet done
        (void)DRV_ETHMAC_SingleListHeadRemove(&txList);
    }

    // what's left in txList needs to be added back to the TxQueue
    // preferrably in the same order
    F_DRV_ETHMAC_TxLock(pMacD);
    DRV_ETHMAC_SingleListAppend(&pMacD->mData.TxQueue, &txList);
    F_DRV_ETHMAC_TxUnlock(pMacD);
}

/*********************************************************************
* Function:        void* F_MacCallocCallback( size_t nitems, size_t size, void* param )
 *
 * PreCondition:    None
 * 
 * Input:           nitems - number of items to be allocated
 *                  size   - size of each item
 *                  param  - optional parameter specified when DRV_ETHMAC_PIC32DescriptorsPoolAdd() called 
 * 
 * Output:          pointer to the allocated memory of NULL if allocation failed
 * 
 * Side Effects:    None
 * 
 * Overview:        Memory allocation callback.
 * 
 * Note:            None
 ********************************************************************/
static void* F_MacCallocCallback( size_t nitems, size_t size, void* param )
{
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)param;
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)
    return (*(TCPIP_MAC_HEAP_CallocFDbg)pMacD->mData.mac_callocF)(pMacD->mData.mac_AllocH, nitems, size, TCPIP_THIS_MODULE_ID, __LINE__);
#else
    return (*pMacD->mData.mac_callocF)(pMacD->mData.mac_AllocH, nitems, size);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)

}



#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
static void F_MACDeinit(DRV_ETHMAC_INSTANCE_DCPT* pMacD )
{
    (void)SYS_INT_SourceDisable(pMacD->mData.macIntSrc);      // stop Eth ints
    DRV_ETHMAC_LibClose(pMacD, DRV_ETHMAC_CLOSE_DEFAULT);
    SYS_INT_SourceStatusClear(pMacD->mData.macIntSrc); // clear any pending interrupt flag
    (void)DRV_ETHMAC_PIC32MACEventDeInit((DRV_HANDLE)pMacD);

    const DRV_ETHPHY_OBJECT_BASE* pPhyBase =  pMacD->mData.macConfig.pPhyBase;

    if(pMacD->mData.hPhyClient != DRV_HANDLE_INVALID)
    {
        pPhyBase->phy_Close(pMacD->mData.hPhyClient);
        pMacD->mData.hPhyClient = DRV_HANDLE_INVALID;
    }
    if(pMacD->mData.hPhySysObject != SYS_MODULE_OBJ_INVALID)
    {
        pPhyBase->phy_Deinitialize(pMacD->mData.hPhySysObject);
        pMacD->mData.hPhySysObject = SYS_MODULE_OBJ_INVALID;
    }
    F_MACCleanup(pMacD);
    pMacD->mData.sysStat = SYS_STATUS_UNINITIALIZED; 
    pMacD->mData.macFlags.val = 0;
}

static void F_MACCleanup(DRV_ETHMAC_INSTANCE_DCPT* pMacD )
{
    // TX clean up
    DRV_ETHMAC_LibDescriptorsPoolCleanUp(pMacD, DRV_ETHMAC_DCPT_TYPE_TX,  &F_MacTxFreeCallback, (void*)pMacD);
    F_MacTxDiscardQueues(pMacD, TCPIP_MAC_PKT_ACK_NET_DOWN, false); 

    // RX clean up
    DRV_ETHMAC_LibDescriptorsPoolCleanUp(pMacD, DRV_ETHMAC_DCPT_TYPE_RX,  &F_MacRxFreeCallback, (void*)pMacD);


    F_DRV_ETHMAC_RxDelete(pMacD);
    F_DRV_ETHMAC_TxDelete(pMacD);
}

/*********************************************************************
* Function:        void F_MacTxFreeCallback(  void* ptr, void* param )
 *
 * PreCondition:    None
 * 
 * Input:           ptr -  pointer to be freed
 *                  param  - optional parameter specified when DRV_ETHMAC_PIC32DescriptorsPoolCleanUp() called 
 * 
 * Output:          None
 * 
 * Side Effects:    None
 * 
 * Overview:        Memory free callback.
 * 
 * Note:            None
 ********************************************************************/
static void F_MacTxFreeCallback(  void* ptr, void* param )
{
    TCPIP_MAC_PACKET* pTxPkt;
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)param;

    uint8_t*    pTxBuff = (uint8_t*)DRV_ETHMAC_LibDescriptorGetBuffer(pMacD, ptr);
    if(pTxBuff != NULL)
    {
        // check if the payload offset is actually used for TX
        uint8_t* segBuff = (uint8_t*)((uint32_t)pTxBuff & pMacD->mData.dataOffsetMask);
        TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = CF_Ptr8toSegGapDsc(segBuff + pMacD->mData.gapDcptOffset);
        pTxPkt = pGap->segmentPktPtr;

        (*pMacD->mData.pktAckF)(pTxPkt, TCPIP_MAC_PKT_ACK_NET_DOWN, TCPIP_THIS_MODULE_ID);
    }

#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)
    (*(TCPIP_MAC_HEAP_FreeFDbg)pMacD->mData.mac_freeF)(pMacD->mData.mac_AllocH, ptr, TCPIP_THIS_MODULE_ID, __LINE__);
#else
    (void)(*pMacD->mData.mac_freeF)(pMacD->mData.mac_AllocH, ptr);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)

}

static void F_MacRxFreeCallback(  void* ptr, void* param )
{
    TCPIP_MAC_PACKET* pRxPkt;
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)param;
    
    uint8_t*    pRxBuff = (uint8_t*)DRV_ETHMAC_LibDescriptorGetBuffer(pMacD, ptr);
    if(pRxBuff != NULL)
    {
        uint8_t* segBuff = (uint8_t*)((uint32_t)pRxBuff & pMacD->mData.dataOffsetMask);
        TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = CF_Ptr8toSegGapDsc(segBuff + pMacD->mData.gapDcptOffset);
        pRxPkt = pGap->segmentPktPtr;
        pRxPkt->pDSeg->next = NULL;     // break the ETH MAC run time chaining
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)
        (*pMacD->mData.pktFreeFDbg)(pRxPkt, TCPIP_THIS_MODULE_ID);
#else
        (*pMacD->mData.pktFreeF)(pRxPkt);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)
    }

#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)
    (*(TCPIP_MAC_HEAP_FreeFDbg)pMacD->mData.mac_freeF)(pMacD->mData.mac_AllocH, ptr, TCPIP_THIS_MODULE_ID, __LINE__);
#else
    (void)(*pMacD->mData.mac_freeF)(pMacD->mData.mac_AllocH, ptr);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)

}

#else
static void F_MACDeinit(DRV_ETHMAC_INSTANCE_DCPT* pMacD )
{
    SYS_INT_SourceDisable(pMacD->mData.macIntSrc);      // stop Eth ints

    const DRV_ETHPHY_OBJECT_BASE* pPhyBase =  pMacD->mData.macConfig.pPhyBase;

    if(pMacD->mData.hPhyClient != DRV_HANDLE_INVALID)
    {
        pPhyBase->phy_Close(pMacD->mData.hPhyClient);
        pMacD->mData.hPhyClient = DRV_HANDLE_INVALID;
    }
    pMacD->mData.hPhySysObject = SYS_MODULE_OBJ_INVALID;
    pMacD->mData.sysStat = SYS_STATUS_UNINITIALIZED; 
    pMacD->mData.macFlags.val = 0;
}
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)

static void F_MacTxDiscardQueues(DRV_ETHMAC_INSTANCE_DCPT* pMacD, TCPIP_MAC_PKT_ACK_RES ackRes, bool synch)
{
    TCPIP_MAC_PACKET* pPkt;
    DRV_ETHMAC_SGL_LIST discardTx;

    DRV_ETHMAC_SingleListInitialize(&discardTx);

    if(synch)
    {
        F_DRV_ETHMAC_TxLock(pMacD);
    }

    while( (pPkt = CF_LstNode2EthMacPkt(DRV_ETHMAC_SingleListHeadRemove(&pMacD->mData.TxQueue))) != NULL)
    {   // acknowledge the packet
        DRV_ETHMAC_SingleListTailAdd(&discardTx, CF_EthMacPkt2LstNode(pPkt));
    }

    if(synch)
    {
        F_DRV_ETHMAC_TxUnlock(pMacD);
    }

    // acknowledge the packets
    while( (pPkt = CF_LstNode2EthMacPkt(DRV_ETHMAC_SingleListHeadRemove(&discardTx))) != NULL)
    {
        (*pMacD->mData.pktAckF)(pPkt, ackRes, TCPIP_THIS_MODULE_ID);
    }
}

// a RX packet has been done with 
static void F_MacRxPacketAck(TCPIP_MAC_PACKET* pRxPkt,  const void* param)
{
    TCPIP_MAC_PACKET* pCurrPkt;
    TCPIP_MAC_DATA_SEGMENT* pSeg, *pNSeg;

    DRV_ETHMAC_INSTANCE_DCPT* pMacD = CF_VoidPtr2EthDcpt(param);

    bool isMacDead = pMacD->mData.macFlags.macInit == 0; // if we're dead and gone
    
    pSeg = pRxPkt->pDSeg;
    while(pSeg != NULL) 
    {
        pNSeg = pSeg->next;
        pSeg->next = NULL;     // break the ETH MAC run time chaining
        if(!isMacDead)
        {   // acknowledge the ETHC
            // No further acknowledge/processing needed from MAC
            F_DRV_ETHMAC_RxLock(pMacD);
            (void)DRV_ETHMAC_LibRxAcknowledgeBuffer(pMacD, pSeg->segLoad);
            F_DRV_ETHMAC_RxUnlock(pMacD);
        }

        // extract packet the segment belongs to
        TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = CF_Ptr8toSegGapDsc(pSeg->segBuffer + pMacD->mData.gapDcptOffset);
        pCurrPkt = pGap->segmentPktPtr;

        if(isMacDead || (pSeg->segFlags & TCPIP_MAC_SEG_FLAG_RX_STICKY) == 0)
        {   // free the packet this segment belongs to
            // Note: smart alloc could be implemented: reuse the dynamic buffers if below the threshold!
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)
            (*pMacD->mData.pktFreeFDbg)(pCurrPkt, TCPIP_THIS_MODULE_ID);
#else
            (*pMacD->mData.pktFreeF)(pCurrPkt);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)
        }
        else
        {
            pCurrPkt->pktFlags &= ~TCPIP_MAC_PKT_FLAG_QUEUED;
        }
        pSeg = pNSeg;
    }

}


/*************************
 * local data
 *************************/


static DRV_HANDLE         hEventMac;     // the MAC we belong to


/*********************************
 *  local proto 
 ******************************************/

/****************************************************************************
 * Function:        XtlEventsTcp2Eth
 *
 * PreCondition:    None
 *
 * Input:           tcpEv - the TcpIp events to be translated
 *
 * Output:          Ethernet events
 *
 * Side Effects:    None
 *
 * Overview:        This function translates from  events to Ethernet events
 *
 * Note:            - The translation could be done in a more systematic way
 *                    using a (few) LUT(s).
 *                    For now this direct approach is taken.
 *                  - The CONN_LOST/CONN_ESTABLISHED events do NOT map to Ethernet events! 
 *                    The current translation function assumes no overlap between a CONN event
 *                    and an Ethernet one!
 *                    By this translation the CONN_LOST/CONN_ESTABLISHED event is lost!
 ******************************************************************************/
/*static __inline__*/static  DRV_ETH_EVENTS /*__attribute__((always_inline))*/ XtlEventsTcp2Eth(TCPIP_MAC_EVENT tcpEv)
{
    uint32_t  eEvents;

    eEvents =  (uint32_t)(((uint32_t)tcpEv & ((uint32_t)TCPIP_MAC_EV_TX_BUSERR))<<4);
    eEvents |= (uint32_t)(((uint32_t)tcpEv & ((uint32_t)TCPIP_MAC_EV_TX_ABORT))>>7);
    eEvents |= (uint32_t)(((uint32_t)tcpEv & ((uint32_t)TCPIP_MAC_EV_TX_DONE))>>5);
    eEvents |= (uint32_t)(((uint32_t)tcpEv & ((uint32_t)TCPIP_MAC_EV_RX_BUSERR|TCPIP_MAC_EV_RX_PKTPEND))<<6);
    eEvents |= (uint32_t)(((uint32_t)tcpEv & (uint32_t)(TCPIP_MAC_EV_RX_EWMARK|TCPIP_MAC_EV_RX_FWMARK|TCPIP_MAC_EV_RX_DONE))<<3);
    eEvents |= (uint32_t)(((uint32_t)tcpEv & ((uint32_t)TCPIP_MAC_EV_RX_ACT))<<2);
    eEvents |= (uint32_t)(((uint32_t)tcpEv & (uint32_t)(TCPIP_MAC_EV_RX_BUFNA|TCPIP_MAC_EV_RX_OVFLOW))>>1);
    
    return (DRV_ETH_EVENTS)eEvents;
}

/****************************************************************************
 * Function:        XtlEventsEth2Tcp
 *
 * PreCondition:    None
 *
 * Input:           eEvents - the Eth events to be translated
 *
 * Output:          TcpIp events
 *
 * Side Effects:    None
 *
 * Overview:        This function translates from Ethernet events to TcpIp events
 *
 * Note:            - The translation could be done in a more systematic way
 *                    using a (few) LUT(s).
 *                    For now this direct approach is taken.
 *                  - The CONN_LOST/CONN_ESTABLISHED events do NOT map to Ethernet events! 
 *                    The current translation function assumes no overlap between a CONN event
 *                    and an Ethernet one!
 *                    By this translation a CONN_LOST/CONN_ESTABLISHED event cannot be generated!
 ******************************************************************************/
/*static __inline__*/static  TCPIP_MAC_EVENT /*__attribute__((always_inline))*/ XtlEventsEth2Tcp(DRV_ETH_EVENTS eEvents)
{
    uint32_t tcpEv;

    tcpEv =  (uint32_t)(((uint32_t)eEvents & ((uint32_t)_ETHIRQ_TXBUSE_MASK))>>4);
    tcpEv |= (uint32_t)(((uint32_t)eEvents & ((uint32_t)_ETHIRQ_TXABORT_MASK))<<7);
    tcpEv |= (uint32_t)(((uint32_t)eEvents & ((uint32_t)_ETHIRQ_TXDONE_MASK))<<5);
    tcpEv |= (uint32_t)(((uint32_t)eEvents & ((uint32_t)_ETHIRQ_RXBUSE_MASK|(uint32_t)_ETHIRQ_PKTPEND_MASK))>>6);
    tcpEv |= (uint32_t)(((uint32_t)eEvents & (((uint32_t)_ETHIRQ_EWMARK_MASK|(uint32_t)_ETHIRQ_FWMARK_MASK|(uint32_t)_ETHIRQ_RXDONE_MASK)))>>3);
    tcpEv |= (uint32_t)(((uint32_t)eEvents & ((uint32_t)_ETHIRQ_RXACT_MASK))>>2);
    tcpEv |= (uint32_t)(((uint32_t)eEvents & ((uint32_t)_ETHIRQ_RXBUFNA_MASK|(uint32_t)_ETHIRQ_RXOVFLW_MASK))<<1);
    
    return (TCPIP_MAC_EVENT)tcpEv;
}

/*********************************
 *  implementation
 ******************************************/

/*******************************************************************************
  Function:
    TCPIP_MAC_RES DRV_ETHMAC_PIC32MACEventInit(DRV_HANDLE hMac, TCPIP_MAC_EventF eventF, const void* eventParam)

  Summary:
    Initializes the ethernet event notification.

  Description:
     This function initializes the ethernet event notification.
     It performs any resource allocation that may be needed.

  Precondition:
     None.

  Parameters:
    hMac        - parameter identifying the intended MAC  
    eventF      - notification function
    eventParam  - notification parameter
    
  Returns:
    TCPIP_MAC_RES_OK  if initialization succeeded,
    an error code otherwise

  Example:
    <code>
    DRV_ETHMAC_PIC32MACEventInit( hMac, eventF, param);
    </code>

  Remarks:

    Not multi-threaded safe.
*****************************************************************************/
static TCPIP_MAC_RES DRV_ETHMAC_PIC32MACEventInit(DRV_HANDLE hMac, TCPIP_MAC_EventF eventF, const void* eventParam)
{
    DRV_ETHMAC_EVENT_DCPT*    pDcpt; 
    DRV_ETHMAC_INSTANCE_DCPT*     pMacD;

    if(hEventMac == hMac)
    {   // re-init the same MAC
        return TCPIP_MAC_RES_OK;
    }
    else if(hEventMac != 0U)
    {   // already opened
        return TCPIP_MAC_RES_IS_BUSY;
    }
    else
    {
        /* Do Nothing */
    }

    pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)hMac;
    (void) SYS_INT_SourceDisable(pMacD->mData.macIntSrc);      // stop Eth ints
    SYS_INT_SourceStatusClear(pMacD->mData.macIntSrc);

    pDcpt = &pMacD->mData.pic32m_ev_group_dcpt;
    pDcpt->TcpEnabledEvents = TCPIP_MAC_EV_NONE;
    pDcpt->TcpPendingEvents = TCPIP_MAC_EV_NONE;
    pDcpt->EthEnabledEvents = (DRV_ETH_EVENTS)0U;
    pDcpt->EthPendingEvents = (DRV_ETH_EVENTS)0U;
    pDcpt->TcpNotifyFnc = eventF;
    pDcpt->TcpNotifyParam = eventParam;

    hEventMac = hMac;    
    return TCPIP_MAC_RES_OK;
}

/*******************************************************************************
  Function:
    TCPIP_MAC_RES    DRV_ETHMAC_PIC32MACEventDeInit(DRV_HANDLE hMac )

  Summary:
    De-initializes the ethernet event notification.

  Description:
     This function de-initializes the ethernet event notification.
     It performs any resource clean-up that may be needed.

  Precondition:
     None.

  Parameters:
    hMac      - parameter identifying the intended MAC  
    
  Returns:
    TCPIP_MAC_RES_OK  always

  Example:
    <code>
    DRV_ETHMAC_PIC32MACEventDeInit( hMac );
    </code>

  Remarks:

    Not multi-threaded safe.
*****************************************************************************/
#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
static TCPIP_MAC_RES DRV_ETHMAC_PIC32MACEventDeInit(DRV_HANDLE hMac)
{
    DRV_ETHMAC_EVENT_DCPT* pDcpt; 
    DRV_ETHMAC_INSTANCE_DCPT*  pMacD;

    if(hEventMac != hMac)
    {   // not us?
        return TCPIP_MAC_RES_IS_BUSY;
    }

    pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)hMac;
    (void)SYS_INT_SourceDisable(pMacD->mData.macIntSrc);      // stop Eth ints
    SYS_INT_SourceStatusClear(pMacD->mData.macIntSrc);

    pDcpt = &pMacD->mData.pic32m_ev_group_dcpt;
    pDcpt->TcpNotifyFnc = NULL;
    pDcpt->TcpEnabledEvents = TCPIP_MAC_EV_NONE;
    pDcpt->TcpPendingEvents = TCPIP_MAC_EV_NONE;
    pDcpt->EthEnabledEvents = (DRV_ETH_EVENTS)0U; 
    pDcpt->EthPendingEvents = (DRV_ETH_EVENTS)0U;

    hEventMac = 0;
    return TCPIP_MAC_RES_OK;
}
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)


/*******************************************************************************
  Function:
    bool    DRV_ETHMAC_PIC32MACEventMaskSet(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvMask, bool enable)

  Summary:
    Enables the MAC events.

  Description:
     This function sets the enabled events.
     Multiple events can be orr-ed together.
     All events that are set will be added to the notification process. The old events will be disabled.
     The stack (or stack user) has to catch the events that are notified and process them:
         - The stack should process the TCPIP_MAC_EV_RX_PKTPEND/TCPIP_MAC_EV_RX_DONE, TCPIP_MAC_EV_TX_DONE transfer events
         - Process the specific condition and acknowledge them calling DRV_ETHMAC_PIC32MACEventAcknowledge() so that they can be re-enabled.

  Precondition:
   DRV_ETHMAC_PIC32MACEventInit should have been called.

  Parameters:
    hMac      - parameter identifying the intended MAC  
    macEvMask - events the user of the stack wants to add/delete for notification
    enable    - if true, the events will  be enabled, else disabled
    
  Returns:
    true  if operation succeeded,
    an error code otherwise

  Example:
    <code>
    DRV_ETHMAC_PIC32MACEventMaskSet( hMac, TCPIP_MAC_EV_RX_OVFLOW | TCPIP_MAC_EV_RX_BUFNA, true );
    </code>

  Remarks:
    The event notification system enables the user of the TCPIP stack to call into the stack
    for processing only when there are relevant events rather than being forced to periodically call
    from within a loop.
    
    If the notification events are nill the interrupt processing will be disabled.
    Otherwise the event notification will be enabled and the interrupts relating to the requested events will be enabled.
    
    Note that once an event has been caught by the stack ISR (and reported if a notification handler is in place)
    it will be disabled until the DRV_ETHMAC_PIC32MACEventAcknowledge() is called.

*****************************************************************************/
bool DRV_ETHMAC_PIC32MACEventMaskSet(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvMask, bool enable)
{
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = PIC32HandleToMacInst(hMac);
    if(pMacD == NULL)
    {
        return false;
    }

    DRV_ETHMAC_EVENT_DCPT*  pDcpt = &pMacD->mData.pic32m_ev_group_dcpt; 
    DRV_ETHERNET_REGISTERS* ethId = pMacD->mData.pEthReg;

    if(enable)
    {
        DRV_ETH_EVENTS  ethSetEvents;
        ethSetEvents = XtlEventsTcp2Eth(macEvMask);

        if(pDcpt->TcpEnabledEvents != 0U)
        {   // already have some active     
            (void) SYS_INT_SourceDisable(pMacD->mData.macIntSrc);      // stop ints for a while
        }

        pDcpt->TcpEnabledEvents |= macEvMask;        // add more
        pDcpt->EthEnabledEvents |= ethSetEvents;  

        if(pDcpt->TcpEnabledEvents != 0)
        {
            ethSetEvents &= ~pDcpt->EthPendingEvents;     // keep just the new un-ack events
            DRV_ETH_EventsClr(ethId, ethSetEvents );   // clear the old pending ones
            DRV_ETH_EventsEnableSet(ethId, ethSetEvents );                 // enable the new un-ack ones!

            SYS_INT_SourceEnable(pMacD->mData.macIntSrc);       // enable
        }
    }
    else
    {   // disable some events
        DRV_ETH_EVENTS  ethClrEvents;
        bool         ethILev = false;

        macEvMask &= pDcpt->TcpEnabledEvents;                  // keep just the enabled ones
        ethClrEvents = XtlEventsTcp2Eth(macEvMask);

        if(pDcpt->TcpEnabledEvents != 0U)
        {   // already have some active     
            ethILev = SYS_INT_SourceDisable(pMacD->mData.macIntSrc);      // stop ints for a while
        }

        pDcpt->TcpEnabledEvents &= ~macEvMask;     // clear some of them
        pDcpt->EthEnabledEvents &= ~ethClrEvents;

        pDcpt->TcpPendingEvents &= ~macEvMask;     // remove them from un-ack list
        pDcpt->EthPendingEvents &= ~ethClrEvents;

        DRV_ETH_EventsEnableClr(ethId, ethClrEvents);   // no longer enabled
        DRV_ETH_EventsClr(ethId, ethClrEvents);         // clear the pending ones

        if(pDcpt->TcpEnabledEvents != 0U)
        {
            SYS_INT_SourceRestore(pMacD->mData.macIntSrc, (bool)ethILev);   // re-enable
        }
    }

    return true;
}


/*******************************************************************************
  Function:
    bool    DRV_ETHMAC_PIC32MACEventAcknowledge(DRV_HANDLE hMac, TCPIP_MAC_EVENT tcpAckEv)

  Summary:
    Acknowledges and re-enables processed events.

  Description:
    This function acknowledges and re-enables processed events.
    Multiple events can be orr-ed together as they are processed together.
    The events acknowledged by this function should be the events that have been retrieved from the stack
    by calling DRV_ETHMAC_PIC32MACEventPendingGet() or have been passed to the user by the stack using the notification handler
    (PIC32MACEventSetNotifyHandler()) and have been processed and have to be re-enabled.


  Precondition:
   DRV_ETHMAC_PIC32MACEventInit should have been called.

  Parameters:
    hMac      - parameter identifying the intended MAC  
    macEvents - the events that the user processed and need to be re-enabled
    
  Returns:
    true if events acknowledged
    false if no events to be acknowledged

  Example:
    <code>
    DRV_ETHMAC_PIC32MACEventAcknowledge( hMac, stackNewEvents );
    </code>

  Remarks:   
    All events should be acknowledged, in order to be re-enabled.

    Some events are fatal errors and should not be acknowledged (TCPIP_MAC_EV_RX_BUSERR, TCPIP_MAC_EV_TX_BUSERR).
    Stack re-initialization is needed under such circumstances.

    Some events are just system/application behavior and they are intended only as simple info (TCPIP_MAC_EV_RX_OVFLOW,
    TCPIP_MAC_EV_RX_BUFNA, TCPIP_MAC_EV_TX_ABORT, TCPIP_MAC_EV_RX_ACT).

    The TCPIP_MAC_EV_RX_FWMARK and TCPIP_MAC_EV_RX_EWMARK events are part of the normal flow control operation (if auto flow control was enabled).
    They should be enabled alternatively, if needed.

    The events are persistent. They shouldn't be re-enabled unless they have been processed and the condition that generated them was removed.
    Re-enabling them immediately without proper processing will have dramatic effects on system performance.

*****************************************************************************/
bool DRV_ETHMAC_PIC32MACEventAcknowledge(DRV_HANDLE hMac, TCPIP_MAC_EVENT tcpAckEv)
{

    DRV_ETHMAC_INSTANCE_DCPT* pMacD = PIC32HandleToMacInst(hMac);
    if(pMacD == NULL)
    {
        return false;
    }

    bool                   ethILev;
    DRV_ETHMAC_EVENT_DCPT*  pDcpt = &pMacD->mData.pic32m_ev_group_dcpt; 
    DRV_ETHERNET_REGISTERS* ethId = pMacD->mData.pEthReg;

    if(pDcpt->TcpEnabledEvents != 0)
    {   // already have some active     
        DRV_ETH_EVENTS  ethAckEv;

        ethAckEv=XtlEventsTcp2Eth(tcpAckEv);

        ethILev = SYS_INT_SourceDisable(pMacD->mData.macIntSrc);  // stop ints for a while

        pDcpt->TcpPendingEvents &= ~tcpAckEv;         // no longer pending

        pDcpt->EthPendingEvents &= ~ethAckEv;         // no longer pending

        DRV_ETH_EventsClr(ethId, ethAckEv);                 // clear the old pending ones
        DRV_ETH_EventsEnableSet(ethId, ethAckEv);           // re-enable the ack ones
        

        SYS_INT_SourceRestore(pMacD->mData.macIntSrc, ethILev);   // re-enable
        return true;
    }

    return false;
}


/*******************************************************************************
  Function:
    TCPIP_MAC_EVENT DRV_ETHMAC_PIC32MACEventPendingGet(DRV_HANDLE hMac)

  Summary:
    Returns the currently pending events.

  Description:
    This function returns the currently pending events belonging to a group.
    Multiple events can be orr-ed together as they accumulate.
    The stack should be called for processing whenever a stack managed event (TCPIP_MAC_EV_RX_PKTPEND, TCPIP_MAC_EV_TX_DONE) is present.
    The other, non critical events, may not be managed by the stack and passed to an user.
    They will have to be eventually acknowledged if re-enabling is needed.

  Precondition:
   hMac      - parameter identifying the intended MAC  
   DRV_ETHMAC_PIC32MACEventInit should have been called.

  Parameters:
    
  Returns:
    The currently stack pending events.

  Example:
    <code>
    TCPIP_MAC_EVENT currEvents = DRV_ETHMAC_PIC32MACEventPendingGet( hMac);
    </code>

  Remarks:   
    This is the preferred method to get the current pending MAC events.
    The stack maintains a proper image of the events from their occurrence to their acknowledgement.
    
    Even with a notification handler in place it's better to use this function to get the current pending events
    rather than using the events passed by the notification handler which could be stale.
    
    The events are persistent. They shouldn't be re-enabled unless they have been processed and
    the condition that generated them was removed.
    Re-enabling them immediately without proper processing will have dramatic effects on system performance.

    The returned value is just a momentary value. The pending events can change any time.
*****************************************************************************/
TCPIP_MAC_EVENT DRV_ETHMAC_PIC32MACEventPendingGet(DRV_HANDLE hMac)
{
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = PIC32HandleToMacInst(hMac);
    if(pMacD != NULL)
    {
        return pMacD->mData.pic32m_ev_group_dcpt.TcpPendingEvents;
    }

    return TCPIP_MAC_EV_NONE;
}


/**************************************
 * local functions
 ****************************************/

static DRV_ETH_RX_FILTERS F_DRV_ETHMAC_MacToEthFilter(TCPIP_MAC_RX_FILTER_TYPE macFilter)
{
    int ix;
    DRV_ETH_RX_FILTERS rxFilter = (DRV_ETH_RX_FILTERS)0U;

    for(ix = 0; ix < sizeof(DRV_ETHMAC_FiltConvTbl) / sizeof(*DRV_ETHMAC_FiltConvTbl); ix++)
    {
        if( ((uint32_t)macFilter & (1 << ix)) != 0)
        {
            rxFilter |= DRV_ETHMAC_FiltConvTbl[ix];
        }
    }

    return rxFilter;
}


void ETHERNET_InterruptHandler(void)
{
    DRV_ETHMAC_Tasks_ISR((SYS_MODULE_OBJ)0);
}

/****************************************************************************
 * Function:        DRV_ETHMAC_Tasks_ISR
 *
 * PreCondition:    DRV_ETHMAC_PIC32MACEventInit, DRV_ETHMAC_PIC32MACEventMaskSet should have been called.
 *
 * Input:           macIndex - PIC32 MAC object index
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function processes the Ethernet interrupts and reports the events back to the user.
 *
 * Note:            None
 ******************************************************************************/
void DRV_ETHMAC_Tasks_ISR( SYS_MODULE_OBJ macIndex )
{
    DRV_ETH_EVENTS  currEthEvents;
    uint32_t    currGroupEvents;
    DRV_ETHMAC_EVENT_DCPT* pDcpt;
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = &s_pic32_emb_mac_dcpt[macIndex];
    DRV_ETHERNET_REGISTERS* ethId = pMacD->mData.pEthReg;

    currEthEvents = DRV_ETH_EventsGet(ethId);


    // process interrupts
    pDcpt = &pMacD->mData.pic32m_ev_group_dcpt;
    currGroupEvents = ((uint32_t)currEthEvents & (uint32_t)pDcpt->EthEnabledEvents);     //  keep just the relevant ones

    if(currGroupEvents != 0U)
    {
        pDcpt->EthPendingEvents |= (DRV_ETH_EVENTS)currGroupEvents;                    // add the new events
        pDcpt->TcpPendingEvents |= XtlEventsEth2Tcp((DRV_ETH_EVENTS)currGroupEvents);

        DRV_ETH_EventsEnableClr(ethId, (DRV_ETH_EVENTS)currGroupEvents);         // these will get reported; disable them until ack is received back
        DRV_ETH_EventsClr(ethId, (DRV_ETH_EVENTS)currGroupEvents);               // acknowledge the ETHC

        if((pDcpt->TcpNotifyFnc) != NULL)
        {
            (*pDcpt->TcpNotifyFnc)(pDcpt->TcpPendingEvents, pDcpt->TcpNotifyParam);     // let the user know
        }
    }
    
    SYS_INT_SourceStatusClear(pMacD->mData.macIntSrc);         // acknowledge the int Controller
}

static uint32_t F_DRV_ETHMAC_GetFrmTxOk(DRV_ETHERNET_REGISTERS* ethId)
{
    return DRV_ETH_FramesTxdOkCountGet(ethId);
}

static uint32_t F_DRV_ETHMAC_GetFrmRxOk(DRV_ETHERNET_REGISTERS* ethId)
{
    return DRV_ETH_FramesRxdOkCountGet(ethId);
}

static uint32_t F_DRV_ETHMAC_GetFrmPktCount(DRV_ETHERNET_REGISTERS* ethId)
{
    return DRV_ETH_RxPacketCountGet(ethId);
}

static uint32_t F_DRV_ETHMAC_GetFrmRxOvflow(DRV_ETHERNET_REGISTERS* ethId)
{
    return DRV_ETH_RxOverflowCountGet(ethId);
}

static uint32_t F_DRV_ETHMAC_GetFrmFcsError(DRV_ETHERNET_REGISTERS* ethId)
{
    return DRV_ETH_FCSErrorCountGet(ethId);
}

static uint32_t F_DRV_ETHMAC_GetAlignError(DRV_ETHERNET_REGISTERS* ethId)
{
    return DRV_ETH_AlignErrorCountGet(ethId);
}

static uint32_t F_DRV_ETHMAC_GetSCollisionCount(DRV_ETHERNET_REGISTERS* ethId)
{
    return DRV_ETH_SingleCollisionCountGet(ethId);
}

static uint32_t F_DRV_ETHMAC_GetMCollisionCount(DRV_ETHERNET_REGISTERS* ethId)
{
    return DRV_ETH_MultipleCollisionCountGet(ethId);
}


