/*******************************************************************************
  Memory Driver Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    drv_memory_definitions.h

  Summary:
    Memory Driver Interface Definition

  Description:
    The Memory driver provides a simple interface to manage the various memory
    devices. This file defines the interface definition for the Memory driver.
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
#ifndef _DRV_MEMORY_DEFINITIONS_H
#define _DRV_MEMORY_DEFINITIONS_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "driver/driver_common.h"
#include "system/system.h"
#include "system/system_common.h"
#include "system/system_module.h"
#include "system/system_media.h"
#include "system/int/sys_int.h"

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
    Memory Device Transfer Status.

 Description:
    This Data structure is used to indicate the current transfer
    status of the attached media.

 Remarks:
    None.
*/
typedef enum
{
    /* Transfer being processed */
    MEMORY_DEVICE_TRANSFER_BUSY,
    /* Transfer is successfully completed*/
    MEMORY_DEVICE_TRANSFER_COMPLETED,
    /* Transfer had error*/
    MEMORY_DEVICE_TRANSFER_ERROR_UNKNOWN,
} MEMORY_DEVICE_TRANSFER_STATUS;

/*
 Summary:
    Memory Device Geometry Table.

 Description:
    This Data Structure is used by Memory driver to get
    the media geometry details.

    The Media attached to memory driver needs to fill in
    this data structure when GEOMETRY_GET is called.

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
} MEMORY_DEVICE_GEOMETRY;

/* Function pointer typedef to open the attached media */
typedef DRV_HANDLE (*DRV_MEMORY_DEVICE_OPEN)( const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT ioIntent );

/* Function pointer typedef to close the attached media */
typedef void (*DRV_MEMORY_DEVICE_CLOSE)( const DRV_HANDLE handle );

/* Function pointer typedef to erase a sector from attached media */
typedef bool (*DRV_MEMORY_DEVICE_SECTOR_ERASE)( const DRV_HANDLE handle, uint32_t address);

/* Function pointer typedef to get the status of the attached media */
typedef SYS_STATUS (*DRV_MEMORY_DEVICE_STATUS)( const SYS_MODULE_INDEX drvIndex );

/* Function pointer typedef to read from the attached media */
typedef bool (*DRV_MEMORY_DEVICE_READ)( const DRV_HANDLE handle, void *rx_data, uint32_t rx_data_length, uint32_t address );

/* Function pointer typedef to write a page to the attached media */
typedef bool (*DRV_MEMORY_DEVICE_PAGE_WRITE)( const DRV_HANDLE handle, void *tx_data, uint32_t address );

/* Function pointer typedef to get the Geometry details from attached media */
typedef bool (*DRV_MEMORY_DEVICE_GEOMETRY_GET)( const DRV_HANDLE handle, MEMORY_DEVICE_GEOMETRY *geometry );

/* Function pointer typedef to get the transfer Status from attached media */
typedef uint32_t (*DRV_MEMORY_DEVICE_TRANSFER_STATUS_GET)( const DRV_HANDLE handle );

/* Function pointer typedef for event handler to be sent to attached media */
typedef void (*DRV_MEMORY_EVENT_HANDLER)( MEMORY_DEVICE_TRANSFER_STATUS status, uintptr_t context );

/* Function pointer typedef to set the event handler with attached media */
typedef void (*DRV_MEMORY_DEVICE_EVENT_HANDLER_SET) ( const DRV_HANDLE handle, DRV_MEMORY_EVENT_HANDLER eventHandler, uintptr_t context );

/* 
 Summary:
    Memory Device API.

 Description:
    This Data Structure is used by attached media to populate the
    required device functions for media transactions.

    This will be used in memory driver init structure.

 Remarks:
    None.
*/
typedef struct
{
    DRV_MEMORY_DEVICE_OPEN Open;

    DRV_MEMORY_DEVICE_CLOSE Close;

    DRV_MEMORY_DEVICE_SECTOR_ERASE SectorErase;

    DRV_MEMORY_DEVICE_STATUS Status;

    DRV_MEMORY_DEVICE_READ Read;

    DRV_MEMORY_DEVICE_PAGE_WRITE PageWrite;
    
    DRV_MEMORY_DEVICE_EVENT_HANDLER_SET EventHandlerSet;

    DRV_MEMORY_DEVICE_GEOMETRY_GET GeometryGet;

    DRV_MEMORY_DEVICE_TRANSFER_STATUS_GET TransferStatusGet;
} DRV_MEMORY_DEVICE_INTERFACE;

/*
  Summary:
    Memory Driver Initialization Data

  Description:
    This data type defines the data required to initialize the Memory Driver.

  Remarks:
    Not all initialization features are available for all devices. Please
    refer to the attached media device capabilities.
*/

typedef struct
{
    /* Attached Memory Device index */
    SYS_MODULE_INDEX memDevIndex;

    /* Flash Device functions */
    const DRV_MEMORY_DEVICE_INTERFACE *memoryDevice;

    /* Flag to indicate if attached memory device configured to interrupt mode */
    bool isMemDevInterruptEnabled;

    /* Number of milliseconds to poll for transfer status check */
    uint32_t memDevStatusPollUs;

    /* FS enabled */
    bool isFsEnabled;

    /* Memory Device Type */
    uint8_t deviceMediaType;

    /* Erase Write Buffer pointer */
    uint8_t *ewBuffer;

    /* Memory pool for Client Objects */
    uintptr_t  clientObjPool;

    /* Pointer to Buffer Objects array */
    uintptr_t  bufferObj;

    /* Buffer Queue Size */
    size_t queueSize;

    /* Maximum number of clients */
    size_t nClientsMax;

} DRV_MEMORY_INIT;

#ifdef __cplusplus
}
#endif

#endif // #ifndef _DRV_MEMORY_DEFINITIONS_H
/*******************************************************************************
 End of File
*/

