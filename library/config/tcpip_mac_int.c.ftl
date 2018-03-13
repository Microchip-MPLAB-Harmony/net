<#--
/*******************************************************************************
  TCPIP ETH MAC Interrupt Handler Template File

  File Name:
    tcpip_mac_int.c.ftl

  Summary:
    This file contains source code for interrupt handler.

  Description:
    This file contains source code necessary to initialize the system. It implements
    interrupt calls necessary for TCPIP ETH MAC interrupts in the application.
 *******************************************************************************/

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
<#if CONFIG_DSTBDPIC32CZ == true>
<#if CONFIG_TCPIP_GMAC_INTERRUPT_MODE == true>
void GMAC_Handler(void)
{
	DRV_GMAC_Tasks_ISR((SYS_MODULE_OBJ)0);
}
/* This function is used by ETHMAC driver */
bool SYS_INT_SourceRestore(INT_SOURCE src, int level)
{
    if(level)
    {
        SYS_INT_SourceEnable(src);
    }

    return level;
}
</#if>
<#else>
<#if CONFIG_TCPIP_EMAC_INTERRUPT_MODE == true>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_TCPIP_EMAC_ISR_VECTOR}, IPL${CONFIG_TCPIP_EMAC_INT_PRIO_NUM}SOFT) _IntHandler_ETHMAC(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_TCPIP_EMAC_INT_PRIO_NUM}AUTO), vector(${CONFIG_TCPIP_EMAC_ISR_VECTOR}))) IntHandler_ETHMAC_ISR( void );
</#if>
void IntHandler_ETHMAC(void)
</#if>
<#else>
void __ISR(${CONFIG_TCPIP_EMAC_ISR_VECTOR}, ipl${CONFIG_TCPIP_EMAC_INT_PRIO_NUM}AUTO) _IntHandler_ETHMAC(void)
</#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context save.  */
   _tx_thread_context_save();
</#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
    DRV_ETHMAC_Tasks_ISR((SYS_MODULE_OBJ)0);
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context restore.  */
   _tx_thread_context_restore();
</#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
</#if>
</#if>
}

/* This function is used by ETHMAC driver */
bool SYS_INT_SourceRestore(INT_SOURCE src, int level)
{
    if(level)
    {
        SYS_INT_SourceEnable(src);
    }

    return level;
}

</#if>
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
