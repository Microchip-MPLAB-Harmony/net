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
    tcpipSntpClient.setLabel("Use SNTP Client")
    tcpipSntpClient.setVisible(False)
    tcpipSntpClient.setDescription("Use SNTP Client")
    tcpipSntpClient.setDefaultValue(True) 
    
    # Default Interface
    tcpipSntpDefault = tcpipSntpComponent.createStringSymbol("TCPIP_NTP_DEFAULT_IF", None)  
    tcpipSntpDefault.setLabel("Default Interface")
    tcpipSntpDefault.setVisible(True)
    tcpipSntpDefault.setDescription("Default Interface")
    
    if Peripheral.moduleExists("GMAC"):
        tcpipSntpDefault.setDefaultValue("GMAC")
    else:
        tcpipSntpDefault.setDefaultValue("PIC32INT")
    tcpipSntpDefault.setReadOnly(True)

    # SNTP Version
    tcpipSntpVersion = tcpipSntpComponent.createComboSymbol("TCPIP_NTP_STRING_VERSION", None, TCPIP_SNTP_PROTOCOL_TYPE)
    tcpipSntpVersion.setLabel("SNTP Version")
    tcpipSntpVersion.setVisible(True)
    tcpipSntpVersion.setDescription("SNTP Version")
    tcpipSntpVersion.setDefaultValue("SNTP_VERSION_4")

    # Default Connection Type
    tcpipSntpConnType = tcpipSntpComponent.createComboSymbol("TCPIP_NTP_DEFAULT_CONNECTION_TYPE", None, TCPIP_ADDRESS_TYPE_STRICT)
    tcpipSntpConnType.setLabel("Default Connection Type")
    tcpipSntpConnType.setVisible(True)
    tcpipSntpConnType.setDescription("Default Connection Type")
    tcpipSntpConnType.setDefaultValue("IP_ADDRESS_TYPE_IPV4")

    # SNTP Server Name
    tcpipSntpSrvrName = tcpipSntpComponent.createStringSymbol("TCPIP_NTP_SERVER", None)
    tcpipSntpSrvrName.setLabel("NTP Server Name")
    tcpipSntpSrvrName.setVisible(True)
    tcpipSntpSrvrName.setDescription("SNTP Server Name")
    tcpipSntpSrvrName.setDefaultValue("pool.ntp.org")
        
    # Advanced Settings
    tcpipSntpAdvSettings = tcpipSntpComponent.createMenuSymbol("TCPIP_NTP_ADV_SETTING", None)
    tcpipSntpAdvSettings.setLabel("Advanced Settings")
    tcpipSntpAdvSettings.setDescription("Advanced Settings")
    tcpipSntpAdvSettings.setVisible(True)
    
    # Task Tick Rate in ms
    tcpipSntpTskTickRate = tcpipSntpComponent.createIntegerSymbol("TCPIP_NTP_TASK_TICK_RATE", tcpipSntpAdvSettings)
    tcpipSntpTskTickRate.setLabel("SNTP Task Rate (in msec)")
    tcpipSntpTskTickRate.setVisible(True)
    tcpipSntpTskTickRate.setDescription("Task Tick Rate in ms")
    tcpipSntpTskTickRate.setDefaultValue(1100)


    # RX Queue Limit
    tcpipSntpRxQueueLimit = tcpipSntpComponent.createIntegerSymbol("TCPIP_NTP_RX_QUEUE_LIMIT", tcpipSntpAdvSettings)
    tcpipSntpRxQueueLimit.setLabel("RX Queue Limit")
    tcpipSntpRxQueueLimit.setVisible(True)
    tcpipSntpRxQueueLimit.setDescription("RX Queue Limit")
    tcpipSntpRxQueueLimit.setDefaultValue(2)

    # Server Address Max Length
    tcpipSntpSrvrMaxLength = tcpipSntpComponent.createIntegerSymbol("TCPIP_NTP_SERVER_MAX_LENGTH", tcpipSntpAdvSettings)
    tcpipSntpSrvrMaxLength.setLabel("Server Address Maximum Length")
    tcpipSntpSrvrMaxLength.setVisible(True)
    tcpipSntpSrvrMaxLength.setDescription("Server Address Max Length")
    tcpipSntpSrvrMaxLength.setDefaultValue(30)

    # Query Interval in seconds
    tcpipSntpQueryInterval = tcpipSntpComponent.createIntegerSymbol("TCPIP_NTP_QUERY_INTERVAL", tcpipSntpAdvSettings)
    tcpipSntpQueryInterval.setLabel("Query Interval (in sec)")
    tcpipSntpQueryInterval.setVisible(True)
    tcpipSntpQueryInterval.setDescription("Query Interval in seconds")
    tcpipSntpQueryInterval.setDefaultValue(600)

    # Fast Query Interval
    tcpipSntpFastQueryInterval = tcpipSntpComponent.createIntegerSymbol("TCPIP_NTP_FAST_QUERY_INTERVAL", tcpipSntpAdvSettings)
    tcpipSntpFastQueryInterval.setLabel("Fast Query Interval (in sec)")
    tcpipSntpFastQueryInterval.setVisible(True)
    tcpipSntpFastQueryInterval.setDescription("Fast Query Interval")
    tcpipSntpFastQueryInterval.setDefaultValue(14)

    # Sntp Epoch
    tcpipSntpEpoch = tcpipSntpComponent.createLongSymbol("TCPIP_NTP_EPOCH", tcpipSntpAdvSettings)
    tcpipSntpEpoch.setLabel("Epoch")
    tcpipSntpEpoch.setVisible(True)
    tcpipSntpEpoch.setDescription("Sntp Epoch")
    tcpipSntpEpoch.setDefaultValue(2208988800) 

    # SNTP Reply Time-out in Seconds
    tcpipSntpReplyTimeout = tcpipSntpComponent.createIntegerSymbol("TCPIP_NTP_REPLY_TIMEOUT", tcpipSntpAdvSettings)
    tcpipSntpReplyTimeout.setLabel("NTP Reply Timeout (in sec)")
    tcpipSntpReplyTimeout.setVisible(True)
    tcpipSntpReplyTimeout.setDescription("SNTP Reply Time-out in Seconds")
    tcpipSntpReplyTimeout.setDefaultValue(6)

    # Maximum Stratum
    tcpipSntpStratumMax = tcpipSntpComponent.createIntegerSymbol("TCPIP_NTP_MAX_STRATUM", tcpipSntpAdvSettings)
    tcpipSntpStratumMax.setLabel("Max Stratum")
    tcpipSntpStratumMax.setVisible(True)
    tcpipSntpStratumMax.setDescription("Maximum Stratum")
    tcpipSntpStratumMax.setDefaultValue(15)

    # Time Stamp Time-Out
    tcpipSntpTimestampTimeout = tcpipSntpComponent.createIntegerSymbol("TCPIP_NTP_TIME_STAMP_TMO", tcpipSntpAdvSettings)
    tcpipSntpTimestampTimeout.setLabel("Time Stamp Timeout (in )")
    tcpipSntpTimestampTimeout.setVisible(True)
    tcpipSntpTimestampTimeout.setDescription("Time Stamp Time-Out")
    tcpipSntpTimestampTimeout.setDefaultValue(660)
    
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
