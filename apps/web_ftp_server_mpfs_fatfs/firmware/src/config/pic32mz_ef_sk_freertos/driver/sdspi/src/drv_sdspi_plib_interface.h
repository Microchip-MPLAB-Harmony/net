/*******************************************************************************
  SD Card SPI Driver Interface Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    drv_sdspi_plib_interface.h

  Summary:
    SD Card (SPI) Driver PLIB Interface implementation

  Description:
    This interface file segregates the SD Card SPI protocol from the underlying
    hardware layer implementation.
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

#ifndef _DRV_SDSPI_PLIB_INTERFACE_H
#define _DRV_SDSPI_PLIB_INTERFACE_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "drv_sdspi_local.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

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

void _DRV_SDSPI_SPIPlibCallbackHandler( uintptr_t context );

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
);

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
);

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
);

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
);

// *****************************************************************************
/* SD Card SPI Write With CS Disabled

  Summary:
    Writes the requested number of bytes to the SD Card with CS disabled

  Description:

  Remarks:
    This is a blocking implementation. This function does not block on a semaphore.
*/

bool _DRV_SDSPI_SPIWriteWithChipSelectDisabled(
    DRV_SDSPI_OBJ* dObj,
    void* pWriteBuffer,
    uint32_t nBytes
);

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
);

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
);

// *****************************************************************************
/* Card Insertion/Removal Polling Timer

  Summary:
    Registers an event handler with the Timer System Service and starts the
    timer.

  Description:
    The registered event handler is called when the time period elapses.

  Remarks:

*/

bool _DRV_SDSPI_CardDetectPollingTimerStart(
    DRV_SDSPI_OBJ* const dObj,
    uint32_t period
);

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
);

// *****************************************************************************
/* SD Card Command-Response Timer Start

  Summary:
    Starts the command-response timer.

  Description:
    The registered event handler is called when the time period elapses.

  Remarks:

*/
bool _DRV_SDSPI_CmdResponseTimerStart(
    DRV_SDSPI_OBJ* const dObj,
    uint32_t period
);

// *****************************************************************************
/* SD Card Timer Stop

  Summary:
    Stops the SD card timer.

  Description:
    The registered event handler is called when the time period elapses.

  Remarks:

*/

bool _DRV_SDSPI_TimerStop( DRV_SDSPI_OBJ* const dObj );

// *****************************************************************************
/* SD Card Command-Response Timer Stop

  Summary:
    Stops the command-response timer.

  Description:
    None.

  Remarks:

*/
bool _DRV_SDSPI_CmdResponseTimerStop( DRV_SDSPI_OBJ* const dObj );

// *****************************************************************************
/* SD Card SPI Speed Setup

  Summary:
    Configures the SPI clock frequency by calling the SPI PLIB.

  Description:
    This function is used by the SD Card driver to switch between the initial
    low frequency and to higher clock frequency once the SD card is initialized.

  Remarks:

*/

bool _DRV_SDSPI_SPISpeedSetup(
    DRV_SDSPI_OBJ* const dObj,
    uint32_t clockFrequency
);

#endif //#ifndef _DRV_SDSPI_PLIB_INTERFACE_H
