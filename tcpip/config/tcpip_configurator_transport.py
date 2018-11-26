# coding: utf-8
##############################################################################
# Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
#
# Subject to your compliance with these terms, you may use Microchip software
# and any derivatives exclusively with Microchip products. It is your
# responsibility to comply with third party license terms applicable to your
# use of third party software (including open source software) that may
# accompany Microchip software.
#
# THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
# EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
# WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
# PARTICULAR PURPOSE.
#
# IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
# INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
# WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
# BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
# FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
# ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
# THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
##############################################################################

################################################################################
#### Business Logic ####
################################################################################

############################################################################
#### Code Generation ####
############################################################################
def instantiateComponent(tcpipAutoConfigTransportComponent):

	global tcpipAutoConfigStackGroup
	global tcpipAutoConfigTransportGroup

	tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
	if (tcpipAutoConfigStackGroup == None):
		tcpipAutoConfigStackGroup = Database.createGroup(None, "TCP/IP STACK")
		
	tcpipAutoConfigTransportGroup = Database.findGroup("TRANSPORT LAYER")
	if (tcpipAutoConfigTransportGroup == None):
		tcpipAutoConfigTransportGroup = Database.createGroup("TCP/IP STACK", "TRANSPORT LAYER")		
	
	# Enable Transport Configurator
	tcpipAutoConfigTransEnable = tcpipAutoConfigTransportComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_TRANS_ENABLE", None)
	tcpipAutoConfigTransEnable.setVisible(False)
	tcpipAutoConfigTransEnable.setDefaultValue(True)

	if(Database.getSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_APPS_ENABLE") != True) and (Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_NET_ENABLE") != True) and (Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_DRV_ENABLE") != True) and (Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_BASIC_ENABLE") != True):
		Database.setActiveGroup("TRANSPORT LAYER")
	
	# Enable TCP
	tcpipAutoConfigTCP = tcpipAutoConfigTransportComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_TCP", None)
	tcpipAutoConfigTCP.setLabel("TCP")
	tcpipAutoConfigTCP.setVisible(True)
	tcpipAutoConfigTCP.setDescription("Enable TCP")	
	tcpipAutoConfigTCP.setDependencies(tcpipAutoConfigTCPEnable, ["TCPIP_AUTOCONFIG_ENABLE_TCP"])
	
	# Enable UDP
	tcpipAutoConfigUDP = tcpipAutoConfigTransportComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_UDP", None)
	tcpipAutoConfigUDP.setLabel("UDP")
	tcpipAutoConfigUDP.setVisible(True)
	tcpipAutoConfigUDP.setDescription("Enable UDP")	
	tcpipAutoConfigUDP.setDependencies(tcpipAutoConfigUDPEnable, ["TCPIP_AUTOCONFIG_ENABLE_UDP"])
	
#######################################################################################################
def enableTcpipAutoConfigTrans(enable):
	global tcpipAutoConfigAppsGroup
	global tcpipAutoConfigTransportGroup
	global tcpipAutoConfigNetworkGroup	
	global tcpipAutoConfigDriverGroup
	global tcpipAutoConfigStackGroup
	global tcpipAutoConfigBasicGroup
	global isEnabled
	if(enable == True):
		tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
		if (tcpipAutoConfigAppsGroup == None):
			tcpipAutoConfigAppsGroup = Database.createGroup("TCP/IP STACK", "APPLICATION LAYER")
			
		tcpipAutoConfigTransportGroup = Database.findGroup("TRANSPORT LAYER")
		if (tcpipAutoConfigTransportGroup == None):
			tcpipAutoConfigTransportGroup = Database.createGroup("TCP/IP STACK", "TRANSPORT LAYER")

		tcpipAutoConfigNetworkGroup = Database.findGroup("NETWORK LAYER")
		if (tcpipAutoConfigNetworkGroup == None):
			tcpipAutoConfigNetworkGroup = Database.createGroup("TCP/IP STACK", "NETWORK LAYER")
			
		tcpipAutoConfigDriverGroup = Database.findGroup("DRIVER LAYER")
		if (tcpipAutoConfigDriverGroup == None):
			tcpipAutoConfigDriverGroup = Database.createGroup("TCP/IP STACK", "DRIVER LAYER")

		tcpipAutoConfigBasicGroup = Database.findGroup("BASIC CONFIGURATION")
		if (tcpipAutoConfigBasicGroup == None):
			tcpipAutoConfigBasicGroup = Database.createGroup("TCP/IP STACK", "BASIC CONFIGURATION")
			
		if(Database.getComponentByID("tcpip_apps_config") == None):
			res = tcpipAutoConfigAppsGroup.addComponent("tcpip_apps_config")
			res = Database.activateComponents(["tcpip_apps_config"], "APPLICATION LAYER", False)
		
		if(Database.getComponentByID("tcpip_network_config") == None):
			res = tcpipAutoConfigNetworkGroup.addComponent("tcpip_network_config")
			res = Database.activateComponents(["tcpip_network_config"], "NETWORK LAYER", False)
			
		if(Database.getComponentByID("tcpip_driver_config") == None):
			res = tcpipAutoConfigDriverGroup.addComponent("tcpip_driver_config")
			res = Database.activateComponents(["tcpip_driver_config"], "DRIVER LAYER", False)	
			
		if(Database.getComponentByID("tcpip_basic_config") == None):
			res = tcpipAutoConfigBasicGroup.addComponent("tcpip_basic_config")
			res = Database.activateComponents(["tcpip_basic_config"], "BASIC CONFIGURATION", False)				
			
		if(Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_STACK") != True):
			Database.setSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_STACK", True, 2)
			
		if(Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_NETCONFIG") != True):
			Database.setSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_NETCONFIG", True, 2)	


#################Business Logic- Transport Layer #########################################	
def tcpipAutoConfigTCPEnable(symbol, event):
	enableTcpipAutoConfigTrans(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipTcp"], "TRANSPORT LAYER")	
		tcpipAutoConfigTransportGroup.setAttachmentVisible("tcpipTcp", "libtcpipTcp")
	else:
		res = Database.deactivateComponents(["tcpipTcp"])
	
def tcpipAutoConfigUDPEnable(symbol, event):
	enableTcpipAutoConfigTrans(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipUdp"], "TRANSPORT LAYER")	
		tcpipAutoConfigTransportGroup.setAttachmentVisible("tcpipUdp", "libtcpipUdp")		
	else:
		res = Database.deactivateComponents(["tcpipUdp"])
	
	
