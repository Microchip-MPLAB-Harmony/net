    
def instantiateComponent(tcpipDnssComponent):
	print("TCPIP DNS Client Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	
	# Enable DNS Server
	tcpipDnss = tcpipDnssComponent.createBooleanSymbol("TCPIP_USE_DNSS", None)
	tcpipDnss.setLabel("DNS Server")
	tcpipDnss.setVisible(False)
	tcpipDnss.setDescription("Enable DNS Server")
	tcpipDnss.setDefaultValue(True)
	#tcpipDnss.setDependencies(tcpipDnssMenuVisible, ["tcpipUdp.TCPIP_USE_UDP"])

	# Default DNS Host Name Length
	tcpipDnssHostNameLen = tcpipDnssComponent.createIntegerSymbol("TCPIP_DNSS_HOST_NAME_LEN", None)
	tcpipDnssHostNameLen.setLabel("Default DNS Host Name Length")
	tcpipDnssHostNameLen.setVisible(True)
	tcpipDnssHostNameLen.setDescription("Default DNS Host Name Length")
	tcpipDnssHostNameLen.setDefaultValue(64)
	#tcpipDnssHostNameLen.setDependencies(tcpipDnssMenuVisible, ["TCPIP_USE_DNSS"])

	# Reply DNS Info with Board Info
	tcpipDnssReplyBoardAddr = tcpipDnssComponent.createBooleanSymbol("TCPIP_DNSS_REPLY_BOARD_ADDR", None)
	tcpipDnssReplyBoardAddr.setLabel("Reply DNS Info with Board Info")
	tcpipDnssReplyBoardAddr.setVisible(True)
	tcpipDnssReplyBoardAddr.setDescription("Reply DNS Info with Board Info")
	tcpipDnssReplyBoardAddr.setDefaultValue(True)
	#tcpipDnssReplyBoardAddr.setDependencies(tcpipDnssMenuVisible, ["TCPIP_USE_DNSS"])

	# Maximum Number of IPv4 Entries
	tcpipDnssIpv4EntryMaxNum = tcpipDnssComponent.createIntegerSymbol("TCPIP_DNSS_CACHE_PER_IPV4_ADDRESS", None)
	tcpipDnssIpv4EntryMaxNum.setLabel("Maximum Number of IPv4 Entries")
	tcpipDnssIpv4EntryMaxNum.setVisible(True)
	tcpipDnssIpv4EntryMaxNum.setDescription("Maximum Number of IPv4 Entries")
	tcpipDnssIpv4EntryMaxNum.setDefaultValue(2)
	#tcpipDnssIpv4EntryMaxNum.setDependencies(tcpipDnssMenuVisible, ["TCPIP_USE_DNSS"])

	# Maximum Number of IPv6 Entries
	tcpipDnssIpv6EntryMaxNum = tcpipDnssComponent.createIntegerSymbol("TCPIP_DNSS_CACHE_PER_IPV6_ADDRESS", None)
	tcpipDnssIpv6EntryMaxNum.setLabel("Maximum Number of IPv6 Entries")
	tcpipDnssIpv6EntryMaxNum.setVisible(False)
	tcpipDnssIpv6EntryMaxNum.setDescription("Maximum Number of IPv6 Entries")
	tcpipDnssIpv6EntryMaxNum.setDefaultValue(1)
	tcpipDnssIpv6EntryMaxNum.setDependencies(tcpipDnssIpv6EntryVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# Default TTL Time for an IP Address in Seconds
	tcpipDnssTtlTime = tcpipDnssComponent.createIntegerSymbol("TCPIP_DNSS_TTL_TIME", None)
	tcpipDnssTtlTime.setLabel("Default TTL Time for an IP Address in Seconds")
	tcpipDnssTtlTime.setVisible(True)
	tcpipDnssTtlTime.setDescription("Default TTL Time for an IP Address in Seconds")
	tcpipDnssTtlTime.setDefaultValue(600)
	#tcpipDnssTtlTime.setDependencies(tcpipDnssMenuVisible, ["TCPIP_USE_DNSS"])

	# DNS Server Time-Out Task Processing Rate in Milliseconds
	tcpipDnssTskProcessRate = tcpipDnssComponent.createIntegerSymbol("TCPIP_DNSS_TASK_PROCESS_RATE", None)
	tcpipDnssTskProcessRate.setLabel("DNS Server Time-Out Task Processing Rate in Milliseconds")
	tcpipDnssTskProcessRate.setVisible(True)
	tcpipDnssTskProcessRate.setDescription("DNS Server Time-Out Task Processing Rate in Milliseconds")
	tcpipDnssTskProcessRate.setDefaultValue(33)
	#tcpipDnssTskProcessRate.setDependencies(tcpipDnssMenuVisible, ["TCPIP_USE_DNSS"])


	# Delete Old Cache if Still in Place
	tcpipDnssDeleteOldLease = tcpipDnssComponent.createBooleanSymbol("TCPIP_DNSS_DELETE_OLD_LEASE", None)
	tcpipDnssDeleteOldLease.setLabel("Delete Old Cache if Still in Place")
	tcpipDnssDeleteOldLease.setVisible(True)
	tcpipDnssDeleteOldLease.setDescription("Delete Old Cache if Still in Place")
	tcpipDnssDeleteOldLease.setDefaultValue(True)
	#tcpipDnssDeleteOldLease.setDependencies(tcpipDnssMenuVisible, ["TCPIP_USE_DNSS"])

	#Add to system_config.h
	tcpipDnssHeaderFtl = tcpipDnssComponent.createFileSymbol(None, None)
	tcpipDnssHeaderFtl.setSourcePath("tcpip/config/dnss.h.ftl")
	tcpipDnssHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipDnssHeaderFtl.setMarkup(True)
	tcpipDnssHeaderFtl.setType("STRING")

	# Add dnss.c file
	tcpipDnssSourceFile = tcpipDnssComponent.createFileSymbol(None, None)
	tcpipDnssSourceFile.setSourcePath("tcpip/src/dnss.c")
	tcpipDnssSourceFile.setOutputName("dnss.c")
	tcpipDnssSourceFile.setOverwrite(True)
	tcpipDnssSourceFile.setDestPath("library/tcpip/src/")
	tcpipDnssSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipDnssSourceFile.setType("SOURCE")
	tcpipDnssSourceFile.setEnabled(True)
	#tcpipDnssSourceFile.setDependencies(tcpipDnssGenSourceFile, ["TCPIP_USE_DNSS"])
	
	
	
def tcpipDnssMenuVisible(symbol, event):
	if (event["value"] == True):
		print("DNS Client Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("DNS Client Menu Invisible.")
		symbol.setVisible(False)	

# make Maximum number of IPv6 entries visible under DNS Server
def tcpipDnssIpv6EntryVisible(symbol, event):
	if (event["value"] == True):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)
		
def tcpipDnssGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])

def destroyComponent(component):
	Database.setSymbolValue("tcpipDnss", "TCPIP_USE_DNSS", False, 2)	