/*******************************************************************************
  ENCx24J600 Check Interrupt State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_check_int_state.c
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
#include "drv_encx24j600_check_int_state.h"
#include "../drv_encx24j600_local.h"
#include "../drv_encx24j600_utils.h"
#include "drv_encx24j600_running_state.h"


int32_t DRV_ENCX24J600_ChkIntStateTask(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    DRV_ENCX24J600_CHECK_INT_INFO * curSt = &(pDrvInst->mainStateInfo.runningInfo.chkIntInfo);
    DRV_ENCX24J600_RegUnion reg = {0};
    uintptr_t ret;
    switch(curSt->state)
    {
        case DRV_ENCX24J600_CI_WAIT:
        {
            //TODO: For ISR mode put in some logic here.  Check the state of the pin
            curSt->state = DRV_ENCX24J600_CI_READ_EIR;
        }
        case DRV_ENCX24J600_CI_READ_EIR:
        {
            ret = (*pDrvInst->busVTable->fpSfrRdStart)(pDrvInst, DRV_ENCX24J600_SFR_EIR, DRV_ENCX24J600_CI_OP_READ_EIR);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CI_WAIT_FOR_EIR;
                curSt->eirOp = ret;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CI_WAIT_FOR_EIR:
        {
            if (_DRV_ENCX24J600_ReadSfr(pDrvInst, curSt->eirOp, &reg, DRV_ENCX24J600_CI_OP_READ_EIR))
            {
                if (reg.eir.LINKIF == 1)
                {
                    pDrvInst->mainStateInfo.runningInfo.chkStaInfo.statusUpdateNeeded = true;
                }
                if (!((pDrvInst->rxDescriptors.state > DRV_ENCX24J600_RX_EMPTY_PACKET) && (pDrvInst->rxDescriptors.state < DRV_ENCX24J600_RX_WAIT_FOR_PICKUP)))
                {
                    if (!pDrvInst->rxPacketJustComplete)
                    {
                        if (reg.eir.PCFULIF == 1)
                        {
                            pDrvInst->rxPacketPending = true;
                        }
                        if (reg.eir.PKTIF == 1)
                        {
                            pDrvInst->rxPacketPending = true;
                        }
                        if (reg.eir.RXABTIF == 1)
                        {
                            pDrvInst->rxPacketPending = true;
                        }
                        if (!(reg.eir.PCFULIF == 1) && !(reg.eir.PKTIF == 1) && !(reg.eir.RXABTIF == 1))
                        {
                            pDrvInst->rxPacketPending = false;

                        }
                    }
                    else
                    {
                        pDrvInst->rxPacketJustComplete = false;
                    }
                }
                if (reg.eir.TXABTIF == 1)
                {
                    pDrvInst->mainStateInfo.runningInfo.chkTxStaInfo.statusUpdateNeeded = true;
                }
                if (reg.eir.TXIF == 1)
                {
                    pDrvInst->mainStateInfo.runningInfo.chkTxStaInfo.statusUpdateNeeded = true;
                }
                curSt->state = DRV_ENCX24J600_CI_WAIT;
            }
        }
    }

    return 0;
}
int32_t DRV_ENCX24J600_ChkIntStateEnter(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    DRV_ENCX24J600_CHECK_INT_INFO * curSt = &(pDrvInst->mainStateInfo.runningInfo.chkIntInfo);
    curSt->state = DRV_ENCX24J600_CI_WAIT;


    return 0;
}
int32_t DRV_ENCX24J600_ChkIntStateExit(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    return 0;
}
