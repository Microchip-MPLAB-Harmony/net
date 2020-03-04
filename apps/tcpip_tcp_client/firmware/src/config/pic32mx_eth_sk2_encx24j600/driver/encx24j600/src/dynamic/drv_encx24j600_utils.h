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
/*****************************************************************************
 Copyright (C) 2014-2020 Microchip Technology Inc. and its subsidiaries.

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
*****************************************************************************/

// DOM-IGNORE-END

#ifndef _DRV_ENCX24J600_UTILS_H_
#define _DRV_ENCX24J600_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif
    
#include "drv_encx24j600_ds_defs.h"
#include "drv_encx24j600_local.h"
#include "tcpip/tcpip_mac.h"
#include "system_config.h"  
    
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
