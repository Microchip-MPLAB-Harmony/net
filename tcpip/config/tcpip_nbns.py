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

def instantiateComponent(tcpipNbnsComponent):
    print("TCPIP NBNS Component")
    configName = Variables.get("__CONFIGURATION_NAME")
        
    # Use NetBIOS Name Server
    tcpipNbns = tcpipNbnsComponent.createBooleanSymbol("TCPIP_USE_NBNS", None)
    tcpipNbns.setHelp("mcc_h3_nbns_configurations")
    tcpipNbns.setLabel("Use NetBIOS Name Server")
    tcpipNbns.setVisible(False)
    tcpipNbns.setDescription("Use NetBIOS Name Server")
    tcpipNbns.setDefaultValue(True)
        
    # Advanced Settings
    tcpipNbnsAdvSettings = tcpipNbnsComponent.createMenuSymbol("TCPIP_NBNS_ADV_SETTING", None)
    tcpipNbnsAdvSettings.setLabel("Advanced Settings")
    tcpipNbnsAdvSettings.setDescription("Advanced Settings")
    tcpipNbnsAdvSettings.setVisible(True)
    
    # NetBIOS Task Rate in ms
    tcpipNbnsTskTickRate = tcpipNbnsComponent.createIntegerSymbol("TCPIP_NBNS_TASK_TICK_RATE", tcpipNbnsAdvSettings)
    tcpipNbnsTskTickRate.setHelp("mcc_h3_nbns_configurations")
    tcpipNbnsTskTickRate.setLabel("NetBIOS Task Rate (in msec)")
    tcpipNbnsTskTickRate.setVisible(True)
    tcpipNbnsTskTickRate.setDescription("NetBIOS Task Rate in ms")
    tcpipNbnsTskTickRate.setDefaultValue(110)

    #Add to system_config.h
    tcpipNbnsHeaderFtl = tcpipNbnsComponent.createFileSymbol(None, None)
    tcpipNbnsHeaderFtl.setSourcePath("tcpip/config/nbns.h.ftl")
    tcpipNbnsHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipNbnsHeaderFtl.setMarkup(True)
    tcpipNbnsHeaderFtl.setType("STRING")

    # Add nbns.c file
    tcpipNbnsSourceFile = tcpipNbnsComponent.createFileSymbol(None, None)
    tcpipNbnsSourceFile.setSourcePath("tcpip/src/nbns.c")
    tcpipNbnsSourceFile.setOutputName("nbns.c")
    tcpipNbnsSourceFile.setOverwrite(True)
    tcpipNbnsSourceFile.setDestPath("library/tcpip/src/")
    tcpipNbnsSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipNbnsSourceFile.setType("SOURCE")
    tcpipNbnsSourceFile.setEnabled(True)

# make Nbns Server option visible
def tcpipNbnsMenuVisible(symbol, event):
    tcpipIPv4 = Database.getSymbolValue("tcpipIPv4","TCPIP_STACK_USE_IPV4")
    tcpipUdp = Database.getSymbolValue("tcpipUdp","TCPIP_USE_UDP")

    if(tcpipIPv4 and tcpipUdp):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)
        
def tcpipNbnsMenuVisibleSingle(symbol, event):
    if (event["value"] == True):
        print("NBNS Menu Visible.")     
        testname =Variables.get("__PROCESSOR") 
        print(testname)
        symbol.setVisible(True)
    else:
        print("NBNS Menu Invisible.")
        symbol.setVisible(False)

def tcpipNbnsGenSourceFile(sourceFile, event):
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
    Database.setSymbolValue("tcpipNbns", "TCPIP_USE_NBNS", False, 2)
