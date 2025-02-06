<#--
/*******************************************************************************
  FTP CLIENT Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    ftpc.h.ftl

  Summary:
    FTPC Freemarker Template File

  Description:

 *******************************************************************************/
-->

<#--
Copyright (C) 2019-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

<#if TCPIP_STACK_USE_FTP_CLIENT == true>
/*** FTPC Configuration ***/
#define TCPIP_STACK_USE_FTP_CLIENT
#define TCPIP_FTPC_TASK_TICK_RATE        ${TCPIP_FTPC_TASK_TICK_RATE}
#define TCPIP_FTPC_TMO                   ${TCPIP_FTPC_TMO}
#define TCPIP_FTPC_MAX_NUM_CLIENT        ${TCPIP_FTPC_MAX_NUM_CLIENT}
#define TCPIP_FTPC_DATA_SKT_TX_BUFF_SIZE_DFLT        ${TCPIP_FTPC_DATA_SKT_TX_BUFF_SIZE_DFLT}
#define TCPIP_FTPC_DATA_SKT_RX_BUFF_SIZE_DFLT        ${TCPIP_FTPC_DATA_SKT_RX_BUFF_SIZE_DFLT}
<#if TCPIP_FTPC_COMMANDS == true>
#define TCPIP_FTPC_COMMANDS  
</#if>
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
