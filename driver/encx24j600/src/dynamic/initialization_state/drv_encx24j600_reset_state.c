/*******************************************************************************
  ENCx24J600 Detect State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_reset_state.c
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
#include "drv_encx24j600_reset_state.h"
#include "../drv_encx24j600_local.h"
#include "system/tmr/sys_tmr.h"

#define WAIT_25_6_US_DIV 39062
#define WAIT_256_US_DIV 3906


int32_t DRV_ENCX24J600_ResetStateTask(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    DRV_ENCX24J600_RegUnion reg = {0};
    uintptr_t ret;
    switch(pDrvInst->mainStateInfo.initInfo.resetStateInfo.state)
    {
        case DRV_ENCX24J600_RS_READ_ESTAT:
        {
            ret = (*pDrvInst->busVTable->fpSfrRdStart)(pDrvInst, DRV_ENCX24J600_SFR_ESTAT, 0);
            if (ret != 0)
            {
                pDrvInst->mainStateInfo.initInfo.resetStateInfo.op = ret;
                pDrvInst->mainStateInfo.initInfo.resetStateInfo.state = DRV_ENCX24J600_RS_WAIT_FOR_ESTAT;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_RS_WAIT_FOR_ESTAT:
        {
            switch ((*pDrvInst->busVTable->fpOpResult)(pDrvInst, pDrvInst->mainStateInfo.initInfo.resetStateInfo.op))
            {
                case DRV_ENCX24J600_BR_PENDING:
                    break;
                case DRV_ENCX24J600_BR_ERROR:
                    //Report Error
                    break;
                case DRV_ENCX24J600_BR_SUCCESS:
                {
                    (*pDrvInst->busVTable->fpSfrRdResult)(pDrvInst, pDrvInst->mainStateInfo.initInfo.resetStateInfo.op, &reg, 0);
                    if (reg.estat.CLKRDY == 0x1)
                    {
                        pDrvInst->mainStateInfo.initInfo.resetStateInfo.state = DRV_ENCX24J600_RS_SEND_RESET;
                    }
                    else
                    {
                        pDrvInst->mainStateInfo.initInfo.resetStateInfo.state = DRV_ENCX24J600_RS_READ_ESTAT;
                        break;
                    }
                }
            }
        }
        break;
        case DRV_ENCX24J600_RS_SEND_RESET:
        {
            ret = (*pDrvInst->busVTable->fpSysRst)(pDrvInst);
            if (ret != 0)
            {
                pDrvInst->mainStateInfo.initInfo.resetStateInfo.state = DRV_ENCX24J600_RS_WAIT_1;
                pDrvInst->mainStateInfo.initInfo.resetStateInfo.waitTillTick = SYS_TMR_SystemCountGet() + (SYS_TMR_SystemCountFrequencyGet() / WAIT_25_6_US_DIV);
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_RS_WAIT_1:
        {
            if (SYS_TMR_SystemCountGet() > pDrvInst->mainStateInfo.initInfo.resetStateInfo.waitTillTick)
            {
                pDrvInst->mainStateInfo.initInfo.resetStateInfo.state = DRV_ENCX24J600_RS_READ_EUDAST;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_RS_READ_EUDAST:
        {
            ret = (*pDrvInst->busVTable->fpSfrRdStart)(pDrvInst, DRV_ENCX24J600_SFR_EUDAST, 0);
            if (ret != 0)
            {
                pDrvInst->mainStateInfo.initInfo.resetStateInfo.state = DRV_ENCX24J600_RS_WAIT_FOR_EUDAST;
                pDrvInst->mainStateInfo.initInfo.resetStateInfo.op = ret;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_RS_WAIT_FOR_EUDAST:
        {
            switch ((*pDrvInst->busVTable->fpOpResult)(pDrvInst, pDrvInst->mainStateInfo.initInfo.detectStateInfo.readOp))
            {
                case DRV_ENCX24J600_BR_PENDING:
                    break;
                case DRV_ENCX24J600_BR_ERROR:
                    //Report Error
                    break;
                case DRV_ENCX24J600_BR_SUCCESS:
                {
                    (*pDrvInst->busVTable->fpSfrRdResult)(pDrvInst, pDrvInst->mainStateInfo.initInfo.resetStateInfo.op, &reg, 0);
                    if (reg.eudast.EUDAST == 0x0000)
                    {
                        pDrvInst->mainStateInfo.initInfo.resetStateInfo.state = DRV_ENCX24J600_RS_WAIT_2;
                        pDrvInst->mainStateInfo.initInfo.resetStateInfo.waitTillTick = SYS_TMR_SystemCountGet() + (SYS_TMR_SystemCountFrequencyGet() / WAIT_256_US_DIV);
                    }
                    else
                    {
                        pDrvInst->mainStateInfo.initInfo.resetStateInfo.state = DRV_ENCX24J600_RS_SEND_RESET;
                        break;
                    }
                }
            }
        }
        break;
        case DRV_ENCX24J600_RS_WAIT_2:
        {
            if (SYS_TMR_SystemCountGet() > pDrvInst->mainStateInfo.initInfo.resetStateInfo.waitTillTick)
            {
                return 1;
            }
        }
    }
    return 0;
}
int32_t DRV_ENCX24J600_ResetStateEnter(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    pDrvInst->mainStateInfo.initInfo.resetStateInfo.state = DRV_ENCX24J600_RS_READ_ESTAT;
    return 0;
}
int32_t DRV_ENCX24J600_ResetStateExit(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    return 0;
}
