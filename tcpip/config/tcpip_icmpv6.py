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

    
def instantiateComponent(tcpipIcmpv6Component):
    print("TCPIP ICMPv6 Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # Use ICMPv6 Server
    tcpipIcmpv6UseServer = tcpipIcmpv6Component.createBooleanSymbol("TCPIP_STACK_USE_ICMPV6_SERVER", None)
    tcpipIcmpv6UseServer.setHelp("mcc_h3_icmpv6_configurations")
    tcpipIcmpv6UseServer.setLabel("Use ICMPv6 Server")
    tcpipIcmpv6UseServer.setVisible(True)
    tcpipIcmpv6UseServer.setDescription("Use ICMPv6 Server")
    tcpipIcmpv6UseServer.setDefaultValue(True)

    # Use ICMPv6 Client
    tcpipIcmpv6UseClient = tcpipIcmpv6Component.createBooleanSymbol("TCPIP_STACK_USE_ICMPV6_CLIENT", None)
    tcpipIcmpv6UseClient.setHelp("mcc_h3_icmpv6_configurations")
    tcpipIcmpv6UseClient.setLabel("Use ICMPv6 Client")
    tcpipIcmpv6UseClient.setVisible(True)
    tcpipIcmpv6UseClient.setDescription("Use ICMPv6 Client")
    tcpipIcmpv6UseClient.setDefaultValue(False)

    # Enable IPV6 Console Commands
    tcpipIcmpv6ClientCommand= tcpipIcmpv6Component.createBooleanSymbol("TCPIP_ICMPV6_CLIENT_CONSOLE_CMD", tcpipIcmpv6UseClient)
    tcpipIcmpv6ClientCommand.setHelp("mcc_h3_icmpv6_configurations")
    tcpipIcmpv6ClientCommand.setLabel("Enable ICMPv6 Client Console Command")
    tcpipIcmpv6ClientCommand.setVisible(False)
    tcpipIcmpv6ClientCommand.setDescription("Enable ICMPV6 CLient Console Command")
    tcpipIcmpv6ClientCommand.setDefaultValue(True)
    tcpipIcmpv6ClientCommand.setDependencies(tcpipIcmpv6MenuVisible, ["TCPIP_STACK_USE_ICMPV6_CLIENT"])
    
    # Number of ICMPv6 Echo requests send
    tcpipIcmpv6EchoReqNum = tcpipIcmpv6Component.createIntegerSymbol("TCPIP_STACK_COMMANDS_ICMPV6_ECHO_REQUESTS", tcpipIcmpv6ClientCommand)
    tcpipIcmpv6EchoReqNum.setHelp("mcc_h3_icmpv6_configurations")
    tcpipIcmpv6EchoReqNum.setLabel("Number of ICMPv6 Echo requests")
    tcpipIcmpv6EchoReqNum.setVisible(False)
    tcpipIcmpv6EchoReqNum.setDescription("Number of ICMPv6 Echo requests")
    tcpipIcmpv6EchoReqNum.setDefaultValue(4)
    tcpipIcmpv6EchoReqNum.setDependencies(tcpipIcmpv6MenuVisible, ["TCPIP_ICMPV6_CLIENT_CONSOLE_CMD"])

    # ICMPV6 Reply Time-out in ms
    tcpipIcmpv6EchoReqDelay = tcpipIcmpv6Component.createIntegerSymbol("TCPIP_STACK_COMMANDS_ICMPV6_ECHO_REQUEST_DELAY", tcpipIcmpv6ClientCommand)
    tcpipIcmpv6EchoReqDelay.setHelp("mcc_h3_icmpv6_configurations")
    tcpipIcmpv6EchoReqDelay.setLabel("ICMPv6 Request Delay (in msec)")
    tcpipIcmpv6EchoReqDelay.setVisible(False)
    tcpipIcmpv6EchoReqDelay.setDescription("ICMPv6 Request delay in ms")
    tcpipIcmpv6EchoReqDelay.setDefaultValue(1000)
    tcpipIcmpv6EchoReqDelay.setDependencies(tcpipIcmpv6MenuVisible, ["TCPIP_ICMPV6_CLIENT_CONSOLE_CMD"])
    
    # ICMPv6 Give Up Time-out in ms
    tcpipIcmpv6EchoTimeout= tcpipIcmpv6Component.createIntegerSymbol("TCPIP_STACK_COMMANDS_ICMPV6_ECHO_TIMEOUT", tcpipIcmpv6ClientCommand)
    tcpipIcmpv6EchoTimeout.setHelp("mcc_h3_icmp_configurations")
    tcpipIcmpv6EchoTimeout.setLabel("ICMPv6 Give Up Time-out (in msec)")
    tcpipIcmpv6EchoTimeout.setVisible(False)
    tcpipIcmpv6EchoTimeout.setDescription("ICMPv6 Give Up Time-out in ms")
    tcpipIcmpv6EchoTimeout.setDefaultValue(5000)
    tcpipIcmpv6EchoTimeout.setDependencies(tcpipIcmpv6MenuVisible, ["TCPIP_ICMPV6_CLIENT_CONSOLE_CMD"])
    
    # Add icmpv6.c file
    tcpipICMPv6SourceFile = tcpipIcmpv6Component.createFileSymbol(None, None)
    tcpipICMPv6SourceFile.setSourcePath("tcpip/src/icmpv6.c")
    tcpipICMPv6SourceFile.setOutputName("icmpv6.c")
    tcpipICMPv6SourceFile.setOverwrite(True)
    tcpipICMPv6SourceFile.setDestPath("library/tcpip/src/")
    tcpipICMPv6SourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipICMPv6SourceFile.setType("SOURCE")
    tcpipICMPv6SourceFile.setEnabled(True)

    #Add to system_config.h
    tcpipIcmpv6HeaderFtl = tcpipIcmpv6Component.createFileSymbol(None, None)
    tcpipIcmpv6HeaderFtl.setSourcePath("tcpip/config/icmpv6.h.ftl")
    tcpipIcmpv6HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipIcmpv6HeaderFtl.setMarkup(True)
    tcpipIcmpv6HeaderFtl.setType("STRING")
    
def tcpipIcmpv6MenuVisible(symbol, event):
    if (event["value"] == True):
        print("TCPIP Menu Visible.")        
        symbol.setVisible(True)
    else:
        print("TCPIP Menu Invisible.")
        symbol.setVisible(False)

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
    Database.setSymbolValue("tcpipIcmpv6", "TCPIP_STACK_USE_ICMPV6_SERVER", False, 2)
