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
    tcpipNetConfigInterfaceCount.setLabel("Number of Instances")
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

    