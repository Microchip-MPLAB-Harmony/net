/*******************************************************************************
MPLAB Harmony Networking Presentation socket conversion API header file

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
/*****************************************************************************
 Copyright (C) 2015-2018 Microchip Technology Inc. and its subsidiaries.

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


//DOM-IGNORE-END

#ifndef _NET_PRES_SOCKET_CONV_API_
#define _NET_PRES_SOCKET_CONV_API_

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
  Summary:
    Opens a presentation socket.

  Description:
    Provides a unified method for opening all presentation sockets types. Sockets 
	are created at the presentation layer module initialization, and can be claimed 
	with this function and freed using NET_PRES_SKT_Close.
    The presentation layer will call the corresponding open function in the transport 
	layer, and if encryption is specified the presentation layer will also handle 
	encryption negotiation.

  Precondition:
    MPLAB Harmony Networking Presentation Layer is initialized.

  Parameters:
    index          - Index of the presentation layer
    socketType     - The type of socket to open.
    addType        - The type of address being used. This is passed unaltered to the 
	                 transport layer.
    port           - The port to listen or to send to.  This is passed unaltered to 
	                 the transport layer.
    addr           - Address to use. This is passed unaltered to the transport layer.
    error          - The extended error code of the function

  Returns:
    - NET_PRES_INVALID_SOCKET      - No sockets of the specified type were available to be
                                     opened
    - NET_PRES_SKT_HANDLE_T handle - Returned when NET_PRES_INVALID_SOCKET is returned. Save 
	                                 this handle and use it when calling all other presentation 
								     socket APIs.
 */

#define NET_PRES_SKT_Open NET_PRES_SocketOpen

/*****************************************************************************
  Summary:
 Check to see if a mode is supported by open.

  Description:
Check to see if a mode is supported by open.

  Precondition:
    MPLAB Harmony Networking Presentation Layer is initialized.

  Parameters:
    index          - Index of the presentation layer
    socketType     - The type of socket to mode to be checked.

  Returns:
    - true                         - mode is supported
    - false                        - mode is not supported
 */

#define NET_PRES_SKT_IsOpenModeSupported NET_PRES_SocketIsOpenModeSupported

/******************************************************************************
  Summary:
    Binds a socket to a local address.

  Description:
    This function calls directly to the transport layer's bind function.

  Precondition:
    A socket needs to have been opened by NET_PRES_SKT_Open.

  Parameters:
    handle    - The socket to bind
    addType   - The type of address being used. This is passed unaltered to the 
	            transport layer.
    port      - The port to use. This is passed unaltered to the transport layer.
    addr      - The address to bind to.  This is passed unaltered to the transport 
	            layer.
    error     - The extended error code of the function

  Returns:
    - true  - Indicates success
    - false - Indicates failure

 */

#define NET_PRES_SKT_Bind NET_PRES_SocketBind

/******************************************************************************
  Summary:
    Binds a socket to a remote local address.

  Description:
   This function calls directly to the transport layer's remote bind function.

  Precondition:
    A socket needs to have been opened by NET_PRES_SKT_Open.

  Parameters:
    handle   - The socket to bind
    addType  - The type of address being used. This is passed unaltered to the 
	           transport layer.
    port     - The port to use.  This is passed unaltered to the transport layer.
    addr     - The address to bind to. This is passed unaltered to the transport layer.
    error    - The extended error code of the function

  Returns:
    - true  - Indicates success
    - false - Indicates failure

 */

#define NET_PRES_SKT_RemoteBind NET_PRES_SocketRemoteBind

/******************************************************************************
  Summary:
    Allows setting options to a socket like adjust RX/TX buffer size, etc.

  Description:
    Various options can be set at the socket level. This function calls directly 
	to the transport layer's OptionSet function, if it exists.

  Precondition:
    A socket needs to have been opened by NET_PRES_SKT_Open.

  Parameters:
    handle    - The socket to set options for
    option    - The specific option to be set, this is passed unaltered to the 
	            transport layer
    optParam  - The option value, this is passed unaltered to the transport layer
    error     - The extended error code of the function

  Returns:
    - true  - Indicates success
    - false - Indicates failure

  */
#define NET_PRES_SKT_OptionsSet NET_PRES_SocketOptionsSet
/******************************************************************************
  Summary:
    Allows getting the options for a socket like: current RX/TX buffer size, etc

  Description:
    Various options can be get at the socket level.
    This function calls directly to the transport layer's OptionGet function, if 
	it exists.

  Precondition:
    A socket needs to have been opened by NET_PRES_SKT_Open.

  Parameters:
    handle    - The socket to set options for
    option    - The specific option to set, this is passed unaltered to the transport layer
    optParam  - The option value, which is passed unaltered to the transport layer
    error     - The extended error code of the function

  Returns:
    - true  - Indicates success
    - false - Indicates failure

  */

#define NET_PRES_SKT_OptionsGet NET_PRES_SocketOptionsGet

/******************************************************************************
  Summary:
    Determines if a socket has an established connection.

  Description:
    This function determines if a socket has an established connection to
    a remote node.  This function calls directly to the transport layer's 
    IsConnected function, if it exists.

  Precondition:
    A socket needs to have been opened by NET_PRES_SKT_Open.

  Parameters:
    handle   - The presentation layer socket handle
    error    - The extended error code of the function

  Return Values:
    - true  - The socket has an established connection to a remote node
    - false - The socket is not currently connected

  */
#define NET_PRES_SKT_IsConnected NET_PRES_SocketIsConnected
/*****************************************************************************
  Summary:
    Self-clearing semaphore indicating socket reset.

  Description:
    This function is a self-clearing semaphore indicating whether or not
    a socket has been disconnected since the previous call.   This function calls 
    directly to the transport layer's IsConnected function, if it exists.

  Precondition:
    A socket needs to have been opened by NET_PRES_SKT_Open.

  Parameters:
    handle  - The presentation layer socket handle
    error   - The extended error code of the function

  Return Values:
    - true  - The socket has been disconnected since the previous call
    - false - The socket has not been disconnected since the previous call
 */
#define NET_PRES_SKT_WasReset NET_PRES_SocketWasReset
/******************************************************************************
  Summary:
    Disconnects an open socket.

  Description:
    This function calls the transport layer's disconnect function directly, 
	if it exists.

  Precondition:
    A socket needs to have been opened by NET_PRES_SKT_Open.

  Parameters:
    handle  - The presentation layer socket handle
    error   - The extended error code of the function

  Returns:
    - true  - Indicates success
    - false - Indicates failure

  */
#define NET_PRES_SKT_Disconnect NET_PRES_SocketDisconnect
/******************************************************************************
  Summary:
	Connects a client socket.

  Description:
	This function calls the transport layer's connect function directly, 
	if it exists.


  Precondition:
    A socket needs to have been opened by NET_PRES_SKT_Open.

  Parameters:
    handle  - The presentation layer socket handle
    error   - The extended error code of the function

  Returns:
    - true  - Indicates success
    - false - Indicates failure

  */
#define NET_PRES_SKT_Connect NET_PRES_SocketConnect

/******************************************************************************
  Summary:
	Disconnects an open socket and destroys the socket handle, releasing the 
	associated resources.

  Description:
    This function calls the encryption provider's close function and then calls 
	the close function of the transport layer for the socket and frees the socket 
	for reuse.

  Precondition:
    A socket needs to have been opened by NET_PRES_SKT_Open.

  Parameters:
    handle   - The presentation layer socket handle
    error    - The extended error code of the function

  Returns:
    None.
	
  */
#define NET_PRES_SKT_Close NET_PRES_SocketClose

/*****************************************************************************
  Summary:
    Obtains information about a currently open socket.

  Description:
    This function calls the transport layer's SocketInfoGet, if it exists.

  Precondition:
    A socket needs to have been opened by NET_PRES_SKT_Open.

  Parameters:
    handle  - The presentation layer socket handle
    info    - The buffer that the information gets written to
    error   - The extended error code of the function

  Returns:
    - true  - Indicates success
    - false - Indicates failure

 */

#define NET_PRES_SKT_SocketInfoGet NET_PRES_SocketInfoGet

/*****************************************************************************
  Summary:
    Determines how much free space is available in the TX buffer.

  Description:
    This function calls the transport layer's WriteIsRead, if it exists.  A note on
    encrypted versus unencrypted sockets: This function only checks the transport layer
    to see how big the buffer is. Encrypted communications may take up more space
    per character than clear communications, so this function may not return the 
    exact number of characters you can actually write to the buffer if you are
    using an encrypted connection.

  Precondition:
    A socket needs to have been opened by NET_PRES_SKT_Open.

  Parameters:
    handle  - The presentation layer socket handle
    error   - The extended error code of the function

  Returns:
    The number of bytes available to be written in the TX buffer.
	
 */
#define NET_PRES_SKT_WriteIsReady NET_PRES_SocketWriteIsReady

/*****************************************************************************
  Summary:
    Takes a buffer and sends it to the encryption provider.
    
  Description:
    This function takes a buffer and sends it to the encryption provider for an 
    encrypted socket, or to the transport layer directly for an unencrypted
    socket.

  Precondition:
    A socket needs to have been opened by NET_PRES_SKT_Open.

  Parameters:
    handle    - The presentation layer socket handle
    buffer    - The pointer to the array to be written
    size      - The number of bytes to be written
    error     - The extended error code of the function

  Returns:
    The number of bytes written to the socket. If less than len, the
    buffer became full or the socket is not connected.

 */

#define NET_PRES_SKT_Write NET_PRES_SocketWrite

/*****************************************************************************
  Summary:
    Immediately transmits all pending TX data.

  Description:
    This function calls the transport layer's flush function, if it exists.

  Precondition:
    A socket needs to have been opened by NET_PRES_SKT_Open.

  Parameters:
    handle  - The presentation layer socket handle
    error   - The extended error code of the function

  Returns:
    - true  - Indicates success
    - false - Indicates failure

 */

#define NET_PRES_SKT_Flush NET_PRES_SocketFlush

/*****************************************************************************
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
    A socket needs to have been opened by NET_PRES_SKT_Open.

  Parameters:
    handle     - The presentation layer socket handle
    error      - The extended error code of the function

  Returns:
    The number of bytes available to be read from the TCP RX buffer.
	
  */
#define NET_PRES_SKT_ReadIsReady NET_PRES_SocketReadIsReady

/*****************************************************************************
  Summary:
    Reads an array of data byes from a socket's RX buffer/FIFO.

  Description:
    This function reads an array of data bytes from a socket's RX buffer/FIFO.  
	The data is removed from the FIFO in the process.  If the connection is encrypted
    this function calls the encryption provider's read function, otherwise it
    calls the transport layer's read function.

  Precondition:
    A socket needs to have been opened by NET_PRES_SKT_Open.

  Parameters:
    handle - The presentation layer socket handle
    buffer - The pointer to the array to store data that was read
    len    - The number of bytes to be read.
    error  - The extended error code of the function

  Returns:
    The number of bytes read from the socket.  If less than len, the
    RX FIFO buffer became empty or the socket is not connected.

  Remarks:
    If the supplied buffer is null, the data is simply discarded.

 */
#define NET_PRES_SKT_Read NET_PRES_SocketRead

/*****************************************************************************

  Summary:
    Reads a specified number of data bytes from the RX buffer/FIFO without
    removing them from the buffer.

  Description:
    If the socket is encrypted this function will call the encryption provider's
    peek function.  Otherwise this function calls the transport layer's peek
    function.

  Precondition:
    A socket needs to have been opened by NET_PRES_SKT_Open.

  Parameters:
    handle - The presentation layer socket handle
    buffer - Destination to write the peeked data bytes
    size   - Length of bytes to peek from the RX FIFO and copy to the buffer
    error  - The extended error code of the function
  

  Return Values:
    The number of bytes actually peeked from the stream and copied to the buffer.

  Remarks:
    None
 */

#define NET_PRES_SKT_Peek NET_PRES_SocketPeek

/*****************************************************************************
  Summary:
    Discards any pending data in the RX FIFO.
  
  Description:
    This function calls the transport layer's discard function, if it exists.

  Precondition:
    A socket needs to have been opened by NET_PRES_SKT_Open

  Parameters:
    handle   - The presentation layer socket handle
    error    - The extended error code of the function

  Returns:
    The number of bytes that have been discarded from the RX buffer.

 */
#define NET_PRES_SKT_Discard NET_PRES_SocketDiscard

// *****************************************************************************
/* 
  Summary:
    Registers a socket signal handler.

  Description:
    This function calls the transport layer's register signal handle function 
	directly, if it exists

  Precondition:
     A socket needs to have been opened by NET_PRES_SKT_Open

  Parameters:
    handle     - The presentation layer socket handle
    sigMask    - The mask of signals to be reported, this parameter is passed to 
	             the transport layer directly
    handler    - signal handler to be called when an event occurs.  This parameter 
	             is passed to the transport layer directly
    hParam     - Parameter to be used in the handler call.  This parameter is passed 
	             to the transport layer directly
    error      - The extended error code of the function

  Returns:
    - valid handle      - Indicates the call succeeded
	- null handle       - Indicates the call failed (null handler, no such socket, 
	                      existent handler)

 */

#define NET_PRES_SKT_SignalHandlerRegister NET_PRES_SocketSignalHandlerRegister


// *****************************************************************************
/* 
  Summary:
    Deregisters a previously registered socket signal handler.
    
  Description:
    This function calls the transport layer's deregister signal handler function,
	if it exists

  Precondition:
     A socket needs to have been opened by NET_PRES_SKT_Open.

  Parameters:
   handle    - The presentation layer socket handle
     hSig    - A handle returned by a previous call to TCPIP_TCP_SignalHandlerRegister
    error    - The extended error code of the function

  Returns:
    - true	- If the call succeeds
    - false - If no such handler is registered
 */

#define NET_PRES_SKT_SignalHandlerDeregister NET_PRES_SocketSignalHandlerDeregister

// *****************************************************************************
/* 
  Summary:
   This function checks if encryption negotiation is still in progress.

  Description:
    This function returns checks to see if an encrypted socket is still undergoing 
	negotiation. 
 
   Precondition:
     A socket needs to have been opened by NET_PRES_SKT_Open.

  Parameters:
   handle    - The presentation layer socket handle
    error    - The extended error code of the function

  Returns:
    - true	- if the encryption negotiation is still ongoing
    - false - if there is no negotiation ongoing

 */

#define NET_PRES_SKT_IsNegotiatingEncryption NET_PRES_SocketIsNegotiatingEncryption

// *****************************************************************************
/* 
  Summary:
    This function checks whether a connection is secure.

  Description:
    This function returns whether or not the connection is secure.  It will return
    true if encryption negotiation was successful .
 
   Precondition:
     A socket needs to have been opened by NET_PRES_SKT_Open.

  Parameters:
   handle   - The presentation layer socket handle
    error   - The extended error code of the function

  Returns:
    - true	- If the communications is secure
    - false - If the communications is not secure

 */
#define NET_PRES_SKT_IsSecure NET_PRES_SocketIsSecure

// *****************************************************************************
/* 
  Summary:
    This function turns an insecure socket into a secure socket.

  Details:
    This function will turn an unencrypted socket into an encrypted socket and starts
    encryption negotiation.
 
   Precondition:
     A socket needs to have been opened by NET_PRES_SKT_Open.

  Parameters:
     handle - The presentation layer socket handle
     error  - The extended error code of the function

  Returns:
    - true	- if the call was successful
    - false - if the call was unsuccessful

 */

#define NET_PRES_SKT_EncryptSocket NET_PRES_SocketEncryptSocket

#ifdef __cplusplus
}
#endif


#endif //_NET_PRES_SOCKET_API_
