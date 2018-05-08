    
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

	#NTP Access Time-out For The IPv6 ULA Address Generation in ms
	tcpipIPv6UlaNtpAccessTimeout = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_ULA_NTP_ACCESS_TMO", None)
	tcpipIPv6UlaNtpAccessTimeout.setLabel("NTP Access Time-out For The IPv6 ULA Address Generation - ms")
	tcpipIPv6UlaNtpAccessTimeout.setVisible(True)
	tcpipIPv6UlaNtpAccessTimeout.setDescription("NTP Access Time-out For The IPv6 ULA Address Generation in ms")
	tcpipIPv6UlaNtpAccessTimeout.setDefaultValue(12000)
	#tcpipIPv6UlaNtpAccessTimeout.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

	#NTP Time Stamp Validity Window in ms
	tcpipIPv6UlaNtpValidWindowTimestamp = tcpipIPv6Component.createIntegerSymbol("TCPIP_IPV6_ULA_NTP_VALID_WINDOW", None)
	tcpipIPv6UlaNtpValidWindowTimestamp.setLabel("NTP Time Stamp Validity Window - ms")
	tcpipIPv6UlaNtpValidWindowTimestamp.setVisible(True)
	tcpipIPv6UlaNtpValidWindowTimestamp.setDescription("NTP Time Stamp Validity Window in ms")
	tcpipIPv6UlaNtpValidWindowTimestamp.setDefaultValue(1000)
	#tcpipIPv6UlaNtpValidWindowTimestamp.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

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

	# Use ICMPv6 Server
	tcpipIPv6UseIcmpv6Server = tcpipIPv6Component.createBooleanSymbol("TCPIP_STACK_USE_ICMPV6_SERVER", None)
	tcpipIPv6UseIcmpv6Server.setLabel("Use ICMPv6 Server")
	tcpipIPv6UseIcmpv6Server.setVisible(True)
	tcpipIPv6UseIcmpv6Server.setDescription("Use ICMPv6 Server")
	tcpipIPv6UseIcmpv6Server.setDefaultValue(True)
	#tcpipIPv6UseIcmpv6Server.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

	# Use ICMPv6 Client
	tcpipIPv6UseIcmpv6Client = tcpipIPv6Component.createBooleanSymbol("TCPIP_STACK_USE_ICMPV6_CLIENT", None)
	tcpipIPv6UseIcmpv6Client.setLabel("Use ICMPv6 Client")
	tcpipIPv6UseIcmpv6Client.setVisible(True)
	tcpipIPv6UseIcmpv6Client.setDescription("Use ICMPv6 Client")
	tcpipIPv6UseIcmpv6Client.setDefaultValue(False)
	#tcpipIPv6UseIcmpv6Client.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_IPV6"])

	# Enable IPV6 User Notification
	tcpipIPv6UseIcmpv6Client = tcpipIPv6Component.createBooleanSymbol("TCPIP_ICMPV6_CLIENT_USER_NOTIFICATION", tcpipIPv6UseIcmpv6Client)
	tcpipIPv6UseIcmpv6Client.setLabel("Enable IPV6 User Notification")
	tcpipIPv6UseIcmpv6Client.setVisible(False)
	tcpipIPv6UseIcmpv6Client.setDescription("Enable IPV6 User Notification")
	tcpipIPv6UseIcmpv6Client.setDefaultValue(True)
	tcpipIPv6UseIcmpv6Client.setDependencies(tcpipIPv6MenuVisible, ["TCPIP_STACK_USE_ICMPV6_CLIENT"])

	# niyas use Variables.get("__TCPIP_DIR") 
	#execfile(__TCPIP_DIR + "/library/config/ndp.py")

	#Add to system_config.h
	tcpipIPv6HeaderFtl = tcpipIPv6Component.createFileSymbol(None, None)
	tcpipIPv6HeaderFtl.setSourcePath("library/config/ipv6.h.ftl")
	tcpipIPv6HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipIPv6HeaderFtl.setMarkup(True)
	tcpipIPv6HeaderFtl.setType("STRING")

	# Add ipv6.c file
	tcpipIPv6SourceFile = tcpipIPv6Component.createFileSymbol(None, None)
	tcpipIPv6SourceFile.setSourcePath("library/src/ipv6.c")
	tcpipIPv6SourceFile.setOutputName("ipv6.c")
	tcpipIPv6SourceFile.setOverwrite(True)
	tcpipIPv6SourceFile.setDestPath("library/tcpip/src/")
	tcpipIPv6SourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipIPv6SourceFile.setType("SOURCE")
	tcpipIPv6SourceFile.setEnabled(True)
	#tcpipIPv6SourceFile.setDependencies(tcpipIpv6GenSourceFile, ["TCPIP_STACK_USE_IPV6"])

	# Add icmpv6.c file
	tcpipICMPv6SourceFile = tcpipIPv6Component.createFileSymbol(None, None)
	tcpipICMPv6SourceFile.setSourcePath("library/src/icmpv6.c")
	tcpipICMPv6SourceFile.setOutputName("icmpv6.c")
	tcpipICMPv6SourceFile.setOverwrite(True)
	tcpipICMPv6SourceFile.setDestPath("library/tcpip/src/")
	tcpipICMPv6SourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipICMPv6SourceFile.setType("SOURCE")
	tcpipICMPv6SourceFile.setEnabled(True)
	#tcpipICMPv6SourceFile.setDependencies(tcpipIpv6GenSourceFile, ["TCPIP_STACK_USE_IPV6"])

def tcpipIPv6MenuVisible(symbol, event):
	if (event["value"] == True):
		print("TCPIP Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("TCPIP Menu Invisible.")
		symbol.setVisible(False)
		
def tcpipIpv6GenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])