/*******************************************************************************
  ENC28J60 Check Interrupt State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_check_int_state.c
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
#include "drv_enc28j60_check_int_state.h"
#include "..//drv_enc28j60_local.h"
#include "../drv_enc28j60_utils.h"
#include "drv_enc28j60_running_state.h"



int32_t DRV_ENC28J60_ChkIntStateTask(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
{
    DRV_ENC28J60_CHECK_INT_INFO * curSt = &(pDrvInst->mainStateInfo.runningInfo.chkIntInfo);
    DRV_ENC28J60_RegUnion reg;
    uintptr_t ret;
    DRV_ENC28J60_BUS_RESULT res;

    switch(curSt->state)
    {
        case DRV_ENC28J60_CI_WAIT:
            //TODO aa: ENC28J60 interrupts are not supported for now!
            curSt->state = DRV_ENC28J60_CI_READ_EIR;
            // no break

        case DRV_ENC28J60_CI_READ_EIR:
            ret = (*pDrvInst->busVTable->fpSfrRdStart)(pDrvInst, DRV_ENC28J60_SFR_EIR, false);
            if (ret != 0)
            {
                curSt->state = DRV_ENC28J60_CI_WAIT_FOR_EIR;
                curSt->op = ret;
            }
            // retry
            break;

        case DRV_ENC28J60_CI_WAIT_FOR_EIR:
            reg.value = 0;
            res = (*pDrvInst->busVTable->fpSfrRdResult)(pDrvInst, curSt->op, &reg, true);
            if(res < 0)
            {   // some error occurred; retry; TODO aa: retry counter needed!!!
                curSt->state = DRV_ENC28J60_CI_READ_EIR;
                break;
            }
            else if(res == DRV_ENC28J60_BR_PENDING)
            {   // wait some more
                break;
            }

            // success
            if (reg.eir.LINKIF != 0)
            {
                pDrvInst->mainStateInfo.runningInfo.chkStaInfo.linkUpdateNeeded = true;
            }
            if (reg.eir.TXERIF != 0 || reg.eir.TXIF != 0)
            {
                pDrvInst->mainStateInfo.runningInfo.chkTxStaInfo.txUpdateNeeded = true;
            }

            curSt->state = DRV_ENC28J60_CI_READ_PKTCNT;
            // no break;

        case DRV_ENC28J60_CI_READ_PKTCNT:
            // Errata Workaround
            // PKTIF is not reliabale. 
            // Read the EPKTCNT to check for pending packets
            ret = (*pDrvInst->busVTable->fpSfrRdStart)(pDrvInst, DRV_ENC28J60_SFR_EPKTCNT, false);
            if(ret != 0)
            {   // success
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_CI_WAIT_FOR_PKTCNT;
            }
            // else retry
            break;

        case DRV_ENC28J60_CI_WAIT_FOR_PKTCNT:
            res = (*pDrvInst->busVTable->fpSfrRdResult)(pDrvInst, curSt->op, &reg, true);
            if(res == DRV_ENC28J60_BR_SUCCESS)
            {
                pDrvInst->rxPacketPending = reg.epktcnt.EPKTCNT != 0;
                curSt->state = DRV_ENC28J60_CI_WAIT;
            }
            else if(res < 0)
            {   // some error; retry
                curSt->state = DRV_ENC28J60_CI_READ_PKTCNT;
            }
            // else wait some more
            break;

        default:
            break;
    }
    return 0;

}

int32_t DRV_ENC28J60_ChkIntStateEnter(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
{
    DRV_ENC28J60_CHECK_INT_INFO * curSt = &(pDrvInst->mainStateInfo.runningInfo.chkIntInfo);
    curSt->state = DRV_ENC28J60_CI_WAIT;
	pDrvInst->mainStateInfo.runningInfo.chkStaInfo.linkUpdateNeeded = true;

    return 0;
}
int32_t DRV_ENC28J60_ChkIntStateExit(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
{
    return 0;
}
