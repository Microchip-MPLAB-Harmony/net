def instantiateComponent(tcpipIperfComponent):
	print("TCPIP IPERF Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	# Use iperf Benchmark Tool
	tcpipIperf = tcpipIperfComponent.createBooleanSymbol("TCPIP_USE_IPERF", None)
	tcpipIperf.setLabel("Use iperf Benchmark Tool")
	tcpipIperf.setVisible(False)
	tcpipIperf.setDescription("Use iperf Benchmark Tool")
	tcpipIperf.setDefaultValue(True) 

	# Socket TX Buffer Size
	tcpipIperfTxBuffSize = tcpipIperfComponent.createIntegerSymbol("TCPIP_IPERF_TX_BUFFER_SIZE", None)
	tcpipIperfTxBuffSize.setLabel("Socket TX Buffer Size")
	tcpipIperfTxBuffSize.setVisible(True)
	tcpipIperfTxBuffSize.setDescription("Socket TX Buffer Size")
	tcpipIperfTxBuffSize.setDefaultValue(4096)
	#tcpipIperfTxBuffSize.setDependencies(tcpipIperfMenuVisible, ["TCPIP_USE_IPERF"])

	# Socket RX Buffer Size
	tcpipIperfRxBuffSize = tcpipIperfComponent.createIntegerSymbol("TCPIP_IPERF_RX_BUFFER_SIZE", None)
	tcpipIperfRxBuffSize.setLabel("Socket RX Buffer Size")
	tcpipIperfRxBuffSize.setVisible(True)
	tcpipIperfRxBuffSize.setDescription("Socket RX Buffer Size")
	tcpipIperfRxBuffSize.setDefaultValue(4096)
	#tcpipIperfRxBuffSize.setDependencies(tcpipIperfMenuVisible, ["TCPIP_USE_IPERF"])

	# Time-out for TX Channel to Become Ready in ms
	tcpipIperfTxWaitTimeout = tcpipIperfComponent.createStringSymbol("TCPIP_IPERF_TX_WAIT_TMO", None)
	tcpipIperfTxWaitTimeout.setLabel("Time-out for TX Channel to Become Ready in ms")
	tcpipIperfTxWaitTimeout.setVisible(True)
	tcpipIperfTxWaitTimeout.setDescription("Time-out for TX Channel to Become Ready in ms")
	tcpipIperfTxWaitTimeout.setDefaultValue("100")
	#tcpipIperfTxWaitTimeout.setDependencies(tcpipIperfMenuVisible, ["TCPIP_USE_IPERF"])

	# Maximum Number of UDP TX Packet Queue
	tcpipIperfTxQueueLimit = tcpipIperfComponent.createIntegerSymbol("TCPIP_IPERF_TX_QUEUE_LIMIT", None)
	tcpipIperfTxQueueLimit.setLabel("Maximum Number of UDP TX Packet Queue")
	tcpipIperfTxQueueLimit.setVisible(True)
	tcpipIperfTxQueueLimit.setDescription("Maximum Number of UDP TX Packet Queue")
	tcpipIperfTxQueueLimit.setDefaultValue(2)
	#tcpipIperfTxQueueLimit.setDependencies(tcpipIperfMenuVisible, ["TCPIP_USE_IPERF"])

	# Iperf timing error in ms
	tcpipIperfTimingError = tcpipIperfComponent.createIntegerSymbol("TCPIP_IPERF_TIMING_ERROR_MARGIN", None)
	tcpipIperfTimingError.setLabel("Iperf timing error - ms")
	tcpipIperfTimingError.setVisible(True)
	tcpipIperfTimingError.setDescription("Iperf timing error in ms")
	tcpipIperfTimingError.setDefaultValue(0)
	#tcpipIperfTimingError.setDependencies(tcpipIperfMenuVisible, ["TCPIP_USE_IPERF"])

	# Number of Iperf Instances
	tcpipIperfInstancesMax = tcpipIperfComponent.createIntegerSymbol("TCPIP_IPERF_MAX_INSTANCES", None)
	tcpipIperfInstancesMax.setLabel("Number of Iperf Instances")
	tcpipIperfInstancesMax.setVisible(True)
	tcpipIperfInstancesMax.setDescription("Number of Iperf Instances")
	tcpipIperfInstancesMax.setDefaultValue(1)
	#tcpipIperfInstancesMax.setDependencies(tcpipIperfMenuVisible, ["TCPIP_USE_IPERF"])

	# TX Default Bandwidth in Mbps
	tcpipIperfTxBwLimit= tcpipIperfComponent.createIntegerSymbol("TCPIP_IPERF_TX_BW_LIMIT", None)
	tcpipIperfTxBwLimit.setLabel("TX Default Bandwidth(in Mbps)")
	tcpipIperfTxBwLimit.setVisible(True)
	tcpipIperfTxBwLimit.setDescription("TX Default Bandwidth in Mbps")
	tcpipIperfTxBwLimit.setDefaultValue(1)
	#tcpipIperfTxBwLimit.setDependencies(tcpipIperfMenuVisible, ["TCPIP_USE_IPERF"])

	#Add to system_config.h
	tcpipIperfHeaderFtl = tcpipIperfComponent.createFileSymbol(None, None)
	tcpipIperfHeaderFtl.setSourcePath("library/config/iperf.h.ftl")
	tcpipIperfHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipIperfHeaderFtl.setMarkup(True)
	tcpipIperfHeaderFtl.setType("STRING")

	# Add iperf.c file
	tcpipIperfSourceFile = tcpipIperfComponent.createFileSymbol(None, None)
	tcpipIperfSourceFile.setSourcePath("library/src/iperf.c")
	tcpipIperfSourceFile.setOutputName("iperf.c")
	tcpipIperfSourceFile.setOverwrite(True)
	tcpipIperfSourceFile.setDestPath("library/tcpip/src/")
	tcpipIperfSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipIperfSourceFile.setType("SOURCE")
	tcpipIperfSourceFile.setEnabled(True)
	#tcpipIperfSourceFile.setDependencies(tcpipIperfGenSourceFile, ["TCPIP_USE_IPERF"])
	
def tcpipIperfMenuVisible(symbol, event):
	if (event["value"] == True):
		print("Telnet Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("Telnet Menu Invisible.")
		symbol.setVisible(False)
		
def tcpipIperfGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])