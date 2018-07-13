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
Copyright (c) 2018 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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
   MATRIX_REGS->CCFG_SYSIO|= 0x00000010;
   /************************ PIO A Initialization ************************/
   /* PORTA PIO Disable and Peripheral Enable*/
   ((pio_registers_t*)PIO_PORT_A)->PIO_PDR = ~0xFFDFFFFF;

   /* PORTA Pull Up Enable/Disable as per MHC selection */
   ((pio_registers_t*)PIO_PORT_A)->PIO_PUDR = ~0x00000800;
   ((pio_registers_t*)PIO_PORT_A)->PIO_PPDDR = 0x00000800;
   ((pio_registers_t*)PIO_PORT_A)->PIO_PUER = 0x00000800;

   /* PORTA Pull Down Enable/Disable as per MHC selection */
   ((pio_registers_t*)PIO_PORT_A)->PIO_PPDDR = ~0x00000000;

   /* PORTA Output Write Enable */
   ((pio_registers_t*)PIO_PORT_A)->PIO_OWER = PIO_OWER_Msk;

   /************************ PIO B Initialization ************************/
   /* PORTB Peripheral Function Selection */
   ((pio_registers_t*)PIO_PORT_B)->PIO_ABCDSR[0]= 0x00000010;
   ((pio_registers_t*)PIO_PORT_B)->PIO_ABCDSR[1]= 0x00000010;
   /* PORTB PIO Disable and Peripheral Enable*/
   ((pio_registers_t*)PIO_PORT_B)->PIO_PDR = ~0xFFFFFFEF;

   /* PORTB Pull Up Enable/Disable as per MHC selection */
   ((pio_registers_t*)PIO_PORT_B)->PIO_PUDR = ~0x00000000;

   /* PORTB Pull Down Enable/Disable as per MHC selection */
   ((pio_registers_t*)PIO_PORT_B)->PIO_PPDDR = ~0x00000000;

   /* PORTB Output Write Enable */
   ((pio_registers_t*)PIO_PORT_B)->PIO_OWER = PIO_OWER_Msk;

   /************************ PIO C Initialization ************************/
   /* PORTC Initial state High */
   ((pio_registers_t*)PIO_PORT_C)->PIO_SODR = 0x00000100;

   /* PORTC Pull Up Enable/Disable as per MHC selection */
   ((pio_registers_t*)PIO_PORT_C)->PIO_PUDR = ~0x00000000;

   /* PORTC Pull Down Enable/Disable as per MHC selection */
   ((pio_registers_t*)PIO_PORT_C)->PIO_PPDDR = ~0x00000000;

   /* PORTC Output Direction Enable */
   ((pio_registers_t*)PIO_PORT_C)->PIO_OER = 0x00000100;
   /* PORTC Output Write Enable */
   ((pio_registers_t*)PIO_PORT_C)->PIO_OWER = PIO_OWER_Msk;

   /************************ PIO D Initialization ************************/
   /* PORTD PIO Disable and Peripheral Enable*/
   ((pio_registers_t*)PIO_PORT_D)->PIO_PDR = ~0xFFFFFC00;

   /* PORTD Pull Up Enable/Disable as per MHC selection */
   ((pio_registers_t*)PIO_PORT_D)->PIO_PUDR = ~0x00000000;

   /* PORTD Pull Down Enable/Disable as per MHC selection */
   ((pio_registers_t*)PIO_PORT_D)->PIO_PPDDR = ~0x00000000;

   /* PORTD Output Write Enable */
   ((pio_registers_t*)PIO_PORT_D)->PIO_OWER = PIO_OWER_Msk;

   /************************ PIO E Initialization ************************/

   /* PORTE Pull Up Enable/Disable as per MHC selection */
   ((pio_registers_t*)PIO_PORT_E)->PIO_PUDR = ~0x00000000;

   /* PORTE Pull Down Enable/Disable as per MHC selection */
   ((pio_registers_t*)PIO_PORT_E)->PIO_PPDDR = ~0x00000000;

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
    uint32_t PIO_PortReadLatch ( PIO_PORT port )

  Summary:
    Read the latched value on all the I/O lines of the selected port.

  Remarks:
    See plib_pio.h for more details.
*/
uint32_t PIO_PortReadLatch(PIO_PORT port)
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

// *****************************************************************************
/* Function:
    void PIO_PortInterruptEnable(PIO_PORT port, uint32_t mask)

  Summary:
    Enables IO interrupt on selected IO pins of a port.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortInterruptEnable(PIO_PORT port, uint32_t mask)
{
    ((pio_registers_t*)port)->PIO_IER = mask;
}

// *****************************************************************************
/* Function:
    void PIO_PortInterruptDisable(PIO_PORT port, uint32_t mask)

  Summary:
    Disables IO interrupt on selected IO pins of a port.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortInterruptDisable(PIO_PORT port, uint32_t mask)
{
    ((pio_registers_t*)port)->PIO_IDR = mask;
}






/*******************************************************************************
 End of File
*/
