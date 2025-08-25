/*******************************************************************************
  Address Resolution Protocol (ARP) Client and Server

  Summary:
    ARP implementation file
    
  Description:
    This source file contains the functions and storage of the 
    ARP routines
    
    Provides IP address to Ethernet MAC address translation
    Reference: RFC 826
*******************************************************************************/

/*
Copyright (C) 2011-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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









#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_ARP

#include "tcpip/src/tcpip_private.h"
#include "tcpip/src/arp_private.h"

#if defined(TCPIP_STACK_USE_IPV4)

/****************************************************************************
  Section:
    Constants and Variables
  ***************************************************************************/

// global ARP module descriptor
typedef struct
{
    size_t              nIfs;               // number of interfaces ARP running on
    ARP_CACHE_DCPT*     arpCacheDcpt;       // ARP caches per interface
    const void*         memH;               // memory allocation handle
    int                 initCount;          // ARP module initialization count
    bool                deleteOld;          // if 0 and old cache still in place don't re-initialize it

    uint32_t            timeSeconds;        // coarse ARP time keeping, seconds
    uint32_t            timeMs;             // coarse ARP time keeping, milliseconds
    TCPIP_SIGNAL_HANDLE   timerHandle;

    PROTECTED_SINGLE_LIST registeredUsers;  // notification users
    // timing
    uint32_t            entrySolvedTmo;     // solved entry removed after this tmo
                                            // if not referenced - seconds
    uint32_t            entryPendingTmo;    // timeout for a pending to be solved entry in the cache, in seconds
    uint32_t            entryRetryTmo;      // timeout for resending an ARP request for a pending entry - seconds
                                            // 1 sec < tmo < entryPendingTmo
    size_t              permQuota;          // max percentage of permanent entries allowed in the cache - %
    uint16_t            entryRetries;       // number of retries for a regular ARP cache entry
    uint16_t            entryGratRetries;   // number of retries for a gratuitous ARP request; default is 1

    TCPIP_MAC_PACKET*   pMacPkt;            // packet that we use to send requests ARP requests
                                            // only ONE packet is used for now even if there are multiple interfaces!!!
}ARP_MODULE_DCPT;


// the module descriptor
static ARP_MODULE_DCPT arpMod = { 0 };


static TCPIP_MAC_ADDR             arpBcastAdd = { {0xff, 0xff, 0xff, 0xff, 0xff, 0xff} };

#ifdef TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL
#define MAX_REG_APPS            2U      // MAX num allowed registrations of Modules/Apps
static struct arp_app_callbacks reg_apps[MAX_REG_APPS]; // Call-Backs storage for MAX of two Modules/Apps

#endif

/****************************************************************************
  Section:
    Helper Function Prototypes
  ***************************************************************************/

static bool         F_ARPSendIfPkt(const TCPIP_NET_IF* pIf, TCPIP_ARP_OPERATION_TYPE oper, uint32_t srcIP, uint32_t dstIP, const TCPIP_MAC_ADDR* dstMAC, const TCPIP_MAC_ADDR* srcMAC);

#ifdef TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL
static void         F_ARPProcessRxPkt(const TCPIP_NET_IF* pIf, ARP_PACKET* packet);
#endif

static void         F_ARPSwapPacket(ARP_PACKET* p);

static void         F_ARPUpdateEntry(const TCPIP_NET_IF* pIf, ARP_HASH_ENTRY* arpHE, const TCPIP_MAC_ADDR* hwAdd);
static TCPIP_ARP_RESULT   F_ARPAddCompleteEntry(const TCPIP_NET_IF* pIf, IPV4_ADDR* pIPAddr, const TCPIP_MAC_ADDR* hwAdd);
    
#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void         F_ARPDeleteResources(void);
static void         F_ARPDeleteCache(ARP_CACHE_DCPT* pArpDcpt);
static void         F_ARPDeleteClients(void);
#else
#define F_ARPDeleteResources()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static void         F_ARPNotifyClients(const TCPIP_NET_IF* pNetIf, const IPV4_ADDR* ipAdd, const TCPIP_MAC_ADDR* MACAddr, TCPIP_ARP_EVENT_TYPE evType);

static TCPIP_ARP_RESULT   F_ARPProbeAddress(TCPIP_NET_IF* pIf, const IPV4_ADDR* IPAddr, const IPV4_ADDR* srcAddr, TCPIP_ARP_OPERATION_TYPE opType, TCPIP_MAC_ADDR* pHwAdd);

static TCPIP_MAC_PACKET* F_ARPAllocateTxPacket(void);

static void         F_ARPTxAckFnc (TCPIP_MAC_PACKET * pPkt, const void * param);

static void         TCPIP_ARP_Timeout(void);
static void         TCPIP_ARP_Process(void);


#if defined( OA_HASH_DYNAMIC_KEY_MANIPULATION )
size_t TCPIP_ARP_HashKeyHash(OA_HASH_DCPT* pOH, const void* key);
#if defined(OA_DOUBLE_HASH_PROBING)
size_t TCPIP_ARP_HashProbeHash(OA_HASH_DCPT* pOH, const void* key);
#endif  // defined(OA_DOUBLE_HASH_PROBING)
OA_HASH_ENTRY* TCPIP_ARP_HashEntryDelete(OA_HASH_DCPT* pOH);
int TCPIP_ARP_HashKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key);
void TCPIP_ARP_HashKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key);
#endif  // defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )

// helpers
static __inline__  ARP_HASH_ENTRY*  __attribute__((always_inline)) FC_Oa2HashEntry(OA_HASH_ENTRY* hE)
{
    union
    {
        OA_HASH_ENTRY*  hE;
        ARP_HASH_ENTRY* arpHE;
    }U_OA_HASH;

    U_OA_HASH.hE = hE;
    return U_OA_HASH.arpHE;
}

static __inline__  ARP_HASH_ENTRY*  __attribute__((always_inline)) FC_Node2HashEntry(SGL_LIST_NODE* pN)
{
    union
    {
        uint8_t* uPtr;
        ARP_HASH_ENTRY* arpHE;
    }U_PTR_HASH;

    U_PTR_HASH.uPtr = ((uint8_t*)pN - offsetof(struct S_TAG_ARP_HASH_ENTRY, next));
    return U_PTR_HASH.arpHE;
}

static __inline__  ARP_LIST_NODE*  __attribute__((always_inline)) FC_Node2ArpNode(SGL_LIST_NODE* pN)
{
    union
    {
        SGL_LIST_NODE* node;
        ARP_LIST_NODE* arpNode;
    }U_ARP_NODE;

    U_ARP_NODE.node = pN;
    return U_ARP_NODE.arpNode;
}

static __inline__  SGL_LIST_NODE*  __attribute__((always_inline)) FC_HashEntry2Node(ARP_HASH_ENTRY* hE)
{
    union
    {
        ARP_HASH_ENTRY* arpHE;
        SGL_LIST_NODE* node;
    }U_NODE_ARP;

    U_NODE_ARP.arpHE = hE;
    return U_NODE_ARP.node;
}

static __inline__  SGL_LIST_NODE*  __attribute__((always_inline)) FC_ArpH2Node(TCPIP_ARP_HANDLE hArp)
{
    union
    {
        TCPIP_ARP_HANDLE hArp;
        SGL_LIST_NODE* node;
    }U_ARP_HANDLE_NODE;

    U_ARP_HANDLE_NODE.hArp = hArp;
    return U_ARP_HANDLE_NODE.node;
}

static __inline__  ARP_PACKET*  __attribute__((always_inline)) FC_Uptr2ArpPkt(uint8_t* uPtr)
{
    union
    {
        uint8_t*    uPtr;
        ARP_PACKET* pArp;
    }U_UPTR_ARP_PKT;

    U_UPTR_ARP_PKT.uPtr = uPtr;
    return U_UPTR_ARP_PKT.pArp;
}

static __inline__  ARP_PACKET*  __attribute__((always_inline)) FC_CUptr2ArpPkt(const uint8_t* cuPtr)
{
    union
    {
        const uint8_t*  cuPtr;
        ARP_PACKET*     pArpPkt;
    }U_CUPTR_ARP_PKT;

    U_CUPTR_ARP_PKT.cuPtr = cuPtr;
    return U_CUPTR_ARP_PKT.pArpPkt;
}

static __inline__  TCPIP_ARP_OPERATION_TYPE  __attribute__((always_inline)) FC_Uval2ArpOp(uint32_t uval32)
{
    union
    {
        uint32_t    uval32;
        TCPIP_ARP_OPERATION_TYPE opType;
    }U_UINT_ARP_OPER;

    U_UINT_ARP_OPER.uval32 = uval32;
    return U_UINT_ARP_OPER.opType; 
}

static __inline__  uint32_t  __attribute__((always_inline)) FC_ArpOp2Uval(TCPIP_ARP_OPERATION_TYPE opType)
{
    union
    {
        TCPIP_ARP_OPERATION_TYPE opType;
        uint32_t    uval32;
    }U_ARP_OPER_UINT;

    U_ARP_OPER_UINT.opType = opType;
    return U_ARP_OPER_UINT.uval32; 
}

static __inline__  ARP_CACHE_DCPT* __attribute__((always_inline)) FC_Cvptr2ArpDcpt(const void* cvptr)
{
    union
    {
        const void*     cvptr;
        ARP_CACHE_DCPT* pArpDcpt;
    }U_CV_PTR_ARP_DCPT;

    U_CV_PTR_ARP_DCPT.cvptr = cvptr;
    return U_CV_PTR_ARP_DCPT.pArpDcpt; 
}

static __inline__  ARP_ENTRY_FLAGS __attribute__((always_inline)) FC_Uval2EntryFlags(uint32_t uval32)
{
    union
    {
        uint32_t    uval32;
        ARP_ENTRY_FLAGS entryFlags;
    }U_UINT_ARP_FLAGS;

    U_UINT_ARP_FLAGS.uval32 = uval32;
    return U_UINT_ARP_FLAGS.entryFlags;
}

// sets an ARP hash entry
static  void  F_ARPSetEntry(ARP_HASH_ENTRY* arpHE, ARP_ENTRY_FLAGS newFlags,
                            const TCPIP_MAC_ADDR* hwAdd, PROTECTED_SINGLE_LIST* addList)
{
    arpHE->hEntry.flags.value &= ~(uint16_t)ARP_FLAG_ENTRY_VALID_MASK;
    arpHE->hEntry.flags.value |= (uint16_t)newFlags;
    
    if(hwAdd != NULL)
    {
        arpHE->hwAdd = *hwAdd;
    }
    
    arpHE->tInsert = arpMod.timeSeconds;
    arpHE->nRetries = 1;
    if(addList != NULL)
    {
        TCPIP_Helper_ProtSglListTailAdd(addList, &arpHE->lnode);
    }
}


// re-inserts at the tail, makes the entry fresh
static  void F_ARPRefreshEntry(ARP_HASH_ENTRY* arpHE, PROTECTED_SINGLE_LIST* pL)
{
    (void)TCPIP_Helper_ProtSglListNodeRemove(pL, &arpHE->lnode);
    arpHE->tInsert = arpMod.timeSeconds;
    TCPIP_Helper_ProtSglListTailAdd(pL, &arpHE->lnode);
}

static  void F_ARPRemoveCacheEntries(ARP_CACHE_DCPT* pArpDcpt)
{

    if(pArpDcpt->hashDcpt != NULL)
    {
        TCPIP_OAHASH_EntriesRemoveAll(pArpDcpt->hashDcpt);
        TCPIP_Helper_ProtSglListRemoveAll(&pArpDcpt->incompleteList);
        TCPIP_Helper_ProtSglListRemoveAll(&pArpDcpt->completeList);
        TCPIP_Helper_ProtSglListRemoveAll(&pArpDcpt->permList);
    }
}

static  void F_ARPRemoveEntry(ARP_CACHE_DCPT* pArpDcpt, OA_HASH_ENTRY* hE)
{
    PROTECTED_SINGLE_LIST     *remList;

    if((hE->flags.value & (uint16_t)ARP_FLAG_ENTRY_PERM) != 0U)
    {
        remList =  &pArpDcpt->permList;
    }
    else if((hE->flags.value & (uint16_t)ARP_FLAG_ENTRY_COMPLETE) != 0U)
    {
        remList =  &pArpDcpt->completeList;
    }
    else
    {
        remList =  &pArpDcpt->incompleteList;
    }

    ARP_HASH_ENTRY* arpHE = FC_Oa2HashEntry(hE);
    (void)TCPIP_Helper_ProtSglListNodeRemove(remList, &arpHE->lnode);

    TCPIP_OAHASH_EntryRemove(pArpDcpt->hashDcpt, hE);

}

#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT) && (TCPIP_ARP_PRIMARY_CACHE_ONLY != 0)
static __inline__  ARP_CACHE_DCPT*  __attribute__((always_inline)) F_ARPGetIfDcpt(const TCPIP_NET_IF* pIf)
{
    return arpMod.arpCacheDcpt + TCPIPStackNetGetPrimary(pIf)->priIfIx;
}
#else
static __inline__  ARP_CACHE_DCPT*  __attribute__((always_inline)) F_ARPGetIfDcpt(const TCPIP_NET_IF* pIf)
{
    return arpMod.arpCacheDcpt + TCPIP_STACK_NetIxGet(pIf);
}
#endif  // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT) && (TCPIP_ARP_PRIMARY_CACHE_ONLY != 0)

/****************************************************************************
  Section:
    Function Implementations
  ***************************************************************************/
#ifdef TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL
/************ User Application APIs ****************************************/

/*****************************************************************************
  Function:
    ssize_t TCPIP_ARP_CallbacksRegister(struct arp_app_callbacks *app)

  Summary:
    Registering callback with ARP module to get notified about certian events.
    
  Description:
    This function allows end user application to register with callbacks, which
    will be called by ARP module to give notification to user-application about 
    events occurred at ARP layer. For ex: when a ARP-packet is received, which is
    conflicting with our own pair of addresses (MAC-Address and IP-address).
    This is an extension for zeroconf protocol implementation (ZeroconfLL.c)

  Precondition:
    None

  Parameters:
    app - ARP-Application callbacks structure supplied by user-application 
    
  Returns:
    id > 0 - Returns non-negative value that represents the id of registration
             The same id needs to be used in de-registration
    -1     - When registered applications exceed MAX_REG_APPS and there is no
             free slot for registration
 
  ***************************************************************************/
ssize_t TCPIP_ARP_CallbacksRegister(struct arp_app_callbacks *app)
{
    size_t ix;
    for(ix = 0; ix < (size_t)MAX_REG_APPS; ix++)
    {
        if(!reg_apps[ix].used)
        {
            reg_apps[ix].TCPIP_ARP_PacketNotify = app->TCPIP_ARP_PacketNotify;
            reg_apps[ix].used = 1;
            return ((ssize_t)ix + 1); // Return Code. Should be used in deregister.
        }
    }

    return -1; // No space for registration
}

/*****************************************************************************
  Function:
    bool TCPIP_ARP_CallbacksDeregister(size_t regId)

  Summary:
    De-Registering callbacks with ARP module that are registered previously.
    
  Description:
    This function allows end user-application to de-register with callbacks, 
    which were registered previously.
    This is called by user-application, when its no longer interested in 
    notifications from ARP-Module. This allows the other application to get 
    registered with ARP-module.   

  Precondition:
    None

  Parameters:
    regId - Registration-id returned in TCPIP_ARP_CallbacksRegister call
    
  Returns:
    true  - On success
    false - Failure to indicate invalid regId  
  ***************************************************************************/ 
bool TCPIP_ARP_CallbacksDeregister(size_t regId)
{
    if(regId > (size_t)MAX_REG_APPS)
    {
        return false;
    }

    reg_apps[regId - 1U].used = 0;     // To indicate free slot for registration
    return true;
}


/*****************************************************************************
  Function:
    void F_ARPProcessRxPkt(const TCPIP_NET_IF* pIf, ARP_PACKET* packet)

  Summary:
    Processes Received-ARP packet (ARP request/Reply).
    
  Description:
    This function is to pass-on the ARP-packet to registered application,
    with the notification of Rx-ARP packet. 

  Precondition:
    ARP packet is received completely from MAC

  Parameters:
    pIf   - interface to use 
    packet - Rx packet to be processed     

  Returns:
    None   
  ***************************************************************************/
static void F_ARPProcessRxPkt(const TCPIP_NET_IF* pIf, ARP_PACKET* packet)
{
    bool pass_on = false; // Flag to indicate whether need to be forwarded
    size_t ix;

    // Probing Stage
    if(pIf->netIPAddr.Val == 0U)
    {
        pass_on = true; // Pass to Registered-Application for further processing        
    }
    else if(pIf->netIPAddr.Val != 0U)
    {
        /* Late-conflict */
        if(packet->SenderIPAddr.Val == pIf->netIPAddr.Val)
        {
            pass_on = true;
        }
    }
    else
    {
        // do nothing
    }

    if(pass_on != false)
    {
    
        for(ix = 0U; ix < (size_t)MAX_REG_APPS; ix++)
        {
            if(reg_apps[ix].used)
            {
                reg_apps[ix].TCPIP_ARP_PacketNotify(pIf,
                packet->SenderIPAddr.Val,
                                packet->TargetIPAddr.Val,
                                &packet->SenderMACAddr,
                                &packet->TargetMACAddr,
                                packet->Operation);                
            }
        }
    }
}

#else
ssize_t TCPIP_ARP_CallbacksRegister(struct arp_app_callbacks *app)
{
    return -1; // No space for registration
}

bool TCPIP_ARP_CallbacksDeregister(size_t regId)
{
    return false;
}
#endif  // TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL


/*****************************************************************************
  Function:
    static bool F_ARPSendIfPkt(TCPIP_NET_IF* pIf, TCPIP_ARP_OPERATION_TYPE oper, uint32_t srcIP, uint32_t dstIP, const TCPIP_MAC_ADDR* dstMAC, const TCPIP_MAC_ADDR* srcMAC)

  Description:
    Writes an ARP packet to the MAC using the interface pointer for src IP and MAC address.

  Precondition:
    None

  Parameters:
  if srcMAC == 0, then it uses the the MAC address of the used interface pIf

  Return Values:
    true - The ARP packet was generated properly
    false - otherwise

  ***************************************************************************/
static bool F_ARPSendIfPkt(const TCPIP_NET_IF* pIf, TCPIP_ARP_OPERATION_TYPE oper, uint32_t srcIP, uint32_t dstIP, const TCPIP_MAC_ADDR* dstMAC, const TCPIP_MAC_ADDR* srcMAC)
{
    TCPIP_MAC_PACKET* pMacPkt;
    ARP_PACKET*       pArp;

    if(arpMod.pMacPkt != NULL && (arpMod.pMacPkt->pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_QUEUED) == 0U)
    {
        pMacPkt = arpMod.pMacPkt;
    }
    else
    {   // packet not available, have to allocate another one
        if((pMacPkt = F_ARPAllocateTxPacket()) == NULL)
        {
            return false;
        }
        arpMod.pMacPkt = pMacPkt;   // show we're using this one now
    }

    pArp = FC_Uptr2ArpPkt(pMacPkt->pNetLayer);
    pArp->HardwareType  = HW_ETHERNET;
    pArp->Protocol      = ARP_IP;
    pArp->MACAddrLen    = (uint8_t)sizeof(TCPIP_MAC_ADDR);
    pArp->ProtocolLen   = (uint8_t)sizeof(IPV4_ADDR);
    pArp->Operation = (uint16_t)oper;

    if(srcMAC == NULL)
    {
        pArp->SenderMACAddr = pIf->netMACAddr;
    }
    else
    {
        pArp->SenderMACAddr = *srcMAC;
    }
    pArp->SenderIPAddr.Val  = srcIP;
    pArp->TargetMACAddr = *dstMAC;
    pArp->TargetIPAddr.Val  = dstIP;

    F_ARPSwapPacket(pArp);

    // format the MAC packet
    pMacPkt->pDSeg->segLen = (uint8_t)sizeof(ARP_PACKET);
    const TCPIP_MAC_ADDR* pMacAddr = FC_CUptr2CMacAdd(TCPIP_STACK_NetUpMACAddressGet(pIf));
    if(TCPIP_PKT_PacketMACFormat(pMacPkt, dstMAC, pMacAddr, TCPIP_ETHER_TYPE_ARP))
    {
        pMacPkt->next = NULL;  // send single packet
        pMacPkt->pktIf = pIf;
        TCPIP_PKT_FlightLogTx(pMacPkt, TCPIP_THIS_MODULE_ID);
        if(TCPIPStackPacketTx(pIf, pMacPkt) >= (TCPIP_MAC_RES)0)
        {   // MAC sets itself the TCPIP_MAC_PKT_FLAG_QUEUED
            return true;
        }
        TCPIP_PKT_FlightLogAcknowledge(pMacPkt, TCPIP_THIS_MODULE_ID, TCPIP_MAC_PKT_ACK_MAC_REJECT_ERR);
    }

    // something failed
    return false;
}

/*****************************************************************************
  Function:
    static void F_ARPUpdateEntry(const TCPIP_NET_IF* pIf, ARP_HASH_ENTRY* arpHE, const TCPIP_MAC_ADDR* hwAdd)

  Description:
    Updates the info for an existing ARP cache entry

  Precondition:
    None

  Parameters:
    pIf             - interface
    arpHE           - particular cache entry to be updated
    hwAdd           - the (new) hardware address

  Return Values:
    None
  ***************************************************************************/
static void F_ARPUpdateEntry(const TCPIP_NET_IF* pIf, ARP_HASH_ENTRY* arpHE, const TCPIP_MAC_ADDR* hwAdd)
{
    TCPIP_ARP_EVENT_TYPE evType; 
    ARP_CACHE_DCPT  *pArpDcpt;
    
    pArpDcpt = F_ARPGetIfDcpt(pIf);
    if((arpHE->hEntry.flags.value & (uint16_t)ARP_FLAG_ENTRY_PERM) == 0U)
    {   

        if((arpHE->hEntry.flags.value & (uint16_t)ARP_FLAG_ENTRY_COMPLETE) == 0U)
        {   // was waiting for this one, it was queued
            evType = ARP_EVENT_SOLVED;
            (void)TCPIP_Helper_ProtSglListNodeRemove(&pArpDcpt->incompleteList, &arpHE->lnode);
        }
        else
        {   // completed entry, but now updated
            evType = ARP_EVENT_UPDATED;
            (void)TCPIP_Helper_ProtSglListNodeRemove(&pArpDcpt->completeList, &arpHE->lnode);
        }
        
        // move to tail, updated
        F_ARPSetEntry(arpHE, ARP_FLAG_ENTRY_COMPLETE, hwAdd, &pArpDcpt->completeList);
    }
    else
    {   // permanent entries are not updated
        evType = ARP_EVENT_PERM_UPDATE;
    }

    F_ARPNotifyClients(pIf, &arpHE->ipAddress, &arpHE->hwAdd, evType);

}


/*****************************************************************************
  Function:
    static TCPIP_ARP_RESULT F_ARPAddCompleteEntry(const TCPIP_NET_IF* pIf, IPV4_ADDR* pIPAddr, const TCPIP_MAC_ADDR* hwAdd)

  Description:
    Updates the info for an existing ARP cache entry

  Precondition:
    None

  Parameters:
    pIf             - network interface 
    arpHE           - particular cache entry to be updated
    hwAdd           - the (new) hardware address

  Return Values:
    ARP_RES_CACHE_FULL  - cache full error
    ARP_RES_OK          - success
  ***************************************************************************/
static TCPIP_ARP_RESULT F_ARPAddCompleteEntry(const TCPIP_NET_IF* pIf, IPV4_ADDR* pIPAddr, const TCPIP_MAC_ADDR* hwAdd)
{
    ARP_CACHE_DCPT  *pArpDcpt;
    ARP_HASH_ENTRY  *arpHE;
    OA_HASH_ENTRY   *hE;

    pArpDcpt = F_ARPGetIfDcpt(pIf);
    
    hE = TCPIP_OAHASH_EntryLookupOrInsert(pArpDcpt->hashDcpt, pIPAddr);
    if(hE == NULL)
    {   // oops, hash full?
        return ARP_RES_CACHE_FULL;
    }

    // now in cache
    arpHE = FC_Oa2HashEntry(hE);
    if(arpHE->hEntry.flags.newEntry != 0U)
    {   // populate the new entry
        F_ARPSetEntry(arpHE, ARP_FLAG_ENTRY_COMPLETE, hwAdd, &pArpDcpt->completeList);
    }
    else
    {   // existent entry
        F_ARPUpdateEntry(pIf, arpHE, hwAdd);
    }

    return ARP_RES_OK;
}



/*****************************************************************************
  Function:
    void TCPIP_ARP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_ARP_MODULE_CONFIG* arpData)

  Summary:
    Initializes the ARP module.
    
  Description:
    Initializes the ARP module.
    Calls can be done with the request of not tearing down the ARP cache
    This helps for ifup/ifdown sequences.
    Of course, if this is the case the memory allocated for the ARP cache
    has to be from a persistent heap.
    
  Precondition:
    None

  Parameters:
    stackCtrl  - stack initialization parameters
    arpData    - ARP specific initialization parameters

  Returns:
    true if initialization succeded,
    false otherwise
  
  Remarks:
    The request to maintain old ARP cache info (deleteOld field from the TCPIP_ARP_MODULE_CONFIG initialization data)
    is not implemented for stack init/deinit sequences.
    To maintain the data after the stack is completely de-initialized would need a persistent heap
    that's not yet implemented.
    The selection cannot be changed by ifup since this operation does not carry ARP configuration 
    parameters (arpDate == 0).
  ***************************************************************************/
bool TCPIP_ARP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* initData)
{
    OA_HASH_DCPT*   hashDcpt;
    ARP_CACHE_DCPT* pArpDcpt;
    size_t          hashMemSize;
    size_t          ix, nArpIfs;
    bool            iniRes;

    if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {   // interface going up
        // store the delete option for de-initialization
        if(arpMod.deleteOld)
        {   // remove the old entries, if there
#if (TCPIP_ARP_PRIMARY_CACHE_ONLY != 0)
            if(!TCPIPStackNetIsPrimary(stackCtrl->pNetIf))
            {
                return true;
            }
#endif // (TCPIP_ARP_PRIMARY_CACHE_ONLY != 0)
            
            pArpDcpt = F_ARPGetIfDcpt(stackCtrl->pNetIf);
            F_ARPRemoveCacheEntries(pArpDcpt);
        }
        // else do not re-initialize
        return true;
    }

    // stack going up


    if(arpMod.initCount == 0)
    {   // first time we're run
        // check we have a consistent task rate
#if !defined(TCPIP_ARP_TASK_PROCESS_RATE) || (TCPIP_ARP_TASK_PROCESS_RATE == 0)
        return false;
#endif // !defined(TCPIP_ARP_TASK_PROCESS_RATE) || (TCPIP_ARP_TASK_PROCESS_RATE == 0)
        // check initialization data is provided
        const TCPIP_ARP_MODULE_CONFIG* arpData = (const TCPIP_ARP_MODULE_CONFIG*)initData;
        if(arpData == NULL)
        {
            return false;
        }

        // store the delete option for de-initialization
        arpMod.deleteOld = arpData->deleteOld;

        // check if there's any persistent data
#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT) && (TCPIP_ARP_PRIMARY_CACHE_ONLY != 0)
        nArpIfs = (size_t)stackCtrl->nIfs - (size_t)stackCtrl->nAliases;
#else
        nArpIfs = (size_t)stackCtrl->nIfs;
#endif // (TCPIP_ARP_PRIMARY_CACHE_ONLY != 0)
        if(arpMod.arpCacheDcpt != NULL && (arpData->deleteOld || arpMod.nIfs != nArpIfs))
        {   // delete the old copy
            F_ARPDeleteResources();
        }

        // store the memory allocation handle
        arpMod.memH = stackCtrl->memH;
        arpMod.nIfs =  nArpIfs;

        // parameters initialization
        arpMod.entrySolvedTmo = arpData->entrySolvedTmo;
        arpMod.entryPendingTmo = arpData->entryPendingTmo;
        arpMod.entryRetryTmo = arpData->entryRetryTmo;
        arpMod.permQuota = arpData->permQuota;
        arpMod.entryRetries = (uint16_t)arpData->retries;
        arpMod.entryGratRetries =  (uint16_t)arpData->gratProbeCount;


        if(arpMod.arpCacheDcpt == NULL)
        {
            arpMod.arpCacheDcpt = (ARP_CACHE_DCPT*)TCPIP_HEAP_Calloc(arpMod.memH, arpMod.nIfs, sizeof(*arpMod.arpCacheDcpt)); 
            if(arpMod.arpCacheDcpt == NULL)
            {   // failed
                return false;
            }

            hashMemSize = sizeof(OA_HASH_DCPT) + arpData->cacheEntries * sizeof(ARP_HASH_ENTRY);
            pArpDcpt = arpMod.arpCacheDcpt;
            for(ix = 0; ix < arpMod.nIfs; ix++)
            {
                hashDcpt = (OA_HASH_DCPT*)TCPIP_HEAP_Malloc(arpMod.memH, hashMemSize);

                if(hashDcpt == NULL)
                {   // failed
                    F_ARPDeleteResources();
                    return false;
                }

                // populate the entries
                hashDcpt->memBlk = hashDcpt + 1;
                hashDcpt->hParam = pArpDcpt;    // store the descriptor it belongs to
                hashDcpt->hEntrySize = sizeof(ARP_HASH_ENTRY);
                hashDcpt->hEntries = arpData->cacheEntries;
                hashDcpt->probeStep = ARP_HASH_PROBE_STEP;

#if defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )
                hashDcpt->hashF = &TCPIP_ARP_HashKeyHash;
#if defined(OA_DOUBLE_HASH_PROBING)
                hashDcpt->probeHash = &TCPIP_ARP_HashProbeHash;
#endif  // defined(OA_DOUBLE_HASH_PROBING)
                hashDcpt->delF = &TCPIP_ARP_HashEntryDelete;
                hashDcpt->cmpF = &TCPIP_ARP_HashKeyCompare;
                hashDcpt->cpyF = &TCPIP_ARP_HashKeyCopy; 
#endif  // defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )
                TCPIP_OAHASH_Initialize(hashDcpt);

                pArpDcpt->hashDcpt = hashDcpt;
                while(true)
                {
                    if((iniRes = TCPIP_Helper_ProtSglListInitialize(&pArpDcpt->permList)) == false)
                    {
                        break;
                    }

                    if((iniRes = TCPIP_Helper_ProtSglListInitialize(&pArpDcpt->completeList)) == false)
                    {
                        break;
                    }

                    iniRes = TCPIP_Helper_ProtSglListInitialize(&pArpDcpt->incompleteList);
                    break;
                }

                if(iniRes == false)
                {
                    F_ARPDeleteResources();
                    return false;
                }

                pArpDcpt->purgeThres = ((size_t)arpData->purgeThres * pArpDcpt->hashDcpt->hEntries + 99U) / 100U;
                pArpDcpt->purgeQuanta = arpData->purgeQuanta;

                pArpDcpt++;
            }

            arpMod.pMacPkt = F_ARPAllocateTxPacket();
            arpMod.timerHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_ARP_Task, TCPIP_ARP_TASK_PROCESS_RATE);
            iniRes = TCPIP_Notification_Initialize(&arpMod.registeredUsers);

            if(arpMod.pMacPkt == NULL || arpMod.timerHandle == NULL || iniRes == false)
            {
                F_ARPDeleteResources();
                return false;
            }

            arpMod.timeSeconds = arpMod.timeMs = 0;
        }
    }

    // per interface initialization
    pArpDcpt = F_ARPGetIfDcpt(stackCtrl->pNetIf);

    if(arpMod.deleteOld)
    {   // remove the old entries, if there
        F_ARPRemoveCacheEntries(pArpDcpt);
    }
    // else do not re-initialize
    
    arpMod.initCount++;

    return true;
}




#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_ARP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{

    if(arpMod.initCount > 0)
    {   // we're up and running
        if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_DOWN)
        {   // interface going down
            if(arpMod.deleteOld)
            {
                ARP_CACHE_DCPT* pArpDcpt;    
#if (TCPIP_ARP_PRIMARY_CACHE_ONLY != 0)
                pArpDcpt = TCPIPStackNetIsPrimary(stackCtrl->pNetIf) ? F_ARPGetIfDcpt(stackCtrl->pNetIf) : NULL;
#else
                pArpDcpt = F_ARPGetIfDcpt(stackCtrl->pNetIf);    
#endif // (TCPIP_ARP_PRIMARY_CACHE_ONLY != 0)
                if(pArpDcpt != NULL)
                {
                    F_ARPRemoveCacheEntries(pArpDcpt);
                }
            }
        }
        else
        {   // if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
            // stack shut down
            if(--arpMod.initCount == 0)
            {   // all closed
                // release resources
                // if(arpMod.deleteOld)
                // Note: ignored, always clean up at stack shut down
                {
                    F_ARPDeleteResources();
                }
            }
        }
    }

}

static void F_ARPDeleteResources(void)
{

    if(arpMod.arpCacheDcpt != NULL)
    {
        size_t ix;
        ARP_CACHE_DCPT* pArpDcpt;

        pArpDcpt = arpMod.arpCacheDcpt;
        for(ix = 0; ix < arpMod.nIfs; ix++)
        {
            F_ARPDeleteCache(pArpDcpt);
            pArpDcpt++;
        }

        (void)TCPIP_HEAP_Free(arpMod.memH, arpMod.arpCacheDcpt);
        arpMod.arpCacheDcpt = NULL;
    }

    F_ARPDeleteClients();
    arpMod.memH = NULL;

    if(arpMod.timerHandle != NULL)
    {
        TCPIPStackSignalHandlerDeregister(arpMod.timerHandle);
        arpMod.timerHandle = NULL;
    }

    if(arpMod.pMacPkt != NULL)
    {
        if((arpMod.pMacPkt->pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_QUEUED) == 0U)
        {
            TCPIP_PKT_PacketFree(arpMod.pMacPkt);
        }
        arpMod.pMacPkt = NULL;
    }

}

static void F_ARPDeleteCache(ARP_CACHE_DCPT* pArpDcpt)
{

    if(pArpDcpt->hashDcpt != NULL)
    {
        TCPIP_OAHASH_EntriesRemoveAll(pArpDcpt->hashDcpt);
        TCPIP_Helper_ProtSglListDeinitialize(&pArpDcpt->incompleteList);
        TCPIP_Helper_ProtSglListDeinitialize(&pArpDcpt->completeList);
        TCPIP_Helper_ProtSglListDeinitialize(&pArpDcpt->permList);
        
        (void)TCPIP_HEAP_Free(arpMod.memH, pArpDcpt->hashDcpt);
        pArpDcpt->hashDcpt = NULL;
    }

}

static void F_ARPDeleteClients(void)
{
    TCPIP_Notification_Deinitialize(&arpMod.registeredUsers, arpMod.memH);
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static TCPIP_MAC_PACKET* F_ARPAllocateTxPacket(void)
{
    TCPIP_MAC_PACKET*   pPkt;

    // allocate TCPIP_MAC_PACKET packet
    pPkt = TCPIP_PKT_PacketAlloc((uint16_t)sizeof(TCPIP_MAC_PACKET), (uint16_t)sizeof(ARP_PACKET), (TCPIP_MAC_PACKET_FLAGS)(TCPIP_MAC_PKT_FLAG_ARP | (uint32_t)TCPIP_MAC_PKT_FLAG_TX));

    if(pPkt != NULL)
    {
        TCPIP_PKT_PacketAcknowledgeSet(pPkt, &F_ARPTxAckFnc, NULL);
    }


    return pPkt;
}


static void F_ARPTxAckFnc (TCPIP_MAC_PACKET * pPkt, const void * param)
{
    if(arpMod.pMacPkt != pPkt)
    {   // another one allocated
        TCPIP_PKT_PacketFree(pPkt);
    }
    else
    {   // still using this packet
        pPkt->pktFlags &= ~(uint32_t)TCPIP_MAC_PKT_FLAG_QUEUED;
    }
}



TCPIP_ARP_HANDLE TCPIP_ARP_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_ARP_EVENT_HANDLER handler, const void* hParam)
{
    if(handler != NULL && arpMod.memH != NULL) 
    {
        ARP_LIST_NODE arpNode;
        arpNode.next = NULL;
        arpNode.handler = handler;
        arpNode.hParam = hParam;
        arpNode.hNet = hNet;

        SGL_LIST_NODE* node = TCPIP_Notification_Add(&arpMod.registeredUsers, arpMod.memH, &arpNode, sizeof(arpNode));
        return FC_Node2ArpNode(node);
    }

    return NULL;

}

// deregister the event handler
bool TCPIP_ARP_HandlerDeRegister(TCPIP_ARP_HANDLE hArp)
{
    if(hArp != NULL && arpMod.memH != NULL)
    {
        SGL_LIST_NODE* node = FC_ArpH2Node(hArp);
        if(TCPIP_Notification_Remove(node, &arpMod.registeredUsers, arpMod.memH))
        {
            return true;
        }
    }

    return false;
}

static void F_ARPNotifyClients(const TCPIP_NET_IF* pNetIf, const IPV4_ADDR* ipAdd, const TCPIP_MAC_ADDR* MACAddr, TCPIP_ARP_EVENT_TYPE evType)
{
    ARP_LIST_NODE* aNode;

    TCPIP_Notification_Lock(&arpMod.registeredUsers);

    aNode = FC_Node2ArpNode(arpMod.registeredUsers.list.head);
    while(aNode != NULL)
    {
        if(aNode->hNet == NULL || aNode->hNet == pNetIf)
        {   // trigger event
            (*aNode->handler)(pNetIf, ipAdd, MACAddr, evType, aNode->hParam);
        }
        aNode = aNode->next;
    }
    TCPIP_Notification_Unlock(&arpMod.registeredUsers);
    
}


// called after service needed reported
// maintain the queues, processes, etc.

void TCPIP_ARP_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

#if (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)
    size_t netIx, nIfs;
    const TCPIP_NET_IF *pIf;
    uint32_t  signalParam;

    sigPend = TCPIPStackModuleSignalParamGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL, &signalParam);

    if(((uint32_t)sigPend & (uint32_t)TCPIP_MODULE_SIGNAL_IF_CHANGE) != 0U)
    { // interface address change occurred
        nIfs = (size_t)TCPIP_STACK_NumberOfNetworksGet();
        for(netIx = 0; netIx < nIfs; netIx++)
        {
           if((signalParam & ((uint32_t)1U << netIx)) != 0U) 
           {
               pIf = (const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(netIx);
               // gratuitous PROBE_ONLY
               TCPIP_ARP_OPERATION_TYPE opType = FC_Uval2ArpOp((uint32_t)ARP_OPERATION_RESP | (uint32_t)ARP_OPERATION_CONFIGURE | (uint32_t)ARP_OPERATION_PROBE_ONLY);
               (void)TCPIP_ARP_Probe(pIf, &pIf->netIPAddr, &pIf->netIPAddr, opType);
           }
        }
    }
#else
    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);
#endif  // (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)
    
    if(((uint32_t)sigPend & (uint32_t)TCPIP_MODULE_SIGNAL_RX_PENDING) != 0U)
    { //  RX signal occurred
        TCPIP_ARP_Process();
    }

    if(((uint32_t)sigPend & (uint32_t)TCPIP_MODULE_SIGNAL_TMO) != 0U)
    { // regular TMO occurred
        TCPIP_ARP_Timeout();
    }

}

static void TCPIP_ARP_Timeout(void)
{
    size_t netIx, purgeIx, nArpIfs;
    ARP_HASH_ENTRY  *pE;
    ARP_CACHE_DCPT  *pArpDcpt;
    SGL_LIST_NODE   *pN;
    const TCPIP_NET_IF *pIf;
    bool        isConfig;
    uint16_t    maxRetries;


    arpMod.timeMs += (uint32_t)TCPIP_ARP_TASK_PROCESS_RATE;
    arpMod.timeSeconds = arpMod.timeMs / 1000U;

    nArpIfs = (size_t)TCPIP_STACK_NumberOfNetworksGet();
    pArpDcpt = arpMod.arpCacheDcpt;
    for(netIx = 0; netIx < nArpIfs; netIx++)
    {
        pIf = (const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(netIx);
#if (TCPIP_ARP_PRIMARY_CACHE_ONLY != 0)
        if(!TCPIPStackNetIsPrimary(pIf))
        {
            continue;
        }
#endif // (TCPIP_ARP_PRIMARY_CACHE_ONLY != 0)

        // process the incomplete queue
        // see if there's something to remove
        while( (pN = pArpDcpt->incompleteList.list.head) != NULL)
        {

            pE = FC_Node2HashEntry(pN); 
            if( (arpMod.timeSeconds - pE->tInsert) >= arpMod.entryPendingTmo)
            {   // expired, remove it
                TCPIP_OAHASH_EntryRemove(pArpDcpt->hashDcpt, &pE->hEntry);
                (void)TCPIP_Helper_ProtSglListHeadRemove(&pArpDcpt->incompleteList);
                F_ARPNotifyClients(pIf, &pE->ipAddress, NULL, ARP_EVENT_REMOVED_TMO);
            }
            else
            {   // this list is ordered, we can safely break out
                break;
            }
        }

        // see if we have to query again
        isConfig = TCPIPStackIsConfig(pIf);

        for(pN = pArpDcpt->incompleteList.list.head; pN != NULL; pN = pN->next)
        {
            pE = FC_Node2HashEntry(pN); 
            if((pE->hEntry.flags.value & (uint16_t)ARP_FLAG_ENTRY_GRATUITOUS) != 0U)
            {
                maxRetries = arpMod.entryGratRetries;
            }
            else
            {
                maxRetries = arpMod.entryRetries;
            }
            if( pE->nRetries < maxRetries && (arpMod.timeSeconds - pE->tInsert) >= pE->nRetries * arpMod.entryRetryTmo)
            {   // expired, retry it
                if(isConfig == false || (pE->hEntry.flags.value & (uint16_t)ARP_FLAG_ENTRY_CONFIGURE) != 0U)
                {
                    (void)F_ARPSendIfPkt(pIf, ARP_OPERATION_REQ, (uint32_t)pIf->netIPAddr.Val, pE->ipAddress.Val, &arpBcastAdd, NULL);
                    pE->nRetries++;
                }
            }
        }

        // see the completed entries queue
        while( (pN = pArpDcpt->completeList.list.head) != NULL)
        {
            pE = FC_Node2HashEntry(pN); 
            if( (arpMod.timeSeconds - pE->tInsert) >= arpMod.entrySolvedTmo)
            {   // expired, remove it
                TCPIP_OAHASH_EntryRemove(pArpDcpt->hashDcpt, &pE->hEntry);
                (void)TCPIP_Helper_ProtSglListHeadRemove(&pArpDcpt->completeList);
                F_ARPNotifyClients(pIf, &pE->ipAddress, NULL, ARP_EVENT_REMOVED_EXPIRED);
            }
            else
            {   // this list is ordered, we can safely break out
                break;
            }
        }

        // finally purge, if needed
        if(pArpDcpt->hashDcpt->fullSlots >= pArpDcpt->purgeThres)
        {
            for(purgeIx = 0; purgeIx < pArpDcpt->purgeQuanta; purgeIx++)
            {
                pN = TCPIP_Helper_ProtSglListHeadRemove(&pArpDcpt->completeList);
                if(pN != NULL)
                {
                    pE = FC_Node2HashEntry(pN); 
                    TCPIP_OAHASH_EntryRemove(pArpDcpt->hashDcpt, &pE->hEntry);
                    F_ARPNotifyClients(pIf, &pE->ipAddress, NULL, ARP_EVENT_REMOVED_PURGED);
                }
                else
                {   // no more entries
                    break;
                }
            }
        } 

        pArpDcpt++;  
    } 

}

static void TCPIP_ARP_Process(void)
{
    const TCPIP_NET_IF* pInIf, *pTgtIf;
    TCPIP_MAC_PACKET* pPkt;

    ARP_PACKET      *pArpPkt;
    OA_HASH_ENTRY   *hE;
    ARP_CACHE_DCPT  *pArpDcpt;
    TCPIP_MAC_PKT_ACK_RES ackRes;
    TCPIP_ARP_RESULT arpReqRes;
    IPV4_ADDR targetAdd;
    IPV4_ADDR   algnSenderIpAddr;


    // extract queued ARP packets
    while((pPkt = TCPIPStackModuleRxExtract(TCPIP_THIS_MODULE_ID)) != NULL)
    {
        TCPIP_PKT_FlightLogRx(pPkt, TCPIP_THIS_MODULE_ID);
        arpReqRes = ARP_RES_OK;

        // Obtain the incoming ARP packet and process
        pArpPkt = FC_Uptr2ArpPkt(pPkt->pNetLayer);
        F_ARPSwapPacket(pArpPkt);

        // Validate the ARP packet
        if ( pArpPkt->HardwareType != HW_ETHERNET     ||
                pArpPkt->MACAddrLen != sizeof(TCPIP_MAC_ADDR)  ||
                pArpPkt->ProtocolLen != sizeof(IPV4_ADDR) )
        {
            ackRes = TCPIP_MAC_PKT_ACK_STRUCT_ERR;
        }
        else
        {
            targetAdd.Val = pArpPkt->TargetIPAddr.Val;
            // detect the proper alias interface
            pInIf = TCPIPStackMapAliasInterface((const TCPIP_NET_IF*)pPkt->pktIf, &targetAdd);
            pPkt->pktIf = FC_Net2Cvptr(pInIf);
            pArpDcpt = F_ARPGetIfDcpt(pInIf);

#ifdef TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL
            F_ARPProcessRxPkt(pInIf, pArpPkt);
#endif

            // Handle incoming ARP packet
            algnSenderIpAddr.Val = pArpPkt->SenderIPAddr.Val;
            hE = TCPIP_OAHASH_EntryLookup(pArpDcpt->hashDcpt, &algnSenderIpAddr.Val);
            if(hE != NULL)
            {   // we already have this sender and we should update it
                F_ARPUpdateEntry(pInIf, FC_Oa2HashEntry(hE), &pArpPkt->SenderMACAddr);
            }

            pTgtIf = NULL;
            if(targetAdd.Val == pInIf->netIPAddr.Val)
            {
                pTgtIf = pInIf;
            }
#if defined(TCPIP_STACK_USE_MAC_BRIDGE) && (!defined(TCPIP_STACK_MAC_BRIDGE_DISABLE_GLUE_PORTS) || (TCPIP_STACK_MAC_BRIDGE_DISABLE_GLUE_PORTS == 0))
            // check if there is another interface that has this IP address, bridged to pInIf
            else if(TCPIPStack_BridgeCheckIf(pInIf))
            {   // inIf is bridged
                TCPIP_NET_IF* pOtherIf = TCPIP_STACK_NetByAddress(&targetAdd);
                if(pOtherIf != NULL && TCPIPStack_BridgeCheckIf(pOtherIf))
                {   // inIf and target are bridged; however we check that they are in the same network
                    if(TCPIPStackNetNetwork(pOtherIf) == TCPIPStackNetNetwork(pInIf))
                    {
                        pTgtIf = pOtherIf;
                    }
                }
            }
#endif  // defined(TCPIP_STACK_USE_MAC_BRIDGE) && (!defined(TCPIP_STACK_MAC_BRIDGE_DISABLE_GLUE_PORTS) || (TCPIP_STACK_MAC_BRIDGE_DISABLE_GLUE_PORTS == 0))
            else
            {
                // do nothing
            }

            while( pTgtIf != NULL && !TCPIPStackIsConfig(pTgtIf))
            {   // we are the target and we should add to cache anyway
                if(hE == NULL)
                {   // not there yet
                    arpReqRes = F_ARPAddCompleteEntry(pInIf, &algnSenderIpAddr, &pArpPkt->SenderMACAddr);
                }

                // Handle incoming ARP operation
                if(pArpPkt->Operation == (uint16_t)ARP_OPERATION_REQ)
                {   
                    // ARP packet asking for this host IP address 
#ifdef TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL
                    /* Fix for Loop-Back suppression:
                     * For ZCLL-Claim packets, host should not respond.
                     * Check Sender's MAC-address with own MAC-address and 
                     * if it is matched, response will not be sent back. This
                     * was leading to flooding of ARP-answeres */
                    if(memcmp(pArpPkt->SenderMACAddr.v, pTgtIf->netMACAddr.v, 6) == 0)
                    {
#if ((ARP_DEBUG_MASK & ARP_DEBUG_ZCLL_MASK) != 0)
                        SYS_CONSOLE_MESSAGE("Loopback answer suppressed \r\n");
#endif  // ((ARP_DEBUG_MASK & ARP_DEBUG_ZCLL_MASK) != 0)
                        break;
                    }
#endif

                    // Need to send a reply to the requestor 
                    // Send an ARP response to the received request
                    if(!F_ARPSendIfPkt(pInIf, ARP_OPERATION_RESP, (uint32_t)pTgtIf->netIPAddr.Val, (uint32_t)pArpPkt->SenderIPAddr.Val, &pArpPkt->SenderMACAddr, &pTgtIf->netMACAddr))
                    {
                        arpReqRes =  ARP_RES_TX_FAILED;
                    }
                }
                break;
            }

            ackRes = TCPIP_MAC_PKT_ACK_RX_OK;
        }

        if(arpReqRes != ARP_RES_OK)
        {
        }

        TCPIP_PKT_PacketAcknowledge(pPkt, ackRes); 
    }

}

    
// the IP layer should request for the proper IP address!
// no checking is done at this level
TCPIP_ARP_RESULT TCPIP_ARP_Resolve(TCPIP_NET_HANDLE hNet, const IPV4_ADDR* IPAddr)
{
    TCPIP_NET_IF *pIf;
   
    if(IPAddr == NULL || IPAddr->Val == 0U)
    {   // do not store 0's in cache
        return ARP_RES_BAD_ADDRESS;
    }
    
    pIf = TCPIPStackHandleToNetLinked(hNet);
    if(pIf == NULL)
    {
        return ARP_RES_NO_INTERFACE;
    }

    if(TCPIPStackIsConfig(pIf))
    {   // no ARP probes during configuration
        return ARP_RES_CONFIGURE_ERR;
    }

    return F_ARPProbeAddress(pIf, IPAddr, &pIf->netIPAddr, ARP_OPERATION_REQ, NULL);
}

TCPIP_ARP_RESULT TCPIP_ARP_Probe(TCPIP_NET_HANDLE hNet, const IPV4_ADDR* IPAddr, const IPV4_ADDR* srcAddr, TCPIP_ARP_OPERATION_TYPE opType)
{
    TCPIP_NET_IF *pIf;

    if(IPAddr == NULL || IPAddr->Val == 0U || srcAddr == NULL)
    {   // do not store 0's in cache
        return ARP_RES_BAD_ADDRESS;
    }

    pIf =TCPIPStackHandleToNetLinked(hNet);
    if(pIf == NULL)
    {
        return ARP_RES_NO_INTERFACE;
    }

    uint32_t uApOp = FC_ArpOp2Uval(opType);
    uApOp &= (uint32_t)ARP_OPERATION_CONFIGURE;
    if(TCPIPStackIsConfig(pIf) && uApOp == 0U)
    {   // no ARP probes during configuration
        return ARP_RES_CONFIGURE_ERR;
    }

    return F_ARPProbeAddress(pIf, IPAddr, srcAddr, opType, NULL);
}


static TCPIP_ARP_RESULT F_ARPProbeAddress(TCPIP_NET_IF* pIf, const IPV4_ADDR* IPAddr, const IPV4_ADDR* srcAddr, TCPIP_ARP_OPERATION_TYPE opType, TCPIP_MAC_ADDR* pHwAdd)
{
    ARP_CACHE_DCPT  *pArpDcpt;
    OA_HASH_ENTRY   *hE;
    ARP_HASH_ENTRY* arpHE;
    uint32_t        uArpOp;
    uint32_t        uFlags;
    TCPIP_ARP_OPERATION_TYPE sendOpType;

    uArpOp = FC_ArpOp2Uval(opType);
    if((uArpOp & (uint32_t)ARP_OPERATION_PROBE_ONLY) != 0U)
    {   // just send an ARP probe
        uArpOp = FC_ArpOp2Uval(opType);
        uArpOp &= (uint32_t)ARP_OPERATION_MASK;
        sendOpType = FC_Uval2ArpOp(uArpOp);
        return F_ARPSendIfPkt(pIf, sendOpType, (uint32_t)srcAddr->Val, (uint32_t)IPAddr->Val, &arpBcastAdd, NULL) ? ARP_RES_PROBE_OK : ARP_RES_PROBE_FAILED;
    }

    pArpDcpt = F_ARPGetIfDcpt(pIf);

    hE = TCPIP_OAHASH_EntryLookupOrInsert(pArpDcpt->hashDcpt, &IPAddr->Val);
    if(hE == NULL)
    {   // oops!
        return ARP_RES_CACHE_FULL;
    }
        
    if(hE->flags.newEntry != 0U)
    {   // new entry; add it to the not done list 
        uArpOp = FC_ArpOp2Uval(opType);

        uFlags = (uArpOp & (uint32_t)ARP_OPERATION_CONFIGURE) != 0U ? (uint32_t)ARP_FLAG_ENTRY_CONFIGURE : (uint32_t)ARP_FLAG_NONE;
        if((uArpOp & (uint32_t)ARP_OPERATION_GRATUITOUS) != 0U) 
        {
            uFlags |= (uint32_t)ARP_FLAG_ENTRY_GRATUITOUS; 
        }
        arpHE = FC_Oa2HashEntry(hE);
        F_ARPSetEntry(arpHE, FC_Uval2EntryFlags(uFlags), NULL, &pArpDcpt->incompleteList);

        // initiate an ARP request operation
        uArpOp &= (uint32_t)ARP_OPERATION_MASK;
        sendOpType = FC_Uval2ArpOp(uArpOp);

        (void)F_ARPSendIfPkt(pIf, sendOpType, (uint32_t)srcAddr->Val, arpHE->ipAddress.Val, &arpBcastAdd, NULL);
        return ARP_RES_ENTRY_NEW;
    }
    // else, even if it is not complete, TCPIP_ARP_Task will initiate retransmission
    // Normally if the entry is existent, it should be refreshed, since it's obviously needed.
    // However, the TCPIP_ARP_IsResolved() will do it, because that's the call that actually uses the entry!
    if((hE->flags.value & (uint16_t)ARP_FLAG_ENTRY_VALID_MASK) != 0U)
    {   // found address in cache
        arpHE = FC_Oa2HashEntry(hE);
        if(pHwAdd != NULL)
        {
            *pHwAdd = arpHE->hwAdd;
        }
        if((hE->flags.value & (uint16_t)ARP_FLAG_ENTRY_COMPLETE) != 0U)
        {   // an existent entry, re-used, gets refreshed
            F_ARPRefreshEntry(arpHE, &pArpDcpt->completeList);
        }
        return ARP_RES_ENTRY_SOLVED;
    }
    
    // incomplete
    return ARP_RES_ENTRY_QUEUED;


}

bool TCPIP_ARP_IsResolved(TCPIP_NET_HANDLE hNet, const IPV4_ADDR* IPAddr, TCPIP_MAC_ADDR* MACAddr)
{
    OA_HASH_ENTRY   *hE;
    ARP_CACHE_DCPT  *pArpDcpt;
    TCPIP_NET_IF  *pIf;

    if(IPAddr == NULL || IPAddr->Val == 0U)
    {   
        return false;
    }

    pIf = TCPIPStackHandleToNetUp(hNet);
    if(pIf == NULL)
    {
        return false;
    }
    

    pArpDcpt = F_ARPGetIfDcpt(pIf);
    
    hE = TCPIP_OAHASH_EntryLookup(pArpDcpt->hashDcpt, &IPAddr->Val);
    if(hE != NULL && (hE->flags.value & (uint16_t)ARP_FLAG_ENTRY_VALID_MASK) != 0U)
    {   // found address in cache
        ARP_HASH_ENTRY* arpHE = FC_Oa2HashEntry(hE);
        if(MACAddr != NULL)
        {
            *MACAddr = arpHE->hwAdd;
        }
        if((hE->flags.value & (uint16_t)ARP_FLAG_ENTRY_COMPLETE) != 0U)
        {   // an existent entry, re-used, gets refreshed
            F_ARPRefreshEntry(arpHE, &pArpDcpt->completeList);
        }
        return true;
    }
    
    return false;
    
}



/*****************************************************************************
  Function:
    void F_ARPSwapPacket(ARP_PACKET* p)

  Description:
    Swaps endian-ness of header information in an ARP packet.

  Precondition:
    None

  Parameters:
    p - The ARP packet to be swapped

  Returns:
    None
  ***************************************************************************/
static void F_ARPSwapPacket(ARP_PACKET* p)
{
    p->HardwareType     = TCPIP_Helper_htons(p->HardwareType);
    p->Protocol         = TCPIP_Helper_htons(p->Protocol);
    p->Operation        = TCPIP_Helper_htons(p->Operation);
}

TCPIP_ARP_RESULT TCPIP_ARP_EntrySet(TCPIP_NET_HANDLE hNet, const IPV4_ADDR* ipAdd, const TCPIP_MAC_ADDR* hwAdd, bool perm)
{
    ARP_CACHE_DCPT  *pArpDcpt;
    ARP_HASH_ENTRY  *arpHE;
    OA_HASH_ENTRY   *hE;
    PROTECTED_SINGLE_LIST     *oldList, *newList;
    ARP_ENTRY_FLAGS newFlags;
    TCPIP_ARP_RESULT res;
    TCPIP_NET_IF    *pIf;
    bool setEntry;

    if(ipAdd == NULL || ipAdd->Val == 0U)
    {   // do not store 0's in cache
        return ARP_RES_BAD_ADDRESS;
    }
    
    pIf = TCPIPStackHandleToNetUp(hNet);
    if(pIf == NULL)
    {
        return ARP_RES_NO_INTERFACE;
    }
    
    pArpDcpt = F_ARPGetIfDcpt(pIf);

    hE = TCPIP_OAHASH_EntryLookupOrInsert(pArpDcpt->hashDcpt, &ipAdd->Val);
    if(hE == NULL)
    {   // oops!
        return ARP_RES_CACHE_FULL;
    }

    // where to put it
    if(perm)
    {
        newList = &pArpDcpt->permList;
        newFlags = ARP_FLAG_ENTRY_PERM;
    }
    else
    {
        newList = &pArpDcpt->completeList;
        newFlags = ARP_FLAG_ENTRY_COMPLETE;       // complete
    }
    
    arpHE = FC_Oa2HashEntry(hE);
    setEntry = false;
   
    if(hE->flags.newEntry == 0U)
    {   // existent entry
        if( (hE->flags.value & (uint16_t)ARP_FLAG_ENTRY_PERM) != 0U)
        {
            oldList =  &pArpDcpt->permList;
        }
        else if( (hE->flags.value & (uint16_t)ARP_FLAG_ENTRY_COMPLETE) != 0U)
        {
            oldList =  &pArpDcpt->completeList;
        }
        else
        {
            oldList =  &pArpDcpt->incompleteList;
        }

        if(newList != oldList)
        {   // remove from the old list
            (void)TCPIP_Helper_ProtSglListNodeRemove(oldList, &arpHE->lnode);
            setEntry = true;
        }
        res = ARP_RES_ENTRY_EXIST;
    }
    else
    {
        setEntry = true;
        res = ARP_RES_OK;
    }
    
    // add it to where it belongs
    if(setEntry == true)
    {
        F_ARPSetEntry(arpHE, newFlags, hwAdd, newList);

        if(perm)
        {
            if(TCPIP_Helper_ProtSglListCount(&pArpDcpt->permList) >= ((size_t)arpMod.permQuota * pArpDcpt->hashDcpt->hEntries) / 100U)
            {   // quota exceeded
                res = ARP_RES_PERM_QUOTA_EXCEED;
            }
        }
    }

    return res;
}

TCPIP_ARP_RESULT TCPIP_ARP_EntryGet(TCPIP_NET_HANDLE hNet, const IPV4_ADDR* ipAdd, TCPIP_MAC_ADDR* pHwAdd, bool probe)
{   
    TCPIP_NET_IF  *pIf;

    if(ipAdd == NULL || ipAdd->Val == 0U)
    {
        return ARP_RES_BAD_ADDRESS;
    }

    pIf = TCPIPStackHandleToNetUp(hNet);
    if(pIf == NULL)
    {
        return ARP_RES_NO_INTERFACE;
    }

    if(probe)
    {
        if(TCPIPStackIsConfig(pIf))
        {   // no ARP probes during configuration
            return ARP_RES_CONFIGURE_ERR;
        }
        return F_ARPProbeAddress(pIf, ipAdd, &pIf->netIPAddr, ARP_OPERATION_REQ, pHwAdd);
    }
    else
    {
        return (TCPIP_ARP_IsResolved(pIf, ipAdd, pHwAdd))? ARP_RES_ENTRY_SOLVED : ARP_RES_NO_ENTRY;
    }

}

TCPIP_ARP_RESULT TCPIP_ARP_EntryRemove(TCPIP_NET_HANDLE hNet,  const IPV4_ADDR* ipAdd)
{
    OA_HASH_ENTRY   *hE;
    ARP_HASH_ENTRY* arpHE;
    ARP_CACHE_DCPT  *pArpDcpt;
    TCPIP_NET_IF  *pIf;

    if(ipAdd == NULL || ipAdd->Val == 0U)
    {
        return ARP_RES_BAD_ADDRESS;
    }

    pIf = TCPIPStackHandleToNetUp(hNet);
    if(pIf == NULL)
    {
        return ARP_RES_NO_INTERFACE;
    }
    

    pArpDcpt = F_ARPGetIfDcpt(pIf);

    hE = TCPIP_OAHASH_EntryLookup(pArpDcpt->hashDcpt, &ipAdd->Val);

    if(hE == NULL)
    {
        return ARP_RES_NO_ENTRY;
    }

    F_ARPRemoveEntry(pArpDcpt, hE);
    arpHE = FC_Oa2HashEntry(hE);
    F_ARPNotifyClients(pIf, &arpHE->ipAddress, NULL, ARP_EVENT_REMOVED_USER);
    
    return ARP_RES_OK;
}


TCPIP_ARP_RESULT TCPIP_ARP_EntryRemoveNet(TCPIP_NET_HANDLE hNet, const IPV4_ADDR* ipAdd, const IPV4_ADDR* mask , TCPIP_ARP_ENTRY_TYPE type)
{
    OA_HASH_ENTRY   *hE;
    ARP_HASH_ENTRY  *arpHE;
    ARP_CACHE_DCPT  *pArpDcpt;
    OA_HASH_DCPT    *pOH;
    TCPIP_NET_IF    *pIf;
    size_t          index;
    uint16_t        andFlags, resFlags;
    uint32_t        matchAdd;

    if(ipAdd == NULL || ipAdd->Val == 0U)
    {
        return ARP_RES_BAD_ADDRESS;
    }

    pIf = TCPIPStackHandleToNetUp(hNet);
    if(pIf == NULL)
    {
        return ARP_RES_NO_INTERFACE;
    }

    TCPIP_ARP_RESULT arpRes = ARP_RES_OK;

    switch (type)
    {
        case ARP_ENTRY_TYPE_PERMANENT:
            andFlags = resFlags = (uint16_t)ARP_FLAG_ENTRY_PERM;
            break;

        case ARP_ENTRY_TYPE_COMPLETE:
            andFlags = resFlags =  (uint16_t)ARP_FLAG_ENTRY_COMPLETE;
            break;
            
        case ARP_ENTRY_TYPE_INCOMPLETE:
            andFlags = ((uint16_t)ARP_FLAG_ENTRY_PERM | (uint16_t)ARP_FLAG_ENTRY_COMPLETE);
            resFlags = 0;
            break;
            
        case ARP_ENTRY_TYPE_ANY:
            andFlags = resFlags = 0U;
            break;

        default:
            arpRes = ARP_RES_BAD_TYPE;
            break;
    }

    if(arpRes != ARP_RES_OK)
    {
        return arpRes;
    }


    pArpDcpt = F_ARPGetIfDcpt(pIf);
    pOH = pArpDcpt->hashDcpt;
    matchAdd = ipAdd->Val & mask->Val;

    
    // scan all entries
    for(index = 0; index < pOH->hEntries; index++)
    {
        hE = TCPIP_OAHASH_EntryGet(pArpDcpt->hashDcpt, index);
        if(hE->flags.busy != 0U)
        {
            if((hE->flags.value & andFlags) == resFlags)
            {   // flags match
                arpHE = FC_Oa2HashEntry(hE);
                if((arpHE->ipAddress.Val & mask->Val) == matchAdd)
                {   // address match;  delete entry
                    F_ARPRemoveEntry(pArpDcpt, hE);
                    F_ARPNotifyClients(pIf, &arpHE->ipAddress, NULL, ARP_EVENT_REMOVED_USER);
                }
            }
        }
    }

    return ARP_RES_OK;
}

TCPIP_ARP_RESULT TCPIP_ARP_EntryRemoveAll(TCPIP_NET_HANDLE hNet)
{
    OA_HASH_ENTRY   *hE;
    ARP_HASH_ENTRY  *arpHE;
    ARP_CACHE_DCPT  *pArpDcpt;
    OA_HASH_DCPT    *pOH;
    TCPIP_NET_IF    *pIf;
    size_t          index;


    pIf = TCPIPStackHandleToNetUp(hNet);
    if(pIf == NULL)
    {
        return ARP_RES_NO_INTERFACE;
    }

    pArpDcpt = F_ARPGetIfDcpt(pIf);
    pOH = pArpDcpt->hashDcpt;

    // scan all entries so we can notify that they are removed
    for(index = 0; index < pOH->hEntries; index++)
    {
        hE = TCPIP_OAHASH_EntryGet(pArpDcpt->hashDcpt, index);
        if(hE->flags.busy != 0U)
        {
            arpHE = FC_Oa2HashEntry(hE);
            // delete entry
            F_ARPRemoveEntry(pArpDcpt, hE);
            F_ARPNotifyClients(pIf, &arpHE->ipAddress, NULL, ARP_EVENT_REMOVED_USER);
        }
    }



    return ARP_RES_OK;
}

TCPIP_ARP_RESULT TCPIP_ARP_EntryQuery(TCPIP_NET_HANDLE hNet, size_t index, TCPIP_ARP_ENTRY_QUERY* pArpQuery)
{
    OA_HASH_ENTRY   *hE;
    ARP_HASH_ENTRY  *arpHE;
    ARP_CACHE_DCPT  *pArpDcpt;
    TCPIP_MAC_ADDR        noHwAdd = {{0, 0, 0, 0, 0, 0}};
    TCPIP_NET_IF  *pIf;

    pIf = TCPIPStackHandleToNetUp(hNet);
    if(pIf == NULL)
    {
        return ARP_RES_NO_INTERFACE;
    }

    pArpDcpt = F_ARPGetIfDcpt(pIf);
    hE = TCPIP_OAHASH_EntryGet(pArpDcpt->hashDcpt, index);
    

    if(hE == NULL)
    {
        return ARP_RES_BAD_INDEX;
    }
    
    arpHE = FC_Oa2HashEntry(hE);

    if(pArpQuery != NULL)
    {
        pArpQuery->entryIpAdd.Val = 0U;
        pArpQuery->entryHwAdd = noHwAdd;
        
        if(hE->flags.busy == 0U)
        {
            pArpQuery->entryType = ARP_ENTRY_TYPE_INVALID;
        }
        else if((hE->flags.value & (uint16_t)ARP_FLAG_ENTRY_VALID_MASK) != 0U)
        {
            pArpQuery->entryType = ((hE->flags.value & (uint16_t)ARP_FLAG_ENTRY_PERM) != 0U)?
                                ARP_ENTRY_TYPE_PERMANENT:ARP_ENTRY_TYPE_COMPLETE;
            pArpQuery->entryIpAdd.Val = arpHE->ipAddress.Val;
            pArpQuery->entryHwAdd = arpHE->hwAdd;
        }
        else
        {
            pArpQuery->entryType = ARP_ENTRY_TYPE_INCOMPLETE;
            pArpQuery->entryIpAdd.Val = arpHE->ipAddress.Val;
        }
    }

    return ARP_RES_OK;
}

size_t TCPIP_ARP_CacheEntriesNoGet(TCPIP_NET_HANDLE hNet, TCPIP_ARP_ENTRY_TYPE type)
{
    TCPIP_NET_IF  *pIf;
    
    pIf = TCPIPStackHandleToNetUp(hNet);
    if(pIf == NULL)
    {
        return 0;
    }
    
    ARP_CACHE_DCPT  *pArpDcpt = F_ARPGetIfDcpt(pIf);
    OA_HASH_DCPT    *pOH = pArpDcpt->hashDcpt;

    size_t nEntries;
    switch(type)
    {
        case ARP_ENTRY_TYPE_INVALID:
           nEntries = pOH->hEntries - pOH->fullSlots;
           break;

        case ARP_ENTRY_TYPE_PERMANENT:
           nEntries = TCPIP_Helper_ProtSglListCount(&pArpDcpt->permList);
           break;

        case ARP_ENTRY_TYPE_COMPLETE:
           nEntries = TCPIP_Helper_ProtSglListCount(&pArpDcpt->completeList);
           break;

        case ARP_ENTRY_TYPE_INCOMPLETE:
           nEntries = TCPIP_Helper_ProtSglListCount(&pArpDcpt->incompleteList);
           break;

        case ARP_ENTRY_TYPE_ANY:
           nEntries = pOH->fullSlots;
           break;

        default:    // case ARP_ENTRY_TYPE_TOTAL:
           nEntries = pOH->hEntries;
           break;
    }

    return nEntries;

}

TCPIP_ARP_RESULT TCPIP_ARP_CacheThresholdSet(TCPIP_NET_HANDLE hNet, size_t purgeThres, size_t purgeEntries)
{
    TCPIP_NET_IF  *pIf;

    pIf = TCPIPStackHandleToNetUp(hNet);
    if(pIf == NULL)
    {
        return ARP_RES_NO_INTERFACE;
    }
    
    ARP_CACHE_DCPT  *pArpDcpt = F_ARPGetIfDcpt(pIf);

    pArpDcpt->purgeThres = ((size_t)purgeThres * pArpDcpt->hashDcpt->hEntries + 99U) / 100U;
    pArpDcpt->purgeQuanta = purgeEntries;

    return ARP_RES_OK;
}

#if !defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )

// static versions
// 
size_t TCPIP_OAHASH_KeyHash(OA_HASH_DCPT* pOH, const void* key)
{
    return fnv_32_hash(key, sizeof(((ARP_HASH_ENTRY*)0)->ipAddress)) % (pOH->hEntries);
}

#if defined(OA_DOUBLE_HASH_PROBING)
size_t TCPIP_OAHASH_HashProbe(OA_HASH_DCPT* pOH, const void* key)
{
    return fnv_32a_hash(key, sizeof(((ARP_HASH_ENTRY*)0)->ipAddress)) % (pOH->hEntries);
}

#endif  // defined(OA_DOUBLE_HASH_PROBING)

// Deletes an entry to make room in the hash table.
// This shouldn't normally occur if TCPIP_ARP_Task()
// does its job of periodically performing the cache clean-up.
// However, since the threshold can be dynamically adjusted,
// the situation could still occur
OA_HASH_ENTRY* TCPIP_OAHASH_EntryDelete(OA_HASH_DCPT* pOH)
{
    ARP_CACHE_DCPT  *pArpDcpt;
    ARP_HASH_ENTRY  *pE;
    SGL_LIST_NODE   *pN;
    SINGLE_LIST     *pRemList = NULL;    
    
    pArpDcpt = (ARP_CACHE_DCPT*)pOH->hParam;

    if( (pN = pArpDcpt->incompleteList.head) != NULL)
    {
        pE = FC_Node2HashEntry(pN);
        if( (arpMod.timeSeconds - pE->tInsert) >= arpMod.entryPendingTmo)
        {   // we remove this one
            pRemList = &pArpDcpt->incompleteList;
        }
    }

    if(pRemList == NULL)
    {   // no luck with the incomplete list; use the complete one
            pRemList = &pArpDcpt->completeList;
    }

    pN = TCPIP_Helper_SingleListHeadRemove(pRemList);

    if(pN)
    {
        pE = FC_Node2HashEntry(pN);
        return &pE->hEntry;    
    }

    // it's possible to be unable to make room in the cache
    // for example, too many permanent entries added...
                   
    return 0;
}


int TCPIP_OAHASH_KeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key)
{
    return ((ARP_HASH_ENTRY*)hEntry)->ipAddress.Val != ((ARP_UNALIGNED_KEY*)key)->v;
}

void TCPIP_OAHASH_KeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key)
{

    ((ARP_HASH_ENTRY*)dstEntry)->ipAddress.Val = ((ARP_UNALIGNED_KEY*)key)->v;
}

#else   // defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )
// dynamic versions
// 
size_t TCPIP_ARP_HashKeyHash(OA_HASH_DCPT* pOH, const void* key)
{
    return fnv_32_hash(key, sizeof(((ARP_HASH_ENTRY*)0)->ipAddress)) % (pOH->hEntries);
}

#if defined(OA_DOUBLE_HASH_PROBING)
size_t TCPIP_ARP_HashProbeHash(OA_HASH_DCPT* pOH, const void* key)
{
    return fnv_32a_hash(key, sizeof(((ARP_HASH_ENTRY*)0)->ipAddress)) % (pOH->hEntries);
}

#endif  // defined(OA_DOUBLE_HASH_PROBING)

// Deletes an entry to make room in the hash table.
// This shouldn't normally occur if TCPIP_ARP_Task()
// does its job of periodically performing the cache clean-up.
// However, since the threshold can be dynamically adjusted,
// the situation could still occur
OA_HASH_ENTRY* TCPIP_ARP_HashEntryDelete(OA_HASH_DCPT* pOH)
{
    ARP_CACHE_DCPT  *pArpDcpt;
    ARP_HASH_ENTRY  *pE;
    SGL_LIST_NODE   *pN;
    PROTECTED_SINGLE_LIST     *pRemList = NULL;
    
    pArpDcpt = FC_Cvptr2ArpDcpt(pOH->hParam);

    if( (pN = pArpDcpt->incompleteList.list.head) != NULL)
    {
        pE = FC_Node2HashEntry(pN);
        if( (arpMod.timeSeconds - pE->tInsert) >= arpMod.entryPendingTmo)
        {   // we remove this one
            pRemList = &pArpDcpt->incompleteList;
        }
    }

    if(pRemList == NULL)
    {   // no luck with the incomplete list; use the complete one
            pRemList = &pArpDcpt->completeList;
    }

    pN = TCPIP_Helper_ProtSglListHeadRemove(pRemList);

    if(pN != NULL)
    {
        pE = FC_Node2HashEntry(pN);
        return &pE->hEntry;    
    }

    // it's possible to be unable to make room in the cache
    // for example, too many permanent entries added...
                   
    return NULL;
}


int TCPIP_ARP_HashKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key)
{
    ARP_HASH_ENTRY* arpHE = FC_Oa2HashEntry(hEntry);
    return (arpHE->ipAddress.Val == ((const ARP_UNALIGNED_KEY*)key)->v) ? 0 : 1;
}

void TCPIP_ARP_HashKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key)
{
    ARP_HASH_ENTRY* arpHE = FC_Oa2HashEntry(dstEntry);
    arpHE->ipAddress.Val = ((const ARP_UNALIGNED_KEY*)key)->v;
}


#endif  // !defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )

#endif  // defined(TCPIP_STACK_USE_IPV4)


