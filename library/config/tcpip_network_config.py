############### Network Configuration ######################
tcpipNetConfigNumMaximum = 10
tcpipNetConfigNumPrev = 1

##############################################################

def instantiateComponent(tcpipNetConfigComponent, index):
	print("TCPIP TCP Component")
	configName = Variables.get("__CONFIGURATION_NAME")	

	
	print(tcpipNetConfigComponent.getID())
	
	tcpipNetConfigIndex = tcpipNetConfigComponent.createIntegerSymbol("INDEX", None)
	tcpipNetConfigIndex.setVisible(False)
	tcpipNetConfigIndex.setDefaultValue(index)	
	
	
	# Network interface Number
	tcpipNetConfigNum = tcpipNetConfigComponent.createIntegerSymbol("TCPIP_STACK_NETWORK_CONFIG_NUMBER", None)
	tcpipNetConfigNum.setLabel("Network Configurations Index")
	tcpipNetConfigNum.setVisible(True)
	tcpipNetConfigNum.setValue(int(str(index)),1)
	tcpipNetConfigNum.setReadOnly(True)
	
	# Network interface Enable
	tcpipNetConfigEnable = tcpipNetConfigComponent.createBooleanSymbol("TCPIP_STACK_NETWORK_CONFIG_IDX" + str(index),None)
	tcpipNetConfigEnable.setLabel("Network interface Enable")
	tcpipNetConfigEnable.setVisible(False)
	tcpipNetConfigEnable.setValue(True,1)
	print(tcpipNetConfigEnable.getID())
	
	# Network interface name
	tcpipNetIfName = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX" + str(index),None)	
	tcpipNetIfName.setLabel("Interface")
	tcpipNetIfName.setVisible(True)
	tcpipNetIfName.setDefaultValue("")
	tcpipNetIfName.setReadOnly(True)


	#niyas to do : check the need of this
	# Ethernet MAC needed
	tcpipNetEthMacNeeded = tcpipNetConfigComponent.createBooleanSymbol("TCPIP_NETWORK_DEFAULT_INTERFACE_ETH_MAC_NEEDED_IDX"+str(index),None)
	tcpipNetEthMacNeeded.setVisible(False)
	tcpipNetEthMacNeeded.setDefaultValue(True)
	#tcpipNetEthMacNeeded.setDependencies(tcpipNetEthMacNeedUpdate, tcpipNetIfName)

	# Network Host name
	tcpipNetHostName = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_HOST_NAME_IDX" + str(index), None)
	tcpipNetHostName.setLabel("Host Name")
	tcpipNetHostName.setVisible(True)
	tcpipNetHostName.setDefaultValue("")
	#tcpipNetHostName.setDependencies(tcpipNetHostNameUpdate, ["TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX"])	
	tcpipNetHostName.setDependencies(tcpipNetHostNameUpdate, [tcpipNetIfName.getID()])
	
	
	# Network Interface MAC address
	tcpipNetMacAddress = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX" + str(index),None)
	tcpipNetMacAddress.setLabel("Mac Address")
	tcpipNetMacAddress.setVisible(True)
	tcpipNetMacAddress.setDefaultValue("")
	tcpipNetMacAddress.setDependencies(tcpipNetMacAddrUpdate, [tcpipNetIfName.getID()])
	
	# Network Interface IP address
	tcpipNetIpAddress = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_IP_ADDRESS_IDX" + str(index), None)
	tcpipNetIpAddress.setLabel("IPv4 Static Address")
	tcpipNetIpAddress.setVisible(True)
	tcpipNetIpAddress.setDefaultValue("")
	tcpipNetIpAddress.setDependencies(tcpipNetIpAddrUpdate, [tcpipNetIfName.getID()])

	# Network Interface IP address Mask
	tcpipNetIpAddrMask = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_IP_MASK_IDX" + str(index),None)
	tcpipNetIpAddrMask.setLabel("IPv4 SubNet Mask")
	tcpipNetIpAddrMask.setVisible(True)
	tcpipNetIpAddrMask.setDefaultValue("255.255.255.0")
	#tcpipNetIpAddrMask.setDependencies(tcpipNetConfigMenuVisible, [tcpipNetIfName.getID()])

	# Network Interface Gateway IP address
	tcpipNetGatewayIpAddr = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_GATEWAY_IDX" + str(index),None)
	tcpipNetGatewayIpAddr.setLabel("IPv4 Default Gateway Address")
	tcpipNetGatewayIpAddr.setVisible(True)
	tcpipNetGatewayIpAddr.setDefaultValue("")
	tcpipNetGatewayIpAddr.setDependencies(tcpipNetGatewayIpAddrUpdate, [tcpipNetIfName.getID()])	
	
	# Network Interface Primary DNS IP address
	tcpipNetPrimDnsIpAddr = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_DNS_IDX" + str(index),None)
	tcpipNetPrimDnsIpAddr.setLabel("IPv4 Primary DNS")
	tcpipNetPrimDnsIpAddr.setVisible(True)
	tcpipNetPrimDnsIpAddr.setDefaultValue("")
	tcpipNetPrimDnsIpAddr.setDependencies(tcpipNetPrimDnsIpAddrUpdate, [tcpipNetIfName.getID()])	

	# Network Interface Secondary DNS IP address
	tcpipNetSecDnsIpAddr = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_SECOND_DNS_IDX" + str(index),None)
	tcpipNetSecDnsIpAddr.setLabel("IPv4 Secondary DNS")
	tcpipNetSecDnsIpAddr.setVisible(True)
	tcpipNetSecDnsIpAddr.setDefaultValue("0.0.0.0")
	#tcpipNetSecDnsIpAddr[index].setDependencies(tcpipNetConfigMenuVisible, [tcpipNetConfig[index].getID()])		

	# Network Interface Power Mode
	tcpipNetPwrMode = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_POWER_MODE_IDX" + str(index),None)
	tcpipNetPwrMode.setLabel("Power Mode")
	tcpipNetPwrMode.setVisible(True)
	tcpipNetPwrMode.setDefaultValue("full")
	#tcpipNetPwrMode.setDependencies(tcpipNetConfigMenuVisible, [tcpipNetConfig[index].getID()])
	
	# Network Configuration Start-up Flags Menu
	tcpipNetStartupFlag = tcpipNetConfigComponent.createMenuSymbol("TCPIP_NETWORK_STARTUP_FLAG_IDX" + str(index),None)
	tcpipNetStartupFlag.setLabel("Network Configuration Start-up Flags")
	tcpipNetStartupFlag.setVisible(True)	
	#tcpipNetStartupFlag[index].setDependencies(tcpipNetConfigMenuVisible, [tcpipNetConfig[index].getID()])
	
	# Enable DHCP Client on this Network Configuration
	tcpipNetDhcpClientEnable = tcpipNetConfigComponent.createBooleanSymbol("TCPIP_NETWORK_INTERFACE_FLAG_DHCP_CLIENT_IDX"+str(index),tcpipNetStartupFlag)
	tcpipNetDhcpClientEnable.setLabel("DHCP Client Enabled on this Interface")
	tcpipNetDhcpClientEnable.setVisible(True)
	tcpipNetDhcpClientEnable.setDefaultValue(True)
	
	
	# Enable ZeroConf Link Local on this Network Configuration
	tcpipNetZConfLLEnable = tcpipNetConfigComponent.createBooleanSymbol("TCPIP_NETWORK_INTERFACE_FLAG_ZCLL_IDX"+str(index),tcpipNetStartupFlag)
	tcpipNetZConfLLEnable.setLabel("ZeroConf Link Local Enabled on this Interface")
	tcpipNetZConfLLEnable.setVisible(True)
	tcpipNetZConfLLEnable.setDefaultValue(False)
	tcpipNetZConfLLEnable.setReadOnly(True)	
	
	
	# Enable DHCP Server on this Network Configuration
	tcpipNetDhcpServerEnable = tcpipNetConfigComponent.createBooleanSymbol("TCPIP_NETWORK_INTERFACE_FLAG_DHCP_SERVER_IDX"+str(index),tcpipNetStartupFlag)
	tcpipNetDhcpServerEnable.setLabel("DHCP Server Enabled on this Interface")
	tcpipNetDhcpServerEnable.setVisible(True)
	tcpipNetDhcpServerEnable.setDefaultValue(False)
	tcpipNetDhcpServerEnable.setReadOnly(True)	
	tcpipNetDhcpServerEnable.setDependencies(tcpipNetStartupFlagDhcpServer, [tcpipNetDhcpClientEnable.getID(), tcpipNetZConfLLEnable.getID()])
	tcpipNetZConfLLEnable.setDependencies(tcpipNetStartupFlagZConf, [tcpipNetDhcpClientEnable.getID(), tcpipNetDhcpServerEnable.getID()])
	tcpipNetDhcpClientEnable.setDependencies(tcpipNetStartupFlagDhcpClient, [tcpipNetZConfLLEnable.getID(), tcpipNetDhcpServerEnable.getID()])
	
	# Enable DNS Client on this Network Configuration
	tcpipNetDnsClientEnable = tcpipNetConfigComponent.createBooleanSymbol("TCPIP_NETWORK_INTERFACE_FLAG_DNS_CLIENT_IDX"+str(index),tcpipNetStartupFlag)
	tcpipNetDnsClientEnable.setLabel("DNS Client Enabled on this Interface")
	tcpipNetDnsClientEnable.setVisible(True)
	tcpipNetDnsClientEnable.setDefaultValue(True)		
	
	
	# Enable DNS Server on this Network Configuration
	tcpipNetDnsServerEnable = tcpipNetConfigComponent.createBooleanSymbol("TCPIP_NETWORK_INTERFACE_FLAG_DNS_SERVER_IDX"+str(index),tcpipNetStartupFlag)
	tcpipNetDnsServerEnable.setLabel("DNS Server Enabled on this Interface")
	tcpipNetDnsServerEnable.setVisible(True)
	tcpipNetDnsServerEnable.setDefaultValue(False)
	tcpipNetDnsServerEnable.setReadOnly(True)	
	tcpipNetDnsServerEnable.setDependencies(tcpipNetStartupFlagDnsServer, [tcpipNetDnsClientEnable.getID()])
	tcpipNetDnsClientEnable.setDependencies(tcpipNetStartupFlagDnsClient, [tcpipNetDnsServerEnable.getID()])
	
	# Enable Multicast on this Network Configuration
	tcpipNetMcastEnable = tcpipNetConfigComponent.createBooleanSymbol("TCPIP_NETWORK_CONFIG_MULTICAST_IDX"+str(index),tcpipNetStartupFlag)
	tcpipNetMcastEnable.setLabel("Multicast Enabled on this Interface")
	tcpipNetMcastEnable.setVisible(True)
	tcpipNetMcastEnable.setDefaultValue(False)
	#tcpipNetMcastEnable.setDependencies(tcpipNetConfigMenuVisible, [tcpipNetConfig[index].getID()])

	# Input IPv6 Static Address and Subnet Prefix Length
	tcpipNetIpv6AddrSubnetInput = tcpipNetConfigComponent.createBooleanSymbol("TCPIP_NETWORK_INTERFACE_FLAG_IPV6_ADDRESS_IDX"+str(index),tcpipNetStartupFlag)
	tcpipNetIpv6AddrSubnetInput.setLabel("IPv6 Static Address and Subnet Prefix Length")
	tcpipNetIpv6AddrSubnetInput.setVisible(False)
	tcpipNetIpv6AddrSubnetInput.setDefaultValue(False)	
	tcpipNetIpv6AddrSubnetInput.setDependencies(tcpipNetConfigMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])	
	
	# IPv6 Static address for this interface
	tcpipNetIpv6StatAddr = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_IPV6_ADDRESS_IDX" + str(index), tcpipNetIpv6AddrSubnetInput)
	tcpipNetIpv6StatAddr.setLabel("IPv6 Static Address")
	tcpipNetIpv6StatAddr.setVisible(False)
	tcpipNetIpv6StatAddr.setDefaultValue("fde4:8dba:82e1::")
	tcpipNetIpv6StatAddr.setDependencies(tcpipNetConfigMenuVisible, [tcpipNetIpv6AddrSubnetInput.getID()])	
	
	# IPv6 Static address Prefix Length for this interface
	tcpipNetIpv6StatAddrPrfxLen = tcpipNetConfigComponent.createIntegerSymbol("TCPIP_NETWORK_DEFAULT_IPV6_PREFIX_LENGTH_IDX" + str(index), tcpipNetIpv6AddrSubnetInput)
	tcpipNetIpv6StatAddrPrfxLen.setLabel("IPv6 Static Address Prefix Length")
	tcpipNetIpv6StatAddrPrfxLen.setVisible(False)
	tcpipNetIpv6StatAddrPrfxLen.setDefaultValue(64)
	tcpipNetIpv6StatAddrPrfxLen.setDependencies(tcpipNetConfigMenuVisible, [tcpipNetIpv6AddrSubnetInput.getID()])
		
	# IPv6 Default Gateway Address for this interface
	tcpipNetIpv6GatewayAddr = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_IPV6_GATEWAY_IDX" + str(index), tcpipNetIpv6AddrSubnetInput)
	tcpipNetIpv6GatewayAddr.setLabel("IPv6 Default Gateway Address")
	tcpipNetIpv6GatewayAddr.setVisible(False)
	tcpipNetIpv6GatewayAddr.setDefaultValue("")
	tcpipNetIpv6GatewayAddr.setDependencies(tcpipNetConfigMenuVisible, [tcpipNetIpv6AddrSubnetInput.getID()])

	# Network Interface MAC Driver Object
	tcpipNetMacDrvObj = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX" + str(index),None)
	tcpipNetMacDrvObj.setLabel("Network MAC Driver")
	tcpipNetMacDrvObj.setVisible(True)
	tcpipNetMacDrvObj.setDefaultValue("")
	tcpipNetMacDrvObj.setDependencies(tcpipNetMACDrvObjUpdate, [tcpipNetIfName.getID()])


	tcpipNetConfigSysConfigFile = tcpipNetConfigComponent.createFileSymbol("TCPIP_NETWORK_CONFIG", None)
	tcpipNetConfigSysConfigFile.setType("STRING")
	tcpipNetConfigSysConfigFile.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipNetConfigSysConfigFile.setSourcePath("library/config/network_config_idx.h.ftl")
	tcpipNetConfigSysConfigFile.setMarkup(True)
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
	#print(symbol.getID())
	#print(event["id"])

	tcpipInterfaceName = event["value"]
	symbol.clearValue()
	if(tcpipInterfaceName == "ETHMAC"):
		symbol.setValue("MCHPBOARD_E",1)
	else:
		if(tcpipInterfaceName == "GMAC"):
			symbol.setValue("MCHPBOARD_C",1)
		else:
			if(tcpipInterfaceName == "ENCX24J600"):
				symbol.setValue("MCHPENCX24_E",1)
			else:
				if(tcpipInterfaceName == "ENC28J60"):
					symbol.setValue("MCHPENC28_E",1)
				else:
					# Implement dhcp server client dependency
					if((tcpipInterfaceName == "MRF24WN") or (tcpipInterfaceName == "WINC1500") or (tcpipInterfaceName == "WILC1000")):
						symbol.setValue("MCHPBOARD_W",1)
					else:
						symbol.setValue("",1)
	
	print("END tcpipNetHostNameUpdate")

# niyas todo : check the need of this func
def tcpipNetEthMacNeedUpdate(symbol, event):
	symbol.clearValue()
	if ((event["value"] == "PIC32CINT") or (event["value"] == "PIC32INT")):
		symbol.setValue(True,1)
	else:
		symbol.setValue(False,1)
	
def tcpipNetMacAddrUpdate(symbol, event):	
	print("Start tcpipNetMacAddrUpdate")
	#print(symbol.getID())
	#print(event["id"])
	#tcpipMacAddrIndex = int(symbol.getID().strip("TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX"))
		
	#print(tcpipMacAddrIndex)
	tcpipInterfaceName = event["value"]
	symbol.clearValue()
	if(tcpipInterfaceName == "GMAC"):
		symbol.setValue("00:04:25:1C:A0:02",1)
	else:
		symbol.setValue("",1)
	
	print("END tcpipNetMacAddrUpdate")	

def tcpipNetIpAddrUpdate(symbol, event):	
	print("Start tcpipNetIpAddrUpdate")
	#print(symbol.getID())
	#print(event["id"])
	#tcpipIpAddrIndex = int(symbol.getID().strip("TCPIP_NETWORK_DEFAULT_IP_ADDRESS_IDX"))
		
	#print(tcpipIpAddrIndex)
	tcpipInterfaceName = event["value"]
	
	symbol.clearValue()
	if(tcpipInterfaceName == "ETHMAC"):
		symbol.setValue("192.168.100.10",1)
	else:
		if(tcpipInterfaceName == "GMAC"):
			symbol.setValue("192.168.100.11",1)
		else:
			if(tcpipInterfaceName == "ENCX24J600"):
				symbol.setValue("192.168.100.12",1)
			else:
				if(tcpipInterfaceName == "ENC28J60"):
					symbol.setValue("192.168.100.13",1)
				else:
					# niyas implment dhcp server client dependency
					if((tcpipInterfaceName == "MRF24WN") or (tcpipInterfaceName == "WINC1500") or (tcpipInterfaceName == "WILC1000")):
						symbol.setValue("192.168.1.2",1)
					else:
						symbol.setValue("0.0.0.0",1)
	
	print("END tcpipNetIpAddrUpdate")	

def tcpipNetGatewayIpAddrUpdate(symbol, event):	
	print("Start tcpipNetGatewayIpAddrUpdate")
	#print(symbol.getID())
	#print(event["id"])
	#tcpipGatewayIpAddrIndex = int(symbol.getID().strip("TCPIP_NETWORK_DEFAULT_GATEWAY_IDX"))
		
	#print(tcpipGatewayIpAddrIndex)
	tcpipInterfaceName = event["value"]	
	symbol.clearValue()
	if((tcpipInterfaceName == "ETHMAC") or (tcpipInterfaceName == "GMAC") or (tcpipInterfaceName == "ENCX24J600")):
		symbol.setValue("192.168.100.1",1)
	else:
		if(tcpipInterfaceName == "ENC28J60"):
			symbol.setValue("192.168.100.2",1)
		else:
			if((tcpipInterfaceName == "MRF24WN") or (tcpipInterfaceName == "WINC1500") or (tcpipInterfaceName == "WILC1000")):
				symbol.setValue("192.168.1.1",1)
			else:					
				# niyas Implement PIC32WK dependency
				symbol.setValue("0.0.0.0",1)
	
	print("END tcpipNetGatewayIpAddrUpdate")		
	
def tcpipNetPrimDnsIpAddrUpdate(symbol, event):	
	print("Start tcpipNetPrimDnsIpAddrUpdate")
	#print(symbol.getID())
	#print(event["id"])
	#tcpipGatewayIpAddrIndex = int(symbol.getID().strip("TCPIP_NETWORK_DEFAULT_DNS_IDX"))
		
	#print(tcpipGatewayIpAddrIndex)
	tcpipInterfaceName = event["value"]
	symbol.clearValue()
	if((tcpipInterfaceName == "ETHMAC") or (tcpipInterfaceName == "GMAC") or (tcpipInterfaceName == "ENCX24J600")):
		symbol.setValue("192.168.100.1",1)
	else:
		if(tcpipInterfaceName == "ENC28J60"):
			symbol.setValue("192.168.100.2",1)
		else:
			if((tcpipInterfaceName == "MRF24WN") or (tcpipInterfaceName == "WINC1500") or (tcpipInterfaceName == "WILC1000")):
				symbol.setValue("192.168.1.1",1)
			else:					
				#niyas Implement PIC32WK dependency
				symbol.setValue("0.0.0.0",1)
		
	print("END tcpipNetPrimDnsIpAddrUpdate")

def tcpipNetMACDrvObjUpdate(symbol, event):	
	print("Start tcpipNetMACDrvObjUpdate")
	#print(symbol.getID())
	#print(event["id"])
	#tcpipIpAddrIndex = int(symbol.getID().strip("TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX"))
		
	#print(tcpipIpAddrIndex)
	tcpipInterfaceName = event["value"]
	
	if(tcpipInterfaceName == "ETHMAC"):
		symbol.setValue("DRV_ETHMAC_PIC32MACObject",1)
	else:
		if(tcpipInterfaceName == "GMAC"):
			symbol.setValue("DRV_GMAC_Object",1)
		else:
			if(tcpipInterfaceName == "ENCX24J600"):
				symbol.setValue("DRV_ENCX24J600_MACObject",1)
			else:
				if(tcpipInterfaceName == "ENC28J60"):
					symbol.setValue("DRV_ENC28J60_MACObject",1)
				else:
					# niyas implment dhcp server client dependency
					if(tcpipInterfaceName == "MRF24WN"):
						symbol.setValue("WDRV_MRF24WN_MACObject",1)
					else:
						if(tcpipInterfaceName == "WINC1500"):
							symbol.setValue("WDRV_WINC1500_MACObject",1)
						else:	
							if(tcpipInterfaceName == "WILC1000"):
								symbol.setValue("WDRV_WILC1000_MACObject",1)
							else:	
								# niyas implment PIC32WK dependency
								symbol.setValue("",1)

	print("END tcpipNetMACDrvObjUpdate")	
	
#niyas revisit this implementataion
# def tcpipNetEnableConfig(symbol, event):
	# global tcpipNetConfigNumPrev
	# print("Start tcpipNetEnableConfig")
	# print(symbol.getID())
	# print(event["id"])
	# tcpipNetConfigNumberValue = event["value"]
	# print(tcpipNetConfigNumberValue)
	# print(tcpipNetConfigNumPrev)
	# if(tcpipNetConfigNumberValue > tcpipNetConfigNumPrev ):
		# tcpipNetConfig[tcpipNetConfigNumPrev].setVisible(True)
		# tcpipNetConfig[tcpipNetConfigNumPrev].setValue(True, 1)
		# print("Set TRUE"+ str(tcpipNetConfigNumPrev))
		# tcpipNetConfigNumPrev = tcpipNetConfigNumPrev + 1
		# #Add more network configurations
	# else:
		# if(tcpipNetConfigNumberValue < tcpipNetConfigNumPrev ):
			# #Reduce network configurations
			# tcpipNetConfigNumPrev = tcpipNetConfigNumPrev - 1
			# tcpipNetConfig[tcpipNetConfigNumPrev].setVisible(False)
			# tcpipNetConfig[tcpipNetConfigNumPrev].setValue(False, 1)
			# print("Set FALSE"+ str(tcpipNetConfigNumPrev))
			
		# #else:
			# #Do Nothing
			
	# print("END tcpipNetEnableConfig")

def tcpipNetStartupFlagZConf(symbol, event):	
	print("Start tcpipNetStartupFlagZConf")
	#print(symbol.getID())
	#print(event["id"])
	tcpipNetStartupZConfIndex = int(symbol.getID().strip("TCPIP_NETWORK_INTERFACE_FLAG_ZCLL_IDX"))
		
	#print(tcpipNetStartupZConfIndex)
	data = symbol.getComponent()
	dhcpServerEnable = data.getSymbolValue("TCPIP_NETWORK_INTERFACE_FLAG_DHCP_SERVER_IDX"+str(tcpipNetStartupZConfIndex))
	dhcpClientEnable = data.getSymbolValue("TCPIP_NETWORK_INTERFACE_FLAG_DHCP_CLIENT_IDX"+str(tcpipNetStartupZConfIndex))

	if((dhcpServerEnable == False) and (dhcpClientEnable == False)) :
		symbol.setReadOnly(False)			
	else:
		symbol.setReadOnly(True)
	
	print("END tcpipNetStartupFlagZConf")	
	
def tcpipNetStartupFlagDhcpClient(symbol, event):	
	print("Start tcpipNetStartupFlagDhcpClient")
	#print(symbol.getID())
	#print(event["id"])

	tcpipNetStartupDhcpClientIndex = int(symbol.getID().strip("TCPIP_NETWORK_INTERFACE_FLAG_DHCP_CLIENT_IDX"))
		
	#print(tcpipNetStartupDhcpClientIndex)
	data = symbol.getComponent()
	dhcpServerEnable = data.getSymbolValue("TCPIP_NETWORK_INTERFACE_FLAG_DHCP_SERVER_IDX"+str(tcpipNetStartupDhcpClientIndex))
	zConfLLEnable = data.getSymbolValue("TCPIP_NETWORK_INTERFACE_FLAG_ZCLL_IDX"+str(tcpipNetStartupDhcpClientIndex))
	#print(dhcpServerEnable,zConfLLEnable)
	if((dhcpServerEnable == False) and (zConfLLEnable == False)) :
		symbol.setReadOnly(False)			
	else:
		symbol.setReadOnly(True)
	
	print("END tcpipNetStartupFlagDhcpClient")
	

def tcpipNetStartupFlagDhcpServer(symbol, event):	
	print("Start tcpipNetStartupFlagDhcpServer")
	#print(symbol.getID())
	#print(event["id"])
	tcpipNetStartupDhcpServerIndex = int(symbol.getID().strip("TCPIP_NETWORK_INTERFACE_FLAG_DHCP_SERVER_IDX"))
	data = symbol.getComponent()
	dhcpClientEnable = data.getSymbolValue("TCPIP_NETWORK_INTERFACE_FLAG_DHCP_CLIENT_IDX"+str(tcpipNetStartupDhcpServerIndex))
	zConfLLEnable = data.getSymbolValue("TCPIP_NETWORK_INTERFACE_FLAG_ZCLL_IDX"+str(tcpipNetStartupDhcpServerIndex))
	
	if((dhcpClientEnable == False) and (zConfLLEnable == False)) :
		symbol.setReadOnly(False)			
	else:
		symbol.setReadOnly(True)
	
	print("END tcpipNetStartupFlagDhcpClient")
	
def tcpipNetStartupFlagDnsServer(symbol, event):	
	print("Start tcpipNetStartupFlagDnsServer")
	#print(symbol.getID())
	#print(event["id"])
	#tcpipNetStartupDnsServerIndex = int(symbol.getID().strip("TCPIP_NETWORK_INTERFACE_FLAG_DNS_SERVER_IDX"))
		
	#print(tcpipNetStartupDnsServerIndex)
		
	if(event["value"] == False) :
		symbol.setReadOnly(False)			
	else:
		symbol.setReadOnly(True)
	
	print("END tcpipNetStartupFlagDnsServer")
	
def tcpipNetStartupFlagDnsClient(symbol, event):	
	print("Start tcpipNetStartupFlagDnsClient")
	#print(symbol.getID())
	#print(event["id"])
	#tcpipNetStartupDnsClientIndex = int(symbol.getID().strip("TCPIP_NETWORK_INTERFACE_FLAG_DNS_CLIENT_IDX"))
		
	#print(tcpipNetStartupDnsClientIndex)
	
	if(event["value"] == False)  :
		symbol.setReadOnly(False)			
	else:
		symbol.setReadOnly(True)
		
	print("END tcpipNetStartupFlagDnsClient")
###############################################################################################################	

def onDependentComponentAdded(tcpipNetConfigComponent, id, remoteComponent):
	if id == "NETCONFIG_MAC_Dependency":
		tcpipNetConfigIndex = int(tcpipNetConfigComponent.getID().strip("tcpipNetConfig_"))
		macInterface = tcpipNetConfigComponent.getSymbolByID("TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX"+str(tcpipNetConfigIndex))
		macInterface.clearValue()
		print(tcpipNetConfigIndex)
		macInterface.setValue(remoteComponent.getDisplayName(), 2)

def onDependentComponentRemoved(tcpipNetConfigComponent, id, remoteComponent):
	if id == "NETCONFIG_MAC_Dependency":
		tcpipNetConfigIndex = int(tcpipNetConfigComponent.getID().strip("tcpipNetConfig_"))
		tcpipNetConfigComponent.clearSymbolValue("TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX"+str(tcpipNetConfigIndex))					