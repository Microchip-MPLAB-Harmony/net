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



