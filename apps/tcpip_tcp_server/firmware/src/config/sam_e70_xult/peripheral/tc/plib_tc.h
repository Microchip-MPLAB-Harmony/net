/*******************************************************************************
  TC Peripheral Library Interface Header File

  Company
    Microchip Technology Inc.

  File Name
    plib_tc.h

  Summary
    TC peripheral library interface.

  Description
    This file defines the interface to the TC peripheral library.  This
    library provides access to and control of the associated peripheral
    instance.

  Remarks:
    This header is for documentation only.  The actual tc<x> headers will be
    generated as required by the MCC (where <x> is the peripheral instance
    number).

    Every interface symbol has a lower-case 'x' in it following the "TC"
    abbreviation.  This 'x' will be replaced by the peripheral instance number
    in the generated headers.  These are the actual functions that should be
    used.

    This interface supports the following different modes of the TC peripheral.
        * Timer Mode
        * Capture Mode
        * Compare Mode
        * Quadrature Mode
    Only one of these modes (and only the associated interface functions) will
    be active (and thus will be generated at a time.)
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

#ifndef PLIB_TCx_H    // Guards against multiple inclusion
#define PLIB_TCx_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

/*  This section lists the other files that are included in this file.
*/
#include <stddef.h>
#include <stdbool.h>

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
/*  The following data type definitions are used by the functions in this
    interface and should be considered part it.
*/

/* Interrupt source mask for compare mode

   Summary:
    Identifies channel interrupt source mask

   Description:
    This enumeration identifies TC compare mode interrupt source mask.

   Remarks:
    None.
*/
typedef enum
{
    TC_COMPARE_NONE = 0U,
    TC_COMPARE_A = TC_SR_CPAS_Msk,
    TC_COMPARE_B = TC_SR_CPBS_Msk,
    TC_COMPARE_C = TC_SR_CPCS_Msk,
    TC_COMPARE_STATUS_MSK = TC_SR_CPAS_Msk | TC_SR_CPBS_Msk | TC_SR_CPCS_Msk
}TC_COMPARE_STATUS;

// *****************************************************************************

/* Interrupt source mask for capture mode

   Summary:
    Identifies channel interrupt source mask

   Description:
    This enumeration identifies TC capture mode interrupt source mask.

   Remarks:
    None.
*/
typedef enum
{
    TC_CAPTURE_NONE = 0U,
    TC_CAPTURE_COUNTER_OVERFLOW = TC_SR_COVFS_Msk,
    TC_CAPTURE_LOAD_OVERRUN = TC_SR_LOVRS_Msk,
    TC_CAPTURE_A_LOAD = TC_SR_LDRAS_Msk,
    TC_CAPTURE_B_LOAD = TC_SR_LDRBS_Msk,
    TC_CAPTURE_STATUS_MSK = TC_SR_COVFS_Msk | TC_SR_LOVRS_Msk | TC_SR_LDRAS_Msk | TC_SR_LDRBS_Msk
}TC_CAPTURE_STATUS;

// *****************************************************************************

/* Interrupt source mask for quadrature mode

   Summary:
    Identifies channel interrupt source mask

   Description:
    This enumeration identifies TC quadrature mode interrupt source mask.

   Remarks:
    None.
*/
typedef enum
{
    TC_QUADRATURE_NONE = 0U,
    TC_QUADRATURE_INDEX = TC_QISR_IDX_Msk,
    TC_QUADRATURE_DIR_CHANGE = TC_QISR_DIRCHG_Msk,
    TC_QUADRATURE_ERROR = TC_QISR_QERR_Msk,
    TC_QUADRATURE_STATUS_MSK = TC_QISR_IDX_Msk | TC_QISR_DIRCHG_Msk | TC_QISR_QERR_Msk
}TC_QUADRATURE_STATUS;

// *****************************************************************************

/* Callback Function Pointer

   Summary:
    Defines the function pointer data type and function signature for the tc
    channel callback function.

   Description:
    This data type defines the function pointer and function signature for the
    tc channel callback function.  The library will call back the client's
    function with this signature from the interrupt routine.

   Function:
    void (*TC_CALLBACK) ( uintptr_t context )

   Precondition:
    TCx_Initialize must have been called for the given TC channel
    instance and TCx_CallbackRegister must have been called to register the
    function to be called.

   Parameters:
    context  - Allows the caller to provide a context value (usually a pointer
               to the callers context for multi-instance clients).

   Returns:
    None.

   Example:

    <code>
    void TC_CallbackFn ( uintptr_t context );

    TC0_CH1_TimerCallbackRegister(TC_CallbackFn, NULL);
    </code>

    Remarks:
            Interrupt is different in each mode of TC peripheral.
            e.g. In timer mode and compare mode, period interrupt will be
            enabled and registered function will be called back. In Capture
            mode, registered function will be called from load event interrupt
            routine.
*/

typedef void (*TC_CALLBACK) ( uintptr_t context);
// *****************************************************************************

/* Callback structure

   Summary:
    Callback structure

   Description:
    This stores the callback function pointer and context

   Remarks:
    None.
*/
typedef struct
{
    TC_CALLBACK callback_fn;
    uintptr_t context;
}TC_CALLBACK_OBJECT;

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
    void TCx_CHy_TimerInitialize ( void );

  Summary:
    Initializes given instance of TC channel

  Description:
    This function initializes given instance of TC channel in timer mode with
    the options configured in MCC GUI.

  Precondition:
    MCC GUI should be configured with the desired values.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    TC1_CH0_TimerInitialize();
    </code>

  Remarks:
    This function must be called before any other TC function is
    called.  This function is normally only be called once during system
    initialization.
*/

void TCx_CHy_TimerInitialize ( void );


// *****************************************************************************
/* Function:
    void TCx_CHy_TimerStart ( void );

  Summary:
    Starts the given TC channel counter

  Description:
    This function enables the clock and starts the counter of associated channel.

  Precondition:
    TCx_CHy_TimerInitialize() function must have been called first for the given
    channel.

  Parameters:
    None

  Returns:
    None.

  Example:
    <code>
    TC0_CH1_TimerInitialize();
    TC0_CH1_TimerStart();
    </code>

  Remarks:
    None
*/

void TCx_CHy_TimerStart ( void );


// *****************************************************************************
/* Function:
    void TCx_CHy_TimerStop ( void );

  Summary:
    Stops the given TC channel counter

  Description:
    This function stops the clock and thus counter of associated channel.

  Precondition:
    TCx_CHy_TimerInitialize() function must have been called first for the given
    channel.

  Parameters:
    None

  Returns:
    None.

  Example:
    <code>
    TC1_CH2_TimerInitialize();
    TC1_CH2_TimerStart();
    TC1_CH2_TimerStop();
    </code>

  Remarks:
    None
*/

void TCx_CHy_TimerStop ( void );


// *****************************************************************************
/* Function:
    void TCx_CHy_TimerPeriodSet ( uint16_t period );

  Summary:
    Sets the period value of a given timer channel.

  Description:
    This function writes the period value.  When timer counter matches period
    value, counter is reset and interrupt can be generated.

  Precondition:
    TCx_CHy_TimerInitialize function must have been called first for the given
    channel.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    TC0_CH1_TimerInitialize();
    TC0_CH1_TimerPeriodSet(0x500ul);
    </code>

  Remarks:
    None
*/

void TCx_CHy_TimerPeriodSet ( uint16_t period );


// *****************************************************************************
/* Function:
    uint16_t TCx_CHy_TimerPeriodGet ( void );

  Summary:
    Reads the period value of given timer channel

  Description:
    This function reads the value of period of given timer channel.

  Precondition:
    TCx_CHy_TimerInitialize function must have been called first for the given
    channel.

  Parameters:
    None

  Returns:
    The timer's period value.

  Example:
    <code>
    uint16_t period;

    TC0_CH1_TimerInitialize();
    period = TC0_CH1_TimerPeriodGet();
    </code>

  Remarks:
    None
*/

uint16_t TCx_CHy_TimerPeriodGet ( void );


// *****************************************************************************
/* Function:
    uint16_t TCx_CHy_TimerCounterGet ( void );

  Summary:
    Reads the timer channel counter value

  Description:
    This function reads the timer channel counter value.

  Precondition:
    TCx_CHy_TimerInitialize() function must have been called first for the given
    channel.

  Parameters:
    None

  Returns:
    The timer's counter value

  Example:
    <code>
    uint16_t counter;

    TC0_CH2_TimerInitialize();
    TC0_CH2_TimerStart();
    counter = TC0_CH2_TimerCounterGet();
    </code>

  Remarks:
    None
*/

uint16_t TCx_CHy_TimerCounterGet ( void );


// *****************************************************************************
/* Function:
    uint32_t TCx_CHy_TimerFrequencyGet ( void );

  Summary:
    Provides the given timer's counter-increment frequency.

  Description:
    This function provides the frequency at which the given counter
    increments. It can be used to convert differences between counter values
    to real time or real-time intervals to timer period values.

  Precondition:
    TCx_CHy_TimerInitialize() function must have been called first for the given
    channel.

  Parameters:
    None

  Returns:
      The frequency (in Hz) at which the timer's counter increments.

  Example:
    <code>
    uint32_t frequency;

    TC0_CH1_TimerInitialize();
    frequency = TC0_CH1_TimerFrequencyGet();
    </code>

  Remarks:
    None
*/

uint32_t TCx_CHy_TimerFrequencyGet ( void );

// *****************************************************************************
/* Function:
    bool TCx_CHy_TimerPeriodHasExpired ( void );

  Summary:
    Checks whether timer period is elapsed

  Description:
    This function checks the status of the timer period interrupt.

  Precondition:
    TCx_CHy_TimerInitialize() function must have been called first for the given
    channel.

  Parameters:
    None

  Returns:
    true - timer period has expired
    false - timer period is not expired

  Example:
    <code>
    bool status;

    TC0_CH1_TimerInitialize();
    status = TC0_CH1_TimerPeriodHasExpired();
    </code>

  Remarks:
    None
*/
bool TCx_CHy_TimerPeriodHasExpired(void);


// *****************************************************************************
/* Function:
    void TCx_CHy_TimerCallbackRegister ( TC_CALLBACK callback, uintptr_t context );

  Summary:
    Registers the function to be called from interrupt

  Description
    This function registers the callback function to be called from interrupt.

  Precondition:
    TCx_CHy_TimerInitialize() must have been called first.

  Parameters:
      callback - callback function pointer

      context  - Allows the caller to provide a context value (usually a pointer
                 to the callers context for multi-instance clients).

  Returns:
    None.

  Example:
    <code>
    void TC_CallbackFn ( uintptr_t context );

    TC0_CH1_TimerInitialize();
    TC0_CH1_TimerCallbackRegistert(TC_CallbackFn, NULL);
    </code>

  Remarks:
    Context value can be set to NULL if not required.
    To disable callback function, pass NULL for the callback parameter.
*/

void TCx_CHy_TimerCallbackRegister ( TC_CALLBACK callback, uintptr_t context );


// *****************************************************************************
/* Function:
    void TCx_CHy_CaptureInitialize ( void )

  Summary:
    Initializes given instance of TC channel to capture mode.

  Description:
    This function initializes given instance of TC channel to capture mode with
    the options configured in MCC GUI, allowing it to be ready to start
    capturing selected signal event counter values.

  Precondition:
    MCC GUI should be configured with the desired values.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    TC0_CH1_CaptureInitialize();
    </code>

  Remarks:
    This function must be called before any other TC function is
    called.  This function is normally only called once during system
    initialization.
*/

void TCx_CHy_CaptureInitialize ( void );


// *****************************************************************************
/* Function:
    void TCx_CHy_CaptureStart ( void );

  Summary:
    Starts the given TC channel capture counter.

  Description:
    This function enables the clock and starts the counter for the associated
    channel so that it can start capturing signal-event counter values.

  Precondition:
    TCx_CHy_CaptureInitialize() function must have been called first for the
    given channel.

  Parameters:
    None

  Returns:
    None.

  Example:
    <code>
    TC0_CH1_CaptureInitialize();
    TC0_CH1_CaptureStart();
    </code>

  Remarks:
    None
*/

void TCx_CHy_CaptureStart ( void );


// *****************************************************************************
/* Function:
    void TCx_CHy_CaptureStop ( void );

  Summary:
    Stops the given TC channel counter.

  Description:
    This function stops the clock and thus counter of associated channel,
    preventing it from capturing signal-event counter values.

  Precondition:
    TCx_CHy_CaptureInitialize() function must have been called first for the
    given channel.

  Parameters:
    None

  Returns:
    None.

  Example:
    <code>
    TC0_CH1_CaptureInitialize();
    TC0_CH1_CaptureStart();
    // Capture signal event counter values.
    TC0_CH1_CaptureStop();
    </code>

  Remarks:
    None
*/


void TCx_CHy_CaptureStop ( void );


// *****************************************************************************
/* Function:
    void TCx_CHy_CaptureCallbackRegister ( TC_CALLBACK callback, uintptr_t context )


  Summary:
    Registers the function to be called from the capture-event.

  Description:
    This function registers the callback function to be called from the interrupt
    when the selected capture event occurs.

  Precondition:
    TCx_CHy_CaptureInitialize() must have been called first.

  Parameters:
      callback  - Callback function pointer.

      context   - Allows the caller to provide a context value (usually a
                  pointer to the callers context for multi-instance clients).

  Returns:
    None.

  Example:
    <code>
    void TC_CallbackFn(uintptr_t context);

    TC0_CH1_CaptureInitialize();
    TC0_CH1_CaptureCallbackRegister(TC_CallbackFn, NULL);
    </code>

  Remarks:
    Context value can be set to NULL if not required.

    To disable callback function, pass NULL for the callback parameter.
*/

void TCx_CHy_CaptureCallbackRegister ( TC_CALLBACK callback, uintptr_t context );


// *****************************************************************************
/* Function:
    uint16_t TCx_CHy_CaptureAGet ( void )

  Summary:
    Returns the Capture-A value.

  Description:
    This function provides the Capture-A value that was stored when the selected
    event occurred on the input signal.  The caller should call the
    TCx_CHy_CaptureAEventOccured function to identify if the Capture-A value
    has been updated by the selected signal event.

  Precondition:
    TCx_CHy_TimerInitialize() function must have been called first for the given
    channel to configure channel in capture mode.

  Parameters:
    None

  Returns:
    Most recent Capture-A value.

  Example:
    <code>
    uint16_t capture;


    if (TC0_CH1_CaptureAEventOccured() == true)
    {
        capture = TC0_CH1_CaptureAGet();
    }
    </code>

  Remarks:
    This function is applicable only for capture mode.
*/

uint16_t TCx_CHy_CaptureAGet ( void );


// *****************************************************************************
/* Function:
    uint16_t TCx_CHy_CaptureBGet ( void )

  Summary:
    Returns the Capture-B value.

  Description:
    This function provides the Capture-B value that was stored when the selected
    event occurred on the input signal.  The caller should call the
    TCx_CHy_CaptureBEventOccured function to identify if the Capture-B value
    has been updated by the selected signal event.

  Precondition:
    TCx_CHy_TimerInitialize function must have been called first for the given
    channel to configure channel in capture mode.

  Parameters:
    None

  Returns:
    Most recent Capture-B value.

  Example:
    <code>
    uint16_t capture;


    if (TC0_CH1_CaptureBEventOccured() == true)
    {
        capture = TC0_CH1_CaptureBGet();
    }
    </code>

  Remarks:
    This function is applicable only for capture mode.
*/

uint16_t TCx_CHy_CaptureBGet ( void );

// *****************************************************************************
/* Function:
    TC_CAPTURE_STATUS TCx_CHy_CaptureStatusGet ( void )

  Summary:
    Identifies status of the capture events

  Description:
    This function returns the status of the events that occur in capture mode.

  Precondition:
    TCx_CHy_TimerInitialize() function must have been called first for the given
    channel to configure channel in capture mode.

  Parameters:
    None

  Returns:
    TC_CAPTURE_STATUS - status of the capture events

  Example:
    <code>
    uint16_t capture;

    if ((TC0_CH1_CaptureStatusGet() & TC_CAPTURE_A_LOAD) == true)
    {
        capture = TC0_CH1_CaptureAGet();
    }
    </code>

  Remarks:
    This function is applicable only for capture mode. Event status bits are cleared
    after reading the status register.
*/

TC_CAPTURE_STATUS TCx_CHy_CaptureStatusGet ( void );

// *****************************************************************************
/* Function:
    void TCx_CHy_CompareInitialize ( void )

  Summary:
    Initializes given TC channel in compare mode.

  Description:
    This function initializes given instance of a TC channel in compare mode with
    the options selected in MCC GUI.

  Precondition:
    MCC GUI should be configured with the desired values.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    TCx_CHy_CompareInitialize();
    </code>

  Remarks:
    This function must be called before any other TC function is
    called.  This function should only be called once during system
    initialization.
*/

void TCx_CHy_CompareInitialize ( void );


// *****************************************************************************
/* Function:
    void TCx_CHy_CompareStart ( void )

  Summary:
    Starts the given TC channel compare counter.

  Description:
    This function enables the clock and starts the counter of associated channel
    in compare mode so that the TC module may start generating the selected
    waveform.

  Precondition:
    TCx_CHy_CompareInitialize() function must have been called first for the given
    channel.

  Parameters:
    None

  Returns:
    None.

  Example:
    <code>
    TC0_CH1_CompareInitialize();
    TC0_CH1_CompareStart();
    </code>

  Remarks:
    None
*/

void TCx_CHy_CompareStart ( void );


// *****************************************************************************
/* Function:
    void TCx_CHy_CompareStop ( void )

  Summary:
    Stops the given TC channel counter in compare mode.

  Description:
    This function stops the clock and thus counter of associated channel,
    freezing the associated waveform generation.

  Precondition:
    TCx_CHy_CompareInitialize function must have been called first for the
    given channel.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    TC0_CH1_CompareInitialize();
    TC0_CH1_CompareStart();
    // Generate waveform a long as desired, then...
    TC0_CH1_CompareStop();
    </code>

  Remarks:
    None
*/

void TCx_CHy_CompareStop ( void );


// *****************************************************************************
/* Function:
    void TCx_CHy_ComparePeriodSet ( uint32_t period );

  Summary:
    Sets the period value of a given TC channel in compare mode.

  Description:
    This function writes the TC compare timer's period value.  When counter
    matches the period value, it is reset, the waveform phase is updated and
    an interrupt can be generated.

  Precondition:
    TCx_CHy_CompareInitialize() function must have been called first for the
    given channel.

  Parameters:
    period  - The timer period value necessary to achieve the desired waveform
              frequency.

  Returns:
    None.

  Example:
    <code>
    TC0_CH1_CompareInitialize();
    TC0_CH1_ComparePeriodSet(0x500U);
    </code>

  Remarks:
    The caller must know the number of significant bits of timer.  The Period
    value is right-aligned.
*/

void TCx_CHy_ComparePeriodSet ( uint32_t period );


// *****************************************************************************
/* Function:
    uint32_t TCx_CHy_ComparePeriodGet ( void )

  Summary:
    Gets the period value of given timer channel in compare mode.

  Description:
    This function gets the current value of period of given timer channel in
    compare mode.

  Precondition:
    TCx_CHy_CompareInitialize() function must have been called first for the given channel.

  Parameters:
    None

  Returns:
    The timer's period value.

  Example:
    <code>
    uint32_t period;
    TC0_CH1_CompareInitialize();
    period = TC0_CH1_ComparePeriodGet();
    </code>

  Remarks:
    The caller must know the number of significant bits of timer.  Period
    value is right-aligned.
*/

uint32_t TCx_CHy_ComparePeriodGet ( void );

// *****************************************************************************
/* Function:
    uint32_t TCx_CHy_CompareFrequencyGet ( void )

  Summary:
    Provides the given timer's counter-increment frequency in compare mode.

  Description:
    This function provides the frequency at which the given counter
    increments in compare mode.  It can be used to convert differences between
    counter values to real time or real-time intervals to timer period values.

  Precondition:
    TCx_CHy_CompareInitialize function must have been called first for the
    given channel.

  Parameters:
    None

  Returns:
      The frequency (in Hz) at which the timer's counter increments.

  Example:
    <code>
    uint32_t frequency;
    TC0_CH1_CompareInitialize();
    frequency = TC0_CH1_CompareFrequencyGet();
    </code>

  Remarks:
    None
*/

uint32_t TCx_CHy_CompareFrequencyGet ( void );

// *****************************************************************************
/* Function:
    void TCx_CHy_CompareCallbackRegister ( TC_CALLBACK callback, uintptr_t context )

  Summary:
    Registers the function to be called from interrupt.

  Description
    This function registers the callback function to be called from interrupt

  Precondition:
    TCx_CHy_CompareInitialize must have been called first.

  Parameters:
      callback  - Callback function pointer.

      context   - Value provided back to the caller by the callback (usually a
                  pointer to the caller's context for multi-instance clients).

  Returns:
    None.

  Example:
    <code>
    void TC_CallbackFn(uintptr_t context);

    TC0_CH1_CompareInitialize();
    TC0_CH1_CompareCallbackRegister(TC_CallbackFn, NULL);
    </code>

  Remarks:
    Context value can be set to NULL if not required.

   To disable callback function, pass NULL for the callback parameter.
*/

void TCx_CHy_CompareCallbackRegister ( TC_CALLBACK callback, uintptr_t context );


// *****************************************************************************
/* Function:
    void TCx_CHy_CompareASet ( uint16_t value )

  Summary:
    Sets the Compare-A value of the given timer channel in compare mode.

  Description:
    This function sets the Compare-A value of the given timer channel, which
    decides the ON time the waveform generated in compare mode.

  Precondition:
    TCx_CHy_TimerInitialize function must have been called first for the given
    channel to configure channel in compare mode.

  Parameters:
    value - Compare-A value.

  Returns:
    None.

  Example:
    <code>
    TC0_CH1_CompareInitialize();
    TC0_CH1_CompareASet(0x100);
    </code>

  Remarks:
    This function is applicable only for compare mode.
*/

void TCx_CHy_CompareASet ( uint16_t value );


// *****************************************************************************
/* Function:
    void TCx_CHy_CompareBSet ( uint16_t value )

  Summary:
    Sets the Compare-B value of the given timer channel in compare mode.

  Description:
    This function sets the Compare-B value of the given timer channel, which
    decides the ON time the waveform generated in compare mode.

  Precondition:
    TCx_CHy_TimerInitialize function must have been called first for the given
    channel to configure channel in compare mode.

  Parameters:
    value - Compare-B value.

  Returns:
    None.

  Example:
    <code>
    TC0_CH1_CompareInitialize();
    TC0_CH1_CompareBSet(0x200);
    </code>

  Remarks:
    This function is applicable only for compare mode.
*/

void TCx_CHy_CompareBSet ( uint16_t value );

// *****************************************************************************
/* Function:
    TC_COMPARE_STATUS TCx_CHy_CompareStatusGet ( void )

  Summary:
    Identifies status of the compare events

  Description:
    This function returns the status of the events that occur in compare mode.

  Precondition:
    TCx_CHy_CompareInitialize() function must have been called first for the given
    channel to configure channel in capture mode.

  Parameters:
    None

  Returns:
    TC_COMPARE_STATUS - status of the capture events

  Example:
    <code>

    if ((TC0_CH1_CompareStatusGet() & TC_COMPARE_C) == true)
    {
        TC0_CH1_CompareASet(0x1000U);
        TC0_CH1_CompareBSet(0x500U);
    }
    </code>

  Remarks:
    This function is applicable only for compare mode. Event status bits are cleared
    after reading the status register.
*/

TC_COMPARE_STATUS TCx_CHy_CompareStatusGet ( void );

// *****************************************************************************
/* Function:
    void TCx_QuadratureInitialize ( void );

  Summary:
    Initializes given instance of TC channel in quadrature mode.

  Description:
    This function initializes given instance of TC channel in quadrature mode
    with options configured in MCC GUI.

  Precondition:
    MCC GUI should be configured with the desired options.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    TC0_QuadratureInitialize();
    </code>

  Remarks:
    This function must be called before any other TC function is called.

    This function is normally called only be called once during system
    initialization.
*/

void TCx_QuadratureInitialize ( void );

// *****************************************************************************
/* Function:
    void TCx_QuadratureStart ( void )

  Summary:
    Starts the given TC channel counter in quadrature mode.

  Description:
    This function enables the clock and starts the counter of associated channel
    in quadrature mode, allowing the TC to begin tracking the quadrature signals.

  Precondition:
    TCx_QuadratureInitialize function must have been called first for the given
    channel.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    TC0_QuadratureInitialize();
    TC0_QuadratureStart();
    </code>

  Remarks:
    None.
*/

void TCx_QuadratureStart ( void );

// *****************************************************************************
/* Function:
    void TCx_QuadratureStop ( void )

  Summary:
    Stops the given TC channel counter in quadrature mode.

  Description:
    This function stops the clock and thus counter of associated channel in
    quadrature mode, stopping tracking of the quadrature signals.

  Precondition:
    TCx_QuadratureInitialize function must have been called first for the given
    channel.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    TC0_QuadratureInitialize();
    TC0_QuadratureStart();
    TC0_QuadratureStop();
    </code>

  Remarks:
    None.
*/

void TCx_QuadratureStop ( void );
// *****************************************************************************
/* Function:
    int16_t TCx_QuadraturePositionGet ( void )

  Summary:
    Reads the angular position from the quadrature encoder.

  Description:
    This function reads the position from the quadrature encoder by reading as
    tracked by the channel counter.

  Precondition:
    TCx_QuadratureInitialize function must have been called first for the given
    channel.

  Parameters:
    None.

  Returns:
    Position of the encoder. 

  Example:
    <code>
    int16_t position;

    TC0_QuadratureInitialize();
    TC0_QuadratureStart();
    position = TC0_QuadraturePositionGet();
    </code>

  Remarks:
    In counter-clockwise direction, position counter works in 
    down counting mode. 
*/

int16_t TCx_QuadraturePositionGet ( void );

// *****************************************************************************
/* Function:
    int16_t TCx_QuadratureRevolutionsGet ( void )

  Summary:
    Reads the number of revolutions from the quadrature encoder.

  Description:
    This function reads the number of revolutions from the quadrature encoder by reading as
    tracked by the channel counter.

  Precondition:
    TCx_QuadratureInitialize function must have been called first for the given
    channel.

  Parameters:
    None.

  Returns:
    Number of revolutions of the encoder.  

  Example:
    <code>
    int16_t revolutions;

    TC0_QuadratureInitialize();
    TC0_QuadratureStart();
    revolutions = TC0_QuadratureRevolutionsGet();
    </code>

  Remarks:
    In counter-clockwise direction, revolution counter works in 
    down counting mode. This function is available only if quadrature
    encoder provides index pulse. 
*/

int16_t TCx_QuadratureRevolutionsGet ( void );
// *****************************************************************************
/* Function:
    uint16_t TCx_QuadratureSpeedGet ( void )

  Summary:
    Reads the quadrature index change speed.

  Description:
    This function reads the number of quadrature pulses captured in timer
    channel for given time base.

  Precondition:
    TCx_QuadratureInitialize function must have been called first for the given
    channel.

  Parameters:
    None

  Returns:
    Number of quadrature pulses counted in given time base which is used to determine
    speed of motion tracked by the encoder.

  Example:
    <code>
    uint16_t speed;
    TC0_QuadratureInitialize();
    TC0_QuadratureStart();
    speed = TC0_QuadratureSpeedGet();
    </code>

  Remarks:
    Caller must calculate the actual speed based on returned speed count value,
    time base and number of quadrature encoder pulses.
*/

uint16_t TCx_QuadratureSpeedGet ( void );

// *****************************************************************************
/* Function:
    TC_QUADRATURE_STATUS TCx_CHy_QuadratureStatusGet (void)

  Summary:
    Identifies status of the quadrature events

  Description:
    This function returns the status of the events that occur in quadrature mode.

  Precondition:
    TCx_CHy_QuadratureInitialize() function must have been called first for the given
    channel to configure channel in quadrature mode.

  Parameters:
    None

  Returns:
    TC_QUADRATURE_STATUS - status of the capture events

  Example:
    <code>

    uint32_t position;

    if ((TC0_QuadratureStatusGet() & TC_QUADRATURE_INDEX) == true)
    {
        position = TC0_QuadraturePositionGet()
    }
    </code>

  Remarks:
    This function is applicable only for quadrature mode. Event status bits are cleared
    after reading the status register.
*/

TC_QUADRATURE_STATUS TCx_QuadratureStatusGet ( void );

// *****************************************************************************
/* Function:
    void TCx_QuadratureCallbackRegister ( TC_CALLBACK callback, uintptr_t context )

  Summary:
    Registers the function to be called from interrupt.

  Description
    This function registers the callback function to be called from interrupt

  Precondition:
    TCx_QuadratureInitialize must have been called first.

  Parameters:
      callback  - Callback function pointer.

      context   - Value provided back to the caller by the callback (usually a
                  pointer to the caller's context for multi-instance clients).

  Returns:
    None.

  Example:
    <code>
    void TC_CallbackFn(uintptr_t context);

    TC0_QuadratureInitialize();
    TC0_QuadratureCallbackRegister(TC_CallbackFn, NULL);
    </code>

  Remarks:
    Context value can be set to NULL if not required.

   To disable callback function, pass NULL for the callback parameter.
*/

void TCx_QuadratureCallbackRegister ( TC_CALLBACK callback, uintptr_t context );

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

}

#endif
// DOM-IGNORE-END

#endif //_PLIB_TCx_H

/**
 End of File
*/