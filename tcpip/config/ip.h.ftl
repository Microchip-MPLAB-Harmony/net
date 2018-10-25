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

<#if TCPIP_STACK_USE_IPV4 == true>
/*** IPv4 Configuration ***/
<#if TCPIP_IPV4_FRAGMENTATION == true>
/*** IPv4 Fragmentation Configuration ***/
#define TCPIP_IPV4_FRAGMENTATION		        	true
#define TCPIP_IPV4_FRAGMENT_TIMEOUT		        	${TCPIP_IPV4_FRAGMENT_TIMEOUT}
#define TCPIP_IPV4_FRAGMENT_MAX_STREAMS		       	${TCPIP_IPV4_FRAGMENT_MAX_STREAMS}
#define TCPIP_IPV4_FRAGMENT_MAX_NUMBER		       	${TCPIP_IPV4_FRAGMENT_MAX_NUMBER}
#define TCPIP_IPV4_TASK_TICK_RATE		       	    ${TCPIP_IPV4_TASK_TICK_RATE}
</#if>
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
