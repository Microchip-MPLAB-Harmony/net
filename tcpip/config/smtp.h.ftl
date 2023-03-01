<#--
/*******************************************************************************
  SMTP Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    smtp.h.ftl

  Summary:
    SMTP Freemarker Template File

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

<#if TCPIP_USE_SMTP_CLIENT == true>
/*** SMTP Configuration ***/
#define TCPIP_STACK_USE_SMTP_CLIENT
#define TCPIP_SMTP_SERVER_REPLY_TIMEOUT 	${TCPIP_SMTP_SERVER_REPLY_TIMEOUT}
#define TCPIP_SMTP_WRITE_READY_SPACE 	    ${TCPIP_SMTP_WRITE_READY_SPACE}
#define TCPIP_SMTP_MAX_WRITE_SIZE   	    ${TCPIP_SMTP_MAX_WRITE_SIZE}
#define TCPIP_SMTP_TASK_TICK_RATE			${TCPIP_SMTP_TASK_TICK_RATE}
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
