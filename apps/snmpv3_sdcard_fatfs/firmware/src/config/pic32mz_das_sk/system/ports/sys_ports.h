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
    SYS_PORT_A = 0,
    SYS_PORT_B = 1,
    SYS_PORT_C = 2,
    SYS_PORT_D = 3,
    SYS_PORT_E = 4,
    SYS_PORT_F = 5,
    SYS_PORT_G = 6,
    SYS_PORT_H = 7,
    SYS_PORT_J = 8,
    SYS_PORT_K = 9,

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

    SYS_PORT_PIN_RA0 = 0,
    SYS_PORT_PIN_RA1 = 1,
    SYS_PORT_PIN_RA2 = 2,
    SYS_PORT_PIN_RA3 = 3,
    SYS_PORT_PIN_RA4 = 4,
    SYS_PORT_PIN_RA5 = 5,
    SYS_PORT_PIN_RA6 = 6,
    SYS_PORT_PIN_RA7 = 7,
    SYS_PORT_PIN_RA9 = 9,
    SYS_PORT_PIN_RA10 = 10,
    SYS_PORT_PIN_RA14 = 14,
    SYS_PORT_PIN_RA15 = 15,
    SYS_PORT_PIN_RB0 = 16,
    SYS_PORT_PIN_RB1 = 17,
    SYS_PORT_PIN_RB2 = 18,
    SYS_PORT_PIN_RB3 = 19,
    SYS_PORT_PIN_RB4 = 20,
    SYS_PORT_PIN_RB5 = 21,
    SYS_PORT_PIN_RB6 = 22,
    SYS_PORT_PIN_RB7 = 23,
    SYS_PORT_PIN_RB8 = 24,
    SYS_PORT_PIN_RB9 = 25,
    SYS_PORT_PIN_RB10 = 26,
    SYS_PORT_PIN_RB11 = 27,
    SYS_PORT_PIN_RB12 = 28,
    SYS_PORT_PIN_RB13 = 29,
    SYS_PORT_PIN_RB14 = 30,
    SYS_PORT_PIN_RB15 = 31,
    SYS_PORT_PIN_RC1 = 33,
    SYS_PORT_PIN_RC2 = 34,
    SYS_PORT_PIN_RC3 = 35,
    SYS_PORT_PIN_RC4 = 36,
    SYS_PORT_PIN_RC12 = 44,
    SYS_PORT_PIN_RC13 = 45,
    SYS_PORT_PIN_RC14 = 46,
    SYS_PORT_PIN_RC15 = 47,
    SYS_PORT_PIN_RD0 = 48,
    SYS_PORT_PIN_RD1 = 49,
    SYS_PORT_PIN_RD2 = 50,
    SYS_PORT_PIN_RD3 = 51,
    SYS_PORT_PIN_RD4 = 52,
    SYS_PORT_PIN_RD5 = 53,
    SYS_PORT_PIN_RD6 = 54,
    SYS_PORT_PIN_RD7 = 55,
    SYS_PORT_PIN_RD9 = 57,
    SYS_PORT_PIN_RD10 = 58,
    SYS_PORT_PIN_RD11 = 59,
    SYS_PORT_PIN_RD12 = 60,
    SYS_PORT_PIN_RD13 = 61,
    SYS_PORT_PIN_RD14 = 62,
    SYS_PORT_PIN_RD15 = 63,
    SYS_PORT_PIN_RE0 = 64,
    SYS_PORT_PIN_RE1 = 65,
    SYS_PORT_PIN_RE2 = 66,
    SYS_PORT_PIN_RE3 = 67,
    SYS_PORT_PIN_RE4 = 68,
    SYS_PORT_PIN_RE5 = 69,
    SYS_PORT_PIN_RE6 = 70,
    SYS_PORT_PIN_RE7 = 71,
    SYS_PORT_PIN_RE8 = 72,
    SYS_PORT_PIN_RE9 = 73,
    SYS_PORT_PIN_RF0 = 80,
    SYS_PORT_PIN_RF1 = 81,
    SYS_PORT_PIN_RF2 = 82,
    SYS_PORT_PIN_RF3 = 83,
    SYS_PORT_PIN_RF4 = 84,
    SYS_PORT_PIN_RF5 = 85,
    SYS_PORT_PIN_RF8 = 88,
    SYS_PORT_PIN_RF12 = 92,
    SYS_PORT_PIN_RF13 = 93,
    SYS_PORT_PIN_RG0 = 96,
    SYS_PORT_PIN_RG1 = 97,
    SYS_PORT_PIN_RG6 = 102,
    SYS_PORT_PIN_RG7 = 103,
    SYS_PORT_PIN_RG8 = 104,
    SYS_PORT_PIN_RG9 = 105,
    SYS_PORT_PIN_RG12 = 108,
    SYS_PORT_PIN_RG13 = 109,
    SYS_PORT_PIN_RG14 = 110,
    SYS_PORT_PIN_RG15 = 111,
    SYS_PORT_PIN_RH0 = 112,
    SYS_PORT_PIN_RH1 = 113,
    SYS_PORT_PIN_RH2 = 114,
    SYS_PORT_PIN_RH3 = 115,
    SYS_PORT_PIN_RH4 = 116,
    SYS_PORT_PIN_RH5 = 117,
    SYS_PORT_PIN_RH6 = 118,
    SYS_PORT_PIN_RH7 = 119,
    SYS_PORT_PIN_RH8 = 120,
    SYS_PORT_PIN_RH9 = 121,
    SYS_PORT_PIN_RH10 = 122,
    SYS_PORT_PIN_RH11 = 123,
    SYS_PORT_PIN_RH12 = 124,
    SYS_PORT_PIN_RH13 = 125,
    SYS_PORT_PIN_RH14 = 126,
    SYS_PORT_PIN_RH15 = 127,
    SYS_PORT_PIN_RJ0 = 128,
    SYS_PORT_PIN_RJ1 = 129,
    SYS_PORT_PIN_RJ2 = 130,
    SYS_PORT_PIN_RJ3 = 131,
    SYS_PORT_PIN_RJ4 = 132,
    SYS_PORT_PIN_RJ5 = 133,
    SYS_PORT_PIN_RJ6 = 134,
    SYS_PORT_PIN_RJ7 = 135,
    SYS_PORT_PIN_RJ8 = 136,
    SYS_PORT_PIN_RJ9 = 137,
    SYS_PORT_PIN_RJ10 = 138,
    SYS_PORT_PIN_RJ11 = 139,
    SYS_PORT_PIN_RJ12 = 140,
    SYS_PORT_PIN_RJ13 = 141,
    SYS_PORT_PIN_RJ14 = 142,
    SYS_PORT_PIN_RJ15 = 143,
    SYS_PORT_PIN_RK0 = 144,
    SYS_PORT_PIN_RK1 = 145,
    SYS_PORT_PIN_RK2 = 146,
    SYS_PORT_PIN_RK3 = 147,
    SYS_PORT_PIN_RK4 = 148,
    SYS_PORT_PIN_RK5 = 149,
    SYS_PORT_PIN_RK6 = 150,
    SYS_PORT_PIN_RK7 = 151,

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
    Port Initialization must have been done using appropriate Initialize API call.

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
       To read the latched value on this pin, SYS_PORT_PinLatchRead API should be used.
*/
static inline bool SYS_PORT_PinRead(SYS_PORT_PIN pin);

// *****************************************************************************
/* Function:
    bool SYS_PORT_PinLatchRead ( SYS_PORT_PIN pin )

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
    value = SYS_PORT_PinLatchRead(SYS_PORT_PIN_PB3);

    </code>

  Remarks:
    To read actual pin value, SYS_PORT_PinRead API should be used.
*/
static inline bool SYS_PORT_PinLatchRead(SYS_PORT_PIN pin);

// *****************************************************************************
/* Function:
    void SYS_PORT_PinToggle(SYS_PORT_PIN pin)

  Summary:
    Toggles the selected pin.

  Description:
    This function toggles/inverts the value on the selected I/O line/pin.

  Precondition:
    Port Initialization must have been done using appropriate Initialize API call.

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


#include "sys_ports_mapping.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END
#endif // SYS_PORTS_H
