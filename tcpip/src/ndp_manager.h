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
    NDP_OPTION_TYPE_MTU =             5,
    //
    NDP_OPTION_TYPE_6LOWPAN_CONTEXT = 34,
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

typedef struct __attribute__((aligned(2), __packed__)) NDP_OPTION_PREFIX_INFO
{
    uint8_t vType;
    uint8_t vLength;
    uint8_t vPrefixLen;
    struct __attribute__((__packed__))
    {
        uint8_t bReserved1     :6;
        uint8_t bA             :1;
        uint8_t bL             :1;
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

typedef struct __attribute__((aligned(2), __packed__)) NDP_OPTION_6LoWPAN_CONTEXT
{
    uint8_t vType;
    uint8_t vLength;
    uint8_t vContextLen;
    struct __attribute__((__packed__))
    {
        uint8_t bRes            :3;
        uint8_t bC              :1;
        uint8_t bCID            :4;
    } flags;
    uint16_t dReserved;
    uint16_t dValidLifetime;
    uint8_t  dContextPrefix[64 / 8];    // G3-PLC context is 64 bits
} NDP_OPTION_6LoWPAN_CONTEXT;

typedef struct _IPV6_HEAP_NDP_NC_ENTRY
{
    struct _IPV6_HEAP_NDP_NC_ENTRY * next;
    IPV6_ADDR remoteIPAddress;
    TCPIP_MAC_ADDR remoteMACAddr;
    uint8_t reachabilityState;      // a NEIGHBOR_UNREACHABILITY_DETECT_STATE value
    uint8_t unansweredProbes;
    uint32_t nextNUDTime;
    SINGLE_LIST queuedPackets;
    uint32_t staleStateTimeout;

    union
    {
        uint8_t val;
        struct
        {
            uint8_t bIsRouter       : 1;
            uint8_t bResolvingAddress : 1;
            uint8_t bIsPerm         : 1;    // permanent entry
            uint8_t reserved        : 5;
        };
    }flags;
    IPV6_ADDR_STRUCT * preferredSource;
} IPV6_HEAP_NDP_NC_ENTRY;

typedef struct _IPV6_HEAP_NDP_DR_ENTRY
{
    struct _IPV6_HEAP_NDP_DR_ENTRY * next;
    IPV6_HEAP_NDP_NC_ENTRY * neighborInfo;
    uint32_t invalidationTimer;
    uint32_t tickTimer;
} IPV6_HEAP_NDP_DR_ENTRY;

typedef struct _IPV6_HEAP_NDP_DC_ENTRY
{
    struct _IPV6_HEAP_NDP_DC_ENTRY * next;
    IPV6_ADDR remoteIPAddress;
    uint32_t pathMTUIncreaseTimer;
    uint16_t pathMTU;
    IPV6_HEAP_NDP_NC_ENTRY * nextHopNeighbor;
} IPV6_HEAP_NDP_DC_ENTRY;

typedef struct _IPV6_HEAP_NDP_PL_ENTRY
{
    struct _IPV6_HEAP_NDP_PL_ENTRY * next;
    IPV6_ADDR prefix;
    uint32_t validLifetime;
    uint32_t lastTickTime;
    uint8_t prefixLength;
} IPV6_HEAP_NDP_PL_ENTRY;

typedef enum
{
    IPV6_HEAP_NDP_DR_ID = 0,            // default router
    IPV6_HEAP_NDP_NC_ID,                // neighbor cache
    IPV6_HEAP_NDP_DC_ID,                // destination cache
    IPV6_HEAP_NDP_PL_ID,                // prefix list
    IPV6_HEAP_NDP_AF_ID,                // not used
    IPV6_HEAP_ADDR_UNICAST_ID,          // Unicast local address
    IPV6_HEAP_ADDR_MULTICAST_ID,        // Multicast listener
    IPV6_HEAP_ADDR_UNICAST_TENTATIVE_ID,// Tentative local unicast address
} IPV6_HEAP_ENTRY_TYPE;

//************
// Stack APIs
//************

bool TCPIP_NDP_Initialize (const TCPIP_STACK_MODULE_CTRL* const stackInit,
                    const void* ndpData);
void TCPIP_NDP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackInit);

IPV6_HEAP_NDP_NC_ENTRY * TCPIP_NDP_NborEntryCreate (TCPIP_NET_IF * pNetIf, const IPV6_ADDR * remoteIPAddr, const TCPIP_MAC_ADDR * remoteMACAddr, uint8_t initialState, uint8_t routerFlag, IPV6_ADDR_STRUCT * preferredSource);
IPV6_HEAP_NDP_DR_ENTRY * TCPIP_NDP_DefaultRouterEntryCreate (TCPIP_NET_IF * pNetIf, IPV6_HEAP_NDP_NC_ENTRY * neighbor, uint32_t invalidationTime);
IPV6_HEAP_NDP_DC_ENTRY * TCPIP_NDP_DestCacheEntryCreate (TCPIP_NET_IF * pNetIf, const IPV6_ADDR * remoteIPAddress, uint32_t linkMTU, IPV6_HEAP_NDP_NC_ENTRY * neighbor);
IPV6_HEAP_NDP_PL_ENTRY * TCPIP_NDP_PrefixListEntryCreate (TCPIP_NET_IF * pNetIf, IPV6_ADDR * prefix, uint8_t prefixLength, uint32_t validLifetime);

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

#if defined(TCPIP_IPV6_G3_PLC_SUPPORT) && (TCPIP_IPV6_G3_PLC_SUPPORT != 0)
bool TCPIP_NDP_IsG3PLC_Neighbor(const TCPIP_NET_IF * pNetIf, const IPV6_ADDR * address, TCPIP_MAC_ADDR* pMacAdd);
#endif  // defined(TCPIP_IPV6_G3_PLC_SUPPORT) && (TCPIP_IPV6_G3_PLC_SUPPORT != 0)

#endif // _NDP_MANAGER_H

