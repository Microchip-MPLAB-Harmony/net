def instantiateComponent(tcpipIperfComponent):
	print("TCPIP IPERF Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	# Use iperf Benchmark Tool
	tcpipIperf = tcpipIperfComponent.createBooleanSymbol("TCPIP_USE_IPERF", None)
	tcpipIperf.setLabel("Use iperf Benchmark Tool")
	tcpipIperf.setVisible(True)
	tcpipIperf.setDescription("Use iperf Benchmark Tool")
	tcpipIperf.setDefaultValue(False) 

	# Socket TX Buffer Size
	tcpipIperfTxBuffSize = tcpipIperfComponent.createIntegerSymbol("TCPIP_IPERF_TX_BUFFER_SIZE", tcpipIperf)
	tcpipIperfTxBuffSize.setLabel("Socket TX Buffer Size")
	tcpipIperfTxBuffSize.setVisible(False)
	tcpipIperfTxBuffSize.setDescription("Socket TX Buffer Size")
	tcpipIperfTxBuffSize.setDefaultValue(4096)
	tcpipIperfTxBuffSize.setDependencies(tcpipIperfMenuVisible, ["TCPIP_USE_IPERF"])

	# Socket RX Buffer Size
	tcpipIperfRxBuffSize = tcpipIperfComponent.createIntegerSymbol("TCPIP_IPERF_RX_BUFFER_SIZE", tcpipIperf)
	tcpipIperfRxBuffSize.setLabel("Socket RX Buffer Size")
	tcpipIperfRxBuffSize.setVisible(False)
	tcpipIperfRxBuffSize.setDescription("Socket RX Buffer Size")
	tcpipIperfRxBuffSize.setDefaultValue(4096)
	tcpipIperfRxBuffSize.setDependencies(tcpipIperfMenuVisible, ["TCPIP_USE_IPERF"])

	# Time-out for TX Channel to Become Ready in ms
	tcpipIperfTxWaitTimeout = tcpipIperfComponent.createStringSymbol("TCPIP_IPERF_TX_WAIT_TMO", tcpipIperf)
	tcpipIperfTxWaitTimeout.setLabel("Time-out for TX Channel to Become Ready in ms")
	tcpipIperfTxWaitTimeout.setVisible(False)
	tcpipIperfTxWaitTimeout.setDescription("Time-out for TX Channel to Become Ready in ms")
	tcpipIperfTxWaitTimeout.setDefaultValue("100")
	tcpipIperfTxWaitTimeout.setDependencies(tcpipIperfMenuVisible, ["TCPIP_USE_IPERF"])

	# Maximum Number of UDP TX Packet Queue
	tcpipIperfTxQueueLimit = tcpipIperfComponent.createIntegerSymbol("TCPIP_IPERF_TX_QUEUE_LIMIT", tcpipIperf)
	tcpipIperfTxQueueLimit.setLabel("Maximum Number of UDP TX Packet Queue")
	tcpipIperfTxQueueLimit.setVisible(False)
	tcpipIperfTxQueueLimit.setDescription("Maximum Number of UDP TX Packet Queue")
	tcpipIperfTxQueueLimit.setDefaultValue(2)
	tcpipIperfTxQueueLimit.setDependencies(tcpipIperfMenuVisible, ["TCPIP_USE_IPERF"])

	# Iperf timing error in ms
	tcpipIperfTimingError = tcpipIperfComponent.createIntegerSymbol("TCPIP_IPERF_TIMING_ERROR_MARGIN", tcpipIperf)
	tcpipIperfTimingError.setLabel("Iperf timing error - ms")
	tcpipIperfTimingError.setVisible(False)
	tcpipIperfTimingError.setDescription("Iperf timing error in ms")
	tcpipIperfTimingError.setDefaultValue(0)
	tcpipIperfTimingError.setDependencies(tcpipIperfMenuVisible, ["TCPIP_USE_IPERF"])

	# Number of Iperf Instances
	tcpipIperfInstancesMax = tcpipIperfComponent.createIntegerSymbol("TCPIP_IPERF_MAX_INSTANCES", tcpipIperf)
	tcpipIperfInstancesMax.setLabel("Number of Iperf Instances")
	tcpipIperfInstancesMax.setVisible(False)
	tcpipIperfInstancesMax.setDescription("Number of Iperf Instances")
	tcpipIperfInstancesMax.setDefaultValue(1)
	tcpipIperfInstancesMax.setDependencies(tcpipIperfMenuVisible, ["TCPIP_USE_IPERF"])

	# TX Default Bandwidth in Mbps
	tcpipIperfTxBwLimit= tcpipIperfComponent.createIntegerSymbol("TCPIP_IPERF_TX_BW_LIMIT", tcpipIperf)
	tcpipIperfTxBwLimit.setLabel("TX Default Bandwidth(in Mbps)")
	tcpipIperfTxBwLimit.setVisible(False)
	tcpipIperfTxBwLimit.setDescription("TX Default Bandwidth in Mbps")
	tcpipIperfTxBwLimit.setDefaultValue(1)
	tcpipIperfTxBwLimit.setDependencies(tcpipIperfMenuVisible, ["TCPIP_USE_IPERF"])

	# Add iperf.h file to project
	tcpipIperfHeaderFile = tcpipIperfComponent.createFileSymbol(None, None)
	tcpipIperfHeaderFile.setSourcePath("library/iperf.h")
	tcpipIperfHeaderFile.setOutputName("iperf.h")
	tcpipIperfHeaderFile.setDestPath("library/tcpip/")
	tcpipIperfHeaderFile.setProjectPath("config/" + configName + "library/tcpip/")
	tcpipIperfHeaderFile.setType("HEADER")
	tcpipIperfHeaderFile.setOverwrite(True)

	# Add iperf_manager.h file to project
	tcpipIperfManagerHeaderFile = tcpipIperfComponent.createFileSymbol(None, None)
	tcpipIperfManagerHeaderFile.setSourcePath("library/src/iperf_manager.h")
	tcpipIperfManagerHeaderFile.setOutputName("iperf_manager.h")
	tcpipIperfManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipIperfManagerHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipIperfManagerHeaderFile.setType("HEADER")
	tcpipIperfManagerHeaderFile.setOverwrite(True)

	# Add iperf.c file
	tcpipIperfSourceFile = tcpipIperfComponent.createFileSymbol(None, None)
	tcpipIperfSourceFile.setSourcePath("library/src/iperf.c")
	tcpipIperfSourceFile.setOutputName("iperf.c")
	tcpipIperfSourceFile.setOverwrite(True)
	tcpipIperfSourceFile.setDestPath("library/tcpip/src/")
	tcpipIperfSourceFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipIperfSourceFile.setType("SOURCE")
	tcpipIperfSourceFile.setEnabled(True)
	tcpipIperfSourceFile.setDependencies(tcpipIperfGenSourceFile, ["TCPIP_USE_IPERF"])
	
def tcpipIperfMenuVisible(symbol, event):
	if (event["value"] == True):
		print("Telnet Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("Telnet Menu Invisible.")
		symbol.setVisible(False)
		
def tcpipIperfGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])