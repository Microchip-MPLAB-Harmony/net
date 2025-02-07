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
-->

<#--
Copyright (C) 2014-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#define TCPIP_IPV6_NDP_MAX_RTR_SOLICITATION_DELAY 	${TCPIP_MAX_RTR_SOLICITATION_DELAY}
#define TCPIP_IPV6_NDP_RTR_SOLICITATION_INTERVAL 	${TCPIP_RTR_SOLICITATION_INTERVAL}
#define TCPIP_IPV6_NDP_MAX_RTR_SOLICITATIONS 		${TCPIP_MAX_RTR_SOLICITATIONS}
#define TCPIP_IPV6_NDP_MAX_MULTICAST_SOLICIT 		${TCPIP_MAX_MULTICAST_SOLICIT}
#define TCPIP_IPV6_NDP_MAX_UNICAST_SOLICIT 			${TCPIP_MAX_UNICAST_SOLICIT}
#define TCPIP_IPV6_NDP_MAX_ANYCAST_DELAY_TIME 		${TCPIP_MAX_ANYCAST_DELAY_TIME}
#define TCPIP_IPV6_NDP_MAX_NEIGHBOR_ADVERTISEMENT 	${TCPIP_MAX_NEIGHBOR_ADVERTISEMENT}
#define TCPIP_IPV6_NDP_REACHABLE_TIME 				${TCPIP_REACHABLE_TIME}
#define TCPIP_IPV6_NDP_RETRANS_TIMER 				${TCPIP_RETRANS_TIMER}
#define TCPIP_IPV6_NDP_DELAY_FIRST_PROBE_TIME 		${TCPIP_DELAY_FIRST_PROBE_TIME}
#define TCPIP_IPV6_NDP_VALID_LIFETIME_TWO_HOURS 	(60 * 60 * 2)
#define TCPIP_IPV6_MTU_INCREASE_TIMEOUT 			${TCPIP_IPV6_MTU_INCREASE_TIMEOUT}
#define TCPIP_IPV6_NDP_TASK_TIMER_RATE 				${TCPIP_NDP_TASK_TIMER_RATE}
