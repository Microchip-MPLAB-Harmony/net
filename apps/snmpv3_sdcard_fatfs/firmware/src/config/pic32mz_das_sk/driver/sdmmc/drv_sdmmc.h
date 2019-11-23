/*******************************************************************************
  SD Host Controller Driver Interface

  Company:
    Microchip Technology Inc.

  File Name:
    drv_sdmmc.h

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

#ifndef _DRV_SDMMC_H
#define _DRV_SDMMC_H


// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "system/system.h"
#include "driver/driver_common.h"
#include "system/int/sys_int.h"
#include "system/system_media.h"
#include "driver/sdmmc/drv_sdmmc_definitions.h"

// *****************************************************************************
/* SDMMC Driver command handle.

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
typedef SYS_MEDIA_BLOCK_COMMAND_HANDLE DRV_SDMMC_COMMAND_HANDLE;


// *****************************************************************************
/* SDMMC Driver Invalid Command Handle.

  Summary:
    SDMMC Driver's Invalid Command Handle.

  Description:
    This value defines the SDMMC Driver Invalid Command Handle. This value
    is returned by read or write routines when the command request was not
    accepted.

  Remarks:
    None.
*/

#define DRV_SDMMC_COMMAND_HANDLE_INVALID SYS_MEDIA_BLOCK_COMMAND_HANDLE_INVALID

// *****************************************************************************
/* SDMMC Driver Events

   Summary
    Identifies the possible events that can result from a request.

   Description
    This enumeration identifies the possible events that can result from a
    read or a write request issued by the client.

   Remarks:
    One of these values is passed in the "event" parameter of the event
    handling callback function that client registered with the driver by
    calling the DRV_SDMMC_EventHandlerSet function when a request is completed.
*/

typedef enum
{
    /* Operation has been completed successfully. */
    DRV_SDMMC_EVENT_COMMAND_COMPLETE = SYS_MEDIA_EVENT_BLOCK_COMMAND_COMPLETE,

    /* There was an error during the operation */
    DRV_SDMMC_EVENT_COMMAND_ERROR = SYS_MEDIA_EVENT_BLOCK_COMMAND_ERROR

} DRV_SDMMC_EVENT;


// *****************************************************************************
/* SDMMC Driver Events

   Summary
    Identifies the possible events that can result from a request.

   Description
    This enumeration identifies the possible status values of a read or write
    buffer request submitted to the driver.

   Remarks:
    One of these values is returned by the DRV_SDMMC_CommandStatus routine.
*/
typedef enum
{
    /* Done OK and ready */
    DRV_SDMMC_COMMAND_COMPLETED          = SYS_MEDIA_COMMAND_COMPLETED,

    /* Scheduled but not started */
    DRV_SDMMC_COMMAND_QUEUED             = SYS_MEDIA_COMMAND_QUEUED,

    /* Currently being in transfer */
    DRV_SDMMC_COMMAND_IN_PROGRESS        = SYS_MEDIA_COMMAND_IN_PROGRESS,

    /* Unknown Command */
    DRV_SDMMC_COMMAND_ERROR_UNKNOWN      = SYS_MEDIA_COMMAND_UNKNOWN,

} DRV_SDMMC_COMMAND_STATUS;


// *****************************************************************************
/* SDMMC Driver Event Handler Function Pointer

   Summary
    Pointer to a SDMMCDriver Event handler function

   Description
    This data type defines the required function signature for the SDMMC event
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
    void APP_MySDMMCEventHandler
    (
        DRV_SDMMC_EVENT event,
        DRV_SDMMC_COMMAND_HANDLE commandHandle,
        uintptr_t context
    )
    {
        MY_APP_DATA_STRUCT* pAppData = (MY_APP_DATA_STRUCT* ) context;

        switch(event)
        {
            case DRV_SDMMC_EVENT_COMMAND_COMPLETE:

                // Handle the completed buffer.
                break;

            case DRV_SDMMC_EVENT_COMMAND_ERROR:
            default:

                // Handle error.
                break;
        }
    }
    </code>

  Remarks:
    If the event is DRV_SDMMC_EVENT_COMMAND_COMPLETE, it means that the
    write or a read operation was completed successfully.

    If the event is DRV_SDMMC_EVENT_COMMAND_ERROR, it means that the operation
    was not completed successfully.

    The context parameter contains the handle to the client context, provided
    at the time the event handling function was  registered using the
    DRV_SDMMC_EventHandlerSet function. This context handle value is
    passed back to the client as the "context" parameter.  It can be any value
    necessary to identify the client context or instance (such as a pointer to
    the client's data) instance of the client that made the read/write
    request.
*/
typedef SYS_MEDIA_EVENT_HANDLER DRV_SDMMC_EVENT_HANDLER;

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - System Level
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_SDMMC_Initialize (
         const SYS_MODULE_INDEX index,
         const SYS_MODULE_INIT* const init
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
    DRV_SDMMC_INIT      init;
    SYS_MODULE_OBJ      objectHandle;

    // Populate the SD Card initialization structure

    objectHandle = DRV_SDMMC_Initialize(DRV_SDMMC_INDEX_0, (SYS_MODULE_INIT*)&init);
    if (objectHandle == SYS_MODULE_OBJ_INVALID)
    {
        // Handle error
    }
    </code>

  Remarks:
    This routine must be called before any other SD Card routine is called.
    This routine should only be called once during system initialization.

    This routine will NEVER block for hardware access. The system must use
    DRV_SDMMC_Status to find out when the driver is in the ready state.
*/

SYS_MODULE_OBJ DRV_SDMMC_Initialize
(
    const SYS_MODULE_INDEX index,
    const SYS_MODULE_INIT* const init
);

// *****************************************************************************
/* Function:
    SYS_STATUS DRV_SDMMC_Status (
        SYS_MODULE_OBJ object
    );

  Summary:
    Provides the current status of the SD Card driver module.

  Description:
    This routine provides the current status of the SD Card driver module.

  Precondition:
    Function DRV_SDMMC_Initialize must have been called before calling this
    function

  Parameters:
    object                    - Driver object handle, returned from the
                                DRV_SDMMC_Initialize routine

  Returns:
    SYS_STATUS_READY          - Indicates that the driver is busy with a
                                previous system level operation and cannot start
                                another

                                Note Any value greater than SYS_STATUS_READY is
                                also a normal running state in which the driver
                                is ready to accept new operations.

    SYS_STATUS_UNINITIALIZED  - Driver is not initialized.

  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from DRV_SDMMC_Initialize
    SYS_STATUS          status;

    status = DRV_SDMMC_Status(object);

    if (status == SYS_STATUS_READY)
    {
        // Driver is initialized and ready to accept requests
    }
    </code>

  Remarks:
    None.
*/

SYS_STATUS DRV_SDMMC_Status
(
    SYS_MODULE_OBJ object
);

// *****************************************************************************
/* Function:
    void DRV_SDMMC_Tasks (
        SYS_MODULE_OBJ object
    );

  Summary:
    Maintains the driver's state machine.

  Description:
    This routine is used to maintain the driver's internal state machine.

  Precondition:
    The DRV_SDMMC_Initialize routine must have been called for the specified
    SDMMC driver instance.

  Parameters:
    object      - Object handle for the specified driver instance (returned from
                  DRV_SDMMC_Initialize)

  Returns:
    None

  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from DRV_SDMMC_Initialize

    while (true)
    {
        DRV_SDMMC_Tasks (object);

        // Do other tasks
    }
    </code>

  Remarks:
    This routine is normally not called directly by an application. It is
    called by the system's Tasks routine (SYS_Tasks).
*/

void DRV_SDMMC_Tasks
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
    DRV_HANDLE DRV_SDMMC_Open (
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
    Function DRV_SDMMC_Initialize must have been called before calling this
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

    handle = DRV_SDMMC_Open (DRV_SDMMC_INDEX_0, DRV_IO_INTENT_EXCLUSIVE);

    if (handle == DRV_HANDLE_INVALID)
    {
        // Unable to open the driver
    }
    </code>

  Remarks:
    The handle returned is valid until the DRV_SDMMC_Close routine is called.
*/

DRV_HANDLE DRV_SDMMC_Open
(
    const SYS_MODULE_INDEX drvIndex,
    const DRV_IO_INTENT    intent
);


// *****************************************************************************
/* Function:
    void DRV_SDMMC_Close (
        DRV_HANDLE handle
    );

  Summary:
    Closes an opened-instance of the SD Card driver.

  Description:
    This routine closes an opened-instance of the SD Card driver, invalidating
    the handle.

  Precondition:
    The DRV_SDMMC_Initialize routine must have been called for the specified
    SD Card driver instance.

    DRV_SDMMC_Open must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    None

  Example:
    <code>
    DRV_HANDLE handle;  // Returned from DRV_SDMMC_Open

    DRV_SDMMC_Close (handle);
    </code>

  Remarks:
    After calling this routine, the handle passed in "handle" must not be used
    with any of the remaining driver routines.  A new handle must be obtained by
    calling DRV_SDMMC_Open before the caller may use the driver again.

    If any requests were queued by the client, the driver will remove those
    requests from the queue and issue a DRV_SDMMC_EVENT_COMMAND_ERROR event,
    (if an event handler is registered by the client) before the client is
    closed.
*/

void DRV_SDMMC_Close
(
    DRV_HANDLE handle
);

// *****************************************************************************
/* Function:
    void DRV_SDMMC_AsyncRead (
        const DRV_HANDLE handle,
        DRV_SDMMC_COMMAND_HANDLE* commandHandle,
        void* targetBuffer,
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
    DRV_SDMMC_COMMAND_HANDLE_INVALID in the commandHandle argument under the
    following circumstances:
    - if the driver handle is invalid
    - if the target buffer pointer is NULL
    - if the number of blocks to be read is zero or more than the actual number
      of blocks available
    - if a buffer object could not be allocated to the request

    If the requesting client registered an event callback with the driver, the
    driver will issue a DRV_SDMMC_EVENT_COMMAND_COMPLETE event if the
    buffer was processed successfully or DRV_SDMMC_EVENT_COMMAND_ERROR
    event if the buffer was not processed successfully.

  Precondition:
    The DRV_SDMMC_Initialize routine must have been called for the specified SDMMC
    driver instance.

    DRV_SDMMC_Open must have been called and a valid handle must have been obtained.

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
    DRV_SDMMC_COMMAND_HANDLE_INVALID if the request was not successful.

  Example:
    <code>

    uint8_t myBuffer[MY_BUFFER_SIZE];

    // address should be block aligned.
    uint32_t blockStart = 0x00;
    uint32_t nBlock = 2;
    DRV_SDMMC_COMMAND_HANDLE commandHandle;
    MY_APP_OBJ myAppObj;

    // mySDMMCHandle is the handle returned
    // by the DRV_SDMMC_Open function.

    DRV_SDMMC_AsyncRead(mySDMMCHandle, &commandHandle, &myBuffer[0], blockStart, nBlock);

    if(commandHandle == DRV_SDMMC_COMMAND_HANDLE_INVALID)
    {
        // Error handling here
    }
    else
    {
        // Read Successfully queued
    }

    </code>

  Remarks:
    None.
*/

void DRV_SDMMC_AsyncRead
(
    DRV_HANDLE handle,
    DRV_SDMMC_COMMAND_HANDLE* commandHandle,
    void* targetBuffer,
    uint32_t blockStart,
    uint32_t nBlock
);

// *****************************************************************************
/* Function:
    void DRV_SDMMC_AsyncWrite (
        const DRV_HANDLE handle,
        DRV_SDMMC_COMMAND_HANDLE* commandHandle,
        void* sourceBuffer,
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
    DRV_SDMMC_COMMAND_HANDLE_INVALID in the commandHandle argument under the
    following circumstances:
    - if the driver handle is invalid
    - if the source buffer pointer is NULL
    - if the number of blocks to write is zero or more than the actual number
      of blocks available
    - if a buffer object could not be allocated to the request

    If the requesting client registered an event callback with the driver, the
    driver will issue a DRV_SDMMC_EVENT_COMMAND_COMPLETE event if the
    buffer was processed successfully or DRV_SDMMC_EVENT_COMMAND_ERROR
    event if the buffer was not processed successfully.

  Precondition:
    The DRV_SDMMC_Initialize routine must have been called for the specified
    SDMMC driver instance.

    DRV_SDMMC_Open routine must have been called to obtain a valid opened device
    handle.

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
    DRV_SDMMC_COMMAND_HANDLE_INVALID if the request was not successful.

  Example:
    <code>

    uint8_t myBuffer[MY_BUFFER_SIZE];

    // address should be block aligned.
    uint32_t blockStart = 0x00;
    uint32_t nBlock = 2;
    DRV_SDMMC_COMMAND_HANDLE commandHandle;
    MY_APP_OBJ myAppObj;

    // mySDMMCHandle is the handle returned
    // by the DRV_SDMMC_Open function.

    // Client registers an event handler with driver

    DRV_SDMMC_EventHandlerSet(mySDMMCHandle, APP_SDMMCEventHandler, (uintptr_t)&myAppObj);

    DRV_SDMMC_AsyncWrite(mySDMMCHandle, &commandHandle, &myBuffer[0], blockStart, nBlock);

    if(commandHandle == DRV_SDMMC_COMMAND_HANDLE_INVALID)
    {
        // Error handling here
    }

    // Event is received when
    // the buffer is processed.

    void APP_SDMMCEventHandler(
        DRV_SDMMC_EVENT event,
        DRV_SDMMC_COMMAND_HANDLE commandHandle,
        uintptr_t contextHandle
    )
    {
        // contextHandle points to myAppObj.

        switch(event)
        {
            case DRV_SDMMC_EVENT_COMMAND_COMPLETE:

                // This means the data was transferred successfully
                break;

            case DRV_SDMMC_EVENT_COMMAND_ERROR:

                // Error handling here
                break;

            default:
                break;
        }
    }

    </code>

  Remarks:
    None.
*/

void DRV_SDMMC_AsyncWrite
(
    DRV_HANDLE handle,
    DRV_SDMMC_COMMAND_HANDLE* commandHandle,
    void* sourceBuffer,
    uint32_t blockStart,
    uint32_t nBlock
);

// *****************************************************************************
/* Function:
    DRV_SDMMC_COMMAND_STATUS DRV_SDMMC_CommandStatus (
        const DRV_HANDLE handle,
        const DRV_SDMMC_COMMAND_HANDLE commandHandle
    );

  Summary:
    Gets the current status of the command.

  Description:
    This routine gets the current status of the command. The application must use
    this routine where the status of a scheduled command needs to be polled on. The
    function may return DRV_SDMMC_COMMAND_ERROR_UNKNOWN in a case where the command
    handle has expired. A command handle expires when the internal buffer object
    is re-assigned to another read or write request. It is recommended that this
    function be called regularly in order to track the command status correctly.

    The application can alternatively register an event handler to receive read or
    write operation completion events.

  Preconditions:
    The DRV_SDMMC_Initialize routine must have been called.

    The DRV_SDMMC_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    A DRV_SDMMC_COMMAND_STATUS value describing the current status of the command.
    Returns DRV_SDMMC_COMMAND_ERROR_UNKNOWN if the client handle or the command
    handle is not valid.

  Example:
    <code>
    DRV_HANDLE                    handle;         // Returned from DRV_SDMMC_Open
    DRV_SDMMC_COMMAND_HANDLE      commandHandle;
    DRV_SDMMC_COMMAND_STATUS      status;

    status = DRV_SDMMC_CommandStatus(handle, commandHandle);
    if(status == DRV_SDMMC_COMMAND_COMPLETED)
    {
        // Operation Done
    }
    </code>

  Remarks:
    This routine will not block for hardware access and will immediately return
    the current status.
*/

DRV_SDMMC_COMMAND_STATUS DRV_SDMMC_CommandStatus
(
    const DRV_HANDLE handle,
    const DRV_SDMMC_COMMAND_HANDLE commandHandle
);

// *****************************************************************************
/* Function:
    SYS_MEDIA_GEOMETRY* DRV_SDMMC_GeometryGet (
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
    The DRV_SDMMC_Initialize routine must have been called for the
    specified SDMMC driver instance.

    The DRV_SDMMC_Open routine must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open function

  Returns:
    SYS_MEDIA_GEOMETRY - Pointer to structure which holds the media geometry information.

  Example:
    <code>

    SYS_MEDIA_GEOMETRY* SDMMCGeometry;
    uint32_t readBlockSize, writeBlockSize, eraseBlockSize;
    uint32_t nReadBlocks, nReadRegions, totalSize;

    SDMMCGeometry = DRV_SDMMC_GeometryGet(SDMMCOpenHandle1);

    readBlockSize  = SDMMCGeometry->geometryTable->blockSize;
    nReadBlocks = SDMMCGeometry->geometryTable->numBlocks;
    nReadRegions = SDMMCGeometry->numReadRegions;

    writeBlockSize  = (SDMMCGeometry->geometryTable +1)->blockSize;
    eraseBlockSize  = (SDMMCGeometry->geometryTable +2)->blockSize;

    totalSize = readBlockSize * nReadBlocks * nReadRegions;

    </code>

  Remarks:
    None.
*/

SYS_MEDIA_GEOMETRY* DRV_SDMMC_GeometryGet
(
    const DRV_HANDLE handle
);


// *****************************************************************************
/* Function:
    void DRV_SDMMC_EventHandlerSet (
        const DRV_HANDLE handle,
        const void* eventHandler,
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
    The DRV_SDMMC_Initialize routine must have been called for the specified
    SDMMC driver instance.

    The DRV_SDMMC_Open routine must have been called to obtain a valid opened
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
    DRV_SDMMC_COMMAND_HANDLE commandHandle;

    // drvSDMMCHandle is the handle returned
    // by the DRV_SDMMC_Open function.

    // Client registers an event handler with driver. This is done once.

    DRV_SDMMC_EventHandlerSet(drvSDMMCHandle, APP_SDMMCEventHandler, (uintptr_t)&myAppObj);

    DRV_SDMMC_AsyncRead(drvSDMMCHandle, &commandHandle, &myBuffer[0], blockStart, nBlock);

    if(commandHandle == DRV_SDMMC_COMMAND_HANDLE_INVALID)
    {
        // Error handling here
    }

    // Event Processing Technique. Event is received when operation is done.

    void APP_SDMMCEventHandler(
        DRV_SDMMC_EVENT event,
        DRV_SDMMC_COMMAND_HANDLE handle,
        uintptr_t context
    )
    {
        // The context handle was set to an application specific
        // object. It is now retrievable easily in the event handler.

        MY_APP_OBJ myAppObj = (MY_APP_OBJ* ) context;

        switch(event)
        {
            case DRV_SDMMC_EVENT_COMMAND_COMPLETE:

                // This means the data was transferred successfully
                break;

            case DRV_SDMMC_EVENT_COMMAND_ERROR:

                // Error handling here
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

void DRV_SDMMC_EventHandlerSet
(
    const DRV_HANDLE handle,
    const void* eventHandler,
    const uintptr_t context
);

// *****************************************************************************
/* Function:
    bool DRV_SDMMC_IsAttached (
        const DRV_HANDLE handle
    );

  Summary:
    Returns the physical attach status of the SD Card.

  Description:
    This function returns the physical attach status of the SD Card.

  Precondition:
    The DRV_SDMMC_Initialize routine must have been called for the specified
    SDMMC driver instance.

    The DRV_SDMMC_Open routine must have been called to obtain a valid opened
    device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open function

  Returns:
    Returns false if the handle is invalid otherwise returns the attach status
    of the SD Card. Returns true if the SD Card is attached and initialized by the
    SDMMC driver otherwise returns false.

  Example:
    <code>

    // drvSDMMCHandle is the handle returned
    // by the DRV_SDMMC_Open function.

    bool isSDMMCAttached;
    isSDMMCAttached = DRV_SDMMC_isAttached(drvSDMMCHandle);

    </code>

  Remarks:
    None.
*/

bool DRV_SDMMC_IsAttached
(
    const DRV_HANDLE handle
);

// *****************************************************************************
/* Function:
    bool DRV_SDMMC_IsWriteProtected (
        const DRV_HANDLE handle
    );

  Summary:
    Returns the write protect status of the SDMMC.

  Description:
    This function returns the physical write status of the SDMMC. This function
    returns true if the SD Card is write protected otherwise it returns false.

  Precondition:
    The DRV_SDMMC_Initialize routine must have been called for the specified
    SDMMC driver instance.

    The DRV_SDMMC_Open routine must have been called to obtain a valid opened
    device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open function

  Returns:
    Returns true if the attached SD Card is write protected.
    Returns false if the handle is not valid, or if the SD Card is not write protected,
    or if the write protection check is not enabled.
  Example:
    <code>

    bool isWriteProtected;
    isWriteProtected = DRV_SDMMC_IsWriteProtected(drvSDMMCHandle);

    </code>

  Remarks:
    None.
*/

bool DRV_SDMMC_IsWriteProtected
(
    const DRV_HANDLE handle
);

#include "driver/sdmmc/src/drv_sdmmc_local.h"

#endif // #ifndef _DRV_SDMMC_H

/*******************************************************************************
 End of File
*/
