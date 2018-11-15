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
def instantiateComponent(tcpipAutoConfigDriverComponent):

	# Enable GMAC
	tcpipAutoConfigGMAC = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_GMAC", None)
	tcpipAutoConfigGMAC.setLabel("Enable GMAC")
	tcpipAutoConfigGMAC.setVisible(True)
	tcpipAutoConfigGMAC.setDescription("Enable GMAC")
	tcpipAutoConfigGMAC.setDependencies(tcpipAutoConfigGMACEnable, ["TCPIP_AUTOCONFIG_ENABLE_GMAC"])	

	# Enable MIIM_Driver
	tcpipAutoConfigMIIM_Driver = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", None)
	tcpipAutoConfigMIIM_Driver.setLabel("Enable MIIM_Driver")
	tcpipAutoConfigMIIM_Driver.setVisible(True)
	tcpipAutoConfigMIIM_Driver.setDescription("Enable MIIM_Driver")
	tcpipAutoConfigMIIM_Driver.setDependencies(tcpipAutoConfigMIIMDriverEnable, ["TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver"])		
	
	# Enable KSZ8061
	tcpipAutoConfigKSZ8061 = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_KSZ8061", None)
	tcpipAutoConfigKSZ8061.setLabel("Enable KSZ8061")
	tcpipAutoConfigKSZ8061.setVisible(True)
	tcpipAutoConfigKSZ8061.setDescription("Enable KSZ8061")	
	tcpipAutoConfigKSZ8061.setDependencies(tcpipAutoConfigKSZ8061Enable, ["TCPIP_AUTOCONFIG_ENABLE_KSZ8061"])	

	# Enable LAN8740
	tcpipAutoConfigLAN8740 = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_LAN8740", None)
	tcpipAutoConfigLAN8740.setLabel("Enable LAN8740")
	tcpipAutoConfigLAN8740.setVisible(True)
	tcpipAutoConfigLAN8740.setDescription("Enable LAN8740")
	tcpipAutoConfigLAN8740.setDependencies(tcpipAutoConfigLAN8740Enable, ["TCPIP_AUTOCONFIG_ENABLE_LAN8740"])		
	
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
		tcpipAutoConfigNetworkGroup = Database.createGroup("TCP/IP STACK", "NETWORK LAYER")
		tcpipAutoConfigBasicGroup = Database.createGroup("TCP/IP STACK", "TCP/IP BASIC CONFIGURATION")
		
		if(Database.getComponentByID("tcpip_apps_config") == None):
			res = tcpipAutoConfigAppsGroup.addComponent("tcpip_apps_config")
			res = Database.activateComponents(["tcpip_apps_config"], "APPLICATION LAYER")	
			Database.setActiveGroup("DRIVER LAYER")
			
		if(Database.getComponentByID("tcpip_transport_config") == None):
			res = tcpipAutoConfigTransportGroup.addComponent("tcpip_transport_config")
			res = Database.activateComponents(["tcpip_transport_config"], "TRANSPORT LAYER")	
			Database.setActiveGroup("DRIVER LAYER")
		
		if(Database.getComponentByID("tcpip_network_config") == None):
			res = tcpipAutoConfigNetworkGroup.addComponent("tcpip_network_config")
			res = Database.activateComponents(["tcpip_network_config"], "NETWORK LAYER")	
			Database.setActiveGroup("DRIVER LAYER")
			
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


#################Business Logic- Driver Layer #########################################	
def tcpipAutoConfigGMACEnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["drvSamv71Gmac"],"DRIVER LAYER")	
		tcpipAutoConfigDriverGroup.setNodeVisible("drvSamv71Gmac", "libdrvSamv71Gmac")
	else:
		res = Database.deactivateComponents(["drvSamv71Gmac"])
	
def tcpipAutoConfigETHMACEnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["drvPic32mEthmac"],"DRIVER LAYER")	
		tcpipAutoConfigDriverGroup.setNodeVisible("drvPic32mEthmac", "libdrvPic32mEthmac")
	else:
		res = Database.deactivateComponents(["drvPic32mEthmac"])
	
	
def tcpipAutoConfigMIIMDriverEnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["drvMiim"],"DRIVER LAYER")	
	else:
		res = Database.deactivateComponents(["drvMiim"])
	
def tcpipAutoConfigKSZ8061Enable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["drvExtPhyKsz8061"],"DRIVER LAYER")	
		Database.setSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", True, 2)		
	else:
		res = Database.deactivateComponents(["drvExtPhyKsz8061"])
	
	
def tcpipAutoConfigLAN8740Enable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["drvExtPhyLan8740"],"DRIVER LAYER")	
		Database.setSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", True, 2)		
	else:
		res = Database.deactivateComponents(["drvExtPhyLan8740"])
	
