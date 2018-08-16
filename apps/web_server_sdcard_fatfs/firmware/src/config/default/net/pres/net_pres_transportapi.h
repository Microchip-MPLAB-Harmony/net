/*******************************************************************************
MPLAB Harmony Networking Presentation Layer Header File

  Company:
    Microchip Technology Inc.
    
  Filename:
    net_pres_transportapi.h
    
  Summary:
    API descriptions that the transport layers follow for the presentation layer.
    
  Description:
    This file describes the API that transport layers follow for to integrate
    with MPLAB Harmony's Networking Presentation Layer.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2015 released Microchip Technology Inc.  All rights reserved.

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
//DOM-IGNORE-END

#ifndef _NET_PRES_TRANSPORT_API_H_
#define _NET_PRES_TRANSPORT_API_H_

#include <stdint.h>
#include <stdbool.h>
#include "system_config.h"

#include "net_pres.h"
#include "net_pres_socketapi.h"

#ifdef __cplusplus
extern "C" {
#endif

// *****************************************************************************
/* MPLAB Harmony Networking Presentation Transport Address Structure

  Summary:
    Defines a generic address structure to pass to the transport layer.

  Description:
    This data type is just a generic address structure.  The presentation layer does
    not do any processing on this data, but instead passes it directly to the transport.

  Remarks:
    None.
*/
typedef struct {
    uint8_t addr[16];  // So far biggest for IPv6
} NET_PRES_TRANS_ADDR_T;

// *****************************************************************************
/* MPLAB Harmony Networking Presentation Layer Address type

  Summary:
    Defines the enumeration for the type of address.

  Description:
    Defines the enumeration for the type of address.  This enumeration is not used 
	directly by the presentation layer and is used to enforce a consistent interface 
	between layers.

  Remarks:
    None.
*/

typedef enum 
{
    NET_PRES_ADDRT_UNKNOWN,
} NET_PRES_TRANS_ADDRESS_TYPE;

// *****************************************************************************
/* MPLAB Harmony Networking Presentation Layer Option type

  Summary:
    Defines the enumeration for the type of options.

  Description:
    Defines the enumeration for the type of options.  This enumeration is not used
    directly by the presentation layer and is used to enforce a consistent interface
    between layers.

  Remarks:
    None.
*/

typedef enum
{
    NET_PRES_OPT_UNKNOWN,
} NET_PRES_TRANS_OPTION_T;


//*****************************************************************************
/*
 Transport Layer Open Function Pointer Prototype
 
  Summary:
    Opens a presentation socket.

  Description:
    This function is called by the presentation layer when an application wants
    to open a socket.

  Precondition:
 Transport layer must be initialized.

  Parameters:
    addType     - The type of address being used. This is passed unaltered to the 
	              transport layer.
    port        - The port to listen or to send to.  This is passed unaltered to 
	              the transport layer.
    address     - The address to use. This is passed unaltered to the transport layer.

  Returns:
    - NET_PRES_INVALID_SOCKET       - No sockets of the specified type were available to be
                                      opened.
    - NET_PRES_SKT_HANDLE_T handle  - Returned when NET_PRES_INVALID_SOCKET is returned. 
	                                  Save this handle and use it when calling all 
									  other presentation socket APIs.
 */
 
typedef NET_PRES_SKT_HANDLE_T (*NET_PRES_TransOpen)(NET_PRES_TRANS_ADDRESS_TYPE addType, 
                                NET_PRES_SKT_PORT_T port, NET_PRES_ADDRESS * address);
//******************************************************************************
/*
 Transport Layer Bind Function Pointer Prototype
 
  Summary:
    Binds a socket to a local address.

  Description:
   This function is called by the presentation layer when an application wants
   to bind a socket.

  Precondition:
    A socket needs to have been opened by NET_PRES_TransOpen.

  Parameters:
    handle  -   The handle returned from NET_PRES_TransOpen.
    addType -   The type of address being used. This is passed unaltered to the transport layer.
    port    -   The port to use. This is passed unaltered to the transport layer.
    address -   The address to bind to. This is passed unaltered to the transport layer.

  Returns:
    - true  - Indicates success
    - false - Indicates failure

 */

typedef bool (*NET_PRES_TransBind)(NET_PRES_SKT_HANDLE_T handle, NET_PRES_TRANS_ADDRESS_TYPE addType, 
               NET_PRES_SKT_PORT_T port, NET_PRES_ADDRESS * address);

//******************************************************************************
/*
 Transport Layer Option Function Pointer Prototype
 
  Summary:
    Sets of gets a socket's options.

  Description:
   This function is called by the presentation layer when an application wants
   to get the current socket options or set them.

  Precondition:
     A socket needs to have been opened by NET_PRES_TransOpen.

  Parameters:
    handle   - The handle returned from NET_PRES_TransOpen.
    option   - The option to set or get.
    optParam - The pointer to option specific information.

  Returns:
    - true  - Indicates success
    - false - Indicates failure

 */
 
typedef bool (*NET_PRES_TransOption)(NET_PRES_SKT_HANDLE_T handle, NET_PRES_TRANS_OPTION_T option, 
               void * optParam);

//******************************************************************************
/*
 Transport Layer Boolean Function Pointer Prototype
 
  Summary:
    Generic function prototype for functions that return a bool.

  Description:
   This function is called by the presentation layer when it accesses a function 
   that takes no parameters apart from the socket handle and returns a boolean.

  Precondition:
    A socket needs to have been opened by NET_PRES_TransOpen.

  Parameters:
    handle  - The handle returned from NET_PRES_TransOpen.

  Returns:
    The result is passed directly through from the transport layer to the 
    application.  The meaning of the return is dependent on the transport function

 */
typedef bool (*NET_PRES_TransBool)(NET_PRES_SKT_HANDLE_T handle);

//******************************************************************************
/*
 Transport Layer Close Function Pointer Prototype
 
  Summary:
    Function prototype for functions that closes a socket.

  Description:
   This function is called by the presentation layer when the application wants
   to close a connection.
 
  Precondition:
    A socket needs to have been opened by NET_PRES_TransOpen.

  Parameters:
    handle   - The handle returned from NET_PRES_TransOpen.

  Returns:
    None.

 */
typedef void (*NET_PRES_TransClose)(NET_PRES_SKT_HANDLE_T handle);

//******************************************************************************
/*
 Transport Layer Get Socket Info Function Pointer Prototype
 
  Summary:
    Function prototype for functions that gets the information on a socket.

  Description:
   This function is called by the presentation layer when the application wants
   to get information on a socket.
 
  Precondition:
    A socket needs to have been opened by NET_PRES_TransOpen.

  Parameters:
    handle - The handle returned from NET_PRES_TransOpen.
    info   - The socket information.

  Returns:
    - true  - Indicates success
    - false - Indicates failure

 */
typedef bool (*NET_PRES_TransSocketInfoGet)(NET_PRES_SKT_HANDLE_T handle, void * info);

//******************************************************************************
/*
 Transport Layer Peek Function Pointer Prototype
 
  Summary:
    Function prototype for functions that peeks on the socket's buffer.

  Description:
   This function is called by the presentation layer when the application wants
   to peek into the buffer of an unencrypted socket.
 
  Precondition:
    A socket needs to have been opened by NET_PRES_TransOpen.

  Parameters:
    handle   - The handle returned from NET_PRES_TransOpen.
    vBuffer  - The buffer location to put the information.
    wLen     - The size of the buffer.
    wStart   - Where to start peeking into the buffer. This parameter is not used 
	           and will always be set to '0'.

  Returns:
    - true  - Indicates success
    - false - Indicates failure

 */

typedef bool (*NET_PRES_TransPeek)(NET_PRES_SKT_HANDLE_T handle, uint8_t *vBuffer, 
               uint16_t wLen, uint16_t wStart);

//******************************************************************************
/*
 Transport Layer Discard Function Pointer Prototype
 
  Summary:
    Function prototype for functions that clears a socket's RX buffer.

  Description:
    This function is called by the presentation layer when the application wants
    to discard the RX buffer in a socket.
 
  Precondition:
    A socket needs to have been opened by NET_PRES_TransOpen.

  Parameters:
    handle   - The handle returned from NET_PRES_TransOpen.
 
  Returns:
    The number of bytes discarded.

 */
typedef uint16_t (*NET_PRES_TransDiscard)(NET_PRES_SKT_HANDLE_T handle);

//******************************************************************************
/*
 Transport Layer Register Handler Function Pointer Prototype
 
  Summary:
    Function prototype that registers a handler with a socket.

  Description:
    This function is called by the presentation layer when the application wants
    to register a handler function.
 
  Precondition:
    A socket needs to have been opened by NET_PRES_TransOpen.

  Parameters:
    handle    -   The handle returned from NET_PRES_TransOpen.
    sigMask   -   The event mask.
    handler   -   The event handler function.
    hParam    -   Parameters passed to the handler function.
 
  Returns:
    The handle of a signal handler.

 */
 
typedef NET_PRES_SIGNAL_HANDLE (*NET_PRES_TransHandlerRegister)(NET_PRES_SKT_HANDLE_T handle, 
                       uint16_t sigMask, NET_PRES_SIGNAL_FUNCTION handler, const void* hParam);

//******************************************************************************
/*
 Transport Layer Deregister Handler Function Pointer Prototype
 
  Summary:
    Function prototype that deregisters a handler with a socket.

  Description:
   This function is called by the presentation layer when the application wants
   to deregister a handler function.
 
  Precondition:
   A socket needs to have been opened by NET_PRES_TransOpen.

  Parameters:
    handle  - The handle returned from NET_PRES_TransOpen.
    hSig    - The handler handle returned from NET_PRES_TransHandlerRegister.
 
  Returns:
    - true  - Indicates success
    - false - Indicates failure

 */

typedef bool (*NET_PRES_TransSignalHandlerDeregister)(NET_PRES_SKT_HANDLE_T handle, 
               NET_PRES_SIGNAL_FUNCTION hSig);

// *****************************************************************************
/* Presentation Layer Transport Layer Read Function Pointer Prototype

  Summary:
    Defines the read function provided by the transport layer.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function prototype is used to define the function that the Networking 
    Presentation Layer will pass to the provider when it is initialized.  The 
	provider will use this function when it needs to read from the transport layer.

  Preconditions:
    None.

  Parameters:
    transHandle	- This is the transport layer handle provided by the transport layer when
                  a communications channel is open.
    buffer	    - This is a pointer to the buffer that the transport layer will copy data to.
    count       - This is the size of the buffer.

  Returns:
    The number of data bytes copied by the transport channel into the buffer.
	
*/

typedef uint16_t (*NET_PRES_TransRead)(uintptr_t transHandle, uint8_t* buffer, uint16_t count);

// *****************************************************************************
/* Presentation Layer Transport Layer Write Function Pointer Prototype

  Summary:
    Defines the write function provided by the transport layer.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function prototype is used to define the function that the Networking 
    Presentation Layer will pass to the provider when it is initialized. The 
	provider will use this function when it needs to write to the transport layer.

  Preconditions:
    None.

  Parameters:
    transHandle	- This is the transport layer handle provided by the transport layer when
                  a communications channel is open.
    buffer	    - This is a pointer to the buffer contains the data to be passed to 
	              the transport layer.
    count       - This is the size of the buffer.

  Returns:
    The number of data bytes accepted by the transport layer.
	
*/

typedef uint16_t (*NET_PRES_TransWrite)(uintptr_t transHandle, const uint8_t* buffer, uint16_t count);

// *****************************************************************************
/* Presentation Layer  Transport Layer Ready Function

  Summary:
    Defines the ready function provided by the transport layer.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function prototype is used to define the function that the Networking 
	Presentation Layer will pass to the provider when it is initialized.  The 
	provider will use this function when it needs to check if it can read or write 
	to the layer.

  Preconditions:
    None.

  Parameters:
    transHandle	- This is the transport layer handle provided by the transport layer when
                  a communications channel is open.

  Returns:
    - true  - The presentation layer can read or write to the transport layer
    - false - The transport layer is busy and cannot accept reads or write
	
*/

typedef uint16_t (*NET_PRES_TransReady)(uintptr_t transHandle);


// *****************************************************************************
/* Presentation Layer  Transport Layer Is Port Encrypted

  Summary:
    Checks to see if a port is encrypted by default.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function prototype is used by the presentation layer to determine if 
    a port is encrypted by default or not when it is opened.

  Preconditions:
    None.

  Parameters:
    - port     - The port number to be checked.

  Returns:
    - true  - The port is encrypted by default and the presentation layer will
              start negotiating encryption when it is connected.
    - false - The post is not encrypted by default.
	
*/


typedef bool (*NET_PRES_TransIsPortDefaultSecured)(uint16_t port);

// *****************************************************************************
/* MPLAB Harmony Networking Presentation Transport Information Structure

  Summary:
    Defines the data that the transport layer needs to provide to the Networking
    Presentation Layer.

  Description:
    This data type defines the data required by the transport layer to effectively
    work with the Networking Presentation Layer.  The data is there to allow the 
	Networking Presentation Layer to configure the provider to effectively use 
	the transport layer.

  Remarks:
    None.
*/

typedef struct _NET_PRES_TransportObject{
    NET_PRES_TransOpen fpOpen;           // Function pointer to the transport's open call
    NET_PRES_TransBind fpLocalBind;      // Function pointer to the transport's bind call
    NET_PRES_TransBind fpRemoteBind;     // Function pointer to the transport's remote bind call
    NET_PRES_TransOption fpOptionGet;    // Function call to the the transport's option get call
    NET_PRES_TransOption fpOptionSet;    // Function call to the the transport's option set call
    NET_PRES_TransBool fpIsConnected;    // Function call to the the transport's is connected call
    NET_PRES_TransBool fpWasReset;       // Function call to the the transport's was reset call
    NET_PRES_TransBool fpDisconnect;     // Function call to the the transport's disconnect call
    NET_PRES_TransBool fpConnect;        // Function call to the the transport's connect call
    NET_PRES_TransClose fpClose;         // Function call to the the transport's close call
    NET_PRES_TransSocketInfoGet fpSocketInfoGet; // Function call to the the transport's get socket info call
    NET_PRES_TransBool fpFlush;          // Function call to the the transport's flush call
    NET_PRES_TransPeek fpPeek;           // Function call to the the transport's peek call
    NET_PRES_TransDiscard fpDiscard;     // Function call to the the transport's discard call
    NET_PRES_TransHandlerRegister fpHandlerRegister;  // Function call to the the transport's register handler call
    NET_PRES_TransSignalHandlerDeregister fpHandlerDeregister; // Function call to the the transport's deregister handler call
    
    /* Function pointer to call when doing a read from a transport layer*/
    NET_PRES_TransRead fpRead;
    /* Function pointer to call when doing a write to a transport layer*/
    NET_PRES_TransWrite fpWrite;
    /* Function pointer to call when checking to see if there is data available to be read from a transport layer*/
    NET_PRES_TransReady fpReadyToRead;
    /* Function pointer to call when checking to see if there is space available to be write to a transport layer*/
    NET_PRES_TransReady fpReadyToWrite;

    /* Function pointer to call when checking to see if a port is secure by default*/
    NET_PRES_TransIsPortDefaultSecured fpIsPortDefaultSecure;
    
} NET_PRES_TransportObject;

#ifdef __cplusplus
}
#endif

#endif