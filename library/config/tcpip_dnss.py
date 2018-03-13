    
def instantiateComponent(tcpipDnssComponent):
	print("TCPIP DNS Client Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	
	# Enable DNS Server
	tcpipDnss = tcpipDnssComponent.createBooleanSymbol("TCPIP_USE_DNSS", None)
	tcpipDnss.setLabel("DNS Server")
	tcpipDnss.setVisible(True)
	tcpipDnss.setDescription("Enable DNS Server")
	tcpipDnss.setDefaultValue(False)
	tcpipDnss.setDependencies(tcpipDnssMenuVisible, ["tcpipUdp.TCPIP_USE_UDP"])

	# Default DNS Host Name Length
	tcpipDnssHostNameLen = tcpipDnssComponent.createIntegerSymbol("TCPIP_DNSS_HOST_NAME_LEN", tcpipDnss)
	tcpipDnssHostNameLen.setLabel("Default DNS Host Name Length")
	tcpipDnssHostNameLen.setVisible(False)
	tcpipDnssHostNameLen.setDescription("Default DNS Host Name Length")
	tcpipDnssHostNameLen.setDefaultValue(64)
	tcpipDnssHostNameLen.setDependencies(tcpipDnssMenuVisible, ["TCPIP_USE_DNSS"])

	# Reply DNS Info with Board Info
	tcpipDnssReplyBoardAddr = tcpipDnssComponent.createBooleanSymbol("TCPIP_DNSS_REPLY_BOARD_ADDR", tcpipDnss)
	tcpipDnssReplyBoardAddr.setLabel("Reply DNS Info with Board Info")
	tcpipDnssReplyBoardAddr.setVisible(False)
	tcpipDnssReplyBoardAddr.setDescription("Reply DNS Info with Board Info")
	tcpipDnssReplyBoardAddr.setDefaultValue(True)
	tcpipDnssReplyBoardAddr.setDependencies(tcpipDnssMenuVisible, ["TCPIP_USE_DNSS"])

	# Maximum Number of IPv4 Entries
	tcpipDnssIpv4EntryMaxNum = tcpipDnssComponent.createIntegerSymbol("TCPIP_DNSS_CACHE_PER_IPV4_ADDRESS", tcpipDnss)
	tcpipDnssIpv4EntryMaxNum.setLabel("Maximum Number of IPv4 Entries")
	tcpipDnssIpv4EntryMaxNum.setVisible(False)
	tcpipDnssIpv4EntryMaxNum.setDescription("Maximum Number of IPv4 Entries")
	tcpipDnssIpv4EntryMaxNum.setDefaultValue(2)
	tcpipDnssIpv4EntryMaxNum.setDependencies(tcpipDnssMenuVisible, ["TCPIP_USE_DNSS"])

	# Maximum Number of IPv6 Entries
	tcpipDnssIpv6EntryMaxNum = tcpipDnssComponent.createIntegerSymbol("TCPIP_DNSS_CACHE_PER_IPV6_ADDRESS", tcpipDnss)
	tcpipDnssIpv6EntryMaxNum.setLabel("Maximum Number of IPv6 Entries")
	tcpipDnssIpv6EntryMaxNum.setVisible(False)
	tcpipDnssIpv6EntryMaxNum.setDescription("Maximum Number of IPv6 Entries")
	tcpipDnssIpv6EntryMaxNum.setDefaultValue(1)
	tcpipDnssIpv6EntryMaxNum.setDependencies(tcpipDnssIpv6EntryVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6", "TCPIP_USE_DNSS"])

	# Default TTL Time for an IP Address in Seconds
	tcpipDnssTtlTime = tcpipDnssComponent.createIntegerSymbol("TCPIP_DNSS_TTL_TIME", tcpipDnss)
	tcpipDnssTtlTime.setLabel("Default TTL Time for an IP Address in Seconds")
	tcpipDnssTtlTime.setVisible(False)
	tcpipDnssTtlTime.setDescription("Default TTL Time for an IP Address in Seconds")
	tcpipDnssTtlTime.setDefaultValue(600)
	tcpipDnssTtlTime.setDependencies(tcpipDnssMenuVisible, ["TCPIP_USE_DNSS"])

	# DNS Server Time-Out Task Processing Rate in Milliseconds
	tcpipDnssTskProcessRate = tcpipDnssComponent.createIntegerSymbol("TCPIP_DNSS_TASK_PROCESS_RATE", tcpipDnss)
	tcpipDnssTskProcessRate.setLabel("DNS Server Time-Out Task Processing Rate in Milliseconds")
	tcpipDnssTskProcessRate.setVisible(False)
	tcpipDnssTskProcessRate.setDescription("DNS Server Time-Out Task Processing Rate in Milliseconds")
	tcpipDnssTskProcessRate.setDefaultValue(33)
	tcpipDnssTskProcessRate.setDependencies(tcpipDnssMenuVisible, ["TCPIP_USE_DNSS"])


	# Delete Old Cache if Still in Place
	tcpipDnssDeleteOldLease = tcpipDnssComponent.createBooleanSymbol("TCPIP_DNSS_DELETE_OLD_LEASE", tcpipDnss)
	tcpipDnssDeleteOldLease.setLabel("Delete Old Cache if Still in Place")
	tcpipDnssDeleteOldLease.setVisible(False)
	tcpipDnssDeleteOldLease.setDescription("Delete Old Cache if Still in Place")
	tcpipDnssDeleteOldLease.setDefaultValue(True)
	tcpipDnssDeleteOldLease.setDependencies(tcpipDnssMenuVisible, ["TCPIP_USE_DNSS"])

	# Add dnss.h file to project
	tcpipDnssHeaderFile = tcpipDnssComponent.createFileSymbol(None, None)
	tcpipDnssHeaderFile.setSourcePath("library/dnss.h")
	tcpipDnssHeaderFile.setOutputName("dnss.h")
	tcpipDnssHeaderFile.setDestPath("library/tcpip/")
	tcpipDnssHeaderFile.setProjectPath("config/" + configName + "library/tcpip/")
	tcpipDnssHeaderFile.setType("HEADER")
	tcpipDnssHeaderFile.setOverwrite(True)

	# Add dnss_manager.h file to project
	tcpipDnssManagerHeaderFile = tcpipDnssComponent.createFileSymbol(None, None)
	tcpipDnssManagerHeaderFile.setSourcePath("library/src/dnss_manager.h")
	tcpipDnssManagerHeaderFile.setOutputName("dnss_manager.h")
	tcpipDnssManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipDnssManagerHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipDnssManagerHeaderFile.setType("HEADER")
	tcpipDnssManagerHeaderFile.setOverwrite(True)

	# Add dnss_private.h file to project
	tcpipDnssPrivateHeaderFile = tcpipDnssComponent.createFileSymbol(None, None)
	tcpipDnssPrivateHeaderFile.setSourcePath("library/src/dnss_private.h")
	tcpipDnssPrivateHeaderFile.setOutputName("dnss_private.h")
	tcpipDnssPrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipDnssPrivateHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipDnssPrivateHeaderFile.setType("HEADER")
	tcpipDnssPrivateHeaderFile.setOverwrite(True)

	# Add dnss.c file
	tcpipDnssSourceFile = tcpipDnssComponent.createFileSymbol(None, None)
	tcpipDnssSourceFile.setSourcePath("library/src/dnss.c")
	tcpipDnssSourceFile.setOutputName("dnss.c")
	tcpipDnssSourceFile.setOverwrite(True)
	tcpipDnssSourceFile.setDestPath("library/tcpip/src/")
	tcpipDnssSourceFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipDnssSourceFile.setType("SOURCE")
	tcpipDnssSourceFile.setEnabled(True)
	tcpipDnssSourceFile.setDependencies(tcpipDnssGenSourceFile, ["TCPIP_USE_DNSS"])
	
	
	
def tcpipDnssMenuVisible(symbol, event):
	if (event["value"] == True):
		print("DNS Client Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("DNS Client Menu Invisible.")
		symbol.setVisible(False)	

# make Maximum number of IPv6 entries visible under DNS Server
def tcpipDnssIpv6EntryVisible(symbol, event):
	tcpipIPv6 = Database.getSymbolValue("tcpipIPv6","TCPIP_STACK_USE_IPV6")
	tcpipDnss = Database.getSymbolValue("tcpipDnss","TCPIP_USE_DNSS")

	if(tcpipIPv6 and tcpipDnss):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)
		
def tcpipDnssGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])