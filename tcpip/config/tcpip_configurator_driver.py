"""
Copyright (C) 2022-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

autoConnectTableGMAC = [["BASIC CONFIGURATION", "tcpipNetConfig_0:NETCONFIG_MAC_Dependency", "DRIVER LAYER", "drvGmac:libdrvGmac"]]
autoConnectTableGMAC0 = [["BASIC CONFIGURATION", "tcpipNetConfig_0:NETCONFIG_MAC_Dependency", "DRIVER LAYER", "drvGmac0:libdrvGmac0"]]
autoConnectTableGMAC1 = [["BASIC CONFIGURATION", "tcpipNetConfig_0:NETCONFIG_MAC_Dependency", "DRIVER LAYER", "drvGmac1:libdrvGmac1"]]
autoConnectTableEthmac = [["BASIC CONFIGURATION", "tcpipNetConfig_0:NETCONFIG_MAC_Dependency", "DRIVER LAYER", "drvPic32mEthmac:libdrvPic32mEthmac"]]
autoConnectTableEMAC0 = [["BASIC CONFIGURATION", "tcpipNetConfig_0:NETCONFIG_MAC_Dependency", "DRIVER LAYER", "drvEmac0:libdrvMac0"]]
autoConnectTableEMAC1 = [["BASIC CONFIGURATION", "tcpipNetConfig_0:NETCONFIG_MAC_Dependency", "DRIVER LAYER", "drvEmac1:libdrvMac1"]]
autoConnectTableWINC = [["BASIC CONFIGURATION", "tcpipNetConfig_0:NETCONFIG_MAC_Dependency", "DRIVER LAYER", "drvWifiWinc:libdrvWincMac"]]
################################################################################
#### Business Logic ####
################################################################################

import os

############################################################################
#### Code Generation ####
############################################################################
def instantiateComponent(tcpipAutoConfigDriverComponent):

    tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
    if (tcpipAutoConfigStackGroup == None):
        tcpipAutoConfigStackGroup = Database.createGroup(None, "TCP/IP STACK")
        
    tcpipAutoConfigDriverGroup = Database.findGroup("DRIVER LAYER")
    if (tcpipAutoConfigDriverGroup == None):
        tcpipAutoConfigDriverGroup = Database.createGroup("TCP/IP STACK", "DRIVER LAYER")

    seriesNode = ATDF.getNode('/avr-tools-device-file/devices')
    deviceSeries = seriesNode.getChildren()[0].getAttribute("series")
    # Enable Driver Configurator
    tcpipAutoConfigDrvEnable = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_DRV_ENABLE", None)
    tcpipAutoConfigDrvEnable.setVisible(False)
    tcpipAutoConfigDrvEnable.setDefaultValue(True)
    # Check if Wireless Module are present to show respective Drivers
    harmonyFrameworkPath = Variables.get("__FRAMEWORK_ROOT")     
    wirelessWincPresent = os.path.isdir(harmonyFrameworkPath + "/wireless_wifi/driver/winc")
    wirelessEthmacPresent = os.path.isdir(harmonyFrameworkPath + "/wireless_wifi/driver/ethmac")
    net_10Baset1sPresent = os.path.isdir(harmonyFrameworkPath + "/net_10base_t1s/driver")
    if ("SAMA7G" in Variables.get("__PROCESSOR")) and (Peripheral.moduleExists("GMAC")):
        # Enable GMAC0
        tcpipAutoConfigGMAC0 = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_GMAC0", None)
        tcpipAutoConfigGMAC0.setLabel("GMAC0")
        tcpipAutoConfigGMAC0.setVisible(True)
        tcpipAutoConfigGMAC0.setDescription("Enable GMAC0")
        tcpipAutoConfigGMAC0.setDependencies(tcpipAutoConfigGMAC0Enable, ["TCPIP_AUTOCONFIG_ENABLE_GMAC0"])
        # Enable GMAC1
        tcpipAutoConfigGMAC1 = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_GMAC1", None)
        tcpipAutoConfigGMAC1.setLabel("GMAC1")
        tcpipAutoConfigGMAC1.setVisible(True)
        tcpipAutoConfigGMAC1.setDescription("Enable GMAC1")
        tcpipAutoConfigGMAC1.setDependencies(tcpipAutoConfigGMAC1Enable, ["TCPIP_AUTOCONFIG_ENABLE_GMAC1"])         
    elif Peripheral.moduleExists("GMAC"): 
        # Enable GMAC
        tcpipAutoConfigGMAC = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_GMAC", None)
        tcpipAutoConfigGMAC.setLabel("GMAC")
        tcpipAutoConfigGMAC.setVisible(True)
        tcpipAutoConfigGMAC.setDescription("Enable GMAC")
        tcpipAutoConfigGMAC.setDependencies(tcpipAutoConfigGMACEnable, ["TCPIP_AUTOCONFIG_ENABLE_GMAC"])    
    elif ("PIC32M" in Variables.get("__PROCESSOR")):
        # Enable Ethernet MAC
        tcpipAutoConfigEthmac = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_ETHMAC", None)
        tcpipAutoConfigEthmac.setLabel("ETHMAC")
        if (deviceSeries != "PIC32MZW"):
            tcpipAutoConfigEthmac.setVisible(True)
        else:
            tcpipAutoConfigEthmac.setVisible(wirelessEthmacPresent)
        
        tcpipAutoConfigEthmac.setDescription("Enable ETHMAC")
        tcpipAutoConfigEthmac.setDependencies(tcpipAutoConfigETHMACEnable, ["TCPIP_AUTOCONFIG_ENABLE_ETHMAC"])  
    elif Peripheral.moduleExists( "EMAC" ):
        tcpipAutoConfigEMAC0 = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_EMAC0", None)
        tcpipAutoConfigEMAC0.setLabel("EMAC0")
        tcpipAutoConfigEMAC0.setVisible(True)
        tcpipAutoConfigEMAC0.setDescription("Enable EMAC0")
        tcpipAutoConfigEMAC0.setDependencies(tcpipAutoConfigEMAC0Enable, ["TCPIP_AUTOCONFIG_ENABLE_EMAC0"])     
        tcpipAutoConfigEMAC1 = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_EMAC1", None)
        tcpipAutoConfigEMAC1.setLabel("EMAC1")
        tcpipAutoConfigEMAC1.setVisible(True)
        tcpipAutoConfigEMAC1.setDescription("Enable EMAC1")
        tcpipAutoConfigEMAC1.setDependencies(tcpipAutoConfigEMAC1Enable, ["TCPIP_AUTOCONFIG_ENABLE_EMAC1"]) 
        
    # PPP driver menu
    processor =  Variables.get("__PROCESSOR")  

    if "PIC32MZ" in processor or "SAME5" in processor:
        tcpipAutoConfigPPPMenu = tcpipAutoConfigDriverComponent.createMenuSymbol("TCPIP_AUTOCONFIG_PPP_MENU", None)
        tcpipAutoConfigPPPMenu.setLabel("PPP Driver")
        tcpipAutoConfigPPPMenu.setVisible(True)
        tcpipAutoConfigPPPMenu.setDescription("PPP menu") 
    
    # Enable MIIM_Driver
    tcpipAutoConfigMIIM_Driver = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", None)
    tcpipAutoConfigMIIM_Driver.setLabel("MIIM_Driver")
    tcpipAutoConfigMIIM_Driver.setVisible(True)
    tcpipAutoConfigMIIM_Driver.setDescription("Enable MIIM_Driver")
    tcpipAutoConfigMIIM_Driver.setDependencies(tcpipAutoConfigMIIMDriverEnable, ["TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver"])        
    
    # PHY driver menu
    tcpipAutoConfigPhyMenu = tcpipAutoConfigDriverComponent.createMenuSymbol("TCPIP_AUTOCONFIG_PHY_MENU", None)
    tcpipAutoConfigPhyMenu.setLabel("PHY Drivers")
    tcpipAutoConfigPhyMenu.setVisible(True)
    tcpipAutoConfigPhyMenu.setDescription("PHY menu") 
    
    # Enable KSZ8041
    tcpipAutoConfigKSZ8041 = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_KSZ8041", tcpipAutoConfigPhyMenu)
    tcpipAutoConfigKSZ8041.setLabel("KSZ8041")
    tcpipAutoConfigKSZ8041.setVisible(True)
    tcpipAutoConfigKSZ8041.setDescription("Enable KSZ8041") 
    tcpipAutoConfigKSZ8041.setDependencies(tcpipAutoConfigKSZ8041Enable, ["TCPIP_AUTOCONFIG_ENABLE_KSZ8041"])  
    
    # Enable KSZ8061
    tcpipAutoConfigKSZ8061 = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_KSZ8061", tcpipAutoConfigPhyMenu)
    tcpipAutoConfigKSZ8061.setLabel("KSZ8061")
    tcpipAutoConfigKSZ8061.setVisible(True)
    tcpipAutoConfigKSZ8061.setDescription("Enable KSZ8061") 
    tcpipAutoConfigKSZ8061.setDependencies(tcpipAutoConfigKSZ8061Enable, ["TCPIP_AUTOCONFIG_ENABLE_KSZ8061"])   

    # Enable KSZ8081
    tcpipAutoConfigKSZ8081 = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_KSZ8081", tcpipAutoConfigPhyMenu)
    tcpipAutoConfigKSZ8081.setLabel("KSZ8081")
    tcpipAutoConfigKSZ8081.setVisible(True)
    tcpipAutoConfigKSZ8081.setDescription("Enable KSZ8081") 
    tcpipAutoConfigKSZ8081.setDependencies(tcpipAutoConfigKSZ8081Enable, ["TCPIP_AUTOCONFIG_ENABLE_KSZ8081"])   

    # Enable KSZ8091
    tcpipAutoConfigKSZ8091 = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_KSZ8091", tcpipAutoConfigPhyMenu)
    tcpipAutoConfigKSZ8091.setLabel("KSZ8091")
    tcpipAutoConfigKSZ8091.setVisible(True)
    tcpipAutoConfigKSZ8091.setDescription("Enable KSZ8091") 
    tcpipAutoConfigKSZ8091.setDependencies(tcpipAutoConfigKSZ8091Enable, ["TCPIP_AUTOCONFIG_ENABLE_KSZ8091"])
    
    # Enable KSZ8863
    tcpipAutoConfigKSZ8863 = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_KSZ8863", tcpipAutoConfigPhyMenu)
    tcpipAutoConfigKSZ8863.setLabel("KSZ8863")
    tcpipAutoConfigKSZ8863.setVisible(True)
    tcpipAutoConfigKSZ8863.setDescription("Enable KSZ8863") 
    tcpipAutoConfigKSZ8863.setDependencies(tcpipAutoConfigKSZ8863Enable, ["TCPIP_AUTOCONFIG_ENABLE_KSZ8863"]) 
    
    # Enable LAN8740
    tcpipAutoConfigLAN8740 = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_LAN8740", tcpipAutoConfigPhyMenu)
    tcpipAutoConfigLAN8740.setLabel("LAN8740")
    tcpipAutoConfigLAN8740.setVisible(True)
    tcpipAutoConfigLAN8740.setDescription("Enable LAN8740")
    tcpipAutoConfigLAN8740.setDependencies(tcpipAutoConfigLAN8740Enable, ["TCPIP_AUTOCONFIG_ENABLE_LAN8740"])       
    
    # Enable LAN8742a
    tcpipAutoConfigLAN8742a = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_LAN8742A", tcpipAutoConfigPhyMenu)
    tcpipAutoConfigLAN8742a.setLabel("LAN8742A")
    tcpipAutoConfigLAN8742a.setVisible(True)
    tcpipAutoConfigLAN8742a.setDescription("Enable LAN8742A")
    tcpipAutoConfigLAN8742a.setDependencies(tcpipAutoConfigLAN8742aEnable, ["TCPIP_AUTOCONFIG_ENABLE_LAN8742A"])       
    
    # Enable LAN8700
    tcpipAutoConfigLAN8700 = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_LAN8700", tcpipAutoConfigPhyMenu)
    tcpipAutoConfigLAN8700.setLabel("LAN8700")
    tcpipAutoConfigLAN8700.setVisible(True)
    tcpipAutoConfigLAN8700.setDescription("Enable LAN8700")
    tcpipAutoConfigLAN8700.setDependencies(tcpipAutoConfigLAN8700Enable, ["TCPIP_AUTOCONFIG_ENABLE_LAN8700"]) 
    
    # Enable LAN8720
    tcpipAutoConfigLAN8720 = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_LAN8720", tcpipAutoConfigPhyMenu)
    tcpipAutoConfigLAN8720.setLabel("LAN8720")
    tcpipAutoConfigLAN8720.setVisible(True)
    tcpipAutoConfigLAN8720.setDescription("Enable LAN8720")
    tcpipAutoConfigLAN8720.setDependencies(tcpipAutoConfigLAN8720Enable, ["TCPIP_AUTOCONFIG_ENABLE_LAN8720"]) 
    
    # Enable LAN9303
    tcpipAutoConfigLAN9303 = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_LAN9303", tcpipAutoConfigPhyMenu)
    tcpipAutoConfigLAN9303.setLabel("LAN9303")
    tcpipAutoConfigLAN9303.setVisible(True)
    tcpipAutoConfigLAN9303.setDescription("Enable LAN9303")
    tcpipAutoConfigLAN9303.setDependencies(tcpipAutoConfigLAN9303Enable, ["TCPIP_AUTOCONFIG_ENABLE_LAN9303"]) 
    
    # Enable DP83640
    tcpipAutoConfigDP83640 = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_DP83640", tcpipAutoConfigPhyMenu)
    tcpipAutoConfigDP83640.setLabel("DP83640")
    tcpipAutoConfigDP83640.setVisible(True)
    tcpipAutoConfigDP83640.setDescription("Enable DP83640")
    tcpipAutoConfigDP83640.setDependencies(tcpipAutoConfigDP83640Enable, ["TCPIP_AUTOCONFIG_ENABLE_DP83640"]) 
    
    # Enable DP83848
    tcpipAutoConfigDP83848 = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_DP83848", tcpipAutoConfigPhyMenu)
    tcpipAutoConfigDP83848.setLabel("DP83848")
    tcpipAutoConfigDP83848.setVisible(True)
    tcpipAutoConfigDP83848.setDescription("Enable DP83848")
    tcpipAutoConfigDP83848.setDependencies(tcpipAutoConfigDP83848Enable, ["TCPIP_AUTOCONFIG_ENABLE_DP83848"]) 
    
    # Enable IP101GR
    tcpipAutoConfigIP101GR = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_IP101GR", tcpipAutoConfigPhyMenu)
    tcpipAutoConfigIP101GR.setLabel("IP101GR")
    tcpipAutoConfigIP101GR.setVisible(True)
    tcpipAutoConfigIP101GR.setDescription("Enable IP101GR")
    tcpipAutoConfigIP101GR.setDependencies(tcpipAutoConfigIP101GREnable, ["TCPIP_AUTOCONFIG_ENABLE_IP101GR"])     

    # Enable VSC8540
    tcpipAutoConfigVSC8540 = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_VSC8540", tcpipAutoConfigPhyMenu)
    tcpipAutoConfigVSC8540.setLabel("VSC8540")
    tcpipAutoConfigVSC8540.setVisible(True)
    tcpipAutoConfigVSC8540.setDescription("Enable VSC8540")
    tcpipAutoConfigVSC8540.setDependencies(tcpipAutoConfigVSC8540Enable, ["TCPIP_AUTOCONFIG_ENABLE_VSC8540"])  
    
    # Enable LAN867x
    tcpipAutoConfigLAN867x = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_LAN867x", tcpipAutoConfigPhyMenu)
    tcpipAutoConfigLAN867x.setLabel("LAN867x")
    tcpipAutoConfigLAN867x.setVisible(net_10Baset1sPresent)
    tcpipAutoConfigLAN867x.setDescription("Enable LAN867x")
    tcpipAutoConfigLAN867x.setDependencies(tcpipAutoConfigLAN867xEnable, ["TCPIP_AUTOCONFIG_ENABLE_LAN867x"])

    # Enable LAN9354
    tcpipAutoConfigLAN9354 = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_LAN9354", tcpipAutoConfigPhyMenu)
    tcpipAutoConfigLAN9354.setLabel("LAN9354")
    tcpipAutoConfigLAN9354.setVisible(True)
    tcpipAutoConfigLAN9354.setDescription("Enable LAN9354")
    tcpipAutoConfigLAN9354.setDependencies(tcpipAutoConfigLAN9354Enable, ["TCPIP_AUTOCONFIG_ENABLE_LAN9354"]) 
    
    # Enable LAN8770
    tcpipAutoConfigLAN8770 = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_LAN8770", tcpipAutoConfigPhyMenu)
    tcpipAutoConfigLAN8770.setLabel("LAN8770")
    tcpipAutoConfigLAN8770.setVisible(True)
    tcpipAutoConfigLAN8770.setDescription("Enable LAN8770")
    tcpipAutoConfigLAN8770.setDependencies(tcpipAutoConfigLAN8770Enable, ["TCPIP_AUTOCONFIG_ENABLE_LAN8770"]) 
    
    # External Ethernet Controller driver menu
    tcpipAutoConfigExtEthControllerMenu = tcpipAutoConfigDriverComponent.createMenuSymbol("TCPIP_AUTOCONFIG_EXT_ETH_CTRL_MENU", None)
    tcpipAutoConfigExtEthControllerMenu.setLabel("External Ethernet Controllers")
    tcpipAutoConfigExtEthControllerMenu.setVisible(True)
    tcpipAutoConfigExtEthControllerMenu.setDescription("External Ethernet Controller driver menu") 
        
    # Enable ENCX24J600
    tcpipAutoConfigENCX24J600 = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_ENCX24J600", tcpipAutoConfigExtEthControllerMenu)
    tcpipAutoConfigENCX24J600.setLabel("ENCX24J600")
    tcpipAutoConfigENCX24J600.setVisible(True)
    tcpipAutoConfigENCX24J600.setDescription("Enable ENCX24J600")
    tcpipAutoConfigENCX24J600.setDependencies(tcpipAutoConfigENCX24J600Enable, ["TCPIP_AUTOCONFIG_ENABLE_ENCX24J600"])   
        
    # Enable ENC28J60
    tcpipAutoConfigENC28J60 = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_ENC28J60", tcpipAutoConfigExtEthControllerMenu)
    tcpipAutoConfigENC28J60.setLabel("ENC28J60")
    tcpipAutoConfigENC28J60.setVisible(True)
    tcpipAutoConfigENC28J60.setDescription("Enable ENC28J60")
    tcpipAutoConfigENC28J60.setDependencies(tcpipAutoConfigENC28J60Enable, ["TCPIP_AUTOCONFIG_ENABLE_ENC28J60"])   
        
    # Enable WINC MAC
    tcpipAutoConfigWINC = tcpipAutoConfigDriverComponent.createBooleanSymbol("TCPIP_AUTOCONFIG_ENABLE_WINC", None)
    tcpipAutoConfigWINC.setLabel("WINC")
    tcpipAutoConfigWINC.setVisible(wirelessWincPresent)
    tcpipAutoConfigWINC.setDescription("Enable WINC")
    tcpipAutoConfigWINC.setDependencies(tcpipAutoConfigWINCEnable, ["TCPIP_AUTOCONFIG_ENABLE_WINC"])    
########################################################################################################
def finalizeComponent(tcpipAutoConfigDriverComponent):
    tcpipAutoConfigDriverGroup = Database.findGroup("DRIVER LAYER")
    tcpipAutoConfigDriverGroup.addComponent(tcpipAutoConfigDriverComponent.getID())
    
    if(Database.getSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_APPS_ENABLE") != True) and (Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_TRANS_ENABLE") != True) and (Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_NET_ENABLE") != True) and (Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_BASIC_ENABLE") != True):
        Database.setActiveGroup("DRIVER LAYER") 
#######################################################################################################
def enableTcpipAutoConfigDrv(enable):

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
            
        if(Database.getComponentByID("tcpip_basic_config") == None):
            res = tcpipAutoConfigBasicGroup.addComponent("tcpip_basic_config")
            res = Database.activateComponents(["tcpip_basic_config"], "BASIC CONFIGURATION", False)         
            
        if(Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_STACK") != True):
            setVal("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_STACK", True)
        if(Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_NETCONFIG") != True):            
            setVal("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_NETCONFIG", True)
#################Business Logic- Driver Layer ######################################### 
def tcpipAutoConfigGMACEnable(symbol, event):
    tcpipAutoConfigDriverGroup = Database.findGroup("DRIVER LAYER")
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvGmac"],"DRIVER LAYER")   
        tcpipAutoConfigDriverGroup.setAttachmentVisible("drvGmac", "libdrvGmac")
        res = Database.connectDependencies(autoConnectTableGMAC)
    else:
        res = Database.deactivateComponents(["drvGmac"])
        
def tcpipAutoConfigGMAC0Enable(symbol, event):
    tcpipAutoConfigDriverGroup = Database.findGroup("DRIVER LAYER")
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvGmac0"],"DRIVER LAYER")   
        tcpipAutoConfigDriverGroup.setAttachmentVisible("drvGmac0", "libdrvGmac0")
        res = Database.connectDependencies(autoConnectTableGMAC0)
    else:
        res = Database.deactivateComponents(["drvGmac0"])
        
def tcpipAutoConfigGMAC1Enable(symbol, event):
    tcpipAutoConfigDriverGroup = Database.findGroup("DRIVER LAYER")
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvGmac1"],"DRIVER LAYER")   
        tcpipAutoConfigDriverGroup.setAttachmentVisible("drvGmac1", "libdrvGmac1")
        res = Database.connectDependencies(autoConnectTableGMAC1)
    else:
        res = Database.deactivateComponents(["drvGmac0"])        
def tcpipAutoConfigETHMACEnable(symbol, event):
    tcpipAutoConfigDriverGroup = Database.findGroup("DRIVER LAYER")
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvPic32mEthmac"],"DRIVER LAYER")   
        tcpipAutoConfigDriverGroup.setAttachmentVisible("drvPic32mEthmac", "libdrvPic32mEthmac")
        res = Database.connectDependencies(autoConnectTableEthmac)
    else:
        res = Database.deactivateComponents(["drvPic32mEthmac"])
    
def tcpipAutoConfigEMAC0Enable(symbol, event):
    tcpipAutoConfigDriverGroup = Database.findGroup("DRIVER LAYER")
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvEmac0"],"DRIVER LAYER")   
        tcpipAutoConfigDriverGroup.setAttachmentVisible("drvEmac0", "libdrvMac0")
        res = Database.connectDependencies(autoConnectTableEMAC0)
    else:
        res = Database.deactivateComponents(["drvEmac0"])
    
def tcpipAutoConfigEMAC1Enable(symbol, event):
    tcpipAutoConfigDriverGroup = Database.findGroup("DRIVER LAYER")
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvEmac1"],"DRIVER LAYER")   
        tcpipAutoConfigDriverGroup.setAttachmentVisible("drvEmac1", "libdrvMac1")
        res = Database.connectDependencies(autoConnectTableEMAC1)
    else:
        res = Database.deactivateComponents(["drvEmac1"])
def tcpipAutoConfigMIIMDriverEnable(symbol, event):
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvMiim"],"DRIVER LAYER")   
    else:
        res = Database.deactivateComponents(["drvMiim"])

    
def tcpipAutoConfigKSZ8041Enable(symbol, event):
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvExtPhyKsz8041"],"DRIVER LAYER")  
        if(Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver") != True):
            Database.setSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", True, 2)      
    else:
        res = Database.deactivateComponents(["drvExtPhyKsz8041"])
        
def tcpipAutoConfigKSZ8061Enable(symbol, event):
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvExtPhyKsz8061"],"DRIVER LAYER")  
        if(Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver") != True):
            Database.setSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", True, 2)      
    else:
        res = Database.deactivateComponents(["drvExtPhyKsz8061"])
    
    
def tcpipAutoConfigKSZ8081Enable(symbol, event):
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvExtPhyKsz8081"],"DRIVER LAYER")  
        if(Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver") != True):
            Database.setSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", True, 2)      
    else:
        res = Database.deactivateComponents(["drvExtPhyKsz8081"])
    
def tcpipAutoConfigKSZ8091Enable(symbol, event):
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvExtPhyKsz8091"],"DRIVER LAYER")  
        if(Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver") != True):
            Database.setSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", True, 2)      
    else:
        res = Database.deactivateComponents(["drvExtPhyKsz8091"])
    
def tcpipAutoConfigKSZ8863Enable(symbol, event):
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvExtPhyKsz8863"],"DRIVER LAYER")  
        if(Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver") != True):
            Database.setSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", True, 2)      
    else:
        res = Database.deactivateComponents(["drvExtPhyKsz8863"])
       
def tcpipAutoConfigLAN8740Enable(symbol, event):
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvExtPhyLan8740"],"DRIVER LAYER")  
        if(Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver") != True):
            Database.setSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", True, 2)      
    else:
        res = Database.deactivateComponents(["drvExtPhyLan8740"])
        
def tcpipAutoConfigLAN8742aEnable(symbol, event):
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvExtPhyLan8742a"],"DRIVER LAYER")  
        if(Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver") != True):
            Database.setSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", True, 2)      
    else:
        res = Database.deactivateComponents(["drvExtPhyLan8742a"])
        
def tcpipAutoConfigLAN8700Enable(symbol, event):
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvExtPhyLan8700"],"DRIVER LAYER")  
        if(Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver") != True):
            Database.setSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", True, 2)      
    else:
        res = Database.deactivateComponents(["drvExtPhyLan8700"])
        
def tcpipAutoConfigLAN8720Enable(symbol, event):
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvExtPhyLan8720"],"DRIVER LAYER")  
        if(Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver") != True):
            Database.setSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", True, 2)      
    else:
        res = Database.deactivateComponents(["drvExtPhyLan8720"])
        
def tcpipAutoConfigLAN9303Enable(symbol, event):
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvExtPhyLan9303"],"DRIVER LAYER")  
        if(Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver") != True):
            Database.setSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", True, 2)      
    else:
        res = Database.deactivateComponents(["drvExtPhyLan9303"])
        
def tcpipAutoConfigDP83640Enable(symbol, event):
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvExtPhyDp83640"],"DRIVER LAYER")  
        if(Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver") != True):
            Database.setSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", True, 2)      
    else:
        res = Database.deactivateComponents(["drvExtPhyDp83640"])
        
def tcpipAutoConfigDP83848Enable(symbol, event):
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvExtPhyDp83848"],"DRIVER LAYER")  
        if(Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver") != True):
            Database.setSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", True, 2)      
    else:
        res = Database.deactivateComponents(["drvExtPhyDp83848"])
        
def tcpipAutoConfigIP101GREnable(symbol, event):
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvExtPhyIp101gr"],"DRIVER LAYER")  
        if(Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver") != True):
            Database.setSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", True, 2)      
    else:
        res = Database.deactivateComponents(["drvExtPhyIp101gr"])

def tcpipAutoConfigVSC8540Enable(symbol, event):
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvExtPhyVsc8540"],"DRIVER LAYER")  
        if(Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver") != True):
            Database.setSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", True, 2)      
    else:
        res = Database.deactivateComponents(["drvExtPhyVsc8540"])

def tcpipAutoConfigLAN867xEnable(symbol, event):
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvExtPhyLan867x"],"DRIVER LAYER")  
        if(Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver") != True):
            Database.setSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", True, 2)
    else:
        res = Database.deactivateComponents(["drvExtPhyLan867x"])
        
def tcpipAutoConfigLAN9354Enable(symbol, event):
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvExtPhyLan9354"],"DRIVER LAYER")  
        if(Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver") != True):
            Database.setSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", True, 2)      
    else:
        res = Database.deactivateComponents(["drvExtPhyLan9354"])
        
def tcpipAutoConfigLAN8770Enable(symbol, event):
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvExtPhyLan8770"],"DRIVER LAYER")  
        if(Database.getSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver") != True):
            Database.setSymbolValue("tcpip_driver_config", "TCPIP_AUTOCONFIG_ENABLE_MIIM_Driver", True, 2)      
    else:
        res = Database.deactivateComponents(["drvExtPhyLan8770"])
        
def tcpipAutoConfigENCX24J600Enable(symbol, event):
    tcpipAutoConfigDriverGroup = Database.findGroup("DRIVER LAYER")
    tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True) and (Database.getComponentByID("drvExtMacEncx24j600") == None):
        res = Database.activateComponents(["drvExtMacEncx24j600"],"DRIVER LAYER")  
        # tcpipAutoConfigDriverGroup.setAttachmentVisible("drvExtMacEncx24j600_0", "libdrvExtMacEncx24j600")
        # tcpipAutoConfigDriverGroup.setAttachmentVisible("drvExtMacEncx24j600_0","ENCX24J600_SPI")
        # tcpipAutoConfigStackGroup.setAttachmentVisible("DRIVER LAYER", "drvExtMacEncx24j600_0:ENCX24J600_SPI")
    else:
        res = Database.deactivateComponents(["drvExtMacEnc28j60"])
     
   
def tcpipAutoConfigENC28J60Enable(symbol, event):
    tcpipAutoConfigDriverGroup = Database.findGroup("DRIVER LAYER")
    tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True) and (Database.getComponentByID("drvExtMacEnc28j60") == None):
        res = Database.activateComponents(["drvExtMacEnc28j60"],"DRIVER LAYER")  
        # tcpipAutoConfigDriverGroup.setAttachmentVisible("drvExtMacEnc28j60_0", "libdrvExtMacEnc28j60")
        # tcpipAutoConfigDriverGroup.setAttachmentVisible("drvExtMacEnc28j60_0","ENC28J60_SPI")
        # tcpipAutoConfigStackGroup.setAttachmentVisible("DRIVER LAYER", "drvExtMacEnc28j60_0:ENC28J60_SPI")
    else:
        res = Database.deactivateComponents(["drvExtMacEnc28j60"])
                      
def tcpipAutoConfigWINCEnable(symbol, event):
    tcpipAutoConfigDriverGroup = Database.findGroup("DRIVER LAYER")
    tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
    enableTcpipAutoConfigDrv(True)
    if (event["value"] == True):
        res = Database.activateComponents(["drvWifiWinc"],"DRIVER LAYER")   
        #Todo: change to Database.sendMessage(); but need handleMessage() in drvWifiWinc
        Database.setSymbolValue("drvWifiWinc", "DRV_WIFI_WINC_DRIVER_MODE", "Ethernet Mode")
        Database.setSymbolValue("drvWifiWinc", "DRV_WIFI_WINC_USE_TCPIP_STACK", True)
        tcpipAutoConfigDriverGroup.setAttachmentVisible("drvWifiWinc", "libdrvWincMac")
        tcpipAutoConfigDriverGroup.setAttachmentVisible("drvWifiWinc", "spi")
        tcpipAutoConfigStackGroup.setAttachmentVisible("DRIVER LAYER", "drvWifiWinc:spi")
        res = Database.connectDependencies(autoConnectTableWINC)        
    else:
        res = Database.deactivateComponents(["drvWifiWinc"])            

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
    
