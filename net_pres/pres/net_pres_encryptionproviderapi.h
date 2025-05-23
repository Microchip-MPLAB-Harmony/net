/*******************************************************************************
MPLAB Harmony Networking Presentation Layer Encryption Provider Header File

  Company:
    Microchip Technology Inc.
    
  Filename:
    net_pres_encryptionproviderapi.h
    
  Summary:
    API descriptions that encryption providers follow for the presentation layer.
    
  Description:
    This file describes the API that encryption providers follow for the Networking
    Presentation Layer.

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2015-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef H_NET_PRES_ENCRYPTION_PROVIDER_API_H_
#define H_NET_PRES_ENCRYPTION_PROVIDER_API_H_

#include <stdint.h>
#include <stdbool.h>
#include "system_config.h"

#include "net_pres.h"

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
extern "C" {
#endif

struct S_NET_PRES_TransportObject;
//DOM-IGNORE-END
// *****************************************************************************
/* MPLAB Harmony Networking Presentation Layer Encryption status type

  Summary:
    Defines the enumeration for the state and status of the encrypted portion 
    of a connection.

  Description:
    This enumeration defines the enumeration for the state and status of the 
    encrypted portion of a connection.

  Remarks:
    None.
*/

typedef enum
{
    NET_PRES_ENC_SS_UNKNOWN,                      // Presentation encryption is in 
                                                  // an unknown/default state
    NET_PRES_ENC_SS_WAITING_TO_START_NEGOTIATION, // Presentation encryption has not 
                                                  // started negotiation
    NET_PRES_ENC_SS_CLIENT_NEGOTIATING,           // Presentation encryption client 
                                                  // negotiation is in progress
    NET_PRES_ENC_SS_SERVER_NEGOTIATING,           // Presentation encryption server 
                                                  // negotiation is in progress
    NET_PRES_ENC_SS_OPEN,                         // Presentation encryption negotiation 
                                                  // is complete and data can be sent/received
    NET_PRES_ENC_SS_FAILED,                       // Presentation encryption negotiation failed 
                                                  // or some other failure
    NET_PRES_ENC_SS_CLOSING,                      // Presentation encryption is closing, but
                                                  // connection needs to be pumped for final packets
    NET_PRES_ENC_SS_CLOSED                        // Presentation encryption is closed, provider 
                                                  // data has been freed
}NET_PRES_EncSessionStatus;

// *****************************************************************************
/* Presentation Encryption Provider Initialization Function Pointer Prototype

  Summary:
    Defines the initialization function to the encryption provider.
      <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function pointer prototype defines the initialization function to the 
    encryption provider.

  Preconditions:
    None.

  Parameters:
    transObject - This is a copy of the structure the transport layer provides
                  to the presentation layer to read and write data.

  Returns:
    - true  - Initialization succeeded
    - false - Initialization did not succeed

 */
typedef bool (*Net_ProvInit)(struct S_NET_PRES_TransportObject * transObject);

// *****************************************************************************
/* Presentation Encryption Provider Close Function Pointer Prototype

  Summary:
   Defines the deinitialization function for the provider.
   <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function pointer prototype defines the deinitialization function for the 
    provider.

  Preconditions:
    None.

  Parameters:
    None.

  Returns:
    - true  - Deinitialization succeeded
    - false - Deinitialization did not succeed

 */
typedef bool (*Net_ProvDeinit)( void );


// *****************************************************************************
/* Presentation Encryption Provider Open Connection Prototype

  Summary:
    Defines the open connection function to the provider.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function pointer prototype defines the open connection function to the 
    provider.

  Preconditions:
    None.

  Parameters:
   obj          - The NET_PRES object handle, returned from NET_PRES_Initialize
   presHandle   - The NET_PRES handle used for this client.
   transHandle  - The handle from the transport layer to use for this client.
   providerData - A pointer to the buffer for the provider to keep connection
                  specific data.

  Returns:
    - true  - Create succeeded
    - false - Create did not succeed

 */
typedef bool (*Net_ProvOpen)(SYS_MODULE_OBJ obj, uintptr_t presHandle, uintptr_t transHandle, void * providerData);

// *****************************************************************************
/* Presentation Encryption Provider Connect Prototype

  Summary:
    Connects the function to the provider.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function is used by the presentation layer to pump the encryption negotiation.
    While negotiation is ongoing, the presentation layer's task function will continue to 
    call the function until negotiation ends.
 
  Preconditions:
    A connection must have already been created.

  Parameters:
    providerData - A pointer to the buffer that keeps the providerData returned from 
                   the Open call.

    Returns:
    - NET_PRES_ENC_SS_CLIENT_NEGOTIATING - Client is still negotiating the connection
    - NET_PRES_ENC_SS_SERVER_NEGOTIATING - Server is still negotiating the connection
    - NET_PRES_ENC_SS_OPEN               - Negotiation is complete and data can be 
                                           securely transmitted
    - NET_PRES_ENC_SS_FAILED             - Negotiation failed

 */
typedef NET_PRES_EncSessionStatus (*Net_ProvConnect)(void * providerData); 
        // called to pump the negotiation

// *****************************************************************************
/* Presentation Encryption Provider Close Function Pointer Prototype

  Summary:
    Defines the close function to the provider.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function pointer defines the close function.  It is called by the 
    Networking Presentation Layer after a connection has been closed by the 
    client.

  Preconditions:
    A connection must have already been created.

  Parameters:
    providerData - A pointer to the buffer for the provider to keep connection
                   specific data.

  Returns:
    - NET_PRES_ENC_SS_CLOSING - Connection is closing, function must be called again to 
                                pump the close
    - NET_PRES_ENC_SS_CLOSED  - The connection is closed and can be cleaned up

 */
typedef NET_PRES_EncSessionStatus (*Net_ProvConnectionClose)(void * providerData); 

// *****************************************************************************
/* Presentation Encryption Provider Write Function Pointer Prototype

  Summary:
    Defines the write function to the provider.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function pointer defines the write function.  It is called by the 
    presentation layer when the application wants to write to a secured connection.

  Preconditions:
    A connection must have already been created, and be in the open state.

  Parameters:
    providerData - A pointer to the buffer for the provider to keep connection
                   specific data.
    buffer       - This is a pointer to the buffer that will be sent to the provider.
    size         - This is the size of the buffer.


  Returns:
    The number of bytes transferred.

 */
typedef int32_t (*Net_ProvWrite)(void * providerData, const uint8_t * buffer, uint16_t size);   

// *****************************************************************************
/* Presentation Encryption Provider Write Ready Function Pointer Prototype

  Summary:
    Defines the write ready function to the provider.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function pointer defines the write ready function. It is called by the 
    presentation layer when the application wants to check the write space
    to a secured connection.
    The function checks for the requested size.
    If this is not available, it checks for at least minimum size (if != 0)

  Preconditions:
    A connection must have already been created, and be in the open state.

  Parameters:
    providerData - A pointer to the buffer for the provider to keep connection
                   specific data.
    reqSize      - The requested size to check for.
    minSize      - Minimum size to check for. Could be 0, if not used.


  Returns:
    The number of bytes available in the output buffer:
    - >= reqSize, if the requested space is available in the output buffer
    - >= minSize, if there's at least this minimum space (minSize != 0)
    - 0, requested (minimum) space cannot be granted

 */
typedef uint16_t (*Net_ProvWriteReady)(void * providerData, uint16_t reqSize, uint16_t minSize);   

// *****************************************************************************
/* Presentation Encryption Provider Read Function Pointer Prototype

  Summary:
   Defines the read function to the provider
   <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function pointer defines the read function.  It is called by the 
    presentation layer when the presentation client wants to read from a secured 
    connection.

  Preconditions:
    A connection must have already been created, and be in the open state.

  Parameters:
    providerData - A pointer to the buffer for the provider to keep connection
                   specific data.
    buffer       - A pointer to the buffer that will be read from the provider.
    count        - Size of the buffer.


  Returns:
    The number of bytes transferred.

  Remarks:
    If the supplied buffer is NULL the operation is ignored.

 */
typedef int32_t (*Net_ProvRead)(void * providerData, uint8_t * buffer, uint16_t size);   

// *****************************************************************************
/* Presentation Encryption Provider Read Ready Function Pointer Prototype

  Summary:
   Defines the read ready function to the provider
   <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function pointer defines the read ready function.  It is called by the 
    presentation layer when the presentation client wants to check whether 
    read data is available from a secured connection.

  Preconditions:
    A connection must have already been created, and be in the open state.

  Parameters:
    providerData - A pointer to the buffer for the provider to keep connection
                   specific data.


  Returns:
    The number of bytes ready to be read.

 */
typedef int32_t (*Net_ProvReadReady)(void * providerData);   

// *****************************************************************************
/* Presentation Encryption Provider Is Initialized Pointer Prototype

  Summary:
    Determines whether the encryption provider has been initialized.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function pointer determines whether the encryption provider has been 
    initialized and informs the presentation layer.

  Preconditions:
    A connection must have already been created, and be in the open state.

  Parameters:
    None.

  Returns:
    - true  - The provider has been initialized
    - false - The provider has not been initialized

 */
typedef bool (*Net_ProvIsInitialized)( void );

// *****************************************************************************
/* Presentation Encryption Provider Output Size Function Pointer Prototype

  Summary:
    Defines the output size function to the provider.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function pointer defines the output size function. It is called by the 
    presentation layer when the application wants to check how many bytes will
    be sent across the Transport layer, given a specified plaintext input size. 
    

  Preconditions:
    A connection must have already been created, and be in the open state.
    This function must be called after the SSL/TLS handshake has been completed. 

  Parameters:
    providerData - A pointer to the buffer for the provider to keep connection
                   specific data.
    inSize       - The requested plain text size to check for.


  Returns:
    Upon success, the requested size will be returned. 
    Upon error, 0 is returned:
            - if the input size is greater than the maximum TLS fragment size 
            - invalid function argument
            - if the SSL/TLS handshake has not been completed yet 
 */
typedef int32_t (*Net_ProvOutputSize)(void * providerData, int32_t inSize);

// *****************************************************************************
/* Presentation Encryption Provider Maximum Output Size Function Pointer Prototype

  Summary:
    Defines the maximum output size function to the provider.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function pointer defines the maximum output size function.
    Returns the maximum record layer size for plaintext data.
    This will correspond to either the maximum SSL/TLS record size as specified by the protocol standard,
    the maximum TLS fragment size as set by the TLS Max Fragment Length extension.
      
    

  Preconditions:
    A connection must have already been created, and be in the open state.
    This function must be called after the SSL/TLS handshake has been completed. 

  Parameters:
    providerData - A pointer to the buffer for the provider to keep connection
                   specific data.

  Returns:
    Upon success, the maximum output size will be returned.
    Upon error, 0 is returned:
            - invalid function argument
            - if the SSL/TLS handshake has not been completed yet 
 */
typedef int32_t (*Net_ProvMaxOutputSize)(void * providerData);

// *****************************************************************************
/* Presentation Encryption Provider Information Structure

  Summary:
    Defines the data that the presentation layer needs from the provider.

  Description:
    This data type is given to the presentation layer during initialization to 
    provide information on the provider, so it can be used during secure 
    communications.

  Remarks:
    None.
*/
typedef struct S_NET_PRES_EncProviderObject
{
    Net_ProvInit fpInit;               // Function pointer to open/initialize 
                                                   // the provider
    Net_ProvDeinit fpDeinit;           // Function pointer to close/deinitialize 
                                                   // the provider
    Net_ProvOpen fpOpen;               // Function pointer to create a stream client 
                                                   // connection
    Net_ProvConnect fpConnect;         // Function pointer to connect and pump the 
                                                   // negotiation of a stream client connection
    Net_ProvConnectionClose fpClose;   // Function Pointer to close and clean 
                                                   // up a connection
    Net_ProvWrite fpWrite;             // Function Pointer to write data to a connection
    Net_ProvWriteReady fpWriteReady;   // Function Pointer to check the connection write space
    Net_ProvRead fpRead;               // Function pointer to read data from a connection
    Net_ProvReadReady fpReadReady;     // Function pointer to return the available read data from a connection
    Net_ProvRead fpPeek;               // Function pointer to peek at data from a connection
    Net_ProvIsInitialized fpIsInited;  // Function pointer to check to determine if
                                                   // the provider has been initialized
    Net_ProvOutputSize fpOutputSize;   // Function pointer to get the output size
    Net_ProvMaxOutputSize fpMaxOutputSize; // Function pointer to get the maximum output size
}Net_ProvObject;

#ifdef __cplusplus
}
#endif

#endif // H_NET_PRES_ENCRYPTION_PROVIDER_API_H_

