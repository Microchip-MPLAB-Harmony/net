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

