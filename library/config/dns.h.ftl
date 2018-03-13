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

/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS  WITHOUT  WARRANTY  OF  ANY  KIND,
EITHER EXPRESS  OR  IMPLIED,  INCLUDING  WITHOUT  LIMITATION,  ANY  WARRANTY  OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A  PARTICULAR  PURPOSE.
IN NO EVENT SHALL MICROCHIP OR  ITS  LICENSORS  BE  LIABLE  OR  OBLIGATED  UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,  BREACH  OF  WARRANTY,  OR
OTHER LEGAL  EQUITABLE  THEORY  ANY  DIRECT  OR  INDIRECT  DAMAGES  OR  EXPENSES
INCLUDING BUT NOT LIMITED TO ANY  INCIDENTAL,  SPECIAL,  INDIRECT,  PUNITIVE  OR
CONSEQUENTIAL DAMAGES, LOST  PROFITS  OR  LOST  DATA,  COST  OF  PROCUREMENT  OF
SUBSTITUTE  GOODS,  TECHNOLOGY,  SERVICES,  OR  ANY  CLAIMS  BY  THIRD   PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE  THEREOF),  OR  OTHER  SIMILAR  COSTS.
*******************************************************************************/
-->
<#if CONFIG_TCPIP_USE_DNS_CLIENT == true>
/*** DNS Client Configuration ***/
#define TCPIP_STACK_USE_DNS
#define TCPIP_DNS_CLIENT_SERVER_TMO					${CONFIG_TCPIP_DNS_CLIENT_SERVER_TMO}
#define TCPIP_DNS_CLIENT_TASK_PROCESS_RATE			${CONFIG_TCPIP_DNS_CLIENT_TASK_PROCESS_RATE}
#define TCPIP_DNS_CLIENT_CACHE_ENTRIES				${CONFIG_TCPIP_DNS_CLIENT_CACHE_ENTRIES}
#define TCPIP_DNS_CLIENT_CACHE_ENTRY_TMO			${CONFIG_TCPIP_DNS_CLIENT_CACHE_ENTRY_TMO}
#define TCPIP_DNS_CLIENT_CACHE_PER_IPV4_ADDRESS		${CONFIG_TCPIP_DNS_CLIENT_CACHE_PER_IPV4_ADDRESS}
#define TCPIP_DNS_CLIENT_CACHE_PER_IPV6_ADDRESS		${CONFIG_TCPIP_DNS_CLIENT_CACHE_PER_IPV6_ADDRESS}
#define TCPIP_DNS_CLIENT_ADDRESS_TYPE			    ${CONFIG_TCPIP_DNS_CLIENT_ADDRESS_TYPE}
#define TCPIP_DNS_CLIENT_CACHE_DEFAULT_TTL_VAL		${CONFIG_TCPIP_DNS_CLIENT_CACHE_DEFAULT_TTL_VAL}
#define TCPIP_DNS_CLIENT_CACHE_UNSOLVED_ENTRY_TMO	${CONFIG_TCPIP_DNS_CLIENT_CACHE_UNSOLVED_ENTRY_TMO}
#define TCPIP_DNS_CLIENT_LOOKUP_RETRY_TMO			${CONFIG_TCPIP_DNS_CLIENT_LOOKUP_RETRY_TMO}
#define TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN			${CONFIG_TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN}
#define TCPIP_DNS_CLIENT_MAX_SELECT_INTERFACES		${CONFIG_TCPIP_DNS_CLIENT_MAX_SELECT_INTERFACES}
<#if CONFIG_TCPIP_DNS_CLIENT_DELETE_OLD_ENTRIES == true>
#define TCPIP_DNS_CLIENT_DELETE_OLD_ENTRIES			true
<#else>
#define TCPIP_DNS_CLIENT_DELETE_OLD_ENTRIES			false
</#if>
<#if CONFIG_TCPIP_DNS_CLIENT_USER_NOTIFICATION == true>
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
