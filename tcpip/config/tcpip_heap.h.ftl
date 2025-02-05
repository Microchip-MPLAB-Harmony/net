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
-->

<#--
Copyright (C) 2014-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
-->

/*** TCPIP Heap Configuration ***/
<#if TCPIP_STACK_USE_HEAP_CONFIG == "TCPIP_STACK_HEAP_TYPE_EXTERNAL_HEAP">
#define TCPIP_STACK_USE_EXTERNAL_HEAP
<#else>
<#if TCPIP_STACK_USE_HEAP_CONFIG == "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP">
#define TCPIP_STACK_USE_INTERNAL_HEAP
#define TCPIP_STACK_DRAM_SIZE                       ${TCPIP_STACK_DRAM_SIZE}
<#elseif TCPIP_STACK_USE_HEAP_CONFIG == "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL">
#define TCPIP_STACK_USE_INTERNAL_HEAP_POOL
#define TCPIP_HEAP_POOL_ENTRIES_NUMBER          ${TCPIP_HEAP_POOL_ENTRIES_NUMBER}
#define TCPIP_STACK_INTERNAL_HEAP_POOL_EXPANSION_SIZE ${TCPIP_STACK_POOL_EXPANSION_SIZE}
</#if>
#define TCPIP_STACK_DRAM_RUN_LIMIT                  ${TCPIP_STACK_DRAM_RUN_LIMIT}
</#if>

<#if TCPIP_STACK_MALLOC_FUNC?has_content>
#define TCPIP_STACK_MALLOC_FUNC                     ${TCPIP_STACK_MALLOC_FUNC}
</#if>

<#if TCPIP_STACK_CALLOC_FUNC?has_content>
#define TCPIP_STACK_CALLOC_FUNC                     ${TCPIP_STACK_CALLOC_FUNC}
</#if>

<#if TCPIP_STACK_FREE_FUNC?has_content>
#define TCPIP_STACK_FREE_FUNC                       ${TCPIP_STACK_FREE_FUNC}
</#if>


<#if TCPIP_STACK_DRAM_DEBUG_ENABLE == true>
#define TCPIP_STACK_DRAM_DEBUG_ENABLE
</#if>
<#if TCPIP_STACK_DRAM_TRACE_ENABLE == true>
#define TCPIP_STACK_DRAM_TRACE_ENABLE
#define TCPIP_STACK_DRAM_TRACE_SLOTS                 ${TCPIP_STACK_DRAM_TRACE_SLOTS}
</#if>

#define TCPIP_STACK_HEAP_USE_FLAGS                   ${TCPIP_STACK_HEAP_USE_FLAGS}

#define TCPIP_STACK_HEAP_USAGE_CONFIG                ${TCPIP_STACK_HEAP_USAGE_CONFIG}

#define TCPIP_STACK_SUPPORTED_HEAPS                  ${TCPIP_STACK_SUPPORTED_HEAPS}

<#--
/*******************************************************************************
 End of File
*/
-->
