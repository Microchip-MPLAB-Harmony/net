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
#### Business Logic ####
################################################################################

############################################################################
#### Code Generation ####
############################################################################
def instantiateComponent(tcpipAutoConfigTransportComponent):

    tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
    if (tcpipAutoConfigStackGroup == None):
        tcpipAutoConfigStackGroup = Database.createGroup(None, "TCP/IP STACK")
        
    tcpipAutoConfigTransportGroup = Database.findGroup("TRANSPORT LAYER")
    if (tcpipAutoConfigTransportGroup == None):
        tcpipAutoConfigTransportGroup = Database.createGroup("TCP/IP STACK", "TRANSPORT LAYER")     
    
    # Enable Transport Configurator
    tcpipAutoConfigTransEnable = tcpipAutoConfigTransportComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_TRANS_ENABLE", None)
    tcpipAutoConfigTransEnable.setVisible(False)
    tcpipAutoConfigTransEnable.setDefaultValue(True)

    # Enable TCP
    tcpipAutoConfigTCP = tcpipAutoConfigTransportComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_TCP", None)
    tcpipAutoConfigTCP.setLabel("TCP")
    tcpipAutoConfigTCP.setVisible(True)
    tcpipAutoConfigTCP.setDescription("Enable TCP") 
    tcpipAutoConfigTCP.setDependencies(tcpipAutoConfigTCPEnable, ["TCPIP_AUTOCONFIG_ENABLE_TCP"])
    
    # Enable UDP
    tcpipAutoConfigUDP = tcpipAutoConfigTransportComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_UDP", None)
    tcpipAutoConfigUDP.setLabel("UDP")
    tcpipAutoConfigUDP.setVisible(True)
    tcpipAutoConfigUDP.setDescription("Enable UDP") 
    tcpipAutoConfigUDP.setDependencies(tcpipAutoConfigUDPEnable, ["TCPIP_AUTOCONFIG_ENABLE_UDP"])
    
########################################################################################################
def finalizeComponent(tcpipAutoConfigTransportComponent):
    tcpipAutoConfigTransportGroup = Database.findGroup("TRANSPORT LAYER")
    tcpipAutoConfigTransportGroup.addComponent(tcpipAutoConfigTransportComponent.getID())
    
    if(Database.getSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_APPS_ENABLE") != True) and (Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_NET_ENABLE") != True) and (Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_DRV_ENABLE") != True) and (Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_BASIC_ENABLE") != True):
        Database.setActiveGroup("TRANSPORT LAYER")  
#######################################################################################################
def enableTcpipAutoConfigTrans(enable):

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
        
        if(Database.getComponentByID("tcpip_network_config") == None):
            res = tcpipAutoConfigNetworkGroup.addComponent("tcpip_network_config")
            res = Database.activateComponents(["tcpip_network_config"], "NETWORK LAYER", False)
            
        if(Database.getComponentByID("tcpip_driver_config") == None):
            res = tcpipAutoConfigDriverGroup.addComponent("tcpip_driver_config")
            res = Database.activateComponents(["tcpip_driver_config"], "DRIVER LAYER", False)   
            
        if(Database.getComponentByID("tcpip_basic_config") == None):
            res = tcpipAutoConfigBasicGroup.addComponent("tcpip_basic_config")
            res = Database.activateComponents(["tcpip_basic_config"], "BASIC CONFIGURATION", False)             
            
        if(Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_STACK") != True):
            setVal("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_STACK", True)
        if(Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_NETCONFIG") != True): 
            setVal("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_NETCONFIG", True)

#################Business Logic- Transport Layer #########################################  
def tcpipAutoConfigTCPEnable(symbol, event):
    tcpipAutoConfigTransportGroup = Database.findGroup("TRANSPORT LAYER")
    enableTcpipAutoConfigTrans(True)
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipTcp"], "TRANSPORT LAYER")  
        if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
            setVal("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True)
        tcpipAutoConfigTransportGroup.setAttachmentVisible("tcpipTcp", "libtcpipTcp")
        # Enable Sys Random H2 Adapter. if not enabled
        if(Database.getSymbolValue("tcpipStack", "TCPIP_STACK_SYS_RANDOM_H2_ADAPTER") != True): 
            setVal("tcpipStack", "TCPIP_STACK_SYS_RANDOM_H2_ADAPTER", True)
    else:
        res = Database.deactivateComponents(["tcpipTcp"])
        setVal("tcpipStack", "TCPIP_STACK_SYS_RANDOM_H2_ADAPTER", False)
    
def tcpipAutoConfigUDPEnable(symbol, event):
    tcpipAutoConfigTransportGroup = Database.findGroup("TRANSPORT LAYER")
    enableTcpipAutoConfigTrans(True)
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipUdp"], "TRANSPORT LAYER")  
        if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
            setVal("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True)
        tcpipAutoConfigTransportGroup.setAttachmentVisible("tcpipUdp", "libtcpipUdp")       
    else:
        res = Database.deactivateComponents(["tcpipUdp"])

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
        
