/*******************************************************************************
 NDP Manager API Header File
 
  Company:
    Microchip Technology Inc.
    
  File Name:
    ndp_manager.h
    
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

#ifndef _NDP_MANAGER_H
#define _NDP_MANAGER_H

//************
// Stack types
//************
typedef enum
{
    NDP_STATE_FAILED = 0,
    NDP_STATE_NONE,
    NDP_STATE_INCOMPLETE,
    NDP_STATE_REACHABLE,
    NDP_STATE_STALE,
    NDP_STATE_DELAY,
    NDP_STATE_PROBE
} NEIGHBOR_UNREACHABILITY_DETECT_STATE;

typedef enum
{
    NDP_OPTION_TYPE_LLA_SOURCE =      1,
    NDP_OPTION_TYPE_LLA_TARGET =      2,
    NDP_OPTION_TYPE_PREFIX_INFO =     3,
    NDP_OPTION_TYPE_REDIRECT =        4,
    NDP_OPTION_TYPE_MTU =             5
} NDP_OPTION_TYPE;

typedef enum
{
    IPV6_NDP_DAD_NS_RECEIVED = 0,
    IPV6_NDP_DAD_NA_RECEIVED = 1
} IPV6_NDP_DAD_TYPE_RECEIVED;

typedef struct
{
    uint8_t vType;
    uint8_t vLength;
    TCPIP_MAC_ADDR mLinkLayerAddr;
} NDP_OPTION_LLA;

typedef struct __attribute__((__packed__))
{
    uint8_t vType;
    uint8_t vLength;
    uint8_t vPrefixLen;
    struct __attribute__((__packed__))
    {
        unsigned bReserved1     :6;
        unsigned bA             :1;
        unsigned bL             :1;
    } flags;
    uint32_t dValidLifetime;
    uint32_t dPreferredLifetime;
    uint32_t dReserved2;
    IPV6_ADDR aPrefix;
} NDP_OPTION_PREFIX_INFO;

typedef struct
{
    uint8_t vType;
    uint8_t vLength;
    uint16_t wReserved1;
    uint32_t dReserved2;
} NDP_OPTION_REDIRECTED;

typedef struct
{
    uint8_t vType;
    uint8_t vLength;
    uint16_t wReserved;
    uint32_t dMTU;
} NDP_OPTION_MTU;

typedef struct _IPV6_HEAP_NDP_NC_ENTRY
{
    struct _IPV6_HEAP_NDP_NC_ENTRY * next;
    IPV6_ADDR remoteIPAddress;
    TCPIP_MAC_ADDR remoteMACAddr;
    unsigned char reachabilityState;
    unsigned char unansweredProbes;
    unsigned long nextNUDTime;
    SINGLE_LIST queuedPackets;
    uint32_t staleStateTimeout;

    struct
    {
        unsigned bIsRouter     :1;
        unsigned bResolvingAddress : 1;
        unsigned Reserved        :6;
    } flags;
    IPV6_ADDR_STRUCT * preferredSource;
} IPV6_HEAP_NDP_NC_ENTRY;

typedef struct _IPV6_HEAP_NDP_DR_ENTRY
{
    struct _IPV6_HEAP_NDP_DR_ENTRY * next;
    IPV6_HEAP_NDP_NC_ENTRY * neighborInfo;
    unsigned long invalidationTimer;
    uint32_t tickTimer;
} IPV6_HEAP_NDP_DR_ENTRY;

typedef struct _IPV6_HEAP_NDP_DC_ENTRY
{
    struct _IPV6_HEAP_NDP_DC_ENTRY * next;
    IPV6_ADDR remoteIPAddress;
    unsigned long pathMTUIncreaseTimer;
    unsigned short pathMTU;
    IPV6_HEAP_NDP_NC_ENTRY * nextHopNeighbor;
} IPV6_HEAP_NDP_DC_ENTRY;

typedef struct _IPV6_HEAP_NDP_PL_ENTRY
{
    struct _IPV6_HEAP_NDP_PL_ENTRY * next;
    IPV6_ADDR prefix;
    unsigned long validLifetime;
    unsigned long lastTickTime;
    unsigned char prefixLength;
} IPV6_HEAP_NDP_PL_ENTRY;

typedef enum
{
    IPV6_HEAP_NDP_DR_ID = 0,
    IPV6_HEAP_NDP_NC_ID,
    IPV6_HEAP_NDP_DC_ID,
    IPV6_HEAP_NDP_PL_ID,
    IPV6_HEAP_NDP_AF_ID,
    IPV6_HEAP_ADDR_UNICAST_ID,
    IPV6_HEAP_ADDR_MULTICAST_ID,
    IPV6_HEAP_ADDR_UNICAST_TENTATIVE_ID,
} IPV6_HEAP_ENTRY_TYPE;

//************
// Stack APIs
//************

bool TCPIP_NDP_Initialize (const TCPIP_STACK_MODULE_CTRL* const stackInit,
                    const void* ndpData);
void TCPIP_NDP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackInit);

IPV6_HEAP_NDP_NC_ENTRY * TCPIP_NDP_NborEntryCreate (TCPIP_NET_IF * pNetIf, const IPV6_ADDR * remoteIPAddr, TCPIP_MAC_ADDR * remoteMACAddr, unsigned char initialState, unsigned char routerFlag, IPV6_ADDR_STRUCT * preferredSource);
IPV6_HEAP_NDP_DR_ENTRY * TCPIP_NDP_DefaultRouterEntryCreate (TCPIP_NET_IF * pNetIf, IPV6_HEAP_NDP_NC_ENTRY * neighbor, unsigned long invalidationTime);
IPV6_HEAP_NDP_DC_ENTRY * TCPIP_NDP_DestCacheEntryCreate (TCPIP_NET_IF * pNetIf, const IPV6_ADDR * remoteIPAddress, unsigned long linkMTU, IPV6_HEAP_NDP_NC_ENTRY * neighbor);
IPV6_HEAP_NDP_PL_ENTRY * TCPIP_NDP_PrefixListEntryCreate (TCPIP_NET_IF * pNetIf, IPV6_ADDR * prefix, unsigned char prefixLength, unsigned long validLifetime);

IPV6_HEAP_NDP_NC_ENTRY * TCPIP_NDP_NborEntryDelete (TCPIP_NET_IF * pNetIf, IPV6_HEAP_NDP_NC_ENTRY * entry);

void * TCPIP_NDP_RemoteNodeFind (TCPIP_NET_IF * pNetIf, const IPV6_ADDR * source, uint8_t type);

char TCPIP_NDP_DupAddrDiscoveryDetect (TCPIP_NET_IF * pNetIf, IPV6_ADDR_STRUCT * localAddressPointer);
void TCPIP_NDP_DupAddrDiscoveryProcess (IPV6_ADDR_STRUCT * localAddressPointer, uint8_t type);

void TCPIP_NDP_RouterSolicitStart (TCPIP_NET_IF * pNetIf);
void TCPIP_NDP_RouterSolicitStop (TCPIP_NET_IF * pNetIf);

void TCPIP_NDP_AddressResolve (IPV6_HEAP_NDP_NC_ENTRY * entry);
IPV6_HEAP_NDP_NC_ENTRY * TCPIP_NDP_NextHopGet (TCPIP_NET_IF * pNetIf, const IPV6_ADDR * address);
void TCPIP_NDP_ReachabilitySet (TCPIP_NET_IF * pNetIf, IPV6_HEAP_NDP_NC_ENTRY * neighborPointer, NEIGHBOR_UNREACHABILITY_DETECT_STATE newState);

void TCPIP_NDP_PrefixInfoProcessForOnLinkStatus (TCPIP_NET_IF * pNetIf, NDP_OPTION_PREFIX_INFO * prefixInfo);
void TCPIP_NDP_SAAPrefixInfoProcess (TCPIP_NET_IF * pNetIf, NDP_OPTION_PREFIX_INFO * prefixInfo);
uint8_t TCPIP_NDP_PrefixOnLinkStatusGet (TCPIP_NET_IF * pNetIf, const IPV6_ADDR * address);


void TCPIP_NDP_Task (void);

void TCPIP_NDP_LinkedListEntryInsert (TCPIP_NET_IF * pNetIf, void * entry, uint8_t type);
void * TCPIP_NDP_LinkedListEntryRemove (TCPIP_NET_IF * pNetIf, void * entry, uint8_t type);

IPV6_ADDR_STRUCT * TCPIP_NDP_UnicastAddressMove (TCPIP_NET_IF * pNetIf, IPV6_ADDR_STRUCT * entryLocation, IPV6_ADDR_STRUCT * previousEntryLocation);

void TCPIP_NDP_NborCacheLinkLayerAddressUpdate (TCPIP_NET_IF * pNetIf, IPV6_HEAP_NDP_NC_ENTRY * neighborPointer, TCPIP_MAC_ADDR * linkLayerAddr, uint8_t reachability);

IPV6_HEAP_NDP_DR_ENTRY * TCPIP_NDP_DefaultRouterGet (TCPIP_NET_IF * pNetIf);

#endif // _NDP_MANAGER_H

