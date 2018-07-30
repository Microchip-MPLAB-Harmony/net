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
#define DRV_NVM_MEDIA_START_ADDRESS         DRV_MEMORY_DEVICE_START_ADDRESS
#define DRV_NVM_MEDIA_SIZE                  DRV_MEMORY_DEVICE_MEDIA_SIZE

#define APP_LED_1 0
#define APP_LED_2 1
#define APP_LED_3 2

#define APP_SWITCH_1StateGet()      SWITCH_AL_PA11_Get()
#define APP_SWITCH_2StateGet()      SWITCH_AL_PA11_Get()
#define APP_SWITCH_3StateGet()      SWITCH_AL_PA11_Get()


#define BSP_LEDStateSet(x,y)            PIO_PinWrite(PIO_PIN_PC8,y)
#define BSP_LEDStateGet(APP_LED_1)      PIO_PinReadLatch(PIO_PIN_PC8)
#define BSP_LEDToggle(APP_LED_1)        LED_AL_PC8_Toggle()
#endif // SYSTEM_CONFIG_H
/*******************************************************************************
 End of File
*/

