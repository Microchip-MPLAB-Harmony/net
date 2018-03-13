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
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.
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
#ifndef _DRV_ENC28J60_TX_PACKET_STATE_H_
#define _DRV_ENC28J60_TX_PACKET_STATE_H_


#include "system_config.h"
#include "tcpip/tcpip_mac.h"

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
