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
#define TCPIP_DNS_CLIENT_LOOKUP_RETRY_TMO			${TCPIP_DNS_CLIENT_LOOKUP_RETRY_TMO}
#define TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN			${TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN}
#define TCPIP_DNS_CLIENT_MAX_SELECT_INTERFACES		${TCPIP_DNS_CLIENT_MAX_SELECT_INTERFACES}
<#if TCPIP_DNS_CLIENT_DELETE_OLD_ENTRIES == true>
#define TCPIP_DNS_CLIENT_DELETE_OLD_ENTRIES			true
<#else>
#define TCPIP_DNS_CLIENT_DELETE_OLD_ENTRIES			false
</#if>
<#if TCPIP_DNS_CLIENT_CONSOLE_CMD == true>
#define TCPIP_DNS_CLIENT_CONSOLE_CMD               	true
<#else>
#define TCPIP_DNS_CLIENT_CONSOLE_CMD              	false
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
