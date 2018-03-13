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
#include "drv_enc28j60_detect_state.h"
#include "../drv_enc28j60_local.h"
#include "../drv_enc28j60_utils.h"


// TODO aa: a retry counter should be implemented!
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
                // TODO aa: retry count needed!
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



