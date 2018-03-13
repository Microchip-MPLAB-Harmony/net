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
/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.
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
#include "drv_encx24j600_initialization_state.h"
#include "../drv_encx24j600_local.h"


int32_t DRV_ENCX24J600_InitStateTask(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    switch (pDrvInst->mainStateInfo.initInfo.state)
    {
        case DRV_ENCX24J600_IS_OPEN_BUS:
        {
            int32_t res = (*pDrvInst->busVTable->fpOpenIf)(pDrvInst);
            if (res == 0)
            {
                pDrvInst->mainStateInfo.initInfo.state = DRV_ENCX24J600_IS_DETECT;
                DRV_ENCX24J600_DetectStateEnter(pDrvInst);
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_IS_DETECT:
        {
            int32_t res = DRV_ENCX24J600_DetectStateTask(pDrvInst);
            if (res == 1)
            {
                pDrvInst->mainStateInfo.initInfo.state = DRV_ENCX24J600_IS_RESET;
                DRV_ENCX24J600_DetectStateExit(pDrvInst);
                DRV_ENCX24J600_ResetStateEnter(pDrvInst);
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_IS_RESET:
        {
            int32_t res = DRV_ENCX24J600_ResetStateTask(pDrvInst);
            if (res == 1)
            {
                pDrvInst->mainStateInfo.initInfo.state = DRV_ENCX24J600_IS_CONFIG;
                DRV_ENCX24J600_ResetStateExit(pDrvInst);
                DRV_ENCX24J600_ConfigStateEnter(pDrvInst);
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_IS_CONFIG:
        {
            int32_t res = DRV_ENCX24J600_ConfigStateTask(pDrvInst);
            if (res == 1)
            {
                DRV_ENCX24J600_ConfigStateExit(pDrvInst);
                return 1;
            }
        }
            break;
    }
    return 0;
}

int32_t DRV_ENCX24J600_InitStateEnter(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    pDrvInst->mainStateInfo.initInfo.state = DRV_ENCX24J600_IS_OPEN_BUS;
    return 0;
}

int32_t DRV_ENCX24J600_InitStateExit(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    return 0;
}

