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
