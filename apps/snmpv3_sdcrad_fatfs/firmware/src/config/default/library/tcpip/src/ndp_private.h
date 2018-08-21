/*******************************************************************************
 NDP Private API Header File
 
  Company:
    Microchip Technology Inc.
    
  File Name:
    ndp_private.h
    
  Summary:

  Description:

*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2012 released Microchip Technology Inc.  All rights reserved.

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
// DOM-IGNORE-END

#ifndef _NDP_PRIVATE_H
#define _NDP_PRIVATE_H

//***************
// Private types
//***************
#define DUPLICATE_ADDR_DETECT_TRANSMITS         3
#define DUPLICATE_ADDR_DISCOVERY_THREADS        4

#define IPV6_INTERFACE_ID_SIZE                  64u

#define DAD_UNAVAILABLE                         -1
#define DAD_OK                                  0
#define DAD_ADDRESS_DUPLICATED                  1
#define DAD_PENDING                             2
#define DAD_BAD_ARGUMENT                        3

//**************
// Private APIs
//**************
void * TCPIP_NDP_PrefixFind (TCPIP_NET_IF * pNetIf, IPV6_ADDR * prefix, unsigned char prefixLength, unsigned char usePrefixLength);

char TCPIP_NDP_DupAddrDiscoveryStatus (IPV6_ADDR_STRUCT * localAddressPointer);

void TCPIP_NDP_AddressConstructFromPrefix (TCPIP_NET_IF * pNetIf, IPV6_ADDR * destination, IPV6_ADDR * prefix, unsigned char prefixLength);

IPV6_ADDR_STRUCT * TCPIP_NDP_TentativeAddressPromote (TCPIP_NET_IF * pNetIf, IPV6_ADDR_STRUCT * entryLocation);

void TCPIP_NDP_DestCacheUpdate (TCPIP_NET_IF * pNetIf, IPV6_HEAP_NDP_NC_ENTRY * neighborEntry);

#endif // _NDP_PRIVATE_H

