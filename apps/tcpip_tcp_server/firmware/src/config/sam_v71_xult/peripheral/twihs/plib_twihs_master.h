/*******************************************************************************
  TWIHS Peripheral Library Interface Header File

  Company
    Microchip Technology Inc.

  File Name
    twihs_master.h

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

#ifndef PLIB_TWIHS_MASTER_H   
#define PLIB_TWIHS_MASTER_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

/*  This section lists the other files that are included in this file.
*/
#include <stdbool.h>
#include <stddef.h>

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
/* TWIHS Transfer Status

   Summary:
    TWIHS Transfer Status data type.

   Description:
    This data type defines the TWIHS Transfer Status.

   Remarks:
    None.
*/

typedef enum
{	
	/* No Error */
    TWIHS_ERROR_NONE,
	
	/* Slave returned Nack */
    TWIHS_ERROR_NACK,
	
} TWIHS_ERROR;

// *****************************************************************************
/* TWIHS State.

   Summary:
    TWIHS PLib Task State.

   Description:
    This data type defines the TWIHS PLib Task State.

   Remarks:
    None.
  
*/

typedef enum {

    /* TWIHS PLib Task Error State */
	TWIHS_STATE_ERROR = -1,
	
	/* TWIHS PLib Task Idle State */
	TWIHS_STATE_IDLE,
	
	/* TWIHS PLib Task Address Send State */
	TWIHS_STATE_ADDR_SEND,
	
	/* TWIHS PLib Task Read Transfer State */
	TWIHS_STATE_TRANSFER_READ,
	
	/* TWIHS PLib Task Write Transfer State */
	TWIHS_STATE_TRANSFER_WRITE,
	
	/* TWIHS PLib Task Transfer Complete State */
	TWIHS_STATE_WAIT_FOR_TXCOMP,
	
	/* TWIHS PLib Task Transfer Done State */
	TWIHS_STATE_TRANSFER_DONE,

} TWIHS_STATE;

// *****************************************************************************
/* TWIHS Callback

   Summary:
    TWIHS Callback Function Pointer.

   Description:
    This data type defines the TWIHS Callback Function Pointer.

   Remarks:
    None.
*/

typedef void (*TWIHS_CALLBACK) 
(   
    /* Transfer context */
    uintptr_t contextHandle 
);

// *****************************************************************************
/* TWIHS PLib Instance Object

   Summary:
    TWIHS PLib Object structure.

   Description:
    This data structure defines the TWIHS PLib Instance Object.

   Remarks:
    None.
*/

typedef struct
{
    uint16_t address;
    uint8_t *writeBuffer;
    uint8_t	*readBuffer;
    size_t  writeSize;
	size_t  readSize;
    size_t  writeCount;
	size_t  readCount;
	
	/* State */
	TWIHS_STATE state;
	
	/* Transfer status */
    TWIHS_ERROR error;
	
	/* Transfer Event Callback */
	TWIHS_CALLBACK callback;

    /* Transfer context */
    uintptr_t context;	

} TWIHS_OBJ;

// *****************************************************************************
/* Transaction Request Block

   Summary:
    Transaction Request Block Structure.

   Description:
    This data structure defines the Transaction Request Block.

   Remarks:
    None.
*/

typedef struct 
{
    /* TWIHS Clock Speed */
    uint32_t clkSpeed;

} TWIHS_TRANSFER_SETUP;

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END

#endif // PLIB_TWIHS_MASTER_H

/*******************************************************************************
 End of File
*/
