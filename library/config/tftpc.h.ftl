<#--
/*******************************************************************************
  TFTP Client Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    tftpc.h.ftl

  Summary:
    TFTP Client Freemarker Template File

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
<#if CONFIG_TCPIP_USE_TFTPC_MODULE == true>
/*** TFTP Client Configuration ***/
#define TCPIP_STACK_USE_TFTP_CLIENT
#define TCPIP_TFTPC_DEFAULT_IF                  "${CONFIG_TCPIP_TFTPC_DEFAULT_IF}"
#define TCPIP_TFTPC_SERVERADDRESS_LEN           ${CONFIG_TCPIP_TFTPC_SERVERADDRESS_LEN}
#define TCPIP_TFTPC_FILENAME_LEN                ${CONFIG_TCPIP_TFTPC_FILENAME_LEN}
<#if CONFIG_TCPIP_TFTPC_USER_NOTIFICATION == true>
#define TCPIP_TFTPC_USER_NOTIFICATION   true
<#else>
#define TCPIP_TFTPC_USER_NOTIFICATION   false
</#if>
#define TCPIP_TFTPC_TASK_TICK_RATE              ${CONFIG_TCPIP_TFTPC_TASK_TICK_RATE}
#define TCPIP_TFTPC_CMD_PROCESS_TIMEOUT         ${CONFIG_TCPIP_TFTPC_CMD_PROCESS_TIMEOUT}
#define TCPIP_TFTPC_ARP_TIMEOUT                 ${CONFIG_TCPIP_TFTPC_ARP_TIMEOUT}
#define TCPIP_TFTPC_MAX_RETRIES                 ${CONFIG_TCPIP_TFTPC_MAX_RETRIES}

</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
