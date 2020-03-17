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
/*****************************************************************************
 Copyright (C) 2015-2018 Microchip Technology Inc. and its subsidiaries.

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
