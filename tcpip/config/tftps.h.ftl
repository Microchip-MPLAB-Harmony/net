<#--
/*******************************************************************************
  TFTP Server Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    tftps.h.ftl

  Summary:
    TFTP Server Freemarker Template File

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

<#if TCPIP_USE_TFTPS_MODULE == true>
/*** TFTP Server Configuration ***/
#define TCPIP_STACK_USE_TFTP_SERVER
#define TCPIP_TFTPS_TASK_TICK_RATE              		${TCPIP_TFTPS_TASK_TICK_RATE}
#define TCPIP_TFTPS_CMD_PROCESS_TIMEOUT           		${TCPIP_TFTPS_CMD_PROCESS_TIMEOUT}
#define TCPIP_TFTPS_RETRANSMIT_COUNT					${TCPIP_TFTPS_RETRANSMIT_COUNT}
#define TCPIP_TFTPS_CLIENT_NUMBER                		${TCPIP_TFTPS_CLIENT_NUMBER}
#define TCPIP_TFTPS_DEFAULT_FILESIZE         			${TCPIP_TFTPS_DEFAULT_FILESIZE}
#define TCPIP_TFTPS_DEFAULT_BLOCK_SIZE                  ${TCPIP_TFTPS_DEFAULT_BLOCK_SIZE}
#define TCPIP_TFTPS_DEFAULT_TIMEOUT                 	${TCPIP_TFTPS_DEFAULT_TIMEOUT}
#define TCPIP_TFTPS_FILENAME_LEN						${TCPIP_TFTPS_FILENAME_LEN}
<#if TCPIP_TFTPS_CONSOLE_CMD == true>
#define TCPIP_TFTPS_CONSOLE_CMD               	true
<#else>
#define TCPIP_TFTPS_CONSOLE_CMD              	false
</#if>
<#if TCPIP_TFTPS_USER_NOTIFICATION == true>
#define TCPIP_TFTPS_USER_NOTIFICATION					true
<#else>
#define TCPIP_TFTPS_USER_NOTIFICATION					false
</#if>
<#if TCPIP_TFTPS_MOUNT_POINT?has_content>
#define TCPIP_TFTPS_MOUNT_POINT							"${TCPIP_TFTPS_MOUNT_POINT}"
<#else>
#define TCPIP_TFTPS_MOUNT_POINT 							"\"
</#if>
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
