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
def instantiateComponent(tcpipAutoConfigTransportComponent):

	global tcpipAutoConfigStackGroup
	global tcpipAutoConfigAppsGroup
	global tcpipAutoConfigTransportGroup
	global tcpipAutoConfigNetworkGroup	
	global tcpipAutoConfigDriverGroup
	global tcpipAutoConfigBasicGroup

	tcpipAutoConfigStackGroup = Database.createGroup(None, "TCP/IP STACK")
	tcpipAutoConfigAppsGroup = Database.createGroup("TCP/IP STACK", "TRANSPORT LAYER")	
	
	Database.setActiveGroup("TRANSPORT LAYER")
	
	# Enable TCP
	tcpipAutoConfigTCP = tcpipAutoConfigTransportComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_TCP", None)
	tcpipAutoConfigTCP.setLabel("Enable TCP")
	tcpipAutoConfigTCP.setVisible(True)
	tcpipAutoConfigTCP.setDescription("Enable TCP")	
	tcpipAutoConfigTCP.setDependencies(tcpipAutoConfigTCPEnable, ["TCPIP_AUTOCONFIG_ENABLE_TCP"])
	
	# Enable UDP
	tcpipAutoConfigUDP = tcpipAutoConfigTransportComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_UDP", None)
	tcpipAutoConfigUDP.setLabel("Enable UDP")
	tcpipAutoConfigUDP.setVisible(True)
	tcpipAutoConfigUDP.setDescription("Enable UDP")	
	tcpipAutoConfigUDP.setDependencies(tcpipAutoConfigUDPEnable, ["TCPIP_AUTOCONFIG_ENABLE_UDP"])
	
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
		tcpipAutoConfigNetworkGroup = Database.createGroup("TCP/IP STACK", "NETWORK LAYER")	
		tcpipAutoConfigDriverGroup = Database.createGroup("TCP/IP STACK", "DRIVER LAYER")
		tcpipAutoConfigBasicGroup = Database.createGroup("TCP/IP STACK", "TCP/IP BASIC CONFIGURATION")
		if(Database.getComponentByID("tcpip_apps_config") == None):
			res = tcpipAutoConfigAppsGroup.addComponent("tcpip_apps_config")
			res = Database.activateComponents(["tcpip_apps_config"], "APPLICATION LAYER")	
			Database.setActiveGroup("TRANSPORT LAYER")
		
		if(Database.getComponentByID("tcpip_network_config") == None):
			res = tcpipAutoConfigNetworkGroup.addComponent("tcpip_network_config")
			res = Database.activateComponents(["tcpip_network_config"], "NETWORK LAYER")	
			Database.setActiveGroup("TRANSPORT LAYER")
			
		if(Database.getComponentByID("tcpip_driver_config") == None):
			res = tcpipAutoConfigDriverGroup.addComponent("tcpip_driver_config")
			res = Database.activateComponents(["tcpip_driver_config"], "DRIVER LAYER")	
			Database.setActiveGroup("TRANSPORT LAYER")
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


#################Business Logic- Transport Layer #########################################	
def tcpipAutoConfigTCPEnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipTcp"], "TRANSPORT LAYER")	
		tcpipAutoConfigTransportGroup.setNodeVisible("tcpipTcp", "libtcpipTcp")
	else:
		res = Database.deactivateComponents(["tcpipTcp"])
	
def tcpipAutoConfigUDPEnable(symbol, event):
	#enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipUdp"], "TRANSPORT LAYER")	
		tcpipAutoConfigTransportGroup.setNodeVisible("tcpipUdp", "libtcpipUdp")		
	else:
		res = Database.deactivateComponents(["tcpipUdp"])
	
	
