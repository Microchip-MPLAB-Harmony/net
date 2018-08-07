def instantiateComponent(netPresComponent, index):
	print("Network Presentation Instance Component")
	configName = Variables.get("__CONFIGURATION_NAME")	
	
	print(netPresComponent.getID())
	
	netPresInstnIndex = netPresComponent.createIntegerSymbol("INDEX", None)
	netPresInstnIndex.setVisible(False)
	netPresInstnIndex.setDefaultValue(index)
	
	numInstances  = Database.getSymbolValue("netPres", "NET_PRES_INSTANCES")
	numInstances = numInstances + 1
	Database.setSymbolValue("netPres", "NET_PRES_INSTANCES", numInstances, 1)
	print(numInstances)
	
	# Network Presentation Instance
	netPresInstnEnable = netPresComponent.createBooleanSymbol("NET_PRES_IDX" + str(index),None)
	netPresInstnEnable.setLabel("Net Presentation Instance " + str(index))
	netPresInstnEnable.setVisible(True)
	netPresInstnEnable.setDefaultValue(True)
	print(netPresInstnEnable.getID())	
	
	# Network Presentation Instance name
	netPresInstnName = netPresComponent.createStringSymbol("NET_PRES_CONFIG_NAME_IDX" + str(index),None)	
	netPresInstnName.setLabel("Name of Presentation Instance")
	netPresInstnName.setVisible(True)
	netPresInstnName.setDefaultValue("NET_PRES_" + str(index))
	
	# Enable MPLAB Harmony TCP/IP as Transport Layer
	netPresInstnTcpipTransLayer = netPresComponent.createBooleanSymbol("NET_PRES_TRANSPORT_AS_TCPIP_IDX" + str(index),None)
	netPresInstnTcpipTransLayer.setLabel("Use MPLAB Harmony TCP/IP as Transport Layer?")
	netPresInstnTcpipTransLayer.setVisible(True)
	netPresInstnTcpipTransLayer.setDefaultValue(True)

	# Enable Stream Connections?
	netPresInstnStreamConn = netPresComponent.createBooleanSymbol("NET_PRES_SUPPORT_STREAM_IDX" + str(index),None)
	netPresInstnStreamConn.setLabel("Support Stream Connections?")
	netPresInstnStreamConn.setVisible(True)
	netPresInstnStreamConn.setDefaultValue((Database.getSymbolValue("tcpipTcp", "TCPIP_USE_TCP") == True) and (netPresInstnTcpipTransLayer.getValue() == True))

	# Enable Data-gram Connections?
	netPresInstnDatagramConn = netPresComponent.createBooleanSymbol("NET_PRES_SUPPORT_DATAGRAM_IDX" + str(index),None)
	netPresInstnDatagramConn.setLabel("Support Data-gram Connections?")
	netPresInstnDatagramConn.setVisible(True)
	netPresInstnDatagramConn.setDefaultValue((Database.getSymbolValue("tcpipUdp", "TCPIP_USE_UDP") == True) and (netPresInstnTcpipTransLayer.getValue() == True))

	# Enable Server Connections?
	netPresInstnSrvrConn = netPresComponent.createBooleanSymbol("NET_PRES_SUPPORT_SERVER_IDX" + str(index),None)
	netPresInstnSrvrConn.setLabel("Support Server Connections?")
	netPresInstnSrvrConn.setVisible(True)
	netPresInstnSrvrConn.setDefaultValue(True)

	# Enable Client Connections?
	netPresInstnClientConn = netPresComponent.createBooleanSymbol("NET_PRES_SUPPORT_CLIENT_IDX" + str(index),None)
	netPresInstnClientConn.setLabel("Support Client Connections?")
	netPresInstnClientConn.setVisible(True)
	netPresInstnClientConn.setDefaultValue(True)

	# Enable Encryption?
	netPresInstnEncryptEnable = netPresComponent.createBooleanSymbol("NET_PRES_SUPPORT_ENCRYPTION" + str(index),None)
	netPresInstnEncryptEnable.setLabel("Support Encryption?")
	netPresInstnEncryptEnable.setVisible(True)
	netPresInstnEncryptEnable.setDefaultValue(False)

# H3_ToDo	
# config NET_PRES_USE_WOLF_SSL_IDX${INSTANCE}
    # bool "Use wolfSSL as Encryption Provider?"
    # depends on USE_3RDPARTY_WOLFSSL
    # depends on !NET_PRES_GENERATE_ENC_STUBS_IDX${INSTANCE}
    # default y
	
# config NET_PRES_USE_WOLF_SSL_DEBUG_LOG_IDX${INSTANCE}
    # bool "Add system console logging for wolf ssl?"
	# depends on NET_PRES_USE_WOLF_SSL_IDX${INSTANCE}
	# depends on WOLFSSL_DEBUG_SUPPORT
	# depends on USE_SYS_CONSOLE
	# default n
	
# config NET_PRES_USE_WOLF_SSL_DEBUG_LOG_BUFFERS_IDX${INSTANCE}
	# int "Number of log message buffers"
    # depends on NET_PRES_USE_WOLF_SSL_DEBUG_LOG_IDX${INSTANCE}
	# default 80
	
	# Generate Encryption Provider Stub Code?
	netPresInstnGenEncryptStub = netPresComponent.createBooleanSymbol("NET_PRES_GENERATE_ENC_STUBS_IDX" + str(index),netPresInstnEncryptEnable)
	netPresInstnGenEncryptStub.setLabel("Generate Encryption Provider Stub Code?")
	netPresInstnGenEncryptStub.setVisible(False)
	netPresInstnGenEncryptStub.setDefaultValue(False)
	netPresInstnGenEncryptStub.setDependencies(netPresInstnEncryptMenuVisible, [netPresInstnEncryptEnable.getID()])
	
	# Support Stream Encryption?
	netPresInstnStreamEncrypt = netPresComponent.createBooleanSymbol("NET_PRES_SUPPORT_STREAM_ENC_IDX" + str(index),netPresInstnEncryptEnable)
	netPresInstnStreamEncrypt.setLabel("Support Stream Encryption?")
	netPresInstnStreamEncrypt.setVisible(False)
	netPresInstnStreamEncrypt.setDefaultValue(True)
	netPresInstnStreamEncrypt.setDependencies(netPresInstnEncryptMenuVisible, [netPresInstnEncryptEnable.getID()])
	
	# Support Data-gram Encryption?
	netPresInstnDataGramEncrypt = netPresComponent.createBooleanSymbol("NET_PRES_SUPPORT_DATAGRAM_ENC_IDX" + str(index),netPresInstnEncryptEnable)
	netPresInstnDataGramEncrypt.setLabel("Support Data-gram Encryption?")
	netPresInstnDataGramEncrypt.setVisible(False)
	netPresInstnDataGramEncrypt.setDefaultValue(False)
	netPresInstnDataGramEncrypt.setDependencies(netPresInstnEncryptMenuVisible, [netPresInstnEncryptEnable.getID()])
		
	# Support Server Encryption?
	netPresInstnServerEncrypt = netPresComponent.createBooleanSymbol("NET_PRES_SUPPORT_SERVER_ENC_IDX" + str(index),netPresInstnEncryptEnable)
	netPresInstnServerEncrypt.setLabel("Support Server Encryption?")
	netPresInstnServerEncrypt.setVisible(False)
	netPresInstnServerEncrypt.setDefaultValue(False)
	netPresInstnServerEncrypt.setDependencies(netPresInstnEncryptMenuVisible, [netPresInstnEncryptEnable.getID()])
			
	# Support Client Encryption?
	netPresInstnClientEncrypt = netPresComponent.createBooleanSymbol("NET_PRES_SUPPORT_CLIENT_ENC_IDX" + str(index),netPresInstnEncryptEnable)
	netPresInstnClientEncrypt.setLabel("Support Client Encryption?")
	netPresInstnClientEncrypt.setVisible(False)
	netPresInstnClientEncrypt.setDefaultValue(True)
	netPresInstnClientEncrypt.setDependencies(netPresInstnEncryptMenuVisible, [netPresInstnEncryptEnable.getID()])
    
	
					
	
def netPresInstnEncryptMenuVisible(symbol, event):
	if (event["value"] == True):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)	