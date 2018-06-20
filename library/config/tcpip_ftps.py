    
def instantiateComponent(tcpipFtpsComponent):
	print("TCPIP FTP Server Component")
	configName = Variables.get("__CONFIGURATION_NAME")
		
	# Use FTP Module
	tcpipFtpsModule = tcpipFtpsComponent.createBooleanSymbol("TCPIP_USE_FTP_MODULE", None)
	tcpipFtpsModule.setLabel("Use FTP Module")
	tcpipFtpsModule.setVisible(False)
	tcpipFtpsModule.setDescription("Use FTP Module")
	tcpipFtpsModule.setDefaultValue(True)
	#tcpipFtpsModule.setDependencies(tcpipFtpsModuleMenuVisible, ["tcpipIPv4.TCPIP_STACK_USE_IPV4", "tcpipTcp.TCPIP_USE_TCP"])
	# H3_ToDo  select USE_SYS_FS_NEEDED

	# Maximum Length for User Name
	tcpipFtpsUsrNameMaxLen = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTP_USER_NAME_LEN", None)
	tcpipFtpsUsrNameMaxLen.setLabel("Maximum Length for User Name")
	tcpipFtpsUsrNameMaxLen.setVisible(True)
	tcpipFtpsUsrNameMaxLen.setDescription("Maximum Length for User Name")
	tcpipFtpsUsrNameMaxLen.setDefaultValue(10)
	#tcpipFtpsUsrNameMaxLen.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])

	# Maximum Length of FTP Login Password
	tcpipFtpsLoginPswdMaxLen = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTP_PASSWD_LEN", None)
	tcpipFtpsLoginPswdMaxLen.setLabel("Maximum Length of FTP Login Password")
	tcpipFtpsLoginPswdMaxLen.setVisible(True)
	tcpipFtpsLoginPswdMaxLen.setDescription("Maximum Length of FTP Login Password")
	tcpipFtpsLoginPswdMaxLen.setDefaultValue(10)
	#tcpipFtpsLoginPswdMaxLen.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])

	# Maximum Number of FTP Connections Allowed per Interface
	tcpipFtpsConnMaxNum = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTP_MAX_CONNECTIONS", None)
	tcpipFtpsConnMaxNum.setLabel("Maximum Number of FTP Connections Allowed per Interface")
	tcpipFtpsConnMaxNum.setVisible(True)
	tcpipFtpsConnMaxNum.setDescription("Maximum Number of FTP Connections Allowed per Interface")
	tcpipFtpsConnMaxNum.setDefaultValue(1)
	#tcpipFtpsConnMaxNum.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])

	# Transmit Buffer Size for the FTP Data Socket
	tcpipFtpsDataSktTxBuffSize = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTP_DATA_SKT_TX_BUFF_SIZE", None)
	tcpipFtpsDataSktTxBuffSize.setLabel("TX Buffer for the FTP Data Socket")
	tcpipFtpsDataSktTxBuffSize.setVisible(True)
	tcpipFtpsDataSktTxBuffSize.setDescription("Transmit Buffer Size for the FTP Data Socket")
	tcpipFtpsDataSktTxBuffSize.setDefaultValue(0)
	#tcpipFtpsDataSktTxBuffSize.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])

	# Receive Buffer Size for the FTP Data Socket
	tcpipFtpsDataSktRxBuffSize = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTP_DATA_SKT_RX_BUFF_SIZE", None)
	tcpipFtpsDataSktRxBuffSize.setLabel("RX Buffer for the FTP Data Socket")
	tcpipFtpsDataSktRxBuffSize.setVisible(True)
	tcpipFtpsDataSktRxBuffSize.setDescription("Receive Buffer Size for the FTP Data Socket")
	tcpipFtpsDataSktRxBuffSize.setDefaultValue(0)
	#tcpipFtpsDataSktRxBuffSize.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])

	# FTP Server Task Rate in msec
	tcpipFtpsTskTickRate = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTPS_TASK_TICK_RATE", None)
	tcpipFtpsTskTickRate.setLabel("FTP Server Task Rate in msec")
	tcpipFtpsTskTickRate.setVisible(True)
	tcpipFtpsTskTickRate.setDescription("FTP Server Task Rate in msec")
	tcpipFtpsTskTickRate.setDefaultValue(33)
	#tcpipFtpsTskTickRate.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])

	# FTP Server timeout in seconds
	tcpipFtpsTimeout = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTP_TIMEOUT", None)
	tcpipFtpsTimeout.setLabel("FTP Server timeout in sec")
	tcpipFtpsTimeout.setVisible(True)
	tcpipFtpsTimeout.setDescription("FTP Server timeout in seconds")
	tcpipFtpsTimeout.setDefaultValue(180)
	#tcpipFtpsTimeout.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])


	# FTP Login User Name
	tcpipFtpsLoginUsrName = tcpipFtpsComponent.createStringSymbol("TCPIP_FTP_USER_NAME", None)
	tcpipFtpsLoginUsrName.setLabel("FTP Login User Name")
	tcpipFtpsLoginUsrName.setVisible(True)
	tcpipFtpsLoginUsrName.setDescription("FTP Login User Name")
	tcpipFtpsLoginUsrName.setDefaultValue("Microchip")
	#tcpipFtpsLoginUsrName.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])

	# FTP Login Password
	tcpipFtpsLoginUsrName = tcpipFtpsComponent.createStringSymbol("TCPIP_FTP_PASSWORD", None)
	tcpipFtpsLoginUsrName.setLabel("FTP Login Password")
	tcpipFtpsLoginUsrName.setVisible(True)
	tcpipFtpsLoginUsrName.setDescription("FTP Login Password")
	tcpipFtpsLoginUsrName.setDefaultValue("Harmony")
	#tcpipFtpsLoginUsrName.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])

	# Enable FTP File PUT Command
	tcpipFtpsFilePut = tcpipFtpsComponent.createBooleanSymbol("TCPIP_FTP_PUT_ENABLED", None)
	tcpipFtpsFilePut.setLabel("FTP File PUT Command")
	tcpipFtpsFilePut.setVisible(True)
	tcpipFtpsFilePut.setDescription("Enable FTP File PUT Command")
	tcpipFtpsFilePut.setDefaultValue(True)
	#tcpipFtpsFilePut.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])

	#Add to system_config.h
	tcpipFtpsHeaderFtl = tcpipFtpsComponent.createFileSymbol(None, None)
	tcpipFtpsHeaderFtl.setSourcePath("library/config/ftp.h.ftl")
	tcpipFtpsHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipFtpsHeaderFtl.setMarkup(True)
	tcpipFtpsHeaderFtl.setType("STRING")	

	# Add ftp.c file
	tcpipFtpsSourceFile = tcpipFtpsComponent.createFileSymbol(None, None)
	tcpipFtpsSourceFile.setSourcePath("library/src/ftp.c")
	tcpipFtpsSourceFile.setOutputName("ftp.c")
	tcpipFtpsSourceFile.setOverwrite(True)
	tcpipFtpsSourceFile.setDestPath("library/tcpip/src/")
	tcpipFtpsSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipFtpsSourceFile.setType("SOURCE")
	tcpipFtpsSourceFile.setEnabled(True)
	#tcpipFtpsSourceFile.setDependencies(tcpipFtpsGenSourceFile, ["TCPIP_USE_FTP_MODULE"])

	
def tcpipFtpsMenuVisible(symbol, event):
	if (event["value"] == True):
		print("FTP server Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("FTP Server Menu Invisible.")
		symbol.setVisible(False)	
		
# make FTP Module option visible
def tcpipFtpsModuleMenuVisible(tcpipDependentSymbol, tcpipIPSymbol):
	tcpipIPv4 = Database.getSymbolValue("tcpipIPv4","TCPIP_STACK_USE_IPV4")
	tcpipTcp = Database.getSymbolValue("tcpipTcp","TCPIP_USE_TCP")

	if(tcpipIPv4 and tcpipTcp):
		tcpipDependentSymbol.setVisible(True)
	else:
		tcpipDependentSymbol.setVisible(False)
		
def tcpipFtpsGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])