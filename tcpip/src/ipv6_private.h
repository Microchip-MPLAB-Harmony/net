/*******************************************************************************
  IPv6 private API for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    ipv6_private.h

  Summary:

  Description:
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

#ifndef _IPV6_PRIVATE_H_
#define _IPV6_PRIVATE_H_

// Index of address selection rules for IPv6 default address selection
typedef enum
{
    ADDR_INVALID = 0,
    ADDR_USER_SPECIFIED,
    ADDR_INVALID_ADDRESS_SPECIFIED,
    ADDR_UNDEFINED,
    ADDR_STILL_VALID,
    ADDR_SEL_RULE_1,
    ADDR_SEL_RULE_2,
    ADDR_SEL_RULE_3,
    ADDR_SEL_RULE_4,
    ADDR_SEL_RULE_5,
    ADDR_SEL_RULE_6,
    ADDR_SEL_RULE_7,
    ADDR_SEL_RULE_8,
    ADDR_SEL_RULE_9,
    ADDR_SEL_RULE_10,
} IPV6_ADDR_SEL_INDEX;


// IPV6 event registration
typedef struct  _TAG_IPV6_LIST_NODE
{
    struct _TAG_IPV6_LIST_NODE*     next;       // next node in list
                                                // makes it valid SGL_LIST_NODE node
    IPV6_EVENT_HANDLER              handler;    // handler to be called for event
    const void*                     hParam;     // handler parameter
    TCPIP_NET_HANDLE                hNet;       // interface that's registered for
                                                // 0 if all
}IPV6_LIST_NODE;


// IPv6 ULA state machine
typedef enum
{
    TCPIP_IPV6_ULA_IDLE,
    TCPIP_IPV6_ULA_PARAM_SET,
    TCPIP_IPV6_ULA_CONN_START,
    TCPIP_IPV6_ULA_TSTAMP_GET,

}TCPIP_IPV6_ULA_STATE;

// misc IP functions
//

/*****************************************************************************
  Function:
    IPV6_DATA_SEGMENT_HEADER * TCPIP_IPV6_DataSegmentHeaderAllocate (uint16_t len)

  Summary:
    Allocates a data segment header and an optional payload

  Description:
    Allocates a data segment header and an optional payload

  Precondition:
    None

  Parameters:
    len - Length of the optional dynamic payload to allocate for this segment.

  Returns:
    IPV6_DATA_SEGMENT_HEADER * - Pointer to the new segment.

  Remarks:
    None
  ***************************************************************************/
IPV6_DATA_SEGMENT_HEADER * TCPIP_IPV6_DataSegmentHeaderAllocate (uint16_t len);


/*****************************************************************************
  Function:
    unsigned short TCPIP_IPV6_PseudoHeaderChecksumGet (IPV6_PACKET * ptrPacket)

  Summary:
    Returns the 16-bit checksum of the pseudo-header for an IP packet.

  Description:
    Returns the 16-bit checksum of the pseudo-header for an IP packet.

  Precondition:
    None

  Parameters:
    ptrPacket - The packet

  Returns:
    unsigned short - The checksum

  Remarks:
    A flag in the packet is used to determine the address type (IPv4/6) for
    this calculation.
  ***************************************************************************/
unsigned short TCPIP_IPV6_PseudoHeaderChecksumGet (IPV6_PACKET * pkt);


/*****************************************************************************
  Function:
    void TCPIP_IPV6_PacketSegmentInsert (IPV6_DATA_SEGMENT_HEADER * ptrSegment,
        IPV6_PACKET * ptrPacket, IPV6_SEGMENT_TYPE type)

  Summary:
    Inserts a data segment into an IPV6_PACKET structure.

  Description:
    This function inserts a data segment into an IPV6_PACKET structure.  It will
    update the next header fields in existing segments, if applicable.

  Precondition:
    None

  Parameters:
    ptrSegment - The segment to insert.
    ptrPacket - The packet to insert the segment into.
    type - The segment type.  Defined by IPV6_SEGMENT_TYPE enumeration.

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void TCPIP_IPV6_PacketSegmentInsert (IPV6_DATA_SEGMENT_HEADER * ptrSegment, IPV6_PACKET * ptrPacket, IPV6_SEGMENT_TYPE);


/*****************************************************************************
  Function:
    IPV6_DATA_SEGMENT_HEADER * TCPIP_IPV6_DataSegmentGetByType (IPV6_PACKET * ptrPacket, IPV6_SEGMENT_TYPE type)

  Summary:
    Returns the data segment header of a segment of specified type from a
    packet.

  Description:
    Returns the data segment header of a segment of specified type from a
    packet.

  Precondition:
    None

  Parameters:
    ptrPacket - The packet to search.
    type - IPV6_SEGMENT_TYPE segment type value to search for.

  Returns:
    IPV6_DATA_SEGMENT_HEADER * - Pointer to the specified segment type, or NULL.

  Remarks:
    There is a special IPV6_SEGMENT_TYPE value defined:
        - TYPE_IPV6_BEGINNING_OF_WRITABLE_PART searches for the first upper layer
            payload segment to which data can be written.
  ***************************************************************************/
IPV6_DATA_SEGMENT_HEADER * TCPIP_IPV6_DataSegmentGetByType (IPV6_PACKET * ptrPacket, IPV6_SEGMENT_TYPE type);


/*****************************************************************************
  Function:
    IPV6_DATA_SEGMENT_HEADER * TCPIP_IPV6_DataSegmentContentsGetByType (
        IPV6_PACKET * ptrPacket, IPV6_SEGMENT_TYPE type)

  Summary:
    Returns a pointer to the contents of a segment of specified type from a
    packet.

  Description:
    Returns a pointer to the contents of a segment of specified type from a
    packet.

  Precondition:
    None

  Parameters:
    ptrPacket - The packet to search.
    type - IPV6_SEGMENT_TYPE segment type value to search for.

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void * TCPIP_IPV6_DataSegmentContentsGetByType (IPV6_PACKET * ptrPacket, IPV6_SEGMENT_TYPE type);


/*****************************************************************************
  Function:
    void TCPIP_IPV6_PacketDataFree (IPV6_PACKET * ptrPacket)

  Summary:
    Frees all dynamically allocated structures used by an IPV6_PACKET struct.

  Description:
    Frees all dynamically allocated structures used by an IPV6_PACKET struct.

  Precondition:
    None

  Parameters:
    ptrPacket - The packet to free.

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void TCPIP_IPV6_PacketDataFree (IPV6_PACKET * ptrPacket);


/*****************************************************************************
  Function:
    void TCPIP_IPV6_FragmentBufferFree (void * ptrFragment)

  Summary:
    Frees a fragment processing buffer.

  Description:
    Frees a fragment processing buffer.

  Precondition:
    None

  Parameters:
    ptrFragment - The fragment to free.

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void TCPIP_IPV6_FragmentBufferFree (void * ptrFragment);


#define TCPIP_IPV6_GetOptionHeader(h, data, count)   MACGetArray (h,(unsigned char *)data, count << 3)


/*****************************************************************************
  Function:
    uint8_t TCPIP_IPV6_HopByHopOptionsHeaderProcess (TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt, uint8_t * nextHeader, uint16_t * length)

  Summary:
    Processes an IPv6 Hop-by-hop Extension header.

  Description:
    Processes an IPv6 Hop-by-hop Extension header.

  Precondition:
    None

  Parameters:
    pNetIf - The interface the packet with this header was received on.
    pRxPkt  - The RX packet
    nextHeader - Return value for the next header.
    length - Return value for the header length.

  Returns:
    uint8_t - An action code for the TCPIP_IPV6_Process function.

  Remarks:
    None
  ***************************************************************************/
uint8_t TCPIP_IPV6_HopByHopOptionsHeaderProcess (TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt, uint8_t * nextHeader, uint16_t * length);


/*****************************************************************************
  Function:
    uint8_t TCPIP_IPV6_DestinationOptionsHeaderProcess (TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt, uint8_t * nextHeader, uint16_t * length)

  Summary:
    Processes an IPv6 Destination Extension header.

  Description:
    Processes an IPv6 Destination Extension header.

  Precondition:
    None

  Parameters:
    pNetIf - The interface the packet with this header was received on.
    pRxPkt  - The RX packet
    nextHeader - Return value for the next header.
    length - Return value for the header length.

  Returns:
    uint8_t - An action code for the TCPIP_IPV6_Process function.

  Remarks:
    None
  ***************************************************************************/
uint8_t TCPIP_IPV6_DestinationOptionsHeaderProcess (TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt, uint8_t * nextHeader, uint16_t * length);


/*****************************************************************************
  Function:
    uint8_t TCPIP_IPV6_RoutingHeaderProcess (TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt
        uint8_t * nextHeader, uint16_t * length)

  Summary:
    Processes an IPv6 Routing Extension header.

  Description:
    Processes an IPv6 Routing Extension header.

  Precondition:
    None

  Parameters:
    pNetIf - The interface the packet with this header was received on.
    pRxPkt  - The RX packet
    nextHeader - Return value for the next header.
    length - Return value for the header length.

  Returns:
    uint8_t - An action code for the TCPIP_IPV6_Process function.

  Remarks:
    None
  ***************************************************************************/
uint8_t TCPIP_IPV6_RoutingHeaderProcess (TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt, uint8_t * nextHeader, uint16_t * length);


/*****************************************************************************
  Function:
    uint8_t TCPIP_IPV6_FragmentationHeaderProcess(TCPIP_NET_IF * pNetIf,
                                                  IPV6_ADDR * remoteIP,
                                                  IPV6_ADDR * localIP,
                                                  uint8_t * nextHeader,
                                                  uint16_t dataCount,
                                                  uint16_t headerLen,
                                                  TCPIP_MAC_PACKET* pRxPkt,
                                                  uint16_t previousHeaderLen)
  Summary:
    Processes an IPv6 Fragmentation Extension header.

  Description:
    Processes an IPv6 Fragmentation Extension header.  This will usually results
    in the packet data being cached in a fragment buffer.

  Precondition:
    None

  Parameters:
    pNetIf - The interface the packet with this header was received on.
    remoteIP - The packet's source IP address
    localIP - The packet's destination IP address
    nextHeader - Return value for the next header
    dataCount - Length of the fragment header and everything after it
    headerLen - Length of all headers before the fragmentation header (including
        the IPv6 header)
    remoteMACAddr - The sender's MAC address
    previousHeaderLen - Length of the previous extension header (for finding
        next header value)

  Returns:
    uint8_t - An action code for the TCPIP_IPV6_Process function.

  Remarks:
    None
  ***************************************************************************/
uint8_t TCPIP_IPV6_FragmentationHeaderProcess (TCPIP_NET_IF * pNetIf, const IPV6_ADDR * source, const IPV6_ADDR * dest, uint8_t * nextHeader, uint16_t dataCount, uint16_t headerLen, TCPIP_MAC_PACKET* pRxPkt, uint16_t previousHeader);


/*****************************************************************************
  Function:
    IPV6_ADDR_STRUCT * TCPIP_IPV6_SolicitedNodeMulticastAddressFind(TCPIP_NET_IF * pNetIf, IPV6_ADDR * addr, unsigned char listType)

  Summary:
    Finds a unicast address based on a given solicited-node multicast address

  Description:
    Finds a unicast address based on a given solicited-node multicast address

  Precondition:
    None

  Parameters:
    pNetIf - The interface to check for the address.
    addr - The address to check for.
    listType - IPV6_ADDR_TYPE_UNICAST_TENTATIVE or IPV6_ADDR_TYPE_UNICAST

  Returns:
    IPV6_ADDR_STRUCT * - Pointer to the found address, or NULL

  Remarks:
    None
  ***************************************************************************/
IPV6_ADDR_STRUCT *  TCPIP_IPV6_SolicitedNodeMulticastAddressFind(TCPIP_NET_IF * pNetIf, const IPV6_ADDR * addr, unsigned char listType);


#define IPV6_ADDR_POLICY_TABLE_LEN      (sizeof (gPolicyTable) / sizeof (IPV6_ADDRESS_POLICY))


/*****************************************************************************
  Function:
    uint8_t TCPIP_IPV6_DASPolicyGet (const IPV6_ADDR * addr, uint8_t * label, uint8_t * precedence, uint8_t * prefixLen)

  Summary:
    Gets the default address selection policy for an address.

  Description:
    Gets the default address selection policy for an address.

  Precondition:
    None

  Parameters:
    addr - The given address
    label - Return value for the address label
    precedence - Return value for the precedence
    prefixLen - Return value for the prefix length

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
unsigned char TCPIP_IPV6_DASPolicyGet (const IPV6_ADDR * addr, unsigned char * label, unsigned char * precedence, unsigned char * prefixLen);


#define TCPIP_IPV6_PutRxData(mac,pkt,len)        TCPIP_IPV6_ArrayPutHelper(pkt, mac, IPV6_DATA_NETWORK_FIFO, len)


/*****************************************************************************
  Function:
    void TCPIP_IPV6_FragmentTask (void)

  Summary:
    IPv6 fragment processing task function.

  Description:
    IPv6 fragment processing task function.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void TCPIP_IPV6_FragmentTask (void);


/*****************************************************************************
  Function:
    void TCPIP_IPV6_TimestampsTaskUpdate (void)

  Summary:
    Task to update timestamps and check for validity for NDP structures.

  Description:
    Task to update timestamps and check for validity for NDP structures.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void TCPIP_IPV6_TimestampsTaskUpdate (void);


/*****************************************************************************
  Function:
    bool TCPIP_IPV6_PacketTransmitInFragments (IPV6_PACKET * pkt, uint16_t mtu)

  Summary:
    Transmits a packet in fragments across a link with an MTU less than the
    packet size.

  Description:
    Transmits a packet in fragments across a link with an MTU less than the
    packet size.

  Precondition:
    None

  Parameters:
    pkt - The packet
    mtu - The link MTU.

  Returns:
    bool - true if the packet was transmitted, false otherwise.

  Remarks:
    None
  ***************************************************************************/
bool TCPIP_IPV6_PacketTransmitInFragments (IPV6_PACKET * pkt, uint16_t mtu);


/*****************************************************************************
  Function:
    void TCPIP_IPV6_FreeConfigLists (IPV6_INTERFACE_CONFIG * pNetIf)

  Summary:
    Frees all dynamically allocated structures from linked lists used
    for IPv6 configuration on an interface.

  Description:
    Frees all dynamically allocated structures from linked lists used
    for IPv6 configuration on an interface.

  Precondition:
    None

  Parameters:
    pNetIf - The interface.

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void TCPIP_IPV6_FreeConfigLists (IPV6_INTERFACE_CONFIG * pNetIf);


/*****************************************************************************
  Function:
    unsigned char TCPIP_IPV6_ASCompareSourceAddresses(TCPIP_NET_IF * pNetIf,
                                                      IPV6_ADDR_STRUCT * addressOne,
                                                      IPV6_ADDR_STRUCT * addressTwo,
                                                      IPV6_ADDR * dest,
                                                      IPV6_ADDR_SEL_INDEX rule)

  Summary:
    Compares two IPv6 addresses using specified rules to determine which
    ones are preferable.

  Description:
    Compares two IPv6 addresses using specified rules to determine which
    ones are preferable.

  Precondition:
    None

  Parameters:
    pNetIf - The given interface
    addressOne - One address to compare
    addressTwo - The other address to compare
    dest - A destination address (used for some comparisons)
    rule - The address comparison rule to use

  Returns:
    bool - true if addressTwo is preferred over addressOne, false otherwise

  Remarks:
    None
  ***************************************************************************/
unsigned char TCPIP_IPV6_ASCompareSourceAddresses(TCPIP_NET_IF * pNetIf, IPV6_ADDR_STRUCT * addressOne, IPV6_ADDR_STRUCT * addressTwo, const IPV6_ADDR * dest, IPV6_ADDR_SEL_INDEX rule);

#endif // _IPV6_PRIVATE_H_



