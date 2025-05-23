"""
Copyright (C) 2019-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
"""

def instantiateComponent(netPresComponent, index):
    print("Network Presentation Instance Component")
    configName = Variables.get("__CONFIGURATION_NAME")  
    
    print(netPresComponent.getID())
    processor = Variables.get("__PROCESSOR")
    
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

    netPresUseWolfSSLDebug = netPresComponent.createBooleanSymbol("NET_PRES_USE_WOLF_SSL_DEBUG_LOG_IDX" + str(index),netPresInstnEncryptEnable)
    netPresUseWolfSSLDebug.setLabel("Add system console logging for wolf ssl?")
    netPresUseWolfSSLDebug.setVisible(False)
    netPresUseWolfSSLDebug.setDefaultValue(False)
    netPresUseWolfSSLDebug.setDependencies(netPresWolfSSLDebugEnable, [netPresUseWolfSSL.getID(), "lib_wolfssl.wolfssl_debug"])  

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
        res = data.setSymbolValue("NET_PRES_GENERATE_ENC_STUBS_IDX"+str(netPresIndex), False)
        res = data.setSymbolValue("NET_PRES_USE_WOLF_SSL_IDX"+str(netPresIndex), True)
        res = Database.activateComponents(["lib_wolfssl"])
        #Todo: change to Database.sendMessage(); but need handleMessage() in lib_wolfssl
        Database.setSymbolValue("lib_wolfssl","wolfssl", True) 
        setVal("netPres", "NET_PRES_BLOB_CERT", 0)
        setVal("netPres", "NET_PRES_BLOB_CLIENT_CERT_FORMAT", 1)
        setVal("netPres", "NET_PRES_BLOB_CLIENT_CERT_FILENAME", "wolfssl/certs_test.h")
        setVal("netPres", "NET_PRES_BLOB_CLIENT_CERT_VARIABLE", "client_cert_der_2048")
        setVal("netPres", "NET_PRES_BLOB_CLIENT_CERT_LEN_VARIABLE", "sizeof_client_cert_der_2048")
        setVal("netPres", "NET_PRES_BLOB_SERVER_CERT_FORMAT", 1)
        setVal("netPres", "NET_PRES_BLOB_SERVER_CERT_FILENAME", "wolfssl/certs_test.h")
        setVal("netPres", "NET_PRES_BLOB_SERVER_CERT_VARIABLE", "server_cert_der_2048")
        setVal("netPres", "NET_PRES_BLOB_SERVER_CERT_LEN_VARIABLE", "sizeof_server_cert_der_2048")
        setVal("netPres", "NET_PRES_BLOB_SERVER_KEY_FILENAME", "wolfssl/certs_test.h")
        setVal("netPres", "NET_PRES_BLOB_SERVER_KEY_VARIABLE", "server_key_der_2048")
        setVal("netPres", "NET_PRES_BLOB_SERVER_KEY_LEN_VARIABLE", "sizeof_server_key_der_2048")
        
    else:
        #Todo: change to Database.sendMessage(); but need handleMessage() in lib_wolfssl
        Database.setSymbolValue("lib_wolfssl","wolfssl", False)
        res = Database.deactivateComponents(["lib_wolfssl"])
        res = data.setSymbolValue("NET_PRES_USE_WOLF_SSL_IDX"+str(netPresIndex), False)
        setVal("netPres", "NET_PRES_BLOB_CERT", 1)
        res = data.setSymbolValue("NET_PRES_GENERATE_ENC_STUBS_IDX"+str(netPresIndex), True)
        
        
        
    
def netPresWolfSSLDebugEnable(symbol, event):
    netPresIndex = int(symbol.getID().strip("NET_PRES_USE_WOLF_SSL_DEBUG_LOG_IDX")) 
    data = symbol.getComponent()
    netPresWolfSSLEnabled = data.getSymbolValue("NET_PRES_USE_WOLF_SSL_IDX"+str(netPresIndex))
    wolfSSLDebugEnabled = Database.getSymbolValue("lib_wolfssl", "wolfssl_debug")
    if (netPresWolfSSLEnabled == True) and (wolfSSLDebugEnabled == True):
        symbol.setValue(True)
    else:
        if (symbol.getValue() == True):
            symbol.setValue(False)

        
def netPresInstnStreamEnable(symbol, event):    
    netPresTCPEnabled = Database.getSymbolValue("tcpipTcp", "TCPIP_USE_TCP")
    data = symbol.getComponent()
    netPresIndex = int(symbol.getID().strip("NET_PRES_SUPPORT_STREAM_IDX")) 
    netPresTcpipTransportEnabled = data.getSymbolValue("NET_PRES_TRANSPORT_AS_TCPIP_IDX"+str(netPresIndex))

    if (netPresTCPEnabled == True) and (netPresTcpipTransportEnabled == True):
        symbol.setValue(True)        
    else:
        symbol.setValue(False)
        

def netPresInstnDatagramEnable(symbol, event):
    netPresUDPEnabled = Database.getSymbolValue("tcpipUdp", "TCPIP_USE_UDP")        
    data = symbol.getComponent()
    netPresIndex = int(symbol.getID().strip("NET_PRES_SUPPORT_DATAGRAM_IDX"))   
    netPresTcpipTransportEnabled = data.getSymbolValue("NET_PRES_TRANSPORT_AS_TCPIP_IDX"+str(netPresIndex))

    if (netPresUDPEnabled == True) and (netPresTcpipTransportEnabled == True):
        symbol.setValue(True)
    else:
        symbol.setValue(False)
        

#Set symbols of other components
def setVal(component, symbol, value):
    triggerDict = {"Component":component,"Id":symbol, "Value":value}
    if(Database.sendMessage(component, "SET_SYMBOL", triggerDict) == None):
        print "Set Symbol Failure" + component + ":" + symbol + ":" + str(value)
        return False
    else:
        return True

#Handle messages from other components
def handleMessage(messageID, args):
    retDict= {}
    if (messageID == "SET_SYMBOL"):
        print "handleMessage: Set Symbol"
        retDict= {"Return": "Success"}
        Database.setSymbolValue(args["Component"], args["Id"], args["Value"])
    else:
        retDict= {"Return": "UnImplemented Command"}
    return retDict
