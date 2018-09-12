/*******************************************************************************
  Interrupt System Service Mapping File

  Company:
    Microchip Technology Inc.

  File Name:
    sys_int_mapping.h

  Summary:
    Interrupt System Service mapping file.

  Description:
    This header file contains the mapping of the APIs defined in the API header
    to either the function implementations or macro implementation or the
    specific variant implementation.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED 'AS IS' WITHOUT WARRANTY OF ANY KIND,
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

#ifndef SYS_INT_MAPPING_H
#define SYS_INT_MAPPING_H

// *****************************************************************************
// *****************************************************************************
// Section: Interrupt System Service Mapping
// *****************************************************************************
// *****************************************************************************
#define SYS_INT_IsEnabled( ) ( __get_PRIMASK() == 0 )

#define SYS_INT_SourceEnable(source)  NVIC_EnableIRQ(source)

#define SYS_INT_SourceIsEnabled(source)  NVIC_GetEnableIRQ(source)

#define SYS_INT_SourceStatusGet(source)  NVIC_GetPendingIRQ(source)

#define SYS_INT_SourceStatusSet(source)  NVIC_SetPendingIRQ(source)

#define SYS_INT_SourceStatusClear(source)  NVIC_ClearPendingIRQ(source)


#endif // SYS_INT_MAPPING_PIC32_H

/*******************************************************************************
 End of File
*/
