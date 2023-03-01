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

    
def instantiateComponent(tcpipNdpComponent):
    print("TCPIP NDP Component")
    configName = Variables.get("__CONFIGURATION_NAME")  
    
    # Enable Neighbor Discovery Protocol Settings   
    tcpipNdp = tcpipNdpComponent.createBooleanSymbol("TCPIP_USE_NDP", None)
    tcpipNdp.setHelp("mcc_h3_ndp_configurations")
    tcpipNdp.setLabel("NDP")
    tcpipNdp.setVisible(False)
    tcpipNdp.setDescription("Enable Neighbor Discovery Protocol Settings")
    tcpipNdp.setDefaultValue(True)

    # Advanced Settings
    tcpipNdpAdvSettings = tcpipNdpComponent.createMenuSymbol("TCPIP_NDP_ADV_SETTING", None)
    tcpipNdpAdvSettings.setLabel("Advanced Settings")
    tcpipNdpAdvSettings.setDescription("Advanced Settings")
    tcpipNdpAdvSettings.setVisible(True)

    # NDP Task Timer in msec
    tcpipNdpTaskTimer = tcpipNdpComponent.createIntegerSymbol("TCPIP_NDP_TASK_TIMER_RATE", tcpipNdpAdvSettings)
    tcpipNdpTaskTimer.setHelp("mcc_h3_ndp_configurations")
    tcpipNdpTaskTimer.setLabel("NDP Task Rate (in msec)")
    tcpipNdpTaskTimer.setVisible(True)
    tcpipNdpTaskTimer.setDescription("NDP Task Timer in msec")
    tcpipNdpTaskTimer.setDefaultValue(32)
    
    # RTR Solicitation Delay in seconds
    tcpipNdpRtrDelay = tcpipNdpComponent.createIntegerSymbol("TCPIP_MAX_RTR_SOLICITATION_DELAY", tcpipNdpAdvSettings)
    tcpipNdpRtrDelay.setHelp("mcc_h3_ndp_configurations")
    tcpipNdpRtrDelay.setLabel("RTR Solicitation Delay (in sec)")
    tcpipNdpRtrDelay.setVisible(True)
    tcpipNdpRtrDelay.setDescription("RTR Solicitation Delay in seconds")
    tcpipNdpRtrDelay.setDefaultValue(1)

    # RTR Solicitation Interval in seconds
    tcpipNdpRtrSolicitInterval = tcpipNdpComponent.createIntegerSymbol("TCPIP_RTR_SOLICITATION_INTERVAL", tcpipNdpAdvSettings)
    tcpipNdpRtrSolicitInterval.setHelp("mcc_h3_ndp_configurations")
    tcpipNdpRtrSolicitInterval.setLabel("RTR Solicitation Interval (in sec)")
    tcpipNdpRtrSolicitInterval.setVisible(True)
    tcpipNdpRtrSolicitInterval.setDescription("RTR Solicitation Interval in seconds")
    tcpipNdpRtrSolicitInterval.setDefaultValue(4)

    # Maximum RTR Solicitations
    tcpipNdpRtrSolicitMax = tcpipNdpComponent.createIntegerSymbol("TCPIP_MAX_RTR_SOLICITATIONS", tcpipNdpAdvSettings)
    tcpipNdpRtrSolicitMax.setHelp("mcc_h3_ndp_configurations")
    tcpipNdpRtrSolicitMax.setLabel("Maximum RTR Solicitations")
    tcpipNdpRtrSolicitMax.setVisible(True)
    tcpipNdpRtrSolicitMax.setDescription("Maximum RTR Solicitations")
    tcpipNdpRtrSolicitMax.setDefaultValue(3)

    # Maximum Multicast Solicitations
    tcpipNdpMcastSolicitMax = tcpipNdpComponent.createIntegerSymbol("TCPIP_MAX_MULTICAST_SOLICIT", tcpipNdpAdvSettings)
    tcpipNdpMcastSolicitMax.setHelp("mcc_h3_ndp_configurations")
    tcpipNdpMcastSolicitMax.setLabel("Maximum Multicast Solicitations")
    tcpipNdpMcastSolicitMax.setVisible(True)
    tcpipNdpMcastSolicitMax.setDescription("Maximum Multicast Solicitations")
    tcpipNdpMcastSolicitMax.setDefaultValue(3)

    # Maximum Unicast Solicitations
    tcpipNdpUcastSolicitMax = tcpipNdpComponent.createIntegerSymbol("TCPIP_MAX_UNICAST_SOLICIT", tcpipNdpAdvSettings)
    tcpipNdpUcastSolicitMax.setHelp("mcc_h3_ndp_configurations")
    tcpipNdpUcastSolicitMax.setLabel("Maximum Unicast Solicitations")
    tcpipNdpUcastSolicitMax.setVisible(True)
    tcpipNdpUcastSolicitMax.setDescription("Maximum Unicast Solicitations")
    tcpipNdpUcastSolicitMax.setDefaultValue(3)

    # Maximum Anycast Delay Time in Seconds
    tcpipNdpMaxAnycastDelayTime = tcpipNdpComponent.createIntegerSymbol("TCPIP_MAX_ANYCAST_DELAY_TIME", tcpipNdpAdvSettings)
    tcpipNdpMaxAnycastDelayTime.setHelp("mcc_h3_ndp_configurations")
    tcpipNdpMaxAnycastDelayTime.setLabel("Maximum Anycast Delay Time (in sec)")
    tcpipNdpMaxAnycastDelayTime.setVisible(True)
    tcpipNdpMaxAnycastDelayTime.setDescription("Maximum Anycast Delay Time in Seconds")
    tcpipNdpMaxAnycastDelayTime.setDefaultValue(1)

    # Maximum Neighbor Advertisement Transmissions
    tcpipNdpMaxNeighbAdvTxn = tcpipNdpComponent.createIntegerSymbol("TCPIP_MAX_NEIGHBOR_ADVERTISEMENT", tcpipNdpAdvSettings)
    tcpipNdpMaxNeighbAdvTxn.setHelp("mcc_h3_ndp_configurations")
    tcpipNdpMaxNeighbAdvTxn.setLabel("Maximum Neighbor Advertisement Transmissions")
    tcpipNdpMaxNeighbAdvTxn.setVisible(True)
    tcpipNdpMaxNeighbAdvTxn.setDescription("Maximum Neighbor Advertisement Transmissions")
    tcpipNdpMaxNeighbAdvTxn.setDefaultValue(3)

    # Reachable Time in Seconds
    tcpipNdpReachTime = tcpipNdpComponent.createIntegerSymbol("TCPIP_REACHABLE_TIME", tcpipNdpAdvSettings)
    tcpipNdpReachTime.setHelp("mcc_h3_ndp_configurations")
    tcpipNdpReachTime.setLabel("Reachable Time (in sec)")
    tcpipNdpReachTime.setVisible(True)
    tcpipNdpReachTime.setDescription("Reachable Time in Seconds")
    tcpipNdpReachTime.setDefaultValue(30)

    # Re-transmission Timer in Seconds
    tcpipNdpRetxnTime = tcpipNdpComponent.createIntegerSymbol("TCPIP_RETRANS_TIMER", tcpipNdpAdvSettings)
    tcpipNdpRetxnTime.setHelp("mcc_h3_ndp_configurations")
    tcpipNdpRetxnTime.setLabel("Re-transmission Timer (in sec)")
    tcpipNdpRetxnTime.setVisible(True)
    tcpipNdpRetxnTime.setDescription("Re-transmission Timer in Seconds")
    tcpipNdpRetxnTime.setDefaultValue(1)

    # Delay First Probe Time in Seconds
    tcpipNdpDlyFirstProbeTime = tcpipNdpComponent.createIntegerSymbol("TCPIP_DELAY_FIRST_PROBE_TIME", tcpipNdpAdvSettings)
    tcpipNdpDlyFirstProbeTime.setHelp("mcc_h3_ndp_configurations")
    tcpipNdpDlyFirstProbeTime.setLabel("Delay First Probe Time (in sec)")
    tcpipNdpDlyFirstProbeTime.setVisible(True)
    tcpipNdpDlyFirstProbeTime.setDescription("Delay First Probe Time in Seconds")
    tcpipNdpDlyFirstProbeTime.setDefaultValue(5)

    # IPv6 Maximum Transmission Unit Increase Time-out in Seconds
    tcpipNdpMtuIncreaseTimeout = tcpipNdpComponent.createIntegerSymbol("TCPIP_IPV6_MTU_INCREASE_TIMEOUT", tcpipNdpAdvSettings)
    tcpipNdpMtuIncreaseTimeout.setHelp("mcc_h3_ndp_configurations")
    tcpipNdpMtuIncreaseTimeout.setLabel("IPv6 MTU Increase Time-out (in sec)")
    tcpipNdpMtuIncreaseTimeout.setVisible(True)
    tcpipNdpMtuIncreaseTimeout.setDescription("IPv6 Maximum Transmission Unit Increase Time-out in Seconds")
    tcpipNdpMtuIncreaseTimeout.setDefaultValue(600)

    #Add to system_config.h
    tcpipNdpHeaderFtl = tcpipNdpComponent.createFileSymbol(None, None)
    tcpipNdpHeaderFtl.setSourcePath("tcpip/config/ndp.h.ftl")
    tcpipNdpHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipNdpHeaderFtl.setMarkup(True)
    tcpipNdpHeaderFtl.setType("STRING")

    # Add ndp.c file
    tcpipNdpSourceFile = tcpipNdpComponent.createFileSymbol(None, None)
    tcpipNdpSourceFile.setSourcePath("tcpip/src/ndp.c")
    tcpipNdpSourceFile.setOutputName("ndp.c")
    tcpipNdpSourceFile.setOverwrite(True)
    tcpipNdpSourceFile.setDestPath("library/tcpip/src/")
    tcpipNdpSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipNdpSourceFile.setType("SOURCE")
    tcpipNdpSourceFile.setEnabled(True)

    

def tcpipNdpMenuVisible(symbol, event):
    if (event["value"] == True):
        print("TCPIP Menu Visible.")        
        symbol.setVisible(True)
    else:
        print("TCPIP Menu Invisible.")
        symbol.setVisible(False)
        
def tcpipNdpGenSourceFile(sourceFile, event):
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
    Database.setSymbolValue("tcpipNdp", "TCPIP_USE_NDP", False, 2)
