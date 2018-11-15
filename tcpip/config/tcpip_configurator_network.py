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
global isEnabled
isEnabled = False
autoConnectTableFS = [["tcpipSysFsWrapper", "TcpipFsWarapper_SysFS_Dependency", "sys_fs", "sys_fs"]]
autoConnectTableTIME = [["tcpipStack", "Core_SysTime_Dependency", "sys_time", "sys_time"]]
autoConnectTableCRYPTOHttp = [["tcpipHttp", "Http_Crypto_Dependency", "lib_crypto", "lib_crypto"]]
autoConnectTableCRYPTOHttpNet = [["tcpipHttpNet", "HttpNet_Crypto_Dependency", "lib_crypto", "lib_crypto"]]
############################################################################
#### Code Generation ####
############################################################################
def instantiateComponent(tcpipAutoConfigNetworkComponent):
	
	# Enable ARP
	tcpipAutoConfigARP = tcpipAutoConfigNetworkComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_ARP", None)
	tcpipAutoConfigARP.setLabel("Enable ARP")
	tcpipAutoConfigARP.setVisible(True)
	tcpipAutoConfigARP.setDescription("Enable ARP")	
	tcpipAutoConfigARP.setDependencies(tcpipAutoConfigARPEnable, ["TCPIP_AUTOCONFIG_ENABLE_ARP"])
	
	# Enable ICMPv4
	tcpipAutoConfigICMPv4 = tcpipAutoConfigNetworkComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_ICMPv4", None)
	tcpipAutoConfigICMPv4.setLabel("Enable ICMPv4")
	tcpipAutoConfigICMPv4.setVisible(True)
	tcpipAutoConfigICMPv4.setDescription("Enable ICMPv4")
	tcpipAutoConfigICMPv4.setDependencies(tcpipAutoConfigICMPEnable, ["TCPIP_AUTOCONFIG_ENABLE_ICMPv4"])
	
	# Enable IGMP
	tcpipAutoConfigIGMP = tcpipAutoConfigNetworkComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_IGMP", None)
	tcpipAutoConfigIGMP.setLabel("Enable IGMP")
	tcpipAutoConfigIGMP.setVisible(True)
	tcpipAutoConfigIGMP.setDescription("Enable IGMP")	
	tcpipAutoConfigIGMP.setDependencies(tcpipAutoConfigIGMPEnable, ["TCPIP_AUTOCONFIG_ENABLE_IGMP"])
	
	# Enable IPv4
	tcpipAutoConfigIPv4 = tcpipAutoConfigNetworkComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_IPV4", None)
	tcpipAutoConfigIPv4.setLabel("Enable IPv4")
	tcpipAutoConfigIPv4.setVisible(True)
	tcpipAutoConfigIPv4.setDescription("Enable IPv4")
	tcpipAutoConfigIPv4.setDependencies(tcpipAutoConfigIPv4Enable, ["TCPIP_AUTOCONFIG_ENABLE_IPV4"])
	
	# Enable IPv6
	tcpipAutoConfigIPv6 = tcpipAutoConfigNetworkComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_IPV6", None)
	tcpipAutoConfigIPv6.setLabel("Enable IPv6")
	tcpipAutoConfigIPv6.setVisible(True)
	tcpipAutoConfigIPv6.setDescription("Enable IPv6")
	tcpipAutoConfigIPv6.setDependencies(tcpipAutoConfigIPv6Enable, ["TCPIP_AUTOCONFIG_ENABLE_IPV6"])
	
	# Enable NDP
	tcpipAutoConfigNDP = tcpipAutoConfigNetworkComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_NDP", None)
	tcpipAutoConfigNDP.setLabel("Enable NDP")
	tcpipAutoConfigNDP.setVisible(True)
	tcpipAutoConfigNDP.setDescription("Enable NDP")	
	tcpipAutoConfigNDP.setDependencies(tcpipAutoConfigNDPEnable, ["TCPIP_AUTOCONFIG_ENABLE_NDP"])	
	#tcpipIPv4Frag.setDefaultValue(False)
	#tcpipIPv4Frag.setDependencies(tcpipIPv4MenuVisible, ["TCPIP_STACK_USE_IPV4"])
	
#######################################################################################################
def enableTcpipAutoConfig(enable):
	global tcpipAutoConfigAppsGroup
	global tcpipAutoConfigTransportGroup
	global tcpipAutoConfigNetworkGroup	
	global tcpipAutoConfigDriverGroup
	global tcpipAutoConfigStackGroup
	global tcpipAutoConfigBasicGroup
	global isEnabled
	if(enable == True):
		tcpipAutoConfigAppsGroup = Database.createGroup("TCP/IP STACK", "APPLICATION LAYER")
		tcpipAutoConfigTransportGroup = Database.createGroup("TCP/IP STACK", "TRANSPORT LAYER")	
		tcpipAutoConfigDriverGroup = Database.createGroup("TCP/IP STACK", "DRIVER LAYER")
		tcpipAutoConfigBasicGroup = Database.createGroup("TCP/IP STACK", "TCP/IP BASIC CONFIGURATION")
		
		if(Database.getComponentByID("tcpip_apps_config") == None):
			res = tcpipAutoConfigAppsGroup.addComponent("tcpip_apps_config")
			res = Database.activateComponents(["tcpip_apps_config"], "APPLICATION LAYER")	
			Database.setActiveGroup("NETWORK LAYER")
			
		if(Database.getComponentByID("tcpip_transport_config") == None):
			res = tcpipAutoConfigTransportGroup.addComponent("tcpip_transport_config")
			res = Database.activateComponents(["tcpip_transport_config"], "TRANSPORT LAYER")	
			Database.setActiveGroup("NETWORK LAYER")
			
		if(Database.getComponentByID("tcpip_driver_config") == None):
			res = tcpipAutoConfigDriverGroup.addComponent("tcpip_driver_config")
			res = Database.activateComponents(["tcpip_driver_config"], "DRIVER LAYER")	
			Database.setActiveGroup("NETWORK LAYER")
			
		# if (isEnabled != True):	
			# res = Database.activateComponents(["HarmonyCore"])
			# tcpipAutoConfigStackGroup = Database.createGroup(None, "TCP/IP STACK")
			# tcpipAutoConfigAppsGroup = Database.createGroup("TCP/IP STACK", "APPLICATION LAYER")
			# tcpipAutoConfigTransportGroup = Database.createGroup("TCP/IP STACK", "TRANSPORT LAYER")
			# tcpipAutoConfigNetworkGroup = Database.createGroup("TCP/IP STACK", "NETWORK LAYER")	
			# tcpipAutoConfigDriverGroup = Database.createGroup("TCP/IP STACK", "DRIVER LAYER")
			# tcpipAutoConfigBasicGroup = Database.createGroup("TCP/IP STACK", "TCP/IP BASIC CONFIGURATION")
			
			# res = Database.activateComponents(["tcpipStack"], "TCP/IP BASIC CONFIGURATION")
			# tcpipAutoConfigBasicGroup.setNodeVisible("tcpipStack", "Core_SysTime_Dependency")
			# tcpipAutoConfigBasicGroup.setNodeVisible("tcpipStack", "libtcpipStack")
			# res = Database.activateComponents(["tcpipNetConfig"], "TCP/IP BASIC CONFIGURATION")
			# tcpipAutoConfigBasicGroup.setNodeVisible("tcpipNetConfig", "NETCONFIG_MAC_Dependency") #niyas: not working 
			# tcpipAutoConfigBasicGroup.setNodeVisible("tcpipNetConfig", "libtcpipNetConfig")			
			
			# tcpipAutoConfigStackGroup.setNodeVisible("tcpipStack", "Core_SysTime_Dependency") #niyas: not working 

			# rootGroup = Database.getRootGroup()
			# rootGroup.addComponent("sys_time")
			# res = Database.activateComponents(["sys_time"])
			
			# res = Database.connectDependencies(autoConnectTableTIME)

			# isEnabled = True

#################Business Logic- Network Layer #########################################
def tcpipAutoConfigARPEnable(symbol, event):	
	enableTcpipAutoConfig(True)	
	if (event["value"] == True):		
		res = Database.activateComponents(["tcpipArp"],"NETWORK LAYER")	
		tcpipAutoConfigNetworkGroup.setNodeVisible("tcpipArp", "libtcpipArp")
	else:		
		res = Database.deactivateComponents(["tcpipArp"])	

def tcpipAutoConfigICMPEnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipIcmp"], "NETWORK LAYER")	
		tcpipAutoConfigNetworkGroup.setNodeVisible("tcpipIcmp", "libtcpipIcmp")
		Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipIcmp"])

def tcpipAutoConfigIGMPEnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipIgmp"],"NETWORK LAYER")	
		tcpipAutoConfigNetworkGroup.setNodeVisible("tcpipIgmp", "libtcpipIgmp")
		Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipIgmp"])
	
def tcpipAutoConfigIPv4Enable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipIPv4"],"NETWORK LAYER")	
		tcpipAutoConfigNetworkGroup.setNodeVisible("tcpipIPv4", "libTcpipIPv4")
		Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_ARP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipIPv4"])
	
def tcpipAutoConfigIPv6Enable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipIPv6"],"NETWORK LAYER")	
		tcpipAutoConfigNetworkGroup.setNodeVisible("tcpipIPv6", "libTcpipIPv6")
		Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_NDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipIPv6"])
	
def tcpipAutoConfigNDPEnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipNdp"],"NETWORK LAYER")	
		tcpipAutoConfigNetworkGroup.setNodeVisible("tcpipNdp", "libtcpipNdp")
	else:
		res = Database.deactivateComponents(["tcpipNdp"])


	
