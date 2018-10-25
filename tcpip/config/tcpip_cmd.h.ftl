<#--
/*******************************************************************************
  UDP Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    tcpip_cmd.h.ftl

  Summary:
    tcpip_cmd Freemarker Template File

  Description:

*******************************************************************************/
-->

<#----------------------------------------------------------------------------
 Copyright (C) 2014-2018 Microchip Technology Inc. and its subsidiaries.

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

<#if TCPIP_STACK_USE_COMMANDS == true>
/*** tcpip_cmd Configuration ***/
#define TCPIP_STACK_COMMAND_ENABLE
<#if TCPIP_STACK_COMMANDS_STORAGE_ENABLE == true>
#define TCPIP_STACK_COMMANDS_STORAGE_ENABLE
</#if>
#define TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUESTS         ${TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUESTS}
#define TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DELAY    ${TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DELAY}
#define TCPIP_STACK_COMMANDS_ICMP_ECHO_TIMEOUT          ${TCPIP_STACK_COMMANDS_ICMP_ECHO_TIMEOUT}
<#if TCPIP_STACK_COMMANDS_WIFI_ENABLE == true>
#define TCPIP_STACK_COMMANDS_WIFI_ENABLE             	true
<#else>
#define TCPIP_STACK_COMMANDS_WIFI_ENABLE             	false
</#if>
#define TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_BUFF_SIZE    ${TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_BUFF_SIZE}
#define TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DATA_SIZE    ${TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DATA_SIZE}
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
