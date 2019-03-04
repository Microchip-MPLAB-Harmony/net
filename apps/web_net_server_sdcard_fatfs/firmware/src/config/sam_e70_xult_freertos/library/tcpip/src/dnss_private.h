
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









#ifndef _DNSS_PRIVATE_H_
#define _DNSS_PRIVATE_H_

#define DNS_SERVER_ENABLE   1
#define DNS_SERVER_DISABLE  0

#define     TCPIP_DNSS_HOSTNAME_MAX_SIZE  255
#define     TCPIP_DNSS_HASH_PROBE_STEP      1    // step to advance for hash collision
// When the DNS server entry validity time is 0, then this following macro
// value will be send as TTL time and this value specifies that the etry is permanent 
// and the entry can be removed only when user deletes it.
#define     TCPIP_DNSS_PERMANENT_ENTRY_TTL_TIME     0xFFFFFFFF

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
	uint8_t*				    sHostNameData;  //Host name size
    IPV4_ADDR                   ip4Address;    // Provide a symbol for IPv4 entries number; 
    TCPIP_UINT32_VAL            tConfig;       //  Configured time for all the IPv4 and Ipv6 address per hostname
    IP_ADDRESS_TYPE             recordType;     //  DNS protocol record type is mapped to IP address type
    TCPIP_UINT32_VAL            entryTimeout; // Time out in seconds configured from command prompt.
    IPV6_ADDR                   ip6Address;    // Provide a symbol for IPv6 entries number
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

typedef union
{
	uint8_t Val;
	struct
	{
		unsigned char DNSServInUse 		: 1;	// Indicates the DNS Server module is in use
		unsigned char AddressType 	: 2;    // IP_ADDRESS_TYPE_IPV6 or IP_ADDRESS_TYPE_IPV4
        unsigned char filler        : 5;
	} bits;
} DNS_SERVER_FLAGS;

typedef enum
{
    DNSS_STATE_START             = 0,
    DNSS_STATE_WAIT_REQUEST,
    DNSS_STATE_PUT_REQUEST,
    DNSS_STATE_DONE,
}DNSS_STATE;

typedef struct
{
    OA_HASH_DCPT*       dnssHashDcpt;       // contiguous space for a hash descriptor  and hash table entries  
    UDP_SOCKET          dnsSrvSocket;               // Server Socket used by DNS Server
    DNSS_STATE		    smState;           // server state machine status
    uint8_t             dnsSrvInitCount;
    DNS_SERVER_FLAGS    flags;
    int                 intfIdx;    
	const void 		    *memH;
    int                 IPv4EntriesPerDNSName;
#ifdef TCPIP_STACK_USE_IPV6
    int                 IPv6EntriesPerDNSName;
#endif
    int             dnsSTickPending;             // DNS server processing tick
    tcpipSignalHandle dnsSSignalHandle;
    uint32_t        dnsSTimeMseconds;
    bool            replyWithBoardInfo;
}DNSS_DCPT;

/*
* DNS Server ENTRY flags used in hEntry->flags
* note that only the hEntry->flags.user fields
* should be used! 
*/

// DNS Server cache entry
typedef struct	_TAG_DNSS_HASH_ENTRY 
{
	OA_HASH_ENTRY				hEntry; 		// hash header;
	uint8_t*                    memblk;         // memory block for IPv4 and IPv6 and TTL block of memory. Hostname is not part of this block
	uint32_t                    tInsert;        // start time per hash entry

    int                         nIPv4Entries;     // how many entries in the ip4Address[] array; if IPv4 is defined
    IPV4_ADDR*                  pip4Address;  // pointer to an array of IPv4 entries 
#ifdef TCPIP_STACK_USE_IPV6
    int                         nIPv6Entries;     // how many entries in the ip6Address[] array; if IPv6 is defined
    IPV6_ADDR*                  pip6Address;  // pointer to an array of IPv6 entries
#endif    
    uint8_t                     *pHostName;
	uint8_t						recordType;  // record can be  IPv6 ( 28 )or IPv4 ( 1)	
	uint8_t                     netIfIdx;
    TCPIP_UINT32_VAL            validityTime;      // user configured time per hash entry in second
}DNSS_HASH_ENTRY;



size_t TCPIP_OAHASH_DNSS_KeyHash(OA_HASH_DCPT* pOH, const void* key);
OA_HASH_ENTRY* TCPIP_OAHASH_DNSS_EntryDelete(OA_HASH_DCPT* pOH);
int TCPIP_OAHASH_DNSS_KeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key);
void TCPIP_OAHASH_DNSS_KeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key);
size_t TCPIP_OAHASH_DNSS_ProbeHash(OA_HASH_DCPT* pOH, const void* key);




#endif     /*_DNSS_PRIVATE_H_*/

