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

<#----------------------------------------------------------------------------
 Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.

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

/*** wolfMQTT configuration ***/
#define WOLFMQTT_NONBLOCK
// #define ENABLE_MQTT_TLS
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
