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

    
def instantiateComponent(tcpipIcmpComponent):
	print("TCPIP ICMP Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	# ICMPv4 Client and Server
	tcpipIcmp = tcpipIcmpComponent.createBooleanSymbol("TCPIP_STACK_USE_ICMPV4", None)
	tcpipIcmp.setLabel("ICMPv4 Client and Server")
	tcpipIcmp.setVisible(False)
	tcpipIcmp.setDescription("ICMPv4 Client and Server")
	tcpipIcmp.setDefaultValue(True)
	#tcpipIcmp.setDependencies(tcpipIcmpMenuVisible, ["tcpipIPv4.TCPIP_STACK_USE_IPV4"])

	# Use ICMPv4 Server
	tcpipIcmpv4Server = tcpipIcmpComponent.createBooleanSymbol("TCPIP_STACK_USE_ICMP_SERVER", None)
	tcpipIcmpv4Server.setLabel("Use ICMPv4 Server")
	tcpipIcmpv4Server.setVisible(True)
	tcpipIcmpv4Server.setDescription("Use ICMPv4 Server")
	tcpipIcmpv4Server.setDefaultValue(True) 

	# Use ICMPv4 Client
	tcpipIcmpv4Client = tcpipIcmpComponent.createBooleanSymbol("TCPIP_STACK_USE_ICMP_CLIENT", None)
	tcpipIcmpv4Client.setLabel("Use ICMPv4 Client")
	tcpipIcmpv4Client.setVisible(True)
	tcpipIcmpv4Client.setDescription("Use ICMPv4 Client")
	tcpipIcmpv4Client.setDefaultValue(False)
	#tcpipIcmpv4Client.setDependencies(tcpipIcmpMenuVisible, ["TCPIP_STACK_USE_ICMPV4"])

	# Enable User Notification
	tcpipIcmpClientUsrNotify = tcpipIcmpComponent.createBooleanSymbol("TCPIP_ICMP_CLIENT_USER_NOTIFICATION", tcpipIcmpv4Client)
	tcpipIcmpClientUsrNotify.setLabel("Enable User Notification")
	tcpipIcmpClientUsrNotify.setVisible(False)
	tcpipIcmpClientUsrNotify.setDescription("Enable User Notification")
	tcpipIcmpClientUsrNotify.setDefaultValue(True)
	tcpipIcmpClientUsrNotify.setDependencies(tcpipIcmpMenuVisible, ["TCPIP_STACK_USE_ICMP_CLIENT"])

	# Echo request timeout in ms
	tcpipIcmpEchoReqTimeout = tcpipIcmpComponent.createIntegerSymbol("TCPIP_ICMP_ECHO_REQUEST_TIMEOUT", tcpipIcmpv4Client)
	tcpipIcmpEchoReqTimeout.setLabel("Echo request timeout - ms")
	tcpipIcmpEchoReqTimeout.setVisible(False)
	tcpipIcmpEchoReqTimeout.setDescription("Echo request timeout in ms")
	tcpipIcmpEchoReqTimeout.setDefaultValue(500)
	tcpipIcmpEchoReqTimeout.setDependencies(tcpipIcmpMenuVisible, ["TCPIP_STACK_USE_ICMP_CLIENT"])

	# ICMP task rate in ms
	tcpipIcmpTskTickRate = tcpipIcmpComponent.createIntegerSymbol("TCPIP_ICMP_TASK_TICK_RATE", tcpipIcmpv4Client)
	tcpipIcmpTskTickRate.setLabel("ICMP task rate - ms")
	tcpipIcmpTskTickRate.setVisible(False)
	tcpipIcmpTskTickRate.setDescription("ICMP task rate in ms")
	tcpipIcmpTskTickRate.setDefaultValue(33)
	tcpipIcmpTskTickRate.setDependencies(tcpipIcmpMenuVisible, ["TCPIP_STACK_USE_ICMP_CLIENT"])

	# Allow Replies to Echo Broadcast Requests
	tcpipIcmpEchoBroadcast = tcpipIcmpComponent.createBooleanSymbol("TCPIP_ICMP_ECHO_BROADCASTS", tcpipIcmpv4Server)
	tcpipIcmpEchoBroadcast.setLabel("Allow Replies to Echo Broadcast Requests")
	tcpipIcmpEchoBroadcast.setVisible(True)
	tcpipIcmpEchoBroadcast.setDescription("Allow Replies to Echo Broadcast Requests")
	tcpipIcmpEchoBroadcast.setDefaultValue(False)
	tcpipIcmpEchoBroadcast.setDependencies(tcpipIcmpMenuVisible, ["TCPIP_STACK_USE_ICMP_SERVER"])
	
	#Add to system_config.h
	tcpipIcmpHeaderFtl = tcpipIcmpComponent.createFileSymbol(None, None)
	tcpipIcmpHeaderFtl.setSourcePath("tcpip/config/icmp.h.ftl")
	tcpipIcmpHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipIcmpHeaderFtl.setMarkup(True)
	tcpipIcmpHeaderFtl.setType("STRING")

	# Add icmp.c file
	tcpipIcmpSourceFile = tcpipIcmpComponent.createFileSymbol(None, None)
	tcpipIcmpSourceFile.setSourcePath("tcpip/src/icmp.c")
	tcpipIcmpSourceFile.setOutputName("icmp.c")
	tcpipIcmpSourceFile.setOverwrite(True)
	tcpipIcmpSourceFile.setDestPath("library/tcpip/src/")
	tcpipIcmpSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipIcmpSourceFile.setType("SOURCE")
	tcpipIcmpSourceFile.setEnabled(True)
	tcpipIcmpSourceFile.setDependencies(tcpipIcmpGenSourceFile, ["TCPIP_STACK_USE_ICMP_SERVER","TCPIP_STACK_USE_ICMP_CLIENT"])

def tcpipIcmpMenuVisible(symbol, event):
	if (event["value"] == True):
		print("ICMP Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("ICMP Menu Invisible.")
		symbol.setVisible(False)
		
def tcpipIcmpGenSourceFile(sourceFile, event):
	tcpipIcmpClient = Database.getSymbolValue("tcpipIcmp","TCPIP_STACK_USE_ICMP_CLIENT")
	tcpipIcmpServer = Database.getSymbolValue("tcpipIcmp","TCPIP_STACK_USE_ICMP_SERVER")
	if(tcpipIcmpClient or tcpipIcmpServer ):
		sourceFile.setEnabled(event["value"])
		

def destroyComponent(component):
	Database.setSymbolValue("tcpipIcmp", "TCPIP_STACK_USE_ICMPV4", False, 2)