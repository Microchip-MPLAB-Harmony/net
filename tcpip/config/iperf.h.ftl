<#--
/*******************************************************************************
  iperf Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    iperf.h.ftl

  Summary:
    iperf Freemarker Template File

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

<#if TCPIP_USE_IPERF == true>
/*** iperf Configuration ***/
#define TCPIP_STACK_USE_IPERF
#define TCPIP_IPERF_TX_BUFFER_SIZE		${TCPIP_IPERF_TX_BUFFER_SIZE}
#define TCPIP_IPERF_RX_BUFFER_SIZE  	${TCPIP_IPERF_RX_BUFFER_SIZE}
#define TCPIP_IPERF_TX_WAIT_TMO     	${TCPIP_IPERF_TX_WAIT_TMO}
#define TCPIP_IPERF_TX_QUEUE_LIMIT  	${TCPIP_IPERF_TX_QUEUE_LIMIT}
#define TCPIP_IPERF_TIMING_ERROR_MARGIN ${TCPIP_IPERF_TIMING_ERROR_MARGIN}
#define TCPIP_IPERF_MAX_INSTANCES       ${TCPIP_IPERF_MAX_INSTANCES}
#define TCPIP_IPERF_TX_BW_LIMIT  		${TCPIP_IPERF_TX_BW_LIMIT}
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
