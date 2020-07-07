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

    
def instantiateComponent(tcpipTcpComponent):
    print("TCPIP TCP Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # Enable TCP
    tcpipTCP = tcpipTcpComponent.createBooleanSymbol("TCPIP_USE_TCP", None)
    tcpipTCP.setLabel("Use TCP")
    tcpipTCP.setVisible(False)
    tcpipTCP.setDescription("Enable TCP")
    tcpipTCP.setDefaultValue(True)

    # Maximum TX Segment Size
    tcpipTcpMaxTxSegSize = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_MAX_SEG_SIZE_TX", None)
    tcpipTcpMaxTxSegSize.setLabel("Maximum TX Segment Size")
    tcpipTcpMaxTxSegSize.setVisible(True)
    tcpipTcpMaxTxSegSize.setDescription("Maximum TX Segment Size")
    tcpipTcpMaxTxSegSize.setDefaultValue(1460)

    # Default TX Socket Size
    tcpipTcpSktTxSize = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_SOCKET_DEFAULT_TX_SIZE", None)
    tcpipTcpSktTxSize.setLabel("Default TX Size")
    tcpipTcpSktTxSize.setVisible(True)
    tcpipTcpSktTxSize.setDescription("Default TX Socket Size")
    tcpipTcpSktTxSize.setDefaultValue(512)

    # Default RX Socket Size
    tcpipTcpSktRxSize = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_SOCKET_DEFAULT_RX_SIZE", None)
    tcpipTcpSktRxSize.setLabel("Default RX Size")
    tcpipTcpSktRxSize.setVisible(True)
    tcpipTcpSktRxSize.setDescription("Default RX Socket Size")
    tcpipTcpSktRxSize.setDefaultValue(512)

    # Enable TCP Sockets Dynamic Options
    tcpipTcpDynOptions = tcpipTcpComponent.createBooleanSymbol("TCPIP_TCP_DYNAMIC_OPTIONS", None)
    tcpipTcpDynOptions.setLabel("Enable TCP Sockets Dynamic Options")
    tcpipTcpDynOptions.setVisible(True)
    tcpipTcpDynOptions.setDescription("Enable TCP Sockets Dynamic Options")
    tcpipTcpDynOptions.setDefaultValue(True)

    # Start Timeout in ms
    tcpipTcpStartTimeout = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_START_TIMEOUT_VAL", None)
    tcpipTcpStartTimeout.setLabel("Start Time-out - ms")
    tcpipTcpStartTimeout.setVisible(True)
    tcpipTcpStartTimeout.setDescription("Start Timeout in ms")
    tcpipTcpStartTimeout.setDefaultValue(1000)

    # Delayed Ack Timeout in ms
    tcpipTcpDelayAckTimeout = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_DELAYED_ACK_TIMEOUT", None)
    tcpipTcpDelayAckTimeout.setLabel("Delayed Ack Time-out - ms")
    tcpipTcpDelayAckTimeout.setVisible(True)
    tcpipTcpDelayAckTimeout.setDescription("Delayed Ack Timeout in ms")
    tcpipTcpDelayAckTimeout.setDefaultValue(100)

    # Fin Wait 2 Timeout in ms
    tcpipTcpFinWait2Timeout = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_FIN_WAIT_2_TIMEOUT", None)
    tcpipTcpFinWait2Timeout.setLabel("Fin Wait 2 Time-out - ms")
    tcpipTcpFinWait2Timeout.setVisible(True)
    tcpipTcpFinWait2Timeout.setDescription("Fin Wait 2 Timeout in ms")
    tcpipTcpFinWait2Timeout.setDefaultValue(5000)

    # Keep Alive Timeout in ms
    tcpipTcpKeepAliveTimeout = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_KEEP_ALIVE_TIMEOUT", None)
    tcpipTcpKeepAliveTimeout.setLabel("Keep Alive Time-out - ms")
    tcpipTcpKeepAliveTimeout.setVisible(True)
    tcpipTcpKeepAliveTimeout.setDescription("Keep Alive Timeout in ms")
    tcpipTcpKeepAliveTimeout.setDefaultValue(10000)

    # Maximum number of retries
    tcpipTcpMaxRetryNum = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_MAX_RETRIES", None)
    tcpipTcpMaxRetryNum.setLabel("Maximum Retries")
    tcpipTcpMaxRetryNum.setVisible(True)
    tcpipTcpMaxRetryNum.setDescription("Maximum number of retries")
    tcpipTcpMaxRetryNum.setDefaultValue(5)

    # Advanced Settings
    tcpipTcpAdvSettings = tcpipTcpComponent.createMenuSymbol("TCPIP_TCP_ADV_SETTING", None)
    tcpipTcpAdvSettings.setLabel("Advanced Settings")
    tcpipTcpAdvSettings.setDescription("Advanced Settings")
    tcpipTcpAdvSettings.setVisible(True)
    
    # Maximum number of Unacked Keep Alives
    tcpipTcpMaxUnackKeepAlivesNum = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_MAX_UNACKED_KEEP_ALIVES", tcpipTcpAdvSettings)
    tcpipTcpMaxUnackKeepAlivesNum.setLabel("Maximum Unacked Keep Alives")
    tcpipTcpMaxUnackKeepAlivesNum.setVisible(True)
    tcpipTcpMaxUnackKeepAlivesNum.setDescription("Maximum number of Unacked Keep Alives")
    tcpipTcpMaxUnackKeepAlivesNum.setDefaultValue(6)

    # Maximum number of Syn Retries
    tcpipTcpMaxSynRetryNum = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_MAX_SYN_RETRIES", tcpipTcpAdvSettings)
    tcpipTcpMaxSynRetryNum.setLabel("Maximum SYN Retries")
    tcpipTcpMaxSynRetryNum.setVisible(True)
    tcpipTcpMaxSynRetryNum.setDescription("Maximum number of Syn Retries")
    tcpipTcpMaxSynRetryNum.setDefaultValue(3)
        
    # Auto Transmit Timeout in ms
    tcpipTcpAutoTxTimeout = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_AUTO_TRANSMIT_TIMEOUT_VAL", tcpipTcpAdvSettings)
    tcpipTcpAutoTxTimeout.setLabel("Auto Transmit Time-out - ms")
    tcpipTcpAutoTxTimeout.setVisible(True)
    tcpipTcpAutoTxTimeout.setDescription("Auto Transmit Timeout in ms")
    tcpipTcpAutoTxTimeout.setDefaultValue(40)

    # Window Update Time-out
    tcpipTcpWindowUpdateTimeout = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_WINDOW_UPDATE_TIMEOUT_VAL", tcpipTcpAdvSettings)
    tcpipTcpWindowUpdateTimeout.setLabel("Window Update Time-out")
    tcpipTcpWindowUpdateTimeout.setVisible(True)
    tcpipTcpWindowUpdateTimeout.setDescription("Window Update Time-out")
    tcpipTcpWindowUpdateTimeout.setDefaultValue(200)

    # Close Wait Timeout in ms
    tcpipTcpCloseWaitTimeout = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_CLOSE_WAIT_TIMEOUT", tcpipTcpAdvSettings)
    tcpipTcpCloseWaitTimeout.setLabel("OBSOLETE! Close Wait Time-out - ms. Should be 0!")
    tcpipTcpCloseWaitTimeout.setVisible(True)
    tcpipTcpCloseWaitTimeout.setDescription("OBSOLETE! Close Wait Timeout in ms. Should be 0!")
    tcpipTcpCloseWaitTimeout.setDefaultValue(0)

    # Maximum number of sockets
    tcpipTcpMaxSktNum = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_MAX_SOCKETS", tcpipTcpAdvSettings)
    tcpipTcpMaxSktNum.setLabel("Maximum Sockets")
    tcpipTcpMaxSktNum.setVisible(True)
    tcpipTcpMaxSktNum.setDescription("Maximum number of sockets")
    tcpipTcpMaxSktNum.setDefaultValue(10)

    # TCP Task Tick Rate
    tcpipTcpTaskTickRate = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_TASK_TICK_RATE", tcpipTcpAdvSettings)
    tcpipTcpTaskTickRate.setLabel("TCP Task Rate")
    tcpipTcpTaskTickRate.setVisible(True)
    tcpipTcpTaskTickRate.setDescription("TCP Task Tick Rate")
    tcpipTcpTaskTickRate.setDefaultValue(5)

    # Maximum Segment Length (MSL) Timeout in seconds
    tcpipTcpMaxSegLenTimeout = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_MSL_TIMEOUT", tcpipTcpAdvSettings)
    tcpipTcpMaxSegLenTimeout.setLabel("Maximum Segment Length (MSL) Timeout - seconds")
    tcpipTcpMaxSegLenTimeout.setVisible(True)
    tcpipTcpMaxSegLenTimeout.setDescription("Maximum Segment Length (MSL) Timeout in seconds")
    tcpipTcpMaxSegLenTimeout.setDefaultValue(0)

    # TCP quiet time in seconds
    tcpipTcpQuietTime = tcpipTcpComponent.createIntegerSymbol("TCPIP_TCP_QUIET_TIME", tcpipTcpAdvSettings)
    tcpipTcpQuietTime.setLabel("TCP quiet time - seconds")
    tcpipTcpQuietTime.setVisible(True)
    tcpipTcpQuietTime.setDescription("TCP quiet time in seconds")
    tcpipTcpQuietTime.setDefaultValue(0)

    # Enable TCP Commands
    tcpipTcpCommands = tcpipTcpComponent.createBooleanSymbol("TCPIP_TCP_COMMANDS", tcpipTcpAdvSettings)
    tcpipTcpCommands.setLabel("Enable TCP Commands")
    tcpipTcpCommands.setVisible(True)
    tcpipTcpCommands.setDescription("Enable TCP Commands")
    tcpipTcpCommands.setDefaultValue(False)

    # Enable External Packet Processing
    tcpipTcpExtPktProcess = tcpipTcpComponent.createBooleanSymbol("TCPIP_TCP_EXTERN_PACKET_PROCESS", tcpipTcpAdvSettings)
    tcpipTcpExtPktProcess.setLabel("Enable External Packet Processing")
    tcpipTcpExtPktProcess.setVisible(True)
    tcpipTcpExtPktProcess.setDescription("Allows External Processing of RX Packets")
    tcpipTcpExtPktProcess.setDefaultValue(False)
   
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
    
    ownSockets = (nSockets - nHTTP_NET_Connections - nHTTP_Connections - nTelnetConnections - nMailConnections - nFTPSConnections  - nIperfInstances)
    
    heap_size = ownSockets * (sktTxBuffSize + sktRxBuffSize) + (nSockets * 132) \
                + nHTTP_NET_Connections * (176 + sktTxBuffSize_HTTPNET + sktRxBuffSize_HTTPNET) \
                + nHTTP_Connections * (176 + sktTxBuffSize_HTTP + sktRxBuffSize_HTTP) \
                + nTelnetConnections * (sktTxBuffSize_Telnet + sktRxBuffSize_Telnet) \
                + nMailConnections * (176 + sktTxBuffSize_Smtpc + sktRxBuffSize_Smtpc) \
                + nFTPSConnections * (sktTxBuffSize_Ftps + sktRxBuffSize_Ftps) \
                + nIperfInstances * (sktTxBuffSize_iperf + sktRxBuffSize_iperf)
                    
    return heap_size    
    
def tcpipTcpHeapUpdate(symbol, event): 
    heap_size = tcpipTcpHeapCalc()
    symbol.setValue(heap_size)
    if(event["id"] == "TCPIP_STACK_HEAP_CALC_MASK"):
        symbol.setVisible(event["value"])


def tcpipTcpMenuVisibleSingle(symbol, event):
    if (event["value"] == True):
        print("TCP Menu Visible.")      
        symbol.setVisible(True)
    else:
        print("TCP Menu Invisible.")
        symbol.setVisible(False)    
        
# make TCP options visible
def tcpipTcpMenuVisible(symbol, tcpipIPSymbol):
    tcpipIPv4 = Database.getSymbolValue("tcpipIPv4","TCPIP_STACK_USE_IPV4")
    tcpipIPv6 = Database.getSymbolValue("tcpipIPv6","TCPIP_STACK_USE_IPV6")
    print("TCP Menu")
    if(tcpipIPv4 or tcpipIPv6):
        symbol.setVisible(True)
        symbol.setValue(True,1)
    else:
        symbol.setVisible(False)
        symbol.setValue(False,1)

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
      
  
def destroyComponent(component):
    Database.setSymbolValue("tcpipTcp", "TCPIP_USE_TCP", False, 2)
