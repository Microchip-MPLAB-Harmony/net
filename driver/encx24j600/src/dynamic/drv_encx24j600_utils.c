/*******************************************************************************
  ENCx24J600 Driver Utilities
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_utils.c
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

#include "drv_encx24j600_utils.h"
#include "osal/osal.h"
#include "drv_encx24j600_local.h"
void DRV_ENCX24J600_SetEvent(struct _DRV_ENCX24J600_DriverInfo *  pDrvInst, TCPIP_MAC_EVENT events)
{
    events &= pDrvInst->eventMask;
    if (events == 0)
    {
        return;
    }
    OSAL_RESULT res = OSAL_MUTEX_Lock(&pDrvInst->drvMutex, OSAL_WAIT_FOREVER);
    res = res;
    SYS_ASSERT(res == OSAL_RESULT_TRUE, "Could not lock the driver mutex");

    pDrvInst->currentEvents |= events;
    (*pDrvInst->stackCfg.eventF)(events, pDrvInst->stackCfg.eventParam);
    
    res = OSAL_MUTEX_Unlock(&pDrvInst->drvMutex);
    res = res;
    SYS_ASSERT(res == OSAL_RESULT_TRUE, "Could not unlock the driver mutex");

}

void DRV_ENCX24J600_AddGpData(struct _DRV_ENCX24J600_DriverInfo *  pDrvInst, uint16_t size)
{
    pDrvInst->txBufferRemaining -= size;
    pDrvInst->gpPtrVal += size;
    if (pDrvInst->gpPtrVal >= pDrvInst->encMemRxStart)
    {
        pDrvInst->gpPtrVal = pDrvInst->gpPtrVal - pDrvInst->encMemRxStart;
    }

}