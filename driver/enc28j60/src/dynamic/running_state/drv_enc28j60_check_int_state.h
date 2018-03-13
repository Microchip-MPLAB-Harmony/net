/*******************************************************************************
  ENC28J60 Check Interrupt State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_check_int_state.h
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
#ifndef _DRV_ENC28J60_CHECK_INT_STATE_H_
#define _DRV_ENC28J60_CHECK_INT_STATE_H_

#include "system_config.h"

struct _DRV_ENC28J60_DriverInfo;

typedef enum
{
    DRV_ENC28J60_CI_WAIT,
    DRV_ENC28J60_CI_READ_EIR,
    DRV_ENC28J60_CI_WAIT_FOR_EIR,
    DRV_ENC28J60_CI_READ_PKTCNT,
	DRV_ENC28J60_CI_WAIT_FOR_PKTCNT,

}DRV_ENC28J60_CHECK_INT_STATES;

typedef struct _DRV_ENC28J60_CHECK_INT_INFO
{
    DRV_ENC28J60_CHECK_INT_STATES state;
    uintptr_t op;
}DRV_ENC28J60_CHECK_INT_INFO;

int32_t DRV_ENC28J60_ChkIntStateTask(struct _DRV_ENC28J60_DriverInfo * pDrvInst);
int32_t DRV_ENC28J60_ChkIntStateEnter(struct _DRV_ENC28J60_DriverInfo * pDrvInst);
int32_t DRV_ENC28J60_ChkIntStateExit(struct _DRV_ENC28J60_DriverInfo * pDrvInst);



#endif
