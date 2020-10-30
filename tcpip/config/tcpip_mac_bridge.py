"""*****************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
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

def instantiateComponent(tcpipBridgeComponent):
    print("TCPIP MAC Bridge Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # Enable the bridge   
    tcpipMacBridge = tcpipBridgeComponent.createBooleanSymbol("TCPIP_STACK_USE_MAC_BRIDGE", None)
    tcpipMacBridge.setLabel("MAC Bridge")
    tcpipMacBridge.setVisible(False)
    tcpipMacBridge.setDescription("Enable the MAC Bridge")
    tcpipMacBridge.setDefaultValue(True)
    
    # Enable the bridge Commands
    tcpipBridgeCommands = tcpipBridgeComponent.createBooleanSymbol("TCPIP_STACK_MAC_BRIDGE_COMMANDS", None)
    tcpipBridgeCommands.setLabel("Enable the MAC Bridge Commands")
    tcpipBridgeCommands.setVisible(True)
    tcpipBridgeCommands.setDescription("Enable the MAC Bridge TCP/IP Commands")
    tcpipBridgeCommands.setDefaultValue(False)
    
    # FDB entries
    tcpipBridgeFDBEntries = tcpipBridgeComponent.createIntegerSymbol("TCPIP_MAC_BRIDGE_FDB_TABLE_ENTRIES", None)
    tcpipBridgeFDBEntries.setLabel("Number of Entries in the FDB")
    tcpipBridgeFDBEntries.setVisible(True)
    tcpipBridgeFDBEntries.setDescription("Number of Entries in the Filtering Database")
    tcpipBridgeFDBEntries.setDefaultValue(17)

    # maximum number of ports
    tcpipBridgeMaxPorts = tcpipBridgeComponent.createIntegerSymbol("TCPIP_MAC_BRIDGE_MAX_PORTS_NO", None)
    tcpipBridgeMaxPorts.setLabel("Maximum Number of Ports in the Bridge")
    tcpipBridgeMaxPorts.setVisible(True)
    tcpipBridgeMaxPorts.setDescription("Maximum Number of Ports in the Bridge")
    tcpipBridgeMaxPorts.setDefaultValue(2)

    # packet pool size
    tcpipBridgePktPool = tcpipBridgeComponent.createIntegerSymbol("TCPIP_MAC_BRIDGE_PACKET_POOL_SIZE", None)
    tcpipBridgePktPool.setLabel("Number of Packets in the Pool")
    tcpipBridgePktPool.setVisible(True)
    tcpipBridgePktPool.setDescription("Number of Packets in the Pool")
    tcpipBridgePktPool.setDefaultValue(8)

    # pool packet size
    tcpipBridgePktSize = tcpipBridgeComponent.createIntegerSymbol("TCPIP_MAC_BRIDGE_PACKET_SIZE", None)
    tcpipBridgePktSize.setLabel("Size of the Packets in the Pool")
    tcpipBridgePktSize.setVisible(True)
    tcpipBridgePktSize.setDescription("Size of the Packets in the Pool")
    tcpipBridgePktSize.setDefaultValue(1536)

    # pool replenish
    tcpipBridgePoolRepl = tcpipBridgeComponent.createIntegerSymbol("TCPIP_MAC_BRIDGE_PACKET_POOL_REPLENISH", None)
    tcpipBridgePoolRepl.setLabel(" Number of packets to Replenish the pool")
    tcpipBridgePoolRepl.setVisible(True)
    tcpipBridgePoolRepl.setDescription(" Number of Packets to Replenish the Pool if Empty")
    tcpipBridgePoolRepl.setDefaultValue(2)

    # descriptor pool size
    tcpipBridgeDcptPool = tcpipBridgeComponent.createIntegerSymbol("TCPIP_MAC_BRIDGE_DCPT_POOL_SIZE", None)
    tcpipBridgeDcptPool.setLabel("Number of Descriptors in the Pool")
    tcpipBridgeDcptPool.setVisible(True)
    tcpipBridgeDcptPool.setDescription("Number of Descriptors in the Pool")
    tcpipBridgeDcptPool.setDefaultValue(16)

    # descriptor replenish
    tcpipBridgeDcptRepl = tcpipBridgeComponent.createIntegerSymbol("TCPIP_MAC_BRIDGE_DCPT_POOL_REPLENISH", None)
    tcpipBridgeDcptRepl.setLabel(" Number of Descriptors to Replenish the pool")
    tcpipBridgeDcptRepl.setVisible(True)
    tcpipBridgeDcptRepl.setDescription(" Number of Descriptors to Replenish the Pool if Empty")
    tcpipBridgeDcptRepl.setDefaultValue(4)

    # Advanced Settings
    tcpipBridgeAdvSettings = tcpipBridgeComponent.createMenuSymbol("TCPIP_MAC_BRIDGE_ADV_SETTING", None)
    tcpipBridgeAdvSettings.setLabel("Advanced Settings")
    tcpipBridgeAdvSettings.setDescription("Advanced Settings")
    tcpipBridgeAdvSettings.setVisible(True)
    
    # Purge timeout
    tcpipBridgeTmo = tcpipBridgeComponent.createIntegerSymbol("TCPIP_MAC_BRIDGE_ENTRY_TIMEOUT", tcpipBridgeAdvSettings)
    tcpipBridgeTmo.setLabel("Timeout for an Entry to be Purged")
    tcpipBridgeTmo.setVisible(True)
    tcpipBridgeTmo.setDescription("Timeout for an Entry to be Purged from the FDB")
    tcpipBridgeTmo.setDefaultValue(300)

    # transit delay
    tcpipBridgeTransit = tcpipBridgeComponent.createIntegerSymbol("TCPIP_MAC_BRIDGE_MAX_TRANSIT_DELAY", tcpipBridgeAdvSettings)
    tcpipBridgeTransit.setLabel("Maximum Transit Delay")
    tcpipBridgeTransit.setVisible(True)
    tcpipBridgeTransit.setDescription("Maximum Transit Delay within the Bridge")
    tcpipBridgeTransit.setDefaultValue(1)

    # Dynamic Learning 
    tcpipBridgeDynLearn = tcpipBridgeComponent.createBooleanSymbol("TCPIP_MAC_BRIDGE_NO_DYNAMIC_LEARN", tcpipBridgeAdvSettings)
    tcpipBridgeDynLearn.setLabel("Do not Learn Dynamic Addresses")
    tcpipBridgeDynLearn.setVisible(True)
    tcpipBridgeDynLearn.setDescription("Do not Learn Dynamic Addresses, Use Only Static FDB Entries")
    tcpipBridgeDynLearn.setDefaultValue(False)

    # FDB Entry exists
    tcpipBridgeEntryExists = tcpipBridgeComponent.createBooleanSymbol("TCPIP_MAC_BRIDGE_FDB_ENTRY_EXISTS", tcpipBridgeAdvSettings)
    tcpipBridgeEntryExists.setLabel("Forward Traffic Only if Entry Exists")
    tcpipBridgeEntryExists.setVisible(True)
    tcpipBridgeEntryExists.setDescription("Forward Traffic Only if Entry Exists in FDB")
    tcpipBridgeEntryExists.setDefaultValue(False)

    # Initialize with interface names
    tcpipBridgeIfName = tcpipBridgeComponent.createBooleanSymbol("TCPIP_MAC_BRIDGE_IF_NAME_TABLE", tcpipBridgeAdvSettings)
    tcpipBridgeIfName.setLabel("Use Interface Names for Initialization")
    tcpipBridgeIfName.setVisible(True)
    tcpipBridgeIfName.setDescription("Use Interface Names for Initialization, ASCII Strings")
    tcpipBridgeIfName.setDefaultValue(False)

    # Bridge Task Tick Rate in ms
    tcpipBridgeTaskTickRate = tcpipBridgeComponent.createIntegerSymbol("TCPIP_MAC_BRIDGE_TASK_RATE", tcpipBridgeAdvSettings)
    tcpipBridgeTaskTickRate.setLabel("Bridge Task Rate (in msec)")
    tcpipBridgeTaskTickRate.setVisible(True)
    tcpipBridgeTaskTickRate.setDescription("Bridge Maintenance Task Rate (in msec)")
    tcpipBridgeTaskTickRate.setDefaultValue(333)

    #####################################################################################################
    # Add to configuration.h
    tcpipBridgeHeaderFtl = tcpipBridgeComponent.createFileSymbol(None, None)
    tcpipBridgeHeaderFtl.setSourcePath("tcpip/config/tcpip_mac_bridge.h.ftl")
    tcpipBridgeHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipBridgeHeaderFtl.setMarkup(True)
    tcpipBridgeHeaderFtl.setType("STRING")

    # Add the tcpip_mac_bridge.c file
    tcpipBridgeSourceFile = tcpipBridgeComponent.createFileSymbol(None, None)
    tcpipBridgeSourceFile.setSourcePath("tcpip/src/tcpip_mac_bridge.c")
    tcpipBridgeSourceFile.setOutputName("tcpip_mac_bridge.c")
    tcpipBridgeSourceFile.setOverwrite(True)
    tcpipBridgeSourceFile.setDestPath("library/tcpip/src/")
    tcpipBridgeSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipBridgeSourceFile.setType("SOURCE")
    tcpipBridgeSourceFile.setEnabled(True)
    #tcpipBridgeSourceFile.setDependencies(tcpipBridgeGenSourceFile, ["TCPIP_STACK_USE_MAC_BRIDGE"])
    

def tcpipBridgeMenuVisible(symbol, event):
    if (event["value"] == True):
        print("TCPIP Menu Visible.")        
        symbol.setVisible(True)
    else:
        print("TCPIP Menu Invisible.")
        symbol.setVisible(False)
        

def tcpipBridgeGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])


# Set symbols of other components
def setVal(component, symbol, value):
    triggerDict = {"Component":component,"Id":symbol, "Value":value}
    if(Database.sendMessage(component, "SET_SYMBOL", triggerDict) == None):
        print "Set Symbol Failure" + component + ":" + symbol + ":" + str(value)
        return False
    else:
        return True

# Handle messages from other components
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
    Database.setSymbolValue("tcpipMacBridge", "TCPIP_STACK_USE_MAC_BRIDGE", False, 2)


