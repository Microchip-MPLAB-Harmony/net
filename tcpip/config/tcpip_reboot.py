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

    
def instantiateComponent(tcpipRebootComponent):
    print("TCPIP FTP Server Component")
    configName = Variables.get("__CONFIGURATION_NAME")
        
    # Use Reboot Server
    tcpipReboot = tcpipRebootComponent.createBooleanSymbol("TCPIP_USE_REBOOT_SERVER", None)
    tcpipReboot.setHelp("mcc_h3_reboot_configurations")
    tcpipReboot.setLabel("Use Reboot Server")
    tcpipReboot.setVisible(False)
    tcpipReboot.setDescription("Use Reboot Server")
    tcpipReboot.setDefaultValue(True)
    tcpipReboot.setDependencies(tcpipRebootServerMenuVisible, ["tcpipIPv4.TCPIP_STACK_USE_IPV4", "tcpipUdp.TCPIP_USE_UDP"])

    # Allow Only Same Subnet
    tcpipRebootAllowSameSubnet = tcpipRebootComponent.createBooleanSymbol("TCPIP_REBOOT_SAME_SUBNET_ONLY", None)
    tcpipRebootAllowSameSubnet.setHelp("mcc_h3_reboot_configurations")
    tcpipRebootAllowSameSubnet.setLabel("Allow Only Same Subnet")
    tcpipRebootAllowSameSubnet.setVisible(True)
    tcpipRebootAllowSameSubnet.setDescription("Allow Only Same Subnet")
    tcpipRebootAllowSameSubnet.setDefaultValue(True)
    #tcpipRebootAllowSameSubnet.setDependencies(tcpipRebootMenuSingle, ["TCPIP_USE_REBOOT_SERVER"])

    # Reboot Message
    tcpipRebootMessage = tcpipRebootComponent.createStringSymbol("TCPIP_REBOOT_MESSAGE", None)
    tcpipRebootMessage.setHelp("mcc_h3_reboot_configurations")
    tcpipRebootMessage.setLabel("Reboot Message")
    tcpipRebootMessage.setVisible(True)
    tcpipRebootMessage.setDescription("Reboot Message")
    tcpipRebootMessage.setDefaultValue("MCHP Reboot")
    #tcpipRebootMessage.setDependencies(tcpipRebootMenuSingle, ["TCPIP_USE_REBOOT_SERVER"])

    # Reboot Server Tick Rate in ms
    tcpipRebootTskTickRate = tcpipRebootComponent.createIntegerSymbol("TCPIP_REBOOT_TASK_TICK_RATE", None)
    tcpipRebootTskTickRate.setHelp("mcc_h3_reboot_configurations")
    tcpipRebootTskTickRate.setLabel("Reboot Server Tick Rate - ms")
    tcpipRebootTskTickRate.setVisible(True)
    tcpipRebootTskTickRate.setDescription("Reboot Server Tick Rate in ms")
    tcpipRebootTskTickRate.setDefaultValue(1130)
    #tcpipRebootTskTickRate.setDependencies(tcpipRebootMenuSingle, ["TCPIP_USE_REBOOT_SERVER"])

    #Add to system_config.h
    tcpipRebootHeaderFtl = tcpipRebootComponent.createFileSymbol(None, None)
    tcpipRebootHeaderFtl.setSourcePath("tcpip/config/tcpip_reboot.h.ftl")
    tcpipRebootHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipRebootHeaderFtl.setMarkup(True)
    tcpipRebootHeaderFtl.setType("STRING")

    # Add tcpip_reboot.c file
    tcpipRebootSourceFile = tcpipRebootComponent.createFileSymbol(None, None)
    tcpipRebootSourceFile.setSourcePath("tcpip/src/tcpip_reboot.c")
    tcpipRebootSourceFile.setOutputName("tcpip_reboot.c")
    tcpipRebootSourceFile.setOverwrite(True)
    tcpipRebootSourceFile.setDestPath("library/tcpip/src/")
    tcpipRebootSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipRebootSourceFile.setType("SOURCE")
    tcpipRebootSourceFile.setEnabled(True)
    #tcpipRebootSourceFile.setDependencies(tcpipRebootGenSourceFile, ["TCPIP_USE_REBOOT_SERVER"])

# make Reboot Server option visible
def tcpipRebootServerMenuVisible(symbol, event):
    tcpipIPv4 = Database.getSymbolValue("tcpipIPv4","TCPIP_STACK_USE_IPV4")
    tcpipUdp = Database.getSymbolValue("tcpipUdp","TCPIP_USE_UDP")

    if(tcpipIPv4 and tcpipUdp):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)
        
def tcpipRebootMenuSingle(symbol, event):
    if (event["value"] == True):
        print("Reboot Menu Visible.")       
        symbol.setVisible(True)
    else:
        print("Reboot Menu Invisible.")
        symbol.setVisible(False)
        
        
def tcpipRebootGenSourceFile(sourceFile, event):
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
    Database.setSymbolValue("tcpipReboot", "TCPIP_USE_REBOOT_SERVER", False, 2)
