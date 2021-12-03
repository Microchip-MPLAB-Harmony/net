"""*****************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
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

TCPIP_DHCPV6_DUID_TYPE = ["TCPIP_DHCPV6_DUID_TYPE_LL","TCPIP_DHCPV6_DUID_TYPE_LLT","TCPIP_DHCPV6_DUID_TYPE_EN"]

def instantiateComponent(tcpipDhcpv6Component):
    print("TCPIP DHCPv6 Client Component")
    configName = Variables.get("__CONFIGURATION_NAME")

    # Enable DHCPV6 Client
    tcpipDhcpcv6 = tcpipDhcpv6Component.createBooleanSymbol("TCPIP_STACK_USE_DHCPV6_CLIENT", None)
    tcpipDhcpcv6.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpcv6.setLabel("Enable Client")
    tcpipDhcpcv6.setVisible(False)
    tcpipDhcpcv6.setDescription("Enable DHCPv6 Client")
    tcpipDhcpcv6.setDefaultValue(True)
    
    # DHCP Unique Indentifier Type (DUID)
    tcpipDhcpClientV6DUIDTypeConfig= tcpipDhcpv6Component.createComboSymbol("TCPIP_DHCPV6_DUID_TYPE_CONFIG", None, TCPIP_DHCPV6_DUID_TYPE)
    tcpipDhcpClientV6DUIDTypeConfig.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6DUIDTypeConfig.setLabel("Select DHCP Unique Identifier Type")
    tcpipDhcpClientV6DUIDTypeConfig.setVisible(True)
    tcpipDhcpClientV6DUIDTypeConfig.setDescription("DHCP Unique Identity Type Selection")
    tcpipDhcpClientV6DUIDTypeConfig.setDefaultValue("TCPIP_DHCPV6_DUID_TYPE_LL")

    # minimum size for a DHCPv6 UDP buffer
    tcpipDhcpClientV6udpTxBufferSize = tcpipDhcpv6Component.createIntegerSymbol("TCPIP_DHCPV6_MIN_UDP_TX_BUFFER_SIZE", None)
    tcpipDhcpClientV6udpTxBufferSize.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6udpTxBufferSize.setLabel("Minimum UDP Tx Buffer Size (in bytes)") 
    tcpipDhcpClientV6udpTxBufferSize.setVisible(True)
    tcpipDhcpClientV6udpTxBufferSize.setDefaultValue(512)
    tcpipDhcpClientV6udpTxBufferSize.setReadOnly(True)
    tcpipDhcpClientV6udpTxBufferSize.setDescription("DHCPv6 Client UDP Tx buffer Size (in bytes)")
    
    # DHCP v6 client maximum Message buffer size
    tcpipDhcpClientV6MessageBufferSize = tcpipDhcpv6Component.createIntegerSymbol("TCPIP_DHCPV6_MESSAGE_BUFFER_SIZE", None)
    tcpipDhcpClientV6MessageBufferSize.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6MessageBufferSize.setLabel("Maximum Message Buffer Size")
    tcpipDhcpClientV6MessageBufferSize.setVisible(True)
    tcpipDhcpClientV6MessageBufferSize.setDescription("Maximum Message Buffer Size")
    tcpipDhcpClientV6MessageBufferSize.setDefaultValue(512)
    
    # DHCP v6 client maximum Number Message buffer
    tcpipDhcpClientV6MessageBufferNo = tcpipDhcpv6Component.createIntegerSymbol("TCPIP_DHCPV6_MESSAGE_BUFFERS", None)
    tcpipDhcpClientV6MessageBufferNo.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6MessageBufferNo.setLabel("Maximum Number of Message Buffers")
    tcpipDhcpClientV6MessageBufferNo.setVisible(True)
    tcpipDhcpClientV6MessageBufferNo.setDescription("Maximum Number of Message Buffers")
    tcpipDhcpClientV6MessageBufferNo.setDefaultValue(8)
    
    # Enable DHCP Client Console Command
    tcpipDhcpClientV6CmdEnable = tcpipDhcpv6Component.createBooleanSymbol("TCPIP_DHCPV6_CLIENT_CONSOLE_CMD", None)
    tcpipDhcpClientV6CmdEnable.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6CmdEnable.setLabel("Enable Console Commands")
    tcpipDhcpClientV6CmdEnable.setVisible(True)
    tcpipDhcpClientV6CmdEnable.setDescription("Enable DHCPv6 Client Console Commands")
    tcpipDhcpClientV6CmdEnable.setDefaultValue(True)
    
    # Advanced Settings
    tcpipDhcpClientV6AdvSettings = tcpipDhcpv6Component.createMenuSymbol("TCPIP_DHCPV6_ADV_SETTING", None)
    tcpipDhcpClientV6AdvSettings.setLabel("Advanced Settings")
    tcpipDhcpClientV6AdvSettings.setDescription("Advanced Settings")
    tcpipDhcpClientV6AdvSettings.setVisible(True)

    # DHCPv6  Startup Configuration Flags Settings
    tcpipDhcpClientV6StartupConfigFlag = tcpipDhcpv6Component.createMenuSymbol("TCPIP_DHCPV6_CONFIG_STARTUP_FLAG", tcpipDhcpClientV6AdvSettings)
    tcpipDhcpClientV6StartupConfigFlag.setLabel("Startup Configuration Flags")
    tcpipDhcpClientV6StartupConfigFlag.setVisible(True)
    tcpipDhcpClientV6StartupConfigFlag.setDescription("DHCPv6 Client Startup Configuration Flags Settings")

    # Enable DHCPv6 at stack start up
    tcpipDhcpClientv6EnableStackStart = tcpipDhcpv6Component.createBooleanSymbol("TCPIP_DHCPV6_FLAG_STACK_START_ENABLE", tcpipDhcpClientV6StartupConfigFlag)
    tcpipDhcpClientv6EnableStackStart.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientv6EnableStackStart.setLabel("Enable DHCPv6 Client")
    tcpipDhcpClientv6EnableStackStart.setVisible(True)
    tcpipDhcpClientv6EnableStackStart.setDescription("Enable DHCPv6 at stack start up")
    tcpipDhcpClientv6EnableStackStart.setDefaultValue(True)
    
    # Disable DAD processing 
    tcpipDhcpClientv6DisableDAD = tcpipDhcpv6Component.createBooleanSymbol("TCPIP_DHCPV6_FLAG_DAD_DISABLE", tcpipDhcpClientV6StartupConfigFlag)
    tcpipDhcpClientv6DisableDAD.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientv6DisableDAD.setLabel("Skip DAD Processing")
    tcpipDhcpClientv6DisableDAD.setVisible(True)
    tcpipDhcpClientv6DisableDAD.setDescription("Skip DAD process if selected and allow if not selected.")
    tcpipDhcpClientv6DisableDAD.setDefaultValue(False)

    # Ignore Renew Life Time if selected
    tcpipDhcpClientv6IgnoreRenewLFtime = tcpipDhcpv6Component.createBooleanSymbol("TCPIP_DHCPV6_FLAG_IA_IGNORE_RENEW_LTIME", tcpipDhcpClientV6StartupConfigFlag)
    tcpipDhcpClientv6IgnoreRenewLFtime.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientv6IgnoreRenewLFtime.setLabel("Ignore Renew Life Time")
    tcpipDhcpClientv6IgnoreRenewLFtime.setVisible(True)
    tcpipDhcpClientv6IgnoreRenewLFtime.setDescription("Ignore Renew Life Time if selected and allow if not selected.")
    tcpipDhcpClientv6IgnoreRenewLFtime.setDefaultValue(False)

    # Ignore Renew Life Time if selected
    tcpipDhcpClientv6IgnoreRebindLFtime = tcpipDhcpv6Component.createBooleanSymbol("TCPIP_DHCPV6_FLAG_IA_IGNORE_REBIND_LTIME", tcpipDhcpClientV6StartupConfigFlag)
    tcpipDhcpClientv6IgnoreRebindLFtime.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientv6IgnoreRebindLFtime.setLabel("Ignore Rebind Life Time")
    tcpipDhcpClientv6IgnoreRebindLFtime.setVisible(True)
    tcpipDhcpClientv6IgnoreRebindLFtime.setDescription("Ignore Rebind Life Time if selected and allow if not selected.")
    tcpipDhcpClientv6IgnoreRebindLFtime.setDefaultValue(False)

    # Ignore Renew Life Time if selected
    tcpipDhcpClientv6NotifySubState = tcpipDhcpv6Component.createBooleanSymbol("TCPIP_DHCPV6_FLAG_IA_NOTIFY_SUB_STATE", tcpipDhcpClientV6StartupConfigFlag)
    tcpipDhcpClientv6NotifySubState.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientv6NotifySubState.setLabel("IA Notification Substate Change")
    tcpipDhcpClientv6NotifySubState.setVisible(True)
    tcpipDhcpClientv6NotifySubState.setDescription("IA Notification for Substate if selected and Notification for IA state if not selected")
    tcpipDhcpClientv6NotifySubState.setDefaultValue(False)

    #DHCP v6 client Identity association for Non Temporary Association
    tcpipDHCPv6ClientIANASettings = tcpipDhcpv6Component.createMenuSymbol("TCPIP_DHCPV6_IANA_SETTING", tcpipDhcpClientV6AdvSettings)
    tcpipDHCPv6ClientIANASettings.setLabel("Client IANA Settings")
    tcpipDHCPv6ClientIANASettings.setDescription("DHCPv6 Client IANA Settings")
    tcpipDHCPv6ClientIANASettings.setVisible(True)

    # DHCP v6 client  maximum number of IANA descriptors 
    tcpipDhcpClientV6IANADescriptor = tcpipDhcpv6Component.createIntegerSymbol("TCPIP_DHCPV6_IANA_DESCRIPTORS_NO", tcpipDHCPv6ClientIANASettings)
    tcpipDhcpClientV6IANADescriptor.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6IANADescriptor.setLabel("Maximum IANA Descriptors")
    tcpipDhcpClientV6IANADescriptor.setVisible(True)
    tcpipDhcpClientV6IANADescriptor.setDescription("Maximum IANA Descriptors")
    tcpipDhcpClientV6IANADescriptor.setDefaultValue(4)

    # DHCP v6 client IANA T1 Time interval in seconds
    tcpipDhcpClientV6IANATimeIntervalT1 = tcpipDhcpv6Component.createIntegerSymbol("TCPIP_DHCPV6_IANA_DEFAULT_T1", tcpipDHCPv6ClientIANASettings)
    tcpipDhcpClientV6IANATimeIntervalT1.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6IANATimeIntervalT1.setLabel("T1 Time Interval(in secs) ")
    tcpipDhcpClientV6IANATimeIntervalT1.setVisible(True)
    tcpipDhcpClientV6IANATimeIntervalT1.setDescription("Time Interval after which Client contacts Server")
    tcpipDhcpClientV6IANATimeIntervalT1.setDefaultValue(0)

    # DHCP v6 client IANA T2 Time interval in seconds
    tcpipDhcpClientV6IANATimeIntervalT2 = tcpipDhcpv6Component.createIntegerSymbol("TCPIP_DHCPV6_IANA_DEFAULT_T2", tcpipDHCPv6ClientIANASettings)
    tcpipDhcpClientV6IANATimeIntervalT2.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6IANATimeIntervalT2.setLabel("T2 Time Interval (in secs) ")
    tcpipDhcpClientV6IANATimeIntervalT2.setVisible(True)
    tcpipDhcpClientV6IANATimeIntervalT2.setDescription("Time Interval after which Client contacts any available Server")
    tcpipDhcpClientV6IANATimeIntervalT2.setDefaultValue(0)
    
    # DHCP v6 client IANA Solicit T1 Time interval in seconds
    tcpipDhcpClientV6IANASolicitTimeIntervalT1 = tcpipDhcpv6Component.createIntegerSymbol("TCPIP_DHCPV6_IANA_SOLICIT_T1", tcpipDHCPv6ClientIANASettings)
    tcpipDhcpClientV6IANASolicitTimeIntervalT1.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6IANASolicitTimeIntervalT1.setLabel("Solicit T1  Time Interval(in secs) ")
    tcpipDhcpClientV6IANASolicitTimeIntervalT1.setVisible(True)
    tcpipDhcpClientV6IANASolicitTimeIntervalT1.setDescription("Time Interval for Solicit message")
    tcpipDhcpClientV6IANASolicitTimeIntervalT1.setDefaultValue(0)
    
    # DHCP v6 client IANA Solicit T2 Time interval in seconds
    tcpipDhcpClientV6IANASolicitTimeIntervalT2 = tcpipDhcpv6Component.createIntegerSymbol("TCPIP_DHCPV6_IANA_SOLICIT_T2", tcpipDHCPv6ClientIANASettings)
    tcpipDhcpClientV6IANASolicitTimeIntervalT2.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6IANASolicitTimeIntervalT2.setLabel("Solicit T2 Time Interval (in secs) ")
    tcpipDhcpClientV6IANASolicitTimeIntervalT2.setVisible(True)
    tcpipDhcpClientV6IANASolicitTimeIntervalT2.setDescription("Time Interval for Solicit message")
    tcpipDhcpClientV6IANASolicitTimeIntervalT2.setDefaultValue(0)
    
    # DHCP v6 Number of IANA Solicit Address No
    tcpipDhcpClientV6IANASolicitAddressNo = tcpipDhcpv6Component.createIntegerSymbol("TCPIP_DHCPV6_IANA_SOLICIT_ADDRESSES_NO", tcpipDHCPv6ClientIANASettings)
    tcpipDhcpClientV6IANASolicitAddressNo.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6IANASolicitAddressNo.setLabel("Maximum IANA Solicit address")
    tcpipDhcpClientV6IANASolicitAddressNo.setVisible(True)
    tcpipDhcpClientV6IANASolicitAddressNo.setDescription("Maximum IANA Solicit address")
    tcpipDhcpClientV6IANASolicitAddressNo.setDefaultValue(0)
    
    # DHCP v6 Number of IANA Solicit Default Address 
    tcpipDhcpClientV6IANASolicitAddress = tcpipDhcpv6Component.createStringSymbol("TCPIP_DHCPV6_IANA_SOLICIT_DEFAULT_ADDRESS", tcpipDHCPv6ClientIANASettings)
    tcpipDhcpClientV6IANASolicitAddress.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6IANASolicitAddress.setLabel("IANA Solicit address")
    tcpipDhcpClientV6IANASolicitAddress.setVisible(True)
    tcpipDhcpClientV6IANASolicitAddress.setDescription("IANA Solicit address")
    tcpipDhcpClientV6IANASolicitAddress.setDefaultValue("::0")
    
    #DHCP v6 client Identity association for Temporary Association
    tcpipDHCPv6ClientIATASettings = tcpipDhcpv6Component.createMenuSymbol("TCPIP_DHCPV6_IATA_SETTING", tcpipDhcpClientV6AdvSettings)
    tcpipDHCPv6ClientIATASettings.setLabel("Client IATA Settings")
    tcpipDHCPv6ClientIATASettings.setDescription("DHCPv6 Client IATA Settings")
    tcpipDHCPv6ClientIATASettings.setVisible(True)
    
    # DHCP v6 client  maximum number of IATA descriptors 
    tcpipDhcpClientV6IATADescriptor = tcpipDhcpv6Component.createIntegerSymbol("TCPIP_DHCPV6_IATA_DESCRIPTORS_NO", tcpipDHCPv6ClientIATASettings)
    tcpipDhcpClientV6IATADescriptor.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6IATADescriptor.setLabel("Maximum IATA Descriptors")
    tcpipDhcpClientV6IATADescriptor.setVisible(True)
    tcpipDhcpClientV6IATADescriptor.setDescription("Maximum IATA Descriptors")
    tcpipDhcpClientV6IATADescriptor.setDefaultValue(2)
    
    # DHCP v6 client IATA T1 Time interval in seconds
    tcpipDhcpClientV6IATATimeIntervalT1 = tcpipDhcpv6Component.createIntegerSymbol("TCPIP_DHCPV6_IATA_DEFAULT_T1", tcpipDHCPv6ClientIATASettings)
    tcpipDhcpClientV6IATATimeIntervalT1.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6IATATimeIntervalT1.setLabel("T1 Time Interval(in secs) ")
    tcpipDhcpClientV6IATATimeIntervalT1.setVisible(True)
    tcpipDhcpClientV6IATATimeIntervalT1.setDescription("Time Interval after which Client contacts Server")
    tcpipDhcpClientV6IATATimeIntervalT1.setDefaultValue(0)
    
    # DHCP v6 client IATA T2 Time interval in seconds
    tcpipDhcpClientV6IATATimeIntervalT2 = tcpipDhcpv6Component.createIntegerSymbol("TCPIP_DHCPV6_IATA_DEFAULT_T2", tcpipDHCPv6ClientIATASettings)
    tcpipDhcpClientV6IATATimeIntervalT2.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6IATATimeIntervalT2.setLabel("T2 Time Interval (in secs) ")
    tcpipDhcpClientV6IATATimeIntervalT2.setVisible(True)
    tcpipDhcpClientV6IATATimeIntervalT2.setDescription("Time Interval after which Client contacts any available Server")
    tcpipDhcpClientV6IATATimeIntervalT2.setDefaultValue(0)
    
    # DHCP v6 Number of IATA Solicit Address No
    tcpipDhcpClientV6IATASolicitAddressNo = tcpipDhcpv6Component.createIntegerSymbol("TCPIP_DHCPV6_IATA_SOLICIT_ADDRESSES_NO", tcpipDHCPv6ClientIATASettings)
    tcpipDhcpClientV6IATASolicitAddressNo.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6IATASolicitAddressNo.setLabel("Maximum IATA Solicit address")
    tcpipDhcpClientV6IATASolicitAddressNo.setVisible(True)
    tcpipDhcpClientV6IATASolicitAddressNo.setDescription("Maximum IATA Solicit address")
    tcpipDhcpClientV6IATASolicitAddressNo.setDefaultValue(0)
    
    # DHCP v6 Number of IATA Solicit Default Address 
    tcpipDhcpClientV6IATASolicitAddress = tcpipDhcpv6Component.createStringSymbol("TCPIP_DHCPV6_IATA_SOLICIT_DEFAULT_ADDRESS", tcpipDHCPv6ClientIATASettings)
    tcpipDhcpClientV6IATASolicitAddress.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6IATASolicitAddress.setLabel("IATA Solicit address")
    tcpipDhcpClientV6IATASolicitAddress.setVisible(True)
    tcpipDhcpClientV6IATASolicitAddress.setDescription("IATA Solicit address")
    tcpipDhcpClientV6IATASolicitAddress.setDefaultValue("::0")
        
    # DHCP v6 client  Free IA descriptors per client
    tcpipDhcpClientV6IADescriptor = tcpipDhcpv6Component.createIntegerSymbol("TCPIP_DHCPV6_IA_FREE_DESCRIPTORS_NO", tcpipDhcpClientV6AdvSettings)
    tcpipDhcpClientV6IADescriptor.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6IADescriptor.setLabel("Maximum IA Free Descriptors")
    tcpipDhcpClientV6IADescriptor.setVisible(True)
    tcpipDhcpClientV6IADescriptor.setDescription("Maximum IA Free Descriptors")
    tcpipDhcpClientV6IADescriptor.setDefaultValue(2)
    
    # DHCP v6 client IA prefered Life Time interval in seconds
    tcpipDhcpClientV6IAPrefTimeInterval = tcpipDhcpv6Component.createIntegerSymbol("TCPIP_DHCPV6_IA_SOLICIT_ADDRESS_PREF_LTIME", tcpipDhcpClientV6AdvSettings)
    tcpipDhcpClientV6IAPrefTimeInterval.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6IAPrefTimeInterval.setLabel("IA Preferred Time Interval(in secs) ")
    tcpipDhcpClientV6IAPrefTimeInterval.setVisible(True)
    tcpipDhcpClientV6IAPrefTimeInterval.setDescription("IA Preferred Time For the Solicit Address")
    tcpipDhcpClientV6IAPrefTimeInterval.setDefaultValue(0)
    
    # DHCP v6 client IA Valid Life Time interval in seconds
    tcpipDhcpClientV6IAValidTimeInterval = tcpipDhcpv6Component.createIntegerSymbol("TCPIP_DHCPV6_IA_SOLICIT_ADDRESS_VALID_LTIME", tcpipDhcpClientV6AdvSettings)
    tcpipDhcpClientV6IAValidTimeInterval.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6IAValidTimeInterval.setLabel("IA valid Time Interval (in secs)")
    tcpipDhcpClientV6IAValidTimeInterval.setVisible(True)
    tcpipDhcpClientV6IAValidTimeInterval.setDescription("IA Valid Time For the Solicit Address")
    tcpipDhcpClientV6IAValidTimeInterval.setDefaultValue(0)
    
    # DHCP v6 client Number of characters for a server status
    tcpipDhcpClientV6ServerStatusMessagelen = tcpipDhcpv6Component.createIntegerSymbol("TCPIP_DHCPV6_STATUS_CODE_MESSAGE_LEN", tcpipDhcpClientV6AdvSettings)
    tcpipDhcpClientV6ServerStatusMessagelen.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6ServerStatusMessagelen.setLabel("Maximum Length for Server Status message")
    tcpipDhcpClientV6ServerStatusMessagelen.setVisible(True)
    tcpipDhcpClientV6ServerStatusMessagelen.setDescription("Maximum Length for Server Status message")
    tcpipDhcpClientV6ServerStatusMessagelen.setDefaultValue(0)
    
    # DHCP v6 client Number of DNS servers to store
    tcpipDhcpClientV6DNSServerNumber = tcpipDhcpv6Component.createIntegerSymbol("TCPIP_DHCPV6_DNS_SERVERS_NO", tcpipDhcpClientV6AdvSettings)
    tcpipDhcpClientV6DNSServerNumber.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6DNSServerNumber.setLabel("Maximum Number of DNS Server Stored")
    tcpipDhcpClientV6DNSServerNumber.setVisible(True)
    tcpipDhcpClientV6DNSServerNumber.setDescription("Maximum Number of DNS Server Stored from DHCP Server")
    tcpipDhcpClientV6DNSServerNumber.setDefaultValue(2)
    
    # DHCP v6 client space for Domain Search List option
    tcpipDhcpClientV6DomainSerachListSize = tcpipDhcpv6Component.createIntegerSymbol("TCPIP_DHCPV6_DOMAIN_SEARCH_LIST_SIZE", tcpipDhcpClientV6AdvSettings)
    tcpipDhcpClientV6DomainSerachListSize.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6DomainSerachListSize.setLabel("Maximum Size Domain Search List")
    tcpipDhcpClientV6DomainSerachListSize.setVisible(True)
    tcpipDhcpClientV6DomainSerachListSize.setDescription("Maximum Size Domain Search List option multiple of 16")
    tcpipDhcpClientV6DomainSerachListSize.setDefaultValue(64)
    
    # DHCP v6 client preference value for server selection
    tcpipDhcpClientV6ForcedServerSelection = tcpipDhcpv6Component.createIntegerSymbol("TCPIP_DHCPV6_FORCED_SERVER_PREFERENCE", tcpipDhcpClientV6AdvSettings)
    tcpipDhcpClientV6ForcedServerSelection.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6ForcedServerSelection.setLabel("Maximum Prefered Server Selection")
    tcpipDhcpClientV6ForcedServerSelection.setVisible(True)
    tcpipDhcpClientV6ForcedServerSelection.setDescription("Maximum Server Selection 8 bit value")
    tcpipDhcpClientV6ForcedServerSelection.setDefaultValue(255)
        
    # DHCPV6 Client task Task Rate in msec
    tcpipDhcpClientv6TickRate = tcpipDhcpv6Component.createIntegerSymbol("TCPIP_DHCPV6_TASK_TICK_RATE", tcpipDhcpClientV6AdvSettings)
    tcpipDhcpClientv6TickRate.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientv6TickRate.setLabel("Client Task Rate (in msec)")
    tcpipDhcpClientv6TickRate.setVisible(True)
    tcpipDhcpClientv6TickRate.setDescription("DHCPv6 Task Rate in msec")
    tcpipDhcpClientv6TickRate.setDefaultValue(100)
    
    # DHCP v6 Client port
    tcpipDhcpClientV6ConnectPort = tcpipDhcpv6Component.createIntegerSymbol("TCPIP_DHCPV6_CLIENT_CONNECT_PORT", tcpipDhcpClientV6AdvSettings)
    tcpipDhcpClientV6ConnectPort.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6ConnectPort.setLabel("Client UDP Connection Port")
    tcpipDhcpClientV6ConnectPort.setVisible(True)
    tcpipDhcpClientV6ConnectPort.setDescription("Client Port for DHCP v6 Client Transactions")
    tcpipDhcpClientV6ConnectPort.setDefaultValue(546)

    # Remote Server Port for DHCP V6 Server Messages
    tcpipDhcpClientV6ServerListenPort= tcpipDhcpv6Component.createIntegerSymbol("TCPIP_DHCPV6_SERVER_LISTEN_PORT", tcpipDhcpClientV6AdvSettings)
    tcpipDhcpClientV6ServerListenPort.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6ServerListenPort.setLabel("Remote Server UDP Listening Port")
    tcpipDhcpClientV6ServerListenPort.setVisible(True)
    tcpipDhcpClientV6ServerListenPort.setDescription("Remote Server Port for DHCP v6 Server Messages")
    tcpipDhcpClientV6ServerListenPort.setDefaultValue(547)

    # DHCP V6 Client user notification process
    tcpipDhcpClientV6UserNotificationProcess= tcpipDhcpv6Component.createBooleanSymbol("TCPIP_DHCPV6_USER_NOTIFICATION", tcpipDhcpClientV6AdvSettings)
    tcpipDhcpClientV6UserNotificationProcess.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6UserNotificationProcess.setLabel("Enable User Notification")
    tcpipDhcpClientV6UserNotificationProcess.setVisible(True)
    tcpipDhcpClientV6UserNotificationProcess.setDescription("DHCPv6 Client Enable user Notification Process")
    tcpipDhcpClientV6UserNotificationProcess.setDefaultValue(False)
    
    #DHCPv6 Client Debug Levels
    tcpipDhcpClientV6DebugLevel = tcpipDhcpv6Component.createBooleanSymbol("TCPIP_DHCPV6_CONFIG_DEBUG_LEVEL", tcpipDhcpClientV6AdvSettings)
    tcpipDhcpClientV6DebugLevel.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6DebugLevel.setLabel("Debug Level Configuration Flags")
    tcpipDhcpClientV6DebugLevel.setVisible(False)
    tcpipDhcpClientV6DebugLevel.setDescription("DHCPv6 Client Debug Level Configuration Flags")
    tcpipDhcpClientV6DebugLevel.setDefaultValue(False)

    #Enable DHCPv6 Basic Debugging
    tcpipDhcpClientV6BasicDebugging = tcpipDhcpv6Component.createBooleanSymbol("TCPIP_DHCPV6_DEBUG_MASK_BASIC", tcpipDhcpClientV6DebugLevel)
    tcpipDhcpClientV6BasicDebugging.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientV6BasicDebugging.setLabel("Enable Basic Debugging Level")
    tcpipDhcpClientV6BasicDebugging.setVisible(True)
    tcpipDhcpClientV6BasicDebugging.setDescription("Enable DHCPv6 Basic Debugging Level")
    tcpipDhcpClientV6BasicDebugging.setDefaultValue(True)
    tcpipDhcpClientV6BasicDebugging.setDependencies(tcpipDhcpv6MenuVisible, ["TCPIP_DHCPV6_CONFIG_DEBUG_LEVEL"])
    
    #Enable DHCPv6 Report IN messages 
    tcpipDhcpClientv6DebugINReport = tcpipDhcpv6Component.createBooleanSymbol("TCPIP_DHCPV6_DEBUG_MASK_IN", tcpipDhcpClientV6DebugLevel)
    tcpipDhcpClientv6DebugINReport.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientv6DebugINReport.setLabel("Report Debug IN Messages")
    tcpipDhcpClientv6DebugINReport.setVisible(True)
    tcpipDhcpClientv6DebugINReport.setDescription("Report Debug IN Messages")
    tcpipDhcpClientv6DebugINReport.setDefaultValue(False)
    tcpipDhcpClientv6DebugINReport.setDependencies(tcpipDhcpv6MenuVisible, ["TCPIP_DHCPV6_CONFIG_DEBUG_LEVEL"])
    
    #Enable DHCPv6 Report OUT messages 
    tcpipDhcpClientv6DebugOUTReport = tcpipDhcpv6Component.createBooleanSymbol("TCPIP_DHCPV6_DEBUG_MASK_OUT", tcpipDhcpClientV6DebugLevel)
    tcpipDhcpClientv6DebugOUTReport.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientv6DebugOUTReport.setLabel("Report Debug OUT Messages")
    tcpipDhcpClientv6DebugOUTReport.setVisible(True)
    tcpipDhcpClientv6DebugOUTReport.setDescription("Report Debug OUT Messages")
    tcpipDhcpClientv6DebugOUTReport.setDefaultValue(False)
    tcpipDhcpClientv6DebugOUTReport.setDependencies(tcpipDhcpv6MenuVisible, ["TCPIP_DHCPV6_CONFIG_DEBUG_LEVEL"])
    
    #Enable DHCPv6 client Debug state changes 
    tcpipDhcpClientv6DebugStateChange = tcpipDhcpv6Component.createBooleanSymbol("TCPIP_DHCPV6_DEBUG_MASK_CLIENT_STATE", tcpipDhcpClientV6DebugLevel)
    tcpipDhcpClientv6DebugStateChange.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientv6DebugStateChange.setLabel("State Change Messages")
    tcpipDhcpClientv6DebugStateChange.setVisible(True)
    tcpipDhcpClientv6DebugStateChange.setDescription("State Change Messages")
    tcpipDhcpClientv6DebugStateChange.setDefaultValue(False)
    tcpipDhcpClientv6DebugStateChange.setDependencies(tcpipDhcpv6MenuVisible, ["TCPIP_DHCPV6_CONFIG_DEBUG_LEVEL"])
        
    #Enable DHCPv6 client Debug Report on statistics
    tcpipDhcpClientv6DebugBufStat = tcpipDhcpv6Component.createBooleanSymbol("TCPIP_DHCPV6_DEBUG_MASK_CLIENT_STATS", tcpipDhcpClientV6DebugLevel)
    tcpipDhcpClientv6DebugBufStat.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientv6DebugBufStat.setLabel("Report On Statistics")
    tcpipDhcpClientv6DebugBufStat.setVisible(True)
    tcpipDhcpClientv6DebugBufStat.setDescription("Report Statistics: Buffers Lists and Statistics Counters")
    tcpipDhcpClientv6DebugBufStat.setDefaultValue(False)
    tcpipDhcpClientv6DebugBufStat.setDependencies(tcpipDhcpv6MenuVisible, ["TCPIP_DHCPV6_CONFIG_DEBUG_LEVEL"])
    
    #Enable DHCPv6 Report client state when user notification is made 
    tcpipDhcpClientv6DebugReportNotificationClientState = tcpipDhcpv6Component.createBooleanSymbol("TCPIP_DHCPV6_DEBUG_MASK_CLIENT_NOTIFY_STATE", tcpipDhcpClientV6DebugLevel)
    tcpipDhcpClientv6DebugReportNotificationClientState.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientv6DebugReportNotificationClientState.setLabel("Report Client State On User Notification")
    tcpipDhcpClientv6DebugReportNotificationClientState.setVisible(True)
    tcpipDhcpClientv6DebugReportNotificationClientState.setDescription("Report Client State On User Notification")
    tcpipDhcpClientv6DebugReportNotificationClientState.setDefaultValue(False)
    tcpipDhcpClientv6DebugReportNotificationClientState.setDependencies(tcpipDhcpv6MenuVisible, ["TCPIP_DHCPV6_CONFIG_DEBUG_LEVEL"])
    
    #Enable DHCPv6 Report Current IA states 
    tcpipDhcpClientv6DebugIAState = tcpipDhcpv6Component.createBooleanSymbol("TCPIP_DHCPV6_DEBUG_MASK_IA_STATE", tcpipDhcpClientV6DebugLevel)
    tcpipDhcpClientv6DebugIAState.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientv6DebugIAState.setLabel("Report IA State")
    tcpipDhcpClientv6DebugIAState.setVisible(True)
    tcpipDhcpClientv6DebugIAState.setDescription("Report IA State")
    tcpipDhcpClientv6DebugIAState.setDefaultValue(False)
    tcpipDhcpClientv6DebugIAState.setDependencies(tcpipDhcpv6MenuVisible, ["TCPIP_DHCPV6_CONFIG_DEBUG_LEVEL"])
    
    #Enable DHCPv6 Current IA Sub States 
    tcpipDhcpClientv6DebugIASubState = tcpipDhcpv6Component.createBooleanSymbol("TCPIP_DHCPV6_DEBUG_MASK_IA_SUBSTATE", tcpipDhcpClientV6DebugLevel)
    tcpipDhcpClientv6DebugIASubState.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientv6DebugIASubState.setLabel("Report IA Sub State")
    tcpipDhcpClientv6DebugIASubState.setVisible(True)
    tcpipDhcpClientv6DebugIASubState.setDescription("Report IA Sub State")
    tcpipDhcpClientv6DebugIASubState.setDefaultValue(False)
    tcpipDhcpClientv6DebugIASubState.setDependencies(tcpipDhcpv6MenuVisible, ["TCPIP_DHCPV6_CONFIG_DEBUG_LEVEL"])
    
    #Enable DHCPv6 client Report on Additional State
    tcpipDhcpClientv6DebugAdditionalState = tcpipDhcpv6Component.createBooleanSymbol("TCPIP_DHCPV6_DEBUG_MASK_ADD_STATE", tcpipDhcpClientV6DebugLevel)
    tcpipDhcpClientv6DebugAdditionalState.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientv6DebugAdditionalState.setLabel("Report On Additional State")
    tcpipDhcpClientv6DebugAdditionalState.setVisible(True)
    tcpipDhcpClientv6DebugAdditionalState.setDescription("Report On Additional State")
    tcpipDhcpClientv6DebugAdditionalState.setDefaultValue(False)
    tcpipDhcpClientv6DebugAdditionalState.setDependencies(tcpipDhcpv6MenuVisible, ["TCPIP_DHCPV6_CONFIG_DEBUG_LEVEL"])
    
    #Enable DHCPv6 Current static debugging lists 
    tcpipDhcpClientv6DebugStaticList = tcpipDhcpv6Component.createBooleanSymbol("TCPIP_DHCPV6_DEBUG_MASK_LISTS", tcpipDhcpClientV6DebugLevel)
    tcpipDhcpClientv6DebugStaticList.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientv6DebugStaticList.setLabel("Report Static List Details")
    tcpipDhcpClientv6DebugStaticList.setVisible(True)
    tcpipDhcpClientv6DebugStaticList.setDescription("Report Static List Details")
    tcpipDhcpClientv6DebugStaticList.setDefaultValue(False)
    tcpipDhcpClientv6DebugStaticList.setDependencies(tcpipDhcpv6MenuVisible, ["TCPIP_DHCPV6_CONFIG_DEBUG_LEVEL"])
    
    #Enable DHCPv6 client Buffer Traces
    tcpipDhcpClientv6DebugBufferTrace = tcpipDhcpv6Component.createBooleanSymbol("TCPIP_DHCPV6_DEBUG_MASK_BUFF_TRACE", tcpipDhcpClientV6DebugLevel)
    tcpipDhcpClientv6DebugBufferTrace.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientv6DebugBufferTrace.setLabel("Report On Buffer Trace")
    tcpipDhcpClientv6DebugBufferTrace.setVisible(True)
    tcpipDhcpClientv6DebugBufferTrace.setDescription("Report On Buffer Trace")
    tcpipDhcpClientv6DebugBufferTrace.setDefaultValue(False)
    tcpipDhcpClientv6DebugBufferTrace.setDependencies(tcpipDhcpv6MenuVisible, ["TCPIP_DHCPV6_CONFIG_DEBUG_LEVEL"])
    
    tcpipDhcpcv6heapdependency = ["tcpipNetConfig.TCPIP_STACK_NETWORK_INTERAFCE_COUNT", "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]
    
    # DHCPC V6 Heap Size
    tcpipDhcpClientv6HeapSize = tcpipDhcpv6Component.createIntegerSymbol("TCPIP_DHCPCV6_HEAP_SIZE", None)
    tcpipDhcpClientv6HeapSize.setHelp("mcc_h3_dhcpv6_configurations")
    tcpipDhcpClientv6HeapSize.setLabel("DHCPCV6 Heap Size (bytes)") 
    tcpipDhcpClientv6HeapSize.setVisible(False)
    tcpipDhcpClientv6HeapSize.setDefaultValue(tcpipDhcpcv6HeapCalc())
    tcpipDhcpClientv6HeapSize.setReadOnly(True)
    tcpipDhcpClientv6HeapSize.setDependencies(tcpipDhcpcv6HeapUpdate, tcpipDhcpcv6heapdependency)
    
    # FTL file
    tcpipDhcpv6HeaderFtl = tcpipDhcpv6Component.createFileSymbol(None, None)
    tcpipDhcpv6HeaderFtl.setSourcePath("tcpip/config/dhcpv6.h.ftl")
    tcpipDhcpv6HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipDhcpv6HeaderFtl.setMarkup(True)
    tcpipDhcpv6HeaderFtl.setType("STRING")
    
    # Add dhcpv6.c file
    tcpipDhcpv6ClientSourceFile = tcpipDhcpv6Component.createFileSymbol(None, None)
    tcpipDhcpv6ClientSourceFile.setSourcePath("tcpip/src/dhcpv6.c")
    tcpipDhcpv6ClientSourceFile.setOutputName("dhcpv6.c")
    tcpipDhcpv6ClientSourceFile.setOverwrite(True)
    tcpipDhcpv6ClientSourceFile.setDestPath("library/tcpip/src/")
    tcpipDhcpv6ClientSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipDhcpv6ClientSourceFile.setType("SOURCE")
    tcpipDhcpv6ClientSourceFile.setEnabled(True)

def tcpipDhcpV6MenuVisibleSingle(symbol, event):
    if (event["value"] == True):
        print("DHCP v6 Client Menu Visible.")      
        symbol.setVisible(True)
    else:
        print("DHCP v6 Client Menu Invisible.")
        symbol.setVisible(False)    

# make DHCPV6 Client option visible
def tcpipDhcpv6MenuVisible(tcpipDependentSymbol, event):
    tcpipIPv6 = Database.getSymbolValue("tcpipIPv6","TCPIP_STACK_USE_IPV6")
    tcpipUdp = Database.getSymbolValue("tcpipUdp","TCPIP_USE_UDP")
    print("DHCP v6 IPv6 UDP  Menu On.")
    if(tcpipIPv6 and tcpipUdp and (event["value"] == True)):
        tcpipDependentSymbol.setVisible(True)
        print("DHCPv6 IPv6 UDP  Menu On.")
    else:
        tcpipDependentSymbol.setVisible(False)  
        print("DHCPv6 IPv4 UDP  Menu Off.")       

def tcpipDhcpv6ClientHeapCalc(): 
    numInterface = Database.getSymbolValue("tcpipNetConfig","TCPIP_STACK_NETWORK_INTERAFCE_COUNT")
    heap_size = 0
    if (numInterface != None):
        heap_size = numInterface * 88
    return heap_size    

def tcpipDhcpv6ClientHeapUpdate(symbol, event): 
    heap_size = tcpipDhcpcHeapCalc()
    symbol.setValue(heap_size)
    if(event["id"] == "TCPIP_STACK_HEAP_CALC_MASK"):
        symbol.setVisible(event["value"])

#Set symbols of other components
def setVal(component, symbol, value):
    triggerDict = {"Component":component,"Id":symbol, "Value":value}
    if(Database.sendMessage(component, "SET_SYMBOL", triggerDict) == None):
        print "Set Symbol Failure" + component + ":" + symbol + ":" + str(value)
        return False
    else:
        return True

def tcpipDhcpcv6HeapCalc(): 
    numInterface = Database.getSymbolValue("tcpipNetConfig","TCPIP_STACK_NETWORK_INTERAFCE_COUNT")
    nIanaDcpts = Database.getSymbolValue("tcpipDhcpcv6","TCPIP_DHCPV6_IANA_DESCRIPTORS_NO")
    nIataDcpts = Database.getSymbolValue("tcpipDhcpcv6", "TCPIP_DHCPV6_IATA_DESCRIPTORS_NO")
    nIaFreeDcpts = Database.getSymbolValue("tcpipDhcpcv6", "TCPIP_DHCPV6_IA_FREE_DESCRIPTORS_NO")
    nMsgBuffers = Database.getSymbolValue("tcpipDhcpcv6", "TCPIP_DHCPV6_MESSAGE_BUFFERS")
    msgBufferSize = Database.getSymbolValue("tcpipDhcpcv6", "TCPIP_DHCPV6_MESSAGE_BUFFER_SIZE")
    heap_size = 0
    if (numInterface != None):
        heap_size = numInterface * (452+(nIanaDcpts+nIataDcpts+nIaFreeDcpts) * 208 + nMsgBuffers * msgBufferSize)
    return heap_size    

def tcpipDhcpcv6HeapUpdate(symbol, event): 
    heap_size = tcpipDhcpcv6HeapCalc()
    symbol.setValue(heap_size)
    if(event["id"] == "TCPIP_STACK_HEAP_CALC_MASK"):
        symbol.setVisible(event["value"])

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

def destroyComponent(component):
    Database.setSymbolValue("tcpipDhcpcv6", "TCPIP_STACK_USE_DHCPV6_CLIENT", False, 2)

