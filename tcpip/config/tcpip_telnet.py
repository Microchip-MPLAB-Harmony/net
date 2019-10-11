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
    #tcpipTelnetConnMax.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

    # TX Buffer size
    tcpipTelnetTxSize = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_SKT_TX_BUFF_SIZE", None)
    tcpipTelnetTxSize.setLabel("Socket TX Buffer Size")
    tcpipTelnetTxSize.setVisible(True)
    tcpipTelnetTxSize.setDescription("Socket TX Buffer Size")
    tcpipTelnetTxSize.setDefaultValue(0)
    #tcpipTelnetTxSize.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

    # RX Buffer size
    tcpipTelnetRxSize = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_SKT_RX_BUFF_SIZE", None)
    tcpipTelnetRxSize.setLabel("Socket RX Buffer Size")
    tcpipTelnetRxSize.setVisible(True)
    tcpipTelnetRxSize.setDescription("Socket RX Buffer Size")
    tcpipTelnetRxSize.setDefaultValue(0)
    #tcpipTelnetRxSize.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

    # Listening port
    tcpipTelnetListenPort = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_LISTEN_PORT", None)
    tcpipTelnetListenPort.setLabel("Listening Port")
    tcpipTelnetListenPort.setVisible(True)
    tcpipTelnetListenPort.setDescription("Telnet Server Listening Port")
    tcpipTelnetListenPort.setDefaultValue(23)
    #tcpipTelnetListenPort.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

    # Print buffer size
    tcpipTelnetPrintBuffSize = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_PRINT_BUFF_SIZE", None)
    tcpipTelnetPrintBuffSize.setLabel("Size of the Internal Print Buffer")
    tcpipTelnetPrintBuffSize.setVisible(True)
    tcpipTelnetPrintBuffSize.setDescription("Size of the Internal Print Buffer")
    tcpipTelnetPrintBuffSize.setDefaultValue(200)
    #tcpipTelnetPrintBuffSize.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

    # Line buffer size
    tcpipTelnetLineBuffSize = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_LINE_BUFF_SIZE", None)
    tcpipTelnetLineBuffSize.setLabel("Size of the Internal Line Buffer")
    tcpipTelnetLineBuffSize.setVisible(True)
    tcpipTelnetLineBuffSize.setDescription("Size of the Internal Line Buffer")
    tcpipTelnetLineBuffSize.setDefaultValue(80)
    #tcpipTelnetLineBuffSize.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

    # User name buffer size
    tcpipTelnetUserBuffSize = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_USERNAME_SIZE", None)
    tcpipTelnetUserBuffSize.setLabel("Size of the Internal User Name Buffer")
    tcpipTelnetUserBuffSize.setVisible(True)
    tcpipTelnetUserBuffSize.setDescription("Size of the Internal User Name Buffer")
    tcpipTelnetUserBuffSize.setDefaultValue(15)
    #tcpipTelnetUserBuffSize.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

    # telnet Configuration Flags Settings
    tcpipTelnetConfigFlag = tcpipTelnetComponent.createMenuSymbol(None, None)
    tcpipTelnetConfigFlag.setLabel("Configuration Flags")
    tcpipTelnetConfigFlag.setVisible(True)
    tcpipTelnetConfigFlag.setDescription("telnet Configuration Flags Settings")
    #tcpipTelnetConfigFlag.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

    # telnet sockets created with NO-DELAY option
    tcpipTelnetConfigFlagNoDly = tcpipTelnetComponent.createBooleanSymbol("TCPIP_TELNET_CONFIG_FLAG_NO_DELAY", tcpipTelnetConfigFlag)
    tcpipTelnetConfigFlagNoDly.setLabel("telnet sockets created with NO-DELAY option")
    tcpipTelnetConfigFlagNoDly.setVisible(True)
    tcpipTelnetConfigFlagNoDly.setDescription("telnet sockets created with NO-DELAY option")
    tcpipTelnetConfigFlagNoDly.setDefaultValue(False)
    #tcpipTelnetConfigFlagNoDly.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

    # Pass control characters
    tcpipTelnetConfigPassControl = tcpipTelnetComponent.createBooleanSymbol("TCPIP_TELNET_CONFIG_PASS_CONTROL_CHARS", tcpipTelnetConfigFlag)
    tcpipTelnetConfigPassControl.setLabel("Pass Control Characters to Console")
    tcpipTelnetConfigPassControl.setVisible(True)
    tcpipTelnetConfigPassControl.setDescription("Do not Process Control Characters but Pass them to the Console")
    tcpipTelnetConfigPassControl.setDefaultValue(False)
    #tcpipTelnetConfigPassControl.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

    # telnet Authentication Menu
    tcpipTelnetAuthMenu = tcpipTelnetComponent.createMenuSymbol(None, None)
    tcpipTelnetAuthMenu.setLabel("telnet Authentication")
    tcpipTelnetAuthMenu.setVisible(True)
    tcpipTelnetAuthMenu.setDescription("telnet Authentication Settings")
    #tcpipTelnetAuthMenu.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

    # telnet Run Menu
    tcpipTelnetRunMenu = tcpipTelnetComponent.createMenuSymbol("TCPIP_TELNET_RUN_MENU", tcpipTelnetAuthMenu)
    tcpipTelnetRunMenu.setLabel("Run Time User Authentication")
    tcpipTelnetRunMenu.setVisible(True)
    tcpipTelnetRunMenu.setDescription("telnet Calls Authentication Handler at Run Time")
    #tcpipTelnetRunMenu.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

    # telnet Run time Authentication
    tcpipTelnetRunAuth = tcpipTelnetComponent.createBooleanSymbol("TCPIP_TELNET_RUN_AUTHENTICATION", tcpipTelnetRunMenu)
    tcpipTelnetRunAuth.setLabel("Enable Run Time Authentication")
    tcpipTelnetRunAuth.setVisible(True)
    tcpipTelnetRunAuth.setDescription("Enable Run Time User Authentication")
    tcpipTelnetRunAuth.setDefaultValue(True)
    #tcpipTelnetRunAuth.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

    # telnet Connection info Authentication
    tcpipTelnetAuthInfo = tcpipTelnetComponent.createBooleanSymbol("TCPIP_TELNET_AUTHENTICATION_CONN_INFO", tcpipTelnetRunMenu)
    tcpipTelnetAuthInfo.setLabel("Authentication Connection Info")
    tcpipTelnetAuthInfo.setVisible(True)
    tcpipTelnetAuthInfo.setDescription("Connection Info passed to the Authentication Handler")
    tcpipTelnetAuthInfo.setDefaultValue(True)
    #tcpipTelnetAuthInfo.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

    # telnet Obsolete Menu
    tcpipTelnetObsMenu = tcpipTelnetComponent.createMenuSymbol("TCPIP_TELNET_OBS_MENU", tcpipTelnetAuthMenu)
    tcpipTelnetObsMenu.setLabel("Obsolete Build Time Authentication")
    tcpipTelnetObsMenu.setVisible(True)
    tcpipTelnetObsMenu.setDescription("OBSOLETE Build Time Authentication")
    #tcpipTelnetObsMenu.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

    # telnet Obsolete Authentication
    tcpipTelnetObsAuth = tcpipTelnetComponent.createBooleanSymbol("TCPIP_TELNET_OBS_AUTHENTICATION", tcpipTelnetObsMenu)
    tcpipTelnetObsAuth.setLabel("Enable Obsolete Build Time Authentication")
    tcpipTelnetObsAuth.setVisible(True)
    tcpipTelnetObsAuth.setDescription("Enable OBSOLETE Build Time Authentication")
    tcpipTelnetObsAuth.setDefaultValue(False)
    #tcpipTelnetObsAuth.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

    # Telnet User Name
    tcpipTelnetUsrName = tcpipTelnetComponent.createStringSymbol("TCPIP_TELNET_USERNAME", tcpipTelnetObsMenu)
    tcpipTelnetUsrName.setLabel("User Name")
    tcpipTelnetUsrName.setVisible(True)
    tcpipTelnetUsrName.setDescription("Telnet User Name")
    tcpipTelnetUsrName.setDefaultValue("admin")
    #tcpipTelnetUsrName.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

    # Telnet Password
    tcpipTelnetPswd = tcpipTelnetComponent.createStringSymbol("TCPIP_TELNET_PASSWORD", tcpipTelnetObsMenu)
    tcpipTelnetPswd.setLabel("Password")
    tcpipTelnetPswd.setVisible(True)
    tcpipTelnetPswd.setDescription("Telnet Password")
    tcpipTelnetPswd.setDefaultValue("microchip")
    #tcpipTelnetPswd.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

    # Task Rate in ms
    tcpipTelnetTskTickRate = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_TASK_TICK_RATE", None)
    tcpipTelnetTskTickRate.setLabel("Task Rate - ms")
    tcpipTelnetTskTickRate.setVisible(True)
    tcpipTelnetTskTickRate.setDescription("Task Rate in ms")
    tcpipTelnetTskTickRate.setDefaultValue(100)
    #tcpipTelnetTskTickRate.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

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
    #tcpipTelnetSourceFile.setDependencies(tcpipTelnetGenSourceFile, ["TCPIP_USE_TELNET"])
    
def tcpipTelnetMenuVisible(symbol, event):
    if (event["value"] == True):
        print("Telnet Menu Visible.")       
        symbol.setVisible(True)
    else:
        print("Telnet Menu Invisible.")
        symbol.setVisible(False)
        
def tcpipTelnetGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])

def destroyComponent(component):
    Database.setSymbolValue("tcpipTelnet", "TCPIP_USE_TELNET", False, 2)
