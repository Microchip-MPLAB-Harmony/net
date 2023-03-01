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

