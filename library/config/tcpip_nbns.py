def instantiateComponent(tcpipNbnsComponent):
	print("TCPIP NBNS Component")
	configName = Variables.get("__CONFIGURATION_NAME")
		
	# Use NetBIOS Name Server
	tcpipNbns = tcpipNbnsComponent.createBooleanSymbol("TCPIP_USE_NBNS", None)
	tcpipNbns.setLabel("Use NetBIOS Name Server")
	tcpipNbns.setVisible(False)
	tcpipNbns.setDescription("Use NetBIOS Name Server")
	tcpipNbns.setDefaultValue(True)
	#tcpipNbns.setDependencies(tcpipNbnsMenuVisible, ["tcpipIPv4.TCPIP_STACK_USE_IPV4", "tcpipUdp.TCPIP_USE_UDP"])

	# NetBIOS Task Rate in ms
	tcpipNbnsTskTickRate = tcpipNbnsComponent.createIntegerSymbol("TCPIP_NBNS_TASK_TICK_RATE", None)
	tcpipNbnsTskTickRate.setLabel("NetBIOS Task Rate - ms")
	tcpipNbnsTskTickRate.setVisible(True)
	tcpipNbnsTskTickRate.setDescription("NetBIOS Task Rate in ms")
	tcpipNbnsTskTickRate.setDefaultValue(110)
	#tcpipNbnsTskTickRate.setDependencies(tcpipNbnsMenuVisibleSingle, ["TCPIP_USE_NBNS"])

	#Add to system_config.h
	tcpipNbnsHeaderFtl = tcpipNbnsComponent.createFileSymbol(None, None)
	tcpipNbnsHeaderFtl.setSourcePath("library/config/nbns.h.ftl")
	tcpipNbnsHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipNbnsHeaderFtl.setMarkup(True)
	tcpipNbnsHeaderFtl.setType("STRING")

	# Add nbns.c file
	tcpipNbnsSourceFile = tcpipNbnsComponent.createFileSymbol(None, None)
	tcpipNbnsSourceFile.setSourcePath("library/src/nbns.c")
	tcpipNbnsSourceFile.setOutputName("nbns.c")
	tcpipNbnsSourceFile.setOverwrite(True)
	tcpipNbnsSourceFile.setDestPath("library/tcpip/src/")
	tcpipNbnsSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipNbnsSourceFile.setType("SOURCE")
	tcpipNbnsSourceFile.setEnabled(True)
	#tcpipNbnsSourceFile.setDependencies(tcpipNbnsGenSourceFile, ["TCPIP_USE_NBNS"])

# make Nbns Server option visible
def tcpipNbnsMenuVisible(symbol, event):
	tcpipIPv4 = Database.getSymbolValue("tcpipIPv4","TCPIP_STACK_USE_IPV4")
	tcpipUdp = Database.getSymbolValue("tcpipUdp","TCPIP_USE_UDP")

	if(tcpipIPv4 and tcpipUdp):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)
		
def tcpipNbnsMenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("NBNS Menu Visible.")		
		testname =Variables.get("__PROCESSOR") 
		print(testname)
		symbol.setVisible(True)
	else:
		print("NBNS Menu Invisible.")
		symbol.setVisible(False)

def tcpipNbnsGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])