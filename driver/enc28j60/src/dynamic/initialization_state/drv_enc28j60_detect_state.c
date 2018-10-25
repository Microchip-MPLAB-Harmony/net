/*******************************************************************************
  ENC28J60 Detect State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_detect_state.c
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
#include "drv_enc28j60_detect_state.h"
#include "../drv_enc28j60_local.h"
#include "../drv_enc28j60_utils.h"


int32_t DRV_ENC28J60_DetectStateTask(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
{
    uint16_t    phyID1Value;
	DRV_ENC28J60_PHY_RES phyRes;
    DRV_ENC28J60_RegUnion phyReg;
    DRV_ENC28J60_DETECT_STATE_INFO* pInfo = &pDrvInst->mainStateInfo.initInfo.detectStateInfo;

    switch (pInfo->state)
    {
        case DRV_ENC28J60_DS_PHID1_READ:
            phyRes = (*pDrvInst->busVTable->fpPhyRdStart)(pDrvInst, DRV_ENC28J60_PHY_SFR_PHID1);
            if(phyRes == DRV_ENC28J60_PHY_RES_PENDING)
            {   // advance
                pInfo->state = DRV_ENC28J60_DS_VERIFY_PHYID;
            }
            // retry
            break;

        case DRV_ENC28J60_DS_VERIFY_PHYID:
            phyRes = (*pDrvInst->busVTable->fpPhyRd)(pDrvInst, DRV_ENC28J60_PHY_SFR_PHID1, &phyID1Value);
            if(phyRes < 0)
            {   // some error; restart
               pInfo->state = DRV_ENC28J60_DS_PHID1_READ; 
                break;
            }
            else if(phyRes == DRV_ENC28J60_PHY_RES_PENDING)
            {   // wait some more
                break;
            }
            // success; check it's the proper ID
            if(phyID1Value != DRV_ENC28J60_PHY_ID1_VALUE)
            {   // failed; retry
                pInfo->state = DRV_ENC28J60_DS_PHID1_READ; 
                break;
            }

            // success; reset the PHY
            pInfo->state = DRV_ENC28J60_DS_RESET_PHY; 
            // no break
            
        case DRV_ENC28J60_DS_RESET_PHY:
            phyReg.value = 0;
            phyReg.phcon1.PRST = 1;
            phyRes = (*pDrvInst->busVTable->fpPhyWrStart)(pDrvInst, DRV_ENC28J60_PHY_SFR_PHCON1, phyReg.value);
            if(phyRes == DRV_ENC28J60_PHY_RES_PENDING)
            {   // advance
                pInfo->state = DRV_ENC28J60_DS_WAIT_RESET_PHY;
            }
            // else failed; retry
            break;

        case DRV_ENC28J60_DS_WAIT_RESET_PHY:
            phyRes = (*pDrvInst->busVTable->fpPhyWr)(pDrvInst, DRV_ENC28J60_PHY_SFR_PHCON1);
            if(phyRes == DRV_ENC28J60_PHY_RES_OK)
            {   // success
                pInfo->state = DRV_ENC28J60_DS_WAIT_FOR_DETECT_RESTART;
                return 1;
            }
            else if(phyRes < 0)
            {   // error: retry
                pInfo->state = DRV_ENC28J60_DS_RESET_PHY;
            }
            // else wait some more
            break;
            
        case DRV_ENC28J60_DS_WAIT_FOR_DETECT_RESTART:
            break;
    }

    return 0;
}

int32_t DRV_ENC28J60_DetectStateEnter(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
{
    pDrvInst->mainStateInfo.initInfo.detectStateInfo.state = DRV_ENC28J60_DS_PHID1_READ;
    return 0;
}

int32_t DRV_ENC28J60_DetectStateExit(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
{
    return 0;
}



