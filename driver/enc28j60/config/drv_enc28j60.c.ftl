<#--
/*******************************************************************************
  ENC28J60 Driver Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60.c.ftl

  Summary:
    ENC28J60 Driver Freemarker Template File

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

// <editor-fold defaultstate="collapsed" desc="ENC 60 Driver Initialization Data">

<#if (DRV_ENC28J60_USE_DRIVER)?has_content && (DRV_ENC28J60_USE_DRIVER == true)>
/* ENC 600 Driver Configuration */
const DRV_ENC28J60_Configuration drvEnc28j60InitData[] = {
<#list 0..(__INSTANCE_COUNT?number-1) as idx>
{

    .txDescriptors =        DRV_ENC28J60_MAC_TX_DESCRIPTORS_IDX${idx},
    .rxDescriptors =        DRV_ENC28J60_MAC_RX_DESCRIPTORS_IDX${idx},
    .rxDescBufferSize =     DRV_ENC28J60_MAX_RX_BUFFER_IDX${idx},
    .rxBufferSize = 		DRV_ENC28J60_RX_BUFFER_SIZE_IDX${idx},
    .maxFrameSize =	        DRV_ENC28J60_MAX_FRAME_SIZE_IDX${idx},
	.spiSetup.chipSelect =  DRV_ENC28J60_SPI_CS_IDX${idx},
},
</#list>
};

</#if>
// </editor-fold>

