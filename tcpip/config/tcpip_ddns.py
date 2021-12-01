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

    
def instantiateComponent(tcpipDdnsComponent):
    print("TCPIP DNS Client Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # Enable Dynamis DNS Module
    tcpipDdns = tcpipDdnsComponent.createBooleanSymbol("TCPIP_USE_DDNS", None)
    tcpipDdns.setHelp("mcc_h3_ddns_configurations")
    tcpipDdns.setLabel("Dynamic DNS Module")
    tcpipDdns.setVisible(False)
    tcpipDdns.setDescription("Enable Dynamic DNS Module")
    tcpipDdns.setDefaultValue(True)

    # Default CheckIP server for Determining Current IP Address
    tcpipDdnsChkIpServer = tcpipDdnsComponent.createStringSymbol("TCPIP_DDNS_CHECKIP_SERVER", None)
    tcpipDdnsChkIpServer.setHelp("mcc_h3_ddns_configurations")
    tcpipDdnsChkIpServer.setLabel("Default CheckIP Server")
    tcpipDdnsChkIpServer.setVisible(True)
    tcpipDdnsChkIpServer.setDescription("Default CheckIP server for Determining Current IP Address")
    tcpipDdnsChkIpServer.setDefaultValue("checkip.dyndns.com")
    
    # Advanced Settings
    tcpipDdnsAdvSettings = tcpipDdnsComponent.createMenuSymbol("TCPIP_DDNS_ADV_SETTING", None)
    tcpipDdnsAdvSettings.setLabel("Advanced Settings")
    tcpipDdnsAdvSettings.setDescription("Advanced Settings")
    tcpipDdnsAdvSettings.setVisible(True)
    
    # Dynamic DNS Task Rate in msec
    tcpipDdnsTskRate = tcpipDdnsComponent.createIntegerSymbol("TCPIP_DDNS_TASK_TICK_RATE", tcpipDdnsAdvSettings)
    tcpipDdnsTskRate.setHelp("mcc_h3_ddns_configurations")
    tcpipDdnsTskRate.setLabel("dynDNS Task Rate (in msec)")
    tcpipDdnsTskRate.setVisible(True)
    tcpipDdnsTskRate.setDescription("Dynamic DNS Task Rate in msec")
    tcpipDdnsTskRate.setDefaultValue(777)
    
    # Add to system_config.h
    tcpipDdnsHeaderFtl = tcpipDdnsComponent.createFileSymbol(None, None)
    tcpipDdnsHeaderFtl.setSourcePath("tcpip/config/ddns.h.ftl")
    tcpipDdnsHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipDdnsHeaderFtl.setMarkup(True)
    tcpipDdnsHeaderFtl.setType("STRING")

    # Add ddns.c file
    tcpipDdnsSourceFile = tcpipDdnsComponent.createFileSymbol(None, None)
    tcpipDdnsSourceFile.setSourcePath("tcpip/src/ddns.c")
    tcpipDdnsSourceFile.setOutputName("ddns.c")
    tcpipDdnsSourceFile.setOverwrite(True)
    tcpipDdnsSourceFile.setDestPath("library/tcpip/src/")
    tcpipDdnsSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipDdnsSourceFile.setType("SOURCE")
    tcpipDdnsSourceFile.setEnabled(True)

# make Dynamic DNS module option visible

def tcpipDdnsMenuVisible(symbol, event):
    if (event["value"] == True):
        print("DDNS Menu Visible.")     
        symbol.setVisible(True)
    else:
        print("DDNS Menu Invisible.")
        symbol.setVisible(False)    

        
def tcpipDdnsGenSourceFile(sourceFile, event):
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
    Database.setSymbolValue("tcpipDdns", "TCPIP_USE_DDNS", False, 2)
