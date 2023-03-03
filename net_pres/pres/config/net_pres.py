"""
Copyright (C) 2019-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
autoConnectTableCrypto = [["lib_crypto", "LIB_CRYPTO_WOLFCRYPT_Dependency", "lib_wolfcrypt", "lib_wolfcrypt"]] 
autoConnectTableWolfssl = [["lib_wolfssl", "WolfSSL_Crypto_Dependency", "lib_wolfcrypt", "lib_wolfcrypt"]] 
netPresMaxCaCertNum = 3
netPresMoreClientCertConfigNumPrev = 0
netPresMaxCaCertConfigIdx = []
netPresBlobMoreClientCertFormat = []
netPresBlobMoreClientCertFileName = []
netPresBlobMoreClientCertVar = []
netPresBlobMoreClientCertLenVar = []

def instantiateComponent(net_PresComponent):

    print("Network Presentation Component")
    configName = Variables.get("__CONFIGURATION_NAME")
        
    # Network Presentation Layer
    netPresNeeded = net_PresComponent.createBooleanSymbol("NET_PRES_NEEDED", None)
    netPresNeeded.setLabel("Use Network Presentation Layer")
    netPresNeeded.setVisible(False)
    netPresNeeded.setDefaultValue(True)

    # Use MPLAB Harmony Networking Presentation Layer
    netPresUse = net_PresComponent.createBooleanSymbol("NET_PRES_USE", None)
    netPresUse.setLabel("Use MPLAB Harmony Networking Presentation Layer")
    netPresUse.setVisible(False)
    netPresUse.setDefaultValue(True)
    
    # RTOS Configuration
    netPresRtosMenu = net_PresComponent.createMenuSymbol("NET_PRES_RTOS_MENU", None)
    netPresRtosMenu.setLabel("RTOS Configuration")
    netPresRtosMenu.setDescription("RTOS Configuration")
    netPresRtosMenu.setVisible(False)
    netPresRtosMenu.setVisible((Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != "BareMetal") and (Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != None))
    netPresRtosMenu.setDependencies(netPresshowRTOSMenu, ["HarmonyCore.SELECT_RTOS"])
    
    # Net Pres Execution mode
    netPresExecMode = net_PresComponent.createComboSymbol("NET_PRES_RTOS", netPresRtosMenu, ["Standalone"]) 
    netPresExecMode.setLabel("Run this driver instance as")
    netPresExecMode.setVisible(False)
    netPresExecMode.setDescription("Net Pres Execution mode")
    netPresExecMode.setDefaultValue("Standalone")
    
    # Net Pres Task Stack Size
    netPresTaskSize = net_PresComponent.createIntegerSymbol("NET_PRES_RTOS_STACK_SIZE", netPresRtosMenu)
    netPresTaskSize.setLabel("Stack Size (in bytes)")
    netPresTaskSize.setVisible(True)
    netPresTaskSize.setDescription("Net Pres Task Stack Size")
    netPresTaskSize.setDefaultValue(4096)
    netPresTaskSize.setDependencies(netPresRTOSStandaloneMenu, ["NET_PRES_RTOS"])
    
    # Net Pres Task Priority
    netPresTaskPriority = net_PresComponent.createIntegerSymbol("NET_PRES_RTOS_TASK_PRIORITY", netPresRtosMenu)
    netPresTaskPriority.setLabel("Task Priority")
    netPresTaskPriority.setVisible(True)
    netPresTaskPriority.setDescription("Net Pres Task Priority")
    netPresTaskPriority.setDefaultValue(1)
    netPresTaskPriority.setDependencies(netPresRTOSStandaloneMenu, ["NET_PRES_RTOS"])
    
    # Net Pres Task Delay?
    netPresUseTaskDelay = net_PresComponent.createBooleanSymbol("NET_PRES_RTOS_USE_DELAY", netPresRtosMenu)
    netPresUseTaskDelay.setLabel("Use Task Delay?")
    netPresUseTaskDelay.setVisible(True)
    netPresUseTaskDelay.setDescription("Net Pres Use Task Delay?")
    netPresUseTaskDelay.setDefaultValue(True)
    netPresUseTaskDelay.setDependencies(netPresRTOSStandaloneMenu, ["NET_PRES_RTOS"])
    
    # Net Pres Task Delay
    netPresTaskDelay = net_PresComponent.createIntegerSymbol("NET_PRES_RTOS_DELAY", netPresRtosMenu)
    netPresTaskDelay.setLabel("Task Delay")
    netPresTaskDelay.setVisible(True)
    netPresTaskDelay.setDescription("Net Pres Task Delay")
    netPresTaskDelay.setDefaultValue(1)
    netPresTaskDelay.setDependencies(netPresRTOSTaskDelayMenu, ["NET_PRES_RTOS","NET_PRES_RTOS_USE_DELAY"])
        
    # Number of Presentation Sockets
    netPresSocketCnt = net_PresComponent.createIntegerSymbol("NET_PRES_SOCKETS", None)
    netPresSocketCnt.setLabel("Number of Presentation Sockets")
    netPresSocketCnt.setVisible(True)
    netPresSocketCnt.setDescription("Number of Presentation Sockets")
    netPresSocketCnt.setDefaultValue(10)    
    
    # Enable TNGTLS Certificate loading?
    netPresTNGTLScert = net_PresComponent.createBooleanSymbol("NET_PRES_BLOB_ENABLE_ATECC_TNGTLS",None)
    netPresTNGTLScert.setLabel("Trust&Go client certificate")
    netPresTNGTLScert.setVisible(True)
    netPresTNGTLScert.setDescription("Auto load device certificate from Trust&Go TNGTLS? Cryptoauth lib should be configured seperately")
    netPresTNGTLScert.setDefaultValue(False)
    netPresTNGTLScert.setDependencies(netPresWolfsslTngtls,["NET_PRES_BLOB_ENABLE_ATECC_TNGTLS"])
    
    # Enable MPLAB Harmony TCP/IP as Transport Layer
    netPresTcpipTransLayer = net_PresComponent.createBooleanSymbol("NET_PRES_TRANSPORT_AS_TCPIP", None)
    netPresTcpipTransLayer.setLabel("Use MPLAB Harmony TCP/IP as Transport Layer?")
    netPresTcpipTransLayer.setVisible(True)
    netPresTcpipTransLayer.setDefaultValue(True)   
    netPresTcpipTransLayer.setReadOnly(True)

    # Enable Stream Connections?
    netPresStreamConn = net_PresComponent.createBooleanSymbol("NET_PRES_SUPPORT_STREAM", None)
    netPresStreamConn.setLabel("Support Stream Connections?")
    netPresStreamConn.setVisible(True)
    netPresStreamConn.setDefaultValue((Database.getSymbolValue("tcpipTcp", "TCPIP_USE_TCP") == True))
    netPresStreamConn.setDependencies(netPresStreamEnable, ["tcpipTcp.TCPIP_USE_TCP","NET_PRES_TRANSPORT_AS_TCPIP"])

    # Enable Data-gram Connections?
    netPresDatagramConn = net_PresComponent.createBooleanSymbol("NET_PRES_SUPPORT_DATAGRAM",None)
    netPresDatagramConn.setLabel("Support Datagram Connections?")
    netPresDatagramConn.setVisible(True)
    netPresDatagramConn.setDefaultValue((Database.getSymbolValue("tcpipUdp", "TCPIP_USE_UDP") == True))
    netPresDatagramConn.setDependencies(netPresDatagramEnable, ["tcpipUdp.TCPIP_USE_UDP", "NET_PRES_TRANSPORT_AS_TCPIP"])

    # Enable Server Connections?
    netPresSrvrConn = net_PresComponent.createBooleanSymbol("NET_PRES_SUPPORT_SERVER", None)
    netPresSrvrConn.setLabel("Support Server Connections?")
    netPresSrvrConn.setVisible(True)
    netPresSrvrConn.setDefaultValue(True)

    # Enable Client Connections?
    netPresClientConn = net_PresComponent.createBooleanSymbol("NET_PRES_SUPPORT_CLIENT", None)
    netPresClientConn.setLabel("Support Client Connections?")
    netPresClientConn.setVisible(True)
    netPresClientConn.setDefaultValue(True)

    # Enable Encryption?
    netPresEncryptEnable = net_PresComponent.createBooleanSymbol("NET_PRES_SUPPORT_ENCRYPTION", None)
    netPresEncryptEnable.setLabel("Support Encryption?")
    netPresEncryptEnable.setVisible(True)
    netPresEncryptEnable.setDefaultValue(False)

    netPresEncryptionProvider = net_PresComponent.createKeyValueSetSymbol("NET_PRES_ENC_PROVIDE", netPresEncryptEnable)
    netPresEncryptionProvider.setVisible(False)
    netPresEncryptionProvider.setLabel("Encryption Provider")
    netPresEncryptionProvider.addKey("WolfSSL", "0", "Enable WolfSSL")
    netPresEncryptionProvider.addKey("Generate Stub", "1", "Generate Encryption Stub")
    netPresEncryptionProvider.setDisplayMode("Key")
    netPresEncryptionProvider.setOutputMode("Key")
    netPresEncryptionProvider.setDefaultValue(1)
    netPresEncryptionProvider.setDependencies(netPresEncryptMenuVisible, ["NET_PRES_SUPPORT_ENCRYPTION"])
    
    netPresUseWolfSSL = net_PresComponent.createBooleanSymbol("NET_PRES_USE_WOLF_SSL", netPresEncryptEnable)
    netPresUseWolfSSL.setLabel("Use wolfSSL as Encryption Provider?")
    netPresUseWolfSSL.setVisible(False)
    netPresUseWolfSSL.setDefaultValue((netPresEncryptionProvider.getSelectedKey() == "WolfSSL"))
    netPresUseWolfSSL.setDependencies(netPresEncProviderMenu, ["NET_PRES_ENC_PROVIDE"])     
    
    net_PresComponent.setDependencyEnabled("Net_Pres_SNTP_Dependency", False)
    
    # Generate Encryption Provider Stub Code?
    netPresGenEncryptStub = net_PresComponent.createBooleanSymbol("NET_PRES_GENERATE_ENC_STUBS", netPresEncryptEnable)
    netPresGenEncryptStub.setLabel("Generate Encryption Provider Stub Code?")
    netPresGenEncryptStub.setVisible(False)
    netPresGenEncryptStub.setDefaultValue((netPresEncryptionProvider.getSelectedKey() == "Generate Stub"))

    netPresUseWolfSSLDebug = net_PresComponent.createBooleanSymbol("NET_PRES_USE_WOLF_SSL_DEBUG_LOG", netPresEncryptEnable)
    netPresUseWolfSSLDebug.setLabel("Add system console logging for wolf ssl?")
    netPresUseWolfSSLDebug.setVisible(False)
    netPresUseWolfSSLDebug.setDefaultValue(False)
    netPresUseWolfSSLDebug.setDependencies(netPresWolfSSLDebugEnable, ["NET_PRES_USE_WOLF_SSL", "lib_wolfssl.wolfssl_debug"])  

    netPresWolfSSLLogBuffNum = net_PresComponent.createIntegerSymbol("NET_PRES_USE_WOLF_SSL_DEBUG_LOG_BUFFERS", netPresUseWolfSSLDebug)
    netPresWolfSSLLogBuffNum.setVisible(False)
    netPresWolfSSLLogBuffNum.setLabel("Number of log message buffers")
    netPresWolfSSLLogBuffNum.setDefaultValue(80)
    netPresWolfSSLLogBuffNum.setDependencies(netPresEncryptMenuVisible, ["NET_PRES_USE_WOLF_SSL_DEBUG_LOG"])  
    
    # Support Stream Encryption?
    netPresStreamEncrypt = net_PresComponent.createBooleanSymbol("NET_PRES_SUPPORT_STREAM_ENC", netPresEncryptEnable)
    netPresStreamEncrypt.setLabel("Support Stream Encryption?")
    netPresStreamEncrypt.setVisible(False)
    netPresStreamEncrypt.setDefaultValue(True)
    netPresStreamEncrypt.setDependencies(netPresEncryptMenuVisible, ["NET_PRES_SUPPORT_ENCRYPTION"])
    
    # Support Data-gram Encryption?
    netPresDataGramEncrypt = net_PresComponent.createBooleanSymbol("NET_PRES_SUPPORT_DATAGRAM_ENC", netPresEncryptEnable)
    netPresDataGramEncrypt.setLabel("Support Data-gram Encryption?")
    netPresDataGramEncrypt.setVisible(False)
    netPresDataGramEncrypt.setDefaultValue(False)
    netPresDataGramEncrypt.setDependencies(netPresEncryptMenuVisible, ["NET_PRES_SUPPORT_ENCRYPTION"])
        
    # Support Server Encryption?
    netPresServerEncrypt = net_PresComponent.createBooleanSymbol("NET_PRES_SUPPORT_SERVER_ENC", netPresEncryptEnable)
    netPresServerEncrypt.setLabel("Support Server Encryption?")
    netPresServerEncrypt.setVisible(False)
    netPresServerEncrypt.setDefaultValue(False)
    netPresServerEncrypt.setDependencies(netPresEncryptMenuVisible, ["NET_PRES_SUPPORT_ENCRYPTION"])
            
    # Support Client Encryption?
    netPresClientEncrypt = net_PresComponent.createBooleanSymbol("NET_PRES_SUPPORT_CLIENT_ENC",netPresEncryptEnable)
    netPresClientEncrypt.setLabel("Support Client Encryption?")
    netPresClientEncrypt.setVisible(False)
    netPresClientEncrypt.setDefaultValue(True)
    netPresClientEncrypt.setDependencies(netPresEncryptMenuVisible, ["NET_PRES_SUPPORT_ENCRYPTION"])

    # Support SNI?
    netPresSNI = net_PresComponent.createBooleanSymbol("NET_PRES_SUPPORT_SNI",netPresEncryptEnable)
    netPresSNI.setLabel("Support SNI?")
    netPresSNI.setVisible(False)
    netPresSNI.setDefaultValue(False)
    netPresSNI.setDependencies(netPresWolfSSLSNI, ["NET_PRES_SUPPORT_SNI"])

    # Host Name for SNI?
    netPresSNIHostName = net_PresComponent.createStringSymbol("NET_PRES_SUPPORT_SNI_HOST_NAME", netPresSNI) 
    netPresSNIHostName.setLabel("SNI Host name")
    netPresSNIHostName.setVisible(False)
    netPresSNIHostName.setDescription("SNI host name")
    netPresSNIHostName.setDefaultValue("microchip.com")
    
    # Support ALPN?
    netPresALPN = net_PresComponent.createBooleanSymbol("NET_PRES_SUPPORT_ALPN",netPresEncryptEnable)
    netPresALPN.setLabel("Support ALPN?")
    netPresALPN.setVisible(False)
    netPresALPN.setDefaultValue(False)
    netPresALPN.setDependencies(netPresWolfSSLALPN, ["NET_PRES_SUPPORT_ALPN"])
    
    # Protocol Name for ALPN?
    netPresALPNProtocolName = net_PresComponent.createStringSymbol("NET_PRES_SUPPORT_ALPN_PROTOCOL_NAME", netPresALPN) 
    netPresALPNProtocolName.setLabel("ALPN Protocol Name List")
    netPresALPNProtocolName.setVisible(False)
    netPresALPNProtocolName.setDescription("ALPN protocol name")
    netPresALPNProtocolName.setDefaultValue("x-amzn-mqtt-ca")
    
    netPresBlobCert = net_PresComponent.createKeyValueSetSymbol("NET_PRES_BLOB_CERT",netPresEncryptEnable)
    netPresBlobCert.setVisible(False)
    netPresBlobCert.setLabel("Encryption Certificate Store")
    netPresBlobCert.addKey("Fixed Flash Based Certificate Repo", "0", "Use Fixed Flash Based Certificate Repository for Encryption")
    netPresBlobCert.addKey("Generate Certificate Store Stubs", "1", "Generate Certificate Store Stubs")
    netPresBlobCert.setDisplayMode("Key")
    netPresBlobCert.setOutputMode("Key")
    netPresBlobCert.setDefaultValue(1)    
    netPresBlobCert.setDependencies(netPresEncryptMenuVisible, ["NET_PRES_SUPPORT_ENCRYPTION"])
    
    # Use Fixed Flash Based Certificate Repository for Encryption?
    netPresBlobCertRepo = net_PresComponent.createBooleanSymbol("NET_PRES_BLOB_CERT_REPO", None)
    netPresBlobCertRepo.setLabel("Use Fixed Flash Based Certificate Repository for Encryption?")
    netPresBlobCertRepo.setVisible(False)
    netPresBlobCertRepo.setDescription("Use Fixed Flash Based Certificate Repository for Encryption?")
    netPresBlobCertRepo.setDefaultValue(False)
    netPresBlobCertRepo.setDependencies(netPresBlobCertMenu, ["NET_PRES_BLOB_CERT"])
    
    # Generate Certificate Store Stubs?
    netPresGenCertStub = net_PresComponent.createBooleanSymbol("NET_PRES_CERT_STORE_STUBS", None)
    netPresGenCertStub.setLabel("Generate Certificate Store Stubs?")
    netPresGenCertStub.setVisible(False)
    netPresGenCertStub.setDescription("Generate Certificate Store Stubs?")
    netPresGenCertStub.setDefaultValue(False)
    # netPresGenCertStub.setDependencies(netPresBlobCertMenu, ["NET_PRES_BLOB_CERT"])
    
    # Enable Peer Certificates Verification?
    netPresBlobPeerCertVerify = net_PresComponent.createBooleanSymbol("NET_PRES_BLOB_ENABLE_PEER_CERT_VERIFICATION", netPresBlobCert)
    netPresBlobPeerCertVerify.setLabel("Enable Peer Certificates Verification")
    netPresBlobPeerCertVerify.setVisible(False)
    netPresBlobPeerCertVerify.setDescription("Enable Peer Certificates Verification")
    netPresBlobPeerCertVerify.setDefaultValue(False)  
    netPresBlobPeerCertVerify.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_CERT_REPO"])

    # Support Client Certificates?
    netPresBlobClientSupport = net_PresComponent.createBooleanSymbol("NET_PRES_BLOB_CLIENT_SUPPORT", netPresBlobCert)
    netPresBlobClientSupport.setLabel("Support Client Certificates?")
    netPresBlobClientSupport.setVisible(False)
    netPresBlobClientSupport.setDescription("Support Client Certificates?")
    netPresBlobClientSupport.setDefaultValue(True)  
    netPresBlobClientSupport.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_CERT_REPO"])

    # File Type for Client CA Certificate?
    netPresBlobClientCertFormat = net_PresComponent.createKeyValueSetSymbol("NET_PRES_BLOB_CLIENT_CERT_FORMAT",netPresBlobClientSupport)
    netPresBlobClientCertFormat.setVisible(True)
    netPresBlobClientCertFormat.setLabel("CA Certificate Format")
    netPresBlobClientCertFormat.addKey("PEM", "0", "PEM")
    netPresBlobClientCertFormat.addKey("ASN1", "1", "ASN1")
    netPresBlobClientCertFormat.setDisplayMode("Key")
    netPresBlobClientCertFormat.setOutputMode("Key")
    netPresBlobClientCertFormat.setDefaultValue(0)
    netPresBlobClientCertFormat.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_CLIENT_SUPPORT"]) 
    
    # File name containing definitions for Client Certificates?
    netPresBlobClientCertFileName = net_PresComponent.createStringSymbol("NET_PRES_BLOB_CLIENT_CERT_FILENAME", netPresBlobClientSupport) 
    netPresBlobClientCertFileName.setLabel("CA Certificate definition file name")
    netPresBlobClientCertFileName.setVisible(True)
    netPresBlobClientCertFileName.setDescription("CA Certificate Definition file name")
    netPresBlobClientCertFileName.setDefaultValue("ca-certs.h")
    netPresBlobClientCertFileName.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_CLIENT_SUPPORT"]) 

    # Variable Name Containing Data for Client Certificates?
    netPresBlobClientCertVar = net_PresComponent.createStringSymbol("NET_PRES_BLOB_CLIENT_CERT_VARIABLE", netPresBlobClientSupport) 
    netPresBlobClientCertVar.setLabel("CA Certificate data variable name")
    netPresBlobClientCertVar.setVisible(True)
    netPresBlobClientCertVar.setDescription("CA Certificate Data variable name")
    netPresBlobClientCertVar.setDefaultValue("caCert")
    netPresBlobClientCertVar.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_CLIENT_SUPPORT"])  

    # Variable Name Containing Size of Client Certificates?
    netPresBlobClientCertLenVar = net_PresComponent.createStringSymbol("NET_PRES_BLOB_CLIENT_CERT_LEN_VARIABLE", netPresBlobClientSupport) 
    netPresBlobClientCertLenVar.setLabel("CA Certificate Size variable name")
    netPresBlobClientCertLenVar.setVisible(True)
    netPresBlobClientCertLenVar.setDescription("CA Certificate Size variable name")
    netPresBlobClientCertLenVar.setDefaultValue("caCert_len")
    netPresBlobClientCertLenVar.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_CLIENT_SUPPORT"])


    # Support Runtime Addition of Certificates?
    netPresBlobRuntimeCertAdd = net_PresComponent.createBooleanSymbol("NET_PRES_BLOB_RUNTIME_CERT_SUPPORT", netPresBlobClientSupport)
    netPresBlobRuntimeCertAdd.setLabel("Support Runtime Certificates Addition?")
    netPresBlobRuntimeCertAdd.setVisible(True)
    netPresBlobRuntimeCertAdd.setDescription("Support Runtime Certificates Addition?")
    netPresBlobRuntimeCertAdd.setDefaultValue(False)  
    netPresBlobRuntimeCertAdd.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_CLIENT_SUPPORT"])

    # Maximum Community Support
    netPresBlobMoreClientCertMax = net_PresComponent.createIntegerSymbol("NET_PRES_BLOB_MORE_CLIENT_CERT", netPresBlobClientSupport)
    netPresBlobMoreClientCertMax.setHelp("mcc_h3_more_client_cert_configurations")
    netPresBlobMoreClientCertMax.setLabel("More CA Certificate Entries")
    netPresBlobMoreClientCertMax.setMax(netPresMaxCaCertNum)
    netPresBlobMoreClientCertMax.setMin(0)
    netPresBlobMoreClientCertMax.setVisible(True)
    netPresBlobMoreClientCertMax.setDescription("Maximum Client Certificate Support")
    netPresBlobMoreClientCertMax.setDefaultValue(netPresMoreClientCertConfigNumPrev)
    
####-----------------------------------------------------------------------------------------##########
    for index in range(0,netPresMaxCaCertNum):  
        netPresMaxCaCertConfigIdx.append(net_PresComponent.createBooleanSymbol("NET_PRES_BLOB_MORE_CLIENT_CERT_CONFIG_IDX"+str(index),netPresBlobMoreClientCertMax))
        netPresMaxCaCertConfigIdx[index].setHelp("mcc_h3_more_client_cert_configurations")
        netPresMaxCaCertConfigIdx[index].setLabel("Client Certificate "+ str(index))
        netPresMaxCaCertConfigIdx[index].setVisible(True)
        if (index < netPresBlobMoreClientCertMax.getValue()):  
            netPresMaxCaCertConfigIdx[index].setDefaultValue(True)
        else:
            netPresMaxCaCertConfigIdx[index].setDefaultValue(False)
            
        netPresMaxCaCertConfigIdx[index].setDependencies(netPresMoreClientCertConfig, ["NET_PRES_BLOB_CLIENT_SUPPORT", "NET_PRES_BLOB_MORE_CLIENT_CERT"])
#        netPresMaxCaCertConfigIdx[index].setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_MORE_CLIENT_CERT"])
            
        # File Type for Client CA Certificate?
        netPresBlobMoreClientCertFormat.append(net_PresComponent.createKeyValueSetSymbol("NET_PRES_BLOB_MORE_CLIENT_CERT_FORMAT_IDX" + str(index),netPresMaxCaCertConfigIdx[index]))
        netPresBlobMoreClientCertFormat[index].setVisible(True)
        netPresBlobMoreClientCertFormat[index].setLabel("CA Certificate Format")
        netPresBlobMoreClientCertFormat[index].addKey("PEM", "0", "PEM")
        netPresBlobMoreClientCertFormat[index].addKey("ASN1", "1", "ASN1")
        netPresBlobMoreClientCertFormat[index].setDisplayMode("Key")
        netPresBlobMoreClientCertFormat[index].setOutputMode("Key")
        netPresBlobMoreClientCertFormat[index].setDefaultValue(0)
        netPresBlobMoreClientCertFormat[index].setDependencies(netPresMenuVisible, [netPresMaxCaCertConfigIdx[index].getID()])
        
        # File Type for Client CA Certificate Dummy?
        netPresBlobMoreClientCertFileName.append(net_PresComponent.createStringSymbol("NET_PRES_BLOB_MORE_CLIENT_CERT_FILENAME_IDX" + str(index),netPresMaxCaCertConfigIdx[index]))
        netPresBlobMoreClientCertFileName[index].setVisible(True)
        netPresBlobMoreClientCertFileName[index].setLabel("CA Certificate definition file name")
        netPresBlobMoreClientCertFileName[index].setDefaultValue("ca-certs.h")
        netPresBlobMoreClientCertFileName[index].setDependencies(netPresMenuVisible, [netPresMaxCaCertConfigIdx[index].getID()])
        
        # Variable Name Containing Data for Client Certificates?
        netPresBlobMoreClientCertVar.append(net_PresComponent.createStringSymbol("NET_PRES_BLOB_MORE_CLIENT_CERT_VARIABLE_IDX" + str(index), netPresMaxCaCertConfigIdx[index])) 
        netPresBlobMoreClientCertVar[index].setLabel("CA Certificate data variable name")
        netPresBlobMoreClientCertVar[index].setVisible(True)
        netPresBlobMoreClientCertVar[index].setDescription("CA Certificate Data variable name")
        netPresBlobMoreClientCertVar[index].setDefaultValue("caCert")
        netPresBlobMoreClientCertVar[index].setDependencies(netPresMenuVisible, [netPresMaxCaCertConfigIdx[index].getID()])  

        # Variable Name Containing Size of Client Certificates?
        netPresBlobMoreClientCertLenVar.append(net_PresComponent.createStringSymbol("NET_PRES_BLOB_MORE_CLIENT_CERT_LEN_VARIABLE_IDX" + str(index), netPresMaxCaCertConfigIdx[index])) 
        netPresBlobMoreClientCertLenVar[index].setLabel("CA Certificate Size variable name")
        netPresBlobMoreClientCertLenVar[index].setVisible(True)
        netPresBlobMoreClientCertLenVar[index].setDescription("CA Certificate Size variable name")
        netPresBlobMoreClientCertLenVar[index].setDefaultValue("caCert_len")
        netPresBlobMoreClientCertLenVar[index].setDependencies(netPresMenuVisible, [netPresMaxCaCertConfigIdx[index].getID()])
####-----------------------------------------------------------------------------------------##########    


     # Support X509 TLS Mutual Authentication?
    netPresBlobClientMutualAuthSupport = net_PresComponent.createBooleanSymbol("NET_PRES_BLOB_CLIENT_MUTUAL_AUTH_SUPPORT", netPresBlobClientSupport)
    netPresBlobClientMutualAuthSupport.setLabel("Support X509 TLS Mutual Authentication?")
    netPresBlobClientMutualAuthSupport.setVisible(True)
    netPresBlobClientMutualAuthSupport.setDescription("Support X509 TLS Mutual Authentication?")
    netPresBlobClientMutualAuthSupport.setDefaultValue(False)
    netPresBlobClientMutualAuthSupport.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_CLIENT_SUPPORT"])

    # File Type for X509 TLS Credentials
    netPresBlobClientMutualAuthFormat = net_PresComponent.createKeyValueSetSymbol("NET_PRES_BLOB_CLIENT_MUTUAL_AUTH_FORMAT",netPresBlobClientMutualAuthSupport)
    netPresBlobClientMutualAuthFormat.setVisible(False)
    netPresBlobClientMutualAuthFormat.setLabel("Device Certificate and Private Key Format")
    netPresBlobClientMutualAuthFormat.addKey("PEM", "0", "PEM")
    netPresBlobClientMutualAuthFormat.addKey("ASN1", "1", "ASN1")
    netPresBlobClientMutualAuthFormat.setDisplayMode("Key")
    netPresBlobClientMutualAuthFormat.setOutputMode("Key")
    netPresBlobClientMutualAuthFormat.setDefaultValue(0)
    netPresBlobClientMutualAuthFormat.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_CLIENT_MUTUAL_AUTH_SUPPORT"])

    # File name containing definitions for TLS Mutual Authnetication Credentials?
    netPresBlobClientMutualAuthFileName = net_PresComponent.createStringSymbol("NET_PRES_BLOB_CLIENT_MUTUAL_AUTH_FILENAME", netPresBlobClientMutualAuthSupport) 
    netPresBlobClientMutualAuthFileName.setLabel("File name containing definitions for TLS Credentials?")
    netPresBlobClientMutualAuthFileName.setVisible(False)
    netPresBlobClientMutualAuthFileName.setDescription("File name containing definitions for TLS Credentials?")
    netPresBlobClientMutualAuthFileName.setDefaultValue("device-certs.h")
    netPresBlobClientMutualAuthFileName.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_CLIENT_MUTUAL_AUTH_SUPPORT"])

    # Variable Name Containing Type of Device certificate?
    netPresBlobClientDeviceCertIsChainCertVar = net_PresComponent.createBooleanSymbol("NET_PRES_BLOB_CLIENT_IS_DEVICE_CERT_CHAIN", netPresBlobClientMutualAuthSupport) 
    netPresBlobClientDeviceCertIsChainCertVar.setLabel("Is Device Certificate a Chain Certificate in PEM format?")
    netPresBlobClientDeviceCertIsChainCertVar.setVisible(True)
    netPresBlobClientDeviceCertIsChainCertVar.setDescription("Device Certificate is a Chain Certificate? Only PEM chain certificate  is supported! Device Private Key can be either PEM or ASN1(DER)")
    netPresBlobClientDeviceCertIsChainCertVar.setDefaultValue(False)
    netPresBlobClientDeviceCertIsChainCertVar.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_CLIENT_MUTUAL_AUTH_SUPPORT"])


    # Variable Name Containing Data for TLS Device Certificate?
    netPresBlobClientDeviceCertVar = net_PresComponent.createStringSymbol("NET_PRES_BLOB_CLIENT_DEVICE_CERT_VARIABLE", netPresBlobClientMutualAuthSupport) 
    netPresBlobClientDeviceCertVar.setLabel("Variable Name Containing Data for Device Certificate?")
    netPresBlobClientDeviceCertVar.setVisible(False)
    netPresBlobClientDeviceCertVar.setDescription("Variable Name Containing Data for Device Certificate?")
    netPresBlobClientDeviceCertVar.setDefaultValue("deviceCert")
    netPresBlobClientDeviceCertVar.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_CLIENT_MUTUAL_AUTH_SUPPORT"])

    # Variable Name Containing Size of TLS Device Certificate?
    netPresBlobClientDeviceCertLenVar = net_PresComponent.createStringSymbol("NET_PRES_BLOB_CLIENT_DEVICE_CERT_LEN_VARIABLE", netPresBlobClientMutualAuthSupport) 
    netPresBlobClientDeviceCertLenVar.setLabel("Variable Name Containing Size of Device Certificate?")
    netPresBlobClientDeviceCertLenVar.setVisible(False)
    netPresBlobClientDeviceCertLenVar.setDescription("Variable Name Containing Size of Device Certificate?")
    netPresBlobClientDeviceCertLenVar.setDefaultValue("deviceCert_len")
    netPresBlobClientDeviceCertLenVar.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_CLIENT_MUTUAL_AUTH_SUPPORT"])

    # Variable Name Containing Data for TLS Device Private Key?
    netPresBlobClientDevicePvtKeyVar = net_PresComponent.createStringSymbol("NET_PRES_BLOB_CLIENT_DEVICE_PVT_KEY_VARIABLE", netPresBlobClientMutualAuthSupport) 
    netPresBlobClientDevicePvtKeyVar.setLabel("Variable Name Containing Data for Device Private Key?")
    netPresBlobClientDevicePvtKeyVar.setVisible(False)
    netPresBlobClientDevicePvtKeyVar.setDescription("Variable Name Containing Data for Device Private Key?")
    netPresBlobClientDevicePvtKeyVar.setDefaultValue("devicePvtKey")
    netPresBlobClientDevicePvtKeyVar.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_CLIENT_MUTUAL_AUTH_SUPPORT"])

    # Variable Name Containing Size of TLS Device Private Key?
    netPresBlobClientDevicePvtKeyLenVar = net_PresComponent.createStringSymbol("NET_PRES_BLOB_CLIENT_DEVICE_PVT_KEY_LEN_VARIABLE", netPresBlobClientMutualAuthSupport) 
    netPresBlobClientDevicePvtKeyLenVar.setLabel("Variable Name Containing Size of Device Private Key?")
    netPresBlobClientDevicePvtKeyLenVar.setVisible(False)
    netPresBlobClientDevicePvtKeyLenVar.setDescription("Variable Name Containing Size of Device Private Key?")
    netPresBlobClientDevicePvtKeyLenVar.setDefaultValue("devicePvtKey_len")
    netPresBlobClientDevicePvtKeyLenVar.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_CLIENT_MUTUAL_AUTH_SUPPORT"])

    # Support Server Certificate?"
    netPresBlobServerSupport = net_PresComponent.createBooleanSymbol("NET_PRES_BLOB_SERVER_SUPPORT", netPresBlobCert)
    netPresBlobServerSupport.setLabel("Support Server Certificate?")
    netPresBlobServerSupport.setVisible(False)
    netPresBlobServerSupport.setDescription("Support Server Certificate?")
    netPresBlobServerSupport.setDefaultValue(True)  
    netPresBlobServerSupport.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_CERT_REPO"])

    # File Type for Server Credentials
    netPresBlobServerCertFormat = net_PresComponent.createKeyValueSetSymbol("NET_PRES_BLOB_SERVER_CERT_FORMAT",netPresBlobServerSupport)
    netPresBlobServerCertFormat.setVisible(True)
    netPresBlobServerCertFormat.setLabel("Server Certificate and Private Key Format")
    netPresBlobServerCertFormat.addKey("PEM", "0", "PEM")
    netPresBlobServerCertFormat.addKey("ASN1", "1", "ASN1")
    netPresBlobServerCertFormat.setDisplayMode("Key")
    netPresBlobServerCertFormat.setOutputMode("Key")
    netPresBlobServerCertFormat.setDefaultValue(0)
    netPresBlobServerCertFormat.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_SERVER_SUPPORT"])

    # File name containing definitions for Server Certificates?
    netPresBlobServerCertFileName = net_PresComponent.createStringSymbol("NET_PRES_BLOB_SERVER_CERT_FILENAME", netPresBlobServerSupport) 
    netPresBlobServerCertFileName.setLabel("Server Certificates Definitions File Name")
    netPresBlobServerCertFileName.setVisible(True)
    netPresBlobServerCertFileName.setDescription("File name containing definitions for Server Certificates?")
    netPresBlobServerCertFileName.setDefaultValue("ca-certs.h")
    netPresBlobServerCertFileName.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_SERVER_SUPPORT"]) 

    # Variable Name Containing Data for Server Certificates?
    netPresBlobServerCertVar = net_PresComponent.createStringSymbol("NET_PRES_BLOB_SERVER_CERT_VARIABLE", netPresBlobServerSupport) 
    netPresBlobServerCertVar.setLabel("Server Certificates Data Variable Name")
    netPresBlobServerCertVar.setVisible(True)
    netPresBlobServerCertVar.setDescription("Variable Name Containing Data for Server Certificates?")
    netPresBlobServerCertVar.setDefaultValue("serverCert")
    netPresBlobServerCertVar.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_SERVER_SUPPORT"])  

    # Variable Name Containing Size of Server Certificates?
    netPresBlobServerCertLenVar = net_PresComponent.createStringSymbol("NET_PRES_BLOB_SERVER_CERT_LEN_VARIABLE", netPresBlobServerSupport) 
    netPresBlobServerCertLenVar.setLabel("Server Certificates Size Variable Name")
    netPresBlobServerCertLenVar.setVisible(True)
    netPresBlobServerCertLenVar.setDescription("Variable Name Containing Size of Server Certificates?")
    netPresBlobServerCertLenVar.setDefaultValue("serverCert_len")
    netPresBlobServerCertLenVar.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_SERVER_SUPPORT"])

    # File name containing definitions for Server Private Key?
    netPresBlobServerKeyFileName = net_PresComponent.createStringSymbol("NET_PRES_BLOB_SERVER_KEY_FILENAME", netPresBlobServerSupport) 
    netPresBlobServerKeyFileName.setLabel("Server Private Key Definitions File Name")
    netPresBlobServerKeyFileName.setVisible(True)
    netPresBlobServerKeyFileName.setDescription("File name containing definitions for Server Private Key?")
    netPresBlobServerKeyFileName.setDefaultValue("ca-certs.h")
    netPresBlobServerKeyFileName.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_SERVER_SUPPORT"])
    
    # Variable Name Containing Data for Server Private Key?
    netPresBlobServerKeyVar = net_PresComponent.createStringSymbol("NET_PRES_BLOB_SERVER_KEY_VARIABLE", netPresBlobServerSupport) 
    netPresBlobServerKeyVar.setLabel("Server Private Key Data Variable Name")
    netPresBlobServerKeyVar.setVisible(True)
    netPresBlobServerKeyVar.setDescription("Variable Name Containing Data for Server Private Key?")
    netPresBlobServerKeyVar.setDefaultValue("serverKey")
    netPresBlobServerKeyVar.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_SERVER_SUPPORT"])
    
    # Variable Name Containing Size of Server Private Key?
    netPresBlobServerKeyLen = net_PresComponent.createStringSymbol("NET_PRES_BLOB_SERVER_KEY_LEN_VARIABLE", netPresBlobServerSupport) 
    netPresBlobServerKeyLen.setLabel("Server Private Key Size Variable Name")
    netPresBlobServerKeyLen.setVisible(True)
    netPresBlobServerKeyLen.setDescription("Variable Name Containing Size of Server Private Key?")
    netPresBlobServerKeyLen.setDefaultValue("serverKey_len")
    netPresBlobServerKeyLen.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_SERVER_SUPPORT"])
    
    # Generate Client Certificate Stubs?
    netPresCertStoreStubClient = net_PresComponent.createBooleanSymbol("NET_PRES_CERT_STORE_STUBS_CLIENT", netPresBlobCert)
    netPresCertStoreStubClient.setLabel("Generate Client Certificate Stubs?")
    netPresCertStoreStubClient.setVisible(True)
    netPresCertStoreStubClient.setDescription("Generate Client Certificate Stubs?")
    netPresCertStoreStubClient.setDefaultValue(True)    
    netPresCertStoreStubClient.setDependencies(netPresMenuVisible, ["NET_PRES_CERT_STORE_STUBS"])   
    
    # Generate Server Certificate Stubs?
    netPresCertStoreStubServer = net_PresComponent.createBooleanSymbol("NET_PRES_CERT_STORE_STUBS_SERVER", netPresBlobCert)
    netPresCertStoreStubServer.setLabel("Generate Server Certificate Stubs?")
    netPresCertStoreStubServer.setVisible(True)
    netPresCertStoreStubServer.setDescription("Generate Server Certificate Stubs?")
    netPresCertStoreStubServer.setDefaultValue(True)    
    netPresCertStoreStubServer.setDependencies(netPresMenuVisible, ["NET_PRES_CERT_STORE_STUBS"])           

    # file NET_PRES1_H "$HARMONY_VERSION_PATH/framework/net_pres/pres/net_pres.h"  to "$PROJECT_HEADER_FILES/framework/net_pres/pres/net_pres.h"
    netPresHeaderFile = net_PresComponent.createFileSymbol(None, None)
    netPresHeaderFile.setSourcePath("net_pres/pres/net_pres.h")
    netPresHeaderFile.setOutputName("net_pres.h")
    netPresHeaderFile.setDestPath("net_pres/pres/")
    netPresHeaderFile.setProjectPath("config/" + configName + "/net_pres/pres/")
    netPresHeaderFile.setType("HEADER")
    netPresHeaderFile.setOverwrite(True)
    
    # file NET_PRES2_H "$HARMONY_VERSION_PATH/framework/net_pres/pres/net_pres_certstore.h"  to "$PROJECT_HEADER_FILES/framework/net_pres/pres/net_pres_certstore.h"
    netPresCertStoreHeaderFile = net_PresComponent.createFileSymbol(None, None)
    netPresCertStoreHeaderFile.setSourcePath("net_pres/pres/net_pres_certstore.h")
    netPresCertStoreHeaderFile.setOutputName("net_pres_certstore.h")
    netPresCertStoreHeaderFile.setDestPath("net_pres/pres/")
    netPresCertStoreHeaderFile.setProjectPath("config/" + configName + "/net_pres/pres/")
    netPresCertStoreHeaderFile.setType("HEADER")
    netPresCertStoreHeaderFile.setOverwrite(True)   
    
    # file NET_PRES3_H "$HARMONY_VERSION_PATH/framework/net_pres/pres/net_pres_encryptionproviderapi.h"  to "$PROJECT_HEADER_FILES/framework/net_pres/pres/net_pres_encryptionproviderapi.h"
    netPresEncryptApiHeaderFile = net_PresComponent.createFileSymbol(None, None)
    netPresEncryptApiHeaderFile.setSourcePath("net_pres/pres/net_pres_encryptionproviderapi.h")
    netPresEncryptApiHeaderFile.setOutputName("net_pres_encryptionproviderapi.h")
    netPresEncryptApiHeaderFile.setDestPath("net_pres/pres/")
    netPresEncryptApiHeaderFile.setProjectPath("config/" + configName + "/net_pres/pres/")
    netPresEncryptApiHeaderFile.setType("HEADER")
    netPresEncryptApiHeaderFile.setOverwrite(True)      
    
    # file NET_PRES4_H "$HARMONY_VERSION_PATH/framework/net_pres/pres/net_pres_socketapi.h"  to "$PROJECT_HEADER_FILES/framework/net_pres/pres/net_pres_socketapi.h"
    netPresSocketApiHeaderFile = net_PresComponent.createFileSymbol(None, None)
    netPresSocketApiHeaderFile.setSourcePath("net_pres/pres/net_pres_socketapi.h")
    netPresSocketApiHeaderFile.setOutputName("net_pres_socketapi.h")
    netPresSocketApiHeaderFile.setDestPath("net_pres/pres/")
    netPresSocketApiHeaderFile.setProjectPath("config/" + configName + "/net_pres/pres/")
    netPresSocketApiHeaderFile.setType("HEADER")
    netPresSocketApiHeaderFile.setOverwrite(True)       
    
    # file NET_PRES5_H "$HARMONY_VERSION_PATH/framework/net_pres/pres/net_pres_transportapi.h"  to "$PROJECT_HEADER_FILES/framework/net_pres/pres/net_pres_transportapi.h"
    netPresTransApiHeaderFile = net_PresComponent.createFileSymbol(None, None)
    netPresTransApiHeaderFile.setSourcePath("net_pres/pres/net_pres_transportapi.h")
    netPresTransApiHeaderFile.setOutputName("net_pres_transportapi.h")
    netPresTransApiHeaderFile.setDestPath("net_pres/pres/")
    netPresTransApiHeaderFile.setProjectPath("config/" + configName + "/net_pres/pres/")
    netPresTransApiHeaderFile.setType("HEADER")
    netPresTransApiHeaderFile.setOverwrite(True)        
    
    # file NET_PRES_LOCAL_H "$HARMONY_VERSION_PATH/framework/net_pres/pres/src/net_pres_local.h"  to "$PROJECT_HEADER_FILES/framework/net_pres/pres/src/net_pres_local.h"
    netPresLocalHeaderFile = net_PresComponent.createFileSymbol(None, None)
    netPresLocalHeaderFile.setSourcePath("net_pres/pres/src/net_pres_local.h")
    netPresLocalHeaderFile.setOutputName("net_pres_local.h")
    netPresLocalHeaderFile.setDestPath("net_pres/pres/src/")
    netPresLocalHeaderFile.setProjectPath("config/" + configName + "/net_pres/pres/src/")
    netPresLocalHeaderFile.setType("HEADER")
    netPresLocalHeaderFile.setOverwrite(True)       
        
    # file NET_PRES_SOCKETAPICONVERSION_H "$HARMONY_VERSION_PATH/framework/net_pres/pres/net_pres_socketapiconversion.h"  to "$PROJECT_HEADER_FILES/framework/net_pres/pres/net_pres_socketapiconversion.h"
    netPresSktApiConvHeaderFile = net_PresComponent.createFileSymbol(None, None)
    netPresSktApiConvHeaderFile.setSourcePath("net_pres/pres/net_pres_socketapiconversion.h")
    netPresSktApiConvHeaderFile.setOutputName("net_pres_socketapiconversion.h")
    netPresSktApiConvHeaderFile.setDestPath("net_pres/pres/")
    netPresSktApiConvHeaderFile.setProjectPath("config/" + configName + "/net_pres/pres/")
    netPresSktApiConvHeaderFile.setType("HEADER")
    netPresSktApiConvHeaderFile.setOverwrite(True)  

    # add "<#include \"/framework/net/templates/system_init.c.data.ftl\">"  to list SYSTEM_INIT_C_MODULE_INITIALIZATION_DATA
    netPresSysInitDataSourceFtl = net_PresComponent.createFileSymbol(None, None)
    netPresSysInitDataSourceFtl.setType("STRING")
    netPresSysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    netPresSysInitDataSourceFtl.setSourcePath("net_pres/pres/templates_net_pres/system/system_data_initialize.c.ftl")
    netPresSysInitDataSourceFtl.setMarkup(True)

    netPresSystemDefFile = net_PresComponent.createFileSymbol(None, None)
    netPresSystemDefFile.setType("STRING")
    netPresSystemDefFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    netPresSystemDefFile.setSourcePath("net_pres/pres/templates_net_pres/system/system_definitions.h.ftl")
    netPresSystemDefFile.setMarkup(True)
    
    netPresSystemDefObjFile = net_PresComponent.createFileSymbol(None, None)
    netPresSystemDefObjFile.setType("STRING")
    netPresSystemDefObjFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_OBJECTS")
    netPresSystemDefObjFile.setSourcePath("net_pres/pres/templates_net_pres/system/system_definitions_object.h.ftl")
    netPresSystemDefObjFile.setMarkup(True)
    
    netPresSystemConfigFtl = net_PresComponent.createFileSymbol(None, None)
    netPresSystemConfigFtl.setSourcePath("net_pres/pres/templates_net_pres/system/system_config.h.ftl")
    netPresSystemConfigFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    netPresSystemConfigFtl.setMarkup(True)
    netPresSystemConfigFtl.setType("STRING")
    
    # add "<#include \"/framework/net/templates/system_tasks.c.ftl\">"  to list SYSTEM_TASKS_C_CALL_LIB_TASKS
    netPresSysTaskSourceFtl = net_PresComponent.createFileSymbol(None, None)
    netPresSysTaskSourceFtl.setType("STRING")
    netPresSysTaskSourceFtl.setOutputName("core.LIST_SYSTEM_TASKS_C_CALL_LIB_TASKS")
    netPresSysTaskSourceFtl.setSourcePath("net_pres/pres/templates_net_pres/system/system_tasks.c.ftl")
    netPresSysTaskSourceFtl.setMarkup(True)

    netPresSystemRtosTasksFile = net_PresComponent.createFileSymbol("NET_PRES_SYS_RTOS_TASK", None)
    netPresSystemRtosTasksFile.setType("STRING")
    netPresSystemRtosTasksFile.setOutputName("core.LIST_SYSTEM_RTOS_TASKS_C_DEFINITIONS")
    netPresSystemRtosTasksFile.setSourcePath("net_pres/pres/templates_net_pres/system/system_rtos_tasks.c.ftl")
    netPresSystemRtosTasksFile.setMarkup(True)
    netPresSystemRtosTasksFile.setEnabled((Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != "BareMetal"))
    netPresSystemRtosTasksFile.setDependencies(genRtosTask, ["HarmonyCore.SELECT_RTOS"])
    
    # file NET_PRES_C "$HARMONY_VERSION_PATH/framework/net_pres/pres/src/net_pres.c" to "$PROJECT_SOURCE_FILES/framework/net_pres/pres/src/net_pres.c"
    netPresSourceFile = net_PresComponent.createFileSymbol(None, None)
    netPresSourceFile.setSourcePath("net_pres/pres/src/net_pres.c")
    netPresSourceFile.setOutputName("net_pres.c")
    netPresSourceFile.setOverwrite(True)
    netPresSourceFile.setDestPath("net_pres/pres/src/")
    netPresSourceFile.setProjectPath("config/" + configName + "/net_pres/pres/src/")
    netPresSourceFile.setType("SOURCE")
    netPresSourceFile.setEnabled(True)
        
    # template NET_PRES_ENC_GLUE_H_TEMPLATE "$HARMONY_VERSION_PATH/framework/net_pres/pres/templates/net_pres_enc_glue.h.ftl" to "$PROJECT_HEADER_FILES/app/system_config/$CONFIGURATION/framework/net_pres/pres/net_pres_enc_glue.h"
    netPresEncGlueHeaderFile = net_PresComponent.createFileSymbol(None, None)
    netPresEncGlueHeaderFile.setSourcePath("net_pres/pres/templates_net_pres/net_pres_enc_glue.h.ftl")
    netPresEncGlueHeaderFile.setOutputName("net_pres_enc_glue.h")
    netPresEncGlueHeaderFile.setMarkup(True)
    netPresEncGlueHeaderFile.setDestPath("net_pres/pres/")
    netPresEncGlueHeaderFile.setProjectPath("config/" + configName + "/net_pres/pres/")
    netPresEncGlueHeaderFile.setType("HEADER")
    netPresEncGlueHeaderFile.setOverwrite(True)
    
    # template NET_PRES_ENC_GLUE_C_TEMPLATE "$HARMONY_VERSION_PATH/framework/net_pres/pres/templates/net_pres_enc_glue.c.ftl" to "$PROJECT_SOURCE_FILES/app/system_config/$CONFIGURATION/framework/net_pres/pres/net_pres_enc_glue.c"   
    netPresEncGlueSourceFile = net_PresComponent.createFileSymbol(None, None)
    netPresEncGlueSourceFile.setSourcePath("net_pres/pres/templates_net_pres/net_pres_enc_glue.c.ftl")
    netPresEncGlueSourceFile.setOutputName("net_pres_enc_glue.c")
    netPresEncGlueSourceFile.setDestPath("net_pres/pres/")
    netPresEncGlueSourceFile.setProjectPath("config/" + configName + "/net_pres/pres/")
    netPresEncGlueSourceFile.setType("SOURCE")
    netPresEncGlueSourceFile.setMarkup(True)    

    # ifblock NET_PRES_BLOB_CERT_REPO || NET_PRES_CERT_STORE_STUBS
    # template NET_PRES_ENC_CERT_STORE_C_TEMPLATE "$HARMONY_VERSION_PATH/framework/net_pres/pres/templates/net_pres_cert_store.c.ftl" to "$PROJECT_SOURCE_FILES/app/system_config/$CONFIGURATION/framework/net_pres/pres/net_pres_cert_store.c"
    # endif 
    netPresCertStoreSourceFile = net_PresComponent.createFileSymbol(None, None)
    netPresCertStoreSourceFile.setSourcePath("net_pres/pres/templates_net_pres/net_pres_cert_store.c.ftl")
    netPresCertStoreSourceFile.setOutputName("net_pres_cert_store.c")
    netPresCertStoreSourceFile.setDestPath("net_pres/pres/")
    netPresCertStoreSourceFile.setProjectPath("config/" + configName + "/net_pres/pres/")
    netPresCertStoreSourceFile.setType("SOURCE")
    netPresCertStoreSourceFile.setMarkup(True)
    netPresCertStoreSourceFile.setEnabled(True)
    
def finalizeComponent(tcpipNetPresComponent):  
    tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
    if (tcpipAutoConfigStackGroup != None):    
        tcpipNetPresGroup = Database.findGroup("PRESENTATION LAYER")
        if (tcpipNetPresGroup == None):
            tcpipNetPresGroup = Database.createGroup("TCP/IP STACK", "PRESENTATION LAYER") 
        tcpipNetPresGroup.addComponent(tcpipNetPresComponent.getID())
        Database.setActiveGroup("PRESENTATION LAYER")
        tcpipNetPresGroup.setAttachmentVisible("net_Pres", "Net_Pres_Crypto_Dependency")
        tcpipAutoConfigStackGroup.setAttachmentVisible("PRESENTATION LAYER", "net_Pres:Net_Pres_Crypto_Dependency")
        


        
def netPresRTOSTaskDelayMenu(symbol, event):
    netPresRtos = Database.getSymbolValue("net_Pres","NET_PRES_RTOS")
    netPresRtosUseDelay = Database.getSymbolValue("net_Pres","NET_PRES_RTOS_USE_DELAY")
    if((netPresRtos == 'Standalone') and netPresRtosUseDelay):      
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)
        
def netPresMenuVisible(symbol, event):
    symbol.setVisible(event["value"])

def netPresBlobCertMenu(symbol, event): 
    data = symbol.getComponent()
    if (event["value"] == 0):        
        res = data.setSymbolValue("NET_PRES_CERT_STORE_STUBS", False)
        res = data.setSymbolValue("NET_PRES_BLOB_CERT_REPO", True)
    else:
        res = data.setSymbolValue("NET_PRES_BLOB_CERT_REPO", False)
        res = data.setSymbolValue("NET_PRES_CERT_STORE_STUBS", True)     
       
def netPresshowRTOSMenu(symbol, event):
    if (event["value"] == None):
        symbol.setVisible(False)
    elif (event["value"] != "BareMetal"):
        # If not Bare Metal
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)
        
def netPresRTOSStandaloneMenu(symbol, event):
    if (event["value"] == 'Standalone'):        
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)     

def genRtosTask(symbol, event):
    symbol.setEnabled((Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != "BareMetal"))

def netPresEncryptMenuVisible(symbol, event):
    localComponent = symbol.getComponent() 
    symbol.setVisible(event["value"])
    if(event["value"] == False):
        localComponent.setDependencyEnabled("Net_Pres_SNTP_Dependency", False)
    elif (localComponent.getSymbolValue("NET_PRES_ENC_PROVIDE") == 0):
        localComponent.setDependencyEnabled("Net_Pres_SNTP_Dependency", True)
    
def netPresEncProviderMenu(symbol, event):
    data = symbol.getComponent() 
    if (event["value"] == 0):      
        wolfssl_connect = False
        crypto_connect = False
        res = data.setSymbolValue("NET_PRES_GENERATE_ENC_STUBS", False)
        res = data.setSymbolValue("NET_PRES_USE_WOLF_SSL", True)
        if(Database.getComponentByID("lib_wolfssl") == None):
            res = Database.activateComponents(["lib_wolfssl"])
            wolfssl_connect = True
        if(Database.getComponentByID("lib_crypto") == None):
            res = Database.activateComponents(["lib_crypto"]) 
            crypto_connect = True
        if(Database.getComponentByID("lib_wolfcrypt") == None):
            res = Database.activateComponents(["lib_wolfcrypt"])
            wolfssl_connect = True
            crypto_connect = True
        if wolfssl_connect == True :
            res = Database.connectDependencies(autoConnectTableWolfssl) 
        if crypto_connect == True :
            res = Database.connectDependencies(autoConnectTableCrypto)  
         
        data.setDependencyEnabled("Net_Pres_SNTP_Dependency", True)
        #Todo: change to Database.sendMessage(); but need handleMessage() in lib_wolfssl
        Database.setSymbolValue("lib_wolfssl","wolfssl", True) 
        data.setSymbolValue("NET_PRES_BLOB_CERT", 0)
        data.setSymbolValue("NET_PRES_BLOB_CLIENT_CERT_FORMAT", 1)
        data.setSymbolValue("NET_PRES_BLOB_CLIENT_CERT_FILENAME", "wolfssl/certs_test.h")
        data.setSymbolValue("NET_PRES_BLOB_CLIENT_CERT_VARIABLE", "client_cert_der_2048")
        data.setSymbolValue("NET_PRES_BLOB_CLIENT_CERT_LEN_VARIABLE", "sizeof_client_cert_der_2048")
        data.setSymbolValue("NET_PRES_BLOB_SERVER_CERT_FORMAT", 1)
        data.setSymbolValue("NET_PRES_BLOB_SERVER_CERT_FILENAME", "wolfssl/certs_test.h")
        data.setSymbolValue("NET_PRES_BLOB_SERVER_CERT_VARIABLE", "server_cert_der_2048")
        data.setSymbolValue("NET_PRES_BLOB_SERVER_CERT_LEN_VARIABLE", "sizeof_server_cert_der_2048")
        data.setSymbolValue("NET_PRES_BLOB_SERVER_KEY_FILENAME", "wolfssl/certs_test.h")
        data.setSymbolValue("NET_PRES_BLOB_SERVER_KEY_VARIABLE", "server_key_der_2048")
        data.setSymbolValue("NET_PRES_BLOB_SERVER_KEY_LEN_VARIABLE", "sizeof_server_key_der_2048")
        
        if(Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != "BareMetal") and (Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != None) :
            print("FreeRTOS selected")
            data.setSymbolValue("NET_PRES_RTOS_STACK_SIZE",32768)
            
        
    else:
        #Todo: change to Database.sendMessage(); but need handleMessage() in lib_wolfssl
        Database.setSymbolValue("lib_wolfssl","wolfssl", False)
        res = Database.deactivateComponents(["lib_wolfssl"])
        res = data.setSymbolValue("NET_PRES_USE_WOLF_SSL", False)
        data.setSymbolValue("NET_PRES_BLOB_CERT", 1)
        res = data.setSymbolValue("NET_PRES_GENERATE_ENC_STUBS", True)
        data.setDependencyEnabled("Net_Pres_SNTP_Dependency", False)
   
def netPresWolfSSLDebugEnable(symbol, event):
    data = symbol.getComponent()
    netPresWolfSSLEnabled = data.getSymbolValue("NET_PRES_USE_WOLF_SSL")
    wolfSSLDebugEnabled = Database.getSymbolValue("lib_wolfssl", "wolfssl_debug")
    if (netPresWolfSSLEnabled == True) and (wolfSSLDebugEnabled == True):
        symbol.setValue(True)
    else:
        if (symbol.getValue() == True):
            symbol.setValue(False)
        
def netPresStreamEnable(symbol, event):    
    netPresTCPEnabled = Database.getSymbolValue("tcpipTcp", "TCPIP_USE_TCP")
    data = symbol.getComponent()
    netPresTcpipTransportEnabled = data.getSymbolValue("NET_PRES_TRANSPORT_AS_TCPIP")

    if (netPresTCPEnabled == True) and (netPresTcpipTransportEnabled == True):
        symbol.setValue(True)        
    else:
        symbol.setValue(False)
   
def netPresDatagramEnable(symbol, event):
    netPresUDPEnabled = Database.getSymbolValue("tcpipUdp", "TCPIP_USE_UDP")        
    data = symbol.getComponent()  
    netPresTcpipTransportEnabled = data.getSymbolValue("NET_PRES_TRANSPORT_AS_TCPIP")

    if (netPresUDPEnabled == True) and (netPresTcpipTransportEnabled == True):
        symbol.setValue(True)
    else:
        symbol.setValue(False)
        
def netPresWolfsslTngtls(symbol, event):
    Database.setSymbolValue("lib_wolfssl","wolfsslLoadTNGTLSCert",event["value"])
        
def netPresWolfSSLSNI(symbol, event):
    Database.setSymbolValue("lib_wolfssl","wolfsslTlsSni",event["value"])

def netPresWolfSSLALPN(symbol, event):
    Database.setSymbolValue("lib_wolfssl","wolfsslTlsAlpn",event["value"])


def netPresMoreClientCertConfig(symbol, event):
    global netPresMoreClientCertConfigNumPrev
    print("Start netPresMoreClientCertConfig")
    if(event["id"] == "NET_PRES_BLOB_CLIENT_SUPPORT" ):
        netPresClientSupportEnable = Database.getSymbolValue("netPresBlobClientSupport","NET_PRES_BLOB_CLIENT_SUPPORT")
        netPresIndex = int(symbol.getID().strip("NET_PRES_BLOB_MORE_CLIENT_CERT_CONFIG_IDX"))
        print("NetPres Index: " + str(netPresIndex) )
        print(netPresMoreClientCertConfigNumPrev)
        if(netPresClientSupportEnable == True):
            if(netPresIndex < netPresMoreClientCertConfigNumPrev ):
                symbol.setVisible(True)
        else:
            symbol.setVisible(False)
        
    else:       
        print(symbol.getID())
        print(event["id"])
        netPresMoreClientCertConfigNumberValue = event["value"]
        print(netPresMoreClientCertConfigNumberValue)
        print(netPresMoreClientCertConfigNumPrev)
        if(netPresMoreClientCertConfigNumberValue > netPresMoreClientCertConfigNumPrev ):
            netPresMaxCaCertConfigIdx[netPresMoreClientCertConfigNumPrev].setVisible(True)
            netPresMaxCaCertConfigIdx[netPresMoreClientCertConfigNumPrev].setValue(True, 1)
            print("Set TRUE"+ str(netPresMoreClientCertConfigNumPrev))
            netPresMoreClientCertConfigNumPrev = netPresMoreClientCertConfigNumPrev + 1
            #Add more network configurations
        else:
            if(netPresMoreClientCertConfigNumberValue < netPresMoreClientCertConfigNumPrev ):
                #Reduce network configurations
                netPresMoreClientCertConfigNumPrev = netPresMoreClientCertConfigNumPrev - 1
                netPresMaxCaCertConfigIdx[netPresMoreClientCertConfigNumPrev].setVisible(False)
                netPresMaxCaCertConfigIdx[netPresMoreClientCertConfigNumPrev].setValue(False, 1)
                print("Set FALSE"+ str(netPresMoreClientCertConfigNumPrev))
                
            else:
                print("Do Nothing: "+ str(netPresMoreClientCertConfigNumPrev))
                #Do Nothing
            
    print("END netPresMoreClientCertConfig")
    
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
