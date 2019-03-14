<#--
/*******************************************************************************
  TCPIP MAC Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_extphy_ksz8091.h.ftl

  Summary:
    TCPIP MAC Freemarker Template File

  Description:

*******************************************************************************/
-->

<#----------------------------------------------------------------------------
 Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.

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


#define TCPIP_INTMAC_PHY_CONFIG_FLAGS               \
<#if TCPIP_INTMAC_PHY_CONFIG_RMII>
                                                    DRV_ETHPHY_CFG_RMII | \
</#if>
<#if TCPIP_INTMAC_PHY_CONFIG_ALTERNATE>
                                                    DRV_ETHPHY_CFG_ALTERNATE | \
</#if>
<#if TCPIP_INTMAC_PHY_CONFIG_AUTO>
                                                    DRV_ETHPHY_CFG_AUTO | \
</#if>
                                                    0

#define TCPIP_INTMAC_PHY_LINK_INIT_DELAY            ${TCPIP_INTMAC_PHY_LINK_INIT_DELAY}
#define TCPIP_INTMAC_PHY_ADDRESS                    ${TCPIP_INTMAC_PHY_ADDRESS}
#define DRV_ETHPHY_INSTANCES_NUMBER                 ${DRV_ETHPHY_INSTANCES_NUMBER}
#define DRV_ETHPHY_CLIENTS_NUMBER                   ${DRV_ETHPHY_CLIENTS_NUMBER}
#define DRV_ETHPHY_INDEX                            ${DRV_ETHPHY_INDEX}
#define DRV_ETHPHY_PERIPHERAL_ID                    ${DRV_ETHPHY_PERIPHERAL_ID}
#define DRV_ETHPHY_NEG_INIT_TMO                     ${DRV_ETHPHY_NEG_INIT_TMO}
#define DRV_ETHPHY_NEG_DONE_TMO                     ${DRV_ETHPHY_NEG_DONE_TMO}
#define DRV_ETHPHY_RESET_CLR_TMO                    ${DRV_ETHPHY_RESET_CLR_TMO}
<#if drvMiim.DRV_MIIM_USE_DRIVER?has_content && drvMiim.DRV_MIIM_USE_DRIVER == true >
#define DRV_ETHPHY_USE_DRV_MIIM                     true
<#else>
#define DRV_ETHPHY_USE_DRV_MIIM                     false
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
