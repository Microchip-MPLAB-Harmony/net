/*******************************************************************************
  ENCx24J600 Driver Local Interface
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_local.h
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2014-2020 Microchip Technology Inc. and its subsidiaries.

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
#ifndef _DRV_ENCX24J600_LOCAL_H_
#define _DRV_ENCX24J600_LOCAL_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "packet/drv_encx24j600_tx_packet.h"
#include "packet/drv_encx24j600_rx_packet.h"
#include "drv_encx24j600_main_state.h"
#include "bus/drv_encx24j600_bus.h"
#include "driver/encx24j600/drv_encx24j600.h"
#include "tcpip/src/link_list.h"
#include "osal/osal.h"
#include "system_config.h"

#define MAX_TX_DESCRIPTORS 2
#define MAX_RX_DESCRIPTORS 1

typedef struct _DRV_ENCX24J600_DriverInfo
{
    bool inUse;
    /* The mutex to protect the */
    OSAL_MUTEX_HANDLE_TYPE  drvMutex;
    /* A copy of the driver configuration*/
    DRV_ENCX24J600_Configuration drvCfg;
    /* A copy of the stack configuration*/
    TCPIP_MAC_MODULE_CTRL stackCfg;
    bool stackCfgReady;

    // Client information
    bool exclusiveMode;
    uint8_t numClients;

    // Driver info
    const DRV_ENCX24J600_BusVTable * busVTable;
    void * busData;

    DRV_ENCX24J600_MainStateInfo mainStateInfo;

    uint16_t encMemRxStart;
    uint16_t encMemRxEnd;
    uint16_t encMemGpStart;
    uint16_t encMemGpEnd;
    uint16_t encMemUdaStart;
    uint16_t encMemUdaEnd;

    TCPIP_MAC_PARAMETERS stackParameters;
    TCPIP_MAC_EVENT eventMask;
    TCPIP_MAC_EVENT currentEvents;

    uint16_t gpPtrVal;
    uint16_t rxPtrVal;
    uint16_t txBufferRemaining;

    bool rxPacketPending;
    bool rxPacketJustComplete;

    DRV_ENCX24J600_TX_PACKET_INFO txDescriptors[MAX_TX_DESCRIPTORS];
    DRV_ENCX24J600_RX_PACKET_INFO rxDescriptors;
    PROTECTED_SINGLE_LIST rxFreePackets;
    PROTECTED_SINGLE_LIST rxWaitingForPickupPackets;
    PROTECTED_SINGLE_LIST txPendingPackets;


}DRV_ENCX24J600_DriverInfo;

typedef struct _DRV_ENCX24J600_ClientInfo
{
    bool inUse;
    DRV_ENCX24J600_DriverInfo * pDrvInst;
}DRV_ENCX24J600_ClientInfo;

#ifdef __cplusplus
}
#endif
#endif
