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
-->

<#----------------------------------------------------------------------------
 Copyright (C) 2016-2018 Microchip Technology Inc. and its subsidiaries.

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

<#if TCPIP_USE_IGMP == true>
/*** IGMP Configuration ***/
#define TCPIP_STACK_USE_IGMP
#define TCPIP_IGMP_INTERFACES		        	    ${TCPIP_IGMP_INTERFACES}
#define TCPIP_IGMP_MCAST_GROUPS		    		    ${TCPIP_IGMP_MCAST_GROUPS}
<#if TCPIP_IGMPV2_SUPPORT_ONLY == true>
#define TCPIP_IGMPV2_SUPPORT_ONLY             		true
<#else>
#define TCPIP_IGMPV2_SUPPORT_ONLY             		false
</#if>
#define TCPIP_IGMP_SOURCES_PER_GROUP			    ${TCPIP_IGMP_SOURCES_PER_GROUP}
#define TCPIP_IGMP_SOCKET_RECORDS_PER_SOURCE		${TCPIP_IGMP_SOCKET_RECORDS_PER_SOURCE}
#define TCPIP_IGMP_ROBUSTNESS_VARIABLE		        ${TCPIP_IGMP_ROBUSTNESS_VARIABLE}
#define TCPIP_IGMP_UNSOLICITED_REPORT_INTERVAL		${TCPIP_IGMP_UNSOLICITED_REPORT_INTERVAL}
<#if TCPIP_IGMP_USER_NOTIFICATION == true>
#define TCPIP_IGMP_USER_NOTIFICATION             	true
<#else>
#define TCPIP_IGMP_USER_NOTIFICATION             	false
</#if>
#define TCPIP_IGMP_TASK_TICK_RATE		    		${TCPIP_IGMP_TASK_TICK_RATE}
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
