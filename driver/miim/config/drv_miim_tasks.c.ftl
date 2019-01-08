<#----------------------------------------------------------------------------
 Copyright (C) 2016-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
----------------------------------------------------------------------------->

<#if DRV_MIIM_USE_DRIVER == true>
<#if (USE_3RDPARTY_RTOS)?has_content && USE_3RDPARTY_RTOS == true>
<#if (DRV_MIIM_TASKS == "PROTO") && (DRV_MIIM_RTOS == "Standalone")>
<#if THIRDPARTY_RTOS_USED == "ThreadX">
void _DRV_MIIM_Tasks(ULONG thread_input);
<#else>
void _DRV_MIIM_Tasks(void);
</#if>
</#if>
<#if DRV_MIIM_TASKS == "CREATE_TASK">
 <#if DRV_MIIM_RTOS == "Standalone">
    /* Create task for MIIM state machine*/
<@RTOS_TASK_CREATE RTOS_NAME=THIRDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_MIIM_Tasks"
TASK_NAME="DRV_MIIM Tasks" TASK_PRI=DRV_MIIM_RTOS_TASK_PRIORITY
TASK_STK_SZ=DRV_MIIM_RTOS_STACK_SIZE/>
  </#if>
</#if>
<#if (DRV_MIIM_TASKS == "TASK_CALL_NO_RTOS") || (DRV_MIIM_TASKS == "TASK_CALL" && DRV_MIIM_RTOS != "Standalone")>
    DRV_MIIM_Tasks (sysObj.drvMiim);
</#if>
<#if DRV_MIIM_TASKS == "LOCAL_FUNCTION">
<#if DRV_MIIM_RTOS == "Standalone">
<#if THIRDPARTY_RTOS_USED == "ThreadX">
void _DRV_MIIM_Tasks(ULONG thread_input)
<#else>
void _DRV_MIIM_Tasks(void)
</#if>
 {
<#if THIRDPARTY_RTOS_USED == "uC/OS-III">
    OS_ERR os_err;
	
</#if> 
    while(1)
    {
        DRV_MIIM_Tasks (sysObj.drvMiim);
        <@RTOS_TASK_DELAY RTOS_NAME=THIRDPARTY_RTOS_USED TASK_DELAY=DRV_MIIM_RTOS_DELAY/>		
    }
 }
</#if>
</#if>
<#else>
    DRV_MIIM_Tasks (sysObj.drvMiim);
</#if>
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
