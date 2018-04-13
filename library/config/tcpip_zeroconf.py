def instantiateComponent(tcpipZeroConfComponent):
	print("TCPIP ZEROCONF Component")
	configName = Variables.get("__CONFIGURATION_NAME")
		
	# Use Link Local Zero Config
	tcpipZc = tcpipZeroConfComponent.createBooleanSymbol("TCPIP_USE_LINK_ZERO_CONFIG", None)
	tcpipZc.setLabel("Use Link Local Zero Config")
	tcpipZc.setVisible(True)
	tcpipZc.setDescription("Use Link Local Zero Config")
	tcpipZc.setDefaultValue(False)   # Niyas to check the default value
	tcpipZc.setDependencies(tcpipZeroConfMenuVisibleSingle, ["tcpipIPv4.TCPIP_STACK_USE_IPV4"])

	# Enable Link Local Warning Messages
	tcpipZcWarnZcll = tcpipZeroConfComponent.createBooleanSymbol("TCPIP_ZC_WARN_ZCLL", tcpipZc)
	tcpipZcWarnZcll.setLabel("Enable Link Local Warning Messages")
	tcpipZcWarnZcll.setVisible(False)
	tcpipZcWarnZcll.setDescription("Enable Link Local Warning Messages")
	tcpipZcWarnZcll.setDefaultValue(False) 
	tcpipZcWarnZcll.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_LINK_ZERO_CONFIG"])

	# Enable Link Local Information Messages
	tcpipZcInfoZcll = tcpipZeroConfComponent.createBooleanSymbol("TCPIP_ZC_INFO_ZCLL", tcpipZc)
	tcpipZcInfoZcll.setLabel("Enable Link Local Information Messages")
	tcpipZcInfoZcll.setVisible(False)
	tcpipZcInfoZcll.setDescription("Enable Link Local Information Messages")
	tcpipZcInfoZcll.setDefaultValue(False) 
	tcpipZcInfoZcll.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_LINK_ZERO_CONFIG"])

	# Enable Link Local Debug Messages
	tcpipZcDebugZcll = tcpipZeroConfComponent.createBooleanSymbol("TCPIP_ZC_DEBUG_ZCLL", tcpipZc)
	tcpipZcDebugZcll.setLabel("Enable Link Local Debug Messages")
	tcpipZcDebugZcll.setVisible(False)
	tcpipZcDebugZcll.setDescription("Enable Link Local Debug Messages")
	tcpipZcDebugZcll.setDefaultValue(False) 
	tcpipZcDebugZcll.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_LINK_ZERO_CONFIG"])

	# Probe Wait in seconds
	tcpipZcProbeWait = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_LL_PROBE_WAIT", tcpipZc)
	tcpipZcProbeWait.setLabel("Probe Wait (seconds)")
	tcpipZcProbeWait.setVisible(False)
	tcpipZcProbeWait.setDescription("Probe Wait in seconds")
	tcpipZcProbeWait.setDefaultValue(1)
	tcpipZcProbeWait.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_LINK_ZERO_CONFIG"])

	# Minimum Time Between Probes in seconds
	tcpipZcProbeMin = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_LL_PROBE_MIN", tcpipZc)
	tcpipZcProbeMin.setLabel("Minimum Time Between Probes (seconds)")
	tcpipZcProbeMin.setVisible(False)
	tcpipZcProbeMin.setDescription("Minimum Time Between Probes in seconds")
	tcpipZcProbeMin.setDefaultValue(1)
	tcpipZcProbeMin.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_LINK_ZERO_CONFIG"])

	# Maxiumum Time Between Probes in seconds
	tcpipZcProbeMax = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_LL_PROBE_MAX", tcpipZc)
	tcpipZcProbeMax.setLabel("Maxiumum Time Between Probes (seconds)")
	tcpipZcProbeMax.setVisible(False)
	tcpipZcProbeMax.setDescription("Maxiumum Time Between Probes in seconds")
	tcpipZcProbeMax.setDefaultValue(2)
	tcpipZcProbeMax.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_LINK_ZERO_CONFIG"])

	# Number of Probes
	tcpipZcProbeNum = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_LL_PROBE_NUM", tcpipZc)
	tcpipZcProbeNum.setLabel("Number of Probes")
	tcpipZcProbeNum.setVisible(False)
	tcpipZcProbeNum.setDescription("Number of Probes")
	tcpipZcProbeNum.setDefaultValue(3)
	tcpipZcProbeNum.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_LINK_ZERO_CONFIG"])

	# Delay Before Announcing
	tcpipZcAnnounceWait = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_LL_ANNOUNCE_WAIT", tcpipZc)
	tcpipZcAnnounceWait.setLabel("Delay Before Announcing")
	tcpipZcAnnounceWait.setVisible(False)
	tcpipZcAnnounceWait.setDescription("Delay Before Announcing")
	tcpipZcAnnounceWait.setDefaultValue(2)
	tcpipZcAnnounceWait.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_LINK_ZERO_CONFIG"])

	# Number of Announcement Packets
	tcpipZcAnnounceNum = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_LL_ANNOUNCE_NUM", tcpipZc)
	tcpipZcAnnounceNum.setLabel("Number of Announcement Packets")
	tcpipZcAnnounceNum.setVisible(False)
	tcpipZcAnnounceNum.setDescription("Number of Announcement Packets")
	tcpipZcAnnounceNum.setDefaultValue(2)
	tcpipZcAnnounceNum.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_LINK_ZERO_CONFIG"])

	# Time Between Announcement Packets in seconds
	tcpipZcAnnounceInterval = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_LL_ANNOUNCE_INTERVAL", tcpipZc)
	tcpipZcAnnounceInterval.setLabel("Time Between Announcement Packets (seconds)")
	tcpipZcAnnounceInterval.setVisible(False)
	tcpipZcAnnounceInterval.setDescription("Time Between Announcement Packets in seconds")
	tcpipZcAnnounceInterval.setDefaultValue(2)
	tcpipZcAnnounceInterval.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_LINK_ZERO_CONFIG"])

	# Maximum Conflicts Before Rate Limiting
	tcpipZcConflictMax = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_LL_MAX_CONFLICTS", tcpipZc)
	tcpipZcConflictMax.setLabel("Maximum Conflicts Before Rate Limiting")
	tcpipZcConflictMax.setVisible(False)
	tcpipZcConflictMax.setDescription("Maximum Conflicts Before Rate Limiting")
	tcpipZcConflictMax.setDefaultValue(10)
	tcpipZcConflictMax.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_LINK_ZERO_CONFIG"])

	# Delay Between Attempts in seconds
	tcpipZcRateLimitInterval = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_LL_RATE_LIMIT_INTERVAL", tcpipZc)
	tcpipZcRateLimitInterval.setLabel("Delay Between Attempts (seconds)")
	tcpipZcRateLimitInterval.setVisible(False)
	tcpipZcRateLimitInterval.setDescription("Delay Between Attempts in seconds")
	tcpipZcRateLimitInterval.setDefaultValue(60)
	tcpipZcRateLimitInterval.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_LINK_ZERO_CONFIG"])

	# Rate Between Defensive ARPs in seconds
	tcpipZcDefendInterval = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_LL_DEFEND_INTERVAL", tcpipZc)
	tcpipZcDefendInterval.setLabel("Rate Between Defensive ARPs (seconds)")
	tcpipZcDefendInterval.setVisible(False)
	tcpipZcDefendInterval.setDescription("Rate Between Defensive ARPs in seconds")
	tcpipZcDefendInterval.setDefaultValue(10)
	tcpipZcDefendInterval.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_LINK_ZERO_CONFIG"])

	# Base IP
	tcpipZcBaseIp = tcpipZeroConfComponent.createStringSymbol("TCPIP_ZC_LL_IPV4_LLBASE", tcpipZc) # Niyas to change datatype to hex
	tcpipZcBaseIp.setLabel("Base IP")
	tcpipZcBaseIp.setVisible(False)
	tcpipZcBaseIp.setDescription("Base IP")
	tcpipZcBaseIp.setDefaultValue("0xa9fe0100")
	tcpipZcBaseIp.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_LINK_ZERO_CONFIG"])

	# Base Netmask
	tcpipZcBaseNetmask = tcpipZeroConfComponent.createStringSymbol("TCPIP_ZC_LL_IPV4_LLBASE_MASK", tcpipZc) # Niyas to change datatype to hex
	tcpipZcBaseNetmask.setLabel("Base Netmask")
	tcpipZcBaseNetmask.setVisible(False)
	tcpipZcBaseNetmask.setDescription("Base Netmask")
	tcpipZcBaseNetmask.setDefaultValue("0x0000FFFF")
	tcpipZcBaseNetmask.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_LINK_ZERO_CONFIG"])

	# Link Local Task Rate in ms
	tcpipZcTskTickRate = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_LL_TASK_TICK_RATE", tcpipZc)
	tcpipZcTskTickRate.setLabel("Link Local Task Rate (ms)")
	tcpipZcTskTickRate.setVisible(False)
	tcpipZcTskTickRate.setDescription("Link Local Task Rate in ms")
	tcpipZcTskTickRate.setDefaultValue(333)
	tcpipZcTskTickRate.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_LINK_ZERO_CONFIG"])

	# Use Multicast DNS Zero Config (Bonjour)
	tcpipZcMulticastDns = tcpipZeroConfComponent.createBooleanSymbol("TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG", tcpipZc)
	tcpipZcMulticastDns.setLabel("Use Multicast DNS Zero Config (Bonjour)")
	tcpipZcMulticastDns.setVisible(False)
	tcpipZcMulticastDns.setDescription("Use Multicast DNS Zero Config (Bonjour)")
	tcpipZcMulticastDns.setDefaultValue(False) 
	tcpipZcMulticastDns.setDependencies(tcpipMulticastDnsZcMenuVisible, ["tcpipUdp.TCPIP_USE_UDP", "TCPIP_USE_LINK_ZERO_CONFIG"])

	# Enable Multicast DNS Information Messages
	tcpipZcMdnsInfo = tcpipZeroConfComponent.createBooleanSymbol("TCPIP_ZC_INFO_MDNS", tcpipZcMulticastDns)
	tcpipZcMdnsInfo.setLabel("Enable Multicast DNS Information Messages")
	tcpipZcMdnsInfo.setVisible(False)
	tcpipZcMdnsInfo.setDescription("Enable Multicast DNS Information Messages")
	tcpipZcMdnsInfo.setDefaultValue(False) 
	tcpipZcMdnsInfo.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

	# Enable Multicast DNS Warning Messages
	tcpipZcMdnsWarn = tcpipZeroConfComponent.createBooleanSymbol("TCPIP_ZC_WARN_MDNS", tcpipZcMulticastDns)
	tcpipZcMdnsWarn.setLabel("Enable Multicast DNS Warning Messages")
	tcpipZcMdnsWarn.setVisible(False)
	tcpipZcMdnsWarn.setDescription("Enable Multicast DNS Warning Messages")
	tcpipZcMdnsWarn.setDefaultValue(False) 
	tcpipZcMdnsWarn.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

	# Enable Multicast DNS Debug Messages
	tcpipZcMdnsDebug = tcpipZeroConfComponent.createBooleanSymbol("TCPIP_ZC_DEBUG_MDNS", tcpipZcMulticastDns)
	tcpipZcMdnsDebug.setLabel("Enable Multicast DNS Debug Messages")
	tcpipZcMdnsDebug.setVisible(False)
	tcpipZcMdnsDebug.setDescription("Enable Multicast DNS Debug Messages")
	tcpipZcMdnsDebug.setDefaultValue(False) 
	tcpipZcMdnsDebug.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

	# Task Tick Rate
	tcpipZcMdnsTskTickRate = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_TASK_TICK_RATE", tcpipZcMulticastDns)
	tcpipZcMdnsTskTickRate.setLabel("Task Rate")
	tcpipZcMdnsTskTickRate.setVisible(False)
	tcpipZcMdnsTskTickRate.setDescription("Task Tick Rate")
	tcpipZcMdnsTskTickRate.setDefaultValue(63)
	tcpipZcMdnsTskTickRate.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

	# Port Number
	tcpipZcMdnsPort = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_PORT", tcpipZcMulticastDns)
	tcpipZcMdnsPort.setLabel("Port")
	tcpipZcMdnsPort.setVisible(False)
	tcpipZcMdnsPort.setDescription("Port")
	tcpipZcMdnsPort.setDefaultValue(5353)
	tcpipZcMdnsPort.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

	# Maximum Host Name Size
	tcpipZcMdnsHostNameSizeMax = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_MAX_HOST_NAME_SIZE", tcpipZcMulticastDns)
	tcpipZcMdnsHostNameSizeMax.setLabel("Max Host Name Size")
	tcpipZcMdnsHostNameSizeMax.setVisible(False)
	tcpipZcMdnsHostNameSizeMax.setDescription("Maximum Host Name Size")
	tcpipZcMdnsHostNameSizeMax.setDefaultValue(32)
	tcpipZcMdnsHostNameSizeMax.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

	# Maximum Size Allowed For a Label
	tcpipZcMdnsLabelSizeMax = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_MAX_LABEL_SIZE", tcpipZcMulticastDns)
	tcpipZcMdnsLabelSizeMax.setLabel("Maximum Size Allowed For a Label")
	tcpipZcMdnsLabelSizeMax.setVisible(False)
	tcpipZcMdnsLabelSizeMax.setDescription("Maximum Size Allowed For a Label")
	tcpipZcMdnsLabelSizeMax.setDefaultValue(64)
	tcpipZcMdnsLabelSizeMax.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

	# Maximum Resource Record Name Size
	tcpipZcMdnsRrNameSizeMax = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_MAX_RR_NAME_SIZE", tcpipZcMulticastDns)
	tcpipZcMdnsRrNameSizeMax.setLabel("Max Resource Record Name Size")
	tcpipZcMdnsRrNameSizeMax.setVisible(False)
	tcpipZcMdnsRrNameSizeMax.setDescription("Maximum Resource Record Name Size")
	tcpipZcMdnsRrNameSizeMax.setDefaultValue(256)
	tcpipZcMdnsRrNameSizeMax.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

	# Maximum Server Type Size
	tcpipZcMdnsSrvTypeSizeMax = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_MAX_SRV_TYPE_SIZE", tcpipZcMulticastDns)
	tcpipZcMdnsSrvTypeSizeMax.setLabel("Max Server Type Size")
	tcpipZcMdnsSrvTypeSizeMax.setVisible(False)
	tcpipZcMdnsSrvTypeSizeMax.setDescription("Maximum Server Type Size")
	tcpipZcMdnsSrvTypeSizeMax.setDefaultValue(32)
	tcpipZcMdnsSrvTypeSizeMax.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

	# Maximum Server Name Size
	tcpipZcMdnsSrvNameSizeMax = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_MAX_SRV_NAME_SIZE", tcpipZcMulticastDns)
	tcpipZcMdnsSrvNameSizeMax.setLabel("Max Server Name Size")
	tcpipZcMdnsSrvNameSizeMax.setVisible(False)
	tcpipZcMdnsSrvNameSizeMax.setDescription("Maximum Server Name Size")
	tcpipZcMdnsSrvNameSizeMax.setDefaultValue(64)
	tcpipZcMdnsSrvNameSizeMax.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

	# Maximum Text Data Size
	tcpipZcMdnsTxtDataSizeMax = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_MAX_TXT_DATA_SIZE", tcpipZcMulticastDns)
	tcpipZcMdnsTxtDataSizeMax.setLabel("Max Text Data Size")
	tcpipZcMdnsTxtDataSizeMax.setVisible(False)
	tcpipZcMdnsTxtDataSizeMax.setDescription("Maximum Text Data Size")
	tcpipZcMdnsTxtDataSizeMax.setDefaultValue(128)
	tcpipZcMdnsTxtDataSizeMax.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

	# Time-To-Live for a Resource-Record in seconds
	tcpipZcMdnsRrTtlVal= tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_RESOURCE_RECORD_TTL_VAL", tcpipZcMulticastDns)
	tcpipZcMdnsRrTtlVal.setLabel("Time-To-Live for a Resource-Record (seconds)")
	tcpipZcMdnsRrTtlVal.setVisible(False)
	tcpipZcMdnsRrTtlVal.setDescription("Time-To-Live for a Resource-Record in seconds")
	tcpipZcMdnsRrTtlVal.setDefaultValue(3600)
	tcpipZcMdnsRrTtlVal.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

	# Maximum Number of Resource-Records/Service
	tcpipZcMdnsRrNumMax= tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_MAX_RR_NUM", tcpipZcMulticastDns)
	tcpipZcMdnsRrNumMax.setLabel("Max Number of Resource-Records/Service")
	tcpipZcMdnsRrNumMax.setVisible(False)
	tcpipZcMdnsRrNumMax.setDescription("Maximum Number of Resource-Records/Service")
	tcpipZcMdnsRrNumMax.setDefaultValue(4)
	tcpipZcMdnsRrNumMax.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

	# Initial Random Delay in ms
	tcpipZcMdnsProbeWait= tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_PROBE_WAIT", tcpipZcMulticastDns)
	tcpipZcMdnsProbeWait.setLabel("Initial Random Delay (ms)")
	tcpipZcMdnsProbeWait.setVisible(False)
	tcpipZcMdnsProbeWait.setDescription("Initial Random Delay in ms")
	tcpipZcMdnsProbeWait.setDefaultValue(750)
	tcpipZcMdnsProbeWait.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

	# Maximum Delay Till Repeated Probe in ms
	tcpipZcMdnsProbeInterval= tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_PROBE_INTERVAL", tcpipZcMulticastDns)
	tcpipZcMdnsProbeInterval.setLabel("Maximum Delay Till Repeated Probe (ms)")
	tcpipZcMdnsProbeInterval.setVisible(False)
	tcpipZcMdnsProbeInterval.setDescription("Maximum Delay Till Repeated Probe in ms")
	tcpipZcMdnsProbeInterval.setDefaultValue(250)
	tcpipZcMdnsProbeInterval.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

	# Number of Probes
	tcpipZcMdnsProbeNum= tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_PROBE_NUM", tcpipZcMulticastDns)
	tcpipZcMdnsProbeNum.setLabel("Number of Probes")
	tcpipZcMdnsProbeNum.setVisible(False)
	tcpipZcMdnsProbeNum.setDescription("Number of Probes")
	tcpipZcMdnsProbeNum.setDefaultValue(3)
	tcpipZcMdnsProbeNum.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

	# Maximum Number of Conflicts
	tcpipZcMdnsProbeConflictNumMax= tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_MAX_PROBE_CONFLICT_NUM", tcpipZcMulticastDns)
	tcpipZcMdnsProbeConflictNumMax.setLabel("Max Num of Conflicts")
	tcpipZcMdnsProbeConflictNumMax.setVisible(False)
	tcpipZcMdnsProbeConflictNumMax.setDescription("Maximum Number of Conflicts")
	tcpipZcMdnsProbeConflictNumMax.setDefaultValue(30)
	tcpipZcMdnsProbeConflictNumMax.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

	# Number of Announcement Packets
	tcpipZcMdnsAnnounceNum= tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_ANNOUNCE_NUM", tcpipZcMulticastDns)
	tcpipZcMdnsAnnounceNum.setLabel("Number of Announcement Packets")
	tcpipZcMdnsAnnounceNum.setVisible(False)
	tcpipZcMdnsAnnounceNum.setDescription("Number of Announcement Packets")
	tcpipZcMdnsAnnounceNum.setDefaultValue(3)
	tcpipZcMdnsAnnounceNum.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

	# Time Between Announcement Packets in ms
	tcpipZcMdnsAnnounceInterval= tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_ANNOUNCE_INTERVAL", tcpipZcMulticastDns)
	tcpipZcMdnsAnnounceInterval.setLabel("Time Between Announcement Packets (ms)")
	tcpipZcMdnsAnnounceInterval.setVisible(False)
	tcpipZcMdnsAnnounceInterval.setDescription("Time Between Announcement Packets in ms")
	tcpipZcMdnsAnnounceInterval.setDefaultValue(250)
	tcpipZcMdnsAnnounceInterval.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

	# Delay Before Announcing in ms
	tcpipZcMdnsAnnounceWait= tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_ANNOUNCE_WAIT", tcpipZcMulticastDns)
	tcpipZcMdnsAnnounceWait.setLabel("Delay Before Announcing (ms)")
	tcpipZcMdnsAnnounceWait.setVisible(False)
	tcpipZcMdnsAnnounceWait.setDescription("Delay Before Announcing in ms")
	tcpipZcMdnsAnnounceWait.setDefaultValue(250)
	tcpipZcMdnsAnnounceWait.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

	#Add to system_config.h
	tcpipZcHeaderFtl = tcpipZeroConfComponent.createFileSymbol(None, None)
	tcpipZcHeaderFtl.setSourcePath("library/config/zeroconf.h.ftl")
	tcpipZcHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipZcHeaderFtl.setMarkup(True)
	tcpipZcHeaderFtl.setType("STRING")
	
	# Add zero_conf_helper.c file
	tcpipZcHelperSourceFile = tcpipZeroConfComponent.createFileSymbol(None, None)
	tcpipZcHelperSourceFile.setSourcePath("library/src/zero_conf_helper.c")
	tcpipZcHelperSourceFile.setOutputName("zero_conf_helper.c")
	tcpipZcHelperSourceFile.setOverwrite(True)
	tcpipZcHelperSourceFile.setDestPath("library/tcpip/src/")
	tcpipZcHelperSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipZcHelperSourceFile.setType("SOURCE")
	tcpipZcHelperSourceFile.setEnabled(True)
	tcpipZcHelperSourceFile.setDependencies(tcpipZeroConfGenSourceFile, ["TCPIP_USE_LINK_ZERO_CONFIG"])

	# Add zero_conf_link_local.c file
	tcpipZcLlSourceFile = tcpipZeroConfComponent.createFileSymbol(None, None)
	tcpipZcLlSourceFile.setSourcePath("library/src/zero_conf_link_local.c")
	tcpipZcLlSourceFile.setOutputName("zero_conf_link_local.c")
	tcpipZcLlSourceFile.setOverwrite(True)
	tcpipZcLlSourceFile.setDestPath("library/tcpip/src/")
	tcpipZcLlSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipZcLlSourceFile.setType("SOURCE")
	tcpipZcLlSourceFile.setEnabled(True)
	tcpipZcLlSourceFile.setDependencies(tcpipZeroConfGenSourceFile, ["TCPIP_USE_LINK_ZERO_CONFIG"])

	# Add zero_conf_multicast_dns.c file
	tcpipZcMdnsSourceFile = tcpipZeroConfComponent.createFileSymbol(None, None)
	tcpipZcMdnsSourceFile.setSourcePath("library/src/zero_conf_multicast_dns.c")
	tcpipZcMdnsSourceFile.setOutputName("zero_conf_multicast_dns.c")
	tcpipZcMdnsSourceFile.setOverwrite(True)
	tcpipZcMdnsSourceFile.setDestPath("library/tcpip/src/")
	tcpipZcMdnsSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipZcMdnsSourceFile.setType("SOURCE")
	tcpipZcMdnsSourceFile.setEnabled(True)
	tcpipZcMdnsSourceFile.setDependencies(tcpipZeroConfGenSourceFile, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

# make Multicast DNS Zero Config option visible
def tcpipMulticastDnsZcMenuVisible(tcpipDependentSymbol, tcpipIPSymbol):	
	tcpipUdp = Database.getSymbolValue("tcpipUdp","TCPIP_USE_UDP")
	tcpipZc = Database.getSymbolValue("tcpipZeroConf","TCPIP_USE_LINK_ZERO_CONFIG")

	if(tcpipUdp and tcpipZc):
		tcpipDependentSymbol.setVisible(True)
	else:
		tcpipDependentSymbol.setVisible(False)

		
def tcpipZeroConfMenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("ZeroConf Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("ZeroConf Menu Invisible.")
		symbol.setVisible(False)

def tcpipZeroConfGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])