def instantiateComponent(tcpipCmdComponent):
	print("TCPIP ZEROCONF Component")
	configName = Variables.get("__CONFIGURATION_NAME")
		
	# Use TCP/IP Commands
	tcpipCmd = tcpipCmdComponent.createBooleanSymbol("TCPIP_STACK_USE_COMMANDS", None)
	tcpipCmd.setLabel("Use TCP/IP Commands")
	tcpipCmd.setVisible(False)
	tcpipCmd.setDescription("Use TCP/IP Commands")
	tcpipCmd.setDefaultValue(True)   # Niyas to check the default value
	# Niyas to do 
	# select USE_SYS_RESET_NEEDED
	# select USE_SYS_CONSOLE_NEEDED
	# select USE_SYS_COMMAND_NEEDED 

	# Enable Storage for Stack Commands
	tcpipCmdStorage = tcpipCmdComponent.createBooleanSymbol("TCPIP_STACK_COMMANDS_STORAGE_ENABLE", None)
	tcpipCmdStorage.setLabel("Enable Storage for Stack Commands")
	tcpipCmdStorage.setVisible(True)
	tcpipCmdStorage.setDescription("Enable Storage for Stack Commands")
	tcpipCmdStorage.setDefaultValue(False)  
	tcpipCmdStorage.setDependencies(tcpipCmdStorageMenuVisible, ["tcpipStack.TCPIP_STACK_CONFIGURATION_SAVE_RESTORE"]) 

	# Number of ICMP Echo requests
	tcpipCmdIcmpEchoReqNum = tcpipCmdComponent.createIntegerSymbol("TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUESTS", None)
	tcpipCmdIcmpEchoReqNum.setLabel("Number of ICMP Echo requests")
	tcpipCmdIcmpEchoReqNum.setVisible(True)
	tcpipCmdIcmpEchoReqNum.setDescription("Number of ICMP Echo requests")
	tcpipCmdIcmpEchoReqNum.setDefaultValue(4)
	#tcpipCmdIcmpEchoReqNum.setDependencies(tcpipCmdMenuVisibleSingle, ["TCPIP_STACK_USE_COMMANDS"])

	# ICMP Reply Time-out in ms
	tcpipCmdIcmpEchoReqDelay = tcpipCmdComponent.createIntegerSymbol("TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DELAY", None)
	tcpipCmdIcmpEchoReqDelay.setLabel("ICMP Reply Time-out, ms")
	tcpipCmdIcmpEchoReqDelay.setVisible(True)
	tcpipCmdIcmpEchoReqDelay.setDescription("ICMP Reply Time-out in ms")
	tcpipCmdIcmpEchoReqDelay.setDefaultValue(1000)
	#tcpipCmdIcmpEchoReqDelay.setDependencies(tcpipCmdMenuVisibleSingle, ["TCPIP_STACK_USE_COMMANDS"])

	# ICMP Give Up Time-out in ms
	tcpipCmdIcmpEchoTimeout= tcpipCmdComponent.createIntegerSymbol("TCPIP_STACK_COMMANDS_ICMP_ECHO_TIMEOUT", None)
	tcpipCmdIcmpEchoTimeout.setLabel("ICMP Give Up Time-out, ms")
	tcpipCmdIcmpEchoTimeout.setVisible(True)
	tcpipCmdIcmpEchoTimeout.setDescription("ICMP Give Up Time-out in ms")
	tcpipCmdIcmpEchoTimeout.setDefaultValue(5000)
	#tcpipCmdIcmpEchoTimeout.setDependencies(tcpipCmdMenuVisibleSingle, ["TCPIP_STACK_USE_COMMANDS"])

	# Enable Wi-Fi Related Commands
	tcpipCmdWifi = tcpipCmdComponent.createBooleanSymbol("TCPIP_STACK_COMMANDS_WIFI_ENABLE", None)
	tcpipCmdWifi.setLabel("Enable Wi-Fi Related Commands")
	tcpipCmdWifi.setVisible(True)
	tcpipCmdWifi.setDescription("Enable Wi-Fi Related Commands")
	tcpipCmdWifi.setDefaultValue(False) 
	#tcpipCmdWifi.setDependencies(tcpipCmdMenuVisibleSingle, ["TCPIP_STACK_USE_COMMANDS"])

	# Echo Request Data Buffer size - bytes
	tcpipCmdIcmpEchoReqBuffSize= tcpipCmdComponent.createIntegerSymbol("TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_BUFF_SIZE", None)
	tcpipCmdIcmpEchoReqBuffSize.setLabel("Echo Request Data Buffer size - bytes")
	tcpipCmdIcmpEchoReqBuffSize.setVisible(True)
	tcpipCmdIcmpEchoReqBuffSize.setDescription("Echo Request Data Buffer size - bytes")
	tcpipCmdIcmpEchoReqBuffSize.setDefaultValue(2000)
	#tcpipCmdIcmpEchoReqBuffSize.setDependencies(tcpipCmdMenuVisibleSingle, ["tcpipTcp.TCPIP_USE_TCP"])

	# Echo request Default Data Size - bytes
	tcpipCmdIcmpEchoReqDataSize= tcpipCmdComponent.createIntegerSymbol("TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DATA_SIZE", None)
	tcpipCmdIcmpEchoReqDataSize.setLabel("Echo request Default Data Size - bytes")
	tcpipCmdIcmpEchoReqDataSize.setVisible(True)
	tcpipCmdIcmpEchoReqDataSize.setDescription("Echo request Default Data Size - bytes")
	tcpipCmdIcmpEchoReqDataSize.setDefaultValue(100)
	tcpipCmdIcmpEchoReqDataSize.setDependencies(tcpipCmdMenuVisibleSingle, ["tcpipTcp.TCPIP_USE_TCP"])

	#Add to system_config.h
	tcpipCmdHeaderFtl = tcpipCmdComponent.createFileSymbol(None, None)
	tcpipCmdHeaderFtl.setSourcePath("library/config/tcpip_cmd.h.ftl")
	tcpipCmdHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipCmdHeaderFtl.setMarkup(True)
	tcpipCmdHeaderFtl.setType("STRING")

	# Add tcpip_commands.c file
	tcpipCmdSourceFile = tcpipCmdComponent.createFileSymbol(None, None)
	tcpipCmdSourceFile.setSourcePath("library/src/tcpip_commands.c")
	tcpipCmdSourceFile.setOutputName("tcpip_commands.c")
	tcpipCmdSourceFile.setOverwrite(True)
	tcpipCmdSourceFile.setDestPath("library/tcpip/src/")
	tcpipCmdSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipCmdSourceFile.setType("SOURCE")
	tcpipCmdSourceFile.setEnabled(True)
	#tcpipCmdSourceFile.setDependencies(tcpipCmdGenSourceFile, ["TCPIP_STACK_USE_COMMANDS"])

# Enable Storage for Stack Commands option visible
def tcpipCmdStorageMenuVisible(tcpipDependentSymbol, tcpipIPSymbol):	

	if (event["value"] == True):
		tcpipDependentSymbol.setVisible(True)
	else:
		tcpipDependentSymbol.setVisible(False)
		
def tcpipCmdMenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("TFTPC Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("TFTPC Menu Invisible.")
		symbol.setVisible(False)

def tcpipCmdGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])