<#--
/*******************************************************************************
  TCPIP MAC Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    tcpip_mac.h.ftl

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
#define TCPIP_EMAC_TX_DESCRIPTORS				    ${TCPIP_EMAC_TX_DESCRIPTORS}
#define TCPIP_EMAC_RX_DESCRIPTORS				    ${TCPIP_EMAC_RX_DESCRIPTORS}
#define TCPIP_EMAC_RX_DEDICATED_BUFFERS				${TCPIP_EMAC_RX_DEDICATED_BUFFERS}
#define TCPIP_EMAC_RX_INIT_BUFFERS				    ${TCPIP_EMAC_RX_INIT_BUFFERS}
#define TCPIP_EMAC_RX_LOW_THRESHOLD				    ${TCPIP_EMAC_RX_LOW_THRESHOLD}
#define TCPIP_EMAC_RX_LOW_FILL				        ${TCPIP_EMAC_RX_LOW_FILL}
#define TCPIP_EMAC_MAX_FRAME		    			${TCPIP_EMAC_MAX_FRAME}
#define TCPIP_EMAC_LINK_MTU		    			    ${TCPIP_EMAC_LINK_MTU}
#define TCPIP_EMAC_RX_BUFF_SIZE		    			${TCPIP_EMAC_RX_BUFF_SIZE}
#define TCPIP_EMAC_RX_FRAGMENTS		    			${TCPIP_EMAC_RX_FRAGMENTS}

#define TCPIP_EMAC_RX_FILTERS                       \
<#if TCPIP_EMAC_ETH_FILTER_BCAST_ACCEPT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_BCAST_ACCEPT |\
</#if>
<#if TCPIP_EMAC_ETH_FILTER_MCAST_ACCEPT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_MCAST_ACCEPT |\
</#if>
<#if TCPIP_EMAC_ETH_FILTER_UCAST_ACCEPT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_UCAST_ACCEPT |\
</#if>
<#if TCPIP_EMAC_ETH_FILTER_UCAST_OTHER_ACCEPT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_UCAST_OTHER_ACCEPT |\
</#if>
<#if TCPIP_EMAC_ETH_FILTER_RUNT_REJECT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_RUNT_REJECT |\
</#if>
<#if TCPIP_EMAC_ETH_FILTER_RUNT_ACCEPT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_RUNT_ACCEPT |\
</#if>
<#if TCPIP_EMAC_ETH_FILTER_CRC_ERROR_REJECT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_CRC_ERROR_REJECT |\
</#if>
<#if TCPIP_EMAC_ETH_FILTER_CRC_ERROR_ACCEPT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_CRC_ERROR_ACCEPT |\
</#if>
                                                    0
#define TCPIP_EMAC_ETH_OPEN_FLAGS       			\
<#if TCPIP_EMAC_ETH_OF_AUTO_NEGOTIATION>
                                                    TCPIP_ETH_OPEN_AUTO |\
</#if>
<#if TCPIP_EMAC_ETH_OF_FULL_DUPLEX>
                                                    TCPIP_ETH_OPEN_FDUPLEX |\
</#if>
<#if TCPIP_EMAC_ETH_OF_HALF_DUPLEX>
                                                    TCPIP_ETH_OPEN_HDUPLEX |\
</#if>
<#if TCPIP_EMAC_ETH_OF_100>
                                                    TCPIP_ETH_OPEN_100 |\
</#if>
<#if TCPIP_EMAC_ETH_OF_10>
                                                    TCPIP_ETH_OPEN_10 |\
</#if>
<#if TCPIP_EMAC_ETH_OF_HUGE_PKTS>
                                                    TCPIP_ETH_OPEN_HUGE_PKTS |\
</#if>
<#if TCPIP_EMAC_ETH_OF_MAC_LOOPBACK>
                                                    TCPIP_ETH_OPEN_MAC_LOOPBACK |\
</#if>
<#if TCPIP_EMAC_ETH_OF_PHY_LOOPBACK>
                                                    TCPIP_ETH_OPEN_PHY_LOOPBACK |\
</#if>
<#if TCPIP_EMAC_ETH_OF_MDIX_AUTO>
                                                    TCPIP_ETH_OPEN_MDIX_AUTO |\
</#if>
<#if TCPIP_EMAC_ETH_OF_MDIX_SWAP>
                                                    TCPIP_ETH_OPEN_MDIX_SWAP |\
</#if>
<#if TCPIP_EMAC_ETH_OF_RMII>
                                                    TCPIP_ETH_OPEN_RMII |\
</#if>
                                                    0

#define TCPIP_INTMAC_MODULE_ID		    			${TCPIP_EMAC_MODULE_ID}
#define TCPIP_INTMAC_PERIPHERAL_CLK  				${core.CONFIG_SYS_CLK_PBCLK5_FREQ}

#define DRV_ETHMAC_INSTANCES_NUMBER				${DRV_ETHMAC_INSTANCES_NUMBER}
#define DRV_ETHMAC_CLIENTS_NUMBER				${DRV_ETHMAC_CLIENTS_NUMBER}
#define DRV_ETHMAC_INDEX	    	    		${DRV_ETHMAC_INDEX}
#define DRV_ETHMAC_PERIPHERAL_ID				${DRV_ETHMAC_PERIPHERAL_ID}
#define DRV_ETHMAC_INTERRUPT_SOURCE				${DRV_ETHMAC_INTERRUPT_SOURCE}

<#if INTERRUPT_ENABLE == true>
#define DRV_ETHMAC_INTERRUPT_MODE        			true
<#else>
#define DRV_ETHMAC_INTERRUPT_MODE        			true
</#if>

<#if TCPIP_EMAC_AUTO_FLOW_CONTROL_ENABLE == true>
#define TCPIP_EMAC_AUTO_FLOW_CONTROL_ENABLE        	true
#define TCPIP_EMAC_FLOW_CONTROL_PAUSE_BYTES         ${TCPIP_EMAC_FLOW_CONTROL_PAUSE_BYTES}
#define TCPIP_EMAC_FLOW_CONTROL_FULL_WMARK          ${TCPIP_EMAC_FLOW_CONTROL_FULL_WMARK}
#define TCPIP_EMAC_FLOW_CONTROL_EMPTY_WMARK         ${TCPIP_EMAC_FLOW_CONTROL_EMPTY_WMARK}
<#else>
#define TCPIP_EMAC_AUTO_FLOW_CONTROL_ENABLE        	false
</#if>
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
