/*******************************************************************************
  USB stack external dependencies file

  Company:
    Microchip Technology Inc.

  File Name:
    usb_external_dependencies.h

  Summary:
    USB stack external dependencies file

  Description:
    USB stack external dependencies file. 
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2012 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _USB_EXTERNAL_DEPENDENCIES_H
#define _USB_EXTERNAL_DEPENDENCIES_H

#include <string.h>
#include "system/system_common.h"
#include "configuration.h"
#include "system/system_module.h"

#if defined (USB_HOST_DEVICES_NUMBER) &&  (USB_HOST_DEVICES_NUMBER > 0)
#include "system/time/sys_time.h"
#define SYS_TMR_HANDLE_INVALID SYS_TIME_HANDLE_INVALID
#define SYS_TMR_CallbackSingle(delay,context,callback) SYS_TIME_CallbackRegisterMS(callback,context,delay, SYS_TIME_SINGLE)
#endif 

#define SYS_DEBUG_PRINT(level, format, ...) 
#define SYS_DEBUG_MESSAGE(a,b, ...)
#define SYS_DEBUG(a,b)
//#define SYS_DEBUG_MESSAGE(a , b)
#endif 
/*******************************************************************************
 End of File
*/