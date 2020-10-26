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
/*****************************************************************************
 Copyright (C) 2014-2018 Microchip Technology Inc. and its subsidiaries.

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
bool    DRV_ENCX24J600_RxPacketAck(TCPIP_MAC_PACKET* pkt,  const void* param);

int32_t DRV_ENCX24J600_RxPacketTask(struct _DRV_ENCX24J600_DriverInfo * pDrvInst, DRV_ENCX24J600_RX_PACKET_INFO *pkt);
int32_t DRV_ENCX24J600_RxPacketEnter(struct _DRV_ENCX24J600_DriverInfo * pDrvInst, DRV_ENCX24J600_RX_PACKET_INFO *pkt);
int32_t DRV_ENCX24J600_RxPacketExit(struct _DRV_ENCX24J600_DriverInfo * pDrvInst, DRV_ENCX24J600_RX_PACKET_INFO *pkt);



#ifdef __cplusplus
}
#endif

#endif
