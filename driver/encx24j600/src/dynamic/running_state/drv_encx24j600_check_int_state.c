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
