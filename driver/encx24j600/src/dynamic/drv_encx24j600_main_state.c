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
/*
Copyright (C) 2014-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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