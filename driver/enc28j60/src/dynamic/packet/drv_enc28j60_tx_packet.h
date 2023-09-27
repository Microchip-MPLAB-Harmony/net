/*******************************************************************************
  ENC28J0 TX Packet and State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_tx_packet.h
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
#ifndef _DRV_ENC28J60_TX_PACKET_STATE_H_
#define _DRV_ENC28J60_TX_PACKET_STATE_H_



#include "tcpip/tcpip_mac.h"
#include "system_config.h"

struct _DRV_ENC28J60_DriverInfo;


typedef enum
{
    DRV_ENC28J60_TP_NO_PKT_STATE,
    // sending the packet to the ENC
    DRV_ENC28J60_TP_WRITE_TXST,
    DRV_ENC28J60_TP_WAIT_TXST,
    DRV_ENC28J60_TP_SET_ETXND,
    DRV_ENC28J60_TP_WAIT_ETXND,
    DRV_ENC28J60_TP_SET_WRPT,
    DRV_ENC28J60_TP_WAIT_WRPT,
    DRV_ENC28J60_TP_SEND_PKT,
    DRV_ENC28J60_TP_SEND_PKT_WAIT,
    DRV_ENC28J60_TP_READ_EIR,
    DRV_ENC28J60_TP_WAIT_EIR,
    DRV_ENC28J60_TP_SET_TXRST,
    DRV_ENC28J60_TP_WAIT_SET_TXRST,
    DRV_ENC28J60_TP_CLR_TXRST,
    DRV_ENC28J60_TP_WAIT_CLR_TXRST,
    DRV_ENC28J60_TP_CLR_TXERIF,
    DRV_ENC28J60_TP_WAIT_CLR_TXERIF,
    // transmitting the packet
    DRV_ENC28J60_TP_READY_FOR_CTTX,
    DRV_ENC28J60_TP_WAIT_FOR_CTTX,
    DRV_ENC28J60_TP_RQ_PKT_TX,
    DRV_ENC28J60_TP_WAIT_PKT_TX,
    DRV_ENC28J60_TP_WAIT_FOR_COMPLETE,
    DRV_ENC28J60_TP_RST_EIR,
    DRV_ENC28J60_TP_CLR_EIR,
    DRV_ENC28J60_TP_WAIT_CLR_EIR,
    DRV_ENC28J60_TP_RESET_TX,
    DRV_ENC28J60_TP_RESET_DONE,
    
}DRV_ENC28J60_TX_PACKET_STATES;

typedef struct _DRV_ENC28J60_TX_PACKET_INFO
{
    struct _DRV_ENC28J60_TX_PACKET_INFO * next;
    DRV_ENC28J60_TX_PACKET_STATES state;
    TCPIP_MAC_PACKET * macPkt;
    TCPIP_MAC_DATA_SEGMENT*         pDSeg;
    uintptr_t           operation;
    uint16_t            gpPtr;
    uint16_t            pktLen;     // current packet length
}DRV_ENC28J60_TX_PACKET_INFO;

int32_t DRV_ENC28J60_TxPacketTask(struct _DRV_ENC28J60_DriverInfo * pDrvInst, DRV_ENC28J60_TX_PACKET_INFO *pkt);
int32_t DRV_ENC28J60_TxPacketEnter(struct _DRV_ENC28J60_DriverInfo * pDrvInst, DRV_ENC28J60_TX_PACKET_INFO *pkt);
int32_t DRV_ENC28J60_TxPacketExit(struct _DRV_ENC28J60_DriverInfo * pDrvInst, DRV_ENC28J60_TX_PACKET_INFO *pkt);



#endif
