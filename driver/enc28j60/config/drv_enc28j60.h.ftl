<#--
/*******************************************************************************
  enc28j60 Driver Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_ecn28j60.h.ftl

  Summary:
    enc28j60 Driver Freemarker Template File

  Description:

*******************************************************************************/
-->

<#----------------------------------------------------------------------------
 Copyright (C) 2015-2018 Microchip Technology Inc. and its subsidiaries.

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


<#if CONFIG_DRV_ENC28J60_USE_DRIVER>
/*** enc28j60 Driver Configuration ***/
/*** Driver Compilation and static configuration options. ***/
#define TCPIP_IF_ENC28J60
#define DRV_ENC28J60_INSTANCES_NUMBER ${CONFIG_DRV_ENC28J60_INSTANCES_NUMBER}
#define DRV_ENC28J60_CLIENT_INSTANCES ${CONFIG_DRV_ENC28J60_CLIENT_INSTANCES}
<#if CONFIG_DRV_ENC28J60_IDX0>
#define DRV_ENC28J60_MAC_TX_DESCRIPTORS_IDX0 ${CONFIG_DRV_ENC28J60_MAC_TX_DESCRIPTORS_IDX0}
#define DRV_ENC28J60_MAC_RX_DESCRIPTORS_IDX0 ${CONFIG_DRV_ENC28J60_MAC_RX_DESCRIPTORS_IDX0}
#define DRV_ENC28J60_MAX_RX_BUFFER_IDX0 ${CONFIG_DRV_ENC28J60_MAX_RX_BUFFER_IDX0}
#define DRV_ENC28J60_SPI_DRIVER_INDEX_IDX0 ${CONFIG_DRV_ENC28J60_SPI_DRV_INDEX_IDX0}
#define DRV_ENC28J60_SPI_BPS_IDX0 ${CONFIG_DRV_ENC28J60_SPI_BPS_IDX0}
#define DRV_ENC28J60_SPI_SS_PORT_MODULE_IDX0 ${CONFIG_DRV_ENC28J60_SPI_SS_PORT_MODULE_IDX0}
#define DRV_ENC28J60_SPI_SS_PORT_CHANNEL_IDX0 ${CONFIG_DRV_ENC28J60_SPI_SS_PORT_CHANNEL_IDX0}
#define DRV_ENC28J60_SPI_SS_PORT_PIN_IDX0 ${CONFIG_DRV_ENC28J60_SPI_SS_PORT_PIN_IDX0}
#define DRV_ENC28J60_RX_BUFFER_SIZE_IDX0 ${CONFIG_DRV_ENC28J60_RX_BUFFER_SIZE_IDX0}
#define DRV_ENC28J60_MAX_FRAME_SIZE_IDX0 ${CONFIG_DRV_ENC28J60_MAX_FRAME_SIZE_IDX0}
</#if>
</#if>
