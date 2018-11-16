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
<#if TCPIP_TFTPS_USER_NOTIFICATION == true>
#define TCPIP_TFTPS_USER_NOTIFICATION					true
<#else>
#define TCPIP_TFTPS_USER_NOTIFICATION					false
</#if>

</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
