/*******************************************************************************
  ENC28J60 Running State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_running_state.h
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2015-2020 Microchip Technology Inc. and its subsidiaries.

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
#ifndef _DRV_ENC28J60_RUNNING_STATE_H_
#define _DRV_ENC28J60_RUNNING_STATE_H_


#include "system_config.h"
#include "drv_enc28j60_check_int_state.h"
#include "drv_enc28j60_check_status_state.h"
#include "drv_enc28j60_change_duplex_state.h"
#include "drv_enc28j60_check_tx_status_state.h"
#include "drv_enc28j60_reset_rx_state.h"

struct _DRV_ENC28J60_DriverInfo;

typedef enum
{
    DRV_ENC28J60_CD_OP_SET_MACON=51,
    DRV_ENC28J60_CD_OP_SET_BBIPG,
    DRV_ENC28J60_TP_OP_READ_ECON1,
    DRV_ENC28J60_TP_OP_SET_ECON1,
    DRV_ENC28J60_TP_OP_SET_ETXSTL,
    DRV_ENC28J60_TP_OP_SET_ETXSTH,
    DRV_ENC28J60_TP_OP_SET_ETXNDL,
    DRV_ENC28J60_TP_OP_SET_ETXNDH,
	DRV_ENC28J60_TP_OP_SET_EWRPTL,
	DRV_ENC28J60_TP_OP_SET_EWRPTH,
    DRV_ENC28J60_TP_OP_RST_EIR,
    DRV_ENC28J60_TP_OP_SET_EIE,
    DRV_ENC28J60_RP_OP_SET_RXRDPTL,
    DRV_ENC28J60_RP_OP_SET_RXRDPTH,
    DRV_ENC28J60_RP_OP_SET_ERDPTL,
    DRV_ENC28J60_RP_OP_SET_ERDPTH,
    DRV_ENC28J60_RP_OP_SET_RXND,
    DRV_ENC28J60_RP_OP_SET_ECON2,
    DRV_ENC28J60_CS_OP_SET_MICMD,
	DRV_ENC28J60_CS_OP_READ_EPKTCNT,
	DRV_ENC28J60_TP_OP_READ_EWRPTL,
	DRV_ENC28J60_TP_OP_READ_EWRPTH,
	DRV_ENC28J60_CI_OP_READ_EIR,
	DRV_ENC28J60_RRX_OP_SET_RXRST,
	DRV_ENC28J60_RRX_OP_SET_ERXSTL,
	DRV_ENC28J60_RRX_OP_SET_ERXSTH,
	DRV_ENC28J60_RRX_OP_SET_ERXRDPTL,
	DRV_ENC28J60_RRX_OP_SET_ERXRDPTH,
	DRV_ENC28J60_RRX_OP_SET_ERXNDL,
	DRV_ENC28J60_RRX_OP_SET_ERXNDH,
    DRV_ENC28J60_RRX_OP_DEC_PKT_CNT = DRV_ENC28J60_RRX_OP_SET_ERXSTL,
    DRV_ENC28J60_RRX_OP_CLEAR_RXRST,
    DRV_ENC28J60_RRX_OP_SET_RXST = DRV_ENC28J60_RRX_OP_CLEAR_RXRST,
    DRV_ENC28J60_RRX_OP_READ_EPKCNT,
    DRV_ENC28J60_RRX_OP_RXEN = DRV_ENC28J60_RRX_OP_READ_EPKCNT,
	    
}DRV_ENC28J60_RUNNING_STATE_OPS;

typedef struct _DRV_ENC28J60_RUNNING_STATE_INFO
{
    bool ctsToEnc;
    bool ctTx;
    bool ctrFromEnc;

    DRV_ENC28J60_CHECK_INT_INFO chkIntInfo;
    DRV_ENC28J60_CHECK_STATUS_INFO chkStaInfo;
    DRV_ENC28J60_CHANGE_DUPLEX_INFO chgDupInfo;
    DRV_ENC28J60_CHECK_TX_STATUS_INFO chkTxStaInfo;
	DRV_ENC28J60_RESET_RX_INFO resetRxInfo;
    int nTxOkPackets;
    int nRxOkPackets;
}DRV_ENC28J60_RUNNING_STATE_INFO;


int32_t DRV_ENC28J60_RunningStateTask( struct _DRV_ENC28J60_DriverInfo * pDrvInst);
int32_t DRV_ENC28J60_RunningStateEnter(struct _DRV_ENC28J60_DriverInfo * pDrvInst);
int32_t DRV_ENC28J60_RunningStateExit(struct _DRV_ENC28J60_DriverInfo * pDrvInst);





#endif
