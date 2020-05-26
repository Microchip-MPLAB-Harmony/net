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
	tcpipCmd.setLabel("Use TCP/IP Commands")
	tcpipCmd.setVisible(False)
	tcpipCmd.setDescription("Use TCP/IP Commands")
	tcpipCmd.setDefaultValue(True)

	# Enable Storage for Stack Commands
	tcpipCmdStorage = tcpipCmdComponent.createBooleanSymbol("TCPIP_STACK_COMMANDS_STORAGE_ENABLE", None)
	tcpipCmdStorage.setLabel("Enable Storage for Stack Commands")
	tcpipCmdStorage.setVisible((Database.getSymbolValue("tcpipStack", "TCPIP_STACK_CONFIGURATION_SAVE_RESTORE") == True))
	tcpipCmdStorage.setDescription("Enable Storage for Stack Commands")
	tcpipCmdStorage.setDefaultValue(False)  
	tcpipCmdStorage.setDependencies(tcpipCmdStorageMenuVisible, ["tcpipStack.TCPIP_STACK_CONFIGURATION_SAVE_RESTORE"]) 

	# Number of ICMP Echo requests
	tcpipCmdIcmpEchoReqNum = tcpipCmdComponent.createIntegerSymbol("TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUESTS", None)
	tcpipCmdIcmpEchoReqNum.setLabel("Number of ICMP Echo requests")
	tcpipCmdIcmpEchoReqNum.setVisible(True)
	tcpipCmdIcmpEchoReqNum.setDescription("Number of ICMP Echo requests")
	tcpipCmdIcmpEchoReqNum.setDefaultValue(4)
	#tcpipCmdIcmpEchoReqNum.setDependencies(tcpipCmdMenuVisibleSingle, ["TCPIP_STACK_USE_COMMANDS"])

	# ICMP Reply Time-out in ms
	tcpipCmdIcmpEchoReqDelay = tcpipCmdComponent.createIntegerSymbol("TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DELAY", None)
	tcpipCmdIcmpEchoReqDelay.setLabel("ICMP Reply Time-out, ms")
	tcpipCmdIcmpEchoReqDelay.setVisible(True)
	tcpipCmdIcmpEchoReqDelay.setDescription("ICMP Reply Time-out in ms")
	tcpipCmdIcmpEchoReqDelay.setDefaultValue(1000)
	#tcpipCmdIcmpEchoReqDelay.setDependencies(tcpipCmdMenuVisibleSingle, ["TCPIP_STACK_USE_COMMANDS"])

	# ICMP Give Up Time-out in ms
	tcpipCmdIcmpEchoTimeout= tcpipCmdComponent.createIntegerSymbol("TCPIP_STACK_COMMANDS_ICMP_ECHO_TIMEOUT", None)
	tcpipCmdIcmpEchoTimeout.setLabel("ICMP Give Up Time-out, ms")
	tcpipCmdIcmpEchoTimeout.setVisible(True)
	tcpipCmdIcmpEchoTimeout.setDescription("ICMP Give Up Time-out in ms")
	tcpipCmdIcmpEchoTimeout.setDefaultValue(5000)
	#tcpipCmdIcmpEchoTimeout.setDependencies(tcpipCmdMenuVisibleSingle, ["TCPIP_STACK_USE_COMMANDS"])

	# Enable Wi-Fi Related Commands
	tcpipCmdWifi = tcpipCmdComponent.createBooleanSymbol("TCPIP_STACK_COMMANDS_WIFI_ENABLE", None)
	tcpipCmdWifi.setLabel("Enable Wi-Fi Related Commands")
	tcpipCmdWifi.setVisible(False) # H3_ToDo: Enable this when Wifi is added
	tcpipCmdWifi.setDescription("Enable Wi-Fi Related Commands")
	tcpipCmdWifi.setDefaultValue(False) 
	#tcpipCmdWifi.setDependencies(tcpipCmdMenuVisibleSingle, ["TCPIP_STACK_USE_COMMANDS"])

	# Echo Request Data Buffer size - bytes
	tcpipCmdIcmpEchoReqBuffSize= tcpipCmdComponent.createIntegerSymbol("TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_BUFF_SIZE", None)
	tcpipCmdIcmpEchoReqBuffSize.setLabel("Echo Request Data Buffer size - bytes")
	tcpipCmdIcmpEchoReqBuffSize.setVisible((Database.getSymbolValue("tcpipTcp", "TCPIP_USE_TCP") == True))
	tcpipCmdIcmpEchoReqBuffSize.setDescription("Echo Request Data Buffer size - bytes")
	tcpipCmdIcmpEchoReqBuffSize.setDefaultValue(2000)
	tcpipCmdIcmpEchoReqBuffSize.setDependencies(tcpipCmdICMPEnable, ["tcpipTcp.TCPIP_USE_TCP"])

	# Echo request Default Data Size - bytes
	tcpipCmdIcmpEchoReqDataSize= tcpipCmdComponent.createIntegerSymbol("TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DATA_SIZE", None)
	tcpipCmdIcmpEchoReqDataSize.setLabel("Echo request Default Data Size - bytes")
	tcpipCmdIcmpEchoReqDataSize.setVisible((Database.getSymbolValue("tcpipTcp", "TCPIP_USE_TCP") == True))
	tcpipCmdIcmpEchoReqDataSize.setDescription("Echo request Default Data Size - bytes")
	tcpipCmdIcmpEchoReqDataSize.setDefaultValue(100)
	tcpipCmdIcmpEchoReqDataSize.setDependencies(tcpipCmdICMPEnable, ["tcpipTcp.TCPIP_USE_TCP"])

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
	#tcpipCmdSourceFile.setDependencies(tcpipCmdGenSourceFile, ["TCPIP_STACK_USE_COMMANDS"])

# Enable Storage for Stack Commands option visible
def tcpipCmdStorageMenuVisible(symbol, event):	

	if (event["value"] == True):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)
		
def tcpipCmdMenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("TFTPC Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("TFTPC Menu Invisible.")
		symbol.setVisible(False)

def tcpipCmdGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])
	
def tcpipCmdICMPEnable(symbol, event):	
	tcpipCmdTCPEnabled = Database.getSymbolValue("tcpipTcp", "TCPIP_USE_TCP")	
	if (tcpipCmdTCPEnabled == True):
		symbol.setVisible(True)		
	else:
		symbol.setVisible(False)
		

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
	Database.setSymbolValue("tcpipCmd", "TCPIP_STACK_USE_COMMANDS", False, 2)