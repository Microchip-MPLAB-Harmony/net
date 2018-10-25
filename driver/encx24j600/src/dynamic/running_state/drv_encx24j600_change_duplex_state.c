/*******************************************************************************
  ENCx24J600 Change Duplex State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_change_duplex_state.c
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2014-2018 Microchip Technology Inc. and its subsidiaries.

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
#include "drv_encx24j600_change_duplex_state.h"
#include "../drv_encx24j600_local.h"
#include "drv_encx24j600_running_state.h"
#include "../drv_encx24j600_utils.h"


int32_t DRV_ENCX24J600_ChgDupStateTask(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    DRV_ENCX24J600_CHANGE_DUPLEX_INFO * curSt = &pDrvInst->mainStateInfo.runningInfo.chgDupInfo;
    DRV_ENCX24J600_RegUnion reg = {0};
    uintptr_t ret;

    switch (curSt->state)
    {
        case DRV_ENCX24J600_CD_WAIT:
        {
            if (curSt->duplexChangeNeeded)
            {
                curSt->state = DRV_ENCX24J600_CD_SET_MACON2;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CD_SET_MACON2:
        {
            reg.value = 0;
            reg.macon2.FULDPX = 1;
            if (curSt->duplexMode)
            {
                ret = (pDrvInst->busVTable->fpSfrBitSet)(pDrvInst, DRV_ENCX24J600_SFR_MACON2, reg, DRV_ENCX24J600_CD_OP_SET_MACON2);
            }
            else
            {
                ret = (pDrvInst->busVTable->fpSfrBitClr)(pDrvInst, DRV_ENCX24J600_SFR_MACON2, reg, DRV_ENCX24J600_CD_OP_SET_MACON2);
            }
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CD_SET_BBIPG;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CD_SET_BBIPG:
        {
            reg.value = 0;
            if (curSt->duplexMode)
            {
                reg.mabbipg.BBIPG = 0x15;
            }
            else
            {
                reg.mabbipg.BBIPG = 0x12;
            }
            ret = (*pDrvInst->busVTable->fpSfrWr)(pDrvInst, DRV_ENCX24J600_SFR_MABBIPG, reg, DRV_ENCX24J600_CD_OP_SET_BBIPG);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CD_ENABLE_RX;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CD_ENABLE_RX:
        {
            ret = (*pDrvInst->busVTable->fpRxEnable)(pDrvInst);
            if (ret != 0)
            {
                curSt->duplexChangeNeeded = false;
                curSt->state = DRV_ENCX24J600_CD_WAIT;
            }
        }
    }

    return 0;
}

int32_t DRV_ENCX24J600_ChgDupStateEnter(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    DRV_ENCX24J600_CHANGE_DUPLEX_INFO * curSt = &pDrvInst->mainStateInfo.runningInfo.chgDupInfo;
    curSt->state = DRV_ENCX24J600_CD_WAIT;
    curSt->duplexChangeNeeded = true;
    curSt->duplexMode = false;
    return 0;
}

int32_t DRV_ENCX24J600_ChgDupStateExit(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    return 0;
}


