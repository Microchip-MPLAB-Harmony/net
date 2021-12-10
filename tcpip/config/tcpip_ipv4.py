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

tcpipIPv4FwdEntriesMax = 10
tcpipIPv4FwdEntriesMin = 1
tcpipIPv4FwdPrev_cnt  = 0

tcpipIPv4FwdEntry = []
tcpipIPv4FwdBool = []
tcpipIPv4FwdEntryNetAdd = []
tcpipIPv4FwdEntryNetMask = []
tcpipIPv4FwdEntryGwAdd = []
tcpipIPv4FwdEntrySrcIfName = []
tcpipIPv4FwdEntryDestIfName = []
tcpipIPv4FwdEntryMetric = []
    
def instantiateComponent(tcpipIPv4Component):
    print("TCPIP IPv4 Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # Enable IPv4   
    tcpipIPv4 = tcpipIPv4Component.createBooleanSymbol("TCPIP_STACK_USE_IPV4", None)
    tcpipIPv4.setHelp("mcc_h3_ipv4_configurations")
    tcpipIPv4.setLabel("IPv4")
    tcpipIPv4.setVisible(False)
    tcpipIPv4.setDescription("Enable IPv4")
    tcpipIPv4.setDefaultValue(True)
    
    # Enable IPv4 Commands
    tcpipIpv4Commands = tcpipIPv4Component.createBooleanSymbol("TCPIP_IPV4_COMMANDS_ENABLE", None)
    tcpipIpv4Commands.setHelp("mcc_h3_ipv4_configurations")
    tcpipIpv4Commands.setLabel("Enable IPv4 Commands")
    tcpipIpv4Commands.setVisible(True)
    tcpipIpv4Commands.setDescription("Enable the IPv4 TCP/IP Commands")
    tcpipIpv4Commands.setDefaultValue(False)
    
    # Advanced Settings
    tcpipIPv4AdvSettings = tcpipIPv4Component.createMenuSymbol("TCPIP_IPV4_ADV_SETTING", None)
    tcpipIPv4AdvSettings.setLabel("Advanced Settings")
    tcpipIPv4AdvSettings.setDescription("Advanced Settings")
    tcpipIPv4AdvSettings.setVisible(True)
    
    # ARP queue slots
    tcpipIPv4ArpSlots = tcpipIPv4Component.createIntegerSymbol("TCPIP_IPV4_ARP_QUEUE_SLOTS", tcpipIPv4AdvSettings)
    tcpipIPv4ArpSlots.setHelp("mcc_h3_ipv4_configurations")
    tcpipIPv4ArpSlots.setLabel("Number of ARP Requests IPv4 can queue")
    tcpipIPv4ArpSlots.setVisible(True)
    tcpipIPv4ArpSlots.setDescription("Number of ARP requests that IPv4 Can Queue Up")
    tcpipIPv4ArpSlots.setDefaultValue(10)

    # Load IPv4 Configurations
    tcpipIPv4Frag = tcpipIPv4Component.createBooleanSymbol("TCPIP_IPV4_FRAGMENTATION", tcpipIPv4AdvSettings)
    tcpipIPv4Frag.setHelp("mcc_h3_ipv4_configurations")
    tcpipIPv4Frag.setLabel("Enable IPv4 Fragmentation Support")
    tcpipIPv4Frag.setVisible(True)
    tcpipIPv4Frag.setDescription("Enable IPv4 fragmentation support")
    tcpipIPv4Frag.setDefaultValue(False)
    #tcpipIPv4Frag.setDependencies(tcpipIPv4MenuVisible, ["TCPIP_STACK_USE_IPV4"])

    # Settings for IPv4 Fragmentation
    tcpipIPv4FragSetting = tcpipIPv4Component.createMenuSymbol("TCPIP_IPV4_FRAGMENT_SETTING", tcpipIPv4Frag)
    tcpipIPv4FragSetting.setLabel("IPv4 Fragmentation Settings")
    tcpipIPv4FragSetting.setVisible(False)
    tcpipIPv4FragSetting.setDescription("IPv4 Fragmentation Settings")
    tcpipIPv4FragSetting.setDependencies(tcpipIPv4MenuVisible, ["TCPIP_IPV4_FRAGMENTATION"])

    # Initial fragment timeout in seconds
    tcpipIPv4FragTimeout = tcpipIPv4Component.createIntegerSymbol("TCPIP_IPV4_FRAGMENT_TIMEOUT", tcpipIPv4FragSetting)
    tcpipIPv4FragTimeout.setHelp("mcc_h3_ipv4_configurations")
    tcpipIPv4FragTimeout.setLabel("Initial fragment timeout (in sec)")
    tcpipIPv4FragTimeout.setVisible(True)
    tcpipIPv4FragTimeout.setDescription("Initial fragment timeout in seconds")
    tcpipIPv4FragTimeout.setDefaultValue(15)

    # Limit for the number of fragmented streams
    tcpipIPv4FragStreamMaxNum = tcpipIPv4Component.createIntegerSymbol("TCPIP_IPV4_FRAGMENT_MAX_STREAMS", tcpipIPv4FragSetting)
    tcpipIPv4FragStreamMaxNum.setHelp("mcc_h3_ipv4_configurations")
    tcpipIPv4FragStreamMaxNum.setLabel("Maximum number of fragmented streams")
    tcpipIPv4FragStreamMaxNum.setVisible(True)
    tcpipIPv4FragStreamMaxNum.setDescription("Maximum limit for number of fragmented streams")
    tcpipIPv4FragStreamMaxNum.setDefaultValue(3)

    # Limit for the number of fragments
    tcpipIPv4FragMaxNum = tcpipIPv4Component.createIntegerSymbol("TCPIP_IPV4_FRAGMENT_MAX_NUMBER", tcpipIPv4FragSetting)
    tcpipIPv4FragMaxNum.setHelp("mcc_h3_ipv4_configurations")
    tcpipIPv4FragMaxNum.setLabel("Maximum number of fragments")
    tcpipIPv4FragMaxNum.setVisible(True)
    tcpipIPv4FragMaxNum.setDescription("Maximum limit for number of fragments")
    tcpipIPv4FragMaxNum.setDefaultValue(4)

    # Enable External Packet Processing
    tcpipIpv4ExtPktProcess = tcpipIPv4Component.createBooleanSymbol("TCPIP_IPV4_EXTERN_PACKET_PROCESS", tcpipIPv4AdvSettings)
    tcpipIpv4ExtPktProcess.setHelp("mcc_h3_ipv4_configurations")
    tcpipIpv4ExtPktProcess.setLabel("Enable External Packet Processing")
    tcpipIpv4ExtPktProcess.setVisible(True)
    tcpipIpv4ExtPktProcess.setDescription("Allows External Processing of RX Packets")
    tcpipIpv4ExtPktProcess.setDefaultValue(False)
    
    # IPv4 Task Tick Rate in ms
    tcpipIPv4TaskTickRate = tcpipIPv4Component.createIntegerSymbol("TCPIP_IPV4_TASK_TICK_RATE", tcpipIPv4FragSetting)
    tcpipIPv4TaskTickRate.setHelp("mcc_h3_ipv4_configurations")
    tcpipIPv4TaskTickRate.setLabel("IPv4 Fragmentation Task Rate (in msec)")
    tcpipIPv4TaskTickRate.setVisible(True)
    tcpipIPv4TaskTickRate.setDescription("IPv4 task rate (in msec)")
    tcpipIPv4TaskTickRate.setDefaultValue(37)

    # IPv4 Forwarding Support
    tcpipIPv4Forward = tcpipIPv4Component.createBooleanSymbol("TCPIP_IPV4_FORWARD", tcpipIPv4AdvSettings)
    tcpipIPv4Forward.setHelp("mcc_h3_ipv4_configurations")
    tcpipIPv4Forward.setLabel("Enable IPv4 Forwarding Support")
    tcpipIPv4Forward.setVisible(True)
    tcpipIPv4Forward.setDescription("Enable IPv4 Forwarding Support")
    tcpipIPv4Forward.setDefaultValue(False)
    #tcpipIPv4Frag.setDependencies(tcpipIPv4MenuVisible, ["TCPIP_STACK_USE_IPV4"])

    # Settings for IPv4 Forwarding
    tcpipIPv4FwdSetting = tcpipIPv4Component.createMenuSymbol("TCPIP_IPV4_FORWARD_SETTING", tcpipIPv4Forward)
    tcpipIPv4FwdSetting.setLabel("IPv4 Forwarding Settings")
    tcpipIPv4FwdSetting.setVisible(False)
    tcpipIPv4FwdSetting.setDescription("IPv4 Forwarding Settings")
    tcpipIPv4FwdSetting.setDependencies(tcpipIPv4MenuVisible, ["TCPIP_IPV4_FORWARD"])

    # Enable initialization with ASCII strings
    tcpipIPv4FwdAsciiEnable = tcpipIPv4Component.createBooleanSymbol("TCPIP_IPV4_FORWARD_ASCII_ENABLE", tcpipIPv4FwdSetting)
    tcpipIPv4FwdAsciiEnable.setHelp("mcc_h3_ipv4_configurations")
    tcpipIPv4FwdAsciiEnable.setLabel("Support Forwarding Table with ASCII strings")
    tcpipIPv4FwdAsciiEnable.setVisible(True)
    tcpipIPv4FwdAsciiEnable.setDescription("Support Forwarding Table initialization with ASCII strings")
    tcpipIPv4FwdAsciiEnable.setDefaultValue(True)

    # Enable initialization with Binary values
    tcpipIPv4FwdDynEnable = tcpipIPv4Component.createBooleanSymbol("TCPIP_IPV4_FORWARD_DYN_ENABLE", tcpipIPv4FwdSetting)
    tcpipIPv4FwdDynEnable.setHelp("mcc_h3_ipv4_configurations")
    tcpipIPv4FwdDynEnable.setLabel("Support Dynamic Forwarding Table Manipulation")
    tcpipIPv4FwdDynEnable.setVisible(True)
    tcpipIPv4FwdDynEnable.setDescription("Support Dynamic Forwarding Table API")
    tcpipIPv4FwdDynEnable.setDefaultValue(True)

    # Enable forwarding of bcast packets
    tcpipIPv4FwdBcastEnable = tcpipIPv4Component.createBooleanSymbol("TCPIP_IPV4_FORWARD_BCAST_ENABLE", tcpipIPv4FwdSetting)
    tcpipIPv4FwdBcastEnable.setHelp("mcc_h3_ipv4_configurations")
    tcpipIPv4FwdBcastEnable.setLabel("Enable Broadcast Traffic Forwarding")
    tcpipIPv4FwdBcastEnable.setVisible(True)
    tcpipIPv4FwdBcastEnable.setDescription("Enable Forwarding Broadcast Packets on the Forwarding Interface")
    tcpipIPv4FwdBcastEnable.setDefaultValue(False)

    # Enable forwarding statistics
    tcpipIPv4FwdStatsEnable = tcpipIPv4Component.createBooleanSymbol("TCPIP_IPV4_FORWARD_STATS_ENABLE", tcpipIPv4FwdSetting)
    tcpipIPv4FwdStatsEnable.setHelp("mcc_h3_ipv4_configurations")
    tcpipIPv4FwdStatsEnable.setLabel("Enable Forwarding Statistics")
    tcpipIPv4FwdStatsEnable.setVisible(True)
    tcpipIPv4FwdStatsEnable.setDescription("Enable Forwarding Statistics Gathering on the Forwarding Interfaces")
    tcpipIPv4FwdStatsEnable.setDefaultValue(False)

    # Forwarding queue slots
    tcpipIPv4FwdSlots = tcpipIPv4Component.createIntegerSymbol("TCPIP_IPV4_FWD_QUEUE_SLOTS", tcpipIPv4FwdSetting)
    tcpipIPv4FwdSlots.setHelp("mcc_h3_ipv4_configurations")
    tcpipIPv4FwdSlots.setLabel("Forwarding TX Queue Slots")
    tcpipIPv4FwdSlots.setVisible(True)
    tcpipIPv4FwdSlots.setDescription("Number of Multicast and Broadcast Packets that IPv4 Forwarding Can Queue Up")
    tcpipIPv4FwdSlots.setDefaultValue(0)

    # Max number of entries in the forwarding table
    tcpipIPv4FwdTblMaxSize = tcpipIPv4Component.createIntegerSymbol("TCPIP_IPV4_FORWARD_TABLE_MAX_SIZE", tcpipIPv4FwdSetting)
    tcpipIPv4FwdTblMaxSize.setHelp("mcc_h3_ipv4_configurations")
    tcpipIPv4FwdTblMaxSize.setLabel("Forwarding Table Maximum Size")
    tcpipIPv4FwdTblMaxSize.setVisible(True)
    tcpipIPv4FwdTblMaxSize.setDescription("Maximum Number of Entries in the Forwarding Table")
    tcpipIPv4FwdTblMaxSize.setMin(tcpipIPv4FwdEntriesMin)
    tcpipIPv4FwdTblMaxSize.setMax(tcpipIPv4FwdEntriesMax)
    tcpipIPv4FwdTblMaxSize.setDefaultValue(tcpipIPv4FwdEntriesMax)
    tcpipIPv4FwdTblMaxSize.setReadOnly(True)

    # Select ASCII/Binary entries
    tcpipIPv4FwdAsciiSelect = tcpipIPv4Component.createBooleanSymbol("TCPIP_IPV4_FORWARD_ASCII_SELECT", tcpipIPv4FwdSetting)
    tcpipIPv4FwdAsciiSelect.setHelp("mcc_h3_ipv4_configurations")
    tcpipIPv4FwdAsciiSelect.setLabel("Use ASCII Init Forwarding Table")
    tcpipIPv4FwdAsciiSelect.setVisible(True)
    tcpipIPv4FwdAsciiSelect.setDescription("Use ASCII Values for the Initialization Forwarding Table")
    tcpipIPv4FwdAsciiSelect.setDefaultValue(True)
    tcpipIPv4FwdAsciiSelect.setReadOnly(True)
    
    # Number of forwarding table entries
    tcpipIPv4FwdTableNum = tcpipIPv4Component.createIntegerSymbol("TCPIP_IPV4_FORWARD_TABLE_CNT", tcpipIPv4FwdSetting)
    tcpipIPv4FwdTableNum.setHelp("mcc_h3_ipv4_configurations")
    tcpipIPv4FwdTableNum.setLabel("Number of Entries in the Init Forwarding table")
    tcpipIPv4FwdTableNum.setVisible(True)
    tcpipIPv4FwdTableNum.setDescription("Current Entries in the Initialization Forwarding table")
    tcpipIPv4FwdTableNum.setDefaultValue(0)
    tcpipIPv4FwdTableNum.setMax(tcpipIPv4FwdEntriesMax)
    tcpipIPv4FwdTableNum.setMin(0)
    tcpipIPv4FwdTableNum.setDependencies(tcpipIPv4FwdTableEn, ["TCPIP_IPV4_FORWARD_TABLE_CNT"]) 
    #####################################################################################################
    for index in range(0, tcpipIPv4FwdEntriesMax): 
        tcpipIPv4FwdEntry.append(tcpipIPv4Component.createMenuSymbol("TCPIP_IPV4_FWD_ENTRY_NUMBERX" + str(index), tcpipIPv4FwdSetting))
        tcpipIPv4FwdEntry[index].setLabel("Forwarding Entry " + str(index))
        tcpipIPv4FwdEntry[index].setVisible(False)
        
        tcpipIPv4FwdBool.append(tcpipIPv4Component.createBooleanSymbol("TCPIP_IPV4_FWD_ENTRY_BOOLX" + str(index), tcpipIPv4FwdSetting))
        tcpipIPv4FwdBool[index].setLabel("Forwarding Entry " + str(index))
        tcpipIPv4FwdBool[index].setVisible(False)
        tcpipIPv4FwdBool[index].setDefaultValue(False)
        
        # Net Address
        tcpipIPv4FwdEntryNetAdd.append(tcpipIPv4Component.createStringSymbol("TCPIP_IPV4_FWD_ENTRY_NET_ADD_IDX" + str(index), tcpipIPv4FwdEntry[index]))
        tcpipIPv4FwdEntryNetAdd[index].setHelp("mcc_h3_ipv4_configurations")
        tcpipIPv4FwdEntryNetAdd[index].setLabel("Network Destination Address")
        tcpipIPv4FwdEntryNetAdd[index].setVisible(False)
        tcpipIPv4FwdEntryNetAdd[index].setDefaultValue("0.0.0.0")
        tcpipIPv4FwdEntryNetAdd[index].setDependencies(tcpipIPv4MenuVisible, [tcpipIPv4FwdBool[index].getID()])

        # Net Mask
        tcpipIPv4FwdEntryNetMask.append(tcpipIPv4Component.createStringSymbol("TCPIP_IPV4_FWD_ENTRY_NET_MASK_IDX" + str(index), tcpipIPv4FwdEntry[index]))
        tcpipIPv4FwdEntryNetMask[index].setHelp("mcc_h3_ipv4_configurations")
        tcpipIPv4FwdEntryNetMask[index].setLabel("Network Mask")
        tcpipIPv4FwdEntryNetMask[index].setVisible(False)
        tcpipIPv4FwdEntryNetMask[index].setDefaultValue("0.0.0.0")
        tcpipIPv4FwdEntryNetMask[index].setDependencies(tcpipIPv4MenuVisible, [tcpipIPv4FwdBool[index].getID()])

        # Gw Address
        tcpipIPv4FwdEntryGwAdd.append(tcpipIPv4Component.createStringSymbol("TCPIP_IPV4_FWD_ENTRY_GW_ADD_IDX" + str(index), tcpipIPv4FwdEntry[index]))
        tcpipIPv4FwdEntryGwAdd[index].setHelp("mcc_h3_ipv4_configurations")
        tcpipIPv4FwdEntryGwAdd[index].setLabel("Gateway Destination Address")
        tcpipIPv4FwdEntryGwAdd[index].setVisible(False)
        tcpipIPv4FwdEntryGwAdd[index].setDefaultValue("0.0.0.0")
        tcpipIPv4FwdEntryGwAdd[index].setDependencies(tcpipIPv4MenuVisible, [tcpipIPv4FwdBool[index].getID()])

        # Source Interface name
        tcpipIPv4FwdEntrySrcIfName.append(tcpipIPv4Component.createStringSymbol("TCPIP_IPV4_FWD_ENTRY_SRC_IF_NAME_IDX" + str(index), tcpipIPv4FwdEntry[index]))
        tcpipIPv4FwdEntrySrcIfName[index].setHelp("mcc_h3_ipv4_configurations")
        tcpipIPv4FwdEntrySrcIfName[index].setLabel("Source Interface Name")
        tcpipIPv4FwdEntrySrcIfName[index].setVisible(False)
        tcpipIPv4FwdEntrySrcIfName[index].setDefaultValue("")
        tcpipIPv4FwdEntrySrcIfName[index].setDependencies(tcpipIPv4MenuVisible, [tcpipIPv4FwdBool[index].getID()])

        # Destination Interface name
        tcpipIPv4FwdEntryDestIfName.append(tcpipIPv4Component.createStringSymbol("TCPIP_IPV4_FWD_ENTRY_DEST_IF_NAME_IDX" + str(index), tcpipIPv4FwdEntry[index]))
        tcpipIPv4FwdEntryDestIfName[index].setHelp("mcc_h3_ipv4_configurations")
        tcpipIPv4FwdEntryDestIfName[index].setLabel("Destination Interface Name")
        tcpipIPv4FwdEntryDestIfName[index].setVisible(False)
        tcpipIPv4FwdEntryDestIfName[index].setDefaultValue("")
        tcpipIPv4FwdEntryDestIfName[index].setDependencies(tcpipIPv4MenuVisible, [tcpipIPv4FwdBool[index].getID()])


        # Metric
        tcpipIPv4FwdEntryMetric.append(tcpipIPv4Component.createIntegerSymbol("TCPIP_IPV4_FWD_ENTRY_METRIC_IDX" + str(index), tcpipIPv4FwdEntry[index]))
        tcpipIPv4FwdEntryMetric[index].setHelp("mcc_h3_ipv4_configurations")
        tcpipIPv4FwdEntryMetric[index].setLabel("Entry Metric Value")
        tcpipIPv4FwdEntryMetric[index].setVisible(False)
        tcpipIPv4FwdEntryMetric[index].setDefaultValue(10)
        tcpipIPv4FwdEntryMetric[index].setMin(0)
        tcpipIPv4FwdEntryMetric[index].setMax(255)
        tcpipIPv4FwdEntryMetric[index].setDependencies(tcpipIPv4MenuVisible, [tcpipIPv4FwdBool[index].getID()])

    #####################################################################################################
    #Add to system_config.h
    tcpipIPv4HeaderFtl = tcpipIPv4Component.createFileSymbol(None, None)
    tcpipIPv4HeaderFtl.setSourcePath("tcpip/config/ip.h.ftl")
    tcpipIPv4HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipIPv4HeaderFtl.setMarkup(True)
    tcpipIPv4HeaderFtl.setType("STRING")

    # Add ipv4.c file
    tcpipIPv4SourceFile = tcpipIPv4Component.createFileSymbol(None, None)
    tcpipIPv4SourceFile.setSourcePath("tcpip/src/ipv4.c")
    tcpipIPv4SourceFile.setOutputName("ipv4.c")
    tcpipIPv4SourceFile.setOverwrite(True)
    tcpipIPv4SourceFile.setDestPath("library/tcpip/src/")
    tcpipIPv4SourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipIPv4SourceFile.setType("SOURCE")
    tcpipIPv4SourceFile.setEnabled(True)
    #tcpipIPv4SourceFile.setDependencies(tcpipIpv4GenSourceFile, ["TCPIP_STACK_USE_IPV4"])
    

def tcpipIPv4MenuVisible(symbol, event):
    if (event["value"] == True):
        print("TCPIP Menu Visible.")        
        symbol.setVisible(True)
    else:
        print("TCPIP Menu Invisible.")
        symbol.setVisible(False)
        
def tcpipIpv4GenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])


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
    Database.setSymbolValue("tcpipIPv4", "TCPIP_STACK_USE_IPV4", False, 2)

def tcpipIPv4FwdEntryInstance(symbol, event):
    print("In tcpipIPv4FwdEntryInstance")
    print("symbol", symbol)
    print("event:", event)


def tcpipIPv4FwdTableEn(symbol, event):
    global tcpipIPv4FwdPrev_cnt
    count  = event["value"]
    symCom = symbol.getComponent()

    if(count > tcpipIPv4FwdPrev_cnt):
        while (tcpipIPv4FwdPrev_cnt < count):            
            symId = symCom.getSymbolByID("TCPIP_IPV4_FWD_ENTRY_NUMBERX" + str(tcpipIPv4FwdPrev_cnt))             
            symId.setVisible(True)            
            symId = symCom.getSymbolByID("TCPIP_IPV4_FWD_ENTRY_BOOLX" + str(tcpipIPv4FwdPrev_cnt))             
            symId.setValue(True)
            tcpipIPv4FwdPrev_cnt = tcpipIPv4FwdPrev_cnt + 1
    else:
        while (tcpipIPv4FwdPrev_cnt > count):
            symId = symCom.getSymbolByID("TCPIP_IPV4_FWD_ENTRY_NUMBERX" + str(tcpipIPv4FwdPrev_cnt - 1))
            symId.setVisible(False)        
            symId = symCom.getSymbolByID("TCPIP_IPV4_FWD_ENTRY_BOOLX" + str(tcpipIPv4FwdPrev_cnt - 1)) 
            symId.setValue(False)
            tcpipIPv4FwdPrev_cnt = tcpipIPv4FwdPrev_cnt - 1

