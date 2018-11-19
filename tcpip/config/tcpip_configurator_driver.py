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
def instantiateComponent(tcpipAutoConfigDriverComponent):
	global tcpipAutoConfigStackGroup
	global tcpipAutoConfigDriverGroup

	tcpipAutoConfigStackGroup = Database.getGroup("TCP/IP STACK")
	if (tcpipAutoConfigStackGroup == None):
		tcpipAutoConfigStackGroup = Database.createGroup(None, "TCP/IP STACK")
		
	tcpipAutoConfigDriverGroup = Database.getGroup("DRIVER LAYER")
	if (tcpipAutoConfigDriverGroup == None):
		tcpipAutoConfigDriverGroup = Database.createGroup("TCP/IP STACK", "DRIVER LAYER")

	# Enable Driver Configurator
	tcpipAutoConfigDrvEnable = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_DRV_ENABLE", None)
	tcpipAutoConfigDrvEnable.setVisible(False)
	tcpipAutoConfigDrvEnable.setDefaultValue(True)

	if(Database.getSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_APPS_ENABLE") != True) and (Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_TRANS_ENABLE") != True) and (Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_NET_ENABLE") != True) and (Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_BASIC_ENABLE") != True):
		Database.setActiveGroup("DRIVER LAYER")
		
	# Enable GMAC
	tcpipAutoConfigGMAC = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_GMAC", None)
	tcpipAutoConfigGMAC.setLabel("GMAC")
	tcpipAutoConfigGMAC.setVisible(True)
	tcpipAutoConfigGMAC.setDescription("Enable GMAC")
	tcpipAutoConfigGMAC.setDependencies(tcpipAutoConfigGMACEnable, ["TCPIP_AUTOCONFIG_ENABLE_GMAC"])	

	# Enable MIIM_Driver
	tcpipAutoConfigMIIM_Driver = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", None)
	tcpipAutoConfigMIIM_Driver.setLabel("MIIM_Driver")
	tcpipAutoConfigMIIM_Driver.setVisible(True)
	tcpipAutoConfigMIIM_Driver.setDescription("Enable MIIM_Driver")
	tcpipAutoConfigMIIM_Driver.setDependencies(tcpipAutoConfigMIIMDriverEnable, ["TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver"])		
	
	# Enable KSZ8061
	tcpipAutoConfigKSZ8061 = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_KSZ8061", None)
	tcpipAutoConfigKSZ8061.setLabel("KSZ8061")
	tcpipAutoConfigKSZ8061.setVisible(True)
	tcpipAutoConfigKSZ8061.setDescription("Enable KSZ8061")	
	tcpipAutoConfigKSZ8061.setDependencies(tcpipAutoConfigKSZ8061Enable, ["TCPIP_AUTOCONFIG_ENABLE_KSZ8061"])	

	# Enable LAN8740
	tcpipAutoConfigLAN8740 = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_LAN8740", None)
	tcpipAutoConfigLAN8740.setLabel("LAN8740")
	tcpipAutoConfigLAN8740.setVisible(True)
	tcpipAutoConfigLAN8740.setDescription("Enable LAN8740")
	tcpipAutoConfigLAN8740.setDependencies(tcpipAutoConfigLAN8740Enable, ["TCPIP_AUTOCONFIG_ENABLE_LAN8740"])		
	
#######################################################################################################
def enableTcpipAutoConfigDrv(enable):
	global tcpipAutoConfigAppsGroup
	global tcpipAutoConfigTransportGroup
	global tcpipAutoConfigNetworkGroup	
	global tcpipAutoConfigDriverGroup
	global tcpipAutoConfigStackGroup
	global tcpipAutoConfigBasicGroup
	global isEnabled
	if(enable == True):
		tcpipAutoConfigAppsGroup = Database.getGroup("APPLICATION LAYER")
		if (tcpipAutoConfigAppsGroup == None):
			tcpipAutoConfigAppsGroup = Database.createGroup("TCP/IP STACK", "APPLICATION LAYER")
			
		tcpipAutoConfigTransportGroup = Database.getGroup("TRANSPORT LAYER")
		if (tcpipAutoConfigTransportGroup == None):
			tcpipAutoConfigTransportGroup = Database.createGroup("TCP/IP STACK", "TRANSPORT LAYER")

		tcpipAutoConfigNetworkGroup = Database.getGroup("NETWORK LAYER")
		if (tcpipAutoConfigNetworkGroup == None):
			tcpipAutoConfigNetworkGroup = Database.createGroup("TCP/IP STACK", "NETWORK LAYER")
			
		tcpipAutoConfigDriverGroup = Database.getGroup("DRIVER LAYER")
		if (tcpipAutoConfigDriverGroup == None):
			tcpipAutoConfigDriverGroup = Database.createGroup("TCP/IP STACK", "DRIVER LAYER")

		tcpipAutoConfigBasicGroup = Database.getGroup("BASIC CONFIGURATION")
		if (tcpipAutoConfigBasicGroup == None):
			tcpipAutoConfigBasicGroup = Database.createGroup("TCP/IP STACK", "BASIC CONFIGURATION")
		
		if(Database.getComponentByID("tcpip_apps_config") == None):
			res = tcpipAutoConfigAppsGroup.addComponent("tcpip_apps_config")
			res = Database.activateComponents(["tcpip_apps_config"], "APPLICATION LAYER")
			
		if(Database.getComponentByID("tcpip_transport_config") == None):
			res = tcpipAutoConfigTransportGroup.addComponent("tcpip_transport_config")
			res = Database.activateComponents(["tcpip_transport_config"], "TRANSPORT LAYER")
		
		if(Database.getComponentByID("tcpip_network_config") == None):
			res = tcpipAutoConfigNetworkGroup.addComponent("tcpip_network_config")
			res = Database.activateComponents(["tcpip_network_config"], "NETWORK LAYER")
			
		if(Database.getComponentByID("tcpip_basic_config") == None):
			res = tcpipAutoConfigBasicGroup.addComponent("tcpip_basic_config")
			res = Database.activateComponents(["tcpip_basic_config"], "BASIC CONFIGURATION")			
			
		if(Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_STACK") != True)
			Database.setSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_STACK", True, 2)
			
		if(Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_NETCONFIG") != True)
			Database.setSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_NETCONFIG", True, 2)				

#################Business Logic- Driver Layer #########################################	
def tcpipAutoConfigGMACEnable(symbol, event):
	enableTcpipAutoConfigDrv(True)
	if (event["value"] == True):
		res = Database.activateComponents(["drvSamv71Gmac"],"DRIVER LAYER")	
		tcpipAutoConfigDriverGroup.setNodeVisible("drvSamv71Gmac", "libdrvSamv71Gmac")
	else:
		res = Database.deactivateComponents(["drvSamv71Gmac"])
	
def tcpipAutoConfigETHMACEnable(symbol, event):
	enableTcpipAutoConfigDrv(True)
	if (event["value"] == True):
		res = Database.activateComponents(["drvPic32mEthmac"],"DRIVER LAYER")	
		tcpipAutoConfigDriverGroup.setNodeVisible("drvPic32mEthmac", "libdrvPic32mEthmac")
	else:
		res = Database.deactivateComponents(["drvPic32mEthmac"])
	
	
def tcpipAutoConfigMIIMDriverEnable(symbol, event):
	enableTcpipAutoConfigDrv(True)
	if (event["value"] == True):
		res = Database.activateComponents(["drvMiim"],"DRIVER LAYER")	
	else:
		res = Database.deactivateComponents(["drvMiim"])
	
def tcpipAutoConfigKSZ8061Enable(symbol, event):
	enableTcpipAutoConfigDrv(True)
	if (event["value"] == True):
		res = Database.activateComponents(["drvExtPhyKsz8061"],"DRIVER LAYER")	
		if(Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver") != True)
			Database.setSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", True, 2)		
	else:
		res = Database.deactivateComponents(["drvExtPhyKsz8061"])
	
	
def tcpipAutoConfigLAN8740Enable(symbol, event):
	enableTcpipAutoConfigDrv(True)
	if (event["value"] == True):
		res = Database.activateComponents(["drvExtPhyLan8740"],"DRIVER LAYER")	
		if(Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver") != True)
			Database.setSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", True, 2)		
	else:
		res = Database.deactivateComponents(["drvExtPhyLan8740"])
	
