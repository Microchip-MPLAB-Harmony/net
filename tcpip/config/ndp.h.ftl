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
