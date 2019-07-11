/*******************************************************************************
  MX25L Driver Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    drv_mx25l.h

  Summary:
    MX25L Driver Interface Definition

  Description:
    The MX25L driver provides a simple interface to manage the MX25L series
    of SQI Flash Memory connected to Microchip microcontrollers. This file
    defines the interface definition for the MX25L driver.
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

#ifndef _DRV_MX25L_H
#define _DRV_MX25L_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdio.h>
#include <stdbool.h>
#include "drv_mx25l_definitions.h"

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
    MX25L Driver Transfer Status

 Description:
    This data type will be used to indicate the current transfer status for MX25L
    driver.

 Remarks:
    None.
*/

typedef enum
{
    /* Transfer is being processed */
    DRV_MX25L_TRANSFER_BUSY,
    /* Transfer is successfully completed*/
    DRV_MX25L_TRANSFER_COMPLETED,
    /* Transfer had error*/
    DRV_MX25L_TRANSFER_ERROR_UNKNOWN,
} DRV_MX25L_TRANSFER_STATUS;

/*
 Summary:
    MX25L Device Geometry data.

 Description:
    This data type will be used to get the geometry details of the
    MX25L flash device.

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
} DRV_MX25L_GEOMETRY;

// *****************************************************************************
// *****************************************************************************
// Section: MX25L Driver Module Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_MX25L_Initialize
    (
        const SYS_MODULE_INDEX drvIndex,
        const SYS_MODULE_INIT *const init
    );

  Summary:
    Initializes the MX25L Driver

  Description:
    This routine initializes the MX25L driver making it ready for client to use.
    - Resets the Flash Device
    - Puts it on QUAD IO Mode
    - Unlocks the flash

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
    // This code snippet shows an example of initializing the MX25L Driver
    // with MX25L QSPI flash device attached.

    SYS_MODULE_OBJ  objectHandle;

    const DRV_MX25L_PLIB_INTERFACE drvMX25LPlibAPI = {
        .CommandWrite  = QSPI_CommandWrite,
        .RegisterRead  = QSPI_RegisterRead,
        .RegisterWrite = QSPI_RegisterWrite,
        .MemoryRead    = QSPI_MemoryRead,
        .MemoryWrite   = QSPI_MemoryWrite
    };

    const DRV_MX25L_INIT drvMX25LInitData =
    {
        .mx25lPlib         = &drvMX25LPlibAPI,
    };

    objectHandle = DRV_MX25L_Initialize((SYS_MODULE_INDEX)DRV_MX25L_INDEX, (SYS_MODULE_INIT *)&drvMX25LInitData);

    if (SYS_MODULE_OBJ_INVALID == objectHandle)
    {
        // Handle error
    }
    </code>

  Remarks:
    This routine must be called before any other MX25L driver routine is called.

    This routine should only be called once during system initialization.

    This routine will block for hardware access.
*/

SYS_MODULE_OBJ DRV_MX25L_Initialize
(
    const SYS_MODULE_INDEX drvIndex,
    const SYS_MODULE_INIT *const init
);

// ****************************************************************************
/* Function:
    DRV_HANDLE DRV_MX25L_Open( const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT ioIntent );

  Summary:
    Opens the specified MX25L driver instance and returns a handle to it

  Description:
    This routine opens the specified MX25L driver instance and provides a handle.

    This handle must be provided to all other client-level operations to identify
    the caller and the instance of the driver.

  Preconditions:
    Function DRV_MX25L_Initialize must have been called before calling this
    function.

    Driver should be in ready state to accept the request. Can be checked by
    calling DRV_MX25L_Status().

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

    handle = DRV_MX25L_Open(DRV_MX25L_INDEX);
    if (DRV_HANDLE_INVALID == handle)
    {
        // Unable to open the driver
    }
    </code>

  Remarks:
    The handle returned is valid until the DRV_MX25L_Close routine is called.

    If the driver has already been opened, it should not be opened again.
*/

DRV_HANDLE DRV_MX25L_Open( const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT ioIntent );

// *****************************************************************************
/* Function:
    void DRV_MX25L_Close( const DRV_HANDLE handle );

  Summary:
    Closes an opened-instance of the MX25L driver

  Description:
    This routine closes an opened-instance of the MX25L driver, invalidating
    the handle.

  Precondition:
    DRV_MX25L_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    None

  Example:
    <code>
    DRV_HANDLE handle;  // Returned from DRV_MX25L_Open

    DRV_MX25L_Close(handle);
    </code>

  Remarks:
    After calling this routine, the handle passed in "handle" must not be used
    with any of the remaining driver routines. A new handle must be obtained by
    calling DRV_MX25L_Open before the caller may use the driver again.

    Usually there is no need for the driver client to verify that the Close
    operation has completed.
*/

void DRV_MX25L_Close( const DRV_HANDLE handle );

// *************************************************************************
/* Function:
    SYS_STATUS DRV_MX25L_Status( const SYS_MODULE_INDEX drvIndex );

  Summary:
    Gets the current status of the MX25L driver module.

  Description:
    This routine provides the current status of the MX25L driver module.

  Preconditions:
    Function DRV_MX25L_Initialize should have been called before calling
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

    Status = DRV_MX25L_Status(DRV_MX25L_INDEX);
    </code>

  Remarks:
    This routine will NEVER block wait for hardware.
*/

SYS_STATUS DRV_MX25L_Status( const SYS_MODULE_INDEX drvIndex );

// *****************************************************************************
/* Function:
    bool DRV_MX25L_ResetFlash(void);

  Summary:
    Reset the flash device to standby mode.

  Description:
    This function schedules a blocking operation for resetting the flash device to
    standby mode. All the volatile bits and settings will be cleared then,
    which makes the device return to the default status as power on.

  Precondition:
    The DRV_MX25L_Open() routine must have been called for the
    specified MX25L driver instance.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    false
    - if Reset-Enable or Reset flash command itself fails

    true
    - if the reset is successfully completed

  Example:
    <code>
    if(true != DRV_MX25L_ResetFlash())
    {
        // Error handling here
    }

    </code>

  Remarks:
    This routine will block wait for request to complete.
*/

bool DRV_MX25L_ResetFlash(void);

// *****************************************************************************
/* Function:
    bool DRV_MX25L_ReadJedecId( const DRV_HANDLE handle, void *jedec_id );

  Summary:
    Reads JEDEC-ID of the flash device.

  Description:
    This function schedules a blocking operation for reading the JEDEC-ID.
    This information can be used to get the flash device geometry.

    The request is sent in QUAD_MODE to flash device.

  Precondition:
    The DRV_MX25L_Open() routine must have been called for the
    specified MX25L driver instance.

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

    DRV_HANDLE handle;  // Returned from DRV_MX25L_Open
    uint32_t jedec_id = 0;

    if(true != DRV_MX25L_ReadJedecId(handle, &jedec_id))
    {
        // Error handling here
    }

    </code>

  Remarks:
    This routine will block wait for transfer to complete.
*/

bool DRV_MX25L_ReadJedecId( const DRV_HANDLE handle, void *jedec_id );

// **************************************************************************
/* Function:
    bool DRV_MX25L_SectorErase( const DRV_HANDLE handle, uint32_t address );

  Summary:
    Erase the sector from the specified block start address.

  Description:
    This function schedules a non-blocking sector erase operation of flash memory.
    Each Sector is of 4 KByte.

    The requesting client should call DRV_MX25L_TransferStatusGet() API to know
    the current status of the request.

    The request is sent in QUAD_MODE to flash device.

  Preconditions:
    The DRV_MX25L_Open() routine must have been called for the
    specified MX25L driver instance.

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

    DRV_HANDLE handle;  // Returned from DRV_MX25L_Open
    uint32_t sectorStart = 0;

    if(false == DRV_MX25L_SectorErase(handle, sectorStart))
    {
        // Error handling here
    }

    // Wait for erase to be completed
    while(DRV_MX25L_TRANSFER_BUSY == DRV_MX25L_TransferStatusGet(handle));

    </code>

  Remarks:
    This routine will block wait until erase request is submitted successfully.

    Client should wait until erase is complete to send next transfer request.
*/

bool DRV_MX25L_SectorErase( const DRV_HANDLE handle, uint32_t address );

// **************************************************************************
/* Function:
    bool DRV_MX25L_BlockErase( const DRV_HANDLE handle, uint32_t address );

  Summary:
    Erase a block from the specified block start address.

  Description:
    This function schedules a non-blocking block erase operation of flash memory.
    The block size can be 8 KByte, 32KByte or 64 KByte.

    The requesting client should call DRV_MX25L_TransferStatusGet() API to know
    the current status of the request.

    The request is sent in QUAD_MODE to flash device.

  Preconditions:
    The DRV_MX25L_Open() routine must have been called for the
    specified MX25L driver instance.

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

    DRV_HANDLE handle;  // Returned from DRV_MX25L_Open
    uint32_t blockStart = 0;

    if(false == DRV_MX25L_SectorErase(handle, blockStart))
    {
        // Error handling here
    }

    // Wait for erase to be completed
    while(DRV_MX25L_TRANSFER_BUSY == DRV_MX25L_TransferStatusGet(handle));

    </code>

  Remarks:
    This routine will block wait until erase request is submitted successfully.

    Client should wait until erase is complete to send next transfer request.
*/

bool DRV_MX25L_BlockErase( const DRV_HANDLE handle, uint32_t address );

// **************************************************************************
/* Function:
    bool DRV_MX25L_ChipErase( const DRV_HANDLE handle );

  Summary:
    Erase entire flash memory.

  Description:
    This function schedules a non-blocking chip erase operation of flash memory.

    The requesting client should call DRV_MX25L_TransferStatusGet() API to know
    the current status of the request.

    The request is sent in QUAD_MODE to flash device.

  Preconditions:
    The DRV_MX25L_Open() routine must have been called for the
    specified MX25L driver instance.

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

    DRV_HANDLE handle;  // Returned from DRV_MX25L_Open

    if(false == DRV_MX25L_ChipErase(handle))
    {
        // Error handling here
    }

    // Wait for erase to be completed
    while(DRV_MX25L_TRANSFER_BUSY == DRV_MX25L_TransferStatusGet(handle));

    </code>

  Remarks:
    This routine will block wait until erase request is submitted successfully.

    Client should wait until erase is complete to send next transfer request.
*/

bool DRV_MX25L_ChipErase( const DRV_HANDLE handle );

// *****************************************************************************
/* Function:
    bool DRV_MX25L_Read( const DRV_HANDLE handle, void *rx_data, uint32_t rx_data_length, uint32_t address );

  Summary:
    Reads n bytes of data from the specified start address of flash memory.

  Description:
    This function schedules a blocking operation for reading requested
    number of data bytes from the flash memory.

    The request is sent in QUAD_MODE to flash device.

  Precondition:
    The DRV_MX25L_Open() routine must have been called for the
    specified MX25L driver instance.

  Parameters:
    handle          - A valid open-instance handle, returned from the driver's
                      open routine

    *rx_data        - Buffer pointer into which the data read from the MX25L
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

    DRV_HANDLE handle;  // Returned from DRV_MX25L_Open
    uint8_t readBuffer[BUFFER_SIZE];

    if (true != DRV_MX25L_Read(handle, (void *)&readBuffer, BUFFER_SIZE, MEM_ADDRESS))
    {
        // Error handling here
    }

    </code>

  Remarks:
    This routine will block waiting until read request is completed successfully.
*/

bool DRV_MX25L_Read( const DRV_HANDLE handle, void *rx_data, uint32_t rx_data_length, uint32_t address );

// *****************************************************************************
/* Function:
    bool DRV_MX25L_PageWrite( const DRV_HANDLE handle, void *tx_data, uint32_t tx_data_length, uint32_t address );

  Summary:
    Writes one page of data starting at the specified address.

  Description:
    This function schedules a non-blocking write operation for writing maximum one page
    of data into flash memory.

    The requesting client should call DRV_MX25L_TransferStatusGet() API to know
    the current status of the request.

    The request is sent in QUAD_MODE to flash device.

  Preconditions:
    The DRV_MX25L_Open() routine must have been called for the
    specified MX25L driver instance.

    The flash address location which has to be written, must have been erased
    before using the MX25L_xxxErase() routine.

    The flash address has to be a Page aligned address.

  Parameters:
    handle          - A valid open-instance handle, returned from the driver's
                      open routine

    *tx_data        - The source buffer containing data to be programmed into MX25L
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

    DRV_HANDLE handle;  // Returned from DRV_MX25L_Open
    uint8_t writeBuffer[BUFFER_SIZE];
    bool status = false;

    if(false == DRV_MX25L_SectorErase(handle))
    {
        // Error handling here
    }

    // Wait for erase to be completed
    while(DRV_MX25L_TRANSFER_BUSY == DRV_MX25L_TransferStatusGet(handle));

    for (uint32_t j = 0; j < BUFFER_SIZE; j += PAGE_SIZE)
    {
        if (true != DRV_MX25L_PageWrite(handle, (void *)&writeBuffer[j], (MEM_ADDRESS + j)))
        {
            status = false;
            break;
        }

        // Wait for write to be completed
        while(DRV_MX25L_TRANSFER_BUSY == DRV_MX25L_TransferStatusGet(handle));
        status = true;
    }

    if(status == false)
    {
        // Error handling here
    }

    </code>

  Remarks:
    This routine will block wait until write request is submitted successfully.

    Client should wait until write is complete to send next transfer request.
*/

bool DRV_MX25L_PageWrite( const DRV_HANDLE handle, void *tx_data, uint32_t address );

// *****************************************************************************
/* Function:
    DRV_MX25L_TRANSFER_STATUS DRV_MX25L_TransferStatusGet( const DRV_HANDLE handle );

  Summary:
    Gets the current status of the transfer request.

  Description:
    This routine gets the current status of the transfer request. The application
    must use this routine where the status of a scheduled request needs to be
    polled on.

  Preconditions:
    The DRV_MX25L_Open() routine must have been called for the
    specified MX25L driver instance.

  Parameters:
    handle          - A valid open-instance handle, returned from the driver's
                      open routine

  Returns:
    DRV_MX25L_TRANSFER_ERROR_UNKNOWN
    - If the flash status register read request fails

    DRV_MX25L_TRANSFER_BUSY
    - If the current transfer request is still being processed

    DRV_MX25L_TRANSFER_COMPLETED
    - If the transfer request is completed

  Example:
    <code>

    DRV_HANDLE handle;  // Returned from DRV_MX25L_Open

    if (DRV_MX25L_TRANSFER_COMPLETED == DRV_MX25L_TransferStatusGet(handle))
    {
        // Operation Done
    }
    </code>

  Remarks:
    This routine will block for hardware access.
*/

DRV_MX25L_TRANSFER_STATUS DRV_MX25L_TransferStatusGet( const DRV_HANDLE handle );

// *****************************************************************************
/* Function:
    bool DRV_MX25L_GeometryGet( const DRV_HANDLE handle, MX25L_GEOMETRY *geometry );

  Summary:
    Returns the geometry of the device.

  Description:
    This API gives the following geometrical details of the MX25L Flash:
    - Number of Read/Write/Erase Blocks and their size in each region of the device
    - Flash block start address.

  Precondition:
    The DRV_MX25L_Open() routine must have been called for the
    specified MX25L driver instance.

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

    DRV_HANDLE handle;  // Returned from DRV_MX25L_Open
    DRV_MX25L_GEOMETRY mx25lFlashGeometry;
    uint32_t readBlockSize, writeBlockSize, eraseBlockSize;
    uint32_t nReadBlocks, nReadRegions, totalFlashSize;

    DRV_MX25L_GeometryGet(handle, &mx25lFlashGeometry);

    readBlockSize  = mx25lFlashGeometry.read_blockSize;
    nReadBlocks = mx25lFlashGeometry.read_numBlocks;
    nReadRegions = mx25lFlashGeometry.numReadRegions;

    writeBlockSize  = mx25lFlashGeometry.write_blockSize;
    eraseBlockSize  = mx25lFlashGeometry.erase_blockSize;

    totalFlashSize = readBlockSize * nReadBlocks * nReadRegions;

    </code>

  Remarks:
    This API is more useful when used to interface with block driver
    like memory driver.
*/

bool DRV_MX25L_GeometryGet( const DRV_HANDLE handle, DRV_MX25L_GEOMETRY *geometry );

#ifdef __cplusplus
}
#endif

#endif // #ifndef _DRV_MX25L_H
/*******************************************************************************
 End of File
*/