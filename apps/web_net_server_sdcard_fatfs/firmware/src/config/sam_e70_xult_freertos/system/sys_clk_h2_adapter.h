/*******************************************************************************
  Adapter Clock System Service Library Interface Header File

  Company
    Microchip Technology Inc.

  File Name
    sys_clk_h2_adapter.h

  Summary
    Clock system service library interface.

  Description
    This is the adapter file defines the interface to the Clock system service 
    library from H2. The System Clock service APIs are used to provide an adapter 
    routines which are part of H2.

  Remarks:
   
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2017-2018 Microchip Technology Inc. and its subsidiaries.

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

#ifndef SYS_CLK_ADPATER_H    // Guards against multiple inclusion
#define SYS_CLK_ADPATER_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section lists the other files that are included in this file.
*/

#include <stdint.h>
#include <stdbool.h>
#include "system/system.h"
#include "toolchain_specifics.h"

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
// *****************************************************************************
/*  Internal Clock Outputs enumeration

  Summary:
    Lists the available internal clocks.

  Description:
    This enumeration lists all of the available internal clocks. This is
    used by the SYS_CLK_FrequencyGet function to get an actual frequency
    of the corresponding internal clock source.

  Remarks:
    This enumeration is supported on the following Microchip device families:
        - PIC32C

    The system service covers a wide range of devices. Not all the elements
    listed in here will be relevant for your device.
*/

typedef enum
{
    /* Core Clock (HCLK) */
    SYS_CLK_PROCESSOR,

    /* System Clock (MCK) */
    SYS_CLK_MASTER,

    /* SysTick Clock */
    SYS_CLK_SYSTICK,

    /* Generic Clock 0 (I2S 0) */
    SYS_CLK_GENERIC_0,

    /* Generic Clock 1 (I2S 1) */
    SYS_CLK_GENERIC_1,

    /* Peripheral Clock 0 */
    SYS_CLK_PERIPHERAL_0,

    /* Peripheral Clock 1 */
    SYS_CLK_PERIPHERAL_1,

    /* Programmable Clock 0 */
    SYS_CLK_PROGRAMMABLE_0,

    /* Programmable Clock 1 */
    SYS_CLK_PROGRAMMABLE_1,

    /* Programmable Clock 2 */
    SYS_CLK_PROGRAMMABLE_2,

    /* Programmable Clock 3 (ETM) */
    SYS_CLK_PROGRAMMABLE_3,

    /* Programmable Clock 4 (UARTx / USARTx) */
    SYS_CLK_PROGRAMMABLE_4,

    /* Programmable Clock 5 (CANx) */
    SYS_CLK_PROGRAMMABLE_5,

    /* Programmable Clock 6 (TCx) */
    SYS_CLK_PROGRAMMABLE_6,

    /* Programmable Clock 7 (TC0) */
    SYS_CLK_PROGRAMMABLE_7,

    /* USB Full-Speed Clock (USB_48M) */
    SYS_CLK_USB_FS,

    /* USB High-Speed Clock (USB_480M) */
    SYS_CLK_USB_HS,

} SYS_CLK_CLOCK;
	


// *****************************************************************************
// *****************************************************************************
// Section: System Interface Functions
// *****************************************************************************
// *****************************************************************************
/*  System interface functions are called by system code to initialize the
    module and maintain proper operation of it.
*/

// *****************************************************************************
/* Function:
   uint32_t SYS_CLK_FrequencyGet ( SYS_CLK_CLOCK clock  )

Summary:
Returns status of the specific module instance of the Timer module.

Description:
This function returns the status of the specific module instance disabling its
operation (and any hardware for driver modules).

PreCondition:
The SYS_TMR_Initialize function should have been called before calling
this function.

Parameters:
object          - SYS TMR object handle, returned from SYS_TMR_Initialize

Returns:
SYS_STATUS_READY    Indicates that any previous module operation for the
specified module has completed

SYS_STATUS_BUSY     Indicates that a previous module operation for the
specified module has not yet completed

SYS_STATUS_ERROR    Indicates that the specified module is in an error state
*/

static __inline__ uint32_t __attribute__((always_inline)) SYS_CLK_FrequencyGet ( SYS_CLK_CLOCK clock  )
{
    return 150000000;           // ATSAME70Q21B
}


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END

#endif //SYS_CLK_ADPATER_H
