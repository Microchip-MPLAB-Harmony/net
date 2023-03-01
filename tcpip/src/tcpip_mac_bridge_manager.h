/*******************************************************************************
  Bridge Manager Private API for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_mac_bridge_manager.h

  Summary:
    Bridge Manager Private API for Microchip TCP/IP Stack

  Description:
    This file provides the API definitions for the TCP/IP Stack Bridge Manager.

*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2012-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef _TCPIP_MAC_BRIDGE_MANAGER_H_
#define _TCPIP_MAC_BRIDGE_MANAGER_H_


// private stack API


// standard initialization function
bool TCPIP_MAC_Bridge_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_MAC_BRIDGE_CONFIG* pBConfig);


void TCPIP_MAC_Bridge_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl);

// MAC bridging packet processing result

typedef enum
{
    TCPIP_MAC_BRIDGE_PKT_RES_HOST_PROCESS   = 0,        // the host should process this packet
    TCPIP_MAC_BRIDGE_PKT_RES_BRIDGE_DISCARD,            // the packet was discarded by the bridge
                                                        // the host should ignore the packet and not process
    TCPIP_MAC_BRIDGE_PKT_RES_BRIDGE_PROCESS,            // the packet is processed by the bridge
                                                        // the host should ignore the packet and not process


}TCPIP_MAC_BRIDGE_PKT_RES;


// bridge processing of an incoming packet
// returns:
//      - TCPIP_MAC_BRIDGE_PKT_RES_HOST_PROCESS:
//              - normal packet processing should occur
//                (the bridge does not handle the packet)
//      - TCPIP_MAC_BRIDGE_PKT_RES_BRIDGE_DISCARD:
//              the stack should ignore the packet
//              was discarded by the bridge too
//
//      - TCPIP_MAC_BRIDGE_PKT_RES_BRIDGE_PROCESS:
//              all processing was done by the bridge
//              the stack should ignore the packet
//
TCPIP_MAC_BRIDGE_PKT_RES    TCPIP_MAC_Bridge_ProcessPacket(TCPIP_MAC_PACKET* pRxPkt);



#endif  // _TCPIP_MAC_BRIDGE_MANAGER_H_



