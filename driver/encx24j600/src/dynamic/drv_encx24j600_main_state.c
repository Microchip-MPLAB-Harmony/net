/*******************************************************************************
  ENCx24J600 Main State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_main_state.c
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

#include "drv_encx24j600_main_state.h"
#include "drv_encx24j600_local.h"


int32_t DRV_ENCX24J600_MainStateTask(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    switch (pDrvInst->mainStateInfo.state)
    {
        case DRV_ENCX24J600_MS_UNINITIALIZED:
            break;
        case DRV_ENCX24J600_MS_INITIALIZATION:
        {
            int32_t res = DRV_ENCX24J600_InitStateTask(pDrvInst);
            if (res == 1)
            {
                pDrvInst->mainStateInfo.state = DRV_ENCX24J600_MS_CLOSED;
                DRV_ENCX24J600_InitStateExit(pDrvInst);
                DRV_ENCX24J600_ClosedStateEnter(pDrvInst);
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_MS_CLOSED:
        {
            int32_t res = DRV_ENCX24J600_ClosedStateTask(pDrvInst);
            if (res == 1)
            {
                pDrvInst->mainStateInfo.state = DRV_ENCX24J600_MS_RUNNING;
                DRV_ENCX24J600_ClosedStateExit(pDrvInst);
                DRV_ENCX24J600_RunningStateEnter(pDrvInst);
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_MS_RUNNING:
        {
            DRV_ENCX24J600_RunningStateTask(pDrvInst);
            if (pDrvInst->numClients == 0)
            {
                pDrvInst->mainStateInfo.state = DRV_ENCX24J600_MS_CLOSING;
                DRV_ENCX24J600_RunningStateExit(pDrvInst);
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_MS_CLOSING:
            break;
    }
    return 0;


}