/*******************************************************************************
 ICMPv6 Manager API Header File
 
  Company:
    Microchip Technology Inc.
    
  File Name:
    icmpv6_manager.h
    
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

#ifndef _ICMPV6_MANAGER_H
#define _ICMPV6_MANAGER_H

//*************
// Stack types
//*************
#define ICMPV6_CHECKSUM_OFFSET                          2u

typedef struct __attribute__((aligned(2), __packed__))
{
    uint8_t vType;
    uint8_t vCode;
    uint16_t wChecksum;
    uint32_t Reserved;
} ICMPV6_HEADER_ROUTER_SOLICITATION;

typedef struct __attribute__((aligned(2), __packed__))
{
    uint8_t vType;
    uint8_t vCode;
    uint16_t wChecksum;
    uint8_t curHopLimit;
    struct __attribute__((__packed__))
    {
        unsigned Reserved : 6;
        unsigned O : 1;
        unsigned M : 1;
    } flags;
    uint16_t routerLifetime;
    uint32_t reachableTime;
    uint32_t retransTime;
} ICMPV6_HEADER_ROUTER_ADVERTISEMENT;

typedef struct  __attribute__((aligned(2), __packed__)) ICMPV6_HEADER_NEIGHBOR_SOLICITATION
{
    uint8_t vType;
    uint8_t vCode;
    uint16_t wChecksum;
    uint32_t Reserved;
    IPV6_ADDR aTargetAddress;
} ICMPV6_HEADER_NEIGHBOR_SOLICITATION;

typedef struct  __attribute__((aligned(2), __packed__)) ICMPV6_HEADER_NEIGHBOR_ADVERTISEMENT
{
    uint8_t vType;
    uint8_t vCode;
    uint16_t wChecksum;
    union
    {
        struct __attribute__((__packed__))
        {
            unsigned Reserved : 5;
            unsigned O : 1;
            unsigned S : 1;
            unsigned R : 1;
        } bits;
        uint8_t Val;
    } flags;
    unsigned char Reserved1;
    unsigned short Reserved2;
    IPV6_ADDR aTargetAddress;
} ICMPV6_HEADER_NEIGHBOR_ADVERTISEMENT;

typedef struct __attribute__((aligned(2), __packed__)) ICMPV6_HEADER_REDIRECT
{
    uint8_t vType;
    uint8_t vCode;
    uint16_t wChecksum;
    uint32_t Reserved;
    IPV6_ADDR aTargetAddress;
    IPV6_ADDR aDestinationAddress;
} ICMPV6_HEADER_REDIRECT;

typedef union
{
    ICMPV6_HEADER_ERROR                         header_Error;
    ICMPV6_HEADER_ECHO                          header_Echo;
    ICMPV6_HEADER_ROUTER_SOLICITATION           header_RS;
    ICMPV6_HEADER_ROUTER_ADVERTISEMENT          header_RA;
    ICMPV6_HEADER_NEIGHBOR_SOLICITATION         header_NS;
    ICMPV6_HEADER_NEIGHBOR_ADVERTISEMENT        header_NA;
    ICMPV6_HEADER_REDIRECT                      header_Rd;
} ICMPV6_HEADER_TYPES;

//*************
// Stack APIs
//*************
bool TCPIP_ICMPV6_Initialize (const TCPIP_STACK_MODULE_CTRL* const stackInit,
                       const void* icmpv6Data);
void TCPIP_ICMPV6_Deinitialize (const TCPIP_STACK_MODULE_CTRL* const stackInit);

void TCPIP_ICMPV6_Process(TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt, IPV6_ADDR_STRUCT * localIPStruct, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, uint16_t dataLen, uint16_t headerLen, uint8_t hopLimit, uint8_t addrType);

IPV6_PACKET * TCPIP_ICMPV6_HeaderErrorPut (TCPIP_NET_IF * pNetIf, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, uint8_t code, uint8_t type, uint32_t additionalData);
IPV6_PACKET * TCPIP_ICMPV6_HeaderRouterSolicitationPut (TCPIP_NET_IF * pNetIf, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP);
IPV6_PACKET * TCPIP_ICMPV6_HeaderNeighborSolicitationPut (TCPIP_NET_IF * pNetIf, IPV6_ADDR * localIP, IPV6_ADDR * remoteIP, IPV6_ADDR * targetAddr);
IPV6_PACKET * TCPIP_ICMPV6_HeaderNeighborAdvertisementPut (TCPIP_NET_IF * pNetIf, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, IPV6_ADDR * targetAddr, bool solicited, bool override);

IPV6_PACKET * TCPIP_ICMPV6_Open (const TCPIP_NET_IF * pNetIf, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP);


#if defined(TCPIP_IPV6_G3_PLC_BORDER_ROUTER) && (TCPIP_IPV6_G3_PLC_BORDER_ROUTER != 0)
bool    TCPIP_ICMPV6_G3RouterAdvertisementPut (const TCPIP_NET_IF * pNetIf, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, const IPV6_ADDR_STRUCT* advIP);
void    TCPIP_ICMPV6_G3AdvertisementSelect(TCPIP_NET_IF * pNetIf, const IPV6_ADDR_STRUCT** pLclStruct, const IPV6_ADDR_STRUCT** pAdvStruct);
#endif // defined(TCPIP_IPV6_G3_PLC_BORDER_ROUTER) && (TCPIP_IPV6_G3_PLC_BORDER_ROUTER != 0)

#endif

