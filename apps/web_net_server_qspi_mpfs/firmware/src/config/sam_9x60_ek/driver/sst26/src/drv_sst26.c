/******************************************************************************
  SST26 Driver Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    drv_sst26.c

  Summary:
    SST26 Driver Interface Definition

  Description:
    The SST26 Driver provides a interface to access the SST26 peripheral on the PIC32
    microcontroller. This file should be included in the project if SST26 driver
    functionality is needed.
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

#include "driver/sst26/src/drv_sst26_local.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global objects
// *****************************************************************************
// *****************************************************************************

#define DRV_SST26_PAGE_SIZE   256
#define DRV_SST26_ERASE_SIZE  4096

static DRV_SST26_OBJECT gDrvSST26Obj;
static DRV_SST26_OBJECT *dObj = &gDrvSST26Obj;

static qspi_command_xfer_t qspi_command_xfer = { 0 };
static qspi_register_xfer_t qspi_register_xfer = { 0 };
static qspi_memory_xfer_t qspi_memory_xfer = { 0 };

/* Table mapping the Flash ID's to their sizes. */
static uint32_t gSstFlashIdSizeTable [5][2] = {
    {0x01, 0x200000}, /* 16 MBit */
    {0x41, 0x200000}, /* 16 MBit */
    {0x02, 0x400000}, /* 32 MBit */
    {0x42, 0x400000}, /* 32 MBit */
    {0x43, 0x800000}  /* 64 MBit */
};

// *****************************************************************************
// *****************************************************************************
// Section: SST26 Driver Local Functions
// *****************************************************************************
// *****************************************************************************

/* This function returns the flash size in bytes for the specified deviceId. A
 * zero is returned if the device id is not supported. */
static uint32_t DRV_SST26_GetFlashSize( uint8_t deviceId )
{
    uint8_t i = 0;

    for (i = 0; i < 5; i++)
    {
        if (deviceId == gSstFlashIdSizeTable[i][0])
        {
            return gSstFlashIdSizeTable[i][1];
        }
    }

    return 0;
}

static bool DRV_SST26_ResetFlash(void)
{
    bool status = false;

    memset((void *)&qspi_command_xfer, 0, sizeof(qspi_command_xfer_t));

    qspi_command_xfer.instruction = SST26_CMD_FLASH_RESET_ENABLE;
    qspi_command_xfer.width = SINGLE_BIT_SPI;

    if (dObj->sst26Plib->CommandWrite(&qspi_command_xfer, 0) == false)
    {
        return status;
    }

    qspi_command_xfer.instruction = SST26_CMD_FLASH_RESET;
    qspi_command_xfer.width = SINGLE_BIT_SPI;

    status  = dObj->sst26Plib->CommandWrite(&qspi_command_xfer, 0);

    return status;
}

static bool DRV_SST26_EnableQuadIO(void)
{
    bool status = false;

    memset((void *)&qspi_command_xfer, 0, sizeof(qspi_command_xfer_t));

    qspi_command_xfer.instruction = SST26_CMD_ENABLE_QUAD_IO;
    qspi_command_xfer.width = SINGLE_BIT_SPI;

    status  = dObj->sst26Plib->CommandWrite(&qspi_command_xfer, 0);

    return status;
}

static bool DRV_SST26_WriteEnable(void)
{
    bool status = false;

    memset((void *)&qspi_command_xfer, 0, sizeof(qspi_command_xfer_t));

    qspi_command_xfer.instruction = SST26_CMD_WRITE_ENABLE;
    qspi_command_xfer.width = QUAD_CMD;

    status  = dObj->sst26Plib->CommandWrite(&qspi_command_xfer, 0);

    return status;
}

// *****************************************************************************
// *****************************************************************************
// Section: SST26 Driver Global Functions
// *****************************************************************************
// *****************************************************************************

bool DRV_SST26_UnlockFlash( const DRV_HANDLE handle )
{
    bool status = false;

    if(handle == DRV_HANDLE_INVALID)
    {
        return status;
    }

    if (DRV_SST26_WriteEnable() == false)
    {
        return status;
    }

    memset((void *)&qspi_command_xfer, 0, sizeof(qspi_command_xfer_t));

    qspi_command_xfer.instruction = SST26_CMD_UNPROTECT_GLOBAL;
    qspi_command_xfer.width = QUAD_CMD;

    status  = dObj->sst26Plib->CommandWrite(&qspi_command_xfer, 0);

    return status;
}

bool DRV_SST26_ReadJedecId( const DRV_HANDLE handle, void *jedec_id)
{
    bool status = false;

    if(handle == DRV_HANDLE_INVALID)
    {
        return status;
    }

    memset((void *)&qspi_register_xfer, 0, sizeof(qspi_register_xfer_t));

    qspi_register_xfer.instruction = SST26_CMD_QUAD_JEDEC_ID_READ;
    qspi_register_xfer.width = QUAD_CMD;
    qspi_register_xfer.dummy_cycles = 2;

    status  = dObj->sst26Plib->RegisterRead(&qspi_register_xfer, (uint32_t *)jedec_id, 3);

    return status;
}

bool DRV_SST26_ReadStatus( const DRV_HANDLE handle, void *rx_data, uint32_t rx_data_length )
{
    bool status = false;

    if(handle == DRV_HANDLE_INVALID)
    {
        return status;
    }

    memset((void *)&qspi_register_xfer, 0, sizeof(qspi_register_xfer_t));

    qspi_register_xfer.instruction = SST26_CMD_READ_STATUS_REG;
    qspi_register_xfer.width = QUAD_CMD;
    qspi_register_xfer.dummy_cycles = 2;

    status  = dObj->sst26Plib->RegisterRead(&qspi_register_xfer, (uint32_t *)rx_data, rx_data_length);

    return status;
}

DRV_SST26_TRANSFER_STATUS DRV_SST26_TransferStatusGet( const DRV_HANDLE handle )
{
    DRV_SST26_TRANSFER_STATUS status = DRV_SST26_TRANSFER_ERROR_UNKNOWN;

    uint8_t reg_status = 0;

    if(handle == DRV_HANDLE_INVALID)
    {
        return status;
    }

    if (gDrvSST26Obj.curOpType == DRV_SST26_OPERATION_TYPE_READ )
    {
        return DRV_SST26_TRANSFER_COMPLETED;
    }

    if (DRV_SST26_ReadStatus(handle, (void *)&reg_status, 1) == false)
    {
        return status;
    }

    if(reg_status & (1<<0))
        status = DRV_SST26_TRANSFER_BUSY;
    else
        status = DRV_SST26_TRANSFER_COMPLETED;

    return status;
}

bool DRV_SST26_Read( const DRV_HANDLE handle, void *rx_data, uint32_t rx_data_length, uint32_t address )
{
    bool status = false;

    if(handle == DRV_HANDLE_INVALID)
    {
        return status;
    }

    memset((void *)&qspi_memory_xfer, 0, sizeof(qspi_memory_xfer_t));

    qspi_memory_xfer.instruction = SST26_CMD_HIGH_SPEED_READ;
    qspi_memory_xfer.width = QUAD_CMD;
    qspi_memory_xfer.dummy_cycles = 6;

    status = dObj->sst26Plib->MemoryRead(&qspi_memory_xfer, (uint32_t *)rx_data, rx_data_length, address);

    gDrvSST26Obj.curOpType = DRV_SST26_OPERATION_TYPE_READ;

    return status;
}

bool DRV_SST26_PageWrite( const DRV_HANDLE handle, void *tx_data, uint32_t address )
{
    bool status = false;

    if(handle == DRV_HANDLE_INVALID)
    {
        return status;
    }

    if (DRV_SST26_WriteEnable() == false)
    {
        return status;
    }

    memset((void *)&qspi_memory_xfer, 0, sizeof(qspi_memory_xfer_t));

    qspi_memory_xfer.instruction = SST26_CMD_PAGE_PROGRAM;
    qspi_memory_xfer.width = QUAD_CMD;

    status = dObj->sst26Plib->MemoryWrite(&qspi_memory_xfer, (uint32_t *)tx_data, DRV_SST26_PAGE_SIZE, address);

    gDrvSST26Obj.curOpType = DRV_SST26_OPERATION_TYPE_WRITE;

    return status;
}

static bool DRV_SST26_Erase( uint8_t instruction, uint32_t address )
{
    bool status = false;

    if (DRV_SST26_WriteEnable() == false)
    {
        return status;
    }

    qspi_command_xfer.instruction = instruction;
    qspi_command_xfer.width = QUAD_CMD;
    qspi_command_xfer.addr_en = 1;

    status = dObj->sst26Plib->CommandWrite(&qspi_command_xfer, address);

    gDrvSST26Obj.curOpType = DRV_SST26_OPERATION_TYPE_ERASE;

    return status;
}

bool DRV_SST26_SectorErase( const DRV_HANDLE handle, uint32_t address )
{
    if(handle == DRV_HANDLE_INVALID)
    {
        return false;
    }

    return (DRV_SST26_Erase(SST26_CMD_SECTOR_ERASE, address));
}

bool DRV_SST26_BulkErase( const DRV_HANDLE handle, uint32_t address )
{
    if(handle == DRV_HANDLE_INVALID)
    {
        return false;
    }

    return (DRV_SST26_Erase(SST26_CMD_BULK_ERASE_64K, address));
}

bool DRV_SST26_ChipErase( const DRV_HANDLE handle )
{
    if(handle == DRV_HANDLE_INVALID)
    {
        return false;
    }

    return (DRV_SST26_Erase(SST26_CMD_CHIP_ERASE, 0));
}

bool DRV_SST26_GeometryGet( const DRV_HANDLE handle, DRV_SST26_GEOMETRY *geometry )
{
    uint32_t flash_size = 0;
    uint8_t  jedec_id[3] = { 0 };

    if (DRV_SST26_ReadJedecId(handle, (void *)&jedec_id) == false)
    {
        return false;
    }

    flash_size = DRV_SST26_GetFlashSize(jedec_id[2]);

    if (flash_size == 0)
    {
        return false;
    }

    /* Read block size and number of blocks */
    geometry->read_blockSize = 1;
    geometry->read_numBlocks = flash_size;

    /* Write block size and number of blocks */
    geometry->write_blockSize = DRV_SST26_PAGE_SIZE;
    geometry->write_numBlocks = (flash_size / DRV_SST26_PAGE_SIZE);

    /* Erase block size and number of blocks */
    geometry->erase_blockSize = DRV_SST26_ERASE_SIZE;
    geometry->erase_numBlocks = (flash_size / DRV_SST26_ERASE_SIZE);

    geometry->numReadRegions = 1;
    geometry->numWriteRegions = 1;
    geometry->numEraseRegions = 1;

    geometry->blockStartAddress = DRV_SST26_START_ADDRESS;

    return true;
}

DRV_HANDLE DRV_SST26_Open( const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT ioIntent )
{
    if ((dObj->status != SYS_STATUS_READY) ||
        (dObj->nClients >= DRV_SST26_CLIENTS_NUMBER))
    {
        return DRV_HANDLE_INVALID;
    }

    dObj->nClients++;

    /* Reset SST26 Flash device */
    if (DRV_SST26_ResetFlash() == false)
    {
        return DRV_HANDLE_INVALID;
    }

    /* Put SST26 Flash device on QUAD IO Mode */
    if (DRV_SST26_EnableQuadIO() == false)
    {
        return DRV_HANDLE_INVALID;
    }

    if ((ioIntent & DRV_IO_INTENT_WRITE) == (DRV_IO_INTENT_WRITE))
    {
        /* Unlock the Flash */
        if (DRV_SST26_UnlockFlash((DRV_HANDLE)drvIndex) == false)
        {
            return DRV_HANDLE_INVALID;
        }
    }

    dObj->ioIntent = ioIntent;

    return ((DRV_HANDLE)drvIndex);
}

void DRV_SST26_Close( const DRV_HANDLE handle )
{
    if(handle != DRV_HANDLE_INVALID)
    {
        dObj->nClients--;
    }
}

SYS_MODULE_OBJ DRV_SST26_Initialize
(
    const SYS_MODULE_INDEX drvIndex,
    const SYS_MODULE_INIT *const init
)
{
    DRV_SST26_INIT *sst26Init = NULL;

    /* Check if the instance has already been initialized. */
    if (dObj->inUse == true)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    dObj->status    = SYS_STATUS_UNINITIALIZED;

    /* Indicate that this object is in use */
    dObj->inUse     = true;
    dObj->nClients  = 0;

    /* Assign to the local pointer the init data passed */
    sst26Init       = (DRV_SST26_INIT *)init;

    /* Initialize the attached memory device functions */
    dObj->sst26Plib = sst26Init->sst26Plib;

    dObj->status    = SYS_STATUS_READY;

    /* Return the driver index */
    return ( (SYS_MODULE_OBJ)drvIndex );
}

SYS_STATUS DRV_SST26_Status( const SYS_MODULE_INDEX drvIndex )
{
    /* Return the driver status */
    return (gDrvSST26Obj.status);
}
