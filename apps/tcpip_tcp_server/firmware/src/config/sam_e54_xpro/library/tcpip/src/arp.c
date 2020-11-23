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

/*****************************************************************************
 Copyright (C) 2011-2018 Microchip Technology Inc. and its subsidiaries.

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
    int                 nIfs;                // number of interfaces ARP running on
    ARP_CACHE_DCPT*     arpCacheDcpt;        // ARP caches per interface
    const void*         memH;                // memory allocation handle
    int                 initCount;           // ARP module initialization count
    bool                deleteOld;           // if 0 and old cache still in place don't re-initialize it

    uint32_t            timeSeconds;         // coarse ARP time keeping, seconds
    uint32_t            timeMs;              // coarse ARP time keeping, milliseconds
    tcpipSignalHandle   timerHandle;

    PROTECTED_SINGLE_LIST registeredUsers;     // notification users
    // timing
    uint32_t            entrySolvedTmo;      // solved entry removed after this tmo
                                             // if not referenced - seconds
    uint32_t            entryPendingTmo;     // timeout for a pending to be solved entry in the cache, in seconds
    uint32_t            entryRetryTmo;       // timeout for resending an ARP request for a pending entry - seconds
                                             // 1 sec < tmo < entryPendingTmo
    int                 permQuota;           // max percentage of permanent entries allowed in the cache - %
    uint16_t            entryRetries;        // number of retries for a regular ARP cache entry
    uint16_t            entryGratRetries;    // number of retries for a gratuitous ARP request; default is 1

    TCPIP_MAC_PACKET*   pMacPkt;             // packet that we use to send requests ARP requests
                                             // only ONE packet is used for now even if there are multiple interfaces!!!
}ARP_MODULE_DCPT;


// the module descriptor
static ARP_MODULE_DCPT arpMod = { 0 };


static TCPIP_MAC_ADDR             arpBcastAdd = { {0xff, 0xff, 0xff, 0xff, 0xff, 0xff} };

#ifdef TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL
#define MAX_REG_APPS            2           // MAX num allowed registrations of Modules/Apps
static struct arp_app_callbacks reg_apps[MAX_REG_APPS]; // Call-Backs storage for MAX of two Modules/Apps

#endif

/****************************************************************************
  Section:
    Helper Function Prototypes
  ***************************************************************************/

static bool         _ARPSendIfPkt(TCPIP_NET_IF* pIf, TCPIP_ARP_OPERATION_TYPE oper, uint32_t srcIP, uint32_t dstIP, TCPIP_MAC_ADDR* dstMAC);

#ifdef TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL
static void         _ARPProcessRxPkt(TCPIP_NET_IF* pIf, ARP_PACKET* packet);
#endif

static void         _SwapARPPacket(ARP_PACKET* p);

static void         _ARPUpdateEntry(TCPIP_NET_IF* pIf, ARP_HASH_ENTRY* arpHE, TCPIP_MAC_ADDR* hwAdd);
static TCPIP_ARP_RESULT   _ARPAddCompleteEntry(TCPIP_NET_IF* pIf, IPV4_ADDR* pIPAddr, TCPIP_MAC_ADDR* hwAdd);
    
#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void         _ARPDeleteResources(void);
static void         _ARPDeleteCache(ARP_CACHE_DCPT* pArpDcpt);
static void         _ARPDeleteClients(void);
#else
#define _ARPDeleteResources()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static void         _ARPNotifyClients(TCPIP_NET_IF* pNetIf, const IPV4_ADDR* ipAdd, const TCPIP_MAC_ADDR* MACAddr, TCPIP_ARP_EVENT_TYPE evType);

static TCPIP_ARP_RESULT   _ARPProbeAddress(TCPIP_NET_IF* pIf, IPV4_ADDR* IPAddr, IPV4_ADDR* srcAddr, TCPIP_ARP_OPERATION_TYPE opType, TCPIP_MAC_ADDR* pHwAdd);

static TCPIP_MAC_PACKET* _ARPAllocateTxPacket(void);

static bool         _ARPTxAckFnc (TCPIP_MAC_PACKET * pPkt, const void * param);

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

/*static __inline__*/static  void /*__attribute__((always_inline))*/ _ARPSetEntry(ARP_HASH_ENTRY* arpHE, ARP_ENTRY_FLAGS newFlags,
                                                                      TCPIP_MAC_ADDR* hwAdd, PROTECTED_SINGLE_LIST* addList)
{
    arpHE->hEntry.flags.value &= ~ARP_FLAG_ENTRY_VALID_MASK;
    arpHE->hEntry.flags.value |= newFlags;
    
    if(hwAdd)
    {
        arpHE->hwAdd = *hwAdd;
    }
    
    arpHE->tInsert = arpMod.timeSeconds;
    arpHE->nRetries = 1;
    if(addList)
    {
        TCPIP_Helper_ProtectedSingleListTailAdd(addList, (SGL_LIST_NODE*)&arpHE->next);
    }
}


// re-inserts at the tail, makes the entry fresh
/*static __inline__*/static  void /*__attribute__((always_inline))*/ _ARPRefreshEntry(ARP_HASH_ENTRY* arpHE, PROTECTED_SINGLE_LIST* pL)
{
    TCPIP_Helper_ProtectedSingleListNodeRemove(pL, (SGL_LIST_NODE*)&arpHE->next);
    arpHE->tInsert = arpMod.timeSeconds;
    TCPIP_Helper_ProtectedSingleListTailAdd(pL, (SGL_LIST_NODE*)&arpHE->next);
}

/*static __inline__*/static  void /*__attribute__((always_inline))*/ _ARPRemoveCacheEntries(ARP_CACHE_DCPT* pArpDcpt)
{

    if(pArpDcpt->hashDcpt)
    {
        TCPIP_OAHASH_EntriesRemoveAll(pArpDcpt->hashDcpt);
        TCPIP_Helper_ProtectedSingleListRemoveAll(&pArpDcpt->incompleteList);
        TCPIP_Helper_ProtectedSingleListRemoveAll(&pArpDcpt->completeList);
        TCPIP_Helper_ProtectedSingleListRemoveAll(&pArpDcpt->permList);
    }
}

static  void _ARPRemoveEntry(ARP_CACHE_DCPT* pArpDcpt, OA_HASH_ENTRY* hE)
{
    PROTECTED_SINGLE_LIST     *remList;

    if((hE->flags.value & ARP_FLAG_ENTRY_PERM) != 0 )
    {
        remList =  &pArpDcpt->permList;
    }
    else if((hE->flags.value & ARP_FLAG_ENTRY_COMPLETE) != 0 )
    {
        remList =  &pArpDcpt->completeList;
    }
    else
    {
        remList =  &pArpDcpt->incompleteList;
    }

    TCPIP_Helper_ProtectedSingleListNodeRemove(remList, (SGL_LIST_NODE*)&((ARP_HASH_ENTRY*)hE)->next);

    TCPIP_OAHASH_EntryRemove(pArpDcpt->hashDcpt, hE);

}

#if (TCPIP_ARP_PRIMARY_CACHE_ONLY != 0)
static __inline__  ARP_CACHE_DCPT*  __attribute__((always_inline)) _ARPGetIfDcpt(TCPIP_NET_IF* pIf)
{
    return arpMod.arpCacheDcpt + TCPIP_STACK_NetIxGet(_TCPIPStackNetGetPrimary(pIf));
}
#else
static __inline__  ARP_CACHE_DCPT*  __attribute__((always_inline)) _ARPGetIfDcpt(TCPIP_NET_IF* pIf)
{
    return arpMod.arpCacheDcpt + TCPIP_STACK_NetIxGet(pIf);
}
#endif // (TCPIP_ARP_PRIMARY_CACHE_ONLY != 0)

/****************************************************************************
  Section:
    Function Implementations
  ***************************************************************************/
#ifdef TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL
/************ User Application APIs ****************************************/

/*****************************************************************************
  Function:
    int8_t TCPIP_ARP_CallbacksRegister(struct arp_app_callbacks *app)

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
int8_t TCPIP_ARP_CallbacksRegister(struct arp_app_callbacks *app)
{
    uint8_t i;
    for(i=0; i<MAX_REG_APPS; i++)
    {
        if(!reg_apps[i].used)
        {
            reg_apps[i].TCPIP_ARP_PacketNotify = app->TCPIP_ARP_PacketNotify;
            reg_apps[i].used = 1;
            return (i+1); // Return Code. Should be used in deregister.
        }
    }
    return -1; // No space for registration
}

/*****************************************************************************
  Function:
    bool TCPIP_ARP_CallbacksDeregister(int8_t reg_id)

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
    reg_id - Registration-id returned in TCPIP_ARP_CallbacksRegister call
    
  Returns:
    true  - On success
    false - Failure to indicate invalid reg_id  
  ***************************************************************************/ 
bool TCPIP_ARP_CallbacksDeregister(int8_t reg_id)
{
    if(reg_id <= 0 || reg_id > MAX_REG_APPS)
        return false;

    reg_apps[reg_id-1].used = 0; // To indicate free slot for registration
    return true;
}


/*****************************************************************************
  Function:
    void _ARPProcessRxPkt(TCPIP_NET_IF* pIf, ARP_PACKET* packet)

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
static void _ARPProcessRxPkt(TCPIP_NET_IF* pIf, ARP_PACKET* packet)
{
    uint8_t pass_on = 0; // Flag to indicate whether need to be forwarded
    uint8_t i;

    // Probing Stage
    if(pIf->netIPAddr.Val == 0x00)
    {
        pass_on = 1; // Pass to Registered-Application for further processing        
    }
    else if(pIf->netIPAddr.Val)
    {
        /* Late-conflict */
        if(packet->SenderIPAddr.Val == pIf->netIPAddr.Val)
        {
            pass_on = 1;
        }
    }
    if(pass_on)
    {
    
        for(i =0; i< MAX_REG_APPS; i++)
        {
            if(reg_apps[i].used)
            {
                reg_apps[i].TCPIP_ARP_PacketNotify(pIf,
				packet->SenderIPAddr.Val,
                                packet->TargetIPAddr.Val,
                                &packet->SenderMACAddr,
                                &packet->TargetMACAddr,
                                packet->Operation);                
            }
        }
    }
}

#endif  // TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL


/*****************************************************************************
  Function:
    static bool _ARPSendIfPkt(TCPIP_NET_IF* pIf, TCPIP_ARP_OPERATION_TYPE oper, uint32_t srcIP, uint32_t dstIP, TCPIP_MAC_ADDR* dstMAC)

  Description:
    Writes an ARP packet to the MAC using the interface pointer for src IP and MAC address.

  Precondition:
    None

  Parameters:

  Return Values:
    true - The ARP packet was generated properly
    false - otherwise

  ***************************************************************************/
static bool _ARPSendIfPkt(TCPIP_NET_IF* pIf, TCPIP_ARP_OPERATION_TYPE oper, uint32_t srcIP, uint32_t dstIP, TCPIP_MAC_ADDR* dstMAC)
{
    TCPIP_MAC_PACKET* pMacPkt;
    ARP_PACKET*       pArp;

    if(arpMod.pMacPkt != 0 && (arpMod.pMacPkt->pktFlags & TCPIP_MAC_PKT_FLAG_QUEUED) == 0)
    {
        pMacPkt = arpMod.pMacPkt;
    }
    else
    {   // packet not available, have to allocate another one
        if((pMacPkt = _ARPAllocateTxPacket()) == 0)
        {
            return false;
        }
        arpMod.pMacPkt = pMacPkt;   // show we're using this one now
    }


    pArp = (ARP_PACKET*)pMacPkt->pNetLayer;

    pArp->HardwareType  = HW_ETHERNET;
    pArp->Protocol      = ARP_IP;
    pArp->MACAddrLen    = sizeof(TCPIP_MAC_ADDR);
    pArp->ProtocolLen   = sizeof(IPV4_ADDR);
    pArp->Operation = oper;

    pArp->SenderMACAddr = pIf->netMACAddr;
    pArp->SenderIPAddr.Val  = srcIP;
    pArp->TargetMACAddr = *dstMAC;
    pArp->TargetIPAddr.Val  = dstIP;

    _SwapARPPacket(pArp);

    // format the MAC packet
    pMacPkt->pDSeg->segLen = sizeof(ARP_PACKET);
    if(TCPIP_PKT_PacketMACFormat(pMacPkt, dstMAC, (const TCPIP_MAC_ADDR*)TCPIP_STACK_NetUpMACAddressGet(pIf), TCPIP_ETHER_TYPE_ARP))
    {
        pMacPkt->next = 0;  // send single packet
        pMacPkt->pktIf = pIf;
        TCPIP_PKT_FlightLogTx(pMacPkt, TCPIP_THIS_MODULE_ID);
        if(_TCPIPStackPacketTx(pIf, pMacPkt) >= 0)
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
    static void _ARPUpdateEntry(TCPIP_NET_IF* pIf, ARP_HASH_ENTRY* arpHE, TCPIP_MAC_ADDR* hwAdd)

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
static void _ARPUpdateEntry(TCPIP_NET_IF* pIf, ARP_HASH_ENTRY* arpHE, TCPIP_MAC_ADDR* hwAdd)
{
    TCPIP_ARP_EVENT_TYPE evType; 
    ARP_CACHE_DCPT  *pArpDcpt;
    
    pArpDcpt = _ARPGetIfDcpt(pIf);
    if((arpHE->hEntry.flags.value & ARP_FLAG_ENTRY_PERM) == 0)
    {   

        if((arpHE->hEntry.flags.value & ARP_FLAG_ENTRY_COMPLETE) == 0)
        {   // was waiting for this one, it was queued
            evType = ARP_EVENT_SOLVED;
            TCPIP_Helper_ProtectedSingleListNodeRemove(&pArpDcpt->incompleteList, (SGL_LIST_NODE*)&arpHE->next);
        }
        else
        {   // completed entry, but now updated
            evType = ARP_EVENT_UPDATED;
            TCPIP_Helper_ProtectedSingleListNodeRemove(&pArpDcpt->completeList, (SGL_LIST_NODE*)&arpHE->next);
        }
        
        // move to tail, updated
        _ARPSetEntry(arpHE, ARP_FLAG_ENTRY_COMPLETE, hwAdd, &pArpDcpt->completeList);
    }
    else
    {   // permanent entries are not updated
        evType = ARP_EVENT_PERM_UPDATE;
    }

    _ARPNotifyClients(pIf, &arpHE->ipAddress, &arpHE->hwAdd, evType);

}


/*****************************************************************************
  Function:
    static TCPIP_ARP_RESULT _ARPAddCompleteEntry(TCPIP_NET_IF* pIf, IPV4_ADDR* pIPAddr, TCPIP_MAC_ADDR* hwAdd)

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
static TCPIP_ARP_RESULT _ARPAddCompleteEntry(TCPIP_NET_IF* pIf, IPV4_ADDR* pIPAddr, TCPIP_MAC_ADDR* hwAdd)
{
    ARP_CACHE_DCPT  *pArpDcpt;
    ARP_HASH_ENTRY  *arpHE;
    OA_HASH_ENTRY   *hE;

    pArpDcpt = _ARPGetIfDcpt(pIf);
    
    hE = TCPIP_OAHASH_EntryLookupOrInsert(pArpDcpt->hashDcpt, pIPAddr);
    if(hE == 0)
    {   // oops, hash full?
        return ARP_RES_CACHE_FULL;
    }

    // now in cache
    arpHE = (ARP_HASH_ENTRY*)hE;
    if(arpHE->hEntry.flags.newEntry != 0)
    {   // populate the new entry
        _ARPSetEntry(arpHE, ARP_FLAG_ENTRY_COMPLETE, hwAdd, &pArpDcpt->completeList);
    }
    else
    {   // existent entry
        _ARPUpdateEntry(pIf, arpHE, hwAdd);
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
bool TCPIP_ARP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_ARP_MODULE_CONFIG* arpData)
{
    OA_HASH_DCPT*   hashDcpt;
    ARP_CACHE_DCPT* pArpDcpt;
    size_t          hashMemSize;
    int             ix, nArpIfs;
    bool            iniRes;

    if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface going up
        // store the delete option for de-initialization
        if(arpMod.deleteOld)
        {   // remove the old entries, if there
#if (TCPIP_ARP_PRIMARY_CACHE_ONLY != 0)
            if(!_TCPIPStackNetIsPrimary(stackCtrl->pNetIf))
            {
                return true;
            }
#endif // (TCPIP_ARP_PRIMARY_CACHE_ONLY != 0)
            
            pArpDcpt = _ARPGetIfDcpt(stackCtrl->pNetIf);
            _ARPRemoveCacheEntries(pArpDcpt);
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
        if(arpData == 0)
        {
            return false;
        }

        // store the delete option for de-initialization
        arpMod.deleteOld = arpData->deleteOld;

        // check if there's any persistent data
#if (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT) && (TCPIP_ARP_PRIMARY_CACHE_ONLY != 0)
        nArpIfs = stackCtrl->nIfs - stackCtrl->nAliases;
#else
        nArpIfs = stackCtrl->nIfs;
#endif // (TCPIP_ARP_PRIMARY_CACHE_ONLY != 0)
        if(arpMod.arpCacheDcpt != 0 && (arpData->deleteOld || arpMod.nIfs != nArpIfs))
        {   // delete the old copy
            _ARPDeleteResources();
        }

        // store the memory allocation handle
        arpMod.memH = stackCtrl->memH;
        arpMod.nIfs =  nArpIfs;

        // parameters initialization
        arpMod.entrySolvedTmo = arpData->entrySolvedTmo;
        arpMod.entryPendingTmo = arpData->entryPendingTmo;
        arpMod.entryRetryTmo = arpData->entryRetryTmo;
        arpMod.permQuota = arpData->permQuota;
        arpMod.entryRetries = arpData->retries;
        arpMod.entryGratRetries =  arpData->gratProbeCount;


        if(arpMod.arpCacheDcpt == 0)
        {
            arpMod.arpCacheDcpt = (ARP_CACHE_DCPT*)TCPIP_HEAP_Calloc(arpMod.memH, arpMod.nIfs, sizeof(*arpMod.arpCacheDcpt)); 
            if(arpMod.arpCacheDcpt == 0)
            {   // failed
                return false;
            }

            hashMemSize = sizeof(OA_HASH_DCPT) + arpData->cacheEntries * sizeof(ARP_HASH_ENTRY);
            for(ix = 0, pArpDcpt = arpMod.arpCacheDcpt; ix < arpMod.nIfs; ix++, pArpDcpt++)
            {
                hashDcpt = (OA_HASH_DCPT*)TCPIP_HEAP_Malloc(arpMod.memH, hashMemSize);

                if(hashDcpt == 0)
                {   // failed
                    _ARPDeleteResources();
                    return false;
                }

                // populate the entries
                hashDcpt->memBlk = hashDcpt + 1;
                hashDcpt->hParam = pArpDcpt;    // store the descriptor it belongs to
                hashDcpt->hEntrySize = sizeof(ARP_HASH_ENTRY);
                hashDcpt->hEntries = arpData->cacheEntries;
                hashDcpt->probeStep = ARP_HASH_PROBE_STEP;

#if defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )
                hashDcpt->hashF = TCPIP_ARP_HashKeyHash;
#if defined(OA_DOUBLE_HASH_PROBING)
                hashDcpt->probeHash = TCPIP_ARP_HashProbeHash;
#endif  // defined(OA_DOUBLE_HASH_PROBING)
                hashDcpt->delF = TCPIP_ARP_HashEntryDelete;
                hashDcpt->cmpF = TCPIP_ARP_HashKeyCompare;
                hashDcpt->cpyF = TCPIP_ARP_HashKeyCopy; 
#endif  // defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )
                TCPIP_OAHASH_Initialize(hashDcpt);

                pArpDcpt->hashDcpt = hashDcpt;
                while(true)
                {
                    if((iniRes = TCPIP_Helper_ProtectedSingleListInitialize(&pArpDcpt->permList)) == false)
                    {
                        break;
                    }

                    if((iniRes = TCPIP_Helper_ProtectedSingleListInitialize(&pArpDcpt->completeList)) == false)
                    {
                        break;
                    }

                    iniRes = TCPIP_Helper_ProtectedSingleListInitialize(&pArpDcpt->incompleteList);
                    break;
                }

                if(iniRes == false)
                {
                    _ARPDeleteResources();
                    return false;
                }

                pArpDcpt->purgeThres = (arpData->purgeThres * pArpDcpt->hashDcpt->hEntries + 99)/100;
                pArpDcpt->purgeQuanta = arpData->purgeQuanta;

            }

            arpMod.pMacPkt = _ARPAllocateTxPacket();
            arpMod.timerHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_ARP_Task, TCPIP_ARP_TASK_PROCESS_RATE);
            iniRes = TCPIP_Notification_Initialize(&arpMod.registeredUsers);

            if(arpMod.pMacPkt == 0 || arpMod.timerHandle == 0 || iniRes == false)
            {
                _ARPDeleteResources();
                return false;
            }

            arpMod.timeSeconds = arpMod.timeMs = 0;
        }
    }

    // per interface initialization
    pArpDcpt = _ARPGetIfDcpt(stackCtrl->pNetIf);

    if(arpMod.deleteOld)
    {   // remove the old entries, if there
        _ARPRemoveCacheEntries(pArpDcpt);
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
        if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN)
        {   // interface going down
            if(arpMod.deleteOld)
            {
                ARP_CACHE_DCPT* pArpDcpt;    
#if (TCPIP_ARP_PRIMARY_CACHE_ONLY != 0)
                pArpDcpt = _TCPIPStackNetIsPrimary(stackCtrl->pNetIf) ? _ARPGetIfDcpt(stackCtrl->pNetIf) : 0;
#else
                pArpDcpt = _ARPGetIfDcpt(stackCtrl->pNetIf);    
#endif // (TCPIP_ARP_PRIMARY_CACHE_ONLY != 0)
                if(pArpDcpt != 0)
                {
                    _ARPRemoveCacheEntries(pArpDcpt);
                }
            }
        }
        else
        {   // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
            // stack shut down
            if(--arpMod.initCount == 0)
            {   // all closed
                // release resources
                // if(arpMod.deleteOld)
                // Note: ignored, always clean up at stack shut down
                {
                    _ARPDeleteResources();
                }
            }
        }
    }

}

static void _ARPDeleteResources(void)
{

    if(arpMod.arpCacheDcpt)
    {
        int ix;
        ARP_CACHE_DCPT* pArpDcpt;

        for(ix = 0, pArpDcpt = arpMod.arpCacheDcpt; ix < arpMod.nIfs; ix++, pArpDcpt++)
        {
            _ARPDeleteCache(pArpDcpt);
        }

        TCPIP_HEAP_Free(arpMod.memH, arpMod.arpCacheDcpt);
        arpMod.arpCacheDcpt = 0;
    }

    _ARPDeleteClients();
    arpMod.memH = 0;

    if(arpMod.timerHandle)
    {
        _TCPIPStackSignalHandlerDeregister(arpMod.timerHandle);
        arpMod.timerHandle = 0;
    }

    if(arpMod.pMacPkt)
    {
        if((arpMod.pMacPkt->pktFlags & TCPIP_MAC_PKT_FLAG_QUEUED) == 0 )
        {
            TCPIP_PKT_PacketFree(arpMod.pMacPkt);
        }
        arpMod.pMacPkt = 0;
    }

}

static void _ARPDeleteCache(ARP_CACHE_DCPT* pArpDcpt)
{

    if(pArpDcpt->hashDcpt)
    {
        TCPIP_OAHASH_EntriesRemoveAll(pArpDcpt->hashDcpt);
        TCPIP_Helper_ProtectedSingleListDeinitialize(&pArpDcpt->incompleteList);
        TCPIP_Helper_ProtectedSingleListDeinitialize(&pArpDcpt->completeList);
        TCPIP_Helper_ProtectedSingleListDeinitialize(&pArpDcpt->permList);
        
        TCPIP_HEAP_Free(arpMod.memH, pArpDcpt->hashDcpt);
        pArpDcpt->hashDcpt = 0;
    }

}

static void _ARPDeleteClients(void)
{
    TCPIP_Notification_Deinitialize(&arpMod.registeredUsers, arpMod.memH);
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static TCPIP_MAC_PACKET* _ARPAllocateTxPacket(void)
{
    TCPIP_MAC_PACKET*   pPkt;

    // allocate TCPIP_MAC_PACKET packet
    pPkt = TCPIP_PKT_PacketAlloc(sizeof(TCPIP_MAC_PACKET), sizeof(ARP_PACKET), TCPIP_MAC_PKT_FLAG_ARP | TCPIP_MAC_PKT_FLAG_TX);

    if(pPkt)
    {
        TCPIP_PKT_PacketAcknowledgeSet(pPkt, _ARPTxAckFnc, 0);
    }


    return pPkt;
}


static bool _ARPTxAckFnc (TCPIP_MAC_PACKET * pPkt, const void * param)
{
    if(arpMod.pMacPkt != pPkt)
    {   // another one allocated
        TCPIP_PKT_PacketFree(pPkt);
        return false;
    }
    // else we should be OK
    return true;
}



TCPIP_ARP_HANDLE TCPIP_ARP_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_ARP_EVENT_HANDLER handler, const void* hParam)
{
    if(handler && arpMod.memH)
    {
        ARP_LIST_NODE arpNode;
        arpNode.handler = handler;
        arpNode.hParam = hParam;
        arpNode.hNet = hNet;

        return (ARP_LIST_NODE*)TCPIP_Notification_Add(&arpMod.registeredUsers, arpMod.memH, &arpNode, sizeof(arpNode));
    }

    return 0;

}

// deregister the event handler
bool TCPIP_ARP_HandlerDeRegister(TCPIP_ARP_HANDLE hArp)
{
    if(hArp && arpMod.memH)
    {
        if(TCPIP_Notification_Remove((SGL_LIST_NODE*)hArp, &arpMod.registeredUsers, arpMod.memH))
        {
            return true;
        }
    }

    return false;
}

static void _ARPNotifyClients(TCPIP_NET_IF* pNetIf, const IPV4_ADDR* ipAdd, const TCPIP_MAC_ADDR* MACAddr, TCPIP_ARP_EVENT_TYPE evType)
{
    ARP_LIST_NODE* aNode;

    TCPIP_Notification_Lock(&arpMod.registeredUsers);
    for(aNode = (ARP_LIST_NODE*)arpMod.registeredUsers.list.head; aNode != 0; aNode = aNode->next)
    {
        if(aNode->hNet == 0 || aNode->hNet == pNetIf)
        {   // trigger event
            (*aNode->handler)(pNetIf, ipAdd, MACAddr, evType, aNode->hParam);
        }
    }
    TCPIP_Notification_Unlock(&arpMod.registeredUsers);
    
}


// called after service needed reported
// maintain the queues, processes, etc.

void TCPIP_ARP_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

#if (_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)
    int netIx, nIfs;
    TCPIP_NET_IF *pIf;
    uint32_t  signalParam;

    sigPend = _TCPIPStackModuleSignalParamGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL, &signalParam);

    if((sigPend & (TCPIP_MODULE_SIGNAL_INTERFACE_CHANGE)) != 0)
    { // interface address change occurred
        nIfs = TCPIP_STACK_NumberOfNetworksGet();
        for(netIx = 0; netIx < nIfs; netIx++)
        {
           if((signalParam & (1 << netIx)) != 0) 
           {
               pIf = (TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(netIx);
               // gratuitous PROBE_ONLY
               TCPIP_ARP_Probe(pIf, &pIf->netIPAddr, &pIf->netIPAddr, ARP_OPERATION_RESP | ARP_OPERATION_CONFIGURE | ARP_OPERATION_PROBE_ONLY);
           }
        }
    }
#else
    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);
#endif  // (_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)
    
    if((sigPend & TCPIP_MODULE_SIGNAL_RX_PENDING) != 0)
    { //  RX signal occurred
        TCPIP_ARP_Process();
    }

    if((sigPend & TCPIP_MODULE_SIGNAL_TMO) != 0)
    { // regular TMO occurred
        TCPIP_ARP_Timeout();
    }

}



static void TCPIP_ARP_Timeout(void)
{
    int netIx, purgeIx;
    ARP_HASH_ENTRY  *pE;
    ARP_CACHE_DCPT  *pArpDcpt;
    SGL_LIST_NODE   *pN;
    TCPIP_NET_IF *pIf;
    int         nArpIfs;
    bool        isConfig;
    uint16_t    maxRetries;


    arpMod.timeMs += TCPIP_ARP_TASK_PROCESS_RATE;
    arpMod.timeSeconds = arpMod.timeMs / 1000;

    nArpIfs = TCPIP_STACK_NumberOfNetworksGet();
    pArpDcpt = arpMod.arpCacheDcpt;
    for(netIx = 0; netIx < nArpIfs; netIx++)
    {
        pIf = (TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(netIx);
#if (TCPIP_ARP_PRIMARY_CACHE_ONLY != 0)
        if(!_TCPIPStackNetIsPrimary(pIf))
        {
            continue;
        }
#endif // (TCPIP_ARP_PRIMARY_CACHE_ONLY != 0)

        // process the incomplete queue
        // see if there's something to remove
        while( (pN = pArpDcpt->incompleteList.list.head) != 0)
        {
            pE = (ARP_HASH_ENTRY*) ((uint8_t*)pN - offsetof(ARP_HASH_ENTRY, next));
            if( (arpMod.timeSeconds - pE->tInsert) >= arpMod.entryPendingTmo)
            {   // expired, remove it
                TCPIP_OAHASH_EntryRemove(pArpDcpt->hashDcpt, &pE->hEntry);
                TCPIP_Helper_ProtectedSingleListHeadRemove(&pArpDcpt->incompleteList);
                _ARPNotifyClients(pIf, &pE->ipAddress, 0, ARP_EVENT_REMOVED_TMO);
            }
            else
            {   // this list is ordered, we can safely break out
                break;
            }
        }

        // see if we have to query again
        isConfig = _TCPIPStackIsConfig(pIf);

        for(pN = pArpDcpt->incompleteList.list.head; pN != 0; pN = pN->next)
        {
            pE = (ARP_HASH_ENTRY*) ((uint8_t*)pN - offsetof(ARP_HASH_ENTRY, next));
            if((pE->hEntry.flags.value & ARP_FLAG_ENTRY_GRATUITOUS) != 0)
            {
                maxRetries = arpMod.entryGratRetries;
            }
            else
            {
                maxRetries = arpMod.entryRetries;
            }
            if( pE->nRetries < maxRetries && (arpMod.timeSeconds - pE->tInsert) >= pE->nRetries * arpMod.entryRetryTmo)
            {   // expired, retry it
                if(isConfig == false || (pE->hEntry.flags.value & ARP_FLAG_ENTRY_CONFIGURE) != 0 )
                {
                    _ARPSendIfPkt(pIf, ARP_OPERATION_REQ, (uint32_t)pIf->netIPAddr.Val, pE->ipAddress.Val, &arpBcastAdd);
                    pE->nRetries++;
                }
            }
        }

        // see the completed entries queue
        while( (pN = pArpDcpt->completeList.list.head) != 0)
        {
            pE = (ARP_HASH_ENTRY*) ((uint8_t*)pN - offsetof(ARP_HASH_ENTRY, next));
            if( (arpMod.timeSeconds - pE->tInsert) >= arpMod.entrySolvedTmo)
            {   // expired, remove it
                TCPIP_OAHASH_EntryRemove(pArpDcpt->hashDcpt, &pE->hEntry);
                TCPIP_Helper_ProtectedSingleListHeadRemove(&pArpDcpt->completeList);
                _ARPNotifyClients(pIf, &pE->ipAddress, 0, ARP_EVENT_REMOVED_EXPIRED);
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
                pN = TCPIP_Helper_ProtectedSingleListHeadRemove(&pArpDcpt->completeList);
                if(pN)
                {
                    pE = (ARP_HASH_ENTRY*) ((uint8_t*)pN - offsetof(ARP_HASH_ENTRY, next));
                    TCPIP_OAHASH_EntryRemove(pArpDcpt->hashDcpt, &pE->hEntry);
                    _ARPNotifyClients(pIf, &pE->ipAddress, 0, ARP_EVENT_REMOVED_PURGED);
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
    TCPIP_NET_IF* pIf;
    TCPIP_MAC_PACKET* pPkt;

    ARP_PACKET      *pArpPkt;
    TCPIP_MAC_ADDR        *dstMAC; 
    OA_HASH_ENTRY   *hE;
    ARP_CACHE_DCPT  *pArpDcpt;
    TCPIP_MAC_PKT_ACK_RES ackRes;
    TCPIP_ARP_RESULT arpReqRes;
    IPV4_ADDR targetAdd;
    IPV4_ADDR alignedIpV4Addr;


    // extract queued ARP packets
    while((pPkt = _TCPIPStackModuleRxExtract(TCPIP_THIS_MODULE_ID)) != 0)
    {
        TCPIP_PKT_FlightLogRx(pPkt, TCPIP_THIS_MODULE_ID);
        arpReqRes = ARP_RES_OK;

        // Obtain the incoming ARP packet and process
        pArpPkt = (ARP_PACKET*)pPkt->pNetLayer;
        _SwapARPPacket(pArpPkt);

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
            pPkt->pktIf = pIf = _TCPIPStackMapAliasInterface((TCPIP_NET_IF*)pPkt->pktIf, &targetAdd);
            pArpDcpt = _ARPGetIfDcpt(pIf);

#ifdef TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL
            _ARPProcessRxPkt(pIf, pArpPkt);
#endif

            // Handle incoming ARP packet
            alignedIpV4Addr.Val = pArpPkt->SenderIPAddr.Val;
            hE = TCPIP_OAHASH_EntryLookup(pArpDcpt->hashDcpt, &alignedIpV4Addr.Val);
            if(hE != 0)
            {   // we already have this sender and we should update it
                _ARPUpdateEntry(pIf, (ARP_HASH_ENTRY*)hE, &pArpPkt->SenderMACAddr);
            }

            while(!_TCPIPStackIsConfig(pIf) && (targetAdd.Val == pIf->netIPAddr.Val))
            {   // we are the target and we should add to cache anyway
                if(hE == 0)
                {   // not there yet
                    alignedIpV4Addr.Val = pArpPkt->SenderIPAddr.Val;
                    arpReqRes = _ARPAddCompleteEntry(pIf, &alignedIpV4Addr, &pArpPkt->SenderMACAddr);
                }

                // Handle incoming ARP operation
                if(pArpPkt->Operation == ARP_OPERATION_REQ)
                {   
                    // ARP packet asking for this host IP address 
#ifdef TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL
                    /* Fix for Loop-Back suppression:
                     * For ZCLL-Claim packets, host should not respond.
                     * Check Sender's MAC-address with own MAC-address and 
                     * if it is matched, response will not be sent back. This
                     * was leading to flooding of ARP-answeres */
                    if(!memcmp (&pArpPkt->SenderMACAddr, &pIf->netMACAddr, 6))
                    {
                        SYS_CONSOLE_MESSAGE("Loopback answer suppressed \r\n");
                        break;
                    }
#endif

                    // Need to send a reply to the requestor 
                    dstMAC = &pArpPkt->SenderMACAddr;
                    // Send an ARP response to the received request
                    if(!_ARPSendIfPkt(pIf, ARP_OPERATION_RESP, (uint32_t)pIf->netIPAddr.Val, (uint32_t)pArpPkt->SenderIPAddr.Val, dstMAC))
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
TCPIP_ARP_RESULT TCPIP_ARP_Resolve(TCPIP_NET_HANDLE hNet, IPV4_ADDR* IPAddr)
{
    TCPIP_NET_IF *pIf;
   
    if(IPAddr == 0 || IPAddr->Val == 0)
    {   // do not store 0's in cache
        return ARP_RES_BAD_ADDRESS;
    }
    
    pIf = _TCPIPStackHandleToNetLinked(hNet);
    if(pIf == 0)
    {
        return ARP_RES_NO_INTERFACE;
    }

    if(_TCPIPStackIsConfig(pIf))
    {   // no ARP probes during configuration
        return ARP_RES_CONFIGURE_ERR;
    }

    return _ARPProbeAddress(pIf, IPAddr, &pIf->netIPAddr, ARP_OPERATION_REQ, 0);
}

TCPIP_ARP_RESULT TCPIP_ARP_Probe(TCPIP_NET_HANDLE hNet, IPV4_ADDR* IPAddr, IPV4_ADDR* srcAddr, TCPIP_ARP_OPERATION_TYPE opType)
{
    TCPIP_NET_IF *pIf;

    if(IPAddr == 0 || IPAddr->Val == 0 || srcAddr == 0)
    {   // do not store 0's in cache
        return ARP_RES_BAD_ADDRESS;
    }

    pIf =_TCPIPStackHandleToNetLinked(hNet);
    if(pIf == 0)
    {
        return ARP_RES_NO_INTERFACE;
    }


    if(_TCPIPStackIsConfig(pIf) && (opType & ARP_OPERATION_CONFIGURE) == 0)
    {   // no ARP probes during configuration
        return ARP_RES_CONFIGURE_ERR;
    }

    return _ARPProbeAddress(pIf, IPAddr, srcAddr, opType, 0);
}


static TCPIP_ARP_RESULT _ARPProbeAddress(TCPIP_NET_IF* pIf, IPV4_ADDR* IPAddr, IPV4_ADDR* srcAddr, TCPIP_ARP_OPERATION_TYPE opType, TCPIP_MAC_ADDR* pHwAdd)
{
    ARP_CACHE_DCPT  *pArpDcpt;
    OA_HASH_ENTRY   *hE;
   
     
    if((opType & ARP_OPERATION_PROBE_ONLY) != 0)
    {   // just send an ARP probe
        return _ARPSendIfPkt(pIf, (opType & ARP_OPERATION_MASK), (uint32_t)srcAddr->Val, (uint32_t)IPAddr->Val, &arpBcastAdd) ? ARP_RES_PROBE_OK : ARP_RES_PROBE_FAILED;
    }

    pArpDcpt = _ARPGetIfDcpt(pIf);

    hE = TCPIP_OAHASH_EntryLookupOrInsert(pArpDcpt->hashDcpt, &IPAddr->Val);
    if(hE == 0)
    {   // oops!
        return ARP_RES_CACHE_FULL;
    }
        
    if(hE->flags.newEntry != 0)
    {   // new entry; add it to the not done list 
        ARP_ENTRY_FLAGS newFlags = (opType & ARP_OPERATION_CONFIGURE) != 0 ? ARP_FLAG_ENTRY_CONFIGURE : 0;
        if((opType & ARP_OPERATION_GRATUITOUS) != 0) 
        {
            newFlags |= ARP_FLAG_ENTRY_GRATUITOUS;
        }
        _ARPSetEntry((ARP_HASH_ENTRY*)hE, newFlags, 0, &pArpDcpt->incompleteList);

        // initiate an ARP request operation
        _ARPSendIfPkt(pIf, (opType & ARP_OPERATION_MASK), (uint32_t)srcAddr->Val, ((ARP_HASH_ENTRY*)hE)->ipAddress.Val, &arpBcastAdd);
        return ARP_RES_ENTRY_NEW;
    }
    // else, even if it is not complete, TCPIP_ARP_Task will initiate retransmission
    // Normally if the entry is existent, it should be refreshed, since it's obviously needed.
    // However, the TCPIP_ARP_IsResolved() will do it, because that's the call that actually uses the entry!
    if((hE->flags.value & ARP_FLAG_ENTRY_VALID_MASK) != 0)
    {   // found address in cache
        ARP_HASH_ENTRY  *arpHE = (ARP_HASH_ENTRY*)hE;
        if(pHwAdd)
        {
            *pHwAdd = arpHE->hwAdd;
        }
        if((hE->flags.value & ARP_FLAG_ENTRY_COMPLETE) != 0 )
        {   // an existent entry, re-used, gets refreshed
            _ARPRefreshEntry(arpHE, &pArpDcpt->completeList);
        }
        return ARP_RES_ENTRY_SOLVED;
    }
    
    // incomplete
    return ARP_RES_ENTRY_QUEUED;


}

bool TCPIP_ARP_IsResolved(TCPIP_NET_HANDLE hNet, IPV4_ADDR* IPAddr, TCPIP_MAC_ADDR* MACAddr)
{
    OA_HASH_ENTRY   *hE;
    ARP_CACHE_DCPT  *pArpDcpt;
    TCPIP_NET_IF  *pIf;

    if(IPAddr == 0 || IPAddr->Val == 0)
    {   
        return ARP_RES_BAD_ADDRESS;
    }

    pIf = _TCPIPStackHandleToNetUp(hNet);
    if(!pIf)
    {
        return ARP_RES_NO_INTERFACE;
    }
    

    pArpDcpt = _ARPGetIfDcpt(pIf);
    
    hE = TCPIP_OAHASH_EntryLookup(pArpDcpt->hashDcpt, &IPAddr->Val);
    if(hE != 0 && (hE->flags.value & ARP_FLAG_ENTRY_VALID_MASK) != 0 )
    {   // found address in cache
        ARP_HASH_ENTRY  *arpHE = (ARP_HASH_ENTRY*)hE;
        if(MACAddr)
        {
            *MACAddr = arpHE->hwAdd;
        }
        if((hE->flags.value & ARP_FLAG_ENTRY_COMPLETE) != 0 )
        {   // an existent entry, re-used, gets refreshed
            _ARPRefreshEntry(arpHE, &pArpDcpt->completeList);
        }
        return true;
    }
    
    return false;
    
}



/*****************************************************************************
  Function:
    void _SwapARPPacket(ARP_PACKET* p)

  Description:
    Swaps endian-ness of header information in an ARP packet.

  Precondition:
    None

  Parameters:
    p - The ARP packet to be swapped

  Returns:
    None
  ***************************************************************************/
static void _SwapARPPacket(ARP_PACKET* p)
{
    p->HardwareType     = TCPIP_Helper_htons(p->HardwareType);
    p->Protocol         = TCPIP_Helper_htons(p->Protocol);
    p->Operation        = TCPIP_Helper_htons(p->Operation);
}

TCPIP_ARP_RESULT TCPIP_ARP_EntrySet(TCPIP_NET_HANDLE hNet, IPV4_ADDR* ipAdd, TCPIP_MAC_ADDR* hwAdd, bool perm)
{
    ARP_CACHE_DCPT  *pArpDcpt;
    ARP_HASH_ENTRY  *arpHE;
    OA_HASH_ENTRY   *hE;
    PROTECTED_SINGLE_LIST     *oldList, *newList;
    ARP_ENTRY_FLAGS newFlags;
    TCPIP_ARP_RESULT      res;
    TCPIP_NET_IF    *pIf;

    if(ipAdd == 0 || ipAdd->Val == 0)
    {   // do not store 0's in cache
        return ARP_RES_BAD_ADDRESS;
    }
    
    pIf = _TCPIPStackHandleToNetUp(hNet);
    if(!pIf)
    {
        return ARP_RES_NO_INTERFACE;
    }
    
    pArpDcpt = _ARPGetIfDcpt(pIf);

    hE = TCPIP_OAHASH_EntryLookupOrInsert(pArpDcpt->hashDcpt, &ipAdd->Val);
    if(hE == 0)
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
    
    arpHE = (ARP_HASH_ENTRY*)hE;
   
    if(hE->flags.newEntry == 0)
    {   // existent entry
        if( (hE->flags.value & ARP_FLAG_ENTRY_PERM) != 0 )
        {
            oldList =  &pArpDcpt->permList;
        }
        else if( (hE->flags.value & ARP_FLAG_ENTRY_COMPLETE) != 0 )
        {
            oldList =  &pArpDcpt->completeList;
        }
        else
        {
            oldList =  &pArpDcpt->incompleteList;
        }

        if(newList != oldList)
        {   // remove from the old list
            TCPIP_Helper_ProtectedSingleListNodeRemove(oldList, (SGL_LIST_NODE*)&arpHE->next);
        }
        res = ARP_RES_ENTRY_EXIST;
    }
    else
    {
        res = ARP_RES_OK;
    }
    
    // add it to where it belongs
    _ARPSetEntry(arpHE, newFlags, hwAdd, newList);

    if(TCPIP_Helper_ProtectedSingleListCount(&pArpDcpt->permList) >= (arpMod.permQuota * pArpDcpt->hashDcpt->fullSlots)/100)
    {   // quota exceeded
        res = ARP_RES_PERM_QUOTA_EXCEED;
    }

    return res;
}

TCPIP_ARP_RESULT TCPIP_ARP_EntryGet(TCPIP_NET_HANDLE hNet, IPV4_ADDR* ipAdd, TCPIP_MAC_ADDR* pHwAdd, bool probe)
{   
    TCPIP_NET_IF  *pIf;

    if(ipAdd == 0 || ipAdd->Val == 0)
    {
        return ARP_RES_BAD_ADDRESS;
    }

    pIf = _TCPIPStackHandleToNetUp(hNet);
    if(pIf == 0)
    {
        return ARP_RES_NO_INTERFACE;
    }

    if(probe)
    {
        if(_TCPIPStackIsConfig(pIf))
        {   // no ARP probes during configuration
            return ARP_RES_CONFIGURE_ERR;
        }
        return _ARPProbeAddress(pIf, ipAdd, &pIf->netIPAddr, ARP_OPERATION_REQ, pHwAdd);
    }
    else
    {
        return (TCPIP_ARP_IsResolved(pIf, ipAdd, pHwAdd))? ARP_RES_OK : ARP_RES_NO_ENTRY;
    }

}

TCPIP_ARP_RESULT TCPIP_ARP_EntryRemove(TCPIP_NET_HANDLE hNet,  IPV4_ADDR* ipAdd)
{
    OA_HASH_ENTRY   *hE;
    ARP_CACHE_DCPT  *pArpDcpt;
    TCPIP_NET_IF  *pIf;

    if(ipAdd == 0 || ipAdd->Val == 0)
    {
        return ARP_RES_BAD_ADDRESS;
    }

    pIf = _TCPIPStackHandleToNetUp(hNet);
    if(!pIf)
    {
        return ARP_RES_NO_INTERFACE;
    }
    

    pArpDcpt = _ARPGetIfDcpt(pIf);

    hE = TCPIP_OAHASH_EntryLookup(pArpDcpt->hashDcpt, &ipAdd->Val);

    if(hE == 0)
    {
        return ARP_RES_NO_ENTRY;
    }

    _ARPRemoveEntry(pArpDcpt, hE);
    _ARPNotifyClients(pIf, &((ARP_HASH_ENTRY*)hE)->ipAddress, 0, ARP_EVENT_REMOVED_USER);
    
    return ARP_RES_OK;
}


TCPIP_ARP_RESULT TCPIP_ARP_EntryRemoveNet(TCPIP_NET_HANDLE hNet, IPV4_ADDR* ipAdd, IPV4_ADDR* mask , TCPIP_ARP_ENTRY_TYPE type)
{
    OA_HASH_ENTRY   *hE;
    ARP_HASH_ENTRY  *arpHE;
    ARP_CACHE_DCPT  *pArpDcpt;
    OA_HASH_DCPT    *pOH;
    TCPIP_NET_IF    *pIf;
    int             index;
    uint16_t        andFlags, resFlags;
    uint32_t        matchAdd;

    if(ipAdd == 0 || ipAdd->Val == 0)
    {
        return ARP_RES_BAD_ADDRESS;
    }

    pIf = _TCPIPStackHandleToNetUp(hNet);
    if(!pIf)
    {
        return ARP_RES_NO_INTERFACE;
    }

    switch (type)
    {
        case ARP_ENTRY_TYPE_PERMANENT:
            andFlags = resFlags = ARP_FLAG_ENTRY_PERM;
            break;

        case ARP_ENTRY_TYPE_COMPLETE:
            andFlags = resFlags =  ARP_FLAG_ENTRY_COMPLETE;
            break;
            
        case ARP_ENTRY_TYPE_INCOMPLETE:
            andFlags = (ARP_FLAG_ENTRY_PERM | ARP_FLAG_ENTRY_COMPLETE);
            resFlags = 0;
            break;
            
        case ARP_ENTRY_TYPE_ANY:
            andFlags = resFlags = 0;
            break;

        default:
            return ARP_RES_BAD_TYPE;
    }


    pArpDcpt = _ARPGetIfDcpt(pIf);
    pOH = pArpDcpt->hashDcpt;
    matchAdd = ipAdd->Val & mask->Val;

    
    // scan all entries
    for(index = 0; index < pOH->hEntries; index++)
    {
        hE = TCPIP_OAHASH_EntryGet(pArpDcpt->hashDcpt, index);
        if(hE->flags.busy != 0)
        {
            if((hE->flags.value & andFlags) == resFlags)
            {   // flags match
                arpHE = (ARP_HASH_ENTRY*)hE;
                if((arpHE->ipAddress.Val & mask->Val) == matchAdd)
                {   // address match;  delete entry
                    _ARPRemoveEntry(pArpDcpt, hE);
                    _ARPNotifyClients(pIf, &arpHE->ipAddress, 0, ARP_EVENT_REMOVED_USER);
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
    int             index;


    pIf = _TCPIPStackHandleToNetUp(hNet);
    if(!pIf)
    {
        return ARP_RES_NO_INTERFACE;
    }

    pArpDcpt = _ARPGetIfDcpt(pIf);
    pOH = pArpDcpt->hashDcpt;

    // scan all entries so we can notify that they are removed
    for(index = 0; index < pOH->hEntries; index++)
    {
        hE = TCPIP_OAHASH_EntryGet(pArpDcpt->hashDcpt, index);
        if(hE->flags.busy != 0)
        {
            arpHE = (ARP_HASH_ENTRY*)hE;
            // delete entry
            _ARPRemoveEntry(pArpDcpt, hE);
            _ARPNotifyClients(pIf, &arpHE->ipAddress, 0, ARP_EVENT_REMOVED_USER);
        }
    }



    return ARP_RES_OK;
}

TCPIP_ARP_RESULT TCPIP_ARP_EntryQuery(TCPIP_NET_HANDLE hNet, size_t index, TCPIP_ARP_ENTRY_QUERY* pArpQuery)
{
    OA_HASH_ENTRY   *hE;
    ARP_HASH_ENTRY  *arpHE;
    ARP_CACHE_DCPT  *pArpDcpt;
    TCPIP_MAC_ADDR        noHwAdd = {{0}};
    TCPIP_NET_IF  *pIf;

    pIf = _TCPIPStackHandleToNetUp(hNet);
    if(!pIf)
    {
        return ARP_RES_NO_INTERFACE;
    }

    pArpDcpt = _ARPGetIfDcpt(pIf);
    hE = TCPIP_OAHASH_EntryGet(pArpDcpt->hashDcpt, index);
    

    if(hE == 0)
    {
        return ARP_RES_BAD_INDEX;
    }
    
    arpHE = (ARP_HASH_ENTRY*)hE;

    if(pArpQuery)
    {
        pArpQuery->entryIpAdd.Val = 0;
        pArpQuery->entryHwAdd = noHwAdd;
        
        if(hE->flags.busy == 0)
        {
            pArpQuery->entryType = ARP_ENTRY_TYPE_INVALID;
        }
        else if((hE->flags.value & ARP_FLAG_ENTRY_VALID_MASK) != 0)
        {
            pArpQuery->entryType = ((hE->flags.value & ARP_FLAG_ENTRY_PERM) != 0)?
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
    
    pIf = _TCPIPStackHandleToNetUp(hNet);
    if(!pIf)
    {
        return 0;
    }
    
    ARP_CACHE_DCPT  *pArpDcpt = _ARPGetIfDcpt(pIf);
    OA_HASH_DCPT    *pOH = pArpDcpt->hashDcpt;

    switch(type)
    {
        case ARP_ENTRY_TYPE_INVALID:
           return pOH->hEntries - pOH->fullSlots;

        case ARP_ENTRY_TYPE_PERMANENT:
           return TCPIP_Helper_ProtectedSingleListCount(&pArpDcpt->permList);

        case ARP_ENTRY_TYPE_COMPLETE:
           return TCPIP_Helper_ProtectedSingleListCount(&pArpDcpt->completeList);

        case ARP_ENTRY_TYPE_INCOMPLETE:
           return TCPIP_Helper_ProtectedSingleListCount(&pArpDcpt->incompleteList);

        case ARP_ENTRY_TYPE_ANY:
           return pOH->fullSlots;

        default:    // case ARP_ENTRY_TYPE_TOTAL:
           return pOH->hEntries;
    }

}

TCPIP_ARP_RESULT TCPIP_ARP_CacheThresholdSet(TCPIP_NET_HANDLE hNet, int purgeThres, int purgeEntries)
{
    TCPIP_NET_IF  *pIf;

    pIf = _TCPIPStackHandleToNetUp(hNet);
    if(!pIf)
    {
        return ARP_RES_NO_INTERFACE;
    }
    
    ARP_CACHE_DCPT  *pArpDcpt = _ARPGetIfDcpt(pIf);

    pArpDcpt->purgeThres = (purgeThres * pArpDcpt->hashDcpt->hEntries + 99)/100;
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
    SINGLE_LIST     *pRemList = 0;    
    
    pArpDcpt = (ARP_CACHE_DCPT*)pOH->hParam;

    if( (pN = pArpDcpt->incompleteList.head) != 0)
    {
        pE = (ARP_HASH_ENTRY*) ((uint8_t*)pN - offsetof(ARP_HASH_ENTRY, next));
        if( (arpMod.timeSeconds - pE->tInsert) >= arpMod.entryPendingTmo)
        {   // we remove this one
            pRemList = &pArpDcpt->incompleteList;
        }
    }

    if(pRemList == 0)
    {   // no luck with the incomplete list; use the complete one
            pRemList = &pArpDcpt->completeList;
    }

    pN = TCPIP_Helper_SingleListHeadRemove(pRemList);

    if(pN)
    {
        pE = (ARP_HASH_ENTRY*) ((uint8_t*)pN - offsetof(ARP_HASH_ENTRY, next));
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
    PROTECTED_SINGLE_LIST     *pRemList = 0;
    
    pArpDcpt = (ARP_CACHE_DCPT*)pOH->hParam;

    if( (pN = pArpDcpt->incompleteList.list.head) != 0)
    {
        pE = (ARP_HASH_ENTRY*) ((uint8_t*)pN - offsetof(ARP_HASH_ENTRY, next));
        if( (arpMod.timeSeconds - pE->tInsert) >= arpMod.entryPendingTmo)
        {   // we remove this one
            pRemList = &pArpDcpt->incompleteList;
        }
    }

    if(pRemList == 0)
    {   // no luck with the incomplete list; use the complete one
            pRemList = &pArpDcpt->completeList;
    }

    pN = TCPIP_Helper_ProtectedSingleListHeadRemove(pRemList);

    if(pN)
    {
        pE = (ARP_HASH_ENTRY*) ((uint8_t*)pN - offsetof(ARP_HASH_ENTRY, next));
        return &pE->hEntry;    
    }

    // it's possible to be unable to make room in the cache
    // for example, too many permanent entries added...
                   
    return 0;
}


int TCPIP_ARP_HashKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key)
{
    return ((ARP_HASH_ENTRY*)hEntry)->ipAddress.Val != ((ARP_UNALIGNED_KEY*)key)->v;
}

void TCPIP_ARP_HashKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key)
{

    ((ARP_HASH_ENTRY*)dstEntry)->ipAddress.Val = ((ARP_UNALIGNED_KEY*)key)->v;
}


#endif  // !defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )

#endif  // defined(TCPIP_STACK_USE_IPV4)


