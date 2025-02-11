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

phyName = "LAN8740"
def instantiateComponent(drvExtPhyLan8740Component):
    print("LAN8740 PHY Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")

    # PHY Address
    drvExtPhyLan8740Addr= drvExtPhyLan8740Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
    drvExtPhyLan8740Addr.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8740Addr.setLabel("PHY Address") 
    drvExtPhyLan8740Addr.setVisible(True)
    drvExtPhyLan8740Addr.setDescription("PHY Address")
    drvExtPhyLan8740Addr.setDefaultValue(0)
    
    # Use a Function to be called at PHY Reset
    drvExtPhyLan8740ResetCallbackEnable = drvExtPhyLan8740Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
    drvExtPhyLan8740ResetCallbackEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8740ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
    drvExtPhyLan8740ResetCallbackEnable.setVisible(True)
    drvExtPhyLan8740ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
    drvExtPhyLan8740ResetCallbackEnable.setDefaultValue(False)
    
    # App Function
    drvExtPhyLan8740ResetCallback = drvExtPhyLan8740Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyLan8740ResetCallbackEnable)
    drvExtPhyLan8740ResetCallback.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8740ResetCallback.setLabel("App Function")
    drvExtPhyLan8740ResetCallback.setVisible(False)
    drvExtPhyLan8740ResetCallback.setDescription("App Function")
    drvExtPhyLan8740ResetCallback.setDefaultValue("App" + phyName + "ResetFunction")
    drvExtPhyLan8740ResetCallback.setDependencies(drvExtPhyLan8740MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])
    
    # External PHY Connection Flags
    drvExtPhyLan8740ConnFlag = drvExtPhyLan8740Component.createMenuSymbol(None, None) 
    drvExtPhyLan8740ConnFlag.setLabel("External PHY Connection Flags")
    drvExtPhyLan8740ConnFlag.setVisible(True)
    drvExtPhyLan8740ConnFlag.setDescription("External PHY Connection Flags")
    
    # MII Data Interface
    drvExtPhyLan8740ConfigMii = drvExtPhyLan8740Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_MII", drvExtPhyLan8740ConnFlag)
    drvExtPhyLan8740ConfigMii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8740ConfigMii.setLabel("MII Data Interface")
    drvExtPhyLan8740ConfigMii.setVisible(True)
    drvExtPhyLan8740ConfigMii.setDescription("MII Data Interface")
    drvExtPhyLan8740ConfigMii.setDefaultValue(False)
    
    # RMII Data Interface
    drvExtPhyLan8740ConfigRmii = drvExtPhyLan8740Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyLan8740ConnFlag)
    drvExtPhyLan8740ConfigRmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8740ConfigRmii.setLabel("RMII Data Interface")
    drvExtPhyLan8740ConfigRmii.setVisible(True)
    drvExtPhyLan8740ConfigRmii.setDescription("RMII Data Interface")
    
    if (Peripheral.moduleExists("GMAC")) or (Peripheral.moduleExists("ETH")):
        drvExtPhyLan8740ConfigRmii.setDefaultValue(True)
    elif ("PIC32M" in Variables.get("__PROCESSOR")) or ("WFI32" in Variables.get("__PROCESSOR")):
        drvExtPhyLan8740ConfigRmii.setDefaultValue(False)
    
        # Configuration Fuses Is ALT
        drvExtPhyLan8740ConfigAlt = drvExtPhyLan8740Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_ALTERNATE", drvExtPhyLan8740ConnFlag)
        drvExtPhyLan8740ConfigAlt.setHelp("mcc_h3_phy_configurations")
        drvExtPhyLan8740ConfigAlt.setLabel("Configuration Fuses Is ALT")
        drvExtPhyLan8740ConfigAlt.setVisible(True)
        drvExtPhyLan8740ConfigAlt.setDescription("Configuration Fuses Is ALT")
        drvExtPhyLan8740ConfigAlt.setDefaultValue(False)
        
        # Use The Fuses Configuration
        drvExtPhyLan8740ConfigAuto = drvExtPhyLan8740Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTO", drvExtPhyLan8740ConnFlag)
        drvExtPhyLan8740ConfigAuto.setHelp("mcc_h3_phy_configurations")
        drvExtPhyLan8740ConfigAuto.setLabel("Use The Fuses Configuration")
        drvExtPhyLan8740ConfigAuto.setVisible(True)
        drvExtPhyLan8740ConfigAuto.setDescription("Use The Fuses Configuration")
        drvExtPhyLan8740ConfigAuto.setDefaultValue(True)
    
    # Advanced Settings
    drvExtPhyLan8740AdvSettings = drvExtPhyLan8740Component.createMenuSymbol("TCPIP_INTMAC_PHY_ADV_SETTING", None)
    drvExtPhyLan8740AdvSettings.setLabel("Advanced Settings")
    drvExtPhyLan8740AdvSettings.setDescription("Advanced Settings")
    drvExtPhyLan8740AdvSettings.setVisible(True)

    # Delay for the Link Initialization in ms
    drvExtPhyLan8740LinkInitDelay= drvExtPhyLan8740Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", drvExtPhyLan8740AdvSettings)
    drvExtPhyLan8740LinkInitDelay.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8740LinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
    drvExtPhyLan8740LinkInitDelay.setVisible(True)
    drvExtPhyLan8740LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
    drvExtPhyLan8740LinkInitDelay.setDefaultValue(500)
      
    # External PHY Type
    drvExtPhyLan8740PhyType = drvExtPhyLan8740Component.createStringSymbol("TCPIP_EMAC_PHY_TYPE", drvExtPhyLan8740AdvSettings)
    #drvExtPhyLan8740PhyType.setLabel("External PHY Type")
    drvExtPhyLan8740PhyType.setVisible(False)
    #drvExtPhyLan8740PhyType.setDescription("External PHY Type")
    drvExtPhyLan8740PhyType.setDefaultValue(phyName)
    
    # Driver PHY Negotiation Time-out - ms
    drvExtPhyLan8740NegInitTimeout= drvExtPhyLan8740Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", drvExtPhyLan8740AdvSettings)
    drvExtPhyLan8740NegInitTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8740NegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
    drvExtPhyLan8740NegInitTimeout.setVisible(True)
    drvExtPhyLan8740NegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
    drvExtPhyLan8740NegInitTimeout.setDefaultValue(1)
    
    # Driver PHY Negotiation Done Time-out - ms
    drvExtPhyLan8740NegDoneTimeout= drvExtPhyLan8740Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", drvExtPhyLan8740AdvSettings)
    drvExtPhyLan8740NegDoneTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8740NegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
    drvExtPhyLan8740NegDoneTimeout.setVisible(True)
    drvExtPhyLan8740NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
    drvExtPhyLan8740NegDoneTimeout.setDefaultValue(2000)
    
    # Driver PHY Reset Clear Time-out - ms
    drvExtPhyLan8740ResetClearTimeout= drvExtPhyLan8740Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", drvExtPhyLan8740AdvSettings)
    drvExtPhyLan8740ResetClearTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8740ResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
    drvExtPhyLan8740ResetClearTimeout.setVisible(True)
    drvExtPhyLan8740ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
    drvExtPhyLan8740ResetClearTimeout.setDefaultValue(500)
    
    # Driver PHY Instances Number
    drvExtPhyLan8740InstanceNum= drvExtPhyLan8740Component.createIntegerSymbol("DRV_ETHPHY_INSTANCES_NUMBER", drvExtPhyLan8740AdvSettings)
    drvExtPhyLan8740InstanceNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8740InstanceNum.setLabel("PHY Instances Number") 
    drvExtPhyLan8740InstanceNum.setVisible(True)
    drvExtPhyLan8740InstanceNum.setDescription("Driver PHY Instances Number")
    drvExtPhyLan8740InstanceNum.setDefaultValue(1)
    drvExtPhyLan8740InstanceNum.setReadOnly(True)
    
    # Driver PHY Clients Number
    drvExtPhyLan8740ClientNum= drvExtPhyLan8740Component.createIntegerSymbol("DRV_ETHPHY_CLIENTS_NUMBER", drvExtPhyLan8740AdvSettings)
    drvExtPhyLan8740ClientNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8740ClientNum.setLabel("PHY Clients Number") 
    drvExtPhyLan8740ClientNum.setVisible(True)
    drvExtPhyLan8740ClientNum.setDescription("Driver PHY Clients Number")
    drvExtPhyLan8740ClientNum.setDefaultValue(1)
    drvExtPhyLan8740ClientNum.setReadOnly(True)
    
    # Driver PHY Peripheral Index Number
    drvExtPhyLan8740IndexNum= drvExtPhyLan8740Component.createIntegerSymbol("DRV_ETHPHY_INDEX", drvExtPhyLan8740AdvSettings)
    drvExtPhyLan8740IndexNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8740IndexNum.setLabel("PHY Peripheral Index Number") 
    drvExtPhyLan8740IndexNum.setVisible(True)
    drvExtPhyLan8740IndexNum.setDescription("Driver PHY Peripheral Index Number")
    drvExtPhyLan8740IndexNum.setDefaultValue(1)
    drvExtPhyLan8740IndexNum.setReadOnly(True)
    
    # Driver PHY Peripheral ID
    drvExtPhyLan8740PeripheralId= drvExtPhyLan8740Component.createStringSymbol("DRV_ETHPHY_PERIPHERAL_ID", drvExtPhyLan8740AdvSettings)
    drvExtPhyLan8740PeripheralId.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8740PeripheralId.setLabel("PHY Peripheral ID") 
    drvExtPhyLan8740PeripheralId.setVisible(True)
    drvExtPhyLan8740PeripheralId.setDescription("Driver PHY Peripheral ID")
    drvExtPhyLan8740PeripheralId.setDefaultValue("")
    drvExtPhyLan8740PeripheralId.setReadOnly(True)

    # External MAC Name
    drvExtPhyMacName= drvExtPhyLan8740Component.createStringSymbol("DRV_ETHPHY_MAC_NAME", None)
    drvExtPhyMacName.setLabel("Mac Name") 
    drvExtPhyMacName.setVisible(False)   
    drvExtPhyMacName.setDefaultValue("")
    drvExtPhyMacName.setReadOnly(True) 
                
    #Add forward declaration to initialization.c
    drvExtPhyLan8740InitDataSourceFtl = drvExtPhyLan8740Component.createFileSymbol(None, None)
    drvExtPhyLan8740InitDataSourceFtl.setType("STRING")
    drvExtPhyLan8740InitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    drvExtPhyLan8740InitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_driver_initialize.c.ftl")
    drvExtPhyLan8740InitDataSourceFtl.setMarkup(True)    
    
    #Add to initialization.c
    drvExtPhyLan8740SysInitDataSourceFtl = drvExtPhyLan8740Component.createFileSymbol(None, None)
    drvExtPhyLan8740SysInitDataSourceFtl.setType("STRING")
    drvExtPhyLan8740SysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    drvExtPhyLan8740SysInitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_data_initialize.c.ftl")
    drvExtPhyLan8740SysInitDataSourceFtl.setMarkup(True)    
    
    #Add to system_config.h
    drvExtPhyLan8740HeaderFtl = drvExtPhyLan8740Component.createFileSymbol(None, None)
    drvExtPhyLan8740HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_lan8740.h.ftl")
    drvExtPhyLan8740HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    drvExtPhyLan8740HeaderFtl.setMarkup(True)
    drvExtPhyLan8740HeaderFtl.setType("STRING") 
    
    #Add to definitions.h
    drvExtPhyLan8740SysdefFtl = drvExtPhyLan8740Component.createFileSymbol(None, None)
    drvExtPhyLan8740SysdefFtl.setSourcePath("driver/ethphy/templates/system/system_definitions.h.ftl")
    drvExtPhyLan8740SysdefFtl.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    drvExtPhyLan8740SysdefFtl.setMarkup(True)
    drvExtPhyLan8740SysdefFtl.setType("STRING")

    # file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
    # Add drv_ethphy.h file to project
    drvExtPhyHeaderFile = drvExtPhyLan8740Component.createFileSymbol(None, None)
    drvExtPhyHeaderFile.setSourcePath("driver/ethphy/drv_ethphy.h")
    drvExtPhyHeaderFile.setOutputName("drv_ethphy.h")
    drvExtPhyHeaderFile.setDestPath("driver/ethphy/")
    drvExtPhyHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/")
    drvExtPhyHeaderFile.setType("HEADER")
    drvExtPhyHeaderFile.setOverwrite(True)
    drvExtPhyHeaderFile.setEnabled(True)

    # file TCPIP_ETH_PHY_LOCAL_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/drv_ethphy_local.h" to           "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/drv_ethphy_local.h"
    # Add drv_ethphy_local.h file to project
    drvExtPhyLocalHeaderFile = drvExtPhyLan8740Component.createFileSymbol(None, None)
    drvExtPhyLocalHeaderFile.setSourcePath("driver/ethphy/src/drv_ethphy_local.h")
    drvExtPhyLocalHeaderFile.setOutputName("drv_ethphy_local.h")
    drvExtPhyLocalHeaderFile.setDestPath("driver/ethphy/src/")
    drvExtPhyLocalHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/")
    drvExtPhyLocalHeaderFile.setType("HEADER")
    drvExtPhyLocalHeaderFile.setOverwrite(True)
    drvExtPhyLocalHeaderFile.setEnabled(True)

    # file TCPIP_ETH_EXT_PHY_REGS_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h" to    "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h"
    # Add drv_extphy_regs.h file to project
    drvExtPhyLan8740RegHeaderFile = drvExtPhyLan8740Component.createFileSymbol(None, None)
    drvExtPhyLan8740RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
    drvExtPhyLan8740RegHeaderFile.setOutputName("drv_extphy_regs.h")
    drvExtPhyLan8740RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyLan8740RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyLan8740RegHeaderFile.setType("HEADER")
    drvExtPhyLan8740RegHeaderFile.setOverwrite(True)
    drvExtPhyLan8740RegHeaderFile.setEnabled(True)

    # file TCPIP_MAC_PHY_LAN8740_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_lan8740.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_lan8740.h"
    # Add drv_extphy_lan8740.h file to project
    drvExtPhyLan8740HeaderFile = drvExtPhyLan8740Component.createFileSymbol(None, None)
    drvExtPhyLan8740HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_lan8740.h")
    drvExtPhyLan8740HeaderFile.setOutputName("drv_extphy_lan8740.h")
    drvExtPhyLan8740HeaderFile.setDestPath("driver/ethphy/")
    drvExtPhyLan8740HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/")
    drvExtPhyLan8740HeaderFile.setType("HEADER")
    drvExtPhyLan8740HeaderFile.setOverwrite(True)
    drvExtPhyLan8740HeaderFile.setEnabled(True)

    # Add drv_extphy_lan8740_priv.h file to project
    drvExtPhyLan8740PrivFile = drvExtPhyLan8740Component.createFileSymbol(None, None)
    drvExtPhyLan8740PrivFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_lan8740_priv.h")
    drvExtPhyLan8740PrivFile.setOutputName("drv_extphy_lan8740_priv.h")
    drvExtPhyLan8740PrivFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyLan8740PrivFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyLan8740PrivFile.setType("HEADER")
    drvExtPhyLan8740PrivFile.setOverwrite(True)
    drvExtPhyLan8740PrivFile.setEnabled(True)


    # file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
    # Add drv_ethphy.c file
    drvExtPhySourceFile = drvExtPhyLan8740Component.createFileSymbol(None, None)
    drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
    drvExtPhySourceFile.setOutputName("drv_ethphy.c")
    drvExtPhySourceFile.setOverwrite(True)
    drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setType("SOURCE")
    drvExtPhySourceFile.setEnabled(True)

    
    # ifblock TCPIP_EMAC_PHY_TYPE = "LAN8740"
    # file TCPIP_MAC_PHY_LAN8740_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_lan8740.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_lan8740.c"
    # endif
    # Add drv_extphy_lan8740.c file
    drvExtPhyLan8740SourceFile = drvExtPhyLan8740Component.createFileSymbol(None, None)
    drvExtPhyLan8740SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_lan8740.c")
    drvExtPhyLan8740SourceFile.setOutputName("drv_extphy_lan8740.c")
    drvExtPhyLan8740SourceFile.setOverwrite(True)
    drvExtPhyLan8740SourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyLan8740SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyLan8740SourceFile.setType("SOURCE")
    drvExtPhyLan8740SourceFile.setEnabled(True)
    
    
def drvExtPhyLan8740MenuVisibleSingle(symbol, event):
    if (event["value"] == True):
        print("EthMac Menu Visible.")       
        symbol.setVisible(True)
    else:
        print("EthMac Menu Invisible.")
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
