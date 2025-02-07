<#--
/*******************************************************************************
  TCP Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    tcpip_mac_bridge.h.ftl

  Summary:
    TCPIP MAC Bridge Freemarker Template File

  Description:

*******************************************************************************/
-->

<#--
Copyright (C) 2020-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

<#if TCPIP_STACK_USE_MAC_BRIDGE == true>
/*** Bridge Configuration ***/
#define TCPIP_STACK_USE_MAC_BRIDGE
<#if TCPIP_STACK_MAC_BRIDGE_COMMANDS == true>
#define TCPIP_STACK_MAC_BRIDGE_COMMANDS true
<#else>
#define TCPIP_STACK_MAC_BRIDGE_COMMANDS false
</#if>
#define TCPIP_MAC_BRIDGE_FDB_TABLE_ENTRIES          ${TCPIP_MAC_BRIDGE_FDB_TABLE_ENTRIES}
#define TCPIP_MAC_BRIDGE_MAX_PORTS_NO               ${TCPIP_MAC_BRIDGE_MAX_PORTS_NO}
#define TCPIP_MAC_BRIDGE_PACKET_POOL_SIZE           ${TCPIP_MAC_BRIDGE_PACKET_POOL_SIZE}
#define TCPIP_MAC_BRIDGE_PACKET_SIZE                ${TCPIP_MAC_BRIDGE_PACKET_SIZE}
#define TCPIP_MAC_BRIDGE_PACKET_POOL_REPLENISH      ${TCPIP_MAC_BRIDGE_PACKET_POOL_REPLENISH}
#define TCPIP_MAC_BRIDGE_DCPT_POOL_SIZE             ${TCPIP_MAC_BRIDGE_DCPT_POOL_SIZE}
#define TCPIP_MAC_BRIDGE_DCPT_POOL_REPLENISH        ${TCPIP_MAC_BRIDGE_DCPT_POOL_REPLENISH}
/* Advanced */
#define TCPIP_MAC_BRIDGE_ENTRY_TIMEOUT              ${TCPIP_MAC_BRIDGE_ENTRY_TIMEOUT}
#define TCPIP_MAC_BRIDGE_MAX_TRANSIT_DELAY          ${TCPIP_MAC_BRIDGE_MAX_TRANSIT_DELAY}
#define TCPIP_MAC_BRIDGE_TASK_RATE                  ${TCPIP_MAC_BRIDGE_TASK_RATE}

#define TCPIP_MAC_BRIDGE_STATISTICS          		${TCPIP_MAC_BRIDGE_STATISTICS?c}
#define TCPIP_MAC_BRIDGE_EVENT_NOTIFY          		${TCPIP_MAC_BRIDGE_EVENT_NOTIFY?c}

<#if TCPIP_MAC_BRIDGE_IF_NAME_TABLE == true>
#define TCPIP_MAC_BRIDGE_IF_NAME_TABLE true
<#else>
#define TCPIP_MAC_BRIDGE_IF_NAME_TABLE false
</#if>

#define TCPIP_MC_BRIDGE_INIT_FLAGS                  \
<#if TCPIP_MAC_BRIDGE_NO_DYNAMIC_LEARN == true>
                                                    | TCPIP_MAC_BRIDGE_FLAG_NO_DYNAMIC_LEARN |\
</#if>
<#if TCPIP_MAC_BRIDGE_FDB_ENTRY_EXISTS == true>
                                                    TCPIP_MAC_BRIDGE_FLAG_FDB_ENTRY_EXISTS |\
</#if>
<#if TCPIP_MAC_BRIDGE_IF_NAME_TABLE == true>
                                                    TCPIP_MAC_BRIDGE_FLAG_IF_NAME_TABLE |\
</#if>
                                                    0

<#if TCPIP_STACK_MAC_BRIDGE_DISABLE_GLUE == true>
#define TCPIP_STACK_MAC_BRIDGE_DISABLE_GLUE_PORTS true
<#else>
#define TCPIP_STACK_MAC_BRIDGE_DISABLE_GLUE_PORTS false
</#if>
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
