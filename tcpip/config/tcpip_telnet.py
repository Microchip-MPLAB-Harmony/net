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

    
def instantiateComponent(tcpipTelnetComponent):
	print("TCPIP Telnet Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	# Use Telnet Server
	tcpipTelnet = tcpipTelnetComponent.createBooleanSymbol("TCPIP_USE_TELNET", None)
	tcpipTelnet.setLabel("Use Telnet Server")
	tcpipTelnet.setVisible(False)
	tcpipTelnet.setDescription("Use Telnet Server")
	tcpipTelnet.setDefaultValue(True) 

	# Max Connections
	tcpipTelnetConnMax = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_MAX_CONNECTIONS", None)
	tcpipTelnetConnMax.setLabel("Maximum Connections")
	tcpipTelnetConnMax.setVisible(True)
	tcpipTelnetConnMax.setDescription("Maximum Connections")
	tcpipTelnetConnMax.setDefaultValue(2)
	#tcpipTelnetConnMax.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

	# Telnet User Name
	tcpipTelnetUsrName = tcpipTelnetComponent.createStringSymbol("TCPIP_TELNET_USERNAME", None)
	tcpipTelnetUsrName.setLabel("User Name")
	tcpipTelnetUsrName.setVisible(True)
	tcpipTelnetUsrName.setDescription("Telnet User Name")
	tcpipTelnetUsrName.setDefaultValue("admin")
	#tcpipTelnetUsrName.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

	# Telnet Password
	tcpipTelnetPswd = tcpipTelnetComponent.createStringSymbol("TCPIP_TELNET_PASSWORD", None)
	tcpipTelnetPswd.setLabel("Password")
	tcpipTelnetPswd.setVisible(True)
	tcpipTelnetPswd.setDescription("Telnet Password")
	tcpipTelnetPswd.setDefaultValue("microchip")
	#tcpipTelnetPswd.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

	# Task Rate in ms
	tcpipTelnetTskTickRate = tcpipTelnetComponent.createIntegerSymbol("TCPIP_TELNET_TASK_TICK_RATE", None)
	tcpipTelnetTskTickRate.setLabel("Task Rate - ms")
	tcpipTelnetTskTickRate.setVisible(True)
	tcpipTelnetTskTickRate.setDescription("Task Rate in ms")
	tcpipTelnetTskTickRate.setDefaultValue(100)
	#tcpipTelnetTskTickRate.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

	# Reject Unsecure Connections
	tcpipTelnetRejectUnsecure = tcpipTelnetComponent.createBooleanSymbol("TCPIP_TELNET_REJECT_UNSECURED", None)
	tcpipTelnetRejectUnsecure.setLabel("Reject Unsecure Connections")
	tcpipTelnetRejectUnsecure.setVisible(True)
	tcpipTelnetRejectUnsecure.setDescription("Reject Unsecure Connections")
	tcpipTelnetRejectUnsecure.setDefaultValue(False)
	#tcpipTelnetRejectUnsecure.setDependencies(tcpipTelnetMenuVisible, ["TCPIP_USE_TELNET"])

	#Add to system_config.h
	tcpipTelnetHeaderFtl = tcpipTelnetComponent.createFileSymbol(None, None)
	tcpipTelnetHeaderFtl.setSourcePath("tcpip/config/telnet.h.ftl")
	tcpipTelnetHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipTelnetHeaderFtl.setMarkup(True)
	tcpipTelnetHeaderFtl.setType("STRING")

	# Add telnet.c file
	tcpipTelnetSourceFile = tcpipTelnetComponent.createFileSymbol(None, None)
	tcpipTelnetSourceFile.setSourcePath("tcpip/src/telnet.c")
	tcpipTelnetSourceFile.setOutputName("telnet.c")
	tcpipTelnetSourceFile.setOverwrite(True)
	tcpipTelnetSourceFile.setDestPath("library/tcpip/src/")
	tcpipTelnetSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipTelnetSourceFile.setType("SOURCE")
	tcpipTelnetSourceFile.setEnabled(True)
	#tcpipTelnetSourceFile.setDependencies(tcpipTelnetGenSourceFile, ["TCPIP_USE_TELNET"])
	
def tcpipTelnetMenuVisible(symbol, event):
	if (event["value"] == True):
		print("Telnet Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("Telnet Menu Invisible.")
		symbol.setVisible(False)
		
def tcpipTelnetGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])

def destroyComponent(component):
	Database.setSymbolValue("tcpipTelnet", "TCPIP_USE_TELNET", False, 2)