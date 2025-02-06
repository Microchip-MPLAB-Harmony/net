/*******************************************************************************
  Domain Name System (DNS) Server dummy

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    - Acts as a DNS server, but gives out the local IP address for all 
      queries to force web browsers to access the board.
    - Reference: RFC 1034 and RFC 1035
*******************************************************************************/

//DOM-IGNORE-BEGIN
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








//DOM-IGNORE-END

#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_DNS_SERVER

#include "tcpip/src/tcpip_private.h"
#include "tcpip/src/dnss_private.h"

#if defined(TCPIP_STACK_USE_DNS_SERVER)


typedef struct
{
    TCPIP_UINT16_VAL wTransactionID;
    TCPIP_UINT16_VAL wFlags;
    TCPIP_UINT16_VAL wQuestions;
    TCPIP_UINT16_VAL wAnswerRRs;
    TCPIP_UINT16_VAL wAuthorityRRs;
    TCPIP_UINT16_VAL wAdditionalRRs;
} DNSS_HEADER;

static DNSS_DCPT gDnsSrvDcpt;
static unsigned int dnsSrvInitCount = 0U;

static void F_DNSCopyRXNameToTX(UDP_SOCKET s);
static  TCPIP_DNSS_RESULT  F_DNSSUpdateHashEntry( DNSS_HASH_ENTRY *dnsSHE,TCPIP_DNSS_CACHE_ENTRY dnssCacheEntry);
static  TCPIP_DNSS_RESULT  F_DNSSSetHashEntry( DNSS_HASH_ENTRY_FLAGS newFlags,TCPIP_DNSS_CACHE_ENTRY dnssCacheEntry);
static void F_DNSSGetRecordType(UDP_SOCKET s,TCPIP_UINT16_VAL *recordType);
static bool TCPIP_DNSS_ValidateIf(const TCPIP_NET_IF* pIf);
static bool F_DNSS_Enable(TCPIP_NET_HANDLE hNet, bool checkIfUp);
static uint8_t TCPIP_DNSS_DataGet(uint16_t pos);
static void TCPIP_DNSS_CacheTimeTask(void);
static void TCPIP_DNSS_Process(void);
static void F_DNSSSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param);

static __inline__ void  __attribute__((always_inline)) TCPIP_DNSS_DataPut(uint8_t* buf, uint16_t pos, uint8_t val)
{
    buf[pos] = val;
}

#if defined(OA_DOUBLE_HASH_PROBING)
size_t TCPIP_OAHASH_DNSS_ProbeHash(OA_HASH_DCPT* pOH, const void* key);
#endif // defined(OA_DOUBLE_HASH_PROBING)

// conversion functions/helpers
//
static __inline__ DNSS_HASH_ENTRY* __attribute__((always_inline)) FC_OaHash2DnssHash(OA_HASH_ENTRY* he)
{
    union
    {
        OA_HASH_ENTRY*    he;
        DNSS_HASH_ENTRY* dnssHe;
    }U_OA_HASH_DNSS_HASH;

    U_OA_HASH_DNSS_HASH.he = he;
    return U_OA_HASH_DNSS_HASH.dnssHe;
}

// Server Need to parse the incoming hostname from client . replace Len with dot
static char hostNameWithDot[TCPIP_DNSS_HOST_NAME_LEN + 1] = {0};
static uint16_t countWithDot=0U;

// Server Need to parse the incoming hostname from client . keep Len and this array will be 
//used while transmitting Name Server response packet
static uint8_t hostNameWithLen[TCPIP_DNSS_HOST_NAME_LEN+1]={0}; 
static uint16_t countWithLen=0U;

static uint8_t  dnsSrvRecvByte[64+1] = {0};
// DNS server received buffer position
static uint16_t gDnsSrvBytePos = 0U;

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static  void F_DNSSRemoveCacheEntries(void);
static void F_DNSS_RemoveHashAll(void)
{
    DNSS_DCPT* pDnsSDcpt=NULL;
    OA_HASH_DCPT *pOhDcpt=NULL;

    pDnsSDcpt = &gDnsSrvDcpt;

    pOhDcpt = pDnsSDcpt->dnssHashDcpt;
    if(pOhDcpt == NULL)
    {
        return;
    }

    F_DNSSRemoveCacheEntries();

    pOhDcpt = NULL;
}
#else
#define F_DNSS_RemoveHashAll()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

bool TCPIP_DNSS_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* initData)
{
    DNSS_DCPT           *pDnsSDcpt=NULL;
    OA_HASH_DCPT        *hashDcpt=NULL;
    size_t              hashMemSize;
    uint32_t            cacheEntries=0;
    uint32_t            memoryBlockSize=0;
    uint8_t             *pMemoryBlock=NULL;
    uint8_t             hashCnt=0;
    OA_HASH_ENTRY       *pBkt=NULL;
    DNSS_HASH_ENTRY     *pE=NULL;

    if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart      
        return true;
    }

    pDnsSDcpt = &gDnsSrvDcpt;
    (void)memset(pDnsSDcpt, 0, sizeof(*pDnsSDcpt));

    if(dnsSrvInitCount == 0U)
    {
        if(initData == NULL)
        {
            return false;
        }

#if !defined(TCPIP_DNSS_HOST_NAME_LEN) || (TCPIP_DNSS_HOST_NAME_LEN == 0)
        return false;
#endif  // !defined(TCPIP_DNSS_HOST_NAME_LEN) || (TCPIP_DNSS_HOST_NAME_LEN == 0)

        const TCPIP_DNSS_MODULE_CONFIG* pDnsSConfig = (const TCPIP_DNSS_MODULE_CONFIG*)initData;
        pDnsSDcpt->memH  = stackCtrl->memH;

        if(pDnsSDcpt->dnssHashDcpt == NULL)
        {
            cacheEntries = pDnsSConfig->IPv4EntriesPerDNSName;
#ifdef TCPIP_STACK_USE_IPV6
            cacheEntries += pDnsSConfig->IPv6EntriesPerDNSName;
#endif
            hashMemSize = sizeof(OA_HASH_DCPT) + cacheEntries * sizeof(DNSS_HASH_ENTRY);
            hashDcpt = (OA_HASH_DCPT*)TCPIP_HEAP_Calloc(pDnsSDcpt->memH,1,hashMemSize);
            if(hashDcpt == NULL)
            {   // failed
                return false;
            }

            // populate the entries
            hashDcpt->memBlk = hashDcpt + 1;
            hashDcpt->hParam = hashDcpt;    // store the descriptor it belongs to
            hashDcpt->hEntrySize = sizeof(DNSS_HASH_ENTRY);
            hashDcpt->hEntries = cacheEntries;
            hashDcpt->probeStep = TCPIP_DNSS_HASH_PROBE_STEP;

            hashDcpt->hashF = &TCPIP_OAHASH_DNSS_KeyHash;
            hashDcpt->delF = &TCPIP_OAHASH_DNSS_EntryDelete;
            hashDcpt->cmpF = &TCPIP_OAHASH_DNSS_KeyCompare;
            hashDcpt->cpyF = &TCPIP_OAHASH_DNSS_KeyCopy;
#if defined(OA_DOUBLE_HASH_PROBING)
            hashDcpt->probeHash = &TCPIP_OAHASH_DNSS_ProbeHash;
#endif  // defined(OA_DOUBLE_HASH_PROBING)

            TCPIP_OAHASH_Initialize(hashDcpt);
            pDnsSDcpt->dnssHashDcpt = hashDcpt;

            pDnsSDcpt->IPv4EntriesPerDNSName = pDnsSConfig->IPv4EntriesPerDNSName;
#ifdef TCPIP_STACK_USE_IPV6
            pDnsSDcpt->IPv6EntriesPerDNSName = pDnsSConfig->IPv6EntriesPerDNSName;
#endif
        }
        pDnsSDcpt->dnsSrvSocket = INVALID_UDP_SOCKET;
        pDnsSDcpt->replyWithBoardInfo = pDnsSConfig->replyBoardAddr ? 1U : 0U;


        if(pDnsSDcpt->dnsSSignalHandle == NULL)
        {   // once per service
            pDnsSDcpt->dnsSSignalHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_DNSS_Task, TCPIP_DNSS_TASK_PROCESS_RATE);
            if(pDnsSDcpt->dnsSSignalHandle != NULL)
            {
                pDnsSDcpt->dnsSTimeMseconds = 0U;
            }
            else
            {
                return false;
            }
        }
        // allocate memory for each DNS hostname , IPv4 address and IPv6 address
            // and the allocation will be done per Hash descriptor
         memoryBlockSize = pDnsSDcpt->IPv4EntriesPerDNSName * sizeof(IPV4_ADDR)
#if defined(TCPIP_STACK_USE_IPV6)
        + pDnsSDcpt->IPv6EntriesPerDNSName * sizeof(IPV6_ADDR)
#endif
        + (uint32_t)TCPIP_DNSS_HOST_NAME_LEN + 1UL;

        for(hashCnt=0;hashCnt < cacheEntries;hashCnt++)
        {
            pBkt = TCPIP_OAHASH_EntryGet(hashDcpt, hashCnt);

            pE = FC_OaHash2DnssHash(pBkt);
            pE->pHostName = NULL;
            pE->pip4Address = NULL;
#if defined(TCPIP_STACK_USE_IPV6)
            pE->pip6Address = NULL;
#endif
            pMemoryBlock = (uint8_t *)TCPIP_HEAP_Calloc(pDnsSDcpt->memH,1,memoryBlockSize);
            if(pMemoryBlock == NULL)
            {
                // if there is any pMemoryBlock already made for other
                // hash entries , we need to remove those also
                F_DNSS_RemoveHashAll();
                return false;
            }

            pE = FC_OaHash2DnssHash(pBkt);
            pE->memblk = pMemoryBlock;

            // if IPv4EntriesPerDNSName != 0, then allocate memory for IPv4 entries
            if(pDnsSDcpt->IPv4EntriesPerDNSName != 0U)
            {
                pE->pip4Address = FC_U8Ptr2Ip4Add(pMemoryBlock);
            }
#if defined(TCPIP_STACK_USE_IPV6)
            if(pDnsSDcpt->IPv6EntriesPerDNSName != 0U)
            {
                pE->pip6Address = FC_U8Ptr2Ip6Add(pMemoryBlock + pDnsSDcpt->IPv4EntriesPerDNSName * sizeof(IPV4_ADDR));
            }
#endif
            // allocate Hostname
            pE->pHostName = (uint8_t *)(pMemoryBlock + (pDnsSDcpt->IPv4EntriesPerDNSName * (sizeof(IPV4_ADDR)))
#if defined(TCPIP_STACK_USE_IPV6)
                    + (pDnsSDcpt->IPv6EntriesPerDNSName * (sizeof(IPV6_ADDR)))
#endif
            );
        }

        dnsSrvInitCount++;
    }

    if(stackCtrl->pNetIf->Flags.bIsDnsServerEnabled != 0U)
    {
        (void)F_DNSS_Enable(stackCtrl->pNetIf,false);
    }
    
    return true;
}

static bool F_DNSS_SendResponse(DNSS_HEADER *dnsHeader, const TCPIP_NET_IF* pNet)
{
    TCPIP_UINT16_VAL recordType;
    DNSS_DCPT*  pDnsSrvDcpt;
    UDP_SOCKET  s;
    OA_HASH_ENTRY*  hE = NULL;
    DNSS_HASH_ENTRY* dnsSHE = NULL;
    uint8_t*    pMemoryBlock = NULL;
    size_t      resAnswerRRs = 0U;
    TCPIP_UINT32_VAL ttlTime = {0U};
    uint8_t     *txbuf;
    size_t      count = 0U;
    uint16_t    offset=0U;
    uint32_t    servTxMsgSize=0U;
    uint16_t    txBufPos = 0U;
    uint8_t     hostNamePos=0U;
#if defined (TCPIP_STACK_USE_IPV6)
    uint8_t     i = 0U;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;
    IPV6_ADDR_STRUCT * addressPointer;
#endif

    pDnsSrvDcpt  = &gDnsSrvDcpt;
    if(pDnsSrvDcpt->dnssHashDcpt == NULL)
    {
        return false;
    }
    s = pDnsSrvDcpt->dnsSrvSocket;

     // collect hostname from Client Query Named server packet
    F_DNSCopyRXNameToTX(s);   // Copy hostname of first question over to TX packet
    if(strlen(hostNameWithDot) == 0U)
    {       
        return false;
    }
    // Get the Record type
    F_DNSSGetRecordType(s, &recordType);

    while (true)
    {
#if defined(TCPIP_STACK_USE_IPV6)
        if(recordType.Val == (uint16_t)TCPIP_DNSS_TYPE_A)
        {   // OK
            break;
        }
#endif  // defined(TCPIP_STACK_USE_IPV6)

#if defined(TCPIP_STACK_USE_IPV6)
        if(recordType.Val == (uint16_t)TCPIP_DNSS_TYPE_AAAA)
        {   // OK
            break;
        }
#endif  // defined(TCPIP_STACK_USE_IPV6)

        return false;
    }

    if(pDnsSrvDcpt->replyWithBoardInfo == 0U)
    {
        hE = TCPIP_OAHASH_EntryLookup(pDnsSrvDcpt->dnssHashDcpt, (uint8_t *)hostNameWithDot);
        if(hE != NULL)
        {
            dnsSHE = FC_OaHash2DnssHash(hE);
            pMemoryBlock = (uint8_t*)dnsSHE->memblk;
        }
        else
        {
            return false;
        }
    }

    uint32_t sysFreq = SYS_TMR_TickCounterFrequencyGet(); 
    // update Answer field
    // If the client Query answer is zero, then Response will have all the answers which is present in the cache
    // else if the client query answer count is more than the available answer counts  of the cache, then Answer RRs should
    // be the value of available entries in the cache , else if only the limited Answer RRs
    if(recordType.Val == (uint16_t)TCPIP_DNSS_TYPE_A)
    {
        if(pDnsSrvDcpt->replyWithBoardInfo != 0U)
        {
            resAnswerRRs = 1U;
            ttlTime.Val = (uint32_t)TCPIP_DNSS_TTL_TIME;
        }
        else if(hE != NULL)
        {
            if((dnsHeader->wAnswerRRs.Val == 0U) || (dnsHeader->wAnswerRRs.Val > dnsSHE->nIPv4Entries))
            {
            // all the available IPv4 address entries
                resAnswerRRs = dnsSHE->nIPv4Entries;
            }
            else  // only limited entries
            {
                resAnswerRRs = (uint32_t)dnsHeader->wAnswerRRs.Val;
            }
            // ttl time  w.r.t configured per entry
            // if the validityTime is not equal to 0
            if(dnsSHE->validityTime.Val != 0U)
            {
                ttlTime.Val = dnsSHE->validityTime.Val - ((SYS_TMR_TickCountGet() - dnsSHE->tInsert) / sysFreq);
            }
            // else TTL time will be default value of TCPIP_DNSS_PERMANENT_ENTRY_TTL_TIME
            else
            {
                ttlTime.Val = TCPIP_DNSS_PERMANENT_ENTRY_TTL_TIME;
            }
        }
        else
        {
            return false;
        }
    }
#if defined(TCPIP_STACK_USE_IPV6)
    else if(recordType.Val == (uint16_t)TCPIP_DNSS_TYPE_AAAA)
    {
        if(pDnsSrvDcpt->replyWithBoardInfo != 0U)
        {
            resAnswerRRs = 1U;
            ttlTime.Val = (uint32_t)TCPIP_DNSS_TTL_TIME;
        }
        else if(hE != NULL)
        {
            if((dnsHeader->wAnswerRRs.Val == 0U) || (dnsHeader->wAnswerRRs.Val > dnsSHE->nIPv6Entries))
            {
            // all the available IPv6 address entries
                resAnswerRRs = dnsSHE->nIPv6Entries;
            }
            else  // only limited entries
            {
                resAnswerRRs = (uint32_t)dnsHeader->wAnswerRRs.Val;
            }
             // ttl time  w.r.t configured per entry
            // if the validityTime is not equal to 0
            if(dnsSHE->validityTime.Val != 0U)
            {
                ttlTime.Val = dnsSHE->validityTime.Val - ((SYS_TMR_TickCountGet() - dnsSHE->tInsert) / sysFreq);
            }
            // else ttl time will be default value of TCPIP_DNSS_PERMANENT_ENTRY_TTL_TIME
            else
            {
                ttlTime.Val = (uint32_t)TCPIP_DNSS_PERMANENT_ENTRY_TTL_TIME;
            }
        }
        else
        {
            return false;
        }
    }
#endif
    else
    {
        return false;
    }
    offset = 0xC00C; // that is the location at 0x0c ( 12) DNS packet compression RFC 1035
    servTxMsgSize = sizeof(DNSS_HEADER)         // DNS header
                    + countWithLen + 2U + 2U;  // Query hostname + type + class
    if(recordType.Val == (uint16_t)TCPIP_DNSS_TYPE_A)
    {
        // offset + record type+class+ttl+ip type+size of IP address * number of answers present in the HASH table
        servTxMsgSize += resAnswerRRs * (2U + 2U + 2U + 4U + 2U + sizeof(IPV4_ADDR));
    }
#if defined(TCPIP_STACK_USE_IPV6)
    else if(recordType.Val == (uint16_t)TCPIP_DNSS_TYPE_AAAA)
    {
        // offset + record type+class+ttl+ip type+size of IP address * number of answers present ih HASH table
        servTxMsgSize += resAnswerRRs * (2U + 2U + 2U + 4U + 2U + sizeof(IPV6_ADDR));
    }
#endif
    else
    {
        TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
        return false;   // should not happen 
    }
    // check that we can transmit a DNS response packet
    TCPIPStack_Assert(servTxMsgSize < 0xffffU, __FILE__, __func__, __LINE__);
    if(TCPIP_UDP_TxPutIsReady(s, (uint16_t)servTxMsgSize) == 0U)
    {
        (void)TCPIP_UDP_OptionsSet(s, UDP_OPTION_TX_BUFF, FC_Uint2VPtr(servTxMsgSize));
        return false;
    }
     //this will put the start pointer at the beginning of the TX buffer
    (void)TCPIP_UDP_TxOffsetSet(s,0,false);

    //Get the write pointer:
    txbuf = TCPIP_UDP_TxPointerGet(s);
    if(txbuf == NULL)
    {
       return false;
    }
    txBufPos = 0;
    // Write DNS Server response packet
    // Transaction ID
    TCPIP_DNSS_DataPut(txbuf, txBufPos++, dnsHeader->wTransactionID.v[1]);
    TCPIP_DNSS_DataPut(txbuf, txBufPos++, dnsHeader->wTransactionID.v[0]);

    if((dnsHeader->wFlags.Val & 0x0100U) != 0U)
    {
        TCPIP_DNSS_DataPut(txbuf, txBufPos++, 0x81U); // Message is a response with recursion desired
    }
    else
    {
        TCPIP_DNSS_DataPut(txbuf, txBufPos++, 0x80U); // Message is a response without recursion desired flag set
    }
    TCPIP_DNSS_DataPut(txbuf, txBufPos++, 0x80U); // Recursion available

    // Question
    TCPIP_DNSS_DataPut(txbuf, txBufPos++, dnsHeader->wQuestions.v[1]);
    TCPIP_DNSS_DataPut(txbuf, txBufPos++, dnsHeader->wQuestions.v[0]);

    // Answer
    TCPIP_DNSS_DataPut(txbuf, txBufPos++, (uint8_t)((resAnswerRRs >> 8) & 0xFFU));
    TCPIP_DNSS_DataPut(txbuf, txBufPos++, (uint8_t)(resAnswerRRs & 0xFFU));

    // send Authority and Additional RRs as 0 , It will change latter 
    // when we support Authentication and Additional DNS info
    TCPIP_DNSS_DataPut(txbuf, txBufPos++, 0U);
    TCPIP_DNSS_DataPut(txbuf, txBufPos++, 0U);
    TCPIP_DNSS_DataPut(txbuf, txBufPos++, 0U);
    TCPIP_DNSS_DataPut(txbuf, txBufPos++, 0U);
    // Prepare all the queries
    for(hostNamePos=0;hostNamePos<countWithLen;hostNamePos++)
    {
        TCPIP_DNSS_DataPut(txbuf, txBufPos++, hostNameWithLen[hostNamePos]);
    }
    // Record Type
    TCPIP_DNSS_DataPut(txbuf, txBufPos++, recordType.v[1]);
    TCPIP_DNSS_DataPut(txbuf, txBufPos++, recordType.v[0]);
    // Class
    TCPIP_DNSS_DataPut(txbuf, txBufPos++, 0x00U);
    TCPIP_DNSS_DataPut(txbuf, txBufPos++, 0x01U);

    // Prepare Answer for all the answers
    for(count = 0U; count < resAnswerRRs; count++)
    {
        // Put Host name Pointer As per RFC1035 DNS compression
        TCPIP_DNSS_DataPut(txbuf, txBufPos++, (uint8_t)((offset >> 8) & 0xFFU));
        TCPIP_DNSS_DataPut(txbuf, txBufPos++, (uint8_t)(offset & 0xFFU));

        // Record Type
        TCPIP_DNSS_DataPut(txbuf, txBufPos++, recordType.v[1]);
        TCPIP_DNSS_DataPut(txbuf, txBufPos++, recordType.v[0]);
        // Class
        TCPIP_DNSS_DataPut(txbuf, txBufPos++, 0x00U);
        TCPIP_DNSS_DataPut(txbuf, txBufPos++, 0x01U);
        // TTL
        TCPIP_DNSS_DataPut(txbuf, txBufPos++, ttlTime.v[3]);
        TCPIP_DNSS_DataPut(txbuf, txBufPos++, ttlTime.v[2]);
        TCPIP_DNSS_DataPut(txbuf, txBufPos++, ttlTime.v[1]);
        TCPIP_DNSS_DataPut(txbuf, txBufPos++, ttlTime.v[0]);
        if(recordType.Val == (uint16_t)TCPIP_DNSS_TYPE_A)
        {
            // Length for TYPE_A
            TCPIP_DNSS_DataPut(txbuf, txBufPos++, 0x00U);
            TCPIP_DNSS_DataPut(txbuf, txBufPos++, 0x04U);
            if(hE != NULL)
            {
                dnsSHE->pip4Address = FC_U8Ptr2Ip4Add(pMemoryBlock);
                if(dnsSHE->pip4Address == NULL)
                {
                   return false;
                }
                TCPIP_DNSS_DataPut(txbuf, txBufPos++, dnsSHE->pip4Address[count].v[0]);
                TCPIP_DNSS_DataPut(txbuf, txBufPos++, dnsSHE->pip4Address[count].v[1]);
                TCPIP_DNSS_DataPut(txbuf, txBufPos++, dnsSHE->pip4Address[count].v[2]);
                TCPIP_DNSS_DataPut(txbuf, txBufPos++, dnsSHE->pip4Address[count].v[3]);
            }
            else
            {
                TCPIP_DNSS_DataPut(txbuf, txBufPos++, pNet->netIPAddr.v[0]);
                TCPIP_DNSS_DataPut(txbuf, txBufPos++, pNet->netIPAddr.v[1]);
                TCPIP_DNSS_DataPut(txbuf, txBufPos++, pNet->netIPAddr.v[2]);
                TCPIP_DNSS_DataPut(txbuf, txBufPos++, pNet->netIPAddr.v[3]);
            }
        }
#if defined(TCPIP_STACK_USE_IPV6)
        else if(recordType.Val == (uint16_t)TCPIP_DNSS_TYPE_AAAA)
        {
            // Length for TYPE_A
            TCPIP_DNSS_DataPut(txbuf, txBufPos++, 0x00U);     // Data Length 16 bytes
            TCPIP_DNSS_DataPut(txbuf, txBufPos++, 0x10U);  // sizeof (IPV6_ADDR)
            if(hE != NULL)
            {
                dnsSHE->pip6Address = FC_U8Ptr2Ip6Add(pMemoryBlock + pDnsSrvDcpt->IPv4EntriesPerDNSName * sizeof(IPV4_ADDR));
                if(dnsSHE->pip6Address == NULL)
                {
                   return false;
                }
                for(i=0;i<sizeof(IPV6_ADDR);i++)
                {
                    TCPIP_DNSS_DataPut(txbuf, txBufPos++, dnsSHE->pip6Address[count].v[i]);
                }
            }
            else
            {
                pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pNet);
                addressPointer = FC_DblNode2AddStruct(pIpv6Config->listIpv6UnicastAddresses.head);
                // only one IPv6 uni-cast address
                for(i=0;i<sizeof(IPV6_ADDR);i++)
                {
                    TCPIP_DNSS_DataPut(txbuf, txBufPos++, addressPointer->address.v[i]);
                }
            }
        }
#endif
        else
        {
            TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
            return false;   // should not happen 
        }
    }
    // Transmit all the server bytes
    // TCPIP_UDP_ArrayPut(s,txbuf,txBufPos);
    // Once it is completed writing into the buffer, you need to update the Tx offset again,
    // because the socket flush function calculates how many bytes are in the buffer using the current write pointer:
    (void)TCPIP_UDP_TxOffsetSet(s,txBufPos, false);
    (void)TCPIP_UDP_Flush(s);
    return true;
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_DNSS_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    DNSS_DCPT *pDnsSDcpt;
    pDnsSDcpt = &gDnsSrvDcpt;
    if(dnsSrvInitCount > 0U)
    {   // we're up and running        
        if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
        {
            if(--dnsSrvInitCount == 0U)
            {   // all closed
                // release resources
                if(pDnsSDcpt->dnsSSignalHandle != NULL)
                {
                    TCPIPStackSignalHandlerDeregister(pDnsSDcpt->dnsSSignalHandle);
                    pDnsSDcpt->dnsSSignalHandle = NULL;
                    pDnsSDcpt->dnsSTimeMseconds = 0U;
                }
                if(pDnsSDcpt->dnsSrvSocket != INVALID_UDP_SOCKET)
                {
                    (void)TCPIP_UDP_Close(pDnsSDcpt->dnsSrvSocket);
                }
            }
            // remove all the cache entries
            F_DNSSRemoveCacheEntries();
        }
    }
}

static  void F_DNSSRemoveCacheEntries(void)
{
    OA_HASH_ENTRY* pBkt=NULL;
    DNSS_HASH_ENTRY *dnsSHE=NULL;
    DNSS_DCPT       *pDnsSDcpt=NULL;
    uint8_t *pMemoryBlock=NULL;
    size_t      bktIx=0;
        
    pDnsSDcpt = &gDnsSrvDcpt;   

    if(pDnsSDcpt->dnssHashDcpt != NULL)
    {
        for(bktIx = 0; bktIx < pDnsSDcpt->dnssHashDcpt->hEntries; bktIx++)
        {
            pBkt = TCPIP_OAHASH_EntryGet(pDnsSDcpt->dnssHashDcpt, bktIx);
            dnsSHE = FC_OaHash2DnssHash(pBkt);
            pMemoryBlock = (uint8_t*)dnsSHE->memblk;

            (void)TCPIP_HEAP_Free(pDnsSDcpt->memH,pMemoryBlock);
            dnsSHE->nIPv4Entries = 0U;
#if defined(TCPIP_STACK_USE_IPV6)
            dnsSHE->nIPv6Entries = 0U;
#endif
            TCPIP_OAHASH_EntryRemove(pDnsSDcpt->dnssHashDcpt,pBkt);

        }
        (void)TCPIP_HEAP_Free(pDnsSDcpt->memH,pDnsSDcpt->dnssHashDcpt);
        pDnsSDcpt->dnssHashDcpt = NULL;
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

TCPIP_DNSS_RESULT TCPIP_DNSS_AddressCntGet(size_t index, char* hostName, size_t hostSize, size_t* ipCount)
{
    DNSS_HASH_ENTRY* pDnsSHE;
    OA_HASH_ENTRY   *hE;
    OA_HASH_DCPT    *pOH;
    DNSS_DCPT*  pDnsSDcpt;

    pDnsSDcpt = &gDnsSrvDcpt;
    pOH = pDnsSDcpt->dnssHashDcpt;
    if((hostName == NULL || hostSize == 0U) || (pDnsSDcpt->dnssHashDcpt==NULL))
    {
        return TCPIP_DNSS_RES_MEMORY_FAIL;
    }
    if(index >= pOH->hEntries)
    {
        return TCPIP_DNSS_RES_NO_SERVICE;
    }

    hE = TCPIP_OAHASH_EntryGet(pOH, index);
    if((hE->flags.busy != 0U) && (hE->flags.value & (uint16_t)DNSS_FLAG_ENTRY_COMPLETE) != 0U)
    {
       pDnsSHE = FC_OaHash2DnssHash(hE);
       (void)strncpy((char*)hostName, (char*)pDnsSHE->pHostName, hostSize - 1U);
       hostName[hostSize - 1U] = '\0';
       *ipCount = pDnsSHE->nIPv4Entries;
#if defined(TCPIP_STACK_USE_IPV6)
        *ipCount += pDnsSHE->nIPv6Entries;
#endif
        return TCPIP_DNSS_RES_OK;
    }
    return TCPIP_DNSS_RES_NO_ENTRY;

}

TCPIP_DNSS_RESULT TCPIP_DNSS_EntryGet(uint8_t* hostName, IP_ADDRESS_TYPE type, size_t index, IP_MULTI_ADDRESS* pGetAdd, uint32_t* ttlTime)
{
    OA_HASH_ENTRY   *hE;
    DNSS_HASH_ENTRY *dnsSHE;    
    DNSS_DCPT       *pDnsSDcpt; 
    uint8_t         *pMemoryBlock;
#if defined(TCPIP_STACK_USE_IPV6)
    uint8_t i=0;
    const IPV6_ADDR ipv6_addr_unspecified = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
#endif
    pDnsSDcpt = &gDnsSrvDcpt;
    if((hostName == NULL) || (pDnsSDcpt->dnssHashDcpt==NULL))
    {
        return TCPIP_DNSS_RES_NO_ENTRY;
    }
    hE = TCPIP_OAHASH_EntryLookup(pDnsSDcpt->dnssHashDcpt, (uint8_t *)hostName);
    if(hE == NULL)
    {  
        return TCPIP_DNSS_RES_NO_ENTRY;
    }
    dnsSHE = FC_OaHash2DnssHash(hE);
    
    pMemoryBlock = (uint8_t*)dnsSHE->memblk;

    uint32_t sysFreq = SYS_TMR_TickCounterFrequencyGet(); 

    if(type == IP_ADDRESS_TYPE_IPV4)
    {
        if(index >= dnsSHE->nIPv4Entries)
        {
            return TCPIP_DNSS_RES_NO_SERVICE;
        }
        dnsSHE->pip4Address = FC_U8Ptr2Ip4Add(pMemoryBlock);
        if(dnsSHE->pip4Address == NULL)
        {
            return TCPIP_DNSS_RES_NO_ENTRY;
        }

        if(dnsSHE->pip4Address[index].Val != 0U)
        {
            pGetAdd->v4Add.Val = dnsSHE->pip4Address[index].Val;
        }
        else
        {
            return TCPIP_DNSS_RES_NO_IPADDRESS;
        }
        
        if(dnsSHE->validityTime.Val != 0U)
        {
            *ttlTime = dnsSHE->validityTime.Val - ((SYS_TMR_TickCountGet() - dnsSHE->tInsert) / sysFreq);
        }
        else
        {            
            *ttlTime = TCPIP_DNSS_PERMANENT_ENTRY_TTL_TIME;
        }
    }
#if defined(TCPIP_STACK_USE_IPV6)
    if(type == IP_ADDRESS_TYPE_IPV6)
    {
        if(index >= dnsSHE->nIPv6Entries)
        {
            return TCPIP_DNSS_RES_NO_SERVICE;
        }
        dnsSHE->pip6Address = FC_U8Ptr2Ip6Add(pMemoryBlock + pDnsSDcpt->IPv4EntriesPerDNSName * sizeof(IPV4_ADDR));
        if(dnsSHE->pip6Address == NULL)
        {
            return TCPIP_DNSS_RES_NO_ENTRY;
        }

        if(memcmp(dnsSHE->pip6Address[i].v, ipv6_addr_unspecified.v, sizeof(IPV6_ADDR)) != 0)
        {
            (void)memcpy(pGetAdd->v6Add.v,dnsSHE->pip6Address[i].v,sizeof(IPV6_ADDR));
        }
        else
        {
            return TCPIP_DNSS_RES_NO_IPADDRESS;
        }
        
        if(dnsSHE->validityTime.Val != 0U)
        {
            *ttlTime = dnsSHE->validityTime.Val - ((SYS_TMR_TickCountGet() - dnsSHE->tInsert) / sysFreq);
        }
        else
        {            
            *ttlTime = TCPIP_DNSS_PERMANENT_ENTRY_TTL_TIME;
        }
    }
#endif
    return TCPIP_DNSS_RES_OK;
}


TCPIP_DNSS_RESULT TCPIP_DNSS_EntryAdd(const char* name, IP_ADDRESS_TYPE type, IP_MULTI_ADDRESS* pAdd,uint32_t entryTimeout)
{
    OA_HASH_ENTRY   *hE;
    DNSS_DCPT       *pDnsSDcpt; 
    DNSS_HASH_ENTRY* dnsSHE;
    TCPIP_DNSS_CACHE_ENTRY dnssCacheEntry;
    
    pDnsSDcpt = &gDnsSrvDcpt;
    
    if((name == NULL) || (pDnsSDcpt->dnssHashDcpt==NULL))
    {
        return TCPIP_DNSS_RES_MEMORY_FAIL;
    }

    (void)memset(&dnssCacheEntry, 0, sizeof(dnssCacheEntry));
    dnssCacheEntry.sHostNameData = (const uint8_t*)name;
    dnssCacheEntry.addressType = type;
    dnssCacheEntry.entryTimeout.Val = entryTimeout;
    if(dnssCacheEntry.addressType == IP_ADDRESS_TYPE_IPV4)
    {
        dnssCacheEntry.ip4Address.Val = pAdd->v4Add.Val;
    }
#if defined(TCPIP_STACK_USE_IPV6)     
    else if(dnssCacheEntry.addressType == IP_ADDRESS_TYPE_IPV6)
    {
        (void)memcpy(&dnssCacheEntry.ip6Address,&pAdd->v6Add,sizeof(IPV6_ADDR));
    }
#endif  
    else
    {
        return TCPIP_DNSS_RES_NO_ENTRY;
    }
    hE = TCPIP_OAHASH_EntryLookup(pDnsSDcpt->dnssHashDcpt, dnssCacheEntry.sHostNameData);
    if(hE != NULL)
    {
        dnsSHE = FC_OaHash2DnssHash(hE);
        return F_DNSSUpdateHashEntry(dnsSHE, dnssCacheEntry);
    }

    return F_DNSSSetHashEntry(DNSS_FLAG_ENTRY_COMPLETE, dnssCacheEntry);

}

static  TCPIP_DNSS_RESULT  F_DNSSUpdateHashEntry( DNSS_HASH_ENTRY *dnsSHE,TCPIP_DNSS_CACHE_ENTRY dnssCacheEntry)
{
    DNSS_DCPT       *pDnsSDcpt; 
    uint8_t *pMemoryBlock;    
    pDnsSDcpt = &gDnsSrvDcpt;
    uint8_t     i=0;

    pMemoryBlock = (uint8_t*)dnsSHE->memblk;

    if(dnssCacheEntry.addressType == IP_ADDRESS_TYPE_IPV4)
    {
        if(dnsSHE->nIPv4Entries >= pDnsSDcpt->IPv4EntriesPerDNSName)
        {
            return TCPIP_DNSS_RES_CACHE_FULL;
        }
        dnsSHE->pip4Address = FC_U8Ptr2Ip4Add(pMemoryBlock);
        if(dnsSHE->pip4Address == NULL)
        {
            return TCPIP_DNSS_RES_MEMORY_FAIL;
        }
        for(i=0; i<pDnsSDcpt->IPv4EntriesPerDNSName; i++)
        {
            if(dnsSHE->pip4Address[i].Val == dnssCacheEntry.ip4Address.Val )
            {
                return TCPIP_DNSS_RES_DUPLICATE_ENTRY;
            }
        }
        dnsSHE->pip4Address[dnsSHE->nIPv4Entries].Val = 
                            dnssCacheEntry.ip4Address.Val;
        dnsSHE->nIPv4Entries ++ ;
    }
#if defined(TCPIP_STACK_USE_IPV6)
    if(dnssCacheEntry.addressType == IP_ADDRESS_TYPE_IPV6)
    {
        if(dnsSHE->nIPv6Entries >= pDnsSDcpt->IPv6EntriesPerDNSName)
        {
            return TCPIP_DNSS_RES_CACHE_FULL;
        }
        dnsSHE->pip6Address = FC_U8Ptr2Ip6Add(pMemoryBlock + pDnsSDcpt->IPv4EntriesPerDNSName * sizeof(IPV4_ADDR));
        if(dnsSHE->pip6Address == NULL)
        {
            return TCPIP_DNSS_RES_MEMORY_FAIL;
        }
        for(i=0; i < pDnsSDcpt->IPv6EntriesPerDNSName; i++)
        {
            if(memcmp(dnsSHE->pip6Address[i].v, dnssCacheEntry.ip6Address.v, sizeof(IPV6_ADDR)) == 0)
            {
                return TCPIP_DNSS_RES_DUPLICATE_ENTRY;
            }
        }
        (void)memcpy( &dnsSHE->pip6Address[dnsSHE->nIPv6Entries],&dnssCacheEntry.ip6Address,sizeof(IPV6_ADDR));
        dnsSHE->nIPv6Entries ++ ;
    }
#endif
    dnsSHE->validityTime = dnssCacheEntry.entryTimeout;
    
    return TCPIP_DNSS_RES_OK;
}

static  TCPIP_DNSS_RESULT  F_DNSSSetHashEntry( DNSS_HASH_ENTRY_FLAGS newFlags,TCPIP_DNSS_CACHE_ENTRY dnssCacheEntry)
{
    uint8_t *pMemoryBlock;    
    DNSS_DCPT       *pDnsSDcpt; 
    OA_HASH_ENTRY* hE;
    DNSS_HASH_ENTRY *dnsSHE;    

    pDnsSDcpt = &gDnsSrvDcpt;
    if(pDnsSDcpt->dnssHashDcpt==NULL)
    {
        return TCPIP_DNSS_RES_MEMORY_FAIL;
    }
    hE = TCPIP_OAHASH_EntryLookupOrInsert(pDnsSDcpt->dnssHashDcpt, dnssCacheEntry.sHostNameData);
    if(hE == NULL)
    {
        return TCPIP_DNSS_RES_CACHE_FULL;
    }
    dnsSHE = FC_OaHash2DnssHash(hE);
    pMemoryBlock = dnsSHE->memblk;
    dnsSHE->hEntry.flags.value &= ~(uint16_t)DNSS_FLAG_ENTRY_VALID_MASK;
    dnsSHE->hEntry.flags.value |= (uint16_t)newFlags;
    dnsSHE->memblk = pMemoryBlock;
    dnsSHE->addType = (uint8_t)dnssCacheEntry.addressType;
    
    if(dnssCacheEntry.addressType == IP_ADDRESS_TYPE_IPV4)
    {
        if(dnsSHE->nIPv4Entries >= pDnsSDcpt->IPv4EntriesPerDNSName)
        {
            return TCPIP_DNSS_RES_CACHE_FULL;
        }
        dnsSHE->pip4Address = FC_U8Ptr2Ip4Add(pMemoryBlock);
        if(dnsSHE->pip4Address == NULL)
        {
            return TCPIP_DNSS_RES_MEMORY_FAIL;
        }
        dnsSHE->pip4Address[dnsSHE->nIPv4Entries].Val = 
                            dnssCacheEntry.ip4Address.Val;
        dnsSHE->nIPv4Entries ++ ;
    }
#if defined(TCPIP_STACK_USE_IPV6)
    if(dnssCacheEntry.addressType == IP_ADDRESS_TYPE_IPV6)
    {
        if(dnsSHE->nIPv6Entries >= pDnsSDcpt->IPv6EntriesPerDNSName)
        {
            return TCPIP_DNSS_RES_CACHE_FULL;
        }
        dnsSHE->pip6Address = FC_U8Ptr2Ip6Add(pMemoryBlock + pDnsSDcpt->IPv4EntriesPerDNSName * sizeof(IPV4_ADDR));
        if(dnsSHE->pip6Address == NULL)
        {
            return TCPIP_DNSS_RES_MEMORY_FAIL;
        }
        (void)memcpy( &dnsSHE->pip6Address[dnsSHE->nIPv6Entries],&dnssCacheEntry.ip6Address,sizeof(IPV6_ADDR));
        dnsSHE->nIPv6Entries ++ ;
    }
#endif

    dnsSHE->tInsert = SYS_TMR_TickCountGet();
    dnsSHE->validityTime = dnssCacheEntry.entryTimeout;
    return TCPIP_DNSS_RES_OK;
}

TCPIP_DNSS_RESULT TCPIP_DNSS_CacheEntryRemove(const char* name, IP_ADDRESS_TYPE type, IP_MULTI_ADDRESS* pAdd)
{
    OA_HASH_ENTRY* hE;
    DNSS_HASH_ENTRY *dnsSHE;    
    DNSS_DCPT       *pDnsSDcpt; 
    uint8_t *pMemoryBlock;  
    size_t      ix=0;
    bool        addrISPresent=false;
    
    pDnsSDcpt = &gDnsSrvDcpt;
    if((name == NULL) || (pDnsSDcpt->dnssHashDcpt==NULL))
    {
        return TCPIP_DNSS_RES_MEMORY_FAIL;
    }
    hE = TCPIP_OAHASH_EntryLookup(pDnsSDcpt->dnssHashDcpt, (const uint8_t*)name);
    if(hE == NULL)
    {  
        return TCPIP_DNSS_RES_NO_ENTRY;
    }
    dnsSHE = FC_OaHash2DnssHash(hE);
    if((uint8_t)type != dnsSHE->addType)
    {  
        return TCPIP_DNSS_RES_NO_ENTRY;
    }
    pMemoryBlock = (uint8_t*)dnsSHE->memblk;

    if(dnsSHE->addType == (uint8_t)IP_ADDRESS_TYPE_IPV4)
    {
        if(dnsSHE->nIPv4Entries > pDnsSDcpt->IPv4EntriesPerDNSName)
        {
            return TCPIP_DNSS_RES_MEMORY_FAIL;
        }
        dnsSHE->pip4Address = FC_U8Ptr2Ip4Add(pMemoryBlock);
        if(dnsSHE->pip4Address == NULL)
        {
            return TCPIP_DNSS_RES_MEMORY_FAIL;
        }
        for(ix=0; ix<pDnsSDcpt->IPv4EntriesPerDNSName; ix++)
        {
            if(dnsSHE->pip4Address[ix].Val == pAdd->v4Add.Val)
            {
                dnsSHE->nIPv4Entries--;
                dnsSHE->pip4Address[ix].Val = 0;
                addrISPresent = true;
                break;
            }
        }
    }
#if defined(TCPIP_STACK_USE_IPV6)
    if(dnsSHE->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
    {
        if(dnsSHE->nIPv6Entries > pDnsSDcpt->IPv6EntriesPerDNSName)
        {
            return TCPIP_DNSS_RES_MEMORY_FAIL;
        }
        dnsSHE->pip6Address = FC_U8Ptr2Ip6Add(pMemoryBlock + pDnsSDcpt->IPv4EntriesPerDNSName * sizeof(IPV4_ADDR));
        if(dnsSHE->pip6Address == NULL)
        {
            return TCPIP_DNSS_RES_MEMORY_FAIL;
        }
        for(ix=0; ix < pDnsSDcpt->IPv6EntriesPerDNSName; ix++)
        {
            if(memcmp(dnsSHE->pip6Address[ix].v, pAdd->v6Add.v, sizeof(IPV6_ADDR)) == 0)
            {
                dnsSHE->nIPv6Entries--;
                (void)memset(&dnsSHE->pip6Address[ix], 0,sizeof(IPV6_ADDR));
                addrISPresent = true;
                break;
            }
        }
    }
#endif

    if(addrISPresent == false)
    {
        return TCPIP_DNSS_RES_NO_ENTRY;
    }

   // Free Hash entry and free the allocated memory for this HostName if there
   // is no IPv4 and IPv6 entry
   if(dnsSHE->nIPv4Entries == 0U
#if defined(TCPIP_STACK_USE_IPV6)
     && dnsSHE->nIPv6Entries == 0U
#endif
    )
    {       
        TCPIP_OAHASH_EntryRemove(pDnsSDcpt->dnssHashDcpt,hE);
    }
   return TCPIP_DNSS_RES_OK;
    
}

static uint8_t TCPIP_DNSS_DataGet(uint16_t pos)
{
    return (uint8_t)(dnsSrvRecvByte[pos]);
}

void TCPIP_DNSS_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);
    
    if(((uint16_t)sigPend & (uint16_t)TCPIP_MODULE_SIGNAL_TMO) != 0U)
    { // regular TMO occurred
        TCPIP_DNSS_CacheTimeTask();
    }

    if(gDnsSrvDcpt.dnsServInUse == 0U || gDnsSrvDcpt.dnssHashDcpt == NULL)
    {
        return;
    }


    if(((uint16_t)sigPend & (uint16_t)TCPIP_MODULE_SIGNAL_RX_PENDING) != 0U)
    { //  RX signal occurred
        TCPIP_DNSS_Process();
    }

}

// send a signal to the DNSS module that data is available
// no manager alert needed since this normally results as a higher layer (UDP) signal
static void F_DNSSSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_UDP_SIGNAL_RX_DATA)
    {
        (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}



static void TCPIP_DNSS_Process(void)
{
    UDP_SOCKET  s;    
    UDP_SOCKET_INFO     udpSockInfo;
    const TCPIP_NET_IF* pNet = NULL;
    DNSS_HEADER DNSServHeader;
    uint16_t recvLen = 0U;
    static TCPIP_UINT16_VAL transactionId;
  
    s = gDnsSrvDcpt.dnsSrvSocket;

    while(true)
    {
     // See if a DNS query packet has arrived
        recvLen = TCPIP_UDP_GetIsReady(s);
        if(recvLen == 0U)
        {
           break;
        }
        if(recvLen > (uint16_t)sizeof(dnsSrvRecvByte) - 1U)
        {
            (void)TCPIP_UDP_Discard(s);
            continue;
        }
        gDnsSrvBytePos = 0U;
        (void)memset(&udpSockInfo, 0, sizeof(udpSockInfo));
        (void)TCPIP_UDP_SocketInfoGet(s, &udpSockInfo);
        pNet = (const TCPIP_NET_IF*)udpSockInfo.hNet;
        // check if DHCP server is enabled or Not for this incoming packet interface
        if(!TCPIP_DNSS_ValidateIf(pNet))
        {
            (void)TCPIP_UDP_Discard(s);
            continue;
        }
        // Read DNS header
        (void)TCPIP_UDP_ArrayGet(s, (uint8_t*)dnsSrvRecvByte, recvLen);
        // Assign DNS transaction ID
        DNSServHeader.wTransactionID.v[1] = dnsSrvRecvByte[gDnsSrvBytePos++];
        DNSServHeader.wTransactionID.v[0] = dnsSrvRecvByte[gDnsSrvBytePos++];
        // To make a response for a valid address and quicker one.
        // this is used to make sure that we should not transmit same address response again and again for sometime.
        // This will help to improve the throughput
        //We are checking the previous transaction id and process the rx packet only when it is a new transaction id
        // comparing to the previous one.
        if(transactionId.Val != DNSServHeader.wTransactionID.Val)
        {
            transactionId.Val = DNSServHeader.wTransactionID.Val;
        }
        else
        {
            (void)TCPIP_UDP_Discard(s);
            continue;
        }
        // Assign DNS wflags
        DNSServHeader.wFlags.v[1] = dnsSrvRecvByte[gDnsSrvBytePos++];
        DNSServHeader.wFlags.v[0] = dnsSrvRecvByte[gDnsSrvBytePos++];

        DNSServHeader.wQuestions.v[1] = dnsSrvRecvByte[gDnsSrvBytePos++];
        DNSServHeader.wQuestions.v[0] = dnsSrvRecvByte[gDnsSrvBytePos++];

        DNSServHeader.wAnswerRRs.v[1] = dnsSrvRecvByte[gDnsSrvBytePos++];
        DNSServHeader.wAnswerRRs.v[0] = dnsSrvRecvByte[gDnsSrvBytePos++];

        DNSServHeader.wAuthorityRRs.v[1] = dnsSrvRecvByte[gDnsSrvBytePos++];
        DNSServHeader.wAuthorityRRs.v[0] = dnsSrvRecvByte[gDnsSrvBytePos++];

        DNSServHeader.wAdditionalRRs.v[1] = dnsSrvRecvByte[gDnsSrvBytePos++];
        DNSServHeader.wAdditionalRRs.v[0] = dnsSrvRecvByte[gDnsSrvBytePos++];

        // Ignore this packet if it isn't a query
        if((DNSServHeader.wFlags.Val & 0x8000U) == 0x8000U)
        {
            (void)TCPIP_UDP_Discard(s);
            break;
        }
        // Ignore this packet if there are no questions in it
        if(DNSServHeader.wQuestions.Val == 0u)
        {
            (void)TCPIP_UDP_Discard(s);
            break;
        }
        // send the DNS client query response
        if(!F_DNSS_SendResponse(&DNSServHeader, pNet))
        {
            (void)TCPIP_UDP_Discard(s);
            continue;
        }
    }
}
// returns true if the pIf can be selected for DNS traffic
// false otherwise
static bool TCPIP_DNSS_ValidateIf(const TCPIP_NET_IF* pIf)
{
    // check that DNS is enabled
    if(TCPIP_DNSS_IsEnabled(pIf))
    {
        // check that interface is up and linked
        if(TCPIPStackHandleToNetLinked(pIf) != NULL)
        {
            // check for a valid address
            if(!TCPIPStackIsConfig(pIf) && TCPIPStackNetAddress(pIf) != 0U)
            {
                return true;
            }
        }
    }
    return false;
}

/*****************************************************************************
  Function:
    static void F_DNSCopyRXNameToTX(UDP_SOCKET s)

  Summary:
    Copies a DNS hostname, possibly including name compression, from the RX 
    packet to the TX packet (without name compression in TX case).
    
  Description:
    None

  Precondition:
    RX pointer is set to currently point to the DNS name to copy

  Parameters:
    None

  Returns:
    None
  ***************************************************************************/
static void F_DNSCopyRXNameToTX(UDP_SOCKET s)
{
    uint16_t w;
    uint8_t i=0,j=0;
    uint8_t len;
    //uint8_t data[64]={0};
    
    countWithDot=0U;
    countWithLen=0U;
    while(true)
    {
        // Get first byte which will tell us if this is a 16-bit pointer or the
        // length of the first of a series of labels
        //  return;
        i = TCPIP_DNSS_DataGet(gDnsSrvBytePos++);
        
        // Check if this is a pointer, if so, get the remaining 8 bits and seek to the pointer value
        if((i & 0xC0u) == 0xC0u)
        {
            w = TCPIP_DNSS_DataGet(gDnsSrvBytePos++);
            gDnsSrvBytePos =  w;
            continue;
        }

        // Write the length byte
        len = i;
        if(countWithLen==0U && countWithDot==0U)
        {
            hostNameWithLen[countWithLen++]=len;
        }
        else
        {
            hostNameWithLen[countWithLen++]=len;
            // when it reached the end of hostname , then '.' is not required
            if(len != 0U)
            {
                hostNameWithDot[countWithDot++]='.';
            }
        }   
        
        // Exit if we've reached a zero length label
        if(len == 0u)
        {
            hostNameWithDot[countWithDot] = '\0';
            return;
        }

        //UDPGetArray(s,data,len);
        for(j=0;j<len;j++)
        {
            i = TCPIP_DNSS_DataGet(gDnsSrvBytePos++);
        // update the hostNameWithDot with data 
            hostNameWithLen[countWithLen++] = i;
        
        // update the hostNameWithLen with data 
            hostNameWithDot[countWithDot++] = (char)i;
        }

        if((countWithLen > (uint16_t)TCPIP_DNSS_HOST_NAME_LEN) || (countWithDot > (uint16_t)TCPIP_DNSS_HOST_NAME_LEN))
        {
            return;
        }        
    }
}

static void TCPIP_DNSS_CacheTimeTask(void)
{
    DNSS_HASH_ENTRY* pDnsSHE;
    OA_HASH_ENTRY   *hE;
    size_t          bktIx;
    OA_HASH_DCPT    *pOH;
    DNSS_DCPT*  pDnsSDcpt;


    
    pDnsSDcpt = &gDnsSrvDcpt;
    pOH = pDnsSDcpt->dnssHashDcpt;
    if(pDnsSDcpt->dnssHashDcpt==NULL)
    {
        return;
    }
    gDnsSrvDcpt.dnsSTimeMseconds += (uint32_t)TCPIP_DNSS_TASK_PROCESS_RATE;
    uint32_t sysFreq = SYS_TMR_TickCounterFrequencyGet(); 

// check the lease values and if there is any entry whose lease value exceeds the lease duration remove the lease entries from the HASH.

    for(bktIx = 0; bktIx < pOH->hEntries; bktIx++)
    {
        hE = TCPIP_OAHASH_EntryGet(pOH, bktIx);
        if((hE->flags.busy != 0U) && (hE->flags.value & (uint16_t)DNSS_FLAG_ENTRY_COMPLETE) != 0U)
        {
            pDnsSHE = FC_OaHash2DnssHash(hE);
            // only check the entries which has a valid validity time
            // if the entry has 0 validity time, then that entry is a permanent entry
            // no need to be removed from here
            if(pDnsSHE->validityTime.Val != 0U)
            {
                if(((SYS_TMR_TickCountGet() - pDnsSHE->tInsert) / sysFreq) > pDnsSHE->validityTime.Val )
                {
                    pDnsSHE->tInsert = 0;
                    TCPIP_OAHASH_EntryRemove(pOH,hE);

                    pDnsSHE->nIPv4Entries = 0U;
    #ifdef TCPIP_STACK_USE_IPV6
                    pDnsSHE->nIPv6Entries = 0U;
    #endif    
                }
            }
        }       
    }   
}

bool TCPIP_DNSS_IsEnabled(TCPIP_NET_HANDLE hNet)
{
    const TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);
    DNSS_DCPT        *pDnsSDcpt;

    pDnsSDcpt = &gDnsSrvDcpt;
    if(pDnsSDcpt->dnssHashDcpt==NULL)
    {
        return false;
    }

    if(pNetIf != NULL)
    {
        if((pNetIf->Flags.bIsDnsServerEnabled == 1U) && (pDnsSDcpt->dnsServInUse != 0U))
        {
            return true;
        }
    }
    return false;
}

bool TCPIP_DNSS_Enable(TCPIP_NET_HANDLE hNet)
{
    return F_DNSS_Enable(hNet, true);
}

static bool F_DNSS_Enable(TCPIP_NET_HANDLE hNet, bool checkIfUp)
{
    DNSS_DCPT        *pDnsSDcpt;
    TCPIP_NET_IF    *pNetIf;
    
    pDnsSDcpt = &gDnsSrvDcpt;
    if(pDnsSDcpt->dnssHashDcpt == NULL)
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
    
    if(pNetIf == NULL || TCPIP_STACK_DNSServiceCanStart(pNetIf, TCPIP_STACK_DNS_SERVICE_SERVER) == false)
    {
        return false;
    }
    pNetIf->Flags.bIsDnsServerEnabled = 1U;
    
    if(pDnsSDcpt->dnsSrvSocket == INVALID_UDP_SOCKET)
    {
        pDnsSDcpt->dnsSrvSocket = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_ANY, TCPIP_DNS_SERVER_PORT, NULL);
        if( pDnsSDcpt->dnsSrvSocket == INVALID_UDP_SOCKET)
        {
            return false;
        }
        pDnsSDcpt->intfIdx = pNetIf->netIfIx;
        pDnsSDcpt->dnsServInUse = 1U;
        (void)TCPIP_UDP_SignalHandlerRegister(pDnsSDcpt->dnsSrvSocket, TCPIP_UDP_SIGNAL_RX_DATA, &F_DNSSSocketRxSignalHandler, NULL);
    }
    return true;
}

bool TCPIP_DNSS_Disable(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);
    DNSS_DCPT* pServer;

    pServer  = &gDnsSrvDcpt;
    if((pNetIf == NULL)||(pServer->dnssHashDcpt==NULL))
    {
        return false;
    }
    
    if(pNetIf->netIfIx != pServer->intfIdx)
    {
        return false;
    }
    
    if(pServer->dnsServInUse != 0U)
    {
        pServer->dnsServInUse = 0U;
        pNetIf->Flags.bIsDnsServerEnabled = 0U;
 
        if(pServer->dnsSrvSocket != INVALID_UDP_SOCKET)
        {
            (void)TCPIP_UDP_Close(pServer->dnsSrvSocket);
        }
    }    

    return true;    
}

static void F_DNSSGetRecordType(UDP_SOCKET s, TCPIP_UINT16_VAL* recordType)
{
    recordType->v[1] = TCPIP_DNSS_DataGet(gDnsSrvBytePos++);
    recordType->v[0] = TCPIP_DNSS_DataGet(gDnsSrvBytePos++);
}

size_t TCPIP_OAHASH_DNSS_KeyHash(OA_HASH_DCPT* pOH, const void* key)
{
    const uint8_t* dnsHostNameKey;
    size_t      hostnameLen=0;

    dnsHostNameKey = (const uint8_t*)key;
    hostnameLen = strlen((const char*)dnsHostNameKey);
    return fnv_32_hash(dnsHostNameKey, hostnameLen) % (pOH->hEntries);
}

OA_HASH_ENTRY* TCPIP_OAHASH_DNSS_EntryDelete(OA_HASH_DCPT* pOH)
{
    OA_HASH_ENTRY*  pBkt;
    size_t      bktIx;
    DNSS_HASH_ENTRY  *pE;
    DNSS_DCPT        *pDnssDcpt;

    pDnssDcpt = &gDnsSrvDcpt;
    if(pDnssDcpt->dnssHashDcpt == NULL)
    {
        return NULL;
    }
    uint32_t sysFreq = SYS_TMR_TickCounterFrequencyGet(); 

    for(bktIx = 0; bktIx < pOH->hEntries; bktIx++)
    {
        pBkt = TCPIP_OAHASH_EntryGet(pOH, bktIx);       
        if(pBkt->flags.busy != 0U)
        {
            pE = FC_OaHash2DnssHash(pBkt);
            if(pE->validityTime.Val != 0U)
            {
                // return the hash bucket entry if the current tick count is more than configured Validity entry time out value.
                if(SYS_TMR_TickCountGet() - pE->tInsert > (pE->validityTime.Val * sysFreq))
                {
                    return pBkt;
                }
            }
            else
            {
                // return the hash bucket entry if the validity time is 0.
                    return pBkt;
            }
        }
    }
    return NULL;
}

int TCPIP_OAHASH_DNSS_KeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key)
{
    DNSS_HASH_ENTRY* pDnssHE;
    const uint8_t*   dnsHostNameKey;
    
    pDnssHE = FC_OaHash2DnssHash(hEntry);
    dnsHostNameKey = (const uint8_t*)key;    
    
    return strcmp((const char*)pDnssHE->pHostName,(const char*)dnsHostNameKey);
}

void TCPIP_OAHASH_DNSS_KeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key)
{
    const uint8_t*  dnsHostNameKey;
    DNSS_HASH_ENTRY* pDnssHE;
    size_t hostnameLen;

    if(key == NULL)
    {
        return;
    }
    
    pDnssHE = FC_OaHash2DnssHash(dstEntry);
    dnsHostNameKey = (const uint8_t*)key;
    hostnameLen = strlen((const char*)dnsHostNameKey);
    if(hostnameLen > (size_t)TCPIP_DNSS_HOST_NAME_LEN)
    {
        return;
    }

    if(pDnssHE->pHostName == NULL)
    {
        return;
    }
    (void)memset(pDnssHE->pHostName, 0, TCPIP_DNSS_HOST_NAME_LEN);
    (void)memcpy(pDnssHE->pHostName, dnsHostNameKey, hostnameLen);
    pDnssHE->pHostName[hostnameLen] = 0;
}

#if defined(OA_DOUBLE_HASH_PROBING)
size_t TCPIP_OAHASH_DNSS_ProbeHash(OA_HASH_DCPT* pOH, const void* key)
{
    uint8_t    *dnsHostNameKey;
    size_t      hostnameLen=0;
    
    dnsHostNameKey = (uint8_t  *)key;
    hostnameLen = strlen(dnsHostNameKey);
    return fnv_32a_hash(dnsHostNameKey, hostnameLen) % (pOH->hEntries);
}
#endif  // defined(OA_DOUBLE_HASH_PROBING)

#else
bool TCPIP_DNSS_IsEnabled(TCPIP_NET_HANDLE hNet){return false;}
bool TCPIP_DNSS_Enable(TCPIP_NET_HANDLE hNet){return false;}
bool TCPIP_DNSS_Disable(TCPIP_NET_HANDLE hNet){return false;}


#endif //#if defined(TCPIP_STACK_USE_DNS_SERVER)
