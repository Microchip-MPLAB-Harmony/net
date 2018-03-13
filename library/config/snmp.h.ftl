<#--
/*******************************************************************************
  SNMP Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    snmp.h.ftl

  Summary:
    SNMP Freemarker Template File

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

<#if CONFIG_TCPIP_USE_SNMP == true>
/*** SNMP Configuration ***/
#define TCPIP_STACK_USE_SNMP_SERVER
#define TCPIP_SNMP_TASK_PROCESS_RATE 				${CONFIG_TCPIP_SNMP_TASK_PROCESS_RATE}
<#if CONFIG_TCPIP_SNMP_BIB_FILE_NAME?has_content>
#define TCPIP_SNMP_BIB_FILE_NAME 					"${CONFIG_TCPIP_SNMP_BIB_FILE_NAME}"
<#else>
#define TCPIP_SNMP_BIB_FILE_NAME 					0
</#if>
#define TCPIP_SNMP_OID_MAX_LEN						${CONFIG_TCPIP_SNMP_OID_MAX_LEN}
#define TCPIP_SNMP_MAX_MSG_SIZE 					${CONFIG_TCPIP_SNMP_MAX_MSG_SIZE}
#define TCPIP_SNMP_MAX_NON_REC_ID_OID 				${CONFIG_TCPIP_SNMP_MAX_NON_REC_ID_OID}
#define TCPIP_SNMP_COMMUNITY_MAX_LEN 				${CONFIG_TCPIP_SNMP_COMMUNITY_MAX_LEN}
#define TCPIP_SNMP_MAX_COMMUNITY_SUPPORT 			${CONFIG_TCPIP_SNMP_MAX_COMMUNITY_SUPPORT}
#define TCPIP_SNMP_NOTIFY_COMMUNITY_LEN 			${CONFIG_TCPIP_SNMP_NOTIFY_COMMUNITY_LEN}
#define TCPIP_SNMP_TRAP_COMMUNITY_MAX_LEN_MEM_USE 	${CONFIG_TCPIP_SNMP_TRAP_COMMUNITY_MAX_LEN_MEM_USE}
#define TCPIP_SNMP_TRAP_TABLE_SIZE 					${CONFIG_TCPIP_SNMP_TRAP_TABLE_SIZE}
<#if CONFIG_TCPIP_SNMP_USE_TRAP_SUPPORT == true>
#define TCPIP_SNMP_USE_TRAP_SUPPORT   				true
<#else>
#define TCPIP_SNMP_USE_TRAP_SUPPORT   				false
</#if>
<#if CONFIG_TCPIP_SNMP_STACK_USE_V2_TRAP == true>

#define TCPIP_SNMP_STACK_USE_V2_TRAP   				true
<#else>

#define TCPIP_SNMP_STACK_USE_V2_TRAP   				false
</#if>
/***The maximum size of TRAP community string length***/
#define TCPIP_SNMP_TRAP_COMMUNITY_MAX_LEN       	(TCPIP_SNMP_TRAP_COMMUNITY_MAX_LEN_MEM_USE+1)
</#if>
<#if CONFIG_TCPIP_SNMP_STACK_CONFIG_IDX0 == true>
<#include "/framework/tcpip/config/snmp_stack_config_idx0.h.ftl">
</#if>
<#if CONFIG_TCPIP_SNMP_STACK_CONFIG_IDX1 == true>
<#include "/framework/tcpip/config/snmp_stack_config_idx1.h.ftl">
</#if>
<#if CONFIG_TCPIP_SNMP_STACK_CONFIG_IDX2 == true>

<#include "/framework/tcpip/config/snmp_stack_config_idx2.h.ftl">
</#if>
<#if CONFIG_TCPIP_USE_SNMPv3 == true>
<#include "/framework/tcpip/config/snmpv3.h.ftl">
</#if>


<#--
/*******************************************************************************
 End of File
*/
-->
