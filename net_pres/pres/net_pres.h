/*******************************************************************************
MPLAB Harmony Networking Presentation Layer Header File

  Company:
    Microchip Technology Inc.
    
  Filename:
    net_pres.h
    
  Summary:
    Describes the system level interface and common definitions to the MPLAB 
    Harmony presentation layer.
    
  Description:
    This file describes the system interface and common definitions to the MPLAB 
    Harmony Networking Presentation Layer.

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

#ifndef H_NET_PRES_H_
#define H_NET_PRES_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "system_config.h"
#include "system/system_module.h"

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
extern "C" {
#endif

struct S_NET_PRES_TransportObject;
struct S_NET_PRES_EncProviderObject;
//DOM-IGNORE-END   

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Net Presentation Instance Initialization data

  Summary:
    Initializes a Presentation layer.

  Description:
    This data type initializes a Presentation layer.

  Remarks:
    None.
*/

typedef struct {
    // Pointer to the transport object that handles the stream server
    const struct S_NET_PRES_TransportObject * pTransObject_ss;  
    // Pointer to the transport object that handles the stream client
    const struct S_NET_PRES_TransportObject * pTransObject_sc;  
    // Pointer to the transport object that handles the datagram server
    const struct S_NET_PRES_TransportObject * pTransObject_ds;  
    // Pointer to the transport object that handles the datagram client
    const struct S_NET_PRES_TransportObject * pTransObject_dc;  
    // Pointer to the encryption provider object that handles the stream server
    const struct S_NET_PRES_EncProviderObject * pProvObject_ss;  
    // Pointer to the encryption provider object that handles the stream client
    const struct S_NET_PRES_EncProviderObject * pProvObject_sc;  
    // Pointer to the encryption provider object that handles the datagram server
    const struct S_NET_PRES_EncProviderObject * pProvObject_ds;  
    // Pointer to the encryption provider object that handles the datagram client
    const struct S_NET_PRES_EncProviderObject * pProvObject_dc;  
}NET_PRES_INST_DATA;

// *****************************************************************************
/* Net Presentation Initialization data

  Summary:
    Initializes a Presentation layer.

  Description:
    Data type that initializes a Presentation layer.

  Remarks:
    None.
*/

typedef struct {
    uint8_t numLayers;                     // Number of presentation layers
    const NET_PRES_INST_DATA  * pInitData; // Pointer to an array of pointers to 
                                           // presentation layer instance data.
}NET_PRES_INIT_DATA;

// *****************************************************************************
/* Net Presentation Index Type

  Summary:
    Sets the type for the presentation layer index.

  Description:
    This data type sets the type for the presentation layer index.

  Remarks:
    None.
*/
typedef uint8_t NET_PRES_INDEX;


// *****************************************************************************
/* Net Presentation Port Type

  Summary:
    Sets the type for the presentation layer port.

  Description:
    This data type sets the type for the presentation layer port.

  Remarks:
    None.
*/
typedef uint16_t NET_PRES_SKT_PORT_T;

// *****************************************************************************
/* Net Presentation Signal Handle Type

  Summary:
    Sets the type for the presentation layer signal handle.

  Description:
    This data type sets the type for the presentation layer signal handle.

  Remarks:
    None.
*/

typedef const void* NET_PRES_SIGNAL_HANDLE;

// *****************************************************************************
/* Net Presentation Callback Handle Type

  Summary:
    Type for the presentation layer callback handle.

  Description:
    This data type sets the type for the presentation layer callback handle.

  Remarks:
    None.
*/

typedef const void* NET_PRES_CBACK_HANDLE;

// *****************************************************************************
/* Net Presentation Socket Handle Type

  Summary:
    Sets the type for the presentation layer socket handle.

  Description:
    This data type sets the type for the presentation layer socket handle.

  Remarks:
    None.
*/

typedef int16_t NET_PRES_SKT_HANDLE_T;

// *****************************************************************************
/*
  Macro:
    NET_PRES_INVALID_SOCKET

  Summary:
    Invalid socket indicator macro.

  Description:
    Indicates that the socket is invalid or could not be opened.
*/
#define NET_PRES_INVALID_SOCKET (-1)

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - Callbacks
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Type:
    NET_PRES_SIGNAL_FUNCTION

  Summary:
    MPLAB Harmony Networking Presentation Layer Signal function.

  Description:
    Prototype of a signal handler. Socket user can register a handler for the
    socket. Once an event occurs the registered handler will be called.

  Parameters:
    handle      - The presentation socket to be used
    hNet        - The network interface on which the event has occurred
    sigType     - The type of signal that has occurred
    param       - An additional parameter that can has been specified at the handler 
                  registration call. Currently not used and it will be null.


  Remarks:
    The handler has to be short and fast. It is meant for setting an event flag, 
    not for lengthy processing!

 */

typedef void    (*NET_PRES_SIGNAL_FUNCTION)(NET_PRES_SKT_HANDLE_T handle, NET_PRES_SIGNAL_HANDLE hNet, 
                 uint16_t sigType, const void* param);

// *****************************************************************************
/*
  Type:
    NET_PRES_SNI_CALLBACK

  Summary:
    MPLAB Harmony Networking Presentation Layer SNI callback function.

  Description:
    Prototype of a SNI function needed for the TLS "Server Name Indication".
    NET_PRES user can register a function to be used when SNI is needed.
    The registered handler will be called at run time as needed
    to communicate the hostname of the site to the server.

  Parameters:
    handle      - The presentation socket that is in use


  Remarks:
    None

 */

typedef const char*    (*NET_PRES_SNI_CALLBACK)(NET_PRES_SKT_HANDLE_T handle);

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - System Level
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Network Presentation Layer Initialization

  Summary:
    Initializes the Network Presentation Layer sub-system with the configuration data.
    <p><b>Implementation:</b> Dynamic</p>
    
  Description:
    Initializes the Network Presentation Layer sub-system with the configuration data.
    
  Preconditions:
    None.

  Parameters:
    index   - This is the index of the network presentation layer instance to be initialized.  
              Since there is only one network presentation layer, this parameter is ignored.
    initData - This is a pointer to a NET_PRES_INIT_DATA structure
    
    Returns:
      - Valid handle to the presentation instance - If successful
      - SYS_MODULE_OBJ_INVALID                    - If unsuccessful 
*/

SYS_MODULE_OBJ NET_PRES_Initialize( const SYS_MODULE_INDEX index, const void* initData);

// *****************************************************************************
/* Network Presentation Layer Deinitialization

  Summary:
    Deinitializes the Network Presentation Layer Instance.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function deallocates any resources allocated by the initialization function.  

  Preconditions:
    The layer must be successfully initialized with NET_PRES_Initialize.
    
  Parameters:
    Object  - the valid object returned from NET_PRES_Initialize

  Returns:
    None.
*/

void NET_PRES_Deinitialize(SYS_MODULE_OBJ obj);

// *****************************************************************************
/* Network Presentation Layer Reinitialization

  Summary:
    Reinitializes the instance of the presentation layer.
    <p><b>Implementation:</b> Dynamic</p>
    
  Description:
    This function will deinitialize and initialize the layer instance. 

  Preconditions:
    The layer must be successfully initialized with NET_PRES_Initialize.

  Parameters:
    object  - The object valid passed back to NET_PRES_Initialize
    initData - The new initialization structure

    Returns:
    None.
      
    */

void NET_PRES_Reinitialize(SYS_MODULE_OBJ obj, const void* initData);

// *****************************************************************************
/* MPLAB Harmony Networking Presentation Layer Tasks

  Summary:
    MPLAB Harmony tasks function used for general presentation layer tasks.
    <p><b>Implementation:</b> Dynamic</p>
    
  Description:
    This function is called by the main loop.  It is used to pump encryption 
    connections during negotiations.

  Preconditions:
    The layer must be successfully initialized with NET_PRES_Initialize.

  Parameters:
    object  - The valid object passed back to NET_PRES_Initialize

  Returns:
    None.
      
    */

void NET_PRES_Tasks(SYS_MODULE_OBJ obj);

//**************************************************************************
/*

  Summary:
    Provides the current status of the MPLAB Harmony Networking Presentation 
    Layer.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function provides the current status of the MPLAB Harmony Net 
    Presentation Layer.

  Precondition:
    The NET_PRES_Initialize function must have been called before calling
    this function.

  Parameters:
    object -  Layer object handle, returned from NET_PRES_Initialize

  Returns:
    - SYS_STATUS_READY  - Indicates that any previous module operation for the
                          specified module has completed
    - SYS_STATUS_UNINITIALIZED   - Indicates the module has not been initialized
    - SYS_STATUS_BUSY   - Indicates that the module is busy and can't accept 
                          operations
    - SYS_STATUS_ERROR  - Indicates that there is a fatal error in the module

  Remarks:
    None.
*/

SYS_STATUS NET_PRES_Status ( SYS_MODULE_OBJ object );

// *****************************************************************************
/* 

  Summary:
    Returns the Network Presentation Layer object
    
  Description:
    Returns the Network Presentation Layer object corresponding to the selected index.
    
  Preconditions:
    The NET_PRES_Initialize function must have been called before calling
    this function.

  Parameters:
    index   - This is the index of the network presentation layer instance object
              Since there is only one network presentation layer, this parameter is ignored.
    
    Returns:
      - Valid handle to the presentation instance - If successful
      - SYS_MODULE_OBJ_INVALID                    - If unsuccessful 
*/

SYS_MODULE_OBJ NET_PRES_ModuleObjGet(const SYS_MODULE_INDEX index);

//**************************************************************************
/*

  Summary:
    Registers a SNI callback with the MPLAB Harmony Networking Presentation 
    Layer.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function registers a dynamic SNI callback with the MPLAB Harmony Net 
    Presentation Layer.

  Precondition:
    The NET_PRES_Initialize function must have been called before calling
    this function.

  Parameters:
    object - Layer object handle, returned from NET_PRES_Initialize
    cBack  - callback to be registered

  Returns:
    - valid handle  - Indicates the call succeeded
                      The handle could be used in a call to NET_PRES_SniCallbackDeregister 
                        
    - null handle   - Indicates the call failed (null callback, existent handler, etc.)

  Remarks:
    Currently only one registration is supported.
    The user must call NET_PRES_SniCallbackDeregister before registering a new callback 
*/
NET_PRES_CBACK_HANDLE NET_PRES_SniCallbackRegister(SYS_MODULE_OBJ obj, NET_PRES_SNI_CALLBACK cBack); 

//**************************************************************************
/*

  Summary:
    Deregisters a SNI callback with the MPLAB Harmony Networking Presentation 
    Layer.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function deregisters a previously registered dynamic SNI callback with the MPLAB Harmony Net 
    Presentation Layer.

  Precondition:
    The NET_PRES_Initialize function must have been called before calling
    this function.

  Parameters:
    object      - Layer object handle, returned from NET_PRES_Initialize
    cBackHandle - handle of the callback to be deregistered

  Returns:
    - true  - Indicates the call succeeded
    - false - Indicates the call failed (callback mismatch, non-existent handler, etc.)

  Remarks:
    Currently only one registration is supported.
    The user must call NET_PRES_SniCallbackDeregister before registering a new callback 
*/
bool NET_PRES_SniCallbackDeregister(SYS_MODULE_OBJ obj, NET_PRES_CBACK_HANDLE cBackHandle); 

//**************************************************************************
/*

  Summary:
    Gets a registered SNI callback with the MPLAB Harmony Networking Presentation 
    Layer.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function gets the currently registered SNI callback with the MPLAB Harmony Net 
    Presentation Layer.

  Precondition:
    The NET_PRES_Initialize function must have been called before calling
    this function.

  Parameters:
    object - Layer object handle, returned from NET_PRES_Initialize

  Returns:
    - valid handle  - Indicates the call succeeded
                        
    - null handle   - Indicates the call failed (null registered handle, etc.)

  Remarks:
    None
*/
NET_PRES_SNI_CALLBACK NET_PRES_SniCallbackGet(SYS_MODULE_OBJ obj);


#ifdef __cplusplus
}
#endif


#endif //H_NET_PRES_H_
