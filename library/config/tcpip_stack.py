__TCPIP_DIR = "D:/06_Repo/05_H3/h3/net"


################## DHCP Server Instances ##############################
tcpipDhcpsInstancesNumPrev = 1
tcpipDhcpsInstance = []
tcpipDhcpsAddrRangeStart = []
############################################################################

#TCPIP_STACK_IF_NAME =	["PIC32INT", 	"PIC32CINT", 	"ENCX24J600", 	"ENC28J60", 	"MRF24WN", 		"WINC1500", 	"WILC1000" ]
#TCPIP_STACK_HOST_NAME = ["MCHPBOARD_E",	"MCHPBOARD_C",	"MCHPBOARD_E",	"MCHPENC_E",	"MCHPBOARD_W",	"MCHPBOARD_W",	"MCHPBOARD_W"]

#global tcpipNetConfigNum
#TCPIP_STACK_HOST_NAME =["PIC32INT", "PIC32CINT", "ENCX24J600", "ENC28J60", "MRF24WN", "WINC1500", "WILC1000" ]
    
def instantiateComponent(tcpipStackComponent):
	print("TCPIP main Stack Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	
	# TCP/IP Stack Enable 
	tcpipStackEnable = tcpipStackComponent.createBooleanSymbol("USE_TCPIP_STACK", None)
	tcpipStackEnable.setLabel("Use TCP/IP Stack?")
	tcpipStackEnable.setDescription("Enables TCP/IP stack")
	tcpipStackEnable.setReadOnly(True)
	tcpipStackEnable.setDefaultValue(True)
	tcpipStackEnable.setVisible(False)
	
	# Define number of network Configurations
	tcpipStackTickRate = tcpipStackComponent.createIntegerSymbol("TCPIP_STACK_TICK_RATE", None)
	tcpipStackTickRate.setLabel("TCP/IP Stack State Machine Tick Rate, ms.")
	tcpipStackTickRate.setVisible(True)
	tcpipStackTickRate.setDescription("TCP/IP Stack State Machine Tick Rate")
	tcpipStackTickRate.setDefaultValue(5)
	
	# Event Notification
	tcpipStackEventNotify = tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_EVENT_NOTIFICATION", None)
	tcpipStackEventNotify.setVisible(False)
	tcpipStackEventNotify.setDefaultValue(True)
	
	# Enable User Notification
	tcpipStackUserNotify= tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_USER_NOTIFICATION", None)
	tcpipStackUserNotify.setLabel("Enable User Notification")
	tcpipStackUserNotify.setVisible(True)
	tcpipStackUserNotify.setDescription("Enable User Notification")
	tcpipStackUserNotify.setDefaultValue(False)
	tcpipStackUserNotify.setDependencies(tcpipStackMenuVisible, ["TCPIP_STACK_EVENT_NOTIFICATION"])

	# Enable Deinitialize of TCP/IP stack
	tcpipStackDown= tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_DOWN_OPERATION", None)
	tcpipStackDown.setLabel("Enable Stack Deinitialize Operations")
	tcpipStackDown.setVisible(True)
	tcpipStackDown.setDescription("Enable Deinitialize of TCP/IP stack")
	tcpipStackDown.setDefaultValue(True)
	
	# Enable stack up/down of TCP/IP stack
	tcpipStackIfUpDown= tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_IF_UP_DOWN_OPERATION", tcpipStackDown)
	tcpipStackIfUpDown.setLabel("Enable IF Up/Down Operations")
	tcpipStackIfUpDown.setVisible(True)
	tcpipStackIfUpDown.setDescription("Enable interface up/down of TCP/IP stack")
	tcpipStackIfUpDown.setDefaultValue(True)
	tcpipStackIfUpDown.setDependencies(tcpipStackMenuVisible, ["TCPIP_STACK_DOWN_OPERATION"])

	# Enable stack Mac Down
	tcpipStackMacDown= tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_MAC_DOWN_OPERATION", None)
	tcpipStackMacDown.setLabel("Enable MAC Down on Failure")
	tcpipStackMacDown.setVisible(False)
	tcpipStackMacDown.setDescription("Enable MAC Down on Failure")
	tcpipStackMacDown.setDefaultValue(True)
	tcpipStackMacDown.setDependencies(tcpipStackMenuInvisible, ["TCPIP_STACK_DOWN_OPERATION"])

	# Enable Interface Address Change Signaling
	tcpipStackIfChange= tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_INTERFACE_CHANGE_SIGNALING", None)
	tcpipStackIfChange.setLabel("Enable Interface Address Change Signaling")
	tcpipStackIfChange.setVisible(True)
	tcpipStackIfChange.setDescription("Enable Interface Address Change Signaling")
	tcpipStackIfChange.setDefaultValue(False)
	tcpipStackIfChange.setDependencies(tcpipStackMenuVisible, ["TCPIP_STACK_USE_IPV4"])

	# Enable Configuration Save/Restore Functionality
	tcpipStackConfigSaveRestore= tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_CONFIGURATION_SAVE_RESTORE", None)
	tcpipStackConfigSaveRestore.setLabel("Enable Configuration Save/Restore Functionality")
	tcpipStackConfigSaveRestore.setVisible(True)
	tcpipStackConfigSaveRestore.setDescription("Enable Configuration Save/Restore Functionality")
	tcpipStackConfigSaveRestore.setDefaultValue(True)
	
		
	# Enable File System Wrapper
	tcpipStackSysFsWrapper = tcpipStackComponent.createBooleanSymbol("TCPIP_SYS_FS_WRAPPER_NEEDED", None)
	tcpipStackSysFsWrapper.setVisible(False)
	#niyas look how to implement these conditions
	#depends on TCPIP_USE_SNMP || TCPIP_STACK_USE_HTTP_SERVER || TCPIP_STACK_USE_HTTP_NET_SERVER || TCPIP_USE_FTP_MODULE || TCPIP_USE_TFTPC_MODULE
	#niyas if(USE_SYS_FS_NEEDED == False):
		#tcpipStackSysFsWrapper.setDefaultValue(False)
	#else:
		#tcpipStackSysFsWrapper.setDefaultValue(True)

	# Define number of entries in the secure port table
	tcpipStackSecPortNum = tcpipStackComponent.createIntegerSymbol("TCPIP_STACK_SECURE_PORT_ENTRIES", None)
	tcpipStackSecPortNum.setLabel("Number of entries in the secure port table")
	tcpipStackSecPortNum.setVisible(True)
	tcpipStackSecPortNum.setDescription("Number of entries in the secure port table")
	tcpipStackSecPortNum.setDefaultValue(10)

	# Enable Alias Interfaces Support
	tcpipStackAliasInterface= tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_ALIAS_INTERFACE_SUPPORT", None)
	tcpipStackAliasInterface.setLabel("Enable Alias Interfaces Support")
	tcpipStackAliasInterface.setVisible(True)
	tcpipStackAliasInterface.setDescription("Enable Alias Interfaces Support")
	tcpipStackAliasInterface.setDefaultValue(False)

	# Enable Packet Logger
	tcpipStackPktLogEnable= tcpipStackComponent.createBooleanSymbol("TCPIP_PACKET_LOG_ENABLE", None)
	tcpipStackPktLogEnable.setLabel("Enable Packet Logger")
	tcpipStackPktLogEnable.setVisible(True)
	tcpipStackPktLogEnable.setDescription("Enable Packet Logger")
	tcpipStackPktLogEnable.setDefaultValue(False)

	# Number of Entries in the Packet Logger
	tcpipStackPktLogSize= tcpipStackComponent.createIntegerSymbol("TCPIP_PKT_LOG_SIZE", tcpipStackPktLogEnable)
	tcpipStackPktLogSize.setLabel("Number of Entries in the Packet Logger")
	tcpipStackPktLogSize.setVisible(False)
	tcpipStackPktLogSize.setDescription("Number of Entries in the Packet Logger")
	tcpipStackPktLogSize.setDefaultValue(40)
	tcpipStackPktLogSize.setDependencies(tcpipStackMenuVisible, ["TCPIP_PACKET_LOG_ENABLE"])	
	
def tcpipStackMenuVisible(symbol, event):
	if (event["value"] == True):
		print("TCPIP Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("TCPIP Menu Invisible.")
		symbol.setVisible(False)
		
def tcpipStackMenuInvisible(symbol, event):
	if (event["value"] == False):
		print("TCPIP Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("TCPIP Menu Invisible.")
		symbol.setVisible(False)		
