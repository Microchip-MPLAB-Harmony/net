/*******************************************************************************
  ENCx24J600 TX Packet and State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_tx_packet.h
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
#ifndef _DRV_ENCX244J600_TX_PACKET_STATE_H_
#define _DRV_ENCX244J600_TX_PACKET_STATE_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "tcpip/tcpip_mac.h"
#include "system_config.h"


struct _DRV_ENCX24J600_DriverInfo;


typedef enum
{
    DRV_ENCX24J600_TP_NO_PKT_STATE,
    DRV_ENCX24J600_TP_WAIT_FOR_CTS_TO_ENC,
    DRV_ENCX24J600_TP_READ_GPWRREG,
    DRV_ENCX24J600_TP_SND_PKT_TO_ENC,
    DRV_ENCX24J600_TP_WAIT_FOR_CTTX,
    DRV_ENCX24J600_TP_WAIT_FOR_GPWRREG,
    DRV_ENCX24J600_TP_SET_ETXST,
    DRV_ENCX24J600_TP_SET_ETXLEN,
    DRV_ENCX24J600_TP_RQ_PKT_TX,
    DRV_ENCX24J600_TP_WAIT_FOR_COMPLETE,
    DRV_ENCX24J600_TP_RST_EIR
}DRV_ENCX24J600_TX_PACKET_STATES;

typedef struct _DRV_ENCX24J600_TX_PACKET_INFO
{
    DRV_ENCX24J600_TX_PACKET_STATES state;
    TCPIP_MAC_PACKET * pkt;
    struct _DRV_ENCX24J600_TX_PACKET_INFO * next;
    uint16_t gpPtr;
    TCPIP_MAC_DATA_SEGMENT*         pDSeg;
    uintptr_t operation;
}DRV_ENCX24J600_TX_PACKET_INFO;

int32_t DRV_ENCX24J600_TxPacketTask(struct _DRV_ENCX24J600_DriverInfo * pDrvInst, DRV_ENCX24J600_TX_PACKET_INFO *pkt);
int32_t DRV_ENCX24J600_TxPacketEnter(struct _DRV_ENCX24J600_DriverInfo * pDrvInst, DRV_ENCX24J600_TX_PACKET_INFO *pkt);
int32_t DRV_ENCX24J600_TxPacketExit(struct _DRV_ENCX24J600_DriverInfo * pDrvInst, DRV_ENCX24J600_TX_PACKET_INFO *pkt);



#ifdef __cplusplus
}
#endif

#endif
