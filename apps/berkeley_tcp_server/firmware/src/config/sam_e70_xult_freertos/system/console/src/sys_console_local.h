/*******************************************************************************
  Console System Service Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    sys_console_local.h

  Summary:
    Console System Service local declarations and definitions.

  Description:
    This file contains the Console System Service local declarations and definitions.
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


#ifndef SYS_CONSOLE_LOCAL_H
#define SYS_CONSOLE_LOCAL_H


// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h> /* ssize_t */

#include "configuration.h"
#include "system/system.h"
#include "driver/driver.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

typedef uintptr_t CONSOLE_DEVICE_HANDLE;

typedef void (*consoleCallbackFunction) (void *handle);

typedef enum
{
    SYS_CONSOLE_STATUS_NOT_CONFIGURED,

    SYS_CONSOLE_STATUS_CONFIGURED,

    SYS_CONSOLE_STATUS_BUSY,

    SYS_CONSOLE_STATUS_ERROR

} SYS_CONSOLE_STATUS;


typedef enum
{
    SYS_CONSOLE_EVENT_WRITE_COMPLETE,

    SYS_CONSOLE_EVENT_READ_COMPLETE,

} SYS_CONSOLE_EVENT;


// *****************************************************************************
// *****************************************************************************
// Section: Extern data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Prototypes
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: UART Prototypes
// *****************************************************************************
// *****************************************************************************
CONSOLE_DEVICE_HANDLE Console_UART_Open(const SYS_MODULE_INDEX index, DRV_IO_INTENT intent);
void Console_UART_Close(CONSOLE_DEVICE_HANDLE consDevHandle);
ssize_t Console_UART_Write(int fd, const void *buf, size_t count);
ssize_t Console_UART_Read(int fd, void *buf, size_t count);
char Console_UART_ReadC(int fd);
void Console_UART_Tasks (SYS_MODULE_OBJ consObj);
SYS_CONSOLE_STATUS Console_UART_Status (void);
void Console_UART_RegisterCallback(consoleCallbackFunction cbFunc, SYS_CONSOLE_EVENT event);
void Console_UART_Flush(void);

#endif //#ifndef SYS_CONSOLE_LOCAL_H

/*******************************************************************************
 End of File
*/

