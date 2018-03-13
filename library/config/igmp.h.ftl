<#--
/*******************************************************************************
  IGMP Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    igmp.h.ftl

  Summary:
    IGMP Freemarker Template File

  Description:

*******************************************************************************/

/*******************************************************************************
Copyright (c) 2016 released Microchip Technology Inc.  All rights reserved.

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
<#if CONFIG_TCPIP_USE_IGMP == true>
/*** IGMP Configuration ***/
#define TCPIP_STACK_USE_IGMP
#define TCPIP_IGMP_INTERFACES		        	    ${CONFIG_TCPIP_IGMP_INTERFACES}
#define TCPIP_IGMP_MCAST_GROUPS		    		    ${CONFIG_TCPIP_IGMP_MCAST_GROUPS}
<#if CONFIG_TCPIP_IGMPV2_SUPPORT_ONLY == true>
#define TCPIP_IGMPV2_SUPPORT_ONLY             		true
<#else>
#define TCPIP_IGMPV2_SUPPORT_ONLY             		false
</#if>
#define TCPIP_IGMP_SOURCES_PER_GROUP			    ${CONFIG_TCPIP_IGMP_SOURCES_PER_GROUP}
#define TCPIP_IGMP_SOCKET_RECORDS_PER_SOURCE		${CONFIG_TCPIP_IGMP_SOCKET_RECORDS_PER_SOURCE}
#define TCPIP_IGMP_ROBUSTNESS_VARIABLE		        ${CONFIG_TCPIP_IGMP_ROBUSTNESS_VARIABLE}
#define TCPIP_IGMP_UNSOLICITED_REPORT_INTERVAL		${CONFIG_TCPIP_IGMP_UNSOLICITED_REPORT_INTERVAL}
<#if CONFIG_TCPIP_IGMP_USER_NOTIFICATION == true>
#define TCPIP_IGMP_USER_NOTIFICATION             	true
<#else>
#define TCPIP_IGMP_USER_NOTIFICATION             	false
</#if>
#define TCPIP_IGMP_TASK_TICK_RATE		    		${CONFIG_TCPIP_IGMP_TASK_TICK_RATE}
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
