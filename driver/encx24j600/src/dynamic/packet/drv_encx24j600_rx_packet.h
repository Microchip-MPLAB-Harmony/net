/*******************************************************************************
  ENCx24J600 RX Packet and state machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_rx_packet.h
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
#ifndef _DRV_ENCX244J600_RX_PACKET_STATE_H_
#define _DRV_ENCX244J600_RX_PACKET_STATE_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "../drv_encx24j600_ds_defs.h"
#include "tcpip/tcpip_mac.h"
#include "system_config.h"
    
struct _DRV_ENCX24J600_DriverInfo;


typedef enum
{
    DRV_ENCX24J600_RX_EMPTY_PACKET,
    DRV_ENCX24J600_RX_SET_ERXRDPTR,
    DRV_ENCX24J600_RX_READ_RSV,
    DRV_ENCX24J600_RX_WAIT_FOR_RSV,
    DRV_ENCX24J600_RX_READ_PKT,
    DRV_ENCX24J600_RX_WAIT_FOR_READ,
    DRV_ENCX24J600_RX_RESET_ERXRDPTR,
    DRV_ENCX24J600_RX_SET_ERXTAIL,
    DRV_ENCX24J600_RX_SEND_PKTDEC,
    DRV_ENCX24J600_RX_RST_EIR,
    DRV_ENCX24J600_RX_WAIT_FOR_PICKUP,
    DRV_ENCX24J600_RX_WAIT_FOR_ACK,

}DRV_ENCX24J600_RX_PACKET_STATES;

typedef struct _DRV_ENCX24J600_RX_PACKET_INFO
{
    DRV_ENCX24J600_RX_PACKET_STATES state;
    struct _DRV_ENCX24J600_RX_PACKET_INFO * next;
    TCPIP_MAC_PACKET * pkt;

    uintptr_t operation;

    uint8_t dummyData;  // Do not move from the RSV.
    DRV_ENCx24J600_RSV rsv;
    uint8_t retry;
    struct _DRV_ENCX24J600_DriverInfo * pDrvInst;

}DRV_ENCX24J600_RX_PACKET_INFO;
void    DRV_ENCX24J600_RxPacketAck(TCPIP_MAC_PACKET* pkt,  const void* param);

int32_t DRV_ENCX24J600_RxPacketTask(struct _DRV_ENCX24J600_DriverInfo * pDrvInst, DRV_ENCX24J600_RX_PACKET_INFO *pkt);
int32_t DRV_ENCX24J600_RxPacketEnter(struct _DRV_ENCX24J600_DriverInfo * pDrvInst, DRV_ENCX24J600_RX_PACKET_INFO *pkt);
int32_t DRV_ENCX24J600_RxPacketExit(struct _DRV_ENCX24J600_DriverInfo * pDrvInst, DRV_ENCX24J600_RX_PACKET_INFO *pkt);



#ifdef __cplusplus
}
#endif

#endif
