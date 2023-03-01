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