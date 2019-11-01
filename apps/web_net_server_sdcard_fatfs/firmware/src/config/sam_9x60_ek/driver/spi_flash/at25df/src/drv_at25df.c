/******************************************************************************
  DRV_AT25DF Library Interface Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    drv_at25df.c

  Summary:
    AT25DF FLASH Driver Library Interface implementation

  Description:
    The AT25DF Library provides a interface to access the AT25DF external FLASH.
    This file implements the AT25DF Library interface.
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
#include "configuration.h"
#include "driver/spi_flash/at25df/drv_at25df.h"
#include "driver/spi_flash/at25df/src/drv_at25df_local.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global objects
// *****************************************************************************
// *****************************************************************************

#define TOTAL_DEVICE           1
#define DRV_AT25DF_ERASE_SIZE  4096

/* This is the driver instance object array. */
static DRV_AT25DF_OBJ gDrvAT25DFObj;

/* Flash Device ID Table*/
static uint32_t gAt25dfDeviceIdTable [TOTAL_DEVICE] = {
    0x0001471F
};

// *****************************************************************************
// *****************************************************************************
// Section: DRV_AT25DF Driver Local Functions
// *****************************************************************************
// *****************************************************************************

static bool _DRV_AT25DF_ReadData(void* rxData, uint32_t rxDataLength)
{
    bool status = false;

    /* Assert Chip Select */
    SYS_PORT_PinClear(gDrvAT25DFObj.chipSelectPin);

    if (gDrvAT25DFObj.spiPlib->read((uint8_t*)rxData, rxDataLength) == true)
    {
        status = true;
    }
    else
    {
        /* De-assert the chip select */
        SYS_PORT_PinSet(gDrvAT25DFObj.chipSelectPin);
    }

    return status;
}

static bool _DRV_AT25DF_WriteEnable(void)
{
    bool status = false;

    gDrvAT25DFObj.at25dfCommand[0] = DRV_AT25DF_CMD_WRITE_ENABLE;

    /* Assert Chip Select */
    SYS_PORT_PinClear(gDrvAT25DFObj.chipSelectPin);

    if(gDrvAT25DFObj.spiPlib->write(&gDrvAT25DFObj.at25dfCommand[0], 1) == true)
    {
        status = true;
    }
    else
    {
        /* De-assert the chip select */
        SYS_PORT_PinSet(gDrvAT25DFObj.chipSelectPin);
    }

    return status;
}

static bool _DRV_AT25DF_WriteMemoryAddress(uint8_t command, uint32_t address)
{
    bool status = false;
    uint32_t nBytes = 0;

    gDrvAT25DFObj.at25dfCommand[nBytes++] = command;

    if (gDrvAT25DFObj.flashSize > 65536)
    {
        gDrvAT25DFObj.at25dfCommand[nBytes++] = (uint8_t)(address>>16);
    }
    if (gDrvAT25DFObj.flashSize > 256)
    {
        gDrvAT25DFObj.at25dfCommand[nBytes++] = (uint8_t)(address>>8);
    }
    gDrvAT25DFObj.at25dfCommand[nBytes++] = (uint8_t)(address);

    /* Assert Chip Select */
    SYS_PORT_PinClear(gDrvAT25DFObj.chipSelectPin);

    /* Send page write or read command and memory address */
    if(gDrvAT25DFObj.spiPlib->write(gDrvAT25DFObj.at25dfCommand, nBytes) == true)
    {
        status = true;
    }
    else
    {
        /* De-assert the chip select */
        SYS_PORT_PinSet(gDrvAT25DFObj.chipSelectPin);
    }

    return status;
}

static bool _DRV_AT25DF_WriteData(void* txData, uint32_t txDataLength, uint32_t address )
{
    bool status = false;
    uint32_t nTransferBytes = 0;

    /* Calculate the max number of bytes that can be written in the current page */
    nTransferBytes = gDrvAT25DFObj.pageSize - (address % gDrvAT25DFObj.pageSize);

    /* Check if the pending bytes are greater than nTransferBytes */
    nTransferBytes = txDataLength >= nTransferBytes? nTransferBytes: txDataLength;

    gDrvAT25DFObj.memoryAddr = address + nTransferBytes;
    gDrvAT25DFObj.bufferAddr = (uint8_t *)((uint8_t*)txData + nTransferBytes);
    gDrvAT25DFObj.nPendingBytes = txDataLength - nTransferBytes;

    /* Assert Chip Select */
    SYS_PORT_PinClear(gDrvAT25DFObj.chipSelectPin);

    /* Send data */
    if (gDrvAT25DFObj.spiPlib->write((uint8_t*)txData, nTransferBytes) == true)
    {
        status = true;
    }
    else
    {
        /* De-assert the chip select */
        SYS_PORT_PinSet(gDrvAT25DFObj.chipSelectPin);
    }
    return status;
}

static bool _DRV_AT25DF_Write( void* txData, uint32_t txDataLength, uint32_t address )
{
    bool status = false;

    if ((address + txDataLength) > gDrvAT25DFObj.flashSize)
    {
        /* Writing past the flash size results in an error */
        return status;
    }

    gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_BUSY;

    /* Save the request */
    gDrvAT25DFObj.memoryAddr = address;
    gDrvAT25DFObj.bufferAddr = txData;
    gDrvAT25DFObj.nPendingBytes = txDataLength;

    gDrvAT25DFObj.state = DRV_AT25DF_STATE_WRITE_CMD_ADDR;

    /* Start the transfer by submitting a Write Enable request. Further commands
     * will be issued from the interrupt context.
    */
    if (_DRV_AT25DF_WriteEnable() == false)
    {
        gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_ERROR;
    }
    else
    {
        status = true;
    }

    return status;
}

static bool _DRV_AT25DF_Erase(uint8_t command, uint32_t address)
{
    bool status = false;

    gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_BUSY;

    /* Save the request */
    gDrvAT25DFObj.at25dfCommand[1] = command;
    gDrvAT25DFObj.memoryAddr = address;

    gDrvAT25DFObj.state = DRV_AT25DF_STATE_ERASE_DATA;

    /* Start the transfer by submitting a Write Enable request. Further commands
     * will be issued from the interrupt context.
    */
    if (_DRV_AT25DF_WriteEnable() == false)
    {
        gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_ERROR;
    }
    else
    {
        status = true;
    }

    return status;
}

static bool _DRV_AT25DF_EraseData(uint8_t command, uint32_t address)
{
    bool     status = false;
    uint32_t nBytes = 0;

    gDrvAT25DFObj.at25dfCommand[nBytes++] = command;

    if (command != DRV_AT25DF_CMD_CHIP_ERASE)
    {
        if (gDrvAT25DFObj.flashSize > 65536)
        {
            gDrvAT25DFObj.at25dfCommand[nBytes++] = (uint8_t)(address>>16);
        }
        if (gDrvAT25DFObj.flashSize > 256)
        {
            gDrvAT25DFObj.at25dfCommand[nBytes++] = (uint8_t)(address>>8);
        }
        gDrvAT25DFObj.at25dfCommand[nBytes++] = (uint8_t)(address);
    }

    /* Assert Chip Select */
    SYS_PORT_PinClear(gDrvAT25DFObj.chipSelectPin);

    /* Send Erase command */
    if(gDrvAT25DFObj.spiPlib->write(gDrvAT25DFObj.at25dfCommand, nBytes) == true)
    {
        status = true;
    }
    else
    {
        /* De-assert the chip select */
        SYS_PORT_PinSet(gDrvAT25DFObj.chipSelectPin);
    }

    return status;
}

static bool _DRV_AT25DF_ReadStatus(void)
{
    bool status = false;

    gDrvAT25DFObj.at25dfCommand[0] = DRV_AT25DF_CMD_READ_STATUS_REG;

    /* Assert Chip Select */
    SYS_PORT_PinClear(gDrvAT25DFObj.chipSelectPin);

    /* Read 1-byte dummy and then 2-byte read status register value */
    if(gDrvAT25DFObj.spiPlib->writeRead(&gDrvAT25DFObj.at25dfCommand[0], 1, \
            &gDrvAT25DFObj.at25dfCommand[1], 3 ) == true)
    {
        status = true;
    }
    else
    {
        /* De-assert the chip select */
        SYS_PORT_PinSet(gDrvAT25DFObj.chipSelectPin);
    }

    return status;
}

static bool _DRV_AT25DF_UnlockFlash(void)
{
    bool status = false;

    gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_BUSY;
    gDrvAT25DFObj.state = DRV_AT25DF_STATE_UNLOCK_FLASH;

    /* Start the transfer by submitting a Write Enable request. Further commands
     * will be issued from the interrupt context.
    */
    if (_DRV_AT25DF_WriteEnable() == false)
    {
        gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_ERROR;
    }
    else
    {
        status = true;
    }

    return status;
}

static bool _DRV_AT25DF_ReadJedecId(void)
{
    bool status = false;

    gDrvAT25DFObj.at25dfCommand[0] = DRV_AT25DF_CMD_JEDEC_ID_READ;

    gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_BUSY;
    gDrvAT25DFObj.state = DRV_AT25DF_STATE_READ_JEDECID;

    /* Assert Chip Select */
    SYS_PORT_PinClear(gDrvAT25DFObj.chipSelectPin);

    if(gDrvAT25DFObj.spiPlib->write(&gDrvAT25DFObj.at25dfCommand[0], 1) == true)
    {
        status = true;
    }
    else
    {
        /* De-assert the chip select */
        SYS_PORT_PinSet(gDrvAT25DFObj.chipSelectPin);
    }

    return status;
}

static bool _DRV_AT25DF_CheckJedecId(void)
{
    bool status = false;
    uint8_t i   = 0;

    for (i = 0; i < TOTAL_DEVICE; i++)
    {
        if (gAt25dfDeviceIdTable[i] == gDrvAT25DFObj.jedecId)
        {
            status = true;
            break;
        }
    }

    return status;
}

static void _DRV_AT25DF_Handler( void )
{
    switch(gDrvAT25DFObj.state)
    {
        case DRV_AT25DF_STATE_WRITE_CMD_ADDR:
            /* De-assert the chip select */
            SYS_PORT_PinSet(gDrvAT25DFObj.chipSelectPin);
            /* Send page write command and memory address */
            if (_DRV_AT25DF_WriteMemoryAddress(DRV_AT25DF_CMD_PAGE_PROGRAM,
                                               gDrvAT25DFObj.memoryAddr) == true)
            {
                gDrvAT25DFObj.state = DRV_AT25DF_STATE_WRITE_DATA;
            }
            else
            {
                gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_ERROR;
            }
            break;
        case DRV_AT25DF_STATE_WRITE_DATA:

            if (_DRV_AT25DF_WriteData(gDrvAT25DFObj.bufferAddr,
                                      gDrvAT25DFObj.nPendingBytes, gDrvAT25DFObj.memoryAddr) == true)
            {
                gDrvAT25DFObj.state = DRV_AT25DF_STATE_CHECK_WRITE_STATUS;
            }
            else
            {
                gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_ERROR;
            }
            break;
        case DRV_AT25DF_STATE_CHECK_WRITE_STATUS:
            /* De-assert the chip select */
            SYS_PORT_PinSet(gDrvAT25DFObj.chipSelectPin);
            /* Read the status of FLASH internal write cycle */
            if (_DRV_AT25DF_ReadStatus() == true)
            {
                gDrvAT25DFObj.state = DRV_AT25DF_STATE_WAIT_WRITE_COMPLETE;
            }
            else
            {
                gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_ERROR;
            }
            break;
        case DRV_AT25DF_STATE_WAIT_WRITE_COMPLETE:
            /* De-assert the chip select */
            SYS_PORT_PinSet(gDrvAT25DFObj.chipSelectPin);
            /* Check the busy bit in the status register. 0 = Ready, 1 = busy*/
            if (gDrvAT25DFObj.at25dfCommand[3] & 0x01)
            {
                /* Keep reading the status of FLASH internal write cycle */
                if (_DRV_AT25DF_ReadStatus() == false)
                {
                    gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_ERROR;
                }
            }
            else
            {
                /* Internal write complete. Now check if more data pending */
                if (gDrvAT25DFObj.nPendingBytes)
                {
                    /* Enable writing to FLASH */
                    if (_DRV_AT25DF_WriteEnable() == false)
                    {
                        gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_ERROR;
                    }
                    else
                    {
                        gDrvAT25DFObj.state = DRV_AT25DF_STATE_WRITE_CMD_ADDR;
                    }
                }
                else
                {
                    gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_COMPLETED;
                }
            }
            break;

        case DRV_AT25DF_STATE_READ_DATA:

            if (_DRV_AT25DF_ReadData((void*)gDrvAT25DFObj.bufferAddr, gDrvAT25DFObj.nPendingBytes) == true)
            {
                gDrvAT25DFObj.state = DRV_AT25DF_STATE_WAIT_READ_COMPLETE;
            }
            else
            {
                gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_ERROR;
            }
            break;

        case DRV_AT25DF_STATE_WAIT_READ_COMPLETE:
            /* De-assert the chip select */
            SYS_PORT_PinSet(gDrvAT25DFObj.chipSelectPin);
            gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_COMPLETED;
            break;

        case DRV_AT25DF_STATE_ERASE_DATA:
            /* De-assert the chip select */
            SYS_PORT_PinSet(gDrvAT25DFObj.chipSelectPin);
            /* Send Erase command and memory address */
            if (_DRV_AT25DF_EraseData(gDrvAT25DFObj.at25dfCommand[1],
                                      gDrvAT25DFObj.memoryAddr) == true)
            {
                gDrvAT25DFObj.state = DRV_AT25DF_STATE_CHECK_ERASE_STATUS;
            }
            else
            {
                gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_ERROR;
            }
            break;

        case DRV_AT25DF_STATE_UNLOCK_FLASH:
            /* De-assert the chip select */
            SYS_PORT_PinSet(gDrvAT25DFObj.chipSelectPin);
            /* Global Unprotect Flash command */
            gDrvAT25DFObj.at25dfCommand[0] = DRV_AT25DF_CMD_WRITE_STATUS_REG;
            gDrvAT25DFObj.at25dfCommand[1] = 0x00;
            /* Assert Chip Select and Send Global Unprotect Flash command */
            SYS_PORT_PinClear(gDrvAT25DFObj.chipSelectPin);
            if (gDrvAT25DFObj.spiPlib->write(&gDrvAT25DFObj.at25dfCommand[0], 2) == true)
            {
                gDrvAT25DFObj.state = DRV_AT25DF_STATE_CHECK_UNLOCK_FLASH_STATUS;
            }
            else
            {
                gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_ERROR;
            }
            break;

        case DRV_AT25DF_STATE_READ_JEDECID:
            if (gDrvAT25DFObj.spiPlib->read((uint8_t*)&gDrvAT25DFObj.jedecId, 4) == true)
            {
                gDrvAT25DFObj.state = DRV_AT25DF_STATE_WAIT_JEDECID_COMPLETE;
            }
            else
            {
                gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_ERROR;
            }
            break;

        case DRV_AT25DF_STATE_WAIT_JEDECID_COMPLETE:
            /* De-assert the chip select */
            SYS_PORT_PinSet(gDrvAT25DFObj.chipSelectPin);
            if (_DRV_AT25DF_CheckJedecId() == true)
            {
                gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_COMPLETED;
            }
            else
            {
                gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_ERROR;
            }
            break;

        case DRV_AT25DF_STATE_CHECK_ERASE_STATUS:
        case DRV_AT25DF_STATE_CHECK_UNLOCK_FLASH_STATUS:
            /* De-assert the chip select */
            SYS_PORT_PinSet(gDrvAT25DFObj.chipSelectPin);
            /* Read the status of FLASH internal cycle */
            if (_DRV_AT25DF_ReadStatus() == true)
            {
                if (gDrvAT25DFObj.state == DRV_AT25DF_STATE_CHECK_ERASE_STATUS)
                {
                    gDrvAT25DFObj.state = DRV_AT25DF_STATE_WAIT_ERASE_COMPLETE;
                }
                else
                {
                    gDrvAT25DFObj.state = DRV_AT25DF_STATE_WAIT_UNLOCK_FLASH_COMPLETE;
                }
            }
            else
            {
                gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_ERROR;
            }
            break;

        case DRV_AT25DF_STATE_WAIT_ERASE_COMPLETE:
        case DRV_AT25DF_STATE_WAIT_UNLOCK_FLASH_COMPLETE:
            /* De-assert the chip select */
            SYS_PORT_PinSet(gDrvAT25DFObj.chipSelectPin);
            /* Check the busy bit in the status register. 0 = Ready, 1 = busy*/
            if (gDrvAT25DFObj.at25dfCommand[3] & 0x01)
            {
                /* Keep reading the status of FLASH internal cycle */
                if (_DRV_AT25DF_ReadStatus() == false)
                {
                    gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_ERROR;
                }
            }
            else
            {
                gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_COMPLETED;
            }
            break;

        default:
            break;
    }
}

/* This function will be called by SPI PLIB when transfer is completed */
static void _SPIEventHandler(uintptr_t context )
{
    _DRV_AT25DF_Handler ();

    /* If transfer is complete, notify the application */
    if (gDrvAT25DFObj.transferStatus != DRV_AT25DF_TRANSFER_STATUS_BUSY)
    {
        if (gDrvAT25DFObj.eventHandler)
        {
            gDrvAT25DFObj.eventHandler(gDrvAT25DFObj.transferStatus, gDrvAT25DFObj.context);
        }
    }
}


// *****************************************************************************
// *****************************************************************************
// Section: DRV_AT25DF Driver Global Functions
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ DRV_AT25DF_Initialize(
    const SYS_MODULE_INDEX drvIndex,
    const SYS_MODULE_INIT * const init
)
{
    DRV_AT25DF_INIT* at25dfInit = (DRV_AT25DF_INIT *)init;

    /* Validate the request */
    if(drvIndex >= DRV_AT25DF_INSTANCES_NUMBER)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    if(gDrvAT25DFObj.inUse == true)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    gDrvAT25DFObj.status                = SYS_STATUS_UNINITIALIZED;

    gDrvAT25DFObj.inUse                 = true;
    gDrvAT25DFObj.nClients              = 0;
    gDrvAT25DFObj.transferStatus        = DRV_AT25DF_TRANSFER_STATUS_COMPLETED;
    gDrvAT25DFObj.writeCompleted        = true;

    gDrvAT25DFObj.spiPlib               = at25dfInit->spiPlib;
    gDrvAT25DFObj.nClientsMax           = at25dfInit->numClients;
    gDrvAT25DFObj.pageSize              = at25dfInit->pageSize;
    gDrvAT25DFObj.flashSize             = at25dfInit->flashSize;
    gDrvAT25DFObj.blockStartAddress     = at25dfInit->blockStartAddress;
    gDrvAT25DFObj.chipSelectPin         = at25dfInit->chipSelectPin;

    gDrvAT25DFObj.spiPlib->callbackRegister(_SPIEventHandler, (uintptr_t)NULL);

    /* De-assert Chip Select pin to begin with. */
    SYS_PORT_PinSet(gDrvAT25DFObj.chipSelectPin);

    /* Update the status */
    gDrvAT25DFObj.status = SYS_STATUS_READY;

    /* Return the object structure */
    return ( (SYS_MODULE_OBJ)drvIndex );

}

SYS_STATUS DRV_AT25DF_Status( const SYS_MODULE_INDEX drvIndex )
{
    /* Return the driver status */
    return (gDrvAT25DFObj.status);
}

DRV_HANDLE DRV_AT25DF_Open(
    const SYS_MODULE_INDEX drvIndex,
    const DRV_IO_INTENT ioIntent
)
{
    /* Validate the request */
    if (drvIndex >= DRV_AT25DF_INSTANCES_NUMBER)
    {
        return DRV_HANDLE_INVALID;
    }

    if((gDrvAT25DFObj.status != SYS_STATUS_READY) || (gDrvAT25DFObj.inUse == false) \
            || (gDrvAT25DFObj.nClients >= gDrvAT25DFObj.nClientsMax))
    {
        return DRV_HANDLE_INVALID;
    }

    /* Unlock the Flash */
    if (_DRV_AT25DF_UnlockFlash() == false)
    {
        return DRV_HANDLE_INVALID;
    }
    while (gDrvAT25DFObj.transferStatus != DRV_AT25DF_TRANSFER_STATUS_COMPLETED);

    gDrvAT25DFObj.nClients++;

    return ((DRV_HANDLE)0);
}

void DRV_AT25DF_Close( const DRV_HANDLE handle )
{
    if((handle != DRV_HANDLE_INVALID) && (handle == 0))
    {
        gDrvAT25DFObj.nClients--;
    }
}

void DRV_AT25DF_EventHandlerSet(
    const DRV_HANDLE handle,
    const DRV_AT25DF_EVENT_HANDLER eventHandler,
    const uintptr_t context
)
{
    if((handle != DRV_HANDLE_INVALID) && (handle == 0))
    {
        gDrvAT25DFObj.eventHandler = eventHandler;
        gDrvAT25DFObj.context = context;
    }
}

bool DRV_AT25DF_Read(
    const DRV_HANDLE handle,
    void* rxData,
    uint32_t rxDataLength,
    uint32_t address
)
{
    bool isRequestAccepted = false;

    if((handle == DRV_HANDLE_INVALID) || (handle > 0) || (rxData == NULL) \
            || (rxDataLength == 0) || (gDrvAT25DFObj.transferStatus == DRV_AT25DF_TRANSFER_STATUS_BUSY))
    {
        return isRequestAccepted;
    }

    if ((address + rxDataLength) > gDrvAT25DFObj.flashSize)
    {
        /* Writing past the flash size results in an error */
        return isRequestAccepted;
    }

    gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_BUSY;

    /* save the request */
    gDrvAT25DFObj.nPendingBytes = rxDataLength;
    gDrvAT25DFObj.bufferAddr = rxData;
    gDrvAT25DFObj.memoryAddr = address;

    gDrvAT25DFObj.state = DRV_AT25DF_STATE_READ_CMD_ADDR;

    if (_DRV_AT25DF_WriteMemoryAddress(DRV_AT25DF_CMD_READ, address) == true)
    {
        gDrvAT25DFObj.state = DRV_AT25DF_STATE_READ_DATA;
        isRequestAccepted = true;
    }
    else
    {
        gDrvAT25DFObj.transferStatus = DRV_AT25DF_TRANSFER_STATUS_ERROR;
    }

    return isRequestAccepted;
}

bool DRV_AT25DF_Write( const DRV_HANDLE handle, void *txData, uint32_t txDataLength, uint32_t address )
{
    if((handle == DRV_HANDLE_INVALID) || (handle > 0) || (txData == NULL) \
            || (txDataLength == 0) || (gDrvAT25DFObj.transferStatus == DRV_AT25DF_TRANSFER_STATUS_BUSY))
    {
        return false;
    }
    else
    {
        return _DRV_AT25DF_Write(txData, txDataLength, address);
    }
}

bool DRV_AT25DF_PageWrite(const DRV_HANDLE handle, void *txData, uint32_t address )
{
    return DRV_AT25DF_Write(handle, txData, gDrvAT25DFObj.pageSize, address );
}

bool DRV_AT25DF_SectorErase(const DRV_HANDLE handle, uint32_t address)
{
    return (_DRV_AT25DF_Erase(DRV_AT25DF_CMD_SECTOR_ERASE_4K, address));
}

bool DRV_AT25DF_BlockErase(const DRV_HANDLE handle, uint32_t address)
{
    return (_DRV_AT25DF_Erase(DRV_AT25DF_CMD_BLOCK_ERASE_64K, address));
}

bool DRV_AT25DF_ChipErase(const DRV_HANDLE handle)
{
    return (_DRV_AT25DF_Erase(DRV_AT25DF_CMD_CHIP_ERASE, 0));
}

DRV_AT25DF_TRANSFER_STATUS DRV_AT25DF_TransferStatusGet(const DRV_HANDLE handle)
{
    if((handle == DRV_HANDLE_INVALID) || (handle > 0))
    {
        return DRV_AT25DF_TRANSFER_STATUS_ERROR;
    }
    else
    {
        return gDrvAT25DFObj.transferStatus;
    }
}

bool DRV_AT25DF_GeometryGet(const DRV_HANDLE handle, DRV_AT25DF_GEOMETRY *geometry)
{
    if((handle == DRV_HANDLE_INVALID) || (handle > 0))
    {
        return false;
    }

    if (_DRV_AT25DF_ReadJedecId() == false)
    {
        return false;
    }

    /* Read block size and number of blocks */
    geometry->readBlockSize = 1;
    geometry->readNumBlocks = gDrvAT25DFObj.flashSize;

    /* Write block size and number of blocks */
    geometry->writeBlockSize = gDrvAT25DFObj.pageSize;
    geometry->writeNumBlocks = gDrvAT25DFObj.flashSize/gDrvAT25DFObj.pageSize;

    /* Erase block size and number of blocks */
    geometry->eraseBlockSize = DRV_AT25DF_ERASE_SIZE;
    geometry->eraseNumBlocks = gDrvAT25DFObj.flashSize/DRV_AT25DF_ERASE_SIZE;

    /* Number of regions */
    geometry->readNumRegions = 1;
    geometry->writeNumRegions = 1;
    geometry->eraseNumRegions = 1;

    /* Block start address */
    geometry->blockStartAddress = gDrvAT25DFObj.blockStartAddress;

    return true;
}
