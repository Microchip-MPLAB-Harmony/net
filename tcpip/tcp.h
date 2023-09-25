/*******************************************************************************
 Transmission Control Protocol (TCP) Communications Layer API Header File

  Company:
    Microchip Technology Inc.

  File Name:
    tcp.h

  Summary:
    Transmission Control Protocol (TCP) Communications Layer API

  Description:
    - Provides reliable, handshaked transport of application stream
      oriented data with flow control
    - Reference: RFC 793
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

#ifndef __TCP_H__
#define __TCP_H__


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Type:
    TCP_PORT

  Summary:
    Defines a TCP Port number.

  Description:
    TCP Port Number identifier.
*/
typedef uint16_t TCP_PORT;

// *****************************************************************************
/*
  Type:
    TCP_SOCKET

  Summary:
    Defines a TCP Socket.

  Description:
    A TCP_SOCKET is stored as a single int16_t number.
*/
typedef int16_t TCP_SOCKET;

// *****************************************************************************
/*
  Macro:
    INVALID_SOCKET

  Summary:
    Invalid socket indicator macro.

  Description:
    Indicates that the socket is invalid or could not be opened.
*/
#define INVALID_SOCKET      (-1)


// *****************************************************************************
/*
  Enumeration:
    TCPIP_TCP_STATE

  Summary:
    TCP socket state.

  Description:
    Enumeration describing the possible states of a TCP socket.
*/

typedef enum
{
    TCPIP_TCP_STATE_LISTEN,                 // Socket is listening for connections
    TCPIP_TCP_STATE_SYN_SENT,               // A SYN has been sent, awaiting an SYN+ACK
    TCPIP_TCP_STATE_SYN_RECEIVED,           // A SYN has been received, awaiting an ACK
    TCPIP_TCP_STATE_ESTABLISHED,            // Socket is connected and connection is established
    TCPIP_TCP_STATE_FIN_WAIT_1,             // FIN WAIT state 1
    TCPIP_TCP_STATE_FIN_WAIT_2,             // FIN WAIT state 2
    TCPIP_TCP_STATE_CLOSING,                // Socket is closing
    TCPIP_TCP_STATE_TIME_WAIT,              // 2MSL state
    TCPIP_TCP_STATE_CLOSE_WAIT,             // Waiting to close the socket
    TCPIP_TCP_STATE_LAST_ACK,               // The final ACK has been sent
    TCPIP_TCP_STATE_CLIENT_WAIT_DISCONNECT, // client socket lost connection, waiting for app close/disconnect
    TCPIP_TCP_STATE_CLIENT_WAIT_CONNECT,    // client socket waiting for connection
    TCPIP_TCP_STATE_KILLED,                 // socket is killed; debug reporting state
} TCPIP_TCP_STATE;


// *****************************************************************************
/*
  Enumeration:
    TCP_SOCKET_FLAGS

  Summary:
    TCP socket flags.

  Description:
    Enumeration describing the possible flags of a TCP socket.
*/

typedef enum
{
    TCP_SOCKET_FLAG_NONE        = 0,        // No flag set, invalid socket
    TCP_SOCKET_FLAG_VALID       = 0x01,     // socket exists: validity flag
    TCP_SOCKET_FLAG_CONNECTED   = 0x02,     // socket is currently connected
    TCP_SOCKET_FLAG_RST         = 0x04,     // remote party issued a reset
    TCP_SOCKET_FLAG_FIN         = 0x08,     // remote party issued a FIN
} TCP_SOCKET_FLAGS;




// *****************************************************************************
/*
  Structure:
    TCP_SOCKET_INFO

  Summary:
    TCP socket information.

  Description:
    Gets information about a socket.
*/
typedef struct
{
    IP_ADDRESS_TYPE     addressType;        // Address type of the socket IPv4 or IPv6
    IP_MULTI_ADDRESS    remoteIPaddress;    // Remote address to which the socket is connected
    IP_MULTI_ADDRESS    localIPaddress;     // Local address socket is bound to
    TCP_PORT            remotePort;         // Port number associated with remote node
    TCP_PORT            localPort;          // Local port number
    TCPIP_NET_HANDLE    hNet;               // Associated interface
    TCPIP_TCP_STATE     state;              // Current socket state
    uint16_t            rxSize;             // size of the RX buffer
    uint16_t            txSize;             // size of the TX buffer
    uint16_t            rxPending;          // bytes pending in RX buffer
    uint16_t            txPending;          // bytes pending in TX buffer
    TCP_SOCKET_FLAGS    flags;              // socket flags
} TCP_SOCKET_INFO;

// *****************************************************************************
/*
  Enumeration:
    TCP_ADJUST_FLAGS

  Summary:
    TCP adjust RX and TX buffers flags.

  Description:
    Adjusts socket RX and TX buffer sizes.
*/
typedef enum
{
    TCP_ADJUST_GIVE_REST_TO_RX  = 0x01,     // Resize flag: extra bytes go to RX
    TCP_ADJUST_GIVE_REST_TO_TX  = 0x02,     // Resize flag: extra bytes go to TX
    TCP_ADJUST_PRESERVE_RX      = 0x04,     // Resize flag: attempt to preserve RX buffer
    TCP_ADJUST_PRESERVE_TX      = 0x08,     // Resize flag: attempt to preserve TX buffer
    TCP_ADJUST_TX_ONLY          = 0x10,     // Resize flag: adjust the TX buffer only
    TCP_ADJUST_RX_ONLY          = 0x20,     // Resize flag: adjust the RX buffer only

}TCP_ADJUST_FLAGS;

// *****************************************************************************
/*
  Enumeration:
    TCP_SOCKET_OPTION

  Summary:
    TCP Socket run-time options.

  Description:
    This enumeration defines TCP socket run-time options.
*/
typedef enum
{
    TCP_OPTION_LINGER,              // The LINGER option controls the action taken when unsent data is queued on a socket and the socket is closed.
                                    // The linger option can be turned on/off and the timeout can be specified.
    TCP_OPTION_KEEP_ALIVE,          // Enable the use of keep-alive packets on TCP connections.
                                    // The option can be turned on/off and the timeout can be specified.
    TCP_OPTION_RX_BUFF,             // Request different RX buffer size. Has to call TCPIP_TCP_OptionsGet to see the exact space
                                    // allocated.
    TCP_OPTION_TX_BUFF,             // Request different TX buffer size. Has to call TCPIP_TCP_OptionsGet to see the exact space
                                    // allocated.
    TCP_OPTION_RX_TMO,              // Specifies the RX timeout. If no data arrives in the specified timeout the socket is closed.
    TCP_OPTION_TX_TMO,              // Specifies the TX timeout. If no data can be sent in the specified timeout the socket is closed.
    TCP_OPTION_NODELAY,             // Enables the NO DELAY/Nagle algorithm functionality.
                                    // The default setting is disabled.
    TCP_OPTION_EXCLUSIVE_ADDRESS,   // Enables a socket to be bound for exclusive access.
                                    // The default setting is disabled - option not supported yet.
    TCP_OPTION_THRES_FLUSH,         // Sets the type of half buffer TX flush for the socket.
                                    // The default setting is TCP_OPTION_THRES_FLUSH_AUTO
    TCP_OPTION_DELAY_SEND_ALL_ACK,  // Enables/disables the delay of sending the pending data when no unacknowledged data.
                                    // When this option is disabled, the socket will immediately send any pending data chunk
                                    // whenever all the previous data is acknowledged by the remote party (there is no unacknowledged data).
                                    // This holds true for the 1st transmission too, or after a pause of some length.
                                    // If the option is enabled, the socket will delay sending the data, waiting for more data to accumulate.
                                    // This could be useful when the application makes multiple calls with small data chunks.
                                    // The default setting is disabled.
    TCP_OPTION_TX_TTL,              // Specifies the Time To Live for packets sent by the socket.
                                    // If 0, the socket will use the default global IPv4 TTL setting.
                                    // This option allows the user to specify a different TTL value.
    TCP_OPTION_TOS,                 // Sets the Type of Service (TOS) for IPv4 packets sent by the socket
} TCP_SOCKET_OPTION;


// *****************************************************************************
/*
  Structure:
    TCP_OPTION_LINGER_DATA

  Summary:
    Socket linger options.

  Description:
    This structure defines socket linger options.
*/
typedef struct
{
    bool        lingerEnable;       // Enable/disable linger; enabled by default for any socket.
    bool        gracefulEnable;     // Enable/disable graceful close; enabled by default for any socket.
    uint16_t    lingerTmo;          // Linger timeout in seconds (when enabled).
                                    // This option is not supported yet.
}TCP_OPTION_LINGER_DATA;


// *****************************************************************************
/*
  Structure:
    TCP_OPTION_KEEP_ALIVE_DATA

  Summary:
    Socket keep alive options

  Description:
    This structure defines socket keep alive options.
*/
typedef struct
{
    bool        keepAliveEnable;        // Enable/disable keep alive option; disabled by default for any socket.
    uint16_t    keepAliveTmo;           // keep alive timeout in milliseconds 
                                        // ignored when keep alive is disabled
                                        // if 0, the default build time value is used
    uint8_t     keepAliveUnackLim;      // limit of keep alives to be sent
                                        // the socket will reset the communication channel if no reply received after so many retries
                                        // ignored when keep alive is disabled
                                        // if 0, the default build time value is used
}TCP_OPTION_KEEP_ALIVE_DATA;

// *****************************************************************************
/*
  Enumeration:
    TCP_OPTION_THRES_FLUSH_TYPE

  Summary:
    List of the socket half threshold TX flush types.

  Description:
    Describes the possible types of the socket half threshold TX flush.
     
*/
typedef enum
{
    TCP_OPTION_THRES_FLUSH_AUTO,    // The socket will set the half buffer flush based on the TX buffer size.
                                    // This is the default setting.
                                    // For TX buffers >= 1.5 MSS the half buffer flush will be disabled.
                                    // This results in higher performance/throughput for the socket.
    TCP_OPTION_THRES_FLUSH_OFF,     // Always disable the socket flush at half buffer threshold.
                                    // No socket flush is performed when the half TX buffer threshold is passed.
    TCP_OPTION_THRES_FLUSH_ON,      // Always enable the socket flush at half buffer threshold.
                                    // Socket flush is always performed when the half TX buffer threshold is passed.
                                    // This is useful for small TX buffers when the remote party implements the delayed ACK algorithm.
}TCP_OPTION_THRES_FLUSH_TYPE;

// *****************************************************************************
/*
  Enumeration:
    TCPIP_TCP_SIGNAL_TYPE

  Summary:
    TCP run time signal/event types.

  Description:
    Description of the signals/events that a TCP socket can generate.


  Remarks:
    These signals are used in the socket event handling notification functions.
    It is possible that multiple flags are set as part of the same notification.

    The signals are 16 bits wide.
*/
typedef enum
{
    // TX related signals
    TCPIP_TCP_SIGNAL_TX_DONE         = 0x0001,  // A TCP packet was successfully 
                                                // transmitted on the interface.
                                                // This may indicate or not that new 
                                                // data can be sent with this socket.
        // Note 1: The TCP buffer space is controlled by the TCP state machine;
        //         The fact that a packet was transmitted, doesn't necessarily mean
        //         that new data can be written into the socket buffer: 
        //      2: The packet may be an ACK only packet, retry packet, etc., and does
        //         not free any writing space in the socket buffer
        //      3: A new socket writing space may not be available because the other 
        //         party has not acknowledged the data it was sent so far. Therefore, 
        //         the information carried by this signal has limited usage.
        //      4: This notification is the result of an user action (explicit call to 
        //         TCPIP_TCP_Flush) or as a result of the internal TCP state machine

    TCPIP_TCP_SIGNAL_TX_DATA_DONE    = 0x0002,   // A TCP data packet carrying valid payload 
                                                 // was successfully transmitted on the interface.
                                                // This may indicate or not that new data can be 
                                                // sent with this socket.
                                                // Note: The previous notes for TCPIP_TCP_SIGNAL_TX_DONE 
                                                // still apply.
                                               
    TCPIP_TCP_SIGNAL_TX_SPACE        = 0x0004,  // New TX space is available in the socket TX buffer.
                                                // This event signals that the remote party has acknowledged 
                                                // some data and, as a result, TX buffer space is available.

    TCPIP_TCP_SIGNAL_TX_RST          = 0x0008,  // Reset sent to the remote host and connection is reset

    // RX related signals
    TCPIP_TCP_SIGNAL_ESTABLISHED     = 0x0100,  // Socket established a connection (client or server socket)

    TCPIP_TCP_SIGNAL_RX_DATA         = 0x0200,  // A data packet was successfully received and there is data 
                                                // available for this socket

    TCPIP_TCP_SIGNAL_RX_FIN          = 0x0400,  // Remote host finished its data and sent a FIN; 

    TCPIP_TCP_SIGNAL_RX_RST          = 0x0800,  // Remote host reset the connection; 

    TCPIP_TCP_SIGNAL_KEEP_ALIVE_TMO  = 0x1000,  // Keep alive has timed out;  Connection to the remote 
                                                // host has been aborted;

    // interface related signals
    TCPIP_TCP_SIGNAL_IF_DOWN         = 0x2000,  // associated interface is going down;
                                                // sockets connected on this interface will be disconnected, but still alive
    TCPIP_TCP_SIGNAL_IF_CHANGE       = 0x4000,  // associated interface has changed address
                                                // sockets connected on this interface will be disconnected, but still alive

}TCPIP_TCP_SIGNAL_TYPE;

// *****************************************************************************
/*
  Type:
    TCPIP_TCP_SIGNAL_FUNCTION

  Summary:
    TCP signal handler.

  Description:
    Prototype of a TCP signal handler. Socket user can register a handler for the
    TCP socket. Once an TCP event occurs the registered handler will be called.

  Parameters:
    hTCP        - TCP socket to be used
    hNet        - the network interface on which the event has occurred
    sigType     - type of TCP signal that has occurred
    param       - additional parameter that has been specified at the handler 
                  registration call
                  This is user supplied and is not used by the TCP module.

  Remarks:
    The handler has to be short and fast. It is meant for
    setting an event flag, not for lengthy processing!

 */

typedef void    (*TCPIP_TCP_SIGNAL_FUNCTION)(TCP_SOCKET hTCP, TCPIP_NET_HANDLE hNet, 
                  TCPIP_TCP_SIGNAL_TYPE sigType, const void* param);


// *****************************************************************************
/*
  Type:
    TCPIP_TCP_SIGNAL_HANDLE

  Summary:
    TCP socket handle.

  Description:
    A handle that a socket client can use after the signal handler has been registered.
 */

typedef const void* TCPIP_TCP_SIGNAL_HANDLE;

// *****************************************************************************
/*
  Type:
    TCPIP_TCP_PROCESS_HANDLE

  Summary:
    Defines a TCP packet processing handle.

  Description:
    Definition of an packet processing handle used for
    packet processing registration by the TCP clients.

*/
typedef const void* TCPIP_TCP_PROCESS_HANDLE;

// *****************************************************************************
/* TCP packet handler Pointer

  Function:
    bool <FunctionName> (TCPIP_NET_HANDLE hNet, struct _tag_TCPIP_MAC_PACKET* rxPkt, const void* hParam);

  Summary:
    Pointer to a function(handler) that will get called to process an incoming TCP packet.

  Description:
    Pointer to a function that will be called by the TCP module
    when a RX packet is available.

  Precondition:
    None

  Parameters:
    hNet        - network handle on which the packet has arrived
    rxPkt       - pointer to incoming packet
    hParam      - user passed parameter when handler was registered

  Returns:
    true - if the packet is processed by the external handler.
           In this case the TCP module will no longer process the packet
    false - the packet needs to be processed internally by the TCP as usual           

  Remarks:
    The packet handler is called in the TCP context.
    The handler should be kept as short as possible as it affects the processing of all the other
    TCP RX traffic.

    Before calling the external packet handler:
    - the rxPkt->pktFlags has the bit 9 (value 0x0200) set for an IPv6 packet, cleared for IPv4
    - the rxPkt->pTransportLayer points to an TCP_HEADER data structure.
    - the rxPkt->pNetLayer points to an IPV4_HEADER/IPV6_HEADER data structure.
    - the rxPkt->pktIf points to the interface receiving the packet
    - the first data segment segLen is adjusted to store the size of the TCP data 

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
typedef bool(*TCPIP_TCP_PACKET_HANDLER)(TCPIP_NET_HANDLE hNet, struct _tag_TCPIP_MAC_PACKET* rxPkt, const void* hParam);


// *****************************************************************************
/*
  Structure:
    TCPIP_TCP_MODULE_CONFIG

  Summary:
    TCP module run-time configuration/initialization data.

  Description:
    This structure defines TCP module run-time configuration/initialization data.
*/
//
typedef struct
{
    int             nSockets;       // Number of sockets to be created
    uint16_t        sktTxBuffSize;  // Size of the socket TX buffer
    uint16_t        sktRxBuffSize;  // Size of the socket RX buffer

} TCPIP_TCP_MODULE_CONFIG;


// *****************************************************************************
// *****************************************************************************
// Section: General Functions
// *****************************************************************************
// *****************************************************************************

//*****************************************************************************
/*
  Function:
     TCP_SOCKET TCPIP_TCP_ServerOpen(IP_ADDRESS_TYPE addType, TCP_PORT localPort,  
                                     IP_MULTI_ADDRESS* localAddress)

  Summary:
    Opens a TCP socket as a server.

  Description:
    Provides a unified method for opening TCP server sockets.

    Sockets are created at the TCP module initialization, and can be claimed with this
    function and freed using TCPIP_TCP_Close.

  Precondition:
    TCP is initialized.

  Parameters:
    IP_ADDRESS_TYPE addType         -   The type of address being used. Example: IP_ADDRESS_TYPE_IPV4.
    TCP_PORT localPort              -   TCP port to listen on for connections.
    IP_MULTI_ADDRESS* localAddress  -   Local address to use.
                                        Use 0 if listening on any network interface.

  Returns:
    - INVALID_SOCKET - No sockets of the specified type were available to be opened
    - TCP_SOCKET handle - Save this handle and use it when calling all other TCP APIs

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
TCP_SOCKET  TCPIP_TCP_ServerOpen(IP_ADDRESS_TYPE addType, TCP_PORT localPort,  
                                 IP_MULTI_ADDRESS* localAddress);

//*****************************************************************************
/*
  Function:
     TCP_SOCKET TCPIP_TCP_ClientOpen(IP_ADDRESS_TYPE addType, TCP_PORT remotePort, 
                                     IP_MULTI_ADDRESS* remoteAddress)

  Summary:
    Opens a TCP socket as a client.

  Description:
    Provides a unified method for opening TCP client sockets.

    Sockets are created at the TCP module initialization, and can be claimed with this
    function and freed using TCPIP_TCP_Abort or TCPIP_TCP_Close.

  Precondition:
    TCP is initialized.

  Parameters:
    IP_ADDRESS_TYPE addType         -   The type of address being used. Example: IP_ADDRESS_TYPE_IPV4.
    TCP_PORT remotePort             -   TCP port to connect to.
                                        The local port for client sockets will be automatically picked
                                        by the TCP module.
    IP_MULTI_ADDRESS* remoteAddress -   The remote address to be used.
                                        If 0 then the address is unspecified

  Returns:
    - INVALID_SOCKET - No sockets of the specified type were available to be opened
    - TCP_SOCKET handle - Save this handle and use it when calling all other TCP APIs

  Remarks:
    IP_ADDRESS_TYPE_ANY is not supported (not a valid type for client open)!

    If the remoteAddress != 0 (and the address pointed by remoteAddress != 0)
    then the socket will immediately initiate a connection to the remote host

    If the remoteAddress is unspecified, no connection is initiated.
    Client socket parameters can be set using TCPIP_TCP_Bind, TCPIP_TCP_RemoteBind, etc.
    calls and then connection initiated by calling TCPIP_TCP_Connect.
 
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
TCP_SOCKET  TCPIP_TCP_ClientOpen(IP_ADDRESS_TYPE addType, TCP_PORT remotePort, 
                                 IP_MULTI_ADDRESS* remoteAddress);

//******************************************************************************
/*
  Function:
    bool  TCPIP_TCP_Bind(TCP_SOCKET hTCP, IP_ADDRESS_TYPE addType, TCP_PORT localPort,  
                         IP_MULTI_ADDRESS* localAddress)

  Summary:
    Binds a socket to a local address.

  Description:
    This function is meant for unconnected server and client sockets.
    It is similar to TCPIP_TCP_SocketNetSet that assigns a specific source interface for a socket.
    If localPort is 0 the stack will assign a unique local port. Sockets don't need specific binding, 
    it is done automatically by the stack.
    However, specific binding can be requested using these functions.
    Works for both client and server sockets.

  Precondition:
    TCP socket should have been opened with TCPIP_TCP_ServerOpen()/TCPIP_TCP_ClientOpen().
    hTCP - valid socket

  Parameters:
    hTCP            -   Socket to bind
    addType         -   The type of address being used. Example: IP_ADDRESS_TYPE_IPV4/IP_ADDRESS_TYPE_IPV6.
    localPort       -   Local port to use
                        If 0,  the stack will assign a unique local port 
    localAddress    -   The local address to bind to.
                        Could be NULL if the local address does not need to be changed

  Returns:
    - true  - Indicates success
    - false - Indicates failure


  Remarks:
    The call will fail if the socket is already connected (both server and client sockets).

 */
bool  TCPIP_TCP_Bind(TCP_SOCKET hTCP, IP_ADDRESS_TYPE addType, TCP_PORT localPort, 
                     IP_MULTI_ADDRESS* localAddress);

//******************************************************************************
/*
  Function:
    bool  TCPIP_TCP_RemoteBind(TCP_SOCKET hTCP, IP_ADDRESS_TYPE addType, TCP_PORT remotePort, 
                               IP_MULTI_ADDRESS* remoteAddress)

  Summary:
    Binds a socket to a remote address.

  Description:
    This function is meant for unconnected server and client sockets.
    Sockets don't need specific remote binding, they should accept connections on any incoming interface.
    Thus the binding is done automatically by the stack.
    However, specific remote binding can be requested using these functions.
    For a server socket it can be used to restrict accepting connections from  a specific remote host.
    For a client socket it will just change the default binding done when the socket was opened.

  Precondition:
    TCP socket should have been opened with TCPIP_TCP_ServerOpen()/TCPIP_TCP_ClientOpen().
    hTCP - valid socket

  Parameters:
    hTCP            -   Socket to bind
    addType         -   The type of address being used. Example: IP_ADDRESS_TYPE_IPV4/IP_ADDRESS_TYPE_IPV6.
    remotePort      -   remote port to use
                        Could be 0 if the remote port does not need to be changed
    remoteAddress   -   The remote address to bind to.
                        Could be NULL if the remote address does not need to be changed

  Returns:
    - true  - Indicates success
    - false - Indicates failure


  Remarks:
    The socket remote port is changed only if a non-zero remotePort value is passed.

    The socket remote host address is changed only if a non-zero remoteAddress value is passed.

    The call will fail if the socket is already connected (both server and client sockets).

  */
bool  TCPIP_TCP_RemoteBind(TCP_SOCKET hTCP, IP_ADDRESS_TYPE addType, TCP_PORT remotePort, 
                           IP_MULTI_ADDRESS* remoteAddress);

//******************************************************************************
/*
  Function:
    bool  TCPIP_TCP_OptionsSet(TCP_SOCKET hTCP, TCP_SOCKET_OPTION option, void* optParam)

  Summary:
    Allows setting options to a socket like adjust RX/TX buffer size, etc.

  Description:
    Various options can be set at the socket level.
    This function provides compatibility with BSD implementations.

  Precondition:
    TCP socket should have been opened with TCPIP_TCP_ServerOpen()/TCPIP_TCP_ClientOpen().
    hTCP - valid socket


  Parameters:
    hTCP            - socket to set options for
    option          - specific option to be set
    optParam        - the option value; this is option dependent
                      - TCP_OPTION_LINGER               - pointer to a TCP_OPTION_LINGER_DATA structure
                      - TCP_OPTION_KEEP_ALIVE           - pointer to a TCP_OPTION_KEEP_ALIVE_DATA structure
                      - TCP_OPTION_RX_BUFF              - size of the new RX buffer (<= 65535 bytes)
                      - TCP_OPTION_TX_BUFF              - size of the new TX buffer (<= 65535 bytes)
                      - TCP_OPTION_RX_TMO               - not supported yet
                      - TCP_OPTION_TX_TMO               - not supported yet
                      - TCP_OPTION_NODELAY              - boolean to enable/disable the NO DELAY functionality
                      - TCP_OPTION_EXCLUSIVE_ADDRESS    - not supported yet
                      - TCP_OPTION_THRES_FLUSH          - a TCP_OPTION_THRES_FLUSH_TYPE
                      - TCP_OPTION_DELAY_SEND_ALL_ACK   - boolean to enable/disable the DELAY Send All ACK data functionality
                      - TCP_OPTION_TX_TTL              - 8-bit value of TTL
                      - TCP_OPTION_TOS                 - 8-bit value of the TOS

  Returns:
    - true  - Indicates success
    - false - Indicates failure
  */
bool  TCPIP_TCP_OptionsSet(TCP_SOCKET hTCP, TCP_SOCKET_OPTION option, void* optParam);


//******************************************************************************
/*
  Function:
    bool  TCPIP_TCP_OptionsGet(TCP_SOCKET hTCP, TCP_SOCKET_OPTION option, void* optParam)

  Summary:
    Allows getting the options for a socket like: current RX/TX buffer size, etc.

  Description:
    Various options can be get at the socket level.
    This function provides compatibility with BSD implementations.

  Precondition:
    TCP socket should have been opened with TCPIP_TCP_ServerOpen()/TCPIP_TCP_ClientOpen().
    hTCP - valid socket


  Parameters:
    hTCP            - socket to get options for
    option          - specific option to get
    optParam        - pointer to an area that will receive the option value; this is option dependent
                      the size of the area has to be large enough to accommodate the specific option
                      - TCP_OPTION_LINGER               - pointer to a TCP_OPTION_LINGER_DATA structure
                      - TCP_OPTION_KEEP_ALIVE           - pointer to a TCP_OPTION_KEEP_ALIVE_DATA structure
                      - TCP_OPTION_RX_BUFF              - size of the new RX buffer
                      - TCP_OPTION_TX_BUFF              - size of the new TX buffer
                      - TCP_OPTION_RX_TMO               - not supported yet
                      - TCP_OPTION_TX_TMO               - not supported yet
                      - TCP_OPTION_NODELAY              - pointer to boolean to return current NO DELAY status
                      - TCP_OPTION_EXCLUSIVE_ADDRESS    - not supported yet
                      - TCP_OPTION_THRES_FLUSH          - a TCP_OPTION_THRES_FLUSH_TYPE
                      - TCP_OPTION_DELAY_SEND_ALL_ACK   - pointer to boolean to return current DELAY Send All ACK status
                      - TCP_OPTION_TX_TTL               - pointer to an 8 bit value to receive the TTL value
                      - TCP_OPTION_TOS                  - pointer to an 8 bit value to receive the TOS

  Returns:
    - true  - Indicates success
    - false - Indicates failure
  */
bool  TCPIP_TCP_OptionsGet(TCP_SOCKET hTCP, TCP_SOCKET_OPTION option, void* optParam);

//******************************************************************************
/*
  Function:
    bool  TCPIP_TCP_IsConnected(TCP_SOCKET hTCP)

  Summary:
  Determines if a socket has an established connection.

  Description:
    This function determines if a socket has an established connection to
    a remote node.  Call this function after calling TCPIP_TCP_ServerOpen()/TCPIP_TCP_ClientOpen()
    to determine when the connection is set up and ready for use.

  Precondition:
    TCP socket should have been opened with TCPIP_TCP_ServerOpen()/TCPIP_TCP_ClientOpen().
    hTCP - valid socket

  Parameters:
    hTCP - The socket to check.

  Return Values:
    - true  - The socket has an established connection to a remote node.
    - false - The socket is not currently connected.

  Remarks:
    A socket is said to be connected only if it is in one of the states:
    TCPIP_TCP_STATE_ESTABLISHED, TCPIP_TCP_STATE_FIN_WAIT_1, TCPIP_TCP_STATE_FIN_WAIT_2 or TCPIP_TCP_STATE_CLOSE_WAIT
    In all of these states the socket can exchange data with the other end of the connection (either full duplex or
    only TX/RX).

    Otherwise the call will return false.
  */
bool  TCPIP_TCP_IsConnected(TCP_SOCKET hTCP);

//*****************************************************************************
/*
  Function:
    bool TCPIP_TCP_WasReset(TCP_SOCKET hTCP)

  Summary:
    Self-clearing semaphore indicating socket reset.

  Description:
    This function is a self-clearing semaphore indicating whether or not
    a socket has been reset since the previous call.
    This function indicates that a RST was received from the remote node,
    a TCPIP_TCP_Disconnect was called or the socket was somehow re-initalized.

    It also returns true just after the socket was opened.

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket to check.

  Return Values:
    - true - The socket has been reset since the previous call.
    - false - The socket has not been reset since the previous call.

  Remarks:
    A server socket upon receiving a RST signal will go to listen state.

    A client socket will be closed when RST is received.

    The info that the FIN was received could be also obtained
    by registering a signal with the socket (the preferred way).
 */
bool   TCPIP_TCP_WasReset(TCP_SOCKET hTCP);

//*****************************************************************************
/*
  Function:
    bool TCPIP_TCP_WasDisconnected(TCP_SOCKET hTCP)

  Summary:
    Function indicating that the socket was disconnected from the remode node.

  Description:
    This function indicates that the socket has received a FIN from the remote node.
    This signals that the remote node has finished transmitting its data.
    By default, the socket will be in the CLOSE_WAIT state.

    At this point it is up to the socket owner to send some more data or to close the socket.
    
    When done transmitting its data, the socket owner should call TCPIP_TCP_Disconnect() or TCPIP_TCP_Close().
    TCPIP_TCP_Disconnect works for both server and client sockets.
    The server socket will return to listen state.
    The client socket will be closed.

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket to check.

  Return Values:
    - true - The socket has received FIN
    - false - The socket has not received FIN
              or the socket is invalid

  Remarks:
    The obsolete symbol TCPIP_TCP_CLOSE_WAIT_TIMEOUT != 0
    causes the socket to transition itself from CLOSE_WAIT state:
    - to listen state for a server socket
    - to close a client socket
    However this behavior is not according to the standards and could be dangerous
    especially in multi-threaded environments.

    The info that the FIN was received could be also obtained
    by registering a signal with the socket (the preferred way).

 */
bool   TCPIP_TCP_WasDisconnected(TCP_SOCKET hTCP);

//******************************************************************************
/*
  Function:
    bool  TCPIP_TCP_Disconnect(TCP_SOCKET hTCP)

  Summary:
    Disconnects an open socket.

  Description:
    This function closes the TX side of a connection by sending a FIN (if 
    currently connected) to the remote node of the connection.
    
    If the socket has the linger option set (default), the queued TX data transmission will be attempted
    before sending the FIN.
    If the linger option is off, the queued TX data will be discarded.

    Please note that this call may fail in which case it can be re-issued.

  Precondition:
    TCP socket should have been opened with TCPIP_TCP_ServerOpen()/TCPIP_TCP_ClientOpen().
    hTCP - valid socket

  Parameters:
    hTCP - Handle of the socket to disconnect.

  Returns:
    - true  - If the call succeeded
    - false - Indicates that the notification could not be sent to the remote host.
              The call can be reissued at a later time if desired.

  Remarks:
    None.
    
  */
bool  TCPIP_TCP_Disconnect(TCP_SOCKET hTCP);

//******************************************************************************
/*
  Function:
    void  TCPIP_TCP_Abort(TCP_SOCKET hTCP, bool killSocket)

  Summary:
    Aborts a connection.

  Description:
    This function aborts a connection to a remote node by sending a RST (if 
    currently connected).
    Any pending TX/RX data is discarded.

    A client socket will always be closed and the associated resources released.
    The socket cannot be used again after this call.

    A server socket will abort the current connection:
    - if killSocket == false the socket will remain listening
    - if killSocket == true the socket will be closed and all associated resources released.
      The socket cannot be used again after this call.

  Precondition:
    TCP socket should have been opened with TCPIP_TCP_ServerOpen()/TCPIP_TCP_ClientOpen().
    hTCP - valid socket

  Parameters:
    hTCP - Handle to the socket to disconnect.
    killSocket  - if true, it kills a server socket

  Returns:
    None.
  */
void  TCPIP_TCP_Abort(TCP_SOCKET hTCP, bool killSocket);

//******************************************************************************
/*
  Function:
    bool  TCPIP_TCP_Connect(TCP_SOCKET hTCP)

  Summary:
    Connects a client socket.

  Description:
    This function will try to initiate a connection on a client socket that is not 
    connected yet.
    The client socket should have been created with  a call to TCPIP_TCP_ClientOpen 
    having the remoteAddress set to 0.


  Precondition:
    TCP socket should have been opened with TCPIP_TCP_ClientOpen with an unspecified address.
    hTCP - valid socket

  Parameters:
    hTCP - Handle to the client socket to connect.

  Returns:
    - true  - If the call succeeded
    - false - If the call failed

  Remarks:
    The call will fail if the client socket has no remote host specified.
    Use TCPIP_TCP_RemoteBind() to specify a remote host address for the client socket.

  */
bool  TCPIP_TCP_Connect(TCP_SOCKET hTCP);

//******************************************************************************
/*
  Function:
    bool  TCPIP_TCP_Close(TCP_SOCKET hTCP)

  Summary:
    Disconnects an open socket and destroys the socket handle, releasing the associated 
    resources.

  Description:
    Graceful Option Set:
    If the graceful option is set for the socket (default), a TCPIP_TCP_Disconnect will 
    be tried. If the linger option is set (default) the TCPIP_TCP_Disconnect will try to send
    any queued TX data before issuing FIN. If the FIN send operation fails or the socket is 
    not connected the abort is generated.

    Graceful Option Not Set:
    If the graceful option is not set, or the previous step could not send the FIN,a 
    TCPIP_TCP_Abort is called, sending a RST to the remote node. Communication is closed,
    the socket is no longer valid and the associated resources are freed.

  Precondition:
    TCP socket should have been opened with TCPIP_TCP_ServerOpen/TCPIP_TCP_ClientOpen.
    hTCP - valid socket

  Parameters:
    hTCP - Handle to the socket to disconnect and close.

  Returns:
    - true  - If the call succeeded
    - false - If the call failed (no such socket)

  */
bool  TCPIP_TCP_Close(TCP_SOCKET hTCP);

//*****************************************************************************
/*
  Function:
    bool TCPIP_TCP_SocketInfoGet(TCP_SOCKET hTCP, TCP_SOCKET_INFO* remoteInfo)

  Summary:
    Obtains information about a currently open socket.

  Description:
    Fills the provided TCP_SOCKET_INFO structure associated with this socket.
    This contains the IP addresses and port numbers for both the local and remote endpoints.

  Precondition:
    TCP is initialized and the socket is connected.

  Parameters:
    hTCP - The socket to check.

  Returns:
    - true  - if the call succeeded
    - false - if no such socket exists or the socket is not open
 */
bool  TCPIP_TCP_SocketInfoGet(TCP_SOCKET hTCP, TCP_SOCKET_INFO* pInfo);

//*****************************************************************************
/*
  Function:
    TCP_SOCKET_FLAGS  TCPIP_TCP_SocketFlagsGet(TCP_SOCKET hTCP);

  Summary:
    Obtains the current socket flags

  Description:
    The function will return flags associated with the current socket

  Precondition:
    TCP is initialized

  Parameters:
    hTCP - The socket handle

  Returns:
    a TCP_SOCKET_FLAGS value

  Remarks:
    TCP_SOCKET_FLAG_VALID will not be set for a non existent socket 
 */
TCP_SOCKET_FLAGS  TCPIP_TCP_SocketFlagsGet(TCP_SOCKET hTCP);

//*****************************************************************************
/*
  Function:
    uint16_t TCPIP_TCP_PutIsReady(TCP_SOCKET hTCP)

  Summary:
    Determines how much free space is available in the TCP TX buffer.

  Description:
    Call this function to determine how many bytes can be written to the
    TCP TX buffer.  If this function returns zero, the application must
    return to the main stack loop before continuing in order to transmit
    more data.

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket to check.

  Returns:
    The number of bytes available to be written in the TCP TX buffer.
 */
uint16_t  TCPIP_TCP_PutIsReady(TCP_SOCKET hTCP);

//*****************************************************************************
/*
  Function:
    uint16_t TCPIP_TCP_ArrayPut(TCP_SOCKET hTCP, const uint8_t* data, uint16_t len)

  Description:
    Writes an array from a buffer to a TCP socket.

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket to which data is to be written.
    data - Pointer to the array to be written.
    len  - Number of bytes to be written.

  Returns:
    The number of bytes written to the socket.  If less than len, the
    buffer became full or the socket is not connected.
    
  Remarks:
    This operation can cause a TCP packet to be transmitted over the
    network (i.e., a TCPIP_TCP_Flush operation to be performed) when there is
    data in the TCP TX buffer that can be sent and any of the following occurs:
    - There is no more space available in the TCP buffer.
    - The TCP_OPTION_THRES_FLUSH_ON is set and the TX buffer is at least half full
    - The amount of data that can be sent is bigger than the remote host MSS
      or than half of the maximum advertised window size.
    - The Nagle algorithm is disabled and there is no unacknowledged data. 

    If none of these occur and the socket user does not add data to the TX socket buffer,
    the TCP state machine will automatically flush the buffer when the TCP_AUTO_TRANSMIT_TIMEOUT_VAL
    time-out elapsed. 

    The default TCP_AUTO_TRANSMIT_TIMEOUT_VAL is 40 ms.

 */
uint16_t  TCPIP_TCP_ArrayPut(TCP_SOCKET hTCP, const uint8_t* Data, uint16_t Len);

//*****************************************************************************
/*
  Function:
    const uint8_t* TCPIP_TCP_StringPut(TCP_SOCKET hTCP, const uint8_t* data)

  Summary:
    Writes a null-terminated string to a TCP socket.
  
  Description:
    This function writes a null-terminated string to a TCP socket.
    The null-terminator is not copied to the socket.

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket to which data is to be written.
    data - Pointer to the string to be written.

  Returns:
    Pointer to the byte following the last byte written to the socket.  If
    this pointer does not dereference to a NULL byte, the buffer became full
    or the socket is not connected.

  Remarks:
    The return value of this function differs from that of TCPIP_TCP_ArrayPut.
    To write long strings in a single state, initialize the *data pointer to the
    first byte, then call this function repeatedly (breaking to the main
    stack loop after each call) until the return value dereferences to a NULL
    byte. Save the return value as the new starting *data pointer otherwise.

 */
const uint8_t*      TCPIP_TCP_StringPut(TCP_SOCKET hTCP, const uint8_t* Data);


//*****************************************************************************
/*
  Function:
    bool TCPIP_TCP_Flush(TCP_SOCKET hTCP)

  Summary:
    Immediately transmits all pending TX data.

  Description:
    This function immediately transmits all pending TX data with a PSH
    flag. If this function is not called, data will automatically be sent
    by the TCP state machine (see the TCPIP_TCP_ArrayPut() description/notes).

  Precondition:
    TCP is initialized and the socket is connected.

  Parameters:
    hTCP - The socket whose data is to be transmitted.

  Returns:
    true if data is flushed from the socket
    false otherwise

  Remarks:
    The application should not call this function explicitly because
    doing this will interfere with the TCP algorithm and degrade performance
    of the socket data transfer.
    One exception is when the application knows that it put all the data
    it needed into the TCP buffer and it makes sense to flush the socket instead
    of waiting TCP_AUTO_TRANSMIT_TIMEOUT_VAL timeout to elapse.

 */
bool  TCPIP_TCP_Flush(TCP_SOCKET hTCP);

//*****************************************************************************
/*
  Function:
    uint16_t TCPIP_TCP_FifoTxFullGet(TCP_SOCKET hTCP)

  Summary:
    Determines how many bytes are pending in the TCP TX FIFO.
  
  Description:
    This function determines how many bytes are pending in the TCP TX FIFO.

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket to check.

  Returns:
    Number of bytes pending to be flushed in the TCP TX FIFO.
    
  Remarks:
    None.
    
 */
uint16_t  TCPIP_TCP_FifoTxFullGet(TCP_SOCKET hTCP);

//*****************************************************************************
/*
  Macro:
    TCPIP_TCP_FifoTxFreeGet(TCP_SOCKET hTCP)

  Summary:
    Determines how many bytes are free and could be written in the TCP TX FIFO.

  Description:
    This macro returns the number of bytes that are free in the socket TX buffer.

  Parameters:
    hTCP - The socket to check.

  Returns:
    The number of bytes available to be written in the TCP TX buffer.
    
  Remarks:
    None.
*/
uint16_t  TCPIP_TCP_FifoTxFreeGet(TCP_SOCKET hTCP);


//******************************************************************************
/*
  Function:
    uint16_t  TCPIP_TCP_Put(TCP_SOCKET hTCP, uint8_t byte)

  Description:
    Writes a single byte to a TCP socket.

  Precondition:
    TCP socket should have been opened with TCPIP_TCP_ServerOpen()/TCPIP_TCP_ClientOpen().
    hTCP - valid socket

  Parameters:
    hTCP - The socket to which data is to be written.
    byte - The byte to write.

  Return Values:
    - 1 - The byte was written to the transmit buffer.
    - 0 - The transmit buffer was full, or the socket is not connected.

  Remarks:
    Note that this function is inefficient and its use is discouraged.
    A buffered approach (TCPIP_TCP_ArrayPut) is preferred.
  */
uint16_t  TCPIP_TCP_Put(TCP_SOCKET hTCP, uint8_t byte);

//*****************************************************************************
/*
  Function:
    uint16_t TCPIP_TCP_GetIsReady(TCP_SOCKET hTCP)

  Summary:
    Determines how many bytes can be read from the TCP RX buffer.

  Description:
    Call this function to determine how many bytes can be read from the
    TCP RX buffer.  If this function returns zero, the application must
    return to the main stack loop before continuing in order to wait for
    more data to arrive.

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket to check.

  Returns:
    The number of bytes available to be read from the TCP RX buffer.
    
  Remarks:
    None.
  */
uint16_t  TCPIP_TCP_GetIsReady(TCP_SOCKET hTCP);

//*****************************************************************************
/*
  Function:
    uint16_t TCPIP_TCP_ArrayGet(TCP_SOCKET hTCP, uint8_t* buffer, uint16_t len)

  Summary:
    Reads an array of data bytes from a TCP socket's RX buffer/FIFO.
  
  Description:
    This function reads an array of data bytes from a TCP socket's RX buffer/FIFO.  
    The data is removed from the FIFO in the process.

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket from which data is to be read.
    buffer - Pointer to the array to store data that was read.
    len  - Number of bytes to be read.

  Returns:
    The number of bytes read from the socket.  If less than len, the
    RX FIFO buffer became empty or the socket is not connected.

  Remarks:
    If the supplied buffer is null, the data is simply discarded.

 */
uint16_t  TCPIP_TCP_ArrayGet(TCP_SOCKET hTCP, uint8_t* buffer, uint16_t count);

//*****************************************************************************
/*
  Function:
    uint8_t TCPIP_TCP_Peek(TCP_SOCKET hTCP, uint16_t wStart)

  Summary:
    Peaks at one byte in the TCP RX buffer/FIFO without removing it from the buffer.

  Description:
    This function peaks at one byte in the TCP RX buffer/FIFO without removing it from 
    the buffer.

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket to peek from (read without removing from stream).
    wStart - Zero-indexed starting position within the FIFO to peek from.

  Return Values:
    Byte peeked from the RX FIFO.  If there is no data in the buffer or an
    illegal wStart starting offset is given, then an indeterminate value is
    returned.  The caller must ensure that valid parameters are passed to this function. 

  Remarks:
    Note that this function is inefficient and its use is discouraged.
    A buffered approach (TCPIP_TCP_ArrayPeek) is preferred.

 */
uint8_t  TCPIP_TCP_Peek(TCP_SOCKET hTCP, uint16_t wStart);


//*****************************************************************************
/*
  Function:
    uint16_t TCPIP_TCP_ArrayPeek(TCP_SOCKET hTCP, uint8_t *vBuffer, uint16_t wLen, uint16_t wStart)

  Summary:
    Reads a specified number of data bytes from the TCP RX buffer/FIFO without
    removing them from the buffer.

  Description:
    This function reads a specified number of data bytes from the TCP RX FIFO without
    removing them from the buffer.  No TCP control actions are taken as a
    result of this function (ex: no window update is sent to the remote node).

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP    - The socket to peek from (read without removing from stream).
    vBuffer - Destination to write the peeked data bytes.
    wLen    - Length of bytes to peek from the RX FIFO and copy to vBuffer.
    wStart  - Zero-indexed starting position within the FIFO from which to 
              start peeking.

  Return Values:
    Number of bytes actually peeked from the stream and copied to vBuffer.
    This value can be less than wLen if wStart + wLen is greater than the
    deepest possible character in the RX FIFO.

  Remarks:
    None.
 */
uint16_t  TCPIP_TCP_ArrayPeek(TCP_SOCKET hTCP, uint8_t *vBuffer, uint16_t wLen, uint16_t wStart);


//*****************************************************************************
/*
  Function:
    uint16_t TCPIP_TCP_FifoRxFreeGet(TCP_SOCKET hTCP)

  Summary:
    Determines how many bytes are free in the RX buffer/FIFO.
  
  Description:
    This function determines how many bytes are free in the RX buffer/FIFO.

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket to check.

  Returns:
    The number of bytes free in the TCP RX FIFO.  If zero, no additional
    data can be received until the application removes some data using one
    of the TCPIP_TCP_Get family functions.

 */
uint16_t  TCPIP_TCP_FifoRxFreeGet(TCP_SOCKET hTCP);


// *****************************************************************************
/*
  Macro:
    TCPIP_TCP_FifoRxFullGet

  Summary:
    Determines how many bytes are pending in the RX buffer/FIFO.

  Description:
    Alias to TCPIP_TCP_GetIsReady provided for API completeness
    
  Remarks:
    None.
*/
#define  TCPIP_TCP_FifoRxFullGet(a)  TCPIP_TCP_GetIsReady(a)


//*****************************************************************************
/*
  Function:
    uint16_t TCPIP_TCP_Discard(TCP_SOCKET hTCP)

  Summary:
    Discards any pending data in the RCP RX FIFO.
  
  Description:
    This function discards any pending data in the TCP RX FIFO.

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket whose RX buffer/FIFO is to be cleared.

  Returns:
    The number of bytes that have been discarded from the RX buffer.

 */
uint16_t TCPIP_TCP_Discard(TCP_SOCKET hTCP);


// *****************************************************************************
/*
  Function:
    uint16_t  TCPIP_TCP_Get(TCP_SOCKET hTCP, uint8_t* byte)

  Summary:
    Retrieves a single byte to a TCP socket.
  
  Description:
    This function retrieves a single byte to a TCP socket.

  Precondition:
    TCP socket should have been opened with TCPIP_TCP_ServerOpen()/TCPIP_TCP_ClientOpen().
    hTCP - valid socket

  Parameters:
    hTCP - The socket from which to read.
    byte - Pointer to location in which the read byte should be stored.

  Return Values:
    - 1 - A byte was read from the buffer.
    - 0 - The buffer was empty, or the socket is not connected.

  Remarks:
    Note that this function is inefficient.
    A buffered approach (TCPIP_TCP_ArrayGet) is preferred.

  */
uint16_t  TCPIP_TCP_Get(TCP_SOCKET hTCP, uint8_t* byte);


//*****************************************************************************
/*
  Function:
    uint16_t TCPIP_TCP_Find(TCP_SOCKET hTCP, uint8_t cFind, uint16_t wStart, 
                            uint16_t wSearchLen, bool bTextCompare)

  Summary:
    Searches for a byte in the TCP RX buffer.

  Description:
    This function finds the first occurrence of a byte in the TCP RX
    buffer.  It can be used by an application to abstract searches
    out of their own application code.  For increased efficiency, the
    function is capable of limiting the scope of search to a specific
    range of bytes.  It can also perform a case-insensitive search if
    required.

    For example, if the buffer contains "I love PIC MCUs!" and the cFind
    byte is ' ', a value of 1 will be returned.

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket to search within.
    cFind - The byte to find in the buffer.
    wStart - Zero-indexed starting position within the buffer.
    wSearchLen - Length from wStart to search in the buffer.
    bTextCompare - true for case-insensitive text search, false for binary search

  Return Values:
    - 0xFFFF - Search array not found
    - Any other value - Zero-indexed position of the first occurrence

  Remarks:
    For better performance of this function, try to search for characters
    that are expected to exist or limit the scope of the search as much as possible.
    The HTTP module, for example, uses this function to parse headers.
    However, it searches for newlines, then the separating colon, then reads the header name to
    an internal buffer for comparison.  This has proven to be significantly faster
    than searching for full header name strings outright.
 */
uint16_t  TCPIP_TCP_Find(TCP_SOCKET hTCP, uint8_t cFind, uint16_t wStart, uint16_t wSearchLen, bool bTextCompare);


//*****************************************************************************
/*
  Function:
    uint16_t TCPIP_TCP_ArrayFind(TCP_SOCKET hTCP, uint8_t* cFindArray, uint16_t wLen, 
                            uint16_t wStart, uint16_t wSearchLen, bool bTextCompare)

  Summary:
    Searches for a string in the TCP RX buffer.

  Description:
    This function finds the first occurrence of an array of bytes in the
    TCP RX buffer.  It can be used by an application to abstract searches
    out of their own application code.  For increased efficiency, the
    function is capable of limiting the scope of search to a specific
    range of bytes.  It can also perform a case-insensitive search if
    required.

    For example, if the buffer contains "I love PIC MCUs!" and the search
    array is "love" with a length of 4, a value of 2 will be returned.

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket to search within.
    cFindArray - The array of bytes to find in the buffer.
    wLen - Length of cFindArray.
    wStart - Zero-indexed starting position within the buffer.
    wSearchLen - Length from wStart to search in the buffer.
    bTextCompare - true for case-insensitive text search, false for binary search

  Return Values:
    - 0xFFFF - Search array not found
    - Any other value - Zero-indexed position of the first occurrence

  Remarks:
    For better performance of this function, try to search for characters
    that are expected to exist or limit the scope of the search as much as possible.
    The HTTP module, for example, uses this function to parse headers.
    However, it searches for newlines, then the separating colon, then reads the header name to
    an internal buffer for comparison.  This has proven to be significantly faster
    than searching for full header name strings outright.
 */
uint16_t  TCPIP_TCP_ArrayFind(TCP_SOCKET hTCP, const uint8_t* cFindArray, 
       uint16_t wLen, uint16_t wStart, uint16_t wSearchLen, bool bTextCompare);

//*****************************************************************************
/*
  Function:
    bool TCPIP_TCP_FifoSizeAdjust(TCP_SOCKET hTCP, uint16_t wMinRXSize, uint16_t wMinTXSize, 
                                  TCP_ADJUST_FLAGS vFlags)

  Summary:
    Adjusts the relative sizes of the RX and TX buffers.

  Description:
    This function can be used to simultaneously adjust the sizes
    of the RX and TX FIFOs.

    Adjusting the size of the TX/RX FIFO on the fly can allow for optimal transmission speed
    for one-sided application protocols.
    For example, HTTP typically begins by receiving large amounts of data from the client,
    then switches to serving large amounts of data back.
    Adjusting the FIFO at these points can increase performance in systems that have
    limited resources.
    Once the FIFOs are adjusted, a window update is sent.

    Although the TX and RX socket buffers are completely independent, for the
    purpose of this function they can be considered together if
    neither TCP_ADJUST_TX_ONLY nor TCP_ADJUST_RX_ONLY flags are set.

    In these conditions the TCP_ADJUST_FLAGS control the distribution
    of the  available space between the TX and RX FIFOs.
    If neither or both of TCP_ADJUST_GIVE_REST_TO_TX and
    TCP_ADJUST_GIVE_REST_TO_RX are set, the function distributes the
    remaining space (if any) equally.
    If the new requested FIFOs space is greater that the old existing FIFOs space
    the TCP_ADJUST_GIVE_REST_TO_TX and TCP_ADJUST_GIVE_REST_TO_RX are ignored.

    TCP_ADJUST_PRESERVE_RX and TCP_ADJUST_PRESERVE_TX request the
    preserving of the existing data.
    Existing data can be preserved as long as the old data in the buffer
    does not exceed the capacity of the new buffer.

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP        - The socket to be adjusted
    wMinRXSize  - Minimum number of bytes for the RX FIFO
    wMinTXSize  - Minimum number of bytes for the TX FIFO
    vFlags      - If TCP_ADJUST_TX_ONLY or TCP_ADJUST_RX_ONLY  are not set,
                    the TX and RX buffers are evaluated together and any
                    combination of the following flags is valid:
                    TCP_ADJUST_GIVE_REST_TO_RX, TCP_ADJUST_GIVE_REST_TO_TX
                    TCP_ADJUST_PRESERVE_RX, TCP_ADJUST_PRESERVE_TX
                    
                  If TCP_ADJUST_TX_ONLY or TCP_ADJUST_RX_ONLY is set
                    TX and RX buffers are treated individually
                    and TCP_ADJUST_GIVE_REST_TO_TX, TCP_ADJUST_GIVE_REST_TO_RX
                    values are irrelevant.
                    
                 TCP_ADJUST_TX_ONLY and TCP_ADJUST_RX_ONLY both set is invalid.

  Return Values:
    - true  - The FIFOs were adjusted successfully
    - false - New RX and/or TX buffers could not be allocated
              and therefore the socket was left unchanged.

  Remarks:
    The function will automatically maintain minimal values for both TX and RX buffers.

    To avoid having a socket with no associated buffers, the function first
    allocates the new buffers and, if succeeded, it frees the old ones.

    The TX and RX FIFOs (buffers) associated with a socket
    are completely separate and independent.
    However, when TCP_ADJUST_TX_ONLY or TCP_ADJUST_RX_ONLY are not set,
    for the purpose of this function, the TX and RX FIFOs are considered
    to be contiguous so that the total FIFO space is divided between
    the TX and RX FIFOs.
    This provides backward compatibility with previous versions of this function.

    The new flags TCP_ADJUST_TX_ONLY and TCP_ADJUST_RX_ONLY allow changing
    the size of TX and RX buffers independently.
    This is the preferred option.

    TX or RX associated buffer sizes can be changed too using the socket options.
    See TCPIP_TCP_OptionsSet.

    The size of the buffers should NOT be decreased when the socket has pending data
    to be sent to the remote party or to be received by the socket user.
    Doing this may disrupt the communication, make the TCP algorithm fail or have an 
    unpredicted behavior!

    This function is OBSOLETE and should NOT BE USED in new projects!
    It will be removed from the API in the future.
    The socket options TCP_OPTION_RX_BUFF and TCP_OPTION_TX_BUFF should be used to adjust the size of the socket buffers.

 */
bool  TCPIP_TCP_FifoSizeAdjust(TCP_SOCKET hTCP, uint16_t wMinRXSize, uint16_t wMinTXSize, 
                               TCP_ADJUST_FLAGS vFlags);

//*****************************************************************************
/*
  Function:
    bool TCPIP_TCP_SocketNetSet(TCP_SOCKET hTCP, TCPIP_NET_HANDLE hNet, bool persistent)

  Summary:
    Sets the interface for an TCP socket

  Description:
    This function sets the network interface for an TCP socket

  Precondition:
    TCP socket should have been opened with TCPIP_TCP_ClientOpen()/TCPIP_TCP_ServerOpen().
    hTCP - valid socket

  Parameters:
    hTCP        - The TCP socket
    hNet        - interface handle.
    persistent  - if true: 
                    when the socket connection is closed and it listens again, it will retain this network interface setting.
                    The same behavior is obtained by opening socket with TCPIP_TCP_ServerOpen() with a 
                    valid localAddress parameter
                - if false:
                    when a server socket that was created using TCPIP_TCP_ServerOpen() with localAddress == 0
                    closes the connection, the socket will re-listen on any interface

  Returns:
    - true  - Indicates success
    - false - Indicates failure

  Remarks:
    A NULL hNet can be passed (0) so that the current network interface selection 
    will be cleared.

    The persistent setting is applicable only to server sockets, as these sockets return to listen mode when a connection is closed.
    When a client socket connection is closed, the socket is destroyed and no information is maintained.
 */
bool   TCPIP_TCP_SocketNetSet(TCP_SOCKET hTCP, TCPIP_NET_HANDLE hNet, bool persistent);

//*****************************************************************************
/*
  Function:
TCPIP_NET_HANDLE  TCPIP_TCP_SocketNetGet(TCP_SOCKET hTCP);

  Summary:
    Gets the current network interface of an TCP socket.

  Description:
    This function returns the interface handle associated to a TCP socket.

  Precondition:
    TCP socket should have been opened with TCPIP_TCP_ClientOpen()/TCPIP_TCP_ServerOpen().
    hTCP - valid socket

  Parameters:
    hTCP - The TCP socket

  Returns:
    The handle of the local interface this socket is bound to.

  Remarks:
    The returned handle could be NULL if the socket is invalid or the socket is not 
    currently connected.

 */
TCPIP_NET_HANDLE  TCPIP_TCP_SocketNetGet(TCP_SOCKET hTCP);

// *****************************************************************************
/* Function:
    TCPIP_TCP_SignalHandlerRegister(TCP_SOCKET s, TCPIP_TCP_SIGNAL_TYPE sigMask, 
                          TCPIP_TCP_SIGNAL_FUNCTION handler, const void* hParam)

  Summary:
    Registers a TCP socket signal handler.

  Description:
    This function registers a TCP socket signal handler.
    The TCP module will call the registered handler when a
    TCP signal (TCPIP_TCP_SIGNAL_TYPE) occurs.

  Precondition:
    TCP valid socket.

  Parameters:
    s           - The TCP socket
    sigMask      - mask of signals to be reported
    handler     - signal handler to be called when a TCP event occurs.
    hParam      - Parameter to be used in the handler call.
                  This is user supplied and is not used by the TCP module.

  Returns:
    Returns a valid handle if the call succeeds, or a null handle if
    the call failed (null handler, no such socket, existent handler).

  Remarks:

    Only one signal handler per socket is supported.
    A new handler does not override the existent one.
    Instead TCPIP_TCP_SignalHandlerDeregister has to be explicitly called.

    The handler has to be short and fast. It is meant for
    setting an event flag, not for lengthy processing!

    The hParam is passed by the client.

    For multi-threaded systems the TCP/IP packet dispatch does not occur on the user thread.
    The signal handler will be called on a different thread context.
    It is essential that this handler is non blocking and really fast.
    

    For multi-threaded systems, once set, it is not recommended to change the signal handler at run time.
    Synchronization between user threads and packet dispatch threads could be difficult.
    If really need to be changed, make sure that the old handler could still be called
    and it should be valid until the new one is taken into account.
    TCPIP_TCP_SignalHandlerDeregister needs to be called before registering another handler.

 */

TCPIP_TCP_SIGNAL_HANDLE      TCPIP_TCP_SignalHandlerRegister(TCP_SOCKET s, TCPIP_TCP_SIGNAL_TYPE sigMask, 
                           TCPIP_TCP_SIGNAL_FUNCTION handler, const void* hParam);

// *****************************************************************************
/* Function:
    TCPIP_TCP_SignalHandlerDeregister(TCP_SOCKET s, TCPIP_TCP_SIGNAL_HANDLE hSig)

  Summary:
    Deregisters a previously registered TCP socket signal handler.
    
  Description:
    This function deregisters the TCP socket signal handler.

  Precondition:
    hSig valid TCP handle.

  Parameters:
    s       - The TCP socket
    hSig    - A handle returned by a previous call to TCPIP_TCP_SignalHandlerRegister.

  Returns:
    - true  - if the call succeeds
    - false - if no such handler is registered
 */

bool             TCPIP_TCP_SignalHandlerDeregister(TCP_SOCKET s, TCPIP_TCP_SIGNAL_HANDLE hSig);

// *****************************************************************************
/* Function:
    int TCPIP_TCP_SocketsNumberGet(void)

  Summary:
    Returns the number of existent TCP sockets.
    
  Description:
    This function returns the number of created TCP sockets.
    This is the maximum number of sockets that can be opened at any moment
    as it's been passed as parameter when TCP module was created.

  Precondition:
    TCP module properly initialized

  Parameters:
    None

  Returns:
    The number of TCP sockets
 */

int     TCPIP_TCP_SocketsNumberGet(void);

// *****************************************************************************
/* Function:
    bool    TCPIP_TCP_IsReady(void);

  Summary:
    Returns the current status of the TCP module
    
  Description:
    This function returns the current status of the TCP module.
    If the TCP quiet time has elapsed (or it was not enabled when TCP was built)
    then the TCP module is ready for operation and TCP sockets can send and receive data.
    If the TCP quiet time is still in effect, then the TCP module is quiet and it
    won't transmit any kind of data.

  Precondition:
    TCP module properly initialized

  Parameters:
    None

  Returns:
    true    - the quiet time has elapsed and the TCP module is ready to transmit data
    false   - the quiet time is still in effect and TCP module is silent

  Remarks:
    The TCP quiet time occurs at the stack start up and can be enabled/disabled
    using the TCP module configuration parameters.

 */

bool    TCPIP_TCP_IsReady(void);

// *****************************************************************************
/* Function:
    bool TCPIP_TCP_SocketTraceSet(TCP_SOCKET sktNo, bool enable)

  Summary:
    Sets the current socket trace status.
    
  Description:
    This function enables or disables the trace status of the specified socket.
    The trace functionality needs to be enabled in the TCP module for 
    this function to succeed.
    Currently when socket trace is enabled, the current socket state
    transitions are displayed at the system console. 

  Precondition:
    TCP module properly initialized

  Parameters:
    sktNo       - socket to enable/disable trace
    enable      - boolean to enable/disable socket trace

  Returns:
    - true if the call succeeded
    - false if there was an error (no such socket, tracing not enabled, etc.)
 */

bool    TCPIP_TCP_SocketTraceSet(TCP_SOCKET sktNo, bool enable);

//*******************************************************************************
/*
  Function:
    TCPIP_TCP_PROCESS_HANDLE    TCPIP_TCP_PacketHandlerRegister(TCPIP_TCP_PACKET_HANDLER pktHandler, const void* handlerParam)

  Summary:
    Sets a new packet processing handler.

  Description:
    This function registers a new packet processing handler.
    The caller can use the handler to be notified of incoming packets
    and given a chance to examine/process them.

  Precondition:
    TCP properly initialized

  Parameters:
    pktHandler      - the packet handler which will be called for an incoming packet
    handlerParam    - packet handler parameter

  Returns:
    - a valid TCPIP_TCP_PROCESS_HANDLE - if the operation succeeded
    - NULL - if the operation failed

  Example:
    <code>
    TCPIP_TCP_PROCESS_HANDLE pktHandle = TCPIP_TCP_PacketHandlerRegister( myPktHandler, myParam );
    </code>

  Remarks:
    Currently only one packet handler is supported for the TCP module.
    The call will fail if a handler is already registered.
    Use TCPIP_TCP_PacketHandlerDeregister first

    Exists only if TCPIP_TCP_EXTERN_PACKET_PROCESS is true 

  */
TCPIP_TCP_PROCESS_HANDLE     TCPIP_TCP_PacketHandlerRegister(TCPIP_TCP_PACKET_HANDLER pktHandler, const void* handlerParam);


//*******************************************************************************
/*
  Function:
    bool    TCPIP_TCP_PacketHandlerDeregister(TCPIP_TCP_PROCESS_HANDLE pktHandle);

  Summary:
    Deregisters a previously registered packet handler.

  Description:
    This function removes a packet processing handler.

  Precondition:
    TCP properly initialized

  Parameters:
    pktHandle   - TCPIP packet handle obtained by a call to TCPIP_TCP_PacketHandlerRegister


  Returns:
    - true  - if the operation succeeded
    - false - if the operation failed

  Example:
    <code>
    TCPIP_TCP_PROCESS_HANDLE myHandle = TCPIP_TCP_PacketHandlerRegister(myPacketHandler, myParam );
    // process incoming packets
    // now we're done with it
    TCPIP_TCP_PacketHandlerDeregister(myHandle);
    </code>

  Remarks:
    Exists only if TCPIP_TCP_EXTERN_PACKET_PROCESS is true 

  */
bool    TCPIP_TCP_PacketHandlerDeregister(TCPIP_TCP_PROCESS_HANDLE pktHandle);

// *****************************************************************************
/*
  Function:
    void  TCPIP_TCP_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs TCP module tasks in the TCP/IP stack.

  Precondition:
    The TCP module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_TCP_Task(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // __TCP_H__

/*
 End of File
*/
