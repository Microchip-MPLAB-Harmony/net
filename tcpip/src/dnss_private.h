
/*******************************************************************************
DNS Server module private header

  Company:
    Microchip Technology Inc.
    
  File Name:
    dnss_private.h

  Summary:
    Header definitions file for DNS Server module
    
  Description:
    This file contains the private definitions for the DNS module
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









#ifndef H_DNSS_PRIVATE_H_
#define H_DNSS_PRIVATE_H_


#define     TCPIP_DNSS_HASH_PROBE_STEP      1U    // step to advance for hash collision
// When the DNS server entry validity time is 0, then this following macro
// value will be send as TTL time and this value specifies that the etry is permanent 
// and the entry can be removed only when user deletes it.
#define     TCPIP_DNSS_PERMANENT_ENTRY_TTL_TIME     0xFFFFFFFFU

// *****************************************************************************
/* 
  Structure:
    TCPIP_DNSS_CACHE_ENTRY

  Summary:
    This structure is used to set and get the cache entries for DNS client use.

  Description:
    DNS server cache details use this structure to get from cache entry and set to the entry.

  Remarks:
    None.
*/
typedef struct
{
    const uint8_t*              sHostNameData;  // Host name size
    IPV4_ADDR                   ip4Address;     // Provide a symbol for IPv4 entries number; 
    TCPIP_UINT32_VAL            tConfig;        // Configured time for all the IPv4 and Ipv6 address per hostname
    IP_ADDRESS_TYPE             addressType;     // DNS protocol record type is mapped to IP address type
    TCPIP_UINT32_VAL            entryTimeout;   // Time out in seconds configured from command prompt.
    IPV6_ADDR                   ip6Address;     // Provide a symbol for IPv6 entries number
}TCPIP_DNSS_CACHE_ENTRY;



// DNS ENTRY flags used in hEntry->flags
// note that only the hEntry->flags.user fields
// should be used!
typedef enum
{
    DNSS_FLAG_ENTRY_BUSY         = 0x0001,          // this is used by the hash itself!
    // user flags
    DNSS_FLAG_ENTRY_INCOMPLETE   = 0x0040,          // entry is not completed yet
    DNSS_FLAG_ENTRY_COMPLETE     = 0x0080,          // regular entry, complete
                                                   // else it's incomplete
                                                   //
    DNSS_FLAG_ENTRY_VALID_MASK   = (DNSS_FLAG_ENTRY_INCOMPLETE | DNSS_FLAG_ENTRY_COMPLETE )

}DNSS_HASH_ENTRY_FLAGS;

typedef enum
{
    DNSS_STATE_START             = 0,
    DNSS_STATE_WAIT_REQUEST,
    DNSS_STATE_PUT_REQUEST,
    DNSS_STATE_DONE,
}DNSS_STATE;

typedef struct
{
    OA_HASH_DCPT*       dnssHashDcpt;           // contiguous space for a hash descriptor  and hash table entries  
    UDP_SOCKET          dnsSrvSocket;           // Server Socket used by DNS Server
    uint16_t            intfIdx;    
    const void          *memH;
    size_t              IPv4EntriesPerDNSName;
#ifdef TCPIP_STACK_USE_IPV6
    size_t              IPv6EntriesPerDNSName;
#endif
    TCPIP_SIGNAL_HANDLE dnsSSignalHandle;
    uint32_t            dnsSTimeMseconds;
    uint8_t             dnsServInUse;
    uint8_t             replyWithBoardInfo;
}DNSS_DCPT;

/*
* DNS Server ENTRY flags used in hEntry->flags
* note that only the hEntry->flags.user fields
* should be used! 
*/

// DNS Server cache entry
typedef struct  S_TAG_DNSS_HASH_ENTRY 
{
    OA_HASH_ENTRY               hEntry;         // hash header;
    uint8_t*                    memblk;         // memory block for IPv4 and IPv6 and TTL block of memory. Hostname is not part of this block
    uint32_t                    tInsert;        // start time per hash entry

    size_t                      nIPv4Entries;   // how many entries in the ip4Address[] array; if IPv4 is defined
    IPV4_ADDR*                  pip4Address;    // pointer to an array of IPv4 entries 
#ifdef TCPIP_STACK_USE_IPV6
    size_t                      nIPv6Entries;   // how many entries in the ip6Address[] array; if IPv6 is defined
    IPV6_ADDR*                  pip6Address;    // pointer to an array of IPv6 entries
#endif    
    uint8_t                     *pHostName;
    uint8_t                     addType;        // IP_ADDRESS_TYPE value
    uint8_t                     netIfIdx;
    TCPIP_UINT32_VAL            validityTime;    // user configured time per hash entry in second
}DNSS_HASH_ENTRY;



size_t TCPIP_OAHASH_DNSS_KeyHash(OA_HASH_DCPT* pOH, const void* key);
OA_HASH_ENTRY* TCPIP_OAHASH_DNSS_EntryDelete(OA_HASH_DCPT* pOH);
int TCPIP_OAHASH_DNSS_KeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key);
void TCPIP_OAHASH_DNSS_KeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key);




#endif     /*H_DNSS_PRIVATE_H_*/

