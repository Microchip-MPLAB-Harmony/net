/*******************************************************************************
  ENCx24J600 Check Status State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_check_status_state.h
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
#ifndef _DRV_ENCX24J600_CHECK_STATUS_STATE_H_
#define _DRV_ENCX24J600_CHECK_STATUS_STATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "system_config.h"

struct _DRV_ENCX24J600_DriverInfo;

typedef enum
{
    DRV_ENCX24J600_CS_WAIT,
    DRV_ENCX24J600_CS_READ_ESTAT,
    DRV_ENCX24J600_CS_WAIT_FOR_ESTAT,
    DRV_ENCX24J600_CS_CLR_EIR

}DRV_ENCX24J600_CHECK_STATUS_STATES;

typedef struct _DRV_ENCX24J600_CHECK_STATUS_INFO
{
    DRV_ENCX24J600_CHECK_STATUS_STATES state;
    uintptr_t estatOp;
    bool statusUpdateNeeded;
    bool linkState;

}DRV_ENCX24J600_CHECK_STATUS_INFO;

int32_t DRV_ENCX24J600_ChkStatusStateTask(struct _DRV_ENCX24J600_DriverInfo * pDrvInst);
int32_t DRV_ENCX24J600_ChkStatusStateEnter(struct _DRV_ENCX24J600_DriverInfo * pDrvInst);
int32_t DRV_ENCX24J600_ChkStatusStateExit(struct _DRV_ENCX24J600_DriverInfo * pDrvInst);




#ifdef __cplusplus
}
#endif


#endif
