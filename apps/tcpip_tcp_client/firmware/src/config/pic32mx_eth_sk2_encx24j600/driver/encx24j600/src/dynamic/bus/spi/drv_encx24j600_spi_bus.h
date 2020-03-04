/*******************************************************************************
  ENCx24J600 Driver SPI Bus Interface
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_spi_bus.h
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2014-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/

// DOM-IGNORE-END

#ifndef _ENCX24J600_SPI_BUS_H_
#define _ENCX24J600_SPI_BUS_H_

#include "../drv_encx24j600_bus.h"
#include "driver/spi/drv_spi.h"
#include <stdbool.h>

typedef struct _DRV_ENCX24J600_spiBusData
{
    DRV_HANDLE clientHandle;
    uintptr_t bufferHandles[20]; // change that 20 to something else
    uint8_t commandWrBuffers[20][4]; // change that 20 to something else
    uint8_t commandRdBuffers[20][4]; // change that 20 to something else
    uint8_t currentBank;
    bool bankSelectNeeded;

}DRV_ENCX24J600_spiBusData;

int32_t DRV_ENCX24J600_SPI_InitializeInterface(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance );
int32_t DRV_ENCX24J600_SPI_DeinitializeInterface(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance);

// *****************************************************************************
/* Open

    Summary:
    Opens the bus interface

    Details:
    This function call opens the bus interface.

    Preconditions:
    The bus has to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance

    Returns
        Negative if error
        Valid Handle on success
*/
int32_t DRV_ENCX24J600_SPI_OpenInterface(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance );

// *****************************************************************************
/* Close

    Summary:
    Closes the bus interface

    Details:
    This function call closes the bus interface.

    Preconditions:
    The bus has to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance

    Returns:
        None
*/
void DRV_ENCX24J600_SPI_CloseInterface( struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance);

// *****************************************************************************
/* Operation Result

    Summary
    Checks the status of an operation on the bus.

    Details
    This function checks the status of an operation that was previously start.

    Preconditions
    The bus had to have been initialized first.

    Parameters
        pDrvInstance – The driver instance
        handle - Handle created by the operation.

    Returns
        DRV_ENCX24J600_BR_SUCCESS – if the operation was successful
        DRV_ENCX24J600_BR_PENDING – if the operation is still pending
        DRV_ENCX24J600_BR_ERROR – if there was an error in the operation
*/
DRV_ENCX24J600_BUS_RESULT DRV_ENCX24J600_SPI_OperationResult( struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance, uintptr_t  handle );


// *****************************************************************************
/* Special Function Register Write

    Summary:
    Starts a write operation to the special function register.

    Details:
    This function sends a value to the specified register.  It uses an opIndex
    to help with chaining commands to the bus.  Each command to the bus can be
    on a different index to allow them to be buffered.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance
        reg – The Special Function Register to write to.
        Value – the value to write into the register
        opIndex – the index to use for this operation

    Returns:
        NULL – On Error
        Valid Handle – on success
*/
uintptr_t DRV_ENCX24J600_SPI_SfrWrite(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance, DRV_ENCX24J600_SFR_MAP  reg, DRV_ENCX24J600_RegUnion  value,  uint8_t  opIndex );

// *****************************************************************************
/* Special Function Register Read Start

    Summary:
    Starts a read operation from the special function register.

    Details
    This function starts a read operation of a value from the specified register.
    It uses an opIndex to help with chaining commands to the bus.  Each command
    to the bus can be on a different index to allow them to be buffered.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance
        reg – The Special Function Register to write to.
        opIndex – the index to use for this operation

    Returns
        NULL – On Error
        Valid Handle – on success
*/
uintptr_t DRV_ENCX24J600_SPI_SfrReadStart(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance, DRV_ENCX24J600_SFR_MAP  reg, uint8_t  opIndex );

// *****************************************************************************
/* Special Function Register Read Result

    Summary
    Gets the result of a read operation.

    Details
    This function checks the result of the read operation and if it was
    successful it will write the value into the output.

    Preconditions
    The bus had to have been initialized first.

    Parameters
        pDrvInstance – The driver instance
        handle – the handle from the read start operation
        value – where to put the results of the operation
        opIndex – the index to use for this operation

    Returns
        DRV_ENCX24J600_BR_SUCCESS – if the operation was successful
        DRV_ENCX24J600_BR_PENDING – if the operation is still pending
        DRV_ENCX24J600_BR_ERROR – if there was an error in the operation
*/
DRV_ENCX24J600_BUS_RESULT DRV_ENCX24J600_SPI_SfrReadResult(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance, uintptr_t  handle, DRV_ENCX24J600_RegUnion *  value,  uint8_t  opIndex );

// *****************************************************************************
/* Special Function Register Bit Set

    Summary
    Sets a selection of bits in a special function register.

    Details
    This function will allow the setting of specific bits in a register without
    having to read the register first.  The bits turned on in value are the bits
    that get set.

    Preconditions
    The bus had to have been initialized first.

    Parameters
        pDrvInstance – The driver instance
        reg – The Special Function Register to write to.
        Value – The bits to set in the register.
        opIndex – the index to use for this operation

    Returns
        NULL – On Error
        Valid Handle – on success
*/
uintptr_t DRV_ENCX24J600_SPI_SfrBitSet(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance, DRV_ENCX24J600_SFR_MAP  reg, DRV_ENCX24J600_RegUnion  value, uint8_t  opIndex );

// *****************************************************************************
/* Special Function Register Bit Clear

    Summary:
    Clears a selection of bits in a special function register.

    Details:
    This function will allow the clearing of specific bits in a register without
    having to read the register first.  The bits turned on in value are the bits
    that get cleared.

    Preconditions:
    The bus had to have been initialized first.

    Parameters
        pDrvInstance – The driver instance
        reg – The Special Function Register to write to.
        Value – The bits to clear in the register.
        opIndex – the index to use for this operation

    Returns
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENCX24J600_SPI_SfrBitClear(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance, DRV_ENCX24J600_SFR_MAP  reg, DRV_ENCX24J600_RegUnion  value, uint8_t  opIndex );

// *****************************************************************************
/* System Reset

    Summary:
    Sends a device reset.

    Details:
    This function sends the system reset command to the ENCx24J600.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance

    Returns:
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENCX24J600_SPI_SystemReset(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance );

// *****************************************************************************
/* Enable Receive

    Summary:
    Sends an enable RX.

    Details:
    This function sends the Enable RX command to the ENCx24J600.

    Preconditions:
    The bus had to have been initialized first.

    Parameters
        pDrvInstance – The driver instance

    Returns
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENCX24J600_SPI_EnableRX(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance );

// *****************************************************************************
/* Disable Receive

    Summary:
    Sends a disable RX.

    Details:
    This function sends the Disable RX command to the ENCx24J600.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance

    Returns
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENCX24J600_SPI_DisableRX(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance );

// *****************************************************************************
/* Request Packet Transmission

    Summary:
    Sends a request packet transmission.

    Details:
    This function sends the request packet transmission command to the
    ENCx24J600.  This command is called by the transmit state machine after a
    packet has been primed.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance

    Returns
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENCX24J600_SPI_ReqPktTx(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance );

// *****************************************************************************
/* Decrement Packet Counter

    Summary:
    Sends a decrement packet counter.

    Details:
    This function sends the Decrement Packet command to the ENCx24J600.  This is
    called by the RX state machine after it has received a packet.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance

    Returns
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENCX24J600_SPI_DecrPktCtr(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance );

// *****************************************************************************
/* Enable Interrupts

    Summary:
    Sends an enable interrupts.

    Details:
    This function sends the Enable Interrupts command to the ENCx24J600.

    Preconditions:
    The bus had to have been initialized first.

    Parameters
        pDrvInstance – The driver instance

    Returns
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENCX24J600_SPI_EnableInterrupts(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance );

// *****************************************************************************
/*  Disable Interrupts

    Summary:
    Sends a disable interrupts.

    Details:
    This function sends the Disable Interrupts command to the ENCx24J600.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance

    Returns:
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENCX24J600_SPI_DisableInterrupts(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance );

// *****************************************************************************
/*  Disable Flow Control

    Summary:
    Sends a disable flow control.

    Details
    This function sends the disable flow control command to the ENCx24J600.

    Preconditions:
    The bus had to have been initialized first.

    Parameters
        pDrvInstance – The driver instance

    Returns
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENCX24J600_SPI_FlowCtrlDisable(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance );

// *****************************************************************************
/*  Single Flow Control

    Summary:
    Sends a single flow control.

    Details:
    This function sends the single flow control command to the ENCx24J600.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance

    Returns
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENCX24J600_SPI_FlowCtrlSingle(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance );

// *****************************************************************************
/*  Multiple Flow Control

    Summary:
    Sends a multiple flow control.

    Details:
    This function sends the multiple flow control command to the ENCx24J600.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance

    Returns:
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENCX24J600_SPI_FlowCtrlMult(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance );

// *****************************************************************************
/*  Clear Flow Control

    Summary:
    Sends a clear flow control.

    Details:
    This function sends the clear flow control command to the ENCx24J600.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance

    Returns:
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENCX24J600_SPI_FlowCtrClear(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance );

// *****************************************************************************
/*  PHY Register Write

    Summary:
    Write to a PHY register on the ENC.

    Details:
    This function starts a write to the PHY on the ENC.  It is actually two bus
    operations so when opIndex is used it requires it and the next index to
    store the buffer and handle.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance
        reg – The PHY register to write
        value – the Value to write
        opIndex – the operation index.

    Returns:
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENCX24J600_SPI_PhyWrite(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance, DRV_ENCX24J600_PHY_SFR_MAP  reg, DRV_ENCX24J600_RegUnion  value, uint8_t  opIndex );

// *****************************************************************************
/*  Write Pointer

    Summary:
    Writes a value to one of the registers.

    Details:
    The ENC hardware has six registers.  This function writes a value to one of
    those registers.  The value refers to the memory location in the ENC hardware.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance
        reg – The register to write
        value – the value to write
        opIndex – the operation index.

    Returns:
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENCX24J600_SPI_WritePointer(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance, DRV_ENCX24J600_POINTER  reg, uint16_t  value, uint8_t  opIndex );

// *****************************************************************************
/*  Read Pointer Start

    Summary:
    Starts a read from one of the registers.

    Details:
    The ENC hardware has six registers.  This function starts a read operation
    from one of those registers.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance
        reg – The register to write
        opIndex – the operation index.

    Returns:
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENCX24J600_SPI_ReadPointerStart(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance, DRV_ENCX24J600_POINTER  reg, uint8_t  opIndex );

// *****************************************************************************
/*  Read Pointer Result

    Summary:
    Gets the result from a read pointer operation

    Details:
    This function gets the results from a read pointer operation

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance
        handle – From the read operation
        value – the location for the results
        opIndex – the operation index.

    Returns:
        DRV_ENCX24J600_BR_SUCCESS – if the operation was successful
        DRV_ENCX24J600_BR_PENDING – if the operation is still pending
        DRV_ENCX24J600_BR_ERROR – if there was an error in the operation
*/
 DRV_ENCX24J600_BUS_RESULT DRV_ENCX24J600_SPI_ReadPointerResult(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance, uintptr_t handle, uint16_t* value, uint8_t  opIndex );

// *****************************************************************************
/* Write Data

    Summary:
    Writes data to the ENC hardware

    Details:
    This function writes data to the ENC.

    Preconditions:
    The bus had to have been initialized first.  The parameters to this function
    are a little different than expected.  The TCP/IP packet segment is allocated
    with some space before the actual pointer in the packet.  For the PIC32
    Internal MAC this is two bytes.  This data is used by the MAC for its own
    purposes.  In the case of the ENCX24J600 only 1 byte is needed for SPI.  For
    PSP a different number may be needed.  The buffer that is passed in is the
    start of the data segment, and this function assumes there is some allocated
    space before the pointer.  The datasize is the size of the data portion, not
    the total size of the buffer.

    Parameters:
        pDrvInstance – The driver instance
        reg – the register to write to
        buffer – the location of the buffer to write
        dataSize – the size of the data to write.

    Returns:
        0 – on error
        Valid handle – on success
*/
 uintptr_t DRV_ENCX24J600_SPI_WriteData(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance, DRV_ENCX24J600_POINTER  reg, uint8_t *  buffer, uint16_t  dataSize);

// *****************************************************************************
/* Read Data Start

    Summary:
    Read data from the ENC hardware.

    Details:
    This function reads data from the ENC. The parameters to this function are a
    little different than expected.  The TCP/IP packet segment is allocated with
    some space before the actual pointer in the packet.  For the PIC32 Internal
    MAC this is two bytes.  This data is used by the MAC for its own purposes.
    n the case of the ENCX24J600 only 1 byte is needed for SPI.  For PSP a
    different number may be needed.  The buffer that is passed in is the start
    of the data segment, and this function assumes there is some allocated space
    before the pointer.  The datasize is the size of the data portion, not the
    total size of the buffer.

    Preconditions:
    The bus had to have been initialized first.

    Parameters
        pDrvInstance – The driver instance
        reg – the register to write to
        buffer – the location of the buffer to write
        dataSize – the size of the data to read.

    Returns:
        0 – on error
        Valid handle – on success
*/
 uintptr_t DRV_ENCX24J600_SPI_ReadData(struct _DRV_ENCX24J600_DriverInfo *  pDrvInstance, DRV_ENCX24J600_POINTER  reg, uint8_t *  buffer, uint16_t  dataSize);

 extern const DRV_ENCX24J600_BusVTable drv_encx24j600_spi_vtable;


#endif