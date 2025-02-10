/*******************************************************************************
  ENC28J60 Initialization State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_initialization_state.c
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2015-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#include "drv_enc28j60_initialization_state.h"
#include "../drv_enc28j60_local.h"


int32_t DRV_ENC28J60_InitStateTask(struct S_DRV_ENC28J60_DriverInfo * pDrvInst)
{
    int32_t res;
    int32_t retVal = 0;

    switch (pDrvInst->mainStateInfo.initInfo.state)
    {
        case DRV_ENC28J60_IS_OPEN_BUS:
            res = (*pDrvInst->busVTable->fpOpenIf)(pDrvInst);
            if (res == 0)
            {
                pDrvInst->mainStateInfo.initInfo.state = DRV_ENC28J60_IS_DETECT;
                (void)DRV_ENC28J60_DetectStateEnter(pDrvInst);
            }
            break;

        case DRV_ENC28J60_IS_DETECT:
            res = DRV_ENC28J60_DetectStateTask(pDrvInst);
            if (res == 1)
            {
                pDrvInst->mainStateInfo.initInfo.state = DRV_ENC28J60_IS_RESET;
                (void)DRV_ENC28J60_DetectStateExit(pDrvInst);
                (void)DRV_ENC28J60_ResetStateEnter(pDrvInst);
            }
            break;

        case DRV_ENC28J60_IS_RESET:
            res = DRV_ENC28J60_ResetStateTask(pDrvInst);
            if (res == 1)
            {
                pDrvInst->mainStateInfo.initInfo.state = DRV_ENC28J60_IS_CONFIG;
                (void)DRV_ENC28J60_ResetStateExit(pDrvInst);
                (void)DRV_ENC28J60_ConfigStateEnter(pDrvInst);
            }
            break;

        case DRV_ENC28J60_IS_CONFIG:
            res = DRV_ENC28J60_ConfigStateTask(pDrvInst);
            if (res == 1)
            {
                (void)DRV_ENC28J60_ConfigStateExit(pDrvInst);
                retVal = 1;
            }
            break;

        default:
            // do nothing
            break;
    }

    return retVal;
}

int32_t DRV_ENC28J60_InitStateEnter(struct S_DRV_ENC28J60_DriverInfo * pDrvInst)
{
    pDrvInst->mainStateInfo.initInfo.state = DRV_ENC28J60_IS_OPEN_BUS;
    return 0;
}

int32_t DRV_ENC28J60_InitStateExit(struct S_DRV_ENC28J60_DriverInfo * pDrvInst)
{
    return 0;
}

