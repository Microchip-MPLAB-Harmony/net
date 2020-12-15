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

    
def instantiateComponent(tcpipNdpComponent):
    print("TCPIP NDP Component")
    configName = Variables.get("__CONFIGURATION_NAME")  
    
    # Enable Neighbor Discovery Protocol Settings   
    tcpipNdp = tcpipNdpComponent.createBooleanSymbol("TCPIP_USE_NDP", None)
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
    tcpipNdpTaskTimer.setLabel("NDP Task Rate (in msec)")
    tcpipNdpTaskTimer.setVisible(True)
    tcpipNdpTaskTimer.setDescription("NDP Task Timer in msec")
    tcpipNdpTaskTimer.setDefaultValue(32)
    
    # RTR Solicitation Delay in seconds
    tcpipNdpRtrDelay = tcpipNdpComponent.createIntegerSymbol("TCPIP_MAX_RTR_SOLICITATION_DELAY", tcpipNdpAdvSettings)
    tcpipNdpRtrDelay.setLabel("RTR Solicitation Delay (in sec)")
    tcpipNdpRtrDelay.setVisible(True)
    tcpipNdpRtrDelay.setDescription("RTR Solicitation Delay in seconds")
    tcpipNdpRtrDelay.setDefaultValue(1)

    # RTR Solicitation Interval in seconds
    tcpipNdpRtrSolicitInterval = tcpipNdpComponent.createIntegerSymbol("TCPIP_RTR_SOLICITATION_INTERVAL", tcpipNdpAdvSettings)
    tcpipNdpRtrSolicitInterval.setLabel("RTR Solicitation Interval (in sec)")
    tcpipNdpRtrSolicitInterval.setVisible(True)
    tcpipNdpRtrSolicitInterval.setDescription("RTR Solicitation Interval in seconds")
    tcpipNdpRtrSolicitInterval.setDefaultValue(4)

    # Maximum RTR Solicitations
    tcpipNdpRtrSolicitMax = tcpipNdpComponent.createIntegerSymbol("TCPIP_MAX_RTR_SOLICITATIONS", tcpipNdpAdvSettings)
    tcpipNdpRtrSolicitMax.setLabel("Maximum RTR Solicitations")
    tcpipNdpRtrSolicitMax.setVisible(True)
    tcpipNdpRtrSolicitMax.setDescription("Maximum RTR Solicitations")
    tcpipNdpRtrSolicitMax.setDefaultValue(3)

    # Maximum Multicast Solicitations
    tcpipNdpMcastSolicitMax = tcpipNdpComponent.createIntegerSymbol("TCPIP_MAX_MULTICAST_SOLICIT", tcpipNdpAdvSettings)
    tcpipNdpMcastSolicitMax.setLabel("Maximum Multicast Solicitations")
    tcpipNdpMcastSolicitMax.setVisible(True)
    tcpipNdpMcastSolicitMax.setDescription("Maximum Multicast Solicitations")
    tcpipNdpMcastSolicitMax.setDefaultValue(3)

    # Maximum Unicast Solicitations
    tcpipNdpUcastSolicitMax = tcpipNdpComponent.createIntegerSymbol("TCPIP_MAX_UNICAST_SOLICIT", tcpipNdpAdvSettings)
    tcpipNdpUcastSolicitMax.setLabel("Maximum Unicast Solicitations")
    tcpipNdpUcastSolicitMax.setVisible(True)
    tcpipNdpUcastSolicitMax.setDescription("Maximum Unicast Solicitations")
    tcpipNdpUcastSolicitMax.setDefaultValue(3)

    # Maximum Anycast Delay Time in Seconds
    tcpipNdpMaxAnycastDelayTime = tcpipNdpComponent.createIntegerSymbol("TCPIP_MAX_ANYCAST_DELAY_TIME", tcpipNdpAdvSettings)
    tcpipNdpMaxAnycastDelayTime.setLabel("Maximum Anycast Delay Time (in sec)")
    tcpipNdpMaxAnycastDelayTime.setVisible(True)
    tcpipNdpMaxAnycastDelayTime.setDescription("Maximum Anycast Delay Time in Seconds")
    tcpipNdpMaxAnycastDelayTime.setDefaultValue(1)

    # Maximum Neighbor Advertisement Transmissions
    tcpipNdpMaxNeighbAdvTxn = tcpipNdpComponent.createIntegerSymbol("TCPIP_MAX_NEIGHBOR_ADVERTISEMENT", tcpipNdpAdvSettings)
    tcpipNdpMaxNeighbAdvTxn.setLabel("Maximum Neighbor Advertisement Transmissions")
    tcpipNdpMaxNeighbAdvTxn.setVisible(True)
    tcpipNdpMaxNeighbAdvTxn.setDescription("Maximum Neighbor Advertisement Transmissions")
    tcpipNdpMaxNeighbAdvTxn.setDefaultValue(3)

    # Reachable Time in Seconds
    tcpipNdpReachTime = tcpipNdpComponent.createIntegerSymbol("TCPIP_REACHABLE_TIME", tcpipNdpAdvSettings)
    tcpipNdpReachTime.setLabel("Reachable Time (in sec)")
    tcpipNdpReachTime.setVisible(True)
    tcpipNdpReachTime.setDescription("Reachable Time in Seconds")
    tcpipNdpReachTime.setDefaultValue(30)

    # Re-transmission Timer in Seconds
    tcpipNdpRetxnTime = tcpipNdpComponent.createIntegerSymbol("TCPIP_RETRANS_TIMER", tcpipNdpAdvSettings)
    tcpipNdpRetxnTime.setLabel("Re-transmission Timer (in sec)")
    tcpipNdpRetxnTime.setVisible(True)
    tcpipNdpRetxnTime.setDescription("Re-transmission Timer in Seconds")
    tcpipNdpRetxnTime.setDefaultValue(1)

    # Delay First Probe Time in Seconds
    tcpipNdpDlyFirstProbeTime = tcpipNdpComponent.createIntegerSymbol("TCPIP_DELAY_FIRST_PROBE_TIME", tcpipNdpAdvSettings)
    tcpipNdpDlyFirstProbeTime.setLabel("Delay First Probe Time (in sec)")
    tcpipNdpDlyFirstProbeTime.setVisible(True)
    tcpipNdpDlyFirstProbeTime.setDescription("Delay First Probe Time in Seconds")
    tcpipNdpDlyFirstProbeTime.setDefaultValue(5)

    # IPv6 Maximum Transmission Unit Increase Time-out in Seconds
    tcpipNdpMtuIncreaseTimeout = tcpipNdpComponent.createIntegerSymbol("TCPIP_IPV6_MTU_INCREASE_TIMEOUT", tcpipNdpAdvSettings)
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