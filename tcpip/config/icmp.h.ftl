<#--
/*******************************************************************************
  ICMP Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    icmp.h.ftl

  Summary:
    ICMP Freemarker Template File

  Description:

*******************************************************************************/
-->

<#--
Copyright (C) 2014-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

<#if TCPIP_STACK_USE_ICMPV4 == true>
    <#if TCPIP_STACK_USE_ICMP_SERVER == true>
        <#lt>/*** ICMPv4 Server Configuration ***/
        <#lt>#define TCPIP_STACK_USE_ICMP_SERVER
        <#if TCPIP_ICMP_ECHO_BROADCASTS == true>
            <#lt>#define TCPIP_ICMP_ECHO_ALLOW_BROADCASTS    true
        <#else>
            <#lt>#define TCPIP_ICMP_ECHO_ALLOW_BROADCASTS    false
        </#if>
    </#if>

    <#if TCPIP_STACK_USE_ICMP_CLIENT == true>
        <#lt>/*** ICMPv4 Client Configuration ***/
        <#lt>#define TCPIP_STACK_USE_ICMP_CLIENT
        <#lt>#define TCPIP_ICMP_ECHO_REQUEST_TIMEOUT        ${TCPIP_ICMP_ECHO_REQUEST_TIMEOUT}
        <#lt>#define TCPIP_ICMP_TASK_TICK_RATE              ${TCPIP_ICMP_TASK_TICK_RATE}
        <#lt>#define TCPIP_STACK_MAX_CLIENT_ECHO_REQUESTS   ${TCPIP_STACK_MAX_CLIENT_ECHO_REQUESTS}
        <#lt>#define TCPIP_ICMP_COMMAND_ENABLE              ${TCPIP_ICMP_COMMAND_ENABLE?c}
        <#if TCPIP_ICMP_COMMAND_ENABLE == true>
            <#lt>#define TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUESTS         ${TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUESTS}
            <#lt>#define TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DELAY    ${TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DELAY}
            <#lt>#define TCPIP_STACK_COMMANDS_ICMP_ECHO_TIMEOUT          ${TCPIP_STACK_COMMANDS_ICMP_ECHO_TIMEOUT}
            <#lt>#define TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_BUFF_SIZE    ${TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_BUFF_SIZE}
            <#lt>#define TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DATA_SIZE    ${TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DATA_SIZE}
        </#if>
    </#if>
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
