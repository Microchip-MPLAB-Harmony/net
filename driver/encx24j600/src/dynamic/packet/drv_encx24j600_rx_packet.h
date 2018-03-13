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
#ifndef _DRV_ENCX244J600_RX_PACKET_STATE_H_
#define _DRV_ENCX244J600_RX_PACKET_STATE_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "system_config.h"
#include "tcpip/tcpip_mac.h"
#include "../drv_encx24j600_ds_defs.h"

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
