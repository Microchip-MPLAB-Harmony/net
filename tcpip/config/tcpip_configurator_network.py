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
def instantiateComponent(tcpipAutoConfigNetworkComponent):
    processor = Variables.get("__PROCESSOR")
    tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
    if (tcpipAutoConfigStackGroup == None):
        tcpipAutoConfigStackGroup = Database.createGroup(None, "TCP/IP STACK")
        
    tcpipAutoConfigNetworkGroup = Database.findGroup("NETWORK LAYER")
    if (tcpipAutoConfigNetworkGroup == None):
        tcpipAutoConfigNetworkGroup = Database.createGroup("TCP/IP STACK", "NETWORK LAYER") 
    
    # Enable Network Configurator
    tcpipAutoConfigNetEnable = tcpipAutoConfigNetworkComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_NET_ENABLE", None)
    tcpipAutoConfigNetEnable.setVisible(False)
    tcpipAutoConfigNetEnable.setDefaultValue(True)

    # Enable ARP
    tcpipAutoConfigARP = tcpipAutoConfigNetworkComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_ARP", None)
    tcpipAutoConfigARP.setLabel("ARP")
    tcpipAutoConfigARP.setVisible(True)
    tcpipAutoConfigARP.setDescription("Enable ARP") 
    tcpipAutoConfigARP.setDependencies(tcpipAutoConfigARPEnable, ["TCPIP_AUTOCONFIG_ENABLE_ARP"])
    
    # Enable ICMPv4
    tcpipAutoConfigICMPv4 = tcpipAutoConfigNetworkComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_ICMPv4", None)
    tcpipAutoConfigICMPv4.setLabel("ICMPv4")
    tcpipAutoConfigICMPv4.setVisible(True)
    tcpipAutoConfigICMPv4.setDescription("Enable ICMPv4")
    tcpipAutoConfigICMPv4.setDependencies(tcpipAutoConfigICMPEnable, ["TCPIP_AUTOCONFIG_ENABLE_ICMPv4"])
    
    # Enable IGMP
    tcpipAutoConfigIGMP = tcpipAutoConfigNetworkComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_IGMP", None)
    tcpipAutoConfigIGMP.setLabel("IGMP")
    tcpipAutoConfigIGMP.setVisible(True)
    tcpipAutoConfigIGMP.setDescription("Enable IGMP")   
    tcpipAutoConfigIGMP.setDependencies(tcpipAutoConfigIGMPEnable, ["TCPIP_AUTOCONFIG_ENABLE_IGMP"])
    
    # Enable IPv4
    tcpipAutoConfigIPv4 = tcpipAutoConfigNetworkComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_IPV4", None)
    tcpipAutoConfigIPv4.setLabel("IPv4")
    tcpipAutoConfigIPv4.setVisible(True)
    tcpipAutoConfigIPv4.setDescription("Enable IPv4")
    tcpipAutoConfigIPv4.setDependencies(tcpipAutoConfigIPv4Enable, ["TCPIP_AUTOCONFIG_ENABLE_IPV4"])

    # Enable IPv6
    tcpipAutoConfigIPv6 = tcpipAutoConfigNetworkComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_IPV6", None)
    tcpipAutoConfigIPv6.setLabel("IPv6")
    tcpipAutoConfigIPv6.setVisible(True)
    tcpipAutoConfigIPv6.setDescription("Enable IPv6")
    tcpipAutoConfigIPv6.setDependencies(tcpipAutoConfigIPv6Enable, ["TCPIP_AUTOCONFIG_ENABLE_IPV6"])

    # Enable ICMPv6
    tcpipAutoConfigICMPv6 = tcpipAutoConfigNetworkComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_ICMPv6", None)
    tcpipAutoConfigICMPv6.setLabel("ICMPv6")
    tcpipAutoConfigICMPv6.setVisible(True)
    tcpipAutoConfigICMPv6.setDescription("Enable ICMPv6")   
    tcpipAutoConfigICMPv6.setDependencies(tcpipAutoConfigIcmpv6Enable, ["TCPIP_AUTOCONFIG_ENABLE_ICMPv6"])  
    
    # Enable NDP
    tcpipAutoConfigNDP = tcpipAutoConfigNetworkComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_NDP", None)
    tcpipAutoConfigNDP.setLabel("NDP")
    tcpipAutoConfigNDP.setVisible(True)
    tcpipAutoConfigNDP.setDescription("Enable NDP") 
    tcpipAutoConfigNDP.setDependencies(tcpipAutoConfigNDPEnable, ["TCPIP_AUTOCONFIG_ENABLE_NDP"])
    
########################################################################################################
def finalizeComponent(tcpipAutoConfigNetworkComponent):
    tcpipAutoConfigNetworkGroup = Database.findGroup("NETWORK LAYER")
    tcpipAutoConfigNetworkGroup.addComponent(tcpipAutoConfigNetworkComponent.getID())
    
    if(Database.getSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_APPS_ENABLE") != True) and (Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_TRANS_ENABLE") != True) and (Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_DRV_ENABLE") != True) and (Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_BASIC_ENABLE") != True):
        Database.setActiveGroup("NETWORK LAYER")        
#######################################################################################################
def enableTcpipAutoConfigNet(enable):

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
#################Business Logic- Network Layer #########################################
def tcpipAutoConfigARPEnable(symbol, event):    
    tcpipAutoConfigNetworkGroup = Database.findGroup("NETWORK LAYER")
    enableTcpipAutoConfigNet(True)  
    if (event["value"] == True):        
        res = Database.activateComponents(["tcpipArp"],"NETWORK LAYER") 
        tcpipAutoConfigNetworkGroup.setAttachmentVisible("tcpipArp", "libtcpipArp")
    else:       
        res = Database.deactivateComponents(["tcpipArp"])   

def tcpipAutoConfigICMPEnable(symbol, event):
    tcpipAutoConfigNetworkGroup = Database.findGroup("NETWORK LAYER")
    enableTcpipAutoConfigNet(True)
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipIcmp"], "NETWORK LAYER")   
        tcpipAutoConfigNetworkGroup.setAttachmentVisible("tcpipIcmp", "libtcpipIcmp")
        if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
            Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
    else:
        res = Database.deactivateComponents(["tcpipIcmp"])

def tcpipAutoConfigIGMPEnable(symbol, event):
    tcpipAutoConfigNetworkGroup = Database.findGroup("NETWORK LAYER")
    enableTcpipAutoConfigNet(True)
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipIgmp"],"NETWORK LAYER")    
        tcpipAutoConfigNetworkGroup.setAttachmentVisible("tcpipIgmp", "libtcpipIgmp")
        if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
            Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
    else:
        res = Database.deactivateComponents(["tcpipIgmp"])
    
def tcpipAutoConfigIPv4Enable(symbol, event):
    tcpipAutoConfigNetworkGroup = Database.findGroup("NETWORK LAYER")
    enableTcpipAutoConfigNet(True)
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipIPv4"],"NETWORK LAYER")    
        tcpipAutoConfigNetworkGroup.setAttachmentVisible("tcpipIPv4", "libTcpipIPv4")
        if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_ARP") != True):
            Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_ARP", True, 2)
    else:
        res = Database.deactivateComponents(["tcpipIPv4"])
    
def tcpipAutoConfigIPv6Enable(symbol, event):
    tcpipAutoConfigNetworkGroup = Database.findGroup("NETWORK LAYER")
    enableTcpipAutoConfigNet(True)
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipIPv6"],"NETWORK LAYER")    
        tcpipAutoConfigNetworkGroup.setAttachmentVisible("tcpipIPv6", "libTcpipIPv6")
        if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_ICMPv6") != True):
            Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_ICMPv6", True, 2)
        if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_NDP") != True):
            Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_NDP", True, 2)
    else:
        res = Database.deactivateComponents(["tcpipIPv6"])
        if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_ICMPv6") == True):
            Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_ICMPv6", False)
        if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_NDP") == True):
            Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_NDP", False)
    
def tcpipAutoConfigIcmpv6Enable(symbol, event):
    tcpipAutoConfigNetworkGroup = Database.findGroup("NETWORK LAYER")
    enableTcpipAutoConfigNet(True)
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipIcmpv6"],"NETWORK LAYER")  
        tcpipAutoConfigNetworkGroup.setAttachmentVisible("tcpipIcmpv6", "libtcpipIcmpv6")
    else:
        res = Database.deactivateComponents(["tcpipIcmpv6"])
        
def tcpipAutoConfigNDPEnable(symbol, event):
    tcpipAutoConfigNetworkGroup = Database.findGroup("NETWORK LAYER")
    enableTcpipAutoConfigNet(True)
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipNdp"],"NETWORK LAYER") 
        tcpipAutoConfigNetworkGroup.setAttachmentVisible("tcpipNdp", "libtcpipNdp")
    else:
        res = Database.deactivateComponents(["tcpipNdp"])

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
    

    
