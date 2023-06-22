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
<#if TCPIP_USE_ETH_MAC == true> 
    <#lt>/*** ${GMAC_PERIPHERAL_INSTANCE} Configuration ***/
    <#lt>#define DRV_${GMAC_PERIPHERAL_INSTANCE}
    <#lt>#define DRV_${TCPIP_INTMAC_DEVICE}
    <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_TX_DESCRIPTORS_COUNT_DUMMY    ${.vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_TX_DESCRIPTORS_COUNT_DUMMY"]}
    <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_DESCRIPTORS_COUNT_DUMMY    ${.vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_DESCRIPTORS_COUNT_DUMMY"]}
    <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_BUFF_SIZE_DUMMY            ${.vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_BUFF_SIZE_DUMMY"]}
    <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_TX_BUFF_SIZE_DUMMY            ${.vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_TX_BUFF_SIZE_DUMMY"]}
    <#lt><#assign queCnt = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_MAX_MAC_QUE_CNT"?eval - 1 >
    <#lt><#list 0..queCnt as qIndx>
    <#lt><#assign MAC_QUEUE_ENABLE =  "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUEUE_" + qIndx?string >
    <#lt><#assign MAC_QUEUE_TX_ENABLE =  "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_TX_EN_QUE" + qIndx?string >
    <#lt><#assign MAC_QUEUE_RX_ENABLE =  "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_EN_QUE" + qIndx?string >
        <#lt><#if .vars[MAC_QUEUE_ENABLE]?has_content>
            <#lt><#if .vars[ MAC_QUEUE_ENABLE ] == true>
                <#lt><#if .vars[ MAC_QUEUE_TX_ENABLE ] == true>
                    <#lt>/*** QUEUE ${qIndx} TX Configuration ***/
                    <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_TX_DESCRIPTORS_COUNT_QUE${qIndx}            ${.vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_TX_DESCRIPTORS_COUNT_QUE" + qIndx?string]}
                <#lt><#else>
                    <#lt>/*** QUEUE ${qIndx} Dummy TX Configuration ***/
                    <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_TX_DESCRIPTORS_COUNT_QUE${qIndx}         TCPIP_${GMAC_PERIPHERAL_INSTANCE}_TX_DESCRIPTORS_COUNT_DUMMY
                <#lt></#if>
                <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_MAX_TX_PKT_SIZE_QUE${qIndx}                 ${.vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_MAX_TX_PKT_SIZE_QUE" + qIndx?string]}
                <#lt>/*** QUEUE ${qIndx} RX Configuration ***/
                <#lt><#if .vars[ MAC_QUEUE_RX_ENABLE ] == true>
                    <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_DESCRIPTORS_COUNT_QUE${qIndx}            ${.vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_DESCRIPTORS_COUNT_QUE" + qIndx?string]}
                    <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_BUFF_SIZE_QUE${qIndx}                    ${.vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_BUFF_SIZE_QUE" + qIndx?string]}
                    <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_DEDICATED_BUFFERS_QUE${qIndx}            ${.vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_DEDICATED_BUFFERS_QUE" + qIndx?string]}
                    <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_ADDL_BUFF_COUNT_QUE${qIndx}              ${.vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_ADDL_BUFF_COUNT_QUE" + qIndx?string]}
                    <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_BUFF_COUNT_THRESHOLD_QUE${qIndx}         ${.vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_BUFF_COUNT_THRESHOLD_QUE" + qIndx?string]}
                    <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_BUFF_ALLOC_COUNT_QUE${qIndx}             ${.vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_BUFF_ALLOC_COUNT_QUE" + qIndx?string]}
                <#lt><#else>
                    <#lt>/*** QUEUE ${qIndx} Dummy RX Configuration ***/
                    <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_DESCRIPTORS_COUNT_QUE${qIndx}            TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_DESCRIPTORS_COUNT_DUMMY
                    <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_BUFF_SIZE_QUE${qIndx}                    TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_BUFF_SIZE_DUMMY
                    <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_DEDICATED_BUFFERS_QUE${qIndx}            1
                    <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_ADDL_BUFF_COUNT_QUE${qIndx}              0
                    <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_BUFF_COUNT_THRESHOLD_QUE${qIndx}         0
                    <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_BUFF_ALLOC_COUNT_QUE${qIndx}             0
                <#lt></#if>
            <#lt><#else>
                <#lt>/*** QUEUE ${qIndx} Not Enabled - Dummy Configuration ***/
                <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_TX_DESCRIPTORS_COUNT_QUE${qIndx}             TCPIP_${GMAC_PERIPHERAL_INSTANCE}_TX_DESCRIPTORS_COUNT_DUMMY
                <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_MAX_TX_PKT_SIZE_QUE${qIndx}                  0
                <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_DESCRIPTORS_COUNT_QUE${qIndx}             TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_DESCRIPTORS_COUNT_DUMMY
                <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_BUFF_SIZE_QUE${qIndx}                     TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_BUFF_SIZE_DUMMY
                <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_DEDICATED_BUFFERS_QUE${qIndx}             1
                <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_ADDL_BUFF_COUNT_QUE${qIndx}               0
                <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_BUFF_COUNT_THRESHOLD_QUE${qIndx}          0
                <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_BUFF_ALLOC_COUNT_QUE${qIndx}              0
            <#lt></#if>
        <#lt></#if>
    <#lt></#list>
    <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_FILTERS                       \
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_FILTER_BCAST_ACCEPT"] == true>
                                                        TCPIP_MAC_RX_FILTER_TYPE_BCAST_ACCEPT |\
    </#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_FILTER_MCAST_ACCEPT"] == true>
                                                        TCPIP_MAC_RX_FILTER_TYPE_MCAST_ACCEPT |\
    </#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_FILTER_UCAST_ACCEPT"] == true>
                                                        TCPIP_MAC_RX_FILTER_TYPE_UCAST_ACCEPT |\
    </#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_FILTER_MCAST_HASH_ACCEPT"] == true>
                                                        TCPIP_MAC_RX_FILTER_TYPE_MCAST_HASH_ACCEPT |\
    </#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_FILTER_UCAST_HASH_ACCEPT"] == true>
                                                        TCPIP_MAC_RX_FILTER_TYPE_UCAST_HASH_ACCEPT |\
    </#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_FILTER_CRC_ERROR_REJECT"] == true>
                                                        TCPIP_MAC_RX_FILTER_TYPE_CRC_ERROR_REJECT |\
    </#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_FILTER_CRC_ERROR_ACCEPT"] == true>
                                                        TCPIP_MAC_RX_FILTER_TYPE_CRC_ERROR_ACCEPT |\
    </#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_FILTER_MAX_FRAME_ACCEPT"] == true>
                                                        TCPIP_MAC_RX_FILTER_TYPE_MAXFRAME_ACCEPT |\
    </#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_FILTER_ALL_ACCEPT"] == true>
                                                        TCPIP_MAC_RX_FILTER_TYPE_ALL_ACCEPT |\
    </#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_FILTER_FRAME_ERROR_ACCEPT"] == true>
                                                        TCPIP_MAC_RX_FILTER_TYPE_FRAMEERROR_ACCEPT |\
    </#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_FILTER_JUMBO_FRAME_ACCEPT"] == true>
                                                        TCPIP_MAC_RX_FILTER_TYPE_JUMBOFRAME_ACCEPT |\
    </#if> 
                                                          0
       
    <#lt><#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_QUE_FILTER_EN"]?has_content>   
        <#lt><#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_QUE_FILTER_EN"] == true>  
            <#lt><#assign GMAC_SCREEN1_COUNT_QUE =  "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_SCREEN1_COUNT_QUE">
            <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_SCREEN1_COUNT_QUE               ${.vars[GMAC_SCREEN1_COUNT_QUE]}
            <#lt><#if .vars[GMAC_SCREEN1_COUNT_QUE] gte 1>      
                <#lt><#list 0 ..(.vars[GMAC_SCREEN1_COUNT_QUE] -1) as i >
                    <#lt><#assign rxQue = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_SCREEN1_QUE_IDX" + i?string>
                    <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_SCREEN1_QUE_IDX${i}                ${.vars[rxQue]}
                    <#lt><#assign dstcen = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN1_DSTCEN_IDX" + i?string>
                    <#lt><#if .vars[dstcen] == true>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN1_DSTCEN_IDX${i}          true
                        <#lt><#assign dstc = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN1_DSTC_IDX" + i?string>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN1_DSTC_IDX${i}            0x${.vars[dstc]}
                    <#lt><#else>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN1_DSTCEN_IDX${i}          false
                    <#lt></#if>                
                    <#lt><#assign udpen = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN1_UDPEN_IDX" + i?string>
                    <#lt><#if .vars[udpen] == true>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN1_UDPEN_IDX${i}           true
                        <#lt><#assign udpprtnum = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN1_UDPPORTNUM_IDX" +i?string>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN1_UDPPORTNUM_IDX${i}      ${.vars[udpprtnum]}
                    <#lt><#else>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN1_UDPEN_IDX${i}        false
                    <#lt></#if>                  
                <#lt></#list>
            <#lt></#if>
            <#lt><#assign GMAC_SCREEN2_COUNT_QUE =  "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_SCREEN2_COUNT_QUE">
            <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_SCREEN2_COUNT_QUE               ${.vars[GMAC_SCREEN2_COUNT_QUE]}
            <#lt><#if .vars[GMAC_SCREEN2_COUNT_QUE] gte 1>      
                <#lt><#list 0 ..(.vars[GMAC_SCREEN2_COUNT_QUE] -1) as i >
                    <#lt><#assign rxQue = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_SCREEN2_QUE_IDX" + i?string>
                    <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_SCREEN2_QUE_IDX${i}                ${.vars[rxQue]}
                    <#lt><#assign vlanprioen = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN2_VLANPRIOEN_IDX" + i?string>
                    <#lt><#if .vars[vlanprioen] == true>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_VLANPRIOEN_IDX${i}          true
                        <#lt><#assign vlanprio = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN2_VLANPRIO_IDX" + i?string>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_VLANPRIO_IDX${i}          ${.vars[vlanprio]}
                    <#lt><#else>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_VLANPRIOEN_IDX${i}          false
                    <#lt></#if>
                    
                    <#lt><#assign ethtypeen = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN2_ETHTYPEEN_IDX" + i?string>
                    <#lt><#if .vars[ethtypeen] == true>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_ETHTYPEEN_IDX${i}          true
                        <#lt><#assign ethtype = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN2_ETHTYPE_IDX" + i?string>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_ETHTYPE_IDX${i}          0x${.vars[ethtype]}
                    <#lt><#else>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_ETHTYPEEN_IDX${i}          false
                    <#lt></#if>
                    
                    <#lt><#assign compaen = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN2_COMPAEN_IDX" + i?string>
                    <#lt><#if .vars[compaen] == true>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPAEN_IDX${i}          true
                        <#lt><#assign compa = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN2_COMPA_IDX" + i?string>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPA_IDX${i}          0x${.vars[compa]}
                        <#lt><#assign compamask = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN2_COMPAMASK_IDX" + i?string>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPAMASK_IDX${i}          0x${.vars[compamask]}
                        <#lt><#assign compaoffst = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN2_COMPAOFFST_IDX" + i?string>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPAOFFST_IDX${i}          ${.vars[compaoffst]}
                        <#lt><#assign compaoffststart = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN2_COMPAOFFSTSTRT_IDX" + i?string>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPAOFFSTSTRT_IDX${i}          ${.vars[compaoffststart]}
                    <#lt><#else>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPAEN_IDX${i}          false
                    <#lt></#if>
                    
                    <#lt><#assign compben = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN2_COMPBEN_IDX" + i?string>
                    <#lt><#if .vars[compben] == true>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPBEN_IDX${i}          true
                        <#lt><#assign compb = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN2_COMPB_IDX" + i?string>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPB_IDX${i}          0x${.vars[compb]}
                        <#lt><#assign compbmask = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN2_COMPBMASK_IDX" + i?string>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPBMASK_IDX${i}          0x${.vars[compbmask]}
                        <#lt><#assign compboffst = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN2_COMPBOFFST_IDX" + i?string>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPBOFFST_IDX${i}          ${.vars[compboffst]}
                        <#lt><#assign compboffststart = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN2_COMPBOFFSTSTRT_IDX" + i?string>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPBOFFSTSTRT_IDX${i}          ${.vars[compboffststart]}
                    <#lt><#else>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPBEN_IDX${i}          false
                    <#lt></#if>
                    
                    <#lt><#assign compcen = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN2_COMPCEN_IDX" + i?string>
                    <#lt><#if .vars[compcen] == true>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPCEN_IDX${i}          true
                        <#lt><#assign compc = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN2_COMPC_IDX" + i?string>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPC_IDX${i}          0x${.vars[compc]}
                        <#lt><#assign compcmask = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN2_COMPCMASK_IDX" + i?string>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPCMASK_IDX${i}          0x${.vars[compcmask]}
                        <#lt><#assign compcoffst = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN2_COMPCOFFST_IDX" + i?string>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPCOFFST_IDX${i}          ${.vars[compcoffst]}
                        <#lt><#assign compcoffststart = "TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_QUE_SCREEN2_COMPCOFFSTSTRT_IDX" + i?string>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPCOFFSTSTRT_IDX${i}          ${.vars[compcoffststart]}
                    <#lt><#else>
                        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_QUE_SCREEN2_COMPCEN_IDX${i}          false
                    <#lt></#if>
                <#lt></#list>
            <#lt></#if>
        <#lt><#else>
            <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_SCREEN1_COUNT_QUE        0 
            <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_SCREEN2_COUNT_QUE        0       
        <#lt></#if>
    <#lt><#else>
        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_SCREEN1_COUNT_QUE        0 
        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_SCREEN2_COUNT_QUE        0  
    <#lt></#if>  

    <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_ETH_OPEN_FLAGS                   \
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_OF_AUTO_NEGOTIATION"] == true>
                                                        TCPIP_ETH_OPEN_AUTO |\
    </#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_OF_FULL_DUPLEX"] == true>
                                                        TCPIP_ETH_OPEN_FDUPLEX |\
    </#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_OF_HALF_DUPLEX"] == true>
                                                        TCPIP_ETH_OPEN_HDUPLEX |\
    </#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_OF_1000"]?has_content>
        <#lt><#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_OF_1000"] == true>
                                                        TCPIP_ETH_OPEN_1000 |\
        <#lt></#if>
    </#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_OF_100"] == true>
                                                        TCPIP_ETH_OPEN_100 |\
    </#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_OF_10"] == true>
                                                        TCPIP_ETH_OPEN_10 |\
    </#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_OF_HUGE_PKTS"] == true>
                                                        TCPIP_ETH_OPEN_HUGE_PKTS |\
    </#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_OF_MAC_LOOPBACK"] == true>
                                                        TCPIP_ETH_OPEN_MAC_LOOPBACK |\
    </#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_OF_PHY_LOOPBACK"] == true>
                                                        TCPIP_ETH_OPEN_PHY_LOOPBACK |\
    </#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_OF_MDIX_AUTO"] == true>
                                                        TCPIP_ETH_OPEN_MDIX_AUTO |\
    </#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_OF_MDIX_SWAP"] == true>
                                                        TCPIP_ETH_OPEN_MDIX_SWAP |\
    </#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_OF_RMII"] == true>
                                                        TCPIP_ETH_OPEN_RMII |\
    </#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_OF_MII"] == true>
                                                        TCPIP_ETH_OPEN_MII |\
    </#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_OF_GMII"]?has_content>
        <#lt><#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_OF_GMII"] == true>
                                                        TCPIP_ETH_OPEN_GMII |\
        <#lt></#if>
    </#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_OF_RGMII"]?has_content>
        <#lt><#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_ETH_OF_RGMII"] == true>
                                                        TCPIP_ETH_OPEN_RGMII |\
        <#lt></#if>
    </#if>
                                                        0

    <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_MODULE_ID                       ${.vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_MODULE_ID"]}

    <#lt><#if (TCPIP_INTMAC_DEVICE)?has_content>
        <#lt><#if TCPIP_INTMAC_DEVICE == "SAME7x_V7x">
            <#lt>#define TCPIP_INTMAC_PERIPHERAL_CLK                 ${core.MASTER_CLOCK_FREQUENCY}
        <#lt><#elseif TCPIP_INTMAC_DEVICE == "SAMRH71">
            <#lt>#define TCPIP_INTMAC_PERIPHERAL_CLK                 ${core.CLK_MCK_FREQ}
        <#lt><#elseif TCPIP_INTMAC_DEVICE == "SAME5x">
            <#lt>#define TCPIP_INTMAC_PERIPHERAL_CLK                 ${core.MAIN_CLOCK_FREQUENCY}
        <#lt><#elseif TCPIP_INTMAC_DEVICE == "PIC32CZ">
            <#lt>#define TCPIP_INTMAC_PERIPHERAL_CLK                 (${core.MAIN_CLOCK_FREQUENCY}/${core.CONF_MCLK_CLKDIV1})           
        <#lt><#elseif TCPIP_INTMAC_DEVICE == "SAMA5D2">
            <#lt>#define TCPIP_INTMAC_PERIPHERAL_CLK                 ${core.MCK_CLK_FREQUENCY}
        <#lt><#elseif TCPIP_INTMAC_DEVICE == "SAMA7G">
            <#lt>#define TCPIP_INTMAC_PERIPHERAL_CLK                 ${core.MCK1_FREQUENCY}
        <#lt><#elseif TCPIP_INTMAC_DEVICE == "SAM9X7">
            <#lt>#define TCPIP_INTMAC_PERIPHERAL_CLK                 ${core.MCK_FREQUENCY}
        <#lt><#elseif TCPIP_INTMAC_DEVICE == "PIC32CK">
            <#lt>#define TCPIP_INTMAC_PERIPHERAL_CLK                 ${core.MAIN_CLOCK_FREQUENCY}
        <#lt><#elseif TCPIP_INTMAC_DEVICE == "PIC32CXSG41">
            <#lt>#define TCPIP_INTMAC_PERIPHERAL_CLK                 ${core.MAIN_CLOCK_FREQUENCY}
        <#lt></#if>
    <#lt></#if>

    <#lt><#if .vars["DRV_${GMAC_PERIPHERAL_INSTANCE?string}_RX_CHKSM_OFFLOAD"] == true>
        <#lt>#define DRV_${GMAC_PERIPHERAL_INSTANCE}_RX_CHKSM_OFFLOAD             (TCPIP_MAC_CHECKSUM_TCP | TCPIP_MAC_CHECKSUM_UDP | TCPIP_MAC_CHECKSUM_IPV4)
    <#lt><#else>
        <#lt>#define DRV_${GMAC_PERIPHERAL_INSTANCE}_RX_CHKSM_OFFLOAD               (TCPIP_MAC_CHECKSUM_NONE)           
    <#lt></#if>
    <#lt><#if .vars["DRV_${GMAC_PERIPHERAL_INSTANCE?string}_TX_CHKSM_OFFLOAD"] == true>
        <#lt>#define DRV_${GMAC_PERIPHERAL_INSTANCE}_TX_CHKSM_OFFLOAD               (TCPIP_MAC_CHECKSUM_TCP | TCPIP_MAC_CHECKSUM_UDP | TCPIP_MAC_CHECKSUM_IPV4) 
    <#lt><#else> 
        <#lt>#define DRV_${GMAC_PERIPHERAL_INSTANCE}_TX_CHKSM_OFFLOAD               (TCPIP_MAC_CHECKSUM_NONE)       
    <#lt></#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_TX_PRIO_COUNT"]?has_content>
        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_TX_PRIO_COUNT                ${.vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_TX_PRIO_COUNT"]}
    <#lt><#else>
        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_TX_PRIO_COUNT                1
    <#lt></#if>
    <#if .vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_PRIO_COUNT"]?has_content>
        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_PRIO_COUNT                ${.vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_RX_PRIO_COUNT"]}
    <#lt><#else>
        <#lt>#define TCPIP_${GMAC_PERIPHERAL_INSTANCE}_RX_PRIO_COUNT                1
    <#lt></#if>
    <#lt>#define DRV_${GMAC_PERIPHERAL_INSTANCE}_NUMBER_OF_QUEUES               ${.vars["TCPIP_${GMAC_PERIPHERAL_INSTANCE?string}_MAX_MAC_QUE_CNT"]}
    <#lt>#define DRV_${GMAC_PERIPHERAL_INSTANCE}_RMII_MODE                      ${.vars["DRV_${GMAC_PERIPHERAL_INSTANCE?string}_RMII_VALUE"]}

    <#lt><#if (TCPIP_INTMAC_DEVICE)?has_content>
        <#lt><#if ((TCPIP_INTMAC_DEVICE == "SAME7x_V7x") || (TCPIP_INTMAC_DEVICE == "PIC32CZ") || (TCPIP_INTMAC_DEVICE == "SAMRH71"))>
            <#lt><#if .vars["DRV_${GMAC_PERIPHERAL_INSTANCE?string}_NO_CACHE_CONFIG"]?has_content>
                <#lt><#if .vars["DRV_${GMAC_PERIPHERAL_INSTANCE?string}_NO_CACHE_CONFIG"] == true>
                    <#lt>#define DRV_${GMAC_PERIPHERAL_INSTANCE}_DESCRIPTOR_ADDRESS                0x${.vars["DRV_${GMAC_PERIPHERAL_INSTANCE?string}_NOCACHE_MEM_ADDRESS"]}
                <#lt><#else>
                    <#lt>#error Configure Non-Cachebable Memory for storing GMAC Descriptors
                <#lt></#if>
            <#lt></#if>
        <#lt></#if>
    <#lt></#if>
<#lt></#if>

<#--
/*******************************************************************************
 End of File
*/
-->
