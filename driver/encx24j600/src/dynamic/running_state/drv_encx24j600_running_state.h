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
