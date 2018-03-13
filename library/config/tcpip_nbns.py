def instantiateComponent(tcpipNbnsComponent):
	print("TCPIP NBNS Component")
	configName = Variables.get("__CONFIGURATION_NAME")
		
	# Use NetBIOS Name Server
	tcpipNbns = tcpipNbnsComponent.createBooleanSymbol("TCPIP_USE_NBNS", None)
	tcpipNbns.setLabel("Use NetBIOS Name Server")
	tcpipNbns.setVisible(True)
	tcpipNbns.setDescription("Use NetBIOS Name Server")
	tcpipNbns.setDefaultValue(True)
	tcpipNbns.setDependencies(tcpipNbnsMenuVisible, ["tcpipIPv4.TCPIP_STACK_USE_IPV4", "tcpipUdp.TCPIP_USE_UDP"])

	# NetBIOS Task Rate in ms
	tcpipNbnsTskTickRate = tcpipNbnsComponent.createIntegerSymbol("TCPIP_NBNS_TASK_TICK_RATE", tcpipNbns)
	tcpipNbnsTskTickRate.setLabel("NetBIOS Task Rate - ms")
	tcpipNbnsTskTickRate.setVisible(True)
	tcpipNbnsTskTickRate.setDescription("NetBIOS Task Rate in ms")
	tcpipNbnsTskTickRate.setDefaultValue(110)
	tcpipNbnsTskTickRate.setDependencies(tcpipNbnsMenuVisibleSingle, ["TCPIP_USE_NBNS"])

	# Add nbns.h file to project
	tcpipUdpHeaderFile = tcpipNbnsComponent.createFileSymbol(None, None)
	tcpipUdpHeaderFile.setSourcePath("library/nbns.h")
	tcpipUdpHeaderFile.setOutputName("nbns.h")
	tcpipUdpHeaderFile.setDestPath("library/tcpip/")
	tcpipUdpHeaderFile.setProjectPath("config/" + configName + "library/tcpip/")
	tcpipUdpHeaderFile.setType("HEADER")
	tcpipUdpHeaderFile.setOverwrite(True)

	# Add nbns_manager.h file to project
	tcpipUdpManagerHeaderFile = tcpipNbnsComponent.createFileSymbol(None, None)
	tcpipUdpManagerHeaderFile.setSourcePath("library/src/nbns_manager.h")
	tcpipUdpManagerHeaderFile.setOutputName("nbns_manager.h")
	tcpipUdpManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipUdpManagerHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipUdpManagerHeaderFile.setType("HEADER")
	tcpipUdpManagerHeaderFile.setOverwrite(True)

	# Add nbns.c file
	tcpipUdpSourceFile = tcpipNbnsComponent.createFileSymbol(None, None)
	tcpipUdpSourceFile.setSourcePath("library/src/nbns.c")
	tcpipUdpSourceFile.setOutputName("nbns.c")
	tcpipUdpSourceFile.setOverwrite(True)
	tcpipUdpSourceFile.setDestPath("library/tcpip/src/")
	tcpipUdpSourceFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipUdpSourceFile.setType("SOURCE")
	tcpipUdpSourceFile.setEnabled(True)
	tcpipUdpSourceFile.setDependencies(tcpipNbnsGenSourceFile, ["TCPIP_USE_NBNS"])

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