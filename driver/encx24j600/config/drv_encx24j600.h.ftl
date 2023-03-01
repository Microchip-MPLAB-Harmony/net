<#--
/*******************************************************************************
  ENCX24J600 Driver Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_ecnX24j600.h.ftl

  Summary:
    ENCX24J600 Driver Freemarker Template File

  Description:

*******************************************************************************/
-->

<#--
Copyright (C) 2020-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

<#if (DRV_ENCX24J600_USE_DRIVER)?has_content && (DRV_ENCX24J600_USE_DRIVER == true)>
/*** ENCX24J600 Driver Configuration ***/
/*** Driver Compilation and static configuration options. ***/
#define TCPIP_IF_ENCX24J600

#define DRV_ENCX24J600_INSTANCES_NUMBER ${__INSTANCE_COUNT}
<#list 0..(__INSTANCE_COUNT?number-1) as idx>
	<#assign clientInstnNum = "drvExtMacEncx24j600_"+idx+".DRV_ENCX24J600_CLIENT_INSTANCES_IDX"+idx>
	<#lt>#define DRV_ENCX24J600_CLIENT_INSTANCES_IDX${idx}		${clientInstnNum?eval}
	<#assign txDescNum = "drvExtMacEncx24j600_"+idx+".DRV_ENCX24J600_MAC_TX_DESCRIPTORS_IDX" + idx>
	<#lt>#define DRV_ENCX24J600_MAC_TX_DESCRIPTORS_IDX${idx}		${txDescNum?eval}
	<#assign rxDescNum = "drvExtMacEncx24j600_"+idx+".DRV_ENCX24J600_MAC_RX_DESCRIPTORS_IDX" + idx>
	<#lt>#define DRV_ENCX24J600_MAC_RX_DESCRIPTORS_IDX${idx}		${rxDescNum?eval}
	<#assign rxDescBuffSize= "drvExtMacEncx24j600_"+idx+".DRV_ENCX24J600_MAX_RX_BUFFER_IDX" + idx>
	<#lt>#define DRV_ENCX24J600_MAX_RX_BUFFER_IDX${idx}		${rxDescBuffSize?eval}
	<#assign rxBuffSize= "drvExtMacEncx24j600_"+idx+".DRV_ENCX24J600_RX_BUFFER_SIZE_IDX" + idx>
	<#lt>#define DRV_ENCX24J600_RX_BUFFER_SIZE_IDX${idx}		${rxBuffSize?eval}
	<#assign frameSize= "drvExtMacEncx24j600_"+idx+".DRV_ENCX24J600_MAX_FRAME_SIZE_IDX" + idx>
	<#lt>#define DRV_ENCX24J600_MAX_FRAME_SIZE_IDX${idx}		${frameSize?eval}
	
	<#assign ethType = "drvExtMacEncx24j600_"+idx+".DRV_ENCX24J600_ETHERNET_TYPE_IDX" + idx>
	<#if ethType?eval == "100base-T">
		<#lt>#define DRV_ENCX24J600_ETHERNET_TYPE_IDX${idx} TCPIP_ETH_OPEN_100
	<#elseif ethType?eval == "10base-T">	
		<#lt>#define DRV_ENCX24J600_ETHERNET_TYPE_IDX${idx} TCPIP_ETH_OPEN_10
	<#else>
		<#lt>#define DRV_ENCX24J600_ETHERNET_TYPE_IDX${idx} TCPIP_ETH_OPEN_AUTO
	</#if>
	
	<#assign duplexMode = "drvExtMacEncx24j600_"+idx+".DRV_ENCX24J600_DUPLEX_MODE_IDX" + idx>
	<#if duplexMode?eval == "Half Duplex">
		<#lt>#define DRV_ENCX24J600_DUPLEX_MODE_IDX${idx} TCPIP_ETH_OPEN_HDUPLEX
	<#elseif duplexMode?eval == "Full Duplex">	
		<#lt>#define DRV_ENCX24J600_DUPLEX_MODE_IDX${idx} TCPIP_ETH_OPEN_FDUPLEX
	<#else>
		<#lt>#define DRV_ENCX24J600_DUPLEX_MODE_IDX${idx} TCPIP_ETH_OPEN_AUTO
	</#if>
	<#assign chipSelectPin= "drvExtMacEncx24j600_"+idx+".DRV_ENCX24J600_SPI_CS_IDX" + idx>
	<#if (chipSelectPin?eval == "SYS_PORT_PIN_NONE")>
		<#lt>#error Configure SPI Chip-Select pin for interfacing ENCX24J600 (in ENCX24J600 Driver)
	<#else>
		<#lt>#define DRV_ENCX24J600_SPI_CS_IDX${idx}		${chipSelectPin?eval}
	</#if>

</#list>
</#if>


