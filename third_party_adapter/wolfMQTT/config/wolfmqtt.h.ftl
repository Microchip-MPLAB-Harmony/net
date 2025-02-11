<#--
/*******************************************************************************
  WolfMQTT Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    wolfmqtt.h.ftl

  Summary:
    WolfMQTT Freemarker Template File

  Description:

*******************************************************************************/
-->

<#--
Copyright (C) 2019-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

/*** wolfMQTT configuration ***/
#define WOLFMQTT_NONBLOCK
#define WOLFMQTT_USER_SETTINGS
// #define WOLFMQTT_NO_TIMEOUT
// #define WOLFMQTT_NO_STDIN_CAP

<#if WOLFMQTT_DISCONNECT_CB == true>
#define WOLFMQTT_DISCONNECT_CB
</#if>
<#if WOLFMQTT_NO_ERROR_STRINGS == true>
#define WOLFMQTT_NO_ERROR_STRINGS
</#if>
<#if WOLFMQTT_SN == true>
#define WOLFMQTT_SN
</#if>
<#if WOLFMQTT_V5 == true>
#define WOLFMQTT_V5
<#if WOLFMQTT_PROPERTY_CB == true>
#define WOLFMQTT_PROPERTY_CB
</#if>
</#if>
<#if WOLFMQTT_NO_STDIO == true>
#define WOLFMQTT_NO_STDIO
</#if>
<#if WOLFMQTT_NO_STDIO == false && WOLFMQTT_DEBUG_CLIENT == true>
#define WOLFMQTT_DEBUG_CLIENT
</#if>
<#if WOLFMQTT_NO_STDIO == false && WOLFMQTT_DEBUG_SOCKET == true>
#define WOLFMQTT_DEBUG_SOCKET
</#if>

<#if WOLFMQTT_TLS_ENABLE == true>
#define ENABLE_MQTT_TLS
<#else>
// #define ENABLE_MQTT_TLS
</#if>

/*** wolMQTT Net Glue configuration ***/
#define WMQTT_NET_GLUE_FORCE_TLS			${WMQTT_NET_GLUE_FORCE_TLS?c}
#define WMQTT_NET_GLUE_IPV6					${WMQTT_NET_GLUE_IPV6?c}
#define WMQTT_NET_GLUE_MAX_BROKER_NAME		${WMQTT_NET_GLUE_MAX_BROKER_NAME}
#define WMQTT_NET_GLUE_DEBUG_ENABLE			${WMQTT_NET_GLUE_DEBUG_ENABLE?c}
#define WMQTT_NET_GLUE_ERROR_STRINGS		${WMQTT_NET_GLUE_ERROR_STRINGS?c}
#define WMQTT_NET_GLUE_MALLOC				${WMQTT_NET_GLUE_MALLOC}
#define WMQTT_NET_GLUE_FREE					${WMQTT_NET_GLUE_FREE}
#define WMQTT_NET_SKT_TX_BUFF				${WMQTT_NET_SKT_TX_BUFF}
#define WMQTT_NET_SKT_RX_BUFF				${WMQTT_NET_SKT_RX_BUFF}




<#--
/*******************************************************************************
 End of File
*/
-->
