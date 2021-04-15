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

static DNSS_DCPT gDnsSrvDcpt={0,INVALID_UDP_SOCKET ,DNSS_STATE_START,0};

static void _DNSCopyRXNameToTX(UDP_SOCKET s);
static  TCPIP_DNSS_RESULT  _DNSSUpdateHashEntry( DNSS_HASH_ENTRY *dnsSHE,TCPIP_DNSS_CACHE_ENTRY dnssCacheEntry);
static  TCPIP_DNSS_RESULT  _DNSSSetHashEntry( DNSS_HASH_ENTRY_FLAGS newFlags,TCPIP_DNSS_CACHE_ENTRY dnssCacheEntry);
static void _DNSSGetRecordType(UDP_SOCKET s,TCPIP_UINT16_VAL *recordType);
static bool TCPIP_DNSS_ValidateIf(TCPIP_NET_IF* pIf);
static bool _DNSS_Enable(TCPIP_NET_HANDLE hNet, bool checkIfUp);
static bool TCPIP_DNSS_DataPut(uint8_t * buf,uint32_t pos,uint8_t val);
static uint8_t TCPIP_DNSS_DataGet(uint16_t pos);
static void TCPIP_DNSS_CacheTimeTask(void);
static void TCPIP_DNSS_Process(void);
static void _DNSSSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param);



// Server Need to parse the incoming hostname from client . replace Len with dot
static uint8_t hostNameWithDot[TCPIP_DNSS_HOST_NAME_LEN+1]={0};
static uint16_t countWithDot=0;

// Server Need to parse the incoming hostname from client . keep Len and this array will be 
//used while transmitting Name Server response packet
static uint8_t hostNameWithLen[TCPIP_DNSS_HOST_NAME_LEN+1]={0}; 
static uint16_t countWithLen=0;

static uint8_t  dnsSrvRecvByte[64+1]={0};
// DNS server received buffer position
static uint32_t gDnsSrvBytePos=0;

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static  void _DNSSRemoveCacheEntries(void);
static void _DNSS_RemoveHashAll(void)
{
    DNSS_DCPT* pDnsSDcpt=NULL;
    OA_HASH_DCPT *pOhDcpt=NULL;

    pDnsSDcpt = &gDnsSrvDcpt;
    if(pDnsSDcpt == NULL) return;

    pOhDcpt = pDnsSDcpt->dnssHashDcpt;
    if(pOhDcpt == NULL) return;

    _DNSSRemoveCacheEntries();

    pOhDcpt = 0;
}
#else
#define _DNSS_RemoveHashAll()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

bool TCPIP_DNSS_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_DNSS_MODULE_CONFIG* pDnsSConfig)
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

    if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {	// interface restart      
        return true;
    }

    pDnsSDcpt = &gDnsSrvDcpt;

    if(pDnsSDcpt->dnsSrvInitCount==0)
    {
        if(pDnsSConfig == 0)
        {
            return false;
        }

        pDnsSDcpt->memH  = stackCtrl->memH;

        if(pDnsSDcpt->dnssHashDcpt == 0)
        {
            cacheEntries = pDnsSConfig->IPv4EntriesPerDNSName;
#ifdef TCPIP_STACK_USE_IPV6
            cacheEntries += pDnsSConfig->IPv6EntriesPerDNSName;
#endif
            hashMemSize = sizeof(OA_HASH_DCPT) + cacheEntries * sizeof(DNSS_HASH_ENTRY);
            hashDcpt = (OA_HASH_DCPT*)TCPIP_HEAP_Calloc(pDnsSDcpt->memH,1,hashMemSize);
            if(hashDcpt == 0)
            {	// failed
                return false;
            }

            // populate the entries
            hashDcpt->memBlk = hashDcpt + 1;
            hashDcpt->hParam = hashDcpt;	// store the descriptor it belongs to
            hashDcpt->hEntrySize = sizeof(DNSS_HASH_ENTRY);
            hashDcpt->hEntries = cacheEntries;
            hashDcpt->probeStep = TCPIP_DNSS_HASH_PROBE_STEP;

            hashDcpt->hashF = TCPIP_OAHASH_DNSS_KeyHash;
            hashDcpt->delF = TCPIP_OAHASH_DNSS_EntryDelete;
            hashDcpt->cmpF = TCPIP_OAHASH_DNSS_KeyCompare;
            hashDcpt->cpyF = TCPIP_OAHASH_DNSS_KeyCopy;
#if defined(OA_DOUBLE_HASH_PROBING)
            hashDcpt->probeHash = TCPIP_OAHASH_DNSS_ProbeHash;
#endif  // defined(OA_DOUBLE_HASH_PROBING)

            TCPIP_OAHASH_Initialize(hashDcpt);
            pDnsSDcpt->dnssHashDcpt = hashDcpt;
            pDnsSDcpt->flags.Val = 0;

            pDnsSDcpt->IPv4EntriesPerDNSName= pDnsSConfig->IPv4EntriesPerDNSName;
#ifdef TCPIP_STACK_USE_IPV6
            pDnsSDcpt->IPv6EntriesPerDNSName = pDnsSConfig->IPv6EntriesPerDNSName;
#endif
        }
        pDnsSDcpt->dnsSrvSocket = INVALID_UDP_SOCKET;
        pDnsSDcpt->smState = DNSS_STATE_START;
        pDnsSDcpt->replyWithBoardInfo = pDnsSConfig->replyBoardAddr;
        pDnsSDcpt->dnsSrvInitCount++;


        if(pDnsSDcpt->dnsSSignalHandle == 0)
        {	// once per service
            pDnsSDcpt->dnsSSignalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_DNSS_Task, TCPIP_DNSS_TASK_PROCESS_RATE);
            if(pDnsSDcpt->dnsSSignalHandle)
            {
                pDnsSDcpt->dnsSTimeMseconds = 0;
            }
            else
            {
                return false;
            }
        }
        // allocate memory for each DNS hostname , IPv4 address and IPv6 address
            // and the allocation will be done per Hash descriptor
         memoryBlockSize = pDnsSDcpt->IPv4EntriesPerDNSName*sizeof(IPV4_ADDR)
#if defined(TCPIP_STACK_USE_IPV6)
        + pDnsSDcpt->IPv6EntriesPerDNSName*sizeof(IPV6_ADDR)
#endif
        +TCPIP_DNSS_HOST_NAME_LEN+1;

        for(hashCnt=0;hashCnt < cacheEntries;hashCnt++)
        {
            pBkt = TCPIP_OAHASH_EntryGet(hashDcpt, hashCnt);

            pE = (DNSS_HASH_ENTRY*)pBkt;
            pE->pHostName = NULL;
            pE->pip4Address = NULL;
#if defined(TCPIP_STACK_USE_IPV6)
            pE->pip6Address = NULL;
#endif
            pMemoryBlock = (uint8_t *)TCPIP_HEAP_Calloc(pDnsSDcpt->memH,1,memoryBlockSize);
            if(pMemoryBlock == 0)
            {
                if(hashDcpt != 0)
                {
                    // if there is any pMemoryBlock already made for other
                    // hash entries , we need to remove those also
                   _DNSS_RemoveHashAll();
                }
                return false;
            }

            pE = (DNSS_HASH_ENTRY *)pBkt;
            pE->memblk = pMemoryBlock;

            // if IPv4EntriesPerDNSName != 0, then allocate memory for IPv4 entries
            if(pDnsSDcpt->IPv4EntriesPerDNSName)
            {
                pE->pip4Address = (IPV4_ADDR *)pMemoryBlock;
            }
#if defined(TCPIP_STACK_USE_IPV6)
            if(pDnsSDcpt->IPv6EntriesPerDNSName)
            {
                pE->pip6Address = (IPV6_ADDR *)(pMemoryBlock+pDnsSDcpt->IPv4EntriesPerDNSName*(sizeof(IPV4_ADDR)));
            }
#endif
            // allocate Hostname
            if(TCPIP_DNSS_HOST_NAME_LEN != 0)
            {
                pE->pHostName = (uint8_t *)(pMemoryBlock+(pDnsSDcpt->IPv4EntriesPerDNSName*(sizeof(IPV4_ADDR)))
#if defined(TCPIP_STACK_USE_IPV6)
                            + (pDnsSDcpt->IPv6EntriesPerDNSName * (sizeof(IPV6_ADDR)))
#endif
                            );
            }
            
        }
    }

    if(stackCtrl->pNetIf->Flags.bIsDnsServerEnabled!= 0)
    {
        _DNSS_Enable(stackCtrl->pNetIf,false);
    }
	
    return true;
}

static bool _DNSS_SendResponse(DNSS_HEADER *dnsHeader,TCPIP_NET_IF *pNet)
{
    TCPIP_UINT16_VAL    recordType;
    DNSS_DCPT   *pDnsSrvDcpt;
    UDP_SOCKET  s;
    OA_HASH_ENTRY* hE=NULL;
    DNSS_HASH_ENTRY *dnsSHE = NULL;
    uint8_t *pMemoryBlock = NULL;
    uint16_t resAnswerRRs=0;
    uint32_t ttlTime = 0;
    uint8_t *txbuf;
    uint8_t  count=0;
    uint16_t     offset=0;
    uint32_t   servTxMsgSize=0;
    uint32_t   txBufPos = 0;
    uint8_t    hostNamePos=0;
#if defined (TCPIP_STACK_USE_IPV6)
    uint8_t     i=0;
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
    _DNSCopyRXNameToTX(s);   // Copy hostname of first question over to TX packet
    if(strlen((char*)hostNameWithDot) == 0)
    {       
        return false;
    }
    // Get the Record type
    _DNSSGetRecordType(s,&recordType);
    switch(recordType.Val)
    {
        case TCPIP_DNSS_TYPE_A:
#if defined(TCPIP_STACK_USE_IPV6)
        case TCPIP_DNSS_TYPE_AAAA:
#endif
            break;
        default:            
            return false;
    }
    if(!pDnsSrvDcpt->replyWithBoardInfo)
    {
        hE = TCPIP_OAHASH_EntryLookup(pDnsSrvDcpt->dnssHashDcpt, (uint8_t *)hostNameWithDot);
        if(hE != 0)
        {
            dnsSHE = (DNSS_HASH_ENTRY*)hE;
            pMemoryBlock = (uint8_t*)dnsSHE->memblk;
        }
        else
        {
            return false;
        }
    }
    // update Answer field
    // If the client Query answer is zero, then Response will have all the answers which is present in the cache
    // else if the client query answer count is more than the available answer counts  of the cache, then Answer RRs should
    // be the value of available entries in the cache , else if only the limited Answer RRs
    if(recordType.Val == TCPIP_DNSS_TYPE_A)
    {
        if(pDnsSrvDcpt->replyWithBoardInfo)
        {
            resAnswerRRs = 1;
            ttlTime = TCPIP_DNSS_TTL_TIME;
        }
        else if(hE != 0)
        {
            if((dnsHeader->wAnswerRRs.Val == 0) || (dnsHeader->wAnswerRRs.Val > dnsSHE->nIPv4Entries))
            {
            // all the available IPv4 address entries
                resAnswerRRs = dnsSHE->nIPv4Entries;
            }
            else  // only limited entries
            {
                resAnswerRRs = dnsHeader->wAnswerRRs.Val;
            }
            // ttl time  w.r.t configured per entry
            // if the validityTime is not equal to 0
            if(dnsSHE->validityTime.Val != 0)
            {
                ttlTime = dnsSHE->validityTime.Val - ((SYS_TMR_TickCountGet() - dnsSHE->tInsert)/SYS_TMR_TickCounterFrequencyGet());
            }
            // else TTL time will be default value of TCPIP_DNSS_PERMANENT_ENTRY_TTL_TIME
            else
            {
                ttlTime = TCPIP_DNSS_PERMANENT_ENTRY_TTL_TIME;
            }
        }
        else
        {
            return false;
        }
    }
#if defined(TCPIP_STACK_USE_IPV6)
    else if(recordType.Val == TCPIP_DNSS_TYPE_AAAA)
    {
        if(pDnsSrvDcpt->replyWithBoardInfo)
        {
            resAnswerRRs = 1;
            ttlTime = TCPIP_DNSS_TTL_TIME;
        }
        else if(hE != 0)
        {
            if((dnsHeader->wAnswerRRs.Val == 0) || (dnsHeader->wAnswerRRs.Val > dnsSHE->nIPv6Entries))
            {
            // all the available IPv6 address entries
                resAnswerRRs = dnsSHE->nIPv6Entries;
            }
            else  // only limited entries
            {
                resAnswerRRs = dnsHeader->wAnswerRRs.Val;
            }
             // ttl time  w.r.t configured per entry
            // if the validityTime is not equal to 0
            if(dnsSHE->validityTime.Val != 0)
            {
                ttlTime = dnsSHE->validityTime.Val - ((SYS_TMR_TickCountGet() - dnsSHE->tInsert)/SYS_TMR_TickCounterFrequencyGet());
            }
            // else ttl time will be default value of TCPIP_DNSS_PERMANENT_ENTRY_TTL_TIME
            else
            {
                ttlTime = TCPIP_DNSS_PERMANENT_ENTRY_TTL_TIME;
            }
        }
        else
        {
            return false;
        }
    }
#endif
    offset = 0xC00C; // that is the location at 0x0c ( 12) DNS packet compression RFC 1035
    servTxMsgSize = sizeof(DNSS_HEADER)         // DNS header
                    + countWithLen+2+2;  // Query hostname + type + class
    if(recordType.Val == TCPIP_DNSS_TYPE_A)
    {
        // offset + record type+class+ttl+ip type+size of IP address * number of answers present in the HASH table
        servTxMsgSize += resAnswerRRs*(2+2+2+4+2+sizeof(IPV4_ADDR));
    }
#if defined(TCPIP_STACK_USE_IPV6)
    else if(recordType.Val == TCPIP_DNSS_TYPE_AAAA)
    {
        // offset + record type+class+ttl+ip type+size of IP address * number of answers present ih HASH table
        servTxMsgSize += resAnswerRRs*(2+2+2+4+2+sizeof(IPV6_ADDR));
    }
#endif
    // check that we can transmit a DNS response packet
    if(!TCPIP_UDP_TxPutIsReady(s, servTxMsgSize))
    {
        TCPIP_UDP_OptionsSet(s, UDP_OPTION_TX_BUFF, (void*)(unsigned int)servTxMsgSize);
        return false;
    }
     //this will put the start pointer at the beginning of the TX buffer
    TCPIP_UDP_TxOffsetSet(s,0,false);

    //Get the write pointer:
    txbuf = TCPIP_UDP_TxPointerGet(s);
    if(txbuf == 0)
    {
       return false;
    }
    txBufPos = 0;
    // Write DNS Server response packet
    // Transaction ID
    TCPIP_DNSS_DataPut(txbuf,txBufPos++,dnsHeader->wTransactionID.v[1]);
    TCPIP_DNSS_DataPut(txbuf,txBufPos++,dnsHeader->wTransactionID.v[0]);

    if(dnsHeader->wFlags.Val & 0x0100)
    {
        TCPIP_DNSS_DataPut(txbuf,txBufPos++,0x81); // Message is a response with recursion desired
    }
    else
    {
        TCPIP_DNSS_DataPut(txbuf,txBufPos++,0x80); // Message is a response without recursion desired flag set
    }
    TCPIP_DNSS_DataPut(txbuf,txBufPos++,0x80); // Recursion available

    // Question
    TCPIP_DNSS_DataPut(txbuf,txBufPos++,dnsHeader->wQuestions.v[1]);
    TCPIP_DNSS_DataPut(txbuf,txBufPos++,dnsHeader->wQuestions.v[0]);

    // Answer
    TCPIP_DNSS_DataPut(txbuf,txBufPos++,resAnswerRRs>>8&0xFF);
    TCPIP_DNSS_DataPut(txbuf,txBufPos++,resAnswerRRs&0xFF);

    // send Authority and Additional RRs as 0 , It will change latter 
    // when we support Authentication and Additional DNS info
    TCPIP_DNSS_DataPut(txbuf,txBufPos++,0);
    TCPIP_DNSS_DataPut(txbuf,txBufPos++,0);
    TCPIP_DNSS_DataPut(txbuf,txBufPos++,0);
    TCPIP_DNSS_DataPut(txbuf,txBufPos++,0);
    // Prepare all the queries
    for(hostNamePos=0;hostNamePos<countWithLen;hostNamePos++)
    {
        TCPIP_DNSS_DataPut(txbuf,txBufPos++,hostNameWithLen[hostNamePos]);
    }
    // Record Type
    TCPIP_DNSS_DataPut(txbuf,txBufPos++,recordType.v[1]);
    TCPIP_DNSS_DataPut(txbuf,txBufPos++,recordType.v[0]);
    // Class
    TCPIP_DNSS_DataPut(txbuf,txBufPos++,0x00);
    TCPIP_DNSS_DataPut(txbuf,txBufPos++,0x01);

    // Prepare Answer for all the answers
    for(count=0;count <resAnswerRRs;count++)
    {
        // Put Host name Pointer As per RFC1035 DNS compression
        TCPIP_DNSS_DataPut(txbuf,txBufPos++,offset>>8 &0xFF);
        TCPIP_DNSS_DataPut(txbuf,txBufPos++,offset&0xFF);

        // Record Type
        TCPIP_DNSS_DataPut(txbuf,txBufPos++,recordType.v[1]);
        TCPIP_DNSS_DataPut(txbuf,txBufPos++,recordType.v[0]);
        // Class
        TCPIP_DNSS_DataPut(txbuf,txBufPos++,0x00);
        TCPIP_DNSS_DataPut(txbuf,txBufPos++,0x01);
        // TTL
        TCPIP_DNSS_DataPut(txbuf,txBufPos++,ttlTime>>24&0xFF);
        TCPIP_DNSS_DataPut(txbuf,txBufPos++,ttlTime>>16&0xFF);
        TCPIP_DNSS_DataPut(txbuf,txBufPos++,ttlTime>>8&0xFF);
        TCPIP_DNSS_DataPut(txbuf,txBufPos++,ttlTime&0xFF);
        if(recordType.Val == TCPIP_DNSS_TYPE_A)
        {
            // Length for TYPE_A
            TCPIP_DNSS_DataPut(txbuf,txBufPos++,0x00);
            TCPIP_DNSS_DataPut(txbuf,txBufPos++,0x04);
            if(hE != 0)
            {
                dnsSHE->pip4Address = (IPV4_ADDR *)pMemoryBlock;
                if(dnsSHE->pip4Address == 0)
                {
                   return false;
                }
                TCPIP_DNSS_DataPut(txbuf,txBufPos++,dnsSHE->pip4Address[count].v[0]);
                TCPIP_DNSS_DataPut(txbuf,txBufPos++,dnsSHE->pip4Address[count].v[1]);
                TCPIP_DNSS_DataPut(txbuf,txBufPos++,dnsSHE->pip4Address[count].v[2]);
                TCPIP_DNSS_DataPut(txbuf,txBufPos++,dnsSHE->pip4Address[count].v[3]);
            }
            else
            {
                TCPIP_DNSS_DataPut(txbuf,txBufPos++,pNet->netIPAddr.v[0]);
                TCPIP_DNSS_DataPut(txbuf,txBufPos++,pNet->netIPAddr.v[1]);
                TCPIP_DNSS_DataPut(txbuf,txBufPos++,pNet->netIPAddr.v[2]);
                TCPIP_DNSS_DataPut(txbuf,txBufPos++,pNet->netIPAddr.v[3]);
            }
        }
#if defined(TCPIP_STACK_USE_IPV6)
        else if(recordType.Val == TCPIP_DNSS_TYPE_AAAA)
        {
            // Length for TYPE_A
            TCPIP_DNSS_DataPut(txbuf,txBufPos++,0x00);     // Data Length 16 bytes
            TCPIP_DNSS_DataPut(txbuf,txBufPos++,0x10);  // sizeof (IPV6_ADDR)
            if(hE != 0)
            {
                dnsSHE->pip6Address = (IPV6_ADDR *)(pMemoryBlock+pDnsSrvDcpt->IPv4EntriesPerDNSName*sizeof(IPV4_ADDR));
                if(dnsSHE->pip6Address == 0)
                {
                   return false;
                }
                for(i=0;i<sizeof(IPV6_ADDR);i++)
                {
                    TCPIP_DNSS_DataPut(txbuf,txBufPos++,dnsSHE->pip6Address[count].v[i]);
                }
            }
            else
            {
                pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pNet);
                addressPointer = (IPV6_ADDR_STRUCT *)pIpv6Config->listIpv6UnicastAddresses.head;
                // only one IPv6 uni-cast address
                for(i=0;i<sizeof(IPV6_ADDR);i++)
                {
                    TCPIP_DNSS_DataPut(txbuf,txBufPos++,addressPointer->address.v[i]);
                }
            }
        }
#endif
    }
    // Transmit all the server bytes
    //TCPIP_UDP_ArrayPut(s,txbuf,txBufPos);
    // Once it is completed writing into the buffer, you need to update the Tx offset again,
    // because the socket flush function calculates how many bytes are in the buffer using the current write pointer:
    TCPIP_UDP_TxOffsetSet(s,txBufPos, false);
    TCPIP_UDP_Flush(s);
    return true;
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_DNSS_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    DNSS_DCPT *pDnsSDcpt;
    pDnsSDcpt = &gDnsSrvDcpt;
    if(pDnsSDcpt->dnsSrvInitCount > 0)
    {	// we're up and running        
        if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
        {
            if(--pDnsSDcpt->dnsSrvInitCount == 0)
            {	// all closed
                // release resources
                if(pDnsSDcpt->dnsSSignalHandle)
                {
                    _TCPIPStackSignalHandlerDeregister(pDnsSDcpt->dnsSSignalHandle);
                    pDnsSDcpt->dnsSSignalHandle = 0;
                    pDnsSDcpt->dnsSTickPending = 0;
                    pDnsSDcpt->dnsSTimeMseconds = 0;
                }
                if(pDnsSDcpt->dnsSrvSocket != INVALID_UDP_SOCKET)
                {
                    TCPIP_UDP_Close(pDnsSDcpt->dnsSrvSocket);
                }
            }
            // remove all the cache entries
            _DNSSRemoveCacheEntries();
        }
    }
}

static  void _DNSSRemoveCacheEntries(void)
{
    OA_HASH_ENTRY* pBkt=NULL;
    DNSS_HASH_ENTRY *dnsSHE=NULL;
    DNSS_DCPT       *pDnsSDcpt=NULL;
    uint8_t *pMemoryBlock=NULL;
    size_t      bktIx=0;
        
    pDnsSDcpt = &gDnsSrvDcpt;   

    if(pDnsSDcpt->dnssHashDcpt)
    {
        for(bktIx = 0; bktIx < pDnsSDcpt->dnssHashDcpt->hEntries; bktIx++)
        {
            pBkt = TCPIP_OAHASH_EntryGet(pDnsSDcpt->dnssHashDcpt, bktIx);
            dnsSHE = (DNSS_HASH_ENTRY*)pBkt;
            pMemoryBlock = (uint8_t*)dnsSHE->memblk;

            TCPIP_HEAP_Free(pDnsSDcpt->memH,pMemoryBlock);
            dnsSHE->nIPv4Entries = 0;
#if defined(TCPIP_STACK_USE_IPV6)
            dnsSHE->nIPv6Entries = 0;
#endif
            TCPIP_OAHASH_EntryRemove(pDnsSDcpt->dnssHashDcpt,pBkt);

        }
        TCPIP_HEAP_Free(pDnsSDcpt->memH,pDnsSDcpt->dnssHashDcpt);
        pDnsSDcpt->dnssHashDcpt = NULL;
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

TCPIP_DNSS_RESULT TCPIP_DNSS_AddressCntGet(int index, char* hostName, size_t hostSize, size_t* ipCount)
{
    DNSS_HASH_ENTRY* pDnsSHE;
    OA_HASH_ENTRY	*hE;
    OA_HASH_DCPT	*pOH;
    DNSS_DCPT*	pDnsSDcpt;

    pDnsSDcpt = &gDnsSrvDcpt;
    pOH = pDnsSDcpt->dnssHashDcpt;
    if((hostName == 0 || hostSize == 0) || (pDnsSDcpt->dnssHashDcpt==NULL))
    {
        return TCPIP_DNSS_RES_MEMORY_FAIL;
    }
    if(index >= pOH->hEntries)
    {
        return TCPIP_DNSS_RES_NO_SERVICE;
    }

    hE = TCPIP_OAHASH_EntryGet(pOH, index);
    if((hE->flags.busy != 0) && (hE->flags.value & DNSS_FLAG_ENTRY_COMPLETE))
    {
       pDnsSHE = (DNSS_HASH_ENTRY*)hE;
       strncpy((char*)hostName, (char*)pDnsSHE->pHostName, hostSize - 1);
       hostName[hostSize - 1] = 0;
       *ipCount = pDnsSHE->nIPv4Entries;
#if defined(TCPIP_STACK_USE_IPV6)
        *ipCount += pDnsSHE->nIPv6Entries;
#endif
        return TCPIP_DNSS_RES_OK;
    }
    return TCPIP_DNSS_RES_NO_ENTRY;

}

TCPIP_DNSS_RESULT TCPIP_DNSS_EntryGet(uint8_t * hostName,IP_ADDRESS_TYPE type,int index,IP_MULTI_ADDRESS* pGetAdd,uint32_t *ttlTime)
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
    if((hostName == 0) || (pDnsSDcpt->dnssHashDcpt==NULL))
    {
        return TCPIP_DNSS_RES_NO_ENTRY;
    }
    hE = TCPIP_OAHASH_EntryLookup(pDnsSDcpt->dnssHashDcpt, (uint8_t *)hostName);
    if(hE == 0)
    {  
        return TCPIP_DNSS_RES_NO_ENTRY;
    }
    dnsSHE = (DNSS_HASH_ENTRY*)hE;
    
    pMemoryBlock = (uint8_t*)dnsSHE->memblk;

    if(type == IP_ADDRESS_TYPE_IPV4)
    {
        if(index >= dnsSHE->nIPv4Entries)
            return TCPIP_DNSS_RES_NO_SERVICE;
        dnsSHE->pip4Address = (IPV4_ADDR *)pMemoryBlock;
        if(dnsSHE->pip4Address == 0)
            return TCPIP_DNSS_RES_NO_ENTRY;

        if(dnsSHE->pip4Address[index].Val != 0)
        {
            pGetAdd->v4Add.Val = dnsSHE->pip4Address[index].Val;
        }
        else
        {
            return TCPIP_DNSS_RES_NO_IPADDRESS;
        }
        
        if(dnsSHE->validityTime.Val != 0)
        {
            *ttlTime = dnsSHE->validityTime.Val - ((SYS_TMR_TickCountGet() - dnsSHE->tInsert)/SYS_TMR_TickCounterFrequencyGet());
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
            return TCPIP_DNSS_RES_NO_SERVICE;
        dnsSHE->pip6Address = (IPV6_ADDR *)(pMemoryBlock+pDnsSDcpt->IPv4EntriesPerDNSName*sizeof(IPV4_ADDR));
        if(dnsSHE->pip6Address == 0)
            return TCPIP_DNSS_RES_NO_ENTRY;

        if(memcmp(dnsSHE->pip6Address[i].v, ipv6_addr_unspecified.v, sizeof(IPV6_ADDR)) != 0)
        {
            memcpy(pGetAdd->v6Add.v,dnsSHE->pip6Address[i].v,sizeof(IPV6_ADDR));
        }
        else
        {
            return TCPIP_DNSS_RES_NO_IPADDRESS;
        }
        
        if(dnsSHE->validityTime.Val != 0)
        {
            *ttlTime = dnsSHE->validityTime.Val - ((SYS_TMR_TickCountGet() - dnsSHE->tInsert)/SYS_TMR_TickCounterFrequencyGet());
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
	
    if((name == 0) || (pDnsSDcpt->dnssHashDcpt==NULL))
    {
        return TCPIP_DNSS_RES_MEMORY_FAIL;
    }

    dnssCacheEntry.sHostNameData = (uint8_t *)name;
    dnssCacheEntry.recordType = type;
    dnssCacheEntry.entryTimeout.Val = entryTimeout;
    if(dnssCacheEntry.recordType == IP_ADDRESS_TYPE_IPV4)
    {
        dnssCacheEntry.ip4Address.Val = pAdd->v4Add.Val;
    }
#if defined(TCPIP_STACK_USE_IPV6)     
    else if(dnssCacheEntry.recordType == IP_ADDRESS_TYPE_IPV6)
    {
        memcpy(&dnssCacheEntry.ip6Address,&pAdd->v6Add,sizeof(IPV6_ADDR));
    }
#endif	
    else
    {
        return TCPIP_DNSS_RES_NO_ENTRY;
    }
    hE = TCPIP_OAHASH_EntryLookup(pDnsSDcpt->dnssHashDcpt, dnssCacheEntry.sHostNameData);
    if(hE != 0)
    {
        dnsSHE = (DNSS_HASH_ENTRY*)hE;
        return _DNSSUpdateHashEntry(dnsSHE, dnssCacheEntry);
    }

    return _DNSSSetHashEntry(DNSS_FLAG_ENTRY_COMPLETE, dnssCacheEntry);

}

static  TCPIP_DNSS_RESULT  _DNSSUpdateHashEntry( DNSS_HASH_ENTRY *dnsSHE,TCPIP_DNSS_CACHE_ENTRY dnssCacheEntry)
{
    DNSS_DCPT       *pDnsSDcpt; 
    uint8_t *pMemoryBlock;    
    pDnsSDcpt = &gDnsSrvDcpt;
    uint8_t     i=0;

    pMemoryBlock = (uint8_t*)dnsSHE->memblk;

    if(dnssCacheEntry.recordType == IP_ADDRESS_TYPE_IPV4)
    {
        if(dnsSHE->nIPv4Entries >= pDnsSDcpt->IPv4EntriesPerDNSName)
            return TCPIP_DNSS_RES_CACHE_FULL;
        dnsSHE->pip4Address = (IPV4_ADDR *)pMemoryBlock;
        if(dnsSHE->pip4Address == 0)
            return TCPIP_DNSS_RES_MEMORY_FAIL;
        for(i=0;i<pDnsSDcpt->IPv4EntriesPerDNSName;i++)
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
    if(dnssCacheEntry.recordType == IP_ADDRESS_TYPE_IPV6)
    {
        if(dnsSHE->nIPv6Entries >= pDnsSDcpt->IPv6EntriesPerDNSName)
            return TCPIP_DNSS_RES_CACHE_FULL;
        dnsSHE->pip6Address = (IPV6_ADDR *)(pMemoryBlock+pDnsSDcpt->IPv4EntriesPerDNSName*sizeof(IPV4_ADDR));
        if(dnsSHE->pip6Address == 0)
            return TCPIP_DNSS_RES_MEMORY_FAIL;
        for(i=0;i<pDnsSDcpt->IPv6EntriesPerDNSName;i++)
        {
            if(memcmp(&dnsSHE->pip6Address[i],&dnssCacheEntry.ip6Address,sizeof(IPV6_ADDR)) == 0)
            {
                return TCPIP_DNSS_RES_DUPLICATE_ENTRY;
            }
        }
        memcpy( &dnsSHE->pip6Address[dnsSHE->nIPv6Entries],&dnssCacheEntry.ip6Address,sizeof(IPV6_ADDR));
        dnsSHE->nIPv6Entries ++ ;
    }
#endif
    dnsSHE->validityTime = dnssCacheEntry.entryTimeout;
    
    return TCPIP_DNSS_RES_OK;
}

static  TCPIP_DNSS_RESULT  _DNSSSetHashEntry( DNSS_HASH_ENTRY_FLAGS newFlags,TCPIP_DNSS_CACHE_ENTRY dnssCacheEntry)
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
    if(hE == 0)
    {
        return TCPIP_DNSS_RES_CACHE_FULL;
    }
    dnsSHE = (DNSS_HASH_ENTRY*)hE;
    pMemoryBlock = dnsSHE->memblk;
    dnsSHE->hEntry.flags.value &= ~DNSS_FLAG_ENTRY_VALID_MASK;
    dnsSHE->hEntry.flags.value |= newFlags;
    dnsSHE->memblk = pMemoryBlock;
    dnsSHE->recordType = dnssCacheEntry.recordType;
    
    if(dnssCacheEntry.recordType == IP_ADDRESS_TYPE_IPV4)
    {
        if(dnsSHE->nIPv4Entries >= pDnsSDcpt->IPv4EntriesPerDNSName)
            return TCPIP_DNSS_RES_CACHE_FULL;
        dnsSHE->pip4Address = (IPV4_ADDR *)pMemoryBlock;
        if(dnsSHE->pip4Address == 0)
            return TCPIP_DNSS_RES_MEMORY_FAIL;
        dnsSHE->pip4Address[dnsSHE->nIPv4Entries].Val = 
                            dnssCacheEntry.ip4Address.Val;
        dnsSHE->nIPv4Entries ++ ;
    }
#if defined(TCPIP_STACK_USE_IPV6)
    if(dnssCacheEntry.recordType == IP_ADDRESS_TYPE_IPV6)
    {
        if(dnsSHE->nIPv6Entries >= pDnsSDcpt->IPv6EntriesPerDNSName)
            return TCPIP_DNSS_RES_CACHE_FULL;
        dnsSHE->pip6Address = (IPV6_ADDR *)(pMemoryBlock+pDnsSDcpt->IPv4EntriesPerDNSName*sizeof(IPV4_ADDR));
        if(dnsSHE->pip6Address == 0)
            return TCPIP_DNSS_RES_MEMORY_FAIL;
        memcpy( &dnsSHE->pip6Address[dnsSHE->nIPv6Entries],&dnssCacheEntry.ip6Address,sizeof(IPV6_ADDR));
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
    int         i=0;
    bool        addrISPresent=false;
    
    pDnsSDcpt = &gDnsSrvDcpt;
    if((name == 0) || (pDnsSDcpt->dnssHashDcpt==NULL))
    {
        return TCPIP_DNSS_RES_MEMORY_FAIL;
    }
    hE = TCPIP_OAHASH_EntryLookup(pDnsSDcpt->dnssHashDcpt, (uint8_t *)name);
    if(hE == 0)
    {  
        return TCPIP_DNSS_RES_NO_ENTRY;
    }
    dnsSHE = (DNSS_HASH_ENTRY*)hE;
    if(type != dnsSHE->recordType)
    {  
        return TCPIP_DNSS_RES_NO_ENTRY;
    }
    pMemoryBlock = (uint8_t*)dnsSHE->memblk;

    if(dnsSHE->recordType == IP_ADDRESS_TYPE_IPV4)
    {
        if(dnsSHE->nIPv4Entries > pDnsSDcpt->IPv4EntriesPerDNSName)
            return TCPIP_DNSS_RES_MEMORY_FAIL;
        dnsSHE->pip4Address = (IPV4_ADDR *)pMemoryBlock;
        if(dnsSHE->pip4Address == 0)
            return TCPIP_DNSS_RES_MEMORY_FAIL;
        for(i=0;i<pDnsSDcpt->IPv4EntriesPerDNSName;i++)
        {
            if(dnsSHE->pip4Address[i].Val == pAdd->v4Add.Val)
            {
                dnsSHE->nIPv4Entries--;
                dnsSHE->pip4Address[i].Val = 0;
                addrISPresent = true;
                break;
            }
        }
    }
#if defined(TCPIP_STACK_USE_IPV6)
    if(dnsSHE->recordType == IP_ADDRESS_TYPE_IPV6)
    {
        if(dnsSHE->nIPv6Entries > pDnsSDcpt->IPv6EntriesPerDNSName)
            return TCPIP_DNSS_RES_MEMORY_FAIL;
        dnsSHE->pip6Address = (IPV6_ADDR *)(pMemoryBlock+pDnsSDcpt->IPv4EntriesPerDNSName*sizeof(IPV4_ADDR));
        if(dnsSHE->pip6Address == 0)
            return TCPIP_DNSS_RES_MEMORY_FAIL;
        for(i=0;i<pDnsSDcpt->IPv6EntriesPerDNSName;i++)
        {
            if(memcmp(&dnsSHE->pip6Address[i],&pAdd->v6Add,sizeof(IPV6_ADDR)) == 0)
            {
                dnsSHE->nIPv6Entries--;
                memset(&dnsSHE->pip6Address[i], 0,sizeof(IPV6_ADDR));
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
   if(!dnsSHE->nIPv4Entries 
#if defined(TCPIP_STACK_USE_IPV6)
     && !dnsSHE->nIPv6Entries
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

static bool TCPIP_DNSS_DataPut(uint8_t * buf,uint32_t pos,uint8_t val)
{
    if(buf == 0)
        return false;
    buf[pos] = val;
    return true;
}

void TCPIP_DNSS_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);
    
    if((sigPend & TCPIP_MODULE_SIGNAL_TMO) != 0)
    { // regular TMO occurred
        TCPIP_DNSS_CacheTimeTask();
    }

    if(gDnsSrvDcpt.flags.bits.DNSServInUse == false || gDnsSrvDcpt.dnssHashDcpt==NULL)
    {
        return;
    }


    if((sigPend & TCPIP_MODULE_SIGNAL_RX_PENDING) != 0)
    { //  RX signal occurred
        TCPIP_DNSS_Process();
    }

}

// send a signal to the DNSS module that data is available
// no manager alert needed since this normally results as a higher layer (UDP) signal
static void _DNSSSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_UDP_SIGNAL_RX_DATA)
    {
        _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}



static void TCPIP_DNSS_Process(void)
{
    UDP_SOCKET  s;    
    UDP_SOCKET_INFO     udpSockInfo;
    TCPIP_NET_IF* pNet=NULL;
    DNSS_HEADER DNSServHeader;
    uint32_t recvLen=0;
    static TCPIP_UINT16_VAL transactionId;
  
    s = gDnsSrvDcpt.dnsSrvSocket;

    while(1)
    {
     // See if a DNS query packet has arrived
        recvLen = TCPIP_UDP_GetIsReady(s);
        if(recvLen == 0)
        {
           break;
        }
        if(recvLen > (sizeof(dnsSrvRecvByte)-1))
        {
            TCPIP_UDP_Discard(s);
            continue;
        }
        gDnsSrvBytePos = 0;
        TCPIP_UDP_SocketInfoGet(s, &udpSockInfo);
        pNet = (TCPIP_NET_IF*)udpSockInfo.hNet;
        // check if DHCP server is enabled or Not for this incoming packet interface
        if(!TCPIP_DNSS_ValidateIf(pNet))
        {
            TCPIP_UDP_Discard(s);
            continue;
        }
        // Read DNS header
        TCPIP_UDP_ArrayGet(s, (uint8_t*)dnsSrvRecvByte, recvLen);
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
            TCPIP_UDP_Discard(s);
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
        if((DNSServHeader.wFlags.Val & 0x8000) == 0x8000u)
        {
            TCPIP_UDP_Discard(s);
            break;
        }
        // Ignore this packet if there are no questions in it
        if(DNSServHeader.wQuestions.Val == 0u)
        {
            TCPIP_UDP_Discard(s);
            break;
        }
        // send the DNS client query response
        if(!_DNSS_SendResponse(&DNSServHeader,pNet))
        {
            TCPIP_UDP_Discard(s);
            continue;
        }
    }
}
// returns true if the pIf can be selected for DNS traffic
// false otherwise
static bool TCPIP_DNSS_ValidateIf(TCPIP_NET_IF* pIf)
{
    // check that DNS is enabled
    if(TCPIP_DNSS_IsEnabled(pIf))
    {
        // check that interface is up and linked
        if(_TCPIPStackHandleToNetLinked(pIf) != 0)
        {
            // check for a valid address
            if(!_TCPIPStackIsConfig(pIf) && _TCPIPStackNetAddress(pIf) != 0)
            {
                return true;
            }
        }
    }
    return false;
}

/*****************************************************************************
  Function:
	static void _DNSCopyRXNameToTX(UDP_SOCKET s)

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
static void _DNSCopyRXNameToTX(UDP_SOCKET s)
{
    uint16_t w;
    uint8_t i=0,j=0;
    uint8_t len;
    //uint8_t data[64]={0};
    
    countWithDot=0;
    countWithLen=0;
    while(1)
    {
        // Get first byte which will tell us if this is a 16-bit pointer or the
        // length of the first of a series of labels
        //	return;
        i = TCPIP_DNSS_DataGet(gDnsSrvBytePos++);
		
        // Check if this is a pointer, if so, get the remaining 8 bits and seek to the pointer value
        if((i & 0xC0u) == 0xC0u)
        {
            ((uint8_t*)&w)[1] = i & 0x3F;
            w = TCPIP_DNSS_DataGet(gDnsSrvBytePos++);
            gDnsSrvBytePos =  w;
            continue;
        }

        // Write the length byte
        len = i;
        if(countWithLen==0 && countWithDot==0)
        {
            hostNameWithLen[countWithLen++]=len;
        }
        else
        {
            hostNameWithLen[countWithLen++]=len;
            // when it reached the end of hostname , then '.' is not required
            if(len!=0)
                hostNameWithDot[countWithDot++]='.';
        }	
		
        // Exit if we've reached a zero length label
        if(len == 0u)
        {
            hostNameWithDot[countWithDot] = 0;
            return;
        }

        //UDPGetArray(s,data,len);
        for(j=0;j<len;j++)
        {
            i = TCPIP_DNSS_DataGet(gDnsSrvBytePos++);
        // update the hostNameWithDot with data 
            hostNameWithLen[countWithLen++] = i;
        
        // update the hostNameWithLen with data 
            hostNameWithDot[countWithDot++] = i;
        }

        if((countWithLen > TCPIP_DNSS_HOST_NAME_LEN) || (countWithDot > TCPIP_DNSS_HOST_NAME_LEN))
        {
            return;
        }        
    }
}

static void TCPIP_DNSS_CacheTimeTask(void)
{
    DNSS_HASH_ENTRY* pDnsSHE;
    OA_HASH_ENTRY	*hE;
    int 		bktIx=0;
    OA_HASH_DCPT	*pOH;
    DNSS_DCPT*	pDnsSDcpt;


    
    pDnsSDcpt = &gDnsSrvDcpt;
    pOH = pDnsSDcpt->dnssHashDcpt;
    if(pDnsSDcpt->dnssHashDcpt==NULL)
    {
        return;
    }
    gDnsSrvDcpt.dnsSTimeMseconds += TCPIP_DNSS_TASK_PROCESS_RATE;

// check the lease values and if there is any entry whose lease value exceeds the lease duration remove the lease entries from the HASH.

    for(bktIx = 0; bktIx < pOH->hEntries; bktIx++)
    {
        hE = TCPIP_OAHASH_EntryGet(pOH, bktIx);
    	if((hE->flags.busy != 0) && (hE->flags.value & DNSS_FLAG_ENTRY_COMPLETE))
    	{
            pDnsSHE = (DNSS_HASH_ENTRY*)hE;
            // only check the entries which has a valid validity time
            // if the entry has 0 validity time, then that entry is a permanent entry
            // no need to be removed from here
            if(pDnsSHE->validityTime.Val != 0)
            {
                if(((SYS_TMR_TickCountGet() - pDnsSHE->tInsert)/SYS_TMR_TickCounterFrequencyGet()) > pDnsSHE->validityTime.Val )
                {
                    pDnsSHE->tInsert = 0;
                    TCPIP_OAHASH_EntryRemove(pOH,hE);

                    pDnsSHE->nIPv4Entries = 0;
    #ifdef TCPIP_STACK_USE_IPV6
                    pDnsSHE->nIPv6Entries = 0;
    #endif    
                }
            }
      	}       
    }   
}

bool TCPIP_DNSS_IsEnabled(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(hNet);
    DNSS_DCPT        *pDnsSDcpt;

    pDnsSDcpt = &gDnsSrvDcpt;
    if(pDnsSDcpt->dnssHashDcpt==NULL)
    {
        return false;
    }
    if(pNetIf)
    {
        if((pNetIf->Flags.bIsDnsServerEnabled == true) && (pDnsSDcpt->flags.bits.DNSServInUse == true))
        {
            return true;
        }
    }
    return false;
}

bool TCPIP_DNSS_Enable(TCPIP_NET_HANDLE hNet)
{
    return _DNSS_Enable(hNet, true);
}

static bool _DNSS_Enable(TCPIP_NET_HANDLE hNet, bool checkIfUp)
{
    DNSS_DCPT        *pDnsSDcpt;
    TCPIP_NET_IF    *pNetIf;
    
    pDnsSDcpt = &gDnsSrvDcpt;
    if((pDnsSDcpt == 0)||(pDnsSDcpt->dnssHashDcpt==NULL))
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
    
    if(pNetIf == 0 || TCPIP_STACK_DNSServiceCanStart(pNetIf, TCPIP_STACK_DNS_SERVICE_SERVER) == false)
    {
        return false;
    }
    pNetIf->Flags.bIsDnsServerEnabled = true;
    
    if(pDnsSDcpt->dnsSrvSocket == INVALID_UDP_SOCKET)
    {
        pDnsSDcpt->dnsSrvSocket = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_ANY, TCPIP_DNS_SERVER_PORT, 0);
        if( pDnsSDcpt->dnsSrvSocket == INVALID_UDP_SOCKET)
        {
            return false;
        }
        pDnsSDcpt->intfIdx = pNetIf->netIfIx;
        pDnsSDcpt->flags.bits.DNSServInUse = DNS_SERVER_ENABLE;
        TCPIP_UDP_SignalHandlerRegister(pDnsSDcpt->dnsSrvSocket, TCPIP_UDP_SIGNAL_RX_DATA, _DNSSSocketRxSignalHandler, 0);
    }
    return true;
}

bool TCPIP_DNSS_Disable(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNetUp(hNet);
    DNSS_DCPT* pServer;

    pServer  = &gDnsSrvDcpt;
    if((pNetIf == 0)||(pServer->dnssHashDcpt==NULL))
    {
        return false;
    }
    
    if(pNetIf->netIfIx != pServer->intfIdx)
    {
        return false;
    }
    
    if(pServer->flags.bits.DNSServInUse == DNS_SERVER_ENABLE)
    {
        pServer->smState = DNSS_STATE_START;
        pServer->flags.bits.DNSServInUse = DNS_SERVER_DISABLE;
        pNetIf->Flags.bIsDnsServerEnabled = false;
 
        if(pServer->dnsSrvSocket != INVALID_UDP_SOCKET)
        {
            TCPIP_UDP_Close(pServer->dnsSrvSocket);
        }
    }    

    return true;	
}

static void _DNSSGetRecordType(UDP_SOCKET s,TCPIP_UINT16_VAL *recordType)
{
    recordType->v[1] = TCPIP_DNSS_DataGet(gDnsSrvBytePos++);
    recordType->v[0] = TCPIP_DNSS_DataGet(gDnsSrvBytePos++);
}

size_t TCPIP_OAHASH_DNSS_KeyHash(OA_HASH_DCPT* pOH, const void* key)
{
    uint8_t    *dnsHostNameKey;
    size_t      hostnameLen=0;

    dnsHostNameKey = (uint8_t *)key;
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
        return 0;
    }
    for(bktIx = 0; bktIx < pOH->hEntries; bktIx++)
    {
        pBkt = TCPIP_OAHASH_EntryGet(pOH, bktIx);		
        if(pBkt->flags.busy != 0)
        {
            pE = (DNSS_HASH_ENTRY*)pBkt;
            if(pE->validityTime.Val != 0)
            {
                // return the hash bucket entry if the current tick count is more than configured Validity entry time out value.
                if(SYS_TMR_TickCountGet() - pE->tInsert > (pE->validityTime.Val * SYS_TMR_TickCounterFrequencyGet()))
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
    return 0;
}

int TCPIP_OAHASH_DNSS_KeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key)
{
    DNSS_HASH_ENTRY  *pDnssHE;
    uint8_t         *dnsHostNameKey;
    
    pDnssHE =(DNSS_HASH_ENTRY  *)hEntry;
    dnsHostNameKey = (uint8_t *)key;    
    
    return strcmp((const char*)pDnssHE->pHostName,(const char*)dnsHostNameKey);
}

void TCPIP_OAHASH_DNSS_KeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key)
{
    uint8_t    *dnsHostNameKey;
    DNSS_HASH_ENTRY  *pDnssHE;
    size_t          hostnameLen=0;

    if(key==NULL) return;
    
    pDnssHE =(DNSS_HASH_ENTRY  *)dstEntry;
    dnsHostNameKey = (uint8_t *)key;
    hostnameLen = strlen((const char*)dnsHostNameKey);
    if(hostnameLen>TCPIP_DNSS_HOST_NAME_LEN)
    {
        return;
    }
    if(dnsHostNameKey)
    {
        if(pDnssHE->pHostName == NULL) return;
        memset(pDnssHE->pHostName,0,TCPIP_DNSS_HOST_NAME_LEN);
        memcpy(pDnssHE->pHostName,dnsHostNameKey,hostnameLen);
        pDnssHE->pHostName[hostnameLen]='\0';
    }
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
