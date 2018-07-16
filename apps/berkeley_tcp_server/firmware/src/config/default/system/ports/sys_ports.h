/*******************************************************************************
  PORTS Service

  Company:
    Microchip Technology Inc.

  File Name:
    sys_ports.h

  Summary:
    PORTS Service Header File

  Description:
    This library provides an interface to control and interact with PORTS
    System Service.

*******************************************************************************/

/*******************************************************************************
Copyright (c) 2018 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS  WITHOUT  WARRANTY  OF  ANY  KIND,
EITHER EXPRESS  OR  IMPLIED,  INCLUDING  WITHOUT  LIMITATION,  ANY  WARRANTY  OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A  PARTICULAR  PURPOSE.
IN NO EVENT SHALL MICROCHIP OR  ITS  LICENSORS  BE  LIABLE  OR  OBLIGATED  UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,  BREACH  OF  WARRANTY,  OR
OTHER LEGAL  EQUITABLE  THEORY  ANY  DIRECT  OR  INDIRECT  DAMAGES  OR  EXPENSES
INCLUDING BUT NOT LIMITED TO ANY  INCIDENTAL,  SPECIAL,  INDIRECT,  PUNITIVE  OR
CONSEQUENTIAL DAMAGES, LOST  PROFITS  OR  LOST  DATA,  COST  OF  PROCUREMENT  OF
SUBSTITUTE  GOODS,  TECHNOLOGY,  SERVICES,  OR  ANY  CLAIMS  BY  THIRD   PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE  THEREOF),  OR  OTHER  SIMILAR  COSTS.
*******************************************************************************/

#ifndef SYS_PORTS_H
#define SYS_PORTS_H

#include "device.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data types and constants
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* Sys Port

  Summary:
    Identifies the available Port Channels.

  Description:
    This enumeration identifies the available Port Channels.

  Remarks:
    The caller should not rely on the specific numbers assigned to any of
    these values as they may change from one processor to the next.

    Not all ports are available on all devices.  Refer to the specific
    device data sheet to determine which ports are supported.
*/

typedef enum
{
    /* Port A Pins */
    SYS_PORT_A = PIOA_BASE_ADDRESS,

    /* Port B Pins */
    SYS_PORT_B = PIOB_BASE_ADDRESS,

    /* Port C Pins */
    SYS_PORT_C = PIOC_BASE_ADDRESS,

    /* Port D Pins */
    SYS_PORT_D = PIOD_BASE_ADDRESS,

    /* Port E Pins */
    SYS_PORT_E = PIOE_BASE_ADDRESS

} SYS_PORT;

// *****************************************************************************
/* Sys Port Pins

  Summary:
    Identifies the available port pins.

  Description:
    This enumeration identifies the available port pins.

  Remarks:
    The caller should not rely on the specific numbers assigned to any of
    these values as they may change from one processor to the next.

    Not all pins are available on all devices.  Refer to the specific
    device data sheet to determine which pins are supported.
*/

typedef enum
{
    /* PA0 pin */
    SYS_PORT_PIN_PA0,

    /* PA1 pin */
    SYS_PORT_PIN_PA1,

    /* PA2 pin */
    SYS_PORT_PIN_PA2,

    /* PA3 pin */
    SYS_PORT_PIN_PA3,

    /* PA4 pin */
    SYS_PORT_PIN_PA4,

    /* PA5 pin */
    SYS_PORT_PIN_PA5,

    /* PA6 pin */
    SYS_PORT_PIN_PA6,

    /* PA7 pin */
    SYS_PORT_PIN_PA7,

    /* PA8 pin */
    SYS_PORT_PIN_PA8,

    /* PA9 pin */
    SYS_PORT_PIN_PA9,

    /* PA10 pin */
    SYS_PORT_PIN_PA10,

    /* PA11 pin */
    SYS_PORT_PIN_PA11,

    /* PA12 pin */
    SYS_PORT_PIN_PA12,

    /* PA13 pin */
    SYS_PORT_PIN_PA13,

    /* PA14 pin */
    SYS_PORT_PIN_PA14,

    /* PA15 pin */
    SYS_PORT_PIN_PA15,

    /* PA16 pin */
    SYS_PORT_PIN_PA16,

    /* PA17 pin */
    SYS_PORT_PIN_PA17,

    /* PA18 pin */
    SYS_PORT_PIN_PA18,

    /* PA19 pin */
    SYS_PORT_PIN_PA19,

    /* PA20 pin */
    SYS_PORT_PIN_PA20,

    /* PA21 pin */
    SYS_PORT_PIN_PA21,

    /* PA22 pin */
    SYS_PORT_PIN_PA22,

    /* PA23 pin */
    SYS_PORT_PIN_PA23,

    /* PA24 pin */
    SYS_PORT_PIN_PA24,

    /* PA25 pin */
    SYS_PORT_PIN_PA25,

    /* PA26 pin */
    SYS_PORT_PIN_PA26,

    /* PA27 pin */
    SYS_PORT_PIN_PA27,

    /* PA28 pin */
    SYS_PORT_PIN_PA28,

    /* PA29 pin */
    SYS_PORT_PIN_PA29,

    /* PA30 pin */
    SYS_PORT_PIN_PA30,

    /* PA31 pin */
    SYS_PORT_PIN_PA31,

    /* PB0 pin */
    SYS_PORT_PIN_PB0,

    /* PB1 pin */
    SYS_PORT_PIN_PB1,

    /* PB2 pin */
    SYS_PORT_PIN_PB2,

    /* PB3 pin */
    SYS_PORT_PIN_PB3,

    /* PB4 pin */
    SYS_PORT_PIN_PB4,

    /* PB5 pin */
    SYS_PORT_PIN_PB5,

    /* PB6 pin */
    SYS_PORT_PIN_PB6,

    /* PB7 pin */
    SYS_PORT_PIN_PB7,

    /* PB8 pin */
    SYS_PORT_PIN_PB8,

    /* PB9 pin */
    SYS_PORT_PIN_PB9,

    /* PB10 pin */
    SYS_PORT_PIN_PB10,

    /* PB11 pin */
    SYS_PORT_PIN_PB11,

    /* PB12 pin */
    SYS_PORT_PIN_PB12,

    /* PB13 pin */
    SYS_PORT_PIN_PB13,

    /* PB14 pin */
    SYS_PORT_PIN_PB14,

    /* PB15 pin */
    SYS_PORT_PIN_PB15,

    /* PB16 pin */
    SYS_PORT_PIN_PB16,

    /* PB17 pin */
    SYS_PORT_PIN_PB17,

    /* PB18 pin */
    SYS_PORT_PIN_PB18,

    /* PB19 pin */
    SYS_PORT_PIN_PB19,

    /* PB20 pin */
    SYS_PORT_PIN_PB20,

    /* PB21 pin */
    SYS_PORT_PIN_PB21,

    /* PB22 pin */
    SYS_PORT_PIN_PB22,

    /* PB23 pin */
    SYS_PORT_PIN_PB23,

    /* PB24 pin */
    SYS_PORT_PIN_PB24,

    /* PB25 pin */
    SYS_PORT_PIN_PB25,

    /* PB26 pin */
    SYS_PORT_PIN_PB26,

    /* PB27 pin */
    SYS_PORT_PIN_PB27,

    /* PB28 pin */
    SYS_PORT_PIN_PB28,

    /* PB29 pin */
    SYS_PORT_PIN_PB29,

    /* PB30 pin */
    SYS_PORT_PIN_PB30,

    /* PB31 pin */
    SYS_PORT_PIN_PB31,

    /* PC0 pin */
    SYS_PORT_PIN_PC0,

    /* PC1 pin */
    SYS_PORT_PIN_PC1,

    /* PC2 pin */
    SYS_PORT_PIN_PC2,

    /* PC3 pin */
    SYS_PORT_PIN_PC3,

    /* PC4 pin */
    SYS_PORT_PIN_PC4,

    /* PC5 pin */
    SYS_PORT_PIN_PC5,

    /* PC6 pin */
    SYS_PORT_PIN_PC6,

    /* PC7 pin */
    SYS_PORT_PIN_PC7,

    /* PC8 pin */
    SYS_PORT_PIN_PC8,

    /* PC9 pin */
    SYS_PORT_PIN_PC9,

    /* PC10 pin */
    SYS_PORT_PIN_PC10,

    /* PC11 pin */
    SYS_PORT_PIN_PC11,

    /* PC12 pin */
    SYS_PORT_PIN_PC12,

    /* PC13 pin */
    SYS_PORT_PIN_PC13,

    /* PC14 pin */
    SYS_PORT_PIN_PC14,

    /* PC15 pin */
    SYS_PORT_PIN_PC15,

    /* PC16 pin */
    SYS_PORT_PIN_PC16,

    /* PC17 pin */
    SYS_PORT_PIN_PC17,

    /* PC18 pin */
    SYS_PORT_PIN_PC18,

    /* PC19 pin */
    SYS_PORT_PIN_PC19,

    /* PC20 pin */
    SYS_PORT_PIN_PC20,

    /* PC21 pin */
    SYS_PORT_PIN_PC21,

    /* PC22 pin */
    SYS_PORT_PIN_PC22,

    /* PC23 pin */
    SYS_PORT_PIN_PC23,

    /* PC24 pin */
    SYS_PORT_PIN_PC24,

    /* PC25 pin */
    SYS_PORT_PIN_PC25,

    /* PC26 pin */
    SYS_PORT_PIN_PC26,

    /* PC27 pin */
    SYS_PORT_PIN_PC27,

    /* PC28 pin */
    SYS_PORT_PIN_PC28,

    /* PC29 pin */
    SYS_PORT_PIN_PC29,

    /* PC30 pin */
    SYS_PORT_PIN_PC30,

    /* PC31 pin */
    SYS_PORT_PIN_PC31,

    /* PD0 pin */
    SYS_PORT_PIN_PD0,

    /* PD1 pin */
    SYS_PORT_PIN_PD1,

    /* PD2 pin */
    SYS_PORT_PIN_PD2,

    /* PD3 pin */
    SYS_PORT_PIN_PD3,

    /* PD4 pin */
    SYS_PORT_PIN_PD4,

    /* PD5 pin */
    SYS_PORT_PIN_PD5,

    /* PD6 pin */
    SYS_PORT_PIN_PD6,

    /* PD7 pin */
    SYS_PORT_PIN_PD7,

    /* PD8 pin */
    SYS_PORT_PIN_PD8,

    /* PD9 pin */
    SYS_PORT_PIN_PD9,

    /* PD10 pin */
    SYS_PORT_PIN_PD10,

    /* PD11 pin */
    SYS_PORT_PIN_PD11,

    /* PD12 pin */
    SYS_PORT_PIN_PD12,

    /* PD13 pin */
    SYS_PORT_PIN_PD13,

    /* PD14 pin */
    SYS_PORT_PIN_PD14,

    /* PD15 pin */
    SYS_PORT_PIN_PD15,

    /* PD16 pin */
    SYS_PORT_PIN_PD16,

    /* PD17 pin */
    SYS_PORT_PIN_PD17,

    /* PD18 pin */
    SYS_PORT_PIN_PD18,

    /* PD19 pin */
    SYS_PORT_PIN_PD19,

    /* PD20 pin */
    SYS_PORT_PIN_PD20,

    /* PD21 pin */
    SYS_PORT_PIN_PD21,

    /* PD22 pin */
    SYS_PORT_PIN_PD22,

    /* PD23 pin */
    SYS_PORT_PIN_PD23,

    /* PD24 pin */
    SYS_PORT_PIN_PD24,

    /* PD25 pin */
    SYS_PORT_PIN_PD25,

    /* PD26 pin */
    SYS_PORT_PIN_PD26,

    /* PD27 pin */
    SYS_PORT_PIN_PD27,

    /* PD28 pin */
    SYS_PORT_PIN_PD28,

    /* PD29 pin */
    SYS_PORT_PIN_PD29,

    /* PD30 pin */
    SYS_PORT_PIN_PD30,

    /* PD31 pin */
    SYS_PORT_PIN_PD31,

    /* PE0 pin */
    SYS_PORT_PIN_PE0,

    /* PE1 pin */
    SYS_PORT_PIN_PE1,

    /* PE2 pin */
    SYS_PORT_PIN_PE2,

    /* PE3 pin */
    SYS_PORT_PIN_PE3,

    /* PE4 pin */
    SYS_PORT_PIN_PE4,

    /* PE5 pin */
    SYS_PORT_PIN_PE5,

    /* PE6 pin */
    SYS_PORT_PIN_PE6,

    /* PE7 pin */
    SYS_PORT_PIN_PE7,

    /* PE8 pin */
    SYS_PORT_PIN_PE8,

    /* PE9 pin */
    SYS_PORT_PIN_PE9,

    /* PE10 pin */
    SYS_PORT_PIN_PE10,

    /* PE11 pin */
    SYS_PORT_PIN_PE11,

    /* PE12 pin */
    SYS_PORT_PIN_PE12,

    /* PE13 pin */
    SYS_PORT_PIN_PE13,

    /* PE14 pin */
    SYS_PORT_PIN_PE14,

    /* PE15 pin */
    SYS_PORT_PIN_PE15,

    /* PE16 pin */
    SYS_PORT_PIN_PE16,

    /* PE17 pin */
    SYS_PORT_PIN_PE17,

    /* PE18 pin */
    SYS_PORT_PIN_PE18,

    /* PE19 pin */
    SYS_PORT_PIN_PE19,

    /* PE20 pin */
    SYS_PORT_PIN_PE20,

    /* PE21 pin */
    SYS_PORT_PIN_PE21,

    /* PE22 pin */
    SYS_PORT_PIN_PE22,

    /* PE23 pin */
    SYS_PORT_PIN_PE23,

    /* PE24 pin */
    SYS_PORT_PIN_PE24,

    /* PE25 pin */
    SYS_PORT_PIN_PE25,

    /* PE26 pin */
    SYS_PORT_PIN_PE26,

    /* PE27 pin */
    SYS_PORT_PIN_PE27,

    /* PE28 pin */
    SYS_PORT_PIN_PE28,

    /* PE29 pin */
    SYS_PORT_PIN_PE29,

    /* PE30 pin */
    SYS_PORT_PIN_PE30,

    /* PE31 pin */
    SYS_PORT_PIN_PE31,

    /* This element should not be used in any of the PORTS APIs.
       It will be used by other modules or application to denote that none of the PORT Pin is used */
    SYS_PORT_PIN_NONE = -1

} SYS_PORT_PIN;


// *****************************************************************************
// *****************************************************************************
// Section: SYS PORT Functions which operates on one pin at a time
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void SYS_PORT_PinWrite(SYS_PORT_PIN pin, bool value)

  Summary:
    Writes to the selected pin.

  Description:
    This function writes/drives the "value" on the selected I/O line/pin.

  Precondition:
    None.

  Parameters:
    pin       - One of the IO pins from the enum SYS_PORT_PIN
    value     - value to be written on the selected pin:
                true  = set pin to high (1).
                false = clear pin to low (0).

  Returns:
    None.

  Example:
    <code>
    SYS_PORT_PinWrite(SYS_PORT_PIN_PB3, true);
    </code>

  Remarks:
    None.
*/
static inline void SYS_PORT_PinWrite(SYS_PORT_PIN pin, bool value);

// *****************************************************************************
/* Function:
    bool SYS_PORT_PinRead(SYS_PORT_PIN pin)

  Summary:
    Read the selected pin value.

  Description:
    This function reads the selected pin value.
    it reads the value regardless of pin configuration, whether uniquely as an
    input, or driven by the PIO Controller, or driven by peripheral.

  Precondition:
    Reading the I/O line levels requires the clock of the PIO Controller to be
    enabled, otherwise this API reads the levels present on the I/O line at the
    time the clock was disabled.

  Parameters:
    pin - One of the IO pins from the enum SYS_PORT_PIN

  Returns:
    Returns the read value of the selected I/O pin.

  Example:
    <code>

    bool value;
    value = SYS_PORT_PinRead(SYS_PORT_PIN_PB3);

    </code>

  Remarks:
       To read the latched value on this pin, SYS_PORT_PinReadLatch API should be used.
*/
static inline bool SYS_PORT_PinRead(SYS_PORT_PIN pin);

// *****************************************************************************
/* Function:
    bool SYS_PORT_PinReadLatch ( SYS_PORT_PIN pin )

  Summary:
    Read the value driven on the selected pin.

  Description:
    This function reads the data driven on the selected I/O line/pin.
    Whatever data is written/driven on I/O line by using any of the PORTS
    APIs, will be read by this API.

  Precondition:
    None.

  Parameters:
    pin - One of the IO pins from the enum SYS_PORT_PIN

  Returns:
    Returns the value driven on the selected I/O pin.

  Example:
    <code>

    bool value;
    value = SYS_PORT_PinReadLatch(SYS_PORT_PIN_PB3);

    </code>

  Remarks:
    To read actual pin value, SYS_PORT_PinRead API should be used.
*/
static inline bool SYS_PORT_PinReadLatch(SYS_PORT_PIN pin);

// *****************************************************************************
/* Function:
    void SYS_PORT_PinToggle(SYS_PORT_PIN pin)

  Summary:
    Toggles the selected pin.

  Description:
    This function toggles/inverts the value on the selected I/O line/pin.

  Precondition:
    None.

  Parameters:
    pin - One of the IO pins from the enum SYS_PORT_PIN

  Returns:
    None.

  Example:
    <code>

    SYS_PORT_PinToggle(SYS_PORT_PIN_PB3);

    </code>

  Remarks:
    None.
*/
static inline void SYS_PORT_PinToggle(SYS_PORT_PIN pin);

// *****************************************************************************
/* Function:
    void SYS_PORT_PinSet(SYS_PORT_PIN pin)

  Summary:
    Sets the selected pin.

  Description:
    This function drives '1' on the selected I/O line/pin.

  Precondition:
    None.

  Parameters:
    pin - One of the IO pins from the enum SYS_PORT_PIN

  Returns:
    None.

  Example:
    <code>

    SYS_PORT_PinSet(SYS_PORT_PIN_PB3);

    </code>

  Remarks:
    None.
*/
static inline void SYS_PORT_PinSet(SYS_PORT_PIN pin);

// *****************************************************************************
/* Function:
    void SYS_PORT_PinClear(SYS_PORT_PIN pin)

  Summary:
    Clears the selected pin.

  Description:
    This function drives '0' on the selected I/O line/pin.

  Precondition:
    None.

  Parameters:
    pin - One of the IO pins from the enum SYS_PORT_PIN

  Returns:
    None.

  Example:
    <code>

    SYS_PORT_PinClear(SYS_PORT_PIN_PB3);

    </code>

  Remarks:
    None.
*/
static inline void SYS_PORT_PinClear(SYS_PORT_PIN pin);

// *****************************************************************************
/* Function:
    void SYS_PORT_PinInputEnable(SYS_PORT_PIN pin)

  Summary:
    Enables selected IO pin as input.

  Description:
    This function enables selected IO pin as input.

  Precondition:
    None.

  Parameters:
    pin - One of the IO pins from the enum SYS_PORT_PIN

  Returns:
    None.

  Example:
    <code>

    SYS_PORT_PinInputEnable(SYS_PORT_PIN_PB3);

    </code>

  Remarks:
    None.
*/
static inline void SYS_PORT_PinInputEnable(SYS_PORT_PIN pin);

// *****************************************************************************
/* Function:
    void SYS_PORT_PinOutputEnable(SYS_PORT_PIN pin)

  Summary:
    Enables selected IO pin as output.

  Description:
    This function enables selected IO pin as output.

  Precondition:
    None.

  Parameters:
    pin - One of the IO pins from the enum SYS_PORT_PIN

  Returns:
    None.

  Example:
    <code>

    SYS_PORT_PinOutputEnable(SYS_PORT_PIN_PB3);

    </code>

  Remarks:
    None.
*/
static inline void SYS_PORT_PinOutputEnable(SYS_PORT_PIN pin);

// *****************************************************************************
/* Function:
    void SYS_PORT_PinInterruptEnable(SYS_PORT_PIN pin)

  Summary:
    Enables IO interrupt on selected IO pin.

  Description:
    This function enables interrupt on selected IO pin.

  Precondition:
    None.

  Parameters:
    pin - One of the IO pins from the enum SYS_PORT_PIN

  Returns:
    None.

  Example:
    <code>

    SYS_PORT_PinInterruptEnable(SYS_PORT_PIN_PB3);

    </code>

  Remarks:
    None.
*/
static inline void SYS_PORT_PinInterruptEnable(SYS_PORT_PIN pin);

// *****************************************************************************
/* Function:
    void SYS_PORT_PinInterruptDisable(SYS_PORT_PIN pin)

  Summary:
    Disables IO interrupt on selected IO pin.

  Description:
    This function disables IO interrupt on selected IO pin.

  Precondition:
    None.

  Parameters:
    pin       - One of the IO pins from the enum SYS_PORT_PIN

  Returns:
    None.

  Example:
    <code>

    SYS_PORT_PinInterruptDisable(SYS_PORT_PIN_PB3);

    </code>

  Remarks:
    None.
*/
static inline void SYS_PORT_PinInterruptDisable(SYS_PORT_PIN pin);

#include "sys_ports_mapping.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END
#endif // SYS_PORTS_H
