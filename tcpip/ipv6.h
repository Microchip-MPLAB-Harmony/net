/*******************************************************************************
  IPv6 Definitions for the Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    ipv6.h

  Summary:
    IPv6 is the next generation TCP/IP protocol suite.

  Description:
    Like IPv4 , IPv6 (Internet Protocol Version 6) is the another version of IP protocol.
    This is the next-generation protocol  which works parallel with IPv4. Microchip TCP/IP 
    stack is a dual stack architecture where both Ipv4 and Ipv6 works simultaneously.
    (RFC - 2460,3484)
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

#ifndef H_IPV6_H_
#define H_IPV6_H_

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
// *****************************************************************************
// Section: public definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* 
  Enumeration:
    IPV6_NEXT_HEADER_TYPE

  Summary:
    Defines a list of IPv6 next header types.

  Description:
    Identifies the type of the next header immediately follows IPv6 header.

  Remarks:
    None
 */
typedef enum
{
    IPV6_PROT_HOP_BY_HOP_OPTIONS_HEADER = (0u), // IPv6 Hop-by-Hop Opt. Header
    IPV6_PROT_ICMP = (1u), // ICMPv4 Header
    IPV6_PROT_TCP = (6u), // TCP protocol Header
    IPV6_PROT_UDP = (17u), // UDP protocol Header
    IPV6_PROT_IPV6 = (41u), // IPv6 Protocol
    IPV6_PROT_ROUTING_HEADER = (43u), // IPv6 Routing Header
    IPV6_PROT_FRAGMENTATION_HEADER = (44u), // IPv6 Fragmentation Header
    IPV6_PROT_ENCAPSULATING_SECURITY_PAYLOAD_HEADER = (50u), // Encapsulating Security Payload Header
    IPV6_PROT_AUTHENTICATION_HEADER = (51u), // Authentication Header
    IPV6_PROT_ICMPV6 = (58u), // ICMPv6 Protocol
    IPV6_PROT_NONE = (59u), // No next header
    IPV6_PROT_DESTINATION_OPTIONS_HEADER = (60u) // Destination Options Header
} IPV6_NEXT_HEADER_TYPE;

// IP Version details
#define IP_VERSION_6    (1u) // Using IPv6 
#define IP_VERSION_4    (0u) // Using IPv4

// *****************************************************************************
/* 
  Enumeration:
    IPV6_ACTION 

  Summary:
    Provides a list of possible IPv6 actions.

  Description:
    Different actions need to be taken depending on the result of our header processing.

  Remarks:
    None.
 */
typedef enum {
    IPV6_ACTION_NONE = 0, // No action. Skip the option 
    IPV6_ACTION_DISCARD_SILENT, // Discard the packet silently
    IPV6_ACTION_DISCARD_PP_0, // Discard the packet and send an ICMP parameter problem message with code value 0
    IPV6_ACTION_DISCARD_PP_2, // Discard the packet and send an ICMP parameter problem message with code value 2
    IPV6_ACTION_DISCARD_PP_2_NOT_MC, // Discard the packet and send an ICMP parameter problem message with code value 3
    IPV6_ACTION_BEGIN_EX_HEADER_PROCESSING, // Begin extension processing

} IPV6_ACTION;

/* IPv6 Type-length-value type code for the Pad 1 option */
#define IPV6_TLV_PAD_1                          0u
/* IPv6 Type-length-value type code for the Pad N option */
#define IPV6_TLV_PAD_N                          1u
/* IPv6 Type-length-value type code for the Hop-by-hop "Jumbo-gram Payload" option */
#define IPV6_TLV_HBHO_PAYLOAD_JUMBOGRAM         0xC2u
/* IPv6 Type-length-value type code for the Hop-by-hop "Router Alert" option */
#define IPV6_TLV_HBHO_ROUTER_ALERT              0x05u

/* IPv6 action code for the unrecognized option reaction to skip the option */
#define IPV6_TLV_UNREC_OPT_SKIP_OPTION          0x00
/* IPv6 action code for the unrecognized option reaction to discard the packet silently */
#define IPV6_TLV_UNREC_OPT_DISCARD_SILENT       0x01
/* IPv6 action code for the unrecognized option reaction to discard the packet and send 
an ICMP parameter problem message 
*/
#define IPV6_TLV_UNREC_OPT_DISCARD_PP           0x02
/* IPv6 action code for the unrecognized option reaction to discard the packet and send
 an ICMP parameter problem message is the destination address isn't a multicast address
*/
#define IPV6_TLV_UNREC_OPT_DISCARD_PP_NOT_MC    0x03

/* Header offset for payload length */
#define IPV6_HEADER_OFFSET_PAYLOAD_LENGTH       (0x04u) 
/* Header offset for next header */
#define IPV6_HEADER_OFFSET_NEXT_HEADER          (0x06u) 
/* Header offset for source address */
#define IPV6_HEADER_OFFSET_SOURCE_ADDR          (0x08u)
/* Header offset for destination address */
#define IPV6_HEADER_OFFSET_DEST_ADDR            (0x08u + sizeof (IPV6_ADDR)) 


// *****************************************************************************
/* 
  Enumeration: 
    IPV6_ADDRESS_PREFERENCE 

  Summary:
    Provides selection of public versus temporary addresses.

  Description:
    None

  Remarks:
    None.
 */
typedef enum
{
    IPV6_PREFER_PUBLIC_ADDRESSES = 0,
    IPV6_PREFER_TEMPORARY_ADDRESSES
} IPV6_ADDRESS_PREFERENCE;


/* MISRA C-2012 Rule 5.2 deviated:1 Deviation record ID -  H3_MISRAC_2012_R_5_2_NET_DR_6 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:1 "MISRA C-2012 Rule 5.2" "H3_MISRAC_2012_R_5_2_NET_DR_6" 
// *****************************************************************************
/* 
  Enumeration: 
  IPV6_SEGMENT_TYPE 

  Summary:
    Provides an enumeration of IPv6 segment types.

  Description:
    IPv6 extended header.

  Remarks:
    None.
 */
typedef enum
{
    TYPE_IPV6_HEADER = 1u,
    TYPE_IPV6_EX_HEADER_HOP_BY_HOP_OPTIONS,
    TYPE_IPV6_EX_HEADER_DESTINATION_OPTIONS_1,
    TYPE_IPV6_EX_HEADER_ROUTING,
    TYPE_IPV6_EX_HEADER_FRAGMENT,
    TYPE_IPV6_EX_HEADER_AUTHENTICATION_HEADER,
    TYPE_IPV6_EX_HEADER_ENCAPSULATING_SECURITY_PAYLOAD,
    TYPE_IPV6_EX_HEADER_DESTINATION_OPTIONS_2,
    TYPE_IPV6_UPPER_LAYER_HEADER,
    TYPE_IPV6_UPPER_LAYER_PAYLOAD,
    TYPE_IPV6_BEGINNING_OF_WRITABLE_PART,
    TYPE_IPV6_END_OF_LIST
} IPV6_SEGMENT_TYPE;
#pragma coverity compliance end_block "MISRA C-2012 Rule 5.2"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */

//DOM-IGNORE-BEGIN
extern const IPV6_ADDR IPV6_FIXED_ADDR_UNSPECIFIED;
extern const IPV6_ADDR IPV6_FIXED_ADDR_ALL_NODES_MULTICAST;
extern const IPV6_ADDR IPV6_FIXED_ADDR_ALL_ROUTER_MULTICAST;
extern const IPV6_ADDR IPV6_SOLICITED_NODE_MULTICAST;
extern const TCPIP_MAC_ADDR  IPV6_MULTICAST_MAC_ADDRESS;
//DOM-IGNORE-END

/* MISRA C-2012 Rule 5.5 deviated:2 Deviation record ID -  H3_MISRAC_2012_R_5_5_NET_DR_8 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:2 "MISRA C-2012 Rule 5.5" "H3_MISRAC_2012_R_5_5_NET_DR_8" 
// *****************************************************************************
/* 
  Type:
    IPV6_TLV_OPTION_TYPE

  Summary:
    Data structure for IPv6 TLV options.

  Description:
    This type defines the data structure for IPv6 TLV options.

  Remarks:
    None.
 */
typedef union
{
    unsigned char b;

    struct
    {
        unsigned option : 6;
        unsigned unrecognizedAction : 2;
    } bits;
} IPV6_TLV_OPTION_TYPE;
#pragma coverity compliance end_block "MISRA C-2012 Rule 5.5"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */


/* MISRA C-2012 Rule 5.6 deviated:1 Deviation record ID -  H3_MISRAC_2012_R_5_6_NET_DR_14 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:1 "MISRA C-2012 Rule 5.6" "H3_MISRAC_2012_R_5_6_NET_DR_14" 
// *****************************************************************************
/* 
  Type:
    IPV6_ADDRESS_TYPE

  Summary:
    Data structure for IPv6 address types.

  Description:
    This type defines the data structure for IPv6 address types.

  Remarks:
    None.
 */
typedef union
{
    unsigned char byte;

    struct
    {
        unsigned scope : 4;
        unsigned type : 2;
    } bits;
} IPV6_ADDRESS_TYPE;
#pragma coverity compliance end_block "MISRA C-2012 Rule 5.6"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */


// *****************************************************************************
/* 
  Type:
    IPV6_ADDRESS_POLICY

  Summary:
    Data structure for IPv6 address policy.

  Description:
    The policy table is the longest matching prefix lookup table and is 
    used to select the destination IPv6 Address.
    
    If Precedence(A) > Precedence(B), address A has higher precedence than address B.
    The label value Label(A) allows for policies that prefer a particular
    source address prefix for use with a destination address prefix.
    Default Policy table -
    Prefix          Precedence  Label
    ::1/128             50      0
    ::/0                40      1
    2002::/16           30      2
    ::/96               20      3
    ::ffff:0.0.0.0/96   10      4
    
  Example -
    const IPV6_ADDRESS_POLICY gPolicyTable[] = {
    {{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}}, 128, 50,  0},          - Loopback address
    {{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},   0, 40,  1},          - Unspecified address
    {{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00}},  96, 35,  4},          - IPv4-mapped address
    {{{0x20, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},  16, 30,  2},          - 2002::/15 - 6to4
    {{{0x20, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},  32,  5,  5},          - 2001::/32 - Teredo tunneling
    {{{0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},   7,  3, 13},          - ULA
    };
  
  Remarks:
    None.
 */
typedef struct
{
    IPV6_ADDR address; // IPv6 address
    unsigned char prefixLength; // IPv6 prefix length
    unsigned char precedence; // IPv6 address precedence
    unsigned char label; // IPv6 label type
} IPV6_ADDRESS_POLICY;


// *****************************************************************************
/* 
  Structure :
    IPV6_DATA_SEGMENT_HEADER

  Summary:
    Data structure for IPv6 Data Segment header.

  Description:
    Data structure is used to allocate a data segment header and an optional payload.

  Remarks:
    None.
 */
typedef struct S_IPV6_DATA_SEGMENT_HEADER
{
    uint8_t* dataLocation; // Location of the data to transmit
    unsigned short segmentSize; // Size of this data segment
    unsigned short segmentLen; // Number of bytes of data in this segment
    unsigned char memory; // Type: IPV6_DATA_NONE, IPV6_DATA_DYNAMIC_BUFFER, IPV6_DATA_NETWORK_FIFO, IPV6_DATA_PIC_RAM
    unsigned char segmentType; // Type of segment contents
    struct S_IPV6_DATA_SEGMENT_HEADER * nextSegment; // Pointer to the next data segment
    // void * data[];  // Optional buffer space
} IPV6_DATA_SEGMENT_HEADER;



// *****************************************************************************
/* 
  Structure: 
    IPV6_RX_FRAGMENT_BUFFER

  Summary:
    Data structure for IPv6 Received fragmented packet.

  Description:
    Each fragment is composed of unfragmentable Part and  fragmentable part.
    Allocate  memory for the fragmented packet w.r.t to TCPIP_IPV6_RX_FRAGMENTED_BUFFER_SIZE.
    The RFC specifies that the fragments must be reassembled in one minute or less.secondsRemaining
    is the times remaining for reassemble.  

  Remarks:
    None.
 */
typedef struct S_IPV6_RX_FRAGMENT_BUFFER
{
    struct S_IPV6_RX_FRAGMENT_BUFFER * next; // Next fragmented packet    
    uint8_t *ptrPacket; // Packet information
    uint32_t identification; // Fragment id
    uint16_t bytesInPacket; // Number of bytes written to packet
    uint16_t packetSize; // Packet size (packet is complete when this matches bytesInPacket)
    uint16_t firstFragmentLength; // Length of the first fragment
    uint8_t secondsRemaining; // Number of seconds remaining during which the fragment can be reassembled
} IPV6_RX_FRAGMENT_BUFFER;


// *****************************************************************************
/* 
  Structure:
    IPV6_FRAGMENT_HEADER

  Summary:
    Data structure for IPv6 fragment header.

  Description:
    The Fragment header is used by an IPv6 source to send a packet larger
    than would fit in the path MTU to its destination.
    The Fragment header is identified by a Next Header value of 44 in the
    immediately preceding header.

  Remarks:
    None.
 */
typedef struct __attribute__((__packed__))
{
    uint8_t nextHeader;
    uint8_t reserved;

    union
    {
        struct __attribute__((__packed__))
        {
            unsigned m : 1;
            unsigned reserved2 : 2;
            unsigned fragmentOffset : 13;
        }
        bits;
        uint16_t w;
    } offsetM;
    uint32_t identification;
} IPV6_FRAGMENT_HEADER;

#define IPV6_DATA_NONE                (0x0u)        // The data segment is unused
#define IPV6_DATA_DYNAMIC_BUFFER      (0x1u)        // Data to transmit is allocated in dynamically allocated RAM
#define IPV6_DATA_NETWORK_FIFO        (0x2u)        // Data to transmit is stored in the Network Controller's FIFOs
#define IPV6_DATA_PIC_RAM             (0x3u)        // Data to transmit is stored in PIC RAM

#define IPV6_NO_UPPER_LAYER_CHECKSUM  (0xFFFFu)     // Value flag for no upper layer checksum


// *****************************************************************************
/* 
  Structure:
    IPV6_HEADER
    
  Summary:
    IPv6 packet header definition.

  Description:
    IPv6 packet header definition.

  Remarks:
    Any extension headers present are considered part of the payload length.
 */
typedef struct
{
    unsigned long V_T_F; // Version , Traffic class and Flow Label 
    unsigned short PayloadLength; // Length of IPv6 payload, i.e. the rest of packet following this IPv6 header in octets 
    unsigned char NextHeader; // Identifies the type of header immediately following IPv6 header
    unsigned char HopLimit; // Decremented by 1 by each node that forwards the packet
    IPV6_ADDR SourceAddress; // 128 bit address of originator
    IPV6_ADDR DestAddress; // 128 bit address of intended recipient
} IPV6_HEADER;


// *****************************************************************************
/* IPv6 Differentiated Services Code Point (DSCP)

  Summary:
    Enumeration describing the DSCP Class Selector values for an IPv6 packet

  Description:
    This is a enumeration pf the possible values for the DSCP Class Selector values of an IPv6 packet
    The DSCP field is part of the 'Traffic Class' header member.
    It is a 6 bit value.

  Remarks:
    The DSCP specifies a mechanism for classifying and managing network traffic and providing quality of service
    in an IPv6 packet.

    There is backward compatibility with the IPv4 TOS values.
    A DSCP value divided by 8 will result in an IPv4 TOS value.

    Default value is 0, i.e. standard service class, default forwarding (DF).

    Currently only 6 bit values are supported!

    The Class Selector values are just some predefined/standard values.
    Full range of 6 bit values is supported.

 */

typedef enum __attribute__ ((__packed__))
{
    TCPIP_IPV6_DSCP_CS0     = 0,
    TCPIP_IPV6_DSCP_CS1     = 8,
    TCPIP_IPV6_DSCP_CS2     = 16,
    TCPIP_IPV6_DSCP_CS3     = 24,
    TCPIP_IPV6_DSCP_CS4     = 32,
    TCPIP_IPV6_DSCP_CS5     = 40,
    TCPIP_IPV6_DSCP_CS6     = 48,
    TCPIP_IPV6_DSCP_CS7     = 56,

    //
    TCPIP_IPV6_DSCP_MAX    = ((1U << 6) - 1U),    // maximum value, 6 bits 
}TCPIP_IPV6_DSCP_CS;




// *****************************************************************************
/* 
  Enumeration:
    IPV6_EVENT_TYPE 

  Summary:
    This enumeration is used to notify IPv6 client applications.

  Description:
    These events are used while notifying to the registered applications.

  Remarks:
    None.
 */
typedef enum
{
    IPV6_EVENT_ADDRESS_ADDED = 1,  // Event is generated when IPv6 added to the list
    IPV6_EVENT_ADDRESS_REMOVED, // Event is generated when IPv6 address removed from the list
    //
    IPV6_EVENT_ULA_ADDRESS_GENERATED, // Event is generated when IPv6 unique local address included to the list
    IPV6_EVENT_ULA_ADDRESS_FAILED, // Event is generated when IPv6 unique local address is failed  

} IPV6_EVENT_TYPE;

// *****************************************************************************
/* Type:
    IPV6_HANDLE

  Summary:
    Notification handler that can be called for successful address addition and removed.

  Description:
    The format of a notification handler registered with the IPv6 module.
    Once an IPV6_EVENT_TYPE occurs the IPv6 module will be called for the registered handler.
    
  Remarks:
    If pNetIf == 0, the notification is called for events on any interface.
*/
typedef const void * IPV6_HANDLE; // Pointer to IPv6 object

// *****************************************************************************
/* 
  Type:
    IPV6_RIID_PR_FNC

  Summary:
    Optional Pseudo Random function to be used for the IPv6 address random interface Identifier (RID) generation.

  Description:
    RFC 7217 defines a way to generate a RID using a Pseudo Random Function (PRF).
    This type describes the PRF that can be optionally supplied to generate the RID.

  Parameters:
    hNet - Handle identifying the network interface for which this PRF is calculated
    ip6Addr - address to be updated with the calculated RID.
              On input the address should contain the prefix (1st prefixLen bits)
              On output it needs to be updated with the generated RID in the least significant 64 bits.
    prefixLen - the length of the prefix, as described in RFC7217 Section 5.
    dadCounter - the DAD counter, as described in RFC7217 Section 5.

  Returns:
    true    - if the calculation was successful and the ip6Addr has been updated
    false   - otherwise. Some failure has occurred.

  Remarks:
    If this function is provided, it has to compute a stable Random Interface ID.
    as described in RFC7217 Section 5.
    Its output has to be at least 64 bits long!
    It should update the Interface Identifier part of ip6Addr according to the RFC7217!

    Could be NULL and a default internal SHA1 function will be used. 

    The prefixLen parameter should normally be 64.
    But it can be a different value - for example 10 when using an LL address.
    However, the length of the Interface Identifier field will be 64 bits in this implementation!
    So, the relation: prefix length + Interface Identifier length = 128 bits is NOT enforced!

    Additional notes from RFC 7217:
        - The pseudorandom function (PRF) MUST NOT be computable from the outside (without knowledge of the secret key).
        -  PRF() MUST also be difficult to reverse, such that it resists attempts to obtain the secret_key,
           even when given samples of the output of PRF() and knowledge or control of the other input parameters.
        - PRF() SHOULD produce an output of at least 64 bits.
        - PRF() could be implemented as a cryptographic hash of the concatenation of each of the function parameters.
            SHA-1 [FIPS-SHS] and SHA-256 are two possible options for PRF().
            MD5 [RFC1321] is considered UNACCEPTABLE for PRF() [RFC6151]
 */
typedef bool (*IPV6_RIID_PR_FNC)(TCPIP_NET_HANDLE hNet, IPV6_ADDR* ip6Addr, uint8_t prefixLen, uint8_t dadCounter); 

// *****************************************************************************
/* 
  Type:
    IPV6_RIID_NET_IFACE_FNC

  Summary:
    Net_Iface function to be used for random interface ID generation.

  Description:
    RFC 7217 defines an Net_Iface parameter to be used for random interface ID generation.
    This is an implementation-dependent stable identifier associated with the network interface
    for which the RID is being generated.

  Parameters:
    hNet - Handle identifying the network interface for which the Net_Iface is calculated
    pNetIface - address to store a pointer to the calculated identifier.
    
  Returns:
    The size of the Net_Iface to use, in bytes.

  Remarks:
    As recommended by the standard, usual options are:
    - The Interface Index
    - The Interface Name
    - The Interface Link-layer address
    - Logical Network Service Identity

    If the function returns 0, then the Net_Iface parameter in the RID generation will be skipped.

    Could be NULL and a default internal implementation using the interface Link-layer address will be used.

 */
typedef size_t (*IPV6_RIID_NET_IFACE_FNC)(TCPIP_NET_HANDLE hNet, const uint8_t** pNetIface);

// *****************************************************************************
/* 
  Type:
    IPV6_RIID_NET_ID_FNC

  Summary:
    Optional Network_ID get function to be used for random interface ID generation.

  Description:
    RFC 7217 defines an optional Network_ID parameter to be used for random interface ID generation.
    The Network_ID should be any network-specific data that identifies the subnet to which the interface is attached,
    for example, the IEEE 802.11 Service Set Identifier (SSID).

  Parameters:
    hNet - Handle identifying the network interface for which the Network_ID is calculated
    pNetworkID - address to store a pointer to the calculated Network_ID.
    
  Returns:
    The size of the Network_ID to use, in bytes.


  Remarks:
    If the function is NULL or returns 0, then the Network_ID in the RID generation will be skipped.

 */
typedef size_t (*IPV6_RIID_NET_ID_FNC)(TCPIP_NET_HANDLE hNet, const uint8_t** pNetworkID);

// *****************************************************************************
/* 
  Type:
    IPV6_RIID_SECRET_KEY_FNC

  Summary:
    Secret_key to be used for random interface ID generation.

  Description:
    RFC 7217 defines the mandatory Secret_key parameter to be used for random interface ID generation.

  Parameters:
    hNet - Handle identifying the network interface for which the Secret_key is calculated
    pSecretKey - address to store a pointer to the calculated Secret_key.
    
  Returns:
    The size of the Secret_key to use, in bytes.
    It should be at least 16 bytes (128 bits) as required by the standard!


  Remarks:
    If a custom IPV6_RIID_PR_FNC is not provided, then this function CANNOT be NULL!
   
    If the function returns a value < 16 bytes, then the RID generation will fail!

    Additional notes from the RFC 7217:
        - The function should generate a secret key that is not known by the attacker.
        - The secret key SHOULD be of at least 128 bits.
        - It MUST be initialized to a pseudo-random number (see [RFC4086] for randomness requirements for security)
          when the operating system is installed or when the IPv6 protocol stack is "bootstrapped" for the first time.
        - An implementation MAY provide the means for the system administrator to display and change the secret key.

 */
typedef size_t (*IPV6_RIID_SECRET_KEY_FNC)(TCPIP_NET_HANDLE hNet, const uint8_t** pSecretKey);


// *****************************************************************************
/* 
  Enumeration:
    TCPIP_IPV6_CONFIG_FLAGS 

  Summary:
    This enumeration is used to initialize the IPv6 module

  Description:
    Flags for the IPv6 module configuration and initialization

  Remarks:
    16 bits supported only
 */
typedef enum
{
     TCPIP_IPV6_FLAG_NONE                      = 0,
     TCPIP_IPV6_FLAG_RANDOM_INTERFACE_ID       = 0x0001,    // generate a random interface ID in IP addresses
                                                            // TCPIP_IPV6_RANDOM_INTERFACE_ID_ENABLE shoud be defined and != 0
     
     // More flags will be eventually added

} TCPIP_IPV6_CONFIG_FLAGS;

// *****************************************************************************
/* 
  Structure:
    TCPIP_IPV6_MODULE_CONFIG

  Summary:
    Provides a place holder for IPv6 configuration.

  Description:
    IPv6 module runtime configuration and initialization parameters.

  Remarks:
    None.
 */
typedef struct
{
    uint16_t        rxfragmentBufSize;      // RX fragmented buffer size
    uint16_t        fragmentPktRxTimeout;   // fragmented packet time out value
    // members used only if TCPIP_IPV6_RANDOM_INTERFACE_ID_ENABLE != 0 ! 
    IPV6_RIID_PR_FNC            pRiidFnc;   // Random Interface ID generation function pointer
                                            // See the notes from the IPV6_RIID_PR_FNC definition.
                                            // Could be NULL and a default SHA1 function will be used. 
    IPV6_RIID_NET_IFACE_FNC     pNetIfaceFnc;   // Net_Iface generation function pointer
                                            // See the notes from the IPV6_RIID_NET_IFACE_FNC definition.
                                            // Could be NULL and a default interface Link-layer address will be used. 
        
    IPV6_RIID_NET_ID_FNC        pNetworkIdFnc;  // Optional network ID get function pointer
                                            // Could be NULL and the Network_ID parameter will be skipped
                                            // See the notes from the IPV6_RIID_NET_ID_FNC definition.
                                            //
    IPV6_RIID_SECRET_KEY_FNC    pSecretKeyFnc;  // Secret key function to be used in the feneration of the RID
                                            // This cannot be NULL if pRiidFnc == NULL!
                                            // See the notes from the IPV6_RIID_SECRET_KEY_FNC definition.
                                            //
    // end of TCPIP_IPV6_RANDOM_INTERFACE_ID_ENABLE != 0 ! members

    TCPIP_IPV6_CONFIG_FLAGS     configFlags;    // configuration flags
}TCPIP_IPV6_MODULE_CONFIG;

// *****************************************************************************
/* 
  Type:
    IPV6_EVENT_HANDLER

  Summary:
    Clients can register a handler with the IPv6 service.

  Description:
    Once an IPv6 event occurs the IPv6 service will call the registered handler
    The handler has to be short and fast.
    It is meant for setting an event flag, not for lengthy processing!

    evParam is a parameter that's associated to an IPv6 event
    - For an address event (IPV6_EVENT_ADDRESS_ADDED, IPV6_EVENT_ADDRESS_REMOVED) it should typecast
      to (const IPV6_ADDR_STRUCT*)
    - For an IPV6_EVENT_ULA_ADDRESS_GENERATED ULA event it should typecast
      to (const IPV6_ADDR*)
    - For an IPV6_EVENT_ULA_ADDRESS_FAILED ULA event the evParam is an IPV6_ULA_RESULT error code

    The evParam is invalid outside of the IPV6_EVENT_HANDLER context call and should not be stored
    by the caller. Info that's needed has to be copied into caller's own context.

    usrParam is a user-supplied parameter

  Remarks:
    For address related events the passed (const IPV6_ADDR_STRUCT*) parameter
    is invalid after the notification call.

 */
typedef void (*IPV6_EVENT_HANDLER)(TCPIP_NET_HANDLE hNet, IPV6_EVENT_TYPE evType, const void* evParam, const void* usrParam);


// *****************************************************************************
/* 
  Type:
    IPV6_PACKET_ACK_FNC
    
  Summary:
    Packet allocation and deallocation acknowledgment callback function.

  Description:
    Packet allocation and deallocation acknowledgment callback function.
     
  Parameters:
    IPV6_PACKET * - Pointer to the packet that was transmitted
    res - True if the packet was sent, false otherwise
    param - callback parameter
    
  Returns:
    None

  Remarks:
    None.
*/
typedef void (*IPV6_PACKET_ACK_FNC)(void* pkt, bool success, const void * param);

//*****************************************************************************
/* 
  Structure:
    IPV6_PACKET
    
  Summary:
    Packet structure/state tracking for IPv6 packets.

  Description:
    IPv6 packets are queued for future transmission.queuedPacketTimeout is used to 
    time-out IPv6 queued packets.
     
  Remarks:
    For IPv6 queuing the time out has to be 0!
    The queue is processed separately by the NDP.IPV6_PACKET_ACK_FNC is called after the 
    successful removal and inclusion of the packet.
*/
typedef struct S_IPV6_PACKET
{
    struct S_IPV6_PACKET * next;                 // Next packet in a queue
    unsigned short payloadLen;                  // Amount of data in payload buffer
    unsigned short headerLen;                   // Total header length (IP header + IPv6 Extension headers)
    unsigned short upperLayerHeaderLen;         // Total length of the upper layer header
    unsigned short upperLayerChecksumOffset;    // Offset of the upper layer checksum
    unsigned char upperLayerHeaderType;         // Type definition for the upper-layer header type
    union
    {
        struct __attribute__((packed))
        {
            unsigned reserved :         3;
            unsigned useUnspecAddr :    1; // This packet should use the unspecified address
            unsigned sourceSpecified :  1; // The upper layer or application layer specified a source address
            unsigned queued :           1; // Packet has been queued
            unsigned addressType :      2; // IP_ADDRESS_TYPE_IPV6 or IP_ADDRESS_TYPE_IPV4
        };
        uint8_t val;
    } flags;
    TCPIP_MAC_ADDR remoteMACAddr;               // The packet's remote MAC address
    IPV6_PACKET_ACK_FNC ack6Fnc;                // function to be called when IPv6 done with the packet
    TCPIP_MAC_PACKET_ACK_FUNC macAckFnc;        // function to be called when MAC done with a TX packet
    void* ack6Param;                            // parameter to be used
    void* clientData;                           // optional packet client data
    void * neighbor;                            // The neighbor that the message was received from    
    unsigned short offsetInSegment;             // Offset used for storing fragment transmission information
    uint32_t queuedPacketTimeout;               // Time out for IPv6 packets which are queued
    TCPIP_NET_HANDLE netIfH;                    // packet network interface
    IPV6_HEADER ipv6Header;                     // IPv6 header definition
    IPV6_DATA_SEGMENT_HEADER payload;           // IPv6 data segment payload
} IPV6_PACKET;

// *****************************************************************************
/* 
  Enumeration:
    IPV6_ULA_FLAGS 

  Summary:
    Provides a list of possible ULA action flags.

  Description:
    This enumeration provides a list of possible flags for the Unique Local Address 
    (ULA) generation.

  Remarks:
    8 bit values only.
 */
typedef enum
{
    IPV6_ULA_FLAG_NTPV4         = 0x01,     // use an IPv4 NTP server access in Unique Local Address generation
                                            // default is an IPv6 server
    IPV6_ULA_FLAG_GENERATE_ONLY = 0x02,     // generate an address only, don't add it to the interface addresses
    IPV6_ULA_FLAG_SKIP_DAD      = 0x04,     // when adding the address to the interface, skip the Duplicate Address Detection

} IPV6_ULA_FLAGS;

// *****************************************************************************
/* 
  Enumeration:
    IPV6_ULA_RESULT

  Summary:
    Provides a list of possible ULA results.

  Description:
    This enumeration provides a list of possible results for the Unique Local Address 
    (ULA) generation.

  Remarks:
    None.
 */
typedef enum
{
    // success codes
    IPV6_ULA_RES_OK,                // the address generation was started successfully

    // error codes
    IPV6_ULA_RES_BUSY               = -1,   // address generation module is busy
    IPV6_ULA_RES_IF_ERR             = -2,   // interface non existent
    IPV6_ULA_RES_NTP_ACCESS_ERR     = -3,   // NTP module could not be accessed
    IPV6_ULA_RES_NTP_TSTAMP_ERR     = -4,   // wrong NTP time stamp received
    IPV6_ULA_RES_NOT_SUPPORTED      = -5,   // wrong call/unsupported

} IPV6_ULA_RESULT;

// *****************************************************************************
/* 
  Enumeration:
    TCPIP_IPV6_RESULT

  Summary:
    Provides a list of possible generic IPv6 results.

  Description:
    This enumeration provides a list of possible results for the IPv6 API

  Remarks:
    None.
 */
typedef enum
{
    // success codes
    TCPIP_IPV6_RES_OK               = 0,    // the IPv6 operation was completed successfuly

    // error codes
    TCPIP_IPV6_RES_BAD_ARG          = -1,   // a bad argument was provided
    TCPIP_IPV6_RES_IF_ERR           = -2,   // interface non existent
    TCPIP_IPV6_RES_MCAST_ERR        = -3,   // a multicast address was provided
    TCPIP_IPV6_RES_ALLOC_ERR        = -4,   // memory allocation failed
    TCPIP_IPV6_RES_ADDRESS_ERR      = -5,   // no such address found, address error
    TCPIP_IPV6_RES_BAD_IF           = -6,   // bad interface selected for the operation
    TCPIP_IPV6_RES_OPER_ERR         = -7,   // operation error, not supported

    // other error codes will be added
    TCPIP_IPV6_RES_INTERNAL_ERR     = -20,   // internal/unexpected error; should not happen

} TCPIP_IPV6_RESULT;

// *****************************************************************************
/* 
  Enumeration:
    TCPIP_IPV6_NEIGHBOR_FLAGS

  Summary:
    Provides a list of possible generic IPv6 neighbor flags.

  Description:
    This enumeration provides a list of possible flags for the IPv6 neighbor API

  Remarks:
    8 bit values only.
 */
typedef enum
{
    // success codes
    TCPIP_IPV6_NEIGHBOR_FLAG_NONE       = 0,        // none, default
    TCPIP_IPV6_NEIGHBOR_FLAG_PERM       = 0x01,     // a permanent address is provided, no need to be validated
                                                    // otherwise address is a temporary one, needs to be validated (DAD, etc.)
    TCPIP_IPV6_NEIGHBOR_FLAG_ROUTER     = 0x02,     // the added neighbor is a router
                                                    // otherwise a regular host (default)

    // other flags will be eventually added

} TCPIP_IPV6_NEIGHBOR_FLAGS;


// *****************************************************************************
/*
  Type:
    TCPIP_IPV6_PROCESS_HANDLE

  Summary:
    Defines a IPv6 packet processing handle.

  Description:
    Definition of an packet processing handle used for
    packet processing registration by the IPv6 clients.

*/
typedef const void* TCPIP_IPV6_PROCESS_HANDLE;

// *****************************************************************************
/* IPv6 packet handler Pointer

  Function:
    bool <FunctionName> (TCPIP_NET_HANDLE hNet, TCPIP_MAC_PACKET* rxPkt, const void* hParam);

  Summary:
    Pointer to a function(handler) that will get called to process an incoming IPv6 packet.

  Description:
    Pointer to a function that will be called by the IPv6 module
    when a RX packet is available.

  Precondition:
    None

  Parameters:
    hNet        - network handle on which the packet has arrived
    rxPkt       - pointer to incoming packet
    hParam      - user passed parameter when handler was registered

  Returns:
    true - if the packet is processed by the external handler.
           In this case the IPv6 module will no longer process the packet
    false - the packet needs to be processed internally by the IPv6 as usual           

  Remarks:
    The packet handler is called in the IPv6 context.
    The handler should be kept as short as possible as it affects the processing of all the other
    IPv6 RX traffic.

    Before calling the external packet handler 
    - the rxPkt->pktIf is updated
    - the rxPkt->pNetLayer points to an IPV6_HEADER data structure.

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
typedef bool(*TCPIP_IPV6_PACKET_HANDLER)(TCPIP_NET_HANDLE hNet, TCPIP_MAC_PACKET* rxPkt, const void* hParam);

// *****************************************************************************
// *****************************************************************************
// Section: IPv6 Routines
// *****************************************************************************
// *****************************************************************************

//*****************************************************************************
/*
  Function:
    bool TCPIP_IPV6_InterfaceIsReady (TCPIP_NET_HANDLE netH)

  Summary:
    Determines if an interface is ready for IPv6 transactions.

  Description:
    Returns the current state of the IPv6 interface (i.e., determines 
    if an interface is ready for IPv6 transactions).

  Precondition:
    None.

  Parameters:
    netH - The interface to check

  Returns:
    - true  - if the interface has IPv6 functionality available
    - false - if the interface does not have IPv6 functionality available

  Remarks:
    None.
  */
bool TCPIP_IPV6_InterfaceIsReady(TCPIP_NET_HANDLE netH);


//*****************************************************************************
/*
  Function:
    IPV6_ADDR_STRUCT * TCPIP_IPV6_DASSourceAddressSelect (TCPIP_NET_HANDLE hNetIf, 
                                   const IPV6_ADDR * dest, const IPV6_ADDR * requestedSource)

  Summary:
    Determines the appropriate source address for a given destination
    address.

  Description:
    The IPv6 policy table is used to select the destination address. The destination 
    address selection algorithm takes a list of IPv6 addresses (gPolicyTable) and 
    sorts the linked list. There are eight sorting rules. Starting with the last rule 
    and working to the most important, using a stable sorting algorithm, will produce 
    a sorted list most  efficiently.  The best average run time we'll get with a stable 
    sort with O(1) memory usage is O(n^2), so we'll use an insertion sort.  This will 
    usually be most efficient for small lists (which should be the typical case).
    If a rule determines a result, then the remaining rules are not relevant and should be
    ignored.

  Precondition:
    TCPIP_IPV6_Initialize is required.
    TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    hNetIf - The given interface
    dest - The destination address
    requestedSource - A specified source

  Returns:
    - On Success - Pointer to the selected source address
    - On Failure - NULL
    
  Remarks:
    None.
  */
IPV6_ADDR_STRUCT * TCPIP_IPV6_DASSourceAddressSelect(TCPIP_NET_HANDLE hNetIf, 
                       const IPV6_ADDR * dest, const IPV6_ADDR * requestedSource);


//*****************************************************************************
/*
  Function:
    IPV6_PACKET * TCPIP_IPV6_TxPacketAllocate (TCPIP_NET_HANDLE netH, 
                                 IPV6_PACKET_ACK_FNC ack6Fnc, void* ack6Param);


  Summary:
    Dynamically allocates a packet for transmitting IP protocol data.

  Description:
    This function dynamically allocates a packet for transmitting IP protocol data and 
    sets the packet IPv6 protocol for a TX packet.

  Precondition:
    TCPIP_IPV6_Initialize is required.
    TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    netH        - Interface of the outgoing packet.
    ack6Fnc     - function to be called when IP is done with the TX packet
                  (finished transmitting)
    ack6Param   - parameter to be used for this callback
                  This has meaning only for the caller of the
                  TCPIP_IPV6_TxPacketAllocate

  Returns:
    - On Success - Pointer to the allocated packet
    - On Failure - NULL

  Remarks:
    None.
 */
IPV6_PACKET * TCPIP_IPV6_TxPacketAllocate(TCPIP_NET_HANDLE netH, IPV6_PACKET_ACK_FNC ack6Fnc, void* ack6Param);


//*****************************************************************************
/*
  Function:
    unsigned short TCPIP_IPV6_TxIsPutReady (IPV6_PACKET * ptrPacket,
        unsigned short count)

  Summary:
    Determines whether a TX packet can be written to.

  Description:
    This function determines whether a TX packet can be written to.  This function 
    will allocate additional space to the packet to accommodate the user.

  Precondition:
    TCPIP_IPV6_Initialize is required.
    TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    ptrPacket - The packet to check
    count - The amount of writable space to check for

  Returns:
    - On Success - The amount of space available
    - On Failure - 0
  
  Remarks:
    None.
 */
unsigned short TCPIP_IPV6_TxIsPutReady(IPV6_PACKET * ptrPacket, unsigned short count);


//*****************************************************************************
/*
  Function:
    void TCPIP_IPV6_PacketFree (IPV6_PACKET * ptrPacket)

  Summary:
    Frees a TCP/IP Packet structure from dynamic memory.

  Description:
    This function frees a TCP/IP Packet structure from dynamic memory.

  Precondition:
    TCPIP_IPV6_Initialize is required.
    TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    ptrPacket - The packet to free.

  Returns:
    None.

  Remarks:
    None.
 */
void TCPIP_IPV6_PacketFree(IPV6_PACKET * ptrPacket);


//*****************************************************************************
/*
  Function:
    bool TCPIP_IPV6_Put (IPV6_PACKET * pkt, unsigned char v)

  Summary:
    Writes a character of data to a packet.

  Description:
    This function writes a character of data to a packet.

  Precondition:
    TCPIP_IPV6_Initialize is required.
    TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    pkt - The packet
    v - The character

  Returns:
    - true  - if the character was written
    - false - if the character was not written

  Remarks:
    None.
 */
bool TCPIP_IPV6_Put(IPV6_PACKET * pkt, unsigned char v);


//*****************************************************************************
/*
  Function:
    unsigned short TCPIP_IPV6_ArrayPutHelper (IPV6_PACKET * ptrPacket, 
             const void * dataSource, uint8_t dataType, unsigned short len)

  Summary:
    Helper function to write data to a packet.

  Description:
    This is a helper function for writing data to a packet.

  Precondition:
    TCPIP_IPV6_Initialize is required.
    TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    ptrPacket - The packet
    dataSource - The address of the data on its medium
    dataType - Descriptor of the data type (dynamic memory on PIC, in a
                    network FIFO, in static PIC RAM)
    len - Length of the data

  Returns:
    unsigned short - The number of bytes of data written.

  Remarks:
    None.
  */
unsigned short TCPIP_IPV6_ArrayPutHelper(IPV6_PACKET * ptrPacket, const void * dataSource, uint8_t dataType, unsigned short len);


//******************************************************************************
/*
  Function:
        unsigned short TCPIP_IPV6_PutArray (IPV6_PACKET * ptrPacket, 
                                  const void * dataSource, unsigned short len)

  Summary:
    Writes data to a packet

  Description:
    This function writes data to an outgoing packet.

  Precondition:
    TCPIP_IPV6_Initialize is required.
    TCPIP_IPV6_InterfaceIsReady should be true.
    The TCPIP_IPV6_TxIsPutReady function must have returned a value greater
    than or equal to 'len'.

  Parameters:
    ptrPacket - The packet
    dataSource - Pointer to the data to copy to the packet
    len - Length of the data

  Returns:
    unsigned short - The number of bytes of data written.

  Remarks:
    None.
 */
#define TCPIP_IPV6_PutArray(pkt,data,len)  TCPIP_IPV6_ArrayPutHelper(pkt, data, IPV6_DATA_PIC_RAM, len)


//*****************************************************************************
/*
  Function:
    unsigned short TCPIP_IPV6_PayloadSet (IPV6_PACKET * ptrPacket, uint8_t* payload, 
                                          unsigned short len)

  Summary:
    Allocates a segment on the end of a packet segment chain and uses it to
    address prebuffered data.

  Description:
    This function will allocate a data segment header and append it to the
    end of a chain of segments in a TX packet.  It will set the data pointer in
    the packet segment to a pre-existing buffer of data.

  Precondition:
    TCPIP_IPV6_Initialize should be called.
    TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    ptrPacket - The packet
    payload - Address of the data payload
    len - Length of the data payload

  Returns:
    - On Success - The amount of data added to the packet length
    - On Failure - 0

  Remarks:
    This function is useful for adding payloads to outgoing packets without
    copying them if the data is in another pre-existing buffer (i.e., TCP).
*/
unsigned short TCPIP_IPV6_PayloadSet(IPV6_PACKET * ptrPacket, uint8_t* payload, unsigned short len);


//*****************************************************************************
/*
  Function:
    int TCPIP_IPV6_Flush (IPV6_PACKET * ptrPacket)

  Summary:
    Flushes a IP TX packet.

  Description:
    This function flushes a IP TX packet. Determines the link-layer address, 
    if necessary and calculates the upper-layer checksum, if necessary.

  Precondition:
    TCPIP_IPV6_Initialize should be called.
    TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    ptrPacket - The packet to flush

  Returns:
    - 1   - if the packet has been transmitted
    - 0   - if the packet has been queued
    - <0 - if the packet has been discarded for some error

  Remarks:
    None.
*/
int TCPIP_IPV6_Flush(IPV6_PACKET * ptrPacket);


//******************************************************************************
/*
  Function:
    uint8_t TCPIP_IPV6_Get (TCPIP_MAC_PACKET* pRxPkt, uint8_t* pData)

  Summary:
    Reads the next byte of data from the specified MAC.

  Description:
    This function is used to read the next byte of data from a packet.

  Precondition:
    TCPIP_IPV6_Initialize should be called.
    TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    pRxPkt - The MAC RX packet to read data from
    pData -  Byte data to be read

  Returns:
    - 1 - On Successful read
    - 0 - No byte is available to read


  Remarks:
    None.
 */
uint8_t TCPIP_IPV6_Get (TCPIP_MAC_PACKET* pRxPkt, uint8_t* pData);


//******************************************************************************
/*
  Function:
    uint8_t TCPIP_IPV6_ArrayGet (TCPIP_MAC_PACKET* pRxPkt, uint8_t *val,
        uint16_t len);

  Summary:
    Reads the next byte of data from the specified MAC.

  Description:
    Reads a character of data from a packet.

  Precondition:
    TCPIP_IPV6_Initialize is required.
    TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    pRxPkt - The MAC RX packet to read data from
    val - The buffer to store the data
    len - The amount of data to read

  Returns:
    - > 0 - The number of bytes read
    - 0 - No byte is available to read

  Remarks:
    None
 */
uint8_t TCPIP_IPV6_ArrayGet (TCPIP_MAC_PACKET* pRxPkt, uint8_t *val, uint16_t len);


//*****************************************************************************
/*
  Function:
    void  TCPIP_IPV6_DestAddressSet(IPV6_PACKET * p, const IPV6_ADDR * addr)

  Summary:
    Sets the destination address for a IPv6 packet.

  Description:
    This API is used to configure the destination address for a IPv6 packet.

  Precondition:
    TCPIP_IPV6_Initialize should be called.
    TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    p - pointer to IPv6 packet
    addr - destination address

  Returns:
    None.

  Remarks:
    None.
 */
void  TCPIP_IPV6_DestAddressSet(IPV6_PACKET * p, const IPV6_ADDR * addr);


//*****************************************************************************
/*
  Function:
    IPV6_ADDR *  TCPIP_IPV6_DestAddressGet(IPV6_PACKET * p)

  Summary:
    Gets the destination address for a IPv6 packet.

  Description:
    This function is used to get the destination address for a IPv6 packet.

  Precondition:
    TCPIP_IPV6_Initialize should be called.
    TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    p - pointer to IPv6 packet

  Returns:
    IPV6_ADDR * 
    - On Success - Get a valid IPv6 Destination address
    - On Failure - NULL
  Remarks:
    None.
*/
IPV6_ADDR *  TCPIP_IPV6_DestAddressGet(IPV6_PACKET * p);


//*****************************************************************************
/*
  Function:
    void  TCPIP_IPV6_SourceAddressSet(IPV6_PACKET * p, const IPV6_ADDR * addr)

  Summary:
    Sets the source address for a IPv6 packet.

  Description:
    This function is used to configure the source address for a IPv6 packet.

  Precondition:
    TCPIP_IPV6_Initialize should be called.
    TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    p - pointer to IPv6 packet
    addr - source address

  Returns:
    None.

  Remarks:
    None.
 */
void  TCPIP_IPV6_SourceAddressSet(IPV6_PACKET * p, const IPV6_ADDR * addr);


//*****************************************************************************
/*
  Function:
    IPV6_ADDR *  TCPIP_IPV6_SourceAddressGet(IPV6_PACKET * p)

  Summary:
    Gets the source address for an IPv6 packet.

  Description:
    This API is used to get the source address for an IPv6 packet.

  Precondition:
    TCPIP_IPV6_Initialize should be called.
    TCPIP_IPV6_InterfaceIsReady should be true.
    
  Parameters:
    p - pointer to IPv6 packet

  Returns:
    IPV6_ADDR *
    - On Success - Get a valid IPv6 Source address
    - On Failure - NULL

  Remarks:
    None.
*/
IPV6_ADDR *  TCPIP_IPV6_SourceAddressGet(IPV6_PACKET * p);


//*****************************************************************************
/*
  Function:
    IPV6_ADDR_STRUCT * TCPIP_IPV6_UnicastAddressAdd (TCPIP_NET_HANDLE netH, 
                const IPV6_ADDR * address, uint8_t prefixLen, uint8_t skipProcessing)

  Summary:
    Adds a unicast address to a specified interface

  Description:
    Adds a unicast address to a specified interface.  Starts duplicate address
    detection if necessary.

  Precondition:
    TCPIP_IPV6_Initialize is required.
    TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    netH            - The interface to which the the address is to be addded
    address         - The address to add
    prefixLen       - The prefix length associated to this static address
                      (providing the subnet prefix length). If 0, the default value 
                      of 64 will be used
    skipProcessing  - true to skip Duplicate address detection; otherwise, false

  Returns:
    IPV6_ADDR_STRUCT * 
    - On Success - Pointer to the structure of the newly allocated address
    - On Failure - NULL

  Remarks:
    The RFC (4291) requires the interface ID for all unicast addresses,
    (except those that start with the binary value 000)
    to be 64 bits long and to be constructed in Modified EUI-64 format.
    Therefore the prefixLen parameter should probably always be 64.

*/
IPV6_ADDR_STRUCT * TCPIP_IPV6_UnicastAddressAdd(TCPIP_NET_HANDLE netH, 
            const IPV6_ADDR * address, uint8_t prefixLen, uint8_t skipProcessing);


//*****************************************************************************
/*
  Function:
    void TCPIP_IPV6_AddressUnicastRemove(TCPIP_NET_HANDLE netH, const IPV6_ADDR * address)

  Summary:
    Removed a configured unicast address from an interface.

  Description:
    This function is used to remove a configured unicast address from an interface.

  Precondition:
    TCPIP_IPV6_Initialize should be called.
    TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    netH    - The interface to remove the address
    address - The address

  Returns:
    None.

  Remarks:
    None.
*/
void TCPIP_IPV6_AddressUnicastRemove(TCPIP_NET_HANDLE netH, const IPV6_ADDR * address);


//*****************************************************************************
/*
  Function:
    IPV6_ADDR_STRUCT * TCPIP_IPV6_MulticastListenerAdd(TCPIP_NET_HANDLE hNet, const IPV6_ADDR * address)

  Summary:
    Adds a multicast listener to an interface.

  Description:
    This function is used to add the IPv6 multicast address to an interface.

  Precondition:
    TCPIP_IPV6_Initialize should be called.
    TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    hNet - The interface to add the address
    address - The new listener

  Returns:
    IPV6_ADDR_STRUCT * 
    - On Success - Pointer to the new listener
    - On Failure - NULL

  Remarks:
    None.
*/
IPV6_ADDR_STRUCT * TCPIP_IPV6_MulticastListenerAdd(TCPIP_NET_HANDLE hNet, const IPV6_ADDR * address);


//*****************************************************************************
/*
  Function:
    void TCPIP_IPV6_MulticastListenerRemove (TCPIP_NET_HANDLE netH, const IPV6_ADDR * address)

  Summary:
    Removes a multicast listener from a given interface.

  Description:
    This function removes a multicast listener from a given interface.

  Precondition:
    TCPIP_IPV6_Initialize should be called.
    TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    netH  - The interface
    address - The address

  Returns:
    None.

  Remarks:
    None.
*/
void TCPIP_IPV6_MulticastListenerRemove(TCPIP_NET_HANDLE netH, const IPV6_ADDR * address);


//****************************************************************************
/*
  Function:
    IPV6_ULA_RESULT TCPIP_IPV6_UniqueLocalUnicastAddressAdd (TCPIP_NET_HANDLE netH, 
          uint16_t subnetID, IPV6_ULA_FLAGS genFlags, IP_MULTI_ADDRESS* ntpAddress)
    
  Summary:
    Adds a Unique Local Unicast Address (ULA) to a specified interface.
    
  Description:
    This function starts the process of adding an ULA address to the
    specified interface. The segments of the generated address are as
    follows:
    - FC00::/7     - ULA prefix
    - L            - 1 bit set to 1, locally assigned
    - Global ID    - 40 bit random generated identifier
    - subnet ID    - 16 bit subnet identifier
    - Interface ID - 64 bit interface identifier generated as a EUI64 from the 
                     specified interface MAC
    
    The randomness of the "Global ID" prefix of the generated IPv6 address
    is obtained by using an NTP server. The supplied NTP server will be
    contacted to obtain an NTP time stamp. This time stamp together with the
    EUI64 identifier obtained from the interface MAC are passed through a
    160 bits hash algorithm (SHA1) and the least significant 40 bits are
    used as the GlobalID of the interface.
    
  Preconditions:
    The IPv6 stack is initialized and the interface is up and configured.
    
  Input:
    netH -        The interface to add the address to.
    subnetID -    The subnet ID to be used.
    genFlags -    Address generation flags:
                  * IPV6_ULA_FLAG_NTPV4 - if set, the NTP server will be contacted over an IPv4 connection.
                    Otherwise, a default IPv6 connection will be attempted
                  * IPV6_ULA_FLAG_GENERATE_ONLY - if set, the address will not 
                    be added to the list of the addresses for the specified interface
                  * IPV6_ULA_FLAG_SKIP_DAD - if set, the DAD processing will be skipped
    ntpAddress -  The NTP server address - it is an IPv4/IPv6 address as
                  selected by the IPV6_ULA_FLAG_NTPV4 flag (the IP address
                  could be obtained with an DNS call into the stack)
                  Not currently used!
  Return:
    - IPV6_ULA_RES_BUSY - address generation module is busy
    - IPV6_ULA_RES_IF_ERR - IPv6 interface is not up
    - IPV6_ULA_RES_OK - if the call succeeded and the generation process was
                        started
  Remarks:
    ULA generation needs to be enabled in the stack at build time.

    Only one address generation at a time is supported for now. Before
    attempting a new address generation the previous operation has to be
    completed, otherwise the call will fail.
    
    This function requires that the NTP client is enabled in the stack. If
    not, the call will fail.
    
    The caller will be notified by the outcome of the operation by the
    stack using the standard IPv6 notification handler (registered by
    TCPIP_IPV6_HandlerRegister call).

    The ntpAddress parameter is not currently used!
    The NTP timestamp will be obtained using the default NTP server address
*/
IPV6_ULA_RESULT    TCPIP_IPV6_UniqueLocalUnicastAddressAdd (TCPIP_NET_HANDLE netH, 
         uint16_t subnetID, IPV6_ULA_FLAGS genFlags, IP_MULTI_ADDRESS* ntpAddress);


//*****************************************************************************
/*
  Function:
    bool    TCPIP_IPV6_RouterAddressAdd(TCPIP_NET_HANDLE netH, const IPV6_ADDR * rAddress, 
                                             unsigned long validTime, int flags);

  Summary:
    Adds a new router address to a specified interface.

  Description:
    This function adds a unicast address as a routing address to a specified interface.

  Precondition:
    rAddress pointer to a valid IPv6 router address
    IPv6 stack initialized, interface up and configured

  Parameters:
    netH            - The interface to which the address is to be added
    rAddress        - The router address to add
    validTime       - The time this valid will be valid, seconds.
                      If 0, the entry will be valid forever.
    flags           - Creation flags (not used for now, should be 0)

  Returns:
    - true  - operation succeeded
    - false - operation failed (interface not valid, interface still configuring, no memory, etc.)

  Remarks:
    The validTime parameter is relevant for an existent router on the network.
    If such router does not exist the stack will eventually discard the entry automatically.

*/
bool    TCPIP_IPV6_RouterAddressAdd(TCPIP_NET_HANDLE netH, const IPV6_ADDR * rAddress, 
                                    unsigned long validTime, int flags);

//*****************************************************************************
/*
  Function:
    const IPV6_ADDR*    TCPIP_IPV6_DefaultRouterGet(TCPIP_NET_HANDLE hNet);

  Summary:
    Returns the current router address for a specified interface.

  Description:
    This function returns the current router address for a specified interface.

  Precondition:
    The IPv6 stack is initialized and the interface is up and configured.

  Parameters:
    netH            - The interface for which to return the router address.

  Returns:
    - valid IPV6_ADDR pointer  - if the interface exists and a valid router address exists
    - 0 - if the interface does not exist and a valid router address does not exist

  Remarks:
    None.

*/
const IPV6_ADDR*    TCPIP_IPV6_DefaultRouterGet(TCPIP_NET_HANDLE netH);

//*****************************************************************************
/*
  Function:
    void    TCPIP_IPV6_DefaultRouterDelete(TCPIP_NET_HANDLE netH);

  Summary:
    Deletes the current router list for a specified interface

  Description:
    This function deletes all default routers on a given interface

  Precondition:
    The IPv6 stack is initialized and the interface is up and configured.

  Parameters:
    netH            - The interface for which to delete the router address.

  Returns:
    None.

  Remarks:
    None.

*/
void    TCPIP_IPV6_DefaultRouterDelete(TCPIP_NET_HANDLE netH);

//*****************************************************************************
/*
  Function:
    IPV6_HANDLE TCPIP_IPV6_HandlerRegister(TCPIP_NET_HANDLE hNet, 
                              IPV6_EVENT_HANDLER handler, const void* hParam)

  Summary:
    Registers an IPv6 event handler callback function.

  Description:
    This function is used to register a notification handler with the IPv6 module.

  Precondition:
    The IPv6 stack is initialized and the interface is up and configured.

  Parameters:
    netH  - Specifies interface to register on.
    handler - Handler to be called for event.
    hParam - The hParam is passed by the client and will be used by the 
            IPv6 when the notification is made. It is used for per-thread 
            content or if more modules, for example, share the same handler
            and need a way to differentiate the callback.

  Returns:
    Handle to registered callback.
    - On Success - Returns a valid handle
    - On Failure - null handle
    
  Remarks:
    None.
 */
IPV6_HANDLE TCPIP_IPV6_HandlerRegister(TCPIP_NET_HANDLE hNet, 
                              IPV6_EVENT_HANDLER handler, const void* hParam);


//*****************************************************************************
/*
  Function:
    bool TCPIP_IPV6_HandlerDeregister(IPV6_HANDLE hIpv6)

  Summary:
    Deregisters an IPv6 event handler callback function.

  Description:
    This function deregisters a previously registered IPv6 event handler callback function.

  Precondition:
    The IPv6 stack is initialized and the interface is up and configured.

  Parameters:
    hIpv6 - Handle to registered callback

  Returns:
    - true  - if deregister successful
    - false - if deregister is not successful

  Remarks:
    None.
  */
bool TCPIP_IPV6_HandlerDeregister(IPV6_HANDLE hIpv6);

// *****************************************************************************
/*
  Function:
    uint16_t TCPIP_IPV6_MaxDatagramDataSizeGet(TCPIP_NET_HANDLE netH);

  Summary:
    Helper to get the maximum datagream data size (MDDS) on a specific interface.
   
  Description:
    The function is a helper to allow retrieving the MDDS on the specified interface.
   
  Precondition:
    IPv6 properly initialized
    Interface properly initialized
        

  Parameters:
    netH      - handle of the interface

  Returns:
    - the maximum datagrem data size that can be transported on the selected interface
    - 0 - invalid interface specified
      
  Remarks:
    None

 */
uint16_t TCPIP_IPV6_MaxDatagramDataSizeGet(TCPIP_NET_HANDLE netH);

// *****************************************************************************
/*
  Function:
    bool TCPIP_IPV6_IsFragmentationEnabled(void);

  Summary:
    Helper to get the IPv6 fragmentation support.
   
  Description:
    The function is a helper to return the IPv6 fragmentation support.
   
  Precondition:
    IPv6 properly initialized

  Parameters:
    None

  Returns:
    - true  - if the fragmentation is enabled and supported by the IPv6 module 
    - false - fragmentation is not supported
      
  Remarks:
    The IPv6 fragmentation support is always enabled.

 */
static __inline__ bool TCPIP_IPV6_IsFragmentationEnabled(void);

static __inline__ bool __attribute__((always_inline)) TCPIP_IPV6_IsFragmentationEnabled(void)
{
    return true;
}

//*******************************************************************************
/*
  Function:
    TCPIP_IPV6_PROCESS_HANDLE    TCPIP_IPV6_PacketHandlerRegister(TCPIP_IPV6_PACKET_HANDLER pktHandler, const void* handlerParam)

  Summary:
    Sets a new packet processing handler.

  Description:
    This function registers a new packet processing handler.
    The caller can use the handler to be notified of incoming packets
    and given a chance to examine/process them.

  Precondition:
    IPv6 properly initialized

  Parameters:
    pktHandler      - the packet handler which will be called for an incoming packet
    handlerParam    - packet handler parameter

  Returns:
    - a valid TCPIP_IPV6_PROCESS_HANDLE - if the operation succeeded
    - NULL - if the operation failed

  Example:
    <code>
    TCPIP_IPV6_PROCESS_HANDLE pktHandle = TCPIP_IPV6_PacketHandlerRegister( myPktHandler, myParam );
    </code>

  Remarks:
    Currently only one packet handler is supported for the IPv6 module.
    The call will fail if a handler is already registered.
    Use TCPIP_IPV6_PacketHandlerDeregister first

    Exists only if TCPIP_IPV6_EXTERN_PACKET_PROCESS is true 

  */
TCPIP_IPV6_PROCESS_HANDLE     TCPIP_IPV6_PacketHandlerRegister(TCPIP_IPV6_PACKET_HANDLER pktHandler, const void* handlerParam);


//*******************************************************************************
/*
  Function:
    bool    TCPIP_IPV6_PacketHandlerDeregister(TCPIP_IPV6_PROCESS_HANDLE pktHandle);

  Summary:
    Deregisters a previously registered packet handler.

  Description:
    This function removes a packet processing handler.

  Precondition:
    IPv6 properly initialized

  Parameters:
    pktHandle   - TCPIP packet handle obtained by a call to TCPIP_IPV6_PacketHandlerRegister


  Returns:
    - true  - if the operation succeeded
    - false - if the operation failed

  Example:
    <code>
    TCPIP_IPV6_PROCESS_HANDLE myHandle = TCPIP_IPV6_PacketHandlerRegister(myPacketHandler, myParam );
    - process incoming packets
    - now we're done with it
    TCPIP_IPV6_PacketHandlerDeregister(myHandle);
    </code>

  Remarks:
    Exists only if TCPIP_IPV6_EXTERN_PACKET_PROCESS is true 

  */
bool    TCPIP_IPV6_PacketHandlerDeregister(TCPIP_IPV6_PROCESS_HANDLE pktHandle);


//*****************************************************************************
/*
  Function:
    TCPIP_IPV6_RESULT TCPIP_IPV6_NeighborAddressAdd(TCPIP_NET_HANDLE netH, const IPV6_ADDR * nAddress, const TCPIP_MAC_ADDR* nMacAddr, TCPIP_IPV6_NEIGHBOR_FLAGS flags);

  Summary:
    Adds a new neighbor address to a specified interface.

  Description:
    This function adds a unicast address as a neighbor/host address to a specified interface.

  Precondition:
    nAddress pointer to a valid IPv6 address
    nMacAddr pointer to a valid MAC address
    IPv6 stack initialized, interface up and configured

  Parameters:
    netH            - The interface to which the address is to be added
                        Must be valid
    nAddress        - The neighbor address to add
                        Must be valid, != NULL, unicast
    nMacAddr        - The neighbor MAC address
                        Must be valid, != NULL
    flags           - neighbor address creation flags
                       currently only TCPIP_IPV6_NEIGHBOR_FLAG_PERM and TCPIP_IPV6_NEIGHBOR_FLAG_ROUTER are used

  Returns:
    - TCPIP_IPV6_RES_OK  - operation succeeded
    - a TCPIP_IPV6_RESULT result < 0 if operation failed

  Remarks:
    This API will add a regular neighbor host/router address
    when the MAC address is known and needs to be specified

    TCPIP_IPV6_RouterAddressAdd could also be used to add a router address.

    By default the created address will be a temporary one, that needs to be validated
    unless the TCPIP_IPV6_NEIGHBOR_FLAG_PERM is specified
    A permanent address will not be validated on the link but it will be used as it is.
    A temporary address will have to be validated before used

    If a router entry is created with this call, the entry will be valid forever!

    API is useful for G3-PLC networks support.
*/
TCPIP_IPV6_RESULT TCPIP_IPV6_NeighborAddressAdd(TCPIP_NET_HANDLE netH, const IPV6_ADDR * nAddress, const TCPIP_MAC_ADDR* nMacAddr, TCPIP_IPV6_NEIGHBOR_FLAGS flags);

//*****************************************************************************
/*
  Function:
    TCPIP_IPV6_RESULT TCPIP_IPV6_NeighborAddressDelete(TCPIP_NET_HANDLE netH, const IPV6_ADDR * nAddress);

  Summary:
    Deletes a neighbor address from the specified interface.

  Description:
    This function deletes a unicast address that was added to the specified interface.

  Precondition:
    nAddress pointer to a valid IPv6 address
    IPv6 stack initialized, interface up and configured

  Parameters:
    netH            - The interface from which the address is to be deleted
    nAddress        - The neighbor/host address to delete

  Returns:
    - TCPIP_IPV6_RES_OK  - operation succeeded
    - a TCPIP_IPV6_RESULT result < 0 if operation failed

  Remarks:
    None
*/
TCPIP_IPV6_RESULT TCPIP_IPV6_NeighborAddressDelete(TCPIP_NET_HANDLE netH, const IPV6_ADDR * nAddress);

//*****************************************************************************
/*
  Function:
    TCPIP_IPV6_RESULT TCPIP_IPV6_G3PLC_PanIdSet(TCPIP_NET_HANDLE netH, uint16_t panId);

  Summary:
    Sets the PANId for a G3-PLC interface

  Description:
    This function will set the PANId to be used in the G3-PLC network for the specified interface.

  Precondition:
    IPv6 stack initialized, interface up and configured
    netH configured as a G3-PLC interface

  Parameters:
    netH            - The interface for which the PANId is to be set
    panId           - the value of the PANId

  Returns:
    - TCPIP_IPV6_RES_OK  - operation succeeded
    - a TCPIP_IPV6_RESULT result < 0 if operation failed

  Remarks:
    None
*/
TCPIP_IPV6_RESULT TCPIP_IPV6_G3PLC_PanIdSet(TCPIP_NET_HANDLE netH, uint16_t panId);

// *****************************************************************************
/* IPv6 packet TX priority handler

  Function:
    uint8_t <FunctionName> (TCPIP_NET_HANDLE hNet, uint8_t dscp);

  Summary:
    Pointer to a function(handler) that will get called to calculate the priority queue of an outgoing IPv6 packet.

  Description:
    Pointer to a function that will be called by the IPv6 module
    when a packet needs to be transmitted.

  Precondition:
    None

  Parameters:
    hNet - network handle on which the packet will be transmitted
    dscp - the packet IPv6 header Differentiated Service Field (dscp) value

  Returns:
    An 8 bit value specifying the MAC priority queue with which the packet is to be transmitted.
    For MACs that support priority queues (GMAC), the packets will be transmitted with different priorities.
    This function allows an IPv6 user to select the desired priority.


  Remarks:
   The IPv6 header IPV6_HEADER::ds value is set using:
        - TCPIP_UDP_OptionsSet(skt, UDP_OPTION_DSCP, ...) for UDP
        - TCPIP_TCP_OptionsSet(skt, TCP_OPTION_DSCP, ...) for TCP

   The number of MAC supported priority queues is available using TCPIP_STACK_MacTxPriGet(hNet)

   The returned value should NOT exceed the maximum MAC supported priority queue - 1!

   Registration of a function to calculate the packet priority is optional.
   By default an internal calculation is performed if no such function is registered.
   The calculation is as follows:
        - Q == the number of MAC supported priority queues
        - M == the maximum IPv6 dscp value == TCPIP_IPV6_DSCP_MAX (63)
        - d == the current value of the IPv6 header ds (dscp)
        - q == the MAC priority queue:
        q = (d x Q) / M - 1;

 */
typedef uint8_t(*TCPIP_IPV6_TX_PRI_HANDLER)(TCPIP_NET_HANDLE hNet, uint8_t dscp);

//*******************************************************************************
/*
  Function:
    bool    TCPIP_IPV6_TxPriHandlerRegister(TCPIP_IPV6_TX_PRI_HANDLER priHandler);

  Summary:
    Registers a packet TX priority queue handler

  Description:
    This function registers a new packet TX priority handler.
    The handler will be called when the IPv6 module needs to transmit a packet 
    and the packet MAC priority queue needs to be calculated.


  Precondition:
    IPv6 properly initialized

  Parameters:
    priHandler      - the packet priority handler which will be called for an outgoing packet

  Returns:
    - true - if the operation succeeded
    - false - if the operation failed
                i.e. another handler is already registered

  Remarks:
    Currently only one packet TX priority handler is supported for the IPv6 module.
    The call will fail if a handler is already registered.
    Use TCPIP_IPV6_TxPriHandlerDeregister first

    The registration of an handler is optional.
    See the TCPIP_IPV6_TX_PRI_HANDLER for the default priority queues calculation.
  */
bool    TCPIP_IPV6_TxPriHandlerRegister(TCPIP_IPV6_TX_PRI_HANDLER priHandler);

//*******************************************************************************
/*
  Function:
    bool    TCPIP_IPV6_TxPriHandlerDeregister(TCPIP_IPV6_TX_PRI_HANDLER priHandler);

  Summary:
    Deregisters a packet TX priority queue handler

  Description:
    This function deregisters a previously registered packet TX priority handler.

  Precondition:
    IPv6 properly initialized

  Parameters:
    priHandler      - the packet priority handler which has been previously registered

  Returns:
    - true - if the operation succeeded
    - false - if the operation failed
                i.e. no such handler is registered

  Remarks:
    Currently only one packet TX priority handler is supported for the IPv6 module.
    TCPIP_IPV6_TxPriHandlerDeregister should be called before registering another handler.

  */
bool    TCPIP_IPV6_TxPriHandlerDeregister(TCPIP_IPV6_TX_PRI_HANDLER priHandler);


// *****************************************************************************
/*
  Function:
    void  TCPIP_IPV6_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs IPv6 module tasks in the TCP/IP stack.

  Precondition:
    The IPv6 stack is initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_IPV6_Task(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // H_IPV6_H_
