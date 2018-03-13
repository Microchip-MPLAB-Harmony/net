/*******************************************************************************
  ENCx24J600 Main State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_main_state.h
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

#ifndef _DRV_ENCX24J600_MAIN_STATE_H_
#define _DRV_ENCX24J600_MAIN_STATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "system_config.h"
#include "initialization_state/drv_encx24j600_initialization_state.h"
#include "closed_state/drv_encx24j600_closed_state.h"
#include "running_state/drv_encx24j600_running_state.h"

struct _DRV_ENCX24J600_DriverInfo;

typedef enum {
    DRV_ENCX24J600_MS_UNINITIALIZED,
    DRV_ENCX24J600_MS_INITIALIZATION,
    DRV_ENCX24J600_MS_CLOSED,
    DRV_ENCX24J600_MS_RUNNING,
    DRV_ENCX24J600_MS_CLOSING,
}DRV_ENCX24J600_MAIN_STATES;

typedef struct _DRV_ENCX24J600_MainStateInfo
{
    DRV_ENCX24J600_MAIN_STATES state;
    DRV_ENCX24J600_INITIALIZATION_STATE_INFO initInfo;
    DRV_ENCX24J600_RUNNING_STATE_INFO runningInfo;

}DRV_ENCX24J600_MainStateInfo;

int32_t DRV_ENCX24J600_MainStateTask(struct _DRV_ENCX24J600_DriverInfo * pDrvInst);


#ifdef __cplusplus
extern "C" }
#endif

#endif
