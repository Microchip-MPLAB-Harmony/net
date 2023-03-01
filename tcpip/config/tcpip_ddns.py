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
