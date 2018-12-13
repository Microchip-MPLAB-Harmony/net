######################  TCP/IP LIBRARY  ######################
def loadModule():
	print("Load Module: Harmony TCP/IP Stack")
	
	###########  TCP/IP LIBRARY General Configurations  ###########	
	tcpipStackComponent = Module.CreateSharedComponent("tcpipStack", "TCPIP CORE", "/Libraries/TCPIP/CORE/", "tcpip/config/tcpip_stack.py")
	tcpipStackComponent.addCapability("libtcpipStack","TCPIP_CORE",True)
	tcpipStackComponent.addDependency("Core_NetConfig_Dependency", "NETCONFIG", None, True, True)
	tcpipStackComponent.addDependency("Core_SysTime_Dependency", "SYS_TIME", None, True, True)
	tcpipStackComponent.addDependency("Core_SysConsole_Dependency", "SYS_CONSOLE")
	tcpipStackComponent.setDisplayType("TCP/IP Library")
		
	tcpipNetConfigComponent = Module.CreateGeneratorComponent("tcpipNetConfig", "NETCONFIG", "/Libraries/TCPIP/CORE/","tcpip/config/tcpip_network_config_common.py","tcpip/config/tcpip_network_config.py")
	tcpipNetConfigComponent.addDependency("NETCONFIG_MAC_Dependency", "MAC")
	tcpipNetConfigComponent.addCapability("libtcpipNetConfig","NETCONFIG",True)
	tcpipNetConfigComponent.setDisplayType("TCP/IP Library")
	
	tcpipSysFsWrapperComponent = Module.CreateSharedComponent("tcpipSysFsWrapper", "TCPIP File System Wrapper", "/Libraries/TCPIP/CORE/", "tcpip/config/tcpip_sysfs_wrapper.py")
	tcpipSysFsWrapperComponent.addCapability("libtcpipSysFsWrapper","TCPIP_FS_WRAPPER",True)	
	tcpipSysFsWrapperComponent.addDependency("TcpipFsWarapper_SysFS_Dependency", "SYS_FS")	
	tcpipSysFsWrapperComponent.setDisplayType("TCP/IP Library")
	
	tcpipCmdComponent = Module.CreateComponent("tcpipCmd", "TCPIP CMD", "/Libraries/TCPIP/CORE/", "tcpip/config/tcpip_cmd.py")
	tcpipCmdComponent.addCapability("libtcpipCmd","TCPIP_CMD",True)	
	# tcpipCmdComponent.addDependency("Cmd_Stack_Dependency", "TCPIP_CORE", None, True, True)
	# tcpipCmdComponent.addDependency("Cmd_SysConsole_Dependency", "SYS_CONSOLE")
	tcpipCmdComponent.setDisplayType("TCP/IP Library")

	###########  TCP/IP LIBRARY Network Layer Configurations  ###########
	tcpipArpComponent = Module.CreateComponent("tcpipArp", "ARP", "/Libraries/TCPIP/Layer3-NETWORK/", "tcpip/config/tcpip_arp.py")
	tcpipArpComponent.addCapability("libtcpipArp","ARP", True)
	tcpipArpComponent.setDisplayType("TCP/IP Library")
	
	tcpipIcmpComponent = Module.CreateComponent("tcpipIcmp", "ICMPv4", "/Libraries/TCPIP/Layer3-NETWORK/", "tcpip/config/tcpip_icmp.py")
	tcpipIcmpComponent.addCapability("libtcpipIcmp","ICMPv4",True)	
	tcpipIcmpComponent.addDependency("Icmp_IPv4_Dependency", "IPv4", None, True, True)
	tcpipIcmpComponent.setDisplayType("TCP/IP Library")

	tcpipIgmpComponent = Module.CreateComponent("tcpipIgmp", "IGMP", "/Libraries/TCPIP/Layer3-NETWORK/", "tcpip/config/tcpip_igmp.py")
	tcpipIgmpComponent.addCapability("libtcpipIgmp","IGMP",True)
	tcpipIgmpComponent.addDependency("Igmp_IPv4_Dependency", "IPv4", None, True, True)
	tcpipIgmpComponent.setDisplayType("TCP/IP Library")
	
	tcpipIPv4Component = Module.CreateSharedComponent("tcpipIPv4", "IPv4", "/Libraries/TCPIP/Layer3-NETWORK/", "tcpip/config/tcpip_ipv4.py")
	tcpipIPv4Component.addCapability("libTcpipIPv4","IPv4",True)
	tcpipIPv4Component.addCapability("libTcpipIPv4IP","IP",True)
	#niyas tcpipIPv4Component.addDependency("Ipv4_Stack_Dependency", "TCPIP_CORE", None, True, True)	
	tcpipIPv4Component.addDependency("Ipv4_Arp_Dependency", "ARP", None, True, True)	
	tcpipIPv4Component.setDisplayType("TCP/IP Library")
	
	tcpipIPv6Component = Module.CreateSharedComponent("tcpipIPv6", "IPv6", "/Libraries/TCPIP/Layer3-NETWORK/", "tcpip/config/tcpip_ipv6.py")
	tcpipIPv6Component.addCapability("libTcpipIPv6","IPv6",True)
	tcpipIPv6Component.addCapability("libTcpipIPv6IP","IP",True)
	tcpipIPv6Component.addDependency("Ipv6_Ndp_Dependency", "NDP", None, True, True)
	tcpipIPv6Component.setDisplayType("TCP/IP Library")
	
	tcpipNdpComponent = Module.CreateComponent("tcpipNdp", "NDP", "/Libraries/TCPIP/Layer3-NETWORK/", "tcpip/config/tcpip_ndp.py")
	tcpipNdpComponent.addCapability("libtcpipNdp","NDP",True)
	tcpipNdpComponent.setDisplayType("TCP/IP Library")
	###########  TCP/IP LIBRARY Transport Layer Configurations  ###########
	tcpipTcpComponent = Module.CreateSharedComponent("tcpipTcp", "TCP", "/Libraries/TCPIP/Layer4-TRANSPORT/", "tcpip/config/tcpip_tcp.py")
	tcpipTcpComponent.addCapability("libtcpipTcp","TCP",True)
	tcpipTcpComponent.addDependency("Tcp_IP_Dependency", "IP", None, True, True)
	tcpipTcpComponent.setDisplayType("TCP/IP Library")
	
	tcpipUdpComponent = Module.CreateSharedComponent("tcpipUdp", "UDP", "/Libraries/TCPIP/Layer4-TRANSPORT/", "tcpip/config/tcpip_udp.py")
	tcpipUdpComponent.addCapability("libtcpipUdp","UDP",True)
	tcpipUdpComponent.addDependency("Udp_IP_Dependency", "IP", None, True, True)
	tcpipUdpComponent.setDisplayType("TCP/IP Library")

	###########  TCP/IP LIBRARY Application Layer Configurations  ###########	
	tcpipAnnounceComponent = Module.CreateComponent("tcpipAnnounce", "ANNOUNCE", "/Libraries/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_announce.py")
	tcpipAnnounceComponent.addCapability("libtcpipAnnounce","ANNOUNCE",True)
	tcpipAnnounceComponent.addDependency("Announce_IPv4_Dependency", "IPv4", None, True, True)
	tcpipAnnounceComponent.addDependency("Announce_UDP_Dependency", "UDP", None, True, True)	
	tcpipAnnounceComponent.setDisplayType("TCP/IP Library")
	
	tcpipBerkeleyApiComponent = Module.CreateComponent("tcpipBerkeleyApi", "Berkeley API", "/Libraries/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_berkeley_api.py")
	tcpipBerkeleyApiComponent.addCapability("libtcpipBerkeleyApi","BSD",True)	
	tcpipBerkeleyApiComponent.addDependency("BSD_TCP_Dependency", "TCP", None, True, True)
	tcpipBerkeleyApiComponent.addDependency("BSD_UDP_Dependency", "UDP", None, True, True)
	tcpipBerkeleyApiComponent.addDependency("BSD_NETPRES_Dependency", "net_pres")
	tcpipBerkeleyApiComponent.setDisplayType("TCP/IP Library")
	
	tcpipDdnsComponent = Module.CreateComponent("tcpipDdns", "DDNS", "/Libraries/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_ddns.py")
	tcpipDdnsComponent.addCapability("libtcpipDdns","DDNS",True)	
	tcpipDdnsComponent.addDependency("Ddns_UDP_Dependency", "UDP", None, True, True)
	tcpipDdnsComponent.addDependency("Ddns_IPv4_Dependency", "IPv4", None, True, True)	
	tcpipDdnsComponent.setDisplayType("TCP/IP Library")
	
	tcpipDhcpComponent = Module.CreateComponent("tcpipDhcp", "DHCP CLIENT", "/Libraries/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_dhcp.py")
	tcpipDhcpComponent.addCapability("libtcpipDhcp","DHCPC",True)
	tcpipDhcpComponent.addDependency("Dhcpc_IPv4_Dependency", "IPv4", None, True, True)
	tcpipDhcpComponent.addDependency("Dhcpc_UDP_Dependency", "UDP", None, True, True)
	tcpipDhcpComponent.setDisplayType("TCP/IP Library")
	
	tcpipDhcpsComponent = Module.CreateComponent("tcpipDhcps", "DHCP SERVER", "/Libraries/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_dhcps.py")
	tcpipDhcpsComponent.addCapability("libtcpipDhcps","DHCPS",True)
	tcpipDhcpsComponent.addDependency("Dhcps_IPv4_Dependency", "IPv4", None, True, True)
	tcpipDhcpsComponent.addDependency("Dhcps_UDP_Dependency", "UDP", None, True, True)	
	tcpipDhcpsComponent.setDisplayType("TCP/IP Library")
	
	tcpipDnsComponent = Module.CreateComponent("tcpipDns", "DNS CLIENT", "/Libraries/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_dns.py")
	tcpipDnsComponent.addCapability("libtcpipDns","DNSC",True)	
	tcpipDnsComponent.addDependency("Dns_UDP_Dependency", "UDP", None, True, True)
	tcpipDnsComponent.setDisplayType("TCP/IP Library")
	
	tcpipDnssComponent = Module.CreateComponent("tcpipDnss", "DNS SERVER", "/Libraries/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_dnss.py")
	tcpipDnssComponent.addCapability("libtcpipDnss","DNSS",True)	
	tcpipDnssComponent.addDependency("Dnss_UDP_Dependency", "UDP", None, True, True)	
	tcpipDnssComponent.setDisplayType("TCP/IP Library")
	
	tcpipFtpsComponent = Module.CreateComponent("tcpipFtps", "FTP SERVER", "/Libraries/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_ftps.py")
	tcpipFtpsComponent.addCapability("libtcpipFtps","FTPS",True)	
	tcpipFtpsComponent.addDependency("Ftps_TCP_Dependency", "TCP", None, True, True)	
	tcpipFtpsComponent.addDependency("Ftps_IPv4_Dependency", "IPv4", None, True, True)	
	tcpipFtpsComponent.addDependency("Ftps_TcpipFs_Dependency", "TCPIP_FS_WRAPPER", None, True, True)
	tcpipFtpsComponent.setDisplayType("TCP/IP Library")
	
	tcpipHttpNetComponent = Module.CreateComponent("tcpipHttpNet", "HTTPNET SERVER", "/Libraries/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_httpnet.py")
	tcpipHttpNetComponent.addCapability("libtcpipHttpNet","HTTPNET",True)	
	tcpipHttpNetComponent.addDependency("HttpNet_TCP_Dependency", "TCP", None, True, True)	
	tcpipHttpNetComponent.addDependency("HttpNet_TcpipFs_Dependency", "TCPIP_FS_WRAPPER", None, True, True)
	tcpipHttpNetComponent.addDependency("HttpNet_Crypto_Dependency", "LIB_CRYPTO")
	tcpipHttpNetComponent.addDependency("HttpNet_NetPres_Dependency", "net_pres")
	tcpipHttpNetComponent.setDisplayType("TCP/IP Library")
	
	tcpipHttpComponent = Module.CreateComponent("tcpipHttp", "HTTP SERVER", "/Libraries/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_http.py")
	tcpipHttpComponent.addCapability("libtcpipHttp","HTTP",True)
	tcpipHttpComponent.addDependency("Http_TCP_Dependency", "TCP", None, True, True)
	tcpipHttpComponent.addDependency("Http_TcpipFs_Dependency", "TCPIP_FS_WRAPPER", None, True, True)
	tcpipHttpComponent.addDependency("Http_Crypto_Dependency", "LIB_CRYPTO")
	tcpipHttpComponent.setDisplayType("TCP/IP Library")

	tcpipIperfComponent = Module.CreateComponent("tcpipIperf", "IPERF", "/Libraries/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_iperf.py")
	tcpipIperfComponent.addCapability("libtcpipIperf","IPERF",True)
	tcpipIperfComponent.addDependency("Iperf_TCP_Dependency", "TCP", None, True, True)
	tcpipIperfComponent.addDependency("Iperf_UDP_Dependency", "UDP", None, True, True)
	tcpipIperfComponent.setDisplayType("TCP/IP Library")
	
	tcpipNbnsComponent = Module.CreateComponent("tcpipNbns", "NBNS", "/Libraries/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_nbns.py")
	tcpipNbnsComponent.addCapability("libtcpipNbns","NBNS",True)
	tcpipNbnsComponent.addDependency("Nbns_IPv4_Dependency", "IPv4", None, True, True)
	tcpipNbnsComponent.addDependency("Nbns_UDP_Dependency", "UDP", None, True, True)	
	tcpipNbnsComponent.setDisplayType("TCP/IP Library")
	
	tcpipRebootComponent = Module.CreateComponent("tcpipReboot", "REBOOT", "/Libraries/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_reboot.py")
	tcpipRebootComponent.addCapability("libtcpipReboot","REBOOT",True)
	tcpipRebootComponent.addDependency("Reboot_IPv4_Dependency", "IPv4", None, True, True)
	tcpipRebootComponent.addDependency("Reboot_UDP_Dependency", "UDP", None, True, True)
	tcpipRebootComponent.setDisplayType("TCP/IP Library")

	tcpipSmtpcComponent = Module.CreateComponent("tcpipSmtpc", "SMTP CLIENT", "/Libraries/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_smtpc.py")
	tcpipSmtpcComponent.addCapability("libtcpipSmtpc","SMTPC",True)
	tcpipSmtpcComponent.addDependency("Smtpc_TCP_Dependency", "TCP", None, True, True)	
	tcpipSmtpcComponent.addDependency("Smtpc_NetPres_Dependency", "net_pres")
	tcpipSmtpcComponent.setDisplayType("TCP/IP Library")
	
	tcpipSnmpComponent = Module.CreateComponent("tcpipSnmp", "SNMP", "/Libraries/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_snmp.py")
	tcpipSnmpComponent.addCapability("libtcpipSnmp","SNMP",True)	
	tcpipSnmpComponent.addDependency("Snmp_UDP_Dependency", "UDP", None, True, True)	
	tcpipSnmpComponent.addDependency("Snmp_TcpipFs_Dependency", "TCPIP_FS_WRAPPER", None, True, True)
	tcpipSnmpComponent.addDependency("Snmp_Crypto_Dependency", "LIB_CRYPTO")
	tcpipSnmpComponent.setDisplayType("TCP/IP Library")
	
	tcpipSnmpv3Component = Module.CreateComponent("tcpipSnmpv3", "SNMPV3", "/Libraries/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_snmpv3.py")
	tcpipSnmpv3Component.addCapability("libtcpipSnmpv3","SNMPV3",True)	
	tcpipSnmpv3Component.addDependency("Snmpv3_SNMP_Dependency", "SNMP", None, True, True)
	tcpipSnmpv3Component.setDisplayType("TCP/IP Library")

	tcpipSntpComponent = Module.CreateComponent("tcpipSntp", "SNTP", "/Libraries/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_sntp.py")
	tcpipSntpComponent.addCapability("libtcpipSntp","SNTP",True)	
	tcpipSntpComponent.addDependency("Sntp_UDP_Dependency", "UDP", None, True, True)
	#tcpipSntpComponent.addDependency("Sntp_MAC_Dependency", "MAC")
	tcpipSntpComponent.setDisplayType("TCP/IP Library")
	
	tcpipTelnetComponent = Module.CreateComponent("tcpipTelnet", "TELNET", "/Libraries/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_telnet.py")
	tcpipTelnetComponent.addCapability("libtcpipTelnet","TELNET",True)
	tcpipTelnetComponent.addDependency("Telnet_TcpipCmd_Dependency", "TCPIP_CMD", None, True, True)
	tcpipTelnetComponent.addDependency("Telnet_NetPres_Dependency", "net_pres")
	tcpipTelnetComponent.setDisplayType("TCP/IP Library")
	
	tcpipTftpcComponent = Module.CreateComponent("tcpipTftpc", "TFTP CLIENT", "/Libraries/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_tftpc.py")
	tcpipTftpcComponent.addCapability("libtcpipTftpc","TFTPC",True)
	tcpipTftpcComponent.addDependency("Tftpc_IPv4_Dependency", "IPv4", None, True, True)
	tcpipTftpcComponent.addDependency("Tftpc_UDP_Dependency", "UDP", None, True, True)	
	#tcpipTftpcComponent.addDependency("Tftpc_MAC_Dependency", "MAC")
	tcpipTftpcComponent.addDependency("Tftpc_TcpipFs_Dependency", "TCPIP_FS_WRAPPER", None, True, True)
	tcpipTftpcComponent.setDisplayType("TCP/IP Library")
	
	tcpipTftpsComponent = Module.CreateComponent("tcpipTftps", "TFTP SERVER", "/Libraries/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_tftps.py")
	tcpipTftpsComponent.addCapability("libtcpipTftps","TFTPS",True)
	tcpipTftpsComponent.addDependency("Tftps_IPv4_Dependency", "IPv4", None, True, True)
	tcpipTftpsComponent.addDependency("Tftps_UDP_Dependency", "UDP", None, True, True)	
	#tcpipTftpsComponent.addDependency("Tftps_MAC_Dependency", "MAC")
	tcpipTftpsComponent.addDependency("Tftps_TcpipFs_Dependency", "TCPIP_FS_WRAPPER", None, True, True)
	tcpipTftpsComponent.setDisplayType("TCP/IP Library")
	
	tcpipZeroConfComponent = Module.CreateComponent("tcpipZeroConf", "ZEROCONF", "/Libraries/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_zeroconf.py")
	tcpipZeroConfComponent.addCapability("libtcpipZeroConf","ZEROCONF",True)
	tcpipZeroConfComponent.addDependency("ZeroConf_IPv4_Dependency", "IPv4", None, True, True)
	tcpipZeroConfComponent.addDependency("ZeroConf_UDP_Dependency", "UDP", None, True, True)
	tcpipZeroConfComponent.setDisplayType("TCP/IP Library")

	###########  TCP/IP LIBRARY Datalink & Physical Layer Configurations  ###########
	if Peripheral.moduleExists("GMAC"):
		drvSamv71GmacComponent = Module.CreateComponent("drvSamv71Gmac", "GMAC", "/Harmony/Drivers/MAC Driver/Internal/", "driver/gmac/config/drv_intmac_gmac.py")
		drvSamv71GmacComponent.addCapability("libdrvSamv71Gmac","MAC")
		drvSamv71GmacComponent.addDependency("GMAC_PHY_Dependency", "PHY", None, True, True)	
	elif "PIC32M" in Variables.get("__PROCESSOR"):
		drvPic32mEthmacComponent = Module.CreateComponent("drvPic32mEthmac", "ETHMAC", "/Harmony/Drivers/MAC Driver/Internal/", "driver/ethmac/config/drv_intmac_ethmac.py")
		drvPic32mEthmacComponent.addCapability("libdrvPic32mEthmac","MAC")
		drvPic32mEthmacComponent.addDependency("ETHMAC_PHY_Dependency", "PHY", None, True, True)
	

	## MIIM Driver
	drvMiimComponent = Module.CreateComponent("drvMiim", "MIIM Driver", "/Harmony/Drivers/", "driver/miim/config/drv_miim.py")
	drvMiimComponent.addCapability("libdrvMiim","MIIM",True)	
	
	## External PHY Driver
	drvExtPhyKsz8061Component = Module.CreateComponent("drvExtPhyKsz8061", "KSZ8061", "/Harmony/Drivers/PHY Driver", "driver/ethphy/config/drv_extphy_ksz8061.py")
	drvExtPhyKsz8061Component.addCapability("libdrvExtPhyKsz8061","PHY",True)	
	drvExtPhyKsz8061Component.addDependency("KSZ8061_MIIM_Dependency", "MIIM", None, True, True)	
	
	drvExtPhyLan8740Component = Module.CreateComponent("drvExtPhyLan8740", "LAN8740", "/Harmony/Drivers/PHY Driver", "driver/ethphy/config/drv_extphy_lan8740.py")
	drvExtPhyLan8740Component.addCapability("libdrvExtPhyLan8740","PHY",True)	
	drvExtPhyLan8740Component.addDependency("LAN8740_MIIM_Dependency", "MIIM", None, True, True)		

	########################## Harmony Network Presentation Module #################################
	netPresComponent = Module.CreateGeneratorComponent("netPres", "Presentation Layer", "/Harmony/Harmony Networking","net/pres/config/netPres_common.py","net/pres/config/netPres.py")
	netPresComponent.addCapability("libNetPres","net_pres")	
	############################### Third Party wolfSSL Module #####################################
	tlsComponent = Module.CreateComponent("lib_wolfssl", "wolfSSL Library", "//Third Party Libraries/wolfSSL/", "config\wolfssl.py")
	
	############################### TCP/IP STACK CONFIGURATOR #####################################
	#tcpipAutoConfigComponent = Module.CreateComponent("tcpip_template", "TCP/IP Stack Configurator", "/Libraries/TCPIP/", "tcpip/config/tcpip_templates.py")

	tcpipAutoConfigAppsComponent = Module.CreateComponent("tcpip_apps_config", "TCP/IP Application Layer Configurator", "/Libraries/TCPIP/", "tcpip/config/tcpip_configurator_apps.py")

	tcpipAutoConfigTransportComponent = Module.CreateComponent("tcpip_transport_config", "TCP/IP Transport Layer Configurator", "/Libraries/TCPIP/", "tcpip/config/tcpip_configurator_transport.py")

	tcpipAutoConfigNetworkComponent = Module.CreateComponent("tcpip_network_config", "TCP/IP Network Layer Configurator", "/Libraries/TCPIP/", "tcpip/config/tcpip_configurator_network.py")

	tcpipAutoConfigDriverComponent = Module.CreateComponent("tcpip_driver_config", "TCP/IP Driver Layer Configurator", "/Libraries/TCPIP/", "tcpip/config/tcpip_configurator_driver.py")

	tcpipAutoConfigBasicComponent = Module.CreateComponent("tcpip_basic_config", "TCP/IP Basic Configurator", "/Libraries/TCPIP/", "tcpip/config/tcpip_configurator_basic.py")
