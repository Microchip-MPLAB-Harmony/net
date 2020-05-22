"""*****************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************"""

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
    #tcpipNetHostName.setDependencies(tcpipNetHostNameUpdate, ["TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX"]) 
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
    #tcpipNetIpAddrMask.setDependencies(tcpipNetConfigMenuVisible, [tcpipNetIfName.getID()])

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

    # Network Interface Secondary DNS IP address
    tcpipNetSecDnsIpAddr = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_SECOND_DNS_IDX" + str(index),None)
    tcpipNetSecDnsIpAddr.setLabel("IPv4 Secondary DNS")
    tcpipNetSecDnsIpAddr.setVisible(True)
    tcpipNetSecDnsIpAddr.setDefaultValue("0.0.0.0")
    #tcpipNetSecDnsIpAddr[index].setDependencies(tcpipNetConfigMenuVisible, [tcpipNetConfig[index].getID()])        

    # Network Interface Power Mode
    tcpipNetPwrMode = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_POWER_MODE_IDX" + str(index),None)
    tcpipNetPwrMode.setLabel("Power Mode")
    tcpipNetPwrMode.setVisible(True)
    tcpipNetPwrMode.setDefaultValue("full")
    #tcpipNetPwrMode.setDependencies(tcpipNetConfigMenuVisible, [tcpipNetConfig[index].getID()])
    
    # Network Configuration Start-up Flags Menu
    tcpipNetStartupFlag = tcpipNetConfigComponent.createMenuSymbol("TCPIP_NETWORK_STARTUP_FLAG_IDX" + str(index),None)
    tcpipNetStartupFlag.setLabel("Network Configuration Start-up Flags")
    tcpipNetStartupFlag.setVisible(True)    
    #tcpipNetStartupFlag[index].setDependencies(tcpipNetConfigMenuVisible, [tcpipNetConfig[index].getID()])

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
    #tcpipNetMcastEnable.setDependencies(tcpipNetConfigMenuVisible, [tcpipNetConfig[index].getID()])

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
    
    # Network Interface MAC Driver Object
    tcpipNetMacDrvObj = tcpipNetConfigComponent.createStringSymbol("TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX" + str(index),None)
    tcpipNetMacDrvObj.setLabel("Network MAC Driver")
    tcpipNetMacDrvObj.setVisible(True)
    tcpipNetMacDrvObj.setDefaultValue("")
    tcpipNetMacDrvObj.setDependencies(tcpipNetMACDrvObjUpdate, [tcpipNetIfName.getID()])


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

        
def tcpipNetHostNameUpdate(symbol, event):
    interfaceToHostName = {
        'ETHMAC':       'MCHPBOARD_E',
        'GMAC':         'MCHPBOARD_C',
        'ENCX24J600':   'MCHPENCX24_E',
        'ENC28J60':     'MCHPENC28_E',
        'EMAC0':        'SAM9X60_EK',
        'EMAC1':        'SAM9X60_EK',
        'MRF24WN':      'MCHPBOARD_W',
        'WINC':         'MCHPBOARD_W',
        'WINC1500':     'MCHPBOARD_W',
        'WILC1000':     'MCHPBOARD_W',
    }
    print "tcpipNetHostNameUpdate: symbol.getValue()=" + str( symbol.getValue() )
    symbol.clearValue()
    symbol.setValue( interfaceToHostName.get( event[ "value" ], '' ) )
    print "tcpipNetHostNameUpdate: symbol.getValue()=" + str( symbol.getValue() )

    
def tcpipNetMacAddrUpdate(symbol, event):   
    interfaceToMacAddress = {
        'GMAC':         '00:04:25:1C:A0:02',
        'EMAC0':        '00:04:25:1C:A0:03',
        'EMAC1':        '00:04:25:1C:A0:04',
        'ENC28J60':     '00:04:a3:12:34:56',
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
        'ENCX24J600':   '192.168.100.12',
        'ENC28J60':     '192.168.100.13',
        'EMAC0':        '192.168.100.14',
        'EMAC1':        '192.168.100.15',
        'MRF24WN':      '192.168.1.2',
        'WINC':         '192.168.1.2',
        'WINC1500':     '192.168.1.2',
        'WILC1000':     '192.168.1.2',
    }
    ipAddress = interfaceToIpAddress.get( tcpipInterfaceName, '0.0.0.0' )
    print "tcpipNetIpAddrUpdate: symbol.getValue()=" + str( symbol.getValue() )
    if(     (tcpipInterfaceName == "MRF24WN")
        or  (tcpipInterfaceName == "WINC1500")
        or  (tcpipInterfaceName == "WILC1000")
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
        'ENCX24J600':   '192.168.100.1',
        'ENC28J60':     '192.168.100.2',
        'EMAC0':        '192.168.100.1',
        'EMAC1':        '192.168.100.2',
        'MRF24WN':      '192.168.1.1',
        'WINC':         '192.168.1.1',
        'WINC1500':     '192.168.1.1',
        'WILC1000':     '192.168.1.1',
    }
    print "tcpipNetGatewayIpAddrUpdate: symbol.getValue()=" + str( symbol.getValue() )
    symbol.clearValue()
    symbol.setValue( interfaceToIpAddress.get( event[ "value" ], '0.0.0.0' ) )
    
    print "tcpipNetGatewayIpAddrUpdate: symbol.getValue()=" + str( symbol.getValue() )
    
def tcpipNetPrimDnsIpAddrUpdate(symbol, event): 
    interfaceToIpAddress = {
        'ETHMAC':       '192.168.100.1',
        'GMAC':         '192.168.100.1',
        'ENCX24J600':   '192.168.100.1',
        'ENC28J60':     '192.168.100.2',
        'EMAC0':        '192.168.100.1',
        'EMAC1':        '192.168.100.2',
        'MRF24WN':      '192.168.1.1',
        'WINC':         '192.168.1.1',
        'WINC1500':     '192.168.1.1',
        'WILC1000':     '192.168.1.1',
    }
    print "tcpipNetPrimDnsIpAddrUpdate: symbol.getValue()=" + str( symbol.getValue() )
    symbol.clearValue()
    symbol.setValue( interfaceToIpAddress.get( event[ "value" ], '0.0.0.0' ) )
    print "tcpipNetPrimDnsIpAddrUpdate: symbol.getValue()=" + str( symbol.getValue() )


def tcpipNetMACDrvObjUpdate(symbol, event):
    interfaceToMacObject = {
        'ETHMAC':       'DRV_ETHMAC_PIC32MACObject',
        'GMAC':         'DRV_GMAC_Object',
        'ENCX24J600':   'DRV_ENCX24J600_MACObject',
        'ENC28J60':     'DRV_ENC28J60_MACObject',
        'EMAC0':        'DRV_EMAC0_Object',
        'EMAC1':        'DRV_EMAC1_Object',
        'MRF24WN':      'WDRV_MRF24WN_MACObject',
        'WINC':         'WDRV_WINC_MACObject',
        'WINC1500':     'WDRV_WINC1500_MACObject',
        'WILC1000':     'WDRV_WILC1000_MACObject',
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

def onAttachmentDisconnected(source, target):
    if (source["id"] == "NETCONFIG_MAC_Dependency"):    
        tcpipNetConfigIndex = int(source["component"].getID().strip("tcpipNetConfig_"))
        source["component"].clearSymbolValue("TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX"+str(tcpipNetConfigIndex))
  
    
def destroyComponent(tcpipNetConfigComponent):
    netconfig_interface_counter_dict = {}
    netconfig_interface_counter_dict = Database.sendMessage("tcpipNetConfig", "NETCONFIG_INTERFACE_COUNTER_DEC", netconfig_interface_counter_dict)
