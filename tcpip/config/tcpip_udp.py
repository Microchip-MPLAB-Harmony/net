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

    
def instantiateComponent(tcpipUdpComponent):
    print("TCPIP UDP Component")
    configName = Variables.get("__CONFIGURATION_NAME")

    # Enable UDP
    tcpipUDP = tcpipUdpComponent.createBooleanSymbol("TCPIP_USE_UDP", None)
    tcpipUDP.setLabel("Use UDP")
    tcpipUDP.setVisible(False)
    tcpipUDP.setDescription("Enable UDP")
    tcpipUDP.setDefaultValue(True)

    # Maximum number of UDP Sockets
    tcpipUdpMaxNumSocket = tcpipUdpComponent.createIntegerSymbol("TCPIP_UDP_MAX_SOCKETS", None)
    tcpipUdpMaxNumSocket.setLabel("Maximum number of Sockets")
    tcpipUdpMaxNumSocket.setVisible(True)
    tcpipUdpMaxNumSocket.setDescription("Maximum number of UDP Sockets")
    tcpipUdpMaxNumSocket.setDefaultValue(10)

    # Default TX Socket Buffer Size
    tcpipUdpSktTxSize = tcpipUdpComponent.createIntegerSymbol("TCPIP_UDP_SOCKET_DEFAULT_TX_SIZE", None)
    tcpipUdpSktTxSize.setLabel("Default TX Buffer Size")
    tcpipUdpSktTxSize.setVisible(True)
    tcpipUdpSktTxSize.setDescription("Default TX Socket Buffer Size")
    tcpipUdpSktTxSize.setDefaultValue(512)

    # Enable Calculate TX Checksum
    tcpipUdpTxUseCheckSum = tcpipUdpComponent.createBooleanSymbol("TCPIP_UDP_USE_TX_CHECKSUM", None)
    tcpipUdpTxUseCheckSum.setLabel("Calculate TX Checksum")
    tcpipUdpTxUseCheckSum.setVisible(True)
    tcpipUdpTxUseCheckSum.setDescription("Enable Calculate TX Checksum")
    tcpipUdpTxUseCheckSum.setDefaultValue(True)

    # Enable Calculate RX Checksum
    tcpipUdpRxUseCheckSum = tcpipUdpComponent.createBooleanSymbol("TCPIP_UDP_USE_RX_CHECKSUM", None)
    tcpipUdpRxUseCheckSum.setLabel("Calculate RX Checksum")
    tcpipUdpRxUseCheckSum.setVisible(True)
    tcpipUdpRxUseCheckSum.setDescription("Enable Calculate RX Checksum")
    tcpipUdpRxUseCheckSum.setDefaultValue(True)

    # Default TX Socket Queue limit
    tcpipUdpSktTxQueueLimit = tcpipUdpComponent.createIntegerSymbol("TCPIP_UDP_SOCKET_DEFAULT_TX_QUEUE_LIMIT", None)
    tcpipUdpSktTxQueueLimit.setLabel("TX Queue Limit")
    tcpipUdpSktTxQueueLimit.setVisible(True)
    tcpipUdpSktTxQueueLimit.setDescription("Default TX Socket Queue limit")
    tcpipUdpSktTxQueueLimit.setDefaultValue(3)

    # Default RX Socket Queue limit
    tcpipUdpSktRxQueueLimit = tcpipUdpComponent.createIntegerSymbol("TCPIP_UDP_SOCKET_DEFAULT_RX_QUEUE_LIMIT", None)
    tcpipUdpSktRxQueueLimit.setLabel("RX Queue Limit")
    tcpipUdpSktRxQueueLimit.setVisible(True)
    tcpipUdpSktRxQueueLimit.setDescription("Default RX Socket Queue limit")
    tcpipUdpSktRxQueueLimit.setDefaultValue(3)
    
    # Advanced Settings
    tcpipUdpAdvSettings = tcpipUdpComponent.createMenuSymbol("TCPIP_UDP_ADV_SETTING", None)
    tcpipUdpAdvSettings.setLabel("Advanced Settings")
    tcpipUdpAdvSettings.setDescription("Advanced Settings")
    tcpipUdpAdvSettings.setVisible(True)
    
    # Enable UDP Pool Buffers
    tcpipUdpPoolBuffers = tcpipUdpComponent.createBooleanSymbol("TCPIP_UDP_USE_POOL_BUFFERS", tcpipUdpAdvSettings)
    tcpipUdpPoolBuffers.setLabel("Enable Pool Buffers")
    tcpipUdpPoolBuffers.setVisible(True)
    tcpipUdpPoolBuffers.setDescription("Enable UDP Pool Buffers")
    tcpipUdpPoolBuffers.setDefaultValue(False)


    # Number of Socket Pool Buffers
    tcpipUdpSktPoolBufferNum = tcpipUdpComponent.createIntegerSymbol("TCPIP_UDP_SOCKET_POOL_BUFFERS", tcpipUdpPoolBuffers)
    tcpipUdpSktPoolBufferNum.setLabel("Pool Buffers")
    tcpipUdpSktPoolBufferNum.setVisible(False)
    tcpipUdpSktPoolBufferNum.setDescription("Number of Socket Pool Buffers")
    tcpipUdpSktPoolBufferNum.setDefaultValue(4)
    tcpipUdpSktPoolBufferNum.setDependencies(tcpipUdpMenuVisibleSingle, ["TCPIP_UDP_USE_POOL_BUFFERS"])

    # UDP Socket Pool Buffer Size
    tcpipUdpSktPoolBufferSize = tcpipUdpComponent.createIntegerSymbol("TCPIP_UDP_SOCKET_POOL_BUFFER_SIZE", tcpipUdpPoolBuffers)
    tcpipUdpSktPoolBufferSize.setLabel("Pool Buffer Size")
    tcpipUdpSktPoolBufferSize.setVisible(False)
    tcpipUdpSktPoolBufferSize.setDescription("UDP Socket Pool Buffer Size")
    tcpipUdpSktPoolBufferSize.setDefaultValue(512)
    tcpipUdpSktPoolBufferSize.setDependencies(tcpipUdpMenuVisibleSingle, ["TCPIP_UDP_USE_POOL_BUFFERS"])

    # Enable UDP Commands
    tcpipUdpCommands = tcpipUdpComponent.createBooleanSymbol("TCPIP_UDP_COMMANDS", tcpipUdpAdvSettings)
    tcpipUdpCommands.setLabel("Enable UDP Commands")
    tcpipUdpCommands.setVisible(True)
    tcpipUdpCommands.setDescription("Enable UDP Commands")
    tcpipUdpCommands.setDefaultValue(False)

    # Enable External Packet Processing
    tcpipUdpExtPktProcess = tcpipUdpComponent.createBooleanSymbol("TCPIP_UDP_EXTERN_PACKET_PROCESS", tcpipUdpAdvSettings)
    tcpipUdpExtPktProcess.setLabel("Enable External Packet Processing")
    tcpipUdpExtPktProcess.setVisible(True)
    tcpipUdpExtPktProcess.setDescription("Allows External Processing of RX Packets")
    tcpipUdpExtPktProcess.setDefaultValue(False)

    tcpipUdpheapdependency = [  "TCPIP_UDP_MAX_SOCKETS", "TCPIP_UDP_SOCKET_DEFAULT_TX_QUEUE_LIMIT", 
                                "TCPIP_UDP_SOCKET_DEFAULT_TX_SIZE", "TCPIP_UDP_USE_POOL_BUFFERS", 
                                "TCPIP_UDP_SOCKET_POOL_BUFFERS", "TCPIP_UDP_SOCKET_POOL_BUFFER_SIZE", 
                                "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]    
             
    # UDP Heap Size
    tcpipUdpHeapSize = tcpipUdpComponent.createIntegerSymbol("TCPIP_UDP_HEAP_SIZE", None)
    tcpipUdpHeapSize.setLabel("UDP Heap Size (bytes)") 
    tcpipUdpHeapSize.setVisible(False)
    tcpipUdpHeapSize.setDefaultValue(tcpipUdpHeapCalc())
    tcpipUdpHeapSize.setReadOnly(True)
    tcpipUdpHeapSize.setDependencies(tcpipUdpHeapUpdate, tcpipUdpheapdependency)   
    
    #Add to system_config.h
    tcpipUdpHeaderFtl = tcpipUdpComponent.createFileSymbol(None, None)
    tcpipUdpHeaderFtl.setSourcePath("tcpip/config/udp.h.ftl")
    tcpipUdpHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipUdpHeaderFtl.setMarkup(True)
    tcpipUdpHeaderFtl.setType("STRING")

    # Add udp.c file
    tcpipUdpSourceFile = tcpipUdpComponent.createFileSymbol(None, None)
    tcpipUdpSourceFile.setSourcePath("tcpip/src/udp.c")
    tcpipUdpSourceFile.setOutputName("udp.c")
    tcpipUdpSourceFile.setOverwrite(True)
    tcpipUdpSourceFile.setDestPath("library/tcpip/src/")
    tcpipUdpSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipUdpSourceFile.setType("SOURCE")
    tcpipUdpSourceFile.setEnabled(True)


def tcpipUdpHeapCalc(): 
    nSockets = Database.getSymbolValue("tcpipUdp","TCPIP_UDP_MAX_SOCKETS")
    nTxQueueLmt = Database.getSymbolValue("tcpipUdp","TCPIP_UDP_SOCKET_DEFAULT_TX_QUEUE_LIMIT")
    sktTxBuffSize = Database.getSymbolValue("tcpipUdp","TCPIP_UDP_SOCKET_DEFAULT_TX_SIZE")
    
    poolBuffers = 0
    poolBufferSize = 0
    if(Database.getSymbolValue("tcpipUdp","TCPIP_UDP_USE_POOL_BUFFERS") == True):
        poolBuffers = Database.getSymbolValue("tcpipUdp","TCPIP_UDP_SOCKET_POOL_BUFFERS")
        poolBufferSize = Database.getSymbolValue("tcpipUdp","TCPIP_UDP_SOCKET_POOL_BUFFER_SIZE") 
        if(poolBufferSize < 256):
            poolBufferSize = 256
    
    heap_size = nSockets * ( 4 + 96 + (nTxQueueLmt * sktTxBuffSize))+ (poolBuffers * poolBufferSize)
    return heap_size    
    
def tcpipUdpHeapUpdate(symbol, event): 
    heap_size = tcpipUdpHeapCalc()
    symbol.setValue(heap_size)
    if(event["id"] == "TCPIP_STACK_HEAP_CALC_MASK"):
        symbol.setVisible(event["value"])

def tcpipUdpMenuVisibleSingle(symbol, event):
    if (event["value"] == True):
        print("TCP Menu Visible.")      
        symbol.setVisible(True)
    else:
        print("TCP Menu Invisible.")
        symbol.setVisible(False)    
        
# make UDP options visible
def tcpipUdpMenuVisible(symbol, tcpipIPSymbol):
    tcpipIPv4 = Database.getSymbolValue("tcpipIPv4","TCPIP_STACK_USE_IPV4")
    tcpipIPv6 = Database.getSymbolValue("tcpipIPv6","TCPIP_STACK_USE_IPV6")
    
    print(Database.getSymbolValue("tcpipUdp","TCPIP_USE_UDP"))
    if(tcpipIPv4 or tcpipIPv6):
        symbol.setVisible(True)
        symbol.setValue(True,1)
        print("UDP ON")
    else:
        
        symbol.setVisible(False)
        symbol.setValue(False,1)
        print("UDP OFF")
    print(Database.getSymbolValue("tcpip","TCPIP_USE_UDP"))
    
def tcpipUdpGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])

def onAttachmentConnected(source, target):
    if (source["id"] == "tcpipIPv4") or (source["id"] == "tcpipIPv6") : 
        Database.clearSymbolValue("tcpipUdp", "TCPIP_USE_UDP")
        Database.setSymbolValue("tcpipUdp", "TCPIP_USE_UDP", True, 2)
        
def onAttachmentDisconnected(source, target):
    if (source["id"] == "tcpipIPv4") or (source["id"] == "tcpipIPv6") : 
        Database.clearSymbolValue("tcpipUdp", "TCPIP_USE_UDP")
        Database.setSymbolValue("tcpipUdp", "TCPIP_USE_UDP", False, 2)

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
    Database.setSymbolValue("tcpipUdp", "TCPIP_USE_UDP", False, 2)
    
