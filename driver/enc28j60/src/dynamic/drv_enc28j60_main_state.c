/*******************************************************************************
  ENC28J60 Main State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_main_state.c
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

#include "drv_enc28j60_main_state.h"
#include "drv_enc28j60_local.h"


int32_t DRV_ENC28J60_MainStateTask(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
{
    switch (pDrvInst->mainStateInfo.state)
    {
        case DRV_ENC28J60_MS_UNINITIALIZED:
            break;
        case DRV_ENC28J60_MS_INITIALIZATION:
        {
            int32_t res = DRV_ENC28J60_InitStateTask(pDrvInst);
            if (res == 1)
            {
                pDrvInst->mainStateInfo.state = DRV_ENC28J60_MS_CLOSED;
                DRV_ENC28J60_InitStateExit(pDrvInst);
                DRV_ENC28J60_ClosedStateEnter(pDrvInst);
            }
            else
            {
                break;
            }
        }
        case DRV_ENC28J60_MS_CLOSED:
        {
            int32_t res = DRV_ENC28J60_ClosedStateTask(pDrvInst);
            if (res == 1)
            {
                pDrvInst->mainStateInfo.state = DRV_ENC28J60_MS_RUNNING;
                DRV_ENC28J60_ClosedStateExit(pDrvInst);
                DRV_ENC28J60_RunningStateEnter(pDrvInst);
            }
            else
            {
                break;
            }
        }
        case DRV_ENC28J60_MS_RUNNING:
        {
            DRV_ENC28J60_RunningStateTask(pDrvInst);
            if (pDrvInst->numClients == 0)
            {
                pDrvInst->mainStateInfo.state = DRV_ENC28J60_MS_CLOSING;
                DRV_ENC28J60_RunningStateExit(pDrvInst);
            }
            else
            {
                break;
            }
        }
        case DRV_ENC28J60_MS_CLOSING:
            break;
    }
    return 0;


}