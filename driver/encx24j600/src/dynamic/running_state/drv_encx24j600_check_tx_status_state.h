/*******************************************************************************
  ENCx24J600 Check TX Status State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_check_tx_status_state.h
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
#ifndef _DRV_ENCX24J600_CHECK_TX_STATUS_STATE_H_
#define _DRV_ENCX24J600_CHECK_TX_STATUS_STATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "system_config.h"
#include <stdbool.h>
#include <stdint.h>

struct _DRV_ENCX24J600_DriverInfo;

typedef enum
{
    DRV_ENCX24J600_CTS_WAIT,
    DRV_ENCX24J600_CTS_READ_ETXSTAT,
    DRV_ENCX24J600_CTS_WAIT_FOR_ETXSTAT

}DRV_ENCX24J600_CHECK_TX_STATUS_STATES;

typedef struct _DRV_ENCX24J600_CHECK_TX_STATUS_INFO
{
    DRV_ENCX24J600_CHECK_TX_STATUS_STATES state;
    uintptr_t etxstatOp;
    bool statusUpdateNeeded;
    uint16_t status;
}DRV_ENCX24J600_CHECK_TX_STATUS_INFO;

int32_t DRV_ENCX24J600_ChkTxStatusStateTask(struct _DRV_ENCX24J600_DriverInfo * pDrvInst);
int32_t DRV_ENCX24J600_ChkTxStatusStateEnter(struct _DRV_ENCX24J600_DriverInfo * pDrvInst);
int32_t DRV_ENCX24J600_ChkTxStatusStateExit(struct _DRV_ENCX24J600_DriverInfo * pDrvInst);




#ifdef __cplusplus
}
#endif


#endif
