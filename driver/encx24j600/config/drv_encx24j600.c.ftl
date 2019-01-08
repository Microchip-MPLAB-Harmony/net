<#--
/*******************************************************************************
  ENCX24J600 Driver Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600.c.ftl

  Summary:
    ENCX24J600 Driver Freemarker Template File

  Description:

*******************************************************************************/
-->

<#----------------------------------------------------------------------------
 Copyright (C) 2013-2018 Microchip Technology Inc. and its subsidiaries.

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

// <editor-fold defaultstate="collapsed" desc="ENC 600 Driver Initialization Data">
<#if CONFIG_DRV_ENCX24J600_USE_DRIVER>
/* ENC 600 Driver Configuration */
<#if CONFIG_DRV_ENCX24J600_IDX0>
DRV_ENCX24J600_Configuration drvEncX24j600InitDataIdx0 = {
    .txDescriptors =        DRV_ENCX24J600_MAC_TX_DESCRIPTORS_IDX0,
    .rxDescriptors =        DRV_ENCX24J600_MAC_RX_DESCRIPTORS_IDX0,
    .rxDescBufferSize =     DRV_ENCX24J600_MAX_RX_BUFFER_IDX0,
    .spiDrvIndex =          DRV_ENCX24J600_SPI_DRIVER_INDEX_IDX0,
    .spiBps =               DRV_ENCX24J600_SPI_BPS_IDX0,
    .spiSSPortModule =      DRV_ENCX24J600_SPI_SS_PORT_MODULE_IDX0,
    .spiSSPortChannel =     DRV_ENCX24J600_SPI_SS_PORT_CHANNEL_IDX0,
    .spiSSPortPin =         DRV_ENCX24J600_SPI_SS_PORT_PIN_IDX0,
    .rxBufferSize = 		DRV_ENCX24J600_RX_BUFFER_SIZE_IDX0,
    .maxFrameSize =	        DRV_ENCX24J600_MAX_FRAME_SIZE_IDX0,
	.ethType=			    DRV_ENCX24J600_ETHERNET_TYPE_IDX0,
	.dupMode=				DRV_ENCX24J600_DUPLEX_MODE_IDX0,
};
</#if>
</#if>
// </editor-fold>
