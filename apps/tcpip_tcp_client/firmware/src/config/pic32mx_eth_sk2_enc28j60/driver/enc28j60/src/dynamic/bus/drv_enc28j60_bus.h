/*******************************************************************************
  ENC28J60 Driver Bus Interface
  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_enc28j60_bus.h
  Summary:
    
  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2015-2020 Microchip Technology Inc. and its subsidiaries.

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
#if !defined(_DRV_ENC28J60_BUS_H_)
#define _DRV_ENC28J60_BUS_H_


#include "../drv_enc28j60_ds_defs.h"
#include "driver/driver_common.h" 
#include "system_config.h"
   


struct _DRV_ENC28J60_DriverInfo;

typedef enum
{
    DRV_ENC28J60_BR_SUCCESS,
    DRV_ENC28J60_BR_PENDING,

    DRV_ENC28J60_BR_ERROR       = -1,
    DRV_ENC28J60_BR_INVALID     = -2,

}DRV_ENC28J60_BUS_RESULT;

typedef enum
{
    DRV_ENC28J60_PTR_ERDPTL,
    DRV_ENC28J60_PTR_ERDPTH,
    DRV_ENC28J60_PTR_EWRPTL,
    DRV_ENC28J60_PTR_EWRPTH,
    DRV_ENC28J60_PTR_ETXSTL,
    DRV_ENC28J60_PTR_ETXSTH,
    DRV_ENC28J60_PTR_ETXNDL,
    DRV_ENC28J60_PTR_ETXNDH,
    DRV_ENC28J60_PTR_ERXSTL,
    DRV_ENC28J60_PTR_ERXSTH,    
	DRV_ENC28J60_PTR_ERXNDL,
    DRV_ENC28J60_PTR_ERXNDH,
    DRV_ENC28J60_PTR_ERXRDPTL,
    DRV_ENC28J60_PTR_ERXRDPTH,
    DRV_ENC28J60_PTR_ERXWRPTL,
    DRV_ENC28J60_PTR_ERXWRPTH,
	DRV_ENC28J60_PTR_EDMASTL,
    DRV_ENC28J60_PTR_EDMASTH,
    DRV_ENC28J60_PTR_EDMANDL,
    DRV_ENC28J60_PTR_EDMANDH,
    DRV_ENC28J60_PTR_EDMADSTL,
    DRV_ENC28J60_PTR_EDMADSTH,
}DRV_ENC28J60_POINTER;


// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - Bus Level
// *****************************************************************************
// *****************************************************************************

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
typedef int32_t (*DRV_ENC28J60_OpenInterface)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance );

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
typedef void (*DRV_ENC28J60_CloseInterface)( struct _DRV_ENC28J60_DriverInfo *  pDrvInstance);

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
        ack - if the operation handle is to be flagged as completed and removed
              if false, this operation will be kept valid and status/result could be polled again
              The removal will occur only when the operation has completed.
              A call with ack set is needed when the user is done with this operation. 

    Returns
        DRV_ENC28J60_BR_SUCCESS – if the operation was successful
        DRV_ENC28J60_BR_PENDING – if the operation is still pending
        DRV_ENC28J60_BR_ERROR – if there was an error in the operation
*/
typedef DRV_ENC28J60_BUS_RESULT (*DRV_ENC28J60_OperationResult)( struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, uintptr_t  handle, bool ack );


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
        autoAck - if the SPI operation, once completed, can acknowledge itself
                  i.e. investigation of the operation result is not needed
                  If autoAck == false, then the DRV_ENC28J60_OperationResult is needed
                  and it will acknowledge the operation completion 

    Returns:
        NULL – On Error
        Valid Handle – on success

    Remarks:
        The autoAck behavior is valid for all SPI functions that take this parameter.

*/
typedef uintptr_t (*DRV_ENC28J60_SfrWrite)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  reg, DRV_ENC28J60_RegUnion  value, bool autoAck);

// function for writing 2 registers - low and high - at a time
// the operation completion checks that both operations are complete
typedef uintptr_t (*DRV_ENC28J60_SfrWrite16)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  sfrLow, uint16_t regValue, bool autoAck);

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
typedef uintptr_t (*DRV_ENC28J60_SfrReadStart)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  reg, bool autoAck);

// read 16 bits at a time function
typedef uintptr_t (*DRV_ENC28J60_SfrRead16Start)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP sfrLow, bool autoAck);

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
        ack - if the operation handle is to be flagged as completed and removed
              if false, this operation will be kept valid and status/result could be polled again
              A call with ack set is needed when the user is done with this operation. 

    Returns
        DRV_ENC28J60_BR_SUCCESS – if the operation was successful
        DRV_ENC28J60_BR_PENDING – if the operation is still pending
        DRV_ENC28J60_BR_ERROR – if there was an error in the operation
*/
typedef DRV_ENC28J60_BUS_RESULT (*DRV_ENC28J60_SfrReadResult)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, uintptr_t  handle, DRV_ENC28J60_RegUnion *  value, bool ack);

// read the result of a 16 bit initiated operation
typedef DRV_ENC28J60_BUS_RESULT (*DRV_ENC28J60_SfrRead16Result)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, uintptr_t  handle, uint16_t* value, bool ack);

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
typedef uintptr_t (*DRV_ENC28J60_SfrBitSet)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  reg, DRV_ENC28J60_RegUnion  value, bool autoAck);

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
typedef uintptr_t (*DRV_ENC28J60_SfrBitClear)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  reg, DRV_ENC28J60_RegUnion  value, bool autoAck);

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
typedef uintptr_t (*DRV_ENC28J60_SystemReset)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck );

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
typedef uintptr_t (*DRV_ENC28J60_EnableRX)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck );

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
typedef uintptr_t (*DRV_ENC28J60_DisableRX)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck );

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
typedef uintptr_t (*DRV_ENC28J60_DecrPktCtr)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck );

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
typedef uintptr_t (*DRV_ENC28J60_EnableInterrupts)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck );

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
typedef uintptr_t (*DRV_ENC28J60_DisableInterrupts)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck );

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
typedef uintptr_t (*DRV_ENC28J60_FlowCtrlDisable)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck );

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
typedef uintptr_t (*DRV_ENC28J60_FlowCtrlSingle)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck );

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
typedef uintptr_t (*DRV_ENC28J60_FlowCtrlMult)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck );

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
typedef uintptr_t (*DRV_ENC28J60_FlowCtrClear)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck );



// PHY operation result
typedef enum
{
    DRV_ENC28J60_PHY_RES_OK,        // operation successful
    DRV_ENC28J60_PHY_RES_PENDING,   // operation in progress

    // errors
    DRV_ENC28J60_PHY_RES_BUSY           = -1,   // another operation in progress; wait and retry
    DRV_ENC28J60_PHY_RES_RESOURCE_ERROR = -2,   // couldn't find resources for the operation;
    DRV_ENC28J60_PHY_RES_OP_ERROR       = -3,   // operation error;
    DRV_ENC28J60_PHY_RES_TXFER_ERROR    = -4,   // transfer error;
}DRV_ENC28J60_PHY_RES;



// *****************************************************************************
/*  PHY Register Start Write

    Summary:
    Starts the write to a PHY register on the ENC.

    Details:
    This function starts a write to the PHY on the ENC.
    A write consists of multiple bus operations and states

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance    - The driver instance
        reg             - The PHY register to write
        value           - the value to be written to the register

    Returns:
        DRV_ENC28J60_PHY_RES_PENDING if the operation started properly
        an error code otherwise

    Remarks:
        After this call the user MUST call the DRV_ENC28J60_PhyWrite operation until it returns
        OK or some error.
        This advances the write state machine!
*/
typedef DRV_ENC28J60_PHY_RES (*DRV_ENC28J60_PhyWriteStart)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_PHY_SFR_MAP  reg, uint16_t regValue);

// *****************************************************************************
/*  PHY Register Write

    Summary:
    Completes a previous write operation started by DRV_ENC28J60_PhyWriteStart;

    Details:
    This function advances the write state machine to complete the write to the PHY on the ENC.
    A write consists of multiple bus operations and states

    Preconditions:
    The bus had to have been initialized first.
    The DRV_ENC28J60_PhyWriteStart should have been called

    Parameters:
        pDrvInstance    - The driver instance
        reg             - The PHY register currently in the write operation

    Returns:
        DRV_ENC28J60_PHY_RES_PENDING if the operation is progressing properly
        DRV_ENC28J60_PHY_RES_OK if the operation completed successfully and another operation could be started
        an error code otherwise

    Remarks:
        This call advances the write state machine!
*/
typedef DRV_ENC28J60_PHY_RES (*DRV_ENC28J60_PhyWrite)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_PHY_SFR_MAP reg);

// *****************************************************************************
/*  PHY Register Start Read

    Summary:
    Starts the read to a PHY register from the ENC.

    Details:
    This function starts a read from the PHY on the ENC.
    A read consists of multiple bus operations and states

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance    - The driver instance
        reg             - The PHY register to read

    Returns:
        DRV_ENC28J60_PHY_RES_PENDING if the operation started properly
        an error code otherwise

    Remarks:
        After this call the user MUST call the DRV_ENC28J60_PhyRead operation until it returns
        OK or some error.
        This advances the read state machine!
*/
typedef DRV_ENC28J60_PHY_RES (*DRV_ENC28J60_PhyReadStart)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_PHY_SFR_MAP  reg);

// *****************************************************************************
/*  PHY Register Read

    Summary:
    Completes a previous read operation started by DRV_ENC28J60_PhyReadStart;

    Details:
    This function advances the read state machine to complete the read to the PHY on the ENC.
    A read consists of multiple bus operations and states

    Preconditions:
    The bus had to have been initialized first.
    The DRV_ENC28J60_PhyReadStart should have been called

    Parameters:
        pDrvInstance    - The driver instance
        reg             - The PHY register currently in the write operation
        phyRegValue     - adress to store the PHY register value, if transaction succeeded

    Returns:
        DRV_ENC28J60_PHY_RES_PENDING if the operation is progressing properly
        DRV_ENC28J60_PHY_RES_OK if the operation completed successfully and another operation could be started
        an error code otherwise

    Remarks:
        This call advances the phyRegValue state machine!
*/
typedef DRV_ENC28J60_PHY_RES (*DRV_ENC28J60_PhyRead)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_PHY_SFR_MAP reg, uint16_t* phyRegValue);


// *****************************************************************************
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
        pHandle      - address to store the transaction handle, if successful
  
    Returns:
        A TCPIP_MAC_RES result:
        TCPIP_MAC_RES_PENDING if retry needed
        TCPIP_MAC_RES_OK if operation went through
        TCPIP_MAC_RES_PACKET_ERR - rejected, cannot transmit such packet
        
        

    Remarks:
        The SPI driver does not currently support write operations with multiple linked segments 
        (this feature is needed because each data segment has to start with an WBM command to the ENC).
        Thus the zero copy functionality of the TCP/IP stack cannot be used for this driver when
        a packet consists of multiple segments.
        In this case a copy is done to an internal buffer that is transmitted over the SPI wire.
*/
typedef TCPIP_MAC_RES (*DRV_ENC28J60_WritePacket)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, TCPIP_MAC_PACKET* pTxPkt, uintptr_t* pHandle , bool autoAck);

// *****************************************************************************
/* Read Data Start

    Summary:
    Read data from the ENC hardware.  

    Details:
    This function reads data from the ENC. The parameters to this function are a 
    little different than expected.  The TCP/IP packet segment is allocated with 
    some space before the actual pointer in the packet.  For the PIC32 Internal 
    MAC this is two bytes.  This data is used by the MAC for its own purposes.  
    n the case of the ENC28J60 only 1 byte is needed for SPI. The buffer that 
	is passed in is the start of the data segment, and this function assumes 
	there is some allocated space before the pointer.  The datasize is the size
	of the data portion, not the total size of the buffer.

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
typedef uintptr_t (*DRV_ENC28J60_ReadDataStart)(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, uint8_t *  buffer, uint16_t  dataSize, bool autoAck);

typedef struct _DRV_ENC28J60_BusVTable
{
    DRV_ENC28J60_OpenInterface fpOpenIf;
    DRV_ENC28J60_CloseInterface fpCloseIf;
    DRV_ENC28J60_OperationResult fpOpResult;
    DRV_ENC28J60_SfrWrite fpSfrWr;
    DRV_ENC28J60_SfrWrite16 fpSfrWr16;
    DRV_ENC28J60_SfrReadStart fpSfrRdStart;
    DRV_ENC28J60_SfrReadResult fpSfrRdResult;
    DRV_ENC28J60_SfrRead16Start fpSfrRd16Start;
    DRV_ENC28J60_SfrRead16Result fpSfrRd16Result;
    DRV_ENC28J60_SfrBitSet fpSfrBitSet;
    DRV_ENC28J60_SfrBitClear fpSfrBitClr;
    DRV_ENC28J60_SystemReset fpSysRst;
    DRV_ENC28J60_EnableRX fpRxEnable;
    DRV_ENC28J60_DisableRX fpRxDisable;
    DRV_ENC28J60_DecrPktCtr fpDecPktCnt;
    DRV_ENC28J60_EnableInterrupts fpIntEnable;
    DRV_ENC28J60_DisableInterrupts fpIntDisable;
    DRV_ENC28J60_FlowCtrlDisable fpFlowCtrlDisable;
    DRV_ENC28J60_FlowCtrlSingle fpFlowCtrlSingle;
    DRV_ENC28J60_FlowCtrlMult fpFlowCtrlMult;
    DRV_ENC28J60_FlowCtrClear fpFlowCtrlClr;
    DRV_ENC28J60_PhyWriteStart fpPhyWrStart;
    DRV_ENC28J60_PhyWrite fpPhyWr;
    DRV_ENC28J60_PhyReadStart fpPhyRdStart;
    DRV_ENC28J60_PhyRead fpPhyRd;
    DRV_ENC28J60_WritePacket fpPktWr;
    DRV_ENC28J60_ReadDataStart fpDataRdStart;
}DRV_ENC28J60_BusVTable;




#endif
