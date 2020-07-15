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

    
def instantiateComponent(tcpipFtpcComponent):
    print("TCPIP FTPC Client Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # Enable FTP Client
    tcpipFtpc = tcpipFtpcComponent.createBooleanSymbol("TCPIP_STACK_USE_FTP_CLIENT", None)
    tcpipFtpc.setLabel("FTP Client")
    tcpipFtpc.setVisible(False)
    tcpipFtpc.setDescription("Enable FTP Client")
    tcpipFtpc.setDefaultValue(True)
    
    # FTP Client Tick Rate in msec
    tcpipFtpcTickRate = tcpipFtpcComponent.createIntegerSymbol("TCPIP_FTPC_TASK_TICK_RATE", None)
    tcpipFtpcTickRate.setLabel("FTPC Tick Rate (in msec)")
    tcpipFtpcTickRate.setVisible(True)
    tcpipFtpcTickRate.setDescription("FTPC Tick Rate in msec")
    tcpipFtpcTickRate.setDefaultValue(5)
    
    # Transmit Buffer Size for the FTP Client Data Socket
    tcpipFtpcDataSktTxBuffSize = tcpipFtpcComponent.createIntegerSymbol("TCPIP_FTPC_DATA_SKT_TX_BUFF_SIZE_DFLT", None)
    tcpipFtpcDataSktTxBuffSize.setLabel("Default Data Socket Transmit Buffer Size")
    tcpipFtpcDataSktTxBuffSize.setVisible(True)
    tcpipFtpcDataSktTxBuffSize.setDescription("Default Transmit Buffer Size for the FTPC Data Socket")
    tcpipFtpcDataSktTxBuffSize.setDefaultValue(0)

    # Receive Buffer Size for the FTP Client Data Socket
    tcpipFtpcDataSktRxBuffSize = tcpipFtpcComponent.createIntegerSymbol("TCPIP_FTPC_DATA_SKT_RX_BUFF_SIZE_DFLT", None)
    tcpipFtpcDataSktRxBuffSize.setLabel("Default Data Socket Receive Buffer Size")
    tcpipFtpcDataSktRxBuffSize.setVisible(True)
    tcpipFtpcDataSktRxBuffSize.setDescription("Default Receive Buffer Size for the FTPC Data Socket")
    tcpipFtpcDataSktRxBuffSize.setDefaultValue(0)
    
    # FTP Client request Time-Out
    tcpipFtpcReqTimeout = tcpipFtpcComponent.createIntegerSymbol("TCPIP_FTPC_TMO", None)
    tcpipFtpcReqTimeout.setLabel("FTP Request Time-out (in sec)")
    tcpipFtpcReqTimeout.setVisible(True)
    tcpipFtpcReqTimeout.setDescription("FTP Request Time-out in seconds")
    tcpipFtpcReqTimeout.setDefaultValue(2)

    # FTP Client maximum number of simultaneous client connections
    tcpipFtpcMaxNumClient = tcpipFtpcComponent.createIntegerSymbol("TCPIP_FTPC_MAX_NUM_CLIENT", None)
    tcpipFtpcMaxNumClient.setLabel("Maximum number of simultaneous client")
    tcpipFtpcMaxNumClient.setVisible(True)
    tcpipFtpcMaxNumClient.setDescription("Maximum number of simultaneous client")
    tcpipFtpcMaxNumClient.setDefaultValue(3)    
    
    # Enable FTPC Commands
    tcpipFtpcCmdEnable = tcpipFtpcComponent.createBooleanSymbol("TCPIP_FTPC_COMMANDS", None) 
    tcpipFtpcCmdEnable.setLabel("Enable FTP Client Commands")
    tcpipFtpcCmdEnable.setVisible(True)
    tcpipFtpcCmdEnable.setDescription("Enable FTP Client Commands")
    tcpipFtpcCmdEnable.setDefaultValue(False)

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
    
