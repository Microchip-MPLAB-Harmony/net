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

<#if TCPIP_SNMPV3_STACK_PRIV_PROTOCOL_IDX0 == "SNMPV3_AES_PRIV" || TCPIP_SNMPV3_STACK_PRIV_PROTOCOL_IDX1 == "SNMPV3_AES_PRIV" || TCPIP_SNMPV3_STACK_PRIV_PROTOCOL_IDX2 == "SNMPV3_AES_PRIV">
#define TCPIP_SNMPV3_SUPPORT_AES
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

