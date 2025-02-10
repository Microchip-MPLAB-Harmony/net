<#--
Copyright (C) 2021-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
    .linkInitDelay                     = DRV_${DRV_MAC_PHY_TYPE}_PHY_LINK_INIT_DELAY,
<#if (drvExtPhyLan9303.TCPIP_EMAC_PHY_TYPE)?has_content && (drvExtPhyLan9303.TCPIP_EMAC_PHY_TYPE) == "LAN9303">
    .pPhyBase               = &DRV_ETHPHY_OBJECT_BASE_LAN9303,
<#elseif (drvExtPhyKsz8863.TCPIP_EMAC_PHY_TYPE)?has_content && (drvExtPhyKsz8863.TCPIP_EMAC_PHY_TYPE) == "KSZ8863">
    .pPhyBase               = &DRV_ETHPHY_OBJECT_BASE_ksz8863,
<#elseif (drvExtPhyLan9354.TCPIP_EMAC_PHY_TYPE)?has_content && (drvExtPhyLan9354.TCPIP_EMAC_PHY_TYPE) == "LAN9354">
    .pPhyBase               = &DRV_ETHPHY_OBJECT_BASE_lan9354,
<#elseif (drvExtPhyDummy.TCPIP_EMAC_PHY_TYPE)?has_content && (drvExtPhyDummy.TCPIP_EMAC_PHY_TYPE) == "Dummy">
    .pPhyBase               = &DRV_ETHPHY_OBJECT_BASE_Dummy,
<#else>
    .pPhyBase               = &DRV_ETHPHY_OBJECT_BASE_Default,
</#if>
    .pPhyInit               = &tcpipPhyInitData_${DRV_MAC_PHY_TYPE},
};
