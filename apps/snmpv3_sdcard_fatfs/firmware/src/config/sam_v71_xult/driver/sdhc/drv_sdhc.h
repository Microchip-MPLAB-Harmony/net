/*******************************************************************************
  SD Host Controller Driver Interface

  Company:
    Microchip Technology Inc.

  File Name:
    drv_sdhc.h

  Summary:
    SD Host Controller Driver Interface File

  Description:
    The SD Host Controller driver provides a simple interface to manage the SD
    Host Controller peripheral.  This file defines the interface definitions
    and prototypes for the SD Host Controller driver.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
//DOM-IGNORE-END

#ifndef _DRV_SDHC_H
#define _DRV_SDHC_H

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "system/system.h"
#include "driver/driver_common.h"
#include "system/int/sys_int.h"
#include "system/system_media.h"
#include "system/time/sys_time.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************


// *****************************************************************************
/*
  Summary:
    Handle identifying commands queued in the driver.

  Description:
    A command handle is returned by a call to the Read or Write
    functions. This handle allows the application to track the completion
    of the operation. This command handle is also returned to the client
    along with the event that has occurred with respect to the command.
    This allows the application to connect the event to a specific
    command in case where multiple commands are queued.

    The command handle associated with the command request expires when the
    client has been notified of the completion of the command (after event
    handler function that notifies the client returns) or after the command
    has been retired by the driver if no event handler callback was set.

  Remarks:
    None.
*/
typedef SYS_MEDIA_BLOCK_COMMAND_HANDLE DRV_SDHC_COMMAND_HANDLE;


// *****************************************************************************
/*
  Summary:
    SDHC Driver's Invalid Command Handle.

  Description:
    This value defines the SDHC Driver Invalid Command Handle. This value
    is returned by read or write routines when the command request was not
    accepted.

  Remarks:
    None.
*/

#define DRV_SDHC_COMMAND_HANDLE_INVALID SYS_MEDIA_BLOCK_COMMAND_HANDLE_INVALID

// *****************************************************************************
/*
   Summary
    Identifies the possible events that can result from a request.

   Description
    This enumeration identifies the possible events that can result from a
    read or a write request issued by the client.

   Remarks:
    One of these values is passed in the "event" parameter of the event
    handling callback function that client registered with the driver by
    calling the DRV_SDHC_EventHandlerSet function when a request is completed.
*/

typedef enum
{
    /* Operation has been completed successfully. */
    DRV_SDHC_EVENT_COMMAND_COMPLETE = SYS_MEDIA_EVENT_BLOCK_COMMAND_COMPLETE,

    /* There was an error during the operation */
    DRV_SDHC_EVENT_COMMAND_ERROR = SYS_MEDIA_EVENT_BLOCK_COMMAND_ERROR

} DRV_SDHC_EVENT;


// *****************************************************************************
/*
   Summary
    SDHC Driver Command Events

   Description
    This enumeration identifies the possible events that can result from a
    read or a write request made by the client.

   Remarks:
    One of these values is passed in the "event" parameter of the event
    handling callback function that client registered with the driver by
    calling the DRV_SDHC_EventHandlerSet function when a request is completed.
*/
typedef enum
{
    /*Done OK and ready */
    DRV_SDHC_COMMAND_COMPLETED          = SYS_MEDIA_COMMAND_COMPLETED,

    /*Scheduled but not started */
    DRV_SDHC_COMMAND_QUEUED             = SYS_MEDIA_COMMAND_QUEUED,

    /*Currently being in transfer */
    DRV_SDHC_COMMAND_IN_PROGRESS        = SYS_MEDIA_COMMAND_IN_PROGRESS,

    /*Unknown Command */
    DRV_SDHC_COMMAND_ERROR_UNKNOWN      = SYS_MEDIA_COMMAND_UNKNOWN,

} DRV_SDHC_COMMAND_STATUS;

// *****************************************************************************
/*

  Summary:
    SDHC Driver Module Index Numbers

  Description:
    These constants provide SDHC driver index definitions.

  Remarks:
    These values should be passed into the DRV_SDHC_Initialize and
    DRV_SDHC_Open routines to identify the driver instance in use.
*/

#define DRV_SDHC_INDEX_0         (0)


// *****************************************************************************
/*
  Summary:
    SDHC Driver Module Index Count

  Description:
    This constant identifies number of valid SDHC driver indices.

  Remarks:
    This value is derived from part-specific header files defined as part of the
    peripheral libraries.
*/

#define DRV_SDHC_INDEX_COUNT                     (1)

// *****************************************************************************
/*
  Summary:
     SDHC Driver Maximum allowed limit

  Description:
    This constant identifies number of valid SD Card driver indices.

  Remarks:
    This value is derived from part-specific header files defined as part of the
    peripheral libraries.
*/

#define SDHC_MAX_LIMIT                                                (2)


// *****************************************************************************
/*
  Summary:
    SD Card Detection Logic

  Description:
    This enum defines Card detection logic levels.

  Remarks:
    None.
*/

typedef enum
{
    /* The media event is SD Card attach */
    SDHC_DETECTION_LOGIC_ACTIVE_LOW,

    /* The media event is SD Card detach */
    SDHC_DETECTION_LOGIC_ACTIVE_HIGH,

}SDHC_DETECTION_LOGIC;

// *****************************************************************************
/*
  Summary:
    SDHC Bus Width

  Description:
    This enum defines SDHC Transfer bus widths.

  Remarks:
    None.
*/

typedef enum
{
    DRV_SDHC_BUS_WIDTH_1_BIT = 0,
    DRV_SDHC_BUS_WIDTH_4_BIT,
} DRV_SDHC_BUS_WIDTH;

// *****************************************************************************
/*
  Summary:
    SDHC Bus Speed

  Description:
    This enum defines SDHC bus Speeds Supported.

    - Standard SD-Cards only support Default Speed.
    - SD-Cards with High Capacity support Both Default and High Speed

  Remarks:
    None.
*/

typedef enum
{
    DRV_SDHC_SPEED_MODE_DEFAULT = 0,
    DRV_SDHC_SPEED_MODE_HIGH
} DRV_SDHC_SPEED_MODE;

// *****************************************************************************
/*
  Summary:
    SD Host Controller Driver Initialization Data

  Description:
    This structure contains all the data necessary to initialize the SD Card
    device.

  Remarks:
    A pointer to a structure of this format containing the desired
    initialization data must be passed into the DRV_SDHC_Initialize routine.
*/

typedef struct
{
    bool sdCardDetectEnable;

    bool sdWriteProtectEnable;

    DRV_SDHC_SPEED_MODE speedMode;

    DRV_SDHC_BUS_WIDTH busWidth;

    bool isFsEnabled;

} DRV_SDHC_INIT;


// *****************************************************************************
/*
   Summary
    SDHC Driver Event Handler Function Pointer

   Description
    This data type defines the required function signature for the SDHC event
    handling callback function. A client must register a pointer to an event
    handling function whose function signature (parameter and return value
    types) match the types specified by this function pointer in order to
    receive event calls back from the driver.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    event           - Identifies the type of event

    commandHandle   - Handle returned from the Read/Write requests

    context         - Value identifying the context of the application that
                      registered the event handling function

  Returns:
    None.

  Example:
    <code>
    void APP_MySDHCEventHandler
    (
        DRV_SDHC_EVENT event,
        DRV_SDHC_COMMAND_HANDLE commandHandle,
        uintptr_t context
    )
    {
        MY_APP_DATA_STRUCT pAppData = (MY_APP_DATA_STRUCT) context;

        switch(event)
        {
            case DRV_SDHC_EVENT_COMMAND_COMPLETE:

                // Handle the completed buffer.
                break;

            case DRV_SDHC_EVENT_COMMAND_ERROR:
            default:

                // Handle error.
                break;
        }
    }
    </code>

  Remarks:
    If the event is DRV_SDHC_EVENT_COMMAND_COMPLETE, it means that the
    write or a erase operation was completed successfully.

    If the event is DRV_SDHC_EVENT_COMMAND_ERROR, it means that the scheduled
    operation was not completed successfully.

    The context parameter contains the handle to the client context, provided
    at the time the event handling function was  registered using the
    DRV_SDHC_EventHandlerSet function. This context handle value is
    passed back to the client as the "context" parameter.  It can be any value
    necessary to identify the client context or instance (such as a pointer to
    the client's data) instance of the client that made the read/write/erase
    request.
*/
typedef SYS_MEDIA_EVENT_HANDLER DRV_SDHC_EVENT_HANDLER;


// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - System Level
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_SDHC_Initialize
     (
         const SYS_MODULE_INDEX index,
         const SYS_MODULE_INIT  * const init
     );

  Summary:
    Initializes the SD Card driver.

  Description:
    This routine initializes the SD Card driver, making it ready for clients to
    open and use the driver.

  Precondition:
    None.

  Parameters:
    drvIndex        - Index for the driver instance to be initialized

    init            - Pointer to a data structure containing any data necessary
                      to initialize the driver. This pointer may be null if no
                      data is required because static overrides have been
                      provided.

  Returns:
    If successful, returns a valid handle to a driver object. Otherwise, it
    returns SYS_MODULE_OBJ_INVALID.

  Example:
    <code>
    DRV_SDHC_INIT     init;
    SYS_MODULE_OBJ      objectHandle;

    // Populate the SD Card initialization structure

    objectHandle = DRV_SDHC_Initialize(DRV_SDHC_INDEX_0, (SYS_MODULE_INIT*)&init);
    if (SYS_MODULE_OBJ_INVALID == objectHandle)
    {
        // Handle error
    }
    </code>

  Remarks:
    This routine must be called before any other SDHC routine is called.

    This routine should only be called once during system initialization
    unless DRV_SDHC_Deinitialize is called to deinitialize the driver instance.

    This routine will NEVER block for hardware access. If the operation requires
    time to allow the hardware to reinitialize, it will be reported by the
    DRV_SDHC_Status operation. The system must use DRV_SDHC_Status to find out
    when the driver is in the ready state.
*/

SYS_MODULE_OBJ DRV_SDHC_Initialize
(
    const SYS_MODULE_INDEX index,
    const SYS_MODULE_INIT  *const init
);


// *****************************************************************************
/* Function:
    void DRV_SDHC_Reinitialize
     (
         SYS_MODULE_OBJ          object,
         const SYS_MODULE_INIT * const init
     );

  Summary:
    Reinitializes the driver and refreshes any associated hardware settings.

  Description:
    This routine reinitializes the driver and refreshes any associated hardware
    settings using the given initialization data, but it will not interrupt any
    ongoing operations.

  Precondition:
    Function DRV_SDHC_Initialize must have been called before calling this
    routine and a valid SYS_MODULE_OBJ must have been returned.

  Parameters:
    object          - Driver object handle, returned from the DRV_SDHC_Initialize
                      routine

    init            - Pointer to the initialization data structure

  Returns:
    None

  Example:
    <code>
    DRV_SDHC_INIT     init;
    SYS_MODULE_OBJ      objectHandle;  //  Returned from DRV_SDHC_Initialize

    // Update the required fields of the SD Card initialization structure

    DRV_SDHC_Reinitialize (objectHandle, (SYS_MODULE_INIT*)&init);
    </code>

  Remarks:
    This function can be called multiple times to reinitialize the module.

    This operation can be used to refresh any supported hardware registers as
    specified by the initialization data or to change the power state of the
    module.

    This routine will NEVER block for hardware access. If the operation requires
    time to allow the hardware to reinitialize, it will be reported by the
    DRV_SDHC_Status operation. The system must use DRV_SDHC_Status to find out
    when the driver is in the ready state.
*/

void DRV_SDHC_Reinitialize
(
    SYS_MODULE_OBJ          object,
    const SYS_MODULE_INIT * const init
);

// *****************************************************************************
/* Function:
    void DRV_SDHC_Deinitialize
     (
         SYS_MODULE_OBJ object
     );

  Summary:
    Deinitializes the specified instance of the SD Card driver module.

  Description:
    Deinitializes the specified instance of the SD Card driver module, disabling
    its operation (and any hardware). Invalidates all the internal data.

  Precondition:
    Function DRV_SDHC_Initialize must have been called before calling this
    routine and a valid SYS_MODULE_OBJ must have been returned.

  Parameters:
    object          - Driver object handle, returned from the
                      DRV_SDHC_Initialize routine.

  Returns:
    None.

  Example:
    <code>
    SYS_MODULE_OBJ      objectHandle;     //  Returned from DRV_SDHC_Initialize
    SYS_STATUS          status;

    DRV_SDHC_Deinitialize(objectHandle);

    status = DRV_SDHC_Status(objectHandle);
    if (SYS_MODULE_UNINITIALIZED == status)
    {
        // Check again later if you need to know
        // when the driver is deinitialized.
    }
    </code>

  Remarks:
    Once the Initialize operation has been called, the Deinitialize operation
    must be called before the Initialize operation can be called again.

    This routine will NEVER block waiting for hardware. If the operation
    requires time to allow the hardware to complete, this will be reported by
    the DRV_SDHC_Status operation.  The system has to use DRV_SDHC_Status to
    check if the de-initialization is complete.
*/

void DRV_SDHC_Deinitialize
(
    SYS_MODULE_OBJ object
);


// *****************************************************************************
/* Function:
    SYS_STATUS DRV_SDHC_Status
     (
         SYS_MODULE_OBJ object
     );

  Summary:
    Provides the current status of the SD Card driver module.

  Description:
    This routine provides the current status of the SD Card driver module.

  Precondition:
    Function DRV_SDHC_Initialize must have been called before calling this
    function

  Parameters:
    object                    - Driver object handle, returned from the
                                DRV_SDHC_Initialize routine

  Returns:
    SYS_STATUS_READY          - Indicates that the driver is busy with a
                                previous system level operation and cannot start
                                another

                                Note Any value greater than SYS_STATUS_READY is
                                also a normal running state in which the driver
                                is ready to accept new operations.

    SYS_STATUS_BUSY           - Indicates that the driver is busy with a
                                previous system level operation and cannot start
                                another

    SYS_STATUS_ERROR          - Indicates that the driver is in an error state

                                Note:  Any value less than SYS_STATUS_ERROR is
                                also an error state.

    SYS_MODULE_DEINITIALIZED  - Indicates that the driver has been deinitialized

                                Note:  This value is less than SYS_STATUS_ERROR

  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from DRV_SDHC_Initialize
    SYS_STATUS          status;

    status = DRV_SDHC_Status(object);

    if (SYS_MODULE_UNINITIALIZED == status)
    {
        // Check again later if you need to know
        // when the driver is deinitialized.
    }
    else if (SYS_STATUS_ERROR >= status)
    {
        // Handle error
    }
    </code>

  Remarks:
    This operation can be used to determine when any of the driver's module
    level operations has completed.

    If the status operation returns SYS_STATUS_BUSY, then a previous operation
    has not yet completed. If the status operation returns SYS_STATUS_READY,
    then it indicates that all previous operations have completed.

    The value of SYS_STATUS_ERROR is negative (-1).  Any value less than that is
    also an error state.

    This routine will NEVER block waiting for hardware.

    If the Status operation returns an error value, the error may be cleared by
    calling the reinitialize operation. If that fails, the deinitialize
    operation will need to be called, followed by the initialize operation to
    return to normal operations.
*/

SYS_STATUS DRV_SDHC_Status
(
    SYS_MODULE_OBJ object
);

// *****************************************************************************
/* Function:
    void DRV_SDHC_Tasks
     (
         SYS_MODULE_OBJ object
     );

  Summary:
    Maintains the driver's state machine.

  Description:
    This routine is used to maintain the driver's internal state machine.

  Precondition:
    The DRV_SDHC_Initialize routine must have been called for the specified
    SDHC driver instance.

  Parameters:
    object      - Object handle for the specified driver instance (returned from
                  DRV_SDHC_Initialize)

  Returns:
    None

  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from DRV_SDHC_Initialize

    while (true)
    {
        DRV_SDHC_Tasks (object);

        // Do other tasks
    }
    </code>

  Remarks:
    This routine is normally not called directly by an application. It is
    called by the system's Tasks routine (SYS_Tasks) or by the appropriate raw
    ISR.

    This routine may execute in an ISR context and will never block or access any
    resources that may cause it to block.
*/

void DRV_SDHC_Tasks
(
    SYS_MODULE_OBJ object
);

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - Client Level
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    DRV_HANDLE DRV_SDHC_Open
     (
         const SYS_MODULE_INDEX drvIndex,
         const DRV_IO_INTENT    intent
     );

  Summary:
    Opens the specified SD Card driver instance and returns a handle to it.

  Description:
    This routine opens the specified SD Card driver instance and provides a
    handle that must be provided to all other client-level operations to
    identify the caller and the instance of the driver.

  Precondition:
    Function DRV_SDHC_Initialize must have been called before calling this
    function.

  Parameters:
    drvIndex    - Identifier for the object instance to be opened

    intent      - Zero or more of the values from the enumeration
                  DRV_IO_INTENT "ORed" together to indicate the intended use
                  of the driver

  Returns:
    If successful, the routine returns a valid open-instance handle (a number
    identifying both the caller and the module instance).

    If an error occurs, the return value is DRV_HANDLE_INVALID.

  Example:
    <code>
    DRV_HANDLE  handle;

    handle = DRV_SDHC_Open (DRV_SDHC_INDEX_0, DRV_IO_INTENT_EXCLUSIVE);

    if (DRV_HANDLE_INVALID == handle)
    {
        // Unable to open the driver
    }
    </code>

  Remarks:
    The handle returned is valid until the DRV_SDHC_Close routine is called.

    This routine will NEVER block waiting for hardware.

    If the DRV_IO_INTENT_BLOCKING is requested and the driver was built
    appropriately to support blocking behavior, then other client-level
    operations may block waiting on hardware until they are complete.

    If the requested intent flags are not supported, the routine will return
    DRV_HANDLE_INVALID.
*/

DRV_HANDLE DRV_SDHC_Open
(
    const SYS_MODULE_INDEX drvIndex,
    const DRV_IO_INTENT    intent
);


// *****************************************************************************
/* Function:
    void DRV_SDHC_Close
     (
         DRV_HANDLE handle
     );

  Summary:
    Closes an opened-instance of the SD Card driver.

  Description:
    This routine closes an opened-instance of the SD Card driver, invalidating
    the handle.

  Precondition:
    DRV_SDHC_Open must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    None

  Example:
    <code>
    DRV_HANDLE handle;  // Returned from DRV_SDHC_Open

    DRV_SDHC_Close (handle);
    </code>

  Remarks:
    After calling this routine, the handle passed in "handle" must not be used
    with any of the remaining driver routines.  A new handle must be obtained by
    calling DRV_SDHC_Open before the caller may use the driver again.

    If DRV_IO_INTENT_BLOCKING was requested and the driver was built
    appropriately to support blocking behavior call may block until the
    operation is complete.

    If DRV_IO_INTENT_NON_BLOCKING request the driver client can call the
    DRV_SDHC_Status operation to find out when the module is in
    the ready state (the handle is no longer valid).

    Note:
    Usually there is no need for the driver client to verify that the Close
    operation has completed.
*/

void DRV_SDHC_Close
(
    DRV_HANDLE handle
);

// *****************************************************************************
/* Function:
    void DRV_SDHC_AsyncRead
    (
        const DRV_HANDLE handle,
        DRV_SDHC_COMMAND_HANDLE * commandHandle,
        void * targetBuffer,
        uint32_t blockStart,
        uint32_t nBlock
    );

  Summary:
    Reads blocks of data from the specified block address of the SD Card.

  Description:
    This function schedules a non-blocking read operation for reading blocks
    of data from the SD Card. The function returns with a valid buffer handle
    in the commandHandle argument if the read request was scheduled successfully.
    The function adds the request to the hardware instance queue and returns
    immediately. While the request is in the queue, the application buffer is
    owned by the driver and should not be modified. The function returns
    DRV_SDHC_COMMAND_HANDLE_INVALID in the commandHandle argument under the
    following circumstances:
    - if the driver handle is invalid
    - if the target buffer pointer is NULL
    - if the number of blocks to be read is zero or more than the actual number
      of blocks available
    - if a buffer object could not be allocated to the request
    - if the client opened the driver in write only mode

    If the requesting client registered an event callback with the driver, the
    driver will issue a DRV_SDHC_EVENT_COMMAND_COMPLETE event if the
    buffer was processed successfully or DRV_SDHC_EVENT_COMMAND_ERROR
    event if the buffer was not processed successfully.

  Precondition:
    DRV_SDHC_Open must have been called with DRV_IO_INTENT_READ or
    DRV_IO_INTENT_READWRITE as the ioIntent to obtain a valid opened device handle.

  Parameters:
    handle        - A valid open-instance handle, returned from the driver's
                    open function

    commandHandle - Pointer to an argument that will contain the return buffer
                    handle

    targetBuffer  - Buffer into which the data read from the SD Card will be placed

    blockStart    - Start block address of the SD Card from where the read should begin.

    nBlock        - Total number of blocks to be read.

  Returns:
    The buffer handle is returned in the commandHandle argument. It will be
    DRV_SDHC_COMMAND_HANDLE_INVALID if the request was not successful.

  Example:
    <code>

    uint8_t myBuffer[MY_BUFFER_SIZE];

    // address should be block aligned.
    uint32_t blockStart = 0x00;
    uint32_t    nBlock = 2;
    DRV_SDHC_COMMAND_HANDLE commandHandle;
    MY_APP_OBJ myAppObj;

    // mySDHCHandle is the handle returned
    // by the DRV_SDHC_Open function.

    DRV_SDHC_AsyncRead(mySDHCHandle, &commandHandle, &myBuffer, blockStart, nBlock);

    if(DRV_SDHC_COMMAND_HANDLE_INVALID == commandHandle)
    {
        // Error handling here
    }
    else
    {
        // Read Successful
    }

    </code>

  Remarks:
    None.
*/

void DRV_SDHC_AsyncRead
(
    DRV_HANDLE handle,
    DRV_SDHC_COMMAND_HANDLE *commandHandle,
    void *targetBuffer,
    uint32_t blockStart,
    uint32_t nBlock
);

// *****************************************************************************
/* Function:
    void DRV_SDHC_AyncWrite
    (
        const DRV_HANDLE handle,
        DRV_SDHC_COMMAND_HANDLE * commandHandle,
        void * sourceBuffer,
        uint32_t blockStart,
        uint32_t nBlock
    );

  Summary:
    Writes blocks of data starting at the specified address of the SD Card.

  Description:
    This function schedules a non-blocking write operation for writing blocks
    of data to the SD Card. The function returns with a valid buffer handle
    in the commandHandle argument if the write request was scheduled successfully.
    The function adds the request to the hardware instance queue and returns
    immediately. While the request is in the queue, the application buffer is
    owned by the driver and should not be modified. The function returns
    DRV_SDHC_COMMAND_HANDLE_INVALID in the commandHandle argument under the
    following circumstances:
    - if a buffer object could not be allocated to the request
    - if the source buffer pointer is NULL
    - if the client opened the driver for read only
    - if the number of blocks to be written is either zero or more than the number
      of blocks actually available
    - if the write queue size is full or queue depth is insufficient
    - if the driver handle is invalid

    If the requesting client registered an event callback with the driver, the
    driver will issue a DRV_SDHC_EVENT_COMMAND_COMPLETE event if the
    buffer was processed successfully or DRV_SDHC_EVENT_COMMAND_ERROR
    event if the buffer was not processed successfully.

  Precondition:
    DRV_SDHC_Open() routine must have been called to obtain a valid opened device
    handle. DRV_IO_INTENT_WRITE or DRV_IO_INTENT_READWRITE must have been specified
    as a parameter to this routine.

  Parameters:
    handle        - A valid open-instance handle, returned from the driver's
                    open function

    commandHandle - Pointer to an argument that will contain the return buffer
                    handle

    sourceBuffer  - The source buffer containing data to be programmed to the SD Card.

    blockStart    - Start block address of SD Card where the writes should begin.

    nBlock        - Total number of blocks to be written.

  Returns:
    The buffer handle is returned in the commandHandle argument. It will be
    DRV_SDHC_COMMAND_HANDLE_INVALID if the request was not successful.

  Example:
    <code>

    uint8_t myBuffer[MY_BUFFER_SIZE];

    // address should be block aligned.
    uint32_t blockStart = 0x00;
    uint32_t    nBlock = 2;
    DRV_SDHC_COMMAND_HANDLE commandHandle;
    MY_APP_OBJ myAppObj;

    // mySDHCHandle is the handle returned
    // by the DRV_SDHC_Open function.

    // Client registers an event handler with driver

    DRV_SDHC_EventHandlerSet(mySDHCHandle, APP_SDHCEventHandler, (uintptr_t)&myAppObj);

    DRV_SDHC_AyncWrite(mySDHCHandle, &commandHandle, &myBuffer, blockStart, nBlock);

    if(DRV_SDHC_COMMAND_HANDLE_INVALID == commandHandle)
    {
        // Error handling here
    }

    // Event is received when
    // the buffer is processed.

    void APP_SDHCEventHandler(DRV_SDHC_EVENT event,
            DRV_SDHC_COMMAND_HANDLE commandHandle, uintptr_t contextHandle)
    {
        // contextHandle points to myAppObj.

        switch(event)
        {
            case DRV_SDHC_EVENT_COMMAND_COMPLETE:

                // This means the data was transferred.
                break;

            case DRV_SDHC_EVENT_COMMAND_ERROR:

                // Error handling here.
                break;

            default:
                break;
        }
    }

    </code>

  Remarks:
    None.
*/

void DRV_SDHC_AsyncWrite
(
    DRV_HANDLE handle,
    DRV_SDHC_COMMAND_HANDLE *commandHandle,
    void *sourceBuffer,
    uint32_t blockStart,
    uint32_t nBlock
);

// *****************************************************************************
/* Function:
    DRV_SDHC_COMMAND_STATUS DRV_SDHC_CommandStatusGet
    (
        const DRV_HANDLE handle,
        const DRV_SDHC_COMMAND_HANDLE commandHandle
    );

  Summary:
    Gets the current status of the command.

  Description:
    This routine gets the current status of the command. The application must use
    this routine where the status of a scheduled command needs to be polled on. The
    function may return DRV_SDHC_COMMAND_HANDLE_INVALID in a case where the command
    handle has expired. A command handle expires when the internal buffer object
    is re-assigned to another read or write request. It is recommended that this
    function be called regularly in order to track the command status correctly.

    The application can alternatively register an event handler to receive read or
    write operation completion events.

  Preconditions:
    The DRV_SDHC_Open() must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    - A DRV_SDHC_COMMAND_STATUS value describing the current status of the command.
    - DRV_SDHC_COMMAND_HANDLE_INVALID if the client handle or the command
      handle is not valid.

  Example:
    <code>
    DRV_HANDLE                     handle;         // Returned from DRV_SDHC_Open
    DRV_SDHC_COMMAND_HANDLE      commandHandle;
    DRV_SDHC_COMMAND_STATUS      status;

    status = DRV_SDHC_CommandStatusget(handle, commandHandle);
    if(status == DRV_SDHC_COMMAND_COMPLETED)
    {
        // Operation Done
    }
    </code>

  Remarks:
    This routine will not block for hardware access and will immediately return
    the current status.
*/

DRV_SDHC_COMMAND_STATUS DRV_SDHC_CommandStatusGet
(
    const DRV_HANDLE handle,
    const DRV_SDHC_COMMAND_HANDLE commandHandle
);

// *****************************************************************************
/* Function:
    SYS_MEDIA_GEOMETRY * DRV_SDHC_GeometryGet
    (
        const DRV_HANDLE handle
    );

  Summary:
    Returns the geometry of the device.

  Description:
    This API gives the following geometrical details of the SD Card.
    - Media Property
    - Number of Read/Write/Erase regions in the SD Card
    - Number of Blocks and their size in each region of the device

  Precondition:
    The DRV_SDHC_Open() routine must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open function

  Returns:
    SYS_MEDIA_GEOMETRY - Pointer to structure which holds the media geometry information.

  Example:
    <code>

    SYS_MEDIA_GEOMETRY * SDHCGeometry;
    uint32_t readBlockSize, writeBlockSize, eraseBlockSize;
    uint32_t nReadBlocks, nReadRegions, totalSize;

    SDHCGeometry = DRV_SDHC_GeometryGet(SDHCOpenHandle1);

    readBlockSize  = SDHCGeometry->geometryTable->blockSize;
    nReadBlocks = SDHCGeometry->geometryTable->numBlocks;
    nReadRegions = SDHCGeometry->numReadRegions;

    writeBlockSize  = (SDHCGeometry->geometryTable +1)->blockSize;
    eraseBlockSize  = (SDHCGeometry->geometryTable +2)->blockSize;

    totalSize = readBlockSize * nReadBlocks * nReadRegions;

    </code>

  Remarks:
    None.
*/

SYS_MEDIA_GEOMETRY * DRV_SDHC_GeometryGet
(
    const DRV_HANDLE handle
);


// *****************************************************************************
/* Function:
    void DRV_SDHC_EventHandlerSet
    (
        const DRV_HANDLE handle,
        const void * eventHandler,
        const uintptr_t context
    );

  Summary:
    Allows a client to identify an event handling function for the driver to
    call back when queued operation has completed.

  Description:
    This function allows a client to identify an event handling function for
    the driver to call back when queued operation has completed.
    When a client queues a request for a read or a write operation, it is provided
    with a handle identifying the buffer that was added to the driver's buffer queue.
    The driver will pass this handle back to the client by calling "eventHandler"
    function when the queued operation has completed.

    The event handler should be set before the client performs any read or write
    operations that could generate events. The event handler once set, persists
    until the client closes the driver or sets another event handler (which could
    be a "NULL" pointer to indicate no callback).

  Precondition:
    The DRV_SDHC_Open() routine must have been called to obtain a valid opened
    device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open function

    eventHandler - Pointer to the event handler function implemented by the user

    context      - The value of parameter will be passed back to the client
                   unchanged, when the eventHandler function is called. It can
                   be used to identify any client specific data object that
                   identifies the instance of the client module (for example,
                   it may be a pointer to the client module's state structure).

  Returns:
    None.

  Example:
    <code>
    // myAppObj is an application specific state data object.
    MY_APP_OBJ myAppObj;

    uint8_t myBuffer[MY_BUFFER_SIZE];
    uint32_t blockStart, nBlock;
    DRV_SDHC_COMMAND_HANDLE commandHandle;

    // drvSDHCHandle is the handle returned
    // by the DRV_SDHC_Open function.

    // Client registers an event handler with driver. This is done once.

    DRV_SDHC_EventHandlerSet(drvSDHCHandle, APP_SDHCEventHandler, (uintptr_t)&myAppObj);

    DRV_SDHC_AsyncRead(drvSDHCHandle, &commandHandle, &myBuffer, blockStart, nBlock);

    if(DRV_SDHC_COMMAND_HANDLE_INVALID == commandHandle)
    {
        // Error handling here
    }

    // Event Processing Technique. Event is received when operation is done.

    void APP_SDHCEventHandler(DRV_SDHC_EVENT event,
            DRV_SDHC_COMMAND_HANDLE handle, uintptr_t context)
    {
        // The context handle was set to an application specific
        // object. It is now retrievable easily in the event handler.
        MY_APP_OBJ myAppObj = (MY_APP_OBJ *) context;

        switch(event)
        {
            case DRV_SDHC_EVENT_COMMAND_COMPLETE:

                // This means the data was transferred.
                break;

            case DRV_SDHC_EVENT_COMMAND_ERROR:

                // Error handling here.

                break;

            default:
                break;
        }
    }
    </code>

  Remarks:
    If the client does not want to be notified when the queued operation
    has completed, it does not need to register a callback.
*/

void DRV_SDHC_EventHandlerSet
(
    const DRV_HANDLE handle,
    const void * eventHandler,
    const uintptr_t context
);

// *****************************************************************************
/* Function:
    bool DRV_SDHC_IsAttached
    (
        const DRV_HANDLE handle
    );

  Summary:
    Returns the physical attach status of the SD Card.

  Description:
    This function returns the physical attach status of the SD Card.

  Precondition:
    The DRV_SDHC_Open() routine must have been called to obtain a valid opened
    device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open function

  Returns:
    Returns false if the handle is invalid otherwise returns the attach status
    of the SD Card. Returns true if the SD Card is attached and initialized by the
    SDHC driver otherwise returns false.

  Example:
    <code>


    bool isSDHCAttached;
    isSDHCAttached = DRV_SDHC_isAttached(drvSDHCHandle);

    </code>

  Remarks:
    None.
*/

bool DRV_SDHC_IsAttached
(
    const DRV_HANDLE handle
);

// *****************************************************************************
/* Function:
    bool DRV_SDHC_IsWriteProtected
    (
        const DRV_HANDLE handle
    );

  Summary:
    Returns the write protect status of the SDHC.

  Description:
    This function returns the physical attach status of the SDHC. This function
    returns true if the SD Card is write protected otherwise it returns false.

  Precondition:
    The DRV_SDHC_Open() routine must have been called to obtain a valid opened
    device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open function

  Returns:
    Returns true if the attached SD Card is write protected.
    Returns false if the handle is not valid, or if the SD Card is not write protected.
  Example:
    <code>

    bool isWriteProtected;
    isWriteProtected = DRV_SDHC_IsWriteProtected(drvSDHCHandle);

    </code>

  Remarks:
    None.
*/

bool DRV_SDHC_IsWriteProtected
(
    const DRV_HANDLE handle
);

#endif // #ifndef _DRV_SDHC_H

/*******************************************************************************
 End of File
*/
