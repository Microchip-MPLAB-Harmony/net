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

<#----------------------------------------------------------------------------
 Copyright (C) 2014-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
----------------------------------------------------------------------------->

<#if TCPIP_USE_HEAP == true>
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
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
