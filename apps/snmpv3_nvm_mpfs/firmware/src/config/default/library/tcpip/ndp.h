/*******************************************************************************
  Neighbor Discovery Protocol (NDP) API Header File

  Company:
    Microchip Technology Inc.

  File Name:
    ndp.h
	
  Summary:
    IPv6 Internet Communication Message Neighbor Discovery Protocol (NDP).

  Description:
    Neighbor Discovery Protocol (NDP) in IPv6 is the substitute of
    as ARP (which is used in IPv4 for address resolve). NDP is used discover link local
    addresses of the IPv6 nodes present in the local link using a mix of ICMPv6
    messages and multicast addresses, stateless auto-configuration and router redirection.
	
*******************************************************************************/
//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2012-2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
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
//DOM-IGNORE-END

#ifndef _NDP_H
#define _NDP_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************

//*****************************************************************************
/*
  Function:
    void TCPIP_NDP_NborReachConfirm (TCPIP_NET_HANDLE netH, IPV6_ADDR * address)

  Summary:
    Confirms that a neighbor is reachable.

  Description:
    This function is used by upper-layer protocols to indicate that round-trip
    communications were confirmed with a neighboring node.

  Precondition:
    None.

  Parameters:
    pNetIf - The interface the neighbor is on.
    address - The address of the neighbor.

  Returns:
    None.

  Remarks:
    None.
	
*/
void TCPIP_NDP_NborReachConfirm (TCPIP_NET_HANDLE netH, const IPV6_ADDR * address);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // _NDP_H