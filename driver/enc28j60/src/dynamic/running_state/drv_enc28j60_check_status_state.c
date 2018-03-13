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
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.
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

