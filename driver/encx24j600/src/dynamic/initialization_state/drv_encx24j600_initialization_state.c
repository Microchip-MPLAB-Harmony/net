/*******************************************************************************
  ENCx24J600 Initialization State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_initialization_state.c
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
#include "drv_encx24j600_initialization_state.h"
#include "../drv_encx24j600_local.h"


int32_t DRV_ENCX24J600_InitStateTask(struct S_DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    int32_t retRes = 0;
    int32_t res;
    switch (pDrvInst->mainStateInfo.initInfo.state)
    {
        case DRV_ENCX24J600_IS_OPEN_BUS:
            res = (*pDrvInst->busVTable->fpOpenIf)(pDrvInst);
            if (res == 0)
            {
                pDrvInst->mainStateInfo.initInfo.state = DRV_ENCX24J600_IS_DETECT;
                (void)DRV_ENCX24J600_DetectStateEnter(pDrvInst);
            }
            break;
        case DRV_ENCX24J600_IS_DETECT:
            res = DRV_ENCX24J600_DetectStateTask(pDrvInst);
            if (res == 1)
            {
                pDrvInst->mainStateInfo.initInfo.state = DRV_ENCX24J600_IS_RESET;
                (void)DRV_ENCX24J600_DetectStateExit(pDrvInst);
                (void)DRV_ENCX24J600_ResetStateEnter(pDrvInst);
            }
            break;
        case DRV_ENCX24J600_IS_RESET:
            res = DRV_ENCX24J600_ResetStateTask(pDrvInst);
            if (res == 1)
            {
                pDrvInst->mainStateInfo.initInfo.state = DRV_ENCX24J600_IS_CONFIG;
                (void)DRV_ENCX24J600_ResetStateExit(pDrvInst);
                (void)DRV_ENCX24J600_ConfigStateEnter(pDrvInst);
            }
            break;
        case DRV_ENCX24J600_IS_CONFIG:
            res = DRV_ENCX24J600_ConfigStateTask(pDrvInst);
            if (res == 1)
            {
                (void)DRV_ENCX24J600_ConfigStateExit(pDrvInst);
                retRes = 1;
            }
            break;
        default:
            // do nothing
            break;
    }
    return retRes;
}

int32_t DRV_ENCX24J600_InitStateEnter(struct S_DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    pDrvInst->mainStateInfo.initInfo.state = DRV_ENCX24J600_IS_OPEN_BUS;
    return 0;
}

int32_t DRV_ENCX24J600_InitStateExit(struct S_DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    return 0;
}

