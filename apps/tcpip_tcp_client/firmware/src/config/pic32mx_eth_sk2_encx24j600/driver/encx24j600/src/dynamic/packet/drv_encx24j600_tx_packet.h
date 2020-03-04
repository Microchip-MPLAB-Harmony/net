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
