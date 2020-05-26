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

def instantiateComponent(tcpipNbnsComponent):
	print("TCPIP NBNS Component")
	configName = Variables.get("__CONFIGURATION_NAME")
		
	# Use NetBIOS Name Server
	tcpipNbns = tcpipNbnsComponent.createBooleanSymbol("TCPIP_USE_NBNS", None)
	tcpipNbns.setLabel("Use NetBIOS Name Server")
	tcpipNbns.setVisible(False)
	tcpipNbns.setDescription("Use NetBIOS Name Server")
	tcpipNbns.setDefaultValue(True)
	#tcpipNbns.setDependencies(tcpipNbnsMenuVisible, ["tcpipIPv4.TCPIP_STACK_USE_IPV4", "tcpipUdp.TCPIP_USE_UDP"])

	# NetBIOS Task Rate in ms
	tcpipNbnsTskTickRate = tcpipNbnsComponent.createIntegerSymbol("TCPIP_NBNS_TASK_TICK_RATE", None)
	tcpipNbnsTskTickRate.setLabel("NetBIOS Task Rate - ms")
	tcpipNbnsTskTickRate.setVisible(True)
	tcpipNbnsTskTickRate.setDescription("NetBIOS Task Rate in ms")
	tcpipNbnsTskTickRate.setDefaultValue(110)
	#tcpipNbnsTskTickRate.setDependencies(tcpipNbnsMenuVisibleSingle, ["TCPIP_USE_NBNS"])

	#Add to system_config.h
	tcpipNbnsHeaderFtl = tcpipNbnsComponent.createFileSymbol(None, None)
	tcpipNbnsHeaderFtl.setSourcePath("tcpip/config/nbns.h.ftl")
	tcpipNbnsHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipNbnsHeaderFtl.setMarkup(True)
	tcpipNbnsHeaderFtl.setType("STRING")

	# Add nbns.c file
	tcpipNbnsSourceFile = tcpipNbnsComponent.createFileSymbol(None, None)
	tcpipNbnsSourceFile.setSourcePath("tcpip/src/nbns.c")
	tcpipNbnsSourceFile.setOutputName("nbns.c")
	tcpipNbnsSourceFile.setOverwrite(True)
	tcpipNbnsSourceFile.setDestPath("library/tcpip/src/")
	tcpipNbnsSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipNbnsSourceFile.setType("SOURCE")
	tcpipNbnsSourceFile.setEnabled(True)
	#tcpipNbnsSourceFile.setDependencies(tcpipNbnsGenSourceFile, ["TCPIP_USE_NBNS"])

# make Nbns Server option visible
def tcpipNbnsMenuVisible(symbol, event):
	tcpipIPv4 = Database.getSymbolValue("tcpipIPv4","TCPIP_STACK_USE_IPV4")
	tcpipUdp = Database.getSymbolValue("tcpipUdp","TCPIP_USE_UDP")

	if(tcpipIPv4 and tcpipUdp):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)
		
def tcpipNbnsMenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("NBNS Menu Visible.")		
		testname =Variables.get("__PROCESSOR") 
		print(testname)
		symbol.setVisible(True)
	else:
		print("NBNS Menu Invisible.")
		symbol.setVisible(False)

def tcpipNbnsGenSourceFile(sourceFile, event):
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
	Database.setSymbolValue("tcpipNbns", "TCPIP_USE_NBNS", False, 2)