<#--
/*******************************************************************************
  SNMPv3 Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    snmpv3.h.ftl

  Summary:
    SNMPv3 Freemarker Template File

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
<#if TCPIP_USE_SNMPv3 == true>
/*** SNMPv3 Configuration ***/
#define TCPIP_STACK_USE_SNMPV3_SERVER
#define TCPIP_SNMPV3_USER_SECURITY_NAME_LEN 			${TCPIP_SNMPV3_USER_SECURITY_NAME_LEN}
#define TCPIP_SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN 	${TCPIP_SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN}
#define TCPIP_SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN 	${TCPIP_SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN}
#define TCPIP_SNMPV3_USM_MAX_USER						${TCPIP_SNMPV3_USM_MAX_USER}
<#if TCPIP_SNMPV3_STACK_USE_V1_V2_TRAP == true>
#define TCPIP_SNMPV3_STACK_USE_V1_V2_TRAP				true
<#else>
#define TCPIP_SNMPV3_STACK_USE_V1_V2_TRAP				false
</#if>
<#list 0..TCPIP_SNMPV3_USE_MAX_NUM as i>
<#assign TCPIP_SNMPV3_STACK_CONFIG_IDX_ENABLE = "TCPIP_SNMPV3_STACK_CONFIG_IDX" + i>
<#assign TCPIP_SNMPV3_STACK_USM_NAME = "TCPIP_SNMPV3_STACK_USM_NAME_IDX" + i>
<#assign TCPIP_SNMPV3_STACK_SECURITY_LEVEL = "TCPIP_SNMPV3_STACK_SECURITY_LEVEL_IDX" + i>
<#assign TCPIP_SNMPV3_STACK_AUTH_PROTOCOL = "TCPIP_SNMPV3_STACK_AUTH_PROTOCOL_IDX" + i>
<#assign TCPIP_SNMPV3_STACK_AUTH_PASSWORD = "TCPIP_SNMPV3_STACK_AUTH_PASSWORD_IDX" + i>
<#assign TCPIP_SNMPV3_STACK_PRIV_PROTOCOL = "TCPIP_SNMPV3_STACK_PRIV_PROTOCOL_IDX" + i>
<#assign TCPIP_SNMPV3_STACK_PRIV_PASSWORD = "TCPIP_SNMPV3_STACK_PRIV_PASSWORD_IDX" + i>
<#assign TCPIP_SNMPV3_TARGET_ENTRY_MESSAGE_PROTOCOL_TYPE = "TCPIP_SNMPV3_TARGET_ENTRY_MESSAGE_PROTOCOL_TYPE_IDX" + i>
<#assign TCPIP_SNMPV3_TARGET_ENTRY_SEC_MODEL_TYPE = "TCPIP_SNMPV3_TARGET_ENTRY_SEC_MODEL_TYPE_IDX" + i>

<#if .vars[TCPIP_SNMPV3_STACK_CONFIG_IDX_ENABLE]?has_content && (.vars[TCPIP_SNMPV3_STACK_CONFIG_IDX_ENABLE] != false)>	
/*** SNMPV3 Stack Configuration Index ${i} ***/
<#if (.vars[TCPIP_SNMPV3_STACK_USM_NAME]?has_content)>
#define TCPIP_SNMPV3_STACK_USM_NAME_IDX${i} 	"${.vars[TCPIP_SNMPV3_STACK_USM_NAME]}" 
<#else>
#define TCPIP_SNMPV3_STACK_USM_NAME_IDX${i}		0
</#if>
#define TCPIP_SNMPV3_STACK_SECURITY_LEVEL_IDX${i} 	${.vars[TCPIP_SNMPV3_STACK_SECURITY_LEVEL]}
#define TCPIP_SNMPV3_STACK_AUTH_PROTOCOL_IDX${i} 	${.vars[TCPIP_SNMPV3_STACK_AUTH_PROTOCOL]}
<#if (.vars[TCPIP_SNMPV3_STACK_AUTH_PASSWORD]?has_content)>
#define TCPIP_SNMPV3_STACK_AUTH_PASSWORD_IDX${i} 	"${.vars[TCPIP_SNMPV3_STACK_AUTH_PASSWORD]}" 
<#else>
#define TCPIP_SNMPV3_STACK_AUTH_PASSWORD_IDX${i}	0
</#if>
#define TCPIP_SNMPV3_STACK_PRIV_PROTOCOL_IDX${i} 	${.vars[TCPIP_SNMPV3_STACK_PRIV_PROTOCOL]}
<#if (.vars[TCPIP_SNMPV3_STACK_PRIV_PASSWORD]?has_content)>
#define TCPIP_SNMPV3_STACK_PRIV_PASSWORD_IDX${i} 	"${.vars[TCPIP_SNMPV3_STACK_PRIV_PASSWORD]}" 
<#else>
#define TCPIP_SNMPV3_STACK_PRIV_PASSWORD_IDX${i}	0
</#if>
<#if (.vars[TCPIP_SNMPV3_STACK_USM_NAME]?has_content)>
#define TCPIP_SNMPV3_TARGET_ENTRY_SEC_NAME_IDX${i} 	"${.vars[TCPIP_SNMPV3_STACK_USM_NAME]}" 
<#else>
#define TCPIP_SNMPV3_TARGET_ENTRY_SEC_NAME_IDX${i}	0
</#if>
#define TCPIP_SNMPV3_TARGET_ENTRY_MESSAGE_PROTOCOL_TYPE_IDX${i} 	${.vars[TCPIP_SNMPV3_TARGET_ENTRY_MESSAGE_PROTOCOL_TYPE]}
#define TCPIP_SNMPV3_TARGET_ENTRY_SEC_MODEL_TYPE_IDX${i} 	${.vars[TCPIP_SNMPV3_TARGET_ENTRY_SEC_MODEL_TYPE]}
#define TCPIP_SNMPV3_TARGET_ENTRY_SEC_LEVEL_IDX${i} 	${.vars[TCPIP_SNMPV3_STACK_SECURITY_LEVEL]}
</#if>
</#list>
<#if TCPIP_SNMPV3_STACK_PRIV_PROTOCOL_IDX0 == "SNMPV3_DES_PRIV" || TCPIP_SNMPV3_STACK_PRIV_PROTOCOL_IDX1 == "SNMPV3_DES_PRIV" || TCPIP_SNMPV3_STACK_PRIV_PROTOCOL_IDX2 == "SNMPV3_DES_PRIV">
#define TCPIP_SNMPV3_SUPPORT_DES
</#if>
/***User security name length for memory validation***/
#define TCPIP_SNMPV3_USER_SECURITY_NAME_LEN_MEM_USE (TCPIP_SNMPV3_USER_SECURITY_NAME_LEN+1)

/***SNMPv3 authentication localized Key length for memory validation***/
#define TCPIP_SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN_MEM_USE (TCPIP_SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN+1)

/***SNMPv3 privacy key length size for memory validation***/
#define TCPIP_SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN_MEM_USE (TCPIP_SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN+1)
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->

