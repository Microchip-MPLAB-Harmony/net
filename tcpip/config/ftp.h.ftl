<#--
/*******************************************************************************
  FTP Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    ftp.h.ftl

  Summary:
    FTP Freemarker Template File

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

<#if TCPIP_USE_FTP_MODULE == true>
/*** FTP Configuration ***/
#define TCPIP_STACK_USE_FTP_SERVER
#define TCPIP_FTP_USER_NAME_LEN                 ${TCPIP_FTP_USER_NAME_LEN}
#define TCPIP_FTP_MAX_CONNECTIONS               ${TCPIP_FTP_MAX_CONNECTIONS}
#define TCPIP_FTP_DATA_SKT_TX_BUFF_SIZE         ${TCPIP_FTP_DATA_SKT_TX_BUFF_SIZE}
#define TCPIP_FTP_DATA_SKT_RX_BUFF_SIZE         ${TCPIP_FTP_DATA_SKT_RX_BUFF_SIZE}
#define TCPIP_FTPS_TASK_TICK_RATE               ${TCPIP_FTPS_TASK_TICK_RATE}
#define TCPIP_FTP_TIMEOUT                       ${TCPIP_FTP_TIMEOUT}

#define TCPIP_FTPS_COMMAND_LISTEN_PORT          ${TCPIP_FTPS_COMMAND_LISTEN_PORT}
#define TCPIP_FTPS_DATA_LISTEN_PORT             ${TCPIP_FTPS_DATA_LISTEN_PORT}
<#if TCPIP_FTP_MOUNT_POINT?has_content>
#define TCPIP_FTP_MOUNT_POINT                   "${TCPIP_FTP_MOUNT_POINT}"
<#else>
#define TCPIP_FTP_MOUNT_POINT                   "\"
</#if>

/***Comment this line out to disable MPFS***/
<#if TCPIP_FTP_PUT_ENABLED == true>
#define TCPIP_FTP_PUT_ENABLED   
</#if>

<#if TCPIP_FTPS_AUTH_CONFIG == "Run Time Authentication">
#define TCPIP_FTPS_OBSOLETE_AUTHENTICATION false
<#if TCPIP_FTPS_AUTHENTICATION_CONN_INFO>
#define TCPIP_FTPS_AUTHENTICATION_CONN_INFO true
<#else>
#define TCPIP_FTPS_AUTHENTICATION_CONN_INFO false
</#if>
<#else>
#define TCPIP_FTPS_OBSOLETE_AUTHENTICATION true
#define TCPIP_FTP_USER_NAME                     "${TCPIP_FTP_USER_NAME}"
#define TCPIP_FTP_PASSWORD                      "${TCPIP_FTP_PASSWORD}"
#define TCPIP_FTP_PASSWD_LEN                    ${TCPIP_FTP_PASSWD_LEN}
</#if>

</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
