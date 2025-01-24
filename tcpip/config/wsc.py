"""
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

    
def instantiateComponent(tcpipWscComponent):
    print("TCPIP WS Client Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # Use WS Client
    tcpipWsClient = tcpipWscComponent.createBooleanSymbol("TCPIP_STACK_USE_WS_CLIENT", None)
    tcpipWsClient.setHelp("mcc_h3_wsc_configurations")
    tcpipWsClient.setLabel("Use WSC")
    tcpipWsClient.setVisible(False)
    tcpipWsClient.setDescription("Use WS Client")
    tcpipWsClient.setDefaultValue(True) 

    # Number of WSC Connections to be Created
    tcpipWscConnNum = tcpipWscComponent.createIntegerSymbol("TCPIP_WSC_CONNECTIONS", None)
    tcpipWscConnNum.setHelp("mcc_h3_wsc_configurations")
    tcpipWscConnNum.setLabel("Number of WSC Connections to be Created")
    tcpipWscConnNum.setVisible(True)
    tcpipWscConnNum.setDescription("Number of WSC Connections to be Created")
    tcpipWscConnNum.setDefaultValue(2)

    # Size of the TX Buffer for the WSC Socket
    tcpipWscSktTxBuffSize = tcpipWscComponent.createIntegerSymbol("TCPIP_WSC_SKT_TX_BUFF_SIZE", None)
    tcpipWscSktTxBuffSize.setHelp("mcc_h3_wsc_configurations")
    tcpipWscSktTxBuffSize.setLabel("WSC Socket TX Buffer Size")
    tcpipWscSktTxBuffSize.setVisible(True)
    tcpipWscSktTxBuffSize.setDescription("Size of the TX Buffer for the WSC Socket")
    tcpipWscSktTxBuffSize.setDefaultValue(2048)

    # Size of the RX Buffer for the WSC Socket
    tcpipWscSktRxBuffSize = tcpipWscComponent.createIntegerSymbol("TCPIP_WSC_SKT_RX_BUFF_SIZE", None)
    tcpipWscSktRxBuffSize.setHelp("mcc_h3_wsc_configurations")
    tcpipWscSktRxBuffSize.setLabel("WSC Socket RX Buffer Size")
    tcpipWscSktRxBuffSize.setVisible(True)
    tcpipWscSktRxBuffSize.setDescription("Size of the RX Buffer for the WSC Socket")
    tcpipWscSktRxBuffSize.setDefaultValue(2048)

    # Enable WSC Commands
    tcpipWscCommands = tcpipWscComponent.createBooleanSymbol("TCPIP_WSC_COMMANDS", None)
    tcpipWscCommands.setHelp("mcc_h3_wsc_configurations")
    tcpipWscCommands.setLabel("Enable WSC Console Commands")
    tcpipWscCommands.setVisible(True)
    tcpipWscCommands.setDescription("Enable WSC Commands")
    tcpipWscCommands.setDefaultValue(False)


    # Advanced Settings
    tcpipWscAdvSettings = tcpipWscComponent.createMenuSymbol("TCPIP_WSC_ADV_SETTING", None)
    tcpipWscAdvSettings.setLabel("Advanced Settings")
    tcpipWscAdvSettings.setDescription("Advanced Settings")
    tcpipWscAdvSettings.setVisible(True)
    
    # WSC Task Tick Rate in ms
    tcpipWscTaskTickRate = tcpipWscComponent.createIntegerSymbol("TCPIP_WSC_TASK_RATE", tcpipWscAdvSettings)
    tcpipWscTaskTickRate.setHelp("mcc_h3_wsc_configurations")
    tcpipWscTaskTickRate.setLabel("WSC Task Rate (in msec)")
    tcpipWscTaskTickRate.setVisible(True)
    tcpipWscTaskTickRate.setDescription("WSC Task Tick Rate in ms")
    tcpipWscTaskTickRate.setDefaultValue(33)

    # Server Response Timeout in milliseconds
    tcpipWscServerReplyTimeout = tcpipWscComponent.createIntegerSymbol("TCPIP_WSC_SERVER_REPLY_TIMEOUT", tcpipWscAdvSettings)
    tcpipWscServerReplyTimeout.setHelp("mcc_h3_wsc_configurations")
    tcpipWscServerReplyTimeout.setLabel("Server Response Timeout (in milli-sec)")
    tcpipWscServerReplyTimeout.setVisible(True)
    tcpipWscServerReplyTimeout.setDescription("Server Response Timeout in milli-seconds")
    tcpipWscServerReplyTimeout.setDefaultValue(2000)

    # User Read Timeout in milliseconds
    tcpipWscUserReadTimeout = tcpipWscComponent.createIntegerSymbol("TCPIP_WSC_USER_READ_TIMEOUT", tcpipWscAdvSettings)
    tcpipWscUserReadTimeout.setHelp("mcc_h3_wsc_configurations")
    tcpipWscUserReadTimeout.setLabel("User Read Timeout (in milli-sec)")
    tcpipWscUserReadTimeout.setVisible(True)
    tcpipWscUserReadTimeout.setDescription("User Read Message Timeout in milli-seconds")
    tcpipWscUserReadTimeout.setDefaultValue(500)

    # Configuration flags - not supported for now

    # WSC maximum size a WS server/host name string could take
    tcpipWscSrvMaxLen = tcpipWscComponent.createIntegerSymbol("TCPIP_WSC_SERVER_MAX_LEN", tcpipWscAdvSettings)
    tcpipWscSrvMaxLen.setHelp("mcc_h3_wsc_configurations")
    tcpipWscSrvMaxLen.setLabel("WSC Server Name Size")
    tcpipWscSrvMaxLen.setVisible(True)
    tcpipWscSrvMaxLen.setDescription("WSC Max Server Name Size")
    tcpipWscSrvMaxLen.setDefaultValue(64)

    # WSC maximum size a WS resource string could take
    tcpipWscResourceMaxLen = tcpipWscComponent.createIntegerSymbol("TCPIP_WSC_RESOURCE_MAX_LEN", tcpipWscAdvSettings)
    tcpipWscResourceMaxLen.setHelp("mcc_h3_wsc_configurations")
    tcpipWscResourceMaxLen.setLabel("WSC Resource String Size")
    tcpipWscResourceMaxLen.setVisible(True)
    tcpipWscResourceMaxLen.setDescription("WSC Max Resource String Size")
    tcpipWscResourceMaxLen.setDefaultValue(64)

    # WSC maximum size a WS sub-protocol name string could take
    tcpipWscProtoMaxLen = tcpipWscComponent.createIntegerSymbol("TCPIP_WSC_PROTO_MAX_LEN", tcpipWscAdvSettings)
    tcpipWscProtoMaxLen.setHelp("mcc_h3_wsc_configurations")
    tcpipWscProtoMaxLen.setLabel("WSC Protocol Name Size")
    tcpipWscProtoMaxLen.setVisible(True)
    tcpipWscProtoMaxLen.setDescription("WSC Max Protocol Name Size")
    tcpipWscProtoMaxLen.setDefaultValue(16)

    # WSC maximum number of sub-protocols to be requested
    tcpipWscProtoMaxNo = tcpipWscComponent.createIntegerSymbol("TCPIP_WSC_PROTO_MAX_NO", tcpipWscAdvSettings)
    tcpipWscProtoMaxNo.setHelp("mcc_h3_wsc_configurations")
    tcpipWscProtoMaxNo.setLabel("WSC Protocol Number")
    tcpipWscProtoMaxNo.setVisible(True)
    tcpipWscProtoMaxNo.setDescription("WSC Max Protocol Number")
    tcpipWscProtoMaxNo.setDefaultValue(2)

    # WSC semaphore configuration
    tcpipWscSemConn = tcpipWscComponent.createBooleanSymbol("TCPIP_WSC_CONNECTION_SEM", tcpipWscAdvSettings)
    tcpipWscSemConn.setHelp("mcc_h3_wsc_configurations")
    tcpipWscSemConn.setLabel("Enable WSC Semaphore per Connection")
    tcpipWscSemConn.setVisible(True)
    tcpipWscSemConn.setDescription("WSC One Synchronization Semaphore per Connection")
    tcpipWscSemConn.setDefaultValue(True)

    # WSC large frame support - not implemented
    tcpipWscLargeFrame = tcpipWscComponent.createBooleanSymbol("TCPIP_WSC_LARGE_FRAME_SUPPORT", tcpipWscAdvSettings)
    tcpipWscLargeFrame.setHelp("mcc_h3_wsc_configurations")
    tcpipWscLargeFrame.setLabel("Enable WSC large frame support")
    tcpipWscLargeFrame.setVisible(True)
    tcpipWscLargeFrame.setDescription("WSC large frame support - not implemented")
    tcpipWscLargeFrame.setDefaultValue(False)
    tcpipWscLargeFrame.setReadOnly(True)

    tcpipWscheapdependency = ["TCPIP_WSC_CONNECTIONS", "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]    
        
    # WSC Heap Size
    tcpipWscHeapSize = tcpipWscComponent.createIntegerSymbol("TCPIP_WSC_HEAP_SIZE", None)
    tcpipWscHeapSize.setLabel("WSC Heap Size (bytes)") 
    tcpipWscHeapSize.setVisible(False)
    tcpipWscHeapSize.setDefaultValue(tcpipWscHeapCalc())
    tcpipWscHeapSize.setReadOnly(True)
    tcpipWscHeapSize.setDependencies(tcpipWscHeapUpdate, tcpipWscheapdependency)  
    
    # Add to system_config.h
    tcpipWscHeaderFtl = tcpipWscComponent.createFileSymbol(None, None)
    tcpipWscHeaderFtl.setSourcePath("tcpip/config/wsc.h.ftl")
    tcpipWscHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipWscHeaderFtl.setMarkup(True)
    tcpipWscHeaderFtl.setType("STRING")

    # Add wsc.c file
    tcpipWscSourceFile = tcpipWscComponent.createFileSymbol(None, None)
    tcpipWscSourceFile.setSourcePath("tcpip/src/wsc.c")
    tcpipWscSourceFile.setOutputName("wsc.c")
    tcpipWscSourceFile.setOverwrite(True)
    tcpipWscSourceFile.setDestPath("library/tcpip/src/")
    tcpipWscSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipWscSourceFile.setType("SOURCE")
    tcpipWscSourceFile.setEnabled(True)
    
def tcpipWscMenuVisibleSingle(symbol, event):
    if (event["value"] == True):
        print("WSC Menu Visible.")        
        symbol.setVisible(True)
    else:
        print("WSC Menu Invisible.")
        symbol.setVisible(False)

def tcpipWscGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])

def tcpipWscHeapCalc(): 
    numConn = Database.getSymbolValue("tcpipWSC","TCPIP_WSC_CONNECTIONS")
    heap_size = numConn * 176
    return heap_size    
    
def tcpipWscHeapUpdate(symbol, event): 
    heap_size = tcpipWscHeapCalc()
    symbol.setValue(heap_size)
    if(event["id"] == "TCPIP_STACK_HEAP_CALC_MASK"):
        symbol.setVisible(event["value"])

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
    Database.setSymbolValue("tcpipWSC", "TCPIP_STACK_USE_WS_CLIENT", False, 2)
    
