/*******************************************************************************
  TWIHS Peripheral Library Interface Header File

  Company
    Microchip Technology Inc.

  File Name
    plib_twihs0.h

  Summary
    TWIHS peripheral library interface.

  Description
    This file defines the interface to the TWIHS peripheral library.  This 
    library provides access to and control of the associated peripheral 
    instance.

  Remarks:
    
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

#ifndef PLIB_TWIHS0_H   
#define PLIB_TWIHS0_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

/*  This section lists the other files that are included in this file.
*/
#include "plib_twihs_master.h"

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

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines
// *****************************************************************************
// *****************************************************************************
/* The following functions make up the methods (set of possible operations) of 
   this interface.
*/

// *****************************************************************************
/* Function:
    void TWIHS0_Initialize(void)

   Summary:
    Initializes given instance of the TWIHS peripheral.
	
   Description:
    This function initializes the given instance of the TWIHS peripheral as
    configured by the user from within the MCC.

   Precondition:
    None.
	
   Parameters:
    None.
	
   Returns:
    None

   Example:
    <code>
    TWIHS1_Initialize();
    </code>

   Remarks:
    Stops the TWIHS if it was already running and reinitializes it.
*/

void TWIHS0_Initialize(void);

// *****************************************************************************
/* Function:
    void TWIHS0_CallbackRegister(TWIHS_CALLBACK callback, uintptr_t contextHandle)
	
   Summary:
    Sets the pointer to the function (and it's context) to be called when the 
    given TWIHS's transfer events occur.

   Description:
    This function sets the pointer to a client function to be called "back" 
    when the given TWIHS's transfer events occur. It also passes a context value 
    (usually a pointer to a context structure) that is passed into the
    function when it is called.

   Precondition:
    TWIHS0_Initialize must have been called for the associated TWIHS instance.

   Parameters:
    callback - A pointer to a function with a calling signature defined 
	by the TWIHS_CALLBACK data type.

    context - A value (usually a pointer) passed (unused) into the function 
	identified by the callback parameter.
  
   Returns:
    None.

   Example:
    <code>
    TWIHS0_CallbackRegister(MyTWIHSCallback, &myData);
    </code>

   Remarks:
    The context parameter is ignored if the pointer passed is NULL.
    To disable the callback function, pass a NULL for the callback parameter.
    See the TWIHS_CALLBACK type definition for additional information.
*/

void TWIHS0_CallbackRegister(TWIHS_CALLBACK callback, uintptr_t contextHandle);

// *****************************************************************************
/* Function:
    bool TWIHS0_IsBusy(void)
	
   Summary:
    Returns the Peripheral busy status.
    
   Description:
    This function returns the peripheral's busy status.

   Precondition:
    TWIHSx_Initialize must have been called for the associated TWIHS instance.

   Parameters:
    None.
	
   Returns:
    true - Busy.
    false - Not busy.
    
   Example:
    <code>
        uint8_t myData [NUM_BYTES] = {'1', '0', ' ', 'B', 'Y', 'T', 'E', 'S', '!', '!',};
	  
        // wait for the current transfer to complete
	    while(TWIHS0_IsBusy( ));
	    
        // perform the next transfer
	    if(!TWIHS0_Write( SLAVE_ADDR, &myData[0], NUM_BYTES ))
	    {
		    // error handling
	    }
    
    </code>

   Remarks:
    None.
*/

bool TWIHS0_IsBusy(void);

// *****************************************************************************
/* Function:
    bool TWIHS0_Read(uint16_t address, uint8_t *pdata, size_t length)
	
   Summary:
    Reads data from the slave.

   Description:
    This function reads the data of size in bytes equal to length into pdata buffer 
	from the slave having given address. Master will generate Stop condition after 
	completion of the read.

   Precondition:
    TWIHS0_Initialize must have been called for the associated TWIHS instance.

   Parameters:
    address - 7-bit / 10-bit slave address.
	pdata   - pointer to destination data buffer
	length  - length of data buffer in number of bytes.
  
   Returns:
    Read request status.
    True - Read request was successful.
    False - Read request has failed.


   Example:
    <code>
	    uint8_t myData [NUM_BYTES];
	  
	    if(!TWIHS0_Read( SLAVE_ADDR, &myData[0], NUM_BYTES ))
	    {
		    // error handling
	    }
    </code>

   Remarks:
    None.
*/

bool TWIHS0_Read(uint16_t address, uint8_t *pdata, size_t length);

// *****************************************************************************
/* Function:
    bool TWIHS0_Write(uint16_t address, uint8_t *pdata, size_t length)
	
   Summary:
    Writes data onto the slave.

   Description:
    This function writes the data from pdata buffer of size in bytes equal to 
	length onto the slave with given address. Master will generate Stop 
	condition after completion of the write.

   Precondition:
    TWIHS0_Initialize must have been called for the associated TWIHS instance.

   Parameters:
    address - 7-bit / 10-bit slave address.
	pdata   - pointer to source data buffer
	length  - length of data buffer in number of bytes.
	
   Returns:
    Request status.
    True - Request was successful.
    False - Request has failed.

   Example:
    <code>
	    uint8_t myData [NUM_BYTES] = {'1', '0', ' ', 'B', 'Y', 'T', 'E', 'S', '!', '!',};
	  
	    if(!TWIHS0_Write( SLAVE_ADDR, &myData[0], NUM_BYTES ))
	    {
		    // error handling
	    }
    </code>

   Remarks:
    None.
*/

bool TWIHS0_Write(uint16_t address, uint8_t *pdata, size_t length);

// *****************************************************************************
/* Function:
    bool TWIHS0_WriteRead(uint16_t address, uint8_t *wdata, size_t wlength, uint8_t *rdata, size_t rlength)
	
   Summary:
    Write and Read data from Slave.

   Description:
    This function writes data from wdata buffer to the slave of given address and
	of size(in bytes) equal to wlength. Master then generates a repeated start and
	do a read operation. Read operation reads data into rdata buffer of size(in
    bytes) equal to rlength from the slave of given address. Master generates a stop
	condition after reading the data.

   Precondition:
    TWIHS0_Initialize must have been called for the associated TWIHS instance.

   Parameters:
    address - 7-bit / 10-bit slave address.
	wdata   - pointer to write data buffer
	wlength - write data length in bytes.
	rdata   - pointer to read data buffer.
	rlength - read data length in bytes.
  
   Returns:
    Request status.
    True - Request was successful.
    False - Request has failed.

   Example:
    <code>
	    uint8_t myTxData [NUM_BYTES] = {'1', '0', ' ', 'B', 'Y', 'T', 'E', 'S', '!', '!'};
		uint8_t myRxData [NUM_BYTES] = {0};
	  
	    if(!TWIHS0_WriteRead( SLAVE_ADDR, &myTxData[0], NUM_BYTES, myRxData, NUM_BYTES ))
	    {
		    // error handling
	    }
    </code>

   Remarks:
*/

bool TWIHS0_WriteRead(uint16_t address, uint8_t *wdata, size_t wlength, uint8_t *rdata, size_t rlength);

// *****************************************************************************
/* Function:
    TWIHS_ERROR TWIHS0_ErrorGet(void)
	
   Summary:
    Returns the error during transfer.

   Description:
    This function returns the error during transfer.

   Precondition:
    TWIHS0_Initialize must have been called for the associated TWIHS instance.

   Parameters:
    None.
	
   Returns:
    Request status.
    True - Request was successful.
    False - Request has failed.
	
   Example:
    <code>
    if(TWIHS_ERROR_NONE == TWIHS0_ErrorGet())
    {
        //TWIHS transfer is completed, go to next state.
    }
    </code>

   Remarks:
    None.
*/

TWIHS_ERROR TWIHS0_ErrorGet(void);

// *****************************************************************************
/* Function:
    bool TWIHS0_TransferSetup(TWIHS_TRANSFER_SETUP* setup, uint32_t srcClkFreq)

   Summary:
    Dynamic setup of TWIHS Peripheral.

   Precondition:
    TWIHS0_Initialize must have been called for the associated TWIHS instance.
	The transfer status should not be busy.
	
   Parameters:
    setup - Pointer to the structure containing the transfer setup.
    srcClkFreq - TWIHS Peripheral Clock Source Frequency.
	
   Returns:
    true - Transfer setup was updated Successfully.
    false - Failure while updating transfer setup.
    
   Example:
    <code>    
    
    TWIHS_TRANSFER_SETUP setup;
    
    setup.clkSpeed = 400000;
    
    // Make sure that the I2C is not busy before changing the I2C clock frequency
    if (TWIHS0_IsBusy() == false)
    {
        if (TWIHS0_TransferSetup( &setup, 0 ) == true)
        {
            // Transfer Setup updated successfully
        }
    }    
    </code>

   Remarks:
    srcClkFreq overrides any change in the peripheral clock frequency. 
    If configured to zero PLib takes the peripheral clock frequency from MHC.
*/

bool TWIHS0_TransferSetup(TWIHS_TRANSFER_SETUP* setup, uint32_t srcClkFreq );

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END

#endif //PLIB_TWIHS0_H

/*******************************************************************************
 End of File
*/
