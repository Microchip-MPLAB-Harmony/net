<#--
/*******************************************************************************
  TFTP Server Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    tftps.h.ftl

  Summary:
    TFTP Server Freemarker Template File

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
<#if CONFIG_TCPIP_USE_TFTPS_MODULE == true>
/*** TFTP Server Configuration ***/
#define TCPIP_STACK_USE_TFTP_SERVER
#define TCPIP_TFTP_SERVER_TMO           				${CONFIG_TCPIP_TFTP_SERVER_TMO}
#define TCPIP_TFTPS_CLIENT_NUMBER                		${CONFIG_TCPIP_TFTPS_CLIENT_NUMBER}
#define TCPIP_TFTPS_RETRANSMIT_COUNT					${CONFIG_TCPIP_TFTPS_RETRANSMIT_COUNT}
#define TCPIP_TFTPS_TASK_TICK_RATE              		${CONFIG_TCPIP_TFTPS_TASK_TICK_RATE}
#define TCPIP_TFTPS_DEFAULT_FILESIZE         			${CONFIG_TCPIP_TFTPS_DEFAULT_FILESIZE}
#define TCPIP_TFTPS_DEFAULT_BLOCK_SIZE                  ${CONFIG_TCPIP_TFTPS_DEFAULT_BLOCK_SIZE}
#define TCPIP_TFTPS_DEFAULT_TIMEOUT                 	${CONFIG_TCPIP_TFTPS_DEFAULT_TIMEOUT}
<#if CONFIG_TCPIP_TFTPS_USER_NOTIFICATION == true>
#define TCPIP_TFTPS_USER_NOTIFICATION					true
<#else>
#define TCPIP_TFTPS_USER_NOTIFICATION					false
</#if>

</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
