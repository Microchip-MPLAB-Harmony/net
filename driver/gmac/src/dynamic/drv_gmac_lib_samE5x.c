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

/** D E F I N I T I O N S ****************************************************/
//PIC32C internal GMAC peripheral
#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_MAC_PIC32C
// RX Buffer allocation types
#define GMAC_RX_STICKY_BUFFERS	1
#define GMAC_RX_DYNAMIC_BUFFERS	0
#define GMAC_INT_BITS  (GMAC_INT_RX_BITS | GMAC_INT_TX_BITS)

// Maximum RX frame selection
#if (TCPIP_GMAC_RX_MAX_FRAME == 1536)
#define GMAC_MAX_RXFS       1   // extend frame to 1536 bytes
#elif (TCPIP_GMAC_RX_MAX_FRAME == 1518)
#define GMAC_MAX_RXFS       0   // standard frame of 1518 bytes
#else
#error "Wrong TCPIP_GMAC_RX_MAX_FRAME value (1518/1536 allowed)!"
#endif


/******************************************************************************
 * Prototypes
 ******************************************************************************/
static bool _MacRxPacketAck(TCPIP_MAC_PACKET* pkt,  const void* param);
static bool _IsBufferNotAvailable(DRV_GMAC_DRIVER * pMACDrv);
static GMAC_RXFRAME_STATE _SearchRxPacket(DRV_GMAC_DRIVER * pMACDrv,
        DRV_PIC32CGMAC_RX_FRAME_INFO *rx_frame_state, GMAC_QUE_LIST queueIdx);
static DRV_PIC32CGMAC_RESULT _GetRxPacket(DRV_GMAC_DRIVER * pMACDrv,
        DRV_PIC32CGMAC_RX_FRAME_INFO *rx_frame_state, TCPIP_MAC_PACKET** pRxPkt, GMAC_QUE_LIST queueIdx);  
static DRV_PIC32CGMAC_RESULT _AllocateRxPacket(DRV_GMAC_DRIVER * pMACDrv, uint16_t buffer_count, GMAC_QUE_LIST queue_idx, bool sticky_flag);

//GMAC TX and RX Descriptor structure with multiple Queues	
typedef struct
{
	DRV_PIC32CGMAC_HW_TXDCPT sTxDesc_queue0[TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE0];    
    DRV_PIC32CGMAC_HW_RXDCPT sRxDesc_queue0[TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE0];    
} DRV_PIC32CGMAC_HW_DCPT_ARRAY ;   

// place the descriptors in 8-byte aligned memory region
static __attribute__((__aligned__(8))) DRV_PIC32CGMAC_HW_DCPT_ARRAY gmac_dcpt_array;

uint32_t    drvGmacQueEvents;     //Priority Queue Event Status 
//GMAC Interrupt sources for 6 Priority Queues
INT_SOURCE  drvGmacIRQ[DRV_GMAC_NUMBER_OF_QUEUES] = {GMAC_IRQn};
/****************************************************************************
 * Function:        DRV_PIC32CGMAC_LibInit
 * Summary: Initialize GMAC peripheral registers
 *****************************************************************************/
void DRV_PIC32CGMAC_LibInit(DRV_GMAC_DRIVER* pMACDrv) 
{		
	/* Configure the AHB Bridge Clock for GMAC */
    MCLK_REGS->MCLK_AHBMASK |= MCLK_AHBMASK_GMAC(1);	
	/* Configure the APBB Bridge Clock for GMAC */
    MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_GMAC(1);
	
	//disable Tx
	GMAC_REGS->GMAC_NCR &= ~GMAC_NCR_TXEN_Msk;
	//disable Rx
	GMAC_REGS->GMAC_NCR &= ~GMAC_NCR_RXEN_Msk;
	
	//disable all GMAC interrupts for QUEUE 0
	GMAC_REGS->GMAC_IDR = GMAC_INT_ALL;	//Clear statistics register
	GMAC_REGS->GMAC_NCR |=  GMAC_NCR_CLRSTAT_Msk;
	//Clear RX Status
	GMAC_REGS->GMAC_RSR =  GMAC_RSR_RXOVR_Msk | GMAC_RSR_REC_Msk | GMAC_RSR_BNA_Msk  | GMAC_RSR_HNO_Msk;
	//Clear TX Status
	GMAC_REGS->GMAC_TSR = GMAC_TSR_UBR_Msk  | GMAC_TSR_COL_Msk  | GMAC_TSR_RLE_Msk | GMAC_TSR_TXGO_Msk |
																	GMAC_TSR_TFC_Msk  | GMAC_TSR_TXCOMP_Msk  | GMAC_TSR_HRESP_Msk;
									
	//Clear Interrupt status
	GMAC_REGS->GMAC_ISR;

	//Set network configurations like speed, full duplex, copy all frames, no broadcast, 
	// pause enable, remove FCS, MDC clock   
    GMAC_REGS->GMAC_NCFGR = GMAC_NCFGR_SPD(1) | GMAC_NCFGR_FD(1) | GMAC_NCFGR_CLK(3)  |	GMAC_NCFGR_PEN(1)  | GMAC_NCFGR_RFCS(1)
                            | GMAC_NCFGR_MAXFS(GMAC_MAX_RXFS)| GMAC_NCFGR_RXBUFO(pMACDrv->sGmacData._dataOffset);
	
	if((pMACDrv->sGmacData.gmacConfig.checksumOffloadRx) != TCPIP_MAC_CHECKSUM_NONE)
    {
        GMAC_REGS->GMAC_NCFGR |= GMAC_NCFGR_RXCOEN_Msk;
    }
	
	// Set MAC address	    
    DRV_PIC32CGMAC_LibSetMacAddr((const uint8_t *)(pMACDrv->sGmacData.gmacConfig.macAddress.v));
	
	// MII mode config
    //Configure in RMII mode
	if((TCPIP_INTMAC_PHY_CONFIG_FLAGS) & DRV_ETHPHY_CFG_RMII)
		GMAC_REGS->GMAC_UR = GMAC_UR_MII(0); //initial mode set as RMII
	else
		GMAC_REGS->GMAC_UR = GMAC_UR_MII(1); //initial mode set as MII
}




/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibInitTransfer
 * Summary : Configure DMA and interrupts
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibInitTransfer(DRV_GMAC_DRIVER* pMACDrv,GMAC_QUE_LIST queueIdx) 
{
	
	uint16_t wRxDescCnt_temp = pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nRxDescCnt;
	uint16_t wTxDescCnt_temp = pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nTxDescCnt;
	uint16_t wRxBufferSize_temp = pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].rxBufferSize;
	uint32_t queIntEnable = 0;
    
	if (wRxDescCnt_temp < 1 || wTxDescCnt_temp < 1)
    {
        return DRV_PIC32CGMAC_RES_DESC_CNT_ERR;
    }

	if (wRxBufferSize_temp < DRV_GMAC_MIN_RX_SIZE)
    {
        return DRV_PIC32CGMAC_RES_RX_SIZE_ERR;
    }

    if(pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].queueRxEnable == true)
    {
        queIntEnable = GMAC_INT_RX_BITS;
    }
    if(pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].queueTxEnable == true)
    {
        queIntEnable |= GMAC_INT_TX_BITS;
    }
    
	//write dma configuration to register
	GMAC_REGS->GMAC_DCFGR = GMAC_DCFGR_DRBS((wRxBufferSize_temp >> 6)) | GMAC_DCFGR_RXBMS(3) | GMAC_DCFGR_TXPBMS(1) | GMAC_DCFGR_FBLDO(4) | GMAC_DCFGR_DDRP(1);
	
	if((pMACDrv->sGmacData.gmacConfig.checksumOffloadTx) != TCPIP_MAC_CHECKSUM_NONE)
    {
        GMAC_REGS->GMAC_DCFGR |= GMAC_DCFGR_TXCOEN_Msk;
    }
	
	//enable GMAC interrupts
	GMAC_REGS->GMAC_IER = queIntEnable;
    
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
	
	// Clear interrupt status
	GMAC_REGS->GMAC_ISR;

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
        GMAC_REGS->GMAC_UR = GMAC_UR_MII(0);
	}
	else
	{
        //Configure in MII mode
        GMAC_REGS->GMAC_UR = GMAC_UR_MII(1);
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
	DRV_PIC32CGMAC_RESULT gmacRes = DRV_PIC32CGMAC_RES_OK;
    TCPIP_MAC_PACKET *  pPacket;
	uint16_t nRxDescCnt = pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nRxDescCnt;	
	uint8_t desc_idx = start_index;

	while (nDesc_Cnt--)
	{
        //dynamically allocate new rx packets, if number of rx packets are less than threshold
		if((pMACDrv->sGmacData.gmac_queue[queueIdx]._RxQueue.nNodes) < (pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nRxBuffCntThres))
        {      
            gmacRes = _AllocateRxPacket(pMACDrv, pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nRxBuffAllocCnt, queueIdx, GMAC_RX_DYNAMIC_BUFFERS);
            if(gmacRes == DRV_PIC32CGMAC_RES_OUT_OF_MEMORY)
            {
                pMACDrv->sGmacData._rxStat.nRxBuffNotAvailable++;			
                break; 
            }
        }
        
		if(pMACDrv->sGmacData.gmac_queue[queueIdx].pRxPckt[desc_idx] == 0)
		{
			if((pPacket = (TCPIP_MAC_PACKET *)DRV_PIC32CGMAC_SingleListHeadRemove(&pMACDrv->sGmacData.gmac_queue[queueIdx]._RxQueue))!= NULL)
			{
                _DRV_GMAC_RxLock(pMACDrv);			
				/* Reset status value. */
				pMACDrv->sGmacData.gmac_queue[queueIdx].pRxDesc[desc_idx].rx_desc_status.val = 0;
			
                uint32_t segBuffer = (uint32_t)(pPacket->pDSeg->segBuffer) & GMAC_RX_ADDRESS_MASK;   // should be 4-byte aligned
				if (desc_idx == pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nRxDescCnt - 1)
                    pMACDrv->sGmacData.gmac_queue[queueIdx].pRxDesc[desc_idx].rx_desc_buffaddr.val =  segBuffer | GMAC_RX_WRAP_BIT;
                else
                    pMACDrv->sGmacData.gmac_queue[queueIdx].pRxDesc[desc_idx].rx_desc_buffaddr.val =  segBuffer;            

                _DRV_GMAC_RxUnlock(pMACDrv);			

				// set the packet acknowledgment
				pPacket->ackFunc = (TCPIP_MAC_PACKET_ACK_FUNC)_MacRxPacketAck;
				pPacket->ackParam = pMACDrv;
						
				/* Save packet pointer */
				pMACDrv->sGmacData.gmac_queue[queueIdx].pRxPckt[desc_idx] = pPacket;
				pPacket->next = 0;
				
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
	DRV_PIC32CGMAC_RESULT gmacRes = DRV_PIC32CGMAC_RES_OK;
    uint8_t queue_idx, desc_idx;
	TCPIP_MAC_PACKET **pRxPcktAlloc;
    
	for(queue_idx=0; queue_idx < DRV_GMAC_NUMBER_OF_QUEUES; queue_idx++)
	{		
        DRV_PIC32CGMAC_SingleListInitialize(&pMACDrv->sGmacData.gmac_queue[queue_idx]._RxQueue);
        
        //allocate array of rxpckt pointer
        pRxPcktAlloc = (*pMACDrv->sGmacData._callocF)(pMACDrv->sGmacData._AllocH,pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queue_idx].nRxDescCnt, sizeof(TCPIP_MAC_PACKET *));
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
			pMACDrv->sGmacData.gmac_queue[queue_idx].pRxPckt[desc_idx] = 0;
			pMACDrv->sGmacData.gmac_queue[queue_idx].pRxDesc[desc_idx].rx_desc_buffaddr.val = 0;
			pMACDrv->sGmacData.gmac_queue[queue_idx].pRxDesc[desc_idx].rx_desc_status.val = 0;
		}
		pMACDrv->sGmacData.gmac_queue[queue_idx].pRxDesc[desc_idx-1].rx_desc_buffaddr.val |= GMAC_RX_WRAP_BIT;
		
        
        gmacRes = _AllocateRxPacket(pMACDrv, pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queue_idx].nRxDedicatedBuffers, queue_idx, GMAC_RX_STICKY_BUFFERS);	
        if(gmacRes != DRV_PIC32CGMAC_RES_OK)
        {
            break;
        }
        
		gmacRes = _AllocateRxPacket(pMACDrv, pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queue_idx].nRxAddlBuffCount, queue_idx, GMAC_RX_DYNAMIC_BUFFERS);	
        if(gmacRes != DRV_PIC32CGMAC_RES_OK)
		{
            break;
        }
			
		gmacRes = DRV_PIC32CGMAC_LibRxBuffersAppend(pMACDrv, queue_idx,0,pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queue_idx].nRxDescCnt);		
		if(gmacRes != DRV_PIC32CGMAC_RES_OK)
		{
			break;
		}
		
		GMAC_REGS->GMAC_RBQB = GMAC_RBQB_ADDR_Msk & ((uint32_t)pMACDrv->sGmacData.gmac_queue[queue_idx].pRxDesc);
		
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
	uint8_t queue_idx, desc_idx;
	
	for(queue_idx=0; queue_idx < DRV_GMAC_NUMBER_OF_QUEUES; queue_idx++)
	{
		//initialize Tx Queue
		DRV_PIC32CGMAC_SingleListInitialize(&pMACDrv->sGmacData.gmac_queue[queue_idx]._TxQueue);
		for(desc_idx=0; desc_idx < pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queue_idx].nTxDescCnt; desc_idx++)
		{
            pMACDrv->sGmacData.gmac_queue[queue_idx].nTxDescHead = 0;
            pMACDrv->sGmacData.gmac_queue[queue_idx].nTxDescTail = 0;
			pMACDrv->sGmacData.gmac_queue[queue_idx].pTxDesc[desc_idx].tx_desc_buffaddr = 0;
			pMACDrv->sGmacData.gmac_queue[queue_idx].pTxDesc[desc_idx].tx_desc_status.val = GMAC_TX_USED_BIT | GMAC_TX_LAST_BUFFER_BIT;
		}
		pMACDrv->sGmacData.gmac_queue[queue_idx].pTxDesc[desc_idx-1].tx_desc_status.val |= GMAC_TX_WRAP_BIT;

		GMAC_REGS->GMAC_TBQB = GMAC_TBQB_ADDR_Msk & ((uint32_t)pMACDrv->sGmacData.gmac_queue[queue_idx].pTxDesc);

	}	
	return DRV_PIC32CGMAC_RES_OK;
}//DRV_PIC32CGMAC_LibTxInit

/****************************************************************************
 * Function:        DRV_PIC32CGMAC_LibTxSendPacket
 * Summary: Add Tx packet to Tx descriptors and trigger Tx Start
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibTxSendPacket(DRV_GMAC_DRIVER * pMACDrv, TCPIP_MAC_DATA_SEGMENT * pPktDSeg,  GMAC_QUE_LIST queueIdx)  
{
	DRV_PIC32CGMAC_HW_TXDCPT *pTxDesc = pMACDrv->sGmacData.gmac_queue[queueIdx].pTxDesc;
	uint16_t wTxIndex = pMACDrv->sGmacData.gmac_queue[queueIdx].nTxDescHead ;
	uint16_t wTxDescCount =pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nTxDescCnt;
    uint8_t nLoopCnt =0;
    
      
    while (pPktDSeg)
    {
        //check for enough number of tx descriptors available
        if((_DRV_GMAC_DescSpace(wTxIndex, pMACDrv->sGmacData.gmac_queue[queueIdx].nTxDescTail, wTxDescCount)) &&
           (pTxDesc[wTxIndex].tx_desc_buffaddr == 0))
        {
            pTxDesc[wTxIndex].tx_desc_status.val &= (GMAC_TX_WRAP_BIT |GMAC_TX_USED_BIT); //clear all Tx Status except Wrap Bit and Used Bit
            pTxDesc[wTxIndex].tx_desc_buffaddr = (uint32_t)((uint8_t *)pPktDSeg->segLoad);	//set the buffer address
            pTxDesc[wTxIndex].tx_desc_status.val |= (pPktDSeg->segLen) & GMAC_LENGTH_FRAME; //Set Length for each frame
            GCIRC_INC(wTxIndex,wTxDescCount); //Increment the index of Tx Desc
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
    GCIRC_DEC(wTxIndex,wTxDescCount);
    pTxDesc[wTxIndex].tx_desc_status.val |= GMAC_TX_LAST_BUFFER_BIT;

    //Clear the 'Used' bit of descriptors in reverse order to avoid race condition
    while(nLoopCnt)
    {
        pTxDesc[wTxIndex].tx_desc_status.val &= ~GMAC_TX_USED_BIT;
        GCIRC_DEC(wTxIndex,wTxDescCount);
        nLoopCnt--;
    }
    
    //Enable Transmission
    GMAC_REGS->GMAC_NCR |= GMAC_NCR_TSTART_Msk;	
    
	return DRV_PIC32CGMAC_RES_OK;

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
    DRV_PIC32CGMAC_HW_TXDCPT *pTxDesc = pMACDrv->sGmacData.gmac_queue[queueIdx].pTxDesc;
	DRV_PIC32CGMAC_RESULT res = DRV_PIC32CGMAC_RES_NO_PACKET;
	uint16_t tailIndex = pMACDrv->sGmacData.gmac_queue[queueIdx].nTxDescTail;
	uint16_t headIndex = pMACDrv->sGmacData.gmac_queue[queueIdx].nTxDescHead;
    uint8_t* pbuff = 0;    
    
    while((tailIndex != headIndex) &&
          ((pTxDesc[tailIndex].tx_desc_status.val) & GMAC_TX_USED_BIT) &&
          (pTxDesc[tailIndex].tx_desc_buffaddr != 0))
    {
        if(GMAC_TX_ERR_BITS & (pTxDesc[tailIndex].tx_desc_status.val) )
        {   // transmit error
            pMACDrv->sGmacData._txDiscard = true;
            res = DRV_PIC32CGMAC_RES_TX_ERR;
            break;
        }        
        
        // get aligned buffer address from Tx Descriptor Buffer Address
        pbuff = (uint8_t*)((uint32_t)pTxDesc[tailIndex].tx_desc_buffaddr & pMACDrv->sGmacData._dataOffsetMask);
        // get packet address from buffer address
        TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = (TCPIP_MAC_SEGMENT_GAP_DCPT*)(pbuff + pMACDrv->sGmacData._dcptOffset);
        pPkt = pGap->segmentPktPtr;
        
        
        while(tailIndex != headIndex)
        {
            // 'Used' bit only set for first buffer of frame after successful transmission; set it for all buffers
            pTxDesc[tailIndex].tx_desc_status.val |= GMAC_TX_USED_BIT;
            pTxDesc[tailIndex].tx_desc_status.val &= GMAC_TX_USED_BIT | GMAC_TX_WRAP_BIT | GMAC_TX_LAST_BUFFER_BIT;
            pTxDesc[tailIndex].tx_desc_buffaddr = 0;
            if(GMAC_TX_LAST_BUFFER_BIT & (pTxDesc[tailIndex].tx_desc_status.val) )
            {
                pPkt->pktFlags &= ~TCPIP_MAC_PKT_FLAG_QUEUED;
                // Tx Callback
                (pMACDrv->sGmacData.pktAckF)(pPkt, TCPIP_MAC_PKT_ACK_TX_OK, TCPIP_THIS_MODULE_ID);				
                pMACDrv->sGmacData._txStat.nTxOkPackets++;
                // Clear 'Last Buffer' bit
                pTxDesc[tailIndex].tx_desc_status.val &= ~GMAC_TX_LAST_BUFFER_BIT;
                
                //increment the tail for the next packet processing
                GCIRC_INC(tailIndex, pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nTxDescCnt);   
                res = DRV_PIC32CGMAC_RES_OK;
                break;
            } 
            
            //increment the tail for the next packet processing
            GCIRC_INC(tailIndex, pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nTxDescCnt);  
                    
        }        
        
        pMACDrv->sGmacData.gmac_queue[queueIdx].nTxDescTail = tailIndex;
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
    while((pPkt = (TCPIP_MAC_PACKET*)DRV_PIC32CGMAC_SingleListHeadRemove(&pMACDrv->sGmacData.gmac_queue[queueIdx]._TxQueue)) != 0)
	{
        // release pending list packets
        if(*pMACDrv->sGmacData.pktAckF)
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
    uint8_t* pbuff = 0;          
    
	while(tailIndex != headIndex)
    {
        // get aligned buffer address from Tx Descriptor Buffer Address
        pbuff = (uint8_t*)((uint32_t)pTxDesc[tailIndex].tx_desc_buffaddr & pMACDrv->sGmacData._dataOffsetMask);
        // get packet address from buffer address
        TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = (TCPIP_MAC_SEGMENT_GAP_DCPT*)(pbuff + pMACDrv->sGmacData._dcptOffset);
        pPkt = pGap->segmentPktPtr;
        
        pPkt->pktFlags &= ~TCPIP_MAC_PKT_FLAG_QUEUED;
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
 * Function: DRV_PIC32CGMAC_LibRxGetPacket
 * Summary: extract  RX packet from the RX descriptor
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxGetPacket(DRV_GMAC_DRIVER * pMACDrv, TCPIP_MAC_PACKET** pRxPkt, int* pnBuffs, DRV_GMAC_RXDCPT_STATUS* pRxStat, GMAC_QUE_LIST queueIdx)  
{	
	DRV_PIC32CGMAC_RX_FRAME_INFO rx_frame_state = {0,0,0};
    DRV_PIC32CGMAC_RESULT   res = DRV_PIC32CGMAC_RES_NO_PACKET;
	GMAC_RXFRAME_STATE  frameState = GMAC_RX_NO_FRAME_STATE;
    bool bna_flag = 0;	
    
	//Check for BNA error due to shortage of Rx Buffers
	bna_flag = _IsBufferNotAvailable(pMACDrv);
	
	if(!pRxPkt)
	{
        
		return res;
	}
    
    //Search all Rx descriptors for a valid rx packet
	frameState = _SearchRxPacket(pMACDrv, &rx_frame_state, queueIdx);
    
	//Valid packet detected with SOF and EOF
	if (frameState == GMAC_RX_VALID_FRAME_DETECTED_STATE)
	{		
		//pass the packet status to upper layer
		*pRxStat = (pMACDrv->sGmacData.gmac_queue[queueIdx].pRxDesc[rx_frame_state.endIndex].rx_desc_status);		
		//number of buffers to return
		*pnBuffs = rx_frame_state.buffer_count;		
		// extract Rx packet and pass it to upper layer
		res = _GetRxPacket(pMACDrv, &rx_frame_state, pRxPkt, queueIdx);		
	}
	
    if(bna_flag == true)
    {
		//Clear Buffer Not Available Flag	
        GMAC_REGS->GMAC_RSR = GMAC_RSR_BNA_Msk ; 
        bna_flag = false;          
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
    if(dType == DRV_GMAC_DCPT_TYPE_TX)
    {
        pMACDrv->sGmacData.gmac_queue[0].pTxDesc = gmac_dcpt_array.sTxDesc_queue0;

    }
    else if(dType == DRV_GMAC_DCPT_TYPE_RX)
    {
        pMACDrv->sGmacData.gmac_queue[0].pRxDesc = gmac_dcpt_array.sRxDesc_queue0;

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
    GMAC_REGS->SA[0].GMAC_SAB = (pMacAddr[3] << 24)
                                | (pMacAddr[2] << 16)
                                | (pMacAddr[1] <<  8)
                                | (pMacAddr[0]);

    GMAC_REGS->SA[0].GMAC_SAT = (pMacAddr[5] <<  8)
                                | (pMacAddr[4]) ;
    
    return DRV_PIC32CGMAC_RES_OK;
}
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibGetMacAddr (uint8_t * pMacAddr)
{

    pMacAddr[0] = (GMAC_REGS->SA[0].GMAC_SAB)& 0xFF;
    pMacAddr[1] = ((GMAC_REGS->SA[0].GMAC_SAB)>>8)& 0xFF;
    pMacAddr[2] = ((GMAC_REGS->SA[0].GMAC_SAB)>>16)& 0xFF;
    pMacAddr[3] = ((GMAC_REGS->SA[0].GMAC_SAB)>>24)& 0xFF;
    pMacAddr[4] = (GMAC_REGS->SA[0].GMAC_SAT)& 0xFF;
    pMacAddr[5] = ((GMAC_REGS->SA[0].GMAC_SAT)>>8)& 0xFF;
    
    return DRV_PIC32CGMAC_RES_OK;
}

/****************************************************************************
 * Function:    DRV_PIC32CGMAC_LibRxBuffersCountGet
 * Summary :    Returns the number of pending RX buffers and scheduled buffers in the GMAC queues.
 *****************************************************************************/
DRV_PIC32CGMAC_RESULT DRV_PIC32CGMAC_LibRxBuffersCountGet(DRV_GMAC_DRIVER* pMACDrv, int* pendBuffs, int* schedBuffs)
{
    int pend_buffer_cnt = 0;
    int sched_buffer_cnt = 0;
    
     
    for(uint16_t queue_idx=0; queue_idx < DRV_GMAC_NUMBER_OF_QUEUES; queue_idx++)	
    {
        if(pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queue_idx].queueRxEnable == true)
        {
            for(uint16_t desc_idx=0; desc_idx < pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queue_idx].nRxDescCnt; desc_idx++)
            {                
                if ((pMACDrv->sGmacData.gmac_queue[queue_idx].pRxDesc[desc_idx].rx_desc_buffaddr.val) & GMAC_RX_ADDRESS_MASK)
                {
                    if ((pMACDrv->sGmacData.gmac_queue[queue_idx].pRxDesc[desc_idx].rx_desc_buffaddr.val) & GMAC_RX_OWNERSHIP_BIT)
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
    
    if(pendBuffs)
    {
        *pendBuffs = pend_buffer_cnt;
    }
    if(schedBuffs)
    {
        *schedBuffs = sched_buffer_cnt;
    }
    return DRV_PIC32CGMAC_RES_OK;
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibSetInterruptSrc
 * Summary: update GMAC Queue structure with interrupt source
 *****************************************************************************/
void DRV_PIC32CGMAC_LibSetInterruptSrc(DRV_GMAC_DRIVER* pMACDrv)
{
    pMACDrv->sGmacData.gmac_queue[GMAC_QUE_0]._queIntSrc = drvGmacIRQ[GMAC_QUE_0];    
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibSetPriorityToQueueNum
 * Summary: mapping priority to GMAC Queue number
 *****************************************************************************/
bool DRV_PIC32CGMAC_LibSetPriorityToQueueNum(DRV_GMAC_DRIVER* pMACDrv) 
{        
    pMACDrv->sGmacData.gmacConfig.txPrioNumToQueIndx[0] = GMAC_QUE_0;
    pMACDrv->sGmacData.gmacConfig.rxPrioNumToQueIndx[0] = GMAC_QUE_0;    
    
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
uint8_t DRV_PIC32CGMAC_LibGetHighPrioReadyQue(void)
{
    if(drvGmacQueEvents & GMAC_QUE0_MASK)
    {
        return (uint8_t)GMAC_QUE_0;            
    }    
    return DRV_GMAC_NO_ACTIVE_QUEUE;
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibClearPriorityQue
 * Summary: Clear the ready status of  priority queue
 *****************************************************************************/
void DRV_PIC32CGMAC_LibClearPriorityQue(DRV_GMAC_DRIVER *pMACDrv, GMAC_QUE_LIST queueIdx)
{
    bool intStat;
    intStat = SYS_INT_SourceDisable(pMACDrv->sGmacData.gmac_queue[GMAC_QUE_0]._queIntSrc);
    drvGmacQueEvents &= ~(1<<queueIdx);
    __DMB();
    SYS_INT_SourceRestore(pMACDrv->sGmacData.gmac_queue[GMAC_QUE_0]._queIntSrc, intStat);
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibSysInt_Disable
 * Summary: Disable all GMAC interrupts specified in queue mask
 *****************************************************************************/
void DRV_PIC32CGMAC_LibSysInt_Disable(DRV_GMAC_DRIVER *pMACDrv, uint32_t queMask, bool *queStat)
{
    if(queMask & GMAC_QUE0_MASK)
    {
        if(queStat)
        {
            queStat[GMAC_QUE_0] = SYS_INT_SourceDisable(pMACDrv->sGmacData.gmac_queue[GMAC_QUE_0]._queIntSrc);
        }
        else
        {
            SYS_INT_SourceDisable(pMACDrv->sGmacData.gmac_queue[GMAC_QUE_0]._queIntSrc);
        }

    }
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibSysIntStatus_Clear
 * Summary: Clear all GMAC interrupts status specified in queue mask
 *****************************************************************************/
void DRV_PIC32CGMAC_LibSysIntStatus_Clear(DRV_GMAC_DRIVER *pMACDrv, uint32_t queMask)
{
    if(queMask & GMAC_QUE0_MASK)
    {
        SYS_INT_SourceStatusClear(pMACDrv->sGmacData.gmac_queue[GMAC_QUE_0]._queIntSrc);                        
    }    
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibSysInt_Enable
 * Summary: Enable all GMAC interrupts specified in queue mask 
 *****************************************************************************/
void DRV_PIC32CGMAC_LibSysInt_Enable(DRV_GMAC_DRIVER *pMACDrv, uint32_t queMask)
{
    if(queMask & GMAC_QUE0_MASK)
    {
        SYS_INT_SourceEnable(pMACDrv->sGmacData.gmac_queue[GMAC_QUE_0]._queIntSrc);
    }
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibSysInt_Restore
 * Summary: Restore all GMAC interrupts specified in queue mask
 *****************************************************************************/
void DRV_PIC32CGMAC_LibSysInt_Restore(DRV_GMAC_DRIVER *pMACDrv, uint32_t queMask, bool *queStat)
{
    if(queMask & GMAC_QUE0_MASK)
    {
        if(queStat)
        {
            SYS_INT_SourceRestore(pMACDrv->sGmacData.gmac_queue[GMAC_QUE_0]._queIntSrc, queStat[GMAC_QUE_0]); 
        }            
    }
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibReadInterruptStatus
 * Summary: read GMAC interrupt status
 *****************************************************************************/
uint32_t DRV_PIC32CGMAC_LibReadInterruptStatus(GMAC_QUE_LIST queueIdx)
{
    return GMAC_REGS->GMAC_ISR;
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibSysInt_Restore
 * Summary: enable GMAC interrupt events
 *****************************************************************************/
void DRV_PIC32CGMAC_LibEnableInterrupt(GMAC_QUE_LIST queueIdx, GMAC_EVENTS ethEvents)
{
    GMAC_REGS->GMAC_IER = ethEvents;
}

/****************************************************************************
 * Function: DRV_PIC32CGMAC_LibSysInt_Restore
 * Summary: disable GMAC interrupt events
 *****************************************************************************/
void DRV_PIC32CGMAC_LibDisableInterrupt(GMAC_QUE_LIST queueIdx, GMAC_EVENTS ethEvents)
{
    GMAC_REGS->GMAC_IDR = ethEvents;
}
/****************************************************************************
 * GMAC Interrupt Service Routines(ISR)
 *****************************************************************************/
//GMAC interrupt handler for Priority Queue 0
void GMAC_InterruptHandler(void)
{
    GMAC_EVENTS  currEthEvents = (GMAC_EVENTS)GMAC_REGS->GMAC_ISR;
    GMAC_REGS->GMAC_IDR = currEthEvents;
    if(currEthEvents & GMAC_EV_RX_ALL)
    {
        drvGmacQueEvents |= GMAC_QUE0_MASK;
    }
    __DMB();
	DRV_GMAC_Tasks_ISR((SYS_MODULE_OBJ)0, (uint32_t)currEthEvents);
}


/****************************************************************************
 * Function:        _MacRxPacketAck
 * Summary: ACK function to free the RX packet
 *****************************************************************************/
static bool _MacRxPacketAck(TCPIP_MAC_PACKET* pPkt,  const void* param) 
{
    TCPIP_MAC_DATA_SEGMENT *    pDSegNext;
	DRV_GMAC_DRIVER * pMacDrv = (DRV_GMAC_DRIVER *)param; 
    //hard-coded for queue 0
    GMAC_QUE_LIST queueIdx = GMAC_QUE_0;
    bool res = false;

    if(pPkt && pPkt->pDSeg)
    {
        while(pPkt->pDSeg)
        {
            pDSegNext = pPkt->pDSeg->next;
            pPkt->pDSeg->next = 0;
            
            // for dynamic packets(non-sticky):
            // always free if NO_SMART_ALLOC flag is set
            // free if RX packets greater than the defined threshold
            if((pPkt->pDSeg->segFlags  & TCPIP_MAC_SEG_FLAG_RX_STICKY) == 0 && 
                (((pMacDrv->sGmacData._controlFlags & TCPIP_MAC_CONTROL_NO_SMART_ALLOC) != 0) ||
                    (pMacDrv->sGmacData.gmac_queue[queueIdx]._RxQueue.nNodes > 
                        pMacDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx].nRxBuffCntThres)))
            {            
                (*pMacDrv->sGmacData.pktFreeF)(pPkt);          
            }		
            else
            {
                // add the packet to new queue for re-use
                DRV_PIC32CGMAC_SingleListTailAdd(&pMacDrv->sGmacData.gmac_queue[queueIdx]._RxQueue, (DRV_PIC32CGMAC_SGL_LIST_NODE*)pPkt); 

            }
   
            if( !pDSegNext )
            {   // packet handling is now complete
                break;
            }
            // Ethernet packet stored in multiple MAC descriptors, each segment
            // is allocated as a complete mac packet
            // extract the packet pointer using the segment load buffer
            TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = (TCPIP_MAC_SEGMENT_GAP_DCPT*)(pDSegNext->segBuffer + pMacDrv->sGmacData._dcptOffset);
            pPkt = pGap->segmentPktPtr;
        }	
        res  = true; 
    }
    
    return res;	
}

/****************************************************************************
 * Function:        _IsBufferNotAvailable
 * Summary: Check if 'Buffer Not Available' set by GMAC; if set, clear the BNA status bit
 * Return : True if BNA is set; False for no BNA status
 *****************************************************************************/
static bool _IsBufferNotAvailable(DRV_GMAC_DRIVER * pMACDrv)
{
    if(GMAC_REGS->GMAC_RSR & GMAC_RSR_BNA_Msk ) //Check for BNA error due to shortage of Rx Buffers
    {       
		//Clear GMAC 'Buffer Not Available' Flag			
		GMAC_REGS->GMAC_RSR = GMAC_RSR_BNA_Msk ;             
		__DMB();  
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
static GMAC_RXFRAME_STATE _SearchRxPacket(DRV_GMAC_DRIVER * pMACDrv,DRV_PIC32CGMAC_RX_FRAME_INFO *rx_frame_state, GMAC_QUE_LIST queueIdx)  
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
                        rx_index =  fixed_mod(((rx_frame_state->startIndex) + (rx_frame_state->buffer_count) -1),nRxDscCnt);	
                        //number of rx descriptors used by error packet
                        nRx_buffer = rx_frame_state->buffer_count;
                        //reset the Rx packet search state machine
                        frameState = GMAC_RX_NO_FRAME_STATE;

                        //clear all the descriptors before the new SOF
                        while(nRx_buffer--)
                        {                            
                            //Get Rx Packet assigned to Rx Descriptor with error
                            pRxPkt = pMACDrv->sGmacData.gmac_queue[queueIdx].pRxPckt[rx_index];
                            //Call Rx acknowledgment function to free/reuse the Rx packet
                            (pRxPkt->ackFunc)(pRxPkt, pRxPkt->ackParam);
                            //remove rx packet from rx desc
                            pMACDrv->sGmacData.gmac_queue[queueIdx].pRxPckt[rx_index] = 0; 
                            _DRV_GMAC_RxLock(pMACDrv);
                            //clear the buffer address bitfields
                            pMACDrv->sGmacData.gmac_queue[queueIdx].pRxDesc[rx_index].rx_desc_buffaddr.val &= ~GMAC_RX_ADDRESS_MASK; 
                            _DRV_GMAC_RxUnlock(pMACDrv);
                            //decrement index
                            GCIRC_DEC(rx_index, nRxDscCnt); 

                        }
                        // refill the cleared rx descriptors
                        DRV_PIC32CGMAC_LibRxBuffersAppend(pMACDrv, queueIdx, (rx_frame_state->startIndex), (rx_frame_state->buffer_count));
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
                //get index of rx descriptor with error
				rx_index = fixed_mod(((rx_frame_state->startIndex) + (rx_frame_state->buffer_count) -1),nRxDscCnt);	
                //number of rx descriptors used by error packet
				nRx_buffer = rx_frame_state->buffer_count;
                //reset the Rx packet search state machine
				frameState = GMAC_RX_NO_FRAME_STATE;
                
				//clear all the descriptors before the error				
				while(nRx_buffer--)
				{                  
                    //Get Rx Packet assigned to Rx Descriptor with error
                    pRxPkt = pMACDrv->sGmacData.gmac_queue[queueIdx].pRxPckt[rx_index];
                    //call the rx packet ack function, to reuse/free the packet
                    (pRxPkt->ackFunc)(pRxPkt, pRxPkt->ackParam);	
                    //remove rx packet from rx desc
					pMACDrv->sGmacData.gmac_queue[queueIdx].pRxPckt[rx_index] = 0;
                    _DRV_GMAC_RxLock(pMACDrv);
                    //clear the buffer address bit fields
					pMACDrv->sGmacData.gmac_queue[queueIdx].pRxDesc[rx_index].rx_desc_buffaddr.val &= ~GMAC_RX_ADDRESS_MASK; 
					_DRV_GMAC_RxUnlock(pMACDrv);
                    //decrement rx index to restart searching from new SOF frame
                    GCIRC_DEC(rx_index, nRxDscCnt);
					
				}		
                // refill the cleared rx descriptors
				DRV_PIC32CGMAC_LibRxBuffersAppend(pMACDrv, queueIdx, (rx_frame_state->startIndex), (rx_frame_state->buffer_count));
				//decrement search count 
                search_count--; 
                //decrement rx index to restart searching from new SOF frame	
				GCIRC_DEC(nRxDescIndex, nRxDscCnt); 		
			}
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
 * Function:        _GetRxPacket
 * Summary: Get Rx packet buffer from GMAC and transfer to TCP/IP stack. 
 * Assign new Rx buffers to GMAC
 * Return : new buffer allocation status
 *****************************************************************************/
static DRV_PIC32CGMAC_RESULT _GetRxPacket(DRV_GMAC_DRIVER * pMACDrv,DRV_PIC32CGMAC_RX_FRAME_INFO *rx_frame_state, TCPIP_MAC_PACKET** pRxPkt, GMAC_QUE_LIST queueIdx)  
{	
    TCPIP_MAC_DATA_SEGMENT *pDseg;
    DRV_PIC32CGMAC_HW_RXDCPT *pRxDesc = pMACDrv->sGmacData.gmac_queue[queueIdx].pRxDesc;
    DRV_GMAC_QUEUE gmac_queue = pMACDrv->sGmacData.gmac_queue[queueIdx];
    TCPIP_MODULE_GMAC_QUEUE_CONFIG gmac_queue_config = pMACDrv->sGmacData.gmacConfig.gmac_queue_config[queueIdx];
	DRV_PIC32CGMAC_RESULT   res = DRV_PIC32CGMAC_RES_NO_PACKET;
	uint32_t frameSize = 0;
	uint16_t rx_index;
	uint16_t nRx_buffer;
	
	
	*pRxPkt = gmac_queue.pRxPckt[rx_frame_state->startIndex]; 
	nRx_buffer = rx_frame_state->buffer_count;		
	rx_index = rx_frame_state->startIndex;
	
	//calculate the size of frame
	frameSize = (pRxDesc[rx_frame_state->endIndex].rx_desc_status.val & GMAC_LENGTH_FRAME); 
	
	//backup of data Segment for later use
	pDseg = (*pRxPkt)->pDSeg; 
	
	if(nRx_buffer)
    {
		//process all the packet buffers
		while(nRx_buffer--) 
		{
            // Segment length based on received frame length and Rx buffer size
			(*pRxPkt)->pDSeg->segLen = (frameSize >= gmac_queue_config.rxBufferSize) ?
												gmac_queue_config.rxBufferSize :frameSize;
													
			(*pRxPkt)->pDSeg->segSize = gmac_queue_config.rxBufferSize;
			
			// RX frame bigger than GMAC RX buffer?
			if(frameSize >= gmac_queue_config.rxBufferSize)
            {
                //more Rx buffers used for Rx packet
				frameSize = frameSize - gmac_queue_config.rxBufferSize;
            }
			else
            {   // last Rx buffer of Rx Packet
				frameSize = 0;		
            }
			
			//release the rx packets from GMAC queue
			gmac_queue.pRxPckt[rx_index] = 0; 
			
			_DRV_GMAC_RxLock(pMACDrv);
			//clear the buffer address bit-fields
			gmac_queue.pRxDesc[rx_index].rx_desc_buffaddr.val &= ~GMAC_RX_ADDRESS_MASK; 
			_DRV_GMAC_RxUnlock(pMACDrv);

			//more Rx buffers needed for Rx packet
			if(frameSize)
			{
				GCIRC_INC(rx_index, gmac_queue_config.nRxDescCnt);
				
				(*pRxPkt)->pDSeg->next = gmac_queue.pRxPckt[rx_index]->pDSeg;
				(*pRxPkt)->pDSeg = (*pRxPkt)->pDSeg->next;	
			}			
			
		}
			
		// restore to first data segment
		(*pRxPkt)->pDSeg = (TCPIP_MAC_DATA_SEGMENT*)pDseg; 
		
		//allocate new packets in place of used buffers
		DRV_PIC32CGMAC_LibRxBuffersAppend(pMACDrv, queueIdx, rx_frame_state->startIndex, rx_frame_state->buffer_count); 
		res = DRV_PIC32CGMAC_RES_OK;
	}
	else
    {
        res = DRV_PIC32CGMAC_RES_NO_PACKET;
    }
	
	return res;
}

/****************************************************************************
 * Function:        _AllocateRxPacket
 * Summary: Dynamically allocate Rx packet
 * Allocate new Rx buffers to GMAC
 * Return : new buffer allocation status
 *****************************************************************************/
static DRV_PIC32CGMAC_RESULT _AllocateRxPacket(DRV_GMAC_DRIVER * pMACDrv, uint16_t buffer_count, GMAC_QUE_LIST queue_idx, bool sticky_flag)
{
	TCPIP_MAC_PACKET* pRxPkt;
    DRV_PIC32CGMAC_RESULT gmacAllocRes = DRV_PIC32CGMAC_RES_OK;
	uint8_t rxbuff_idx;
	
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
            pRxPkt->pDSeg->segFlags |=  TCPIP_MAC_SEG_FLAG_RX_STICKY;
        }
        
		DRV_PIC32CGMAC_SingleListTailAdd(&pMACDrv->sGmacData.gmac_queue[queue_idx]._RxQueue, (DRV_PIC32CGMAC_SGL_LIST_NODE*)pRxPkt); 
    }
	return gmacAllocRes;
}	

