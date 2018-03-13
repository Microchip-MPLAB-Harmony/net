/*******************************************************************************
  ENC28J60 Driver Utilities
  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_utils.h
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.
Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).
You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.
SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
// DOM-IGNORE-END

#ifndef _DRV_ENC28J60_UTILS_H_
#define _DRV_ENC28J60_UTILS_H_


#include "system_config.h"
#include "tcpip/tcpip_mac.h"
#include "drv_enc28j60_ds_defs.h"
#include "drv_enc28j60_local.h"
#include "osal/osal.h"
#include "packet/drv_enc28j60_rx_packet.h"
#include "packet/drv_enc28j60_tx_packet.h"

void DRV_ENC28J60_SetEvent(struct _DRV_ENC28J60_DriverInfo *  pDrvInst, TCPIP_MAC_EVENT events);

void DRV_ENC28J60_AddGpData(struct _DRV_ENC28J60_DriverInfo *  pDrvInst, uint16_t size);

static inline void DRV_ENC28J60_TxAck(struct _DRV_ENC28J60_DriverInfo *  pDrvInst, uint16_t size)
{
    pDrvInst->txBufferRemaining += size;
}

#endif
