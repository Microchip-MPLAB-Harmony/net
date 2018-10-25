<#--
/*******************************************************************************
  DHCP Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    dhcp.h.ftl

  Summary:
    DHCP Freemarker Template File

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

<#if TCPIP_STACK_USE_DHCP_CLIENT == true>
/*** DHCP Configuration ***/
#define TCPIP_STACK_USE_DHCP_CLIENT
#define TCPIP_DHCP_TIMEOUT                          ${TCPIP_DHCP_TIMEOUT}
#define TCPIP_DHCP_TASK_TICK_RATE                   ${TCPIP_DHCP_TASK_TICK_RATE}
#define TCPIP_DHCP_HOST_NAME_SIZE                   ${TCPIP_DHCP_HOST_NAME_SIZE}
#define TCPIP_DHCP_CLIENT_CONNECT_PORT              ${TCPIP_DHCP_CLIENT_CONNECT_PORT}
#define TCPIP_DHCP_SERVER_LISTEN_PORT               ${TCPIP_DHCP_SERVER_LISTEN_PORT}
<#if TCPIP_DHCP_CLIENT_ENABLED == true>
#define TCPIP_DHCP_CLIENT_ENABLED                   true
<#else>
#define TCPIP_DHCP_CLIENT_ENABLED                   false
</#if>

</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
