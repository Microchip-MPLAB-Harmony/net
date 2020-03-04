/*******************************************************************************
  ENC28J60 Check TX Status State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_check_tx_status_state.h
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2014-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/

// DOM-IGNORE-END
#ifndef _DRV_ENC28J60_CHECK_TX_STATUS_STATE_H_
#define _DRV_ENC28J60_CHECK_TX_STATUS_STATE_H_

#include "system_config.h"
#include <stdbool.h>
#include <stdint.h>

struct _DRV_ENC28J60_DriverInfo;

typedef enum
{
    DRV_ENC28J60_CTS_WAIT,
    DRV_ENC28J60_CTS_READ_PHSTAT2,
    DRV_ENC28J60_CTS_WAIT_PHSTAT2,
    DRV_ENC28J60_CTS_READ_ECON1,
    DRV_ENC28J60_CTS_WAIT_ECON1,
    
    DRV_ENC28J60_CTS_DONE,
}DRV_ENC28J60_CHECK_TX_STATUS_STATES;

typedef struct _DRV_ENC28J60_CHECK_TX_STATUS_INFO
{
    DRV_ENC28J60_CHECK_TX_STATUS_STATES state;
    bool txUpdateNeeded;
    uintptr_t   op;
    void* pTxLock;
}DRV_ENC28J60_CHECK_TX_STATUS_INFO;

int32_t DRV_ENC28J60_ChkTxStatusStateTask(struct _DRV_ENC28J60_DriverInfo * pDrvInst);
int32_t DRV_ENC28J60_ChkTxStatusStateEnter(struct _DRV_ENC28J60_DriverInfo * pDrvInst);
int32_t DRV_ENC28J60_ChkTxStatusStateExit(struct _DRV_ENC28J60_DriverInfo * pDrvInst);

#endif
