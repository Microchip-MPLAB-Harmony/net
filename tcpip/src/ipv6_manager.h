/*******************************************************************************
  IPv6 private manager API for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    ipv6_manager.h

  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
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








// DOM-IGNORE-END

#ifndef H_IPV6_MANAGER_H_
#define H_IPV6_MANAGER_H_

// Stack structures

// IPv6 Pseudo header (needed for TCP and UDP checksum calculations/verification)
typedef struct
{
    IPV6_ADDR SourceAddress;
    IPV6_ADDR DestAddress;
    unsigned long PacketLength;
    unsigned short zero1;
    unsigned char zero2;
    unsigned char NextHeader;
} IPV6_PSEUDO_HEADER;

#if defined(TCPIP_IPV6_RANDOM_INTERFACE_ID_ENABLE) && (TCPIP_IPV6_RANDOM_INTERFACE_ID_ENABLE != 0)
#define TCPIP_IPV6_RIID_ENABLED   1
#else
#define TCPIP_IPV6_RIID_ENABLED   0
#endif

// IPv6 module configuration descriptor:
// global settings per module
typedef struct
{
    uint16_t        configFlags;            // a TCPIP_IPV6_CONFIG_FLAGS value
    uint16_t        pad;                    // padding, not used
    uint16_t        rxfragmentBufSize;      // RX fragmented buffer size
    uint16_t        fragmentPktRxTimeout;   // fragmented packet timeout value
#if (TCPIP_IPV6_RIID_ENABLED != 0)    
    IPV6_RIID_PR_FNC        riidPrF;        // Random Interface ID generation function - optional
    IPV6_RIID_NET_IFACE_FNC riidNetIfaceF;  // Net_Iface generation function - optional
    IPV6_RIID_NET_ID_FNC    riidNetIdF;     // network_ID generation function - optional
    IPV6_RIID_SECRET_KEY_FNC riidKeyF;      // secret key generation function - mandatory if riidPrF == NULL
#endif  // TCPIP_IPV6_RIID_ENABLED != 0

}TCPIP_IPV6_CONFIG_DCPT;

typedef struct
{
    IPV6_HEAP_NDP_DR_ENTRY * currentDefaultRouter;
    DOUBLE_LIST listIpv6UnicastAddresses;         // IPV6_ADDR_STRUCT list
    DOUBLE_LIST listIpv6MulticastAddresses;       // IPV6_ADDR_STRUCT list
    DOUBLE_LIST listIpv6TentativeAddresses;       // IPV6_ADDR_STRUCT list
    uint32_t baseReachableTime;
    uint32_t reachableTime;
    uint32_t retransmitTime;
    uint32_t mtuIncreaseTimer;
    uint16_t linkMTU;
    uint16_t multicastMTU;
    SINGLE_LIST listNeighborCache;                // IPV6_HEAP_NDP_NC_ENTRY list
    SINGLE_LIST listDefaultRouter;                // IPV6_HEAP_NDP_DR_ENTRY list
    SINGLE_LIST listDestinationCache;             // IPV6_HEAP_NDP_DC_ENTRY list
    SINGLE_LIST listPrefixList;                   // IPV6_HEAP_NDP_PL_ENTRY list
    SINGLE_LIST rxFragments;                      // IPV6_RX_FRAGMENT_BUFFER list
    uint8_t curHopLimit;
    uint8_t initState;
    uint8_t policyPreferTempOrPublic;
    uint8_t g3PanIdSet;                         // The G3-PLC network PAN_Id has been set for this interface   
    uint16_t        g3PanId;                    // PAN_Id for a G3 network
    uint16_t        pad16;
} IPV6_INTERFACE_CONFIG;

// stack private API
//

/*****************************************************************************
  Function:
    bool TCPIP_IPV6_Initialize(
        const TCPIP_STACK_MODULE_CTRL* const pStackInit,
        const TCPIP_IPV6_MODULE_CONFIG* pIpv6Init)

  Summary:
    Initializes an IPv6 interface.

  Description:
    This function initializes an interface for IPv6 operation.  It will
    create the task timer for the IP task and the IPv6 initialization task
    (if it hasn't already been created by a previous call to this function).

  Precondition:
    The IP module initialization function must have been called.

  Parameters:
    pStackInit - Stack initialization parameters
    pIpv6Init - Unused supplementary data field

  Returns:
    true if interface is being initialized, false otherwise

  Remarks:
    None
  ***************************************************************************/
bool TCPIP_IPV6_Initialize(const TCPIP_STACK_MODULE_CTRL* const pStackInit, const void* initData);


/*****************************************************************************
  Function:
    void TCPIP_IPV6_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)

  Summary:
    Disables IPv6 functionality on the specified interface.

  Description:
    This function will disable IPv6 functionality on a specified interface.
    It will free any dynamically allocated structures.

  Precondition:
    None

  Parameters:
    stackCtrl - Stack initialization parameters

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void TCPIP_IPV6_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl);


/*****************************************************************************
  Function:
    void TCPIP_IPV6_InitializeStop (TCPIP_NET_IF * pNetIf)

  Summary:
    Stops IPv6 initialization

  Description:
    This function will halt IPv6 initialization for a specified interface.
    If it determines that no interfaces are initializing IPv6, it will
    destroy the IPv6 initialization task system timer.

  Precondition:
    None

  Parameters:
    pNetIf - The interface to top initialization for.

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void TCPIP_IPV6_InitializeStop (TCPIP_NET_IF * pNetIf);


// misc IP functions
//

/*****************************************************************************
  Function:
    bool TCPIP_IPV6_HeaderGet(TCPIP_MAC_PACKET* pRxPkt, IPV6_ADDR * localIPAddr, IPV6_ADDR * remoteIPAddr, uint8_t *protocol, uint16_t *len, uint8_t * hopLimit);

  Summary:
    Reads IPv6 header information from the MAC.

  Description:
    Reads IPv6 header information from the MAC.

  Precondition:
    None

  Parameters:
    pRxPkt - The packet to check for a header.
    localIPAddr - The incoming packet's destination address.
    remoteIPAddr - The incoming packet's source address.
    protocol - Return value for the next header protocol.
    len - Return value for the payload length.
    hopLimit - Return value for the hop limit of the packet

  Returns:
    bool - True if a packet header was read, false otherwise.

  Remarks:
    None
  ***************************************************************************/
bool TCPIP_IPV6_HeaderGet(TCPIP_MAC_PACKET* pRxPkt, IPV6_ADDR * localIPAddr, IPV6_ADDR * remoteIPAddr, uint8_t *protocol, uint16_t *len, uint8_t * hopLimit);

/*****************************************************************************
  Function:
    bool TCPIP_IPV6_AddressesGet(TCPIP_MAC_PACKET* pRxPkt, const IPV6_ADDR** pDestIPAddr, const IPV6_ADDR** pSourceIPAddr);

  Summary:
    Reads IPv6 address information from a MAC packet.

  Description:
    Reads IPv6 header address information from a MAC packet.

  Precondition:
    None

  Parameters:
    pRxPkt - The MAC packet to check for a valid IPv6 header.
    pDestIPAddr - address to store a pointer to the incoming packet's destination address.
    pSourceIPAddr - address to store a pointer to the incoming packet's source address.

  Returns:
    bool - True if a valid IPv6 packet header found in the MAC packet,
           false otherwise.

  Remarks:
    This function doesn't change any the internal MAC packet data structure members.

  ***************************************************************************/
bool TCPIP_IPV6_AddressesGet(const TCPIP_MAC_PACKET* pRxPkt, const IPV6_ADDR** pDestIPAddr, const IPV6_ADDR** pSourceIPAddr);


/*****************************************************************************
  Function:
    void    TCPIP_IPV6_DiscardRx(TCPIP_MAC_PACKET* pRxPkt, TCPIP_MAC_PKT_ACK_RES ackRes);

  Summary:
    Discards an RX packet.

  Description:
    This function discards an MAC RX packet

  Precondition:
    None

  Parameters:
    pRxPkt - The RX packet.
    ackRes  - acknowledge code

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void    TCPIP_IPV6_DiscardRx(TCPIP_MAC_PACKET* pRxPkt, TCPIP_MAC_PKT_ACK_RES ackRes);

/*****************************************************************************
  Function:
   void*    TCPIP_IPV6_GetReadPtrInRx(TCPIP_MAC_PACKET* pRxPkt);

  Summary:
    Reads the read pointer in an RX packet.

  Description:
    This function reads the read pointer in an RX packet.

  Precondition:
    None

  Parameters:
    pRxPkt - The RX packet.

  Returns:
    the current read pointer value

  Remarks:
    None
  ***************************************************************************/
void*    TCPIP_IPV6_GetReadPtrInRx(TCPIP_MAC_PACKET* pRxPkt);


/*****************************************************************************
  Function:
    void*   TCPIP_IPV6_SetReadPtr(TCPIP_MAC_PACKET* pRxPkt, void* address);

  Summary:
    Sets the read pointer in an RX packet.

  Description:
    This function sets the read pointer in an RX packet.

  Precondition:
    None

  Parameters:
    pRxPkt - The RX packet.

  Returns:
    the old read pointer value

  Remarks:
    None
  ***************************************************************************/
void*   TCPIP_IPV6_SetReadPtr(TCPIP_MAC_PACKET* pRxPkt, void* address);


/*****************************************************************************
  Function:
    bool TCPIP_IPV6_TxPacketStructCopy (IPV6_PACKET * destination, IPV6_PACKET * source)

  Summary:
    Copies data from one IPV6_PACKET to another.

  Description:
    This function will copy essential transmission data from one IP packet
    to another.  This includes the upper-layer header and IP header
    information.

  Precondition:
    None

  Parameters:
    destination - The destination packet.
    source - The source packet

  Returns:
    true if the data was copied successfully, false otherwise

  Remarks:
    None
  ***************************************************************************/
bool TCPIP_IPV6_TxPacketStructCopy (IPV6_PACKET * destination, IPV6_PACKET * source);


/*****************************************************************************
  Function:
    void TCPIP_IPV6_PacketIPProtocolSet (IPV6_PACKET * ptrPacket)
  Summary:
    Sets the packet IP protocol for a TX packet to IPv4 or IPv6.

  Description:
    Sets the packet IP protocol for a TX packet to IPv4 or IPv6.

  Precondition:
    None

  Parameters:
    ptrPacket - Pointer to the target packet.

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void TCPIP_IPV6_PacketIPProtocolSet (IPV6_PACKET * ptrPacket);


/*****************************************************************************
  Function:
    IPV6_DATA_SEGMENT_HEADER *
        TCPIP_IPV6_UpperLayerHeaderPut(IPV6_PACKET * ptrPacket,
                                       void * header,
                                       unsigned short len,
                                       unsigned char type,
                                       unsigned short checksumOffset)
  Summary:
    Adds an upper layer header to an IP TX packet.

  Description:
    This function will add an upper layer header to the chain of segments
    in an IPV6_PACKET structure.  It will also initialize the packet's
    upper-layer header variables.

  Precondition:
    None

  Parameters:
    ptrPacket - The packet that the upper layer header applies to.
    header - Pointer to the header data.
    len - Length of the header.
    type - Standard upper layer header type.
    checksumOffset - Offset of the upper-layer checksum in the header, or
        IPV6_NO_UPPER_LAYER_CHECKSUM.

  Returns:
    IPV6_DATA_SEGMENT_HEADER * - Pointer to the segment in the packet that
        contains the header information.

  Remarks:
    This function will automatically allocate memory to store the header data.
  ***************************************************************************/
IPV6_DATA_SEGMENT_HEADER * TCPIP_IPV6_UpperLayerHeaderPut (IPV6_PACKET * ptrPacket, void * header, unsigned short len, unsigned char type, unsigned short checksumOffset);


// Populate the IPV6_PACKET ptrIPHeader and upperLayerHeaderType fields.
// This is a change from the original TCPIP_IPV6_HeaderPut function; the old function
// would also write the Ethernet and IP header to the MAC TX buffer, including
// the link-layer address.  Now the link-layer address is passed in in the TCPIP_IPV6_Flush
// function.  The length of remoteIPAddr is 4 or 16, depending on packet's IP protocol
// type.
void TCPIP_IPV6_HeaderPut(IPV6_PACKET * ptrPacket, uint8_t protocol);


/*****************************************************************************
  Function:
    unsigned short TCPIP_IPV6_PayloadLengthGet (IPV6_PACKET * pkt)

  Summary:
    Returns the current payload length of a packet.

  Description:
    Returns the current payload length of a packet.

  Precondition:
    None

  Parameters:
    pkt - The packet.

  Returns:
    unsigned short - The current payload length, in bytes.

  Remarks:
    None
 ***************************************************************************/
unsigned short TCPIP_IPV6_PayloadLengthGet (IPV6_PACKET * pkt);


/*****************************************************************************
  Function:
    void * TCPIP_IPV6_UpperLayerHeaderPtrGet(IPV6_PACKET * pkt)

  Summary:
    Returns a pointer to the upper layer header segment in a packet.

  Description:
    Returns a pointer to the upper layer header segment in a packet.

  Precondition:
    None

  Parameters:
    pkt - The packet.

  Returns:
    void * - Pointer to the upper layer header.

  Remarks:
    None
 ***************************************************************************/
void * TCPIP_IPV6_UpperLayerHeaderPtrGet(IPV6_PACKET * pkt);


#define TCPIP_IPV6_GetUpperLayerHeaderLen(s) (s->upperLayerHeaderLen)


/*****************************************************************************
  Function:
    unsigned short TCPIP_IPV6_PayloadChecksumCalculate (IPV6_PACKET * ptrPacket)

  Summary:
    Calculates the checksum of an upper layer payload for a TX packet.

  Description:
    Calculates the checksum of an upper layer payload for a TX packet.

  Precondition:
    None

  Parameters:
    ptrPacket - The packet

  Returns:
    unsigned short - The checksum.

  Remarks:
    None
  ***************************************************************************/
unsigned short TCPIP_IPV6_PayloadChecksumCalculate (IPV6_PACKET * ptrPacket);


/*****************************************************************************
  Function:
    void TCPIP_IPV6_TransmitPacketStateReset (IPV6_PACKET * pkt)

  Summary:
    Removes payload data from a TX packet structure so it can be reused by
    a socket for further communications.

  Description:
    Removes payload data from a TX packet structure so it can be reused by
    a socket for further communications.

  Precondition:
    None

  Parameters:
    pkt - The packet to reset.

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void TCPIP_IPV6_TransmitPacketStateReset (IPV6_PACKET * pkt);


/*****************************************************************************
  Function:
    bool TCPIP_IPV6_PacketTransmit (IPV6_PACKET * pkt)

  Summary:
    Writes the formatted packet to the MAC layer and flushes it.

  Description:
    Writes the formatted packet to the MAC layer and flushes it.

  Precondition:
    None

  Parameters:
    pkt - The packet to transmit

  Returns:
    bool - true if the packet was transmitted, false if the MAC is unlinked

  Remarks:
    None
  ***************************************************************************/
bool TCPIP_IPV6_PacketTransmit (IPV6_PACKET * pkt);


#define TCPIP_IPV6_IsPacketQueued(p)       (((IPV6_PACKET *)p)->flags.queued)
#define TCPIP_IPV6_PacketSetQueued(p)      (((IPV6_PACKET *)p)->flags.queued = true)


/*****************************************************************************
  Function:
    void TCPIP_IPV6_SingleListFree (void * list)

  Summary:
    Frees and deallocates all nodes from a singly linked list.

  Description:
    Frees and deallocates all nodes from a singly linked list.

  Precondition:
    None

  Parameters:
    list - The list.

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void TCPIP_IPV6_SingleListFree (void * list);


/*****************************************************************************
  Function:
    void TCPIP_IPV6_HopLimitSet(IPV6_PACKET * ptrPacket, uint8_t hopLimit)

  Summary:
    Sets the hop limit of a TX packet to a specific value.

  Description:
    Sets the hop limit of a TX packet to a specific value.

  Precondition:
    None

  Parameters:
    ptrPacket - The TX packet.
    hopLimit - The new hop limit value.

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void TCPIP_IPV6_HopLimitSet(IPV6_PACKET * ptrPacket, uint8_t hopLimit);


/*****************************************************************************
  Function:
    void TCPIP_IPV6_RxBufferSet(TCPIP_MAC_PACKET* pRxPkt, uint16_t Offset)

  Summary:
    Sets the read pointer for the current RX buffer (IPv6 version).

  Description:
    Sets the read pointer for the current RX buffer to the specified offset
    after an IPv6 header.

  Precondition:
    None

  Parameters:
    pRxPkt - The packet to set the pointer for.
    Offset - The offset to set.

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void TCPIP_IPV6_RxBufferSet(TCPIP_MAC_PACKET* pRxPkt, uint16_t Offset);

static __inline__ uint16_t __attribute__((always_inline)) TCPIP_IPV6_GetHash(const IPV6_ADDR* add, uint16_t a, uint16_t b)
{
    uint16_t hash6 = (add->w[0] + add->w[1] + add->w[2] + add->w[3] + add->w[4] + add->w[5] + add->w[6] + add->w[7] + a) ^ b;
    return hash6;
}

/*****************************************************************************
  Function:
    IPV6_ADDR_STRUCT * TCPIP_IPV6_AddressFind(TCPIP_NET_IF * pNetIf, const IPV6_ADDR * addr, unsigned char listType)

  Summary:
    Finds a local IPv6 address.

  Description:
    This function finds a local address in the list of tentative, unicast,
    or multicast addresses.

  Precondition:
    None

  Parameters:
    pNetIf - The interface to check for the address.
    addr - The address to check for.
    listType - IPV6_ADDR_TYPE_UNICAST_TENTATIVE, IPV6_ADDR_TYPE_UNICAST,
            IPV6_ADDR_TYPE_MULTICAST

  Returns:
    IPV6_ADDR_STRUCT * - Pointer to the found address, or NULL

  Remarks:
    None
  ***************************************************************************/
IPV6_ADDR_STRUCT *  TCPIP_IPV6_AddressFind(const TCPIP_NET_IF * pNetIf, const IPV6_ADDR * addr, IPV6_ADDR_TYPE listType);


/*****************************************************************************
  Function:
    bool TCPIP_IPV6_AddressIsSolicitedNodeMulticast (IPV6_ADDR * address)

  Summary:
    Determines if the specified address is a solicited node multicast address.

  Description:
    Determines if the specified address is a solicited node multicast address.

  Precondition:
    None

  Parameters:
    address - The address.

  Returns:
    bool - true if the address is a solicited-node multicast address, false
        otherwise

  Remarks:
    None
  ***************************************************************************/
bool  TCPIP_IPV6_AddressIsSolicitedNodeMulticast (const IPV6_ADDR * address);


/*****************************************************************************
  Function:
    uint8_t TCPIP_IPV6_AddressTypeGet (const TCPIP_NET_IF * pNetIf, const IPV6_ADDR * address)

  Summary:
    Gets the scope and type of an IPv6 address.

  Description:
    Gets the scope and type of an IPv6 address.

  Precondition:
    None

  Parameters:
    pNetIf - The interface (used to check for link-locality)
    address - The address to check.

  Returns:
    IPV6_ADDRESS_TYPE - The address scope and type

  Remarks:
    None
  ***************************************************************************/
uint8_t TCPIP_IPV6_AddressTypeGet (const TCPIP_NET_IF * pNetIf, const IPV6_ADDR * address);


/*****************************************************************************
  Function:
    void TCPIP_IPV6_ErrorSend (TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt, IPV6_ADDR * localIP,
        IPV6_ADDR * remoteIP, uint8_t code, uint8_t type,
        uint32_t additionalData, uint16_t packetLen)

  Summary:
    Sends an ICMPv6 error message to a remote node.

  Description:
    Sends an ICMPv6 error message to a remote node.

  Precondition:
    None

  Parameters:
    pNetIf - The outgoing interface to send the message on.
    pRxPkt  - source RX packet to use
    localIP - The local IP address to use.
    remoteIP - The destination IP address to use.
    code - The error code value.
    type - The error type.
    additionalData - Additional ICMPv6 header data.
    packetLen - Length of packet data to include from the packet RX buffer.

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void TCPIP_IPV6_ErrorSend (const TCPIP_NET_IF * pNetIf, TCPIP_MAC_PACKET* pRxPkt, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, uint8_t code, uint8_t type, uint32_t additionalData, uint16_t packetLen);


void TCPIP_IPV6_ClientsNotify(const TCPIP_NET_IF* pNetIf, IPV6_EVENT_TYPE evType, const void* evParam);


IPV6_INTERFACE_CONFIG* TCPIP_IPV6_InterfaceConfigGet(const TCPIP_NET_IF* pNetIf);

const TCPIP_IPV6_CONFIG_DCPT* TCPIP_IPV6_ConfigDcptGet(void);

/*****************************************************************************
  Function:
    void TCPIP_IPV6_DoubleListFree (void * list)

  Summary:
    Frees and deallocates all nodes from a doubly linked list.

  Description:
    Frees and deallocates all nodes from a doubly linked list.

  Precondition:
    None

  Parameters:
    list - The list.

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void TCPIP_IPV6_DoubleListFree (void * list);

// *****************************************************************************

/*
  Function:
        const IPV6_ADDR*  TCPIP_IPV6_PacketGetDestAddress(TCPIP_MAC_PACKET* pRxPkt);

  Summary:
        Returns the IPv6 destination address associated with a TCPIP_MAC_PACKET

  Description:
        The function will return a pointer to where the IPv6 destination address
        is located in the TCPIP_MAC_PACKET.
        The TCPIP_MAC_PACKET is supposed to be a valid IPv6 packet that has
        properly destination address set.

  Precondition:
        pRxPkt - valid IPv6 packet, pMacLayer, pNetLayer filed properly set
        

  Parameters:
        pRxPkt - packet to query


  Returns:
     a valid pointer to an IPV6_ADDR if it succeeds
     0 - if call failed
      
  Remarks:
        This function is mainly meant for RX packets.
 */

const IPV6_ADDR* TCPIP_IPV6_PacketGetDestAddress(const TCPIP_MAC_PACKET* pRxPkt);


// *****************************************************************************

/*
  Function:
        const IPV6_ADDR*  TCPIP_IPV6_PacketGetSourceAddress(TCPIP_MAC_PACKET* pRxPkt);

  Summary:
        Returns the IPv6 source address associated with a TCPIP_MAC_PACKET

  Description:
        The function will return a pointer to where the IPv6 source address
        is located in the TCPIP_MAC_PACKET.
        The TCPIP_MAC_PACKET is supposed to be a valid IPv6 packet that has
        properly source address set.

  Precondition:
        pRxPkt - valid IPv6 packet, pMacLayer, pNetLayer filed properly set
        

  Parameters:
        pRxPkt - packet to query


  Returns:
     a valid pointer to an IPV6_ADDR if it succeeds
     0 - if call failed
      
  Remarks:
        This function is mainly meant for RX packets.
 */

const IPV6_ADDR* TCPIP_IPV6_PacketGetSourceAddress(const TCPIP_MAC_PACKET* pRxPkt);


// *****************************************************************************

/*
  Function:
        void TCPIP_IPV6_SetRemoteMacAddress(IPV6_PACKET * ptrPacket, const TCPIP_MAC_ADDR* pMacAdd);

  Summary:
        Sets the remote MAC address into a IPV6_PACKET

  Description:
        The function will set the remote MAC address of an IPV6_PACKET to the requested value.

  Precondition:
        ptrPacket - valid IPV6_PACKET pointer
        pMacAdd   - valid TCPIP_MAC_ADDR pointer
        

  Parameters:
        ptrPacket - packet to set the TCPIP_MAC_ADDR for
        pMacAdd   - pointer to TCPIP_MAC_ADDR to set


  Returns:
     None
      
  Remarks:
     None
 */
void TCPIP_IPV6_SetRemoteMacAddress(IPV6_PACKET * ptrPacket, const TCPIP_MAC_ADDR* pMacAdd);

// *****************************************************************************

/*
  Function:
        void TCPIP_IPV6_SetPacketMacAcknowledge(IPV6_PACKET * ptrPacket, TCPIP_MAC_PACKET_ACK_FUNC macAckFnc);

  Summary:
        Sets the packet MAC acknowledged function into a IPV6_PACKET

  Description:
        The function will set the MAC acknowledge function of an IPV6_PACKET to the requested value.

  Precondition:
        ptrPacket - valid IPV6_PACKET pointer
        

  Parameters:
        ptrPacket - packet to set the TCPIP_MAC_ADDR for
        macAckFnc - TCPIP_MAC_PACKET_ACK_FUNC pointer


  Returns:
     None
      
  Remarks:
     This function will be called by the MAC driver once it's done transmitting a IPv6 packet.
 */
void TCPIP_IPV6_SetPacketMacAcknowledge(IPV6_PACKET * ptrPacket, TCPIP_MAC_PACKET_ACK_FUNC macAckFnc);

// *****************************************************************************

/*
  Function:
        void TCPIP_IPV6_PacketAck(IPV6_PACKET * ptrPacket, bool success);

  Summary:
        Acknowledges a TX packet that IPv6 module has processed.

  Description:
        The function will call the IPv6 packet acknowledge function of an IPV6_PACKET once the IPv6 module TX processing of the packet is done.

  Precondition:
        ptrPacket - valid IPV6_PACKET pointer
        

  Parameters:
        ptrPacket - packet to set the TCPIP_MAC_ADDR for
        success   - true if the packet was sent successfuly
                    false otherwise


  Returns:
     None
      
  Remarks:
     This function should be called by the IPv6 module once it's done processing/transmitting a IPv6 packet.
 */
void TCPIP_IPV6_PacketAck(IPV6_PACKET * ptrPacket, bool success);

// IPv6 Interface Identifier length, in bits. Fixed for this implementation
#define IPV6_INTERFACE_ID_SIZE          64U

// corresponding offset in the IPv6 address, bytes
#define IPV6_INTERFACE_ID_OFFSET        8U

// minimum accepted secret key size, bytes
#define IPV6_RIID_SECRET_KEY_MIN_SIZE   16U

// default DAD retries for RIID generation
#define IPV6_RIID_IDGEN_RETRIES         3U

/*****************************************************************************
  Function:
    bool TCPIP_IPV6_AddressInterfaceIDSet (const TCPIP_NET_IF * pNetIf, IPV6_ADDR *pAddress, uint8_t prefixLength, uint8_t DAD_Counter)

  Summary:
    Sets the 64-bit interface ID in an IPv6 address.

  Description:
    Sets the 64-bit interface ID in an IPv6 address

  Precondition:
    The network prefix is set in the address structure

  Parameters:
    pNetIf - The interface
    pAddress - The IPv6 address where the interface ID is set.
    prefixLength - The length of the address prefix
    dadCounter - The Duplicate Address Detection Counter

  Returns:
    true  - On success
    false - Failure to indicate an error occurred  

  Remarks:
    None

  ***************************************************************************/
bool TCPIP_IPV6_AddressInterfaceIDSet (const TCPIP_NET_IF * pNetIf, IPV6_ADDR *pAddress, uint8_t prefixLen, uint8_t dadCounter);

// conversion functions/helpers
//
static __inline__ IPV6_PACKET* __attribute__((always_inline)) FC_SglNode2Ip6Pkt(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        IPV6_PACKET* pkt;
    }U_SGL_NODE_IP6_PKT;

    U_SGL_NODE_IP6_PKT.node = node;
    return U_SGL_NODE_IP6_PKT.pkt;
}

// union of NDP entry types
typedef union
{
    void*  vPtr;
    SGL_LIST_NODE*  node;
    DBL_LIST_NODE*  dNode;
    //
    IPV6_HEAP_NDP_PL_ENTRY* pPlEntry;
    IPV6_HEAP_NDP_DC_ENTRY* pDcEntry;
    IPV6_HEAP_NDP_DR_ENTRY* pDrEntry;
    IPV6_HEAP_NDP_NC_ENTRY* pNcEntry;
    //
    IPV6_ADDR_STRUCT* pAddStr;
}IPV6_HEAP_NDP_ENTRY_U;

static __inline__ IPV6_HEAP_NDP_ENTRY_U __attribute__((always_inline)) FC_SglNode2NdpEntry(SGL_LIST_NODE* sNode)
{
    IPV6_HEAP_NDP_ENTRY_U ndpEntry;
    ndpEntry.node = sNode;

    return ndpEntry;
}

static __inline__ IPV6_HEAP_NDP_ENTRY_U __attribute__((always_inline)) FC_VPtr2NdpEntry(void* vPtr)
{
    IPV6_HEAP_NDP_ENTRY_U ndpEntry;
    ndpEntry.vPtr = vPtr;

    return ndpEntry;
}

static __inline__ IPV6_ADDR_STRUCT* __attribute__((always_inline)) FC_DblNode2AddStruct(DBL_LIST_NODE* node)
{
    union
    {
        DBL_LIST_NODE*  node;
        IPV6_ADDR_STRUCT* pAddStr;
    }U_DBL_NODE_ADD_STR;

    U_DBL_NODE_ADD_STR.node = node;
    return U_DBL_NODE_ADD_STR.pAddStr;
}

static __inline__ DBL_LIST_NODE* __attribute__((always_inline)) FC_AddStruct2DblNode(IPV6_ADDR_STRUCT* pAddStr)
{
    union
    {
        IPV6_ADDR_STRUCT* pAddStr;
        DBL_LIST_NODE*  dnode;
    }U_ADD_STR_DBL_NODE;

    U_ADD_STR_DBL_NODE.pAddStr = pAddStr;
    return U_ADD_STR_DBL_NODE.dnode;
}

static __inline__ IPV6_ADDR_STRUCT* __attribute__((always_inline)) FC_AddHndl2Ipv6AddStruct(IPV6_ADDR_HANDLE addH)
{
    union
    {
        IPV6_ADDR_HANDLE    addH;
        IPV6_ADDR_STRUCT*   pStruct;
    }U_DBL_NODE_IPV6_ADD_STRUCT;

    U_DBL_NODE_IPV6_ADD_STRUCT.addH = addH;
    return U_DBL_NODE_IPV6_ADD_STRUCT.pStruct;
}

static __inline__ const IPV6_ADDR* __attribute__((always_inline)) FC_U8Ptr2CIp6Add(uint8_t* u8Ptr)
{
    union
    {
        uint8_t*  u8Ptr;
        const IPV6_ADDR* pIp6Add;
    }U_U8PTR_IP6_ADD;

    U_U8PTR_IP6_ADD.u8Ptr = u8Ptr;
    return U_U8PTR_IP6_ADD.pIp6Add;
}

static __inline__ const IPV6_HEADER* __attribute__((always_inline)) FC_CuPtr2IPv6Hdr(const uint8_t* cuPtr8)
{
    union
    {
        const uint8_t* cuPtr8;
        const IPV6_HEADER*  pHdr;
    }U_PTR8_IP6_HDR;

    U_PTR8_IP6_HDR.cuPtr8 = cuPtr8;
    return U_PTR8_IP6_HDR.pHdr;
}

static __inline__ IPV6_HEADER* __attribute__((always_inline)) FC_VPtr2IPv6Hdr(void* vPtr)
{
    union
    {
        void* vPtr;
        IPV6_HEADER*  pHdr;
    }U_VPTR_IP6_HDR;

    U_VPTR_IP6_HDR.vPtr = vPtr;
    return U_VPTR_IP6_HDR.pHdr;
}


#endif // H_IPV6_MANAGER_H_
