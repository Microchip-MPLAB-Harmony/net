<#--
/*******************************************************************************
  TCPIP MAC Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    tcpip_mac_pic32c.h.ftl

  Summary:
    TCPIP MAC Freemarker Template File

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
<#if TCPIP_USE_ETH_MAC == true>
/*** TCPIP MAC Configuration ***/
#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY				${TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY}
#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY				${TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY}
#define TCPIP_GMAC_RX_BUFF_SIZE_DUMMY				    	${TCPIP_GMAC_RX_BUFF_SIZE_DUMMY}
#define TCPIP_GMAC_TX_BUFF_SIZE_DUMMY				    	${TCPIP_GMAC_TX_BUFF_SIZE_DUMMY}

<#if (TCPIP_GMAC_QUEUE_0)?has_content>
	<#if TCPIP_GMAC_QUEUE_0 == true>
		/*** QUEUE 0 TX Configuration ***/
		<#if TCPIP_GMAC_TX_EN_QUE0 == true>
			<#lt>#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE0				${TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE0}
			<#lt>#define TCPIP_GMAC_TX_BUFF_SIZE_QUE0				    	${TCPIP_GMAC_TX_BUFF_SIZE_QUE0}			
		<#else>
			/*** Dummy TX Configuration ***/
			<#lt>#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE0				TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY
			<#lt>#define TCPIP_GMAC_TX_BUFF_SIZE_QUE0				    	TCPIP_GMAC_TX_BUFF_SIZE_DUMMY
		</#if>
		<#lt>#define TCPIP_GMAC_MAX_TX_PKT_SIZE_QUE0				    	${TCPIP_GMAC_MAX_TX_PKT_SIZE_QUE0}
		
		/*** QUEUE 0 RX Configuration ***/
		<#if TCPIP_GMAC_RX_EN_QUE0 == true>
			<#lt>#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE0				${TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE0}
			<#lt>#define TCPIP_GMAC_RX_BUFF_SIZE_QUE0				    	${TCPIP_GMAC_RX_BUFF_SIZE_QUE0}
			<#lt>#define TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE0				${TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE0}
			<#lt>#define TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE0				   	${TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE0}
			<#lt>#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE0			${TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE0}
			<#lt>#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE0				${TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE0}
		<#else>
			/*** Dummy RX Configuration ***/
			<#lt>#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE0				TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY
			<#lt>#define TCPIP_GMAC_RX_BUFF_SIZE_QUE0				    	TCPIP_GMAC_RX_BUFF_SIZE_DUMMY
			<#lt>#define TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE0				1
			<#lt>#define TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE0				   	0
			<#lt>#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE0			0
			<#lt>#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE0				0
		</#if>
	<#else>
		/*** Dummy TX Configuration ***/
		<#lt>#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE0					TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY
		<#lt>#define TCPIP_GMAC_TX_BUFF_SIZE_QUE0				    		TCPIP_GMAC_TX_BUFF_SIZE_DUMMY
		<#lt>#define TCPIP_GMAC_MAX_TX_PKT_SIZE_QUE0				    	0
		/*** Dummy RX Configuration ***/
		<#lt>#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE0					TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY
		<#lt>#define TCPIP_GMAC_RX_BUFF_SIZE_QUE0				    		TCPIP_GMAC_RX_BUFF_SIZE_DUMMY
		<#lt>#define TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE0					1
		<#lt>#define TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE0				   		0
		<#lt>#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE0				0
		<#lt>#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE0					0
	</#if>
</#if>


<#if (TCPIP_GMAC_QUEUE_1)?has_content>
	<#if TCPIP_GMAC_QUEUE_1 == true>
		/*** QUEUE 1 TX Configuration ***/
		<#if TCPIP_GMAC_TX_EN_QUE1 == true>
			<#lt>#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE1				${TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE1}
			<#lt>#define TCPIP_GMAC_TX_BUFF_SIZE_QUE1				    	${TCPIP_GMAC_TX_BUFF_SIZE_QUE1}
		<#else>
			/*** Dummy TX Configuration ***/
			<#lt>#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE1				TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY
			<#lt>#define TCPIP_GMAC_TX_BUFF_SIZE_QUE1				    	TCPIP_GMAC_TX_BUFF_SIZE_DUMMY
		</#if>
		<#lt>#define TCPIP_GMAC_MAX_TX_PKT_SIZE_QUE1				    	${TCPIP_GMAC_MAX_TX_PKT_SIZE_QUE1}

		/*** QUEUE 1 RX Configuration ***/
		<#if TCPIP_GMAC_RX_EN_QUE1 == true>
			<#lt>#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE1				${TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE1}
			<#lt>#define TCPIP_GMAC_RX_BUFF_SIZE_QUE1				    	${TCPIP_GMAC_RX_BUFF_SIZE_QUE1}
			<#lt>#define TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE1				${TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE1}
			<#lt>#define TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE1				   	${TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE1}
			<#lt>#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE1			${TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE1}
			<#lt>#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE1				${TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE1}
		<#else>
			/*** Dummy RX Configuration ***/
			<#lt>#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE1				TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY
			<#lt>#define TCPIP_GMAC_RX_BUFF_SIZE_QUE1				    	TCPIP_GMAC_RX_BUFF_SIZE_DUMMY
			<#lt>#define TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE1				1
			<#lt>#define TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE1				   	0
			<#lt>#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE1			0
			<#lt>#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE1				0
		</#if>
	<#else>
		/*** Dummy TX Configuration ***/
		<#lt>#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE1					TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY
		<#lt>#define TCPIP_GMAC_TX_BUFF_SIZE_QUE1				    		TCPIP_GMAC_TX_BUFF_SIZE_DUMMY
		<#lt>#define TCPIP_GMAC_MAX_TX_PKT_SIZE_QUE1				    	0
		/*** Dummy RX Configuration ***/
		<#lt>#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE1					TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY
		<#lt>#define TCPIP_GMAC_RX_BUFF_SIZE_QUE1				    		TCPIP_GMAC_RX_BUFF_SIZE_DUMMY
		<#lt>#define TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE1					1
		<#lt>#define TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE1				   		0
		<#lt>#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE1				0
		<#lt>#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE1					0
	</#if>
</#if>



<#if (TCPIP_GMAC_QUEUE_2)?has_content>
	<#if TCPIP_GMAC_QUEUE_2 == true>
		/*** QUEUE 2 TX Configuration ***/
		<#if TCPIP_GMAC_TX_EN_QUE2 == true>
			<#lt>#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE2				${TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE2}
			<#lt>#define TCPIP_GMAC_TX_BUFF_SIZE_QUE2				    	${TCPIP_GMAC_TX_BUFF_SIZE_QUE2}
		<#else>
			/*** Dummy TX Configuration ***/
			<#lt>#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE2				TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY
			<#lt>#define TCPIP_GMAC_TX_BUFF_SIZE_QUE2				    	TCPIP_GMAC_TX_BUFF_SIZE_DUMMY
		</#if>
		<#lt>#define TCPIP_GMAC_MAX_TX_PKT_SIZE_QUE2				    	${TCPIP_GMAC_MAX_TX_PKT_SIZE_QUE2}

		/*** QUEUE 2 RX Configuration ***/
		<#if TCPIP_GMAC_RX_EN_QUE2 == true>
			<#lt>#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE2				${TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE2}
			<#lt>#define TCPIP_GMAC_RX_BUFF_SIZE_QUE2				    	${TCPIP_GMAC_RX_BUFF_SIZE_QUE2}
			<#lt>#define TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE2				${TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE2}
			<#lt>#define TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE2				   	${TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE2}
			<#lt>#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE2			${TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE2}
			<#lt>#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE2				${TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE2}
		<#else>
			/*** Dummy RX Configuration ***/
			<#lt>#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE2				TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY
			<#lt>#define TCPIP_GMAC_RX_BUFF_SIZE_QUE2				    	TCPIP_GMAC_RX_BUFF_SIZE_DUMMY
			<#lt>#define TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE2				1
			<#lt>#define TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE2				   	0
			<#lt>#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE2			0
			<#lt>#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE2				0
		</#if>
	<#else>
		/*** Dummy TX Configuration ***/
		<#lt>#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE2					TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY
		<#lt>#define TCPIP_GMAC_TX_BUFF_SIZE_QUE2				    		TCPIP_GMAC_TX_BUFF_SIZE_DUMMY
		<#lt>#define TCPIP_GMAC_MAX_TX_PKT_SIZE_QUE2				    	0
		/*** Dummy RX Configuration ***/
		<#lt>#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE2					TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY
		<#lt>#define TCPIP_GMAC_RX_BUFF_SIZE_QUE2				    		TCPIP_GMAC_RX_BUFF_SIZE_DUMMY
		<#lt>#define TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE2					1
		<#lt>#define TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE2				   		0
		<#lt>#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE2				0
		<#lt>#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE2					0
	</#if>
</#if>

<#if (TCPIP_GMAC_QUEUE_3)?has_content>
	<#if TCPIP_GMAC_QUEUE_3 == true>
		/*** QUEUE 3 TX Configuration ***/
		<#if TCPIP_GMAC_TX_EN_QUE3 == true>
			<#lt>#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE3				${TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE3}
			<#lt>#define TCPIP_GMAC_TX_BUFF_SIZE_QUE3				    	${TCPIP_GMAC_TX_BUFF_SIZE_QUE3}
		<#else>
			/*** Dummy TX Configuration ***/
			<#lt>#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE3				TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY
			<#lt>#define TCPIP_GMAC_TX_BUFF_SIZE_QUE3				    	TCPIP_GMAC_TX_BUFF_SIZE_DUMMY
		</#if>
		<#lt>#define TCPIP_GMAC_MAX_TX_PKT_SIZE_QUE3				    	${TCPIP_GMAC_MAX_TX_PKT_SIZE_QUE3}

		/*** QUEUE 3 RX Configuration ***/
		<#if TCPIP_GMAC_RX_EN_QUE3 == true>
			<#lt>#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE3				${TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE3}
			<#lt>#define TCPIP_GMAC_RX_BUFF_SIZE_QUE3				    	${TCPIP_GMAC_RX_BUFF_SIZE_QUE3}
			<#lt>#define TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE3				${TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE3}
			<#lt>#define TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE3				   	${TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE3}
			<#lt>#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE3			${TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE3}
			<#lt>#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE3				${TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE3}
		<#else>
			/*** Dummy RX Configuration ***/
			<#lt>#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE3				TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY
			<#lt>#define TCPIP_GMAC_RX_BUFF_SIZE_QUE3				    	TCPIP_GMAC_RX_BUFF_SIZE_DUMMY
			<#lt>#define TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE3				1
			<#lt>#define TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE3				   	0
			<#lt>#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE3			0
			<#lt>#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE3				0
		</#if>
	<#else>
		/*** Dummy TX Configuration ***/
		<#lt>#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE3					TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY
		<#lt>#define TCPIP_GMAC_TX_BUFF_SIZE_QUE3				    		TCPIP_GMAC_TX_BUFF_SIZE_DUMMY
		<#lt>#define TCPIP_GMAC_MAX_TX_PKT_SIZE_QUE3				    	0
		/*** Dummy RX Configuration ***/
		<#lt>#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE3					TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY
		<#lt>#define TCPIP_GMAC_RX_BUFF_SIZE_QUE3				    		TCPIP_GMAC_RX_BUFF_SIZE_DUMMY
		<#lt>#define TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE3					1
		<#lt>#define TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE3				   		0
		<#lt>#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE3				0
		<#lt>#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE3					0
	</#if>
</#if>

<#if (TCPIP_GMAC_QUEUE_4)?has_content>
	<#if TCPIP_GMAC_QUEUE_4 == true>
		/*** QUEUE 4 TX Configuration ***/
		<#if TCPIP_GMAC_TX_EN_QUE4 == true>
			<#lt>#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE4				${TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE4}
			<#lt>#define TCPIP_GMAC_TX_BUFF_SIZE_QUE4				    	${TCPIP_GMAC_TX_BUFF_SIZE_QUE4}
		<#else>
			/*** Dummy TX Configuration ***/
			<#lt>#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE4				TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY
			<#lt>#define TCPIP_GMAC_TX_BUFF_SIZE_QUE4				    	TCPIP_GMAC_TX_BUFF_SIZE_DUMMY
		</#if>		
		<#lt>#define TCPIP_GMAC_MAX_TX_PKT_SIZE_QUE4				    	${TCPIP_GMAC_MAX_TX_PKT_SIZE_QUE4}

		/*** QUEUE 4 RX Configuration ***/
		<#if TCPIP_GMAC_RX_EN_QUE4 == true>
			<#lt>#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE4				${TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE4}
			<#lt>#define TCPIP_GMAC_RX_BUFF_SIZE_QUE4				    	${TCPIP_GMAC_RX_BUFF_SIZE_QUE4}
			<#lt>#define TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE4				${TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE4}
			<#lt>#define TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE4				   	${TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE4}
			<#lt>#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE4			${TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE4}
			<#lt>#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE4				${TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE4}
		<#else>
			/*** Dummy RX Configuration ***/
			<#lt>#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE4				TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY
			<#lt>#define TCPIP_GMAC_RX_BUFF_SIZE_QUE4				    	TCPIP_GMAC_RX_BUFF_SIZE_DUMMY
			<#lt>#define TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE4				1
			<#lt>#define TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE4				   	0
			<#lt>#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE4			0
			<#lt>#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE4				0
		</#if>
	<#else>
		/*** Dummy TX Configuration 1***/
		<#lt>#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE4					TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY
		<#lt>#define TCPIP_GMAC_TX_BUFF_SIZE_QUE4				    		TCPIP_GMAC_TX_BUFF_SIZE_DUMMY
		<#lt>#define TCPIP_GMAC_MAX_TX_PKT_SIZE_QUE4				    	0
		/*** Dummy RX Configuration ***/
		<#lt>#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE4					TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY
		<#lt>#define TCPIP_GMAC_RX_BUFF_SIZE_QUE4				    		TCPIP_GMAC_RX_BUFF_SIZE_DUMMY
		<#lt>#define TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE4					1
		<#lt>#define TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE4				   		0
		<#lt>#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE4				0
		<#lt>#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE4					0
	</#if>
</#if>

<#if (TCPIP_GMAC_QUEUE_5)?has_content>
	<#if TCPIP_GMAC_QUEUE_5 == true>
		/*** QUEUE 5 TX Configuration ***/
		<#if TCPIP_GMAC_TX_EN_QUE5 == true>
			<#lt>#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE5				${TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE5}
			<#lt>#define TCPIP_GMAC_TX_BUFF_SIZE_QUE5				    	${TCPIP_GMAC_TX_BUFF_SIZE_QUE5}
		<#else>
			/*** Dummy TX Configuration ***/
			<#lt>#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE5				TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY
			<#lt>#define TCPIP_GMAC_TX_BUFF_SIZE_QUE5				    	TCPIP_GMAC_TX_BUFF_SIZE_DUMMY
		</#if>
		<#lt>#define TCPIP_GMAC_MAX_TX_PKT_SIZE_QUE5				    	${TCPIP_GMAC_MAX_TX_PKT_SIZE_QUE5}
		
		/*** QUEUE 5 RX Configuration ***/
		<#if TCPIP_GMAC_RX_EN_QUE5 == true>
			<#lt>#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE5				${TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE5}
			<#lt>#define TCPIP_GMAC_RX_BUFF_SIZE_QUE5				    	${TCPIP_GMAC_RX_BUFF_SIZE_QUE5}
			<#lt>#define TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE5				${TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE5}
			<#lt>#define TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE5				   	${TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE5}
			<#lt>#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE5			${TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE5}
			<#lt>#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE5				${TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE5}
		<#else>
			/*** Dummy RX Configuration ***/
			<#lt>#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE5				TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY
			<#lt>#define TCPIP_GMAC_RX_BUFF_SIZE_QUE5				    	TCPIP_GMAC_RX_BUFF_SIZE_DUMMY
			<#lt>#define TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE5				1
			<#lt>#define TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE5				   	0
			<#lt>#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE5			0
			<#lt>#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE5				0
		</#if>
	<#else>
		/*** Dummy TX Configuration ***/
		<#lt>#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE5					TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY
		<#lt>#define TCPIP_GMAC_TX_BUFF_SIZE_QUE5				    		TCPIP_GMAC_TX_BUFF_SIZE_DUMMY
		<#lt>#define TCPIP_GMAC_MAX_TX_PKT_SIZE_QUE5				    	0
		/*** Dummy RX Configuration ***/
		<#lt>#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE5					TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY
		<#lt>#define TCPIP_GMAC_RX_BUFF_SIZE_QUE5				    		TCPIP_GMAC_RX_BUFF_SIZE_DUMMY
		<#lt>#define TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUE5					1
		<#lt>#define TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE5				   		0
		<#lt>#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE5				0
		<#lt>#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE5					0
	</#if>
</#if>

#define TCPIP_GMAC_RX_MAX_FRAME		    			${TCPIP_GMAC_RX_MAX_FRAME}
#define TCPIP_GMAC_RX_FILTERS                       \
<#if TCPIP_GMAC_ETH_FILTER_BCAST_ACCEPT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_BCAST_ACCEPT |\
</#if>
<#if TCPIP_GMAC_ETH_FILTER_MCAST_ACCEPT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_MCAST_ACCEPT |\
</#if>
<#if TCPIP_GMAC_ETH_FILTER_UCAST_ACCEPT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_UCAST_ACCEPT |\
</#if>
<#if TCPIP_GMAC_ETH_FILTER_MCAST_HASH_ACCEPT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_MCAST_HASH_ACCEPT |\
</#if>
<#if TCPIP_GMAC_ETH_FILTER_UCAST_HASH_ACCEPT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_UCAST_HASH_ACCEPT |\
</#if>
<#if TCPIP_GMAC_ETH_FILTER_CRC_ERROR_REJECT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_CRC_ERROR_REJECT |\
</#if>
<#if TCPIP_GMAC_ETH_FILTER_CRC_ERROR_ACCEPT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_CRC_ERROR_ACCEPT |\
</#if>
<#if TCPIP_GMAC_ETH_FILTER_MAX_FRAME_ACCEPT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_MAXFRAME_ACCEPT |\
</#if>
<#if TCPIP_GMAC_ETH_FILTER_ALL_ACCEPT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_ALL_ACCEPT |\
</#if>
<#if TCPIP_GMAC_ETH_FILTER_FRAME_ERROR_ACCEPT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_FRAMEERROR_ACCEPT |\
</#if>
<#if TCPIP_GMAC_ETH_FILTER_JUMBO_FRAME_ACCEPT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_JUMBOFRAME_ACCEPT |\
</#if>
													0
<#if (TCPIP_GMAC_RX_QUE_FILTER_EN)?has_content>  
	<#if TCPIP_GMAC_RX_QUE_FILTER_EN == true>  
		<#lt>#define TCPIP_GMAC_SCREEN1_COUNT_QUE							${TCPIP_GMAC_SCREEN1_COUNT_QUE}
		<#if TCPIP_GMAC_SCREEN1_COUNT_QUE gte 1>
			<#list 0 ..(TCPIP_GMAC_SCREEN1_COUNT_QUE -1) as i >
				<#assign rxQue = "TCPIP_GMAC_SCREEN1_QUE_IDX" + i>
				<#lt>#define TCPIP_GMAC_SCREEN1_QUE_IDX${i}								${.vars[rxQue]}
				<#assign dstcen = "TCPIP_QUE_SCREEN1_DSTCEN_IDX" + i>
				<#if .vars[dstcen] == true>
					<#lt>#define TCPIP_QUE_SCREEN1_DSTCEN_IDX${i}							true
					<#assign dstc = "TCPIP_QUE_SCREEN1_DSTC_IDX" + i>
					<#lt>#define TCPIP_QUE_SCREEN1_DSTC_IDX${i}								0x${.vars[dstc]}
				<#else>
					<#lt>#define TCPIP_QUE_SCREEN1_DSTCEN_IDX${i}							false
				</#if>
				
				<#assign udpen = "TCPIP_QUE_SCREEN1_UDPEN_IDX" + i>
				<#if .vars[udpen] == true>
					<#lt>#define TCPIP_QUE_SCREEN1_UDPEN_IDX${i}							true
					<#assign udpprtnum = "TCPIP_QUE_SCREEN1_UDPPORTNUM_IDX" + i>
					<#lt>#define TCPIP_QUE_SCREEN1_UDPPORTNUM_IDX${i}						${.vars[udpprtnum]}
				<#else>
					<#lt>#define TCPIP_QUE_SCREEN1_UDPEN_IDX${i}							false
				</#if>	
				
			</#list>
		</#if>

		<#lt>#define TCPIP_GMAC_SCREEN2_COUNT_QUE							${TCPIP_GMAC_SCREEN2_COUNT_QUE}
		<#if TCPIP_GMAC_SCREEN2_COUNT_QUE gte 1>
			<#list 0 ..(TCPIP_GMAC_SCREEN2_COUNT_QUE -1) as i >
				<#assign rxQue = "TCPIP_GMAC_SCREEN2_QUE_IDX" + i>
				<#lt>#define TCPIP_GMAC_SCREEN2_QUE_IDX${i}								${.vars[rxQue]}
				<#assign vlanprioen = "TCPIP_QUE_SCREEN2_VLANPRIOEN_IDX" + i>
				<#if .vars[vlanprioen] == true>
					<#lt>#define TCPIP_QUE_SCREEN2_VLANPRIOEN_IDX${i}						true
					<#assign vlanprio = "TCPIP_QUE_SCREEN2_VLANPRIO_IDX" + i>
					<#lt>#define TCPIP_QUE_SCREEN2_VLANPRIO_IDX${i}							${.vars[vlanprio]}
				<#else>
					<#lt>#define TCPIP_QUE_SCREEN2_VLANPRIOEN_IDX${i}						false
				</#if>
				
				<#assign ethtypeen = "TCPIP_QUE_SCREEN2_ETHTYPEEN_IDX" + i>
				<#if .vars[ethtypeen] == true>
					<#lt>#define TCPIP_QUE_SCREEN2_ETHTYPEEN_IDX${i}						true
					<#assign ethtype = "TCPIP_QUE_SCREEN2_ETHTYPE_IDX" + i>
					<#lt>#define TCPIP_QUE_SCREEN2_ETHTYPE_IDX${i}							0x${.vars[ethtype]}
				<#else>
					<#lt>#define TCPIP_QUE_SCREEN2_ETHTYPEEN_IDX${i}						false
				</#if>
				
				<#assign compaen = "TCPIP_QUE_SCREEN2_COMPAEN_IDX" + i>
				<#if .vars[compaen] == true>
					<#lt>#define TCPIP_QUE_SCREEN2_COMPAEN_IDX${i}							true
					<#assign compa = "TCPIP_QUE_SCREEN2_COMPA_IDX" + i>
					<#lt>#define TCPIP_QUE_SCREEN2_COMPA_IDX${i}							0x${.vars[compa]}
					<#assign compamask = "TCPIP_QUE_SCREEN2_COMPAMASK_IDX" + i>
					<#lt>#define TCPIP_QUE_SCREEN2_COMPAMASK_IDX${i}						0x${.vars[compamask]}
					<#assign compaoffst = "TCPIP_QUE_SCREEN2_COMPAOFFST_IDX" + i>
					<#lt>#define TCPIP_QUE_SCREEN2_COMPAOFFST_IDX${i}						${.vars[compaoffst]}
					<#assign compaoffststart = "TCPIP_QUE_SCREEN2_COMPAOFFSTSTRT_IDX" + i>
					<#lt>#define TCPIP_QUE_SCREEN2_COMPAOFFSTSTRT_IDX${i}					${.vars[compaoffststart]}
				<#else>
					<#lt>#define TCPIP_QUE_SCREEN2_COMPAEN_IDX${i}							false
				</#if>
				
				<#assign compben = "TCPIP_QUE_SCREEN2_COMPBEN_IDX" + i>
				<#if .vars[compben] == true>
					<#lt>#define TCPIP_QUE_SCREEN2_COMPBEN_IDX${i}							true
					<#assign compb = "TCPIP_QUE_SCREEN2_COMPB_IDX" + i>
					<#lt>#define TCPIP_QUE_SCREEN2_COMPB_IDX${i}							0x${.vars[compb]}
					<#assign compbmask = "TCPIP_QUE_SCREEN2_COMPBMASK_IDX" + i>
					<#lt>#define TCPIP_QUE_SCREEN2_COMPBMASK_IDX${i}						0x${.vars[compbmask]}
					<#assign compboffst = "TCPIP_QUE_SCREEN2_COMPBOFFST_IDX" + i>
					<#lt>#define TCPIP_QUE_SCREEN2_COMPBOFFST_IDX${i}						${.vars[compboffst]}
					<#assign compboffststart = "TCPIP_QUE_SCREEN2_COMPBOFFSTSTRT_IDX" + i>
					<#lt>#define TCPIP_QUE_SCREEN2_COMPBOFFSTSTRT_IDX${i}					${.vars[compboffststart]}
				<#else>
					<#lt>#define TCPIP_QUE_SCREEN2_COMPBEN_IDX${i}							false
				</#if>
				
				<#assign compcen = "TCPIP_QUE_SCREEN2_COMPCEN_IDX" + i>
				<#if .vars[compcen] == true>
					<#lt>#define TCPIP_QUE_SCREEN2_COMPCEN_IDX${i}							true
					<#assign compc = "TCPIP_QUE_SCREEN2_COMPC_IDX" + i>
					<#lt>#define TCPIP_QUE_SCREEN2_COMPC_IDX${i}							0x${.vars[compc]}
					<#assign compcmask = "TCPIP_QUE_SCREEN2_COMPCMASK_IDX" + i>
					<#lt>#define TCPIP_QUE_SCREEN2_COMPCMASK_IDX${i}						0x${.vars[compcmask]}
					<#assign compcoffst = "TCPIP_QUE_SCREEN2_COMPCOFFST_IDX" + i>
					<#lt>#define TCPIP_QUE_SCREEN2_COMPCOFFST_IDX${i}						${.vars[compcoffst]}
					<#assign compcoffststart = "TCPIP_QUE_SCREEN2_COMPCOFFSTSTRT_IDX" + i>
					<#lt>#define TCPIP_QUE_SCREEN2_COMPCOFFSTSTRT_IDX${i}					${.vars[compcoffststart]}
				<#else>
					<#lt>#define TCPIP_QUE_SCREEN2_COMPCEN_IDX${i}							false
				</#if>
				
			</#list>
		</#if>	
	<#else>
		<#lt>#define TCPIP_GMAC_SCREEN1_COUNT_QUE							0
		<#lt>#define TCPIP_GMAC_SCREEN2_COUNT_QUE							0		
	</#if>
<#else>
	<#lt>#define TCPIP_GMAC_SCREEN1_COUNT_QUE							0
	<#lt>#define TCPIP_GMAC_SCREEN2_COUNT_QUE							0	
</#if>	
													
#define TCPIP_GMAC_ETH_OPEN_FLAGS       			\
<#if TCPIP_GMAC_ETH_OF_AUTO_NEGOTIATION>
                                                    TCPIP_ETH_OPEN_AUTO |\
</#if>
<#if TCPIP_GMAC_ETH_OF_FULL_DUPLEX>
                                                    TCPIP_ETH_OPEN_FDUPLEX |\
</#if>
<#if TCPIP_GMAC_ETH_OF_HALF_DUPLEX>
                                                    TCPIP_ETH_OPEN_HDUPLEX |\
</#if>
<#if TCPIP_GMAC_ETH_OF_100>
                                                    TCPIP_ETH_OPEN_100 |\
</#if>
<#if TCPIP_GMAC_ETH_OF_10>
                                                    TCPIP_ETH_OPEN_10 |\
</#if>
<#if TCPIP_GMAC_ETH_OF_HUGE_PKTS>
                                                    TCPIP_ETH_OPEN_HUGE_PKTS |\
</#if>
<#if TCPIP_GMAC_ETH_OF_MAC_LOOPBACK>
                                                    TCPIP_ETH_OPEN_MAC_LOOPBACK |\
</#if>
<#if TCPIP_GMAC_ETH_OF_PHY_LOOPBACK>
                                                    TCPIP_ETH_OPEN_PHY_LOOPBACK |\
</#if>
<#if TCPIP_GMAC_ETH_OF_MDIX_AUTO>
                                                    TCPIP_ETH_OPEN_MDIX_AUTO |\
</#if>
<#if TCPIP_GMAC_ETH_OF_MDIX_SWAP>
                                                    TCPIP_ETH_OPEN_MDIX_SWAP |\
</#if>
<#if TCPIP_GMAC_ETH_OF_RMII>
                                                    TCPIP_ETH_OPEN_RMII |\
</#if>
                                                    0

#define TCPIP_INTMAC_MODULE_ID		    			${TCPIP_INTMAC_MODULE_ID}
<#if (TCPIP_INTMAC_DEVICE)?has_content>
<#if TCPIP_INTMAC_DEVICE == "SAME7x_V7x">
#define TCPIP_INTMAC_PERIPHERAL_CLK  				${core.MASTER_CLOCK_FREQUENCY}
<#elseif TCPIP_INTMAC_DEVICE == "SAMRH71">
#define TCPIP_INTMAC_PERIPHERAL_CLK  				${core.CLK_MCK_FREQ}
<#elseif TCPIP_INTMAC_DEVICE == "SAME5x">
#define TCPIP_INTMAC_PERIPHERAL_CLK  				${core.MAIN_CLOCK_FREQUENCY}
<#elseif TCPIP_INTMAC_DEVICE == "SAMA5D2">
#define TCPIP_INTMAC_PERIPHERAL_CLK  				${core.MCK_CLK_FREQUENCY}
</#if>
</#if>

<#if DRV_GMAC_RX_CHKSM_OFFLOAD == true>
#define DRV_GMAC_RX_CHKSM_OFFLOAD				(TCPIP_MAC_CHECKSUM_TCP | TCPIP_MAC_CHECKSUM_UDP | TCPIP_MAC_CHECKSUM_IPV4)
<#else>
#define DRV_GMAC_RX_CHKSM_OFFLOAD				(TCPIP_MAC_CHECKSUM_NONE)			
</#if>
<#if DRV_GMAC_TX_CHKSM_OFFLOAD == true>
#define DRV_GMAC_TX_CHKSM_OFFLOAD				(TCPIP_MAC_CHECKSUM_TCP | TCPIP_MAC_CHECKSUM_UDP | TCPIP_MAC_CHECKSUM_IPV4)	
<#else>	
#define DRV_GMAC_TX_CHKSM_OFFLOAD				(TCPIP_MAC_CHECKSUM_NONE)		
</#if>
<#if (TCPIP_GMAC_TX_PRIO_COUNT)?has_content>
#define TCPIP_GMAC_TX_PRIO_COUNT				${TCPIP_GMAC_TX_PRIO_COUNT}
<#else>
#define TCPIP_GMAC_TX_PRIO_COUNT				1
</#if>
<#if (TCPIP_GMAC_RX_PRIO_COUNT)?has_content>
#define TCPIP_GMAC_RX_PRIO_COUNT				${TCPIP_GMAC_RX_PRIO_COUNT}
<#else>
#define TCPIP_GMAC_RX_PRIO_COUNT				1
</#if>
#define DRV_GMAC_NUMBER_OF_QUEUES				${DRV_GMAC_NUMBER_OF_QUEUES}
#define DRV_GMAC_RMII_MODE						${DRV_GMAC_RMII_VALUE}
<#if (TCPIP_INTMAC_DEVICE)?has_content>
	<#if ((TCPIP_INTMAC_DEVICE == "SAME7x_V7x") || (TCPIP_INTMAC_DEVICE == "SAMRH71"))>
		<#if DRV_GMAC_NO_CACHE_CONFIG?has_content && DRV_GMAC_NO_CACHE_CONFIG == true>
			<#lt>#define DRV_GMAC_DESCRIPTOR_ADDRESS				0x${DRV_GMAC_NOCACHE_MEM_ADDRESS}
		<#else>
			<#lt>#error Configure Non-Cachebable Memory for storing GMAC Descriptors
		</#if>
	</#if>
</#if>
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
