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
/*
Copyright (C) 2014-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
*/

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
