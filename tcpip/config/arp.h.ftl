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
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
