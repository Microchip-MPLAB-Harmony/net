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
def instantiateComponent(tcpipAutoConfigNetworkComponent):
	global tcpipAutoConfigStackGroup
	global tcpipAutoConfigNetworkGroup

	tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
	if (tcpipAutoConfigStackGroup == None):
		tcpipAutoConfigStackGroup = Database.createGroup(None, "TCP/IP STACK")
		
	tcpipAutoConfigNetworkGroup = Database.findGroup("NETWORK LAYER")
	if (tcpipAutoConfigNetworkGroup == None):
		tcpipAutoConfigNetworkGroup = Database.createGroup("TCP/IP STACK", "NETWORK LAYER")	
	
	# Enable Network Configurator
	tcpipAutoConfigNetEnable = tcpipAutoConfigNetworkComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_NET_ENABLE", None)
	tcpipAutoConfigNetEnable.setVisible(False)
	tcpipAutoConfigNetEnable.setDefaultValue(True)

	if(Database.getSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_APPS_ENABLE") != True) and (Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_TRANS_ENABLE") != True) and (Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_DRV_ENABLE") != True) and (Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_BASIC_ENABLE") != True):
		Database.setActiveGroup("NETWORK LAYER")
		
	# Enable ARP
	tcpipAutoConfigARP = tcpipAutoConfigNetworkComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_ARP", None)
	tcpipAutoConfigARP.setLabel("ARP")
	tcpipAutoConfigARP.setVisible(True)
	tcpipAutoConfigARP.setDescription("Enable ARP")	
	tcpipAutoConfigARP.setDependencies(tcpipAutoConfigARPEnable, ["TCPIP_AUTOCONFIG_ENABLE_ARP"])
	
	# Enable ICMPv4
	tcpipAutoConfigICMPv4 = tcpipAutoConfigNetworkComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_ICMPv4", None)
	tcpipAutoConfigICMPv4.setLabel("ICMPv4")
	tcpipAutoConfigICMPv4.setVisible(True)
	tcpipAutoConfigICMPv4.setDescription("Enable ICMPv4")
	tcpipAutoConfigICMPv4.setDependencies(tcpipAutoConfigICMPEnable, ["TCPIP_AUTOCONFIG_ENABLE_ICMPv4"])
	
	# Enable IGMP
	tcpipAutoConfigIGMP = tcpipAutoConfigNetworkComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_IGMP", None)
	tcpipAutoConfigIGMP.setLabel("IGMP")
	tcpipAutoConfigIGMP.setVisible(True)
	tcpipAutoConfigIGMP.setDescription("Enable IGMP")	
	tcpipAutoConfigIGMP.setDependencies(tcpipAutoConfigIGMPEnable, ["TCPIP_AUTOCONFIG_ENABLE_IGMP"])
	
	# Enable IPv4
	tcpipAutoConfigIPv4 = tcpipAutoConfigNetworkComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_IPV4", None)
	tcpipAutoConfigIPv4.setLabel("IPv4")
	tcpipAutoConfigIPv4.setVisible(True)
	tcpipAutoConfigIPv4.setDescription("Enable IPv4")
	tcpipAutoConfigIPv4.setDependencies(tcpipAutoConfigIPv4Enable, ["TCPIP_AUTOCONFIG_ENABLE_IPV4"])
	
	# Enable IPv6
	tcpipAutoConfigIPv6 = tcpipAutoConfigNetworkComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_IPV6", None)
	tcpipAutoConfigIPv6.setLabel("IPv6")
	tcpipAutoConfigIPv6.setVisible(True)
	tcpipAutoConfigIPv6.setDescription("Enable IPv6")
	tcpipAutoConfigIPv6.setDependencies(tcpipAutoConfigIPv6Enable, ["TCPIP_AUTOCONFIG_ENABLE_IPV6"])
	
	# Enable NDP
	tcpipAutoConfigNDP = tcpipAutoConfigNetworkComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_NDP", None)
	tcpipAutoConfigNDP.setLabel("NDP")
	tcpipAutoConfigNDP.setVisible(True)
	tcpipAutoConfigNDP.setDescription("Enable NDP")	
	tcpipAutoConfigNDP.setDependencies(tcpipAutoConfigNDPEnable, ["TCPIP_AUTOCONFIG_ENABLE_NDP"])
	
#######################################################################################################
def enableTcpipAutoConfigNet(enable):
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
			
		if(Database.getComponentByID("tcpip_transport_config") == None):
			res = tcpipAutoConfigTransportGroup.addComponent("tcpip_transport_config")
			res = Database.activateComponents(["tcpip_transport_config"], "TRANSPORT LAYER", False)	
			
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

#################Business Logic- Network Layer #########################################
def tcpipAutoConfigARPEnable(symbol, event):	
	enableTcpipAutoConfigNet(True)	
	if (event["value"] == True):		
		res = Database.activateComponents(["tcpipArp"],"NETWORK LAYER")	
		tcpipAutoConfigNetworkGroup.setAttachmentVisible("tcpipArp", "libtcpipArp")
	else:		
		res = Database.deactivateComponents(["tcpipArp"])	

def tcpipAutoConfigICMPEnable(symbol, event):
	enableTcpipAutoConfigNet(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipIcmp"], "NETWORK LAYER")	
		tcpipAutoConfigNetworkGroup.setAttachmentVisible("tcpipIcmp", "libtcpipIcmp")
		if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
			Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipIcmp"])

def tcpipAutoConfigIGMPEnable(symbol, event):
	enableTcpipAutoConfigNet(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipIgmp"],"NETWORK LAYER")	
		tcpipAutoConfigNetworkGroup.setAttachmentVisible("tcpipIgmp", "libtcpipIgmp")
		if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
			Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipIgmp"])
	
def tcpipAutoConfigIPv4Enable(symbol, event):
	enableTcpipAutoConfigNet(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipIPv4"],"NETWORK LAYER")	
		tcpipAutoConfigNetworkGroup.setAttachmentVisible("tcpipIPv4", "libTcpipIPv4")
		if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_ARP") != True):
			Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_ARP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipIPv4"])
	
def tcpipAutoConfigIPv6Enable(symbol, event):
	enableTcpipAutoConfigNet(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipIPv6"],"NETWORK LAYER")	
		tcpipAutoConfigNetworkGroup.setAttachmentVisible("tcpipIPv6", "libTcpipIPv6")
		if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_NDP") != True):
			Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_NDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipIPv6"])
	
def tcpipAutoConfigNDPEnable(symbol, event):
	enableTcpipAutoConfigNet(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipNdp"],"NETWORK LAYER")	
		tcpipAutoConfigNetworkGroup.setAttachmentVisible("tcpipNdp", "libtcpipNdp")
	else:
		res = Database.deactivateComponents(["tcpipNdp"])


	
