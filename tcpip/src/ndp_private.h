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

#ifndef _NDP_PRIVATE_H
#define _NDP_PRIVATE_H

//***************
// Private types
//***************
#define DUPLICATE_ADDR_DETECT_TRANSMITS         3
#define DUPLICATE_ADDR_DETECT_RETRIES           3
#define DUPLICATE_ADDR_DISCOVERY_THREADS        4

#define DAD_UNAVAILABLE                         -1
#define DAD_OK                                  0
#define DAD_ADDRESS_DUPLICATED                  1
#define DAD_PENDING                             2
#define DAD_BAD_ARGUMENT                        3

// Router advertisement constatnts
// G3-PLC support only
#define TCPIP_IPV6_MAX_INITIAL_RTR_ADVERTISEMENTS   3       // number
#define TCPIP_IPV6_MAX_INITIAL_RTR_ADVERT_INTERVAL  16      // seconds
// min/max router advertisement interval
#define TCPIP_IPV6_MIN_RTR_ADV_INTERVAL             200         // seconds
#define TCPIP_IPV6_MAX_RTR_ADV_INTERVAL             600         // seconds

//**************
// Private APIs
//**************
void * TCPIP_NDP_PrefixFind (TCPIP_NET_IF * pNetIf, IPV6_ADDR * prefix, uint8_t prefixLength, uint8_t usePrefixLength);

char TCPIP_NDP_DupAddrDiscoveryStatus (IPV6_ADDR_STRUCT * localAddressPointer);

bool TCPIP_NDP_AddressConstructFromPrefix (TCPIP_NET_IF * pNetIf, IPV6_ADDR * destination, IPV6_ADDR * prefix, uint8_t prefixLength);

IPV6_ADDR_STRUCT * TCPIP_NDP_TentativeAddressPromote (TCPIP_NET_IF * pNetIf, IPV6_ADDR_STRUCT * entryLocation);

void TCPIP_NDP_DestCacheUpdate (TCPIP_NET_IF * pNetIf, IPV6_HEAP_NDP_NC_ENTRY * neighborEntry);

#endif // _NDP_PRIVATE_H

