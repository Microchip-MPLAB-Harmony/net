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

<#--
Copyright (C) 2014-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
<#if __PROCESSOR?contains("PIC32MZ")>
#define TCPIP_INTMAC_PERIPHERAL_CLK  				${core.CONFIG_SYS_CLK_PBCLK5_FREQ}
<#elseif __PROCESSOR?contains("PIC32MX")>
#define TCPIP_INTMAC_PERIPHERAL_CLK  				${core.SYS_CLK_FREQ}
</#if>

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

<#if DRV_ETHMAC_MULTI_CLIENT == true>
#define DRV_ETHMAC_MULTI_CLIENT        			true
<#else>
#define DRV_ETHMAC_MULTI_CLIENT        			false
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
