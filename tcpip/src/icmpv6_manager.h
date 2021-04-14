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

typedef struct  __attribute__((aligned(2), __packed__))
{
    uint8_t vType;
    uint8_t vCode;
    uint16_t wChecksum;
    uint32_t Reserved;
    IPV6_ADDR aTargetAddress;
} ICMPV6_HEADER_NEIGHBOR_SOLICITATION;

typedef struct  __attribute__((aligned(2), __packed__))
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

typedef struct __attribute__((aligned(2), __packed__))
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
IPV6_PACKET * TCPIP_ICMPV6_HeaderRouterSolicitationPut (TCPIP_NET_IF * pNetIf, IPV6_ADDR * localIP, IPV6_ADDR * remoteIP);
IPV6_PACKET * TCPIP_ICMPV6_HeaderNeighborSolicitationPut (TCPIP_NET_IF * pNetIf, IPV6_ADDR * localIP, IPV6_ADDR * remoteIP, IPV6_ADDR * targetAddr);
IPV6_PACKET * TCPIP_ICMPV6_HeaderNeighborAdvertisementPut (TCPIP_NET_IF * pNetIf, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, IPV6_ADDR * targetAddr, bool solicited, bool override);

IPV6_PACKET * TCPIP_ICMPV6_Open (TCPIP_NET_IF * pNetIf, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP);


#endif

