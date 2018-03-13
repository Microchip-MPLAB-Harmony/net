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


