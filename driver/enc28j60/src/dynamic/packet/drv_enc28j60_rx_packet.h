/*******************************************************************************
  ENC28J60 RX Packet and state machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_rx_packet.h
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
#ifndef _DRV_ENC28J60_RX_PACKET_STATE_H_
#define _DRV_ENC28J60_RX_PACKET_STATE_H_


#include "../drv_enc28j60_ds_defs.h"
#include "tcpip/tcpip_mac.h"
#include "system_config.h"


struct _DRV_ENC28J60_DriverInfo;


typedef enum
{
    DRV_ENC28J60_RX_EMPTY_PACKET,
    DRV_ENC28J60_RX_SET_ERDPTR,
    DRV_ENC28J60_RX_WAIT_ERDPTR,

    DRV_ENC28J60_RX_READ_RSV,
    DRV_ENC28J60_RX_WAIT_RSV,
    DRV_ENC28J60_RX_READ_PKT,
    DRV_ENC28J60_RX_WAIT_FOR_READ,
    DRV_ENC28J60_RX_PKTDEC,
    DRV_ENC28J60_RX_WAIT_FOR_PKTDEC,
    DRV_ENC28J60_RX_RESET_ERXRDPTR,
    DRV_ENC28J60_RX_RESET_WAIT,

}DRV_ENC28J60_RX_PACKET_STATES;

typedef struct _DRV_ENC28J60_RX_PACKET_INFO
{
    DRV_ENC28J60_RX_PACKET_STATES state;
    struct _DRV_ENC28J60_RX_PACKET_INFO * next;
    TCPIP_MAC_PACKET * macPkt;

    uintptr_t operation;

    uint8_t retry;
    uint8_t dummyData;  // extra byte for reading the RSV
    DRV_ENC28J60_RSV rsv;
    struct _DRV_ENC28J60_DriverInfo * pDrvInst;

}DRV_ENC28J60_RX_PACKET_INFO;
void    DRV_ENC28J60_RxPacketAck(TCPIP_MAC_PACKET* pkt,  const void* param);

int32_t DRV_ENC28J60_RxPacketTask(struct _DRV_ENC28J60_DriverInfo * pDrvInst, DRV_ENC28J60_RX_PACKET_INFO *pkt);
int32_t DRV_ENC28J60_RxPacketEnter(struct _DRV_ENC28J60_DriverInfo * pDrvInst, DRV_ENC28J60_RX_PACKET_INFO *pkt);
int32_t DRV_ENC28J60_RxPacketExit(struct _DRV_ENC28J60_DriverInfo * pDrvInst, DRV_ENC28J60_RX_PACKET_INFO *pkt);


#endif
