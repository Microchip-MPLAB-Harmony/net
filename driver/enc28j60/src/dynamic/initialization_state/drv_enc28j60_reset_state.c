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
/*
Copyright (C) 2015-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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


