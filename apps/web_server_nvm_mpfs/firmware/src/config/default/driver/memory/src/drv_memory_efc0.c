/******************************************************************************
  EFC0 Interface Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    drv_memory_efc0.c

  Summary:
    Memory Driver EFC0 Interface Definition

  Description:
    The Memory Driver provides a interface to access the EFC0 peripheral on the
    microcontroller.
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

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#include "driver/memory/drv_memory_efc0.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global objects
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Driver EFC0 Interface Global Functions
// *****************************************************************************
// *****************************************************************************

MEMORY_DEVICE_TRANSFER_STATUS DRV_EFC0_TransferStatusGet( const DRV_HANDLE handle )
{
    if (EFC0_IsBusy() == true)
    {
        return MEMORY_DEVICE_TRANSFER_BUSY;
    }
    else
    {
        return MEMORY_DEVICE_TRANSFER_COMPLETED;
    }
}

bool DRV_EFC0_Read( const DRV_HANDLE handle, void *rx_data, uint32_t rx_data_length, uint32_t address )
{
    return (EFC0_Read((uint32_t *)rx_data, rx_data_length, address));
}

bool DRV_EFC0_PageWrite( const DRV_HANDLE handle, void *tx_data, uint32_t address )
{
    return (EFC0_PageWrite((uint32_t *)tx_data, address));
}

bool DRV_EFC0_SectorErase( const DRV_HANDLE handle, uint32_t address )
{
    return (EFC0_SectorErase(address));
}

bool DRV_EFC0_GeometryGet( const DRV_HANDLE handle, MEMORY_DEVICE_GEOMETRY *geometry )
{
    /* Read block size and number of blocks */
    geometry->read_blockSize = 1;
    geometry->read_numBlocks = (DRV_MEMORY_DEVICE_MEDIA_SIZE * DRV_MEMORY_DEVICE_MEDIA_SIZE);

    /* Write block size and number of blocks */
    geometry->write_blockSize = EFC0_PAGESIZE;
    geometry->write_numBlocks = ((DRV_MEMORY_DEVICE_MEDIA_SIZE * DRV_MEMORY_DEVICE_MEDIA_SIZE) / EFC0_PAGESIZE);

    /* Erase block size and number of blocks */
    geometry->erase_blockSize = EFC0_SECTORSIZE;
    geometry->erase_numBlocks = ((DRV_MEMORY_DEVICE_MEDIA_SIZE * DRV_MEMORY_DEVICE_MEDIA_SIZE) / EFC0_SECTORSIZE);

    geometry->numReadRegions = 1;
    geometry->numWriteRegions = 1;
    geometry->numEraseRegions = 1;

    geometry->blockStartAddress = DRV_MEMORY_DEVICE_START_ADDRESS;

    return true;
}

DRV_HANDLE DRV_EFC0_Open( const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT ioIntent )
{
    return ((DRV_HANDLE)drvIndex);
}

void DRV_EFC0_Close( const DRV_HANDLE handle )
{

}

SYS_STATUS DRV_EFC0_Status( const SYS_MODULE_INDEX drvIndex )
{
    /* Return the driver status */
    return (SYS_STATUS_READY);
}
