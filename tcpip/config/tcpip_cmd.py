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

def instantiateComponent(tcpipCmdComponent):
    print("TCPIP CMD Component")
    configName = Variables.get("__CONFIGURATION_NAME")
        
    # Use TCP/IP Commands
    tcpipCmd = tcpipCmdComponent.createBooleanSymbol("TCPIP_STACK_USE_COMMANDS", None)
    tcpipCmd.setHelp("mcc_h3_tcpipcmd_configurations")
    tcpipCmd.setLabel("Use TCP/IP Commands")
    tcpipCmd.setVisible(False)
    tcpipCmd.setDescription("Use TCP/IP Commands")
    tcpipCmd.setDefaultValue(True)

    # Enable Storage for Stack Commands
    tcpipCmdStorage = tcpipCmdComponent.createBooleanSymbol("TCPIP_STACK_COMMANDS_STORAGE_ENABLE", None)
    tcpipCmdStorage.setHelp("mcc_h3_tcpipcmd_configurations")
    tcpipCmdStorage.setLabel("Enable Storage for Stack Commands")
    tcpipCmdStorage.setVisible((Database.getSymbolValue("tcpipStack", "TCPIP_STACK_CONFIGURATION_SAVE_RESTORE") == True))
    tcpipCmdStorage.setDescription("Enable Storage for Stack Commands")
    tcpipCmdStorage.setDefaultValue(False)  
    tcpipCmdStorage.setDependencies(tcpipCmdStorageMenuVisible, ["tcpipStack.TCPIP_STACK_CONFIGURATION_SAVE_RESTORE"]) 

    #Add to system_config.h
    tcpipCmdHeaderFtl = tcpipCmdComponent.createFileSymbol(None, None)
    tcpipCmdHeaderFtl.setSourcePath("tcpip/config/tcpip_cmd.h.ftl")
    tcpipCmdHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipCmdHeaderFtl.setMarkup(True)
    tcpipCmdHeaderFtl.setType("STRING")

    # Add tcpip_commands.c file
    tcpipCmdSourceFile = tcpipCmdComponent.createFileSymbol(None, None)
    tcpipCmdSourceFile.setSourcePath("tcpip/src/tcpip_commands.c")
    tcpipCmdSourceFile.setOutputName("tcpip_commands.c")
    tcpipCmdSourceFile.setOverwrite(True)
    tcpipCmdSourceFile.setDestPath("library/tcpip/src/")
    tcpipCmdSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipCmdSourceFile.setType("SOURCE")
    tcpipCmdSourceFile.setEnabled(True)

# Enable Storage for Stack Commands option visible
def tcpipCmdStorageMenuVisible(symbol, event):  
    if (event["value"] == True):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)


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
    Database.setSymbolValue("tcpipCmd", "TCPIP_STACK_USE_COMMANDS", False, 2)
