/*******************************************************************************
  Ethernet Driver Library Source Code

  Summary:
    This file contains the source code for the Ethernet Driver library.

  Description:
    This library provides a low-level abstraction of the Ethernet module.
    It can be used to simplify low-level access to the module without the
    necessity of interacting directly with the module's registers,
    thus hiding differences from one microcontroller variant to another.
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2020-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
// DOM-IGNORE-END

#include "driver/emac/src/dynamic/drv_emac_lib.h"

static void                 rxMacPacketAck(         TCPIP_MAC_PACKET *  pMacPacket, const void *  param);
static MAC_RXFRAME_STATE    rxFindValidPacket(      MAC_DRIVER *  pMacDrvr, MAC_DRVR_RX_FRAME_INFO *  pFrameInfo);
static bool                 rxUpdateBnaStatistics(  MAC_DRIVER *  pMacDrvr );
static MAC_DRVR_RESULT      rxExtractPacket(        MAC_DRIVER *  pMacDrvr, MAC_DRVR_RX_FRAME_INFO *  pFrameInfo, TCPIP_MAC_PACKET **  ppRxMacPacket);
static MAC_RX_FILTERS       macToEthFilter(         TCPIP_MAC_RX_FILTER_TYPE  macFilter );
static uint64_t             macDrvrHashValueGet(    emac_registers_t *  pToMacRegs );
static void                 macDrvrHashValueSet(    emac_registers_t *  pToMacRegs, uint64_t  value);

// RX Buffer allocation types
// MAC TX and RX Descriptor structure with multiple Queues
#ifdef DRV_EMAC0_TX_DESCRIPTORS_COUNT_QUE0
typedef struct
{
    MAC_DRVR_HW_RXDCPT rxDescQueue0[ DRV_EMAC0_RX_DESCRIPTORS_COUNT_QUE0 ];
    MAC_DRVR_HW_TXDCPT txDescQueue0[ DRV_EMAC0_TX_DESCRIPTORS_COUNT_QUE0 ];
} EMAC0_DRVR_HW_DCPT_SPACE;

// place the descriptors in an uncached memory region
#if defined(__XC32)
static __attribute__((__aligned__(32))) __attribute__((space(data),section(".region_nocache"))) EMAC0_DRVR_HW_DCPT_SPACE EMAC0_DcptArray;
#elif defined(__IAR_SYSTEMS_ICC__)
static __attribute__((__aligned__(4))) EMAC0_DRVR_HW_DCPT_SPACE EMAC0_DcptArray @".region_nocache";
#endif
#endif

#ifdef DRV_EMAC1_TX_DESCRIPTORS_COUNT_QUE0
typedef struct
{
    MAC_DRVR_HW_RXDCPT rxDescQueue0[ DRV_EMAC1_RX_DESCRIPTORS_COUNT_QUE0 ];
    MAC_DRVR_HW_TXDCPT txDescQueue0[ DRV_EMAC1_TX_DESCRIPTORS_COUNT_QUE0 ];
} EMAC1_DRVR_HW_DCPT_SPACE;
// place the descriptors in an uncached memory region
#if defined(__XC32)
static __attribute__((__aligned__(32))) __attribute__((space(data),section(".region_nocache"))) EMAC1_DRVR_HW_DCPT_SPACE EMAC1_DcptArray;
#elif defined(__IAR_SYSTEMS_ICC__)
static __attribute__((__aligned__(4))) EMAC1_DRVR_HW_DCPT_SPACE EMAC1_DcptArray @".region_nocache";
#endif
#endif


// basic level  debugging
#if ((EMAC_DRV_DEBUG_LEVEL & EMAC_DRV_DEBUG_MASK_BASIC) != 0)
static volatile int V_EMACDrvStayAssertLoop = 0;
static void F_EMACAssertCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("EMAC Drv Assert: %s, in line: %d, \r\n", message, lineNo);
        while(V_EMACDrvStayAssertLoop != 0)
        {
        }
    }
}
#else
#define F_EMACAssertCond(cond, message, lineNo)
#endif  // (EMAC_DRV_DEBUG_LEVEL)




void macDrvrLibDescriptorsPoolClear( void )
{
#if defined( DRV_EMAC0_RX_DESCRIPTORS_COUNT_QUE0 ) || defined( DRV_EMAC0_TX_DESCRIPTORS_COUNT_QUE0 )
     (void)memset( &EMAC0_DcptArray, 0, sizeof( EMAC0_DcptArray ) );
#endif
#if defined( DRV_EMAC1_RX_DESCRIPTORS_COUNT_QUE0 ) || defined( DRV_EMAC1_TX_DESCRIPTORS_COUNT_QUE0 )
     (void)memset( &EMAC1_DcptArray, 0, sizeof( EMAC1_DcptArray ) );
#endif
}

void macDrvrLibDescriptorsPoolAssignment( MAC_DRIVER *  pMacDrvr, MAC_DRVR_DCPT_TYPE  dType)
{
    if( dType == MAC_DRVR_DCPT_TYPE_TX )
    {
#ifdef DRV_EMAC0_TX_DESCRIPTORS_COUNT_QUE0
        if( pMacDrvr->macIx == 0U)
        {
            int32_t ii;
            pMacDrvr->pTxDesc = EMAC0_DcptArray.txDescQueue0;
            for( ii = 0; ii < DRV_EMAC0_TX_DESCRIPTORS_COUNT_QUE0; ++ii )
            {
                pMacDrvr->pTxDesc[ ii ].status.val |= DRVRnEMAC_TX_USE_BIT;      // SW owns control
            }
            pMacDrvr->pTxDesc[ ii - 1 ].status.val |= EMAC_TX_WRAP_BIT;
        }
#endif
#ifdef DRV_EMAC1_TX_DESCRIPTORS_COUNT_QUE0
        if( pMacDrvr->macIx == 1U)
        {
            int32_t ii;
            pMacDrvr->pTxDesc = EMAC1_DcptArray.txDescQueue0;
            for( ii = 0; ii < DRV_EMAC1_TX_DESCRIPTORS_COUNT_QUE0; ++ii )
            {
                pMacDrvr->pTxDesc[ ii ].status.val |= DRVRnEMAC_TX_USE_BIT;      // SW owns control
            }
            pMacDrvr->pTxDesc[ ii - 1 ].status.val |= EMAC_TX_WRAP_BIT;
        }
#endif
    }
    else if( dType == MAC_DRVR_DCPT_TYPE_RX )
    {
#ifdef DRV_EMAC0_RX_DESCRIPTORS_COUNT_QUE0
        if( pMacDrvr->macIx == 0U )
        {
            int32_t ii;
            pMacDrvr->pRxDesc = EMAC0_DcptArray.rxDescQueue0;
            for( ii = 0; ii < DRV_EMAC0_RX_DESCRIPTORS_COUNT_QUE0; ++ii )
            {
                pMacDrvr->pRxDesc[ ii ].bufferAddress.val |= DRVRnEMAC_RX_OWNER_BIT;    // SW owns control
            }
            pMacDrvr->pRxDesc[ ii - 1 ].bufferAddress.val |= EMAC_RX_WRAP_BIT;
        }
#endif
#ifdef DRV_EMAC1_RX_DESCRIPTORS_COUNT_QUE0
        if( pMacDrvr->macIx == 1U)
        {
            int32_t ii;
            pMacDrvr->pRxDesc = EMAC1_DcptArray.rxDescQueue0;
            for( ii = 0; ii < DRV_EMAC1_RX_DESCRIPTORS_COUNT_QUE0; ++ii )
            {
                pMacDrvr->pRxDesc[ ii ].bufferAddress.val |= DRVRnEMAC_RX_OWNER_BIT;    // SW owns control
            }
            pMacDrvr->pRxDesc[ ii - 1 ].bufferAddress.val |= EMAC_RX_WRAP_BIT;
        }
#endif
    }
    else
    {
        // do nothing
    }
    __DSB();
}

MAC_DRVR_RESULT macDrvrLibSetMacAddr( MAC_DRIVER * pMacDrvr )
{
    emac_registers_t *  pMacRegs = (emac_registers_t *) pMacDrvr->config.ethModuleId;
    uint8_t *           pMacAddr = (uint8_t *) pMacDrvr->config.macAddress.v;

    pMacRegs->EMAC_SA[ 0 ].EMAC_SAxB = ((uint32_t)pMacAddr[ 3 ] << 24)
                                    | ((uint32_t)pMacAddr[ 2 ] << 16)
                                    | ((uint32_t)pMacAddr[ 1 ] <<  8)
                                    | ((uint32_t)pMacAddr[ 0 ])
                                    ;

    pMacRegs->EMAC_SA[ 0 ].EMAC_SAxT = ((uint32_t)pMacAddr[ 5 ] <<  8)
                                    | ((uint32_t)pMacAddr[ 4 ])
                                    ;
    return MAC_DRVR_RES_OK;
}

MAC_DRVR_RESULT macDrvrLibGetMacAddr( MAC_DRIVER *  pMacDrvr, uint8_t *  pMacAddr)
{
    emac_registers_t * pMacRegs = (emac_registers_t *) pMacDrvr->config.ethModuleId;

    pMacAddr[ 0 ] = (uint8_t)(pMacRegs->EMAC_SA[ 0 ].EMAC_SAxB) & 0xFFU;
    pMacAddr[ 1 ] = (uint8_t)((pMacRegs->EMAC_SA[ 0 ].EMAC_SAxB) >> 8) & 0xFFU;
    pMacAddr[ 2 ] = (uint8_t)((pMacRegs->EMAC_SA[ 0 ].EMAC_SAxB) >> 16) & 0xFFU;
    pMacAddr[ 3 ] = (uint8_t)((pMacRegs->EMAC_SA[ 0 ].EMAC_SAxB) >> 24) & 0xFFU;
    pMacAddr[ 4 ] = (uint8_t)(pMacRegs->EMAC_SA[ 0 ].EMAC_SAxT) & 0xFFU;
    pMacAddr[ 5 ] = (uint8_t)((pMacRegs->EMAC_SA[ 0 ].EMAC_SAxT) >> 8) & 0xFFU;

    return MAC_DRVR_RES_OK;
}

/****************************************************************************
 * Function: macDrvrLibInitializeEmac
 * Summary: Initialize MAC peripheral registers
 *****************************************************************************/
void macDrvrLibInitializeEmac( MAC_DRIVER * pMacDrvr )
{
    MAC_RX_FILTERS      macRxFilt;
    uint32_t            ncfgrPreserved;
    emac_registers_t *  pMacRegs = (emac_registers_t *) pMacDrvr->config.ethModuleId;
    // must be done first
    pMacRegs->EMAC_NCR &= ~(EMAC_NCR_RE_Msk | EMAC_NCR_TE_Msk);
    // disable all MAC interrupts for QUEUE
    pMacRegs->EMAC_IDR = (uint32_t)MAC_EV_ALL;
    // Clear Interrupt status
    (void) pMacRegs->EMAC_ISR;
    pMacRegs->EMAC_NCR  |=  EMAC_NCR_CLRSTAT_Msk        // Clear statistics
    //                  |   EMAC_NCR_WESTAT_Msk         // Enable the statistics
                        |   EMAC_NCR_MPE_Msk            // Enable the management port
                        ;
    // Clear RX Status
    pMacRegs->EMAC_RSR = EMAC_RSR_Msk;
    // Clear TX Status
    pMacRegs->EMAC_TSR = EMAC_TSR_Msk;

    if( (pMacDrvr->config.macRxFilters & (uint32_t)TCPIP_MAC_RX_FILTER_TYPE_MCAST_ACCEPT) != 0U )
    {   // Receive All Multi-cast packets, so set 64-bit hash value to all ones.
        // check against DRV_EMACx_RX_FILTERS
        MAC_DRVR_HASH hash;
        hash.value = 0xffffffffffffffffULL;    // Set 64-bit Hash value to all 1s, to receive all multi-cast
        hash.calculate = 0U;             // No hash calculation; directly set hash register
        (void)macDrvrLibRxFilterHashCalculate( pMacDrvr, &hash );
    }
    if(((uint16_t)pMacDrvr->config.pPhyInit->phyFlags & (uint16_t)DRV_ETHPHY_CFG_RMII) != 0U)
    {
        pMacRegs->EMAC_USRIO = EMAC_USRIO_RMII( 1 ); // initial mode set as RMII
    }
    else
    {
        pMacRegs->EMAC_USRIO = EMAC_USRIO_RMII( 0 ); // initial mode set as MII
    }
    pMacRegs->EMAC_USRIO |= EMAC_USRIO_CLKEN_Msk;
    // Set network configurations link speed, full duplex, copy all frames, no broadcast,
    // pause enable, remove FCS, MDC clock, and Rx Filters
    macRxFilt = macToEthFilter( (TCPIP_MAC_RX_FILTER_TYPE)pMacDrvr->config.macRxFilters );
    ncfgrPreserved = ((uint32_t)(pMacRegs->EMAC_NCFGR)) & EMAC_NCFGR_CLK_Msk;
    pMacRegs->EMAC_NCFGR = EMAC_NCFGR_FD_Msk | EMAC_NCFGR_PAE_Msk | EMAC_NCFGR_DRFCS_Msk | ncfgrPreserved | (uint32_t)macRxFilt
                            | EMAC_NCFGR_RBOF( pMacDrvr->dataOffset ) | EMAC_NCFGR_SPD_Msk ;
    // Set MAC address
    (void)macDrvrLibSetMacAddr( pMacDrvr );
}

/****************************************************************************
 * Function: macDrvrLibTransferEnable
 * Summary: Enable MAC Rx and Tx
 *****************************************************************************/
void macDrvrLibTransferEnable( MAC_DRIVER * pMacDrvr )
{
    emac_registers_t * pMacRegs = (emac_registers_t *) pMacDrvr->config.ethModuleId;
    pMacRegs->EMAC_NCR |= (EMAC_NCR_RE_Msk | EMAC_NCR_TE_Msk);
}

/****************************************************************************
 * Function: macDrvrLibClose
 * Summary: Disable MAC Rx, Tx and interrupts
 *****************************************************************************/
void macDrvrLibClose( MAC_DRIVER *  pMacDrvr, MAC_DRVR_CLOSE_FLAGS  cFlags)
{
    // disable Rx, Tx
    emac_registers_t * pMacRegs = (emac_registers_t *) pMacDrvr->config.ethModuleId;
    pMacRegs->EMAC_NCR &= ~(EMAC_NCR_RE_Msk | EMAC_NCR_TE_Msk);
    (void) pMacRegs->EMAC_ISR;
}

/****************************************************************************
 * Function: macDrvrLibMacOpen
 * Summary : Open MAC driver
 *****************************************************************************/
void macDrvrLibMacOpen( MAC_DRIVER *  pMacDrvr, TCPIP_ETH_OPEN_FLAGS  oFlags, TCPIP_ETH_PAUSE_TYPE  pauseType)
{
    emac_registers_t * pMacRegs = (emac_registers_t *) pMacDrvr->config.ethModuleId;
    uint32_t ncfgrActual = pMacRegs->EMAC_NCFGR;
    uint32_t negotiateMask = EMAC_NCFGR_FD_Msk | EMAC_NCFGR_SPD_Msk ;
    uint32_t ncfgrDesired = ncfgrActual & ~negotiateMask;

    if( ((uint32_t)oFlags & (uint32_t)TCPIP_ETH_OPEN_RMII) != 0U)
    {
        pMacRegs->EMAC_USRIO |= EMAC_USRIO_RMII_Msk;
    }
    else
    {
        pMacRegs->EMAC_USRIO &= ~EMAC_USRIO_RMII_Msk;
    }
    pMacRegs->EMAC_USRIO |= EMAC_USRIO_CLKEN_Msk;
    ////
    ncfgrDesired |= ((uint32_t)oFlags & (uint32_t)TCPIP_ETH_OPEN_FDUPLEX) != 0U ? EMAC_NCFGR_FD_Msk : 0U;
    ncfgrDesired |= ((uint32_t)oFlags & (uint32_t)TCPIP_ETH_OPEN_100) != 0U ? EMAC_NCFGR_SPD_Msk : 0U;
    if( ncfgrDesired != ncfgrActual )
    {
        pMacRegs->EMAC_NCR &= ~(EMAC_NCR_RE_Msk | EMAC_NCR_TE_Msk);
        pMacRegs->EMAC_NCFGR = ncfgrDesired;
        pMacRegs->EMAC_NCR |= (EMAC_NCR_RE_Msk | EMAC_NCR_TE_Msk);
    }
}

MAC_DRVR_RESULT macDrvrLibTxInit( MAC_DRIVER * pMacDrvr )
{
    emac_registers_t *  pMacRegs = (emac_registers_t *) pMacDrvr->config.ethModuleId;
    uint16_t            ii;

    pMacDrvr->txInsertPt = 0;
    pMacDrvr->txExtractPt = 0;
    macDrvrPacketListInitialize( &pMacDrvr->txPendingList );
    for( ii = 0; ii < pMacDrvr->config.nTxDescCnt; ii++ )
    {
        pMacDrvr->pTxDesc[ ii ].bufferAddress = 0;
        pMacDrvr->pTxDesc[ ii ].status.val = DRVRnEMAC_TX_USE_BIT;
    }
    pMacDrvr->pTxDesc[ ii - 1U].status.val |= EMAC_TX_WRAP_BIT;
    __DSB();
    pMacRegs->EMAC_TBQP = ((uint32_t) pMacDrvr->pTxDesc) & EMAC_TBQP_ADDR_Msk;
    return MAC_DRVR_RES_OK;
}

MAC_DRVR_RESULT macDrvrLibTxSendPacket( MAC_DRIVER *  pMacDrvr, TCPIP_MAC_PACKET *  pMacPacket, uint32_t  requiredDescCount)
{   // The descriptors are in non-Cacheable memory; but, the frame data is probably
    // in a non-Coherent state -- hence the DCACH clean calls.  The synchronization
    // barriers are necessary to keep the EMAC engine, which is running in parallel,
    // from jumping the gun.
    emac_registers_t *          pMacRegs = (emac_registers_t *) pMacDrvr->config.ethModuleId;
    TCPIP_MAC_DATA_SEGMENT *    pDSeg = pMacPacket->pDSeg;
    MAC_DRVR_HW_TXDCPT *        pTxDesc = pMacDrvr->pTxDesc;
    uint16_t                    ii = pMacDrvr->txInsertPt;
    uint16_t                    newInsertPt;

    // we must always own the sentinel insert point
    newInsertPt = (uint16_t)getModulus( (int32_t)ii + (int32_t)requiredDescCount, (int32_t)pMacDrvr->config.nTxDescCnt );
    pTxDesc[ newInsertPt ].status.val |= DRVRnEMAC_TX_USE_BIT;
    // to avoid a race condition; we'll skip the first segment, update all the
    // descriptors and then come back and place the first segment.
    while( pDSeg->next != NULL)
    {
        pDSeg = pDSeg->next;
        DCACHE_CLEAN_BY_ADDR( FC_U8Ptr2U32Ptr(pDSeg->segLoad), (int32_t)pDSeg->segLen );
        ii = (uint16_t)moduloIncrement( (uint32_t)ii, pMacDrvr->config.nTxDescCnt );
        pTxDesc[ ii ].status.val &= EMAC_TX_WRAP_BIT;
        pTxDesc[ ii ].status.val |= (uint32_t)EMAC_TX_DESC_LEN_MASK & (uint32_t)pDSeg->segLen;
        pTxDesc[ ii ].bufferAddress = (uint32_t) pDSeg->segLoad;
    }
    pTxDesc[ ii ].status.val |= EMAC_TX_LAST_BUFFER_BIT;
    pDSeg = pMacPacket->pDSeg;
    DCACHE_CLEAN_BY_ADDR( FC_U8Ptr2U32Ptr(pDSeg->segLoad), (int32_t)pDSeg->segLen );
    // insert the first segment -- this could be the only segment; so preserve EMAC_TX_LAST_BUFFER_BIT
    ii = pMacDrvr->txInsertPt;
    pTxDesc[ ii ].status.val &= (EMAC_TX_WRAP_BIT | EMAC_TX_LAST_BUFFER_BIT | DRVRnEMAC_TX_USE_BIT);
    pTxDesc[ ii ].status.val |= (uint32_t)EMAC_TX_DESC_LEN_MASK & (uint32_t)pDSeg->segLen;
    pTxDesc[ ii ].bufferAddress = (uint32_t) pDSeg->segLoad;
    __DSB();                                                                // insure data is placed before giving up ownership
    pTxDesc[ ii ].status.val &= ~(DRVRnEMAC_TX_USE_BIT);                    // give emac control of the descriptors
    // start transmission; this is only necessary the first time -- after that the EMAC is running in parallel
    pMacRegs->EMAC_NCR |= EMAC_NCR_TSTART_Msk;
    //
    pMacDrvr->txInsertPt = newInsertPt;
    return MAC_DRVR_RES_OK;
}

/****************************************************************************
 *****************************************************************************/
static MAC_RX_FILTERS macToEthFilter( TCPIP_MAC_RX_FILTER_TYPE macFilter )
{
    uint32_t rxFilter = 0U;

    rxFilter |= ((uint16_t)macFilter & (uint16_t)TCPIP_MAC_RX_FILTER_TYPE_FRAMEERROR_ACCEPT) != 0U ? (uint32_t)MAC_FILT_PREAMBLE_ERR_ACCEPT : 0U;
    rxFilter |= ((uint16_t)macFilter & (uint16_t)TCPIP_MAC_RX_FILTER_TYPE_CRC_ERROR_ACCEPT) != 0U  ? (uint32_t)MAC_FILT_CRC_ERR_ACCEPT : 0U;
    rxFilter |= ((uint16_t)macFilter & (uint16_t)TCPIP_MAC_RX_FILTER_TYPE_MAXFRAME_ACCEPT) != 0U   ? (uint32_t)MAC_FILT_MAXFRAME_ACCEPT : 0U;
    rxFilter |= ((uint16_t)macFilter & (uint16_t)TCPIP_MAC_RX_FILTER_TYPE_UCAST_HASH_ACCEPT) != 0U ? (uint32_t)MAC_FILT_UCASTHASH_ACCEPT : 0U;
    rxFilter |= ((uint16_t)macFilter & (uint16_t)TCPIP_MAC_RX_FILTER_TYPE_MCAST_ACCEPT)  != 0U     ? (uint32_t)MAC_FILT_MCASTHASH_ACCEPT : 0U;
    rxFilter |= ((uint16_t)macFilter & (uint16_t)TCPIP_MAC_RX_FILTER_TYPE_MCAST_HASH_ACCEPT) != 0U ? (uint32_t)MAC_FILT_MCASTHASH_ACCEPT : 0U;
    rxFilter |= ((uint16_t)macFilter & (uint16_t)TCPIP_MAC_RX_FILTER_TYPE_BCAST_ACCEPT)  != 0U     ? 0U : (uint32_t)MAC_FILT_BCAST_REJECT;
    rxFilter |= ((uint16_t)macFilter & (uint16_t)TCPIP_MAC_RX_FILTER_TYPE_ALL_ACCEPT)  != 0U       ? (uint32_t)MAC_FILT_ALLFRAME_ACCEPT : 0U;
    rxFilter |= ((uint16_t)macFilter & (uint16_t)TCPIP_MAC_RX_FILTER_TYPE_JUMBOFRAME_ACCEPT) != 0U ? (uint32_t)MAC_FILT_JUMBOFRAME_ACCEPT : 0U;

    return (MAC_RX_FILTERS)rxFilter;
}

MAC_DRVR_RESULT macDrvrLibRxInit( MAC_DRIVER * pMacDrvr )
{
    uint16_t ii;

    pMacDrvr->rxExtractPt = 0;
    pMacDrvr->rxHardStop = 0;
    macDrvrPacketListInitialize( &pMacDrvr->rxMacPacketPool );
    macDrvrPacketListInitialize( &pMacDrvr->rxDynamicReleasePool );

    for( ii = 0; ii < pMacDrvr->config.nRxDescCnt; ++ii )
    {
        pMacDrvr->pRxDesc[ ii ].status.val = 0;
        pMacDrvr->pRxDesc[ ii ].bufferAddress.val = DRVRnEMAC_RX_OWNER_BIT;
    }
    pMacDrvr->pRxDesc[ ii - 1U].bufferAddress.val |= EMAC_RX_WRAP_BIT;
    __DSB();
    emac_registers_t * pMacRegs = (emac_registers_t *) pMacDrvr->config.ethModuleId;
    pMacRegs->EMAC_RBQP = (uint32_t) pMacDrvr->pRxDesc & EMAC_RBQP_ADDR_Msk;

    return MAC_DRVR_RES_OK;
}

MAC_DRVR_RESULT macDrvrLibRxAllocate( MAC_DRIVER *  pMacDrvr, uint16_t  bufferCount, bool  stickyFlag)
{
    TCPIP_MAC_PACKET *  pMacPacket;
    MAC_DRVR_RESULT     response = MAC_DRVR_RES_OK;
    uint16_t            ii;

    for( ii = 0; ii < bufferCount; ++ii )
    {   // indicate RX
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)
        pMacPacket = pMacDrvr->callBack.pktAllocFDbg(sizeof( TCPIP_MAC_PACKET ), pMacDrvr->config.rxBufferSize - sizeof( TCPIP_MAC_ETHERNET_HEADER ), 0, TCPIP_THIS_MODULE_ID);
#else
        pMacPacket = pMacDrvr->callBack.pktAllocF(sizeof( TCPIP_MAC_PACKET ), pMacDrvr->config.rxBufferSize - sizeof( TCPIP_MAC_ETHERNET_HEADER ), 0);
#endif
        if( NULL == pMacPacket )
        {
            pMacDrvr->rxStat.packetAllocFailures++;
            response = MAC_DRVR_RES_OUT_OF_MEMORY;
            SYS_CONSOLE_PRINT( "macDrvrLibRxAllocate out of memory\r\n" );
            break;
        }
        pMacPacket->next = NULL;
        /////
        // assert seg buffer is properly aligned by the allocator function !
        F_EMACAssertCond(((uint32_t)pMacPacket->pDSeg->segBuffer & ~(EMAC_RX_ADDRESS_MASK)) == 0U, __func__, __LINE__);
        // setup the packet acknowledgment for later use;
        pMacPacket->ackFunc = &rxMacPacketAck;
        pMacPacket->ackParam = pMacDrvr;
        if( stickyFlag )
        {
            pMacPacket->pDSeg->segFlags |= (uint16_t)TCPIP_MAC_SEG_FLAG_RX_STICKY;
        }
        /////
        macDrvrPacketListTailAdd( &pMacDrvr->rxMacPacketPool, pMacPacket);
    }
    return response;
}

MAC_DRVR_RESULT macDrvrLibRxPopulateDescriptors( MAC_DRIVER * pMacDrvr )
{
    TCPIP_MAC_PACKET *  pMacPacket;
    uint32_t            dataAddress;
    MAC_DRVR_RESULT     response = MAC_DRVR_RES_OK;
    uint16_t            ii;

    // we hold onto ownership of a sentinel slot, rxHardStop. So, we need the
    // next slot -- for the hard stop to advance
    ii = (uint16_t)moduloIncrement( (uint32_t)pMacDrvr->rxHardStop, (uint32_t)pMacDrvr->config.nRxDescCnt );
    while(true)
    {
        if(((uint32_t)DRVRnEMAC_RX_OWNER_BIT & pMacDrvr->pRxDesc[ ii ].bufferAddress.val) == 0U)
        {
            break;
        }

        if((EMAC_RX_ADDRESS_MASK & pMacDrvr->pRxDesc[ ii ].bufferAddress.val) != 0U)
        {
            break;
        }

        // we own the descriptor and the address is marked finished -- all zeroes
        pMacPacket = macDrvrPacketListHeadRemove( &pMacDrvr->rxMacPacketPool );
        if( pMacPacket == NULL )
        {
            response = MAC_DRVR_RES_NO_RX_QUEUE;
            break;
        }
        //
        pMacDrvr->pRxDesc[ pMacDrvr->rxHardStop ].status.val = 0; // reset status value
        // assert seg load is properly aligned
        F_EMACAssertCond(((uint32_t)pMacPacket->pDSeg->segBuffer & ~(EMAC_RX_ADDRESS_MASK)) == 0U, __func__, __LINE__);
        dataAddress = (uint32_t) pMacPacket->pDSeg->segBuffer;
        if( pMacDrvr->rxHardStop == (pMacDrvr->config.nRxDescCnt - 1U) )
        {
            dataAddress |= EMAC_RX_WRAP_BIT;
        }
        ///////// EMACs DMA IS GIVEN OWNERSHIP OF THE BUFFER HERE //////////
        // set address and simultaneously clear OWNER ship bit --  provide
        // emac-dma with ownership
        __DSB();
        pMacDrvr->pRxDesc[ pMacDrvr->rxHardStop ].bufferAddress.val = dataAddress;
        ////////////////////////////////////////////////////////////////////
        // now move the hard stop up
        pMacDrvr->rxStat.nRxSchedBuffers++;
        pMacDrvr->rxHardStop = ii;
        //
        ii = (uint16_t)moduloIncrement( (uint32_t)ii, (uint32_t)pMacDrvr->config.nRxDescCnt );
    }

    return response;
}

/*******************************************************************************
 * Function: macDrvrHashValueSet
 * Summary:  Set Hash Value in MAC register
 ******************************************************************************/
static __inline__ void __attribute__((always_inline)) macDrvrHashValueSet( emac_registers_t *  pToMacRegs, uint64_t  value)
{
    pToMacRegs->EMAC_HRB = (uint32_t)value & 0xffffffffU;
    pToMacRegs->EMAC_HRT = (uint32_t)(value >> 32) & 0xffffffffU;
}

/*******************************************************************************
 * Function: macDrvrHashValueGet
 * Summary:  Read Hash Value in MAC register
 ******************************************************************************/
static __inline__ uint64_t __attribute__((always_inline))
macDrvrHashValueGet( emac_registers_t * pToMacRegs )
{
    uint64_t value = 0;
    value = pToMacRegs->EMAC_HRT;
    value = (value << 32) | pToMacRegs->EMAC_HRB;
    return value;
}

/****************************************************************************
 * Function:    macDrvrLibRxFilterHashCalculate
 * Summary :    calculate the hash value for given mac address and set the hash
                value in MAC register
 *****************************************************************************/
MAC_DRVR_RESULT macDrvrLibRxFilterHashCalculate( MAC_DRIVER *  pMacDrvr, MAC_DRVR_HASH * hash)
{
    uint64_t value;
    MAC_DRVR_HASH_INDEX hashIndex;
    const MAC_DRVR_ADDR_OCTET * addr;
    emac_registers_t * pMacRegs = (emac_registers_t *) pMacDrvr->config.ethModuleId;

    if( hash->calculate == 1U )   // calculate hash for given MAC address
    {
        union
        {
            const TCPIP_MAC_ADDR *  destMacAddr;
            const MAC_DRVR_ADDR_OCTET* addr;
        }U_MAC_ADDR_OCTET;

        U_MAC_ADDR_OCTET.destMacAddr = hash->destMacAddr;
        addr = U_MAC_ADDR_OCTET.addr;

        hashIndex.index = 0;
        hashIndex.bits.b0 = (uint8_t)((addr[ 5 ].bits.b0)^(addr[ 5 ].bits.b6)^(addr[ 4 ].bits.b4)^(addr[ 3 ].bits.b2)^(addr[ 2 ].bits.b0)^(addr[ 2 ].bits.b6)^(addr[ 1 ].bits.b4)^(addr[ 0 ].bits.b2));
        hashIndex.bits.b1 = (uint8_t)((addr[ 5 ].bits.b1)^(addr[ 5 ].bits.b7)^(addr[ 4 ].bits.b5)^(addr[ 3 ].bits.b3)^(addr[ 2 ].bits.b1)^(addr[ 2 ].bits.b7)^(addr[ 1 ].bits.b5)^(addr[ 0 ].bits.b3));
        hashIndex.bits.b2 = (uint8_t)((addr[ 5 ].bits.b2)^(addr[ 4 ].bits.b0)^(addr[ 4 ].bits.b6)^(addr[ 3 ].bits.b4)^(addr[ 2 ].bits.b2)^(addr[ 1 ].bits.b0)^(addr[ 1 ].bits.b6)^(addr[ 0 ].bits.b4));
        hashIndex.bits.b3 = (uint8_t)((addr[ 5 ].bits.b3)^(addr[ 4 ].bits.b1)^(addr[ 4 ].bits.b7)^(addr[ 3 ].bits.b5)^(addr[ 2 ].bits.b3)^(addr[ 1 ].bits.b1)^(addr[ 1 ].bits.b7)^(addr[ 0 ].bits.b5));
        hashIndex.bits.b4 = (uint8_t)((addr[ 5 ].bits.b4)^(addr[ 4 ].bits.b2)^(addr[ 3 ].bits.b0)^(addr[ 3 ].bits.b6)^(addr[ 2 ].bits.b4)^(addr[ 1 ].bits.b2)^(addr[ 0 ].bits.b0)^(addr[ 0 ].bits.b6));
        hashIndex.bits.b5 = (uint8_t)((addr[ 5 ].bits.b5)^(addr[ 4 ].bits.b3)^(addr[ 3 ].bits.b1)^(addr[ 3 ].bits.b7)^(addr[ 2 ].bits.b5)^(addr[ 1 ].bits.b3)^(addr[ 0 ].bits.b1)^(addr[ 0 ].bits.b7));

        // read the current hash value stored in register
        value = macDrvrHashValueGet( pMacRegs );
        value |= (1ULL << (hashIndex.index));
    }
    else
    {   // Set hash value directly
        value = hash->value;
    }
    macDrvrHashValueSet( pMacRegs, value );

    return MAC_DRVR_RES_OK;
}

/****************************************************************************
 * Function: rxGetPacket
 * Summary:  extract RX packet from the RX descriptor
 *****************************************************************************/
MAC_DRVR_RESULT rxGetPacket( MAC_DRIVER *  pMacDrvr, TCPIP_MAC_PACKET **  ppRxMacPacket, MAC_DRVR_RXDCPT_STATUS *  pRxStatus)
{
    MAC_DRVR_RX_FRAME_INFO  frameInfo = { 0, 0, 0 };
    MAC_DRVR_RESULT         response = MAC_DRVR_RES_NO_PACKET;

    if( ppRxMacPacket != NULL && pRxStatus != NULL )
    {
        (void)rxUpdateBnaStatistics( pMacDrvr );
        if( MAC_RX_VALID_FRAME_DETECTED_STATE == rxFindValidPacket( pMacDrvr, &frameInfo ) )
        {   // extract the packet and provide it and it's status, to the upper layer
            *pRxStatus = pMacDrvr->pRxDesc[ frameInfo.endIndex ].status;
            response = rxExtractPacket( pMacDrvr, &frameInfo, ppRxMacPacket );
            // place fresh buffers in the vacated descriptors
            (void)macDrvrLibRxPopulateDescriptors( pMacDrvr );
        }
    }

    return response;
}

/****************************************************************************
 * Function: rxUpdateBnaStatistics
 * Summary:  Check 'Buffer Not Available' set by MAC; when set clear and update stats
 * Return :  True if BNA is set; False for otherwise
 *****************************************************************************/
static bool rxUpdateBnaStatistics( MAC_DRIVER * pMacDrvr )
{
    emac_registers_t *  pMacRegs = (emac_registers_t *) pMacDrvr->config.ethModuleId;
    bool                retval = false;

    if( (pMacRegs->EMAC_RSR & EMAC_RSR_BNA_Msk) != 0U) // check for BNA error due to shortage of Rx Buffers
    {
        pMacRegs->EMAC_RSR = EMAC_RSR_BNA_Msk;  // clear 'Buffer Not Available'
        pMacDrvr->rxStat.nRxBuffNotAvailable++;
        retval = true;
    }
    return retval;
}

void rxExpungeFrameSegments( MAC_DRIVER *  pMacDrvr, MAC_DRVR_RX_FRAME_INFO *  pFrameInfo)
{
    TCPIP_MAC_PACKET *          pMacPacket;
    MAC_DRVR_RX_FRAME_INFO      tmpFrameInfo = { 0, 0, 0 };
    uint32_t                    ii, jj;

    // using in bound info: create a frame from each segment, extract each
    // descriptor, provide a segLen and then use the ack function to
    // recycle the memory
    pMacDrvr->rxStat.nRxErrorPackets++;
    for( ii = 0; ii < pFrameInfo->bufferCount; ++ii )
    {
        jj = getModulus((int32_t)pFrameInfo->startIndex + (int32_t)ii, (int32_t)pMacDrvr->config.nRxDescCnt);
        tmpFrameInfo.endIndex = tmpFrameInfo.startIndex = (uint16_t)jj;
        tmpFrameInfo.bufferCount = 1;
        if( MAC_DRVR_RES_OK == rxExtractPacket( pMacDrvr, &tmpFrameInfo, &pMacPacket ) )
        {
            pMacDrvr->rxStat.expungedSegments++;
            rxMacPacketAck( pMacPacket, pMacDrvr );
        }
    }
}

/****************************************************************************
 * Function: rxFindValidPacket
 * Summary:  Search Rx descriptors for valid Rx data packet; identify start and
 *           end rx descriptor indexes of valid rx data packet
 * Return :  frame search status
 *****************************************************************************/
static MAC_RXFRAME_STATE rxFindValidPacket( MAC_DRIVER *  pMacDrvr, MAC_DRVR_RX_FRAME_INFO *  pFrameInfo)
{
    MAC_DRVR_HW_RXDCPT *    pRxDesc = pMacDrvr->pRxDesc;
    MAC_RXFRAME_STATE       frameState = MAC_RX_NO_FRAME_STATE;
    uint16_t                rxDescCount = pMacDrvr->config.nRxDescCnt;
    uint16_t                ii = pMacDrvr->rxExtractPt;

    // search the descriptors for valid data frame; search maximum of descriptor count
    while( ii != pMacDrvr->rxHardStop )
    {
        if( ((uint32_t)DRVRnEMAC_RX_OWNER_BIT & pRxDesc[ ii ].bufferAddress.val) == 0U )
        {   // we don't have ownership -- emac dma does: even when
            // (MAC_RX_SOF_DETECTED_STATE == frameState), we'll assume emac's not done
            return MAC_RX_NO_FRAME_STATE;
        }

        // Rx Descriptors with software owned descriptor
        // look for the first descriptor of data frame
        if( MAC_RX_NO_FRAME_STATE == frameState )
        {
            if( (EMAC_RX_SOF_BIT & pRxDesc[ ii ].status.val) != 0U)
            {   // Start of Frame bit set
                // transition the state to SOF detected
                frameState = MAC_RX_SOF_DETECTED_STATE;
                pFrameInfo->startIndex = ii;
                pFrameInfo->bufferCount = 1;    // start counting number of frames from 1
                if( (EMAC_RX_EOF_BIT & pRxDesc[ ii ].status.val) != 0U)
                {   // End of Frame in same descriptor
                    // SOF and EOF in same descriptor; transition to EOF detected
                    frameState = MAC_RX_VALID_FRAME_DETECTED_STATE;
                    pFrameInfo->endIndex = ii;
                    //
                    break;
                }
            }
            else
            {   // Mid or End of Frame without a start of frame; must be an error
                // any packet dma given back to us without SOF is not usable
                pFrameInfo->startIndex = ii;
                pFrameInfo->bufferCount = 1;
                pFrameInfo->endIndex = ii;
                rxExpungeFrameSegments( pMacDrvr, pFrameInfo );
            }
        }
        else if( MAC_RX_SOF_DETECTED_STATE == frameState )
        {
            ++pFrameInfo->bufferCount;
            if( (EMAC_RX_SOF_BIT & pRxDesc[ ii ].status.val) != 0U)
            {   // SOF detected again; must be an error, clear the preceding descriptors
                // and use this as the new start
                pFrameInfo->endIndex = (uint16_t)moduloDecrement( (uint32_t)ii, (uint32_t)rxDescCount );    // end of group we're expunging is one back
                --pFrameInfo->bufferCount;
                rxExpungeFrameSegments( pMacDrvr, pFrameInfo );
                pFrameInfo->startIndex = ii;    // new beginning
                pFrameInfo->bufferCount = 1;    // start counting number of frames from 1
            }
            if( (EMAC_RX_EOF_BIT & pRxDesc[ ii ].status.val) != 0U)
            {
                frameState = MAC_RX_VALID_FRAME_DETECTED_STATE;
                pFrameInfo->endIndex = ii;
                //
                break;
            }
        }
        else
        {
            // do nothing
        }
        ii = (uint16_t)moduloIncrement( (uint32_t)ii, (uint32_t)rxDescCount );
    }
    return frameState;
}

/****************************************************************************
 * Function: rxExtractPacket
 * Summary:  Extract Rx packet from MAC and transfer to TCP/IP stack.
 *           Assign new Rx packet buffers to MAC
 * Return :  new buffer allocation status
 *****************************************************************************/
static MAC_DRVR_RESULT rxExtractPacket( MAC_DRIVER *  pMacDrvr, MAC_DRVR_RX_FRAME_INFO *  pFrameInfo, TCPIP_MAC_PACKET **  ppRxMacPacket)
{
    TCPIP_MAC_PACKET *          pMacPacket;
    TCPIP_MAC_DATA_SEGMENT *    pDSeg;
    uint8_t *                   pSegBuff;
    TCPIP_MAC_SEGMENT_GAP_DCPT* pGap;
    uint16_t                    bytesRemaining;
    uint16_t                    extractSize;
    uint16_t                    bufferCount = pFrameInfo->bufferCount;
    uint16_t                    ii = pFrameInfo->startIndex;
    MAC_DRVR_RESULT             retval = MAC_DRVR_RES_NO_PACKET;

    // extract pMacPacket using the segment buffer

    pSegBuff = (uint8_t*)(EMAC_RX_ADDRESS_MASK & pMacDrvr->pRxDesc[ ii ].bufferAddress.val);
    // get packet pointer from buffer gap descriptor
    pGap = FC_U8Ptr2GapDcpt(pSegBuff + pMacDrvr->gapDcptOffset);
    pMacPacket = pGap->segmentPktPtr;

    if( pMacPacket != NULL)
    {
        if( ++pMacDrvr->rxStat.macPacketsInStack > pMacDrvr->rxStat.macPacketsInStackHighPoint )
        {
            pMacDrvr->rxStat.macPacketsInStackHighPoint = pMacDrvr->rxStat.macPacketsInStack;
        }
        pMacDrvr->rxStat.macSegmentsInStack += (uint32_t)bufferCount;
        // frame size is in the last segment and indicates the number of bytes to process
        bytesRemaining = (uint16_t)EMAC_RX_FRAME_LENGTH_MASK & (uint16_t)pMacDrvr->pRxDesc[ pFrameInfo->endIndex ].status.val;
        // process all the frame segments
        pDSeg = pMacPacket->pDSeg;
        pDSeg->segFlags |= (uint16_t)TCPIP_MAC_SEG_FLAG_ACK_REQUIRED; // allow rxMacPacketAck entry
        while( bufferCount-- != 0U)
        {
            --pMacDrvr->rxStat.nRxSchedBuffers;
            // clear the buffer address bit-fields but retain EMAC_RX_WRAP_BIT and DRVRnEMAC_RX_OWNER_BIT
            pMacDrvr->pRxDesc[ ii ].bufferAddress.val &= (EMAC_RX_WRAP_BIT | DRVRnEMAC_RX_OWNER_BIT);
            pMacDrvr->pRxDesc[ ii ].status.val = 0;
            //
            pDSeg->segLoad = pDSeg->segBuffer;
            extractSize = pMacDrvr->config.rxBufferSize;
            if( ii == pFrameInfo->startIndex )
            {   // adjust the segment start and size
                pDSeg->segLoad += pMacDrvr->dataOffset;
                extractSize -= pMacDrvr->dataOffset;
            }

            if( extractSize > bytesRemaining )
            {
                extractSize = bytesRemaining;
            }
            bytesRemaining -= extractSize;
            pDSeg->segLen = extractSize;
            pDSeg->segSize = pMacDrvr->config.rxBufferSize;
            DCACHE_INVALIDATE_BY_ADDR( FC_U8Ptr2U32Ptr(pDSeg->segLoad), (int32_t)extractSize );
            // link the next descriptors segment to the parent packet
            if( bytesRemaining != 0U)
            {
                TCPIP_MAC_PACKET * pTemp;

                pMacPacket->pktFlags |= (uint32_t)TCPIP_MAC_PKT_FLAG_SPLIT;
                ii = (uint16_t)moduloIncrement( (uint32_t)ii, (uint32_t)pMacDrvr->config.nRxDescCnt );
                // extract pMacPacket using the segLoad buffer
                pSegBuff = (uint8_t*)(EMAC_RX_ADDRESS_MASK & pMacDrvr->pRxDesc[ ii ].bufferAddress.val);
                // get packet pointer from buffer gap descriptor
                pGap = FC_U8Ptr2GapDcpt(pSegBuff + pMacDrvr->gapDcptOffset);
                pTemp = pGap->segmentPktPtr;

                pDSeg->next = pTemp->pDSeg;
                pDSeg = pDSeg->next;
            }
        }
        pFrameInfo->endIndex = (uint16_t)moduloIncrement( (uint32_t)pFrameInfo->endIndex, (uint32_t)pMacDrvr->config.nRxDescCnt);
        pMacDrvr->rxExtractPt = pFrameInfo->endIndex;
        retval = MAC_DRVR_RES_OK;
    }

    // setup reply
    *ppRxMacPacket = pMacPacket;

    return retval;
}

/****************************************************************************
 * Function: rxMacPacketAck
 * Summary:  ACK function to free the RX packet
 *****************************************************************************/
static void rxMacPacketAck( TCPIP_MAC_PACKET * pMacPacket, const void * param )
{
    union
    {
        const void * param;
        MAC_DRIVER* pMacDrvr;
    }U_PARAM_MAC_DRIVER;

    U_PARAM_MAC_DRIVER.param = param;
    MAC_DRIVER *                pMacDrvr = U_PARAM_MAC_DRIVER.pMacDrvr;
    TCPIP_MAC_DATA_SEGMENT *    pDSegNext;

    if( pMacPacket != NULL &&  pMacPacket->pDSeg != NULL && (pMacPacket->pDSeg->segFlags & (uint16_t)TCPIP_MAC_SEG_FLAG_ACK_REQUIRED) != 0U)
    {
        pMacPacket->pDSeg->segFlags &= ~(uint16_t)TCPIP_MAC_SEG_FLAG_ACK_REQUIRED;
        pMacDrvr->rxStat.macPacketsInStack--;
        while( pMacPacket->pDSeg != NULL)
        {
            pMacDrvr->rxStat.macSegmentsInStack--;
            pDSegNext = pMacPacket->pDSeg->next;
            // prep to recycle packet into rxMacPacketPool; buffer offset
            // assert seg buffer is properly aligned
            F_EMACAssertCond(((uint32_t)pMacPacket->pDSeg->segBuffer & ~(EMAC_RX_ADDRESS_MASK)) == 0U, __func__, __LINE__);
            pMacPacket->pDSeg->next = NULL;
            // recycle the packet
            if(pMacDrvr->rxDynamicRelease != 0U &&  (pMacPacket->pDSeg->segFlags & (uint16_t)TCPIP_MAC_SEG_FLAG_RX_STICKY) == 0U)
            {
                --pMacDrvr->rxDynamicRelease;
                macDrvrPacketListTailAdd( &pMacDrvr->rxDynamicReleasePool, pMacPacket );
            }
            else
            {
                macDrvrPacketListTailAdd( &pMacDrvr->rxMacPacketPool, pMacPacket );
            }

            if( pDSegNext == NULL)
            {   // packet handling is now complete
                break;
            }
            // Ethernet packet stored in multiple MAC descriptors, each segment
            // is allocated as a complete mac packet
            // extract the packet pointer using the segment gap descriptor
            TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = FC_U8Ptr2GapDcpt(pDSegNext->segBuffer + pMacDrvr->gapDcptOffset);
            pMacPacket = pGap->segmentPktPtr;
        }
    }
    else
    {   // double release ????
        F_EMACAssertCond(false, __func__, __LINE__);
    }
}

MAC_DRVR_RESULT macDrvrLibGetDynamicBuffers( MAC_DRIVER * pMacDrvr )
{
    MAC_DRVR_RESULT retval = MAC_DRVR_RES_OK;
    int32_t         buffersRequired = 0;
    int32_t         poolLevel = (int32_t)pMacDrvr->config.nRxStaticBufferCnt - (int32_t)pMacDrvr->config.nRxDescCnt;
    // setup for exponential moving average
    // to avoid floating math we'll scale alpha and oneMinusAlpha and use integers
    // instead of percentages
    // accumulation = ((alpha * newValue) + (1-alpha)(accumulation)) / scale
    pMacDrvr->rxPoolLevelEma.scale = 10000;
    pMacDrvr->rxPoolLevelEma.alpha = (int32_t)pMacDrvr->config.nRxBuffAllocCnt;
    pMacDrvr->rxPoolLevelEma.alpha *= pMacDrvr->rxPoolLevelEma.scale;
    pMacDrvr->rxPoolLevelEma.alpha /= (int32_t)pMacDrvr->config.rxDeviceMaxDescriptors;
    //
    pMacDrvr->rxPoolLevelEma.oneMinusAlpha = pMacDrvr->rxPoolLevelEma.scale - pMacDrvr->rxPoolLevelEma.alpha;
    pMacDrvr->rxPoolLevelEma.setPoint = poolLevel;
    if( pMacDrvr->config.nRxBuffAllocCnt != 0U && pMacDrvr->config.nRxBuffCntThres != 0U)
    {   // assign the set point to be (allocStepSize + 1)/2 greater than
        // the available threshold
        uint32_t nAlloc = ((uint32_t)pMacDrvr->config.nRxBuffAllocCnt + 1UL) >> 1;
        pMacDrvr->rxPoolLevelEma.setPoint = (int32_t)pMacDrvr->config.nRxBuffCntThres + (int32_t)nAlloc;
        if( poolLevel < pMacDrvr->rxPoolLevelEma.setPoint )
        {
            buffersRequired = pMacDrvr->rxPoolLevelEma.setPoint - poolLevel;
            retval = macDrvrLibRxAllocate(  pMacDrvr, (uint16_t)buffersRequired, MAC_RX_DYNAMIC_BUFFERS);
            if( MAC_DRVR_RES_OK != retval )
            {
                buffersRequired = 0;
            }
            poolLevel += buffersRequired;
        }
    }
    pMacDrvr->rxPoolLevelEma.accumulation = poolLevel;
    //
    pMacDrvr->rxDynamicAllocation = (uint16_t)buffersRequired;
    pMacDrvr->rxDynamicRelease = 0U;

    return retval;
}

MAC_DRVR_RESULT macDrvrLibManageDynamicBuffers( MAC_DRIVER * pMacDrvr )
{
    int32_t         ema;
    int32_t         nNodes;
    MAC_DRVR_RESULT retval = MAC_DRVR_RES_OK;

    if( pMacDrvr->config.nRxBuffAllocCnt != 0U && pMacDrvr->config.nRxBuffCntThres != 0U)
    {
        ema = pMacDrvr->rxPoolLevelEma.accumulation;
        ema = ((int32_t)pMacDrvr->rxMacPacketPool.nNodes * pMacDrvr->rxPoolLevelEma.alpha) + (ema * pMacDrvr->rxPoolLevelEma.oneMinusAlpha);
        ema /= pMacDrvr->rxPoolLevelEma.scale;

        if( ema > (int32_t)pMacDrvr->config.nRxBuffCntThres )
        {
            if( ema > pMacDrvr->rxPoolLevelEma.setPoint )
            {   // only ask for 1 packet at a time to be added to the dynamic release list
                pMacDrvr->rxDynamicRelease = 1;
                ema = pMacDrvr->rxPoolLevelEma.setPoint;
            }
            (void)macDrvrLibFreeDynamicBuffers( pMacDrvr );
        }
        else
        {
            nNodes = 0;
            while( pMacDrvr->rxDynamicReleasePool.nNodes != 0U)
            {   // reuse anything waiting for release
                if( ++nNodes >= (int32_t)pMacDrvr->config.nRxBuffAllocCnt )
                {
                    break;
                }
                macDrvrPacketListTailAdd( &pMacDrvr->rxMacPacketPool, macDrvrPacketListHeadRemove( &pMacDrvr->rxDynamicReleasePool ));
            }
            if( nNodes < (int32_t)pMacDrvr->config.nRxBuffAllocCnt )
            {   // not enough so get more
                nNodes = (int32_t)pMacDrvr->config.nRxBuffAllocCnt - nNodes;
                retval = macDrvrLibRxAllocate(  pMacDrvr, (uint16_t)nNodes, MAC_RX_DYNAMIC_BUFFERS);
                if( MAC_DRVR_RES_OK == retval )
                {
                    pMacDrvr->rxDynamicAllocation += (uint16_t)nNodes;
                    ema = pMacDrvr->rxPoolLevelEma.setPoint;
                }
            }
        }

        pMacDrvr->rxPoolLevelEma.accumulation = ema;
    }
    return retval;
}

MAC_DRVR_RESULT macDrvrLibFreeDynamicBuffers( MAC_DRIVER * pMacDrvr )
{
    TCPIP_MAC_PACKET * pMacPacket;

    while( pMacDrvr->rxDynamicReleasePool.nNodes != 0U)
    {   // free items in the release pool
        pMacPacket = macDrvrPacketListHeadRemove( &pMacDrvr->rxDynamicReleasePool );
        if( pMacPacket != NULL)
        {
            --pMacDrvr->rxDynamicAllocation;
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)
            pMacDrvr->callBack.pktFreeFDbg( pMacPacket, TCPIP_THIS_MODULE_ID);
#else
            pMacDrvr->callBack.pktFreeF( pMacPacket );
#endif
            (void)pMacDrvr->callBack.freeF( pMacDrvr->allocH, pMacPacket );
        }
    }

    return MAC_DRVR_RES_OK;
}

/* Generic Single Linked List Manipulation ************************************/
TCPIP_MAC_PACKET * macDrvrPacketListHeadRemove( MAC_DRVR_PACKET_LIST * pList)
{
    TCPIP_MAC_PACKET * pMacPacket = pList->head;
    if( pMacPacket  != NULL)
    {
        if( pList->head == pList->tail )
        {
            pList->head = pList->tail = NULL;
        }
        else
        {
            pList->head = pMacPacket->next;
        }
        --pList->nNodes;
        pMacPacket->next = NULL;
    }
    return pMacPacket;
}

void macDrvrPacketListTailAdd( MAC_DRVR_PACKET_LIST *  pList, TCPIP_MAC_PACKET *  pMacPacket
    )
{
    if( pMacPacket  != NULL)
    {
        pMacPacket->next = NULL;
        if( pList->tail  != NULL)
        {
            pList->tail->next = pMacPacket;
            pList->tail = pMacPacket;
        }
        else
        {
            pList->head = pList->tail = pMacPacket;
        }
        ++pList->nNodes;
    }
}

void macDrvrPacketListHeadAdd( MAC_DRVR_PACKET_LIST *  pList, TCPIP_MAC_PACKET *  pMacPacket)
{
    if( pMacPacket  != NULL)
    {
        if( pList->head  != NULL)
        {
            pMacPacket->next = pList->head;
            pList->head = pMacPacket;
        }
        else
        {
            pList->head = pList->tail = pMacPacket;
            pMacPacket->next = NULL;
        }
        ++pList->nNodes;
    }
}

