<#--
/*******************************************************************************
  DNS Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    dns.h.ftl

  Summary:
    DNS Freemarker Template File

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


<#if TCPIP_USE_DNS_CLIENT == true>
/*** DNS Client Configuration ***/
#define TCPIP_STACK_USE_DNS
#define TCPIP_DNS_CLIENT_SERVER_TMO					${TCPIP_DNS_CLIENT_SERVER_TMO}
#define TCPIP_DNS_CLIENT_TASK_PROCESS_RATE			${TCPIP_DNS_CLIENT_TASK_PROCESS_RATE}
#define TCPIP_DNS_CLIENT_CACHE_ENTRIES				${TCPIP_DNS_CLIENT_CACHE_ENTRIES}
#define TCPIP_DNS_CLIENT_CACHE_ENTRY_TMO			${TCPIP_DNS_CLIENT_CACHE_ENTRY_TMO}
#define TCPIP_DNS_CLIENT_CACHE_PER_IPV4_ADDRESS		${TCPIP_DNS_CLIENT_CACHE_PER_IPV4_ADDRESS}
#define TCPIP_DNS_CLIENT_CACHE_PER_IPV6_ADDRESS		${TCPIP_DNS_CLIENT_CACHE_PER_IPV6_ADDRESS}
#define TCPIP_DNS_CLIENT_ADDRESS_TYPE			    ${TCPIP_DNS_CLIENT_ADDRESS_TYPE}
#define TCPIP_DNS_CLIENT_CACHE_DEFAULT_TTL_VAL		${TCPIP_DNS_CLIENT_CACHE_DEFAULT_TTL_VAL}
#define TCPIP_DNS_CLIENT_CACHE_UNSOLVED_ENTRY_TMO	${TCPIP_DNS_CLIENT_CACHE_UNSOLVED_ENTRY_TMO}
#define TCPIP_DNS_CLIENT_LOOKUP_RETRY_TMO			${TCPIP_DNS_CLIENT_LOOKUP_RETRY_TMO}
#define TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN			${TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN}
#define TCPIP_DNS_CLIENT_MAX_SELECT_INTERFACES		${TCPIP_DNS_CLIENT_MAX_SELECT_INTERFACES}
<#if TCPIP_DNS_CLIENT_DELETE_OLD_ENTRIES == true>
#define TCPIP_DNS_CLIENT_DELETE_OLD_ENTRIES			true
<#else>
#define TCPIP_DNS_CLIENT_DELETE_OLD_ENTRIES			false
</#if>
<#if TCPIP_DNS_CLIENT_USER_NOTIFICATION == true>
#define TCPIP_DNS_CLIENT_USER_NOTIFICATION   true
<#else>
#define TCPIP_DNS_CLIENT_USER_NOTIFICATION   false
</#if>
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
