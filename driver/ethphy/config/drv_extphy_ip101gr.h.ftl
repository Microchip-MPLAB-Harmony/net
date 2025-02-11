<#--
/*******************************************************************************
  TCPIP MAC Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_extphy_ip101gr.h.ftl

  Summary:
    TCPIP MAC Freemarker Template File

  Description:

*******************************************************************************/
-->

<#--
Copyright (C) 2020-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
<#if TCPIP_INTMAC_PHY_CONFIG_RMII>              
                                                    | DRV_ETHPHY_CFG_RMII \
</#if>
<#if TCPIP_INTMAC_PHY_CONFIG_ALTERNATE?has_content && TCPIP_INTMAC_PHY_CONFIG_ALTERNATE == true>
                                                    | DRV_ETHPHY_CFG_ALTERNATE \
</#if>
<#if TCPIP_INTMAC_PHY_CONFIG_AUTO?has_content && TCPIP_INTMAC_PHY_CONFIG_AUTO == true>
                                                    | DRV_ETHPHY_CFG_AUTO \
</#if>
                                                    )
                                                    
#define DRV_${emac_phy_type}_PHY_LINK_INIT_DELAY            ${TCPIP_INTMAC_PHY_LINK_INIT_DELAY}
#define DRV_${emac_phy_type}_PHY_ADDRESS                    ${TCPIP_INTMAC_PHY_ADDRESS}
#define DRV_${emac_phy_type}_PHY_PERIPHERAL_ID              ${DRV_ETHPHY_PERIPHERAL_ID}
#define DRV_ETHPHY_${emac_phy_type}_NEG_INIT_TMO            ${DRV_ETHPHY_NEG_INIT_TMO}
#define DRV_ETHPHY_${emac_phy_type}_NEG_DONE_TMO            ${DRV_ETHPHY_NEG_DONE_TMO}
#define DRV_ETHPHY_${emac_phy_type}_RESET_CLR_TMO           ${DRV_ETHPHY_RESET_CLR_TMO}

<#--
/*******************************************************************************
 End of File
*/
-->
