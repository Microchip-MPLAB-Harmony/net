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

    
def instantiateComponent(tcpipIcmpComponent):
    print("TCPIP ICMP Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    # ICMPv4 Client and Server
    tcpipIcmp = tcpipIcmpComponent.createBooleanSymbol("TCPIP_STACK_USE_ICMPV4", None)
    tcpipIcmp.setLabel("ICMPv4 Client and Server")
    tcpipIcmp.setVisible(False)
    tcpipIcmp.setDescription("ICMPv4 Client and Server")
    tcpipIcmp.setDefaultValue(True)

    # Use ICMPv4 Server
    tcpipIcmpv4Server = tcpipIcmpComponent.createBooleanSymbol("TCPIP_STACK_USE_ICMP_SERVER", None)
    tcpipIcmpv4Server.setLabel("Use ICMPv4 Server")
    tcpipIcmpv4Server.setVisible(True)
    tcpipIcmpv4Server.setDescription("Use ICMPv4 Server")
    tcpipIcmpv4Server.setDefaultValue(True) 
    
    # Allow Replies to Echo Broadcast Requests
    tcpipIcmpEchoBroadcast = tcpipIcmpComponent.createBooleanSymbol("TCPIP_ICMP_ECHO_BROADCASTS", tcpipIcmpv4Server)
    tcpipIcmpEchoBroadcast.setLabel("Allow Replies to Echo Broadcast Requests")
    tcpipIcmpEchoBroadcast.setVisible(True)
    tcpipIcmpEchoBroadcast.setDescription("Allow Replies to Echo Broadcast Requests")
    tcpipIcmpEchoBroadcast.setDefaultValue(False)
    tcpipIcmpEchoBroadcast.setDependencies(tcpipIcmpMenuVisible, ["TCPIP_STACK_USE_ICMP_SERVER"])
    
    # Use ICMPv4 Client
    tcpipIcmpv4Client = tcpipIcmpComponent.createBooleanSymbol("TCPIP_STACK_USE_ICMP_CLIENT", None)
    tcpipIcmpv4Client.setLabel("Use ICMPv4 Client")
    tcpipIcmpv4Client.setVisible(True)
    tcpipIcmpv4Client.setDescription("Use ICMPv4 Client")
    tcpipIcmpv4Client.setDefaultValue(False)

    # ICMP task rate in ms
    tcpipIcmpTskTickRate = tcpipIcmpComponent.createIntegerSymbol("TCPIP_ICMP_TASK_TICK_RATE", tcpipIcmpv4Client)
    tcpipIcmpTskTickRate.setLabel("ICMP Client Task Rate (in msec)")
    tcpipIcmpTskTickRate.setVisible(False)
    tcpipIcmpTskTickRate.setDescription("ICMP task rate in ms")
    tcpipIcmpTskTickRate.setDefaultValue(33)
    tcpipIcmpTskTickRate.setDependencies(tcpipIcmpMenuVisible, ["TCPIP_STACK_USE_ICMP_CLIENT"])    

    # Enable ICMP Commands
    tcpipIcmpCommandEnable = tcpipIcmpComponent.createBooleanSymbol("TCPIP_ICMP_COMMAND_ENABLE", tcpipIcmpv4Client)
    tcpipIcmpCommandEnable.setLabel("Enable ICMP Client Console Commands")
    tcpipIcmpCommandEnable.setVisible(False)
    tcpipIcmpCommandEnable.setDescription("Enable ICMP Client Console Commands")
    tcpipIcmpCommandEnable.setDefaultValue(False)
    tcpipIcmpCommandEnable.setDependencies(tcpipIcmpMenuVisible, ["TCPIP_STACK_USE_ICMP_CLIENT"])
    
    # Number of ICMP Echo requests
    tcpipIcmpEchoReqNum = tcpipIcmpComponent.createIntegerSymbol("TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUESTS", tcpipIcmpCommandEnable)
    tcpipIcmpEchoReqNum.setLabel("Number of ICMP Echo requests")
    tcpipIcmpEchoReqNum.setVisible(False)
    tcpipIcmpEchoReqNum.setDescription("Number of ICMP Echo requests")
    tcpipIcmpEchoReqNum.setDefaultValue(4)
    tcpipIcmpEchoReqNum.setDependencies(tcpipIcmpMenuVisible, ["TCPIP_ICMP_COMMAND_ENABLE"])

    # ICMP Reply Time-out in ms
    tcpipIcmpEchoReqDelay = tcpipIcmpComponent.createIntegerSymbol("TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DELAY", tcpipIcmpCommandEnable)
    tcpipIcmpEchoReqDelay.setLabel("ICMP Reply Time-out (in msec)")
    tcpipIcmpEchoReqDelay.setVisible(False)
    tcpipIcmpEchoReqDelay.setDescription("ICMP Reply Time-out in ms")
    tcpipIcmpEchoReqDelay.setDefaultValue(1000)
    tcpipIcmpEchoReqDelay.setDependencies(tcpipIcmpMenuVisible, ["TCPIP_ICMP_COMMAND_ENABLE"])

    # ICMP Give Up Time-out in ms
    tcpipIcmpEchoTimeout= tcpipIcmpComponent.createIntegerSymbol("TCPIP_STACK_COMMANDS_ICMP_ECHO_TIMEOUT", tcpipIcmpCommandEnable)
    tcpipIcmpEchoTimeout.setLabel("ICMP Give Up Time-out (in msec)")
    tcpipIcmpEchoTimeout.setVisible(False)
    tcpipIcmpEchoTimeout.setDescription("ICMP Give Up Time-out in ms")
    tcpipIcmpEchoTimeout.setDefaultValue(5000)
    tcpipIcmpEchoTimeout.setDependencies(tcpipIcmpMenuVisible, ["TCPIP_ICMP_COMMAND_ENABLE"])

    # Echo Request Data Buffer size - bytes
    tcpipIcmpEchoReqBuffSize= tcpipIcmpComponent.createIntegerSymbol("TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_BUFF_SIZE", tcpipIcmpCommandEnable)
    tcpipIcmpEchoReqBuffSize.setLabel("Echo Request Data Buffer size - bytes")
    tcpipIcmpEchoReqBuffSize.setVisible(False)
    tcpipIcmpEchoReqBuffSize.setDescription("Echo Request Data Buffer size - bytes")
    tcpipIcmpEchoReqBuffSize.setDefaultValue(2000)
    tcpipIcmpEchoReqBuffSize.setDependencies(tcpipIcmpTCPEnable, ["tcpipTcp.TCPIP_USE_TCP", "TCPIP_ICMP_COMMAND_ENABLE"])

    # Echo request Default Data Size - bytes
    tcpipIcmpEchoReqDataSize= tcpipIcmpComponent.createIntegerSymbol("TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DATA_SIZE", tcpipIcmpCommandEnable)
    tcpipIcmpEchoReqDataSize.setLabel("Echo request Default Data Size - bytes")
    tcpipIcmpEchoReqDataSize.setVisible(False)
    tcpipIcmpEchoReqDataSize.setDescription("Echo request Default Data Size - bytes")
    tcpipIcmpEchoReqDataSize.setDefaultValue(100)
    tcpipIcmpEchoReqDataSize.setDependencies(tcpipIcmpTCPEnable, ["tcpipTcp.TCPIP_USE_TCP", "TCPIP_ICMP_COMMAND_ENABLE"])

    # Enable User Notification
    tcpipIcmpClientUsrNotify = tcpipIcmpComponent.createBooleanSymbol("TCPIP_ICMP_CLIENT_USER_NOTIFICATION", tcpipIcmpv4Client)
    tcpipIcmpClientUsrNotify.setLabel("Enable User Notification")
    tcpipIcmpClientUsrNotify.setVisible(False)
    tcpipIcmpClientUsrNotify.setDescription("Enable User Notification")
    tcpipIcmpClientUsrNotify.setDefaultValue(True)
    tcpipIcmpClientUsrNotify.setDependencies(tcpipIcmpMenuVisible, ["TCPIP_STACK_USE_ICMP_CLIENT"])

    # Echo request timeout in ms
    tcpipIcmpEchoReqTimeout = tcpipIcmpComponent.createIntegerSymbol("TCPIP_ICMP_ECHO_REQUEST_TIMEOUT", tcpipIcmpv4Client)
    tcpipIcmpEchoReqTimeout.setLabel("Echo request timeout (in msec)")
    tcpipIcmpEchoReqTimeout.setVisible(False)
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
    if(tcpipIcmpClient or tcpipIcmpServer ):
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