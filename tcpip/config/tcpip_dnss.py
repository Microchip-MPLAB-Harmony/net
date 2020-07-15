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


def instantiateComponent(tcpipDnssComponent):
    print("TCPIP DNS Client Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    
    processor = Variables.get("__PROCESSOR")
    # Enable DNS Server
    tcpipDnss = tcpipDnssComponent.createBooleanSymbol("TCPIP_USE_DNSS", None)
    tcpipDnss.setLabel("DNS Server")
    tcpipDnss.setVisible(False)
    tcpipDnss.setDescription("Enable DNS Server")
    tcpipDnss.setDefaultValue(True)

    # Default DNS Host Name Length
    tcpipDnssHostNameLen = tcpipDnssComponent.createIntegerSymbol("TCPIP_DNSS_HOST_NAME_LEN", None)
    tcpipDnssHostNameLen.setLabel("Default DNS Host Name Length")
    tcpipDnssHostNameLen.setVisible(True)
    tcpipDnssHostNameLen.setDescription("Default DNS Host Name Length")
    tcpipDnssHostNameLen.setDefaultValue(64)

    # Reply DNS Info with Board Info
    tcpipDnssReplyBoardAddr = tcpipDnssComponent.createBooleanSymbol("TCPIP_DNSS_REPLY_BOARD_ADDR", None)
    tcpipDnssReplyBoardAddr.setLabel("Reply DNS Info with Board Info")
    tcpipDnssReplyBoardAddr.setVisible(True)
    tcpipDnssReplyBoardAddr.setDescription("Reply DNS Info with Board Info")
    tcpipDnssReplyBoardAddr.setDefaultValue(True)

    # Maximum Number of IPv4 Entries
    tcpipDnssIpv4EntryMaxNum = tcpipDnssComponent.createIntegerSymbol("TCPIP_DNSS_CACHE_PER_IPV4_ADDRESS", None)
    tcpipDnssIpv4EntryMaxNum.setLabel("Maximum Number of IPv4 Cache Entries")
    tcpipDnssIpv4EntryMaxNum.setVisible(True)
    tcpipDnssIpv4EntryMaxNum.setDescription("Maximum Number of IPv4 Entries")
    tcpipDnssIpv4EntryMaxNum.setDefaultValue(2)

    # Maximum Number of IPv6 Entries
    tcpipDnssIpv6EntryMaxNum = tcpipDnssComponent.createIntegerSymbol("TCPIP_DNSS_CACHE_PER_IPV6_ADDRESS", None)
    tcpipDnssIpv6EntryMaxNum.setLabel("Maximum Number of IPv6 Cache Entries")
    tcpipDnssIpv6EntryMaxNum.setVisible(False)
    tcpipDnssIpv6EntryMaxNum.setDescription("Maximum Number of IPv6 Entries")
    tcpipDnssIpv6EntryMaxNum.setDefaultValue(1)
    tcpipDnssIpv6EntryMaxNum.setDependencies(tcpipDnssIpv6EntryVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])
    
    # Default TTL Time for an IP Address in Seconds
    tcpipDnssTtlTime = tcpipDnssComponent.createIntegerSymbol("TCPIP_DNSS_TTL_TIME", None)
    tcpipDnssTtlTime.setLabel("Default TTL Time for an IP Address (in sec) ")
    tcpipDnssTtlTime.setVisible(True)
    tcpipDnssTtlTime.setDescription("Default TTL Time for an IP Address in Seconds")
    tcpipDnssTtlTime.setDefaultValue(600)

    # DNS Server Time-Out Task Processing Rate in Milliseconds
    tcpipDnssTskProcessRate = tcpipDnssComponent.createIntegerSymbol("TCPIP_DNSS_TASK_PROCESS_RATE", None)
    tcpipDnssTskProcessRate.setLabel("DNS Server Time-Out Task Processing Rate (in msec)")
    tcpipDnssTskProcessRate.setVisible(True)
    tcpipDnssTskProcessRate.setDescription("DNS Server Time-Out Task Processing Rate in Milliseconds")
    tcpipDnssTskProcessRate.setDefaultValue(33)


    # Delete Old Cache if Still in Place
    tcpipDnssDeleteOldLease = tcpipDnssComponent.createBooleanSymbol("TCPIP_DNSS_DELETE_OLD_LEASE", None)
    tcpipDnssDeleteOldLease.setLabel("Delete Old Cache if Still in Place")
    tcpipDnssDeleteOldLease.setVisible(True)
    tcpipDnssDeleteOldLease.setDescription("Delete Old Cache if Still in Place")
    tcpipDnssDeleteOldLease.setDefaultValue(True)

    tcpipDnssheapdependency = [ "tcpipIPv6.TCPIP_STACK_USE_IPV6", "TCPIP_DNSS_CACHE_PER_IPV4_ADDRESS", 
                                "TCPIP_DNSS_CACHE_PER_IPV6_ADDRESS", "TCPIP_DNSS_HOST_NAME_LEN", 
                                "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]
        
    # DNS Server Heap Size
    tcpipDnssHeapSize = tcpipDnssComponent.createIntegerSymbol("TCPIP_DNSS_HEAP_SIZE", None)
    tcpipDnssHeapSize.setLabel("DNS Server Heap Size (bytes)") 
    tcpipDnssHeapSize.setVisible(False)
    tcpipDnssHeapSize.setDefaultValue(tcpipDnssHeapCalc())
    tcpipDnssHeapSize.setReadOnly(True)
    tcpipDnssHeapSize.setDependencies(tcpipDnssHeapUpdate, tcpipDnssheapdependency)
    
    #Add to system_config.h
    tcpipDnssHeaderFtl = tcpipDnssComponent.createFileSymbol(None, None)
    tcpipDnssHeaderFtl.setSourcePath("tcpip/config/dnss.h.ftl")
    tcpipDnssHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipDnssHeaderFtl.setMarkup(True)
    tcpipDnssHeaderFtl.setType("STRING")

    # Add dnss.c file
    tcpipDnssSourceFile = tcpipDnssComponent.createFileSymbol(None, None)
    tcpipDnssSourceFile.setSourcePath("tcpip/src/dnss.c")
    tcpipDnssSourceFile.setOutputName("dnss.c")
    tcpipDnssSourceFile.setOverwrite(True)
    tcpipDnssSourceFile.setDestPath("library/tcpip/src/")
    tcpipDnssSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipDnssSourceFile.setType("SOURCE")
    tcpipDnssSourceFile.setEnabled(True)    
    

def tcpipDnssHeapCalc(): 
    IPv4EntriesPerDNSName = Database.getSymbolValue("tcpipDnss","TCPIP_DNSS_CACHE_PER_IPV4_ADDRESS")
    if((Database.getSymbolValue("tcpipIPv6", "TCPIP_STACK_USE_IPV6") == True)): 
        IPv6EntriesPerDNSName = Database.getSymbolValue("tcpipDnss","TCPIP_DNSS_CACHE_PER_IPV6_ADDRESS")
    else:
        IPv6EntriesPerDNSName = 0
    hostNameLen = Database.getSymbolValue("tcpipDnss","TCPIP_DNSS_HOST_NAME_LEN")
    
    heap_size = 40 + ((IPv4EntriesPerDNSName + IPv6EntriesPerDNSName) * (40 + (IPv4EntriesPerDNSName * 4) + (IPv6EntriesPerDNSName * 16)  + hostNameLen))
    return heap_size    
    
def tcpipDnssHeapUpdate(symbol, event): 
    heap_size = tcpipDnssHeapCalc()
    symbol.setValue(heap_size)
    if(event["id"] == "TCPIP_STACK_HEAP_CALC_MASK"):
        symbol.setVisible(event["value"])
    
def tcpipDnssMenuVisible(symbol, event):
    if (event["value"] == True):
        print("DNS Client Menu Visible.")       
        symbol.setVisible(True)
    else:
        print("DNS Client Menu Invisible.")
        symbol.setVisible(False)    

# make Maximum number of IPv6 entries visible under DNS Server
def tcpipDnssIpv6EntryVisible(symbol, event):
    if (event["value"] == True):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)
        
def tcpipDnssGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])

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
    Database.setSymbolValue("tcpipDnss", "TCPIP_USE_DNSS", False, 2)    
