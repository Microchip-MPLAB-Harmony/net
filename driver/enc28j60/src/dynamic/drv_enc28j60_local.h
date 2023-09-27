/*******************************************************************************
  ENC28J60 Driver Local Interface
  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_local.h
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
#ifndef _DRV_ENC28J60_LOCAL_H_
#define _DRV_ENC28J60_LOCAL_H_

#include "packet/drv_enc28j60_tx_packet.h"
#include "packet/drv_enc28j60_rx_packet.h"
#include "drv_enc28j60_main_state.h"
#include "bus/drv_enc28j60_bus.h"
#include "driver/enc28j60/drv_enc28j60.h"
#include "tcpip/src/tcpip_private.h"
#include "osal/osal.h"
#include "system_config.h"





#define MAX_TX_DESCRIPTORS 1
#define MAX_RX_DESCRIPTORS 1

// ENCJ60 debug levels
#define DRV_ENC28J60_DEBUG_MASK_BASIC           (0x0001)

// enable ENCJ60 debugging levels
#define DRV_ENC28J60_DEBUG_LEVEL  0

typedef struct _DRV_ENC28J60_DriverInfo
{
    bool inUse;
    /* The mutex to protect the */
    OSAL_MUTEX_HANDLE_TYPE  drvMutex;
    /* A copy of the driver configuration*/
    DRV_ENC28J60_Configuration drvCfg;
    /* A copy of the stack configuration*/
    TCPIP_MAC_MODULE_CTRL stackCfg;
    bool stackCfgReady;

    // Client information
    bool exclusiveMode;
    uint8_t numClients;

    // Driver info
    const DRV_ENC28J60_BusVTable * busVTable;
    void * busData;

    DRV_ENC28J60_MainStateInfo mainStateInfo;

    uint16_t encMemRxStart;
    uint16_t encMemRxEnd;
    uint16_t encMemRdPtr;
    uint16_t encMemRxRdPtr;
    uint16_t encMemRxWrPtr;

    uint16_t encRamForAppSize;
    uint16_t encRamForAppStartAdr;
    uint16_t encMemTxStart;
    uint16_t encMemTxEnd;
    uint16_t encMemWrPtr;
    uint16_t encMemGpEnd;

    uint8_t encMacAdr5;
    uint8_t encMacAdr6;
    uint8_t encMacAdr3;
    uint8_t encMacAdr4;
    uint8_t encMacAdr1; 
    uint8_t encMacAdr2;
                        
    uint8_t encMacon1;
    uint8_t encMacon3;
    uint8_t encMacon4;
    
    
    TCPIP_MAC_PARAMETERS stackParameters;
    TCPIP_MAC_EVENT eventMask;
    TCPIP_MAC_EVENT currentEvents;

    uint16_t txPtrVal;
    uint16_t rxPtrVal;
    uint16_t txBufferRemaining;

    bool rxPacketPending;

    uintptr_t txStateMachineOp;
    uintptr_t rxStateMachineOp;

    DRV_ENC28J60_TX_PACKET_INFO txDescriptors[MAX_TX_DESCRIPTORS];
    DRV_ENC28J60_RX_PACKET_INFO rxDescriptors;
    PROTECTED_SINGLE_LIST rxFreePackets;
    PROTECTED_SINGLE_LIST rxWaitingForPickupPackets;
    PROTECTED_SINGLE_LIST txPendingPackets;

}DRV_ENC28J60_DriverInfo;

typedef struct _DRV_ENC28J60_ClientInfo
{
    bool inUse;
    DRV_ENC28J60_DriverInfo * pDrvInst;
}DRV_ENC28J60_ClientInfo;


// ENCJ60 debugging
//
void DRV_ENC28J60_Assert(bool cond, const char* message, int lineNo);


#endif
