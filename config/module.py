"""
Copyright (C) 2019-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
"""

######################  TCP/IP LIBRARY  ######################
def loadModule():
    print("Load Module: Harmony TCP/IP Stack")
    processor = Variables.get("__PROCESSOR")
    seriesNode = ATDF.getNode('/avr-tools-device-file/devices')
    deviceSeries = seriesNode.getChildren()[0].getAttribute("series")
    ###########  TCP/IP LIBRARY General Configurations  ########### 
    tcpipStackComponent = Module.CreateSharedComponent("tcpipStack", "TCPIP CORE", "/TCPIP/CORE/", "tcpip/config/tcpip_stack.py")
    tcpipStackComponent.addCapability("libtcpipStack","TCPIP_CORE",True)
    tcpipStackComponent.addDependency("Core_NetConfig_Dependency", "NETCONFIG", None, True, True)
    tcpipStackComponent.addDependency("Core_SysTime_Dependency", "SYS_TIME", None, True, True)
    tcpipStackComponent.addDependency("Core_SysConsole_Dependency", "SYS_CONSOLE", None, False, False)
    tcpipStackComponent.setDisplayType("TCP/IP Library")
        
    tcpipNetConfigComponent = Module.CreateGeneratorComponent("tcpipNetConfig", "NETCONFIG", "/TCPIP/CORE/","tcpip/config/tcpip_network_config_common.py","tcpip/config/tcpip_network_config.py")
    tcpipNetConfigComponent.addDependency("NETCONFIG_MAC_Dependency", "MAC")
    tcpipNetConfigComponent.addCapability("libtcpipNetConfig","NETCONFIG",True)
    tcpipNetConfigComponent.setDisplayType("TCP/IP Library")
    
    tcpipCmdComponent = Module.CreateComponent("tcpipCmd", "TCPIP CMD", "/TCPIP/CORE/", "tcpip/config/tcpip_cmd.py")
    tcpipCmdComponent.addCapability("libtcpipCmd","TCPIP_CMD",True) 
    tcpipCmdComponent.addDependency("TcpipCmd_SysCmd_Dependency", "SYS_COMMAND", None, True, True)
    tcpipCmdComponent.setDisplayType("TCP/IP Library")

    ###########  TCP/IP LIBRARY Network Layer Configurations  ###########
    tcpipArpComponent = Module.CreateComponent("tcpipArp", "ARP", "/TCPIP/Layer3-NETWORK/", "tcpip/config/tcpip_arp.py")
    tcpipArpComponent.addCapability("libtcpipArp","ARP", True)
    tcpipArpComponent.setDisplayType("Network Layer")
    
    tcpipIcmpComponent = Module.CreateComponent("tcpipIcmp", "ICMPv4", "/TCPIP/Layer3-NETWORK/", "tcpip/config/tcpip_icmp.py")
    tcpipIcmpComponent.addCapability("libtcpipIcmp","ICMPv4",True)  
    tcpipIcmpComponent.addDependency("Icmp_IPv4_Dependency", "IPv4", None, True, True)
    tcpipIcmpComponent.setDisplayType("Network Layer")

    tcpipIgmpComponent = Module.CreateComponent("tcpipIgmp", "IGMP", "/TCPIP/Layer3-NETWORK/", "tcpip/config/tcpip_igmp.py")
    tcpipIgmpComponent.addCapability("libtcpipIgmp","IGMP",True)
    tcpipIgmpComponent.addDependency("Igmp_IPv4_Dependency", "IPv4", None, True, True)
    tcpipIgmpComponent.setDisplayType("Network Layer")
    
    tcpipIPv4Component = Module.CreateSharedComponent("tcpipIPv4", "IPv4", "/TCPIP/Layer3-NETWORK/", "tcpip/config/tcpip_ipv4.py")
    tcpipIPv4Component.addCapability("libTcpipIPv4","IPv4",True)
    tcpipIPv4Component.addCapability("libTcpipIPv4IP","IP",True)
    #tcpipIPv4Component.addDependency("Ipv4_Stack_Dependency", "TCPIP_CORE", None, True, True)  
    tcpipIPv4Component.addDependency("Ipv4_Arp_Dependency", "ARP", None, True, True)    
    tcpipIPv4Component.setDisplayType("Network Layer")
    
    tcpipIPv6Component = Module.CreateSharedComponent("tcpipIPv6", "IPv6", "/TCPIP/Layer3-NETWORK/", "tcpip/config/tcpip_ipv6.py")
    tcpipIPv6Component.addCapability("libTcpipIPv6","IPv6",True)
    tcpipIPv6Component.addCapability("libTcpipIPv6IP","IP",True)
    tcpipIPv6Component.addDependency("Ipv6_Ndp_Dependency", "NDP", None, True, True)
    tcpipIPv6Component.addDependency("Ipv6_Icmpv6_Dependency", "ICMPv6", None, True, True)
    tcpipIPv6Component.setDisplayType("Network Layer")
    
    tcpipIcmpv6Component = Module.CreateComponent("tcpipIcmpv6", "ICMPv6", "/TCPIP/Layer3-NETWORK/", "tcpip/config/tcpip_icmpv6.py")
    tcpipIcmpv6Component.addCapability("libtcpipIcmpv6","ICMPv6",True)  
    tcpipIcmpv6Component.setDisplayType("Network Layer")
    
    tcpipNdpComponent = Module.CreateComponent("tcpipNdp", "NDP", "/TCPIP/Layer3-NETWORK/", "tcpip/config/tcpip_ndp.py")
    tcpipNdpComponent.addCapability("libtcpipNdp","NDP",True)
    tcpipNdpComponent.setDisplayType("Network Layer")

    ###########  TCP/IP LIBRARY Transport Layer Configurations  ###########
    tcpipTcpComponent = Module.CreateSharedComponent("tcpipTcp", "TCP", "/TCPIP/Layer4-TRANSPORT/", "tcpip/config/tcpip_tcp.py")
    tcpipTcpComponent.addCapability("libtcpipTcp","TCP",True)
    tcpipTcpComponent.addDependency("Tcp_IP_Dependency", "IP", None, True, True)
    tcpipTcpComponent.addDependency("Tcp_Crypto_Dependency", "LIB_CRYPTO", None, True, True)
    tcpipTcpComponent.setDisplayType("Transport Layer")
    
    tcpipUdpComponent = Module.CreateSharedComponent("tcpipUdp", "UDP", "/TCPIP/Layer4-TRANSPORT/", "tcpip/config/tcpip_udp.py")
    tcpipUdpComponent.addCapability("libtcpipUdp","UDP",True)
    tcpipUdpComponent.addDependency("Udp_IP_Dependency", "IP", None, True, True)
    tcpipUdpComponent.setDisplayType("Transport Layer")

    ########################## TCP/IP LIBRARY Presentation Layer Configurations #################################    
    net_PresComponent = Module.CreateComponent("net_Pres", "Presentation Layer", "/TCPIP/Layer6-PRESENTATION", "net_pres/pres/config/net_pres.py")
    net_PresComponent.addCapability("libNet_Pres","net_pres",True)    
    net_PresComponent.addDependency("Net_Pres_Crypto_Dependency", "TLS Provider", None, False, False)
    net_PresComponent.addDependency("Net_Pres_SNTP_Dependency", "SNTP", None, True, True)
    net_PresComponent.setDisplayType("Presentation Layer")
    
    ###########  TCP/IP LIBRARY Application Layer Configurations  ###########   
    tcpipAnnounceComponent = Module.CreateComponent("tcpipAnnounce", "ANNOUNCE", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_announce.py")
    tcpipAnnounceComponent.addCapability("libtcpipAnnounce","ANNOUNCE",True)
    tcpipAnnounceComponent.addDependency("Announce_IPv4_Dependency", "IPv4", None, True, True)
    tcpipAnnounceComponent.addDependency("Announce_UDP_Dependency", "UDP", None, True, True)    
    tcpipAnnounceComponent.setDisplayType("Application Layer")
    
    tcpipBerkeleyApiComponent = Module.CreateComponent("tcpipBerkeleyApi", "Berkeley API", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_berkeley_api.py")
    tcpipBerkeleyApiComponent.addCapability("libtcpipBerkeleyApi","BSD",True)   
    tcpipBerkeleyApiComponent.addDependency("BSD_TCP_Dependency", "TCP", None, True, True)
    tcpipBerkeleyApiComponent.addDependency("BSD_UDP_Dependency", "UDP", None, True, True)
    tcpipBerkeleyApiComponent.addDependency("BSD_NETPRES_Dependency", "net_pres", True, True)
    tcpipBerkeleyApiComponent.addDependency("BSD_DNSC_Dependency", "DNSC", None, True, True)
    tcpipBerkeleyApiComponent.setDisplayType("Application Layer")
    
    tcpipDdnsComponent = Module.CreateComponent("tcpipDdns", "DDNS", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_ddns.py")
    tcpipDdnsComponent.addCapability("libtcpipDdns","DDNS",True)    
    tcpipDdnsComponent.addDependency("Ddns_UDP_Dependency", "UDP", None, True, True)
    tcpipDdnsComponent.addDependency("Ddns_IPv4_Dependency", "IPv4", None, True, True)  
    tcpipDdnsComponent.setDisplayType("Application Layer")
    
    tcpipDhcpComponent = Module.CreateComponent("tcpipDhcp", "DHCP CLIENT", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_dhcp.py")
    tcpipDhcpComponent.addCapability("libtcpipDhcp","DHCPC",True)
    tcpipDhcpComponent.addDependency("Dhcpc_IPv4_Dependency", "IPv4", None, True, True)
    tcpipDhcpComponent.addDependency("Dhcpc_UDP_Dependency", "UDP", None, True, True)
    tcpipDhcpComponent.setDisplayType("Application Layer")
    
    tcpipDhcpsComponent = Module.CreateComponent("tcpipDhcps", "DHCP SERVER", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_dhcps.py")
    tcpipDhcpsComponent.addCapability("libtcpipDhcps","DHCPS",True)
    tcpipDhcpsComponent.addDependency("Dhcps_IPv4_Dependency", "IPv4", None, True, True)
    tcpipDhcpsComponent.addDependency("Dhcps_UDP_Dependency", "UDP", None, True, True) 
    tcpipDhcpsComponent.addDependency("Dhcps_Icmp_Dependency", "ICMPv4", None, True, True)
    tcpipDhcpsComponent.setDisplayType("Application Layer")
    
    # DHCP Server new module
    tcpipDhcpServerComponent = Module.CreateComponent("tcpipDhcpServer", "DHCP SERVER v2", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_dhcp_server.py")
    tcpipDhcpServerComponent.addCapability("libtcpipDhcpServer","DHCPS",True)
    tcpipDhcpServerComponent.addDependency("DhcpServer_IPv4_Dependency", "IPv4", None, True, True)
    tcpipDhcpServerComponent.addDependency("DhcpServer_UDP_Dependency", "UDP", None, True, True) 
    tcpipDhcpServerComponent.addDependency("DhcpServer_Icmp_Dependency", "ICMPv4", None, True, True)
    tcpipDhcpServerComponent.setDisplayType("Application Layer")
    
    tcpipDhcpcv6Component = Module.CreateComponent("tcpipDhcpcv6", "DHCPV6 CLIENT", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_dhcpv6.py")
    tcpipDhcpcv6Component.addCapability("libtcpipDhcpcv6","DHCPCV6",True)
    tcpipDhcpcv6Component.addDependency("Dhcpcv6_IPv6_Dependency", "IPv6", None, True, True)
    tcpipDhcpcv6Component.addDependency("Dhcpcv6_UDP_Dependency", "UDP", None, True, True)
    tcpipDhcpcv6Component.setDisplayType("Application Layer")
    
    tcpipDnsComponent = Module.CreateComponent("tcpipDns", "DNS CLIENT", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_dns.py")
    tcpipDnsComponent.addCapability("libtcpipDns","DNSC",True)  
    tcpipDnsComponent.addDependency("Dns_UDP_Dependency", "UDP", None, True, True)
    tcpipDnsComponent.setDisplayType("Application Layer")
    
    tcpipDnssComponent = Module.CreateComponent("tcpipDnss", "DNS SERVER", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_dnss.py")
    tcpipDnssComponent.addCapability("libtcpipDnss","DNSS",True)    
    tcpipDnssComponent.addDependency("Dnss_UDP_Dependency", "UDP", None, True, True)    
    tcpipDnssComponent.setDisplayType("Application Layer")
    
    tcpipFtpsComponent = Module.CreateComponent("tcpipFtps", "FTP SERVER", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_ftps.py")
    tcpipFtpsComponent.addCapability("libtcpipFtps","FTPS",True)    
    tcpipFtpsComponent.addDependency("Ftps_TCP_Dependency", "TCP", None, True, True)    
    tcpipFtpsComponent.addDependency("Ftps_TcpipFs_Dependency", "SYS_FS", None, True, True)
    tcpipFtpsComponent.setDisplayType("Application Layer")
    
    tcpipHttpNetComponent = Module.CreateComponent("tcpipHttpNet", "HTTPNET SERVER", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_httpnet.py")
    tcpipHttpNetComponent.addCapability("libtcpipHttpNet","HTTPNET",True)   
    tcpipHttpNetComponent.addDependency("HttpNet_TCP_Dependency", "TCP", None, True, True)  
    tcpipHttpNetComponent.addDependency("HttpNet_TcpipFs_Dependency", "SYS_FS", None, True, True)
    tcpipHttpNetComponent.addDependency("HttpNet_NetPres_Dependency", "net_pres", True, True)
    tcpipHttpNetComponent.setDisplayType("Application Layer")

    tcpipHttpSrvV2Component = Module.CreateComponent("tcpipHttpSrvV2", "HTTP SERVER v2", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_http_server.py")
    tcpipHttpSrvV2Component.addCapability("libtcpipHttpV2","HTTP Server v2",True)   
    tcpipHttpSrvV2Component.addDependency("HttpV2_TCP_Dependency", "TCP", None, True, True)  
    tcpipHttpSrvV2Component.addDependency("HttpV2_TcpipFs_Dependency", "SYS_FS", None, True, True)
    tcpipHttpSrvV2Component.addDependency("HttpV2_NetPres_Dependency", "net_pres", True, True)
    tcpipHttpSrvV2Component.setDisplayType("Application Layer")

    if "SAMA5" not in processor:    
        tcpipHttpComponent = Module.CreateComponent("tcpipHttp", "HTTP SERVER", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_http.py")
        tcpipHttpComponent.addCapability("libtcpipHttp","HTTP",True)
        tcpipHttpComponent.addDependency("Http_TCP_Dependency", "TCP", None, True, True)
        tcpipHttpComponent.addDependency("Http_TcpipFs_Dependency", "SYS_FS", None, True, True)
        tcpipHttpComponent.addDependency("Http_Crypto_Dependency", "LIB_CRYPTO", None, True, True)
        tcpipHttpComponent.setDisplayType("Application Layer")

    tcpipIperfComponent = Module.CreateComponent("tcpipIperf", "IPERF", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_iperf.py")
    tcpipIperfComponent.addCapability("libtcpipIperf","IPERF",True)
    tcpipIperfComponent.addDependency("Iperf_TCP_Dependency", "TCP", None, True, True)
    tcpipIperfComponent.addDependency("Iperf_UDP_Dependency", "UDP", None, True, True)
    tcpipIperfComponent.setDisplayType("Application Layer")

    tcpipFtpcComponent = Module.CreateComponent("tcpipFtpc", "FTP CLIENT", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_ftpc.py")
    tcpipFtpcComponent.addCapability("libtcpipFtpc","FTPC",True)
    tcpipFtpcComponent.addDependency("FTPC_TCP_Dependency", "TCP", None, True, True)    
    tcpipFtpcComponent.addDependency("FTPC_TcipFs_Dependency", "SYS_FS", None, True, True)
    tcpipFtpcComponent.setDisplayType("Application Layer")
    
    tcpipNbnsComponent = Module.CreateComponent("tcpipNbns", "NBNS", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_nbns.py")
    tcpipNbnsComponent.addCapability("libtcpipNbns","NBNS",True)
    tcpipNbnsComponent.addDependency("Nbns_IPv4_Dependency", "IPv4", None, True, True)
    tcpipNbnsComponent.addDependency("Nbns_UDP_Dependency", "UDP", None, True, True)    
    tcpipNbnsComponent.setDisplayType("Application Layer")
    
    # tcpipRebootComponent = Module.CreateComponent("tcpipReboot", "REBOOT", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_reboot.py")
    # tcpipRebootComponent.addCapability("libtcpipReboot","REBOOT",True)
    # tcpipRebootComponent.addDependency("Reboot_IPv4_Dependency", "IPv4", None, True, True)
    # tcpipRebootComponent.addDependency("Reboot_UDP_Dependency", "UDP", None, True, True)
    # tcpipRebootComponent.setDisplayType("Library")

    tcpipSmtpcComponent = Module.CreateComponent("tcpipSmtpc", "SMTP CLIENT", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_smtpc.py")
    tcpipSmtpcComponent.addCapability("libtcpipSmtpc","SMTPC",True)
    tcpipSmtpcComponent.addDependency("Smtpc_TCP_Dependency", "TCP", None, True, True)  
    tcpipSmtpcComponent.addDependency("Smtpc_NetPres_Dependency", "net_pres", True, True)
    tcpipSmtpcComponent.addDependency("Smtpc_DNSC_Dependency", "DNSC", None, True, True)
    tcpipSmtpcComponent.addDependency("Smtpc_TcpipFs_Dependency", "SYS_FS", None, True, True)
    tcpipSmtpcComponent.setDisplayType("Application Layer")
    
    tcpipSnmpComponent = Module.CreateComponent("tcpipSnmp", "SNMP", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_snmp.py")
    tcpipSnmpComponent.addCapability("libtcpipSnmp","SNMP",True)    
    tcpipSnmpComponent.addDependency("Snmp_UDP_Dependency", "UDP", None, True, True)    
    tcpipSnmpComponent.addDependency("Snmp_TcpipFs_Dependency", "SYS_FS", None, True, True)
    tcpipSnmpComponent.setDisplayType("Application Layer")

    if "SAMA5" not in processor:    
        tcpipSnmpv3Component = Module.CreateComponent("tcpipSnmpv3", "SNMPV3", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_snmpv3.py")
        tcpipSnmpv3Component.addCapability("libtcpipSnmpv3","SNMPV3",True)  
        tcpipSnmpv3Component.addDependency("Snmpv3_SNMP_Dependency", "SNMP", None, True, True)
        tcpipSnmpv3Component.setDisplayType("Application Layer")

    tcpipSntpComponent = Module.CreateComponent("tcpipSntp", "SNTP", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_sntp.py")
    tcpipSntpComponent.addCapability("libtcpipSntp","SNTP",True)    
    tcpipSntpComponent.addDependency("Sntp_DNSC_Dependency", "DNSC", None, True, True)
    tcpipSntpComponent.setDisplayType("Application Layer")
    
    tcpipTelnetComponent = Module.CreateComponent("tcpipTelnet", "TELNET", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_telnet.py")
    tcpipTelnetComponent.addCapability("libtcpipTelnet","TELNET",True)
    tcpipTelnetComponent.addDependency("Telnet_TcpipCmd_Dependency", "TCPIP_CMD", None, True, True)
    tcpipTelnetComponent.addDependency("Telnet_NetPres_Dependency", "net_pres", True, True)
    tcpipTelnetComponent.setDisplayType("Application Layer")
    
    tcpipTftpcComponent = Module.CreateComponent("tcpipTftpc", "TFTP CLIENT", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_tftpc.py")
    tcpipTftpcComponent.addCapability("libtcpipTftpc","TFTPC",True)
    tcpipTftpcComponent.addDependency("Tftpc_IPv4_Dependency", "IPv4", None, True, True)
    tcpipTftpcComponent.addDependency("Tftpc_UDP_Dependency", "UDP", None, True, True)  
    #tcpipTftpcComponent.addDependency("Tftpc_MAC_Dependency", "MAC")
    tcpipTftpcComponent.addDependency("Tftpc_TcpipFs_Dependency", "SYS_FS", None, True, True)
    tcpipTftpcComponent.setDisplayType("Application Layer")
    
    tcpipTftpsComponent = Module.CreateComponent("tcpipTftps", "TFTP SERVER", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_tftps.py")
    tcpipTftpsComponent.addCapability("libtcpipTftps","TFTPS",True)
    tcpipTftpsComponent.addDependency("Tftps_IPv4_Dependency", "IPv4", None, True, True)
    tcpipTftpsComponent.addDependency("Tftps_UDP_Dependency", "UDP", None, True, True)  
    #tcpipTftpsComponent.addDependency("Tftps_MAC_Dependency", "MAC")
    tcpipTftpsComponent.addDependency("Tftps_TcpipFs_Dependency", "SYS_FS", None, True, True)
    tcpipTftpsComponent.setDisplayType("Application Layer")
    
    tcpipZeroConfComponent = Module.CreateComponent("tcpipZeroConf", "ZEROCONF", "/TCPIP/Layer7-APPLICATION/", "tcpip/config/tcpip_zeroconf.py")
    tcpipZeroConfComponent.addCapability("libtcpipZeroConf","ZEROCONF",True)
    tcpipZeroConfComponent.addDependency("ZeroConf_IPv4_Dependency", "IPv4", None, True, True)
    tcpipZeroConfComponent.addDependency("ZeroConf_UDP_Dependency", "UDP", None, True, True)
    tcpipZeroConfComponent.setDisplayType("Application Layer")

    ###########  TCP/IP LIBRARY Datalink & Physical Layer Configurations  ###########
    if ("SAMA7G" in Variables.get("__PROCESSOR")) and (Peripheral.moduleExists("GMAC")):
        drvGmacComponent_0 = Module.CreateComponent("drvGmac0", "GMAC0", "/Drivers/MAC Driver/Internal/", "driver/gmac/config/drv_intmac_gmac.py")
        drvGmacComponent_0.addMultiCapability("libdrvGmac0","MAC", None)
        drvGmacComponent_0.addDependency("GMAC0_PHY_Dependency", "PHY", None, False, True)  
        drvGmacComponent_0.setDisplayType("MAC Layer")
        drvGmacComponent_1 = Module.CreateComponent("drvGmac1", "GMAC1", "/Drivers/MAC Driver/Internal/", "driver/gmac/config/drv_intmac_gmac.py")
        drvGmacComponent_1.addMultiCapability("libdrvGmac1","MAC", None)
        drvGmacComponent_1.addDependency("GMAC1_PHY_Dependency", "PHY", None, False, True)  
        drvGmacComponent_1.setDisplayType("MAC Layer")
    elif Peripheral.moduleExists("GMAC"):
        drvGmacComponent = Module.CreateComponent("drvGmac", "GMAC", "/Drivers/MAC Driver/Internal/", "driver/gmac/config/drv_intmac_gmac.py")
        drvGmacComponent.addMultiCapability("libdrvGmac","MAC", None)
        drvGmacComponent.addDependency("GMAC_PHY_Dependency", "PHY", None, False, True)  
        drvGmacComponent.setDisplayType("MAC Layer")
    elif (("PIC32CZ" in Variables.get("__PROCESSOR")) or ("PIC32CK" in Variables.get("__PROCESSOR"))) and (Peripheral.moduleExists("ETH")):
        drvGmacComponent = Module.CreateComponent("drvGmac", "ETH", "/Harmony/Drivers/MAC Driver/Internal/", "driver/gmac/config/drv_intmac_gmac.py")
        drvGmacComponent.addCapability("libdrvGmac","MAC")
        drvGmacComponent.addDependency("ETH_PHY_Dependency", "PHY", None, False, True)  
        drvGmacComponent.setDisplayType("MAC Layer")    
    elif Peripheral.moduleExists("EMAC"):
        aDrvMacComponent_0 = Module.CreateComponent("drvEmac0", "EMAC0", "/Drivers/MAC Driver/Internal/", "driver/emac/config/drv_intmac_emac.py")
        aDrvMacComponent_0.addMultiCapability( "libdrvMac0", "MAC", None)
        aDrvMacComponent_0.addDependency("MAC_PHY_Dependency0", "PHY", None, False, True)
        aDrvMacComponent_0.setDisplayType("MAC Layer")
        aDrvMacComponent_1 = Module.CreateComponent("drvEmac1", "EMAC1", "/Drivers/MAC Driver/Internal/", "driver/emac/config/drv_intmac_emac.py")
        aDrvMacComponent_1.addMultiCapability( "libdrvMac1", "MAC", None)
        aDrvMacComponent_1.addDependency("MAC_PHY_Dependency1", "PHY", None, False, True)
        aDrvMacComponent_1.setDisplayType("MAC Layer")
    elif ("PIC32M" in Variables.get("__PROCESSOR")) and (deviceSeries != "PIC32MZW"):
        drvPic32mEthmacComponent = Module.CreateComponent("drvPic32mEthmac", "ETHMAC", "/Drivers/MAC Driver/Internal/", "driver/ethmac/config/drv_intmac_ethmac.py")
        drvPic32mEthmacComponent.addMultiCapability("libdrvPic32mEthmac","MAC", None)
        drvPic32mEthmacComponent.addDependency("ETHMAC_PHY_Dependency", "PHY", None, False, True)
        drvPic32mEthmacComponent.setDisplayType("MAC Layer")
            
    ## MIIM Driver
    drvMiimComponent = Module.CreateComponent("drvMiim", "MIIM Driver", "/Drivers/", "driver/miim/config/drv_miim.py")
    drvMiimComponent.addCapability("libdrvMiim","MIIM",True)   
    drvMiimComponent.setDisplayType("PHY Layer")
    
    ################# ETHERNET PHY Driver ##############################################
    #Driver for Microchip VSC8540 PHY
    drvExtPhyVsc8540Component = Module.CreateComponent("drvExtPhyVsc8540", "VSC8540", "/Drivers/PHY Driver", "driver/ethphy/config/drv_extphy_vsc8540.py")
    drvExtPhyVsc8540Component.addCapability("libdrvExtPhyVsc8540","PHY",False)   
    drvExtPhyVsc8540Component.addDependency("VSC8540_MIIM_Dependency", "MIIM", None, True, True)    
    drvExtPhyVsc8540Component.setDisplayType("PHY Layer")

    #Driver for Micrel KSZ8041 PHY
    drvExtPhyKsz8041Component = Module.CreateComponent("drvExtPhyKsz8041", "KSZ8041", "/Drivers/PHY Driver", "driver/ethphy/config/drv_extphy_ksz8041.py")
    drvExtPhyKsz8041Component.addCapability("libdrvExtPhyKsz8041","PHY",False)   
    drvExtPhyKsz8041Component.addDependency("KSZ8041_MIIM_Dependency", "MIIM", None, True, True)  
    drvExtPhyKsz8041Component.setDisplayType("PHY Layer")
    
    #Driver for Micrel KSZ8061 PHY
    drvExtPhyKsz8061Component = Module.CreateComponent("drvExtPhyKsz8061", "KSZ8061", "/Drivers/PHY Driver", "driver/ethphy/config/drv_extphy_ksz8061.py")
    drvExtPhyKsz8061Component.addCapability("libdrvExtPhyKsz8061","PHY",False)   
    drvExtPhyKsz8061Component.addDependency("KSZ8061_MIIM_Dependency", "MIIM", None, True, True)    
    drvExtPhyKsz8061Component.setDisplayType("PHY Layer")
    
    #Driver for Micrel KSZ8081 PHY
    drvExtPhyKsz8081Component = Module.CreateComponent("drvExtPhyKsz8081", "KSZ8081", "/Drivers/PHY Driver", "driver/ethphy/config/drv_extphy_ksz8081.py")
    drvExtPhyKsz8081Component.addCapability("libdrvExtPhyKsz8081","PHY",False)   
    drvExtPhyKsz8081Component.addDependency("KSZ8081_MIIM_Dependency", "MIIM", None, True, True)    
    drvExtPhyKsz8081Component.setDisplayType("PHY Layer")
    
    #Driver for Micrel KSZ8091 PHY
    drvExtPhyKsz8091Component = Module.CreateComponent("drvExtPhyKsz8091", "KSZ8091", "/Drivers/PHY Driver", "driver/ethphy/config/drv_extphy_ksz8091.py")
    drvExtPhyKsz8091Component.addCapability("libdrvExtPhyKsz8091","PHY",False)   
    drvExtPhyKsz8091Component.addDependency("KSZ8091_MIIM_Dependency", "MIIM", None, True, True)    
    drvExtPhyKsz8091Component.setDisplayType("PHY Layer")
    
    #Driver for Micrel KSZ8863 PHY
    drvExtPhyKsz8863Component = Module.CreateComponent("drvExtPhyKsz8863", "KSZ8863", "/Drivers/PHY Driver", "driver/ethphy/config/drv_extphy_ksz8863.py")
    drvExtPhyKsz8863Component.addCapability("libdrvExtPhyKsz8863","PHY",False)   
    drvExtPhyKsz8863Component.addDependency("KSZ8863_MIIM_Dependency", "MIIM", None, True, True)  
    drvExtPhyKsz8863Component.setDisplayType("PHY Layer")
    
    #Driver for Micrel KSZ9031 PHY
    drvExtPhyKsz9031Component = Module.CreateComponent("drvExtPhyKsz9031", "KSZ9031", "/Drivers/PHY Driver", "driver/ethphy/config/drv_extphy_ksz9031.py")
    drvExtPhyKsz9031Component.addCapability("libdrvExtPhyKsz9031","PHY",False)   
    drvExtPhyKsz9031Component.addDependency("KSZ9031_MIIM_Dependency", "MIIM", None, True, True)    
    drvExtPhyKsz9031Component.setDisplayType("PHY Layer")
    
    #Driver for Micrel KSZ9131 PHY
    drvExtPhyKsz9131Component = Module.CreateComponent("drvExtPhyKsz9131", "KSZ9131", "/Drivers/PHY Driver", "driver/ethphy/config/drv_extphy_ksz9131.py")
    drvExtPhyKsz9131Component.addCapability("libdrvExtPhyKsz9131","PHY",False)   
    drvExtPhyKsz9131Component.addDependency("KSZ9131_MIIM_Dependency", "MIIM", None, True, True)    
    drvExtPhyKsz9131Component.setDisplayType("PHY Layer")
    
    #Driver for LAN8700 PHY
    drvExtPhyLan8700Component = Module.CreateComponent("drvExtPhyLan8700", "LAN8700", "/Drivers/PHY Driver", "driver/ethphy/config/drv_extphy_lan8700.py")
    drvExtPhyLan8700Component.addCapability("libdrvExtPhyLan8700","PHY",False)   
    drvExtPhyLan8700Component.addDependency("LAN8700_MIIM_Dependency", "MIIM", None, True, True)       
    drvExtPhyLan8700Component.setDisplayType("PHY Layer")
    
    #Driver for LAN8720PHY
    drvExtPhyLan8720Component = Module.CreateComponent("drvExtPhyLan8720", "LAN8720", "/Drivers/PHY Driver", "driver/ethphy/config/drv_extphy_lan8720.py")
    drvExtPhyLan8720Component.addCapability("libdrvExtPhyLan8720","PHY",False)   
    drvExtPhyLan8720Component.addDependency("LAN8720_MIIM_Dependency", "MIIM", None, True, True) 
    drvExtPhyLan8720Component.setDisplayType("PHY Layer")
    
    #Driver for LAN8740 PHY
    drvExtPhyLan8740Component = Module.CreateComponent("drvExtPhyLan8740", "LAN8740", "/Drivers/PHY Driver", "driver/ethphy/config/drv_extphy_lan8740.py")
    drvExtPhyLan8740Component.addCapability("libdrvExtPhyLan8740","PHY",False)   
    drvExtPhyLan8740Component.addDependency("LAN8740_MIIM_Dependency", "MIIM", None, True, True)        
    drvExtPhyLan8740Component.setDisplayType("PHY Layer")
    
    #Driver for LAN8742A PHY
    drvExtPhyLan8742aComponent = Module.CreateComponent("drvExtPhyLan8742a", "LAN8742A", "/Drivers/PHY Driver", "driver/ethphy/config/drv_extphy_lan8742a.py")
    drvExtPhyLan8742aComponent.addCapability("libdrvExtPhyLan8742a","PHY",False)   
    drvExtPhyLan8742aComponent.addDependency("LAN8742a_MIIM_Dependency", "MIIM", None, True, True)        
    drvExtPhyLan8742aComponent.setDisplayType("PHY Layer")
    
    #Driver for LAN9303PHY
    drvExtPhyLan9303Component = Module.CreateComponent("drvExtPhyLan9303", "LAN9303", "/Drivers/PHY Driver", "driver/ethphy/config/drv_extphy_lan9303.py")
    drvExtPhyLan9303Component.addCapability("libdrvExtPhyLan9303","PHY",False)   
    drvExtPhyLan9303Component.addDependency("LAN9303_MIIM_Dependency", "MIIM", None, True, True) 
    drvExtPhyLan9303Component.setDisplayType("PHY Layer")
    
    #Driver for DP83640PHY
    drvExtPhyDp83640Component = Module.CreateComponent("drvExtPhyDp83640", "DP83640", "/Drivers/PHY Driver", "driver/ethphy/config/drv_extphy_dp83640.py")
    drvExtPhyDp83640Component.addCapability("libdrvExtPhyDp83640","PHY",False)   
    drvExtPhyDp83640Component.addDependency("DP83640_MIIM_Dependency", "MIIM", None, True, True) 
    drvExtPhyDp83640Component.setDisplayType("PHY Layer")
    
    #Driver for DP83848PHY
    drvExtPhyDp83848Component = Module.CreateComponent("drvExtPhyDp83848", "DP83848", "/Drivers/PHY Driver", "driver/ethphy/config/drv_extphy_dp83848.py")
    drvExtPhyDp83848Component.addCapability("libdrvExtPhyDp83848","PHY",False)   
    drvExtPhyDp83848Component.addDependency("DP83848_MIIM_Dependency", "MIIM", None, True, True) 
    drvExtPhyDp83848Component.setDisplayType("PHY Layer")
    
    #Driver for IP101GRPHY
    drvExtPhyIp101grComponent = Module.CreateComponent("drvExtPhyIp101gr", "IP101GR", "/Drivers/PHY Driver", "driver/ethphy/config/drv_extphy_ip101gr.py")
    drvExtPhyIp101grComponent.addCapability("libdrvExtPhyIp101gr","PHY",False)   
    drvExtPhyIp101grComponent.addDependency("IP101GR_MIIM_Dependency", "MIIM", None, True, True)     
    drvExtPhyIp101grComponent.setDisplayType("PHY Layer")

    #Driver for LAN9354 PHY
    drvExtPhyLan9354Component = Module.CreateComponent("drvExtPhyLan9354", "LAN9354", "/Drivers/PHY Driver", "driver/ethphy/config/drv_extphy_lan9354.py")
    drvExtPhyLan9354Component.addCapability("libdrvExtPhyLan9354","PHY",False)   
    drvExtPhyLan9354Component.addDependency("LAN9354_MIIM_Dependency", "MIIM", None, True, True)        
    drvExtPhyLan9354Component.setDisplayType("PHY Layer")
    
    #Driver for LAN8770 PHY
    drvExtPhyLan8770Component = Module.CreateComponent("drvExtPhyLan8770", "LAN8770", "/Drivers/PHY Driver", "driver/ethphy/config/drv_extphy_lan8770.py")
    drvExtPhyLan8770Component.addCapability("libdrvExtPhyLan8770","PHY",False)   
    drvExtPhyLan8770Component.addDependency("LAN8770_MIIM_Dependency", "MIIM", None, True, True)        
    drvExtPhyLan8770Component.setDisplayType("PHY Layer")
    
    #Driver for ENCX24J600
    drvExtMacEncx24j600Component = Module.CreateGeneratorComponent("drvExtMacEncx24j600", "ENCX24J600", "/Drivers/External Ethernet Controller", "driver/encx24j600/config/drv_encx24j600_common.py", "driver/encx24j600/config/drv_encx24j600.py")
    drvExtMacEncx24j600Component.addMultiCapability("libdrvExtMacEncx24j600","MAC",None)   
    drvExtMacEncx24j600Component.addDependency("ENCX24J600_SPI", "DRV_SPI", None, False, True)   
    drvExtMacEncx24j600Component.setDisplayType("MAC Layer")
    
    #Driver for ENC28J60
    drvExtMacEnc28j60Component = Module.CreateGeneratorComponent("drvExtMacEnc28j60", "ENC28J60", "/Drivers/External Ethernet Controller", "driver/enc28j60/config/drv_enc28j60_common.py", "driver/enc28j60/config/drv_enc28j60.py")
    drvExtMacEnc28j60Component.addMultiCapability("libdrvExtMacEnc28j60","MAC",None)  
    drvExtMacEnc28j60Component.addDependency("ENC28J60_SPI", "DRV_SPI", None, False, True)   
    drvExtMacEnc28j60Component.setDisplayType("MAC Layer")
    ########################## Harmony Network Presentation Module #################################    
    # OBSOLETE Module - Do not use this module for new demos#
    netPresComponent = Module.CreateGeneratorComponent("netPres", "Presentation Layer(Obsolete)", "/TCPIP/Layer6-PRESENTATION","net_pres/pres/config/netPres_common.py","net_pres/pres/config/netPres.py")
    netPresComponent.addCapability("libNetPres","net_pres",True)    
    netPresComponent.addDependency("NetPres_Crypto_Dependency", "TLS Provider", None, False, False)
    # netPresComponent.setDisplayType("Presentation Layer")
    
    ############################### TCP/IP STACK CONFIGURATOR #####################################
    #tcpipAutoConfigComponent = Module.CreateComponent("tcpip_template", "TCP/IP Stack Configurator", "/TCPIP/", "tcpip/config/tcpip_templates.py")

    tcpipAutoConfigAppsComponent = Module.CreateComponent("tcpip_apps_config", "TCP/IP Application Layer Configurator", "/TCPIP/Net Configurators(Obsolete)/", "tcpip/config/tcpip_configurator_apps.py")
    tcpipAutoConfigAppsComponent.setDisplayType("Application Configurator")
    
    tcpipAutoConfigTransportComponent = Module.CreateComponent("tcpip_transport_config", "TCP/IP Transport Layer Configurator", "/TCPIP/Net Configurators(Obsolete)/", "tcpip/config/tcpip_configurator_transport.py")
    tcpipAutoConfigTransportComponent.setDisplayType("Transport Configurator")
    
    tcpipAutoConfigNetworkComponent = Module.CreateComponent("tcpip_network_config", "TCP/IP Network Layer Configurator", "/TCPIP/Net Configurators(Obsolete)/", "tcpip/config/tcpip_configurator_network.py")
    tcpipAutoConfigNetworkComponent.setDisplayType("Network Configurator")
    
    tcpipAutoConfigDriverComponent = Module.CreateComponent("tcpip_driver_config", "TCP/IP Driver Layer Configurator", "/TCPIP/Net Configurators(Obsolete)/", "tcpip/config/tcpip_configurator_driver.py")
    tcpipAutoConfigDriverComponent.setDisplayType("MAC/PHY Configurator")
    
    tcpipAutoConfigBasicComponent = Module.CreateComponent("tcpip_basic_config", "TCP/IP Basic Configurator", "/TCPIP/Net Configurators(Obsolete)/", "tcpip/config/tcpip_configurator_basic.py")
    tcpipAutoConfigBasicComponent.setDisplayType("Basic Configurator")
    ########################## Thirdy-Party Modules #################################   
    # Wolfmqtt 
    wolfmqttComponent = Module.CreateComponent("lib_wolfmqtt", "wolfMQTT Library", "/Third Party Libraries/wolfSSL/", "third_party_adapter/wolfMQTT/config/wolfmqtt.py")
    wolfmqttComponent.addCapability("lib_wolfmqtt","wolfmqtt",True)   
    wolfmqttComponent.addDependency("WolfMqtt_NetPres_Dependency", "net_pres", None, True, False)
    wolfmqttComponent.setDisplayType("Thirdparty")
