/*******************************************************************************
  ENC28J60 Driver SPI Bus Interface
  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_spi_bus.h
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2015-2018 Microchip Technology Inc. and its subsidiaries.

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

#ifndef _ENC28J60_SPI_BUS_H_
#define _ENC28J60_SPI_BUS_H_


#include "../../drv_enc28j60_local.h"
#include "../drv_enc28j60_bus.h"
#include "../../initialization_state/drv_enc28j60_configure_state.h"
#include "driver/spi/drv_spi.h"

// number of SPI operations in parallel for this driver
#define     DRV_ENC28J60_SPI_PARALLEL_OP_NO     20

// number of external buffers to be created
// i.e. # of parallel multi-segment tx transfers that can occur
//
#define DRV_ENC28J60_OP_EXT_BUFFERS     2

// size of the external operation buffer
// large enough to accomodate the largest packet
// that needs to be processed (transmitted)
// For now the largest possible frame, no CRC
#define DRV_ENC28J60_OP_EXT_BUFFER_SIZE         1514
// extra size for SPI operations
#define DRV_ENC28J60_OP_EXT_BUFFER_EXTRA_SIZE   2


// size of the internal operation write and read buffers
#define DRV_ENC28J60_OP_WR_INT_BUFFER_SIZE     4
#define DRV_ENC28J60_OP_RD_INT_BUFFER_SIZE     4

// number of retries for a PHY operation
#define DRV_ENC28J60_PHY_OP_RETRIES         3


// operation extended buffer descriptor
// for operation needing extended buffers
typedef struct _tag_DRV_ENC28J60_OP_EXT_BUFF
{
    struct  _tag_DRV_ENC28J60_OP_EXT_BUFF*   next;              // safe cast to a linked node
    uint8_t extBuffer[DRV_ENC28J60_OP_EXT_BUFFER_SIZE + DRV_ENC28J60_OP_EXT_BUFFER_EXTRA_SIZE];     // always 2 extra byte for SPI ops + control byte
}DRV_ENC28J60_OP_EXT_BUFF;


// status of an ongoing operation
typedef enum
{
    DRV_ENC28J60_OP_STAT_IDLE,      // no op in progress, entry is not used
    DRV_ENC28J60_OP_STAT_BUSY,      // op in progress, entry is busy
    DRV_ENC28J60_OP_STAT_SUCCESS,   // op completed successfully
    DRV_ENC28J60_OP_STAT_ERROR,     // op failed
}DRV_ENC28J60_OP_STATUS;

typedef enum
{
    DRV_ENC28J60_OP_FLAG_NONE           = 0,        // no flag set
    DRV_ENC28J60_OP_FLAG_AUTO_ACK       = 0x01,     // operation auto acknowledges
    DRV_ENC28J60_OP_FLAG_LINKED         = 0x02,     // linked node

}DRV_ENC28J60_OP_FLAGS;

// operation descriptor
typedef struct _tag_DRV_ENC28J60_OP_DCPT
{
    struct _tag_DRV_ENC28J60_OP_DCPT*   next;   // safe cast to a linked node
    DRV_ENC28J60_DriverInfo*    pDrvInstance;   // driver it belongs to
    DRV_SPI_TRANSFER_HANDLE       spiHandle;      // ongoing SPI operation handle; 
    DRV_ENC28J60_OP_EXT_BUFF*   extBuffer;      // for operations needing an external buffer
    struct _tag_DRV_ENC28J60_OP_DCPT*   link;   // for linked ops
    uint8_t                     opWrBuffer[DRV_ENC28J60_OP_WR_INT_BUFFER_SIZE];  // internal buffer for short write operations
    uint8_t                     opRdBuffer[DRV_ENC28J60_OP_RD_INT_BUFFER_SIZE];  // internal buffer for short read operations
    uint8_t                     opStatus;       // DRV_ENC28J60_OP_STATUS: current status
    uint8_t                     opFlags;        // DRV_ENC28J60_OP_FLAGS
    uint8_t                     padding[2];     // not used

}DRV_ENC28J60_OP_DCPT;

typedef struct _DRV_ENC28J60_spiBusData
{
    DRV_HANDLE              clientHandle;
    SINGLE_LIST             opFreeList;     // list of free operation descriptors that can be used
    SINGLE_LIST             opListAutoAck;  // descriptors that need to be auto acknowledged once they're done
    SINGLE_LIST             extBuffFreeList;   // list of free external buffers that can be used
    DRV_ENC28J60_OP_DCPT    opArray[DRV_ENC28J60_SPI_PARALLEL_OP_NO]; // all available operations
    DRV_ENC28J60_OP_EXT_BUFF extBuffArray[DRV_ENC28J60_OP_EXT_BUFFERS];    // all available external buffers
    uint16_t                extBuffSize;        // size of the external buffer
    uint8_t                 currentBank;
    uint8_t                 padding[1];     // padding, not used
    // statistics
    uint32_t                spiOkCnt;       // number of successful SPI operations
    uint32_t                spiFailCnt;     // number of failed SPI operations
    uint32_t                opDcptOkCnt;    // number of successful attempts to get op descriptor
    uint32_t                opDcptFailCnt;  // number of failed attempts to get an operation descriptor
    uint32_t                extBuffOkCnt;   // number of successful attempts to get an external buffer
    uint32_t                extBuffFailCnt;  // number of failed attempts to get an external buffer
    
}DRV_ENC28J60_spiBusData;

int32_t DRV_ENC28J60_SPI_InitializeInterface(DRV_ENC28J60_DriverInfo *  pDrvInstance );
int32_t DRV_ENC28J60_SPI_DeinitializeInterface(DRV_ENC28J60_DriverInfo *  pDrvInstance);

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
int32_t DRV_ENC28J60_SPI_OpenInterface(DRV_ENC28J60_DriverInfo *  pDrvInstance );

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
void DRV_ENC28J60_SPI_CloseInterface( DRV_ENC28J60_DriverInfo *  pDrvInstance);

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
        DRV_ENC28J60_BR_SUCCESS – if the operation was successful
        DRV_ENC28J60_BR_PENDING – if the operation is still pending
        DRV_ENC28J60_BR_ERROR – if there was an error in the operation
*/
DRV_ENC28J60_BUS_RESULT DRV_ENC28J60_SPI_OperationResult( DRV_ENC28J60_DriverInfo *  pDrvInstance, uintptr_t  handle, bool ack );


// *****************************************************************************
/* Special Function Register Write

    Summary:
    Starts a write operation to the special function register.

    Details:
    This function sends a value to the specified register.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance
        reg – The Special Function Register to write to.
        Value – the value to write into the register

    Returns:
        NULL – On Error
        Valid Handle – on success
*/
uintptr_t DRV_ENC28J60_SPI_SfrWrite(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  sfr, DRV_ENC28J60_RegUnion  value, bool autoAck);

// write 2 registers at a time - 16 bit operation
uintptr_t DRV_ENC28J60_SPI_SfrWrite16(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  sfrLow, uint16_t regValue, bool autoAck);

// *****************************************************************************
/* Special Function Register Read Start

    Summary:
    Starts a read operation from the special function register.

    Details
    This function starts a read operation of a value from the specified register.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance
        reg – The Special Function Register to write to.

    Returns
        NULL – On Error
        Valid Handle – on success
*/
uintptr_t DRV_ENC28J60_SPI_SfrReadStart(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  reg, bool autoAck);

// same read operation in 16 bits
uintptr_t DRV_ENC28J60_SPI_SfrRead16Start(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  sfrLow, bool autoAck);

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

    Returns
        DRV_ENC28J60_BR_SUCCESS – if the operation was successful
        DRV_ENC28J60_BR_PENDING – if the operation is still pending
        DRV_ENC28J60_BR_ERROR – if there was an error in the operation
*/
DRV_ENC28J60_BUS_RESULT DRV_ENC28J60_SPI_SfrReadResult(DRV_ENC28J60_DriverInfo *  pDrvInstance, uintptr_t  handle, DRV_ENC28J60_RegUnion *  value, bool ack);

DRV_ENC28J60_BUS_RESULT DRV_ENC28J60_SPI_SfrRead16Result(DRV_ENC28J60_DriverInfo *  pDrvInstance, uintptr_t  handle, uint16_t* value, bool ack);

DRV_ENC28J60_PHY_RES DRV_ENC28J60_SPI_PhyReadStart(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_PHY_SFR_MAP  reg);

DRV_ENC28J60_PHY_RES DRV_ENC28J60_SPI_PhyRead(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_PHY_SFR_MAP reg, uint16_t* phyRegValue);

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

    Returns
        NULL – On Error
        Valid Handle – on success
*/
uintptr_t DRV_ENC28J60_SPI_SfrBitSet(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  reg, DRV_ENC28J60_RegUnion  value, bool autoAck);

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

    Returns
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENC28J60_SPI_SfrBitClear(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  reg, DRV_ENC28J60_RegUnion  value, bool autoAck);

// *****************************************************************************
/* System Reset

    Summary:
    Sends a device reset.

    Details:
    This function sends the system reset command to the ENC28J60.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance

    Returns:
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENC28J60_SPI_SystemReset(DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck );

// *****************************************************************************
/* Enable Receive

    Summary:
    Sends an enable RX.

    Details:
    This function sends the Enable RX command to the ENC28J60.

    Preconditions:
    The bus had to have been initialized first.

    Parameters
        pDrvInstance – The driver instance

    Returns
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENC28J60_SPI_EnableRX(DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck);

// *****************************************************************************
/* Disable Receive

    Summary:
    Sends a disable RX.

    Details:
    This function sends the Disable RX command to the ENC28J60.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance

    Returns
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENC28J60_SPI_DisableRX(DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck);


// *****************************************************************************
/* Decrement Packet Counter

    Summary:
    Sends a decrement packet counter.

    Details:
    This function sends the Decrement Packet command to the ENC28J60.  This is
    called by the RX state machine after it has received a packet.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance

    Returns
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENC28J60_SPI_DecrPktCtr(DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck);

// *****************************************************************************
/* Enable Interrupts

    Summary:
    Sends an enable interrupts.

    Details:
    This function sends the Enable Interrupts command to the ENC28J60.

    Preconditions:
    The bus had to have been initialized first.

    Parameters
        pDrvInstance – The driver instance

    Returns
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENC28J60_SPI_EnableInterrupts(DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck  );

// *****************************************************************************
/*  Disable Interrupts

    Summary:
    Sends a disable interrupts.

    Details:
    This function sends the Disable Interrupts command to the ENC28J60.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance

    Returns:
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENC28J60_SPI_DisableInterrupts(DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck);

// *****************************************************************************
/*  Disable Flow Control

    Summary:
    Sends a disable flow control.

    Details
    This function sends the disable flow control command to the ENC28J60.

    Preconditions:
    The bus had to have been initialized first.

    Parameters
        pDrvInstance – The driver instance

    Returns
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENC28J60_SPI_FlowCtrlDisable(DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck );

// *****************************************************************************
/*  Single Flow Control

    Summary:
    Sends a single flow control.

    Details:
    This function sends the single flow control command to the ENC28J60.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance

    Returns
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENC28J60_SPI_FlowCtrlSingle(DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck );

// *****************************************************************************
/*  Multiple Flow Control

    Summary:
    Sends a multiple flow control.

    Details:
    This function sends the multiple flow control command to the ENC28J60.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance

    Returns:
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENC28J60_SPI_FlowCtrlMult(DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck );

// *****************************************************************************
/*  Clear Flow Control

    Summary:
    Sends a clear flow control.

    Details:
    This function sends the clear flow control command to the ENC28J60.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance

    Returns:
        NULL – On Error
        Valid Handle – on success
*/
 uintptr_t DRV_ENC28J60_SPI_FlowCtrClear(DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck );

DRV_ENC28J60_PHY_RES DRV_ENC28J60_SPI_PhyWriteStart(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_PHY_SFR_MAP  reg, uint16_t  regValue);

// *****************************************************************************
/*  PHY Register Write

    Summary:
    Write to a PHY register on the ENC.

    Details:
    This function starts a write to the PHY on the ENC.  It is actually two bus
    operations

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance – The driver instance
        reg – The PHY register to write
        value – the Value to write

    Returns:
        NULL – On Error
        Valid Handle – on success
*/
DRV_ENC28J60_PHY_RES DRV_ENC28J60_SPI_PhyWrite(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_PHY_SFR_MAP  reg);


/* Write Packet

    Summary:
    Writes a MAC packet to the ENC hardware
    
    Details:
    This function sends MAC packet to the ENC. 

    Preconditions:
    The bus had to have been initialized first.
    
    Parameters:
        pDrvInstance - The driver instance
        pTxPkt       - packet to be transmitted
        pHandle      - handle to use if autoAck == false
        autoAck      - no query of the operation completion is needed
  
    Returns:
        0 on error
        Valid handle on success

    Remarks:
        The SPI driver does not currently support write operations with multiple linked segments 
        (this feature is needed because each data segment has to start with an WBM command to the ENC).
        Thus the zero copy functionality of the TCP/IP stack cannot be used for this driver.
        A copy is done to an internal buffer that is transmitted over the SPI wire.
*/
TCPIP_MAC_RES DRV_ENC28J60_SPI_WritePacket(DRV_ENC28J60_DriverInfo *  pDrvInstance, TCPIP_MAC_PACKET* pTxPkt, uintptr_t* pHandle, bool autoAck);

// *****************************************************************************
/* Read Data Start

    Summary:
    Read data from the ENC hardware.

    Details:
    This function reads data from the ENC. The parameters to this function are a
    little different than expected.  The TCP/IP packet segment is allocated with
    some space before the actual pointer in the packet.  For the PIC32 Internal
    MAC this is two bytes.  This data is used by the MAC for its own purposes.
    n the case of the ENC28J60 only 1 byte is needed for SPI.  For PSP a
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
uintptr_t DRV_ENC28J60_SPI_ReadDataStart(DRV_ENC28J60_DriverInfo*  pDrvInstance, uint8_t* buffer, uint16_t dataSize, bool autoAck);


#endif
