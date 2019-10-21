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








#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_DNS_CLIENT

#include "tcpip/src/tcpip_private.h"

#if defined(TCPIP_STACK_USE_DNS)
#include "tcpip/src/dns_private.h"

    
/****************************************************************************
  Section:
    Constants and Global Variables
  ***************************************************************************/


static TCPIP_DNS_DCPT     gDnsDcpt;
static TCPIP_DNS_DCPT*    pgDnsDcpt = 0;

static int          dnsInitCount = 0;       // module initialization count
/****************************************************************************
  Section:
    Function Prototypes
  ***************************************************************************/

static void                 _DNSNotifyClients(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* pDnsHE, TCPIP_DNS_EVENT_TYPE evType);
static void                 _DNSPutString(uint8_t **putbuf, const char* string);
static void                 _DNSDiscardName(TCPIP_DNS_RX_DATA* srcBuff);
static int                  _DNSGetData(TCPIP_DNS_RX_DATA* srcBuff, uint8_t *destBuff, int bytes);
static bool                 _DNS_SelectIntf(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* pDnsHE);
static bool                 _DNS_Enable(TCPIP_NET_HANDLE hNet, bool checkIfUp, TCPIP_DNS_ENABLE_FLAGS flags);
static void                 _DNS_DeleteHash(TCPIP_DNS_DCPT* pDnsDcpt);
static TCPIP_DNS_RESULT     _DNS_Send_Query(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* pDnsHE);
static TCPIP_DNS_RESULT     _DNS_Resolve(const char* hostName, TCPIP_DNS_RESOLVE_TYPE type, bool forceQuery);
static bool                 _DNS_ProcessPacket(TCPIP_DNS_DCPT* pDnsDcpt);
static  TCPIP_DNS_RESULT    _DNSCompleteHashEntry(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* dnsHE);
static  void                _DNS_CleanCache(TCPIP_DNS_DCPT* pDnsDcpt);
static TCPIP_DNS_RESULT     _DNS_IsNameResolved(const char* hostName, IPV4_ADDR* hostIPv4, IPV6_ADDR* hostIPv6, bool singleAddress);
static bool                 _DNS_ValidateIf(TCPIP_NET_IF* pIf, TCPIP_DNS_HASH_ENTRY* pDnsHE, bool wrapAround);
static bool                 _DNS_AddSelectionIf(TCPIP_NET_IF* pIf, TCPIP_NET_IF** dnsIfTbl, int tblEntries);
static bool                 _DNS_NetIsValid(TCPIP_NET_IF* pIf);

static void                 TCPIP_DNS_ClientProcess(bool isTmo);
static void                 TCPIP_DNS_CacheTimeout(TCPIP_DNS_DCPT* pDnsDcpt);
static void                 _DNSSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param);
static bool                 _DNSClientSocketOpen(TCPIP_DNS_DCPT* pDnsDcpt);
#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void                 _DNSClientSocketClose(TCPIP_DNS_DCPT* pDnsDcpt);
static void                 _DNSClientCleanup(TCPIP_DNS_DCPT* pDnsDcpt);
#else
#define _DNSClientCleanup(pDnsDcpt)
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)
static TCPIP_DNS_HASH_ENTRY *_DNSHashEntryFromTransactionId(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_UINT16_VAL transactionId);
static bool                 _DNS_RESPONSE_HashEntryUpdate(TCPIP_DNS_RX_DATA* dnsRxData, TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* dnsHE);
static int                  _DNS_GetAddresses(const char* hostName, int startIndex, IP_MULTI_ADDRESS* pIPAddr, int nIPAddresses, TCPIP_DNS_ADDRESS_REC_MASK recMask);



#if ((TCPIP_DNS_DEBUG_LEVEL & TCPIP_DNS_DEBUG_MASK_BASIC) != 0)
volatile int _DNSStayAssertLoop = 0;
static void _DNSAssertCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("DNS Assert: %s, in line: %d, \r\n", message, lineNo);
        while(_DNSStayAssertLoop != 0);
    }
}
// a debug condition, not really assertion
volatile int _DNSStayCondLoop = 0;
static void _DNSDbgCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("DNS Cond: %s, in line: %d, \r\n", message, lineNo);
        while(_DNSStayCondLoop != 0);
    }
}

#else
#define _DNSAssertCond(cond, message, lineNo)
#define _DNSDbgCond(cond, message, lineNo)
#endif  // (TCPIP_DNS_DEBUG_LEVEL & TCPIP_DNS_DEBUG_MASK_BASIC)

#if ((TCPIP_DNS_DEBUG_LEVEL & TCPIP_DNS_DEBUG_MASK_EVENTS) != 0)
static const char* _DNSDbg_EvNameTbl[] = 
{
    // general events
    "none",         // TCPIP_DNS_DBG_EVENT_NONE
    "query",        // TCPIP_DNS_DBG_EVENT_NAME_QUERY
    "solved",       // TCPIP_DNS_DBG_EVENT_NAME_RESOLVED
    "expired",      // TCPIP_DNS_DBG_EVENT_NAME_EXPIRED
    "removed",      // TCPIP_DNS_DBG_EVENT_NAME_REMOVED
    "noname",       // TCPIP_DNS_DBG_EVENT_NAME_ERROR
    "skt error",    // TCPIP_DNS_DBG_EVENT_SOCKET_ERROR
    "no if",        // TCPIP_DNS_DBG_EVENT_NO_INTERFACE
    // debug events
    "id error",     // TCPIP_DNS_DBG_EVENT_ID_ERROR 
    "solved error", // TCPIP_DNS_DBG_EVENT_COMPLETE_ERROR 
    "ip error",     // TCPIP_DNS_DBG_EVENT_NO_IP_ERROR 
    "unsolicited packet",   // TCPIP_DNS_DBG_EVENT_UNSOLICITED_ERROR 
};

static void _DNS_DbgEvent(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* pDnsHE, TCPIP_DNS_DBG_EVENT_TYPE evType)
{
    _DNSAssertCond(0 <= evType && evType <= TCPIP_DNS_DBG_EVENT_UNSOLICITED_ERROR, __func__, __LINE__);
    const char* hostName = (pDnsHE != 0) ? pDnsHE->pHostName : "no host";
    const char* ifName = (pDnsHE != 0 && pDnsHE->currNet != 0) ? TCPIP_STACK_NetNameGet(pDnsHE->currNet) : "no if";

    const char* evName = _DNSDbg_EvNameTbl[evType];
    SYS_CONSOLE_PRINT("DNS Event: %s, IF: %s, host: %s, time: %d\r\n", evName, ifName, hostName, pDnsDcpt->dnsTime);
}
#else
#define _DNS_DbgEvent(pDnsDcpt, pDnsHE, evType)
#endif  // ((TCPIP_DNS_DEBUG_LEVEL & TCPIP_DNS_DEBUG_MASK_EVENTS) != 0)


/*****************************************************************************
 swap DNS Header content packet . This API is used when we recive
 the DNS response for a query from the server.
  ***************************************************************************/
static void _SwapDNSPacket(TCPIP_DNS_HEADER * p)
{
    p->TransactionID.Val = TCPIP_Helper_htons(p->TransactionID.Val);
    p->Flags.Val = TCPIP_Helper_htons(p->Flags.Val);
    p->AdditionalRecords.Val = TCPIP_Helper_htons(p->AdditionalRecords.Val);
    p->Answers.Val = TCPIP_Helper_htons(p->Answers.Val);
    p->AuthoritativeRecords.Val = TCPIP_Helper_htons(p->AuthoritativeRecords.Val);
    p->Questions.Val = TCPIP_Helper_htons(p->Questions.Val);
}

static void _SwapDNSAnswerPacket(TCPIP_DNS_ANSWER_HEADER * p)
{
    p->ResponseClass.Val = TCPIP_Helper_htons(p->ResponseClass.Val);
    p->ResponseLen.Val = TCPIP_Helper_htons(p->ResponseLen.Val);
    p->ResponseTTL.Val = TCPIP_Helper_htonl(p->ResponseTTL.Val);
    p->ResponseType.Val = TCPIP_Helper_htons(p->ResponseType.Val);
}

static void _DNS_CleanCacheEntry(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* pDnsHE)
{
    if(pDnsHE->hEntry.flags.busy)
    {
        if((pDnsHE->hEntry.flags.value & TCPIP_DNS_FLAG_ENTRY_COMPLETE) == 0)
        {   // deleting an unsolved entry
            pDnsDcpt->unsolvedEntries--;
            _DNSAssertCond(pDnsDcpt->unsolvedEntries >= 0, __func__, __LINE__);
        }
        TCPIP_OAHASH_EntryRemove(pDnsDcpt->hashDcpt, &pDnsHE->hEntry);
        pDnsHE->nIPv4Entries = pDnsHE->nIPv6Entries = 0;
    }
}

static  void _DNS_CleanCache(TCPIP_DNS_DCPT* pDnsDcpt)
{
    size_t          bktIx;
    TCPIP_DNS_HASH_ENTRY* pE;
    OA_HASH_DCPT*   pOh;
    
    if((pOh = pDnsDcpt->hashDcpt) != 0)
    {
        for(bktIx = 0; bktIx < pOh->hEntries; bktIx++)
        {
            pE = (TCPIP_DNS_HASH_ENTRY*)TCPIP_OAHASH_EntryGet(pOh, bktIx);
            _DNS_CleanCacheEntry(pDnsDcpt, pE);
        }
    }
}

static void _DNS_UpdateExpiredHashEntry_Notify(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* pDnsHE)
{
    // Notify to to the specific Host name that it is expired.
    _DNSNotifyClients(pDnsDcpt, pDnsHE, TCPIP_DNS_EVENT_NAME_EXPIRED);
    _DNS_CleanCacheEntry(pDnsDcpt, pDnsHE);
}

static  TCPIP_DNS_RESULT  _DNSCompleteHashEntry(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* dnsHE)
{
     
    dnsHE->hEntry.flags.value &= ~TCPIP_DNS_FLAG_ENTRY_TIMEOUT;
    dnsHE->hEntry.flags.value |= TCPIP_DNS_FLAG_ENTRY_COMPLETE;
    dnsHE->recordMask = TCPIP_DNS_ADDRESS_REC_NONE;

    if(dnsHE->nIPv4Entries != 0)
    {
        dnsHE->recordMask |= TCPIP_DNS_ADDRESS_REC_IPV4;
    }
    if(dnsHE->nIPv6Entries != 0)
    {
        dnsHE->recordMask |= TCPIP_DNS_ADDRESS_REC_IPV6;
    }

    if(dnsHE->ipTTL.Val == 0)
    {
        dnsHE->ipTTL.Val = TCPIP_DNS_CLIENT_CACHE_DEFAULT_TTL_VAL;
    }
    dnsHE->tRetry = dnsHE->tInsert = pDnsDcpt->dnsTime; 
    pDnsDcpt->unsolvedEntries--;
    _DNSAssertCond(pDnsDcpt->unsolvedEntries >= 0, __func__, __LINE__);

    return TCPIP_DNS_RES_OK;
}

static  void _DNSDeleteCacheEntries(TCPIP_DNS_DCPT* pDnsDcpt)
{
    size_t          bktIx;
    TCPIP_DNS_HASH_ENTRY* pE;
    OA_HASH_DCPT*   pOh;
    
    if((pOh = pDnsDcpt->hashDcpt) != 0)
    {
        for(bktIx = 0; bktIx < pOh->hEntries; bktIx++)
        {
            pE = (TCPIP_DNS_HASH_ENTRY*)TCPIP_OAHASH_EntryGet(pOh, bktIx);
            TCPIP_OAHASH_EntryRemove(pOh, &pE->hEntry);
            TCPIP_HEAP_Free(pDnsDcpt->memH, pE->memblk);
            memset(pE, 0, sizeof(*pE));
        }
    }
}

// deletes the associated DNS client hash
static void _DNS_DeleteHash(TCPIP_DNS_DCPT* pDnsDcpt)
{
    _DNSDeleteCacheEntries(pDnsDcpt);
    TCPIP_HEAP_Free(pDnsDcpt->memH, pDnsDcpt->hashDcpt);
    pDnsDcpt->hashDcpt = 0;
    pDnsDcpt->unsolvedEntries = 0;
}

/****************************************************************************
  Section:
    Implementation
  ***************************************************************************/

bool TCPIP_DNS_ClientInitialize(const TCPIP_STACK_MODULE_CTRL* const stackData,
                       const TCPIP_DNS_CLIENT_MODULE_CONFIG* dnsData)
{
    OA_HASH_DCPT    *hashDcpt;
    size_t          hashMemSize;
    uint32_t        memoryBlockSize;
    int             hashCnt;
    uint8_t         *pMemoryBlock;
    OA_HASH_ENTRY   *pBkt;
    TCPIP_DNS_HASH_ENTRY  *pE;
    
    if(stackData->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        if(stackData->pNetIf->Flags.bIsDnsClientEnabled != 0)
        {   // enable DNS client service
            _DNS_Enable(stackData->pNetIf, false, TCPIP_DNS_ENABLE_DEFAULT);
        }
        return true;
    }

    if(dnsInitCount == 0)
    {   // stack start up; initialize just once
        bool iniRes;
        TCPIP_DNS_DCPT* pDnsDcpt = &gDnsDcpt;
        memset(pDnsDcpt, 0, sizeof(*pDnsDcpt));

        if(dnsData == 0)
        {
            return false;
        }

        pDnsDcpt->memH = stackData->memH;
        hashMemSize = sizeof(OA_HASH_DCPT) + dnsData->cacheEntries * sizeof(TCPIP_DNS_HASH_ENTRY);
        hashDcpt = (OA_HASH_DCPT*)TCPIP_HEAP_Malloc(pDnsDcpt->memH, hashMemSize);
        if(hashDcpt == 0)
        {   // failed
            return false;
        }  
        // populate the entries
        memset(hashDcpt, 0, hashMemSize);
        hashDcpt->memBlk = hashDcpt + 1;
        hashDcpt->hParam = hashDcpt;    // store the descriptor it belongs to
        hashDcpt->hEntrySize = sizeof(TCPIP_DNS_HASH_ENTRY);
        hashDcpt->hEntries = dnsData->cacheEntries;
        hashDcpt->probeStep = TCPIP_DNS_HASH_PROBE_STEP;

        hashDcpt->hashF = TCPIP_DNS_OAHASH_KeyHash;
        hashDcpt->delF = TCPIP_DNS_OAHASH_DeleteEntry;
        hashDcpt->cmpF = TCPIP_DNS_OAHASH_KeyCompare;
        hashDcpt->cpyF = TCPIP_DNS_OAHASH_KeyCopy;
#if defined(OA_DOUBLE_HASH_PROBING)
        hashDcpt->probeHash = TCPIP_DNS_OAHASH_ProbeHash;
#endif  // defined(OA_DOUBLE_HASH_PROBING)

        TCPIP_OAHASH_Initialize(hashDcpt);
        pDnsDcpt->hashDcpt = hashDcpt;
        pDnsDcpt->dnsSocket =  INVALID_UDP_SOCKET;
        pDnsDcpt->cacheEntryTMO = dnsData->entrySolvedTmo;
        pDnsDcpt->nIPv4Entries= dnsData->nIPv4Entries;
        pDnsDcpt->nIPv6Entries = dnsData->nIPv6Entries;
#if defined (TCPIP_STACK_USE_IPV4)
        pDnsDcpt->ipAddressType = IP_ADDRESS_TYPE_IPV4;     // dnsData->ipAddressType;
#else
        pDnsDcpt->ipAddressType = IP_ADDRESS_TYPE_IPV6;     // dnsData->ipAddressType;
#endif // defined (TCPIP_STACK_USE_IPV6)

        // allocate memory for each DNS hostname , IPv4 address and IPv6 address
        // and the allocation will be done per Hash descriptor
        memoryBlockSize = pDnsDcpt->nIPv4Entries * sizeof(IPV4_ADDR)
            + pDnsDcpt->nIPv6Entries * sizeof(IPV6_ADDR)
            + TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN ;
        for(hashCnt = 0; hashCnt < dnsData->cacheEntries; hashCnt++)
        {
            pBkt = TCPIP_OAHASH_EntryGet(hashDcpt, hashCnt);

            pE = (TCPIP_DNS_HASH_ENTRY*)pBkt;
            pMemoryBlock = (uint8_t *)TCPIP_HEAP_Malloc(pDnsDcpt->memH, memoryBlockSize);
            if((pE->memblk = pMemoryBlock) == 0)
            {
                _DNS_DeleteHash(pDnsDcpt);
                return false;
            }
            pE->pHostName = 0;
            pE->pip4Address = 0;
            pE->pip6Address = 0;
            // set memory for IPv4 entries
            if(pDnsDcpt->nIPv4Entries)
            {
                pE->pip4Address = (IPV4_ADDR *)pMemoryBlock;
                pMemoryBlock += pDnsDcpt->nIPv4Entries * (sizeof(IPV4_ADDR));
            }
            // set memory for IPv6 entries
            if(pDnsDcpt->nIPv6Entries)
            {
                pE->pip6Address = (IPV6_ADDR *)pMemoryBlock;
                pMemoryBlock += pDnsDcpt->nIPv6Entries * (sizeof(IPV6_ADDR));
            }
            // allocate Hostname
            if(TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN != 0)
            {
                pE->pHostName = (char*)pMemoryBlock;
            }
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

            if((pDnsDcpt->dnsSignalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_DNS_ClientTask, TCPIP_DNS_CLIENT_TASK_PROCESS_RATE)) == 0)
            {
                break;
            }
            // create the DNS socket
            if(!_DNSClientSocketOpen(pDnsDcpt))
            {   // failed
                break;
            }
                
            // success
            iniRes = true;
            break;
        }        

        if(iniRes == false)
        {
            _DNSClientCleanup(pDnsDcpt);
            return false;
        }

        // module is initialized and pgDnsDcpt is valid!
        pgDnsDcpt = &gDnsDcpt;
    }

    if(stackData->pNetIf->Flags.bIsDnsClientEnabled != 0)
    {   // enable DNS client service
        _DNS_Enable(stackData->pNetIf, false, TCPIP_DNS_ENABLE_DEFAULT);
    }
    dnsInitCount++;
    return true;
}

static bool _DNSClientSocketOpen(TCPIP_DNS_DCPT* pDnsDcpt)
{
    UDP_SOCKET dnsSocket;

    if(pDnsDcpt == 0)
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

        dnsSocket = TCPIP_UDP_ClientOpen(pDnsDcpt->ipAddressType, TCPIP_DNS_SERVER_PORT, 0);
        if(dnsSocket == INVALID_UDP_SOCKET)
        {
            break;
        }

        const unsigned int minDnsTxSize = 18 + TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN + 1;
        bufferSize = TCPIP_UDP_TxPutIsReady(dnsSocket, minDnsTxSize);
        if(bufferSize < minDnsTxSize)
        {
            if(!TCPIP_UDP_OptionsSet(dnsSocket, UDP_OPTION_TX_BUFF, (void*)minDnsTxSize))
            {
                break;
            }
        }

        TCPIP_UDP_OptionsSet(dnsSocket, UDP_OPTION_STRICT_ADDRESS, (void*)false);
        if(TCPIP_UDP_SignalHandlerRegister(dnsSocket, TCPIP_UDP_SIGNAL_RX_DATA, _DNSSocketRxSignalHandler, 0) == 0)
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
        TCPIP_UDP_Close(dnsSocket);
    }

    return success;
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void _DNSClientSocketClose(TCPIP_DNS_DCPT* pDnsDcpt)
{
    if(pDnsDcpt->dnsSocket != INVALID_UDP_SOCKET)
    {
        TCPIP_UDP_Close(pDnsDcpt->dnsSocket);
        pDnsDcpt->dnsSocket = INVALID_UDP_SOCKET;
    }
}


static void _DNSClientCleanup(TCPIP_DNS_DCPT* pDnsDcpt)
{
    _DNSClientSocketClose(pDnsDcpt);

    // Remove dns Timer Handle
    if( pDnsDcpt->dnsSignalHandle)
    {
       _TCPIPStackSignalHandlerDeregister( pDnsDcpt->dnsSignalHandle);
        pDnsDcpt->dnsSignalHandle = 0;
    }
    // Remove DNS register users
#if (TCPIP_DNS_CLIENT_USER_NOTIFICATION != 0)
    TCPIP_Notification_Deinitialize(&pDnsDcpt->dnsRegisteredUsers, pDnsDcpt->memH);
#endif  // (TCPIP_DNS_CLIENT_USER_NOTIFICATION != 0)

    // Delete Hash Entries
    _DNS_DeleteHash(pDnsDcpt);
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
                TCPIP_UDP_Disconnect(pDnsDcpt->dnsSocket, true);
            }
        }

        if(stackData->stackAction == TCPIP_STACK_ACTION_DEINIT)
        {   // stack shut down
            if(--dnsInitCount == 0)
            {   // all closed and Release DNS client Hash resources
                _DNSClientCleanup(pDnsDcpt);
                // module is de-initialized and pgDnsDcpt is invalid!
                pgDnsDcpt = 0;
            }
        }
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

TCPIP_DNS_RESULT TCPIP_DNS_Resolve(const char* hostName, TCPIP_DNS_RESOLVE_TYPE type)
{
    return _DNS_Resolve(hostName, type, false);
}

TCPIP_DNS_RESULT TCPIP_DNS_Send_Query(const char* hostName, TCPIP_DNS_RESOLVE_TYPE type)
{
    return _DNS_Resolve(hostName, type, true);
}

static TCPIP_DNS_RESULT _DNS_Resolve(const char* hostName, TCPIP_DNS_RESOLVE_TYPE type, bool forceQuery)
{
    TCPIP_DNS_DCPT            *pDnsDcpt;
    TCPIP_DNS_HASH_ENTRY      *dnsHE;
    IP_MULTI_ADDRESS    ipAddr;
    TCPIP_DNS_ADDRESS_REC_MASK recMask;

    pDnsDcpt = pgDnsDcpt;

    if(pDnsDcpt == 0)
    {
        return TCPIP_DNS_RES_NO_SERVICE;
    }

    if(hostName == 0 || strlen(hostName) == 0 || strlen(hostName)  >= TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN)
    {
        return TCPIP_DNS_RES_INVALID_HOSTNAME; 
    }

    if(TCPIP_Helper_StringToIPAddress(hostName, &ipAddr.v4Add) || TCPIP_Helper_StringToIPv6Address (hostName, &ipAddr.v6Add))
    {   // DNS request is a valid IPv4 or IPv6 address
        return  TCPIP_DNS_RES_NAME_IS_IPADDRESS;
    }
 
    dnsHE = (TCPIP_DNS_HASH_ENTRY*)TCPIP_OAHASH_EntryLookupOrInsert(pDnsDcpt->hashDcpt, (void*)hostName);
    if(dnsHE == 0)
    {   // no more entries
        return TCPIP_DNS_RES_CACHE_FULL; 
    }

    if(type == TCPIP_DNS_TYPE_A)
    {
        recMask = TCPIP_DNS_ADDRESS_REC_IPV4;
    }
    else if(type == TCPIP_DNS_TYPE_AAAA)
    {
        recMask = TCPIP_DNS_ADDRESS_REC_IPV6;
    }
    else
    {
        recMask = TCPIP_DNS_ADDRESS_REC_IPV4 | TCPIP_DNS_ADDRESS_REC_IPV6;
    }

    if(forceQuery == 0 && dnsHE->hEntry.flags.newEntry == 0)
    {   // already in hash
        if((dnsHE->recordMask & recMask) == recMask)
        {   // already have the requested type
            if((dnsHE->hEntry.flags.value & TCPIP_DNS_FLAG_ENTRY_COMPLETE) != 0)
            {
               return TCPIP_DNS_RES_OK; 
            }
            return (dnsHE->hEntry.flags.value & TCPIP_DNS_FLAG_ENTRY_TIMEOUT) == 0 ? TCPIP_DNS_RES_PENDING : TCPIP_DNS_RES_SERVER_TMO; 
        }
        // else new query is needed, for new type
    }

    // this is a forced/new entry/query
    // update entry parameters
    if(dnsHE->hEntry.flags.newEntry != 0)
    {
        dnsHE->nIPv4Entries = 0;
        dnsHE->nIPv6Entries = 0;
        dnsHE->hEntry.flags.value &= ~(TCPIP_DNS_FLAG_ENTRY_COMPLETE | TCPIP_DNS_FLAG_ENTRY_TIMEOUT);
    }
    else
    {   // forced
        if((recMask & TCPIP_DNS_ADDRESS_REC_IPV4) != 0)
        {
            dnsHE->nIPv4Entries = 0;
        }
        if((recMask & TCPIP_DNS_ADDRESS_REC_IPV6) != 0)
        {
            dnsHE->nIPv6Entries = 0;
        }
        dnsHE->hEntry.flags.value &= ~TCPIP_DNS_FLAG_ENTRY_COMPLETE;
    }
    dnsHE->ipTTL.Val = 0;
    dnsHE->currNet = 0;     // new search will re-select the interface and server
    dnsHE->currServerIx = 0;
    dnsHE->resolve_type = type;
    dnsHE->recordMask |= recMask;
    dnsHE->tRetry = dnsHE->tInsert = pDnsDcpt->dnsTime;
    pDnsDcpt->unsolvedEntries++;
    return _DNS_Send_Query(pDnsDcpt, dnsHE);
}

static int _DNS_GetAddresses(const char* hostName, int startIndex, IP_MULTI_ADDRESS* pIPAddr, int nIPAddresses, TCPIP_DNS_ADDRESS_REC_MASK recMask)
{
    TCPIP_DNS_DCPT*     pDnsDcpt;
    TCPIP_DNS_HASH_ENTRY*    dnsHashEntry;
    IPV4_ADDR*       pDst4Addr;
    IPV4_ADDR*       pSrc4Addr;
    IPV6_ADDR*       pDst6Addr;
    IPV6_ADDR*       pSrc6Addr;
    int              nAddrs, ix;

    pDnsDcpt = pgDnsDcpt;
    if(pDnsDcpt == 0 || hostName == 0 || pIPAddr == 0 || nIPAddresses == 0)
    {
        return 0;
    }

    dnsHashEntry = (TCPIP_DNS_HASH_ENTRY*)TCPIP_OAHASH_EntryLookup(pDnsDcpt->hashDcpt, hostName);
    if(dnsHashEntry == 0 || (dnsHashEntry->hEntry.flags.value & TCPIP_DNS_FLAG_ENTRY_COMPLETE) == 0)
    {
        return 0;
    }

    recMask &= (TCPIP_DNS_ADDRESS_REC_MASK)dnsHashEntry->recordMask;

    if(recMask == 0)
    {
        return 0; 
    }

    nAddrs = 0;
    if(recMask == TCPIP_DNS_ADDRESS_REC_IPV4)
    {
        pDst4Addr = &pIPAddr->v4Add;
        pSrc4Addr =  dnsHashEntry->pip4Address + startIndex;
        for(ix = startIndex; ix <= dnsHashEntry->nIPv4Entries && nAddrs < nIPAddresses; ix++, nAddrs++, pDst4Addr++, pSrc4Addr++)
        {
            pDst4Addr->Val = pSrc4Addr->Val;
        }
    }
    else
    {   // TCPIP_DNS_ADDRESS_REC_IPV6
        pDst6Addr = &pIPAddr->v6Add;
        pSrc6Addr =  dnsHashEntry->pip6Address + startIndex;
        for(ix = startIndex; ix <= dnsHashEntry->nIPv6Entries && nAddrs < nIPAddresses; ix++, nAddrs++, pDst6Addr++)
        {
            memcpy(pDst6Addr->v, pSrc6Addr->v, sizeof(*pDst6Addr));
        }
    }

    return nAddrs;
}

int TCPIP_DNS_GetIPv4Addresses(const char* hostName, int startIndex, IPV4_ADDR* pIPv4Addr, int nIPv4Addresses)
{
    return _DNS_GetAddresses(hostName, startIndex, (IP_MULTI_ADDRESS*)pIPv4Addr, nIPv4Addresses, TCPIP_DNS_ADDRESS_REC_IPV4);
}

int TCPIP_DNS_GetIPv6Addresses(const char* hostName, int startIndex, IPV6_ADDR* pIPv6Addr, int nIPv6Addresses)
{
    return _DNS_GetAddresses(hostName, startIndex, (IP_MULTI_ADDRESS*)pIPv6Addr, nIPv6Addresses, TCPIP_DNS_ADDRESS_REC_IPV6);
}

int TCPIP_DNS_GetIPAddressesNumber(const char* hostName, IP_ADDRESS_TYPE type)
{
    TCPIP_DNS_DCPT*     pDnsDcpt;
    TCPIP_DNS_HASH_ENTRY*    dnsHashEntry;
    OA_HASH_ENTRY*   hE;
    TCPIP_DNS_ADDRESS_REC_MASK recMask;
    int         nAddresses;
    
    pDnsDcpt = pgDnsDcpt;
    if(pDnsDcpt == 0 || hostName == 0)
    {
        return 0;
    }

    if(type == IP_ADDRESS_TYPE_IPV4)
    {
        recMask = TCPIP_DNS_ADDRESS_REC_IPV4;
    }
    else if(type == IP_ADDRESS_TYPE_IPV6)
    {
        recMask = TCPIP_DNS_ADDRESS_REC_IPV6;
    }
    else
    {
        recMask = TCPIP_DNS_ADDRESS_REC_IPV4 | TCPIP_DNS_ADDRESS_REC_IPV6;
    }


    nAddresses = 0;
    hE = TCPIP_OAHASH_EntryLookup(pDnsDcpt->hashDcpt, hostName);
    if(hE != 0)
    {
        if(hE->flags.value & TCPIP_DNS_FLAG_ENTRY_COMPLETE)
        {
            dnsHashEntry = (TCPIP_DNS_HASH_ENTRY*)hE;
            if(((dnsHashEntry->recordMask & recMask) & TCPIP_DNS_ADDRESS_REC_IPV4) != 0)
            {
                nAddresses += dnsHashEntry->nIPv4Entries;
            }

            if(((dnsHashEntry->recordMask & recMask) & TCPIP_DNS_ADDRESS_REC_IPV6) != 0)
            {
                 nAddresses += dnsHashEntry->nIPv6Entries;
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
        hostIPv6 = 0;
    }
    else if(type == IP_ADDRESS_TYPE_IPV6)
    {
        hostIPv6 = &hostIP->v6Add;
        hostIPv4 = 0;
    }
    else
    {
        hostIPv4 = &hostIP->v4Add;
        hostIPv6 = &hostIP->v6Add;
    }

    return _DNS_IsNameResolved(hostName, hostIPv4, hostIPv6, true);
}

TCPIP_DNS_RESULT  TCPIP_DNS_IsNameResolved(const char* hostName, IPV4_ADDR* hostIPv4, IPV6_ADDR* hostIPv6)
{
    return _DNS_IsNameResolved(hostName, hostIPv4, hostIPv6, false);
}

// retrieves the IP addresses corresponding to the hostName
// if singleAddress, then only one address is returned, either IPv4 or IPv6
static TCPIP_DNS_RESULT  _DNS_IsNameResolved(const char* hostName, IPV4_ADDR* hostIPv4, IPV6_ADDR* hostIPv6, bool singleAddress)
{    
    TCPIP_DNS_DCPT*     pDnsDcpt;
    TCPIP_DNS_HASH_ENTRY* pDnsHE;
    int                         nIPv4Entries;
    int                         nIPv6Entries;
    IP_MULTI_ADDRESS            mAddr;            

    if(hostIPv4)
    {
        hostIPv4->Val = 0;
    }
    if(hostIPv6)
    {
        memset(hostIPv6->v, 0, sizeof(*hostIPv6));
    }

    pDnsDcpt = pgDnsDcpt;

    if(pDnsDcpt == 0)
    {
        return TCPIP_DNS_RES_NO_SERVICE;
    }
    
    if(TCPIP_Helper_StringToIPAddress(hostName, &mAddr.v4Add))
    {   // name id a IPv4 address
        if(hostIPv4)
        {
            hostIPv4->Val = mAddr.v4Add.Val;
        }
        return  TCPIP_DNS_RES_OK; 
    }
    if (TCPIP_Helper_StringToIPv6Address (hostName, &mAddr.v6Add))
    {   // name is a IPv6 address
        if(hostIPv6)
        {
            memcpy (hostIPv6->v, mAddr.v6Add.v, sizeof (IPV6_ADDR));
        }
        return  TCPIP_DNS_RES_OK; 
    }
    
    pDnsHE = (TCPIP_DNS_HASH_ENTRY*)TCPIP_OAHASH_EntryLookup(pDnsDcpt->hashDcpt, hostName);
    if(pDnsHE == 0)
    {
        return TCPIP_DNS_RES_NO_NAME_ENTRY;
    }

    if((pDnsHE->hEntry.flags.value & TCPIP_DNS_FLAG_ENTRY_COMPLETE) == 0)
    {   // unsolved entry   
        return (pDnsHE->hEntry.flags.value & TCPIP_DNS_FLAG_ENTRY_TIMEOUT) == 0 ? TCPIP_DNS_RES_PENDING : TCPIP_DNS_RES_SERVER_TMO; 
    }

    // completed entry
    nIPv6Entries = pDnsHE->nIPv6Entries;
    nIPv4Entries = pDnsHE->nIPv4Entries;

    if(nIPv6Entries || nIPv4Entries)
    {
        if(nIPv6Entries)
        {
            if(hostIPv6)
            {
                memcpy (hostIPv6->v, pDnsHE->pip6Address + nIPv6Entries - 1, sizeof (IPV6_ADDR));
                if(singleAddress)
                {   //  retrieve only one address
                    nIPv4Entries = 0;
                }
            }
        }

        if(nIPv4Entries)
        {
            if(hostIPv4)
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

    if(pClientInfo)
    {
        pClientInfo->strictNet = pDnsDcpt->strictNet;
        pClientInfo->prefNet = pDnsDcpt->prefNet;
        pClientInfo->dnsTime = pDnsDcpt->dnsTime;
        pClientInfo->pendingEntries = pDnsDcpt->unsolvedEntries;
        pClientInfo->currentEntries = pDnsDcpt->hashDcpt->fullSlots;
        pClientInfo->totalEntries = pDnsDcpt->hashDcpt->hEntries;
    }
    return TCPIP_DNS_RES_OK;
}

TCPIP_DNS_RESULT TCPIP_DNS_EntryQuery(TCPIP_DNS_ENTRY_QUERY *pDnsQuery, int queryIndex)
{
    OA_HASH_ENTRY*  pBkt;
    TCPIP_DNS_HASH_ENTRY  *pE;
    TCPIP_DNS_DCPT        *pDnsDcpt;
    int             ix;
    uint32_t        currTime;

    pDnsDcpt = pgDnsDcpt;

    if(pDnsDcpt == 0 || pDnsDcpt->hashDcpt == 0)
    {
        return TCPIP_DNS_RES_NO_SERVICE;
    }

    if(pDnsQuery == 0 || pDnsQuery->hostName == 0 || pDnsQuery->nameLen == 0)
    {
        return TCPIP_DNS_RES_INVALID_HOSTNAME;
    }

    pBkt = TCPIP_OAHASH_EntryGet(pDnsDcpt->hashDcpt, queryIndex);
    if(pBkt == 0)
    {
        return TCPIP_DNS_RES_NO_IX_ENTRY;
    }

    if(pBkt->flags.busy != 0)
    {
        pE = (TCPIP_DNS_HASH_ENTRY*)pBkt;
        strncpy(pDnsQuery->hostName, pE->pHostName, pDnsQuery->nameLen - 1);
        pDnsQuery->hostName[pDnsQuery->nameLen - 1] = 0;
        pDnsQuery->hNet = pE->currNet;
        pDnsQuery->serverIx = pE->currServerIx;

        if((pBkt->flags.value & TCPIP_DNS_FLAG_ENTRY_COMPLETE) != 0)
        {
            pDnsQuery->status = TCPIP_DNS_RES_OK;
            currTime = pDnsDcpt->dnsTime;
            if(pDnsDcpt->cacheEntryTMO > 0)
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
                memcpy(pDnsQuery->ipv6Entry[ix].v, pE->pip6Address[ix].v, sizeof(IPV6_ADDR));
            }
            pDnsQuery->nIPv6ValidEntries = ix;

            return TCPIP_DNS_RES_OK;
        }
        else
        {
            pDnsQuery->status = (pE->hEntry.flags.value & TCPIP_DNS_FLAG_ENTRY_TIMEOUT) == 0 ? TCPIP_DNS_RES_PENDING : TCPIP_DNS_RES_SERVER_TMO; 
            pDnsQuery->ttlTime = 0;
            pDnsQuery->nIPv4ValidEntries = 0;
            pDnsQuery->nIPv6ValidEntries = 0;
        }
        return TCPIP_DNS_RES_OK;
    }

    return TCPIP_DNS_RES_EMPTY_IX_ENTRY;
}

// selects a interface for a DNS hash entry transaction 
static bool _DNS_SelectIntf(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* pDnsHE)
{
    int             ix;
    int             nIfs;
    TCPIP_NET_IF*   pDnsIf;
    TCPIP_NET_IF*   dnsSelectIfs[TCPIP_DNS_CLIENT_MAX_SELECT_INTERFACES];    // interfaces to select for DNS


    if(pDnsDcpt->strictNet != 0)
    {   // only the strict interface is checked
        return _DNS_ValidateIf(pDnsDcpt->strictNet, pDnsHE, true);
    }

    memset(dnsSelectIfs, 0, sizeof(dnsSelectIfs));

    // add the interfaces to be considered
    // the preferred interface
    _DNS_AddSelectionIf(pDnsDcpt->prefNet, dnsSelectIfs, sizeof(dnsSelectIfs) / sizeof(*dnsSelectIfs));
    // the default interface
    _DNS_AddSelectionIf((TCPIP_NET_IF*)TCPIP_STACK_NetDefaultGet(), dnsSelectIfs, sizeof(dnsSelectIfs) / sizeof(*dnsSelectIfs));
    // and any other interface
    nIfs = TCPIP_STACK_NumberOfNetworksGet();
    for(ix = 0; ix < nIfs; ix++)
    {
       if(!_DNS_AddSelectionIf((TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(ix), dnsSelectIfs, sizeof(dnsSelectIfs) / sizeof(*dnsSelectIfs)))
       {
           break;
       }
    }

    // calculate the number of interfaces we have
    nIfs = 0;
    for(ix = 0; ix < sizeof(dnsSelectIfs) / sizeof(*dnsSelectIfs); ix++)
    {
        if(dnsSelectIfs[ix] != 0)
        {
            nIfs++;
        }
    }

    // search an interface
    for(ix = 0; ix < sizeof(dnsSelectIfs) / sizeof(*dnsSelectIfs); ix++)
    {
        if((pDnsIf = dnsSelectIfs[ix]) != 0)
        {   // for the last valid interface allow DNS servers wrap around
            if(_DNS_ValidateIf(pDnsIf, pDnsHE, (ix == nIfs - 1) ? true : false))
            {
                return true;
            }
        }
    }

    // couldn't find a valid interface
    pDnsHE->currNet = 0;    // make sure next time we start with a fresh interface
    return false;
}

// inserts a DNS valid interface into the dnsIfTbl
// returns false if table is full and interface could not be added
// true if OK
// Interface must support DNS traffic:
// up and running, configured and have valid DNS servers 
static bool _DNS_AddSelectionIf(TCPIP_NET_IF* pIf, TCPIP_NET_IF** dnsIfTbl, int tblEntries)
{
    int ix, addIx;

    if(pIf && _DNS_NetIsValid(pIf))
    {
        addIx = -1;
        for(ix = 0; ix < tblEntries; ix++)
        {
            if(dnsIfTbl[ix] == pIf)
            {   // already there
                return true;
            }
            if(dnsIfTbl[ix] == 0 && addIx < 0)
            {   // insert slot
                addIx = ix;
            }
        }

        // pIf is not in the table
        if(addIx >= 0)
        {
            dnsIfTbl[addIx] = pIf;
        }
        else
        {   // table full
            return false;
        }
    }

    return true;
}

// returns true if the pIf can be selected for DNS traffic
// false otherwise
// it sets the current interface and server index too
// if a retry (i.e. the entry already has that interface) the server index is advanced too
// if wrapAround, the server index is cleared to 0
static bool _DNS_ValidateIf(TCPIP_NET_IF* pIf, TCPIP_DNS_HASH_ENTRY* pDnsHE, bool wrapAround)
{
    int ix, startIx;
    bool    srvFound;

    if(_DNS_NetIsValid(pIf))
    {
        srvFound = false;
        startIx = (pDnsHE->currNet == pIf) ?  pDnsHE->currServerIx + 1 : 0;
        for(ix = startIx; ix < sizeof(pIf->dnsServer) / sizeof(*pIf->dnsServer); ix++)
        {
            if(pIf->dnsServer[ix].Val != 0)
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
                if(pIf->dnsServer[ix].Val != 0)
                {   // all good; select new interface
                    srvFound = true;
                    break;
                }
            }
        }

        if(srvFound)
        {
            pDnsHE->currNet = pIf;
            pDnsHE->currServerIx = ix;
            return true;
        }
    }

    return false;
}

// returns true if a network interface can be selected for DNS traffic
// false otherwise
static bool _DNS_NetIsValid(TCPIP_NET_IF* pIf)
{

    if(TCPIP_STACK_NetIsReady(pIf) != 0)
    {   // interface is up, linked and configured
        if(_TCPIPStackNetAddress(pIf) != 0)
        {   // has a valid address
            if(pIf->Flags.bIsDnsClientEnabled != 0)
            {   // DNS enabled on this interface
                return pIf->dnsServer[0].Val != 0 || pIf->dnsServer[1].Val != 0;
            }
        }
    }

    return false;
}
// send a signal to the DNS module that data is available
// no manager alert needed since this normally results as a higher layer (UDP) signal
static void _DNSSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_UDP_SIGNAL_RX_DATA)
    {
        _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}

static TCPIP_DNS_RESULT _DNS_Send_Query(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* pDnsHE)
{
    TCPIP_DNS_HEADER    DNSPutHeader;
    uint8_t             *wrPtr, *startPtr;
    uint16_t            sktPayload;
    TCPIP_DNS_EVENT_TYPE evType;
    TCPIP_DNS_RESULT    res;
    IPV4_ADDR           dnsServerAdd;
    UDP_SOCKET          dnsSocket = pDnsDcpt->dnsSocket;
    
    pDnsHE->hEntry.flags.value &= ~TCPIP_DNS_FLAG_ENTRY_COMPLETE;

    while(true)
    {
        if(!TCPIP_UDP_PutIsReady(dnsSocket))
        {   // failed to allocate another TX buffer
            res = TCPIP_DNS_RES_SOCKET_ERROR;
            evType = TCPIP_DNS_EVENT_SOCKET_ERROR;
            break; 
        }

        if(!_DNS_SelectIntf(pDnsDcpt, pDnsHE))
        {   // couldn't get an output interface
            res = TCPIP_DNS_RES_NO_INTERFACE;
            evType = TCPIP_DNS_EVENT_NO_INTERFACE;
            break; 
        }

        // this will put the start pointer at the beginning of the TX buffer
        TCPIP_UDP_TxOffsetSet(dnsSocket, 0, false);    

        //Get the write pointer:
        wrPtr = TCPIP_UDP_TxPointerGet(dnsSocket);
        if(wrPtr == 0)
        {
            res = TCPIP_DNS_RES_SOCKET_ERROR;
            evType = TCPIP_DNS_EVENT_SOCKET_ERROR;
            break; 
        }

        // set up the socket
        TCPIP_UDP_Bind(dnsSocket, IP_ADDRESS_TYPE_IPV4, 0, (IP_MULTI_ADDRESS*)&pDnsHE->currNet->netIPAddr);
        dnsServerAdd.Val = pDnsHE->currNet->dnsServer[pDnsHE->currServerIx].Val;
        TCPIP_UDP_DestinationIPAddressSet(dnsSocket, pDnsDcpt->ipAddressType, (IP_MULTI_ADDRESS*)&dnsServerAdd);
        TCPIP_UDP_DestinationPortSet(dnsSocket, TCPIP_DNS_SERVER_PORT);

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
        DNSPutHeader.Answers.Val = DNSPutHeader.AuthoritativeRecords.Val = DNSPutHeader.AdditionalRecords.Val = 0;

        // copy the DNS header to the UDP buffer
        memcpy(wrPtr, &DNSPutHeader, sizeof(TCPIP_DNS_HEADER));
        wrPtr += sizeof(TCPIP_DNS_HEADER);

        // Put hostname string to resolve
        _DNSPutString(&wrPtr, pDnsHE->pHostName);

        // Type: TCPIP_DNS_TYPE_A A (host address) or TCPIP_DNS_TYPE_MX for mail exchange
        *wrPtr++ = 0x00;
        *wrPtr++ = pDnsHE->resolve_type;

        // Class: IN (Internet)
        *wrPtr++ = 0x00;
        *wrPtr++ = 0x01; // 0x0001
    
        // Put complete DNS query packet buffer to the UDP buffer
        // Once it is completed writing into the buffer, you need to update the Tx offset again,
        // because the socket flush function calculates how many bytes are in the buffer using the current write pointer:
        sktPayload = (uint16_t)(wrPtr - startPtr);
        TCPIP_UDP_TxOffsetSet(dnsSocket, sktPayload, false);

        if(TCPIP_UDP_Flush(dnsSocket) != sktPayload)
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
    _DNSNotifyClients(pDnsDcpt, pDnsHE, evType);
    return res;
}

TCPIP_DNS_RESULT TCPIP_DNS_RemoveEntry(const char *hostName)
{
    TCPIP_DNS_HASH_ENTRY  *pDnsHE;
    TCPIP_DNS_DCPT        *pDnsDcpt;

    pDnsDcpt = pgDnsDcpt;
    if(pDnsDcpt == 0 || pDnsDcpt->hashDcpt == 0)
    {
        return TCPIP_DNS_RES_NO_SERVICE;
    }

    if(hostName == NULL)
    {
        return TCPIP_DNS_RES_INVALID_HOSTNAME;
    }

    pDnsHE = (TCPIP_DNS_HASH_ENTRY*)TCPIP_OAHASH_EntryLookup(pDnsDcpt->hashDcpt, hostName);
    if(pDnsHE != 0)
    {
        _DNS_UpdateExpiredHashEntry_Notify(pDnsDcpt, pDnsHE);
        return TCPIP_DNS_RES_OK;
    }

    return TCPIP_DNS_RES_NO_NAME_ENTRY;
}

TCPIP_DNS_RESULT TCPIP_DNS_RemoveAll(void)
{
    OA_HASH_ENTRY*  pBkt;
    int             bktIx;
    TCPIP_DNS_DCPT        *pDnsDcpt;
    OA_HASH_DCPT*   pOh;

    pDnsDcpt = pgDnsDcpt;

    if(pDnsDcpt == 0 || pDnsDcpt->hashDcpt == 0)
    {
        return TCPIP_DNS_RES_NO_SERVICE;
    }

    pOh = pDnsDcpt->hashDcpt;
    for(bktIx = 0; bktIx < pOh->hEntries; bktIx++)
    {
        pBkt = TCPIP_OAHASH_EntryGet(pOh, bktIx);
        if(pBkt != 0)
        {
            if(pBkt->flags.busy != 0)
            {
                _DNS_UpdateExpiredHashEntry_Notify(pDnsDcpt, (TCPIP_DNS_HASH_ENTRY*)pBkt);
            }
        }
    }

    return TCPIP_DNS_RES_OK;
}

void TCPIP_DNS_ClientTask(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != 0)
    {   // signal: TMO/RX occurred
        TCPIP_DNS_ClientProcess((sigPend & TCPIP_MODULE_SIGNAL_TMO) != 0);
    }

}

static void TCPIP_DNS_CacheTimeout(TCPIP_DNS_DCPT* pDnsDcpt)
{
    TCPIP_DNS_HASH_ENTRY  *pDnsHE;
    int             bktIx;
    OA_HASH_DCPT    *pOH;
    uint32_t        currTime;
    uint32_t        timeout;

    // get current time: seconds
    currTime = pDnsDcpt->dnsTime;
    pOH = pDnsDcpt->hashDcpt;

    // check the lease values; remove the expired leases
    for(bktIx = 0; bktIx < pOH->hEntries; bktIx++)
    {
        pDnsHE = (TCPIP_DNS_HASH_ENTRY*)TCPIP_OAHASH_EntryGet(pOH, bktIx);
        if(pDnsHE->hEntry.flags.busy != 0)
        {   // not empty hash slot
            if((pDnsHE->hEntry.flags.value & TCPIP_DNS_FLAG_ENTRY_COMPLETE) != 0)
            {   // solved entry: check timeout
                // if cacheEntryTMO is equal to zero, then TTL time is the timeout period. 
                if((timeout = pDnsDcpt->cacheEntryTMO) == 0)
                {
                    timeout = pDnsHE->ipTTL.Val;
                }
            }
            else
            {   // unsolved entry
                timeout = TCPIP_DNS_CLIENT_CACHE_UNSOLVED_ENTRY_TMO;
            }

            if((currTime - pDnsHE->tInsert) >= timeout)
            {
                _DNS_UpdateExpiredHashEntry_Notify(pDnsDcpt, pDnsHE);
            }
            else if((pDnsHE->hEntry.flags.value & TCPIP_DNS_FLAG_ENTRY_COMPLETE) == 0 && (currTime - pDnsHE->tRetry) >= TCPIP_DNS_CLIENT_LOOKUP_RETRY_TMO)
            {   // send further probes for unsolved entries
                _DNS_Send_Query(pDnsDcpt, pDnsHE);
                pDnsHE->hEntry.flags.value |= TCPIP_DNS_FLAG_ENTRY_TIMEOUT;
                pDnsHE->tRetry = currTime;
            }
        }
    } 
}


static void TCPIP_DNS_ClientProcess(bool isTmo)
{
    TCPIP_DNS_DCPT*     pDnsDcpt;
  
    if((pDnsDcpt = pgDnsDcpt) == 0)
    {   // nothing to do
        return;
    }

    if(isTmo)
    {   // maintain the cache timeouts
        pDnsDcpt->dnsTime = SYS_TMR_TickCountGetLong() / SYS_TMR_TickCounterFrequencyGet();
        TCPIP_DNS_CacheTimeout(pDnsDcpt);
    }

    while(true)
    {
        if(!TCPIP_UDP_GetIsReady(pDnsDcpt->dnsSocket))
        {   // done
            break;
        }

        _DNSDbgCond(pDnsDcpt->unsolvedEntries != 0, __func__, __LINE__);
        if(pDnsDcpt->unsolvedEntries != 0)
        {   // waiting for a reply; process the packet
            _DNS_ProcessPacket(pDnsDcpt);
        }
        else
        {
            _DNS_DbgEvent(pDnsDcpt, 0, TCPIP_DNS_DBG_EVENT_UNSOLICITED_ERROR);
        }

        TCPIP_UDP_Discard(pDnsDcpt->dnsSocket);
    }

}

// extracts the IPv4/IPv6 addresses and updates the hash entry
// returns true if processing could continue
// false otherwise (already have the number of required addresses, etc.)
static bool _DNS_RESPONSE_HashEntryUpdate(TCPIP_DNS_RX_DATA* dnsRxData, TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* dnsHE)
{
    TCPIP_DNS_ANSWER_HEADER DNSAnswerHeader;    
    IP_MULTI_ADDRESS        ipAddr;
    bool                    canContinue;
    bool                    discardData;


    _DNSDiscardName(dnsRxData); // Throw away response name

    _DNSGetData(dnsRxData, (uint8_t *)&DNSAnswerHeader, sizeof(TCPIP_DNS_ANSWER_HEADER));
    _SwapDNSAnswerPacket(&DNSAnswerHeader);

    // Make sure that this is a 4 byte IP address, response type A or MX, class 1
    // Check if this is Type A, MX, or AAAA
    canContinue = true;
    discardData = true;

    while( DNSAnswerHeader.ResponseClass.Val == 0x0001u) // Internet class
    {
        if (DNSAnswerHeader.ResponseType.Val == TCPIP_DNS_TYPE_A && DNSAnswerHeader.ResponseLen.Val == 4)
        {            
            // read the buffer
            _DNSGetData(dnsRxData, ipAddr.v4Add.v, sizeof(IPV4_ADDR));
            discardData = false;
            if(dnsHE->nIPv4Entries >= pDnsDcpt->nIPv4Entries)
            {   // have enough entries
                canContinue = false;
                break;
            }
            // update the Hash entry for IPv4 address
            dnsHE->pip4Address[dnsHE->nIPv4Entries].Val = ipAddr.v4Add.Val;
            if((DNSAnswerHeader.ResponseTTL.Val < dnsHE->ipTTL.Val) || (dnsHE->ipTTL.Val == 0))
            {
                dnsHE->ipTTL.Val = DNSAnswerHeader.ResponseTTL.Val;
            }
            dnsHE->nIPv4Entries++;
            break;
        }

        if (DNSAnswerHeader.ResponseType.Val == TCPIP_DNS_TYPE_AAAA && DNSAnswerHeader.ResponseLen.Val == 16)
        {
            if((dnsHE->recordMask & TCPIP_DNS_ADDRESS_REC_IPV6) == 0)
            {
                break;
            }           

            // read the buffer
            _DNSGetData(dnsRxData, ipAddr.v6Add.v, sizeof (IPV6_ADDR));
            discardData = false;
            if(dnsHE->nIPv6Entries >= pDnsDcpt->nIPv6Entries)
            {   // have enough entries
                canContinue = false;
                break;
            }
            // update the Hash entry for IPv6 address
            memcpy( &dnsHE->pip6Address[dnsHE->nIPv6Entries], ipAddr.v6Add.v, sizeof(IPV6_ADDR));
            if((DNSAnswerHeader.ResponseTTL.Val < dnsHE->ipTTL.Val) || (dnsHE->ipTTL.Val == 0))
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
        _DNSGetData(dnsRxData, 0, DNSAnswerHeader.ResponseLen.Val);
    }
    return canContinue;
}

static TCPIP_DNS_HASH_ENTRY* _DNSHashEntryFromTransactionId(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_UINT16_VAL transactionId)
{
    TCPIP_DNS_HASH_ENTRY    *pDnsHE;
    int                     bktIx;
    OA_HASH_DCPT            *pOH;
    
    pOH = pDnsDcpt->hashDcpt;

    for(bktIx = 0; bktIx < pOH->hEntries; bktIx++)
    {
        pDnsHE = (TCPIP_DNS_HASH_ENTRY*)TCPIP_OAHASH_EntryGet(pOH, bktIx);
        if(pDnsHE->hEntry.flags.busy != 0)
        {
            if(pDnsHE->transactionId.Val == transactionId.Val)
            {
                return pDnsHE;
            }
        }
    }
    return 0;
}

// retrieves a number of bytes from a source buffer and copies the data into the supplied destination buffer (if !0)
// returns the number of bytes removed from the source data buffer
// upodates the source buffer descriptor
static int _DNSGetData(TCPIP_DNS_RX_DATA* srcBuff, uint8_t *destBuff, int bytes)
{
    int nBytes = srcBuff->endPtr - srcBuff->wrPtr;

    if(bytes < nBytes)
    {
        nBytes =  bytes;
    }

    if(destBuff && nBytes)
    {
        memcpy(destBuff, srcBuff->wrPtr, nBytes);
    }

    srcBuff->wrPtr += nBytes;

    return nBytes;
}

// process a DNS packet
// returns true if info updated
// false if no entry was completed
static bool _DNS_ProcessPacket(TCPIP_DNS_DCPT* pDnsDcpt)
{
    TCPIP_DNS_HEADER        DNSHeader;
    TCPIP_DNS_HASH_ENTRY*   dnsHE;
    int                     dnsPacketSize;
    TCPIP_DNS_RX_DATA       dnsRxData;
    uint8_t                 dnsRxBuffer[TCPIP_DNS_RX_BUFFER_SIZE];
    bool                    procRes;
    TCPIP_DNS_EVENT_TYPE    evType = TCPIP_DNS_EVENT_NONE;
    TCPIP_DNS_DBG_EVENT_TYPE evDbgType = TCPIP_DNS_DBG_EVENT_NONE;


    // Get DNS Reply packet
    dnsPacketSize = TCPIP_UDP_ArrayGet(pDnsDcpt->dnsSocket, dnsRxBuffer, sizeof(dnsRxBuffer));

    dnsRxData.head = dnsRxBuffer;
    dnsRxData.wrPtr = dnsRxData.head;
    dnsRxData.endPtr = dnsRxData.head + dnsPacketSize;

    // Retrieve the DNS header and de-big-endian it
    memcpy((void*)&DNSHeader, dnsRxData.wrPtr, sizeof(DNSHeader));
    // Shift get pointer to the next poniter to get the new value
    dnsRxData.wrPtr = dnsRxData.wrPtr + sizeof(DNSHeader);
    // Swap DNS Header received packet
    _SwapDNSPacket(&DNSHeader);

    while(true)
    {
        // find DNS HASH entry from transaction ID
        dnsHE = _DNSHashEntryFromTransactionId(pDnsDcpt, DNSHeader.TransactionID);

        // Throw this packet away if no response to our query or name already exists
        if(dnsHE == 0)
        {
            evDbgType = TCPIP_DNS_DBG_EVENT_ID_ERROR;
            procRes = false;
            break;
        }

        if((dnsHE->hEntry.flags.value & TCPIP_DNS_FLAG_ENTRY_COMPLETE) != 0)
        {
            evDbgType = TCPIP_DNS_DBG_EVENT_COMPLETE_ERROR;
            procRes = false;
            break;
        }

        if((DNSHeader.Flags.v[0] & 0x03) != 0)
        {   
            evType = TCPIP_DNS_EVENT_NAME_ERROR;
            procRes = false;
            break;
        }

        // Remove all questions (queries)
        while(DNSHeader.Questions.Val--)
        {
            _DNSDiscardName(&dnsRxData);
            // Ignore Question Type and Question class
            _DNSGetData(&dnsRxData, 0, 4); // Question type class
        }

        // Scan through answers
        while(DNSHeader.Answers.Val--)
        {
            if(!_DNS_RESPONSE_HashEntryUpdate(&dnsRxData, pDnsDcpt, dnsHE))
            {
                break;
            }
        }

        // Remove all Authoritative Records
        while(DNSHeader.AuthoritativeRecords.Val--)
        {
            if(!_DNS_RESPONSE_HashEntryUpdate(&dnsRxData, pDnsDcpt, dnsHE))
            {
                break;
            }
        }

        // Remove all Additional Records
        while(DNSHeader.AdditionalRecords.Val--)
        {
            if(!_DNS_RESPONSE_HashEntryUpdate(&dnsRxData, pDnsDcpt, dnsHE))
            {
                break;
            }
        }

        if(dnsHE->nIPv4Entries > 0 || dnsHE->nIPv6Entries > 0)
        {
            evType = TCPIP_DNS_EVENT_NAME_RESOLVED;
            procRes = true;
        }           
        else
        {
            evDbgType = TCPIP_DNS_DBG_EVENT_NO_IP_ERROR;
            procRes = false;
        }

        break;
    }

    if(evType != TCPIP_DNS_EVENT_NONE)
    {
        _DNSNotifyClients(pDnsDcpt, dnsHE, evType);

        if(evType == TCPIP_DNS_EVENT_NAME_RESOLVED)
        {   // mark entry as solved
            _DNSCompleteHashEntry(pDnsDcpt, dnsHE);
        }
        else if(evType == TCPIP_DNS_EVENT_NAME_ERROR)
        {   // Remove name if "No Such name"
            TCPIP_DNS_RemoveEntry(dnsHE->pHostName);
        }
    }
    else if (evDbgType != TCPIP_DNS_DBG_EVENT_NONE)
    {
        _DNS_DbgEvent(pDnsDcpt, dnsHE, evDbgType);
    }

    return procRes;
}
// This function writes a string to a buffer, ensuring that it is
// properly formatted.
static void _DNSPutString(uint8_t** wrPtr, const char* string)
{
    const char *rightPtr;
    uint8_t i;
    int     len;
    uint8_t *pPutDnsStr = *wrPtr;

    rightPtr = string;

    while(true)
    {
        do
        {
            i = *rightPtr++;
        }while((i != 0) && (i != '.') && (i != '/') && (i != ',') && (i != '>'));

        // Put the length and data
        // Also, skip over the '.' in the input string
        len = rightPtr - string - 1;
        *pPutDnsStr++ = (uint8_t)len;

        memcpy(pPutDnsStr, string, len);
        pPutDnsStr = pPutDnsStr + len;

        string += len + 1;

        if(i == 0 || i == '/' || i == ',' || i == '>')
        {
            break;
        }
    }

    // Put the string null terminator character (zero length label)
    *pPutDnsStr++ = 0;
    *wrPtr = pPutDnsStr;
}

// Reads a name string or string pointer from the DNS socket and discards it.
// Each string consists of a series of labels.
// Each label consists of a length prefix byte, followed by the label bytes.
// At the end of the string, a zero length label is found as termination.
// If name compression is used, this function will automatically detect the pointer and discard it.
static void _DNSDiscardName(TCPIP_DNS_RX_DATA* srcBuff)
{
    uint8_t labelLen;

    while(1)
    {
        // Get first byte which will tell us if this is a 16-bit pointer or the
        // length of the first of a series of labels
        labelLen = *srcBuff->wrPtr++;
        if(labelLen == 0)
        {
            return;
        }
        
        // Check if this is a pointer, if so, get the remaining 8 bits and return
        if((labelLen & 0xc0) == 0xc0)
        {
            srcBuff->wrPtr++;
            return;
        }

        // Ignore these bytes
        _DNSGetData(srcBuff, 0, labelLen);
    }
}

static size_t TCPIP_DNS_OAHASH_KeyHash(OA_HASH_DCPT* pOH, const void* key)
{
    uint8_t    *dnsHostNameKey;
    size_t      hostnameLen=0;

    dnsHostNameKey = (uint8_t *)key;
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
        if(pBkt->flags.busy != 0 && (pBkt->flags.value & TCPIP_DNS_FLAG_ENTRY_COMPLETE) != 0)
        {
            pE = (TCPIP_DNS_HASH_ENTRY*)pBkt;
            timeout = (pDnsDcpt->cacheEntryTMO > 0) ? pDnsDcpt->cacheEntryTMO : pE->ipTTL.Val;

            if((currTime - pE->tInsert) >= timeout)
            {
                _DNSNotifyClients(pDnsDcpt, pE, TCPIP_DNS_EVENT_NAME_REMOVED);
                return pBkt;
            }
        }
    }
    return 0;
}


static int TCPIP_DNS_OAHASH_KeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key)
{
    TCPIP_DNS_HASH_ENTRY  *pDnsHE;
    uint8_t         *dnsHostNameKey;

  
    pDnsHE =(TCPIP_DNS_HASH_ENTRY  *)hEntry;
    dnsHostNameKey = (uint8_t *)key;    
    
    return strcmp(pDnsHE->pHostName, (const char*)dnsHostNameKey);
}

static void TCPIP_DNS_OAHASH_KeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key)
{
    uint8_t    *dnsHostNameKey;
    TCPIP_DNS_HASH_ENTRY  *pDnsHE;
    size_t          hostnameLen=0;

    if(key==NULL) 
    {
        return;
    }
    
    pDnsHE =(TCPIP_DNS_HASH_ENTRY  *)dstEntry;
    dnsHostNameKey = (uint8_t *)key;
    hostnameLen = strlen((const char*)dnsHostNameKey);
    if(hostnameLen>TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN) 
    {
        return;
    }

    if(dnsHostNameKey && pDnsHE->pHostName != 0)
    {
        memset(pDnsHE->pHostName, 0, TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN);
        memcpy(pDnsHE->pHostName, dnsHostNameKey, hostnameLen);
        pDnsHE->pHostName[hostnameLen]='\0';
    }
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

    if(pDnsDcpt && handler && pDnsDcpt->memH)
    {
        TCPIP_DNS_LIST_NODE* newNode = (TCPIP_DNS_LIST_NODE*)TCPIP_Notification_Add(&pDnsDcpt->dnsRegisteredUsers, pDnsDcpt->memH, sizeof(*newNode));
        if(newNode)
        {
            newNode->handler = handler;
            newNode->hParam = hParam;
            newNode->hNet = hNet;
            return newNode;
        }
    }
    return 0;
}

// deregister the event handler
bool TCPIP_DNS_HandlerDeRegister(TCPIP_DNS_HANDLE hDns)
{
    TCPIP_DNS_DCPT* pDnsDcpt = pgDnsDcpt;
    if(pDnsDcpt && pDnsDcpt->memH && hDns)
    {
        if(TCPIP_Notification_Remove((SGL_LIST_NODE*)hDns, &pDnsDcpt->dnsRegisteredUsers, pDnsDcpt->memH))
        {
            return true;
        }
    }
    return false;
}
#endif  // (TCPIP_DNS_CLIENT_USER_NOTIFICATION != 0)

static void _DNSNotifyClients(TCPIP_DNS_DCPT* pDnsDcpt, TCPIP_DNS_HASH_ENTRY* pDnsHE, TCPIP_DNS_EVENT_TYPE evType)
{
    _DNS_DbgEvent(pDnsDcpt, pDnsHE, (TCPIP_DNS_DBG_EVENT_TYPE)evType);

#if (TCPIP_DNS_CLIENT_USER_NOTIFICATION != 0)
    TCPIP_DNS_LIST_NODE* dNode;
    bool     triggerNotify;

    TCPIP_Notification_Lock(&pDnsDcpt->dnsRegisteredUsers);
    for(dNode = (TCPIP_DNS_LIST_NODE*)pDnsDcpt->dnsRegisteredUsers.list.head; dNode != 0; dNode = dNode->next)
    {
        if(dNode->hNet == 0 || dNode->hNet == pDnsHE->currNet)
        {   // trigger event
            triggerNotify = dNode->hParam == 0 ? true : strcmp(dNode->hParam, pDnsHE->pHostName) == 0;
            if(triggerNotify)
            {
                (*dNode->handler)(pDnsHE->currNet, evType, pDnsHE->pHostName, dNode->hParam);
            }
        }
    }    
    TCPIP_Notification_Unlock(&pDnsDcpt->dnsRegisteredUsers);
#endif  // (TCPIP_DNS_CLIENT_USER_NOTIFICATION != 0)
}

bool TCPIP_DNS_IsEnabled(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(hNet);
    if(pNetIf)
    {
        return pNetIf->Flags.bIsDnsClientEnabled != 0;
    }
    return false;
}

bool TCPIP_DNS_Enable(TCPIP_NET_HANDLE hNet, TCPIP_DNS_ENABLE_FLAGS flags)
{
    return _DNS_Enable(hNet, true, flags);
}

static bool _DNS_Enable(TCPIP_NET_HANDLE hNet, bool checkIfUp, TCPIP_DNS_ENABLE_FLAGS flags)
{
    TCPIP_DNS_DCPT        *pDnsDcpt;
    TCPIP_NET_IF    *pNetIf;

    pDnsDcpt = pgDnsDcpt;
    if(pDnsDcpt == 0)
    {
        return false;
    }

    if(checkIfUp)
    {
        pNetIf = _TCPIPStackHandleToNetUp(hNet);
    }
    else
    {
        pNetIf = _TCPIPStackHandleToNet(hNet);
    }

    if(pNetIf == 0 || TCPIP_STACK_DNSServiceCanStart(pNetIf, TCPIP_STACK_DNS_SERVICE_CLIENT) == false)
    {
        return false;
    }

    pNetIf->Flags.bIsDnsClientEnabled = true;      
    if(flags == TCPIP_DNS_ENABLE_STRICT)
    {
        pDnsDcpt->strictNet =  pNetIf;
    }
    else if(flags == TCPIP_DNS_ENABLE_PREFERRED)
    {
        pDnsDcpt->prefNet =  pNetIf;
    }
    return true;
}

bool TCPIP_DNS_Disable(TCPIP_NET_HANDLE hNet, bool clearCache)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNet(hNet);
    TCPIP_DNS_DCPT *pDnsDcpt;

    pDnsDcpt = pgDnsDcpt;
    if(pDnsDcpt == 0 || pNetIf == 0)
    {
        return false;
    }

    pNetIf->Flags.bIsDnsClientEnabled = false;
    if(pDnsDcpt->strictNet == pNetIf)
    {
        pDnsDcpt->strictNet = 0;
    }
    if(pDnsDcpt->prefNet == pNetIf)
    {
        pDnsDcpt->prefNet = 0;
    }

    if(clearCache)
    {
        _DNS_CleanCache(pDnsDcpt);
    }

    return true;    
}

#else
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

TCPIP_DNS_RESULT TCPIP_DNS_EntryQuery(TCPIP_DNS_ENTRY_QUERY *pDnsQuery, int queryIndex)
{
    return TCPIP_DNS_RES_NO_SERVICE;
}

#endif  //#if defined(TCPIP_STACK_USE_DNS)

