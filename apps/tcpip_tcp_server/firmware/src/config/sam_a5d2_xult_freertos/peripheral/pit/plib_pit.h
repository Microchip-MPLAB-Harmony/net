/*******************************************************************************
  Periodic Interval Timer (PIT) 

  Company:
    Microchip Technology Inc.

  File Name:
    plib_pit.h

  Summary:
    Periodic Interval Timer (PIT) PLIB.

  Description:
    This file declares the interface for the Periodic Interval Timer (PIT).
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

#ifndef _PLIB_PIT_H
#define _PLIB_PIT_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdbool.h>
#include <stdint.h>

#include "device.h" // static inline macro

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************

// *****************************************************************************
/* PIT Interrupt Callback Function definition.

  Summary:
    PIT Interrupt Callback Function definition.

  Description:
    Defines the function signature for the PIT interrupt callback.

*/

typedef void (*PIT_CALLBACK)(uintptr_t context);

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
   void PIT_TimerInitialize(void);

  Summary:
    Initialize PIT registers per user config.

  Description:
    Initalize the period and, if configured, enable the counter and interrupt.

  Precondition:
    None.

  Parameters:
    None.
  
  Returns:
    None.
*/
void PIT_TimerInitialize(void);

// *****************************************************************************
/* Function:
    void PIT_TimerRestart(void);

  Summary:
   Restart the PIT counter.

  Description:
    Signal the PIT counter to stop, wait for it to stop, then restart the counter.

  Precondition:
    None.

  Parameters:
    None.
  
  Returns:
    None.
*/
void PIT_TimerRestart(void);

// *****************************************************************************
/* Function:
    void PIT_TimerStart(void);

  Summary:
   Start the PIT counter.

  Description:
    Start the PIT counter.  If interrupts are enabled an interrupt will occur
    every time the PIV value is reached.

  Precondition:
    None.

  Parameters:
    None.
  
  Returns:
    None.
*/
void PIT_TimerStart(void);

// *****************************************************************************
/* Function:
    void PIT_TimerStop(void);

  Summary:
   Stop the PIT counter.

  Description:
    Stop the PIT counter.

  Precondition:
    None.

  Parameters:
    None.
  
  Returns:
    None.
*/
void PIT_TimerStop(void);

// *****************************************************************************
/* Function:
   void PIT_TimerPeriodSet(uint32_t period);

  Summary:
   Set the timer period value.

  Description:
    Set the timer period value by programming the PIV field in the MR register.

  Precondition:
    None.

  Parameters:
   period       - The period (PIV) value of the PIT. 
  
  Returns:
    None.
*/
void PIT_TimerPeriodSet(uint32_t period);

// *****************************************************************************
/* Function:
    uint32_t PIT_TimerPeriodGet(void);

  Summary:
   Get the timer period value.

  Description:
    Return the current PIV value of the PIT.

  Precondition:
    None.

  Parameters:
    None.
  
  Returns:
    None.
*/
uint32_t PIT_TimerPeriodGet(void);

// *****************************************************************************
/* Function:
    uint32_t PIT_TimerCounterGet(void);

  Summary:
   Get the timer counter value.

  Description:
    Return the current counter (CPIV) value of the PIT.

  Precondition:
    None.

  Parameters:
    None.
  
  Returns:
    None.
*/
uint32_t PIT_TimerCounterGet(void);


// *****************************************************************************
/* Function:
    void PIT_TimerCompareSet(void);

  Summary:
   Set the timer comparison value.

  Description:
    Provide a future PIT count value for comparison purposes.  When PIT timer
    counter is greater than or equal to the value an event will be created.

  Precondition:
    None.

  Parameters:
    None.
  
  Returns:
    None.
*/
void PIT_TimerCompareSet( uint16_t compare );


// *****************************************************************************
/* Function:
    uint32_t PIT_TimerFrequencyGet(void);

  Summary:
    Get the timer clock frequency.

  Description:
    Return the clock frequency of the PIT.

  Precondition:
    None.

  Parameters:
    None.
  
  Returns:
    None.
*/
uint32_t PIT_TimerFrequencyGet(void);

// *****************************************************************************
/* Function:
    void PIT_DelayMs(uint32_t ms);

  Summary:
    Delays processing for x milliseconds.

  Description:
    Delays execution by using  the PIT timer to determine when given number of
    milliseconds has expired.  

  Precondition:
    PIT is configured and enabled.  The PIT interrupt is also enabled.

  Parameters:
    ms      - number of milliseconds to delay
  
  Returns:
    None.
*/
void PIT_DelayMs(uint32_t ms);

// *****************************************************************************
/* Function:
    void PIT_TimerCallbackSet(PIT_CALLBACK callback, uintptr_t context);

  Summary:
    Register callback for PIT interrupt.

  Description:
    When the timer interrupt occurs the given callback will called with the
    given context.

  Precondition:
    None.

  Parameters:
    callback    - Callback function
    context     - paramter to callback function
  
  Returns:
    None.
*/
void PIT_TimerCallbackSet(PIT_CALLBACK callback, uintptr_t context);

// *****************************************************************************
/* Function:
    void PIT_ClearInterrupt(void);

  Summary:
    PIT Clear Interrupt.

  Description:
    Clear the PIT interrupt by reading the PIVR register.  Meant
    to be used by external interrupt handlers (e.g. FreeRTOS tick hanlder).

  Precondition:
    None.

  Parameters:
    None.
  
  Returns:
    None.
*/
__STATIC_INLINE void PIT_ClearInterrupt(void)
{
    (uint32_t)PIT_REGS->PIT_PIVR;
}

#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif

#endif

/*******************************************************************************
 End of File
*/
