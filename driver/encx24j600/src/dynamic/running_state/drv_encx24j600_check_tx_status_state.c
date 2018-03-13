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
