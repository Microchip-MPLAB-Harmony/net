/******************************************************************************
  NVM Interface Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    drv_memory_nvm.c

  Summary:
    Memory Driver NVM Interface Definition

  Description:
    The Memory Driver provides a interface to access the NVM peripheral on the
    microcontroller.
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

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#include "definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global objects
// *****************************************************************************
// *****************************************************************************
DRV_MEMORY_EVENT_HANDLER   nvmEventHandler;

// *****************************************************************************
// *****************************************************************************
// Section: Driver NVM Local Functions
// *****************************************************************************
// *****************************************************************************
static void DRV_NVM_EventHandler( uintptr_t context)
{
    nvmEventHandler(MEMORY_DEVICE_TRANSFER_COMPLETED, context);
}

// *****************************************************************************
// *****************************************************************************
// Section: Driver NVM Interface Global Functions
// *****************************************************************************
// *****************************************************************************

MEMORY_DEVICE_TRANSFER_STATUS DRV_NVM_TransferStatusGet( const DRV_HANDLE handle )
{
    if (NVM_IsBusy() == true)
    {
        return MEMORY_DEVICE_TRANSFER_BUSY;
    }
    else
    {
        return MEMORY_DEVICE_TRANSFER_COMPLETED;
    }
}

bool DRV_NVM_Read( const DRV_HANDLE handle, void *rx_data, uint32_t rx_data_length, uint32_t address )
{
    return (NVM_Read((uint32_t *)rx_data, rx_data_length, address));
}

bool DRV_NVM_PageWrite( const DRV_HANDLE handle, void *tx_data, uint32_t address )
{
    return (NVM_RowWrite((uint32_t *)tx_data, address));
}

bool DRV_NVM_SectorErase( const DRV_HANDLE handle, uint32_t address )
{
    return (NVM_PageErase(address));
}

void DRV_NVM_EventHandlerSet( const DRV_HANDLE handle, const DRV_MEMORY_EVENT_HANDLER eventHandler, const uintptr_t context )
{
    nvmEventHandler = eventHandler;
    NVM_CallbackRegister(DRV_NVM_EventHandler, context);
}

bool DRV_NVM_GeometryGet( const DRV_HANDLE handle, MEMORY_DEVICE_GEOMETRY *geometry )
{
    /* Read block size and number of blocks */
    geometry->read_blockSize = 1;
    geometry->read_numBlocks = (DRV_MEMORY_DEVICE_MEDIA_SIZE * 1024);

    /* Write block size and number of blocks */
    geometry->write_blockSize = DRV_MEMORY_DEVICE_PROGRAM_SIZE;
    geometry->write_numBlocks = ((DRV_MEMORY_DEVICE_MEDIA_SIZE * 1024) / DRV_MEMORY_DEVICE_PROGRAM_SIZE);

    /* Erase block size and number of blocks */
    geometry->erase_blockSize = DRV_MEMORY_DEVICE_ERASE_SIZE;
    geometry->erase_numBlocks = ((DRV_MEMORY_DEVICE_MEDIA_SIZE * 1024) / DRV_MEMORY_DEVICE_ERASE_SIZE);

    geometry->numReadRegions = 1;
    geometry->numWriteRegions = 1;
    geometry->numEraseRegions = 1;

    geometry->blockStartAddress = DRV_MEMORY_DEVICE_START_ADDRESS;

    return true;
}

DRV_HANDLE DRV_NVM_Open( const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT ioIntent )
{
    return ((DRV_HANDLE)drvIndex);
}

void DRV_NVM_Close( const DRV_HANDLE handle )
{

}

SYS_STATUS DRV_NVM_Status( const SYS_MODULE_INDEX drvIndex )
{
    /* Return the driver status */
    return (SYS_STATUS_READY);
}
