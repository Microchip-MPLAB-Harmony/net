<#--
/*******************************************************************************
  TCP/IP Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
   system_config.h.ftl

  Summary:
    TCP/IP Freemarker Template File

  Description:

*******************************************************************************/
-->

<#--
Copyright (C) 2018-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
-->


<#if (HarmonyCore.SELECT_RTOS)?? && HarmonyCore.SELECT_RTOS != "BareMetal">
    <#lt>/* TCP/IP RTOS Configurations*/
    <#if (HarmonyCore.SELECT_RTOS)?? && HarmonyCore.SELECT_RTOS == "FreeRTOS">
        <#lt>#define TCPIP_RTOS_STACK_SIZE                ${TCPIP_RTOS_TASK_STACK_SIZE / 4}
    <#else>
        <#lt>#define TCPIP_RTOS_STACK_SIZE                ${TCPIP_RTOS_TASK_STACK_SIZE}
    </#if>
    <#lt>#define TCPIP_RTOS_PRIORITY             ${TCPIP_RTOS_TASK_PRIORITY}
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
