/*******************************************************************************
  Ethernet Driver Library Source Code

  Summary:
    This file contains the source code for the Ethernet Driver library.

  Description:
    This library provides a low-level abstraction of the Ethernet module
    on Microchip PIC32MX family microcontrollers with a convenient C language
    interface.  It can be used to simplify low-level access to the module
    without the necessity of interacting directly with the module's registers,
    thus hiding differences from one microcontroller variant to another.
*******************************************************************************/
//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2008-2018 Microchip Technology Inc. and its subsidiaries.

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

//DOM-IGNORE-END


#include "driver/gmac/src/dynamic/drv_gmac_lib.h"

#if defined(PIC32C_GMAC_ISR_TX) //GMAC with TX interrupt enabled
	#define GMAC_INT_BITS  (GMAC_INT_RX_BITS | GMAC_INT_TX_BITS)
#else
	#define GMAC_INT_BITS  (GMAC_INT_RX_BITS)
#endif


static bool _MacPacketAck(TCPIP_MAC_PACKET* pkt,  const void* param);

/* TX descriptors for 6 Queues */
            
typedef struct
{
	DRV_PIC32CGMAC_HW_TXDCPT sTxDesc_queue0[TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE0];
	DRV_PIC32CGMAC_HW_TXDCPT sTxDesc_queue1[TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE1];
    DRV_PIC32CGMAC_HW_TXDCPT sTxDesc_queue2[TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE2];
	DRV_PIC32CGMAC_HW_TXDCPT sTxDesc_queue3[TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE3];
    DRV_PIC32CGMAC_HW_TXDCPT sTxDesc_queue4[TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE4];
	DRV_PIC32CGMAC_HW_TXDCPT sTxDesc_queue5[TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE5];
    
    DRV_PIC32CGMAC_HW_RXDCPT sRxDesc_queue0[TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE0];
	DRV_PIC32CGMAC_HW_RXDCPT sRxDesc_queue1[TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE1];
    DRV_PIC32CGMAC_HW_RXDCPT sRxDesc_queue2[TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE2];
	DRV_PIC32CGMAC_HW_RXDCPT sRxDesc_queue3[TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE3];
    DRV_PIC32CGMAC_HW_RXDCPT sRxDesc_queue4[TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE4];
	DRV_PIC32CGMAC_HW_RXDCPT sRxDesc_queue5[TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE5];
    
} DRV_PIC32CGMAC_HW_DCPT_ARRAY ; 


// place the descriptors in an uncached memory region
__attribute__((__aligned__(8))) __attribute__((space(data),address(0x2045F000))) __attribute__((keep))DRV_PIC32CGMAC_HW_DCPT_ARRAY gmac_dcpt_array;

/****************************************************************************
 * Function:        DRV_PIC32CGMAC_LibInit
 * Summary: Initialize GMAC peripheral registers
 *****************************************************************************/
void DRV_PIC32CGMAC_LibInit(DRV_GMAC_DRIVER* pMACDrv) 
{		
	
	//disable Tx
	GMAC_REGS->GMAC_NCR &= ~GMAC_NCR_TXEN_Msk;
	//disable Rx
	GMAC_REGS->GMAC_NCR &= ~GMAC_NCR_RXEN_Msk;
	
	//disable all GMAC interrupts for QUEUE 0
	GMAC_REGS->GMAC_IDR = GMAC_INT_ALL;
	//disable all GMAC interrupts for QUEUE 1
	GMAC_REGS->GMAC_IDRPQ[0] = GMAC_INT_ALL;
	//disable all GMAC interrupts for QUEUE 2
	GMAC_REGS->GMAC_IDRPQ[1] = GMAC_INT_ALL;
	
	//Clear statistics register
	GMAC_REGS->GMAC_NCR |=  GMAC_NCR_CLRSTAT_Msk;
	//Clear RX Status
	GMAC_REGS->GMAC_RSR =  GMAC_RSR_RXOVR_Msk | GMAC_RSR_REC_Msk | GMAC_RSR_BNA_Msk  | GMAC_RSR_HNO_Msk;
	//Clear TX Status
	GMAC_REGS->GMAC_TSR = GMAC_TSR_UBR_Msk  | GMAC_TSR_COL_Msk  | GMAC_TSR_RLE_Msk | GMAC_TSR_TXGO_Msk |
																	GMAC_TSR_TFC_Msk  | GMAC_TSR_TXCOMP_Msk  | GMAC_TSR_HRESP_Msk;
									
	//Clear Interrupt status
	GMAC_REGS->GMAC_ISR;
	GMAC_REGS->GMAC_ISRPQ[0] ;   
	GMAC_REGS->GMAC_ISRPQ[1] ;
	//Set network configurations like speed, full duplex, copy all frames, no broadcast, 
	// pause enable, remove FCS, MDC clock
    GMAC_REGS->GMAC_NCFGR = GMAC_NCFGR_SPD(1) | GMAC_NCFGR_FD(1) | GMAC_NCFGR_DBW(0) | GMAC_NCFGR_CLK(4)  |	GMAC_NCFGR_PEN(1)  | GMAC_NCFGR_RFCS(1);
	
	// Set MAC address    
    DRV_PIC32CGMAC_LibSetMacAddr((const uint8_t *)(pMACDrv->sGmacData.gmacConfig.macAddress.v));
	// MII mode config
    //Configure in RMII mode
	if((TCPIP_INTMAC_PHY_CONFIG_FLAGS) & DRV_ETHPHY_CFG_RMII)
		GMAC_REGS->GMAC_UR = GMAC_UR_RMII(0); //initial mode set as RMII
	else
		GMAC_REGS->GMAC_UR = GMAC_UR_RMII(1); //initial mode set as MII
}




/****************************************************************************
 * Function:        DRV_PIC32CGMAC_LibInitTransfer
 * Summary :  Configure DMA and interrupts
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibInitTransfer(DRV_GMAC_DRIVER* pMACDrv,GMAC_QUE_LIST queueIdx) 
{
	
	uint16_t wRxDescCnt_temp = pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nRxDescCnt;
	uint16_t wTxDescCnt_temp = pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nTxDescCnt;
	uint16_t wRxBufferSize_temp = pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].rxBufferSize;

	uint32_t wDmaCfg;
	
	if (wRxDescCnt_temp < 1 || wTxDescCnt_temp < 1)
	return DRV_PIC32CGMAC_RES_DESC_CNT_ERR;

	if (!wRxBufferSize_temp || wRxBufferSize_temp > TCPIP_GMAC_RX_MAX_FRAME)
	return DRV_PIC32CGMAC_RES_RX_SIZE_ERR;

    // Setup the interrupts for RX/TX completion (and errors) 
    if(queueIdx)
    {   //for all QUEUEs other than QUEUE 0
        if(queueIdx < DRV_GMAC_NUMBER_OF_QUEUES)
    {
        //dma configuration
            wDmaCfg = (GMAC_RBSRPQ_RBS_Msk & ((wRxBufferSize_temp >> 6) << GMAC_RBSRPQ_RBS_Pos));
        //write dma configuration to register
            GMAC_REGS->GMAC_RBSRPQ[queueIdx - 1] = wDmaCfg;
            //enable GMAC interrupts
            GMAC_REGS->GMAC_IERPQ[queueIdx - 1] = GMAC_INT_BITS;
        }
        else
        {
           return DRV_PIC32CGMAC_RES_QUEUE_ERR;
        }
    }
    else //for QUEUE 0
    {
        //dma configuration
        wDmaCfg = (GMAC_DCFGR_DRBS_Msk & ((wRxBufferSize_temp >> 6) << GMAC_DCFGR_DRBS_Pos))| (GMAC_DCFGR_RXBMS_Msk & ((3) << GMAC_DCFGR_RXBMS_Pos)) | GMAC_DCFGR_TXPBMS_Msk  | GMAC_DCFGR_FBLDO_INCR4 | GMAC_DCFGR_DDRP_Msk;
        //write dma configuration to register
        GMAC_REGS->GMAC_DCFGR = wDmaCfg;
        //enable GMAC interrupts
        GMAC_REGS->GMAC_IER = GMAC_INT_BITS;
    }
	return DRV_PIC32CGMAC_RES_OK;
	
}

/****************************************************************************
 * Function:        DRV_PIC32CGMAC_LibTransferEnable
 * Summary: Enable Rx and Tx of GMAC
 *****************************************************************************/
void DRV_PIC32CGMAC_LibTransferEnable (DRV_GMAC_DRIVER* pMACDrv)
{
	// Enable Rx and Tx, plus the statistics register.
	GMAC_REGS->GMAC_NCR |= GMAC_NCR_TXEN_Msk;
	GMAC_REGS->GMAC_NCR |= GMAC_NCR_RXEN_Msk;	
	GMAC_REGS->GMAC_NCR |= GMAC_NCR_WESTAT_Msk;
	
}


/****************************************************************************
 * Function:        DRV_PIC32CGMAC_LibClose
 * Summary: Disable GMAC Rx, Tx and interrupts
 *****************************************************************************/
void DRV_PIC32CGMAC_LibClose(DRV_GMAC_DRIVER * pMACDrv, DRV_PIC32CGMAC_CLOSE_FLAGS cFlags)
{
	// disable Rx, Tx, Eth controller itself

	GMAC_REGS->GMAC_NCR &= ~GMAC_NCR_TXEN_Msk;
	GMAC_REGS->GMAC_NCR &= ~GMAC_NCR_RXEN_Msk;

	GMAC_REGS->GMAC_ISR;
	GMAC_REGS->GMAC_ISRPQ[0];
	GMAC_REGS->GMAC_ISRPQ[1];
}


/****************************************************************************
 * Function:        DRV_PIC32CGMAC_LibMACOpen
 * Summary : Open GMAC driver
 *****************************************************************************/
void DRV_PIC32CGMAC_LibMACOpen(DRV_GMAC_DRIVER * pMACDrv, TCPIP_ETH_OPEN_FLAGS oFlags, TCPIP_ETH_PAUSE_TYPE pauseType)
{	
	uint32_t ncfgr;
	
	GMAC_REGS->GMAC_NCR &= ~GMAC_NCR_TXEN_Msk;
	GMAC_REGS->GMAC_NCR &= ~GMAC_NCR_RXEN_Msk;
	
	ncfgr = GMAC_REGS->GMAC_NCFGR;
	
	if(oFlags & TCPIP_ETH_OPEN_FDUPLEX)
		ncfgr |= GMAC_NCFGR_FD_Msk ;
	else
		ncfgr &= ~GMAC_NCFGR_FD_Msk ;	
	
	if(oFlags & TCPIP_ETH_OPEN_100)
		ncfgr |= GMAC_NCFGR_SPD_Msk ;
	else
		ncfgr &= ~GMAC_NCFGR_SPD_Msk ;
		
	if(pauseType & TCPIP_ETH_PAUSE_TYPE_EN_RX)
		ncfgr |= GMAC_NCFGR_PEN_Msk ;
	else
		ncfgr &= ~GMAC_NCFGR_PEN_Msk ;		
	
	GMAC_REGS->GMAC_NCFGR = ncfgr;
	
	if(oFlags & TCPIP_ETH_OPEN_RMII)	
	{
        //Configure in RMII mode
        GMAC_REGS->GMAC_UR = GMAC_UR_RMII(0);
	}
	else
	{
        //Configure in MII mode
        GMAC_REGS->GMAC_UR = GMAC_UR_RMII(1);
	}
	
	GMAC_REGS->GMAC_NCR |= GMAC_NCR_RXEN_Msk;
	GMAC_REGS->GMAC_NCR |= GMAC_NCR_TXEN_Msk;

}



/****************************************************************************
 * Function:        DRV_PIC32CGMAC_LibRxBuffersAppend
 * Summary : allocate and add new RX buffers to RX descriptor
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxBuffersAppend(DRV_GMAC_DRIVER* pMACDrv, GMAC_QUE_LIST queueIdx, uint16_t start_index, uint16_t nDesc_Cnt) 
{
	TCPIP_MAC_PACKET*   pRxPkt;   
	DRV_PIC32CGMAC_RESULT gmacRes = DRV_PIC32CGMAC_RES_OK;
	uint8_t desc_idx = start_index;

	while (nDesc_Cnt--)
	{
		if(pMACDrv->sGmacData.gmac_queue[queueIdx].pRxPckt[desc_idx] == 0)
		{
			// allocate the RX buffers
			pRxPkt = 0;						
			
            //allocated packet will be cache-aligned (32-byte aligned)
			pRxPkt = (*pMACDrv->sGmacData.pktAllocF)(sizeof(*pRxPkt), pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].rxBufferSize   - sizeof(TCPIP_MAC_ETHERNET_HEADER), 0);
			if(pRxPkt == NULL)
			{   // failed
				gmacRes = DRV_PIC32CGMAC_RES_OUT_OF_MEMORY;
				break;
			}			
			
			_DRV_GMAC_RxLock(pMACDrv);
			
			/* Reset status value. */
			pMACDrv->sGmacData.gmac_queue[queueIdx].pRxDesc[desc_idx].rx_desc_status.val = 0;
			
			if (desc_idx == pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nRxDescCnt - 1)
				pMACDrv->sGmacData.gmac_queue[queueIdx].pRxDesc[desc_idx].rx_desc_buffaddr.val =  ((uint32_t)pRxPkt->pDSeg->segLoad | GMAC_RX_WRAP_BIT);
			else
				pMACDrv->sGmacData.gmac_queue[queueIdx].pRxDesc[desc_idx].rx_desc_buffaddr.val =  ((uint32_t)pRxPkt->pDSeg->segLoad & GMAC_ADDRESS_MASK);
			
			_DRV_GMAC_RxUnlock(pMACDrv);
			

			// set the packet acknowledgement
			pRxPkt->ackFunc = _MacPacketAck;
			pRxPkt->ackParam = pMACDrv;
					
			/* Save packet pointer */
			pMACDrv->sGmacData.gmac_queue[queueIdx].pRxPckt[desc_idx] = pRxPkt;
			pRxPkt->next = 0;
			
			GCIRC_INC(desc_idx,pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nRxDescCnt );

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
	uint8_t queue_idx, desc_idx;
	DRV_PIC32CGMAC_RESULT gmacRes = DRV_PIC32CGMAC_RES_OK;
		
	for(queue_idx=0; queue_idx < DRV_GMAC_NUMBER_OF_QUEUES; queue_idx++)
	{
		for(desc_idx=0; desc_idx < pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queue_idx].nRxDescCnt; desc_idx++)
		{
            pMACDrv->sGmacData.gmac_queue[queue_idx].nRxDescIndex = 0;
			pMACDrv->sGmacData.gmac_queue[queue_idx].pRxPckt[desc_idx] = 0;
			pMACDrv->sGmacData.gmac_queue[queue_idx].pRxDesc[desc_idx].rx_desc_buffaddr.val = 0;
			pMACDrv->sGmacData.gmac_queue[queue_idx].pRxDesc[desc_idx].rx_desc_status.val = 0;
		}
		pMACDrv->sGmacData.gmac_queue[queue_idx].pRxDesc[desc_idx-1].rx_desc_buffaddr.val |= GMAC_RX_WRAP_BIT;
		gmacRes = DRV_PIC32CGMAC_LibRxBuffersAppend(pMACDrv, queue_idx,0,pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queue_idx].nRxDescCnt);
		
		if(gmacRes != DRV_PIC32CGMAC_RES_OK)
		{
			break;
		}
		if (!queue_idx)
			GMAC_REGS->GMAC_RBQB = GMAC_RBQB_ADDR_Msk & ((uint32_t)pMACDrv->sGmacData.gmac_queue[queue_idx].pRxDesc);
		else
			GMAC_REGS->GMAC_RBQBAPQ[queue_idx - 1] = GMAC_RBQB_ADDR_Msk & ((uint32_t)pMACDrv->sGmacData.gmac_queue[queue_idx].pRxDesc);
	}
	
	return gmacRes;
}//DRV_PIC32CGMAC_LibRxInit

/****************************************************************************
 * Function:        DRV_PIC32CGMAC_LibTxInit
 * Summary : initialize TX Descriptors
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibTxInit(DRV_GMAC_DRIVER* pMACDrv) 
{
	uint8_t queue_idx, desc_idx;
	DRV_PIC32CGMAC_RESULT gmacRes = DRV_PIC32CGMAC_RES_OK;
	DRV_PIC32CGMAC_SGL_LIST_NODE*   pNewQueueNode;
	
	for(queue_idx=0; queue_idx < DRV_GMAC_NUMBER_OF_QUEUES; queue_idx++)
	{
		//initialise the 3 different Tx Queues
		DRV_PIC32CGMAC_SingleListInitialize(&pMACDrv->sGmacData.gmac_queue[queue_idx]._TxNewQueue);
		DRV_PIC32CGMAC_SingleListInitialize(&pMACDrv->sGmacData.gmac_queue[queue_idx]._TxStartQueue);
		DRV_PIC32CGMAC_SingleListInitialize(&pMACDrv->sGmacData.gmac_queue[queue_idx]._TxAckQueue);
		for(desc_idx=0; desc_idx < pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queue_idx].nTxDescCnt; desc_idx++)
		{
            pMACDrv->sGmacData.gmac_queue[queue_idx].nTxDescHead = 0;
            pMACDrv->sGmacData.gmac_queue[queue_idx].nTxDescTail = 0;
			pMACDrv->sGmacData.gmac_queue[queue_idx].pTxDesc[desc_idx].tx_desc_buffaddr = 0;
			pMACDrv->sGmacData.gmac_queue[queue_idx].pTxDesc[desc_idx].tx_desc_status.val = GMAC_TX_USED_BIT | GMAC_TX_LAST_BUFFER_BIT;
			
			pNewQueueNode = (*pMACDrv->sGmacData._callocF)(pMACDrv->sGmacData._AllocH,1, sizeof(*pNewQueueNode));
			DRV_PIC32CGMAC_SingleListTailAdd(&pMACDrv->sGmacData.gmac_queue[queue_idx]._TxNewQueue, (DRV_PIC32CGMAC_SGL_LIST_NODE*)pNewQueueNode); // each gmac_queue will have seperate _TxNewQueue
		}
		pMACDrv->sGmacData.gmac_queue[queue_idx].pTxDesc[desc_idx-1].tx_desc_status.val |= GMAC_TX_WRAP_BIT;
				
		if (!queue_idx)
			GMAC_REGS->GMAC_TBQB = GMAC_TBQB_ADDR_Msk & ((uint32_t)pMACDrv->sGmacData.gmac_queue[queue_idx].pTxDesc);
		else
			GMAC_REGS->GMAC_TBQBAPQ[queue_idx - 1] = GMAC_TBQB_ADDR_Msk & ((uint32_t)pMACDrv->sGmacData.gmac_queue[queue_idx].pTxDesc);
	}
	
	return gmacRes;
}//DRV_PIC32CGMAC_LibTxInit

/****************************************************************************
 * Function:        _Calculate_Descriptor_Count
 * Summary : calculate number of Tx descriptors needed for a Tx frame packet
 *****************************************************************************/
static inline uint16_t _Calculate_Descriptor_Count (uint16_t numBytes, uint16_t buffSize )
{
	uint16_t nTxDesc = 0;
	nTxDesc = numBytes/buffSize;
	if((numBytes) % buffSize)
	{
		nTxDesc +=1;
	}
	return nTxDesc;
}



/****************************************************************************
 * Function:        DRV_PIC32CGMAC_LibTxSendPacket
 * Summary: Add Tx packet to Tx descriptors and trigger Tx Start
 *****************************************************************************/

DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibTxSendPacket(DRV_GMAC_DRIVER * pMACDrv,GMAC_QUE_LIST queueIdx)  
{
	DRV_PIC32CGMAC_RESULT     res;
	DRV_PIC32CGMAC_SGL_LIST_NODE*   txqueue_node;	
	DRV_PIC32CGMAC_HW_TXDCPT *pTxDesc = pMACDrv->sGmacData.gmac_queue[queueIdx].pTxDesc;
    TCPIP_MAC_DATA_SEGMENT *pPkt,*pPkt_temp;	
	uint16_t wTxIndex =0, wNewTxHead =0;
	uint16_t nRemByte =0, nLoopCnt =0;
	uint16_t txBufferSize = pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].txBufferSize;
	uint16_t wTxDescCount =pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nTxDescCnt;	
	uint32_t nTotalDesc_count = 0; 

	res=DRV_PIC32CGMAC_RES_NO_PACKET;
	
	txqueue_node = DRV_PIC32CGMAC_SingleListHeadRemove(&pMACDrv->sGmacData.gmac_queue[queueIdx]._TxStartQueue);

	pPkt_temp = pPkt = ((TCPIP_MAC_PACKET *)txqueue_node->data)->pDSeg;
    
	txqueue_node->startIndex = pMACDrv->sGmacData.gmac_queue[queueIdx].nTxDescHead;
	

	//calculate the number of descriptors, even for multi packet frame
	while(pPkt_temp != 0)
	{
		nTotalDesc_count  += _Calculate_Descriptor_Count(pPkt_temp->segLen,pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].txBufferSize);
		pPkt_temp = pPkt_temp->next;
	}
	

	//check for enough number of tx descriptors available
	if(nTotalDesc_count <= (GCIRC_SPACE(pMACDrv->sGmacData.gmac_queue[queueIdx].nTxDescHead, pMACDrv->sGmacData.gmac_queue[queueIdx].nTxDescTail,
															wTxDescCount)))
	{
		wTxIndex = pMACDrv->sGmacData.gmac_queue[queueIdx].nTxDescHead ;
		wNewTxHead = fixed_mod((wTxIndex + nTotalDesc_count),wTxDescCount);
        
        //Clean D-Cache is cache is enabled
        if( (SCB->CCR & SCB_CCR_DC_Msk) == SCB_CCR_DC_Msk)
        {
#ifdef CACHE_CLEAN_BY_ADDRESS
			SCB_CleanDCache_by_Addr ((uint32_t*)pPkt->segLoad, (int32_t) pPkt_temp->segLen);
#else
			SCB_CleanDCache();
#endif			
        }
		
		while (pPkt)
		{
			//number of descriptor for each packets
			nLoopCnt = _Calculate_Descriptor_Count(pPkt->segLen,pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].txBufferSize);
			nRemByte = pPkt->segLen;
			
			//until all the descriptors for this packet is updated
			while(nLoopCnt != 0)
			{
				pTxDesc[wTxIndex].tx_desc_status.val &= (GMAC_TX_WRAP_BIT |GMAC_TX_USED_BIT); //clear all Tx Status except Wrap Bit and Used Bit
				pTxDesc[wTxIndex].tx_desc_buffaddr = (uint32_t)(&((uint8_t *)pPkt->segLoad)[pPkt->segLen - nRemByte]);	//set the buffer address
				if(nRemByte > txBufferSize)
				{
					pTxDesc[wTxIndex].tx_desc_status.val |= txBufferSize & GMAC_LENGTH_FRAME; //Set Length for each frame
					nRemByte -= txBufferSize;
				}
				else
				{
					pTxDesc[wTxIndex].tx_desc_status.val |= nRemByte & GMAC_LENGTH_FRAME;
					nRemByte = 0;
				}
				GCIRC_INC(wTxIndex,pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nTxDescCnt); //Increment the index of Tx Desc
				nLoopCnt--;
				
			}
			pPkt = pPkt->next;
		}
		//Set Last Buffer bit for the last descriptor of the packet frame
		GCIRC_DEC(wTxIndex,pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nTxDescCnt);
		pTxDesc[wTxIndex].tx_desc_status.val |= GMAC_TX_LAST_BUFFER_BIT;
		txqueue_node->endIndex = wTxIndex;
		
		//Clear the Used bit for all descriptors in reverse order to avoid race condition
		nLoopCnt = nTotalDesc_count;
		while(nLoopCnt != 0)
		{
			pTxDesc[wTxIndex].tx_desc_status.val &= ~GMAC_TX_USED_BIT;
			GCIRC_DEC(wTxIndex,pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nTxDescCnt);
			nLoopCnt--;
		}
		//Update new Tx Head Index
		pMACDrv->sGmacData.gmac_queue[queueIdx].nTxDescHead = wNewTxHead;
		
		//memory barrier to ensure all the memories updated before enabling transmission
        __DMB();
		//Enable Transmission
		GMAC_REGS->GMAC_NCR |= GMAC_NCR_TSTART_Msk;	
		
		//Add to TxAck Queue
		DRV_PIC32CGMAC_SingleListTailAdd(&pMACDrv->sGmacData.gmac_queue[queueIdx]._TxAckQueue, txqueue_node);
		
		res=DRV_PIC32CGMAC_RES_OK;
		
	}
	else
	{ //not enough descriptors available; add back the packet to TxStartQueue head
		DRV_PIC32CGMAC_SingleListHeadAdd(&pMACDrv->sGmacData.gmac_queue[queueIdx]._TxStartQueue, txqueue_node); 
		res=DRV_PIC32CGMAC_RES_NO_DESCRIPTORS;
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
	TCPIP_MAC_PACKET* pPkt;
	DRV_PIC32CGMAC_SGL_LIST_NODE*   pTxAckNode;
	DRV_PIC32CGMAC_RESULT res;
	uint16_t tempIndex = 0;	

	res = DRV_PIC32CGMAC_RES_DESC_CNT_ERR;		
	
	//any packet to be acknowledged in the TxAckQueue	
	while( pMACDrv->sGmacData.gmac_queue[queueIdx]._TxAckQueue.nNodes != 0)
	{		
		tempIndex = (pMACDrv->sGmacData.gmac_queue[queueIdx]._TxAckQueue.head)->startIndex;	
			
		//Only check the first Descriptor USED bit to see whether the buffer is used by GMAC
		if((pMACDrv->sGmacData.gmac_queue[queueIdx].pTxDesc[tempIndex].tx_desc_status.val) & GMAC_TX_USED_BIT)
		{
			pTxAckNode = DRV_PIC32CGMAC_SingleListHeadRemove(&pMACDrv->sGmacData.gmac_queue[queueIdx]._TxAckQueue); 
			pPkt  = (TCPIP_MAC_PACKET*)(pTxAckNode->data);
			pPkt->pktFlags &= ~TCPIP_MAC_PKT_FLAG_QUEUED;
			pMACDrv->sGmacData.gmac_queue[queueIdx].nTxDescTail= pTxAckNode->endIndex;
				
			//increment the tail for the next packet processing
			GCIRC_INC(pMACDrv->sGmacData.gmac_queue[queueIdx].nTxDescTail,pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nTxDescCnt);
				
			// Tx Callback
			(*pMACDrv->sGmacData.pktAckF)(pPkt, TCPIP_MAC_PKT_ACK_TX_OK, TCPIP_MODULE_MAC_PIC32C);				
			pMACDrv->sGmacData._txStat.nTxOkPackets++;
						
			// Set USED bit for all the ack packets; because hardware set USED bit only for first buffer
			while(1)
			{
				pMACDrv->sGmacData.gmac_queue[queueIdx].pTxDesc[tempIndex].tx_desc_status.val |= GMAC_TX_USED_BIT;
				
				if(tempIndex == pTxAckNode->endIndex)
				{					
					break;
				}
				GCIRC_INC(tempIndex,pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nTxDescCnt);
			}
			
			//clean the Tx Node	for reusing		
			_Clear_TxNode(pTxAckNode);
				
			// add the node to Free queue
			DRV_PIC32CGMAC_SingleListTailAdd(&pMACDrv->sGmacData.gmac_queue[queueIdx]._TxNewQueue,pTxAckNode);
			
			res = DRV_PIC32CGMAC_RES_OK;
				
		}
		else
		{	
			//packet in TxAckQueue, but transmission not complete	
			break;
		}		
	}			
	return res;	

} //DRV_PIC32CGMAC_LibTxAckPacket



/****************************************************************************
 * Function:        DRV_PIC32CGMAC_LibRxGetPacket
 * Summary: extract  RX packet from the RX descriptor
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxGetPacket(DRV_GMAC_DRIVER * pMACDrv, TCPIP_MAC_PACKET** pRxPkt, int* pnBuffs, DRV_GMAC_RXDCPT_STATUS* pRxStat, GMAC_QUE_LIST queueIdx)  
{	
	DRV_PIC32CGMAC_HW_RXDCPT *pRxDesc = pMACDrv->sGmacData.gmac_queue[queueIdx].pRxDesc;
	uint16_t nRxDescIndex = pMACDrv->sGmacData.gmac_queue[queueIdx].nRxDescIndex;
    TCPIP_MAC_DATA_SEGMENT *pPkt;
	TCPIP_MAC_PACKET* pRxTempPkt;
	DRV_PIC32CGMAC_RESULT     res;
	
    uint32_t bna_flag = 0;
    
	uint16_t startIndex = 0;	
	uint16_t endIndex = 0;
	GMAC_RXFRAME_STATE frameState = GMAC_RX_NO_FRAME_STATE;	
	uint32_t frameSize = 0;
	uint16_t search_count = 0;
	uint16_t frame_count = 0; 
	uint16_t nRxDscCnt =  pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nRxDescCnt;
	uint16_t rx_index;
	uint16_t nRx_buffer;
	
	//init return status as NO PACKET	
	res=DRV_PIC32CGMAC_RES_NO_PACKET;
    
    if(GMAC_REGS->GMAC_RSR & GMAC_RSR_BNA_Msk ) //Check for BNA error due to shortage of Rx Buffers
    {
        bna_flag = 1;
        pMACDrv->sGmacData._rxStat.nRxBuffNotAvailable++;		
		GMAC_REGS->GMAC_RSR = GMAC_RSR_BNA_Msk ; //Clear Buffer Not Available Flag	            
       __DMB();        
    }
	
	if(!pRxPkt)
	{
		return res;
	}
	
	//search the descriptors for valid data frame; search maximum of descriptor count
	while ( search_count < nRxDscCnt)
	{
		//Rx Descriptors with Ownership bit Set? i.e. software owned descriptor?
		if ((((uint32_t)(pRxDesc[nRxDescIndex].rx_desc_buffaddr.val) & GMAC_RX_OWNERSHIP_BIT) == GMAC_RX_OWNERSHIP_BIT))
        {
            //valid buffer address
            if(((uint32_t)(pRxDesc[nRxDescIndex].rx_desc_buffaddr.val) & GMAC_ADDRESS_MASK) != (uint32_t)0)
            {                
                //look for the first descriptor of data frame
                if(frameState == GMAC_RX_NO_FRAME_STATE)
                {
                    // Start of Frame bit set?
                    if(GMAC_RX_SOF_BIT == ((uint32_t)(pRxDesc[nRxDescIndex].rx_desc_status.val) & GMAC_RX_SOF_BIT))
                    {
                        //transition the stat to SOF detected
                        frameState = GMAC_RX_SOF_DETECTED_STATE;
                        startIndex = nRxDescIndex;
                        frame_count = 1; //start counting number of frames from 1
                        search_count = 1; // Search maximum total number of descriptor count to find a valid frame 

                        // End of Frame in same descriptor?	
                        if(GMAC_RX_EOF_BIT == ((uint32_t)(pRxDesc[nRxDescIndex].rx_desc_status.val) & GMAC_RX_EOF_BIT))
                        {
                            //SOF and EOF in same descriptor; transition to EOF detected
                            frameState = GMAC_RX_EOF_DETECTED_STATE;
                            endIndex = nRxDescIndex;
                            break;
                        }
                    }					
                }
                else if(frameState == GMAC_RX_SOF_DETECTED_STATE)
                {	
                    //SOF detected in another descriptor; then it an error			
                    if(GMAC_RX_SOF_BIT == ((uint32_t)(pRxDesc[nRxDescIndex].rx_desc_status.val) & GMAC_RX_SOF_BIT))
                    {
                        rx_index =  fixed_mod((startIndex + frame_count -1),nRxDscCnt);					
                        nRx_buffer = frame_count;
                        frameState = GMAC_RX_NO_FRAME_STATE;

                        //clear the above descriptors
                        while(nRx_buffer--)
                        {
                            (*pMACDrv->sGmacData.pktFreeF)(pMACDrv->sGmacData.gmac_queue[queueIdx].pRxPckt[rx_index]);
                            pMACDrv->sGmacData.gmac_queue[queueIdx].pRxPckt[rx_index] = 0; //remove rx packet from rx desc
                            pMACDrv->sGmacData.gmac_queue[queueIdx].pRxDesc[rx_index].rx_desc_buffaddr.val &= ~GMAC_ADDRESS_MASK; //clear the buffer address bitfields
                            GCIRC_DEC(rx_index, nRxDscCnt); //decrement

                        }

                        DRV_PIC32CGMAC_LibRxBuffersAppend(pMACDrv, queueIdx, startIndex, frame_count);
                        search_count--; //decrement search count
                        GCIRC_DEC(nRxDescIndex, nRxDscCnt); //decrement rx index to restart searching from new SOF frame
                    }
                    else
                    {
                        frame_count++; //increment the frame count
                        //EOF detected in new descriptor?
                        if(GMAC_RX_EOF_BIT == ((uint32_t)(pRxDesc[nRxDescIndex].rx_desc_status.val) & GMAC_RX_EOF_BIT))
                        {
                            frameState = GMAC_RX_EOF_DETECTED_STATE;
                            endIndex = nRxDescIndex;
                            break;
                        }					
                    }				
                }				
            }
            else
            {
                //software owned descriptor without valid buffer
                DRV_PIC32CGMAC_LibRxBuffersAppend(pMACDrv, queueIdx, nRxDescIndex, 1);

            }
        }
		else
		{
			//Ownership bit not set in an intermediate descriptor after an SOF is detected? error condition, free all the used buffers
			if(frameState == GMAC_RX_SOF_DETECTED_STATE)
			{
				rx_index = fixed_mod((startIndex + frame_count -1),nRxDscCnt);	
				nRx_buffer = frame_count;
				frameState = GMAC_RX_NO_FRAME_STATE;
								
				while(nRx_buffer--)
				{
					(*pMACDrv->sGmacData.pktFreeF)(pMACDrv->sGmacData.gmac_queue[queueIdx].pRxPckt[rx_index]);					
					pMACDrv->sGmacData.gmac_queue[queueIdx].pRxPckt[rx_index] = 0; //remove rx packet from rx desc
					pMACDrv->sGmacData.gmac_queue[queueIdx].pRxDesc[rx_index].rx_desc_buffaddr.val &= ~GMAC_ADDRESS_MASK; //clear the buffer address bit fields
					GCIRC_DEC(rx_index, nRxDscCnt); //decrement 
					
				}								
				DRV_PIC32CGMAC_LibRxBuffersAppend(pMACDrv, queueIdx, startIndex, frame_count);
				search_count--; //decrement search count 
				GCIRC_DEC(nRxDescIndex, nRxDscCnt); //decrement rx index to restart searching from new SOF frame			
			}
		}
		
		GCIRC_INC(nRxDescIndex, nRxDscCnt);
		if ((search_count++) >  nRxDscCnt)
			break;		
		
	}
	//Valid packet detected with SOF and EOF
	if (frameState == GMAC_RX_EOF_DETECTED_STATE)
	{		
		pRxTempPkt = *pRxPkt = pMACDrv->sGmacData.gmac_queue[queueIdx].pRxPckt[startIndex];
		
		// Set the index to end of curent search. This index will be used in next packet search
		GCIRC_INC(nRxDescIndex, pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nRxDescCnt);
		pMACDrv->sGmacData.gmac_queue[queueIdx].nRxDescIndex = nRxDescIndex;
		
		*pnBuffs = frame_count; //number of buffers to return
		nRx_buffer = frame_count;		
		rx_index = startIndex;
		
		//calculate the size of frame
		frameSize = (pRxDesc[endIndex].rx_desc_status.val & GMAC_LENGTH_FRAME); 
		
		pPkt = (*pRxPkt)->pDSeg; //backup of data Segment for later use
		while(nRx_buffer--) //process all the packet buffers
		{
			(*pRxPkt)->pDSeg->segLen = (frameSize >= pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].rxBufferSize) ?
												pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].rxBufferSize :frameSize;
                                           			
			(*pRxPkt)->pDSeg->segSize = pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].rxBufferSize;
			
			if(frameSize >= pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].rxBufferSize)
				frameSize = frameSize - pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].rxBufferSize;
			else
				frameSize = 0;			
			
			pMACDrv->sGmacData.gmac_queue[queueIdx].pRxPckt[rx_index] = 0; //release the rx packets from Gmac queue
			pMACDrv->sGmacData.gmac_queue[queueIdx].pRxDesc[rx_index].rx_desc_buffaddr.val &= ~GMAC_ADDRESS_MASK; //clear the buffer address bitfields
			
            //Invalidate D-Cache if cache is enabled
            if( (SCB->CCR & SCB_CCR_DC_Msk) == SCB_CCR_DC_Msk)
            {
                SCB_InvalidateDCache_by_Addr((uint32_t *)(pRxTempPkt)->pDSeg->segLoad, (*pRxPkt)->pDSeg->segLen);
            }
            
			if(frameSize)
			{
				GCIRC_INC(rx_index, pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nRxDescCnt);
				
				(*pRxPkt)->pDSeg->next = pMACDrv->sGmacData.gmac_queue[queueIdx].pRxPckt[rx_index]->pDSeg;
				(*pRxPkt)->pDSeg = (*pRxPkt)->pDSeg->next;	
			}			
			
		}
		//pass the packet status to upper layer
		*pRxStat = (DRV_GMAC_RXDCPT_STATUS)(pMACDrv->sGmacData.gmac_queue[queueIdx].pRxDesc[rx_index].rx_desc_status.val);
		
		(*pRxPkt)->pDSeg = (TCPIP_MAC_DATA_SEGMENT*)pPkt; // restore to first data segment
		
		DRV_PIC32CGMAC_LibRxBuffersAppend(pMACDrv, queueIdx, startIndex, frame_count); //allocate new packets in place of used buffers
		res = DRV_PIC32CGMAC_RES_OK;
		
	}
	
    if(bna_flag == true)
    {
		//Clear Buffer Not Available Flag	
        GMAC_REGS->GMAC_RSR = GMAC_RSR_BNA_Msk ; 
        bna_flag = false;
      	__DMB();          
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
    if(pN)
    {
        if(pL->head == pL->tail)
        {
            pL->head = pL->tail = 0;
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
	pN->next = 0;
	if(pL->tail == 0)
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
	
	if(pN)
	{
		if(pL->head == 0)
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


void DRV_GMAC_LibDescriptorsPoolAdd (DRV_GMAC_DRIVER * pMACDrv, DRV_GMAC_DCPT_TYPE dType)
{
   // GMAC_QUE_LIST queueIdx;
    
    if(dType == DRV_GMAC_DCPT_TYPE_TX)
    {
        pMACDrv->sGmacData.gmac_queue[0].pTxDesc = gmac_dcpt_array.sTxDesc_queue0;
        pMACDrv->sGmacData.gmac_queue[1].pTxDesc = gmac_dcpt_array.sTxDesc_queue1;
        pMACDrv->sGmacData.gmac_queue[2].pTxDesc = gmac_dcpt_array.sTxDesc_queue2;
        pMACDrv->sGmacData.gmac_queue[3].pTxDesc = gmac_dcpt_array.sTxDesc_queue3;
        pMACDrv->sGmacData.gmac_queue[4].pTxDesc = gmac_dcpt_array.sTxDesc_queue4;
        pMACDrv->sGmacData.gmac_queue[5].pTxDesc = gmac_dcpt_array.sTxDesc_queue5;
    }
    else if(dType == DRV_GMAC_DCPT_TYPE_RX)
    {
        pMACDrv->sGmacData.gmac_queue[0].pRxDesc = gmac_dcpt_array.sRxDesc_queue0;
        pMACDrv->sGmacData.gmac_queue[1].pRxDesc = gmac_dcpt_array.sRxDesc_queue1;
        pMACDrv->sGmacData.gmac_queue[2].pRxDesc = gmac_dcpt_array.sRxDesc_queue2;
        pMACDrv->sGmacData.gmac_queue[3].pRxDesc = gmac_dcpt_array.sRxDesc_queue3;
        pMACDrv->sGmacData.gmac_queue[4].pRxDesc = gmac_dcpt_array.sRxDesc_queue4;
        pMACDrv->sGmacData.gmac_queue[5].pRxDesc = gmac_dcpt_array.sRxDesc_queue5;
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

    if(hash->calculate_hash == true) // Calculate hash for given MAC address
    {
        mac_addr = (DRV_GMAC_MAC_ADDR*)(hash->DestMACAddr);

        hash_Index.index = 0;      
        hash_Index.bits.b0 = (mac_addr[5].bits.b0)^(mac_addr[5].bits.b6)^(mac_addr[4].bits.b4)^(mac_addr[3].bits.b2)^(mac_addr[2].bits.b0)^(mac_addr[2].bits.b6)^(mac_addr[1].bits.b4)^(mac_addr[0].bits.b2);
        hash_Index.bits.b1 = (mac_addr[5].bits.b1)^(mac_addr[5].bits.b7)^(mac_addr[4].bits.b5)^(mac_addr[3].bits.b3)^(mac_addr[2].bits.b1)^(mac_addr[2].bits.b7)^(mac_addr[1].bits.b5)^(mac_addr[0].bits.b3);
        hash_Index.bits.b2 = (mac_addr[5].bits.b2)^(mac_addr[4].bits.b0)^(mac_addr[4].bits.b6)^(mac_addr[3].bits.b4)^(mac_addr[2].bits.b2)^(mac_addr[1].bits.b0)^(mac_addr[1].bits.b6)^(mac_addr[0].bits.b4);
        hash_Index.bits.b3 = (mac_addr[5].bits.b3)^(mac_addr[4].bits.b1)^(mac_addr[4].bits.b7)^(mac_addr[3].bits.b5)^(mac_addr[2].bits.b3)^(mac_addr[1].bits.b1)^(mac_addr[1].bits.b7)^(mac_addr[0].bits.b5);
        hash_Index.bits.b4 = (mac_addr[5].bits.b4)^(mac_addr[4].bits.b2)^(mac_addr[3].bits.b0)^(mac_addr[3].bits.b6)^(mac_addr[2].bits.b4)^(mac_addr[1].bits.b2)^(mac_addr[0].bits.b0)^(mac_addr[0].bits.b6);
        hash_Index.bits.b5 = (mac_addr[5].bits.b5)^(mac_addr[4].bits.b3)^(mac_addr[3].bits.b1)^(mac_addr[3].bits.b7)^(mac_addr[2].bits.b5)^(mac_addr[1].bits.b3)^(mac_addr[0].bits.b1)^(mac_addr[0].bits.b7);

        // read the current hash value stored in register
        hash_value = _DRV_GMAC_HashValueGet();
        hash_value  |= (1 << (hash_Index.index));
    }
    else
    {   // Set hash value directly
        hash_value = hash->hash_value;
    }
    _DRV_GMAC_HashValueSet(hash_value);

    return DRV_PIC32CGMAC_RES_OK;
}

DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibSetMacAddr (const uint8_t * pMacAddr)
{
    GMAC_REGS->GMAC_SA[0].GMAC_SAB = (pMacAddr[3] << 24)
                                | (pMacAddr[2] << 16)
                                | (pMacAddr[1] <<  8)
                                | (pMacAddr[0]);

    GMAC_REGS->GMAC_SA[0].GMAC_SAT = (pMacAddr[5] <<  8)
                                | (pMacAddr[4]) ;
    
    return DRV_PIC32CGMAC_RES_OK;
}
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibGetMacAddr (uint8_t * pMacAddr)
{

    pMacAddr[0] = (GMAC_REGS->GMAC_SA[0].GMAC_SAB)& 0xFF;
    pMacAddr[1] = ((GMAC_REGS->GMAC_SA[0].GMAC_SAB)>>8)& 0xFF;
    pMacAddr[2] = ((GMAC_REGS->GMAC_SA[0].GMAC_SAB)>>16)& 0xFF;
    pMacAddr[3] = ((GMAC_REGS->GMAC_SA[0].GMAC_SAB)>>24)& 0xFF;
    pMacAddr[4] = (GMAC_REGS->GMAC_SA[0].GMAC_SAT)& 0xFF;
    pMacAddr[5] = ((GMAC_REGS->GMAC_SA[0].GMAC_SAT)>>8)& 0xFF;
    
    return DRV_PIC32CGMAC_RES_OK;
}

/****************************************************************************
 * Function:        _MacPacketAck
 * Summary: ACK function to free the RX/TX packet
 *****************************************************************************/
static bool _MacPacketAck(TCPIP_MAC_PACKET* pPkt,  const void* param) 
{
	TCPIP_MAC_PACKET* pCurrPkt;

	DRV_GMAC_DRIVER * pMacDrv = (DRV_GMAC_DRIVER *)param;	  

    if(pPkt)
    {
       if(pPkt->pDSeg == 0)
        return false;
    }
    
	while(pPkt != 0)
	{
		pCurrPkt = 0;

        if(pPkt->pDSeg->next)
        {          
            pCurrPkt = (TCPIP_MAC_PACKET*)((uint32_t)(pPkt->pDSeg->next) - ((uint32_t)(pPkt->pDSeg) - (uint32_t)(pPkt)));
        }
		(*pMacDrv->sGmacData.pktFreeF)(pPkt);
		pPkt = pCurrPkt;
        
	}
	return false; 
	
}
