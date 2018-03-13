/*******************************************************************************
  ENCx24J600 Initialization State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_initialization_state.h
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
#ifndef _DRV_ENCX24J600_INITIALIZATION_STATE_H_
#define _DRV_ENCX24J600_INITIALIZATION_STATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "system_config.h"
#include "drv_encx24j600_detect_state.h"
#include "drv_encx24j600_reset_state.h"
#include "drv_encx24j600_configure_state.h"

struct _DRV_ENCX24J600_DriverInfo;

typedef enum
{
    DRV_ENCX24J600_IS_OPEN_BUS,
    DRV_ENCX24J600_IS_DETECT,
    DRV_ENCX24J600_IS_RESET,
    DRV_ENCX24J600_IS_CONFIG,
}DRV_ENCX24J600_INITIALIZATION_STATES;

typedef struct _DRV_ENCX24J600_INITIALIZATION_STATE_INFO
{
    DRV_ENCX24J600_INITIALIZATION_STATES state;
    DRV_ENCX24J600_DETECT_STATE_INFO detectStateInfo;
    DRV_ENCX24J600_RESET_STATE_INFO resetStateInfo;
    DRV_ENCX24J600_CONFIGURE_STATE_INFO configStateInfo;
}DRV_ENCX24J600_INITIALIZATION_STATE_INFO;

int32_t DRV_ENCX24J600_InitStateTask(struct _DRV_ENCX24J600_DriverInfo * pDrvInst);
int32_t DRV_ENCX24J600_InitStateEnter(struct _DRV_ENCX24J600_DriverInfo * pDrvInst);
int32_t DRV_ENCX24J600_InitStateExit(struct _DRV_ENCX24J600_DriverInfo * pDrvInst);



#ifdef __cplusplus
}
#endif


#endif
