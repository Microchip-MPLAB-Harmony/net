# Global definitions  
TCPIP_SNTP_PROTOCOL_TYPE = ["SNTP_VERSION_3", "SNTP_VERSION_4"]
#H3_ToDo multiple definition for following; find a method
TCPIP_ADDRESS_TYPE_STRICT = ["IP_ADDRESS_TYPE_IPV4", "IP_ADDRESS_TYPE_IPV6"]
TCPIP_STACK_IF_NAME = []
#H3_ToDo modify the interface name
TCPIP_STACK_PIC32C_IF_NAME =	["PIC32CINT", 	"ENCX24J600", 	"ENC28J60", 	"MRF24WN", 		"WINC1500", 	"WILC1000" ]
TCPIP_STACK_PIC32M_IF_NAME =	["PIC32INT", 	"ENCX24J600", 	"ENC28J60", 	"MRF24WN", 		"WINC1500", 	"WILC1000" ]

def instantiateComponent(tcpipSntpComponent):
	#global TCPIP_STACK_IF_NAME
	#global TCPIP_ADDRESS_TYPE_STRICT
	print("TCPIP SNTP Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	if "SAME70" in Variables.get("__PROCESSOR"):
		TCPIP_STACK_IF_NAME = TCPIP_STACK_PIC32C_IF_NAME
	else:
		TCPIP_STACK_IF_NAME = TCPIP_STACK_PIC32M_IF_NAME
	# Use SNTP Client Needed
	tcpipSntpClientNeeded = tcpipSntpComponent.createBooleanSymbol("TCPIP_USE_SNTP_CLIENT_NEEDED", None)
	tcpipSntpClientNeeded.setVisible(False)
	tcpipSntpClientNeeded.setDescription("Use SNTP Client")
	tcpipSntpClientNeeded.setDefaultValue(False) 

	# Use SNTP Client
	tcpipSntpClient = tcpipSntpComponent.createBooleanSymbol("TCPIP_USE_SNTP_CLIENT", None)
	tcpipSntpClient.setLabel("Use SNTP Client")
	tcpipSntpClient.setVisible(False)
	tcpipSntpClient.setDescription("Use SNTP Client")
	tcpipSntpClient.setDefaultValue(True) 
	# H3_ToDo  
	# default n if !TCPIP_USE_SNTP_CLIENT_NEEDED
	# default y if TCPIP_USE_SNTP_CLIENT_NEEDED
	#tcpipSntpClient.setDependencies(tcpipSntpMenuVisibleSingle, ["tcpipUdp.TCPIP_USE_UDP"])

	# ifblock !PIC32WK    H3_ToDo
	# Default Interface
	tcpipSntpDefault = tcpipSntpComponent.createComboSymbol("TCPIP_NTP_DEFAULT_IF", None, TCPIP_STACK_IF_NAME)
	tcpipSntpDefault.setLabel("Default Interface")
	tcpipSntpDefault.setVisible(True)
	tcpipSntpDefault.setDescription("Default Interface")
	tcpipSntpDefault.setDefaultValue("PIC32CINT")
	# H3_ToDo
	# default "PIC32CINT" if USE_PIC32CINT_ETH_MAC_NEEDED
	# default "PIC32INT" if USE_PIC32INT_ETH_MAC_NEEDED
	# default "MRF24WN" if HAVE_WIFI
	#tcpipSntpDefault.setDependencies(tcpipSntpMenuVisibleSingle, ["TCPIP_USE_SNTP_CLIENT"])

	# H3_ToDo
	# ifblock PIC32WK 
	# config TCPIP_NTP_DEFAULT_IF_WK
		# string "Default Interface"
		# depends on TCPIP_USE_SNTP_CLIENT
		# default "PIC32WK" 
		# ---help---
		# IDH_HTML_TCPIP_SNTP_Introduction
		# ---endhelp---
	# endif 

	# SNTP Version
	tcpipSntpVersion = tcpipSntpComponent.createComboSymbol("TCPIP_NTP_STRING_VERSION", None, TCPIP_SNTP_PROTOCOL_TYPE)
	tcpipSntpVersion.setLabel("SNTP Version")
	tcpipSntpVersion.setVisible(True)
	tcpipSntpVersion.setDescription("SNTP Version")
	tcpipSntpVersion.setDefaultValue("SNTP_VERSION_4")
	#tcpipSntpVersion.setDependencies(tcpipSntpMenuVisibleSingle, ["TCPIP_USE_SNTP_CLIENT"])

	# Default Connection Type
	tcpipSntpConnType = tcpipSntpComponent.createComboSymbol("TCPIP_NTP_DEFAULT_CONNECTION_TYPE", None, TCPIP_ADDRESS_TYPE_STRICT)
	tcpipSntpConnType.setLabel("Default Connection Type")
	tcpipSntpConnType.setVisible(True)
	tcpipSntpConnType.setDescription("Default Connection Type")
	tcpipSntpConnType.setDefaultValue("TCPIP_STACK_USE_IPV4")
	# H3_ToDo
	# default "IP_ADDRESS_TYPE_IPV4" if TCPIP_STACK_USE_IPV4
	# default "IP_ADDRESS_TYPE_IPV6" if TCPIP_STACK_USE_IPV6
	#tcpipSntpConnType.setDependencies(tcpipSntpMenuVisibleSingle, ["TCPIP_USE_SNTP_CLIENT"])

	# Sntp Epoch
	tcpipSntpEpoch = tcpipSntpComponent.createIntegerSymbol("TCPIP_NTP_EPOCH", None)
	tcpipSntpEpoch.setLabel("Epoch")
	tcpipSntpEpoch.setVisible(True)
	tcpipSntpEpoch.setDescription("Sntp Epoch")
	tcpipSntpEpoch.setDefaultValue(2147483216)   # H3_ToDo to set the correct value 2208988800
	#tcpipSntpEpoch.setDependencies(tcpipSntpMenuVisibleSingle, ["TCPIP_USE_SNTP_CLIENT"])

	# SNTP Reply Time-out in Seconds
	tcpipSntpReplyTimeout = tcpipSntpComponent.createIntegerSymbol("TCPIP_NTP_REPLY_TIMEOUT", None)
	tcpipSntpReplyTimeout.setLabel("NTP Reply Time-out in Seconds")
	tcpipSntpReplyTimeout.setVisible(True)
	tcpipSntpReplyTimeout.setDescription("SNTP Reply Time-out in Seconds")
	tcpipSntpReplyTimeout.setDefaultValue(6)
	#tcpipSntpReplyTimeout.setDependencies(tcpipSntpMenuVisibleSingle, ["TCPIP_USE_SNTP_CLIENT"])

	# Maximum Stratum
	tcpipSntpStratumMax = tcpipSntpComponent.createIntegerSymbol("TCPIP_NTP_MAX_STRATUM", None)
	tcpipSntpStratumMax.setLabel("Max Stratum")
	tcpipSntpStratumMax.setVisible(True)
	tcpipSntpStratumMax.setDescription("Maximum Stratum")
	tcpipSntpStratumMax.setDefaultValue(15)
	#tcpipSntpStratumMax.setDependencies(tcpipSntpMenuVisibleSingle, ["TCPIP_USE_SNTP_CLIENT"])

	# Time Stamp Time-Out
	tcpipSntpTimestampTimeout = tcpipSntpComponent.createIntegerSymbol("TCPIP_NTP_TIME_STAMP_TMO", None)
	tcpipSntpTimestampTimeout.setLabel("Time Stamp Time-Out")
	tcpipSntpTimestampTimeout.setVisible(True)
	tcpipSntpTimestampTimeout.setDescription("Time Stamp Time-Out")
	tcpipSntpTimestampTimeout.setDefaultValue(660)
	#tcpipSntpTimestampTimeout.setDependencies(tcpipSntpMenuVisibleSingle, ["TCPIP_USE_SNTP_CLIENT"])

	# SNTP Server Name
	tcpipSntpSrvrName = tcpipSntpComponent.createStringSymbol("TCPIP_NTP_SERVER", None)
	tcpipSntpSrvrName.setLabel("NTP Server Name")
	tcpipSntpSrvrName.setVisible(True)
	tcpipSntpSrvrName.setDescription("SNTP Server Name")
	tcpipSntpSrvrName.setDefaultValue("pool.ntp.org")
	#tcpipSntpSrvrName.setDependencies(tcpipSntpMenuVisibleSingle, ["TCPIP_USE_SNTP_CLIENT"])

	# Server Address Max Length
	tcpipSntpSrvrMaxLength = tcpipSntpComponent.createStringSymbol("TCPIP_NTP_SERVER_MAX_LENGTH", None)
	tcpipSntpSrvrMaxLength.setLabel("Server Address Max Length")
	tcpipSntpSrvrMaxLength.setVisible(True)
	tcpipSntpSrvrMaxLength.setDescription("Server Address Max Length")
	tcpipSntpSrvrMaxLength.setDefaultValue("30")
	#tcpipSntpSrvrMaxLength.setDependencies(tcpipSntpMenuVisibleSingle, ["TCPIP_USE_SNTP_CLIENT"])

	# Query Interval in seconds
	tcpipSntpQueryInterval = tcpipSntpComponent.createStringSymbol("TCPIP_NTP_QUERY_INTERVAL", None)
	tcpipSntpQueryInterval.setLabel("Query Interval - Seconds")
	tcpipSntpQueryInterval.setVisible(True)
	tcpipSntpQueryInterval.setDescription("Query Interval in seconds")
	tcpipSntpQueryInterval.setDefaultValue("600")
	#tcpipSntpQueryInterval.setDependencies(tcpipSntpMenuVisibleSingle, ["TCPIP_USE_SNTP_CLIENT"])

	# Fast Query Interval
	tcpipSntpFastQueryInterval = tcpipSntpComponent.createStringSymbol("TCPIP_NTP_FAST_QUERY_INTERVAL", None)
	tcpipSntpFastQueryInterval.setLabel("Fast Query Interval")
	tcpipSntpFastQueryInterval.setVisible(True)
	tcpipSntpFastQueryInterval.setDescription("Fast Query Interval")
	tcpipSntpFastQueryInterval.setDefaultValue("14")
	#tcpipSntpFastQueryInterval.setDependencies(tcpipSntpMenuVisibleSingle, ["TCPIP_USE_SNTP_CLIENT"])

	# Task Tick Rate in ms
	tcpipSntpTskTickRate = tcpipSntpComponent.createStringSymbol("TCPIP_NTP_TASK_TICK_RATE", None)
	tcpipSntpTskTickRate.setLabel("Task Tick Rate - ms")
	tcpipSntpTskTickRate.setVisible(True)
	tcpipSntpTskTickRate.setDescription("Task Tick Rate in ms")
	tcpipSntpTskTickRate.setDefaultValue("1100")
	#tcpipSntpTskTickRate.setDependencies(tcpipSntpMenuVisibleSingle, ["TCPIP_USE_SNTP_CLIENT"])


	# RX Queue Limit
	tcpipSntpRxQueueLimit = tcpipSntpComponent.createStringSymbol("TCPIP_NTP_RX_QUEUE_LIMIT", None)
	tcpipSntpRxQueueLimit.setLabel("RX Queue Limit")
	tcpipSntpRxQueueLimit.setVisible(True)
	tcpipSntpRxQueueLimit.setDescription("RX Queue Limit")
	tcpipSntpRxQueueLimit.setDefaultValue("2")
	#tcpipSntpRxQueueLimit.setDependencies(tcpipSntpMenuVisibleSingle, ["TCPIP_USE_SNTP_CLIENT"])

	#Add to system_config.h
	tcpipSntpHeaderFtl = tcpipSntpComponent.createFileSymbol(None, None)
	tcpipSntpHeaderFtl.setSourcePath("library/config/sntp.h.ftl")
	tcpipSntpHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipSntpHeaderFtl.setMarkup(True)
	tcpipSntpHeaderFtl.setType("STRING")

	# Add sntp.c file
	tcpipSntpSourceFile = tcpipSntpComponent.createFileSymbol(None, None)
	tcpipSntpSourceFile.setSourcePath("library/src/sntp.c")
	tcpipSntpSourceFile.setOutputName("sntp.c")
	tcpipSntpSourceFile.setOverwrite(True)
	tcpipSntpSourceFile.setDestPath("library/tcpip/src/")
	tcpipSntpSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipSntpSourceFile.setType("SOURCE")
	tcpipSntpSourceFile.setEnabled(True)
	tcpipSntpSourceFile.setDependencies(tcpipSntpGenSourceFile, ["TCPIP_USE_SNTP_CLIENT"])


def tcpipSntpMenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("SNTP Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("SNTP Menu Invisible.")
		symbol.setVisible(False)

def tcpipSntpGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])