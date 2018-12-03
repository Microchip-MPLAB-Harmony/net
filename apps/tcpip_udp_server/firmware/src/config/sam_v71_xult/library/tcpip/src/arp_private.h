/*******************************************************************************
  ARP module private header

  Company:
    Microchip Technology Inc.
    
  File Name:
    arp_private.h

  Summary:
    Header definitions file for ARP module
    
  Description:
    This file contains the private definitions for the ARP module
*******************************************************************************/
// DOM-IGNORE-BEGIN
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








// DOM-IGNORE-END

#ifndef _ARP_PRIVATE_H_ 
#define _ARP_PRIVATE_H_ 


//#define OA_HASH_DYNAMIC_KEY_MANIPULATION
// do not use multiple hashes for this implementation


// definitions
// 



#define HW_ETHERNET             (0x0001u)   // ARP Hardware type as defined by IEEE 802.3
#define ARP_IP                  (0x0800u)   // ARP IP packet type as defined by IEEE 802.3


// ARP packet structure
typedef struct __attribute__((aligned(2), packed))
{
    uint16_t    HardwareType;   // Link-layer protocol type (Ethernet is 1)
    uint16_t    Protocol;       // The upper-layer protocol issuing an ARP request (0x0800 for IPv4)
    uint8_t     MACAddrLen;     // MAC address length (6)
    uint8_t     ProtocolLen;    // Length of addresses used in the upper-layer protocol (4)
    uint16_t    Operation;      // The operation the sender is performing (ARP_OPERATION_REQ or ARP_OPERATION_RESP)
    TCPIP_MAC_ADDR    SenderMACAddr;  // The sender's hardware (MAC) address
    IPV4_ADDR     SenderIPAddr;   // The sender's IP address
    TCPIP_MAC_ADDR    TargetMACAddr;  // The target node's hardware (MAC) address
    IPV4_ADDR     TargetIPAddr;   // The target node's IP address
} ARP_PACKET;



// ARP cache entry
typedef struct _TAG_ARP_HASH_ENTRY 
{
    OA_HASH_ENTRY               hEntry;         // hash header;
    struct _TAG_ARP_HASH_ENTRY* next;           // ordered link list by tInsert
    IPV4_ADDR                   ipAddress;      // the hash key: the IP address
    uint32_t                    tInsert;        // arp time it was inserted 
    TCPIP_MAC_ADDR                    hwAdd;          // the hardware address
    uint16_t                    nRetries;       // number of retries for an incomplete entry
}ARP_HASH_ENTRY;

// ARP flags used in hEntry->flags
// note that only the hEntry->flags.user fields
// should be used! 
typedef enum
{
    ARP_FLAG_ENTRY_BUSY         = 0x0001,          // this is used by the hash itself!
    // user flags
    ARP_FLAG_ENTRY_PERM         = 0x0040,          // entry is permanent
    ARP_FLAG_ENTRY_COMPLETE     = 0x0080,          // regular entry, complete
                                                   // else it's incomplete
                                                   //
    ARP_FLAG_ENTRY_CONFIGURE    = 0x0100,          // configuration query, transmit always
    ARP_FLAG_ENTRY_GRATUITOUS   = 0x0200,          // gratuitous ARP query, use different retry number                                                   
    ARP_FLAG_ENTRY_VALID_MASK   = (ARP_FLAG_ENTRY_PERM | ARP_FLAG_ENTRY_COMPLETE )
                                                     
                                                  
}ARP_ENTRY_FLAGS;

    
        


#define     ARP_HASH_PROBE_STEP      1


// each ARP cache consists of
typedef struct
{
    OA_HASH_DCPT*       hashDcpt;       // contiguous space for a hash descriptor
                                        // and hash table entries
    PROTECTED_SINGLE_LIST         permList;       // list of active entries that never expire
    PROTECTED_SINGLE_LIST         completeList;   // list of completed, valid entries
    PROTECTED_SINGLE_LIST         incompleteList; // list of not completed yet entries
    size_t              purgeThres;     // threshold to start cache purging
    size_t              purgeQuanta;    // how many entries to purge
}ARP_CACHE_DCPT;

// ARP unaligned key
// the ip address field is unaligned
// in ARP packets
typedef struct __attribute__((packed))
{
    uint32_t    v;
}ARP_UNALIGNED_KEY;



// ARP event registration

typedef struct  _TAG_ARP_LIST_NODE
{
	struct _TAG_ARP_LIST_NODE*		next;		// next node in list
                                                // makes it valid SGL_LIST_NODE node
    TCPIP_ARP_EVENT_HANDLER               handler;    // handler to be called for event
    const void*                     hParam;     // handler parameter
    TCPIP_NET_HANDLE                hNet;       // interface that's registered for
                                                // 0 if all    
}ARP_LIST_NODE;


#endif  // _ARP_PRIVATE_H_ 

