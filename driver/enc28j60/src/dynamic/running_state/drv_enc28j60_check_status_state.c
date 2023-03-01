/*******************************************************************************
  ENC28J60 Check Status State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_check_status_state.c
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2015-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#include "drv_enc28j60_check_status_state.h"
#include "../drv_enc28j60_local.h"
#include "../drv_enc28j60_utils.h"
#include "drv_enc28j60_running_state.h"


int32_t DRV_ENC28J60_ChkStatusStateTask(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
{
    DRV_ENC28J60_CHECK_STATUS_INFO * curSt = &(pDrvInst->mainStateInfo.runningInfo.chkStaInfo);
    DRV_ENC28J60_PHY_RES    phyRes;
    DRV_ENC28J60_RegUnion   reg;

    switch(curSt->state)
    {
        case DRV_ENC28J60_CS_WAIT:
            if(curSt->linkUpdateNeeded)
            {
                curSt->state = DRV_ENC28J60_CS_READ_PHSTAT2;
            }
            else
            {
                break;  // For some reasont he Interrupts aren't firing.
            }

        case DRV_ENC28J60_CS_READ_PHSTAT2:
            phyRes = (*pDrvInst->busVTable->fpPhyRdStart)(pDrvInst, DRV_ENC28J60_PHY_SFR_PHSTAT2);
            if (phyRes >= 0)
            {   // success
                curSt->state = DRV_ENC28J60_CS_WAIT_READ_PHSTAT2;
            }
            // else retry
            break;

        case DRV_ENC28J60_CS_WAIT_READ_PHSTAT2:
            phyRes = (*pDrvInst->busVTable->fpPhyRd)(pDrvInst, DRV_ENC28J60_PHY_SFR_PHSTAT2, &reg.value);
            if(phyRes < 0)
            {   // some error; retry
                curSt->state = DRV_ENC28J60_CS_READ_PHSTAT2;
                break;
            }
            else if(phyRes == DRV_ENC28J60_PHY_RES_PENDING)
            {   // wait some more
                break;
            }

            // success
            curSt->linkState = reg.phstat2.LSTAT != 0;
            curSt->linkUpdateNeeded = false;
            curSt->state = DRV_ENC28J60_CS_READ_PHIR;
            // no break;

        case DRV_ENC28J60_CS_READ_PHIR:
            // read PHIR to clear PGIF, PLNKIF, EIR.LINKIF!
            phyRes = (*pDrvInst->busVTable->fpPhyRdStart)(pDrvInst, DRV_ENC28J60_PHY_SFR_PHIR);
            if (phyRes >= 0)
            {   // success
                curSt->state = DRV_ENC28J60_CS_WAIT_PHIR;
            }
            // else retry
            break;

        case DRV_ENC28J60_CS_WAIT_PHIR:
            phyRes = (*pDrvInst->busVTable->fpPhyRd)(pDrvInst, DRV_ENC28J60_PHY_SFR_PHIR, &reg.value);
            if(phyRes < 0)
            {   // some error; retry
                curSt->state = DRV_ENC28J60_CS_READ_PHIR;
                break;
            }
            else if(phyRes == DRV_ENC28J60_PHY_RES_PENDING)
            {   // wait some more
                break;
            }

            // success; done
            curSt->state = DRV_ENC28J60_CS_WAIT;
            break;

        default:
            break;
    }

    return 0;
}

int32_t DRV_ENC28J60_ChkStatusStateEnter(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
{
    DRV_ENC28J60_CHECK_STATUS_INFO * curSt = &(pDrvInst->mainStateInfo.runningInfo.chkStaInfo);
    curSt->state = DRV_ENC28J60_CS_WAIT;
    curSt->linkUpdateNeeded = true;
    curSt->linkState = false;
    return 0;
}

int32_t DRV_ENC28J60_ChkStatusStateExit(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
{
    return 0;
}

