    
def instantiateComponent(tcpipUdpComponent):
	print("TCPIP UDP Component")
	configName = Variables.get("__CONFIGURATION_NAME")

	# Enable UDP
	tcpipUDP = tcpipUdpComponent.createBooleanSymbol("TCPIP_USE_UDP", None)
	tcpipUDP.setLabel("Use UDP")
	tcpipUDP.setVisible(False)
	tcpipUDP.setDescription("Enable UDP")
	tcpipUDP.setDefaultValue(True)
	#tcpipUDP.setDependencies(tcpipUdpMenuVisible, ["tcpipIPv4.TCPIP_STACK_USE_IPV4", "tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# Maximum number of UDP Sockets
	tcpipUdpMaxNumSocket = tcpipUdpComponent.createIntegerSymbol("TCPIP_UDP_MAX_SOCKETS", None)
	tcpipUdpMaxNumSocket.setLabel("Maximum number of Sockets")
	tcpipUdpMaxNumSocket.setVisible(True)
	tcpipUdpMaxNumSocket.setDescription("Maximum number of UDP Sockets")
	tcpipUdpMaxNumSocket.setDefaultValue(10)
	#tcpipUdpMaxNumSocket.setDependencies(tcpipUdpMenuVisibleSingle, ["TCPIP_USE_UDP"])

	# Default TX Socket Buffer Size
	tcpipUdpSktTxSize = tcpipUdpComponent.createIntegerSymbol("TCPIP_UDP_SOCKET_DEFAULT_TX_SIZE", None)
	tcpipUdpSktTxSize.setLabel("Default TX Buffer Size")
	tcpipUdpSktTxSize.setVisible(True)
	tcpipUdpSktTxSize.setDescription("Default TX Socket Buffer Size")
	tcpipUdpSktTxSize.setDefaultValue(512)
	#tcpipUdpSktTxSize.setDependencies(tcpipUdpMenuVisibleSingle, ["TCPIP_USE_UDP"])

	# Enable Calculate TX Checksum
	tcpipUdpTxUseCheckSum = tcpipUdpComponent.createBooleanSymbol("TCPIP_UDP_USE_TX_CHECKSUM", None)
	tcpipUdpTxUseCheckSum.setLabel("Calculate TX Checksum")
	tcpipUdpTxUseCheckSum.setVisible(True)
	tcpipUdpTxUseCheckSum.setDescription("Enable Calculate TX Checksum")
	tcpipUdpTxUseCheckSum.setDefaultValue(True)
	#tcpipUdpTxUseCheckSum.setDependencies(tcpipUdpMenuVisibleSingle, ["TCPIP_USE_UDP"])

	# Enable Calculate RX Checksum
	tcpipUdpRxUseCheckSum = tcpipUdpComponent.createBooleanSymbol("TCPIP_UDP_USE_RX_CHECKSUM", None)
	tcpipUdpRxUseCheckSum.setLabel("Calculate RX Checksum")
	tcpipUdpRxUseCheckSum.setVisible(True)
	tcpipUdpRxUseCheckSum.setDescription("Enable Calculate RX Checksum")
	tcpipUdpRxUseCheckSum.setDefaultValue(True)
	#tcpipUdpRxUseCheckSum.setDependencies(tcpipUdpMenuVisibleSingle, ["TCPIP_USE_UDP"])

	# Default TX Socket Queue limit
	tcpipUdpSktTxQueueLimit = tcpipUdpComponent.createIntegerSymbol("TCPIP_UDP_SOCKET_DEFAULT_TX_QUEUE_LIMIT", None)
	tcpipUdpSktTxQueueLimit.setLabel("TX Queue Limit")
	tcpipUdpSktTxQueueLimit.setVisible(True)
	tcpipUdpSktTxQueueLimit.setDescription("Default TX Socket Queue limit")
	tcpipUdpSktTxQueueLimit.setDefaultValue(3)
	#tcpipUdpSktTxQueueLimit.setDependencies(tcpipUdpMenuVisibleSingle, ["TCPIP_USE_UDP"])

	# Default RX Socket Queue limit
	tcpipUdpSktRxQueueLimit = tcpipUdpComponent.createIntegerSymbol("TCPIP_UDP_SOCKET_DEFAULT_RX_QUEUE_LIMIT", None)
	tcpipUdpSktRxQueueLimit.setLabel("RX Queue Limit")
	tcpipUdpSktRxQueueLimit.setVisible(True)
	tcpipUdpSktRxQueueLimit.setDescription("Default RX Socket Queue limit")
	tcpipUdpSktRxQueueLimit.setDefaultValue(3)
	#tcpipUdpSktRxQueueLimit.setDependencies(tcpipUdpMenuVisibleSingle, ["TCPIP_USE_UDP"])

	# Enable UDP Pool Buffers
	tcpipUdpPoolBuffers = tcpipUdpComponent.createBooleanSymbol("TCPIP_UDP_USE_POOL_BUFFERS", None)
	tcpipUdpPoolBuffers.setLabel("Enable Pool Buffers")
	tcpipUdpPoolBuffers.setVisible(True)
	tcpipUdpPoolBuffers.setDescription("Enable UDP Pool Buffers")
	tcpipUdpPoolBuffers.setDefaultValue(False)
	#tcpipUdpPoolBuffers.setDependencies(tcpipUdpMenuVisibleSingle, ["TCPIP_USE_UDP"])


	# Number of Socket Pool Buffers
	tcpipUdpSktPoolBufferNum = tcpipUdpComponent.createIntegerSymbol("TCPIP_UDP_SOCKET_POOL_BUFFERS", tcpipUdpPoolBuffers)
	tcpipUdpSktPoolBufferNum.setLabel("Pool Buffers")
	tcpipUdpSktPoolBufferNum.setVisible(False)
	tcpipUdpSktPoolBufferNum.setDescription("Number of Socket Pool Buffers")
	tcpipUdpSktPoolBufferNum.setDefaultValue(4)
	tcpipUdpSktPoolBufferNum.setDependencies(tcpipUdpMenuVisibleSingle, ["TCPIP_UDP_USE_POOL_BUFFERS"])

	# UDP Socket Pool Buffer Size
	tcpipUdpSktPoolBufferSize = tcpipUdpComponent.createIntegerSymbol("TCPIP_UDP_SOCKET_POOL_BUFFER_SIZE", tcpipUdpPoolBuffers)
	tcpipUdpSktPoolBufferSize.setLabel("Pool Buffer Size")
	tcpipUdpSktPoolBufferSize.setVisible(False)
	tcpipUdpSktPoolBufferSize.setDescription("UDP Socket Pool Buffer Size")
	tcpipUdpSktPoolBufferSize.setDefaultValue(512)
	tcpipUdpSktPoolBufferSize.setDependencies(tcpipUdpMenuVisibleSingle, ["TCPIP_UDP_USE_POOL_BUFFERS"])

	# Enable UDP Commands
	tcpipUdpCommands = tcpipUdpComponent.createBooleanSymbol("TCPIP_UDP_COMMANDS", None)
	tcpipUdpCommands.setLabel("Enable UDP Commands")
	tcpipUdpCommands.setVisible(True)
	tcpipUdpCommands.setDescription("Enable UDP Commands")
	tcpipUdpCommands.setDefaultValue(False)
	#tcpipUdpCommands.setDependencies(tcpipUdpMenuVisibleSingle, ["TCPIP_USE_UDP"])

	#Add to system_config.h
	tcpipUdpHeaderFtl = tcpipUdpComponent.createFileSymbol(None, None)
	tcpipUdpHeaderFtl.setSourcePath("library/config/udp.h.ftl")
	tcpipUdpHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipUdpHeaderFtl.setMarkup(True)
	tcpipUdpHeaderFtl.setType("STRING")

	# Add udp.c file
	tcpipUdpSourceFile = tcpipUdpComponent.createFileSymbol(None, None)
	tcpipUdpSourceFile.setSourcePath("library/src/udp.c")
	tcpipUdpSourceFile.setOutputName("udp.c")
	tcpipUdpSourceFile.setOverwrite(True)
	tcpipUdpSourceFile.setDestPath("library/tcpip/src/")
	tcpipUdpSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipUdpSourceFile.setType("SOURCE")
	tcpipUdpSourceFile.setEnabled(True)
	#tcpipUdpSourceFile.setDependencies(tcpipUdpGenSourceFile, ["TCPIP_USE_UDP"])

def tcpipUdpMenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("TCP Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("TCP Menu Invisible.")
		symbol.setVisible(False)	
		
# make UDP options visible
def tcpipUdpMenuVisible(symbol, tcpipIPSymbol):
	tcpipIPv4 = Database.getSymbolValue("tcpipIPv4","TCPIP_STACK_USE_IPV4")
	tcpipIPv6 = Database.getSymbolValue("tcpipIPv6","TCPIP_STACK_USE_IPV6")
	
	print(Database.getSymbolValue("tcpip","TCPIP_USE_UDP"))
	if(tcpipIPv4 or tcpipIPv6):
		symbol.setVisible(True)
		symbol.setValue(True,1)
		print("UDP ON")
	else:
		
		symbol.setVisible(False)
		symbol.setValue(False,1)
		print("UDP OFF")
	print(Database.getSymbolValue("tcpip","TCPIP_USE_UDP"))
	
def tcpipUdpGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])