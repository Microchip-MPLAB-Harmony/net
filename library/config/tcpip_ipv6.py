    
def instantiateComponent(tcpipIPv6Component):
	print("TCPIP IPv6 Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	
	# Enable IPv6 	
	tcpipIPv6 = tcpipIPv6Component.createBooleanSymbol("TCPIP_STACK_USE_IPV6", None)
	tcpipIPv6.setLabel("IPv6")
	tcpipIPv6.setVisible(True)
	tcpipIPv6.setDescription("Enable IPv6")
	tcpipIPv6.setDefaultValue(False)

	# Minimum Allocation Unit For The Payload
	tcpipIPv6MinAllocBlkSize = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_DEFAULT_ALLOCATION_BLOCK_SIZE", tcpipIPv6)
	tcpipIPv6MinAllocBlkSize.setLabel("Minimum Allocation Unit For The Payload")
	tcpipIPv6MinAllocBlkSize.setVisible(False)
	tcpipIPv6MinAllocBlkSize.setDescription("Minimum Allocation Unit For The Payload")
	tcpipIPv6MinAllocBlkSize.setDefaultValue(64)
	tcpipIPv6MinAllocBlkSize.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

	# Lower Bounds Of The Maximum Transmission Unit
	tcpipIPv6MinLinkMTU = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_MINIMUM_LINK_MTU", tcpipIPv6)
	tcpipIPv6MinLinkMTU.setLabel("Lower Bounds Of The Maximum Transmission Unit")
	tcpipIPv6MinLinkMTU.setVisible(False)
	tcpipIPv6MinLinkMTU.setDescription("Lower Bounds Of The Maximum Transmission Unit")
	tcpipIPv6MinLinkMTU.setDefaultValue(1280)
	tcpipIPv6MinLinkMTU.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

	# Maximum Transmission Unit
	tcpipIPv6LinkMTU = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_DEFAULT_LINK_MTU", tcpipIPv6)
	tcpipIPv6LinkMTU.setLabel("Maximum Transmission Unit")
	tcpipIPv6LinkMTU.setVisible(False)
	tcpipIPv6LinkMTU.setDescription("Maximum Transmission Unit")
	tcpipIPv6LinkMTU.setDefaultValue(1500)
	tcpipIPv6LinkMTU.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

	# IPv6 Default Hop Limit
	tcpipIPv6HopLimit = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_DEFAULT_CUR_HOP_LIMIT", tcpipIPv6)
	tcpipIPv6HopLimit.setLabel("IPv6 Default Hop Limit")
	tcpipIPv6HopLimit.setVisible(False)
	tcpipIPv6HopLimit.setDescription("IPv6 Default Hop Limit")
	tcpipIPv6HopLimit.setDefaultValue(64)
	tcpipIPv6HopLimit.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

	# Router Advertisement Reachable Time in Seconds
	tcpipIPv6RouterAdvReachTime = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_DEFAULT_BASE_REACHABLE_TIME", tcpipIPv6)
	tcpipIPv6RouterAdvReachTime.setLabel("Router Advertisement Reachable Time in Seconds")
	tcpipIPv6RouterAdvReachTime.setVisible(False)
	tcpipIPv6RouterAdvReachTime.setDescription("Router Advertisement Reachable Time in Seconds")
	tcpipIPv6RouterAdvReachTime.setDefaultValue(30)
	tcpipIPv6RouterAdvReachTime.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

	# Router Advertisement's Retransmission Time in ms
	tcpipIPv6RouterAdvRetransTime = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_DEFAULT_RETRANSMIT_TIME", tcpipIPv6)
	tcpipIPv6RouterAdvRetransTime.setLabel("Router Advertisement's Retransmission Time in ms")
	tcpipIPv6RouterAdvRetransTime.setVisible(False)
	tcpipIPv6RouterAdvRetransTime.setDescription("Router Advertisement's Retransmission Time in ms")
	tcpipIPv6RouterAdvRetransTime.setDefaultValue(1000)
	tcpipIPv6RouterAdvRetransTime.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

	# Maximum Number of Queued Packets per Remote
	tcpipIPv6QueNeighbPktMaxNum = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_QUEUE_NEIGHBOR_PACKET_LIMIT", tcpipIPv6)
	tcpipIPv6QueNeighbPktMaxNum.setLabel("Maximum Number of Queued Packets per Remote")
	tcpipIPv6QueNeighbPktMaxNum.setVisible(False)
	tcpipIPv6QueNeighbPktMaxNum.setDescription("Maximum Number of Queued Packets per Remote")
	tcpipIPv6QueNeighbPktMaxNum.setDefaultValue(1)
	tcpipIPv6QueNeighbPktMaxNum.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

	# Stale Neighbor Discovery Packet Time-out
	tcpipIPv6NeighbCacheEntryStaleTimeout = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_NEIGHBOR_CACHE_ENTRY_STALE_TIMEOUT", tcpipIPv6)
	tcpipIPv6NeighbCacheEntryStaleTimeout.setLabel("Stale Neighbor Discovery Packet Time-out")
	tcpipIPv6NeighbCacheEntryStaleTimeout.setVisible(False)
	tcpipIPv6NeighbCacheEntryStaleTimeout.setDescription("Stale Neighbor Discovery Packet Time-out")
	tcpipIPv6NeighbCacheEntryStaleTimeout.setDefaultValue(600)
	tcpipIPv6NeighbCacheEntryStaleTimeout.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

	# Maximum Number of Multicast Queued IPv6
	tcpipIPv6QueMcastPktMaxNum = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_QUEUE_MCAST_PACKET_LIMIT", tcpipIPv6)
	tcpipIPv6QueMcastPktMaxNum.setLabel("Maximum Number of Multicast Queued IPv6")
	tcpipIPv6QueMcastPktMaxNum.setVisible(False)
	tcpipIPv6QueMcastPktMaxNum.setDescription("Maximum Number of Multicast Queued IPv6")
	tcpipIPv6QueMcastPktMaxNum.setDefaultValue(4)
	tcpipIPv6QueMcastPktMaxNum.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

	# Multicast Packet Time-out in Seconds
	tcpipIPv6QueMcastPktTimeout = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_QUEUED_MCAST_PACKET_TIMEOUT", tcpipIPv6)
	tcpipIPv6QueMcastPktTimeout.setLabel("Multicast Packet Time-out - Seconds")
	tcpipIPv6QueMcastPktTimeout.setVisible(False)
	tcpipIPv6QueMcastPktTimeout.setDescription("Multicast Packet Time-out in Seconds")
	tcpipIPv6QueMcastPktTimeout.setDefaultValue(10)
	tcpipIPv6QueMcastPktTimeout.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

	#IPv6 Task Processing Rate in ms
	tcpipIPv6TaskProcessRate = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_TASK_PROCESS_RATE", tcpipIPv6)
	tcpipIPv6TaskProcessRate.setLabel("IPv6 Task Processing Rate - ms")
	tcpipIPv6TaskProcessRate.setVisible(False)
	tcpipIPv6TaskProcessRate.setDescription("IPv6 Task Processing Rate in ms")
	tcpipIPv6TaskProcessRate.setDefaultValue(1000)
	tcpipIPv6TaskProcessRate.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

	#IPv6 Init Task Processing Rate in ms
	tcpipIPv6InitTaskProcessRate = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_INIT_TASK_PROCESS_RATE", tcpipIPv6)
	tcpipIPv6InitTaskProcessRate.setLabel("IPv6 Init Task Processing Rate - ms")
	tcpipIPv6InitTaskProcessRate.setVisible(False)
	tcpipIPv6InitTaskProcessRate.setDescription("IPv6 Init Task Processing Rate in ms")
	tcpipIPv6InitTaskProcessRate.setDefaultValue(32)
	tcpipIPv6InitTaskProcessRate.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

	#NTP Access Time-out For The IPv6 ULA Address Generation in ms
	tcpipIPv6UlaNtpAccessTimeout = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_ULA_NTP_ACCESS_TMO", tcpipIPv6)
	tcpipIPv6UlaNtpAccessTimeout.setLabel("NTP Access Time-out For The IPv6 ULA Address Generation - ms")
	tcpipIPv6UlaNtpAccessTimeout.setVisible(False)
	tcpipIPv6UlaNtpAccessTimeout.setDescription("NTP Access Time-out For The IPv6 ULA Address Generation in ms")
	tcpipIPv6UlaNtpAccessTimeout.setDefaultValue(12000)
	tcpipIPv6UlaNtpAccessTimeout.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

	#NTP Time Stamp Validity Window in ms
	tcpipIPv6UlaNtpValidWindowTimestamp = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_ULA_NTP_VALID_WINDOW", tcpipIPv6)
	tcpipIPv6UlaNtpValidWindowTimestamp.setLabel("NTP Time Stamp Validity Window - ms")
	tcpipIPv6UlaNtpValidWindowTimestamp.setVisible(False)
	tcpipIPv6UlaNtpValidWindowTimestamp.setDescription("NTP Time Stamp Validity Window in ms")
	tcpipIPv6UlaNtpValidWindowTimestamp.setDefaultValue(1000)
	tcpipIPv6UlaNtpValidWindowTimestamp.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

	#Fragmentation Packet Time-out Value
	tcpipIPv6FragPktTimeout = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_FRAGMENT_PKT_TIMEOUT", tcpipIPv6)
	tcpipIPv6FragPktTimeout.setLabel("Fragmentation Packet Time-out Value")
	tcpipIPv6FragPktTimeout.setVisible(False)
	tcpipIPv6FragPktTimeout.setDescription("Fragmentation Packet Time-out Value")
	tcpipIPv6FragPktTimeout.setDefaultValue(60)
	tcpipIPv6FragPktTimeout.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

	#Fragmentation Packet Time-out Value
	tcpipIPv6RxFragBuffSizeMax = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_RX_FRAGMENTED_BUFFER_SIZE", tcpipIPv6)
	tcpipIPv6RxFragBuffSizeMax.setLabel("Maximum RX Fragmented Buffer Size")
	tcpipIPv6RxFragBuffSizeMax.setVisible(False)
	tcpipIPv6RxFragBuffSizeMax.setDescription("Maximum RX Fragmented Buffer Size")
	tcpipIPv6RxFragBuffSizeMax.setDefaultValue(1514)
	tcpipIPv6RxFragBuffSizeMax.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

	# Use ICMPv6 Server
	tcpipIPv6UseIcmpv6Server = tcpipIPv6Component.createBooleanSymbol("TCPIP_STACK_USE_ICMPV6_SERVER", tcpipIPv6)
	tcpipIPv6UseIcmpv6Server.setLabel("Use ICMPv6 Server")
	tcpipIPv6UseIcmpv6Server.setVisible(False)
	tcpipIPv6UseIcmpv6Server.setDescription("Use ICMPv6 Server")
	tcpipIPv6UseIcmpv6Server.setDefaultValue(True)
	tcpipIPv6UseIcmpv6Server.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

	# Use ICMPv6 Client
	tcpipIPv6UseIcmpv6Client = tcpipIPv6Component.createBooleanSymbol("TCPIP_STACK_USE_ICMPV6_CLIENT", tcpipIPv6)
	tcpipIPv6UseIcmpv6Client.setLabel("Use ICMPv6 Client")
	tcpipIPv6UseIcmpv6Client.setVisible(False)
	tcpipIPv6UseIcmpv6Client.setDescription("Use ICMPv6 Client")
	tcpipIPv6UseIcmpv6Client.setDefaultValue(False)
	tcpipIPv6UseIcmpv6Client.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

	# Enable IPV6 User Notification
	tcpipIPv6UseIcmpv6Client = tcpipIPv6Component.createBooleanSymbol("TCPIP_ICMPV6_CLIENT_USER_NOTIFICATION", tcpipIPv6UseIcmpv6Client)
	tcpipIPv6UseIcmpv6Client.setLabel("Enable IPV6 User Notification")
	tcpipIPv6UseIcmpv6Client.setVisible(False)
	tcpipIPv6UseIcmpv6Client.setDescription("Enable IPV6 User Notification")
	tcpipIPv6UseIcmpv6Client.setDefaultValue(True)
	tcpipIPv6UseIcmpv6Client.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_ICMPV6_CLIENT"])

	# niyas use Variables.get("__TCPIP_DIR") 
	#execfile(__TCPIP_DIR + "/library/config/ndp.py")

	# Add ipv6.h file to project
	tcpipIPv6HeaderFile = tcpipIPv6Component.createFileSymbol(None, None)
	tcpipIPv6HeaderFile.setSourcePath("library/ipv6.h")
	tcpipIPv6HeaderFile.setOutputName("ipv6.h")
	tcpipIPv6HeaderFile.setDestPath("library/tcpip/")
	tcpipIPv6HeaderFile.setProjectPath("config/" + configName + "library/tcpip/")
	tcpipIPv6HeaderFile.setType("HEADER")
	tcpipIPv6HeaderFile.setOverwrite(True)

	# Add ipv6_manager.h file to project
	tcpipIPv6ManagerHeaderFile = tcpipIPv6Component.createFileSymbol(None, None)
	tcpipIPv6ManagerHeaderFile.setSourcePath("library/src/ipv6_manager.h")
	tcpipIPv6ManagerHeaderFile.setOutputName("ipv6_manager.h")
	tcpipIPv6ManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipIPv6ManagerHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipIPv6ManagerHeaderFile.setType("HEADER")
	tcpipIPv6ManagerHeaderFile.setOverwrite(True)

	# Add ipv6_private.h file to project
	tcpipIPv6PrivateHeaderFile = tcpipIPv6Component.createFileSymbol(None, None)
	tcpipIPv6PrivateHeaderFile.setSourcePath("library/src/ipv6_private.h")
	tcpipIPv6PrivateHeaderFile.setOutputName("ipv6_private.h")
	tcpipIPv6PrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipIPv6PrivateHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipIPv6PrivateHeaderFile.setType("HEADER")
	tcpipIPv6PrivateHeaderFile.setOverwrite(True)

	# Add icmpv6.h file to project
	tcpipICMPv6HeaderFile = tcpipIPv6Component.createFileSymbol(None, None)
	tcpipICMPv6HeaderFile.setSourcePath("library/icmpv6.h")
	tcpipICMPv6HeaderFile.setOutputName("icmpv6.h")
	tcpipICMPv6HeaderFile.setDestPath("library/tcpip/")
	tcpipICMPv6HeaderFile.setProjectPath("config/" + configName + "library/tcpip/")
	tcpipICMPv6HeaderFile.setType("HEADER")
	tcpipICMPv6HeaderFile.setOverwrite(True)

	# Add icmpv6_manager.h file to project
	tcpipICMPv6ManagerHeaderFile = tcpipIPv6Component.createFileSymbol(None, None)
	tcpipICMPv6ManagerHeaderFile.setSourcePath("library/src/icmpv6_manager.h")
	tcpipICMPv6ManagerHeaderFile.setOutputName("icmpv6_manager.h")
	tcpipICMPv6ManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipICMPv6ManagerHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipICMPv6ManagerHeaderFile.setType("HEADER")
	tcpipICMPv6ManagerHeaderFile.setOverwrite(True)

	# Add ipv6.c file
	tcpipIPv6SourceFile = tcpipIPv6Component.createFileSymbol(None, None)
	tcpipIPv6SourceFile.setSourcePath("library/src/ipv6.c")
	tcpipIPv6SourceFile.setOutputName("ipv6.c")
	tcpipIPv6SourceFile.setOverwrite(True)
	tcpipIPv6SourceFile.setDestPath("library/tcpip/src/")
	tcpipIPv6SourceFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipIPv6SourceFile.setType("SOURCE")
	tcpipIPv6SourceFile.setEnabled(False)
	tcpipIPv6SourceFile.setDependencies(tcpipIpv6GenSourceFile, ["TCPIP_STACK_USE_IPV6"])

	# Add icmpv6.c file
	tcpipICMPv6SourceFile = tcpipIPv6Component.createFileSymbol(None, None)
	tcpipICMPv6SourceFile.setSourcePath("library/src/icmpv6.c")
	tcpipICMPv6SourceFile.setOutputName("icmpv6.c")
	tcpipICMPv6SourceFile.setOverwrite(True)
	tcpipICMPv6SourceFile.setDestPath("library/tcpip/src/")
	tcpipICMPv6SourceFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipICMPv6SourceFile.setType("SOURCE")
	tcpipICMPv6SourceFile.setEnabled(False)
	tcpipICMPv6SourceFile.setDependencies(tcpipIpv6GenSourceFile, ["TCPIP_STACK_USE_IPV6"])

def tcpipIPv6MenuVisible(symbol, event):
	if (event["value"] == True):
		print("TCPIP Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("TCPIP Menu Invisible.")
		symbol.setVisible(False)
		
def tcpipIpv6GenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])