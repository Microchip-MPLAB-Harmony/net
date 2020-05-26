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

    
def instantiateComponent(tcpipIPv4Component):
    print("TCPIP IPv4 Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # Enable IPv4   
    tcpipIPv4 = tcpipIPv4Component.createBooleanSymbol("TCPIP_STACK_USE_IPV4", None)
    tcpipIPv4.setLabel("IPv4")
    tcpipIPv4.setVisible(False)
    tcpipIPv4.setDescription("Enable IPv4")
    tcpipIPv4.setDefaultValue(True)

    # Load IPv4 Configurations
    tcpipIPv4Frag = tcpipIPv4Component.createBooleanSymbol("TCPIP_IPV4_FRAGMENTATION", None)
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
    tcpipIPv4FragTimeout.setLabel("Initial fragment timeout - seconds")
    tcpipIPv4FragTimeout.setVisible(True)
    tcpipIPv4FragTimeout.setDescription("Initial fragment timeout in seconds")
    tcpipIPv4FragTimeout.setDefaultValue(15)

    # Limit for the number of fragmented streams
    tcpipIPv4FragStreamMaxNum = tcpipIPv4Component.createIntegerSymbol("TCPIP_IPV4_FRAGMENT_MAX_STREAMS", tcpipIPv4FragSetting)
    tcpipIPv4FragStreamMaxNum.setLabel("Maximum number of fragmented streams")
    tcpipIPv4FragStreamMaxNum.setVisible(True)
    tcpipIPv4FragStreamMaxNum.setDescription("Maximum limit for number of fragmented streams")
    tcpipIPv4FragStreamMaxNum.setDefaultValue(3)

    # Limit for the number of fragments
    tcpipIPv4FragMaxNum = tcpipIPv4Component.createIntegerSymbol("TCPIP_IPV4_FRAGMENT_MAX_NUMBER", tcpipIPv4FragSetting)
    tcpipIPv4FragMaxNum.setLabel("Maximum number of fragments")
    tcpipIPv4FragMaxNum.setVisible(True)
    tcpipIPv4FragMaxNum.setDescription("Maximum limit for number of fragments")
    tcpipIPv4FragMaxNum.setDefaultValue(4)

    # Enable External Packet Processing
    tcpipIpv4ExtPktProcess = tcpipIPv4Component.createBooleanSymbol("TCPIP_IPV4_EXTERN_PACKET_PROCESS", None)
    tcpipIpv4ExtPktProcess.setLabel("Enable External Packet Processing")
    tcpipIpv4ExtPktProcess.setVisible(True)
    tcpipIpv4ExtPktProcess.setDescription("Allows External Processing of RX Packets")
    tcpipIpv4ExtPktProcess.setDefaultValue(False)
    
    # IPv4 Task Tick Rate in ms
    tcpipIPv4TaskTickRate = tcpipIPv4Component.createIntegerSymbol("TCPIP_IPV4_TASK_TICK_RATE", tcpipIPv4FragSetting)
    tcpipIPv4TaskTickRate.setLabel("IPv4 task rate - ms")
    tcpipIPv4TaskTickRate.setVisible(True)
    tcpipIPv4TaskTickRate.setDescription("IPv4 task rate in ms")
    tcpipIPv4TaskTickRate.setDefaultValue(37)

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
        print "Set Symbol Failure"
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
