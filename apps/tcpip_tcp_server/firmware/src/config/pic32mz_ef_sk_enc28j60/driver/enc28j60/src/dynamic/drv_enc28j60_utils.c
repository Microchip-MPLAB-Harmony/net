/*******************************************************************************
  ENC28J60 Driver Utilities
  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_utils.c
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
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
*****************************************************************************/

// DOM-IGNORE-END

#include "drv_enc28j60_utils.h"
#include "osal/osal.h"
#include "drv_enc28j60_local.h"

void DRV_ENC28J60_SetEvent(struct _DRV_ENC28J60_DriverInfo *  pDrvInst, TCPIP_MAC_EVENT events)
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




void DRV_ENC28J60_AddGpData(struct _DRV_ENC28J60_DriverInfo *  pDrvInst, uint16_t size)
{
    pDrvInst->txBufferRemaining -= size;
    pDrvInst->txPtrVal += size;
    if (pDrvInst->txPtrVal >=pDrvInst->encRamForAppStartAdr)
    {
        pDrvInst->txPtrVal = pDrvInst->encMemTxStart + (pDrvInst->txPtrVal - pDrvInst->encRamForAppStartAdr);
    }

}

// ENCJ60 debugging
//
volatile int DRV_ENC28J60_StayAssertLoop = 0;

void DRV_ENC28J60_Assert(bool cond, const char* message, int lineNo)
{
#if ((DRV_ENC28J60_DEBUG_LEVEL & DRV_ENC28J60_DEBUG_MASK_BASIC) != 0)

    if(cond == false)
    {
        SYS_CONSOLE_PRINT("ENC28J60 Assert: %s, in line: %d, \r\n", message, lineNo);
        while(DRV_ENC28J60_StayAssertLoop == 1);
    }
    
#endif  // ((DRV_ENC28J60_DEBUG_LEVEL & DRV_ENC28J60_DEBUG_MASK_BASIC) != 0)
}


