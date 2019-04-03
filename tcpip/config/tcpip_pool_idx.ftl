<#----------------------------------------------------------------------------
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
---------------------------------------------------------------------------->

config TCPIP_HEAP_POOL_ENTRIES_NUMBER_GT_${INSTANCE+1}
    depends on TCPIP_USE_HEAP && (TCPIP_STACK_USE_HEAP_CONFIG = "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL")
    bool
<#if INSTANCE != 0>
	default n if TCPIP_HEAP_POOL_ENTRIES_NUMBER_GT_${INSTANCE} = n     
</#if>	
	default n if TCPIP_HEAP_POOL_ENTRIES_NUMBER = ${INSTANCE+1}
	default y
	
config TCPIP_HEAP_POOL_ENTRY_NUMBERX${INSTANCE}
    depends on TCPIP_USE_HEAP && (TCPIP_STACK_USE_HEAP_CONFIG = "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL")
<#if INSTANCE != 0>
	             && TCPIP_HEAP_POOL_ENTRIES_NUMBER_GT_${INSTANCE}
</#if>
    bool "Pool Entry ${INSTANCE}"
    default y
    ---help---
    Number of Entries for the Pool Heap
    ---endhelp---

ifblock TCPIP_HEAP_POOL_ENTRY_NUMBERX${INSTANCE}

config TCPIP_HEAP_POOL_ENTRY_SIZE_IDX${INSTANCE}
    int "Size of the Block, Bytes" 
    range 32 4096
    default 2064 if ${INSTANCE} = 0 
    default 1616 if ${INSTANCE} = 1 
    default 1024 if ${INSTANCE} = 2 
    default 768 if ${INSTANCE} = 3
    default 512 if ${INSTANCE} = 4
    default 256 if ${INSTANCE} = 5
    default 128 if ${INSTANCE} = 6
    default 64 if ${INSTANCE} = 7
    default 32 if ${INSTANCE} = 8
    default 64 if ${INSTANCE} = 9
    default 64 if ${INSTANCE} = 10
    default 64 if ${INSTANCE} = 11
    default 64 if ${INSTANCE} = 12
    default 64 if ${INSTANCE} = 13
    default 64 if ${INSTANCE} = 14
    default 64 if ${INSTANCE} = 15
    default 64 if ${INSTANCE} = 16
    default 64 if ${INSTANCE} = 17
    default 64 if ${INSTANCE} = 18
    default 64 if ${INSTANCE} = 19
    ---help---
      Size of the Block
    ---endhelp---
    
config TCPIP_HEAP_POOL_ENTRY_BLOCKS_IDX${INSTANCE}
    int "Number of Pre-allocated Blocks with this Size" 
    range 0 4096
    default 0 if ${INSTANCE} = 0 
    default 0 if ${INSTANCE} = 1 
    default 0 if ${INSTANCE} = 2
    default 0 if ${INSTANCE} = 3
    default 0 if ${INSTANCE} = 4
    default 0 if ${INSTANCE} = 5
    default 0 if ${INSTANCE} = 6
    default 0 if ${INSTANCE} = 7
    default 0 if ${INSTANCE} = 8
    default 0 if ${INSTANCE} = 9
    default 0 if ${INSTANCE} = 10
    default 0 if ${INSTANCE} = 11
    default 0 if ${INSTANCE} = 12
    default 0 if ${INSTANCE} = 13
    default 0 if ${INSTANCE} = 14
    default 0 if ${INSTANCE} = 15
    default 0 if ${INSTANCE} = 16
    default 0 if ${INSTANCE} = 17
    default 0 if ${INSTANCE} = 18
    default 0 if ${INSTANCE} = 19
    ---help---
      Number of blocks for the Entry
    ---endhelp---
    
    
config TCPIP_HEAP_POOL_ENTRY_EXP_BLOCKS_IDX${INSTANCE}
    int "Number of Blocks to Dynamically Expand for this Pool Entry" 
    range 0 4096
    default 2 if ${INSTANCE} = 0 
    default 2 if ${INSTANCE} = 1 
    default 2 if ${INSTANCE} = 2
    default 2 if ${INSTANCE} = 3
    default 2 if ${INSTANCE} = 4
    default 4 if ${INSTANCE} = 5
    default 4 if ${INSTANCE} = 6
    default 8 if ${INSTANCE} = 7
    default 10 if ${INSTANCE} = 8
    default 0 if ${INSTANCE} = 9
    default 0 if ${INSTANCE} = 10
    default 0 if ${INSTANCE} = 11
    default 0 if ${INSTANCE} = 12
    default 0 if ${INSTANCE} = 13
    default 0 if ${INSTANCE} = 14
    default 0 if ${INSTANCE} = 15
    default 0 if ${INSTANCE} = 16
    default 0 if ${INSTANCE} = 17
    default 0 if ${INSTANCE} = 18
    default 0 if ${INSTANCE} = 19
    ---help---
      Number of blocks to expand for the Entry
    ---endhelp---

endif

