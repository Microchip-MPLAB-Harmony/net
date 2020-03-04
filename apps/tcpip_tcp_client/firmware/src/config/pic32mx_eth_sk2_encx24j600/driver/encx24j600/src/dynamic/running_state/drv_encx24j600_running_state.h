/*******************************************************************************
  ENCx24J600 Running State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_running_state.h
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2014-2020 Microchip Technology Inc. and its subsidiaries.

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
#ifndef _DRV_ENCX24J600_RUNNING_STATE_H_
#define _DRV_ENCX24J600_RUNNING_STATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "system_config.h"
#include "drv_encx24j600_check_int_state.h"
#include "drv_encx24j600_check_status_state.h"
#include "drv_encx24j600_change_duplex_state.h"
#include "drv_encx24j600_check_tx_status_state.h"
#include "drv_encx24j600_reset_rx_state.h"
    
struct _DRV_ENCX24J600_DriverInfo;

typedef enum
{
    DRV_ENCX24J600_CI_OP_READ_EIR,
    DRV_ENCX24J600_CS_OP_READ_ESTAT,
    DRV_ENCX24J600_CS_OP_RST_EIR,
    DRV_ENCX24J600_CD_OP_SET_MACON2,
    DRV_ENCX24J600_CD_OP_SET_BBIPG,
    DRV_ENCX24J600_CTS_OP_READ_ETXSTAT,
    DRV_ENCX24J500_TP_OP_READ_GPWRREG,
    DRV_ENCX24J600_TP_OP_SET_ETXST = DRV_ENCX24J500_TP_OP_READ_GPWRREG,
    DRV_ENCX24J600_TP_OP_SET_ETXLEN,
    DRV_ENCX24J600_TP_OP_RST_EIR,
    DRV_ENCX24J600_RP_OP_SET_RXRDPTR,
    DRV_ENCX24J600_RP_OP_SET_RXTAIL = DRV_ENCX24J600_RP_OP_SET_RXRDPTR,
    DRV_ENCX24J600_RP_OP_RST_EIR,
    DRV_ENCX24J600_RRX_OP_SET_RXRST,
    DRV_ENCX24J600_RRX_OP_DEC_PKT_CNT = DRV_ENCX24J600_RRX_OP_SET_RXRST,
    DRV_ENCX24J600_RRX_OP_CLEAR_RXRST,
    DRV_ENCX24J600_RRX_OP_SET_RXST = DRV_ENCX24J600_RRX_OP_CLEAR_RXRST,
    DRV_ENCX24J600_RRX_OP_READ_ESTAT,
    DRV_ENCX24J600_RRX_OP_RXEN = DRV_ENCX24J600_RRX_OP_READ_ESTAT,
}DRV_ENCX24J600_RUNNING_STATE_OPS;

typedef struct _DRV_ENCX24J600_RUNNING_STATE_INFO
{
    bool ctsToEnc;
    bool ctTx;
    bool ctrFromEnc;

    DRV_ENCX24J600_CHECK_INT_INFO chkIntInfo;
    DRV_ENCX24J600_CHECK_STATUS_INFO chkStaInfo;
    DRV_ENCX24J600_CHANGE_DUPLEX_INFO chgDupInfo;
    DRV_ENCX24J600_CHECK_TX_STATUS_INFO chkTxStaInfo;
    DRV_ENCX24J600_RESET_RX_INFO resetRxInfo;
    int nTxOkPackets;
    int nRxOkPackets;
}DRV_ENCX24J600_RUNNING_STATE_INFO;


int32_t DRV_ENCX24J600_RunningStateTask(struct _DRV_ENCX24J600_DriverInfo * pDrvInst);
int32_t DRV_ENCX24J600_RunningStateEnter(struct _DRV_ENCX24J600_DriverInfo * pDrvInst);
int32_t DRV_ENCX24J600_RunningStateExit(struct _DRV_ENCX24J600_DriverInfo * pDrvInst);




#ifdef __cplusplus
}
#endif


#endif
