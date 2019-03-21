/*******************************************************************************
  Ethernet PIC32 driver Template Implementation

  File Name:
    drv_eth_pic32_lib.h

  Summary:
   Ethernet PIC32 driver Template Implementation

  Description:
    All the Driver configuration Inline functions

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

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

#ifndef _DRV_ETHMAC_LIB_H
#define _DRV_ETHMAC_LIB_H
#include "eth_registers.h"

//******************************************************************************
/* Function :  ETH_AlignErrorCountClear_Default

  Summary:
    Implements Default variant of PLIB_ETH_AlignErrorCountClear
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_AlignErrorCountClear function.
*/

PLIB_TEMPLATE void ETH_AlignErrorCountClear_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	eth->ETHALGNERR.ALGNERRCNT =0x0000;
}


//******************************************************************************
/* Function :  ETH_AlignErrorCountGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_AlignErrorCountGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_AlignErrorCountGet function.
*/

PLIB_TEMPLATE uint16_t ETH_AlignErrorCountGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
    return (eth->ETHALGNERR.ALGNERRCNT);
}


//******************************************************************************
/* Function :  ETH_ExistsAlignmentErrorCount_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsAlignmentErrorCount

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsAlignmentErrorCount function.
*/

#define PLIB_ETH_ExistsAlignmentErrorCount PLIB_ETH_ExistsAlignmentErrorCount
PLIB_TEMPLATE bool ETH_ExistsAlignmentErrorCount_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_ALIGNMENTERRORCOUNT_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_AutoFlowControlEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_AutoFlowControlEnable
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_AutoFlowControlEnable function.
*/

PLIB_TEMPLATE void ETH_AutoFlowControlEnable_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
    eth->ETHCON1SET = _ETHCON1_AUTOFC_MASK;
}

//******************************************************************************
/* Function :  ETH_AutoFlowControlDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_AutoFlowControlDisable
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_AutoFlowControlDisable function.
*/

PLIB_TEMPLATE void ETH_AutoFlowControlDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	eth->ETHCON1.AUTOFC = 0;
}

//******************************************************************************
/* Function :  ETH_AutoFlowControlIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_AutoFlowControlIsEnabled
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_AutoFlowControlIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_AutoFlowControlIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
    return (bool)(eth->ETHCON1.AUTOFC);
}

//******************************************************************************
/* Function :  ETH_ExistsAutoFlowControl_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsAutoFlowControl
	

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsAutoFlowControl function.
*/

#define PLIB_ETH_ExistsAutoFlowControl PLIB_ETH_ExistsAutoFlowControl
PLIB_TEMPLATE bool ETH_ExistsAutoFlowControl_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_AUTOFLOWCONTROL_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_SingleCollisionCountClear_Default

  Summary:
    Implements Default variant of PLIB_ETH_SingleCollisionCountClear
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_SingleCollisionCountClear function.
*/

PLIB_TEMPLATE void ETH_SingleCollisionCountClear_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	eth->ETHSCOLFRM.SCOLFRMCNT = 0x0000;
}


//******************************************************************************
/* Function :  ETH_SingleCollisionCountGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_SingleCollisionCountGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_SingleCollisionCountGet function.
*/

PLIB_TEMPLATE uint16_t ETH_SingleCollisionCountGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
    return (uint16_t)(eth->ETHSCOLFRM.SCOLFRMCNT);
}


//******************************************************************************
/* Function :  ETH_MultipleCollisionCountClear_Default

  Summary:
    Implements Default variant of PLIB_ETH_MultipleCollisionCountClear
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_MultipleCollisionCountClear function.
*/

PLIB_TEMPLATE void ETH_MultipleCollisionCountClear_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	eth->ETHMCOLFRM.MCOLFRMCNT = 0x0000;
}


//******************************************************************************
/* Function :  ETH_MultipleCollisionCountGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_MultipleCollisionCountGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_MultipleCollisionCountGet function.
*/

PLIB_TEMPLATE uint16_t ETH_MultipleCollisionCountGet_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));
    return (uint16_t)(eth->ETHMCOLFRM.MCOLFRMCNT);
}


//******************************************************************************
/* Function :  ETH_ExistsCollisionCounts_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsCollisionCounts

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsCollisionCounts function.
*/

#define PLIB_ETH_ExistsCollisionCounts PLIB_ETH_ExistsCollisionCounts
PLIB_TEMPLATE bool ETH_ExistsCollisionCounts_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_COLLISIONCOUNTS_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_CollisionWindowGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_CollisionWindowGet
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_CollisionWindowGet function.
*/

PLIB_TEMPLATE uint8_t ETH_CollisionWindowGet_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));
    return (uint8_t)(eth->EMACxCLRT.CWINDOW);									
}


//******************************************************************************
/* Function :  ETH_CollisionWindowSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_CollisionWindowSet
	This function performs atomic register access. 
	
  Description:
    This template implements the Default variant of the PLIB_ETH_CollisionWindowSet function.
*/

PLIB_TEMPLATE void ETH_CollisionWindowSet_Default( ETH_MODULE_ID index , uint8_t collisionWindowValue )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	eth->EMACxCLRT.CWINDOW = collisionWindowValue;
}

//******************************************************************************
/* Function :  ETH_ExistsCollisionWindow_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsCollisionWindow

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsCollisionWindow function.
*/

#define PLIB_ETH_ExistsCollisionWindow PLIB_ETH_ExistsCollisionWindow
PLIB_TEMPLATE bool ETH_ExistsCollisionWindow_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_COLLISIONWINDOW_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_Enable_Default

  Summary:
    Implements Default variant of PLIB_ETH_Enable
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_Enable function.
*/

PLIB_TEMPLATE void ETH_Enable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));
    eth->ETHCON1SET = _ETHCON1_ON_MASK;	
}


//******************************************************************************
/* Function :  ETH_Disable_Default

  Summary:
    Implements Default variant of PLIB_ETH_Disable
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_Disable function.
*/

PLIB_TEMPLATE void ETH_Disable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	eth->ETHCON1.ON = 0;
}


//******************************************************************************
/* Function :  ETH_IsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_IsEnabled.
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_IsEnabled function.
*/

PLIB_TEMPLATE bool ETH_IsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));
    return (bool)(eth->ETHCON1.ON) ;	
}


//******************************************************************************
/* Function :  ETH_ExistsEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsEnable

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsEnable function.
*/

#define PLIB_ETH_ExistsEnable PLIB_ETH_ExistsEnable
PLIB_TEMPLATE bool ETH_ExistsEnable_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_ENABLE_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_RxPacketCountGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxPacketCountGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RxPacketCountGet function.
*/

PLIB_TEMPLATE uint8_t ETH_RxPacketCountGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
    return (uint8_t)(eth->ETHSTAT.BUFCNT); 
}


//******************************************************************************
/* Function :  ETH_EthernetIsBusy_Default

  Summary:
    Implements Default variant of PLIB_ETH_EthernetIsBusy
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_EthernetIsBusy function.
*/

PLIB_TEMPLATE bool ETH_EthernetIsBusy_Default( ETH_MODULE_ID index )
{
 	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
    return (bool)(eth->ETHSTAT.BUSY); 						   
}


//******************************************************************************
/* Function :  ETH_TransmitIsBusy_Default

  Summary:
    Implements Default variant of PLIB_ETH_TransmitIsBusy
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_TransmitIsBusy function.
*/

PLIB_TEMPLATE bool ETH_TransmitIsBusy_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));
    return (bool)(eth->ETHSTAT.TXBUSY); 
}


//******************************************************************************
/* Function :  ETH_ReceiveIsBusy_Default

  Summary:
    Implements Default variant of PLIB_ETH_ReceiveIsBusy
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_ReceiveIsBusy function.
*/

PLIB_TEMPLATE bool ETH_ReceiveIsBusy_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));
    return (bool)(eth->ETHSTAT.RXBUSY); 
}


//******************************************************************************
/* Function :  ETH_ExistsEthernetControllerStatus_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsEthernetControllerStatus

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsEthernetControllerStatus function.
*/

#define PLIB_ETH_ExistsEthernetControllerStatus PLIB_ETH_ExistsEthernetControllerStatus
PLIB_TEMPLATE bool ETH_ExistsEthernetControllerStatus_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_ETHERNETCONTROLLERSTATUS_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_FCSErrorCountClear_Default

  Summary:
    Implements Default variant of PLIB_ETH_FCSErrorCountClear
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_FCSErrorCountClear function.
*/

PLIB_TEMPLATE void ETH_FCSErrorCountClear_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	eth->ETHFCSERR.FCSERRCNT = 0x0000;
}


//******************************************************************************
/* Function :  ETH_FCSErrorCountGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_FCSErrorCountGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_FCSErrorCountGet function.
*/

PLIB_TEMPLATE uint16_t ETH_FCSErrorCountGet_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));
    return (uint16_t)eth->ETHFCSERR.FCSERRCNT;
}


//******************************************************************************
/* Function :  ETH_ExistsFCSErrorCount_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsFCSErrorCount

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsFCSErrorCount function.
*/

#define PLIB_ETH_ExistsFCSErrorCount PLIB_ETH_ExistsFCSErrorCount
PLIB_TEMPLATE bool ETH_ExistsFCSErrorCount_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_FCSERRORCOUNT_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_FramesTxdOkCountClear_Default

  Summary:
    Implements Default variant of PLIB_ETH_FramesTxdOkCountClear
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_FramesTxdOkCountClear function.
*/

PLIB_TEMPLATE void ETH_FramesTxdOkCountClear_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));

	eth->ETHFRMTXOK.FRMTXOKCNT = 0x0000;
}


//******************************************************************************
/* Function :  ETH_FramesTxdOkCountGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_FramesTxdOkCountGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_FramesTxdOkCountGet function.
*/

PLIB_TEMPLATE uint16_t ETH_FramesTxdOkCountGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	return (uint16_t)eth->ETHFRMTXOK.FRMTXOKCNT;
}


//******************************************************************************
/* Function :  ETH_ExistsFramesTransmittedOK_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsFramesTransmittedOK

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsFramesTransmittedOK function.
*/

#define PLIB_ETH_ExistsFramesTransmittedOK PLIB_ETH_ExistsFramesTransmittedOK
PLIB_TEMPLATE bool ETH_ExistsFramesTransmittedOK_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_FRAMESTRANSMITTEDOK_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_FramesRxdOkCountClear_Default

  Summary:
    Implements Default variant of PLIB_ETH_FramesRxdOkCountClear
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_FramesRxdOkCountClear function.
*/

PLIB_TEMPLATE void ETH_FramesRxdOkCountClear_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));
 
	eth->ETHFRMRXOK.FRMRXOKCNT =0x0000;
}


//******************************************************************************
/* Function :  ETH_FramesRxdOkCountGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_FramesRxdOkCountGet
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_FramesRxdOkCountGet function.
*/

PLIB_TEMPLATE uint16_t ETH_FramesRxdOkCountGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
    return (uint16_t)eth->ETHFRMRXOK.FRMRXOKCNT;
}


//******************************************************************************
/* Function :  ETH_ExistsFramexReceivedOK_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsFramexReceivedOK

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsFramexReceivedOK function.
*/

#define PLIB_ETH_ExistsFramexReceivedOK PLIB_ETH_ExistsFramexReceivedOK
PLIB_TEMPLATE bool ETH_ExistsFramexReceivedOK_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_FRAMEXRECEIVEDOK_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_HashTableSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_HashTableSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_HashTableSet function.
*/

PLIB_TEMPLATE void ETH_HashTableSet_Default( ETH_MODULE_ID index , uint64_t hashTableValue )
{
    
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	PLIB_ASSERT(((!eth->ETHCON1.RXEN)|| (!eth->ETHRXFC,HTEN)),
                "Receive must be disabled or Hash Table turned off!");
		
	eth->ETHHT0SET = _ETHHT0_HTLOWER_MASK & ((uint32_t)hashTableValue);
	eth->ETHHT1SET = _ETHHT1_HTUPPER_MASK &((uint32_t)(hashTableValue>>32));
	
}

//******************************************************************************
/* Function :  ETH_RxFiltersHTSet_Default

  Summary:
    Implements Default variant of ETH_RxFiltersHTSet

  Description:
    This template implements the Default variant of the ETH_RxFiltersHTSet function.
*/
PLIB_TEMPLATE void ETH_RxFiltersHTSet_Default(ETH_MODULE_ID index,uint64_t htable)
{
    ETH_HashTableSet_Default(index,htable);
}



//******************************************************************************
/* Function :  ETH_HashTableGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_HashTableGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_HashTableGet function.
*/

PLIB_TEMPLATE uint32_t ETH_HashTableGet_Default( ETH_MODULE_ID index )
{
   	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	 return ( ((uint64_t)eth->ETHHT1.HTUPPER<<32)
            + ((uint64_t)eth->ETHHT0.HTLOWER));		
}


//******************************************************************************
/* Function :  ETH_ExistsHashTable_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsHashTable

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsHashTable function.
*/

#define PLIB_ETH_ExistsHashTable PLIB_ETH_ExistsHashTable
PLIB_TEMPLATE bool ETH_ExistsHashTable_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_HASHTABLE_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_BackToBackIPGGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_BackToBackIPGGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_BackToBackIPGGet function.
*/

PLIB_TEMPLATE uint8_t ETH_BackToBackIPGGet_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (uint8_t) eth->EMACxIPGT.B2BIPKTGP;
}


//******************************************************************************
/* Function :  ETH_BackToBackIPGSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_BackToBackIPGSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_BackToBackIPGSet function.
*/

PLIB_TEMPLATE void ETH_BackToBackIPGSet_Default( ETH_MODULE_ID index , uint8_t backToBackIPGValue )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	
	eth->EMACxIPGTCLR = _EMAC1IPGT_B2BIPKTGP_MASK;
	eth->EMACxIPGTSET = _EMAC1IPGT_B2BIPKTGP_MASK & ((uint32_t)backToBackIPGValue << _EMAC1IPGT_B2BIPKTGP_POSITION);	 
					 
}


//******************************************************************************
/* Function :  ETH_NonBackToBackIPG1Get_Default

  Summary:
    Implements Default variant of PLIB_ETH_NonBackToBackIPG1Get
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_NonBackToBackIPG1Get function.
*/

PLIB_TEMPLATE uint8_t ETH_NonBackToBackIPG1Get_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (uint8_t) eth->EMACxIPGR.NB2BIPKTGP1;
}


//******************************************************************************
/* Function :  ETH_NonBackToBackIPG1Set_Default

  Summary:
    Implements Default variant of PLIB_ETH_NonBackToBackIPG1Set
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_NonBackToBackIPG1Set function.
*/

PLIB_TEMPLATE void ETH_NonBackToBackIPG1Set_Default( ETH_MODULE_ID index , uint8_t nonBackToBackIPGValue )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	eth->EMACxIPGRCLR = _EMACxIPGR_NB2BIPKTGP1_MASK;
	eth->EMACxIPGRSET = _EMACxIPGR_NB2BIPKTGP1_MASK & ((uint32_t)nonBackToBackIPGValue << _EMACxIPGR_NB2BIPKTGP1_POSITION);	
}


//******************************************************************************
/* Function :  ETH_NonBackToBackIPG2Get_Default

  Summary:
    Implements Default variant of PLIB_ETH_NonBackToBackIPG2Get
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_NonBackToBackIPG2Get function.
*/

PLIB_TEMPLATE uint8_t ETH_NonBackToBackIPG2Get_Default( ETH_MODULE_ID index )
{	
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (uint8_t) eth->EMACxIPGR.NB2BIPKTGP2;
}


//******************************************************************************
/* Function :  ETH_NonBackToBackIPG2Set_Default

  Summary:
    Implements Default variant of PLIB_ETH_NonBackToBackIPG2Set
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_NonBackToBackIPG2Set function.
*/

PLIB_TEMPLATE void ETH_NonBackToBackIPG2Set_Default( ETH_MODULE_ID index , uint8_t nonBackToBackIPGValue )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxIPGRCLR = _EMACxIPGR_NB2BIPKTGP2_MASK;
	eth->EMACxIPGRSET = _EMACxIPGR_NB2BIPKTGP2_MASK & ((uint32_t)nonBackToBackIPGValue <<_EMACxIPGR_NB2BIPKTGP2_POSITION);	
}


//******************************************************************************
/* Function :  ETH_ExistsInterPacketGaps_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsInterPacketGaps

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsInterPacketGaps function.
*/

#define PLIB_ETH_ExistsInterPacketGaps PLIB_ETH_ExistsInterPacketGaps
PLIB_TEMPLATE bool ETH_ExistsInterPacketGaps_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_INTERPACKETGAPS_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_InterruptSourceEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_InterruptSourceEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_InterruptSourceEnable function.
*/

PLIB_TEMPLATE void ETH_InterruptSourceEnable_Default( ETH_MODULE_ID index , ETH_INTERRUPT_SOURCES intmask )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	eth->ETHIENSET = _ETHIEN_w_MASK & ((uint32_t)intmask <<_ETHIEN_w_POSITION);
}
//******************************************************************************
/* Function :  ETH_EventsEnableSet_Default

  Summary:
    Implements Default variant of ETH_EventsEnableSet
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the ETH_EventsEnableSet function.
*/
PLIB_TEMPLATE void ETH_EventsEnableSet_Default(ETH_MODULE_ID index,PLIB_ETH_EVENTS eEvents)
{
    ETH_InterruptSourceEnable_Default(index,eEvents);
}

//******************************************************************************
/* Function :  ETH_InterruptSourceDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_InterruptSourceDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_InterruptSourceDisable function.
*/

PLIB_TEMPLATE void ETH_InterruptSourceDisable_Default( ETH_MODULE_ID index , ETH_INTERRUPT_SOURCES intmask )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	eth->ETHIENCLR = _ETHIEN_w_MASK & ((uint32_t)intmask <<_ETHIEN_w_POSITION);
}

//******************************************************************************
/* Function :  ETH_EventsEnableClr_Default

  Summary:
    Implements Default variant of ETH_EventsEnableClr

  Description:
    This template implements the Default variant of the ETH_EventsEnableClr function.

*/

PLIB_TEMPLATE void ETH_EventsEnableClr_Default(ETH_MODULE_ID index,PLIB_ETH_EVENTS eEvents)
{
   ETH_InterruptSourceDisable_Default(index,eEvents);
}

//******************************************************************************
/* Function :  ETH_EventsEnableWrite_Default

  Summary:
    Implements Default variant of ETH_EventsEnableWrite

  Description:
    This template implements the Default variant of the ETH_EventsEnableWrite function.
*/
PLIB_TEMPLATE void ETH_EventsEnableWrite_Default(ETH_MODULE_ID index,PLIB_ETH_EVENTS eEvents)
{
    ETH_InterruptSourceEnable_Default(index,eEvents);
    ETH_InterruptSourceDisable_Default(index,~eEvents);
}

//******************************************************************************
/* Function :  ETH_InterruptSourceIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_InterruptSourceIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_InterruptSourceIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_InterruptSourceIsEnabled_Default( ETH_MODULE_ID index , ETH_INTERRUPT_SOURCES intmask )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	return ((eth->ETHIEN.w & (uint32_t)intmask ) > 0 ? true : false);	
}


//******************************************************************************
/* Function :  ETH_InterruptSourcesGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_InterruptSourcesGet
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_InterruptSourcesGet function.
*/

PLIB_TEMPLATE ETH_INTERRUPT_SOURCES ETH_InterruptSourcesGet_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	return (ETH_INTERRUPT_SOURCES)eth->ETHIEN.w;
}


//******************************************************************************
/* Function :  ETH_EventsEnableGet_Default

  Summary:
    Implements Default variant of ETH_EventsEnableGet

  Description:
    This template implements the Default variant of the ETH_EventsEnableGet function.
*/
PLIB_TEMPLATE PLIB_ETH_EVENTS ETH_EventsEnableGet_Default(ETH_MODULE_ID index)
{
    return ETH_InterruptSourcesGet_Default(index);
}


//******************************************************************************
/* Function :  ETH_InterruptSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_InterruptSet
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_InterruptSet function.
*/

PLIB_TEMPLATE void ETH_InterruptSet_Default( ETH_MODULE_ID index , ETH_INTERRUPT_SOURCES intmask )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	eth->ETHIRQSET = _ETHIRQ_w_MASK & ((uint32_t)intmask << _ETHIRQ_w_POSITION);	
}


//******************************************************************************
/* Function :  ETH_InterruptClear_Default

  Summary:
    Implements Default variant of PLIB_ETH_InterruptClear
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_InterruptClear function.
*/

PLIB_TEMPLATE void ETH_InterruptClear_Default( ETH_MODULE_ID index , ETH_INTERRUPT_SOURCES intmask )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	eth->ETHIRQCLR = _ETHIRQ_w_MASK & ((uint32_t)intmask << _ETHIRQ_w_POSITION);
}

//******************************************************************************
/* Function :  ETH_EventsClr_Default

  Summary:
    Implements Default variant of ETH_EventsClr

  Description:
    This template implements the Default variant of the ETH_EventsClr function.
*/
PLIB_TEMPLATE void ETH_EventsClr_Default(ETH_MODULE_ID index,PLIB_ETH_EVENTS eEvents)
{
    ETH_InterruptClear_Default(index,eEvents);
}

//******************************************************************************
/* Function :  ETH_InterruptsGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_InterruptsGet.
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_InterruptsGet function.
*/

PLIB_TEMPLATE ETH_INTERRUPT_SOURCES ETH_InterruptsGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	return  (ETH_INTERRUPT_SOURCES) eth->ETHIRQ.w;
}

//******************************************************************************
/* Function :  ETH_EventsGet_Default

  Summary:
    Implements Default variant of ETH_EventsGet

  Description:
    This template implements the Default variant of the ETH_EventsGet function.
*/
PLIB_TEMPLATE PLIB_ETH_EVENTS ETH_EventsGet_Default(ETH_MODULE_ID index)
{
    return ETH_InterruptsGet_Default(index);
}

//******************************************************************************
/* Function :  ETH_InterruptStatusGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_InterruptStatusGet
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_InterruptStatusGet function.
*/

PLIB_TEMPLATE bool ETH_InterruptStatusGet_Default( ETH_MODULE_ID index , ETH_INTERRUPT_SOURCES intmask )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	return ((eth->ETHIRQ.w & ((uint32_t) intmask )) > 0 ? true : false);	
}


//******************************************************************************
/* Function :  ETH_ExistsInterrupt_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsInterrupt

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsInterrupt function.
*/

#define PLIB_ETH_ExistsInterrupt PLIB_ETH_ExistsInterrupt
PLIB_TEMPLATE bool ETH_ExistsInterrupt_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_INTERRUPT_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_LoopbackEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_LoopbackEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_LoopbackEnable function.
*/

PLIB_TEMPLATE void ETH_LoopbackEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	eth->EMACxCFG1SET = _EMACxCFG1_LOOPBACK_MASK;
}


//******************************************************************************
/* Function :  ETH_LoopbackDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_LoopbackDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_LoopbackDisable function.
*/

PLIB_TEMPLATE void ETH_LoopbackDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1.LOOPBACK=0;
}


//******************************************************************************
/* Function :  ETH_LoopbackIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_LoopbackIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_LoopbackIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_LoopbackIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) (eth->EMACxCFG1.LOOPBACK);
}


//******************************************************************************
/* Function :  ETH_TxPauseEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_TxPauseEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_TxPauseEnable function.
*/

PLIB_TEMPLATE void ETH_TxPauseEnable_Default( ETH_MODULE_ID index )
{
 	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1SET = _EMACxCFG1_TXPAUSE_MASK;
}


//******************************************************************************
/* Function :  ETH_TxPauseDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_TxPauseDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_TxPauseDisable function.
*/

PLIB_TEMPLATE void ETH_TxPauseDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1.TXPAUSE=0;
}


//******************************************************************************
/* Function :  ETH_TxPauseIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_TxPauseIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_TxPauseIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_TxPauseIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxCFG1.TXPAUSE);    
}


//******************************************************************************
/* Function :  ETH_RxPauseEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxPauseEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_RxPauseEnable function.
*/

PLIB_TEMPLATE void ETH_RxPauseEnable_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1SET = _EMACxCFG1_RXPAUSE_MASK;
}


//******************************************************************************
/* Function :  ETH_RxPauseDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxPauseDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_RxPauseDisable function.
*/

PLIB_TEMPLATE void ETH_RxPauseDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1.RXPAUSE=0;
}


//******************************************************************************
/* Function :  ETH_RxPauseIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxPauseIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_RxPauseIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_RxPauseIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxCFG1.RXPAUSE);
}


//******************************************************************************
/* Function :  ETH_PassAllEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_PassAllEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_PassAllEnable function.
*/

PLIB_TEMPLATE void ETH_PassAllEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1SET = _EMACxCFG1_PASSALL_MASK;	
}


//******************************************************************************
/* Function :  ETH_PassAllDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_PassAllDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_PassAllDisable function.
*/

PLIB_TEMPLATE void ETH_PassAllDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1.PASSALL=0;	
}


//******************************************************************************
/* Function :  ETH_PassAllIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_PassAllIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_PassAllIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_PassAllIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) eth->EMACxCFG1.PASSALL;
}


//******************************************************************************
/* Function :  ETH_ReceiveEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_ReceiveEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_ReceiveEnable function.
*/

PLIB_TEMPLATE void ETH_ReceiveEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1SET = _EMACxCFG1_RXENABLE_MASK;	
}


//******************************************************************************
/* Function :  ETH_ReceiveDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_ReceiveDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_ReceiveDisable function.
*/

PLIB_TEMPLATE void ETH_ReceiveDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1.RXENABLE = 0;		
}


//******************************************************************************
/* Function :  ETH_ReceiveIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_ReceiveIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_ReceiveIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_ReceiveIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) eth->EMACxCFG1.RXENABLE;    
}


//******************************************************************************
/* Function :  ETH_ExcessDeferEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExcessDeferEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_ExcessDeferEnable function.
*/

PLIB_TEMPLATE void ETH_ExcessDeferEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2SET = _EMACxCFG2_EXCESSDFR_MASK;	
}


//******************************************************************************
/* Function :  ETH_ExcessDeferDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExcessDeferDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_ExcessDeferDisable function.
*/

PLIB_TEMPLATE void ETH_ExcessDeferDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2.EXCESSDFR=0;	
}


//******************************************************************************
/* Function :  ETH_ExcessDeferIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExcessDeferIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_ExcessDeferIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_ExcessDeferIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) eth->EMACxCFG2.EXCESSDFR;
}


//******************************************************************************
/* Function :  ETH_BackPresNoBackoffEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_BackPresNoBackoffEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_BackPresNoBackoffEnable function.
*/

PLIB_TEMPLATE void ETH_BackPresNoBackoffEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2SET = _EMACxCFG2_BPNOBKOFF_MASK;	
}


//******************************************************************************
/* Function :  ETH_BackPresNoBackoffDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_BackPresNoBackoffDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_BackPresNoBackoffDisable function.
*/

PLIB_TEMPLATE void ETH_BackPresNoBackoffDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2.BPNOBKOFF=0;
}


//******************************************************************************
/* Function :  ETH_BackPresNoBackoffIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_BackPresNoBackoffIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_BackPresNoBackoffIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_BackPresNoBackoffIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) eth->EMACxCFG2.BPNOBKOFF;
}


//******************************************************************************
/* Function :  ETH_NoBackoffEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_NoBackoffEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_NoBackoffEnable function.
*/

PLIB_TEMPLATE void ETH_NoBackoffEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2SET=_EMACxCFG2_NOBKOFF_MASK;	
}


//******************************************************************************
/* Function :  ETH_NoBackoffDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_NoBackoffDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_NoBackoffDisable function.
*/

PLIB_TEMPLATE void ETH_NoBackoffDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2.NOBKOFF=0;
}


//******************************************************************************
/* Function :  ETH_NoBackoffIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_NoBackoffIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_NoBackoffIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_NoBackoffIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxCFG2.NOBKOFF);
}


//******************************************************************************
/* Function :  ETH_LongPreambleEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_LongPreambleEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_LongPreambleEnable function.
*/

PLIB_TEMPLATE void ETH_LongPreambleEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2SET=_EMACxCFG2_LONGPRE_MASK;
}


//******************************************************************************
/* Function :  ETH_LongPreambleDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_LongPreambleDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_LongPreambleDisable function.
*/

PLIB_TEMPLATE void ETH_LongPreambleDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2.LONGPRE = 0;
}


//******************************************************************************
/* Function :  ETH_LongPreambleIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_LongPreambleIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_LongPreambleIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_LongPreambleIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxCFG2.LONGPRE);
}


//******************************************************************************
/* Function :  ETH_PurePreambleEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_PurePreambleEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_PurePreambleEnable function.
*/

PLIB_TEMPLATE void ETH_PurePreambleEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2SET=_EMACxCFG2_PUREPRE_MASK;
}


//******************************************************************************
/* Function :  ETH_PurePreambleDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_PurePreambleDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_PurePreambleDisable function.
*/

PLIB_TEMPLATE void ETH_PurePreambleDisable_Default( ETH_MODULE_ID index )
{
   volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
   eth->EMACxCFG2.PUREPRE=0;
}


//******************************************************************************
/* Function :  ETH_PurePreambleIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_PurePreambleIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_PurePreambleIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_PurePreambleIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) eth->EMACxCFG2.PUREPRE;
}


//******************************************************************************
/* Function :  ETH_AutoDetectPadGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_AutoDetectPadGet
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_AutoDetectPadGet function.
*/

PLIB_TEMPLATE ETH_AUTOPAD_OPTION ETH_AutoDetectPadGet_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));							   
	return (ETH_AUTOPAD_OPTION) ((eth->EMACxCFG2.w &	
	(_EMACxCFG2_AUTOPAD_MASK | _EMACxCFG2_VLANPAD_MASK | 
	_EMACxCFG2_PADENABLE_MASK)) >> _EMACxCFG2_PADENABLE_POSITION );			
}

//******************************************************************************
/* Function :  ETH_AutoDetectPadSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_AutoDetectPadSet
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_AutoDetectPadSet function.
*/

PLIB_TEMPLATE void ETH_AutoDetectPadSet_Default( ETH_MODULE_ID index , ETH_AUTOPAD_OPTION option )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));							   
	eth->EMACxCFG2SET = ((_EMACxCFG2_AUTOPAD_MASK | _EMACxCFG2_VLANPAD_MASK 
						| _EMACxCFG2_PADENABLE_MASK) & 
						((uint32_t) option << _EMACxCFG2_PADENABLE_POSITION));						
					
}


//******************************************************************************
/* Function :  ETH_AutoDetectPadClear_Default

  Summary:
    Implements Default variant of PLIB_ETH_AutoDetectPadClear
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_AutoDetectPadClear function.
*/

PLIB_TEMPLATE void ETH_AutoDetectPadClear_Default( ETH_MODULE_ID index , ETH_AUTOPAD_OPTION option )
{

	volatile eth_registers_t * eth = ((eth_registers_t *)(index));							   
	eth->EMACxCFG2.AUTOPAD=0;
	eth->EMACxCFG2.VLANPAD=0;
	eth->EMACxCFG2.PADENABLE=0;
}


//******************************************************************************
/* Function :  ETH_CRCEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_CRCEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_CRCEnable function.
*/

PLIB_TEMPLATE void ETH_CRCEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2SET=_EMACxCFG2_CRCENABLE_MASK;
}


//******************************************************************************
/* Function :  ETH_CRCDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_CRCDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_CRCDisable function.
*/

PLIB_TEMPLATE void ETH_CRCDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2.CRCENABLE=0;
}


//******************************************************************************
/* Function :  ETH_CRCIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_CRCIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_CRCIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_CRCIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxCFG2.CRCENABLE);
}


//******************************************************************************
/* Function :  ETH_DelayedCRCEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_DelayedCRCEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_DelayedCRCEnable function.
*/

PLIB_TEMPLATE void ETH_DelayedCRCEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2SET=_EMACxCFG2_DELAYCRC_MASK;
}


//******************************************************************************
/* Function :  ETH_DelayedCRCDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_DelayedCRCDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_DelayedCRCDisable function.
*/

PLIB_TEMPLATE void ETH_DelayedCRCDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2.DELAYCRC= 0;
}


//******************************************************************************
/* Function :  ETH_DelayedCRCIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_DelayedCRCIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_DelayedCRCIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_DelayedCRCIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxCFG2.DELAYCRC);
}


//******************************************************************************
/* Function :  ETH_HugeFrameEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_HugeFrameEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_HugeFrameEnable function.
*/

PLIB_TEMPLATE void ETH_HugeFrameEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2SET=_EMACxCFG2_HUGEFRM_MASK;
}


//******************************************************************************
/* Function :  ETH_HugeFrameDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_HugeFrameDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_HugeFrameDisable function.
*/

PLIB_TEMPLATE void ETH_HugeFrameDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2.HUGEFRM = 0;
}


//******************************************************************************
/* Function :  ETH_HugeFrameIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_HugeFrameIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_HugeFrameIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_HugeFrameIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxCFG2.HUGEFRM);
}


//******************************************************************************
/* Function :  ETH_FrameLengthCheckEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_FrameLengthCheckEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_FrameLengthCheckEnable function.
*/

PLIB_TEMPLATE void ETH_FrameLengthCheckEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2SET=_EMACxCFG2_LENGTHCK_MASK;
}


//******************************************************************************
/* Function :  ETH_FrameLengthCheckDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_FrameLengthCheckDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_FrameLengthCheckDisable function.
*/

PLIB_TEMPLATE void ETH_FrameLengthCheckDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2.LENGTHCK=0;
}


//******************************************************************************
/* Function :  ETH_FrameLengthCheckIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_FrameLengthCheckIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_FrameLengthCheckIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_FrameLengthCheckIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) eth->EMACxCFG2.LENGTHCK;
}


//******************************************************************************
/* Function :  ETH_FullDuplexEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_FullDuplexEnable
	This function performs atomic register access. 
  Description:
    This template implements the Default variant of the PLIB_ETH_FullDuplexEnable function.
*/

PLIB_TEMPLATE void ETH_FullDuplexEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2SET=_EMACxCFG2_FULLDPLX_MASK;
}


//******************************************************************************
/* Function :  ETH_FullDuplexDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_FullDuplexDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_FullDuplexDisable function.
*/

PLIB_TEMPLATE void ETH_FullDuplexDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2.FULLDPLX=0;
}


//******************************************************************************
/* Function :  ETH_FullDuplexIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_FullDuplexIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_FullDuplexIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_FullDuplexIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxCFG2.FULLDPLX);    
}


//******************************************************************************
/* Function :  ETH_ExistsMAC_Configuration_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsMAC_Configuration

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsMAC_Configuration function.
*/

#define PLIB_ETH_ExistsMAC_Configuration PLIB_ETH_ExistsMAC_Configuration
PLIB_TEMPLATE bool ETH_ExistsMAC_Configuration_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_MAC_CONFIGURATION_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_MIIResetEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIResetEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIResetEnable function.
*/

PLIB_TEMPLATE void ETH_MIIResetEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1SET=_EMACxCFG1_SOFTRESET_MASK;
}


//******************************************************************************
/* Function :  ETH_MIIResetDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIResetDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIResetDisable function.
*/

PLIB_TEMPLATE void ETH_MIIResetDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1.SOFTRESET=0;
}


//******************************************************************************
/* Function :  ETH_MIIResetIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIResetIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIResetIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_MIIResetIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) eth->EMACxCFG1.SOFTRESET;
    
}


//******************************************************************************
/* Function :  ETH_SimResetEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_SimResetEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_SimResetEnable function.
*/

PLIB_TEMPLATE void ETH_SimResetEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1SET=_EMACxCFG1_SIMRESET_MASK;
}


//******************************************************************************
/* Function :  ETH_SimResetDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_SimResetDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_SimResetDisable function.
*/

PLIB_TEMPLATE void ETH_SimResetDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1.SIMRESET=0;
}


//******************************************************************************
/* Function :  ETH_SimResetIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_SimResetIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_SimResetIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_SimResetIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) (eth->EMACxCFG1.SIMRESET);
}


//******************************************************************************
/* Function :  ETH_MCSRxResetEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MCSRxResetEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MCSRxResetEnable function.
*/

PLIB_TEMPLATE void ETH_MCSRxResetEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1SET=_EMACxCFG1_RESETRMCS_MASK;
}


//******************************************************************************
/* Function :  ETH_MCSRxResetDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MCSRxResetDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MCSRxResetDisable function.
*/

PLIB_TEMPLATE void ETH_MCSRxResetDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1.RESETRMCS=0;
}


//******************************************************************************
/* Function :  ETH_MCSRxResetIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_MCSRxResetIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MCSRxResetIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_MCSRxResetIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) (eth->EMACxCFG1.RESETRMCS);	
}


//******************************************************************************
/* Function :  ETH_RxFuncResetEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxFuncResetEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_RxFuncResetEnable function.
*/

PLIB_TEMPLATE void ETH_RxFuncResetEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1SET =_EMACxCFG1_RESETRFUN_MASK;
}


//******************************************************************************
/* Function :  ETH_RxFuncResetDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxFuncResetDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_RxFuncResetDisable function.
*/

PLIB_TEMPLATE void ETH_RxFuncResetDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1.RESETRFUN = 0;

}


//******************************************************************************
/* Function :  ETH_RxFuncResetIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxFuncResetIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_RxFuncResetIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_RxFuncResetIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxCFG1.RESETRFUN);
}


//******************************************************************************
/* Function :  ETH_MCSTxResetEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MCSTxResetEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MCSTxResetEnable function.
*/

PLIB_TEMPLATE void ETH_MCSTxResetEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1SET =_EMACxCFG1_RESETTMCS_MASK;
}


//******************************************************************************
/* Function :  ETH_MCSTxResetDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MCSTxResetDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MCSTxResetDisable function.
*/

PLIB_TEMPLATE void ETH_MCSTxResetDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1.RESETTMCS = 0;
}


//******************************************************************************
/* Function :  ETH_MCSTxResetIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_MCSTxResetIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MCSTxResetIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_MCSTxResetIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) (eth->EMACxCFG1.RESETTMCS);   
}


//******************************************************************************
/* Function :  ETH_TxFuncResetEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_TxFuncResetEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_TxFuncResetEnable function.
*/

PLIB_TEMPLATE void ETH_TxFuncResetEnable_Default( ETH_MODULE_ID index )
{   	
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1SET =_EMACxCFG1_RESETTFUN_MASK;
}


//******************************************************************************
/* Function :  ETH_TxFuncResetDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_TxFuncResetDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_TxFuncResetDisable function.
*/

PLIB_TEMPLATE void ETH_TxFuncResetDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1.RESETTFUN = 0;
}


//******************************************************************************
/* Function :  ETH_TxFuncResetIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_TxFuncResetIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_TxFuncResetIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_TxFuncResetIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxCFG1.RESETTFUN);    
}


//******************************************************************************
/* Function :  ETH_ExistsMAC_Resets_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsMAC_Resets
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsMAC_Resets function.
*/

#define PLIB_ETH_ExistsMAC_Resets PLIB_ETH_ExistsMAC_Resets
PLIB_TEMPLATE bool ETH_ExistsMAC_Resets_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_MAC_RESETS_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_TestBackPressEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_TestBackPressEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_TestBackPressEnable function.
*/

PLIB_TEMPLATE void ETH_TestBackPressEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxTESTSET=_EMACxTEST_TESTBP_MASK;
}


//******************************************************************************
/* Function :  ETH_TestBackPressDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_TestBackPressDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_TestBackPressDisable function.
*/

PLIB_TEMPLATE void ETH_TestBackPressDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxTESTCLR=_EMACxTEST_TESTBP_MASK;
}


//******************************************************************************
/* Function :  ETH_TestBackPressIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_TestBackPressIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_TestBackPressIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_TestBackPressIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxTEST.TESTBP);    
}


//******************************************************************************
/* Function :  ETH_TestPauseEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_TestPauseEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_TestPauseEnable function.
*/

PLIB_TEMPLATE void ETH_TestPauseEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxTESTSET=_EMACxTEST_TESTPAUSE_MASK;
}


//******************************************************************************
/* Function :  ETH_TestPauseDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_TestPauseDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_TestPauseDisable function.
*/

PLIB_TEMPLATE void ETH_TestPauseDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxTESTCLR=_EMACxTEST_TESTPAUSE_MASK;
}


//******************************************************************************
/* Function :  ETH_TestPauseIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_TestPauseIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_TestPauseIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_TestPauseIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxTEST.TESTPAUSE);    
}


//******************************************************************************
/* Function :  ETH_ShortcutQuantaEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_ShortcutQuantaEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_ShortcutQuantaEnable function.
*/

PLIB_TEMPLATE void ETH_ShortcutQuantaEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxTESTSET=_EMACxTEST_SHRTQNTA_MASK;
}


//******************************************************************************
/* Function :  ETH_ShortcutQuantaDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_ShortcutQuantaDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_ShortcutQuantaDisable function.
*/

PLIB_TEMPLATE void ETH_ShortcutQuantaDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxTESTCLR=_EMACxTEST_SHRTQNTA_MASK;
}


//******************************************************************************
/* Function :  ETH_ShortcutQuantaIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_ShortcutQuantaIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_ShortcutQuantaIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_ShortcutQuantaIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxTEST.SHRTQNTA);
}


//******************************************************************************
/* Function :  ETH_ExistsMAC_Testing_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsMAC_Testing

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsMAC_Testing function.
*/

#define PLIB_ETH_ExistsMAC_Testing PLIB_ETH_ExistsMAC_Testing
PLIB_TEMPLATE bool ETH_ExistsMAC_Testing_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_MAC_TESTING_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_ManualFlowControlEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_ManualFlowControlEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_ManualFlowControlEnable function.
*/

PLIB_TEMPLATE void ETH_ManualFlowControlEnable_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->ETHCON1SET =_ETHCON1_MANFC_MASK;
}


//******************************************************************************
/* Function :  ETH_ManualFlowControlDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_ManualFlowControlDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_ManualFlowControlDisable function.
*/

PLIB_TEMPLATE void ETH_ManualFlowControlDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->ETHCON1CLR =_ETHCON1_MANFC_MASK;
}


//******************************************************************************
/* Function :  ETH_ManualFlowControlIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_ManualFlowControlIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_ManualFlowControlIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_ManualFlowControlIsEnabled_Default( ETH_MODULE_ID index )
{  
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->ETHCON1.MANFC);
}


//******************************************************************************
/* Function :  ETH_ExistsManualFlowControl_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsManualFlowControl

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsManualFlowControl function.
*/

#define PLIB_ETH_ExistsManualFlowControl PLIB_ETH_ExistsManualFlowControl
PLIB_TEMPLATE bool ETH_ExistsManualFlowControl_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_MANUALFLOWCONTROL_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_MaxFrameLengthGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_MaxFrameLengthGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_MaxFrameLengthGet function.
*/

PLIB_TEMPLATE uint16_t ETH_MaxFrameLengthGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (uint16_t) eth->EMACxMAXF.MACMAXF;
}


//******************************************************************************
/* Function :  ETH_MaxFrameLengthSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_MaxFrameLengthSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_MaxFrameLengthSet function.
*/

PLIB_TEMPLATE void ETH_MaxFrameLengthSet_Default( ETH_MODULE_ID index , uint16_t MaxFrameLength )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	eth->EMACxMAXF.MACMAXF = (uint16_t) (_EMACxMAXF_MACMAXF_MASK & (MaxFrameLength << _EMACxMAXF_MACMAXF_POSITION));
}


PLIB_TEMPLATE void ETH_MACSetMaxFrame_Default(ETH_MODULE_ID index,uint16_t maxFrmSz)
{
    ETH_MaxFrameLengthSet_Default(index,maxFrmSz);
}

//******************************************************************************
/* Function :  ETH_ExistsMaxFrameLength_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsMaxFrameLength

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsMaxFrameLength function.
*/

#define PLIB_ETH_ExistsMaxFrameLength PLIB_ETH_ExistsMaxFrameLength
PLIB_TEMPLATE bool ETH_ExistsMaxFrameLength_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_MAXFRAMELENGTH_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_MIIMResetEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMResetEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMResetEnable function.
*/

PLIB_TEMPLATE void ETH_MIIMResetEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxMCFGSET = _EMACxMCFG_RESETMGMT_MASK; 
}


//******************************************************************************
/* Function :  ETH_MIIMResetDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMResetDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMResetDisable function.
*/

PLIB_TEMPLATE void ETH_MIIMResetDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxMCFGCLR = _EMACxMCFG_RESETMGMT_MASK; 
}


//******************************************************************************
/* Function :  ETH_MIIMResetIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMResetIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMResetIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_MIIMResetIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxMCFG.RESETMGMT);    
}


//******************************************************************************
/* Function :  ETH_MIIMClockGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMClockGet
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMClockGet function.
*/

PLIB_TEMPLATE ETH_MIIM_CLK ETH_MIIMClockGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (ETH_MIIM_CLK)(eth->EMACxMCFG.CLKSEL); 
}


//******************************************************************************
/* Function :  ETH_MIIMClockSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMClockSet
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMClockSet function.
*/

PLIB_TEMPLATE void ETH_MIIMClockSet_Default( ETH_MODULE_ID index , ETH_MIIM_CLK MIIMClock )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	eth->EMACxMCFG.CLKSEL = MIIMClock ;
}


//******************************************************************************
/* Function :  ETH_MIIMNoPreEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMNoPreEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMNoPreEnable function.
*/

PLIB_TEMPLATE void ETH_MIIMNoPreEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxMCFGSET = _EMACxMCFG_NOPRE_MASK;	
}

//******************************************************************************
/* Function :  ETH_MIIMNoPreDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMNoPreDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMNoPreDisable function.
*/

PLIB_TEMPLATE void ETH_MIIMNoPreDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxMCFG.NOPRE=0;	
}


//******************************************************************************
/* Function :  ETH_MIIMNoPreIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMNoPreIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMNoPreIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_MIIMNoPreIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxMCFG.NOPRE);
}


//******************************************************************************
/* Function :  ETH_MIIMScanIncrEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMScanIncrEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMScanIncrEnable function.
*/

PLIB_TEMPLATE void ETH_MIIMScanIncrEnable_Default( ETH_MODULE_ID index )
{    
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxMCFGSET = _EMACxMCFG_SCANINC_MASK;		
}


//******************************************************************************
/* Function :  ETH_MIIMScanIncrDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMScanIncrDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMScanIncrDisable function.
*/

PLIB_TEMPLATE void ETH_MIIMScanIncrDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxMCFG.SCANINC=0;	
}


//******************************************************************************
/* Function :  ETH_MIIMScanIncrIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMScanIncrIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMScanIncrIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_MIIMScanIncrIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxMCFG.SCANINC);	
}


//******************************************************************************
/* Function :  ETH_ExistsMIIM_Config_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsMIIM_Config

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsMIIM_Config function.
*/

#define PLIB_ETH_ExistsMIIM_Config PLIB_ETH_ExistsMIIM_Config
PLIB_TEMPLATE bool ETH_ExistsMIIM_Config_Default( ETH_MODULE_ID index )
{
    return true;
}

#endif /*_ETH_MIIM_CONFIG_DEFAULT_H*/


//******************************************************************************
/* Function :  ETH_LinkHasFailed_Default

  Summary:
    Implements Default variant of PLIB_ETH_LinkHasFailed
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_LinkHasFailed function.
*/

PLIB_TEMPLATE bool ETH_LinkHasFailed_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) (eth->EMACxMIND.LINKFAIL);
}

//******************************************************************************
/* Function :  ETH_ClearDataNotValid_Default

  Summary:
    Implements Default variant of PLIB_ETH_ClearDataNotValid
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_ClearDataNotValid function.
*/
PLIB_TEMPLATE void ETH_ClearDataNotValid_Default( ETH_MODULE_ID index )
{ 
	volatile eth_registers_t * eth = ((eth_registers_t *)(index)); 
	eth->EMACxMINDCLR = _EMACxMIND_NOTVALID_MASK; 
} 

//******************************************************************************
/* Function :  ETH_DataNotValid_Default

  Summary:
    Implements Default variant of PLIB_ETH_DataNotValid
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_DataNotValid function.
*/

PLIB_TEMPLATE bool ETH_DataNotValid_Default( ETH_MODULE_ID index )
{    
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) (eth->EMACxMIND.NOTVALID);
}


//******************************************************************************
/* Function :  ETH_MIIMIsScanning_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMIsScanning
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMIsScanning function.
*/

PLIB_TEMPLATE bool ETH_MIIMIsScanning_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) (eth->EMACxMIND.SCAN);
}


//******************************************************************************
/* Function :  ETH_MIIMIsBusy_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMIsBusy

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMIsBusy function.
*/

PLIB_TEMPLATE bool ETH_MIIMIsBusy_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) (eth->EMACxMIND.MIIMBUSY);
}


//******************************************************************************
/* Function :  ETH_ExistsMIIM_Indicators_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsMIIM_Indicators

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsMIIM_Indicators function.
*/

#define PLIB_ETH_ExistsMIIM_Indicators PLIB_ETH_ExistsMIIM_Indicators
PLIB_TEMPLATE bool ETH_ExistsMIIM_Indicators_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_MIIM_INDICATORS_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_PHYAddressGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_PHYAddressGet

  Description:
    This template implements the Default variant of the PLIB_ETH_PHYAddressGet function.
*/

PLIB_TEMPLATE uint8_t ETH_PHYAddressGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (uint8_t)(eth->EMACxMADR.PHYADDR);
}


//******************************************************************************
/* Function :  ETH_PHYAddressSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_PHYAddressSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_PHYAddressSet function.
*/

PLIB_TEMPLATE void ETH_PHYAddressSet_Default( ETH_MODULE_ID index , uint8_t phyAddr )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxMADR.PHYADDR = phyAddr;
}


//******************************************************************************
/* Function :  ETH_RegisterAddressGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_RegisterAddressGet

  Description:
    This template implements the Default variant of the PLIB_ETH_RegisterAddressGet function.
*/

PLIB_TEMPLATE uint8_t ETH_RegisterAddressGet_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (uint8_t)(eth->EMACxMADR.REGADDR);							 								 
}


//******************************************************************************
/* Function :  ETH_RegisterAddressSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_RegisterAddressSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RegisterAddressSet function.
*/

PLIB_TEMPLATE void ETH_RegisterAddressSet_Default( ETH_MODULE_ID index , uint8_t regAddr )
{				  
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxMADR.REGADDR = regAddr;
}


//******************************************************************************
/* Function :  ETH_ExistsMIIMAddresses_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsMIIMAddresses

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsMIIMAddresses function.
*/

#define PLIB_ETH_ExistsMIIMAddresses PLIB_ETH_ExistsMIIMAddresses
PLIB_TEMPLATE bool ETH_ExistsMIIMAddresses_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_MIIMADDRESSES_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_MIIMReadStart_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMReadStart
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMReadStart function.
*/

PLIB_TEMPLATE void ETH_MIIMReadStart_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxMCMDSET=_EMACxMCMD_READ_MASK;
	
#if defined(__PIC32MZ__)
    __asm__ __volatile__ ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");   // there's 4 clock cycles till busy is set for a read op
#else
    __asm__ __volatile__ ("nop; nop; nop; nop;");   // there's 4 clock cycles till busy is set for a read op
#endif
    
}


//******************************************************************************
/* Function :  ETH_MIIMWriteStart_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMWriteStart
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMWriteStart function.
*/

PLIB_TEMPLATE void ETH_MIIMWriteStart_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxMCMDCLR=_EMACxMCMD_READ_MASK;
}


//******************************************************************************
/* Function :  ETH_ExistsMIIMReadWrite_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsMIIMReadWrite

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsMIIMReadWrite function.
*/

#define PLIB_ETH_ExistsMIIMReadWrite PLIB_ETH_ExistsMIIMReadWrite
PLIB_TEMPLATE bool ETH_ExistsMIIMReadWrite_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_MIIMREADWRITE_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_MIIMScanModeEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMScanModeEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMScanModeEnable function.
*/

PLIB_TEMPLATE void ETH_MIIMScanModeEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxMCMDSET = _EMACxMCMD_SCAN_MASK;
}


//******************************************************************************
/* Function :  ETH_MIIMScanModeDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMScanModeDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMScanModeDisable function.
*/

PLIB_TEMPLATE void ETH_MIIMScanModeDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxMCMDCLR = _EMACxMCMD_SCAN_MASK;
}


//******************************************************************************
/* Function :  ETH_MIIMScanModeIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMScanModeIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMScanModeIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_MIIMScanModeIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxMCMD.SCAN);
}


//******************************************************************************
/* Function :  ETH_ExistsMIIMScanMode_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsMIIMScanMode

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsMIIMScanMode function.
*/

#define PLIB_ETH_ExistsMIIMScanMode PLIB_ETH_ExistsMIIMScanMode
PLIB_TEMPLATE bool ETH_ExistsMIIMScanMode_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_MIIMSCANMODE_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_MIIMWriteDataSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMWriteDataSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMWriteDataSet function.
*/

PLIB_TEMPLATE void ETH_MIIMWriteDataSet_Default( ETH_MODULE_ID index , uint16_t writeData )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		

	eth->EMACxMWTD.MWTD =writeData ;  				 
                     
#if defined(__PIC32MZ__)
    __asm__ __volatile__ ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"); // there's 2 clock cycles till busy is set for a write op
#else                     
    __asm__ __volatile__ ("nop; nop;"); // there's 2 clock cycles till busy is set for a write op
#endif
}


//******************************************************************************
/* Function :  ETH_MIIMReadDataGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMReadDataGet

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMReadDataGet function.
*/

PLIB_TEMPLATE uint16_t ETH_MIIMReadDataGet_Default( ETH_MODULE_ID index )
{    
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (uint16_t)(eth->EMACxMRDD.MRDD);								 
}


//******************************************************************************
/* Function :  ETH_ExistsMIIWriteReadData_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsMIIWriteReadData

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsMIIWriteReadData function.
*/

#define PLIB_ETH_ExistsMIIWriteReadData PLIB_ETH_ExistsMIIWriteReadData
PLIB_TEMPLATE bool ETH_ExistsMIIWriteReadData_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_MIIWRITEREADDATA_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_PatternMatchSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_PatternMatchSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_PatternMatchSet function.
*/

PLIB_TEMPLATE void ETH_PatternMatchSet_Default( ETH_MODULE_ID index , uint64_t patternMatchMaskValue )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	 
    PLIB_ASSERT(
        !(bool)(eth->ETHCON1.RXEN) ||(eth->ETHRXFC.PMMODE == 0),
        "Receive must be disabled or pattern match mode disabled!");
				
	eth->ETHPMM0SET = _ETHPMM0_PMMLOWER_MASK&((uint32_t)patternMatchMaskValue);
	
	eth->ETHPMM1SET =	_ETHPMM1_PMMUPPER_MASK&((uint32_t)(patternMatchMaskValue>>32));    
}


//******************************************************************************
/* Function :  ETH_PatternMatchGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_PatternMatchGet

  Description:
    This template implements the Default variant of the PLIB_ETH_PatternMatchGet function.
*/

PLIB_TEMPLATE uint64_t ETH_PatternMatchGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	return ((uint64_t)eth->ETHPMM0.PMMLOWER + ( (uint64_t)eth->ETHPMM1.PMMUPPER<<32));
}


//******************************************************************************
/* Function :  ETH_PatternMatchChecksumSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_PatternMatchChecksumSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_PatternMatchChecksumSet function.
*/

PLIB_TEMPLATE void ETH_PatternMatchChecksumSet_Default( ETH_MODULE_ID index , uint16_t PatternMatchChecksumValue )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	 
    PLIB_ASSERT(
        !(bool)(eth->ETHCON1.RXEN) ||(eth->ETHRXFC.PMMODE == 0),
        "Receive must be disabled or pattern match mode disabled!");
	    eth->ETHPMCSSET = _ETHPMCS_PMCS_MASK & ((uint32_t)PatternMatchChecksumValue << _ETHPMCS_PMCS_POSITION);							
}


//******************************************************************************
/* Function :  ETH_PatternMatchChecksumGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_PatternMatchChecksumGet

  Description:
    This template implements the Default variant of the PLIB_ETH_PatternMatchChecksumGet function.
*/

PLIB_TEMPLATE uint16_t ETH_PatternMatchChecksumGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
    return (uint16_t)eth->ETHPMCS.PMCS;
}


//******************************************************************************
/* Function :  ETH_PatternMatchOffsetSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_PatternMatchOffsetSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_PatternMatchOffsetSet function.
*/

PLIB_TEMPLATE void ETH_PatternMatchOffsetSet_Default( ETH_MODULE_ID index , uint16_t PatternMatchOffsetValue )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	 
    PLIB_ASSERT(!(bool)(eth->ETHCON1.RXEN) ||(eth->ETHRXFC.PMMODE == 0),
        "Receive must be disabled or pattern match mode disabled!");
		
	eth->ETHPMOSET = _ETHPMO_PMO_MASK & ((uint32_t)PatternMatchOffsetValue << _ETHPMO_PMO_POSITION );    
}


//******************************************************************************
/* Function :  ETH_PatternMatchOffsetGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_PatternMatchOffsetGet

  Description:
    This template implements the Default variant of the PLIB_ETH_PatternMatchOffsetGet function.
*/

PLIB_TEMPLATE uint16_t ETH_PatternMatchOffsetGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
    return (uint16_t)eth->ETHPMO.PMO;
}


//******************************************************************************
/* Function :  ETH_PatternMatchModeSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_PatternMatchModeSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_PatternMatchModeSet function.
*/

PLIB_TEMPLATE void ETH_PatternMatchModeSet_Default( ETH_MODULE_ID index , ETH_PATTERN_MATCH_MODE modeSel )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	
    PLIB_ASSERT(!(bool)(eth->ETHCON1.RXEN),"Receive must be disabled"); 			
	eth->ETHRXFCSET = _ETHRXFC_PMMODE_MASK & ((uint32_t) modeSel << _ETHRXFC_PMMODE_POSITION); 
}

//******************************************************************************
/* Function :  ETH_RxFiltersPMClr_Default

  Summary:
    Implements Default variant of ETH_RxFiltersPMClr

  Description:
    This template implements the Default variant of the ETH_RxFiltersPMClr function.
*/
PLIB_TEMPLATE void ETH_RxFiltersPMClr_Default(ETH_MODULE_ID index)
{
    ETH_PatternMatchModeSet_Default(index,0);
}
//******************************************************************************
/* Function :  ETH_PatternMatchModeGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_PatternMatchModeGet

  Description:
    This template implements the Default variant of the PLIB_ETH_PatternMatchModeGet function.
*/

PLIB_TEMPLATE ETH_PATTERN_MATCH_MODE ETH_PatternMatchModeGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
     return (ETH_PATTERN_MATCH_MODE)eth->ETHRXFC.PMMODE;                     
}


//******************************************************************************
/* Function :  ETH_ExistsPatternMatch_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsPatternMatch

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsPatternMatch function.
*/

#define PLIB_ETH_ExistsPatternMatch PLIB_ETH_ExistsPatternMatch
PLIB_TEMPLATE bool ETH_ExistsPatternMatch_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_PATTERNMATCH_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_PauseTimerSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_PauseTimerSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_PauseTimerSet function.
*/

PLIB_TEMPLATE void ETH_PauseTimerSet_Default( ETH_MODULE_ID index , uint16_t PauseTimerValue )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));					 
	eth->ETHCON1SET = _ETHCON1_PTV_MASK & ((uint32_t)PauseTimerValue << _ETHCON1_PTV_POSITION);				 
}


//******************************************************************************
/* Function :  ETH_PauseTimerGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_PauseTimerGet
	This function performs atomic register access.
	
  Description:
    This template implements the Default variant of the PLIB_ETH_PauseTimerGet function.
*/

PLIB_TEMPLATE uint16_t ETH_PauseTimerGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));					 
	return (uint16_t)eth->ETHCON1.PTV;		
}


//******************************************************************************
/* Function :  ETH_ExistsPauseTimer_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsPauseTimer

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsPauseTimer function.
*/

#define PLIB_ETH_ExistsPauseTimer PLIB_ETH_ExistsPauseTimer
PLIB_TEMPLATE bool ETH_ExistsPauseTimer_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_PAUSETIMER_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_ReceiveBufferSizeGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_ReceiveBufferSizeGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_ReceiveBufferSizeGet function.
*/

PLIB_TEMPLATE uint8_t ETH_ReceiveBufferSizeGet_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));					 
	return	(uint8_t)eth->ETHCON2.RXBUF_SZ;
}


//******************************************************************************
/* Function :  ETH_ReceiveBufferSizeSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_ReceiveBufferSizeSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_ReceiveBufferSizeSet function.
*/

PLIB_TEMPLATE void ETH_ReceiveBufferSizeSet_Default( ETH_MODULE_ID index , uint8_t ReceiveBufferSize )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));					 
	PLIB_ASSERT(!(bool)eth->ETHCON1.RXEN,"Receive must be disabled!");
	eth->ETHCON2CLR = _ETHCON2_RXBUF_SZ_MASK;
	eth->ETHCON2SET = _ETHCON2_RXBUF_SZ_MASK & ((uint32_t)ReceiveBufferSize << _ETHCON2_RXBUF_SZ_POSITION);
}

//******************************************************************************
/* Function :  ETH_RxSetBufferSize_Defaultt

  Summary:
    Implements Default variant of ETH_RxSetBufferSize

  Description:
    This template implements the Default variant of the ETH_RxSetBufferSize function.
*/
PLIB_TEMPLATE int ETH_RxSetBufferSize_Default(ETH_MODULE_ID index,int rxBuffSize)
{
    rxBuffSize >>= 4;     // truncate
    if(!rxBuffSize)
    {
        return -1;
    }

    ETH_ReceiveBufferSizeSet_Default(index, rxBuffSize);

    return rxBuffSize << 4;
}


//******************************************************************************
/* Function :  ETH_ExistsReceiveBufferSize_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsReceiveBufferSize

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsReceiveBufferSize function.
*/

#define PLIB_ETH_ExistsReceiveBufferSize PLIB_ETH_ExistsReceiveBufferSize
PLIB_TEMPLATE bool ETH_ExistsReceiveBufferSize_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_RECEIVEBUFFERSIZE_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_RxEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_RxEnable function.
*/

PLIB_TEMPLATE void ETH_RxEnable_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));					 
	eth->ETHCON1SET =_ETHCON1_RXEN_MASK;
}


//******************************************************************************
/* Function :  ETH_RxDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_RxDisable function.
*/

PLIB_TEMPLATE void ETH_RxDisable_Default( ETH_MODULE_ID index )
{
   	volatile eth_registers_t * eth = ((eth_registers_t *)(index));					 
	eth->ETHCON1CLR =_ETHCON1_RXEN_MASK;
}


//******************************************************************************
/* Function :  ETH_RxIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_RxIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_RxIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));					 
	return	(bool)eth->ETHCON1.RXEN;
}


//******************************************************************************
/* Function :  ETH_ExistsRxEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsRxEnable

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsRxEnable function.
*/

#define PLIB_ETH_ExistsRxEnable PLIB_ETH_ExistsRxEnable
PLIB_TEMPLATE bool ETH_ExistsRxEnable_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_RECEIVEENABLE_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_ReceiveFilterEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_ReceiveFilterEnable
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_ReceiveFilterEnable function.
*/

PLIB_TEMPLATE void ETH_ReceiveFilterEnable_Default( ETH_MODULE_ID index , ETH_RECEIVE_FILTER filter )
{
   	volatile eth_registers_t * eth = ((eth_registers_t *)(index));			
	
	eth->ETHRXFCSET = ((uint32_t)filter  & ETH_ENABLE_ALL_FILTER);	
}


//******************************************************************************
/* Function :  ETH_RxFiltersSet_Default

  Summary:
    Implements Default variant of ETH_RxFiltersSet

  Description:
    This template implements the Default variant of the ETH_RxFiltersSet function.
*/
PLIB_TEMPLATE void ETH_RxFiltersSet_Default(ETH_MODULE_ID index,ETH_RX_FILTERS rxFilters)
{
    ETH_ReceiveFilterEnable_Default(index,rxFilters);
}


//******************************************************************************
/* Function :  ETH_ReceiveFilterDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_ReceiveFilterDisable
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_ReceiveFilterDisable function.
*/

PLIB_TEMPLATE void ETH_ReceiveFilterDisable_Default( ETH_MODULE_ID index , ETH_RECEIVE_FILTER filter )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	eth->ETHRXFCCLR = (uint32_t)filter & ETH_ENABLE_ALL_FILTER;	
}

//******************************************************************************
/* Function :  ETH_RxFiltersClr_Default

  Summary:
    Implements Default variant of ETH_RxFiltersClr

  Description:
    This template implements the Default variant of the ETH_RxFiltersClr function.
*/
PLIB_TEMPLATE void ETH_RxFiltersClr_Default(ETH_MODULE_ID index,ETH_RX_FILTERS rxFilters)
{
    ETH_ReceiveFilterDisable_Default(index,rxFilters);
}

//******************************************************************************
/* Function :  ETH_RxFiltersWrite_Default

  Summary:
    Implements Default variant of ETH_RxFiltersWrite

  Description:
    This template implements the Default variant of the ETH_RxFiltersWrite function.
*/

PLIB_TEMPLATE void ETH_RxFiltersWrite_Default(ETH_MODULE_ID index,ETH_RX_FILTERS rxFilters)
{
    ETH_ReceiveFilterEnable_Default(index,rxFilters);
    ETH_ReceiveFilterDisable_Default(index,~rxFilters);
}

//******************************************************************************
/* Function :  ETH_ReceiveFilterIsEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_ReceiveFilterIsEnable
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_ReceiveFilterIsEnable function.
*/

PLIB_TEMPLATE bool ETH_ReceiveFilterIsEnable_Default( ETH_MODULE_ID index , ETH_RECEIVE_FILTER filter )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
		
    return ((eth->ETHRXFC.w &(uint32_t)filter) > 0 ? true : false );
}


//******************************************************************************
/* Function :  ETH_ExistsReceiveFilters_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsReceiveFilters

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsReceiveFilters function.
*/

#define PLIB_ETH_ExistsReceiveFilters PLIB_ETH_ExistsReceiveFilters
PLIB_TEMPLATE bool ETH_ExistsReceiveFilters_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_RECEIVEFILTERS_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_RxOverflowCountClear_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxOverflowCountClear
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RxOverflowCountClear function.
*/

PLIB_TEMPLATE void ETH_RxOverflowCountClear_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	eth->ETHRXOVFLOW.RXOVFLWCNT=0x0000;
}


//******************************************************************************
/* Function :  ETH_RxOverflowCountGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxOverflowCountGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RxOverflowCountGet function.
*/

PLIB_TEMPLATE uint16_t ETH_RxOverflowCountGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	return (uint16_t)eth->ETHRXOVFLOW.RXOVFLWCNT;
}


//******************************************************************************
/* Function :  ETH_ExistsReceiveOverflowCount_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsReceiveOverflowCount
	

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsReceiveOverflowCount function.
*/

#define PLIB_ETH_ExistsReceiveOverflowCount PLIB_ETH_ExistsReceiveOverflowCount
PLIB_TEMPLATE bool ETH_ExistsReceiveOverflowCount_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_RECEIVEOVERFLOWCOUNT_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_RxFullWmarkSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxFullWmarkSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RxFullWmarkSet function.
*/

PLIB_TEMPLATE void ETH_RxFullWmarkSet_Default( ETH_MODULE_ID index , uint8_t watermarkValue )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));					 

	eth->ETHRXWM.RXFWM = watermarkValue;			 
}


//******************************************************************************
/* Function :  ETH_RxFullWmarkGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxFullWmarkGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RxFullWmarkGet function.
*/

PLIB_TEMPLATE uint8_t ETH_RxFullWmarkGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));					 
	return (uint8_t) eth->ETHRXWM.RXFWM;
}


//******************************************************************************
/* Function :  ETH_RxEmptyWmarkSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxEmptyWmarkSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RxEmptyWmarkSet function.
*/

PLIB_TEMPLATE void ETH_RxEmptyWmarkSet_Default( ETH_MODULE_ID index , uint8_t watermarkValue )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));					 
    eth->ETHRXWM.RXEWM = watermarkValue;		
}


//******************************************************************************
/* Function :  ETH_RxEmptyWmarkGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxEmptyWmarkGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RxEmptyWmarkGet function.
*/

PLIB_TEMPLATE uint8_t ETH_RxEmptyWmarkGet_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));					 
	return (uint8_t) eth->ETHRXWM.RXEWM;
}


//******************************************************************************
/* Function :  ETH_ExistsReceiveWmarks_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsReceiveWmarks

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsReceiveWmarks function.
*/

#define PLIB_ETH_ExistsReceiveWmarks PLIB_ETH_ExistsReceiveWmarks
PLIB_TEMPLATE bool ETH_ExistsReceiveWmarks_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_RECEIVEWMARKS_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_ReTxMaxGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_ReTxMaxGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_ReTxMaxGet function.
*/

PLIB_TEMPLATE uint8_t ETH_ReTxMaxGet_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	return (uint8_t) eth->EMACxCLRT.RETX;								
}


//******************************************************************************
/* Function :  ETH_ReTxMaxSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_ReTxMaxSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_ReTxMaxSet function.
*/

PLIB_TEMPLATE void ETH_ReTxMaxSet_Default( ETH_MODULE_ID index , uint16_t retransmitMax )
{	 
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	

	eth->EMACxCLRT.RETX = retransmitMax;												 
}


//******************************************************************************
/* Function :  ETH_ExistsRetransmissionMaximum_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsRetransmissionMaximum

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsRetransmissionMaximum function.
*/

#define PLIB_ETH_ExistsRetransmissionMaximum PLIB_ETH_ExistsRetransmissionMaximum
PLIB_TEMPLATE bool ETH_ExistsRetransmissionMaximum_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_RETRANSMISSIONMAXIMUM_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_RMIIResetEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_RMIIResetEnable
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RMIIResetEnable function.
*/

PLIB_TEMPLATE void ETH_RMIIResetEnable_Default( ETH_MODULE_ID index )
{   	
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	eth->EMACxSUPPSET = _EMACxSUPP_RESETRMII_MASK; 
}


//******************************************************************************
/* Function :  ETH_RMIIResetDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_RMIIResetDisable
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RMIIResetDisable function.
*/

PLIB_TEMPLATE void ETH_RMIIResetDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	eth->EMACxSUPP.RESETRMII = 0ul;
}


//******************************************************************************
/* Function :  ETH_RMIIResetIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_RMIIResetIsEnabled
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RMIIResetIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_RMIIResetIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	return (bool) eth->EMACxSUPP.RESETRMII; 
}


//******************************************************************************
/* Function :  ETH_RMIISpeedGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_RMIISpeedGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RMIISpeedGet function.
*/

PLIB_TEMPLATE ETH_RMII_SPEED ETH_RMIISpeedGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	return (ETH_RMII_SPEED) eth->EMACxSUPP.SPEEDRMII; 	
}


//******************************************************************************
/* Function :  ETH_RMIISpeedSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_RMIISpeedSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RMIISpeedSet function.
*/

PLIB_TEMPLATE void ETH_RMIISpeedSet_Default( ETH_MODULE_ID index , ETH_RMII_SPEED RMIISpeed )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	eth->EMACxSUPP.SPEEDRMII = RMIISpeed ;	
}


//******************************************************************************
/* Function :  ETH_ExistsRMII_Support_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsRMII_Support

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsRMII_Support function.
*/

#define PLIB_ETH_ExistsRMII_Support PLIB_ETH_ExistsRMII_Support
PLIB_TEMPLATE bool ETH_ExistsRMII_Support_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_RMII_SUPPORT_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_RxBufferCountDecrement_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxBufferCountDecrement
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_RxBufferCountDecrement function.
*/

PLIB_TEMPLATE void ETH_RxBufferCountDecrement_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	eth->ETHCON1SET =_ETHCON1_BUFCDEC_MASK;
}


//******************************************************************************
/* Function :  ETH_ExistsRxBufferCountDecrement_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsRxBufferCountDecrement

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsRxBufferCountDecrement function.
*/

#define PLIB_ETH_ExistsRxBufferCountDecrement PLIB_ETH_ExistsRxBufferCountDecrement
PLIB_TEMPLATE bool ETH_ExistsRxBufferCountDecrement_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_RXBUFFERCOUNTDECREMENT_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_RxFiltersPMSet_Default

  Summary:
    Implements Default variant of ETH_RxFiltersPMSet

  Description:
    This template implements the Default variant of the ETH_RxFiltersPMSet function.
*/
PLIB_TEMPLATE void ETH_RxFiltersPMSet_Default(ETH_MODULE_ID index,
                                   ETH_PMATCH_MODE mode,
                                   uint64_t matchMask,
                                   uint32_t matchOffs,
                                   uint32_t matchChecksum)
{
    ETH_PatternMatchModeSet_Default(index,ETH_PATTERN_MATCH_DISABLED);

    ETH_PatternMatchSet_Default(index,matchMask);
    ETH_PatternMatchOffsetSet_Default(index,matchOffs);
    ETH_PatternMatchChecksumSet_Default(index,matchChecksum);

    if(mode&ETH_FILT_PMATCH_INVERT)
    {
        ETH_ReceiveFilterEnable_Default(index,ETH_PATTERN_MATCH_INVERSION);
    }
    else
    {
        ETH_ReceiveFilterDisable_Default(index,ETH_PATTERN_MATCH_INVERSION);
    }

 // Enable Pattern Match mode
    ETH_PatternMatchModeSet_Default(index,mode&(~ETH_FILT_PMATCH_INVERT));
}

//******************************************************************************
/* Function :  ETH_ExistsRxFilterPMSet_Default

  Summary:
    Implements Default variant of ETH_ExistsRxFilterPMSet

  Description:
    This template implements the Default variant of the ETH_ExistsRxFilterPMSet function.
*/

#define PLIB_ETH_ExistsRxFilterPMSet PLIB_ETH_ExistsRxFilterPMSet
PLIB_TEMPLATE bool ETH_ExistsRxFiltersPMSet_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_RXFILTERPM_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_RxPacketDescAddrSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxPacketDescAddrSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RxPacketDescAddrSet function.
*/

PLIB_TEMPLATE void ETH_RxPacketDescAddrSet_Default( ETH_MODULE_ID index , uint8_t *rxPacketDescStartAddr )
{  
   
   volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
    eth->ETHRXST.RXSTADDR = ((uint32_t)rxPacketDescStartAddr>>2);


}

//******************************************************************************
/* Function :  ETH_RxPacketDescAddrGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxPacketDescAddrGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RxPacketDescAddrGet function.
*/

PLIB_TEMPLATE uint8_t *ETH_RxPacketDescAddrGet_Default( ETH_MODULE_ID index )
{
  	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
    return (uint8_t *)eth->ETHRXST.w;
}


//******************************************************************************
/* Function :  ETH_ExistsRxPacketDescriptorAddress_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsRxPacketDescriptorAddress
	

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsRxPacketDescriptorAddress function.
*/

#define PLIB_ETH_ExistsRxPacketDescriptorAddress PLIB_ETH_ExistsRxPacketDescriptorAddress
PLIB_TEMPLATE bool ETH_ExistsRxPacketDescriptorAddress_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_RXPACKETDESCRIPTORADDRESS_DEFAULT_H*/
//******************************************************************************
/* Function :  ETH_StationAddressGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_StationAddressGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_StationAddressGet function.
*/

PLIB_TEMPLATE uint8_t ETH_StationAddressGet_Default( ETH_MODULE_ID index , uint8_t which )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));
    volatile uint32_t * ethReg ;
	volatile uint32_t bitsMask =_EMACxSA0_STNADDR6_MASK;
	volatile uint32_t bitsPos = _EMACxSA0_STNADDR6_POSITION;
	volatile uint8_t addrRegIdx;
	
	addrRegIdx=2u-((which-1u)/2u); //Which EMACxSAx register to be accessed

	if(which%2u)
	{
		bitsMask = _EMACxSA0_STNADDR5_MASK;
		bitsPos = _EMACxSA0_STNADDR5_POSITION;
	}
	
	if(addrRegIdx == 0)
	{
		eth = (eth_registers_t * )(&eth->EMACxSA0); 
	}
	else if(addrRegIdx == 1)
	{
		eth = (eth_registers_t * )(&eth->EMACxSA1); 
	}
	else 
	{
		eth = (eth_registers_t * )(&eth->EMACxSA2); 
	}
	ethReg = (uint32_t *)eth;	
	
	return (uint8_t) ((*ethReg & bitsMask)>> bitsPos);
    
}


//******************************************************************************
/* Function :  ETH_StationAddressSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_StationAddressSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_StationAddressSet function.
*/

PLIB_TEMPLATE void ETH_StationAddressSet_Default( ETH_MODULE_ID index , uint8_t which , uint8_t stationAddress )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	volatile uint8_t * ethReg ;
	volatile uint32_t bitsMask = _EMACxSA0_STNADDR6_MASK;
	volatile uint32_t bitsPos = _EMACxSA0_STNADDR6_POSITION;
	volatile uint32_t *ethSetReg;
	volatile uint32_t *ethClrReg;
	volatile uint8_t addrRegIdx;
	
	if(which % 2u)
	{
		bitsMask = _EMACxSA0_STNADDR5_MASK;
		bitsPos = _EMACxSA0_STNADDR5_POSITION;
	}
	
	addrRegIdx = 2 - ((which - 1) / 2); //Which EMACxSAx register to be accessed
	if(addrRegIdx == 0)
	{
		eth = (eth_registers_t * )(&eth->EMACxSA0); 
	}
	else if(addrRegIdx == 1)
	{
		eth = (eth_registers_t * )(&eth->EMACxSA1); 
	}
	else 
	{
		eth = (eth_registers_t * )(&eth->EMACxSA2); 
	}
	ethReg = (uint8_t *)eth;	
	
	ethClrReg = (uint32_t*)(ethReg + 4); // SET Reg Address for the corresponding EMACxSAx reg 
	ethSetReg = (uint32_t*)(ethReg + 8); // CLR Reg Address for the corresponding EMACxSAx reg
	
	*ethClrReg = bitsMask;
	*ethSetReg = bitsMask & ((uint32_t)stationAddress << bitsPos);	
      
}

//******************************************************************************
/* Function :  ETH_MACSetAddress_Default

  Summary:
    Implements Default variant of ETH_MACSetAddress

  Description:
    This template implements the Default variant of the ETH_MACSetAddress function.
*/
PLIB_TEMPLATE void ETH_MACSetAddress_Default(ETH_MODULE_ID index,uint8_t bAddress[6])
{
    ETH_StationAddressSet_Default(index, 1, bAddress[0]);
    ETH_StationAddressSet_Default(index, 2, bAddress[1]);
    ETH_StationAddressSet_Default(index, 3, bAddress[2]);
    ETH_StationAddressSet_Default(index, 4, bAddress[3]);
    ETH_StationAddressSet_Default(index, 5, bAddress[4]);
    ETH_StationAddressSet_Default(index, 6, bAddress[5]);    
}

//******************************************************************************
/* Function :  ETH_MACGetAddress_Default

  Summary:
    Implements Default variant of ETH_MACGetAddress

  Description:
    This template implements the Default variant of the ETH_MACGetAddress function.
*/
PLIB_TEMPLATE void ETH_MACGetAddress_Default(ETH_MODULE_ID index,uint8_t bAddress[6])
{
    *bAddress++ = ETH_StationAddressGet_Default(index,1);
    *bAddress++ = ETH_StationAddressGet_Default(index,2);
    *bAddress++ = ETH_StationAddressGet_Default(index,3);
    *bAddress++ = ETH_StationAddressGet_Default(index,4);
    *bAddress++ = ETH_StationAddressGet_Default(index,5);
    *bAddress   = ETH_StationAddressGet_Default(index,6);
}

//******************************************************************************
/* Function :  ETH_ExistsStationAddress_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsStationAddress

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsStationAddress function.
*/

#define PLIB_ETH_ExistsStationAddress PLIB_ETH_ExistsStationAddress
PLIB_TEMPLATE bool ETH_ExistsStationAddress_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_STATIONADDRESS_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_StopInIdleEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_StopInIdleEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_StopInIdleEnable function.
*/

PLIB_TEMPLATE void ETH_StopInIdleEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	eth->ETHCON1SET =_ETHCON1_SIDL_MASK ;	
}


//******************************************************************************
/* Function :  ETH_StopInIdleDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_StopInIdleDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_StopInIdleDisable function.
*/

PLIB_TEMPLATE void ETH_StopInIdleDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
    eth->ETHCON1.SIDL=0 ;
}


//******************************************************************************
/* Function :  ETH_StopInIdleIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_StopInIdleIsEnabled
		
  Description:
    This template implements the Default variant of the PLIB_ETH_StopInIdleIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_StopInIdleIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	return (bool)(eth->ETHCON1.SIDL); 
}


//******************************************************************************
/* Function :  ETH_ExistsStopInIdle_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsStopInIdle

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsStopInIdle function.
*/

#define PLIB_ETH_ExistsStopInIdle PLIB_ETH_ExistsStopInIdle
PLIB_TEMPLATE bool ETH_ExistsStopInIdle_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_STOPINIDLE_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_TxRTSEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_TxRTSEnable
	This function performs atomic register access.	

  Description:
    This template implements the Default variant of the PLIB_ETH_TxRTSEnable function.
*/

PLIB_TEMPLATE void ETH_TxRTSEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	eth->ETHCON1SET =_ETHCON1_TXRTS_MASK;
}


//******************************************************************************
/* Function :  ETH_TxRTSDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_TxRTSDisable
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_TxRTSDisable function.
*/

PLIB_TEMPLATE void ETH_TxRTSDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	eth->ETHCON1CLR =_ETHCON1_TXRTS_MASK;
}


//******************************************************************************
/* Function :  ETH_TxRTSIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_TxRTSIsEnabled

  Description:
    This template implements the Default variant of the PLIB_ETH_TxRTSIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_TxRTSIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	return (bool)(eth->ETHCON1.TXRTS);  
}


//******************************************************************************
/* Function :  ETH_ExistsTransmitRTS_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsTransmitRTS

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsTransmitRTS function.
*/

#define PLIB_ETH_ExistsTransmitRTS PLIB_ETH_ExistsTransmitRTS
PLIB_TEMPLATE bool ETH_ExistsTransmitRTS_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_TRANSMITRTS_DEFAULT_H*/

//******************************************************************************
/* Function :  ETH_TxPacketDescAddrSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_TxPacketDescAddrSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_TxPacketDescAddrSet function.
*/

PLIB_TEMPLATE void ETH_TxPacketDescAddrSet_Default( ETH_MODULE_ID index , uint8_t * txPacketDescStartAddr )
{
volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
    eth->ETHTXST.TXSTADDR = ((uint32_t)txPacketDescStartAddr>>2);

	
}


//******************************************************************************
/* Function :  ETH_TxPacketDescAddrGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_TxPacketDescAddrGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_TxPacketDescAddrGet function.
*/

PLIB_TEMPLATE uint8_t *  ETH_TxPacketDescAddrGet_Default( ETH_MODULE_ID index )
{
	
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
    return (uint8_t *)eth->ETHTXST.w;

}


//******************************************************************************
/* Function :  ETH_ExistsTxPacketDescriptorAddress_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsTxPacketDescriptorAddress

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsTxPacketDescriptorAddress function.
*/

#define PLIB_ETH_ExistsTxPacketDescriptorAddress PLIB_ETH_ExistsTxPacketDescriptorAddress
PLIB_TEMPLATE bool ETH_ExistsTxPacketDescriptorAddress_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_TXPACKETDESCRIPTORADDRESS_DEFAULT_H*/
#endif // _DRV_ETHMAC_LIB_H