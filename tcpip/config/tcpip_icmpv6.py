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
