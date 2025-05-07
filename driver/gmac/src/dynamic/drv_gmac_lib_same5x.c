/*******************************************************************************
  Ethernet Driver Library Source Code

  Summary:
    This file contains the source code for the Ethernet Driver library.

  Description:
    This library provides a low-level abstraction of the Ethernet module
    on Microchip SAME54 family microcontrollers with a convenient C language
    interface.  It can be used to simplify low-level access to the module
    without the necessity of interacting directly with the module's registers,
    thus hiding differences from one microcontroller variant to another.
*******************************************************************************/
//DOM-IGNORE-BEGIN
/*
Copyright (C) 2008-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

//DOM-IGNORE-END
#include "driver/gmac/src/dynamic/drv_gmac_lib.h"
/** D E F I N I T I O N S ****************************************************/
//PIC32C internal GMAC peripheral
#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_MAC_PIC32C
// RX Buffer allocation types
#define GMAC_RX_STICKY_BUFFERS  1
#define GMAC_RX_DYNAMIC_BUFFERS 0

/******************************************************************************
 * Prototypes
 ******************************************************************************/
static void MacRxPacketAck(TCPIP_MAC_PACKET* pPkt,  const void* param);
static bool IsBufferNotAvailable(DRV_GMAC_DRIVER * pMACDrv);
static GMAC_RXFRAME_STATE SearchRxPacket(DRV_GMAC_DRIVER * pMACDrv,
            DRV_PIC32CGMAC_RX_FRAME_INFO *rx_frame_state, GMAC_QUE_LIST queueIdx);
static DRV_PIC32CGMAC_RESULT GetRxPacket(DRV_GMAC_DRIVER * pMACDrv,
            DRV_PIC32CGMAC_RX_FRAME_INFO *rx_frame_state, TCPIP_MAC_PACKET** pRxPkt, GMAC_QUE_LIST queueIdx);
static DRV_PIC32CGMAC_RESULT AllocateRxPacket(DRV_GMAC_DRIVER * pMACDrv, 
            uint16_t buffer_count, GMAC_QUE_LIST queue_idx, bool sticky_flag);
static uint16_t GetPktSegCount(TCPIP_MAC_DATA_SEGMENT * pktHead);
static void F_DRV_GMAC_HashValueSet(DRV_GMAC_DRIVER* pMACDrv, uint64_t hash_value);
static  uint64_t  F_DRV_GMAC_HashValueGet(DRV_GMAC_DRIVER* pMACDrv);
//GMAC TX and RX Descriptor structure with multiple Queues  
typedef struct
{
  DRV_PIC32CGMAC_HW_TXDCPT sTxDesc_queue0[TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE0];
    DRV_PIC32CGMAC_HW_RXDCPT sRxDesc_queue0[TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE0];
} DRV_PIC32CGMAC_HW_DCPT_ARRAY ;

// place the descriptors in 8-byte aligned memory region
static __attribute__((__aligned__(8))) DRV_PIC32CGMAC_HW_DCPT_ARRAY gmac_dcpt_array;

static DRV_GMAC_QUEUE s_gmac_queue[DRV_GMAC_NUMBER_OF_QUEUES];

static uint32_t    drvGmacQueEvents;     //Priority Queue Event Status

// conversion functions/helpers
static __inline__ DRV_PIC32CGMAC_SGL_LIST_NODE* __attribute__((always_inline)) FC_MacPkt2LstNode(TCPIP_MAC_PACKET * ptrPacket)
{
    union
    {
        TCPIP_MAC_PACKET * pPkt;
        DRV_PIC32CGMAC_SGL_LIST_NODE * pNode;
    }U_PKT_NODE;

    U_PKT_NODE.pPkt = ptrPacket;
    return U_PKT_NODE.pNode;
}
static __inline__ TCPIP_MAC_PACKET * __attribute__((always_inline)) FC_LstNode2MacPkt(DRV_PIC32CGMAC_SGL_LIST_NODE * lstNode)
{
    union
    {
        DRV_PIC32CGMAC_SGL_LIST_NODE * pNode;
        TCPIP_MAC_PACKET * pPkt;        
    }U_NODE_PKT;

    U_NODE_PKT.pNode = lstNode;
    return U_NODE_PKT.pPkt;
}
static __inline__ DRV_GMAC_TX_DESC_INDEX * __attribute__((always_inline)) FC_LstNode2TxDscIdx(DRV_PIC32CGMAC_SGL_LIST_NODE * lstNode)
{
    union
    {
        DRV_PIC32CGMAC_SGL_LIST_NODE * pNode;
        DRV_GMAC_TX_DESC_INDEX * pTxDscIdx;        
    }U_NODE_DSCIDX;

    U_NODE_DSCIDX.pNode = lstNode;
    return U_NODE_DSCIDX.pTxDscIdx;
}
static __inline__ DRV_PIC32CGMAC_SGL_LIST_NODE * __attribute__((always_inline)) FC_TxDscIdx2LstNode(DRV_GMAC_TX_DESC_INDEX * txDsc)
{
    union
    {
        DRV_GMAC_TX_DESC_INDEX * pTxDscIdx; 
        DRV_PIC32CGMAC_SGL_LIST_NODE * pNode;               
    }U_DSCIDX_NODE;

    U_DSCIDX_NODE.pTxDscIdx = txDsc;
    return U_DSCIDX_NODE.pNode;
}
static __inline__ TCPIP_MAC_SEGMENT_GAP_DCPT * __attribute__((always_inline)) FC_Ptr8toSegGapDsc(uint8_t * pPkt)
{
    union
    {
        uint8_t * pPtr_8;; 
        TCPIP_MAC_SEGMENT_GAP_DCPT * pSegGapDsc;              
    }U_PTR8_SEGGAPDSC;

    U_PTR8_SEGGAPDSC.pPtr_8 = pPkt;
    return U_PTR8_SEGGAPDSC.pSegGapDsc;
}
static __inline__ DRV_GMAC_MAC_ADDR* __attribute__((always_inline)) FC_MacAddr2GmacAddr(TCPIP_MAC_ADDR const * macaddr)
{
    union
    {
        TCPIP_MAC_ADDR const * pMacAddr;
        DRV_GMAC_MAC_ADDR* pGmacAddr;
    }U_MACADDR_GMACADDR;

    U_MACADDR_GMACADDR.pMacAddr = macaddr;
    return U_MACADDR_GMACADDR.pGmacAddr;
}
static __inline__ DRV_GMAC_DRIVER* __attribute__((always_inline)) FC_Prm2GmacDrv(void const * pParam)
{
    union
    {
        const void* pPtr;
        DRV_GMAC_DRIVER * pGmacDrv;
    }U_PTR_GMACDRV;

    U_PTR_GMACDRV.pPtr = pParam;
    return U_PTR_GMACDRV.pGmacDrv;
}
/****************************************************************************
 * Function:        DRV_PIC32CGMAC_LibInit
 * Summary: Initialize GMAC peripheral registers
 *****************************************************************************/
void DRV_PIC32CGMAC_LibInit(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    /* Configure the AHB Bridge Clock for GMAC */
    MCLK_REGS->MCLK_AHBMASK |= MCLK_AHBMASK_GMAC(1);
    /* Configure the APBB Bridge Clock for GMAC */
    MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_GMAC(1);

    //disable Tx
    pGmacRegs->GMAC_NCR &= ~GMAC_NCR_TXEN_Msk;
    //disable Rx
    pGmacRegs->GMAC_NCR &= ~GMAC_NCR_RXEN_Msk;

    //disable all GMAC interrupts for QUEUE 0
    pGmacRegs->GMAC_IDR = GMAC_INT_ALL;   //Clear statistics register
    pGmacRegs->GMAC_NCR |=  GMAC_NCR_CLRSTAT_Msk;
    //Clear RX Status
    pGmacRegs->GMAC_RSR =  GMAC_RSR_RXOVR_Msk | GMAC_RSR_REC_Msk | GMAC_RSR_BNA_Msk  | GMAC_RSR_HNO_Msk;
    //Clear TX Status
    pGmacRegs->GMAC_TSR = GMAC_TSR_UBR_Msk  | GMAC_TSR_COL_Msk  | GMAC_TSR_RLE_Msk | GMAC_TSR_TXGO_Msk |
                      GMAC_TSR_TFC_Msk  | GMAC_TSR_TXCOMP_Msk  | GMAC_TSR_HRESP_Msk;

    //Clear Interrupt status
    pGmacRegs->GMAC_ISR;

    //Set network configurations like speed, full duplex, copy all frames, no broadcast,
    // pause enable, remove FCS, MDC clock
    pGmacRegs->GMAC_NCFGR = GMAC_NCFGR_SPD(1) | GMAC_NCFGR_FD(1) | GMAC_NCFGR_CLK(3)  | GMAC_NCFGR_PEN(1) |
                            GMAC_NCFGR_RFCS(1) | GMAC_NCFGR_MAXFS(GMAC_MAX_RXFS) |
                            GMAC_NCFGR_RXBUFO(pMACDrv->sGmacData.dataOffset);

    if((pMACDrv->sGmacData.gmacConfig.checksumOffloadRx) != TCPIP_MAC_CHECKSUM_NONE)
    {
        pGmacRegs->GMAC_NCFGR |= GMAC_NCFGR_RXCOEN_Msk;
    }

    // Set MAC address
    (void)DRV_PIC32CGMAC_LibSetMacAddr(pMACDrv, (const uint8_t *)(pMACDrv->sGmacData.gmacConfig.macAddress.v));

    // MII mode config
    //Configure in RMII mode
    if(((uint32_t)pMACDrv->sGmacData.gmacConfig.pPhyInit->phyFlags & (uint32_t)DRV_ETHPHY_CFG_RMII) != 0U)//RMII Mode
    {
        pGmacRegs->GMAC_UR = GMAC_UR_MII(0); //initial mode set as RMII
    }
    else
    {
        pGmacRegs->GMAC_UR = GMAC_UR_MII(1); //initial mode set as MII
    }
}




/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibInitTransfer
 * Summary : Configure DMA and interrupts
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibInitTransfer(DRV_GMAC_DRIVER* pMACDrv,GMAC_QUE_LIST queueIdx)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    uint16_t wRxDescCnt_temp =      pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nRxDescCnt;
    uint16_t wTxDescCnt_temp =      pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nTxDescCnt;
    uint16_t wRxBufferSize_temp =   pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].rxBufferSize;
    uint32_t queIntEnable = 0;

    if (wRxDescCnt_temp < 1U || wTxDescCnt_temp < 1U)
    {
        return DRV_PIC32CGMAC_RES_DESC_CNT_ERR;
    }

    if ((wRxBufferSize_temp == 0U) || wRxBufferSize_temp > DRV_GMAC_RX_MAX_FRAME)
    {
        return DRV_PIC32CGMAC_RES_RX_SIZE_ERR;
    }

    if(pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].queueRxEnable == (uint8_t)true)
    {
        queIntEnable = GMAC_INT_RX_BITS;
    }
    if(pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].queueTxEnable == (uint8_t)true)
    {
        queIntEnable |= GMAC_INT_TX_BITS;
    }

    //write dma configuration to register
    pGmacRegs->GMAC_DCFGR = GMAC_DCFGR_DRBS(((uint32_t)wRxBufferSize_temp >> 6)) | GMAC_DCFGR_RXBMS(3) | GMAC_DCFGR_TXPBMS(1) |
                                      GMAC_DCFGR_FBLDO(4) | GMAC_DCFGR_DDRP(1);

    if((pMACDrv->sGmacData.gmacConfig.checksumOffloadTx) != TCPIP_MAC_CHECKSUM_NONE)
    {
        pGmacRegs->GMAC_DCFGR |= GMAC_DCFGR_TXCOEN_Msk;
    }

    //enable GMAC interrupts
    pGmacRegs->GMAC_IER = queIntEnable;

    return DRV_PIC32CGMAC_RES_OK;
}

/****************************************************************************
 * Function:        DRV_PIC32CGMAC_LibTransferEnable
 * Summary: Enable Rx and Tx of GMAC
 *****************************************************************************/
void DRV_PIC32CGMAC_LibTransferEnable (DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    // Enable Rx and Tx, plus the statistics register.
    pGmacRegs->GMAC_NCR |= GMAC_NCR_TXEN_Msk;
    pGmacRegs->GMAC_NCR |= GMAC_NCR_RXEN_Msk;
    pGmacRegs->GMAC_NCR |= GMAC_NCR_WESTAT_Msk;
}


/****************************************************************************
 * Function:        DRV_PIC32CGMAC_LibClose
 * Summary: Disable GMAC Rx, Tx and interrupts
 *****************************************************************************/
void DRV_PIC32CGMAC_LibClose(DRV_GMAC_DRIVER * pMACDrv, DRV_PIC32CGMAC_CLOSE_FLAGS cFlags)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    // disable Rx, Tx, Eth controller itself
    pGmacRegs->GMAC_NCR &= ~GMAC_NCR_TXEN_Msk;
    pGmacRegs->GMAC_NCR &= ~GMAC_NCR_RXEN_Msk;

    // Clear interrupt status
    pGmacRegs->GMAC_ISR;
}


/****************************************************************************
 * Function:        DRV_PIC32CGMAC_LibMACOpen
 * Summary : Open GMAC driver
 *****************************************************************************/
void DRV_PIC32CGMAC_LibMACOpen(DRV_GMAC_DRIVER * pMACDrv, TCPIP_ETH_OPEN_FLAGS oFlags, TCPIP_ETH_PAUSE_TYPE pauseType)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    uint32_t ncfgr;
    uint8_t queueIdx; 

    pGmacRegs->GMAC_NCR &= ~GMAC_NCR_TXEN_Msk;
    pGmacRegs->GMAC_NCR &= ~GMAC_NCR_RXEN_Msk;

    ncfgr = pGmacRegs->GMAC_NCFGR;

    if(((uint32_t)oFlags & (uint32_t)TCPIP_ETH_OPEN_FDUPLEX) != 0U)
    {
        ncfgr |= GMAC_NCFGR_FD_Msk;
    }
    else
    {
        ncfgr &= ~GMAC_NCFGR_FD_Msk;
    }

    if(((uint32_t)oFlags & (uint32_t)TCPIP_ETH_OPEN_100) != 0U)
    {
        ncfgr |= GMAC_NCFGR_SPD_Msk;
    }
    else
    {
        ncfgr &= ~GMAC_NCFGR_SPD_Msk;
    }

    if(((uint32_t)pauseType & (uint32_t)TCPIP_ETH_PAUSE_TYPE_EN_RX) != 0U)
    {
        ncfgr |= GMAC_NCFGR_PEN_Msk;
    }
    else
    {
        ncfgr &= ~GMAC_NCFGR_PEN_Msk;
    }

    pGmacRegs->GMAC_NCFGR = ncfgr;

    if(((uint32_t)oFlags & (uint32_t)TCPIP_ETH_OPEN_RMII) != 0U)
    {
        //Configure in RMII mode
        pGmacRegs->GMAC_UR = GMAC_UR_MII(0);
    }
    else
    {
        //Configure in MII mode
        pGmacRegs->GMAC_UR = GMAC_UR_MII(1);
    }
    // Reset Tx Indexes. After TXEN reset, the Transmit Queue Pointer will point to the start of the
    // transmit descriptor list.
    for(queueIdx = (uint8_t)GMAC_QUE_0; queueIdx < pMACDrv->sGmacData.gmacConfig.macQueNum; queueIdx++)
    {
        // Reset Transmit Indexes
        DRV_PIC32CGMAC_LibClearTxIndex(pMACDrv, (GMAC_QUE_LIST)queueIdx);
    }
    
    pGmacRegs->GMAC_NCR |= GMAC_NCR_RXEN_Msk;
    pGmacRegs->GMAC_NCR |= GMAC_NCR_TXEN_Msk;
}

/****************************************************************************
 * Function:        DRV_PIC32CGMAC_LibRxBuffersAppend
 * Summary : allocate and add new RX buffers to RX descriptor
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxBuffersAppend(DRV_GMAC_DRIVER* pMACDrv, GMAC_QUE_LIST queueIdx, uint16_t start_index, uint16_t nDscCnt)
{
    DRV_PIC32CGMAC_RESULT gmacRes = DRV_PIC32CGMAC_RES_OK;
    TCPIP_MAC_PACKET *  pPacket;
    uint16_t nRxDescCnt = pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nRxDescCnt;
    uint16_t desc_idx = start_index;

    while ((nDscCnt--) != 0U)
    {
        //dynamically allocate new rx packets, if number of rx packets are less than threshold
        if(((uint16_t)pMACDrv->sGmacData.gmac_queue[queueIdx].RxQueue.nNodes) < (pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nRxBuffCntThres))
        {
            gmacRes = AllocateRxPacket(pMACDrv, pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nRxBuffAllocCnt, queueIdx, GMAC_RX_DYNAMIC_BUFFERS);
            if(gmacRes == DRV_PIC32CGMAC_RES_OUT_OF_MEMORY)
            {
                pMACDrv->sGmacData.rxStat.nRxBuffNotAvailable++;
                break;
            }
        }

        if(pMACDrv->sGmacData.gmac_queue[queueIdx].pRxPckt[desc_idx] == NULL)
        {
            F_DRV_GMAC_RxLock(pMACDrv);
            pPacket = FC_LstNode2MacPkt(DRV_PIC32CGMAC_SingleListHeadRemove(&pMACDrv->sGmacData.gmac_queue[queueIdx].RxQueue));
            F_DRV_GMAC_RxUnlock(pMACDrv);
            
            if(pPacket != NULL)
            {
                uint32_t segBuffer = (uint32_t)(pPacket->pDSeg->segBuffer) & GMAC_RX_ADDRESS_MASK;   // should be 4-byte aligned                
                F_DRV_GMAC_RxLock(pMACDrv);
                /* Reset status value. */
                pMACDrv->sGmacData.gmac_queue[queueIdx].pRxDesc[desc_idx].rx_desc_status.val = 0;
                
                if (desc_idx == pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nRxDescCnt - 1U)
                {
                    pMACDrv->sGmacData.gmac_queue[queueIdx].pRxDesc[desc_idx].rx_desc_buffaddr.val =  segBuffer | GMAC_RX_WRAP_BIT;
                }
                else
                {
                    pMACDrv->sGmacData.gmac_queue[queueIdx].pRxDesc[desc_idx].rx_desc_buffaddr.val =  segBuffer;  
                }                    

                F_DRV_GMAC_RxUnlock(pMACDrv);

                // set the packet acknowledgment
                pPacket->ackFunc = (TCPIP_MAC_PACKET_ACK_FUNC)&MacRxPacketAck;
                pPacket->ackParam = pMACDrv;
                //Clear the packet flags
                pPacket->pktFlags = 0;        
                /* Save packet pointer */
                pMACDrv->sGmacData.gmac_queue[queueIdx].pRxPckt[desc_idx] = pPacket;
                pPacket->next = NULL;

                GCIRC_INC(desc_idx,nRxDescCnt );
            }   

        }

    }  
    return gmacRes;

} //DRV_PIC32CGMAC_LibRxBuffersAppend

/****************************************************************************
 * Function:        DRV_PIC32CGMAC_LibRxInit
 * Summary : initialize Rx Descriptors
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxInit(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    DRV_PIC32CGMAC_RESULT gmacRes = DRV_PIC32CGMAC_RES_OK;
    uint16_t desc_idx;
    uint8_t queue_idx;
    TCPIP_MAC_PACKET **pRxPcktAlloc;

    for(queue_idx=0; queue_idx < pMACDrv->sGmacData.gmacConfig.macQueNum; queue_idx++)
    {
        DRV_PIC32CGMAC_SingleListInitialize(&pMACDrv->sGmacData.gmac_queue[queue_idx].RxQueue);

        //allocate array of rxpckt pointer
        pRxPcktAlloc = (*pMACDrv->sGmacData.mac_callocF)(pMACDrv->sGmacData.mac_AllocH,pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queue_idx].nRxDescCnt, sizeof(TCPIP_MAC_PACKET *));
        if(pRxPcktAlloc == NULL)
        {
            // failed
            gmacRes = DRV_PIC32CGMAC_RES_OUT_OF_MEMORY;
            break;
        }
        
        (pMACDrv->sGmacData.gmac_queue[queue_idx].pRxPckt) = pRxPcktAlloc;

        for(desc_idx=0; desc_idx < pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queue_idx].nRxDescCnt; desc_idx++)
        {
            pMACDrv->sGmacData.gmac_queue[queue_idx].nRxDescIndex = 0;
            pMACDrv->sGmacData.gmac_queue[queue_idx].pRxPckt[desc_idx] = NULL;
            pMACDrv->sGmacData.gmac_queue[queue_idx].pRxDesc[desc_idx].rx_desc_buffaddr.val = 0;
            pMACDrv->sGmacData.gmac_queue[queue_idx].pRxDesc[desc_idx].rx_desc_status.val = 0;
        }
        pMACDrv->sGmacData.gmac_queue[queue_idx].pRxDesc[desc_idx-1U].rx_desc_buffaddr.val |= GMAC_RX_WRAP_BIT;

        if (pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queue_idx].queueRxEnable == (uint8_t)true)
        {
            gmacRes = AllocateRxPacket(pMACDrv, pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queue_idx].nRxDedicatedBuffers, (GMAC_QUE_LIST)queue_idx, GMAC_RX_STICKY_BUFFERS);
            if(gmacRes != DRV_PIC32CGMAC_RES_OK)
            {
                break;
            }

            gmacRes = AllocateRxPacket(pMACDrv, pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queue_idx].nRxAddlBuffCount, (GMAC_QUE_LIST)queue_idx, GMAC_RX_DYNAMIC_BUFFERS);
            if(gmacRes != DRV_PIC32CGMAC_RES_OK)
            {
                break;
            }

            gmacRes = DRV_PIC32CGMAC_LibRxBuffersAppend(pMACDrv, (GMAC_QUE_LIST)queue_idx,0,pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queue_idx].nRxDescCnt);
            if(gmacRes != DRV_PIC32CGMAC_RES_OK)
            {
                break;
            }
        }

        pGmacRegs->GMAC_RBQB = GMAC_RBQB_ADDR_Msk & ((uint32_t)pMACDrv->sGmacData.gmac_queue[queue_idx].pRxDesc);

    }

    return gmacRes;
}//DRV_PIC32CGMAC_LibRxInit

/****************************************************************************
 * Function:        DRV_PIC32CGMAC_LibRxQueFilterInit
 * Summary : initialize Rx Filters
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxQueFilterInit(DRV_GMAC_DRIVER* pMACDrv)
{
    return DRV_PIC32CGMAC_RES_OK;
}

/****************************************************************************
 * Function:        DRV_PIC32CGMAC_LibTxInit
 * Summary : initialize TX Descriptors
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibTxInit(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    uint16_t desc_idx;
    uint8_t queue_idx;
    DRV_GMAC_TX_DESC_INDEX * queue_node;

    for(queue_idx=0; queue_idx < pMACDrv->sGmacData.gmacConfig.macQueNum; queue_idx++)
    {
        //initialize Tx Queue
        DRV_PIC32CGMAC_SingleListInitialize(&pMACDrv->sGmacData.gmac_queue[queue_idx].TxQueue);        
        //initialize Queue for Pool of unused nodes for Transmit packet acknowledgment
        DRV_PIC32CGMAC_SingleListInitialize(&pMACDrv->sGmacData.gmac_queue[queue_idx].TxDescAckPoolQueue);
        //initialize Queue for Unacknowledged Transmit Descriptors
        DRV_PIC32CGMAC_SingleListInitialize(&pMACDrv->sGmacData.gmac_queue[queue_idx].TxDescUnAckQueue);
        
        for(desc_idx=0; desc_idx < pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queue_idx].nTxDescCnt; desc_idx++)
        {
            pMACDrv->sGmacData.gmac_queue[queue_idx].nTxDescHead = 0;
            pMACDrv->sGmacData.gmac_queue[queue_idx].nTxDescTail = 0;
            pMACDrv->sGmacData.gmac_queue[queue_idx].pTxDesc[desc_idx].tx_desc_buffaddr = 0;
            pMACDrv->sGmacData.gmac_queue[queue_idx].pTxDesc[desc_idx].tx_desc_status.val = GMAC_TX_USED_BIT | GMAC_TX_LAST_BUFFER_BIT;
            //Allocate memory for nodes to store Tx Descriptor Index used for transmission
            queue_node = (*pMACDrv->sGmacData.mac_callocF)(pMACDrv->sGmacData.mac_AllocH,1, sizeof(DRV_GMAC_TX_DESC_INDEX));
            DRV_PIC32CGMAC_SingleListTailAdd(&pMACDrv->sGmacData.gmac_queue[queue_idx].TxDescAckPoolQueue, FC_TxDscIdx2LstNode(queue_node)); 
                        
        }
        pMACDrv->sGmacData.gmac_queue[queue_idx].pTxDesc[desc_idx-1U].tx_desc_status.val |= GMAC_TX_WRAP_BIT;

        pGmacRegs->GMAC_TBQB = GMAC_TBQB_ADDR_Msk & ((uint32_t)pMACDrv->sGmacData.gmac_queue[queue_idx].pTxDesc);
    }
    return DRV_PIC32CGMAC_RES_OK;
}//DRV_PIC32CGMAC_LibTxInit

/****************************************************************************
 * Function:        DRV_PIC32CGMAC_LibTxSendPacket
 * Summary: Add Tx packet to Tx descriptors and trigger Tx Start
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibTxSendPacket(DRV_GMAC_DRIVER * pMACDrv, TCPIP_MAC_DATA_SEGMENT * pPktDSeg,  GMAC_QUE_LIST queueIdx)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    DRV_PIC32CGMAC_HW_TXDCPT *pTxDesc = pMACDrv->sGmacData.gmac_queue[queueIdx].pTxDesc;
    uint16_t wTxIndex = pMACDrv->sGmacData.gmac_queue[queueIdx].nTxDescHead ;
    uint16_t wTxDescCount =pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nTxDescCnt;
    uint16_t txDesc_free = 0;
    uint16_t pktSegCnt = 0;
    uint16_t nLoopCnt =0;
    DRV_PIC32CGMAC_RESULT res = DRV_PIC32CGMAC_RES_NO_DESCRIPTORS;
    DRV_PIC32CGMAC_SGL_LIST_NODE*   pTxQueueNode;
    DRV_GMAC_TX_DESC_INDEX * queue_node;
    uint16_t wStartTxIndex = wTxIndex;
    
    txDesc_free = F_DRV_GMAC_DescSpace(wTxIndex, pMACDrv->sGmacData.gmac_queue[queueIdx].nTxDescTail, wTxDescCount);
    pktSegCnt = GetPktSegCount(pPktDSeg);

    if (txDesc_free >= pktSegCnt)
    {
        while (pPktDSeg != NULL)
        {
            //check for enough number of tx descriptors available
            if(pTxDesc[wTxIndex].tx_desc_buffaddr == 0U)
            {
                //clear all Tx Status except Wrap Bit and Used Bit
                pTxDesc[wTxIndex].tx_desc_status.val &= (GMAC_TX_WRAP_BIT |GMAC_TX_USED_BIT); 
                //set the buffer address
                pTxDesc[wTxIndex].tx_desc_buffaddr = (uint32_t)((uint8_t *)pPktDSeg->segLoad); 
                //Set Length for each frame                
                pTxDesc[wTxIndex].tx_desc_status.val |= (pPktDSeg->segLen) & GMAC_LENGTH_FRAME; 
                //Increment the index of Tx Desc
                GCIRC_INC(wTxIndex,wTxDescCount); 
                pPktDSeg = pPktDSeg->next;
                nLoopCnt++;
            }
            else
            {
                //not enough descriptors available; add back the packet to TxQueue head
                return DRV_PIC32CGMAC_RES_NO_DESCRIPTORS;
            }
        }
        //Update new Tx Head Index
        pMACDrv->sGmacData.gmac_queue[queueIdx].nTxDescHead = wTxIndex;

        //Set Last Buffer bit for the last descriptor of the packet frame
        GCIRC_DEC((wTxIndex),(wTxDescCount));
        pTxDesc[wTxIndex].tx_desc_status.val |= GMAC_TX_LAST_BUFFER_BIT;

        pTxQueueNode = DRV_PIC32CGMAC_SingleListHeadRemove(&pMACDrv->sGmacData.gmac_queue[queueIdx].TxDescAckPoolQueue);
        queue_node = FC_LstNode2TxDscIdx(pTxQueueNode);
        queue_node->buffer_count = nLoopCnt;
        queue_node->endIndex = wTxIndex;
        queue_node->startIndex = wStartTxIndex;
        //Clear the 'Used' bit of descriptors in reverse order to avoid race condition
        while(nLoopCnt != 0U)
        {
            pTxDesc[wTxIndex].tx_desc_status.val &= ~GMAC_TX_USED_BIT;
            GCIRC_DEC((wTxIndex),(wTxDescCount));
            nLoopCnt--;
        }
        DRV_PIC32CGMAC_SingleListTailAdd(&pMACDrv->sGmacData.gmac_queue[queueIdx].TxDescUnAckQueue, (DRV_PIC32CGMAC_SGL_LIST_NODE*)pTxQueueNode);
        //Enable Transmission
        pGmacRegs->GMAC_NCR |= GMAC_NCR_TSTART_Msk;

        res = DRV_PIC32CGMAC_RES_OK;
    }

    return res;
} //DRV_PIC32CGMAC_LibTxSendPacket

/****************************************************************************
 * Function:        DRV_PIC32CGMAC_LibTxAckPacket
 *
 * PreCondition:    None
 *
 * Input:           pMACDrv    - GMAC driver instance
 *                  queueIdx   - GMAc queue index
 *
 * Output:          DRV_PIC32CGMAC_RES_OK - success
 *                  DRV_PIC32CGMAC_RES_DESC_CNT_ERR   - descriptor error
 *
 * Side Effects:    None
 *
 * Overview:        This function acknowledges a packet.
 *                  The supplied packet has to have been completed otherwise the call will fail.
 *
 *
 * Note:            None
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibTxAckPacket(DRV_GMAC_DRIVER * pMACDrv, GMAC_QUE_LIST queueIdx)  
{
    DRV_PIC32CGMAC_HW_TXDCPT *pTxDesc = pMACDrv->sGmacData.gmac_queue[queueIdx].pTxDesc;
    TCPIP_MAC_PACKET* pPkt = NULL;

    uint8_t* pbuff = NULL;    
    uint16_t buffer_count = 0;
    uint16_t buffer_start_index = 0;
    uint16_t buffer_end_index = 0;

    DRV_PIC32CGMAC_RESULT res = DRV_PIC32CGMAC_RES_NO_PACKET;
    DRV_GMAC_TX_DESC_INDEX * pTxAckQueueNode;
        
    while(true)    
    {
        F_DRV_GMAC_TxLock(pMACDrv); 
        pTxAckQueueNode = FC_LstNode2TxDscIdx(DRV_PIC32CGMAC_SingleListHeadRemove(&pMACDrv->sGmacData.gmac_queue[queueIdx].TxDescUnAckQueue));
        F_DRV_GMAC_TxUnlock(pMACDrv);
        
        if(pTxAckQueueNode == NULL )
        {
            break;
        }
        //remove head of TxDescUnAckQueue queue if USED bit is set.        
        //Check head of TxDescUnAckQueue queue start index descriptor 'USED' bit is set?
        if((((pTxDesc[pTxAckQueueNode->startIndex].tx_desc_status.val) & GMAC_TX_USED_BIT) != 0U) && (pTxDesc[pTxAckQueueNode->startIndex].tx_desc_buffaddr != 0U))
        {            
            //get buffer count
            buffer_count = pTxAckQueueNode->buffer_count;
            //get start index
            buffer_start_index = pTxAckQueueNode->startIndex;        
            //get end index
            buffer_end_index = pTxAckQueueNode->endIndex;

            //get pPkt from descriptor with start index
            // get aligned buffer address from Tx Descriptor Buffer Address
            pbuff = (uint8_t*)((uint32_t)pTxDesc[buffer_start_index].tx_desc_buffaddr & pMACDrv->sGmacData.dataOffsetMask);
            // get packet address from buffer address
            TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = FC_Ptr8toSegGapDsc(pbuff + pMACDrv->sGmacData.dcptOffset);
            // keep this packet address for acknowledgment as first buffer of frame has parent pkt address
            pPkt = pGap->segmentPktPtr;

            while((buffer_count--) != 0U)
            {
                // 'Used' bit only set for first buffer of frame after successful transmission; set it for all buffers
                pTxDesc[buffer_start_index].tx_desc_status.val |= GMAC_TX_USED_BIT;
                // clear all Tx status other than Used bit, Wrap bit and Last Buffer bit
                pTxDesc[buffer_start_index].tx_desc_status.val &= GMAC_TX_USED_BIT | GMAC_TX_WRAP_BIT | GMAC_TX_LAST_BUFFER_BIT;
                // clear Tx buffer address
                pTxDesc[buffer_start_index].tx_desc_buffaddr = 0;
                GCIRC_INC(buffer_start_index, pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nTxDescCnt);                
            }

            pPkt->pktFlags &= ~(uint32_t)TCPIP_MAC_PKT_FLAG_QUEUED;
            // Tx Callback
            (pMACDrv->sGmacData.pktAckF)(pPkt, TCPIP_MAC_PKT_ACK_TX_OK, TCPIP_THIS_MODULE_ID);              
            pMACDrv->sGmacData.txStat.nTxOkPackets++;
            // Clear 'Last Buffer' bit
            pTxDesc[buffer_end_index].tx_desc_status.val &= ~GMAC_TX_LAST_BUFFER_BIT;
            // save tailIndex
            pMACDrv->sGmacData.gmac_queue[queueIdx].nTxDescTail = buffer_end_index;         
            //memory barrier
            __DMB();
            res = DRV_PIC32CGMAC_RES_OK;
            
            // add the node to unused queue after clearing the values
            pTxAckQueueNode->buffer_count = 0;
            pTxAckQueueNode->startIndex = pTxAckQueueNode->endIndex = 0;  
            
            F_DRV_GMAC_TxLock(pMACDrv); 
            DRV_PIC32CGMAC_SingleListTailAdd(&pMACDrv->sGmacData.gmac_queue[queueIdx].TxDescAckPoolQueue, FC_TxDscIdx2LstNode(pTxAckQueueNode)); 
            F_DRV_GMAC_TxUnlock(pMACDrv);
        }
        else
        {
            F_DRV_GMAC_TxLock(pMACDrv); 
            // add the node back to queue as transmit not yet completed
            DRV_PIC32CGMAC_SingleListHeadAdd(&pMACDrv->sGmacData.gmac_queue[queueIdx].TxDescUnAckQueue, FC_TxDscIdx2LstNode(pTxAckQueueNode)); 
            F_DRV_GMAC_TxUnlock(pMACDrv);
            break;
        }    
        
    }       
    return res; 
} //DRV_PIC32CGMAC_LibTxAckPacket

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibTxAckPendPacket
 * Summary : Acknowledge pending packets in transmit queue
 *****************************************************************************/
void DRV_PIC32CGMAC_LibTxAckPendPacket( DRV_GMAC_DRIVER * pMACDrv, GMAC_QUE_LIST queueIdx, TCPIP_MAC_PKT_ACK_RES ackRes)
{
    TCPIP_MAC_PACKET* pPkt;

    //packet in queue for transmission
    while(true)
    {
        F_DRV_GMAC_TxLock(pMACDrv); 
        pPkt = FC_LstNode2MacPkt(DRV_PIC32CGMAC_SingleListHeadRemove(&pMACDrv->sGmacData.gmac_queue[queueIdx].TxQueue));
        F_DRV_GMAC_TxUnlock(pMACDrv);
        if(pPkt == NULL)
        {
            break;
        }
        // release pending list packets
        if(pMACDrv->sGmacData.pktAckF != NULL)
        {   // Tx Callback
            (*pMACDrv->sGmacData.pktAckF)(pPkt, ackRes, TCPIP_THIS_MODULE_ID);
        }

    }
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibTxAckErrPacket
 * Summary : Acknowledge packets in transmit descriptors during transmit errors
 *****************************************************************************/
void DRV_PIC32CGMAC_LibTxAckErrPacket( DRV_GMAC_DRIVER * pMACDrv, GMAC_QUE_LIST queueIdx, TCPIP_MAC_PKT_ACK_RES ackRes)
{
    TCPIP_MAC_PACKET* pPkt;
    DRV_PIC32CGMAC_HW_TXDCPT *pTxDesc = pMACDrv->sGmacData.gmac_queue[queueIdx].pTxDesc;
    uint16_t tailIndex = pMACDrv->sGmacData.gmac_queue[queueIdx].nTxDescTail;
    uint16_t headIndex = pMACDrv->sGmacData.gmac_queue[queueIdx].nTxDescHead;
    uint8_t* pbuff = NULL;

    while(tailIndex != headIndex)
    {
        // get aligned buffer address from Tx Descriptor Buffer Address
        pbuff = (uint8_t*)((uint32_t)pTxDesc[tailIndex].tx_desc_buffaddr & pMACDrv->sGmacData.dataOffsetMask);
        // get packet address from buffer address
        TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = FC_Ptr8toSegGapDsc(pbuff + pMACDrv->sGmacData.dcptOffset);
        pPkt = pGap->segmentPktPtr;

        pPkt->pktFlags &= ~(uint32_t)TCPIP_MAC_PKT_FLAG_QUEUED;
        // Tx Callback
        (pMACDrv->sGmacData.pktAckF)(pPkt, ackRes, TCPIP_THIS_MODULE_ID);

        // 'Used' bit only set for first buffer of frame after successful transmission; set it for all buffers
        pTxDesc[tailIndex].tx_desc_status.val &= GMAC_TX_WRAP_BIT;
        pTxDesc[tailIndex].tx_desc_status.val |= GMAC_TX_USED_BIT | GMAC_TX_LAST_BUFFER_BIT;
        pTxDesc[tailIndex].tx_desc_buffaddr = 0;
        //increment the tail for the next packet processing
        GCIRC_INC(tailIndex, pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nTxDescCnt);
    }
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibTxClearUnAckPacket
 * Summary : Acknowledge the Unacknowledged Transmit packets during transmit errors
 *****************************************************************************/
void DRV_PIC32CGMAC_LibTxClearUnAckPacket( DRV_GMAC_DRIVER * pMACDrv, GMAC_QUE_LIST queueIdx, TCPIP_MAC_PKT_ACK_RES ackRes)
{
    DRV_PIC32CGMAC_HW_TXDCPT *pTxDesc = pMACDrv->sGmacData.gmac_queue[queueIdx].pTxDesc;
    TCPIP_MAC_PACKET* pPkt = NULL;
    uint8_t* pbuff = NULL;    
    uint16_t buffer_count = 0;
    uint16_t buffer_start_index = 0;
    uint16_t buffer_end_index = 0;
    DRV_GMAC_TX_DESC_INDEX * pTxAckQueueNode;
    
    while(true)    
    {        
        F_DRV_GMAC_TxLock(pMACDrv);
        pTxAckQueueNode = FC_LstNode2TxDscIdx(DRV_PIC32CGMAC_SingleListHeadRemove(&pMACDrv->sGmacData.gmac_queue[queueIdx].TxDescUnAckQueue));        
        F_DRV_GMAC_TxUnlock(pMACDrv);
        
        if(pTxAckQueueNode == NULL )
        {
            break;
        }
        //get buffer count
        buffer_count = pTxAckQueueNode->buffer_count;
        //get start index
        buffer_start_index = pTxAckQueueNode->startIndex;        
        //get end index
        buffer_end_index = pTxAckQueueNode->endIndex;

        //get pPkt from descriptor with start index
        // get aligned buffer address from Tx Descriptor Buffer Address
        pbuff = (uint8_t*)((uint32_t)pTxDesc[buffer_start_index].tx_desc_buffaddr & pMACDrv->sGmacData.dataOffsetMask);
        // get packet address from buffer address
        TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = FC_Ptr8toSegGapDsc(pbuff + pMACDrv->sGmacData.dcptOffset);
        // keep this packet address for acknowledgment as first buffer of frame has parent pkt address
        pPkt = pGap->segmentPktPtr;

        while((buffer_count--) != 0U)
        {
            // 'Used' bit only set for first buffer of frame after successful transmission; set it for all buffers
            pTxDesc[buffer_start_index].tx_desc_status.val |= GMAC_TX_USED_BIT;
            // clear all Tx status other than Used bit, Wrap bit and Last Buffer bit
            pTxDesc[buffer_start_index].tx_desc_status.val &= GMAC_TX_USED_BIT | GMAC_TX_WRAP_BIT | GMAC_TX_LAST_BUFFER_BIT;
            // clear Tx buffer address
            pTxDesc[buffer_start_index].tx_desc_buffaddr = 0;
            GCIRC_INC(buffer_start_index, pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nTxDescCnt);                
        }

        pPkt->pktFlags &= ~(uint32_t)TCPIP_MAC_PKT_FLAG_QUEUED;
        // Tx Callback
        (pMACDrv->sGmacData.pktAckF)(pPkt, ackRes, TCPIP_THIS_MODULE_ID);               
        pMACDrv->sGmacData.txStat.nTxErrorPackets++;
        // Clear 'Last Buffer' bit
        pTxDesc[buffer_end_index].tx_desc_status.val &= ~GMAC_TX_LAST_BUFFER_BIT;
        // save tailIndex
        pMACDrv->sGmacData.gmac_queue[queueIdx].nTxDescTail = buffer_end_index;     
        //memory barrier
        __DMB();

        // add the node to unused queue after clearing the values
        pTxAckQueueNode->buffer_count = 0;
        pTxAckQueueNode->startIndex = pTxAckQueueNode->endIndex = 0;  
        F_DRV_GMAC_TxLock(pMACDrv); 
        DRV_PIC32CGMAC_SingleListTailAdd(&pMACDrv->sGmacData.gmac_queue[queueIdx].TxDescAckPoolQueue, FC_TxDscIdx2LstNode(pTxAckQueueNode)); 
        F_DRV_GMAC_TxUnlock(pMACDrv);
            
    }
        
}
/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibRxGetPacket
 * Summary: extract  RX packet from the RX descriptor
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxGetPacket(DRV_GMAC_DRIVER * pMACDrv, TCPIP_MAC_PACKET** pRxPkt, int* pnBuffs, TCPIP_MAC_PACKET_RX_STAT_PIC32C* pRxStat, GMAC_QUE_LIST queueIdx)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    DRV_PIC32CGMAC_RX_FRAME_INFO rx_frame_state = {0,0,0};
    DRV_PIC32CGMAC_RESULT   res = DRV_PIC32CGMAC_RES_NO_PACKET;
    GMAC_RXFRAME_STATE  frameState = GMAC_RX_NO_FRAME_STATE;
    bool bna_flag = false;

    //Check for BNA error due to shortage of Rx Buffers
    bna_flag = IsBufferNotAvailable(pMACDrv);

    if(pRxPkt == NULL)
    {
        return res;
    }

    //Search all Rx descriptors for a valid rx packet
    frameState = SearchRxPacket(pMACDrv, &rx_frame_state, queueIdx);

    //Valid packet detected with SOF and EOF
    if (frameState == GMAC_RX_VALID_FRAME_DETECTED_STATE)
    {
        //pass the packet status to upper layer
        *pRxStat = (pMACDrv->sGmacData.gmac_queue[queueIdx].pRxDesc[rx_frame_state.endIndex].rx_desc_status);
        //number of buffers to return
        *pnBuffs = (int)rx_frame_state.buffer_count;
        // extract Rx packet and pass it to upper layer
        res = GetRxPacket(pMACDrv, &rx_frame_state, pRxPkt, queueIdx);
    }

    if(bna_flag == true)
    {
        // Clear Buffer Not Available Flag
        pGmacRegs->GMAC_RSR = GMAC_RSR_BNA_Msk ;
    }

    return res;

} //DRV_PIC32CGMAC_LibRxGetPacket

/*****************************************************************************
 ********************** Generic Single Linked List Manipulation **************
 *****************************************************************************/

// removes the head node
DRV_PIC32CGMAC_SGL_LIST_NODE*  DRV_PIC32CGMAC_SingleListHeadRemove(DRV_PIC32CGMAC_SGL_LIST* pL)
{
    DRV_PIC32CGMAC_SGL_LIST_NODE* pN = pL->head;
    if(pN != NULL)
    {
        if(pL->head == pL->tail)
        {
            pL->head = pL->tail = NULL;
        }
        else
        {
            pL->head = pN->next;
        }
        pL->nNodes--;
    }

    return pN;
}

// adds node to tail
void  DRV_PIC32CGMAC_SingleListTailAdd(DRV_PIC32CGMAC_SGL_LIST* pL, DRV_PIC32CGMAC_SGL_LIST_NODE* pN)
{
    pN->next = NULL;
    if(pL->tail == NULL)
    {
        pL->head = pL->tail = pN;
    }
    else
    {
        pL->tail->next = pN;
        pL->tail = pN;
    }
    pL->nNodes++;
}

// adds node to tail
void  DRV_PIC32CGMAC_SingleListHeadAdd(DRV_PIC32CGMAC_SGL_LIST* pL, DRV_PIC32CGMAC_SGL_LIST_NODE* pN)
{

    if(pN != NULL)
    {
        if(pL->head == NULL)
        {
            pL->head = pL->tail = pN;
        }
        else
        {
            pN->next = pL->head;
            pL->head = pN;
        }
        pL->nNodes++;
    }

}

/****************************************************************************
 * Function:    DRV_GMAC_LibDescriptorsPoolAdd
 * Summary :    Add Descriptor Pool
 *****************************************************************************/
void DRV_GMAC_LibDescriptorsPoolAdd (DRV_GMAC_DRIVER * pMACDrv, DRV_GMAC_DCPT_TYPE dType)
{
    if(pMACDrv->sGmacData.gmac_queue == NULL)
    {
        pMACDrv->sGmacData.gmac_queue = s_gmac_queue;
    }

    if(dType == DRV_GMAC_DCPT_TYPE_TX)
    {
        pMACDrv->sGmacData.gmac_queue[0].pTxDesc = gmac_dcpt_array.sTxDesc_queue0;

    }
    else if(dType == DRV_GMAC_DCPT_TYPE_RX)
    {
        pMACDrv->sGmacData.gmac_queue[0].pRxDesc = gmac_dcpt_array.sRxDesc_queue0;

    }
    else
    {
        //Do nothing
    }

}

/****************************************************************************
 * Function:    DRV_PIC32CGMAC_LibRxFilterHash_Calculate
 * Summary :    calculate the hash value for given mac address and set the hash value in GMAC register
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxFilterHash_Calculate(DRV_GMAC_DRIVER* pMACDrv, DRV_GMAC_HASH *hash)
{
    uint64_t hash_value;

    DRV_GMAC_HASH_INDEX hash_Index;
    DRV_GMAC_MAC_ADDR *mac_addr;

    if(hash->calculate_hash == (uint8_t)true) // Calculate hash for given MAC address
    {
        mac_addr = FC_MacAddr2GmacAddr(hash->DestMACAddr);

        hash_Index.index = 0;
        hash_Index.bits.b0 = (uint8_t)(((uint32_t)mac_addr[5].bits.b0)^((uint32_t)mac_addr[5].bits.b6)^((uint32_t)mac_addr[4].bits.b4)^((uint32_t)mac_addr[3].bits.b2)^((uint32_t)mac_addr[2].bits.b0)^((uint32_t)mac_addr[2].bits.b6)^((uint32_t)mac_addr[1].bits.b4)^((uint32_t)mac_addr[0].bits.b2));
        hash_Index.bits.b1 = (uint8_t)(((uint32_t)mac_addr[5].bits.b1)^((uint32_t)mac_addr[5].bits.b7)^((uint32_t)mac_addr[4].bits.b5)^((uint32_t)mac_addr[3].bits.b3)^((uint32_t)mac_addr[2].bits.b1)^((uint32_t)mac_addr[2].bits.b7)^((uint32_t)mac_addr[1].bits.b5)^((uint32_t)mac_addr[0].bits.b3));
        hash_Index.bits.b2 = (uint8_t)(((uint32_t)mac_addr[5].bits.b2)^((uint32_t)mac_addr[4].bits.b0)^((uint32_t)mac_addr[4].bits.b6)^((uint32_t)mac_addr[3].bits.b4)^((uint32_t)mac_addr[2].bits.b2)^((uint32_t)mac_addr[1].bits.b0)^((uint32_t)mac_addr[1].bits.b6)^((uint32_t)mac_addr[0].bits.b4));
        hash_Index.bits.b3 = (uint8_t)(((uint32_t)mac_addr[5].bits.b3)^((uint32_t)mac_addr[4].bits.b1)^((uint32_t)mac_addr[4].bits.b7)^((uint32_t)mac_addr[3].bits.b5)^((uint32_t)mac_addr[2].bits.b3)^((uint32_t)mac_addr[1].bits.b1)^((uint32_t)mac_addr[1].bits.b7)^((uint32_t)mac_addr[0].bits.b5));
        hash_Index.bits.b4 = (uint8_t)(((uint32_t)mac_addr[5].bits.b4)^((uint32_t)mac_addr[4].bits.b2)^((uint32_t)mac_addr[3].bits.b0)^((uint32_t)mac_addr[3].bits.b6)^((uint32_t)mac_addr[2].bits.b4)^((uint32_t)mac_addr[1].bits.b2)^((uint32_t)mac_addr[0].bits.b0)^((uint32_t)mac_addr[0].bits.b6));
        hash_Index.bits.b5 = (uint8_t)(((uint32_t)mac_addr[5].bits.b5)^((uint32_t)mac_addr[4].bits.b3)^((uint32_t)mac_addr[3].bits.b1)^((uint32_t)mac_addr[3].bits.b7)^((uint32_t)mac_addr[2].bits.b5)^((uint32_t)mac_addr[1].bits.b3)^((uint32_t)mac_addr[0].bits.b1)^((uint32_t)mac_addr[0].bits.b7));

        // read the current hash value stored in register
        hash_value = F_DRV_GMAC_HashValueGet(pMACDrv);
        hash_value  |= ((uint64_t)1UL << (hash_Index.index));
    }
    else
    {   // Set hash value directly
        hash_value = hash->hash_value;
    }
    F_DRV_GMAC_HashValueSet(pMACDrv, hash_value);

    return DRV_PIC32CGMAC_RES_OK;
}

/****************************************************************************
 * Function:    DRV_PIC32CGMAC_LibSetMacAddr
 * Summary :    Set MAC Address
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibSetMacAddr (DRV_GMAC_DRIVER* pMACDrv, const uint8_t * pMacAddr)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    pGmacRegs->SA[0].GMAC_SAB = ((uint32_t)pMacAddr[3] << 24)
                              | ((uint32_t)pMacAddr[2] << 16)
                              | ((uint32_t)pMacAddr[1] <<  8)
                              | ((uint32_t)pMacAddr[0]);

    pGmacRegs->SA[0].GMAC_SAT = ((uint32_t)pMacAddr[5] <<  8)
                                | ((uint32_t)pMacAddr[4]) ;

    return DRV_PIC32CGMAC_RES_OK;
}

/****************************************************************************
 * Function:    DRV_PIC32CGMAC_LibGetMacAddr
 * Summary :    Get MAC Address
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibGetMacAddr (DRV_GMAC_DRIVER* pMACDrv, uint8_t * pMacAddr)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;

    pMacAddr[0] = (uint8_t)((pGmacRegs->SA[0].GMAC_SAB)& (uint32_t)0xFFU);
    pMacAddr[1] = (uint8_t)(((pGmacRegs->SA[0].GMAC_SAB)>>8)& (uint32_t)0xFFU);
    pMacAddr[2] = (uint8_t)(((pGmacRegs->SA[0].GMAC_SAB)>>16)& (uint32_t)0xFFU);
    pMacAddr[3] = (uint8_t)(((pGmacRegs->SA[0].GMAC_SAB)>>24)& (uint32_t)0xFFU);
    pMacAddr[4] = (uint8_t)((pGmacRegs->SA[0].GMAC_SAT)& (uint32_t)0xFFU);
    pMacAddr[5] = (uint8_t)(((pGmacRegs->SA[0].GMAC_SAT)>>8)& (uint32_t)0xFFU);

    return DRV_PIC32CGMAC_RES_OK;
}

/****************************************************************************
 * Function:    DRV_PIC32CGMAC_LibRxBuffersCountGet
 * Summary :    Returns the number of pending RX buffers and scheduled buffers in the GMAC queues.
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxBuffersCountGet(DRV_GMAC_DRIVER* pMACDrv, uint32_t* pendBuffs, uint32_t* schedBuffs)
{
    uint32_t pend_buffer_cnt = 0;
    uint32_t sched_buffer_cnt = 0;

    for(uint16_t queue_idx=0; queue_idx < pMACDrv->sGmacData.gmacConfig.macQueNum; queue_idx++)
    {
        if(pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queue_idx].queueRxEnable == (uint8_t)true)
        {
            for(uint16_t desc_idx=0; desc_idx < pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queue_idx].nRxDescCnt; desc_idx++)
            {
                if (((pMACDrv->sGmacData.gmac_queue[queue_idx].pRxDesc[desc_idx].rx_desc_buffaddr.val) & GMAC_RX_ADDRESS_MASK) != 0U)
                {
                    if (((pMACDrv->sGmacData.gmac_queue[queue_idx].pRxDesc[desc_idx].rx_desc_buffaddr.val) & GMAC_RX_OWNERSHIP_BIT) != 0U)
                    {
                        pend_buffer_cnt++;
                    }
                    else
                    {
                        sched_buffer_cnt++;
                    }
                }
            }
        }
    }

    if(pendBuffs != NULL)
    {
        *pendBuffs = pend_buffer_cnt;
    }
    if(schedBuffs != NULL)
    {
        *schedBuffs = sched_buffer_cnt;
    }
    return DRV_PIC32CGMAC_RES_OK;
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibSetPriorityToQueueNum
 * Summary: mapping priority to GMAC Queue number
 *****************************************************************************/
bool DRV_PIC32CGMAC_LibSetPriorityToQueueNum(DRV_GMAC_DRIVER* pMACDrv)
{
    pMACDrv->sGmacData.gmacConfig.txPrioNumToQueIndx[0] = (uint8_t)GMAC_QUE_0;
    pMACDrv->sGmacData.gmacConfig.rxPrioNumToQueIndx[0] = (uint8_t)GMAC_QUE_0;

    // successful
    return true;
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibGetPriorityFromQueueNum
 * Summary: retrieve priority number from queue index
 *****************************************************************************/
uint8_t DRV_PIC32CGMAC_LibGetPriorityFromQueueNum(DRV_GMAC_DRIVER* pMACDrv, GMAC_QUE_LIST queueIdx)
{
    return (uint8_t)queueIdx;
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibGetHighPrioReadyQue
 * Summary: Return the highest priority queue ready
 *****************************************************************************/
uint8_t DRV_PIC32CGMAC_LibGetHighPrioReadyQue(DRV_GMAC_DRIVER* pMACDrv)
{
    if((drvGmacQueEvents & GMAC_QUE0_MASK) != 0U)
    {
        return (uint8_t)GMAC_QUE_0;
    }
    return (uint8_t)DRV_GMAC_NO_ACTIVE_QUEUE;
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibClearPriorityQue
 * Summary: Clear the ready status of  priority queue
 *****************************************************************************/
void DRV_PIC32CGMAC_LibClearPriorityQue(DRV_GMAC_DRIVER *pMACDrv, GMAC_QUE_LIST queueIdx)
{
    bool intStat;
    intStat = SYS_INT_SourceDisable(pMACDrv->sGmacData.gmacConfig.gmac_queue_config[GMAC_QUE_0].queueIntSrc);
    drvGmacQueEvents &= ~(uint32_t)(1UL<<(uint8_t)queueIdx);
    SYS_INT_SourceRestore(pMACDrv->sGmacData.gmacConfig.gmac_queue_config[GMAC_QUE_0].queueIntSrc, intStat);
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibSysInt_Disable
 * Summary: Disable all GMAC interrupts specified in queue mask
 *****************************************************************************/
void DRV_PIC32CGMAC_LibSysInt_Disable(DRV_GMAC_DRIVER *pMACDrv, uint32_t queMask, bool *queStat)
{
    if((queMask & GMAC_QUE0_MASK) != 0U)
    {
            if(queStat != NULL)
        {
            queStat[GMAC_QUE_0] = SYS_INT_SourceDisable(pMACDrv->sGmacData.gmacConfig.gmac_queue_config[GMAC_QUE_0].queueIntSrc);
        }
        else
        {
            (void)SYS_INT_SourceDisable(pMACDrv->sGmacData.gmacConfig.gmac_queue_config[GMAC_QUE_0].queueIntSrc);
        }

    }
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibSysIntStatus_Clear
 * Summary: Clear all GMAC interrupts status specified in queue mask
 *****************************************************************************/
void DRV_PIC32CGMAC_LibSysIntStatus_Clear(DRV_GMAC_DRIVER *pMACDrv, uint32_t queMask)
{
    if((queMask & GMAC_QUE0_MASK) != 0U)
    {
        SYS_INT_SourceStatusClear(pMACDrv->sGmacData.gmacConfig.gmac_queue_config[GMAC_QUE_0].queueIntSrc);
    }
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibSysInt_Enable
 * Summary: Enable all GMAC interrupts specified in queue mask
 *****************************************************************************/
void DRV_PIC32CGMAC_LibSysInt_Enable(DRV_GMAC_DRIVER *pMACDrv, uint32_t queMask)
{
    if((queMask & GMAC_QUE0_MASK) != 0U)
    {
        SYS_INT_SourceEnable(pMACDrv->sGmacData.gmacConfig.gmac_queue_config[GMAC_QUE_0].queueIntSrc);
    }
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibSysInt_Restore
 * Summary: Restore all GMAC interrupts specified in queue mask
 *****************************************************************************/
void DRV_PIC32CGMAC_LibSysInt_Restore(DRV_GMAC_DRIVER *pMACDrv, uint32_t queMask, bool *queStat)
{
    if((queMask & GMAC_QUE0_MASK) != 0U)
    {
        if(queStat != NULL)
        {
            SYS_INT_SourceRestore(pMACDrv->sGmacData.gmacConfig.gmac_queue_config[GMAC_QUE_0].queueIntSrc, queStat[GMAC_QUE_0]);
        }
    }
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibReadInterruptStatus
 * Summary: read GMAC interrupt status
 *****************************************************************************/
uint32_t DRV_PIC32CGMAC_LibReadInterruptStatus(DRV_GMAC_DRIVER *pMACDrv, GMAC_QUE_LIST queueIdx)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_ISR;
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibSysInt_Restore
 * Summary: enable GMAC interrupt events
 *****************************************************************************/
void DRV_PIC32CGMAC_LibEnableInterrupt(DRV_GMAC_DRIVER *pMACDrv, GMAC_QUE_LIST queueIdx, GMAC_EVENTS ethEvents)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    pGmacRegs->GMAC_IER = (uint32_t)ethEvents;
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibSysInt_Restore
 * Summary: disable GMAC interrupt events
 *****************************************************************************/
void DRV_PIC32CGMAC_LibDisableInterrupt(DRV_GMAC_DRIVER *pMACDrv, GMAC_QUE_LIST queueIdx, GMAC_EVENTS ethEvents)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    pGmacRegs->GMAC_IDR = (uint32_t)ethEvents;
}
/****************************************************************************
 * GMAC Interrupt Service Routines(ISR)
 *****************************************************************************/
//GMAC interrupt handler for Priority Queue 0
void GMAC_InterruptHandler(void)
{
    GMAC_EVENTS  currEthEvents = (GMAC_EVENTS)GMAC_REGS->GMAC_ISR;
    if(((uint32_t)currEthEvents & (uint32_t)GMAC_EV_RX_ALL) != 0U)
    {
        drvGmacQueEvents |= GMAC_QUE0_MASK;
    }
  DRV_GMAC_Tasks_ISR((SYS_MODULE_OBJ)0, (uint32_t)currEthEvents);
}

/****************************************************************************
 * Function:        MacRxPacketAck
 * Summary: ACK function to free the RX packet
 *****************************************************************************/
static void MacRxPacketAck(TCPIP_MAC_PACKET* pPkt,  const void* param)
{
    TCPIP_MAC_DATA_SEGMENT *    pDSegNext;
    DRV_GMAC_DRIVER * pMacDrv = FC_Prm2GmacDrv(param);
    //hard-coded for queue 0
    GMAC_QUE_LIST queueIdx = GMAC_QUE_0;

    if((pPkt != NULL) && (pPkt->pDSeg != NULL))
    {
        while((pPkt->pDSeg) != NULL)
        {
            pDSegNext = pPkt->pDSeg->next;
            pPkt->pDSeg->next = NULL;

            // for dynamic packets(non-sticky):
            // always free if NO_SMART_ALLOC flag is set
            // free if RX packets greater than the defined threshold
            if(((pPkt->pDSeg->segFlags  & (uint16_t)TCPIP_MAC_SEG_FLAG_RX_STICKY) == 0U) &&
                (((pMacDrv->sGmacData.mac_controlFlags & (uint16_t)TCPIP_MAC_CONTROL_NO_SMART_ALLOC) != 0U) ||
                    ((uint16_t)pMacDrv->sGmacData.gmac_queue[queueIdx].RxQueue.nNodes >
                        pMacDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nRxBuffCntThres)))
            {
                (*pMacDrv->sGmacData.pktFreeF)(pPkt);
            }
            else
            {
                F_DRV_GMAC_RxLock(pMacDrv); 
                // add the packet to new queue for re-use
                DRV_PIC32CGMAC_SingleListTailAdd(&pMacDrv->sGmacData.gmac_queue[queueIdx].RxQueue, FC_MacPkt2LstNode(pPkt));
                F_DRV_GMAC_RxUnlock(pMacDrv);
            }

            if( pDSegNext == NULL)
            {   // packet handling is now complete
                break;
            }
            // Ethernet packet stored in multiple MAC descriptors, each segment
            // is allocated as a complete mac packet
            // extract the packet pointer using the segment load buffer
            TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = FC_Ptr8toSegGapDsc(pDSegNext->segBuffer + pMacDrv->sGmacData.dcptOffset);
            pPkt = pGap->segmentPktPtr;
        }
    }
}

/****************************************************************************
 * Function:        IsBufferNotAvailable
 * Summary: Check if 'Buffer Not Available' set by GMAC; if set, clear the BNA status bit
 * Return : True if BNA is set; False for no BNA status
 *****************************************************************************/
static bool IsBufferNotAvailable(DRV_GMAC_DRIVER * pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    //Check for BNA error due to shortage of Rx Buffers
    if((pGmacRegs->GMAC_RSR & GMAC_RSR_BNA_Msk ) != 0U) 
    {
        //Clear GMAC 'Buffer Not Available' Flag
        pGmacRegs->GMAC_RSR = GMAC_RSR_BNA_Msk ;
        return true;
    }
    else
    {
        return false;
    }

}

/****************************************************************************
 * Function:        _SearchRxPacket
 * Summary: Search Rx descriptors for valid Rx data packet; identify start and
 * end rx descriptor indexes of valid rx data packet
 * Return : frame search status
 *****************************************************************************/
static GMAC_RXFRAME_STATE SearchRxPacket(DRV_GMAC_DRIVER * pMACDrv,DRV_PIC32CGMAC_RX_FRAME_INFO *rx_frame_state, GMAC_QUE_LIST queueIdx)
{
    DRV_PIC32CGMAC_HW_RXDCPT *pRxDesc = pMACDrv->sGmacData.gmac_queue[queueIdx].pRxDesc;
    TCPIP_MAC_PACKET* pRxPkt;
    GMAC_RXFRAME_STATE frameState = GMAC_RX_NO_FRAME_STATE;
    uint16_t search_count = 0;
    uint16_t nRxDscCnt =  pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nRxDescCnt;
    uint16_t nRxDescIndex = pMACDrv->sGmacData.gmac_queue[queueIdx].nRxDescIndex;
    uint16_t rx_index;
    uint16_t nRx_buffer;

    //search the descriptors for valid data frame; search maximum of descriptor count
    while ( search_count < nRxDscCnt)
    {
        //Rx Descriptors with Ownership bit Set? i.e. software owned descriptor?
        if ((((uint32_t)(pRxDesc[nRxDescIndex].rx_desc_buffaddr.val) & GMAC_RX_OWNERSHIP_BIT) == GMAC_RX_OWNERSHIP_BIT))
        {
            //valid buffer address
            if(((uint32_t)(pRxDesc[nRxDescIndex].rx_desc_buffaddr.val) & GMAC_RX_ADDRESS_MASK) != (uint32_t)0)
            {
                //look for the first descriptor of data frame
                if(frameState == GMAC_RX_NO_FRAME_STATE)
                {
                    // Start of Frame bit set?
                    if(GMAC_RX_SOF_BIT == ((uint32_t)(pRxDesc[nRxDescIndex].rx_desc_status.val) & GMAC_RX_SOF_BIT))
                    {

                        //transition the state to SOF detected
                        frameState = GMAC_RX_SOF_DETECTED_STATE;
                        rx_frame_state->startIndex = nRxDescIndex;
                        //start counting number of frames from 1
                        rx_frame_state->buffer_count = 1;
                        // Search maximum total number of descriptor count to find a valid frame
                        search_count = 1;

                        // End of Frame in same descriptor?
                        if(GMAC_RX_EOF_BIT == ((uint32_t)(pRxDesc[nRxDescIndex].rx_desc_status.val) & GMAC_RX_EOF_BIT))
                        {
                            //SOF and EOF in same descriptor; transition to EOF detected
                            frameState = GMAC_RX_VALID_FRAME_DETECTED_STATE;
                            rx_frame_state->endIndex = nRxDescIndex;
                            // increment rx index and continue search
                            GCIRC_INC(nRxDescIndex, nRxDscCnt);
                            break;
                        }
                    }
                }
                else if(frameState == GMAC_RX_SOF_DETECTED_STATE)
                {
                    //SOF detected in another descriptor; then it an error
                    if(GMAC_RX_SOF_BIT == ((uint32_t)(pRxDesc[nRxDescIndex].rx_desc_status.val) & GMAC_RX_SOF_BIT))
                    {
                        //get index of rx descriptor with error
                        rx_index =  (uint16_t)fixed_mod((((int)rx_frame_state->startIndex) + ((int)rx_frame_state->buffer_count) -1),(int)nRxDscCnt);
                        //number of rx descriptors used by error packet
                        nRx_buffer = rx_frame_state->buffer_count;
                        //reset the Rx packet search state machine
                        frameState = GMAC_RX_NO_FRAME_STATE;

                        //clear all the descriptors before the new SOF
                        while((nRx_buffer--) != 0U) 
                        {
                            //Get Rx Packet assigned to Rx Descriptor with error
                            pRxPkt = pMACDrv->sGmacData.gmac_queue[queueIdx].pRxPckt[rx_index];
                            //Call Rx acknowledgment function to free/reuse the Rx packet
                            (pRxPkt->ackFunc)(pRxPkt, pRxPkt->ackParam);
                            //remove rx packet from rx desc
                            pMACDrv->sGmacData.gmac_queue[queueIdx].pRxPckt[rx_index] = NULL;
                            F_DRV_GMAC_RxLock(pMACDrv);
                            //clear the buffer address bitfields
                            pMACDrv->sGmacData.gmac_queue[queueIdx].pRxDesc[rx_index].rx_desc_buffaddr.val &= ~GMAC_RX_ADDRESS_MASK;
                            F_DRV_GMAC_RxUnlock(pMACDrv);
                            //decrement index
                            GCIRC_DEC(rx_index, nRxDscCnt);

                        }
                        // refill the cleared rx descriptors
                        (void)DRV_PIC32CGMAC_LibRxBuffersAppend(pMACDrv, queueIdx, (rx_frame_state->startIndex), (rx_frame_state->buffer_count));
                        //decrement search count
                        search_count--;
                        //decrement rx index to restart searching from new SOF frame
                        GCIRC_DEC(nRxDescIndex, nRxDscCnt);
                    }
                    else
                    {
                        //increment the frame count
                        rx_frame_state->buffer_count++;
                        //EOF detected in new descriptor?
                        if(GMAC_RX_EOF_BIT == ((uint32_t)(pRxDesc[nRxDescIndex].rx_desc_status.val) & GMAC_RX_EOF_BIT))
                        {
                            frameState = GMAC_RX_VALID_FRAME_DETECTED_STATE;
                            rx_frame_state->endIndex = nRxDescIndex;
                            // increment rx index and continue search
                            GCIRC_INC(nRxDescIndex, nRxDscCnt);
                            break;
                        }
                        else
                        {
                            // Do Nothing
                        }
                    }
                }
                else
                {
                    // Do Nothing
                }
            }
            else
            {
                //software owned descriptor without valid buffer
                (void)DRV_PIC32CGMAC_LibRxBuffersAppend(pMACDrv, queueIdx, nRxDescIndex, 1);

            }
        }
        else
        {
            //No RX packets in RX descriptors
            //Continue search in next descriptor
        }

        // increment rx index and continue search
        GCIRC_INC(nRxDescIndex, nRxDscCnt);
        // when search count exceeded number of rx descriptors, exit the search
        if ((search_count++) >  nRxDscCnt)
        {
            break;
        }

    }
    //update new rx descriptor index for next packet search
    pMACDrv->sGmacData.gmac_queue[queueIdx].nRxDescIndex = nRxDescIndex;

    return frameState;
}

/****************************************************************************
 * Function:        GetRxPacket
 * Summary: Get Rx packet buffer from GMAC and transfer to TCP/IP stack.
 * Assign new Rx buffers to GMAC
 * Return : new buffer allocation status
 *****************************************************************************/
static DRV_PIC32CGMAC_RESULT GetRxPacket(DRV_GMAC_DRIVER * pMACDrv,DRV_PIC32CGMAC_RX_FRAME_INFO *rx_frame_state, TCPIP_MAC_PACKET** pRxPkt, GMAC_QUE_LIST queueIdx)
{
    TCPIP_MAC_DATA_SEGMENT *pDseg;
    DRV_PIC32CGMAC_HW_RXDCPT *pRxDesc = pMACDrv->sGmacData.gmac_queue[queueIdx].pRxDesc;
    DRV_GMAC_QUEUE gmac_queue = pMACDrv->sGmacData.gmac_queue[queueIdx];
    TCPIP_MODULE_GMAC_QUEUE_CONFIG g_queue_config = pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx];
    DRV_PIC32CGMAC_RESULT   res = DRV_PIC32CGMAC_RES_NO_PACKET;
    uint32_t frameSize = 0;
    uint32_t checksumFlag = 0;
    uint16_t rx_index;
    uint16_t nRx_buffer;
        
    *pRxPkt = gmac_queue.pRxPckt[rx_frame_state->startIndex]; 
    nRx_buffer = rx_frame_state->buffer_count;      
    rx_index = rx_frame_state->startIndex;
    
    //calculate the size of frame
    frameSize = (pRxDesc[rx_frame_state->endIndex].rx_desc_status.val & GMAC_LENGTH_FRAME); 

    if((pMACDrv->sGmacData.gmacConfig.checksumOffloadRx) != TCPIP_MAC_CHECKSUM_NONE)
    {
        checksumFlag = (pRxDesc[rx_frame_state->endIndex].rx_desc_status.val & GMAC_RX_CHECKSUM_OFFLOAD_STATUS); 
        
        //translate checksum status to packet flag
        switch (checksumFlag)
        {
            case GMAC_RX_CHECKSUM_IP_OFFLOAD:
                (*pRxPkt)->pktFlags |= (uint32_t)TCPIP_MAC_PKT_FLAG_RX_CHKSUM_IP;
                break;

            case GMAC_RX_CHECKSUM_IP_TCP_OFFLOAD:
                (*pRxPkt)->pktFlags |= ((uint32_t)TCPIP_MAC_PKT_FLAG_RX_CHKSUM_IP | (uint32_t)TCPIP_MAC_PKT_FLAG_RX_CHKSUM_TCP);
                break;

            case GMAC_RX_CHECKSUM_IP_UDP_OFFLOAD:
                (*pRxPkt)->pktFlags |= ((uint32_t)TCPIP_MAC_PKT_FLAG_RX_CHKSUM_IP | (uint32_t)TCPIP_MAC_PKT_FLAG_RX_CHKSUM_UDP);
                break;
                
            default: 
                /* Do Nothing */
                break;
        }
        
    }    
    
    //backup of data Segment for later use
    pDseg = (*pRxPkt)->pDSeg;

    if(nRx_buffer != 0U)
    {
        //process all the packet buffers
        while(nRx_buffer-- != 0U)
        {
            // Segment length based on received frame length and Rx buffer size
            (*pRxPkt)->pDSeg->segLen = (frameSize >= g_queue_config.rxBufferSize) ? g_queue_config.rxBufferSize : (uint16_t)frameSize;

            (*pRxPkt)->pDSeg->segSize = g_queue_config.rxBufferSize;

            // RX frame bigger than GMAC RX buffer?
            if(frameSize >= g_queue_config.rxBufferSize)
            {
                //more Rx buffers used for Rx packet
                frameSize = frameSize - g_queue_config.rxBufferSize;
            }
            else
            {   // last Rx buffer of Rx Packet
                frameSize = 0;
            }

            //release the rx packets from GMAC queue
            gmac_queue.pRxPckt[rx_index] = NULL;

            F_DRV_GMAC_RxLock(pMACDrv);
            //clear the buffer address bit-fields
            gmac_queue.pRxDesc[rx_index].rx_desc_buffaddr.val &= ~GMAC_RX_ADDRESS_MASK;
            F_DRV_GMAC_RxUnlock(pMACDrv);

            //more Rx buffers needed for Rx packet
            if((frameSize) != 0U) 
            {
                GCIRC_INC(rx_index, g_queue_config.nRxDescCnt);

                (*pRxPkt)->pDSeg->next = gmac_queue.pRxPckt[rx_index]->pDSeg;
                (*pRxPkt)->pDSeg = (*pRxPkt)->pDSeg->next;
            }

        }

        // restore to first data segment
        (*pRxPkt)->pDSeg = (TCPIP_MAC_DATA_SEGMENT*)pDseg;

        //allocate new packets in place of used buffers
        (void)DRV_PIC32CGMAC_LibRxBuffersAppend(pMACDrv, queueIdx, rx_frame_state->startIndex, rx_frame_state->buffer_count);
        res = DRV_PIC32CGMAC_RES_OK;
    }
    else
    {
        res = DRV_PIC32CGMAC_RES_NO_PACKET;
    }

    return res;
}

/****************************************************************************
 * Function:        AllocateRxPacket
 * Summary: Dynamically allocate Rx packet
 * Allocate new Rx buffers to GMAC
 * Return : new buffer allocation status
 *****************************************************************************/
static DRV_PIC32CGMAC_RESULT AllocateRxPacket(DRV_GMAC_DRIVER * pMACDrv, uint16_t buffer_count, GMAC_QUE_LIST queue_idx, bool sticky_flag)
{
    TCPIP_MAC_PACKET* pRxPkt;
    DRV_PIC32CGMAC_RESULT gmacAllocRes = DRV_PIC32CGMAC_RES_OK;
    uint16_t rxbuff_idx;
    
    for(rxbuff_idx=0; rxbuff_idx < buffer_count; rxbuff_idx++)
    {      
        //allocated packet will be cache-aligned (32-byte aligned)
        pRxPkt = (*pMACDrv->sGmacData.pktAllocF)(sizeof(*pRxPkt), pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queue_idx].rxBufferSize   - sizeof(TCPIP_MAC_ETHERNET_HEADER), 0);
        if(pRxPkt == NULL)
        {   
            //rx pkt allocation failed
            gmacAllocRes = DRV_PIC32CGMAC_RES_OUT_OF_MEMORY;
            break;
        }
        
        if(sticky_flag)
        {
            pRxPkt->pDSeg->segFlags |=  (uint16_t)TCPIP_MAC_SEG_FLAG_RX_STICKY;
        }
        F_DRV_GMAC_RxLock(pMACDrv); 
        DRV_PIC32CGMAC_SingleListTailAdd(&pMACDrv->sGmacData.gmac_queue[queue_idx].RxQueue, FC_MacPkt2LstNode(pRxPkt));
        F_DRV_GMAC_RxUnlock(pMACDrv);
    }
    return gmacAllocRes;
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibSetRxFilter
 * Summary: Set GMAC Rx Filters
 *****************************************************************************/
void DRV_PIC32CGMAC_LibSetRxFilter(DRV_GMAC_DRIVER* pMACDrv,GMAC_RX_FILTERS  gmacRxFilt)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    uint32_t rxfilter = (uint32_t)(pGmacRegs->GMAC_NCFGR) & (~(uint32_t)GMAC_FILT_ALL_FILTERS);
    pGmacRegs->GMAC_NCFGR  = (rxfilter|(uint32_t)gmacRxFilt) ;
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibClearTxComplete
 * Summary: Clear GMAC Tx Complete status
 *****************************************************************************/
void DRV_PIC32CGMAC_LibClearTxComplete(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    // Clear the TXCOMP transmit status
    pGmacRegs->GMAC_TSR = GMAC_TSR_TXCOMP_Msk;
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibIsTxComplete
 * Summary: Check GMAC Tx Complete status
 *****************************************************************************/
bool DRV_PIC32CGMAC_LibIsTxComplete(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return ((pGmacRegs->GMAC_TSR & GMAC_TSR_TXCOMP_Msk) != 0U)? true:false;
}
/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibTxEnable
 * Summary: Enable/Disable GMAC Transmit
 *****************************************************************************/
void DRV_PIC32CGMAC_LibTxEnable(DRV_GMAC_DRIVER* pMACDrv, bool enable)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    if (enable)
    {
        pGmacRegs->GMAC_NCR |= GMAC_NCR_TXEN_Msk;
    }
    else
    {
        pGmacRegs->GMAC_NCR &= ~GMAC_NCR_TXEN_Msk;
    }
}

/****************************************************************************
 * Function:    DRV_PIC32CGMAC_LibClearTxIndex
 * Summary:     Reset Transmit Processing Indexes
 *****************************************************************************/
void  DRV_PIC32CGMAC_LibClearTxIndex(DRV_GMAC_DRIVER* pMACDrv, GMAC_QUE_LIST queueIdx)
{
    // Reset Transmit processing indexes. Because, after TXEN reset, the Transmit  
    // Queue Pointer will point to the start of the transmit descriptor list.
    pMACDrv->sGmacData.gmac_queue[queueIdx].nTxDescHead = 0;
    pMACDrv->sGmacData.gmac_queue[queueIdx].nTxDescTail = 0;
}
/****************************************************************************
 * Function:        _GetPktSegCount
 * Summary: Counts no. of packet segments in MAC packet
 * Return : Number of packet segments
 *****************************************************************************/
static uint16_t GetPktSegCount(TCPIP_MAC_DATA_SEGMENT * pktHead)
{
    // Initialize count
    uint16_t count = 0;
    // Initialize current
    TCPIP_MAC_DATA_SEGMENT * current = pktHead;

    while (current != NULL)
    {
        count++;
        current = current->next;
    }
    return count;
}

/*****************************************************************************
 ********************* GMAC Statistics Register Access routines **************
 *****************************************************************************/

uint32_t DRV_PIC32CGMAC_LibGetTxOctetLow(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_OTLO;
}

uint32_t DRV_PIC32CGMAC_LibGetTxOctetHigh(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_OTHI;
}

uint32_t DRV_PIC32CGMAC_LibGetTxFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_FT;
}

uint32_t DRV_PIC32CGMAC_LibGetTxBCastFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_BCFT;
}

uint32_t DRV_PIC32CGMAC_LibGetTxMCastFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_MFT;
}

uint32_t DRV_PIC32CGMAC_LibGetTxPauseFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_PFT;
}

uint32_t DRV_PIC32CGMAC_LibGetTx64ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_BFT64;
}

uint32_t DRV_PIC32CGMAC_LibGetTx127ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_TBFT127;
}

uint32_t DRV_PIC32CGMAC_LibGetTx255ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_TBFT255;
}

uint32_t DRV_PIC32CGMAC_LibGetTx511ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_TBFT511;
}

uint32_t DRV_PIC32CGMAC_LibGetTx1023ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_TBFT1023;
}

uint32_t DRV_PIC32CGMAC_LibGetTx1518ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_TBFT1518;
}

uint32_t DRV_PIC32CGMAC_LibGetTxGT1518ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_GTBFT1518;
}

uint32_t DRV_PIC32CGMAC_LibGetTxUnderRunFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_TUR;
}

uint32_t DRV_PIC32CGMAC_LibGetTxSingleCollFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_SCF;
}

uint32_t DRV_PIC32CGMAC_LibGetTxMultiCollFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_MCF;
}

uint32_t DRV_PIC32CGMAC_LibGetTxExcessCollFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_EC;
}

uint32_t DRV_PIC32CGMAC_LibGetTxLateCollFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_LC;
}

uint32_t DRV_PIC32CGMAC_LibGetTxDeferFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_DTF;
}

uint32_t DRV_PIC32CGMAC_LibGetTxCSErrorFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_CSE;
}

uint32_t DRV_PIC32CGMAC_LibGetRxOctetLow(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_ORLO;
}

uint32_t DRV_PIC32CGMAC_LibGetRxOctetHigh(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_ORHI;
}

uint32_t DRV_PIC32CGMAC_LibGetRxFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_FR;
}

uint32_t DRV_PIC32CGMAC_LibGetRxBCastFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_BCFR;
}

uint32_t DRV_PIC32CGMAC_LibGetRxMCastFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_MFR;
}

uint32_t DRV_PIC32CGMAC_LibGetRxPauseFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_PFR;
}

uint32_t DRV_PIC32CGMAC_LibGetRx64ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_BFR64;
}

uint32_t DRV_PIC32CGMAC_LibGetRx127ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_TBFR127;
}

uint32_t DRV_PIC32CGMAC_LibGetRx255ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_TBFR255;
}

uint32_t DRV_PIC32CGMAC_LibGetRx511ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_TBFR511;
}

uint32_t DRV_PIC32CGMAC_LibGetRx1023ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_TBFR1023;
}

uint32_t DRV_PIC32CGMAC_LibGetRx1518ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_TBFR1518;
}

uint32_t DRV_PIC32CGMAC_LibGetRxGT1518ByteFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_TMXBFR;
}

uint32_t DRV_PIC32CGMAC_LibGetRxUnderSizeFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_UFR;
}

uint32_t DRV_PIC32CGMAC_LibGetRxOverSizeFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_OFR;
}

uint32_t DRV_PIC32CGMAC_LibGetRxJabberFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_JR;
}

uint32_t DRV_PIC32CGMAC_LibGetRxFCSErrorFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_FCSE;
}

uint32_t DRV_PIC32CGMAC_LibGetRxLFErrorFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_LFFE;
}

uint32_t DRV_PIC32CGMAC_LibGetRxSymErrorFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_RSE;
}

uint32_t DRV_PIC32CGMAC_LibGetRxAlignErrorFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_AE;
}

uint32_t DRV_PIC32CGMAC_LibGetRxResErrorFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_RRE;
}

uint32_t DRV_PIC32CGMAC_LibGetRxOverRunFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_ROE;
}

uint32_t DRV_PIC32CGMAC_LibGetRxIPHdrCSErrorFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_IHCE;
}

uint32_t DRV_PIC32CGMAC_LibGetRxTCPCSErrorFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_TCE;
}

uint32_t DRV_PIC32CGMAC_LibGetRxUDPCSErrorFrameCount(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    return pGmacRegs->GMAC_UCE;
}

/****************************************************************************
 * Function:        F_DRV_GMAC_HashValueSet
 * Summary: Set Hash Value in GMAC register
 *****************************************************************************/
static void F_DRV_GMAC_HashValueSet(DRV_GMAC_DRIVER* pMACDrv, uint64_t hash_value)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    pGmacRegs->GMAC_HRB = (uint32_t)hash_value & 0xffffffffUL;
    pGmacRegs->GMAC_HRT = (uint32_t)(hash_value >> 32) & 0xffffffffUL;
}

/****************************************************************************
 * Function:        F_DRV_GMAC_HashValueGet
 * Summary: Read Hash Value in GMAC register
 *****************************************************************************/
static  uint64_t  F_DRV_GMAC_HashValueGet(DRV_GMAC_DRIVER* pMACDrv)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) pMACDrv->sGmacData.gmacConfig.ethModuleId;
    uint64_t hash_value = 0;
    hash_value = pGmacRegs->GMAC_HRT;
    hash_value = (hash_value << 32) | pGmacRegs->GMAC_HRB;
    return hash_value;
}
