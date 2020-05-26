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

# Global definitions  
TCPIP_ADDRESS_TYPE_STRICT = ["IP_ADDRESS_TYPE_IPV4", "IP_ADDRESS_TYPE_IPV6"]
    
def instantiateComponent(tcpipDnsComponent):
    print("TCPIP DNS Client Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # Enable DNS Client
    tcpipDnsc = tcpipDnsComponent.createBooleanSymbol("TCPIP_USE_DNS_CLIENT", None)
    tcpipDnsc.setLabel("DNS Client")
    tcpipDnsc.setVisible(False)
    tcpipDnsc.setDescription("Enable DNS Client")
    tcpipDnsc.setDefaultValue(True)
    #tcpipDnsc.setDependencies(tcpipDnsMenuVisible, ["tcpipUdp.TCPIP_USE_UDP"])

    # DNS Client-Server Time out in Seconds
    tcpipDnscServerTimeout = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_SERVER_TMO", None)
    tcpipDnscServerTimeout.setLabel("Client-Server Time-out in Seconds")
    tcpipDnscServerTimeout.setVisible(True)
    tcpipDnscServerTimeout.setDescription("Client-Server Time out in Seconds")
    tcpipDnscServerTimeout.setDefaultValue(60)
    #tcpipDnscServerTimeout.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

    # DNS Client Task Update Rate in msec
    tcpipDnscTskProcessRate = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_TASK_PROCESS_RATE", None)
    tcpipDnscTskProcessRate.setLabel("Task Update Rate in msec")
    tcpipDnscTskProcessRate.setVisible(True)
    tcpipDnscTskProcessRate.setDescription("Task Update Rate in msec")
    tcpipDnscTskProcessRate.setDefaultValue(200)
    #tcpipDnscTskProcessRate.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

    # Number of DNS Resolver Entries
    tcpipDnscCacheEntryNum = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_CACHE_ENTRIES", None)
    tcpipDnscCacheEntryNum.setLabel("Number of DNS Resolver Entries")
    tcpipDnscCacheEntryNum.setVisible(True)
    tcpipDnscCacheEntryNum.setDescription("Number of DNS Resolver Entries")
    tcpipDnscCacheEntryNum.setDefaultValue(5)
    #tcpipDnscCacheEntryNum.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

    # DNS Client Cache Entry Time outs
    tcpipDnscCacheEntryTimeout = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_CACHE_ENTRY_TMO", None)
    tcpipDnscCacheEntryTimeout.setLabel("DNS Client Time-outs")
    tcpipDnscCacheEntryTimeout.setVisible(True)
    tcpipDnscCacheEntryTimeout.setDescription("DNS Client Cache Entry Time outs")
    tcpipDnscCacheEntryTimeout.setDefaultValue(0)
    #tcpipDnscCacheEntryTimeout.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

    # Maximum Number of IPv4 Answers to be Considered
    tcpipDnscCachePerIpv4Num = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_CACHE_PER_IPV4_ADDRESS", None)
    tcpipDnscCachePerIpv4Num.setLabel("Maximum Number of IPv4 Answers to be Considered")
    tcpipDnscCachePerIpv4Num.setVisible(True)
    tcpipDnscCachePerIpv4Num.setDescription("Maximum Number of IPv4 Answers to be Considered")
    tcpipDnscCachePerIpv4Num.setDefaultValue(5)
    #tcpipDnscCachePerIpv4Num.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

    # Maximum Number of IPv6 Answers to be Considered
    tcpipDnscCachePerIpv6Num = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_CACHE_PER_IPV6_ADDRESS", None)
    tcpipDnscCachePerIpv6Num.setLabel("Maximum Number of IPv6 Answers to be Considered")
    tcpipDnscCachePerIpv6Num.setVisible(True)
    tcpipDnscCachePerIpv6Num.setDescription("Maximum Number of IPv4 Answers to be Considered")
    tcpipDnscCachePerIpv6Num.setDefaultValue(1)
    #tcpipDnscCachePerIpv6Num.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

    # DNS Connection Type 
    tcpipDnscAddressType = tcpipDnsComponent.createComboSymbol("TCPIP_DNS_CLIENT_ADDRESS_TYPE", None, TCPIP_ADDRESS_TYPE_STRICT)
    tcpipDnscAddressType.setLabel("DNS Connection Type")
    tcpipDnscAddressType.setVisible(True)
    tcpipDnscAddressType.setDescription("DNS Connection Type")
    tcpipDnscAddressType.setDefaultValue("IP_ADDRESS_TYPE_IPV4")
    #tcpipDnscAddressType.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

    # Default TTL time in Seconds
    tcpipDnscCacheTtlTime = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_CACHE_DEFAULT_TTL_VAL", None)
    tcpipDnscCacheTtlTime.setLabel("Default TTL time in Seconds")
    tcpipDnscCacheTtlTime.setVisible(True)
    tcpipDnscCacheTtlTime.setDescription("Default TTL time in Seconds")
    tcpipDnscCacheTtlTime.setDefaultValue(1200)
    #tcpipDnscCacheTtlTime.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

    # Time-out for an Unsolved Entry in the Cache
    tcpipDnscCacheUnsolvedEntryTimeout = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_CACHE_UNSOLVED_ENTRY_TMO", None)
    tcpipDnscCacheUnsolvedEntryTimeout.setLabel("Time-out for an Unsolved Entry in the Cache")
    tcpipDnscCacheUnsolvedEntryTimeout.setVisible(True)
    tcpipDnscCacheUnsolvedEntryTimeout.setDescription("Time-out for an Unsolved Entry in the Cache")
    tcpipDnscCacheUnsolvedEntryTimeout.setDefaultValue(10)
    #tcpipDnscCacheUnsolvedEntryTimeout.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

    # Retry Lookup Host name after Time-out
    tcpipDnscLookupRetryTimeout = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_LOOKUP_RETRY_TMO", None)
    tcpipDnscLookupRetryTimeout.setLabel("Retry Lookup Host name after Time-out")
    tcpipDnscLookupRetryTimeout.setVisible(True)
    tcpipDnscLookupRetryTimeout.setDescription("Retry Lookup Host name after Time-out")
    tcpipDnscLookupRetryTimeout.setDefaultValue(5)
    #tcpipDnscLookupRetryTimeout.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

    # DNS Client Host Name Length
    tcpipDnscHostNameLenMax = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN", None)
    tcpipDnscHostNameLenMax.setLabel("Host Name Length")
    tcpipDnscHostNameLenMax.setVisible(True)
    tcpipDnscHostNameLenMax.setDescription("DNS Client Host Name Length")
    tcpipDnscHostNameLenMax.setDefaultValue(64)
    #tcpipDnscHostNameLenMax.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

    # Maximum Number of Interfaces Considered for DNS resolution
    tcpipDnscInterfaceMaxNum = tcpipDnsComponent.createIntegerSymbol("TCPIP_DNS_CLIENT_MAX_SELECT_INTERFACES", None)
    tcpipDnscInterfaceMaxNum.setLabel("Maximum Number of Interfaces Considered for DNS resolution")
    tcpipDnscInterfaceMaxNum.setVisible(True)
    tcpipDnscInterfaceMaxNum.setDescription("Maximum Number of Interfaces Considered for DNS resolution")
    tcpipDnscInterfaceMaxNum.setDefaultValue(4)
    #tcpipDnscInterfaceMaxNum.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

    # Delete Old Entries
    tcpipDnscDeleteOldEntry= tcpipDnsComponent.createBooleanSymbol("TCPIP_DNS_CLIENT_DELETE_OLD_ENTRIES", None)
    tcpipDnscDeleteOldEntry.setLabel("Delete Old Entries")
    tcpipDnscDeleteOldEntry.setVisible(True)
    tcpipDnscDeleteOldEntry.setDescription("Delete Old Entries")
    tcpipDnscDeleteOldEntry.setDefaultValue(True)
    #tcpipDnscDeleteOldEntry.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

    # Enable User Notification
    tcpipDnscUsrNotification= tcpipDnsComponent.createBooleanSymbol("TCPIP_DNS_CLIENT_USER_NOTIFICATION", None)
    tcpipDnscUsrNotification.setLabel("Enable User Notification")
    tcpipDnscUsrNotification.setVisible(True)
    tcpipDnscUsrNotification.setDescription("Enable User Notification")
    tcpipDnscUsrNotification.setDefaultValue(False)
    #tcpipDnscUsrNotification.setDependencies(tcpipDnsMenuVisible, ["TCPIP_USE_DNS_CLIENT"])

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
