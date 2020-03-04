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

// <editor-fold defaultstate="collapsed" desc="ENC 600 Driver Initialization Data">

<#if (DRV_ENCX24J600_USE_DRIVER)?has_content && (DRV_ENCX24J600_USE_DRIVER == true)>
/* ENC 600 Driver Configuration */
const DRV_ENCX24J600_Configuration drvEncX24j600InitData[] = {
<#list 0..(__INSTANCE_COUNT?number-1) as idx>
{

    .txDescriptors =        DRV_ENCX24J600_MAC_TX_DESCRIPTORS_IDX${idx},
    .rxDescriptors =        DRV_ENCX24J600_MAC_RX_DESCRIPTORS_IDX${idx},
    .rxDescBufferSize =     DRV_ENCX24J600_MAX_RX_BUFFER_IDX${idx},
    .rxBufferSize = 		DRV_ENCX24J600_RX_BUFFER_SIZE_IDX${idx},
    .maxFrameSize =	        DRV_ENCX24J600_MAX_FRAME_SIZE_IDX${idx},
	.ethType=			    DRV_ENCX24J600_ETHERNET_TYPE_IDX${idx},
	.dupMode=				DRV_ENCX24J600_DUPLEX_MODE_IDX${idx},
	.spiSetup.chipSelect =  DRV_ENCX24J600_SPI_CS_IDX${idx},
},
</#list>
};
</#if>
// </editor-fold>

