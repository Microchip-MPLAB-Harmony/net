<#--
/*******************************************************************************
  UDP Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    tcpip_cmd.h.ftl

  Summary:
    tcpip_cmd Freemarker Template File

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
<#if CONFIG_TCPIP_STACK_USE_COMMANDS == true>
/*** tcpip_cmd Configuration ***/
#define TCPIP_STACK_COMMAND_ENABLE
<#if CONFIG_TCPIP_STACK_COMMANDS_STORAGE_ENABLE == true>
#define TCPIP_STACK_COMMANDS_STORAGE_ENABLE
</#if>
#define TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUESTS         ${CONFIG_TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUESTS}
#define TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DELAY    ${CONFIG_TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DELAY}
#define TCPIP_STACK_COMMANDS_ICMP_ECHO_TIMEOUT          ${CONFIG_TCPIP_STACK_COMMANDS_ICMP_ECHO_TIMEOUT}
<#if CONFIG_TCPIP_STACK_COMMANDS_WIFI_ENABLE == true>
#define TCPIP_STACK_COMMANDS_WIFI_ENABLE             	true
<#else>
#define TCPIP_STACK_COMMANDS_WIFI_ENABLE             	false
</#if>
#define TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_BUFF_SIZE    ${CONFIG_TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_BUFF_SIZE}
#define TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DATA_SIZE    ${CONFIG_TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DATA_SIZE}
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
