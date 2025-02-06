/*******************************************************************************
  Internet Control Message Protocol (ICMP) Server

  Summary:
    Module for Microchip TCP/IP Stack

  Description:
    - Provides "ping" diagnostics
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








#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_ICMPV6

#include "tcpip/src/tcpip_private.h"

#if defined TCPIP_STACK_USE_IPV6



static const void*      icmpv6MemH = NULL;        // memory handle
static int              icmpv6InitCount = 0;      // module initialization count

// Callback function for informing the upper-layer protocols about ICMPv6 events
typedef void (*icmpv6Callback) (TCPIP_NET_HANDLE hNetIf, uint8_t type, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, void * header);

static PROTECTED_SINGLE_LIST      icmpv6RegisteredUsers = { {0} };
//
// ICMPv6 callback registration
typedef struct  S_TAG_ICMPV6_LIST_NODE
{
    struct S_TAG_ICMPV6_LIST_NODE*   next;       // next node in list
                                                // makes it valid SGL_LIST_NODE node
    icmpv6Callback                  callback;   // handler to be called for ICMPV6 event
}ICMPV6_LIST_NODE;

// prototypes
//
static void F_ICMPV6_AckPacket (void * pktPointer, bool sent, const void * param);

static void F_ICMPV6_NotifyClients(TCPIP_NET_HANDLE hNetIf, uint8_t type, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, void * header);

// conversion functions/helpers
//
static __inline__ ICMPV6_LIST_NODE* __attribute__((always_inline)) FC_SglNode2Icmp6Node(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        ICMPV6_LIST_NODE* pIcmp6Node;
    }U_SGL_NODE_ICMP6_NODE;

    U_SGL_NODE_ICMP6_NODE.node = node;
    return U_SGL_NODE_ICMP6_NODE.pIcmp6Node;
}

static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_Icmp6Hndl2SglNode(ICMPV6_HANDLE hIcmpv6)
{
    union
    {
        ICMPV6_HANDLE hIcmpv6;
        SGL_LIST_NODE*  node;
    }U_HNDL_SGL_NODE;

    U_HNDL_SGL_NODE.hIcmpv6 = hIcmpv6;
    return U_HNDL_SGL_NODE.node;
}


static __inline__ IPV6_HEAP_NDP_DC_ENTRY* __attribute__((always_inline)) FC_SglNode2NdpDcEntry(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        IPV6_HEAP_NDP_DC_ENTRY* pNdpDcEntry;
    }U_SGL_NODE_NDP_DC_ENTRY;

    U_SGL_NODE_NDP_DC_ENTRY.node = node;
    return U_SGL_NODE_NDP_DC_ENTRY.pNdpDcEntry;
}

/*****************************************************************************
  Function:
    bool TCPIP_ICMPV6_Initialize (const TCPIP_STACK_MODULE_CTRL* const stackInit, const void* icmpv6Data)

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
bool TCPIP_ICMPV6_Initialize (const TCPIP_STACK_MODULE_CTRL* const stackInit, const void* initData)
{
    if(stackInit->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
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
        if(stackInit->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            TCPIP_Notification_Deinitialize(&icmpv6RegisteredUsers, icmpv6MemH);
            if(--icmpv6InitCount == 0)
            {   // all closed
                // release resources
                icmpv6MemH = NULL;
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
    if(callback != NULL && icmpv6MemH != NULL)
    {
        ICMPV6_LIST_NODE icmpNode;
        icmpNode.next = NULL;
        icmpNode.callback = callback;

        return FC_SglNode2Icmp6Node(TCPIP_Notification_Add(&icmpv6RegisteredUsers, icmpv6MemH, &icmpNode, sizeof(icmpNode)));
    }

    return NULL;

}


bool TCPIP_ICMPV6_CallbackDeregister(ICMPV6_HANDLE hIcmpv6)
{
    if(hIcmpv6 != NULL && icmpv6MemH != NULL)
    {
        if(TCPIP_Notification_Remove(FC_Icmp6Hndl2SglNode(hIcmpv6), &icmpv6RegisteredUsers, icmpv6MemH))
        {
            return true;
        }
    }

    return false;


}

bool TCPIP_ICMPV6_EchoRequestSend(TCPIP_NET_HANDLE netH, IPV6_ADDR * targetAddr, uint16_t sequenceNumber, uint16_t identifier, uint32_t packetSize)
{
    uint32_t  payload = 0x44332211U;
    IPV6_PACKET * pkt;
    IPV6_ADDR_STRUCT * localAddress;
    uint16_t    icmpPktSize = 0U;
    uint32_t    count=0;
    TCPIP_NET_IF  *pNetIf;

    if(netH != NULL)
    {
        pNetIf = TCPIPStackHandleToNetUp(netH);
    }
    else
    {
        pNetIf =  TCPIPStackAnyNetLinked(true);
    }

    localAddress = TCPIP_IPV6_DASSourceAddressSelect (pNetIf, targetAddr, NULL);

    if (localAddress == NULL)
    {
       //SYS_OUT_MESSAGE_LINE ("No local addr!", 1);
        return false;
    }

    const IPV6_ADDR* pLclAdd = FC_U8Ptr2CIp6Add(((uint8_t*)localAddress + offsetof(struct S_IPV6_ADDR_STRUCT, address)));
    pkt = TCPIP_ICMPV6_HeaderEchoRequestPut (pNetIf, pLclAdd, targetAddr, (uint8_t)ICMPV6_INFO_ECHO_REQUEST,
                                        identifier, sequenceNumber);

    if(packetSize == 0U)
    {
        icmpPktSize = 4U;
    }
    else if(packetSize >= 1280U)
    {
        icmpPktSize = 1600U;
    }
    else
    {
        // do nothing
    }

    if (TCPIP_IPV6_TxIsPutReady(pkt, icmpPktSize) < icmpPktSize)
    {
        TCPIP_IPV6_PacketFree (pkt);
        return false;
    }

    if(packetSize == 0U)
    {
        (void)TCPIP_IPV6_PutArray (pkt, (uint8_t *)&payload, (uint16_t)sizeof (uint32_t));
    }
    else
    {
        for(count = 0; count < (packetSize / 4U); count++)
        {
            (void)TCPIP_IPV6_PutArray (pkt, (uint8_t *)&payload, (uint16_t)sizeof(uint32_t));
        }
    }

    // Just let the IPv6 module figure out the next hop neighbor and its MAC address
    (void)TCPIP_ICMPV6_Flush (pkt);

    return true;
}

/*****************************************************************************
  Function:
    static void F_ICMPV6_AckPacket (void * pktPointer, bool sent, const void * param)

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
static void F_ICMPV6_AckPacket (void * pktPointer, bool sent, const void * param)
{
    if (pktPointer != NULL)
    {
        TCPIP_IPV6_PacketFree (pktPointer);
    }
}

/*****************************************************************************
  Function:
    IPV6_PACKET * TCPIP_ICMPV6_Open (const TCPIP_NET_IF * pNetIf, IPV6_ADDR * localIP,
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
IPV6_PACKET * TCPIP_ICMPV6_Open (const TCPIP_NET_IF * pNetIf, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP)
{
    IPV6_PACKET * pkt = TCPIP_IPV6_TxPacketAllocate(pNetIf, &F_ICMPV6_AckPacket, NULL);

    if (pkt == NULL)
    {
        return NULL;
    }

    TCPIP_IPV6_DestAddressSet(pkt,remoteIP);
    if (localIP != NULL)
    {
        if (localIP == &IPV6_FIXED_ADDR_UNSPECIFIED)
        {
            pkt->flags.useUnspecAddr = 1U;
            pkt->flags.sourceSpecified = 1U;
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
    IPV6_PACKET * TCPIP_ICMPV6_HeaderErrorPut (const TCPIP_NET_IF * pNetIf,
        const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, uint8_t code,
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
IPV6_PACKET * TCPIP_ICMPV6_HeaderErrorPut (const TCPIP_NET_IF * pNetIf, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, uint8_t code, uint8_t type, uint32_t additionalData)
{
    ICMPV6_HEADER_ERROR header;
    IPV6_PACKET * pkt;

    pkt = TCPIP_ICMPV6_Open (pNetIf, localIP, remoteIP);
    if (pkt == NULL)
    {
        return NULL;
    }

    header.vType = type;
    header.vCode = code;
    header.wChecksum = 0x0000;
    header.additionalData = additionalData;

    TCPIP_IPV6_HeaderPut(pkt, (uint8_t)IPV6_PROT_ICMPV6);

    TCPIP_IPV6_HopLimitSet(pkt, 255U);

    // Put the ICMPv6 Header
    if (TCPIP_IPV6_UpperLayerHeaderPut (pkt, (void *)&header, (uint16_t)sizeof (ICMPV6_HEADER_ERROR), (uint8_t)IPV6_PROT_ICMPV6, ICMPV6_CHECKSUM_OFFSET) == NULL)
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

    TCPIP_NET_IF * pNetIf = TCPIPStackHandleToNet(hNetIf);
    if (pNetIf == NULL)
    {
        return NULL;
    }

    pkt = TCPIP_ICMPV6_Open (pNetIf, localIP, remoteIP);
    if (pkt == NULL)
    {
        return NULL;
    }

    header.vType = type;
    header.vCode = (uint8_t)ICMPV6_INFO_EREQ_CODE;
    header.wChecksum = 0x0000;
    header.identifier = TCPIP_Helper_htons (identifier);
    header.sequenceNumber = TCPIP_Helper_htons (sequenceNumber);

    TCPIP_IPV6_HeaderPut(pkt, (uint8_t)IPV6_PROT_ICMPV6);

    // Put the ICMPv6 Header
    if (TCPIP_IPV6_UpperLayerHeaderPut (pkt, (void *)&header, (uint16_t)sizeof (ICMPV6_HEADER_ECHO), (uint8_t)IPV6_PROT_ICMPV6, ICMPV6_CHECKSUM_OFFSET) == NULL)
    {
        TCPIP_IPV6_PacketFree(pkt);
        return NULL;
    }

    return pkt;
}

/*****************************************************************************
  Function:
    IPV6_PACKET * TCPIP_ICMPV6_HeaderRouterSolicitationPut (TCPIP_NET_IF * pNetIf,
        const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP)

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
IPV6_PACKET * TCPIP_ICMPV6_HeaderRouterSolicitationPut (const TCPIP_NET_IF * pNetIf, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP)
{
    ICMPV6_HEADER_ROUTER_SOLICITATION header;
    IPV6_PACKET * pkt;

    pkt = TCPIP_ICMPV6_Open (pNetIf, localIP, remoteIP);
    if (pkt == NULL)
    {
        return NULL;
    }

    header.vType = (uint8_t)ICMPV6_INFO_ROUTER_SOLICITATION;
    header.vCode = 0U;
    header.wChecksum = 0x0000;
    header.Reserved = 0x00000000;

    TCPIP_IPV6_HeaderPut(pkt, (uint8_t)IPV6_PROT_ICMPV6);

    TCPIP_IPV6_HopLimitSet(pkt, 255U);

    // Put the ICMPv6 Header
    if (TCPIP_IPV6_UpperLayerHeaderPut (pkt, (void *)&header, (uint16_t)sizeof (ICMPV6_HEADER_ROUTER_SOLICITATION), (uint8_t)IPV6_PROT_ICMPV6, ICMPV6_CHECKSUM_OFFSET) == NULL)
    {
        TCPIP_IPV6_PacketFree(pkt);
        return NULL;
    }

    return pkt;
}

#if defined(TCPIP_IPV6_G3_PLC_BORDER_ROUTER) && (TCPIP_IPV6_G3_PLC_BORDER_ROUTER != 0)
// searches for proper address structures to be used for a G3-PLC router advertisement message
// populates pLclStruct with a local IP struct to be used (unicast, link local scope)
// populates pAdvStruct with a local IP struct to be used (unicast, global scope) 
void TCPIP_ICMPV6_G3AdvertisementSelect(const TCPIP_NET_IF * pNetIf, const IPV6_ADDR_STRUCT** pLclStruct, const IPV6_ADDR_STRUCT** pAdvStruct)
{   
    IPV6_ADDR_STRUCT* llNode;   // link local address of the interface
    IPV6_ADDR_STRUCT* advNode;  // address to be advertised
    IPV6_ADDR_STRUCT* addrNode; // node pointer

    IPV6_INTERFACE_CONFIG*  pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pNetIf);

    // iterate through the unicast list
    advNode = llNode = NULL;
    for(addrNode = FC_DblNode2AddStruct(pIpv6Config->listIpv6UnicastAddresses.head); addrNode != NULL; addrNode = addrNode->next)
    {
        if(addrNode->flags.type == (unsigned)IPV6_ADDR_TYPE_UNICAST)
        {
            if(addrNode->flags.scope == (unsigned)IPV6_ADDR_SCOPE_GLOBAL && advNode == NULL)
            {   // Note: advertise just the 1st unicast gbl address;
                advNode = addrNode;
            }
            else if(addrNode->flags.scope == (unsigned)IPV6_ADDR_SCOPE_LINK_LOCAL && llNode == NULL)
            {
                llNode = addrNode;
            }
            else
            {
                // do nothing
            }
        }

        if(advNode != NULL && llNode != NULL)
        {   // have all we need
            break;
        }
    }

    if(pLclStruct != NULL)
    {
        *pLclStruct = llNode;
    }
    if(pAdvStruct != NULL)
    {
        *pAdvStruct = advNode;
    }
}



/*****************************************************************************
  Function:
    bool TCPIP_ICMPV6_G3RouterAdvertisementPut (const TCPIP_NET_IF * pNetIf, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, const IPV6_ADDR_STRUCT* advIP)

  Summary:
    Transmits an ICMPv6 router advertisement packet.

  Description:
    The routine allocates a packet, sets the proper IPv6 Header, and upper-layer header for an ICMPv6
    router advertisement.
    It transmits the packet over the network.

  Precondition:
    None

  Parameters:
    pNetIf - The interface for the outgoing packet.
    localIP - The local address that should be used for the advertisement
    remoteIP - The packet's destination address
    advIP - the IPv6 address to be advertised
                Used fields:
                    IPV6_ADDR address;
                    prefixLen;

  Returns:
    true - if the call succeeded and th epacket transmitted over the network
    false - if the call failed (allocation error, not enough space for all options)

  Remarks:
         IP fields:
              Source Address MUST be the link-local address assigned to the interface from which this message is sent.
              Destination Address Typically the Source Address of an invoking Router Solicitation or the all-nodes multicast address.
              Hop Limit      255
         ICMP fields:
              Type           134
              Code           0
              Checksum       The ICMP checksum.
              Cur Hop Limit  8-bit unsigned integer.  The default value that should be placed in the Hop Count field of the IP
                             header for outgoing IP packets.  A value of zero means unspecified (by this router).
        
    
  ***************************************************************************/
bool TCPIP_ICMPV6_G3RouterAdvertisementPut (const TCPIP_NET_IF * pNetIf, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, const IPV6_ADDR_STRUCT* advIP)
{
    ICMPV6_HEADER_ROUTER_ADVERTISEMENT header;
    NDP_OPTION_PREFIX_INFO prefixOption;
    NDP_OPTION_6LoWPAN_CONTEXT wpanContext;
    IPV6_PACKET * pkt;

    pkt = TCPIP_ICMPV6_Open (pNetIf, localIP, remoteIP);
    if (pkt == NULL)
    {
        return false;
    }

    (void)memset(&header, 0, sizeof(header));
    header.vType = (uint8_t)ICMPV6_INFO_ROUTER_ADVERTISEMENT;
    
    header.routerLifetime = 0xffff; // max value allowed by RFC 6775

    TCPIP_IPV6_HeaderPut(pkt, (uint8_t)IPV6_PROT_ICMPV6);

    TCPIP_IPV6_HopLimitSet(pkt, 255U);

    // Put the ICMPv6 Header
    if (TCPIP_IPV6_UpperLayerHeaderPut (pkt, (void *)&header, (uint16_t)sizeof (ICMPV6_HEADER_ROUTER_ADVERTISEMENT), (uint8_t)IPV6_PROT_ICMPV6, ICMPV6_CHECKSUM_OFFSET) == NULL)
    {
        TCPIP_IPV6_PacketFree(pkt);
        return false;
    }

    if (TCPIP_IPV6_TxIsPutReady(pkt, (uint16_t)(sizeof(prefixOption) + sizeof(wpanContext)))!= 0U)
    {
        // set the prefixOption
        (void)memset(&prefixOption, 0, sizeof(prefixOption));
        prefixOption.vType = (uint8_t)NDP_OPTION_TYPE_PREFIX_INFO;
        prefixOption.vLength = sizeof(prefixOption) / 8U;     // == 4 for G3-PLC;
        prefixOption.vPrefixLen = advIP->prefixLen;
        prefixOption.flags.bA = 1;
        prefixOption.flags.bL = 1;
        prefixOption.dValidLifetime = 0xFFFFFFFFU;
        prefixOption.dPreferredLifetime = 0xFFFFFFFFU;
        (void)memcpy(prefixOption.aPrefix.v, advIP->address.v, ((size_t)advIP->prefixLen + 7U) / 8U);   // round up to M8

        // set the NDP_OPTION_6LoWPAN_CONTEXT
        (void)memset(&wpanContext, 0, sizeof(wpanContext));
        wpanContext.vType = (uint8_t)NDP_OPTION_TYPE_6LOWPAN_CONTEXT;
        wpanContext.vLength = (uint8_t)(sizeof(wpanContext) / 8U);     // == 2 for G3-PLC 
        wpanContext.vContextLen = advIP->prefixLen; 
        wpanContext.flags.bC = 1;
        // wpanContext.flags.bCID = 0;
        wpanContext.dValidLifetime = 0xFFFF;
        (void)memcpy(wpanContext.dContextPrefix, advIP->address.v, ((size_t)advIP->prefixLen + 7U) / 8U);   // round up to M8

        // set the options
        (void)TCPIP_IPV6_PutArray(pkt, (uint8_t*)&prefixOption, (uint16_t)sizeof(prefixOption));
        (void)TCPIP_IPV6_PutArray(pkt, (uint8_t*)&wpanContext, (uint16_t)sizeof(wpanContext));

        // done
        (void)TCPIP_ICMPV6_Flush (pkt);
        return true;
    }

    TCPIP_IPV6_PacketFree(pkt);
    return false;
}
#else
void TCPIP_ICMPV6_G3AdvertisementSelect(const TCPIP_NET_IF * pNetIf, const IPV6_ADDR_STRUCT** pLclStruct, const IPV6_ADDR_STRUCT** pAdvStruct)
{
    // unimplemented
}

bool TCPIP_ICMPV6_G3RouterAdvertisementPut (const TCPIP_NET_IF * pNetIf, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, const IPV6_ADDR_STRUCT* advIP)
{
    return false;
}
#endif  // defined(TCPIP_IPV6_G3_PLC_BORDER_ROUTER) && (TCPIP_IPV6_G3_PLC_BORDER_ROUTER != 0)

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
IPV6_PACKET * TCPIP_ICMPV6_HeaderNeighborSolicitationPut (const TCPIP_NET_IF * pNetIf, const IPV6_ADDR * localIP, IPV6_ADDR * remoteIP, IPV6_ADDR * targetAddr)
{
    ICMPV6_HEADER_NEIGHBOR_SOLICITATION header;
    IPV6_PACKET * pkt;

    pkt = TCPIP_ICMPV6_Open (pNetIf, localIP, remoteIP);
    if (pkt == NULL)
    {
        return NULL;
    }

    header.vType = (uint8_t)ICMPV6_INFO_NEIGHBOR_SOLICITATION;
    header.vCode = 0U;
    header.wChecksum = 0x0000;
    header.Reserved = 0x00000000;
    IPV6_ADDR* pDstAdd = FC_U8Ptr2Ip6Add(((uint8_t*)&header + offsetof(struct ICMPV6_HEADER_NEIGHBOR_SOLICITATION, aTargetAddress)));
    (void)memcpy (pDstAdd, targetAddr, sizeof (IPV6_ADDR));

    TCPIP_IPV6_HeaderPut(pkt, (uint8_t)IPV6_PROT_ICMPV6);

    TCPIP_IPV6_HopLimitSet(pkt, 255U);

    // Put the ICMPv6 Header
    if (TCPIP_IPV6_UpperLayerHeaderPut (pkt, (void *)&header, (uint16_t)sizeof (ICMPV6_HEADER_NEIGHBOR_SOLICITATION), (uint8_t)IPV6_PROT_ICMPV6, ICMPV6_CHECKSUM_OFFSET) == NULL)
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
IPV6_PACKET * TCPIP_ICMPV6_HeaderNeighborAdvertisementPut (const TCPIP_NET_IF * pNetIf, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, IPV6_ADDR * targetAddr, bool solicited, bool override)
{
    ICMPV6_HEADER_NEIGHBOR_ADVERTISEMENT header;
    IPV6_PACKET * pkt;

    pkt = TCPIP_ICMPV6_Open (pNetIf, localIP, remoteIP);
    if (pkt == NULL)
    {
        return NULL;
    }

    header.vType = (uint8_t)ICMPV6_INFO_NEIGHBOR_ADVERTISEMENT;
    header.vCode = 0U;
    header.wChecksum = 0x0000;
    // This will set the reserved bits and flags to 0
    header.flags.Val = 0;
    header.Reserved1 = 0;
    header.Reserved2 = 0;
    if (override)
    {
        header.flags.bits.O = 1;
    }

    if (solicited)
    {
        header.flags.bits.S = 1;
    }

    IPV6_ADDR* pDstAdd = FC_U8Ptr2Ip6Add(((uint8_t*)&header + offsetof(struct ICMPV6_HEADER_NEIGHBOR_ADVERTISEMENT, aTargetAddress)));
    (void)memcpy(pDstAdd, targetAddr, sizeof(IPV6_ADDR));

    TCPIP_IPV6_HeaderPut(pkt, (uint8_t)IPV6_PROT_ICMPV6);

    TCPIP_IPV6_HopLimitSet(pkt, 255U);

    // Put the ICMPv6 Header
    if (TCPIP_IPV6_UpperLayerHeaderPut (pkt, (void *)&header, (uint16_t)sizeof (ICMPV6_HEADER_NEIGHBOR_ADVERTISEMENT), (uint8_t)IPV6_PROT_ICMPV6, ICMPV6_CHECKSUM_OFFSET) == NULL)
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
    return TCPIP_IPV6_Flush(pkt) > 0;
}

/*****************************************************************************
  Function:
    static void F_ICMPv6_SwapHeader(ICMPV6_HEADER_TYPES * header)

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
static void F_ICMPv6_SwapHeader(ICMPV6_HEADER_TYPES * header)
{
    header->header_Error.wChecksum        = TCPIP_Helper_ntohs(header->header_Error.wChecksum);
    switch (header->header_Error.vType)
    {
        case (uint8_t)ICMPV6_INFO_ECHO_REQUEST:
        case (uint8_t)ICMPV6_INFO_ECHO_REPLY:
            header->header_Echo.identifier = TCPIP_Helper_ntohs (header->header_Echo.identifier);
            header->header_Echo.sequenceNumber = TCPIP_Helper_ntohs (header->header_Echo.sequenceNumber);
            break;
        case (uint8_t)ICMPV6_INFO_ROUTER_ADVERTISEMENT:
            header->header_RA.routerLifetime = TCPIP_Helper_ntohs (header->header_RA.routerLifetime);
            header->header_RA.retransTime = TCPIP_Helper_ntohl (header->header_RA.retransTime);
            header->header_RA.reachableTime = TCPIP_Helper_ntohl (header->header_RA.reachableTime);
            break;
        case (uint8_t)ICMPV6_ERROR_PACKET_TOO_BIG:
            header->header_Error.additionalData = TCPIP_Helper_ntohl (header->header_Error.additionalData);
            break;
        default:
            // do nothing
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
    Packet discard is done at the IPv6 layer with the acknowledge code in the pRxPkt->ipv6PktData !
  ***************************************************************************/
void TCPIP_ICMPV6_Process(const TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt, IPV6_ADDR_STRUCT * localIPStruct, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, uint16_t dataLen, uint16_t headerLen, uint8_t hopLimit, uint8_t addrType)
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
    const IPV6_ADDR* pLclAdd;
    IPV6_ADDR* pTargetAdd, *pDestAdd;
#if defined(TCPIP_IPV6_G3_PLC_SUPPORT) && (TCPIP_IPV6_G3_PLC_SUPPORT != 0)
    bool isG3Host;
#endif

    pRxPkt->ipv6PktData = (uint16_t)TCPIP_MAC_PKT_ACK_RX_OK;
    
    if (pNetIf == NULL)
    {
        return;
    }

    pIpv6Config = TCPIP_IPV6_InterfaceConfigGet(pNetIf);

    // Calculate checksums
    (void)memcpy (&pseudoHeader.SourceAddress, remoteIP, sizeof (IPV6_ADDR));
    (void)memcpy (&pseudoHeader.DestAddress, localIP, sizeof (IPV6_ADDR));
    // Total payload length is the length of data + extension headers
    pseudoHeader.PacketLength = TCPIP_Helper_ntohs(dataLen);
    pseudoHeader.zero1 = (unsigned short) 0;
    pseudoHeader.zero2 = (unsigned char) 0;
    pseudoHeader.NextHeader = (uint8_t)IPV6_PROT_ICMPV6;

    checksums.w[0] = ~TCPIP_Helper_CalcIPChecksum((uint8_t*)&pseudoHeader, (uint16_t)sizeof(pseudoHeader), 0);

    checksums.w[1] = TCPIP_Helper_CalcIPChecksum(pRxPkt->pNetLayer, dataLen, 0); 

    if(checksums.w[0] != checksums.w[1])
    {
        pRxPkt->ipv6PktData = (uint16_t) TCPIP_MAC_PKT_ACK_CHKSUM_ERR;
        return;
    }

    TCPIP_IPV6_RxBufferSet(pRxPkt, headerLen);

    (void)TCPIP_IPV6_Get(pRxPkt, &h.header_Error.vType);

    // Get any extra types
    switch (h.header_Error.vType)
    {
        case (uint8_t)ICMPV6_ERROR_DEST_UNREACHABLE:
        case (uint8_t)ICMPV6_ERROR_PACKET_TOO_BIG:
        case (uint8_t)ICMPV6_ERROR_TIME_EXCEEDED:
        case (uint8_t)ICMPV6_ERROR_PARAMETER_PROBLEM:
            (void)TCPIP_IPV6_ArrayGet(pRxPkt, (uint8_t *)&h + 1, (uint16_t)sizeof (h.header_Error) - 1U);
            break;

        case (uint8_t)ICMPV6_INFO_ECHO_REQUEST:
        case (uint8_t)ICMPV6_INFO_ECHO_REPLY:
            (void)TCPIP_IPV6_ArrayGet(pRxPkt, (uint8_t *)&h + 1, (uint16_t)sizeof (h.header_Echo) - 1U);
            break;

        case (uint8_t)ICMPV6_INFO_ROUTER_SOLICITATION:
            if (addrType == (uint8_t)IPV6_ADDR_TYPE_UNICAST_TENTATIVE)
            {
                return;
            }
            (void)TCPIP_IPV6_ArrayGet(pRxPkt, (uint8_t *)&h + 1, (uint16_t)sizeof (h.header_RS) - 1U);
            break;

        case (uint8_t)ICMPV6_INFO_ROUTER_ADVERTISEMENT:
            if (addrType == (uint8_t)IPV6_ADDR_TYPE_UNICAST_TENTATIVE)
            {
                return;
            }
            (void)TCPIP_IPV6_ArrayGet(pRxPkt, (uint8_t *)&h + 1, (uint16_t)sizeof (h.header_RA) - 1U);
            break;

        case (uint8_t)ICMPV6_INFO_NEIGHBOR_SOLICITATION:
        case (uint8_t)ICMPV6_INFO_NEIGHBOR_ADVERTISEMENT:
            (void)TCPIP_IPV6_ArrayGet(pRxPkt, (uint8_t *)&h + 1, (uint16_t)sizeof (h.header_NS) - 1U);
            break;

        case (uint8_t)ICMPV6_INFO_REDIRECT:
#if defined(TCPIP_IPV6_G3_PLC_SUPPORT) && (TCPIP_IPV6_G3_PLC_SUPPORT != 0)
            if((pNetIf->startFlags & (uint16_t)TCPIP_NETWORK_CONFIG_IPV6_G3_NET) != 0U)
            {   // unexpected for a G3-PLC network!
                TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
                return;
            }
#endif  // defined(TCPIP_IPV6_G3_PLC_SUPPORT) && (TCPIP_IPV6_G3_PLC_SUPPORT != 0)
            if (addrType == (uint8_t)IPV6_ADDR_TYPE_UNICAST_TENTATIVE)
            {
                return;
            }
            (void)TCPIP_IPV6_ArrayGet(pRxPkt, (uint8_t *)&h + 1, (uint16_t)sizeof (h.header_Rd) - 1U);
            break;

        default:
            // do nothing: unsupported header
            break;

    }

    F_ICMPv6_SwapHeader(&h);

    uint32_t sysFreq = SYS_TMR_TickCounterFrequencyGet();

    switch (h.header_Error.vType)
    {
        case (uint8_t)ICMPV6_ERROR_DEST_UNREACHABLE:
            F_ICMPV6_NotifyClients(pNetIf, (uint8_t)ICMPV6_ERROR_DEST_UNREACHABLE, localIP, remoteIP, &h.header_Error);
            break;

        case (uint8_t)ICMPV6_ERROR_PACKET_TOO_BIG:
            tempDataLen = dataLen - (uint16_t)sizeof (h.header_Error);
            if(dataLen < (uint16_t)sizeof(h.header_Error) || tempDataLen < (uint16_t)sizeof(IPV6_HEADER))
            // If the received packet doesn't contain the IPv6 header of the packet that caused the
            // error, we can't extract the destination address and update the destination cache entry's MTU
            {
                break;
            }

            // Use the pseudo-header to store the extracted destination address
            (void)TCPIP_IPV6_SetReadPtr(pRxPkt, (uint8_t *)TCPIP_IPV6_GetReadPtrInRx(pRxPkt) + IPV6_HEADER_OFFSET_DEST_ADDR);
            (void)TCPIP_IPV6_ArrayGet (pRxPkt, (uint8_t *)&pseudoHeader.DestAddress, (uint16_t)sizeof (IPV6_ADDR));

            if (pseudoHeader.DestAddress.v[0] == 0xFFU)
            {
                // Update the multicast MTU
                if (pIpv6Config->multicastMTU > h.header_Error.additionalData)
                {
                    // Set the multicast MTU to the new value.
                    // If the value is less than the minimum link MTU the flush function will handle it
                    pIpv6Config->multicastMTU = (uint16_t)h.header_Error.additionalData;
                    pIpv6Config->mtuIncreaseTimer = SYS_TMR_TickCountGet() + (sysFreq * (uint32_t)TCPIP_IPV6_MTU_INCREASE_TIMEOUT);
                }
            }
            else
            {
                destinationPointer = TCPIP_NDP_RemoteNodeFind (pNetIf, &pseudoHeader.DestAddress, (uint8_t)IPV6_HEAP_NDP_DC_ID);
                if (destinationPointer == NULL)
                {
                    // If we don't have a specific destination pointer for this node then update the link MTU.
                    // This should happen very rarely, since we should be creating destination cache entries
                    // for all outgoing packets.
                    if (pIpv6Config->linkMTU > h.header_Error.additionalData)
                    {
                        // Set the link MTU to the new value.
                        // If the value is less than the minimum link MTU the flush function will handle it
                        pIpv6Config->linkMTU = (uint16_t) h.header_Error.additionalData;
                        pIpv6Config->mtuIncreaseTimer = SYS_TMR_TickCountGet() + (sysFreq * (uint32_t)TCPIP_IPV6_MTU_INCREASE_TIMEOUT);
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
                        destinationPointer->pathMTU = (uint16_t)h.header_Error.additionalData;
                        destinationPointer->pathMTUIncreaseTimer = SYS_TMR_TickCountGet() + (sysFreq * (uint32_t)TCPIP_IPV6_MTU_INCREASE_TIMEOUT);
                    }
                }
            }
            if (icmpv6RegisteredUsers.list.head != NULL)
            {
                TCPIP_IPV6_RxBufferSet(pRxPkt, (uint16_t)sizeof (ICMPV6_HEADER_ERROR));
                F_ICMPV6_NotifyClients(pNetIf, (uint8_t)ICMPV6_ERROR_PACKET_TOO_BIG, localIP, remoteIP, &h.header_Error);
            }
            break;

        case (uint8_t)ICMPV6_ERROR_TIME_EXCEEDED:
            F_ICMPV6_NotifyClients(pNetIf, (uint8_t)ICMPV6_ERROR_TIME_EXCEEDED, localIP, remoteIP, &h.header_Error);
            break;

        case (uint8_t)ICMPV6_ERROR_PARAMETER_PROBLEM:
            F_ICMPV6_NotifyClients(pNetIf, (uint8_t)ICMPV6_ERROR_PARAMETER_PROBLEM, localIP, remoteIP, &h.header_Error);
            break;

#if defined TCPIP_STACK_USE_ICMPV6_SERVER
        case (uint8_t)ICMPV6_INFO_ECHO_REQUEST:
            if(dataLen < 8U)
            {   // ill formed?
                pRxPkt->ipv6PktData = (uint16_t) TCPIP_MAC_PKT_ACK_STRUCT_ERR;
                break;
            }

            F_ICMPV6_NotifyClients(pNetIf, (uint8_t)ICMPV6_INFO_ECHO_REQUEST, localIP, remoteIP, &h.header_Echo);

            TCPIP_IPV6_RxBufferSet (pRxPkt, (uint16_t)sizeof (ICMPV6_HEADER_ECHO) + headerLen);

#if defined(TCPIP_IPV6_G3_PLC_SUPPORT) && (TCPIP_IPV6_G3_PLC_SUPPORT != 0)
            isG3Host = TCPIP_NDP_IsG3PLC_Neighbor(pNetIf, remoteIP, NULL);
            if(!isG3Host)
            {
                neighborPointer = TCPIP_NDP_NextHopGet (pNetIf, remoteIP);
            }

            if (!isG3Host && neighborPointer == NULL)
            {   // couldn't find the neighbor
                break;
            }
#else
            neighborPointer = TCPIP_NDP_NextHopGet (pNetIf, remoteIP);
            if (neighborPointer == NULL)
            {
                break;
            }
#endif  // defined(TCPIP_IPV6_G3_PLC_SUPPORT) && (TCPIP_IPV6_G3_PLC_SUPPORT != 0)


            pLclAdd = FC_U8Ptr2CIp6Add(((uint8_t*)localIPStruct + offsetof(struct S_IPV6_ADDR_STRUCT, address)));
            if (addrType == (uint8_t)IPV6_ADDR_TYPE_UNICAST)
            {
                pkt = TCPIP_ICMPV6_PutHeaderEchoReply (pNetIf, pLclAdd, remoteIP, (uint8_t)ICMPV6_INFO_ECHO_REPLY, h.header_Echo.identifier, h.header_Echo.sequenceNumber);
                if (pkt == NULL)
                {
                    break;
                }

                if (TCPIP_IPV6_TxIsPutReady(pkt, dataLen - 8U) < (dataLen - 8U))
                {
                    pRxPkt->ipv6PktData = (uint16_t) TCPIP_MAC_PKT_ACK_ALLOC_ERR;
                    TCPIP_IPV6_PacketFree (pkt);
                    break;
                }

                (void)TCPIP_IPV6_ArrayPutHelper(pkt, pRxPkt, IPV6_DATA_NETWORK_FIFO, dataLen - 8U);
#if defined(TCPIP_IPV6_G3_PLC_SUPPORT) && (TCPIP_IPV6_G3_PLC_SUPPORT != 0)
                // for a G3-PLC host we do not have an NDP neighbor record
                // set always pkt->neighbor = 0, so it gets set in TCPIP_ICMPV6_Flush!
                pkt->neighbor = isG3Host ? NULL : neighborPointer;
#else
                pkt->neighbor = neighborPointer;
#endif  // defined(TCPIP_IPV6_G3_PLC_SUPPORT) && (TCPIP_IPV6_G3_PLC_SUPPORT != 0)
                (void)TCPIP_ICMPV6_Flush(pkt);
            }
            else if ((addrType == (uint8_t)IPV6_ADDR_TYPE_MULTICAST) || (addrType == (uint8_t)IPV6_ADDR_TYPE_SOLICITED_NODE_MULTICAST))
            {
                localAddressPointer = TCPIP_IPV6_DASSourceAddressSelect (pNetIf, remoteIP, NULL);
                if (localAddressPointer == NULL)
                {
                    break;
                }

                pkt = TCPIP_ICMPV6_PutHeaderEchoReply (pNetIf, pLclAdd, remoteIP, (uint8_t)ICMPV6_INFO_ECHO_REPLY, h.header_Echo.identifier, h.header_Echo.sequenceNumber);
                if (pkt == NULL)
                {
                    break;
                }

                if (TCPIP_IPV6_TxIsPutReady(pkt, dataLen - 8U) < (dataLen - 8U))
                {
                    pRxPkt->ipv6PktData = (uint16_t) TCPIP_MAC_PKT_ACK_ALLOC_ERR;
                    TCPIP_IPV6_PacketFree (pkt);
                    break;
                }

                (void)TCPIP_IPV6_ArrayPutHelper(pkt, pRxPkt, IPV6_DATA_NETWORK_FIFO, dataLen - 8U);
                pkt->neighbor = neighborPointer;
                (void)TCPIP_ICMPV6_Flush(pkt);
            }
            else
            {
                // do nothing
            }
            break;
#endif
        case (uint8_t)ICMPV6_INFO_ECHO_REPLY:
            F_ICMPV6_NotifyClients(pNetIf, (uint8_t)ICMPV6_INFO_ECHO_REPLY, localIP, remoteIP, &h.header_Echo);

            // The Echo Request/Reply indicates successful round-trip communication.
            // Indicate to the NDP module that this remote node is reachable.
            TCPIP_NDP_NborReachConfirm (pNetIf, remoteIP);
            break;

        case (uint8_t)ICMPV6_INFO_ROUTER_SOLICITATION:
#if defined(TCPIP_IPV6_G3_PLC_BORDER_ROUTER) && (TCPIP_IPV6_G3_PLC_BORDER_ROUTER != 0)
            if((pNetIf->startFlags & (uint16_t)TCPIP_NETWORK_CONFIG_IPV6_ROUTER) == 0U)
            {   // not a router interface
                break;
            }

            tempAddressType.byte = TCPIP_IPV6_AddressTypeGet (pNetIf, remoteIP);
            if ((tempAddressType.bits.scope != (unsigned)IPV6_ADDR_SCOPE_LINK_LOCAL) || (dataLen <= 8U))
            {   // ignore corrupted packet
                break;
            }

            if(addrType == (uint8_t)IPV6_ADDR_TYPE_UNICAST)
            {   // expected RS type
#if ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_BASIC) != 0)
                TCPIPStack_Assert(localIPStruct->flags.type == addrType, __FILE__, __func__, __LINE__);
                if(memcmp(localIPStruct->address.v, localIP->v, sizeof(localIP->v)) != 0)
                {
                    TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
                }
#endif // ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_BASIC) != 0)
                if(localIPStruct->flags.scope == (unsigned)IPV6_ADDR_SCOPE_LINK_LOCAL)
                {   // reply to this solicitation
                    const IPV6_ADDR_STRUCT* advNode;  // address to be advertised
                    TCPIP_ICMPV6_G3AdvertisementSelect(pNetIf, NULL, &advNode);
                    if(advNode != NULL)
                    {
                        (void)TCPIP_ICMPV6_G3RouterAdvertisementPut(pNetIf, localIP, remoteIP, advNode);
                    }
                } 
            }
#endif  // defined(TCPIP_IPV6_G3_PLC_BORDER_ROUTER) && (TCPIP_IPV6_G3_PLC_BORDER_ROUTER != 0)
            break;

        case (uint8_t)ICMPV6_INFO_ROUTER_ADVERTISEMENT:
            tempAddressType.byte = TCPIP_IPV6_AddressTypeGet (pNetIf, remoteIP);
            if ((tempAddressType.bits.scope != (unsigned)IPV6_ADDR_SCOPE_LINK_LOCAL) ||
                (hopLimit != 255U) || (h.header_RA.vCode != 0U) || (dataLen <= 16U))
            {
                break;
            }

            // Initialize all option structures to 0
            llaOption.vType = 0U;
            prefixOption.vType = 0U;
            mtuOption.vType = 0U;

            // Process options
            // Ensure there aren't any options with length of 0
            TCPIP_IPV6_RxBufferSet (pRxPkt, headerLen + (uint16_t)sizeof (h.header_RA));
            tempDataLen = dataLen - (uint16_t)sizeof (h.header_RA);
            while (tempDataLen != 0U)
            {
                (void)TCPIP_IPV6_ArrayGet(pRxPkt, mData, 2U);
                i = mData[0];
                j = (uint16_t)mData[1] << 3;

                if (j == 0U)
                {
                    break;
                }

                tempDataLen -= j;
                j -= 2U;

                switch (i)
                {
                    case (uint8_t)NDP_OPTION_TYPE_LLA_SOURCE:
                        if (j != 6U)
                        {
                            return;
                        }
                        llaOption.vType = (uint8_t)NDP_OPTION_TYPE_LLA_SOURCE;
                        (void)TCPIP_IPV6_ArrayGet (pRxPkt, (uint8_t *)&llaOption.mLinkLayerAddr, (uint16_t)sizeof (TCPIP_MAC_ADDR));
                        break;
                    case (uint8_t)NDP_OPTION_TYPE_PREFIX_INFO:
                        if (j != sizeof (NDP_OPTION_PREFIX_INFO) - 2U)
                        {
                            return;
                        }

                        // Indicate that a prefix option is present, but process it later (in case there are
                        // multiple prefix options)
                        prefixOption.vType = (uint8_t)NDP_OPTION_TYPE_PREFIX_INFO;
                        prefixOption.dValidLifetime = 0U;
                        prefixOption.dPreferredLifetime = 0U;

                        (void)TCPIP_IPV6_ArrayGet (pRxPkt, (uint8_t *)&prefixOption.vPrefixLen, (uint16_t)sizeof (NDP_OPTION_PREFIX_INFO) - 2U);

                        prefixOption.vType = (uint8_t)NDP_OPTION_TYPE_PREFIX_INFO;
                        prefixOption.vLength = (uint8_t)j + 2U;

                        prefixOption.dValidLifetime = TCPIP_Helper_ntohl (prefixOption.dValidLifetime);
                        prefixOption.dPreferredLifetime = TCPIP_Helper_ntohl (prefixOption.dPreferredLifetime);
                        break;
                    case (uint8_t)NDP_OPTION_TYPE_MTU:
                        if (j != 6U)
                        {
                            return;
                        }
                        mtuOption.vType = (uint8_t)NDP_OPTION_TYPE_MTU;
                        mtuOption.dMTU = 0U;
                        // Read the two reserved bytes and  the 4-byte MTU
                        (void)TCPIP_IPV6_ArrayGet (pRxPkt, (uint8_t *)&mtuOption.wReserved, 6U);
                        mtuOption.dMTU = TCPIP_Helper_ntohl (mtuOption.dMTU);
                        break;
                    default:
                        (void)TCPIP_IPV6_SetReadPtr(pRxPkt, (uint8_t *)TCPIP_IPV6_GetReadPtrInRx(pRxPkt) + j);
                        break;
                }
            }

            TCPIP_NDP_RouterSolicitStop(pNetIf);

            TCPIP_IPV6_RxBufferSet (pRxPkt, headerLen);

            routerPointer = TCPIP_NDP_RemoteNodeFind (pNetIf, remoteIP, (uint8_t)IPV6_HEAP_NDP_DR_ID);
            if (routerPointer != NULL)
            {
                neighborPointer = routerPointer->neighborInfo;
            }

            // Process the router advertisement's current hop limit field
            if (h.header_RA.curHopLimit != 0U)
            {
                pIpv6Config->curHopLimit = h.header_RA.curHopLimit;
            }

            // Process the router advertisement's reachable time field
            if (h.header_RA.reachableTime != 0U)
            {
                if (pIpv6Config->baseReachableTime != h.header_RA.reachableTime)
                {
                    pIpv6Config->reachableTime = ((h.header_RA.reachableTime * ((SYS_RANDOM_PseudoGet() & 0xFFU) + 128U)) >> 8U) / 1000U;
                }
                pIpv6Config->baseReachableTime = h.header_RA.reachableTime;
            }

            // Process the router advertisement's retransmission time field
            if (h.header_RA.retransTime != 0U)
            {
                pIpv6Config->retransmitTime = h.header_RA.retransTime;
            }

            // Router found
            if (h.header_RA.routerLifetime != 0U)
            {
                if (routerPointer == NULL)
                {
                    neighborPointer = TCPIP_NDP_RemoteNodeFind (pNetIf, remoteIP, (uint8_t)IPV6_HEAP_NDP_NC_ID);
                    if (neighborPointer != NULL)
                    {
                        if (llaOption.vType != 0U)
                        {
                            // If the link layer address is different, update it
                            if (memcmp(neighborPointer->remoteMACAddr.v, llaOption.mLinkLayerAddr.v, sizeof (TCPIP_MAC_ADDR)) != 0)
                            {
                                TCPIP_NDP_NborCacheLinkLayerAddressUpdate (pNetIf, neighborPointer, &llaOption.mLinkLayerAddr, (uint8_t)NDP_STATE_STALE);
                            }
                        }
                    }
                    else
                    {
                        if (llaOption.vType != 0U)
                        {
                            neighborPointer = TCPIP_NDP_NborEntryCreate (pNetIf, remoteIP, &llaOption.mLinkLayerAddr, (uint8_t)NDP_STATE_STALE, 1U, localIPStruct);
                        }
                        else
                        {
                            neighborPointer = TCPIP_NDP_NborEntryCreate (pNetIf, remoteIP, NULL, (uint8_t)NDP_STATE_NONE, 1U, localIPStruct);
                        }
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
                    (void)TCPIP_NDP_LinkedListEntryRemove (pNetIf, routerPointer, (uint8_t)IPV6_HEAP_NDP_DR_ID);
                    routerPointer = NULL;
                }
            }

            // If there's not a default router, at least see if there's a neighbor cache entry
            if (routerPointer == NULL)
            {
                if (neighborPointer == NULL)
                {
                    neighborPointer = TCPIP_NDP_RemoteNodeFind (pNetIf, remoteIP, (uint8_t)IPV6_HEAP_NDP_NC_ID);
                }
            }
            else
            {
                neighborPointer = routerPointer->neighborInfo;
            }

            // Set the bIsRouter flag for any default router or neighbor with this address to true
            if (neighborPointer != NULL)
            {
                neighborPointer->flags.bIsRouter = 1;
            }

            if (llaOption.vType != 0U)
            {
                if (neighborPointer == NULL)
                {
                    // If there's no index by this point, we RX'ed a neighbor advertisement from a previously unknown router
                    // with lifetime set to 0.  If there's an SLLA we should at least create a neighbor cache entry for it.
                    neighborPointer = TCPIP_NDP_NborEntryCreate (pNetIf, remoteIP, &llaOption.mLinkLayerAddr, (uint8_t)NDP_STATE_STALE, 1U, localIPStruct);
                }
                else
                {
                    // Neighbor Cache or Default Router entry exists
                    // If we have a new link-layer address, update it and set reachability to STALE
                    // The bIsRouter flag will already have been set to true
                    if (memcmp (neighborPointer->remoteMACAddr.v, llaOption.mLinkLayerAddr.v, sizeof (TCPIP_MAC_ADDR)) != 0)
                    {
                        TCPIP_NDP_NborCacheLinkLayerAddressUpdate (pNetIf, neighborPointer, &llaOption.mLinkLayerAddr, (uint8_t)NDP_STATE_STALE);
                    }
                }
            }

            if (prefixOption.vType != 0U)
            {
                // Go through the options again to find all of the prefix options
                TCPIP_IPV6_RxBufferSet (pRxPkt, headerLen + (uint16_t)sizeof (h.header_RA));
                tempDataLen = dataLen - (uint16_t)sizeof (h.header_RA);
                while (tempDataLen != 0U)
                {
                    (void)TCPIP_IPV6_ArrayGet(pRxPkt, mData, 2U);
                    i = mData[0];
                    j = (uint16_t)mData[1] << 3;

                    tempDataLen -= j;
                    j -= 2U;

                    if (i == (uint8_t)NDP_OPTION_TYPE_PREFIX_INFO)
                    {
                        (void)TCPIP_IPV6_ArrayGet (pRxPkt, (uint8_t *)&prefixOption.vPrefixLen, (uint16_t)sizeof (NDP_OPTION_PREFIX_INFO) - 2U);

                        prefixOption.vType = (uint8_t)NDP_OPTION_TYPE_PREFIX_INFO;
                        prefixOption.vLength = (uint8_t)j + 2U;

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
                        (void)TCPIP_IPV6_SetReadPtr(pRxPkt, (uint8_t *)TCPIP_IPV6_GetReadPtrInRx(pRxPkt) + j);
                    }
                }
            }

            if (mtuOption.vType != 0U)
            {
#if TCPIP_IPV6_MINIMUM_LINK_MTU < TCPIP_IPV6_DEFAULT_LINK_MTU
                // the minimum link should be less than the default
                if (((uint32_t)TCPIP_IPV6_MINIMUM_LINK_MTU <= mtuOption.dMTU) && (mtuOption.dMTU < (uint32_t)TCPIP_IPV6_DEFAULT_LINK_MTU))
#else
                if (((uint32_t)TCPIP_IPV6_MINIMUM_LINK_MTU <= mtuOption.dMTU))
#endif
                {
                    pIpv6Config->linkMTU = (uint16_t)mtuOption.dMTU;
                    pIpv6Config->mtuIncreaseTimer = SYS_TMR_TickCountGet() + (sysFreq * (uint32_t)TCPIP_IPV6_MTU_INCREASE_TIMEOUT);

                    // Make sure that all existing destination cache nodes are updated to use this MTU if they use this router.
                    // New nodes will get their MTU from pIpv6Config->linkMTU.
                    destinationPointer = FC_SglNode2NdpDcEntry(pIpv6Config->listDestinationCache.head);
                    while (destinationPointer != NULL)
                    {
                        if (destinationPointer->nextHopNeighbor == neighborPointer)
                        {
                            if (mtuOption.dMTU <= destinationPointer->pathMTU)
                            {
                                destinationPointer->pathMTU = (uint16_t)mtuOption.dMTU;
                                destinationPointer->pathMTUIncreaseTimer = pIpv6Config->mtuIncreaseTimer;
                            }
                        }
                        destinationPointer = destinationPointer->next;
                    }
                }
            }

            break;

        case (uint8_t)ICMPV6_INFO_NEIGHBOR_SOLICITATION:
            // Check for packet validity
            pTargetAdd = FC_U8Ptr2Ip6Add(((uint8_t*)&h.header_NS + offsetof(struct ICMPV6_HEADER_NEIGHBOR_SOLICITATION, aTargetAddress)));
            tempAddressType.byte = TCPIP_IPV6_AddressTypeGet (pNetIf, pTargetAdd);
            if ((tempAddressType.bits.type == (unsigned)IPV6_ADDR_TYPE_MULTICAST) ||
                (hopLimit != 255U) || (h.header_NS.vCode != 0U) || (dataLen < 24U))
            {
                return;
            }

            // Try to get the SLLA option and verify that there are no options with len == 0
            TCPIP_IPV6_RxBufferSet (pRxPkt, headerLen + (uint16_t)sizeof (h.header_NS));
            tempDataLen = dataLen - (uint16_t)sizeof (h.header_NS);
            llaOption.vType = 0U;
            while (tempDataLen != 0U)
            {
                (void)TCPIP_IPV6_ArrayGet(pRxPkt, mData, 2U);
                i = mData[0];
                j = (uint16_t)mData[1];

                if (j == 0U)
                {
                    return;
                }
                j = j << 3;

                tempDataLen -= j;
                j -= 2U;

                switch (i)
                {
                    case (uint8_t)NDP_OPTION_TYPE_LLA_SOURCE:
                        if (j != 6U)
                        {
                            return;
                        }
                        llaOption.vType = (uint8_t)NDP_OPTION_TYPE_LLA_SOURCE;
                        llaOption.vLength = 1U;
                        (void)TCPIP_IPV6_ArrayGet (pRxPkt, (uint8_t *)&llaOption.mLinkLayerAddr, (uint16_t)sizeof (TCPIP_MAC_ADDR));
                        break;
                    default:
                        (void)TCPIP_IPV6_SetReadPtr(pRxPkt, (uint8_t *)TCPIP_IPV6_GetReadPtrInRx(pRxPkt) + j);
                        break;
                }
            }

            i = (uint8_t)memcmp (IPV6_FIXED_ADDR_UNSPECIFIED.v, remoteIP->v, sizeof (IPV6_ADDR));

            // If the packet's source address is the unspecified address, verify that the destination is
            // a solicited node multicast address and that there's no SLLA option
            if (i == 0U)
            {
                // If the source address is the unspecified address, this packet must be discarded if the destination is not
                // a solicited-node multicast address or if there is a SLLA option
                if (llaOption.vType == (uint8_t)NDP_OPTION_TYPE_LLA_SOURCE)
                {
                    return;
                }

                if (!TCPIP_IPV6_AddressIsSolicitedNodeMulticast (localIP))
                {
                    return;
                }
            }

            // If the target address is tentative, process it using stateless address autoconfiguration rules
            pTargetAdd = FC_U8Ptr2Ip6Add(((uint8_t*)&h.header_NS + offsetof(struct ICMPV6_HEADER_NEIGHBOR_SOLICITATION, aTargetAddress)));
            if ((localAddressPointer = TCPIP_IPV6_AddressFind (pNetIf, pTargetAdd, IPV6_ADDR_TYPE_UNICAST_TENTATIVE)) != NULL)
            {
                // If the source address is the unspecified address, process this NS message as a DAD event (otherwise it is
                // being used for address resolution)
                if (i == 0U)
                {
                    TCPIP_NDP_DupAddrDiscoveryProcess (localAddressPointer, (uint8_t)IPV6_NDP_DAD_NS_RECEIVED);
                }
                return;
            }

            // Even though the packet was accepted by our filters, the target address isn't necessarily an
            // address used by our interface.  Check to see if it's a valid target.
            if ((localAddressPointer = TCPIP_IPV6_AddressFind (pNetIf, pTargetAdd, IPV6_ADDR_TYPE_UNICAST)) == NULL)
            {
                return;
            }

            // Check to see if the source address was the unspecified address
            if (i != 0U)
            {
                neighborPointer = TCPIP_NDP_RemoteNodeFind (pNetIf, remoteIP, (uint8_t)IPV6_HEAP_NDP_NC_ID);
                if (llaOption.vType == (uint8_t)NDP_OPTION_TYPE_LLA_SOURCE)
                {
                    // If the source address of the received Neighbor Solicitation isn't the unspecified address and
                    // there is a valid source link layer option, update the neighbor cache.
                    if (neighborPointer == NULL)
                    {
                        neighborPointer = TCPIP_NDP_NborEntryCreate (pNetIf, remoteIP, &llaOption.mLinkLayerAddr, (uint8_t)NDP_STATE_STALE, 0U, localIPStruct);
                        if (neighborPointer == NULL)
                        {
                            return;
                        }
                    }
                    else
                    {
                        if (memcmp (neighborPointer->remoteMACAddr.v, llaOption.mLinkLayerAddr.v, sizeof (TCPIP_MAC_ADDR)) != 0)
                        {
                            TCPIP_NDP_NborCacheLinkLayerAddressUpdate (pNetIf, neighborPointer, &llaOption.mLinkLayerAddr, (uint8_t)NDP_STATE_STALE);
                        }
                    }
                }
            }
            else
            {
                // Send a Neighbor Advertisement to the All-nodes multicast address
                pLclAdd = FC_U8Ptr2CIp6Add(((uint8_t*)localAddressPointer + offsetof(struct S_IPV6_ADDR_STRUCT, address)));
                pTargetAdd = FC_U8Ptr2Ip6Add(((uint8_t*)&h.header_NS + offsetof(struct ICMPV6_HEADER_NEIGHBOR_SOLICITATION, aTargetAddress)));
                pkt = TCPIP_ICMPV6_HeaderNeighborAdvertisementPut (pNetIf, pLclAdd, &IPV6_FIXED_ADDR_ALL_NODES_MULTICAST,
                                            pTargetAdd, false, true);
                if (pkt == NULL)
                {
                    return;
                }

                llaOption.vType = (uint8_t)NDP_OPTION_TYPE_LLA_TARGET;
                llaOption.vLength = 1U;
                (void)memcpy(&llaOption.mLinkLayerAddr , &pNetIf->netMACAddr, sizeof (TCPIP_MAC_ADDR));
                if (TCPIP_IPV6_TxIsPutReady(pkt, (uint16_t)sizeof(NDP_OPTION_LLA)) != 0U)
                {
                    (void)TCPIP_IPV6_PutArray(pkt, (uint8_t*)&llaOption, (uint16_t)sizeof(NDP_OPTION_LLA));
                }
                else
                {
                    TCPIP_IPV6_PacketFree (pkt);
                    return;
                }

                (void)TCPIP_ICMPV6_Flush (pkt);
                return;
            }

            if (neighborPointer == NULL)
            {
                return;
            }

            // Send a Neighbor Advertisement to the soliciting node
            pLclAdd = FC_U8Ptr2CIp6Add(((uint8_t*)localAddressPointer + offsetof(struct S_IPV6_ADDR_STRUCT, address)));
            pTargetAdd = FC_U8Ptr2Ip6Add(((uint8_t*)&h.header_NS + offsetof(struct ICMPV6_HEADER_NEIGHBOR_SOLICITATION, aTargetAddress)));
            pkt = TCPIP_ICMPV6_HeaderNeighborAdvertisementPut (pNetIf, pLclAdd, &neighborPointer->remoteIPAddress,
                                        pTargetAdd, true, true);

            llaOption.vType = (uint8_t)NDP_OPTION_TYPE_LLA_TARGET;
            llaOption.vLength = 1U;
            (void)memcpy(&llaOption.mLinkLayerAddr , &pNetIf->netMACAddr, sizeof (TCPIP_MAC_ADDR));
            if (TCPIP_IPV6_TxIsPutReady(pkt, (uint16_t)sizeof(NDP_OPTION_LLA)) != 0U)
            {
                (void)TCPIP_IPV6_PutArray(pkt, (uint8_t*)&llaOption, (uint16_t)sizeof(NDP_OPTION_LLA));
            }
            else
            {
                TCPIP_IPV6_PacketFree (pkt);
                return;
            }

            // If the reachability state of the neighbor is INCOMPLETE the Flush function will cache the NA and try to resolve the address.
            // This should happen rarely, because devices should rarely send Neighbor Solicitations with no SLLA option.
            pkt->neighbor = neighborPointer;
            (void)TCPIP_ICMPV6_Flush (pkt);
            break;

        case (uint8_t)ICMPV6_INFO_NEIGHBOR_ADVERTISEMENT:
            // Check packet for validity
            pTargetAdd = FC_U8Ptr2Ip6Add(((uint8_t*)&h.header_NA + offsetof(struct ICMPV6_HEADER_NEIGHBOR_ADVERTISEMENT, aTargetAddress)));
            tempAddressType.byte = TCPIP_IPV6_AddressTypeGet (pNetIf, pTargetAdd);
            if ((tempAddressType.bits.type == (uint8_t)IPV6_ADDR_TYPE_MULTICAST) ||
                (hopLimit != 255U) || (h.header_NA.vCode != 0U) || (dataLen < 24U))
            {
                return;
            }

            if ((addrType == (uint8_t)IPV6_ADDR_TYPE_MULTICAST) && (h.header_NA.flags.bits.S == 1U))
            {
                return;
            }

            // Retrieve options.
            // Target Link-layer option is the only valid option for Neighbor Advertisements.
            TCPIP_IPV6_RxBufferSet (pRxPkt, headerLen + (uint16_t)sizeof (h.header_NA));
            tempDataLen = dataLen - (uint16_t)sizeof (h.header_NA);
            llaOption.vType = 0U;
            while (tempDataLen != 0U)
            {
                (void)TCPIP_IPV6_ArrayGet(pRxPkt, mData, 2U);
                i = mData[0];
                j = (uint16_t)mData[1];

                if (j == 0U)
                {
                    return;
                }

                j = j << 3;

                tempDataLen -= j;
                j -= 2U;

                switch (i)
                {
                    case (uint8_t)NDP_OPTION_TYPE_LLA_TARGET:
                        if (j != 6U)
                        {
                            return;
                        }
                        llaOption.vType = (uint8_t)NDP_OPTION_TYPE_LLA_TARGET;
                        // llaOption.vLength = 1U;   // unused
                        (void)TCPIP_IPV6_ArrayGet (pRxPkt, (uint8_t *)&llaOption.mLinkLayerAddr, (uint16_t)sizeof (TCPIP_MAC_ADDR));
                        break;

                    default:
                        (void)TCPIP_IPV6_SetReadPtr(pRxPkt, (uint8_t *)TCPIP_IPV6_GetReadPtrInRx(pRxPkt) + j);
                        break;
                }
            }

            // Discard the packet if its destination address was a multicast and the S flag is non-zero
            if (((addrType == (uint8_t)IPV6_ADDR_TYPE_MULTICAST) || (addrType == (uint8_t)IPV6_ADDR_TYPE_SOLICITED_NODE_MULTICAST)) && h.header_NA.flags.bits.S != 0U)
            {
                return;
            }

            pTargetAdd = FC_U8Ptr2Ip6Add(((uint8_t*)&h.header_NA + offsetof(struct ICMPV6_HEADER_NEIGHBOR_ADVERTISEMENT, aTargetAddress)));
            if ((localAddressPointer = TCPIP_IPV6_AddressFind (pNetIf, pTargetAdd, IPV6_ADDR_TYPE_UNICAST_TENTATIVE)) != NULL)
            {
                TCPIP_NDP_DupAddrDiscoveryProcess (localAddressPointer, (uint8_t)IPV6_NDP_DAD_NA_RECEIVED);
                return;
            }
            if ((localAddressPointer = TCPIP_IPV6_AddressFind (pNetIf, pTargetAdd, IPV6_ADDR_TYPE_UNICAST)) != NULL)
            {
                return;
            }

            // Determine if the target address has an entry in the Neighbor Cache.
            // There's no point in storing it if we weren't the one soliciting it.
            if ((neighborPointer = TCPIP_NDP_RemoteNodeFind (pNetIf, pTargetAdd, (uint8_t)IPV6_HEAP_NDP_NC_ID)) == NULL)
            {
                return;
            }

            if ((neighborPointer->reachabilityState == (uint8_t)NDP_STATE_INCOMPLETE) || (neighborPointer->reachabilityState == (uint8_t)NDP_STATE_NONE))
            {
                if (llaOption.vType == (uint8_t)NDP_OPTION_TYPE_LLA_TARGET)
                {
                    if (h.header_NA.flags.bits.S != 0U)
                    {
                        TCPIP_NDP_NborCacheLinkLayerAddressUpdate (pNetIf, neighborPointer, &llaOption.mLinkLayerAddr, (uint8_t)NDP_STATE_REACHABLE);
                    }
                    else
                    {
                        TCPIP_NDP_NborCacheLinkLayerAddressUpdate (pNetIf, neighborPointer, &llaOption.mLinkLayerAddr, (uint8_t)NDP_STATE_STALE);
                    }
                    if (h.header_NA.flags.bits.R != 0U)
                    {
                        neighborPointer->flags.bIsRouter = 1U;
                    }
                    else
                    {
                        neighborPointer->flags.bIsRouter = 0U;
                    }
                }
            }
            else
            {
                if (llaOption.vType == (uint8_t)NDP_OPTION_TYPE_LLA_TARGET)
                {
                    tempDataLen = (uint16_t)memcmp (neighborPointer->remoteMACAddr.v, llaOption.mLinkLayerAddr.v, sizeof (TCPIP_MAC_ADDR));
                }

                if ((h.header_NA.flags.bits.O == 0U) && (llaOption.vType == (uint8_t)NDP_OPTION_TYPE_LLA_TARGET) && (tempDataLen != 0U))
                {
                    // If the override flag is clear and the supplied link-layer address differs from that in the cache
                    // then (a) if the state of the entry is REACHABLE, set it to STALE but do not update the entry, (b)
                    // ignore the advertisement
                    if (neighborPointer->reachabilityState == (uint8_t)NDP_STATE_REACHABLE)
                    {
                        TCPIP_NDP_ReachabilitySet (pNetIf, neighborPointer, (uint8_t)NDP_STATE_STALE);
                    }
                    break;
                }

                // If we get to here, either the override flag was set, the supplied link-layer address was the same as
                // the address in the cache, or no TLLA option was specified

                // Insert the link-layer address in the cache
                if ((llaOption.vType == (uint8_t)NDP_OPTION_TYPE_LLA_TARGET) && (tempDataLen != 0U))
                {
                    (void)memcpy (&neighborPointer->remoteMACAddr, &llaOption.mLinkLayerAddr, sizeof (TCPIP_MAC_ADDR));
                }

                // If the solicited flag is set, set the entry to reachable
                if (h.header_NA.flags.bits.S != 0U)
                {
                    TCPIP_NDP_ReachabilitySet (pNetIf, neighborPointer, (uint8_t)NDP_STATE_REACHABLE);
                }
                else
                {
                    // If S is clear, and the LLA address was updated, set the address to STALE
                    if ((llaOption.vType == (uint8_t)NDP_OPTION_TYPE_LLA_TARGET) && (tempDataLen != 0U))
                    {
                        TCPIP_NDP_ReachabilitySet (pNetIf, neighborPointer, (uint8_t)NDP_STATE_STALE);
                    }
                }

                if ((h.header_NA.flags.bits.R ^ neighborPointer->flags.bIsRouter) != 0U)
                {
                    neighborPointer->flags.bIsRouter = h.header_NA.flags.bits.R;
                    if (neighborPointer->flags.bIsRouter == 0U)
                    {
                        pTargetAdd = FC_U8Ptr2Ip6Add(((uint8_t*)&h.header_NA + offsetof(struct ICMPV6_HEADER_NEIGHBOR_ADVERTISEMENT, aTargetAddress)));
                        if ((routerPointer = TCPIP_NDP_RemoteNodeFind (pNetIf, pTargetAdd, (uint8_t)IPV6_HEAP_NDP_DR_ID)) != NULL)
                        {
                            // Remote this router from the default router list (this will update all Destination Cache entries that use it as a router)
                            (void)TCPIP_NDP_LinkedListEntryRemove (pNetIf, routerPointer, (uint8_t)IPV6_HEAP_NDP_DR_ID);
                        }
                    }
                }
            }
            break;

        case (uint8_t)ICMPV6_INFO_REDIRECT:
            // Check for packet validity
            tempAddressType.byte = TCPIP_IPV6_AddressTypeGet (pNetIf, remoteIP);
            if (tempAddressType.bits.scope != (unsigned)IPV6_ADDR_SCOPE_LINK_LOCAL)
            {
                return;
            }
            if ((hopLimit != 255U) || (h.header_Rd.vCode != 0U) || (dataLen < 40U))
            {
                return;
            }

            // Verify that the destination address is not a multicast address
            if (h.header_Rd.aDestinationAddress.v[0] == 0xFFU)
            {
                return;
            }

            // Verify that the target address is a link-local address (when redirected to a router) or
            // is the same as the Destination Address (when the target itself is actually link-local).
            // The RFC specifies that the target address in a Redirect header is ALWAYS considered link-local,
            // even if the prefix isn't explicitly known to be link-local, so the only real test we can apply
            // here is to check to see if the target address is a non-link-local multicast address.
            pTargetAdd = FC_U8Ptr2Ip6Add(((uint8_t*)&h.header_Rd + offsetof(struct ICMPV6_HEADER_REDIRECT, aTargetAddress)));
            pDestAdd = FC_U8Ptr2Ip6Add(((uint8_t*)&h.header_Rd + offsetof(struct ICMPV6_HEADER_REDIRECT, aDestinationAddress)));
            if (memcmp (pTargetAdd->v, pDestAdd->v, sizeof (IPV6_ADDR)) != 0)
            {
                tempAddressType.byte = TCPIP_IPV6_AddressTypeGet (pNetIf, pTargetAdd);
                if (tempAddressType.bits.type != (unsigned)IPV6_ADDR_TYPE_UNICAST)
                {
                    return;
                }
                // Don't check the scope - we must assume that the target address is link-local
//                if (tempAddressType.bits.scope != IPV6_ADDR_SCOPE_LINK_LOCAL)
//                    return;
            }

            // Determine if IP source address of the redirect message is the same as the first-hop router
            // for the specified ICMP Destination Address
            neighborPointer = TCPIP_NDP_NextHopGet (pNetIf, pDestAdd);
            if (neighborPointer == NULL)
            {
                return;
            }
            if (memcmp (remoteIP->v, neighborPointer->remoteIPAddress.v, sizeof (IPV6_ADDR)) != 0)
            {
                return;
            }

            // Retrieve options.
            // Target Link-layer option and redirected header options are the only valid options for Redirect messages
            TCPIP_IPV6_RxBufferSet (pRxPkt, headerLen + (uint16_t)sizeof (h.header_Rd));
            tempDataLen = dataLen - (uint16_t)sizeof (h.header_Rd);
            llaOption.vType = 0U;
            while (tempDataLen != 0U)
            {
                (void)TCPIP_IPV6_ArrayGet(pRxPkt, mData, 2U);
                i = mData[0];
                j = (uint16_t)mData[1];

                // Discard the packet if any of the option lengths are zero
                if (j == 0U)
                {
                    return;
                }

                j = j << 3;

                tempDataLen -= j;
                j -= 2U;

                switch (i)
                {
                    case (uint8_t)NDP_OPTION_TYPE_LLA_TARGET:
                        if (j != 6U)
                        {
                            return;
                        }
                        llaOption.vType = (uint8_t)NDP_OPTION_TYPE_LLA_TARGET;
                        // llaOption.vLength = 1U; // unused
                        (void)TCPIP_IPV6_ArrayGet (pRxPkt, (uint8_t *)&llaOption.mLinkLayerAddr, (uint16_t)sizeof (TCPIP_MAC_ADDR));
                        break;
                    // If we get a Redirected header just skip it for now
                    case (uint8_t)NDP_OPTION_TYPE_REDIRECT:
                    default:
                        (void)TCPIP_IPV6_SetReadPtr(pRxPkt, (uint8_t *)TCPIP_IPV6_GetReadPtrInRx(pRxPkt) + j);
                        break;
                }
            }

            i = 0U;

            // If the target address is not the same as the destination address we must treat the target address as a router
            pTargetAdd = FC_U8Ptr2Ip6Add(((uint8_t*)&h.header_Rd + offsetof(struct ICMPV6_HEADER_REDIRECT, aTargetAddress)));
            pDestAdd = FC_U8Ptr2Ip6Add(((uint8_t*)&h.header_Rd + offsetof(struct ICMPV6_HEADER_REDIRECT, aDestinationAddress)));
            if (memcmp (pTargetAdd->v, pDestAdd->v, sizeof (IPV6_ADDR)) != 0)
            {
                i = 1U;
            }

            neighborPointer = (IPV6_HEAP_NDP_NC_ENTRY *)TCPIP_NDP_RemoteNodeFind (pNetIf, pTargetAdd, (uint8_t)IPV6_HEAP_NDP_NC_ID);
            if (neighborPointer == NULL)
            {
                if (llaOption.vType == (uint8_t)NDP_OPTION_TYPE_LLA_TARGET)
                {
                    neighborPointer = TCPIP_NDP_NborEntryCreate (pNetIf, pTargetAdd, &llaOption.mLinkLayerAddr, (uint8_t)NDP_STATE_STALE, i, localIPStruct);
                }
                else
                {
                    neighborPointer = TCPIP_NDP_NborEntryCreate (pNetIf, pTargetAdd, NULL, (uint8_t)NDP_STATE_NONE, i, localIPStruct);
                }
                if (neighborPointer == NULL)
                {
                    return;
                }
            }
            else
            {
                if (i != 0U)
                {
                    neighborPointer->flags.bIsRouter = 1U;
                }

                if (llaOption.vType == (uint8_t)NDP_OPTION_TYPE_LLA_TARGET)
                {
                    // If the address in the TLLA option doesn't match the address in the neighbor cache entry,
                    // set the reachability state to stale.
                    if (memcmp (neighborPointer->remoteMACAddr.v, llaOption.mLinkLayerAddr.v, sizeof (TCPIP_MAC_ADDR)) != 0)
                    {
                        TCPIP_NDP_NborCacheLinkLayerAddressUpdate (pNetIf, neighborPointer, &llaOption.mLinkLayerAddr, (uint8_t)NDP_STATE_STALE);
                    }
                }
            }

            pDestAdd = FC_U8Ptr2Ip6Add(((uint8_t*)&h.header_Rd + offsetof(struct ICMPV6_HEADER_REDIRECT, aDestinationAddress)));
            destinationPointer = FC_SglNode2NdpDcEntry(TCPIP_NDP_RemoteNodeFind (pNetIf, pDestAdd, (uint8_t)IPV6_HEAP_NDP_DC_ID));

            if (destinationPointer == NULL)
            {
                destinationPointer = TCPIP_NDP_DestCacheEntryCreate (pNetIf, pDestAdd, pIpv6Config->linkMTU, neighborPointer);
            }
            else
            {
                destinationPointer->nextHopNeighbor = neighborPointer;
            }

            break;

        default:
            // do nothing: unsupported header
            break;
    }
    return;
}


static void F_ICMPV6_NotifyClients(TCPIP_NET_HANDLE hNetIf, uint8_t type, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, void * header)
{
    ICMPV6_LIST_NODE* dNode;

    TCPIP_Notification_Lock(&icmpv6RegisteredUsers);
    for(dNode = FC_SglNode2Icmp6Node(icmpv6RegisteredUsers.list.head); dNode != NULL; dNode = dNode->next)
    {
        (*dNode->callback)(hNetIf, type, localIP, remoteIP, header);
    }
    TCPIP_Notification_Unlock(&icmpv6RegisteredUsers);

}


#endif







