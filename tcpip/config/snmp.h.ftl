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

<#if TCPIP_USE_SNMP == true>
/*** SNMP Configuration ***/
#define TCPIP_STACK_USE_SNMP_SERVER
#define TCPIP_SNMP_TASK_PROCESS_RATE 				${TCPIP_SNMP_TASK_PROCESS_RATE}
<#if TCPIP_SNMP_BIB_FILE_NAME?has_content>
#define TCPIP_SNMP_BIB_FILE_NAME 					"${TCPIP_SNMP_BIB_FILE_NAME}"
<#else>
#define TCPIP_SNMP_BIB_FILE_NAME 					0
</#if>
#define TCPIP_SNMP_OID_MAX_LEN						${TCPIP_SNMP_OID_MAX_LEN}
#define TCPIP_SNMP_MAX_MSG_SIZE 					${TCPIP_SNMP_MAX_MSG_SIZE}
#define TCPIP_SNMP_MAX_NON_REC_ID_OID 				${TCPIP_SNMP_MAX_NON_REC_ID_OID}
#define TCPIP_SNMP_COMMUNITY_MAX_LEN 				${TCPIP_SNMP_COMMUNITY_MAX_LEN}
#define TCPIP_SNMP_MAX_COMMUNITY_SUPPORT 			${TCPIP_SNMP_MAX_COMMUNITY_SUPPORT}
#define TCPIP_SNMP_NOTIFY_COMMUNITY_LEN 			${TCPIP_SNMP_NOTIFY_COMMUNITY_LEN}
#define TCPIP_SNMP_TRAP_COMMUNITY_MAX_LEN_MEM_USE 	${TCPIP_SNMP_TRAP_COMMUNITY_MAX_LEN_MEM_USE}
#define TCPIP_SNMP_TRAP_TABLE_SIZE 					${TCPIP_SNMP_TRAP_TABLE_SIZE}
<#if TCPIP_SNMP_USE_TRAP_SUPPORT == true>
#define TCPIP_SNMP_USE_TRAP_SUPPORT   				true
<#else>
#define TCPIP_SNMP_USE_TRAP_SUPPORT   				false
</#if>
<#if TCPIP_SNMP_STACK_USE_V2_TRAP == true>

#define TCPIP_SNMP_STACK_USE_V2_TRAP   				true
<#else>

#define TCPIP_SNMP_STACK_USE_V2_TRAP   				false
</#if>
/***The maximum size of TRAP community string length***/
#define TCPIP_SNMP_TRAP_COMMUNITY_MAX_LEN       	(TCPIP_SNMP_TRAP_COMMUNITY_MAX_LEN_MEM_USE+1)

<#list 0..TCPIP_SNMP_COMMUNITY_MAX_NUM as i>
<#assign TCPIP_SNMP_STACK_CONFIG_IDX_ENABLE = "TCPIP_SNMP_STACK_CONFIG_IDX" + i>
<#assign TCPIP_SNMP_STACK_READCOMMUNITY_NAME_IDX_ENABLE = "TCPIP_SNMP_STACK_READCOMMUNITY_NAME_IDX" + i>
<#assign TCPIP_SNMP_STACK_WRITECOMMUNITY_NAME_IDX_ENABLE = "TCPIP_SNMP_STACK_WRITECOMMUNITY_NAME_IDX" + i>
<#if .vars[TCPIP_SNMP_STACK_CONFIG_IDX_ENABLE]?has_content && (.vars[TCPIP_SNMP_STACK_CONFIG_IDX_ENABLE] != false)>	
/*** SNMP Stack Configuration Index ${i} ***/
<#if (.vars[TCPIP_SNMP_STACK_READCOMMUNITY_NAME_IDX_ENABLE]?has_content)>
#define TCPIP_SNMP_STACK_READCOMMUNITY_NAME_IDX${i} 	"${.vars[TCPIP_SNMP_STACK_READCOMMUNITY_NAME_IDX_ENABLE]}" 
<#else>
#define TCPIP_SNMP_STACK_READCOMMUNITY_NAME_IDX${i}		0
</#if>
<#if (.vars[TCPIP_SNMP_STACK_WRITECOMMUNITY_NAME_IDX_ENABLE]?has_content)>
#define TCPIP_SNMP_STACK_WRITECOMMUNITY_NAME_IDX${i} 	"${.vars[TCPIP_SNMP_STACK_WRITECOMMUNITY_NAME_IDX_ENABLE]}" 

<#else>
#define TCPIP_SNMP_STACK_WRITECOMMUNITY_NAME_IDX${i} 	0

</#if>
</#if>
</#list>
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
