"""
Copyright (C) 2018-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

################################################################################
#### Business Logic ####
################################################################################


############################################################################
#### Code Generation ####
############################################################################
processor = ""
def instantiateComponent(tcpipAutoConfigAppsComponent):
    global processor
    processor = Variables.get("__PROCESSOR")

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
    
    # Enable DHCPV6_CLIENT
    tcpipAutoConfigDHCPv6_CLIENT = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_DHCPV6_CLIENT", None)
    tcpipAutoConfigDHCPv6_CLIENT.setLabel("DHCPV6 CLIENT")
    tcpipAutoConfigDHCPv6_CLIENT.setVisible(True)
    tcpipAutoConfigDHCPv6_CLIENT.setDescription("Enable DHCPV6_CLIENT") 
    tcpipAutoConfigDHCPv6_CLIENT.setDependencies(tcpipAutoConfigDHCPV6CLIENTEnable, ["TCPIP_AUTOCONFIG_ENABLE_DHCPV6_CLIENT"])


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

    # Enable DHCP_SERVER_V2
    tcpipAutoConfigDHCP_SERVER_v2 = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_DHCP_SERVER_V2", None)
    tcpipAutoConfigDHCP_SERVER_v2.setLabel("DHCP SERVER V2")
    tcpipAutoConfigDHCP_SERVER_v2.setVisible(True)
    tcpipAutoConfigDHCP_SERVER_v2.setDescription("Enable DHCP_SERVER V2")
    tcpipAutoConfigDHCP_SERVER_v2.setDependencies(tcpipAutoConfigDHCPSERVERv2Enable, ["TCPIP_AUTOCONFIG_ENABLE_DHCP_SERVER_V2"])
    
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

    # Enable FTP_CLIENT
    tcpipAutoConfigFTP_CLIENT = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_FTP_CLIENT", None)
    tcpipAutoConfigFTP_CLIENT.setLabel("FTP CLIENT")
    tcpipAutoConfigFTP_CLIENT.setVisible(True)
    tcpipAutoConfigFTP_CLIENT.setDescription("Enable CLIENT")
    tcpipAutoConfigFTP_CLIENT.setDependencies(tcpipAutoConfigFTPCLIENTEnable, ["TCPIP_AUTOCONFIG_ENABLE_FTP_CLIENT"])
    
    # Enable HTTP_NET_SERVER
    tcpipAutoConfigHTTP_NET_SERVER = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_HTTP_NET_SERVER", None)
    tcpipAutoConfigHTTP_NET_SERVER.setLabel("HTTPNET SERVER")
    tcpipAutoConfigHTTP_NET_SERVER.setVisible(True)
    tcpipAutoConfigHTTP_NET_SERVER.setDescription("Enable HTTP_NET_SERVER")
    tcpipAutoConfigHTTP_NET_SERVER.setDependencies(tcpipAutoConfigHTTPNETSERVEREnable, ["TCPIP_AUTOCONFIG_ENABLE_HTTP_NET_SERVER"])

    if(     ("SAMA5" not in processor)
        and ("SAM9X6" not in processor)
    ):
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

    # # Enable REBOOT
    # tcpipAutoConfigREBOOT = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_REBOOT", None)
    # tcpipAutoConfigREBOOT.setLabel("REBOOT")
    # tcpipAutoConfigREBOOT.setVisible(True)
    # tcpipAutoConfigREBOOT.setDescription("Enable REBOOT")
    # tcpipAutoConfigREBOOT.setDependencies(tcpipAutoConfigREBOOTEnable, ["TCPIP_AUTOCONFIG_ENABLE_REBOOT"])

    # Enable SMTP_CLIENT
    tcpipAutoConfigSMTP_CLIENT = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_SMTP_CLIENT", None)
    tcpipAutoConfigSMTP_CLIENT.setLabel("SMTP CLIENT")
    tcpipAutoConfigSMTP_CLIENT.setVisible(True)
    tcpipAutoConfigSMTP_CLIENT.setDescription("Enable SMTP_CLIENT")
    tcpipAutoConfigSMTP_CLIENT.setDependencies(tcpipAutoConfigSMTPCLIENTEnable, ["TCPIP_AUTOCONFIG_ENABLE_SMTP_CLIENT"])

    if(     ("SAMA5" not in processor)
        and ("SAM9X6" not in processor)
    ):
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

    # Enable TFTP_SERVER
    tcpipAutoConfigTFTP_SERVER = tcpipAutoConfigAppsComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_TFTP_SERVER", None)
    tcpipAutoConfigTFTP_SERVER.setLabel("TFTP SERVER")
    tcpipAutoConfigTFTP_SERVER.setVisible(True)
    tcpipAutoConfigTFTP_SERVER.setDescription("Enable TFTP_SERVER")
    tcpipAutoConfigTFTP_SERVER.setDependencies(tcpipAutoConfigTFTPSERVEREnable, ["TCPIP_AUTOCONFIG_ENABLE_TFTP_SERVER"])
    
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
            res = tcpipAutoConfigTransportGroup.addComponent("tcpip_transport_config")
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
            setVal("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_STACK", True)
            
        if(Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_NETCONFIG") != True): 
            setVal("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_NETCONFIG", True)            

#################Business Logic- Application Layer #########################################        
def tcpipAutoConfigANNOUNCEEnable(symbol, event):   
    tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
    enableTcpipAutoConfigApps(True)
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipAnnounce"],"APPLICATION LAYER", False)     
        if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
            setVal("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True)   
        if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
            setVal("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True)  
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
        # Activate new net_Pres component if obsolete netPres Component is not added in project
        if(Database.getComponentByID("netPres") == None):
            if(Database.getComponentByID("net_Pres") == None):
                res = Database.activateComponents(["net_Pres"])  
                if(res == True):
                    Database.setActiveGroup("APPLICATION LAYER")
                    Database.selectComponent("tcpip_apps_config")
        
        if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP") != True):
            setVal("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP", True)
        if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
            setVal("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True)
        if(Database.getSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DNS_CLIENT") != True):
            Database.setSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DNS_CLIENT", True, 2)
    else:
        res = Database.deactivateComponents(["tcpipBerkeleyApi"])
    
def tcpipAutoConfigDDNSEnable(symbol, event):
    tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
    enableTcpipAutoConfigApps(True)
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipDdns"],"APPLICATION LAYER", False)     
        tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipDdns", "libtcpipDdns")
        if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
            setVal("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True)
        if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
            setVal("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True)
    else:
        res = Database.deactivateComponents(["tcpipDdns"])
    
def tcpipAutoConfigDHCPV6CLIENTEnable(symbol, event):
    tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
    enableTcpipAutoConfigApps(True)
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipDhcpcv6"],"APPLICATION LAYER", False) 
        tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipDhcpcv6", "libtcpipDhcpcv6")
        if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV6") != True):
            setVal("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV6", True)
        if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):    setVal("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True)
    else:
        res = Database.deactivateComponents(["tcpipDhcpcv6"])
    
def tcpipAutoConfigDHCPCLIENTEnable(symbol, event):
    tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
    enableTcpipAutoConfigApps(True)
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipDhcp"],"APPLICATION LAYER", False) 
        tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipDhcp", "libtcpipDhcp")
        if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
            setVal("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True)
        if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
            setVal("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True)
    else:
        res = Database.deactivateComponents(["tcpipDhcp"])
    
def tcpipAutoConfigDHCPSERVEREnable(symbol, event):
    tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
    dhcpServerV2_state  = event['source'].getSymbolByID("TCPIP_AUTOCONFIG_ENABLE_DHCP_SERVER_V2")
    enableTcpipAutoConfigApps(True)
    if (event["value"] == True):
        dhcpServerV2_state.setReadOnly(True)
        res = Database.activateComponents(["tcpipDhcps"],"APPLICATION LAYER", False)    
        tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipDhcps", "libtcpipDhcps")
        if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
            setVal("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True)
        if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_ICMPv4") != True):
            setVal("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_ICMPv4", True)
        if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
            setVal("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True)
    else:
        if(dhcpServerV2_state.getReadOnly() == True):
            dhcpServerV2_state.setReadOnly(False)
        if(Database.getComponentByID("tcpipDhcps") != None):
            res = Database.deactivateComponents(["tcpipDhcps"])


def tcpipAutoConfigDHCPSERVERv2Enable(symbol, event):
    tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
    dhcpServer_state  = event['source'].getSymbolByID("TCPIP_AUTOCONFIG_ENABLE_DHCP_SERVER")
    enableTcpipAutoConfigApps(True)
    if (event["value"] == True):
        dhcpServer_state.setReadOnly(True)
        res = Database.activateComponents(["tcpipDhcpServer"],"APPLICATION LAYER", False)    
        tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipDhcpServer", "libtcpipDhcps")
        if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
            setVal("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True)
        if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_ICMPv4") != True):
            setVal("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_ICMPv4", True)
        if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
            setVal("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True)
    else:
        if(dhcpServer_state.getReadOnly() == True):
            dhcpServer_state.setReadOnly(False)
        if(Database.getComponentByID("tcpipDhcpServer") != None):
            res = Database.deactivateComponents(["tcpipDhcpServer"])
    
def tcpipAutoConfigDNSCLIENTEnable(symbol, event):
    tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
    enableTcpipAutoConfigApps(True)
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipDns"],"APPLICATION LAYER", False)  
        tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipDns", "libtcpipDns")
        if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
            setVal("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True)
    else:
        res = Database.deactivateComponents(["tcpipDns"])
    
def tcpipAutoConfigDNSSERVEREnable(symbol, event):
    tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
    enableTcpipAutoConfigApps(True)
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipDnss"],"APPLICATION LAYER", False)
        tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipDnss", "libtcpipDnss")
        if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
            setVal("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True)
    else:
        res = Database.deactivateComponents(["tcpipDnss"])
    

def tcpipAutoConfigFTPSERVEREnable(symbol, event):
    tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
    tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
    enableTcpipAutoConfigApps(True)
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipFtps"],"APPLICATION LAYER", False) 
        
        if(Database.getComponentByID("sys_fs") == None):    
            res = Database.activateComponents(["sys_fs"])            
        
        if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
            setVal("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True)
        if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP") != True):
            setVal("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP", True)
    else:
        if(Database.getComponentByID("tcpipFtps") != None):
            res = Database.deactivateComponents(["tcpipFtps"])


def tcpipAutoConfigFTPCLIENTEnable(symbol, event):
    tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
    tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
    enableTcpipAutoConfigApps(True)
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipFtpc"],"APPLICATION LAYER", False) 
        tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipFtpc", "libtcpipFtpc")
        
        # Activate new net_Pres component if obsolete netPres Component is not added in project
        if(Database.getComponentByID("netPres") == None):    
            if(Database.getComponentByID("net_Pres") == None):
                res = Database.activateComponents(["net_Pres"])  
                if(res == True):
                    Database.setActiveGroup("APPLICATION LAYER")
                    Database.selectComponent("tcpip_apps_config")
        
        if(Database.getComponentByID("sys_fs") == None):    
            res = Database.activateComponents(["sys_fs"])
            
        if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP") != True):
            setVal("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP", True)
    else:
        if(Database.getComponentByID("tcpipFtpc") != None):
            res = Database.deactivateComponents(["tcpipFtpc"])
        
def tcpipAutoConfigHTTPNETSERVEREnable(symbol, event):
    global processor
    tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
    tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
    if(("SAMA5" not in processor) and ("SAM9X6" not in processor)):
        httpserver  = event['source'].getSymbolByID("TCPIP_AUTOCONFIG_ENABLE_HTTP_SERVER")
    enableTcpipAutoConfigApps(True)
    if (event["value"] == True):
        if(("SAMA5" not in processor) and ("SAM9X6" not in processor)):
            httpserver.setReadOnly(True)
        res = Database.activateComponents(["tcpipHttpNet"],"APPLICATION LAYER", False)  
        tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipHttpNet", "libtcpipHttpNet")
        
        # Activate new net_Pres component if obsolete netPres Component is not added in project
        if(Database.getComponentByID("netPres") == None):
            if(Database.getComponentByID("net_Pres") == None):
                res = Database.activateComponents(["net_Pres"])  
                if(res == True):
                    Database.setActiveGroup("APPLICATION LAYER")
                    Database.selectComponent("tcpip_apps_config") 
            
        if(Database.getComponentByID("sys_fs") == None):    
            res = Database.activateComponents(["sys_fs"])
                
        if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP") != True):
            setVal("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP", True)
    else:
        if(("SAMA5" not in processor) and ("SAM9X6" not in processor)):
            if(httpserver.getReadOnly() == True):
                httpserver.setReadOnly(False)
        if(Database.getComponentByID("tcpipHttpNet") != None):  
            res = Database.deactivateComponents(["tcpipHttpNet"])
    
def tcpipAutoConfigHTTPSERVEREnable(symbol, event):
    tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
    tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
    httpnetserver  = event['source'].getSymbolByID("TCPIP_AUTOCONFIG_ENABLE_HTTP_NET_SERVER")
    enableTcpipAutoConfigApps(True)
    if (event["value"] == True):
        httpnetserver.setReadOnly(True)
        res = Database.activateComponents(["tcpipHttp"],"APPLICATION LAYER", False) 
        tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipHttp", "libtcpipHttp")
        
        if(Database.getComponentByID("sys_fs") == None):    
            res = Database.activateComponents(["sys_fs"])
        
        res = Database.activateComponents(["lib_crypto"])
        if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP") != True):
            setVal("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP", True)
    else:
        if(httpnetserver.getReadOnly() == True):
            httpnetserver.setReadOnly(False)
        if(Database.getComponentByID("tcpipHttp") != None):  
            res = Database.deactivateComponents(["tcpipHttp"])
    
def tcpipAutoConfigIPERFEnable(symbol, event):
    tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
    enableTcpipAutoConfigApps(True)
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipIperf"],"APPLICATION LAYER", False)    
        tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipIperf", "libtcpipIperf")
        if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP") != True):
            setVal("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP", True)
        if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
            setVal("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True)
    else:
        res = Database.deactivateComponents(["tcpipIperf"])
    
def tcpipAutoConfigNBNSEnable(symbol, event):
    tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
    enableTcpipAutoConfigApps(True)
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipNbns"],"APPLICATION LAYER", False) 
        tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipNbns", "libtcpipNbns")
        if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
            setVal("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True)
        if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True): 
            setVal("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True)           
    else:
        res = Database.deactivateComponents(["tcpipNbns"])
    
# def tcpipAutoConfigREBOOTEnable(symbol, event):
    # tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
    # enableTcpipAutoConfigApps(True)
    # if (event["value"] == True):
        # res = Database.activateComponents(["tcpipReboot"],"APPLICATION LAYER", False)   
        # tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipReboot", "libtcpipReboot")
        # if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
            # setVal("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True)
        # if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
            # setVal("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True)
    # else:
        # res = Database.deactivateComponents(["tcpipReboot"])
    
def tcpipAutoConfigSMTPCLIENTEnable(symbol, event):
    tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
    tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
    
    enableTcpipAutoConfigApps(True)
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipSmtpc"],"APPLICATION LAYER", False)    
        tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipSmtpc", "libtcpipSmtpc")
        
        # Activate new net_Pres component if obsolete netPres Component is not added in project
        if(Database.getComponentByID("netPres") == None):
            if(Database.getComponentByID("net_Pres") == None):
                res = Database.activateComponents(["net_Pres"])  
                if(res == True):
                    Database.setActiveGroup("APPLICATION LAYER")
                    Database.selectComponent("tcpip_apps_config")
        
        if(Database.getComponentByID("sys_fs") == None):    
            res = Database.activateComponents(["sys_fs"])
        
        if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP") != True):
            setVal("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP", True)
        if(Database.getSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DNS_CLIENT") != True):
            Database.setSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DNS_CLIENT", True, 2)
            
    else:
        # this assumes that tcpipSmtpc is present in the graph, if the protocol is removed then this fails
        res = Database.deactivateComponents(["tcpipSmtpc"])
    
def tcpipAutoConfigSNMPEnable(symbol, event):
    tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
    tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
    enableTcpipAutoConfigApps(True)
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipSnmp"],"APPLICATION LAYER", False) 
        tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipSnmp", "libtcpipSnmp")
        
        if(Database.getComponentByID("sys_fs") == None):    
            res = Database.activateComponents(["sys_fs"])
        
        if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
            setVal("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True)
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
        if(Database.getSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DNS_CLIENT") != True):
            Database.setSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DNS_CLIENT", True, 2)
    else:
        res = Database.deactivateComponents(["tcpipSntp"])
    
def tcpipAutoConfigTELNETEnable(symbol, event):
    tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
    tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
    enableTcpipAutoConfigApps(True)
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipTelnet"],"APPLICATION LAYER", False)   
        tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipTelnet", "libtcpipTelnet")
        
        # Activate new net_Pres component if obsolete netPres Component is not added in project
        if(Database.getComponentByID("netPres") == None):
            if(Database.getComponentByID("net_Pres") == None):
                res = Database.activateComponents(["net_Pres"])  
                if(res == True):
                    Database.setActiveGroup("APPLICATION LAYER")
                    Database.selectComponent("tcpip_apps_config")
            
        if(Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_TCPIPCMD") != True):
            setVal("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_TCPIPCMD", True)
    else:
        res = Database.deactivateComponents(["tcpipTelnet"])
    
def tcpipAutoConfigTFTPCLIENTEnable(symbol, event):
    tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
    tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
    enableTcpipAutoConfigApps(True)
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipTftpc"],"APPLICATION LAYER", False)    
        tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipTftpc", "libtcpipTftpc")
        
        if(Database.getComponentByID("sys_fs") == None):    
            res = Database.activateComponents(["sys_fs"])
         
        if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
            setVal("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True)
        if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
            setVal("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True)
    else:
        res = Database.deactivateComponents(["tcpipTftpc"])
    
def tcpipAutoConfigTFTPSERVEREnable(symbol, event):
    tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
    tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
    enableTcpipAutoConfigApps(True)
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipTftps"],"APPLICATION LAYER", False)    
        tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipTftps", "libtcpipTftps")
        
        if(Database.getComponentByID("sys_fs") == None):    
            res = Database.activateComponents(["sys_fs"])
        
        if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
            setVal("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True)
        if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
            setVal("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True)
    else:
        res = Database.deactivateComponents(["tcpipTftps"])
        
def tcpipAutoConfigZEROCONFEnable(symbol, event):
    tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
    enableTcpipAutoConfigApps(True)
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipZeroConf"],"APPLICATION LAYER", False) 
        tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipZeroConf", "libtcpipZeroConf")
        if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4") != True):
            setVal("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_IPV4", True)
        if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP") != True):
            setVal("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_UDP", True)
    else:
        res = Database.deactivateComponents(["tcpipZeroConf"])


#Set symbols of other components
def setVal(component, symbol, value):
    triggerDict = {"Component":component,"Id":symbol, "Value":value}
    if(Database.sendMessage(component, "SET_SYMBOL", triggerDict) == None):
        print "Set Symbol Failure" + component + ":" + symbol + ":" + str(value)
        return False
    else:
        return True

#Handle messages from other components
def handleMessage(messageID, args):
    retDict= {}
    if (messageID == "SET_SYMBOL"):
        print "handleMessage: Set Symbol"
        retDict= {"Return": "Success"}
        Database.setSymbolValue(args["Component"], args["Id"], args["Value"])
    else:
        retDict= {"Return": "UnImplemented Command"}
    return retDict
    
