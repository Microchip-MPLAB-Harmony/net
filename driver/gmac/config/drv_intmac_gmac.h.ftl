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

/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS  WITHOUT  WARRANTY  OF  ANY  KIND,
EITHER EXPRESS  OR  IMPLIED,  INCLUDING  WITHOUT  LIMITATION,  ANY  WARRANTY  OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A  PARTICULAR  PURPOSE.
IN NO EVENT SHALL MICROCHIP OR  ITS  LICENSORS  BE  LIABLE  OR  OBLIGATED  UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,  BREACH  OF  WARRANTY,  OR
OTHER LEGAL  EQUITABLE  THEORY  ANY  DIRECT  OR  INDIRECT  DAMAGES  OR  EXPENSES
INCLUDING BUT NOT LIMITED TO ANY  INCIDENTAL,  SPECIAL,  INDIRECT,  PUNITIVE  OR
CONSEQUENTIAL DAMAGES, LOST  PROFITS  OR  LOST  DATA,  COST  OF  PROCUREMENT  OF
SUBSTITUTE  GOODS,  TECHNOLOGY,  SERVICES,  OR  ANY  CLAIMS  BY  THIRD   PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE  THEREOF),  OR  OTHER  SIMILAR  COSTS.
*******************************************************************************/
-->
<#if TCPIP_USE_ETH_MAC == true>
/*** TCPIP MAC Configuration ***/
#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY				${TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY}
#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY				${TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY}
#define TCPIP_GMAC_RX_BUFF_SIZE_DUMMY				    	${TCPIP_GMAC_RX_BUFF_SIZE_DUMMY}
#define TCPIP_GMAC_TX_BUFF_SIZE_DUMMY				    	${TCPIP_GMAC_TX_BUFF_SIZE_DUMMY}

<#if TCPIP_GMAC_QUEUE_0 == true>
/*** QUEUE 0 Configuration ***/
#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE0				${TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE0}
#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE0				${TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE0}
#define TCPIP_GMAC_RX_BUFF_SIZE_QUE0				    	${TCPIP_GMAC_RX_BUFF_SIZE_QUE0}
#define TCPIP_GMAC_TX_BUFF_SIZE_QUE0				    	${TCPIP_GMAC_TX_BUFF_SIZE_QUE0}
<#else>
/*** QUEUE 0 Disabled; Dummy Configuration ***/
#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE0				TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY
#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE0				TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY
#define TCPIP_GMAC_RX_BUFF_SIZE_QUE0				    	TCPIP_GMAC_RX_BUFF_SIZE_DUMMY
#define TCPIP_GMAC_TX_BUFF_SIZE_QUE0				    	TCPIP_GMAC_TX_BUFF_SIZE_DUMMY
</#if>

<#if TCPIP_GMAC_QUEUE_1 == true>
/*** QUEUE 1 Configuration ***/
#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE1				${TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE1}
#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE1				${TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE1}
#define TCPIP_GMAC_RX_BUFF_SIZE_QUE1				    	TCPIP_GMAC_RX_BUFF_SIZE_DUMMY
#define TCPIP_GMAC_TX_BUFF_SIZE_QUE1				    	${TCPIP_GMAC_TX_BUFF_SIZE_QUE1}
<#else>
/*** QUEUE 1 Disabled; Dummy Configuration ***/
#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE1				TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY
#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE1				TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY
#define TCPIP_GMAC_RX_BUFF_SIZE_QUE1				    	TCPIP_GMAC_RX_BUFF_SIZE_DUMMY
#define TCPIP_GMAC_TX_BUFF_SIZE_QUE1				    	TCPIP_GMAC_TX_BUFF_SIZE_DUMMY
</#if>

<#if TCPIP_GMAC_QUEUE_2 == true>
/*** QUEUE 2 Configuration ***/
#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE2				${TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE2}
#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE2				${TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE2}
#define TCPIP_GMAC_RX_BUFF_SIZE_QUE2				    	${TCPIP_GMAC_RX_BUFF_SIZE_QUE2}
#define TCPIP_GMAC_TX_BUFF_SIZE_QUE2				    	${TCPIP_GMAC_TX_BUFF_SIZE_QUE2}
<#else>
/*** QUEUE 2 Disabled; Dummy Configuration ***/
#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE2				TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY
#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE2				TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY
#define TCPIP_GMAC_RX_BUFF_SIZE_QUE2				    	TCPIP_GMAC_RX_BUFF_SIZE_DUMMY
#define TCPIP_GMAC_TX_BUFF_SIZE_QUE2				    	TCPIP_GMAC_TX_BUFF_SIZE_DUMMY
</#if>

<#if TCPIP_GMAC_QUEUE_3 == true>
/*** QUEUE 3 Configuration ***/
#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE3				${TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE3}
#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE3				${TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE3}
#define TCPIP_GMAC_RX_BUFF_SIZE_QUE3				    	${TCPIP_GMAC_RX_BUFF_SIZE_QUE3}
#define TCPIP_GMAC_TX_BUFF_SIZE_QUE3				    	${TCPIP_GMAC_TX_BUFF_SIZE_QUE3}
<#else>
/*** QUEUE 3 Disabled; Dummy Configuration ***/
#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE3				TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY
#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE3				TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY
#define TCPIP_GMAC_RX_BUFF_SIZE_QUE3				    	TCPIP_GMAC_RX_BUFF_SIZE_DUMMY
#define TCPIP_GMAC_TX_BUFF_SIZE_QUE3				    	TCPIP_GMAC_TX_BUFF_SIZE_DUMMY
</#if>

<#if TCPIP_GMAC_QUEUE_4 == true>
/*** QUEUE 4 Configuration ***/
#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE4				${TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE4}
#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE4				${TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE4}
#define TCPIP_GMAC_RX_BUFF_SIZE_QUE4				    	${TCPIP_GMAC_RX_BUFF_SIZE_QUE4}
#define TCPIP_GMAC_TX_BUFF_SIZE_QUE4				    	${TCPIP_GMAC_TX_BUFF_SIZE_QUE4}
<#else>
/*** QUEUE 4 Disabled; Dummy Configuration ***/
#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE4				TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY
#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE4				TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY
#define TCPIP_GMAC_RX_BUFF_SIZE_QUE4				    	TCPIP_GMAC_RX_BUFF_SIZE_DUMMY
#define TCPIP_GMAC_TX_BUFF_SIZE_QUE4				    	TCPIP_GMAC_TX_BUFF_SIZE_DUMMY
</#if>

<#if TCPIP_GMAC_QUEUE_5 == true>
/*** QUEUE 5 Configuration ***/
#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE5				${TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE5}
#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE5				${TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE5}
#define TCPIP_GMAC_RX_BUFF_SIZE_QUE5				    	${TCPIP_GMAC_RX_BUFF_SIZE_QUE5}
#define TCPIP_GMAC_TX_BUFF_SIZE_QUE5				    	${TCPIP_GMAC_TX_BUFF_SIZE_QUE5}
<#else>
/*** QUEUE 5 Disabled; Dummy Configuration ***/
#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE5				TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY
#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE5				TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY
#define TCPIP_GMAC_RX_BUFF_SIZE_QUE5				    	TCPIP_GMAC_RX_BUFF_SIZE_DUMMY
#define TCPIP_GMAC_TX_BUFF_SIZE_QUE5				    	TCPIP_GMAC_TX_BUFF_SIZE_DUMMY
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
<#if TCPIP_GMAC_INTERRUPT_MODE == true>
#define TCPIP_GMAC_INTERRUPT_MODE        			true
<#else>
#define TCPIP_GMAC_INTERRUPT_MODE        			true
</#if>
#define DRV_GMAC_INSTANCES_NUMBER				${DRV_GMAC_INSTANCES_NUMBER}
#define DRV_GMAC_CLIENTS_NUMBER					${DRV_GMAC_CLIENTS_NUMBER}
#define DRV_GMAC_INDEX	    	    			${DRV_GMAC_INDEX}
#define DRV_GMAC_PERIPHERAL_ID					${DRV_GMAC_PERIPHERAL_ID}
#define DRV_GMAC_INTERRUPT_SOURCE				${DRV_GMAC_INTERRUPT_SOURCE}
#define DRV_GMAC_POWER_STATE		    		${DRV_GMAC_POWER_STATE}

<#if DRV_GMAC_INTERRUPT_MODE == true>
#define DRV_GMAC_INTERRUPT_MODE        			true
<#else>
#define DRV_GMAC_INTERRUPT_MODE        			true
</#if>

</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
