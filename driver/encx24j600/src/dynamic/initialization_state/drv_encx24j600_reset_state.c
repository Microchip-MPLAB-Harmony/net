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
#include "drv_encx24j600_reset_state.h"
#include "../drv_encx24j600_local.h"
#include "system/time/sys_time.h"

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
                pDrvInst->mainStateInfo.initInfo.resetStateInfo.waitTillTick = SYS_TIME_CounterGet() + (SYS_TIME_FrequencyGet() / WAIT_25_6_US_DIV);
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_RS_WAIT_1:
        {
            if (SYS_TIME_CounterGet() > pDrvInst->mainStateInfo.initInfo.resetStateInfo.waitTillTick)
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
                        pDrvInst->mainStateInfo.initInfo.resetStateInfo.waitTillTick = SYS_TIME_CounterGet() + (SYS_TIME_FrequencyGet() / WAIT_256_US_DIV);
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
            if (SYS_TIME_CounterGet() > pDrvInst->mainStateInfo.initInfo.resetStateInfo.waitTillTick)
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
