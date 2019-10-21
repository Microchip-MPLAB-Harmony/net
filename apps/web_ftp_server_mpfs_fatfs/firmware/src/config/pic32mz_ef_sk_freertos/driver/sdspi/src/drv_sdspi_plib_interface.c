/******************************************************************************
  SD Card (SPI) Driver Interface Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    drv_sdspi_plib_interface.c

  Summary:
    SD Card (SPI) Driver PLIB Interface implementation

  Description:
    This interface file segregates the SD Card SPI protocol from the underlying
    hardware layer implementation for SPI and Timer System service
*******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#include <string.h>
#include "drv_sdspi_plib_interface.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global objects
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: DRV_SDSPI Driver Local Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Timer Event Handler

  Summary:
    Event handler registered by the SD card driver with the Timer System Service

  Description:
    This event handler is called by the Timer System Service when the requested
    time period has elapsed.

  Remarks:

*/

static void DRV_SDSPI_TimerCallback( uintptr_t context )
{
    bool *flag = (bool *)context;
    *flag = true;
}

// *****************************************************************************
/* SDSPI SPI PLIB Event Handler

  Summary:
    Event handler registered by the SD card driver with the SPI PLIB

  Description:
    This event handler is called by the SPI PLIB when a request is complete

  Remarks:
    Block transfers always block on a semaphore in an RTOS environment. This
    semaphore is released from this callback when DMA is not used.

*/

void _DRV_SDSPI_SPIPlibCallbackHandler( uintptr_t context )
{
    DRV_SDSPI_OBJ* dObj = (DRV_SDSPI_OBJ *)context;

    dObj->spiTransferStatus = DRV_SDSPI_SPI_TRANSFER_STATUS_COMPLETE;

    /* Only block transfers wait on semaphore. Post the semaphore and unblock the thread*/
    if (dObj->sdcardSPITransferType == DRV_SDSPI_SPI_TRANSFER_TYPE_BLOCK)
    {
        OSAL_SEM_PostISR( &dObj->transferDone);
    }
}

// *****************************************************************************
/* SDSPI RX DMA Event Handler

  Summary:
    Event handler registered by the SD card driver with the DMA System service
    for the receive DMA channel

  Description:
    This event handler is called by the DMA System Service when a DMA transfer
    is complete.

  Remarks:
    Block transfers always block on a semaphore in an RTOS environment. This
    semaphore is released from this callback when DMA is used.

*/

void _DRV_SDSPI_RX_DMA_CallbackHandler(
    SYS_DMA_TRANSFER_EVENT event,
    uintptr_t context
)
{
    uint32_t index;
    uint32_t size;
    DRV_SDSPI_OBJ* dObj = (DRV_SDSPI_OBJ *)context;

    if(event == SYS_DMA_TRANSFER_COMPLETE)
    {
        if (dObj->rxPending > 0)
        {
            /* txPending is 0. Need to use the dummy data buffer for transmission.
             * Find out the max data that can be received, given the limited size of the dummy data buffer.
             */
            (dObj->rxPending > sizeof(dObj->dummyDataBuffer)) ?
                (size = sizeof(dObj->dummyDataBuffer)): (size = dObj->rxPending);

            index = dObj->nBytesTransferred;

            /* Calculate the remaining rx bytes and total bytes transferred */
            dObj->rxPending -= size;
            dObj->nBytesTransferred += size;

            /* Always set up the rx channel first */
            SYS_DMA_ChannelTransfer(
                dObj->rxDMAChannel,
                (const void*)dObj->rxAddress,
                (const void *)&((uint8_t*)dObj->pReceiveData)[index],
                size
            );

            SYS_DMA_ChannelTransfer(
                dObj->txDMAChannel,
                (const void *)dObj->dummyDataBuffer,
                (const void*)dObj->txAddress,
                size
            );
        }
        else if (dObj->txPending > 0)
        {
            /* rxPending is 0. Need to use the dummy data buffer for reception.
             * Find out the max data that can be transmitted, given the limited size of the dummy data buffer.
             */
            (dObj->txPending > sizeof(dObj->dummyDataBuffer)) ?
                (size = sizeof(dObj->dummyDataBuffer)): (size = dObj->txPending);

            index = dObj->nBytesTransferred;

            /* Calculate the remaining tx bytes and total bytes transferred */
            dObj->txPending -= size;
            dObj->nBytesTransferred += size;

            /* Always set up the rx channel first */
            SYS_DMA_ChannelTransfer(
                dObj->rxDMAChannel,
                (const void*)dObj->rxAddress,
                (const void *)dObj->dummyDataBuffer,
                size
            );

            SYS_DMA_ChannelTransfer(
                dObj->txDMAChannel,
                (const void *)&((uint8_t*)dObj->pTransmitData)[index],
                (const void*)dObj->txAddress,
                size
            );
        }
        else
        {
            dObj->spiTransferStatus = DRV_SDSPI_SPI_TRANSFER_STATUS_COMPLETE;

            /* Only block transfers wait on semaphore. Post the semaphore and unblock the thread*/
            if (dObj->sdcardSPITransferType == DRV_SDSPI_SPI_TRANSFER_TYPE_BLOCK)
            {
                OSAL_SEM_PostISR( &dObj->transferDone);
            }
        }
    }
    else
    {
        dObj->spiTransferStatus = DRV_SDSPI_SPI_TRANSFER_STATUS_ERROR;

        /* Only block transfers wait on semaphore. Post the semaphore and unblock the thread*/
        if (dObj->sdcardSPITransferType == DRV_SDSPI_SPI_TRANSFER_TYPE_BLOCK)
        {
            OSAL_SEM_PostISR( &dObj->transferDone);
        }
    }
    
}

// *****************************************************************************
/* SDSPI TX DMA Event Handler

  Summary:
    Event handler registered by the SD card driver with the DMA System service
    for the transmit DMA channel

  Description:

  Remarks:

*/

void _DRV_SDSPI_TX_DMA_CallbackHandler(
    SYS_DMA_TRANSFER_EVENT event,
    uintptr_t context
)
{
    /* Do nothing */
}


// *****************************************************************************
/* SDSPI DMA Write

  Summary:
    Configures transmit and receive DMA channels for a write operation and starts
    the DMA transfer.

  Description:

  Remarks:

*/

static bool _DRV_SDSPI_DMA_Write(
    DRV_SDSPI_OBJ* dObj,
    void* pWriteBuffer,
    uint32_t nBytes
)
{
    uint32_t size;

    dObj->pTransmitData = pWriteBuffer;
    dObj->pReceiveData = NULL;
    dObj->txPending = nBytes;
    dObj->rxPending = 0;

    dObj->nBytesTransferred = 0;

    /* Initialize the dummy data buffer with 0xFF */
    memset(dObj->dummyDataBuffer, 0xFF, sizeof(dObj->dummyDataBuffer));

    /* Need to use the dummy data buffer for reception.
     * Find out the max data that can be transmitted, given the limited size of the dummy data buffer.
     */
    (dObj->txPending > sizeof(dObj->dummyDataBuffer)) ?
        (size = sizeof(dObj->dummyDataBuffer)): (size = dObj->txPending);

    /* Calculate the remaining tx bytes and total bytes transferred */
    dObj->txPending -= size;
    dObj->nBytesTransferred += size;

    /* Always set up the rx channel first */
    SYS_DMA_ChannelTransfer(
        dObj->rxDMAChannel,
        (const void*)dObj->rxAddress,
        (const void *)dObj->dummyDataBuffer,
        size
    );

    SYS_DMA_ChannelTransfer(
        dObj->txDMAChannel,
        (const void *)pWriteBuffer,
        (const void*)dObj->txAddress,
        size
    );

    return true;
}

// *****************************************************************************
/* SDSPI DMA Read

  Summary:
    Configures the transmit and receive DMA channels for a read operation and starts
    the DMA transfer.

  Description:

  Remarks:

*/

static bool _DRV_SDSPI_DMA_Read(
    DRV_SDSPI_OBJ* dObj,
    void* pReadBuffer,
    uint32_t nBytes
)
{
    uint32_t size;

    dObj->pReceiveData = pReadBuffer;
    dObj->pTransmitData = NULL;
    dObj->rxPending = nBytes;
    dObj->txPending = 0;
    dObj->nBytesTransferred = 0;

    /* Initialize the dummy data buffer with 0xFF */
    memset(dObj->dummyDataBuffer, 0xFF, sizeof(dObj->dummyDataBuffer));

    /* Need to use the dummy data buffer for transmission.
     * Find out the max data that can be received, given the limited size of the dummy data buffer.
     */
    (dObj->rxPending > sizeof(dObj->dummyDataBuffer)) ?
        (size = sizeof(dObj->dummyDataBuffer)): (size = dObj->rxPending);

    /* Calculate the remaining rx bytes and total bytes transferred */
    dObj->rxPending -= size;
    dObj->nBytesTransferred += size;

    /* Always set up the rx channel first */
    SYS_DMA_ChannelTransfer(
        dObj->rxDMAChannel,
        (const void*)dObj->rxAddress,
        (const void *)pReadBuffer,
        size
    );

    SYS_DMA_ChannelTransfer(
        dObj->txDMAChannel,
        (const void *)dObj->dummyDataBuffer,
        (const void*)dObj->txAddress,
        size
    );

    return true;
}
// *****************************************************************************
/* SDSPI Write Block

  Summary:
    Transfers a block (512) bytes of data over SPI using either DMA if it is
    configured or using the SPI PLIB.

  Description:

  Remarks:
    The function blocks on a semaphore which is released from the interrupt
    handler (either DMA handler or the SPI PLIB handler) once the transfer is
    complete.
*/

bool _DRV_SDSPI_SPIBlockWrite(
    DRV_SDSPI_OBJ* dObj,
    void* pWriteBuffer
)
{
    bool isSuccess = false;

    SYS_PORT_PinClear(dObj->chipSelectPin);

    dObj->sdcardSPITransferType = DRV_SDSPI_SPI_TRANSFER_TYPE_BLOCK;
    dObj->spiTransferStatus = DRV_SDSPI_SPI_TRANSFER_STATUS_IN_PROGRESS;

    /* If enabled, used DMA for block transfers */
    if ((dObj->txDMAChannel != SYS_DMA_CHANNEL_NONE) && (dObj->rxDMAChannel != SYS_DMA_CHANNEL_NONE ))
    {
        if (_DRV_SDSPI_DMA_Write(dObj, pWriteBuffer, _DRV_SDSPI_MEDIA_BLOCK_SIZE) == false)
        {
            SYS_PORT_PinSet(dObj->chipSelectPin);
            return isSuccess;
        }
    }
    else
    {
        if (dObj->spiPlib->write (pWriteBuffer, _DRV_SDSPI_MEDIA_BLOCK_SIZE) == false)
        {
            SYS_PORT_PinSet(dObj->chipSelectPin);
            return isSuccess;
        }
    }

    if (OSAL_SEM_Pend( &dObj->transferDone, OSAL_WAIT_FOREVER ) == OSAL_RESULT_TRUE)
    {
        if (dObj->spiTransferStatus == DRV_SDSPI_SPI_TRANSFER_STATUS_COMPLETE)
        {
            isSuccess = true;
        }
    }

    /* Make sure all the bytes have shifted out before de-asserting the CS */
    while(dObj->spiPlib->isBusy());

    SYS_PORT_PinSet(dObj->chipSelectPin);
    return isSuccess;
}

// *****************************************************************************
/* SD Card SPI Write

  Summary:
    Writes the requested number of bytes to the SD Card

  Description:

  Remarks:
    This is a blocking implementation. This function does not block on a semaphore.
*/

bool _DRV_SDSPI_SPIWrite(
    DRV_SDSPI_OBJ* dObj,
    void* pWriteBuffer,
    uint32_t nBytes
)
{
    bool isSuccess = false;

    SYS_PORT_PinClear(dObj->chipSelectPin);

    dObj->sdcardSPITransferType = DRV_SDSPI_SPI_TRANSFER_TYPE_BYTE;
    dObj->spiTransferStatus = DRV_SDSPI_SPI_TRANSFER_STATUS_IN_PROGRESS;

    /* If enabled, used DMA */
    if ((dObj->txDMAChannel != SYS_DMA_CHANNEL_NONE) && (dObj->rxDMAChannel != SYS_DMA_CHANNEL_NONE ))
    {
        if (_DRV_SDSPI_DMA_Write(dObj, pWriteBuffer, nBytes) == false)
        {
            SYS_PORT_PinSet(dObj->chipSelectPin);
            return isSuccess;
        }
    }
    else
    {
        if (dObj->spiPlib->write (pWriteBuffer, nBytes) == false)
        {
            SYS_PORT_PinSet(dObj->chipSelectPin);
            return isSuccess;
        }
    }

    /* Busy wait for the transfer to complete */
    while (dObj->spiTransferStatus == DRV_SDSPI_SPI_TRANSFER_STATUS_IN_PROGRESS);

    if (dObj->spiTransferStatus == DRV_SDSPI_SPI_TRANSFER_STATUS_COMPLETE)
    {
        isSuccess = true;
    }

    /* Make sure all the bytes have shifted out before de-asserting the CS */
    while(dObj->spiPlib->isBusy());

    SYS_PORT_PinSet(dObj->chipSelectPin);
    return isSuccess;
}

// *****************************************************************************
/* SDSPI Read Block

  Summary:
    Transfers a block (512) bytes of data over SPI using either DMA (if it is
    configured) or using the SPI PLIB.

  Description:

  Remarks:
    The function blocks on a semaphore which is released from the interrupt
    handler (either DMA handler or the SPI PLIB handler) once the transfer is
    complete.
*/

bool _DRV_SDSPI_SPIBlockRead(
    DRV_SDSPI_OBJ* dObj,
    void* pReadBuffer
)
{
    bool isSuccess = false;

    SYS_PORT_PinClear(dObj->chipSelectPin);

    dObj->sdcardSPITransferType = DRV_SDSPI_SPI_TRANSFER_TYPE_BLOCK;
    dObj->spiTransferStatus = DRV_SDSPI_SPI_TRANSFER_STATUS_IN_PROGRESS;

    /* If enabled, used DMA for block transfers */
    if ((dObj->txDMAChannel != SYS_DMA_CHANNEL_NONE) && (dObj->rxDMAChannel != SYS_DMA_CHANNEL_NONE ))
    {
        if (_DRV_SDSPI_DMA_Read(dObj, pReadBuffer, _DRV_SDSPI_MEDIA_BLOCK_SIZE) == false)
        {
            SYS_PORT_PinSet(dObj->chipSelectPin);
            return isSuccess;
        }
    }
    else
    {
        if (dObj->spiPlib->read(pReadBuffer, _DRV_SDSPI_MEDIA_BLOCK_SIZE) == false)
        {
            SYS_PORT_PinSet(dObj->chipSelectPin);
            return isSuccess;
        }
    }

    if (OSAL_SEM_Pend( &dObj->transferDone, OSAL_WAIT_FOREVER ) == OSAL_RESULT_TRUE)
    {
        if (dObj->spiTransferStatus == DRV_SDSPI_SPI_TRANSFER_STATUS_COMPLETE)
        {
            isSuccess = true;
        }
    }

    /* Make sure all the bytes have shifted out before de-asserting the CS */
    while(dObj->spiPlib->isBusy());

    SYS_PORT_PinSet(dObj->chipSelectPin);

    return isSuccess;
}

// *****************************************************************************
/* SD Card SPI read

  Summary:
    Reads the requested number of bytes from the SD Card

  Description:

  Remarks:
    This is a blocking implementation. This function does not block on a semaphore.
*/

bool _DRV_SDSPI_SPIRead(
    DRV_SDSPI_OBJ* dObj,
    void* pReadBuffer,
    uint32_t nBytes
)
{
    bool isSuccess = false;

    SYS_PORT_PinClear(dObj->chipSelectPin);

    dObj->sdcardSPITransferType = DRV_SDSPI_SPI_TRANSFER_TYPE_BYTE;
    dObj->spiTransferStatus = DRV_SDSPI_SPI_TRANSFER_STATUS_IN_PROGRESS;

    /* If enabled, used DMA */
    if ((dObj->txDMAChannel != SYS_DMA_CHANNEL_NONE) && (dObj->rxDMAChannel != SYS_DMA_CHANNEL_NONE ))
    {
        if (_DRV_SDSPI_DMA_Read(dObj, pReadBuffer, nBytes) == false)
        {
            SYS_PORT_PinSet(dObj->chipSelectPin);
            return isSuccess;
        }
    }
    else
    {
        if (dObj->spiPlib->read (pReadBuffer, nBytes) == false)
        {
            SYS_PORT_PinSet(dObj->chipSelectPin);
            return isSuccess;
        }
    }

    /* Busy wait for the transfer to complete */
    while (dObj->spiTransferStatus == DRV_SDSPI_SPI_TRANSFER_STATUS_IN_PROGRESS);

    if (dObj->spiTransferStatus == DRV_SDSPI_SPI_TRANSFER_STATUS_COMPLETE)
    {
        isSuccess = true;
    }

    /* Make sure all the bytes have shifted out before de-asserting the CS */
    while(dObj->spiPlib->isBusy());

    SYS_PORT_PinSet(dObj->chipSelectPin);

    return isSuccess;
}

bool _DRV_SDSPI_SPIWriteWithChipSelectDisabled(
    DRV_SDSPI_OBJ* dObj,
    void* pWriteBuffer,
    uint32_t nBytes
)
{
    bool isSuccess = false;

    SYS_PORT_PinSet(dObj->chipSelectPin);

    dObj->sdcardSPITransferType = DRV_SDSPI_SPI_TRANSFER_TYPE_BYTE;
    dObj->spiTransferStatus = DRV_SDSPI_SPI_TRANSFER_STATUS_IN_PROGRESS;

    /* If enabled, used DMA */
    if ((dObj->txDMAChannel != SYS_DMA_CHANNEL_NONE) && (dObj->rxDMAChannel != SYS_DMA_CHANNEL_NONE ))
    {
        if (_DRV_SDSPI_DMA_Write(dObj, pWriteBuffer, nBytes) == false)
        {
            return isSuccess;
        }
    }
    else
    {
        if (dObj->spiPlib->write (pWriteBuffer, nBytes) == false)
        {
            return isSuccess;
        }
    }

    /* Busy wait for the transfer to complete */
    while (dObj->spiTransferStatus == DRV_SDSPI_SPI_TRANSFER_STATUS_IN_PROGRESS);

    if (dObj->spiTransferStatus == DRV_SDSPI_SPI_TRANSFER_STATUS_COMPLETE)
    {
        isSuccess = true;
    }

    return isSuccess;
}

// *****************************************************************************
/* Command Response Timer Start

  Summary:
    Registers an event handler with the Timer System Service and starts the
    command-response timer.

  Description:
    The registered event handler is called when the time period elapses.

  Remarks:

*/
bool _DRV_SDSPI_CardDetectPollingTimerStart(
    DRV_SDSPI_OBJ* const dObj,
    uint32_t period
)
{
    bool isSuccess = false;
    dObj->cardPollingTimerExpired = false;

    dObj->cardPollingTmrHandle = SYS_TIME_CallbackRegisterMS(DRV_SDSPI_TimerCallback,
             (uintptr_t)&dObj->cardPollingTimerExpired, period, SYS_TIME_SINGLE);

    if (dObj->cardPollingTmrHandle != SYS_TIME_HANDLE_INVALID)
    {
        isSuccess = true;
    }

    return isSuccess;
}

// *****************************************************************************
/* Command Response Timer Start

  Summary:
    Registers an event handler with the Timer System Service and starts the
    command-response timer.

  Description:
    The registered event handler is called when the time period elapses.

  Remarks:

*/
bool _DRV_SDSPI_CmdResponseTimerStart(
    DRV_SDSPI_OBJ* const dObj,
    uint32_t period
)
{
    bool isSuccess = false;
    dObj->cmdRespTmrExpired = false;

    dObj->cmdRespTmrHandle = SYS_TIME_CallbackRegisterMS(DRV_SDSPI_TimerCallback,
             (uintptr_t)&dObj->cmdRespTmrExpired, period, SYS_TIME_SINGLE);

    if (dObj->cmdRespTmrHandle != SYS_TIME_HANDLE_INVALID)
    {
        isSuccess = true;
    }

    return isSuccess;
}

// *****************************************************************************
/* Command Response Timer Stop

  Summary:
    Stops the command-response timer.

  Description:

  Remarks:

*/

bool _DRV_SDSPI_CmdResponseTimerStop( DRV_SDSPI_OBJ* const dObj )
{
    bool isSuccess = false;

    if (dObj->cmdRespTmrHandle != SYS_TIME_HANDLE_INVALID)
    {
        SYS_TIME_TimerDestroy(dObj->cmdRespTmrHandle);
        isSuccess = true;
    }

    return isSuccess;
}

// *****************************************************************************
/* SD Card Timer Start

  Summary:
    Starts the SD card timer.

  Description:
    The registered event handler is called when the time period elapses.

  Remarks:

*/

bool _DRV_SDSPI_TimerStart(
    DRV_SDSPI_OBJ* const dObj,
    uint32_t period
)
{
    bool isSuccess = false;
    dObj->timerExpired = false;

    dObj->timerHandle = SYS_TIME_CallbackRegisterMS(DRV_SDSPI_TimerCallback,
             (uintptr_t)&dObj->timerExpired, period, SYS_TIME_SINGLE);

    if (dObj->timerHandle != SYS_TIME_HANDLE_INVALID)
    {
        isSuccess = true;
    }

    return isSuccess;
}

// *****************************************************************************
/* SD Card Timer Stop

  Summary:
    Stops the SD card timer.

  Description:
    The registered event handler is called when the time period elapses.

  Remarks:

*/

bool _DRV_SDSPI_TimerStop( DRV_SDSPI_OBJ* const dObj )
{
    bool isSuccess = false;

    if (dObj->timerHandle != SYS_TIME_HANDLE_INVALID)
    {
        SYS_TIME_TimerDestroy(dObj->timerHandle);
        isSuccess = true;
    }

    return isSuccess;
}

// *****************************************************************************
/* SD Card SPI Speed Setup

  Summary:
    Configures the SPI clock frequency.

  Description:
    This function is used by the SD Card driver to switch between the initial
    low frequency and to higher clock frequency once the SD card is initialized.

  Remarks:

*/

bool _DRV_SDSPI_SPISpeedSetup(
    DRV_SDSPI_OBJ* const dObj,
    uint32_t clockFrequency
)
{
    bool isSuccess = false;
    DRV_SDSPI_TRANSFER_SETUP sdspiSetup;
    DRV_SDSPI_TRANSFER_SETUP setupRemap;

    /* SD Card reads the data on the rising edge of SCK, which means SPI Mode 0
     * and 3 => CPOL = 0, CPHA = 0 and CPOL = 1, CPHA = 1 are supported */

    sdspiSetup.baudRateInHz = clockFrequency;
    sdspiSetup.clockPhase = DRV_SDSPI_CLOCK_PHASE_VALID_LEADING_EDGE;
    sdspiSetup.clockPolarity = DRV_SDSPI_CLOCK_POLARITY_IDLE_LOW;
    sdspiSetup.dataBits = DRV_SDSPI_DATA_BITS_8;

    setupRemap = sdspiSetup;

    setupRemap.clockPolarity = (DRV_SDSPI_CLOCK_POLARITY)dObj->remapClockPolarity[sdspiSetup.clockPolarity];
    setupRemap.clockPhase = (DRV_SDSPI_CLOCK_PHASE)dObj->remapClockPhase[sdspiSetup.clockPhase];
    setupRemap.dataBits = (DRV_SDSPI_DATA_BITS)dObj->remapDataBits[sdspiSetup.dataBits];

    if ((setupRemap.clockPhase != DRV_SDSPI_CLOCK_PHASE_INVALID) &&
        (setupRemap.clockPolarity != DRV_SDSPI_CLOCK_POLARITY_INVALID) &&
        (setupRemap.dataBits != DRV_SDSPI_DATA_BITS_INVALID))
    {
        isSuccess = dObj->spiPlib->transferSetup(&setupRemap, 0);
    }

    return isSuccess;
}
