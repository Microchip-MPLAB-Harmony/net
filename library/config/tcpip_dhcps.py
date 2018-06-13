################## DHCP Server Instances ##############################
tcpipDhcpsInstancesNumPrev = 1
tcpipDhcpsInstance = []
tcpipDhcpsAddrRangeStart = []
tcpipDhcpsIpAddr = []
tcpipDhcpsNetMaskAddr = []
tcpipDhcpsGatewayAddr = []
tcpipDhcpsPrimDnsAddr = []
tcpipDhcpsSecDnsAddr = []
tcpipDhcpsIfIdx = []
tcpipDhcpsPoolEn = []
tcpipNetConfigNumMax = 10  # niyas o check this hard coded value
############################################################################
def instantiateComponent(tcpipDhcpsComponent):
	#tcpipNetConfigNumMax = Database.getSymbolValue("tcpipNetConfig","TCPIP_STACK_NETWORK_CONFIG_NUMBER_MAX") # niyas to check this 
	print(tcpipNetConfigNumMax)
	print("TCPIP DHCP Server Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	
	# Enable DHCP Server
	tcpipDhcps = tcpipDhcpsComponent.createBooleanSymbol("TCPIP_STACK_USE_DHCP_SERVER", None)
	tcpipDhcps.setLabel("DHCP Server")
	tcpipDhcps.setVisible(False)
	tcpipDhcps.setDescription("Enable DHCP Server")
	tcpipDhcps.setDefaultValue(True)
	#tcpipDhcps.setDependencies(tcpipDhcpsMenuVisible, ["tcpipIPv4.TCPIP_STACK_USE_IPV4", "tcpipUdp.TCPIP_USE_UDP"])

	# DHCP Server Update Rate in msec
	tcpipDhcpsTskProcessRate = tcpipDhcpsComponent.createIntegerSymbol("TCPIP_DHCPS_TASK_PROCESS_RATE", None)
	tcpipDhcpsTskProcessRate.setLabel("DHCP Server Update Rate in msec")
	tcpipDhcpsTskProcessRate.setVisible(True)
	tcpipDhcpsTskProcessRate.setDescription("DHCP Server Update Rate in msec")
	tcpipDhcpsTskProcessRate.setDefaultValue(200)
	#tcpipDhcpsTskProcessRate.setDependencies(tcpipDhcpsMenuVisibleSingle, ["TCPIP_STACK_USE_DHCP_SERVER"])

	# Maximum Number of Entries in the Lease Table
	tcpipDhcpsLeaseEntryMaxNum = tcpipDhcpsComponent.createIntegerSymbol("TCPIP_DHCPS_LEASE_ENTRIES_DEFAULT", None)
	tcpipDhcpsLeaseEntryMaxNum.setLabel("Maximum Number of Entries in the Lease Table")
	tcpipDhcpsLeaseEntryMaxNum.setVisible(True)
	tcpipDhcpsLeaseEntryMaxNum.setDescription("Maximum Number of Entries in the Lease Table")
	tcpipDhcpsLeaseEntryMaxNum.setDefaultValue(15)
	#tcpipDhcpsLeaseEntryMaxNum.setDependencies(tcpipDhcpsMenuVisibleSingle, ["TCPIP_STACK_USE_DHCP_SERVER"])

	# Time-out for a Solved Entry in the Cache in Seconds.
	tcpipDhcpsLeaseSolvedEntryTimeout = tcpipDhcpsComponent.createIntegerSymbol("TCPIP_DHCPS_LEASE_SOLVED_ENTRY_TMO", None)
	tcpipDhcpsLeaseSolvedEntryTimeout.setLabel("Time-out for a Solved Entry in the Cache in Seconds.")
	tcpipDhcpsLeaseSolvedEntryTimeout.setVisible(True)
	tcpipDhcpsLeaseSolvedEntryTimeout.setDescription("Time-out for a Solved Entry in the Cache in Seconds.")
	tcpipDhcpsLeaseSolvedEntryTimeout.setDefaultValue(1200)
	#tcpipDhcpsLeaseSolvedEntryTimeout.setDependencies(tcpipDhcpsMenuVisibleSingle, ["TCPIP_STACK_USE_DHCP_SERVER"])

	# Time-out for a Solved Entry in the Cache in Seconds.
	tcpipDhcpsLeaseUnsolvedEntryTimeout = tcpipDhcpsComponent.createIntegerSymbol("TCPIP_DHCPS_LEASE_REMOVED_BEFORE_ACK", None)
	tcpipDhcpsLeaseUnsolvedEntryTimeout.setLabel("Time-out for an Unsolved Entry in Seconds")
	tcpipDhcpsLeaseUnsolvedEntryTimeout.setVisible(True)
	tcpipDhcpsLeaseUnsolvedEntryTimeout.setDescription("Time-out for an Unsolved Entry in Seconds")
	tcpipDhcpsLeaseUnsolvedEntryTimeout.setDefaultValue(5)
	#tcpipDhcpsLeaseUnsolvedEntryTimeout.setDependencies(tcpipDhcpsMenuVisibleSingle, ["TCPIP_STACK_USE_DHCP_SERVER"])

	# Delete Old Entries
	tcpipDhcpsOldEntryDelete = tcpipDhcpsComponent.createBooleanSymbol("TCPIP_DHCP_SERVER_DELETE_OLD_ENTRIES", None)
	tcpipDhcpsOldEntryDelete.setLabel("Delete Old Entries")
	tcpipDhcpsOldEntryDelete.setVisible(True)
	tcpipDhcpsOldEntryDelete.setDescription("Delete Old Entries")
	tcpipDhcpsOldEntryDelete.setDefaultValue(True)
	#tcpipDhcpsOldEntryDelete.setDependencies(tcpipDhcpsMenuVisibleSingle, ["TCPIP_STACK_USE_DHCP_SERVER"])


	######################################################################################################################################

	# Number of DHCP Server Driver Instances
	#tcpipDhcpsNetConfigNum = Database.getSymbolValue("tcpip", "TCPIP_STACK_NETWORK_CONFIG_NUMBER")
	#print(tcpipDhcpsNetConfigNum)
	tcpipDhcpsInstancesNum = tcpipDhcpsComponent.createIntegerSymbol("TCPIP_DHCP_SERVER_INSTANCES_NUMBER", None)
	tcpipDhcpsInstancesNum.setLabel("Number of DHCP Server Driver Instances")
	tcpipDhcpsInstancesNum.setMax(tcpipNetConfigNumMax)
	#tcpipDhcpsInstancesNum.setMax(1)
	tcpipDhcpsInstancesNum.setMin(1)
	tcpipDhcpsInstancesNum.setVisible(True)
	tcpipDhcpsInstancesNum.setDescription("Number of DHCP Server Driver Instances")
	tcpipDhcpsInstancesNum.setDefaultValue(1)
	tcpipDhcpsInstancesNum.setDependencies(tcpipDhcpsInstnNumVisible, ["TCPIP_STACK_USE_DHCP_SERVER","tcpipNetConfig.TCPIP_STACK_NETWORK_CONFIG_NUMBER"])

	for index in range(0,tcpipNetConfigNumMax):	
		tcpipDhcpsInstance.append(tcpipDhcpsComponent.createBooleanSymbol("TCPIP_DHCP_SERVER_IDX"+str(index),None))
		tcpipDhcpsInstance[index].setLabel("DHCP Server Instance "+ str(index))
		#tcpipDhcpsInstance[index].setVisible(True)
		if (index < tcpipDhcpsInstancesNum.getValue()):
			#tcpipDhcpsInstance[index].setVisible(True)
			tcpipDhcpsInstance[index].setDefaultValue(True)
		else:
			tcpipDhcpsInstance[index].setDefaultValue(False)
			#tcpipDhcpsInstance[index].setVisible(False)	
		tcpipDhcpsInstance[index].setVisible(False)	
		tcpipDhcpsInstance[index].setDependencies(tcpipDhcpsEnableInstance, ["TCPIP_DHCP_SERVER_INSTANCES_NUMBER", "tcpipNetConfig.TCPIP_STACK_NETWORK_CONFIG_NUMBER", "TCPIP_STACK_USE_DHCP_SERVER"])		
		
		# DHCP Server Address Range Start
		tcpipDhcpsAddrRangeStart.append(tcpipDhcpsComponent.createStringSymbol("TCPIP_DHCPS_DEFAULT_IP_ADDRESS_RANGE_START_IDX" + str(index), tcpipDhcpsInstance[index]))
		tcpipDhcpsAddrRangeStart[index].setLabel("DHCPS Address Range Start")
		tcpipDhcpsAddrRangeStart[index].setVisible(True)
		tcpipDhcpsAddrRangeStart[index].setDefaultValue("192.168.1.100")
		tcpipDhcpsAddrRangeStart[index].setDependencies(tcpipDhcpsInstnAddrRangeMenu, [tcpipDhcpsInstance[index].getID(),"TCPIP_STACK_USE_DHCP_SERVER"])
		
		# DHCP Server IP Address
		tcpipDhcpsIpAddr.append(tcpipDhcpsComponent.createStringSymbol("TCPIP_DHCPS_DEFAULT_SERVER_IP_ADDRESS_IDX" + str(index), tcpipDhcpsInstance[index]))
		tcpipDhcpsIpAddr[index].setLabel("DHCPS Server IP Address")
		tcpipDhcpsIpAddr[index].setVisible(True)
		tcpipDhcpsIpAddr[index].setDefaultValue("192.168.1.1")
		tcpipDhcpsIpAddr[index].setDependencies(tcpipDhcpsInstnIpAddrMenu, [tcpipDhcpsInstance[index].getID(),"TCPIP_STACK_USE_DHCP_SERVER"])
	
		# DHCP Server Net Mask Address
		tcpipDhcpsNetMaskAddr.append(tcpipDhcpsComponent.createStringSymbol("TCPIP_DHCPS_DEFAULT_SERVER_NETMASK_ADDRESS_IDX" + str(index), tcpipDhcpsInstance[index]))
		tcpipDhcpsNetMaskAddr[index].setLabel("DHCPS Netmask")
		tcpipDhcpsNetMaskAddr[index].setVisible(True)
		tcpipDhcpsNetMaskAddr[index].setDefaultValue("255.255.255.0")
		tcpipDhcpsNetMaskAddr[index].setDependencies(tcpipDhcpsInstnNetMaskAddrMenu, [tcpipDhcpsInstance[index].getID(),"TCPIP_STACK_USE_DHCP_SERVER"])
	
		# DHCP Server Default Gateway Address
		tcpipDhcpsGatewayAddr.append(tcpipDhcpsComponent.createStringSymbol("TCPIP_DHCPS_DEFAULT_SERVER_GATEWAY_ADDRESS_IDX" + str(index), tcpipDhcpsInstance[index]))
		tcpipDhcpsGatewayAddr[index].setLabel("Default Gateway")
		tcpipDhcpsGatewayAddr[index].setVisible(True)
		tcpipDhcpsGatewayAddr[index].setDefaultValue("192.168.1.1")
		tcpipDhcpsGatewayAddr[index].setDependencies(tcpipDhcpsInstnGatewayAddrMenu, [tcpipDhcpsInstance[index].getID(),"TCPIP_STACK_USE_DHCP_SERVER"])	
	
		# DHCP Server Primary DNS Server Address
		tcpipDhcpsPrimDnsAddr.append(tcpipDhcpsComponent.createStringSymbol("TCPIP_DHCPS_DEFAULT_SERVER_PRIMARY_DNS_ADDRESS_IDX" + str(index), tcpipDhcpsInstance[index]))
		tcpipDhcpsPrimDnsAddr[index].setLabel("Primary DNS Server Address")
		tcpipDhcpsPrimDnsAddr[index].setVisible(True)
		tcpipDhcpsPrimDnsAddr[index].setDefaultValue("192.168.1.1")
		tcpipDhcpsPrimDnsAddr[index].setDependencies(tcpipDhcpsInstnPrimDnsAddrMenu, [tcpipDhcpsInstance[index].getID(),"TCPIP_STACK_USE_DHCP_SERVER"])	
	
		# DHCP Server Secondary DNS Server Address
		tcpipDhcpsSecDnsAddr.append(tcpipDhcpsComponent.createStringSymbol("TCPIP_DHCPS_DEFAULT_SERVER_SECONDARY_DNS_ADDRESS_IDX" + str(index), tcpipDhcpsInstance[index]))
		tcpipDhcpsSecDnsAddr[index].setLabel("Secondary DNS Server Address")
		tcpipDhcpsSecDnsAddr[index].setVisible(True)
		tcpipDhcpsSecDnsAddr[index].setDefaultValue("192.168.1.1")
		tcpipDhcpsSecDnsAddr[index].setDependencies(tcpipDhcpsInstnSecDnsAddrMenu, [tcpipDhcpsInstance[index].getID(),"TCPIP_STACK_USE_DHCP_SERVER"])	
	
		# DHCP Server interface index
		tcpipDhcpsIfIdx.append(tcpipDhcpsComponent.createIntegerSymbol("TCPIP_DHCP_SERVER_INTERFACE_INDEX_IDX" + str(index), tcpipDhcpsInstance[index]))
		tcpipDhcpsIfIdx[index].setLabel("Interface Index for the DHCP Server")
		tcpipDhcpsIfIdx[index].setVisible(True)
		tcpipDhcpsIfIdx[index].setDefaultValue(index)
		tcpipDhcpsIfIdx[index].setDependencies(tcpipDhcpsInstnIfIdxMenu, [tcpipDhcpsInstance[index].getID(),"TCPIP_STACK_USE_DHCP_SERVER"])	
	
		# DHCP Server Pool enabled
		tcpipDhcpsPoolEn.append(tcpipDhcpsComponent.createBooleanSymbol("TCPIP_DHCP_SERVER_POOL_ENABLED_IDX" + str(index), tcpipDhcpsInstance[index]))
		tcpipDhcpsPoolEn[index].setLabel("Pool Enabled")
		tcpipDhcpsPoolEn[index].setVisible(True)
		tcpipDhcpsPoolEn[index].setDefaultValue(True)
		tcpipDhcpsPoolEn[index].setDependencies(tcpipDhcpsInstnPoolEnMenu, [tcpipDhcpsInstance[index].getID(),"TCPIP_STACK_USE_DHCP_SERVER"])	
	
	######################################################################################################################################
	#Add to system_config.h
	tcpipDhcpsHeaderFtl = tcpipDhcpsComponent.createFileSymbol(None, None)
	tcpipDhcpsHeaderFtl.setSourcePath("library/config/dhcps.h.ftl")
	tcpipDhcpsHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipDhcpsHeaderFtl.setMarkup(True)
	tcpipDhcpsHeaderFtl.setType("STRING")

	# Add dhcps.c file
	tcpipDhcpSourceFile = tcpipDhcpsComponent.createFileSymbol(None, None)
	tcpipDhcpSourceFile.setSourcePath("library/src/dhcps.c")
	tcpipDhcpSourceFile.setOutputName("dhcps.c")
	tcpipDhcpSourceFile.setOverwrite(True)
	tcpipDhcpSourceFile.setDestPath("library/tcpip/src/")
	tcpipDhcpSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipDhcpSourceFile.setType("SOURCE")
	tcpipDhcpSourceFile.setEnabled(True)
	tcpipDhcpSourceFile.setDependencies(tcpipDhcpsGenSourceFile, ["TCPIP_STACK_USE_DHCP_SERVER"])
	
#############################################################################################################
#niyas revisit this implementataion
#global tcpipNetConfigNum
def tcpipDhcpsEnableInstance(tcpipNetDependentSymbol, event):
	global tcpipDhcpsInstancesNumPrev
	print("Start tcpipDhcpsEnableInstance")
	
	if(event["id"] == "TCPIP_STACK_USE_DHCP_SERVER" ):
		print("USE DHCP Server")
		tcpipDhcpsEnable = Database.getSymbolValue("tcpipDhcps","TCPIP_STACK_USE_DHCP_SERVER")
		print("DHCP Server Enabled: " + str(tcpipDhcpsEnable) )
		tcpipDhcpsIndex = int(tcpipNetDependentSymbol.getID().strip("TCPIP_DHCP_SERVER_IDX"))
		print("DHCP Server Index: " + str(tcpipDhcpsIndex) )
		print(tcpipDhcpsInstancesNumPrev)
		if(tcpipDhcpsEnable == True):
			if(tcpipDhcpsIndex < tcpipDhcpsInstancesNumPrev ):
				tcpipNetDependentSymbol.setVisible(True)
		else:
			tcpipNetDependentSymbol.setVisible(False)
	
	else:	
		#tcpipNetDependentSymbol.setVisible(True)
		if(event["id"] == "TCPIP_STACK_NETWORK_CONFIG_NUMBER" ):
			tcpipDhcpsNetConfigNum = int(Database.getSymbolValue("tcpipNetConfig","TCPIP_STACK_NETWORK_CONFIG_NUMBER"))
			print("Net Config Num:" + str(tcpipDhcpsNetConfigNum))
			tcpipNetDependentSymbol.getComponent().getSymbolByID("TCPIP_DHCP_SERVER_INSTANCES_NUMBER").setMax(tcpipDhcpsNetConfigNum)
		else:			
			print(tcpipNetDependentSymbol.getID())
			tcpipDhcpsInstanceNumberValue = event["value"]
			print("Max No:" + str(tcpipNetDependentSymbol.getComponent().getSymbolByID("TCPIP_DHCP_SERVER_INSTANCES_NUMBER").getMax()))
			print(tcpipDhcpsInstanceNumberValue)
			print(tcpipDhcpsInstancesNumPrev)
			if(tcpipDhcpsInstanceNumberValue > tcpipDhcpsInstancesNumPrev ):
				tcpipDhcpsInstance[tcpipDhcpsInstancesNumPrev].setVisible(True)
				tcpipDhcpsInstance[tcpipDhcpsInstancesNumPrev].setValue(True, 1)
				print("Set TRUE"+ str(tcpipDhcpsInstancesNumPrev))
				tcpipDhcpsInstancesNumPrev = tcpipDhcpsInstancesNumPrev + 1
				#Add more network configurations
			else:
				if(tcpipDhcpsInstanceNumberValue < tcpipDhcpsInstancesNumPrev ):
					#Reduce network configurations
					tcpipDhcpsInstancesNumPrev = tcpipDhcpsInstancesNumPrev - 1
					tcpipDhcpsInstance[tcpipDhcpsInstancesNumPrev].setVisible(False)
					tcpipDhcpsInstance[tcpipDhcpsInstancesNumPrev].setValue(False, 1)
					print("Set FALSE"+ str(tcpipDhcpsInstancesNumPrev))
					
				#else:
					#Do Nothing
	print("END tcpipDhcpsEnableInstance")
	
def tcpipDhcpsInstnNumVisible(symbol, event):	
	if(event["id"] == "TCPIP_STACK_NETWORK_CONFIG_NUMBER" ):
		eventValue = event["value"]
		symbol.setValue(int(eventValue),1)
	else:
		if (event["value"] == True):
			tcpipDhcpsNetConfigNum = int(Database.getSymbolValue("tcpipNetConfig","TCPIP_STACK_NETWORK_CONFIG_NUMBER"))
			symbol.getComponent().getSymbolByID("TCPIP_DHCP_SERVER_INSTANCES_NUMBER").setMax(tcpipDhcpsNetConfigNum)		
			symbol.setVisible(True)
		else:		
			symbol.setVisible(False)

##############################################################################################################

# make DHCP Server option visible
def tcpipDhcpsMenuVisible(tcpipDependentSymbol, tcpipIPSymbol):
	tcpipIPv4 = Database.getSymbolValue("tcpipIPv4","TCPIP_STACK_USE_IPV4")
	tcpipUdp = Database.getSymbolValue("tcpipUdp","TCPIP_USE_UDP")

	if(tcpipIPv4 and tcpipUdp):
		tcpipDependentSymbol.setVisible(True)
	else:
		tcpipDependentSymbol.setVisible(False)

	
def tcpipDhcpsMenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("DHCP Server Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("DHCP Server Menu Invisible.")
		symbol.setVisible(False)	

def tcpipDhcpsInstnAddrRangeMenu(symbol, event):
	print("DHCP Server Instance Start.")
	tcpipDhcpsInstnIndex = int(symbol.getID().strip("TCPIP_DHCPS_DEFAULT_IP_ADDRESS_RANGE_START_IDX"))
	print(tcpipDhcpsInstnIndex)
	tcpipDhcpsInstnEnable = tcpipDhcpsInstance[tcpipDhcpsInstnIndex].getValue()
	tcpipDhcpsEnable = Database.getSymbolValue("tcpipDhcps","TCPIP_STACK_USE_DHCP_SERVER")
	print(tcpipDhcpsEnable)
	
	if(tcpipDhcpsInstnEnable and tcpipDhcpsEnable):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)

def tcpipDhcpsInstnIpAddrMenu(symbol, event):
	print("DHCP Server Instance Start.")
	tcpipDhcpsInstnIndex = int(symbol.getID().strip("TCPIP_DHCPS_DEFAULT_SERVER_IP_ADDRESS_IDX"))
	print(tcpipDhcpsInstnIndex)
	tcpipDhcpsInstnEnable = tcpipDhcpsInstance[tcpipDhcpsInstnIndex].getValue()
	tcpipDhcpsEnable = Database.getSymbolValue("tcpipDhcps","TCPIP_STACK_USE_DHCP_SERVER")
	print(tcpipDhcpsEnable)
	
	if(tcpipDhcpsInstnEnable and tcpipDhcpsEnable):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)

def tcpipDhcpsInstnNetMaskAddrMenu(symbol, event):
	print("DHCP Server Instance Start.")
	tcpipDhcpsInstnIndex = int(symbol.getID().strip("TCPIP_DHCPS_DEFAULT_SERVER_NETMASK_ADDRESS_IDX"))
	print(tcpipDhcpsInstnIndex)
	tcpipDhcpsInstnEnable = tcpipDhcpsInstance[tcpipDhcpsInstnIndex].getValue()
	tcpipDhcpsEnable = Database.getSymbolValue("tcpipDhcps","TCPIP_STACK_USE_DHCP_SERVER")
	print(tcpipDhcpsEnable)

	if(tcpipDhcpsInstnEnable and tcpipDhcpsEnable):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)
	
def tcpipDhcpsInstnGatewayAddrMenu(symbol, event):
	print("DHCP Server Instance Start.")
	tcpipDhcpsInstnIndex = int(symbol.getID().strip("TCPIP_DHCPS_DEFAULT_SERVER_GATEWAY_ADDRESS_IDX"))
	print(tcpipDhcpsInstnIndex)
	tcpipDhcpsInstnEnable = tcpipDhcpsInstance[tcpipDhcpsInstnIndex].getValue()
	tcpipDhcpsEnable = Database.getSymbolValue("tcpipDhcps","TCPIP_STACK_USE_DHCP_SERVER")
	print(tcpipDhcpsEnable)

	if(tcpipDhcpsInstnEnable and tcpipDhcpsEnable):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)	

def tcpipDhcpsInstnPrimDnsAddrMenu(symbol, event):
	print("DHCP Server Instance Start.")
	tcpipDhcpsInstnIndex = int(symbol.getID().strip("TCPIP_DHCPS_DEFAULT_SERVER_PRIMARY_DNS_ADDRESS_IDX"))
	print(tcpipDhcpsInstnIndex)
	tcpipDhcpsInstnEnable = tcpipDhcpsInstance[tcpipDhcpsInstnIndex].getValue()
	tcpipDhcpsEnable = Database.getSymbolValue("tcpipDhcps","TCPIP_STACK_USE_DHCP_SERVER")
	print(tcpipDhcpsEnable)

	if(tcpipDhcpsInstnEnable and tcpipDhcpsEnable):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)	

def tcpipDhcpsInstnSecDnsAddrMenu(symbol, event):
	print("DHCP Server Instance Start.")
	tcpipDhcpsInstnIndex = int(symbol.getID().strip("TCPIP_DHCPS_DEFAULT_SERVER_SECONDARY_DNS_ADDRESS_IDX"))
	print(tcpipDhcpsInstnIndex)
	tcpipDhcpsInstnEnable = tcpipDhcpsInstance[tcpipDhcpsInstnIndex].getValue()
	tcpipDhcpsEnable = Database.getSymbolValue("tcpipDhcps","TCPIP_STACK_USE_DHCP_SERVER")
	print(tcpipDhcpsEnable)

	if(tcpipDhcpsInstnEnable and tcpipDhcpsEnable):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)	

def tcpipDhcpsInstnIfIdxMenu(symbol, event):
	print("DHCP Server Instance Start.")
	tcpipDhcpsInstnIndex = int(symbol.getID().strip("TCPIP_DHCP_SERVER_INTERFACE_INDEX_IDX"))
	print(tcpipDhcpsInstnIndex)
	tcpipDhcpsInstnEnable = tcpipDhcpsInstance[tcpipDhcpsInstnIndex].getValue()
	tcpipDhcpsEnable = Database.getSymbolValue("tcpipDhcps","TCPIP_STACK_USE_DHCP_SERVER")
	print(tcpipDhcpsEnable)

	if(tcpipDhcpsInstnEnable and tcpipDhcpsEnable):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)	

def tcpipDhcpsInstnPoolEnMenu(symbol, event):
	print("DHCP Server Instance Start.")
	tcpipDhcpsInstnIndex = int(symbol.getID().strip("TCPIP_DHCP_SERVER_POOL_ENABLED_IDX"))
	print(tcpipDhcpsInstnIndex)
	tcpipDhcpsInstnEnable = tcpipDhcpsInstance[tcpipDhcpsInstnIndex].getValue()
	tcpipDhcpsEnable = Database.getSymbolValue("tcpipDhcps","TCPIP_STACK_USE_DHCP_SERVER")
	print(tcpipDhcpsEnable)

	if(tcpipDhcpsInstnEnable and tcpipDhcpsEnable):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)		
def tcpipDhcpsGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])