/*******************************************************************************
  ENC28J60 RESET State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_reset_state.c
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
#include "drv_enc28j60_reset_state.h"
#include "../drv_enc28j60_local.h"

int32_t DRV_ENC28J60_ResetStateTask(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
{
    DRV_ENC28J60_RegUnion reg;
    DRV_ENC28J60_BUS_RESULT busRes;
    uintptr_t ret;

	reg.value= 0;
    switch(pDrvInst->mainStateInfo.initInfo.resetStateInfo.state)
    {
        case DRV_ENC28J60_RS_READ_ESTAT:
            ret = (*pDrvInst->busVTable->fpSfrRdStart)(pDrvInst, DRV_ENC28J60_SFR_ESTAT, false);
            if (ret != 0)
            {
                pDrvInst->mainStateInfo.initInfo.resetStateInfo.op = ret;
                pDrvInst->mainStateInfo.initInfo.resetStateInfo.state = DRV_ENC28J60_RS_WAIT_FOR_ESTAT;
            }
            // else retry
            break;

        case DRV_ENC28J60_RS_WAIT_FOR_ESTAT:
            busRes = (*pDrvInst->busVTable->fpSfrRdResult)(pDrvInst, pDrvInst->mainStateInfo.initInfo.resetStateInfo.op, &reg, true);
            if(busRes == DRV_ENC28J60_BR_SUCCESS)
            {
                if(reg.estat.CLKRDY == 0x1)
                {
                    pDrvInst->mainStateInfo.initInfo.resetStateInfo.state = DRV_ENC28J60_RS_SEND_RESET;
                }
                // else wait some more
            }
            else if(busRes < 0)
            {   // some error; retry
                pDrvInst->mainStateInfo.initInfo.resetStateInfo.state = DRV_ENC28J60_RS_READ_ESTAT;
            }
            // else wait some more
            break;

        case DRV_ENC28J60_RS_SEND_RESET:
            ret = (*pDrvInst->busVTable->fpSysRst)(pDrvInst, false);   // send soft system reset
            if (ret != 0)
            {
                pDrvInst->mainStateInfo.initInfo.resetStateInfo.op = ret;
                pDrvInst->mainStateInfo.initInfo.resetStateInfo.state = DRV_ENC28J60_RS_WAIT_RESET;
            }
            // else retry
            break;

        case DRV_ENC28J60_RS_WAIT_RESET:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, pDrvInst->mainStateInfo.initInfo.resetStateInfo.op, true);
            if(busRes == DRV_ENC28J60_BR_SUCCESS)
            {
                pDrvInst->mainStateInfo.initInfo.resetStateInfo.state = DRV_ENC28J60_RS_DONE;
                return 1;
            }
            else if(busRes < 0)
            {   // some error; retry
                pDrvInst->mainStateInfo.initInfo.resetStateInfo.state = DRV_ENC28J60_RS_SEND_RESET;
            }
            // else wait some more
            break;

            //
        default:
            break;
    }

    return 0;
}

int32_t DRV_ENC28J60_ResetStateEnter(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
{
    pDrvInst->mainStateInfo.initInfo.resetStateInfo.state = DRV_ENC28J60_RS_READ_ESTAT;
    return 0;
}

int32_t DRV_ENC28J60_ResetStateExit(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
{
    return 0;
}


