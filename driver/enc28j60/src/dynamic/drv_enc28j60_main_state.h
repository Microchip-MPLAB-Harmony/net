/*******************************************************************************
  ENC28J60 Main State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_main_state.h
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

#ifndef _DRV_ENC28J60_MAIN_STATE_H_
#define _DRV_ENC28J60_MAIN_STATE_H_



#include "system_config.h"
#include "initialization_state/drv_enc28j60_initialization_state.h"
#include "closed_state/drv_enc28j60_closed_state.h"
#include "running_state/drv_enc28j60_running_state.h"

struct _DRV_ENC28J60_DriverInfo;

typedef enum 
{
    DRV_ENC28J60_MS_UNINITIALIZED,
    DRV_ENC28J60_MS_INITIALIZATION,
    DRV_ENC28J60_MS_CLOSED,
    DRV_ENC28J60_MS_RUNNING,
    DRV_ENC28J60_MS_CLOSING
            
}DRV_ENC28J60_MAIN_STATES;

typedef struct _DRV_ENC28J60_MainStateInfo
{
    DRV_ENC28J60_MAIN_STATES state;
    DRV_ENC28J60_INITIALIZATION_STATE_INFO initInfo;
    DRV_ENC28J60_RUNNING_STATE_INFO runningInfo;

}DRV_ENC28J60_MainStateInfo;

int32_t DRV_ENC28J60_MainStateTask(struct _DRV_ENC28J60_DriverInfo * pDrvInst);


#endif
