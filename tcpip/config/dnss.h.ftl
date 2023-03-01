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

<#if TCPIP_USE_DNSS == true>
/*** DNS Server Configuration ***/
#define TCPIP_STACK_USE_DNS_SERVER
#define TCPIP_DNSS_HOST_NAME_LEN		    	${TCPIP_DNSS_HOST_NAME_LEN}
<#if TCPIP_DNSS_REPLY_BOARD_ADDR == true>
#define TCPIP_DNSS_REPLY_BOARD_ADDR				true
<#else>
#define TCPIP_DNSS_REPLY_BOARD_ADDR				false
</#if>
#define TCPIP_DNSS_CACHE_PER_IPV4_ADDRESS		${TCPIP_DNSS_CACHE_PER_IPV4_ADDRESS}
<#if TCPIP_DNSS_CACHE_PER_IPV6_ADDRESS??>
#define TCPIP_DNSS_CACHE_PER_IPV6_ADDRESS		${TCPIP_DNSS_CACHE_PER_IPV6_ADDRESS}
<#else>
#define TCPIP_DNSS_CACHE_PER_IPV6_ADDRESS       0
</#if>
#define TCPIP_DNSS_TTL_TIME						${TCPIP_DNSS_TTL_TIME}
#define TCPIP_DNSS_TASK_PROCESS_RATE			${TCPIP_DNSS_TASK_PROCESS_RATE}
<#if TCPIP_DNSS_DELETE_OLD_LEASE == true>
#define TCPIP_DNSS_DELETE_OLD_LEASE				true
<#else>
#define TCPIP_DNSS_DELETE_OLD_LEASE				false
</#if>
<#if TCPIP_DNSS_CONSOLE_CMD == true>
#define TCPIP_DNSS_CONSOLE_CMD           true
<#else>
#define TCPIP_DNSS_CONSOLE_CMD           false
</#if>
/***Maximum DNS server Cache entries. It is the sum of TCPIP_DNSS_CACHE_PER_IPV4_ADDRESS and TCPIP_DNSS_CACHE_PER_IPV6_ADDRESS.***/
#define TCPIP_DNSS_CACHE_MAX_SERVER_ENTRIES     (TCPIP_DNSS_CACHE_PER_IPV4_ADDRESS + TCPIP_DNSS_CACHE_PER_IPV6_ADDRESS)
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
