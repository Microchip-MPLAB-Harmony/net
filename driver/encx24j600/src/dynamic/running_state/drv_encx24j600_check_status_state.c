/*******************************************************************************
  ENCx24J600 Check Status State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_check_status_state.c
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
#include "drv_encx24j600_check_status_state.h"
#include "../drv_encx24j600_local.h"
#include "../drv_encx24j600_utils.h"
#include "drv_encx24j600_running_state.h"


int32_t DRV_ENCX24J600_ChkStatusStateTask(struct S_DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    DRV_ENCX24J600_CHECK_STATUS_INFO * curSt = &(pDrvInst->mainStateInfo.runningInfo.chkStaInfo);
    DRV_ENCX24J600_RegUnion reg = {0};
    uintptr_t ret;
    switch(curSt->state)
    {
        case DRV_ENCX24J600_CS_WAIT:
            if (curSt->statusUpdateNeeded)
            {
                curSt->state = DRV_ENCX24J600_CS_READ_ESTAT;
            }
            break;  // For some reason the Interrupts aren't firing.

        case DRV_ENCX24J600_CS_READ_ESTAT:
            ret = (*pDrvInst->busVTable->fpSfrRdStart)(pDrvInst, DRV_ENCX24J600_SFR_ESTAT, DRV_ENCX24J600_CS_OP_READ_ESTAT);
            if (ret != 0U)
            {
                curSt->state = DRV_ENCX24J600_CS_WAIT_FOR_ESTAT;
                curSt->estatOp = ret;
            }
            break;

        case DRV_ENCX24J600_CS_WAIT_FOR_ESTAT:
            if (DRV_ENCX24J600_ReadSfr(pDrvInst, curSt->estatOp, &reg, (uint8_t)DRV_ENCX24J600_CS_OP_READ_ESTAT))
            {
                curSt->statusUpdateNeeded = false;
                if (reg.estat.PHYDPX == 1)
                {
                    if (pDrvInst->mainStateInfo.runningInfo.chgDupInfo.duplexMode != true)
                    {
                        pDrvInst->mainStateInfo.runningInfo.chgDupInfo.duplexMode = true;
                        pDrvInst->mainStateInfo.runningInfo.chgDupInfo.duplexChangeNeeded = true;
                    }
                }
                else
                {
                    if (pDrvInst->mainStateInfo.runningInfo.chgDupInfo.duplexMode == true)
                    {
                        pDrvInst->mainStateInfo.runningInfo.chgDupInfo.duplexMode = false;
                        pDrvInst->mainStateInfo.runningInfo.chgDupInfo.duplexChangeNeeded = true;
                    }
                }
                if (curSt->linkState != reg.estat.PHYLNK)
                {
                    curSt->linkState = reg.estat.PHYLNK != 0U;
                }
                if (reg.estat.PKTCNT != 0)
                {
                    //SYS_CONSOLE_PRINT("There are %d packets waiting \r\n", reg.estat.PKTCNT);
                    pDrvInst->rxPacketPending = true;

                }
                else
                {
                    pDrvInst->rxPacketPending = false;

                }
                curSt->state = DRV_ENCX24J600_CS_CLR_EIR;
            }
            break;

        case DRV_ENCX24J600_CS_CLR_EIR:
            reg.value = 0;
            reg.eir.LINKIF = 1;
            ret = (*pDrvInst->busVTable->fpSfrBitClr)(pDrvInst, DRV_ENCX24J600_SFR_EIR, reg, DRV_ENCX24J600_CS_OP_RST_EIR);
            if (ret != 0U)
            {
                curSt->state = DRV_ENCX24J600_CS_WAIT;
            }
            break;

        default:
            // do nothing
            break;

    }

    return 0;
}
int32_t DRV_ENCX24J600_ChkStatusStateEnter(struct S_DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    DRV_ENCX24J600_CHECK_STATUS_INFO * curSt = &(pDrvInst->mainStateInfo.runningInfo.chkStaInfo);
    curSt->state = DRV_ENCX24J600_CS_WAIT;
    curSt->statusUpdateNeeded = true;
    curSt->linkState = false;


    return 0;
}
int32_t DRV_ENCX24J600_ChkStatusStateExit(struct S_DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    return 0;
}
