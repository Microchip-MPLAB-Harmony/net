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
typedef uint32_t(*TIME_FrequencyGet)(void);
typedef uint32_t(*TIME_PeriodGet)(void);
typedef void (*TIME_CompareSet) (uint32_t compare);
typedef uint32_t(*TIME_CompareGet)(void);
typedef void(*TIME_Start)(void);
typedef void(*TIME_Stop)(void);
typedef uint32_t(*TIME_CounterGet)(void);

typedef struct
{
    TIME_CallbackSet timerCallbackSet;
    TIME_PeriodSet timerPeriodSet;
    TIME_FrequencyGet timerFrequencyGet;
    TIME_CompareSet timerCompareSet;
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
    INT_SOURCE hwTimerIntNum;

};


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END


#endif // #ifndef SYS_TIME_DEFINITIONS_H

