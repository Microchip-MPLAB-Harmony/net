    
def instantiateComponent(tcpipDhcpComponent):
	print("TCPIP DHCP Client Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	
	# Enable DHCP Client
	tcpipDhcpc = tcpipDhcpComponent.createBooleanSymbol("TCPIP_STACK_USE_DHCP_CLIENT", None)
	tcpipDhcpc.setLabel("DHCP Client")
	tcpipDhcpc.setVisible(True)
	tcpipDhcpc.setDescription("Enable DHCP Client")
	tcpipDhcpc.setDefaultValue(True)
	tcpipDhcpc.setDependencies(tcpipDhcpcMenuVisible, ["tcpipIPv4.TCPIP_STACK_USE_IPV4", "tcpipUdp.TCPIP_USE_UDP"])

	# DHCP Client Request Time-out in seconds
	tcpipDhcpcReqTimeout = tcpipDhcpComponent.createIntegerSymbol("TCPIP_DHCP_TIMEOUT", tcpipDhcpc)
	tcpipDhcpcReqTimeout.setLabel("DHCP Request Time-out (seconds)")
	tcpipDhcpcReqTimeout.setVisible(True)
	tcpipDhcpcReqTimeout.setDescription("DHCP Request Time-out in seconds")
	tcpipDhcpcReqTimeout.setDefaultValue(10)
	tcpipDhcpcReqTimeout.setDependencies(tcpipDhcpMenuVisibleSingle, ["TCPIP_STACK_USE_DHCP_CLIENT"])

	# DHCP Client Tick Rate in msec
	tcpipDhcpcTickRate = tcpipDhcpComponent.createIntegerSymbol("TCPIP_DHCP_TASK_TICK_RATE", tcpipDhcpc)
	tcpipDhcpcTickRate.setLabel("DHCP Tick Rate (msec)")
	tcpipDhcpcTickRate.setVisible(True)
	tcpipDhcpcTickRate.setDescription("DHCP Tick Rate in msec")
	tcpipDhcpcTickRate.setDefaultValue(5)
	tcpipDhcpcTickRate.setDependencies(tcpipDhcpMenuVisibleSingle, ["TCPIP_STACK_USE_DHCP_CLIENT"])


	# Enable DHCP Client by default at Stack Start-up
	tcpipDhcpcEnable = tcpipDhcpComponent.createBooleanSymbol("TCPIP_DHCP_CLIENT_ENABLED", tcpipDhcpc)
	tcpipDhcpcEnable.setLabel("DHCP Client enabled by default at Stack Start-up")
	tcpipDhcpcEnable.setVisible(True)
	tcpipDhcpcEnable.setDescription("Enable DHCP Client by default at Stack Start-up")
	tcpipDhcpcEnable.setDefaultValue(True)
	tcpipDhcpcEnable.setDependencies(tcpipDhcpMenuVisibleSingle, ["TCPIP_STACK_USE_DHCP_CLIENT"])

	# DHCP Client Host name maximum size
	tcpipDhcpcHostNameSize = tcpipDhcpComponent.createIntegerSymbol("TCPIP_DHCP_HOST_NAME_SIZE", tcpipDhcpc)
	tcpipDhcpcHostNameSize.setLabel("DHCP Host name maximum size")
	tcpipDhcpcHostNameSize.setVisible(True)
	tcpipDhcpcHostNameSize.setDescription("DHCP Client Host name maximum size")
	tcpipDhcpcHostNameSize.setDefaultValue(20)
	tcpipDhcpcHostNameSize.setDependencies(tcpipDhcpMenuVisibleSingle, ["TCPIP_STACK_USE_DHCP_CLIENT"])

	# DHCP Client Host name maximum size
	tcpipDhcpcConnectPort = tcpipDhcpComponent.createIntegerSymbol("TCPIP_DHCP_CLIENT_CONNECT_PORT", tcpipDhcpc)
	tcpipDhcpcConnectPort.setLabel("Client Port for DHCP Client Transactions")
	tcpipDhcpcConnectPort.setVisible(True)
	tcpipDhcpcConnectPort.setDescription("Client Port for DHCP Client Transactions")
	tcpipDhcpcConnectPort.setDefaultValue(68)
	tcpipDhcpcConnectPort.setDependencies(tcpipDhcpMenuVisibleSingle, ["TCPIP_STACK_USE_DHCP_CLIENT"])

	# Remote Server Port for DHCP Server Messages
	tcpipDhcpcServerListenPort= tcpipDhcpComponent.createIntegerSymbol("TCPIP_DHCP_SERVER_LISTEN_PORT", tcpipDhcpc)
	tcpipDhcpcServerListenPort.setLabel("Remote Server Port for DHCP Server Messages")
	tcpipDhcpcServerListenPort.setVisible(True)
	tcpipDhcpcServerListenPort.setDescription("Remote Server Port for DHCP Server Messages")
	tcpipDhcpcServerListenPort.setDefaultValue(67)
	tcpipDhcpcServerListenPort.setDependencies(tcpipDhcpMenuVisibleSingle, ["TCPIP_STACK_USE_DHCP_CLIENT"])
	
	#Add to system_config.h
	tcpipDhcpcHeaderFtl = tcpipDhcpComponent.createFileSymbol(None, None)
	tcpipDhcpcHeaderFtl.setSourcePath("library/config/dhcp.h.ftl")
	tcpipDhcpcHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipDhcpcHeaderFtl.setMarkup(True)
	tcpipDhcpcHeaderFtl.setType("STRING")
	
	# Add dhcp.h file to project
	tcpipDhcpcHeaderFile = tcpipDhcpComponent.createFileSymbol(None, None)
	tcpipDhcpcHeaderFile.setSourcePath("library/dhcp.h")
	tcpipDhcpcHeaderFile.setOutputName("dhcp.h")
	tcpipDhcpcHeaderFile.setDestPath("library/tcpip/")
	tcpipDhcpcHeaderFile.setProjectPath("config/" + configName + "library/tcpip/")
	tcpipDhcpcHeaderFile.setType("HEADER")
	tcpipDhcpcHeaderFile.setOverwrite(True)

	# Add dhcp_manager.h file to project
	tcpipDhcpcManagerHeaderFile = tcpipDhcpComponent.createFileSymbol(None, None)
	tcpipDhcpcManagerHeaderFile.setSourcePath("library/src/dhcp_manager.h")
	tcpipDhcpcManagerHeaderFile.setOutputName("dhcp_manager.h")
	tcpipDhcpcManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipDhcpcManagerHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipDhcpcManagerHeaderFile.setType("HEADER")
	tcpipDhcpcManagerHeaderFile.setOverwrite(True)

	# Add dhcp_private.h file to project
	tcpipDhcpcPrivateHeaderFile = tcpipDhcpComponent.createFileSymbol(None, None)
	tcpipDhcpcPrivateHeaderFile.setSourcePath("library/src/dhcp_private.h")
	tcpipDhcpcPrivateHeaderFile.setOutputName("dhcp_private.h")
	tcpipDhcpcPrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipDhcpcPrivateHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipDhcpcPrivateHeaderFile.setType("HEADER")
	tcpipDhcpcPrivateHeaderFile.setOverwrite(True)

	# Add dhcp.c file
	tcpipDhcpcSourceFile = tcpipDhcpComponent.createFileSymbol(None, None)
	tcpipDhcpcSourceFile.setSourcePath("library/src/dhcp.c")
	tcpipDhcpcSourceFile.setOutputName("dhcp.c")
	tcpipDhcpcSourceFile.setOverwrite(True)
	tcpipDhcpcSourceFile.setDestPath("library/tcpip/src/")
	tcpipDhcpcSourceFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipDhcpcSourceFile.setType("SOURCE")
	tcpipDhcpcSourceFile.setEnabled(True)
	
	
def tcpipDhcpMenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("DHCP Client Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("DHCP Client Menu Invisible.")
		symbol.setVisible(False)	

# make DHCP Client option visible
def tcpipDhcpcMenuVisible(tcpipDependentSymbol, tcpipIPSymbol):
	tcpipIPv4 = Database.getSymbolValue("tcpipIPv4","TCPIP_STACK_USE_IPV4")
	tcpipUdp = Database.getSymbolValue("tcpipUdp","TCPIP_USE_UDP")
	print("DHCP IPv4 UDP  Menu On.")
	if(tcpipIPv4 and tcpipUdp):
		tcpipDependentSymbol.setVisible(True)
		print("DHCP IPv4 UDP  Menu On.")
	else:
		tcpipDependentSymbol.setVisible(False)	
		print("DHCP IPv4 UDP  Menu Off.")		
