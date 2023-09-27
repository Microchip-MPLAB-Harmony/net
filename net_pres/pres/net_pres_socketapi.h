/*******************************************************************************
MPLAB Harmony Networking Presentation socket API header file

  Company:
    Microchip Technology Inc.
    
  Filename:
    net_pres_socketapi.h
    
  Summary:
    Describes the API for accessing presentation layer sockets.
    
  Description:
    This file describes the API for accessing Networking Presentation Layer sockets.

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2015-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef _NET_PRES_SOCKET_API_
#define _NET_PRES_SOCKET_API_

#include <stdint.h>
#include <stdbool.h>
#include "system_config.h"

#include "net_pres.h"

#ifdef __cplusplus
extern "C" {
#endif

// *****************************************************************************
/*
  Enumeration:
    NET_PRES_SKT_T

  Summary:
    NET_PRES socket type

  Description:
    Enumeration describing all the possible NET_PRES socket types

  Remarks:
    None
*/    
typedef enum {

    NET_PRES_SKT_CLIENT = 0x0001,
    NET_PRES_SKT_SERVER = 0x0002,
    NET_PRES_SKT_STREAM = 0x0004,
    NET_PRES_SKT_DATAGRAM = 0x0008,
    NET_PRES_SKT_UNENCRYPTED = 0x0010,
    NET_PRES_SKT_ENCRYPTED = 0x0020,
    
    NET_PRES_SKT_UNENCRYPTED_STREAM_CLIENT = (NET_PRES_SKT_UNENCRYPTED | 
                                              NET_PRES_SKT_STREAM | 
                                              NET_PRES_SKT_CLIENT),
    NET_PRES_SKT_UNENCRYPTED_STREAM_SERVER = (NET_PRES_SKT_UNENCRYPTED | 
                                              NET_PRES_SKT_STREAM | 
                                              NET_PRES_SKT_SERVER),
    NET_PRES_SKT_UNENCRYPTED_DATAGRAM_CLIENT = (NET_PRES_SKT_UNENCRYPTED | 
                                                NET_PRES_SKT_DATAGRAM | 
                                                NET_PRES_SKT_CLIENT),
    NET_PRES_SKT_UNENCRYPTED_DATAGRAM_SERVER = (NET_PRES_SKT_UNENCRYPTED | 
                                                NET_PRES_SKT_DATAGRAM | 
                                                NET_PRES_SKT_SERVER),
    NET_PRES_SKT_ENCRYPTED_STREAM_CLIENT = (NET_PRES_SKT_ENCRYPTED | 
                                            NET_PRES_SKT_STREAM | 
                                            NET_PRES_SKT_CLIENT),
    NET_PRES_SKT_ENCRYPTED_STREAM_SERVER = (NET_PRES_SKT_ENCRYPTED | 
                                            NET_PRES_SKT_STREAM | 
                                            NET_PRES_SKT_SERVER),
    NET_PRES_SKT_ENCRYPTED_DATAGRAM_CLIENT = (NET_PRES_SKT_ENCRYPTED | 
                                              NET_PRES_SKT_DATAGRAM | 
                                              NET_PRES_SKT_CLIENT),
    NET_PRES_SKT_ENCRYPTED_DATAGRAM_SERVER = (NET_PRES_SKT_ENCRYPTED | 
                                              NET_PRES_SKT_DATAGRAM | 
                                              NET_PRES_SKT_SERVER),
    NET_PRES_SKT_DEFAULT_STREAM_CLIENT = (NET_PRES_SKT_STREAM | 
                                            NET_PRES_SKT_CLIENT),
    NET_PRES_SKT_DEFAULT_STREAM_SERVER = (NET_PRES_SKT_STREAM | 
                                            NET_PRES_SKT_SERVER),
    NET_PRES_SKT_DEFAULT_DATAGRAM_CLIENT = (NET_PRES_SKT_DATAGRAM | 
                                              NET_PRES_SKT_CLIENT),
    NET_PRES_SKT_DEFAULT_DATAGRAM_SERVER = (NET_PRES_SKT_DATAGRAM | 
                                              NET_PRES_SKT_SERVER)
} NET_PRES_SKT_T;

// *****************************************************************************
/*
  Enumeration:
    NET_PRES_SKT_ADDR_T

  Summary:
    NET_PRES address type

  Description:
    Enumeration describing all the possible NET_PRES address types

  Remarks:
    None
*/    
typedef enum {
    NET_PRES_SKT_ADDR_UNKNOWN,
} NET_PRES_SKT_ADDR_T;



// *****************************************************************************
/*
  Enumeration:
    NET_PRES_SKT_OPTION_TYPE

  Summary:
    NET_PRES option type

  Description:
    Enumeration describing all the possible NET_PRES option types

  Remarks:
    None
*/    
typedef enum {
    NET_PRES_SKT_OPT_UNKNOWN,
} NET_PRES_SKT_OPTION_TYPE;


// *****************************************************************************
/*
  Enumeration:
    NET_PRES_ADDRESS

  Summary:
    NET_PRES address

  Description:
    Enumeration describing the NET_PRES address

  Remarks:
    None
*/    
typedef struct {
    uint8_t addr[16];
} NET_PRES_ADDRESS;



// *****************************************************************************
/*
  Enumeration:
    NET_PRES_SKT_ERROR_T

  Summary:
    NET_PRES error type

  Description:
    Enumeration describing all the possible NET_PRES error types

  Remarks:
    None
*/    
typedef enum {
    NET_PRES_SKT_OK = 0,
    NET_PRES_SKT_OP_NOT_SUPPORTED = -1,  // Most likely the function is not 
                                         // supported by the socket type
    NET_PRES_SKT_OP_OUT_OF_HANDLES = -2,
    NET_PRES_SKT_OP_INVALID_INDEX = -3,
    NET_PRES_SKT_UNKNOWN_ERROR = -4,
    NET_PRES_SKT_INVALID_SOCKET = -5,  
    NET_PRES_SKT_ENC_NEGO_FAILED = -6,
    NET_PRES_SKT_HANDLER_BUSY = -7,     // a signal handler already registered
    NET_PRES_SKT_HANDLER_ERROR = -8,    // no such signal handler
    NET_PRES_SKT_HANDLER_TRANSP_ERROR = -9,    // transport layer signal operation failed
}NET_PRES_SKT_ERROR_T;

//*****************************************************************************
/*
  Summary:
    Opens a presentation socket.

  Description:
    Provides a unified method for opening all presentation sockets types. Sockets 
    are created at the presentation layer module initialization, and can be claimed 
    with this function and freed using NET_PRES_SocketClose.
    The presentation layer will call the corresponding open function in the transport 
    layer, and if encryption is specified the presentation layer will also handle 
    encryption negotiation.

  Precondition:
    The MPLAB Harmony Networking Presentation Layer is initialized.

  Parameters:
    index          - Index of the presentation layer.
    socketType     - The type of socket to open.
    addType        - The type of address being used. This is passed unaltered to the 
                     transport layer.
    port           - The port to listen or to send to.  This is passed unaltered to 
                     the transport layer.
    addr           - Address to use. This is passed unaltered to the transport layer.
    error          - The extended error code of the function.

  Returns:
    - NET_PRES_INVALID_SOCKET      - No sockets of the specified type were available to be
                                     opened
    - NET_PRES_SKT_HANDLE_T handle - Returned when NET_PRES_INVALID_SOCKET is returned. Save 
                                     this handle and use it when calling all other presentation 
                                     socket APIs.
 */

NET_PRES_SKT_HANDLE_T NET_PRES_SocketOpen(NET_PRES_INDEX index, NET_PRES_SKT_T socketType, NET_PRES_SKT_ADDR_T addrType, NET_PRES_SKT_PORT_T port, NET_PRES_ADDRESS * addr, NET_PRES_SKT_ERROR_T* error);

//*****************************************************************************
/*
  Summary:
    Checks to see if a mode is supported by open.

  Description:
    This function checks to see if a mode is supported by open.

  Precondition:
    The MPLAB Harmony Networking Presentation Layer is initialized.

  Parameters:
    index          - Index of the presentation layer.
    socketType     - The type of socket to mode to be checked.

  Returns:
    - true  - The mode is supported
    - false - The mode is not supported
 */

bool NET_PRES_SocketIsOpenModeSupported(NET_PRES_INDEX index, NET_PRES_SKT_T socketType);


//******************************************************************************
/*
  Summary:
    Binds a socket to a local address.

  Description:
    This function calls directly to the transport layer's bind function.

  Precondition:
    A socket needs to have been opened by NET_PRES_SocketOpen.

  Parameters:
    handle    - The socket to bind.
    addType   - The type of address being used. This is passed unaltered to the 
                transport layer.
    port      - The port to use. This is passed unaltered to the transport layer.
    addr      - The address to bind to.  This is passed unaltered to the transport 
                layer.

  Returns:
    - true  - Indicates success
    - false - Indicates failure

 */

bool NET_PRES_SocketBind(NET_PRES_SKT_HANDLE_T handle, NET_PRES_SKT_ADDR_T addrType, NET_PRES_SKT_PORT_T port, NET_PRES_ADDRESS *addr);

//******************************************************************************
/*
  Summary:
    Binds a socket to a remote local address.

  Description:
   This function calls directly to the transport layer's remote bind function.

  Precondition:
    A socket needs to have been opened by NET_PRES_SocketOpen.

  Parameters:
    handle   - The socket to bind.
    addType  - The type of address being used. This is passed unaltered to the 
               transport layer.
    port     - The port to use.  This is passed unaltered to the transport layer.
    addr     - The address to bind to. This is passed unaltered to the transport layer.

  Returns:
    - true  - Indicates success
    - false - Indicates failure

 */

bool NET_PRES_SocketRemoteBind(NET_PRES_SKT_HANDLE_T handle, NET_PRES_SKT_ADDR_T addrType, NET_PRES_SKT_PORT_T port, NET_PRES_ADDRESS *addr);


//******************************************************************************
/*
  Summary:
    Allows setting options to a socket like adjust RX/TX buffer size, etc.

  Description:
    Various options can be set at the socket level. This function calls directly 
    to the transport layer's OptionSet function, if it exists.

  Precondition:
    A socket needs to have been opened by NET_PRES_SocketOpen.

  Parameters:
    handle    - The socket to set options for.
    option    - The specific option to be set, this is passed unaltered to the 
                transport layer.
    optParam  - The option value, this is passed unaltered to the transport layer.

  Returns:
    - true  - Indicates success
    - false - Indicates failure

  */
bool NET_PRES_SocketOptionsSet(NET_PRES_SKT_HANDLE_T handle, NET_PRES_SKT_OPTION_TYPE option, 
void* optParam);

//******************************************************************************
/*
  Summary:
    Allows the options for a socket such as, current RX/TX buffer size, etc.,
    to be obtained.

  Description:
    Various options can be obtained at the socket level.
    This function calls directly to the transport layer's OptionGet function, if 
    it exists.

  Precondition:
    A socket needs to have been opened by NET_PRES_SocketOpen.

  Parameters:
    handle    - The socket to set options for.
    option    - The specific option to set, this is passed unaltered to the transport layer.
    optParam  - The option value, which is passed unaltered to the transport layer.

  Returns:
    - true  - Indicates success
    - false - Indicates failure

  */

bool NET_PRES_SocketOptionsGet(NET_PRES_SKT_HANDLE_T handle, NET_PRES_SKT_OPTION_TYPE option, void* optParam);

//******************************************************************************
/*
  Summary:
    Determines whether a socket has an established connection.

  Description:
    This function determines whether a socket has an established connection to
    a remote node.  This function calls directly to the transport layer's 
    IsConnected function, if it exists.

  Precondition:
    A socket needs to have been opened by NET_PRES_SocketOpen.

  Parameters:
    handle   - The presentation layer socket handle.

  Return Values:
    - true  - The socket has an established connection to a remote node
    - false - The socket is not currently connected

  */
bool NET_PRES_SocketIsConnected(NET_PRES_SKT_HANDLE_T handle);

//*****************************************************************************
/*
  Summary:
    Self-clearing semaphore indicating socket reset.

  Description:
    This function is a self-clearing semaphore indicating whether or not
    a socket has been reset since the previous call.
    This function calls directly to the transport layer's WasReset function, if it exists.

  Precondition:
    A socket needs to have been opened by NET_PRES_SocketOpen.

  Parameters:
    handle  - The presentation layer socket handle.

  Return Values:
    - true  - The socket has been reset since the previous call
    - false - The socket has not been reset since the previous call
 */
bool NET_PRES_SocketWasReset(NET_PRES_SKT_HANDLE_T handle);

//*****************************************************************************
/*
  Summary:
    Function indicating that the remote node disconnected the socket.

  Description:
    This function indicates that the socket has received a FIN from the remote node.
    This signals that the remote node has finished transmitting its data.
    At this point it is up to the socket owner to send some more data or to close the socket.

    This function calls directly to the transport layer's WasDisconnected function, if it exists.
  
  Precondition:
    A socket needs to have been opened by NET_PRES_SocketOpen.

  Parameters:
    handle  - The presentation layer socket handle.

  Return Values:
    - true  - The socket has received a FIN from the remote node
    - false - The socket has not received a FIN from the remote node
 */
bool NET_PRES_SocketWasDisconnected(NET_PRES_SKT_HANDLE_T handle);

//******************************************************************************
/*
  Summary:
    Disconnects an open socket.

  Description:
    This function calls the transport layer's disconnect function directly, 
    if it exists.

  Precondition:
    A socket needs to have been opened by NET_PRES_SocketOpen.

  Parameters:
    handle  - The presentation layer socket handle.

  Returns:
    - true  - Indicates success
    - false - Indicates failure

  */
bool NET_PRES_SocketDisconnect(NET_PRES_SKT_HANDLE_T handle);

//******************************************************************************
/*
  Summary:
    Connects a client socket.

  Description:
    This function calls the transport layer's connect function directly, 
    if it exists.


  Precondition:
    A socket needs to have been opened by NET_PRES_SocketOpen.

  Parameters:
    handle  - The presentation layer socket handle.

  Returns:
    - true  - Indicates success
    - false - Indicates failure

  */
bool NET_PRES_SocketConnect(NET_PRES_SKT_HANDLE_T handle);

//******************************************************************************
/*
  Summary:
    Disconnects an open socket and destroys the socket handle, releasing the 
    associated resources.

  Description:
    This function calls the encryption provider's close function and then calls 
    the close function of the transport layer for the socket and frees the socket 
    for reuse.

  Precondition:
    A socket needs to have been opened by NET_PRES_SocketOpen.

  Parameters:
    handle   - The presentation layer socket handle.

  Returns:
    None.
    
  */
void NET_PRES_SocketClose(NET_PRES_SKT_HANDLE_T handle);

//*****************************************************************************
/*
  Summary:
    Obtains information about a currently open socket.

  Description:
    This function calls the transport layer's SocketInfoGet, if it exists.

  Precondition:
    A socket needs to have been opened by NET_PRES_SocketOpen.

  Parameters:
    handle  - The presentation layer socket handle.
    info    - The buffer that the information gets written to.

  Returns:
    - true  - Indicates success
    - false - Indicates failure

 */

bool NET_PRES_SocketInfoGet(NET_PRES_SKT_HANDLE_T handle, void * info);

//*****************************************************************************
/*
  Summary:
    Determines how much free space is available in the TX buffer.

  Description:
    This function calls the transport or the encryption layer's WriteIsReady, 
    if it exists.
  
  Precondition:
    A socket needs to have been opened by NET_PRES_SocketOpen.

  Parameters:
    handle  - Presentation layer socket handle.
    reqSize - Write size to check for.
    minSize - Minimum size that could be guaranteed. Could be '0' if not needed.

  Returns:
    The number of bytes available in the TX buffer:
    - >= reqSize - If the requested space is available in the output buffer
    - >= minSize - I there's at least this minimum space (minSize != 0)
    - 0          - Requested (minimum) space cannot be granted
    
 */
uint16_t NET_PRES_SocketWriteIsReady(NET_PRES_SKT_HANDLE_T handle, uint16_t reqSize, 
                                     uint16_t minSize);

//*****************************************************************************
/*
  Summary:
    Takes a buffer and sends it to the encryption provider.
    
  Description:
    This function takes a buffer and sends it to the encryption provider for an 
    encrypted socket, or to the transport layer directly for an unencrypted
    socket.

  Precondition:
    A socket needs to have been opened by NET_PRES_SocketOpen.

  Parameters:
    handle    - The presentation layer socket handle.
    buffer    - The pointer to the array to be written.
    size      - The number of bytes to be written.

  Returns:
    The number of bytes written to the socket. If less than len, the
    buffer became full or the socket is not connected.

 */

uint16_t NET_PRES_SocketWrite(NET_PRES_SKT_HANDLE_T handle, const void * buffer, uint16_t size);

//*****************************************************************************
/*
  Summary:
    Immediately transmits all pending TX data.

  Description:
    This function calls the transport layer's flush function, if it exists.

  Precondition:
    A socket needs to have been opened by NET_PRES_SocketOpen.

  Parameters:
    handle  - The presentation layer socket handle.

  Returns:
    - The number of flushed bytes
    - 0, if no flushed bytes or an error occurred

 */

uint16_t NET_PRES_SocketFlush(NET_PRES_SKT_HANDLE_T handle);

//*****************************************************************************
/*
  Summary:
    Determines how many bytes can be read from the RX buffer.

  Description:
    Call this function to determine how many bytes can be read from the
    RX buffer.  If this function returns zero, the application must
    return to the main stack loop before continuing in order to wait for
    more data to arrive.  This function calls the transport layer's ReadIsReady
    function.  When using an encrypted connection the number of unencrypted bytes
    may turn out to be different than what this function returns.

  Precondition:
    A socket needs to have been opened by NET_PRES_SocketOpen.

  Parameters:
    handle     - The presentation layer socket handle.

  Returns:
    The number of bytes available to be read from the TCP RX buffer.
    
  */
uint16_t NET_PRES_SocketReadIsReady(NET_PRES_SKT_HANDLE_T handle);

//*****************************************************************************
/*
  Summary:
    Reads an array of data bytes from a socket's RX buffer/FIFO.

  Description:
    This function reads an array of data bytes from a socket's RX buffer/FIFO.  
    The data is removed from the FIFO in the process.  If the connection is encrypted
    this function calls the encryption provider's read function, otherwise it
    calls the transport layer's read function.

  Precondition:
    A socket needs to have been opened by NET_PRES_SocketOpen.

  Parameters:
    handle - The presentation layer socket handle.
    buffer - The pointer to the array to store data that was read.
    size   - The number of bytes to be read.

  Returns:
    The number of bytes read from the socket.  If less than len, the
    RX FIFO buffer became empty or the socket is not connected.

  Remarks:
    For encrypted connections, a null buffer is an invalid parameter.
    For non encrypted connections if the supplied buffer is null,
    the data is simply discarded.

 */
uint16_t NET_PRES_SocketRead(NET_PRES_SKT_HANDLE_T handle, void * buffer, uint16_t size);

//*****************************************************************************
/*

  Summary:
    Reads a specified number of data bytes from the RX buffer/FIFO without
    removing them from the buffer.

  Description:
    If the socket is encrypted this function will call the encryption provider's
    peek function.  Otherwise this function calls the transport layer's peek
    function.

  Precondition:
    A socket needs to have been opened by NET_PRES_SocketOpen.

  Parameters:
    handle - The presentation layer socket handle.
    buffer - Destination to write the peeked data bytes.
    size   - Length of bytes to peek from the RX FIFO and copy to the buffer.
  

  Return Values:
    The number of bytes actually peeked from the stream and copied to the buffer.

  Remarks:
    None
 */

uint16_t NET_PRES_SocketPeek(NET_PRES_SKT_HANDLE_T handle,  void * buffer, uint16_t size);

//*****************************************************************************
/*
  Summary:
    Discards any pending data in the RX FIFO.
  
  Description:
    This function calls the transport layer's discard function, if it exists.

  Precondition:
    A socket needs to have been opened by NET_PRES_SocketOpen

  Parameters:
    handle   - The presentation layer socket handle.

  Returns:
    The number of bytes that have been discarded from the RX buffer.

 */
uint16_t NET_PRES_SocketDiscard(NET_PRES_SKT_HANDLE_T handle);

// *****************************************************************************
/* 
  Summary:
    Registers a socket signal handler.

  Description:
    This function calls the transport layer's register signal handle function 
    directly, if it exists

  Precondition:
     A socket needs to have been opened by NET_PRES_SocketOpen

  Parameters:
    handle     - The presentation layer socket handle.
    sigMask    - The mask of signals to be reported, this parameter is passed to 
                 the transport layer directly.
    handler    - signal handler to be called when an event occurs.  This parameter 
                 is passed to the transport layer directly.
    hParam     - Parameter to be used in the handler call.  This parameter is passed 
                 to the transport layer directly.

  Returns:
    - valid handle      - Indicates the call succeeded
    - null handle       - Indicates the call failed (null handler, no such socket, 
                          existent handler)

 */

NET_PRES_SIGNAL_HANDLE NET_PRES_SocketSignalHandlerRegister(NET_PRES_SKT_HANDLE_T handle, 
                 uint16_t sigMask, NET_PRES_SIGNAL_FUNCTION handler, const void* hParam);

// *****************************************************************************
/* 
  Summary:
    Deregisters a previously registered socket signal handler.
    
  Description:
    This function calls the transport layer's deregister signal handler function,
    if it exists

  Precondition:
     A socket needs to have been opened by NET_PRES_SocketOpen.

  Parameters:
    handle  - The presentation layer socket handle.
    hSig    - A handle returned by a previous call to TCPIP_TCP_SignalHandlerRegister.

  Returns:
    - true  - If the call succeeds
    - false - If no such handler is registered
 */

bool NET_PRES_SocketSignalHandlerDeregister(NET_PRES_SKT_HANDLE_T handle, 
                                            NET_PRES_SIGNAL_HANDLE hSig);

// *****************************************************************************
/* 
  Summary:
   This function checks if encryption negotiation is still in progress.

  Description:
    This function returns checks to see if an encrypted socket is still undergoing 
    negotiation. 
 
   Precondition:
     A socket needs to have been opened by NET_PRES_SocketOpen.

  Parameters:
   handle    - The presentation layer socket handle.

  Returns:
    - true  - If the encryption negotiation is still ongoing
    - false - If there is no ongoing negotiation

 */

bool NET_PRES_SocketIsNegotiatingEncryption(NET_PRES_SKT_HANDLE_T handle);

// *****************************************************************************
/* 
  Summary:
    This function checks whether a connection is secure.

  Description:
    This function returns whether or not the connection is secure. It will return
    true if encryption negotiation was successful .
 
   Precondition:
     A socket needs to have been opened by NET_PRES_SocketOpen.

  Parameters:
   handle   - The presentation layer socket handle.

  Returns:
    - true  - If the communications is secure
    - false - If the communications is not secure

 */
bool NET_PRES_SocketIsSecure(NET_PRES_SKT_HANDLE_T handle);

// *****************************************************************************
/* 
  Summary:
    This function turns an insecure socket into a secure socket.

  Details:
    This function will turn an unencrypted socket into an encrypted socket and starts
    encryption negotiation.
 
   Precondition:
     A socket needs to have been opened by NET_PRES_SocketOpen.

  Parameters:
     handle - The presentation layer socket handle.

  Returns:
    - true  - If the call was successful
    - false - If the call was unsuccessful

 */

bool NET_PRES_SocketEncryptSocket(NET_PRES_SKT_HANDLE_T handle);

// *****************************************************************************
/* 
  Summary:
    This function returns the last error code for this socket.

  Details:
    This function will return the last error code that was set for this socket
    and it will clear the current error code.
    An error code is set whenever a socket operation fails for some
    missing functionality, bad parameter, etc.
 
   Precondition:
    A socket needs to have been opened by NET_PRES_SocketOpen.

  Parameters:
    handle - The presentation layer socket handle.

  Returns:
    A NET_PRES_SKT_ERROR_T representing the last encountered error for this socket.

 */

NET_PRES_SKT_ERROR_T NET_PRES_SocketLastError(NET_PRES_SKT_HANDLE_T handle);

// *****************************************************************************
/* 
  Summary:
    This function returns the transport layer handle.

  Details:
    This function returns the transport layer handle for a valid socket
 
   Precondition:
    A socket needs to have been opened by NET_PRES_SocketOpen.

  Parameters:
    handle - The presentation layer socket handle.

  Returns:
    A valid transport layer handle that can be casted into the proper type.

 */


NET_PRES_SKT_HANDLE_T NET_PRES_SocketGetTransportHandle(NET_PRES_SKT_HANDLE_T handle);


#ifdef __cplusplus
}
#endif

#include "net_pres_socketapiconversion.h"

#endif //_NET_PRES_SOCKET_API_
