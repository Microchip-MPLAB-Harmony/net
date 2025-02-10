/*******************************************************************************
  Ethernet Driver Library Source Code

Summary:
This file contains the source code for the Ethernet Driver library.

Description:
This library provides a low-level abstraction of the Ethernet module
on Microchip PIC32MX/PIC32MZ family microcontrollers with a convenient C language
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


#include "./drv_eth_pic32_lib.h"

#define NOP __asm__ __volatile__ ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;")

void DRV_ETH_RxBufferCountDecrement(DRV_ETHERNET_REGISTERS* pEthReg)
{
    pEthReg->ETHCON1SET =_ETHCON1_BUFCDEC_MASK;
}

void DRV_ETH_MIIResetEnable(DRV_ETHERNET_REGISTERS* pEthReg)
{
    pEthReg->EMACxCFG1SET=_EMACxCFG1_SOFTRESET_MASK;
}

void DRV_ETH_MIIResetDisable(DRV_ETHERNET_REGISTERS* pEthReg)
{
    pEthReg->EMACxCFG1CLR =_EMACxCFG1_SOFTRESET_MASK;
}

void DRV_ETH_MaxFrameLengthSet(DRV_ETHERNET_REGISTERS* pEthReg, uint16_t MaxFrameLength)
{
    pEthReg->EMACxMAXF.MACMAXF = (uint16_t)((uint32_t)_EMACxMAXF_MACMAXF_MASK & ((uint32_t)MaxFrameLength << _EMACxMAXF_MACMAXF_POSITION));
}

void DRV_ETH_Disable(DRV_ETHERNET_REGISTERS* pEthReg)
{
    pEthReg->ETHCON1CLR = _ETHCON1_ON_MASK;	    
}

void DRV_ETH_Enable(DRV_ETHERNET_REGISTERS* pEthReg)
{
    pEthReg->ETHCON1SET = _ETHCON1_ON_MASK;	    
}

void DRV_ETH_TxRTSDisable(DRV_ETHERNET_REGISTERS* pEthReg)
{
    pEthReg->ETHCON1CLR =_ETHCON1_TXRTS_MASK;    
}

void DRV_ETH_TxRTSEnable(DRV_ETHERNET_REGISTERS* pEthReg)
{
    pEthReg->ETHCON1SET =_ETHCON1_TXRTS_MASK;
}

void DRV_ETH_RxDisable(DRV_ETHERNET_REGISTERS* pEthReg)
{
    pEthReg->ETHCON1CLR =_ETHCON1_RXEN_MASK;
}

void DRV_ETH_RxEnable(DRV_ETHERNET_REGISTERS* pEthReg)
{
    pEthReg->ETHCON1SET =_ETHCON1_RXEN_MASK;
}

bool DRV_ETH_EthernetIsBusy(DRV_ETHERNET_REGISTERS* pEthReg)
{
    return (bool)(pEthReg->ETHSTAT.BUSY);                          
}

uint8_t DRV_ETH_RxPacketCountGet(DRV_ETHERNET_REGISTERS* pEthReg)
{
    return (uint8_t)(pEthReg->ETHSTAT.BUFCNT); 
}

bool DRV_ETH_TransmitIsBusy(DRV_ETHERNET_REGISTERS* pEthReg)
{
    return (bool)(pEthReg->ETHSTAT.TXBUSY); 
}

bool DRV_ETH_ReceiveIsBusy(DRV_ETHERNET_REGISTERS* pEthReg)
{
    return (bool)(pEthReg->ETHSTAT.RXBUSY); 
}

void DRV_ETH_TxPacketDescAddrSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t *txPacketDescStartAddr)
{
    pEthReg->ETHTXST.TXSTADDR = ((uint32_t)txPacketDescStartAddr>>2);
}

uint8_t* DRV_ETH_TxPacketDescAddrGet(DRV_ETHERNET_REGISTERS* pEthReg)
{
    return (uint8_t *)pEthReg->ETHTXST.w;
}

void DRV_ETH_RxPacketDescAddrSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t *rxPacketDescStartAddr)
{
    pEthReg->ETHRXST.RXSTADDR = ((uint32_t)rxPacketDescStartAddr>>2);
}

uint8_t* DRV_ETH_RxPacketDescAddrGet(DRV_ETHERNET_REGISTERS* pEthReg)
{
    return (uint8_t *)pEthReg->ETHRXST.w;
}

void DRV_ETH_BackToBackIPGSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t backToBackIPGValue)
{
    pEthReg->EMACxIPGTCLR = _EMAC1IPGT_B2BIPKTGP_MASK;
    pEthReg->EMACxIPGTSET = (uint32_t)_EMAC1IPGT_B2BIPKTGP_MASK & ((uint32_t)backToBackIPGValue << _EMAC1IPGT_B2BIPKTGP_POSITION);	 
}

void DRV_ETH_NonBackToBackIPG1Set(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t nonBackToBackIPGValue)
{
    pEthReg->EMACxIPGRCLR = _EMACxIPGR_NB2BIPKTGP1_MASK;
    pEthReg->EMACxIPGRSET = (uint32_t)_EMACxIPGR_NB2BIPKTGP1_MASK & ((uint32_t)nonBackToBackIPGValue << _EMACxIPGR_NB2BIPKTGP1_POSITION);	
}

void DRV_ETH_NonBackToBackIPG2Set(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t nonBackToBackIPGValue)
{
    pEthReg->EMACxIPGRCLR = _EMACxIPGR_NB2BIPKTGP2_MASK;
    pEthReg->EMACxIPGRSET = (uint32_t)_EMACxIPGR_NB2BIPKTGP2_MASK & ((uint32_t)nonBackToBackIPGValue <<_EMACxIPGR_NB2BIPKTGP2_POSITION);	
}

void DRV_ETH_CollisionWindowSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t collisionWindowValue)
{
    pEthReg->EMACxCLRT.CWINDOW = collisionWindowValue;
}

void DRV_ETH_ReTxMaxSet(DRV_ETHERNET_REGISTERS* pEthReg, uint16_t retransmitMax)
{
    pEthReg->EMACxCLRT.RETX = (uint8_t)retransmitMax;                                             
}

void DRV_ETH_RMIIResetEnable(DRV_ETHERNET_REGISTERS* pEthReg)
{
    pEthReg->EMACxSUPPSET = _EMACxSUPP_RESETRMII_MASK; 
}

void DRV_ETH_RMIIResetDisable(DRV_ETHERNET_REGISTERS* pEthReg)
{
    pEthReg->EMACxSUPP.RESETRMII = 0U;
}

void DRV_ETH_RMIISpeedSet(DRV_ETHERNET_REGISTERS* pEthReg, DRV_ETH_RMII_SPEED RMIISpeed)
{
    pEthReg->EMACxSUPP.SPEEDRMII = (uint8_t)RMIISpeed ;	
}

void DRV_ETH_RxFiltersClr(DRV_ETHERNET_REGISTERS* pEthReg, DRV_ETH_RX_FILTERS rxFilters)
{
    pEthReg->ETHRXFCCLR = (uint32_t)rxFilters & (uint32_t)DRV_ETH_FILT_ALL_FILTERS;	
}

void DRV_ETH_RxFiltersSet(DRV_ETHERNET_REGISTERS* pEthReg, DRV_ETH_RX_FILTERS rxFilters)
{
    pEthReg->ETHRXFCSET = ((uint32_t)rxFilters  & (uint32_t)DRV_ETH_FILT_ALL_FILTERS);	
}

void DRV_ETH_PauseTimerSet(DRV_ETHERNET_REGISTERS* pEthReg, uint16_t PauseTimerValue)
{
    pEthReg->ETHCON1SET = _ETHCON1_PTV_MASK & ((uint32_t)PauseTimerValue << _ETHCON1_PTV_POSITION);              
}

void DRV_ETH_RxFullWmarkSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t watermarkValue)
{
    pEthReg->ETHRXWM.RXFWM = watermarkValue;         
}

void DRV_ETH_RxEmptyWmarkSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t watermarkValue)
{
    pEthReg->ETHRXWM.RXEWM = watermarkValue;        
}

void DRV_ETH_TxPauseEnable(DRV_ETHERNET_REGISTERS* pEthReg)
{
    pEthReg->EMACxCFG1SET = _EMACxCFG1_TXPAUSE_MASK;
}

void DRV_ETH_RxPauseEnable(DRV_ETHERNET_REGISTERS* pEthReg)
{
    pEthReg->EMACxCFG1SET = _EMACxCFG1_RXPAUSE_MASK;
}

void DRV_ETH_AutoFlowControlEnable(DRV_ETHERNET_REGISTERS* pEthReg)
{
    pEthReg->ETHCON1SET = _ETHCON1_AUTOFC_MASK;
}

void DRV_ETH_RxFiltersHTSet(DRV_ETHERNET_REGISTERS* pEthReg, uint64_t htable)
{
    pEthReg->ETHHT0SET = _ETHHT0_HTLOWER_MASK & ((uint32_t)htable);
    pEthReg->ETHHT1SET = _ETHHT1_HTUPPER_MASK &((uint32_t)(htable>>32));
}

int DRV_ETH_RxSetBufferSize(DRV_ETHERNET_REGISTERS* pEthReg, int rxBuffSize)
{
    rxBuffSize >>= 4;     // truncate
    if(!rxBuffSize)
    {
        return -1;
    }

    pEthReg->ETHCON2CLR = _ETHCON2_RXBUF_SZ_MASK;
    pEthReg->ETHCON2SET = (uint32_t)_ETHCON2_RXBUF_SZ_MASK & ((uint32_t)rxBuffSize << _ETHCON2_RXBUF_SZ_POSITION);

    return rxBuffSize << 4;
}

void DRV_ETH_MACSetAddress(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t *bAddress)
{
    pEthReg->EMACxSA2.STNADDR1  = *(bAddress + 0);
    pEthReg->EMACxSA2.STNADDR2  = *(bAddress + 1);
    pEthReg->EMACxSA1.STNADDR3  = *(bAddress + 2);
    pEthReg->EMACxSA1.STNADDR4  = *(bAddress + 3);
    pEthReg->EMACxSA0.STNADDR5  = *(bAddress + 4);
    pEthReg->EMACxSA0.STNADDR6  = *(bAddress + 5);
}

void DRV_ETH_MACGetAddress(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t *bAddress )
{
    *(bAddress + 0) = pEthReg->EMACxSA2.STNADDR1;
    *(bAddress + 1) = pEthReg->EMACxSA2.STNADDR2;
    *(bAddress + 2) = pEthReg->EMACxSA1.STNADDR3;
    *(bAddress + 3) = pEthReg->EMACxSA1.STNADDR4;
    *(bAddress + 4) = pEthReg->EMACxSA0.STNADDR5;
    *(bAddress + 5) = pEthReg->EMACxSA0.STNADDR6;
}

uint32_t DRV_ETH_FramesTxdOkCountGet(DRV_ETHERNET_REGISTERS* pEthReg)
{
    return pEthReg->ETHFRMTXOK.FRMTXOKCNT;
}

uint32_t DRV_ETH_FramesRxdOkCountGet(DRV_ETHERNET_REGISTERS* pEthReg)
{
    return pEthReg->ETHFRMRXOK.FRMRXOKCNT;
}

uint32_t DRV_ETH_RxOverflowCountGet(DRV_ETHERNET_REGISTERS* pEthReg)
{
    return pEthReg->ETHRXOVFLOW.RXOVFLWCNT;
}

uint32_t DRV_ETH_FCSErrorCountGet(DRV_ETHERNET_REGISTERS* pEthReg)
{
    return pEthReg->ETHFCSERR.FCSERRCNT;
}

uint32_t DRV_ETH_AlignErrorCountGet(DRV_ETHERNET_REGISTERS* pEthReg)
{
    return (pEthReg->ETHALGNERR.ALGNERRCNT);
}

uint32_t DRV_ETH_SingleCollisionCountGet(DRV_ETHERNET_REGISTERS* pEthReg)
{
    return pEthReg->ETHSCOLFRM.SCOLFRMCNT;
}

uint32_t DRV_ETH_MultipleCollisionCountGet(DRV_ETHERNET_REGISTERS* pEthReg)
{
    return pEthReg->ETHMCOLFRM.MCOLFRMCNT;
}


void DRV_ETH_EventsClr(DRV_ETHERNET_REGISTERS* pEthReg, DRV_ETH_EVENTS eEvents )
{
    pEthReg->ETHIRQCLR = _ETHIRQ_w_MASK & ((uint32_t)eEvents << _ETHIRQ_w_POSITION);
}

void DRV_ETH_EventsEnableSet(DRV_ETHERNET_REGISTERS* pEthReg, DRV_ETH_EVENTS eEvents )
{
    pEthReg->ETHIENSET = _ETHIEN_w_MASK & ((uint32_t)eEvents <<_ETHIEN_w_POSITION);
}

void DRV_ETH_EventsEnableClr(DRV_ETHERNET_REGISTERS* pEthReg, DRV_ETH_EVENTS eEvents)
{
    pEthReg->ETHIENCLR = _ETHIEN_w_MASK & ((uint32_t)eEvents <<_ETHIEN_w_POSITION);
}

DRV_ETH_EVENTS DRV_ETH_EventsGet(DRV_ETHERNET_REGISTERS* pEthReg)
{
    return  (DRV_ETH_EVENTS) pEthReg->ETHIRQ.w;
}


void DRV_ETH_EventsClear(DRV_ETHERNET_REGISTERS* pEthReg, DRV_ETH_EVENTS evmask)
{
    pEthReg->ETHIRQCLR = _ETHIRQ_w_MASK & ((uint32_t)evmask << _ETHIRQ_w_POSITION);
}


bool DRV_ETH_IsEnabled(DRV_ETHERNET_REGISTERS* pEthReg)
{
    return (bool)(pEthReg->ETHCON1.ON) ;      
}

void DRV_ETH_MIIMNoPreEnable(DRV_ETHERNET_REGISTERS* pEthReg)
{    	
	pEthReg->EMACxMCFGSET = _EMACxMCFG_NOPRE_MASK;	
}

void DRV_ETH_MIIMNoPreDisable(DRV_ETHERNET_REGISTERS* pEthReg)
{	
	pEthReg->EMACxMCFGCLR = _EMACxMCFG_NOPRE_MASK;	
}

void DRV_ETH_MIIMScanIncrEnable(DRV_ETHERNET_REGISTERS* pEthReg)
{    	
	pEthReg->EMACxMCFGSET = _EMACxMCFG_SCANINC_MASK;        
}


void DRV_ETH_MIIMScanIncrDisable(DRV_ETHERNET_REGISTERS* pEthReg)
{        
	pEthReg->EMACxMCFGCLR = _EMACxMCFG_SCANINC_MASK;        
}

bool DRV_ETH_MIIMIsBusy(DRV_ETHERNET_REGISTERS* pEthReg)
{        
    return (bool) (pEthReg->EMACxMIND.MIIMBUSY);
}

void DRV_ETH_PHYAddressSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t phyAddr )
{   
    pEthReg->EMACxMADR.PHYADDR = phyAddr;
}

void DRV_ETH_RegisterAddressSet(DRV_ETHERNET_REGISTERS* pEthReg, uint8_t regAddr)
{   
    pEthReg->EMACxMADR.REGADDR = regAddr;
}

void DRV_ETH_ClearDataValid(DRV_ETHERNET_REGISTERS* pEthReg)
{
    pEthReg->EMACxMINDSET = _EMACxMIND_NOTVALID_MASK;
}

void DRV_ETH_MIIMScanModeEnable(DRV_ETHERNET_REGISTERS* pEthReg)
{       
    pEthReg->EMACxMCMDSET = _EMACxMCMD_SCAN_MASK;
}

void DRV_ETH_MIIMScanModeDisable(DRV_ETHERNET_REGISTERS* pEthReg)
{   
    pEthReg->EMACxMCMDCLR = _EMACxMCMD_SCAN_MASK;
}

void DRV_ETH_MIIMWriteDataSet(DRV_ETHERNET_REGISTERS* pEthReg, uint16_t writeData )
{
    pEthReg->EMACxMWTD.MWTD =writeData ;                 
                     
    // there's 2 clock cycles till busy is set for a write op
#if defined(__PIC32MZ__)
    NOP;
#else                     
    __asm__ __volatile__ ("nop; nop;"); 
#endif
}

void DRV_ETH_MIIMWriteStart(DRV_ETHERNET_REGISTERS* pEthReg)
{   
    pEthReg->EMACxMCMDCLR=_EMACxMCMD_READ_MASK;
}

void DRV_ETH_MIIMReadStart(DRV_ETHERNET_REGISTERS* pEthReg)
{       
    pEthReg->EMACxMCMDSET=_EMACxMCMD_READ_MASK;

    // there's 4 clock cycles till busy is set for a read op
#if defined(__PIC32MZ__)
    NOP; 
#else
    __asm__ __volatile__ ("nop; nop; nop; nop;");
#endif
    
}

uint16_t DRV_ETH_MIIMReadDataGet(DRV_ETHERNET_REGISTERS* pEthReg)
{    	
    return (uint16_t)(pEthReg->EMACxMRDD.MRDD);                          
}

bool DRV_ETH_DataNotValid(DRV_ETHERNET_REGISTERS* pEthReg)
{       
    return (bool) (pEthReg->EMACxMIND.NOTVALID);
}

void DRV_ETH_MIIMResetEnable(DRV_ETHERNET_REGISTERS* pEthReg)
{       
    pEthReg->EMACxMCFGSET = _EMACxMCFG_RESETMGMT_MASK; 
}

void DRV_ETH_MIIMResetDisable(DRV_ETHERNET_REGISTERS* pEthReg)
{   
    pEthReg->EMACxMCFGCLR = _EMACxMCFG_RESETMGMT_MASK; 
}

void DRV_ETH_MIIMClockSet(DRV_ETHERNET_REGISTERS* pEthReg , uint8_t miimClock )
{
    pEthReg->EMACxMCFG.CLKSEL = miimClock ;
}

