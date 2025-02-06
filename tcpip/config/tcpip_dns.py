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

# Global definitions  
TCPIP_ADDRESS_TYPE_STRICT = ["IP_ADDRESS_TYPE_IPV4", "IP_ADDRESS_TYPE_IPV6"]
    
def instantiateComponent(tcpipDnsComponent):
    print("TCPIP DNS Client Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # Enable DNS Client
    tcpipDnsc = tcpipDnsComponent.createBooleanSymbol("TCPIP_USE_DNS_CLIENT", None)
    tcpipDnsc.setHelp("mcc_h3_dnsc_configurations")
    tcpipDnsc.setLabel("DNS Client")
    tcpipDnsc.setVisible(False)
    tcpipDnsc.setDescription("Enable DNS Client")
    tcpipDnsc.setDefaultValue(True)
    
    # Number of DNS Resolver Entries
    tcpipDnscCacheEntryNum = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_CACHE_ENTRIES", None)
    tcpipDnscCacheEntryNum.setHelp("mcc_h3_dnsc_configurations")
    tcpipDnscCacheEntryNum.setLabel("Number of DNS Cache Entries")
    tcpipDnscCacheEntryNum.setVisible(True)
    tcpipDnscCacheEntryNum.setDescription("Number of DNS Resolver Cache Entries")
    tcpipDnscCacheEntryNum.setDefaultValue(5)
    
    # DNS Connection Type 
    tcpipDnscAddressType = tcpipDnsComponent.createComboSymbol("TCPIP_DNS_CLIENT_ADDRESS_TYPE", None, TCPIP_ADDRESS_TYPE_STRICT)
    tcpipDnscAddressType.setHelp("mcc_h3_dnsc_configurations")
    tcpipDnscAddressType.setLabel("DNS Connection Type")
    tcpipDnscAddressType.setVisible(True)
    tcpipDnscAddressType.setDescription("DNS Connection Type")
    tcpipDnscAddressType.setDefaultValue("IP_ADDRESS_TYPE_IPV4")

    # DNS Client Host Name Length
    tcpipDnscHostNameLenMax = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN", None)
    tcpipDnscHostNameLenMax.setHelp("mcc_h3_dnsc_configurations")
    tcpipDnscHostNameLenMax.setLabel("DNS Host Name Maximum Length")
    tcpipDnscHostNameLenMax.setVisible(True)
    tcpipDnscHostNameLenMax.setDescription("DNS Client Host Name Length")
    tcpipDnscHostNameLenMax.setDefaultValue(64)

    # Enable DNS Client Console Command
    tcpipDnscCmdEnable = tcpipDnsComponent.createBooleanSymbol("TCPIP_DNS_CLIENT_CONSOLE_CMD", None)
    tcpipDnscCmdEnable.setHelp("mcc_h3_dnsc_configurations")
    tcpipDnscCmdEnable.setLabel("Enable Console Commands")
    tcpipDnscCmdEnable.setVisible(True)
    tcpipDnscCmdEnable.setDescription("Enable DNS Client Console Commands")
    tcpipDnscCmdEnable.setDefaultValue(True)
        
    # Advanced Settings
    tcpipDnscAdvSettings = tcpipDnsComponent.createMenuSymbol("TCPIP_DNS_ADV_SETTING", None)
    tcpipDnscAdvSettings.setLabel("Advanced Settings")
    tcpipDnscAdvSettings.setDescription("Advanced Settings")
    tcpipDnscAdvSettings.setVisible(True)

    # DNS Client Task Update Rate in msec
    tcpipDnscTskProcessRate = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_TASK_PROCESS_RATE", tcpipDnscAdvSettings)
    tcpipDnscTskProcessRate.setHelp("mcc_h3_dnsc_configurations")
    tcpipDnscTskProcessRate.setLabel("DNS Client Task Rate (in msec)")
    tcpipDnscTskProcessRate.setVisible(True)
    tcpipDnscTskProcessRate.setDescription("Task Update Rate in msec")
    tcpipDnscTskProcessRate.setDefaultValue(200)
    
    # DNS Client Cache Entry Time outs
    tcpipDnscCacheEntryTimeout = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_CACHE_ENTRY_TMO", tcpipDnscAdvSettings)
    tcpipDnscCacheEntryTimeout.setHelp("mcc_h3_dnsc_configurations")
    tcpipDnscCacheEntryTimeout.setLabel("Default Cache Entry Timeout (in sec)")
    tcpipDnscCacheEntryTimeout.setVisible(True)
    tcpipDnscCacheEntryTimeout.setDescription("DNS Client Cache Entry Time outs")
    tcpipDnscCacheEntryTimeout.setDefaultValue(0)

    # DNS Client-Server Time out in Seconds
    tcpipDnscServerTimeout = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_SERVER_TMO", tcpipDnscAdvSettings)
    tcpipDnscServerTimeout.setHelp("mcc_h3_dnsc_configurations")
    tcpipDnscServerTimeout.setLabel("No Server Reply Timeout(in sec)")
    tcpipDnscServerTimeout.setVisible(True)
    tcpipDnscServerTimeout.setDescription("Client-Server Time out in Seconds")
    tcpipDnscServerTimeout.setDefaultValue(60)
    
    # Maximum Number of IPv4 Answers to be Considered
    tcpipDnscCachePerIpv4Num = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_CACHE_PER_IPV4_ADDRESS", tcpipDnscAdvSettings)
    tcpipDnscCachePerIpv4Num.setHelp("mcc_h3_dnsc_configurations")
    tcpipDnscCachePerIpv4Num.setLabel("Maximum Number of IPv4 Answers to be Considered")
    tcpipDnscCachePerIpv4Num.setVisible(True)
    tcpipDnscCachePerIpv4Num.setDescription("Maximum Number of IPv4 Answers to be Considered")
    tcpipDnscCachePerIpv4Num.setDefaultValue(5)

    # Maximum Number of IPv6 Answers to be Considered
    tcpipDnscCachePerIpv6Num = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_CACHE_PER_IPV6_ADDRESS", tcpipDnscAdvSettings)
    tcpipDnscCachePerIpv6Num.setHelp("mcc_h3_dnsc_configurations")
    tcpipDnscCachePerIpv6Num.setLabel("Maximum Number of IPv6 Answers to be Considered")
    tcpipDnscCachePerIpv6Num.setVisible(True)
    tcpipDnscCachePerIpv6Num.setDescription("Maximum Number of IPv4 Answers to be Considered")
    tcpipDnscCachePerIpv6Num.setDefaultValue(1)
    
    # Maximum Number of Interfaces Considered for DNS resolution
    tcpipDnscInterfaceMaxNum = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_MAX_SELECT_INTERFACES", tcpipDnscAdvSettings)
    tcpipDnscInterfaceMaxNum.setHelp("mcc_h3_dnsc_configurations")
    tcpipDnscInterfaceMaxNum.setLabel("Maximum Number of Interfaces Considered for DNS resolution")
    tcpipDnscInterfaceMaxNum.setVisible(True)
    tcpipDnscInterfaceMaxNum.setDescription("Maximum Number of Interfaces Considered for DNS resolution")
    tcpipDnscInterfaceMaxNum.setDefaultValue(4)

    # Default TTL time in Seconds
    tcpipDnscCacheTtlTime = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_CACHE_DEFAULT_TTL_VAL", tcpipDnscAdvSettings)
    tcpipDnscCacheTtlTime.setHelp("mcc_h3_dnsc_configurations")
    tcpipDnscCacheTtlTime.setLabel("Default TTL time (in sec)")
    tcpipDnscCacheTtlTime.setVisible(True)
    tcpipDnscCacheTtlTime.setDescription("Default Time-To-Live time in Seconds")
    tcpipDnscCacheTtlTime.setDefaultValue(1200)

    # Retry Lookup Host name after Time-out
    tcpipDnscLookupRetryTimeout = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_LOOKUP_RETRY_TMO", tcpipDnscAdvSettings)
    tcpipDnscLookupRetryTimeout.setHelp("mcc_h3_dnsc_configurations")
    tcpipDnscLookupRetryTimeout.setLabel("Retry Unsolved Cache Entry Timeout (in sec)")
    tcpipDnscLookupRetryTimeout.setVisible(True)
    tcpipDnscLookupRetryTimeout.setDescription("Timeout to Retry Unsolved Cache Entry; Should be less than Unsolved Cache Entry Timeout")
    tcpipDnscLookupRetryTimeout.setDefaultValue(2)
    
    # Delete Old Entries
    tcpipDnscDeleteOldEntry= tcpipDnsComponent.createBooleanSymbol("TCPIP_DNS_CLIENT_DELETE_OLD_ENTRIES", tcpipDnscAdvSettings)
    tcpipDnscDeleteOldEntry.setHelp("mcc_h3_dnsc_configurations")
    tcpipDnscDeleteOldEntry.setLabel("Delete Old Entries")
    tcpipDnscDeleteOldEntry.setVisible(True)
    tcpipDnscDeleteOldEntry.setDescription("Delete Old Entries")
    tcpipDnscDeleteOldEntry.setDefaultValue(True)

    # Enable User Notification
    tcpipDnscUsrNotification= tcpipDnsComponent.createBooleanSymbol("TCPIP_DNS_CLIENT_USER_NOTIFICATION", tcpipDnscAdvSettings)
    tcpipDnscUsrNotification.setHelp("mcc_h3_dnsc_configurations")
    tcpipDnscUsrNotification.setLabel("Enable User Notification")
    tcpipDnscUsrNotification.setVisible(True)
    tcpipDnscUsrNotification.setDescription("Enable User Notification")
    tcpipDnscUsrNotification.setDefaultValue(False)

    tcpipDnscheapdependency = ["TCPIP_DNS_CLIENT_CACHE_ENTRIES", "TCPIP_DNS_CLIENT_CACHE_PER_IPV4_ADDRESS", 
                               "TCPIP_DNS_CLIENT_CACHE_PER_IPV6_ADDRESS", "TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN", 
                               "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]    
        
    # DNS Client Heap Size
    tcpipDnscHeapSize = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_HEAP_SIZE", None)
    tcpipDnscHeapSize.setLabel("DNS Client Heap Size (bytes)") 
    tcpipDnscHeapSize.setVisible(False)
    tcpipDnscHeapSize.setDefaultValue(tcpipDnscHeapCalc())
    tcpipDnscHeapSize.setReadOnly(True)
    tcpipDnscHeapSize.setDependencies(tcpipDnscHeapUpdate, tcpipDnscheapdependency)
    
    #Add to system_config.h
    tcpipDnscHeaderFtl = tcpipDnsComponent.createFileSymbol(None, None)
    tcpipDnscHeaderFtl.setSourcePath("tcpip/config/dns.h.ftl")
    tcpipDnscHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipDnscHeaderFtl.setMarkup(True)
    tcpipDnscHeaderFtl.setType("STRING")

    
def tcpipDnsMenuVisible(symbol, event):
    if (event["value"] == True):
        print("DNS Client Menu Visible.")       
        symbol.setVisible(True)
    else:
        print("DNS Client Menu Invisible.")
        symbol.setVisible(False)    

def tcpipDnscHeapCalc(): 
    cacheEntries = Database.getSymbolValue("tcpipDns","TCPIP_DNS_CLIENT_CACHE_ENTRIES")
    nIPv4Entries = Database.getSymbolValue("tcpipDns","TCPIP_DNS_CLIENT_CACHE_PER_IPV4_ADDRESS")
    nIPv6Entries = Database.getSymbolValue("tcpipDns","TCPIP_DNS_CLIENT_CACHE_PER_IPV6_ADDRESS")
    hostNameLen = Database.getSymbolValue("tcpipDns","TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN")
    heap_size = 40 + (cacheEntries * 44) + (cacheEntries *  ((nIPv4Entries * 4) + (nIPv6Entries * 16)) + hostNameLen)
    return heap_size    
    
def tcpipDnscHeapUpdate(symbol, event): 
    heap_size = tcpipDnscHeapCalc()
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
    Database.setSymbolValue("tcpipDns", "TCPIP_USE_DNS_CLIENT", False, 2)       
