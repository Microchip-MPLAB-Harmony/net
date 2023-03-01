/*******************************************************************************
  ENCx24J600 Change Duplex State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_change_duplex_state.c
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
#include "drv_encx24j600_change_duplex_state.h"
#include "../drv_encx24j600_local.h"
#include "drv_encx24j600_running_state.h"
#include "../drv_encx24j600_utils.h"


int32_t DRV_ENCX24J600_ChgDupStateTask(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    DRV_ENCX24J600_CHANGE_DUPLEX_INFO * curSt = &pDrvInst->mainStateInfo.runningInfo.chgDupInfo;
    DRV_ENCX24J600_RegUnion reg = {0};
    uintptr_t ret;

    switch (curSt->state)
    {
        case DRV_ENCX24J600_CD_WAIT:
        {
            if (curSt->duplexChangeNeeded)
            {
                curSt->state = DRV_ENCX24J600_CD_SET_MACON2;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CD_SET_MACON2:
        {
            reg.value = 0;
            reg.macon2.FULDPX = 1;
            if (curSt->duplexMode)
            {
                ret = (pDrvInst->busVTable->fpSfrBitSet)(pDrvInst, DRV_ENCX24J600_SFR_MACON2, reg, DRV_ENCX24J600_CD_OP_SET_MACON2);
            }
            else
            {
                ret = (pDrvInst->busVTable->fpSfrBitClr)(pDrvInst, DRV_ENCX24J600_SFR_MACON2, reg, DRV_ENCX24J600_CD_OP_SET_MACON2);
            }
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CD_SET_BBIPG;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CD_SET_BBIPG:
        {
            reg.value = 0;
            if (curSt->duplexMode)
            {
                reg.mabbipg.BBIPG = 0x15;
            }
            else
            {
                reg.mabbipg.BBIPG = 0x12;
            }
            ret = (*pDrvInst->busVTable->fpSfrWr)(pDrvInst, DRV_ENCX24J600_SFR_MABBIPG, reg, DRV_ENCX24J600_CD_OP_SET_BBIPG);
            if (ret != 0)
            {
                curSt->state = DRV_ENCX24J600_CD_ENABLE_RX;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_CD_ENABLE_RX:
        {
            ret = (*pDrvInst->busVTable->fpRxEnable)(pDrvInst);
            if (ret != 0)
            {
                curSt->duplexChangeNeeded = false;
                curSt->state = DRV_ENCX24J600_CD_WAIT;
            }
        }
    }

    return 0;
}

int32_t DRV_ENCX24J600_ChgDupStateEnter(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    DRV_ENCX24J600_CHANGE_DUPLEX_INFO * curSt = &pDrvInst->mainStateInfo.runningInfo.chgDupInfo;
    curSt->state = DRV_ENCX24J600_CD_WAIT;
    curSt->duplexChangeNeeded = true;
    curSt->duplexMode = false;
    return 0;
}

int32_t DRV_ENCX24J600_ChgDupStateExit(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    return 0;
}


