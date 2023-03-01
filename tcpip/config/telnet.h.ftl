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

<#if TCPIP_TELNET_AUTH_CONFIG == "Run Time Authentication">
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
