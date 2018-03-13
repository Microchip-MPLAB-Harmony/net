<#--
/*******************************************************************************
  ARP Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    arp.h.ftl

  Summary:
    ARP Freemarker Template File

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
<#if CONFIG_TCPIP_USE_ARP == true>
/*** ARP Configuration ***/
#define TCPIP_ARP_CACHE_ENTRIES                 		${CONFIG_TCPIP_ARP_CACHE_ENTRIES}
<#if CONFIG_TCPIP_ARP_CACHE_DELETE_OLD == true>
#define TCPIP_ARP_CACHE_DELETE_OLD		        	true
<#else>
#define TCPIP_ARP_CACHE_DELETE_OLD              		false
</#if>
#define TCPIP_ARP_CACHE_SOLVED_ENTRY_TMO			${CONFIG_TCPIP_ARP_CACHE_SOLVED_ENTRY_TMO}
#define TCPIP_ARP_CACHE_PENDING_ENTRY_TMO			${CONFIG_TCPIP_ARP_CACHE_PENDING_ENTRY_TMO}
#define TCPIP_ARP_CACHE_PENDING_RETRY_TMO			${CONFIG_TCPIP_ARP_CACHE_PENDING_RETRY_TMO}
#define TCPIP_ARP_CACHE_PERMANENT_QUOTA		    		${CONFIG_TCPIP_ARP_CACHE_PERMANENT_QUOTA}
#define TCPIP_ARP_CACHE_PURGE_THRESHOLD		    		${CONFIG_TCPIP_ARP_CACHE_PURGE_THRESHOLD}
#define TCPIP_ARP_CACHE_PURGE_QUANTA		    		${CONFIG_TCPIP_ARP_CACHE_PURGE_QUANTA}
#define TCPIP_ARP_CACHE_ENTRY_RETRIES		    		${CONFIG_TCPIP_ARP_CACHE_ENTRY_RETRIES}
#define TCPIP_ARP_GRATUITOUS_PROBE_COUNT			${CONFIG_TCPIP_ARP_GRATUITOUS_PROBE_COUNT}
#define TCPIP_ARP_TASK_PROCESS_RATE		        	${CONFIG_TCPIP_ARP_TASK_PROCESS_RATE}
<#if CONFIG_TCPIP_ARP_PRIMARY_CACHE_ONLY == true>
#define TCPIP_ARP_PRIMARY_CACHE_ONLY		        	true
<#else>
#define TCPIP_ARP_PRIMARY_CACHE_ONLY              		false
</#if>
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
