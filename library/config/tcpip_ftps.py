    
def instantiateComponent(tcpipFtpsComponent):
	print("TCPIP FTP Server Component")
	configName = Variables.get("__CONFIGURATION_NAME")
		
	# Use FTP Module
	tcpipFtpsModule = tcpipFtpsComponent.createBooleanSymbol("TCPIP_USE_FTP_MODULE", None)
	tcpipFtpsModule.setLabel("Use FTP Module")
	tcpipFtpsModule.setVisible(True)
	tcpipFtpsModule.setDescription("Use FTP Module")
	tcpipFtpsModule.setDefaultValue(False)
	tcpipFtpsModule.setDependencies(tcpipFtpsModuleMenuVisible, ["tcpipIPv4.TCPIP_STACK_USE_IPV4", "tcpipTcp.TCPIP_USE_TCP"])
	# niyas to do  select USE_SYS_FS_NEEDED

	# Maximum Length for User Name
	tcpipFtpsUsrNameMaxLen = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTP_USER_NAME_LEN", tcpipFtpsModule)
	tcpipFtpsUsrNameMaxLen.setLabel("Maximum Length for User Name")
	tcpipFtpsUsrNameMaxLen.setVisible(False)
	tcpipFtpsUsrNameMaxLen.setDescription("Maximum Length for User Name")
	tcpipFtpsUsrNameMaxLen.setDefaultValue(10)
	tcpipFtpsUsrNameMaxLen.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])

	# Maximum Length of FTP Login Password
	tcpipFtpsLoginPswdMaxLen = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTP_PASSWD_LEN", tcpipFtpsModule)
	tcpipFtpsLoginPswdMaxLen.setLabel("Maximum Length of FTP Login Password")
	tcpipFtpsLoginPswdMaxLen.setVisible(False)
	tcpipFtpsLoginPswdMaxLen.setDescription("Maximum Length of FTP Login Password")
	tcpipFtpsLoginPswdMaxLen.setDefaultValue(10)
	tcpipFtpsLoginPswdMaxLen.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])

	# Maximum Number of FTP Connections Allowed per Interface
	tcpipFtpsConnMaxNum = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTP_MAX_CONNECTIONS", tcpipFtpsModule)
	tcpipFtpsConnMaxNum.setLabel("Maximum Number of FTP Connections Allowed per Interface")
	tcpipFtpsConnMaxNum.setVisible(False)
	tcpipFtpsConnMaxNum.setDescription("Maximum Number of FTP Connections Allowed per Interface")
	tcpipFtpsConnMaxNum.setDefaultValue(1)
	tcpipFtpsConnMaxNum.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])

	# Transmit Buffer Size for the FTP Data Socket
	tcpipFtpsDataSktTxBuffSize = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTP_DATA_SKT_TX_BUFF_SIZE", tcpipFtpsModule)
	tcpipFtpsDataSktTxBuffSize.setLabel("TX Buffer for the FTP Data Socket")
	tcpipFtpsDataSktTxBuffSize.setVisible(False)
	tcpipFtpsDataSktTxBuffSize.setDescription("Transmit Buffer Size for the FTP Data Socket")
	tcpipFtpsDataSktTxBuffSize.setDefaultValue(0)
	tcpipFtpsDataSktTxBuffSize.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])

	# Receive Buffer Size for the FTP Data Socket
	tcpipFtpsDataSktRxBuffSize = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTP_DATA_SKT_RX_BUFF_SIZE", tcpipFtpsModule)
	tcpipFtpsDataSktRxBuffSize.setLabel("RX Buffer for the FTP Data Socket")
	tcpipFtpsDataSktRxBuffSize.setVisible(False)
	tcpipFtpsDataSktRxBuffSize.setDescription("Receive Buffer Size for the FTP Data Socket")
	tcpipFtpsDataSktRxBuffSize.setDefaultValue(0)
	tcpipFtpsDataSktRxBuffSize.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])

	# FTP Server Task Rate in msec
	tcpipFtpsTskTickRate = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTPS_TASK_TICK_RATE", tcpipFtpsModule)
	tcpipFtpsTskTickRate.setLabel("FTP Server Task Rate in msec")
	tcpipFtpsTskTickRate.setVisible(False)
	tcpipFtpsTskTickRate.setDescription("FTP Server Task Rate in msec")
	tcpipFtpsTskTickRate.setDefaultValue(33)
	tcpipFtpsTskTickRate.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])

	# FTP Server timeout in seconds
	tcpipFtpsTimeout = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTP_TIMEOUT", tcpipFtpsModule)
	tcpipFtpsTimeout.setLabel("FTP Server timeout in sec")
	tcpipFtpsTimeout.setVisible(False)
	tcpipFtpsTimeout.setDescription("FTP Server timeout in seconds")
	tcpipFtpsTimeout.setDefaultValue(180)
	tcpipFtpsTimeout.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])


	# FTP Login User Name
	tcpipFtpsLoginUsrName = tcpipFtpsComponent.createStringSymbol("TCPIP_FTP_USER_NAME", tcpipFtpsModule)
	tcpipFtpsLoginUsrName.setLabel("FTP Login User Name")
	tcpipFtpsLoginUsrName.setVisible(False)
	tcpipFtpsLoginUsrName.setDescription("FTP Login User Name")
	tcpipFtpsLoginUsrName.setDefaultValue("Microchip")
	tcpipFtpsLoginUsrName.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])

	# FTP Login Password
	tcpipFtpsLoginUsrName = tcpipFtpsComponent.createStringSymbol("TCPIP_FTP_PASSWORD", tcpipFtpsModule)
	tcpipFtpsLoginUsrName.setLabel("FTP Login Password")
	tcpipFtpsLoginUsrName.setVisible(False)
	tcpipFtpsLoginUsrName.setDescription("FTP Login Password")
	tcpipFtpsLoginUsrName.setDefaultValue("Harmony")
	tcpipFtpsLoginUsrName.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])

	# Enable FTP File PUT Command
	tcpipFtpsFilePut = tcpipFtpsComponent.createBooleanSymbol("TCPIP_FTP_PUT_ENABLED", tcpipFtpsModule)
	tcpipFtpsFilePut.setLabel("FTP File PUT Command")
	tcpipFtpsFilePut.setVisible(False)
	tcpipFtpsFilePut.setDescription("Enable FTP File PUT Command")
	tcpipFtpsFilePut.setDefaultValue(True)
	tcpipFtpsFilePut.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])

	# Add ftp.h file to project
	tcpipFtpsHeaderFile = tcpipFtpsComponent.createFileSymbol(None, None)
	tcpipFtpsHeaderFile.setSourcePath("library/ftp.h")
	tcpipFtpsHeaderFile.setOutputName("ftp.h")
	tcpipFtpsHeaderFile.setDestPath("library/tcpip/")
	tcpipFtpsHeaderFile.setProjectPath("config/" + configName + "library/tcpip/")
	tcpipFtpsHeaderFile.setType("HEADER")
	tcpipFtpsHeaderFile.setOverwrite(True)

	# Add ftp_manager.h file to project
	tcpipFtpsManagerHeaderFile = tcpipFtpsComponent.createFileSymbol(None, None)
	tcpipFtpsManagerHeaderFile.setSourcePath("library/src/ftp_manager.h")
	tcpipFtpsManagerHeaderFile.setOutputName("ftp_manager.h")
	tcpipFtpsManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipFtpsManagerHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipFtpsManagerHeaderFile.setType("HEADER")
	tcpipFtpsManagerHeaderFile.setOverwrite(True)

	# Add ftp_private.h file to project
	tcpipFtpsPrivateHeaderFile = tcpipFtpsComponent.createFileSymbol(None, None)
	tcpipFtpsPrivateHeaderFile.setSourcePath("library/src/ftp_private.h")
	tcpipFtpsPrivateHeaderFile.setOutputName("ftp_private.h")
	tcpipFtpsPrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipFtpsPrivateHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipFtpsPrivateHeaderFile.setType("HEADER")
	tcpipFtpsPrivateHeaderFile.setOverwrite(True)

	# Add ftp.c file
	tcpipFtpsSourceFile = tcpipFtpsComponent.createFileSymbol(None, None)
	tcpipFtpsSourceFile.setSourcePath("library/src/ftp.c")
	tcpipFtpsSourceFile.setOutputName("ftp.c")
	tcpipFtpsSourceFile.setOverwrite(True)
	tcpipFtpsSourceFile.setDestPath("library/tcpip/src/")
	tcpipFtpsSourceFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipFtpsSourceFile.setType("SOURCE")
	tcpipFtpsSourceFile.setEnabled(True)
	tcpipFtpsSourceFile.setDependencies(tcpipFtpsGenSourceFile, ["TCPIP_USE_FTP_MODULE"])

	
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