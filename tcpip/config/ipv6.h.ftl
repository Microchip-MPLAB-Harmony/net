<#--
/*******************************************************************************
  IPv6 Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    ipv6.h.ftl

  Summary:
    IPv6 Freemarker Template File

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

<#if TCPIP_STACK_USE_IPV6 == true>
/*** IPv6 Configuration ***/
#define TCPIP_IPV6_DEFAULT_ALLOCATION_BLOCK_SIZE 		${TCPIP_IPV6_DEFAULT_ALLOCATION_BLOCK_SIZE}
#define TCPIP_IPV6_MINIMUM_LINK_MTU 					${TCPIP_IPV6_MINIMUM_LINK_MTU}
#define TCPIP_IPV6_DEFAULT_LINK_MTU 					${TCPIP_IPV6_DEFAULT_LINK_MTU}
#define TCPIP_IPV6_DEFAULT_CUR_HOP_LIMIT 				${TCPIP_IPV6_DEFAULT_CUR_HOP_LIMIT}
#define TCPIP_IPV6_DEFAULT_BASE_REACHABLE_TIME 			${TCPIP_IPV6_DEFAULT_BASE_REACHABLE_TIME}
#define TCPIP_IPV6_DEFAULT_RETRANSMIT_TIME 				${TCPIP_IPV6_DEFAULT_RETRANSMIT_TIME}
#define TCPIP_IPV6_QUEUE_NEIGHBOR_PACKET_LIMIT 			${TCPIP_IPV6_QUEUE_NEIGHBOR_PACKET_LIMIT}
#define TCPIP_IPV6_NEIGHBOR_CACHE_ENTRY_STALE_TIMEOUT 	${TCPIP_IPV6_NEIGHBOR_CACHE_ENTRY_STALE_TIMEOUT}
#define TCPIP_IPV6_QUEUE_MCAST_PACKET_LIMIT 			${TCPIP_IPV6_QUEUE_MCAST_PACKET_LIMIT}
#define TCPIP_IPV6_QUEUED_MCAST_PACKET_TIMEOUT 			${TCPIP_IPV6_QUEUED_MCAST_PACKET_TIMEOUT}
#define TCPIP_IPV6_TASK_PROCESS_RATE 					${TCPIP_IPV6_TASK_PROCESS_RATE}
#define TCPIP_IPV6_INIT_TASK_PROCESS_RATE 				${TCPIP_IPV6_INIT_TASK_PROCESS_RATE}
#define TCPIP_IPV6_ULA_NTP_ACCESS_TMO 					${TCPIP_IPV6_ULA_NTP_ACCESS_TMO}
#define TCPIP_IPV6_ULA_NTP_VALID_WINDOW 				${TCPIP_IPV6_ULA_NTP_VALID_WINDOW}
#define TCPIP_IPV6_FRAGMENT_PKT_TIMEOUT 				${TCPIP_IPV6_FRAGMENT_PKT_TIMEOUT}
#define TCPIP_IPV6_RX_FRAGMENTED_BUFFER_SIZE 			${TCPIP_IPV6_RX_FRAGMENTED_BUFFER_SIZE}

<#if TCPIP_STACK_USE_ICMPV6_SERVER == true>
#define TCPIP_STACK_USE_ICMPV6_SERVER
</#if>
<#if TCPIP_STACK_USE_ICMPV6_CLIENT == true>
#define TCPIP_STACK_USE_ICMPV6_CLIENT
<#if TCPIP_ICMPV6_CLIENT_USER_NOTIFICATION == true>
#define TCPIP_ICMPV6_CLIENT_USER_NOTIFICATION   true
<#else>
#define TCPIP_ICMPV6_CLIENT_USER_NOTIFICATION   false
</#if>
</#if>
</#if>
