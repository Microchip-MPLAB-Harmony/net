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
    tcpipTelnet.setLabel("Use Telnet Server")
    tcpipTelnet.setVisible(False)
    tcpipTelnet.setDescription("Use Telnet Server")
    tcpipTelnet.setDefaultValue(True) 

    # Max Connections
    tcpipTelnetConnMax = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_MAX_CONNECTIONS", None)
    tcpipTelnetConnMax.setLabel("Maximum Connections")
    tcpipTelnetConnMax.setVisible(True)
    tcpipTelnetConnMax.setDescription("Maximum Connections")
    tcpipTelnetConnMax.setDefaultValue(2)

    # TX Buffer size
    tcpipTelnetTxSize = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_SKT_TX_BUFF_SIZE", None)
    tcpipTelnetTxSize.setLabel("Default Socket TX Buffer Size")
    tcpipTelnetTxSize.setVisible(True)
    tcpipTelnetTxSize.setDescription("Socket TX Buffer Size")
    tcpipTelnetTxSize.setDefaultValue(0)

    # RX Buffer size
    tcpipTelnetRxSize = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_SKT_RX_BUFF_SIZE", None)
    tcpipTelnetRxSize.setLabel("Default Socket RX Buffer Size")
    tcpipTelnetRxSize.setVisible(True)
    tcpipTelnetRxSize.setDescription("Socket RX Buffer Size")
    tcpipTelnetRxSize.setDefaultValue(0)

    # Listening port
    tcpipTelnetListenPort = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_LISTEN_PORT", None)
    tcpipTelnetListenPort.setLabel("Telnet Server Listening Port")
    tcpipTelnetListenPort.setVisible(True)
    tcpipTelnetListenPort.setDescription("Telnet Server Listening Port")
    tcpipTelnetListenPort.setDefaultValue(23)

    # telnet Configuration Flags Settings
    tcpipTelnetConfigFlag = tcpipTelnetComponent.createMenuSymbol(None, None)
    tcpipTelnetConfigFlag.setLabel("Configuration Flags")
    tcpipTelnetConfigFlag.setVisible(True)
    tcpipTelnetConfigFlag.setDescription("telnet Configuration Flags Settings")

    # telnet sockets created with NO-DELAY option
    tcpipTelnetConfigFlagNoDly = tcpipTelnetComponent.createBooleanSymbol("TCPIP_TELNET_CONFIG_FLAG_NO_DELAY", tcpipTelnetConfigFlag)
    tcpipTelnetConfigFlagNoDly.setLabel("Create Telnet Sockets with NO-DELAY option")
    tcpipTelnetConfigFlagNoDly.setVisible(True)
    tcpipTelnetConfigFlagNoDly.setDescription("telnet sockets created with NO-DELAY option")
    tcpipTelnetConfigFlagNoDly.setDefaultValue(False)

    # Pass control characters
    tcpipTelnetConfigPassControl = tcpipTelnetComponent.createBooleanSymbol("TCPIP_TELNET_CONFIG_PASS_CONTROL_CHARS", tcpipTelnetConfigFlag)
    tcpipTelnetConfigPassControl.setLabel("Pass Control Characters to Console")
    tcpipTelnetConfigPassControl.setVisible(True)
    tcpipTelnetConfigPassControl.setDescription("Do not Process Control Characters but Pass them to the Console")
    tcpipTelnetConfigPassControl.setDefaultValue(False)
    
    # Advanced Settings
    tcpipTelnetAdvSettings = tcpipTelnetComponent.createMenuSymbol("TCPIP_TELNET_ADV_SETTING", None)
    tcpipTelnetAdvSettings.setLabel("Advanced Settings")
    tcpipTelnetAdvSettings.setDescription("Advanced Settings")
    tcpipTelnetAdvSettings.setVisible(True)

    # Task Rate in ms
    tcpipTelnetTskTickRate = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_TASK_TICK_RATE", tcpipTelnetAdvSettings)
    tcpipTelnetTskTickRate.setLabel("Telnet Task Rate (in msec)")
    tcpipTelnetTskTickRate.setVisible(True)
    tcpipTelnetTskTickRate.setDescription("Task Rate in ms")
    tcpipTelnetTskTickRate.setDefaultValue(100)

    # Print buffer size
    tcpipTelnetPrintBuffSize = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_PRINT_BUFF_SIZE", tcpipTelnetAdvSettings)
    tcpipTelnetPrintBuffSize.setLabel("Size of the Internal Print Buffer")
    tcpipTelnetPrintBuffSize.setVisible(True)
    tcpipTelnetPrintBuffSize.setDescription("Size of the Internal Print Buffer")
    tcpipTelnetPrintBuffSize.setDefaultValue(200)

    # Line buffer size
    tcpipTelnetLineBuffSize = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_LINE_BUFF_SIZE", tcpipTelnetAdvSettings)
    tcpipTelnetLineBuffSize.setLabel("Size of the Internal Line Buffer")
    tcpipTelnetLineBuffSize.setVisible(True)
    tcpipTelnetLineBuffSize.setDescription("Size of the Internal Line Buffer")
    tcpipTelnetLineBuffSize.setDefaultValue(80)

    # User name buffer size
    tcpipTelnetUserBuffSize = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_USERNAME_SIZE", tcpipTelnetAdvSettings)
    tcpipTelnetUserBuffSize.setLabel("Size of the Internal User Name Buffer")
    tcpipTelnetUserBuffSize.setVisible(True)
    tcpipTelnetUserBuffSize.setDescription("Size of the Internal User Name Buffer")
    tcpipTelnetUserBuffSize.setDefaultValue(15)

    # telnet Authentication Selection
    tcpipTelnetAuth= tcpipTelnetComponent.createComboSymbol("TCPIP_TELNET_AUTH_CONFIG", tcpipTelnetAdvSettings, TCPIP_TELNET_AUTH_TYPES)
    tcpipTelnetAuth.setLabel("Select telnet Authentication")
    tcpipTelnetAuth.setVisible(True)
    tcpipTelnetAuth.setDescription("telnet Authentication Selection")
    tcpipTelnetAuth.setDefaultValue("Run Time Authentication")

    # telnet Connection info Authentication
    tcpipTelnetAuthInfo = tcpipTelnetComponent.createBooleanSymbol("TCPIP_TELNET_AUTHENTICATION_CONN_INFO", tcpipTelnetAuth)
    tcpipTelnetAuthInfo.setLabel("Connection Info Used for Authentication ")
    tcpipTelnetAuthInfo.setVisible(True)
    tcpipTelnetAuthInfo.setDescription("Connection Info is Passed to the Authentication Handler")
    tcpipTelnetAuthInfo.setDefaultValue(True)
    tcpipTelnetAuthInfo.setDependencies(tcpipTelnetAuthInfoVisible, ["TCPIP_TELNET_AUTH_CONFIG"])

    # Telnet User Name
    tcpipTelnetUsrName = tcpipTelnetComponent.createStringSymbol("TCPIP_TELNET_USERNAME", tcpipTelnetAuth)
    tcpipTelnetUsrName.setLabel("User Name")
    tcpipTelnetUsrName.setVisible(False)
    tcpipTelnetUsrName.setDescription("Telnet User Name")
    tcpipTelnetUsrName.setDefaultValue("admin")
    tcpipTelnetUsrName.setDependencies(tcpipTelnetObsAuthVisible, ["TCPIP_TELNET_AUTH_CONFIG"])

    # Telnet Password
    tcpipTelnetPswd = tcpipTelnetComponent.createStringSymbol("TCPIP_TELNET_PASSWORD", tcpipTelnetAuth)
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
