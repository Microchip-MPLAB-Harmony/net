<#--
/*******************************************************************************
  iperf Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    iperf.h.ftl

  Summary:
    iperf Freemarker Template File

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
<#if CONFIG_TCPIP_USE_IPERF == true>
/*** iperf Configuration ***/
#define TCPIP_STACK_USE_IPERF
#define TCPIP_IPERF_TX_BUFFER_SIZE		${CONFIG_TCPIP_IPERF_TX_BUFFER_SIZE}
#define TCPIP_IPERF_RX_BUFFER_SIZE  	${CONFIG_TCPIP_IPERF_RX_BUFFER_SIZE}
#define TCPIP_IPERF_TX_WAIT_TMO     	${CONFIG_TCPIP_IPERF_TX_WAIT_TMO}
#define TCPIP_IPERF_TX_QUEUE_LIMIT  	${CONFIG_TCPIP_IPERF_TX_QUEUE_LIMIT}
#define TCPIP_IPERF_TIMING_ERROR_MARGIN ${CONFIG_TCPIP_IPERF_TIMING_ERROR_MARGIN}
#define TCPIP_IPERF_MAX_INSTANCES       ${CONFIG_TCPIP_IPERF_MAX_INSTANCES}
#define TCPIP_IPERF_TX_BW_LIMIT  		${CONFIG_TCPIP_IPERF_TX_BW_LIMIT}
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
