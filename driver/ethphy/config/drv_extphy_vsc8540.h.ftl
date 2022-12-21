<#--
/*******************************************************************************
  TCPIP MAC Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_extphy_vsc8540.h.ftl

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
#define DRV_${emac_phy_type}_PHY_CONFIG_FLAGS               ( 0 \
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


<#if TCPIP_INTMAC_PHY_CONFIG_MANAGED_MODE>
#define TCPIP_INTMAC_PHY_CONFIG_MANAGED_MODE 
</#if>

<#if TCPIP_INTMAC_PHY_CONFIG_RX_CLK_DELAY == "0.2ns">
#define TCPIP_INTMAC_PHY_CONFIG_RX_CLK_DELAY             0b000
</#if>
<#if TCPIP_INTMAC_PHY_CONFIG_RX_CLK_DELAY == "0.8ns">
#define TCPIP_INTMAC_PHY_CONFIG_RX_CLK_DELAY             0b001
</#if>
<#if TCPIP_INTMAC_PHY_CONFIG_RX_CLK_DELAY == "1.1ns">
#define TCPIP_INTMAC_PHY_CONFIG_RX_CLK_DELAY             0b010
</#if>
<#if TCPIP_INTMAC_PHY_CONFIG_RX_CLK_DELAY == "1.7ns">
#define TCPIP_INTMAC_PHY_CONFIG_RX_CLK_DELAY             0b011
</#if>
<#if TCPIP_INTMAC_PHY_CONFIG_RX_CLK_DELAY == "2.0ns">
#define TCPIP_INTMAC_PHY_CONFIG_RX_CLK_DELAY             0b100
</#if>
<#if TCPIP_INTMAC_PHY_CONFIG_RX_CLK_DELAY == "2.3ns">
#define TCPIP_INTMAC_PHY_CONFIG_RX_CLK_DELAY             0b101
</#if>
<#if TCPIP_INTMAC_PHY_CONFIG_RX_CLK_DELAY == "2.6ns">
#define TCPIP_INTMAC_PHY_CONFIG_RX_CLK_DELAY             0b110
</#if>
<#if TCPIP_INTMAC_PHY_CONFIG_RX_CLK_DELAY == "3.4ns">
#define TCPIP_INTMAC_PHY_CONFIG_RX_CLK_DELAY             0b111
</#if>

<#if TCPIP_INTMAC_PHY_CONFIG_TX_CLK_DELAY == "0.2ns">
#define TCPIP_INTMAC_PHY_CONFIG_TX_CLK_DELAY             0b000
</#if>
<#if TCPIP_INTMAC_PHY_CONFIG_TX_CLK_DELAY == "0.8ns">
#define TCPIP_INTMAC_PHY_CONFIG_TX_CLK_DELAY             0b001
</#if>
<#if TCPIP_INTMAC_PHY_CONFIG_TX_CLK_DELAY == "1.1ns">
#define TCPIP_INTMAC_PHY_CONFIG_TX_CLK_DELAY             0b010
</#if>
<#if TCPIP_INTMAC_PHY_CONFIG_TX_CLK_DELAY == "1.7ns">
#define TCPIP_INTMAC_PHY_CONFIG_TX_CLK_DELAY             0b011
</#if>
<#if TCPIP_INTMAC_PHY_CONFIG_TX_CLK_DELAY == "2.0ns">
#define TCPIP_INTMAC_PHY_CONFIG_TX_CLK_DELAY             0b100
</#if>
<#if TCPIP_INTMAC_PHY_CONFIG_TX_CLK_DELAY == "2.3ns">
#define TCPIP_INTMAC_PHY_CONFIG_TX_CLK_DELAY             0b101
</#if>
<#if TCPIP_INTMAC_PHY_CONFIG_TX_CLK_DELAY == "2.6ns">
#define TCPIP_INTMAC_PHY_CONFIG_TX_CLK_DELAY             0b110
</#if>
<#if TCPIP_INTMAC_PHY_CONFIG_TX_CLK_DELAY == "3.4ns">
#define TCPIP_INTMAC_PHY_CONFIG_TX_CLK_DELAY             0b111
</#if>


<#if TCPIP_INTMAC_PHY_CONFIG_CLKOUT_FREQ == "25MHz">
#define TCPIP_INTMAC_PHY_CONFIG_CLKOUT_FREQ             0b00
#define TCPIP_INTMAC_PHY_CONFIG_CLKOUT_FREQ_Hz          25000000
</#if>
<#if TCPIP_INTMAC_PHY_CONFIG_CLKOUT_FREQ == "50MHz">
#define TCPIP_INTMAC_PHY_CONFIG_CLKOUT_FREQ             0b01
#define TCPIP_INTMAC_PHY_CONFIG_CLKOUT_FREQ_Hz          50000000
</#if>
<#if TCPIP_INTMAC_PHY_CONFIG_CLKOUT_FREQ == "125MHz">
#define TCPIP_INTMAC_PHY_CONFIG_CLKOUT_FREQ             0b10
#define TCPIP_INTMAC_PHY_CONFIG_CLKOUT_FREQ_Hz          125000000
</#if>


#define TCPIP_INTMAC_PHY_CONFIG_CLKOUT_EN               ${TCPIP_INTMAC_PHY_CONFIG_CLKOUT_EN?then(1,0)}

#define TCPIP_INTMAC_PHY_CONFIG_AUTONEG_EN              ${TCPIP_INTMAC_PHY_CONFIG_AUTONEG_EN?then(1,0)}
#define TCPIP_INTMAC_PHY_CONFIG_FORCE_SPD_SEL           ${TCPIP_INTMAC_PHY_CONFIG_FORCE_SPD_SEL?then(1,0)}
#define TCPIP_INTMAC_PHY_CONFIG_DUPLEX_SEL              ${TCPIP_INTMAC_PHY_CONFIG_DUPLEX_SEL?then(1,0)}




#define DRV_${emac_phy_type}_PHY_LINK_INIT_DELAY                     ${TCPIP_INTMAC_PHY_LINK_INIT_DELAY}
#define DRV_${emac_phy_type}_PHY_ADDRESS                             ${TCPIP_INTMAC_PHY_ADDRESS}
#define DRV_${emac_phy_type}_PHY_PERIPHERAL_ID                       ${DRV_ETHPHY_PERIPHERAL_ID}
#define DRV_ETHPHY_${emac_phy_type}_NEG_INIT_TMO                     ${DRV_ETHPHY_NEG_INIT_TMO}
#define DRV_ETHPHY_${emac_phy_type}_NEG_DONE_TMO                     ${DRV_ETHPHY_NEG_DONE_TMO}
#define DRV_ETHPHY_${emac_phy_type}_RESET_CLR_TMO                    ${DRV_ETHPHY_RESET_CLR_TMO}

<#--
/*******************************************************************************
 End of File
*/
-->

