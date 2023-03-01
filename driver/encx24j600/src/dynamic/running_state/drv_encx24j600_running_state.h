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
