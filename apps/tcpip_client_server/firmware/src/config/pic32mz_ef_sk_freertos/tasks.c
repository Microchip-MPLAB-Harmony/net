/*******************************************************************************
 System Tasks File

  File Name:
    tasks.c

  Summary:
    This file contains source code necessary to maintain system's polled tasks.

  Description:
    This file contains source code necessary to maintain system's polled tasks.
    It implements the "SYS_Tasks" function that calls the individual "Tasks"
    functions for all polled MPLAB Harmony modules in the system.

  Remarks:
    This file requires access to the systemObjects global data structure that
    contains the object handles to all MPLAB Harmony module objects executing
    polled in the system.  These handles are passed into the individual module
    "Tasks" functions to identify the instance of the module to maintain.
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"
#include "definitions.h"


// *****************************************************************************
// *****************************************************************************
// Section: RTOS "Tasks" Routine
// *****************************************************************************
// *****************************************************************************

void _TCPIP_STACK_Task(  void *pvParameters  )
{
    while(1)
    {
        TCPIP_STACK_Task(sysObj.tcpip);
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

/* Handle for the APP_Tasks. */
TaskHandle_t xAPP_Tasks;

void _APP_Tasks(  void *pvParameters  )
{
    while(1)
    {
        APP_Tasks();
        vTaskDelay(2 / portTICK_PERIOD_MS);
    }
}
/* Handle for the APP1_Tasks. */
TaskHandle_t xAPP1_Tasks;

void _APP1_Tasks(  void *pvParameters  )
{
    while(1)
    {
        APP1_Tasks();
        vTaskDelay(2 / portTICK_PERIOD_MS);
    }
}
/* Handle for the APP2_Tasks. */
TaskHandle_t xAPP2_Tasks;

void _APP2_Tasks(  void *pvParameters  )
{
    while(1)
    {
        APP2_Tasks();
        vTaskDelay(2 / portTICK_PERIOD_MS);
    }
}
/* Handle for the APP3_Tasks. */
TaskHandle_t xAPP3_Tasks;

void _APP3_Tasks(  void *pvParameters  )
{
    while(1)
    {
        APP3_Tasks();
        vTaskDelay(2 / portTICK_PERIOD_MS);
    }
}
/* Handle for the APP4_Tasks. */
TaskHandle_t xAPP4_Tasks;

void _APP4_Tasks(  void *pvParameters  )
{
    while(1)
    {
        APP4_Tasks();
        vTaskDelay(2 / portTICK_PERIOD_MS);
    }
}

void _SYS_CMD_Tasks(  void *pvParameters  )
{
    while(1)
    {
        SYS_CMD_Tasks();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}



void _DRV_MIIM_Task(  void *pvParameters  )
{
    while(1)
    {
        DRV_MIIM_Tasks(sysObj.drvMiim);
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}




// *****************************************************************************
// *****************************************************************************
// Section: System "Tasks" Routine
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void SYS_Tasks ( void )

  Remarks:
    See prototype in system/common/sys_module.h.
*/
void SYS_Tasks ( void )
{
    /* Maintain system services */
    

    xTaskCreate( _SYS_CMD_Tasks,
        "SYS_CMD_TASKS",
        SYS_CMD_RTOS_STACK_SIZE,
        (void*)NULL,
        SYS_CMD_RTOS_TASK_PRIORITY,
        (TaskHandle_t*)NULL
    );




    /* Maintain Device Drivers */
        xTaskCreate( _DRV_MIIM_Task,
        "DRV_MIIM_Tasks",
        DRV_MIIM_RTOS_STACK_SIZE,
        (void*)NULL,
        DRV_MIIM_RTOS_TASK_PRIORITY,
        (TaskHandle_t*)NULL
    );




    /* Maintain Middleware & Other Libraries */
    
    xTaskCreate( _TCPIP_STACK_Task,
        "TCPIP_STACK_Tasks",
        TCPIP_RTOS_STACK_SIZE,
        (void*)NULL,
        TCPIP_RTOS_PRIORITY,
        (TaskHandle_t*)NULL
    );




    /* Maintain the application's state machine. */
        /* Create OS Thread for APP_Tasks. */
    xTaskCreate((TaskFunction_t) _APP_Tasks,
                "APP_Tasks",
                1024,
                NULL,
                1,
                &xAPP_Tasks);

    /* Create OS Thread for APP1_Tasks. */
    xTaskCreate((TaskFunction_t) _APP1_Tasks,
                "APP1_Tasks",
                1024,
                NULL,
                1,
                &xAPP1_Tasks);

    /* Create OS Thread for APP2_Tasks. */
    xTaskCreate((TaskFunction_t) _APP2_Tasks,
                "APP2_Tasks",
                1024,
                NULL,
                1,
                &xAPP2_Tasks);

    /* Create OS Thread for APP3_Tasks. */
    xTaskCreate((TaskFunction_t) _APP3_Tasks,
                "APP3_Tasks",
                1024,
                NULL,
                1,
                &xAPP3_Tasks);

    /* Create OS Thread for APP4_Tasks. */
    xTaskCreate((TaskFunction_t) _APP4_Tasks,
                "APP4_Tasks",
                1024,
                NULL,
                1,
                &xAPP4_Tasks);




    /* Start RTOS Scheduler. */
    
     /**********************************************************************
     * Create all Threads for APP Tasks before starting FreeRTOS Scheduler *
     ***********************************************************************/
    vTaskStartScheduler(); /* This function never returns. */

}

/*******************************************************************************
 End of File
 */

