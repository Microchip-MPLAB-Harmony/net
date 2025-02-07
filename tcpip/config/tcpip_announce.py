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
# Global definitions  
TCPIP_ANNOUNCE_BRDCST_TYPE = ["Network Limited", "Network Directed"]

def instantiateComponent(tcpipAnnounceComponent):
    print("TCPIP Announce Component")
    configName = Variables.get("__CONFIGURATION_NAME")
        
    # Use Announce Discovery Tool
    tcpipAnnounce = tcpipAnnounceComponent.createBooleanSymbol("TCPIP_USE_ANNOUNCE", None)
    tcpipAnnounce.setHelp("mcc_h3_announce_configurations")
    tcpipAnnounce.setLabel("Use Announce Discovery Tool")
    tcpipAnnounce.setVisible(False)
    tcpipAnnounce.setDescription("Use Announce Discovery Tool")
    tcpipAnnounce.setDefaultValue(True)

    # Enable/Disable Announce Network Directed Broadcast
    tcpipAnnounceNetBroadcast = tcpipAnnounceComponent.createComboSymbol("TCPIP_ANNOUNCE_NETWORK_DIRECTED_BCAST", None, TCPIP_ANNOUNCE_BRDCST_TYPE)
    tcpipAnnounceNetBroadcast.setHelp("mcc_h3_announce_configurations")
    tcpipAnnounceNetBroadcast.setLabel("Announce Broadcast Type")
    tcpipAnnounceNetBroadcast.setVisible(True)
    tcpipAnnounceNetBroadcast.setDescription("Enable/Disable Announce Network Directed Broadcast")
    tcpipAnnounceNetBroadcast.setDefaultValue("Network Limited") 
    
    # Advanced Settings
    tcpipAnnounceAdvSettings = tcpipAnnounceComponent.createMenuSymbol("TCPIP_ANNOUNCE_ADV_SETTING", None)
    tcpipAnnounceAdvSettings.setLabel("Advanced Settings")
    tcpipAnnounceAdvSettings.setDescription("Advanced Settings")
    tcpipAnnounceAdvSettings.setVisible(True)

    # Announce Task Rate in ms
    tcpipAnnounceTskRate= tcpipAnnounceComponent.createIntegerSymbol("TCPIP_ANNOUNCE_TASK_RATE", tcpipAnnounceAdvSettings)
    tcpipAnnounceTskRate.setHelp("mcc_h3_announce_configurations")
    tcpipAnnounceTskRate.setLabel("Announce Task Rate (in msec)")
    tcpipAnnounceTskRate.setVisible(True)
    tcpipAnnounceTskRate.setDescription("Announce Task Rate in ms")
    tcpipAnnounceTskRate.setDefaultValue(333)
    
    # Maximum Payload Size
    tcpipAnnouncePayloadMax= tcpipAnnounceComponent.createIntegerSymbol("TCPIP_ANNOUNCE_MAX_PAYLOAD", tcpipAnnounceAdvSettings)
    tcpipAnnouncePayloadMax.setHelp("mcc_h3_announce_configurations")
    tcpipAnnouncePayloadMax.setLabel("Maximum Payload Size")
    tcpipAnnouncePayloadMax.setVisible(True)
    tcpipAnnouncePayloadMax.setDescription("Maximum Payload Size")
    tcpipAnnouncePayloadMax.setDefaultValue(512)


    #Add to system_config.h
    tcpipAnnounceHeaderFtl = tcpipAnnounceComponent.createFileSymbol(None, None)
    tcpipAnnounceHeaderFtl.setSourcePath("tcpip/config/tcpip_announce.h.ftl")
    tcpipAnnounceHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipAnnounceHeaderFtl.setMarkup(True)
    tcpipAnnounceHeaderFtl.setType("STRING")

    # Add tcpip_announce.c file
    tcpipAnnounceSourceFile = tcpipAnnounceComponent.createFileSymbol(None, None)
    tcpipAnnounceSourceFile.setSourcePath("tcpip/src/tcpip_announce.c")
    tcpipAnnounceSourceFile.setOutputName("tcpip_announce.c")
    tcpipAnnounceSourceFile.setOverwrite(True)
    tcpipAnnounceSourceFile.setDestPath("library/tcpip/src/")
    tcpipAnnounceSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipAnnounceSourceFile.setType("SOURCE")
    tcpipAnnounceSourceFile.setEnabled(True)

# make TCPIP Announce discovery option visible
def tcpipAnnounceMenuVisible(symbol, event):
    tcpipIPv4 = Database.getSymbolValue("tcpipIPv4","TCPIP_STACK_USE_IPV4")
    tcpipUdp = Database.getSymbolValue("tcpipUdp","TCPIP_USE_UDP")

    if(tcpipIPv4 and tcpipUdp):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)
        
def tcpipAnnounceMenuVisibleSingle(symbol, event):
    if (event["value"] == True):
        print("Announce Menu Visible.")     
        symbol.setVisible(True)
    else:
        print("Announce Menu Invisible.")
        symbol.setVisible(False)

def tcpipAnnounceGenSourceFile(sourceFile, event):
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
    Database.setSymbolValue("tcpipAnnounce", "TCPIP_USE_ANNOUNCE", False, 2)
