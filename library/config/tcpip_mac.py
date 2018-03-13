TCPIP_STACK_PHY_TYPE =	["National_DP83640", "National_DP83848", "SMSC_LAN8700", "SMSC_LAN8720", "SMSC_LAN8740", "SMSC_LAN9303", "KSZ8061", "KSZ8041", "KSZ8863", "IP_IP101GR" ]
#niyas global enum from SYS module
SYS_MODULE_POWER_STATE =	["SYS_MODULE_POWER_OFF","SYS_MODULE_POWER_SLEEP","SYS_MODULE_POWER_IDLE_STOP","SYS_MODULE_POWER_IDLE_RUN","SYS_MODULE_POWER_RUN_FULL"]

__TCPIP_DIR = "D:/06_Repo/05_H3/h3/net"
def instantiateComponent(tcpipEthMacComponent):
	print("TCPIP Ethernet MAC Component")
	configName = Variables.get("__CONFIGURATION_NAME")	
	
	
	# Use Internal Ethernet MAC Driver?	
	tcpipEthMac = tcpipEthMacComponent.createBooleanSymbol("TCPIP_USE_ETH_MAC", None) # Niyas to check the component name and dependency
	tcpipEthMac.setLabel("Use Internal Ethernet MAC Driver?")
	tcpipEthMac.setVisible(True)
	tcpipEthMac.setDescription("Use Internal Ethernet MAC Driver?")
	tcpipEthMac.setDefaultValue(True)
	# Niyas to check this dependency
	# default y if USE_PIC32CINT_ETH_MAC_NEEDED
	# default n if !USE_PIC32CINT_ETH_MAC_NEEDED
	# tcpipEthMac.setDependencies(tcpipMenuVisibleSingleTrigger, ["HAVE_ETH"])
	if "PIC32C" in Variables.get("__PROCESSOR"):
		execfile(__TCPIP_DIR + "/library/config/tcpip_mac_pic32c.py")
	else:
		#execfile(__TCPIP_DIR + "/library/config/tcpip_mac_pic32m.py")
		print("PIC32M internal MAC driver not implemented")