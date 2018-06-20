    
def instantiateComponent(tcpipSmtpComponent):
	print("TCPIP SMTPC Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	
	# Use SMTP Client
	tcpipSmtpClient = tcpipSmtpComponent.createBooleanSymbol("TCPIP_USE_SMTP_CLIENT", None)
	tcpipSmtpClient.setLabel("Use SMTP")
	tcpipSmtpClient.setVisible(True)
	tcpipSmtpClient.setDescription("Use SMTP Client")
	tcpipSmtpClient.setDefaultValue(False) 
	# H3_ToDo   select NET_PRES_NEEDED
	tcpipSmtpClient.setDependencies(tcpipSmtpMenuVisibleSingle, ["tcpipTcp.TCPIP_USE_TCP"])

	# Server Reply Time-out
	tcpipSmtpServerReplyTimeout = tcpipSmtpComponent.createIntegerSymbol("TCPIP_SMTP_SERVER_REPLY_TIMEOUT", tcpipSmtpClient)
	tcpipSmtpServerReplyTimeout.setLabel("Server Reply Time-out")
	tcpipSmtpServerReplyTimeout.setVisible(False)
	tcpipSmtpServerReplyTimeout.setDescription("Server Reply Time-out")
	tcpipSmtpServerReplyTimeout.setDefaultValue(8)
	tcpipSmtpServerReplyTimeout.setDependencies(tcpipSmtpMenuVisibleSingle, ["TCPIP_USE_SMTP_CLIENT"])

	# Minimum Amount of Data to Ask from the Transport/Encryption Layer
	tcpipSmtpWriteReadySpace = tcpipSmtpComponent.createIntegerSymbol("TCPIP_SMTP_WRITE_READY_SPACE", tcpipSmtpClient)
	tcpipSmtpWriteReadySpace.setLabel("Minimum Amount of Data to Ask from the Transport/Encryption Layer")
	tcpipSmtpWriteReadySpace.setVisible(False)
	tcpipSmtpWriteReadySpace.setDescription("Minimum Amount of Data to Ask from the Transport/Encryption Layer")
	tcpipSmtpWriteReadySpace.setDefaultValue(150)
	tcpipSmtpWriteReadySpace.setDependencies(tcpipSmtpMenuVisibleSingle, ["TCPIP_USE_SMTP_CLIENT"])

	# Maximum size of data to be written in a discrete string/array email write
	tcpipSmtpWriteSizeMax = tcpipSmtpComponent.createIntegerSymbol("TCPIP_SMTP_MAX_WRITE_SIZE", tcpipSmtpClient)
	tcpipSmtpWriteSizeMax.setLabel("Maximum size of data to be written in a discrete string/array email write")
	tcpipSmtpWriteSizeMax.setVisible(False)
	tcpipSmtpWriteSizeMax.setDescription("Maximum size of data to be written in a discrete string/array email write")
	tcpipSmtpWriteSizeMax.setDefaultValue(512)
	tcpipSmtpWriteSizeMax.setDependencies(tcpipSmtpMenuVisibleSingle, ["TCPIP_USE_SMTP_CLIENT"])

	# SMTP Task Tick Rate in ms
	tcpipSmtpTskTickRate = tcpipSmtpComponent.createIntegerSymbol("TCPIP_SMTP_TASK_TICK_RATE", tcpipSmtpClient)
	tcpipSmtpTskTickRate.setLabel("SMTP Task Tick Rate in ms")
	tcpipSmtpTskTickRate.setVisible(False)
	tcpipSmtpTskTickRate.setDescription("SMTP Task Tick Rate in ms")
	tcpipSmtpTskTickRate.setDefaultValue(55)
	tcpipSmtpTskTickRate.setDependencies(tcpipSmtpMenuVisibleSingle, ["TCPIP_USE_SMTP_CLIENT"])

	#Add to system_config.h
	tcpipSmtpHeaderFtl = tcpipSmtpComponent.createFileSymbol(None, None)
	tcpipSmtpHeaderFtl.setSourcePath("library/config/smtp.h.ftl")
	tcpipSmtpHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipSmtpHeaderFtl.setMarkup(True)
	tcpipSmtpHeaderFtl.setType("STRING")

	# Add smtp.c file
	tcpipSmtpSourceFile = tcpipSmtpComponent.createFileSymbol(None, None)
	tcpipSmtpSourceFile.setSourcePath("library/src/smtp.c")
	tcpipSmtpSourceFile.setOutputName("smtp.c")
	tcpipSmtpSourceFile.setOverwrite(True)
	tcpipSmtpSourceFile.setDestPath("library/tcpip/src/")
	tcpipSmtpSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipSmtpSourceFile.setType("SOURCE")
	tcpipSmtpSourceFile.setEnabled(True)
	tcpipSmtpSourceFile.setDependencies(tcpipSmtpGenSourceFile, ["TCPIP_USE_SMTP_CLIENT"])

def tcpipSmtpMenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("SMTP Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("SMTP Menu Invisible.")
		symbol.setVisible(False)

def tcpipSmtpGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])