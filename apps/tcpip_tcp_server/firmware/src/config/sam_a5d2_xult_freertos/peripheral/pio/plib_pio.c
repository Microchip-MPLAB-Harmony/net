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
 /* Port B Peripheral function F configuration */
	PIOB_REGS->PIO_MSKR = 0xffc000;
	PIOB_REGS->PIO_CFGR = 0x6;
	
 /* Port B Pin 0 configuration */
	PIOB_REGS->PIO_MSKR = 0x1;
	PIOB_REGS->PIO_CFGR |= 0x500;
	
 /* Port B Pin 5 configuration */
	PIOB_REGS->PIO_MSKR = 0x20;
	PIOB_REGS->PIO_CFGR |= 0x500;
	
 /* Port B Pin 6 configuration */
	PIOB_REGS->PIO_MSKR = 0x40;
	PIOB_REGS->PIO_CFGR |= 0x500;
	
 /* Port B Pin 9 configuration */
	PIOB_REGS->PIO_MSKR = 0x200;
	PIOB_REGS->PIO_CFGR |= 0x200;
	
 /* Port B Pin 15 configuration */
	PIOB_REGS->PIO_MSKR = 0x8000;
	PIOB_REGS->PIO_CFGR |= 0x400;
	
 /* Port B Pin 16 configuration */
	PIOB_REGS->PIO_MSKR = 0x10000;
	PIOB_REGS->PIO_CFGR |= 0x400;
	
 /* Port B Pin 17 configuration */
	PIOB_REGS->PIO_MSKR = 0x20000;
	PIOB_REGS->PIO_CFGR |= 0x400;
	
 /* Port B Pin 18 configuration */
	PIOB_REGS->PIO_MSKR = 0x40000;
	PIOB_REGS->PIO_CFGR |= 0x400;
	
 /* Port B Pin 19 configuration */
	PIOB_REGS->PIO_MSKR = 0x80000;
	PIOB_REGS->PIO_CFGR |= 0x400;
	
 /* Port B Pin 20 configuration */
	PIOB_REGS->PIO_MSKR = 0x100000;
	PIOB_REGS->PIO_CFGR |= 0x400;
	
 /* Port B Pin 21 configuration */
	PIOB_REGS->PIO_MSKR = 0x200000;
	PIOB_REGS->PIO_CFGR |= 0x400;
	
 /* Port B Pin 22 configuration */
	PIOB_REGS->PIO_MSKR = 0x400000;
	PIOB_REGS->PIO_CFGR |= 0x400;
	
 /* Port B Pin 23 configuration */
	PIOB_REGS->PIO_MSKR = 0x800000;
	PIOB_REGS->PIO_CFGR |= 0x400;
	
 /* Port B Latch configuration */
	PIOB_REGS->PIO_SODR = 0x61;
	
 /* Port D Pin 2 configuration */
	PIOD_REGS->PIO_MSKR = 0x4;
	PIOD_REGS->PIO_CFGR |= 0x200;
	
 /* Port D Pin 3 configuration */
	PIOD_REGS->PIO_MSKR = 0x8;
	PIOD_REGS->PIO_CFGR |= 0x200;
	
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
    ((pio_registers_t*)port)->PIO_MSKR = mask;
    ((pio_registers_t*)port)->PIO_ODSR = value;
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
    ((pio_registers_t*)port)->PIO_MSKR = mask;
    ((pio_registers_t*)port)->PIO_ODSR ^= mask;
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
    ((pio_registers_t*)port)->PIO_MSKR = mask;
    ((pio_registers_t*)port)->PIO_CFGR &= ~(1 << PIO_CFGR_DIR_Pos);	
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
    ((pio_registers_t*)port)->PIO_MSKR = mask;
    ((pio_registers_t*)port)->PIO_CFGR |= (1 << PIO_CFGR_DIR_Pos);
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
