/*******************************************************************************
  DHCPV6 Manager Private API for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    dhcpv6_manager.h

  Summary:
    DHCP Manager Private API for Microchip TCP/IP Stack

  Description:
    This file provides the API definitions for the TCP/IP Stack DHCP Manager.

*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright � 2012 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED �AS IS� WITHOUT WARRANTY OF ANY KIND,
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

#ifndef _DHCPV6_MANAGER_H_
#define _DHCPV6_MANAGER_H_


// private stack API

bool        TCPIP_DHCPV6_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl,
                const TCPIP_DHCPV6_MODULE_CONFIG* pDhcpConfig);

void        TCPIP_DHCPV6_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData);


void        TCPIP_DHCPV6_ConnectionHandler(TCPIP_NET_IF* pNetIf, TCPIP_MAC_EVENT connEvent);



#endif  // _DHCPV6_MANAGER_H_



