    
def instantiateComponent(tcpipDdnsComponent):
	print("TCPIP DNS Client Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	
	# Enable Dynamis DNS Module
	tcpipDdns = tcpipDdnsComponent.createBooleanSymbol("TCPIP_USE_DDNS", None)
	tcpipDdns.setLabel("Dynamic DNS Module")
	tcpipDdns.setVisible(True)
	tcpipDdns.setDescription("Enable Dynamic DNS Module")
	tcpipDdns.setDefaultValue(False)
	tcpipDdns.setDependencies(tcpipDdnsMenuVisible, ["tcpipIPv4.TCPIP_STACK_USE_IPV4", "tcpipUdp.TCPIP_USE_UDP"])

	# Default CheckIP server for Determining Current IP Address
	tcpipDdnsChkIpServer = tcpipDdnsComponent.createStringSymbol("TCPIP_DDNS_CHECKIP_SERVER", tcpipDdns)
	tcpipDdnsChkIpServer.setLabel("Default CheckIP server for Determining Current IP Address")
	tcpipDdnsChkIpServer.setVisible(False)
	tcpipDdnsChkIpServer.setDescription("Default CheckIP server for Determining Current IP Address")
	tcpipDdnsChkIpServer.setDefaultValue("checkip.dyndns.com")
	tcpipDdnsChkIpServer.setDependencies(tcpipDdnsMenuVisible, ["TCPIP_USE_DDNS"])

	# Dynamic DNS Task Rate in msec
	tcpipDdnsTskRate = tcpipDdnsComponent.createIntegerSymbol("TCPIP_DDNS_TASK_TICK_RATE", tcpipDdns)
	tcpipDdnsTskRate.setLabel("dynDNS Task Rate in msec")
	tcpipDdnsTskRate.setVisible(False)
	tcpipDdnsTskRate.setDescription("Dynamic DNS Task Rate in msec")
	tcpipDdnsTskRate.setDefaultValue(777)
	tcpipDdnsTskRate.setDependencies(tcpipDdnsMenuVisible, ["TCPIP_USE_DDNS"])
	
	# Add to system_config.h
	tcpipDdnsHeaderFtl = tcpipDdnsComponent.createFileSymbol(None, None)
	tcpipDdnsHeaderFtl.setSourcePath("library/config/ddns.h.ftl")
	tcpipDdnsHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipDdnsHeaderFtl.setMarkup(True)
	tcpipDdnsHeaderFtl.setType("STRING")

	# Add ddns.c file
	tcpipDdnsSourceFile = tcpipDdnsComponent.createFileSymbol(None, None)
	tcpipDdnsSourceFile.setSourcePath("library/src/ddns.c")
	tcpipDdnsSourceFile.setOutputName("ddns.c")
	tcpipDdnsSourceFile.setOverwrite(True)
	tcpipDdnsSourceFile.setDestPath("library/tcpip/src/")
	tcpipDdnsSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipDdnsSourceFile.setType("SOURCE")
	tcpipDdnsSourceFile.setEnabled(True)
	tcpipDdnsSourceFile.setDependencies(tcpipDdnsGenSourceFile, ["TCPIP_USE_DDNS"])

# make Dynamic DNS module option visible
def tcpipDdnsMenuVisible(symbol, event):
	tcpipIPv4 = Database.getSymbolValue("tcpipIPv4","TCPIP_STACK_USE_IPV4")
	tcpipUdp = Database.getSymbolValue("tcpipUdp","TCPIP_USE_UDP")

	if(tcpipIPv4 and tcpipUdp):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)	
	
	
def tcpipDdnsMenuVisible(symbol, event):
	if (event["value"] == True):
		print("DDNS Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("DDNS Menu Invisible.")
		symbol.setVisible(False)	

		
def tcpipDdnsGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])