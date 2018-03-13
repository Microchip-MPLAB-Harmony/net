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
#include "drv_enc28j60_initialization_state.h"
#include "../drv_enc28j60_local.h"


int32_t DRV_ENC28J60_InitStateTask(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
{
    int32_t res;

    switch (pDrvInst->mainStateInfo.initInfo.state)
    {
        case DRV_ENC28J60_IS_OPEN_BUS:
            res = (*pDrvInst->busVTable->fpOpenIf)(pDrvInst);
            if (res == 0)
            {
                pDrvInst->mainStateInfo.initInfo.state = DRV_ENC28J60_IS_DETECT;
                DRV_ENC28J60_DetectStateEnter(pDrvInst);
            }
            break;

        case DRV_ENC28J60_IS_DETECT:
            res = DRV_ENC28J60_DetectStateTask(pDrvInst);
            if (res == 1)
            {
                pDrvInst->mainStateInfo.initInfo.state = DRV_ENC28J60_IS_RESET;
                DRV_ENC28J60_DetectStateExit(pDrvInst);
                DRV_ENC28J60_ResetStateEnter(pDrvInst);
            }
            break;
            
        case DRV_ENC28J60_IS_RESET:
            res = DRV_ENC28J60_ResetStateTask(pDrvInst);
            if (res == 1)
            {
                pDrvInst->mainStateInfo.initInfo.state = DRV_ENC28J60_IS_CONFIG;
                DRV_ENC28J60_ResetStateExit(pDrvInst);
                DRV_ENC28J60_ConfigStateEnter(pDrvInst);
            }
            break;
            
        case DRV_ENC28J60_IS_CONFIG:
            res = DRV_ENC28J60_ConfigStateTask(pDrvInst);
            if (res == 1)
            {
                DRV_ENC28J60_ConfigStateExit(pDrvInst);
                return 1;
            }
            break;
    }

    return 0;
}

int32_t DRV_ENC28J60_InitStateEnter(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
{
    pDrvInst->mainStateInfo.initInfo.state = DRV_ENC28J60_IS_OPEN_BUS;
    return 0;
}

int32_t DRV_ENC28J60_InitStateExit(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
{
    return 0;
}

