<#----------------------------------------------------------------------------
* Copyright (C) 2021 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
---------------------------------------------------------------------------->
<#if (TCPIP_INTMAC_DEVICE)?has_content>  
<#if (((TCPIP_INTMAC_DEVICE) == "SAME7x_V7x") || ((TCPIP_INTMAC_DEVICE) == "SAMA5D2") || ((TCPIP_INTMAC_DEVICE) == "SAMRH71"))> 

const DRV_GMAC_RXQUE_FILTER_INIT DRV_GMAC_Rx_Filt_Init =
{ 
	.type1FiltCount = TCPIP_GMAC_SCREEN1_COUNT_QUE,
<#if (TCPIP_GMAC_RX_QUE_FILTER_EN)?has_content>  
<#if ((TCPIP_GMAC_RX_QUE_FILTER_EN) == true)> 	
<#if (TCPIP_GMAC_SCREEN1_COUNT_QUE > 0)>
<#list 0 ..(TCPIP_GMAC_SCREEN1_COUNT_QUE -1) as i >
	.type1FiltInit[${i}].queueIndex = TCPIP_GMAC_SCREEN1_QUE_IDX${i},
	.type1FiltInit[${i}].dstcEnable = TCPIP_QUE_SCREEN1_DSTCEN_IDX${i},
	<#assign dstcen = "TCPIP_QUE_SCREEN1_DSTCEN_IDX${i}"?eval>
	<#if dstcen == true>
    .type1FiltInit[${i}].dstcValue = TCPIP_QUE_SCREEN1_DSTC_IDX${i},
	</#if>	
    .type1FiltInit[${i}].udpEnable = TCPIP_QUE_SCREEN1_UDPEN_IDX${i},
	<#assign udpen = "TCPIP_QUE_SCREEN1_UDPEN_IDX${i}"?eval>
	<#if udpen == true>
    .type1FiltInit[${i}].udpPortNum = TCPIP_QUE_SCREEN1_UDPPORTNUM_IDX${i},
	</#if>	
	
</#list>
</#if>
</#if>
</#if>
	.type2FiltCount = TCPIP_GMAC_SCREEN2_COUNT_QUE,
<#if (TCPIP_GMAC_RX_QUE_FILTER_EN)?has_content>  
<#if ((TCPIP_GMAC_RX_QUE_FILTER_EN) == true)> 
<#if (TCPIP_GMAC_SCREEN2_COUNT_QUE > 0)>
<#list 0 ..(TCPIP_GMAC_SCREEN2_COUNT_QUE -1) as i >
	.type2FiltInit[${i}].queueIndex = TCPIP_GMAC_SCREEN2_QUE_IDX${i},
	.type2FiltInit[${i}].vlanPrioEnable = TCPIP_QUE_SCREEN2_VLANPRIOEN_IDX${i},
	<#assign vlanprioen = "TCPIP_QUE_SCREEN2_VLANPRIOEN_IDX${i}"?eval>
	<#if vlanprioen == true>
	.type2FiltInit[${i}].vlanPrio = TCPIP_QUE_SCREEN2_VLANPRIO_IDX${i},
	</#if>	
    .type2FiltInit[${i}].ethTypeEnable = TCPIP_QUE_SCREEN2_ETHTYPEEN_IDX${i},
	<#assign ethtypeen = "TCPIP_QUE_SCREEN2_ETHTYPEEN_IDX${i}"?eval>
	<#if ethtypeen == true>
    .type2FiltInit[${i}].ethType = TCPIP_QUE_SCREEN2_ETHTYPE_IDX${i},
	</#if>	
	
    .type2FiltInit[${i}].compAEnable = TCPIP_QUE_SCREEN2_COMPAEN_IDX${i},
	<#assign compaen = "TCPIP_QUE_SCREEN2_COMPAEN_IDX${i}"?eval>
	<#if compaen == true>
	.type2FiltInit[${i}].compAValue = TCPIP_QUE_SCREEN2_COMPA_IDX${i},
	.type2FiltInit[${i}].compAMask = TCPIP_QUE_SCREEN2_COMPAMASK_IDX${i},
	.type2FiltInit[${i}].compAOffset = TCPIP_QUE_SCREEN2_COMPAOFFST_IDX${i},
	.type2FiltInit[${i}].compAOffsetStart = TCPIP_QUE_SCREEN2_COMPAOFFSTSTRT_IDX${i},
	</#if>	
	
	.type2FiltInit[${i}].compBEnable = TCPIP_QUE_SCREEN2_COMPBEN_IDX${i},
	<#assign compben = "TCPIP_QUE_SCREEN2_COMPBEN_IDX${i}"?eval>
	<#if compben == true>
	.type2FiltInit[${i}].compBValue = TCPIP_QUE_SCREEN2_COMPB_IDX${i},
	.type2FiltInit[${i}].compBMask = TCPIP_QUE_SCREEN2_COMPBMASK_IDX${i},
	.type2FiltInit[${i}].compBOffset = TCPIP_QUE_SCREEN2_COMPBOFFST_IDX${i},
	.type2FiltInit[${i}].compBOffsetStart = TCPIP_QUE_SCREEN2_COMPBOFFSTSTRT_IDX${i},
	</#if>
	
	.type2FiltInit[${i}].compCEnable = TCPIP_QUE_SCREEN2_COMPCEN_IDX${i},
	<#assign compcen = "TCPIP_QUE_SCREEN2_COMPCEN_IDX${i}"?eval>
	<#if compcen == true>	
	.type2FiltInit[${i}].compCValue = TCPIP_QUE_SCREEN2_COMPC_IDX${i},
	.type2FiltInit[${i}].compCMask = TCPIP_QUE_SCREEN2_COMPCMASK_IDX${i},
	.type2FiltInit[${i}].compCOffset = TCPIP_QUE_SCREEN2_COMPCOFFST_IDX${i},
	.type2FiltInit[${i}].compCOffsetStart = TCPIP_QUE_SCREEN2_COMPCOFFSTSTRT_IDX${i},
	</#if>
	
</#list>
</#if>
</#if>
</#if>
};
</#if>
</#if>
	
/*** GMAC MAC Initialization Data ***/
const TCPIP_MODULE_MAC_PIC32C_CONFIG tcpipMACPIC32CINTInitData =
{ 
	/** QUEUE 0 Intialization**/
<#if (TCPIP_GMAC_QUEUE_0)?has_content>
	<#if (TCPIP_GMAC_QUEUE_0)  == true>
	<#if (TCPIP_GMAC_TX_EN_QUE0)  == true>
	.gmac_queue_config[0].queueTxEnable	= true,
	<#else>	
	.gmac_queue_config[0].queueTxEnable	= false,
	</#if>	
	<#if (TCPIP_GMAC_RX_EN_QUE0)  == true>
	.gmac_queue_config[0].queueRxEnable	= true,
	<#else>	
	.gmac_queue_config[0].queueRxEnable	= false,
	</#if>	
	<#else>	
	.gmac_queue_config[0].queueTxEnable	= false,
	.gmac_queue_config[0].queueRxEnable	= false,		
	</#if>
	.gmac_queue_config[0].nRxDescCnt	= TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE0,
	.gmac_queue_config[0].nTxDescCnt	= TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE0,
	.gmac_queue_config[0].rxBufferSize	= TCPIP_GMAC_RX_BUFF_SIZE_QUE0,
	.gmac_queue_config[0].txMaxPktSize	= TCPIP_GMAC_MAX_TX_PKT_SIZE_QUE0,
	.gmac_queue_config[0].nRxDedicatedBuffers	= TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE0,
	.gmac_queue_config[0].nRxAddlBuffCount	= TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE0,
	.gmac_queue_config[0].nRxBuffCntThres	= TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE0,
	.gmac_queue_config[0].nRxBuffAllocCnt	= TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE0,
</#if>	

<#if (TCPIP_GMAC_QUEUE_1)?has_content>
	/** QUEUE 1 Intialization**/
	<#if (TCPIP_GMAC_QUEUE_1)  == true>
	<#if (TCPIP_GMAC_TX_EN_QUE1)  == true>
	.gmac_queue_config[1].queueTxEnable	= true,
	<#else>	
	.gmac_queue_config[1].queueTxEnable	= false,
	</#if>	
	<#if (TCPIP_GMAC_RX_EN_QUE1)  == true>
	.gmac_queue_config[1].queueRxEnable	= true,
	<#else>	
	.gmac_queue_config[1].queueRxEnable	= false,
	</#if>	
	<#else>			
	.gmac_queue_config[1].queueTxEnable	= false,
	.gmac_queue_config[1].queueRxEnable	= false,
	</#if>
	.gmac_queue_config[1].nRxDescCnt	= TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE1,
	.gmac_queue_config[1].nTxDescCnt	= TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE1,
	.gmac_queue_config[1].rxBufferSize	= TCPIP_GMAC_RX_BUFF_SIZE_QUE1,
	.gmac_queue_config[1].txMaxPktSize	= TCPIP_GMAC_MAX_TX_PKT_SIZE_QUE1,
	.gmac_queue_config[1].nRxDedicatedBuffers	= TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE1,
	.gmac_queue_config[1].nRxAddlBuffCount	= TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE1,
	.gmac_queue_config[1].nRxBuffCntThres	= TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE1,
	.gmac_queue_config[1].nRxBuffAllocCnt	= TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE1,
</#if>

<#if (TCPIP_GMAC_QUEUE_2)?has_content>
	/** QUEUE 2 Intialization**/
	<#if (TCPIP_GMAC_QUEUE_2)  == true>
	<#if (TCPIP_GMAC_TX_EN_QUE2)  == true>
	.gmac_queue_config[2].queueTxEnable	= true,
	<#else>	
	.gmac_queue_config[2].queueTxEnable	= false,
	</#if>	
	<#if (TCPIP_GMAC_RX_EN_QUE2)  == true>
	.gmac_queue_config[2].queueRxEnable	= true,
	<#else>	
	.gmac_queue_config[2].queueRxEnable	= false,
	</#if>
	<#else>	
	.gmac_queue_config[2].queueTxEnable	= false,
	.gmac_queue_config[2].queueRxEnable	= false,		
	</#if>
	.gmac_queue_config[2].nRxDescCnt	= TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE2,
	.gmac_queue_config[2].nTxDescCnt	= TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE2,
	.gmac_queue_config[2].rxBufferSize	= TCPIP_GMAC_RX_BUFF_SIZE_QUE2,
	.gmac_queue_config[2].txMaxPktSize	= TCPIP_GMAC_MAX_TX_PKT_SIZE_QUE2,
	.gmac_queue_config[2].nRxDedicatedBuffers	= TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE2,
	.gmac_queue_config[2].nRxAddlBuffCount	= TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE2,
	.gmac_queue_config[2].nRxBuffCntThres	= TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE2,
	.gmac_queue_config[2].nRxBuffAllocCnt	= TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE2,
</#if>

<#if (TCPIP_GMAC_QUEUE_3)?has_content>
	/** QUEUE 3 Intialization**/
	<#if (TCPIP_GMAC_QUEUE_3)  == true>
	<#if (TCPIP_GMAC_TX_EN_QUE3)  == true>
	.gmac_queue_config[3].queueTxEnable	= true,
	<#else>	
	.gmac_queue_config[3].queueTxEnable	= false,
	</#if>	
	<#if (TCPIP_GMAC_RX_EN_QUE3)  == true>
	.gmac_queue_config[3].queueRxEnable	= true,
	<#else>	
	.gmac_queue_config[3].queueRxEnable	= false,
	</#if>
	<#else>		
	.gmac_queue_config[3].queueTxEnable	= false,
	.gmac_queue_config[3].queueRxEnable	= false,
	</#if>
	.gmac_queue_config[3].nRxDescCnt	= TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE3,
	.gmac_queue_config[3].nTxDescCnt	= TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE3,
	.gmac_queue_config[3].rxBufferSize	= TCPIP_GMAC_RX_BUFF_SIZE_QUE3,
	.gmac_queue_config[3].txMaxPktSize	= TCPIP_GMAC_MAX_TX_PKT_SIZE_QUE3,
	.gmac_queue_config[3].nRxDedicatedBuffers	= TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE3,
	.gmac_queue_config[3].nRxAddlBuffCount	= TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE3,
	.gmac_queue_config[3].nRxBuffCntThres	= TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE3,
	.gmac_queue_config[3].nRxBuffAllocCnt	= TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE3,
</#if>

<#if (TCPIP_GMAC_QUEUE_4)?has_content>	
	/** QUEUE 4 Intialization**/
	<#if (TCPIP_GMAC_QUEUE_4)  == true>
	<#if (TCPIP_GMAC_TX_EN_QUE4)  == true>
	.gmac_queue_config[4].queueTxEnable	= true,
	<#else>	
	.gmac_queue_config[4].queueTxEnable	= false,
	</#if>	
	<#if (TCPIP_GMAC_RX_EN_QUE4)  == true>
	.gmac_queue_config[4].queueRxEnable	= true,
	<#else>	
	.gmac_queue_config[4].queueRxEnable	= false,
	</#if>
	<#else>	
	.gmac_queue_config[4].queueTxEnable	= false,
	.gmac_queue_config[4].queueRxEnable	= false,		
	</#if>
	.gmac_queue_config[4].nRxDescCnt	= TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE4,
	.gmac_queue_config[4].nTxDescCnt	= TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE4,
	.gmac_queue_config[4].rxBufferSize	= TCPIP_GMAC_RX_BUFF_SIZE_QUE4,
	.gmac_queue_config[4].txMaxPktSize	= TCPIP_GMAC_MAX_TX_PKT_SIZE_QUE4,
	.gmac_queue_config[4].nRxDedicatedBuffers	= TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE4,
	.gmac_queue_config[4].nRxAddlBuffCount	= TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE4,
	.gmac_queue_config[4].nRxBuffCntThres	= TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE4,
	.gmac_queue_config[4].nRxBuffAllocCnt	= TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE4,
</#if>

<#if (TCPIP_GMAC_QUEUE_5)?has_content>
	/** QUEUE 5 Intialization**/
	<#if (TCPIP_GMAC_QUEUE_5)  == true>
	<#if (TCPIP_GMAC_TX_EN_QUE5)  == true>
	.gmac_queue_config[5].queueTxEnable	= true,
	<#else>	
	.gmac_queue_config[5].queueTxEnable	= false,
	</#if>	
	<#if (TCPIP_GMAC_RX_EN_QUE5)  == true>
	.gmac_queue_config[5].queueRxEnable	= true,
	<#else>	
	.gmac_queue_config[5].queueRxEnable	= false,
	</#if>	
	<#else>		
	.gmac_queue_config[5].queueTxEnable	= false,
	.gmac_queue_config[5].queueRxEnable	= false,
	</#if>
	.gmac_queue_config[5].nRxDescCnt	= TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE5,
	.gmac_queue_config[5].nTxDescCnt	= TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE5,
	.gmac_queue_config[5].rxBufferSize	= TCPIP_GMAC_RX_BUFF_SIZE_QUE5,
	.gmac_queue_config[5].txMaxPktSize	= TCPIP_GMAC_MAX_TX_PKT_SIZE_QUE5,
	.gmac_queue_config[5].nRxDedicatedBuffers	= TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE5,
	.gmac_queue_config[5].nRxAddlBuffCount	= TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE5,
	.gmac_queue_config[5].nRxBuffCntThres	= TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE5,
	.gmac_queue_config[5].nRxBuffAllocCnt	= TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE5,
</#if>
	.ethFlags               = TCPIP_GMAC_ETH_OPEN_FLAGS,	
	.linkInitDelay          = TCPIP_INTMAC_PHY_LINK_INIT_DELAY,
    .ethModuleId            = TCPIP_INTMAC_MODULE_ID,
<#if (drvExtPhyLan9303.TCPIP_EMAC_PHY_TYPE)?has_content && (drvExtPhyLan9303.TCPIP_EMAC_PHY_TYPE) == "SMSC_LAN9303">
    .pPhyBase               = &DRV_ETHPHY_OBJECT_BASE_smsc9303,	
<#elseif (drvExtPhyKsz8863.TCPIP_EMAC_PHY_TYPE)?has_content && (drvExtPhyKsz8863.TCPIP_EMAC_PHY_TYPE) == "KSZ8863">
    .pPhyBase               = &DRV_ETHPHY_OBJECT_BASE_ksz8863,
<#else>
    .pPhyBase               = &DRV_ETHPHY_OBJECT_BASE_Default,
</#if>
    .pPhyInit               = &tcpipPhyInitData_${DRV_INTMAC_PHY_TYPE},
	.checksumOffloadRx      = DRV_GMAC_RX_CHKSM_OFFLOAD,
    .checksumOffloadTx      = DRV_GMAC_TX_CHKSM_OFFLOAD,
    .macTxPrioNum           = TCPIP_GMAC_TX_PRIO_COUNT,
    .macRxPrioNum           = TCPIP_GMAC_RX_PRIO_COUNT,
<#if (TCPIP_INTMAC_DEVICE)?has_content>  
<#if (((TCPIP_INTMAC_DEVICE) == "SAME7x_V7x") || ((TCPIP_INTMAC_DEVICE) == "SAMA5D2") || ((TCPIP_INTMAC_DEVICE) == "SAMRH71"))> 
	.pRxQueFiltInit			= &DRV_GMAC_Rx_Filt_Init,
</#if>
</#if>
};


