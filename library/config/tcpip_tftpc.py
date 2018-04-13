#niyas multiple definition for following; find a method
TCPIP_STACK_IF_NAME = []
#niyas modify the interface name
TCPIP_STACK_PIC32C_IF_NAME =	["PIC32CINT", 	"ENCX24J600", 	"ENC28J60", 	"MRF24WN", 		"WINC1500", 	"WILC1000" ]
TCPIP_STACK_PIC32M_IF_NAME =	["PIC32INT", 	"ENCX24J600", 	"ENC28J60", 	"MRF24WN", 		"WINC1500", 	"WILC1000" ]    
def instantiateComponent(tcpipTftpcComponent):
	print("TCPIP TFTPC Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	
	if "SAME70" in Variables.get("__PROCESSOR"):
		TCPIP_STACK_IF_NAME = TCPIP_STACK_PIC32C_IF_NAME
	else:
		TCPIP_STACK_IF_NAME = TCPIP_STACK_PIC32M_IF_NAME
		
	# Use TFTP Client Module
	tcpipTftpc = tcpipTftpcComponent.createBooleanSymbol("TCPIP_USE_TFTPC_MODULE", None)
	tcpipTftpc.setLabel("Use TFTP Client Module")
	tcpipTftpc.setVisible(True)
	tcpipTftpc.setDescription("Use TFTP Client Module")
	tcpipTftpc.setDefaultValue(False)
	# select USE_SYS_FS_NEEDED   # Niyas to do
	tcpipTftpc.setDependencies(tcpipTftpcModuleVisible, ["tcpipIPv4.TCPIP_STACK_USE_IPV4", "tcpipUdp.TCPIP_USE_UDP"])

	# Default Interface
	tcpipTftpcDefault = tcpipTftpcComponent.createComboSymbol("TCPIP_TFTPC_DEFAULT_IF", tcpipTftpc, TCPIP_STACK_IF_NAME)
	tcpipTftpcDefault.setLabel("Default Interface")
	tcpipTftpcDefault.setVisible(False)
	tcpipTftpcDefault.setDescription("Default Interface")
	tcpipTftpcDefault.setDefaultValue("PIC32INT")
	# Niyas to do
	# default "PIC32INT" if HAVE_ETH
	# default "MRF24WN" if HAVE_WIFI
	tcpipTftpcDefault.setDependencies(tcpipTftpcMenuVisibleSingle, ["TCPIP_USE_TFTPC_MODULE"])

	# Maximum Length for Server Address
	tcpipTftpcSrvrAddrLen= tcpipTftpcComponent.createIntegerSymbol("TCPIP_TFTPC_SERVERADDRESS_LEN", tcpipTftpc)
	tcpipTftpcSrvrAddrLen.setLabel("Maximum Length for Server Address")
	tcpipTftpcSrvrAddrLen.setVisible(False)
	tcpipTftpcSrvrAddrLen.setDescription("Maximum Length for Server Address")
	tcpipTftpcSrvrAddrLen.setDefaultValue(16)
	tcpipTftpcSrvrAddrLen.setDependencies(tcpipTftpcMenuVisibleSingle, ["TCPIP_USE_TFTPC_MODULE"])

	# Maximum Length for a file name
	tcpipTftpcFilenameLen= tcpipTftpcComponent.createIntegerSymbol("TCPIP_TFTPC_FILENAME_LEN", tcpipTftpc)
	tcpipTftpcFilenameLen.setLabel("Maximum Length for a file name")
	tcpipTftpcFilenameLen.setVisible(False)
	tcpipTftpcFilenameLen.setDescription("Maximum Length for a file name")
	tcpipTftpcFilenameLen.setDefaultValue(32)
	tcpipTftpcFilenameLen.setDependencies(tcpipTftpcMenuVisibleSingle, ["TCPIP_USE_TFTPC_MODULE"])

	# Enable User Notification
	tcpipTftpcUsrNotify = tcpipTftpcComponent.createBooleanSymbol("TCPIP_TFTPC_USER_NOTIFICATION", tcpipTftpc)
	tcpipTftpcUsrNotify.setLabel("Enable User Notification")
	tcpipTftpcUsrNotify.setVisible(False)
	tcpipTftpcUsrNotify.setDescription("Enable User Notification")
	tcpipTftpcUsrNotify.setDefaultValue(False) 
	tcpipTftpcUsrNotify.setDependencies(tcpipTftpcMenuVisibleSingle, ["TCPIP_USE_TFTPC_MODULE"])

	# TFTP Client Task Rate in msec
	tcpipTftpcTskTickRate= tcpipTftpcComponent.createIntegerSymbol("TCPIP_TFTPC_TASK_TICK_RATE", tcpipTftpc)
	tcpipTftpcTskTickRate.setLabel("TFTP Client Task Rate in msec")
	tcpipTftpcTskTickRate.setVisible(False)
	tcpipTftpcTskTickRate.setDescription("TFTP Client Task Rate in msec")
	tcpipTftpcTskTickRate.setDefaultValue(100)
	tcpipTftpcTskTickRate.setDependencies(tcpipTftpcMenuVisibleSingle, ["TCPIP_USE_TFTPC_MODULE"])

	# TFTP Client Socket connection timeout in sec
	tcpipTftpcArpTimeout= tcpipTftpcComponent.createIntegerSymbol("TCPIP_TFTPC_ARP_TIMEOUT", tcpipTftpc)
	tcpipTftpcArpTimeout.setLabel("TFTP Client Socket connection timeout in sec")
	tcpipTftpcArpTimeout.setVisible(False)
	tcpipTftpcArpTimeout.setDescription("TFTP Client Socket connection timeout in sec")
	tcpipTftpcArpTimeout.setDefaultValue(3)
	tcpipTftpcArpTimeout.setDependencies(tcpipTftpcMenuVisibleSingle, ["TCPIP_USE_TFTPC_MODULE"])

	# TFTP Client command process timeout in sec
	tcpipTftpcCmdProcessTimeout= tcpipTftpcComponent.createIntegerSymbol("TCPIP_TFTPC_CMD_PROCESS_TIMEOUT", tcpipTftpc)
	tcpipTftpcCmdProcessTimeout.setLabel("TFTP Client command process timeout in sec")
	tcpipTftpcCmdProcessTimeout.setVisible(False)
	tcpipTftpcCmdProcessTimeout.setDescription("TFTP Client command process timeout in sec")
	tcpipTftpcCmdProcessTimeout.setDefaultValue(3)
	tcpipTftpcCmdProcessTimeout.setDependencies(tcpipTftpcMenuVisibleSingle, ["TCPIP_USE_TFTPC_MODULE"])

	# TFTP Client Maximum retries
	tcpipTftpcRetryMax= tcpipTftpcComponent.createIntegerSymbol("TCPIP_TFTPC_MAX_RETRIES", tcpipTftpc)
	tcpipTftpcRetryMax.setLabel("TFTP Client Maximum retries")
	tcpipTftpcRetryMax.setVisible(False)
	tcpipTftpcRetryMax.setDescription("TFTP Client Maximum retries")
	tcpipTftpcRetryMax.setDefaultValue(3)
	tcpipTftpcRetryMax.setDependencies(tcpipTftpcMenuVisibleSingle, ["TCPIP_USE_TFTPC_MODULE"])

	#Add to system_config.h
	tcpipTftpcHeaderFtl = tcpipTftpcComponent.createFileSymbol(None, None)
	tcpipTftpcHeaderFtl.setSourcePath("library/config/tftpc.h.ftl")
	tcpipTftpcHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipTftpcHeaderFtl.setMarkup(True)
	tcpipTftpcHeaderFtl.setType("STRING")

	# Add tftpc.c file
	tcpipTftpcSourceFile = tcpipTftpcComponent.createFileSymbol(None, None)
	tcpipTftpcSourceFile.setSourcePath("library/src/tftpc.c")
	tcpipTftpcSourceFile.setOutputName("tftpc.c")
	tcpipTftpcSourceFile.setOverwrite(True)
	tcpipTftpcSourceFile.setDestPath("library/tcpip/src/")
	tcpipTftpcSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipTftpcSourceFile.setType("SOURCE")
	tcpipTftpcSourceFile.setEnabled(True)
	tcpipTftpcSourceFile.setDependencies(tcpipTftpcGenSourceFile, ["TCPIP_USE_TFTPC_MODULE"])

# make Reboot Server option visible
def tcpipTftpcModuleVisible(tcpipDependentSymbol, tcpipIPSymbol):
	tcpipIPv4 = Database.getSymbolValue("tcpipIPv4","TCPIP_STACK_USE_IPV4")
	tcpipUdp = Database.getSymbolValue("tcpipUdp","TCPIP_USE_UDP")

	if(tcpipIPv4 and tcpipUdp):
		tcpipDependentSymbol.setVisible(True)
	else:
		tcpipDependentSymbol.setVisible(False)
		
def tcpipTftpcMenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("TFTPC Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("TFTPC Menu Invisible.")
		symbol.setVisible(False)

def tcpipTftpcGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])