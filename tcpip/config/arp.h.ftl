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

<#if TCPIP_USE_ARP == true>
/*** ARP Configuration ***/
#define TCPIP_ARP_CACHE_ENTRIES                 		${TCPIP_ARP_CACHE_ENTRIES}
<#if TCPIP_ARP_CACHE_DELETE_OLD == true>
#define TCPIP_ARP_CACHE_DELETE_OLD		        	true
<#else>
#define TCPIP_ARP_CACHE_DELETE_OLD              		false
</#if>
#define TCPIP_ARP_CACHE_SOLVED_ENTRY_TMO			${TCPIP_ARP_CACHE_SOLVED_ENTRY_TMO}
#define TCPIP_ARP_CACHE_PENDING_ENTRY_TMO			${TCPIP_ARP_CACHE_PENDING_ENTRY_TMO}
#define TCPIP_ARP_CACHE_PENDING_RETRY_TMO			${TCPIP_ARP_CACHE_PENDING_RETRY_TMO}
#define TCPIP_ARP_CACHE_PERMANENT_QUOTA		    		${TCPIP_ARP_CACHE_PERMANENT_QUOTA}
#define TCPIP_ARP_CACHE_PURGE_THRESHOLD		    		${TCPIP_ARP_CACHE_PURGE_THRESHOLD}
#define TCPIP_ARP_CACHE_PURGE_QUANTA		    		${TCPIP_ARP_CACHE_PURGE_QUANTA}
#define TCPIP_ARP_CACHE_ENTRY_RETRIES		    		${TCPIP_ARP_CACHE_ENTRY_RETRIES}
#define TCPIP_ARP_GRATUITOUS_PROBE_COUNT			${TCPIP_ARP_GRATUITOUS_PROBE_COUNT}
#define TCPIP_ARP_TASK_PROCESS_RATE		        	${TCPIP_ARP_TASK_PROCESS_RATE}
<#if TCPIP_ARP_PRIMARY_CACHE_ONLY == true>
#define TCPIP_ARP_PRIMARY_CACHE_ONLY		        	true
<#else>
#define TCPIP_ARP_PRIMARY_CACHE_ONLY              		false
</#if>
<#if TCPIP_ARP_COMMANDS_ENABLE == true>
#define TCPIP_ARP_COMMANDS true
<#else>
#define TCPIP_ARP_COMMANDS false
</#if>
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
