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