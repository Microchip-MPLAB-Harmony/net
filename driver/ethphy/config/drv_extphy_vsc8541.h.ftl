<#--
/*******************************************************************************
  TCPIP MAC Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_extphy_vsc8541.h.ftl

  Summary:
    TCPIP MAC Freemarker Template File

  Description:

*******************************************************************************/
-->

<#--
Copyright (C) 2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
<#list 0..(__INSTANCE_COUNT?number-1) as idx>
	<#assign emac_phy_type = "drvExtPhyVsc8541_"+idx+".TCPIP_EMAC_PHY_TYPE_IDX" + idx>
	<#assign emac_phy_rgmii = "drvExtPhyVsc8541_"+idx+".TCPIP_INTMAC_PHY_CONFIG_RGMII_IDX" + idx>
	<#assign emac_phy_rmii = "drvExtPhyVsc8541_"+idx+".TCPIP_INTMAC_PHY_CONFIG_RMII_IDX" + idx>
	<#assign emac_phy_auto = "drvExtPhyVsc8541_"+idx+".TCPIP_INTMAC_PHY_CONFIG_AUTO_IDX" + idx>
	<#assign link_init_delay = "drvExtPhyVsc8541_"+idx+".TCPIP_INTMAC_PHY_LINK_INIT_DELAY_IDX" + idx>
	<#assign phy_address = "drvExtPhyVsc8541_"+idx+".TCPIP_INTMAC_PHY_ADDRESS_IDX" + idx>
	<#assign peripheral_id = "drvExtPhyVsc8541_"+idx+".DRV_ETHPHY_PERIPHERAL_ID_IDX" + idx>
	<#assign net_init_tmo = "drvExtPhyVsc8541_"+idx+".DRV_ETHPHY_NEG_INIT_TMO_IDX" + idx>
	<#assign net_done_tmo = "drvExtPhyVsc8541_"+idx+".DRV_ETHPHY_NEG_DONE_TMO_IDX" + idx>
	<#assign reset_clr_tmo = "drvExtPhyVsc8541_"+idx+".DRV_ETHPHY_RESET_CLR_TMO_IDX" + idx>
	<#lt>#define DRV_${emac_phy_type?eval}_PHY_CONFIG_FLAGS_IDX${idx}       ( 0 \
	<#if (emac_phy_rgmii?eval)>              
	<#lt>													| DRV_ETHPHY_CFG_RGMII \
	<#elseif (emac_phy_rmii?eval)>              
	<#lt>													| DRV_ETHPHY_CFG_RMII \
	<#elseif (emac_phy_mii?eval)>              
	<#lt>													| DRV_ETHPHY_CFG_MII \
	</#if>
	<#if (emac_phy_alternate?eval)?has_content && (emac_phy_alternate?eval) == true>
	<#lt>													| DRV_ETHPHY_CFG_ALTERNATE \
	<#else>
	<#lt>													| DRV_ETHPHY_CFG_DEFAULT \
	</#if>															
	<#if (emac_phy_auto?eval)?has_content && (emac_phy_auto?eval) == true>
	<#lt>													| DRV_ETHPHY_CFG_AUTO \
	</#if>
	<#lt>													)
	<#lt>#define DRV_${emac_phy_type?eval}_PHY_LINK_INIT_DELAY_IDX${idx}            ${link_init_delay?eval}
	<#lt>#define DRV_${emac_phy_type?eval}_PHY_ADDRESS_IDX${idx}                    ${phy_address?eval}
	<#lt>#define DRV_${emac_phy_type?eval}_PHY_PERIPHERAL_ID_IDX${idx}              ${peripheral_id?eval}
	<#lt>#define DRV_ETHPHY_${emac_phy_type?eval}_NEG_INIT_TMO_IDX${idx}            ${net_init_tmo?eval}
	<#lt>#define DRV_ETHPHY_${emac_phy_type?eval}_NEG_DONE_TMO_IDX${idx}            ${net_done_tmo?eval}
	<#lt>#define DRV_ETHPHY_${emac_phy_type?eval}_RESET_CLR_TMO_IDX${idx}           ${reset_clr_tmo?eval}
</#list>

<#--
/*******************************************************************************
 End of File
*/
-->
