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
# autoConnectTableCRYPTOHttp = [["tcpipHttp", "Http_Crypto_Dependency", "lib_crypto", "lib_crypto"]]
autoConnectTableCRYPTOHttp = [["TCP/IP STACK", "APPLICATION LAYER:tcpipHttp:Http_Crypto_Dependency", "lib_crypto", "lib_crypto"]]
# autoConnectTableCRYPTOHttpNet = [["tcpipHttpNet", "HttpNet_Crypto_Dependency", "lib_crypto", "lib_crypto"]]
autoConnectTableCRYPTOHttpNet = [["TCP/IP STACK", "APPLICATION LAYER:tcpipHttpNet:HttpNet_Crypto_Dependency", "lib_crypto", "lib_crypto"]]
autoConnectTableCRYPTOSnmp = [["TCP/IP STACK", "APPLICATION LAYER:tcpipSnmp:Snmp_Crypto_Dependency", "lib_crypto", "lib_crypto"]]
autoConnectTableNetPresBSD = [["netPres_0", "libNetPres","TCP/IP STACK", "APPLICATION LAYER:tcpipBerkeleyApi:BSD_NETPRES_Dependency"]]
autoConnectTableNetPresSMTPC = [["netPres_0", "libNetPres","TCP/IP STACK", "APPLICATION LAYER:tcpipSmtpc:Smtpc_NetPres_Dependency"]]
############################################################################
#### Code Generation ####
############################################################################
def instantiateComponent(tcpipAutoConfigAppsComponent):

	tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
	if (tcpipAutoConfigStackGroup == None):
		tcpipAutoConfigStackGroup = Database.createGroup(None, "TCP/IP STACK")
		
	tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
	if (tcpipAutoConfigAppsGroup == None):
		tcpipAutoConfigAppsGroup = Database.createGroup("TCP/IP STACK", "APPLICATION LAYER")		

	# Enable Apps Configurator
	tcpipAutoConfigAppsEnable = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_APPS_ENABLE", None)
	tcpipAutoConfigAppsEnable.setVisible(False)
	tcpipAutoConfigAppsEnable.setDefaultValue(True) 
	
	# Enable ANNOUNCE
	tcpipAutoConfigANNOUNCE = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_ANNOUNCE", None)
	tcpipAutoConfigANNOUNCE.setLabel("ANNOUNCE")
	tcpipAutoConfigANNOUNCE.setVisible(True)
	tcpipAutoConfigANNOUNCE.setDescription("Enable ANNOUNCE")
	tcpipAutoConfigANNOUNCE.setDependencies(tcpipAutoConfigANNOUNCEEnable, ["TCPIP_AUTOCONFIG_ENABLE_ANNOUNCE"])
	
	# Enable Berkeley_API
	tcpipAutoConfigBerkeley_API = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_Berkeley_API", None)
	tcpipAutoConfigBerkeley_API.setLabel("Berkeley API")
	tcpipAutoConfigBerkeley_API.setVisible(True)
	tcpipAutoConfigBerkeley_API.setDescription("Enable Berkeley_API")
	tcpipAutoConfigBerkeley_API.setDependencies(tcpipAutoConfigBerkeleyAPIEnable, ["TCPIP_AUTOCONFIG_ENABLE_Berkeley_API"])
	
	# Enable DDNS
	tcpipAutoConfigDDNS = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_DDNS", None)
	tcpipAutoConfigDDNS.setLabel("DDNS")
	tcpipAutoConfigDDNS.setVisible(True)
	tcpipAutoConfigDDNS.setDescription("Enable DDNS")
	tcpipAutoConfigDDNS.setDependencies(tcpipAutoConfigDDNSEnable, ["TCPIP_AUTOCONFIG_ENABLE_DDNS"])

	# Enable DHCP_CLIENT
	tcpipAutoConfigDHCP_CLIENT = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_DHCP_CLIENT", None)
	tcpipAutoConfigDHCP_CLIENT.setLabel("DHCP CLIENT")
	tcpipAutoConfigDHCP_CLIENT.setVisible(True)
	tcpipAutoConfigDHCP_CLIENT.setDescription("Enable DHCP_CLIENT")	
	tcpipAutoConfigDHCP_CLIENT.setDependencies(tcpipAutoConfigDHCPCLIENTEnable, ["TCPIP_AUTOCONFIG_ENABLE_DHCP_CLIENT"])

	# Enable DHCP_SERVER
	tcpipAutoConfigDHCP_SERVER = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_DHCP_SERVER", None)
	tcpipAutoConfigDHCP_SERVER.setLabel("DHCP SERVER")
	tcpipAutoConfigDHCP_SERVER.setVisible(True)
	tcpipAutoConfigDHCP_SERVER.setDescription("Enable DHCP_SERVER")
	tcpipAutoConfigDHCP_SERVER.setDependencies(tcpipAutoConfigDHCPSERVEREnable, ["TCPIP_AUTOCONFIG_ENABLE_DHCP_SERVER"])

	# Enable DNS_CLIENT
	tcpipAutoConfigDNS_CLIENT = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_DNS_CLIENT", None)
	tcpipAutoConfigDNS_CLIENT.setLabel("DNS CLIENT")
	tcpipAutoConfigDNS_CLIENT.setVisible(True)
	tcpipAutoConfigDNS_CLIENT.setDescription("Enable DNS_CLIENT")
	tcpipAutoConfigDNS_CLIENT.setDependencies(tcpipAutoConfigDNSCLIENTEnable, ["TCPIP_AUTOCONFIG_ENABLE_DNS_CLIENT"])

	# Enable DNS_SERVER
	tcpipAutoConfigDNS_SERVER = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_DNS_SERVER", None)
	tcpipAutoConfigDNS_SERVER.setLabel("DNS SERVER")
	tcpipAutoConfigDNS_SERVER.setVisible(True)
	tcpipAutoConfigDNS_SERVER.setDescription("Enable DNS_SERVER")
	tcpipAutoConfigDNS_SERVER.setDependencies(tcpipAutoConfigDNSSERVEREnable, ["TCPIP_AUTOCONFIG_ENABLE_DNS_SERVER"])

	# Enable FTP_SERVER
	tcpipAutoConfigFTP_SERVER = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_FTP_SERVER", None)
	tcpipAutoConfigFTP_SERVER.setLabel("FTP SERVER")
	tcpipAutoConfigFTP_SERVER.setVisible(True)
	tcpipAutoConfigFTP_SERVER.setDescription("Enable FTP_SERVER")
	tcpipAutoConfigFTP_SERVER.setDependencies(tcpipAutoConfigFTPSERVEREnable, ["TCPIP_AUTOCONFIG_ENABLE_FTP_SERVER"])

	# Enable HTTP_NET_SERVER
	tcpipAutoConfigHTTP_NET_SERVER = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_HTTP_NET_SERVER", None)
	tcpipAutoConfigHTTP_NET_SERVER.setLabel("HTTPNET SERVER")
	tcpipAutoConfigHTTP_NET_SERVER.setVisible(True)
	tcpipAutoConfigHTTP_NET_SERVER.setDescription("Enable HTTP_NET_SERVER")
	tcpipAutoConfigHTTP_NET_SERVER.setDependencies(tcpipAutoConfigHTTPNETSERVEREnable, ["TCPIP_AUTOCONFIG_ENABLE_HTTP_NET_SERVER"])

	# Enable HTTP_SERVER
	tcpipAutoConfigHTTP_SERVER = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_HTTP_SERVER", None)
	tcpipAutoConfigHTTP_SERVER.setLabel("HTTP SERVER")
	tcpipAutoConfigHTTP_SERVER.setVisible(True)
	tcpipAutoConfigHTTP_SERVER.setDescription("Enable HTTP_SERVER")	
	tcpipAutoConfigHTTP_SERVER.setDependencies(tcpipAutoConfigHTTPSERVEREnable, ["TCPIP_AUTOCONFIG_ENABLE_HTTP_SERVER"])

	# Enable IPERF
	tcpipAutoConfigIPERF = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_IPERF", None)
	tcpipAutoConfigIPERF.setLabel("IPERF")
	tcpipAutoConfigIPERF.setVisible(True)
	tcpipAutoConfigIPERF.setDescription("Enable IPERF")	
	tcpipAutoConfigIPERF.setDependencies(tcpipAutoConfigIPERFEnable, ["TCPIP_AUTOCONFIG_ENABLE_IPERF"])
	
	# Enable NBNS
	tcpipAutoConfigNBNS = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_NBNS", None)
	tcpipAutoConfigNBNS.setLabel("NBNS")
	tcpipAutoConfigNBNS.setVisible(True)
	tcpipAutoConfigNBNS.setDescription("Enable NBNS")	
	tcpipAutoConfigNBNS.setDependencies(tcpipAutoConfigNBNSEnable, ["TCPIP_AUTOCONFIG_ENABLE_NBNS"])

	# Enable REBOOT
	tcpipAutoConfigREBOOT = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_REBOOT", None)
	tcpipAutoConfigREBOOT.setLabel("REBOOT")
	tcpipAutoConfigREBOOT.setVisible(True)
	tcpipAutoConfigREBOOT.setDescription("Enable REBOOT")
	tcpipAutoConfigREBOOT.setDependencies(tcpipAutoConfigREBOOTEnable, ["TCPIP_AUTOCONFIG_ENABLE_REBOOT"])

	# Enable SMTP_CLIENT
	tcpipAutoConfigSMTP_CLIENT = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_SMTP_CLIENT", None)
	tcpipAutoConfigSMTP_CLIENT.setLabel("SMTP CLIENT")
	tcpipAutoConfigSMTP_CLIENT.setVisible(True)
	tcpipAutoConfigSMTP_CLIENT.setDescription("Enable SMTP_CLIENT")
	tcpipAutoConfigSMTP_CLIENT.setDependencies(tcpipAutoConfigSMTPCLIENTEnable, ["TCPIP_AUTOCONFIG_ENABLE_SMTP_CLIENT"])
	
	# Enable SNMP
	tcpipAutoConfigSNMP = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_SNMP", None)
	tcpipAutoConfigSNMP.setLabel("SNMP")
	tcpipAutoConfigSNMP.setVisible(True)
	tcpipAutoConfigSNMP.setDescription("Enable SNMP")
	tcpipAutoConfigSNMP.setDependencies(tcpipAutoConfigSNMPEnable, ["TCPIP_AUTOCONFIG_ENABLE_SNMP"])

	# Enable SNMPV3
	tcpipAutoConfigSNMPV3 = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_SNMPV3", None)
	tcpipAutoConfigSNMPV3.setLabel("SNMPV3")
	tcpipAutoConfigSNMPV3.setVisible(True)
	tcpipAutoConfigSNMPV3.setDescription("Enable SNMPV3")
	tcpipAutoConfigSNMPV3.setDependencies(tcpipAutoConfigSNMPV3Enable, ["TCPIP_AUTOCONFIG_ENABLE_SNMPV3"])
	
	# Enable SNTP
	tcpipAutoConfigSNTP = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_SNTP", None)
	tcpipAutoConfigSNTP.setLabel("SNTP")
	tcpipAutoConfigSNTP.setVisible(True)
	tcpipAutoConfigSNTP.setDescription("Enable SNTP")
	tcpipAutoConfigSNTP.setDependencies(tcpipAutoConfigSNTPEnable, ["TCPIP_AUTOCONFIG_ENABLE_SNTP"])

	# Enable TELNET
	tcpipAutoConfigTELNET = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_TELNET", None)
	tcpipAutoConfigTELNET.setLabel("TELNET")
	tcpipAutoConfigTELNET.setVisible(True)
	tcpipAutoConfigTELNET.setDescription("Enable TELNET")
	tcpipAutoConfigTELNET.setDependencies(tcpipAutoConfigTELNETEnable, ["TCPIP_AUTOCONFIG_ENABLE_TELNET"])

	# Enable TFTP_CLIENT
	tcpipAutoConfigTFTP_CLIENT = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_TFTP_CLIENT", None)
	tcpipAutoConfigTFTP_CLIENT.setLabel("TFTP CLIENT")
	tcpipAutoConfigTFTP_CLIENT.setVisible(True)
	tcpipAutoConfigTFTP_CLIENT.setDescription("Enable TFTP_CLIENT")
	tcpipAutoConfigTFTP_CLIENT.setDependencies(tcpipAutoConfigTFTPCLIENTEnable, ["TCPIP_AUTOCONFIG_ENABLE_TFTP_CLIENT"])

	# Enable ZEROCONF
	tcpipAutoConfigZEROCONF = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_ZEROCONF", None)
	tcpipAutoConfigZEROCONF.setLabel("ZEROCONF")
	tcpipAutoConfigZEROCONF.setVisible(True)
	tcpipAutoConfigZEROCONF.setDescription("Enable ZEROCONF")
	tcpipAutoConfigZEROCONF.setDependencies(tcpipAutoConfigZEROCONFEnable, ["TCPIP_AUTOCONFIG_ENABLE_ZEROCONF"])
########################################################################################################
def finalizeComponent(tcpipAutoConfigAppsComponent):
	tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
	tcpipAutoConfigAppsGroup.addComponent(tcpipAutoConfigAppsComponent.getID())
	
	if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_TRANS_ENABLE") != True)  and (Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_NET_ENABLE") != True) and (Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_DRV_ENABLE") != True) and (Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_BASIC_ENABLE") != True):
		Database.setActiveGroup("APPLICATION LAYER")	
	
#######################################################################################################
def enableTcpipAutoConfigApps(enable):

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
			
		if(Database.getComponentByID("tcpip_transport_config") == None):
			res = tcpipAutoConfigTransportGroup.addComponent("tcpip_transport_config")#niyas check whether addComponent is needed
			res = Database.activateComponents(["tcpip_transport_config"], "TRANSPORT LAYER", False)	
		
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

#################Business Logic- Application Layer #########################################		
def tcpipAutoConfigANNOUNCEEnable(symbol, event):	
	tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
	enableTcpipAutoConfigApps(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipAnnounce"],"APPLICATION LAYER", False)		
		if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
			Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
		if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
			Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipAnnounce", "libtcpipAnnounce")

	else:
		res = Database.deactivateComponents(["tcpipAnnounce"])
	
def tcpipAutoConfigBerkeleyAPIEnable(symbol, event):
	tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
	tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
	enableTcpipAutoConfigApps(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipBerkeleyApi"],"APPLICATION LAYER", False)	
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipBerkeleyApi", "libtcpipBerkeleyApi")
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipBerkeleyApi", "BSD_NETPRES_Dependency")
		tcpipAutoConfigStackGroup.setAttachmentVisible("APPLICATION LAYER", "tcpipBerkeleyApi:BSD_NETPRES_Dependency")
		if(Database.getComponentByID("netPres") == None):
			res = Database.activateComponents(["netPres"])	
			res = Database.connectDependencies(autoConnectTableNetPresBSD)
		
		if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP") != True):
			Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP", True, 2)
		if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
			Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipBerkeleyApi"])
	
def tcpipAutoConfigDDNSEnable(symbol, event):
	tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
	enableTcpipAutoConfigApps(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipDdns"],"APPLICATION LAYER", False)		
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipDdns", "libtcpipDdns")
		if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
			Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
		if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
			Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipDdns"])
	
def tcpipAutoConfigDHCPCLIENTEnable(symbol, event):
	tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
	enableTcpipAutoConfigApps(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipDhcp"],"APPLICATION LAYER", False)	
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipDhcp", "libtcpipDhcp")
		if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
			Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
		if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
			Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipDhcp"])
	
def tcpipAutoConfigDHCPSERVEREnable(symbol, event):
	tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
	enableTcpipAutoConfigApps(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipDhcps"],"APPLICATION LAYER", False)	
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipDhcps", "libtcpipDhcps")
		if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
			Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
		if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
			Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipDhcps"])
	
def tcpipAutoConfigDNSCLIENTEnable(symbol, event):
	tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
	enableTcpipAutoConfigApps(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipDns"],"APPLICATION LAYER", False)	
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipDns", "libtcpipDns")
		if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
			Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipDns"])
	
def tcpipAutoConfigDNSSERVEREnable(symbol, event):
	tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
	enableTcpipAutoConfigApps(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipDnss"],"APPLICATION LAYER", False)
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipDnss", "libtcpipDnss")
		if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
			Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipDnss"])
	

def tcpipAutoConfigFTPSERVEREnable(symbol, event):
	tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
	enableTcpipAutoConfigApps(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipFtps"],"APPLICATION LAYER", False)	
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipFtps", "libtcpipFtps")

		if(Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_SysFSWrapper") != True):
			Database.setSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_SysFSWrapper", True, 2)	
		if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
			Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
		if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP") != True):
			Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipFtps"])
	
def tcpipAutoConfigHTTPNETSERVEREnable(symbol, event):
	tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
	tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
	enableTcpipAutoConfigApps(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipHttpNet"],"APPLICATION LAYER", False)	
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipHttpNet", "libtcpipHttpNet")
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipHttpNet", "HttpNet_Crypto_Dependency")
		tcpipAutoConfigStackGroup.setAttachmentVisible("APPLICATION LAYER", "tcpipHttpNet:HttpNet_Crypto_Dependency")
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipHttpNet", "HttpNet_NetPres_Dependency")
		tcpipAutoConfigStackGroup.setAttachmentVisible("APPLICATION LAYER", "tcpipHttpNet:HttpNet_NetPres_Dependency")
		
		if(Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_SysFSWrapper") != True):
			Database.setSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_SysFSWrapper", True, 2)	
			
		# res = Database.activateComponents(["lib_crypto"], Database.getRootGroup().getID(), True)	
		res = Database.activateComponents(["lib_crypto"])
		res = Database.connectDependencies(autoConnectTableCRYPTOHttpNet)
		if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP") != True):
			Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipHttpNet"])
	
def tcpipAutoConfigHTTPSERVEREnable(symbol, event):
	tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
	tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
	enableTcpipAutoConfigApps(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipHttp"],"APPLICATION LAYER", False)	
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipHttp", "libtcpipHttp")
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipHttp", "Http_Crypto_Dependency")
		tcpipAutoConfigStackGroup.setAttachmentVisible("APPLICATION LAYER", "tcpipHttp:Http_Crypto_Dependency") #Niyas : This node visibility not working
		if(Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_SysFSWrapper") != True):
			Database.setSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_SysFSWrapper", True, 2)
			
		# res = Database.activateComponents(["lib_crypto"], Database.getRootGroup().getID(), True)	
		res = Database.activateComponents(["lib_crypto"])
		res = Database.connectDependencies(autoConnectTableCRYPTOHttp)
		if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP") != True):
			Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipHttp"])
	
def tcpipAutoConfigIPERFEnable(symbol, event):
	tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
	enableTcpipAutoConfigApps(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipIperf"],"APPLICATION LAYER", False)	
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipIperf", "libtcpipIperf")
		if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP") != True):
			Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP", True, 2)
		if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
			Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipIperf"])
	
def tcpipAutoConfigNBNSEnable(symbol, event):
	tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
	enableTcpipAutoConfigApps(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipNbns"],"APPLICATION LAYER", False)	
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipNbns", "libtcpipNbns")
		if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
			Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
		if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
			Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)		
	else:
		res = Database.deactivateComponents(["tcpipNbns"])
	
def tcpipAutoConfigREBOOTEnable(symbol, event):
	tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
	enableTcpipAutoConfigApps(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipReboot"],"APPLICATION LAYER", False)	
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipReboot", "libtcpipReboot")
		if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
			Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
		if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
			Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipReboot"])
	
def tcpipAutoConfigSMTPCLIENTEnable(symbol, event):
	tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
	tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
	
	enableTcpipAutoConfigApps(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipSmtpc"],"APPLICATION LAYER", False)	
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipSmtpc", "libtcpipSmtpc")
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipSmtpc", "Smtpc_NetPres_Dependency")
		tcpipAutoConfigStackGroup.setAttachmentVisible("APPLICATION LAYER", "tcpipSmtpc:Smtpc_NetPres_Dependency")
		if(Database.getComponentByID("netPres") == None):
			res = Database.activateComponents(["netPres"])	
			res = Database.connectDependencies(autoConnectTableNetPresSMTPC)
		
		if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP") != True):
			Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipSmtpc"])
	
def tcpipAutoConfigSNMPEnable(symbol, event):
	tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
	tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
	enableTcpipAutoConfigApps(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipSnmp"],"APPLICATION LAYER", False)	
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipSnmp", "libtcpipSnmp")
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipSnmp", "Snmp_Crypto_Dependency")
		tcpipAutoConfigStackGroup.setAttachmentVisible("APPLICATION LAYER", "tcpipSnmp:Snmp_Crypto_Dependency") #Niyas : This node visibility not working
		res = Database.activateComponents(["lib_crypto"])
		res = Database.connectDependencies(autoConnectTableCRYPTOSnmp)
		# tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipSnmp", "Snmp_TcpipFs_Dependency")
		if(Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_SysFSWrapper") != True):
			Database.setSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_SysFSWrapper", True, 2)	
		if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
			Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipSnmp"])

def tcpipAutoConfigSNMPV3Enable(symbol, event):
	tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
	enableTcpipAutoConfigApps(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipSnmpv3"],"APPLICATION LAYER", False)	
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipSnmpv3", "libtcpipSnmpv3")
		if(Database.getSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_SNMP") != True):
			Database.setSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_SNMP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipSnmpv3"])

def tcpipAutoConfigSNTPEnable(symbol, event):
	tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
	enableTcpipAutoConfigApps(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipSntp"],"APPLICATION LAYER", False)	
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipSntp", "libtcpipSntp")		
		if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
			Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipSntp"])
	
def tcpipAutoConfigTELNETEnable(symbol, event):
	tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
	tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
	enableTcpipAutoConfigApps(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipTelnet"],"APPLICATION LAYER", False)	
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipTelnet", "libtcpipTelnet")
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipTelnet", "Telnet_NetPres_Dependency")
		tcpipAutoConfigStackGroup.setAttachmentVisible("APPLICATION LAYER", "tcpipTelnet:Telnet_NetPres_Dependency")
		
		if(Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_TCPIPCMD") != True):
			Database.setSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_TCPIPCMD", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipTelnet"])
	
def tcpipAutoConfigTFTPCLIENTEnable(symbol, event):
	tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
	enableTcpipAutoConfigApps(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipTftpc"],"APPLICATION LAYER", False)	
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipTftpc", "libtcpipTftpc")
		# tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipTftpc", "Tftpc_TcpipFs_Dependency")
		if(Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_SysFSWrapper") != True):
			Database.setSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_SysFSWrapper", True, 2)	
		if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
			Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
		if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
			Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipTftpc"])
	
def tcpipAutoConfigZEROCONFEnable(symbol, event):
	tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
	enableTcpipAutoConfigApps(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipZeroConf"],"APPLICATION LAYER", False)	
		tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipZeroConf", "libtcpipZeroConf")
		if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
			Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
		if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
			Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipZeroConf"])


