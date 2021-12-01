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

def instantiateComponent(tcpipArpComponent):
    print("TCPIP ARP Component")
    configName = Variables.get("__CONFIGURATION_NAME")
        
    # ARP Enable 
    tcpipArp = tcpipArpComponent.createBooleanSymbol("TCPIP_USE_ARP", None)
    tcpipArp.setHelp("mcc_h3_arp_configurations")
    tcpipArp.setLabel("Use ARP?")
    tcpipArp.setDescription("Enables ARP")
    tcpipArp.setDefaultValue(True)
    tcpipArp.setVisible(False)
    #tcpipArp.setDependencies(tcpipArpEnable, ["tcpipIPv4.TCPIP_STACK_USE_IPV4"])
    
    # Cache Entries for this Interface
    tcpipArpCacheEntries = tcpipArpComponent.createIntegerSymbol("TCPIP_ARP_CACHE_ENTRIES", None)
    tcpipArpCacheEntries.setHelp("mcc_h3_arp_configurations")
    tcpipArpCacheEntries.setLabel("Number of Cache Entries")
    tcpipArpCacheEntries.setVisible(True)
    tcpipArpCacheEntries.setDescription("Cache Entries for this Interface")
    tcpipArpCacheEntries.setDefaultValue(5)
    tcpipArpCacheEntries.setDependencies(tcpipArpMenuVisibleSingle, ["TCPIP_USE_ARP"])
    
    # Enable ARP Commands
    tcpipArpCommands = tcpipArpComponent.createBooleanSymbol("TCPIP_ARP_COMMANDS_ENABLE", None)
    tcpipArpCommands.setHelp("mcc_h3_arp_configurations")
    tcpipArpCommands.setLabel("Enable ARP Commands")
    tcpipArpCommands.setVisible(True)
    tcpipArpCommands.setDescription("Enable the ARP TCP/IP Commands")
    tcpipArpCommands.setDefaultValue(False)

    # Advanced Settings
    tcpipArpAdvSettings = tcpipArpComponent.createMenuSymbol("TCPIP_ARP_ADV_SETTING", None)
    tcpipArpAdvSettings.setLabel("Advanced Settings")
    tcpipArpAdvSettings.setDescription("Advanced Settings")
    tcpipArpAdvSettings.setVisible(True)

    # Delete Old Cache if Still in Place    
    tcpipArpOldCacheDelete = tcpipArpComponent.createBooleanSymbol("TCPIP_ARP_CACHE_DELETE_OLD", tcpipArpAdvSettings)
    tcpipArpOldCacheDelete.setHelp("mcc_h3_arp_configurations")
    tcpipArpOldCacheDelete.setLabel("Delete Old Cache")
    tcpipArpOldCacheDelete.setVisible(True)
    tcpipArpOldCacheDelete.setDescription("Delete Old Cache if Still in Place")
    tcpipArpOldCacheDelete.setDefaultValue(True)
    tcpipArpOldCacheDelete.setDependencies(tcpipArpMenuVisibleSingle, ["TCPIP_USE_ARP"])

    # Time-Out for a Solved Entry to be Removed if not Referenced in Seconds
    tcpipArpCacheSolvedEntryTimeout = tcpipArpComponent.createIntegerSymbol("TCPIP_ARP_CACHE_SOLVED_ENTRY_TMO", tcpipArpAdvSettings)
    tcpipArpCacheSolvedEntryTimeout.setHelp("mcc_h3_arp_configurations")
    tcpipArpCacheSolvedEntryTimeout.setLabel("Time-Out for removal of Solved Entry (in sec)")
    tcpipArpCacheSolvedEntryTimeout.setVisible(True)
    tcpipArpCacheSolvedEntryTimeout.setDescription("Time-Out for a Solved Entry to be Removed if not Referenced in Seconds")
    tcpipArpCacheSolvedEntryTimeout.setDefaultValue(1200)
    tcpipArpCacheSolvedEntryTimeout.setDependencies(tcpipArpMenuVisibleSingle, ["TCPIP_USE_ARP"])

    # Time-Out for a Pending Cache Entry to be Solved in Seconds
    tcpipArpCachePendingEntryTimeout = tcpipArpComponent.createIntegerSymbol("TCPIP_ARP_CACHE_PENDING_ENTRY_TMO", tcpipArpAdvSettings)
    tcpipArpCachePendingEntryTimeout.setHelp("mcc_h3_arp_configurations")
    tcpipArpCachePendingEntryTimeout.setLabel("Time-Out to solve Pending Cache Entry (in sec)")
    tcpipArpCachePendingEntryTimeout.setVisible(True)
    tcpipArpCachePendingEntryTimeout.setDescription("Time-Out for a Pending Cache Entry to be Solved in Seconds")
    tcpipArpCachePendingEntryTimeout.setDefaultValue(60)
    tcpipArpCachePendingEntryTimeout.setDependencies(tcpipArpMenuVisibleSingle, ["TCPIP_USE_ARP"])

    # Time-Out for Resending an ARP Request for a Pending Entry in Seconds
    tcpipArpCachePendingRetryTimeout = tcpipArpComponent.createIntegerSymbol("TCPIP_ARP_CACHE_PENDING_RETRY_TMO", tcpipArpAdvSettings)
    tcpipArpCachePendingRetryTimeout.setHelp("mcc_h3_arp_configurations")
    tcpipArpCachePendingRetryTimeout.setLabel("Time-Out for Resending an ARP Request for Pending Entry (in sec)")
    tcpipArpCachePendingRetryTimeout.setVisible(True)
    tcpipArpCachePendingRetryTimeout.setDescription("Time-Out for Resending an ARP Request for a Pending Entry in Seconds")
    tcpipArpCachePendingRetryTimeout.setDefaultValue(2)
    tcpipArpCachePendingRetryTimeout.setDependencies(tcpipArpMenuVisibleSingle, ["TCPIP_USE_ARP"])

    # Number of Retries for Resolving an Entry
    tcpipArpCacheEntryRetryNum = tcpipArpComponent.createIntegerSymbol("TCPIP_ARP_CACHE_ENTRY_RETRIES", tcpipArpAdvSettings)
    tcpipArpCacheEntryRetryNum.setHelp("mcc_h3_arp_configurations")
    tcpipArpCacheEntryRetryNum.setLabel("Number of Retries for Resolving an Entry")
    tcpipArpCacheEntryRetryNum.setVisible(True)
    tcpipArpCacheEntryRetryNum.setDescription("Number of Retries for Resolving an Entry")
    tcpipArpCacheEntryRetryNum.setDefaultValue(3)
    tcpipArpCacheEntryRetryNum.setDependencies(tcpipArpMenuVisibleSingle, ["TCPIP_USE_ARP"])

    # Number of Retries Done for a Gratuitous ARP Request
    tcpipArpGratuitousRetryNum = tcpipArpComponent.createIntegerSymbol("TCPIP_ARP_GRATUITOUS_PROBE_COUNT", tcpipArpAdvSettings)
    tcpipArpGratuitousRetryNum.setHelp("mcc_h3_arp_configurations")
    tcpipArpGratuitousRetryNum.setLabel("Number of Retries for Gratuitous ARP Request")
    tcpipArpGratuitousRetryNum.setVisible(True)
    tcpipArpGratuitousRetryNum.setDescription("Number of Retries Done for a Gratuitous ARP Request")
    tcpipArpGratuitousRetryNum.setDefaultValue(1)
    tcpipArpGratuitousRetryNum.setDependencies(tcpipArpMenuVisibleSingle, ["TCPIP_USE_ARP"])

    # Purge Threshold Percentage
    tcpipArpCachePurgeThreshold = tcpipArpComponent.createIntegerSymbol("TCPIP_ARP_CACHE_PURGE_THRESHOLD", tcpipArpAdvSettings)
    tcpipArpCachePurgeThreshold.setHelp("mcc_h3_arp_configurations")
    tcpipArpCachePurgeThreshold.setLabel("Purge Threshold Percentage")
    tcpipArpCachePurgeThreshold.setVisible(True)
    tcpipArpCachePurgeThreshold.setDescription("Purge Threshold Percentage")
    tcpipArpCachePurgeThreshold.setDefaultValue(75)
    tcpipArpCachePurgeThreshold.setDependencies(tcpipArpMenuVisibleSingle, ["TCPIP_USE_ARP"])
    
    # Number of Entries to Delete Once the Threshold is Reached
    tcpipArpCachePurgeQuantaNum = tcpipArpComponent.createIntegerSymbol("TCPIP_ARP_CACHE_PURGE_QUANTA", tcpipArpAdvSettings)
    tcpipArpCachePurgeQuantaNum.setHelp("mcc_h3_arp_configurations")
    tcpipArpCachePurgeQuantaNum.setLabel("Number of Entries to Delete Once the Threshold is Reached")
    tcpipArpCachePurgeQuantaNum.setVisible(True)
    tcpipArpCachePurgeQuantaNum.setDescription("Number of Entries to Delete Once the Threshold is Reached")
    tcpipArpCachePurgeQuantaNum.setDefaultValue(1)
    tcpipArpCachePurgeQuantaNum.setDependencies(tcpipArpMenuVisibleSingle, ["TCPIP_USE_ARP"])

    # Maximum Percentage of Permanent Entries Allowed in the Cache
    tcpipArpCachePermQuota = tcpipArpComponent.createIntegerSymbol("TCPIP_ARP_CACHE_PERMANENT_QUOTA", tcpipArpAdvSettings)
    tcpipArpCachePermQuota.setHelp("mcc_h3_arp_configurations")
    tcpipArpCachePermQuota.setLabel("Maximum Percentage of Permanent Entries Allowed in Cache")
    tcpipArpCachePermQuota.setVisible(True)
    tcpipArpCachePermQuota.setDescription("Maximum Percentage of Permanent Entries Allowed in the Cache")
    tcpipArpCachePermQuota.setDefaultValue(50)
    tcpipArpCachePermQuota.setDependencies(tcpipArpMenuVisibleSingle, ["TCPIP_USE_ARP"])
    
    # Alias Interfaces Share One Cache  
    tcpipArpPrimaryCacheOnly = tcpipArpComponent.createBooleanSymbol("TCPIP_ARP_PRIMARY_CACHE_ONLY", tcpipArpAdvSettings)
    tcpipArpPrimaryCacheOnly.setHelp("mcc_h3_arp_configurations")
    tcpipArpPrimaryCacheOnly.setLabel("Alias Interfaces Share One Cache")
    tcpipArpPrimaryCacheOnly.setVisible(True)
    tcpipArpPrimaryCacheOnly.setDescription("Alias Interfaces Share One Cache")
    tcpipArpPrimaryCacheOnly.setDefaultValue(True)
    tcpipArpPrimaryCacheOnly.setDependencies(tcpipArpMenuVisibleSingle, ["TCPIP_USE_ARP"])
        
    # ARP Task process Rate in msec
    tcpipArpTaskProcessRate = tcpipArpComponent.createIntegerSymbol("TCPIP_ARP_TASK_PROCESS_RATE", tcpipArpAdvSettings)
    tcpipArpTaskProcessRate.setHelp("mcc_h3_arp_configurations")
    tcpipArpTaskProcessRate.setLabel("ARP Task Rate (in msec)")
    tcpipArpTaskProcessRate.setVisible(True)
    tcpipArpTaskProcessRate.setDescription("ARP Task Process Rate in msec")
    tcpipArpTaskProcessRate.setDefaultValue(2000)
    tcpipArpTaskProcessRate.setDependencies(tcpipArpMenuVisibleSingle, ["TCPIP_USE_ARP"])
    
    tcpipArpheapdependency = ["tcpipNetConfig.TCPIP_STACK_NETWORK_INTERAFCE_COUNT", "TCPIP_ARP_CACHE_ENTRIES", "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]    
        
    # ARP Heap Size
    tcpipArpHeapSize = tcpipArpComponent.createIntegerSymbol("TCPIP_ARP_HEAP_SIZE", None)
    tcpipArpHeapSize.setLabel("ARP Heap Size (bytes)") 
    tcpipArpHeapSize.setVisible(False)
    tcpipArpHeapSize.setDefaultValue(tcpipArpHeapCalc())
    tcpipArpHeapSize.setReadOnly(True)
    tcpipArpHeapSize.setDependencies(tcpipArpHeapUpdate, tcpipArpheapdependency)    
    
    #Add to system_config.h
    tcpipArpHeaderFtl = tcpipArpComponent.createFileSymbol(None, None)
    tcpipArpHeaderFtl.setSourcePath("tcpip/config/arp.h.ftl")
    tcpipArpHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipArpHeaderFtl.setMarkup(True)
    tcpipArpHeaderFtl.setType("STRING")

    # Add arp.c file
    tcpipArpSourceFile = tcpipArpComponent.createFileSymbol(None, None)
    tcpipArpSourceFile.setSourcePath("tcpip/src/arp.c")
    tcpipArpSourceFile.setOutputName("arp.c")
    tcpipArpSourceFile.setOverwrite(True)
    tcpipArpSourceFile.setDestPath("library/tcpip/src/")
    tcpipArpSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipArpSourceFile.setType("SOURCE")
    tcpipArpSourceFile.setEnabled(True)
    tcpipArpSourceFile.setDependencies(tcpipArpGenSourceFile, ["TCPIP_USE_ARP"])

def tcpipArpMenuVisibleSingle(symbol, event):
    if (event["value"] == True):
        print("ARP Menu Visible.")      
        symbol.setVisible(True)
    else:
        print("ARP Menu Invisible.")
        symbol.setVisible(False)


def tcpipArpEnable(symbol, event):
    if (event["value"] == True):
        print("ARP Enable")     
        symbol.setValue(True, 1)
    else:
        print("ARP Disable.")
        symbol.setValue(False, 1)       
        
def tcpipArpGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])

def tcpipArpHeapCalc(): 
    numInterface = Database.getSymbolValue("tcpipNetConfig","TCPIP_STACK_NETWORK_INTERAFCE_COUNT")
    cacheEntries = Database.getSymbolValue("tcpipArp","TCPIP_ARP_CACHE_ENTRIES")
    heap_size = 0
    if (numInterface != None):
        heap_size = numInterface * (102 + (cacheEntries * 24))
   
    return heap_size    
    
def tcpipArpHeapUpdate(symbol, event): 
    heap_size = tcpipArpHeapCalc()
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
    Database.setSymbolValue("tcpipArp", "TCPIP_USE_ARP", False, 2)
