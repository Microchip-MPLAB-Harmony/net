    
def instantiateComponent(tcpipIcmpv6Component):
	print("TCPIP ICMPv6 Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	
	# Use ICMPv6 Server
	tcpipIcmpv6UseServer = tcpipIcmpv6Component.createBooleanSymbol("TCPIP_STACK_USE_ICMPV6_SERVER", None)
	tcpipIcmpv6UseServer.setLabel("Use ICMPv6 Server")
	tcpipIcmpv6UseServer.setVisible(True)
	tcpipIcmpv6UseServer.setDescription("Use ICMPv6 Server")
	tcpipIcmpv6UseServer.setDefaultValue(True)

	# Use ICMPv6 Client
	tcpipIcmpv6UseClient = tcpipIcmpv6Component.createBooleanSymbol("TCPIP_STACK_USE_ICMPV6_CLIENT", None)
	tcpipIcmpv6UseClient.setLabel("Use ICMPv6 Client")
	tcpipIcmpv6UseClient.setVisible(True)
	tcpipIcmpv6UseClient.setDescription("Use ICMPv6 Client")
	tcpipIcmpv6UseClient.setDefaultValue(False)

	# Enable IPV6 User Notification
	tcpipIcmpv6ClientUserNotify = tcpipIcmpv6Component.createBooleanSymbol("TCPIP_ICMPV6_CLIENT_USER_NOTIFICATION", tcpipIcmpv6UseClient)
	tcpipIcmpv6ClientUserNotify.setLabel("Enable ICMPV6 User Notification")
	tcpipIcmpv6ClientUserNotify.setVisible(False)
	tcpipIcmpv6ClientUserNotify.setDescription("Enable ICMPV6 User Notification")
	tcpipIcmpv6ClientUserNotify.setDefaultValue(True)
	tcpipIcmpv6ClientUserNotify.setDependencies(tcpipIcmpv6MenuVisible, ["TCPIP_STACK_USE_ICMPV6_CLIENT"])

	# Add icmpv6.c file
	tcpipICMPv6SourceFile = tcpipIcmpv6Component.createFileSymbol(None, None)
	tcpipICMPv6SourceFile.setSourcePath("tcpip/src/icmpv6.c")
	tcpipICMPv6SourceFile.setOutputName("icmpv6.c")
	tcpipICMPv6SourceFile.setOverwrite(True)
	tcpipICMPv6SourceFile.setDestPath("library/tcpip/src/")
	tcpipICMPv6SourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipICMPv6SourceFile.setType("SOURCE")
	tcpipICMPv6SourceFile.setEnabled(True)

	#Add to system_config.h
	tcpipIcmpv6HeaderFtl = tcpipIcmpv6Component.createFileSymbol(None, None)
	tcpipIcmpv6HeaderFtl.setSourcePath("tcpip/config/icmpv6.h.ftl")
	tcpipIcmpv6HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipIcmpv6HeaderFtl.setMarkup(True)
	tcpipIcmpv6HeaderFtl.setType("STRING")
	
def tcpipIcmpv6MenuVisible(symbol, event):
	if (event["value"] == True):
		print("TCPIP Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("TCPIP Menu Invisible.")
		symbol.setVisible(False)

def destroyComponent(component):
	Database.setSymbolValue("tcpipIcmpv6", "TCPIP_STACK_USE_ICMPV6_SERVER", False, 2)