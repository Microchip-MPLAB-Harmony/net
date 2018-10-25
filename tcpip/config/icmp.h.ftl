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

<#if TCPIP_STACK_USE_ICMPV4 == true>
<#if TCPIP_STACK_USE_ICMP_SERVER == true>
/*** ICMPv4 Server Configuration ***/
#define TCPIP_STACK_USE_ICMP_SERVER
<#if TCPIP_ICMP_ECHO_BROADCASTS == true>
#define TCPIP_ICMP_ECHO_ALLOW_BROADCASTS    true
<#else>
#define TCPIP_ICMP_ECHO_ALLOW_BROADCASTS    false
</#if>
</#if>

<#if TCPIP_STACK_USE_ICMP_CLIENT == true>
/*** ICMPv4 Client Configuration ***/
#define TCPIP_STACK_USE_ICMP_CLIENT
<#if TCPIP_ICMP_CLIENT_USER_NOTIFICATION == true>
#define TCPIP_ICMP_CLIENT_USER_NOTIFICATION   true
<#else>
#define TCPIP_ICMP_CLIENT_USER_NOTIFICATION   false
</#if>
#define TCPIP_ICMP_ECHO_REQUEST_TIMEOUT       ${TCPIP_ICMP_ECHO_REQUEST_TIMEOUT}
#define TCPIP_ICMP_TASK_TICK_RATE             ${TCPIP_ICMP_TASK_TICK_RATE}
</#if>
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
