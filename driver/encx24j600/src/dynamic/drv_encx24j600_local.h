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
