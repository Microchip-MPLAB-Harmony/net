<#--
/*******************************************************************************
  TCPIP MAC Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_ppp.h.ftl

  Summary:
    TCPIP MAC Freemarker Template File

  Description:

*******************************************************************************/
-->

<#--
Copyright (C) 2023-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

<#if TCPIP_STACK_USE_PPP_INTERFACE == true>
/*** TCPIP PPP MAC Configuration ***/
#define TCPIP_STACK_USE_PPP_INTERFACE
<#if TCPIP_PPP_HDLC_ENABLE_STATISTICS == true>
#define HDLC_ENABLE_STATISTICS      true
<#else>
#define HDLC_ENABLE_STATISTICS      false
</#if>

<#if TCPIP_PPP_ENABLE_STATISTICS == true>
#define PPP_ENABLE_STATISTICS      true
<#else>
#define PPP_ENABLE_STATISTICS      false
</#if>


#define PPP_MAX_STAT_REGISTRATIONS              ${TCPIP_PPP_MAX_REGISTRATIONS}

<#if TCPIP_PPP_ECHO_ENABLE == true>
#define PPP_ECHO_REQUEST_ENABLE                 true
<#else>
#define PPP_ECHO_REQUEST_ENABLE                 false
</#if>

#define PPP_MAX_ECHO_REQUESTS                   ${TCPIP_PPP_MAX_ECHO_REQUESTS}

<#if TCPIP_PPP_NOTIFICATION_ENABLE == true>
#define PPP_NOTIFICATIONS_ENABLE                    true
<#else>
#define PPP_NOTIFICATIONS_ENABLE                    false
</#if>

#define PPP_MAX_EVENT_REGISTRATIONS             ${TCPIP_PPP_MAX_EVENT_REGISTRATIONS}

#define PPP_MAX_PACKET_SEGMENTS                 ${TCPIP_PPP_MAX_SEGMENTS}

#define TCPIP_IPERF_PPP_TX_QUEUE_LIMIT          ${TCPIP_IPERF_PPP_TX_QUEUE_LIMIT}

<#if TCPIP_PPP_ENABLE_HDLC_COMMANDS == true>
#define TCPIP_STACK_HDLC_COMMANDS                   true
<#else>
#define TCPIP_STACK_HDLC_COMMANDS                   false
</#if>

<#if TCPIP_PPP_ENABLE_COMMANDS == true>
#define TCPIP_STACK_PPP_COMMANDS                    true
<#else>
#define TCPIP_STACK_PPP_COMMANDS                    false
</#if>

#define TCPIP_STACK_COMMANDS_PPP_ECHO_TIMEOUT               ${TCPIP_PPP_ECHO_CMD_TIMEOUT}
#define TCPIP_STACK_COMMANDS_PPP_ECHO_REQUEST_DATA_SIZE     ${TCPIP_PPP_ECHO_CMD_SIZE}
#define TCPIP_STACK_COMMANDS_PPP_ECHO_REQUESTS              ${TCPIP_PPP_ECHO_CMD_REQUESTS}
#define TCPIP_STACK_COMMANDS_PPP_ECHO_REQUEST_DELAY         ${TCPIP_PPP_ECHO_CMD_REQUEST_DELAY}

#define TCPIP_LCP_CONF_FLAGS                       \
<#if TCPIP_LCP_CONF_FLAG_MRU>
                                                    DRV_LCP_OPT_FLAG_MRU |\
</#if>
<#if TCPIP_LCP_CONF_FLAG_MAGIC>
                                                    DRV_LCP_OPT_FLAG_MAGIC_NO |\
</#if>
<#if TCPIP_LCP_CONF_FLAG_ACCM>
                                                    DRV_LCP_OPT_FLAG_ACCM |\
</#if>
                                                    0

#define TCPIP_IPCP_CONF_FLAGS                       \
<#if TCPIP_IPCP_CONF_FLAG_NO_ADDRESS>
                                                    DRV_IPCP_OPT_FLAG_NO_LCL_ADDR4 |\
</#if>
                                                    0
                                                    
#define TCPIP_PPP_CONF_FLAGS                       \
<#if TCPIP_PPP_CONF_FLAG_NO_OPEN>
                                                    DRV_PPP_CONFIG_FLAG_START_NO_OPEN |\
</#if>
<#if TCPIP_PPP_CONF_FLAG_PASSIVE>
                                                    DRV_PPP_CONFIG_FLAG_START_PASSIVE |\
</#if>
<#if TCPIP_PPP_CONF_FLAG_ALWAYS_PASSIVE>
                                                    DRV_PPP_CONFIG_FLAG_ALWAYS_PASSIVE |\
</#if>
                                                    0

<#if DRV_PPP_USE_MAGIC_CALLBACK == true>
#define DRV_PPP_MAGIC_CALLBACK          ${DRV_PPP_MAGIC_CALLBACK}
<#else>
#define DRV_PPP_MAGIC_CALLBACK          0
</#if>

                                                    
#define TCPIP_PPP_RESTART_TMO           ${TCPIP_PPP_RESTART_TMO}
#define TCPIP_PPP_MAX_TERM              ${TCPIP_PPP_MAX_TERM}
#define TCPIP_PPP_MAX_CONFIGURE         ${TCPIP_PPP_MAX_CONFIGURE}
#define TCPIP_PPP_MAX_FAILURE           ${TCPIP_PPP_MAX_FAILURE}
#define TCPIP_PPP_RX_DEDICATED_BUFFERS  ${TCPIP_PPP_RX_DEDICATED_BUFFERS}
#define TCPIP_PPP_RX_INIT_BUFFERS       ${TCPIP_PPP_RX_INIT_BUFFERS}
#define TCPIP_PPP_RX_LOW_THRESHOLD      ${TCPIP_PPP_RX_LOW_THRESHOLD}
#define TCPIP_PPP_RX_LOW_FILL           ${TCPIP_PPP_RX_LOW_FILL}
#define TCPIP_PPP_MAX_RECEIVE_UNIT      ${TCPIP_PPP_MAX_RECEIVE_UNIT}
#define TCPIP_PPP_ECHO_TMO              ${TCPIP_PPP_ECHO_TMO}
#define TCPIP_PPP_LOCAL_IPV4_ADDRESS    "${TCPIP_PPP_LOCAL_IPV4_ADDRESS}"
#define TCPIP_PPP_PEER_IPV4_ADDRESS     "${TCPIP_PPP_PEER_IPV4_ADDRESS}"
#define TCPIP_PPP_RX_ACCM               ${TCPIP_PPP_RX_ACCM}
#define DRV_PPP_HDLC_DRIVER_OBJECT      ${DRV_PPP_HDLC_DRIVER_OBJECT}
#define TCPIP_HDLC_PROC_BUFFER_SIZE     ${TCPIP_HDLC_PROC_BUFFER_SIZE}
#define TCPIP_HDLC_PEEK_BUFFER_SIZE     ${TCPIP_HDLC_PEEK_BUFFER_SIZE}
#define TCPIP_HDLC_OBJECT_INDEX         ${TCPIP_HDLC_OBJECT_INDEX}


</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
