/*******************************************************************************
  ENCx24J600 Driver Utilities
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_utils.h
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.
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

#ifndef _DRV_ENCX24J600_UTILS_H_
#define _DRV_ENCX24J600_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "system_config.h"
#include "tcpip/tcpip_mac.h"
#include "drv_encx24j600_ds_defs.h"
#include "drv_encx24j600_local.h"
#include "osal/osal.h"
#include "packet/drv_encx24j600_rx_packet.h"
#include "packet/drv_encx24j600_tx_packet.h"

static inline bool _DRV_ENCX24J600_ReadSfr(struct _DRV_ENCX24J600_DriverInfo *  pDrvInst, uintptr_t  handle, DRV_ENCX24J600_RegUnion *  value,  uint8_t  opIndex )
{
    switch ((*pDrvInst->busVTable->fpOpResult)(pDrvInst, handle))
    {
        case DRV_ENCX24J600_BR_PENDING:
            break;
        case DRV_ENCX24J600_BR_ERROR:
            //Report Error
            break;
        case DRV_ENCX24J600_BR_SUCCESS:
        {
            (*pDrvInst->busVTable->fpSfrRdResult)(pDrvInst, handle, value, opIndex);
            return true;
        }
    }
    return false;
}

void DRV_ENCX24J600_SetEvent(struct _DRV_ENCX24J600_DriverInfo *  pDrvInst, TCPIP_MAC_EVENT events);

void DRV_ENCX24J600_AddGpData(struct _DRV_ENCX24J600_DriverInfo *  pDrvInst, uint16_t size);

static inline void DRV_ENCX24J600_TxAck(struct _DRV_ENCX24J600_DriverInfo *  pDrvInst, uint16_t size)
{
    pDrvInst->txBufferRemaining += size;
}



#ifdef __cplusplus
}
#endif


#endif
