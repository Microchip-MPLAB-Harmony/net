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
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.
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
#include "drv_enc28j60_reset_state.h"
#include "../drv_enc28j60_local.h"

// TODO aa: some retry mechanism must be implemented
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


