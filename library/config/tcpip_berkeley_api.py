    
def instantiateComponent(tcpipBerkeleyApiComponent):
	print("TCPIP UDP Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	
	# Enable Berkeley API
	tcpipBerkeleyApi = tcpipBerkeleyApiComponent.createBooleanSymbol("TCPIP_STACK_USE_BERKELEY_API", None)
	tcpipBerkeleyApi.setLabel("Berkeley API")
	tcpipBerkeleyApi.setVisible(True)
	tcpipBerkeleyApi.setDescription("Enable Berkeley API")
	tcpipBerkeleyApi.setDefaultValue(False)
	#Ruby to check with Niyas,select NET_PRES_NEEDED

	# Maximum Number of Simultaneous Sockets Supported
	tcpipBerkeleyBsdSktsMaxNum = tcpipBerkeleyApiComponent.createIntegerSymbol("TCPIP_BSD_MAX_BSD_SOCKETS", tcpipBerkeleyApi)
	tcpipBerkeleyBsdSktsMaxNum.setLabel("Max Number of Simultaneous Sockets Supported")
	tcpipBerkeleyBsdSktsMaxNum.setVisible(False)
	tcpipBerkeleyBsdSktsMaxNum.setDescription("Maximum Number of Simultaneous Sockets Supported")
	tcpipBerkeleyBsdSktsMaxNum.setDefaultValue(4)
	tcpipBerkeleyBsdSktsMaxNum.setDependencies(tcpipBerkeleyApiMenuVisible, ["TCPIP_STACK_USE_BERKELEY_API"])

	# Add berkeley_api.h file to project
	tcpipBerkeleyApiHeaderFile = tcpipBerkeleyApiComponent.createFileSymbol(None, None)
	tcpipBerkeleyApiHeaderFile.setSourcePath("library/berkeley_api.h")
	tcpipBerkeleyApiHeaderFile.setOutputName("berkeley_api.h")
	tcpipBerkeleyApiHeaderFile.setDestPath("library/tcpip/")
	tcpipBerkeleyApiHeaderFile.setProjectPath("config/" + configName + "library/tcpip/")
	tcpipBerkeleyApiHeaderFile.setType("HEADER")
	tcpipBerkeleyApiHeaderFile.setOverwrite(True)

	# Add berkeley_api.c file
	tcpipBerkeleyApiSourceFile = tcpipBerkeleyApiComponent.createFileSymbol(None, None)
	tcpipBerkeleyApiSourceFile.setSourcePath("library/src/berkeley_api.c")
	tcpipBerkeleyApiSourceFile.setOutputName("berkeley_api.c")
	tcpipBerkeleyApiSourceFile.setOverwrite(True)
	tcpipBerkeleyApiSourceFile.setDestPath("library/tcpip/src/")
	tcpipBerkeleyApiSourceFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipBerkeleyApiSourceFile.setType("SOURCE")
	tcpipBerkeleyApiSourceFile.setEnabled(True)
	tcpipBerkeleyApiSourceFile.setDependencies(tcpipBerkeleyApiGenSourceFile, ["TCPIP_STACK_USE_BERKELEY_API"])
	
	
def tcpipBerkeleyApiMenuVisible(symbol, event):
	if (event["value"] == True):
		print("BerkeleyApi Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("BerkeleyApi Menu Invisible.")
		symbol.setVisible(False)	
		
def tcpipBerkeleyApiGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])