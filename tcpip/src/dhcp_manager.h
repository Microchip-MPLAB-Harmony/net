/*******************************************************************************
  DHCP Manager Private API for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    dhcp_manager.h

  Summary:
    DHCP Manager Private API for Microchip TCP/IP Stack

  Description:
    This file provides the API definitions for the TCP/IP Stack DHCP Manager.

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

#ifndef _DHCP_MANAGER_H_
#define _DHCP_MANAGER_H_


// private stack API

bool        TCPIP_DHCP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl,
                const TCPIP_DHCP_MODULE_CONFIG* pDhcpConfig);

void        TCPIP_DHCP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData);


void        TCPIP_DHCP_ConnectionHandler(TCPIP_NET_IF* pNetIf, TCPIP_MAC_EVENT connEvent);



#endif  // _DHCP_MANAGER_H_



