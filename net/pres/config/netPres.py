def instantiateComponent(netPresComponent, index):
	print("Network Presentation Instance Component")
	configName = Variables.get("__CONFIGURATION_NAME")	
	
	print(netPresComponent.getID())
	
	netPresInstnIndex = netPresComponent.createIntegerSymbol("INDEX", None)
	netPresInstnIndex.setVisible(False)
	netPresInstnIndex.setDefaultValue(index)
	
	# Network Presentation Instance
	netPresInstnEnable = netPresComponent.createBooleanSymbol("NET_PRES_IDX" + str(index),None)
	netPresInstnEnable.setLabel("Net Presentation Instance " + str(index))
	netPresInstnEnable.setVisible(True)
	netPresInstnEnable.setDefaultValue(True)
	print(netPresInstnEnable.getID())	
	
	# Network Presentation Instance name
	netPresInstnName = netPresComponent.createStringSymbol("NET_PRES_CONFIG_NAME_IDX" + str(index),netPresInstnEnable)	
	netPresInstnName.setLabel("Name of Presentation Instance")
	netPresInstnName.setVisible(True)
	netPresInstnName.setDefaultValue("NET_PRES_" + str(index))
	
	# Enable MPLAB Harmony TCP/IP as Transport Layer
	netPresInstnTcpipTransLayer = netPresComponent.createBooleanSymbol("NET_PRES_TRANSPORT_AS_TCPIP_IDX" + str(index),netPresInstnEnable)
	netPresInstnTcpipTransLayer.setLabel("Use MPLAB Harmony TCP/IP as Transport Layer?")
	netPresInstnTcpipTransLayer.setVisible(True)
	netPresInstnTcpipTransLayer.setDefaultValue(True)

	# Enable Stream Connections?
	netPresInstnStreamConn = netPresComponent.createBooleanSymbol("NET_PRES_SUPPORT_STREAM_IDX" + str(index),netPresInstnEnable)
	netPresInstnStreamConn.setLabel("Support Stream Connections?")
	netPresInstnStreamConn.setVisible(True)
	netPresInstnStreamConn.setDefaultValue((Database.getSymbolValue("tcpipTcp", "TCPIP_USE_TCP") == True) and (netPresInstnTcpipTransLayer.getValue() == True))
	netPresInstnStreamConn.setDependencies(netPresInstnStreamEnable, ["tcpipTcp.TCPIP_USE_TCP",netPresInstnTcpipTransLayer.getID()])

	# Enable Data-gram Connections?
	netPresInstnDatagramConn = netPresComponent.createBooleanSymbol("NET_PRES_SUPPORT_DATAGRAM_IDX" + str(index),netPresInstnEnable)
	netPresInstnDatagramConn.setLabel("Support Data-gram Connections?")
	netPresInstnDatagramConn.setVisible(True)
	netPresInstnDatagramConn.setDefaultValue((Database.getSymbolValue("tcpipUdp", "TCPIP_USE_UDP") == True) and (netPresInstnTcpipTransLayer.getValue() == True))
	netPresInstnDatagramConn.setDependencies(netPresInstnDatagramEnable, ["tcpipUdp.TCPIP_USE_UDP", netPresInstnTcpipTransLayer.getID()])

	# Enable Server Connections?
	netPresInstnSrvrConn = netPresComponent.createBooleanSymbol("NET_PRES_SUPPORT_SERVER_IDX" + str(index),netPresInstnEnable)
	netPresInstnSrvrConn.setLabel("Support Server Connections?")
	netPresInstnSrvrConn.setVisible(True)
	netPresInstnSrvrConn.setDefaultValue(True)

	# Enable Client Connections?
	netPresInstnClientConn = netPresComponent.createBooleanSymbol("NET_PRES_SUPPORT_CLIENT_IDX" + str(index),netPresInstnEnable)
	netPresInstnClientConn.setLabel("Support Client Connections?")
	netPresInstnClientConn.setVisible(True)
	netPresInstnClientConn.setDefaultValue(True)

	# Enable Encryption?
	netPresInstnEncryptEnable = netPresComponent.createBooleanSymbol("NET_PRES_SUPPORT_ENCRYPTION" + str(index),netPresInstnEnable)
	netPresInstnEncryptEnable.setLabel("Support Encryption?")
	netPresInstnEncryptEnable.setVisible(True)
	netPresInstnEncryptEnable.setDefaultValue(False)

# H3_ToDo	
# config NET_PRES_USE_WOLF_SSL_IDX${INSTANCE}
    # bool "Use wolfSSL as Encryption Provider?"
    # depends on USE_3RDPARTY_WOLFSSL
    # depends on !NET_PRES_GENERATE_ENC_STUBS_IDX${INSTANCE}
    # default y
	
	# netPresUseWolfSSL = netPresComponent.createBooleanSymbol("NET_PRES_USE_WOLF_SSL_IDX" + str(index),netPresInstnEncryptEnable)
	# netPresUseWolfSSL.setLabel("Use wolfSSL as Encryption Provider?")
	# netPresUseWolfSSL.setVisible((Database.getSymbolValue("lib_wolfssl", "wolfssl") == True) and (netPresInstnEncryptEnable.getValue() == True))
	# # netPresUseWolfSSL.setVisible(True)
	# netPresUseWolfSSL.setDefaultValue((Database.getSymbolValue("lib_wolfssl", "wolfssl") == True) and (netPresInstnEncryptEnable.getValue() == True))
	# netPresUseWolfSSL.setDependencies(netPresInstnUseWolfSSLMenu, [netPresInstnEncryptEnable.getID(),"lib_wolfssl.wolfssl"])	

	netPresEncryptionProvider = netPresComponent.createKeyValueSetSymbol("NET_PRES_ENC_PROVIDE_IDX"+str(index),netPresInstnEncryptEnable)
	netPresEncryptionProvider.setVisible(False)
	netPresEncryptionProvider.setLabel("Encryption Provider")
	netPresEncryptionProvider.addKey("WolfSSL", "0", "Enable WolfSSL")
	netPresEncryptionProvider.addKey("Generate Stub", "1", "Generate Encryption Stub")
	netPresEncryptionProvider.setDisplayMode("Key")
	netPresEncryptionProvider.setOutputMode("Key")
	netPresEncryptionProvider.setDefaultValue(1)
	netPresEncryptionProvider.setDependencies(netPresInstnEncryptMenuVisible, [netPresInstnEncryptEnable.getID()])
	
	netPresUseWolfSSL = netPresComponent.createBooleanSymbol("NET_PRES_USE_WOLF_SSL_IDX" + str(index),netPresInstnEncryptEnable)
	netPresUseWolfSSL.setLabel("Use wolfSSL as Encryption Provider?")
	netPresUseWolfSSL.setVisible(False)
	netPresUseWolfSSL.setDefaultValue((netPresEncryptionProvider.getSelectedKey() == "WolfSSL"))
	netPresUseWolfSSL.setDependencies(netPresInstnEncProviderMenu, [netPresEncryptionProvider.getID()])		
	
	# Generate Encryption Provider Stub Code?
	netPresInstnGenEncryptStub = netPresComponent.createBooleanSymbol("NET_PRES_GENERATE_ENC_STUBS_IDX" + str(index),netPresInstnEncryptEnable)
	netPresInstnGenEncryptStub.setLabel("Generate Encryption Provider Stub Code?")
	netPresInstnGenEncryptStub.setVisible(False)
	netPresInstnGenEncryptStub.setDefaultValue((netPresEncryptionProvider.getSelectedKey() == "Generate Stub"))
	
	netPresEnableWolfSSL = netPresComponent.createBooleanSymbol("NET_PRES_ENABLE_WOLF_SSL_IDX",netPresInstnEncryptEnable)
	netPresEnableWolfSSL.setLabel("Enable Wolfssl")
	netPresEnableWolfSSL.setVisible(False)
	netPresEnableWolfSSL.setDefaultValue(False)
	netPresEnableWolfSSL.setDependencies(netPresWolfSSLEnable, [netPresUseWolfSSL.getID()])	

	netPresUseWolfSSLDebug = netPresComponent.createBooleanSymbol("NET_PRES_USE_WOLF_SSL_DEBUG_LOG_IDX" + str(index),netPresEncryptionProvider)
	netPresUseWolfSSLDebug.setLabel("Add system console logging for wolf ssl?")
	netPresUseWolfSSLDebug.setVisible(False)
	netPresUseWolfSSLDebug.setDefaultValue(False)
	netPresUseWolfSSLDebug.setDependencies(netPresWolfSSLDebugEnable, [netPresUseWolfSSL.getID(), "lib_wolfssl.wolfsslDebug"])	

	netPresWolfSSLLogBuffNum = netPresComponent.createIntegerSymbol("NET_PRES_USE_WOLF_SSL_DEBUG_LOG_BUFFERS_IDX" + str(index),netPresUseWolfSSLDebug)
	netPresWolfSSLLogBuffNum.setVisible(False)
	netPresWolfSSLLogBuffNum.setLabel("Number of log message buffers")
	netPresWolfSSLLogBuffNum.setDefaultValue(80)
	netPresWolfSSLLogBuffNum.setDependencies(netPresInstnEncryptMenuVisible, [netPresUseWolfSSLDebug.getID()])	
	
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
		
def netPresInstnEncProviderMenu(symbol, event):
	netPresIndex = int(symbol.getID().strip("NET_PRES_USE_WOLF_SSL_IDX"))	
	data = symbol.getComponent()	
	if (event["value"] == 0):
		res = data.setSymbolValue("NET_PRES_USE_WOLF_SSL_IDX"+str(netPresIndex), True, 2)
		res = data.setSymbolValue("NET_PRES_GENERATE_ENC_STUBS_IDX"+str(netPresIndex), False, 2)
		
	else:
		res = data.setSymbolValue("NET_PRES_USE_WOLF_SSL_IDX"+str(netPresIndex), False, 2)
		res = data.setSymbolValue("NET_PRES_GENERATE_ENC_STUBS_IDX"+str(netPresIndex), True, 2)

def netPresWolfSSLEnable (symbol, event):
	if (event["value"] == True):
		res = Database.activateComponents(["lib_wolfssl"])
		Database.setSymbolValue("lib_wolfssl","wolfssl", True, 2)
	else:
		Database.setSymbolValue("lib_wolfssl","wolfssl", False, 2)
		res = Database.deactivateComponents(["lib_wolfssl"])
	
def netPresWolfSSLDebugEnable(symbol, event):
	netPresIndex = int(symbol.getID().strip("NET_PRES_USE_WOLF_SSL_DEBUG_LOG_IDX"))	
	data = symbol.getComponent()
	netPresWolfSSLEnabled = data.getSymbolValue("NET_PRES_USE_WOLF_SSL_IDX"+str(netPresIndex))
	wolfSSLDebugEnabled = Database.getSymbolValue("lib_wolfssl", "wolfsslDebug")
	
	if (netPresWolfSSLEnabled == True) and (wolfSSLDebugEnabled == True):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)

def netPresInstnUseWolfSSLMenu(symbol, event):

	netPresIndex = int(symbol.getID().strip("NET_PRES_USE_WOLF_SSL_IDX"))	
	data = symbol.getComponent()
	netPresEncryptionEnabled = data.getSymbolValue("NET_PRES_SUPPORT_ENCRYPTION"+str(netPresIndex))
	wolfSSLEnabled = Database.getSymbolValue("lib_wolfssl", "wolfssl")
	
	print "wolfssl enabled"
	print (wolfSSLEnabled)
	
	print "net pres encryption"
	print (netPresEncryptionEnabled)
	
	if (netPresEncryptionEnabled == True) and (wolfSSLEnabled == True):
		symbol.setValue(True, 2)
		symbol.setVisible(True)
	else:
		symbol.setValue(False, 2)
		symbol.setVisible(False)
		
def netPresInstnStreamEnable(symbol, event):	
	netPresTCPEnabled = Database.getSymbolValue("tcpipTcp", "TCPIP_USE_TCP")
	data = symbol.getComponent()
	netPresIndex = int(symbol.getID().strip("NET_PRES_SUPPORT_STREAM_IDX"))	
	netPresTcpipTransportEnabled = data.getSymbolValue("NET_PRES_TRANSPORT_AS_TCPIP_IDX"+str(netPresIndex))

	if (netPresTCPEnabled == True) and (netPresTcpipTransportEnabled == True):
		symbol.setValue(True, 2)		
	else:
		symbol.setValue(False, 2)
		

def netPresInstnDatagramEnable(symbol, event):
	netPresUDPEnabled = Database.getSymbolValue("tcpipUdp", "TCPIP_USE_UDP")		
	data = symbol.getComponent()
	netPresIndex = int(symbol.getID().strip("NET_PRES_SUPPORT_DATAGRAM_IDX"))	
	netPresTcpipTransportEnabled = data.getSymbolValue("NET_PRES_TRANSPORT_AS_TCPIP_IDX"+str(netPresIndex))

	if (netPresUDPEnabled == True) and (netPresTcpipTransportEnabled == True):
		symbol.setValue(True, 2)
	else:
		symbol.setValue(False, 2)