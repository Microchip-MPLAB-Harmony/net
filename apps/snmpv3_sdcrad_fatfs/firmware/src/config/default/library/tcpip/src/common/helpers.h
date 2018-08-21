/*******************************************************************************
  Header file for common MCHP helpers

  Summary:
    SUMMARY
    
  Description:
    DESCRIPTION
*******************************************************************************/

/*******************************************************************************
File Name: helpers.h 
Copyright © 2012 released Microchip Technology Inc.  All rights
reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
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

#ifndef __HELPERS_H_
#define __HELPERS_H_

#include <stdint.h>
#include <stdbool.h>


void        uitoa(uint16_t Value, uint8_t* Buffer);

uint8_t     hexatob(uint16_t AsciiVal);
uint8_t     btohexa_high(uint8_t b);
uint8_t     btohexa_low(uint8_t b);

size_t      strncpy_m(char* destStr, size_t destSize, int nStrings, ...);

#endif  // __HELPERS_H_

