<#--
/*******************************************************************************
  DHCP Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    dhcp.h.ftl

  Summary:
    DHCP Freemarker Template File

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
<#if CONFIG_TCPIP_STACK_USE_DHCP_CLIENT == true>
/*** DHCP Configuration ***/
#define TCPIP_STACK_USE_DHCP_CLIENT
#define TCPIP_DHCP_TIMEOUT                          ${CONFIG_TCPIP_DHCP_TIMEOUT}
#define TCPIP_DHCP_TASK_TICK_RATE                   ${CONFIG_TCPIP_DHCP_TASK_TICK_RATE}
#define TCPIP_DHCP_HOST_NAME_SIZE                   ${CONFIG_TCPIP_DHCP_HOST_NAME_SIZE}
#define TCPIP_DHCP_CLIENT_CONNECT_PORT              ${CONFIG_TCPIP_DHCP_CLIENT_CONNECT_PORT}
#define TCPIP_DHCP_SERVER_LISTEN_PORT               ${CONFIG_TCPIP_DHCP_SERVER_LISTEN_PORT}
<#if CONFIG_TCPIP_DHCP_CLIENT_ENABLED == true>
#define TCPIP_DHCP_CLIENT_ENABLED                   true
<#else>
#define TCPIP_DHCP_CLIENT_ENABLED                   false
</#if>

</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
