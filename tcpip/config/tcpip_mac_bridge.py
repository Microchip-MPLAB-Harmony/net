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
def tcpipBridgeMenuVisible(symbol, event):
    symbol.setVisible(event["value"])
    
def tcpipBridgeGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])
   
# Enable the bridge Commands
tcpipBridgeCommands = tcpipNetConfigComponent.createBooleanSymbol("TCPIP_STACK_MAC_BRIDGE_COMMANDS", tcpipNetConfigMacBridgeMenu)
tcpipBridgeCommands.setLabel("Enable the MAC Bridge Commands")
tcpipBridgeCommands.setVisible(False)
tcpipBridgeCommands.setDescription("Enable the MAC Bridge TCP/IP Commands")
tcpipBridgeCommands.setDefaultValue(False)
tcpipBridgeCommands.setDependencies(tcpipBridgeMenuVisible, ["TCPIP_STACK_USE_MAC_BRIDGE"])

# FDB entries
tcpipBridgeFDBEntries = tcpipNetConfigComponent.createIntegerSymbol("TCPIP_MAC_BRIDGE_FDB_TABLE_ENTRIES", tcpipNetConfigMacBridgeMenu)
tcpipBridgeFDBEntries.setLabel("Number of Entries in the FDB")
tcpipBridgeFDBEntries.setVisible(False)
tcpipBridgeFDBEntries.setDescription("Number of Entries in the Filtering Database")
tcpipBridgeFDBEntries.setDefaultValue(17)
tcpipBridgeFDBEntries.setDependencies(tcpipBridgeMenuVisible, ["TCPIP_STACK_USE_MAC_BRIDGE"])

# maximum number of ports
tcpipBridgeMaxPorts = tcpipNetConfigComponent.createIntegerSymbol("TCPIP_MAC_BRIDGE_MAX_PORTS_NO", tcpipNetConfigMacBridgeMenu)
tcpipBridgeMaxPorts.setLabel("Maximum Number of Ports in the Bridge")
tcpipBridgeMaxPorts.setVisible(False)
tcpipBridgeMaxPorts.setDescription("Maximum Number of Ports in the Bridge")
tcpipBridgeMaxPorts.setDefaultValue(2)
tcpipBridgeMaxPorts.setDependencies(tcpipBridgeMenuVisible, ["TCPIP_STACK_USE_MAC_BRIDGE"])

# packet pool size
tcpipBridgePktPool = tcpipNetConfigComponent.createIntegerSymbol("TCPIP_MAC_BRIDGE_PACKET_POOL_SIZE", tcpipNetConfigMacBridgeMenu)
tcpipBridgePktPool.setLabel("Number of Packets in the Pool")
tcpipBridgePktPool.setVisible(False)
tcpipBridgePktPool.setDescription("Number of Packets in the Pool")
tcpipBridgePktPool.setDefaultValue(8)
tcpipBridgePktPool.setDependencies(tcpipBridgeMenuVisible, ["TCPIP_STACK_USE_MAC_BRIDGE"])

# pool packet size
tcpipBridgePktSize = tcpipNetConfigComponent.createIntegerSymbol("TCPIP_MAC_BRIDGE_PACKET_SIZE", tcpipNetConfigMacBridgeMenu)
tcpipBridgePktSize.setLabel("Size of the Packets in the Pool")
tcpipBridgePktSize.setVisible(False)
tcpipBridgePktSize.setDescription("Size of the Packets in the Pool")
tcpipBridgePktSize.setDefaultValue(1536)
tcpipBridgePktSize.setDependencies(tcpipBridgeMenuVisible, ["TCPIP_STACK_USE_MAC_BRIDGE"])

# pool replenish
tcpipBridgePoolRepl = tcpipNetConfigComponent.createIntegerSymbol("TCPIP_MAC_BRIDGE_PACKET_POOL_REPLENISH", tcpipNetConfigMacBridgeMenu)
tcpipBridgePoolRepl.setLabel(" Number of packets to Replenish the pool")
tcpipBridgePoolRepl.setVisible(False)
tcpipBridgePoolRepl.setDescription(" Number of Packets to Replenish the Pool if Empty")
tcpipBridgePoolRepl.setDefaultValue(2)
tcpipBridgePoolRepl.setDependencies(tcpipBridgeMenuVisible, ["TCPIP_STACK_USE_MAC_BRIDGE"])

# descriptor pool size
tcpipBridgeDcptPool = tcpipNetConfigComponent.createIntegerSymbol("TCPIP_MAC_BRIDGE_DCPT_POOL_SIZE", tcpipNetConfigMacBridgeMenu)
tcpipBridgeDcptPool.setLabel("Number of Descriptors in the Pool")
tcpipBridgeDcptPool.setVisible(False)
tcpipBridgeDcptPool.setDescription("Number of Descriptors in the Pool")
tcpipBridgeDcptPool.setDefaultValue(16)
tcpipBridgeDcptPool.setDependencies(tcpipBridgeMenuVisible, ["TCPIP_STACK_USE_MAC_BRIDGE"])

# descriptor replenish
tcpipBridgeDcptRepl = tcpipNetConfigComponent.createIntegerSymbol("TCPIP_MAC_BRIDGE_DCPT_POOL_REPLENISH", tcpipNetConfigMacBridgeMenu)
tcpipBridgeDcptRepl.setLabel(" Number of Descriptors to Replenish the pool")
tcpipBridgeDcptRepl.setVisible(False)
tcpipBridgeDcptRepl.setDescription(" Number of Descriptors to Replenish the Pool if Empty")
tcpipBridgeDcptRepl.setDefaultValue(4)
tcpipBridgeDcptRepl.setDependencies(tcpipBridgeMenuVisible, ["TCPIP_STACK_USE_MAC_BRIDGE"])

# Advanced Settings
tcpipBridgeAdvSettings = tcpipNetConfigComponent.createMenuSymbol("TCPIP_MAC_BRIDGE_ADV_SETTING", tcpipNetConfigMacBridgeMenu)
tcpipBridgeAdvSettings.setLabel("Advanced Settings")
tcpipBridgeAdvSettings.setDescription("Advanced Settings")
tcpipBridgeAdvSettings.setVisible(False)
tcpipBridgeAdvSettings.setDependencies(tcpipBridgeMenuVisible, ["TCPIP_STACK_USE_MAC_BRIDGE"])

# Purge timeout
tcpipBridgeTmo = tcpipNetConfigComponent.createIntegerSymbol("TCPIP_MAC_BRIDGE_ENTRY_TIMEOUT", tcpipBridgeAdvSettings)
tcpipBridgeTmo.setLabel("Timeout for an Entry to be Purged")
tcpipBridgeTmo.setVisible(False)
tcpipBridgeTmo.setDescription("Timeout for an Entry to be Purged from the FDB")
tcpipBridgeTmo.setDefaultValue(300)
tcpipBridgeTmo.setDependencies(tcpipBridgeMenuVisible, ["TCPIP_STACK_USE_MAC_BRIDGE"])

# transit delay
tcpipBridgeTransit = tcpipNetConfigComponent.createIntegerSymbol("TCPIP_MAC_BRIDGE_MAX_TRANSIT_DELAY", tcpipBridgeAdvSettings)
tcpipBridgeTransit.setLabel("Maximum Transit Delay")
tcpipBridgeTransit.setVisible(False)
tcpipBridgeTransit.setDescription("Maximum Transit Delay within the Bridge")
tcpipBridgeTransit.setDefaultValue(1)
tcpipBridgeTransit.setDependencies(tcpipBridgeMenuVisible, ["TCPIP_STACK_USE_MAC_BRIDGE"])

# Dynamic Learning 
tcpipBridgeDynLearn = tcpipNetConfigComponent.createBooleanSymbol("TCPIP_MAC_BRIDGE_NO_DYNAMIC_LEARN", tcpipBridgeAdvSettings)
tcpipBridgeDynLearn.setLabel("Do not Learn Dynamic Addresses")
tcpipBridgeDynLearn.setVisible(False)
tcpipBridgeDynLearn.setDescription("Do not Learn Dynamic Addresses, Use Only Static FDB Entries")
tcpipBridgeDynLearn.setDefaultValue(False)
tcpipBridgeDynLearn.setDependencies(tcpipBridgeMenuVisible, ["TCPIP_STACK_USE_MAC_BRIDGE"])

# FDB Entry exists
tcpipBridgeEntryExists = tcpipNetConfigComponent.createBooleanSymbol("TCPIP_MAC_BRIDGE_FDB_ENTRY_EXISTS", tcpipBridgeAdvSettings)
tcpipBridgeEntryExists.setLabel("Forward Traffic Only if Entry Exists")
tcpipBridgeEntryExists.setVisible(False)
tcpipBridgeEntryExists.setDescription("Forward Traffic Only if Entry Exists in FDB")
tcpipBridgeEntryExists.setDefaultValue(False)
tcpipBridgeEntryExists.setDependencies(tcpipBridgeMenuVisible, ["TCPIP_STACK_USE_MAC_BRIDGE"])

# Initialize with interface names
tcpipBridgeIfName = tcpipNetConfigComponent.createBooleanSymbol("TCPIP_MAC_BRIDGE_IF_NAME_TABLE", tcpipBridgeAdvSettings)
tcpipBridgeIfName.setLabel("Use Interface Names for Initialization")
tcpipBridgeIfName.setVisible(False)
tcpipBridgeIfName.setDescription("Use Interface Names for Initialization, ASCII Strings")
tcpipBridgeIfName.setDefaultValue(False)
tcpipBridgeIfName.setDependencies(tcpipBridgeMenuVisible, ["TCPIP_STACK_USE_MAC_BRIDGE"])

# Bridge Task Tick Rate in ms
tcpipBridgeTaskTickRate = tcpipNetConfigComponent.createIntegerSymbol("TCPIP_MAC_BRIDGE_TASK_RATE", tcpipBridgeAdvSettings)
tcpipBridgeTaskTickRate.setLabel("Bridge Task Rate (in msec)")
tcpipBridgeTaskTickRate.setVisible(False)
tcpipBridgeTaskTickRate.setDescription("Bridge Maintenance Task Rate (in msec)")
tcpipBridgeTaskTickRate.setDefaultValue(333)
tcpipBridgeTaskTickRate.setDependencies(tcpipBridgeMenuVisible, ["TCPIP_STACK_USE_MAC_BRIDGE"])

#####################################################################################################
# Add to configuration.h
tcpipBridgeHeaderFtl = tcpipNetConfigComponent.createFileSymbol(None, None)
tcpipBridgeHeaderFtl.setSourcePath("tcpip/config/tcpip_mac_bridge.h.ftl")
tcpipBridgeHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
tcpipBridgeHeaderFtl.setMarkup(True)
tcpipBridgeHeaderFtl.setType("STRING")

# Add the tcpip_mac_bridge.c file
tcpipBridgeSourceFile = tcpipNetConfigComponent.createFileSymbol(None, None)
tcpipBridgeSourceFile.setSourcePath("tcpip/src/tcpip_mac_bridge.c")
tcpipBridgeSourceFile.setOutputName("tcpip_mac_bridge.c")
tcpipBridgeSourceFile.setOverwrite(True)
tcpipBridgeSourceFile.setDestPath("library/tcpip/src/")
tcpipBridgeSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
tcpipBridgeSourceFile.setType("SOURCE")
tcpipBridgeSourceFile.setEnabled(False)
tcpipBridgeSourceFile.setDependencies(tcpipBridgeGenSourceFile, ["TCPIP_STACK_USE_MAC_BRIDGE"])





