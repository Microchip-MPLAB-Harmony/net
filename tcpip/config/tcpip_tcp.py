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
    
def instantiateComponent(tcpipTcpComponent):
    print("TCPIP TCP Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # Enable TCP
    tcpipTCP = tcpipTcpComponent.createBooleanSymbol("TCPIP_USE_TCP", None)
    tcpipTCP.setHelp("mcc_h3_tcp_configurations")
    tcpipTCP.setLabel("Use TCP")
    tcpipTCP.setVisible(False)
    tcpipTCP.setDescription("Enable TCP")
    tcpipTCP.setDefaultValue(True)

    # Maximum TX Segment Size
    tcpipTcpMaxTxSegSize = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_MAX_SEG_SIZE_TX", None)
    tcpipTcpMaxTxSegSize.setHelp("mcc_h3_tcp_configurations")
    tcpipTcpMaxTxSegSize.setLabel("Maximum TX Segment Size")
    tcpipTcpMaxTxSegSize.setVisible(True)
    tcpipTcpMaxTxSegSize.setDescription("Maximum TX Segment Size")
    tcpipTcpMaxTxSegSize.setDefaultValue(1460)

    # Maximum number of sockets
    tcpipTcpMaxSktNum = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_MAX_SOCKETS", None)
    tcpipTcpMaxSktNum.setHelp("mcc_h3_tcp_configurations")
    tcpipTcpMaxSktNum.setLabel("Maximum Number of Sockets")
    tcpipTcpMaxSktNum.setVisible(True)
    tcpipTcpMaxSktNum.setDescription("Maximum number of sockets")
    tcpipTcpMaxSktNum.setDefaultValue(10)
    
    # Default TX Socket Size
    tcpipTcpSktTxSize = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_SOCKET_DEFAULT_TX_SIZE", None)
    tcpipTcpSktTxSize.setHelp("mcc_h3_tcp_configurations")
    tcpipTcpSktTxSize.setLabel("Default TX Buffer Size")
    tcpipTcpSktTxSize.setVisible(True)
    tcpipTcpSktTxSize.setDescription("Default TX Buffer Size")
    tcpipTcpSktTxSize.setDefaultValue(512)

    # Default RX Socket Size
    tcpipTcpSktRxSize = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_SOCKET_DEFAULT_RX_SIZE", None)
    tcpipTcpSktRxSize.setHelp("mcc_h3_tcp_configurations")
    tcpipTcpSktRxSize.setLabel("Default RX Buffer Size")
    tcpipTcpSktRxSize.setVisible(True)
    tcpipTcpSktRxSize.setDescription("Default RX Buffer Size")
    tcpipTcpSktRxSize.setDefaultValue(512)

    # Keep Alive Timeout in ms
    tcpipTcpKeepAliveTimeout = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_KEEP_ALIVE_TIMEOUT", None)
    tcpipTcpKeepAliveTimeout.setHelp("mcc_h3_tcp_configurations")
    tcpipTcpKeepAliveTimeout.setLabel("Default Keep Alive Time-out (in msec)")
    tcpipTcpKeepAliveTimeout.setVisible(True)
    tcpipTcpKeepAliveTimeout.setDescription("Keep Alive Timeout in ms")
    tcpipTcpKeepAliveTimeout.setDefaultValue(10000)
    
    # Enable TCP Commands
    tcpipTcpCommands = tcpipTcpComponent.createBooleanSymbol("TCPIP_TCP_COMMANDS", None)
    tcpipTcpCommands.setHelp("mcc_h3_tcp_configurations")
    tcpipTcpCommands.setLabel("Enable TCP Console Commands")
    tcpipTcpCommands.setVisible(True)
    tcpipTcpCommands.setDescription("Enable TCP Commands")
    tcpipTcpCommands.setDefaultValue(False)

    # Advanced Settings
    tcpipTcpAdvSettings = tcpipTcpComponent.createMenuSymbol("TCPIP_TCP_ADV_SETTING", None)
    tcpipTcpAdvSettings.setLabel("Advanced Settings")
    tcpipTcpAdvSettings.setDescription("Advanced Settings")
    tcpipTcpAdvSettings.setVisible(True)

    # TCP Task Tick Rate
    tcpipTcpTaskTickRate = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_TASK_TICK_RATE", tcpipTcpAdvSettings)
    tcpipTcpTaskTickRate.setHelp("mcc_h3_tcp_configurations")
    tcpipTcpTaskTickRate.setLabel("TCP Task Rate")
    tcpipTcpTaskTickRate.setVisible(True)
    tcpipTcpTaskTickRate.setDescription("TCP Task Tick Rate")
    tcpipTcpTaskTickRate.setDefaultValue(5)
    
    # Enable TCP Sockets Dynamic Options
    tcpipTcpDynOptions = tcpipTcpComponent.createBooleanSymbol("TCPIP_TCP_DYNAMIC_OPTIONS", tcpipTcpAdvSettings)
    tcpipTcpDynOptions.setHelp("mcc_h3_tcp_configurations")
    tcpipTcpDynOptions.setLabel("Enable TCP Sockets Dynamic Options")
    tcpipTcpDynOptions.setVisible(True)
    tcpipTcpDynOptions.setDescription("Enable TCP Sockets Dynamic Options")
    tcpipTcpDynOptions.setDefaultValue(True)

    # Start Timeout in ms
    tcpipTcpStartTimeout = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_START_TIMEOUT_VAL", tcpipTcpAdvSettings)
    tcpipTcpStartTimeout.setHelp("mcc_h3_tcp_configurations")
    tcpipTcpStartTimeout.setLabel("Start Time-out (in msec)")
    tcpipTcpStartTimeout.setVisible(True)
    tcpipTcpStartTimeout.setDescription("Start Timeout in ms")
    tcpipTcpStartTimeout.setDefaultValue(1000)

    # Delayed Ack Timeout in ms
    tcpipTcpDelayAckTimeout = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_DELAYED_ACK_TIMEOUT", tcpipTcpAdvSettings)
    tcpipTcpDelayAckTimeout.setHelp("mcc_h3_tcp_configurations")
    tcpipTcpDelayAckTimeout.setLabel("Delayed Ack Time-out (in msec)")
    tcpipTcpDelayAckTimeout.setVisible(True)
    tcpipTcpDelayAckTimeout.setDescription("Delayed Ack Timeout in ms")
    tcpipTcpDelayAckTimeout.setDefaultValue(100)

    # Fin Wait 2 Timeout in ms
    tcpipTcpFinWait2Timeout = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_FIN_WAIT_2_TIMEOUT", tcpipTcpAdvSettings)
    tcpipTcpFinWait2Timeout.setHelp("mcc_h3_tcp_configurations")
    tcpipTcpFinWait2Timeout.setLabel("Fin Wait 2 Time-out (in msec)")
    tcpipTcpFinWait2Timeout.setVisible(True)
    tcpipTcpFinWait2Timeout.setDescription("Fin Wait 2 Timeout in ms")
    tcpipTcpFinWait2Timeout.setDefaultValue(5000)


    # Maximum number of retries
    tcpipTcpMaxRetryNum = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_MAX_RETRIES", tcpipTcpAdvSettings)
    tcpipTcpMaxRetryNum.setHelp("mcc_h3_tcp_configurations")
    tcpipTcpMaxRetryNum.setLabel("Maximum Number of Transmission Retries")
    tcpipTcpMaxRetryNum.setVisible(True)
    tcpipTcpMaxRetryNum.setDescription("Maximum number of retries")
    tcpipTcpMaxRetryNum.setDefaultValue(5)
    
    # Maximum number of Unacked Keep Alives
    tcpipTcpMaxUnackKeepAlivesNum = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_MAX_UNACKED_KEEP_ALIVES", tcpipTcpAdvSettings)
    tcpipTcpMaxUnackKeepAlivesNum.setHelp("mcc_h3_tcp_configurations")
    tcpipTcpMaxUnackKeepAlivesNum.setLabel("Maximum Number of Keep Alive Retries")
    tcpipTcpMaxUnackKeepAlivesNum.setVisible(True)
    tcpipTcpMaxUnackKeepAlivesNum.setDescription("Maximum number of Unacked Keep Alives Retries")
    tcpipTcpMaxUnackKeepAlivesNum.setDefaultValue(6)

    # Maximum number of Syn Retries
    tcpipTcpMaxSynRetryNum = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_MAX_SYN_RETRIES", tcpipTcpAdvSettings)
    tcpipTcpMaxSynRetryNum.setHelp("mcc_h3_tcp_configurations")
    tcpipTcpMaxSynRetryNum.setLabel("Maximum Number of SYN Retries")
    tcpipTcpMaxSynRetryNum.setVisible(True)
    tcpipTcpMaxSynRetryNum.setDescription("Maximum number of Syn Retries")
    tcpipTcpMaxSynRetryNum.setDefaultValue(3)
        
    # Auto Transmit Timeout in ms
    tcpipTcpAutoTxTimeout = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_AUTO_TRANSMIT_TIMEOUT_VAL", tcpipTcpAdvSettings)
    tcpipTcpAutoTxTimeout.setHelp("mcc_h3_tcp_configurations")
    tcpipTcpAutoTxTimeout.setLabel("Auto Transmit Time-out (in msec)")
    tcpipTcpAutoTxTimeout.setVisible(True)
    tcpipTcpAutoTxTimeout.setDescription("Auto Transmit Timeout in ms")
    tcpipTcpAutoTxTimeout.setDefaultValue(40)

    # Window Update Time-out
    tcpipTcpWindowUpdateTimeout = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_WINDOW_UPDATE_TIMEOUT_VAL", tcpipTcpAdvSettings)
    tcpipTcpWindowUpdateTimeout.setHelp("mcc_h3_tcp_configurations")
    tcpipTcpWindowUpdateTimeout.setLabel("Window Update Time-out (in msec)")
    tcpipTcpWindowUpdateTimeout.setVisible(True)
    tcpipTcpWindowUpdateTimeout.setDescription("Window Update Time-out")
    tcpipTcpWindowUpdateTimeout.setDefaultValue(200)

    # Close Wait Timeout in ms
    tcpipTcpCloseWaitTimeout = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_CLOSE_WAIT_TIMEOUT", tcpipTcpAdvSettings)
    tcpipTcpCloseWaitTimeout.setHelp("mcc_h3_tcp_configurations")
    tcpipTcpCloseWaitTimeout.setLabel("OBSOLETE! Close Wait Time-out (in msec). Should be 0!")
    tcpipTcpCloseWaitTimeout.setVisible(True)
    tcpipTcpCloseWaitTimeout.setDescription("OBSOLETE! Close Wait Timeout in ms. Should be 0!")
    tcpipTcpCloseWaitTimeout.setDefaultValue(0)

    # Maximum Segment Length (MSL) Timeout in seconds
    tcpipTcpMaxSegLenTimeout = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_MSL_TIMEOUT", tcpipTcpAdvSettings)
    tcpipTcpMaxSegLenTimeout.setHelp("mcc_h3_tcp_configurations")
    tcpipTcpMaxSegLenTimeout.setLabel("Maximum Segment Length (MSL) Timeout (in sec)")
    tcpipTcpMaxSegLenTimeout.setVisible(True)
    tcpipTcpMaxSegLenTimeout.setDescription("Maximum Segment Length (MSL) Timeout in seconds")
    tcpipTcpMaxSegLenTimeout.setDefaultValue(0)

    # TCP quiet time in seconds
    tcpipTcpQuietTime = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_QUIET_TIME", tcpipTcpAdvSettings)
    tcpipTcpQuietTime.setHelp("mcc_h3_tcp_configurations")
    tcpipTcpQuietTime.setLabel("TCP quiet time (in sec)")
    tcpipTcpQuietTime.setVisible(True)
    tcpipTcpQuietTime.setDescription("TCP quiet time in seconds")
    tcpipTcpQuietTime.setDefaultValue(0)

    # Enable External Packet Processing
    tcpipTcpExtPktProcess = tcpipTcpComponent.createBooleanSymbol("TCPIP_TCP_EXTERN_PACKET_PROCESS", tcpipTcpAdvSettings)
    tcpipTcpExtPktProcess.setHelp("mcc_h3_tcp_configurations")
    tcpipTcpExtPktProcess.setLabel("Enable External Packet Processing")
    tcpipTcpExtPktProcess.setVisible(True)
    tcpipTcpExtPktProcess.setDescription("Allows External Processing of RX Packets")
    tcpipTcpExtPktProcess.setDefaultValue(False)
    
    # Disable Crypto Dependency
    tcpipTcpCryptoSupport = tcpipTcpComponent.createBooleanSymbol("TCPIP_TCP_DISABLE_CRYPTO_USAGE", tcpipTcpAdvSettings)
    tcpipTcpCryptoSupport.setHelp("mcc_h3_tcp_configurations")
    tcpipTcpCryptoSupport.setLabel("Disable Crypto Dependency")
    tcpipTcpCryptoSupport.setVisible(True)
    tcpipTcpCryptoSupport.setDescription("Disable Crypto Dependency")
    tcpipTcpCryptoSupport.setDefaultValue(False)
    tcpipCryptoEnable()
    
    # Disable Crypto Dependency
    tcpipTcpCryptoSupportWarning = tcpipTcpComponent.createCommentSymbol("TCPIP_TCP_DISABLE_CRYPTO_USAGE_COMMENT ", tcpipTcpCryptoSupport)
    tcpipTcpCryptoSupportWarning.setLabel("!!Potential security vulnerability. See RFC 6528 and 6056")
    tcpipTcpCryptoSupportWarning.setVisible(False)
    tcpipTcpCryptoSupportWarning.setDependencies(tcpipTcpCryptoDisable, ["TCPIP_TCP_DISABLE_CRYPTO_USAGE"])  
   
    tcpipTcpheapdependency = [  "TCPIP_TCP_MAX_SOCKETS", "TCPIP_TCP_SOCKET_DEFAULT_TX_SIZE", "TCPIP_TCP_SOCKET_DEFAULT_RX_SIZE", 
                                "tcpipHttpNet.TCPIP_STACK_USE_HTTP_NET_SERVER", "tcpipHttpNet.TCPIP_HTTP_NET_MAX_CONNECTIONS", 
                                "tcpipHttpNet.TCPIP_HTTP_NET_SKT_TX_BUFF_SIZE", "tcpipHttpNet.TCPIP_HTTP_NET_SKT_RX_BUFF_SIZE",
                                "tcpipHttp.TCPIP_STACK_USE_HTTP_SERVER", "tcpipHttp.TCPIP_HTTP_MAX_CONNECTIONS", 
                                "tcpipHttp.TCPIP_HTTP_SKT_TX_BUFF_SIZE", "tcpipHttp.TCPIP_HTTP_SKT_RX_BUFF_SIZE",
                                "tcpipTelnet.TCPIP_USE_TELNET", "tcpipTelnet.TCPIP_TELNET_MAX_CONNECTIONS", 
                                "tcpipTelnet.TCPIP_TELNET_SKT_TX_BUFF_SIZE", "tcpipTelnet.TCPIP_TELNET_SKT_RX_BUFF_SIZE",
                                "tcpipSmtpc.TCPIP_USE_SMTPC_CLIENT", "tcpipSmtpc.TCPIP_SMTPC_MAIL_CONNECTIONS", 
                                "tcpipSmtpc.TCPIP_SMTPC_SKT_TX_BUFF_SIZE", "tcpipSmtpc.TCPIP_SMTPC_SKT_RX_BUFF_SIZE",
                                "tcpipFtps.TCPIP_USE_FTP_MODULE", "tcpipFtps.TCPIP_FTP_MAX_CONNECTIONS", 
                                "tcpipFtps.TCPIP_FTP_DATA_SKT_TX_BUFF_SIZE", "tcpipFtps.TCPIP_FTP_DATA_SKT_RX_BUFF_SIZE",
                                "tcpipIperf.TCPIP_USE_IPERF", "tcpipIperf.TCPIP_IPERF_MAX_INSTANCES", 
                                "tcpipIperf.TCPIP_IPERF_TX_BUFFER_SIZE", "tcpipIperf.TCPIP_IPERF_RX_BUFFER_SIZE", 
                                "tcpipWSC.TCPIP_STACK_USE_WS_CLIENT", "tcpipWSC.TCPIP_WSC_CONNECTIONS", 
                                "tcpipWSC.TCPIP_WSC_SKT_TX_BUFF_SIZE", "tcpipWSC.TCPIP_WSC_SKT_RX_BUFF_SIZE",
                                "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]   
        
    # TCP Heap Size
    tcpipTcpHeapSize = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_HEAP_SIZE", None)
    tcpipTcpHeapSize.setLabel("TCP Heap Size (bytes)")  
    tcpipTcpHeapSize.setVisible(False)
    tcpipTcpHeapSize.setDefaultValue(tcpipTcpHeapCalc())
    tcpipTcpHeapSize.setReadOnly(True)
    tcpipTcpHeapSize.setDependencies(tcpipTcpHeapUpdate, tcpipTcpheapdependency)  

    #Add to system_config.h
    tcpipTcpHeaderFtl = tcpipTcpComponent.createFileSymbol(None, None)
    tcpipTcpHeaderFtl.setSourcePath("tcpip/config/tcp.h.ftl")
    tcpipTcpHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipTcpHeaderFtl.setMarkup(True)
    tcpipTcpHeaderFtl.setType("STRING")

    # Add tcp.c file
    tcpipTcpSourceFile = tcpipTcpComponent.createFileSymbol(None, None)
    tcpipTcpSourceFile.setSourcePath("tcpip/src/tcp.c")
    tcpipTcpSourceFile.setOutputName("tcp.c")
    tcpipTcpSourceFile.setOverwrite(True)
    tcpipTcpSourceFile.setDestPath("library/tcpip/src/")
    tcpipTcpSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipTcpSourceFile.setType("SOURCE")
    tcpipTcpSourceFile.setEnabled(True)

def tcpipTcpHeapCalc(): 
    nSockets = Database.getSymbolValue("tcpipTcp","TCPIP_TCP_MAX_SOCKETS")
    sktTxBuffSize = Database.getSymbolValue("tcpipTcp","TCPIP_TCP_SOCKET_DEFAULT_TX_SIZE")
    sktRxBuffSize = Database.getSymbolValue("tcpipTcp","TCPIP_TCP_SOCKET_DEFAULT_RX_SIZE")
    nHTTP_NET_Connections = 0 
    nHTTP_Connections = 0
    nTelnetConnections = 0
    nMailConnections = 0
    nFTPSConnections = 0
    nIperfInstances = 0
    nWscConnections = 0
    sktTxBuffSize_HTTPNET = 0
    sktRxBuffSize_HTTPNET = 0    
    sktTxBuffSize_HTTP = 0
    sktRxBuffSize_HTTP = 0    
    sktTxBuffSize_Telnet = 0
    sktRxBuffSize_Telnet = 0    
    sktTxBuffSize_Smtpc = 0
    sktRxBuffSize_Smtpc = 0    
    sktTxBuffSize_Ftps = 0
    sktRxBuffSize_Ftps = 0    
    sktTxBuffSize_iperf = 0
    sktRxBuffSize_iperf = 0
    sktTxBuffSize_wsc = 0
    sktRxBuffSize_wsc = 0    
    
    if(Database.getSymbolValue("tcpipHttpNet","TCPIP_STACK_USE_HTTP_NET_SERVER") == True):
        if(Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_MAX_CONNECTIONS") != None):
            nHTTP_NET_Connections = Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_MAX_CONNECTIONS")
        if(Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_SKT_TX_BUFF_SIZE") != None):
            sktTxBuffSize_HTTPNET = Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_SKT_TX_BUFF_SIZE")
            if(sktTxBuffSize_HTTPNET == 0):
                sktTxBuffSize_HTTPNET = sktTxBuffSize
        if(Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_SKT_RX_BUFF_SIZE") != None):
            sktRxBuffSize_HTTPNET = Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_SKT_RX_BUFF_SIZE")
            if(sktRxBuffSize_HTTPNET == 0):
                sktRxBuffSize_HTTPNET = sktRxBuffSize
    
    if(Database.getSymbolValue("tcpipHttp","TCPIP_STACK_USE_HTTP_SERVER") == True):    
        if(Database.getSymbolValue("tcpipHttp","TCPIP_HTTP_MAX_CONNECTIONS") != None):
            nHTTP_Connections = Database.getSymbolValue("tcpipHttp","TCPIP_HTTP_MAX_CONNECTIONS")
        if(Database.getSymbolValue("tcpipHttp","TCPIP_HTTP_SKT_TX_BUFF_SIZE")!= None):    
            sktTxBuffSize_HTTP = Database.getSymbolValue("tcpipHttp","TCPIP_HTTP_SKT_TX_BUFF_SIZE")
            if(sktTxBuffSize_HTTP == 0):
                sktTxBuffSize_HTTP = sktTxBuffSize
        if(Database.getSymbolValue("tcpipHttp","TCPIP_HTTP_SKT_RX_BUFF_SIZE")!= None):
            sktRxBuffSize_HTTP = Database.getSymbolValue("tcpipHttp","TCPIP_HTTP_SKT_RX_BUFF_SIZE")
            if(sktRxBuffSize_HTTP == 0):
                sktRxBuffSize_HTTP = sktRxBuffSize
    
    if(Database.getSymbolValue("tcpipTelnet","TCPIP_USE_TELNET") == True):  
        if(Database.getSymbolValue("tcpipTelnet","TCPIP_TELNET_MAX_CONNECTIONS") != None):
            nTelnetConnections = Database.getSymbolValue("tcpipTelnet","TCPIP_TELNET_MAX_CONNECTIONS")
        if(Database.getSymbolValue("tcpipTelnet","TCPIP_TELNET_SKT_TX_BUFF_SIZE") != None):    
            sktTxBuffSize_Telnet = Database.getSymbolValue("tcpipTelnet","TCPIP_TELNET_SKT_TX_BUFF_SIZE")
            if(sktTxBuffSize_Telnet == 0):
                sktTxBuffSize_Telnet = sktTxBuffSize
        if(Database.getSymbolValue("tcpipTelnet","TCPIP_TELNET_SKT_RX_BUFF_SIZE") != None):
            sktRxBuffSize_Telnet = Database.getSymbolValue("tcpipTelnet","TCPIP_TELNET_SKT_RX_BUFF_SIZE")
            if(sktRxBuffSize_Telnet == 0):
                sktRxBuffSize_Telnet = sktRxBuffSize
    
    if(Database.getSymbolValue("tcpipSmtpc","TCPIP_USE_SMTPC_CLIENT") == True):
        if(Database.getSymbolValue("tcpipSmtpc","TCPIP_SMTPC_MAIL_CONNECTIONS") != None):
            nMailConnections = Database.getSymbolValue("tcpipSmtpc","TCPIP_SMTPC_MAIL_CONNECTIONS")
        if(Database.getSymbolValue("tcpipSmtpc","TCPIP_SMTPC_SKT_TX_BUFF_SIZE") != None):
            sktTxBuffSize_Smtpc = Database.getSymbolValue("tcpipSmtpc","TCPIP_SMTPC_SKT_TX_BUFF_SIZE")
            if(sktTxBuffSize_Smtpc == 0):
                sktTxBuffSize_Smtpc = sktTxBuffSize
        if(Database.getSymbolValue("tcpipSmtpc","TCPIP_SMTPC_SKT_RX_BUFF_SIZE") != None):
            sktRxBuffSize_Smtpc = Database.getSymbolValue("tcpipSmtpc","TCPIP_SMTPC_SKT_RX_BUFF_SIZE")
            if(sktRxBuffSize_Smtpc == 0):
                sktRxBuffSize_Smtpc = sktRxBuffSize
    
    if(Database.getSymbolValue("tcpipFtps","TCPIP_USE_FTP_MODULE") == True): 
        if(Database.getSymbolValue("tcpipFtps","TCPIP_FTP_MAX_CONNECTIONS") != None):
            nFTPSConnections = Database.getSymbolValue("tcpipFtps","TCPIP_FTP_MAX_CONNECTIONS")
        if(Database.getSymbolValue("tcpipFtps","TCPIP_FTP_DATA_SKT_TX_BUFF_SIZE") != None):
            sktTxBuffSize_Ftps = Database.getSymbolValue("tcpipFtps","TCPIP_FTP_DATA_SKT_TX_BUFF_SIZE")
            if(sktTxBuffSize_Ftps == 0):
                sktTxBuffSize_Ftps = sktTxBuffSize
        if(Database.getSymbolValue("tcpipFtps","TCPIP_FTP_DATA_SKT_RX_BUFF_SIZE") != None):
            sktRxBuffSize_Ftps = Database.getSymbolValue("tcpipFtps","TCPIP_FTP_DATA_SKT_RX_BUFF_SIZE")
            if(sktRxBuffSize_Ftps == 0):
                sktRxBuffSize_Ftps = sktRxBuffSize
    
    if(Database.getSymbolValue("tcpipIperf","TCPIP_USE_IPERF") == True): 
        if(Database.getSymbolValue("tcpipIperf","TCPIP_IPERF_MAX_INSTANCES") != None):
            nIperfInstances = Database.getSymbolValue("tcpipIperf","TCPIP_IPERF_MAX_INSTANCES")
        if(Database.getSymbolValue("tcpipIperf","TCPIP_IPERF_TX_BUFFER_SIZE") != None):
            sktTxBuffSize_iperf = Database.getSymbolValue("tcpipIperf","TCPIP_IPERF_TX_BUFFER_SIZE")
            if(sktTxBuffSize_iperf == 0):
                sktTxBuffSize_iperf = sktTxBuffSize
        if(Database.getSymbolValue("tcpipIperf","TCPIP_IPERF_RX_BUFFER_SIZE") != None):
            sktRxBuffSize_iperf = Database.getSymbolValue("tcpipIperf","TCPIP_IPERF_RX_BUFFER_SIZE")
            if(sktRxBuffSize_iperf == 0):
                sktRxBuffSize_iperf = sktRxBuffSize
    
    if(Database.getSymbolValue("tcpipWSC","TCPIP_STACK_USE_WS_CLIENT") == True):
        if(Database.getSymbolValue("tcpipWSC","TCPIP_WSC_CONNECTIONS") != None):
            nWscConnections = Database.getSymbolValue("tcpipWSC","TCPIP_WSC_CONNECTIONS")
        if(Database.getSymbolValue("tcpipWSC","TCPIP_WSC_SKT_TX_BUFF_SIZECPIP_SMTPC_SKT_TX_BUFF_SIZE") != None):
            sktTxBuffSize_wsc = Database.getSymbolValue("tcpipWSC","TCPIP_WSC_SKT_TX_BUFF_SIZE")
            if(sktTxBuffSize_wsc == 0):
                sktTxBuffSize_wsc = sktTxBuffSize
        if(Database.getSymbolValue("tcpipWSC","TCPIP_WSC_SKT_TX_BUFF_SIZE") != None):
            sktRxBuffSize_wsc = Database.getSymbolValue("tcpipWSC","TCPIP_WSC_SKT_TX_BUFF_SIZE")
            if(sktRxBuffSize_wsc == 0):
                sktRxBuffSize_wsc = sktRxBuffSize
    
    ownSockets = (nSockets - nHTTP_NET_Connections - nHTTP_Connections - nTelnetConnections - nMailConnections - nFTPSConnections  - nIperfInstances - nWscConnections)
    
    heap_size = ownSockets * (sktTxBuffSize + sktRxBuffSize) + (nSockets * 132) \
                + nHTTP_NET_Connections * (176 + sktTxBuffSize_HTTPNET + sktRxBuffSize_HTTPNET) \
                + nHTTP_Connections * (176 + sktTxBuffSize_HTTP + sktRxBuffSize_HTTP) \
                + nTelnetConnections * (sktTxBuffSize_Telnet + sktRxBuffSize_Telnet) \
                + nMailConnections * (176 + sktTxBuffSize_Smtpc + sktRxBuffSize_Smtpc) \
                + nFTPSConnections * (sktTxBuffSize_Ftps + sktRxBuffSize_Ftps) \
                + nIperfInstances * (sktTxBuffSize_iperf + sktRxBuffSize_iperf) \
                + nWscConnections * (176 + sktTxBuffSize_wsc + sktRxBuffSize_wsc)
                    
    return heap_size    
    
def tcpipTcpHeapUpdate(symbol, event): 
    heap_size = tcpipTcpHeapCalc()
    symbol.setValue(heap_size)
    if(event["id"] == "TCPIP_STACK_HEAP_CALC_MASK"):
        symbol.setVisible(event["value"])


def tcpipTcpMenuVisibleSingle(symbol, event):
    if (event["value"] == True):     
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)  
        
def tcpipTcpCryptoDisable(symbol, event):
    tcpComponent = Database.getComponentByID("tcpipTcp")
    if (event["value"] == True):    
        symbol.setVisible(True)
        symbol.getComponent().setDependencyEnabled("Tcp_Crypto_Dependency", False);
        setVal("tcpipStack", "TCPIP_STACK_SYS_RANDOM_H2_ADAPTER", False)
        # tcpipCryptoDisable()
    else:
        symbol.setVisible(False)  
        symbol.getComponent().setDependencyEnabled("Tcp_Crypto_Dependency", True);
        setVal("tcpipStack", "TCPIP_STACK_SYS_RANDOM_H2_ADAPTER", True)
        tcpipCryptoEnable()
        
# make TCP options visible
def tcpipTcpMenuVisible(symbol, tcpipIPSymbol):
    tcpipIPv4 = Database.getSymbolValue("tcpipIPv4","TCPIP_STACK_USE_IPV4")
    tcpipIPv6 = Database.getSymbolValue("tcpipIPv6","TCPIP_STACK_USE_IPV6")
    if(tcpipIPv4 or tcpipIPv6):
        symbol.setVisible(True)
        symbol.setValue(True,1)
    else:
        symbol.setVisible(False)
        symbol.setValue(False,1)


def tcpipCryptoEnable(): 
    autoConnect = False
    if(Database.getComponentByID("lib_crypto") == None):
        res = Database.activateComponents(["lib_crypto"]) 
        autoConnect = True
    if(Database.getComponentByID("lib_wolfcrypt") == None):
        res = Database.activateComponents(["lib_wolfcrypt"])
        autoConnect = True   
        if(Database.getSymbolValue("lib_wolfcrypt", "wolfcrypt_md5") != True):
            Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_md5", True)
        if(Database.getSymbolValue("lib_wolfcrypt", "wolfcrypt_random") != True):
            Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_random", True)   
        if(Database.getSymbolValue("lib_wolfcrypt", "wolfcrypt_hashdrng") != True):
            Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_hashdrng", True)                
    if(autoConnect == True):
        res = Database.connectDependencies(autoConnectTableCrypto)  

def tcpipCryptoDisable(): 
    if(Database.getComponentByID("lib_crypto") != None):
        res = Database.deactivateComponents(["lib_crypto"]) 
    if(Database.getComponentByID("lib_wolfcrypt") != None):
        res = Database.deactivateComponents(["lib_wolfcrypt"])
    


def tcpipTcpGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])
    
def onAttachmentConnected(source, target):
    if (source["id"] == "tcpipIPv4") or (source["id"] == "tcpipIPv6") : 
        Database.clearSymbolValue("tcpipTcp", "TCPIP_USE_TCP")
        Database.setSymbolValue("tcpipTcp", "TCPIP_USE_TCP", True, 2)
        
def onAttachmentDisconnected(source, target):
    if (source["id"] == "tcpipIPv4") or (source["id"] == "tcpipIPv6") : 
        Database.clearSymbolValue("tcpipTcp", "TCPIP_USE_TCP")
        Database.setSymbolValue("tcpipTcp", "TCPIP_USE_TCP", False, 2)

#Set symbols of other components
def setVal(component, symbol, value):
    triggerDict = {"Component":component,"Id":symbol, "Value":value}
    if(Database.sendMessage(component, "SET_SYMBOL", triggerDict) == None):
        print "Set Symbol Failure" + component + ":" + symbol + ":" + str(value)
        return False
    else:
        return True
        
def finalizeComponent(tcpipTcpComponent):
    # Enable Sys Random H2 Adapter. if not enabled
    if(Database.getSymbolValue("tcpipStack", "TCPIP_STACK_SYS_RANDOM_H2_ADAPTER") != True): 
        setVal("tcpipStack", "TCPIP_STACK_SYS_RANDOM_H2_ADAPTER", True)
            
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
      
  
def destroyComponent(tcpipTcpComponent):
    Database.setSymbolValue("tcpipTcp", "TCPIP_USE_TCP", False, 2)
    if(Database.getSymbolValue("tcpipStack", "TCPIP_STACK_SYS_RANDOM_H2_ADAPTER") != False): 
        setVal("tcpipStack", "TCPIP_STACK_SYS_RANDOM_H2_ADAPTER", False)
