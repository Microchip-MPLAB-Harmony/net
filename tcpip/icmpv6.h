/*******************************************************************************
 Internet Control Message Protocol (ICMP) IPv6 API Header File

 Company:
    Microchip Technology Inc.

  File Name:
    icmpv6.h
    
  Summary:
    IPv6 Internet Communication Message Protocol

  Description:
    Internet Control Message Protocol (ICMP) in IPv6 functions the same as
    ICMP in IPv4.ICMPv6 is used to report error messages and information messages 
    for IPv6 nodes. ICMP packets in IPv6 are used in the IPv6 neighbor discovery process,
    path MTU discovery, and the Multicast Listener Discovery (MLD) protocol for IPv6.
    RFC - 4443.
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

#ifndef H_ICMPV6_H
#define H_ICMPV6_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
// *****************************************************************************
// Section: ICMPv6 Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* ICMPv6 Module Configuration Data Structure

  Summary:
    Placeholder for ICMPv6 module configuration data.

  Description:
    Provides a placeholder for ICMPv6 module configuration data.

  Remarks:
    None.
*/
typedef struct
{
    void*   reserved;
} TCPIP_ICMPV6_MODULE_CONFIG;

// *****************************************************************************
/*
  Type:
    ICMPV6_HANDLE

  Summary:
    ICMPv6 handle.

  Description:
    A handle that a client needs to use when  deregistering a notification handler. 

  Remarks:
    This handle can be used by the client after the event handler has been registered.
*/
typedef const void* ICMPV6_HANDLE;

// *****************************************************************************
/*
  Enumeration:
    ICMPV6_PACKET_TYPES

  Summary:
    ICMPv6 packet types.

  Description:
    Defines different type of ICMPv6 message. Values in the range from 0 to 127 
    indicate an error type of messages. Values in the range 128 to 255 indicate 
    an information message.

  Remarks:
    None.
*/
typedef enum
{
    ICMPV6_ERROR_DEST_UNREACHABLE =      1u,        // Destination Unreachable error packet
    ICMPV6_ERROR_PACKET_TOO_BIG =        2u,        // Packet Too Big error packet
    ICMPV6_ERROR_TIME_EXCEEDED =         3u,        // Time Exceeded error packet
    ICMPV6_ERROR_PARAMETER_PROBLEM =     4u,        // Parameter Problem error packet
    ICMPV6_INFO_ECHO_REQUEST =           128u,      // Echo Request packet
    ICMPV6_INFO_ECHO_REPLY =             129u,      // Echo Reply packet
    ICMPV6_INFO_ROUTER_SOLICITATION =    133u,      // Router solicitation NDP packet
    ICMPV6_INFO_ROUTER_ADVERTISEMENT =   134u,      // Router advertisement NDP packet
    ICMPV6_INFO_NEIGHBOR_SOLICITATION =  135u,      // Neighbor Solicitation NDP packet
    ICMPV6_INFO_NEIGHBOR_ADVERTISEMENT = 136u,      // Neighbor Advertisement NDP packet
    ICMPV6_INFO_REDIRECT =               137u       // Redirect NDP packet
} ICMPV6_PACKET_TYPES;

// *****************************************************************************
/*
  Enumeration:
    ICMPV6_ERR_DU_CODE

  Summary:
    ICMPv6 Destination Unreachable error code.

  Description:
    Defines different type of ICMPv6 Destination Unreachable error code. 

  Remarks:
    A Destination Unreachable message should be generated by a router, or
    by the IPv6 layer in the originating node, in response to a packet
    that cannot be delivered to its destination address for reasons other
    than congestion. An ICMPv6 message MUST NOT be generated if a
    packet is dropped due to congestion.
    
    More details on RFC - 4446.
*/
typedef enum
{
    ICMPV6_ERR_DU_NO_ROUTE =                         0u,  // No route to destination
    ICMPV6_ERR_DU_PROHIBITED =                       1u,  // Communication with destination administratively prohibited
    ICMPV6_ERR_DU_OUTSIDE_SCOPE =                    2u,  // Beyond scope of source address
    ICMPV6_ERR_DU_ADDR_UNREACHABLE =                 3u,  // Address unreachable
    ICMPV6_ERR_DU_PORT_UNREACHABLE =                 4u,  // Port unreachable
    ICMPV6_ERR_DU_SRC_FAILED_INGRESS_POLICY =        5u,  // Source address failed ingress/egress policy
    ICMPV6_ERR_DU_REJECT_ROUTE =                     6u   // Reject route to destination
} ICMPV6_ERR_DU_CODE;

// *****************************************************************************
/*
  Macro:
    ICMPV6_ERR_PTB_CODE

  Summary:
    ICMPv6 Packet Too Big error code.

  Description:
    ICMPv6 packet too big error code value is 0 generated by the originator 
    and ignored by the receiver. 

  Remarks:
   A Packet Too Big MUST be sent by a router in response to a packet
   that it cannot forward because the packet is larger than the MTU of
   the outgoing link.  The information in this message is used as part
   of the Path MTU Discovery process [PMTU].
    
    More details on RFC - 4446.
*/
#define ICMPV6_ERR_PTB_CODE                             0u

// *****************************************************************************
/*
  Enumeration:
    ICMPV6_ERR_TE_CODE

  Summary:
    ICMPv6 Time Exceeded error code.

  Description:
    Definitions for ICMPv6 Time Exceeded error code.
    An ICMPv6 Time Exceeded message with Code 1 is used to report
    fragment reassembly time out. 

  Remarks:
    More details on RFC - 4446.
*/
typedef enum
{
    ICMPV6_ERR_TE_HOP_LIMIT_EXCEEDED =               0u,  // Hop limit exceeded in transit
    ICMPV6_ERR_TE_FRAG_ASSEMBLY_TIME_EXCEEDED =      1u   // Fragment reassembly time exceeded
} ICMPV6_ERR_TE_CODE;

// *****************************************************************************
/*
  Enumeration:
    ICMPV6_ERR_PP_CODE

  Summary:
    ICMPv6 Parameter Problem error code.

  Description:
   Definitions for ICMPv6 Parameter Problem error code.

  Remarks:
    More details on RFC - 4446.
*/
typedef enum
{
    ICMPV6_ERR_PP_ERRONEOUS_HEADER =                 0u,  // Erroneous header field encountered
    ICMPV6_ERR_PP_UNRECOGNIZED_NEXT_HEADER =         1u,  // Unrecognized Next Header type encountered
    ICMPV6_ERR_PP_UNRECOGNIZED_IPV6_OPTION =         2u   // Unrecognized IPv6 option encountered
} ICMPV6_ERR_PP_CODE;

// *****************************************************************************
/*
  Macro:
    ICMPV6_INFO_EREQ_CODE

  Summary:
    ICMPv6 Packet Echo Request information code.

  Description:
   Definition for ICMPv6 Packet Echo Request information code.

  Remarks:
    More details on RFC - 4446.
*/
#define ICMPV6_INFO_EREQ_CODE                           0u

// *****************************************************************************
/*
  Macro:
    ICMPV6_INFO_ERPL_CODE

  Summary:
    ICMPv6 Packet Echo Reply information code.

  Description:
    Definition for ICMPv6 Packet Echo Reply information code.

  Remarks:
    More details on RFC - 4446.
*/
#define ICMPV6_INFO_ERPL_CODE                           0u

// *****************************************************************************
/*
  Structure:
    ICMPV6_HEADER_ERROR

  Summary:
    Header for an ICMPv6 Error packet.

  Description:
    ICMPv6 different code and type error messages (Range 0 to 127). 

  Remarks:
    More details on RFC - 4446.
*/
typedef struct __attribute__((__packed__))
{
    uint8_t vType;  // icmpV6   request or reply type 
    uint8_t vCode; // error code
    uint16_t wChecksum; // Packet TX and RX checksum
    uint32_t additionalData;   // Unused for Destination. Unreachable and Time Exceeded.  
                               // MTU for MTU. Pointer for Parameter Problem.
} ICMPV6_HEADER_ERROR;

// *****************************************************************************
/*
  Structure:
    ICMPV6_HEADER_ERROR

  Summary:
    Header for an ICMPv6 Echo Request/Reply packet.

  Description:
    ICMPv6 Information message header for different code and type(Range 128 to 255). 

  Remarks:
    More details on RFC - 4446.
*/

// Header for an ICMPv6 Echo Request/Reply packet
typedef struct __attribute__((__packed__))
{
    uint8_t vType;   // icmpV6  request or reply type
    uint8_t vCode;   // Error code
    uint16_t wChecksum;  // Packet TX and RX checksum
    uint16_t identifier; // incoming and outgoing packet identifier
    uint16_t sequenceNumber; // request and reply  sequence number
} ICMPV6_HEADER_ECHO;


// *****************************************************************************
// *****************************************************************************
// Section: ICMPv6 Functions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/*
  Function:
    IPV6_PACKET * TCPIP_ICMPV6_HeaderEchoRequestPut (TCPIP_NET_HANDLE hNetIf, 
        IPV6_ADDR * localIP, IPV6_ADDR * remoteIP, uint8_t type, 
        uint16_t identifier, uint16_t sequenceNumber)   

  Summary:
    Allocates a packet, IPv6 Header, and Upper-layer header for an ICMPv6 
    echo request.

  Description:
    This function allocates a packet using TCPIP_ICMPV6_Open() and updates local 
    and remote addresses.
    Updates IPv6 Header with ICMPv6 type and upper-layer header for an ICMPv6 
    echo request.

  Precondition:
    IPv6 and ICMPv6 are initialized.

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
    None.
 */
IPV6_PACKET * TCPIP_ICMPV6_HeaderEchoRequestPut (TCPIP_NET_HANDLE hNetIf, const IPV6_ADDR * localIP, 
           const IPV6_ADDR * remoteIP, uint8_t type, uint16_t identifier, uint16_t sequenceNumber);
#define TCPIP_ICMPV6_PutHeaderEchoReply TCPIP_ICMPV6_HeaderEchoRequestPut

//*****************************************************************************
/*
  Function:
    bool TCPIP_ICMPV6_Flush (IPV6_PACKET * pkt)

  Summary:
    Flushes a ICMPv6 packet.

  Description:
    Transmit out a ICMPv6 packet.   

  Precondition:
    IPv6 and ICMPv6 are initialized.

  Parameters:
    pkt - The packet to flush

  Returns:
    - true  - if the packet was flushed
    - false - if the packet was queued
    
  Remarks:
    None.
*/
bool TCPIP_ICMPV6_Flush (IPV6_PACKET * pkt);

//*****************************************************************************
/*
  Function:
    ICMPV6_HANDLE TCPIP_ICMPV6_CallbackRegister (void (*callback)(TCPIP_NET_HANDLE hNetIf,
        uint8_t type, IPV6_ADDR * localIP, IPV6_ADDR * remoteIP, void * header))

  Summary:
    Registers an upper-layer function for ICMPv6 callback.

  Description:
    This function registers an upper-layer function to handle ICMPv6 messages that may 
    require action at the application layer (Echo Replies, Error messages)  

  Precondition:
    IPv6 and ICMPv6 are initialized.

  Parameters:
    type - ICMPv6 header type
    localIP - IPv6 destination address of the incoming message
    remoteIP - IPv6 address of the node that originated the incoming message
    header - Pointer to the ICMPv6 header

  Returns:
    ICMPV6_HANDLE
    - Success - Returns a valid handle
    - Failure - Null handle
    
  Remarks:
    None.
*/
ICMPV6_HANDLE TCPIP_ICMPV6_CallbackRegister (void (*callback)(TCPIP_NET_HANDLE hNetIf, 
  uint8_t type, const IPV6_ADDR * localIP, const IPV6_ADDR * remoteIP, void * header));

//*****************************************************************************
/*
  Function:
    ICMPV6_HANDLE TCPIP_ICMPV6_CallbackRegister (void (*callback)(TCPIP_NET_HANDLE hNetIf,
        uint8_t type, IPV6_ADDR * localIP, IPV6_ADDR * remoteIP, void * header))

  Summary:
    Deregisters an upper-layer function from ICMPv6 callback.

  Description:
    This function deregisters a previously registered function from the ICMPv6 register list.     

  Precondition:
    IPv6 and ICMPv6 are initialized.

  Parameters:
    hIcmpv6 - ICMPv6 handler
    

  Returns:
    - true  - On Success
    - false - On Failure (If no such handler registered)
    
  Remarks:
    None.
*/
bool          TCPIP_ICMPV6_CallbackDeregister(ICMPV6_HANDLE hIcmpv6);

// *****************************************************************************
/* Function:
    bool TCPIP_ICMPV6_EchoRequestSend(TCPIP_NET_HANDLE netH, IPV6_ADDR * targetAddr,
                    uint16_t sequenceNumber, uint16_t identifier,uint32_t packetSize);

  Summary:
    Sends an ICMPv6 echo request to a remote node.

  Description:
    This function allows a stack client to send an ICMPv6 Echo request message to a remote host.
    The supplied sequence number and identifier will be used in the query message.
    The user will be notified by the result of the query using a notification handle
    registered by using the TCPIP_ICMPV6_CallbackRegister function.

  Precondition:
    The TCP/IP Stack is initialized and up and running.
    IPv6 and ICMPv6 are initialized.

  Parameters:
    - netH      - handle of the network interface to use for the request
                  Can be 0 if a default interface is to be selected
    - targetAddr  - remote host IP address.
    - sequenceNumber    - a sequence number to be used in the request
    - identifier    - a identifier to be used in the request
    - packetSize - Number of bytes per packet request

  Returns:
    - true    - if the query request was successfully sent
    - false   - otherwise (interface not ready for transmission, allocation error, etc.)

  Remarks:
    None.
*/
bool TCPIP_ICMPV6_EchoRequestSend(TCPIP_NET_HANDLE netH, IPV6_ADDR * targetAddr,
                    uint16_t sequenceNumber, uint16_t identifier,uint32_t packetSize);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // H_ICMPV6_H
