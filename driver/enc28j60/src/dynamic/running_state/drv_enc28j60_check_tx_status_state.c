/*******************************************************************************
  ENC28J60 Check TX Status State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_check_tx_status_state.c
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2014-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#include "drv_enc28j60_check_tx_status_state.h"
#include "../drv_enc28j60_local.h"
#include "../drv_enc28j60_utils.h"
#include "drv_enc28j60_running_state.h"


int32_t DRV_ENC28J60_ChkTxStatusStateTask(struct S_DRV_ENC28J60_DriverInfo * pDrvInst)
{
    int choice;
    int count;
    DRV_ENC28J60_PHY_RES phyRes;
    uint16_t    phyRegValue;
    DRV_ENC28J60_RegUnion reg;
    uintptr_t ret;
    DRV_ENC28J60_BUS_RESULT res;
    DRV_ENC28J60_TX_PACKET_INFO* pTxLock;
    DRV_ENC28J60_CHECK_TX_STATUS_INFO * curSt = &(pDrvInst->mainStateInfo.runningInfo.chkTxStaInfo);
    
    switch(curSt->state)
    {
        case DRV_ENC28J60_CTS_WAIT:
            if (curSt->txUpdateNeeded)
            {
                for (choice = 0; choice < MAX_TX_DESCRIPTORS; choice ++)
                {
                    for (count = 0; count < MAX_TX_DESCRIPTORS; count++)
                    {
                        if (pDrvInst->txDescriptors[count].state == DRV_ENC28J60_TP_WAIT_FOR_COMPLETE)
                        {
                            pDrvInst->txDescriptors[count].state = DRV_ENC28J60_TP_RST_EIR;
                            break;
                        }
                    }
                }
                curSt->txUpdateNeeded = false;
                break;
            }

            // no txUpdateNeeded detected
            // try to avoid a TX lock; check if any lock in the DRV_ENC28J60_TP_WAIT_FOR_COMPLETE status
            pTxLock = NULL;
            for (count = 0; count < MAX_TX_DESCRIPTORS; count++)
            {
                if (pDrvInst->txDescriptors[count].state == DRV_ENC28J60_TP_WAIT_FOR_COMPLETE)
                {
                    pTxLock = pDrvInst->txDescriptors + count;
                    break;
                }
            }

            if(pTxLock != NULL)
            {
                curSt->pTxLock =  pTxLock;
                curSt->state = DRV_ENC28J60_CTS_READ_PHSTAT2;
            }

            break;

        case DRV_ENC28J60_CTS_READ_PHSTAT2:
            phyRes = (*pDrvInst->busVTable->fpPhyRdStart)(pDrvInst, DRV_ENC28J60_PHY_SFR_PHSTAT2);
            if(phyRes >= 0)
            {   // success
                curSt->state = DRV_ENC28J60_CTS_WAIT_PHSTAT2;
            }
            // else error; retry
            break;

        case DRV_ENC28J60_CTS_WAIT_PHSTAT2:
            phyRes = (*pDrvInst->busVTable->fpPhyRd)(pDrvInst, DRV_ENC28J60_PHY_SFR_PHSTAT2, &phyRegValue);
            if(phyRes < 0)
            {   // error; retry
                curSt->state = DRV_ENC28J60_CTS_READ_PHSTAT2;
                break;
            }
            else if (phyRes == DRV_ENC28J60_PHY_RES_PENDING)
            {   // wait some more
                break;
            }
            else
            {
                // OK
            }
            // got the PHY value
            reg.value = phyRegValue;
            if(reg.phstat2.TXSTAT != 0)
            {   // PHY is busy, ok, no problem
                curSt->state = DRV_ENC28J60_CTS_DONE;
                break;
            }
            curSt->state = DRV_ENC28J60_CTS_READ_ECON1;
            break;

        case DRV_ENC28J60_CTS_READ_ECON1:
            ret = (*pDrvInst->busVTable->fpSfrRdStart)(pDrvInst, DRV_ENC28J60_SFR_ECON1, false);
            if(ret != 0)
            {   // success
                curSt->op = ret;
                curSt->state = DRV_ENC28J60_CTS_WAIT_ECON1;
            }
            // else retry
            break;

        case DRV_ENC28J60_CTS_WAIT_ECON1:
            res = (*pDrvInst->busVTable->fpSfrRdResult)(pDrvInst, curSt->op, &reg, true);
            if(res < 0)
            {   // some error; retry
                curSt->state = DRV_ENC28J60_CTS_READ_ECON1;
                break;
            }
            else if(res == DRV_ENC28J60_BR_PENDING)
            {   // wait some more
                break;
            }
            else
            {
                // OK
            }

            // have ECON1
            if(reg.econ1.TXRTS == 0)
            {   // all fine, done
                curSt->state = DRV_ENC28J60_CTS_DONE;
                break;
            }

            // TX logic may be locked!
            ((DRV_ENC28J60_TX_PACKET_INFO*)curSt->pTxLock)->state = DRV_ENC28J60_TP_RESET_TX;
            curSt->txUpdateNeeded = true;
            curSt->state = DRV_ENC28J60_CTS_DONE;
            break;

        case DRV_ENC28J60_CTS_DONE:
            curSt->state = DRV_ENC28J60_CTS_WAIT;
            break;

        default:
            // do nothing
            break;
    }

    return 0;
}





int32_t DRV_ENC28J60_ChkTxStatusStateEnter(struct S_DRV_ENC28J60_DriverInfo * pDrvInst)
{
    DRV_ENC28J60_CHECK_TX_STATUS_INFO * curSt = &(pDrvInst->mainStateInfo.runningInfo.chkTxStaInfo);
    curSt->state = DRV_ENC28J60_CTS_WAIT;
    curSt->txUpdateNeeded = false;

    return 0;
}
int32_t DRV_ENC28J60_ChkTxStatusStateExit(struct S_DRV_ENC28J60_DriverInfo * pDrvInst)
{
    return 0;
}

