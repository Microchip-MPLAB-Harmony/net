/*******************************************************************************
  PIO PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_pio_pin.h

  Summary:
    PIO PLIB Pin header file

  Description:
    This library has the inline implementations of several Pin based functions
    provided by Parallel Input/Output controller (PIO) PLIB.

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

#ifndef PLIB_PIO_PIN_H
#define PLIB_PIO_PIN_H

// *****************************************************************************
// *****************************************************************************
// Section: PIO Functions which operates on one pin at a time
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void PIO_PinWrite(PIO_PIN pin, bool value)

  Summary:
    Writes the selected pin.

  Remarks:
    See plib_pio.h for more details.
*/
static inline void PIO_PinWrite(PIO_PIN pin, bool value)
{
    PIO_PortWrite(PIOA_BASE_ADDRESS + (0x200 * (pin>>5)), (uint32_t)(0x1) << (pin & 0x1f), (uint32_t)(value) << (pin & 0x1f));
}

// *****************************************************************************
/* Function:
    bool PIO_PinRead(PIO_PIN pin)

  Summary:
    Read the selected pin value.

  Remarks:
    See plib_pio.h for more details.
*/
static inline bool PIO_PinRead(PIO_PIN pin)
{
    return (bool)((PIO_PortRead(PIOA_BASE_ADDRESS + (0x200 * (pin>>5))) >> (pin & 0x1F)) & 0x1);
}

// *****************************************************************************
/* Function:
    bool PIO_PinLatchRead ( PIO_PIN pin )

  Summary:
    Read the value driven on the selected pin.

  Remarks:
    See plib_pio.h for more details.
*/
static inline bool PIO_PinLatchRead(PIO_PIN pin)
{
    return (bool)((PIO_PortLatchRead(PIOA_BASE_ADDRESS + (0x200 * (pin>>5))) >> (pin & 0x1F)) & 0x1);
}

// *****************************************************************************
/* Function:
    void PIO_PinToggle(PIO_PIN pin)

  Summary:
    Toggles the selected pin.

  Remarks:
    See plib_pio.h for more details.
*/
static inline void PIO_PinToggle(PIO_PIN pin)
{
    PIO_PortToggle(PIOA_BASE_ADDRESS + (0x200 * (pin>>5)), 0x1 << (pin & 0x1F));
}

// *****************************************************************************
/* Function:
    void PIO_PinSet(PIO_PIN pin)

  Summary:
    Sets the selected pin.

  Remarks:
    See plib_pio.h for more details.
*/
static inline void PIO_PinSet(PIO_PIN pin)
{
    PIO_PortSet(PIOA_BASE_ADDRESS + (0x200 * (pin>>5)), 0x1 << (pin & 0x1F));
}

// *****************************************************************************
/* Function:
    void PIO_PinClear(PIO_PIN pin)

  Summary:
    Clears the selected pin.

  Remarks:
    See plib_pio.h for more details.
*/
static inline void PIO_PinClear(PIO_PIN pin)
{
    PIO_PortClear(PIOA_BASE_ADDRESS + (0x200 * (pin>>5)), 0x1 << (pin & 0x1F));
}

// *****************************************************************************
/* Function:
    void PIO_PinInputEnable(PIO_PIN pin)

  Summary:
    Enables selected IO pin as input.

  Remarks:
    See plib_pio.h for more details.
*/
static inline void PIO_PinInputEnable(PIO_PIN pin)
{
    PIO_PortInputEnable(PIOA_BASE_ADDRESS + (0x200 * (pin>>5)), 0x1 << (pin & 0x1F));
}

// *****************************************************************************
/* Function:
    void PIO_PinOutputEnable(PIO_PIN pin)

  Summary:
    Enables selected IO pin as output.

  Remarks:
    See plib_pio.h for more details.
*/
static inline void PIO_PinOutputEnable(PIO_PIN pin)
{
    PIO_PortOutputEnable(PIOA_BASE_ADDRESS + (0x200 * (pin>>5)), 0x1 << (pin & 0x1F));
}

// *****************************************************************************
/* Function:
    void PIO_PinInterruptEnable(PIO_PIN pin)

  Summary:
    Enables IO interrupt on selected IO pin.

  Remarks:
    See plib_pio.h for more details.
*/
static inline void PIO_PinInterruptEnable(PIO_PIN pin)
{
    PIO_PortInterruptEnable(PIOA_BASE_ADDRESS + (0x200 * (pin>>5)), 0x1 << (pin & 0x1F));
}

// *****************************************************************************
/* Function:
    void PIO_PinInterruptDisable(PIO_PIN pin)

  Summary:
    Disables IO interrupt on selected IO pin.

  Remarks:
    See plib_pio.h for more details.
*/
static inline void PIO_PinInterruptDisable(PIO_PIN pin)
{
    PIO_PortInterruptDisable(PIOA_BASE_ADDRESS + (0x200 * (pin>>5)), 0x1 << (pin & 0x1F));
}

#endif // PLIB_PIO_PIN_H
