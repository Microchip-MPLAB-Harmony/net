<#--
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
 -->
<#--
// tcpip_stack.c.ftl: TCP/IP module Initialization Calls
-->

<#if CONFIG_USE_TCPIP_STACK == true>
<#if CONFIG_DSTBDPIC32CZ == true>	
<#if CONFIG_TCPIP_GMAC_INTERRUPT_MODE == true>
    /* set priority for ETHERNET interrupt source */
    SYS_INT_VectorPrioritySet(${CONFIG_DRV_GMAC_INTERRUPT_VECTOR}, ${CONFIG_TCPIP_GMAC_INTERRUPT_PRIORITY});
</#if>  	
<#else>
<#if CONFIG_TCPIP_EMAC_INTERRUPT_MODE == true>
    /* set priority for ETHERNET interrupt source */
    SYS_INT_VectorPrioritySet(INT_VECTOR_ETH, ${CONFIG_TCPIP_EMAC_INTERRUPT_PRIORITY});

    /* set sub-priority for ETHERNET interrupt source */
    SYS_INT_VectorSubprioritySet(INT_VECTOR_ETH, ${CONFIG_TCPIP_EMAC_INTERRUPT_SUB_PRIORITY});
</#if>    
</#if>      
<#if CONFIG_TCPIP_STACK_USE_COMMANDS == true && CONFIG_USE_SYS_COMMAND == false>
    if (!SYS_CMD_Initialize())
    {
        return;
    }
</#if>
    /* TCPIP Stack Initialization */
    sysObj.tcpip = TCPIP_STACK_Init();
    SYS_ASSERT(sysObj.tcpip != SYS_MODULE_OBJ_INVALID, "TCPIP_STACK_Init Failed" );
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->

