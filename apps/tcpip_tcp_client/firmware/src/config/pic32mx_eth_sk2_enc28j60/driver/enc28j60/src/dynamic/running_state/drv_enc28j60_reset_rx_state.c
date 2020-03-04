/*******************************************************************************
  ENCx24J600 Check TX Status State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_check_tx_status_state.c
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
#include "drv_enc28j60_reset_rx_state.h"
#include "../drv_enc28j60_local.h"
#include "../drv_enc28j60_utils.h"
#include "drv_enc28j60_running_state.h"

int32_t DRV_ENC28J60_ResetRxStateTask(DRV_ENC28J60_DriverInfo * pDrvInst)
{
    DRV_ENC28J60_RESET_RX_INFO * curSt = &(pDrvInst->mainStateInfo.runningInfo.resetRxInfo);
    DRV_ENC28J60_BUS_RESULT busRes;
    DRV_ENC28J60_RegUnion reg;
    uintptr_t ret;
    int ix;
    bool failClrPkt;
    
    switch (curSt->state)
    {
        case DRV_ENC28J60_RRX_WAIT:
            break;

        case DRV_ENC28J60_RRX_STARTING:
            curSt->state = DRV_ENC28J60_RRX_SET_RXRST;
            // no break;

        case DRV_ENC28J60_RRX_SET_RXRST:
            reg.value = 0;
            reg.econ1.RXRST = 1;
            ret = (*pDrvInst->busVTable->fpSfrBitSet)(pDrvInst, DRV_ENC28J60_SFR_ECON1, reg, false);
            if (ret != 0)
            {
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_RRX_WAIT_RXRST;
            }
            break;

        case DRV_ENC28J60_RRX_WAIT_RXRST:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes == DRV_ENC28J60_BR_SUCCESS)
            {
                curSt->state = DRV_ENC28J60_RRX_CLR_RXRST;
            }
            else if(busRes < 0)
            {   // some error, retry
                curSt->state = DRV_ENC28J60_RRX_SET_RXRST;
            }
            // else wait some more
            break;

        case DRV_ENC28J60_RRX_CLR_RXRST:
            reg.value = 0;
            reg.econ1.RXRST = 1;
            ret = (*pDrvInst->busVTable->fpSfrBitClr)(pDrvInst, DRV_ENC28J60_SFR_ECON1, reg, false);
            if (ret != 0)
            {
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_RRX_WAIT_CLR_RXRST;
            }
            break;

        case DRV_ENC28J60_RRX_WAIT_CLR_RXRST:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes == DRV_ENC28J60_BR_SUCCESS)
            {
                curSt->state = DRV_ENC28J60_RRX_READ_EPKTCNT_START;
            }
            else if(busRes < 0)
            {   // some error, retry
                curSt->state = DRV_ENC28J60_RRX_CLR_RXRST;
            }
            // else wait some more
            break;

        case DRV_ENC28J60_RRX_READ_EPKTCNT_START:
            ret = (*pDrvInst->busVTable->fpSfrRdStart)(pDrvInst, DRV_ENC28J60_SFR_EPKTCNT, false);
            if (ret != 0)
            {
                curSt->state = DRV_ENC28J60_RRX_READ_EPKTCNT_WAIT;
                curSt->op = ret;
            }
            break;

        case DRV_ENC28J60_RRX_READ_EPKTCNT_WAIT:
            busRes = (*pDrvInst->busVTable->fpSfrRdResult)(pDrvInst, curSt->op, &reg, true);
            if(busRes < 0)
            {   // error; retry
                curSt->state = DRV_ENC28J60_RRX_READ_EPKTCNT_START;
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {   // wait some more
                break;
            }

            // success

            failClrPkt = false;
            for (ix = 0; ix < reg.epktcnt.EPKTCNT; ix++)
            {
                ret = (*pDrvInst->busVTable->fpDecPktCnt)(pDrvInst, true);
                if (ret == 0)
                {
                    failClrPkt = true;
                    break;
                }
            }

            if(failClrPkt)
            {
                curSt->state = failClrPkt ? DRV_ENC28J60_RRX_READ_EPKTCNT_START : DRV_ENC28J60_RRX_SET_ERXST;
            }
            break;                    

        case DRV_ENC28J60_RRX_SET_ERXST:
            ret = (*pDrvInst->busVTable->fpSfrWr16)(pDrvInst, DRV_ENC28J60_SFR_ERXSTL, pDrvInst->encMemRxStart, false);
            if (ret != 0)
            {   
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_RRX_WAIT_ERXST;
            }
            break;           

        case DRV_ENC28J60_RRX_WAIT_ERXST:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes < 0)
            {   // error; retry
                curSt->state = DRV_ENC28J60_RRX_SET_ERXST;
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {   // wiat some more
                break;
            }
            // success
            curSt->state = DRV_ENC28J60_RRX_SET_ERXRDPT;
            // no break

        case DRV_ENC28J60_RRX_SET_ERXRDPT:
            ret = (*pDrvInst->busVTable->fpSfrWr16)(pDrvInst, DRV_ENC28J60_SFR_ERXRDPTL, pDrvInst->encMemRxEnd, false);
            if (ret != 0)
            {
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_RRX_WAIT_ERXRDPT;
            }
            break;            

        case DRV_ENC28J60_RRX_WAIT_ERXRDPT:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes < 0)
            {   // error; retry
                curSt->state = DRV_ENC28J60_RRX_SET_ERXRDPT;
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {   // wait some more
                break;
            }
            // success
            curSt->state = DRV_ENC28J60_RRX_SET_ERXND;
            // no break

        case DRV_ENC28J60_RRX_SET_ERXND:
            ret = (*pDrvInst->busVTable->fpSfrWr16)(pDrvInst, DRV_ENC28J60_SFR_ERXNDL, pDrvInst->encMemRxEnd, false);
            if (ret != 0)
            {
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_RRX_WAIT_ERXND;
            }
            break;

        case DRV_ENC28J60_RRX_WAIT_ERXND:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes < 0)
            {   // error; retry
                curSt->state = DRV_ENC28J60_RRX_SET_ERXND;
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {   // wait some more
                break;
            }
            // success
            curSt->state = DRV_ENC28J60_RRX_SET_RXEN;
            // no break

        case DRV_ENC28J60_RRX_SET_RXEN:
            ret = (*pDrvInst->busVTable->fpRxEnable)(pDrvInst, false);
            if (ret != 0)
            {
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_RRX_WAIT_RXEN;
            }
            break;

        case DRV_ENC28J60_RRX_WAIT_RXEN:
            busRes = (*pDrvInst->busVTable->fpOpResult)(pDrvInst, curSt->op, true);
            if(busRes < 0)
            {   // error; retry
                curSt->state = DRV_ENC28J60_RRX_SET_RXEN;
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {   // wait some more
                break;
            }
            // success
            curSt->state = DRV_ENC28J60_RRX_WAIT;
            break;

        default:
            break;
    }
    return 0;
}

