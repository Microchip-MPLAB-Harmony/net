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

# autoConnectTableFS = [["tcpipSysFsWrapper", "TcpipFsWarapper_SysFS_Dependency", "sys_fs", "sys_fs"]]
autoConnectTableFS = [["TCP/IP STACK", "BASIC CONFIGURATION:tcpipSysFsWrapper:TcpipFsWarapper_SysFS_Dependency", "sys_fs", "sys_fs"]]
# autoConnectTableTIME = [["tcpipStack", "Core_SysTime_Dependency", "sys_time", "sys_time"]]
autoConnectTableTIME = [["TCP/IP STACK", "BASIC CONFIGURATION:tcpipStack:Core_SysTime_Dependency", "sys_time", "sys_time"]]
autoConnectTableConsole = [["TCP/IP STACK", "BASIC CONFIGURATION:tcpipCmd:Cmd_SysConsole_Dependency", "sys_console", "sys_console"]]

############################################################################
#### Code Generation ####
############################################################################
def instantiateComponent(tcpipAutoConfigBasicComponent):

	tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
	if (tcpipAutoConfigStackGroup == None):
		tcpipAutoConfigStackGroup = Database.createGroup(None, "TCP/IP STACK")
		
	tcpipAutoConfigBasicGroup = Database.findGroup("BASIC CONFIGURATION")
	if (tcpipAutoConfigBasicGroup == None):
		tcpipAutoConfigBasicGroup = Database.createGroup("TCP/IP STACK", "BASIC CONFIGURATION")		
	
	# Enable Transport Configurator
	tcpipAutoConfigBasicEnable = tcpipAutoConfigBasicComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_BASIC_ENABLE", None)
	tcpipAutoConfigBasicEnable.setVisible(False)
	tcpipAutoConfigBasicEnable.setDefaultValue(True)
	
	# Enable TCP/IP STACK
	tcpipAutoConfigStack = tcpipAutoConfigBasicComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_STACK", None)
	tcpipAutoConfigStack.setLabel("Basic STACK Configuration")
	tcpipAutoConfigStack.setVisible(True)
	tcpipAutoConfigStack.setDescription("Enable STACK Configuration")	
	tcpipAutoConfigStack.setDependencies(tcpipAutoConfigStackEnable, ["TCPIP_AUTOCONFIG_ENABLE_STACK"])
	
	# Enable Netconfig
	tcpipAutoConfigNetConfig = tcpipAutoConfigBasicComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_NETCONFIG", None)
	tcpipAutoConfigNetConfig.setLabel("Network Configuration")
	tcpipAutoConfigNetConfig.setVisible(True)
	tcpipAutoConfigNetConfig.setDescription("Enable Network Configuration")	
	tcpipAutoConfigNetConfig.setDependencies(tcpipAutoConfigNetConfigEnable, ["TCPIP_AUTOCONFIG_ENABLE_NETCONFIG"])
	
	# Enable TCPIP SysFS Wrapper
	tcpipAutoConfigSysFSWrapper = tcpipAutoConfigBasicComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_SysFSWrapper", None)
	tcpipAutoConfigSysFSWrapper.setLabel("TCP/IP SysFS Wrapper")
	tcpipAutoConfigSysFSWrapper.setVisible(True)
	tcpipAutoConfigSysFSWrapper.setDescription("Enable TCP/IP SysFS Wrapper")	
	tcpipAutoConfigSysFSWrapper.setDependencies(tcpipAutoConfigSysFSWrapperEnable, ["TCPIP_AUTOCONFIG_ENABLE_SysFSWrapper"])	
	
	# Enable TCPIP CMD
	tcpipAutoConfigTcpipCmd = tcpipAutoConfigBasicComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_TCPIPCMD", None)
	tcpipAutoConfigTcpipCmd.setLabel("TCPIP CMD")
	tcpipAutoConfigTcpipCmd.setVisible(True)
	tcpipAutoConfigTcpipCmd.setDescription("Enable TCPIP CMD")	
	tcpipAutoConfigTcpipCmd.setDependencies(tcpipAutoConfigTcpipCmdEnable, ["TCPIP_AUTOCONFIG_ENABLE_TCPIPCMD"])
	
########################################################################################################
def finalizeComponent(tcpipAutoConfigBasicComponent):
	tcpipAutoConfigBasicGroup = Database.findGroup("BASIC CONFIGURATION")
	tcpipAutoConfigBasicGroup.addComponent(tcpipAutoConfigBasicComponent.getID())	
	
	if(Database.getSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_APPS_ENABLE") != True) and (Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_TRANS_ENABLE") != True) and (Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_NET_ENABLE") != True) and (Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_DRV_ENABLE") != True):
		Database.setActiveGroup("BASIC CONFIGURATION")
#######################################################################################################
def enableTcpipAutoConfigBasic(enable):

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
			
		if(Database.getComponentByID("tcpip_network_config") == None):
			res = tcpipAutoConfigNetworkGroup.addComponent("tcpip_network_config")
			res = Database.activateComponents(["tcpip_network_config"], "NETWORK LAYER", False)
			
		if(Database.getComponentByID("tcpip_driver_config") == None):
			res = tcpipAutoConfigDriverGroup.addComponent("tcpip_driver_config")
			res = Database.activateComponents(["tcpip_driver_config"], "DRIVER LAYER", False)	
			
		# if(Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_STACK") != True):
			# Database.setSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_STACK", True, 2)
			
		# if(Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_NETCONFIG") != True):
			# Database.setSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_NETCONFIG", True, 2)	


#################Business Logic- Transport Layer #########################################	
def tcpipAutoConfigStackEnable(symbol, event):
	tcpipAutoConfigBasicGroup = Database.findGroup("BASIC CONFIGURATION")
	tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
	enableTcpipAutoConfigBasic(True)
	if (event["value"] == True):	
		# res = Database.activateComponents(["HarmonyCore"])
		# res = Database.activateComponents(["FreeRTOS"])
		res = Database.activateComponents(["tcpipStack"], "BASIC CONFIGURATION", False)	
		tcpipAutoConfigBasicGroup.setAttachmentVisible("tcpipStack", "libtcpipStack")
		tcpipAutoConfigBasicGroup.setAttachmentVisible("tcpipStack", "Core_NetConfig_Dependency")
		tcpipAutoConfigBasicGroup.setAttachmentVisible("tcpipStack", "Core_SysTime_Dependency")
		tcpipAutoConfigStackGroup.setAttachmentVisible("BASIC CONFIGURATION", "tcpipStack:Core_SysTime_Dependency")
		
		if(Database.getComponentByID("sys_time") == None):
			# res = Database.activateComponents(["sys_time"], Database.getRootGroup().getID(), True)
			res = Database.activateComponents(["sys_time"]) 
			res = Database.connectDependencies(autoConnectTableTIME)
	else:
		res = Database.deactivateComponents(["tcpipStack"])
	
def tcpipAutoConfigNetConfigEnable(symbol, event):
	tcpipAutoConfigBasicGroup = Database.findGroup("BASIC CONFIGURATION")
	enableTcpipAutoConfigBasic(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipNetConfig"], "BASIC CONFIGURATION", False)
		tcpipAutoConfigBasicGroup.setAttachmentVisible("tcpipNetConfig_0", "NETCONFIG_MAC_Dependency")
		tcpipAutoConfigBasicGroup.setAttachmentVisible("tcpipNetConfig", "libtcpipNetConfig")	
	else:
		res = Database.deactivateComponents(["tcpipNetConfig"])
	
	
def tcpipAutoConfigSysFSWrapperEnable(symbol, event):
	tcpipAutoConfigBasicGroup = Database.findGroup("BASIC CONFIGURATION")
	tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
	enableTcpipAutoConfigBasic(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipSysFsWrapper"], "BASIC CONFIGURATION", False)
		tcpipAutoConfigBasicGroup.setAttachmentVisible("tcpipSysFsWrapper", "libtcpipSysFsWrapper")
		tcpipAutoConfigBasicGroup.setAttachmentVisible("tcpipSysFsWrapper", "TcpipFsWarapper_SysFS_Dependency")
		tcpipAutoConfigStackGroup.setAttachmentVisible("BASIC CONFIGURATION", "tcpipSysFsWrapper:TcpipFsWarapper_SysFS_Dependency")
		if(Database.getComponentByID("sys_fs") == None):
			# res = Database.activateComponents(["sys_fs"], Database.getRootGroup().getID(), True)	
			res = Database.activateComponents(["sys_fs"])
			res = Database.connectDependencies(autoConnectTableFS)
	else:
		res = Database.deactivateComponents(["tcpipSysFsWrapper"])	
	
def tcpipAutoConfigTcpipCmdEnable(symbol, event):
	tcpipAutoConfigBasicGroup = Database.findGroup("BASIC CONFIGURATION")
	tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
	enableTcpipAutoConfigBasic(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipCmd"], "BASIC CONFIGURATION", False)
		tcpipAutoConfigBasicGroup.setAttachmentVisible("tcpipCmd", "libtcpipCmd")
		tcpipAutoConfigBasicGroup.setAttachmentVisible("tcpipCmd", "Cmd_SysConsole_Dependency")
		tcpipAutoConfigStackGroup.setAttachmentVisible("BASIC CONFIGURATION", "tcpipCmd:Cmd_SysConsole_Dependency")
		if(Database.getComponentByID("sys_console") == None):
			res = Database.activateComponents(["sys_console"])	
			res = Database.connectDependencies(autoConnectTableConsole)
	else:
		res = Database.deactivateComponents(["tcpipCmd"])	