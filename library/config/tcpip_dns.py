# Global definitions  
TCPIP_ADDRESS_TYPE_STRICT = ["IP_ADDRESS_TYPE_IPV4", "IP_ADDRESS_TYPE_IPV6"]
    
def instantiateComponent(tcpipDnsComponent):
	print("TCPIP DNS Client Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	
	# Enable DNS Client
	tcpipDnsc = tcpipDnsComponent.createBooleanSymbol("TCPIP_USE_DNS_CLIENT", None)
	tcpipDnsc.setLabel("DNS Client")
	tcpipDnsc.setVisible(True)
	tcpipDnsc.setDescription("Enable DNS Client")
	tcpipDnsc.setDefaultValue(True)
	tcpipDnsc.setDependencies(tcpipDnsMenuVisible, ["tcpipUdp.TCPIP_USE_UDP"])

	# DNS Client-Server Time out in Seconds
	tcpipDnscServerTimeout = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_SERVER_TMO", tcpipDnsc)
	tcpipDnscServerTimeout.setLabel("Client-Server Time-out in Seconds")
	tcpipDnscServerTimeout.setVisible(True)
	tcpipDnscServerTimeout.setDescription("Client-Server Time out in Seconds")
	tcpipDnscServerTimeout.setDefaultValue(60)
	tcpipDnscServerTimeout.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

	# DNS Client Task Update Rate in msec
	tcpipDnscTskProcessRate = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_TASK_PROCESS_RATE", tcpipDnsc)
	tcpipDnscTskProcessRate.setLabel("Task Update Rate in msec")
	tcpipDnscTskProcessRate.setVisible(True)
	tcpipDnscTskProcessRate.setDescription("Task Update Rate in msec")
	tcpipDnscTskProcessRate.setDefaultValue(200)
	tcpipDnscTskProcessRate.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

	# Number of DNS Resolver Entries
	tcpipDnscCacheEntryNum = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_CACHE_ENTRIES", tcpipDnsc)
	tcpipDnscCacheEntryNum.setLabel("Number of DNS Resolver Entries")
	tcpipDnscCacheEntryNum.setVisible(True)
	tcpipDnscCacheEntryNum.setDescription("Number of DNS Resolver Entries")
	tcpipDnscCacheEntryNum.setDefaultValue(5)
	tcpipDnscCacheEntryNum.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

	# DNS Client Cache Entry Time outs
	tcpipDnscCacheEntryTimeout = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_CACHE_ENTRY_TMO", tcpipDnsc)
	tcpipDnscCacheEntryTimeout.setLabel("DNS Client Time-outs")
	tcpipDnscCacheEntryTimeout.setVisible(True)
	tcpipDnscCacheEntryTimeout.setDescription("DNS Client Cache Entry Time outs")
	tcpipDnscCacheEntryTimeout.setDefaultValue(0)
	tcpipDnscCacheEntryTimeout.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

	# Maximum Number of IPv4 Answers to be Considered
	tcpipDnscCachePerIpv4Num = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_CACHE_PER_IPV4_ADDRESS", tcpipDnsc)
	tcpipDnscCachePerIpv4Num.setLabel("Maximum Number of IPv4 Answers to be Considered")
	tcpipDnscCachePerIpv4Num.setVisible(True)
	tcpipDnscCachePerIpv4Num.setDescription("Maximum Number of IPv4 Answers to be Considered")
	tcpipDnscCachePerIpv4Num.setDefaultValue(5)
	tcpipDnscCachePerIpv4Num.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

	# Maximum Number of IPv6 Answers to be Considered
	tcpipDnscCachePerIpv6Num = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_CACHE_PER_IPV6_ADDRESS", tcpipDnsc)
	tcpipDnscCachePerIpv6Num.setLabel("Maximum Number of IPv6 Answers to be Considered")
	tcpipDnscCachePerIpv6Num.setVisible(True)
	tcpipDnscCachePerIpv6Num.setDescription("Maximum Number of IPv4 Answers to be Considered")
	tcpipDnscCachePerIpv6Num.setDefaultValue(1)
	tcpipDnscCachePerIpv6Num.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

	# DNS Connection Type 
	tcpipDnscCachePerIpv6Num = tcpipDnsComponent.createComboSymbol("TCPIP_DNS_CLIENT_ADDRESS_TYPE", tcpipDnsc, TCPIP_ADDRESS_TYPE_STRICT)
	tcpipDnscCachePerIpv6Num.setLabel("DNS Connection Type")
	tcpipDnscCachePerIpv6Num.setVisible(True)
	tcpipDnscCachePerIpv6Num.setDescription("DNS Connection Type")
	tcpipDnscCachePerIpv6Num.setDefaultValue("IP_ADDRESS_TYPE_IPV4")
	tcpipDnscCachePerIpv6Num.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

	# Default TTL time in Seconds
	tcpipDnscCacheTtlTime = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_CACHE_DEFAULT_TTL_VAL", tcpipDnsc)
	tcpipDnscCacheTtlTime.setLabel("Default TTL time in Seconds")
	tcpipDnscCacheTtlTime.setVisible(True)
	tcpipDnscCacheTtlTime.setDescription("Default TTL time in Seconds")
	tcpipDnscCacheTtlTime.setDefaultValue(1200)
	tcpipDnscCacheTtlTime.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

	# Time-out for an Unsolved Entry in the Cache
	tcpipDnscCacheUnsolvedEntryTimeout = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_CACHE_UNSOLVED_ENTRY_TMO", tcpipDnsc)
	tcpipDnscCacheUnsolvedEntryTimeout.setLabel("Time-out for an Unsolved Entry in the Cache")
	tcpipDnscCacheUnsolvedEntryTimeout.setVisible(True)
	tcpipDnscCacheUnsolvedEntryTimeout.setDescription("Time-out for an Unsolved Entry in the Cache")
	tcpipDnscCacheUnsolvedEntryTimeout.setDefaultValue(10)
	tcpipDnscCacheUnsolvedEntryTimeout.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

	# Retry Lookup Host name after Time-out
	tcpipDnscLookupRetryTimeout = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_LOOKUP_RETRY_TMO", tcpipDnsc)
	tcpipDnscLookupRetryTimeout.setLabel("Retry Lookup Host name after Time-out")
	tcpipDnscLookupRetryTimeout.setVisible(True)
	tcpipDnscLookupRetryTimeout.setDescription("Retry Lookup Host name after Time-out")
	tcpipDnscLookupRetryTimeout.setDefaultValue(3)
	tcpipDnscLookupRetryTimeout.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

	# DNS Client Host Name Length
	tcpipDnscHostNameLenMax = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN", tcpipDnsc)
	tcpipDnscHostNameLenMax.setLabel("Host Name Length")
	tcpipDnscHostNameLenMax.setVisible(True)
	tcpipDnscHostNameLenMax.setDescription("DNS Client Host Name Length")
	tcpipDnscHostNameLenMax.setDefaultValue(32)
	tcpipDnscHostNameLenMax.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

	# Maximum Number of Interfaces Considered for DNS resolution
	tcpipDnscInterfaceMaxNum = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_MAX_SELECT_INTERFACES", tcpipDnsc)
	tcpipDnscInterfaceMaxNum.setLabel("Maximum Number of Interfaces Considered for DNS resolution")
	tcpipDnscInterfaceMaxNum.setVisible(True)
	tcpipDnscInterfaceMaxNum.setDescription("Maximum Number of Interfaces Considered for DNS resolution")
	tcpipDnscInterfaceMaxNum.setDefaultValue(4)
	tcpipDnscInterfaceMaxNum.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

	# Delete Old Entries
	tcpipDnscDeleteOldEntry= tcpipDnsComponent.createBooleanSymbol("TCPIP_DNS_CLIENT_DELETE_OLD_ENTRIES", tcpipDnsc)
	tcpipDnscDeleteOldEntry.setLabel("Delete Old Entries")
	tcpipDnscDeleteOldEntry.setVisible(True)
	tcpipDnscDeleteOldEntry.setDescription("Delete Old Entries")
	tcpipDnscDeleteOldEntry.setDefaultValue(True)
	tcpipDnscDeleteOldEntry.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

	# Enable User Notification
	tcpipDnscUsrNotification= tcpipDnsComponent.createBooleanSymbol("TCPIP_DNS_CLIENT_USER_NOTIFICATION", tcpipDnsc)
	tcpipDnscUsrNotification.setLabel("Enable User Notification")
	tcpipDnscUsrNotification.setVisible(True)
	tcpipDnscUsrNotification.setDescription("Enable User Notification")
	tcpipDnscUsrNotification.setDefaultValue(False)
	tcpipDnscUsrNotification.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

	#Add to system_config.h
	tcpipDnscHeaderFtl = tcpipDnsComponent.createFileSymbol(None, None)
	tcpipDnscHeaderFtl.setSourcePath("library/config/dns.h.ftl")
	tcpipDnscHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipDnscHeaderFtl.setMarkup(True)
	tcpipDnscHeaderFtl.setType("STRING")
	
	# Add dns.h file to project
	tcpipDnscHeaderFile = tcpipDnsComponent.createFileSymbol(None, None)
	tcpipDnscHeaderFile.setSourcePath("library/dns.h")
	tcpipDnscHeaderFile.setOutputName("dns.h")
	tcpipDnscHeaderFile.setDestPath("library/tcpip/")
	tcpipDnscHeaderFile.setProjectPath("config/" + configName + "library/tcpip/")
	tcpipDnscHeaderFile.setType("HEADER")
	tcpipDnscHeaderFile.setOverwrite(True)

	# Add dns_manager.h file to project
	tcpipDnscManagerHeaderFile = tcpipDnsComponent.createFileSymbol(None, None)
	tcpipDnscManagerHeaderFile.setSourcePath("library/src/dns_manager.h")
	tcpipDnscManagerHeaderFile.setOutputName("dns_manager.h")
	tcpipDnscManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipDnscManagerHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipDnscManagerHeaderFile.setType("HEADER")
	tcpipDnscManagerHeaderFile.setOverwrite(True)

	# Add dns_private.h file to project
	tcpipDnscPrivateHeaderFile = tcpipDnsComponent.createFileSymbol(None, None)
	tcpipDnscPrivateHeaderFile.setSourcePath("library/src/dns_private.h")
	tcpipDnscPrivateHeaderFile.setOutputName("dns_private.h")
	tcpipDnscPrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipDnscPrivateHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipDnscPrivateHeaderFile.setType("HEADER")
	tcpipDnscPrivateHeaderFile.setOverwrite(True)

	# Add dns.c file
	tcpipDnscSourceFile = tcpipDnsComponent.createFileSymbol(None, None)
	tcpipDnscSourceFile.setSourcePath("library/src/dns.c")
	tcpipDnscSourceFile.setOutputName("dns.c")
	tcpipDnscSourceFile.setOverwrite(True)
	tcpipDnscSourceFile.setDestPath("library/tcpip/src/")
	tcpipDnscSourceFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipDnscSourceFile.setType("SOURCE")
	tcpipDnscSourceFile.setEnabled(True)
	
	
	
def tcpipDnsMenuVisible(symbol, event):
	if (event["value"] == True):
		print("DNS Client Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("DNS Client Menu Invisible.")
		symbol.setVisible(False)	
		