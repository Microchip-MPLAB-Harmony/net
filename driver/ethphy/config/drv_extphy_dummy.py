"""
Copyright (C) 2019-2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

phyName = "Dummy"
def instantiateComponent(drvExtPhyDummyComponent):
    print("Dummy PHY Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")

    # Use a Function to be called at PHY Reset
    drvExtPhyDummyResetCallbackEnable = drvExtPhyDummyComponent.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
    drvExtPhyDummyResetCallbackEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDummyResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
    drvExtPhyDummyResetCallbackEnable.setVisible(True)
    drvExtPhyDummyResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
    drvExtPhyDummyResetCallbackEnable.setDefaultValue(False)
    
    # App Function
    drvExtPhyDummyResetCallback = drvExtPhyDummyComponent.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyDummyResetCallbackEnable)
    drvExtPhyDummyResetCallback.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDummyResetCallback.setLabel("App Function")
    drvExtPhyDummyResetCallback.setVisible(False)
    drvExtPhyDummyResetCallback.setDescription("App Function")
    drvExtPhyDummyResetCallback.setDefaultValue("App" + phyName + "ResetFunction")
    drvExtPhyDummyResetCallback.setDependencies(drvExtPhyDummyMenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])
    
    # Use a 'thin' dummy driver
    drvExtPhyDummyThinEnable = drvExtPhyDummyComponent.createBooleanSymbol("DRV_ETHPHY_USE_THIN_DRIVER", None)
    drvExtPhyDummyThinEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDummyThinEnable.setLabel("Use a thin PHY Driver")
    drvExtPhyDummyThinEnable.setVisible(True)
    drvExtPhyDummyThinEnable.setDescription("Use a very simple thin driver with no run-time checking")
    drvExtPhyDummyThinEnable.setDefaultValue(False)
    
    # External PHY Type
    drvExtPhyDummyPhyType = drvExtPhyDummyComponent.createStringSymbol("TCPIP_EMAC_PHY_TYPE", None)
    #drvExtPhyDummyPhyType.setLabel("External PHY Type")
    drvExtPhyDummyPhyType.setVisible(False)
    #drvExtPhyDummyPhyType.setDescription("External PHY Type")
    drvExtPhyDummyPhyType.setDefaultValue(phyName)
    
    # Advanced Settings
    drvExtPhyDummyAdvSettings = drvExtPhyDummyComponent.createMenuSymbol("TCPIP_INTMAC_PHY_ADV_SETTING", None)
    drvExtPhyDummyAdvSettings.setLabel("Advanced Settings")
    drvExtPhyDummyAdvSettings.setDescription("Advanced Settings")
    drvExtPhyDummyAdvSettings.setVisible(True)

    # Delay for the Link Initialization in ms
    drvExtPhyDummyLinkInitDelay= drvExtPhyDummyComponent.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", drvExtPhyDummyAdvSettings)
    drvExtPhyDummyLinkInitDelay.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDummyLinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
    drvExtPhyDummyLinkInitDelay.setVisible(True)
    drvExtPhyDummyLinkInitDelay.setDescription("Delay for the Link Initialization in ms")
    drvExtPhyDummyLinkInitDelay.setDefaultValue(500)
      
    # Driver PHY Instances Number
    drvExtPhyDummyInstanceNum= drvExtPhyDummyComponent.createIntegerSymbol("DRV_ETHPHY_INSTANCES_NUMBER", drvExtPhyDummyAdvSettings)
    drvExtPhyDummyInstanceNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDummyInstanceNum.setLabel("PHY Instances Number") 
    drvExtPhyDummyInstanceNum.setVisible(True)
    drvExtPhyDummyInstanceNum.setDescription("Driver PHY Instances Number")
    drvExtPhyDummyInstanceNum.setDefaultValue(1)
    drvExtPhyDummyInstanceNum.setReadOnly(True)
    
    # Driver PHY Clients Number
    drvExtPhyDummyClientNum= drvExtPhyDummyComponent.createIntegerSymbol("DRV_ETHPHY_CLIENTS_NUMBER", drvExtPhyDummyAdvSettings)
    drvExtPhyDummyClientNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDummyClientNum.setLabel("PHY Clients Number") 
    drvExtPhyDummyClientNum.setVisible(True)
    drvExtPhyDummyClientNum.setDescription("Driver PHY Clients Number")
    drvExtPhyDummyClientNum.setDefaultValue(1)
    drvExtPhyDummyClientNum.setReadOnly(True)
    
    # Driver PHY Peripheral Index Number
    drvExtPhyDummyIndexNum= drvExtPhyDummyComponent.createIntegerSymbol("DRV_ETHPHY_INDEX", drvExtPhyDummyAdvSettings)
    drvExtPhyDummyIndexNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDummyIndexNum.setLabel("PHY Peripheral Index Number") 
    drvExtPhyDummyIndexNum.setVisible(True)
    drvExtPhyDummyIndexNum.setDescription("Driver PHY Peripheral Index Number")
    drvExtPhyDummyIndexNum.setDefaultValue(1)
    drvExtPhyDummyIndexNum.setReadOnly(True)
    
    # Driver PHY Peripheral ID
    drvExtPhyDummyPeripheralId= drvExtPhyDummyComponent.createStringSymbol("DRV_ETHPHY_PERIPHERAL_ID", drvExtPhyDummyAdvSettings)
    drvExtPhyDummyPeripheralId.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDummyPeripheralId.setLabel("PHY Peripheral ID") 
    drvExtPhyDummyPeripheralId.setVisible(False)
    drvExtPhyDummyPeripheralId.setDescription("Driver PHY Peripheral ID")
    drvExtPhyDummyPeripheralId.setDefaultValue("")
    drvExtPhyDummyPeripheralId.setReadOnly(True)
    
    # External MAC Name
    drvExtPhyMacName= drvExtPhyDummyComponent.createStringSymbol("DRV_ETHPHY_MAC_NAME", None)
    drvExtPhyMacName.setLabel("Mac Name") 
    drvExtPhyMacName.setVisible(False)   
    drvExtPhyMacName.setDefaultValue("")
    drvExtPhyMacName.setReadOnly(True) 
                
    #Add forward declaration to initialization.c
    drvExtPhyDummyInitDataSourceFtl = drvExtPhyDummyComponent.createFileSymbol(None, None)
    drvExtPhyDummyInitDataSourceFtl.setType("STRING")
    drvExtPhyDummyInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    drvExtPhyDummyInitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_driver_initialize.c.ftl")
    drvExtPhyDummyInitDataSourceFtl.setMarkup(True)    
    
    #Add to initialization.c
    drvExtPhyDummySysInitDataSourceFtl = drvExtPhyDummyComponent.createFileSymbol(None, None)
    drvExtPhyDummySysInitDataSourceFtl.setType("STRING")
    drvExtPhyDummySysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    drvExtPhyDummySysInitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_data_initialize.c.ftl")
    drvExtPhyDummySysInitDataSourceFtl.setMarkup(True)    
    
    #Add to system_config.h
    drvExtPhyDummyHeaderFtl = drvExtPhyDummyComponent.createFileSymbol(None, None)
    drvExtPhyDummyHeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_dummy.h.ftl")
    drvExtPhyDummyHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    drvExtPhyDummyHeaderFtl.setMarkup(True)
    drvExtPhyDummyHeaderFtl.setType("STRING") 
    
    # file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
    # Add drv_ethphy.h file to project
    drvExtPhyHeaderFile = drvExtPhyDummyComponent.createFileSymbol(None, None)
    drvExtPhyHeaderFile.setSourcePath("driver/ethphy/drv_ethphy.h")
    drvExtPhyHeaderFile.setOutputName("drv_ethphy.h")
    drvExtPhyHeaderFile.setDestPath("driver/ethphy/")
    drvExtPhyHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/")
    drvExtPhyHeaderFile.setType("HEADER")
    drvExtPhyHeaderFile.setOverwrite(True)
    drvExtPhyHeaderFile.setEnabled(True)

    # file drv_extphy_dummy.h "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_dummy.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_dummy.h"
    # Add drv_extphy_dummy.h file to project
    drvExtPhyDummyHeaderFile = drvExtPhyDummyComponent.createFileSymbol(None, None)
    drvExtPhyDummyHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_dummy.h")
    drvExtPhyDummyHeaderFile.setOutputName("drv_extphy_dummy.h")
    drvExtPhyDummyHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyDummyHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyDummyHeaderFile.setType("HEADER")
    drvExtPhyDummyHeaderFile.setOverwrite(True)
    drvExtPhyDummyHeaderFile.setEnabled(True)


    # file drv_extphy_dummy.c "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_dummy.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_dummy.c"
    # Add drv_extphy_dummy.c file
    drvExtPhyDummySourceFile = drvExtPhyDummyComponent.createFileSymbol(None, None)
    drvExtPhyDummySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_dummy.c")
    drvExtPhyDummySourceFile.setOutputName("drv_extphy_dummy.c")

    drvExtPhyDummySourceFile.setOverwrite(True)
    drvExtPhyDummySourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyDummySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyDummySourceFile.setType("SOURCE")
    drvExtPhyDummySourceFile.setEnabled(True)
    drvExtPhyDummySourceFile.setDependencies(drvExtPhyDummySourceFileAdd, ["DRV_ETHPHY_USE_THIN_DRIVER"])
    
    drvExtPhyThinSourceFile = drvExtPhyDummyComponent.createFileSymbol(None, None)
    drvExtPhyThinSourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_dummy_thin.c")
    drvExtPhyThinSourceFile.setOutputName("drv_extphy_dummy_thin.c")
    drvExtPhyThinSourceFile.setOverwrite(True)
    drvExtPhyThinSourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyThinSourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyThinSourceFile.setType("SOURCE")
    drvExtPhyThinSourceFile.setEnabled(False)
    drvExtPhyThinSourceFile.setDependencies(drvExtPhyThinSourceFileAdd, ["DRV_ETHPHY_USE_THIN_DRIVER"])
    
def drvExtPhyDummyMenuVisibleSingle(symbol, event):
    if (event["value"] == True):
        print("EthMac Menu Visible.")       
        symbol.setVisible(True)
    else:
        print("EthMac Menu Invisible.")
        symbol.setVisible(False)
        
def drvExtPhyDummySourceFileAdd(sourceFile, event):
    sourceFile.setEnabled(not event["value"])

def drvExtPhyThinSourceFileAdd(sourceFile, event):
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

class macNegFields():
    def __init__(self):
        self.autoNegSymbol = ''
        self.autoMdixSymbol = ''
        self.swapMdixSymbol = ''
        self.phyLoopSymbol = ''
        self.rmtCtrlSymbol = ''

def getMacNegFields(macTarget):
    macComponentName = macTarget["component"].getDisplayName()

    macFields = macNegFields()
    macFields.rmtCtrlSymbol = macTarget["component"].getSymbolByID("RMT_CTRL_ENABLE_FLAG")
    
    if 'ETHMAC' in macComponentName:
        macFields.autoNegSymbol = macTarget["component"].getSymbolByID("TCPIP_EMAC_ETH_OF_AUTO_NEGOTIATION")
        macFields.autoMdixSymbol = macTarget["component"].getSymbolByID("TCPIP_EMAC_ETH_OF_MDIX_AUTO")
        macFields.swapMdixSymbol = macTarget["component"].getSymbolByID("TCPIP_EMAC_ETH_OF_MDIX_SWAP")
        macFields.phyLoopSymbol = macTarget["component"].getSymbolByID("TCPIP_EMAC_ETH_OF_PHY_LOOPBACK")

    elif 'GMAC' in macComponentName:
        macFields.autoNegSymbol = macTarget["component"].getSymbolByID("TCPIP_"+ macComponentName + "_ETH_OF_AUTO_NEGOTIATION")
        macFields.autoMdixSymbol = macTarget["component"].getSymbolByID("TCPIP_"+ macComponentName + "_ETH_OF_MDIX_AUTO")
        macFields.swapMdixSymbol = macTarget["component"].getSymbolByID("TCPIP_"+ macComponentName + "_ETH_OF_MDIX_SWAP")
        macFields.phyLoopSymbol = macTarget["component"].getSymbolByID("TCPIP_"+ macComponentName + "_ETH_OF_PHY_LOOPBACK")

    elif 'EMAC' in macComponentName:
        macFields.autoNegSymbol = macTarget["component"].getSymbolByID("LINK_AUTO_NEGOTIATION")
        macFields.autoMdixSymbol = macTarget["component"].getSymbolByID("MDIX_AUTO_ENABLE")
        macFields.swapMdixSymbol = macTarget["component"].getSymbolByID("MDIX_SWAP")
        macFields.phyLoopSymbol = macTarget["component"].getSymbolByID("PHY_LOOPBACK")

    else:
        # Unsupported MAC
        return None

    return macFields


def onAttachmentConnected(source, target):
    macFields = getMacNegFields(target)

    if macFields == None:
        # Unsupported MAC
        return
    
    macFields.autoNegSymbol.setValue(False)
    macFields.autoNegSymbol.setVisible(False)

    macFields.autoMdixSymbol.setValue(False)
    macFields.autoMdixSymbol.setVisible(False)
    macFields.swapMdixSymbol.setValue(False)
    macFields.swapMdixSymbol.setVisible(False)

    macFields.phyLoopSymbol.setValue(False)
    macFields.phyLoopSymbol.setVisible(False)

    macFields.rmtCtrlSymbol.setValue(True)

def onAttachmentDisconnected(source, target):
    # this is required to restore the autoneg and other symbols when dummy phy is removed from the project.
    macFields = getMacNegFields(target)

    if macFields == None:
        # Unsupported MAC
        return

    macFields.autoNegSymbol.setValue(True)
    macFields.autoNegSymbol.setVisible(True)

    macFields.autoMdixSymbol.setValue(True)
    macFields.autoMdixSymbol.setVisible(True)
    macFields.swapMdixSymbol.setValue(False)
    macFields.swapMdixSymbol.setVisible(False)

    macFields.phyLoopSymbol.setValue(False)
    macFields.phyLoopSymbol.setVisible(True)
    
    macFields.rmtCtrlSymbol.setValue(False)

