/*******************************************************************************
  SST26 Driver Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    drv_sst26.h

  Summary:
    SST26 Driver Interface Definition

  Description:
    The SST26 driver provides a simple interface to manage the SST26VF series
    of SQI Flash Memory connected to Microchip microcontrollers. This file
    defines the interface definition for the SST26 driver.
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

#ifndef _DRV_SST26_H
#define _DRV_SST26_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "drv_sst26_definitions.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
    extern "C" {
#endif

// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

/*
 Summary:
    SST26 Driver Transfer Status

 Description:
    This data type will be used to indicate the current transfer status for SST26
    driver.

 Remarks:
    None.
*/

typedef enum
{
    /* Transfer is being processed */
    DRV_SST26_TRANSFER_BUSY,
    /* Transfer is successfully completed*/
    DRV_SST26_TRANSFER_COMPLETED,
    /* Transfer had error*/
    DRV_SST26_TRANSFER_ERROR_UNKNOWN,
} DRV_SST26_TRANSFER_STATUS;

/*
 Summary:
    SST26 Device Geometry data.

 Description:
    This data type will be used to get the geometry details of the
    SST26 flash device.

 Remarks:
    None.
*/

typedef struct
{
    uint32_t read_blockSize;
    uint32_t read_numBlocks;
    uint32_t numReadRegions;

    uint32_t write_blockSize;
    uint32_t write_numBlocks;
    uint32_t numWriteRegions;

    uint32_t erase_blockSize;
    uint32_t erase_numBlocks;
    uint32_t numEraseRegions;

    uint32_t blockStartAddress;
} DRV_SST26_GEOMETRY;

// *****************************************************************************
// *****************************************************************************
// Section: SST26 Driver Module Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* SST26 Driver Transfer Event Handler Function Pointer

   Summary
    Pointer to a SST26 Driver Event handler function

   Description
    This data type defines the required function signature for the SST26 driver
    event handling callback function. A client must register a pointer
    using the event handling function whose function signature (parameter
    and return value types) match the types specified by this function pointer
    in order to receive transfer related event calls back from the driver.

    This data type is only supported when sst26 driver is using
    - QSPI PLIB in SPI mode
    - SPI PLIB

  Parameters:
    event - Identifies the type of event

    context - Value identifying the context of the application that
    registered the event handling function.

  Returns:
    None.

  Example:
    <code>
    void APP_MyTransferEventHandler( DRV_SST26_TRANSFER_STATUS event, uintptr_t context )
    {
        MY_APP_DATA_STRUCT* pAppData = (MY_APP_DATA_STRUCT*) context;

        switch(event)
        {
            case DRV_SST26_TRANSFER_COMPLETED:

                // Handle the transfer complete event.
                break;

            case DRV_SST26_TRANSFER_ERROR_UNKNOWN:
            default:

                // Handle error.
                break;
        }
    }
    </code>

  Remarks:
    If the event is DRV_SST26_TRANSFER_COMPLETED, it means that the data was
    transferred successfully.

    If the event is DRV_SST26_TRANSFER_ERROR_UNKNOWN, it means that the data was not
    transferred successfully.

    The context parameter contains the handle to the client context,
    provided at the time the event handling function was registered using the
    DRV_SST26_EventHandlerSet function.  This context handle value is
    passed back to the client as the "context" parameter.  It can be any value
    necessary to identify the client context or instance (such as a pointer to
    the client's data) instance of the client that made the buffer add request.

    The event handler function executes in the driver's interrupt
    context. It is recommended of the application to not perform process
    intensive or blocking operations with in this function.
*/

typedef void ( *DRV_SST26_EVENT_HANDLER )( DRV_SST26_TRANSFER_STATUS event, uintptr_t context );

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_SST26_Initialize
    (
        const SYS_MODULE_INDEX drvIndex,
        const SYS_MODULE_INIT *const init
    );

  Summary:
    Initializes the SST26 Driver

  Description:
    This routine initializes the SST26 driver making it ready for client to use.

  Precondition:
    None.

  Parameters:
    drvIndex -  Identifier for the instance to be initialized

    init     -  Pointer to a data structure containing any data necessary to
                initialize the driver.

  Returns:
    If successful, returns a valid driver instance object.
    Otherwise it returns SYS_MODULE_OBJ_INVALID.

  Example:
    <code>
    // This code snippet shows an example of initializing the SST26 Driver
    // with SST26 QSPI flash device attached.

    SYS_MODULE_OBJ  objectHandle;

    const DRV_SST26_PLIB_INTERFACE drvSST26PlibAPI = {
        .CommandWrite  = QSPI_CommandWrite,
        .RegisterRead  = QSPI_RegisterRead,
        .RegisterWrite = QSPI_RegisterWrite,
        .MemoryRead    = QSPI_MemoryRead,
        .MemoryWrite   = QSPI_MemoryWrite
    };

    const DRV_SST26_INIT drvSST26InitData =
    {
        .sst26Plib         = &drvSST26PlibAPI,
    };

    objectHandle = DRV_SST26_Initialize((SYS_MODULE_INDEX)DRV_SST26_INDEX, (SYS_MODULE_INIT *)&drvSST26InitData);

    if (SYS_MODULE_OBJ_INVALID == objectHandle)
    {
        // Handle error
    }
    </code>

  Remarks:
    This routine must be called before any other SST26 driver routine is called.

    This routine should only be called once during system initialization.
*/

SYS_MODULE_OBJ DRV_SST26_Initialize
(
    const SYS_MODULE_INDEX drvIndex,
    const SYS_MODULE_INIT *const init
);

// ****************************************************************************
/* Function:
    DRV_HANDLE DRV_SST26_Open( const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT ioIntent );

  Summary:
    Opens the specified SST26 driver instance and returns a handle to it

  Description:
    This routine opens the specified SST26 driver instance and provides a handle.

    It performs the following blocking operations:
    - Resets the Flash Device
    - Puts it on QUAD IO Mode
    - Unlocks the flash if DRV_SST26_Open was called with write intent.

    This handle must be provided to all other client-level operations to identify
    the caller and the instance of the driver.

  Preconditions:
    Function DRV_SST26_Initialize must have been called before calling this
    function.

    Driver should be in ready state to accept the request. Can be checked by
    calling DRV_SST26_Status().

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
        - if the driver hardware instance being opened is not initialized.

  Example:
    <code>
    DRV_HANDLE handle;

    handle = DRV_SST26_Open(DRV_SST26_INDEX);
    if (DRV_HANDLE_INVALID == handle)
    {
        // Unable to open the driver
    }
    </code>

  Remarks:
    The handle returned is valid until the DRV_SST26_Close routine is called.

    If the driver has already been opened, it should not be opened again.
*/

DRV_HANDLE DRV_SST26_Open( const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT ioIntent );

// *****************************************************************************
/* Function:
    void DRV_SST26_Close( const DRV_HANDLE handle );

  Summary:
    Closes an opened-instance of the SST26 driver

  Description:
    This routine closes an opened-instance of the SST26 driver, invalidating
    the handle.

  Precondition:
    DRV_SST26_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    None

  Example:
    <code>
    DRV_HANDLE handle;  // Returned from DRV_SST26_Open

    DRV_SST26_Close(handle);
    </code>

  Remarks:
    After calling this routine, the handle passed in "handle" must not be used
    with any of the remaining driver routines. A new handle must be obtained by
    calling DRV_SST26_Open before the caller may use the driver again.

    Usually there is no need for the driver client to verify that the Close
    operation has completed.
*/

void DRV_SST26_Close( const DRV_HANDLE handle );

// *************************************************************************
/* Function:
    SYS_STATUS DRV_SST26_Status( const SYS_MODULE_INDEX drvIndex );

  Summary:
    Gets the current status of the SST26 driver module.

  Description:
    This routine provides the current status of the SST26 driver module.

  Preconditions:
    Function DRV_SST26_Initialize should have been called before calling
    this function.

  Parameters:
    drvIndex   -  Identifier for the instance used to initialize driver

  Returns:
    SYS_STATUS_READY - Indicates that the driver is ready and accept
    requests for new operations.

    SYS_STATUS_UNINITIALIZED - Indicates the driver is not initialized.

    SYS_STATUS_BUSY - Indicates the driver is in busy state.

  Example:
    <code>
    SYS_STATUS          Status;

    Status = DRV_SST26_Status(DRV_SST26_INDEX);
    </code>

  Remarks:
    None.
*/

SYS_STATUS DRV_SST26_Status( const SYS_MODULE_INDEX drvIndex );

// *****************************************************************************
/* Function:
    bool DRV_SST26_UnlockFlash( const DRV_HANDLE handle );

  Summary:
    Unlocks the flash device for Erase and Program operations.

  Description:
    This function schedules a blocking operation for unlocking the flash blocks
    globally. This allows to perform erase and program operations on the flash.

    The request is sent in QUAD_MODE to flash device.

  Precondition:
    The DRV_SST26_Open() routine must have been called for the
    specified SST26 driver instance.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    false
    - if Write enable fails before sending unlock command to flash
    - if Unlock flash command itself fails

    true
    - if the unlock is successfully completed

  Example:
    <code>
    DRV_HANDLE handle;  // Returned from DRV_SST26_Open

    if(true != DRV_SST26_UnlockFlash(handle))
    {
        // Error handling here
    }

    </code>

  Remarks:
    None.
*/

bool DRV_SST26_UnlockFlash( const DRV_HANDLE handle );

// *****************************************************************************
/* Function:
    bool DRV_SST26_ReadJedecId( const DRV_HANDLE handle, void *jedec_id );

  Summary:
    Reads JEDEC-ID of the flash device.

  Description:
    This function schedules a blocking operation for reading the JEDEC-ID.
    This information can be used to get the flash device geometry.

    The request is sent in QUAD_MODE to flash device.

  Precondition:
    The DRV_SST26_Open() routine must have been called for the
    specified SST26 driver instance.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    false
    - if read jedec-id command fails

    true
    - if the read is successfully completed

  Example:
    <code>

    DRV_HANDLE handle;  // Returned from DRV_SST26_Open
    uint32_t jedec_id = 0;

    if(true != DRV_SST26_ReadJedecId(handle, &jedec_id))
    {
        // Error handling here
    }

    </code>

  Remarks:
    None.
*/

bool DRV_SST26_ReadJedecId( const DRV_HANDLE handle, void *jedec_id );

// **************************************************************************
/* Function:
    bool DRV_SST26_SectorErase( const DRV_HANDLE handle, uint32_t address );

  Summary:
    Erase the sector from the specified block start address.

  Description:
    This function schedules a non-blocking sector erase operation of flash memory.
    Each Sector is of 4 KByte.

    The requesting client should call DRV_SST26_TransferStatusGet() API to know
    the current status of the request.

    The request is sent in QUAD_MODE to flash device.

  Preconditions:
    The DRV_SST26_Open() routine must have been called for the
    specified SST26 driver instance.

  Parameters:
    handle        - A valid open-instance handle, returned from the driver's
                   open routine

    address       - block start address from where a sector needs to be erased.

  Returns:
    false
    - if Write enable fails before sending sector erase command to flash
    - if sector erase command itself fails

    true
    - if the erase request is successfully sent to the flash

  Example:
    <code>

    DRV_HANDLE handle;  // Returned from DRV_SST26_Open
    uint32_t sectorStart = 0;

    if(false == DRV_SST26_SectorErase(handle, sectorStart))
    {
        // Error handling here
    }

    // Wait for erase to be completed
    while(DRV_SST26_TRANSFER_BUSY == DRV_SST26_TransferStatusGet(handle));

    </code>

  Remarks:
    None.
*/

bool DRV_SST26_SectorErase( const DRV_HANDLE handle, uint32_t address );

// **************************************************************************
/* Function:
    bool DRV_SST26_BulkErase( const DRV_HANDLE handle, uint32_t address );

  Summary:
    Erase a block from the specified block start address.

  Description:
    This function schedules a non-blocking block erase operation of flash memory.
    The block size can be 8 KByte, 32KByte or 64 KByte.

    The requesting client should call DRV_SST26_TransferStatusGet() API to know
    the current status of the request.

    The request is sent in QUAD_MODE to flash device.

  Preconditions:
    The DRV_SST26_Open() routine must have been called for the
    specified SST26 driver instance.

  Parameters:
    handle        - A valid open-instance handle, returned from the driver's
                   open routine

    address       - block start address to be erased.

  Returns:
    false
    - if Write enable fails before sending sector erase command to flash
    - if block erase command itself fails

    true
    - if the erase request is successfully sent to the flash

  Example:
    <code>

    DRV_HANDLE handle;  // Returned from DRV_SST26_Open
    uint32_t blockStart = 0;

    if(false == DRV_SST26_SectorErase(handle, blockStart))
    {
        // Error handling here
    }

    // Wait for erase to be completed
    while(DRV_SST26_TRANSFER_BUSY == DRV_SST26_TransferStatusGet(handle));

    </code>

  Remarks:
    None.
*/

bool DRV_SST26_BulkErase( const DRV_HANDLE handle, uint32_t address );

// **************************************************************************
/* Function:
    bool DRV_SST26_ChipErase( const DRV_HANDLE handle );

  Summary:
    Erase entire flash memory.

  Description:
    This function schedules a non-blocking chip erase operation of flash memory.

    The requesting client should call DRV_SST26_TransferStatusGet() API to know
    the current status of the request.

    The request is sent in QUAD_MODE to flash device.

  Preconditions:
    The DRV_SST26_Open() routine must have been called for the
    specified SST26 driver instance.

  Parameters:
    handle        - A valid open-instance handle, returned from the driver's
                    open routine

  Returns:
    false
    - if Write enable fails before sending sector erase command to flash
    - if chip erase command itself fails

    true
    - if the erase request is successfully sent to the flash

  Example:
    <code>

    DRV_HANDLE handle;  // Returned from DRV_SST26_Open

    if(false == DRV_SST26_ChipErase(handle))
    {
        // Error handling here
    }

    // Wait for erase to be completed
    while(DRV_SST26_TRANSFER_BUSY == DRV_SST26_TransferStatusGet(handle));

    </code>

  Remarks:
    None.
*/

bool DRV_SST26_ChipErase( const DRV_HANDLE handle );

// *****************************************************************************
/* Function:
    bool DRV_SST26_Read( const DRV_HANDLE handle, void *rx_data, uint32_t rx_data_length, uint32_t address );

  Summary:
    Reads n bytes of data from the specified start address of flash memory.

  Description:
    This function schedules a blocking operation for reading requested
    number of data bytes from the flash memory.

    The request is sent in QUAD_MODE to flash device.

  Precondition:
    The DRV_SST26_Open() routine must have been called for the
    specified SST26 driver instance.

  Parameters:
    handle          - A valid open-instance handle, returned from the driver's
                      open routine

    *rx_data        - Buffer pointer into which the data read from the SST26
                      Flash memory will be placed.

    rx_data_length  - Total number of bytes to be read.

    address         - Read memory start address from where the data should be
                      read.

  Returns:
    false
    - if read command itself fails

    true
    - if number of bytes requested are read from flash memory

  Example:
    <code>

    #define BUFFER_SIZE  1024
    #define MEM_ADDRESS  0x0

    DRV_HANDLE handle;  // Returned from DRV_SST26_Open
    uint8_t readBuffer[BUFFER_SIZE];

    if (true != DRV_SST26_Read(handle, (void *)&readBuffer, BUFFER_SIZE, MEM_ADDRESS))
    {
        // Error handling here
    }

    </code>

  Remarks:
    None.
*/

bool DRV_SST26_Read( const DRV_HANDLE handle, void *rx_data, uint32_t rx_data_length, uint32_t address );

// *****************************************************************************
/* Function:
    bool DRV_SST26_PageWrite( const DRV_HANDLE handle, void *tx_data, uint32_t tx_data_length, uint32_t address );

  Summary:
    Writes one page of data starting at the specified address.

  Description:
    This function schedules a non-blocking write operation for writing maximum one page
    of data into flash memory.

    The requesting client should call DRV_SST26_TransferStatusGet() API to know
    the current status of the request.

    The request is sent in QUAD_MODE to flash device.

  Preconditions:
    The DRV_SST26_Open() routine must have been called for the
    specified SST26 driver instance.

    The flash address location which has to be written, must have been erased
    before using the SST26_xxxErase() routine.

    The flash address has to be a Page aligned address.

  Parameters:
    handle          - A valid open-instance handle, returned from the driver's
                      open routine

    *tx_data        - The source buffer containing data to be programmed into SST26
                      Flash

    tx_data_length  - Total number of bytes to be written. should not be greater
                      than page size

    address         - Write memory start address from where the data should be
                      written

  Returns:
    false
    - if Write enable fails before sending sector erase command to flash
    - if write command itself fails

    true
    - if the write request is successfully sent to the flash

  Example:
    <code>

    #define PAGE_SIZE    256
    #define BUFFER_SIZE  1024
    #define MEM_ADDRESS  0x0

    DRV_HANDLE handle;  // Returned from DRV_SST26_Open
    uint8_t writeBuffer[BUFFER_SIZE];
    bool status = false;

    if(false == DRV_SST26_SectorErase(handle))
    {
        // Error handling here
    }

    // Wait for erase to be completed
    while(DRV_SST26_TRANSFER_BUSY == DRV_SST26_TransferStatusGet(handle));

    for (uint32_t j = 0; j < BUFFER_SIZE; j += PAGE_SIZE)
    {
        if (true != DRV_SST26_PageWrite(handle, (void *)&writeBuffer[j], (MEM_ADDRESS + j)))
        {
            status = false;
            break;
        }

        // Wait for write to be completed
        while(DRV_SST26_TRANSFER_BUSY == DRV_SST26_TransferStatusGet(handle));
        status = true;
    }

    if(status == false)
    {
        // Error handling here
    }

    </code>

  Remarks:
    None.
*/

bool DRV_SST26_PageWrite( const DRV_HANDLE handle, void *tx_data, uint32_t address );

// *****************************************************************************
/* Function:
    DRV_SST26_TRANSFER_STATUS DRV_SST26_TransferStatusGet( const DRV_HANDLE handle );

  Summary:
    Gets the current status of the transfer request.

  Description:
    This routine gets the current status of the transfer request. The application
    must use this routine where the status of a scheduled request needs to be
    polled on.

  Preconditions:
    The DRV_SST26_Open() routine must have been called for the
    specified SST26 driver instance.

  Parameters:
    handle          - A valid open-instance handle, returned from the driver's
                      open routine

  Returns:
    DRV_SST26_TRANSFER_ERROR_UNKNOWN
    - If the flash status register read request fails

    DRV_SST26_TRANSFER_BUSY
    - If the current transfer request is still being processed

    DRV_SST26_TRANSFER_COMPLETED
    - If the transfer request is completed

  Example:
    <code>

    DRV_HANDLE handle;  // Returned from DRV_SST26_Open

    if (DRV_SST26_TRANSFER_COMPLETED == DRV_SST26_TransferStatusGet(handle))
    {
        // Operation Done
    }
    </code>

  Remarks:
    None.
*/

DRV_SST26_TRANSFER_STATUS DRV_SST26_TransferStatusGet( const DRV_HANDLE handle );

// *****************************************************************************
/* Function:
    bool DRV_SST26_GeometryGet( const DRV_HANDLE handle, SST26_GEOMETRY *geometry );

  Summary:
    Returns the geometry of the device.

  Description:
    This API gives the following geometrical details of the SST26 Flash:
    - Number of Read/Write/Erase Blocks and their size in each region of the device
    - Flash block start address.

  Precondition:
    The DRV_SST26_Open() routine must have been called for the
    specified SST26 driver instance.

  Parameters:
    handle            - A valid open-instance handle, returned from the driver's
                        open routine

    *geometry_table   - pointer to flash device geometry table instance

  Returns:
    false
    - if read device id fails

    true
    - if able to get the geometry details of the flash

  Example:
    <code>

    DRV_HANDLE handle;  // Returned from DRV_SST26_Open
    DRV_SST26_GEOMETRY sst26FlashGeometry;
    uint32_t readBlockSize, writeBlockSize, eraseBlockSize;
    uint32_t nReadBlocks, nReadRegions, totalFlashSize;

    DRV_SST26_GeometryGet(handle, &sst26FlashGeometry);

    readBlockSize  = sst26FlashGeometry.read_blockSize;
    nReadBlocks = sst26FlashGeometry.read_numBlocks;
    nReadRegions = sst26FlashGeometry.numReadRegions;

    writeBlockSize  = sst26FlashGeometry.write_blockSize;
    eraseBlockSize  = sst26FlashGeometry.erase_blockSize;

    totalFlashSize = readBlockSize * nReadBlocks * nReadRegions;

    </code>

  Remarks:
    This API is more useful when used to interface with block driver.
*/

bool DRV_SST26_GeometryGet( const DRV_HANDLE handle, DRV_SST26_GEOMETRY *geometry );

// *****************************************************************************
/* Function:
    void DRV_SST26_EventHandlerSet(
        const DRV_HANDLE handle,
        const DRV_SST26_EVENT_HANDLER eventHandler,
        const uintptr_t context
    )

  Summary:
    Allows a client to identify a transfer event handling function for the driver
    to call back when the requested transfer has finished.

  Description:
    This function allows a client to register a transfer event handling function
    with the driver to call back when the requested transfer has finished.

    The event handler should be set before the client submits any transfer
    requests that could generate events. The event handler once set, persists
    until the client closes the driver or sets another event handler (which
    could be a "NULL" pointer to indicate no callback).

    This function is only supported when sst26 driver is using
    - QSPI PLIB in SPI mode
    - SPI PLIB

  Precondition:
    DRV_SST26_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's open routine.

    eventHandler - Pointer to the event handler function.

    context - The value of parameter will be passed back to the client
    unchanged, when the eventHandler function is called.  It can be used to
    identify any client specific data object that identifies the instance of the
    client module (for example, it may be a pointer to the client module's state
    structure).

  Returns:
    None.

  Example:
    <code>

    #define BUFFER_SIZE  256
    #define MEM_ADDRESS  0x00

    // myAppObj is an application specific state data object.
    MY_APP_OBJ myAppObj;

    uint8_t myBuffer[BUFFER_SIZE];

    // myHandle is the handle returned from DRV_SST26_Open API.

    // Client registers an event handler with driver. This is done once

    DRV_SST26_EventHandlerSet( myHandle, APP_SST26TransferEventHandler, (uintptr_t)&myAppObj );

    if (DRV_SST26_Read(myHandle, myBuffer, BUFFER_SIZE, MEM_ADDRESS) == false)
    {
        // Error handling here
    }

    // The registered event handler is called when the request is complete.

    void APP_SST26TransferEventHandler(DRV_SST26_TRANSFER_STATUS event, uintptr_t context)
    {
        // The context handle was set to an application specific
        // object. It is now retrievable easily in the event handler.
        MY_APP_OBJ* pMyAppObj = (MY_APP_OBJ *) context;

        switch(event)
        {
            case DRV_SST26_TRANSFER_COMPLETED:
                // This means the data was transferred.
                break;

            case DRV_SST26_TRANSFER_ERROR:
                // Error handling here.
                break;

            default:
                break;
        }
    }
    </code>

  Remarks:
    If the client does not want to be notified when the queued buffer transfer
    has completed, it does not need to register a callback.
*/

void DRV_SST26_EventHandlerSet(
    const DRV_HANDLE handle,
    const DRV_SST26_EVENT_HANDLER eventHandler,
    const uintptr_t context
);

#ifdef __cplusplus
}
#endif

#endif // #ifndef _DRV_SST26_H
/*******************************************************************************
 End of File
*/