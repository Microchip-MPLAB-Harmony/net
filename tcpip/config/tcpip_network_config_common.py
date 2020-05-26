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

tcpipNetConfigNumMaximum = 10

def instantiateComponent(tcpipNetConfigComponent):

    tcpipNetConfigNumMax = tcpipNetConfigComponent.createIntegerSymbol("TCPIP_STACK_NETWORK_CONFIG_NUMBER_MAX", None)
    tcpipNetConfigNumMax.setLabel("Number of Instances")
    tcpipNetConfigNumMax.setDefaultValue(tcpipNetConfigNumMaximum)
    tcpipNetConfigNumMax.setVisible(False)
    
    tcpipNetConfigInterfaceCount = tcpipNetConfigComponent.createIntegerSymbol("TCPIP_STACK_NETWORK_INTERAFCE_COUNT", None)
    tcpipNetConfigInterfaceCount.setLabel("Number of Instances")
    tcpipNetConfigInterfaceCount.setDefaultValue(1)
    tcpipNetConfigInterfaceCount.setVisible(True)
    tcpipNetConfigInterfaceCount.setReadOnly(True)

#Handle messages from other components
def handleMessage(messageID, args):
    retDict= {}
    if (messageID == "NETCONFIG_INTERFACE_COUNTER_INC"):
        netconfif_count = Database.getSymbolValue("tcpipNetConfig", "TCPIP_STACK_NETWORK_INTERAFCE_COUNT")
        Database.setSymbolValue("tcpipNetConfig", "TCPIP_STACK_NETWORK_INTERAFCE_COUNT", netconfif_count + 1)
        retDict= {"Return": "Success"}
    elif (messageID == "NETCONFIG_INTERFACE_COUNTER_DEC"):
        netconfif_count = Database.getSymbolValue("tcpipNetConfig", "TCPIP_STACK_NETWORK_INTERAFCE_COUNT")
        Database.setSymbolValue("tcpipNetConfig", "TCPIP_STACK_NETWORK_INTERAFCE_COUNT", netconfif_count - 1)
        retDict= {"Return": "Success"}
    elif (messageID == "SET_SYMBOL"):
        print "handleMessage: Set Symbol"
        retDict= {"Return": "Success"}
        Database.setSymbolValue(args["Component"], args["Id"], args["Value"])
    else:
        retDict= {"Return": "UnImplemented Command"}
        
    return retDict

#Set symbols of other components
def setVal(component, symbol, value):
    triggerDict = {"Component":component,"Id":symbol, "Value":value}
    if(Database.sendMessage(component, "SET_SYMBOL", triggerDict) == None):
        print "Set Symbol Failure" + component + ":" + symbol + ":" + str(value)
        return False
    else:
        return True

    