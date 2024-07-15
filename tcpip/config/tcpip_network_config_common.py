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

tcpipNetConfigNumMaximum = 10
macBridgeCount = 0   

def instantiateComponent(tcpipNetConfigComponent):
    configName = Variables.get("__CONFIGURATION_NAME") 
    seriesNode = ATDF.getNode('/avr-tools-device-file/devices')
    deviceSeries = seriesNode.getChildren()[0].getAttribute("series")
    
    tcpipNetConfigNumMax = tcpipNetConfigComponent.createIntegerSymbol("TCPIP_STACK_NETWORK_CONFIG_NUMBER_MAX", None)
    tcpipNetConfigNumMax.setLabel("Number of Instances")
    tcpipNetConfigNumMax.setDefaultValue(tcpipNetConfigNumMaximum)
    tcpipNetConfigNumMax.setVisible(False)
    
    tcpipNetConfigInterfaceCount = tcpipNetConfigComponent.createIntegerSymbol("TCPIP_STACK_NETWORK_INTERAFCE_COUNT", None)
    tcpipNetConfigInterfaceCount.setHelp("mcc_h3_netconfig_configurations")
    tcpipNetConfigInterfaceCount.setLabel("Number of Instances")
    if Variables.get("__H3_RUNTIME") == "MCC":
        tcpipNetConfigInterfaceCount.setDefaultValue(0)
    else:
        tcpipNetConfigInterfaceCount.setDefaultValue(1)
    tcpipNetConfigInterfaceCount.setVisible(True)
    tcpipNetConfigInterfaceCount.setReadOnly(True)   
    
    tcpipNetConfigMacBridgeCount = tcpipNetConfigComponent.createIntegerSymbol("TCPIP_STACK_NETWORK_MAC_BRIDGE_COUNT", None)
    tcpipNetConfigMacBridgeCount.setLabel("Number of Bridge Enabled")
    tcpipNetConfigMacBridgeCount.setDefaultValue(0)
    tcpipNetConfigMacBridgeCount.setVisible(False)
    
    # MAC Bridge Configuration Menu
    tcpipNetConfigMacBridgeMenu = tcpipNetConfigComponent.createMenuSymbol("TCPIP_STACK_NETWORK_MAC_BRIDGE_CONFIG",None)
    tcpipNetConfigMacBridgeMenu.setLabel("MAC Bridge Configuration")
    tcpipNetConfigMacBridgeMenu.setVisible(False)   
    tcpipNetConfigMacBridgeMenu.setDependencies(tcpipNetConfigMACBridgeMenuVisible, ["TCPIP_STACK_NETWORK_MAC_BRIDGE_COUNT"])     
    
    # Enable MAC Bridge Configurations    
    tcpipNetConfigMacBridgeGlobalEnable = tcpipNetConfigComponent.createBooleanSymbol("TCPIP_STACK_USE_MAC_BRIDGE", None)
    tcpipNetConfigMacBridgeGlobalEnable.setLabel("Mac Bridge Global Config Enable")
    tcpipNetConfigMacBridgeGlobalEnable.setDefaultValue(False)
    tcpipNetConfigMacBridgeGlobalEnable.setVisible(False)     
    
    # Device Series
    tcpipNetConfigDeviceSeries = tcpipNetConfigComponent.createStringSymbol("TCPIP_STACK_NETWORK_DEV_SERIES",None)   
    tcpipNetConfigDeviceSeries.setLabel("Device Series")
    tcpipNetConfigDeviceSeries.setVisible(False)
    tcpipNetConfigDeviceSeries.setDefaultValue(deviceSeries)
    tcpipNetConfigDeviceSeries.setReadOnly(True)
    
    #Add to system_config.h
    tcpipNetConfig = tcpipNetConfigComponent.createFileSymbol(None, None)
    tcpipNetConfig.setSourcePath("tcpip/config/network_config.h.ftl")
    tcpipNetConfig.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipNetConfig.setMarkup(True)
    tcpipNetConfig.setType("STRING")
    
    execfile(Module.getPath() + "/tcpip/config/tcpip_mac_bridge.py")    
    
def tcpipNetConfigMACBridgeMenuVisible(symbol, event):
    if (event["value"] > 1):
        symbol.setVisible(True)
        Database.setSymbolValue("tcpipNetConfig", "TCPIP_STACK_USE_MAC_BRIDGE", True)
    else:
        symbol.setVisible(False)
        Database.setSymbolValue("tcpipNetConfig", "TCPIP_STACK_USE_MAC_BRIDGE", False)
        
#Handle messages from other components
def handleMessage(messageID, args):
    retDict= {}
    global macBridgeCount
    if (messageID == "NETCONFIG_INTERFACE_COUNTER_INC"):
        netconfif_count = Database.getSymbolValue("tcpipNetConfig", "TCPIP_STACK_NETWORK_INTERAFCE_COUNT")
        Database.setSymbolValue("tcpipNetConfig", "TCPIP_STACK_NETWORK_INTERAFCE_COUNT", netconfif_count + 1)
        retDict= {"Return": "Success"}
    elif (messageID == "NETCONFIG_INTERFACE_COUNTER_DEC"):
        netconfif_count = Database.getSymbolValue("tcpipNetConfig", "TCPIP_STACK_NETWORK_INTERAFCE_COUNT")
        Database.setSymbolValue("tcpipNetConfig", "TCPIP_STACK_NETWORK_INTERAFCE_COUNT", netconfif_count - 1)
        retDict= {"Return": "Success"}
    elif (messageID == "NETCONFIG_MAC_BRIDGE_ENABLE"):
        macBridgeCount = macBridgeCount + 1        
        Database.setSymbolValue("tcpipNetConfig", "TCPIP_STACK_NETWORK_MAC_BRIDGE_COUNT", macBridgeCount)
        retDict= {"Return": "Success"}
    elif (messageID == "NETCONFIG_MAC_BRIDGE_DISABLE"):
        if (macBridgeCount != 0):
            macBridgeCount = macBridgeCount - 1        
        Database.setSymbolValue("tcpipNetConfig", "TCPIP_STACK_NETWORK_MAC_BRIDGE_COUNT", macBridgeCount)
        retDict= {"Return": "Success"}
    elif (messageID == "SET_SYMBOL"):
        print "handleMessage: Set Symbol"
        retDict= {"Return": "Success"}
        Database.setSymbolValue(args["Component"], args["Id"], args["Value"])
    else:
        retDict= {"Return": "UnImplemented Command"}
        
    return retDict

#Set symbols of other components
def setVal(component, symbol, value):
    triggerDict = {"Component":component,"Id":symbol, "Value":value}
    if(Database.sendMessage(component, "SET_SYMBOL", triggerDict) == None):
        print "Set Symbol Failure" + component + ":" + symbol + ":" + str(value)
        return False
    else:
        return True

    