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
#ifndef _DRV_ENCX24J600_LOCAL_H_
#define _DRV_ENCX24J600_LOCAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "system_config.h"
#include "../../drv_encx24j600.h"
#include "osal/osal.h"

#include "bus/drv_encx24j600_bus.h"
#include "drv_encx24j600_main_state.h"
#include "packet/drv_encx24j600_tx_packet.h"
#include "packet/drv_encx24j600_rx_packet.h"
#include "tcpip/src/link_list.h"

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
