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

TCPIP_TELNET_AUTH_TYPES = ["Run Time Authentication", "(OBSOLETE) Build Time Authentication"]

def tcpipTelnetAuthInfoVisible(symbol, event):
    if (event["value"] == "Run Time Authentication"):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def tcpipTelnetObsAuthVisible(symbol, event):
    if (event["value"] == "Run Time Authentication"):
        symbol.setVisible(False)
    else:
        symbol.setVisible(True)

    
def instantiateComponent(tcpipTelnetComponent):
    print("TCPIP Telnet Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    # Use Telnet Server
    tcpipTelnet = tcpipTelnetComponent.createBooleanSymbol("TCPIP_USE_TELNET", None)
    tcpipTelnet.setHelp("mcc_h3_telnet_configurations")
    tcpipTelnet.setLabel("Use Telnet Server")
    tcpipTelnet.setVisible(False)
    tcpipTelnet.setDescription("Use Telnet Server")
    tcpipTelnet.setDefaultValue(True) 

    # Max Connections
    tcpipTelnetConnMax = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_MAX_CONNECTIONS", None)
    tcpipTelnetConnMax.setHelp("mcc_h3_telnet_configurations")
    tcpipTelnetConnMax.setLabel("Maximum Connections")
    tcpipTelnetConnMax.setVisible(True)
    tcpipTelnetConnMax.setDescription("Maximum Connections")
    tcpipTelnetConnMax.setDefaultValue(2)

    # TX Buffer size
    tcpipTelnetTxSize = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_SKT_TX_BUFF_SIZE", None)
    tcpipTelnetTxSize.setHelp("mcc_h3_telnet_configurations")
    tcpipTelnetTxSize.setLabel("Default Socket TX Buffer Size")
    tcpipTelnetTxSize.setVisible(True)
    tcpipTelnetTxSize.setDescription("Socket TX Buffer Size")
    tcpipTelnetTxSize.setDefaultValue(0)

    # RX Buffer size
    tcpipTelnetRxSize = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_SKT_RX_BUFF_SIZE", None)
    tcpipTelnetRxSize.setHelp("mcc_h3_telnet_configurations")
    tcpipTelnetRxSize.setLabel("Default Socket RX Buffer Size")
    tcpipTelnetRxSize.setVisible(True)
    tcpipTelnetRxSize.setDescription("Socket RX Buffer Size")
    tcpipTelnetRxSize.setDefaultValue(0)

    # Listening port
    tcpipTelnetListenPort = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_LISTEN_PORT", None)
    tcpipTelnetListenPort.setHelp("mcc_h3_telnet_configurations")
    tcpipTelnetListenPort.setLabel("Telnet Server Listening Port")
    tcpipTelnetListenPort.setVisible(True)
    tcpipTelnetListenPort.setDescription("Telnet Server Listening Port")
    tcpipTelnetListenPort.setDefaultValue(23)

    # Advanced Settings
    tcpipTelnetAdvSettings = tcpipTelnetComponent.createMenuSymbol("TCPIP_TELNET_ADV_SETTING", None)
    tcpipTelnetAdvSettings.setLabel("Advanced Settings")
    tcpipTelnetAdvSettings.setDescription("Advanced Settings")
    tcpipTelnetAdvSettings.setVisible(True)

    # Task Rate in ms
    tcpipTelnetTskTickRate = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_TASK_TICK_RATE", tcpipTelnetAdvSettings)
    tcpipTelnetTskTickRate.setHelp("mcc_h3_telnet_configurations")
    tcpipTelnetTskTickRate.setLabel("Telnet Task Rate (in msec)")
    tcpipTelnetTskTickRate.setVisible(True)
    tcpipTelnetTskTickRate.setDescription("Task Rate in ms")
    tcpipTelnetTskTickRate.setDefaultValue(100)

    # Print buffer size
    tcpipTelnetPrintBuffSize = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_PRINT_BUFF_SIZE", tcpipTelnetAdvSettings)
    tcpipTelnetPrintBuffSize.setHelp("mcc_h3_telnet_configurations")
    tcpipTelnetPrintBuffSize.setLabel("Size of the Internal Print Buffer")
    tcpipTelnetPrintBuffSize.setVisible(True)
    tcpipTelnetPrintBuffSize.setDescription("Size of the Internal Print Buffer")
    tcpipTelnetPrintBuffSize.setDefaultValue(200)

    # Line buffer size
    tcpipTelnetLineBuffSize = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_LINE_BUFF_SIZE", tcpipTelnetAdvSettings)
    tcpipTelnetLineBuffSize.setHelp("mcc_h3_telnet_configurations")
    tcpipTelnetLineBuffSize.setLabel("Size of the Internal Line Buffer")
    tcpipTelnetLineBuffSize.setVisible(True)
    tcpipTelnetLineBuffSize.setDescription("Size of the Internal Line Buffer")
    tcpipTelnetLineBuffSize.setDefaultValue(80)

    # User name buffer size
    tcpipTelnetUserBuffSize = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_USERNAME_SIZE", tcpipTelnetAdvSettings)
    tcpipTelnetUserBuffSize.setHelp("mcc_h3_telnet_configurations")
    tcpipTelnetUserBuffSize.setLabel("Size of the Internal User Name Buffer")
    tcpipTelnetUserBuffSize.setVisible(True)
    tcpipTelnetUserBuffSize.setDescription("Size of the Internal User Name Buffer")
    tcpipTelnetUserBuffSize.setDefaultValue(15)

    # telnet Configuration Flags Settings
    tcpipTelnetConfigFlag = tcpipTelnetComponent.createMenuSymbol(None, tcpipTelnetAdvSettings)
    tcpipTelnetConfigFlag.setLabel("Configuration Flags")
    tcpipTelnetConfigFlag.setVisible(True)
    tcpipTelnetConfigFlag.setDescription("telnet Configuration Flags Settings")

    # telnet sockets created with NO-DELAY option
    tcpipTelnetConfigFlagNoDly = tcpipTelnetComponent.createBooleanSymbol("TCPIP_TELNET_CONFIG_FLAG_NO_DELAY", tcpipTelnetConfigFlag)
    tcpipTelnetConfigFlagNoDly.setHelp("mcc_h3_telnet_configurations")
    tcpipTelnetConfigFlagNoDly.setLabel("Create Telnet Sockets with NO-DELAY option")
    tcpipTelnetConfigFlagNoDly.setVisible(True)
    tcpipTelnetConfigFlagNoDly.setDescription("telnet sockets created with NO-DELAY option")
    tcpipTelnetConfigFlagNoDly.setDefaultValue(False)

    # Pass control characters
    tcpipTelnetConfigPassControl = tcpipTelnetComponent.createBooleanSymbol("TCPIP_TELNET_CONFIG_PASS_CONTROL_CHARS", tcpipTelnetConfigFlag)
    tcpipTelnetConfigPassControl.setHelp("mcc_h3_telnet_configurations")
    tcpipTelnetConfigPassControl.setLabel("Pass Control Characters to Console")
    tcpipTelnetConfigPassControl.setVisible(True)
    tcpipTelnetConfigPassControl.setDescription("Do not Process Control Characters but Pass them to the Console")
    tcpipTelnetConfigPassControl.setDefaultValue(False)
    
    # telnet Authentication Selection
    tcpipTelnetAuth= tcpipTelnetComponent.createComboSymbol("TCPIP_TELNET_AUTH_CONFIG", tcpipTelnetAdvSettings, TCPIP_TELNET_AUTH_TYPES)
    tcpipTelnetAuth.setHelp("mcc_h3_telnet_configurations")
    tcpipTelnetAuth.setLabel("Select telnet Authentication")
    tcpipTelnetAuth.setVisible(True)
    tcpipTelnetAuth.setDescription("telnet Authentication Selection")
    tcpipTelnetAuth.setDefaultValue("Run Time Authentication")

    # telnet Connection info Authentication
    tcpipTelnetAuthInfo = tcpipTelnetComponent.createBooleanSymbol("TCPIP_TELNET_AUTHENTICATION_CONN_INFO", tcpipTelnetAuth)
    tcpipTelnetAuthInfo.setHelp("mcc_h3_telnet_configurations")
    tcpipTelnetAuthInfo.setLabel("Connection Info Used for Authentication ")
    tcpipTelnetAuthInfo.setVisible(True)
    tcpipTelnetAuthInfo.setDescription("Connection Info is Passed to the Authentication Handler")
    tcpipTelnetAuthInfo.setDefaultValue(True)
    tcpipTelnetAuthInfo.setDependencies(tcpipTelnetAuthInfoVisible, ["TCPIP_TELNET_AUTH_CONFIG"])

    # Telnet User Name
    tcpipTelnetUsrName = tcpipTelnetComponent.createStringSymbol("TCPIP_TELNET_USERNAME", tcpipTelnetAuth)
    tcpipTelnetUsrName.setHelp("mcc_h3_telnet_configurations")
    tcpipTelnetUsrName.setLabel("User Name")
    tcpipTelnetUsrName.setVisible(False)
    tcpipTelnetUsrName.setDescription("Telnet User Name")
    tcpipTelnetUsrName.setDefaultValue("admin")
    tcpipTelnetUsrName.setDependencies(tcpipTelnetObsAuthVisible, ["TCPIP_TELNET_AUTH_CONFIG"])

    # Telnet Password
    tcpipTelnetPswd = tcpipTelnetComponent.createStringSymbol("TCPIP_TELNET_PASSWORD", tcpipTelnetAuth)
    tcpipTelnetPswd.setHelp("mcc_h3_telnet_configurations")
    tcpipTelnetPswd.setLabel("Password")
    tcpipTelnetPswd.setVisible(False)
    tcpipTelnetPswd.setDescription("Telnet Password")
    tcpipTelnetPswd.setDefaultValue("microchip")
    tcpipTelnetPswd.setDependencies(tcpipTelnetObsAuthVisible, ["TCPIP_TELNET_AUTH_CONFIG"])
    
    tcpipTelnetheapdependency = ["TCPIP_TELNET_MAX_CONNECTIONS", "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]    
        
    # Telnet Heap Size
    tcpipTelnetHeapSize = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_HEAP_SIZE", None)
    tcpipTelnetHeapSize.setLabel("Telnet Heap Size (bytes)") 
    tcpipTelnetHeapSize.setVisible(False)
    tcpipTelnetHeapSize.setDefaultValue(tcpipTelnetHeapCalc())
    tcpipTelnetHeapSize.setReadOnly(True)
    tcpipTelnetHeapSize.setDependencies(tcpipTelnetHeapUpdate, tcpipTelnetheapdependency)   
    
    #Add to system_config.h
    tcpipTelnetHeaderFtl = tcpipTelnetComponent.createFileSymbol(None, None)
    tcpipTelnetHeaderFtl.setSourcePath("tcpip/config/telnet.h.ftl")
    tcpipTelnetHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipTelnetHeaderFtl.setMarkup(True)
    tcpipTelnetHeaderFtl.setType("STRING")

    # Add telnet.c file
    tcpipTelnetSourceFile = tcpipTelnetComponent.createFileSymbol(None, None)
    tcpipTelnetSourceFile.setSourcePath("tcpip/src/telnet.c")
    tcpipTelnetSourceFile.setOutputName("telnet.c")
    tcpipTelnetSourceFile.setOverwrite(True)
    tcpipTelnetSourceFile.setDestPath("library/tcpip/src/")
    tcpipTelnetSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipTelnetSourceFile.setType("SOURCE")
    tcpipTelnetSourceFile.setEnabled(True)
    
def tcpipTelnetMenuVisible(symbol, event):
    if (event["value"] == True):
        print("Telnet Menu Visible.")       
        symbol.setVisible(True)
    else:
        print("Telnet Menu Invisible.")
        symbol.setVisible(False)
        
def tcpipTelnetGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])

def tcpipTelnetHeapCalc(): 
    maxNumConn = Database.getSymbolValue("tcpipTelnet","TCPIP_TELNET_MAX_CONNECTIONS")
    heap_size = maxNumConn * 28
    return heap_size    
    
def tcpipTelnetHeapUpdate(symbol, event): 
    heap_size = tcpipTelnetHeapCalc()
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
    Database.setSymbolValue("tcpipTelnet", "TCPIP_USE_TELNET", False, 2)
