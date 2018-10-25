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

<#if TCPIP_USE_FTP_MODULE == true>
/*** FTP Configuration ***/
#define TCPIP_STACK_USE_FTP_SERVER
#define TCPIP_FTP_USER_NAME_LEN		    			${TCPIP_FTP_USER_NAME_LEN}
#define TCPIP_FTP_PASSWD_LEN		    			${TCPIP_FTP_PASSWD_LEN}
#define TCPIP_FTP_MAX_CONNECTIONS				${TCPIP_FTP_MAX_CONNECTIONS}
#define TCPIP_FTP_DATA_SKT_TX_BUFF_SIZE				${TCPIP_FTP_DATA_SKT_TX_BUFF_SIZE}
#define TCPIP_FTP_DATA_SKT_RX_BUFF_SIZE				${TCPIP_FTP_DATA_SKT_RX_BUFF_SIZE}
#define TCPIP_FTPS_TASK_TICK_RATE	    			${TCPIP_FTPS_TASK_TICK_RATE}
#define TCPIP_FTP_USER_NAME		        		"${TCPIP_FTP_USER_NAME}"
#define TCPIP_FTP_PASSWORD		        		"${TCPIP_FTP_PASSWORD}"
#define TCPIP_FTP_TIMEOUT						${TCPIP_FTP_TIMEOUT}

/***Comment this line out to disable MPFS***/
<#if TCPIP_FTP_PUT_ENABLED == true>
#define TCPIP_FTP_PUT_ENABLED	
</#if>
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
