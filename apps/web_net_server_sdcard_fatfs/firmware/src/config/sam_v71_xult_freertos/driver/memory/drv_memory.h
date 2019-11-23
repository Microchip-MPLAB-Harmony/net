/*******************************************************************************
  Memory Driver Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    drv_memory.h

  Summary:
    Memory Driver Interface Definition

  Description:
    The Memory driver provides a simple interface to manage the MEMORYVF series
    of SQI Flash Memory connected to Microchip micro controllers. This file
    defines the interface definition for the Memory driver.
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
#ifndef _DRV_MEMORY_H
#define _DRV_MEMORY_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "drv_memory_definitions.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Summary:
    Handle to identify commands queued in the driver.

  Description:
    A command handle is returned by a call to the Read, Write, Erase or
    EraseWrite functions. This handle allows the application to track the
    completion of the operation. This command handle is also returned to the
    client along with the event that has occurred with respect to the command.
    This allows the application to connect the event to a specific command in
    case where multiple commands are queued.

    The command handle associated with the command request expires when the
    client has been notified of the completion of the command (after event
    handler function that notifies the client returns) or after the command has
    been retired by the driver if no event handler callback was set.

  Remarks:
    Refer sys_media.h for definition of SYS_MEDIA_BLOCK_COMMAND_HANDLE.
*/

typedef SYS_MEDIA_BLOCK_COMMAND_HANDLE  DRV_MEMORY_COMMAND_HANDLE;

// *****************************************************************************
/*
  Summary:
    This value defines the Memory Driver's Invalid Command Handle.

  Description:
    This value defines the Memory Driver's Invalid Command Handle. This value is
    returned by read/write/erase/erasewrite routines when the command request
    was not accepted.

  Remarks:
    Refer sys_media.h for definition of SYS_MEDIA_BLOCK_COMMAND_HANDLE_INVALID.
*/

#define DRV_MEMORY_COMMAND_HANDLE_INVALID SYS_MEDIA_BLOCK_COMMAND_HANDLE_INVALID

// *****************************************************************************
/*
   Summary
    Identifies the possible events that can result from a request.

   Description
    This enumeration identifies the possible events that can result from a
    read, write, erase or erasewrite request caused by the client.

   Remarks:
    One of these values is passed in the "event" parameter of the event
    handling callback function that client registered with the driver by
    calling the DRV_MEMORY_TransferHandlerSet function when a request is completed.

    Refer sys_media.h for SYS_MEDIA_XXX definitions.
*/

typedef enum
{
    /* Operation has been completed successfully. */
    DRV_MEMORY_EVENT_COMMAND_COMPLETE = SYS_MEDIA_EVENT_BLOCK_COMMAND_COMPLETE,

    /* There was an error during the operation */
    DRV_MEMORY_EVENT_COMMAND_ERROR    = SYS_MEDIA_EVENT_BLOCK_COMMAND_ERROR

} DRV_MEMORY_EVENT;

// ***********************************************************************
/*
  Summary:
    Memory Driver command Status

  Description:
    Specifies the status of the command for the read, write, erase and
    erasewrite operations.

  Remarks:
    Refer sys_media.h for SYS_MEDIA_XXX definitions.
*/
typedef enum
{
    /* Done OK and ready */
    DRV_MEMORY_COMMAND_COMPLETED          = SYS_MEDIA_COMMAND_COMPLETED,

    /* Scheduled but not started */
    DRV_MEMORY_COMMAND_QUEUED             = SYS_MEDIA_COMMAND_QUEUED,

    /* Currently being in transfer */
    DRV_MEMORY_COMMAND_IN_PROGRESS        = SYS_MEDIA_COMMAND_IN_PROGRESS,

    /* Unknown Command */
    DRV_MEMORY_COMMAND_ERROR_UNKNOWN      = SYS_MEDIA_COMMAND_UNKNOWN,

} DRV_MEMORY_COMMAND_STATUS;

// *****************************************************************************
/*
   Summary
    Pointer to a Memory Driver Event handler function

   Description
    This data type defines the required function signature for the Memory event
    handling callback function. A client must register a pointer to an event
    handling function whose function signature (parameter and return value
    types) match the types specified by this function pointer in order to
    receive event calls back from the driver.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    event           - Identifies the type of event

    commandHandle   - Handle returned from the Read/Write/Erase/EraseWrite
                      requests

    context         - Value identifying the context of the application that
                      registered the event handling function

  Returns:
    None.

  Example:
    <code>

    void appTransferHandler
    (
        DRV_MEMORY_EVENT event,
        DRV_MEMORY_COMMAND_HANDLE commandHandle,
        uintptr_t context
    )
    {
        switch(event)
        {
            case DRV_MEMORY_EVENT_COMMAND_COMPLETE:
                xfer_done = true;
                break;
            case DRV_MEMORY_EVENT_COMMAND_ERROR:
                // Handle Error
                break;
            default:
                break;
        }
    }

    </code>

  Remarks:
    If the event is DRV_MEMORY_EVENT_COMMAND_COMPLETE, it means that the
    requested operation was completed successfully.

    If the event is DRV_MEMORY_EVENT_COMMAND_ERROR, it means that the scheduled
    operation was not completed successfully.

    The context parameter contains the handle to the client context, provided
    at the time the event handling function was registered using the
    DRV_MEMORY_TransferHandlerSet function. This context handle value is passed
    back to the client as the "context" parameter. It can be any value
    necessary to identify the client context or instance (such as a pointer to
    the client's data) instance of the client that made the read/write/erase
    request.

    Used in Asynchronous mode of operation.

    Refer sys_media.h for definition of SYS_MEDIA_EVENT_HANDLER.
*/
typedef SYS_MEDIA_EVENT_HANDLER   DRV_MEMORY_TRANSFER_HANDLER;

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Memory Driver Module Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_MEMORY_Initialize
    (
        const SYS_MODULE_INDEX drvIndex,
        const SYS_MODULE_INIT * const init
    );

  Summary:
    Initializes the Memory instance for the specified driver index

  Description:
    This routine initializes the Memory driver instance for the specified
    driver index, making it ready for clients to open and use it.

  Precondition:
    None.

  Parameters:
    drvIndex -  Identifier for the instance to be initialized
    init     -  Pointer to a data structure containing any data necessary to
                initialize the driver.

  Returns:
    If successful, returns a valid handle to a driver instance object.
    Otherwise it returns SYS_MODULE_OBJ_INVALID.

  Example:
    <code>
    // This code snippet shows an example of initializing the Memory Driver
    // with SST26 serial flash device attached and File system Enabled.

    SYS_MODULE_OBJ  objectHandle;

    static uint8_t gDrvMemory0EraseBuffer[DRV_MEMORY_ERASE_BUFFER_SIZE_IDX0] __attribute__((aligned(32)));

    static DRV_MEMORY_CLIENT_OBJECT gDrvMemory0ClientObject[DRV_MEMORY_CLIENTS_NUMBER_IDX0] = { 0 };

    static DRV_MEMORY_BUFFER_OBJECT gDrvMemory0BufferObject[DRV_MEMORY_BUFFER_QUEUE_SIZE_IDX0] = { 0 };

    const DRV_MEMORY_DEVICE_INTERFACE drvMemory0DeviceAPI = {
        .Open               = DRV_SST26_Open,
        .Close              = DRV_SST26_Close,
        .Status             = DRV_SST26_Status,
        .SectorErase        = DRV_SST26_SectorErase,
        .Read               = DRV_SST26_Read,
        .PageWrite          = DRV_SST26_PageWrite,
        .GeometryGet        = (DRV_MEMORY_DEVICE_GEOMETRY_GET)DRV_SST26_GeometryGet,
        .TransferStatusGet  = (DRV_MEMORY_DEVICE_TRANSFER_STATUS_GET)DRV_SST26_TransferStatusGet
    };

    const DRV_MEMORY_INIT drvMemory0InitData =
    {
        .memDevIndex          = DRV_SST26_INDEX,
        .memoryDevice         = &drvMemory0DeviceAPI,
        .isFsEnabled          = true,
        .deviceMediaType      = (uint8_t)SYS_FS_MEDIA_TYPE_SPIFLASH,
        .ewBuffer             = &gDrvMemory0EraseBuffer[0],
        .clientObjPool        = (uintptr_t)&gDrvMemory0ClientObject[0],
        .bufferObj            = (uintptr_t)&gDrvMemory0BufferObject[0],
        .queueSize            = DRV_MEMORY_BUFFER_QUEUE_SIZE_IDX0,
        .nClientsMax          = DRV_MEMORY_CLIENTS_NUMBER_IDX0
    };

    //usage of DRV_MEMORY_INDEX_0 indicates usage of Flash-related APIs
    objectHandle = DRV_MEMORY_Initialize((SYS_MODULE_INDEX)DRV_MEMORY_INDEX_0, (SYS_MODULE_INIT *)&drvMemory0InitData);

    if (SYS_MODULE_OBJ_INVALID == objectHandle)
    {
        // Handle error
    }
    </code>

  Remarks:
    This routine must be called before any other Memory routine is called.

    This routine should only be called once during system initialization.

    This routine will NEVER block for hardware access. If the operation
    requires time to allow the hardware to initialize, it will be reported by
    the DRV_MEMORY_Status operation. The system must use DRV_MEMORY_Status to
    find out when the driver is in the ready state.
*/

SYS_MODULE_OBJ DRV_MEMORY_Initialize
(
    const SYS_MODULE_INDEX drvIndex,
    const SYS_MODULE_INIT * const init
);

// *************************************************************************
/* Function:
    SYS_STATUS DRV_MEMORY_Status( SYS_MODULE_OBJ object );

  Summary:
    Gets the current status of the Memory driver module.

  Description:
    This routine provides the current status of the Memory driver module.

  Preconditions:
    Function DRV_MEMORY_Initialize should have been called before calling
    this function.

  Parameters:
    object -  Driver object handle, returned from the DRV_MEMORY_Initialize
              routine

  Returns:
    SYS_STATUS_READY - Indicates that the driver is ready and accept
    requests for new operations.

    SYS_STATUS_UNINITIALIZED - Indicates the driver is not initialized.

    SYS_STATUS_BUSY - Indicates the driver is in busy state.

  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from DRV_MEMORY_Initialize
    SYS_STATUS          MEMORYStatus;

    MEMORYStatus = DRV_MEMORY_Status(object);

    </code>

  Remarks:
    This routine will NEVER block waiting for hardware.
*/

SYS_STATUS DRV_MEMORY_Status( SYS_MODULE_OBJ object );

// ****************************************************************************
/* Function:
    void DRV_MEMORY_Tasks( SYS_MODULE_OBJ object );

  Summary:
    Maintains the Memory driver's internal state machine.

  Description:
    This routine maintains the driver's internal state machine.

    Initial state is put to process Queue so that driver can accept
    transfer requests(open, erase, read, write, etc) from client.

    This routine is also responsible for checking the status of Erase and Write
    transfer requests and notify the client through transferHandler registered
    if any.

    The state of driver will be busy until transfer is completed. Once transfer
    is done the state goes back to Process sate until new requests is received.

  Preconditions:
    The DRV_MEMORY_Initialize routine must have been called for the specified
    Memory driver instance.

  Parameters:
    object -  Driver object handle, returned from the DRV_MEMORY_Initialize
              routine

  Returns:
    None.

  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from DRV_MEMORY_Initialize

    void SYS_Tasks ( void )
    {
        DRV_Memory_Tasks (object);
        // Do other tasks
    }
    </code>

  Remarks:
    This routine is generated only in Asynchronous mode.

    This routine should not be called directly by application.
    - For Bare Metal it will be called by the system Task routine (SYS_Tasks).
    - For RTOS a separate Thread will be created for this task and will be
      called in the thread context.
*/

void DRV_MEMORY_Tasks( SYS_MODULE_OBJ object );

// *****************************************************************************
// *****************************************************************************
// Section: Memory Driver Client Routines
// *****************************************************************************
// *****************************************************************************

// ****************************************************************************
/* Function:
    DRV_HANDLE DRV_MEMORY_Open
    (
        const SYS_MODULE_INDEX drvIndex,
        const DRV_IO_INTENT ioIntent
    );

  Summary:
    Opens the specified Memory driver instance and returns a handle to it

  Description:
    This routine opens the specified Memory driver instance and provides a handle.

    When Open is called for the first time for specified Memory driver instance it
    opens the attached media device and reads the geomerty details.

    This handle returned must be provided to all other client-level operations to
    identify the caller and the instance of the driver.

  Preconditions:
    Function DRV_MEMORY_Initialize must have been called before calling this
    function.

  Parameters:
    drvIndex   -  Identifier for the instance to be opened

    ioIntent   -  Zero or more of the values from the enumeration
                  DRV_IO_INTENT "ORed" together to indicate the intended use
                  of the driver

  Returns:
    If successful, the routine returns a valid open-instance handle (a
    number identifying both the caller and the module instance).

    If an error occurs, DRV_HANDLE_INVALID is returned. Errors can occur
    under the following circumstances:
        - if the attached media device is not ready or the geometry get fails.
        - if the number of client objects allocated via DRV_MEMORY_CLIENTS_NUMBER
          is insufficient
        - if the client is trying to open the driver but driver has been opened
          exclusively by another client
        - if the client is trying to open the driver exclusively, but has already
          been opened in a non exclusive mode by another client.
        - if the driver hardware instance being opened is not initialized or is
          invalid
        - if the attached memory device open or geometry read fails for first time.

  Example:
    <code>
    DRV_HANDLE handle;

    handle = DRV_MEMORY_Open(DRV_MEMORY_INDEX_0);
    if (DRV_HANDLE_INVALID == handle)
    {
        // Unable to open the driver
    }
    </code>

  Remarks:
    The handle returned is valid until the DRV_MEMORY_Close routine is called.

    This routine will NEVER block wait for hardware. If the driver has
    has already been opened, it cannot be opened again.
*/

DRV_HANDLE DRV_MEMORY_Open
(
    const SYS_MODULE_INDEX drvIndex,
    const DRV_IO_INTENT ioIntent
);

// *****************************************************************************
/* Function:
    void DRV_MEMORY_Close( const DRV_HANDLE handle );

  Summary:
    Closes an opened-instance of the Memory driver

  Description:
    This routine closes an opened-instance of the Memory driver, invalidating
    the handle.

  Precondition:
    DRV_MEMORY_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    None

  Example:
    <code>
    DRV_HANDLE handle;  // Returned from DRV_MEMORY_Open

    DRV_MEMORY_Close(handle);
    </code>

  Remarks:
    After calling this routine, the handle passed in "handle" must not be used
    with any of the remaining driver routines. A new handle must be obtained by
    calling DRV_MEMORY_Open before the caller may use the driver again. Usually
    there is no need for the driver client to verify that the Close operation
    has completed.
*/

void DRV_MEMORY_Close( const DRV_HANDLE handle );

// **************************************************************************
/* Function:
    void DRV_MEMORY_AsyncErase
    (
        const DRV_HANDLE handle,
        DRV_MEMORY_COMMAND_HANDLE *commandHandle,
        uint32_t blockStart,
        uint32_t nBlock
    );

  Summary:
    Erase the specified number of memory blocks from the specified block start.

    Each block is equal to sector size of the memory device attached.

  Description:
    This function schedules a non-blocking sector erase operation on attached
    memory device.

    The function returns with a valid erase handle in the commandHandle argument
    if the erase request was scheduled successfully. The function adds the request
    to the hardware instance queue and returns immediately.

    The function returns DRV_MEMORY_COMMAND_HANDLE_INVALID in the commandHandle
    argument under the following circumstances:
    - if a buffer object could not be allocated to the request
    - if the client opened the driver for read only
    - if the number of blocks to be erased is either zero or more than the number
      of blocks actually available
    - if the erase queue size is full or queue depth is insufficient
    - if the driver handle is invalid

    If the requesting client registered a transfer handler callback with the
    driver will issue a DRV_MEMORY_EVENT_COMMAND_COMPLETE event if the erase
    operation was successful or DRV_MEMORY_EVENT_COMMAND_ERROR event if the
    erase operation was not successful.

    If the requesting client has not registered any transfer handler callback
    with the driver, he can call DRV_MEMORY_CommandStatusGet() API to know
    the current status of the request.

  Preconditions:
    The DRV_MEMORY_Open() must have been called with DRV_IO_INTENT_WRITE or
    DRV_IO_INTENT_READWRITE as a parameter to obtain a valid opened device
    handle.

  Parameters:
    handle        - A valid open-instance handle, returned from the
                    driver's open function

    commandHandle - Pointer to an argument that will contain the return buffer
                    handle

    blockStart    - Block start from where the blocks should be  erased.

    nBlock        - Total number of blocks to be erased.

  Returns:
    The command handle is returned in the commandHandle argument. It Will be
    DRV_MEMORY_COMMAND_HANDLE_INVALID if the request was not queued.

  Example:
    <code>

    // Use DRV_MEMORY_GeometryGet () to find the write region geometry.
    uint32_t blockStart = 0;
    uint32_t nBlocks = 10;
    bool xfer_done = false;

    // memoryHandle is the handle returned by the DRV_MEMORY_Open function.

    // Event is received when the erase request is completed.
    void appTransferHandler
    (
        DRV_MEMORY_EVENT event,
        DRV_MEMORY_COMMAND_HANDLE commandHandle,
        uintptr_t context
    )
    {
        switch(event)
        {
            case DRV_MEMORY_EVENT_COMMAND_COMPLETE:
                xfer_done = true;
                break;
            case DRV_MEMORY_EVENT_COMMAND_ERROR:
                // Handle Error
                break;
            default:
                break;
        }
    }

    DRV_MEMORY_TransferHandlerSet(memoryHandle, appTransferHandler, (uintptr_t)NULL);

    DRV_MEMORY_AsyncErase( memoryHandle, &commandHandle, blockStart, nBlock );

    if(DRV_MEMORY_COMMAND_HANDLE_INVALID == commandHandle)
    {
        // Error handling here
    }

    // Wait for erase to be completed
    while(!xfer_done);

    </code>

  Remarks:
    None.
*/

void DRV_MEMORY_AsyncErase
(
    const DRV_HANDLE handle,
    DRV_MEMORY_COMMAND_HANDLE *commandHandle,
    uint32_t blockStart,
    uint32_t nBlock
);

// **************************************************************************
/* Function:
    bool DRV_MEMORY_SyncErase
    (
        const DRV_HANDLE handle,
        uint32_t blockStart,
        uint32_t nBlock
    );

  Summary:
    Erase the specified number of memory blocks from the specified block start.

    Each block is equal to sector size of the memory device attached.

  Description:
    This function schedules a blocking sector erase operation on attached
    memory device.

  Preconditions:
    The DRV_MEMORY_Open() must have been called with DRV_IO_INTENT_WRITE or
    DRV_IO_INTENT_READWRITE as a parameter to obtain a valid opened device
    handle.

  Parameters:
    handle        - A valid open-instance handle, returned from the
                    driver's open function

    blockStart    - block start from where the blocks should be erased.

    nBlock        - Total number of blocks to be erased.

  Returns:
    true:
        - If the transfer request is successfully completed.

    false:
        - If the client opened the driver for read only
        - If the number of blocks to be erased is either zero or more than the number
          of blocks actually available
        - If the driver handle is invalid

  Example:
    <code>

    // Use DRV_MEMORY_GeometryGet () to find the erase region geometry.
    uint32_t blockStart = 0;
    uint32_t nBlocks = 10;

    // memoryHandle is the handle returned by the DRV_MEMORY_Open function.

    if(DRV_MEMORY_SyncErase( memoryHandle, blockStart, nBlock ) == false)
    {
        // Error handling here
    }

    </code>

  Remarks:
    None.
*/

bool DRV_MEMORY_SyncErase
(
    const DRV_HANDLE handle,
    uint32_t blockStart,
    uint32_t nBlock
);

// *****************************************************************************
/* Function:
    void DRV_MEMORY_AsyncEraseWrite
    (
        const DRV_HANDLE handle,
        DRV_MEMORY_COMMAND_HANDLE * commandHandle,
        void * sourceBuffer,
        uint32_t blockStart,
        uint32_t nBlock
    );

  Summary:
    Erase and Write blocks of data in the sectors where the block start belongs.

  Description:
    This function combines the step of erasing a sector and then writing the
    page. The application can use this function if it wants to avoid having to
    explicitly delete a sector in order to update the pages contained in the
    sector.

    This function schedules a non-blocking operation to erase and write blocks
    of data into attached device memory.

    The function returns with a valid command handle in the commandHandle argument
    if the write request was scheduled successfully. The function adds the request
    to the hardware instance queue and returns immediately.

    While the request is in the queue, the application buffer is owned by the driver
    and should not be modified.

    The function returns DRV_MEMORY_COMMAND_HANDLE_INVALID in the commandHandle
    argument under the following circumstances:
    - if a buffer could not be allocated to the request
    - if the sourceBuffer pointer is NULL
    - if the client opened the driver for read only
    - if the number of blocks to be written is either zero or more than the
      number of blocks actually available
    - if the driver handle is invalid

    If the requesting client registered an event callback with the driver, the
    driver will issue a DRV_MEMORY_EVENT_COMMAND_COMPLETE event if the buffer
    was processed successfully or DRV_MEMORY_EVENT_COMMAND_ERROR event if the
    buffer was not processed successfully.

    If the requesting client has not registered any transfer handler callback
    with the driver, he can call DRV_MEMORY_CommandStatusGet() API to know
    the current status of the request.

  Precondition:
    The DRV_MEMORY_Open() must have been called with DRV_IO_INTENT_WRITE or
    DRV_IO_INTENT_READWRITE as a parameter to obtain a valid opened device
    handle.

  Parameters:
    handle        - A valid open-instance handle, returned from the driver's
                    open function

    commandHandle - Pointer to an argument that will contain the return command
                    handle. If NULL, then command handle is not returned.

    sourceBuffer  - The source buffer containing data to be programmed into media
                    device memory

    blockStart    - Write block start where the write should begin.

    nBlock        - Total number of blocks to be written.

  Returns:
    The command handle is returned in the commandHandle argument. It Will be
    DRV_MEMORY_COMMAND_HANDLE_INVALID if the request was not queued.

  Example:
    <code>

    #define BUFFER_SIZE    4096
    uint8_t buffer[BUFFER_SIZE];

    // Use DRV_MEMORY_GeometryGet () to find the write region geometry.
    uint32_t blockStart = 0x0;
    uint32_t nBlock = BUFFER_SIZE / block_size; // block_size for write geometry
    DRV_MEMORY_COMMAND_HANDLE commandHandle;

    // memoryHandle is the handle returned by the DRV_MEMORY_Open function.
    // Client registers an event handler with driver

    // Event is received when the erase request is completed.
    void appTransferHandler
    (
        DRV_MEMORY_EVENT event,
        DRV_MEMORY_COMMAND_HANDLE commandHandle,
        uintptr_t context
    )
    {
        switch(event)
        {
            case DRV_MEMORY_EVENT_COMMAND_COMPLETE:
                xfer_done = true;
                break;
            case DRV_MEMORY_EVENT_COMMAND_ERROR:
                // Handle Error
                break;
            default:
                break;
        }
    }

    DRV_MEMORY_TransferHandlerSet(memoryHandle, appTransferHandler, (uintptr_t)NULL);

    DRV_MEMORY_AsyncEraseWrite(memoryHandle, &commandHandle, &myBuffer, blockStart, nBlock);

    if(DRV_MEMORY_COMMAND_HANDLE_INVALID == commandHandle)
    {
        // Error handling here
    }

    // Wait for erase to be completed
    while(!xfer_done);

    </code>

  Remarks:
    None.
*/

void DRV_MEMORY_AsyncEraseWrite
(
    const DRV_HANDLE handle,
    DRV_MEMORY_COMMAND_HANDLE *commandHandle,
    void *sourceBuffer,
    uint32_t blockStart,
    uint32_t nBlock
);

// *****************************************************************************
/* Function:
    bool DRV_MEMORY_SyncEraseWrite
    (
        const DRV_HANDLE handle,
        void *sourceBuffer,
        uint32_t blockStart,
        uint32_t nBlock
    );

  Summary:
    Erase and Write blocks of data in the sectors where the block start belongs.

  Description:
    This function combines the step of erasing a sector and then writing the
    page. The application can use this function if it wants to avoid having to
    explicitly delete a sector in order to update the pages contained in the
    sector.

    This function schedules a blocking operation to erase and write blocks
    of data into attached device memory.

  Precondition:
    The DRV_MEMORY_Open() must have been called with DRV_IO_INTENT_WRITE or
    DRV_IO_INTENT_READWRITE as a parameter to obtain a valid opened device
    handle.

  Parameters:
    handle        - A valid open-instance handle, returned from the driver's
                    open function

    sourceBuffer  - The source buffer containing data to be programmed into media
                    device memory

    blockStart    - block start where the write should begin.

    nBlock        - Total number of blocks to be written.

  Returns:
    true:
        - If the transfer request is successfully completed.

    false:
        - If the sourceBuffer pointer is NULL
        - If the client opened the driver for read only
        - If the number of blocks to be written is either zero or more than the
          number of blocks actually available
        - If the driver handle is invalid

  Example:
    <code>

    #define BUFFER_SIZE    4096

    uint8_t buffer[BUFFER_SIZE];

    // Use DRV_MEMORY_GeometryGet () to find the write region geometry.
    uint32_t blockStart = 0x0;
    uint32_t nBlock = BUFFER_SIZE / block_size; // block_size for write geometry

    // memoryHandle is the handle returned by the DRV_MEMORY_Open function.

    if(DRV_MEMORY_SyncEraseWrite(memoryHandle, &myBuffer, blockStart, nBlock) == false)
    {
        // Error handling here
    }

    </code>

  Remarks:
    None.
*/

bool DRV_MEMORY_SyncEraseWrite
(
    const DRV_HANDLE handle,
    void *sourceBuffer,
    uint32_t blockStart,
    uint32_t nBlock
);

// *****************************************************************************
/* Function:
    void DRV_MEMORY_AsyncWrite
    (
        const DRV_HANDLE handle,
        DRV_MEMORY_COMMAND_HANDLE *commandHandle,
        void *sourceBuffer,
        uint32_t blockStart,
        uint32_t nBlock
    );

  Summary:
    Writes nblocks of data starting at the specified block start.

  Description:
    This function schedules a non-blocking write operation for writing blocks
    of data into attached devices memory.

    The function returns with a valid command handle in the commandHandle argument
    if the write request was scheduled successfully.

    The function adds the request to the hardware instance queue and returns immediately.
    While the request is in the queue, the application buffer is owned by the driver
    and should not be modified.

    The function returns DRV_MEMORY_COMMAND_HANDLE_INVALID in the commandHandle argument
    under the following circumstances:
    - if a buffer object could not be allocated to the request
    - if the source buffer pointer is NULL
    - if the client opened the driver for read only
    - if the number of blocks to be written is either zero or more than the
      number of blocks actually available
    - if the driver handle is invalid

    If the requesting client registered an event callback with the driver, the
    driver will issue a DRV_MEMORY_EVENT_COMMAND_COMPLETE event if the buffer
    was processed successfully or DRV_MEMORY_EVENT_COMMAND_ERROR event if the
    buffer was not processed successfully.

    If the requesting client has not registered any transfer handler callback
    with the driver, he can call DRV_MEMORY_CommandStatusGetGet() API to know
    the current status of the request.

  Preconditions:
    DRV_MEMORY_Open() routine must have been called to obtain a valid opened device
    handle.

    The memory address location which has to be written, must have been erased
    before using the DRV_MEMORY_xxxErase() routine.

  Parameters:
    handle        - A valid open-instance handle, returned from the driver's
                    open function

    commandHandle - Pointer to an argument that will contain the return buffer
                    handle

    sourceBuffer  - The source buffer containing data to be programmed into media
                    device memory

    blockStart    - Block start from where the data should be written to.

    nBlock        - Total number of blocks to be written.

  Returns:
    The command handle is returned in the commandHandle argument. It will be
    DRV_MEMORY_COMMAND_HANDLE_INVALID if the request was not successful.

  Example:
    <code>

    #define BUFFER_SIZE    4096
    uint8_t writeBuffer[BUFFER_SIZE];

    // Use DRV_MEMORY_GeometryGet () to find the write region geometry.
    uint32_t blockStart = 0x0;
    uint32_t nBlock = BUFFER_SIZE / block_size; // block_size for write geometry
    bool xfer_done = false;

    DRV_MEMORY_COMMAND_HANDLE commandHandle;

    // memoryHandle is the handle returned by the DRV_MEMORY_Open function.

    // Event is received when the write request is completed.
    void appTransferHandler
    (
        DRV_MEMORY_EVENT event,
        DRV_MEMORY_COMMAND_HANDLE commandHandle,
        uintptr_t context
    )
    {
        switch(event)
        {
            case DRV_MEMORY_EVENT_COMMAND_COMPLETE:
                xfer_done = true;
                break;
            case DRV_MEMORY_EVENT_COMMAND_ERROR:
                // Handle Error
                break;
            default:
                break;
        }
    }

    DRV_MEMORY_TransferHandlerSet(memoryHandle, appTransferHandler, (uintptr_t)NULL);

    DRV_MEMORY_AsyncErase(memoryHandle, &commandHandle, blockStart, nBlock);

    if(DRV_MEMORY_COMMAND_HANDLE_INVALID == commandHandle)
    {
        // Error handling here
    }

    // Wait for erase to be completed
    while(!xfer_done);

    DRV_MEMORY_AsyncWrite(memoryHandle, &commandHandle, &writeBuffer, blockStart, nBlock);

    if(DRV_MEMORY_COMMAND_HANDLE_INVALID == commandHandle)
    {
        // Error handling here
    }

    // Wait for write to be completed
    while(!xfer_done);

    </code>

  Remarks:
    None.
*/

void DRV_MEMORY_AsyncWrite
(
    const DRV_HANDLE handle,
    DRV_MEMORY_COMMAND_HANDLE *commandHandle,
    void *sourceBuffer,
    uint32_t blockStart,
    uint32_t nBlock
);

// *****************************************************************************
/* Function:
    bool DRV_MEMORY_SyncWrite
    (
        const DRV_HANDLE handle,
        void *sourceBuffer,
        uint32_t blockStart,
        uint32_t nBlock
    );

  Summary:
    Writes nblock of data starting at the specified block start.

  Description:
    This function schedules a blocking write operation for writing blocks
    of data into attached devices memory.

  Preconditions:
    DRV_MEMORY_Open() routine must have been called to obtain a valid opened device
    handle.

    The memory block which has to be written, must have been erased
    before using the DRV_MEMORY_xxxErase() routine.

  Parameters:
    handle        - A valid open-instance handle, returned from the driver's
                    open function

    sourceBuffer  - The source buffer containing data to be programmed into media
                    device memory

    blockStart    - Block start from where the data should be written to.

    nBlock        - Total number of blocks to be written.

  Returns:
    true:
        - If the transfer request is successfully completed.

    false:
        - If the source buffer pointer is NULL
        - If the client opened the driver for read only
        - If the number of blocks to be written is either zero or more than the
          number of blocks actually available
        - If the driver handle is invalid

  Example:
    <code>

    #define BUFFER_SIZE    4096
    uint8_t writeBuffer[BUFFER_SIZE];

    // Use DRV_MEMORY_GeometryGet () to find the write region geometry.
    uint32_t blockStart = 0x0;
    uint32_t nBlock = BUFFER_SIZE / block_size; // block_size for write geometry

    // memoryHandle is the handle returned by the DRV_MEMORY_Open function.

    if(DRV_MEMORY_SyncErase(memoryHandle, blockStart, nBlock) == false)
    {
        // Error handling here
    }

    if(DRV_MEMORY_SyncWrite(memoryHandle, &writeBuffer, blockStart, nBlock) == false)
    {
        // Error handling here
    }

    </code>

  Remarks:
    None.
*/

bool DRV_MEMORY_SyncWrite
(
    const DRV_HANDLE handle,
    void *sourceBuffer,
    uint32_t blockStart,
    uint32_t nBlock
);

// *****************************************************************************
/* Function:
    void DRV_MEMORY_AsyncRead
    (
        const DRV_HANDLE handle,
        DRV_MEMORY_COMMAND_HANDLE *commandHandle,
        void *targetBuffer,
        uint32_t blockStart,
        uint32_t nBlock
    );

Summary:
    Reads nblocks of data from the specified block start.

  Description:
    This function schedules a non-blocking read operation for reading blocks of
    data from the memory device attached.

    The function returns with a valid command handle in the commandHandle argument
    if the request was scheduled successfully.

    The function adds the request to the hardware instance queue and returns
    immediately. While the request is in the queue, the application
    buffer is owned by the driver and should not be modified.

    The function returns DRV_MEMORY_COMMAND_HANDLE_INVALID in the commandHandle argument
    under the following circumstances:
    - if a buffer object could not be allocated to the request
    - if the target buffer pointer is NULL
    - if the client opened the driver for write only
    - if the number of blocks to be read is either zero or more than the number
      of blocks actually available
    - if the driver handle is invalid

  Precondition:
    DRV_MEMORY_Open() must have been called with DRV_IO_INTENT_READ or
    DRV_IO_INTENT_READWRITE as the ioIntent to obtain a valid opened device
    handle.

  Parameters:
    handle        - A valid open-instance handle, returned from the driver's
                    open function

    commandHandle - Pointer to an argument that will contain the command handle

    targetBuffer  - Buffer into which the data read from the media device memory
                    will be placed

    blockStart    - Block start from where the data should be read.

    nBlock        - Total number of blocks to be read.

  Returns:
    The command handle is returned in the commandHandle argument. It will be
    DRV_MEMORY_COMMAND_HANDLE_INVALID if the request was not successful.

  Example:
    <code>

    uint8_t readBuffer[BUFFER_SIZE];

    // Use DRV_MEMORY_GeometryGet () to find the read region geometry.
    // Find the block address from which to read data.
    uint32_t blockStart = 0x0;
    uint32_t nBlock = BUFFER_SIZE;
    DRV_MEMORY_COMMAND_HANDLE commandHandle;
    bool xfer_done = false;

    // memoryHandle is the handle returned by the DRV_MEMORY_Open function.

     // Event is received when the write request is completed.
    void appTransferHandler
    (
        DRV_MEMORY_EVENT event,
        DRV_MEMORY_COMMAND_HANDLE commandHandle,
        uintptr_t context
    )
    {
        switch(event)
        {
            case DRV_MEMORY_EVENT_COMMAND_COMPLETE:
                xfer_done = true;
                break;
            case DRV_MEMORY_EVENT_COMMAND_ERROR:
                // Handle Error
                break;
            default:
                break;
        }
    }

    DRV_MEMORY_TransferHandlerSet(memoryHandle, appTransferHandler, (uintptr_t)NULL);

    DRV_MEMORY_AsyncRead(memoryHandle, &commandHandle, &readBuffer, blockStart, nBlock);

    if(DRV_MEMORY_COMMAND_HANDLE_INVALID == commandHandle)
    {
        // Error handling here
    }

    while(!xfer_done);

    </code>

  Remarks:
    None.
*/

void DRV_MEMORY_AsyncRead
(
    const DRV_HANDLE handle,
    DRV_MEMORY_COMMAND_HANDLE *commandHandle,
    void *targetBuffer,
    uint32_t blockStart,
    uint32_t nBlock
);

// *****************************************************************************
/* Function:
    bool DRV_MEMORY_SyncRead
    (
        const DRV_HANDLE handle,
        void *targetBuffer,
        uint32_t blockStart,
        uint32_t nBlock
    );

Summary:
    Reads nblock of data from the specified block start.

  Description:
    This function schedules a blocking read operation for reading blocks of
    data from the memory device attached.

  Precondition:
    DRV_MEMORY_Open() must have been called with DRV_IO_INTENT_READ or
    DRV_IO_INTENT_READWRITE as the ioIntent to obtain a valid opened device
    handle.

  Parameters:
    handle        - A valid open-instance handle, returned from the driver's
                    open function

    targetBuffer  - Buffer into which the data read from the media device memory
                    will be placed

    blockStart    - Block start from where the data should be read.

    nBlock        - Total number of blocks to be read.

  Returns:
    true:
        - If the transfer request is successfully completed.

    false:
        - If the target buffer pointer is NULL
        - If the client opened the driver for write only
        - If the number of blocks to be read is either zero or more than the number
          of blocks actually available
        - If the driver handle is invalid

  Example:
    <code>

    #define BUFFER_SIZE    4096
    uint8_t readBuffer[BUFFER_SIZE];

    // Use DRV_MEMORY_GeometryGet () to find the read region geometry.
    uint32_t blockStart = 0x0;
    uint32_t nBlock = BUFFER_SIZE / block_size; // block_size for read geometry

    // memoryHandle is the handle returned by the DRV_MEMORY_Open function.

    if(DRV_MEMORY_SyncRead(memoryHandle, &readBuffer, blockStart, nBlock) == false)
    {
        // Error handling here
    }

    </code>

  Remarks:
    None.
*/

bool DRV_MEMORY_SyncRead
(
    const DRV_HANDLE handle,
    void *targetBuffer,
    uint32_t blockStart,
    uint32_t nBlock
);

// *****************************************************************************
/* Function:
    SYS_MEDIA_GEOMETRY * DRV_MEMORY_GeometryGet
    (
        const DRV_HANDLE handle
    );

  Summary:
    Returns the geometry of the memory device.

  Description:
    This API gives the following geometrical details of the attached memory device:
    - Media Property
    - Number of Read/Write/Erase regions in the memory device
    - Number of Blocks and their size in each region of the device

  Precondition:
    The DRV_MEMORY_Open() routine must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open function

  Returns:
    SYS_MEDIA_GEOMETRY - Pointer to structure which holds the media geometry information.

  Example:
    <code>

    SYS_MEDIA_GEOMETRY geometry;
    uint32_t readBlockSize, writeBlockSize, eraseBlockSize;
    uint32_t nReadBlocks, nReadRegions, totalFlashSize;

    if (true != DRV_MEMORY_GeometryGet(&geometry))
    {
        // Handle Error
    }

    readBlockSize  = geometry.geometryTable[0].blockSize;
    nReadBlocks = geometry.geometryTable[0].numBlocks;
    nReadRegions = geometry.numReadRegions;

    writeBlockSize  = geometry.geometryTable[1].blockSize;
    eraseBlockSize  = geometry.geometryTable[2].blockSize;

    totalFlashSize = readBlockSize * nReadBlocks * nReadRegions;

    </code>

  Remarks:
    Refer sys_media.h for definition of SYS_MEDIA_GEOMETRY.
*/

SYS_MEDIA_GEOMETRY * DRV_MEMORY_GeometryGet
(
    const DRV_HANDLE handle
);

// *****************************************************************************
/* Function:
    MEMORY_DEVICE_TRANSFER_STATUS DRV_MEMORY_TransferStatusGet
    (
        const DRV_HANDLE handle
    );

  Summary:
    Gets the current status of the transfer request on attached device.

  Description:
    This routine gets the current status of the transfer request. The application
    must use this routine where the status of a scheduled request needs to be
    polled on.

    The application can alternatively register a transfer handler to receive the
    transfer completion events.

  Preconditions:
    DRV_MEMORY_Open() must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open function

  Returns:
    MEMORY_DEVICE_TRANSFER_ERROR_UNKNOWN
        - If the handle is invalid
        - If the status read request fails

    MEMORY_DEVICE_TRANSFER_BUSY
        - If the current transfer request is still being processed

    MEMORY_DEVICE_TRANSFER_COMPLETED
        - If the transfer request is completed

  Example:
    <code>

    // memoryHandle is the handle returned by the DRV_MEMORY_Open function.

    if (MEMORY_DEVICE_TRANSFER_COMPLETED == DRV_MEMORY_TransferStatusGet(memoryHandle))
    {
        // Operation Done
    }
    </code>

  Remarks:
    This routine will block for hardware access.

    Used in Async mode of operation.
*/

MEMORY_DEVICE_TRANSFER_STATUS DRV_MEMORY_TransferStatusGet
(
    const DRV_HANDLE handle
);

// *****************************************************************************
/* Function:
    DRV_MEMORY_COMMAND_STATUS DRV_MEMORY_CommandStatusGet
    (
        const DRV_HANDLE handle,
        const DRV_MEMORY_COMMAND_HANDLE commandHandle
    );

  Summary:
    Gets the current status of the command.

  Description:
    This routine gets the current status of the buffer. The application must use
    this routine where the status of a scheduled buffer needs to polled on.

    The function may return DRV_MEMORY_COMMAND_HANDLE_INVALID in a case where the
    buffer handle has expired. A buffer handle expires when the internal buffer object
    is re-assigned to another erase or write request. It is recommended that this
    function be called regularly in order to track the buffer status correctly.

    The application can alternatively register an event handler to receive write
    or erase operation completion events.

  Preconditions:
    DRV_MEMORY_Open() must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle        - A valid open-instance handle, returned from the driver's
                    open function

    commandHandle - Pointer to an argument that will contain the command handle

  Returns:
    DRV_MEMORY_COMMAND_COMPLETED
        - If the transfer request is completed

    DRV_MEMORY_COMMAND_QUEUED
        - If the command is Queued and waiting to be processed.

    DRV_MEMORY_COMMAND_IN_PROGRESS
    - If the current transfer request is still being processed

    DRV_MEMORY_COMMAND_ERROR_UNKNOWN
        - If the handle is invalid
        - If the status read request fails

  Example:
    <code>

     // memoryHandle is the handle returned by the DRV_MEMORY_Open function.
     // commandHandle is the handle returned by any read/write/erase block operation.

    if (DRV_MEMORY_COMMAND_COMPLETED == DRV_MEMORY_CommandStatusGet(memoryHandle, commandHandle))
    {
        // Operation Done
    }

    </code>

  Remarks:
    Used in Async mode of operation.

*/

DRV_MEMORY_COMMAND_STATUS DRV_MEMORY_CommandStatusGet
(
    const DRV_HANDLE handle,
    const DRV_MEMORY_COMMAND_HANDLE commandHandle
);

// *****************************************************************************
/* Function:
    void DRV_MEMORY_TransferHandlerSet
    (
        const DRV_HANDLE handle,
        const void * transferHandler,
        const uintptr_t context
    );

  Summary:
    Allows a client to identify an event handling function for the driver to
    call back when queued operation has completed.

  Description:
    This function allows a client to set an event handling function for
    the driver to call back when queued operation has completed.

    When a client calls a read, write, erase or a erasewrite function, it is
    provided with a handle identifying the command that was added to the driver's
    buffer queue. The driver will pass this handle back to the client by calling
    "transferHandler" function when the queued operation has completed.

    The event handler should be set before the client performs any operations
    that could generate events. The event handler once set, persists until the
    client closes the driver or sets another event handler (which could be a
    "NULL" pointer to indicate no callback).

  Precondition:
    The DRV_MEMORY_Open() routine must have been called to obtain a valid opened
    device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open function

    transferHandler - Pointer to the event handler function implemented by the
                   user

    context      - The value of parameter will be passed back to the client
                   unchanged, when the transferHandler function is called. It can
                   be used to identify any client specific data object that
                   identifies the instance of the client module (for example,
                   it may be a pointer to the client module's state structure).

  Returns:
    None.

  Example:
    <code>
    DRV_MEMORY_COMMAND_HANDLE commandHandle;

    // memoryHandle is the handle returned by the DRV_MEMORY_Open function.
    // Client registers an event handler with driver

    // Event is received when the erase request is completed.
    void appTransferHandler
    (
        DRV_MEMORY_EVENT event,
        DRV_MEMORY_COMMAND_HANDLE commandHandle,
        uintptr_t context
    )
    {
        switch(event)
        {
            case DRV_MEMORY_EVENT_COMMAND_COMPLETE:
                xfer_done = true;
                break;
            case DRV_MEMORY_EVENT_COMMAND_ERROR:
                // Handle Error
                break;
            default:
                break;
        }
    }

    DRV_MEMORY_TransferHandlerSet(memoryHandle, appTransferHandler, (uintptr_t)NULL);

    </code>

  Remarks:
    If the client does not want to be notified when the queued operation has
    completed, it does not need to register a callback.

    Used in Asynchronous Mode of operation.
*/

void DRV_MEMORY_TransferHandlerSet
(
    const DRV_HANDLE handle,
    const void * transferHandler,
    const uintptr_t context
);

// *****************************************************************************
/* Function:
    bool DRV_MEMORY_IsAttached
    (
        const DRV_HANDLE handle
    );

  Summary:
    Returns the physical attach status of the Media.

  Description:
    This function returns the physical attach status of the Media device.

  Precondition:
    The DRV_MEMORY_Open() routine must have been called to obtain a valid opened
    device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open function

  Returns:
    Returns false if the handle is invalid otherwise returns true.

  Example:
    <code>

    bool isMEMORYAttached;
    isMEMORYAttached = DRV_MEMORY_isAttached(drvMEMORYHandle);

    </code>

  Remarks:
    None.
*/

bool DRV_MEMORY_IsAttached
(
    const DRV_HANDLE handle
);

// *****************************************************************************
/* Function:
    bool DRV_MEMORY_IsWriteProtected
    (
        const DRV_HANDLE handle
    );

  Summary:
    Returns the write protect status of the Memory.

  Description:
    This function returns the write protect status of the Memory.

  Precondition:
    The DRV_MEMORY_Open() routine must have been called to obtain a valid opened
    device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open function

  Returns:
    True - If the memory is write protected.
    False - If the memory is not write protected.

  Example:
    <code>

    bool isWriteProtected;
    isWriteProtected = DRV_MEMORY_IsWriteProtected(drvMEMORYHandle);

    </code>

  Remarks:
    None.
*/

bool DRV_MEMORY_IsWriteProtected
(
    const DRV_HANDLE handle
);

// *****************************************************************************
/* Function:
    uintptr_t DRV_MEMORY_AddressGet
    (
        const DRV_HANDLE handle
    );

  Summary:
    Returns the Memory media start address

  Description:
    This function returns the Memory Media start address.

  Precondition:
    The DRV_MEMORY_Open() routine must have been called to obtain a valid opened
    device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open function

  Returns:
    Start address of the Memory Media if the handle is valid otherwise NULL.

  Example:
    <code>

    uintptr_t startAddress;
    startAddress = DRV_MEMORY_AddressGet(drvMEMORYHandle);

    </code>

  Remarks:
    None.
*/

uintptr_t DRV_MEMORY_AddressGet
(
    const DRV_HANDLE handle
);

// *****************************************************************************
/* Function:
    void DRV_MEMORY_Erase
    (
        const DRV_HANDLE handle,
        SYS_MEDIA_BLOCK_COMMAND_HANDLE* commandHandle,
        void* sourceBuffer,
        uint32_t blockStart,
        uint32_t nBlock
    )

  Summary:
    This routine provides interface to the file system to perform a media
    erase operation in synchronous mode of the Memory driver.

  Description:
    This function is internally used by the file system.

  Preconditions:
    The DRV_MEMORY_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle        - A valid open-instance handle, returned from the driver's
                    open function

    commandHandle - Pointer to an argument that will contain the return buffer
                    handle

    blockStart    - Start block address of SD Card where the writes should begin.

    nBlock        - Total number of blocks to be written.

  Returns:
    The buffer handle is returned in the commandHandle argument. It will be
    DRV_MEMORY_COMMAND_HANDLE_INVALID if the request was not successful.

  Example:
    None.

  Remarks:
    This function is internally used by the file system.
*/

void DRV_MEMORY_Erase
(
    const DRV_HANDLE handle,
    SYS_MEDIA_BLOCK_COMMAND_HANDLE *commandHandle,
    uint32_t blockStart,
    uint32_t nBlock
);

// *****************************************************************************
/* Function:
    void DRV_MEMORY_EraseWrite
    (
        const DRV_HANDLE handle,
        SYS_MEDIA_BLOCK_COMMAND_HANDLE* commandHandle,
        void* sourceBuffer,
        uint32_t blockStart,
        uint32_t nBlock
    )

  Summary:
    This routine provides interface to the file system to perform a media
    erase-write operation in synchronous mode of the Memory driver.

  Description:
    This function is internally used by the file system.

  Preconditions:
    The DRV_MEMORY_Open must have been called to obtain a valid opened device handle.

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
    DRV_MEMORY_COMMAND_HANDLE_INVALID if the request was not successful.

  Example:
    None.

  Remarks:
    This function is internally used by the file system.
*/

void DRV_MEMORY_EraseWrite
(
    const DRV_HANDLE handle,
    SYS_MEDIA_BLOCK_COMMAND_HANDLE *commandHandle,
    void *sourceBuffer,
    uint32_t blockStart,
    uint32_t nBlock
);

// *****************************************************************************
/* Function:
    void DRV_MEMORY_Write
    (
        const DRV_HANDLE handle,
        SYS_MEDIA_BLOCK_COMMAND_HANDLE* commandHandle,
        void* sourceBuffer,
        uint32_t blockStart,
        uint32_t nBlock
    )

  Summary:
    This routine provides interface to the file system to perform a media
    write operation in synchronous mode of the Memory driver.

  Description:
    This function is internally used by the file system.

  Preconditions:
    The DRV_MEMORY_Open must have been called to obtain a valid opened device handle.

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
    DRV_MEMORY_COMMAND_HANDLE_INVALID if the request was not successful.

  Example:
    None.

  Remarks:
    This function is internally used by the file system.
*/

void DRV_MEMORY_Write
(
    const DRV_HANDLE handle,
    SYS_MEDIA_BLOCK_COMMAND_HANDLE *commandHandle,
    void *sourceBuffer,
    uint32_t blockStart,
    uint32_t nBlock
);

// *****************************************************************************
/* Function:
    void DRV_MEMORY_Read
    (
        const DRV_HANDLE handle,
        SYS_MEDIA_BLOCK_COMMAND_HANDLE* commandHandle,
        void* sourceBuffer,
        uint32_t blockStart,
        uint32_t nBlock
    )

  Summary:
    This routine provides interface to the file system to perform a media
    read operation in synchronous mode of the Memory driver.

  Description:
    This function is internally used by the file system.

  Preconditions:
    The DRV_MEMORY_Open must have been called to obtain a valid opened device handle.

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
    DRV_MEMORY_COMMAND_HANDLE_INVALID if the request was not successful.

  Example:
    None.

  Remarks:
    This function is internally used by the file system.
*/
void DRV_MEMORY_Read
(
    const DRV_HANDLE handle,
    SYS_MEDIA_BLOCK_COMMAND_HANDLE *commandHandle,
    void *targetBuffer,
    uint32_t blockStart,
    uint32_t nBlock
);

#ifdef __cplusplus
}
#endif

#include "driver/memory/src/drv_memory_local.h"

#endif // #ifndef _DRV_MEMORY_H
/*******************************************************************************
 End of File
*/
