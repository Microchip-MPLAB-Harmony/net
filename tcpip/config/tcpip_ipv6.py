"""
Copyright (C) 2019-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
"""
TCPIP_IPV6_RIID_TYPES = ["RIID Default Generation", "RIID Custom Generation"]

def tcpipIPv6RiidDefaultVisible(symbol, event):
    if (event["value"] == TCPIP_IPV6_RIID_TYPES[0]):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def tcpipIPv6RiidCustomVisible(symbol, event):
    if (event["value"] == TCPIP_IPV6_RIID_TYPES[1]):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def tcpipIPv6RiidFlagVisible(symbol, event):
    if (event["value"] == True):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

    

    
def instantiateComponent(tcpipIPv6Component):
    print("TCPIP IPv6 Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # Enable IPv6   
    tcpipIPv6 = tcpipIPv6Component.createBooleanSymbol("TCPIP_STACK_USE_IPV6", None)
    tcpipIPv6.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6.setLabel("IPv6")
    tcpipIPv6.setVisible(False)
    tcpipIPv6.setDescription("Enable IPv6")
    tcpipIPv6.setDefaultValue(True)

    # Advanced Settings
    tcpipIPv6AdvSettings = tcpipIPv6Component.createMenuSymbol("TCPIP_IPV6_ADV_SETTING", None)
    tcpipIPv6AdvSettings.setLabel("Advanced Settings")
    tcpipIPv6AdvSettings.setDescription("Advanced Settings")
    tcpipIPv6AdvSettings.setVisible(True)

    #IPv6 Task Processing Rate in ms
    tcpipIPv6TaskProcessRate = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_TASK_PROCESS_RATE", tcpipIPv6AdvSettings)
    tcpipIPv6TaskProcessRate.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6TaskProcessRate.setLabel("IPv6 Task Processing Rate (in msec)")
    tcpipIPv6TaskProcessRate.setVisible(True)
    tcpipIPv6TaskProcessRate.setDescription("IPv6 Task Processing Rate in ms")
    tcpipIPv6TaskProcessRate.setDefaultValue(1000)

    #IPv6 Init Task Processing Rate in ms
    tcpipIPv6InitTaskProcessRate = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_INIT_TASK_PROCESS_RATE", tcpipIPv6AdvSettings)
    tcpipIPv6InitTaskProcessRate.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6InitTaskProcessRate.setLabel("IPv6 Init Task Processing Rate (in msec)")
    tcpipIPv6InitTaskProcessRate.setVisible(True)
    tcpipIPv6InitTaskProcessRate.setDescription("IPv6 Init Task Processing Rate in ms")
    tcpipIPv6InitTaskProcessRate.setDefaultValue(32)

    # Minimum Allocation Unit For The Payload
    tcpipIPv6MinAllocBlkSize = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_DEFAULT_ALLOCATION_BLOCK_SIZE", tcpipIPv6AdvSettings)
    tcpipIPv6MinAllocBlkSize.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6MinAllocBlkSize.setLabel("Minimum Allocation Unit For The Payload")
    tcpipIPv6MinAllocBlkSize.setVisible(True)
    tcpipIPv6MinAllocBlkSize.setDescription("Minimum Allocation Unit For The Payload")
    tcpipIPv6MinAllocBlkSize.setDefaultValue(64)

    # Lower Bounds Of The Maximum Transmission Unit
    tcpipIPv6MinLinkMTU = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_MINIMUM_LINK_MTU", tcpipIPv6AdvSettings)
    tcpipIPv6MinLinkMTU.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6MinLinkMTU.setLabel("Lower Bounds Of The Maximum Transmission Unit")
    tcpipIPv6MinLinkMTU.setVisible(True)
    tcpipIPv6MinLinkMTU.setDescription("Lower Bounds Of The Maximum Transmission Unit")
    tcpipIPv6MinLinkMTU.setDefaultValue(1280)

    # Maximum Transmission Unit
    tcpipIPv6LinkMTU = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_DEFAULT_LINK_MTU", tcpipIPv6AdvSettings)
    tcpipIPv6LinkMTU.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6LinkMTU.setLabel("Maximum Transmission Unit")
    tcpipIPv6LinkMTU.setVisible(True)
    tcpipIPv6LinkMTU.setDescription("Maximum Transmission Unit")
    tcpipIPv6LinkMTU.setDefaultValue(1500)

    # IPv6 Default Hop Limit
    tcpipIPv6HopLimit = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_DEFAULT_CUR_HOP_LIMIT", tcpipIPv6AdvSettings)
    tcpipIPv6HopLimit.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6HopLimit.setLabel("IPv6 Default Hop Limit")
    tcpipIPv6HopLimit.setVisible(True)
    tcpipIPv6HopLimit.setDescription("IPv6 Default Hop Limit")
    tcpipIPv6HopLimit.setDefaultValue(64)

    # Router Advertisement Reachable Time in Seconds
    tcpipIPv6RouterAdvReachTime = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_DEFAULT_BASE_REACHABLE_TIME", tcpipIPv6AdvSettings)
    tcpipIPv6RouterAdvReachTime.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6RouterAdvReachTime.setLabel("Router Advertisement Reachable Time (in sec)")
    tcpipIPv6RouterAdvReachTime.setVisible(True)
    tcpipIPv6RouterAdvReachTime.setDescription("Router Advertisement Reachable Time in Seconds")
    tcpipIPv6RouterAdvReachTime.setDefaultValue(30)

    # Router Advertisement's Retransmission Time in ms
    tcpipIPv6RouterAdvRetransTime = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_DEFAULT_RETRANSMIT_TIME", tcpipIPv6AdvSettings)
    tcpipIPv6RouterAdvRetransTime.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6RouterAdvRetransTime.setLabel("Router Advertisement's Retransmission Time (in msec)")
    tcpipIPv6RouterAdvRetransTime.setVisible(True)
    tcpipIPv6RouterAdvRetransTime.setDescription("Router Advertisement's Retransmission Time in ms")
    tcpipIPv6RouterAdvRetransTime.setDefaultValue(1000)

    # Maximum Number of Queued Packets per Remote
    tcpipIPv6QueNeighbPktMaxNum = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_QUEUE_NEIGHBOR_PACKET_LIMIT", tcpipIPv6AdvSettings)
    tcpipIPv6QueNeighbPktMaxNum.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6QueNeighbPktMaxNum.setLabel("Maximum Number of Queued Packets per Remote")
    tcpipIPv6QueNeighbPktMaxNum.setVisible(True)
    tcpipIPv6QueNeighbPktMaxNum.setDescription("Maximum Number of Queued Packets per Remote")
    tcpipIPv6QueNeighbPktMaxNum.setDefaultValue(1)

    # Stale Neighbor Discovery Packet Time-out
    tcpipIPv6NeighbCacheEntryStaleTimeout = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_NEIGHBOR_CACHE_ENTRY_STALE_TIMEOUT", tcpipIPv6AdvSettings)
    tcpipIPv6NeighbCacheEntryStaleTimeout.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6NeighbCacheEntryStaleTimeout.setLabel("Stale Neighbor Discovery Packet Time-out (in sec)")
    tcpipIPv6NeighbCacheEntryStaleTimeout.setVisible(True)
    tcpipIPv6NeighbCacheEntryStaleTimeout.setDescription("Stale Neighbor Discovery Packet Time-out")
    tcpipIPv6NeighbCacheEntryStaleTimeout.setDefaultValue(600)

    # Maximum Number of Multicast Queued IPv6
    tcpipIPv6QueMcastPktMaxNum = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_QUEUE_MCAST_PACKET_LIMIT", tcpipIPv6AdvSettings)
    tcpipIPv6QueMcastPktMaxNum.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6QueMcastPktMaxNum.setLabel("Maximum Number of Multicast Queued IPv6")
    tcpipIPv6QueMcastPktMaxNum.setVisible(True)
    tcpipIPv6QueMcastPktMaxNum.setDescription("Maximum Number of Multicast Queued IPv6")
    tcpipIPv6QueMcastPktMaxNum.setDefaultValue(4)

    # Multicast Packet Time-out in Seconds
    tcpipIPv6QueMcastPktTimeout = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_QUEUED_MCAST_PACKET_TIMEOUT", tcpipIPv6AdvSettings)
    tcpipIPv6QueMcastPktTimeout.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6QueMcastPktTimeout.setLabel("Multicast Packet Time-out (in sec)")
    tcpipIPv6QueMcastPktTimeout.setVisible(True)
    tcpipIPv6QueMcastPktTimeout.setDescription("Multicast Packet Time-out in Seconds")
        
    # Enable IPv6 ULA Generation
    tcpipIPv6UlaEnable = tcpipIPv6Component.createBooleanSymbol("TCPIP_IPV6_ULA_GENERATE_ENABLE", tcpipIPv6AdvSettings)
    tcpipIPv6UlaEnable.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6UlaEnable.setLabel("Enable ULA Generation")
    tcpipIPv6UlaEnable.setVisible(True)
    tcpipIPv6UlaEnable.setDescription("Enable ULA Generation")
    tcpipIPv6UlaEnable.setDefaultValue(False)
    
    #NTP Access Time-out For The IPv6 ULA Address Generation in ms
    tcpipIPv6UlaNtpAccessTimeout = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_ULA_NTP_ACCESS_TMO", tcpipIPv6UlaEnable)
    tcpipIPv6UlaNtpAccessTimeout.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6UlaNtpAccessTimeout.setLabel("NTP Access Time-out For The IPv6 ULA Address Generation (in msec)")
    tcpipIPv6UlaNtpAccessTimeout.setVisible(False)
    tcpipIPv6UlaNtpAccessTimeout.setDescription("NTP Access Time-out For The IPv6 ULA Address Generation in ms")
    tcpipIPv6UlaNtpAccessTimeout.setDefaultValue(12000)
    tcpipIPv6UlaNtpAccessTimeout.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_IPV6_ULA_GENERATE_ENABLE"])

    #NTP Time Stamp Validity Window in ms
    tcpipIPv6UlaNtpValidWindowTimestamp = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_ULA_NTP_VALID_WINDOW", tcpipIPv6UlaEnable)
    tcpipIPv6UlaNtpValidWindowTimestamp.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6UlaNtpValidWindowTimestamp.setLabel("NTP Time Stamp Validity Window (in msec)")
    tcpipIPv6UlaNtpValidWindowTimestamp.setVisible(False)
    tcpipIPv6UlaNtpValidWindowTimestamp.setDescription("NTP Time Stamp Validity Window in ms")
    tcpipIPv6UlaNtpValidWindowTimestamp.setDefaultValue(1000)
    tcpipIPv6UlaNtpValidWindowTimestamp.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_IPV6_ULA_GENERATE_ENABLE"])

    #Fragmentation Packet Time-out Value
    tcpipIPv6FragPktTimeout = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_FRAGMENT_PKT_TIMEOUT", tcpipIPv6AdvSettings)
    tcpipIPv6FragPktTimeout.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6FragPktTimeout.setLabel("Fragmentation Packet Time-out (in sec)")
    tcpipIPv6FragPktTimeout.setVisible(True)
    tcpipIPv6FragPktTimeout.setDescription("Fragmentation Packet Time-out Value")
    tcpipIPv6FragPktTimeout.setDefaultValue(60)

    #Fragmentation Packet Time-out Value
    tcpipIPv6RxFragBuffSizeMax = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_RX_FRAGMENTED_BUFFER_SIZE", tcpipIPv6AdvSettings)
    tcpipIPv6RxFragBuffSizeMax.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6RxFragBuffSizeMax.setLabel("Maximum RX Fragmented Buffer Size")
    tcpipIPv6RxFragBuffSizeMax.setVisible(True)
    tcpipIPv6RxFragBuffSizeMax.setDescription("Maximum RX Fragmented Buffer Size")
    tcpipIPv6RxFragBuffSizeMax.setDefaultValue(1514)

    # Enable External Packet Processing
    tcpipIpv6ExtPktProcess = tcpipIPv6Component.createBooleanSymbol("TCPIP_IPV6_EXTERN_PACKET_PROCESS", tcpipIPv6AdvSettings)
    tcpipIpv6ExtPktProcess.setHelp("mcc_h3_ipv6_configurations")
    tcpipIpv6ExtPktProcess.setLabel("Enable External Packet Processing")
    tcpipIpv6ExtPktProcess.setVisible(True)
    tcpipIpv6ExtPktProcess.setDescription("Allows External Processing of RX Packets")
    tcpipIpv6ExtPktProcess.setDefaultValue(False)
    tcpipIPv6QueMcastPktTimeout.setDefaultValue(10)
    
    # Enable G3-PLC Support
    tcpipIPv6G3PlcEnable = tcpipIPv6Component.createBooleanSymbol("TCPIP_IPV6_G3_PLC_SUPPORT", tcpipIPv6AdvSettings)
    tcpipIPv6G3PlcEnable.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6G3PlcEnable.setLabel("Enable G3-PLC Support")
    tcpipIPv6G3PlcEnable.setVisible(True)
    tcpipIPv6G3PlcEnable.setDescription("Enable G3-PLC Network Support")
    tcpipIPv6G3PlcEnable.setDefaultValue(False)

    # G3-PLC router or device
    tcpipIPv6G3PlcRouterEnable = tcpipIPv6Component.createBooleanSymbol("TCPIP_IPV6_G3_PLC_BORDER_ROUTER", tcpipIPv6G3PlcEnable)
    tcpipIPv6G3PlcRouterEnable.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6G3PlcRouterEnable.setLabel("Enable G3-PLC router support")
    tcpipIPv6G3PlcRouterEnable.setVisible(False)
    tcpipIPv6G3PlcRouterEnable.setDescription("Enable host to act as a G3-PLC router")
    tcpipIPv6G3PlcRouterEnable.setDefaultValue(False)
    tcpipIPv6G3PlcRouterEnable.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_IPV6_G3_PLC_SUPPORT"])

    # Enable IPv6 RIID Generation
    tcpipIPv6RiidEnable = tcpipIPv6Component.createBooleanSymbol("TCPIP_IPV6_RIID_ENABLE", tcpipIPv6AdvSettings)
    tcpipIPv6RiidEnable.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6RiidEnable.setLabel("Enable RIID Generation")
    tcpipIPv6RiidEnable.setVisible(True)
    tcpipIPv6RiidEnable.setDescription("Enable Random Interface ID Generation")
    tcpipIPv6RiidEnable.setDefaultValue(False)
    
    # RIID Generation Selection
    tcpipIPv6RiidSelection = tcpipIPv6Component.createComboSymbol("TCPIP_IPV6_RIID_SELECTION", tcpipIPv6RiidEnable, TCPIP_IPV6_RIID_TYPES)
    tcpipIPv6RiidSelection.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6RiidSelection.setLabel("Select IPv6 RIID Generation")
    tcpipIPv6RiidSelection.setVisible(False)
    tcpipIPv6RiidSelection.setDescription("RIID Generation Selection")
    tcpipIPv6RiidSelection.setDefaultValue("RIID Default Generation")
    tcpipIPv6RiidSelection.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_IPV6_RIID_ENABLE"])

    # RIID Default Settings
    # RIID Secret Key function
    tcpipIPv6RiidSecKeyF = tcpipIPv6Component.createStringSymbol("TCPIP_IPV6_RIID_SEC_KEY_F", tcpipIPv6RiidSelection)
    tcpipIPv6RiidSecKeyF.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6RiidSecKeyF.setLabel("IPv6 RIID Secret Key function")
    tcpipIPv6RiidSecKeyF.setVisible(True)
    tcpipIPv6RiidSecKeyF.setDescription("Mandatory Custom IPv6 Secret Key function for generating the RIID")
    tcpipIPv6RiidSecKeyF.setDefaultValue("")
    tcpipIPv6RiidSecKeyF.setDependencies(tcpipIPv6RiidDefaultVisible, ["TCPIP_IPV6_RIID_SELECTION"])
    
    # RIID Default Settings
    # RIID NET_IFACE function
    tcpipIPv6RiidNetIfaceF = tcpipIPv6Component.createStringSymbol("TCPIP_IPV6_RIID_NET_IFACE_F", tcpipIPv6RiidSelection)
    tcpipIPv6RiidNetIfaceF.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6RiidNetIfaceF.setLabel("IPv6 RIID NET_IFACE function")
    tcpipIPv6RiidNetIfaceF.setVisible(True)
    tcpipIPv6RiidNetIfaceF.setDescription("Optional IPv6 NET_IFACE function for generating the RIID")
    tcpipIPv6RiidNetIfaceF.setDefaultValue("")
    tcpipIPv6RiidNetIfaceF.setDependencies(tcpipIPv6RiidDefaultVisible, ["TCPIP_IPV6_RIID_SELECTION"])

    # RIID Default Settings
    # RIID NET_ID function
    tcpipIPv6RiidNetIdF = tcpipIPv6Component.createStringSymbol("TCPIP_IPV6_RIID_NET_ID_F", tcpipIPv6RiidSelection)
    tcpipIPv6RiidNetIdF.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6RiidNetIdF.setLabel("IPv6 RIID NET_ID function")
    tcpipIPv6RiidNetIdF.setVisible(True)
    tcpipIPv6RiidNetIdF.setDescription("Optional IPv6 NET_ID function for generating the RIID")
    tcpipIPv6RiidNetIdF.setDefaultValue("")
    tcpipIPv6RiidNetIdF.setDependencies(tcpipIPv6RiidDefaultVisible, ["TCPIP_IPV6_RIID_SELECTION"])

    # RIID Custom PRF function
    tcpipIPv6RiidPrF = tcpipIPv6Component.createStringSymbol("TCPIP_IPV6_RIID_PRF", tcpipIPv6RiidSelection)
    tcpipIPv6RiidPrF.setHelp("mcc_h3_ipv6_configurations")
    tcpipIPv6RiidPrF.setLabel("IPv6 RIID PRF function")
    tcpipIPv6RiidPrF.setVisible(False)
    tcpipIPv6RiidPrF.setDescription("Custom IPv6 PRF function for generating the RIID")
    tcpipIPv6RiidPrF.setDefaultValue("")
    tcpipIPv6RiidPrF.setDependencies(tcpipIPv6RiidCustomVisible, ["TCPIP_IPV6_RIID_SELECTION"])
    
    # IPv6 Configuration Flags Settings
    tcpipIpv6ConfigFlag = tcpipIPv6Component.createMenuSymbol(None, tcpipIPv6AdvSettings)
    tcpipIpv6ConfigFlag.setLabel("Configuration Flags")
    tcpipIpv6ConfigFlag.setVisible(True)
    tcpipIpv6ConfigFlag.setDescription("IPv6 Configuration Flags Settings")

    # IPv6 start with RIID option
    tcpipIpv6ConfigFlagStartRiid = tcpipIPv6Component.createBooleanSymbol("TCPIP_IPV6_CONFIG_FLAG_RIID", tcpipIpv6ConfigFlag)
    tcpipIpv6ConfigFlagStartRiid.setHelp("mcc_h3_ipv6_configurations")
    tcpipIpv6ConfigFlagStartRiid.setLabel("IPv6 RIID Generation option")
    tcpipIpv6ConfigFlagStartRiid.setVisible(False)
    tcpipIpv6ConfigFlagStartRiid.setDescription("Start IPv6 with RIID Generation")
    tcpipIpv6ConfigFlagStartRiid.setDefaultValue(False)
    tcpipIpv6ConfigFlagStartRiid.setDependencies(tcpipIPv6RiidFlagVisible, ["TCPIP_IPV6_RIID_ENABLE"])

    tcpipIpv6heapdependency = ["tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]    
        
    # IPv6 Heap Size
    tcpipIpv6HeapSize = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_HEAP_SIZE", None)
    tcpipIpv6HeapSize.setLabel("IPv6 Heap Size (bytes)") 
    tcpipIpv6HeapSize.setVisible(False)
    tcpipIpv6HeapSize.setDefaultValue(tcpipIpv6HeapCalc())
    tcpipIpv6HeapSize.setReadOnly(True)
    tcpipIpv6HeapSize.setDependencies(tcpipIpv6HeapUpdate, tcpipIpv6heapdependency)
    
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

#Set symbols of other components
def setVal(component, symbol, value):
    triggerDict = {"Component":component,"Id":symbol, "Value":value}
    if(Database.sendMessage(component, "SET_SYMBOL", triggerDict) == None):
        print "Set Symbol Failure" + component + ":" + symbol + ":" + str(value)
        return False
    else:
        return True

#Handle messages from other components
def handleMessage(messageID, args):
    retDict= {}
    if (messageID == "SET_SYMBOL"):
        print "handleMessage: Set Symbol"
        retDict= {"Return": "Success"}
        Database.setSymbolValue(args["Component"], args["Id"], args["Value"])
    else:
        retDict= {"Return": "UnImplemented Command"}
    return retDict
        
def destroyComponent(component):
    Database.setSymbolValue("tcpipIPv6", "TCPIP_STACK_USE_IPV6", False, 2)
    
