/*******************************************************************************
 System Tasks File

  File Name:
    freertos_hooks.c

  Summary:
    This file contains source code necessary for FreeRTOS hooks

  Description:

  Remarks:
 *******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END
#include "FreeRTOS.h"
#include "task.h"

/*
*********************************************************************************************************
*                                          vApplicationStackOverflowHook()
*
* Description : Hook function called by FreeRTOS if a stack overflow happens.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : APP_StateReset()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void vApplicationStackOverflowHook( TaskHandle_t pxTask, signed char *pcTaskName )
{
   ( void ) pcTaskName;
   ( void ) pxTask;

   /* Run time task stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook	function is
   called if a task stack overflow is detected.  Note the system/interrupt
   stack is not checked. */
   taskDISABLE_INTERRUPTS();
   for( ;; );
}

/*
*********************************************************************************************************
*                                     vApplicationMallocFailedHook()
*
* Description : vApplicationMallocFailedHook() will only be called if
*               configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.
*               It is a hook function that will get called if a call to
*               pvPortMalloc() fails.  pvPortMalloc() is called internally by
*               the kernel whenever a task, queue, timer or semaphore is
*               created.  It is also called by various parts of the demo
*               application.  If heap_1.c or heap_2.c are used, then the size of
*               the heap available to pvPortMalloc() is defined by
*               configTOTAL_HEAP_SIZE in FreeRTOSConfig.h, and the
*               xPortGetFreeHeapSize() API function can be used to query the
*               size of free heap space that remains (although it does not
*               provide information on how the remaining heap might be
*               fragmented).
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : APP_StateReset()
*
* Note(s)     : none.
*********************************************************************************************************
*/

void vApplicationMallocFailedHook( void )
{
   /* vApplicationMallocFailedHook() will only be called if
      configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
      function that will get called if a call to pvPortMalloc() fails.
      pvPortMalloc() is called internally by the kernel whenever a task, queue,
      timer or semaphore is created.  It is also called by various parts of the
      demo application.  If heap_1.c or heap_2.c are used, then the size of the
      heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
      FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
      to query the size of free heap space that remains (although it does not
      provide information on how the remaining heap might be fragmented). */
   taskDISABLE_INTERRUPTS();
   for( ;; );
}

/*******************************************************************************
 End of File
 */

 
