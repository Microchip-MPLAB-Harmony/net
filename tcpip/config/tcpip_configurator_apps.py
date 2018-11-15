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
def instantiateComponent(tcpipAutoConfigAppsComponent):
	global tcpipAutoConfigStackGroup
	global tcpipAutoConfigAppsGroup
	global tcpipAutoConfigTransportGroup
	global tcpipAutoConfigNetworkGroup	
	global tcpipAutoConfigDriverGroup
	global tcpipAutoConfigBasicGroup
	
	tcpipAutoConfigStackGroup = Database.createGroup(None, "TCP/IP STACK")
	tcpipAutoConfigAppsGroup = Database.createGroup("TCP/IP STACK", "APPLICATION LAYER")	
	# tcpipAutoConfigTransportGroup = Database.createGroup("TCP/IP STACK", "TRANSPORT LAYER")
	# tcpipAutoConfigNetworkGroup = Database.createGroup("TCP/IP STACK", "NETWORK LAYER")	
	# tcpipAutoConfigDriverGroup = Database.createGroup("TCP/IP STACK", "DRIVER LAYER")
	# tcpipAutoConfigBasicGroup = Database.createGroup("TCP/IP STACK", "TCP/IP BASIC CONFIGURATION")
	# print "Niyas Test"
	# print (str(Database.getComponentByID("tcpip_transport_config")))
	# if(Database.getComponentByID("tcpip_apps_config") == None):
		# res = Database.activateComponents(["tcpip_transport_config"], "TRANSPORT LAYER")	
	
	Database.setActiveGroup("APPLICATION LAYER")	
	# Enable ANNOUNCE
	tcpipAutoConfigANNOUNCE = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_ANNOUNCE", None)
	tcpipAutoConfigANNOUNCE.setLabel("Enable ANNOUNCE")
	tcpipAutoConfigANNOUNCE.setVisible(True)
	tcpipAutoConfigANNOUNCE.setDescription("Enable ANNOUNCE")
	tcpipAutoConfigANNOUNCE.setDependencies(tcpipAutoConfigANNOUNCEEnable, ["TCPIP_AUTOCONFIG_ENABLE_ANNOUNCE"])
	
	# Enable Berkeley_API
	tcpipAutoConfigBerkeley_API = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_Berkeley_API", None)
	tcpipAutoConfigBerkeley_API.setLabel("Enable Berkeley_API")
	tcpipAutoConfigBerkeley_API.setVisible(True)
	tcpipAutoConfigBerkeley_API.setDescription("Enable Berkeley_API")
	tcpipAutoConfigBerkeley_API.setDependencies(tcpipAutoConfigBerkeleyAPIEnable, ["TCPIP_AUTOCONFIG_ENABLE_Berkeley_API"])
	
	# Enable DDNS
	tcpipAutoConfigDDNS = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_DDNS", None)
	tcpipAutoConfigDDNS.setLabel("Enable DDNS")
	tcpipAutoConfigDDNS.setVisible(True)
	tcpipAutoConfigDDNS.setDescription("Enable DDNS")
	tcpipAutoConfigDDNS.setDependencies(tcpipAutoConfigDDNSEnable, ["TCPIP_AUTOCONFIG_ENABLE_DDNS"])

	# Enable DHCP_CLIENT
	tcpipAutoConfigDHCP_CLIENT = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_DHCP_CLIENT", None)
	tcpipAutoConfigDHCP_CLIENT.setLabel("Enable DHCP_CLIENT")
	tcpipAutoConfigDHCP_CLIENT.setVisible(True)
	tcpipAutoConfigDHCP_CLIENT.setDescription("Enable DHCP_CLIENT")	
	tcpipAutoConfigDHCP_CLIENT.setDependencies(tcpipAutoConfigDHCPCLIENTEnable, ["TCPIP_AUTOCONFIG_ENABLE_DHCP_CLIENT"])

	# Enable DHCP_SERVER
	tcpipAutoConfigDHCP_SERVER = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_DHCP_SERVER", None)
	tcpipAutoConfigDHCP_SERVER.setLabel("Enable DHCP_SERVER")
	tcpipAutoConfigDHCP_SERVER.setVisible(True)
	tcpipAutoConfigDHCP_SERVER.setDescription("Enable DHCP_SERVER")
	tcpipAutoConfigDHCP_SERVER.setDependencies(tcpipAutoConfigDHCPSERVEREnable, ["TCPIP_AUTOCONFIG_ENABLE_DHCP_SERVER"])

	# Enable DNS_CLIENT
	tcpipAutoConfigDNS_CLIENT = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_DNS_CLIENT", None)
	tcpipAutoConfigDNS_CLIENT.setLabel("Enable DNS_CLIENT")
	tcpipAutoConfigDNS_CLIENT.setVisible(True)
	tcpipAutoConfigDNS_CLIENT.setDescription("Enable DNS_CLIENT")
	tcpipAutoConfigDNS_CLIENT.setDependencies(tcpipAutoConfigDNSCLIENTEnable, ["TCPIP_AUTOCONFIG_ENABLE_DNS_CLIENT"])

	# Enable DNS_SERVER
	tcpipAutoConfigDNS_SERVER = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_DNS_SERVER", None)
	tcpipAutoConfigDNS_SERVER.setLabel("Enable DNS_SERVER")
	tcpipAutoConfigDNS_SERVER.setVisible(True)
	tcpipAutoConfigDNS_SERVER.setDescription("Enable DNS_SERVER")
	tcpipAutoConfigDNS_SERVER.setDependencies(tcpipAutoConfigDNSSERVEREnable, ["TCPIP_AUTOCONFIG_ENABLE_DNS_SERVER"])

	# Enable FTP_SERVER
	tcpipAutoConfigFTP_SERVER = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_FTP_SERVER", None)
	tcpipAutoConfigFTP_SERVER.setLabel("Enable FTP_SERVER")
	tcpipAutoConfigFTP_SERVER.setVisible(True)
	tcpipAutoConfigFTP_SERVER.setDescription("Enable FTP_SERVER")
	tcpipAutoConfigFTP_SERVER.setDependencies(tcpipAutoConfigFTPSERVEREnable, ["TCPIP_AUTOCONFIG_ENABLE_FTP_SERVER"])

	# Enable HTTP_NET_SERVER
	tcpipAutoConfigHTTP_NET_SERVER = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_HTTP_NET_SERVER", None)
	tcpipAutoConfigHTTP_NET_SERVER.setLabel("Enable HTTP_NET_SERVER")
	tcpipAutoConfigHTTP_NET_SERVER.setVisible(True)
	tcpipAutoConfigHTTP_NET_SERVER.setDescription("Enable HTTP_NET_SERVER")
	tcpipAutoConfigHTTP_NET_SERVER.setDependencies(tcpipAutoConfigHTTPNETSERVEREnable, ["TCPIP_AUTOCONFIG_ENABLE_HTTP_NET_SERVER"])

	# Enable HTTP_SERVER
	tcpipAutoConfigHTTP_SERVER = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_HTTP_SERVER", None)
	tcpipAutoConfigHTTP_SERVER.setLabel("Enable HTTP_SERVER")
	tcpipAutoConfigHTTP_SERVER.setVisible(True)
	tcpipAutoConfigHTTP_SERVER.setDescription("Enable HTTP_SERVER")	
	tcpipAutoConfigHTTP_SERVER.setDependencies(tcpipAutoConfigHTTPSERVEREnable, ["TCPIP_AUTOCONFIG_ENABLE_HTTP_SERVER"])

	# Enable IPERF
	tcpipAutoConfigIPERF = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_IPERF", None)
	tcpipAutoConfigIPERF.setLabel("Enable IPERF")
	tcpipAutoConfigIPERF.setVisible(True)
	tcpipAutoConfigIPERF.setDescription("Enable IPERF")	
	tcpipAutoConfigIPERF.setDependencies(tcpipAutoConfigIPERFEnable, ["TCPIP_AUTOCONFIG_ENABLE_IPERF"])
	
	# Enable NBNS
	tcpipAutoConfigNBNS = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_NBNS", None)
	tcpipAutoConfigNBNS.setLabel("Enable NBNS")
	tcpipAutoConfigNBNS.setVisible(True)
	tcpipAutoConfigNBNS.setDescription("Enable NBNS")	
	tcpipAutoConfigNBNS.setDependencies(tcpipAutoConfigNBNSEnable, ["TCPIP_AUTOCONFIG_ENABLE_NBNS"])

	# Enable REBOOT
	tcpipAutoConfigREBOOT = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_REBOOT", None)
	tcpipAutoConfigREBOOT.setLabel("Enable REBOOT")
	tcpipAutoConfigREBOOT.setVisible(True)
	tcpipAutoConfigREBOOT.setDescription("Enable REBOOT")
	tcpipAutoConfigREBOOT.setDependencies(tcpipAutoConfigREBOOTEnable, ["TCPIP_AUTOCONFIG_ENABLE_REBOOT"])

	# Enable SMTP_CLIENT
	tcpipAutoConfigSMTP_CLIENT = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_SMTP_CLIENT", None)
	tcpipAutoConfigSMTP_CLIENT.setLabel("Enable SMTP_CLIENT")
	tcpipAutoConfigSMTP_CLIENT.setVisible(True)
	tcpipAutoConfigSMTP_CLIENT.setDescription("Enable SMTP_CLIENT")
	tcpipAutoConfigSMTP_CLIENT.setDependencies(tcpipAutoConfigSMTPCLIENTEnable, ["TCPIP_AUTOCONFIG_ENABLE_SMTP_CLIENT"])
	
	# Enable SNMP
	tcpipAutoConfigSNMP = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_SNMP", None)
	tcpipAutoConfigSNMP.setLabel("Enable SNMP")
	tcpipAutoConfigSNMP.setVisible(True)
	tcpipAutoConfigSNMP.setDescription("Enable SNMP")
	tcpipAutoConfigSNMP.setDependencies(tcpipAutoConfigSNMPEnable, ["TCPIP_AUTOCONFIG_ENABLE_SNMP"])

	# Enable SNMPV3
	tcpipAutoConfigSNMPV3 = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_SNMPV3", None)
	tcpipAutoConfigSNMPV3.setLabel("Enable SNMPV3")
	tcpipAutoConfigSNMPV3.setVisible(True)
	tcpipAutoConfigSNMPV3.setDescription("Enable SNMPV3")
	tcpipAutoConfigSNMPV3.setDependencies(tcpipAutoConfigSNMPV3Enable, ["TCPIP_AUTOCONFIG_ENABLE_SNMPV3"])
	
	# Enable SNTP
	tcpipAutoConfigSNTP = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_SNTP", None)
	tcpipAutoConfigSNTP.setLabel("Enable SNTP")
	tcpipAutoConfigSNTP.setVisible(True)
	tcpipAutoConfigSNTP.setDescription("Enable SNTP")
	tcpipAutoConfigSNTP.setDependencies(tcpipAutoConfigSNTPEnable, ["TCPIP_AUTOCONFIG_ENABLE_SNTP"])

	# Enable TELNET
	tcpipAutoConfigTELNET = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_TELNET", None)
	tcpipAutoConfigTELNET.setLabel("Enable TELNET")
	tcpipAutoConfigTELNET.setVisible(True)
	tcpipAutoConfigTELNET.setDescription("Enable TELNET")
	tcpipAutoConfigTELNET.setDependencies(tcpipAutoConfigTELNETEnable, ["TCPIP_AUTOCONFIG_ENABLE_TELNET"])

	# Enable TFTP_CLIENT
	tcpipAutoConfigTFTP_CLIENT = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_TFTP_CLIENT", None)
	tcpipAutoConfigTFTP_CLIENT.setLabel("Enable TFTP_CLIENT")
	tcpipAutoConfigTFTP_CLIENT.setVisible(True)
	tcpipAutoConfigTFTP_CLIENT.setDescription("Enable TFTP_CLIENT")
	tcpipAutoConfigTFTP_CLIENT.setDependencies(tcpipAutoConfigTFTPCLIENTEnable, ["TCPIP_AUTOCONFIG_ENABLE_TFTP_CLIENT"])

	# Enable ZEROCONF
	tcpipAutoConfigZEROCONF = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_ZEROCONF", None)
	tcpipAutoConfigZEROCONF.setLabel("Enable ZEROCONF")
	tcpipAutoConfigZEROCONF.setVisible(True)
	tcpipAutoConfigZEROCONF.setDescription("Enable ZEROCONF")
	tcpipAutoConfigZEROCONF.setDependencies(tcpipAutoConfigZEROCONFEnable, ["TCPIP_AUTOCONFIG_ENABLE_ZEROCONF"])
	
	# Enable TCPIP_CMD
	tcpipAutoConfigTCPIP_CMD = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_TCPIP_CMD", None)
	tcpipAutoConfigTCPIP_CMD.setLabel("Enable TCPIP_CMD")
	tcpipAutoConfigTCPIP_CMD.setVisible(True)
	tcpipAutoConfigTCPIP_CMD.setDescription("Enable TCPIP_CMD")
	tcpipAutoConfigTCPIP_CMD.setDependencies(tcpipAutoConfigCMDEnable, ["TCPIP_AUTOCONFIG_ENABLE_TCPIP_CMD"])
	
	
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
		tcpipAutoConfigTransportGroup = Database.createGroup("TCP/IP STACK", "TRANSPORT LAYER")
		tcpipAutoConfigNetworkGroup = Database.createGroup("TCP/IP STACK", "NETWORK LAYER")	
		tcpipAutoConfigDriverGroup = Database.createGroup("TCP/IP STACK", "DRIVER LAYER")
		tcpipAutoConfigBasicGroup = Database.createGroup("TCP/IP STACK", "TCP/IP BASIC CONFIGURATION")
		if(Database.getComponentByID("tcpip_transport_config") == None):
			res = tcpipAutoConfigTransportGroup.addComponent("tcpip_transport_config")
			res = Database.activateComponents(["tcpip_transport_config"], "TRANSPORT LAYER")	
			Database.setActiveGroup("APPLICATION LAYER")
		
		if(Database.getComponentByID("tcpip_network_config") == None):
			res = tcpipAutoConfigNetworkGroup.addComponent("tcpip_network_config")
			res = Database.activateComponents(["tcpip_network_config"], "NETWORK LAYER")	
			Database.setActiveGroup("APPLICATION LAYER")
			
		if(Database.getComponentByID("tcpip_driver_config") == None):
			res = tcpipAutoConfigDriverGroup.addComponent("tcpip_driver_config")
			res = Database.activateComponents(["tcpip_driver_config"], "DRIVER LAYER")	
			Database.setActiveGroup("APPLICATION LAYER")		
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


#################Business Logic- Application Layer #########################################		
def tcpipAutoConfigANNOUNCEEnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipAnnounce"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipAnnounce", "libtcpipAnnounce")
		#Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
		#Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipAnnounce"])
	
def tcpipAutoConfigBerkeleyAPIEnable(symbol, event):
	#enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipBerkeleyApi"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipBerkeleyApi", "libtcpipBerkeleyApi")
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipBerkeleyApi", "BSD_NETPRES_Dependency")
		#Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP", True, 2)
		#Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipBerkeleyApi"])
	
def tcpipAutoConfigDDNSEnable(symbol, event):
	#enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipDdns"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipDdns", "libtcpipDdns")
		#Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
		#Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipDdns"])
	
def tcpipAutoConfigDHCPCLIENTEnable(symbol, event):
	#enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipDhcp"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipDhcp", "libtcpipDhcp")
		#Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
		#Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipDhcp"])
	
def tcpipAutoConfigDHCPSERVEREnable(symbol, event):
	#enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipDhcps"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipDhcps", "libtcpipDhcps")
		#Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
		#Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipDhcps"])
	
def tcpipAutoConfigDNSCLIENTEnable(symbol, event):
	#enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipDns"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipDns", "libtcpipDns")
		#Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipDns"])
	
def tcpipAutoConfigDNSSERVEREnable(symbol, event):
	#enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipDnss"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipDnss", "libtcpipDnss")
		#Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipDnss"])
	

def tcpipAutoConfigFTPSERVEREnable(symbol, event):
	#enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipFtps"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipFtps", "libtcpipFtps")
		res = Database.activateComponents(["tcpipSysFsWrapper"], "TCP/IP BASIC CONFIGURATION")
		tcpipAutoConfigBasicGroup.setNodeVisible("tcpipSysFsWrapper", "TcpipFsWarapper_SysFS_Dependency")
		
		rootGroup = Database.getRootGroup()
		rootGroup.addComponent("sys_fs")		
		res = Database.activateComponents(["sys_fs"])
		
		res = Database.connectDependencies(autoConnectTableFS)
		#Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
		#Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipFtps"])
	
def tcpipAutoConfigHTTPNETSERVEREnable(symbol, event):
	#enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipHttpNet"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipHttpNet", "libtcpipHttpNet")
		#tcpipAutoConfigAppsGroup.setNodeVisible("tcpipHttpNet", "HttpNet_Crypto_Dependency")
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipHttpNet", "HttpNet_NetPres_Dependency")
		res = Database.activateComponents(["tcpipSysFsWrapper"], "TCP/IP BASIC CONFIGURATION")
		tcpipAutoConfigBasicGroup.setNodeVisible("tcpipSysFsWrapper", "TcpipFsWarapper_SysFS_Dependency")
		rootGroup = Database.getRootGroup()
		rootGroup.addComponent("sys_fs")
		res = Database.activateComponents(["sys_fs"])
		res = Database.connectDependencies(autoConnectTableFS)
		rootGroup.addComponent("lib_crypto")
		res = Database.activateComponents(["lib_crypto"])
		res = Database.connectDependencies(autoConnectTableCRYPTOHttpNet)
		#Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipHttpNet"])
	
def tcpipAutoConfigHTTPSERVEREnable(symbol, event):
	#enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipHttp"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipHttp", "libtcpipHttp")
		#tcpipAutoConfigAppsGroup.setNodeVisible("tcpipHttp", "Http_Crypto_Dependency")
		res = Database.activateComponents(["tcpipSysFsWrapper"], "TCP/IP BASIC CONFIGURATION")
		tcpipAutoConfigBasicGroup.setNodeVisible("tcpipSysFsWrapper", "TcpipFsWarapper_SysFS_Dependency")
		#tcpipAutoConfigStackGroup.setNodeVisible("tcpipHttp", "Http_Crypto_Dependency") #Niyas : This node visibility not working
		rootGroup = Database.getRootGroup()
		rootGroup.addComponent("sys_fs")
		res = Database.activateComponents(["sys_fs"])
		res = Database.connectDependencies(autoConnectTableFS)
		rootGroup.addComponent("lib_crypto")
		res = Database.activateComponents(["lib_crypto"])
		res = Database.connectDependencies(autoConnectTableCRYPTOHttp)
		#Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipHttp"])
	
def tcpipAutoConfigIPERFEnable(symbol, event):
	#enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipIperf"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipIperf", "libtcpipIperf")
		#Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP", True, 2)
		#Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipIperf"])
	
def tcpipAutoConfigNBNSEnable(symbol, event):
	#enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipNbns"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipNbns", "libtcpipNbns")
		#Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
		#Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)		
	else:
		res = Database.deactivateComponents(["tcpipNbns"])
	
def tcpipAutoConfigREBOOTEnable(symbol, event):
	#enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipReboot"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipReboot", "libtcpipReboot")
		#Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
		#Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipReboot"])
	
def tcpipAutoConfigSMTPCLIENTEnable(symbol, event):
	#enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipSmtpc"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipSmtpc", "libtcpipSmtpc")
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipSmtpc", "Smtpc_NetPres_Dependency")
	else:
		res = Database.deactivateComponents(["tcpipSmtpc"])
	
def tcpipAutoConfigSNMPEnable(symbol, event):
	#enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipSnmp"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipSnmp", "libtcpipSnmp")
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipSnmp", "Snmp_Crypto_Dependency")
		# tcpipAutoConfigAppsGroup.setNodeVisible("tcpipSnmp", "Snmp_TcpipFs_Dependency")
		res = Database.activateComponents(["tcpipSysFsWrapper"], "TCP/IP BASIC CONFIGURATION")
		tcpipAutoConfigBasicGroup.setNodeVisible("tcpipSysFsWrapper", "TcpipFsWarapper_SysFS_Dependency")
		rootGroup = Database.getRootGroup()
		rootGroup.addComponent("sys_fs")
		res = Database.activateComponents(["sys_fs"])
		res = Database.connectDependencies(autoConnectTableFS)
	else:
		res = Database.deactivateComponents(["tcpipSnmp"])

def tcpipAutoConfigSNMPV3Enable(symbol, event):
	#enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipSnmpv3"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipSnmpv3", "libtcpipSnmpv3")
	else:
		res = Database.deactivateComponents(["tcpipSnmpv3"])

def tcpipAutoConfigSNTPEnable(symbol, event):
	#enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipSntp"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipSntp", "libtcpipSntp")
	else:
		res = Database.deactivateComponents(["tcpipSntp"])
	
def tcpipAutoConfigTELNETEnable(symbol, event):
	#enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipTelnet"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipTelnet", "libtcpipTelnet")
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipTelnet", "Telnet_NetPres_Dependency")
	else:
		res = Database.deactivateComponents(["tcpipTelnet"])
	
def tcpipAutoConfigTFTPCLIENTEnable(symbol, event):
	#enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipTftpc"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipTftpc", "libtcpipTftpc")
		# tcpipAutoConfigAppsGroup.setNodeVisible("tcpipTftpc", "Tftpc_TcpipFs_Dependency")
		res = Database.activateComponents(["tcpipSysFsWrapper"], "TCP/IP BASIC CONFIGURATION")
		tcpipAutoConfigBasicGroup.setNodeVisible("tcpipSysFsWrapper", "TcpipFsWarapper_SysFS_Dependency")
		rootGroup = Database.getRootGroup()
		rootGroup.addComponent("sys_fs")
		res = Database.activateComponents(["sys_fs"])
		res = Database.connectDependencies(autoConnectTableFS)
	else:
		res = Database.deactivateComponents(["tcpipTftpc"])
	
def tcpipAutoConfigZEROCONFEnable(symbol, event):
	#enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipZeroConf"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipZeroConf", "libtcpipZeroConf")
	else:
		res = Database.deactivateComponents(["tcpipZeroConf"])

def tcpipAutoConfigCMDEnable(symbol, event):
	#enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipCmd"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipCmd", "libtcpipCmd")
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipCmd", "Cmd_SysConsole_Dependency")
	else:
		res = Database.deactivateComponents(["tcpipCmd"])

