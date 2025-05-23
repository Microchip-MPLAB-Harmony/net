"""
Copyright (C) 2019-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

    
def instantiateComponent(tcpipIgmpComponent):
    print("TCPIP IGMP Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # Use IGMPv3 Module
    tcpipIgmp = tcpipIgmpComponent.createBooleanSymbol("TCPIP_USE_IGMP", None)
    tcpipIgmp.setHelp("mcc_h3_igmp_configurations")
    tcpipIgmp.setLabel("Use IGMPv3 Module")
    tcpipIgmp.setVisible(False)
    tcpipIgmp.setDescription("Use IGMPv3 Module")
    tcpipIgmp.setDefaultValue(True)   

    # Number of IGMP Interfaces
    tcpipIgmpInterfaceNum = tcpipIgmpComponent.createIntegerSymbol("TCPIP_IGMP_INTERFACES", None)
    tcpipIgmpInterfaceNum.setHelp("mcc_h3_igmp_configurations")
    tcpipIgmpInterfaceNum.setLabel("Number of IGMP Interfaces")
    tcpipIgmpInterfaceNum.setVisible(True)
    tcpipIgmpInterfaceNum.setDescription("Number of IGMP Interfaces")
    tcpipIgmpInterfaceNum.setDefaultValue(1)

    # Number of Multicast Groups
    tcpipIgmpMcastGrpNum = tcpipIgmpComponent.createIntegerSymbol("TCPIP_IGMP_MCAST_GROUPS", None)
    tcpipIgmpMcastGrpNum.setHelp("mcc_h3_igmp_configurations")
    tcpipIgmpMcastGrpNum.setLabel("Number of Multicast Groups")
    tcpipIgmpMcastGrpNum.setVisible(True)
    tcpipIgmpMcastGrpNum.setDescription("Number of Multicast Groups")
    tcpipIgmpMcastGrpNum.setDefaultValue(7)

    # Number of Sources in Each Group
    tcpipIgmpSourceNum = tcpipIgmpComponent.createIntegerSymbol("TCPIP_IGMP_SOURCES_PER_GROUP", None)
    tcpipIgmpSourceNum.setHelp("mcc_h3_igmp_configurations")
    tcpipIgmpSourceNum.setLabel("Number of Sources in Each Group")
    tcpipIgmpSourceNum.setVisible(True)
    tcpipIgmpSourceNum.setDescription("Number of Sources in Each Group")
    tcpipIgmpSourceNum.setDefaultValue(11)

    # Number of Sockets per Source
    tcpipIgmpSktNum = tcpipIgmpComponent.createIntegerSymbol("TCPIP_IGMP_SOCKET_RECORDS_PER_SOURCE", None)
    tcpipIgmpSktNum.setHelp("mcc_h3_igmp_configurations")
    tcpipIgmpSktNum.setLabel("Number of Socket Records per Source")
    tcpipIgmpSktNum.setVisible(True)
    tcpipIgmpSktNum.setDescription("Number of Socket Records per Source")
    tcpipIgmpSktNum.setDefaultValue(4)
    
    # Advanced Settings
    tcpipIgmpAdvSettings = tcpipIgmpComponent.createMenuSymbol("TCPIP_IGMP_ADV_SETTING", None)
    tcpipIgmpAdvSettings.setLabel("Advanced Settings")
    tcpipIgmpAdvSettings.setDescription("Advanced Settings")
    tcpipIgmpAdvSettings.setVisible(True)

    # IGMP Task Rate
    tcpipIgmpTskTickRate = tcpipIgmpComponent.createIntegerSymbol("TCPIP_IGMP_TASK_TICK_RATE", tcpipIgmpAdvSettings)
    tcpipIgmpTskTickRate.setHelp("mcc_h3_igmp_configurations")
    tcpipIgmpTskTickRate.setLabel("IGMP Task Rate (in msec)")
    tcpipIgmpTskTickRate.setVisible(True)
    tcpipIgmpTskTickRate.setDescription("IGMP Task Rate")
    tcpipIgmpTskTickRate.setDefaultValue(33)
        
    # Default Robustness Variable Value
    tcpipIgmpRobustVarValue = tcpipIgmpComponent.createIntegerSymbol("TCPIP_IGMP_ROBUSTNESS_VARIABLE", tcpipIgmpAdvSettings)
    tcpipIgmpRobustVarValue.setHelp("mcc_h3_igmp_configurations")
    tcpipIgmpRobustVarValue.setLabel("Default Robustness Variable Value")
    tcpipIgmpRobustVarValue.setVisible(True)
    tcpipIgmpRobustVarValue.setDescription("Default Robustness Variable Value")
    tcpipIgmpRobustVarValue.setDefaultValue(2)

    # Default Unsolicited Report Interval in ms
    tcpipIgmpUnsoilicitReportInterval = tcpipIgmpComponent.createIntegerSymbol("TCPIP_IGMP_UNSOLICITED_REPORT_INTERVAL", tcpipIgmpAdvSettings)
    tcpipIgmpUnsoilicitReportInterval.setHelp("mcc_h3_igmp_configurations")
    tcpipIgmpUnsoilicitReportInterval.setLabel("Default Unsolicited Report Interval (in msec)")
    tcpipIgmpUnsoilicitReportInterval.setVisible(True)
    tcpipIgmpUnsoilicitReportInterval.setDescription("Default Unsolicited Report Interval in ms")
    tcpipIgmpUnsoilicitReportInterval.setDefaultValue(1000)

    # IGMPv2 Support Only
    tcpipIgmpv2Support = tcpipIgmpComponent.createBooleanSymbol("TCPIP_IGMPV2_SUPPORT_ONLY", tcpipIgmpAdvSettings)
    tcpipIgmpv2Support.setHelp("mcc_h3_igmp_configurations")
    tcpipIgmpv2Support.setLabel("IGMPv2 Support Only")
    tcpipIgmpv2Support.setVisible(True)
    tcpipIgmpv2Support.setDescription("IGMPv2 Support Only")
    tcpipIgmpv2Support.setDefaultValue(False)   

    # Enable User Notification Functions"
    tcpipIgmpUsrNotify = tcpipIgmpComponent.createBooleanSymbol("TCPIP_IGMP_USER_NOTIFICATION", tcpipIgmpAdvSettings)
    tcpipIgmpUsrNotify.setHelp("mcc_h3_igmp_configurations")
    tcpipIgmpUsrNotify.setLabel("Enable User Notification Functions")
    tcpipIgmpUsrNotify.setVisible(True)
    tcpipIgmpUsrNotify.setDescription("Enable User Notification Functions")
    tcpipIgmpUsrNotify.setDefaultValue(False)   

    #Add to system_config.h
    tcpipIgmpHeaderFtl = tcpipIgmpComponent.createFileSymbol(None, None)
    tcpipIgmpHeaderFtl.setSourcePath("tcpip/config/igmp.h.ftl")
    tcpipIgmpHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipIgmpHeaderFtl.setMarkup(True)
    tcpipIgmpHeaderFtl.setType("STRING")

    # Add igmp.c file
    tcpipIgmpSourceFile = tcpipIgmpComponent.createFileSymbol(None, None)
    tcpipIgmpSourceFile.setSourcePath("tcpip/src/igmp.c")
    tcpipIgmpSourceFile.setOutputName("igmp.c")
    tcpipIgmpSourceFile.setOverwrite(True)
    tcpipIgmpSourceFile.setDestPath("library/tcpip/src/")
    tcpipIgmpSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipIgmpSourceFile.setType("SOURCE")
    tcpipIgmpSourceFile.setEnabled(True)
    #tcpipIgmpSourceFile.setDependencies(tcpipIgmpGenSourceFile, ["TCPIP_USE_IGMP"])

def tcpipIgmpMenuVisibleSingle(symbol, event):
    if (event["value"] == True):
        print("IGMP Menu Visible.")     
        symbol.setVisible(True)
    else:
        print("IGMP Menu Invisible.")
        symbol.setVisible(False)

def tcpipIgmpGenSourceFile(sourceFile, event):
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
    Database.setSymbolValue("tcpipIgmp", "TCPIP_USE_IGMP", False, 2)
