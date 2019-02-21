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

#ifndef _NET_PRES_H_
#define _NET_PRES_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "system_config.h"
#include "system/system_module.h"

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
extern "C" {
#endif

struct _NET_PRES_TransportObject;
struct _NET_PRES_EncProviderObject;
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
    const struct _NET_PRES_TransportObject * pTransObject_ss;  
	// Pointer to the transport object that handles the stream client
    const struct _NET_PRES_TransportObject * pTransObject_sc;  
	// Pointer to the transport object that handles the datagram server
    const struct _NET_PRES_TransportObject * pTransObject_ds;  
	// Pointer to the transport object that handles the datagram client
    const struct _NET_PRES_TransportObject * pTransObject_dc;  
	// Pointer to the encryption provider object that handles the stream server
    const struct _NET_PRES_EncProviderObject * pProvObject_ss;  
	// Pointer to the encryption provider object that handles the stream client
    const struct _NET_PRES_EncProviderObject * pProvObject_sc;  
	// Pointer to the encryption provider object that handles the datagram server
    const struct _NET_PRES_EncProviderObject * pProvObject_ds;  
	// Pointer to the encryption provider object that handles the datagram client
    const struct _NET_PRES_EncProviderObject * pProvObject_dc;  
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
    index	- This is the index of the network presentation layer instance to be initialized.  
              Since there is only one network presentation layer, this parameter is ignored.
    init	- This is a pointer to a NET_PRES_INIT_DATA structure
    
    Returns:
      - Valid handle to the presentation instance - If successful
      - SYS_MODULE_OBJ_INVALID					  - If unsuccessful 
*/

SYS_MODULE_OBJ NET_PRES_Initialize( const SYS_MODULE_INDEX index,
                                    const SYS_MODULE_INIT * const init );

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
    Object	- the valid object returned from NET_PRES_Initialize

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
    object	- The object valid passed back to NET_PRES_Initialize
    init	    - The new initialization structure

    Returns:
    None.
	  
    */

void NET_PRES_Reinitialize(SYS_MODULE_OBJ obj, const SYS_MODULE_INIT * const init);

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
    object	- The valid object passed back to NET_PRES_Initialize

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


#ifdef __cplusplus
}
#endif


#endif //_NET_PRES_H_
