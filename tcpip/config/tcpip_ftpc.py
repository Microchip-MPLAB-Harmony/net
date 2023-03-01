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

    
def instantiateComponent(tcpipFtpcComponent):
    print("TCPIP FTPC Client Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # Enable FTP Client
    tcpipFtpc = tcpipFtpcComponent.createBooleanSymbol("TCPIP_STACK_USE_FTP_CLIENT", None)
    tcpipFtpc.setHelp("mcc_h3_ftpc_configurations")
    tcpipFtpc.setLabel("FTP Client")
    tcpipFtpc.setVisible(False)
    tcpipFtpc.setDescription("Enable FTP Client")
    tcpipFtpc.setDefaultValue(True)

    # FTP Client maximum number of simultaneous client connections
    tcpipFtpcMaxNumClient = tcpipFtpcComponent.createIntegerSymbol("TCPIP_FTPC_MAX_NUM_CLIENT", None)
    tcpipFtpcMaxNumClient.setHelp("mcc_h3_ftpc_configurations")
    tcpipFtpcMaxNumClient.setLabel("Maximum number of simultaneous client")
    tcpipFtpcMaxNumClient.setVisible(True)
    tcpipFtpcMaxNumClient.setDescription("Maximum number of simultaneous client")
    tcpipFtpcMaxNumClient.setDefaultValue(3)
    
    # Transmit Buffer Size for the FTP Client Data Socket
    tcpipFtpcDataSktTxBuffSize = tcpipFtpcComponent.createIntegerSymbol("TCPIP_FTPC_DATA_SKT_TX_BUFF_SIZE_DFLT", None)
    tcpipFtpcDataSktTxBuffSize.setHelp("mcc_h3_ftpc_configurations")
    tcpipFtpcDataSktTxBuffSize.setLabel("Default Data Socket Transmit Buffer Size")
    tcpipFtpcDataSktTxBuffSize.setVisible(True)
    tcpipFtpcDataSktTxBuffSize.setDescription("Default Transmit Buffer Size for the FTPC Data Socket")
    tcpipFtpcDataSktTxBuffSize.setDefaultValue(0)

    # Receive Buffer Size for the FTP Client Data Socket
    tcpipFtpcDataSktRxBuffSize = tcpipFtpcComponent.createIntegerSymbol("TCPIP_FTPC_DATA_SKT_RX_BUFF_SIZE_DFLT", None)
    tcpipFtpcDataSktRxBuffSize.setHelp("mcc_h3_ftpc_configurations")
    tcpipFtpcDataSktRxBuffSize.setLabel("Default Data Socket Receive Buffer Size")
    tcpipFtpcDataSktRxBuffSize.setVisible(True)
    tcpipFtpcDataSktRxBuffSize.setDescription("Default Receive Buffer Size for the FTPC Data Socket")
    tcpipFtpcDataSktRxBuffSize.setDefaultValue(0)
    
    # Enable FTPC Commands
    tcpipFtpcCmdEnable = tcpipFtpcComponent.createBooleanSymbol("TCPIP_FTPC_COMMANDS", None) 
    tcpipFtpcCmdEnable.setHelp("mcc_h3_ftpc_configurations")
    tcpipFtpcCmdEnable.setLabel("Enable Console Commands")
    tcpipFtpcCmdEnable.setVisible(True)
    tcpipFtpcCmdEnable.setDescription("Enable FTP Client Console Commands")
    tcpipFtpcCmdEnable.setDefaultValue(False)
        
    # Advanced Settings
    tcpipFtpcAdvSettings = tcpipFtpcComponent.createMenuSymbol("TCPIP_FTPC_ADV_SETTING", None)
    tcpipFtpcAdvSettings.setLabel("Advanced Settings")
    tcpipFtpcAdvSettings.setDescription("Advanced Settings")
    tcpipFtpcAdvSettings.setVisible(True)
    
    # FTP Client Tick Rate in msec
    tcpipFtpcTickRate = tcpipFtpcComponent.createIntegerSymbol("TCPIP_FTPC_TASK_TICK_RATE", tcpipFtpcAdvSettings)
    tcpipFtpcTickRate.setHelp("mcc_h3_ftpc_configurations")
    tcpipFtpcTickRate.setLabel("FTPC Tick Rate (in msec)")
    tcpipFtpcTickRate.setVisible(True)
    tcpipFtpcTickRate.setDescription("FTPC Tick Rate in msec")
    tcpipFtpcTickRate.setDefaultValue(5)
    
    # FTP Client request Time-Out
    tcpipFtpcReqTimeout = tcpipFtpcComponent.createIntegerSymbol("TCPIP_FTPC_TMO", tcpipFtpcAdvSettings)
    tcpipFtpcReqTimeout.setHelp("mcc_h3_ftpc_configurations")
    tcpipFtpcReqTimeout.setLabel("FTP Request Time-out (in sec)")
    tcpipFtpcReqTimeout.setVisible(True)
    tcpipFtpcReqTimeout.setDescription("FTP Request Time-out in seconds")
    tcpipFtpcReqTimeout.setDefaultValue(2)
    
    tcpipFtpcheapdependency = [ "TCPIP_FTPC_MAX_NUM_CLIENT", "TCPIP_FTPC_DATA_SKT_TX_BUFF_SIZE_DFLT", 
                                "TCPIP_FTPC_DATA_SKT_RX_BUFF_SIZE_DFLT", "tcpipTcp.TCPIP_TCP_SOCKET_DEFAULT_TX_SIZE", 
                                "tcpipTcp.TCPIP_TCP_SOCKET_DEFAULT_RX_SIZE", "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK" ]    

    # FTPC Heap Size
    tcpipFtpcHeapSize = tcpipFtpcComponent.createIntegerSymbol("TCPIP_FTPC_HEAP_SIZE", None)
    tcpipFtpcHeapSize.setLabel("FTPC Heap Size (bytes)") 
    tcpipFtpcHeapSize.setVisible(False)
    tcpipFtpcHeapSize.setDefaultValue(tcpipFtpcHeapCalc())
    tcpipFtpcHeapSize.setReadOnly(True)
    tcpipFtpcHeapSize.setDependencies(tcpipFtpcHeapUpdate, tcpipFtpcheapdependency)  
    
    #Add to system_config.h
    tcpipFtpcHeaderFtl = tcpipFtpcComponent.createFileSymbol(None, None)
    tcpipFtpcHeaderFtl.setSourcePath("tcpip/config/ftpc.h.ftl")
    tcpipFtpcHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipFtpcHeaderFtl.setMarkup(True)
    tcpipFtpcHeaderFtl.setType("STRING")

    # Add dhcp.c file
    tcpipFtpcSourceFile = tcpipFtpcComponent.createFileSymbol(None, None)
    tcpipFtpcSourceFile.setSourcePath("tcpip/src/ftpc.c")
    tcpipFtpcSourceFile.setOutputName("ftpc.c")
    tcpipFtpcSourceFile.setOverwrite(True)
    tcpipFtpcSourceFile.setDestPath("library/tcpip/src/")
    tcpipFtpcSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipFtpcSourceFile.setType("SOURCE")
    tcpipFtpcSourceFile.setEnabled(True)



def tcpipFtpcHeapCalc(): 
    ctrlSktTxBuffSize = 0
    ctrlSktRxBuffSize = 0
    nMaxClients = Database.getSymbolValue("tcpipFtpc","TCPIP_FTPC_MAX_NUM_CLIENT")
    if((Database.getSymbolValue("tcpipTcp", "TCPIP_USE_TCP") == True)): 
        ctrlSktTxBuffSize = Database.getSymbolValue("tcpipTcp","TCPIP_TCP_SOCKET_DEFAULT_TX_SIZE")
        ctrlSktRxBuffSize = Database.getSymbolValue("tcpipTcp","TCPIP_TCP_SOCKET_DEFAULT_RX_SIZE")
    
    dataTxBuffSize = Database.getSymbolValue("tcpipFtpc","TCPIP_FTPC_DATA_SKT_TX_BUFF_SIZE_DFLT")
    if(dataTxBuffSize == 0):   
        if((Database.getSymbolValue("tcpipTcp", "TCPIP_USE_TCP") == True)):
            dataTxBuffSize = Database.getSymbolValue("tcpipTcp","TCPIP_TCP_SOCKET_DEFAULT_TX_SIZE")
    
    dataRxBuffSize = Database.getSymbolValue("tcpipFtpc","TCPIP_FTPC_DATA_SKT_RX_BUFF_SIZE_DFLT")
    if(dataRxBuffSize == 0):   
        if((Database.getSymbolValue("tcpipTcp", "TCPIP_USE_TCP") == True)):
            dataRxBuffSize = Database.getSymbolValue("tcpipTcp","TCPIP_TCP_SOCKET_DEFAULT_RX_SIZE")
    
    heap_size = nMaxClients * (180 + ctrlSktTxBuffSize +  ctrlSktRxBuffSize + (2 * dataTxBuffSize) + (2 * dataRxBuffSize))
    return heap_size    
    
def tcpipFtpcHeapUpdate(symbol, event): 
    heap_size = tcpipFtpcHeapCalc()
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
    Database.setSymbolValue("tcpipFtpc", "TCPIP_STACK_USE_FTP_CLIENT", False, 2)
    
