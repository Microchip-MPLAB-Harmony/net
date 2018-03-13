<#--
/*******************************************************************************
  TCPIP Stack Heap Configuration Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    tcpip_heap.h.ftl

  Summary:
    TCPIP Stack heap configuration Freemarker Template File

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
<#if CONFIG_TCPIP_USE_HEAP == true>
/*** TCPIP Heap Configuration ***/

<#if CONFIG_TCPIP_STACK_USE_HEAP_CONFIG == "TCPIP_STACK_HEAP_TYPE_EXTERNAL_HEAP">
#define TCPIP_STACK_USE_EXTERNAL_HEAP
<#else>
<#if CONFIG_TCPIP_STACK_USE_HEAP_CONFIG == "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP">
#define TCPIP_STACK_USE_INTERNAL_HEAP
#define TCPIP_STACK_DRAM_SIZE                       ${CONFIG_TCPIP_STACK_DRAM_SIZE}
<#elseif CONFIG_TCPIP_STACK_USE_HEAP_CONFIG == "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL">
#define TCPIP_STACK_USE_INTERNAL_HEAP_POOL
#define TCPIP_HEAP_POOL_ENTRIES_NUMBER          ${CONFIG_TCPIP_HEAP_POOL_ENTRIES_NUMBER}
#define TCPIP_STACK_INTERNAL_HEAP_POOL_EXPANSION_SIZE ${CONFIG_TCPIP_STACK_POOL_EXPANSION_SIZE}
</#if>
#define TCPIP_STACK_DRAM_RUN_LIMIT                  ${CONFIG_TCPIP_STACK_DRAM_RUN_LIMIT}
</#if>

<#if CONFIG_TCPIP_STACK_MALLOC_FUNC?has_content>
#define TCPIP_STACK_MALLOC_FUNC                     ${CONFIG_TCPIP_STACK_MALLOC_FUNC}
</#if>

<#if CONFIG_TCPIP_STACK_CALLOC_FUNC?has_content>
#define TCPIP_STACK_CALLOC_FUNC                     ${CONFIG_TCPIP_STACK_CALLOC_FUNC}
</#if>

<#if CONFIG_TCPIP_STACK_FREE_FUNC?has_content>
#define TCPIP_STACK_FREE_FUNC                       ${CONFIG_TCPIP_STACK_FREE_FUNC}
</#if>


<#if CONFIG_TCPIP_STACK_DRAM_DEBUG_ENABLE == true>
#define TCPIP_STACK_DRAM_DEBUG_ENABLE
</#if>
<#if CONFIG_TCPIP_STACK_DRAM_TRACE_ENABLE == true>
#define TCPIP_STACK_DRAM_TRACE_ENABLE
#define TCPIP_STACK_DRAM_TRACE_SLOTS                 ${CONFIG_TCPIP_STACK_DRAM_TRACE_SLOTS}
</#if>

#define TCPIP_STACK_HEAP_USE_FLAGS                   ${CONFIG_TCPIP_STACK_HEAP_USE_FLAGS}

#define TCPIP_STACK_HEAP_USAGE_CONFIG                ${CONFIG_TCPIP_STACK_HEAP_USAGE_CONFIG}

#define TCPIP_STACK_SUPPORTED_HEAPS                  ${CONFIG_TCPIP_STACK_SUPPORTED_HEAPS}
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
