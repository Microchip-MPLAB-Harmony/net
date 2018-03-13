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
#ifndef _DRV_ENCX244J600_TX_PACKET_STATE_H_
#define _DRV_ENCX244J600_TX_PACKET_STATE_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "system_config.h"
#include "tcpip/tcpip_mac.h"

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
