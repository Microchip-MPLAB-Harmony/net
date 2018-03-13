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
#define TCPIP_IPV6_NDP_MAX_RTR_SOLICITATION_DELAY 	${CONFIG_TCPIP_MAX_RTR_SOLICITATION_DELAY}
#define TCPIP_IPV6_NDP_RTR_SOLICITATION_INTERVAL 	${CONFIG_TCPIP_RTR_SOLICITATION_INTERVAL}
#define TCPIP_IPV6_NDP_MAX_RTR_SOLICITATIONS 		${CONFIG_TCPIP_MAX_RTR_SOLICITATIONS}
#define TCPIP_IPV6_NDP_MAX_MULTICAST_SOLICIT 		${CONFIG_TCPIP_MAX_MULTICAST_SOLICIT}
#define TCPIP_IPV6_NDP_MAX_UNICAST_SOLICIT 			${CONFIG_TCPIP_MAX_UNICAST_SOLICIT}
#define TCPIP_IPV6_NDP_MAX_ANYCAST_DELAY_TIME 		${CONFIG_TCPIP_MAX_ANYCAST_DELAY_TIME}
#define TCPIP_IPV6_NDP_MAX_NEIGHBOR_ADVERTISEMENT 	${CONFIG_TCPIP_MAX_NEIGHBOR_ADVERTISEMENT}
#define TCPIP_IPV6_NDP_REACHABLE_TIME 				${CONFIG_TCPIP_REACHABLE_TIME}
#define TCPIP_IPV6_NDP_RETRANS_TIMER 				${CONFIG_TCPIP_RETRANS_TIMER}
#define TCPIP_IPV6_NDP_DELAY_FIRST_PROBE_TIME 		${CONFIG_TCPIP_DELAY_FIRST_PROBE_TIME}
#define TCPIP_IPV6_NDP_VALID_LIFETIME_TWO_HOURS 	(60 * 60 * 2)
#define TCPIP_IPV6_MTU_INCREASE_TIMEOUT 			${CONFIG_TCPIP_IPV6_MTU_INCREASE_TIMEOUT}
#define TCPIP_IPV6_NDP_TASK_TIMER_RATE 				${CONFIG_TCPIP_NDP_TASK_TIMER_RATE}
