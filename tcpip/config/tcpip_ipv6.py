"""*****************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************"""

    
def instantiateComponent(tcpipIPv6Component):
    print("TCPIP IPv6 Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # Enable IPv6   
    tcpipIPv6 = tcpipIPv6Component.createBooleanSymbol("TCPIP_STACK_USE_IPV6", None)
    tcpipIPv6.setLabel("IPv6")
    tcpipIPv6.setVisible(False)
    tcpipIPv6.setDescription("Enable IPv6")
    tcpipIPv6.setDefaultValue(True)

    # Minimum Allocation Unit For The Payload
    tcpipIPv6MinAllocBlkSize = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_DEFAULT_ALLOCATION_BLOCK_SIZE", None)
    tcpipIPv6MinAllocBlkSize.setLabel("Minimum Allocation Unit For The Payload")
    tcpipIPv6MinAllocBlkSize.setVisible(True)
    tcpipIPv6MinAllocBlkSize.setDescription("Minimum Allocation Unit For The Payload")
    tcpipIPv6MinAllocBlkSize.setDefaultValue(64)
    #tcpipIPv6MinAllocBlkSize.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

    # Lower Bounds Of The Maximum Transmission Unit
    tcpipIPv6MinLinkMTU = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_MINIMUM_LINK_MTU", None)
    tcpipIPv6MinLinkMTU.setLabel("Lower Bounds Of The Maximum Transmission Unit")
    tcpipIPv6MinLinkMTU.setVisible(True)
    tcpipIPv6MinLinkMTU.setDescription("Lower Bounds Of The Maximum Transmission Unit")
    tcpipIPv6MinLinkMTU.setDefaultValue(1280)
    #tcpipIPv6MinLinkMTU.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

    # Maximum Transmission Unit
    tcpipIPv6LinkMTU = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_DEFAULT_LINK_MTU", None)
    tcpipIPv6LinkMTU.setLabel("Maximum Transmission Unit")
    tcpipIPv6LinkMTU.setVisible(True)
    tcpipIPv6LinkMTU.setDescription("Maximum Transmission Unit")
    tcpipIPv6LinkMTU.setDefaultValue(1500)
    #tcpipIPv6LinkMTU.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

    # IPv6 Default Hop Limit
    tcpipIPv6HopLimit = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_DEFAULT_CUR_HOP_LIMIT", None)
    tcpipIPv6HopLimit.setLabel("IPv6 Default Hop Limit")
    tcpipIPv6HopLimit.setVisible(True)
    tcpipIPv6HopLimit.setDescription("IPv6 Default Hop Limit")
    tcpipIPv6HopLimit.setDefaultValue(64)
    #tcpipIPv6HopLimit.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

    # Router Advertisement Reachable Time in Seconds
    tcpipIPv6RouterAdvReachTime = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_DEFAULT_BASE_REACHABLE_TIME", None)
    tcpipIPv6RouterAdvReachTime.setLabel("Router Advertisement Reachable Time in Seconds")
    tcpipIPv6RouterAdvReachTime.setVisible(True)
    tcpipIPv6RouterAdvReachTime.setDescription("Router Advertisement Reachable Time in Seconds")
    tcpipIPv6RouterAdvReachTime.setDefaultValue(30)
    #tcpipIPv6RouterAdvReachTime.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

    # Router Advertisement's Retransmission Time in ms
    tcpipIPv6RouterAdvRetransTime = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_DEFAULT_RETRANSMIT_TIME", None)
    tcpipIPv6RouterAdvRetransTime.setLabel("Router Advertisement's Retransmission Time in ms")
    tcpipIPv6RouterAdvRetransTime.setVisible(True)
    tcpipIPv6RouterAdvRetransTime.setDescription("Router Advertisement's Retransmission Time in ms")
    tcpipIPv6RouterAdvRetransTime.setDefaultValue(1000)
    #tcpipIPv6RouterAdvRetransTime.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

    # Maximum Number of Queued Packets per Remote
    tcpipIPv6QueNeighbPktMaxNum = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_QUEUE_NEIGHBOR_PACKET_LIMIT", None)
    tcpipIPv6QueNeighbPktMaxNum.setLabel("Maximum Number of Queued Packets per Remote")
    tcpipIPv6QueNeighbPktMaxNum.setVisible(True)
    tcpipIPv6QueNeighbPktMaxNum.setDescription("Maximum Number of Queued Packets per Remote")
    tcpipIPv6QueNeighbPktMaxNum.setDefaultValue(1)
    #tcpipIPv6QueNeighbPktMaxNum.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

    # Stale Neighbor Discovery Packet Time-out
    tcpipIPv6NeighbCacheEntryStaleTimeout = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_NEIGHBOR_CACHE_ENTRY_STALE_TIMEOUT", None)
    tcpipIPv6NeighbCacheEntryStaleTimeout.setLabel("Stale Neighbor Discovery Packet Time-out")
    tcpipIPv6NeighbCacheEntryStaleTimeout.setVisible(True)
    tcpipIPv6NeighbCacheEntryStaleTimeout.setDescription("Stale Neighbor Discovery Packet Time-out")
    tcpipIPv6NeighbCacheEntryStaleTimeout.setDefaultValue(600)
    #tcpipIPv6NeighbCacheEntryStaleTimeout.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

    # Maximum Number of Multicast Queued IPv6
    tcpipIPv6QueMcastPktMaxNum = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_QUEUE_MCAST_PACKET_LIMIT", None)
    tcpipIPv6QueMcastPktMaxNum.setLabel("Maximum Number of Multicast Queued IPv6")
    tcpipIPv6QueMcastPktMaxNum.setVisible(True)
    tcpipIPv6QueMcastPktMaxNum.setDescription("Maximum Number of Multicast Queued IPv6")
    tcpipIPv6QueMcastPktMaxNum.setDefaultValue(4)
    #tcpipIPv6QueMcastPktMaxNum.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

    # Multicast Packet Time-out in Seconds
    tcpipIPv6QueMcastPktTimeout = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_QUEUED_MCAST_PACKET_TIMEOUT", None)
    tcpipIPv6QueMcastPktTimeout.setLabel("Multicast Packet Time-out - Seconds")
    tcpipIPv6QueMcastPktTimeout.setVisible(True)
    tcpipIPv6QueMcastPktTimeout.setDescription("Multicast Packet Time-out in Seconds")
    tcpipIPv6QueMcastPktTimeout.setDefaultValue(10)
    #tcpipIPv6QueMcastPktTimeout.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

    #IPv6 Task Processing Rate in ms
    tcpipIPv6TaskProcessRate = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_TASK_PROCESS_RATE", None)
    tcpipIPv6TaskProcessRate.setLabel("IPv6 Task Processing Rate - ms")
    tcpipIPv6TaskProcessRate.setVisible(True)
    tcpipIPv6TaskProcessRate.setDescription("IPv6 Task Processing Rate in ms")
    tcpipIPv6TaskProcessRate.setDefaultValue(1000)
    #tcpipIPv6TaskProcessRate.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

    #IPv6 Init Task Processing Rate in ms
    tcpipIPv6InitTaskProcessRate = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_INIT_TASK_PROCESS_RATE", None)
    tcpipIPv6InitTaskProcessRate.setLabel("IPv6 Init Task Processing Rate - ms")
    tcpipIPv6InitTaskProcessRate.setVisible(True)
    tcpipIPv6InitTaskProcessRate.setDescription("IPv6 Init Task Processing Rate in ms")
    tcpipIPv6InitTaskProcessRate.setDefaultValue(32)
    #tcpipIPv6InitTaskProcessRate.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])
    
    # Enable IPv6 ULA Generation
    tcpipIPv6UlaEnable = tcpipIPv6Component.createBooleanSymbol("TCPIP_IPV6_ULA_GENERATE_ENABLE", None)
    tcpipIPv6UlaEnable.setLabel("Enable ULA Generation")
    tcpipIPv6UlaEnable.setVisible(True)
    tcpipIPv6UlaEnable.setDescription("Enable ULA Generation")
    tcpipIPv6UlaEnable.setDefaultValue(False)
    
    #NTP Access Time-out For The IPv6 ULA Address Generation in ms
    tcpipIPv6UlaNtpAccessTimeout = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_ULA_NTP_ACCESS_TMO", tcpipIPv6UlaEnable)
    tcpipIPv6UlaNtpAccessTimeout.setLabel("NTP Access Time-out For The IPv6 ULA Address Generation - ms")
    tcpipIPv6UlaNtpAccessTimeout.setVisible(False)
    tcpipIPv6UlaNtpAccessTimeout.setDescription("NTP Access Time-out For The IPv6 ULA Address Generation in ms")
    tcpipIPv6UlaNtpAccessTimeout.setDefaultValue(12000)
    tcpipIPv6UlaNtpAccessTimeout.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_IPV6_ULA_GENERATE_ENABLE"])

    #NTP Time Stamp Validity Window in ms
    tcpipIPv6UlaNtpValidWindowTimestamp = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_ULA_NTP_VALID_WINDOW", tcpipIPv6UlaEnable)
    tcpipIPv6UlaNtpValidWindowTimestamp.setLabel("NTP Time Stamp Validity Window - ms")
    tcpipIPv6UlaNtpValidWindowTimestamp.setVisible(False)
    tcpipIPv6UlaNtpValidWindowTimestamp.setDescription("NTP Time Stamp Validity Window in ms")
    tcpipIPv6UlaNtpValidWindowTimestamp.setDefaultValue(1000)
    tcpipIPv6UlaNtpValidWindowTimestamp.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_IPV6_ULA_GENERATE_ENABLE"])

    #Fragmentation Packet Time-out Value
    tcpipIPv6FragPktTimeout = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_FRAGMENT_PKT_TIMEOUT", None)
    tcpipIPv6FragPktTimeout.setLabel("Fragmentation Packet Time-out Value")
    tcpipIPv6FragPktTimeout.setVisible(True)
    tcpipIPv6FragPktTimeout.setDescription("Fragmentation Packet Time-out Value")
    tcpipIPv6FragPktTimeout.setDefaultValue(60)
    #tcpipIPv6FragPktTimeout.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

    #Fragmentation Packet Time-out Value
    tcpipIPv6RxFragBuffSizeMax = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_RX_FRAGMENTED_BUFFER_SIZE", None)
    tcpipIPv6RxFragBuffSizeMax.setLabel("Maximum RX Fragmented Buffer Size")
    tcpipIPv6RxFragBuffSizeMax.setVisible(True)
    tcpipIPv6RxFragBuffSizeMax.setDescription("Maximum RX Fragmented Buffer Size")
    tcpipIPv6RxFragBuffSizeMax.setDefaultValue(1514)
    #tcpipIPv6RxFragBuffSizeMax.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

    # Enable External Packet Processing
    tcpipIpv6ExtPktProcess = tcpipIPv6Component.createBooleanSymbol("TCPIP_IPV6_EXTERN_PACKET_PROCESS", None)
    tcpipIpv6ExtPktProcess.setLabel("Enable External Packet Processing")
    tcpipIpv6ExtPktProcess.setVisible(True)
    tcpipIpv6ExtPktProcess.setDescription("Allows External Processing of RX Packets")
    tcpipIpv6ExtPktProcess.setDefaultValue(False)
    
    tcpipIpv6heapdependency = ["tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]    
        
    # IPv6 Heap Size
    tcpipIpv6HeapSize = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_HEAP_SIZE", None)
    tcpipIpv6HeapSize.setLabel("IPv6 Heap Size (bytes)") 
    tcpipIpv6HeapSize.setVisible(False)
    tcpipIpv6HeapSize.setDefaultValue(tcpipIpv6HeapCalc())
    tcpipIpv6HeapSize.setReadOnly(True)
    tcpipDhcpcHeapSize.setDependencies(tcpipIpv6HeapUpdate, tcpipIpv6heapdependency)
    
    #Add to system_config.h
    tcpipIPv6HeaderFtl = tcpipIPv6Component.createFileSymbol(None, None)
    tcpipIPv6HeaderFtl.setSourcePath("tcpip/config/ipv6.h.ftl")
    tcpipIPv6HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipIPv6HeaderFtl.setMarkup(True)
    tcpipIPv6HeaderFtl.setType("STRING")

    # Add ipv6.c file
    tcpipIPv6SourceFile = tcpipIPv6Component.createFileSymbol(None, None)
    tcpipIPv6SourceFile.setSourcePath("tcpip/src/ipv6.c")
    tcpipIPv6SourceFile.setOutputName("ipv6.c")
    tcpipIPv6SourceFile.setOverwrite(True)
    tcpipIPv6SourceFile.setDestPath("library/tcpip/src/")
    tcpipIPv6SourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipIPv6SourceFile.setType("SOURCE")
    tcpipIPv6SourceFile.setEnabled(True)
    #tcpipIPv6SourceFile.setDependencies(tcpipIpv6GenSourceFile, ["TCPIP_STACK_USE_IPV6"])

def tcpipIPv6MenuVisible(symbol, event):
    if (event["value"] == True):
        print("TCPIP Menu Visible.")        
        symbol.setVisible(True)
    else:
        print("TCPIP Menu Invisible.")
        symbol.setVisible(False)
        
def tcpipIpv6GenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])
    
def tcpipIpv6HeapCalc():     
    heap_size = 1024
    return heap_size    
    
def tcpipIpv6HeapUpdate(symbol, event): 
    heap_size = tcpipIpv6HeapCalc()
    symbol.setValue(heap_size)
    if(event["id"] == "TCPIP_STACK_HEAP_CALC_MASK"):
        symbol.setVisible(event["value"])
    
def destroyComponent(component):
    Database.setSymbolValue("tcpipIPv6", "TCPIP_STACK_USE_IPV6", False, 2)
    
