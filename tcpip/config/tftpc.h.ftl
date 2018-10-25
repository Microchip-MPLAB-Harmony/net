<#--
/*******************************************************************************
  TFTP Client Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    tftpc.h.ftl

  Summary:
    TFTP Client Freemarker Template File

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

<#if TCPIP_USE_TFTPC_MODULE == true>
/*** TFTP Client Configuration ***/
#define TCPIP_STACK_USE_TFTP_CLIENT
#define TCPIP_TFTPC_DEFAULT_IF                  "${TCPIP_TFTPC_DEFAULT_IF}"
#define TCPIP_TFTPC_SERVERADDRESS_LEN           ${TCPIP_TFTPC_SERVERADDRESS_LEN}
#define TCPIP_TFTPC_FILENAME_LEN                ${TCPIP_TFTPC_FILENAME_LEN}
<#if TCPIP_TFTPC_USER_NOTIFICATION == true>
#define TCPIP_TFTPC_USER_NOTIFICATION   true
<#else>
#define TCPIP_TFTPC_USER_NOTIFICATION   false
</#if>
#define TCPIP_TFTPC_TASK_TICK_RATE              ${TCPIP_TFTPC_TASK_TICK_RATE}
#define TCPIP_TFTPC_CMD_PROCESS_TIMEOUT         ${TCPIP_TFTPC_CMD_PROCESS_TIMEOUT}
#define TCPIP_TFTPC_ARP_TIMEOUT                 ${TCPIP_TFTPC_ARP_TIMEOUT}
#define TCPIP_TFTPC_MAX_RETRIES                 ${TCPIP_TFTPC_MAX_RETRIES}

</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
