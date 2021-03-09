/*******************************************************************************
  DNS module private header

  Summary:
    Header definitions file for DNS module
    
  Description:
    This file contains the private definitions for the DNS module
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









#ifndef _DNS_PRIVATE_H_ 
#define _DNS_PRIVATE_H_


// DNS debug levels
#define TCPIP_DNS_DEBUG_MASK_BASIC           (0x0001)
#define TCPIP_DNS_DEBUG_MASK_EVENTS          (0x0002)
#define TCPIP_DNS_DEBUG_MASK_ANSWER_NAMES    (0x0004)
#define TCPIP_DNS_DEBUG_MASK_QUESTION_NAMES  (0x0008)
#define TCPIP_DNS_DEBUG_MASK_VALIDATE        (0x0010)
#define TCPIP_DNS_DEBUG_MASK_ARP_FLUSH       (0x0020)

// enable DNS debugging levels
#define TCPIP_DNS_DEBUG_LEVEL               (0)


/* Max number of DNS servers supported*/
#define TCPIP_DNS_MAX_SERVERS_SUPPORTED     2
#define TCPIP_DNS_HASH_PROBE_STEP           1    // step to advance for hash collision

// receive packet buffer size
#define TCPIP_DNS_RX_BUFFER_SIZE            512

// retries for solving a name
//
#if !defined(TCPIP_DNS_CLIENT_LOOKUP_RETRY_TMO) || (TCPIP_DNS_CLIENT_LOOKUP_RETRY_TMO == 0)
#error "Invalid TCPIP_DNS_CLIENT_LOOKUP_RETRY_TMO value!"
#endif

// 2 retries per interface, to be able to try both DNS servers
// this should match the number of DNS servers per interface
#define _TCPIP_DNS_IF_RETRY_COUNT 2

// once an entry is unsolved and exhausted its retries
// it will be removed from the cache
#define _TCPIP_DNS_CLIENT_CACHE_UNSOLVED_EXPIRE_TMO     1

// a DNS debug event
typedef enum
{
    // perfect match to TCPIP_DNS_EVENT_TYPE 
    TCPIP_DNS_DBG_EVENT_NONE = 0,           // DNS no event
    TCPIP_DNS_DBG_EVENT_NAME_QUERY,         // DNS Query sent
    TCPIP_DNS_DBG_EVENT_NAME_RESOLVED,      // DNS Name resolved
    TCPIP_DNS_DBG_EVENT_NAME_EXPIRED,       // name entry expired
    TCPIP_DNS_DBG_EVENT_NAME_REMOVED,       // name removed to make room for another entry
    TCPIP_DNS_DBG_EVENT_NAME_ERROR,         // no such name reported by the DNS server
    TCPIP_DNS_DBG_EVENT_SOCKET_ERROR,       // a DNS probe could not be sent, socket was busy, TX failed, etc.
    TCPIP_DNS_DBG_EVENT_NO_INTERFACE,       // a DNS probe could not be sent, no DNS interface could be selected

    // additional debug events
    TCPIP_DNS_DBG_EVENT_RR_XTRACT_ERROR,    // failed to extract the name from the received DNS RR
    TCPIP_DNS_DBG_EVENT_RR_STRUCT_ERROR,    // failed to skip fields in the RR structure
    TCPIP_DNS_DBG_EVENT_RR_NO_RECORDS,      // the received DNS RR didn't have records
    TCPIP_DNS_DBG_EVENT_RR_MISMATCH,        // the received DNS RR doesn't match the query
    TCPIP_DNS_DBG_EVENT_RR_DATA_ERROR,      // error when trying to retrieve the DNS RR data

    TCPIP_DNS_DBG_EVENT_COMPLETE_ERROR,     // received DNS transaction for complete name
    TCPIP_DNS_DBG_EVENT_NO_IP_ERROR,        // received DNS transaction contains no IP addresses
    TCPIP_DNS_DBG_EVENT_UNSOLICITED_ERROR,  // received unexpected DNS transaction 
}TCPIP_DNS_DBG_EVENT_TYPE;


// Structure used for writing into the DNS UDP socket
typedef struct
{
    uint8_t *head;      // pointer to allocated buffer
    uint8_t* rdPtr;     // current read pointer, initialized to head
    uint8_t* endPtr;    // end pointer, initialized to head + len
}TCPIP_DNS_RX_DATA;

// DNS ENTRY flags used in hEntry->flags
// note that only the hEntry->flags.user fields
// should be used! 
typedef enum
{
    TCPIP_DNS_FLAG_ENTRY_BUSY         = 0x0001,     // this is used by the hash itself!
    // user flags
    TCPIP_DNS_FLAG_ENTRY_COMPLETE     = 0x0080,     // regular entry, complete
                                                    // else it's incomplete
    TCPIP_DNS_FLAG_ENTRY_TIMEOUT      = 0x0100,     // entry has timed out
                                                  
}TCPIP_DNS_HASH_ENTRY_FLAGS;


typedef enum
{
    TCPIP_DNS_ADDRESS_REC_NONE  = 0x0,      // no type
    TCPIP_DNS_ADDRESS_REC_IPV4  = 0x01,     // IPV4 record
    TCPIP_DNS_ADDRESS_REC_IPV6  = 0x02,     // IPV6 record
}TCPIP_DNS_ADDRESS_REC_MASK;

// DNS Resolver cache entry
typedef struct
{
    OA_HASH_ENTRY               hEntry;         // hash header;
    uint8_t*                    memblk;         // memory block for IPv4, IPv6 and hostname
    uint32_t                    tInsert;        // one time per hash entry
    uint32_t                    tRetry;         // retry time per hash entry
    IPV4_ADDR*                  pip4Address;    // pointer to an array of IPv4: nIPv4Entries entries 
    IPV6_ADDR*                  pip6Address;    // pointer to an array of IPv6: nIPv6Entries entries
    TCPIP_UINT32_VAL            ipTTL;          // Minimum TTL per IPv4 and Ipv6 addresses
    TCPIP_NET_IF*               currNet;        // current Interface used 
    char*                       pHostName;
    // unaligned members
    TCPIP_UINT16_VAL            transactionId;
    uint8_t                     nIPv4Entries;   // number of valid entries in the ip4Address[] array;
    uint8_t                     nIPv6Entries;   // number of valid entries in the ip6Address[] array;
    uint8_t                     resolve_type;   // TCPIP_DNS_RESOLVE_TYPE value
    uint8_t                     currServerIx;   // current server used
    uint8_t                     recordMask;     // a TCPIP_DNS_ADDRESS_REC_MASK mask: IPv6/IPv4 
    uint8_t                     currRetry;      // current retry number for an address resolution
    uint8_t                     nRetries;       // # of retries for address resolution
}TCPIP_DNS_HASH_ENTRY;


typedef struct
{
    OA_HASH_DCPT*           hashDcpt;                       // hash descriptor + hash table entries    
    TCPIP_NET_IF*           strictNet;                      // strict DNS interface, if set
    TCPIP_NET_IF*           prefNet;                        // preferred DNS interface, if set
    tcpipSignalHandle       dnsSignalHandle;
    const void              *memH;
    uint32_t                cacheEntryTMO;
    IP_ADDRESS_TYPE         ipAddressType;
#if (TCPIP_DNS_CLIENT_USER_NOTIFICATION != 0)
    PROTECTED_SINGLE_LIST   dnsRegisteredUsers;
#endif  // (TCPIP_DNS_CLIENT_USER_NOTIFICATION != 0)
    uint32_t                dnsTime;                        // coarse DNS time keeping, seconds
    // unaligned members
    uint16_t                nIPv4Entries;
    uint16_t                nIPv6Entries;
    UDP_SOCKET              dnsSocket;                      // Socket used by DHCP Server
    int16_t                 unsolvedEntries;                // number of entries in the cache that need to be solved
}TCPIP_DNS_DCPT;    // DNS descriptor


typedef enum
{
    TCPIP_DNS_RR_TYPE_QUESTION,
    TCPIP_DNS_RR_TYPE_ANSWER,
    TCPIP_DNS_RR_TYPE_AUTHORITATIVE,
    TCPIP_DNS_RR_TYPE_ADDITIONAL,
    //

    TCPIP_DNS_RR_TYPES,

}TCPIP_DNS_RR_TYPE;


// Structure for the DNS header
typedef struct
{
    TCPIP_UINT16_VAL TransactionID;
    TCPIP_UINT16_VAL Flags;
    TCPIP_UINT16_VAL Questions;
    TCPIP_UINT16_VAL Answers;
    TCPIP_UINT16_VAL AuthoritativeRecords;
    TCPIP_UINT16_VAL AdditionalRecords;
} TCPIP_DNS_HEADER;

// DNS response header for ANSWER, Authorative and Additional packet response header
typedef struct __attribute__((packed))
{
    // Response name is first, but it is variable length and must be retrieved
    // using the DNSDiscardName() function
    TCPIP_UINT16_VAL    ResponseType; // Response Type
    TCPIP_UINT16_VAL    ResponseClass; // Response Class
    TCPIP_UINT32_VAL    ResponseTTL; //Time to live
    TCPIP_UINT16_VAL    ResponseLen; // Response length
} TCPIP_DNS_ANSWER_HEADER;

// DNS event registration

typedef struct  _TAG_TCPIP_DNS_LIST_NODE
{
    struct _TAG_TCPIP_DNS_LIST_NODE*    next;       // next node in list
                                                    // makes it valid SGL_LIST_NODE node
    TCPIP_DNS_EVENT_HANDLER             handler;    // handler to be called for event
    const void*                         hParam;     // handler parameter
    TCPIP_NET_HANDLE                    hNet;       // interface that's registered for
                                                    // 0 if all    
}TCPIP_DNS_LIST_NODE;


typedef struct
{
    const TCPIP_DNS_HEADER* dnsHeader;  // header
    uint8_t*                dnsPacket;  // processed packet
    TCPIP_DNS_RX_DATA*      dnsRxData;  // current pointer into the packet
    uint16_t                dnsPacketSize;  // packet size
    TCPIP_DNS_HASH_ENTRY*   dnsHE;          // associated hash entry
    TCPIP_DNS_DBG_EVENT_TYPE evDbgType;     // associated parsing event, if any
}TCPIP_DNS_RR_PROCESS;




static size_t TCPIP_DNS_OAHASH_KeyHash(OA_HASH_DCPT* pOH, const void* key);
static OA_HASH_ENTRY* TCPIP_DNS_OAHASH_DeleteEntry(OA_HASH_DCPT* pOH);
static int TCPIP_DNS_OAHASH_KeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key);
static void TCPIP_DNS_OAHASH_KeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key);
#if defined(OA_DOUBLE_HASH_PROBING)
static size_t TCPIP_DNS_OAHASH_ProbeHash(OA_HASH_DCPT* pOH, const void* key);
#endif  // defined(OA_DOUBLE_HASH_PROBING)




#endif  /* _DNS_PRIVATE_H_ */

