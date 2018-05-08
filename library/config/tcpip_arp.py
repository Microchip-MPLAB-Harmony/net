def instantiateComponent(tcpipArpComponent):
	print("TCPIP ARP Component")
	configName = Variables.get("__CONFIGURATION_NAME")
		
	# ARP Enable 
	tcpipArp = tcpipArpComponent.createBooleanSymbol("TCPIP_USE_ARP", None)
	tcpipArp.setLabel("Use ARP?")
	tcpipArp.setDescription("Enables ARP")
	tcpipArp.setDefaultValue(True)
	tcpipArp.setVisible(False)
	#tcpipArp.setDependencies(tcpipArpEnable, ["tcpipIPv4.TCPIP_STACK_USE_IPV4"])
	
	# Cache Entries for this Interface
	tcpipArpCacheEntries = tcpipArpComponent.createIntegerSymbol("TCPIP_ARP_CACHE_ENTRIES", None)
	tcpipArpCacheEntries.setLabel("Cache Entries for This Interface")
	tcpipArpCacheEntries.setVisible(True)
	tcpipArpCacheEntries.setDescription("Cache Entries for this Interface")
	tcpipArpCacheEntries.setDefaultValue(5)
	tcpipArpCacheEntries.setDependencies(tcpipArpMenuVisibleSingle, ["TCPIP_USE_ARP"])


	# Delete Old Cache if Still in Place	
	tcpipArpOldCacheDelete = tcpipArpComponent.createBooleanSymbol("TCPIP_ARP_CACHE_DELETE_OLD", None)
	tcpipArpOldCacheDelete.setLabel("Delete Old Cache if Still in Place")
	tcpipArpOldCacheDelete.setVisible(True)
	tcpipArpOldCacheDelete.setDescription("Delete Old Cache if Still in Place")
	tcpipArpOldCacheDelete.setDefaultValue(True)
	tcpipArpOldCacheDelete.setDependencies(tcpipArpMenuVisibleSingle, ["TCPIP_USE_ARP"])

	# Time-Out for a Solved Entry to be Removed if not Referenced in Seconds
	tcpipArpCacheSolvedEntryTimeout = tcpipArpComponent.createIntegerSymbol("TCPIP_ARP_CACHE_SOLVED_ENTRY_TMO", None)
	tcpipArpCacheSolvedEntryTimeout.setLabel("Time-Out for a Solved Entry to be Removed if not Referenced in Seconds")
	tcpipArpCacheSolvedEntryTimeout.setVisible(True)
	tcpipArpCacheSolvedEntryTimeout.setDescription("Time-Out for a Solved Entry to be Removed if not Referenced in Seconds")
	tcpipArpCacheSolvedEntryTimeout.setDefaultValue(1200)
	tcpipArpCacheSolvedEntryTimeout.setDependencies(tcpipArpMenuVisibleSingle, ["TCPIP_USE_ARP"])

	# Time-Out for a Pending Cache Entry to be Solved in Seconds
	tcpipArpCachePendingEntryTimeout = tcpipArpComponent.createIntegerSymbol("TCPIP_ARP_CACHE_PENDING_ENTRY_TMO", None)
	tcpipArpCachePendingEntryTimeout.setLabel("Time-Out for a Pending Cache Entry to be Solved in Seconds")
	tcpipArpCachePendingEntryTimeout.setVisible(True)
	tcpipArpCachePendingEntryTimeout.setDescription("Time-Out for a Pending Cache Entry to be Solved in Seconds")
	tcpipArpCachePendingEntryTimeout.setDefaultValue(60)
	tcpipArpCachePendingEntryTimeout.setDependencies(tcpipArpMenuVisibleSingle, ["TCPIP_USE_ARP"])

	# Time-Out for Resending an ARP Request for a Pending Entry in Seconds
	tcpipArpCachePendingRetryTimeout = tcpipArpComponent.createIntegerSymbol("TCPIP_ARP_CACHE_PENDING_RETRY_TMO", None)
	tcpipArpCachePendingRetryTimeout.setLabel("Time-Out for Resending an ARP Request for a Pending Entry in Seconds")
	tcpipArpCachePendingRetryTimeout.setVisible(True)
	tcpipArpCachePendingRetryTimeout.setDescription("Time-Out for Resending an ARP Request for a Pending Entry in Seconds")
	tcpipArpCachePendingRetryTimeout.setDefaultValue(2)
	tcpipArpCachePendingRetryTimeout.setDependencies(tcpipArpMenuVisibleSingle, ["TCPIP_USE_ARP"])

	# Maximum Percentage of Permanent Entries Allowed in the Cache
	tcpipArpCachePermQuota = tcpipArpComponent.createIntegerSymbol("TCPIP_ARP_CACHE_PERMANENT_QUOTA", None)
	tcpipArpCachePermQuota.setLabel("Maximum Percentage of Permanent Entries Allowed in the Cache")
	tcpipArpCachePermQuota.setVisible(True)
	tcpipArpCachePermQuota.setDescription("Maximum Percentage of Permanent Entries Allowed in the Cache")
	tcpipArpCachePermQuota.setDefaultValue(50)
	tcpipArpCachePermQuota.setDependencies(tcpipArpMenuVisibleSingle, ["TCPIP_USE_ARP"])

	# Purge Threshold Percentage
	tcpipArpCachePurgeThreshold = tcpipArpComponent.createIntegerSymbol("TCPIP_ARP_CACHE_PURGE_THRESHOLD", None)
	tcpipArpCachePurgeThreshold.setLabel("Purge Threshold Percentage")
	tcpipArpCachePurgeThreshold.setVisible(True)
	tcpipArpCachePurgeThreshold.setDescription("Purge Threshold Percentage")
	tcpipArpCachePurgeThreshold.setDefaultValue(75)
	tcpipArpCachePurgeThreshold.setDependencies(tcpipArpMenuVisibleSingle, ["TCPIP_USE_ARP"])

	# Number of Entries to Delete Once the Threshold is Reached
	tcpipArpCachePurgeQuantaNum = tcpipArpComponent.createIntegerSymbol("TCPIP_ARP_CACHE_PURGE_QUANTA", None)
	tcpipArpCachePurgeQuantaNum.setLabel("Number of Entries to Delete Once the Threshold is Reached")
	tcpipArpCachePurgeQuantaNum.setVisible(True)
	tcpipArpCachePurgeQuantaNum.setDescription("Number of Entries to Delete Once the Threshold is Reached")
	tcpipArpCachePurgeQuantaNum.setDefaultValue(1)
	tcpipArpCachePurgeQuantaNum.setDependencies(tcpipArpMenuVisibleSingle, ["TCPIP_USE_ARP"])

	# Number of Retries for Resolving an Entry
	tcpipArpCacheEntryRetryNum = tcpipArpComponent.createIntegerSymbol("TCPIP_ARP_CACHE_ENTRY_RETRIES", None)
	tcpipArpCacheEntryRetryNum.setLabel("Number of Retries for Resolving an Entry")
	tcpipArpCacheEntryRetryNum.setVisible(True)
	tcpipArpCacheEntryRetryNum.setDescription("Number of Retries for Resolving an Entry")
	tcpipArpCacheEntryRetryNum.setDefaultValue(3)
	tcpipArpCacheEntryRetryNum.setDependencies(tcpipArpMenuVisibleSingle, ["TCPIP_USE_ARP"])

	# Number of Retries Done for a Gratuitous ARP Request
	tcpipArpGratuitousRetryNum = tcpipArpComponent.createIntegerSymbol("TCPIP_ARP_GRATUITOUS_PROBE_COUNT", None)
	tcpipArpGratuitousRetryNum.setLabel("Number of Retries Done for a Gratuitous ARP Request")
	tcpipArpGratuitousRetryNum.setVisible(True)
	tcpipArpGratuitousRetryNum.setDescription("Number of Retries Done for a Gratuitous ARP Request")
	tcpipArpGratuitousRetryNum.setDefaultValue(1)
	tcpipArpGratuitousRetryNum.setDependencies(tcpipArpMenuVisibleSingle, ["TCPIP_USE_ARP"])

	# ARP Task process Rate in msec
	tcpipArpTaskProcessRate = tcpipArpComponent.createIntegerSymbol("TCPIP_ARP_TASK_PROCESS_RATE", None)
	tcpipArpTaskProcessRate.setLabel("How Often to Process ARP in msec")
	tcpipArpTaskProcessRate.setVisible(True)
	tcpipArpTaskProcessRate.setDescription("ARP Task Process Rate in msec")
	tcpipArpTaskProcessRate.setDefaultValue(2)
	tcpipArpTaskProcessRate.setDependencies(tcpipArpMenuVisibleSingle, ["TCPIP_USE_ARP"])

	# Alias Interfaces Share One Cache	
	tcpipArpPrimaryCacheOnly = tcpipArpComponent.createBooleanSymbol("TCPIP_ARP_PRIMARY_CACHE_ONLY", None)
	tcpipArpPrimaryCacheOnly.setLabel("Alias Interfaces Share One Cache")
	tcpipArpPrimaryCacheOnly.setVisible(True)
	tcpipArpPrimaryCacheOnly.setDescription("Alias Interfaces Share One Cache")
	tcpipArpPrimaryCacheOnly.setDefaultValue(True)
	tcpipArpPrimaryCacheOnly.setDependencies(tcpipArpMenuVisibleSingle, ["TCPIP_USE_ARP"])
	
	#Add to system_config.h
	tcpipArpHeaderFtl = tcpipArpComponent.createFileSymbol(None, None)
	tcpipArpHeaderFtl.setSourcePath("library/config/arp.h.ftl")
	tcpipArpHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipArpHeaderFtl.setMarkup(True)
	tcpipArpHeaderFtl.setType("STRING")

	# Add arp.c file
	tcpipArpSourceFile = tcpipArpComponent.createFileSymbol(None, None)
	tcpipArpSourceFile.setSourcePath("library/src/arp.c")
	tcpipArpSourceFile.setOutputName("arp.c")
	tcpipArpSourceFile.setOverwrite(True)
	tcpipArpSourceFile.setDestPath("library/tcpip/src/")
	tcpipArpSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipArpSourceFile.setType("SOURCE")
	tcpipArpSourceFile.setEnabled(True)
	tcpipArpSourceFile.setDependencies(tcpipArpGenSourceFile, ["TCPIP_USE_ARP"])

def tcpipArpMenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("ARP Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("ARP Menu Invisible.")
		symbol.setVisible(False)


def tcpipArpEnable(symbol, event):
	if (event["value"] == True):
		print("ARP Enable")		
		symbol.setValue(True, 1)
	else:
		print("ARP Disable.")
		symbol.setValue(False, 1)		
		
def tcpipArpGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])