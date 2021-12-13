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

def instantiateComponent(tcpipCmdComponent):
	print("TCPIP CMD Component")
	configName = Variables.get("__CONFIGURATION_NAME")
		
	# Use TCP/IP Commands
	tcpipCmd = tcpipCmdComponent.createBooleanSymbol("TCPIP_STACK_USE_COMMANDS", None)
	tcpipCmd.setHelp("mcc_h3_tcpipcmd_configurations")
	tcpipCmd.setLabel("Use TCP/IP Commands")
	tcpipCmd.setVisible(False)
	tcpipCmd.setDescription("Use TCP/IP Commands")
	tcpipCmd.setDefaultValue(True)

	# Enable Storage for Stack Commands
	tcpipCmdStorage = tcpipCmdComponent.createBooleanSymbol("TCPIP_STACK_COMMANDS_STORAGE_ENABLE", None)
	tcpipCmdStorage.setHelp("mcc_h3_tcpipcmd_configurations")
	tcpipCmdStorage.setLabel("Enable Storage for Stack Commands")
	tcpipCmdStorage.setVisible((Database.getSymbolValue("tcpipStack", "TCPIP_STACK_CONFIGURATION_SAVE_RESTORE") == True))
	tcpipCmdStorage.setDescription("Enable Storage for Stack Commands")
	tcpipCmdStorage.setDefaultValue(False)  
	tcpipCmdStorage.setDependencies(tcpipCmdStorageMenuVisible, ["tcpipStack.TCPIP_STACK_CONFIGURATION_SAVE_RESTORE"]) 

	#Add to system_config.h
	tcpipCmdHeaderFtl = tcpipCmdComponent.createFileSymbol(None, None)
	tcpipCmdHeaderFtl.setSourcePath("tcpip/config/tcpip_cmd.h.ftl")
	tcpipCmdHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipCmdHeaderFtl.setMarkup(True)
	tcpipCmdHeaderFtl.setType("STRING")

	# Add tcpip_commands.c file
	tcpipCmdSourceFile = tcpipCmdComponent.createFileSymbol(None, None)
	tcpipCmdSourceFile.setSourcePath("tcpip/src/tcpip_commands.c")
	tcpipCmdSourceFile.setOutputName("tcpip_commands.c")
	tcpipCmdSourceFile.setOverwrite(True)
	tcpipCmdSourceFile.setDestPath("library/tcpip/src/")
	tcpipCmdSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipCmdSourceFile.setType("SOURCE")
	tcpipCmdSourceFile.setEnabled(True)

# Enable Storage for Stack Commands option visible
def tcpipCmdStorageMenuVisible(symbol, event):	
	if (event["value"] == True):
		symbol.setVisible(True)
	else:
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
	Database.setSymbolValue("tcpipCmd", "TCPIP_STACK_USE_COMMANDS", False, 2)
