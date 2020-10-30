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

<#----------------------------------------------------------------------------
 Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.

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

<#if TCPIP_STACK_USE_MAC_BRIDGE == true>
/*** Bridge Configuration ***/
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

</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
