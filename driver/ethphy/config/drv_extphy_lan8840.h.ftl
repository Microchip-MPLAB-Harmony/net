<#--
/*******************************************************************************
  TCPIP MAC Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_extphy_ksz9131.h.ftl

  Summary:
    TCPIP MAC Freemarker Template File

  Description:

*******************************************************************************/
-->

<#--
Copyright (C) 2024-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

<#assign emac_phy_type = TCPIP_EMAC_PHY_TYPE>

#define DRV_${emac_phy_type}_PHY_CONFIG_FLAGS       ( 0 \
<#if TCPIP_INTMAC_PHY_CONFIG_GMII?has_content && TCPIP_INTMAC_PHY_CONFIG_GMII == true>
                                                    | DRV_ETHPHY_CFG_GMII \
<#elseif TCPIP_INTMAC_PHY_CONFIG_RGMII?has_content && TCPIP_INTMAC_PHY_CONFIG_RGMII == true>            
                                                    | DRV_ETHPHY_CFG_RGMII \
<#elseif TCPIP_INTMAC_PHY_CONFIG_RMII?has_content && TCPIP_INTMAC_PHY_CONFIG_RMII == true>          
                                                    | DRV_ETHPHY_CFG_RMII \
<#else>
                                                    | DRV_ETHPHY_CFG_MII \
</#if>
<#if TCPIP_INTMAC_PHY_CONFIG_ALTERNATE?has_content && TCPIP_INTMAC_PHY_CONFIG_ALTERNATE == true>
                                                    | DRV_ETHPHY_CFG_ALTERNATE \
<#else>
                                                    | DRV_ETHPHY_CFG_DEFAULT \
</#if>
<#if TCPIP_INTMAC_PHY_CONFIG_AUTO?has_content && TCPIP_INTMAC_PHY_CONFIG_AUTO == true>
                                                    | DRV_ETHPHY_CFG_AUTO \
</#if>
                                                    )

#define DRV_${emac_phy_type}_PHY_LINK_INIT_DELAY        ${TCPIP_INTMAC_PHY_LINK_INIT_DELAY}
#define DRV_${emac_phy_type}_PHY_ADDRESS                ${TCPIP_INTMAC_PHY_ADDRESS}
#define DRV_${emac_phy_type}_PHY_PERIPHERAL_ID          ${DRV_ETHPHY_PERIPHERAL_ID}
#define DRV_ETHPHY_${emac_phy_type}_NEG_INIT_TMO        ${DRV_ETHPHY_NEG_INIT_TMO}
#define DRV_ETHPHY_${emac_phy_type}_NEG_DONE_TMO        ${DRV_ETHPHY_NEG_DONE_TMO}
#define DRV_ETHPHY_${emac_phy_type}_RESET_CLR_TMO       ${DRV_ETHPHY_RESET_CLR_TMO}

<#if DRV_ETHPHY_TX_CLK_SKEW_SET == true || DRV_ETHPHY_RX_CLK_SKEW_SET == true || DRV_ETHPHY_RX_CTL_SKEW_SET == true || DRV_ETHPHY_TX_CTL_SKEW_SET == true || 
DRV_ETHPHY_RXD3_SKEW_SET == true || DRV_ETHPHY_RXD2_SKEW_SET == true || DRV_ETHPHY_RXD1_SKEW_SET == true || DRV_ETHPHY_RXD0_SKEW_SET == true ||
DRV_ETHPHY_TXD3_SKEW_SET == true || DRV_ETHPHY_TXD2_SKEW_SET == true || DRV_ETHPHY_TXD1_SKEW_SET == true || DRV_ETHPHY_TXD0_SKEW_SET == true >
#define DRV_ETHPHY_${emac_phy_type}_SKEW_SETTING
</#if>
<#if DRV_ETHPHY_TX_CLK_SKEW_SET == true>
#define DRV_ETHPHY_${emac_phy_type}_TX_CLK_SKEW       0x${DRV_ETHPHY_TX_CLK_SKEW}
</#if>
<#if DRV_ETHPHY_RX_CLK_SKEW_SET == true>
#define DRV_ETHPHY_${emac_phy_type}_RX_CLK_SKEW       0x${DRV_ETHPHY_RX_CLK_SKEW}
</#if>
<#if DRV_ETHPHY_RX_CTL_SKEW_SET == true>
#define DRV_ETHPHY_${emac_phy_type}_RX_CTL_SKEW       0x${DRV_ETHPHY_RX_CTL_SKEW}
</#if>
<#if DRV_ETHPHY_TX_CTL_SKEW_SET == true>
#define DRV_ETHPHY_${emac_phy_type}_TX_CTL_SKEW       0x${DRV_ETHPHY_TX_CTL_SKEW}
</#if>
<#if DRV_ETHPHY_RXD3_SKEW_SET == true>
#define DRV_ETHPHY_${emac_phy_type}_RXD3_SKEW       0x${DRV_ETHPHY_RXD3_SKEW}
</#if>
<#if DRV_ETHPHY_RXD2_SKEW_SET == true>
#define DRV_ETHPHY_${emac_phy_type}_RXD2_SKEW       0x${DRV_ETHPHY_RXD2_SKEW}
</#if>
<#if DRV_ETHPHY_RXD1_SKEW_SET == true>
#define DRV_ETHPHY_${emac_phy_type}_RXD1_SKEW       0x${DRV_ETHPHY_RXD1_SKEW}
</#if>
<#if DRV_ETHPHY_RXD0_SKEW_SET == true>
#define DRV_ETHPHY_${emac_phy_type}_RXD0_SKEW       0x${DRV_ETHPHY_RXD0_SKEW}
</#if>
<#if DRV_ETHPHY_TXD3_SKEW_SET == true>
#define DRV_ETHPHY_${emac_phy_type}_TXD3_SKEW       0x${DRV_ETHPHY_TXD3_SKEW}
</#if>
<#if DRV_ETHPHY_TXD2_SKEW_SET == true>
#define DRV_ETHPHY_${emac_phy_type}_TXD2_SKEW       0x${DRV_ETHPHY_TXD2_SKEW}
</#if>
<#if DRV_ETHPHY_TXD1_SKEW_SET == true>
#define DRV_ETHPHY_${emac_phy_type}_TXD1_SKEW       0x${DRV_ETHPHY_TXD1_SKEW}
</#if>
<#if DRV_ETHPHY_TXD0_SKEW_SET == true>
#define DRV_ETHPHY_${emac_phy_type}_TXD0_SKEW       0x${DRV_ETHPHY_TXD0_SKEW}
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
