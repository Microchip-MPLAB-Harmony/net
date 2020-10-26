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
