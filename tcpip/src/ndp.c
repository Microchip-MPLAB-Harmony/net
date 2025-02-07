/*******************************************************************************
  Neighbor Discovery Protocol (NDP)
*******************************************************************************/

/*
Copyright (C) 2012-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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








#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_NDP

#include "tcpip/src/tcpip_private.h"
#include "tcpip/src/ndp_private.h"


#if defined (TCPIP_STACK_USE_IPV6)


static IPV6_ADDR tempAddress;

#define DAD_STATE_INACTIVE          0U
#define DAD_STATE_TRANSMIT          1U
#define DAD_STATE_TRANSMIT_WAIT     2U
#define DAD_STATE_FAIL              3U
#define DAD_STATE_RETRY_WAIT        4U
#define DAD_STATE_DONE              5U
typedef struct
{
    IPV6_ADDR_STRUCT * addressPointer;
    uint32_t transmitTimer;
    TCPIP_NET_IF * netConfig;
    uint8_t state;      // one of the DAD_STATE_ values
    uint8_t transmitAttempt;
    uint8_t receivedSolicitations;
#if (TCPIP_IPV6_RIID_ENABLED != 0)    
    uint8_t retryCount;
    uint32_t retryTimer;
#endif // (TCPIP_IPV6_RIID_ENABLED != 0)    
} DAD_STATIC_VARS;


static DAD_STATIC_VARS gDuplicateAddrDetectState[DUPLICATE_ADDR_DISCOVERY_THREADS];

#define RS_STATE_INACTIVE   0U
#define RS_STATE_TRANSMIT   1U
#define RS_STATE_WAIT       2U
typedef struct
{
    uint32_t timer;
    uint8_t state;          // one of the RS_STATE_ values
    uint8_t transmitAttempt;
    IPV6_ADDR_STRUCT * address;
} RS_STATIC_VARS;

static TCPIP_SIGNAL_HANDLE     ndpAsyncHandle = NULL;      // handle for the DAD task

static int                  ndpDADCount = 0;            // Count of current DAD tasks
static int                  ndpNUDCount = 0;            // Count of current NUD tasks
static int                  ndpRSCount = 0;             // Count of current RS tasks

static RS_STATIC_VARS* gRSState = NULL;
static uint32_t gInitialDelay;
static bool     gSetInitialDelay = false;

static const void*      ndpMemH = NULL;        // memory handle
static int              ndpInitCount = 0;
static size_t           nStackIfs = 0U;      // max number of interfaces

static uint32_t         sysFreq = 0U;       // current running system frequency

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void F_TCPIP_NDP_Cleanup(void);
#else
#define F_TCPIP_NDP_Cleanup()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static uint32_t F_TCPIP_NDP_RandomSolicitationDelayGet(void);

#if (TCPIP_IPV6_RIID_ENABLED != 0)    
static bool F_TCPIP_NDP_InterfaceIdReserved(IPV6_ADDR_STRUCT* tentativeAddressPointer);
static bool F_TCPIP_NDP_InterfaceIdInUse(TCPIP_NET_IF * pNetIf, IPV6_ADDR_STRUCT* tentativeAddressPointer);
#endif  // (TCPIP_IPV6_RIID_ENABLED != 0)    

static void TCPIP_NDP_DupAddrDiscoveryTask (void);

static void TCPIP_NDP_RouterSolicitTask (void);

static void TCPIP_NDP_NborUnreachDetectTask (void);

static void * TCPIP_NDP_PrefixFind (const TCPIP_NET_IF * pNetIf, IPV6_ADDR * prefix, uint8_t prefixLength, bool usePrefixList);

static uint8_t TCPIP_NDP_DupAddrDiscoveryStatus (IPV6_ADDR_STRUCT * localAddressPointer);

static bool TCPIP_NDP_AddressConstructFromPrefix (const TCPIP_NET_IF * pNetIf, IPV6_ADDR * destination, IPV6_ADDR * prefix, uint8_t prefixLength);

static IPV6_ADDR_STRUCT * TCPIP_NDP_TentativeAddressPromote (const TCPIP_NET_IF * pNetIf, IPV6_ADDR_STRUCT * entryLocation);

static void TCPIP_NDP_DestCacheUpdate (const TCPIP_NET_IF * pNetIf, IPV6_HEAP_NDP_NC_ENTRY * neighborEntry);

#if defined(TCPIP_IPV6_G3_PLC_BORDER_ROUTER) && (TCPIP_IPV6_G3_PLC_BORDER_ROUTER != 0)
static void TCPIP_NDP_G3RouterAdvertiseTask (void);
#endif  // defined(TCPIP_IPV6_G3_PLC_BORDER_ROUTER) && (TCPIP_IPV6_G3_PLC_BORDER_ROUTER != 0)

/*****************************************************************************
  Function:
    bool TCPIP_NDP_Initialize (const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* ndpData)

  Summary:
    Initializes the Neighbor Discovery Protocol module.

  Description:
    This function initializes the Neighbor Discovery Protocol module, including
    setting up and initial random delay, the router solicitation state machine,
    and the heap used to dynamically allocate structures for NDP.

  Precondition:
    None

  Parameters:
    stackCtrl - TCP/IP Stack module initializer structure
    ndpData   - optional initialization data

  Returns:
    true

  Remarks:
    None
  ***************************************************************************/
bool TCPIP_NDP_Initialize (const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* initData)
{
    size_t i;
    RS_STATIC_VARS* pRs;

    if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }

    if(ndpAsyncHandle == NULL)
    {
        ndpAsyncHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_NDP_Task, TCPIP_IPV6_NDP_TASK_TIMER_RATE);
    }
    
    if (ndpAsyncHandle == NULL )
    {
        F_TCPIP_NDP_Cleanup();
        return false;
    }


    //stackCtrl->stackAction == TCPIP_STACK_ACTION_INIT)   // stack init
    if (ndpInitCount == 0)
    {   // Perform one-time initialization

        ndpMemH = stackCtrl->memH;
        nStackIfs = (size_t)stackCtrl->nIfs;
        gRSState = (RS_STATIC_VARS*)TCPIP_HEAP_Calloc(ndpMemH, stackCtrl->nIfs, sizeof(*gRSState));
        if(gRSState == NULL)
        {   // failed
            return false;
        }

        for (i = 0; i < DUPLICATE_ADDR_DISCOVERY_THREADS; i++)
        {
            gDuplicateAddrDetectState[i].state = DAD_STATE_INACTIVE;
        }
        sysFreq = SYS_TMR_TickCounterFrequencyGet(); 
    }

    pRs = gRSState + stackCtrl->netIx;
    pRs->state = RS_STATE_INACTIVE;
    pRs->transmitAttempt = 0;

    gSetInitialDelay = true;

    ndpInitCount++;

    return true;
}

/*****************************************************************************
  Function:
    void TCPIP_NDP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)

  Summary:
    Disabled NDP functionality on a per-interface basis.

  Description:
    This function will disable NDP functionality on a specified interface.

  Precondition:
    None

  Parameters:
    stackCtrl - Stack initialization parameters

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_NDP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    //if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)   // stack down
    //if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN)  // interface down

    if(ndpInitCount > 0)
    {   // we're up and running
        (gRSState + stackCtrl->netIx)->state = RS_STATE_INACTIVE;

        if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--ndpInitCount == 0)
            {   // all closed
                F_TCPIP_NDP_Cleanup();
                (void)TCPIP_HEAP_Free(ndpMemH, gRSState);
                gRSState = NULL;
                ndpMemH = NULL;
            }
        }
    }

}

// clean up the resources
static void F_TCPIP_NDP_Cleanup(void)
{
    if(ndpAsyncHandle != NULL)
    {
        TCPIPStackSignalHandlerDeregister(ndpAsyncHandle);
        ndpAsyncHandle = NULL;
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

/*****************************************************************************
  Function:
    IPV6_HEAP_NDP_NC_ENTRY * TCPIP_NDP_NborEntryCreate (
        TCPIP_NET_IF * pNetIf, IPV6_ADDR * remoteIPAddr,
        const TCPIP_MAC_ADDR * remoteMACAddr, uint8_t initialState,
        uint8_t routerFlag, IPV6_ADDR_STRUCT * preferredSource)

  Summary:
    Creates an entry in the neighbor cache.

  Description:
    This function will dynamically allocate an entry and add it to the linked-
    list of neighbor cache entries for the specified interface.

  Precondition:
    The heap used by this module must be initialized.

  Parameters:
    pNetIf - The interface that the neighbor is on
    remoteIPAddress - The neighbor's IPv6 address
    remoteMACAddress - The neighbor's link-layer address (can be NULL)
    initialState - The neighbor's initial Neighbor Unreachability Detection
        state
    routerFlag - true if the neighbor is a router, false otherwise
    preferredSource - The source address to use when sending traffic to this
        neighbor (if possible)

  Returns:
    IPV6_HEA_NDP_NC_ENTRY * - Pointer to the new Neighbor Cache Entry or
                                NULL if one can't be allocated

  Remarks:
    None
  ***************************************************************************/
IPV6_HEAP_NDP_NC_ENTRY * TCPIP_NDP_NborEntryCreate (const TCPIP_NET_IF * pNetIf, const IPV6_ADDR * remoteIPAddr, const TCPIP_MAC_ADDR * remoteMACAddr, uint8_t initialState, uint8_t routerFlag, IPV6_ADDR_STRUCT * preferredSource)
{
    IPV6_HEAP_NDP_NC_ENTRY * neighborPointer;

    neighborPointer = (IPV6_HEAP_NDP_NC_ENTRY *)TCPIP_HEAP_Malloc (ndpMemH, sizeof (IPV6_HEAP_NDP_NC_ENTRY));

    if (neighborPointer != NULL)
    {
        (void)memcpy (neighborPointer->remoteIPAddress.v, remoteIPAddr->v, sizeof (IPV6_ADDR));
        if (remoteMACAddr != NULL)
        {
            (void)memcpy(neighborPointer->remoteMACAddr.v, remoteMACAddr->v, sizeof (TCPIP_MAC_ADDR));
        }
        else
        {
            (void)memset (neighborPointer->remoteMACAddr.v, 0x00, sizeof (TCPIP_MAC_ADDR));
        }

        TCPIP_NDP_ReachabilitySet (pNetIf, neighborPointer, initialState);
        neighborPointer->unansweredProbes = 0U;
        neighborPointer->nextNUDTime = 0;
        neighborPointer->flags.val = 0;
        TCPIP_Helper_SingleListInitialize (&neighborPointer->queuedPackets);
        if (routerFlag != 0U)
        {
            neighborPointer->flags.bIsRouter = 1U;
        }
        else
        {
            neighborPointer->flags.bIsRouter = 0U;
        }

        neighborPointer->preferredSource = preferredSource;

        TCPIP_NDP_LinkedListEntryInsert (pNetIf, neighborPointer, (uint8_t)IPV6_HEAP_NDP_NC_ID);

        // Let the IP layer start address resolution if there is traffic to send
        neighborPointer->flags.bResolvingAddress = 0U;

//        if (initialState == NDP_STATE_INCOMPLETE)
//            TCPIP_NDP_AddressResolve (neighborPointer);
    }
    return neighborPointer;
}

/*****************************************************************************
  Function:
    IPV6_HEAP_NDP_DR_ENTRY * TCPIP_NDP_DefaultRouterEntryCreate (
        TCPIP_NET_IF * pNetIf, IPV6_HEAP_NDP_NC_ENTRY * neighbor,
        uint32_t invalidationTime)

  Summary:
    Creates an entry in the default router list.

  Description:
    This function will dynamically allocate an entry and add it to the linked-
    list of default router entries for the specified interface.

  Precondition:
    The heap used by this module must be initialized.

  Parameters:
    pNetIf - The interface that the router is on
    neighbor - The neighbor cache entry of the router
    invalidationTime - The router's invalidation timer

  Returns:
    IPV6_HEA_NDP_DR_ENTRY * - Pointer to the new Default Router Entry or
                                NULL if one can't be allocated

  Remarks:
    None
  ***************************************************************************/
IPV6_HEAP_NDP_DR_ENTRY * TCPIP_NDP_DefaultRouterEntryCreate (const TCPIP_NET_IF * pNetIf, IPV6_HEAP_NDP_NC_ENTRY * neighbor, uint32_t invalidationTime)
{
    IPV6_HEAP_NDP_DR_ENTRY * routerPointer;

    routerPointer = (IPV6_HEAP_NDP_DR_ENTRY *)TCPIP_HEAP_Malloc(ndpMemH, sizeof(IPV6_HEAP_NDP_DR_ENTRY));

    if (routerPointer != NULL)
    {
        routerPointer->neighborInfo = neighbor;
        routerPointer->invalidationTimer = invalidationTime;
        routerPointer->tickTimer = SYS_TMR_TickCountGet();
        TCPIP_NDP_LinkedListEntryInsert (pNetIf, routerPointer, (uint8_t)IPV6_HEAP_NDP_DR_ID);
    }

    return routerPointer;
}

/*****************************************************************************
  Function:
    IPV6_HEAP_NDP_DC_ENTRY * TCPIP_NDP_DestCacheEntryCreate (
        TCPIP_NET_IF * pNetIf, IPV6_ADDR * remoteIPAddress,
        uint16_t linkMTU, IPV6_HEAP_NDP_NC_ENTRY * neighbor)

  Summary:
    Creates an entry in the destination cache.

  Description:
    This function will dynamically allocate an entry and add it to the linked-
    list of destination cache entries for the specified interface.

  Precondition:
    The heap used by this module must be initialized.

  Parameters:
    pNetIf - The interface that the destination communicates with
    remoteIPAddress - The destination IP address
    linkMTU - The MTU of the link to the destination
    neighbor - The first hop to reach the destination.

  Returns:
    IPV6_HEA_NDP_DC_ENTRY * - Pointer to the new Destination Cache Entry or
                                NULL if one can't be allocated

  Remarks:
    None
  ***************************************************************************/
IPV6_HEAP_NDP_DC_ENTRY * TCPIP_NDP_DestCacheEntryCreate (const TCPIP_NET_IF * pNetIf, const IPV6_ADDR * remoteIPAddress, uint16_t linkMTU, IPV6_HEAP_NDP_NC_ENTRY * neighbor)
{
    IPV6_HEAP_NDP_DC_ENTRY * destinationPointer;

    destinationPointer = (IPV6_HEAP_NDP_DC_ENTRY *)TCPIP_HEAP_Malloc(ndpMemH, sizeof(IPV6_HEAP_NDP_DC_ENTRY));

    if (destinationPointer != NULL)
    {
        (void)memcpy (destinationPointer->remoteIPAddress.v, remoteIPAddress->v, sizeof (IPV6_ADDR));
        destinationPointer->pathMTU = linkMTU;
        destinationPointer->nextHopNeighbor = neighbor;
        if (linkMTU != (uint16_t)TCPIP_IPV6_DEFAULT_LINK_MTU)
        {
            destinationPointer->pathMTUIncreaseTimer = SYS_TMR_TickCountGet() + (sysFreq * (uint32_t)TCPIP_IPV6_MTU_INCREASE_TIMEOUT);
        }
        else
        {
            destinationPointer->pathMTUIncreaseTimer = 0;
        }
        TCPIP_NDP_LinkedListEntryInsert (pNetIf, destinationPointer, (uint8_t)IPV6_HEAP_NDP_DC_ID);
    }

    return destinationPointer;
}

/*****************************************************************************
  Function:
    IPV6_HEAP_NDP_PL_ENTRY * TCPIP_NDP_PrefixListEntryCreate (
        TCPIP_NET_IF * pNetIf, IPV6_ADDR * prefix,
        uint8_t prefixLength, uint32_t validLifetime)

  Summary:
    Creates an entry in the prefix list.

  Description:
    This function will dynamically allocate an entry and add it to the linked-
    list of on-link prefix entries for the specified interface.

  Precondition:
    The heap used by this module must be initialized.

  Parameters:
    pNetIf - The interface that the prefix is on-link for.
    prefix - An IPv6 address containing the prefix
    prefixLength - Length of the prefix (bits)
    validLifetime - Amount of time the prefix can be considered valid

  Returns:
    IPV6_HEA_NDP_PL_ENTRY * - Pointer to the new Prefix List Entry or
                                NULL if one can't be allocated

  Remarks:
    None
  ***************************************************************************/
IPV6_HEAP_NDP_PL_ENTRY * TCPIP_NDP_PrefixListEntryCreate (const TCPIP_NET_IF * pNetIf, IPV6_ADDR * prefix, uint8_t prefixLength, uint32_t validLifetime)
{
    IPV6_HEAP_NDP_PL_ENTRY * entryPointer;

    entryPointer = (IPV6_HEAP_NDP_PL_ENTRY *)TCPIP_HEAP_Malloc (ndpMemH, sizeof (IPV6_HEAP_NDP_PL_ENTRY));

    if (entryPointer != NULL)
    {
        (void)memcpy (entryPointer->prefix.v, prefix->v, sizeof (IPV6_ADDR));
        entryPointer->prefixLength = prefixLength;
        entryPointer->validLifetime = validLifetime;
        entryPointer->lastTickTime = SYS_TMR_TickCountGet();
        TCPIP_NDP_LinkedListEntryInsert (pNetIf, entryPointer, (uint8_t)IPV6_HEAP_NDP_PL_ID);
    }

    return entryPointer;
}

/*****************************************************************************
  Function:
    IPV6_HEAP_NDP_NC_ENTRY * TCPIP_NDP_NborEntryDelete (
        TCPIP_NET_IF * pNetIf, IPV6_HEAP_NDP_NC_ENTRY * entry)

  Summary:
    Deletes a neighbor cache entry.

  Description:
    This function will delete a neighbor cache entry.  If the neighbor was
    a router, it will remove it from the default router list.  It will
    remove any destination cache entries that use this neighbor as
    a first hop.  It will deallocate any queued packets for this neighbor.

  Precondition:
    None

  Parameters:
    pNetIf - The interface the neighbor is on.
    entry - The neighbor cache entry.

  Returns:
    IPV6_HEAP_NDP_NC_ENTRY * - The neighbor cache entry in the linked list
        after the entry that was removed.  NULL if the removed entry was the
        last one in the list.

  Remarks:
    None
  ***************************************************************************/
IPV6_HEAP_NDP_NC_ENTRY * TCPIP_NDP_NborEntryDelete (const TCPIP_NET_IF * pNetIf, IPV6_HEAP_NDP_NC_ENTRY * entry)
{
    IPV6_HEAP_NDP_NC_ENTRY * ptrNeighbor;
    IPV6_HEAP_NDP_DC_ENTRY * ptrDestination;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;
    IPV6_PACKET * pkt;

    if (pNetIf == NULL || entry == NULL)
    {
        return NULL;
    }

    pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pNetIf);

    while (entry->queuedPackets.nNodes != 0U)
    {
        pkt = FC_SglNode2Ip6Pkt(TCPIP_Helper_SingleListHeadRemove(&entry->queuedPackets));
        TCPIP_IPV6_PacketAck(pkt, false);
    }

    // If this entry is a router, see if we need to remove it from the default router list
    if (entry->flags.bIsRouter != 0U)
    {
        IPV6_HEAP_NDP_DR_ENTRY * ptrRouter;

        ptrRouter = FC_VPtr2NdpEntry(TCPIP_NDP_RemoteNodeFind (pNetIf, &entry->remoteIPAddress, (uint8_t)IPV6_HEAP_NDP_DR_ID)).pDrEntry;
        if (ptrRouter != NULL)
        {
            (void)TCPIP_NDP_LinkedListEntryRemove (pNetIf, ptrRouter, (uint8_t)IPV6_HEAP_NDP_DR_ID);
        }
    }

    ptrNeighbor = TCPIP_NDP_LinkedListEntryRemove (pNetIf, entry, (uint8_t)IPV6_HEAP_NDP_NC_ID);

    ptrDestination = FC_SglNode2NdpEntry(pIpv6Config->listDestinationCache.head).pDcEntry;

    while (ptrDestination != NULL)
    {
        if (ptrDestination->nextHopNeighbor == entry)
        {
            ptrDestination = TCPIP_NDP_LinkedListEntryRemove (pNetIf, ptrDestination, (uint8_t)IPV6_HEAP_NDP_DC_ID);
        }
        else
        {
            ptrDestination = ptrDestination->next;
        }
    }

    return ptrNeighbor;
}

/*****************************************************************************
  Function:
    void TCPIP_NDP_NborReachConfirm (
        TCPIP_NET_HANDLE netH, IPV6_ADDR * address)

  Summary:
    Confirms that a neighbor is reachable.

  Description:
    This function is used by upper-layer protocols to indicate that round-trip
    communications were confirmed with a neighboring node.

  Precondition:
    None

  Parameters:
    pNetIf - The interface the neighbor is on.
    address - The address of the neighbor.

  Returns:
    None.

  Remarks:
    None
  ***************************************************************************/
void TCPIP_NDP_NborReachConfirm (TCPIP_NET_HANDLE netH, const IPV6_ADDR * address)
{
    IPV6_HEAP_NDP_NC_ENTRY * entry;
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(netH);

    if (pNetIf == NULL && address == NULL)
    {
        return;
    }

    entry = FC_VPtr2NdpEntry(TCPIP_NDP_RemoteNodeFind (pNetIf, address, (uint8_t)IPV6_HEAP_NDP_NC_ID)).pNcEntry;
    if (entry !=  NULL)
    {
        TCPIP_NDP_ReachabilitySet (pNetIf, entry, (uint8_t)NDP_STATE_REACHABLE);
    }
}

/*****************************************************************************
  Function:
    IPV6_HEAP_NDP_DR_ENTRY * TCPIP_NDP_DefaultRouterGet (TCPIP_NET_IF * pNetIf)

  Summary:
    Gets a default router for the given interface.

  Description:
    This function determines the default router that should be selected for
    a given interface.  The algorithm will prefer routers with a known
    link-layer address.  If none are available, a default router will be
    selected round-robin.

  Precondition:
    None

  Parameters:
    pNetIf - The interface to get the router for.

  Returns:
    IPV6_HEAP_NDP_DR_ENTRY * - The default router.  NULL if no router
                                is available.

  Remarks:
    None
  ***************************************************************************/
IPV6_HEAP_NDP_DR_ENTRY * TCPIP_NDP_DefaultRouterGet (const TCPIP_NET_IF * pNetIf)
{
    IPV6_HEAP_NDP_DR_ENTRY * routerPointer;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;

    if (pNetIf == NULL)
    {
        return NULL;
    }

    pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pNetIf);

    routerPointer = FC_SglNode2NdpEntry(pIpv6Config->listDefaultRouter.head).pDrEntry;

    while (routerPointer != NULL)
    {
        if ((routerPointer->neighborInfo->flags.bIsRouter != 0U) && (routerPointer->invalidationTimer > 0U) && (routerPointer->neighborInfo->reachabilityState != (uint8_t)NDP_STATE_INCOMPLETE) && (routerPointer->neighborInfo->reachabilityState != (uint8_t)NDP_STATE_NONE))
        {
            pIpv6Config->currentDefaultRouter = routerPointer;
            return routerPointer;
        }
        routerPointer = routerPointer->next;
    }

    // Can't find any non-incomplete routers
    // Select the default router round-robin-style
    routerPointer = pIpv6Config->currentDefaultRouter;
    if (routerPointer == NULL)
    {
        routerPointer = FC_SglNode2NdpEntry(pIpv6Config->listDefaultRouter.head).pDrEntry;
    }
    if (routerPointer != NULL)
    {
        if (routerPointer->next != NULL)
        {
            pIpv6Config->currentDefaultRouter = routerPointer->next;
        }
        else
        {
            pIpv6Config->currentDefaultRouter = routerPointer;
        }
    }
    else
    {
        pIpv6Config->currentDefaultRouter = NULL;
    }

    return pIpv6Config->currentDefaultRouter;
}

/*****************************************************************************
  Function:
    void TCPIP_NDP_Task (void)

  Summary:
    Performs tasks for Network Discovery Protocol

  Description:
    This function performs NDP tasks.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void TCPIP_NDP_Task (void)
{
    TCPIP_NDP_DupAddrDiscoveryTask();
    TCPIP_NDP_RouterSolicitTask();
    TCPIP_NDP_NborUnreachDetectTask();
#if defined(TCPIP_IPV6_G3_PLC_BORDER_ROUTER) && (TCPIP_IPV6_G3_PLC_BORDER_ROUTER != 0)
    TCPIP_NDP_G3RouterAdvertiseTask();
#endif  // defined(TCPIP_IPV6_G3_PLC_BORDER_ROUTER) && (TCPIP_IPV6_G3_PLC_BORDER_ROUTER != 0)
}

/*****************************************************************************
  Function:
    void TCPIP_NDP_DupAddrDiscoveryTask (void)

  Summary:
    Performs tasks for Duplicate Address Discovery

  Description:
    This function performs Duplicate Address Discovery tasks.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
static void TCPIP_NDP_DupAddrDiscoveryTask (void)
{
    IPV6_ADDR_STRUCT * addressPointer;
    uint8_t result;
    const TCPIP_NET_IF* pNetIf;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;
    size_t netIx;

    for(netIx=0; netIx < nStackIfs; netIx++)
    {
        pNetIf = (const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(netIx);
        pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pNetIf);
        addressPointer = FC_DblNode2AddStruct(pIpv6Config->listIpv6TentativeAddresses.head);

        while (addressPointer != NULL)
        {
            result = TCPIP_NDP_DupAddrDiscoveryStatus (addressPointer);
            switch (result)
            {
                case DAD_ADDRESS_DUPLICATED:
                case DAD_BAD_ARGUMENT:
                    ndpDADCount--;
                    addressPointer = TCPIP_NDP_LinkedListEntryRemove (pNetIf, addressPointer, (uint8_t)IPV6_HEAP_ADDR_UNICAST_TENTATIVE_ID);
                    break;
                case DAD_OK:
                    ndpDADCount--;
                    addressPointer = TCPIP_NDP_TentativeAddressPromote (pNetIf, addressPointer);
                    break;
                case DAD_PENDING:
                default:
                    addressPointer = addressPointer->next;
                    break;
            }
        }
    }
}

/*****************************************************************************
  Function:
    bool TCPIP_NDP_DupAddrDiscoveryDetect (
        TCPIP_NET_IF * pNetIf, IPV6_ADDR_STRUCT * localAddressPointer)

  Summary:
    Initiates duplicate address detection for a given address.

  Description:
    This function will begin duplicate address detection for a locally
    configured unicast address.  Until the address is found to be unique it
    will be treated as a tentative address.

  Precondition:
    None

  Parameters:
    pNetIf - The interface the address is configured on.
    localAddressPointer - The local address structure

  Returns:
    true - if the address duplication started OK
    false - no slot available for this DAD 

  Remarks:
    None
  ***************************************************************************/
bool TCPIP_NDP_DupAddrDiscoveryDetect (TCPIP_NET_IF * pNetIf, IPV6_ADDR_STRUCT * localAddressPointer)
{
    size_t i;

    for (i = 0; i < DUPLICATE_ADDR_DISCOVERY_THREADS; i++)
    {
        if (gDuplicateAddrDetectState[i].state == DAD_STATE_INACTIVE)
        {
            gDuplicateAddrDetectState[i].state = DAD_STATE_TRANSMIT;
            gDuplicateAddrDetectState[i].addressPointer = localAddressPointer;
            gDuplicateAddrDetectState[i].transmitAttempt = 0;
            gDuplicateAddrDetectState[i].receivedSolicitations = 0;
            gDuplicateAddrDetectState[i].netConfig = pNetIf;
#if (TCPIP_IPV6_RIID_ENABLED != 0)    
            gDuplicateAddrDetectState[i].retryCount = 0;
#endif  // (TCPIP_IPV6_RIID_ENABLED != 0)    
            ndpDADCount++;
            return true;
        }
    }

    // No free IP addr slots or DAD thread slots
    return false;
}


/*****************************************************************************
  Function:
    uint8_t TCPIP_NDP_DupAddrDiscoveryStatus (IPV6_ADDR_STRUCT * localAddressPointer)

  Summary:
    Determines the status of an address undergoing duplicate address detection.

  Description:
    Determines the status of an address undergoing duplicate address detection.


  Precondition:
    None

  Parameters:
    localAddressPointer - Pointer to the address undergoing DAD.

  Returns:
    DAD_OK - The address is not a duplicate
    DAD_ADDRESS_DUPLICATED - The address is a duplicate
    DAD_PENDING
    DAD_BAD_ARGUMENT

  Remarks:
    None
  ***************************************************************************/
static uint8_t TCPIP_NDP_DupAddrDiscoveryStatus (IPV6_ADDR_STRUCT * localAddressPointer)
{
    size_t ix;
    uint8_t dadRes;

    // Add a delay of 0-TCPIP_IPV6_NDP_MAX_RTR_SOLICITATION_DELAY before the first transmission

    if(gSetInitialDelay)
    {
        gInitialDelay = F_TCPIP_NDP_RandomSolicitationDelayGet();
        gSetInitialDelay = false;
        return DAD_PENDING;
    }

    if (gInitialDelay != 0U)
    {
        if (FC_Ui322I32(SYS_TMR_TickCountGet() - gInitialDelay) > 0)
        {
            // Remove initial delay
            gInitialDelay = 0U;
        }
        else
        {
            return DAD_PENDING;
        }
    }

    for (ix = 0; ix < DUPLICATE_ADDR_DISCOVERY_THREADS; ix++)
    {
        // Determine if any of our active threads matches the address we just received
        if (gDuplicateAddrDetectState[ix].addressPointer == localAddressPointer)
        {
#if (TCPIP_IPV6_RIID_ENABLED != 0)    
            TCPIP_NET_IF * pNetIf = gDuplicateAddrDetectState[ix].netConfig;
            const TCPIP_IPV6_CONFIG_DCPT* pIpv6Dcpt = TCPIP_IPV6_ConfigDcptGet();
#endif // (TCPIP_IPV6_RIID_ENABLED != 0)    
            
            switch (gDuplicateAddrDetectState[ix].state)
            {
                case (uint8_t)DAD_STATE_INACTIVE:
                    dadRes = DAD_BAD_ARGUMENT;
                    break;

                case (uint8_t)DAD_STATE_TRANSMIT:
                    {
                        IPV6_ADDR solicitedNodeMulticastAddr = {{0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                                    0x00, 0x00, 0x00, 0x01, 0xFF,
                                                                    gDuplicateAddrDetectState[ix].addressPointer->address.v[13],
                                                                    gDuplicateAddrDetectState[ix].addressPointer->address.v[14],
                                                                    gDuplicateAddrDetectState[ix].addressPointer->address.v[15]}};
                        IPV6_PACKET * pkt;

                        IPV6_ADDR_STRUCT* pDadStruct = (gDuplicateAddrDetectState + ix)->addressPointer;
                        IPV6_ADDR* pDadAdd = &pDadStruct->address;
#if (TCPIP_IPV6_RIID_ENABLED != 0)
                        if ((pIpv6Dcpt->configFlags & TCPIP_IPV6_FLAG_RANDOM_INTERFACE_ID) != 0)
                        {
                            if (F_TCPIP_NDP_InterfaceIdReserved(pDadStruct) ||
                                F_TCPIP_NDP_InterfaceIdInUse(gDuplicateAddrDetectState[ix].netConfig, pDadStruct))
                            {
                                gDuplicateAddrDetectState[ix].state = DAD_STATE_FAIL;
                                dadRes = DAD_PENDING;
                                break;
                            }
                        }
#endif // (TCPIP_IPV6_RIID_ENABLED != 0)    
                        pkt = TCPIP_ICMPV6_HeaderNeighborSolicitationPut (gDuplicateAddrDetectState[ix].netConfig, &IPV6_FIXED_ADDR_UNSPECIFIED, &solicitedNodeMulticastAddr, pDadAdd);
                        if (pkt != NULL)
                        {
                            if (!TCPIP_ICMPV6_Flush(pkt))
                            {
                                dadRes = DAD_PENDING;
                                break;
                            }
                            gDuplicateAddrDetectState[ix].transmitAttempt++;
                            gDuplicateAddrDetectState[ix].transmitTimer = SYS_TMR_TickCountGet() + ((sysFreq * TCPIP_IPV6_InterfaceConfigGet(gDuplicateAddrDetectState[ix].netConfig)->retransmitTime) / 1000U);
                            gDuplicateAddrDetectState[ix].state = DAD_STATE_TRANSMIT_WAIT;
                        }
                    }
                    dadRes = DAD_PENDING;
                    break;

                case (uint8_t)DAD_STATE_TRANSMIT_WAIT:
                    if (FC_Ui322I32(SYS_TMR_TickCountGet() - gDuplicateAddrDetectState[ix].transmitTimer) > 0)

                    {
                        if (gDuplicateAddrDetectState[ix].transmitAttempt == DUPLICATE_ADDR_DETECT_TRANSMITS)
                        {
                            gDuplicateAddrDetectState[ix].state = DAD_STATE_DONE;
                        }
                        else
                        {
                            gDuplicateAddrDetectState[ix].state = DAD_STATE_TRANSMIT;
                        }
                    }
                    dadRes = DAD_PENDING;
                    break;

                case (uint8_t)DAD_STATE_DONE:
                    gDuplicateAddrDetectState[ix].state = DAD_STATE_INACTIVE;
                    dadRes = DAD_OK;
                    break;

                case (uint8_t)DAD_STATE_FAIL:
#if (TCPIP_IPV6_RIID_ENABLED != 0)    
                    if ((pIpv6Dcpt->configFlags & TCPIP_IPV6_FLAG_RANDOM_INTERFACE_ID) != 0)
                    {
                        gDuplicateAddrDetectState[ix].retryCount++;
                        if (gDuplicateAddrDetectState[ix].retryCount < DUPLICATE_ADDR_DETECT_RETRIES)
                        {
                            gDuplicateAddrDetectState[ix].retryTimer = F_TCPIP_NDP_RandomSolicitationDelayGet();
                            gDuplicateAddrDetectState[ix].state = DAD_STATE_RETRY_WAIT;
                            dadRes = DAD_PENDING;
                            break;
                        }
                    }
#endif  // (TCPIP_IPV6_RIID_ENABLED != 0)    
                    gDuplicateAddrDetectState[ix].state = DAD_STATE_INACTIVE;
                    dadRes = DAD_ADDRESS_DUPLICATED;
                    break;

#if (TCPIP_IPV6_RIID_ENABLED != 0)    
                case DAD_STATE_RETRY_WAIT:
                    if ((FC_Ui322I32(SYS_TMR_TickCountGet()) - (int32_t)gDuplicateAddrDetectState[ix].retryTimer) > 0)
                    {
                        IPV6_ADDR *pAddress = &gDuplicateAddrDetectState[ix].addressPointer->address;
                        uint8_t prefixLength = gDuplicateAddrDetectState[ix].addressPointer->prefixLen;
                        uint8_t retryCount = gDuplicateAddrDetectState[ix].retryCount;
                        if(TCPIP_IPV6_AddressInterfaceIDSet(pNetIf, pAddress, prefixLength, retryCount) == false)
                        {
                            gDuplicateAddrDetectState[ix].state = DAD_STATE_INACTIVE;
                            dadRes = DAD_BAD_ARGUMENT;
                            break;
                        }
                        gDuplicateAddrDetectState[ix].transmitAttempt = 0;
                        gDuplicateAddrDetectState[ix].receivedSolicitations = 0;
                        gDuplicateAddrDetectState[ix].state = DAD_STATE_TRANSMIT;
                    }
                    dadRes = DAD_PENDING;
                    break;
#endif  // (TCPIP_IPV6_RIID_ENABLED != 0)    

                default:
                    gDuplicateAddrDetectState[ix].state = DAD_STATE_INACTIVE;
                    dadRes = DAD_BAD_ARGUMENT;
                    break;
            }
            return dadRes;
        }
    }

    // Could not find matching entry
    return DAD_BAD_ARGUMENT;
}

static uint32_t F_TCPIP_NDP_RandomSolicitationDelayGet(void)
{
    return SYS_TMR_TickCountGet() + (((SYS_RANDOM_PseudoGet() % 40U) * sysFreq * (uint32_t)TCPIP_IPV6_NDP_MAX_RTR_SOLICITATION_DELAY) / 40U);
}

#if (TCPIP_IPV6_RIID_ENABLED != 0)    
static bool F_TCPIP_NDP_InterfaceIdReserved(IPV6_ADDR_STRUCT* tentativeAddressPointer)
{
    // The subnet-router anycast ID (RFC 4291)
    static const uint8_t SUBNET_ROUTER_ANYCAST_IID[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    // The reserved IANA ethernet block ranges 0200:5EFF:FE00:0000-0200:5EFF:FE00:5212 and 0200:5EFF:FE00:5214-0200:5EFF:FEFF:FFFF (RFC 4291)
    // and the Proxy Mobile 0200:5EFF:FE00:5213 (RFC 6543)
    static const uint8_t IANA_ETHERNET_BLOCK_AND_PROXY_MOBILE_PREFIX_RANGE[5] = {0x02, 0x00, 0x5E, 0xFF, 0xFE};
    // The reserved subnet anycast range (RFC 2526)
    static const uint8_t SUBNET_ANYCAST_PREFIX_RANGE[7] = {0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    uint8_t *pInterfaceID = tentativeAddressPointer->address.v + IPV6_INTERFACE_ID_OFFSET;
    
    return  (0 == memcmp(pInterfaceID, SUBNET_ROUTER_ANYCAST_IID, sizeof(SUBNET_ROUTER_ANYCAST_IID))) ||
            (0 == memcmp(pInterfaceID, IANA_ETHERNET_BLOCK_AND_PROXY_MOBILE_PREFIX_RANGE, sizeof(IANA_ETHERNET_BLOCK_AND_PROXY_MOBILE_PREFIX_RANGE)) ||
            ((0 == memcmp(pInterfaceID, SUBNET_ANYCAST_PREFIX_RANGE, sizeof(SUBNET_ANYCAST_PREFIX_RANGE))) && (pInterfaceID[7] >= 0x80)));
}

static bool F_TCPIP_NDP_InterfaceIdInUse(TCPIP_NET_IF * pNetIf, IPV6_ADDR_STRUCT* tentativeAddressPointer)
{
    IPV6_ADDR_STRUCT * unicastAddressPointer;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;
    uint8_t *prefix = tentativeAddressPointer->address.v;
    uint8_t prefixLength = tentativeAddressPointer->prefixLen;
    uint8_t *interfaceID = tentativeAddressPointer->address.v + IPV6_INTERFACE_ID_OFFSET;

    pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pNetIf);
    unicastAddressPointer = FC_DblNode2AddStruct(pIpv6Config->listIpv6UnicastAddresses.head);

    while (unicastAddressPointer != NULL)
    {
        if (unicastAddressPointer->prefixLen != 0)
        {
            if (prefixLength == unicastAddressPointer->prefixLen)
            {
                const IPV6_ADDR* pUnicastAdd = FC_U8Ptr2CIp6Add((uint8_t*)unicastAddressPointer + offsetof(IPV6_ADDR_STRUCT, address));
                if (TCPIP_Helper_FindCommonPrefix (prefix, pUnicastAdd->v, (uint8_t)sizeof (IPV6_ADDR)) >= prefixLength)
                {
                    if (0 == memcmp(interfaceID, pUnicastAdd->v + IPV6_INTERFACE_ID_OFFSET, IPV6_INTERFACE_ID_SIZE / 8U))
                    {
                        return true;
                    }
                }
            }
        }
        unicastAddressPointer = unicastAddressPointer->next;
    }
    
    return false;
}
#endif  // (TCPIP_IPV6_RIID_ENABLED != 0)    

/*****************************************************************************
  Function:
    void TCPIP_NDP_DupAddrDiscoveryProcess (IPV6_ADDR_STRUCT * localAddressPointer,
                                        uint8_t type)

  Summary:
    Handles reception of packets from duplicated addresses.

  Description:
    This function is called if a packet is received from an address that is
    undergoing duplicate address detection.  It will deallocate that
    local address.

  Precondition:
    None

  Parameters:
    localAddressPointer - Pointer to the address that is duplicated.
    type - Type of packet receives (this field is currently unused because
            all available MAC/PHYs have loopback filtering).

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void TCPIP_NDP_DupAddrDiscoveryProcess (IPV6_ADDR_STRUCT * localAddressPointer, uint8_t type)
{
    size_t i;

    for (i = 0; i < DUPLICATE_ADDR_DISCOVERY_THREADS; i++)
    {
        // Determine if any of our active threads matches the address we just received
        if (gDuplicateAddrDetectState[i].addressPointer == localAddressPointer)
        {
            if (type == (uint8_t)IPV6_NDP_DAD_NS_RECEIVED)
            {
                gDuplicateAddrDetectState[i].state = DAD_STATE_FAIL;
            }
            else if (type == (uint8_t)IPV6_NDP_DAD_NA_RECEIVED)
            {
                gDuplicateAddrDetectState[i].state = DAD_STATE_FAIL;
            }
            else
            {
                // do nothing
            }

            return;
        }
    }
}

/*****************************************************************************
  Function:
    void TCPIP_NDP_RouterSolicitStart (TCPIP_NET_IF * pNetIf)

  Summary:
    Begins router solicitation.

  Description:
    This function will initialize the state machine for router solicitation.

  Precondition:
    None

  Parameters:
    pNetIf - The interface to solicit routers on.

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void TCPIP_NDP_RouterSolicitStart (const TCPIP_NET_IF * pNetIf)
{
    RS_STATIC_VARS* pRs;

    pRs = gRSState + TCPIP_STACK_NetIxGet(pNetIf);
    pRs->state = RS_STATE_TRANSMIT;
    pRs->transmitAttempt = 0;
    ndpRSCount++;
}

/*****************************************************************************
  Function:
    void TCPIP_NDP_RouterSolicitTask (void)

  Summary:
    Task function for router solicitation.

  Description:
    This function performs periodic tasks required for router solicitation.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
static void TCPIP_NDP_RouterSolicitTask (void)
{
    NDP_OPTION_LLA sllaOption;
    IPV6_PACKET * pkt;
    size_t netIx;
    const TCPIP_NET_IF * pNetIf;
    RS_STATIC_VARS* pRs;
    IPV6_ADDR_STRUCT* pRsStruct; 
    IPV6_ADDR* pRsAdd;
    const IPV6_ADDR* remSolicitAdd;

    for(netIx=0; netIx< nStackIfs; netIx++)
    {
        pNetIf = (const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet (netIx);
        pRs = gRSState + netIx;

        switch (pRs->state)
        {
            case (uint8_t)RS_STATE_INACTIVE:
                break;

            case (uint8_t)RS_STATE_TRANSMIT:
                if((pNetIf->startFlags & (uint16_t)TCPIP_NETWORK_CONFIG_IPV6_UNICAST_RS) != 0U)
                {
                    remSolicitAdd = TCPIP_IPV6_DefaultRouterGet(pNetIf);
                    if(remSolicitAdd == NULL)
                    {   // cannot send the RS w/o an address
                        break;
                    }
                } 
                else
                {
                    remSolicitAdd = &IPV6_FIXED_ADDR_ALL_ROUTER_MULTICAST;
                }


                if ((pRs->address == NULL) || (pRs->address->flags.scope != (unsigned)IPV6_ADDR_SCOPE_LINK_LOCAL))
                {
                    // Note: Addresses in this list are all unicast, so we can skip that check
                    pRs->address = FC_DblNode2AddStruct(TCPIP_IPV6_InterfaceConfigGet(pNetIf)->listIpv6UnicastAddresses.head);
                    while ((pRs->address != NULL) && (pRs->address->flags.scope != (unsigned)IPV6_ADDR_SCOPE_LINK_LOCAL))
                    {
                        pRs->address = pRs->address->next;
                    }

                    if (pRs->address == NULL)
                    {
                        // If no address can be found, send the solicitation with the unspecified address
                        pkt = TCPIP_ICMPV6_HeaderRouterSolicitationPut (pNetIf, &IPV6_FIXED_ADDR_UNSPECIFIED, remSolicitAdd);
                        (void)TCPIP_ICMPV6_Flush (pkt);
                    }
                    else
                    {
                        sllaOption.vType = (uint8_t)NDP_OPTION_TYPE_LLA_SOURCE;
                        sllaOption.vLength = 1;
                        (void)memcpy(sllaOption.mLinkLayerAddr.v, pNetIf->netMACAddr.v, sizeof (TCPIP_MAC_ADDR));

                        pRsStruct = pRs->address; 
                        pRsAdd = &pRsStruct->address;
                        pkt = TCPIP_ICMPV6_HeaderRouterSolicitationPut (pNetIf, pRsAdd, remSolicitAdd);
                        if (pkt == NULL)
                        {
                            return;
                        }
                        if (TCPIP_IPV6_TxIsPutReady(pkt, (uint16_t)sizeof(NDP_OPTION_LLA)) != 0U)
                        {
                            (void)TCPIP_IPV6_PutArray(pkt, (uint8_t*)&sllaOption, (uint16_t)sizeof(NDP_OPTION_LLA));
                        }
                        else
                        {
                            TCPIP_IPV6_PacketFree (pkt);
                            return;
                        }
                        (void)TCPIP_ICMPV6_Flush (pkt);
                    }
                }
                else
                {
                    sllaOption.vType = (uint8_t)NDP_OPTION_TYPE_LLA_SOURCE;
                    sllaOption.vLength = 1;
                    (void)memcpy(sllaOption.mLinkLayerAddr.v, pNetIf->netMACAddr.v, sizeof (TCPIP_MAC_ADDR));
                    // The previously selected IP address is still valid; use it
                    pRsStruct = pRs->address; 
                    pRsAdd = &pRsStruct->address;
                    pkt = TCPIP_ICMPV6_HeaderRouterSolicitationPut (pNetIf, pRsAdd, remSolicitAdd);
                    if (pkt == NULL)
                    {
                        return;
                    }
                    if (TCPIP_IPV6_TxIsPutReady(pkt, (uint16_t)sizeof(NDP_OPTION_LLA)) != 0U)
                    {
                        (void)TCPIP_IPV6_PutArray(pkt, (uint8_t*)&sllaOption, (uint16_t)sizeof(NDP_OPTION_LLA));
                    }
                    else
                    {
                        TCPIP_IPV6_PacketFree (pkt);
                        return;
                    }
                    (void)TCPIP_ICMPV6_Flush (pkt);
                }
                pRs->transmitAttempt++;
                pRs->timer = SYS_TMR_TickCountGet() + (sysFreq * (uint32_t)TCPIP_IPV6_NDP_RTR_SOLICITATION_INTERVAL);
                pRs->state = RS_STATE_WAIT;
                break;

            case (uint8_t)RS_STATE_WAIT:
                if (FC_Ui322I32(SYS_TMR_TickCountGet() - pRs->timer) > 0)

                {
                    if (pRs->transmitAttempt == (uint32_t)TCPIP_IPV6_NDP_MAX_RTR_SOLICITATIONS)
                    {
                        pRs->state = RS_STATE_INACTIVE;
                        ndpRSCount--;
                    }
                    else
                    {
                        pRs->state = RS_STATE_TRANSMIT;
                    }
                }
                break;

            default:
                TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
                break;

        }
    }
}

/*****************************************************************************
  Function:
    void TCPIP_NDP_RouterSolicitStop (TCPIP_NET_IF * pNetIf)

  Summary:
    Stops router solicitation.

  Description:
    This function will stop router solicitation.  It will be called if the
    soliciting interface receives a router advertisement.

  Precondition:
    None

  Parameters:
    pNetIf - The interface soliciting for routers.

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void TCPIP_NDP_RouterSolicitStop (const TCPIP_NET_IF * pNetIf)
{
    RS_STATIC_VARS* pRs;

    if (pNetIf == NULL)
    {
        return;
    }

    pRs = gRSState + TCPIP_STACK_NetIxGet(pNetIf);

    // Make sure we've sent AT LEAST 1 Router Solicitation
    // If so, disable router solicitations
    if (pRs->transmitAttempt != 0U)
    {
        ndpRSCount--;
        pRs->state = RS_STATE_INACTIVE;
    }
}

/*****************************************************************************
  Function:
    void * TCPIP_NDP_PrefixFind (TCPIP_NET_IF * pNetIf, IPV6_ADDR * prefix,
                                    uint8_t prefixLength,
                                    bool usePrefixList)

  Summary:
    Finds a prefix.

  Description:
    This function either finds a prefix with a set length in the prefix list
    or finds an address that was created using the specified prefix.

  Precondition:
    None

  Parameters:
    pNetIf - The interface of the prefix.
    prefix - The prefix.
    prefixLength - The length of the prefix.
    usePrefixList - true if finding a matching prefix in the prefix list,
                    false if finding an address autoconfigured using the prefix

  Returns:
    void * - IPV6_HEAP_NDP_PL_ENTRY * or IPV6_ADDR_STRUCT * of any matching
                nodes

  Remarks:
    None
  ***************************************************************************/
static void * TCPIP_NDP_PrefixFind (const TCPIP_NET_IF * pNetIf, IPV6_ADDR * prefix, uint8_t prefixLength, bool usePrefixList)
{
    IPV6_HEAP_NDP_PL_ENTRY * prefixPointer;
    IPV6_ADDR_STRUCT * localAddressPointer;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;

    if (pNetIf == NULL)
    {
        return NULL;
    }

    pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pNetIf);

    prefixPointer = FC_SglNode2NdpEntry(pIpv6Config->listPrefixList.head).pPlEntry;
    localAddressPointer = FC_DblNode2AddStruct(pIpv6Config->listIpv6UnicastAddresses.head);

    if (usePrefixList != false)
    {
        // Find a matching prefix in the prefix list
        while (prefixPointer != NULL)
        {
            if (prefixLength == prefixPointer->prefixLength)
            {
                if (memcmp (prefix->v, prefixPointer->prefix.v, sizeof (IPV6_ADDR)) == 0)
                {
                    return prefixPointer;
                }
            }
            prefixPointer = prefixPointer->next;
        }
    }
    else
    {
        // Find an address that matches the prefix that was created using Stateless Address Autoconfiguration
        while (localAddressPointer != NULL)
        {
            if (localAddressPointer->prefixLen != 0U)
            {
                if (prefixLength == localAddressPointer->prefixLen)
                {
                    const IPV6_ADDR* pLclAdd = &localAddressPointer->address;
                    if (TCPIP_Helper_FindCommonPrefix (prefix->v, pLclAdd->v, (uint8_t)sizeof (IPV6_ADDR)) >= prefixLength)
                    {
                        return localAddressPointer;
                    }
                }
            }
            localAddressPointer = localAddressPointer->next;
        }
    }

    return NULL;
}

/*****************************************************************************
  Function:
    void * TCPIP_NDP_RemoteNodeFind (TCPIP_NET_IF * pNetIf, IPV6_ADDR * source,
                                        uint8_t type)

  Summary:
    Finds an entry in the default router list, destination cache, or
    neighbor cache by address.

  Description:
    Finds an entry in the default router list, destination cache, or
    neighbor cache by address.

  Precondition:
    None

  Parameters:
    pNetIf - Interface of the remote node.
    source - The neighbor or router address, or the destination address.
    type - IPV6_HEAP_NDP_DR_ID, IPV6_HEAP_NDP_DC_ID, IPV6_HEAP_NDP_NC_ID

  Returns:
    void * - Pointer to a matching IPV6_HEAP_NDP_DR_ENTRY,
                IPV6_HEAP_NDP_NC_ENTRY, or IPV6_HEAP_NDP_DC_ENTRY structure,
                or NULL.

  Remarks:
    None
  ***************************************************************************/
void * TCPIP_NDP_RemoteNodeFind (const TCPIP_NET_IF * pNetIf, const IPV6_ADDR * source, uint8_t type)
{
    IPV6_INTERFACE_CONFIG*  pIpv6Config;

    if (pNetIf == NULL)
    {
        return NULL;
    }

    pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pNetIf);

    if(type == (uint8_t)IPV6_HEAP_NDP_DR_ID)
    {
        IPV6_HEAP_NDP_DR_ENTRY* pDrEntry = FC_SglNode2NdpEntry(pIpv6Config->listDefaultRouter.head).pDrEntry;
        while (pDrEntry != NULL)
        {
            if (pDrEntry->neighborInfo != NULL)
            {
                if (memcmp (source->v, pDrEntry->neighborInfo->remoteIPAddress.v, sizeof (IPV6_ADDR)) == 0)
                {
                    return (void*)pDrEntry;
                }
            }
            pDrEntry = pDrEntry->next;
        }
    }
    else if(type == (uint8_t)IPV6_HEAP_NDP_NC_ID)
    {
        IPV6_HEAP_NDP_NC_ENTRY* pNcEntry = FC_SglNode2NdpEntry(pIpv6Config->listNeighborCache.head).pNcEntry;
        while (pNcEntry != NULL)
        {
            if (memcmp (source->v, pNcEntry->remoteIPAddress.v, sizeof (IPV6_ADDR)) == 0)
            {
                return (void*)pNcEntry;
            }
            pNcEntry = pNcEntry->next;
        }
    }
    else if(type == (uint8_t)IPV6_HEAP_NDP_DC_ID)
    {
        IPV6_HEAP_NDP_DC_ENTRY* pDcEntry = FC_SglNode2NdpEntry(pIpv6Config->listDestinationCache.head).pDcEntry;
        while (pDcEntry != NULL)
        {
            if (memcmp (source->v, pDcEntry->remoteIPAddress.v, sizeof (IPV6_ADDR)) == 0)
            {
                return (void*)pDcEntry;
            }
            pDcEntry = pDcEntry->next;
        }
    }
    else
    {
        // do nothing
    }

    return NULL;
}

/*****************************************************************************
  Function:
    void TCPIP_NDP_ReachabilitySet (TCPIP_NET_IF * pNetIf,
                                    IPV6_HEAP_NDP_NC_ENTRY * neighborPointer,
                                    uint8_t newState)


  Summary:
    Sets the neighbor unreachability detection state of a neighbor.

  Description:
    Sets the neighbor unreachability detection state of a neighbor.
    Initializes NUD time if necessary.

  Precondition:
    None

  Parameters:
    pNetif - Interface of the neighbor.
    neighborPointer - The neighbor to update the state of.
    newState - The new NUD state, NEIGHBOR_UNREACHABILITY_DETECT_STATE value

  Returns:
    None.

  Remarks:
    None
  ***************************************************************************/
void TCPIP_NDP_ReachabilitySet (const TCPIP_NET_IF * pNetIf, IPV6_HEAP_NDP_NC_ENTRY * neighborPointer, uint8_t newState)
{
    neighborPointer->reachabilityState = newState;
    neighborPointer->staleStateTimeout = 0;
    if (newState == (uint8_t)NDP_STATE_DELAY)
    {
        ndpNUDCount++;
        neighborPointer->unansweredProbes = 0U;
        neighborPointer->nextNUDTime = SYS_TMR_TickCountGet() + (sysFreq * (uint32_t)TCPIP_IPV6_NDP_DELAY_FIRST_PROBE_TIME);

    }
    else if (newState == (uint8_t)NDP_STATE_REACHABLE)
    {
        neighborPointer->nextNUDTime = SYS_TMR_TickCountGet() + (TCPIP_IPV6_InterfaceConfigGet(pNetIf)->reachableTime * sysFreq);
        neighborPointer->flags.bResolvingAddress = 0U;
    }
    else if (newState == (uint8_t)NDP_STATE_STALE)
    {
        neighborPointer->staleStateTimeout = SYS_TMR_TickCountGet() + (sysFreq * (uint32_t)TCPIP_IPV6_NEIGHBOR_CACHE_ENTRY_STALE_TIMEOUT);
    }
    else
    {
        // do nothing
    }
}

/*****************************************************************************
  Function:
    IPV6_HEAP_NDP_NC_ENTRY * TCPIP_NDP_NextHopGet (TCPIP_NET_IF * pNetIf,
                                                    IPV6_ADDR * address)

  Summary:
    Determines the next hop for a given address.

  Description:
    Determines the next hop neighbor for a given address.

  Precondition:
    None

  Parameters:
    pNetIf - The interface of the address.
    address - The destination address.

  Returns:
    IPV6_HEAP_NDP_NC_ENTRY * - Pointer to the next-hop neighbor for the
                                given address.

  Remarks:
    None
  ***************************************************************************/
IPV6_HEAP_NDP_NC_ENTRY * TCPIP_NDP_NextHopGet (const TCPIP_NET_IF * pNetIf, const IPV6_ADDR * address)
{
    IPV6_HEAP_NDP_NC_ENTRY * neighborPointer;
    IPV6_HEAP_NDP_DC_ENTRY * destinationPointer;
    IPV6_HEAP_NDP_DR_ENTRY * routerPointer;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;

    if (address->v[0] == 0xFFU)
    {
        // Use on-link multicast
        return NULL;
    }

    pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pNetIf);
    destinationPointer = FC_VPtr2NdpEntry(TCPIP_NDP_RemoteNodeFind (pNetIf, address, (uint8_t)IPV6_HEAP_NDP_DC_ID)).pDcEntry;
    if (destinationPointer != NULL)
    {
        if (destinationPointer->nextHopNeighbor != NULL)
        {
            return destinationPointer->nextHopNeighbor;
        }
    }

    // Determine if the prefix is on-link
    if (!TCPIP_NDP_PrefixOnLinkStatusGet (pNetIf, address))
    {
        // Destination is probably off-link
        // Use a default router for now
        // If it is on-link, one of the routers
        // will fix it with a Redirect message
        if ((routerPointer = TCPIP_NDP_DefaultRouterGet(pNetIf)) != NULL)
        {
            if (routerPointer->neighborInfo != NULL)
            {
                (void)TCPIP_NDP_DestCacheEntryCreate (pNetIf, address, pIpv6Config->linkMTU, routerPointer->neighborInfo);
                return routerPointer->neighborInfo;
            }
        }
    }

    // Destination is either on-link or there are no
    // default routers available.  In either case,
    // treat the destination address as if it was on-link.
    // Use the destination address as the next-hop.

    neighborPointer = FC_VPtr2NdpEntry(TCPIP_NDP_RemoteNodeFind(pNetIf, address, (uint8_t)IPV6_HEAP_NDP_NC_ID)).pNcEntry;
    if (neighborPointer != NULL)
    {
        // This entry already has a neighbor cache entry
        (void)TCPIP_NDP_DestCacheEntryCreate (pNetIf, address, pIpv6Config->linkMTU, neighborPointer);
        return neighborPointer;
    }
    else
    {
        if ((neighborPointer = TCPIP_NDP_NborEntryCreate (pNetIf, address, NULL, (uint8_t)NDP_STATE_NONE, 0U, NULL)) != NULL)
        {
            (void)TCPIP_NDP_DestCacheEntryCreate (pNetIf, address, pIpv6Config->linkMTU, neighborPointer);
            return neighborPointer;
        }
        else
        {
            return NULL;
        }
    }
}

#if defined(TCPIP_IPV6_G3_PLC_SUPPORT) && (TCPIP_IPV6_G3_PLC_SUPPORT != 0)
// if address is a G3_PLC neighbor address it returns true, and if pMacAdd != 0, it will fill it with the neighbor's MAC address
// otherwise returns false
bool TCPIP_NDP_IsG3PLC_Neighbor(const TCPIP_NET_IF * pNetIf, const IPV6_ADDR * address, TCPIP_MAC_ADDR* pMacAdd)
{
    IPV6_INTERFACE_CONFIG*  pIpv6Config;

    if (address->v[0] == 0xFFU)
    {   // multicast address, not unicast
        return false;
    }

    pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pNetIf);

    if((pNetIf->startFlags & (uint16_t)TCPIP_NETWORK_CONFIG_IPV6_G3_NET) != 0U)
    {   // G3-PLC interface
        uint8_t g3LLAddress[8] = {0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        if(memcmp(address->v, g3LLAddress, sizeof(g3LLAddress)) == 0)
        {   // LLA: potential G3-PLC neighbor address if PAN_Id matches
            if(pIpv6Config->g3PanIdSet != 0U)
            {   // check that the address host part is 'PAN_id:00ff:fe00:xxxx'; the last 2 bytes are the 'G3 Short Address'
                if(TCPIP_Helper_ntohs(address->w[4]) == pIpv6Config->g3PanId && TCPIP_Helper_ntohs(address->w[5]) == 0x00ffU && TCPIP_Helper_ntohs(address->w[6]) == 0xfe00U)
                {   // proper G3-PLC neighbor address
                    if(pMacAdd != NULL)
                    {   // the MAC address is the last 6 bytes of the IPv6 address!
                        (void)memcpy(pMacAdd->v, address->v + (16 - 6), sizeof(TCPIP_MAC_ADDR));
                    }
                    return true;
                }
            }
        }
    }

    return false;
}
#endif  // defined(TCPIP_IPV6_G3_PLC_SUPPORT) && (TCPIP_IPV6_G3_PLC_SUPPORT != 0)


/*****************************************************************************
  Function:
    void TCPIP_NDP_SAAPrefixInfoProcess (TCPIP_NET_IF * pNetIf,
                                            NDP_OPTION_PREFIX_INFO * prefixInfo)

  Summary:
    Processes a prefix information option from a router advertisement.

  Description:
    Processes a prefix information option from a router advertisement
    according to the rules of stateless address autoconfiguration.

  Precondition:
    None

  Parameters:
    pNetIf - The interface the option was received from.
    prefixInfo - Pointer to the prefix information option.

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void TCPIP_NDP_SAAPrefixInfoProcess (const TCPIP_NET_IF * pNetIf, NDP_OPTION_PREFIX_INFO * prefixInfo)
{
    IPV6_ADDR_STRUCT * localAddressPointer;
    IPV6_ADDR* pPrefixAdd;

    if ((prefixInfo->flags.bA != 0U) && !((prefixInfo->aPrefix.v[0] == 0xFEU) && ((prefixInfo->aPrefix.v[1] & 0xC0U) == 0x80U)))
    {
        // Check to ensure that the prefix option preferred lifetime is less than the valid lifetime
        if (prefixInfo->dPreferredLifetime <= prefixInfo->dValidLifetime)
        {
            pPrefixAdd = &prefixInfo->aPrefix;
            if ((localAddressPointer = TCPIP_NDP_PrefixFind (pNetIf, pPrefixAdd, prefixInfo->vPrefixLen, false)) == NULL)
            {
                // Prefix is not equal to the prefix of an address configured by Stateless Address Autoconfiguration
                if (prefixInfo->dValidLifetime != 0U)
                {
                    if (prefixInfo->vPrefixLen + IPV6_INTERFACE_ID_SIZE != 128U)
                    {
                        return;
                    }
                    pPrefixAdd = &prefixInfo->aPrefix;
                    if (TCPIP_NDP_AddressConstructFromPrefix (pNetIf, &tempAddress, pPrefixAdd, prefixInfo->vPrefixLen) == true)
                    {
                        localAddressPointer = TCPIP_IPV6_UnicastAddressAdd (pNetIf, &tempAddress, 0, 0U);
                        if (localAddressPointer != NULL)
                        {
                            localAddressPointer->prefixLen = prefixInfo->vPrefixLen;
                            localAddressPointer->preferredLifetime = prefixInfo->dPreferredLifetime;
                            localAddressPointer->validLifetime = prefixInfo->dValidLifetime;
                            localAddressPointer->lastTickTime = SYS_TMR_TickCountGet();

                        }
                    }
                }
            }
            else
            {
                // Prefix is equal to the prefix of an address configured by Stateless Address Autoconfiguration
                localAddressPointer->preferredLifetime = prefixInfo->dPreferredLifetime;
                if ((prefixInfo->dValidLifetime > FC_I322Ui32(TCPIP_IPV6_NDP_VALID_LIFETIME_TWO_HOURS)) ||
                    (prefixInfo->dValidLifetime > localAddressPointer->validLifetime))
                {
                    localAddressPointer->validLifetime = prefixInfo->dValidLifetime;
                }
                else if (localAddressPointer->validLifetime > FC_I322Ui32(TCPIP_IPV6_NDP_VALID_LIFETIME_TWO_HOURS))
                {
                    localAddressPointer->validLifetime = FC_I322Ui32(TCPIP_IPV6_NDP_VALID_LIFETIME_TWO_HOURS);
                }
                else
                {
                    // If remaining lifetime is <= 2 hours, ignore the prefix info option
                }
            }
        }
    }
}

/*****************************************************************************
  Function:
    void TCPIP_NDP_PrefixInfoProcessForOnLinkStatus (
        TCPIP_NET_IF * pNetIf, NDP_OPTION_PREFIX_INFO * prefixInfo)

  Summary:
    Processes a prefix information option from a router advertisement.

  Description:
    Processes a prefix information option from a router advertisement
    according to the rules of on-link address determination.

  Precondition:
    None

  Parameters:
    pNetIf - The interface the option was received from.
    prefixInfo - Pointer to the prefix information option.

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void TCPIP_NDP_PrefixInfoProcessForOnLinkStatus (const TCPIP_NET_IF * pNetIf, NDP_OPTION_PREFIX_INFO * prefixInfo)
{
    IPV6_HEAP_NDP_PL_ENTRY * prefixPointer;
    IPV6_ADDR* pPrefixAdd;

    pPrefixAdd = &prefixInfo->aPrefix;

    if ((prefixInfo->flags.bL != 0U) && !((prefixInfo->aPrefix.v[0] == 0xFEU) && ((prefixInfo->aPrefix.v[1] & 0xC0U) == 0x80U)))
    {
        if ((prefixPointer = TCPIP_NDP_PrefixFind (pNetIf, pPrefixAdd, prefixInfo->vPrefixLen, true)) == NULL)
        {
            if (prefixInfo->dValidLifetime != 0U)
            {
                // Store this prefix in the heap so it can be used to determine if
                // addresses are on-link
                prefixPointer = TCPIP_NDP_PrefixListEntryCreate (pNetIf, pPrefixAdd, prefixInfo->vPrefixLen, prefixInfo->dValidLifetime);
            }
        }
        else
        {
            if (prefixInfo->dValidLifetime != 0U)
            {
                prefixPointer->validLifetime = prefixInfo->dValidLifetime;
            }
            else
            {
                (void)TCPIP_NDP_LinkedListEntryRemove (pNetIf, prefixPointer, (uint8_t)IPV6_HEAP_NDP_PL_ID);
            }
        }
    }
}

/*****************************************************************************
  Function:
    bool TCPIP_NDP_PrefixOnLinkStatusGet (TCPIP_NET_IF * pNetIf,
        IPV6_ADDR * address)

  Summary:
    Determines if an address is on-link

  Description:
    This function determines if a given unicast address should be treated
    as an on-link address.

  Precondition:
    None

  Parameters:
    pNetIf - Interface of the address.
    address - The address to check.

  Returns:
    true if address on-link
    false otherwise

  Remarks:
    None
  ***************************************************************************/
bool TCPIP_NDP_PrefixOnLinkStatusGet (const TCPIP_NET_IF * pNetIf, const IPV6_ADDR * address)
{
    IPV6_HEAP_NDP_PL_ENTRY * prefixPointer;

    if (pNetIf == NULL)
    {
        return false;
    }

    prefixPointer = FC_SglNode2NdpEntry(TCPIP_IPV6_InterfaceConfigGet(pNetIf)->listPrefixList.head).pPlEntry;

    if ((address->v[0] == 0xFEU) && ((address->v[1] & 0xC0U) == 0x80U))
    {
        return true;
    }

    while (prefixPointer != NULL)
    {
        if (TCPIP_Helper_FindCommonPrefix (address->v, prefixPointer->prefix.v, (uint8_t)sizeof (IPV6_ADDR)) >= prefixPointer->prefixLength)
        {
            return true;
        }
        prefixPointer = prefixPointer->next;
    }

    return false;
}

/*****************************************************************************
  Function:
    bool TCPIP_NDP_AddressConstructFromPrefix (TCPIP_NET_IF * pNetIf, IPV6_ADDR * destination, IPV6_ADDR * prefix, uint8_t prefixLength)

  Summary:
    Constructs an IPv6 address from a given prefix.

  Description:
    This function will construct an address from a given 64-bit prefix.

  Precondition:
    The prefix length must be 64 bits.

  Parameters:
    pNetIf - Interface for the address.
    destination - Return value for the constructed address.
    prefix - Prefix.
    prefixLength - Length of the prefix (should be 8 bytes)

  Returns:
    true  - On success
    false - Failure to indicate an invalid address  

  Remarks:
    None
  ***************************************************************************/
static bool TCPIP_NDP_AddressConstructFromPrefix (const TCPIP_NET_IF * pNetIf, IPV6_ADDR * destination, IPV6_ADDR * prefix, uint8_t prefixLength)
{
    (void)memcpy (destination->v, prefix->v, sizeof (IPV6_ADDR));

    // We'll never construct an address from a prefix
    // that isn't 64 bits (since our interface ID is also 64 bits).
    if ((prefixLength & 0x07U) == 0U)
    {
        return TCPIP_IPV6_AddressInterfaceIDSet(pNetIf, destination, prefixLength, 0);
    }

    return false;
}

/*****************************************************************************
  Function:
    void TCPIP_NDP_NborCacheLinkLayerAddressUpdate (TCPIP_NET_IF * pNetIf,
        IPV6_HEAP_NDP_NC_ENTRY * neighborPointer, TCPIP_MAC_ADDR * linkLayerAddr,
        uint8_t reachability)

  Summary:
    Updates the link-layer address of a neighbor cache entry.

  Description:
    Updates the link-layer address of a neighbor cache entry.  If the entry
    has queued packets, they will be transmitted if a valid link-layer
    address is included.

  Precondition:
    None

  Parameters:
    pNetIf - The interface of the neighbor.
    neighborPointer - The neighbor to update.
    linkLayerAddr - The new link-layer address of the node.
    reachability - The NUD reachability state (NDP_STATE_NONE if this
                    shouldn't be set)

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void TCPIP_NDP_NborCacheLinkLayerAddressUpdate (const TCPIP_NET_IF * pNetIf, IPV6_HEAP_NDP_NC_ENTRY * neighborPointer, TCPIP_MAC_ADDR * linkLayerAddr, uint8_t reachability)
{
    (void)memcpy (neighborPointer->remoteMACAddr.v, linkLayerAddr->v, sizeof (TCPIP_MAC_ADDR));
    if (reachability != (uint8_t)NDP_STATE_NONE)
    {
        TCPIP_NDP_ReachabilitySet (pNetIf, neighborPointer, reachability);
    }

    switch (neighborPointer->reachabilityState)
    {
        case (uint8_t)NDP_STATE_STALE:
        case (uint8_t)NDP_STATE_REACHABLE:
        case (uint8_t)NDP_STATE_DELAY:
        case (uint8_t)NDP_STATE_PROBE:

            if(neighborPointer->reachabilityState == (uint8_t)NDP_STATE_STALE)
            {
                TCPIP_NDP_ReachabilitySet (pNetIf, neighborPointer, (uint8_t)NDP_STATE_DELAY);
            }

            while (neighborPointer->queuedPackets.nNodes != 0U)
            {
                (void)memcpy (FC_SglNode2Ip6Pkt(neighborPointer->queuedPackets.head)->remoteMACAddr.v, neighborPointer->remoteMACAddr.v, sizeof(TCPIP_MAC_ADDR));
                if (TCPIP_IPV6_PacketTransmit (FC_SglNode2Ip6Pkt(neighborPointer->queuedPackets.head)))
                {
                    IPV6_PACKET * pkt;
                    pkt = FC_SglNode2Ip6Pkt(TCPIP_Helper_SingleListHeadRemove(&neighborPointer->queuedPackets));
                    TCPIP_IPV6_PacketAck(pkt, false);
                }
            }
            break;

        default:
            // do nothing
            break;
    }
}

/*****************************************************************************
  Function:
    void TCPIP_NDP_NborUnreachDetectTask (void)

  Summary:
    Neighbor Unreachability Detection task function.

  Description:
    Neighbor Unreachability Detection task function.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
static void TCPIP_NDP_NborUnreachDetectTask (void)
{
    IPV6_HEAP_NDP_NC_ENTRY * neighborPointer;
    IPV6_HEAP_NDP_NC_ENTRY * lastNeighborPointer;
    size_t netIx;
    const TCPIP_NET_IF * pNetIf;
    uint32_t currTime = SYS_TMR_TickCountGet();

    NDP_OPTION_LLA sllaOption;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;
    IPV6_PACKET * pkt;
    IPV6_ADDR *pPrefAdd, *pSrcAdd;

    ndpNUDCount = 0;

    for(netIx=0; netIx < nStackIfs; netIx++)
    {
        pNetIf = (const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet (netIx);
        pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pNetIf);
        neighborPointer = FC_SglNode2NdpEntry(pIpv6Config->listNeighborCache.head).pNcEntry;

        while (neighborPointer != NULL)
        {
            lastNeighborPointer = neighborPointer;
            switch (neighborPointer->reachabilityState)
            {
                case (uint8_t)NDP_STATE_FAILED:
                    neighborPointer->flags.bResolvingAddress = 0U;
                    while (neighborPointer->queuedPackets.nNodes != 0U)
                    {
                        pkt = FC_SglNode2Ip6Pkt(TCPIP_Helper_SingleListHeadRemove(&neighborPointer->queuedPackets));
                        TCPIP_IPV6_PacketAck(pkt, false);
                    }
                    neighborPointer = TCPIP_NDP_NborEntryDelete (pNetIf, neighborPointer);
                    break;

                case (uint8_t)NDP_STATE_REACHABLE:
                    if(neighborPointer->flags.bIsPerm == 0U)
                    {
                        if (FC_Ui322I32(currTime - neighborPointer->nextNUDTime) > 0)
                        {
                            TCPIP_NDP_ReachabilitySet (pNetIf, neighborPointer, (uint8_t)NDP_STATE_STALE);
                            break;
                        }
                    }

                    while (neighborPointer->queuedPackets.nNodes != 0U)
                    {
                        if (TCPIP_IPV6_PacketTransmit (FC_SglNode2Ip6Pkt(neighborPointer->queuedPackets.head)))
                        {
                            pkt = FC_SglNode2Ip6Pkt(TCPIP_Helper_SingleListHeadRemove(&neighborPointer->queuedPackets));
                            TCPIP_IPV6_PacketAck(pkt, true);
                        }
                        else
                        {
                            break;
                        }
                    }
                    break;

                case (uint8_t)NDP_STATE_STALE:
                    while (neighborPointer->queuedPackets.nNodes != 0U)
                    {
                        // Try to transmit the packet if we have a link-layer address for this node
                        if (TCPIP_IPV6_PacketTransmit (FC_SglNode2Ip6Pkt(neighborPointer->queuedPackets.head)))
                        {
                            pkt = FC_SglNode2Ip6Pkt(TCPIP_Helper_SingleListHeadRemove(&neighborPointer->queuedPackets));
                            TCPIP_IPV6_PacketAck(pkt, true);
                            TCPIP_NDP_ReachabilitySet (pNetIf, neighborPointer, (uint8_t)NDP_STATE_DELAY);
                        }
                        else
                        {
                            break;
                        }
                    }

                    if (neighborPointer->staleStateTimeout != 0U)
                    {
                        // Check to see if the neighbor cache entry has timed out
                        if (FC_Ui322I32(currTime - neighborPointer->staleStateTimeout) > 0)
                        {
                            // Remove this node if it isn't being used as a router.
                            if (neighborPointer->flags.bIsRouter == 0U)
                            {
                                neighborPointer = TCPIP_NDP_NborEntryDelete (pNetIf, neighborPointer);
                            }
                        }
                    }
                    break;

                case (uint8_t)NDP_STATE_DELAY:
                case (uint8_t)NDP_STATE_PROBE:
                    if(neighborPointer->reachabilityState == (uint8_t)NDP_STATE_DELAY)
                    {
                        ndpNUDCount++;
                        if (FC_Ui322I32(currTime - neighborPointer->nextNUDTime) > 0)
                        {
                            TCPIP_NDP_ReachabilitySet (pNetIf, neighborPointer, (uint8_t)NDP_STATE_PROBE);
                        }
                        else
                        {
                            break;
                        }
                    }
                    // Fall through if we've entered the probe state
                    ndpNUDCount++;
                    if (FC_Ui322I32(SYS_TMR_TickCountGet() - neighborPointer->nextNUDTime) > 0)
                    {
                        if (neighborPointer->unansweredProbes < (uint8_t)TCPIP_IPV6_NDP_MAX_UNICAST_SOLICIT)
                        {
                            IPV6_ADDR_STRUCT * sourceAddress = NULL;

                            if (neighborPointer->preferredSource != NULL)
                            {
                                pPrefAdd = &neighborPointer->preferredSource->address;
                                sourceAddress = TCPIP_IPV6_DASSourceAddressSelect (pNetIf, &neighborPointer->remoteIPAddress, pPrefAdd);
                            }
                            if (sourceAddress == NULL)
                            {
                                sourceAddress = TCPIP_IPV6_DASSourceAddressSelect (pNetIf, &neighborPointer->remoteIPAddress, NULL);
                            }
                            if (sourceAddress == NULL)
                            {
                                neighborPointer->nextNUDTime = SYS_TMR_TickCountGet();

                                break;
                            }

                            sllaOption.vType = (uint8_t)NDP_OPTION_TYPE_LLA_SOURCE;
                            sllaOption.vLength = 1;
                            (void)memcpy (sllaOption.mLinkLayerAddr.v, pNetIf->netMACAddr.v, sizeof (TCPIP_MAC_ADDR));


                            pSrcAdd = &sourceAddress->address;
                            pkt = TCPIP_ICMPV6_HeaderNeighborSolicitationPut (pNetIf, pSrcAdd, &(neighborPointer->remoteIPAddress), &(neighborPointer->remoteIPAddress));
                            if (pkt == NULL)
                            {
                                // If we can't allocate a transmit packet structure,
                                // just reset the next probe timer to the current time.
                                // This will prevent rollover issues if we're unable to
                                // send a packet for a very very long amount of time.
                                neighborPointer->nextNUDTime = SYS_TMR_TickCountGet();

                            }
                            else
                            {
                                if (TCPIP_IPV6_TxIsPutReady(pkt, (uint16_t)sizeof(NDP_OPTION_LLA)) != 0U)
                                {
                                    (void)TCPIP_IPV6_PutArray(pkt, (uint8_t*)&sllaOption, (uint16_t)sizeof(NDP_OPTION_LLA));
                                    pkt->neighbor = neighborPointer;
                                    (void)TCPIP_ICMPV6_Flush (pkt);
                                    neighborPointer->unansweredProbes++;
                                    neighborPointer->nextNUDTime = SYS_TMR_TickCountGet() + ((TCPIP_IPV6_InterfaceConfigGet(pNetIf)->retransmitTime * sysFreq) / 1000U);

                                }
                                else
                                {
                                    TCPIP_IPV6_PacketFree (pkt);
                                }
                            }
                        }
                        else
                        {
                            // The maximum number of probes has been sent.
                            // Remove this cache entry.  Subsequent traffic to the
                            // associated remote node will recreate it and re-perform
                            // address resolution if necessary.
                            neighborPointer = TCPIP_NDP_NborEntryDelete (pNetIf, neighborPointer);
                        }
                    }
                    break;

                case (uint8_t)NDP_STATE_INCOMPLETE:
                    if (FC_Ui322I32(currTime - neighborPointer->nextNUDTime) > 0)
                    {
                        if (neighborPointer->unansweredProbes == (uint8_t)TCPIP_IPV6_NDP_MAX_MULTICAST_SOLICIT)
                        {
                            // The maximum number of probes has been sent.
                            // Remove this cache entry.  Subsequent traffic to the
                            // associated remote node will recreate it and re-perform
                            // address resolution if necessary.
                            neighborPointer = TCPIP_NDP_NborEntryDelete (pNetIf, neighborPointer);
                        }
                        else
                        {
                            IPV6_ADDR_STRUCT * sourceAddress = NULL;
                            IPV6_ADDR solicitedNodeMulticastAddr = {{0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                                        0x00, 0x00, 0x00, 0x01, 0xFF,
                                                                        neighborPointer->remoteIPAddress.v[13],
                                                                        neighborPointer->remoteIPAddress.v[14],
                                                                        neighborPointer->remoteIPAddress.v[15]}};

                            if (neighborPointer->preferredSource != NULL)
                            {
                                pPrefAdd = &neighborPointer->preferredSource->address;
                                sourceAddress = TCPIP_IPV6_DASSourceAddressSelect (pNetIf, &neighborPointer->remoteIPAddress, pPrefAdd);
                            }
                            if (sourceAddress == NULL)
                            {
                                sourceAddress = TCPIP_IPV6_DASSourceAddressSelect (pNetIf, &neighborPointer->remoteIPAddress, NULL);
                            }
                            if (sourceAddress == NULL)
                            {
                                TCPIP_NDP_ReachabilitySet (pNetIf, neighborPointer, (uint8_t)NDP_STATE_FAILED);
                                break;
                            }
                            else
                            {
                                neighborPointer->preferredSource = sourceAddress;
                            }

                            sllaOption.vType = (uint8_t)NDP_OPTION_TYPE_LLA_SOURCE;
                            sllaOption.vLength = 1;
                            (void)memcpy (sllaOption.mLinkLayerAddr.v, pNetIf->netMACAddr.v, sizeof (TCPIP_MAC_ADDR));

                            pSrcAdd = &sourceAddress->address;
                            pkt = TCPIP_ICMPV6_HeaderNeighborSolicitationPut (pNetIf, pSrcAdd, &solicitedNodeMulticastAddr, &neighborPointer->remoteIPAddress);
                            if (pkt == NULL)
                            {
                                neighborPointer->nextNUDTime = SYS_TMR_TickCountGet();
                            }
                            else
                            {
                                if (TCPIP_IPV6_TxIsPutReady(pkt, (uint16_t)sizeof(NDP_OPTION_LLA)) != 0U)
                                {
                                    (void)TCPIP_IPV6_PutArray(pkt, (uint8_t*)&sllaOption, (uint16_t)sizeof(NDP_OPTION_LLA));
                                    (void)TCPIP_ICMPV6_Flush (pkt);

                                    neighborPointer->unansweredProbes++;
                                    neighborPointer->nextNUDTime = SYS_TMR_TickCountGet() + ((pIpv6Config->retransmitTime * sysFreq) / 1000U);

                                }
                                else
                                {
                                    TCPIP_IPV6_PacketFree (pkt);
                                }
                            }
                        }
                    }
                    break;

                default:
                    // do nothing
                    break;
            }

            if (neighborPointer == lastNeighborPointer)
            {
                neighborPointer = neighborPointer->next;
            }
        }
    }
}

/*****************************************************************************
  Function:
    void TCPIP_NDP_G3RouterAdvertiseTask (void)

  Summary:
    G3-PLC border router advertise task.

  Description:
    Router advertisement task for G3-PLC networks

  Remarks:
    - IP fields:
        - Source Address MUST be the link-local address assigned to the interface from which this message is sent.
        - Destination Address Typically the Source Address of an invoking Router Solicitation or the all-nodes multicast address.

    - Rather than iterating everytime through the unicast list,
      the LL and global unicast addresses could be memorated.
      However this is a low frequency event   
  ***************************************************************************/
#if defined(TCPIP_IPV6_G3_PLC_BORDER_ROUTER) && (TCPIP_IPV6_G3_PLC_BORDER_ROUTER != 0)
// set the advertisement timeout
static void TCPIP_NDP_AdvTimeSet(TCPIP_NET_IF* pNetIf, bool advCount)
{
    uint32_t advTmo = (uint32_t)TCPIP_IPV6_MIN_RTR_ADV_INTERVAL + SYS_RANDOM_PseudoGet() % ((uint32_t)TCPIP_IPV6_MAX_RTR_ADV_INTERVAL + 1U - (uint32_t)TCPIP_IPV6_MIN_RTR_ADV_INTERVAL); 
    if(pNetIf->advInitCount < (uint16_t)TCPIP_IPV6_MAX_INI_RTR_ADV_NO)
    {
        if(advTmo > FC_I322Ui32((int32_t)TCPIP_IPV6_MAX_INI_RTR_ADV_INT_NO))
        {
            advTmo = FC_I322Ui32((int32_t)TCPIP_IPV6_MAX_INI_RTR_ADV_INT_NO);
        } 
    }

    TCPIPStack_Assert(advTmo < 65536U, __FILE__, __func__, __LINE__);

    pNetIf->advTmo = (uint16_t)advTmo;
    if(advCount)
    {
        pNetIf->advInitCount++;
    }
}

static void TCPIP_NDP_G3RouterAdvertiseTask (void)
{
    TCPIP_NET_IF* pNetIf;
    const IPV6_ADDR_STRUCT* llNode;   // link local address of the interface
    const IPV6_ADDR_STRUCT* advNode;  // address to be advertised
    TCPIP_UINT32_VAL deltaTmo;

    size_t netIx;

    for(netIx = 0; netIx < nStackIfs; netIx++)
    {
        pNetIf = FC_NetH2NetIf(TCPIP_STACK_IndexToNet(netIx));
        if(!TCPIP_IPV6_InterfaceIsReady(pNetIf))
        {   // not ready yet
            continue;
        }

        if((pNetIf->startFlags & ((uint16_t)TCPIP_NETWORK_CONFIG_IPV6_ROUTER | (uint16_t)TCPIP_NETWORK_CONFIG_IPV6_ADV_ENABLED)) != ((uint16_t)TCPIP_NETWORK_CONFIG_IPV6_ROUTER | (uint16_t)TCPIP_NETWORK_CONFIG_IPV6_ADV_ENABLED))
        {   // no advertise on this interface
            continue;
        }

        // interface needs to advertise 
        uint32_t currSec = TCPIP_SecCountGet(); 
        if(pNetIf->advInitCount == 0U && pNetIf->advTmo == 0U)
        {   // this is the 1st time; just set the timeouts
            pNetIf->advLastSec = currSec;
            TCPIP_NDP_AdvTimeSet(pNetIf, false);
            continue;
        }

        // check if it's time to advertise
        deltaTmo.Val = currSec - pNetIf->advLastSec;
        // should be < than 65535 seconds!  
        TCPIPStack_Assert(deltaTmo.word.HW == 0U, __FILE__, __func__, __LINE__);
        pNetIf->advLastSec = currSec;   // update the last eval time
        if(pNetIf->advTmo > deltaTmo.word.LW)
        {   // not timeout yet
            pNetIf->advTmo -= deltaTmo.word.LW;
            continue;
        }

        // time to advertise
        TCPIP_ICMPV6_G3AdvertisementSelect(pNetIf, &llNode, &advNode);
        if(advNode != NULL && llNode != NULL)
        {   // advertise
            (void)TCPIP_ICMPV6_G3RouterAdvertisementPut(pNetIf, &llNode->address, &IPV6_FIXED_ADDR_ALL_NODES_MULTICAST, advNode);
            // adjust the timeout
            TCPIP_NDP_AdvTimeSet(pNetIf, true);
        }
    }
}
#endif  // defined(TCPIP_IPV6_G3_PLC_BORDER_ROUTER) && (TCPIP_IPV6_G3_PLC_BORDER_ROUTER != 0)

/*****************************************************************************
  Function:
    void TCPIP_NDP_AddressResolve (IPV6_HEAP_NDP_NC_ENTRY * entry)

  Summary:
    Perorms link-layer address resolution for a neighbor.

  Description:
    Begins link-layer address resolution for a given neighbor.

  Precondition:
    None

  Parameters:
    entry - The neighbor whose link-layer address must be resolved.

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void TCPIP_NDP_AddressResolve (IPV6_HEAP_NDP_NC_ENTRY * entry)
{
    if (entry->flags.bResolvingAddress != 0U)
    {
        return;
    }

    ndpNUDCount++;

    entry->unansweredProbes = 0U;
    entry->nextNUDTime = SYS_TMR_TickCountGet();

    entry->flags.bResolvingAddress = 1U;
}

/*****************************************************************************
  Function:
    void TCPIP_NDP_DestCacheUpdate (TCPIP_NET_IF * pNetIf,
        IPV6_HEAP_NDP_NC_ENTRY * neighborEntry)

  Summary:
    Removes entries from the destination cache if their next-hop neighbor
    becomes invalid.

  Description:
    Removes entries from the destination cache if their next-hop neighbor
    becomes invalid.

  Precondition:
    None

  Parameters:
    pNetIf - Interface for the given neighbor.
    neighbor - The neighbor that will become invalid.

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
static void TCPIP_NDP_DestCacheUpdate (const TCPIP_NET_IF * pNetIf, IPV6_HEAP_NDP_NC_ENTRY * neighborEntry)
{
    IPV6_HEAP_NDP_DC_ENTRY * destinationPointer;

    if (pNetIf == NULL)
    {
        return;
    }

    IPV6_INTERFACE_CONFIG*  pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pNetIf);

    destinationPointer = FC_SglNode2NdpEntry(pIpv6Config->listDestinationCache.head).pDcEntry;

    while (destinationPointer != NULL)
    {
        if (destinationPointer->nextHopNeighbor == neighborEntry)
        {   // remove node destinationPointer from the listDestinationCache
            IPV6_HEAP_NDP_DC_ENTRY* nextNode = destinationPointer->next;
            IPV6_HEAP_NDP_ENTRY_U ndpEntryU = FC_VPtr2NdpEntry((void*)destinationPointer); 
            (void)TCPIP_Helper_SingleListNodeRemove (&pIpv6Config->listDestinationCache, ndpEntryU.node);
            (void)TCPIP_HEAP_Free (ndpMemH, destinationPointer);
            destinationPointer = nextNode;
        }
        else
        {
            destinationPointer = destinationPointer->next;
        }
    }
}

/*****************************************************************************
  Function:
    void * TCPIP_NDP_LinkedListEntryRemove (TCPIP_NET_IF * pNetIf,
        void * entry, uint8_t type)

  Summary:
    Removes an entry from an NDP linked list and frees the memory used for
    that entry.

  Description:
    Removes an entry from an NDP linked list and frees the memory used for
    that entry.

  Precondition:
    None

  Parameters:
    pNetIf - The interface for the entry that will be removed.
    entry - Pointer to the entry that should be removed.
    type -  IPV6_HEAP_NDP_DR_ID - Default router list
            IPV6_HEAP_NDP_DC_ID - Destination Cache
            IPV6_HEAP_NDP_NC_ID - Neighbor Cache
            IPV6_HEAP_NDP_PL_ID - Prefix List
            IPV6_HEAP_ADDR_UNICAST_ID - Unicast local address
            IPV6_HEAP_ADDR_MULTICAST_ID - Multicast listener
            IPV6_HEAP_ADDR_UNICAST_TENTATIVE_ID - Tentative local unicast address

  Returns:
    void * - Pointer to the next node in the given list (or NULL if no more nodes)

  Remarks:
    None
  ***************************************************************************/
void * TCPIP_NDP_LinkedListEntryRemove (const TCPIP_NET_IF * pNetIf, void * entry, uint8_t type)
{
    void * nextNode;
    IPV6_HEAP_NDP_ENTRY_U   ndpEntryU, * pNdpEntryU;
    IPV6_HEAP_NDP_DR_ENTRY *pDrEntry, *nextDrEntry;

    IPV6_INTERFACE_CONFIG*  pIpv6Config;

    if ((entry == NULL) || (pNetIf == NULL))
    {
        return NULL;
    }

    pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pNetIf);
    ndpEntryU = FC_VPtr2NdpEntry(entry); 
    pNdpEntryU = &ndpEntryU;

    switch (type)
    {
        case (uint8_t)IPV6_HEAP_NDP_DR_ID:
            pDrEntry = pNdpEntryU->pDrEntry; 
            TCPIP_NDP_DestCacheUpdate (pNetIf, pDrEntry->neighborInfo);
            nextDrEntry = pDrEntry->next;
            (void)TCPIP_Helper_SingleListNodeRemove (&pIpv6Config->listDefaultRouter, pNdpEntryU->node);
            // Update the current default router if necessary
            if (pDrEntry == pIpv6Config->currentDefaultRouter)
            {
                if (nextDrEntry != NULL)
                {
                    pIpv6Config->currentDefaultRouter = nextDrEntry;
                }
                else
                {
                    pIpv6Config->currentDefaultRouter = FC_SglNode2NdpEntry(pIpv6Config->listDefaultRouter.head).pDrEntry;
                }
            }
            nextNode = (void*)nextDrEntry;
            break;

        case (uint8_t)IPV6_HEAP_NDP_DC_ID:
            nextNode = (void*)pNdpEntryU->pDcEntry->next;
            (void)TCPIP_Helper_SingleListNodeRemove (&pIpv6Config->listDestinationCache, pNdpEntryU->node);
            break;

        case (uint8_t)IPV6_HEAP_NDP_NC_ID:
            nextNode = (void*)pNdpEntryU->pNcEntry->next;
            (void)TCPIP_Helper_SingleListNodeRemove (&pIpv6Config->listNeighborCache, pNdpEntryU->node);
            break;

        case (uint8_t)IPV6_HEAP_NDP_PL_ID:
            nextNode = (void*)pNdpEntryU->pPlEntry->next;
            (void)TCPIP_Helper_SingleListNodeRemove (&pIpv6Config->listPrefixList, pNdpEntryU->node);
            break;

        case (uint8_t)IPV6_HEAP_ADDR_UNICAST_ID:
            nextNode = (void*)pNdpEntryU->pAddStr->next;
            TCPIP_Helper_DoubleListNodeRemove (&pIpv6Config->listIpv6UnicastAddresses, pNdpEntryU->dNode);
            break;

        case (uint8_t)IPV6_HEAP_ADDR_MULTICAST_ID:
            nextNode = (void*)pNdpEntryU->pAddStr->next;
            TCPIP_Helper_DoubleListNodeRemove (&pIpv6Config->listIpv6MulticastAddresses, pNdpEntryU->dNode);
            break;

        case (uint8_t)IPV6_HEAP_ADDR_UNICAST_TENTATIVE_ID:
            nextNode = (void*)pNdpEntryU->pAddStr->next;
            TCPIP_Helper_DoubleListNodeRemove (&pIpv6Config->listIpv6TentativeAddresses, pNdpEntryU->dNode);
            break;

        default:
            nextNode = NULL;
            break;
    }

    (void)TCPIP_HEAP_Free (ndpMemH, entry);

    return nextNode;
}

/*****************************************************************************
  Function:
    void TCPIP_NDP_LinkedListEntryInsert (TCPIP_NET_IF * pNetIf, void * entry,
        uint8_t type)

  Summary:
    Inserts an entry into an NDP linked list

  Description:
    Inserts an entry into an NDP linked list

  Precondition:
    None

  Parameters:
    pNetIf - The interface for the entry that will be inserted.
    entry - Pointer to the entry that will be inserted.
    type -  IPV6_HEAP_NDP_DR_ID - Default router list
            IPV6_HEAP_NDP_DC_ID - Destination Cache
            IPV6_HEAP_NDP_NC_ID - Neighbor Cache
            IPV6_HEAP_NDP_PL_ID - Prefix List
            IPV6_HEAP_ADDR_UNICAST_ID - Unicast local address
            IPV6_HEAP_ADDR_MULTICAST_ID - Multicast listener
            IPV6_HEAP_ADDR_UNICAST_TENTATIVE_ID - Tentative local unicast address

  Returns:
    void * - Pointer to the next node in the given list (or NULL if no more nodes)

  Remarks:
    None
  ***************************************************************************/
void TCPIP_NDP_LinkedListEntryInsert (const TCPIP_NET_IF * pNetIf, void * entry, uint8_t type)
{
    IPV6_INTERFACE_CONFIG*  pIpv6Config;

    if ((entry == NULL) || (pNetIf == NULL))
    {
        return;
    }

    pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pNetIf);

    switch (type)
    {
        case (uint8_t)IPV6_HEAP_NDP_DR_ID:
            TCPIP_Helper_SingleListTailAdd (&pIpv6Config->listDefaultRouter, entry);
            break;

        case (uint8_t)IPV6_HEAP_NDP_DC_ID:
            TCPIP_Helper_SingleListTailAdd (&pIpv6Config->listDestinationCache, entry);
            break;

        case (uint8_t)IPV6_HEAP_NDP_NC_ID:
            TCPIP_Helper_SingleListTailAdd (&pIpv6Config->listNeighborCache, entry);
            break
                ;
        case (uint8_t)IPV6_HEAP_NDP_PL_ID:
            TCPIP_Helper_SingleListTailAdd (&pIpv6Config->listPrefixList, entry);
            break
                ;
        case (uint8_t)IPV6_HEAP_ADDR_UNICAST_ID:
            TCPIP_Helper_DoubleListTailAdd (&pIpv6Config->listIpv6UnicastAddresses, entry);
            break;

        case (uint8_t)IPV6_HEAP_ADDR_MULTICAST_ID:
            TCPIP_Helper_DoubleListTailAdd (&pIpv6Config->listIpv6MulticastAddresses, entry);
            break;

        case (uint8_t)IPV6_HEAP_ADDR_UNICAST_TENTATIVE_ID:
            TCPIP_Helper_DoubleListTailAdd (&pIpv6Config->listIpv6TentativeAddresses, entry);
            break;

        default:
            // do nothing
            break;
    }
}

/*****************************************************************************
  Function:
    IPV6_ADDR_STRUCT * TCPIP_NDP_UnicastAddressMove (TCPIP_NET_IF * pNetIf,
        IPV6_ADDR_STRUCT * entryLocation,
        IPV6_ADDR_STRUCT * previousEntryLocation)

  Summary:
    Inserts an IPv6 address (entryLocaion) into a linked list after the
    address pointed to by previousEntryLocation

  Description:
    Inserts an IPv6 address (entryLocaion) into a linked list after the
    address pointed to by previousEntryLocation.  This function is used to
    sort addresses for default address selection.

  Precondition:
    None

  Parameters:
    pNetIf - Interface the given addresses are on.
    entryLocation - The address that will be inserted.
    previousEntryLocation - The address that will precede the newly inserted
        address

  Returns:
    IPV6_ADDR_STRUCT * - Pointer to the local address after the address that
        was inserted.

  Remarks:
    None
  ***************************************************************************/
IPV6_ADDR_STRUCT * TCPIP_NDP_UnicastAddressMove (TCPIP_NET_IF * pNetIf, IPV6_ADDR_STRUCT * entryLocation, IPV6_ADDR_STRUCT * previousEntryLocation)
{
    IPV6_ADDR_STRUCT * returnVal;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;

    if ((pNetIf == NULL) || (entryLocation == NULL))
    {
        return NULL;
    }

    pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pNetIf);

    returnVal = entryLocation->next;

    DBL_LIST_NODE* entryNode = FC_AddStruct2DblNode(entryLocation); 
    DBL_LIST_NODE* prevEntryNode = FC_AddStruct2DblNode(previousEntryLocation); 
    if (previousEntryLocation != entryLocation->prev)
    {
        if (previousEntryLocation == NULL)
        {
            TCPIP_Helper_DoubleListNodeRemove (&pIpv6Config->listIpv6UnicastAddresses, entryNode);
            TCPIP_Helper_DoubleListHeadAdd (&pIpv6Config->listIpv6UnicastAddresses, entryNode);
        }
        else if (pIpv6Config->listIpv6UnicastAddresses.tail == prevEntryNode)
        {
            TCPIP_Helper_DoubleListNodeRemove (&pIpv6Config->listIpv6UnicastAddresses, entryNode);
            TCPIP_Helper_DoubleListTailAdd (&pIpv6Config->listIpv6UnicastAddresses, entryNode);
        }
        else
        {
            TCPIP_Helper_DoubleListNodeRemove (&pIpv6Config->listIpv6UnicastAddresses, entryNode);

            TCPIP_Helper_DoubleListMidAdd (&pIpv6Config->listIpv6UnicastAddresses, entryNode, prevEntryNode);
        }
    }

    return returnVal;
}

/*****************************************************************************
  Function:
    IPV6_ADDR_STRUCT * TCPIP_NDP_TentativeAddressPromote (
        const TCPIP_NET_IF * pNetIf, IPV6_ADDR_STRUCT * entryLocation)

  Summary:
    Promotes a tentative address to a usable local unicast address.

  Description:
    Promotes a tentative address to a usable local unicast address.

  Precondition:
    None

  Parameters:
    pNetIf - The interface the address is on.
    entryLocation - The address that should be promoted.

  Returns:
    IPV6_ADDR_STRUCT * - The tentative address after the address that is
        promoted.

  Remarks:
    None
  ***************************************************************************/
static IPV6_ADDR_STRUCT * TCPIP_NDP_TentativeAddressPromote (const TCPIP_NET_IF * pNetIf, IPV6_ADDR_STRUCT * entryLocation)
{
    IPV6_ADDR_STRUCT * addressPointer;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;

    if (pNetIf == NULL)
    {
        return NULL;
    }

    pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pNetIf);

    if (entryLocation == NULL)
    {
        return NULL;
    }

    addressPointer = entryLocation->next;

    TCPIP_Helper_DoubleListNodeRemove (&pIpv6Config->listIpv6TentativeAddresses, FC_AddStruct2DblNode(entryLocation));
    TCPIP_Helper_DoubleListTailAdd (&pIpv6Config->listIpv6UnicastAddresses, FC_AddStruct2DblNode(entryLocation));

    TCPIP_IPV6_ClientsNotify(pNetIf, IPV6_EVENT_ADDRESS_ADDED, entryLocation);

    return addressPointer;
}

void TCPIP_IPV6_DefaultRouterDelete(TCPIP_NET_HANDLE netH)
{
    TCPIP_NET_IF * pNetIf =  TCPIPStackHandleToNet(netH);

    if(pNetIf != NULL)
    {
        IPV6_INTERFACE_CONFIG* pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pNetIf);
        if (pIpv6Config != NULL)
        {
            IPV6_HEAP_NDP_NC_ENTRY* neighbor_p = FC_SglNode2NdpEntry(pIpv6Config->listNeighborCache.head).pNcEntry;

            while (neighbor_p != NULL)
            {
                if (neighbor_p->flags.bIsRouter != 0U)
                {
                    (void)TCPIP_NDP_NborEntryDelete (pNetIf, neighbor_p);
                }
                neighbor_p = neighbor_p->next;
            }
        }
    }
}

#endif  // defined (TCPIP_STACK_USE_IPV6)

