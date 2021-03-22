/*******************************************************************************
  IPv4 private API for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    ipv4_private.h

  Summary:
    
  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2012-2020 Microchip Technology Inc. and its subsidiaries.

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

#ifndef _IPV4_PRIVATE_H_
#define _IPV4_PRIVATE_H_

// internal

// debugging
#define TCPIP_IPV4_DEBUG_MASK_BASIC             (0x0001)
#define TCPIP_IPV4_DEBUG_MASK_FRAGMENT          (0x0002)
#define TCPIP_IPV4_DEBUG_MASK_RX_CHECK          (0x0004)
#define TCPIP_IPV4_DEBUG_MASK_ARP_QUEUE         (0x0008)
#define TCPIP_IPV4_DEBUG_MASK_PROC_EXT          (0x0010)
#define TCPIP_IPV4_DEBUG_MASK_FWD               (0x0020)
#define TCPIP_IPV4_DEBUG_MASK_FWD_MAC_DEST      (0x0040)
#define TCPIP_IPV4_DEBUG_MASK_FILT_COUNT        (0x0080)

// enable IPV4 debugging levels
#define TCPIP_IPV4_DEBUG_LEVEL  (0)


// IPv4 packet filter registration

typedef struct  _TAG_IPV4_FILTER_LIST_NODE
{
	struct _TAG_IPV4_FILTER_LIST_NODE*  next;       // next node in list
                                                    // makes it valid SGL_LIST_NODE node
    IPV4_FILTER_FUNC                    handler;    // handler to be called for the filter
    uint8_t                             active;     // the filter is active
    uint8_t                             reserved[3];// not used
}IPV4_FILTER_LIST_NODE;



// IPv4 fragment reassembly

typedef struct _TAG_IPV4_FRAGMENT_NODE
{
    struct _TAG_IPV4_FRAGMENT_NODE* next;       // next fragment node: ipv4FragmentQueue 
    TCPIP_MAC_PACKET*               fragHead;   // head fragments list; connected with pkt_next;
    uint32_t                        fragTStart; // fragment occurring tick 
    uint16_t                        nFrags;     // number of fragments in this node
    uint16_t                        fragTmo;    // fragment expiration timeout, seconds
    
}IPV4_FRAGMENT_NODE;


// IPv4 fragment transmission
typedef struct
{
    TCPIP_MAC_PACKET        macPkt;         // standard MAC packet containing the MAC + IPv4 headers
                                            // always on 1st position to safe cast to SGL_LIST_NODE!
    TCPIP_MAC_DATA_SEGMENT  fragSeg;        // segment carying the IPv4 payload 
                                            // allows zc functionality
}IPV4_FRAG_TX_PKT;                                           


typedef enum
{
    IPV4_FRAG_TX_ACK_HEAD   = 0x01,         // acknowledge the head of a frgmented TX packet
    IPV4_FRAG_TX_ACK_FRAGS  = 0x02,         // acknowledge the fragments within a frgmented TX packet

}IPV4_FRAG_TX_ACK;


// IPv4 fixed options

// mask of internal supported options
// multiple can be set
// 16 bits only
typedef enum
{
    IPV4_OPTION_MASK_NONE           = 0x0000,
    IPV4_OPTION_MASK_ROUTER_ALERT   = 0x0001,

    // other options will be added here
}IPV4_OPTION_TYPE_MASK;


#define IPV4_ROUTER_ALERT_OPTION_SIZE       4   // fixed option requiring 4 bytes
#define IPV4_ROUTER_ALERT_OPTION_COPIED     1 
#define IPV4_ROUTER_ALERT_OPTION_CLASS      0
#define IPV4_ROUTER_ALERT_OPTION_DATA       0
typedef struct
{
    struct
    {   // option type
        uint8_t     optNumber:  5;      // TCPIP_IPV4_OPTION_ROUTER_ALERT
        uint8_t     optClass:   2;      // option class: 0 - control; 2 - debugging and measurement; 1, 3 - reserved
        uint8_t     optCopied:  1;      // option copied to all fragments
    };
    uint8_t         optLength;          // size of the entire option
    uint16_t        optValue;           // option value
}IPV4_ROUTER_ALERT_OPTION;


// maximum size of options in an IPv4 packet
#define IPV4_OPTIONS_MAXIMUM_SIZE           40  // 40 bytes + 20 bytes header == 60 bytes maximum packet

// maximum size ofIPv4 header
#define IPV4_HEADER_MAXIMUM_SIZE            60  // 20 bytes header + 40 bytes max options

// ARP support
typedef enum
{
    IPV4_ARP_PKT_TYPE_NONE, // invalid
    IPV4_ARP_PKT_TYPE_TX,   // IPv4 TX packet: IPV4_PACKET*
    IPV4_ARP_PKT_TYPE_MAC,  // IPv4 MAC packet: TCPIP_MAC_PACKET*
                            // usually for ICMP replies
    IPV4_ARP_PKT_TYPE_FWD,  // IPv4 forwarding packet: TCPIP_MAC_PACKET*
}IPV4_ARP_PKT_TYPE;


// entry queued for an ARP operation
typedef struct _tag_IPV4_ARP_ENTRY
{
    struct _tag_IPV4_ARP_ENTRY* next;   // SGL_LIST_NODE safe cast
    uint8_t                 type;       // IPV4_ARP_PKT_TYPE: packet type
    uint8_t                 arpIfIx;    // index of the interface for which ARP is queued
    uint16_t                reserved;   // not used    
    union
    {
        IPV4_PACKET*        pTxPkt;     // IPv4 packet to be transmitted
        TCPIP_MAC_PACKET*   pMacPkt;    // MAC packet to be transmitted 
        TCPIP_MAC_PACKET*   pFwdPkt;    // packet to be forwarded 
        void*               pPkt;       // generic   
    };
    IPV4_ADDR               arpTarget;  // ARP resolution target
}IPV4_ARP_ENTRY;


// routing

// type of processed packet: processed on this host or routed
// NOTE: 16 bits only!
typedef enum
{
    IPV4_PKT_TYPE_UNK       = 0x00,     // packet of unknown type
    IPV4_PKT_TYPE_UNICAST   = 0x01,     // unicast packet for this host
    IPV4_PKT_TYPE_BCAST     = 0x02,     // broadcast packet
    IPV4_PKT_TYPE_MCAST     = 0x04,     // multicast packet
    // ...
    // flags for packet destination
        // packets are processed internally and/or externally
    IPV4_PKT_DEST_FWD       = 0x0100,   // packet destination is external, to be forwarded
    IPV4_PKT_DEST_HOST      = 0x0200,   // packet destination is the internal host 

}IPV4_PKT_PROC_TYPE;

// internal forwarding entry
// NOTE: same structure as TCPIP_IPV4_FORWARD_ENTRY_BIN! 
typedef struct
{
    uint32_t    netAdd;     // network destination address
    uint32_t    mask;       // associated mask for this route entry
    uint32_t    gwAdd;      // gw destination address
    uint8_t     inIfIx;     // input interface
    uint8_t     outIfIx;    // interface to go out on
    uint8_t     metric;     // path efficiency
    uint8_t     nOnes;      // number of leading ones in the mask

}IPV4_ROUTE_TABLE_ENTRY;

// run time forwarding flags
typedef enum
{
    IPV4_FWD_FLAG_NONE          = 0x00,
    IPV4_FWD_FLAG_FWD_ENABLE    = 0x01,   // forwarding is enabled
    IPV4_FWD_FLAG_BCAST_ENABLE  = 0x02,   // forwarding of broadcasts is enabled
    IPV4_FWD_FLAG_MCAST_ENABLE  = 0x04,   // forwarding of multicasts is enabled


}IPV4_FORWARD_RUN_FLAGS;

// IP forwarding descriptor per interface
typedef struct
{
    IPV4_ROUTE_TABLE_ENTRY* fwdTable;       // forwarding table itself
    size_t                  usedEntries;    // number of entries that are used
    size_t                  totEntries;     // total number of entries
    uint16_t                iniFlags;       // TCPIP_IPV4_FORWARD_FLAGS: initialization flags
    uint16_t                runFlags;       // IPV4_FORWARD_RUN_FLAGS: initialization flags
}IPV4_FORWARD_DESCRIPTOR;

// forwarded packets that need to also be processed locally
// these are bcast/mcast packets
typedef struct _tag_IPV4_FORWARD_NODE
{
    struct _tag_IPV4_FORWARD_NODE*  next;           // DBL_LIST_NODE safe cast
    struct _tag_IPV4_FORWARD_NODE*  prev;           // DBL_LIST_NODE safe cast
    TCPIP_MAC_PACKET*               pFwdPkt;        // packet that's being forwarded
    TCPIP_MAC_PACKET_ACK_FUNC       ownerAckFunc;   // the acknowledge function of the packet owner
                                                    // that's the original ackFnc in the packet before being forwarded
    const void*                     ownerAckParam;  // the original ackParam in the packet
    TCPIP_MAC_ADDR                  sourceMacAdd;   // original source MAC address the packet came from
    TCPIP_MAC_ADDR                  destMacAdd;     // original destination MAC address
}IPV4_FORWARD_NODE;

#endif // _IPV4_PRIVATE_H_



