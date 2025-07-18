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
<#if MAC_PERIPHERAL_INSTANCE??>
// *****************************************************************************
// ${MAC_PERIPHERAL_INSTANCE} -- as produced from drv_intmac.h.ftl

#define EMAC_MIN_RX_SIZE                            64      // always multiple of 16!, 
                                                            // less than 64 creates excessive fragmentation
#define EMAC_MIN_TX_DESCRIPTORS                     1       // needed to accommodate zero copy and TCP traffic
<#assign PERIPHERAL_FREQUENCY = "core.${MAC_PERIPHERAL_INSTANCE}_CLOCK_FREQUENCY"?eval>
#define TCPIP_INTMAC_PERIPHERAL_CLK                 ${PERIPHERAL_FREQUENCY}
#define TCPIP_INTMAC_MAX_NUMBER_OF_QUEUES           ${MAC_MAX_QUEUE_COUNT}
#define TCPIP_INTMAC_MAX_RX_PACKET_POINTERS         ${MAX_RX_PACKET_POINTERS}
#define DRV_${MAC_PERIPHERAL_INSTANCE}_BASE_ADDRESS                      ${MAC_BASE_ADDRESS}
#define DRV_${MAC_PERIPHERAL_INSTANCE}_CLIENTS_NUMBER                    ${MAC_MAX_CLIENTS}
#define DRV_${MAC_PERIPHERAL_INSTANCE}_NUMBER_OF_QUEUES                  ${MAC_QUEUE_COUNT}
#define DRV_${MAC_PERIPHERAL_INSTANCE}_INTERRUPT_SOURCE                  ${MAC_INTERRUPT_SOURCE}
#define DRV_${MAC_PERIPHERAL_INSTANCE}_RMII_MODE                         ${MAC_RMII_VALUE}

<#lt><#assign qq = MAC_MAX_QUEUE_COUNT - 1 >
<#lt><#list 0..qq as ii>
    <#lt><#assign QUEUE_ENABLE =  "QUEUE_ENABLE_" + ii?string >
    <#lt><#if .vars[ QUEUE_ENABLE ]?has_content>
        <#lt><#if .vars[ QUEUE_ENABLE ] == true>
            <#lt>/*** MAC QUEUE ${ii} Configuration ***/
        <#lt><#else>
            <#lt>/*** MAC QUEUE ${ii} -- disabled, dummy configuration ***/
        <#lt></#if>
        <#lt>#define DRV_${MAC_PERIPHERAL_INSTANCE}_TX_DESCRIPTORS_COUNT_QUE${ii}         ${.vars["TX_DESCRIPTOR_COUNT_" + ii?string]}
        <#lt>#define DRV_${MAC_PERIPHERAL_INSTANCE}_TX_BUFF_SIZE_QUE${ii}                 ${.vars["TX_BUFFER_SIZE_" + ii?string]}
        <#lt>#define DRV_${MAC_PERIPHERAL_INSTANCE}_RX_DEVICE_MAX_DESCRIPTORS${ii}        ${.vars["RX_DEVICE_MAX_DESCRIPTORS"]}
        <#lt>#define DRV_${MAC_PERIPHERAL_INSTANCE}_RX_DESCRIPTORS_COUNT_QUE${ii}         ${.vars["RX_DESCRIPTOR_COUNT_" + ii?string]}
        <#lt>#define DRV_${MAC_PERIPHERAL_INSTANCE}_RX_BUFF_SIZE_QUE${ii}                 ${.vars["RX_BUFFER_SIZE_" + ii?string]}
        <#lt>#define DRV_${MAC_PERIPHERAL_INSTANCE}_RX_BUFF_STATIC_COUNT_QUE${ii}         ${.vars["RX_STATIC_BUFFERS_" + ii?string]}
        <#lt>#define DRV_${MAC_PERIPHERAL_INSTANCE}_RX_BUFF_COUNT_THRESHOLD_QUE${ii}      ${.vars["RX_BUFFER_ALLOCATION_THRESHOLD_" + ii?string]}
        <#lt>#define DRV_${MAC_PERIPHERAL_INSTANCE}_RX_BUFF_ALLOC_COUNT_QUE${ii}          ${.vars["RX_BUFFER_ALLOCATION_" + ii?string]}
    <#lt></#if>
<#lt></#list>

#define DRV_${MAC_PERIPHERAL_INSTANCE}_RX_FILTERS                        ( 0\
<#if LINK_ACCEPT_BCAST>
                                                    | TCPIP_MAC_RX_FILTER_TYPE_BCAST_ACCEPT\
</#if>
<#if LINK_ACCEPT_MCAST>
                                                    | TCPIP_MAC_RX_FILTER_TYPE_MCAST_ACCEPT\
</#if>
<#if LINK_ACCEPT_UCAST>
                                                    | TCPIP_MAC_RX_FILTER_TYPE_UCAST_ACCEPT\
</#if>
<#if LINK_ACCEPT_MCAST_HASH>
                                                    | TCPIP_MAC_RX_FILTER_TYPE_MCAST_HASH_ACCEPT\
</#if>
<#if LINK_ACCEPT_UCAST_HASH>
                                                    | TCPIP_MAC_RX_FILTER_TYPE_UCAST_HASH_ACCEPT\
</#if>
<#if LINK_REJECT_CRC_ERROR>
                                                    | TCPIP_MAC_RX_FILTER_TYPE_CRC_ERROR_REJECT\
</#if>
<#if LINK_ACCEPT_CRC_ERROR>
                                                    | TCPIP_MAC_RX_FILTER_TYPE_CRC_ERROR_ACCEPT\
</#if>
<#if LINK_ACCEPT_MAX_FRAME_SIZE_PACKETS>
                                                    | TCPIP_MAC_RX_FILTER_TYPE_MAXFRAME_ACCEPT\
</#if>
<#if LINK_ACCEPT_ALL>
                                                    | TCPIP_MAC_RX_FILTER_TYPE_ALL_ACCEPT\
</#if>
<#if LINK_ACCEPT_FRAME_ERROR>
                                                    | TCPIP_MAC_RX_FILTER_TYPE_FRAMEERROR_ACCEPT\
</#if>
<#if LINK_ACCEPT_JUMBO_FRAME>
                                                    | TCPIP_MAC_RX_FILTER_TYPE_JUMBOFRAME_ACCEPT\
</#if>
                                                    )
#define DRV_${MAC_PERIPHERAL_INSTANCE}_ETH_OPEN_FLAGS                    ( 0\
<#if LINK_AUTO_NEGOTIATION>
                                                    | TCPIP_ETH_OPEN_AUTO\
    <#if LINK_AUTO_FULL_DUPLEX>
                                                    | TCPIP_ETH_OPEN_FDUPLEX\
    </#if>
    <#if LINK_AUTO_HALF_DUPLEX>
                                                    | TCPIP_ETH_OPEN_HDUPLEX\
    </#if>
    <#if LINK_AUTO_100>
                                                    | TCPIP_ETH_OPEN_100\
    </#if>
    <#if LINK_AUTO_10>
                                                    | TCPIP_ETH_OPEN_10\
    </#if>
<#else>
    <#lt><#if '0x00' == LINK_MANUAL_CHOICE >
                                                    | TCPIP_ETH_OPEN_FDUPLEX\
                                                    | TCPIP_ETH_OPEN_100\
    <#lt><#elseif '0x01' == LINK_MANUAL_CHOICE >
                                                    | TCPIP_ETH_OPEN_FDUPLEX\
                                                    | TCPIP_ETH_OPEN_10\
    <#lt><#elseif '0x02' == LINK_MANUAL_CHOICE >
                                                    | TCPIP_ETH_OPEN_HDUPLEX\
                                                    | TCPIP_ETH_OPEN_100\
    <#lt><#elseif '0x03' == LINK_MANUAL_CHOICE >
                                                    | TCPIP_ETH_OPEN_HDUPLEX\
                                                    | TCPIP_ETH_OPEN_10\
    <#lt></#if>
</#if>
<#if ALLOW_HUGE_PACKETS>
                                                    | TCPIP_ETH_OPEN_HUGE_PKTS\
</#if>
<#if MAC_LOOPBACK>
                                                    | TCPIP_ETH_OPEN_MAC_LOOPBACK\
</#if>
<#if PHY_LOOPBACK>
                                                    | TCPIP_ETH_OPEN_PHY_LOOPBACK\
</#if>
<#if MDIX_AUTO_ENABLE>
                                                    | TCPIP_ETH_OPEN_MDIX_AUTO\
</#if>
<#if MDIX_SWAP>
                                                    | TCPIP_ETH_OPEN_MDIX_SWAP\
</#if>
<#if USE_RMII>
                                                    | TCPIP_ETH_OPEN_RMII\
</#if>
                                                    )

// *****************************************************************************
<#lt></#if>
