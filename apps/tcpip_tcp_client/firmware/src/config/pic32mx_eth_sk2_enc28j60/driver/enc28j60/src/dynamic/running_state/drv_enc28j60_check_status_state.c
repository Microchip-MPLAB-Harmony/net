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
/*****************************************************************************
 Copyright (C) 2015-2018 Microchip Technology Inc. and its subsidiaries.

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

