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
Copyright (c) 2016 - 2017 released Microchip Technology Inc. All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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
#ifndef _DRV_MEMORY_DEFINITIONS_H
#define _DRV_MEMORY_DEFINITIONS_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdbool.h>
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

/* Memory Driver Transfer Status
 Summary:
    Defines the data type for the Memory driver transfer status.

 Description:
    This will be used to indicate the current transfer status of the
    memory driver.

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

/* Memory Device Geometry Table data Declaration */
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

typedef bool (*GEOMETRY_GET)( const DRV_HANDLE handle, MEMORY_DEVICE_GEOMETRY *geometry );

typedef uint32_t (*TRANSFER_STATUS_GET)( const DRV_HANDLE handle );

typedef struct
{
    DRV_HANDLE (*Open)( const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT ioIntent );

    void (*Close)( const DRV_HANDLE handle );

    bool (*SectorErase)( const DRV_HANDLE handle, uint32_t address);

    SYS_STATUS (*Status)( const SYS_MODULE_INDEX drvIndex );

    bool (*Read)( const DRV_HANDLE handle, void *rx_data, uint32_t rx_data_length, uint32_t address );

    bool (*PageWrite)( const DRV_HANDLE handle, void *tx_data, uint32_t address );

    GEOMETRY_GET GeometryGet;

    TRANSFER_STATUS_GET TransferStatusGet;
} MEMORY_DEVICE_API;

// *****************************************************************************
/* Memory Driver Initialization Data

  Summary:
    Defines the data required to initialize or reinitialize the Memory Driver

  Description:
    This data type defines the data required to initialize the Memory Driver.

  Remarks:
    Not all initialization features are available for all devices. Please
    refer to the specific device data sheet to determine availability.
*/

typedef struct
{
    /* Attached Memory Device index */
    SYS_MODULE_INDEX memDevIndex;

    /* Flash Device functions */
    const MEMORY_DEVICE_API *memoryDevice;

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

