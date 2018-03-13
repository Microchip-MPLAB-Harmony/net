/*******************************************************************************
  ENCx24J600 Detect State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_detect_state.c
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
#include "drv_encx24j600_detect_state.h"
#include "../drv_encx24j600_local.h"
#include "../drv_encx24j600_utils.h"


int32_t DRV_ENCX24J600_DetectStateTask(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    DRV_ENCX24J600_RegUnion reg = {0};
    uintptr_t ret;
    switch (pDrvInst->mainStateInfo.initInfo.detectStateInfo.state)
    {
        case DRV_ENCX24J600_DS_WRITE_EUDAST:
        {
            reg.eudast.EUDAST = 0x1234;
            ret = (*pDrvInst->busVTable->fpSfrWr)(pDrvInst, DRV_ENCX24J600_SFR_EUDAST, reg, DRV_ENCX24J600_DS_OPS_WRITE_EUDAST);
            if (ret != 0)
            {
                pDrvInst->mainStateInfo.initInfo.detectStateInfo.state = DRV_ENCX24J600_DS_READ_EUDAST;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_DS_READ_EUDAST:
        {
            ret = (*pDrvInst->busVTable->fpSfrRdStart)(pDrvInst, DRV_ENCX24J600_SFR_EUDAST, DRV_ENCX24J600_DS_OPS_READ_EUDAST);
            if (ret != 0)
            {
                pDrvInst->mainStateInfo.initInfo.detectStateInfo.state = DRV_ENCX24J600_DS_WAIT_FOR_READ;
                pDrvInst->mainStateInfo.initInfo.detectStateInfo.readOp = ret;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_DS_WAIT_FOR_READ:
        {
            if (_DRV_ENCX24J600_ReadSfr(pDrvInst, pDrvInst->mainStateInfo.initInfo.detectStateInfo.readOp, &reg, DRV_ENCX24J600_DS_OPS_READ_EUDAST))
            {
                if (reg.eudast.EUDAST == 0x1234)
                {
                    return 1;
                }
                else
                {
                    pDrvInst->mainStateInfo.initInfo.detectStateInfo.state = DRV_ENCX24J600_DS_WRITE_EUDAST;
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }
    return 0;
}

int32_t DRV_ENCX24J600_DetectStateEnter(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    pDrvInst->mainStateInfo.initInfo.detectStateInfo.state = DRV_ENCX24J600_DS_WRITE_EUDAST;
    return 0;
}

int32_t DRV_ENCX24J600_DetectStateExit(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    return 0;
}
