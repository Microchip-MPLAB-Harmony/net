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
<#if CONFIG_TCPIP_USE_ETH_MAC == true>
/*** TCPIP MAC Configuration ***/
#define TCPIP_EMAC_TX_DESCRIPTORS				    ${CONFIG_TCPIP_EMAC_TX_DESCRIPTORS}
#define TCPIP_EMAC_RX_DESCRIPTORS				    ${CONFIG_TCPIP_EMAC_RX_DESCRIPTORS}
#define TCPIP_EMAC_RX_DEDICATED_BUFFERS				${CONFIG_TCPIP_EMAC_RX_DEDICATED_BUFFERS}
#define TCPIP_EMAC_RX_INIT_BUFFERS				    ${CONFIG_TCPIP_EMAC_RX_INIT_BUFFERS}
#define TCPIP_EMAC_RX_LOW_THRESHOLD				    ${CONFIG_TCPIP_EMAC_RX_LOW_THRESHOLD}
#define TCPIP_EMAC_RX_LOW_FILL				        ${CONFIG_TCPIP_EMAC_RX_LOW_FILL}
#define TCPIP_EMAC_MAX_FRAME		    			${CONFIG_TCPIP_EMAC_MAX_FRAME}
#define TCPIP_EMAC_LINK_MTU		    			    ${CONFIG_TCPIP_EMAC_LINK_MTU}
#define TCPIP_EMAC_RX_BUFF_SIZE		    			${CONFIG_TCPIP_EMAC_RX_BUFF_SIZE}
#define TCPIP_EMAC_RX_FRAGMENTS		    			${CONFIG_TCPIP_EMAC_RX_FRAGMENTS}

#define TCPIP_EMAC_RX_FILTERS                       \
<#if CONFIG_TCPIP_EMAC_ETH_FILTER_BCAST_ACCEPT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_BCAST_ACCEPT |\
</#if>
<#if CONFIG_TCPIP_EMAC_ETH_FILTER_MCAST_ACCEPT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_MCAST_ACCEPT |\
</#if>
<#if CONFIG_TCPIP_EMAC_ETH_FILTER_UCAST_ACCEPT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_UCAST_ACCEPT |\
</#if>
<#if CONFIG_TCPIP_EMAC_ETH_FILTER_UCAST_OTHER_ACCEPT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_UCAST_OTHER_ACCEPT |\
</#if>
<#if CONFIG_TCPIP_EMAC_ETH_FILTER_RUNT_REJECT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_RUNT_REJECT |\
</#if>
<#if CONFIG_TCPIP_EMAC_ETH_FILTER_RUNT_ACCEPT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_RUNT_ACCEPT |\
</#if>
<#if CONFIG_TCPIP_EMAC_ETH_FILTER_CRC_ERROR_REJECT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_CRC_ERROR_REJECT |\
</#if>
<#if CONFIG_TCPIP_EMAC_ETH_FILTER_CRC_ERROR_ACCEPT>
                                                    TCPIP_MAC_RX_FILTER_TYPE_CRC_ERROR_ACCEPT |\
</#if>
                                                    0
#define TCPIP_EMAC_ETH_OPEN_FLAGS       			\
<#if CONFIG_TCPIP_EMAC_ETH_OF_AUTO_NEGOTIATION>
                                                    TCPIP_ETH_OPEN_AUTO |\
</#if>
<#if CONFIG_TCPIP_EMAC_ETH_OF_FULL_DUPLEX>
                                                    TCPIP_ETH_OPEN_FDUPLEX |\
</#if>
<#if CONFIG_TCPIP_EMAC_ETH_OF_HALF_DUPLEX>
                                                    TCPIP_ETH_OPEN_HDUPLEX |\
</#if>
<#if CONFIG_TCPIP_EMAC_ETH_OF_100>
                                                    TCPIP_ETH_OPEN_100 |\
</#if>
<#if CONFIG_TCPIP_EMAC_ETH_OF_10>
                                                    TCPIP_ETH_OPEN_10 |\
</#if>
<#if CONFIG_TCPIP_EMAC_ETH_OF_HUGE_PKTS>
                                                    TCPIP_ETH_OPEN_HUGE_PKTS |\
</#if>
<#if CONFIG_TCPIP_EMAC_ETH_OF_MAC_LOOPBACK>
                                                    TCPIP_ETH_OPEN_MAC_LOOPBACK |\
</#if>
<#if CONFIG_TCPIP_EMAC_ETH_OF_PHY_LOOPBACK>
                                                    TCPIP_ETH_OPEN_PHY_LOOPBACK |\
</#if>
<#if CONFIG_TCPIP_EMAC_ETH_OF_MDIX_AUTO>
                                                    TCPIP_ETH_OPEN_MDIX_AUTO |\
</#if>
<#if CONFIG_TCPIP_EMAC_ETH_OF_MDIX_SWAP>
                                                    TCPIP_ETH_OPEN_MDIX_SWAP |\
</#if>
<#if CONFIG_TCPIP_EMAC_ETH_OF_RMII>
                                                    TCPIP_ETH_OPEN_RMII |\
</#if>
                                                    0

#define TCPIP_EMAC_MODULE_ID		    			${CONFIG_TCPIP_EMAC_MODULE_ID}
<#if CONFIG_TCPIP_EMAC_INTERRUPT_MODE == true>
#define TCPIP_EMAC_INTERRUPT_MODE        			true
<#else>
#define TCPIP_EMAC_INTERRUPT_MODE        			true
</#if>
#define DRV_ETHMAC_INSTANCES_NUMBER				${CONFIG_DRV_ETHMAC_INSTANCES_NUMBER}
#define DRV_ETHMAC_CLIENTS_NUMBER				${CONFIG_DRV_ETHMAC_CLIENTS_NUMBER}
#define DRV_ETHMAC_INDEX	    	    			${CONFIG_DRV_ETHMAC_INDEX}
#define DRV_ETHMAC_PERIPHERAL_ID				${CONFIG_DRV_ETHMAC_PERIPHERAL_ID}
#define DRV_ETHMAC_INTERRUPT_VECTOR				${CONFIG_DRV_ETHMAC_INTERRUPT_VECTOR}
#define DRV_ETHMAC_INTERRUPT_SOURCE				${CONFIG_DRV_ETHMAC_INTERRUPT_SOURCE}
#define DRV_ETHMAC_POWER_STATE		    			${CONFIG_DRV_ETHMAC_POWER_STATE}

<#if CONFIG_DRV_ETHMAC_INTERRUPT_MODE == true>
#define DRV_ETHMAC_INTERRUPT_MODE        			true
<#else>
#define DRV_ETHMAC_INTERRUPT_MODE        			true
</#if>


#define TCPIP_EMAC_PHY_CONFIG_FLAGS     			\
<#if CONFIG_TCPIP_EMAC_PHY_CONFIG_RMII>
                                                    DRV_ETHPHY_CFG_RMII | \
</#if>
<#if CONFIG_TCPIP_EMAC_PHY_CONFIG_ALTERNATE>
                                                    DRV_ETHPHY_CFG_ALTERNATE | \
</#if>
<#if CONFIG_TCPIP_EMAC_PHY_CONFIG_AUTO>
                                                    DRV_ETHPHY_CFG_AUTO | \
</#if>
                                                    0                                                    

#define TCPIP_EMAC_PHY_LINK_INIT_DELAY  			${CONFIG_TCPIP_EMAC_PHY_LINK_INIT_DELAY}
#define TCPIP_EMAC_PHY_ADDRESS		    			${CONFIG_TCPIP_EMAC_PHY_ADDRESS}
#define DRV_ETHPHY_INSTANCES_NUMBER				${CONFIG_DRV_ETHPHY_INSTANCES_NUMBER}
#define DRV_ETHPHY_CLIENTS_NUMBER				${CONFIG_DRV_ETHPHY_CLIENTS_NUMBER}
#define DRV_ETHPHY_INDEX		        		${CONFIG_DRV_ETHPHY_INDEX}
#define DRV_ETHPHY_PERIPHERAL_ID				${CONFIG_DRV_ETHPHY_PERIPHERAL_ID}
#define DRV_ETHPHY_NEG_INIT_TMO		    			${CONFIG_DRV_ETHPHY_NEG_INIT_TMO}
#define DRV_ETHPHY_NEG_DONE_TMO		    			${CONFIG_DRV_ETHPHY_NEG_DONE_TMO}
#define DRV_ETHPHY_RESET_CLR_TMO				${CONFIG_DRV_ETHPHY_RESET_CLR_TMO}
<#if CONFIG_DRV_MIIM_USE_DRIVER == true>
#define DRV_ETHPHY_USE_DRV_MIIM                     true
<#else>
#define DRV_ETHPHY_USE_DRV_MIIM                     false
</#if>
<#if CONFIG_TCPIP_EMAC_AUTO_FLOW_CONTROL_ENABLE == true>
#define TCPIP_EMAC_AUTO_FLOW_CONTROL_ENABLE        	true
#define TCPIP_EMAC_FLOW_CONTROL_PAUSE_BYTES         ${CONFIG_TCPIP_EMAC_FLOW_CONTROL_PAUSE_BYTES}
#define TCPIP_EMAC_FLOW_CONTROL_FULL_WMARK          ${CONFIG_TCPIP_EMAC_FLOW_CONTROL_FULL_WMARK}
#define TCPIP_EMAC_FLOW_CONTROL_EMPTY_WMARK         ${CONFIG_TCPIP_EMAC_FLOW_CONTROL_EMPTY_WMARK}
<#else>
#define TCPIP_EMAC_AUTO_FLOW_CONTROL_ENABLE        	false
</#if>
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
