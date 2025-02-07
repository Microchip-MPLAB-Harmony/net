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
TCPIP_SNTP_PROTOCOL_TYPE = ["SNTP_VERSION_3", "SNTP_VERSION_4"]
TCPIP_ADDRESS_TYPE_STRICT = ["IP_ADDRESS_TYPE_IPV4", "IP_ADDRESS_TYPE_IPV6"]

def instantiateComponent(tcpipSntpComponent):
    print("TCPIP SNTP Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # Use SNTP Client Needed
    tcpipSntpClientNeeded = tcpipSntpComponent.createBooleanSymbol("TCPIP_USE_SNTP_CLIENT_NEEDED", None)
    tcpipSntpClientNeeded.setVisible(False)
    tcpipSntpClientNeeded.setDescription("Use SNTP Client")
    tcpipSntpClientNeeded.setDefaultValue(True) 

    # Use SNTP Client
    tcpipSntpClient = tcpipSntpComponent.createBooleanSymbol("TCPIP_USE_SNTP_CLIENT", None)
    tcpipSntpClient.setHelp("mcc_h3_sntp_configurations")
    tcpipSntpClient.setLabel("Use SNTP Client")
    tcpipSntpClient.setVisible(False)
    tcpipSntpClient.setDescription("Use SNTP Client")
    tcpipSntpClient.setDefaultValue(True) 

    # SNTP Server Name
    tcpipSntpSrvrName = tcpipSntpComponent.createStringSymbol("TCPIP_NTP_SERVER", None)
    tcpipSntpSrvrName.setHelp("mcc_h3_sntp_configurations")
    tcpipSntpSrvrName.setLabel("NTP Server Name")
    tcpipSntpSrvrName.setVisible(True)
    tcpipSntpSrvrName.setDescription("SNTP Server Name")
    tcpipSntpSrvrName.setDefaultValue("pool.ntp.org")
    
    # Default Connection Type
    tcpipSntpConnType = tcpipSntpComponent.createComboSymbol("TCPIP_NTP_DEFAULT_CONNECTION_TYPE", None, TCPIP_ADDRESS_TYPE_STRICT)
    tcpipSntpConnType.setHelp("mcc_h3_sntp_configurations")
    tcpipSntpConnType.setLabel("Default Connection Type")
    tcpipSntpConnType.setVisible(True)
    tcpipSntpConnType.setDescription("Default Connection Type")
    tcpipSntpConnType.setDefaultValue("IP_ADDRESS_TYPE_IPV4")
        
    # Advanced Settings
    tcpipSntpAdvSettings = tcpipSntpComponent.createMenuSymbol("TCPIP_NTP_ADV_SETTING", None)
    tcpipSntpAdvSettings.setLabel("Advanced Settings")
    tcpipSntpAdvSettings.setDescription("Advanced Settings")
    tcpipSntpAdvSettings.setVisible(True)
    
    # Task Tick Rate in ms
    tcpipSntpTskTickRate = tcpipSntpComponent.createIntegerSymbol("TCPIP_NTP_TASK_TICK_RATE", tcpipSntpAdvSettings)
    tcpipSntpTskTickRate.setHelp("mcc_h3_sntp_configurations")
    tcpipSntpTskTickRate.setLabel("SNTP Task Rate (in msec)")
    tcpipSntpTskTickRate.setVisible(True)
    tcpipSntpTskTickRate.setDescription("Task Tick Rate in ms")
    tcpipSntpTskTickRate.setDefaultValue(1100)


    # RX Queue Limit
    tcpipSntpRxQueueLimit = tcpipSntpComponent.createIntegerSymbol("TCPIP_NTP_RX_QUEUE_LIMIT", tcpipSntpAdvSettings)
    tcpipSntpRxQueueLimit.setHelp("mcc_h3_sntp_configurations")
    tcpipSntpRxQueueLimit.setLabel("RX Queue Limit")
    tcpipSntpRxQueueLimit.setVisible(True)
    tcpipSntpRxQueueLimit.setDescription("RX Queue Limit")
    tcpipSntpRxQueueLimit.setDefaultValue(2)

    # Server Address Max Length
    tcpipSntpSrvrMaxLength = tcpipSntpComponent.createIntegerSymbol("TCPIP_NTP_SERVER_MAX_LENGTH", tcpipSntpAdvSettings)
    tcpipSntpSrvrMaxLength.setHelp("mcc_h3_sntp_configurations")
    tcpipSntpSrvrMaxLength.setLabel("Server Address Maximum Length")
    tcpipSntpSrvrMaxLength.setVisible(True)
    tcpipSntpSrvrMaxLength.setDescription("Server Address Max Length")
    tcpipSntpSrvrMaxLength.setDefaultValue(30)

    # Query Interval in seconds
    tcpipSntpQueryInterval = tcpipSntpComponent.createIntegerSymbol("TCPIP_NTP_QUERY_INTERVAL", tcpipSntpAdvSettings)
    tcpipSntpQueryInterval.setHelp("mcc_h3_sntp_configurations")
    tcpipSntpQueryInterval.setLabel("Query Interval (in sec)")
    tcpipSntpQueryInterval.setVisible(True)
    tcpipSntpQueryInterval.setDescription("Query Interval in seconds")
    tcpipSntpQueryInterval.setDefaultValue(600)

    # Fast Query Interval
    tcpipSntpFastQueryInterval = tcpipSntpComponent.createIntegerSymbol("TCPIP_NTP_FAST_QUERY_INTERVAL", tcpipSntpAdvSettings)
    tcpipSntpFastQueryInterval.setHelp("mcc_h3_sntp_configurations")
    tcpipSntpFastQueryInterval.setLabel("Fast Query Interval (in sec)")
    tcpipSntpFastQueryInterval.setVisible(True)
    tcpipSntpFastQueryInterval.setDescription("Fast Query Interval")
    tcpipSntpFastQueryInterval.setDefaultValue(14)

    # Sntp Epoch
    tcpipSntpEpoch = tcpipSntpComponent.createLongSymbol("TCPIP_NTP_EPOCH", tcpipSntpAdvSettings)
    tcpipSntpEpoch.setHelp("mcc_h3_sntp_configurations")
    tcpipSntpEpoch.setLabel("Epoch")
    tcpipSntpEpoch.setVisible(True)
    tcpipSntpEpoch.setDescription("Sntp Epoch")
    tcpipSntpEpoch.setDefaultValue(2208988800) 

    # SNTP Reply Time-out in Seconds
    tcpipSntpReplyTimeout = tcpipSntpComponent.createIntegerSymbol("TCPIP_NTP_REPLY_TIMEOUT", tcpipSntpAdvSettings)
    tcpipSntpReplyTimeout.setHelp("mcc_h3_sntp_configurations")
    tcpipSntpReplyTimeout.setLabel("NTP Reply Timeout (in sec)")
    tcpipSntpReplyTimeout.setVisible(True)
    tcpipSntpReplyTimeout.setDescription("SNTP Reply Time-out in Seconds")
    tcpipSntpReplyTimeout.setDefaultValue(6)

    # Maximum Stratum
    tcpipSntpStratumMax = tcpipSntpComponent.createIntegerSymbol("TCPIP_NTP_MAX_STRATUM", tcpipSntpAdvSettings)
    tcpipSntpStratumMax.setHelp("mcc_h3_sntp_configurations")
    tcpipSntpStratumMax.setLabel("Max Stratum")
    tcpipSntpStratumMax.setVisible(True)
    tcpipSntpStratumMax.setDescription("Maximum Stratum")
    tcpipSntpStratumMax.setDefaultValue(15)

    # Time Stamp Time-Out
    tcpipSntpTimestampTimeout = tcpipSntpComponent.createIntegerSymbol("TCPIP_NTP_TIME_STAMP_TMO", tcpipSntpAdvSettings)
    tcpipSntpTimestampTimeout.setHelp("mcc_h3_sntp_configurations")
    tcpipSntpTimestampTimeout.setLabel("Time Stamp Timeout (in sec)")
    tcpipSntpTimestampTimeout.setVisible(True)
    tcpipSntpTimestampTimeout.setDescription("Time Stamp Time-Out")
    tcpipSntpTimestampTimeout.setDefaultValue(660)

    # SNTP Version
    tcpipSntpVersion = tcpipSntpComponent.createComboSymbol("TCPIP_NTP_STRING_VERSION", tcpipSntpAdvSettings, TCPIP_SNTP_PROTOCOL_TYPE)
    tcpipSntpVersion.setHelp("mcc_h3_sntp_configurations")
    tcpipSntpVersion.setLabel("SNTP Version")
    tcpipSntpVersion.setVisible(True)
    tcpipSntpVersion.setDescription("SNTP Version")
    tcpipSntpVersion.setDefaultValue("SNTP_VERSION_4")
        
    # Default Interface
    tcpipSntpDefault = tcpipSntpComponent.createStringSymbol("TCPIP_NTP_DEFAULT_IF", tcpipSntpAdvSettings)  
    tcpipSntpDefault.setHelp("mcc_h3_sntp_configurations")
    tcpipSntpDefault.setLabel("Default Interface")
    tcpipSntpDefault.setVisible(True)
    tcpipSntpDefault.setDescription("Default Interface")
    # Set Interafce 0 as default interface
    if(Database.getSymbolValue("tcpipNetConfig_0", "TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX0") != None):
        tcpipSntpDefault.setDefaultValue(Database.getSymbolValue("tcpipNetConfig_0", "TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX0"))    
    tcpipSntpDefault.setDependencies(tcpipSntpInterface, ["tcpipNetConfig_0.TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX0"])
    
    #Add to system_config.h
    tcpipSntpHeaderFtl = tcpipSntpComponent.createFileSymbol(None, None)
    tcpipSntpHeaderFtl.setSourcePath("tcpip/config/sntp.h.ftl")
    tcpipSntpHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipSntpHeaderFtl.setMarkup(True)
    tcpipSntpHeaderFtl.setType("STRING")

    # Add sntp.c file
    tcpipSntpSourceFile = tcpipSntpComponent.createFileSymbol(None, None)
    tcpipSntpSourceFile.setSourcePath("tcpip/src/sntp.c")
    tcpipSntpSourceFile.setOutputName("sntp.c")
    tcpipSntpSourceFile.setOverwrite(True)
    tcpipSntpSourceFile.setDestPath("library/tcpip/src/")
    tcpipSntpSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipSntpSourceFile.setType("SOURCE")
    tcpipSntpSourceFile.setEnabled(True)
    tcpipSntpSourceFile.setDependencies(tcpipSntpGenSourceFile, ["TCPIP_USE_SNTP_CLIENT"])

        
def tcpipSntpMenuVisibleSingle(symbol, event):
    if (event["value"] == True):
        print("SNTP Menu Visible.")     
        symbol.setVisible(True)
    else:
        print("SNTP Menu Invisible.")
        symbol.setVisible(False)

def tcpipSntpInterface(symbol, event):    
    symbol.setValue(event["value"])
    
def tcpipSntpGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])

def onAttachmentConnected(source, target):
    if (source["id"] == "Sntp_MAC_Dependency"): 
        tcpipSntpIf = source["component"].getSymbolByID("TCPIP_NTP_DEFAULT_IF")
        tcpipSntpIf.clearValue()
        tcpipSntpIf.setValue(target["component"].getDisplayName(), 2)
        
def onAttachmentDisconnected(source, target):
    if (source["id"] == "Sntp_MAC_Dependency"): 
        source["component"].clearSymbolValue("TCPIP_NTP_DEFAULT_IF")    

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
    Database.setSymbolValue("tcpipSntp", "TCPIP_USE_SNTP_CLIENT", False, 2)
