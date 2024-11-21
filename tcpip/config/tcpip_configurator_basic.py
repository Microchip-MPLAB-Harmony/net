"""
Copyright (C) 2018-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

################################################################################
#### Dependencies ####
################################################################################
autoConnectTableConsole = [["TCP/IP STACK", "BASIC CONFIGURATION:tcpipStack:Core_SysConsole_Dependency", "sys_console_0", "sys_console"]]
autoConnectTableDebug = [["sys_debug", "sys_debug_SYS_CONSOLE_dependency", "sys_console_0", "sys_console"]]
autoConnectTableCmd = [["sys_command", "sys_command_SYS_CONSOLE_dependency", "sys_console_0", "sys_console"]]
############################################################################
#### Code Generation ####
############################################################################
def instantiateComponent(tcpipAutoConfigBasicComponent):

    tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
    if (tcpipAutoConfigStackGroup == None):
        tcpipAutoConfigStackGroup = Database.createGroup(None, "TCP/IP STACK")
        
    tcpipAutoConfigBasicGroup = Database.findGroup("BASIC CONFIGURATION")
    if (tcpipAutoConfigBasicGroup == None):
        tcpipAutoConfigBasicGroup = Database.createGroup("TCP/IP STACK", "BASIC CONFIGURATION")     
    
    # Enable Transport Configurator
    tcpipAutoConfigBasicEnable = tcpipAutoConfigBasicComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_BASIC_ENABLE", None)
    tcpipAutoConfigBasicEnable.setVisible(False)
    tcpipAutoConfigBasicEnable.setDefaultValue(True)
    
    # Enable TCP/IP STACK
    tcpipAutoConfigStack = tcpipAutoConfigBasicComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_STACK", None)
    tcpipAutoConfigStack.setLabel("Basic STACK Configuration")
    tcpipAutoConfigStack.setVisible(True)
    tcpipAutoConfigStack.setDescription("Enable STACK Configuration")   
    tcpipAutoConfigStack.setDependencies(tcpipAutoConfigStackEnable, ["TCPIP_AUTOCONFIG_ENABLE_STACK"])
    
    # Enable Netconfig
    tcpipAutoConfigNetConfig = tcpipAutoConfigBasicComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_NETCONFIG", None)
    tcpipAutoConfigNetConfig.setLabel("Network Configuration")
    tcpipAutoConfigNetConfig.setVisible(True)
    tcpipAutoConfigNetConfig.setDescription("Enable Network Configuration") 
    tcpipAutoConfigNetConfig.setDependencies(tcpipAutoConfigNetConfigEnable, ["TCPIP_AUTOCONFIG_ENABLE_NETCONFIG"])  
    
    # Enable TCPIP CMD
    tcpipAutoConfigTcpipCmd = tcpipAutoConfigBasicComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_TCPIPCMD", None)
    tcpipAutoConfigTcpipCmd.setLabel("TCPIP CMD")
    tcpipAutoConfigTcpipCmd.setVisible(True)
    tcpipAutoConfigTcpipCmd.setDescription("Enable TCPIP CMD")  
    tcpipAutoConfigTcpipCmd.setDependencies(tcpipAutoConfigTcpipCmdEnable, ["TCPIP_AUTOCONFIG_ENABLE_TCPIPCMD"])
    
########################################################################################################
def finalizeComponent(tcpipAutoConfigBasicComponent):
    tcpipAutoConfigBasicGroup = Database.findGroup("BASIC CONFIGURATION")
    tcpipAutoConfigBasicGroup.addComponent(tcpipAutoConfigBasicComponent.getID())   
    
    if(Database.getSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_APPS_ENABLE") != True) and (Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_TRANS_ENABLE") != True) and (Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_NET_ENABLE") != True) and (Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_DRV_ENABLE") != True):
        Database.setActiveGroup("BASIC CONFIGURATION")
#######################################################################################################
def enableTcpipAutoConfigBasic(enable):

    if(enable == True):
        tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
        if (tcpipAutoConfigAppsGroup == None):
            tcpipAutoConfigAppsGroup = Database.createGroup("TCP/IP STACK", "APPLICATION LAYER")
            
        tcpipAutoConfigTransportGroup = Database.findGroup("TRANSPORT LAYER")
        if (tcpipAutoConfigTransportGroup == None):
            tcpipAutoConfigTransportGroup = Database.createGroup("TCP/IP STACK", "TRANSPORT LAYER")

        tcpipAutoConfigNetworkGroup = Database.findGroup("NETWORK LAYER")
        if (tcpipAutoConfigNetworkGroup == None):
            tcpipAutoConfigNetworkGroup = Database.createGroup("TCP/IP STACK", "NETWORK LAYER")
            
        tcpipAutoConfigDriverGroup = Database.findGroup("DRIVER LAYER")
        if (tcpipAutoConfigDriverGroup == None):
            tcpipAutoConfigDriverGroup = Database.createGroup("TCP/IP STACK", "DRIVER LAYER")

        tcpipAutoConfigBasicGroup = Database.findGroup("BASIC CONFIGURATION")
        if (tcpipAutoConfigBasicGroup == None):
            tcpipAutoConfigBasicGroup = Database.createGroup("TCP/IP STACK", "BASIC CONFIGURATION")
            
        if(Database.getComponentByID("tcpip_apps_config") == None):
            res = tcpipAutoConfigAppsGroup.addComponent("tcpip_apps_config")
            res = Database.activateComponents(["tcpip_apps_config"], "APPLICATION LAYER", False)
            
        if(Database.getComponentByID("tcpip_transport_config") == None):
            res = tcpipAutoConfigTransportGroup.addComponent("tcpip_transport_config")
            res = Database.activateComponents(["tcpip_transport_config"], "TRANSPORT LAYER", False)
            
        if(Database.getComponentByID("tcpip_network_config") == None):
            res = tcpipAutoConfigNetworkGroup.addComponent("tcpip_network_config")
            res = Database.activateComponents(["tcpip_network_config"], "NETWORK LAYER", False)
            
        if(Database.getComponentByID("tcpip_driver_config") == None):
            res = tcpipAutoConfigDriverGroup.addComponent("tcpip_driver_config")
            res = Database.activateComponents(["tcpip_driver_config"], "DRIVER LAYER", False)   

#################Business Logic- Transport Layer #########################################  
def tcpipAutoConfigStackEnable(symbol, event):
    tcpipAutoConfigBasicGroup = Database.findGroup("BASIC CONFIGURATION")
    tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
    enableTcpipAutoConfigBasic(True)
    if (event["value"] == True):    
        res = Database.activateComponents(["tcpipStack"], "BASIC CONFIGURATION", False) 
        tcpipAutoConfigBasicGroup.setAttachmentVisible("tcpipStack", "libtcpipStack")
        
        if(Database.getComponentByID("sys_time") == None):
            res = Database.activateComponents(["sys_time"]) 
            
        if(Database.getComponentByID("sys_console") == None):
            tcpipAutoConfigBasicGroup.setAttachmentVisible("tcpipStack", "Core_SysConsole_Dependency")
            tcpipAutoConfigStackGroup.setAttachmentVisible("BASIC CONFIGURATION", "tcpipStack:Core_SysConsole_Dependency")
            res = Database.activateComponents(["sys_console"])  
            res = Database.connectDependencies(autoConnectTableConsole)
        if(Database.getComponentByID("sys_debug") == None):
            res = Database.activateComponents(["sys_debug"])
            res = Database.connectDependencies(autoConnectTableDebug)           
        
    else:
        res = Database.deactivateComponents(["tcpipStack"])
    
def tcpipAutoConfigNetConfigEnable(symbol, event):
    tcpipAutoConfigBasicGroup = Database.findGroup("BASIC CONFIGURATION")
    enableTcpipAutoConfigBasic(True)
    if(Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_STACK") != True):
        Database.setSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_STACK", True, 2)
        
    if (event["value"] == True):
        if (Database.getComponentByID("tcpipNetConfig") == None):
            res = Database.activateComponents(["tcpipNetConfig"], "BASIC CONFIGURATION", False)
            tcpipAutoConfigBasicGroup.setAttachmentVisible("tcpipNetConfig_0", "NETCONFIG_MAC_Dependency")
            tcpipAutoConfigBasicGroup.setAttachmentVisible("tcpipNetConfig", "libtcpipNetConfig")   
    else:
        res = Database.deactivateComponents(["tcpipNetConfig"])
    
        
def tcpipAutoConfigTcpipCmdEnable(symbol, event):
    tcpipAutoConfigBasicGroup = Database.findGroup("BASIC CONFIGURATION")
    tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
    enableTcpipAutoConfigBasic(True)
    if(Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_STACK") != True):
        Database.setSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_STACK", True, 2)
    if(Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_NETCONFIG") != True):
        Database.setSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_NETCONFIG", True, 2)
            
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipCmd"], "BASIC CONFIGURATION", False)
        tcpipAutoConfigBasicGroup.setAttachmentVisible("tcpipCmd", "libtcpipCmd")

    else:
        res = Database.deactivateComponents(["tcpipCmd"])
        
    # Enable command processor in SYS_COMMAND
    if(event["value"] == True) and (Database.getComponentByID("sys_command") == None):
        res = Database.activateComponents(["sys_command"])
        res = Database.connectDependencies(autoConnectTableCmd)
        series = ATDF.getNode("/avr-tools-device-file/devices/device").getAttribute("series")
        if(series == "SAME54") or (series == "SAME70") or (series == "SAMV71") or ("PIC32CZCA7" in series) or (series == "SAMA5D2"):
            #Todo: change to Database.sendMessage(); but need handleMessage() in sys_command
            Database.setSymbolValue("sys_command", "SYS_COMMAND_PRINT_BUFFER_SIZE", 2560)
        

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
    
