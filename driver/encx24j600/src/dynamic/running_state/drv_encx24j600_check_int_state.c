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
