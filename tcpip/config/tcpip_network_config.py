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

############### Network Configuration ######################
tcpipNetConfigNumMaximum = 10
tcpipNetConfigNumPrev = 1

##############################################################
def instantiateComponent(tcpipNetConfigComponent, index):
    
    configName = Variables.get("__CONFIGURATION_NAME")  
    print(tcpipNetConfigComponent.getID())

    processor = Variables.get("__PROCESSOR")
    
    tcpipNetConfigIndex = tcpipNetConfigComponent.createIntegerSymbol("INDEX", None)
    tcpipNetConfigIndex.setVisible(False)
    tcpipNetConfigIndex.setDefaultValue(index)  
    
    netconfig_interface_counter_dict = {}
    netconfig_interface_counter_dict = Database.sendMessage("tcpipNetConfig", "NETCONFIG_INTERFACE_COUNTER_INC", netconfig_interface_counter_dict)
    
    # Network interface Number
    tcpipNetConfigNum = tcpipNetConfigComponent.createIntegerSymbol("TCPIP_STACK_NETWORK_CONFIG_NUMBER", None)
    tcpipNetConfigNum.setLabel("Network Configurations Index")
    tcpipNetConfigNum.setVisible(True)
    tcpipNetConfigNum.setValue(int(str(index)),1)
    tcpipNetConfigNum.setReadOnly(True)
    
    # Network interface Enable
    tcpipNetConfigEnable = tcpipNetConfigComponent.createBooleanSymbol("TCPIP_STACK_NETWORK_CONFIG_IDX" + str(index),None)
    tcpipNetConfigEnable.setLabel("Network interface Enable")
    tcpipNetConfigEnable.setVisible(False)
    tcpipNetConfigEnable.setValue(True,1)

    # Network interface name
    tcpipNetIfName = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX" + str(index),None)   
    tcpipNetIfName.setLabel("Interface")
    tcpipNetIfName.setVisible(True)
    tcpipNetIfName.setDefaultValue("")
    tcpipNetIfName.setReadOnly(True)

    # Network Host name
    tcpipNetHostName = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_HOST_NAME_IDX" + str(index), None)
    tcpipNetHostName.setLabel("Host Name")
    tcpipNetHostName.setVisible(True)
    tcpipNetHostName.setDefaultValue("")
    tcpipNetHostName.setDependencies(tcpipNetHostNameUpdate, [tcpipNetIfName.getID()])

    # Network Interface MAC address
    tcpipNetMacAddress = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX" + str(index),None)
    tcpipNetMacAddress.setLabel("Mac Address")
    tcpipNetMacAddress.setVisible(True)
    tcpipNetMacAddress.setDefaultValue("")
    tcpipNetMacAddress.setDependencies(tcpipNetMacAddrUpdate, [tcpipNetIfName.getID()])
    
    # Network Interface IP address
    tcpipNetIpAddress = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_IP_ADDRESS_IDX" + str(index), None)
    tcpipNetIpAddress.setLabel("IPv4 Static Address")
    tcpipNetIpAddress.setVisible(True)
    tcpipNetIpAddress.setDefaultValue("")
    tcpipNetIpAddress.setDependencies(tcpipNetIpAddrUpdate, [tcpipNetIfName.getID()])

    # Network Interface IP address Mask
    tcpipNetIpAddrMask = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_IP_MASK_IDX" + str(index),None)
    tcpipNetIpAddrMask.setLabel("IPv4 SubNet Mask")
    tcpipNetIpAddrMask.setVisible(True)
    tcpipNetIpAddrMask.setDefaultValue("255.255.255.0")

    # Network Interface Gateway IP address
    tcpipNetGatewayIpAddr = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_GATEWAY_IDX" + str(index),None)
    tcpipNetGatewayIpAddr.setLabel("IPv4 Default Gateway Address")
    tcpipNetGatewayIpAddr.setVisible(True)
    tcpipNetGatewayIpAddr.setDefaultValue("")
    tcpipNetGatewayIpAddr.setDependencies(tcpipNetGatewayIpAddrUpdate, [tcpipNetIfName.getID()])    
    
    # Network Interface Primary DNS IP address
    tcpipNetPrimDnsIpAddr = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_DNS_IDX" + str(index),None)
    tcpipNetPrimDnsIpAddr.setLabel("IPv4 Primary DNS")
    tcpipNetPrimDnsIpAddr.setVisible(True)
    tcpipNetPrimDnsIpAddr.setDefaultValue("")
    tcpipNetPrimDnsIpAddr.setDependencies(tcpipNetPrimDnsIpAddrUpdate, [tcpipNetIfName.getID()])    
    
    # Network Configuration Start-up Flags Menu
    tcpipNetStartupFlag = tcpipNetConfigComponent.createMenuSymbol("TCPIP_NETWORK_STARTUP_FLAG_IDX" + str(index),None)
    tcpipNetStartupFlag.setLabel("Network Configuration Start-up Flags")
    tcpipNetStartupFlag.setVisible(True)    

    tcpipNetDhcpFlag = tcpipNetConfigComponent.createKeyValueSetSymbol("TCPIP_NETWORK_INTERFACE_FLAG_DHCP_IDX"+str(index),tcpipNetStartupFlag)
    tcpipNetDhcpFlag.setVisible(True)
    tcpipNetDhcpFlag.setLabel("DHCP Flag")
    tcpipNetDhcpFlag.addKey("DHCP_CLIENT_ENABLE", "0", "Enable DHCP Client")
    tcpipNetDhcpFlag.addKey("DHCP_SERVER_ENABLE", "1", "Enable DHCP Server")
    tcpipNetDhcpFlag.addKey("ZEROCONF_LL_ENABLE", "2", "Enable ZeroConf LL")
    tcpipNetDhcpFlag.addKey("DHCP_NONE", "3", "None")
    tcpipNetDhcpFlag.setDisplayMode("Description")
    tcpipNetDhcpFlag.setOutputMode("Key")
    tcpipNetDhcpFlag.setDefaultValue(0)

    tcpipNetDnsFlag = tcpipNetConfigComponent.createKeyValueSetSymbol("TCPIP_NETWORK_INTERFACE_FLAG_DNS_IDX"+str(index),tcpipNetStartupFlag)
    tcpipNetDnsFlag.setVisible(True)
    tcpipNetDnsFlag.setLabel("DNS Flag")
    tcpipNetDnsFlag.addKey("DNS_CLIENT_ENABLE", "0", "Enable DNS Client")
    tcpipNetDnsFlag.addKey("DNS_SERVER_ENABLE", "1", "Enable DNS Server")
    tcpipNetDnsFlag.addKey("DNS_NONE", "2", "None")
    tcpipNetDnsFlag.setDisplayMode("Description")
    tcpipNetDnsFlag.setOutputMode("Key")
    tcpipNetDnsFlag.setDefaultValue(0)  
    
    # Enable Multicast on this Network Configuration
    tcpipNetMcastEnable = tcpipNetConfigComponent.createBooleanSymbol("TCPIP_NETWORK_CONFIG_MULTICAST_IDX"+str(index),tcpipNetStartupFlag)
    tcpipNetMcastEnable.setLabel("Multicast Enabled on this Interface")
    tcpipNetMcastEnable.setVisible(True)
    tcpipNetMcastEnable.setDefaultValue(False)

    # Input IPv6 Static Address and Subnet Prefix Length
    tcpipNetIpv6AddrSubnetInput = tcpipNetConfigComponent.createBooleanSymbol("TCPIP_NETWORK_INTERFACE_FLAG_IPV6_ADDRESS_IDX"+str(index),tcpipNetStartupFlag)
    tcpipNetIpv6AddrSubnetInput.setLabel("IPv6 Static Address and Subnet Prefix Length")
    tcpipNetIpv6AddrSubnetInput.setVisible(False)
    tcpipNetIpv6AddrSubnetInput.setDefaultValue(False)  
    tcpipNetIpv6AddrSubnetInput.setDependencies(tcpipNetConfigMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])  
    
    # IPv6 Static address for this interface
    tcpipNetIpv6StatAddr = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_IPV6_ADDRESS_IDX" + str(index), tcpipNetIpv6AddrSubnetInput)
    tcpipNetIpv6StatAddr.setLabel("IPv6 Static Address")
    tcpipNetIpv6StatAddr.setVisible(False)
    tcpipNetIpv6StatAddr.setDefaultValue("fde4:8dba:82e1::")
    tcpipNetIpv6StatAddr.setDependencies(tcpipNetConfigMenuVisible, [tcpipNetIpv6AddrSubnetInput.getID()])  
    
    # IPv6 Static address Prefix Length for this interface
    tcpipNetIpv6StatAddrPrfxLen = tcpipNetConfigComponent.createIntegerSymbol("TCPIP_NETWORK_DEFAULT_IPV6_PREFIX_LENGTH_IDX" + str(index), tcpipNetIpv6AddrSubnetInput)
    tcpipNetIpv6StatAddrPrfxLen.setLabel("IPv6 Static Address Prefix Length")
    tcpipNetIpv6StatAddrPrfxLen.setVisible(False)
    tcpipNetIpv6StatAddrPrfxLen.setDefaultValue(64)
    tcpipNetIpv6StatAddrPrfxLen.setDependencies(tcpipNetConfigMenuVisible, [tcpipNetIpv6AddrSubnetInput.getID()])
        
    # IPv6 Default Gateway Address for this interface
    tcpipNetIpv6GatewayAddr = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_IPV6_GATEWAY_IDX" + str(index), tcpipNetIpv6AddrSubnetInput)
    tcpipNetIpv6GatewayAddr.setLabel("IPv6 Default Gateway Address")
    tcpipNetIpv6GatewayAddr.setVisible(False)
    tcpipNetIpv6GatewayAddr.setDefaultValue("")
    tcpipNetIpv6GatewayAddr.setDependencies(tcpipNetConfigMenuVisible, [tcpipNetIpv6AddrSubnetInput.getID()])     

    # G3-PLC support for this interface
    tcpipNetIpv6G3PlcNet = tcpipNetConfigComponent.createBooleanSymbol("TCPIP_NETWORK_INTERFACE_FLAG_IPV6_G3_NET_IDX"+str(index), tcpipNetStartupFlag)
    tcpipNetIpv6G3PlcNet.setLabel("IPv6 G3-PLC network")
    tcpipNetIpv6G3PlcNet.setVisible(False)
    tcpipNetIpv6G3PlcNet.setDefaultValue(False)  
    tcpipNetIpv6G3PlcNet.setDependencies(tcpipNetG3MenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6", "tcpipIPv6.TCPIP_IPV6_G3_PLC_SUPPORT"])  
    
    # G3-PLC router/device on this net
    tcpipNetIpv6G3PlcRouter = tcpipNetConfigComponent.createBooleanSymbol("TCPIP_NETWORK_INTERFACE_FLAG_IPV6_G3_NET_ROUTER_IDX" + str(index), tcpipNetIpv6G3PlcNet)
    tcpipNetIpv6G3PlcRouter.setLabel("G3-PLC router")
    tcpipNetIpv6G3PlcRouter.setVisible(False)
    tcpipNetIpv6G3PlcRouter.setDefaultValue(False)
    tcpipNetIpv6G3PlcRouter.setDependencies(tcpipNetG3RouterMenuVisible, ["tcpipIPv6.TCPIP_IPV6_G3_PLC_BORDER_ROUTER", "TCPIP_NETWORK_INTERFACE_FLAG_IPV6_G3_NET_IDX"+str(index)])  

    # G3-PLC router advertisements
    tcpipNetIpv6G3Advertise = tcpipNetConfigComponent.createBooleanSymbol("TCPIP_NETWORK_INTERFACE_FLAG_IPV6_G3_NET_ROUTER_ADV_IDX" + str(index), tcpipNetIpv6G3PlcRouter)
    tcpipNetIpv6G3Advertise.setLabel("G3-PLC router send advertisements")
    tcpipNetIpv6G3Advertise.setVisible(False)
    tcpipNetIpv6G3Advertise.setDefaultValue(False)
    tcpipNetIpv6G3Advertise.setDependencies(tcpipNetConfigMenuVisible, ["TCPIP_NETWORK_INTERFACE_FLAG_IPV6_G3_NET_ROUTER_IDX"+str(index)])  


    # Advanced Settings
    tcpipNetAdvSettings = tcpipNetConfigComponent.createMenuSymbol("TCPIP_NETWORK_ADV_SETTING_IDX" + str(index), None)
    tcpipNetAdvSettings.setLabel("Advanced Settings")
    tcpipNetAdvSettings.setDescription("Advanced Settings")
    tcpipNetAdvSettings.setVisible(True)

    # Network Interface Secondary DNS IP address
    tcpipNetSecDnsIpAddr = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_SECOND_DNS_IDX" + str(index),tcpipNetAdvSettings)
    tcpipNetSecDnsIpAddr.setLabel("IPv4 Secondary DNS")
    tcpipNetSecDnsIpAddr.setVisible(True)
    tcpipNetSecDnsIpAddr.setDefaultValue("0.0.0.0")  
    
    # Network Interface MAC Driver Object
    tcpipNetMacDrvObj = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX" + str(index),tcpipNetAdvSettings)
    tcpipNetMacDrvObj.setLabel("Network MAC Driver")
    tcpipNetMacDrvObj.setVisible(True)
    tcpipNetMacDrvObj.setDefaultValue("")
    tcpipNetMacDrvObj.setDependencies(tcpipNetMACDrvObjUpdate, [tcpipNetIfName.getID()])
    
    # Add this interface to MAC Bridge
    tcpipAddMacBridge = tcpipNetConfigComponent.createBooleanSymbol("TCPIP_NETWORK_MACBRIDGE_ADD_IDX"+str(index),tcpipNetAdvSettings)
    tcpipAddMacBridge.setLabel("Add Interface to MAC Bridge")
    if( Database.getSymbolValue("tcpipNetConfig", "TCPIP_STACK_NETWORK_INTERAFCE_COUNT") > 1):
        tcpipAddMacBridge.setVisible(True)
    else: 
        tcpipAddMacBridge.setVisible(False)
    tcpipAddMacBridge.setDefaultValue(False)  
    tcpipAddMacBridge.setDependencies(tcpipNetConfigAddMACBridge, ["tcpipNetConfig.TCPIP_STACK_NETWORK_INTERAFCE_COUNT"])    

    # Enable MAC Global Config
    tcpipEnableMacBridgeGlobalConfig = tcpipNetConfigComponent.createBooleanSymbol("TCPIP_NETWORK_MACBRIDGE_GLOBAL_CONFIG_IDX"+str(index),tcpipNetAdvSettings)
    tcpipEnableMacBridgeGlobalConfig.setLabel("MAC Bridge GLobal")
    tcpipEnableMacBridgeGlobalConfig.setVisible(False)
    tcpipEnableMacBridgeGlobalConfig.setDefaultValue(False)  
    tcpipEnableMacBridgeGlobalConfig.setDependencies(tcpipNetConfigMACBridgeGlobal, [tcpipAddMacBridge.getID()]) 
    
    # Network Interface Power Mode
    tcpipNetPwrMode = tcpipNetConfigComponent.createKeyValueSetSymbol("TCPIP_NETWORK_DEFAULT_POWER_MODE_IDX" + str(index),tcpipNetAdvSettings)
    tcpipNetPwrMode.setLabel("Power Mode")
    tcpipNetPwrMode.setVisible(True)
    tcpipNetPwrMode.addKey("full", "0" , "Power full" )
    tcpipNetPwrMode.addKey("down", "1" , "Power down" )
    tcpipNetPwrMode.setDisplayMode("Key")
    tcpipNetPwrMode.setOutputMode("Key")
    tcpipNetPwrMode.setDefaultValue(0)
    
    tcpipNetConfigSysConfigFile = tcpipNetConfigComponent.createFileSymbol("TCPIP_NETWORK_CONFIG", None)
    tcpipNetConfigSysConfigFile.setType("STRING")
    tcpipNetConfigSysConfigFile.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipNetConfigSysConfigFile.setSourcePath("tcpip/config/network_config_idx.h.ftl")
    tcpipNetConfigSysConfigFile.setMarkup(True)
###############################################################################################################         
def tcpipNetConfigMenuVisible(symbol, event):
    if (event["value"] == True):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)
        
def tcpipNetG3MenuVisible(symbol, event):
    useIpv6 = Database.getSymbolValue("tcpipIPv6","TCPIP_STACK_USE_IPV6")
    useG3Plc = Database.getSymbolValue("tcpipIPv6","TCPIP_IPV6_G3_PLC_SUPPORT")
    
    if useIpv6 and useG3Plc:
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def tcpipNetG3RouterMenuVisible(symbol, event):
    useG3PlcRouter = Database.getSymbolValue("tcpipIPv6","TCPIP_IPV6_G3_PLC_BORDER_ROUTER")

    if useG3PlcRouter:
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def tcpipNetConfigAddMACBridge(symbol, event):
    if (event["value"] > 1):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def tcpipNetConfigMACBridgeGlobal(symbol, event):
    macbridge_global_config_dict = {}
    if (event["value"] == True):
        macbridge_global_config_dict = Database.sendMessage("tcpipNetConfig", "NETCONFIG_MAC_BRIDGE_ENABLE", macbridge_global_config_dict)
    else:
        macbridge_global_config_dict = Database.sendMessage("tcpipNetConfig", "NETCONFIG_MAC_BRIDGE_DISABLE", macbridge_global_config_dict)
    
      
def tcpipNetHostNameUpdate(symbol, event):
    interfaceToHostName = {
        'ETHMAC':       'MCHPBOARD_E',
        'GMAC':         'MCHPBOARD_C',
        'GMAC0':        'MCHPGMAC_0',
        'GMAC1':        'MCHPGMAC_1',
        'ETH':          'MCHPBOARD_ETH',
        'ENCX24J600':   'MCHPENCX24_E',
        'ENC28J60':     'MCHPENC28_E',
        'EMAC0':        'SAM9X60_EK',
        'EMAC1':        'SAM9X60_EK',
        'MRF24WN':      'MCHPBOARD_W',
        'WINC':         'MCHPBOARD_W',
        'WINC1500':     'MCHPBOARD_W',
        'WILC1000':     'MCHPBOARD_W',
        'PIC32MZW1':    'MCHPBOARD_W',
        'LAN865x':      'MCHP_LAN865x',
        'G3ADPMAC':     'MCHPBOARD_PLC',
    }
    print "tcpipNetHostNameUpdate: symbol.getValue()=" + str( symbol.getValue() )
    symbol.clearValue()
    symbol.setValue( interfaceToHostName.get( event[ "value" ], '' ) )
    print "tcpipNetHostNameUpdate: symbol.getValue()=" + str( symbol.getValue() )

    
def tcpipNetMacAddrUpdate(symbol, event):   
    interfaceToMacAddress = {
        'GMAC':         '00:04:25:1C:A0:02',
        'GMAC0':        '00:04:25:1C:A0:02',
        'GMAC1':        '00:04:25:1C:A0:03',
        'ETH':          '00:04:25:1C:A0:02',
        'EMAC0':        '00:04:25:1C:A0:03',
        'EMAC1':        '00:04:25:1C:A0:04',
        'ENC28J60':     '00:04:a3:12:34:56',
        'LAN865x':      '00:04:25:1C:A0:02',
    }
    print "tcpipNetMacAddrUpdate: symbol.getValue()=" + str( symbol.getValue() )
    tcpipInterfaceName = event["value"]
    symbol.clearValue()
    macAddress = interfaceToMacAddress.get( tcpipInterfaceName, '' )
    if len( macAddress ):
        symbol.setValue( macAddress )
    else:
        if( tcpipInterfaceName == "ETHMAC" ):
            if "DA" in Variables.get("__PROCESSOR"):
                symbol.setValue( "c4:de:39:75:d8:80" )
    print "tcpipNetMacAddrUpdate: symbol.getValue()=" + str( symbol.getValue() )


def tcpipNetIpAddrUpdate(symbol, event):
    tcpipInterfaceName = event[ "value" ]
    interfaceToIpAddress = {
        'ETHMAC':       '192.168.100.10',
        'GMAC':         '192.168.100.11',
        'GMAC0':        '192.168.100.11',
        'GMAC1':        '192.168.100.12',
        'ETH':          '192.168.100.11',
        'ENCX24J600':   '192.168.100.12',
        'ENC28J60':     '192.168.100.13',
        'EMAC0':        '192.168.100.14',
        'EMAC1':        '192.168.100.15',
        'MRF24WN':      '192.168.1.2',
        'WINC':         '192.168.1.2',
        'WINC1500':     '192.168.1.2',
        'WILC1000':     '192.168.1.2',
        'PIC32MZW1':    '192.168.1.2',
        'LAN865x':      '192.168.100.11',
    }
    ipAddress = interfaceToIpAddress.get( tcpipInterfaceName, '0.0.0.0' )
    print "tcpipNetIpAddrUpdate: symbol.getValue()=" + str( symbol.getValue() )
    if(     (tcpipInterfaceName == "MRF24WN")
        or  (tcpipInterfaceName == "WINC1500")
        or  (tcpipInterfaceName == "WILC1000")
        or  (tcpipInterfaceName == "PIC32MZW1")
    ):
        tcpipDhcpc = Database.getSymbolValue( "tcpipDhcp",  "TCPIP_STACK_USE_DHCP_CLIENT" )
        tcpipDhcps = Database.getSymbolValue( "tcpipDhcps", "TCPIP_STACK_USE_DHCP_SERVER" )
        if( (tcpipDhcpc != False) or (tcpipDhcps != False) ):
            ipAddress = '0.0.0.0'
    symbol.clearValue()
    symbol.setValue( ipAddress )
    print "tcpipNetIpAddrUpdate: symbol.getValue()=" + str( symbol.getValue() )


def tcpipNetGatewayIpAddrUpdate(symbol, event): 
    interfaceToIpAddress = {
        'ETHMAC':       '192.168.100.1',
        'GMAC':         '192.168.100.1',
        'GMAC0':        '192.168.100.1',
        'GMAC1':        '192.168.100.1',
        'ETH':          '192.168.100.1',
        'ENCX24J600':   '192.168.100.1',
        'ENC28J60':     '192.168.100.2',
        'EMAC0':        '192.168.100.1',
        'EMAC1':        '192.168.100.2',
        'MRF24WN':      '192.168.1.1',
        'WINC':         '192.168.1.1',
        'WINC1500':     '192.168.1.1',
        'WILC1000':     '192.168.1.1',
        'PIC32MZW1':    '192.168.1.1',
        'LAN865x':      '192.168.100.1',
    }
    print "tcpipNetGatewayIpAddrUpdate: symbol.getValue()=" + str( symbol.getValue() )
    symbol.clearValue()
    symbol.setValue( interfaceToIpAddress.get( event[ "value" ], '0.0.0.0' ) )
    
    print "tcpipNetGatewayIpAddrUpdate: symbol.getValue()=" + str( symbol.getValue() )
    
def tcpipNetPrimDnsIpAddrUpdate(symbol, event): 
    interfaceToIpAddress = {
        'ETHMAC':       '192.168.100.1',
        'GMAC':         '192.168.100.1',
        'GMAC0':        '192.168.100.1',
        'GMAC1':        '192.168.100.1',
        'ETH':          '192.168.100.1',
        'ENCX24J600':   '192.168.100.1',
        'ENC28J60':     '192.168.100.2',
        'EMAC0':        '192.168.100.1',
        'EMAC1':        '192.168.100.2',
        'MRF24WN':      '192.168.1.1',
        'WINC':         '192.168.1.1',
        'WINC1500':     '192.168.1.1',
        'WILC1000':     '192.168.1.1',
        'PIC32MZW1':    '192.168.1.1',
        'LAN865x':      '192.168.100.1',
    }
    print "tcpipNetPrimDnsIpAddrUpdate: symbol.getValue()=" + str( symbol.getValue() )
    symbol.clearValue()
    symbol.setValue( interfaceToIpAddress.get( event[ "value" ], '0.0.0.0' ) )
    print "tcpipNetPrimDnsIpAddrUpdate: symbol.getValue()=" + str( symbol.getValue() )


def tcpipNetMACDrvObjUpdate(symbol, event):
    interfaceToMacObject = {
        'ETHMAC':       'DRV_ETHMAC_PIC32MACObject',
        'GMAC':         'DRV_GMAC_Object',
        'GMAC0':        'DRV_GMAC0_Object',
        'GMAC1':        'DRV_GMAC1_Object',
        'ETH':          'DRV_GMAC_Object',
        'ENCX24J600':   'DRV_ENCX24J600_MACObject',
        'ENC28J60':     'DRV_ENC28J60_MACObject',
        'EMAC0':        'DRV_EMAC0_Object',
        'EMAC1':        'DRV_EMAC1_Object',
        'MRF24WN':      'WDRV_MRF24WN_MACObject',
        'WINC':         'WDRV_WINC_MACObject',
        'WINC1500':     'WDRV_WINC1500_MACObject',
        'WILC1000':     'WDRV_WILC1000_MACObject',
        'PIC32MZW1':    'WDRV_PIC32MZW1_MACObject',
        'LAN865x':      'DRV_LAN865X_MACObject',
        'G3ADPMAC':     'DRV_G3ADP_MACObject',
    }
    print "tcpipNetMACDrvObjUpdate: symbol.getValue()=" + str( symbol.getValue() )
    symbol.clearValue()
    symbol.setValue( interfaceToMacObject.get( event[ "value" ], '' ) )
    print "tcpipNetMACDrvObjUpdate: symbol.getValue()=" + str( symbol.getValue() )

    
############################################################################################################### 
def onAttachmentConnected(source, target):
    if (source["id"] == "NETCONFIG_MAC_Dependency"):
        tcpipNetConfigIndex = int(source["component"].getID().strip("tcpipNetConfig_"))
        macInterface = source["component"].getSymbolByID("TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX"+str(tcpipNetConfigIndex))
        if( macInterface ):
            macInterface.clearValue()
            macInterface.setValue( target["component"].getDisplayName() )
            setVal("tcpipStack", "TCPIP_STACK_NET_INTERFACE_NAME_IDX" + str(tcpipNetConfigIndex), target["component"].getDisplayName())
            
def onAttachmentDisconnected(source, target):
    if (source["id"] == "NETCONFIG_MAC_Dependency"):    
        tcpipNetConfigIndex = int(source["component"].getID().strip("tcpipNetConfig_"))
        source["component"].clearSymbolValue("TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX"+str(tcpipNetConfigIndex))
        setVal("tcpipStack", "TCPIP_STACK_NET_INTERFACE_NAME_IDX" + str(tcpipNetConfigIndex), "")

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
      
    
def destroyComponent(tcpipNetConfigComponent):
    netconfig_interface_counter_dict = {}
    netconfig_interface_counter_dict = Database.sendMessage("tcpipNetConfig", "NETCONFIG_INTERFACE_COUNTER_DEC", netconfig_interface_counter_dict)    
    tcpipNetConfigIndex = int(tcpipNetConfigComponent.getID().strip("tcpipNetConfig_"))       
    if(Database.getSymbolValue("tcpipNetConfig_" + str(tcpipNetConfigIndex), "TCPIP_NETWORK_MACBRIDGE_ADD_IDX" + str(tcpipNetConfigIndex)) == True):
        macbridge_global_config_dict = {}   
        macbridge_global_config_dict = Database.sendMessage("tcpipNetConfig", "NETCONFIG_MAC_BRIDGE_DISABLE", macbridge_global_config_dict) 
