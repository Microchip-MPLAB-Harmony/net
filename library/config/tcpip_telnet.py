    
def instantiateComponent(tcpipTelnetComponent):
	print("TCPIP Telnet Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	# Use Telnet Server
	tcpipTelnet = tcpipTelnetComponent.createBooleanSymbol("TCPIP_USE_TELNET", None)
	tcpipTelnet.setLabel("Use Telnet Server")
	tcpipTelnet.setVisible(True)
	tcpipTelnet.setDescription("Use Telnet Server")
	tcpipTelnet.setDefaultValue(False) 
	 # niyas to do   
	 # select NET_PRES_NEEDED
	 # select USE_SYS_COMMAND_NEEDED
	tcpipTelnet.setDependencies(tcpipTelnetMenuVisible, ["tcpipTcp.TCPIP_USE_TCP"])

	# Max Connections
	tcpipTelnetConnMax = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_MAX_CONNECTIONS", tcpipTelnet)
	tcpipTelnetConnMax.setLabel("Maximum Connections")
	tcpipTelnetConnMax.setVisible(False)
	tcpipTelnetConnMax.setDescription("Maximum Connections")
	tcpipTelnetConnMax.setDefaultValue(2)
	tcpipTelnetConnMax.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

	# Telnet User Name
	tcpipTelnetUsrName = tcpipTelnetComponent.createStringSymbol("TCPIP_TELNET_USERNAME", tcpipTelnet)
	tcpipTelnetUsrName.setLabel("User Name")
	tcpipTelnetUsrName.setVisible(False)
	tcpipTelnetUsrName.setDescription("Telnet User Name")
	tcpipTelnetUsrName.setDefaultValue("admin")
	tcpipTelnetUsrName.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

	# Telnet Password
	tcpipTelnetPswd = tcpipTelnetComponent.createStringSymbol("TCPIP_TELNET_PASSWORD", tcpipTelnet)
	tcpipTelnetPswd.setLabel("Password")
	tcpipTelnetPswd.setVisible(False)
	tcpipTelnetPswd.setDescription("Telnet Password")
	tcpipTelnetPswd.setDefaultValue("microchip")
	tcpipTelnetPswd.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

	# Task Rate in ms
	tcpipTelnetTskTickRate = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_TASK_TICK_RATE", tcpipTelnet)
	tcpipTelnetTskTickRate.setLabel("Task Rate - ms")
	tcpipTelnetTskTickRate.setVisible(False)
	tcpipTelnetTskTickRate.setDescription("Task Rate in ms")
	tcpipTelnetTskTickRate.setDefaultValue(100)
	tcpipTelnetTskTickRate.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

	# Reject Unsecure Connections
	tcpipTelnetRejectUnsecure = tcpipTelnetComponent.createBooleanSymbol("TCPIP_TELNET_REJECT_UNSECURED", tcpipTelnet)
	tcpipTelnetRejectUnsecure.setLabel("Reject Unsecure Connections")
	tcpipTelnetRejectUnsecure.setVisible(False)
	tcpipTelnetRejectUnsecure.setDescription("Reject Unsecure Connections")
	tcpipTelnetRejectUnsecure.setDefaultValue(False)
	tcpipTelnetRejectUnsecure.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

	#Add to system_config.h
	tcpipTelnetHeaderFtl = tcpipTelnetComponent.createFileSymbol(None, None)
	tcpipTelnetHeaderFtl.setSourcePath("library/config/telnet.h.ftl")
	tcpipTelnetHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipTelnetHeaderFtl.setMarkup(True)
	tcpipTelnetHeaderFtl.setType("STRING")

	# Add telnet.c file
	tcpipTelnetSourceFile = tcpipTelnetComponent.createFileSymbol(None, None)
	tcpipTelnetSourceFile.setSourcePath("library/src/telnet.c")
	tcpipTelnetSourceFile.setOutputName("telnet.c")
	tcpipTelnetSourceFile.setOverwrite(True)
	tcpipTelnetSourceFile.setDestPath("library/tcpip/src/")
	tcpipTelnetSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipTelnetSourceFile.setType("SOURCE")
	tcpipTelnetSourceFile.setEnabled(True)
	tcpipTelnetSourceFile.setDependencies(tcpipTelnetGenSourceFile, ["TCPIP_USE_TELNET"])
	
def tcpipTelnetMenuVisible(symbol, event):
	if (event["value"] == True):
		print("Telnet Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("Telnet Menu Invisible.")
		symbol.setVisible(False)
		
def tcpipTelnetGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])