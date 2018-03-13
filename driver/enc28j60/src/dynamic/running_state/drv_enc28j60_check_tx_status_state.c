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
/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.
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
#include "drv_enc28j60_check_tx_status_state.h"
#include "..//drv_enc28j60_local.h"
#include "../drv_enc28j60_utils.h"
#include "drv_enc28j60_running_state.h"


int32_t DRV_ENC28J60_ChkTxStatusStateTask(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
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
            pTxLock = 0;
            for (count = 0; count < MAX_TX_DESCRIPTORS; count++)
            {
                if (pDrvInst->txDescriptors[count].state == DRV_ENC28J60_TP_WAIT_FOR_COMPLETE)
                {
                    pTxLock = pDrvInst->txDescriptors + count;
                    break;
                }
            }

            if(pTxLock)
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
            // TODO aa: retry counter is needed!
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
            // got the PHY value
            reg.value = phyRegValue;
            if(reg.phstat2.TXSTAT != 0)
            {   // PHY is busy, ok, no problem
                curSt->state = DRV_ENC28J60_CTS_DONE;
                break;
            }
            curSt->state = DRV_ENC28J60_CTS_READ_ECON1;
            // no break

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
            // no break

        case DRV_ENC28J60_CTS_DONE:
            curSt->state = DRV_ENC28J60_CTS_WAIT;
            break;
    }

    return 0;
}





int32_t DRV_ENC28J60_ChkTxStatusStateEnter(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
{
    DRV_ENC28J60_CHECK_TX_STATUS_INFO * curSt = &(pDrvInst->mainStateInfo.runningInfo.chkTxStaInfo);
    curSt->state = DRV_ENC28J60_CTS_WAIT;
    curSt->txUpdateNeeded = false;

    return 0;
}
int32_t DRV_ENC28J60_ChkTxStatusStateExit(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
{
    return 0;
}

