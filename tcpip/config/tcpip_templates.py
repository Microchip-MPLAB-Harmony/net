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
def instantiateComponent(tcpipAutoConfigComponent):
	


#######################################################################################################
	# Menu for Application Layer
	tcpipAutoConfigAppsLayer = tcpipAutoConfigComponent.createMenuSymbol("TCPIP_AUTOCONFIG_APPS_MENU", None)
	tcpipAutoConfigAppsLayer.setLabel("Application Layer")
	tcpipAutoConfigAppsLayer.setVisible(True)
	tcpipAutoConfigAppsLayer.setDescription("Enable Application Layer Components")
	
	# Enable ANNOUNCE
	tcpipAutoConfigANNOUNCE = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_ANNOUNCE", tcpipAutoConfigAppsLayer)
	tcpipAutoConfigANNOUNCE.setLabel("Enable ANNOUNCE")
	tcpipAutoConfigANNOUNCE.setVisible(True)
	tcpipAutoConfigANNOUNCE.setDescription("Enable ANNOUNCE")
	tcpipAutoConfigANNOUNCE.setDependencies(tcpipAutoConfigANNOUNCEEnable, ["TCPIP_AUTOCONFIG_ENABLE_ANNOUNCE"])
	
	# Enable Berkeley_API
	tcpipAutoConfigBerkeley_API = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_Berkeley_API", tcpipAutoConfigAppsLayer)
	tcpipAutoConfigBerkeley_API.setLabel("Enable Berkeley_API")
	tcpipAutoConfigBerkeley_API.setVisible(True)
	tcpipAutoConfigBerkeley_API.setDescription("Enable Berkeley_API")
	tcpipAutoConfigBerkeley_API.setDependencies(tcpipAutoConfigBerkeleyAPIEnable, ["TCPIP_AUTOCONFIG_ENABLE_Berkeley_API"])
	
	# Enable DDNS
	tcpipAutoConfigDDNS = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_DDNS", tcpipAutoConfigAppsLayer)
	tcpipAutoConfigDDNS.setLabel("Enable DDNS")
	tcpipAutoConfigDDNS.setVisible(True)
	tcpipAutoConfigDDNS.setDescription("Enable DDNS")
	tcpipAutoConfigDDNS.setDependencies(tcpipAutoConfigDDNSEnable, ["TCPIP_AUTOCONFIG_ENABLE_DDNS"])

	# Enable DHCP_CLIENT
	tcpipAutoConfigDHCP_CLIENT = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_DHCP_CLIENT", tcpipAutoConfigAppsLayer)
	tcpipAutoConfigDHCP_CLIENT.setLabel("Enable DHCP_CLIENT")
	tcpipAutoConfigDHCP_CLIENT.setVisible(True)
	tcpipAutoConfigDHCP_CLIENT.setDescription("Enable DHCP_CLIENT")	
	tcpipAutoConfigDHCP_CLIENT.setDependencies(tcpipAutoConfigDHCPCLIENTEnable, ["TCPIP_AUTOCONFIG_ENABLE_DHCP_CLIENT"])

	# Enable DHCP_SERVER
	tcpipAutoConfigDHCP_SERVER = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_DHCP_SERVER", tcpipAutoConfigAppsLayer)
	tcpipAutoConfigDHCP_SERVER.setLabel("Enable DHCP_SERVER")
	tcpipAutoConfigDHCP_SERVER.setVisible(True)
	tcpipAutoConfigDHCP_SERVER.setDescription("Enable DHCP_SERVER")
	tcpipAutoConfigDHCP_SERVER.setDependencies(tcpipAutoConfigDHCPSERVEREnable, ["TCPIP_AUTOCONFIG_ENABLE_DHCP_SERVER"])

	# Enable DNS_CLIENT
	tcpipAutoConfigDNS_CLIENT = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_DNS_CLIENT", tcpipAutoConfigAppsLayer)
	tcpipAutoConfigDNS_CLIENT.setLabel("Enable DNS_CLIENT")
	tcpipAutoConfigDNS_CLIENT.setVisible(True)
	tcpipAutoConfigDNS_CLIENT.setDescription("Enable DNS_CLIENT")
	tcpipAutoConfigDNS_CLIENT.setDependencies(tcpipAutoConfigDNSCLIENTEnable, ["TCPIP_AUTOCONFIG_ENABLE_DNS_CLIENT"])

	# Enable DNS_SERVER
	tcpipAutoConfigDNS_SERVER = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_DNS_SERVER", tcpipAutoConfigAppsLayer)
	tcpipAutoConfigDNS_SERVER.setLabel("Enable DNS_SERVER")
	tcpipAutoConfigDNS_SERVER.setVisible(True)
	tcpipAutoConfigDNS_SERVER.setDescription("Enable DNS_SERVER")
	tcpipAutoConfigDNS_SERVER.setDependencies(tcpipAutoConfigDNSSERVEREnable, ["TCPIP_AUTOCONFIG_ENABLE_DNS_SERVER"])

	# Enable FTP_SERVER
	tcpipAutoConfigFTP_SERVER = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_FTP_SERVER", tcpipAutoConfigAppsLayer)
	tcpipAutoConfigFTP_SERVER.setLabel("Enable FTP_SERVER")
	tcpipAutoConfigFTP_SERVER.setVisible(True)
	tcpipAutoConfigFTP_SERVER.setDescription("Enable FTP_SERVER")
	tcpipAutoConfigFTP_SERVER.setDependencies(tcpipAutoConfigFTPSERVEREnable, ["TCPIP_AUTOCONFIG_ENABLE_FTP_SERVER"])

	# Enable HTTP_NET_SERVER
	tcpipAutoConfigHTTP_NET_SERVER = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_HTTP_NET_SERVER", tcpipAutoConfigAppsLayer)
	tcpipAutoConfigHTTP_NET_SERVER.setLabel("Enable HTTP_NET_SERVER")
	tcpipAutoConfigHTTP_NET_SERVER.setVisible(True)
	tcpipAutoConfigHTTP_NET_SERVER.setDescription("Enable HTTP_NET_SERVER")
	tcpipAutoConfigHTTP_NET_SERVER.setDependencies(tcpipAutoConfigHTTPNETSERVEREnable, ["TCPIP_AUTOCONFIG_ENABLE_HTTP_NET_SERVER"])

	# Enable HTTP_SERVER
	tcpipAutoConfigHTTP_SERVER = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_HTTP_SERVER", tcpipAutoConfigAppsLayer)
	tcpipAutoConfigHTTP_SERVER.setLabel("Enable HTTP_SERVER")
	tcpipAutoConfigHTTP_SERVER.setVisible(True)
	tcpipAutoConfigHTTP_SERVER.setDescription("Enable HTTP_SERVER")	
	tcpipAutoConfigHTTP_SERVER.setDependencies(tcpipAutoConfigHTTPSERVEREnable, ["TCPIP_AUTOCONFIG_ENABLE_HTTP_SERVER"])

	# Enable IPERF
	tcpipAutoConfigIPERF = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_IPERF", tcpipAutoConfigAppsLayer)
	tcpipAutoConfigIPERF.setLabel("Enable IPERF")
	tcpipAutoConfigIPERF.setVisible(True)
	tcpipAutoConfigIPERF.setDescription("Enable IPERF")	
	tcpipAutoConfigIPERF.setDependencies(tcpipAutoConfigIPERFEnable, ["TCPIP_AUTOCONFIG_ENABLE_IPERF"])
	
	# Enable NBNS
	tcpipAutoConfigNBNS = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_NBNS", tcpipAutoConfigAppsLayer)
	tcpipAutoConfigNBNS.setLabel("Enable NBNS")
	tcpipAutoConfigNBNS.setVisible(True)
	tcpipAutoConfigNBNS.setDescription("Enable NBNS")	
	tcpipAutoConfigNBNS.setDependencies(tcpipAutoConfigNBNSEnable, ["TCPIP_AUTOCONFIG_ENABLE_NBNS"])

	# Enable REBOOT
	tcpipAutoConfigREBOOT = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_REBOOT", tcpipAutoConfigAppsLayer)
	tcpipAutoConfigREBOOT.setLabel("Enable REBOOT")
	tcpipAutoConfigREBOOT.setVisible(True)
	tcpipAutoConfigREBOOT.setDescription("Enable REBOOT")
	tcpipAutoConfigREBOOT.setDependencies(tcpipAutoConfigREBOOTEnable, ["TCPIP_AUTOCONFIG_ENABLE_REBOOT"])

	# Enable SMTP_CLIENT
	tcpipAutoConfigSMTP_CLIENT = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_SMTP_CLIENT", tcpipAutoConfigAppsLayer)
	tcpipAutoConfigSMTP_CLIENT.setLabel("Enable SMTP_CLIENT")
	tcpipAutoConfigSMTP_CLIENT.setVisible(True)
	tcpipAutoConfigSMTP_CLIENT.setDescription("Enable SMTP_CLIENT")
	tcpipAutoConfigSMTP_CLIENT.setDependencies(tcpipAutoConfigSMTPCLIENTEnable, ["TCPIP_AUTOCONFIG_ENABLE_SMTP_CLIENT"])
	
	# Enable SNMP
	tcpipAutoConfigSNMP = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_SNMP", tcpipAutoConfigAppsLayer)
	tcpipAutoConfigSNMP.setLabel("Enable SNMP")
	tcpipAutoConfigSNMP.setVisible(True)
	tcpipAutoConfigSNMP.setDescription("Enable SNMP")
	tcpipAutoConfigSNMP.setDependencies(tcpipAutoConfigSNMPEnable, ["TCPIP_AUTOCONFIG_ENABLE_SNMP"])

	# Enable SNMPV3
	tcpipAutoConfigSNMPV3 = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_SNMPV3", tcpipAutoConfigAppsLayer)
	tcpipAutoConfigSNMPV3.setLabel("Enable SNMPV3")
	tcpipAutoConfigSNMPV3.setVisible(True)
	tcpipAutoConfigSNMPV3.setDescription("Enable SNMPV3")
	tcpipAutoConfigSNMPV3.setDependencies(tcpipAutoConfigSNMPV3Enable, ["TCPIP_AUTOCONFIG_ENABLE_SNMPV3"])
	
	# Enable SNTP
	tcpipAutoConfigSNTP = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_SNTP", tcpipAutoConfigAppsLayer)
	tcpipAutoConfigSNTP.setLabel("Enable SNTP")
	tcpipAutoConfigSNTP.setVisible(True)
	tcpipAutoConfigSNTP.setDescription("Enable SNTP")
	tcpipAutoConfigSNTP.setDependencies(tcpipAutoConfigSNTPEnable, ["TCPIP_AUTOCONFIG_ENABLE_SNTP"])

	# Enable TELNET
	tcpipAutoConfigTELNET = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_TELNET", tcpipAutoConfigAppsLayer)
	tcpipAutoConfigTELNET.setLabel("Enable TELNET")
	tcpipAutoConfigTELNET.setVisible(True)
	tcpipAutoConfigTELNET.setDescription("Enable TELNET")
	tcpipAutoConfigTELNET.setDependencies(tcpipAutoConfigTELNETEnable, ["TCPIP_AUTOCONFIG_ENABLE_TELNET"])

	# Enable TFTP_CLIENT
	tcpipAutoConfigTFTP_CLIENT = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_TFTP_CLIENT", tcpipAutoConfigAppsLayer)
	tcpipAutoConfigTFTP_CLIENT.setLabel("Enable TFTP_CLIENT")
	tcpipAutoConfigTFTP_CLIENT.setVisible(True)
	tcpipAutoConfigTFTP_CLIENT.setDescription("Enable TFTP_CLIENT")
	tcpipAutoConfigTFTP_CLIENT.setDependencies(tcpipAutoConfigTFTPCLIENTEnable, ["TCPIP_AUTOCONFIG_ENABLE_TFTP_CLIENT"])

	# Enable ZEROCONF
	tcpipAutoConfigZEROCONF = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_ZEROCONF", tcpipAutoConfigAppsLayer)
	tcpipAutoConfigZEROCONF.setLabel("Enable ZEROCONF")
	tcpipAutoConfigZEROCONF.setVisible(True)
	tcpipAutoConfigZEROCONF.setDescription("Enable ZEROCONF")
	tcpipAutoConfigZEROCONF.setDependencies(tcpipAutoConfigZEROCONFEnable, ["TCPIP_AUTOCONFIG_ENABLE_ZEROCONF"])
	
	# Enable TCPIP_CMD
	tcpipAutoConfigTCPIP_CMD = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_TCPIP_CMD", tcpipAutoConfigAppsLayer)
	tcpipAutoConfigTCPIP_CMD.setLabel("Enable TCPIP_CMD")
	tcpipAutoConfigTCPIP_CMD.setVisible(True)
	tcpipAutoConfigTCPIP_CMD.setDescription("Enable TCPIP_CMD")
	tcpipAutoConfigTCPIP_CMD.setDependencies(tcpipAutoConfigCMDEnable, ["TCPIP_AUTOCONFIG_ENABLE_TCPIP_CMD"])
	
#######################################################################################################	
	# Menu for Transport Layer
	tcpipAutoConfigTransportLayer = tcpipAutoConfigComponent.createMenuSymbol("TCPIP_AUTOCONFIG_TRANSPORT_MENU", None)
	tcpipAutoConfigTransportLayer.setLabel("Transport Layer")
	tcpipAutoConfigTransportLayer.setVisible(True)
	tcpipAutoConfigTransportLayer.setDescription("Enable Network Layer Components")
	
	# Enable TCP
	tcpipAutoConfigTCP = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_TCP", tcpipAutoConfigTransportLayer)
	tcpipAutoConfigTCP.setLabel("Enable TCP")
	tcpipAutoConfigTCP.setVisible(True)
	tcpipAutoConfigTCP.setDescription("Enable TCP")	
	tcpipAutoConfigTCP.setDependencies(tcpipAutoConfigTCPEnable, ["TCPIP_AUTOCONFIG_ENABLE_TCP"])
	
	# Enable UDP
	tcpipAutoConfigUDP = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_UDP", tcpipAutoConfigTransportLayer)
	tcpipAutoConfigUDP.setLabel("Enable UDP")
	tcpipAutoConfigUDP.setVisible(True)
	tcpipAutoConfigUDP.setDescription("Enable UDP")	
	tcpipAutoConfigUDP.setDependencies(tcpipAutoConfigUDPEnable, ["TCPIP_AUTOCONFIG_ENABLE_UDP"])
	
#######################################################################################################	
	# Menu for Network Layer
	tcpipAutoConfigNetworkLayer = tcpipAutoConfigComponent.createMenuSymbol("TCPIP_AUTOCONFIG_NETWORK_MENU", None)
	tcpipAutoConfigNetworkLayer.setLabel("Network Layer")
	tcpipAutoConfigNetworkLayer.setVisible(True)
	tcpipAutoConfigNetworkLayer.setDescription("Enable Transport Layer Components")
	
	# Enable ARP
	tcpipAutoConfigARP = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_ARP", tcpipAutoConfigNetworkLayer)
	tcpipAutoConfigARP.setLabel("Enable ARP")
	tcpipAutoConfigARP.setVisible(True)
	tcpipAutoConfigARP.setDescription("Enable ARP")	
	tcpipAutoConfigARP.setDependencies(tcpipAutoConfigARPEnable, ["TCPIP_AUTOCONFIG_ENABLE_ARP"])
	
	# Enable ICMPv4
	tcpipAutoConfigICMPv4 = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_ICMPv4", tcpipAutoConfigNetworkLayer)
	tcpipAutoConfigICMPv4.setLabel("Enable ICMPv4")
	tcpipAutoConfigICMPv4.setVisible(True)
	tcpipAutoConfigICMPv4.setDescription("Enable ICMPv4")
	tcpipAutoConfigICMPv4.setDependencies(tcpipAutoConfigICMPEnable, ["TCPIP_AUTOCONFIG_ENABLE_ICMPv4"])
	
	# Enable IGMP
	tcpipAutoConfigIGMP = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_IGMP", tcpipAutoConfigNetworkLayer)
	tcpipAutoConfigIGMP.setLabel("Enable IGMP")
	tcpipAutoConfigIGMP.setVisible(True)
	tcpipAutoConfigIGMP.setDescription("Enable IGMP")	
	tcpipAutoConfigIGMP.setDependencies(tcpipAutoConfigIGMPEnable, ["TCPIP_AUTOCONFIG_ENABLE_IGMP"])
	
	# Enable IPv4
	tcpipAutoConfigIPv4 = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_IPV4", tcpipAutoConfigNetworkLayer)
	tcpipAutoConfigIPv4.setLabel("Enable IPv4")
	tcpipAutoConfigIPv4.setVisible(True)
	tcpipAutoConfigIPv4.setDescription("Enable IPv4")
	tcpipAutoConfigIPv4.setDependencies(tcpipAutoConfigIPv4Enable, ["TCPIP_AUTOCONFIG_ENABLE_IPV4"])
	
	# Enable IPv6
	tcpipAutoConfigIPv6 = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_IPV6", tcpipAutoConfigNetworkLayer)
	tcpipAutoConfigIPv6.setLabel("Enable IPv6")
	tcpipAutoConfigIPv6.setVisible(True)
	tcpipAutoConfigIPv6.setDescription("Enable IPv6")
	tcpipAutoConfigIPv6.setDependencies(tcpipAutoConfigIPv6Enable, ["TCPIP_AUTOCONFIG_ENABLE_IPV6"])
	
	# Enable NDP
	tcpipAutoConfigNDP = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_NDP", tcpipAutoConfigNetworkLayer)
	tcpipAutoConfigNDP.setLabel("Enable NDP")
	tcpipAutoConfigNDP.setVisible(True)
	tcpipAutoConfigNDP.setDescription("Enable NDP")	
	tcpipAutoConfigNDP.setDependencies(tcpipAutoConfigNDPEnable, ["TCPIP_AUTOCONFIG_ENABLE_NDP"])	
	#tcpipIPv4Frag.setDefaultValue(False)
	#tcpipIPv4Frag.setDependencies(tcpipIPv4MenuVisible, ["TCPIP_STACK_USE_IPV4"])
#######################################################################################################
	# Menu for Driver Layer
	tcpipAutoConfigDriverLayer = tcpipAutoConfigComponent.createMenuSymbol("TCPIP_AUTOCONFIG_DRIVER_MENU", None)
	tcpipAutoConfigDriverLayer.setLabel("DataLink & Physical Layer")
	tcpipAutoConfigDriverLayer.setVisible(True)
	tcpipAutoConfigDriverLayer.setDescription("Enable DataLink & Physical Layer Components")

	# Enable GMAC
	tcpipAutoConfigGMAC = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_GMAC", tcpipAutoConfigDriverLayer)
	tcpipAutoConfigGMAC.setLabel("Enable GMAC")
	tcpipAutoConfigGMAC.setVisible(True)
	tcpipAutoConfigGMAC.setDescription("Enable GMAC")
	tcpipAutoConfigGMAC.setDependencies(tcpipAutoConfigGMACEnable, ["TCPIP_AUTOCONFIG_ENABLE_GMAC"])	

	# Enable MIIM_Driver
	tcpipAutoConfigMIIM_Driver = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", tcpipAutoConfigDriverLayer)
	tcpipAutoConfigMIIM_Driver.setLabel("Enable MIIM_Driver")
	tcpipAutoConfigMIIM_Driver.setVisible(True)
	tcpipAutoConfigMIIM_Driver.setDescription("Enable MIIM_Driver")
	tcpipAutoConfigMIIM_Driver.setDependencies(tcpipAutoConfigMIIMDriverEnable, ["TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver"])		
	
	# Enable KSZ8061
	tcpipAutoConfigKSZ8061 = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_KSZ8061", tcpipAutoConfigDriverLayer)
	tcpipAutoConfigKSZ8061.setLabel("Enable KSZ8061")
	tcpipAutoConfigKSZ8061.setVisible(True)
	tcpipAutoConfigKSZ8061.setDescription("Enable KSZ8061")	
	tcpipAutoConfigKSZ8061.setDependencies(tcpipAutoConfigKSZ8061Enable, ["TCPIP_AUTOCONFIG_ENABLE_KSZ8061"])	

	# Enable LAN8740
	tcpipAutoConfigLAN8740 = tcpipAutoConfigComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_LAN8740", tcpipAutoConfigDriverLayer)
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
		if (isEnabled != True):	
			res = Database.activateComponents(["HarmonyCore"])
			tcpipAutoConfigStackGroup = Database.createGroup(None, "TCP/IP STACK")
			tcpipAutoConfigAppsGroup = Database.createGroup("TCP/IP STACK", "APPLICATION LAYER")
			tcpipAutoConfigTransportGroup = Database.createGroup("TCP/IP STACK", "TRANSPORT LAYER")
			tcpipAutoConfigNetworkGroup = Database.createGroup("TCP/IP STACK", "NETWORK LAYER")	
			tcpipAutoConfigDriverGroup = Database.createGroup("TCP/IP STACK", "DRIVER LAYER")
			tcpipAutoConfigBasicGroup = Database.createGroup("TCP/IP STACK", "TCP/IP BASIC CONFIGURATION")
			
			res = Database.activateComponents(["tcpipStack"], "TCP/IP BASIC CONFIGURATION")
			tcpipAutoConfigBasicGroup.setNodeVisible("tcpipStack", "Core_SysTime_Dependency")
			tcpipAutoConfigBasicGroup.setNodeVisible("tcpipStack", "libtcpipStack")
			res = Database.activateComponents(["tcpipNetConfig"], "TCP/IP BASIC CONFIGURATION")
			tcpipAutoConfigBasicGroup.setNodeVisible("tcpipNetConfig", "NETCONFIG_MAC_Dependency") #niyas: not working 
			tcpipAutoConfigBasicGroup.setNodeVisible("tcpipNetConfig", "libtcpipNetConfig")			
			
			tcpipAutoConfigStackGroup.setNodeVisible("tcpipStack", "Core_SysTime_Dependency") #niyas: not working 

			rootGroup = Database.getRootGroup()
			rootGroup.addComponent("sys_time")
			res = Database.activateComponents(["sys_time"])
			
			res = Database.connectDependencies(autoConnectTableTIME)

			isEnabled = True

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
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipIcmp"])

def tcpipAutoConfigIGMPEnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipIgmp"],"NETWORK LAYER")	
		tcpipAutoConfigNetworkGroup.setNodeVisible("tcpipIgmp", "libtcpipIgmp")
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipIgmp"])
	
def tcpipAutoConfigIPv4Enable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipIPv4"],"NETWORK LAYER")	
		tcpipAutoConfigNetworkGroup.setNodeVisible("tcpipIPv4", "libTcpipIPv4")
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_ARP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipIPv4"])
	
def tcpipAutoConfigIPv6Enable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipIPv6"],"NETWORK LAYER")	
		tcpipAutoConfigNetworkGroup.setNodeVisible("tcpipIPv6", "libTcpipIPv6")
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_NDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipIPv6"])
	
def tcpipAutoConfigNDPEnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipNdp"],"NETWORK LAYER")	
		tcpipAutoConfigNetworkGroup.setNodeVisible("tcpipNdp", "libtcpipNdp")
	else:
		res = Database.deactivateComponents(["tcpipNdp"])

#################Business Logic- Application Layer #########################################		
def tcpipAutoConfigANNOUNCEEnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipAnnounce"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipAnnounce", "libtcpipAnnounce")
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipAnnounce"])
	
def tcpipAutoConfigBerkeleyAPIEnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipBerkeleyApi"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipBerkeleyApi", "libtcpipBerkeleyApi")
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipBerkeleyApi", "BSD_NETPRES_Dependency")
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_TCP", True, 2)
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipBerkeleyApi"])
	
def tcpipAutoConfigDDNSEnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipDdns"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipDdns", "libtcpipDdns")
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipDdns"])
	
def tcpipAutoConfigDHCPCLIENTEnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipDhcp"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipDhcp", "libtcpipDhcp")
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipDhcp"])
	
def tcpipAutoConfigDHCPSERVEREnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipDhcps"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipDhcps", "libtcpipDhcps")
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipDhcps"])
	
def tcpipAutoConfigDNSCLIENTEnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipDns"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipDns", "libtcpipDns")
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipDns"])
	
def tcpipAutoConfigDNSSERVEREnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipDnss"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipDnss", "libtcpipDnss")
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipDnss"])
	

def tcpipAutoConfigFTPSERVEREnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipFtps"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipFtps", "libtcpipFtps")
		res = Database.activateComponents(["tcpipSysFsWrapper"], "TCP/IP BASIC CONFIGURATION")
		tcpipAutoConfigBasicGroup.setNodeVisible("tcpipSysFsWrapper", "TcpipFsWarapper_SysFS_Dependency")
		
		rootGroup = Database.getRootGroup()
		rootGroup.addComponent("sys_fs")		
		res = Database.activateComponents(["sys_fs"])
		
		res = Database.connectDependencies(autoConnectTableFS)
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_TCP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipFtps"])
	
def tcpipAutoConfigHTTPNETSERVEREnable(symbol, event):
	enableTcpipAutoConfig(True)
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
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_TCP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipHttpNet"])
	
def tcpipAutoConfigHTTPSERVEREnable(symbol, event):
	enableTcpipAutoConfig(True)
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
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_TCP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipHttp"])
	
def tcpipAutoConfigIPERFEnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipIperf"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipIperf", "libtcpipIperf")
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_TCP", True, 2)
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipIperf"])
	
def tcpipAutoConfigNBNSEnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipNbns"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipNbns", "libtcpipNbns")
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)		
	else:
		res = Database.deactivateComponents(["tcpipNbns"])
	
def tcpipAutoConfigREBOOTEnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipReboot"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipReboot", "libtcpipReboot")
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True, 2)
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_UDP", True, 2)
	else:
		res = Database.deactivateComponents(["tcpipReboot"])
	
def tcpipAutoConfigSMTPCLIENTEnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipSmtpc"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipSmtpc", "libtcpipSmtpc")
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipSmtpc", "Smtpc_NetPres_Dependency")
	else:
		res = Database.deactivateComponents(["tcpipSmtpc"])
	
def tcpipAutoConfigSNMPEnable(symbol, event):
	enableTcpipAutoConfig(True)
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
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipSnmpv3"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipSnmpv3", "libtcpipSnmpv3")
	else:
		res = Database.deactivateComponents(["tcpipSnmpv3"])

def tcpipAutoConfigSNTPEnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipSntp"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipSntp", "libtcpipSntp")
	else:
		res = Database.deactivateComponents(["tcpipSntp"])
	
def tcpipAutoConfigTELNETEnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipTelnet"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipTelnet", "libtcpipTelnet")
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipTelnet", "Telnet_NetPres_Dependency")
	else:
		res = Database.deactivateComponents(["tcpipTelnet"])
	
def tcpipAutoConfigTFTPCLIENTEnable(symbol, event):
	enableTcpipAutoConfig(True)
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
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipZeroConf"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipZeroConf", "libtcpipZeroConf")
	else:
		res = Database.deactivateComponents(["tcpipZeroConf"])

def tcpipAutoConfigCMDEnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipCmd"],"APPLICATION LAYER")	
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipCmd", "libtcpipCmd")
		tcpipAutoConfigAppsGroup.setNodeVisible("tcpipCmd", "Cmd_SysConsole_Dependency")
	else:
		res = Database.deactivateComponents(["tcpipCmd"])

#################Business Logic- Transport Layer #########################################	
def tcpipAutoConfigTCPEnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipTcp"], "TRANSPORT LAYER")	
		tcpipAutoConfigTransportGroup.setNodeVisible("tcpipTcp", "libtcpipTcp")
	else:
		res = Database.deactivateComponents(["tcpipTcp"])
	
def tcpipAutoConfigUDPEnable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["tcpipUdp"], "TRANSPORT LAYER")	
		tcpipAutoConfigTransportGroup.setNodeVisible("tcpipUdp", "libtcpipUdp")		
	else:
		res = Database.deactivateComponents(["tcpipUdp"])
	
	
	
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
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", True, 2)		
	else:
		res = Database.deactivateComponents(["drvExtPhyKsz8061"])
	
	
def tcpipAutoConfigLAN8740Enable(symbol, event):
	enableTcpipAutoConfig(True)
	if (event["value"] == True):
		res = Database.activateComponents(["drvExtPhyLan8740"],"DRIVER LAYER")	
		Database.setSymbolValue("tcpip_template", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", True, 2)		
	else:
		res = Database.deactivateComponents(["drvExtPhyLan8740"])
	
