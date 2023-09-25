/**********************************************************************
IPv4 Header File

  Company:
    Microchip Technology Inc.
    
  File Name:
    ipv4.h
    
  Summary:
    IPv4 definitions for the Microchip TCP/IP Stack.
    
  Description:
    IP is the workhorse protocol of the TCP/IP protocol suite. All TCP,
    UDP, and ICMP data gets transmitted as IP datagrams. IP provides an
    unreliable, connectionless datagram delivery service.              
  **********************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2012-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef __IPV4_H_
#define __IPV4_H_

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  


// *****************************************************************************
// *****************************************************************************
// Section: Public Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* IPv4 Fowarding Support
 *
 *  The IPv4 module contains support for packet forwarding/routing using a forwarding table
 *
 * Note that this is NOT an implementation of a full router machine!
 * There are features requested by the RFC 1812 that are not supported.
 * Some of the missing features will be eventually added.
 * This is work in progress.
 *
 *  Some of the known issues and implementation limitations:
 *  - the forwarding table is strictly static. There are no routing protocols running
 *    to allow the dynamic update of the table
 *  - the forwarding table is populated when the IPv4 module is initialized:
 *    the table is part of the configuration data
 *  - When forwarding a packet from one interface to another, packet fragmentation is not yet supported
 *  - Forwading  of multicast packets is not currently supported
 *  - A packet can be both processed on the host and forwarded
 *    (which should be possible for mcast or directed broadcast, for example)
 *    Supported for broadcast only.
 *  - Virtual IPv4 interfaces are not yet supported
 *  - Currently the search of the forwarding table is purely sequential!
 *    A better algorithm will be eventually provided.
 *  - There is no provision to update the table at run time when the host obtains
 *    an IP address from a DHCP server, for example.
 *    As a router, probably the address should be static and not received
 *    from another DHCP server
 *  - Source Route Option is not examined/processed in the incoming packets
 *  - Source Route Option is not supported in the forwarded packets
 *  - ICMP messages are not generated
 *  - TOS field is not considered in the packet forwarding
 *  - Source Address Validation is not (yet) supported 
 */

// *****************************************************************************
/* IPv4 fowarding entry

  Summary:
    Defines an entry in the IPv4 forwarding table 
    using ASCII strings for the network addresses

  Description:
    Describes an IPV4 forwarding table entry.
    The forwading table contains multiple forwarding entries.
    Usually the router machine refers to the information in the
    forwarding table as the Forwarding Information Base (FIB)
    The IPv4 module constructs a forwarding table for each interface.
    The initial contents of the forwarding table is specified
    using the IPv4 configuration data.


    The forwarding table is used as follows:

    Internally the forwarding table is ordered based on the number of bits set to 1 in the network mask:
    the higher the number of bits set in the mask, the higher the priority of the forwarding entry.
    If two entries have the same number of set mask bits, then the entry with a lower metric
    has the higher priority.
    For example, the entry with the mask '255.255.255.255' has higher priority
    than the entry with the mask '255.255.255.0'.
    The entry with mask '0.0.0.0' has the lowest priority and is the default entry that will match everything.

    For an incoming IPv4 packet on a specific interface, with a destination address destAddr
    - if the destAddr is the address of a local interface on the current host
      the packet will be processed internally and not forwarded
    - else the forwarding table entries for the input interface are traversed from the highest priority to the lowest
      and a match is searched for, such that:
            destAddr & entry->netMask == entry->netAddress
       When the match occurred, the packet will be forwarded on the output interface
       specified by the entry->outIfName, following the rules:
            - if entry->gwAddress != 0, the packet will be sent to that gateway address
            - else if the destination is on that LAN
                the packet will be forwarded to that destination 
            - Otherwise the packet is not routable and it will be discarded.
    
    Example - we have a dual Ethernat interface host:
        if0 == 192.168.1.136/24 on LAN0
        if1 == 192.168.2.29/24 on LAN1
    The networks are on different LANs and we want to be able to forward some IPv4 traffic
    from one LAN segment to the other.

    Forwarding table example:
    forwardTable[] = 
    {
        Network         Mask             Gateway        inputInterface  outInterface
        // interface 0 entries    
        {192.168.2.240, 255.255.255.255, 0,             eth0,           eth1},       // traffic received on LAN0 for host 192.168.2.240 goes directly to it on LAN1
        {192.168.2.0,   255.255.255.0,   0,             eth0,           eth1},       // traffic received on LAN0 for hosts on LAN1 goes to LAN1 over interface eth1 directly to that host
                                                                                     // Note that this entry makes the previous one unncecessary
        {192.168.3.0,   255.255.255.0,   192.168.2.1,   eth0,           eth1},       // traffic received for hosts on network 192.168.3.0/24 goes to LAN1 over interface eth1 using router 192.168.2.1
        // default entry; will match everything
        {0.0.0.0,       0.0.0.0,         192.168.1.100, eth0,           eth0},       // default entry; will match everything:
                                                                                     // all other traffic on LAN0  will be sent on eth0 to router 192.168.1.100 
        // interface 1 entries    
        {192.168.1.147, 255.255.255.255, 0,             eth1,           eth0},       // traffic received on LAN1 for host 192.168.1.147 goes directly to it on LAN0
        {192.168.1.0,   255.255.255.0,   192.168.1.100, eth1,           eth0},       // traffic received on LAN1 for hosts on LAN0 goes to LAN0 over interface eth0 using router 192.168.1.100
    }


  Remarks:
    A default route is usually provided as part of the IPv4 initialization.
    Note that if no match is found in the forwarding table
    the packet will be discarded and not forwarded

    RFC 1812 specifies that the packets that have as destination one of the LANs 
    directly connected to the router should be delivered without consulting the forwarding table.
    This implementation uses the forwarding table even in this case, as it is believed that this adds
    more flexibility, allowing the user to configure more specific rules for the forwarding.

    The ASCII format is supported when build symbol TCPIP_IPV4_FORWARDING_TABLE_ASCII != 0

 */

typedef struct
{
    /* Network destination address.
     * This address is used for matching the IPv4 packet destination address 
     * For ex: "192.168.2.0" */ 
    const char*     netAddress;
    /* Associated mask for this destination address. For ex: "255.255.255.0"
     * The value needs to be a valid network mask, with contiguous ones in its binary representation
     * followed by contiguous zeros */ 
    const char*     netMask;
    /* Gateway destination address. For ex: "192.168.2.1"
     * Used as packets destination */ 
    const char*     gwAddress;
    /* The name of the input interface that this entry applies to.
     * The forwarding tables are built per interface
     * For ex: "eth0", wlan0", etc. */
    const char*     inIfName;
    /* The name of the interface to go out on if this entry is selected.
     * For ex: "eth0", wlan0", etc. */
    const char*     outIfName;
    /* The path efficiency. The smaller the value, the higher priority for the path.
     * Note that this value is useful only when there are multiple entries in the 
     * forwarding table that have the same number of mask bits set */
    uint8_t         metric;
}TCPIP_IPV4_FORWARD_ENTRY_ASCII;


// *****************************************************************************
/* IPv4 fowarding entry

  Summary:
    Defines an entry in the IPv4 forwarding table 
    using binary values for the network addresses

  Description:
    Describes an IPV4 forwarding table entry.
    See the TCPIP_IPV4_FORWARD_ENTRY_ASCII description

  Remarks:
    The binary format is always supported 
*/

typedef struct
{
    /* Network destination address, network order */
    uint32_t        netAddress;
    /* Network mask, network order */
    uint32_t        netMask;
    /* Gateway destination address, network order */
    uint32_t        gwAddress;
    /* The number of the input interface this entry applies to.
     * The forwarding tables are built per interface */
    uint8_t         inIfIx;
    /* The number of the interface to go out on if this entry is selected. */
    uint8_t         outIfIx;
    /* The path efficiency */
    uint8_t         metric;
    /* number of leading ones in the netMask; negative means entry is invalid/unused.
     * used only for TCPIP_IPV4_ForwadTableEntryGet function */
    int8_t          nOnes;
}TCPIP_IPV4_FORWARD_ENTRY_BIN;

// *****************************************************************************
/* IPv4 fowarding entry

  Summary:
    Reunion of the 2 types: ASCII or binary

  Description:
    Union consisting of the 2 different table entry types

  Remarks:
    None.
*/
typedef union
{
    TCPIP_IPV4_FORWARD_ENTRY_ASCII   entryAscii;
    TCPIP_IPV4_FORWARD_ENTRY_BIN     entryBin;
}TCPIP_IPV4_FORWARD_ENTRY;



// *****************************************************************************
/* IPv4 forwarding flags

  Summary:
    Flags for the configuration of the IPv4 forwarding

  Description:
    These are flags used to configure the run time operation of the
    IPv4 forwarding module

  Remarks:
    None.
 */
typedef enum
{
    TCPIP_IPV4_FWD_FLAG_NONE             = 0x0000,  // no flag set, the default
    TCPIP_IPV4_FWD_FLAG_ENABLED          = 0x0001,  // when the stack/IPv4 module starts, enable the IPv4 forwarding
                                                    // This should be set by default
                                                    // Otherwise the IPv4 forwarding can be eventually enabled at run time using an API
                                                    // Note: this API is not currently supported.
    TCPIP_IPV4_FWD_FLAG_BCAST_ENABLED    = 0x0002,  // enable the forwarding of the broadcast packets
                                                    // the default should be disabled
    TCPIP_IPV4_FWD_FLAG_MCAST_ENABLED    = 0x0004,  // enable the forwarding of the multicast packets
                                                    // the default should be disabled

    TCPIP_IPV4_FWD_FLAG_BINARY_TABLE     = 0x0000,  // The initialization forwarding table is in binary format, not strings - default setting
                                                        // The binary format is always supported 
    TCPIP_IPV4_FWD_FLAG_ASCII_TABLE      = 0x0100,  // The initialization forwarding table is in ASCII format, using strings
                                                        // Build symbol TCPIP_IPV4_FORWARDING_TABLE_ASCII != 0

}TCPIP_IPV4_FORWARD_FLAGS;

// *****************************************************************************
/* IPv4 fowarding statistics entry

  Summary:
    Structure describing the forward statistics maintained by the IPv4 module

  Description:
    Data structure updated by the IPv4 forwarding process when debug is enabled

  Remarks:
    None
*/
typedef struct
{
    unsigned int failNoRoute;       // failures due to no route found
    unsigned int failNetDown;       // failures due to net down
    unsigned int failMacDest;       // failures due to MAC destination error
    unsigned int failMtu;           // failures due to MTU
    unsigned int failArpQueue;      // failures due to ARP pool empty
    unsigned int failFwdQueue;      // failures due to forward TX Queue full 
    unsigned int failMac;           // failures due to MAC rejecting the packet
    unsigned int arpQueued;         // ARP queued MAC addresses
    unsigned int ucastPackets;      // unicast packets to the router address
    unsigned int bcastPackets;      // broadcast packets 
    unsigned int mcastPackets;      // multicast packets 
    unsigned int fwdPackets;        // total should be forwarded packets
    unsigned int fwdQueuedPackets;  // queued packets (forwarded and then processed internally)
    unsigned int macPackets;        // packets actually forwarded to MAC
}TCPIP_IPV4_FORWARD_STAT;

// *****************************************************************************
/* IPv4 ARP statistics data

  Summary:
    Structure describing the ARP statistics maintained by the IPv4 module

  Description:
    Data structure updated by the IPv4 ARP process when ARP debugging is enabled

  Remarks:
    None
*/
typedef struct
{
    size_t nPool;       // entries in the pool
    size_t nPend;       // entries pending to be resolved
    size_t txSubmit;    // submitted for TX
    size_t fwdSubmit;   // submitted for FWD
    size_t txSolved;    // solved for TX
    size_t fwdSolved;   // solved for FWD
    size_t totSolved;   // total solved
    size_t totFailed;   // total failed 
}TCPIP_IPV4_ARP_QUEUE_STAT;

// *****************************************************************************
/* IPv4 configuration

  Summary:
    Initialization parameters supported by IPv4

  Description:
    The parameters needed for the Pv4 module initialization

  Remarks:
    None.
 */
typedef struct
{
    /* The number of entries that IPv4 can queue up for ARP resolution.
       Usually it should be <= the number of total ARP cache entries for all interfaces */
    size_t          arpEntries;


    /* the following members are valid/used only if the IP forwarding is enabled
     * They are valid only when the stack is initialized with multiple network interfaces */
    
    /* Forwarding flags. See the definition of TCPIP_IPV4_FORWARD_FLAGS */
    uint16_t        forwardFlags;

    /* The maximum number of broadcast and multicast packets that can be queued
     * waiting for the output interface to transmit them.
     * This applies only for packets that have to be both forwarded and processed internally
     * which is only broadcast or multicast
     * Adjust depending on your traffic
     * Note that if this limit is exceeded, the packets won't be forwarded
     * but still processed internally
     * If 0, packets won't be forwarded, just processed internally. */
    uint16_t        forwardTxQueueSize;

    /* The maximum number of entries in the forwarding table for each interface */
    uint16_t        forwardTableMaxEntries;

    /* The number of entries in the initialization forwarding table
     * The number of entries per interface cannot exceed the 'forwardTableMaxEntries' value */
    uint16_t        forwardTableSize;

    /* the forwarding table entries to start with
     * It contains the entries for all interfaces involved in forwarding
     * The type of the table is given by the flag TCPIP_IPV4_FWD_FLAG_BINARY_TABLE/TCPIP_IPV4_FWD_FLAG_ASCII_TABLE */
    const TCPIP_IPV4_FORWARD_ENTRY* forwardTable;   

}TCPIP_IPV4_MODULE_CONFIG;

// *****************************************************************************
/* IPv4 operation result

  Summary:
    List of operation result codes

  Description:
    This is the list of results occurring in the IPv4 module initialization 
    and API

  Remarks:
    Negative codes represent errors.
*/
typedef enum
{
    TCPIP_IPV4_RES_OK       = 0,        // everything OK

    // errors
    TCPIP_IPV4_RES_INIT_VAL_ERR     = -1,       // initialization value error
    TCPIP_IPV4_RES_SIGNAL_ERR       = -2,       // failed to create a signal handler
    TCPIP_IPV4_RES_ARP_ERR          = -3,       // failed to initialize ARP queue
    TCPIP_IPV4_RES_NOTIFY_ERR       = -4,       // failed to initialize notifications
    TCPIP_IPV4_RES_MEM_ERR          = -5,       // failed to allocate memory
    TCPIP_IPV4_RES_ENTRIES_ERR      = -6,       // invalid forward table entries
    TCPIP_IPV4_RES_FORMAT_ERR       = -7,       // not supported forward table format 
    TCPIP_IPV4_RES_ADDRESS_ERR      = -8,       // invalid IP address 
    TCPIP_IPV4_RES_MASK_ERR         = -9,       // invalid IP mask 
    TCPIP_IPV4_RES_IF_ERR           = -10,      // invalid interface
    TCPIP_IPV4_RES_FWD_ENTRY_ERR    = -11,      // invalid forwarding table entry
    TCPIP_IPV4_RES_FWD_TABLE_ERR    = -12,      // invalid forwarding table - forwarding not enabled/existing
    TCPIP_IPV4_RES_FWD_LOCK_ERR     = -13,      // lock of the forwarding table could not be created/obtained
    TCPIP_IPV4_RES_FWD_NO_ENTRY_ERR = -14,      // no such entry exists


}TCPIP_IPV4_RES;

// *****************************************************************************
/* IPv4 supported protocols

  Summary:
    List of supported protocols.

  Description:
    This is the list of the protocols that are supported by this IPv4 implementation.

  Remarks:
    None.
 */
typedef enum
{
    IP_PROT_ICMP = (1u),
    IP_PROT_IGMP = (2u),
    IP_PROT_TCP = (6u),
    IP_PROT_UDP = (17u),
} IPV4_HEADER_TYPE;


// *****************************************************************************
/* IPv4 packet fragment definition

  Summary:
    Structure of an IPv4 fragment info.

  Description:
    This is the structure of an IPv4 fragment info in the IPv4 header.

  Remarks:
    None.
 */

typedef union
{
    uint16_t        val;
    struct
    {
        uint16_t    fragOffset:     13;     // fragment offset
        uint16_t    MF:             1;      // more fragments
        uint16_t    DF:             1;      // don't fragment
        uint16_t    reserved:       1;
    };
}IPV4_FRAGMENT_INFO;

// *****************************************************************************
/* IPv4 packet option definition

  Summary:
    Structure of an IPv4 option.

  Description:
    This is the structure of an IPv4 option field in the IPv4 header.

  Remarks:
    None.
 */
typedef struct
{
    struct
    {   // option type
        uint8_t     optNumber:  5;      // specifies an option
        uint8_t     optClass:   2;      // option class: 0 - control; 2 - debugging and measurement; 1, 3 - reserved
        uint8_t     optCopied:  1;      // option copied to all fragments
    };
    uint8_t         optLength;          // size of the entire option
    uint16_t        optData[];          // option specific variable data
}IPV4_OPTION_FIELD;

// *****************************************************************************
/* IPv4 type of service definition

  Summary:
    Structure of an IPv4 type of service field.

  Description:
    This is the structure of an IPv4 type of service header field.

  Remarks:
    None.
 */
typedef union
{
    uint8_t     val;
    struct
    {
        uint8_t     reserved:       2;      // not used
        uint8_t     reliability:    1;      // reliability settings: normal/high
        uint8_t     throughput:     1;      // throughput settings: normal/high
        uint8_t     delay:          1;      // delay settings: normal/low
        uint8_t     precedence:     3;      // precedence; one of TCPIP_IPV4_PRECEDENCE_ values
    };
}IPV4_TYPE_OF_SERVICE;

// *****************************************************************************
/* IPv4 packet header definition

  Summary:
    Structure of an IPv4 header.

  Description:
    This is the structure of an IPv4 packet header.

  Remarks:
    None.
 */
typedef struct
{
    struct
    {
        uint8_t IHL:        4;
        uint8_t Version:    4;
    };
    IPV4_TYPE_OF_SERVICE TypeOfService;
    uint16_t TotalLength;
    uint16_t Identification;
    IPV4_FRAGMENT_INFO FragmentInfo;
    uint8_t TimeToLive;
    uint8_t Protocol;
    uint16_t HeaderChecksum;
    IPV4_ADDR SourceAddress;
    IPV4_ADDR DestAddress;
    uint32_t  options[];
} IPV4_HEADER;


// *****************************************************************************
/* IPv4 packet structure definition

  Summary:
    IPv4 packet structure.

  Description:
    This is the structure of an IPv4 packet for transmission over the network.

  Remarks:
    None.
 */
typedef struct
{
    TCPIP_MAC_PACKET    macPkt;         // standard MAC packet header
                                        // safe cast to TCPIP_MAC_PACKET

    // additional IPv4 packet data 
    IPV4_ADDR           srcAddress;     // packet source
    IPV4_ADDR           destAddress;    // packet destination
    TCPIP_NET_HANDLE    netIfH;         // packet interface
    uint8_t             optionLen;      // length of the options in the IPV4 packet; usually 0
    uint8_t             optionOffset;   // offset of the current option; when multiple options are supported
    uint16_t            optionMask;     // internal options to be embedded in the packet
} IPV4_PACKET;

// *****************************************************************************
/* IPv4 packet filters

  Summary:
    List of supported IPv4 packet filters.

  Description:
    This is the list of the packet filters that are supported by this IPv4 implementation.
    There are 3 types of IPv4 packets currently supported:
        - unicast
        - broadcast
        - multicast
    An IPV4 packet is accepted if the filter corresponding to the packet type is not set 

  Remarks:
    Multiple filters can be set
    
    If no filter is set, all packets are accepted this is the default case.
 */
typedef enum
{
    TCPIP_IPV4_FILTER_NONE      = 0x00,     // no packet filter active. All packets are accepted
    TCPIP_IPV4_FILTER_UNICAST   = 0x01,     // unicast packets will be filtered out
    TCPIP_IPV4_FILTER_BROADCAST = 0x02,     // broadcast packets will be filtered out
    TCPIP_IPV4_FILTER_MULTICAST = 0x04,     // multicast packets will be filtered out
} TCPIP_IPV4_FILTER_TYPE;


// *****************************************************************************
/* IPv4 options

  Summary:
    List of supported IPv4 options

  Description:
    This is the list of the options that are supported by this IPv4 implementation.

  Remarks:
    Currently only limited options are supported.
    
    Other options will be eventually added, including variable size ones.

 */

typedef enum
{
    TCPIP_IPV4_OPTION_NONE          = 0,     // no option; standard packet
    TCPIP_IPV4_OPTION_ROUTER_ALERT  = 20,    // router alert option for IGMP traffic
} TCPIP_IPV4_OPTION_TYPE;

// *****************************************************************************
/* Descriptor for the IPv4 options

  Summary:
    Structure describing the options for an IPv4 packet

  Description:
    This is a data structure describing options that are part of an IPv4 packet

  Remarks:
    None;
 */

typedef struct
{
    TCPIP_IPV4_OPTION_TYPE  optionType;     // current option type
    int                     optionSize;     // current option type in bytes, including the IPV4_OPTION_FIELD data;
}TCPIP_IPV4_OPTION_DCPT;

// *****************************************************************************
/* IPv4 Type Of Service Flags

  Summary:
    List describing the TOS flags/values for an IPv4 packet

  Description:
    This is a enumeration pf the possible values for the Type Of Service field for an IPv4 packet

  Remarks:
    The type is used to set the IPV4_TYPE_OF_SERVICE in an IPv4 packet.

    Multiple flags can be set to specify the precedence, delay, throughput and reliability.

    Default value is 0x00, i.e. normal precedence, delay, throughput and reliability.

    Currently only 6 bit values are supported!
 */

typedef enum __attribute__ ((__packed__))
{
    /* Default values for all fields */
    TCPIP_IPV4_TOS_DEFAULT                = 0x00,

    // precedence types
    TCPIP_IPV4_PRECEDENCE_ROUTINE         = 0x00,
    TCPIP_IPV4_PRECEDENCE_PRIORITY        = 0x01,
    TCPIP_IPV4_PRECEDENCE_IMMEDIATE       = 0x02,
    TCPIP_IPV4_PRECEDENCE_FLASH           = 0x03,
    TCPIP_IPV4_PRECEDENCE_FLASH_OVERRIDE  = 0x04,
    TCPIP_IPV4_PRECEDENCE_CRITIC          = 0x05,
    TCPIP_IPV4_PRECEDENCE_INET_CONTROL    = 0x06,
    TCPIP_IPV4_PRECEDENCE_NET_CONTROL     = 0x07,

    TCPIP_IPV4_PRECEDENCE_MASK            = 0x07,

    // delay flag
    TCPIP_IPV4_DELAY_NORMAL               = 0x00,
    TCPIP_IPV4_DELAY_LOW                  = 0x08,

    // throughput flag
    TCPIP_IPV4_THROUGHPUT_NORMAL          = 0x00,
    TCPIP_IPV4_THROUGHPUT_HIGH            = 0x10,

    // reliability flag
    TCPIP_IPV4_RELIABILITY_NORMAL         = 0x00,
    TCPIP_IPV4_RELIABILITY_HIGH           = 0x20,

}TCPIP_IPV4_TOS_FLAGS;

// *****************************************************************************
/* IPv4 packet parameters

  Summary:
    IPv4 packet formatting parameters

  Description:
    This is the structure describing parameters that can be specified for the formatting of an IPv4 packet for transmission.

  Remarks:
    None.
 */

typedef struct
{
    /* Type Of Service specification */
    /* A value of 0 means TCPIP_IPV4_TOS_DEFAULT */
    TCPIP_IPV4_TOS_FLAGS tosFlags;
    /* Time to Live. 0 means default */
    uint8_t              ttl;
    /* Don't Fragment - boolean. 0 means default: fragmentation allowed */
    uint8_t              df;
}TCPIP_IPV4_PACKET_PARAMS;

// *****************************************************************************
/*
  Type:
    TCPIP_IPV4_PROCESS_HANDLE

  Summary:
    Defines a IPv4 packet processing handle.

  Description:
    Definition of an packet processing handle used for
    packet processing registration by the IPv4 clients.

*/
typedef const void* TCPIP_IPV4_PROCESS_HANDLE;

// *****************************************************************************
/* IPv4 packet handler Pointer

  Function:
    bool <FunctionName> (TCPIP_NET_HANDLE hNet, struct _tag_TCPIP_MAC_PACKET* rxPkt, const void* hParam);

  Summary:
    Pointer to a function(handler) that will get called to process an incoming IPv4 packet.

  Description:
    Pointer to a function that will be called by the IPv4 module
    when a RX packet is available.

  Precondition:
    None

  Parameters:
    hNet        - network handle on which the packet has arrived
    rxPkt       - pointer to incoming packet
    hParam      - user passed parameter when handler was registered

  Returns:
    true - if the packet is processed by the external handler.
           In this case the IPv4 module will no longer process the packet
    false - the packet needs to be processed internally by the IPv4 as usual           

  Remarks:
    The packet handler is called in the IPv4 context.
    The handler should be kept as short as possible as it affects the processing of all the other
    IPv4 RX traffic.

    Before calling the external packet handler 
    - the rxPkt->pNetLayer points to an IPV4_HEADER data structure.
    - the rxPkt->pktIf points to the interface receiving the packet
    - no other checks are done! (checksum, versions, etc.)

    Important!
    When the packet handler returns true, once it's done processing the packet,
    it needs to acknowledge it, i.e. return to the owner,
    which is the MAC driver serving the network interface!
    This means that the packet acknowledge function needs to be called,
    with a proper acknowledge parameter and the QUEUED flag needs to be cleared, if needed:
    if((*rxPkt->ackFunc)(rxPkt, rxPkt->ackParam))
    {
           rxPkt->pktFlags &= ~TCPIP_MAC_PKT_FLAG_QUEUED;
    }
    Failure to do that will result in memory leaks and starvation of the MAC driver.
    See the tcpip_mac.h for details.
    
 */
typedef bool(*TCPIP_IPV4_PACKET_HANDLER)(TCPIP_NET_HANDLE hNet, struct _tag_TCPIP_MAC_PACKET* rxPkt, const void* hParam);

// *****************************************************************************
// *****************************************************************************
// Section: Public Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Function:
    TCPIP_IPV4_PacketFormatTx(IPV4_PACKET* pPkt, uint8_t protocol, uint16_t ipLoadLen, TCPIP_IPV4_PACKET_PARAMS* pParams);

  Summary:
    Formats an IPV4 packet and makes it ready for transmission.

  Description:
    The necessary fields are set into the IPv4 packet.

  Precondition:
    Properly allocated pPkt.
    The source and destination addresses should be updated in the packet.
    The network interface should be updated in the IPV4_PACKET

  Parameters:
    pPkt      - the packet to be formatted
    protocol  - the protocol associated with the packet
    ipLoadLen - the IPv4 packet payload length
    pParams   - pointer to a structure containing parameters for this packet
                If NULL, then default values are used.

  Returns:
    None
      
  Remarks:
    The segments should be properly updated with the right number of bytes (segLen).
    The IP payload length (ipLoadLen) is added only to the 1st segment of the packet!
    Other segments (for packets having multiple packets) are not touched.

 */
void    TCPIP_IPV4_PacketFormatTx(IPV4_PACKET* pPkt, uint8_t protocol, uint16_t ipLoadLen, TCPIP_IPV4_PACKET_PARAMS* pParams);


// *****************************************************************************
/*
  Function:
    bool TCPIP_IPV4_PacketTransmit(IPV4_PACKET* pPkt);

  Summary:
    Transmits an IPV4 packet over the network.

  Description:
    The IPv4 packet is sent to the MAC for transmission.

  Precondition:
    pPkt should have been properly formatted with TCPIP_IPV4_PacketFormatTx().
    The packet interface should be updated.
        

  Parameters:
    pPkt     - the packet to be transmitted

  Returns:
    - true  - if the packet was handed to the MAC or is queued for transmission
    - false - the packet cannot be transmitted (wrong interface, etc.) 
      
  Remarks:
    Only single packets can be transmitted.
    Chained packets are not supported for now.
 */
bool    TCPIP_IPV4_PacketTransmit(IPV4_PACKET* pPkt);

// *****************************************************************************
/*
  Function:
    TCPIP_NET_HANDLE TCPIP_IPV4_SelectSourceInterface(TCPIP_NET_HANDLE netH, 
             IPV4_ADDR* pDestAddress, IPV4_ADDR* pSrcAddress, bool srcSet)

  Summary:
    Selects a source address and an interface based on the IPv4 destination address

  Description:
    Updates the pSrcAddress and returns the needed interface, if successful:
    * if srcSet == 1 and netH != 0, the function will not change anything 
    * if srcSet == 1 and netH == 0, the call will never fail it will use whatever 
      value in pSrcAddress (even 0) and will try to come up with an appropriate interface
    * if srcSet == 0 and netH == 0, it will use the destination address
    * if srcSet == 0 and netH != 0, it will use the address of that interface

  Precondition:
    netH has to be valid (if non-0).

  Parameters:
    netH            - network interface handle
    pDestAddress    - pointer to destination address
    pSrcAddress     - pointer to source address
    srcSet          - boolean; true if address pointed by pSrcAddress is valid

  Returns:
    - A valid interface - if it succeeds and a valid source interface selected
    - 0 - interface selection failed
      
  Remarks:
    None.
 */
TCPIP_NET_HANDLE   TCPIP_IPV4_SelectSourceInterface(TCPIP_NET_HANDLE netH, 
                 const IPV4_ADDR* pDestAddress, IPV4_ADDR* pSrcAddress, bool srcSet);


// *****************************************************************************
/*
  Function:
    TCPIP_NET_HANDLE TCPIP_IPV4_SelectDestInterface(IPV4_ADDR* pDestAddress)

  Summary:
    Selects an interface based on the IPv4 destination address

  Description:
    Updates the pSrcAddress and returns the needed interface, if successful:

  Precondition:
    None

  Parameters:
    pDestAddress    - pointer to destination address

  Returns:
    - A valid interface - if it succeeds and a valid interface selected
    - 0 - interface selection failed
      
  Remarks:
    None.
 */
TCPIP_NET_HANDLE    TCPIP_IPV4_SelectDestInterface(const IPV4_ADDR* pDestAddress);

// *****************************************************************************
/*
  Function:
    const IPV4_ADDR*  TCPIP_IPV4_PacketGetDestAddress(TCPIP_MAC_PACKET* pPkt);

  Summary:
    Returns the IPv4 destination address associated with a TCPIP_MAC_PACKET

  Description:
    The function will return a pointer to where the IPv4 destination address
    is located in the TCPIP_MAC_PACKET.
    The TCPIP_MAC_PACKET is supposed to be a valid IPv4 packet that has
    destination address properly set.

  Precondition:
    pPkt - valid IPv4 packet, pNetLayer filed properly set.
        
  Parameters:
    pPkt - packet to query

  Returns:
    - A valid pointer to an IPV4_ADDR - if it succeeds
    - 0 - if call failed
      
  Remarks:
    This function is primarily meant for RX packets.
 */
static __inline__ const IPV4_ADDR* TCPIP_IPV4_PacketGetDestAddress(TCPIP_MAC_PACKET* pPkt);

static __inline__ const IPV4_ADDR* __attribute__((always_inline)) TCPIP_IPV4_PacketGetDestAddress(TCPIP_MAC_PACKET* pPkt)
{
    return &((IPV4_HEADER*)pPkt->pNetLayer)->DestAddress;
}

// *****************************************************************************
/*
  Function:
    const IPV4_ADDR*  TCPIP_IPV4_PacketGetSourceAddress(TCPIP_MAC_PACKET* pPkt);

  Summary:
    Returns the IPv4 source address associated with a TCPIP_MAC_PACKET

  Description:
    The function will return a pointer to where the IPv4 source address
    is located in the TCPIP_MAC_PACKET.
    The TCPIP_MAC_PACKET is supposed to be a valid IPv4 packet that has
    properly source address set.

  Precondition:
    pPkt - valid IPv4 packet, pNetLayer filed properly set
        

  Parameters:
    pPkt - packet to query


  Returns:
    - A valid pointer to an IPV4_ADDR - if it succeeds
    - 0 - if call failed
      
  Remarks:
    This function is primarily meant for RX packets.
 */
static __inline__ const IPV4_ADDR* TCPIP_IPV4_PacketGetSourceAddress(TCPIP_MAC_PACKET* pPkt);

static __inline__ const IPV4_ADDR* __attribute__((always_inline)) TCPIP_IPV4_PacketGetSourceAddress(TCPIP_MAC_PACKET* pPkt)
{
    return &((IPV4_HEADER*)pPkt->pNetLayer)->SourceAddress;
}

// *****************************************************************************
/*
  Function:
    TCPIP_IPV4_FILTER_TYPE    TCPIP_IPV4_PacketFilterSet(TCPIP_IPV4_FILTER_TYPE filtType);

  Summary:
    Sets the IPV4 packet filters

  Description:
    The function will set the IPv4 packet filters.
    The filters that are present in the mask will be set.
    Other filters won't be touched.

  Precondition:
    filtType - valid IPv4 filter
    IPv4 properly initialized
        

  Parameters:
    filtType - packet filter mask to set


  Returns:
    - the current value of the IPV4 packet filters after this mask was applied.
      
  Remarks:
    None.
 */
TCPIP_IPV4_FILTER_TYPE    TCPIP_IPV4_PacketFilterSet(TCPIP_IPV4_FILTER_TYPE filtType);


// *****************************************************************************
/*
  Function:
    TCPIP_IPV4_FILTER_TYPE    TCPIP_IPV4_PacketFilterClear(TCPIP_IPV4_FILTER_TYPE filtType);

  Summary:
    Clears the IPV4 packet filters

  Description:
    The function will clear the IPv4 packet filters.
    The filters that are present in the mask will be cleared.
    Other filters won't be touched.

  Precondition:
    filtType - valid IPv4 filter
    IPv4 properly initialized
        

  Parameters:
    filtType - packet filter mask to clear


  Returns:
    - the current value of the IPV4 packet filters after this mask was applied.
      
  Remarks:
    None.
 */
TCPIP_IPV4_FILTER_TYPE    TCPIP_IPV4_PacketFilterClear(TCPIP_IPV4_FILTER_TYPE filtType);


// *****************************************************************************
/*
  Function:
    bool TCPIP_IPV4_MacPacketTransmit(TCPIP_MAC_PACKET* pPkt, TCPIP_NET_HANDLE hNet, IPV4_ADDR* pDestAddress);

  Summary:
    Transmits a fully constructed MAC packet over the network.

  Description:
    The function is a helper to transmit a fully constructed MAC packet over an interface.
    The MAC packet is supposed to have all the fields properly constructed
    (including the TCPIP_MAC_ETHERNET_HEADER fields).
    The MAC packet does not need ARP resolution since the DestMACAddr is already known. 
    The packet acknowledge function should be properly set (it will be called by the MAC 
    driver once the packet is transmitted).

  Precondition:
    pPkt should have been properly formatted and constructed into a valid
    MAC packet that can go out over an interface
    pDestAddress - valid pointer
        

  Parameters:
    pPkt        - the packet to be transmitted
    hNet        - interface over which the packet is to be transmitted
    pDestAddress- the IPv4 packet destination address
                  It is used only for routing purposes, to decide if the
                  packet is routed internally or sent to the network interface

  Returns:
    - true  - if the packet was handed to the MAC or queued internally
    - false - the packet cannot be transmitted (wrong interface, etc.) 
      
  Remarks:
    Only single packets can be transmitted.
    Chained packets are not supported for now.

    The packet could be composed of multiple fragments.
    If the transmission fails, some fragments could have been transmitted
    while other failed.
    The flag TCPIP_MAC_PKT_FLAG_QUEUED could be used to determine which 
    fragments didn't make it through.

 */
bool    TCPIP_IPV4_MacPacketTransmit(TCPIP_MAC_PACKET* pPkt, TCPIP_NET_HANDLE hNet, IPV4_ADDR* pDestAddress);

// *****************************************************************************
/*
  Function:
    IPV4_PACKET* TCPIP_IPV4_PacketAlloc(uint16_t pktPayload, TCPIP_IPV4_OPTION_DCPT* pOptions, int nOptions,  TCPIP_MAC_PACKET_FLAGS flags);

  Summary:
    Allocates an IPv4 packet for transmission over the network.

  Description:
    The function is a helper to allocate an IPv4 packet that supports options.
    For fixed size options, the size of the options is determined by its type and
    the supplied size is ignored. 
   

  Precondition:
    IPv4 properly initialized
        

  Parameters:
    pktPayload  - the packet payload
    pOptions    - pointer to an array of option descriptors to be part of this packet
    nOptions    - number of supplied options
    flags       - standard MAC packet flags


  Returns:
    - a valid IPV4_PACKET - if the allocation succeeded
    - 0 - the packet cannot be allocated
      
  Remarks:
        If the option type contains a fixed option length for a supported type (TCPIP_IPV4_OPTION_ROUTER_ALERT, for ex.),
        then the size of the the fixed option is calculated internally amd the supplied option size is ignored.

        For not supported or variable size options the supplied optionLen bytes will be added to the 
        allocated packet.


 */
IPV4_PACKET* TCPIP_IPV4_PacketAlloc(uint16_t pktPayload, TCPIP_IPV4_OPTION_DCPT* pOptions, int nOptions,  TCPIP_MAC_PACKET_FLAGS flags);

// *****************************************************************************
/*
  Function:
    IPV4_OPTION_FIELD* TCPIP_IPV4_PacketOptionFieldSet(IPV4_PACKET* pPkt, uint16_t optionLen, bool firstOption);

  Summary:
    Helper to set an IPv4 packet option field.

  Description:
    The function is a helper to allow setting a specific option into an IPv4 packet that supports options.
    It is meant for supporting user controlled and variable size options.

   
  Precondition:
    IPv4 properly initialized
    pPkt properly created with TCPIP_IPV4_PacketAlloc
        

  Parameters:
    pPkt        - a packet allocated with TCPIP_IPV4_PacketAlloc
    optionLen   - size for the option to set, bytes
                  This is the size of the whole IPV4_OPTION_FIELD, the option data included
    firstOption - specifies that this is the first option to be set to the packet


  Returns:
    - a valid IPV4_OPTION_FIELD pointer that the caller can use to set the IPv4 option fields
    - 0 - invalid packet or the option field cannot hold the required option size
      
  Remarks:
    This function should be called for unsupported options or for options that have variable data.
    It doesn't have to be used for standard supported options (TCPIP_IPV4_OPTION_ROUTER_ALERT, for ex.)
    
    The packet should have been allocated with enough option space using TCPIP_IPV4_PacketAlloc.

    Sequential calls can be made using firstOption true for the first call and false for the 
    subsequent calls.

    When setting the option data, optionLen MUST NOT be exceeded!

    If the option doesn't take a multiple of 4 bytes, it should be padded with the NOP option (0x01).

 */
IPV4_OPTION_FIELD* TCPIP_IPV4_PacketOptionFieldSet(IPV4_PACKET* pPkt, uint16_t optionLen, bool firstOption);

// *****************************************************************************
/*
  Function:
    int TCPIP_IPV4_PacketOptionListGet(TCPIP_MAC_PACKET* pRxPkt, TCPIP_IPV4_OPTION_DCPT* pOptions, int nOptions );

  Summary:
    Helper to get the options that a IPv4 receive packet has.

  Description:
    The function is a helper to allow retrieving a list of options that are present in the received packet.

   
  Precondition:
    IPv4 properly initialized
    pRxPkt a proper IPv4 packet received by the IPv4 module
        

  Parameters:
    pRxPkt      - a packet received by the IPv4 module
    pOptions    - pointer to an array of option descriptors to receive the options present in the packet
    nOptions    - number of elements the array can store
                  Could be 0, in which case pOptions is not updated

  Returns:
    - the number of options found in the packet; 
    - 0 - invalid packet or packet without options
      
  Remarks:
    The packet should be a proper IPv4 packet received through the IPv4 module.
    Only minimal consistency check is done.

    On return, the pOptions is updated, up to nOptions;

 */
int TCPIP_IPV4_PacketOptionListGet(TCPIP_MAC_PACKET* pRxPkt, TCPIP_IPV4_OPTION_DCPT* pOptions, int nOptions );


// *****************************************************************************
/*
  Function:
    int TCPIP_IPV4_PacketOptionGet(TCPIP_MAC_PACKET* pRxPkt, TCPIP_IPV4_OPTION_TYPE optType, IPV4_OPTION_FIELD* pOptField, int optSize);

  Summary:
    Helper to get the options that a IPv4 receive packet has.

  Description:
    The function is a helper to allow retrieving a specific option that is present in an IPv4 received packet.

   
  Precondition:
    IPv4 properly initialized
    pRxPkt a proper IPv4 packet received by the IPv4 module
        

  Parameters:
    pRxPkt      - a packet received by the IPv4 module
    optType     - type of options to be searched/retrieved
    pOptField   - address to store the option
                  Could be NULL if not needed.
    optSize     - size of the pOptField structure, including the optData part
                  If 0, the pOptField will not be updated


  Returns:
    - size of the option, if found in the IPv4 packet
    - 0 if such an option does not exit
      
  Remarks:
    The packet should be a proper IPv4 packet received through the IPv4 module.
    Only minimal consistency check is done.

 */
int TCPIP_IPV4_PacketOptionGet(TCPIP_MAC_PACKET* pRxPkt, TCPIP_IPV4_OPTION_TYPE optType, IPV4_OPTION_FIELD* pOptField, int optSize);

// *****************************************************************************
/*
  Function:
    int TCPIP_IPV4_MaxDatagramDataSizeGet(TCPIP_NET_HANDLE netH);

  Summary:
    Helper to get the maximum datagream data size (MDDS) on a specific interface.
   
  Description:
    The function is a helper to allow retrieving the MDDS on the specified interface.
   
  Precondition:
    IPv4 properly initialized
    Interface properly initialized
        

  Parameters:
    netH      - handle of the interface

  Returns:
    - the maximum datagrem data size that can be transported on the selected interface
    - 0 - invalid interface specified
      
  Remarks:
    None

 */
int TCPIP_IPV4_MaxDatagramDataSizeGet(TCPIP_NET_HANDLE netH);

// *****************************************************************************
/*
  Function:
    bool TCPIP_IPV4_IsFragmentationEnabled(void);

  Summary:
    Helper to get the IPv4 fragmentation support.
   
  Description:
    The function is a helper to return the IPv4 fragmentation support.
   
  Precondition:
    IPv4 properly initialized

  Parameters:
    None

  Returns:
    - true  - if the fragmentation is enabled and supported by the IPv4 module 
    - false - fragmentation is not supported
      
  Remarks:
    The IPv4 fragmentation support is a build time option.

 */
bool TCPIP_IPV4_IsFragmentationEnabled(void);

//*******************************************************************************
/*
  Function:
    TCPIP_IPV4_PROCESS_HANDLE    TCPIP_IPV4_PacketHandlerRegister(TCPIP_IPV4_PACKET_HANDLER pktHandler, const void* handlerParam)

  Summary:
    Sets a new packet processing handler.

  Description:
    This function registers a new packet processing handler.
    The caller can use the handler to be notified of incoming packets
    and given a chance to examine/process them.

  Precondition:
    IPv4 properly initialized

  Parameters:
    pktHandler      - the packet handler which will be called for an incoming packet
    handlerParam    - packet handler parameter

  Returns:
    - a valid TCPIP_IPV4_PROCESS_HANDLE - if the operation succeeded
    - NULL - if the operation failed

  Example:
    <code>
    TCPIP_IPV4_PROCESS_HANDLE pktHandle = TCPIP_IPV4_PacketHandlerRegister( myPktHandler, myParam );
    </code>

  Remarks:
    Currently only one packet handler is supported for the IPv4 module.
    The call will fail if a handler is already registered.
    Use TCPIP_IPV4_PacketHandlerDeregister first

    Exists only if TCPIP_IPV4_EXTERN_PACKET_PROCESS is true 

  */
TCPIP_IPV4_PROCESS_HANDLE     TCPIP_IPV4_PacketHandlerRegister(TCPIP_IPV4_PACKET_HANDLER pktHandler, const void* handlerParam);


//*******************************************************************************
/*
  Function:
    bool    TCPIP_IPV4_PacketHandlerDeregister(TCPIP_IPV4_PROCESS_HANDLE pktHandle);

  Summary:
    Deregisters a previously registered packet handler.

  Description:
    This function removes a packet processing handler.

  Precondition:
    IPv4 properly initialized

  Parameters:
    pktHandle   - TCPIP packet handle obtained by a call to TCPIP_IPV4_PacketHandlerRegister


  Returns:
    - true  - if the operation succeeded
    - false - if the operation failed

  Example:
    <code>
    TCPIP_IPV4_PROCESS_HANDLE myHandle = TCPIP_IPV4_PacketHandlerRegister(myPacketHandler, myParam );
    // process incoming packets
    // now we're done with it
    TCPIP_IPV4_PacketHandlerDeregister(myHandle);
    </code>

  Remarks:
    Exists only if TCPIP_IPV4_EXTERN_PACKET_PROCESS is true 

  */
bool    TCPIP_IPV4_PacketHandlerDeregister(TCPIP_IPV4_PROCESS_HANDLE pktHandle);

// *****************************************************************************
/*
  Function:
    TCPIP_IPV4_RES TCPIP_IPV4_FwdTableAddAscii(const TCPIP_IPV4_FORWARD_ENTRY_ASCII* pEntry, size_t nEntries);

  Summary:
    Helper to dynamically add entries to the FIB using the ASCII format
   
  Description:
    The function is a helper that dynamically adds entries to the forwarding table using the ASCII format 
   
  Precondition:
    IPv4 properly initialized
    Forwarding supported
        

  Parameters:
    pEntry    - pointer to a valid ASCII format entries
    nEntries  - number of entries in the array

  Returns:
    - TCPIP_IPV4_RES_OK if operation successful
    - an TCPIP_IPV4_RES < 0 if an error occurred
   
      
  Remarks:
    Function exists only if IPv4 forwarding is enabled and TCPIP_IPV4_FORWARDING_DYNAMIC_API != 0

    The ASCII format is supported when build symbol TCPIP_IPV4_FORWARDING_TABLE_ASCII != 0

    Routing is suspended while this operation is in progress

    Operation is expensive and occurs with locking of resources.

    Entries will be added sequentially and the operation will stop once an error has occurred.
    Returning an error may still mean that some entries have been added to the FIB!
    Use one single entry if control needed over individual entries.
 */
TCPIP_IPV4_RES TCPIP_IPV4_FwdTableAddAscii(const TCPIP_IPV4_FORWARD_ENTRY_ASCII* pEntry, size_t nEntries);

// *****************************************************************************
/*
  Function:
    TCPIP_IPV4_RES TCPIP_IPV4_FwdTableRemoveAscii(const TCPIP_IPV4_FORWARD_ENTRY_ASCII* pEntry, size_t nEntries);

  Summary:
    Helper to dynamically remove entries from the FIB using the ASCII format
   
  Description:
    The function is a helper that dynamically removes entries from the forwarding table using the ASCII format 
   
  Precondition:
    IPv4 properly initialized
    Forwarding supported
        

  Parameters:
    pEntry    - pointer to a valid ASCII format entries
    nEntries  - number of entries in the array

  Returns:
    - TCPIP_IPV4_RES_OK if operation successful
    - an TCPIP_IPV4_RES < 0 if an error occurred
   
      
  Remarks:
    Function exists only if IPv4 forwarding is enabled and TCPIP_IPV4_FORWARDING_DYNAMIC_API != 0

    The ASCII format is supported when build symbol TCPIP_IPV4_FORWARDING_TABLE_ASCII != 0

    Routing is suspended while this operation is in progress

    Operation is expensive and occurs with locking of resources.

    Entries will be removed sequentially and the operation will stop once an error has occurred.
    Returning an error may still mean that some entries have been removed to the FIB!
    Use one single entry if control needed over individual entries.
 */
TCPIP_IPV4_RES TCPIP_IPV4_FwdTableRemoveAscii(const TCPIP_IPV4_FORWARD_ENTRY_ASCII* pEntry, size_t nEntries);

// *****************************************************************************
/*
  Function:
    TCPIP_IPV4_RES TCPIP_IPV4_FwdTableAddBinary(const TCPIP_IPV4_FORWARD_ENTRY_BIN* pEntry, size_t nEntries);

  Summary:
    Helper to dynamically add entries to the FIB using the binary format
   
  Description:
    The function is a helper that dynamically adds multiple entries to the forwarding table using the binary format 
   
  Precondition:
    IPv4 properly initialized
    Forwarding supported
        

  Parameters:
    pEntry    - pointer to a valid binary format array of entries
    nEntries  - number of entries in the array

  Returns:
    - TCPIP_IPV4_RES_OK if operation successful
    - an TCPIP_IPV4_RES < 0 if an error occurred
   
      
  Remarks:
    Function exists only if IPv4 forwarding is enabled and TCPIP_IPV4_FORWARDING_DYNAMIC_API != 0

    The binary format is always supported 

    Routing is suspended while this operation is in progress

    Operation is expensive and occurs with locking of resources.

    Entries will be added sequentially and the operation will stop once an error has occurred.
    Returning an error may still mean that some entries have been added to the FIB!
    Use one single entry if control needed over individual entries.
 */
TCPIP_IPV4_RES TCPIP_IPV4_FwdTableAddBinary(const TCPIP_IPV4_FORWARD_ENTRY_BIN* pEntry, size_t nEntries);

// *****************************************************************************
/*
  Function:
    TCPIP_IPV4_RES TCPIP_IPV4_FwdTableRemoveBinary(const TCPIP_IPV4_FORWARD_ENTRY_BIN* pEntry, size_t nEntries);

  Summary:
    Helper to dynamically remove entries from the FIB using the binary format
   
  Description:
    The function is a helper that dynamically removes entries from the forwarding table using the binary format 
    The FIB is searched to find an exact match with the supplied entries. 
   
  Precondition:
    IPv4 properly initialized
    Forwarding supported
        

  Parameters:
    pEntry    - pointer to a valid binary format array of entries
    nEntries  - number of entries in the array

  Returns:
    - TCPIP_IPV4_RES_OK if operation successful
    - an TCPIP_IPV4_RES < 0 if an error occurred
   
      
  Remarks:
    Function exists only if IPv4 forwarding is enabled and TCPIP_IPV4_FORWARDING_DYNAMIC_API != 0

    The binary format is always supported 

    Routing is suspended while this operation is in progress

    Operation is expensive and occurs with locking of resources.

    Entries will be removed sequentially and the operation will stop once an error has occurred.
    Returning an error may still mean that some entries have been removed to the FIB!
    Use one single entry if control needed over individual entries.
 */
TCPIP_IPV4_RES TCPIP_IPV4_FwdTableRemoveBinary(const TCPIP_IPV4_FORWARD_ENTRY_BIN* pEntry, size_t nEntries);

// *****************************************************************************
/*
  Function:
    TCPIP_IPV4_RES TCPIP_IPV4_FwdTableRemoveAll(TCPIP_NET_HANDLE netH);

  Summary:
    Helper to dynamically remove all entries from the FIB
   
  Description:
    The function is a helper that dynamically removes all entries from the forwarding table
    corresponding to an interface
   
  Precondition:
    IPv4 properly initialized
    Forwarding supported
        

  Parameters:
    netH    - network interface handle

  Returns:
    - TCPIP_IPV4_RES_OK if operation successful
    - an TCPIP_IPV4_RES < 0 if an error occurred
   
      
  Remarks:
    Function exists only if IPv4 forwarding is enabled and TCPIP_IPV4_FORWARDING_DYNAMIC_API != 0

    Routing is suspended while this operation is in progress

    Operation is expensive and occurs with locking of resources.

    All entries will be removed from the FIB
    The forwarding on that interface is practically disabled
 */
TCPIP_IPV4_RES TCPIP_IPV4_FwdTableRemoveAll(TCPIP_NET_HANDLE netH);

// *****************************************************************************
/*
  Function:
    size_t TCPIP_IPV4_ForwadTableSizeGet(TCPIP_NET_HANDLE netH, size_t* pValid);

  Summary:
    Helper to get the total number of entries in the forwarding table
    for the required interface
   
  Description:
    The function is a helper that returns the total number of entries in the 
    interface forwarding table
    It also returns the number of currently used entries
   
  Precondition:
    IPv4 properly initialized
    IPv4 forwarding enabled
        

  Parameters:
    netH    - network interface handle
    pValid  - address to store the number of valid (in use) entries in the table
              Could be NULL if not needed.

  Returns:
    - the total number of entries + th eones currently used in the forwarding table
      for the exiting interface
    - 0 if not a valid interface or forwarding table does not exist
   
      
  Remarks:
    Function exists only if IPv4 forwarding is enabled

 */
size_t TCPIP_IPV4_ForwadTableSizeGet(TCPIP_NET_HANDLE netH, size_t* pValid);

// *****************************************************************************
/*
  Function:
    bool TCPIP_IPV4_ForwadTableEntryGet(TCPIP_NET_HANDLE netH, size_t index, TCPIP_IPV4_FORWARD_ENTRY_BIN* pFwdEntry);

  Summary:
    Helper to get the contents of a forwarding table
   
  Description:
    The function is a helper that returns the contents of the specified
    entry in the forwarding table for the selected interface
   
  Precondition:
    IPv4 properly initialized
    IPv4 forwarding enabled
        

  Parameters:
    netH        - network interface handle
    index       - index of the entry to get
    pFwdEntry   - pointer to a structure to store the table entry

  Returns:
    - true if a valid index, valid interface and forwarding table exists
    - false otherwise
      
  Remarks:
    Function exists only if IPv4 forwarding is enabled

 */
bool TCPIP_IPV4_ForwadTableEntryGet(TCPIP_NET_HANDLE netH, size_t index, TCPIP_IPV4_FORWARD_ENTRY_BIN* pFwdEntry);

// *****************************************************************************
/*
  Function:
    bool TCPIP_IPv4_ForwardStatGet(size_t index, TCPIP_IPV4_FORWARD_STAT* pStat, bool clear);

  Summary:
    Helper to get the forwarding statistics
   
  Description:
    The function is a helper that returns the contents of the specified
    entry in the forwarding statistics
   
  Precondition:
    IPv4 properly initialized
    IPv4 forwarding enabled
    IPv4 forwarding debugging enabled
        

  Parameters:
    index       - index of the entry to get
    pFwdEntry   - pointer to a structure to store the table entry
    clear   - if true, the statistics are cleared

  Returns:
    - true if a valid index
    - false otherwise
      
  Remarks:
    Function exists only if IPv4 forwarding is enabled

    Currently the forwarding statistics is maintained in 3 different slots:
    - index 0: for interface 0
    - index 1: for interface 1
    - index 2: for all other interfaces

 */
bool TCPIP_IPv4_ForwardStatGet(size_t index, TCPIP_IPV4_FORWARD_STAT* pStat, bool clear);

// *****************************************************************************
/*
  Function:
    bool TCPIP_IPv4_ArpStatGet(TCPIP_IPV4_ARP_QUEUE_STAT* pStat, bool clear);

  Summary:
    Helper to get the ARP statistics
   
  Description:
    The function is a helper that returns the contents of the ARP statistics
    maintained by the IPv4 module
   
  Precondition:
    IPv4 properly initialized
    IPv4 ARP debugging enabled
        

  Parameters:
    pStat   - pointer to a structure to store the ARP statistics
    clear   - if true, the statistics are cleared

  Returns:
    - true if success - ARP statistics exist
    - false otherwise
      
  Remarks:
    Function exists only if IPv4 ARP debugging is enabled

 */
bool TCPIP_IPv4_ArpStatGet(TCPIP_IPV4_ARP_QUEUE_STAT* pStat, bool clear);


// *****************************************************************************
/*
  Function:
    void  TCPIP_IPV4_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs IPv4 module tasks in the TCP/IP stack.

  Precondition:
    The IPv4 module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_IPV4_Task(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // __IPV4_H_ 
