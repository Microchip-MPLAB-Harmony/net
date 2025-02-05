<#--
/*******************************************************************************
  DHCP Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    dhcp.h.ftl

  Summary:
    DHCP Freemarker Template File

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

<#if TCPIP_STACK_USE_DHCP_CLIENT == true>
/*** DHCP Configuration ***/
#define TCPIP_STACK_USE_DHCP_CLIENT
#define TCPIP_DHCP_TIMEOUT                          ${TCPIP_DHCP_TIMEOUT}
#define TCPIP_DHCP_TASK_TICK_RATE                   ${TCPIP_DHCP_TASK_TICK_RATE}
#define TCPIP_DHCP_HOST_NAME_SIZE                   ${TCPIP_DHCP_HOST_NAME_SIZE}
#define TCPIP_DHCP_CLIENT_CONNECT_PORT              ${TCPIP_DHCP_CLIENT_CONNECT_PORT}
#define TCPIP_DHCP_SERVER_LISTEN_PORT               ${TCPIP_DHCP_SERVER_LISTEN_PORT}
<#if TCPIP_DHCP_CLIENT_CONSOLE_CMD == true>
#define TCPIP_DHCP_CLIENT_CONSOLE_CMD               true
<#else>
#define TCPIP_DHCP_CLIENT_CONSOLE_CMD              	false
</#if>

#define TCPIP_DHCP_USE_OPTION_TIME_SERVER           ${TCPIP_DHCP_OPTION_TIME_SERVER_MAX}
#define TCPIP_DHCP_TIME_SERVER_ADDRESSES            ${TCPIP_DHCP_OPTION_TIME_SERVER_MAX}
#define TCPIP_DHCP_USE_OPTION_NTP_SERVER            ${TCPIP_DHCP_OPTION_NTP_SERVER_MAX}
#define TCPIP_DHCP_NTP_SERVER_ADDRESSES             ${TCPIP_DHCP_OPTION_NTP_SERVER_MAX}
#define TCPIP_DHCP_ARP_LEASE_CHECK_TMO              ${TCPIP_DHCP_ARP_LEASE_CHECK_TMO}
#define TCPIP_DHCP_WAIT_ARP_FAIL_CHECK_TMO          ${TCPIP_DHCP_WAIT_ARP_FAIL_CHECK_TMO}

</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
