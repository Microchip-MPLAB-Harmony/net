/*******************************************************************************
  SYS PORTS Static Functions for PORTS System Service

  Company:
    Microchip Technology Inc.

  File Name:
    plib_pio.c

  Summary:
    PIO function implementations for the PIO PLIB.

  Description:
    The PIO PLIB provides a simple interface to manage peripheral
    input-output controller.

*******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

#include "plib_pio.h"


/******************************************************************************
  Function:
    PIO_Initialize ( void )

  Summary:
    Initialize the PIO library.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_Initialize ( void )
{
    /* Selected System IO pins are configured as GPIO */
    MATRIX_REGS->CCFG_SYSIO |= 0x10;

    /************************ PIO A Initialization ************************/
    /* PORTA PIO Disable and Peripheral Enable*/
    ((pio_registers_t*)PIO_PORT_A)->PIO_PDR = 0x200000;
    /* PORTA Pull Up Enable/Disable as per MHC selection */
    ((pio_registers_t*)PIO_PORT_A)->PIO_PUDR = ~0x800;
    ((pio_registers_t*)PIO_PORT_A)->PIO_PUER = 0x800;
    /* PORTA Pull Down Enable/Disable as per MHC selection */
    ((pio_registers_t*)PIO_PORT_A)->PIO_PPDDR = 0xFFFFFFFF;
    /* PORTA Output Write Enable */
    ((pio_registers_t*)PIO_PORT_A)->PIO_OWER = PIO_OWER_Msk;
    /* PORTA Output Direction Enable */
    ((pio_registers_t*)PIO_PORT_A)->PIO_OER = 0x20;
    /* PORTA Initial state High */
    ((pio_registers_t*)PIO_PORT_A)->PIO_SODR = 0x20;

    /************************ PIO B Initialization ************************/
    /* PORTB Peripheral Function Selection */
    ((pio_registers_t*)PIO_PORT_B)->PIO_ABCDSR[0]= 0x10;
    ((pio_registers_t*)PIO_PORT_B)->PIO_ABCDSR[1]= 0x10;
    /* PORTB PIO Disable and Peripheral Enable*/
    ((pio_registers_t*)PIO_PORT_B)->PIO_PDR = 0x10;
    /* PORTB Pull Up Enable/Disable as per MHC selection */
    ((pio_registers_t*)PIO_PORT_B)->PIO_PUDR = 0xFFFFFFFF;
    /* PORTB Pull Down Enable/Disable as per MHC selection */
    ((pio_registers_t*)PIO_PORT_B)->PIO_PPDDR = 0xFFFFFFFF;
    /* PORTB Output Write Enable */
    ((pio_registers_t*)PIO_PORT_B)->PIO_OWER = PIO_OWER_Msk;
    /* PORTB Output Direction Enable */
    ((pio_registers_t*)PIO_PORT_B)->PIO_OER = 0x100;
    /* PORTB Initial state High */
    ((pio_registers_t*)PIO_PORT_B)->PIO_SODR = 0x100;

    /************************ PIO C Initialization ************************/
    /* PORTC Pull Up Enable/Disable as per MHC selection */
    ((pio_registers_t*)PIO_PORT_C)->PIO_PUDR = 0xFFFFFFFF;
    /* PORTC Pull Down Enable/Disable as per MHC selection */
    ((pio_registers_t*)PIO_PORT_C)->PIO_PPDDR = 0xFFFFFFFF;
    /* PORTC Output Write Enable */
    ((pio_registers_t*)PIO_PORT_C)->PIO_OWER = PIO_OWER_Msk;

    /************************ PIO D Initialization ************************/
    /* PORTD PIO Disable and Peripheral Enable*/
    ((pio_registers_t*)PIO_PORT_D)->PIO_PDR = 0x3ff;
    /* PORTD Pull Up Enable/Disable as per MHC selection */
    ((pio_registers_t*)PIO_PORT_D)->PIO_PUDR = 0xFFFFFFFF;
    /* PORTD Pull Down Enable/Disable as per MHC selection */
    ((pio_registers_t*)PIO_PORT_D)->PIO_PPDDR = 0xFFFFFFFF;
    /* PORTD Output Write Enable */
    ((pio_registers_t*)PIO_PORT_D)->PIO_OWER = PIO_OWER_Msk;

    /************************ PIO E Initialization ************************/
    /* PORTE Pull Up Enable/Disable as per MHC selection */
    ((pio_registers_t*)PIO_PORT_E)->PIO_PUDR = 0xFFFFFFFF;
    /* PORTE Pull Down Enable/Disable as per MHC selection */
    ((pio_registers_t*)PIO_PORT_E)->PIO_PPDDR = 0xFFFFFFFF;
    /* PORTE Output Write Enable */
    ((pio_registers_t*)PIO_PORT_E)->PIO_OWER = PIO_OWER_Msk;

}

// *****************************************************************************
// *****************************************************************************
// Section: PIO APIs which operates on multiple pins of a port
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    uint32_t PIO_PortRead ( PIO_PORT port )

  Summary:
    Read all the I/O lines of the selected port.

  Description:
    This function reads the live data values on all the I/O lines of the
    selected port.  Bit values returned in each position indicate corresponding
    pin levels.
    1 = Pin is high.
    0 = Pin is low.

    This function reads the value regardless of pin configuration, whether it is
    set as as an input, driven by the PIO Controller, or driven by a peripheral.

  Remarks:
    If the port has less than 32-bits, unimplemented pins will read as
    low (0).
    Implemented pins are Right aligned in the 32-bit return value.
*/
uint32_t PIO_PortRead(PIO_PORT port)
{
    return ((pio_registers_t*)port)->PIO_PDSR;
}

// *****************************************************************************
/* Function:
    void PIO_PortWrite (PIO_PORT port, uint32_t mask, uint32_t value);

  Summary:
    Write the value on the masked I/O lines of the selected port.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortWrite(PIO_PORT port, uint32_t mask, uint32_t value)
{
    ((pio_registers_t*)port)->PIO_ODSR = (((pio_registers_t*)port)->PIO_ODSR & (~mask)) | (mask & value);
}

// *****************************************************************************
/* Function:
    uint32_t PIO_PortLatchRead ( PIO_PORT port )

  Summary:
    Read the latched value on all the I/O lines of the selected port.

  Remarks:
    See plib_pio.h for more details.
*/
uint32_t PIO_PortLatchRead(PIO_PORT port)
{
    return ((pio_registers_t*)port)->PIO_ODSR;
}

// *****************************************************************************
/* Function:
    void PIO_PortSet ( PIO_PORT port, uint32_t mask )

  Summary:
    Set the selected IO pins of a port.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortSet(PIO_PORT port, uint32_t mask)
{
    ((pio_registers_t*)port)->PIO_SODR = mask;
}

// *****************************************************************************
/* Function:
    void PIO_PortClear ( PIO_PORT port, uint32_t mask )

  Summary:
    Clear the selected IO pins of a port.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortClear(PIO_PORT port, uint32_t mask)
{
    ((pio_registers_t*)port)->PIO_CODR = mask;
}

// *****************************************************************************
/* Function:
    void PIO_PortToggle ( PIO_PORT port, uint32_t mask )

  Summary:
    Toggles the selected IO pins of a port.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortToggle(PIO_PORT port, uint32_t mask)
{
    /* Write into Clr and Set registers */
    ((pio_registers_t*)port)->PIO_ODSR^= mask;
}

// *****************************************************************************
/* Function:
    void PIO_PortInputEnable ( PIO_PORT port, uint32_t mask )

  Summary:
    Enables selected IO pins of a port as input.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortInputEnable(PIO_PORT port, uint32_t mask)
{
    ((pio_registers_t*)port)->PIO_ODR = mask;
}

// *****************************************************************************
/* Function:
    void PIO_PortOutputEnable ( PIO_PORT port, uint32_t mask )

  Summary:
    Enables selected IO pins of a port as output(s).

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortOutputEnable(PIO_PORT port, uint32_t mask)
{
    ((pio_registers_t*)port)->PIO_OER = mask;
}







/*******************************************************************************
 End of File
*/
