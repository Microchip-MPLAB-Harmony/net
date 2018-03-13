/*******************************************************************************
  ENC28J60 Change Duplex State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_change_duplex_state.c
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
#include "drv_enc28j60_change_duplex_state.h"
#include "../drv_enc28j60_local.h"
#include "drv_enc28j60_running_state.h"
#include "../drv_enc28j60_utils.h"


// ENC28J60 does not support auto HD/FD detection
// this file is not needed
int32_t DRV_ENC28J60_ChgDupStateTask(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
{
    return 0;
}

int32_t DRV_ENC28J60_ChgDupStateEnter(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
{
    return 0;
}

int32_t DRV_ENC28J60_ChgDupStateExit(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
{
    return 0;
}


