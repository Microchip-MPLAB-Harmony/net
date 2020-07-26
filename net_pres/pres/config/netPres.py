"""*****************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************"""

def instantiateComponent(netPresComponent):

    print("Network Presentation Component")
    configName = Variables.get("__CONFIGURATION_NAME")
        
    # Network Presentation Layer
    netPresNeeded = netPresComponent.createBooleanSymbol("NET_PRES_NEEDED", None)
    netPresNeeded.setLabel("Use Network Presentation Layer")
    netPresNeeded.setVisible(False)
    netPresNeeded.setDefaultValue(True)

    # Use MPLAB Harmony Networking Presentation Layer
    netPresUse = netPresComponent.createBooleanSymbol("NET_PRES_USE", None)
    netPresUse.setLabel("Use MPLAB Harmony Networking Presentation Layer")
    netPresUse.setVisible(False)
    netPresUse.setDefaultValue(True)
    
    # RTOS Configuration
    netPresRtosMenu = netPresComponent.createMenuSymbol("NET_PRES_RTOS_MENU", None)
    netPresRtosMenu.setLabel("RTOS Configuration")
    netPresRtosMenu.setDescription("RTOS Configuration")
    netPresRtosMenu.setVisible(False)
    netPresRtosMenu.setVisible((Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != "BareMetal") and (Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != None))
    netPresRtosMenu.setDependencies(netPresshowRTOSMenu, ["HarmonyCore.SELECT_RTOS"])
    
    # Net Pres Execution mode
    netPresExecMode = netPresComponent.createComboSymbol("NET_PRES_RTOS", netPresRtosMenu, ["Standalone"]) 
    netPresExecMode.setLabel("Run this driver instance as")
    netPresExecMode.setVisible(False)
    netPresExecMode.setDescription("Net Pres Execution mode")
    netPresExecMode.setDefaultValue("Standalone")
    
    # Net Pres Task Stack Size
    netPresTaskSize = netPresComponent.createIntegerSymbol("NET_PRES_RTOS_STACK_SIZE", netPresRtosMenu)
    netPresTaskSize.setLabel("Stack Size (in bytes)")
    netPresTaskSize.setVisible(True)
    netPresTaskSize.setDescription("Net Pres Task Stack Size")
    netPresTaskSize.setDefaultValue(4096)
    netPresTaskSize.setDependencies(netPresRTOSStandaloneMenu, ["NET_PRES_RTOS"])
    
    # Net Pres Task Priority
    netPresTaskPriority = netPresComponent.createIntegerSymbol("NET_PRES_RTOS_TASK_PRIORITY", netPresRtosMenu)
    netPresTaskPriority.setLabel("Task Priority")
    netPresTaskPriority.setVisible(True)
    netPresTaskPriority.setDescription("Net Pres Task Priority")
    netPresTaskPriority.setDefaultValue(1)
    netPresTaskPriority.setDependencies(netPresRTOSStandaloneMenu, ["NET_PRES_RTOS"])
    
    # Net Pres Task Delay?
    netPresUseTaskDelay = netPresComponent.createBooleanSymbol("NET_PRES_RTOS_USE_DELAY", netPresRtosMenu)
    netPresUseTaskDelay.setLabel("Use Task Delay?")
    netPresUseTaskDelay.setVisible(True)
    netPresUseTaskDelay.setDescription("Net Pres Use Task Delay?")
    netPresUseTaskDelay.setDefaultValue(True)
    netPresUseTaskDelay.setDependencies(netPresRTOSStandaloneMenu, ["NET_PRES_RTOS"])
    
    # Net Pres Task Delay
    netPresTaskDelay = netPresComponent.createIntegerSymbol("NET_PRES_RTOS_DELAY", netPresRtosMenu)
    netPresTaskDelay.setLabel("Task Delay")
    netPresTaskDelay.setVisible(True)
    netPresTaskDelay.setDescription("Net Pres Task Delay")
    netPresTaskDelay.setDefaultValue(1000)
    netPresTaskDelay.setDependencies(netPresRTOSTaskDelayMenu, ["NET_PRES_RTOS","NET_PRES_RTOS_USE_DELAY"])
        
    # Number of Presentation Sockets
    netPresSocketCnt = netPresComponent.createIntegerSymbol("NET_PRES_SOCKETS", None)
    netPresSocketCnt.setLabel("Number of Presentation Sockets")
    netPresSocketCnt.setVisible(True)
    netPresSocketCnt.setDescription("Number of Presentation Sockets")
    netPresSocketCnt.setDefaultValue(10)    
 
    # Enable MPLAB Harmony TCP/IP as Transport Layer
    netPresTcpipTransLayer = netPresComponent.createBooleanSymbol("NET_PRES_TRANSPORT_AS_TCPIP", None)
    netPresTcpipTransLayer.setLabel("Use MPLAB Harmony TCP/IP as Transport Layer?")
    netPresTcpipTransLayer.setVisible(True)
    netPresTcpipTransLayer.setDefaultValue(True)   
    netPresTcpipTransLayer.setReadOnly(True)

    # Enable Stream Connections?
    netPresStreamConn = netPresComponent.createBooleanSymbol("NET_PRES_SUPPORT_STREAM", None)
    netPresStreamConn.setLabel("Support Stream Connections?")
    netPresStreamConn.setVisible(True)
    netPresStreamConn.setDefaultValue((Database.getSymbolValue("tcpipTcp", "TCPIP_USE_TCP") == True))
    netPresStreamConn.setDependencies(netPresStreamEnable, ["tcpipTcp.TCPIP_USE_TCP","NET_PRES_TRANSPORT_AS_TCPIP"])

    # Enable Data-gram Connections?
    netPresDatagramConn = netPresComponent.createBooleanSymbol("NET_PRES_SUPPORT_DATAGRAM",None)
    netPresDatagramConn.setLabel("Support Datagram Connections?")
    netPresDatagramConn.setVisible(True)
    netPresDatagramConn.setDefaultValue((Database.getSymbolValue("tcpipUdp", "TCPIP_USE_UDP") == True))
    netPresDatagramConn.setDependencies(netPresDatagramEnable, ["tcpipUdp.TCPIP_USE_UDP", "NET_PRES_TRANSPORT_AS_TCPIP"])

    # Enable Server Connections?
    netPresSrvrConn = netPresComponent.createBooleanSymbol("NET_PRES_SUPPORT_SERVER", None)
    netPresSrvrConn.setLabel("Support Server Connections?")
    netPresSrvrConn.setVisible(True)
    netPresSrvrConn.setDefaultValue(True)

    # Enable Client Connections?
    netPresClientConn = netPresComponent.createBooleanSymbol("NET_PRES_SUPPORT_CLIENT", None)
    netPresClientConn.setLabel("Support Client Connections?")
    netPresClientConn.setVisible(True)
    netPresClientConn.setDefaultValue(True)

    # Enable Encryption?
    netPresEncryptEnable = netPresComponent.createBooleanSymbol("NET_PRES_SUPPORT_ENCRYPTION", None)
    netPresEncryptEnable.setLabel("Support Encryption?")
    netPresEncryptEnable.setVisible(True)
    netPresEncryptEnable.setDefaultValue(False)

    netPresEncryptionProvider = netPresComponent.createKeyValueSetSymbol("NET_PRES_ENC_PROVIDE", netPresEncryptEnable)
    netPresEncryptionProvider.setVisible(False)
    netPresEncryptionProvider.setLabel("Encryption Provider")
    netPresEncryptionProvider.addKey("WolfSSL", "0", "Enable WolfSSL")
    netPresEncryptionProvider.addKey("Generate Stub", "1", "Generate Encryption Stub")
    netPresEncryptionProvider.setDisplayMode("Key")
    netPresEncryptionProvider.setOutputMode("Key")
    netPresEncryptionProvider.setDefaultValue(1)
    netPresEncryptionProvider.setDependencies(netPresEncryptMenuVisible, ["NET_PRES_SUPPORT_ENCRYPTION"])
    
    netPresUseWolfSSL = netPresComponent.createBooleanSymbol("NET_PRES_USE_WOLF_SSL", netPresEncryptEnable)
    netPresUseWolfSSL.setLabel("Use wolfSSL as Encryption Provider?")
    netPresUseWolfSSL.setVisible(False)
    netPresUseWolfSSL.setDefaultValue((netPresEncryptionProvider.getSelectedKey() == "WolfSSL"))
    netPresUseWolfSSL.setDependencies(netPresEncProviderMenu, ["NET_PRES_ENC_PROVIDE"])     
    
    # Generate Encryption Provider Stub Code?
    netPresGenEncryptStub = netPresComponent.createBooleanSymbol("NET_PRES_GENERATE_ENC_STUBS", netPresEncryptEnable)
    netPresGenEncryptStub.setLabel("Generate Encryption Provider Stub Code?")
    netPresGenEncryptStub.setVisible(False)
    netPresGenEncryptStub.setDefaultValue((netPresEncryptionProvider.getSelectedKey() == "Generate Stub"))

    netPresUseWolfSSLDebug = netPresComponent.createBooleanSymbol("NET_PRES_USE_WOLF_SSL_DEBUG_LOG", netPresEncryptEnable)
    netPresUseWolfSSLDebug.setLabel("Add system console logging for wolf ssl?")
    netPresUseWolfSSLDebug.setVisible(False)
    netPresUseWolfSSLDebug.setDefaultValue(False)
    netPresUseWolfSSLDebug.setDependencies(netPresWolfSSLDebugEnable, ["NET_PRES_USE_WOLF_SSL", "lib_wolfssl.wolfssl_debug"])  

    netPresWolfSSLLogBuffNum = netPresComponent.createIntegerSymbol("NET_PRES_USE_WOLF_SSL_DEBUG_LOG_BUFFERS", netPresUseWolfSSLDebug)
    netPresWolfSSLLogBuffNum.setVisible(False)
    netPresWolfSSLLogBuffNum.setLabel("Number of log message buffers")
    netPresWolfSSLLogBuffNum.setDefaultValue(80)
    netPresWolfSSLLogBuffNum.setDependencies(netPresEncryptMenuVisible, ["NET_PRES_USE_WOLF_SSL_DEBUG_LOG"])  
    
    # Support Stream Encryption?
    netPresStreamEncrypt = netPresComponent.createBooleanSymbol("NET_PRES_SUPPORT_STREAM_ENC", netPresEncryptEnable)
    netPresStreamEncrypt.setLabel("Support Stream Encryption?")
    netPresStreamEncrypt.setVisible(False)
    netPresStreamEncrypt.setDefaultValue(True)
    netPresStreamEncrypt.setDependencies(netPresEncryptMenuVisible, ["NET_PRES_SUPPORT_ENCRYPTION"])
    
    # Support Data-gram Encryption?
    netPresDataGramEncrypt = netPresComponent.createBooleanSymbol("NET_PRES_SUPPORT_DATAGRAM_ENC", netPresEncryptEnable)
    netPresDataGramEncrypt.setLabel("Support Data-gram Encryption?")
    netPresDataGramEncrypt.setVisible(False)
    netPresDataGramEncrypt.setDefaultValue(False)
    netPresDataGramEncrypt.setDependencies(netPresEncryptMenuVisible, ["NET_PRES_SUPPORT_ENCRYPTION"])
        
    # Support Server Encryption?
    netPresServerEncrypt = netPresComponent.createBooleanSymbol("NET_PRES_SUPPORT_SERVER_ENC", netPresEncryptEnable)
    netPresServerEncrypt.setLabel("Support Server Encryption?")
    netPresServerEncrypt.setVisible(False)
    netPresServerEncrypt.setDefaultValue(False)
    netPresServerEncrypt.setDependencies(netPresEncryptMenuVisible, ["NET_PRES_SUPPORT_ENCRYPTION"])
            
    # Support Client Encryption?
    netPresClientEncrypt = netPresComponent.createBooleanSymbol("NET_PRES_SUPPORT_CLIENT_ENC",netPresEncryptEnable)
    netPresClientEncrypt.setLabel("Support Client Encryption?")
    netPresClientEncrypt.setVisible(False)
    netPresClientEncrypt.setDefaultValue(True)
    netPresClientEncrypt.setDependencies(netPresEncryptMenuVisible, ["NET_PRES_SUPPORT_ENCRYPTION"])
    
    netPresBlobCert = netPresComponent.createKeyValueSetSymbol("NET_PRES_BLOB_CERT",netPresEncryptEnable)
    netPresBlobCert.setVisible(False)
    netPresBlobCert.setLabel("Encryption Certificate Store")
    netPresBlobCert.addKey("Fixed Flash Based Certificate Repo", "0", "Use Fixed Flash Based Certificate Repository for Encryption")
    netPresBlobCert.addKey("Generate Certificate Store Stubs", "1", "Generate Certificate Store Stubs")
    netPresBlobCert.setDisplayMode("Key")
    netPresBlobCert.setOutputMode("Key")
    netPresBlobCert.setDefaultValue(1)    
    netPresBlobCert.setDependencies(netPresEncryptMenuVisible, ["NET_PRES_SUPPORT_ENCRYPTION"])
    
    # Use Fixed Flash Based Certificate Repository for Encryption?
    netPresBlobCertRepo = netPresComponent.createBooleanSymbol("NET_PRES_BLOB_CERT_REPO", None)
    netPresBlobCertRepo.setLabel("Use Fixed Flash Based Certificate Repository for Encryption?")
    netPresBlobCertRepo.setVisible(False)
    netPresBlobCertRepo.setDescription("Use Fixed Flash Based Certificate Repository for Encryption?")
    netPresBlobCertRepo.setDefaultValue(False)
    netPresBlobCertRepo.setDependencies(netPresBlobCertMenu, ["NET_PRES_BLOB_CERT"])
    
    # Generate Certificate Store Stubs?
    netPresGenCertStub = netPresComponent.createBooleanSymbol("NET_PRES_CERT_STORE_STUBS", None)
    netPresGenCertStub.setLabel("Generate Certificate Store Stubs?")
    netPresGenCertStub.setVisible(False)
    netPresGenCertStub.setDescription("Generate Certificate Store Stubs?")
    netPresGenCertStub.setDefaultValue(False)
    netPresBlobCertRepo.setDependencies(netPresBlobCertMenu, ["NET_PRES_BLOB_CERT"])
    
    # Enable Peer Certificates Verification?
    netPresBlobClientSupport = netPresComponent.createBooleanSymbol("NET_PRES_BLOB_ENABLE_PEER_CERT_VERIFICATION", netPresBlobCert)
    netPresBlobClientSupport.setLabel("Enable Peer Certificates Verification")
    netPresBlobClientSupport.setVisible(False)
    netPresBlobClientSupport.setDescription("Enable Peer Certificates Verification")
    netPresBlobClientSupport.setDefaultValue(False)  
    netPresBlobClientSupport.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_CERT_REPO"])

    # Support Client Certificates?
    netPresBlobClientSupport = netPresComponent.createBooleanSymbol("NET_PRES_BLOB_CLIENT_SUPPORT", netPresBlobCert)
    netPresBlobClientSupport.setLabel("Support Client Certificates?")
    netPresBlobClientSupport.setVisible(False)
    netPresBlobClientSupport.setDescription("Support Client Certificates?")
    netPresBlobClientSupport.setDefaultValue(True)  
    netPresBlobClientSupport.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_CERT_REPO"])

    # File name containing definitions for Client Certificates?
    netPresBlobClientCertFileName = netPresComponent.createStringSymbol("NET_PRES_BLOB_CLIENT_CERT_FILENAME", netPresBlobClientSupport) 
    netPresBlobClientCertFileName.setLabel("Client Certificates Definitions File Name")
    netPresBlobClientCertFileName.setVisible(True)
    netPresBlobClientCertFileName.setDescription("File name containing definitions for Client Certificates?")
    netPresBlobClientCertFileName.setDefaultValue("ca-certs.h")
    netPresBlobClientCertFileName.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_CLIENT_SUPPORT"]) 

    # Variable Name Containing Data for Client Certificates?
    netPresBlobClientCertVar = netPresComponent.createStringSymbol("NET_PRES_BLOB_CLIENT_CERT_VARIABLE", netPresBlobClientSupport) 
    netPresBlobClientCertVar.setLabel("Client Certificates Data Variable Name")
    netPresBlobClientCertVar.setVisible(True)
    netPresBlobClientCertVar.setDescription("Variable Name Containing Data for Client Certificates?")
    netPresBlobClientCertVar.setDefaultValue("caCert")
    netPresBlobClientCertVar.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_CLIENT_SUPPORT"])  

    # Variable Name Containing Size of Client Certificates?
    netPresBlobClientCertLenVar = netPresComponent.createStringSymbol("NET_PRES_BLOB_CLIENT_CERT_LEN_VARIABLE", netPresBlobClientSupport) 
    netPresBlobClientCertLenVar.setLabel("Client Certificates Size Variable Name")
    netPresBlobClientCertLenVar.setVisible(True)
    netPresBlobClientCertLenVar.setDescription("Variable Name Containing Size of Client Certificates?")
    netPresBlobClientCertLenVar.setDefaultValue("caCert_len")
    netPresBlobClientCertLenVar.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_CLIENT_SUPPORT"])

    # Support Server Certificate?"
    netPresBlobServerSupport = netPresComponent.createBooleanSymbol("NET_PRES_BLOB_SERVER_SUPPORT", netPresBlobCert)
    netPresBlobServerSupport.setLabel("Support Server Certificate?")
    netPresBlobServerSupport.setVisible(False)
    netPresBlobServerSupport.setDescription("Support Server Certificate?")
    netPresBlobServerSupport.setDefaultValue(True)  
    netPresBlobServerSupport.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_CERT_REPO"])

    # File name containing definitions for Server Certificates?
    netPresBlobServerCertFileName = netPresComponent.createStringSymbol("NET_PRES_BLOB_SERVER_CERT_FILENAME", netPresBlobServerSupport) 
    netPresBlobServerCertFileName.setLabel("Server Certificates Definitions File Name")
    netPresBlobServerCertFileName.setVisible(True)
    netPresBlobServerCertFileName.setDescription("File name containing definitions for Server Certificates?")
    netPresBlobServerCertFileName.setDefaultValue("ca-certs.h")
    netPresBlobServerCertFileName.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_SERVER_SUPPORT"]) 

    # Variable Name Containing Data for Server Certificates?
    netPresBlobServerCertVar = netPresComponent.createStringSymbol("NET_PRES_BLOB_SERVER_CERT_VARIABLE", netPresBlobServerSupport) 
    netPresBlobServerCertVar.setLabel("Server Certificates Data Variable Name")
    netPresBlobServerCertVar.setVisible(True)
    netPresBlobServerCertVar.setDescription("Variable Name Containing Data for Server Certificates?")
    netPresBlobServerCertVar.setDefaultValue("serverCert")
    netPresBlobServerCertVar.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_SERVER_SUPPORT"])  

    # Variable Name Containing Size of Server Certificates?
    netPresBlobServerCertLenVar = netPresComponent.createStringSymbol("NET_PRES_BLOB_SERVER_CERT_LEN_VARIABLE", netPresBlobServerSupport) 
    netPresBlobServerCertLenVar.setLabel("Server Certificates Size Variable Name")
    netPresBlobServerCertLenVar.setVisible(True)
    netPresBlobServerCertLenVar.setDescription("Variable Name Containing Size of Server Certificates?")
    netPresBlobServerCertLenVar.setDefaultValue("serverCert_len")
    netPresBlobServerCertLenVar.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_SERVER_SUPPORT"])

    # File name containing definitions for Server Private Key?
    netPresBlobServerKeyFileName = netPresComponent.createStringSymbol("NET_PRES_BLOB_SERVER_KEY_FILENAME", netPresBlobServerSupport) 
    netPresBlobServerKeyFileName.setLabel("Server Private Key Definitions File Name")
    netPresBlobServerKeyFileName.setVisible(True)
    netPresBlobServerKeyFileName.setDescription("File name containing definitions for Server Private Key?")
    netPresBlobServerKeyFileName.setDefaultValue("ca-certs.h")
    netPresBlobServerKeyFileName.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_SERVER_SUPPORT"])
    
    # Variable Name Containing Data for Server Private Key?
    netPresBlobServerKeyVar = netPresComponent.createStringSymbol("NET_PRES_BLOB_SERVER_KEY_VARIABLE", netPresBlobServerSupport) 
    netPresBlobServerKeyVar.setLabel("Server Private Key Data Variable Name")
    netPresBlobServerKeyVar.setVisible(True)
    netPresBlobServerKeyVar.setDescription("Variable Name Containing Data for Server Private Key?")
    netPresBlobServerKeyVar.setDefaultValue("serverKey")
    netPresBlobServerKeyVar.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_SERVER_SUPPORT"])
    
    # Variable Name Containing Size of Server Private Key?
    netPresBlobServerKeyLen = netPresComponent.createStringSymbol("NET_PRES_BLOB_SERVER_KEY_LEN_VARIABLE", netPresBlobServerSupport) 
    netPresBlobServerKeyLen.setLabel("Server Private Key Size Variable Name")
    netPresBlobServerKeyLen.setVisible(True)
    netPresBlobServerKeyLen.setDescription("Variable Name Containing Size of Server Private Key?")
    netPresBlobServerKeyLen.setDefaultValue("serverKey_len")
    netPresBlobServerKeyLen.setDependencies(netPresMenuVisible, ["NET_PRES_BLOB_SERVER_SUPPORT"])
    
    # Generate Client Certificate Stubs?
    netPresCertStoreStubClient = netPresComponent.createBooleanSymbol("NET_PRES_CERT_STORE_STUBS_CLIENT", netPresBlobCert)
    netPresCertStoreStubClient.setLabel("Generate Client Certificate Stubs?")
    netPresCertStoreStubClient.setVisible(True)
    netPresCertStoreStubClient.setDescription("Generate Client Certificate Stubs?")
    netPresCertStoreStubClient.setDefaultValue(True)    
    netPresCertStoreStubClient.setDependencies(netPresMenuVisible, ["NET_PRES_CERT_STORE_STUBS"])   
    
    # Generate Server Certificate Stubs?
    netPresCertStoreStubServer = netPresComponent.createBooleanSymbol("NET_PRES_CERT_STORE_STUBS_SERVER", netPresBlobCert)
    netPresCertStoreStubServer.setLabel("Generate Server Certificate Stubs?")
    netPresCertStoreStubServer.setVisible(True)
    netPresCertStoreStubServer.setDescription("Generate Server Certificate Stubs?")
    netPresCertStoreStubServer.setDefaultValue(True)    
    netPresCertStoreStubServer.setDependencies(netPresMenuVisible, ["NET_PRES_CERT_STORE_STUBS"])           

    # file NET_PRES1_H "$HARMONY_VERSION_PATH/framework/net_pres/pres/net_pres.h"  to "$PROJECT_HEADER_FILES/framework/net_pres/pres/net_pres.h"
    netPresHeaderFile = netPresComponent.createFileSymbol(None, None)
    netPresHeaderFile.setSourcePath("net_pres/pres/net_pres.h")
    netPresHeaderFile.setOutputName("net_pres.h")
    netPresHeaderFile.setDestPath("net_pres/pres/")
    netPresHeaderFile.setProjectPath("config/" + configName + "/net_pres/pres/")
    netPresHeaderFile.setType("HEADER")
    netPresHeaderFile.setOverwrite(True)
    
    # file NET_PRES2_H "$HARMONY_VERSION_PATH/framework/net_pres/pres/net_pres_certstore.h"  to "$PROJECT_HEADER_FILES/framework/net_pres/pres/net_pres_certstore.h"
    netPresCertStoreHeaderFile = netPresComponent.createFileSymbol(None, None)
    netPresCertStoreHeaderFile.setSourcePath("net_pres/pres/net_pres_certstore.h")
    netPresCertStoreHeaderFile.setOutputName("net_pres_certstore.h")
    netPresCertStoreHeaderFile.setDestPath("net_pres/pres/")
    netPresCertStoreHeaderFile.setProjectPath("config/" + configName + "/net_pres/pres/")
    netPresCertStoreHeaderFile.setType("HEADER")
    netPresCertStoreHeaderFile.setOverwrite(True)   
    
    # file NET_PRES3_H "$HARMONY_VERSION_PATH/framework/net_pres/pres/net_pres_encryptionproviderapi.h"  to "$PROJECT_HEADER_FILES/framework/net_pres/pres/net_pres_encryptionproviderapi.h"
    netPresEncryptApiHeaderFile = netPresComponent.createFileSymbol(None, None)
    netPresEncryptApiHeaderFile.setSourcePath("net_pres/pres/net_pres_encryptionproviderapi.h")
    netPresEncryptApiHeaderFile.setOutputName("net_pres_encryptionproviderapi.h")
    netPresEncryptApiHeaderFile.setDestPath("net_pres/pres/")
    netPresEncryptApiHeaderFile.setProjectPath("config/" + configName + "/net_pres/pres/")
    netPresEncryptApiHeaderFile.setType("HEADER")
    netPresEncryptApiHeaderFile.setOverwrite(True)      
    
    # file NET_PRES4_H "$HARMONY_VERSION_PATH/framework/net_pres/pres/net_pres_socketapi.h"  to "$PROJECT_HEADER_FILES/framework/net_pres/pres/net_pres_socketapi.h"
    netPresSocketApiHeaderFile = netPresComponent.createFileSymbol(None, None)
    netPresSocketApiHeaderFile.setSourcePath("net_pres/pres/net_pres_socketapi.h")
    netPresSocketApiHeaderFile.setOutputName("net_pres_socketapi.h")
    netPresSocketApiHeaderFile.setDestPath("net_pres/pres/")
    netPresSocketApiHeaderFile.setProjectPath("config/" + configName + "/net_pres/pres/")
    netPresSocketApiHeaderFile.setType("HEADER")
    netPresSocketApiHeaderFile.setOverwrite(True)       
    
    # file NET_PRES5_H "$HARMONY_VERSION_PATH/framework/net_pres/pres/net_pres_transportapi.h"  to "$PROJECT_HEADER_FILES/framework/net_pres/pres/net_pres_transportapi.h"
    netPresTransApiHeaderFile = netPresComponent.createFileSymbol(None, None)
    netPresTransApiHeaderFile.setSourcePath("net_pres/pres/net_pres_transportapi.h")
    netPresTransApiHeaderFile.setOutputName("net_pres_transportapi.h")
    netPresTransApiHeaderFile.setDestPath("net_pres/pres/")
    netPresTransApiHeaderFile.setProjectPath("config/" + configName + "/net_pres/pres/")
    netPresTransApiHeaderFile.setType("HEADER")
    netPresTransApiHeaderFile.setOverwrite(True)        
    
    # file NET_PRES_LOCAL_H "$HARMONY_VERSION_PATH/framework/net_pres/pres/src/net_pres_local.h"  to "$PROJECT_HEADER_FILES/framework/net_pres/pres/src/net_pres_local.h"
    netPresLocalHeaderFile = netPresComponent.createFileSymbol(None, None)
    netPresLocalHeaderFile.setSourcePath("net_pres/pres/src/net_pres_local.h")
    netPresLocalHeaderFile.setOutputName("net_pres_local.h")
    netPresLocalHeaderFile.setDestPath("net_pres/pres/src/")
    netPresLocalHeaderFile.setProjectPath("config/" + configName + "/net_pres/pres/src/")
    netPresLocalHeaderFile.setType("HEADER")
    netPresLocalHeaderFile.setOverwrite(True)       
        
    # file NET_PRES_SOCKETAPICONVERSION_H "$HARMONY_VERSION_PATH/framework/net_pres/pres/net_pres_socketapiconversion.h"  to "$PROJECT_HEADER_FILES/framework/net_pres/pres/net_pres_socketapiconversion.h"
    netPresSktApiConvHeaderFile = netPresComponent.createFileSymbol(None, None)
    netPresSktApiConvHeaderFile.setSourcePath("net_pres/pres/net_pres_socketapiconversion.h")
    netPresSktApiConvHeaderFile.setOutputName("net_pres_socketapiconversion.h")
    netPresSktApiConvHeaderFile.setDestPath("net_pres/pres/")
    netPresSktApiConvHeaderFile.setProjectPath("config/" + configName + "/net_pres/pres/")
    netPresSktApiConvHeaderFile.setType("HEADER")
    netPresSktApiConvHeaderFile.setOverwrite(True)  

    # add "<#include \"/framework/net/templates/system_init.c.data.ftl\">"  to list SYSTEM_INIT_C_MODULE_INITIALIZATION_DATA
    netPresSysInitDataSourceFtl = netPresComponent.createFileSymbol(None, None)
    netPresSysInitDataSourceFtl.setType("STRING")
    netPresSysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    netPresSysInitDataSourceFtl.setSourcePath("net_pres/pres/templates/system/system_data_initialize.c.ftl")
    netPresSysInitDataSourceFtl.setMarkup(True)
    
    # add "<#include \"/framework/net/templates/system_init.c.call.ftl\">"  to list SYSTEM_INIT_C_INITIALIZE_MIDDLEWARE
    netPresSysInitCallSourceFtl = netPresComponent.createFileSymbol(None, None)
    netPresSysInitCallSourceFtl.setType("STRING")
    netPresSysInitCallSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_INITIALIZE_MIDDLEWARE")
    netPresSysInitCallSourceFtl.setSourcePath("net_pres/pres/templates/system/system_initialize.c.ftl")
    netPresSysInitCallSourceFtl.setMarkup(True)

    netPresSystemDefFile = netPresComponent.createFileSymbol(None, None)
    netPresSystemDefFile.setType("STRING")
    netPresSystemDefFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    netPresSystemDefFile.setSourcePath("net_pres/pres/templates/system/system_definitions.h.ftl")
    netPresSystemDefFile.setMarkup(True)
    
    netPresSystemDefObjFile = netPresComponent.createFileSymbol(None, None)
    netPresSystemDefObjFile.setType("STRING")
    netPresSystemDefObjFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_OBJECTS")
    netPresSystemDefObjFile.setSourcePath("net_pres/pres/templates/system/system_definitions_object.h.ftl")
    netPresSystemDefObjFile.setMarkup(True)
    
    netPresSystemConfigFtl = netPresComponent.createFileSymbol(None, None)
    netPresSystemConfigFtl.setSourcePath("net_pres/pres/templates/system/system_config.h.ftl")
    netPresSystemConfigFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    netPresSystemConfigFtl.setMarkup(True)
    netPresSystemConfigFtl.setType("STRING")
    
    # add "<#include \"/framework/net/templates/system_tasks.c.ftl\">"  to list SYSTEM_TASKS_C_CALL_LIB_TASKS
    netPresSysTaskSourceFtl = netPresComponent.createFileSymbol(None, None)
    netPresSysTaskSourceFtl.setType("STRING")
    netPresSysTaskSourceFtl.setOutputName("core.LIST_SYSTEM_TASKS_C_CALL_LIB_TASKS")
    netPresSysTaskSourceFtl.setSourcePath("net_pres/pres/templates/system/system_tasks.c.ftl")
    netPresSysTaskSourceFtl.setMarkup(True)

    netPresSystemRtosTasksFile = netPresComponent.createFileSymbol("NET_PRES_SYS_RTOS_TASK", None)
    netPresSystemRtosTasksFile.setType("STRING")
    netPresSystemRtosTasksFile.setOutputName("core.LIST_SYSTEM_RTOS_TASKS_C_DEFINITIONS")
    netPresSystemRtosTasksFile.setSourcePath("net_pres/pres/templates/system/system_rtos_tasks.c.ftl")
    netPresSystemRtosTasksFile.setMarkup(True)
    netPresSystemRtosTasksFile.setEnabled((Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != "BareMetal"))
    netPresSystemRtosTasksFile.setDependencies(genRtosTask, ["HarmonyCore.SELECT_RTOS"])
    
    # file NET_PRES_C "$HARMONY_VERSION_PATH/framework/net_pres/pres/src/net_pres.c" to "$PROJECT_SOURCE_FILES/framework/net_pres/pres/src/net_pres.c"
    netPresSourceFile = netPresComponent.createFileSymbol(None, None)
    netPresSourceFile.setSourcePath("net_pres/pres/src/net_pres.c")
    netPresSourceFile.setOutputName("net_pres.c")
    netPresSourceFile.setOverwrite(True)
    netPresSourceFile.setDestPath("net_pres/pres/src/")
    netPresSourceFile.setProjectPath("config/" + configName + "/net_pres/pres/src/")
    netPresSourceFile.setType("SOURCE")
    netPresSourceFile.setEnabled(True)
        
    # template NET_PRES_ENC_GLUE_H_TEMPLATE "$HARMONY_VERSION_PATH/framework/net_pres/pres/templates/net_pres_enc_glue.h.ftl" to "$PROJECT_HEADER_FILES/app/system_config/$CONFIGURATION/framework/net_pres/pres/net_pres_enc_glue.h"
    netPresEncGlueHeaderFile = netPresComponent.createFileSymbol(None, None)
    netPresEncGlueHeaderFile.setSourcePath("net_pres/pres/templates/net_pres_enc_glue.h.ftl")
    netPresEncGlueHeaderFile.setOutputName("net_pres_enc_glue.h")
    netPresEncGlueHeaderFile.setMarkup(True)
    netPresEncGlueHeaderFile.setDestPath("net_pres/pres/")
    netPresEncGlueHeaderFile.setProjectPath("config/" + configName + "/net_pres/pres/")
    netPresEncGlueHeaderFile.setType("HEADER")
    netPresEncGlueHeaderFile.setOverwrite(True)
    
    # template NET_PRES_ENC_GLUE_C_TEMPLATE "$HARMONY_VERSION_PATH/framework/net_pres/pres/templates/net_pres_enc_glue.c.ftl" to "$PROJECT_SOURCE_FILES/app/system_config/$CONFIGURATION/framework/net_pres/pres/net_pres_enc_glue.c"   
    netPresEncGlueSourceFile = netPresComponent.createFileSymbol(None, None)
    netPresEncGlueSourceFile.setSourcePath("net_pres/pres/templates/net_pres_enc_glue.c.ftl")
    netPresEncGlueSourceFile.setOutputName("net_pres_enc_glue.c")
    netPresEncGlueSourceFile.setDestPath("net_pres/pres/")
    netPresEncGlueSourceFile.setProjectPath("config/" + configName + "/net_pres/pres/")
    netPresEncGlueSourceFile.setType("SOURCE")
    netPresEncGlueSourceFile.setMarkup(True)    

    # ifblock NET_PRES_BLOB_CERT_REPO || NET_PRES_CERT_STORE_STUBS
    # template NET_PRES_ENC_CERT_STORE_C_TEMPLATE "$HARMONY_VERSION_PATH/framework/net_pres/pres/templates/net_pres_cert_store.c.ftl" to "$PROJECT_SOURCE_FILES/app/system_config/$CONFIGURATION/framework/net_pres/pres/net_pres_cert_store.c"
    # endif 
    netPresCertStoreSourceFile = netPresComponent.createFileSymbol(None, None)
    netPresCertStoreSourceFile.setSourcePath("net_pres/pres/templates/net_pres_cert_store.c.ftl")
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
        tcpipNetPresGroup.setAttachmentVisible("netPres", "NetPres_Crypto_Dependency")
        tcpipAutoConfigStackGroup.setAttachmentVisible("PRESENTATION LAYER", "netPres:NetPres_Crypto_Dependency")
        


        
def netPresRTOSTaskDelayMenu(symbol, event):
    netPresRtos = Database.getSymbolValue("netPres","NET_PRES_RTOS")
    netPresRtosUseDelay = Database.getSymbolValue("netPres","NET_PRES_RTOS_USE_DELAY")
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
    symbol.setVisible(event["value"])

def netPresEncProviderMenu(symbol, event):
    data = symbol.getComponent() 
    if (event["value"] == 0):        
        res = data.setSymbolValue("NET_PRES_GENERATE_ENC_STUBS", False)
        res = data.setSymbolValue("NET_PRES_USE_WOLF_SSL", True)
        res = Database.activateComponents(["lib_wolfssl"])
        #Todo: change to Database.sendMessage(); but need handleMessage() in lib_wolfssl
        Database.setSymbolValue("lib_wolfssl","wolfssl", True) 
        data.setSymbolValue("NET_PRES_BLOB_CERT", 0)
        data.setSymbolValue("NET_PRES_BLOB_CLIENT_CERT_FILENAME", "wolfssl/certs_test.h")
        data.setSymbolValue("NET_PRES_BLOB_CLIENT_CERT_VARIABLE", "client_cert_der_2048")
        data.setSymbolValue("NET_PRES_BLOB_CLIENT_CERT_LEN_VARIABLE", "sizeof_client_cert_der_2048")
        data.setSymbolValue("NET_PRES_BLOB_SERVER_CERT_FILENAME", "wolfssl/certs_test.h")
        data.setSymbolValue("NET_PRES_BLOB_SERVER_CERT_VARIABLE", "server_cert_der_2048")
        data.setSymbolValue("NET_PRES_BLOB_SERVER_CERT_LEN_VARIABLE", "sizeof_server_cert_der_2048")
        data.setSymbolValue("NET_PRES_BLOB_SERVER_KEY_FILENAME", "wolfssl/certs_test.h")
        data.setSymbolValue("NET_PRES_BLOB_SERVER_KEY_VARIABLE", "server_key_der_2048")
        data.setSymbolValue("NET_PRES_BLOB_SERVER_KEY_LEN_VARIABLE", "sizeof_server_key_der_2048")
        
    else:
        #Todo: change to Database.sendMessage(); but need handleMessage() in lib_wolfssl
        Database.setSymbolValue("lib_wolfssl","wolfssl", False)
        res = Database.deactivateComponents(["lib_wolfssl"])
        res = data.setSymbolValue("NET_PRES_USE_WOLF_SSL", False)
        data.setSymbolValue("NET_PRES_BLOB_CERT", 1)
        res = data.setSymbolValue("NET_PRES_GENERATE_ENC_STUBS", True)
   
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