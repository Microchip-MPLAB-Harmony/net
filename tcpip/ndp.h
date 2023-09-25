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
// Section: NDP Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* NDP Module Configuration Data Structure

  Summary:
    Placeholder for NDP module configuration data.

  Description:
    Provides a placeholder for NDP module configuration data.

  Remarks:
    None.
*/
typedef struct
{
    void*   reserved;
} TCPIP_NDP_MODULE_CONFIG;

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
