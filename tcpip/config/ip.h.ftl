<#--
/*******************************************************************************
  TCP Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    tcp.h.ftl

  Summary:
    TCP Freemarker Template File

  Description:

*******************************************************************************/
-->

<#--
Copyright (C) 2014-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

<#if TCPIP_STACK_USE_IPV4 == true>
/*** IPv4 Configuration ***/
#define TCPIP_IPV4_ARP_SLOTS                        ${TCPIP_IPV4_ARP_QUEUE_SLOTS}
<#if TCPIP_IPV4_FRAGMENTATION == true>
/*** IPv4 Fragmentation Configuration ***/
#define TCPIP_IPV4_FRAGMENTATION		        	true
#define TCPIP_IPV4_FRAGMENT_TIMEOUT		        	${TCPIP_IPV4_FRAGMENT_TIMEOUT}
#define TCPIP_IPV4_FRAGMENT_MAX_STREAMS		       	${TCPIP_IPV4_FRAGMENT_MAX_STREAMS}
#define TCPIP_IPV4_FRAGMENT_MAX_NUMBER		       	${TCPIP_IPV4_FRAGMENT_MAX_NUMBER}
#define TCPIP_IPV4_TASK_TICK_RATE		       	    ${TCPIP_IPV4_TASK_TICK_RATE}
</#if>
<#if TCPIP_IPV4_EXTERN_PACKET_PROCESS == true>
#define TCPIP_IPV4_EXTERN_PACKET_PROCESS   true
<#else>
#define TCPIP_IPV4_EXTERN_PACKET_PROCESS   false
</#if>

<#if TCPIP_IPV4_COMMANDS_ENABLE == true>
#define TCPIP_IPV4_COMMANDS true
<#else>
#define TCPIP_IPV4_COMMANDS false
</#if>

<#if TCPIP_IPV4_FORWARD == true>
#define TCPIP_IPV4_FORWARDING_ENABLE    true 

#define TCPIP_IPV4_FWD_TX_SLOTS                        ${TCPIP_IPV4_FWD_QUEUE_SLOTS}
<#if TCPIP_IPV4_FORWARD_ASCII_ENABLE == true>
#define TCPIP_IPV4_FORWARDING_TABLE_ASCII     true
<#else>
#define TCPIP_IPV4_FORWARDING_TABLE_ASCII     false
</#if>
<#if TCPIP_IPV4_FORWARD_DYN_ENABLE == true>
#define TCPIP_IPV4_FORWARDING_DYNAMIC_API     true
<#else>
#define TCPIP_IPV4_FORWARDING_DYNAMIC_API     false
</#if>

#define TCPIP_IPV4_FWD_FLAGS                        \
<#if TCPIP_IPV4_FORWARD_ASCII_SELECT == true>
                                                    TCPIP_IPV4_FWD_FLAG_ASCII_TABLE |\
</#if>
<#if TCPIP_IPV4_FORWARD_BCAST_ENABLE == true>
                                                    TCPIP_IPV4_FWD_FLAG_BCAST_ENABLED |\
</#if>
                                                    TCPIP_IPV4_FWD_FLAG_ENABLED

#define TCPIP_IPV4_FORWARDING_TABLE_MAX_SIZE        ${TCPIP_IPV4_FORWARD_TABLE_MAX_SIZE}
#define TCPIP_IPV4_FORWARDING_TABLE_ENTRIES         ${TCPIP_IPV4_FORWARD_TABLE_CNT}

<#if TCPIP_IPV4_FORWARD_STATS_ENABLE == true>
#define TCPIP_IPV4_FORWARDING_STATS     true
<#else>
#define TCPIP_IPV4_FORWARDING_STATS     false
</#if>

<#else>
#define TCPIP_IPV4_FORWARDING_ENABLE    false 
</#if>


</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
