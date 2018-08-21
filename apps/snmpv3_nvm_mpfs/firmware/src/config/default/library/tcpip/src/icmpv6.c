/*******************************************************************************
  Internet Control Message Protocol (ICMP) Server

  Summary:
    Module for Microchip TCP/IP Stack

  Description:
    - Provides "ping" diagnostics
*******************************************************************************/

/*******************************************************************************
File Name:  ICMPv6
Copyright © 2012 released Microchip Technology Inc.  All rights
reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_ICMPV6

#include "tcpip/src/tcpip_private.h"

#if defined TCPIP_STACK_USE_IPV6



static const void*      icmpv6MemH = 0;        // memory handle
static int              icmpv6InitCount = 0;      // module initialization count

// Callback function for informing the upper-layer protocols about ICMPv6 events
typedef void (*icmpv6Callback) (TCPIP_NET_HANDLE hNetIf, uint8_t type, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, void * header);

static PROTECTED_SINGLE_LIST      icmpv6RegisteredUsers = { {0} };
//
// ICMPv6 callback registration
typedef struct  _TAG_ICMPV6_LIST_NODE
{
    struct _TAG_ICMPV6_LIST_NODE*   next;       // next node in list
                                                // makes it valid SGL_LIST_NODE node
    icmpv6Callback                  callback;   // handler to be called for ICMPV6 event
}ICMPV6_LIST_NODE;

// prototypes
//
static bool _ICMPV6_AckPacket (void * pktPointer, bool sent, const void * param);

static void _ICMPV6_NotifyClients(TCPIP_NET_HANDLE hNetIf, uint8_t type, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, void * header);


/*****************************************************************************
  Function:
    bool TCPIP_ICMPV6_Initialize (const TCPIP_STACK_MODULE_CTRL* const stackInit,
                       const void* icmpv6Data)

  Summary:
    Initializes the ICMPv6 modules.

  Description:
    Initializes the ICMPv6 modules.


  Precondition:
    None

  Parameters:
    stackInit - Stack initialization information.
    icmpv6Data - Optional protocol-specific data.

  Returns:
    true

  Remarks:
    None
  ***************************************************************************/
bool TCPIP_ICMPV6_Initialize (const TCPIP_STACK_MODULE_CTRL* const stackInit,
                       const void* icmpv6Data)
{
    if(stackInit->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }

    // stack init

    if (icmpv6InitCount == 0)
    {   // first time we're run
        icmpv6MemH = stackInit->memH;
        if(!TCPIP_Notification_Initialize(&icmpv6RegisteredUsers))
        {
            return false;
        }
    }

    icmpv6InitCount++;
    return true;
}

/*****************************************************************************
  Function:
    bool TCPIP_ICMPV6_Deinitialize (const TCPIP_STACK_MODULE_CTRL* const stackInit)

  Summary:
    Deinitializes the ICMPv6 modules.

  Description:
    Deinitializes the ICMPv6 modules.


  Precondition:
    None

  Parameters:
    stackInit - Stack initialization information.

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_ICMPV6_Deinitialize (const TCPIP_STACK_MODULE_CTRL* const stackInit)
{
    if(icmpv6InitCount > 0)
    {   // we're up and running
        if(stackInit->stackAction == TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            TCPIP_Notification_Deinitialize(&icmpv6RegisteredUsers, icmpv6MemH);
            if(--icmpv6InitCount == 0)
            {   // all closed
                // release resources
                icmpv6MemH = 0;
            }
        }
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

/*****************************************************************************
  Function:
    ICMPV6_HANDLE TCPIP_ICMPV6_CallbackRegister (void (*callback)(TCPIP_NET_HANDLE hNetIf,
        uint8_t type, IPV6_ADDR * localIP, IPV6_ADDR * remoteIP, void * header))

  Summary:
    Registers an upper-layer function for ICMPv6 callback.

  Description:
    Registers an upper-layer function to handle ICMPv6 messages that may
    require action at the application layer (Echo Replies, Error messages)

  Precondition:
    None

  Parameters:
    type - ICMPv6 header type
    localIP - IPv6 destination address of the incoming message
    remoteIP - IPv6 address of the node that originated the incoming message
    header - Pointer to the ICMPv6 header

  Returns:
    A ICMPV6_HANDLE

  Remarks:
    None
  ***************************************************************************/
ICMPV6_HANDLE TCPIP_ICMPV6_CallbackRegister (void (*callback)(TCPIP_NET_HANDLE hNetIf, uint8_t type, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, void * header))
{
    if(callback && icmpv6MemH)
    {
        ICMPV6_LIST_NODE* newNode = (ICMPV6_LIST_NODE*)TCPIP_Notification_Add(&icmpv6RegisteredUsers, icmpv6MemH, sizeof(*newNode));
        if(newNode)
        {
            newNode->callback = callback;
            return newNode;
        }
    }

    return 0;

}


bool TCPIP_ICMPV6_CallbackDeregister(ICMPV6_HANDLE hIcmpv6)
{
    if(hIcmpv6 && icmpv6MemH)
    {
        if(TCPIP_Notification_Remove((SGL_LIST_NODE*)hIcmpv6, &icmpv6RegisteredUsers, icmpv6MemH))
        {
            return true;
        }
    }

    return false;


}

bool TCPIP_ICMPV6_EchoRequestSend(TCPIP_NET_HANDLE netH, IPV6_ADDR * targetAddr, uint16_t sequenceNumber, uint16_t identifier,uint32_t packetSize)
{
    uint32_t  payload = 0x44332211;
    IPV6_PACKET * pkt;
    IPV6_ADDR_STRUCT * localAddress=NULL;
    uint32_t    icmpPktSize=0;
    int count=0;
    TCPIP_NET_IF  *pNetIf;

    pNetIf = _TCPIPStackHandleToNetUp(netH);

    if(localAddress == NULL)
    {
        localAddress = TCPIP_IPV6_DASSourceAddressSelect (pNetIf, targetAddr, NULL);
    }

    if (localAddress == NULL)
    {
       //SYS_OUT_MESSAGE_LINE ("No local addr!", 1);
        return false;
    }

    pkt = TCPIP_ICMPV6_HeaderEchoRequestPut (pNetIf, &localAddress->address, targetAddr, ICMPV6_INFO_ECHO_REQUEST,
                                        identifier, sequenceNumber);

    if(packetSize ==0)
    {
        icmpPktSize = 4;
    }
    else if(packetSize >=1280)
    {
        icmpPktSize = 1600;
    }

    if (TCPIP_IPV6_TxIsPutReady(pkt, icmpPktSize) < icmpPktSize)
    {
        TCPIP_IPV6_PacketFree (pkt);
        return false;
    }

    if(packetSize ==0)
    {
        TCPIP_IPV6_PutArray (pkt, (uint8_t *)&payload, sizeof (uint32_t));
    }
    else
    {
        for(count=0;count<(packetSize/4);count++)
        {
            TCPIP_IPV6_PutArray (pkt, (uint8_t *)&payload, sizeof(uint32_t));
        }
    }

    // Just let the IPv6 module figure out the next hop neighbor and its MAC address
    TCPIP_ICMPV6_Flush (pkt);

    return true;
}

/*****************************************************************************
  Function:
    static void _ICMPV6_AckPacket (void * pktPointer, bool sent, const void * param)

  Summary:
    Deallocates a packet after it has been transmitted

  Description:
    Deallocates a packet after it has been transmitted

  Precondition:
    None

  Parameters:
    pktPointer - Pointer to the packet that was transmitted
    sent - true if the packet was sent, false otherwise
    param - 0

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
static bool _ICMPV6_AckPacket (void * pktPointer, bool sent, const void * param)
{
    if (pktPointer)
    {
        TCPIP_IPV6_PacketFree (pktPointer);
    }
    return false;
}

/*****************************************************************************
  Function:
    IPV6_PACKET * TCPIP_ICMPV6_Open (TCPIP_NET_IF * pNetIf, IPV6_ADDR * localIP,
        IPV6_ADDR * remoteIP)

  Summary:
    Allocates an IPV6_PACKET structure and populates it with source and
    destination address.

  Description:
    Allocates an IPV6_PACKET structure and populates it with source and
    destination address.

  Precondition:
    None

  Parameters:
    pNetIf - Interface for the outgoing packet.
    localIP - local IPv6 address
    remoteIP - destination IPv6 address

  Returns:
    IPV6_PACKET * - Pointer to the allocated packet or NULL.

  Remarks:
    None
  ***************************************************************************/
IPV6_PACKET * TCPIP_ICMPV6_Open (TCPIP_NET_IF * pNetIf, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP)
{
    IPV6_PACKET * pkt = TCPIP_IPV6_TxPacketAllocate(pNetIf, _ICMPV6_AckPacket, 0);

    if (pkt == NULL)
        return NULL;

    TCPIP_IPV6_DestAddressSet(pkt,remoteIP);
    if (localIP != NULL)
    {
        if (localIP == &IPV6_FIXED_ADDR_UNSPECIFIED)
        {
            pkt->flags.useUnspecAddr = true;
            pkt->flags.sourceSpecified = true;
        }
        else
        {
            TCPIP_IPV6_SourceAddressSet(pkt, localIP);
        }
    }

    return pkt;
}

/*****************************************************************************
  Function:
    IPV6_PACKET * TCPIP_ICMPV6_HeaderErrorPut (TCPIP_NET_IF * pNetIf,
        IPV6_ADDR * localIP, IPV6_ADDR * remoteIP, uint8_t code,
        uint8_t type, uint32_t additionalData)

  Summary:
    Allocates a packet, IPv6 Header, and Upper-layer header for an ICMPv6
    error message.

  Description:
    Allocates a packet, IPv6 Header, and Upper-layer header for an ICMPv6
    error message.

  Precondition:
    None

  Parameters:
    pNetIf - The interface for the outgoing packet.
    localIP - The local address that should be used for this packet.
    remoteIP - The packet's destination address
    code - The error code to use
    type - The error type to use
    additionalData - Addition header data (depends on type)

  Returns:
    IPV6_PACKET * - The constructed error packet or NULL

  Remarks:
    None
  ***************************************************************************/
IPV6_PACKET * TCPIP_ICMPV6_HeaderErrorPut (TCPIP_NET_IF * pNetIf, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, uint8_t code, uint8_t type, uint32_t additionalData)
{
    ICMPV6_HEADER_ERROR header;
    IPV6_PACKET * pkt;

    pkt = TCPIP_ICMPV6_Open (pNetIf, localIP, remoteIP);
    if (pkt == NULL)
        return NULL;

    header.vType = type;
    header.vCode = code;
    header.wChecksum = 0x0000;
    header.additionalData = additionalData;

    TCPIP_IPV6_HeaderPut(pkt, IPV6_PROT_ICMPV6);

    TCPIP_IPV6_HopLimitSet(pkt, 255);

    // Put the ICMPv6 Header
    if (TCPIP_IPV6_UpperLayerHeaderPut (pkt, (void *)&header, sizeof (ICMPV6_HEADER_ERROR), IPV6_PROT_ICMPV6, ICMPV6_CHECKSUM_OFFSET) == NULL)
    {
        TCPIP_IPV6_PacketFree(pkt);
        return NULL;
    }

    return pkt;
}

/*****************************************************************************
  Function:
    IPV6_PACKET * TCPIP_ICMPV6_HeaderEchoRequestPut (TCPIP_NET_HANDLE hNetIf,
        IPV6_ADDR * localIP, IPV6_ADDR * remoteIP, uint8_t type,
        uint16_t identifier, uint16_t sequenceNumber)

  Summary:
    Allocates a packet, IPv6 Header, and Upper-layer header for an ICMPv6
    echo request.

  Description:
    Allocates a packet, IPv6 Header, and Upper-layer header for an ICMPv6
    echo request.

  Precondition:
    None

  Parameters:
    pNetIf - The interface for the outgoing packet.
    localIP - The local address that should be used for this packet.
    remoteIP - The packet's destination address
    type - Echo Request or Echo Reply
    identifier - The Echo Request id.
    sequenceNumber - The Echo request sequence number

  Returns:
    IPV6_PACKET * - The constructed error packet or NULL

  Remarks:
    None
  ***************************************************************************/
IPV6_PACKET * TCPIP_ICMPV6_HeaderEchoRequestPut (TCPIP_NET_HANDLE hNetIf, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, uint8_t type, uint16_t identifier, uint16_t sequenceNumber)
{
    ICMPV6_HEADER_ECHO header;
    IPV6_PACKET * pkt;

    TCPIP_NET_IF * pNetIf = _TCPIPStackHandleToNet(hNetIf);
    if (pNetIf == 0)
    {
            return 0;
    }

    pkt = TCPIP_ICMPV6_Open (pNetIf, localIP, remoteIP);
    if (pkt == NULL)
        return NULL;

    header.vType = type;
    header.vCode = ICMPV6_INFO_EREQ_CODE;
    header.wChecksum = 0x0000;
    header.identifier = TCPIP_Helper_htons (identifier);
    header.sequenceNumber = TCPIP_Helper_htons (sequenceNumber);

    TCPIP_IPV6_HeaderPut(pkt, IPV6_PROT_ICMPV6);

    // Put the ICMPv6 Header
    if (TCPIP_IPV6_UpperLayerHeaderPut (pkt, (void *)&header, sizeof (ICMPV6_HEADER_ECHO), IPV6_PROT_ICMPV6, ICMPV6_CHECKSUM_OFFSET) == NULL)
    {
        TCPIP_IPV6_PacketFree(pkt);
        return NULL;
    }

    return pkt;
}

/*****************************************************************************
  Function:
    IPV6_PACKET * TCPIP_ICMPV6_HeaderRouterSolicitationPut (TCPIP_NET_IF * pNetIf,
        IPV6_ADDR * localIP, IPV6_ADDR * remoteIP)

  Summary:
    Allocates a packet, IPv6 Header, and Upper-layer header for an ICMPv6
    router solicitation.

  Description:
    Allocates a packet, IPv6 Header, and Upper-layer header for an ICMPv6
    router solicitation.

  Precondition:
    None

  Parameters:
    pNetIf - The interface for the outgoing packet.
    localIP - The local address that should be used for this packet.
    remoteIP - The packet's destination address

  Returns:
    IPV6_PACKET * - The constructed error packet or NULL

  Remarks:
    None
  ***************************************************************************/
IPV6_PACKET * TCPIP_ICMPV6_HeaderRouterSolicitationPut (TCPIP_NET_IF * pNetIf, IPV6_ADDR * localIP, IPV6_ADDR * remoteIP)
{
    ICMPV6_HEADER_ROUTER_SOLICITATION header;
    IPV6_PACKET * pkt;

    pkt = TCPIP_ICMPV6_Open (pNetIf, localIP, remoteIP);
    if (pkt == NULL)
        return NULL;

    header.vType = ICMPV6_INFO_ROUTER_SOLICITATION;
    header.vCode = 0;
    header.wChecksum = 0x0000;
    header.Reserved = 0x00000000;

    TCPIP_IPV6_HeaderPut(pkt, IPV6_PROT_ICMPV6);

    TCPIP_IPV6_HopLimitSet(pkt,255);

    // Put the ICMPv6 Header
    if (TCPIP_IPV6_UpperLayerHeaderPut (pkt, (void *)&header, sizeof (ICMPV6_HEADER_ROUTER_SOLICITATION), IPV6_PROT_ICMPV6, ICMPV6_CHECKSUM_OFFSET) == NULL)
    {
        TCPIP_IPV6_PacketFree(pkt);
        return NULL;
    }

    return pkt;
}

/*****************************************************************************
  Function:
    IPV6_PACKET * TCPIP_ICMPV6_HeaderNeighborSolicitationPut (
        TCPIP_NET_IF * pNetIf, IPV6_ADDR * localIP, IPV6_ADDR * remoteIP,
        IPV6_ADDR * targetAddr)

  Summary:
    Allocates a packet, IPv6 Header, and Upper-layer header for an ICMPv6
    neighbor solicitation.

  Description:
    Allocates a packet, IPv6 Header, and Upper-layer header for an ICMPv6
    neighbor solicitation.

  Precondition:
    None

  Parameters:
    pNetIf - The interface for the outgoing packet.
    localIP - The local address that should be used for this packet.
    remoteIP - The packet's destination address
    targetAddr - The IPv6 address to solicit

  Returns:
    IPV6_PACKET * - The constructed error packet or NULL

  Remarks:
    None
  ***************************************************************************/
IPV6_PACKET * TCPIP_ICMPV6_HeaderNeighborSolicitationPut (TCPIP_NET_IF * pNetIf, IPV6_ADDR * localIP, IPV6_ADDR * remoteIP, IPV6_ADDR * targetAddr)
{
    ICMPV6_HEADER_NEIGHBOR_SOLICITATION header;
    IPV6_PACKET * pkt;

    pkt = TCPIP_ICMPV6_Open (pNetIf, localIP, remoteIP);
    if (pkt == NULL)
        return NULL;

    header.vType = ICMPV6_INFO_NEIGHBOR_SOLICITATION;
    header.vCode = 0;
    header.wChecksum = 0x0000;
    header.Reserved = 0x00000000;
    memcpy (&header.aTargetAddress, targetAddr, sizeof (IPV6_ADDR));

    TCPIP_IPV6_HeaderPut(pkt, IPV6_PROT_ICMPV6);

    TCPIP_IPV6_HopLimitSet(pkt,255);

    // Put the ICMPv6 Header
    if (TCPIP_IPV6_UpperLayerHeaderPut (pkt, (void *)&header, sizeof (ICMPV6_HEADER_NEIGHBOR_SOLICITATION), IPV6_PROT_ICMPV6, ICMPV6_CHECKSUM_OFFSET) == NULL)
    {
        TCPIP_IPV6_PacketFree(pkt);
        return NULL;
    }

    return pkt;
}

/*****************************************************************************
  Function:
    IPV6_PACKET * TCPIP_ICMPV6_HeaderNeighborAdvertisementPut (
        TCPIP_NET_IF * pNetIf, IPV6_ADDR * localIP, IPV6_ADDR * remoteIP,
        IPV6_ADDR * targetAddr, bool solicited, bool override)

  Summary:
    Allocates a packet, IPv6 Header, and Upper-layer header for an ICMPv6
    neighbor advertisement.

  Description:
    Allocates a packet, IPv6 Header, and Upper-layer header for an ICMPv6
    neighbor advertisement.

  Precondition:
    None

  Parameters:
    pNetIf - The interface for the outgoing packet.
    localIP - The local address that should be used for this packet.
    remoteIP - The packet's destination address
    targetAddr - The address being advertised
    solicited - The value of the 'solicited' flag
    override - The value of the 'override' flag

  Returns:
    IPV6_PACKET * - The constructed error packet or NULL

  Remarks:
    None
  ***************************************************************************/
IPV6_PACKET * TCPIP_ICMPV6_HeaderNeighborAdvertisementPut (TCPIP_NET_IF * pNetIf, IPV6_ADDR * localIP, IPV6_ADDR * remoteIP, IPV6_ADDR * targetAddr, bool solicited, bool override)
{
    ICMPV6_HEADER_NEIGHBOR_ADVERTISEMENT header;
    IPV6_PACKET * pkt;

    pkt = TCPIP_ICMPV6_Open (pNetIf, localIP, remoteIP);
    if (pkt == NULL)
        return NULL;

    header.vType = ICMPV6_INFO_NEIGHBOR_ADVERTISEMENT;
    header.vCode = 0;
    header.wChecksum = 0x0000;
    // This will set the reserved bits and flags to 0
    header.flags.Val = 0;
    header.Reserved1 = 0;
    header.Reserved2 = 0;
    if (override)
        header.flags.bits.O = 1;

    if (solicited)
        header.flags.bits.S = 1;

    memcpy(&header.aTargetAddress, targetAddr, sizeof(IPV6_ADDR));

    TCPIP_IPV6_HeaderPut(pkt, IPV6_PROT_ICMPV6);

    TCPIP_IPV6_HopLimitSet(pkt,255);

    // Put the ICMPv6 Header
    if (TCPIP_IPV6_UpperLayerHeaderPut (pkt, (void *)&header, sizeof (ICMPV6_HEADER_NEIGHBOR_ADVERTISEMENT), IPV6_PROT_ICMPV6, ICMPV6_CHECKSUM_OFFSET) == NULL)
    {
        TCPIP_IPV6_PacketFree(pkt);
        return NULL;
    }

    return pkt;
}

/*****************************************************************************
  Function:
    bool TCPIP_ICMPV6_Flush (IPV6_PACKET * pkt)

  Summary:
    Flushes an ICMPv6 Packet

  Description:
    Flushes an ICMPv6 Packet

  Precondition:
    None

  Parameters:
    pkt - The packet to flush

  Returns:
    true if the packet was flushed, false if the packet was queued

  Remarks:
    None
  ***************************************************************************/
bool TCPIP_ICMPV6_Flush (IPV6_PACKET * pkt)
{
    return TCPIP_IPV6_Flush(pkt);
}

/*****************************************************************************
  Function:
    static void _ICMPv6_SwapHeader(ICMPV6_HEADER_TYPES * header)

  Summary:
    Swaps wrong-endian fields in an ICMPv6 header

  Description:
    Swaps wrong-endian fields in an ICMPv6 header

  Precondition:
    None

  Parameters:
    header - The header to correct

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
static void _ICMPv6_SwapHeader(ICMPV6_HEADER_TYPES * header)
{
    header->header_Error.wChecksum        = TCPIP_Helper_ntohs(header->header_Error.wChecksum);
    switch (header->header_Error.vType)
    {
        case ICMPV6_INFO_ECHO_REQUEST:
        case ICMPV6_INFO_ECHO_REPLY:
            header->header_Echo.identifier = TCPIP_Helper_ntohs (header->header_Echo.identifier);
            header->header_Echo.sequenceNumber = TCPIP_Helper_ntohs (header->header_Echo.sequenceNumber);
            break;
        case ICMPV6_INFO_ROUTER_ADVERTISEMENT:
            header->header_RA.routerLifetime = TCPIP_Helper_ntohs (header->header_RA.routerLifetime);
            header->header_RA.retransTime = TCPIP_Helper_ntohl (header->header_RA.retransTime);
            header->header_RA.reachableTime = TCPIP_Helper_ntohl (header->header_RA.reachableTime);
            break;
        case ICMPV6_ERROR_PACKET_TOO_BIG:
            header->header_Error.additionalData = TCPIP_Helper_ntohl (header->header_Error.additionalData);
        default:
            break;

    }
}

/*****************************************************************************
  Function:
    void TCPIP_ICMPV6_Process(TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt,
        IPV6_ADDR_STRUCT * localIPStruct, IPV6_ADDR * localIP,
        IPV6_ADDR * remoteIP, uint16_t dataLen, uint16_t headerLen,
        uint8_t hopLimit, uint8_t addrType)

  Summary:
    Processes an incoming ICMPv6 packet.

  Description:
    Processes an incoming ICMPv6 packet.

  Precondition:
    None

  Parameters:
    pNetIf - The interface the packet was received on.
    pRxPkt - The RX packet that was received.
    localIPStruct - The local address that corresponded to the destination
        address of the packet (this does not have to equal the destination
        address because of multicast)
    localIP - The destination address of the incoming packet (for checksum)
    remoteIP - The source address of the incoming packet
    dataLen - Length of additional data in the packet
    headerLen - Length of the packet's headers
    hopLimit - Hop limit field of the packet (for validation)
    addrType - Local address type (processing may be limited if the local
        address is tentative)

  Returns:
    None

  Remarks:
    Packet discard is done at the IPv6 layer with the acknowledge code in the pRxPkt->pktClientData !
  ***************************************************************************/
void TCPIP_ICMPV6_Process(TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt, IPV6_ADDR_STRUCT * localIPStruct, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, uint16_t dataLen, uint16_t headerLen, uint8_t hopLimit, uint8_t addrType)
{
    IPV6_PSEUDO_HEADER  pseudoHeader;
    TCPIP_UINT32_VAL        checksums;
    ICMPV6_HEADER_TYPES h;
    NDP_OPTION_LLA               llaOption;
    NDP_OPTION_PREFIX_INFO       prefixOption;
    NDP_OPTION_MTU               mtuOption;
    uint8_t mData[2];
    uint8_t i;
    uint16_t j;
    uint16_t tempDataLen;
    IPV6_ADDRESS_TYPE tempAddressType;
    IPV6_HEAP_NDP_DC_ENTRY * destinationPointer;
    IPV6_HEAP_NDP_DR_ENTRY * routerPointer;
    IPV6_HEAP_NDP_NC_ENTRY * neighborPointer = NULL;
    IPV6_ADDR_STRUCT * localAddressPointer;
    IPV6_PACKET * pkt;
    IPV6_INTERFACE_CONFIG*  pIpv6Config;

    pRxPkt->pktClientData = TCPIP_MAC_PKT_ACK_RX_OK;
    
    if (pNetIf == NULL)
        return;

    pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pNetIf);

    // Calculate checksums
    memcpy (&pseudoHeader.SourceAddress, remoteIP, sizeof (IPV6_ADDR));
    memcpy (&pseudoHeader.DestAddress, localIP, sizeof (IPV6_ADDR));
    // Total payload length is the length of data + extension headers
    pseudoHeader.PacketLength = TCPIP_Helper_ntohs(dataLen);
    pseudoHeader.zero1 = 0;
    pseudoHeader.zero2 = 0;
    pseudoHeader.NextHeader = IPV6_PROT_ICMPV6;

    checksums.w[0] = ~TCPIP_Helper_CalcIPChecksum((uint8_t*)&pseudoHeader,
                                    sizeof(pseudoHeader), 0);

    checksums.w[1] = TCPIP_Helper_CalcIPChecksum(pRxPkt->pNetLayer, dataLen, 0);    //  MACCalcIPBufferChecksum(hMac, dataLen);

    if(checksums.w[0] != checksums.w[1])
    {
        pRxPkt->pktClientData = TCPIP_MAC_PKT_ACK_CHKSUM_ERR;
        return;
    }

    TCPIP_IPV6_RxBufferSet(pRxPkt, headerLen);

    TCPIP_IPV6_Get(pRxPkt, &h.header_Error.vType);

    // Get any extra types
    switch (h.header_Error.vType)
    {
        case ICMPV6_ERROR_DEST_UNREACHABLE:
        case ICMPV6_ERROR_PACKET_TOO_BIG:
        case ICMPV6_ERROR_TIME_EXCEEDED:
        case ICMPV6_ERROR_PARAMETER_PROBLEM:
            TCPIP_IPV6_ArrayGet(pRxPkt, (uint8_t *)&h + 1, sizeof (h.header_Error) - 1);
            break;
        case ICMPV6_INFO_ECHO_REQUEST:
        case ICMPV6_INFO_ECHO_REPLY:
            TCPIP_IPV6_ArrayGet(pRxPkt, (uint8_t *)&h + 1, sizeof (h.header_Echo) - 1);
            break;
        case ICMPV6_INFO_ROUTER_SOLICITATION:
            if (addrType == IPV6_ADDR_TYPE_UNICAST_TENTATIVE)
                return;
            TCPIP_IPV6_ArrayGet(pRxPkt, (uint8_t *)&h + 1, sizeof (h.header_RS) - 1);
            break;
        case ICMPV6_INFO_ROUTER_ADVERTISEMENT:
            if (addrType == IPV6_ADDR_TYPE_UNICAST_TENTATIVE)
                return;
            TCPIP_IPV6_ArrayGet(pRxPkt, (uint8_t *)&h + 1, sizeof (h.header_RA) - 1);
            break;
        case ICMPV6_INFO_NEIGHBOR_SOLICITATION:
        case ICMPV6_INFO_NEIGHBOR_ADVERTISEMENT:
            TCPIP_IPV6_ArrayGet(pRxPkt, (uint8_t *)&h + 1, sizeof (h.header_NS) - 1);
            break;
        case ICMPV6_INFO_REDIRECT:
            if (addrType == IPV6_ADDR_TYPE_UNICAST_TENTATIVE)
                return;
            TCPIP_IPV6_ArrayGet(pRxPkt, (uint8_t *)&h + 1, sizeof (h.header_Rd) - 1);
            break;
    }

    _ICMPv6_SwapHeader(&h);

    switch (h.header_Error.vType)
    {
        case ICMPV6_ERROR_DEST_UNREACHABLE:
            _ICMPV6_NotifyClients(pNetIf, ICMPV6_ERROR_DEST_UNREACHABLE, localIP, remoteIP, &h.header_Error);
            break;
        case ICMPV6_ERROR_PACKET_TOO_BIG:
            tempDataLen = dataLen - sizeof (h.header_Error);
            // If the received packet doesn't contain the IPv6 header of the packet that caused the
            // error, we can't extract the destination address and update the destination cache entry's MTU
            if (tempDataLen < sizeof (IPV6_HEADER))
                break;

            // Use the pseudo-header to store the extracted destination address
            TCPIP_IPV6_SetReadPtr(pRxPkt, (uint8_t *)TCPIP_IPV6_GetReadPtrInRx(pRxPkt) + IPV6_HEADER_OFFSET_DEST_ADDR);
            TCPIP_IPV6_ArrayGet (pRxPkt, (uint8_t *)&pseudoHeader.DestAddress, sizeof (IPV6_ADDR));

            if (pseudoHeader.DestAddress.v[0] == 0xFF)
            {
                // Update the multicast MTU
                if (pIpv6Config->multicastMTU > h.header_Error.additionalData)
                {
                    // Set the multicast MTU to the new value.
                    // If the value is less than the minimum link MTU the flush function will handle it
                    pIpv6Config->multicastMTU = h.header_Error.additionalData;
                    pIpv6Config->mtuIncreaseTimer = SYS_TMR_TickCountGet() + (SYS_TMR_TickCounterFrequencyGet() * TCPIP_IPV6_MTU_INCREASE_TIMEOUT);
                }
            }
            else
            {
                destinationPointer = TCPIP_NDP_RemoteNodeFind (pNetIf, &pseudoHeader.DestAddress, IPV6_HEAP_NDP_DC_ID);
                if (destinationPointer == NULL)
                {
                    // If we don't have a specific destination pointer for this node then update the link MTU.
                    // This should happen very rarely, since we should be creating destination cache entries
                    // for all outgoing packets.
                    if (pIpv6Config->linkMTU > h.header_Error.additionalData)
                    {
                        // Set the link MTU to the new value.
                        // If the value is less than the minimum link MTU the flush function will handle it
                        pIpv6Config->linkMTU = h.header_Error.additionalData;
                        pIpv6Config->mtuIncreaseTimer = SYS_TMR_TickCountGet() + (SYS_TMR_TickCounterFrequencyGet() * TCPIP_IPV6_MTU_INCREASE_TIMEOUT);
                    }
                }
                else
                {
                    // If the global link MTU is less than the specified MTU, use the global one
                    if (pIpv6Config->linkMTU < h.header_Error.additionalData)
                    {
                        h.header_Error.additionalData = pIpv6Config->linkMTU;
                    }
                    if (destinationPointer->pathMTU > h.header_Error.additionalData)
                    {
                        // Set the path MTU to the new value.
                        // If the value is less than the minimum link MTU the flush function will handle it
                        destinationPointer->pathMTU = h.header_Error.additionalData;
                        destinationPointer->pathMTUIncreaseTimer = SYS_TMR_TickCountGet() + (SYS_TMR_TickCounterFrequencyGet() * TCPIP_IPV6_MTU_INCREASE_TIMEOUT);
                    }
                }
            }
            if (icmpv6RegisteredUsers.list.head != NULL)
            {
                TCPIP_IPV6_RxBufferSet(pRxPkt, sizeof (ICMPV6_HEADER_ERROR));
                _ICMPV6_NotifyClients(pNetIf, ICMPV6_ERROR_PACKET_TOO_BIG, localIP, remoteIP, &h.header_Error);
            }
            break;
        case ICMPV6_ERROR_TIME_EXCEEDED:
            _ICMPV6_NotifyClients(pNetIf, ICMPV6_ERROR_TIME_EXCEEDED, localIP, remoteIP, &h.header_Error);
            break;
        case ICMPV6_ERROR_PARAMETER_PROBLEM:
            _ICMPV6_NotifyClients(pNetIf, ICMPV6_ERROR_PARAMETER_PROBLEM, localIP, remoteIP, &h.header_Error);
            break;
#if defined TCPIP_STACK_USE_ICMPV6_SERVER
        case ICMPV6_INFO_ECHO_REQUEST:
            _ICMPV6_NotifyClients(pNetIf, ICMPV6_INFO_ECHO_REQUEST, localIP, remoteIP, &h.header_Echo);

            TCPIP_IPV6_RxBufferSet (pRxPkt, sizeof (ICMPV6_HEADER_ECHO) + headerLen);

            neighborPointer = TCPIP_NDP_NextHopGet (pNetIf, remoteIP);

            if (neighborPointer == NULL)
            {
                return;
            }

            if (addrType == IPV6_ADDR_TYPE_UNICAST)
            {
                pkt = TCPIP_ICMPV6_PutHeaderEchoReply (pNetIf, &localIPStruct->address, remoteIP, ICMPV6_INFO_ECHO_REPLY, h.header_Echo.identifier, h.header_Echo.sequenceNumber);
                if (pkt == NULL)
                    return;

                if (TCPIP_IPV6_TxIsPutReady(pkt, dataLen - 8) < (dataLen - 8))
                {
                    pRxPkt->pktClientData = TCPIP_MAC_PKT_ACK_ALLOC_ERR;
                    TCPIP_IPV6_PacketFree (pkt);
                    return;
                }

                TCPIP_IPV6_ArrayPutHelper(pkt, pRxPkt, IPV6_DATA_NETWORK_FIFO, dataLen - 8);
                pkt->neighbor = neighborPointer;
                TCPIP_ICMPV6_Flush(pkt);
            }
            else if ((addrType == IPV6_ADDR_TYPE_MULTICAST) || (addrType == IPV6_ADDR_TYPE_SOLICITED_NODE_MULTICAST))
            {
                localAddressPointer = TCPIP_IPV6_DASSourceAddressSelect (pNetIf, remoteIP, NULL);
                if (localAddressPointer == NULL)
                    return;

                pkt = TCPIP_ICMPV6_PutHeaderEchoReply (pNetIf, &localIPStruct->address, remoteIP, ICMPV6_INFO_ECHO_REPLY, h.header_Echo.identifier, h.header_Echo.sequenceNumber);
                if (pkt == NULL)
                    return;

                if (TCPIP_IPV6_TxIsPutReady(pkt, dataLen - 8) < (dataLen - 8))
                {
                    pRxPkt->pktClientData = TCPIP_MAC_PKT_ACK_ALLOC_ERR;
                    TCPIP_IPV6_PacketFree (pkt);
                    return;
                }

                TCPIP_IPV6_ArrayPutHelper(pkt, pRxPkt, IPV6_DATA_NETWORK_FIFO, dataLen - 8);
                pkt->neighbor = neighborPointer;
                TCPIP_ICMPV6_Flush(pkt);
            }
            break;
#endif
        case ICMPV6_INFO_ECHO_REPLY:
            _ICMPV6_NotifyClients(pNetIf, ICMPV6_INFO_ECHO_REPLY, localIP, remoteIP, &h.header_Echo);

            // The Echo Request/Reply indicates successfult round-trip communication.
            // Indicate to the NDP module that this remote node is reachable.
            TCPIP_NDP_NborReachConfirm (pNetIf, remoteIP);
            break;
        case ICMPV6_INFO_ROUTER_SOLICITATION:

            break;
        case ICMPV6_INFO_ROUTER_ADVERTISEMENT:
            tempAddressType = TCPIP_IPV6_AddressTypeGet (pNetIf, remoteIP);
            if ((tempAddressType.bits.scope != IPV6_ADDR_SCOPE_LINK_LOCAL) ||
                (hopLimit != 255) || (h.header_RA.vCode != 0) || (dataLen <= 16))
                return;

            // Initialize all option structures to 0
            llaOption.vType = 0;
            prefixOption.vType = 0;
            mtuOption.vType = 0;

            // Process options
            // Ensure there aren't any options with length of 0
            TCPIP_IPV6_RxBufferSet (pRxPkt, headerLen + sizeof (h.header_RA));
            tempDataLen = dataLen - sizeof (h.header_RA);
            while (tempDataLen)
            {
                TCPIP_IPV6_ArrayGet(pRxPkt, mData, 2);
                i = mData[0];
                j = (uint16_t)mData[1] << 3;

                if (j == 0)
                    return;

                tempDataLen -= j;
                j -= 2;

                switch (i)
                {
                    case NDP_OPTION_TYPE_LLA_SOURCE:
                        if (j != 6)
                            return;
                        llaOption.vType = NDP_OPTION_TYPE_LLA_SOURCE;
                        TCPIP_IPV6_ArrayGet (pRxPkt, (uint8_t *)&llaOption.mLinkLayerAddr, sizeof (TCPIP_MAC_ADDR));
                        break;
                    case NDP_OPTION_TYPE_PREFIX_INFO:
                        if (j != sizeof (NDP_OPTION_PREFIX_INFO) - 2)
                            return;

                        // Indicate that a prefix option is present, but process it later (in case there are
                        // multiple prefix options)
                        prefixOption.vType = NDP_OPTION_TYPE_PREFIX_INFO;

                        TCPIP_IPV6_ArrayGet (pRxPkt, (uint8_t *)&prefixOption.vPrefixLen, sizeof (NDP_OPTION_PREFIX_INFO) - 2);

                        prefixOption.vType = NDP_OPTION_TYPE_PREFIX_INFO;
                        prefixOption.vLength = j+2;

                        prefixOption.dValidLifetime = TCPIP_Helper_ntohl (prefixOption.dValidLifetime);
                        prefixOption.dPreferredLifetime = TCPIP_Helper_ntohl (prefixOption.dPreferredLifetime);
                        break;
                    case NDP_OPTION_TYPE_MTU:
                        if (j != 6)
                            return;
                        mtuOption.vType = NDP_OPTION_TYPE_MTU;
                        // Read the two reserved bytes and  the 4-byte MTU
                        TCPIP_IPV6_ArrayGet (pRxPkt, (uint8_t *)&mtuOption.wReserved, 6);
                        mtuOption.dMTU = TCPIP_Helper_ntohl (mtuOption.dMTU);
                        break;
                    default:
                        TCPIP_IPV6_SetReadPtr(pRxPkt, (uint8_t *)TCPIP_IPV6_GetReadPtrInRx(pRxPkt) + j);
                        break;
                }
            }

            TCPIP_NDP_RouterSolicitStop(pNetIf);

            TCPIP_IPV6_RxBufferSet (pRxPkt, headerLen);

            routerPointer = TCPIP_NDP_RemoteNodeFind (pNetIf, remoteIP, IPV6_HEAP_NDP_DR_ID);
            if (routerPointer)
                neighborPointer = routerPointer->neighborInfo;

            // Process the router advertisement's current hop limit field
            if (h.header_RA.curHopLimit != 0)
            {
                pIpv6Config->curHopLimit = h.header_RA.curHopLimit;
            }

            // Process the router advertisement's reachable time field
            if (h.header_RA.reachableTime != 0)
            {
                if (pIpv6Config->baseReachableTime != h.header_RA.reachableTime)
                {
                    pIpv6Config->reachableTime = ((h.header_RA.reachableTime * ((SYS_RANDOM_PseudoGet() & 0xFF) + 128)) >> 8) / 1000;
                }
                pIpv6Config->baseReachableTime = h.header_RA.reachableTime;
            }

            // Process the router advertisement's retransmission time field
            if (h.header_RA.retransTime != 0)
            {
                pIpv6Config->retransmitTime = h.header_RA.retransTime;
            }

            // Router found
            if (h.header_RA.routerLifetime)
            {
                if (routerPointer == NULL)
                {
                    neighborPointer = TCPIP_NDP_RemoteNodeFind (pNetIf, remoteIP, IPV6_HEAP_NDP_NC_ID);
                    if (neighborPointer)
                    {
                        if (llaOption.vType)
                        {
                            // If the link layer address is different, update it
                            if (memcmp(&neighborPointer->remoteMACAddr, &llaOption.mLinkLayerAddr, sizeof (TCPIP_MAC_ADDR)))
                                TCPIP_NDP_NborCacheLinkLayerAddressUpdate (pNetIf, neighborPointer, &llaOption.mLinkLayerAddr, NDP_STATE_STALE);
                        }
                    }
                    else
                    {
                        if (llaOption.vType)
                            neighborPointer = TCPIP_NDP_NborEntryCreate (pNetIf, remoteIP, &llaOption.mLinkLayerAddr, NDP_STATE_STALE, true, localIPStruct);
                        else
                            neighborPointer = TCPIP_NDP_NborEntryCreate (pNetIf, remoteIP, NULL, NDP_STATE_NONE, true, localIPStruct);
                    }
                    if (neighborPointer == NULL)
                    {
                        return;
                    }
                    routerPointer = TCPIP_NDP_DefaultRouterEntryCreate (pNetIf, neighborPointer, h.header_RA.routerLifetime);
                    if (routerPointer == NULL)
                    {
                        return;
                    }
                }

                // Set Invalidation Timer to new Router lifetime
                routerPointer->invalidationTimer = h.header_RA.routerLifetime;
                routerPointer->tickTimer = SYS_TMR_TickCountGet();
            }
            else
            {
                // Remove the default router node; leave the neighbor cache node.  This will update destination cache entries that use this router.
                if (routerPointer != NULL)
                {
                    TCPIP_NDP_LinkedListEntryRemove (pNetIf, routerPointer, IPV6_HEAP_NDP_DR_ID);
                    routerPointer = NULL;
                }
            }

            // If there's not a default router, at least see if there's a neighbor cache entry
            if (routerPointer == NULL)
            {
                if (neighborPointer == NULL)
                    neighborPointer = TCPIP_NDP_RemoteNodeFind (pNetIf, remoteIP, IPV6_HEAP_NDP_NC_ID);
            }
            else
            {
                neighborPointer = routerPointer->neighborInfo;
            }

            // Set the bIsRouter flag for any default router or neighbor with this address to true
            if (neighborPointer != NULL)
                neighborPointer->flags.bIsRouter = 1;

            if (llaOption.vType)
            {
                if (neighborPointer == NULL)
                {
                    // If there's no index by this point, we RX'ed a neighbor advertisement from a previously unknown router
                    // with lifetime set to 0.  If there's an SLLA we should at least create a neighbor cache entry for it.
                    neighborPointer = TCPIP_NDP_NborEntryCreate (pNetIf, remoteIP, &llaOption.mLinkLayerAddr, NDP_STATE_STALE, true, localIPStruct);
                }
                else
                {
                    // Neighbor Cache or Default Router entry exists
                    // If we have a new link-layer address, update it and set reachability to STALE
                    // The bIsRouter flag will already have been set to true
                    if (memcmp (&neighborPointer->remoteMACAddr, &llaOption.mLinkLayerAddr, sizeof (TCPIP_MAC_ADDR)))
                    {
                        TCPIP_NDP_NborCacheLinkLayerAddressUpdate (pNetIf, neighborPointer, &llaOption.mLinkLayerAddr, NDP_STATE_STALE);
                    }
                }
            }

            if (prefixOption.vType)
            {
                // Go through the options again to find all of the prefix options
                TCPIP_IPV6_RxBufferSet (pRxPkt, headerLen + sizeof (h.header_RA));
                tempDataLen = dataLen - sizeof (h.header_RA);
                while (tempDataLen)
                {
                    TCPIP_IPV6_ArrayGet(pRxPkt, mData, 2);
                    i = mData[0];
                    j = (uint16_t)mData[1] << 3;

                    tempDataLen -= j;
                    j -= 2;

                    if (i == NDP_OPTION_TYPE_PREFIX_INFO)
                    {
                        TCPIP_IPV6_ArrayGet (pRxPkt, (uint8_t *)&prefixOption.vPrefixLen, sizeof (NDP_OPTION_PREFIX_INFO) - 2);

                        prefixOption.vType = NDP_OPTION_TYPE_PREFIX_INFO;
                        prefixOption.vLength = j+2;

                        prefixOption.dValidLifetime = TCPIP_Helper_ntohl (prefixOption.dValidLifetime);
                        prefixOption.dPreferredLifetime = TCPIP_Helper_ntohl (prefixOption.dPreferredLifetime);

                        // If the prefix is an on-link prefix, add it to the prefix list
                        TCPIP_NDP_PrefixInfoProcessForOnLinkStatus (pNetIf, &prefixOption);
                        // If the prefix can be used for Stateless Address Autoconfiguration, create
                        // an address with it
                        TCPIP_NDP_SAAPrefixInfoProcess (pNetIf, &prefixOption);
                    }
                    else
                    {
                        TCPIP_IPV6_SetReadPtr(pRxPkt, (uint8_t *)TCPIP_IPV6_GetReadPtrInRx(pRxPkt) + j);
                    }
                }
            }

            if (mtuOption.vType)
            {
                if ((mtuOption.dMTU < TCPIP_IPV6_DEFAULT_LINK_MTU) && (mtuOption.dMTU >= TCPIP_IPV6_MINIMUM_LINK_MTU))
                {
                    pIpv6Config->linkMTU = mtuOption.dMTU;
                    pIpv6Config->mtuIncreaseTimer = SYS_TMR_TickCountGet() + (SYS_TMR_TickCounterFrequencyGet() * TCPIP_IPV6_MTU_INCREASE_TIMEOUT);

                    // Make sure that all existing destination cache nodes are updated to use this MTU if they use this router.
                    // New nodes will get their MTU from pIpv6Config->linkMTU.
                    destinationPointer = (IPV6_HEAP_NDP_DC_ENTRY *)pIpv6Config->listDestinationCache.head;
                    while (destinationPointer != NULL)
                    {
                        if (destinationPointer->nextHopNeighbor == neighborPointer)
                        {
                            if (mtuOption.dMTU <= destinationPointer->pathMTU)
                            {
                                destinationPointer->pathMTU = mtuOption.dMTU;
                                destinationPointer->pathMTUIncreaseTimer = pIpv6Config->mtuIncreaseTimer;
                            }
                        }
                        destinationPointer = destinationPointer->next;
                    }
                }
            }

            break;
        case ICMPV6_INFO_NEIGHBOR_SOLICITATION:
            // Check for packet validity
            tempAddressType = TCPIP_IPV6_AddressTypeGet (pNetIf, &h.header_NS.aTargetAddress);
            if ((tempAddressType.bits.type == IPV6_ADDR_TYPE_MULTICAST) ||
                (hopLimit != 255) || (h.header_NS.vCode != 0) || (dataLen < 24))
                return;

            // Try to get the SLLA option and verify that there are no options with len == 0
            TCPIP_IPV6_RxBufferSet (pRxPkt, headerLen + sizeof (h.header_NS));
            tempDataLen = dataLen - sizeof (h.header_NS);
            llaOption.vType = 0;
            while (tempDataLen)
            {
                TCPIP_IPV6_ArrayGet(pRxPkt, mData, 2);
                i = mData[0];
                j = (uint16_t)mData[1];

                if (j == 0)
                    return;
                j = j << 3;

                tempDataLen -= j;
                j -= 2;

                switch (i)
                {
                    case NDP_OPTION_TYPE_LLA_SOURCE:
                        if (j != 6)
                            return;
                        llaOption.vType = NDP_OPTION_TYPE_LLA_SOURCE;
                        llaOption.vLength = 1;
                        TCPIP_IPV6_ArrayGet (pRxPkt, (uint8_t *)&llaOption.mLinkLayerAddr, sizeof (TCPIP_MAC_ADDR));
                        break;
                    default:
                        TCPIP_IPV6_SetReadPtr(pRxPkt, (uint8_t *)TCPIP_IPV6_GetReadPtrInRx(pRxPkt) + j);
                        break;
                }
            }

            i = memcmp (&IPV6_FIXED_ADDR_UNSPECIFIED, remoteIP, sizeof (IPV6_ADDR));

            // If the packet's source address is the unspecified address, verify that the destination is
            // a solicited node multicast address and that there's no SLLA option
            if (i == 0)
            {
                // If the source address is the unspecified address, this packet must be discarded if the destination is not
                // a solicited-node multicast address or if there is a SLLA option
                if (llaOption.vType == NDP_OPTION_TYPE_LLA_SOURCE)
                    return;

                if (!TCPIP_IPV6_AddressIsSolicitedNodeMulticast (localIP))
                    return;
            }

            // If the target address is tentative, process it using stateless address autoconfiguration rules
            if ((localAddressPointer = TCPIP_IPV6_AddressFind (pNetIf, &h.header_NS.aTargetAddress, IPV6_ADDR_TYPE_UNICAST_TENTATIVE)) != NULL)
            {
                // If the source address is the unspecified address, process this NS message as a DAD event (otherwise it is
                // being used for address resolution)
                if (i == 0)
                    TCPIP_NDP_DupAddrDiscoveryProcess (localAddressPointer, IPV6_NDP_DAD_NS_RECEIVED);
                return;
            }

            // Even though the packet was accepted by our filters, the target address isn't necessarily an
            // address used by our interface.  Check to see if it's a valid target.
            if ((localAddressPointer = TCPIP_IPV6_AddressFind (pNetIf, &h.header_NS.aTargetAddress, IPV6_ADDR_TYPE_UNICAST)) == NULL)
            {
                return;
            }

            // Check to see if the source address was the unspecified address
            if (i != 0)
            {
                neighborPointer = TCPIP_NDP_RemoteNodeFind (pNetIf, remoteIP, IPV6_HEAP_NDP_NC_ID);
                if (llaOption.vType == NDP_OPTION_TYPE_LLA_SOURCE)
                {
                    // If the source address of the received Neighbor Solicitation isn't the unspecified address and
                    // there is a valid source link layer option, update the neighbor cache.
                    if (neighborPointer == NULL)
                    {
                        neighborPointer = TCPIP_NDP_NborEntryCreate (pNetIf, remoteIP, &llaOption.mLinkLayerAddr, NDP_STATE_STALE, false, localIPStruct);
                        if (neighborPointer == NULL)
                            return;
                    }
                    else
                    {
                        if (memcmp (&neighborPointer->remoteMACAddr, &llaOption.mLinkLayerAddr, sizeof (TCPIP_MAC_ADDR)))
                        {
                            TCPIP_NDP_NborCacheLinkLayerAddressUpdate (pNetIf, neighborPointer, &llaOption.mLinkLayerAddr, NDP_STATE_STALE);
                        }
                    }
                }
            }
            else
            {
                // Send a Neighbor Advertisement to the All-nodes multicast address
                pkt = TCPIP_ICMPV6_HeaderNeighborAdvertisementPut (pNetIf, &localAddressPointer->address, (IPV6_ADDR *)&IPV6_FIXED_ADDR_ALL_NODES_MULTICAST,
                                            &h.header_NS.aTargetAddress, false, true);
                if (pkt == NULL)
                    return;

                llaOption.vType = NDP_OPTION_TYPE_LLA_TARGET;
                llaOption.vLength = 1;
                memcpy(&llaOption.mLinkLayerAddr , &pNetIf->netMACAddr, sizeof (TCPIP_MAC_ADDR));
                if (TCPIP_IPV6_TxIsPutReady(pkt, sizeof(NDP_OPTION_LLA)))
                {
                    TCPIP_IPV6_PutArray(pkt, (uint8_t*)&llaOption, sizeof(NDP_OPTION_LLA));
                }
                else
                {
                    TCPIP_IPV6_PacketFree (pkt);
                    return;
                }

                TCPIP_ICMPV6_Flush (pkt);
                return;
            }

            if (neighborPointer == NULL)
                return;

            // Send a Neighbor Advertisement to the soliciting node
            pkt = TCPIP_ICMPV6_HeaderNeighborAdvertisementPut (pNetIf, &localAddressPointer->address, &neighborPointer->remoteIPAddress,
                                        &h.header_NS.aTargetAddress, true, true);

            llaOption.vType = NDP_OPTION_TYPE_LLA_TARGET;
            llaOption.vLength = 1;
            memcpy(&llaOption.mLinkLayerAddr , &pNetIf->netMACAddr, sizeof (TCPIP_MAC_ADDR));
            if (TCPIP_IPV6_TxIsPutReady(pkt, sizeof(NDP_OPTION_LLA)))
            {
                TCPIP_IPV6_PutArray(pkt, (uint8_t*)&llaOption, sizeof(NDP_OPTION_LLA));
            }
            else
            {
                TCPIP_IPV6_PacketFree (pkt);
                return;
            }

            // If the reachability state of the neighbor is INCOMPLETE the Flush function will cache the NA and try to resolve the address.
            // This should happen rarely, because devices should rarely send Neighbor Solicitations with no SLLA option.
            pkt->neighbor = neighborPointer;
            TCPIP_ICMPV6_Flush (pkt);
            break;
        case ICMPV6_INFO_NEIGHBOR_ADVERTISEMENT:
            // Check packet for validity
            tempAddressType = TCPIP_IPV6_AddressTypeGet (pNetIf, &h.header_NA.aTargetAddress);
            if ((tempAddressType.bits.type == IPV6_ADDR_TYPE_MULTICAST) ||
                (hopLimit != 255) || (h.header_NA.vCode != 0) || (dataLen < 24))
                return;

            if ((addrType == IPV6_ADDR_TYPE_MULTICAST) && (h.header_NA.flags.bits.S == 1))
                return;

            // Retrieve options.
            // Target Link-layer option is the only valid option for Neighbor Advertisements.
            TCPIP_IPV6_RxBufferSet (pRxPkt, headerLen + sizeof (h.header_NA));
            tempDataLen = dataLen - sizeof (h.header_NA);
            llaOption.vType = 0;
            while (tempDataLen)
            {
                TCPIP_IPV6_ArrayGet(pRxPkt, mData, 2);
                i = mData[0];
                j = (uint16_t)mData[1];

                if (j == 0)
                    return;

                j = j << 3;

                tempDataLen -= j;
                j -= 2;

                switch (i)
                {
                    case NDP_OPTION_TYPE_LLA_TARGET:
                        if (j != 6)
                            return;
                        llaOption.vType = NDP_OPTION_TYPE_LLA_TARGET;
                        llaOption.vLength = 1;
                        TCPIP_IPV6_ArrayGet (pRxPkt, (uint8_t *)&llaOption.mLinkLayerAddr, sizeof (TCPIP_MAC_ADDR));
                        break;
                    default:
                        TCPIP_IPV6_SetReadPtr(pRxPkt, (uint8_t *)TCPIP_IPV6_GetReadPtrInRx(pRxPkt) + j);
                        break;
                }
            }

            // Discard the packet if its destination address was a multicast and the S flag is non-zero
            if (((addrType == IPV6_ADDR_TYPE_MULTICAST) || (addrType == IPV6_ADDR_TYPE_SOLICITED_NODE_MULTICAST)) && h.header_NA.flags.bits.S)
                return;

            if ((localAddressPointer = TCPIP_IPV6_AddressFind (pNetIf, &h.header_NA.aTargetAddress, IPV6_ADDR_TYPE_UNICAST_TENTATIVE)) != NULL)
            {
                TCPIP_NDP_DupAddrDiscoveryProcess (localAddressPointer, IPV6_NDP_DAD_NA_RECEIVED);
                return;
            }
            if ((localAddressPointer = TCPIP_IPV6_AddressFind (pNetIf, &h.header_NA.aTargetAddress, IPV6_ADDR_TYPE_UNICAST)) != NULL)
            {
                return;
            }

            // Determine if the target address has an entry in the Neighbor Cache.
            // There's no point in storing it if we weren't the one soliciting it.
            if ((neighborPointer = TCPIP_NDP_RemoteNodeFind (pNetIf, &h.header_NA.aTargetAddress, IPV6_HEAP_NDP_NC_ID)) == NULL)
            {
                return;
            }

            if ((neighborPointer->reachabilityState == NDP_STATE_INCOMPLETE) || (neighborPointer->reachabilityState == NDP_STATE_NONE))
            {
                if (llaOption.vType == NDP_OPTION_TYPE_LLA_TARGET)
                {
                    if (h.header_NA.flags.bits.S)
                    {
                        TCPIP_NDP_NborCacheLinkLayerAddressUpdate (pNetIf, neighborPointer, &llaOption.mLinkLayerAddr, NDP_STATE_REACHABLE);
                    }
                    else
                    {
                        TCPIP_NDP_NborCacheLinkLayerAddressUpdate (pNetIf, neighborPointer, &llaOption.mLinkLayerAddr, NDP_STATE_STALE);
                    }
                    if (h.header_NA.flags.bits.R)
                    {
                        neighborPointer->flags.bIsRouter = 1;
                    }
                    else
                    {
                        neighborPointer->flags.bIsRouter = 0;
                    }
                }
            }
            else
            {
                if (llaOption.vType == NDP_OPTION_TYPE_LLA_TARGET)
                {
                    tempDataLen = memcmp (&neighborPointer->remoteMACAddr, &llaOption.mLinkLayerAddr, sizeof (TCPIP_MAC_ADDR));
                }

                if ((h.header_NA.flags.bits.O == 0) && (llaOption.vType == NDP_OPTION_TYPE_LLA_TARGET) && (tempDataLen != 0))
                {
                    // If the override flag is clear and the supplied link-layer address differs from that in the cache
                    // then (a) if the state of the entry is REACHABLE, set it to STALE but do not update the entry, (b)
                    // ignore the advertisement
                    if (neighborPointer->reachabilityState == NDP_STATE_REACHABLE)
                    {
                        TCPIP_NDP_ReachabilitySet (pNetIf, neighborPointer, NDP_STATE_STALE);
                    }
                    break;
                }

                // If we get to here, either the override flag was set, the supplied link-layer address was the same as
                // the address in the cache, or no TLLA option was specified

                // Insert the link-layer address in the cache
                if ((llaOption.vType == NDP_OPTION_TYPE_LLA_TARGET) && (tempDataLen != 0))
                {
                    memcpy (&neighborPointer->remoteMACAddr, &llaOption.mLinkLayerAddr, sizeof (TCPIP_MAC_ADDR));
                }

                // If the solicited flag is set, set the entry to reachable
                if (h.header_NA.flags.bits.S)
                {
                    TCPIP_NDP_ReachabilitySet (pNetIf, neighborPointer, NDP_STATE_REACHABLE);
                }
                else
                {
                    // If S is clear, and the LLA address was updated, set the address to STALE
                    if ((llaOption.vType == NDP_OPTION_TYPE_LLA_TARGET) && (tempDataLen != 0))
                        TCPIP_NDP_ReachabilitySet (pNetIf, neighborPointer, NDP_STATE_STALE);
                }

                if (h.header_NA.flags.bits.R ^ neighborPointer->flags.bIsRouter)
                {
                    neighborPointer->flags.bIsRouter = h.header_NA.flags.bits.R;
                    if (!neighborPointer->flags.bIsRouter)
                    {
                        if ((routerPointer = TCPIP_NDP_RemoteNodeFind (pNetIf, &h.header_NA.aTargetAddress, IPV6_HEAP_NDP_DR_ID)) != NULL)
                        {
                            // Remote this router from the default router list (this will update all Destination Cache entries that use it as a router)
                            TCPIP_NDP_LinkedListEntryRemove (pNetIf, routerPointer, IPV6_HEAP_NDP_DR_ID);
                        }
                    }
                }
            }
            break;
        case ICMPV6_INFO_REDIRECT:
            // Check for packet validity
            tempAddressType = TCPIP_IPV6_AddressTypeGet (pNetIf, remoteIP);
            if (tempAddressType.bits.scope != IPV6_ADDR_SCOPE_LINK_LOCAL)
                return;
            if ((hopLimit != 255) || (h.header_Rd.vCode != 0) || (dataLen < 40))
                return;

            // Verify that the destination address is not a multicast address
            if (h.header_Rd.aDestinationAddress.v[0] == 0xFF)
                return;

            // Verify that the target address is a link-local address (when redirected to a router) or
            // is the same as the Destination Address (when the target itself is actually link-local).
            // The RFC specifies that the target address in a Redirect header is ALWAYS considered link-local,
            // even if the prefix isn't explicitly known to be link-local, so the only real test we can apply
            // here is to check to see if the target address is a non-link-local multicast address.
            if (memcmp (&h.header_Rd.aTargetAddress, &h.header_Rd.aDestinationAddress, sizeof (IPV6_ADDR)) != 0)
            {
                tempAddressType = TCPIP_IPV6_AddressTypeGet (pNetIf, &h.header_Rd.aTargetAddress);
                if (tempAddressType.bits.type != IPV6_ADDR_TYPE_UNICAST)
                    return;
                // Don't check the scope - we must assume that the target address is link-local
//                if (tempAddressType.bits.scope != IPV6_ADDR_SCOPE_LINK_LOCAL)
//                    return;
            }

            // Determine if IP source address of the redirect message is the same as the first-hop router
            // for the specified ICMP Destination Address
            neighborPointer = TCPIP_NDP_NextHopGet (pNetIf, &h.header_Rd.aDestinationAddress);
            if (neighborPointer == NULL)
                return;
            if (memcmp (remoteIP, &neighborPointer->remoteIPAddress, sizeof (IPV6_ADDR)) != 0)
                return;

            // Retrieve options.
            // Target Link-layer option and redirected header options are the only valid options for Redirect messages
            TCPIP_IPV6_RxBufferSet (pRxPkt, headerLen + sizeof (h.header_Rd));
            tempDataLen = dataLen - sizeof (h.header_Rd);
            llaOption.vType = 0;
            while (tempDataLen)
            {
                TCPIP_IPV6_ArrayGet(pRxPkt, mData, 2);
                i = mData[0];
                j = (uint16_t)mData[1];

                // Discard the packet if any of the option lengths are zero
                if (j == 0)
                    return;

                j = j << 3;

                tempDataLen -= j;
                j -= 2;

                switch (i)
                {
                    case NDP_OPTION_TYPE_LLA_TARGET:
                        if (j != 6)
                            return;
                        llaOption.vType = NDP_OPTION_TYPE_LLA_TARGET;
                        llaOption.vLength = 1;
                        TCPIP_IPV6_ArrayGet (pRxPkt, (uint8_t *)&llaOption.mLinkLayerAddr, sizeof (TCPIP_MAC_ADDR));
                        break;
                    // If we get a Redirected header just skip it for now
                    case NDP_OPTION_TYPE_REDIRECT:
                    default:
                        TCPIP_IPV6_SetReadPtr(pRxPkt, (uint8_t *)TCPIP_IPV6_GetReadPtrInRx(pRxPkt) + j);
                        break;
                }
            }

            i = false;

            // If the target address is not the same as the destination address we must treat the target address as a router
            if (memcmp (&h.header_Rd.aTargetAddress, &h.header_Rd.aDestinationAddress, sizeof (IPV6_ADDR)) != 0)
            {
                i = true;
            }

            neighborPointer = (IPV6_HEAP_NDP_NC_ENTRY *)TCPIP_NDP_RemoteNodeFind (pNetIf, &h.header_Rd.aTargetAddress, IPV6_HEAP_NDP_NC_ID);
            if (neighborPointer == NULL)
            {
                if (llaOption.vType == NDP_OPTION_TYPE_LLA_TARGET)
                {
                    neighborPointer = TCPIP_NDP_NborEntryCreate (pNetIf, &h.header_Rd.aTargetAddress, &llaOption.mLinkLayerAddr, NDP_STATE_STALE, i, localIPStruct);
                }
                else
                {
                    neighborPointer = TCPIP_NDP_NborEntryCreate (pNetIf, &h.header_Rd.aTargetAddress, NULL, NDP_STATE_NONE, i, localIPStruct);
                }
                if (neighborPointer == NULL)
                    return;
            }
            else
            {
                if (i)
                {
                    neighborPointer->flags.bIsRouter = true;
                }

                if (llaOption.vType == NDP_OPTION_TYPE_LLA_TARGET)
                {
                    // If the address in the TLLA option doesn't match the address in the neighbor cache entry,
                    // set the reachability state to stale.
                    if (memcmp (&neighborPointer->remoteMACAddr, &llaOption.mLinkLayerAddr, sizeof (TCPIP_MAC_ADDR)))
                    {
                        TCPIP_NDP_NborCacheLinkLayerAddressUpdate (pNetIf, neighborPointer, &llaOption.mLinkLayerAddr, NDP_STATE_STALE);
                    }
                }
            }

            destinationPointer = (IPV6_HEAP_NDP_DC_ENTRY *)TCPIP_NDP_RemoteNodeFind (pNetIf, &h.header_Rd.aDestinationAddress, IPV6_HEAP_NDP_DC_ID);

            if (destinationPointer == NULL)
            {
                destinationPointer = TCPIP_NDP_DestCacheEntryCreate (pNetIf, &h.header_Rd.aDestinationAddress, pIpv6Config->linkMTU, neighborPointer);
            }
            else
            {
                destinationPointer->nextHopNeighbor = neighborPointer;
            }

            break;
    }
    return;
}


static void _ICMPV6_NotifyClients(TCPIP_NET_HANDLE hNetIf, uint8_t type, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, void * header)
{
    ICMPV6_LIST_NODE* dNode;

    TCPIP_Notification_Lock(&icmpv6RegisteredUsers);
    for(dNode = (ICMPV6_LIST_NODE*)icmpv6RegisteredUsers.list.head; dNode != 0; dNode = dNode->next)
    {
        (*dNode->callback)(hNetIf, type, localIP, remoteIP, header);
    }
    TCPIP_Notification_Unlock(&icmpv6RegisteredUsers);

}


#endif







