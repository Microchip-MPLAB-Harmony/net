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
<#if CONFIG_TCPIP_USE_SNMPv3 == true>
/*** SNMPv3 Configuration ***/
#define TCPIP_STACK_USE_SNMPV3_SERVER
#define TCPIP_SNMPV3_USER_SECURITY_NAME_LEN 			${CONFIG_TCPIP_SNMPV3_USER_SECURITY_NAME_LEN}
#define TCPIP_SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN 	${CONFIG_TCPIP_SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN}
#define TCPIP_SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN 	${CONFIG_TCPIP_SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN}
#define TCPIP_SNMPV3_USM_MAX_USER						${CONFIG_TCPIP_SNMPV3_USM_MAX_USER}

<#if CONFIG_TCPIP_SNMPV3_STACK_USE_V1_V2_TRAP == true>
#define TCPIP_SNMPV3_STACK_USE_V1_V2_TRAP				true
<#else>
#define TCPIP_SNMPV3_STACK_USE_V1_V2_TRAP				false
</#if>


<#if CONFIG_TCPIP_SNMPV3_STACK_CONFIG_IDX0 == true>
<#include "/framework/tcpip/config/snmpv3_stack_config_idx0.h.ftl">
</#if>
<#if CONFIG_TCPIP_SNMPV3_STACK_CONFIG_IDX1 == true>
<#include "/framework/tcpip/config/snmpv3_stack_config_idx1.h.ftl">
</#if>
<#if CONFIG_TCPIP_SNMPV3_STACK_CONFIG_IDX2 == true>
<#include "/framework/tcpip/config/snmpv3_stack_config_idx2.h.ftl">
</#if>

<#if CONFIG_TCPIP_SNMPV3_STACK_PRIV_PROTOCOL_IDX0 == "SNMPV3_DES_PRIV" || CONFIG_TCPIP_SNMPV3_STACK_PRIV_PROTOCOL_IDX1 == "SNMPV3_DES_PRIV" || CONFIG_TCPIP_SNMPV3_STACK_PRIV_PROTOCOL_IDX2 == "SNMPV3_DES_PRIV">
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

