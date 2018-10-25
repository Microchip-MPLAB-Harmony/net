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
