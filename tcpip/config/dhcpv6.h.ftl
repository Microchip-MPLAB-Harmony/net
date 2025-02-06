<#--
/*******************************************************************************
  DHCP Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    dhcpv6.h.ftl

  Summary:
    DHCPv6 Freemarker Template File

  Description:

 *******************************************************************************/
-->

<#--
Copyright (C) 2020-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

<#if TCPIP_STACK_USE_DHCPV6_CLIENT == true>
/*** DHCP Configuration ***/
#define TCPIP_STACK_USE_DHCPV6_CLIENT
#define TCPIP_DHCPV6_CONFIG_STARTUP_FLAG                       		\
<#if TCPIP_DHCPV6_FLAG_STACK_START_ENABLE>
																	TCPIP_DHCPV6_FLAG_START_ENABLE |\
</#if>
<#if TCPIP_DHCPV6_FLAG_DAD_DISABLE>
																	TCPIP_DHCPV6_FLAG_DAD_DISABLE |\
</#if>
<#if TCPIP_DHCPV6_FLAG_IA_IGNORE_RENEW_LTIME>
																	TCPIP_DHCPV6_FLAG_IA_IGNORE_RENEW_LTIME |\
</#if>
<#if TCPIP_DHCPV6_FLAG_IA_IGNORE_REBIND_LTIME>
																	TCPIP_DHCPV6_FLAG_IA_IGNORE_REBIND_LTIME |\
</#if>
<#if TCPIP_DHCPV6_FLAG_IA_NOTIFY_SUB_STATE>
																	TCPIP_DHCPV6_FLAG_IA_NOTIFY_SUB_STATE |\
</#if>
																	TCPIP_DHCPV6_FLAG_NONE
																	
#define TCPIP_DHCPV6_MIN_UDP_TX_BUFFER_SIZE                         ${TCPIP_DHCPV6_MIN_UDP_TX_BUFFER_SIZE}
#define TCPIP_DHCPV6_DUID_TYPE_CONFIG								${TCPIP_DHCPV6_DUID_TYPE_CONFIG}
#define TCPIP_DHCPV6_IANA_DESCRIPTORS_NO							${TCPIP_DHCPV6_IANA_DESCRIPTORS_NO}
#define TCPIP_DHCPV6_IANA_DEFAULT_T1								${TCPIP_DHCPV6_IANA_DEFAULT_T1}
#define TCPIP_DHCPV6_IANA_DEFAULT_T2								${TCPIP_DHCPV6_IANA_DEFAULT_T2}
#define TCPIP_DHCPV6_IANA_SOLICIT_T1								${TCPIP_DHCPV6_IANA_SOLICIT_T1}
#define TCPIP_DHCPV6_IANA_SOLICIT_T2								${TCPIP_DHCPV6_IANA_SOLICIT_T2}
#define TCPIP_DHCPV6_IANA_SOLICIT_ADDRESSES_NO						${TCPIP_DHCPV6_IANA_SOLICIT_ADDRESSES_NO}
#define TCPIP_DHCPV6_IANA_SOLICIT_DEFAULT_ADDRESS					"${TCPIP_DHCPV6_IANA_SOLICIT_DEFAULT_ADDRESS}"
#define TCPIP_DHCPV6_IATA_DESCRIPTORS_NO							${TCPIP_DHCPV6_IATA_DESCRIPTORS_NO}
#define TCPIP_DHCPV6_IATA_DEFAULT_T1								${TCPIP_DHCPV6_IATA_DEFAULT_T1}
#define TCPIP_DHCPV6_IATA_DEFAULT_T2								${TCPIP_DHCPV6_IATA_DEFAULT_T2}
#define TCPIP_DHCPV6_IATA_SOLICIT_ADDRESSES_NO						${TCPIP_DHCPV6_IATA_SOLICIT_ADDRESSES_NO}
#define TCPIP_DHCPV6_IATA_SOLICIT_DEFAULT_ADDRESS					"${TCPIP_DHCPV6_IATA_SOLICIT_DEFAULT_ADDRESS}"
#define TCPIP_DHCPV6_IA_FREE_DESCRIPTORS_NO							${TCPIP_DHCPV6_IA_FREE_DESCRIPTORS_NO}
#define TCPIP_DHCPV6_IA_SOLICIT_ADDRESS_PREF_LTIME					${TCPIP_DHCPV6_IA_SOLICIT_ADDRESS_PREF_LTIME}
#define TCPIP_DHCPV6_IA_SOLICIT_ADDRESS_VALID_LTIME					${TCPIP_DHCPV6_IA_SOLICIT_ADDRESS_VALID_LTIME}

#define TCPIP_DHCPV6_TASK_TICK_RATE                   				${TCPIP_DHCPV6_TASK_TICK_RATE}
#define TCPIP_DHCPV6_STATUS_CODE_MESSAGE_LEN                   		${TCPIP_DHCPV6_STATUS_CODE_MESSAGE_LEN}
#define TCPIP_DHCPV6_CLIENT_CONNECT_PORT              				${TCPIP_DHCPV6_CLIENT_CONNECT_PORT}
#define TCPIP_DHCPV6_SERVER_LISTEN_PORT		            			${TCPIP_DHCPV6_SERVER_LISTEN_PORT}
<#if TCPIP_DHCPV6_CLIENT_CONSOLE_CMD == true>
#define TCPIP_DHCPV6_CLIENT_CONSOLE_CMD                				true
<#else>
#define TCPIP_DHCPV6_CLIENT_CONSOLE_CMD              				false
</#if>

#define TCPIP_DHCPV6_FORCED_SERVER_PREFERENCE           			${TCPIP_DHCPV6_FORCED_SERVER_PREFERENCE}
#define TCPIP_DHCPV6_MESSAGE_BUFFERS            					${TCPIP_DHCPV6_MESSAGE_BUFFERS}
#define TCPIP_DHCPV6_MESSAGE_BUFFER_SIZE            				${TCPIP_DHCPV6_MESSAGE_BUFFER_SIZE}
#define TCPIP_DHCPV6_DNS_SERVERS_NO             					${TCPIP_DHCPV6_DNS_SERVERS_NO}
#define TCPIP_DHCPV6_DOMAIN_SEARCH_LIST_SIZE						${TCPIP_DHCPV6_DOMAIN_SEARCH_LIST_SIZE}

<#if TCPIP_DHCPV6_USER_NOTIFICATION == true>
#define TCPIP_DHCPV6_USER_NOTIFICATION                				true
<#else>
#define TCPIP_DHCPV6_USER_NOTIFICATION              				false
</#if>
<#if TCPIP_DHCPV6_CONFIG_DEBUG_LEVEL == true>
#define TCPIP_DHCPV6_DEBUG_LEVEL									\
<#if TCPIP_DHCPV6_DEBUG_MASK_BASIC>
																	TCPIP_DHCPV6_DEBUG_MASK_BASIC |\
</#if>
<#if TCPIP_DHCPV6_DEBUG_MASK_IN>
																	TCPIP_DHCPV6_DEBUG_MASK_IN |\
</#if>
<#if TCPIP_DHCPV6_DEBUG_MASK_OUT>
																	TCPIP_DHCPV6_DEBUG_MASK_OUT |\
</#if>
<#if TCPIP_DHCPV6_DEBUG_MASK_CLIENT_STATE>
																	TCPIP_DHCPV6_DEBUG_MASK_CLIENT_STATE |\
</#if>
<#if TCPIP_DHCPV6_DEBUG_MASK_CLIENT_STATS>
																	TCPIP_DHCPV6_DEBUG_MASK_CLIENT_STATS |\
</#if>
<#if TCPIP_DHCPV6_DEBUG_MASK_CLIENT_NOTIFY_STATE>
																	TCPIP_DHCPV6_DEBUG_MASK_CLIENT_NOTIFY_STATE |\
</#if>
<#if TCPIP_DHCPV6_DEBUG_MASK_IA_STATE>
																	TCPIP_DHCPV6_DEBUG_MASK_IA_STATE |\
</#if>
<#if TCPIP_DHCPV6_DEBUG_MASK_IA_SUBSTATE>
																	TCPIP_DHCPV6_DEBUG_MASK_IA_SUBSTATE |\
</#if>
<#if TCPIP_DHCPV6_DEBUG_MASK_ADD_STATE>
																	TCPIP_DHCPV6_DEBUG_MASK_ADD_STATE |\
</#if>
<#if TCPIP_DHCPV6_DEBUG_MASK_LISTS>
																	TCPIP_DHCPV6_DEBUG_MASK_LISTS |\
</#if>
<#if TCPIP_DHCPV6_DEBUG_MASK_BUFF_TRACE>
																	TCPIP_DHCPV6_DEBUG_MASK_BUFF_TRACE |\
</#if>
																	0
</#if>																	
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
