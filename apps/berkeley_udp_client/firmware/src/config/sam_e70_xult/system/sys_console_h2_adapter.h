/*******************************************************************************
  Adapter Console System Service Library Interface Header File

  Company
    Microchip Technology Inc.

  File Name
    sys_console_h2_adapter.h

  Summary
    Console system service library interface.

  Description
    This is the adapter file defines the interface to the Console system service 
    library from H2. The System Console service APIs are used to provide an adapter 
    routines which are part of H2.

  Remarks:
   
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

#ifndef SYS_CONSOLE_ADPATER_H    // Guards against multiple inclusion
#define SYS_CONSOLE_ADPATER_H


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


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END


#ifndef SYS_CONSOLE_MESSAGE
    #define SYS_CONSOLE_MESSAGE(message)
#endif

#ifndef SYS_CONSOLE_PRINT
    #define SYS_CONSOLE_PRINT(fmt, ...)
#endif

#ifndef SYS_DEBUG
    #define SYS_DEBUG(level,message)    
#endif

#ifndef SYS_ERROR
    #define SYS_ERROR(level,fmt, ...)   
#endif

#ifndef SYS_ERROR_PRINT
    #define SYS_ERROR_PRINT(level,fmt, ...)   
#endif
#endif //SYS_CONSOLE_ADPATER_H