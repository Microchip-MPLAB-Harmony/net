/*******************************************************************************
  UDP Module Definitions for the Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    udp.h

  Summary:
    UDP is a standard transport layer protocol described in RFC 768. It provides 
    fast but unreliable data-gram based transfers over networks, and forms the 
    foundation SNTP, SNMP, DNS, and many other protocol standards
    
  Description:
    UDP is a standard transport layer protocol described in RFC 768.
*******************************************************************************/

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

#ifndef __UDP_H_
#define __UDP_H_

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  


// *****************************************************************************
/*
  Type:
    UDP_PORT

  Summary:
    Defines a type for a UDP port number

  Description:
    Type describing a UDP port

  Remarks:
    None

*/
typedef uint16_t UDP_PORT;

// *****************************************************************************
/*
  Type:
    UDP_SOCKET

  Summary:
    Provides a handle to a UDP Socket

  Description:
    Type describing a UDP socket

  Remarks:
    None

*/
typedef int16_t UDP_SOCKET;


#define INVALID_UDP_SOCKET      (-1)        // Indicates a UDP socket that is not valid

// *****************************************************************************
/*
  Enumeration:
    UDP_SOCKET_FLAGS

  Summary:
    UDP socket flags

  Description:
    List of flags for a UDP socket info

  Remarks:
    Multiple flags can be simultaneously set

*/
typedef enum
{
    UDP_SOCKET_FLAG_NONE        = 0x00,     // no flag set
    UDP_SOCKET_FLAG_STRICT_PORT = 0x01,     // the UDP_OPTION_STRICT_PORT option is set
    UDP_SOCKET_FLAG_STICKY_PORT = 0x02,     // the port UDP_OPTION_STRICT_SET_STICKY  is set

    UDP_SOCKET_FLAG_STRICT_NET  = 0x04,     // the UDP_OPTION_STRICT_NET option is set
    UDP_SOCKET_FLAG_STICKY_NET  = 0x08,     // the port UDP_OPTION_STRICT_SET_STICKY  is set

    UDP_SOCKET_FLAG_STRICT_ADD  = 0x10,     // the UDP_OPTION_STRICT_ADDRESS option is set
    UDP_SOCKET_FLAG_STICKY_ADD  = 0x20,     // the port UDP_OPTION_STRICT_SET_STICKY  is set

}UDP_SOCKET_FLAGS;


// *****************************************************************************
/*
  Structure:
    UDP_SOCKET_INFO

  Summary:
    Information about a UDP socket

  Description:
    Structure containing the information about a UDP socket

  Remarks:
    The information in this structure will be updated based on the current packet received by the socket.
    For example, members like:
        hNet
        remotePort
        remoteIPaddress
    could change from one packet to another.
    This depends on how the socket is configured (allowed to receive on all interfaces, from all ports, etc.).
    See UDP_SOCKET_OPTION for details.

*/
typedef struct
{
    IP_ADDRESS_TYPE     addressType;        // address type of the socket
    IP_MULTI_ADDRESS    remoteIPaddress;    // current socket destination address
    IP_MULTI_ADDRESS    localIPaddress;     // current socket source address
    IP_MULTI_ADDRESS    sourceIPaddress;    // source address of the last packet 
    IP_MULTI_ADDRESS    destIPaddress;      // destination address of the last packet 
    UDP_PORT            remotePort;         // Port number associated with remote node
    UDP_PORT            localPort;          // local port number
    TCPIP_NET_HANDLE    hNet;               // associated interface
    uint16_t            rxQueueSize;        // packets waiting in the rx queue
    uint16_t            txSize;             // tx buffer size
    UDP_SOCKET_FLAGS    flags;              // associated flags
} UDP_SOCKET_INFO;


// *****************************************************************************
/*
  Enumeration:
    UDP_SOCKET_OPTION

  Summary:
    UDP socket options

  Description:
    List of options to be set for a UDP socket

  Remarks:
    None

*/
typedef enum
{
    UDP_OPTION_STRICT_PORT,         // When connection is done the socket stores the remote host local port.
                                    // If option is enabled the remote host local port is always checked to match the initial one.
                                    // If disabled the remote host local port is not checked. 
                                    // Disabled by default on a socket server. Enabled by default on a client socket.
    UDP_OPTION_STRICT_NET,          // When connection is done the socket stores the network interface the connection occurred on.
                                    // If option is enabled the socket accepts data only from the interface that matches the initial connection.
                                    // If disabled the socket receives data from any remote host regardless of the 
                                    // interface on which the packet arrived.
                                    // Disabled by default on a socket server. Enabled by default on a client socket.
    UDP_OPTION_STRICT_ADDRESS,      // When connection is done the socket stores the address of the remote host.
                                    // If option is enabled the socket accepts data only from the host with address that matches the initial connection.
                                    // If disabled the socket receives data from any remote host regardless of the 
                                    // address of that host.
                                    // Disabled by default on a socket server. Enabled by default on a client socket.
    UDP_OPTION_BROADCAST,           // Enables the Broadcast transmission by the socket
    UDP_OPTION_BUFFER_POOL,         // Enables the socket to use the private UDP buffers pool.
                                    // The size of the TX buffer has to be less than the size of the buffers in the pool.
    UDP_OPTION_TX_BUFF,             // Request different TX buffer size. Has to call TCPIP_UDP_OptionsGet to see the exact space
                                    // allocated.
    UDP_OPTION_TX_QUEUE_LIMIT,      // Sets the maximum number of packets that can be queued/allocated by an UDP socket
                                    // at a certain time.
                                    // For sockets that need to transmit a lot of data (Iperf client, for example),
                                    // especially on slow connections this limit prevents running out of memory
                                    // because the MAC transfer cannot keep up with the UDP packet allocation rate
                                    // imposed by the application.
                                    // Adjust depending on the size of the UDP TX buffer, the connection speed
                                    // and the amount of memory available to the stack.
    UDP_OPTION_RX_QUEUE_LIMIT,      // Sets the maximum number of RX packets that can be queued by an UDP socket
                                    // at a certain time.
                                    // Setting this value to 0 will disable the socket receive functionality
    UDP_OPTION_RX_AUTO_ADVANCE,     // Sets the RX auto advance option for a socket. The option is off by default
                                    // If set, the option will automatically advance the socket to the
                                    // next awaiting RX packet when a call to TCPIP_UDP_ArrayGet is made and 
                                    // no more data is available from the current packet.
                                    // Setting this option forces the socket user to correctly monitor the number of bytes
                                    // available and issue a TCPIP_UDP_Discard only when there's bytes left in the current packet.
                                    // However sequential calls to TCPIP_UDP_ArrayGet can be made without the need
                                    // to insert calls to TCPIP_UDP_Discard
                                    //
                                    // If cleared (default case) the socket user can safely issue a TCPIP_UDP_Discard
                                    // even after calling TCPIP_UDP_ArrayGet to read all the bytes from the current RX packet
                                    // Note that TCPIP_UDP_GetIsReady will always advance the current RX packet when no more data is
                                    // available in the current packet.
                                    // Even when the option is cleared a call to TCPIP_UDP_Discard
                                    // is still not needed if all bytes are retrieved with
                                    // TCPIP_UDP_ArrayGet and then TCPIP_UDP_GetIsReady is called. 
    UDP_OPTION_TX_TTL,              // Specifies the Time To Live for packets sent by the socket.
                                    // If 0, the socket will use the default global IPv4 TTL setting.
                                    // This option allows the user to specify a different TTL value.
                                    // Could be used to adjust the TTL value for multicast traffic
                                    // (the default TTL for multicast traffic is 1).
    UDP_OPTION_MULTICAST,           // Sets the multicast options for a socket by using UDP_OPTION_MULTICAST_DATA value
    UDP_OPTION_TOS,                 // Sets the Type of Service (TOS) for IPv4 packets sent by the socket
    UDP_OPTION_DF,                  // Sets the Don't Fragment (DF) option for IPv4 packets sent by the socket
    UDP_OPTION_FIXED_DEST_ADDRESS,  // If set, then the destination address won't change to reply to the latest host that sent the packet
                                    // The socket will reply to the set destination address. 
                                    // Note that if a destination address is not set, the socket will select the host that sent the packet but only the first time
                                    // Useful when all the replies need to go to a specific address.
                                    // If cleared (default case) the socket will reply to the latest host that sent the message.
                                    // 
    UDP_OPTION_FIXED_DEST_PORT,     // If set, ignore the source port of a packet and reply to the currently set destination port.
                                    // Normally a socket would reply to the sender of the packet using the source port of the sender.
                                    // This option allows the socket to reply using the current destination/remote port
                                    // no matter the source port of the received traffic
                                    // If cleared (default) then the socket will reply to the source port of the sender

    UDP_OPTION_ENFORCE_STRICT_NET,  // Do enforce the UDP_OPTION_STRICT_NET status when using TCPIP_UDP_Bind/TCPIP_UDP_SocketNetSet operations
                                    // Option enabled by default; these operations will enforce the UDP_OPTION_STRICT_NET 
                                    // Note: this option is obsolete and it will be eventually removed!
                                    //  Use the UDP_OPTION_STRICT_SET_STICKY/UDP_OPTION_STRICT_CLR_STICKY flags instead

    //
    //
    UDP_OPTION_STRICT_CLR_STICKY = 0x4000,  // a flag to clear the STICKY option setting
                                            // see the UDP_OPTION_STRICT_SET_STICKY flag
                                            // Note: UDP_OPTION_STRICT_SET_STICKY and UDP_OPTION_STRICT_CLR_STICKY are mutually exclusive, cannot be both set
    UDP_OPTION_STRICT_SET_STICKY = 0x8000,  // a flag for the UDP_OPTION_STRICT_PORT/UDP_OPTION_STRICT_NET/UDP_OPTION_STRICT_ADDRESS
                                            // if this flag is set when changing an option, then the option will not be changed by other operations
                                            // that might normally affect the setting
                                            //      For example, TCPIP_UDP_RemoteBind normally enforces UDP_OPTION_STRICT_PORT
                                            //      However, if the UDP_OPTION_STRICT_SET_STICKY flag is set, the UDP_OPTION_STRICT_PORT setting won't be touched
}UDP_SOCKET_OPTION;


// *****************************************************************************
/*
  Enumeration:
    UDP_SOCKET_BCAST_TYPE

  Summary:
    UDP broadcast types.

  Description:
    List of options to be set for a broadcast socket

  Remarks:
    These options are meant for sockets carrying broadcast traffic.

*/
typedef enum
{
    UDP_BCAST_NONE,                    // no broadcast
    UDP_BCAST_NETWORK_LIMITED,         // network limited broadcast
    UDP_BCAST_NETWORK_DIRECTED,        // network directed broadcast

}UDP_SOCKET_BCAST_TYPE;

// *****************************************************************************
/*
  Enumeration:
    UDP_SOCKET_MULTICAST_FLAGS

  Summary:
    UDP multicast options/flags.

  Description:
    List of options to be set for a multicast socket

  Remarks:
    These options are meant for sockets carrying multicast traffic.

    Multiple flags can be set.


    16 bits only supported.
*/
typedef enum
{
    UDP_MCAST_FLAG_LOOSE_NET_SOURCE_PORT    = 0x0001,   // Union of the !UDP_OPTION_STRICT_NET, !UDP_OPTION_STRICT_ADDRESS and !UDP_OPTION_STRICT_PORT  
                                                        // This is the default behavior for a multicast socket.
                                                        // The same bahvior can be obtained by setting individual options.
                                                        // It is the default behavior of a newly created server socket but not for a client socket.
    UDP_MCAST_FLAG_IGNORE_SOURCE_ADD        = 0x0002,   // Ignore the source address of a packet and reply to the currently set (multicast)
                                                        // destination address.
                                                        // Normally a socket would reply to the sender of the packet.
                                                        // But probably this is not what you want for multicast.
                                                        // This option allows the socket to reply to the multicast group instead, no matter
                                                        // from what source it received the multicast traffic.
                                                        // This option is disabled by default when a socket is created and should be enforced when needed.
                                                        // The same effect with using the regular socket option UDP_OPTION_FIXED_DEST_ADDRESS
    UDP_MCAST_FLAG_IGNORE_SOURCE_PORT       = 0x0004,   // Ignore the source port of a packet and reply to the currently set destination port.
                                                        // Normally a socket would reply to the sender of the packet using the source port of the sender.
                                                        // This option allows the socket to reply using the current destination/remote port
                                                        // no matter the source port of the received multicast traffic - which would be the expected
                                                        // behavior from a multicast socket. 
                                                        // This option is disabled by default when a socket is created and should be enforced when needed.
                                                        // The same effect with using the regular socket option UDP_OPTION_FIXED_DEST_PORT
    UDP_MCAST_FLAG_IGNORE_UNICAST           = 0x0008,   // Ignore a packet if its destination is not multicast
    UDP_MCAST_FLAG_LOOP                     = 0x0010,   // When set, the multicast packets sent by the UDP socket will be routed on the internal multicast interface as well.
                                                        // Default is cleared.
                                                        // Applies only to sockets sending multicast traffic
    UDP_MCAST_FLAG_DISABLE_SOURCE_CHECK     = 0x0020,   // Disables/enables the checking of the source of multicast traffic that reaches a socket.
                                                        // The IGMPv3 mandates that the multicast traffic should be passed to a socket only if that socked subscribed
                                                        // for multicast from that source. 
                                                        // However, for high rate data streams that may place an overhead and consume additional CPU power for doing
                                                        // this check for each and every incoming packet.
                                                        // This option allows the user to bypass this run time check.
                                                        // By default the source check is enabled - recommended setting! 
                                                        // Applies only to sockets receiving multicast traffic




    //
    UDP_MCAST_FLAG_DEFAULT                  = (UDP_MCAST_FLAG_LOOSE_NET_SOURCE_PORT | UDP_MCAST_FLAG_IGNORE_SOURCE_ADD | UDP_MCAST_FLAG_IGNORE_SOURCE_PORT | UDP_MCAST_FLAG_IGNORE_UNICAST)
                                                        // the default flags intended for the multicast sockets
                                                        // note that these flags are different from the defaults for a regular socket, client or server.

}UDP_MULTICAST_FLAGS;

// *****************************************************************************
/*
  Structure:
    UDP_OPTION_MULTICAST_DATA

  Summary:
    Data structure used to set a socket multicast options.

  Description:
    Allows the multicast options configuration of a socket.

  Remarks:
    Using the mask and value members, multiple flags can be set and cleared in one single operations.
    The TCPIP_UDP_OptionsSet() operation for UDP_OPTION_MULTICAST will apply the folowing to the socket flags:
    flags &= ~flagsMask;    // clear all flags touched by mask
    flags |= (flagsValue & flagsMask);  // set the corresponding flags that are set in flagsValue. All others will remain cleared.
    

  Example:
    <code>
    // set the flag UDP_MCAST_FLAG_LOOP:
    UDP_OPTION_MULTICAST_DATA mcastData;
    mcastData.flagsMask = UDP_MCAST_FLAG_LOOP;      // specify the flag to touch
    mcastData.flagsValue = UDP_MCAST_FLAG_LOOP;     // new value for the flag:
                                                    // any value with the bit set for the UDP_MCAST_FLAG_LOOP bit
    TCPIP_UDP_OptionsSet(hUdp, UDP_OPTION_MULTICAST, &mcastData);

    // clear the flag UDP_MCAST_FLAG_LOOP:
    mcastData.flagsMask = UDP_MCAST_FLAG_LOOP;      // specify the flag to touch
    mcastData.flagsValue = 0;                       // new value for the flag:
                                                    // any value with the bit cleared for the UDP_MCAST_FLAG_LOOP bit
    TCPIP_UDP_OptionsSet(hUdp, UDP_OPTION_MULTICAST, &mcastData);

    // set UDP_MCAST_FLAG_LOOP and UDP_MCAST_FLAG_DISABLE_SOURCE_CHECK
    // clear  UDP_MCAST_FLAG_IGNORE_SOURCE_ADD and UDP_MCAST_FLAG_IGNORE_SOURCE_PORT
    mcastData.flagsMask = UDP_MCAST_FLAG_LOOP | UDP_MCAST_FLAG_DISABLE_SOURCE_CHECK | UDP_MCAST_FLAG_IGNORE_SOURCE_ADD | UDP_MCAST_FLAG_IGNORE_SOURCE_PORT; // flags to touch
    mcastData.flagsValue = UDP_MCAST_FLAG_LOOP | UDP_MCAST_FLAG_DISABLE_SOURCE_CHECK ;  // value with bits set for the first 2 flags, cleared for the others
    TCPIP_UDP_OptionsSet(hUdp, UDP_OPTION_MULTICAST, &mcastData);
    </code>
*/
//
typedef struct
{
    UDP_MULTICAST_FLAGS     flagsMask;      // mask of flags to be touched (changed) by the operation
    UDP_MULTICAST_FLAGS     flagsValue;     // new value of flags to be changed to,
                                            // for the flags that are touched, according to the mask 
}UDP_OPTION_MULTICAST_DATA;

// *****************************************************************************
/*
  Enumeration:
    TCPIP_UDP_SIGNAL_TYPE

  Summary:
    UDP run-time signal types.

  Description:
    Description of the signals/events that a UDP socket can generate.

  Remarks:
    These signals are used in the socket event handling notification functions.
    Currently a UDP notification doesn't set multiple flags as the TX and RX events are 
    handled separately.

    The signals are 16 bits wide.
*/
typedef enum
{
    // TX related signals
    TCPIP_UDP_SIGNAL_TX_DONE    = 0x0001,   // A data packet was successfully transmitted on the interface
                                            // There may be available buffer space to send new data
                                            // Notes:
                                            //  - other buffer(s) may have been already allocated by the user
                                            //    so the transmitted packet won't be used again as socket writing space
                                            //  - since UDP transmissions are triggered by an explicit call to TCPIP_UDP_Flush,
                                            //    this notification is the result of an user action 
                                               
    // RX related signals
    TCPIP_UDP_SIGNAL_RX_DATA    = 0x0100,   // A data packet was successfully received and there is data available for this socket

    // interface related signals
    TCPIP_UDP_SIGNAL_IF_DOWN    = 0x2000,   // associated interface is going down;
                                            // sockets connected on this interface will be disconnected, but still alive
    TCPIP_UDP_SIGNAL_IF_CHANGE  = 0x4000,   // associated interface has changed address
                                            // sockets connected on this interface will be disconnected, but still alive

}TCPIP_UDP_SIGNAL_TYPE;

// *****************************************************************************
/*
  Type:
    TCPIP_UDP_SIGNAL_FUNCTION

  Summary:
    UDP Signal Handler.

  Description:
    Prototype of a UDP signal handler. Socket user can register a handler for the
    UDP socket. Once an UDP signals occurs the registered handler will be called.

  Parameters:
    hUDP        - UDP socket to be used
    hNet        - the network interface on which the event has occurred
    sigType     - type of UDP signal that has occurred
    param       - additional parameter that can has been specified at the handler registration call
                  Currently not used and it will be null.


  Remarks:
    The handler has to be short and fast. It is meant for
    setting an event flag, not for lengthy processing!

 */

typedef void    (*TCPIP_UDP_SIGNAL_FUNCTION)(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param);


// *****************************************************************************
/*
  Type:
    TCPIP_UDP_SIGNAL_HANDLE

  Summary:
    UDP socket handle.

  Description:
    A handle that a socket client can use after the signal handler has been registered.
 */

typedef const void* TCPIP_UDP_SIGNAL_HANDLE;

// *****************************************************************************
/*
  Type:
    TCPIP_UDP_PROCESS_HANDLE

  Summary:
    Defines a UDP packet processing handle.

  Description:
    Definition of an packet processing handle used for
    packet processing registration by the UDP clients.

*/
typedef const void* TCPIP_UDP_PROCESS_HANDLE;

// *****************************************************************************
/* UDP packet handler Pointer

  Function:
    bool <FunctionName> (TCPIP_NET_HANDLE hNet, struct _tag_TCPIP_MAC_PACKET* rxPkt, const void* hParam);

  Summary:
    Pointer to a function(handler) that will get called to process an incoming UDP packet.

  Description:
    Pointer to a function that will be called by the UDP module
    when a RX packet is available.

  Precondition:
    None

  Parameters:
    hNet        - network handle on which the packet has arrived
    rxPkt       - pointer to incoming packet
    hParam      - user passed parameter when handler was registered

  Returns:
    true - if the packet is processed by the external handler.
           In this case the UDP module will no longer process the packet
    false - the packet needs to be processed internally by the UDP as usual           

  Remarks:
    The packet handler is called in the UDP context.
    The handler should be kept as short as possible as it affects the processing of all the other
    UDP RX traffic.

    Before calling the external packet handler 
    - the rxPkt->pktFlags has the bit 9 (value 0x0200) set for an IPv6 packet, cleared for IPv4
    - the rxPkt->pTransportLayer points to an UDP_HEADER data structure.
    - the rxPkt->pNetLayer points to an IPV4_HEADER/IPV6_HEADER data structure.
    - the rxPkt->pktIf points to the interface receiving the packet
    - the first data segment segLen is adjusted to store the size of the UDP data 

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
typedef bool(*TCPIP_UDP_PACKET_HANDLER)(TCPIP_NET_HANDLE hNet, struct _tag_TCPIP_MAC_PACKET* rxPkt, const void* hParam);


// *****************************************************************************
/*
  Structure:
    TCPIP_UDP_MODULE_CONFIG

  Summary:
    UDP module run time configuration/initialization data.

  Description:
    UDP module configuration/initialization
*/
//
typedef struct
{
    uint16_t        nSockets;       // number of sockets to be created
    uint16_t        sktTxBuffSize;  // default size of the socket TX buffer
    uint16_t        poolBuffers;    // number of buffers in the pool; 0 if none
    uint16_t        poolBufferSize; // size of the buffers in the pool; all equal    
}TCPIP_UDP_MODULE_CONFIG;


// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************

/*
  Function:
     UDP_SOCKET TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE addType, UDP_PORT localPort,  
                                     IP_MULTI_ADDRESS* localAddress)

  Summary:
    Opens a UDP socket as a server.
    
  Description:
    Provides a unified method for opening UDP server sockets. 

  Precondition:
    UDP is initialized.

  Parameters:
    IP_ADDRESS_TYPE addType         -   The type of address being used. 
                                        Example: IP_ADDRESS_TYPE_IPV4 or IP_ADDRESS_TYPE_IPV6.
                                        It can be IP_ADDRESS_TYPE_ANY and the server socket will accept any incoming
                                        type of connection.
    
    UDP_PORT localPort              -   UDP port on which to listen for connections
    
    IP_MULTI_ADDRESS* localAddress  -   Pointer to a local IP address to use.
                                        This needs to be the valid IP address of a local interface
                                        the server is to listen on
                                        or NULL if any incoming interface will do.
    
  Returns:
    - INVALID_SOCKET -  No sockets of the specified type were available to be
                      opened or parameter error.

    - A UDP_SOCKET handle - Save this handle and use it when calling all other UDP APIs. 

  Remarks:
    Sockets and user threads protection
        For efficiency reasons, there is NO PROTECTION for each individual API call
        except to Open and Close sockets!
        What it means is that:
            - the user application should close all its sockets before issuing
              a stack/if down command
              The stack manager takes care of the internally used sockets

            - A socket can NOT be used concurrently from multiple threads!
              It's ok to pass a socket from one thread to another as long as
              there's is no access from more than one thread at a time

 */
UDP_SOCKET          TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE addType, UDP_PORT localPort,  
                    IP_MULTI_ADDRESS* localAddress);

// *****************************************************************************

/*
  Function:
     UDP_SOCKET TCPIP_UDP_ClientOpen(IP_ADDRESS_TYPE addType, UDP_PORT remotePort, 
                          IP_MULTI_ADDRESS* remoteAddress)

  Summary:
    Opens a UDP socket as a client.
    
  Description:
    Provides a unified method for opening UDP client sockets. 

  Precondition:
    UDP is initialized.

  Parameters:
    IP_ADDRESS_TYPE addType - The type of address being used. Example: IP_ADDRESS_TYPE_IPV4 or IP_ADDRESS_TYPE_IPV6.
                              It can also be IP_ADDRESS_TYPE_ANY if the type of the socket will be specified later
                              (using TCPIP_UDP_Bind, TCPIP_UDP_RemoteBind).
    UDP_PORT remotePort     - The remote UDP port to which a connection should be made.
                              The local port for client sockets will be automatically picked
                              by the UDP module.                        
    IP_MULTI_ADDRESS* remoteAddress -   The remote address to connect to.
                              Not used if addType == IP_ADDRESS_TYPE_ANY.
                              Can be NULL if later set by a call to TCPIP_UDP_RemoteBind.
                              
    
  Returns:
    - INVALID_SOCKET -  No sockets of the specified type were available to be opened.

    - A UDP_SOCKET handle - Save this handle and use it when calling all other UDP APIs. 

  Remarks:
    Sockets and user threads protection
        For efficiency reasons, there is NO PROTECTION for each individual API call
        except to Open and Close sockets!
        What it means is that:
            - the user application should close all its sockets before issuing
              a stack/if down command
              The stack manager takes care of the internally used sockets

            - A socket can NOT be used concurrently from multiple threads!
              It's ok to pass a socket from one thread to another as long as
              there's is no access from more than one thread at a time


 */
UDP_SOCKET          TCPIP_UDP_ClientOpen(IP_ADDRESS_TYPE addType, UDP_PORT remotePort, 
                   IP_MULTI_ADDRESS* remoteAddress);

// *****************************************************************************

/*
  Function:
    bool TCPIP_UDP_Bind(UDP_SOCKET hUDP, IP_ADDRESS_TYPE addType, UDP_PORT localPort,  
                        IP_MULTI_ADDRESS* localAddress);

  Summary:
    Bind a socket to an address type, local port and address.
    This function is meant primarily for client sockets.

  Description:
    Sockets don't need specific binding, it is done automatically by the stack
    However, specific binding can be requested using this function.
    Works for both client and server sockets.
    For a server socket it will restrict accepting connections of a specific type
    on a specific interface only.
    For a client socket it will force a specific address type and a local
    port and interface.

  Precondition:
    UDP socket should have been opened with TCPIP_UDP_ServerOpen()/TCPIP_UDP_ClientOpen()().
    hUDP - valid socket

  Parameters:
    hUDP            -   The socket to bind.
    addType         -   The type of address being used.
                        Valid values are IP_ADDRESS_TYPE_ANY/IP_ADDRESS_TYPE_IPV4/IP_ADDRESS_TYPE_IPV6.
    localPort       -   The local port to bind to.
    localAddress    -   Local address to use.
    
  Returns:
    - true  - Indicates success
    - false - Indicates failure

  Remarks:
    If address type IP_ADDRESS_TYPE_ANY is used, the localAddress parameter won't be used
    and the socket won't change the local address.
    However, for the call to succeed, socket must have been created with
    IP_ADDRESS_TYPE_ANY type and must not be currently bound to a IP_ADDRESS_TYPE_IPV4/IP_ADDRESS_TYPE_IPV6 type.

    If address type IP_ADDRESS_TYPE_IPV4/IP_ADDRESS_TYPE_IPV6 is used,
    then the socket type will be changed accordingly, and the localAddress will be used,
    if provided.
    The socket will be bound to this type of address.
    For the call to succeed the socket must currently have the type IP_ADDRESS_TYPE_ANY or to
    match the addType parameter.

    If localAddress is the valid address of a network interface,
    and addType != IP_ADDRESS_TYPE_ANY,
    then the call will enforce UDP_OPTION_STRICT_NET on the socket.

    If localPort is 0, the stack will assign a unique local port
    (if the socket doesn't already have a unique local port)


    In order to change dynamically the type of address,
    the socket must have been created with a IP_ADDRESS_TYPE_ANY type
    and not currently bound to a different address type.
    TCPIP_UDP_Disconnect could be issued to remove a previous IP type binding.
  */
bool   TCPIP_UDP_Bind(UDP_SOCKET hUDP, IP_ADDRESS_TYPE addType, UDP_PORT localPort,  
                      IP_MULTI_ADDRESS* localAddress);

// *****************************************************************************

/*
  Function:
    bool TCPIP_UDP_RemoteBind(UDP_SOCKET hUDP, IP_ADDRESS_TYPE addType, 
                        UDP_PORT remotePort,  IP_MULTI_ADDRESS* remoteAddress);

  Summary:
    Bind a socket to an address type, remote port and address.
    This function is meant primarily for server sockets.

  Description:
    Sockets don't need specific remote binding, they should accept connections on 
    any incoming interface. Therefore, the binding is done automatically by the stack.
    However, specific remote binding can be requested using this function.
    For a server socket it can be used to restrict accepting connections from  a 
    specific remote host.
    For a client socket it will just change the default binding done when the socket was opened.

  Precondition:
    UDP socket should have been opened with TCPIP_UDP_ServerOpen/TCPIP_UDP_ClientOpen.
    hUDP - valid socket

  Parameters:
    hUDP            -   The socket to bind.
    addType         -   The type of address being used.
                        Valid values are IP_ADDRESS_TYPE_ANY/IP_ADDRESS_TYPE_IPV4/IP_ADDRESS_TYPE_IPV6.
    remotePort      -   The remote port to bind to.
    remoteAddress   -   Remote address to use.
    
  Returns:
    - true  - Indicates success
    - false - Indicates failure

  Remarks:
    If address type IP_ADDRESS_TYPE_ANY is used, the remoteAddress parameter won't be used
    and the socket won't change the remote destination address.
    However, for the call to succeed, socket must have been created with
    IP_ADDRESS_TYPE_ANY type and must not be currently bound to a IP_ADDRESS_TYPE_IPV4/IP_ADDRESS_TYPE_IPV6 type.

    If address type IP_ADDRESS_TYPE_IPV4/IP_ADDRESS_TYPE_IPV6 is used,
    then the socket type will be changed accordingly, and the remoteAddress will be used,
    if provided.
    The socket will be bound to this type of address.
    For the call to succeed the socket must currently have the type IP_ADDRESS_TYPE_ANY or to
    match the addType parameter.

    If the remoteAddress != 0 and addType != IP_ADDRESS_TYPE_ANY,
    the call will enforce UDP_OPTION_STRICT_ADDRESS on the socket.
    
    The remote port is always changed, even if remotePort == 0.
    It will enforce UDP_OPTION_STRICT_PORT on the socket.
    
    In order to change dynamically the type of address,
    the socket must have been created with a IP_ADDRESS_TYPE_ANY type
    and not currently bound to a different address type.
    TCPIP_UDP_Disconnect could be issued to remove a previous IP type binding.

    If the socket is a server socket and is bound to a remote port,
    the TCPIP_UDP_Disconnect won't remove the port binding.

  */
bool   TCPIP_UDP_RemoteBind(UDP_SOCKET hUDP, IP_ADDRESS_TYPE addType, UDP_PORT remotePort,  
                            IP_MULTI_ADDRESS* remoteAddress);

// *****************************************************************************
/*
  Function:
    bool TCPIP_UDP_OptionsSet(UDP_SOCKET hUDP, UDP_SOCKET_OPTION option, void* optParam);

  Summary:
    Allows setting options to a socket like adjust RX/TX buffer size, etc

  Description:
    Various options can be set at the socket level.

  Precondition:
    UDP socket should have been opened with TCPIP_UDP_ServerOpen()/TCPIP_UDP_ClientOpen()().
    hUDP - valid socket


  Parameters:
    hUDP            - socket to set options for 
    option          - specific option to be set 
    optParam        - the option value; this is option dependent:   
                      - UDP_OPTION_STRICT_PORT      - boolean enable/disable
                      - UDP_OPTION_STRICT_NET       - boolean enable/disable
                      - UDP_OPTION_STRICT_ADDRESS   - boolean enable/disable
                      - UDP_OPTION_BROADCAST        - UDP_SOCKET_BCAST_TYPE
                      - UDP_OPTION_BUFFER_POOL      - boolean enable/disable
                                                      Note: Changing the UDP_OPTION_BUFFER_POOL will discard
                                                            the data in the current socket buffer 
                      - UDP_OPTION_TX_BUFF          - 16-bit value in bytes of the TX buffer
                                                      Note: the UDP_OPTION_TX_BUFF will discard
                                                         the data in the current socket buffer 
                      - UDP_OPTION_TX_QUEUE_LIMIT   - 16-bit value of the TX queue limit
                      - UDP_OPTION_RX_QUEUE_LIMIT   - 16-bit value of the RX queue limit
                      - UDP_OPTION_RX_AUTO_ADVANCE  - boolean enable/disable
                      - UDP_OPTION_TX_TTL           - 8-bit value of TTL
                      - UDP_OPTION_MULTICAST        - pointer to a UDP_OPTION_MULTICAST_DATA structure
                      - UDP_OPTION_TOS              - 8-bit value of the TOS
                      - UDP_OPTION_DF               - boolean - true: no fragmentation allowed; false: fragmentation allowed
                      - UDP_OPTION_FIXED_DEST_ADDRESS - boolean - true: set fixed destination address; false: clear the fixed destination address
                      - UDP_OPTION_FIXED_DEST_PORT  - boolean - true: set fixed destination port; false: clear the fixed destination port
                      - UDP_OPTION_ENFORCE_STRICT_NET   - boolean - true: enforce strictness (default); false: do not enforce strictness 

  Returns:
    - true  - Indicates success
    - false - Indicates failure

  Remarks:  
    This function provides the run-time functionality
    required to implement some of the standard BSD socket options API.

    The option parameter may be flagged with UDP_OPTION_STRICT_SET_STICKY or UDP_OPTION_STRICT_CLR_STICKY

  */    
bool                TCPIP_UDP_OptionsSet(UDP_SOCKET hUDP, UDP_SOCKET_OPTION option, void* optParam);


// *****************************************************************************
/*
  Function:
    bool TCPIP_UDP_OptionsGet(UDP_SOCKET hUDP, UDP_SOCKET_OPTION option, void* optParam);

  Summary:
    Allows getting the options for a socket such as current RX/TX buffer size, etc.

  Description:
    Various options can be retrieved at the socket level.
    This function provides compatibility with BSD implementations.

  Precondition:
    UDP socket should have been opened with TCPIP_UDP_ServerOpen()/TCPIP_UDP_ClientOpen()().
    hUDP - valid socket


  Parameters:
    hUDP            - socket to get options for 
    option          - specific option to get    
    optParam        - pointer to an area that will receive the option value; this is option dependent
                      the size of the area has to be large enough to accommodate the specific option:    
                      - UDP_OPTION_STRICT_PORT      - pointer to boolean
                      - UDP_OPTION_STRICT_NET       - pointer to boolean
                      - UDP_OPTION_STRICT_ADDRESS   - pointer to boolean
                      - UDP_OPTION_BROADCAST        - pointer to UDP_SOCKET_BCAST_TYPE
                      - UDP_OPTION_BUFFER_POOL      - pointer to boolean
                      - UDP_OPTION_TX_BUFF          - pointer to a 16 bit value to receive bytes of the TX buffer
                      - UDP_OPTION_TX_QUEUE_LIMIT   - pointer to an 16 bit value to receive the TX queue limit
                      - UDP_OPTION_RX_QUEUE_LIMIT   - pointer to an 16 bit value to receive the RX queue limit
                      - UDP_OPTION_RX_AUTO_ADVANCE  - pointer to boolean
                      - UDP_OPTION_TX_TTL           - pointer to an 8 bit value to receive the TTL value
                      - UDP_OPTION_MULTICAST        - pointer to a UDP_MULTICAST_FLAGS value to receive the current socket settings
                      - UDP_OPTION_TOS              - pointer to an 8 bit value to receive the TOS
                      - UDP_OPTION_DF               - pointer to boolean - true: no fragmentation allowed; false: fragmentation allowed
                      - UDP_OPTION_FIXED_DEST_ADDRESS - pointer boolean - true: fixed destination address is set; false: fixed destination address is cleared
                      - UDP_OPTION_FIXED_DEST_PORT  - pointer boolean - true: fixed destination port is set; false: fixed destination port is cleared
                      - UDP_OPTION_ENFORCE_STRICT_NET - pointer to boolean - false:  strictness not enforced; true: strictness enforced (default)

  Returns:
    - true  - Indicates success
    - false - Indicates failure
  */    
bool                TCPIP_UDP_OptionsGet(UDP_SOCKET hUDP, UDP_SOCKET_OPTION option, void* optParam);


// *****************************************************************************

/*
  Function:
      bool TCPIP_UDP_IsConnected(UDP_SOCKET hUDP)
  
 Summary:
      Determines if a socket has an established connection.

 Description:
    This function determines if a socket has an established connection to a remote node.  
    Call this function after calling TCPIP_UDP_ServerOpen/TCPIP_UDP_ClientOpen
    to determine when the connection is set up and ready for use.  

 Precondition:
    None.

 Parameters:
    hUDP - The socket to check.

 Return Values:
    - true  - The socket has been opened.
    - false - The socket is not currently opened.

 Remarks:
    An UDP server or client socket can always send data,
    as long as it has a valid destination address,
    even if there's no remote socket connected to it.
    This function will return true if the socket is opened
    and currently has a valid destination.
    Note that this is true if a client socket was opened with a
    remote host address, a socket received data from a remote host,
    or a explicit remote bind was done.
 */
bool                TCPIP_UDP_IsConnected(UDP_SOCKET hUDP);

// *****************************************************************************

/*
  Function:
    bool TCPIP_UDP_IsOpened(UDP_SOCKET hUDP)
  
 Summary:
    Determines if a socket was opened.

 Description:
    This function determines if a socket was opened.  

 Precondition:
    None.

 Parameters:
    hUDP - The socket to check.

 Return Values:
    - true  - The socket has been opened.
    - false - The socket is not currently opened.

 Remarks:
    This is a backward compatibility call.

 */
#define             TCPIP_UDP_IsOpened(hUDP)      TCPIP_UDP_IsConnected(hUDP)       


// *****************************************************************************

/*
  Function:
    bool TCPIP_UDP_Close(UDP_SOCKET hUDP)

  Summary:
    Closes a UDP socket and frees the handle.
    
  Description:
    Closes a UDP socket and frees the handle.  Call this function to release
    a socket and return it to the pool for use by future communications.

  Precondition:
    UDP socket should have been opened with TCPIP_UDP_ServerOpen/TCPIP_UDP_ClientOpen.
    hUDP - valid socket

  Parameters:
    hUDP - The socket handle to be released.

  Returns:
    - true  - If the call succeeded
    - false - If the call failed (no such socket)
    
  Remarks:
    Always close the socket when no longer in use.
    This will free the allocated resources, including the TX buffers.

  */
bool                TCPIP_UDP_Close(UDP_SOCKET hUDP);

// *****************************************************************************

/*
  Function:
    bool TCPIP_UDP_Disconnect(UDP_SOCKET hUDP, bool flushRxQueue)

  Summary:
    Disconnects a UDP socket and re-initializes it.
    
  Description:
    Disconnects a UDP socket and re-initializes it.
    Call this function to return the socket to its initial open state
    and to use it for future communication.

    This function is meant especially for server sockets that could listen
    on multiple interfaces and on both IPv4 and IPv6 networks.

    When a server socket received an inbound IPv4 connection it will be
    bound to IPv4 connections until it's closed or disconnected.
    Same is true for IPv6 connections.


  Precondition:
    UDP socket should have been opened with TCPIP_UDP_ServerOpen()/TCPIP_UDP_ClientOpen()().
    hUDP - valid socket

  Parameters:
    hUDP            - The socket handle to be disconnected.
    flushRxQueue    - boolean to flush the pending RX queue

  Returns:
    - true  - Indicates success
    - false - Indicates failure
    
  Remarks:
    The call will try to maintain as much as possible from the socket state.
   
    For a server socket the remote port will be cleared unless a TCPIP_UDP_RemoteBind() was performed.
    If the remote port is cleared, a new TCPIP_UDP_RemoteBind or TCPIP_UDP_DestinationPortSet operation
    will be needed if the remote port needs to be maintained.

    For a client socket the remote port is maintained.

    This will free the allocated TX buffers if the socket was opened with IP_ADDRESS_TYPE_ANY.

    All the pending RX packets will be cleared when flushRxQueue is set.
    Otherwise the packets will be kept and will be available for next read operations.

    Note that if the RX pending packets exist and are not flushed,
    the socket will be bound immediately to the connection corresponding
    to the pending packet in the queue.
    
  */
bool                TCPIP_UDP_Disconnect(UDP_SOCKET hUDP, bool flushRxQueue);

// *****************************************************************************

/*
  Function:
    bool TCPIP_UDP_SocketInfoGet(UDP_SOCKET hUDP, UDP_SOCKET_INFO* pInfo)

  Summary:
    Returns information about a selected UDP socket.
    
  Description:
    This function will fill a user passed UDP_SOCKET_INFO structure
    with current status of the selected socket.
    

  Precondition:
    UDP socket should have been opened with TCPIP_UDP_ServerOpen()/TCPIP_UDP_ClientOpen()().
    hUDP - valid socket
    pInfo - valid address of a UDP_SOCKET_INFO structure

  Parameters:
    UDP_SOCKET hUDP - Socket for which information is to be obtained
    UDP_SOCKET_INFO* pInfo - pointer to UDP_SOCKET_INFO to receive socket information

  Returns:
    - true  - if the call succeeded
    - false - if no such socket or invalid pInfo

  */
bool                TCPIP_UDP_SocketInfoGet(UDP_SOCKET hUDP, UDP_SOCKET_INFO* pInfo);

// *****************************************************************************

/*
  Function:
    bool TCPIP_UDP_TxOffsetSet(UDP_SOCKET hUDP, uint16_t wOffset, bool relative)

  Summary:
    Moves the pointer within the TX buffer.
    
  Description:
    This function allows the write location within the TX buffer to be 
    specified. Future calls to TCPIP_UDP_Put, TCPIP_UDP_ArrayPut, TCPIP_UDP_StringPut, etc
    will write data from the indicated location.

  Precondition:
    UDP socket should have been opened with TCPIP_UDP_ServerOpen/TCPIP_UDP_ClientOpen.
    hUDP - valid socket

  Parameters:
    hUDP    - UDP socket handle 
    wOffset - Offset in the UDP packet data payload to move the write pointer.
    relative- if true, the wOffset is added to the current write pointer.
              else the wOffset is from the beginning of the UDP buffer

  Returns:
    - true  - if the offset was valid and the write pointer has been moved
    - false - if the offset was not valid
  */
bool                TCPIP_UDP_TxOffsetSet(UDP_SOCKET hUDP, uint16_t wOffset, bool relative);

// *****************************************************************************

/*
  Function:
    uint16_t TCPIP_UDP_TxPutIsReady(UDP_SOCKET hUDP, unsigned short count)

  Summary:
    Determines how many bytes can be written to the UDP socket.
    
  Description:
    This function returns the number of bytes that can be written to the specified UDP
    socket.
  
  Precondition:
    UDP socket should have been opened with TCPIP_UDP_ServerOpen/TCPIP_UDP_ClientOpen.
    hUDP - valid socket

  Parameters:
    hUDP  - UDP socket handle
    count - Number of bytes requested

  Returns:
    The number of bytes that can be written to this socket.

  Remarks:
    The function won't increase the size of the UDP TX buffer.
    If this is needed use TCPIP_UDP_OptionsSet.
    The count variable is not used.

    This function is deprecated and it will be eventually removed.
    The function is identical to the TCPIP_UDP_PutIsReady and maintained for backward compatibility only.

  */
uint16_t            TCPIP_UDP_TxPutIsReady(UDP_SOCKET hUDP, unsigned short count);

// *****************************************************************************

/*
  Function:
    uint16_t TCPIP_UDP_PutIsReady(UDP_SOCKET hUDP)

  Summary:
    Determines how many bytes can be written to the UDP socket.
    
  Description:
    This function determines how many bytes can be written to the specified UDP
    socket.
    This function performs TX buffer allocation for the socket.
  
  Precondition:
    UDP socket should have been opened with TCPIP_UDP_ServerOpen/TCPIP_UDP_ClientOpen.
    hUDP - valid socket

  Parameters:
    hUDP  - UDP socket handle

  Returns:
    The number of bytes that can be written to this socket.

  Remarks:
    If the current socket TX buffer is in use (in traffic), this function will allocate 
    a new TX buffer.
    Otherwise the current TX buffer will be used.

    The function SHOULD be called before using TCPIP_UDP_ArrayPut
    for the first time or after any TCPIP_UDP_Flush.
    There is no harm in calling it everytime before any TCPIP_UDP_ArrayPut
    (except the overhead).

  */
uint16_t            TCPIP_UDP_PutIsReady(UDP_SOCKET hUDP);

// *****************************************************************************

/*
  Function:
    uint16_t TCPIP_UDP_ArrayPut(UDP_SOCKET hUDP, const uint8_t *cData, uint16_t wDataLen)

  Summary:
    Writes an array of bytes to the UDP socket.
    
  Description:
    This function writes an array of bytes to the UDP socket, 
    while incrementing the socket write pointer.

    TCPIP_UDP_PutIsReady should be used before calling this function
    to verify that there is room in the socket buffer.

  Precondition:
    UDP socket should have been opened with TCPIP_UDP_ServerOpen/TCPIP_UDP_ClientOpen.
    hUDP - valid socket
    cData - valid pointer

  Parameters:
    hUDP  - UDP socket handle
    cData - The array to write to the socket.
    wDataLen - Number of bytes from cData to be written.
    
  Returns:
    The number of bytes successfully placed in the UDP transmit buffer.
    If this value is less than wDataLen, then the buffer became full and the
    input was truncated.

  Remarks:
    The return value could be 0 if there is no TX buffer available
    (for example the socket has TX data queued and the TCPIP_UDP_PutIsReady
    has not been called or the TX buffer allocation failed).

    The return value could be < than wDataLen depending on the size and the available space
    of the socket TX buffer

  */
uint16_t            TCPIP_UDP_ArrayPut(UDP_SOCKET hUDP, const uint8_t *cData, uint16_t wDataLen);

// *****************************************************************************

/*
  Function:
    uint8_t* TCPIP_UDP_StringPut(UDP_SOCKET hUDP, const uint8_t *strData)

  Summary:
    Writes a null-terminated string to the UDP socket.
    
  Description:
    This function writes a null-terminated string to the UDP socket
    while incrementing the socket write pointer.
    TCPIP_UDP_PutIsReady could be used before
    calling this function to verify that there is room in the socket buffer.

  Precondition:
    UDP socket should have been opened with TCPIP_UDP_ServerOpen/TCPIP_UDP_ClientOpen.
    hUDP    - valid socket
    strData - valid pointer

  Parameters:
    hUDP     - UDP socket handle
    strData  - Pointer to the string to be written to the socket.
    
  Returns:
    A pointer to the byte following the last byte written.
    Note that this is different than the TCPIP_UDP_ArrayPut functions.
    If this pointer does not dereference to a NULL byte,
    then the buffer became full and the input data was truncated.

  */
const uint8_t*      TCPIP_UDP_StringPut(UDP_SOCKET hUDP, const uint8_t *strData);

// *****************************************************************************

/*
  Function:
    uint16_t TCPIP_UDP_TxCountGet(UDP_SOCKET hUDP)

  Summary:
    Returns the amount of bytes written into the UDP socket.
    
  Description:
    This function returns the amount of bytes written into the UDP socket,
    i.e. the current position of the write pointer. 

  Precondition:
    UDP socket should have been opened with TCPIP_UDP_ServerOpen/TCPIP_UDP_ClientOpen.
    hUDP - valid socket

  Parameters:
    hUDP   - UDP socket handle

  Return Values:
    The number of bytes in the socket TX buffer; otherwise, 0 if the socket is invalid.

  */
uint16_t            TCPIP_UDP_TxCountGet(UDP_SOCKET hUDP);

// *****************************************************************************

/*
  Function:
    uint16_t TCPIP_UDP_Flush(UDP_SOCKET hUDP)

  Summary:
    Transmits all pending data in a UDP socket.
    
  Description:
    This function builds a UDP packet with the pending TX data and marks it 
    for transmission over the network interface.
    There is no UDP state machine to send the socket data automatically.
    The UDP socket client must call this function to actually send the data over
    the network.

  Precondition:
    UDP socket should have been opened with TCPIP_UDP_ServerOpen/TCPIP_UDP_ClientOpen.
    hUDP - valid socket

  Parameters:
    hUDP   - UDP socket handle
    
  Returns:
    The number of bytes that currently were in the socket TX buffer
    and have been flushed. Otherwise, 0 if the packet could not be transmitted:
    - invalid socket
    - invalid remote address
    - no route to the remote host could be found

  Remarks:
    Note that a UDP socket must be flushed to send data over the network.
    There is no UDP state machine (auto transmit) for UDP sockets.

  */
uint16_t            TCPIP_UDP_Flush(UDP_SOCKET hUDP);

// *****************************************************************************

/*
  Function:
    uint16_t TCPIP_UDP_Put(UDP_SOCKET hUDP, uint8_t v)

  Summary:
    Writes a byte to the UDP socket.
    
  Description:
    This function writes a single byte to the UDP socket, 
    while incrementing the socket write pointer.
    TCPIP_UDP_PutIsReady could be used before calling this function
    to verify that there is room in the socket buffer.

  Precondition:
    UDP socket should have been opened with TCPIP_UDP_ServerOpen/TCPIP_UDP_ClientOpen.
    hUDP - valid socket

  Parameters:
    hUDP   - UDP socket handle
    v - The byte to be loaded into the transmit buffer.

  Return Values: The number of bytes successfully written to the socket
    - 1 - The byte was successfully written to the socket.
    - 0 - The transmit buffer is already full and so the write failed
          or the socket is invalid.
    
  Remarks:
    This function is very inefficient and its use is discouraged.
    A buffered approach (TCPIP_UDP_ArrayPut) is preferred.

  */
uint16_t                TCPIP_UDP_Put(UDP_SOCKET hUDP, uint8_t v);

// *****************************************************************************

/*
  Function:
    uint16_t TCPIP_UDP_GetIsReady(UDP_SOCKET hUDP)

  Summary:
    Determines how many bytes can be read from the UDP socket.
    
  Description:
    This function will return the number of bytes that are available
    in the specified UDP socket RX buffer.
  
  Precondition:
    UDP socket should have been opened with TCPIP_UDP_ServerOpen/TCPIP_UDP_ClientOpen.
    hUDP - valid socket

  Parameters:
    hUDP   - UDP socket handle

  Returns:
    The number of bytes that can be read from this socket.

  Remarks:
    The UDP socket queues incoming RX packets in an internal queue.

    If currently there is no RX packet processed (as a result of retrieving
    all available bytes with TCPIP_UDP_ArrayGet, for example),
    this call will advance the RX packet to be processed to the next
    queued packet.

    If a RX packet is currently processed, the call will return the number of bytes
    left to be read from this packet.

  */
uint16_t            TCPIP_UDP_GetIsReady(UDP_SOCKET hUDP);

// *****************************************************************************

/*
  Function:
    void TCPIP_UDP_RxOffsetSet(UDP_SOCKET hUDP, uint16_t wOffset)

  Summary:
    Moves the read pointer within the socket RX buffer.
    
  Description:
    This function allows the user to specify the read location within the socket RX buffer.
    Future calls to TCPIP_UDP_Get and TCPIP_UDP_ArrayGet will read data from
    the indicated location forward.

  Precondition:
    UDP socket should have been opened with TCPIP_UDP_ServerOpen/TCPIP_UDP_ClientOpen.
    hUDP - valid socket

  Parameters:
    hUDP    - UDP socket handle
    wOffset - Offset from beginning of UDP packet data payload to place the
              read pointer.

  Returns:
    None.

  */
void                TCPIP_UDP_RxOffsetSet(UDP_SOCKET hUDP, uint16_t rOffset);

// *****************************************************************************

/*
  Function:
    uint16_t TCPIP_UDP_ArrayGet(UDP_SOCKET hUDP, uint8_t *cData, uint16_t wDataLen)

  Summary:
    Reads an array of bytes from the UDP socket.
    
  Description:
    This function reads an array of bytes from the UDP socket, 
    while adjusting the current read pointer and decrementing
    the remaining bytes available.
    TCPIP_UDP_GetIsReady should be used before calling this function
    to get the number of the available bytes in the socket.

    
  Precondition:
    UDP socket should have been opened with TCPIP_UDP_ServerOpen/TCPIP_UDP_ClientOpen.
    hUDP - valid socket

  Parameters:
    hUDP     - UDP socket handle
    cData    - The buffer to receive the bytes being read.
               If NULL, the bytes are simply discarded 
    wDataLen - Number of bytes to be read from the socket.
    
  Returns:
    The number of bytes successfully read from the UDP buffer.
    If this value is less than wDataLen, then the buffer was emptied
    and no more data is available.

  Remarks:
    For discarding a number of bytes in the RX buffer the TCPIP_UDP_RxOffsetSet()
    can also be used.

    The UDP socket queues incoming RX packets in an internal queue.
    This call will try to retrieve the bytes from the current processing packet
    but it won't advance the processed packet.
    TCPIP_UDP_GetIsReady should be called to advance the processed RX packet.

    TCPIP_UDP_Discard should be called when done processing the current RX packet.

  */
uint16_t            TCPIP_UDP_ArrayGet(UDP_SOCKET hUDP, uint8_t *cData, uint16_t wDataLen);

// *****************************************************************************

/*
  Function:
    uint16_t TCPIP_UDP_Get(UDP_SOCKET hUDP, uint8_t *v)

  Summary:
    Reads a byte from the UDP socket.
    
  Description:
    This function reads a single byte from the UDP socket, 
    while decrementing the remaining RX buffer length.
    TCPIP_UDP_GetIsReady should be used before calling this function
    to get the number of bytes available in the socket.

  Precondition:
    UDP socket should have been opened with TCPIP_UDP_ServerOpen/TCPIP_UDP_ClientOpen.
    hUDP - valid socket

  Parameters:
    hUDP   - socket handle
    v - The buffer to receive the data being read.

  Return Values: The number of bytes successfully read
    - 1 - A byte was successfully read
    - 0 - No data available in the read buffer or invalid socket

  Remarks:
    This function is very inefficient and its usage is discouraged.
    A buffered approach (TCPIP_UDP_ArrayGet) is preferred.

    See the previous notes for TCPIP_UDP_ArrayGet function.
 */
uint16_t                TCPIP_UDP_Get(UDP_SOCKET hUDP, uint8_t *v);

// *****************************************************************************

/*
  Function:
    uint16_t TCPIP_UDP_Discard(UDP_SOCKET hUDP)

  Summary:
    Discards any remaining RX data from a UDP socket.
    
  Description:
    This function discards any remaining received data in the UDP socket.

  Precondition:
    UDP socket should have been opened with TCPIP_UDP_ServerOpen/TCPIP_UDP_ClientOpen.
    hUDP - valid socket

  Parameters:
    hUDP   - socket handle
    
  Returns:
    Number of discarded bytes, if any.

  Remarks:
    The UDP socket queues incoming RX packets in an internal queue.

    This call will discard the remaining bytes (if any) in the current RX packet
    and will advance the RX packet to be processed to the next
    queued packet.

    This function should be normally called after retrieving the available bytes
    with TCPIP_UDP_ArrayGet.

    When data available, calling it repeatedly will discard
    one pending RX packet at a time.

    Note that a call to TCPIP_UDP_Discard is not needed if all bytes
    are retrieved with  TCPIP_UDP_ArrayGet and then TCPIP_UDP_GetIsReady
    is called. 

  */
uint16_t               TCPIP_UDP_Discard(UDP_SOCKET hUDP);


// *****************************************************************************

/*
  Function:
    bool TCPIP_UDP_SocketNetSet(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet)

  Summary:
    Sets the network interface for an UDP socket
    
  Description:
    This function sets the network interface for an UDP socket

  Precondition:
    UDP socket should have been opened with TCPIP_UDP_ServerOpen/TCPIP_UDP_ClientOpen.
    hUDP - valid socket

  Parameters:
    hUDP - The UDP socket
    hNet - interface handle
    
  Returns:
    - true  - Indicates success
    - false - Indicates failure

  Remarks:
    A NULL hNet can be passed (0) so that the current
    socket network interface selection will be cleared

    If the hNet != 0, it will enforce UDP_OPTION_STRICT_NET on the socket.

  */
bool                TCPIP_UDP_SocketNetSet(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet);

// *****************************************************************************

/*
  Function:
    TCPIP_NET_HANDLE TCPIP_UDP_SocketNetGet(UDP_SOCKET hUDP)

  Summary:
    Gets the network interface of an UDP socket
    
  Description:
    This function returns the interface handle of an UDP socket

  Precondition:
    UDP socket should have been opened with TCPIP_UDP_ServerOpen/TCPIP_UDP_ClientOpen.
    hUDP - valid socket

  Parameters:
    hUDP - The UDP socket
    
  Returns:
    Handle of the interface that socket currently uses.

  */
TCPIP_NET_HANDLE    TCPIP_UDP_SocketNetGet(UDP_SOCKET hUDP);

// *****************************************************************************

/*
  Function:
    bool TCPIP_UDP_BcastIPV4AddressSet(UDP_SOCKET hUDP, UDP_SOCKET_BCAST_TYPE bcastType, 
                                       TCPIP_NET_HANDLE hNet)

  Summary:
    Sets the broadcast IP address of a socket
    Allows an UDP socket to send broadcasts.
    
  Description:
      It sets the broadcast address for the socket


  Precondition:
    UDP initialized
    UDP socket should have been opened with TCPIP_UDP_ServerOpen()/TCPIP_UDP_ClientOpen()().
    hUDP  - valid socket

  Parameters:
    hUDP            -   the UDP socket
    bcastType       -   Type of broadcast
    hNet            -   handle of an interface to use for the network directed broadcast
                        Not used for network limited broadcast
    
  Returns:
    - true  - Indicates success
    - false - Indicates failure:
      - invalid socket
      - invalid socket address type
      - a broadcast for the specified interface could not be obtained
      - invalid broadcast type specified

  Remarks:
    This function allows changing of the destination IPv4 address dynamically.

    However, the call will fail if the socket was previously set to broadcast
    using the TCPIP_UDP_OptionsSet call.
    TCPIP_UDP_OptionsSet takes precedence.

  */
bool   TCPIP_UDP_BcastIPV4AddressSet(UDP_SOCKET hUDP, UDP_SOCKET_BCAST_TYPE bcastType, 
                                    TCPIP_NET_HANDLE hNet);

// *****************************************************************************

/*
  Function:
    bool TCPIP_UDP_DestinationIPAddressSet(UDP_SOCKET hUDP, IP_ADDRESS_TYPE addType, 
                                    IP_MULTI_ADDRESS* remoteAddress)

  Summary:
    Sets the destination IP address of a socket
    
  Description:
      - It sets the IP destination address
        This allows changing the IP destination address dynamically.


  Precondition:
    UDP initialized
    UDP socket should have been opened with TCPIP_UDP_ServerOpen()/TCPIP_UDP_ClientOpen()().
    hUDP  - valid socket
    remoteAddress - valid address pointer

  Parameters:
    hUDP            -   the UDP socket
    addType         -   Type of address: IPv4/IPv6
    remoteAddress   -   pointer to an address to use 
    
  Returns:
    - true  - Indicates success
    - false - Indicates failure:
      - invalid socket
      - invalid socket address type
      - socket is of broadcast type

  Remarks:
    The call will fail if the socket was previously set to broadcast
    using the TCPIP_UDP_OptionsSet call.
    TCPIP_UDP_OptionsSet takes precedence.

    The call will fail if remoteAddress is 0.
    The destination IP address will not be changed.
    
  */
bool    TCPIP_UDP_DestinationIPAddressSet(UDP_SOCKET hUDP, IP_ADDRESS_TYPE addType, 
                                           IP_MULTI_ADDRESS* remoteAddress);

// *****************************************************************************

/*
  Function:
    bool TCPIP_UDP_SourceIPAddressSet(UDP_SOCKET hUDP, IP_ADDRESS_TYPE addType, 
                                      IP_MULTI_ADDRESS* localAddress)

  Summary:
    Sets the source IP address of a socket
    
  Description:
    This function sets the IP source address, which allows changing the source 
    P address dynamically.


  Precondition:
    UDP initialized
    UDP socket should have been opened with TCPIP_UDP_ServerOpen/TCPIP_UDP_ClientOpen.
    hUDP  - valid socket
    localAddress - valid address pointer

  Parameters:
    hUDP            - the UDP socket
    addType         - Type of address: IPv4/IPv6
    localAddress    - pointer to an address to use 
    
  Returns:
    - true  - Indicates success
    - false - Indicates failure:
      - invalid socket
      - invalid socket address type
      - unspecified localAddress


  Remarks:
    The call will fail if localAddress is 0.
    The source IP address will not be changed.
    
  */
bool TCPIP_UDP_SourceIPAddressSet(UDP_SOCKET hUDP, IP_ADDRESS_TYPE addType, 
                                  IP_MULTI_ADDRESS* localAddress);


// *****************************************************************************

/*
  Function:
    bool TCPIP_UDP_DestinationPortSet(UDP_SOCKET s, UDP_PORT remotePort)

  Summary:
    Sets the destination port of a socket
    
  Description:
    This function sets the destination port, which allows changing the destination 
    port dynamically.


  Precondition:
    UDP initialized
    UDP socket should have been opened with TCPIP_UDP_ServerOpen/TCPIP_UDP_ClientOpen.
    hUDP  - valid socket

  Parameters:
    hUDP        -   the UDP socket
    remotePort  -   destination port to use 
    
  Returns:
    - true  - Indicates success
    - false - Indicates an invalid socket

  Remarks:
    The destination remote port will always be changed, even if remotePort == 0.

    It will not change the UDP_OPTION_STRICT_PORT on the socket.
    
  */
bool    TCPIP_UDP_DestinationPortSet(UDP_SOCKET s, UDP_PORT remotePort);


// *****************************************************************************
/* Function:
    TCPIP_UDP_SignalHandlerRegister(UDP_SOCKET s, TCPIP_UDP_SIGNAL_TYPE sigMask, 
                       TCPIP_UDP_SIGNAL_FUNCTION handler, const void* hParam)

  Summary:
    Registers a UDP socket signal handler.

  Description:
    This function registers a UDP socket signal handler.
    The UDP module will call the registered handler when a
    UDP signal (TCPIP_UDP_SIGNAL_TYPE) occurs.

  Precondition:
    UDP valid socket.

  Parameters:
    s           - The UDP socket
    sigMask      - mask of signals to be reported
    handler     - signal handler to be called when a UDP signal occurs.
    hParam      - Parameter to be used in the handler call.
                  This is user supplied and is not used by the UDP module.
                  Currently not used and it should be null.


  Returns:
    Returns a valid handle if the call succeeds, or a null handle if
    the call failed (null handler, no such socket, existent handler).

  Remarks:

    Only one signal handler per socket is supported.
    A new handler does not override the existent one.
    Instead TCPIP_UDP_SignalHandlerDeregister has to be explicitly called.

    The handler has to be short and fast. It is meant for
    setting an event flag, not for lengthy processing!

    The hParam is passed by the client but is currently not used and should be 0.

    For multi-threaded systems the TCP/IP packet dispatch does not occur on the user thread.
    The signal handler will be called on a different thread context.
    It is essential that this handler is non blocking and really fast.
    

    For multi-threaded systems, once set, it is not recommended to change the signal handler at run time.
    Synchronization between user threads and packet dispatch threads could be difficult.
    If really need to be changed, make sure that the old handler could still be called
    and it should be valid until the new one is taken into account.
    TCPIP_UDP_SignalHandlerDeregister needs to be called before registering another handler.


 */

TCPIP_UDP_SIGNAL_HANDLE  TCPIP_UDP_SignalHandlerRegister(UDP_SOCKET s, TCPIP_UDP_SIGNAL_TYPE sigMask, 
                                 TCPIP_UDP_SIGNAL_FUNCTION handler, const void* hParam);

// *****************************************************************************
/* Function:
    TCPIP_UDP_SignalHandlerDeregister(UDP_SOCKET s, TCPIP_UDP_SIGNAL_HANDLE hSig)

  Summary:
    Deregisters a previously registered UDP socket signal handler.
    
  Description:
    Deregisters the UDP socket signal handler.

  Precondition:
    hSig valid UDP signal handle.

  Parameters:
    s       - The UDP socket
    hSig    - A handle returned by a previous call to TCPIP_UDP_SignalHandlerRegister.

  Returns:
    - true  - if the call succeeds
    - false - if no such handler is registered
 */

bool   TCPIP_UDP_SignalHandlerDeregister(UDP_SOCKET s, TCPIP_UDP_SIGNAL_HANDLE hSig);

// *****************************************************************************
/* Function:
    int TCPIP_UDP_SocketsNumberGet(void)

  Summary:
    Returns the number of existent UDP sockets.
    
  Description:
    This function returns the number of created UDP sockets.
    This is the maximum number of sockets that can be opened at any moment
    as it's been passed as parameter when UDP module was created.

  Precondition:
    UDP module properly initialized

  Parameters:
    None

  Returns:
    The number of UDP sockets
 */

int     TCPIP_UDP_SocketsNumberGet(void);

//*******************************************************************************
/*
  Function:
    TCPIP_UDP_PROCESS_HANDLE    TCPIP_UDP_PacketHandlerRegister(TCPIP_UDP_PACKET_HANDLER pktHandler, const void* handlerParam)

  Summary:
    Sets a new packet processing handler.

  Description:
    This function registers a new packet processing handler.
    The caller can use the handler to be notified of incoming packets
    and given a chance to examine/process them.

  Precondition:
    UDP properly initialized

  Parameters:
    pktHandler      - the packet handler which will be called for an incoming packet
    handlerParam    - packet handler parameter

  Returns:
    - a valid TCPIP_UDP_PROCESS_HANDLE - if the operation succeeded
    - NULL - if the operation failed

  Example:
    <code>
    TCPIP_UDP_PROCESS_HANDLE pktHandle = TCPIP_UDP_PacketHandlerRegister( myPktHandler, myParam );
    </code>

  Remarks:
    Currently only one packet handler is supported for the UDP module.
    The call will fail if a handler is already registered.
    Use TCPIP_UDP_PacketHandlerDeregister first

    Exists only if TCPIP_UDP_EXTERN_PACKET_PROCESS is true 

  */
TCPIP_UDP_PROCESS_HANDLE     TCPIP_UDP_PacketHandlerRegister(TCPIP_UDP_PACKET_HANDLER pktHandler, const void* handlerParam);


//*******************************************************************************
/*
  Function:
    bool    TCPIP_UDP_PacketHandlerDeregister(TCPIP_UDP_PROCESS_HANDLE pktHandle);

  Summary:
    Deregisters a previously registered packet handler.

  Description:
    This function removes a packet processing handler.

  Precondition:
    UDP properly initialized

  Parameters:
    pktHandle   - TCPIP packet handle obtained by a call to TCPIP_UDP_PacketHandlerRegister


  Returns:
    - true  - if the operation succeeded
    - false - if the operation failed

  Example:
    <code>
    TCPIP_UDP_PROCESS_HANDLE myHandle = TCPIP_UDP_PacketHandlerRegister(myPacketHandler, myParam );
    // process incoming packets
    // now we're done with it
    TCPIP_UDP_PacketHandlerDeregister(myHandle);
    </code>

  Remarks:
    Exists only if TCPIP_UDP_EXTERN_PACKET_PROCESS is true 

  */
bool    TCPIP_UDP_PacketHandlerDeregister(TCPIP_UDP_PROCESS_HANDLE pktHandle);



// *****************************************************************************
/*
  Function:
    void  TCPIP_UDP_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs UDP module tasks in the TCP/IP stack.

  Precondition:
    The UDP module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_UDP_Task(void);


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // __UDP_H_
