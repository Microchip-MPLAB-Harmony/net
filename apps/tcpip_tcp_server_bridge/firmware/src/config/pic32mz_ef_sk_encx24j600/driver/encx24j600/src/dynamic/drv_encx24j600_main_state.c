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
/*****************************************************************************
 Copyright (C) 2014-2018 Microchip Technology Inc. and its subsidiaries.

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