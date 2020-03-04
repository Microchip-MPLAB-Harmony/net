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

<#----------------------------------------------------------------------------
Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.

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
	<#lt>#define DRV_ENCX24J600_SPI_CS_IDX${idx}		${chipSelectPin?eval}

</#list>
</#if>


