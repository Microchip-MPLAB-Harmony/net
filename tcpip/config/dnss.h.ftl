<#--
/*******************************************************************************
  DNS Server Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    dns.h.ftl

  Summary:
    DNS Server Freemarker Template File

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

<#if TCPIP_USE_DNSS == true>
/*** DNS Server Configuration ***/
#define TCPIP_STACK_USE_DNS_SERVER
#define TCPIP_DNSS_HOST_NAME_LEN		    		${TCPIP_DNSS_HOST_NAME_LEN}
<#if TCPIP_DNSS_REPLY_BOARD_ADDR == true>
#define TCPIP_DNSS_REPLY_BOARD_ADDR				true
<#else>
#define TCPIP_DNSS_REPLY_BOARD_ADDR				false
</#if>
#define TCPIP_DNSS_CACHE_PER_IPV4_ADDRESS			${TCPIP_DNSS_CACHE_PER_IPV4_ADDRESS}
#define TCPIP_DNSS_CACHE_PER_IPV6_ADDRESS			${TCPIP_DNSS_CACHE_PER_IPV6_ADDRESS}
#define TCPIP_DNSS_TTL_TIME							${TCPIP_DNSS_TTL_TIME}
#define TCPIP_DNSS_TASK_PROCESS_RATE			    ${TCPIP_DNSS_TASK_PROCESS_RATE}
<#if TCPIP_DNSS_DELETE_OLD_LEASE == true>
#define TCPIP_DNSS_DELETE_OLD_LEASE				true
<#else>
#define TCPIP_DNSS_DELETE_OLD_LEASE				false
</#if>

/***Maximum DNS server Cache entries. It is the sum of TCPIP_DNSS_CACHE_PER_IPV4_ADDRESS and TCPIP_DNSS_CACHE_PER_IPV6_ADDRESS.***/
#define TCPIP_DNSS_CACHE_MAX_SERVER_ENTRIES     (TCPIP_DNSS_CACHE_PER_IPV4_ADDRESS+TCPIP_DNSS_CACHE_PER_IPV6_ADDRESS)
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
