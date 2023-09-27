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
            {   // some error occurred; retry; 
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
