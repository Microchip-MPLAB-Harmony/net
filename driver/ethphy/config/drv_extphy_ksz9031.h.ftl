<#--
/*******************************************************************************
  TCPIP MAC Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_extphy_ksz9031.h.ftl

  Summary:
    TCPIP MAC Freemarker Template File

  Description:

*******************************************************************************/
-->

<#----------------------------------------------------------------------------
 Copyright (C) 2022 Microchip Technology Inc. and its subsidiaries.

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
----------------------------------------------------------------------------->
<#assign emac_phy_type = TCPIP_EMAC_PHY_TYPE>
#define DRV_${emac_phy_type}_PHY_CONFIG_FLAGS       ( 0 \
<#if TCPIP_INTMAC_PHY_CONFIG_GMII?has_content && TCPIP_INTMAC_PHY_CONFIG_GMII == true>
                                                    | DRV_ETHPHY_CFG_GMII \
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
