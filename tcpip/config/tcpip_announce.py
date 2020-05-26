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

def instantiateComponent(tcpipAnnounceComponent):
	print("TCPIP Announce Component")
	configName = Variables.get("__CONFIGURATION_NAME")
		
	# Use Announce Discovery Tool
	tcpipAnnounce = tcpipAnnounceComponent.createBooleanSymbol("TCPIP_USE_ANNOUNCE", None)
	tcpipAnnounce.setLabel("Use Announce Discovery Tool")
	tcpipAnnounce.setVisible(False)
	tcpipAnnounce.setDescription("Use Announce Discovery Tool")
	tcpipAnnounce.setDefaultValue(True)
	#tcpipAnnounce.setDependencies(tcpipAnnounceMenuVisible, ["tcpipIPv4.TCPIP_STACK_USE_IPV4", "tcpipUdp.TCPIP_USE_UDP"])

	# Maximum Payload Size
	tcpipAnnouncePayloadMax= tcpipAnnounceComponent.createIntegerSymbol("TCPIP_ANNOUNCE_MAX_PAYLOAD", None)
	tcpipAnnouncePayloadMax.setLabel("Maximum Payload Size")
	tcpipAnnouncePayloadMax.setVisible(True)
	tcpipAnnouncePayloadMax.setDescription("Maximum Payload Size")
	tcpipAnnouncePayloadMax.setDefaultValue(512)
	#tcpipAnnouncePayloadMax.setDependencies(tcpipAnnounceMenuVisibleSingle, ["TCPIP_USE_ANNOUNCE"])

	# Use Announce Network Directed Broadcast
	tcpipAnnounceNetBroadcast = tcpipAnnounceComponent.createBooleanSymbol("TCPIP_ANNOUNCE_NETWORK_DIRECTED_BCAST", None)
	tcpipAnnounceNetBroadcast.setLabel("Use Announce Network Directed Broadcast")
	tcpipAnnounceNetBroadcast.setVisible(True)
	tcpipAnnounceNetBroadcast.setDescription("Use Announce Network Directed Broadcast")
	tcpipAnnounceNetBroadcast.setDefaultValue(False) 
	#tcpipAnnounceNetBroadcast.setDependencies(tcpipAnnounceMenuVisibleSingle, ["TCPIP_USE_ANNOUNCE"])

	# Announce Task Rate in ms
	tcpipAnnounceTskRate= tcpipAnnounceComponent.createIntegerSymbol("TCPIP_ANNOUNCE_TASK_RATE", None)
	tcpipAnnounceTskRate.setLabel("Announce Task Rate - ms")
	tcpipAnnounceTskRate.setVisible(True)
	tcpipAnnounceTskRate.setDescription("Announce Task Rate in ms")
	tcpipAnnounceTskRate.setDefaultValue(333)
	#tcpipAnnounceTskRate.setDependencies(tcpipAnnounceMenuVisibleSingle, ["TCPIP_USE_ANNOUNCE"])

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
	#tcpipAnnounceSourceFile.setDependencies(tcpipAnnounceGenSourceFile, ["TCPIP_USE_ANNOUNCE"])

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