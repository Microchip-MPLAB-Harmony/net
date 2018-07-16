/*******************************************************************************
  TIME System Service Definitions Header File

  Company:
    Microchip Technology Inc.

  File Name:
    sys_time_definitions.h

  Summary:
    TIME System Service Definitions Header File

  Description:
    This file provides implementation-specific definitions for the TIME
    system service's system interface.
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

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS  WITHOUT  WARRANTY  OF  ANY  KIND,
EITHER EXPRESS  OR  IMPLIED,  INCLUDING  WITHOUT  LIMITATION,  ANY  WARRANTY  OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A  PARTICULAR  PURPOSE.
IN NO EVENT SHALL MICROCHIP OR  ITS  LICENSORS  BE  LIABLE  OR  OBLIGATED  UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,  BREACH  OF  WARRANTY,  OR
OTHER LEGAL  EQUITABLE  THEORY  ANY  DIRECT  OR  INDIRECT  DAMAGES  OR  EXPENSES
INCLUDING BUT NOT LIMITED TO ANY  INCIDENTAL,  SPECIAL,  INDIRECT,  PUNITIVE  OR
CONSEQUENTIAL DAMAGES, LOST  PROFITS  OR  LOST  DATA,  COST  OF  PROCUREMENT  OF
SUBSTITUTE  GOODS,  TECHNOLOGY,  SERVICES,  OR  ANY  CLAIMS  BY  THIRD   PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE  THEREOF),  OR  OTHER  SIMILAR  COSTS.
*******************************************************************************/
//DOM-IGNORE-END

#ifndef SYS_TIME_DEFINITIONS_H
#define SYS_TIME_DEFINITIONS_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include "system/int/sys_int.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* TIME PLIB API Set needed by the system service */

typedef void(*TIME_CallbackSet)(void * callback, uintptr_t context);
typedef void(*TIME_PeriodSet)(uint32_t period);
typedef void(*TIME_Start)(void);
typedef void(*TIME_Stop)(void);
typedef uint32_t(*TIME_CounterGet)(void);

typedef struct
{
    TIME_CallbackSet timerCallbackSet;
    TIME_PeriodSet timerPeriodSet;
    TIME_Start timerStart;
    TIME_Stop timerStop;
    TIME_CounterGet timerCounterGet;

} TIME_PLIB_API;


// *****************************************************************************
/* TIME system service Initialization Data Declaration */

struct _SYS_TIME_INIT
{
    /* Identifies the PLIB API set to be used by the system service to access
     * the peripheral. */
    TIME_PLIB_API *timePlib;

    /* Interrupt source ID for the TIMER interrupt. */
    INT_SOURCE timeInterrupt;

    /* Hardware timer frequency */
    uint32_t timeFrequency;

};


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END


#endif // #ifndef SYS_TIME_DEFINITIONS_H

