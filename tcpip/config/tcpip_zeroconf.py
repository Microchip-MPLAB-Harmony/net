"""
Copyright (C) 2019-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

def instantiateComponent(tcpipZeroConfComponent):
    print("TCPIP ZEROCONF Component")
    configName = Variables.get("__CONFIGURATION_NAME")
        
    # Use Link Local Zero Config
    tcpipZc = tcpipZeroConfComponent.createBooleanSymbol("TCPIP_USE_LINK_ZERO_CONFIG", None)
    tcpipZc.setHelp("mcc_h3_zcll_configurations")
    tcpipZc.setLabel("Use Link Local Zero Config")
    tcpipZc.setVisible(False)
    tcpipZc.setDescription("Use Link Local Zero Config")
    tcpipZc.setDefaultValue(True)
    
    # Advanced Settings
    tcpipZcMdnsAdvSettings = tcpipZeroConfComponent.createMenuSymbol("TCPIP_ZC_MDNS_ADV_SETTING", None)
    tcpipZcMdnsAdvSettings.setLabel("Advanced Settings")
    tcpipZcMdnsAdvSettings.setDescription("Advanced Settings")
    tcpipZcMdnsAdvSettings.setVisible(True)

    # Link Local Task Rate in ms
    tcpipZcTskTickRate = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_LL_TASK_TICK_RATE", tcpipZcMdnsAdvSettings)
    tcpipZcTskTickRate.setHelp("mcc_h3_zcll_configurations")
    tcpipZcTskTickRate.setLabel("Link Local Task Rate (in msec)")
    tcpipZcTskTickRate.setVisible(True)
    tcpipZcTskTickRate.setDescription("Link Local Task Rate in ms")
    tcpipZcTskTickRate.setDefaultValue(113)
# Enable Link Local Warning Messages
    tcpipZcWarnZcll = tcpipZeroConfComponent.createBooleanSymbol("TCPIP_ZC_WARN_ZCLL", tcpipZcMdnsAdvSettings)
    tcpipZcWarnZcll.setHelp("mcc_h3_zcll_configurations")
    tcpipZcWarnZcll.setLabel("Enable Link Local Warning Messages")
    tcpipZcWarnZcll.setVisible(True)
    tcpipZcWarnZcll.setDescription("Enable Link Local Warning Messages")
    tcpipZcWarnZcll.setDefaultValue(False) 

    # Enable Link Local Information Messages
    tcpipZcInfoZcll = tcpipZeroConfComponent.createBooleanSymbol("TCPIP_ZC_INFO_ZCLL", tcpipZcMdnsAdvSettings)
    tcpipZcInfoZcll.setHelp("mcc_h3_zcll_configurations")
    tcpipZcInfoZcll.setLabel("Enable Link Local Information Messages")
    tcpipZcInfoZcll.setVisible(True)
    tcpipZcInfoZcll.setDescription("Enable Link Local Information Messages")
    tcpipZcInfoZcll.setDefaultValue(False) 

    # Enable Link Local Debug Messages
    tcpipZcDebugZcll = tcpipZeroConfComponent.createBooleanSymbol("TCPIP_ZC_DEBUG_ZCLL", tcpipZcMdnsAdvSettings)
    tcpipZcDebugZcll.setHelp("mcc_h3_zcll_configurations")
    tcpipZcDebugZcll.setLabel("Enable Link Local Debug Messages")
    tcpipZcDebugZcll.setVisible(True)
    tcpipZcDebugZcll.setDescription("Enable Link Local Debug Messages")
    tcpipZcDebugZcll.setDefaultValue(False) 

    # Probe Wait in seconds
    tcpipZcProbeWait = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_LL_PROBE_WAIT", tcpipZcMdnsAdvSettings)
    tcpipZcProbeWait.setHelp("mcc_h3_zcll_configurations")
    tcpipZcProbeWait.setLabel("Probe Wait (in sec)")
    tcpipZcProbeWait.setVisible(True)
    tcpipZcProbeWait.setDescription("Probe Wait in seconds")
    tcpipZcProbeWait.setDefaultValue(1)

    # Minimum Time Between Probes in seconds
    tcpipZcProbeMin = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_LL_PROBE_MIN", tcpipZcMdnsAdvSettings)
    tcpipZcProbeMin.setHelp("mcc_h3_zcll_configurations")
    tcpipZcProbeMin.setLabel("Minimum Time Between Probes (in sec)")
    tcpipZcProbeMin.setVisible(True)
    tcpipZcProbeMin.setDescription("Minimum Time Between Probes in seconds")
    tcpipZcProbeMin.setDefaultValue(1)

    # Maxiumum Time Between Probes in seconds
    tcpipZcProbeMax = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_LL_PROBE_MAX", tcpipZcMdnsAdvSettings)
    tcpipZcProbeMax.setHelp("mcc_h3_zcll_configurations")
    tcpipZcProbeMax.setLabel("Maxiumum Time Between Probes (in sec)")
    tcpipZcProbeMax.setVisible(True)
    tcpipZcProbeMax.setDescription("Maxiumum Time Between Probes in seconds")
    tcpipZcProbeMax.setDefaultValue(2)

    # Number of Probes
    tcpipZcProbeNum = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_LL_PROBE_NUM", tcpipZcMdnsAdvSettings)
    tcpipZcProbeNum.setHelp("mcc_h3_zcll_configurations")
    tcpipZcProbeNum.setLabel("Number of Probes")
    tcpipZcProbeNum.setVisible(True)
    tcpipZcProbeNum.setDescription("Number of Probes")
    tcpipZcProbeNum.setDefaultValue(3)

    # Delay Before Announcing
    tcpipZcAnnounceWait = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_LL_ANNOUNCE_WAIT", tcpipZcMdnsAdvSettings)
    tcpipZcAnnounceWait.setHelp("mcc_h3_zcll_configurations")
    tcpipZcAnnounceWait.setLabel("Delay Before Announcing")
    tcpipZcAnnounceWait.setVisible(True)
    tcpipZcAnnounceWait.setDescription("Delay Before Announcing")
    tcpipZcAnnounceWait.setDefaultValue(2)

    # Number of Announcement Packets
    tcpipZcAnnounceNum = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_LL_ANNOUNCE_NUM", tcpipZcMdnsAdvSettings)
    tcpipZcAnnounceNum.setHelp("mcc_h3_zcll_configurations")
    tcpipZcAnnounceNum.setLabel("Number of Announcement Packets")
    tcpipZcAnnounceNum.setVisible(True)
    tcpipZcAnnounceNum.setDescription("Number of Announcement Packets")
    tcpipZcAnnounceNum.setDefaultValue(2)

    # Time Between Announcement Packets in seconds
    tcpipZcAnnounceInterval = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_LL_ANNOUNCE_INTERVAL", tcpipZcMdnsAdvSettings)
    tcpipZcAnnounceInterval.setHelp("mcc_h3_zcll_configurations")
    tcpipZcAnnounceInterval.setLabel("Time Between Announcement Packets (in sec)")
    tcpipZcAnnounceInterval.setVisible(True)
    tcpipZcAnnounceInterval.setDescription("Time Between Announcement Packets in seconds")
    tcpipZcAnnounceInterval.setDefaultValue(2)

    # Maximum Conflicts Before Rate Limiting
    tcpipZcConflictMax = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_LL_MAX_CONFLICTS", tcpipZcMdnsAdvSettings)
    tcpipZcConflictMax.setHelp("mcc_h3_zcll_configurations")
    tcpipZcConflictMax.setLabel("Maximum Conflicts Before Rate Limiting")
    tcpipZcConflictMax.setVisible(True)
    tcpipZcConflictMax.setDescription("Maximum Conflicts Before Rate Limiting")
    tcpipZcConflictMax.setDefaultValue(10)

    # Delay Between Attempts in seconds
    tcpipZcRateLimitInterval = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_LL_RATE_LIMIT_INTERVAL", tcpipZcMdnsAdvSettings)
    tcpipZcRateLimitInterval.setHelp("mcc_h3_zcll_configurations")
    tcpipZcRateLimitInterval.setLabel("Delay Between Attempts (in sec)")
    tcpipZcRateLimitInterval.setVisible(True)
    tcpipZcRateLimitInterval.setDescription("Delay Between Attempts in seconds")
    tcpipZcRateLimitInterval.setDefaultValue(60)

    # Rate Between Defensive ARPs in seconds
    tcpipZcDefendInterval = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_LL_DEFEND_INTERVAL", tcpipZcMdnsAdvSettings)
    tcpipZcDefendInterval.setHelp("mcc_h3_zcll_configurations")
    tcpipZcDefendInterval.setLabel("Rate Between Defensive ARPs (in sec)")
    tcpipZcDefendInterval.setVisible(True)
    tcpipZcDefendInterval.setDescription("Rate Between Defensive ARPs in seconds")
    tcpipZcDefendInterval.setDefaultValue(10)

    # Base IP
    tcpipZcBaseIp = tcpipZeroConfComponent.createHexSymbol("TCPIP_ZC_LL_IPV4_LLBASE", tcpipZcMdnsAdvSettings) 
    tcpipZcBaseIp.setHelp("mcc_h3_zcll_configurations")
    tcpipZcBaseIp.setLabel("Base IP")
    tcpipZcBaseIp.setVisible(True)
    tcpipZcBaseIp.setDescription("Base IP")
    tcpipZcBaseIp.setDefaultValue(0xa9fe0100)

    # Base Netmask
    tcpipZcBaseNetmask = tcpipZeroConfComponent.createHexSymbol("TCPIP_ZC_LL_IPV4_LLBASE_MASK", tcpipZcMdnsAdvSettings)
    tcpipZcBaseNetmask.setHelp("mcc_h3_zcll_configurations")
    tcpipZcBaseNetmask.setLabel("Base Netmask")
    tcpipZcBaseNetmask.setVisible(True)
    tcpipZcBaseNetmask.setDescription("Base Netmask")
    tcpipZcBaseNetmask.setDefaultValue(0x0000FFFF)

    # Use Multicast DNS Zero Config (Bonjour)
    tcpipZcMulticastDns = tcpipZeroConfComponent.createBooleanSymbol("TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG", tcpipZcMdnsAdvSettings)
    tcpipZcMulticastDns.setHelp("mcc_h3_zcll_configurations")
    tcpipZcMulticastDns.setLabel("Use Multicast DNS Zero Config (Bonjour)")
    tcpipZcMulticastDns.setVisible(False)
    tcpipZcMulticastDns.setDescription("Use Multicast DNS Zero Config (Bonjour)")
    tcpipZcMulticastDns.setDefaultValue(False) 
    #tcpipZcMulticastDns.setDependencies(tcpipMulticastDnsZcMenuVisible, ["tcpipUdp.TCPIP_USE_UDP"])
    #tcpipZcMulticastDns.setDependencies(tcpipMulticastDnsZcMenuVisible, ["tcpipUdp.TCPIP_USE_UDP", "TCPIP_USE_LINK_ZERO_CONFIG"])

    # Task Tick Rate
    tcpipZcMdnsTskTickRate = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_TASK_TICK_RATE", tcpipZcMulticastDns)
    tcpipZcMdnsTskTickRate.setHelp("mcc_h3_zcll_configurations")
    tcpipZcMdnsTskTickRate.setLabel("MDNS Task Rate (in msec)")
    tcpipZcMdnsTskTickRate.setVisible(False)
    tcpipZcMdnsTskTickRate.setDescription("Task Tick Rate in milliseconds")
    tcpipZcMdnsTskTickRate.setDefaultValue(63)
    tcpipZcMdnsTskTickRate.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])
    
    # Enable Multicast DNS Information Messages
    tcpipZcMdnsInfo = tcpipZeroConfComponent.createBooleanSymbol("TCPIP_ZC_INFO_MDNS", tcpipZcMulticastDns)
    tcpipZcMdnsInfo.setHelp("mcc_h3_zcll_configurations")
    tcpipZcMdnsInfo.setLabel("Enable Multicast DNS Information Messages")
    tcpipZcMdnsInfo.setVisible(False)
    tcpipZcMdnsInfo.setDescription("Enable Multicast DNS Information Messages")
    tcpipZcMdnsInfo.setDefaultValue(False) 
    tcpipZcMdnsInfo.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

    # Enable Multicast DNS Warning Messages
    tcpipZcMdnsWarn = tcpipZeroConfComponent.createBooleanSymbol("TCPIP_ZC_WARN_MDNS", tcpipZcMulticastDns)
    tcpipZcMdnsWarn.setHelp("mcc_h3_zcll_configurations")
    tcpipZcMdnsWarn.setLabel("Enable Multicast DNS Warning Messages")
    tcpipZcMdnsWarn.setVisible(False)
    tcpipZcMdnsWarn.setDescription("Enable Multicast DNS Warning Messages")
    tcpipZcMdnsWarn.setDefaultValue(False) 
    tcpipZcMdnsWarn.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

    # Enable Multicast DNS Debug Messages
    tcpipZcMdnsDebug = tcpipZeroConfComponent.createBooleanSymbol("TCPIP_ZC_DEBUG_MDNS", tcpipZcMulticastDns)
    tcpipZcMdnsDebug.setHelp("mcc_h3_zcll_configurations")
    tcpipZcMdnsDebug.setLabel("Enable Multicast DNS Debug Messages")
    tcpipZcMdnsDebug.setVisible(False)
    tcpipZcMdnsDebug.setDescription("Enable Multicast DNS Debug Messages")
    tcpipZcMdnsDebug.setDefaultValue(False) 
    tcpipZcMdnsDebug.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

    # Port Number
    tcpipZcMdnsPort = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_PORT", tcpipZcMulticastDns)
    tcpipZcMdnsPort.setHelp("mcc_h3_zcll_configurations")
    tcpipZcMdnsPort.setLabel("Port")
    tcpipZcMdnsPort.setVisible(False)
    tcpipZcMdnsPort.setDescription("Port")
    tcpipZcMdnsPort.setDefaultValue(5353)
    tcpipZcMdnsPort.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

    # Maximum Host Name Size
    tcpipZcMdnsHostNameSizeMax = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_MAX_HOST_NAME_SIZE", tcpipZcMulticastDns)
    tcpipZcMdnsHostNameSizeMax.setHelp("mcc_h3_zcll_configurations")
    tcpipZcMdnsHostNameSizeMax.setLabel("Maximum Host Name Size")
    tcpipZcMdnsHostNameSizeMax.setVisible(False)
    tcpipZcMdnsHostNameSizeMax.setDescription("Maximum Host Name Size")
    tcpipZcMdnsHostNameSizeMax.setDefaultValue(32)
    tcpipZcMdnsHostNameSizeMax.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

    # Maximum Size Allowed For a Label
    tcpipZcMdnsLabelSizeMax = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_MAX_LABEL_SIZE", tcpipZcMulticastDns)
    tcpipZcMdnsLabelSizeMax.setHelp("mcc_h3_zcll_configurations")
    tcpipZcMdnsLabelSizeMax.setLabel("Maximum Size Allowed For a Label")
    tcpipZcMdnsLabelSizeMax.setVisible(False)
    tcpipZcMdnsLabelSizeMax.setDescription("Maximum Size Allowed For a Label")
    tcpipZcMdnsLabelSizeMax.setDefaultValue(64)
    tcpipZcMdnsLabelSizeMax.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

    # Maximum Resource Record Name Size
    tcpipZcMdnsRrNameSizeMax = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_MAX_RR_NAME_SIZE", tcpipZcMulticastDns)
    tcpipZcMdnsRrNameSizeMax.setHelp("mcc_h3_zcll_configurations")
    tcpipZcMdnsRrNameSizeMax.setLabel("Maximum Resource Record Name Size")
    tcpipZcMdnsRrNameSizeMax.setVisible(False)
    tcpipZcMdnsRrNameSizeMax.setDescription("Maximum Resource Record Name Size")
    tcpipZcMdnsRrNameSizeMax.setDefaultValue(256)
    tcpipZcMdnsRrNameSizeMax.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

    # Maximum Server Type Size
    tcpipZcMdnsSrvTypeSizeMax = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_MAX_SRV_TYPE_SIZE", tcpipZcMulticastDns)
    tcpipZcMdnsSrvTypeSizeMax.setHelp("mcc_h3_zcll_configurations")
    tcpipZcMdnsSrvTypeSizeMax.setLabel("Maximum Server Type Size")
    tcpipZcMdnsSrvTypeSizeMax.setVisible(False)
    tcpipZcMdnsSrvTypeSizeMax.setDescription("Maximum Server Type Size")
    tcpipZcMdnsSrvTypeSizeMax.setDefaultValue(32)
    tcpipZcMdnsSrvTypeSizeMax.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

    # Maximum Server Name Size
    tcpipZcMdnsSrvNameSizeMax = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_MAX_SRV_NAME_SIZE", tcpipZcMulticastDns)
    tcpipZcMdnsSrvNameSizeMax.setHelp("mcc_h3_zcll_configurations")
    tcpipZcMdnsSrvNameSizeMax.setLabel("Maximum Server Name Size")
    tcpipZcMdnsSrvNameSizeMax.setVisible(False)
    tcpipZcMdnsSrvNameSizeMax.setDescription("Maximum Server Name Size")
    tcpipZcMdnsSrvNameSizeMax.setDefaultValue(64)
    tcpipZcMdnsSrvNameSizeMax.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

    # Maximum Text Data Size
    tcpipZcMdnsTxtDataSizeMax = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_MAX_TXT_DATA_SIZE", tcpipZcMulticastDns)
    tcpipZcMdnsTxtDataSizeMax.setHelp("mcc_h3_zcll_configurations")
    tcpipZcMdnsTxtDataSizeMax.setLabel("Maximum Text Data Size")
    tcpipZcMdnsTxtDataSizeMax.setVisible(False)
    tcpipZcMdnsTxtDataSizeMax.setDescription("Maximum Text Data Size")
    tcpipZcMdnsTxtDataSizeMax.setDefaultValue(128)
    tcpipZcMdnsTxtDataSizeMax.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

    # Time-To-Live for a Resource-Record in seconds
    tcpipZcMdnsRrTtlVal= tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_RESOURCE_RECORD_TTL_VAL", tcpipZcMulticastDns)
    tcpipZcMdnsRrTtlVal.setHelp("mcc_h3_zcll_configurations")
    tcpipZcMdnsRrTtlVal.setLabel("Time-To-Live for a Resource-Record (in sec)")
    tcpipZcMdnsRrTtlVal.setVisible(False)
    tcpipZcMdnsRrTtlVal.setDescription("Time-To-Live for a Resource-Record in seconds")
    tcpipZcMdnsRrTtlVal.setDefaultValue(3600)
    tcpipZcMdnsRrTtlVal.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

    # Maximum Number of Resource-Records/Service
    tcpipZcMdnsRrNumMax= tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_MAX_RR_NUM", tcpipZcMulticastDns)
    tcpipZcMdnsRrNumMax.setHelp("mcc_h3_zcll_configurations")
    tcpipZcMdnsRrNumMax.setLabel("Maximum Number of Resource-Records/Service")
    tcpipZcMdnsRrNumMax.setVisible(False)
    tcpipZcMdnsRrNumMax.setDescription("Maximum Number of Resource-Records/Service")
    tcpipZcMdnsRrNumMax.setDefaultValue(4)
    tcpipZcMdnsRrNumMax.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

    # Initial Random Delay in ms
    tcpipZcMdnsProbeWait= tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_PROBE_WAIT", tcpipZcMulticastDns)
    tcpipZcMdnsProbeWait.setHelp("mcc_h3_zcll_configurations")
    tcpipZcMdnsProbeWait.setLabel("Initial Random Delay (in msec)")
    tcpipZcMdnsProbeWait.setVisible(False)
    tcpipZcMdnsProbeWait.setDescription("Initial Random Delay in ms")
    tcpipZcMdnsProbeWait.setDefaultValue(750)
    tcpipZcMdnsProbeWait.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

    # Maximum Delay Till Repeated Probe in ms
    tcpipZcMdnsProbeInterval= tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_PROBE_INTERVAL", tcpipZcMulticastDns)
    tcpipZcMdnsProbeInterval.setHelp("mcc_h3_zcll_configurations")
    tcpipZcMdnsProbeInterval.setLabel("Maximum Delay Till Repeated Probe (in msec)")
    tcpipZcMdnsProbeInterval.setVisible(False)
    tcpipZcMdnsProbeInterval.setDescription("Maximum Delay Till Repeated Probe in ms")
    tcpipZcMdnsProbeInterval.setDefaultValue(250)
    tcpipZcMdnsProbeInterval.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

    # Number of Probes
    tcpipZcMdnsProbeNum= tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_PROBE_NUM", tcpipZcMulticastDns)
    tcpipZcMdnsProbeNum.setHelp("mcc_h3_zcll_configurations")
    tcpipZcMdnsProbeNum.setLabel("Number of Probes")
    tcpipZcMdnsProbeNum.setVisible(False)
    tcpipZcMdnsProbeNum.setDescription("Number of Probes")
    tcpipZcMdnsProbeNum.setDefaultValue(3)
    tcpipZcMdnsProbeNum.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

    # Maximum Number of Conflicts
    tcpipZcMdnsProbeConflictNumMax= tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_MAX_PROBE_CONFLICT_NUM", tcpipZcMulticastDns)
    tcpipZcMdnsProbeConflictNumMax.setHelp("mcc_h3_zcll_configurations")
    tcpipZcMdnsProbeConflictNumMax.setLabel("Maximum Number of Conflicts")
    tcpipZcMdnsProbeConflictNumMax.setVisible(False)
    tcpipZcMdnsProbeConflictNumMax.setDescription("Maximum Number of Conflicts")
    tcpipZcMdnsProbeConflictNumMax.setDefaultValue(30)
    tcpipZcMdnsProbeConflictNumMax.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

    # Number of Announcement Packets
    tcpipZcMdnsAnnounceNum= tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_ANNOUNCE_NUM", tcpipZcMulticastDns)
    tcpipZcMdnsAnnounceNum.setHelp("mcc_h3_zcll_configurations")
    tcpipZcMdnsAnnounceNum.setLabel("Number of Announcement Packets")
    tcpipZcMdnsAnnounceNum.setVisible(False)
    tcpipZcMdnsAnnounceNum.setDescription("Number of Announcement Packets")
    tcpipZcMdnsAnnounceNum.setDefaultValue(3)
    tcpipZcMdnsAnnounceNum.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

    # Time Between Announcement Packets in ms
    tcpipZcMdnsAnnounceInterval= tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_ANNOUNCE_INTERVAL", tcpipZcMulticastDns)
    tcpipZcMdnsAnnounceInterval.setHelp("mcc_h3_zcll_configurations")
    tcpipZcMdnsAnnounceInterval.setLabel("Time Between Announcement Packets (in msec)")
    tcpipZcMdnsAnnounceInterval.setVisible(False)
    tcpipZcMdnsAnnounceInterval.setDescription("Time Between Announcement Packets in ms")
    tcpipZcMdnsAnnounceInterval.setDefaultValue(250)
    tcpipZcMdnsAnnounceInterval.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])

    # Delay Before Announcing in ms
    tcpipZcMdnsAnnounceWait= tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_ANNOUNCE_WAIT", tcpipZcMulticastDns)
    tcpipZcMdnsAnnounceWait.setHelp("mcc_h3_zcll_configurations")
    tcpipZcMdnsAnnounceWait.setLabel("Delay Before Announcing (in msec)")
    tcpipZcMdnsAnnounceWait.setVisible(False)
    tcpipZcMdnsAnnounceWait.setDescription("Delay Before Announcing in ms")
    tcpipZcMdnsAnnounceWait.setDefaultValue(250)
    tcpipZcMdnsAnnounceWait.setDependencies(tcpipZeroConfMenuVisibleSingle, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])
    
    tcpipZcMdnsheapdependency = ["tcpipNetConfig.TCPIP_STACK_NETWORK_INTERAFCE_COUNT", "TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG", 
                                    "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]     
    # ZCLL-MDNS Heap Size
    tcpipZcMdnsHeapSize = tcpipZeroConfComponent.createIntegerSymbol("TCPIP_ZC_MDNS_HEAP_SIZE", None)
    tcpipZcMdnsHeapSize.setLabel("ZCLL-MDNS Heap Size (bytes)") 
    tcpipZcMdnsHeapSize.setVisible(False)
    tcpipZcMdnsHeapSize.setDefaultValue(tcpipZcMdnsHeapCalc())
    tcpipZcMdnsHeapSize.setReadOnly(True)
    tcpipZcMdnsHeapSize.setDependencies(tcpipZcMdnsHeapUpdate, tcpipZcMdnsheapdependency)   

    #Add to system_config.h
    tcpipZcHeaderFtl = tcpipZeroConfComponent.createFileSymbol(None, None)
    tcpipZcHeaderFtl.setSourcePath("tcpip/config/zeroconf.h.ftl")
    tcpipZcHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipZcHeaderFtl.setMarkup(True)
    tcpipZcHeaderFtl.setType("STRING")
    
    # Add zero_conf_helper.c file
    tcpipZcHelperSourceFile = tcpipZeroConfComponent.createFileSymbol(None, None)
    tcpipZcHelperSourceFile.setSourcePath("tcpip/src/zero_conf_helper.c")
    tcpipZcHelperSourceFile.setOutputName("zero_conf_helper.c")
    tcpipZcHelperSourceFile.setOverwrite(True)
    tcpipZcHelperSourceFile.setDestPath("library/tcpip/src/")
    tcpipZcHelperSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipZcHelperSourceFile.setType("SOURCE")
    tcpipZcHelperSourceFile.setEnabled(True)
    tcpipZcHelperSourceFile.setDependencies(tcpipZeroConfGenSourceFile, ["TCPIP_USE_LINK_ZERO_CONFIG"])

    # Add zero_conf_link_local.c file
    tcpipZcLlSourceFile = tcpipZeroConfComponent.createFileSymbol(None, None)
    tcpipZcLlSourceFile.setSourcePath("tcpip/src/zero_conf_link_local.c")
    tcpipZcLlSourceFile.setOutputName("zero_conf_link_local.c")
    tcpipZcLlSourceFile.setOverwrite(True)
    tcpipZcLlSourceFile.setDestPath("library/tcpip/src/")
    tcpipZcLlSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipZcLlSourceFile.setType("SOURCE")
    tcpipZcLlSourceFile.setEnabled(True)
    tcpipZcLlSourceFile.setDependencies(tcpipZeroConfGenSourceFile, ["TCPIP_USE_LINK_ZERO_CONFIG"])

    # Add zero_conf_multicast_dns.c file
    tcpipZcMdnsSourceFile = tcpipZeroConfComponent.createFileSymbol(None, None)
    tcpipZcMdnsSourceFile.setSourcePath("tcpip/src/zero_conf_multicast_dns.c")
    tcpipZcMdnsSourceFile.setOutputName("zero_conf_multicast_dns.c")
    tcpipZcMdnsSourceFile.setOverwrite(True)
    tcpipZcMdnsSourceFile.setDestPath("library/tcpip/src/")
    tcpipZcMdnsSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipZcMdnsSourceFile.setType("SOURCE")
    tcpipZcMdnsSourceFile.setEnabled(True)
    tcpipZcMdnsSourceFile.setDependencies(tcpipZeroConfGenSourceFile, ["TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG"])
        
def tcpipMulticastDnsZcMenuVisible(symbol, event):  
    if (event["value"] == True):                
        symbol.setVisible(True)
    else:       
        symbol.setVisible(False)

        
def tcpipZeroConfMenuVisibleSingle(symbol, event):
    if (event["value"] == True):
        print("ZeroConf Menu Visible.")     
        symbol.setVisible(True)
    else:
        print("ZeroConf Menu Invisible.")
        symbol.setVisible(False)

def tcpipZeroConfGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])
    

def tcpipZcMdnsHeapCalc(): 
    numInterface = Database.getSymbolValue("tcpipNetConfig","TCPIP_STACK_NETWORK_INTERAFCE_COUNT")    
    memUsage = 32
    
    if(Database.getSymbolValue("tcpipZeroConf","TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG") == True):
        memUsage = memUsage + 800   
    
    heap_size = 0
    if (numInterface != None):
        heap_size = numInterface * memUsage
    return heap_size    
    
def tcpipZcMdnsHeapUpdate(symbol, event): 
    heap_size = tcpipZcMdnsHeapCalc()
    symbol.setValue(heap_size)
    if(event["id"] == "TCPIP_STACK_HEAP_CALC_MASK"):
        symbol.setVisible(event["value"])
    
#########################################################################################################
def onAttachmentConnected(source, target):
    if (source["id"] == "ZeroConf_UDP_Dependency"): 
        tcpipZcMulticastDns = source["component"].getSymbolByID("TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG")
        tcpipZcMulticastDns.setVisible(True)
        
def onAttachmentDisconnected(source, target):
    if (source["id"] == "ZeroConf_UDP_Dependency"): 
        tcpipZcMulticastDns = source["component"].getSymbolByID("TCPIP_USE_MULTI_CAST_DNS_ZERO_CONFIG")
        tcpipZcMulticastDns.setVisible(False)

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
      
  
def destroyComponent(component):
    Database.setSymbolValue("tcpipZeroConf", "TCPIP_USE_LINK_ZERO_CONFIG", False, 2)
