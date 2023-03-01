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

    
def instantiateComponent(tcpipIcmpComponent):
    print("TCPIP ICMP Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    # ICMPv4 Client and Server
    tcpipIcmp = tcpipIcmpComponent.createBooleanSymbol("TCPIP_STACK_USE_ICMPV4", None)
    tcpipIcmp.setHelp("mcc_h3_icmp_configurations")
    tcpipIcmp.setLabel("ICMPv4 Client and Server")
    tcpipIcmp.setVisible(False)
    tcpipIcmp.setDescription("ICMPv4 Client and Server")
    tcpipIcmp.setDefaultValue(True)

    # Use ICMPv4 Server
    tcpipIcmpv4Server = tcpipIcmpComponent.createBooleanSymbol("TCPIP_STACK_USE_ICMP_SERVER", None)
    tcpipIcmpv4Server.setHelp("mcc_h3_icmp_configurations")
    tcpipIcmpv4Server.setLabel("Use ICMPv4 Server")
    tcpipIcmpv4Server.setVisible(True)
    tcpipIcmpv4Server.setDescription("Use ICMPv4 Server")
    tcpipIcmpv4Server.setDefaultValue(True) 
    
    # Allow Replies to Echo Broadcast Requests
    tcpipIcmpEchoBroadcast = tcpipIcmpComponent.createBooleanSymbol("TCPIP_ICMP_ECHO_BROADCASTS", tcpipIcmpv4Server)
    tcpipIcmpEchoBroadcast.setHelp("mcc_h3_icmp_configurations")
    tcpipIcmpEchoBroadcast.setLabel("Allow Replies to Echo Broadcast Requests")
    tcpipIcmpEchoBroadcast.setVisible(True)
    tcpipIcmpEchoBroadcast.setDescription("Allow Replies to Echo Broadcast Requests")
    tcpipIcmpEchoBroadcast.setDefaultValue(False)
    tcpipIcmpEchoBroadcast.setDependencies(tcpipIcmpMenuVisible, ["TCPIP_STACK_USE_ICMP_SERVER"])
    
    # Use ICMPv4 Client
    tcpipIcmpv4Client = tcpipIcmpComponent.createBooleanSymbol("TCPIP_STACK_USE_ICMP_CLIENT", None)
    tcpipIcmpv4Client.setHelp("mcc_h3_icmp_configurations")
    tcpipIcmpv4Client.setLabel("Use ICMPv4 Client")
    tcpipIcmpv4Client.setVisible(True)
    tcpipIcmpv4Client.setDescription("Use ICMPv4 Client")
    tcpipDhcpsComp = Database.getComponentByID("tcpipDhcps")
    tcpipDhcpServerV2Comp = Database.getComponentByID("tcpipDhcpServer")
    if (tcpipDhcpsComp != None) or (tcpipDhcpServerV2Comp != None):
        dfltVal = True
    else:
        dfltVal = False
    tcpipIcmpv4Client.setDefaultValue(dfltVal)

    # ICMP task rate in ms
    tcpipIcmpTskTickRate = tcpipIcmpComponent.createIntegerSymbol("TCPIP_ICMP_TASK_TICK_RATE", tcpipIcmpv4Client)
    tcpipIcmpTskTickRate.setHelp("mcc_h3_icmp_configurations")
    tcpipIcmpTskTickRate.setLabel("ICMP Client Task Rate (in msec)")
    tcpipIcmpTskTickRate.setVisible(dfltVal)
    tcpipIcmpTskTickRate.setDescription("ICMP task rate in ms")
    tcpipIcmpTskTickRate.setDefaultValue(33)
    tcpipIcmpTskTickRate.setDependencies(tcpipIcmpMenuVisible, ["TCPIP_STACK_USE_ICMP_CLIENT"])    

    # Maximum number of parallel outgoing ICMP Echo requests
    tcpipIcmpMaxClientReqNum = tcpipIcmpComponent.createIntegerSymbol("TCPIP_STACK_MAX_CLIENT_ECHO_REQUESTS", tcpipIcmpv4Client)
    tcpipIcmpMaxClientReqNum.setHelp("mcc_h3_icmp_configurations")
    tcpipIcmpMaxClientReqNum.setLabel("Maximum Number of Supported Client Requests")
    tcpipIcmpMaxClientReqNum.setVisible(dfltVal)
    tcpipIcmpMaxClientReqNum.setDescription("Maximum Number of Parallel ICMP Echo requests")
    tcpipIcmpMaxClientReqNum.setDefaultValue(4)
    tcpipIcmpMaxClientReqNum.setDependencies(tcpipIcmpMenuVisible, ["TCPIP_STACK_USE_ICMP_CLIENT"])

    # Enable ICMP Commands
    tcpipIcmpCommandEnable = tcpipIcmpComponent.createBooleanSymbol("TCPIP_ICMP_COMMAND_ENABLE", tcpipIcmpv4Client)
    tcpipIcmpCommandEnable.setHelp("mcc_h3_icmp_configurations")
    tcpipIcmpCommandEnable.setLabel("Enable ICMP Client Console Commands")
    tcpipIcmpCommandEnable.setVisible(dfltVal)
    tcpipIcmpCommandEnable.setDescription("Enable ICMP Client Console Commands")
    tcpipIcmpCommandEnable.setDefaultValue(False)
    tcpipIcmpCommandEnable.setDependencies(tcpipIcmpMenuVisible, ["TCPIP_STACK_USE_ICMP_CLIENT"])
    
    # Number of ICMP Echo requests tp send
    tcpipIcmpEchoReqNum = tcpipIcmpComponent.createIntegerSymbol("TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUESTS", tcpipIcmpCommandEnable)
    tcpipIcmpEchoReqNum.setHelp("mcc_h3_icmp_configurations")
    tcpipIcmpEchoReqNum.setLabel("Number of ICMP Echo requests")
    tcpipIcmpEchoReqNum.setVisible(False)
    tcpipIcmpEchoReqNum.setDescription("Number of ICMP Echo requests")
    tcpipIcmpEchoReqNum.setDefaultValue(4)
    tcpipIcmpEchoReqNum.setDependencies(tcpipIcmpMenuVisible, ["TCPIP_ICMP_COMMAND_ENABLE"])

    # ICMP Reply Time-out in ms
    tcpipIcmpEchoReqDelay = tcpipIcmpComponent.createIntegerSymbol("TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DELAY", tcpipIcmpCommandEnable)
    tcpipIcmpEchoReqDelay.setHelp("mcc_h3_icmp_configurations")
    tcpipIcmpEchoReqDelay.setLabel("ICMP Request Delay (in msec)")
    tcpipIcmpEchoReqDelay.setVisible(False)
    tcpipIcmpEchoReqDelay.setDescription("ICMP Request delay in ms")
    tcpipIcmpEchoReqDelay.setDefaultValue(1000)
    tcpipIcmpEchoReqDelay.setDependencies(tcpipIcmpMenuVisible, ["TCPIP_ICMP_COMMAND_ENABLE"])

    # ICMP Give Up Time-out in ms
    tcpipIcmpEchoTimeout= tcpipIcmpComponent.createIntegerSymbol("TCPIP_STACK_COMMANDS_ICMP_ECHO_TIMEOUT", tcpipIcmpCommandEnable)
    tcpipIcmpEchoTimeout.setHelp("mcc_h3_icmp_configurations")
    tcpipIcmpEchoTimeout.setLabel("ICMP Give Up Time-out (in msec)")
    tcpipIcmpEchoTimeout.setVisible(False)
    tcpipIcmpEchoTimeout.setDescription("ICMP Give Up Time-out in ms")
    tcpipIcmpEchoTimeout.setDefaultValue(5000)
    tcpipIcmpEchoTimeout.setDependencies(tcpipIcmpMenuVisible, ["TCPIP_ICMP_COMMAND_ENABLE"])

    # Echo Request Data Buffer size - bytes
    tcpipIcmpEchoReqBuffSize= tcpipIcmpComponent.createIntegerSymbol("TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_BUFF_SIZE", tcpipIcmpCommandEnable)
    tcpipIcmpEchoReqBuffSize.setHelp("mcc_h3_icmp_configurations")
    tcpipIcmpEchoReqBuffSize.setLabel("Echo Request Data Buffer size - bytes")
    tcpipIcmpEchoReqBuffSize.setVisible(False)
    tcpipIcmpEchoReqBuffSize.setDescription("Echo Request Data Buffer size - bytes")
    tcpipIcmpEchoReqBuffSize.setDefaultValue(2000)
    tcpipIcmpEchoReqBuffSize.setDependencies(tcpipIcmpTCPEnable, ["tcpipTcp.TCPIP_USE_TCP", "TCPIP_ICMP_COMMAND_ENABLE"])

    # Echo request Default Data Size - bytes
    tcpipIcmpEchoReqDataSize= tcpipIcmpComponent.createIntegerSymbol("TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DATA_SIZE", tcpipIcmpCommandEnable)
    tcpipIcmpEchoReqDataSize.setHelp("mcc_h3_icmp_configurations")
    tcpipIcmpEchoReqDataSize.setLabel("Echo request Default Data Size - bytes")
    tcpipIcmpEchoReqDataSize.setVisible(False)
    tcpipIcmpEchoReqDataSize.setDescription("Echo request Default Data Size - bytes")
    tcpipIcmpEchoReqDataSize.setDefaultValue(100)
    tcpipIcmpEchoReqDataSize.setDependencies(tcpipIcmpTCPEnable, ["tcpipTcp.TCPIP_USE_TCP", "TCPIP_ICMP_COMMAND_ENABLE"])

    # Echo request timeout in ms
    tcpipIcmpEchoReqTimeout = tcpipIcmpComponent.createIntegerSymbol("TCPIP_ICMP_ECHO_REQUEST_TIMEOUT", tcpipIcmpv4Client)
    tcpipIcmpEchoReqTimeout.setHelp("mcc_h3_icmp_configurations")
    tcpipIcmpEchoReqTimeout.setLabel("Echo request timeout (in msec)")
    tcpipIcmpEchoReqTimeout.setVisible(dfltVal)
    tcpipIcmpEchoReqTimeout.setDescription("Echo request timeout in ms")
    tcpipIcmpEchoReqTimeout.setDefaultValue(500)
    tcpipIcmpEchoReqTimeout.setDependencies(tcpipIcmpMenuVisible, ["TCPIP_STACK_USE_ICMP_CLIENT"])
    
    #Add to system_config.h
    tcpipIcmpHeaderFtl = tcpipIcmpComponent.createFileSymbol(None, None)
    tcpipIcmpHeaderFtl.setSourcePath("tcpip/config/icmp.h.ftl")
    tcpipIcmpHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipIcmpHeaderFtl.setMarkup(True)
    tcpipIcmpHeaderFtl.setType("STRING")

    # Add icmp.c file
    tcpipIcmpSourceFile = tcpipIcmpComponent.createFileSymbol(None, None)
    tcpipIcmpSourceFile.setSourcePath("tcpip/src/icmp.c")
    tcpipIcmpSourceFile.setOutputName("icmp.c")
    tcpipIcmpSourceFile.setOverwrite(True)
    tcpipIcmpSourceFile.setDestPath("library/tcpip/src/")
    tcpipIcmpSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipIcmpSourceFile.setType("SOURCE")
    tcpipIcmpSourceFile.setEnabled(True)
    tcpipIcmpSourceFile.setDependencies(tcpipIcmpGenSourceFile, ["TCPIP_STACK_USE_ICMP_SERVER","TCPIP_STACK_USE_ICMP_CLIENT"])

def tcpipIcmpMenuVisible(symbol, event):
    symbol.setVisible(event["value"])
    
def tcpipIcmpTCPEnable(symbol, event):  
    tcpipIcmpCmdEnabled = Database.getSymbolValue("tcpipIcmp","TCPIP_ICMP_COMMAND_ENABLE")
    tcpipCmdTCPEnabled = Database.getSymbolValue("tcpipTcp", "TCPIP_USE_TCP")   
    if ((tcpipIcmpCmdEnabled == True) and (tcpipCmdTCPEnabled == True)):
        symbol.setVisible(True)     
    else:
        symbol.setVisible(False)
        
def tcpipIcmpGenSourceFile(sourceFile, event):
    tcpipIcmpClient = Database.getSymbolValue("tcpipIcmp","TCPIP_STACK_USE_ICMP_CLIENT")
    tcpipIcmpServer = Database.getSymbolValue("tcpipIcmp","TCPIP_STACK_USE_ICMP_SERVER")
    if((tcpipIcmpClient == True) or (tcpipIcmpServer == True)):
        sourceFile.setEnabled(True)
    else:
        sourceFile.setEnabled(False)
    

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
    Database.setSymbolValue("tcpipIcmp", "TCPIP_STACK_USE_ICMPV4", False, 2)
