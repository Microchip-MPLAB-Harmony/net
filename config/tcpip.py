__TCPIP_DIR = "D:/06_Repo/05_H3/h3/net"
############### Network Configuration ######################
tcpipNetConfigNumMax = 10
tcpipNetConfig = []
tcpipNetIfName = []
tcpipNetHostName = []
tcpipNetMacAddress = []
tcpipNetIpAddress = []
tcpipNetIpAddrMask = []
tcpipNetGatewayIpAddr = []
tcpipNetPrimDnsIpAddr = []
tcpipNetSecDnsIpAddr = []
tcpipNetPwrMode = []
tcpipNetStartupFlag = []
tcpipNetDhcpClientEnable = []
tcpipNetZConfLLEnable = []
tcpipNetDhcpServerEnable = []
tcpipNetDnsClientEnable = []
tcpipNetDnsServerEnable = []
tcpipNetMcastEnable = []
tcpipNetIpv6AddrSubnetInput = []
tcpipNetIpv6StatAddr = []
tcpipNetIpv6StatAddrPrfxLen = []
tcpipNetIpv6GatewayAddr = []
tcpipNetMacDrvObj = []
tcpipNetConfigNumPrev = 1

################## DHCP Server Instances ##############################
tcpipDhcpsInstancesNumPrev = 1
tcpipDhcpsInstance = []
tcpipDhcpsAddrRangeStart = []
############################################################################



#global tcpipNetConfigNum
#TCPIP_STACK_HOST_NAME =["PIC32INT", "PIC32CINT", "ENCX24J600", "ENC28J60", "MRF24WN", "WINC1500", "WILC1000" ]
    
def instantiateComponent(tcpipComponent):
	# TCP/IP Stack Enable 
	tcpipStackEnable = tcpipComponent.createBooleanSymbol("USE_TCPIP_STACK", None)
	tcpipStackEnable.setLabel("Use TCP/IP Stack?")
	tcpipStackEnable.setDescription("Enables TCP/IP stack")
	tcpipStackEnable.setReadOnly(True)
	tcpipStackEnable.setDefaultValue(True)
	
	configName = Variables.get("__CONFIGURATION_NAME")
	
	# RTOS options
	execfile("D:/06_Repo/05_H3/h3/tcpip/library/config/tcpip_rtos.py")
		
	# IPv4 options
	# niyas execfile(Variables.get("__TCPIP_DIR") + "/library/config/ipv4.py")
	execfile(__TCPIP_DIR + "/library/config/ipv4.py")
	
	# IPv6 options	
	# niyas use Variables.get("__TCPIP_DIR") 
	execfile(__TCPIP_DIR + "/library/config/ipv6.py")
			
	# TCP options	
	# niyas use Variables.get("__TCPIP_DIR") 
	execfile(__TCPIP_DIR + "/library/config/tcp.py")
	
	# UDP options		
	# niyas use Variables.get("__TCPIP_DIR") 
	execfile(__TCPIP_DIR + "/library/config/udp.py")
	
	# Network Configurations
	# niyas use Variables.get("__TCPIP_DIR") 
	execfile(__TCPIP_DIR + "/library/config/network_config_idx.py")
	##############################################

	
	#############################################
	# Define number of network Configurations
	tcpipStackTickRate = tcpipComponent.createIntegerSymbol("TCPIP_STACK_TICK_RATE", None)
	tcpipStackTickRate.setLabel("TCP/IP Stack State Machine Tick Rate, ms.")
	tcpipStackTickRate.setVisible(True)
	tcpipStackTickRate.setDescription("TCP/IP Stack State Machine Tick Rate")
	tcpipStackTickRate.setDefaultValue(5)
	
	# Event Notification
	tcpipStackEventNotify = tcpipComponent.createBooleanSymbol("TCPIP_STACK_EVENT_NOTIFICATION", None)
	tcpipStackEventNotify.setVisible(False)
	tcpipStackEventNotify.setDefaultValue(True)
	
	# Enable User Notification
	tcpipStackUserNotify= tcpipComponent.createBooleanSymbol("TCPIP_STACK_USER_NOTIFICATION", None)
	tcpipStackUserNotify.setLabel("Enable User Notification")
	tcpipStackUserNotify.setVisible(True)
	tcpipStackUserNotify.setDescription("Enable User Notification")
	tcpipStackUserNotify.setDefaultValue(False)
	tcpipStackUserNotify.setDependencies(tcpipMenuVisibleSingleTrigger, ["TCPIP_STACK_EVENT_NOTIFICATION"])

	# Enable Deinitialize of TCP/IP stack
	tcpipStackDown= tcpipComponent.createBooleanSymbol("TCPIP_STACK_DOWN_OPERATION", None)
	tcpipStackDown.setLabel("Enable Stack Deinitialize Operations")
	tcpipStackDown.setVisible(True)
	tcpipStackDown.setDescription("Enable Deinitialize of TCP/IP stack")
	tcpipStackDown.setDefaultValue(True)
	
	# Enable stack up/down of TCP/IP stack
	tcpipStackIfUpDown= tcpipComponent.createBooleanSymbol("TCPIP_STACK_IF_UP_DOWN_OPERATION", tcpipStackDown)
	tcpipStackIfUpDown.setLabel("Enable IF Up/Down Operations")
	tcpipStackIfUpDown.setVisible(True)
	tcpipStackIfUpDown.setDescription("Enable interface up/down of TCP/IP stack")
	tcpipStackIfUpDown.setDefaultValue(True)
	tcpipStackIfUpDown.setDependencies(tcpipMenuVisibleSingleTrigger, ["TCPIP_STACK_DOWN_OPERATION"])

	# Enable stack Mac Down
	tcpipStackMacDown= tcpipComponent.createBooleanSymbol("TCPIP_STACK_MAC_DOWN_OPERATION", None)
	tcpipStackMacDown.setLabel("Enable MAC Down on Failure")
	tcpipStackMacDown.setVisible(False)
	tcpipStackMacDown.setDescription("Enable MAC Down on Failure")
	tcpipStackMacDown.setDefaultValue(True)
	tcpipStackMacDown.setDependencies(tcpipMenuInvisibleSingleTrigger, ["TCPIP_STACK_DOWN_OPERATION"])

	# Enable Interface Address Change Signaling
	tcpipStackIfChange= tcpipComponent.createBooleanSymbol("TCPIP_STACK_INTERFACE_CHANGE_SIGNALING", None)
	tcpipStackIfChange.setLabel("Enable Interface Address Change Signaling")
	tcpipStackIfChange.setVisible(True)
	tcpipStackIfChange.setDescription("Enable Interface Address Change Signaling")
	tcpipStackIfChange.setDefaultValue(False)
	tcpipStackIfChange.setDependencies(tcpipMenuVisibleSingleTrigger, ["TCPIP_STACK_USE_IPV4"])

	# Enable Configuration Save/Restore Functionality
	tcpipStackConfigSaveRestore= tcpipComponent.createBooleanSymbol("TCPIP_STACK_CONFIGURATION_SAVE_RESTORE", None)
	tcpipStackConfigSaveRestore.setLabel("Enable Configuration Save/Restore Functionality")
	tcpipStackConfigSaveRestore.setVisible(True)
	tcpipStackConfigSaveRestore.setDescription("Enable Configuration Save/Restore Functionality")
	tcpipStackConfigSaveRestore.setDefaultValue(True)
	
		
	# Enable File System Wrapper
	tcpipStackSysFsWrapper = tcpipComponent.createBooleanSymbol("TCPIP_SYS_FS_WRAPPER_NEEDED", None)
	tcpipStackSysFsWrapper.setVisible(False)
	#niyas look how to implement these conditions
	#depends on TCPIP_USE_SNMP || TCPIP_STACK_USE_HTTP_SERVER || TCPIP_STACK_USE_HTTP_NET_SERVER || TCPIP_USE_FTP_MODULE || TCPIP_USE_TFTPC_MODULE
	#niyas if(USE_SYS_FS_NEEDED == False):
		#tcpipStackSysFsWrapper.setDefaultValue(False)
	#else:
		#tcpipStackSysFsWrapper.setDefaultValue(True)

	# Define number of entries in the secure port table
	tcpipStackSecPortNum = tcpipComponent.createIntegerSymbol("TCPIP_STACK_SECURE_PORT_ENTRIES", None)
	tcpipStackSecPortNum.setLabel("Number of entries in the secure port table")
	tcpipStackSecPortNum.setVisible(True)
	tcpipStackSecPortNum.setDescription("Number of entries in the secure port table")
	tcpipStackSecPortNum.setDefaultValue(10)

	# Enable Alias Interfaces Support
	tcpipStackAliasInterface= tcpipComponent.createBooleanSymbol("TCPIP_STACK_ALIAS_INTERFACE_SUPPORT", None)
	tcpipStackAliasInterface.setLabel("Enable Alias Interfaces Support")
	tcpipStackAliasInterface.setVisible(True)
	tcpipStackAliasInterface.setDescription("Enable Alias Interfaces Support")
	tcpipStackAliasInterface.setDefaultValue(False)

	# Enable Packet Logger
	tcpipStackPktLogEnable= tcpipComponent.createBooleanSymbol("TCPIP_PACKET_LOG_ENABLE", None)
	tcpipStackPktLogEnable.setLabel("Enable Packet Logger")
	tcpipStackPktLogEnable.setVisible(True)
	tcpipStackPktLogEnable.setDescription("Enable Packet Logger")
	tcpipStackPktLogEnable.setDefaultValue(False)

	# Number of Entries in the Packet Logger
	tcpipStackPktLogSize= tcpipComponent.createIntegerSymbol("TCPIP_PKT_LOG_SIZE", tcpipStackPktLogEnable)
	tcpipStackPktLogSize.setLabel("Number of Entries in the Packet Logger")
	tcpipStackPktLogSize.setVisible(False)
	tcpipStackPktLogSize.setDescription("Number of Entries in the Packet Logger")
	tcpipStackPktLogSize.setDefaultValue(40)
	tcpipStackPktLogSize.setDependencies(tcpipMenuVisibleSingleTrigger, ["TCPIP_PACKET_LOG_ENABLE"])	
	
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/tcpip_heap.hconfig"
	# TCP Heap Allocation options	
	# niyas use Variables.get("__TCPIP_DIR") 
	execfile(__TCPIP_DIR + "/library/config/tcpip_heap.py")
	
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/arp.hconfig"
	# Address Resolution Protocol options	
	# niyas use Variables.get("__TCPIP_DIR") 
	execfile(__TCPIP_DIR + "/library/config/arp.py")
	
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/berkeley_api.hconfig"
	# BSD API options	
	# niyas use Variables.get("__TCPIP_DIR") 
	execfile(__TCPIP_DIR + "/library/config/berkeley_api.py")
	
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/dhcp.hconfig"
	# DHCP Client options	
	# niyas use Variables.get("__TCPIP_DIR") 
	execfile(__TCPIP_DIR + "/library/config/dhcp.py")
	
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/dhcps.hconfig"
	# DHCP Server options	
	# niyas use Variables.get("__TCPIP_DIR") 
	execfile(__TCPIP_DIR + "/library/config/dhcps.py")
	
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/dns.hconfig"
	# DNS Client options	
	# niyas use Variables.get("__TCPIP_DIR") 
	execfile(__TCPIP_DIR + "/library/config/dns.py")
	
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/dnss.hconfig"
	# DNS Sever options	
	# niyas use Variables.get("__TCPIP_DIR") 
	execfile(__TCPIP_DIR + "/library/config/dnss.py")
	
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/ddns.hconfig"
	# Dynamic DNS options	
	# niyas use Variables.get("__TCPIP_DIR") 
	execfile(__TCPIP_DIR + "/library/config/ddns.py")	
	
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/ftp.hconfig"
	# FTP Server options	
	# niyas use Variables.get("__TCPIP_DIR") 
	execfile(__TCPIP_DIR + "/library/config/ftp.py")
	
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/ftp.hconfig"
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/http.hconfig"
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/http_net.hconfig"
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/icmp.hconfig"
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/nbns.hconfig"
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/tcpip_reboot.hconfig"
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/smtp.hconfig"
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/smtpc.hconfig"
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/snmp.hconfig"
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/snmpv3.hconfig"
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/sntp.hconfig"
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/telnet.hconfig"
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/zeroconf.hconfig"
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/tcpip_announce.hconfig"
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/iperf.hconfig"
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/tcpip_cmd.hconfig"
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/tftpc.hconfig"
	#source "$HARMONY_VERSION_PATH/framework/tcpip/config/igmp.hconfig"
	#ifblock TCPIP_SYS_FS_WRAPPER_NEEDED
		#source "$HARMONY_VERSION_PATH/framework/tcpip/config/sys_fs_wrapper.hconfig"
	#endif
	#########################################
	# TCP/IP Stack Enable 
	print("TCP/IP Stack is Enabled.")
	
def tcpipMenuVisibleSingleTrigger(tcpipDependentSymbol, event):
	if (event["value"] == True):
		print("TCPIP Menu Visible.")		
		tcpipDependentSymbol.setVisible(True)
	else:
		print("TCPIP Menu Invisible.")
		tcpipDependentSymbol.setVisible(False)
		
def tcpipMenuInvisibleSingleTrigger(tcpipDependentSymbol, event):
	if (event["value"] == False):
		print("TCPIP Menu Visible.")		
		tcpipDependentSymbol.setVisible(True)
	else:
		print("TCPIP Menu Invisible.")
		tcpipDependentSymbol.setVisible(False)		

			
def tcpipGenSourceFile(tcpipSourceFile, event):
	tcpipSourceFile.setEnabled(event["value"])
###############################################################


###############################################################