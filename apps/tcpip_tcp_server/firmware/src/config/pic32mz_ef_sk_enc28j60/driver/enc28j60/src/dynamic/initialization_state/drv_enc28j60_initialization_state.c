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

