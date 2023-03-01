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

