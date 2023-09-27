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
#ifndef _DRV_ENC28J60_CHECK_INT_STATE_H_
#define _DRV_ENC28J60_CHECK_INT_STATE_H_

#include "system_config.h"
#include <stdint.h>

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
