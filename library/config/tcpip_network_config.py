############### Network Configuration ######################
tcpipNetConfigNumMaximum = 10
tcpipNetConfig = []
tcpipNetIfName = []
tcpipNetEthMacNeeded = []
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

TCPIP_STACK_IF_NAME = []
TCPIP_STACK_PIC32C_IF_NAME =	["PIC32CINT", 	"ENCX24J600", 	"ENC28J60", 	"MRF24WN", 		"WINC1500", 	"WILC1000" ]
TCPIP_STACK_PIC32M_IF_NAME =	["PIC32INT", 	"ENCX24J600", 	"ENC28J60", 	"MRF24WN", 		"WINC1500", 	"WILC1000" ]
##############################################################

def instantiateComponent(tcpipNetConfigComponent):
	print("TCPIP TCP Component")
	configName = Variables.get("__CONFIGURATION_NAME")	
	
	tcpipNetConfigNumMax = tcpipNetConfigComponent.createIntegerSymbol("TCPIP_STACK_NETWORK_CONFIG_NUMBER_MAX", None)
	tcpipNetConfigNumMax.setDefaultValue(tcpipNetConfigNumMaximum)
	#tcpipNetConfigNumMax1 = int(Database.getSymbolValue("tcpipNetConfig","TCPIP_STACK_NETWORK_CONFIG_NUMBER_MAX"))
	#print(tcpipNetConfigNumMax1)
	# Define number of network Configurations
	tcpipNetConfigNum = tcpipNetConfigComponent.createIntegerSymbol("TCPIP_STACK_NETWORK_CONFIG_NUMBER", None)
	tcpipNetConfigNum.setLabel("Number of Network Configurations")
	tcpipNetConfigNum.setMax(tcpipNetConfigNumMaximum)
	tcpipNetConfigNum.setMin(1)
	tcpipNetConfigNum.setVisible(True)
	tcpipNetConfigNum.setDescription("Number of Network Configurations")
	tcpipNetConfigNum.setDefaultValue(1)
	if "PIC32C" in Variables.get("__PROCESSOR"):
		TCPIP_STACK_IF_NAME = TCPIP_STACK_PIC32C_IF_NAME
	else:
		TCPIP_STACK_IF_NAME = TCPIP_STACK_PIC32M_IF_NAME
				
	for index in range(0,tcpipNetConfigNumMaximum):	
		tcpipNetConfig.append(tcpipNetConfigComponent.createBooleanSymbol("TCPIP_STACK_NETWORK_CONFIG_IDX"+str(index),None))
		tcpipNetConfig[index].setLabel("Network Configuration "+ str(index))
		tcpipNetConfig[index].setVisible(True)
		if (index < tcpipNetConfigNum.getValue()):
			tcpipNetConfig[index].setVisible(True)
			tcpipNetConfig[index].setDefaultValue(True)
		else:
			tcpipNetConfig[index].setDefaultValue(False)
			tcpipNetConfig[index].setVisible(False)		
		tcpipNetConfig[index].setDependencies(tcpipNetEnableConfig, ["TCPIP_STACK_NETWORK_CONFIG_NUMBER"])
		
		# Network interface name
		tcpipNetIfName.append(tcpipNetConfigComponent.createComboSymbol("TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX" + str(index), tcpipNetConfig[index], TCPIP_STACK_IF_NAME))
		tcpipNetIfName[index].setLabel("Interface")
		tcpipNetIfName[index].setVisible(True)
		tcpipNetIfName[index].setDefaultValue("PIC32CINT")
		tcpipNetIfName[index].setDependencies(tcpipNetConfigMenuVisible, [tcpipNetConfig[index].getID()])	

		# Ethernet MAC needed
		tcpipNetEthMacNeeded.append(tcpipNetConfigComponent.createBooleanSymbol("TCPIP_NETWORK_DEFAULT_INTERFACE_ETH_MAC_NEEDED_IDX"+str(index),tcpipNetConfig[index]))
		tcpipNetEthMacNeeded[index].setVisible(False)
		tcpipNetEthMacNeeded[index].setDefaultValue(True)
		tcpipNetEthMacNeeded[index].setDependencies(tcpipNetEthMacNeedUpdate, [tcpipNetIfName[index].getID()])
		
		# Network Host name
		tcpipNetHostName.append(tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_HOST_NAME_IDX" + str(index), tcpipNetConfig[index]))
		tcpipNetHostName[index].setLabel("Host Name")
		tcpipNetHostName[index].setVisible(True)
		tcpipNetHostName[index].setDefaultValue("MCHPBOARD_C")
		tcpipNetHostName[index].setDependencies(tcpipNetHostNameUpdate, [tcpipNetIfName[index].getID(),tcpipNetConfig[index].getID()])	
		
		
		# Network Interface MAC address
		tcpipNetMacAddress.append(tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX" + str(index), tcpipNetConfig[index]))
		tcpipNetMacAddress[index].setLabel("Mac Address")
		tcpipNetMacAddress[index].setVisible(True)
		tcpipNetMacAddress[index].setDefaultValue("00:04:25:1C:A0:02")
		tcpipNetMacAddress[index].setDependencies(tcpipNetMacAddrUpdate, [tcpipNetIfName[index].getID(), tcpipNetConfig[index].getID()])

		# Network Interface IP address
		tcpipNetIpAddress.append(tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_IP_ADDRESS_IDX" + str(index), tcpipNetConfig[index]))
		tcpipNetIpAddress[index].setLabel("IP Address")
		tcpipNetIpAddress[index].setVisible(True)
		tcpipNetIpAddress[index].setDefaultValue("192.168.100.11")
		tcpipNetIpAddress[index].setDependencies(tcpipNetIpAddrUpdate, [tcpipNetIfName[index].getID(), tcpipNetConfig[index].getID()])

		# Network Interface IP address Mask
		tcpipNetIpAddrMask.append(tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_IP_MASK_IDX" + str(index), tcpipNetConfig[index]))
		tcpipNetIpAddrMask[index].setLabel("Network Mask")
		tcpipNetIpAddrMask[index].setVisible(True)
		tcpipNetIpAddrMask[index].setDefaultValue("255.255.255.0")
		tcpipNetIpAddrMask[index].setDependencies(tcpipNetConfigMenuVisible, [tcpipNetConfig[index].getID()])

		# Network Interface Gateway IP address
		tcpipNetGatewayIpAddr.append(tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_GATEWAY_IDX" + str(index), tcpipNetConfig[index]))
		tcpipNetGatewayIpAddr[index].setLabel("Default Gateway")
		tcpipNetGatewayIpAddr[index].setVisible(True)
		tcpipNetGatewayIpAddr[index].setDefaultValue("192.168.100.1")
		tcpipNetGatewayIpAddr[index].setDependencies(tcpipNetGatewayIpAddrUpdate, [tcpipNetIfName[index].getID(), tcpipNetConfig[index].getID()])	
		
		# Network Interface Primary DNS IP address
		tcpipNetPrimDnsIpAddr.append(tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_DNS_IDX" + str(index), tcpipNetConfig[index]))
		tcpipNetPrimDnsIpAddr[index].setLabel("Primary DNS")
		tcpipNetPrimDnsIpAddr[index].setVisible(True)
		tcpipNetPrimDnsIpAddr[index].setDefaultValue("192.168.100.1")
		tcpipNetPrimDnsIpAddr[index].setDependencies(tcpipNetPrimDnsIpAddrUpdate, [tcpipNetIfName[index].getID(), tcpipNetConfig[index].getID()])	

		# Network Interface Secondary DNS IP address
		tcpipNetSecDnsIpAddr.append(tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_SECOND_DNS_IDX" + str(index), tcpipNetConfig[index]))
		tcpipNetSecDnsIpAddr[index].setLabel("Secondary DNS")
		tcpipNetSecDnsIpAddr[index].setVisible(True)
		tcpipNetSecDnsIpAddr[index].setDefaultValue("0.0.0.0")
		tcpipNetSecDnsIpAddr[index].setDependencies(tcpipNetConfigMenuVisible, [tcpipNetConfig[index].getID()])		

		# Network Interface Power Mode
		tcpipNetPwrMode.append(tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_POWER_MODE_IDX" + str(index), tcpipNetConfig[index]))
		tcpipNetPwrMode[index].setLabel("Power Mode")
		tcpipNetPwrMode[index].setVisible(True)
		tcpipNetPwrMode[index].setDefaultValue("full")
		tcpipNetPwrMode[index].setDependencies(tcpipNetConfigMenuVisible, [tcpipNetConfig[index].getID()])
		
		# Network Configuration Start-up Flags Menu
		tcpipNetStartupFlag.append(tcpipNetConfigComponent.createMenuSymbol("TCPIP_NETWORK_STARTUP_FLAG_IDX" + str(index), tcpipNetConfig[index]))
		tcpipNetStartupFlag[index].setLabel("Network Configuration Start-up Flags")
		tcpipNetStartupFlag[index].setVisible(True)	
		tcpipNetStartupFlag[index].setDependencies(tcpipNetConfigMenuVisible, [tcpipNetConfig[index].getID()])
		
		# Enable DHCP Client on this Network Configuration
		tcpipNetDhcpClientEnable.append(tcpipNetConfigComponent.createBooleanSymbol("TCPIP_NETWORK_INTERFACE_FLAG_DHCP_CLIENT_IDX"+str(index),tcpipNetStartupFlag[index]))
		tcpipNetDhcpClientEnable[index].setLabel("DHCP Client Enabled on this Interface")
		tcpipNetDhcpClientEnable[index].setVisible(True)
		tcpipNetDhcpClientEnable[index].setDefaultValue(True)
		
		
		# Enable ZeroConf Link Local on this Network Configuration
		tcpipNetZConfLLEnable.append(tcpipNetConfigComponent.createBooleanSymbol("TCPIP_NETWORK_INTERFACE_FLAG_ZCLL_IDX"+str(index),tcpipNetStartupFlag[index]))
		tcpipNetZConfLLEnable[index].setLabel("ZeroConf Link Local Enabled on this Interface")
		tcpipNetZConfLLEnable[index].setVisible(True)
		tcpipNetZConfLLEnable[index].setDefaultValue(False)
		tcpipNetZConfLLEnable[index].setReadOnly(True)	
		
		
		# Enable DHCP Server on this Network Configuration
		tcpipNetDhcpServerEnable.append(tcpipNetConfigComponent.createBooleanSymbol("TCPIP_NETWORK_INTERFACE_FLAG_DHCP_SERVER_IDX"+str(index),tcpipNetStartupFlag[index]))
		tcpipNetDhcpServerEnable[index].setLabel("DHCP Server Enabled on this Interface")
		tcpipNetDhcpServerEnable[index].setVisible(True)
		tcpipNetDhcpServerEnable[index].setDefaultValue(False)
		tcpipNetDhcpServerEnable[index].setReadOnly(True)	
		tcpipNetDhcpServerEnable[index].setDependencies(tcpipNetStartupFlagDhcpServer, [tcpipNetDhcpClientEnable[index].getID(), tcpipNetZConfLLEnable[index].getID(), tcpipNetConfig[index].getID()])
		tcpipNetZConfLLEnable[index].setDependencies(tcpipNetStartupFlagZConf, [tcpipNetDhcpClientEnable[index].getID(), tcpipNetDhcpServerEnable[index].getID(), tcpipNetConfig[index].getID()])
		tcpipNetDhcpClientEnable[index].setDependencies(tcpipNetStartupFlagDhcpClient, [tcpipNetZConfLLEnable[index].getID(), tcpipNetDhcpServerEnable[index].getID(), tcpipNetConfig[index].getID()])
		
		# Enable DNS Client on this Network Configuration
		tcpipNetDnsClientEnable.append(tcpipNetConfigComponent.createBooleanSymbol("TCPIP_NETWORK_INTERFACE_FLAG_DNS_CLIENT_IDX"+str(index),tcpipNetStartupFlag[index]))
		tcpipNetDnsClientEnable[index].setLabel("DNS Client Enabled on this Interface")
		tcpipNetDnsClientEnable[index].setVisible(True)
		tcpipNetDnsClientEnable[index].setDefaultValue(True)		
		
		
		# Enable DNS Server on this Network Configuration
		tcpipNetDnsServerEnable.append(tcpipNetConfigComponent.createBooleanSymbol("TCPIP_NETWORK_INTERFACE_FLAG_DNS_SERVER_IDX"+str(index),tcpipNetStartupFlag[index]))
		tcpipNetDnsServerEnable[index].setLabel("DNS Server Enabled on this Interface")
		tcpipNetDnsServerEnable[index].setVisible(True)
		tcpipNetDnsServerEnable[index].setDefaultValue(False)
		tcpipNetDnsServerEnable[index].setReadOnly(True)	
		tcpipNetDnsServerEnable[index].setDependencies(tcpipNetStartupFlagDnsServer, [tcpipNetDnsClientEnable[index].getID(), tcpipNetConfig[index].getID()])
		tcpipNetDnsClientEnable[index].setDependencies(tcpipNetStartupFlagDnsClient, [tcpipNetDnsServerEnable[index].getID(), tcpipNetConfig[index].getID()])
		
		# Enable Multicast on this Network Configuration
		tcpipNetMcastEnable.append(tcpipNetConfigComponent.createBooleanSymbol("TCPIP_NETWORK_CONFIG_MULTICAST_IDX"+str(index),tcpipNetStartupFlag[index]))
		tcpipNetMcastEnable[index].setLabel("Multicast Enabled on this Interface")
		tcpipNetMcastEnable[index].setVisible(True)
		tcpipNetMcastEnable[index].setDefaultValue(False)
		tcpipNetMcastEnable[index].setDependencies(tcpipNetConfigMenuVisible, [tcpipNetConfig[index].getID()])

		# Input IPv6 Static Address and Subnet Prefix Length
		tcpipNetIpv6AddrSubnetInput.append(tcpipNetConfigComponent.createBooleanSymbol("TCPIP_NETWORK_INTERFACE_FLAG_IPV6_ADDRESS_IDX"+str(index),tcpipNetStartupFlag[index]))
		tcpipNetIpv6AddrSubnetInput[index].setLabel("IPv6 Static Address and Subnet Prefix Length")
		tcpipNetIpv6AddrSubnetInput[index].setVisible(True)
		tcpipNetIpv6AddrSubnetInput[index].setDefaultValue(False)	
		# niyas dependency with IPv6 flag
		#tcpipNetDnsServerEnable[index].setDependencies(tcpipNetPrimDnsIpAddrUpdate, [tcpipNetIfName[index].getID(), tcpipNetConfig[index].getID()])
		
		# IPv6 Static address for this interface
		tcpipNetIpv6StatAddr.append(tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_IPV6_ADDRESS_IDX" + str(index), tcpipNetIpv6AddrSubnetInput[index]))
		tcpipNetIpv6StatAddr[index].setLabel("IPv6 Static Address")
		tcpipNetIpv6StatAddr[index].setVisible(False)
		tcpipNetIpv6StatAddr[index].setDefaultValue("fde4:8dba:82e1::")
		tcpipNetIpv6StatAddr[index].setDependencies(tcpipNetConfigMenuVisible, [tcpipNetIpv6AddrSubnetInput[index].getID()])	
		
		# IPv6 Static address Prefix Length for this interface
		tcpipNetIpv6StatAddrPrfxLen.append(tcpipNetConfigComponent.createIntegerSymbol("TCPIP_NETWORK_DEFAULT_IPV6_PREFIX_LENGTH_IDX" + str(index), tcpipNetIpv6AddrSubnetInput[index]))
		tcpipNetIpv6StatAddrPrfxLen[index].setLabel("IPv6 Static Address Prefix Length")
		tcpipNetIpv6StatAddrPrfxLen[index].setVisible(False)
		tcpipNetIpv6StatAddrPrfxLen[index].setDefaultValue(64)
		tcpipNetIpv6StatAddrPrfxLen[index].setDependencies(tcpipNetConfigMenuVisible, [tcpipNetIpv6AddrSubnetInput[index].getID()])
			
		# IPv6 Default Gateway Address for this interface
		tcpipNetIpv6GatewayAddr.append(tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_IPV6_GATEWAY_IDX" + str(index), tcpipNetIpv6AddrSubnetInput[index]))
		tcpipNetIpv6GatewayAddr[index].setLabel("IPv6 Default Gateway Address")
		tcpipNetIpv6GatewayAddr[index].setVisible(False)
		tcpipNetIpv6GatewayAddr[index].setDefaultValue("")
		tcpipNetIpv6GatewayAddr[index].setDependencies(tcpipNetConfigMenuVisible, [tcpipNetIpv6AddrSubnetInput[index].getID()])

		# Network Interface MAC Driver Object
		tcpipNetMacDrvObj.append(tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX" + str(index), tcpipNetConfig[index]))
		tcpipNetMacDrvObj[index].setLabel("Network MAC Driver")
		tcpipNetMacDrvObj[index].setVisible(True)
		tcpipNetMacDrvObj[index].setDefaultValue("DRV_GMAC_Object")
		tcpipNetMacDrvObj[index].setDependencies(tcpipNetMACDrvObjUpdate, [tcpipNetIfName[index].getID(), tcpipNetConfig[index].getID()])	


###############################################################################################################			
def tcpipNetConfigMenuVisible(symbol, event):
	if (event["value"] == True):
		print("Net Config Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("Net Config Menu Invisible.")
		symbol.setVisible(False)
		
def tcpipNetHostNameUpdate(symbol, event):	
	print("Start tcpipNetHostNameUpdate")
	print(symbol.getID())
	print(event["id"])
	tcpipHostNameIndex = int(symbol.getID().strip("TCPIP_NETWORK_DEFAULT_HOST_NAME_IDX"))
	#tcpipIfIndex = TCPIP_STACK_IF_NAME.index(tcpipNetIfName[tcpipHostNameIndex].getValue())
	
	print(tcpipHostNameIndex)
	tcpipInterafceName = tcpipNetIfName[tcpipHostNameIndex].getValue()
	
	if (tcpipNetConfig[tcpipHostNameIndex].getValue() == True):
		symbol.setVisible(True)
		if(tcpipInterafceName == "PIC32INT"):
			symbol.setValue("MCHPBOARD_E",1)
		else:
			if(tcpipInterafceName == "PIC32CINT"):
				symbol.setValue("MCHPBOARD_C",1)
			else:
				if(tcpipInterafceName == "ENCX24J600"):
					symbol.setValue("MCHPBOARD_E",1)
				else:
					if(tcpipInterafceName == "ENC28J60"):
						symbol.setValue("MCHPENC_E",1)
					else:
						# niyas implment dhcp server client dependency
						if((tcpipInterafceName == "MRF24WN") or (tcpipInterafceName == "WINC1500") or (tcpipInterafceName == "WILC1000")):
							symbol.setValue("MCHPBOARD_W",1)
						else:
							symbol.setValue("",1)
	else: 
		symbol.setVisible(False)	
	print("END tcpipNetHostNameUpdate")

def tcpipNetEthMacNeedUpdate(symbol, event):
	if ((event["value"] == "PIC32CINT") or (event["value"] == "PIC32INT")):
		symbol.setValue(True,1)
	else:
		symbol.setValue(False,1)
	
def tcpipNetMacAddrUpdate(symbol, event):	
	print("Start tcpipNetMacAddrUpdate")
	print(symbol.getID())
	print(event["id"])
	tcpipMacAddrIndex = int(symbol.getID().strip("TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX"))
		
	print(tcpipMacAddrIndex)
	
	if (tcpipNetConfig[tcpipMacAddrIndex].getValue() == True):
		symbol.setVisible(True)
		if(tcpipNetIfName[tcpipMacAddrIndex].getValue() == "PIC32CINT"):
			symbol.setValue("00:04:25:1C:A0:02",1)
		else:
			symbol.setValue("",1)
	else: 
		symbol.setVisible(False)	
	print("END tcpipNetMacAddrUpdate")	

def tcpipNetIpAddrUpdate(symbol, event):	
	print("Start tcpipNetIpAddrUpdate")
	print(symbol.getID())
	print(event["id"])
	tcpipIpAddrIndex = int(symbol.getID().strip("TCPIP_NETWORK_DEFAULT_IP_ADDRESS_IDX"))
		
	print(tcpipIpAddrIndex)
	tcpipInterafceName = tcpipNetIfName[tcpipIpAddrIndex].getValue()
	
	if (tcpipNetConfig[tcpipIpAddrIndex].getValue() == True):
		symbol.setVisible(True)
		if(tcpipInterafceName == "PIC32INT"):
			symbol.setValue("192.168.100.10",1)
		else:
			if(tcpipInterafceName == "PIC32CINT"):
				symbol.setValue("192.168.100.11",1)
			else:
				if(tcpipInterafceName == "ENCX24J600"):
					symbol.setValue("192.168.100.12",1)
				else:
					if(tcpipInterafceName == "ENC28J60"):
						symbol.setValue("192.168.100.13",1)
					else:
						# niyas implment dhcp server client dependency
						if((tcpipInterafceName == "MRF24WN") or (tcpipInterafceName == "WINC1500") or (tcpipInterafceName == "WILC1000")):
							symbol.setValue("192.168.1.2",1)
						else:
							symbol.setValue("0.0.0.0",1)
	else: 
		symbol.setVisible(False)	
	print("END tcpipNetIpAddrUpdate")	

def tcpipNetGatewayIpAddrUpdate(symbol, event):	
	print("Start tcpipNetGatewayIpAddrUpdate")
	print(symbol.getID())
	print(event["id"])
	tcpipGatewayIpAddrIndex = int(symbol.getID().strip("TCPIP_NETWORK_DEFAULT_GATEWAY_IDX"))
		
	print(tcpipGatewayIpAddrIndex)
	tcpipInterafceName = tcpipNetIfName[tcpipGatewayIpAddrIndex].getValue()
	
	if (tcpipNetConfig[tcpipGatewayIpAddrIndex].getValue() == True):
		symbol.setVisible(True)
		if((tcpipInterafceName == "PIC32INT") or (tcpipInterafceName == "PIC32CINT") or (tcpipInterafceName == "ENCX24J600")):
			symbol.setValue("192.168.100.1",1)
		else:
			if(tcpipInterafceName == "ENC28J60"):
				symbol.setValue("192.168.100.2",1)
			else:
				if((tcpipInterafceName == "MRF24WN") or (tcpipInterafceName == "WINC1500") or (tcpipInterafceName == "WILC1000")):
					symbol.setValue("192.168.1.1",1)
				else:					
					# niyas implment PIC32WK dependency
					symbol.setValue("0.0.0.0",1)
	else: 
		symbol.setVisible(False)	
	print("END tcpipNetGatewayIpAddrUpdate")		
	
def tcpipNetPrimDnsIpAddrUpdate(symbol, event):	
	print("Start tcpipNetPrimDnsIpAddrUpdate")
	print(symbol.getID())
	print(event["id"])
	tcpipGatewayIpAddrIndex = int(symbol.getID().strip("TCPIP_NETWORK_DEFAULT_DNS_IDX"))
		
	print(tcpipGatewayIpAddrIndex)
	tcpipInterafceName = tcpipNetIfName[tcpipGatewayIpAddrIndex].getValue()
	
	if (tcpipNetConfig[tcpipGatewayIpAddrIndex].getValue() == True):
		symbol.setVisible(True)
		if((tcpipInterafceName == "PIC32INT") or (tcpipInterafceName == "PIC32CINT") or (tcpipInterafceName == "ENCX24J600")):
			symbol.setValue("192.168.100.1",1)
		else:
			if(tcpipInterafceName == "ENC28J60"):
				symbol.setValue("192.168.100.2",1)
			else:
				if((tcpipInterafceName == "MRF24WN") or (tcpipInterafceName == "WINC1500") or (tcpipInterafceName == "WILC1000")):
					symbol.setValue("192.168.1.1",1)
				else:					
					# niyas implment PIC32WK dependency
					symbol.setValue("0.0.0.0",1)
	else: 
		symbol.setVisible(False)	
	print("END tcpipNetPrimDnsIpAddrUpdate")

def tcpipNetMACDrvObjUpdate(symbol, event):	
	print("Start tcpipNetMACDrvObjUpdate")
	print(symbol.getID())
	print(event["id"])
	tcpipIpAddrIndex = int(symbol.getID().strip("TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX"))
		
	print(tcpipIpAddrIndex)
	tcpipInterafceName = tcpipNetIfName[tcpipIpAddrIndex].getValue()
	
	if (tcpipNetConfig[tcpipIpAddrIndex].getValue() == True):
		symbol.setVisible(True)
		if(tcpipInterafceName == "PIC32INT"):
			symbol.setValue("DRV_ETHMAC_PIC32MACObject",1)
		else:
			if(tcpipInterafceName == "PIC32CINT"):
				symbol.setValue("DRV_GMAC_Object",1)
			else:
				if(tcpipInterafceName == "ENCX24J600"):
					symbol.setValue("DRV_ENCX24J600_MACObject",1)
				else:
					if(tcpipInterafceName == "ENC28J60"):
						symbol.setValue("DRV_ENC28J60_MACObject",1)
					else:
						# niyas implment dhcp server client dependency
						if(tcpipInterafceName == "MRF24WN"):
							symbol.setValue("WDRV_MRF24WN_MACObject",1)
						else:
							if(tcpipInterafceName == "WINC1500"):
								symbol.setValue("WDRV_WINC1500_MACObject",1)
							else:	
								if(tcpipInterafceName == "WILC1000"):
									symbol.setValue("WDRV_WILC1000_MACObject",1)
								else:	
									# niyas implment PIC32WK dependency
									symbol.setValue("",1)
	else: 
		symbol.setVisible(False)	
	print("END tcpipNetMACDrvObjUpdate")	
	
#niyas revisit this implementataion
def tcpipNetEnableConfig(symbol, event):
	global tcpipNetConfigNumPrev
	print("Start tcpipNetEnableConfig")
	print(symbol.getID())
	print(event["id"])
	tcpipNetConfigNumberValue = event["value"]
	print(tcpipNetConfigNumberValue)
	print(tcpipNetConfigNumPrev)
	if(tcpipNetConfigNumberValue > tcpipNetConfigNumPrev ):
		tcpipNetConfig[tcpipNetConfigNumPrev].setVisible(True)
		tcpipNetConfig[tcpipNetConfigNumPrev].setValue(True, 1)
		print("Set TRUE"+ str(tcpipNetConfigNumPrev))
		tcpipNetConfigNumPrev = tcpipNetConfigNumPrev + 1
		#Add more network configurations
	else:
		if(tcpipNetConfigNumberValue < tcpipNetConfigNumPrev ):
			#Reduce network configurations
			tcpipNetConfigNumPrev = tcpipNetConfigNumPrev - 1
			tcpipNetConfig[tcpipNetConfigNumPrev].setVisible(False)
			tcpipNetConfig[tcpipNetConfigNumPrev].setValue(False, 1)
			print("Set FALSE"+ str(tcpipNetConfigNumPrev))
			
		#else:
			#Do Nothing
			
	print("END tcpipNetEnableConfig")

def tcpipNetStartupFlagDhcpServer(symbol, event):	
	print("Start tcpipNetStartupFlagDhcpServer")
	print(symbol.getID())
	print(event["id"])
	tcpipNetStartupDhcpServerIndex = int(symbol.getID().strip("TCPIP_NETWORK_INTERFACE_FLAG_DHCP_SERVER_IDX"))
		
	print(tcpipNetStartupDhcpServerIndex)
	
	if (tcpipNetConfig[tcpipNetStartupDhcpServerIndex].getValue() == True):
		symbol.setVisible(True)
		
		if((tcpipNetZConfLLEnable[tcpipNetStartupDhcpServerIndex].getValue() == False) and (tcpipNetDhcpClientEnable[tcpipNetStartupDhcpServerIndex].getValue() == False)) :
			symbol.setReadOnly(False)			
		else:
			symbol.setReadOnly(True)
	else: 
		symbol.setVisible(False)	
	print("END tcpipNetStartupFlagDhcpServer")		
	
def tcpipNetStartupFlagZConf(symbol, event):	
	print("Start tcpipNetStartupFlagZConf")
	print(symbol.getID())
	print(event["id"])
	tcpipNetStartupZConfIndex = int(symbol.getID().strip("TCPIP_NETWORK_INTERFACE_FLAG_ZCLL_IDX"))
		
	print(tcpipNetStartupZConfIndex)
	
	if (tcpipNetConfig[tcpipNetStartupZConfIndex].getValue() == True):
		symbol.setVisible(True)
		
		if((tcpipNetDhcpServerEnable[tcpipNetStartupZConfIndex].getValue() == False) and (tcpipNetDhcpClientEnable[tcpipNetStartupZConfIndex].getValue() == False)) :
			symbol.setReadOnly(False)			
		else:
			symbol.setReadOnly(True)
	else: 
		symbol.setVisible(False)	
	print("END tcpipNetStartupFlagZConf")	
	
def tcpipNetStartupFlagDhcpClient(symbol, event):	
	print("Start tcpipNetStartupFlagDhcpClient")
	print(symbol.getID())
	print(event["id"])
	tcpipNetStartupDhcpClientIndex = int(symbol.getID().strip("TCPIP_NETWORK_INTERFACE_FLAG_DHCP_CLIENT_IDX"))
		
	print(tcpipNetStartupDhcpClientIndex)
	
	if (tcpipNetConfig[tcpipNetStartupDhcpClientIndex].getValue() == True):
		symbol.setVisible(True)
		
		if((tcpipNetDhcpServerEnable[tcpipNetStartupDhcpClientIndex].getValue() == False) and (tcpipNetZConfLLEnable[tcpipNetStartupDhcpClientIndex].getValue() == False)) :
			symbol.setReadOnly(False)			
		else:
			symbol.setReadOnly(True)
	else: 
		symbol.setVisible(False)	
	print("END tcpipNetStartupFlagDhcpClient")
	
	
	
def tcpipNetStartupFlagDnsServer(symbol, event):	
	print("Start tcpipNetStartupFlagDnsServer")
	print(symbol.getID())
	print(event["id"])
	tcpipNetStartupDnsServerIndex = int(symbol.getID().strip("TCPIP_NETWORK_INTERFACE_FLAG_DNS_SERVER_IDX"))
		
	print(tcpipNetStartupDnsServerIndex)
	
	if (tcpipNetConfig[tcpipNetStartupDnsServerIndex].getValue() == True):
		symbol.setVisible(True)
		
		if(tcpipNetDnsClientEnable[tcpipNetStartupDnsServerIndex].getValue() == False)  :
			symbol.setReadOnly(False)			
		else:
			symbol.setReadOnly(True)
	else: 
		symbol.setVisible(False)	
	print("END tcpipNetStartupFlagDnsServer")
	
def tcpipNetStartupFlagDnsClient(symbol, event):	
	print("Start tcpipNetStartupFlagDnsClient")
	print(symbol.getID())
	print(event["id"])
	tcpipNetStartupDnsClientIndex = int(symbol.getID().strip("TCPIP_NETWORK_INTERFACE_FLAG_DNS_CLIENT_IDX"))
		
	print(tcpipNetStartupDnsClientIndex)
	
	if (tcpipNetConfig[tcpipNetStartupDnsClientIndex].getValue() == True):
		symbol.setVisible(True)
		
		if(tcpipNetDnsServerEnable[tcpipNetStartupDnsClientIndex].getValue() == False)  :
			symbol.setReadOnly(False)			
		else:
			symbol.setReadOnly(True)
	else: 
		symbol.setVisible(False)	
	print("END tcpipNetStartupFlagDnsClient")
###############################################################################################################		