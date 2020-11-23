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
/*****************************************************************************
 Copyright (C) 2012-2020 Microchip Technology Inc. and its subsidiaries.

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



