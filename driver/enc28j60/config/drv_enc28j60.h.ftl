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

/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS  WITHOUT  WARRANTY  OF  ANY  KIND,
EITHER EXPRESS  OR  IMPLIED,  INCLUDING  WITHOUT  LIMITATION,  ANY  WARRANTY  OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A  PARTICULAR  PURPOSE.
IN NO EVENT SHALL MICROCHIP OR  ITS  LICENSORS  BE  LIABLE  OR  OBLIGATED  UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,  BREACH  OF  WARRANTY,  OR
OTHER LEGAL  EQUITABLE  THEORY  ANY  DIRECT  OR  INDIRECT  DAMAGES  OR  EXPENSES
INCLUDING BUT NOT LIMITED TO ANY  INCIDENTAL,  SPECIAL,  INDIRECT,  PUNITIVE  OR
CONSEQUENTIAL DAMAGES, LOST  PROFITS  OR  LOST  DATA,  COST  OF  PROCUREMENT  OF
SUBSTITUTE  GOODS,  TECHNOLOGY,  SERVICES,  OR  ANY  CLAIMS  BY  THIRD   PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE  THEREOF),  OR  OTHER  SIMILAR  COSTS.
*******************************************************************************/
-->

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