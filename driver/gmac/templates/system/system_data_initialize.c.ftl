<#--
Copyright (C) 2021-2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
-->
<#lt><#if (TCPIP_INTMAC_DEVICE)?has_content>  
    <#lt><#if (((TCPIP_INTMAC_DEVICE) == "SAME7x_V7x") || ((TCPIP_INTMAC_DEVICE) == "SAMA5D2") || ((TCPIP_INTMAC_DEVICE) == "SAMRH71") || ((TCPIP_INTMAC_DEVICE) == "PIC32CZCA8") || ((TCPIP_INTMAC_DEVICE) == "PIC32CZCA9") || ((TCPIP_INTMAC_DEVICE) == "SAMA7G") || ((TCPIP_INTMAC_DEVICE) == "SAMA7D6") || ((TCPIP_INTMAC_DEVICE) == "SAM9X7"))> 
        <#lt><#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_QUE_FILTER_EN"]?has_content>   
            <#lt><#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_QUE_FILTER_EN"] == true>  
                
                <#lt>DRV_GMAC_TYPE1_FILTER_INIT  ${GMAC_PERIPHERAL_INSTANCE?lower_case}Type1FiltInit[TCPIP_${GMAC_PERIPHERAL_INSTANCE}_SCREEN1_COUNT_QUE]=
                <#lt>{
                    <#lt><#assign GMAC_SCREEN1_COUNT_QUE =  "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_SCREEN1_COUNT_QUE">
                    <#lt><#if .vars[GMAC_SCREEN1_COUNT_QUE] gte 1>      
                        <#lt><#list 0 ..(.vars[GMAC_SCREEN1_COUNT_QUE] -1) as i >
                            <#lt>   {   /** SCREEN1 QUEUE ${i} **/
                                <#lt>       .queueIndex = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_SCREEN1_QUE_IDX${i},
                                <#lt>       .dstcEnable = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN1_DSTCEN_IDX${i},
                                <#lt><#assign dstcen = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN1_DSTCEN_IDX" + i?string>
                                <#lt><#if .vars[dstcen] == true>
                                    <#lt>       .dstcValue = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN1_DSTC_IDX${i},
                                <#lt></#if>  
                                <#lt>       .udpEnable = TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN1_UDPEN_IDX${i},
                                <#lt><#assign udpen = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN1_UDPEN_IDX" + i?string>
                                <#lt><#if .vars[udpen] == true>
                                    <#lt>       .udpPortNum = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN1_UDPPORTNUM_IDX${i},
                                <#lt></#if>     
                            <#lt>   },
                        <#lt></#list>
                    <#lt></#if>
                <#lt>};         

                <#lt>DRV_GMAC_TYPE2_FILTER_INIT  ${GMAC_PERIPHERAL_INSTANCE?lower_case}Type2FiltInit[TCPIP_${GMAC_PERIPHERAL_INSTANCE}_SCREEN2_COUNT_QUE]=
                <#lt>{
                    <#lt><#assign GMAC_SCREEN2_COUNT_QUE =  "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_SCREEN2_COUNT_QUE">
                    <#lt><#if .vars[GMAC_SCREEN2_COUNT_QUE] gte 1>      
                        <#lt><#list 0 ..(.vars[GMAC_SCREEN2_COUNT_QUE] -1) as i >
                            <#lt>   {   /** SCREEN2 QUEUE ${i} **/
                                <#lt>       .queueIndex = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_SCREEN2_QUE_IDX${i},
                                <#lt>       .vlanPrioEnable = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_VLANPRIOEN_IDX${i},                         
                                <#lt><#assign vlanprioen = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN2_VLANPRIOEN_IDX" + i?string>                                
                                <#lt><#if .vars[vlanprioen] == true>
                                    <#lt>       .vlanPrio = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_VLANPRIO_IDX${i},
                                <#lt></#if>                         
                                <#lt>       .ethTypeEnable = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_ETHTYPEEN_IDX${i},
                                <#lt><#assign ethtypeen = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN2_ETHTYPEEN_IDX" + i?string>
                                <#lt><#if .vars[ethtypeen] == true>
                                    <#lt>       .ethType = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_ETHTYPE_IDX${i},
                                <#lt></#if>                                 
                                <#lt>       .compAEnable = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPAEN_IDX${i},
                                <#lt><#assign compaen = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN2_COMPAEN_IDX" + i?string>
                                <#lt><#if .vars[compaen] == true>
                                    <#lt>       .compAValue = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPA_IDX${i},
                                    <#lt>       .compAMask = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPAMASK_IDX${i},
                                    <#lt>       .compAOffset = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPAOFFST_IDX${i},
                                    <#lt>       .compAOffsetStart = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPAOFFSTSTRT_IDX${i},
                                <#lt></#if>                                 
                                <#lt>       .compBEnable = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPBEN_IDX${i},
                                <#lt><#assign compben = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN2_COMPBEN_IDX" + i?string>
                                <#lt><#if .vars[compben] == true>
                                    <#lt>       .compBValue = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPB_IDX${i},
                                    <#lt>       .compBMask = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPBMASK_IDX${i},
                                    <#lt>       .compBOffset = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPBOFFST_IDX${i},
                                    <#lt>       .compBOffsetStart = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPBOFFSTSTRT_IDX${i},
                                <#lt></#if>                             
                                <#lt>       .compCEnable = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPCEN_IDX${i},
                                <#lt><#assign compcen = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN2_COMPCEN_IDX" + i?string>
                                <#lt><#if .vars[compcen] == true>   
                                    <#lt>       .compCValue = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPC_IDX${i},
                                    <#lt>       .compCMask = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPCMASK_IDX${i},
                                    <#lt>       .compCOffset = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPCOFFST_IDX${i},
                                    <#lt>       .compCOffsetStart = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPCOFFSTSTRT_IDX${i},
                                <#lt></#if> 
                            <#lt>   },                          
                        <#lt></#list>
                    <#lt></#if>
                <#lt>};

                <#lt>const DRV_GMAC_RXQUE_FILTER_INIT DRV_${GMAC_PERIPHERAL_INSTANCE}_Rx_Filt_Init =
                <#lt>{ 
                    <#lt>   .type1FiltCount = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_SCREEN1_COUNT_QUE,          
                    <#lt>   .type2FiltCount = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_SCREEN2_COUNT_QUE,
                    <#lt><#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_QUE_FILTER_EN"]?has_content>   
                        <#lt><#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_QUE_FILTER_EN"] == true>  
                            <#lt>   .type1FiltInit = ${GMAC_PERIPHERAL_INSTANCE?lower_case}Type1FiltInit,
                            <#lt>   .type2FiltInit = ${GMAC_PERIPHERAL_INSTANCE?lower_case}Type2FiltInit,
                        <#lt></#if>
                    <#lt></#if>
                <#lt>}; 
                
            <#lt></#if>         
        <#lt></#if>
    <#lt></#if>
<#lt></#if>

uint8_t txPrioNumToQueIndx${GMAC_PERIPHERAL_INSTANCE?lower_case?cap_first} [DRV_${GMAC_PERIPHERAL_INSTANCE}_NUMBER_OF_QUEUES];
uint8_t rxPrioNumToQueIndx${GMAC_PERIPHERAL_INSTANCE?lower_case?cap_first} [DRV_${GMAC_PERIPHERAL_INSTANCE}_NUMBER_OF_QUEUES];

/*** ${GMAC_PERIPHERAL_INSTANCE} Initialization Data ***/
TCPIP_MODULE_GMAC_QUEUE_CONFIG  ${GMAC_PERIPHERAL_INSTANCE?lower_case}_queue_config[DRV_${GMAC_PERIPHERAL_INSTANCE}_NUMBER_OF_QUEUES]=
{
    <#lt><#assign queCnt = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_MAX_MAC_QUE_CNT"?eval - 1 >
    <#lt><#list 0..queCnt as qIndx>
    <#lt><#assign MAC_QUEUE_ENABLE =  "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUEUE_" + qIndx?string >
    <#lt><#assign MAC_QUEUE_TX_ENABLE =  "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_TX_EN_QUE" + qIndx?string >
    <#lt><#assign MAC_QUEUE_RX_ENABLE =  "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_EN_QUE" + qIndx?string >
        <#lt><#if .vars[MAC_QUEUE_ENABLE]?has_content>          
            <#lt>   {   /** QUEUE ${qIndx} Initialization**/
                <#lt><#if .vars[ MAC_QUEUE_ENABLE ] == true>
                    <#lt><#if .vars[ MAC_QUEUE_TX_ENABLE ] == true>
                        <#lt>       .queueTxEnable = true,
                    <#lt><#else>
                        <#lt>       .queueTxEnable = false,
                    <#lt></#if>
                    <#lt><#if .vars[ MAC_QUEUE_RX_ENABLE ] == true>
                        <#lt>       .queueRxEnable = true,
                    <#lt><#else>
                        <#lt>       .queueRxEnable = false,
                    <#lt></#if> 
                <#lt><#else>
                    <#lt>       .queueTxEnable = false,
                    <#lt>       .queueRxEnable = false,
                <#lt></#if> 
                <#lt>       .nRxDescCnt    = ${.vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_DESCRIPTORS_COUNT_QUE" + qIndx?string]},
                <#lt>       .nTxDescCnt    = ${.vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_TX_DESCRIPTORS_COUNT_QUE" + qIndx?string]},
                <#lt>       .rxBufferSize  = ${.vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_BUFF_SIZE_QUE" + qIndx?string]},
                <#lt>       .txMaxPktSize  = ${.vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_MAX_TX_PKT_SIZE_QUE" + qIndx?string]},
                <#lt>       .nRxDedicatedBuffers   = ${.vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_DEDICATED_BUFFERS_QUE" + qIndx?string]},
                <#lt>       .nRxAddlBuffCount  = ${.vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_ADDL_BUFF_COUNT_QUE" + qIndx?string]},
                <#lt>       .nRxBuffCntThres   = ${.vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_BUFF_COUNT_THRESHOLD_QUE" + qIndx?string]},
                <#lt>       .nRxBuffAllocCnt   = ${.vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_BUFF_ALLOC_COUNT_QUE" + qIndx?string]},   
                <#lt>       .queueIntSrc       = ${.vars["DRV_${GMAC_PERIPHERAL_INSTANCE?string}_INT_SRC_QUE" + qIndx?string]},                               
            <#lt>   },
        <#lt></#if>
    <#lt></#list>
};

const TCPIP_MODULE_MAC_PIC32C_CONFIG tcpip${GMAC_PERIPHERAL_INSTANCE}InitData =
{ 
    <#lt>       .gmac_queue_config = ${GMAC_PERIPHERAL_INSTANCE?lower_case}_queue_config,
    <#lt>       .macQueNum = DRV_${GMAC_PERIPHERAL_INSTANCE}_NUMBER_OF_QUEUES, 
    <#lt>       .txPrioNumToQueIndx = txPrioNumToQueIndx${GMAC_PERIPHERAL_INSTANCE?lower_case?cap_first},
    <#lt>       .rxPrioNumToQueIndx = rxPrioNumToQueIndx${GMAC_PERIPHERAL_INSTANCE?lower_case?cap_first},
    <#lt>       .ethFlags               = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_ETH_OPEN_FLAGS,    
    <#lt>       .linkInitDelay          = DRV_${.vars["DRV_${GMAC_PERIPHERAL_INSTANCE?string}_PHY_TYPE"]}_PHY_LINK_INIT_DELAY,
    <#lt>       .ethModuleId            = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_MODULE_ID,
    <#lt><#if (drvExtPhyLan9303.TCPIP_EMAC_PHY_TYPE)?has_content && (drvExtPhyLan9303.TCPIP_EMAC_PHY_TYPE) == "LAN9303">
        <#lt>       .pPhyBase               = &DRV_ETHPHY_OBJECT_BASE_lan9303, 
    <#lt><#elseif (drvExtPhyKsz8863.TCPIP_EMAC_PHY_TYPE)?has_content && (drvExtPhyKsz8863.TCPIP_EMAC_PHY_TYPE) == "KSZ8863">
        <#lt>       .pPhyBase               = &DRV_ETHPHY_OBJECT_BASE_ksz8863,
    <#lt><#elseif (drvExtPhyLan9354.TCPIP_EMAC_PHY_TYPE)?has_content && (drvExtPhyLan9354.TCPIP_EMAC_PHY_TYPE) == "LAN9354">
        <#lt>       .pPhyBase               = &DRV_ETHPHY_OBJECT_BASE_lan9354,
    <#lt><#elseif (drvExtPhyDummy.TCPIP_EMAC_PHY_TYPE)?has_content && (drvExtPhyDummy.TCPIP_EMAC_PHY_TYPE) == "Dummy">
        <#lt>       .pPhyBase               = &DRV_ETHPHY_OBJECT_BASE_Dummy,
    <#lt><#else>
        <#lt>       .pPhyBase               = &DRV_ETHPHY_OBJECT_BASE_Default,
    <#lt></#if>
    <#lt>       .pPhyInit               = &tcpipPhyInitData_${.vars["DRV_${GMAC_PERIPHERAL_INSTANCE?string}_PHY_TYPE"]},
    <#lt><#if ((TCPIP_INTMAC_DEVICE) == "SAMA7G")|| (TCPIP_INTMAC_DEVICE == "SAM9X7")|| (TCPIP_INTMAC_DEVICE == "SAMA7D6")> 
        <#lt><#if .vars["DRV_${GMAC_PERIPHERAL_INSTANCE?string}_REF_CLK_SRC"]?has_content>  
            <#lt>       .macRefClkSrc           = ${.vars["DRV_${GMAC_PERIPHERAL_INSTANCE?string}_REF_CLK_SRC"]},
        <#lt></#if>
    <#lt></#if>
    <#lt>       .checksumOffloadRx      = DRV_${GMAC_PERIPHERAL_INSTANCE}_RX_CHKSM_OFFLOAD,
    <#lt>       .checksumOffloadTx      = DRV_${GMAC_PERIPHERAL_INSTANCE}_TX_CHKSM_OFFLOAD,
    <#lt>       .macTxPrioNum           = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_TX_PRIO_COUNT,
    <#lt>       .macRxPrioNum           = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_PRIO_COUNT,  
    <#lt>       .macRxFilt              = TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_FILTERS,
    <#lt><#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_QUE_FILTER_EN"]?has_content>   
        <#lt><#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_QUE_FILTER_EN"] == true> 
            <#lt>       .pRxQueFiltInit         = &DRV_${GMAC_PERIPHERAL_INSTANCE}_Rx_Filt_Init,
        <#lt></#if>
    <#lt></#if>
};

