<#--
/*******************************************************************************
  telnet Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    telnet.h.ftl

  Summary:
    telnet Freemarker Template File

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

<#if TCPIP_USE_TELNET == true>
/*** telnet Configuration ***/
#define TCPIP_STACK_USE_TELNET_SERVER
#define TCPIP_TELNET_MAX_CONNECTIONS    ${TCPIP_TELNET_MAX_CONNECTIONS}
#define TCPIP_TELNET_TASK_TICK_RATE     ${TCPIP_TELNET_TASK_TICK_RATE}
#define TCPIP_TELNET_SKT_TX_BUFF_SIZE   ${TCPIP_TELNET_SKT_TX_BUFF_SIZE}
#define TCPIP_TELNET_SKT_RX_BUFF_SIZE   ${TCPIP_TELNET_SKT_RX_BUFF_SIZE}
#define TCPIP_TELNET_LISTEN_PORT        ${TCPIP_TELNET_LISTEN_PORT}
#define TCPIP_TELNET_PRINT_BUFF_SIZE    ${TCPIP_TELNET_PRINT_BUFF_SIZE}
#define TCPIP_TELNET_LINE_BUFF_SIZE     ${TCPIP_TELNET_LINE_BUFF_SIZE}
#define TCPIP_TELNET_USERNAME_SIZE      ${TCPIP_TELNET_USERNAME_SIZE}
#define TCPIP_TELNET_CONFIG_FLAGS       \
<#if TCPIP_TELNET_CONFIG_FLAG_NO_DELAY>
                                        TCPIP_TELNET_FLAG_NO_DELAY |\
</#if>
<#if TCPIP_TELNET_CONFIG_PASS_CONTROL_CHARS>
                                       TCPIP_TELNET_FLAG_PASS_CONTROL_CHARS |\
</#if>
                                       TCPIP_TELNET_FLAG_NONE
<#if TCPIP_TELNET_RUN_AUTHENTICATION>
#define TCPIP_TELNET_OBSOLETE_AUTHENTICATION false
<#if TCPIP_TELNET_AUTHENTICATION_CONN_INFO>
#define TCPIP_TELNET_AUTHENTICATION_CONN_INFO true
<#else>
#define TCPIP_TELNET_AUTHENTICATION_CONN_INFO false
</#if>
<#else>
#define TCPIP_TELNET_OBSOLETE_AUTHENTICATION true
#define TCPIP_TELNET_USERNAME           "${TCPIP_TELNET_USERNAME}"
#define TCPIP_TELNET_PASSWORD           "${TCPIP_TELNET_PASSWORD}"
</#if>
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
