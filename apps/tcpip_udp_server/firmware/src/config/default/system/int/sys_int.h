/*******************************************************************************
  Interrupt System Service Library Interface Header File

  Company
    Microchip Technology Inc.

  File Name
    sys_int.h

  Summary
    Interrupt system service library interface.

  Description
    This file defines the interface to the interrupt system service library.
    This library provides access to and control of the interrupt controller.

  Remarks:
    Interrupt controller initialize will be done from within the MCC.

*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
// DOM-IGNORE-END

#ifndef SYS_INT_H    // Guards against multiple inclusion
#define SYS_INT_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

/*  This section lists the other files that are included in this file.
*/

#include <stdbool.h>
#include "system/system.h"
#include "device.h"

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
/* Interrupt Sources

   Summary:
    Defines the data type for all the interrupt sources associated with the
       interrupt controller of the device.

   Description:
    This data type can be used with interface functions to enable, disable,
       set, clear and to get status of any particular interrupt source.

   Remarks:
    This data type is defined using the CMSIS data type that defines the
    interrupt sources set available.
*/

typedef IRQn_Type INT_SOURCE;


// *****************************************************************************
/* Interrupt Controller Status

   Summary:
    Defines the data type for the Interrupt controller status.

   Description:
    This data type will be used to get the interrupt controller status while
    disabling the interrupt controller and restoring the interrupt controller
    state later.

   Remarks:
    None.
*/

typedef uint32_t INT_CONTROLLER_STATUS;


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
    void SYS_INT_Enable( void )

   Summary:
    Enables the interrupt controller.

   Description:
    This function enables the interrupt controller of the device if it is not
    active and enables the global interrupt, allowing enabled sources to
    interrupt the CPU.

   Precondition:
    Interrupt controller initialize must have been called.

   Parameters:
    None.

   Returns:
    None.

  Example:
    <code>
    SYS_INT_Enable();
    </code>

  Remarks:
    None.
*/

void SYS_INT_Enable( void );


// *****************************************************************************
/* Function:
    INT_CONTROLLER_STATUS SYS_INT_Disable( void )

   Summary:
    Disables the interrupt controller.

   Description:
    This function disables the interrupt controller of the device, masking
       the global interrupt.
       This will effectively suspend all the interrupt sources that are
       enabled within the interrupt controller.

   Precondition:
    SYS_INT_Initialize must have been called.

   Parameters:
    None.

   Returns:
    INT_CONTROLLER_STATUS - Interrupt controller status prior to disabling
       the controller, preventing any interrupts from occuring.

  Example:
    <code>
       INT_CONTROLLER_STATUS status = 0;

       status = SYS_INT_Disable();
    </code>

  Remarks:
    Returned status can be passed to SYS_INT_Restore to restore the previous
       global interrupt status (whether it was enabled or disabled).
*/

INT_CONTROLLER_STATUS SYS_INT_Disable( void );


// *****************************************************************************
/* Function:
    bool SYS_INT_IsEnabled( void )

   Summary:
    Returns the interrupt controller's global enable/disable status.

   Description:
    This function returns the interrupt controller's global enable/disable status.
       This may be helpful to check the status while disabling interrupt controller
       to avoid spurious interrupts to the system in certain scenarios.

   Precondition:
    SYS_INT_Initialize must have been called.

   Parameters:
    None.

   Returns:
    true - Interrupt controller is enabled.
    false - Interrupt controller is disabled.

  Example:
    <code>
       INT_CONTROLLER_STATUS status = 0;

       if(true == SYS_INT_IsEnabled())
       {
              status = SYS_INT_Disable();
       }
       else
       {
              //Interrupt controller is already disabled
       }
    </code>

  Remarks:
    None.
*/

bool SYS_INT_IsEnabled( void );


// *****************************************************************************
/* Function:
    void SYS_INT_Restore( INT_CONTROLLER_STATUS state )

   Summary:
    Restores the interrupt controller state.

   Description:
    This function restores the interrupt controller of the device to the
       previously obtained state.

   Precondition:
    SYS_INT_Initialize must have been called.
       SYS_INT_Disable must have been called and INT_CONTROLLER_STATUS is available.

   Parameters:
    INT_CONTROLLER_STATUS - Interrupt controller status prior to disabling
       the controller, to prevent any interrupts.

   Returns:
    None.

  Example:
    <code>
       // intStatus is the return value of SYS_INT_Disable
       SYS_INT_Restore( intStatus );
    </code>

  Remarks:
    None.
*/

void SYS_INT_Restore( INT_CONTROLLER_STATUS state );


// *****************************************************************************
/* Function:
    void SYS_INT_SourceEnable( INT_SOURCE source )

   Summary:
    Enables the specified interrupt source.

   Description:
    This function enables the specified interrupt source/line at the interrupt
       controller level.

   Precondition:
    SYS_INT_Initialize must have been called.

   Parameters:
    source - Interrupt source/line available at interrupt controller.

   Returns:
    None.

  Example:
    <code>
    SYS_INT_SourceEnable(USART0_IRQn);
    </code>

  Remarks:
    The corresponding module level interrupts must be enabled
       to trigger the specified interrupt source/line. For example, USART0
       interrupt enable at interrupt controller level is not triggered unless
       USART0 module level interrupt for TXRDY or RXRDY or Error interrupts are not
       enabled.
*/

void SYS_INT_SourceEnable( INT_SOURCE source );


// *****************************************************************************
/* Function:
    bool SYS_INT_SourceDisable( INT_SOURCE source )

   Summary:
    Disables the specified interrupt source.

   Description:
    This function returns the current interrupt enable/disable status and
       disables the specified interrupt source/line at the interrupt controller
       level.

   Precondition:
    SYS_INT_Initialize must have been called.

   Parameters:
    source - Interrupt source/line available at interrupt controller.

   Returns:
    true - Interrupt line was enabled.
       false - Interrupt line was disabled.

  Example:
    <code>
       bool usart0Int = false;
    usart0Int = SYS_INT_SourceDisable(USART0_IRQn);
    </code>

  Remarks:
    If the corresponding module level interrupts are enabled and triggered,
       the triggers will be ignored at interrupt controller. For example, USART0
       doesn't cause interrupt unless both the interrupt controller source/line and
       USART0 module level interrupt for TX or RX or Error are enabled.
*/

bool SYS_INT_SourceDisable( INT_SOURCE source );


// *****************************************************************************
/* Function:
    bool SYS_INT_SourceIsEnabled( INT_SOURCE source )

   Summary:
    Gets the enable/disable status of the specified interrupt source.

   Description:
    This function returns the enable/disable status of the specified interrupt
       source/line at the interrupt controller level.

   Precondition:
    SYS_INT_Initialize must have been called.

   Parameters:
    source - Interrupt source/line available at interrupt controller.

   Returns:
    true - Interrupt line is enabled.
       false - Interrupt line is disabled.

  Example:
    <code>
       bool usart0Int = false;
       usart0Int = SYS_INT_SourceIsEnabled(USART0_IRQn);
    </code>

  Remarks:
    Unlike SYS_INT_Disable, this function just returns the status and doesn't
       disable the interrupt line.
*/

bool SYS_INT_SourceIsEnabled( INT_SOURCE source );


// *****************************************************************************
/* Function:
    bool SYS_INT_SourceStatusGet( INT_SOURCE source )

   Summary:
    Gets the pending status of the specified interrupt source.

   Description:
    This function returns the pending status of the specified interrupt source
       at the interrupt controller level.

   Precondition:
    SYS_INT_Initialize must have been called.

   Parameters:
    source - Interrupt source/line available at interrupt controller.

   Returns:
    true - Interrupt status is pending.
       false - Interrupt status is not pending.

  Example:
    <code>
       bool usart0IntStatus = SYS_INT_SourceStatusGet(USART0_IRQn);
    </code>

  Remarks:
    Interrupt pending status may get cleared automatically once the
       corresponding interrupt vector executes on some devices.
*/

bool SYS_INT_SourceStatusGet( INT_SOURCE source );


// *****************************************************************************
/* Function:
    void SYS_INT_SourceStatusSet( INT_SOURCE source )

   Summary:
    Sets the pending status of the specified interrupt source manually.

   Description:
    This function manually sets the pending status of the specified interrupt
       source at the interrupt controller level. This triggers interrupt controller
       for a specified source even though the interrupt condition has not met at
       hardware.

   Precondition:
    SYS_INT_Initialize must have been called.

   Parameters:
    source - Interrupt source/line available at interrupt controller.

   Returns:
    None.

  Example:
    <code>
       //Trigger USART0 ISR handler manually
    SYS_INT_SourceStatusSet(USART0_IRQn);
    </code>

  Remarks:
    This feature may not be supported by some devices.
*/

void SYS_INT_SourceStatusSet( INT_SOURCE source );


// *****************************************************************************
/* Function:
    void SYS_INT_SourceStatusClear( INT_SOURCE source )

   Summary:
    Clears the pending status of the specified interrupt source.

   Description:
    This function clears the pending status of the specified interrupt
       source at the interrupt controller level.  It is ignored if the
       interrupt condition has already been cleared in hardware.

   Precondition:
    SYS_INT_Initialize must have been called.

   Parameters:
    source - Interrupt source/line available at interrupt controller.

   Returns:
    None.

  Example:
    <code>
       //Clear a pending interrupt.
       SYS_INT_SourceStatusClear(USART0_IRQn);
    </code>

  Remarks:
    None.
*/

void SYS_INT_SourceStatusClear( INT_SOURCE source );


#include "sys_int_mapping.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END

#endif //SYS_INT_H
