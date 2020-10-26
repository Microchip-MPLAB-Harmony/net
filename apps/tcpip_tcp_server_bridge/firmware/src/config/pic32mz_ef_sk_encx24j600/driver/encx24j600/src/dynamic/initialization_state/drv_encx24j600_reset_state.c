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
