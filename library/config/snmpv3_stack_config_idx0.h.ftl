<#--
/*******************************************************************************
  SNMPv3 Agent Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    snmpv3_stack_config_idx0.h.ftl

  Summary:
    SNMPv3 Agent Freemarker Template File

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
<#if CONFIG_TCPIP_SNMPV3_STACK_CONFIG_IDX0>
/*** SNMPV3 Stack Configuration Index 0 ***/

<#if CONFIG_TCPIP_SNMPV3_STACK_USM_NAME_IDX0?has_content>
#define TCPIP_SNMPV3_STACK_USM_NAME_IDX0 							"${CONFIG_TCPIP_SNMPV3_STACK_USM_NAME_IDX0}"
<#else>
#define TCPIP_SNMPV3_STACK_USM_NAME_IDX0	 						0
</#if>

#define TCPIP_SNMPV3_STACK_SECURITY_LEVEL_IDX0 						${CONFIG_TCPIP_SNMPV3_STACK_SECURITY_LEVEL_IDX0}

#define TCPIP_SNMPV3_STACK_AUTH_PROTOCOL_IDX0						${CONFIG_TCPIP_SNMPV3_STACK_AUTH_PROTOCOL_IDX0}

<#if CONFIG_TCPIP_SNMPV3_STACK_AUTH_PASSWORD_IDX0?has_content>
#define TCPIP_SNMPV3_STACK_AUTH_PASSWORD_IDX0 						"${CONFIG_TCPIP_SNMPV3_STACK_AUTH_PASSWORD_IDX0}"
<#else>
#define TCPIP_SNMPV3_STACK_AUTH_PASSWORD_IDX0	 					0
</#if>

#define TCPIP_SNMPV3_STACK_PRIV_PROTOCOL_IDX0						${CONFIG_TCPIP_SNMPV3_STACK_PRIV_PROTOCOL_IDX0}

<#if CONFIG_TCPIP_SNMPV3_STACK_PRIV_PASSWORD_IDX0?has_content>
#define TCPIP_SNMPV3_STACK_PRIV_PASSWORD_IDX0 						"${CONFIG_TCPIP_SNMPV3_STACK_PRIV_PASSWORD_IDX0}"
<#else>
#define TCPIP_SNMPV3_STACK_PRIV_PASSWORD_IDX0	 					0
</#if>

<#if CONFIG_TCPIP_SNMPV3_STACK_USM_NAME_IDX0?has_content>
#define TCPIP_SNMPV3_TARGET_ENTRY_SEC_NAME_IDX0 					"${CONFIG_TCPIP_SNMPV3_STACK_USM_NAME_IDX0}"
<#else>
#define TCPIP_SNMPV3_TARGET_ENTRY_SEC_NAME_IDX0	 					0
</#if>

#define TCPIP_SNMPV3_TARGET_ENTRY_MESSAGE_PROTOCOL_TYPE_IDX0		${CONFIG_TCPIP_SNMPV3_TARGET_ENTRY_MESSAGE_PROTOCOL_TYPE_IDX0}

#define TCPIP_SNMPV3_TARGET_ENTRY_SEC_MODEL_TYPE_IDX0				${CONFIG_TCPIP_SNMPV3_TARGET_ENTRY_SEC_MODEL_TYPE_IDX0}

#define TCPIP_SNMPV3_TARGET_ENTRY_SEC_LEVEL_IDX0					${CONFIG_TCPIP_SNMPV3_STACK_SECURITY_LEVEL_IDX0}

</#if>