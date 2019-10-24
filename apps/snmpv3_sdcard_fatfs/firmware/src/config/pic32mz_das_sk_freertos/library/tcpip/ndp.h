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
/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

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