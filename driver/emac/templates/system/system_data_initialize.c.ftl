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
/*** EMAC0 Initialization Data ***/
const TCPIP_MODULE_MAC_SAM9X60_CONFIG tcpip${MAC_PERIPHERAL_INSTANCE}InitData =
{
    .macIntSrc                         = DRV_${MAC_PERIPHERAL_INSTANCE}_INTERRUPT_SOURCE,
    .macRxFilters                      = DRV_${MAC_PERIPHERAL_INSTANCE}_RX_FILTERS,
    /** QUEUE Intialization **/
    .rxDeviceMaxDescriptors            = DRV_${MAC_PERIPHERAL_INSTANCE}_RX_DEVICE_MAX_DESCRIPTORS0,
    .nRxDescCnt                        = DRV_${MAC_PERIPHERAL_INSTANCE}_RX_DESCRIPTORS_COUNT_QUE0,
    .nTxDescCnt                        = DRV_${MAC_PERIPHERAL_INSTANCE}_TX_DESCRIPTORS_COUNT_QUE0,
    .rxBufferSize                      = DRV_${MAC_PERIPHERAL_INSTANCE}_RX_BUFF_SIZE_QUE0,
    .txBufferSize                      = DRV_${MAC_PERIPHERAL_INSTANCE}_TX_BUFF_SIZE_QUE0,
    .nRxStaticBufferCnt                = DRV_${MAC_PERIPHERAL_INSTANCE}_RX_BUFF_STATIC_COUNT_QUE0,
    .nRxBuffCntThres                   = DRV_${MAC_PERIPHERAL_INSTANCE}_RX_BUFF_COUNT_THRESHOLD_QUE0,
    .nRxBuffAllocCnt                   = DRV_${MAC_PERIPHERAL_INSTANCE}_RX_BUFF_ALLOC_COUNT_QUE0,
    .ethModuleId                       = DRV_${MAC_PERIPHERAL_INSTANCE}_BASE_ADDRESS,
    .ethFlags                          = DRV_${MAC_PERIPHERAL_INSTANCE}_ETH_OPEN_FLAGS,
    .linkInitDelay                     = TCPIP_INTMAC_PHY_LINK_INIT_DELAY,
<#if (drvExtPhyLan9303.TCPIP_EMAC_PHY_TYPE)?has_content && (drvExtPhyLan9303.TCPIP_EMAC_PHY_TYPE) == "SMSC_LAN9303">
    .pPhyBase               = &DRV_ETHPHY_OBJECT_BASE_smsc9303,
<#elseif (drvExtPhyKsz8863.TCPIP_EMAC_PHY_TYPE)?has_content && (drvExtPhyKsz8863.TCPIP_EMAC_PHY_TYPE) == "KSZ8863">
    .pPhyBase           = &DRV_ETHPHY_OBJECT_BASE_ksz8863,
<#else>
    .pPhyBase               = &DRV_ETHPHY_OBJECT_BASE_Default,
</#if>
    .pPhyInit               = &tcpipPhyInitData_${DRV_MAC_PHY_TYPE},
};
