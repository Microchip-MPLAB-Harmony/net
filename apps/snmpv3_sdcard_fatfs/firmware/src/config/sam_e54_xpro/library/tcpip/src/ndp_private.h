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

