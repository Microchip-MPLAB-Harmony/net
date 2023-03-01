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
#include "drv_encx24j600_check_tx_status_state.h"
#include "..//drv_encx24j600_local.h"
#include "../drv_encx24j600_utils.h"
#include "drv_encx24j600_running_state.h"


int32_t DRV_ENCX24J600_ChkTxStatusStateTask(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    DRV_ENCX24J600_CHECK_TX_STATUS_INFO * curSt = &(pDrvInst->mainStateInfo.runningInfo.chkTxStaInfo);
    DRV_ENCX24J600_RegUnion reg = {0};
    uintptr_t ret;
    switch(curSt->state)
    {
        case DRV_ENCX24J600_CTS_WAIT:
        {
            if (curSt->statusUpdateNeeded)
            {
                curSt->state = DRV_ENCX24J600_CTS_READ_ETXSTAT;
            }
            else
            {
                break;  // For some reasont he Interrupts aren't firing.
            }
        }
        case DRV_ENCX24J600_CTS_READ_ETXSTAT:
        {
            ret = (*pDrvInst->busVTable->fpSfrRdStart)(pDrvInst, DRV_ENCX24J600_SFR_ETXSTAT, DRV_ENCX24J600_CTS_OP_READ_ETXSTAT);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CTS_WAIT_FOR_ETXSTAT;
                curSt->etxstatOp = ret;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CTS_WAIT_FOR_ETXSTAT:
        {
            if (_DRV_ENCX24J600_ReadSfr(pDrvInst, curSt->etxstatOp, &reg, DRV_ENCX24J600_CTS_OP_READ_ETXSTAT))
            {
                curSt->status = reg.value;

                curSt->state = DRV_ENCX24J600_CTS_WAIT;
                uint8_t choice;
                for (choice = 0; choice < MAX_TX_DESCRIPTORS; choice ++)
                {
                    uint8_t count;
                    for (count = 0; count < MAX_TX_DESCRIPTORS; count++)
                    {
                        if (pDrvInst->txDescriptors[count].state == DRV_ENCX24J600_TP_WAIT_FOR_COMPLETE)
                        {
                            pDrvInst->txDescriptors[count].state = DRV_ENCX24J600_TP_RST_EIR;
                            break;
                        }
                    }
                }
                curSt->statusUpdateNeeded = false;
            }
        }
    }

    return 0;
}
int32_t DRV_ENCX24J600_ChkTxStatusStateEnter(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    DRV_ENCX24J600_CHECK_TX_STATUS_INFO * curSt = &(pDrvInst->mainStateInfo.runningInfo.chkTxStaInfo);
    curSt->state = DRV_ENCX24J600_CTS_WAIT;
    curSt->statusUpdateNeeded = false;
    return 0;
}
int32_t DRV_ENCX24J600_ChkTxStatusStateExit(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    return 0;
}
