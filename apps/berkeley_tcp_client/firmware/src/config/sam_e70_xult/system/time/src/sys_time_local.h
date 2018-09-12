/*******************************************************************************
  Time System Service Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    sys_time_local.h

  Summary:
    Time System Service local declarations and definitions.

  Description:
    This file contains the Time System Service local declarations and definitions.
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


#ifndef SYS_TIME_LOCAL_H
#define SYS_TIME_LOCAL_H


// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include "system/time/sys_time.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

//////////////////////////Counter//////////////////////////////////////////////
#define HW_COUNTER_ROLLOVER_CYCLE    1
#define HW_COUNTER_WIDTH             16
#define HW_COUNTER_MAX               0xFFFF
#define COUNTER_MAX                  0xFFFFFFFF
#define COUNTER_WIDTH                32

// *****************************************************************************
/* Timer Handle Macros

  Summary:
    Timer Handle Macros

  Description:
    Timer handle related utility macros. SYS TIME timer handle is a
    combination of timer token and the timer object index. The token
    is a 16 bit number that is incremented for every new timer request
    and is used along with the timer object index to generate a new timer
    handle for every request.

  Remarks:
    None
*/

#define _SYS_TIME_HANDLE_TOKEN_MAX         (0xFFFF)
#define _SYS_TIME_MAKE_HANDLE(token, index) ((token) << 16 | (index))
#define _SYS_TIME_UPDATE_HANDLE_TOKEN(token) \
{ \
    (token)++; \
    if ((token) >= _SYS_TIME_HANDLE_TOKEN_MAX) \
        (token) = 0; \
    else \
        (token) = (token); \
}

// *****************************************************************************
/* SYS TIME OBJECT INSTANCE structure

  Summary:
    System Time object instance structure.

  Description:
    This data type defines the System Time object instance.

  Remarks:
    None.
*/

typedef struct _SYS_TIME_TIMER_OBJ{
  int inuse;    /* TRUE if in use */
  int active;    /* TRUE if soft timer enabled */
  SYS_TIME_CALLBACK_TYPE type;    /* periodic or not */
  uint32_t time;    /* time requested */
  uint32_t timeRemaining;    /* time to wait, relative incase of timers in the list */
  SYS_TIME_CALLBACK callback;    /* set to TRUE at timeout */
  uintptr_t context; /* context */
  int tmrElapsed;   /* Useful only for single shot timer */
  struct _SYS_TIME_TIMER_OBJ * tmrNext; /* Next timer */
  SYS_TIME_HANDLE tmrHandle; /* Unique handle for object */
} SYS_TIME_TIMER_OBJ;


typedef struct{
    SYS_STATUS status;
    TIME_PLIB_API *timePlib;
    INT_SOURCE timeInterrupt;
    uint32_t timeFrequency;
    uint32_t timePeriodPrevious;
    uint32_t timePeriod;
    volatile uint32_t counter;    /* Software counter */
    volatile uint32_t highCounter;    /* Software 64-bit counter */
    volatile uint32_t timeToCounter;     /* Active timer expiry point or software period */
    int tmrElapsed;    /* On every active timer elapsed */
    int interruptContext;    /* On every active timer elapsed */
    SYS_TIME_TIMER_OBJ * tmrActive;

} SYS_TIME_COUNTER_OBJ;   /* set of timers */


#endif //#ifndef SYS_TIME_LOCAL_H

/*******************************************************************************
 End of File
*/

