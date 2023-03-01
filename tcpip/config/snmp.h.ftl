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

<#--
Copyright (C) 2014-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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


<#if TCPIP_USE_SNMP == true>
/*** SNMP Configuration ***/
#define TCPIP_STACK_USE_SNMP_SERVER
#define TCPIP_SNMP_TASK_PROCESS_RATE 				${TCPIP_SNMP_TASK_PROCESS_RATE}
<#if TCPIP_SNMP_MOUNT_POINT?has_content>
#define TCPIP_SNMP_MOUNT_POINT						"${TCPIP_SNMP_MOUNT_POINT}"
<#else>
#define TCPIP_SNMP_MOUNT_POINT 						"\"
</#if>
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
