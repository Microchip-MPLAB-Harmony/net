/*******************************************************************************
  Domain Name System (DNS) Client 
  Module for Microchip TCP/IP Stack

  Summary:
    DNS client implementation file
    
  Description:
    This source file contains the functions of the 
    DNS client routines
    
    Provides  hostname to IP address translation
    Reference: RFC 1035
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

#include "tcpip/src/tcpip_private.h"

<#if (tcpipDns.TCPIP_USE_DNS_CLIENT)?has_content && (tcpipDns.TCPIP_USE_DNS_CLIENT) == true>

#include "tcpip/src/dns_private.h"
#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_DNS_CLIENT

/****************************************************************************
  Section:
    Constants and Global Variables
  ***************************************************************************/


static TCPIP_DNS_DCPT     gDnsDcpt;
static TCPIP_DNS_DCPT*    pgDnsDcpt = NULL;

static int          dnsInitCount = 0;       // module initialization count
/****************************************************************************
  Section:
    Function Prototypes
  ***************************************************************************/

static void                 F_DNSNotifyClients(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* pDnsHE, TCPIP_DNS_EVENT_TYPE evType);
static void                 F_DNSPutString(uint8_t **wrPtr, const char* string);
static int                  F_DNS_ReadName(TCPIP_DNS_RR_PROCESS* pProc, char* nameBuff, size_t buffSize);
static int                  F_DNS_ProcessRR(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_RR_PROCESS* pProc, TCPIP_DNS_RR_TYPE rrType);
static void                 F_DNSInitRxData(TCPIP_DNS_RX_DATA* rxData, uint8_t* pBuff, uint16_t bufferSize);
static bool                 F_DNSGetData(TCPIP_DNS_RX_DATA* srcBuff, void *destBuff, size_t getBytes);
static bool                 F_DNS_SelectIntf(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* pDnsHE);
static bool                 F_DNS_Enable(TCPIP_NET_HANDLE hNet, bool checkIfUp, TCPIP_DNS_ENABLE_FLAGS flags);
static void                 F_DNS_DeleteHash(TCPIP_DNS_DCPT* pDnsDcpt);
static TCPIP_DNS_RESULT     F_DNS_Send_Query(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* pDnsHE);
static TCPIP_DNS_RESULT     F_DNS_Resolve(const char* hostName, TCPIP_DNS_RESOLVE_TYPE type, bool forceQuery);
static bool                 F_DNS_ProcessPacket(TCPIP_DNS_DCPT* pDnsDcpt);
static  TCPIP_DNS_RESULT    F_DNSCompleteHashEntry(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* dnsHE);
static  void                F_DNS_CleanCache(TCPIP_DNS_DCPT* pDnsDcpt);
static TCPIP_DNS_RESULT     F_DNS_IsNameResolved(const char* hostName, IPV4_ADDR* hostIPv4, IPV6_ADDR* hostIPv6, bool singleAddress);
static bool                 F_DNS_ValidateIf(TCPIP_DNS_DCPT* pDnsDcpt, const TCPIP_NET_IF* pIf, TCPIP_DNS_HASH_ENTRY* pDnsHE, bool wrapAround);
static bool                 F_DNS_AddSelectionIf(TCPIP_DNS_DCPT* pDnsDcpt, const TCPIP_NET_IF* pIf, const TCPIP_NET_IF* dnsIfTbl[], size_t tblEntries);
static bool                 F_DNS_NetIsValid(TCPIP_DNS_DCPT* pDnsDcpt, const TCPIP_NET_IF* pIf);

#if defined(TCPIP_STACK_USE_IPV4)
static bool                 F_DNS_ValidateIfIpv4(const TCPIP_NET_IF* pIf, TCPIP_DNS_HASH_ENTRY* pDnsHE, bool wrapAround);
#endif  // defined(TCPIP_STACK_USE_IPV4)

#if defined(TCPIP_STACK_USE_IPV6)
static bool                 F_DNS_ValidateIfIpv6(const TCPIP_NET_IF* pIf, TCPIP_DNS_HASH_ENTRY* pDnsHE, bool wrapAround);
static bool                 F_GetNetIPv6Address(TCPIP_DNS_HASH_ENTRY* pDnsHE, IPV6_ADDR_STRUCT* ip6AddStr);
#endif  // defined(TCPIP_STACK_USE_IPV6)
static void                 TCPIP_DNS_ClientProcess(bool isTmo);
static void                 TCPIP_DNS_CacheTimeout(TCPIP_DNS_DCPT* pDnsDcpt);
static void                 F_DNSSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param);
static bool                 F_DNSClientSocketOpen(TCPIP_DNS_DCPT* pDnsDcpt);
#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void                 F_DNSClientSocketClose(TCPIP_DNS_DCPT* pDnsDcpt);
static void                 F_DNSClientCleanup(TCPIP_DNS_DCPT* pDnsDcpt);
#else
#define F_DNSClientCleanup(pDnsDcpt)
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)
static TCPIP_DNS_HASH_ENTRY *F_DNSHashEntryFromTransactionId(TCPIP_DNS_DCPT* pDnsDcpt, const char* hostName, uint16_t transactionId);
static bool                 F_DNS_RESPONSE_HashEntryUpdate(TCPIP_DNS_RX_DATA* dnsRxData, TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* dnsHE);
static size_t               F_DNS_GetAddresses(const char* hostName, size_t startIndex, IP_MULTI_ADDRESS pIPAddr[], size_t nIPAddresses, TCPIP_DNS_ADDRESS_REC_MASK recMask);

// conversion functions/helpers
//
static __inline__ TCPIP_DNS_HASH_ENTRY* __attribute__((always_inline)) FC_OaHash2DnsHash(OA_HASH_ENTRY* he)
{
    union
    {
        OA_HASH_ENTRY*    he;
        TCPIP_DNS_HASH_ENTRY* dhe;
    }U_OA_HASH_DNS_HASH;

    U_OA_HASH_DNS_HASH.he = he;
    return U_OA_HASH_DNS_HASH.dhe;
}

static __inline__ TCPIP_DNS_LIST_NODE* __attribute__((always_inline)) FC_Sgl2DnsNode(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*    node;
        TCPIP_DNS_LIST_NODE* dnsNode;
    }U_SGL_LIST_DNS_NODE;

    U_SGL_LIST_DNS_NODE.node = node;
    return U_SGL_LIST_DNS_NODE.dnsNode;
}

static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_DnsHndl2SglNode(TCPIP_DNS_HANDLE hDns)
{
    union
    {
        TCPIP_DNS_HANDLE hDns;
        SGL_LIST_NODE*    node;
    }U_SGL_LIST_DNS_NODE;

    U_SGL_LIST_DNS_NODE.hDns = hDns;
    return U_SGL_LIST_DNS_NODE.node;
}

#if ((TCPIP_DNS_DEBUG_LEVEL & TCPIP_DNS_DEBUG_MASK_EVENTS) != 0)
static const char* T_DNSDbg_EvNameTbl[] = 
{
    // general events
    "none",         // TCPIP_DNS_DBG_EVENT_NONE
    "query",        // TCPIP_DNS_DBG_EVENT_NAME_QUERY
    "solved",       // TCPIP_DNS_DBG_EVENT_NAME_RESOLVED
    "expired",      // TCPIP_DNS_DBG_EVENT_NAME_EXPIRED
    "removed",      // TCPIP_DNS_DBG_EVENT_NAME_REMOVED
    "name error",   // TCPIP_DNS_DBG_EVENT_NAME_ERROR
    "skt error",    // TCPIP_DNS_DBG_EVENT_SOCKET_ERROR
    "no if",        // TCPIP_DNS_DBG_EVENT_NO_INTERFACE
    // debug events
    "xtract error",     // TCPIP_DNS_DBG_EVENT_RR_XTRACT_ERROR 
    "str error",        // TCPIP_DNS_DBG_EVENT_RR_STRUCT_ERROR 
    "no RRs",           // TCPIP_DNS_DBG_EVENT_RR_NO_RECORDS 
    "rr miss",          // TCPIP_DNS_DBG_EVENT_RR_MISMATCH
    "rr data err",      // TCPIP_DNS_DBG_EVENT_RR_DATA_ERROR
    "rr complete err",  // TCPIP_DNS_DBG_EVENT_COMPLETE_ERROR
    "ip error",         // TCPIP_DNS_DBG_EVENT_NO_IP_ERROR 
    "unsolicited pkt",  // TCPIP_DNS_DBG_EVENT_UNSOLICITED_ERROR 
};

static void F_DNS_DbgEvent(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* pDnsHE, TCPIP_DNS_DBG_EVENT_TYPE evType)
{
    TCPIPStack_Assert(0 <= (int)evType && (int)evType <= (int)TCPIP_DNS_DBG_EVENT_UNSOLICITED_ERROR, __FILE__, __func__, __LINE__);
    const char* hostName;
    int ifIx;
    int srvIx;
    int retry;
    int nRetries;

    if(pDnsHE == NULL)
    {
        hostName = "no host";
        ifIx = -1;
        srvIx = -1;
        retry = -1;
        nRetries = -1;
    }
    else
    {
        hostName = pDnsHE->pHostName;
        ifIx = TCPIP_STACK_NetIxGet(pDnsHE->currNet);
        srvIx = (int)pDnsHE->currServerIx;
        retry = (int)pDnsHE->currRetry;
        nRetries = (int)pDnsHE->nRetries;
    }

    const char* evName = T_DNSDbg_EvNameTbl[evType];
    SYS_CONSOLE_PRINT("DNS Event: %s, host: %s, time: %d\r\n", evName, hostName, pDnsDcpt->dnsTime);
    SYS_CONSOLE_PRINT("ifIx: %d, srvIx: %d, retry: %d, nRetries: %d\r\n", ifIx, srvIx, retry, nRetries);
}
#else
#define F_DNS_DbgEvent(pDnsDcpt, pDnsHE, evType)
#endif  // ((TCPIP_DNS_DEBUG_LEVEL & TCPIP_DNS_DEBUG_MASK_EVENTS) != 0)

#if ((TCPIP_DNS_DEBUG_LEVEL & (TCPIP_DNS_DEBUG_MASK_ANSWER_NAMES | TCPIP_DNS_DEBUG_MASK_QUESTION_NAMES)) != 0)
static const char* rrNamesTbl[] = 
{
    "quest", "answer", "auth", "addit",
};

static void F_DNS_DbgRRName(char* nameBuffer, TCPIP_DNS_RR_TYPE rrType)
{
    const char* msg = NULL;
    if(rrType == TCPIP_DNS_RR_TYPE_QUESTION)
    {
#if ((TCPIP_DNS_DEBUG_LEVEL & TCPIP_DNS_DEBUG_MASK_QUESTION_NAMES) != 0)
        msg = rrNamesTbl[rrType];
#endif  // ((TCPIP_DNS_DEBUG_LEVEL & TCPIP_DNS_DEBUG_MASK_QUESTION_NAMES) != 0)
    }
    else
    {
#if ((TCPIP_DNS_DEBUG_LEVEL & TCPIP_DNS_DEBUG_MASK_ANSWER_NAMES) != 0)
        msg = rrNamesTbl[rrType];
#endif  // ((TCPIP_DNS_DEBUG_LEVEL & TCPIP_DNS_DEBUG_MASK_ANSWER_NAMES) != 0)
    }

    if(msg != NULL)
    {
        SYS_CONSOLE_PRINT("DNS RR name: %s, -%s-\r\n", msg, nameBuffer);
    }
}
#else
#define F_DNS_DbgRRName(nameBuffer, rrType)
#endif  // ((TCPIP_DNS_DEBUG_LEVEL & (TCPIP_DNS_DEBUG_MASK_ANSWER_NAMES | TCPIP_DNS_DEBUG_MASK_QUESTION_NAMES)) != 0)

#if ((TCPIP_DNS_DEBUG_LEVEL & TCPIP_DNS_DEBUG_MASK_VALIDATE) != 0)
static void F_DNS_DbgValidateIf(const TCPIP_NET_IF* pIf, size_t startIx, size_t selIx, bool isIpv6, bool success)
{
    const char* ipMsg = isIpv6 ? "IPv6" : "IPv4";
    SYS_CONSOLE_PRINT("DNS Validate - if: %d, startIx: %d, selIx: %d, %s, success: %d\r\n", pIf->netIfIx, startIx, selIx, ipMsg, success);
}
#else
#define F_DNS_DbgValidateIf(pIf, startIx, selIx, isIpv6, success)
#endif  // ((TCPIP_DNS_DEBUG_LEVEL & TCPIP_DNS_DEBUG_MASK_VALIDATE) != 0)

#if ((TCPIP_DNS_DEBUG_LEVEL & TCPIP_DNS_DEBUG_MASK_ARP_FLUSH) != 0)
static void F_DNS_DbgArpFlush(const TCPIP_NET_IF* oldIf, size_t oldIx, const TCPIP_NET_IF* newIf, size_t newIx, const IPV4_ADDR* dnsAdd)
{
    char addBuff[20];
    
    addBuff[0] = '\0';
    (void)TCPIP_Helper_IPAddressToString(dnsAdd, addBuff, sizeof(addBuff));
    
    SYS_CONSOLE_PRINT("DNS ARP flush - old If: %d, oldIx %d, new If: %d, newIx: %d, add: %s\r\n", TCPIP_STACK_NetIxGet(oldIf), oldIx, TCPIP_STACK_NetIxGet(newIf), newIx, addBuff); 
}
#else
#define F_DNS_DbgArpFlush(oldIf, oldIx, newIf, newIx, dnsAdd)
#endif  // ((TCPIP_DNS_DEBUG_LEVEL & TCPIP_DNS_DEBUG_MASK_ARP_FLUSH) != 0)
/*****************************************************************************
 swap DNS Header content packet . This API is used when we recive
 the DNS response for a query from the server.
  ***************************************************************************/
static void F_SwapDNSPacket(TCPIP_DNS_HEADER * p)
{
    p->TransactionID.Val = TCPIP_Helper_htons(p->TransactionID.Val);
    p->Flags.Val = TCPIP_Helper_htons(p->Flags.Val);
    p->AdditionalRecords.Val = TCPIP_Helper_htons(p->AdditionalRecords.Val);
    p->Answers.Val = TCPIP_Helper_htons(p->Answers.Val);
    p->AuthoritativeRecords.Val = TCPIP_Helper_htons(p->AuthoritativeRecords.Val);
    p->Questions.Val = TCPIP_Helper_htons(p->Questions.Val);
}

static void F_SwapDNSAnswerPacket(TCPIP_DNS_ANSWER_HEADER * p)
{
    p->ResponseClass.Val = TCPIP_Helper_htons(p->ResponseClass.Val);
    p->ResponseLen.Val = TCPIP_Helper_htons(p->ResponseLen.Val);
    p->ResponseTTL.Val = TCPIP_Helper_htonl(p->ResponseTTL.Val);
    p->ResponseType.Val = TCPIP_Helper_htons(p->ResponseType.Val);
}

static void F_DNS_CleanCacheEntry(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* pDnsHE)
{
    if(pDnsHE->hEntry.flags.busy != 0U)
    {
        if((pDnsHE->hEntry.flags.value & (uint16_t)TCPIP_DNS_FLAG_ENTRY_COMPLETE) == 0U)
        {   // deleting an unsolved entry
            pDnsDcpt->unsolvedEntries--;
            TCPIPStack_Assert(pDnsDcpt->unsolvedEntries >= 0, __FILE__, __func__, __LINE__);
        }
        TCPIP_OAHASH_EntryRemove(pDnsDcpt->hashDcpt, &pDnsHE->hEntry);
        pDnsHE->nIPv4Entries = pDnsHE->nIPv6Entries = 0U;
    }
}

static  void F_DNS_CleanCache(TCPIP_DNS_DCPT* pDnsDcpt)
{
    size_t          bktIx;
    TCPIP_DNS_HASH_ENTRY* pE;
    OA_HASH_DCPT*   pOh;
    
    if((pOh = pDnsDcpt->hashDcpt) != NULL)
    {
        for(bktIx = 0; bktIx < pOh->hEntries; bktIx++)
        {
            pE = FC_OaHash2DnsHash(TCPIP_OAHASH_EntryGet(pOh, bktIx));
            F_DNS_CleanCacheEntry(pDnsDcpt, pE);
        }
    }
}

static void F_DNS_UpdateExpiredHashEntry_Notify(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* pDnsHE)
{
    // Notify to to the specific Host name that it is expired.
    F_DNSNotifyClients(pDnsDcpt, pDnsHE, TCPIP_DNS_EVENT_NAME_EXPIRED);
    F_DNS_CleanCacheEntry(pDnsDcpt, pDnsHE);
}

static  TCPIP_DNS_RESULT  F_DNSCompleteHashEntry(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* dnsHE)
{
     
    dnsHE->hEntry.flags.value &= ~(uint16_t)TCPIP_DNS_FLAG_ENTRY_TIMEOUT;
    dnsHE->hEntry.flags.value |= (uint16_t)TCPIP_DNS_FLAG_ENTRY_COMPLETE;
    dnsHE->recordMask = (uint8_t)TCPIP_DNS_ADDRESS_REC_NONE;

    if(dnsHE->nIPv4Entries != 0U)
    {
        dnsHE->recordMask |= (uint8_t)TCPIP_DNS_ADDRESS_REC_IPV4;
    }
    if(dnsHE->nIPv6Entries != 0U)
    {
        dnsHE->recordMask |= (uint8_t)TCPIP_DNS_ADDRESS_REC_IPV6;
    }

    if(dnsHE->ipTTL.Val == 0U)
    {
        dnsHE->ipTTL.Val = (uint32_t)TCPIP_DNS_CLIENT_CACHE_DEFAULT_TTL_VAL;
    }
    dnsHE->tRetry = dnsHE->tInsert = pDnsDcpt->dnsTime; 
    pDnsDcpt->unsolvedEntries--;
    TCPIPStack_Assert(pDnsDcpt->unsolvedEntries >= 0, __FILE__, __func__, __LINE__);

    return TCPIP_DNS_RES_OK;
}

static  void F_DNSDeleteCacheEntries(TCPIP_DNS_DCPT* pDnsDcpt)
{
    size_t          bktIx;
    TCPIP_DNS_HASH_ENTRY* pE;
    OA_HASH_DCPT*   pOh;
    
    if((pOh = pDnsDcpt->hashDcpt) != NULL)
    {
        for(bktIx = 0; bktIx < pOh->hEntries; bktIx++)
        {
            pE = FC_OaHash2DnsHash(TCPIP_OAHASH_EntryGet(pOh, bktIx));
            TCPIP_OAHASH_EntryRemove(pOh, &pE->hEntry);
            (void)TCPIP_HEAP_Free(pDnsDcpt->memH, pE->memblk);
            (void)memset(pE, 0, sizeof(*pE));
        }
    }
}

// deletes the associated DNS client hash
static void F_DNS_DeleteHash(TCPIP_DNS_DCPT* pDnsDcpt)
{
    F_DNSDeleteCacheEntries(pDnsDcpt);
    (void)TCPIP_HEAP_Free(pDnsDcpt->memH, pDnsDcpt->hashDcpt);
    pDnsDcpt->hashDcpt = NULL;
    pDnsDcpt->unsolvedEntries = 0;
}

/****************************************************************************
  Section:
    Implementation
  ***************************************************************************/

bool TCPIP_DNS_ClientInitialize(const TCPIP_STACK_MODULE_CTRL* const stackData, const void* initData)
{
    OA_HASH_DCPT    *hashDcpt;
    size_t          hashMemSize;
    uint32_t        memoryBlockSize;
    size_t          hashCnt;
    uint8_t         *pMemoryBlock;
    OA_HASH_ENTRY   *pBkt;
    TCPIP_DNS_HASH_ENTRY  *pE;
    
    if(stackData->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        if(stackData->pNetIf->Flags.bIsDnsClientEnabled != 0U)
        {   // enable DNS client service
            (void)F_DNS_Enable(stackData->pNetIf, false, TCPIP_DNS_ENABLE_DEFAULT);
        }
        return true;
    }

    if(dnsInitCount == 0)
    {   // stack start up; initialize just once
        bool iniRes;
        TCPIP_DNS_DCPT* pDnsDcpt = &gDnsDcpt;
        (void)memset(pDnsDcpt, 0, sizeof(*pDnsDcpt));

        if(initData == NULL)
        {
            return false;
        }

#if !defined(TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN) || (TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN == 0)
        return false;
#endif

        const TCPIP_DNS_CLIENT_MODULE_CONFIG* dnsData = (const TCPIP_DNS_CLIENT_MODULE_CONFIG*)initData;
        // only IPv4 operation supported for now
#if !defined (TCPIP_STACK_USE_IPV4)
        if(dnsData->ipAddressType == IP_ADDRESS_TYPE_IPV4)
        {
            (void)F_DNSPutString;
            (void)F_DNS_SelectIntf;
            return false;
        }
#endif  // !defined (TCPIP_STACK_USE_IPV4)

#if !defined (TCPIP_STACK_USE_IPV6)
        if(dnsData->ipAddressType == IP_ADDRESS_TYPE_IPV6)
        {
            (void)F_DNSPutString;
            (void)F_DNS_SelectIntf;
            return false;
        }
#endif  // !defined (TCPIP_STACK_USE_IPV6)

        pDnsDcpt->memH = stackData->memH;
        hashMemSize = sizeof(OA_HASH_DCPT) + dnsData->cacheEntries * sizeof(TCPIP_DNS_HASH_ENTRY);
        hashDcpt = (OA_HASH_DCPT*)TCPIP_HEAP_Malloc(pDnsDcpt->memH, hashMemSize);
        if(hashDcpt == NULL)
        {   // failed
            return false;
        }  
        // populate the entries
        (void)memset(hashDcpt, 0, hashMemSize);
        hashDcpt->memBlk = hashDcpt + 1;
        hashDcpt->hParam = hashDcpt;    // store the descriptor it belongs to
        hashDcpt->hEntrySize = sizeof(TCPIP_DNS_HASH_ENTRY);
        hashDcpt->hEntries = dnsData->cacheEntries;
        hashDcpt->probeStep = TCPIP_DNS_HASH_PROBE_STEP;

        hashDcpt->hashF = &TCPIP_DNS_OAHASH_KeyHash;
        hashDcpt->delF = &TCPIP_DNS_OAHASH_DeleteEntry;
        hashDcpt->cmpF = &TCPIP_DNS_OAHASH_KeyCompare;
        hashDcpt->cpyF = &TCPIP_DNS_OAHASH_KeyCopy;
#if defined(OA_DOUBLE_HASH_PROBING)
        hashDcpt->probeHash = &TCPIP_DNS_OAHASH_ProbeHash;
#endif  // defined(OA_DOUBLE_HASH_PROBING)

        TCPIP_OAHASH_Initialize(hashDcpt);
        pDnsDcpt->hashDcpt = hashDcpt;
        pDnsDcpt->dnsSocket =  INVALID_UDP_SOCKET;
        pDnsDcpt->cacheEntryTMO = dnsData->entrySolvedTmo;
        pDnsDcpt->nIPv4Entries= dnsData->nIPv4Entries;
        pDnsDcpt->nIPv6Entries = dnsData->nIPv6Entries;
        pDnsDcpt->ipAddressType = dnsData->ipAddressType;

        // allocate memory for each DNS hostname , IPv4 address and IPv6 address
        // and the allocation will be done per Hash descriptor
        memoryBlockSize = pDnsDcpt->nIPv4Entries * sizeof(IPV4_ADDR)
            + pDnsDcpt->nIPv6Entries * sizeof(IPV6_ADDR)
            + (uint32_t)TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN ;
        for(hashCnt = 0; hashCnt < dnsData->cacheEntries; hashCnt++)
        {
            pBkt = TCPIP_OAHASH_EntryGet(hashDcpt, hashCnt);

            pE = FC_OaHash2DnsHash(pBkt);
            pMemoryBlock = (uint8_t*)TCPIP_HEAP_Malloc(pDnsDcpt->memH, memoryBlockSize);
            if((pE->memblk = pMemoryBlock) == NULL)
            {
                F_DNS_DeleteHash(pDnsDcpt);
                return false;
            }
            pE->pHostName = NULL;
            pE->pip4Address = NULL;
            pE->pip6Address = NULL;
            // set memory for IPv4 entries
            if(pDnsDcpt->nIPv4Entries != 0U)
            {
                pE->pip4Address = FC_U8Ptr2Ip4Add(pMemoryBlock);
                pMemoryBlock += pDnsDcpt->nIPv4Entries * (sizeof(IPV4_ADDR));
            }
            // set memory for IPv6 entries
            if(pDnsDcpt->nIPv6Entries != 0U)
            {
                pE->pip6Address = FC_U8Ptr2Ip6Add(pMemoryBlock);
                pMemoryBlock += pDnsDcpt->nIPv6Entries * (sizeof(IPV6_ADDR));
            }

            // allocate Hostname
            pE->pHostName = (char*)pMemoryBlock;
        }

        iniRes = false;
        while(true)
        {
#if (TCPIP_DNS_CLIENT_USER_NOTIFICATION != 0)
            if(TCPIP_Notification_Initialize(&pDnsDcpt->dnsRegisteredUsers) == false)
            {
                break;
            }
#endif  // (TCPIP_DNS_CLIENT_USER_NOTIFICATION != 0)

            if((pDnsDcpt->dnsSignalHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_DNS_ClientTask, TCPIP_DNS_CLIENT_TASK_PROCESS_RATE)) == NULL)
            {
                break;
            }
            // create the DNS socket
            if(!F_DNSClientSocketOpen(pDnsDcpt))
            {   // failed
                break;
            }
                
            // success
            iniRes = true;
            break;
        }        

        if(iniRes == false)
        {
            F_DNSClientCleanup(pDnsDcpt);
            return false;
        }

        // module is initialized and pgDnsDcpt is valid!
        pgDnsDcpt = &gDnsDcpt;
    }

    if(stackData->pNetIf->Flags.bIsDnsClientEnabled != 0U)
    {   // enable DNS client service
        (void)F_DNS_Enable(stackData->pNetIf, false, TCPIP_DNS_ENABLE_DEFAULT);
    }
    dnsInitCount++;
    return true;
}

static bool F_DNSClientSocketOpen(TCPIP_DNS_DCPT* pDnsDcpt)
{
    UDP_SOCKET dnsSocket;

    if(pDnsDcpt == NULL)
    {
        return false;
    }

    if(pDnsDcpt->dnsSocket != INVALID_UDP_SOCKET)
    {
        return true;
    }

    bool success = false;

    while(true)
    {
        uint16_t   bufferSize;

        dnsSocket = TCPIP_UDP_ClientOpen(pDnsDcpt->ipAddressType, TCPIP_DNS_SERVER_PORT, NULL);
        if(dnsSocket == INVALID_UDP_SOCKET)
        {
            break;
        }

        uint16_t minDnsTxSize = 18U + (uint16_t)TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN + 1U;
        bufferSize = TCPIP_UDP_TxPutIsReady(dnsSocket, minDnsTxSize);
        if(bufferSize < minDnsTxSize)
        {
            if(!TCPIP_UDP_OptionsSet(dnsSocket, UDP_OPTION_TX_BUFF, FC_Uint2VPtr((uint32_t)minDnsTxSize)))
            {
                break;
            }
        }

        (void)TCPIP_UDP_OptionsSet(dnsSocket, UDP_OPTION_STRICT_ADDRESS, FC_Uint2VPtr(0UL));
        if(TCPIP_UDP_SignalHandlerRegister(dnsSocket, TCPIP_UDP_SIGNAL_RX_DATA, &F_DNSSocketRxSignalHandler, NULL) == NULL)
        {
            break;
        }

        // success
        pDnsDcpt->dnsSocket = dnsSocket;
        success = true;
        break;
    }

    if(!success && dnsSocket != INVALID_UDP_SOCKET)
    {
        (void)TCPIP_UDP_Close(dnsSocket);
    }

    return success;
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void F_DNSClientSocketClose(TCPIP_DNS_DCPT* pDnsDcpt)
{
    if(pDnsDcpt->dnsSocket != INVALID_UDP_SOCKET)
    {
        (void)TCPIP_UDP_Close(pDnsDcpt->dnsSocket);
        pDnsDcpt->dnsSocket = INVALID_UDP_SOCKET;
    }
}


static void F_DNSClientCleanup(TCPIP_DNS_DCPT* pDnsDcpt)
{
    F_DNSClientSocketClose(pDnsDcpt);

    // Remove dns Timer Handle
    if( pDnsDcpt->dnsSignalHandle != NULL)
    {
       TCPIPStackSignalHandlerDeregister( pDnsDcpt->dnsSignalHandle);
        pDnsDcpt->dnsSignalHandle = NULL;
    }
    // Remove DNS register users
#if (TCPIP_DNS_CLIENT_USER_NOTIFICATION != 0)
    TCPIP_Notification_Deinitialize(&pDnsDcpt->dnsRegisteredUsers, pDnsDcpt->memH);
#endif  // (TCPIP_DNS_CLIENT_USER_NOTIFICATION != 0)

    // Delete Hash Entries
    F_DNS_DeleteHash(pDnsDcpt);
}

void TCPIP_DNS_ClientDeinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData)
{
    // interface going down

    if(dnsInitCount > 0)
    {   // we're up and running
        UDP_SOCKET_INFO sktInfo;
        TCPIP_DNS_DCPT* pDnsDcpt = pgDnsDcpt;

        if(TCPIP_UDP_SocketInfoGet(pDnsDcpt->dnsSocket, &sktInfo))
        {   // socket still alive
            if(sktInfo.hNet == stackData->pNetIf)
            {   // going down; disconnect
                (void)TCPIP_UDP_Disconnect(pDnsDcpt->dnsSocket, true);
            }
        }

        if(stackData->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
        {   // stack shut down
            if(--dnsInitCount == 0)
            {   // all closed and Release DNS client Hash resources
                F_DNSClientCleanup(pDnsDcpt);
                // module is de-initialized and pgDnsDcpt is invalid!
                pgDnsDcpt = NULL;
            }
        }
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

TCPIP_DNS_RESULT TCPIP_DNS_Resolve(const char* hostName, TCPIP_DNS_RESOLVE_TYPE type)
{
    return F_DNS_Resolve(hostName, type, false);
}

TCPIP_DNS_RESULT TCPIP_DNS_Send_Query(const char* hostName, TCPIP_DNS_RESOLVE_TYPE type)
{
    return F_DNS_Resolve(hostName, type, true);
}

static TCPIP_DNS_RESULT F_DNS_Resolve(const char* hostName, TCPIP_DNS_RESOLVE_TYPE type, bool forceQuery)
{
    TCPIP_DNS_DCPT            *pDnsDcpt;
    TCPIP_DNS_HASH_ENTRY      *dnsHE;
    IP_MULTI_ADDRESS    ipAddr;
    uint8_t             recMask;

    pDnsDcpt = pgDnsDcpt;

    if(pDnsDcpt == NULL)
    {
        return TCPIP_DNS_RES_NO_SERVICE;
    }

    if(hostName == NULL || strlen(hostName) == 0U || strlen(hostName)  >= (size_t)TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN)
    {
        return TCPIP_DNS_RES_INVALID_HOSTNAME; 
    }

    if(TCPIP_Helper_StringToIPAddress(hostName, &ipAddr.v4Add))
    {   // DNS request is a valid IPv4 or IPv6 address
        return  TCPIP_DNS_RES_NAME_IS_IPADDRESS;
    }
    else if(TCPIP_Helper_StringToIPv6Address (hostName, &ipAddr.v6Add))
    {   // DNS request is a valid IPv4 or IPv6 address
        return  TCPIP_DNS_RES_NAME_IS_IPADDRESS;
    }
    else
    {
        // OK
    }
 
    dnsHE = FC_OaHash2DnsHash(TCPIP_OAHASH_EntryLookupOrInsert(pDnsDcpt->hashDcpt, (const void*)hostName));
    if(dnsHE == NULL)
    {   // no more entries
        return TCPIP_DNS_RES_CACHE_FULL; 
    }

    if(type == TCPIP_DNS_TYPE_A)
    {
        recMask = (uint8_t)TCPIP_DNS_ADDRESS_REC_IPV4;
    }
    else if(type == TCPIP_DNS_TYPE_AAAA)
    {
        recMask = (uint8_t)TCPIP_DNS_ADDRESS_REC_IPV6;
    }
    else
    {
        recMask = (uint8_t)TCPIP_DNS_ADDRESS_REC_IPV4 | (uint8_t)TCPIP_DNS_ADDRESS_REC_IPV6;
    }

    if(forceQuery == false && dnsHE->hEntry.flags.newEntry == 0U)
    {   // already in hash
        if((dnsHE->recordMask & recMask) == recMask)
        {   // already have the requested type
            if((dnsHE->hEntry.flags.value & (uint16_t)TCPIP_DNS_FLAG_ENTRY_COMPLETE) != 0U)
            {
               return TCPIP_DNS_RES_OK; 
            }
            return (dnsHE->hEntry.flags.value & (uint16_t)TCPIP_DNS_FLAG_ENTRY_TIMEOUT) == 0U ? TCPIP_DNS_RES_PENDING : TCPIP_DNS_RES_SERVER_TMO; 
        }
        // else new query is needed, for new type
    }

    // this is a forced/new entry/query
    // update entry parameters
    if(dnsHE->hEntry.flags.newEntry != 0U)
    {
        dnsHE->nIPv4Entries = 0U;
        dnsHE->nIPv6Entries = 0U;
        dnsHE->hEntry.flags.value &= ~((uint16_t)TCPIP_DNS_FLAG_ENTRY_COMPLETE | (uint16_t)TCPIP_DNS_FLAG_ENTRY_TIMEOUT);
    }
    else
    {   // forced
        if((recMask & (uint8_t)TCPIP_DNS_ADDRESS_REC_IPV4) != 0U)
        {
            dnsHE->nIPv4Entries = 0U;
        }
        if((recMask & (uint8_t)TCPIP_DNS_ADDRESS_REC_IPV6) != 0U)
        {
            dnsHE->nIPv6Entries = 0U;
        }
        dnsHE->hEntry.flags.value &= ~(uint16_t)TCPIP_DNS_FLAG_ENTRY_COMPLETE;
    }
    dnsHE->ipTTL.Val = 0U;
    dnsHE->resolve_type = (uint8_t)type;
    dnsHE->recordMask |= recMask;
    dnsHE->tRetry = dnsHE->tInsert = pDnsDcpt->dnsTime;
    dnsHE->currRetry = 0U;
    // if a strict interface, we try only on that; otherwise on all
    unsigned int retryIfs = (pDnsDcpt->strictNet == NULL) ? (unsigned int)TCPIP_STACK_NumberOfNetworksGet() : 1U;
    dnsHE->nRetries = (uint8_t)(retryIfs * M_TCPIP_DNS_IF_RETRY_COUNT);
    pDnsDcpt->unsolvedEntries++;
    return F_DNS_Send_Query(pDnsDcpt, dnsHE);
}

static size_t F_DNS_GetAddresses(const char* hostName, size_t startIndex, IP_MULTI_ADDRESS pIPAddr[], size_t nIPAddresses, TCPIP_DNS_ADDRESS_REC_MASK recMask)
{
    TCPIP_DNS_DCPT* pDnsDcpt;
    TCPIP_DNS_HASH_ENTRY*    dnsHashEntry;
    IP_MULTI_ADDRESS* pDst4Addr;
    IP_MULTI_ADDRESS* pDst6Addr;
    IPV4_ADDR*      pSrc4Addr;
    IPV6_ADDR*      pSrc6Addr;
    size_t          ix;
    size_t          nAddrs;

    pDnsDcpt = pgDnsDcpt;
    if(pDnsDcpt == NULL || hostName == NULL || pIPAddr == NULL || nIPAddresses == 0U)
    {
        return 0U;
    }

    dnsHashEntry = FC_OaHash2DnsHash(TCPIP_OAHASH_EntryLookup(pDnsDcpt->hashDcpt, hostName));
    if(dnsHashEntry == NULL || (dnsHashEntry->hEntry.flags.value & (uint16_t)TCPIP_DNS_FLAG_ENTRY_COMPLETE) == 0U)
    {
        return 0U;
    }

    uint8_t u8Mask = (uint8_t)recMask & dnsHashEntry->recordMask;
    recMask = (TCPIP_DNS_ADDRESS_REC_MASK)u8Mask;

    if(recMask == TCPIP_DNS_ADDRESS_REC_NONE)
    {
        return 0U; 
    }

    nAddrs = 0U;
    if(recMask == TCPIP_DNS_ADDRESS_REC_IPV4)
    {
        pDst4Addr = pIPAddr;
        pSrc4Addr =  dnsHashEntry->pip4Address + startIndex;
        for(ix = startIndex; ix <= dnsHashEntry->nIPv4Entries; ix++)
        {
            pDst4Addr->v4Add.Val = pSrc4Addr->Val;
            pDst4Addr++;
            pSrc4Addr++;
            if(++nAddrs >= nIPAddresses)
            {
                break;
            }
        }
    }
    else
    {   // TCPIP_DNS_ADDRESS_REC_IPV6
        pDst6Addr = pIPAddr;
        pSrc6Addr =  dnsHashEntry->pip6Address + startIndex;
        for(ix = startIndex; ix <= dnsHashEntry->nIPv6Entries; ix++)
        {
            (void)memcpy(pDst6Addr->v6Add.v, pSrc6Addr->v, sizeof(pDst6Addr->v6Add));
            pDst6Addr++;
            if(++nAddrs >= nIPAddresses)
            {
                break;
            }
        }
    }

    return nAddrs;
}

size_t TCPIP_DNS_GetIPv4Addresses(const char* hostName, size_t startIndex, IPV4_ADDR* pIPv4Addr, size_t nIPv4Addresses)
{
    return F_DNS_GetAddresses(hostName, startIndex, FC_Ip4Add2Multi(pIPv4Addr), nIPv4Addresses, TCPIP_DNS_ADDRESS_REC_IPV4);
}

size_t TCPIP_DNS_GetIPv6Addresses(const char* hostName, size_t startIndex, IPV6_ADDR* pIPv6Addr, size_t nIPv6Addresses)
{
    return F_DNS_GetAddresses(hostName, startIndex, FC_Ip6Add2Multi(pIPv6Addr), nIPv6Addresses, TCPIP_DNS_ADDRESS_REC_IPV6);
}

size_t TCPIP_DNS_GetIPAddressesNumber(const char* hostName, IP_ADDRESS_TYPE type)
{
    TCPIP_DNS_DCPT*     pDnsDcpt;
    TCPIP_DNS_HASH_ENTRY*    dnsHashEntry;
    OA_HASH_ENTRY*   hE;
    uint8_t recMask;
    size_t  nAddresses;
    
    pDnsDcpt = pgDnsDcpt;
    if(pDnsDcpt == NULL || hostName == NULL)
    {
        return 0U;
    }

    if(type == IP_ADDRESS_TYPE_IPV4)
    {
        recMask = (uint8_t)TCPIP_DNS_ADDRESS_REC_IPV4;
    }
    else if(type == IP_ADDRESS_TYPE_IPV6)
    {
        recMask = (uint8_t)TCPIP_DNS_ADDRESS_REC_IPV6;
    }
    else
    {
        recMask = (uint8_t)TCPIP_DNS_ADDRESS_REC_IPV4 | (uint8_t)TCPIP_DNS_ADDRESS_REC_IPV6;
    }


    nAddresses = 0U;
    hE = TCPIP_OAHASH_EntryLookup(pDnsDcpt->hashDcpt, hostName);
    if(hE != NULL)
    {
        if((hE->flags.value & (uint16_t)TCPIP_DNS_FLAG_ENTRY_COMPLETE) != 0U)
        {
            dnsHashEntry = FC_OaHash2DnsHash(hE);
            if(((dnsHashEntry->recordMask & recMask) & (uint8_t)TCPIP_DNS_ADDRESS_REC_IPV4) != 0U)
            {
                nAddresses += (size_t)dnsHashEntry->nIPv4Entries;
            }

            if(((dnsHashEntry->recordMask & recMask) & (uint8_t)TCPIP_DNS_ADDRESS_REC_IPV6) != 0U)
            {
                 nAddresses += (size_t)dnsHashEntry->nIPv6Entries;
            }
        }
    }

    return nAddresses;
}

TCPIP_DNS_RESULT  TCPIP_DNS_IsResolved(const char* hostName, IP_MULTI_ADDRESS* hostIP, IP_ADDRESS_TYPE type)
{
    IPV4_ADDR* hostIPv4;
    IPV6_ADDR* hostIPv6;

    if(type == IP_ADDRESS_TYPE_IPV4)
    {
        hostIPv4 = &hostIP->v4Add;
        hostIPv6 = NULL;
    }
    else if(type == IP_ADDRESS_TYPE_IPV6)
    {
        hostIPv6 = &hostIP->v6Add;
        hostIPv4 = NULL;
    }
    else
    {
        hostIPv4 = &hostIP->v4Add;
        hostIPv6 = &hostIP->v6Add;
    }

    return F_DNS_IsNameResolved(hostName, hostIPv4, hostIPv6, true);
}

TCPIP_DNS_RESULT  TCPIP_DNS_IsNameResolved(const char* hostName, IPV4_ADDR* hostIPv4, IPV6_ADDR* hostIPv6)
{
    return F_DNS_IsNameResolved(hostName, hostIPv4, hostIPv6, false);
}

// retrieves the IP addresses corresponding to the hostName
// if singleAddress, then only one address is returned, either IPv4 or IPv6
static TCPIP_DNS_RESULT  F_DNS_IsNameResolved(const char* hostName, IPV4_ADDR* hostIPv4, IPV6_ADDR* hostIPv6, bool singleAddress)
{    
    TCPIP_DNS_DCPT*     pDnsDcpt;
    TCPIP_DNS_HASH_ENTRY* pDnsHE;
    size_t              nIPv4Entries;
    size_t              nIPv6Entries;
    IP_MULTI_ADDRESS    mAddr;            

    if(hostIPv4 != NULL)
    {
        hostIPv4->Val = 0;
    }
    if(hostIPv6 != NULL)
    {
        (void)memset(hostIPv6->v, 0, sizeof(*hostIPv6));
    }

    pDnsDcpt = pgDnsDcpt;

    if(pDnsDcpt == NULL)
    {
        return TCPIP_DNS_RES_NO_SERVICE;
    }
    
    if(TCPIP_Helper_StringToIPAddress(hostName, &mAddr.v4Add))
    {   // name id a IPv4 address
        if(hostIPv4 != NULL)
        {
            hostIPv4->Val = mAddr.v4Add.Val;
        }
        return  TCPIP_DNS_RES_OK; 
    }
    if (TCPIP_Helper_StringToIPv6Address (hostName, &mAddr.v6Add))
    {   // name is a IPv6 address
        if(hostIPv6 != NULL)
        {
            (void)memcpy (hostIPv6->v, mAddr.v6Add.v, sizeof (IPV6_ADDR));
        }
        return  TCPIP_DNS_RES_OK; 
    }
    
    pDnsHE = FC_OaHash2DnsHash(TCPIP_OAHASH_EntryLookup(pDnsDcpt->hashDcpt, hostName));
    if(pDnsHE == NULL)
    {
        return TCPIP_DNS_RES_NO_NAME_ENTRY;
    }

    if((pDnsHE->hEntry.flags.value & (uint16_t)TCPIP_DNS_FLAG_ENTRY_COMPLETE) == 0U)
    {   // unsolved entry   
        return (pDnsHE->hEntry.flags.value & (uint16_t)TCPIP_DNS_FLAG_ENTRY_TIMEOUT) == 0U ? TCPIP_DNS_RES_PENDING : TCPIP_DNS_RES_SERVER_TMO; 
    }

    // completed entry
    nIPv6Entries = pDnsHE->nIPv6Entries;
    nIPv4Entries = pDnsHE->nIPv4Entries;

    if(nIPv6Entries != 0U || nIPv4Entries != 0U)
    {
        if(nIPv6Entries != 0U)
        {
            if(hostIPv6 != NULL)
            {
                (void)memcpy (hostIPv6->v, (pDnsHE->pip6Address + nIPv6Entries - 1U)->v, sizeof (IPV6_ADDR));
                if(singleAddress)
                {   //  retrieve only one address
                    nIPv4Entries = 0U;
                }
            }
        }

        if(nIPv4Entries != 0U)
        {
            if(hostIPv4 != NULL)
            {   // get the  0th location of the address
                hostIPv4->Val = (pDnsHE->pip4Address + 0)->Val;
            }
        }
        return TCPIP_DNS_RES_OK;
    }

    return TCPIP_DNS_RES_NO_IP_ENTRY;
}

TCPIP_DNS_RESULT TCPIP_DNS_ClientInfoGet(TCPIP_DNS_CLIENT_INFO* pClientInfo)
{
    TCPIP_DNS_DCPT* pDnsDcpt = pgDnsDcpt;

    if(pDnsDcpt==NULL)
    {
         return TCPIP_DNS_RES_NO_SERVICE;
    }

    if(pClientInfo != NULL)
    {
        pClientInfo->strictNet = pDnsDcpt->strictNet;
        pClientInfo->prefNet = pDnsDcpt->prefNet;
        pClientInfo->dnsTime = pDnsDcpt->dnsTime;
        pClientInfo->pendingEntries = (uint16_t)pDnsDcpt->unsolvedEntries;
        pClientInfo->currentEntries = (uint16_t)pDnsDcpt->hashDcpt->fullSlots;
        pClientInfo->totalEntries = (uint16_t)pDnsDcpt->hashDcpt->hEntries;
    }
    return TCPIP_DNS_RES_OK;
}

TCPIP_DNS_RESULT TCPIP_DNS_EntryQuery(TCPIP_DNS_ENTRY_QUERY *pDnsQuery, size_t queryIndex)
{
    OA_HASH_ENTRY*  pBkt;
    TCPIP_DNS_HASH_ENTRY  *pE;
    TCPIP_DNS_DCPT        *pDnsDcpt;
    size_t          ix;
    uint32_t        currTime;

    pDnsDcpt = pgDnsDcpt;

    if(pDnsDcpt == NULL || pDnsDcpt->hashDcpt == NULL)
    {
        return TCPIP_DNS_RES_NO_SERVICE;
    }

    if(pDnsQuery == NULL || pDnsQuery->hostName == NULL || pDnsQuery->nameLen == 0U)
    {
        return TCPIP_DNS_RES_INVALID_HOSTNAME;
    }

    pBkt = TCPIP_OAHASH_EntryGet(pDnsDcpt->hashDcpt, queryIndex);
    if(pBkt == NULL)
    {
        return TCPIP_DNS_RES_NO_IX_ENTRY;
    }

    if(pBkt->flags.busy != 0U)
    {
        pE = FC_OaHash2DnsHash(pBkt);
        (void)strncpy(pDnsQuery->hostName, pE->pHostName, pDnsQuery->nameLen - 1U);
        pDnsQuery->hostName[pDnsQuery->nameLen - 1U] = '\0';
        pDnsQuery->hNet = pE->currNet;
        pDnsQuery->serverIx = pE->currServerIx;

        if((pBkt->flags.value & (uint16_t)TCPIP_DNS_FLAG_ENTRY_COMPLETE) != 0U)
        {
            pDnsQuery->status = TCPIP_DNS_RES_OK;
            currTime = pDnsDcpt->dnsTime;
            if(pDnsDcpt->cacheEntryTMO > 0U)
            {
                pDnsQuery->ttlTime = pDnsDcpt->cacheEntryTMO - (currTime - pE->tInsert);
            }
            else
            {
                pDnsQuery->ttlTime = pE->ipTTL.Val - (currTime - pE->tInsert);
            }

            for(ix = 0; ix < pE->nIPv4Entries && ix < pDnsQuery->nIPv4Entries; ix++)
            {
                pDnsQuery->ipv4Entry[ix].Val = pE->pip4Address[ix].Val;
            }
            pDnsQuery->nIPv4ValidEntries = ix;

            for(ix = 0; ix < pE->nIPv6Entries && ix < pDnsQuery->nIPv6Entries; ix++)
            {
                (void)memcpy(pDnsQuery->ipv6Entry[ix].v, pE->pip6Address[ix].v, sizeof(IPV6_ADDR));
            }
            pDnsQuery->nIPv6ValidEntries = ix;

            return TCPIP_DNS_RES_OK;
        }
        else
        {
            pDnsQuery->status = (pE->hEntry.flags.value & (uint16_t)TCPIP_DNS_FLAG_ENTRY_TIMEOUT) == 0U ? TCPIP_DNS_RES_PENDING : TCPIP_DNS_RES_SERVER_TMO; 
            pDnsQuery->ttlTime = 0;
            pDnsQuery->nIPv4ValidEntries = 0;
            pDnsQuery->nIPv6ValidEntries = 0;
        }
        return TCPIP_DNS_RES_OK;
    }

    return TCPIP_DNS_RES_EMPTY_IX_ENTRY;
}

// selects a interface for a DNS hash entry transaction 
static bool F_DNS_SelectIntf(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* pDnsHE)
{
    size_t  ix, nIfs;
    const TCPIP_NET_IF*   pDnsIf;
    const TCPIP_NET_IF*   dnsSelectIfs[TCPIP_DNS_CLIENT_MAX_SELECT_INTERFACES];    // interfaces to select for DNS


    if(pDnsDcpt->strictNet != NULL)
    {   // only the strict interface is checked
        return F_DNS_ValidateIf(pDnsDcpt, pDnsDcpt->strictNet, pDnsHE, true);
    }

    (void)memset(dnsSelectIfs, 0, sizeof(dnsSelectIfs));

    // add the interfaces to be considered
    // the preferred interface
    (void)F_DNS_AddSelectionIf(pDnsDcpt, pDnsDcpt->prefNet, dnsSelectIfs, sizeof(dnsSelectIfs) / sizeof(*dnsSelectIfs));
    // the current interface
    (void)F_DNS_AddSelectionIf(pDnsDcpt, pDnsHE->currNet, dnsSelectIfs, sizeof(dnsSelectIfs) / sizeof(*dnsSelectIfs));
    // the default interface
    (void)F_DNS_AddSelectionIf(pDnsDcpt, (const TCPIP_NET_IF*)TCPIP_STACK_NetDefaultGet(), dnsSelectIfs, sizeof(dnsSelectIfs) / sizeof(*dnsSelectIfs));
    // and any other interface
    nIfs = (size_t)TCPIP_STACK_NumberOfNetworksGet();
    for(ix = 0; ix < nIfs; ix++)
    {
       if(!F_DNS_AddSelectionIf(pDnsDcpt, (const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(ix), dnsSelectIfs, sizeof(dnsSelectIfs) / sizeof(*dnsSelectIfs)))
       {
           break;
       }
    }

    // calculate the number of interfaces we have
    nIfs = 0;
    for(ix = 0; ix < sizeof(dnsSelectIfs) / sizeof(*dnsSelectIfs); ix++)
    {
        if(dnsSelectIfs[ix] != NULL)
        {
            nIfs++;
        }
    }

    // search an interface
    for(ix = 0; ix < sizeof(dnsSelectIfs) / sizeof(*dnsSelectIfs); ix++)
    {
        if((pDnsIf = dnsSelectIfs[ix]) != NULL)
        {   // for the last valid interface allow DNS servers wrap around
            if(F_DNS_ValidateIf(pDnsDcpt, pDnsIf, pDnsHE, (ix == nIfs - 1U)))
            {
                return true;
            }
        }
    }

    // couldn't find a valid interface
    pDnsHE->currNet = NULL;    // make sure next time we start with a fresh interface
    return false;
}

// inserts a DNS valid interface into the dnsIfTbl
// returns false if table is full and interface could not be added
// true if OK
// Interface must support DNS traffic:
// up and running, configured and have valid DNS servers 
static bool F_DNS_AddSelectionIf(TCPIP_DNS_DCPT* pDnsDcpt, const TCPIP_NET_IF* pIf, const TCPIP_NET_IF* dnsIfTbl[], size_t tblEntries)
{
    size_t ix, addIx = 0;
    bool slotFound;

    if(pIf == NULL || !F_DNS_NetIsValid(pDnsDcpt, pIf))
    {
        return false;
    }

    slotFound = false;
    for(ix = 0; ix < tblEntries; ix++)
    {
        if(dnsIfTbl[ix] == pIf)
        {   // already there
            return true;
        }
        if(dnsIfTbl[ix] == NULL && slotFound == false)
        {   // insert slot
            addIx = ix;
            slotFound = true;
        }
    }

    // pIf is not in the table
    if(slotFound == true)
    {
        dnsIfTbl[addIx] = pIf;
        return true;
    }

    // table full
    return false;
}

// returns true if the pIf can be selected for DNS traffic
// false otherwise
// For IPv4 it sets the current interface and server index too
// if a retry (i.e. the entry already has that interface) the server index is advanced too
// if wrapAround, the server index is cleared to 0
static bool F_DNS_ValidateIf(TCPIP_DNS_DCPT* pDnsDcpt, const TCPIP_NET_IF* pIf, TCPIP_DNS_HASH_ENTRY* pDnsHE, bool wrapAround)
{
    if(F_DNS_NetIsValid(pDnsDcpt, pIf))
    {
#if defined(TCPIP_STACK_USE_IPV4)
        if(pDnsDcpt->ipAddressType == IP_ADDRESS_TYPE_IPV4)
        {
            return F_DNS_ValidateIfIpv4(pIf, pDnsHE, wrapAround);
        } 
#endif  // defined(TCPIP_STACK_USE_IPV4)

#if defined(TCPIP_STACK_USE_IPV6)
        if(pDnsDcpt->ipAddressType == IP_ADDRESS_TYPE_IPV6)
        {
            return F_DNS_ValidateIfIpv6(pIf, pDnsHE, wrapAround);
        } 
#endif  // defined(TCPIP_STACK_USE_IPV6)

        TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
    }
    return false;
}

// returns true if the pIf can be selected for DNS traffic
// false otherwise
// it sets the current interface and server index too
// if a retry (i.e. the entry already has that interface) the server index is advanced too
// if wrapAround, the server index is cleared to 0
#if defined(TCPIP_STACK_USE_IPV4)
static bool F_DNS_ValidateIfIpv4(const TCPIP_NET_IF* pIf, TCPIP_DNS_HASH_ENTRY* pDnsHE, bool wrapAround)
{
    size_t  ix, startIx;
    bool    srvFound = false;

    if(pDnsHE->currNet == pIf)
    {   // trying the current interface
        // at 1st attempt just stick to what we had before
        // when retrying; use a different server index
        startIx = (pDnsHE->currRetry == 0U) ? (size_t)pDnsHE->currServerIx : (size_t)pDnsHE->currServerIx + 1U;
    }
    else
    {   // for a new interface start with the 1st server
        startIx = 0U;
    }

    for(ix = startIx; ix < sizeof(pIf->dnsServer) / sizeof(*pIf->dnsServer); ix++)
    {
        if(pIf->dnsServer[ix].Val != 0U)
        {   // all good; select new interface
            srvFound = true;
            break;
        }
    }

    // search from the beginning
    if(!srvFound && wrapAround) 
    {
        for(ix = 0; ix < startIx; ix++)
        {
            if(pIf->dnsServer[ix].Val != 0U)
            {   // all good; select new interface
                srvFound = true;
                break;
            }
        }
    }

    if(srvFound)
    {
        pDnsHE->currNet = pIf;
        pDnsHE->currServerIx = (uint8_t)ix;
    }

    F_DNS_DbgValidateIf(pIf, startIx, ix, false, srvFound);
    return srvFound;
}
#endif  // defined(TCPIP_STACK_USE_IPV4)

// returns true if the pIf can be selected for DNS traffic
// false otherwise
// it sets the current interface and server index too
// if a retry (i.e. the entry already has that interface) the server index is advanced too
// if wrapAround, the server index is cleared to 0
#if defined(TCPIP_STACK_USE_IPV6)
static bool F_DNS_ValidateIfIpv6(const TCPIP_NET_IF* pIf, TCPIP_DNS_HASH_ENTRY* pDnsHE, bool wrapAround)
{
    bool    srvFound = false;

    // for IPv6 we currently support just one IPv6 DNS address
    // just make sure the interface is valid

    if(TCPIP_IPV6_InterfaceIsReady(pIf))
    {   // interface ready
        if(pIf->Flags.bIpv6DnsValid == 1U)
        {
            srvFound = true;
        }
    }

    if(srvFound)
    {
        pDnsHE->currNet = pIf;
        pDnsHE->currServerIx = 0;
    }

    F_DNS_DbgValidateIf(pIf, 0, 0, true, srvFound);
    return srvFound;
}
#endif  // defined(TCPIP_STACK_USE_IPV6)

// returns true if a network interface can be selected for DNS traffic
// false otherwise
static bool F_DNS_NetIsValid(TCPIP_DNS_DCPT* pDnsDcpt, const TCPIP_NET_IF* pIf)
{
    if(TCPIP_STACK_NetIsReady(pIf) == false)
    {   // interface not up, not linked or not configured
        return false;
    }

    if(pIf->Flags.bIsDnsClientEnabled == 0U)
    {   // DNS not enabled on this interface
        return false;
    }

    while(true)
    {
#if defined(TCPIP_STACK_USE_IPV4)
        if(pDnsDcpt->ipAddressType == IP_ADDRESS_TYPE_IPV4)
        {
            if(TCPIPStackNetAddress(pIf) != 0U)
            {   // has a valid address
                return pIf->dnsServer[0].Val != 0U || pIf->dnsServer[1].Val != 0U;
            }
            break;
        }
#endif  // defined(TCPIP_STACK_USE_IPV4)

#if defined(TCPIP_STACK_USE_IPV6)
        if(pDnsDcpt->ipAddressType == IP_ADDRESS_TYPE_IPV6)
        {
            if(TCPIP_IPV6_InterfaceIsReady(pIf))
            {   // interface ready
                return pIf->Flags.bIpv6DnsValid == 1U;
            }
        }
        break;
#endif // defined(TCPIP_STACK_USE_IPV6)
    }

    return false;
}
// send a signal to the DNS module that data is available
// no manager alert needed since this normally results as a higher layer (UDP) signal
static void F_DNSSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_UDP_SIGNAL_RX_DATA)
    {
        (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}

static TCPIP_DNS_RESULT F_DNS_Send_Query(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* pDnsHE)
{
    TCPIP_DNS_HEADER    DNSPutHeader;
    uint8_t             *wrPtr, *startPtr;
    int16_t             sktPayload;
    TCPIP_DNS_EVENT_TYPE evType;
    TCPIP_DNS_RESULT    res;
#if defined(TCPIP_STACK_USE_IPV4)
    const IPV4_ADDR*    dnsServerAdd4;
#endif  // defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_IPV6)
    const IPV6_ADDR*    dnsServerAdd6;
    IPV6_ADDR_STRUCT    ip6AddStruct;
#endif  // defined(TCPIP_STACK_USE_IPV6)
    bool                sktUpdate;
    UDP_SOCKET          dnsSocket = pDnsDcpt->dnsSocket;
    
    pDnsHE->hEntry.flags.value &= ~(uint16_t)TCPIP_DNS_FLAG_ENTRY_COMPLETE;

    while(true)
    {
        size_t oldServerIx = pDnsHE->currServerIx; // store the previously used DNS server index
        const TCPIP_NET_IF* oldIf = pDnsHE->currNet;
        if(!F_DNS_SelectIntf(pDnsDcpt, pDnsHE)) 
        {   // couldn't get an output interface
            res = TCPIP_DNS_RES_NO_INTERFACE;
            evType = TCPIP_DNS_EVENT_NO_INTERFACE;
            break; 
        }

        sktUpdate = (pDnsHE->currServerIx != oldServerIx || pDnsHE->currNet != oldIf);  // if socket needs to change settings: switched to another server/interface
#if ((TCPIP_DNS_DEBUG_LEVEL & TCPIP_DNS_DEBUG_MASK_SKT_UPDATE) != 0)
        if(sktUpdate != false)
        {
            SYS_CONSOLE_PRINT("DNS debug sktUpdate - curr ix: %d, old ix: %d, curr net: 0x%08x, old net: 0x%08x\r\n", pDnsHE->currServerIx, oldServerIx, pDnsHE->currNet, oldIf); 
        }
#endif  // ((TCPIP_DNS_DEBUG_LEVEL & TCPIP_DNS_DEBUG_MASK_SKT_UPDATE) != 0)
        if(oldIf == NULL)
        {
            oldIf = pDnsHE->currNet; 
        }
   
#if defined(TCPIP_STACK_USE_IPV4)
        if(pDnsDcpt->ipAddressType == IP_ADDRESS_TYPE_IPV4 && sktUpdate != false) 
        {
            // abort (if any) pending ARP on the old DNS server.
            // a pending ARP counts as a socket TX pending packet
            // and newer packets could be discarded if the socket limit is exceeded
            IPV4_ADDR oldDns = oldIf->dnsServer[oldServerIx];
            if(oldDns.Val != 0U)
            {
                (void)TCPIP_ARP_EntryRemove(oldIf, &oldDns);
                F_DNS_DbgArpFlush(oldIf, oldServerIx, pDnsHE->currNet, pDnsHE->currServerIx, &oldDns);
            }
        }
#endif  // defined(TCPIP_STACK_USE_IPV4)

        if(TCPIP_UDP_PutIsReady(dnsSocket) == 0U)
        {   // failed to allocate another TX buffer
            res = TCPIP_DNS_RES_SOCKET_ERROR;
            evType = TCPIP_DNS_EVENT_SOCKET_ERROR;
            break; 
        }

        // this will put the start pointer at the beginning of the TX buffer
        (void)TCPIP_UDP_TxOffsetSet(dnsSocket, 0, false);    

        //Get the write pointer:
        wrPtr = TCPIP_UDP_TxPointerGet(dnsSocket);
        if(wrPtr == NULL)
        {
            res = TCPIP_DNS_RES_SOCKET_ERROR;
            evType = TCPIP_DNS_EVENT_SOCKET_ERROR;
            break; 
        }

        // set up the socket, if needed
        res = TCPIP_DNS_RES_OK;
        while(sktUpdate)
        { 
#if defined(TCPIP_STACK_USE_IPV4)
            if(pDnsDcpt->ipAddressType == IP_ADDRESS_TYPE_IPV4)
            { 
                if(!TCPIP_UDP_Bind(dnsSocket, IP_ADDRESS_TYPE_IPV4, 0, FC_Ip4Add2Multi(&pDnsHE->currNet->netIPAddr)))
                {
                    res = TCPIP_DNS_RES_SOCKET_ERROR;
                    evType = TCPIP_DNS_EVENT_SOCKET_ERROR;
                    break; 
                }
                dnsServerAdd4 = pDnsHE->currNet->dnsServer + pDnsHE->currServerIx;
                (void)TCPIP_UDP_DestinationIPAddressSet(dnsSocket, IP_ADDRESS_TYPE_IPV4, FC_Ip4Add2Multi(dnsServerAdd4)); 
                break;
            }
#endif  // defined(TCPIP_STACK_USE_IPV4)

#if defined(TCPIP_STACK_USE_IPV6)
            if(pDnsDcpt->ipAddressType == IP_ADDRESS_TYPE_IPV6)
            {
                if(!F_GetNetIPv6Address(pDnsHE, &ip6AddStruct))
                {   // failed to get valid IPv6 address
                    res = TCPIP_DNS_RES_NO_INTERFACE;
                    evType = TCPIP_DNS_EVENT_NO_INTERFACE;
                    break; 
                }

                if(!TCPIP_UDP_Bind(dnsSocket, IP_ADDRESS_TYPE_IPV6, 0, FC_Ip6Add2Multi(&ip6AddStruct.address)))
                {
                    res = TCPIP_DNS_RES_SOCKET_ERROR;
                    evType = TCPIP_DNS_EVENT_SOCKET_ERROR;
                    break; 
                }
                dnsServerAdd6 = pDnsHE->currNet->netIPv6Dns + pDnsHE->currServerIx;
                (void)TCPIP_UDP_DestinationIPAddressSet(dnsSocket, pDnsDcpt->ipAddressType, FC_Ip6Add2Multi(dnsServerAdd6));
                break;
            }
#endif  // defined(TCPIP_STACK_USE_IPV6)

            TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
            res = TCPIP_DNS_RES_INTERNAL_ERROR;
            evType = TCPIP_DNS_EVENT_INTERNAL_ERROR;
            break; 
        }

        if(res != TCPIP_DNS_RES_OK)
        {
            break;
        }

        (void)TCPIP_UDP_DestinationPortSet(dnsSocket, TCPIP_DNS_SERVER_PORT);

        startPtr = wrPtr;
        // Put DNS query here
        // Set a new Transaction ID
        pDnsHE->transactionId.Val = (uint16_t)SYS_RANDOM_PseudoGet();
        DNSPutHeader.TransactionID.Val = TCPIP_Helper_htons(pDnsHE->transactionId.Val);
        // Flag -- Standard query with recursion
        DNSPutHeader.Flags.Val = TCPIP_Helper_htons(0x0100); // Standard query with recursion
        // Question -- only one question at this time
        DNSPutHeader.Questions.Val = TCPIP_Helper_htons(0x0001); // questions
        // Answers set to zero
        // Name server resource address also set to zero
        // Additional records also set to zero
        DNSPutHeader.Answers.Val = DNSPutHeader.AuthoritativeRecords.Val = DNSPutHeader.AdditionalRecords.Val = 0U;

        // copy the DNS header to the UDP buffer
        (void)memcpy(wrPtr, DNSPutHeader.v, sizeof(TCPIP_DNS_HEADER));
        wrPtr += sizeof(TCPIP_DNS_HEADER);

        // Put hostname string to resolve
        F_DNSPutString(&wrPtr, pDnsHE->pHostName);

        // Type: TCPIP_DNS_TYPE_A A (host address) or TCPIP_DNS_TYPE_MX for mail exchange
        *wrPtr++ = 0x00;
        *wrPtr++ = pDnsHE->resolve_type;

        // Class: IN (Internet)
        *wrPtr++ = 0x00;
        *wrPtr++ = 0x01; // 0x0001
    
        // Put complete DNS query packet buffer to the UDP buffer
        // Once it is completed writing into the buffer, you need to update the Tx offset again,
        // because the socket flush function calculates how many bytes are in the buffer using the current write pointer:
        TCPIPStack_Assert(wrPtr - startPtr >= 0, __FILE__, __func__, __LINE__);
        sktPayload = (int16_t)(wrPtr - startPtr);
        (void)TCPIP_UDP_TxOffsetSet(dnsSocket, (uint16_t)sktPayload, false);

        if(TCPIP_UDP_Flush(dnsSocket) != (uint16_t)sktPayload)
        {
            res = TCPIP_DNS_RES_SOCKET_ERROR;
            evType = TCPIP_DNS_EVENT_SOCKET_ERROR;
        }
        else
        {
            res = TCPIP_DNS_RES_PENDING;
            evType = TCPIP_DNS_EVENT_NAME_QUERY;
        }
        break;
    }

    // Send a DNS notification
    evType = evType;    // hush compiler warning if notifications disabled
    F_DNSNotifyClients(pDnsDcpt, pDnsHE, evType);
    return res;
}

// gets a net IPv6 UNICAST address that matches the scope of the DNS IPv6 address
// returns false if not found
// otherwise updates the ip6AddStr that's passed in and returns true.
// ip6AddStr cannot be NULL!
#if defined(TCPIP_STACK_USE_IPV6)
static bool F_GetNetIPv6Address(TCPIP_DNS_HASH_ENTRY* pDnsHE, IPV6_ADDR_STRUCT* ip6AddStr)
{
    IPV6_ADDR_HANDLE    ip6AddHndl = NULL;
    bool res = false;

    const TCPIP_NET_IF* netIf = pDnsHE->currNet;

    (void)memset(ip6AddStr, 0, sizeof(*ip6AddStr));
    // select a local IPv6 address that matches the DNS address
    IPV6_ADDRESS_TYPE dnsType;
    dnsType.byte = TCPIP_IPV6_AddressTypeGet(netIf, netIf->netIPv6Dns + pDnsHE->currServerIx);

    while(true)
    {
        ip6AddHndl = TCPIP_STACK_NetIPv6AddressGet(netIf, IPV6_ADDR_TYPE_UNICAST, ip6AddStr, ip6AddHndl);
        if(ip6AddHndl == NULL)
        {   // no more valid IPv6 addresses;
            break; 
        }
        else if(ip6AddStr->flags.scope == dnsType.bits.scope)
        {   // found it
            res = true;
            break;
        }
        else
        {
            // continue
        }
    }

#if ((TCPIP_DNS_DEBUG_LEVEL & TCPIP_DNS_DEBUG_MASK_IPV6_SCOPE) != 0)
    SYS_CONSOLE_PRINT("DNS debug - select IPv6 addr scope: %d, res: %d\r\n", dnsType.bits.scope, res);
#endif  // ((TCPIP_DNS_DEBUG_LEVEL & TCPIP_DNS_DEBUG_MASK_IPV6_SCOPE) != 0)

    return res;
}
#endif  // defined(TCPIP_STACK_USE_IPV6)

TCPIP_DNS_RESULT TCPIP_DNS_RemoveEntry(const char *hostName)
{
    TCPIP_DNS_HASH_ENTRY  *pDnsHE;
    TCPIP_DNS_DCPT        *pDnsDcpt;

    pDnsDcpt = pgDnsDcpt;
    if(pDnsDcpt == NULL || pDnsDcpt->hashDcpt == NULL)
    {
        return TCPIP_DNS_RES_NO_SERVICE;
    }

    if(hostName == NULL)
    {
        return TCPIP_DNS_RES_INVALID_HOSTNAME;
    }

    pDnsHE = FC_OaHash2DnsHash(TCPIP_OAHASH_EntryLookup(pDnsDcpt->hashDcpt, hostName));
    if(pDnsHE != NULL)
    {
        F_DNS_UpdateExpiredHashEntry_Notify(pDnsDcpt, pDnsHE);
        return TCPIP_DNS_RES_OK;
    }

    return TCPIP_DNS_RES_NO_NAME_ENTRY;
}

TCPIP_DNS_RESULT TCPIP_DNS_RemoveAll(void)
{
    OA_HASH_ENTRY*  pBkt;
    size_t          bktIx;
    TCPIP_DNS_DCPT* pDnsDcpt;
    OA_HASH_DCPT*   pOh;

    pDnsDcpt = pgDnsDcpt;

    if(pDnsDcpt == NULL || pDnsDcpt->hashDcpt == NULL)
    {
        return TCPIP_DNS_RES_NO_SERVICE;
    }

    pOh = pDnsDcpt->hashDcpt;
    for(bktIx = 0; bktIx < pOh->hEntries; bktIx++)
    {
        pBkt = TCPIP_OAHASH_EntryGet(pOh, bktIx);
        if(pBkt != NULL)
        {
            if(pBkt->flags.busy != 0U)
            {
                F_DNS_UpdateExpiredHashEntry_Notify(pDnsDcpt, FC_OaHash2DnsHash(pBkt));
            }
        }
    }

    return TCPIP_DNS_RES_OK;
}

void TCPIP_DNS_ClientTask(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != TCPIP_MODULE_SIGNAL_NONE)
    {   // signal: TMO/RX occurred
        TCPIP_DNS_ClientProcess(((uint16_t)sigPend & (uint16_t)TCPIP_MODULE_SIGNAL_TMO) != 0U);
    }

}

static void TCPIP_DNS_CacheTimeout(TCPIP_DNS_DCPT* pDnsDcpt)
{
    TCPIP_DNS_HASH_ENTRY  *pDnsHE;
    size_t          bktIx;
    OA_HASH_DCPT*   pOH;
    uint32_t        currTime;
    uint32_t        timeout;

    // get current time: seconds
    currTime = pDnsDcpt->dnsTime;
    pOH = pDnsDcpt->hashDcpt;

    // check the lease values; remove the expired leases
    for(bktIx = 0; bktIx < pOH->hEntries; bktIx++)
    {
        pDnsHE = FC_OaHash2DnsHash(TCPIP_OAHASH_EntryGet(pOH, bktIx));
        if(pDnsHE->hEntry.flags.busy != 0U)
        {   // not empty hash slot
            if((pDnsHE->hEntry.flags.value & (uint16_t)TCPIP_DNS_FLAG_ENTRY_COMPLETE) != 0U)
            {   // solved entry: check timeout
                // if cacheEntryTMO is equal to zero, then TTL time is the timeout period. 
                if((timeout = pDnsDcpt->cacheEntryTMO) == 0U)
                {
                    timeout = pDnsHE->ipTTL.Val;
                }
                if((currTime - pDnsHE->tInsert) >= timeout)
                {
                    F_DNS_UpdateExpiredHashEntry_Notify(pDnsDcpt, pDnsHE);
                }
            }
            else
            {   // unsolved entry
                if((pDnsHE->hEntry.flags.value & (uint16_t)TCPIP_DNS_FLAG_ENTRY_TIMEOUT) == 0U)
                {   // alive entry
                    if((currTime - pDnsHE->tRetry) >= (uint32_t)TCPIP_DNS_CLIENT_LOOKUP_RETRY_TMO)
                    {   // time for another attempt; the previous one should have been expired
                        pDnsHE->tRetry = currTime;
                        if(pDnsHE->currRetry < pDnsHE->nRetries)
                        {   // more attempts; send further probes for unsolved entries
                            pDnsHE->currRetry++;
                            (void)F_DNS_Send_Query(pDnsDcpt, pDnsHE);
                        }
                        else
                        {   // exhausted retries; timed out
                            pDnsHE->hEntry.flags.value |= (uint16_t)TCPIP_DNS_FLAG_ENTRY_TIMEOUT;
                        }
                    }
                }
                else
                {   // timed out entry; check if it's time to remove it from cache
                    if((currTime - pDnsHE->tRetry) >= M_TCPIP_DNS_CACHE_UNSOLVED_EXPIRE_TMO)
                    {
                        F_DNS_UpdateExpiredHashEntry_Notify(pDnsDcpt, pDnsHE);
                    }
                }
            }
        }
    } 
}


static void TCPIP_DNS_ClientProcess(bool isTmo)
{
    TCPIP_DNS_DCPT*     pDnsDcpt;
  
    if((pDnsDcpt = pgDnsDcpt) == NULL)
    {   // nothing to do
        return;
    }

    if(isTmo)
    {   // maintain the cache timeouts
        uint32_t tickFreq = SYS_TMR_TickCounterFrequencyGet(); 
        pDnsDcpt->dnsTime = (uint32_t)(SYS_TMR_TickCountGetLong() / tickFreq);
        TCPIP_DNS_CacheTimeout(pDnsDcpt);
    }

    while(true)
    {
        if(TCPIP_UDP_GetIsReady(pDnsDcpt->dnsSocket) == 0U)
        {   // done
            break;
        }

        TCPIPStack_Condition(pDnsDcpt->unsolvedEntries != 0, __FILE__, __func__, __LINE__);
        if(pDnsDcpt->unsolvedEntries != 0)
        {   // waiting for a reply; process the packet
            (void)F_DNS_ProcessPacket(pDnsDcpt);
        }
        else
        {
            F_DNS_DbgEvent(pDnsDcpt, NULL, TCPIP_DNS_DBG_EVENT_UNSOLICITED_ERROR);
        }

        (void)TCPIP_UDP_Discard(pDnsDcpt->dnsSocket);
    }

}

// extracts the IPv4/IPv6 addresses and updates the hash entry if dnsHE != 0
// if dnsHE == 0, than it just discards
// returns true if processing was successful
// false if some error occurred
static bool F_DNS_RESPONSE_HashEntryUpdate(TCPIP_DNS_RX_DATA* dnsRxData, TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* dnsHE)
{
    TCPIP_DNS_ANSWER_HEADER DNSAnswerHeader;    
    IP_MULTI_ADDRESS        ipAddr;
    bool                    discardData;

    (void)memset(&DNSAnswerHeader, 0, sizeof(DNSAnswerHeader));
    if(!F_DNSGetData(dnsRxData, (uint8_t *)&DNSAnswerHeader, sizeof(TCPIP_DNS_ANSWER_HEADER)))
    {   // failed to read the RR header
        return false;
    }

    F_SwapDNSAnswerPacket(&DNSAnswerHeader);

    // Make sure that this is a 4 byte IP address, response type A or MX, class 1
    // Check if this is Type A, MX, or AAAA
    discardData = true;

    while( dnsHE != NULL && (DNSAnswerHeader.ResponseClass.Val == 1U)) // Internet class
    {
        if (DNSAnswerHeader.ResponseType.Val == (uint16_t)TCPIP_DNS_TYPE_A && DNSAnswerHeader.ResponseLen.Val == 4U)
        {            
            if(dnsHE->nIPv4Entries >= pDnsDcpt->nIPv4Entries)
            {   // we have enough IPv4 entries
                break;
            }

            // read the buffer
            ipAddr.v4Add.Val = 0U;
            if(!F_DNSGetData(dnsRxData, ipAddr.v4Add.v, sizeof(IPV4_ADDR)))
            {
                return false;
            }

            discardData = false;
            // update the Hash entry for IPv4 address
            dnsHE->pip4Address[dnsHE->nIPv4Entries].Val = ipAddr.v4Add.Val;
            if((DNSAnswerHeader.ResponseTTL.Val < dnsHE->ipTTL.Val) || (dnsHE->ipTTL.Val == 0U))
            {
                dnsHE->ipTTL.Val = DNSAnswerHeader.ResponseTTL.Val;
            }
            dnsHE->nIPv4Entries++;
            // done
            break;
        }

        if (DNSAnswerHeader.ResponseType.Val == (uint16_t)TCPIP_DNS_TYPE_AAAA && DNSAnswerHeader.ResponseLen.Val == 16U)
        {
            if((dnsHE->recordMask & (uint8_t)TCPIP_DNS_ADDRESS_REC_IPV6) == 0U || (dnsHE->nIPv6Entries >= pDnsDcpt->nIPv6Entries))
            {   // not needed or enough IPvr entries
                break;
            }           

            // read the buffer
            (void)memset(ipAddr.v6Add.v, 0, sizeof(IPV6_ADDR));
            if(!F_DNSGetData(dnsRxData, ipAddr.v6Add.v, sizeof (IPV6_ADDR)))
            {
                return false;
            }

            discardData = false;
            // update the Hash entry for IPv6 address
            (void)memcpy(dnsHE->pip6Address[dnsHE->nIPv6Entries].v, ipAddr.v6Add.v, sizeof(IPV6_ADDR));
            if((DNSAnswerHeader.ResponseTTL.Val < dnsHE->ipTTL.Val) || (dnsHE->ipTTL.Val == 0U))
            {
                dnsHE->ipTTL.Val = DNSAnswerHeader.ResponseTTL.Val;
            }
            dnsHE->nIPv6Entries++;
            break;
        }

        // else discard and continue
        break;
    }

    if(discardData)
    {
        if(!F_DNSGetData(dnsRxData, NULL, DNSAnswerHeader.ResponseLen.Val))
        {
            return false;
        }
    }

    return true;
}

static TCPIP_DNS_HASH_ENTRY* F_DNSHashEntryFromTransactionId(TCPIP_DNS_DCPT* pDnsDcpt, const char* hostName, uint16_t transactionId)
{
    TCPIP_DNS_HASH_ENTRY* pDnsHE;
    pDnsHE = FC_OaHash2DnsHash(TCPIP_OAHASH_EntryLookup(pDnsDcpt->hashDcpt, hostName));
    if(pDnsHE != NULL)
    {
        if(pDnsHE->transactionId.Val == transactionId)
        {
            return pDnsHE;
        }
    }
    return NULL;
}

// retrieves a number of bytes from a source buffer and copies the data into the supplied destination buffer (if !0)
// returns true if the specified number of bytes could be removed from the source data buffer
// false otherwise
// updates the source buffer descriptor
static bool F_DNSGetData(TCPIP_DNS_RX_DATA* srcBuff, void *destBuff, size_t getBytes)
{
    ptrdiff_t diffBytes = FC_CU8PtrDiff(srcBuff->endPtr, srcBuff->rdPtr);
    TCPIPStack_Assert(diffBytes >= 0, __FILE__, __func__, __LINE__);

    size_t avlblBytes = (size_t)diffBytes;
    size_t copyBytes = getBytes;

    if(copyBytes > avlblBytes)
    {
        copyBytes = avlblBytes;
    }

    if(destBuff != NULL && copyBytes != 0U)
    {
        (void)memcpy((uint8_t*)destBuff, srcBuff->rdPtr, copyBytes);
    }

    srcBuff->rdPtr += copyBytes;

    return copyBytes == getBytes;
}

static void F_DNSInitRxData(TCPIP_DNS_RX_DATA* rxData, uint8_t* pBuff, uint16_t bufferSize)
{
    rxData->head = rxData->rdPtr = pBuff;
    rxData->endPtr = pBuff + bufferSize;
}

static int F_DNS_ProcessRR(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_RR_PROCESS* pProc, TCPIP_DNS_RR_TYPE rrType)
{
    TCPIP_DNS_HASH_ENTRY* dnsHE;
    int nameLen;
    TCPIP_DNS_DBG_EVENT_TYPE evDbgType = TCPIP_DNS_DBG_EVENT_NONE;
    char    nameBuffer[TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN + 1];               // holds the RR name             

    int nRecords = 0;
    uint16_t nRR;
    switch(rrType)
    {
        case TCPIP_DNS_RR_TYPE_QUESTION:
           nRR = pProc->dnsHeader->Questions.Val;
           break;

        case TCPIP_DNS_RR_TYPE_ANSWER:
           nRR = pProc->dnsHeader->Answers.Val;
           break;

        case TCPIP_DNS_RR_TYPE_AUTHORITATIVE:
           nRR = pProc->dnsHeader->AuthoritativeRecords.Val;
           break;

        case TCPIP_DNS_RR_TYPE_ADDITIONAL:
           nRR = pProc->dnsHeader->AdditionalRecords.Val;
           break;

        default:
           nRR = 0U;
           TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
           break;
    }


    while(nRR-- != 0U)
    {
        nameLen = F_DNS_ReadName(pProc, nameBuffer, sizeof(nameBuffer) - 1U);
        if(nameLen <= 0)
        {   // error retrieveing the question name ?
            evDbgType = TCPIP_DNS_DBG_EVENT_RR_XTRACT_ERROR; 
            break;
        }

        F_DNS_DbgRRName(nameBuffer, rrType);

        // make sure it's our query
        dnsHE = F_DNSHashEntryFromTransactionId(pDnsDcpt, nameBuffer, pProc->dnsHeader->TransactionID.Val);
        if(dnsHE == NULL)
        {   // not ours?
            if(pProc->dnsHE != NULL)
            {   // if already have proper names for this, we could use the data
                dnsHE = pProc->dnsHE;
            }
            // else leave dnsHE == 0
        }

        if(dnsHE != NULL)
        {
            if((dnsHE->hEntry.flags.value & (uint16_t)TCPIP_DNS_FLAG_ENTRY_COMPLETE) != 0U)
            {
                evDbgType = TCPIP_DNS_DBG_EVENT_COMPLETE_ERROR;
                break;
            }

            if(pProc->dnsHE == NULL)
            {
                pProc->dnsHE = dnsHE;
            }
            else if(pProc->dnsHE != dnsHE)
            {
                evDbgType = TCPIP_DNS_DBG_EVENT_RR_MISMATCH;
                break;
            }
            else
            {
                // do nothing
            }
        }

        // all good
        if(rrType == TCPIP_DNS_RR_TYPE_QUESTION)
        {   // skip the Question Type and Class
            if(!F_DNSGetData(pProc->dnsRxData, NULL, 4))
            {
                evDbgType = TCPIP_DNS_DBG_EVENT_RR_STRUCT_ERROR;
                break;
            }
        }
        else
        {
            bool entryUpdate = F_DNS_RESPONSE_HashEntryUpdate(pProc->dnsRxData, pDnsDcpt, dnsHE);
            if(entryUpdate == false)
            {
                evDbgType = TCPIP_DNS_DBG_EVENT_RR_DATA_ERROR;
                break;
            }
        }

        nRecords++;
    }

    if(nRecords == 0 && evDbgType == TCPIP_DNS_DBG_EVENT_NONE)
    {
        if(rrType == TCPIP_DNS_RR_TYPE_QUESTION)
        {
            evDbgType = TCPIP_DNS_DBG_EVENT_RR_NO_RECORDS; 
        }
        // other RR types is probably OK to miss 
    } 

    pProc->evDbgType = evDbgType;

    return nRecords;
}



// process a DNS packet
// returns true if info updated
// false if no entry was completed
static bool F_DNS_ProcessPacket(TCPIP_DNS_DCPT* pDnsDcpt)
{
    TCPIP_DNS_HEADER        DNSHeader;
    TCPIP_DNS_HASH_ENTRY    *dnsHE;
    TCPIP_DNS_RX_DATA       dnsRxData;
    uint8_t                 dnsRxBuffer[TCPIP_DNS_RX_BUFFER_SIZE];
    bool                    procFail;
    TCPIP_DNS_EVENT_TYPE    evType = TCPIP_DNS_EVENT_NONE;
    TCPIP_DNS_DBG_EVENT_TYPE evDbgType = TCPIP_DNS_DBG_EVENT_NONE;
    TCPIP_DNS_RR_PROCESS    procRR;


    // Get DNS Reply packet
    uint16_t dnsPacketSize = TCPIP_UDP_ArrayGet(pDnsDcpt->dnsSocket, dnsRxBuffer, (uint16_t)sizeof(dnsRxBuffer));

    F_DNSInitRxData(&dnsRxData, dnsRxBuffer, dnsPacketSize);

    // Retrieve the DNS header and de-big-endian it
    (void)memset(&DNSHeader, 0, sizeof(DNSHeader));
    if(!F_DNSGetData(&dnsRxData, &DNSHeader, sizeof(DNSHeader)))
    {   // incomplete packet?
        return false;
    }

    // Swap DNS Header received packet
    F_SwapDNSPacket(&DNSHeader);

    // populate the RR process structure
    procRR.dnsHeader = &DNSHeader;
    procRR.dnsPacket = dnsRxBuffer;
    procRR.dnsRxData = &dnsRxData;
    procRR.dnsPacketSize = dnsPacketSize;
    procRR.dnsHE = NULL;

    while(true)
    {
        dnsHE = NULL;
        procFail = false;

        if((DNSHeader.Flags.v[0] & 0x03U) != 0U)
        {   
            evType = TCPIP_DNS_EVENT_NAME_ERROR;
            procFail = true;
            break;
        }

        // process queries and all types of RRs
        int ix;
        for(ix = (int)TCPIP_DNS_RR_TYPE_QUESTION; ix < (int)TCPIP_DNS_RR_TYPES; ix++) 
        {
            (void)F_DNS_ProcessRR(pDnsDcpt, &procRR, (TCPIP_DNS_RR_TYPE)ix);
            if(procRR.evDbgType != TCPIP_DNS_DBG_EVENT_NONE)
            {   // some issue occurred
                evDbgType = procRR.evDbgType;
                procFail = true;
                break;
            } 
        }

        if(procFail)
        {   // failed
            break;
        }
        
        // save the entry
        dnsHE = procRR.dnsHE;

        // finally
        if(dnsHE != NULL && (dnsHE->nIPv4Entries > 0U || dnsHE->nIPv6Entries > 0U))
        {
            evType = TCPIP_DNS_EVENT_NAME_RESOLVED;
        }           
        else
        {
            evDbgType = TCPIP_DNS_DBG_EVENT_NO_IP_ERROR;
            procFail = true;
        }

        break;
    }

    if(evType != TCPIP_DNS_EVENT_NONE)
    {
        F_DNSNotifyClients(pDnsDcpt, dnsHE, evType);

        if(evType == TCPIP_DNS_EVENT_NAME_RESOLVED)
        {   // mark entry as solved
            (void)F_DNSCompleteHashEntry(pDnsDcpt, dnsHE);
        }
        else
        {
            // do nothing
            // if(evType == TCPIP_DNS_EVENT_NAME_ERROR && dnsHE != NULL)
            // Remove name if "No Such name"
            // (void)TCPIP_DNS_RemoveEntry(dnsHE->pHostName);
        }
    }
    else if (evDbgType != TCPIP_DNS_DBG_EVENT_NONE)
    {
        F_DNS_DbgEvent(pDnsDcpt, dnsHE, evDbgType);
    }
    else
    {
        // do nothing
    }

    return !procFail;
}
// This function writes a string to a buffer, ensuring that it is
// properly formatted.
static void F_DNSPutString(uint8_t** wrPtr, const char* string)
{
    const char *rightPtr;
    char    ch;
    int     len;
    uint8_t ulen;
    uint8_t *pPutDnsStr = *wrPtr;

    rightPtr = string;

    while(true)
    {
        do
        {
            ch = (char)*rightPtr++;
        }while((ch != '\0') && (ch != '.') && (ch != '/') && (ch != ',') && (ch != '>'));

        // Put the length and data
        // Also, skip over the '.' in the input string
        len = rightPtr - string - 1;
        TCPIPStack_Assert(0 <= len && len < 256, __FILE__, __func__, __LINE__);

        ulen = (uint8_t)len;
        *pPutDnsStr++ = ulen;

        (void)memcpy((char*)pPutDnsStr, string, ulen);
        pPutDnsStr = pPutDnsStr + ulen;

        string += ulen + 1U;

        if(ch == '\0' || ch == '/' || ch == ',' || ch == '>')
        {
            break;
        }
    }

    // Put the string null terminator character (zero length label)
    *pPutDnsStr++ = 0;
    *wrPtr = pPutDnsStr;
}

// Reads a name string or string pointer from the DNS buffer
// Saves the name in the supplied buffer
// nameBuff should be buffSize + 1 characters in size!
// Each string consists of a series of labels.
// Each label consists of a length prefix byte, followed by the label bytes.
// At the end of the string, a zero length label is found as termination.
// If name compression is used, this function will automatically detect the pointer
// 
// returns the size of the assembled name or -1 if error
static int F_DNS_ReadName(TCPIP_DNS_RR_PROCESS* pProc, char* nameBuff, size_t buffSize)
{
    uint8_t labelLen, offset;
    uint16_t labelOffset;
    int     iLabelLen, copyLen, discardLen, avlblLen, nameLen;
    char    *wPtr, *ePtr;
    TCPIP_DNS_RX_DATA   pktBuff;    // for compressed names

    if(nameBuff != NULL && buffSize != 0U)
    {
        wPtr = nameBuff;
        ePtr = nameBuff + buffSize;
        *wPtr = '\0';
    }
    else
    {
        ePtr = wPtr = NULL;
    }


    TCPIP_DNS_RX_DATA* xtractBuff = pProc->dnsRxData;
    bool nameFail = false;
    nameLen = 0;


    while(true)
    {
        // Get first byte which will tell us if this is a 16-bit pointer or the
        // length of the first of a series of labels
        labelLen = 0U;
        if(!F_DNSGetData(xtractBuff, &labelLen, sizeof(labelLen)))
        {   // failed to get label
            nameFail = true;
            break;
        }

        if(labelLen == 0U)
        {   // no more data or done
            break;
        }

        // Check if this is a pointer, if so, jump to the offset
        if((labelLen & 0xc0U) == 0xc0U)
        {   // get the offset
            labelOffset = ((uint16_t)labelLen & 0x3fU) << 8;
            offset = 0;
            if(!F_DNSGetData(xtractBuff, &offset, sizeof(offset)))
            {   // failed to get the offset
                nameFail = true;
                break;
            }

            labelOffset += offset; 
            // jump with absolute offset in the DNS packet
            xtractBuff = &pktBuff;
            F_DNSInitRxData(xtractBuff, pProc->dnsPacket, pProc->dnsPacketSize);
            if(!F_DNSGetData(xtractBuff, NULL, labelOffset))
            {   // overflow ?
                nameFail = true;
                break;
            }

            // jump to the offset
            continue;
        }

        iLabelLen = (int)labelLen;
        if(wPtr != NULL)
        {
            avlblLen = ePtr - wPtr;
            if(iLabelLen > avlblLen)
            {
                copyLen = avlblLen;
                discardLen = iLabelLen - avlblLen;
            }
            else
            {
                copyLen = iLabelLen;
                discardLen = 0;
            }
        }
        else
        {
            copyLen = 0;
            discardLen = iLabelLen;
        }

        if(copyLen != 0)
        {
            TCPIPStack_Assert(copyLen >= 0, __FILE__, __func__, __LINE__);
            if(!F_DNSGetData(xtractBuff, wPtr, (size_t)copyLen))
            {
                nameFail = true;
                break;
            }
            wPtr += copyLen;
            if(wPtr < ePtr)
            {
                *wPtr++ = '.';
                nameLen++;
            }
        }
        if(discardLen != 0)
        {
            TCPIPStack_Assert(discardLen >= 0, __FILE__, __func__, __LINE__);
            if(!F_DNSGetData(xtractBuff, NULL, (size_t)discardLen))
            {
                nameFail = true;
                break;
            }
        }

        nameLen += copyLen + discardLen;
    }

    if(nameFail)
    {
        return -1;
    }

    if(wPtr != NULL)
    {
        if(wPtr != nameBuff && *(wPtr - 1) == '.')
        {
            wPtr--; // remove the last '.' 
            nameLen--;
        }

        *wPtr = '\0';  // end the nameBuff properly
    }


    return nameLen;
}

static size_t TCPIP_DNS_OAHASH_KeyHash(OA_HASH_DCPT* pOH, const void* key)
{
    const uint8_t *dnsHostNameKey;
    size_t      hostnameLen=0;

    dnsHostNameKey = (const uint8_t *)key;
    hostnameLen = strlen((const char*)dnsHostNameKey);
    return fnv_32_hash(dnsHostNameKey, hostnameLen) % (pOH->hEntries);
}


static OA_HASH_ENTRY* TCPIP_DNS_OAHASH_DeleteEntry(OA_HASH_DCPT* pOH)
{
    OA_HASH_ENTRY*  pBkt;
    size_t      bktIx;
    TCPIP_DNS_HASH_ENTRY  *pE;
    TCPIP_DNS_DCPT        *pDnsDcpt;
    uint32_t        currTime;
    uint32_t        timeout;

    pDnsDcpt = pgDnsDcpt;
    currTime = pDnsDcpt->dnsTime;

    for(bktIx = 0; bktIx < pOH->hEntries; bktIx++)
    {
        pBkt = TCPIP_OAHASH_EntryGet(pOH, bktIx);       
        if(pBkt->flags.busy != 0U && (pBkt->flags.value & (uint16_t)TCPIP_DNS_FLAG_ENTRY_COMPLETE) != 0U)
        {
            pE = FC_OaHash2DnsHash(pBkt);
            timeout = (pDnsDcpt->cacheEntryTMO > 0U) ? pDnsDcpt->cacheEntryTMO : pE->ipTTL.Val;

            if((currTime - pE->tInsert) >= timeout)
            {
                F_DNSNotifyClients(pDnsDcpt, pE, TCPIP_DNS_EVENT_NAME_REMOVED);
                return pBkt;
            }
        }
    }
    return NULL;
}


static int TCPIP_DNS_OAHASH_KeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key)
{
    TCPIP_DNS_HASH_ENTRY  *pDnsHE;
    const uint8_t         *dnsHostNameKey;

    pDnsHE = FC_OaHash2DnsHash(hEntry);
    dnsHostNameKey = (const uint8_t*)key;    
    
    return strcmp(pDnsHE->pHostName, (const char*)dnsHostNameKey);
}

static void TCPIP_DNS_OAHASH_KeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key)
{
    if(key == NULL) 
    {
        return;
    }
    
    TCPIP_DNS_HASH_ENTRY* pDnsHE = FC_OaHash2DnsHash(dstEntry);
    const char* dnsHostNameKey = (const char*)key;

    size_t hostnameLen = strlen(dnsHostNameKey);
    if(hostnameLen > (size_t)TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN) 
    {  
        hostnameLen = (size_t)TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN;
    }

    (void)memcpy(pDnsHE->pHostName, dnsHostNameKey, hostnameLen);
    pDnsHE->pHostName[hostnameLen] = '\0';
}

#if defined(OA_DOUBLE_HASH_PROBING)
static size_t TCPIP_DNS_OAHASH_ProbeHash(OA_HASH_DCPT* pOH, const void* key)
{
    uint8_t    *dnsHostNameKey;
    size_t      hostnameLen=0;
    
    dnsHostNameKey = (uint8_t  *)key;
    hostnameLen = strlen(dnsHostNameKey);
    return fnv_32a_hash(dnsHostNameKey, hostnameLen) % (pOH->hEntries);
}
#endif  // defined(OA_DOUBLE_HASH_PROBING)

// Register an DNS event handler
// Use hNet == 0 to register on all interfaces available
// Returns a valid handle if the call succeeds,
// or a null handle if the call failed.
// Function has to be called after the DNS is initialized
// The hParam is passed by the client and will be used by the DNS when the notification is made.
// It is used for per-thread content or if more modules, for example, share the same handler
// and need a way to differentiate the callback.
#if (TCPIP_DNS_CLIENT_USER_NOTIFICATION != 0)
TCPIP_DNS_HANDLE TCPIP_DNS_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_DNS_EVENT_HANDLER handler, const void* hParam)
{
    TCPIP_DNS_DCPT* pDnsDcpt = pgDnsDcpt;

    if(pDnsDcpt != NULL && handler != NULL && pDnsDcpt->memH != NULL)
    {
        TCPIP_DNS_LIST_NODE dnsNode;
        dnsNode.handler = handler;
        dnsNode.hParam = hParam;
        dnsNode.hNet = hNet;
        dnsNode.next = NULL;

        SGL_LIST_NODE* node = TCPIP_Notification_Add(&pDnsDcpt->dnsRegisteredUsers, pDnsDcpt->memH, &dnsNode, sizeof(dnsNode));
        return FC_Sgl2DnsNode(node);
    }
    return NULL;
}

// deregister the event handler
bool TCPIP_DNS_HandlerDeRegister(TCPIP_DNS_HANDLE hDns)
{
    TCPIP_DNS_DCPT* pDnsDcpt = pgDnsDcpt;
    if(pDnsDcpt != NULL && pDnsDcpt->memH != NULL && hDns != NULL)
    {
        if(TCPIP_Notification_Remove(FC_DnsHndl2SglNode(hDns), &pDnsDcpt->dnsRegisteredUsers, pDnsDcpt->memH))
        {
            return true;
        }
    }
    return false;
}
#else
TCPIP_DNS_HANDLE TCPIP_DNS_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_DNS_EVENT_HANDLER handler, const void* hParam)
{
    return NULL;
}
bool TCPIP_DNS_HandlerDeRegister(TCPIP_DNS_HANDLE hDns)
{
    return false;
}
#endif  // (TCPIP_DNS_CLIENT_USER_NOTIFICATION != 0)

static void F_DNSNotifyClients(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* pDnsHE, TCPIP_DNS_EVENT_TYPE evType)
{
    F_DNS_DbgEvent(pDnsDcpt, pDnsHE, (TCPIP_DNS_DBG_EVENT_TYPE)evType);

#if (TCPIP_DNS_CLIENT_USER_NOTIFICATION != 0)
    if(pDnsHE != NULL)
    {
        TCPIP_DNS_LIST_NODE* dNode;
        bool     triggerNotify;

        TCPIP_Notification_Lock(&pDnsDcpt->dnsRegisteredUsers);
        for(dNode = FC_Sgl2DnsNode(pDnsDcpt->dnsRegisteredUsers.list.head); dNode != NULL; dNode = dNode->next)
        {
            if(dNode->hNet == NULL || dNode->hNet == pDnsHE->currNet)
            {   // trigger event
                triggerNotify = dNode->hParam == NULL ? true : strcmp(dNode->hParam, pDnsHE->pHostName) == 0;
                if(triggerNotify)
                {
                    (*dNode->handler)(pDnsHE->currNet, evType, pDnsHE->pHostName, dNode->hParam);
                }
            }
        }    
        TCPIP_Notification_Unlock(&pDnsDcpt->dnsRegisteredUsers);
    }
#endif  // (TCPIP_DNS_CLIENT_USER_NOTIFICATION != 0)
}

bool TCPIP_DNS_IsEnabled(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);
    if(pNetIf != NULL)
    {
        return pNetIf->Flags.bIsDnsClientEnabled != 0U;
    }
    return false;
}

bool TCPIP_DNS_Enable(TCPIP_NET_HANDLE hNet, TCPIP_DNS_ENABLE_FLAGS flags)
{
    return F_DNS_Enable(hNet, true, flags);
}

static bool F_DNS_Enable(TCPIP_NET_HANDLE hNet, bool checkIfUp, TCPIP_DNS_ENABLE_FLAGS flags)
{
    TCPIP_DNS_DCPT        *pDnsDcpt;
    TCPIP_NET_IF    *pNetIf;

    pDnsDcpt = pgDnsDcpt;
    if(pDnsDcpt == NULL)
    {
        return false;
    }

    if(checkIfUp)
    {
        pNetIf = TCPIPStackHandleToNetUp(hNet);
    }
    else
    {
        pNetIf = TCPIPStackHandleToNet(hNet);
    }

    if(pNetIf == NULL || TCPIP_STACK_DNSServiceCanStart(pNetIf, TCPIP_STACK_DNS_SERVICE_CLIENT) == false)
    {
        return false;
    }

    pNetIf->Flags.bIsDnsClientEnabled = 1U;
    if(flags == TCPIP_DNS_ENABLE_STRICT)
    {
        pDnsDcpt->strictNet =  pNetIf;
    }
    else if(flags == TCPIP_DNS_ENABLE_PREFERRED)
    {
        pDnsDcpt->prefNet =  pNetIf;
    }
    else
    {
        // do nothing
    }

    return true;
}

bool TCPIP_DNS_Disable(TCPIP_NET_HANDLE hNet, bool clearCache)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNet(hNet);
    TCPIP_DNS_DCPT *pDnsDcpt;

    pDnsDcpt = pgDnsDcpt;
    if(pDnsDcpt == NULL || pNetIf == NULL)
    {
        return false;
    }

    pNetIf->Flags.bIsDnsClientEnabled = 0U;
    if(pDnsDcpt->strictNet == pNetIf)
    {
        pDnsDcpt->strictNet = NULL;
    }
    if(pDnsDcpt->prefNet == pNetIf)
    {
        pDnsDcpt->prefNet = NULL;
    }

    if(clearCache)
    {
        F_DNS_CleanCache(pDnsDcpt);
    }

    return true;    
}

<#else>
bool TCPIP_DNS_IsEnabled(TCPIP_NET_HANDLE hNet){return false;}
bool TCPIP_DNS_Enable(TCPIP_NET_HANDLE hNet, TCPIP_DNS_ENABLE_FLAGS flags){return false;}
bool TCPIP_DNS_Disable(TCPIP_NET_HANDLE hNet, bool clearCache){return false;}

TCPIP_DNS_RESULT  TCPIP_DNS_Resolve(const char* HostName, TCPIP_DNS_RESOLVE_TYPE Type)
{
    return TCPIP_DNS_RES_NO_SERVICE; 
}

TCPIP_DNS_RESULT  TCPIP_DNS_IsResolved(const char* hostName, IP_MULTI_ADDRESS* hostIP, IP_ADDRESS_TYPE type)
{
    return TCPIP_DNS_RES_NO_SERVICE; 
}

TCPIP_DNS_RESULT  TCPIP_DNS_IsNameResolved(const char* hostName, IPV4_ADDR* hostIPv4, IPV6_ADDR* hostIPv6)
{
    return TCPIP_DNS_RES_NO_SERVICE; 
}

TCPIP_DNS_RESULT TCPIP_DNS_RemoveEntry(const char *hostName)
{
    return TCPIP_DNS_RES_NO_SERVICE;
}

TCPIP_DNS_RESULT TCPIP_DNS_RemoveAll(void)
{
    return TCPIP_DNS_RES_NO_SERVICE;
}

TCPIP_DNS_RESULT TCPIP_DNS_ClientInfoGet(TCPIP_DNS_CLIENT_INFO* pClientInfo)
{
    return TCPIP_DNS_RES_NO_SERVICE;
}

TCPIP_DNS_RESULT TCPIP_DNS_EntryQuery(TCPIP_DNS_ENTRY_QUERY *pDnsQuery, size_t queryIndex)
{
    return TCPIP_DNS_RES_NO_SERVICE;
}

</#if>

