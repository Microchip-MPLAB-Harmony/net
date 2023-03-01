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

def instantiateComponent(tcpipTftpcComponent):
    print("TCPIP TFTPC Component")
    configName = Variables.get("__CONFIGURATION_NAME")

    # Use TFTP Client Module
    tcpipTftpc = tcpipTftpcComponent.createBooleanSymbol("TCPIP_USE_TFTPC_MODULE", None)
    tcpipTftpc.setHelp("mcc_h3_tftpc_configurations")
    tcpipTftpc.setLabel("Use TFTP Client Module")
    tcpipTftpc.setVisible(False)
    tcpipTftpc.setDescription("Use TFTP Client Module")
    tcpipTftpc.setDefaultValue(True)

    # TFTP Client Maximum retries
    tcpipTftpcRetryMax= tcpipTftpcComponent.createIntegerSymbol("TCPIP_TFTPC_MAX_RETRIES", None)
    tcpipTftpcRetryMax.setHelp("mcc_h3_tftpc_configurations")
    tcpipTftpcRetryMax.setLabel("TFTP Client Maximum Retries")
    tcpipTftpcRetryMax.setVisible(True)
    tcpipTftpcRetryMax.setDescription("TFTP Client Maximum retries")
    tcpipTftpcRetryMax.setDefaultValue(3)

    # Maximum Length for a file name
    tcpipTftpcFilenameLen= tcpipTftpcComponent.createIntegerSymbol("TCPIP_TFTPC_FILENAME_LEN", None)
    tcpipTftpcFilenameLen.setHelp("mcc_h3_tftpc_configurations")
    tcpipTftpcFilenameLen.setLabel("Maximum Length for File name")
    tcpipTftpcFilenameLen.setVisible(True)
    tcpipTftpcFilenameLen.setDescription("Maximum Length for a file name")
    tcpipTftpcFilenameLen.setDefaultValue(32)
    
    # Enable TFTP Client Console Commands
    tcpipTftpcCmdEnable = tcpipTftpcComponent.createBooleanSymbol("TCPIP_TFTPC_CONSOLE_CMD", None) 
    tcpipTftpcCmdEnable.setHelp("mcc_h3_tftpc_configurations")
    tcpipTftpcCmdEnable.setLabel("Enable Console Commands")
    tcpipTftpcCmdEnable.setVisible(True)
    tcpipTftpcCmdEnable.setDescription("Enable TFTP Client Console Commands")
    tcpipTftpcCmdEnable.setDefaultValue(False)
            
    # Advanced Settings
    tcpipTftpcAdvSettings = tcpipTftpcComponent.createMenuSymbol("TCPIP_TFTPC_ADV_SETTING", None)
    tcpipTftpcAdvSettings.setLabel("Advanced Settings")
    tcpipTftpcAdvSettings.setDescription("Advanced Settings")
    tcpipTftpcAdvSettings.setVisible(True)

    # TFTP Client Task Rate in msec
    tcpipTftpcTskTickRate= tcpipTftpcComponent.createIntegerSymbol("TCPIP_TFTPC_TASK_TICK_RATE", tcpipTftpcAdvSettings)
    tcpipTftpcTskTickRate.setHelp("mcc_h3_tftpc_configurations")
    tcpipTftpcTskTickRate.setLabel("TFTP Client Task Rate (in msec)")
    tcpipTftpcTskTickRate.setVisible(True)
    tcpipTftpcTskTickRate.setDescription("TFTP Client Task Rate in msec")
    tcpipTftpcTskTickRate.setDefaultValue(100)
    
    # Enable User Notification
    tcpipTftpcUsrNotify = tcpipTftpcComponent.createBooleanSymbol("TCPIP_TFTPC_USER_NOTIFICATION", tcpipTftpcAdvSettings)
    tcpipTftpcUsrNotify.setHelp("mcc_h3_tftpc_configurations")
    tcpipTftpcUsrNotify.setLabel("Enable User Notification")
    tcpipTftpcUsrNotify.setVisible(True)
    tcpipTftpcUsrNotify.setDescription("Enable User Notification")
    tcpipTftpcUsrNotify.setDefaultValue(False) 

    # Maximum Length for Server Address
    tcpipTftpcSrvrAddrLen= tcpipTftpcComponent.createIntegerSymbol("TCPIP_TFTPC_SERVERADDRESS_LEN", tcpipTftpcAdvSettings)
    tcpipTftpcSrvrAddrLen.setHelp("mcc_h3_tftpc_configurations")
    tcpipTftpcSrvrAddrLen.setLabel("Maximum Length for Server Address")
    tcpipTftpcSrvrAddrLen.setVisible(True)
    tcpipTftpcSrvrAddrLen.setDescription("Maximum Length for Server Address")
    tcpipTftpcSrvrAddrLen.setDefaultValue(40)

    # TFTP Client Socket connection timeout in sec
    tcpipTftpcArpTimeout= tcpipTftpcComponent.createIntegerSymbol("TCPIP_TFTPC_ARP_TIMEOUT", tcpipTftpcAdvSettings)
    tcpipTftpcArpTimeout.setHelp("mcc_h3_tftpc_configurations")
    tcpipTftpcArpTimeout.setLabel("TFTP Client Socket Connection Timeout (in sec)")
    tcpipTftpcArpTimeout.setVisible(True)
    tcpipTftpcArpTimeout.setDescription("TFTP Client Socket connection timeout in sec")
    tcpipTftpcArpTimeout.setDefaultValue(3)

    # TFTP Client command process timeout in sec
    tcpipTftpcCmdProcessTimeout= tcpipTftpcComponent.createIntegerSymbol("TCPIP_TFTPC_CMD_PROCESS_TIMEOUT", tcpipTftpcAdvSettings)
    tcpipTftpcCmdProcessTimeout.setHelp("mcc_h3_tftpc_configurations")
    tcpipTftpcCmdProcessTimeout.setLabel("TFTP Client Command Process Timeout (in sec)")
    tcpipTftpcCmdProcessTimeout.setVisible(True)
    tcpipTftpcCmdProcessTimeout.setDescription("TFTP Client command process timeout in sec")
    tcpipTftpcCmdProcessTimeout.setDefaultValue(3)

    # Default Interface
    tcpipTftpcDefault = tcpipTftpcComponent.createStringSymbol("TCPIP_TFTPC_DEFAULT_IF", tcpipTftpcAdvSettings)  
    tcpipTftpcDefault.setHelp("mcc_h3_tftpc_configurations")
    tcpipTftpcDefault.setLabel("Default Interface")
    tcpipTftpcDefault.setVisible(True)
    tcpipTftpcDefault.setDescription("Default Interface")
    # Set Interafce 0 as default interface
    if(Database.getSymbolValue("tcpipNetConfig_0", "TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX0") != None):
        tcpipTftpcDefault.setDefaultValue(Database.getSymbolValue("tcpipNetConfig_0", "TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX0"))    
    tcpipTftpcDefault.setDependencies(tcpipTftpcInterface, ["tcpipNetConfig_0.TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX0"])
    
    tcpipTftpcheapdependency = ["TCPIP_TFTPC_FILENAME_LEN", "tcpipUdp.TCPIP_UDP_SOCKET_DEFAULT_TX_SIZE", 
                                "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"] 
    
    # TFTP Client Heap Size
    tcpipTftpcHeapSize = tcpipTftpcComponent.createIntegerSymbol("TCPIP_TFTPC_HEAP_SIZE", None)
    tcpipTftpcHeapSize.setLabel("TFTP Client Heap Size (bytes)") 
    tcpipTftpcHeapSize.setVisible(False)
    tcpipTftpcHeapSize.setDefaultValue(tcpipTftpcHeapCalc())
    tcpipTftpcHeapSize.setReadOnly(True)
    tcpipTftpcHeapSize.setDependencies(tcpipTftpcHeapUpdate, tcpipTftpcheapdependency)  
    
    #Add to system_config.h
    tcpipTftpcHeaderFtl = tcpipTftpcComponent.createFileSymbol(None, None)
    tcpipTftpcHeaderFtl.setSourcePath("tcpip/config/tftpc.h.ftl")
    tcpipTftpcHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipTftpcHeaderFtl.setMarkup(True)
    tcpipTftpcHeaderFtl.setType("STRING")

    # Add tftpc.c file
    tcpipTftpcSourceFile = tcpipTftpcComponent.createFileSymbol(None, None)
    tcpipTftpcSourceFile.setSourcePath("tcpip/src/tftpc.c")
    tcpipTftpcSourceFile.setOutputName("tftpc.c")
    tcpipTftpcSourceFile.setOverwrite(True)
    tcpipTftpcSourceFile.setDestPath("library/tcpip/src/")
    tcpipTftpcSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipTftpcSourceFile.setType("SOURCE")
    tcpipTftpcSourceFile.setEnabled(True)

def tcpipTftpcHeapCalc():   
    
    min_TFTPC_tx_size = 528 + Database.getSymbolValue("tcpipTftpc","TCPIP_TFTPC_FILENAME_LEN")
    udpsktTxBuffSize = Database.getSymbolValue("tcpipUdp","TCPIP_UDP_SOCKET_DEFAULT_TX_SIZE")
    if(udpsktTxBuffSize >= min_TFTPC_tx_size):
        min_TFTPC_tx_size = 0
        
    heap_size = min_TFTPC_tx_size        
    return heap_size    
    
def tcpipTftpcHeapUpdate(symbol, event): 
    heap_size = tcpipTftpcHeapCalc()
    symbol.setValue(heap_size)
    if(event["id"] == "TCPIP_STACK_HEAP_CALC_MASK"):
        symbol.setVisible(event["value"])
    
# make Reboot Server option visible
def tcpipTftpcModuleVisible(tcpipDependentSymbol, tcpipIPSymbol):
    tcpipIPv4 = Database.getSymbolValue("tcpipIPv4","TCPIP_STACK_USE_IPV4")
    tcpipUdp = Database.getSymbolValue("tcpipUdp","TCPIP_USE_UDP")

    if(tcpipIPv4 and tcpipUdp):
        tcpipDependentSymbol.setVisible(True)
    else:
        tcpipDependentSymbol.setVisible(False)
        
def tcpipTftpcMenuVisibleSingle(symbol, event):
    if (event["value"] == True):
        print("TFTPC Menu Visible.")        
        symbol.setVisible(True)
    else:
        print("TFTPC Menu Invisible.")
        symbol.setVisible(False)


def tcpipTftpcInterface(symbol, event):    
    symbol.setValue(event["value"])


def tcpipTftpcGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])
    
def onAttachmentConnected(source, target):
    if (source["id"] == "Tftpc_MAC_Dependency"):    
        tcpipTftpcIf = source["component"].getSymbolByID("TCPIP_TFTPC_DEFAULT_IF")
        tcpipTftpcIf.clearValue()
        tcpipTftpcIf.setValue(target["component"].getDisplayName(), 2)
       
 
def onAttachmentDisconnected(source, target):
    if (source["id"] == "Tftpc_MAC_Dependency"):    
        source["component"].clearSymbolValue("TCPIP_TFTPC_DEFAULT_IF")

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
    Database.setSymbolValue("tcpipTftpc", "TCPIP_USE_TFTPC_MODULE", False, 2)
