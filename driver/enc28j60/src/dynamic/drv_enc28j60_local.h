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
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.
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
#ifndef _DRV_ENC28J60_LOCAL_H_
#define _DRV_ENC28J60_LOCAL_H_


#include "system_config.h"
#include "../../drv_enc28j60.h"
#include "osal/osal.h"

#include "bus/drv_enc28j60_bus.h"
#include "drv_enc28j60_main_state.h"
#include "packet/drv_enc28j60_tx_packet.h"
#include "packet/drv_enc28j60_rx_packet.h"
#include "tcpip/src/tcpip_private.h"

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
    // TODO aa: wrong name!
    // it doesn't contain gthe open flags for FD/HD, maybe more!
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
