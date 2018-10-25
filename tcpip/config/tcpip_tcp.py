    
def instantiateComponent(tcpipTcpComponent):
	print("TCPIP TCP Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	
	# Enable TCP
	tcpipTCP = tcpipTcpComponent.createBooleanSymbol("TCPIP_USE_TCP", None)
	tcpipTCP.setLabel("Use TCP")
	tcpipTCP.setVisible(False)
	tcpipTCP.setDescription("Enable TCP")
	tcpipTCP.setDefaultValue(True)
	#tcpipTCP.setDependencies(tcpipTcpMenuVisible, ["tcpipIPv4.TCPIP_STACK_USE_IPV4", "tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# Maximum TX Segment Size
	tcpipTcpMaxTxSegSize = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_MAX_SEG_SIZE_TX", None)
	tcpipTcpMaxTxSegSize.setLabel("Max TX Segment Size")
	tcpipTcpMaxTxSegSize.setVisible(True)
	tcpipTcpMaxTxSegSize.setDescription("Maximum TX Segment Size")
	tcpipTcpMaxTxSegSize.setDefaultValue(1460)
	#tcpipTcpMaxTxSegSize.setDependencies(tcpipTcpMenuVisibleSingle, ["TCPIP_USE_TCP"])

	# Default TX Socket Size
	tcpipTcpSktTxSize = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_SOCKET_DEFAULT_TX_SIZE", None)
	tcpipTcpSktTxSize.setLabel("Default TX Size")
	tcpipTcpSktTxSize.setVisible(True)
	tcpipTcpSktTxSize.setDescription("Default TX Socket Size")
	tcpipTcpSktTxSize.setDefaultValue(512)
	#tcpipTcpSktTxSize.setDependencies(tcpipTcpMenuVisibleSingle, ["TCPIP_USE_TCP"])

	# Default RX Socket Size
	tcpipTcpSktRxSize = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_SOCKET_DEFAULT_RX_SIZE", None)
	tcpipTcpSktRxSize.setLabel("Default RX Size")
	tcpipTcpSktRxSize.setVisible(True)
	tcpipTcpSktRxSize.setDescription("Default RX Socket Size")
	tcpipTcpSktRxSize.setDefaultValue(512)
	#tcpipTcpSktRxSize.setDependencies(tcpipTcpMenuVisibleSingle, ["TCPIP_USE_TCP"])

	# Enable TCP Sockets Dynamic Options
	tcpipTcpDynOptions = tcpipTcpComponent.createBooleanSymbol("TCPIP_TCP_DYNAMIC_OPTIONS", None)
	tcpipTcpDynOptions.setLabel("Enable TCP Sockets Dynamic Options")
	tcpipTcpDynOptions.setVisible(True)
	tcpipTcpDynOptions.setDescription("Enable TCP Sockets Dynamic Options")
	tcpipTcpDynOptions.setDefaultValue(True)
	#tcpipTcpDynOptions.setDependencies(tcpipTcpMenuVisibleSingle, ["TCPIP_USE_TCP"])

	# Start Timeout in ms
	tcpipTcpStartTimeout = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_START_TIMEOUT_VAL", None)
	tcpipTcpStartTimeout.setLabel("Start Time-out - ms")
	tcpipTcpStartTimeout.setVisible(True)
	tcpipTcpStartTimeout.setDescription("Start Timeout in ms")
	tcpipTcpStartTimeout.setDefaultValue(1000)
	#tcpipTcpStartTimeout.setDependencies(tcpipTcpMenuVisibleSingle, ["TCPIP_USE_TCP"])

	# Delayed Ack Timeout in ms
	tcpipTcpDelayAckTimeout = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_DELAYED_ACK_TIMEOUT", None)
	tcpipTcpDelayAckTimeout.setLabel("Delayed Ack Time-out - ms")
	tcpipTcpDelayAckTimeout.setVisible(True)
	tcpipTcpDelayAckTimeout.setDescription("Delayed Ack Timeout in ms")
	tcpipTcpDelayAckTimeout.setDefaultValue(100)
	#tcpipTcpDelayAckTimeout.setDependencies(tcpipTcpMenuVisibleSingle, ["TCPIP_USE_TCP"])

	# Fin Wait 2 Timeout in ms
	tcpipTcpFinWait2Timeout = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_FIN_WAIT_2_TIMEOUT", None)
	tcpipTcpFinWait2Timeout.setLabel("Fin Wait 2 Time-out - ms")
	tcpipTcpFinWait2Timeout.setVisible(True)
	tcpipTcpFinWait2Timeout.setDescription("Fin Wait 2 Timeout in ms")
	tcpipTcpFinWait2Timeout.setDefaultValue(5000)
	#tcpipTcpFinWait2Timeout.setDependencies(tcpipTcpMenuVisibleSingle, ["TCPIP_USE_TCP"])

	# Keep Alive Timeout in ms
	tcpipTcpKeepAliveTimeout = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_KEEP_ALIVE_TIMEOUT", None)
	tcpipTcpKeepAliveTimeout.setLabel("Keep Alive Time-out - ms")
	tcpipTcpKeepAliveTimeout.setVisible(True)
	tcpipTcpKeepAliveTimeout.setDescription("Keep Alive Timeout in ms")
	tcpipTcpKeepAliveTimeout.setDefaultValue(10000)
	#tcpipTcpKeepAliveTimeout.setDependencies(tcpipTcpMenuVisibleSingle, ["TCPIP_USE_TCP"])

	# Close Wait Timeout in ms
	tcpipTcpCloseWaitTimeout = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_CLOSE_WAIT_TIMEOUT", None)
	tcpipTcpCloseWaitTimeout.setLabel("Close Wait Time-out - ms")
	tcpipTcpCloseWaitTimeout.setVisible(True)
	tcpipTcpCloseWaitTimeout.setDescription("Close Wait Timeout in ms")
	tcpipTcpCloseWaitTimeout.setDefaultValue(200)
	#tcpipTcpCloseWaitTimeout.setDependencies(tcpipTcpMenuVisibleSingle, ["TCPIP_USE_TCP"])

	# Maximum number of retries
	tcpipTcpMaxRetryNum = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_MAX_RETRIES", None)
	tcpipTcpMaxRetryNum.setLabel("Max Retries")
	tcpipTcpMaxRetryNum.setVisible(True)
	tcpipTcpMaxRetryNum.setDescription("Maximum number of retries")
	tcpipTcpMaxRetryNum.setDefaultValue(5)
	#tcpipTcpMaxRetryNum.setDependencies(tcpipTcpMenuVisibleSingle, ["TCPIP_USE_TCP"])

	# Maximum number of Unacked Keep Alives
	tcpipTcpMaxUnackKeepAlivesNum = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_MAX_UNACKED_KEEP_ALIVES", None)
	tcpipTcpMaxUnackKeepAlivesNum.setLabel("Max Unacked Keep Alives")
	tcpipTcpMaxUnackKeepAlivesNum.setVisible(True)
	tcpipTcpMaxUnackKeepAlivesNum.setDescription("Maximum number of Unacked Keep Alives")
	tcpipTcpMaxUnackKeepAlivesNum.setDefaultValue(6)
	#tcpipTcpMaxUnackKeepAlivesNum.setDependencies(tcpipTcpMenuVisibleSingle, ["TCPIP_USE_TCP"])

	# Maximum number of Syn Retries
	tcpipTcpMaxSynRetryNum = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_MAX_SYN_RETRIES", None)
	tcpipTcpMaxSynRetryNum.setLabel("Max SYN Retries")
	tcpipTcpMaxSynRetryNum.setVisible(True)
	tcpipTcpMaxSynRetryNum.setDescription("Maximum number of Syn Retries")
	tcpipTcpMaxSynRetryNum.setDefaultValue(3)
	#tcpipTcpMaxSynRetryNum.setDependencies(tcpipTcpMenuVisibleSingle, ["TCPIP_USE_TCP"])

	# Auto Transmit Timeout in ms
	tcpipTcpAutoTxTimeout = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_AUTO_TRANSMIT_TIMEOUT_VAL", None)
	tcpipTcpAutoTxTimeout.setLabel("Auto Transmit Time-out - ms")
	tcpipTcpAutoTxTimeout.setVisible(True)
	tcpipTcpAutoTxTimeout.setDescription("Auto Transmit Timeout in ms")
	tcpipTcpAutoTxTimeout.setDefaultValue(40)
	#tcpipTcpAutoTxTimeout.setDependencies(tcpipTcpMenuVisibleSingle, ["TCPIP_USE_TCP"])

	# Window Update Time-out
	tcpipTcpWindowUpdateTimeout = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_WINDOW_UPDATE_TIMEOUT_VAL", None)
	tcpipTcpWindowUpdateTimeout.setLabel("Window Update Time-out")
	tcpipTcpWindowUpdateTimeout.setVisible(True)
	tcpipTcpWindowUpdateTimeout.setDescription("Window Update Time-out")
	tcpipTcpWindowUpdateTimeout.setDefaultValue(200)
	#tcpipTcpWindowUpdateTimeout.setDependencies(tcpipTcpMenuVisibleSingle, ["TCPIP_USE_TCP"])

	# Maximum number of sockets
	tcpipTcpMaxSktNum = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_MAX_SOCKETS", None)
	tcpipTcpMaxSktNum.setLabel("Max Sockets")
	tcpipTcpMaxSktNum.setVisible(True)
	tcpipTcpMaxSktNum.setDescription("Maximum number of sockets")
	tcpipTcpMaxSktNum.setDefaultValue(10)
	#tcpipTcpMaxSktNum.setDependencies(tcpipTcpMenuVisibleSingle, ["TCPIP_USE_TCP"])

	# TCP Task Tick Rate
	tcpipTcpTaskTickRate = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_TASK_TICK_RATE", None)
	tcpipTcpTaskTickRate.setLabel("TCP Task Rate")
	tcpipTcpTaskTickRate.setVisible(True)
	tcpipTcpTaskTickRate.setDescription("TCP Task Tick Rate")
	tcpipTcpTaskTickRate.setDefaultValue(5)
	#tcpipTcpTaskTickRate.setDependencies(tcpipTcpMenuVisibleSingle, ["TCPIP_USE_TCP"])

	# Maximum Segment Length (MSL) Timeout in seconds
	tcpipTcpMaxSegLenTimeout = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_MSL_TIMEOUT", None)
	tcpipTcpMaxSegLenTimeout.setLabel("Maximum Segment Length (MSL) Timeout - seconds")
	tcpipTcpMaxSegLenTimeout.setVisible(True)
	tcpipTcpMaxSegLenTimeout.setDescription("Maximum Segment Length (MSL) Timeout in seconds")
	tcpipTcpMaxSegLenTimeout.setDefaultValue(0)
	#tcpipTcpMaxSegLenTimeout.setDependencies(tcpipTcpMenuVisibleSingle, ["TCPIP_USE_TCP"])

	# TCP quiet time in seconds
	tcpipTcpQuietTime = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_QUIET_TIME", None)
	tcpipTcpQuietTime.setLabel("TCP quiet time - seconds")
	tcpipTcpQuietTime.setVisible(True)
	tcpipTcpQuietTime.setDescription("TCP quiet time in seconds")
	tcpipTcpQuietTime.setDefaultValue(0)
	#tcpipTcpQuietTime.setDependencies(tcpipTcpMenuVisibleSingle, ["TCPIP_USE_TCP"])

	# Enable TCP Commands
	tcpipTcpCommands = tcpipTcpComponent.createBooleanSymbol("TCPIP_TCP_COMMANDS", None)
	tcpipTcpCommands.setLabel("Enable TCP Commands")
	tcpipTcpCommands.setVisible(True)
	tcpipTcpCommands.setDescription("Enable TCP Commands")
	tcpipTcpCommands.setDefaultValue(False)
	#tcpipTcpCommands.setDependencies(tcpipTcpMenuVisibleSingle, ["TCPIP_USE_TCP"])

	#Add to system_config.h
	tcpipTcpHeaderFtl = tcpipTcpComponent.createFileSymbol(None, None)
	tcpipTcpHeaderFtl.setSourcePath("tcpip/config/tcp.h.ftl")
	tcpipTcpHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipTcpHeaderFtl.setMarkup(True)
	tcpipTcpHeaderFtl.setType("STRING")

	# Add tcp.c file
	tcpipTcpSourceFile = tcpipTcpComponent.createFileSymbol(None, None)
	tcpipTcpSourceFile.setSourcePath("tcpip/src/tcp.c")
	tcpipTcpSourceFile.setOutputName("tcp.c")
	tcpipTcpSourceFile.setOverwrite(True)
	tcpipTcpSourceFile.setDestPath("library/tcpip/src/")
	tcpipTcpSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipTcpSourceFile.setType("SOURCE")
	tcpipTcpSourceFile.setEnabled(True)
	#tcpipTcpSourceFile.setDependencies(tcpipTcpGenSourceFile, ["TCPIP_USE_TCP"])

def tcpipTcpMenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("TCP Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("TCP Menu Invisible.")
		symbol.setVisible(False)	
		
# make TCP options visible
def tcpipTcpMenuVisible(symbol, tcpipIPSymbol):
	tcpipIPv4 = Database.getSymbolValue("tcpipIPv4","TCPIP_STACK_USE_IPV4")
	tcpipIPv6 = Database.getSymbolValue("tcpipIPv6","TCPIP_STACK_USE_IPV6")
	print("TCP Menu")
	if(tcpipIPv4 or tcpipIPv6):
		symbol.setVisible(True)
		symbol.setValue(True,1)
	else:
		symbol.setVisible(False)
		symbol.setValue(False,1)

def tcpipTcpGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])
	

def onGenericDependencySatisfied(dependencyID, satisfierID):
	print("satisfied: " + dependencyID + ", " + satisfierID)
	if (satisfierID == "tcpipIPv4") or (satisfierID == "tcpipIPv6"):
		Database.clearSymbolValue("tcpipTcp", "TCPIP_USE_TCP")
		Database.setSymbolValue("tcpipTcp", "TCPIP_USE_TCP", True, 2)

def onGenericDependencyUnsatisfied(dependencyID, satisfierID):
	print("unsatisfied: " + dependencyID + ", " + satisfierID)
	if (satisfierID == "tcpipIPv4") or (satisfierID == "tcpipIPv6"):
		Database.clearSymbolValue("tcpipTcp", "TCPIP_USE_TCP")
		Database.setSymbolValue("tcpipTcp", "TCPIP_USE_TCP", False, 2)

def destroyComponent(component):
	Database.setSymbolValue("tcpipTcp", "TCPIP_USE_TCP", False, 2)