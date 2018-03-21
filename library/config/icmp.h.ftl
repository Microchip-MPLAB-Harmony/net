<#--
/*******************************************************************************
  ICMP Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    icmp.h.ftl

  Summary:
    ICMP Freemarker Template File

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
<#if TCPIP_STACK_USE_ICMPV4 == true>
<#if TCPIP_STACK_USE_ICMP_SERVER == true>
/*** ICMPv4 Server Configuration ***/
#define TCPIP_STACK_USE_ICMP_SERVER
</#if>

<#if TCPIP_STACK_USE_ICMP_CLIENT == true>
/*** ICMPv4 Client Configuration ***/
#define TCPIP_STACK_USE_ICMP_CLIENT
<#if TCPIP_ICMP_CLIENT_USER_NOTIFICATION == true>
#define TCPIP_ICMP_CLIENT_USER_NOTIFICATION   true
<#else>
#define TCPIP_ICMP_CLIENT_USER_NOTIFICATION   false
</#if>
#define TCPIP_ICMP_ECHO_REQUEST_TIMEOUT       ${TCPIP_ICMP_ECHO_REQUEST_TIMEOUT}
#define TCPIP_ICMP_TASK_TICK_RATE             ${TCPIP_ICMP_TASK_TICK_RATE}
</#if>
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
