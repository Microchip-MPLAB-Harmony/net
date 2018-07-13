/*******************************************************************************
  Driver Layer Interface Header

  Company:
    Microchip Technology Inc.

  File Name:
    driver.h

  Summary:
    Driver layer data types and definitions.

  Description:
    This file defines the common macros and definitions for the driver layer
    modules.

  Remarks:
    The parent directory to the "system" directory should be added to the
    compiler's search path for header files such that the following include
    statement will successfully include this file.

    #include "system/system.h"
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
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

#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"


/*** Crypto Library Configuration ***/
#define HAVE_MCAPI
#define MICROCHIP_PIC32C
#define NO_CERTS
#define NO_PWDBASED
#define NO_OLD_TLS
#define NO_SHA
#define NO_AES
#define NO_ASN
#define NO_RSA



#define DRV_NVM_MEDIA_START_ADDRESS         DRV_MEMORY_DEVICE_START_ADDRESS
#define DRV_NVM_MEDIA_SIZE                  DRV_MEMORY_DEVICE_MEDIA_SIZE


#ifndef SYS_DEBUG
    #define SYS_DEBUG(level,message) do { USART_MSG(message); } while (0)
	void USART_MSG(const char* str);	
#endif

#ifndef SYS_ERROR
    #define SYS_ERROR(level,fmt, ...)   
#endif

#ifndef SYS_ERROR_PRINT
    #define SYS_ERROR_PRINT(level,fmt, ...)   do { USART_PRINT(fmt, ##__VA_ARGS__); } while (0)
    int USART_PRINT(const char* format, ...);
#endif

#ifndef SYS_CONSOLE_MESSAGE
    #define SYS_CONSOLE_MESSAGE(message)   do { USART_MSG(message); } while (0)
    void USART_MSG(const char* str);
#endif

#ifndef SYS_CONSOLE_PRINT
    #define SYS_CONSOLE_PRINT(fmt, ...)  do { USART_PRINT(fmt, ##__VA_ARGS__); } while (0)
    int USART_PRINT(const char* format, ...);
#endif
#endif // SYSTEM_CONFIG_H
/*******************************************************************************
 End of File
*/

