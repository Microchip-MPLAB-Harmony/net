"""
Copyright (C) 2023-2025 , Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

phyName = "LAN8770"
def instantiateComponent(drvExtPhyLan8770Component):
    print("LAN8770 PHY Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")

    # PHY Address
    drvExtPhyLan8770Addr= drvExtPhyLan8770Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
    drvExtPhyLan8770Addr.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8770Addr.setLabel("PHY Address") 
    drvExtPhyLan8770Addr.setVisible(True)
    drvExtPhyLan8770Addr.setDescription("PHY Address")
    drvExtPhyLan8770Addr.setDefaultValue(4)
    
    # Use a Function to be called at PHY Reset
    drvExtPhyLan8770ResetCallbackEnable = drvExtPhyLan8770Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
    drvExtPhyLan8770ResetCallbackEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8770ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
    drvExtPhyLan8770ResetCallbackEnable.setVisible(True)
    drvExtPhyLan8770ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
    drvExtPhyLan8770ResetCallbackEnable.setDefaultValue(False)
    
    # App Function
    drvExtPhyLan8770ResetCallback = drvExtPhyLan8770Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyLan8770ResetCallbackEnable)
    drvExtPhyLan8770ResetCallback.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8770ResetCallback.setLabel("App Function")
    drvExtPhyLan8770ResetCallback.setVisible(False)
    drvExtPhyLan8770ResetCallback.setDescription("App Function")
    drvExtPhyLan8770ResetCallback.setDefaultValue("App" + phyName + "ResetFunction")
    drvExtPhyLan8770ResetCallback.setDependencies(drvExtPhyLan8770MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])
    
    # External PHY Connection Flags
    drvExtPhyLan8770ConnFlag = drvExtPhyLan8770Component.createMenuSymbol(None, None) 
    drvExtPhyLan8770ConnFlag.setLabel("External PHY Connection Flags")
    drvExtPhyLan8770ConnFlag.setVisible(True)
    drvExtPhyLan8770ConnFlag.setDescription("External PHY Connection Flags")
    
    # RMII Data Interface
    drvExtPhyLan8770ConfigRmii = drvExtPhyLan8770Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyLan8770ConnFlag)
    drvExtPhyLan8770ConfigRmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8770ConfigRmii.setLabel("RMII Data Interface")
    drvExtPhyLan8770ConfigRmii.setVisible(True)
    drvExtPhyLan8770ConfigRmii.setReadOnly(True)
    drvExtPhyLan8770ConfigRmii.setDescription("RMII Data Interface")
    
    if Peripheral.moduleExists("GMAC"):
        drvExtPhyLan8770ConfigRmii.setDefaultValue(True)
    elif "PIC32M" in Variables.get("__PROCESSOR"):
        drvExtPhyLan8770ConfigRmii.setDefaultValue(True)
    
        # Configuration Fuses Is ALT
        drvExtPhyLan8770ConfigAlt = drvExtPhyLan8770Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_ALTERNATE", drvExtPhyLan8770ConnFlag)
        drvExtPhyLan8770ConfigAlt.setHelp("mcc_h3_phy_configurations")
        drvExtPhyLan8770ConfigAlt.setLabel("Configuration Fuses Is ALT")
        drvExtPhyLan8770ConfigAlt.setVisible(True)
        drvExtPhyLan8770ConfigAlt.setDescription("Configuration Fuses Is ALT")
        drvExtPhyLan8770ConfigAlt.setDefaultValue(False)
        
        # Use The Fuses Configuration
        drvExtPhyLan8770ConfigAuto = drvExtPhyLan8770Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTO", drvExtPhyLan8770ConnFlag)
        drvExtPhyLan8770ConfigAuto.setHelp("mcc_h3_phy_configurations")
        drvExtPhyLan8770ConfigAuto.setLabel("Use The Fuses Configuration")
        drvExtPhyLan8770ConfigAuto.setVisible(True)
        drvExtPhyLan8770ConfigAuto.setDescription("Use The Fuses Configuration")
        drvExtPhyLan8770ConfigAuto.setDefaultValue(True)
    
    # Advanced Settings
    drvExtPhyLan8770AdvSettings = drvExtPhyLan8770Component.createMenuSymbol("TCPIP_INTMAC_PHY_ADV_SETTING", None)
    drvExtPhyLan8770AdvSettings.setLabel("Advanced Settings")
    drvExtPhyLan8770AdvSettings.setDescription("Advanced Settings")
    drvExtPhyLan8770AdvSettings.setVisible(True)

    # Delay for the Link Initialization in ms
    drvExtPhyLan8770LinkInitDelay= drvExtPhyLan8770Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", drvExtPhyLan8770AdvSettings)
    drvExtPhyLan8770LinkInitDelay.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8770LinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
    drvExtPhyLan8770LinkInitDelay.setVisible(True)
    drvExtPhyLan8770LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
    drvExtPhyLan8770LinkInitDelay.setDefaultValue(500)
      
    # External PHY Type
    drvExtPhyLan8770PhyType = drvExtPhyLan8770Component.createStringSymbol("TCPIP_EMAC_PHY_TYPE", drvExtPhyLan8770AdvSettings)
    #drvExtPhyLan8770PhyType.setLabel("External PHY Type")
    drvExtPhyLan8770PhyType.setVisible(False)
    #drvExtPhyLan8770PhyType.setDescription("External PHY Type")
    drvExtPhyLan8770PhyType.setDefaultValue(phyName)
    
    # Driver PHY Negotiation Time-out - ms
    drvExtPhyLan8770NegInitTimeout= drvExtPhyLan8770Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", drvExtPhyLan8770AdvSettings)
    drvExtPhyLan8770NegInitTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8770NegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
    drvExtPhyLan8770NegInitTimeout.setVisible(True)
    drvExtPhyLan8770NegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
    drvExtPhyLan8770NegInitTimeout.setDefaultValue(1)
    
    # Driver PHY Negotiation Done Time-out - ms
    drvExtPhyLan8770NegDoneTimeout= drvExtPhyLan8770Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", drvExtPhyLan8770AdvSettings)
    drvExtPhyLan8770NegDoneTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8770NegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
    drvExtPhyLan8770NegDoneTimeout.setVisible(True)
    drvExtPhyLan8770NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
    drvExtPhyLan8770NegDoneTimeout.setDefaultValue(2000)
    
    # Driver PHY Reset Clear Time-out - ms
    drvExtPhyLan8770ResetClearTimeout= drvExtPhyLan8770Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", drvExtPhyLan8770AdvSettings)
    drvExtPhyLan8770ResetClearTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8770ResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
    drvExtPhyLan8770ResetClearTimeout.setVisible(True)
    drvExtPhyLan8770ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
    drvExtPhyLan8770ResetClearTimeout.setDefaultValue(500)
    
    # Driver PHY Peripheral ID
    drvExtPhyLan8770PeripheralId= drvExtPhyLan8770Component.createStringSymbol("DRV_ETHPHY_PERIPHERAL_ID", drvExtPhyLan8770AdvSettings)
    drvExtPhyLan8770PeripheralId.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8770PeripheralId.setLabel("PHY Peripheral ID") 
    drvExtPhyLan8770PeripheralId.setVisible(True)
    drvExtPhyLan8770PeripheralId.setDescription("Driver PHY Peripheral ID")
    drvExtPhyLan8770PeripheralId.setDefaultValue("")
    drvExtPhyLan8770PeripheralId.setReadOnly(True)

    # External MAC Name
    drvExtPhyMacName= drvExtPhyLan8770Component.createStringSymbol("DRV_ETHPHY_MAC_NAME", None)
    drvExtPhyMacName.setLabel("Mac Name") 
    drvExtPhyMacName.setVisible(False)   
    drvExtPhyMacName.setDefaultValue("")
    drvExtPhyMacName.setReadOnly(True) 
                
    #Add forward declaration to initialization.c
    drvExtPhyLan8770InitDataSourceFtl = drvExtPhyLan8770Component.createFileSymbol(None, None)
    drvExtPhyLan8770InitDataSourceFtl.setType("STRING")
    drvExtPhyLan8770InitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    drvExtPhyLan8770InitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_driver_initialize.c.ftl")
    drvExtPhyLan8770InitDataSourceFtl.setMarkup(True)    
    
    #Add to initialization.c
    drvExtPhyLan8770SysInitDataSourceFtl = drvExtPhyLan8770Component.createFileSymbol(None, None)
    drvExtPhyLan8770SysInitDataSourceFtl.setType("STRING")
    drvExtPhyLan8770SysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    drvExtPhyLan8770SysInitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_data_initialize.c.ftl")
    drvExtPhyLan8770SysInitDataSourceFtl.setMarkup(True)    
    
    #Add to system_config.h
    drvExtPhyLan8770HeaderFtl = drvExtPhyLan8770Component.createFileSymbol(None, None)
    drvExtPhyLan8770HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_lan8770.h.ftl")
    drvExtPhyLan8770HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    drvExtPhyLan8770HeaderFtl.setMarkup(True)
    drvExtPhyLan8770HeaderFtl.setType("STRING") 
    
    #Add to definitions.h
    drvExtPhyLan8770SysdefFtl = drvExtPhyLan8770Component.createFileSymbol(None, None)
    drvExtPhyLan8770SysdefFtl.setSourcePath("driver/ethphy/templates/system/system_definitions.h.ftl")
    drvExtPhyLan8770SysdefFtl.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    drvExtPhyLan8770SysdefFtl.setMarkup(True)
    drvExtPhyLan8770SysdefFtl.setType("STRING")

    # file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
    # Add drv_ethphy.h file to project
    drvExtPhyHeaderFile = drvExtPhyLan8770Component.createFileSymbol(None, None)
    drvExtPhyHeaderFile.setSourcePath("driver/ethphy/drv_ethphy.h")
    drvExtPhyHeaderFile.setOutputName("drv_ethphy.h")
    drvExtPhyHeaderFile.setDestPath("driver/ethphy/")
    drvExtPhyHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/")
    drvExtPhyHeaderFile.setType("HEADER")
    drvExtPhyHeaderFile.setOverwrite(True)
    drvExtPhyHeaderFile.setEnabled(True)

    # file TCPIP_ETH_PHY_LOCAL_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/drv_ethphy_local.h" to           "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/drv_ethphy_local.h"
    # Add drv_ethphy_local.h file to project
    drvExtPhyLocalHeaderFile = drvExtPhyLan8770Component.createFileSymbol(None, None)
    drvExtPhyLocalHeaderFile.setSourcePath("driver/ethphy/src/drv_ethphy_local.h")
    drvExtPhyLocalHeaderFile.setOutputName("drv_ethphy_local.h")
    drvExtPhyLocalHeaderFile.setDestPath("driver/ethphy/src/")
    drvExtPhyLocalHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/")
    drvExtPhyLocalHeaderFile.setType("HEADER")
    drvExtPhyLocalHeaderFile.setOverwrite(True)
    drvExtPhyLocalHeaderFile.setEnabled(True)

    # file TCPIP_ETH_EXT_PHY_REGS_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h" to    "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h"
    # Add drv_extphy_regs.h file to project
    drvExtPhyLan8770RegHeaderFile = drvExtPhyLan8770Component.createFileSymbol(None, None)
    drvExtPhyLan8770RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
    drvExtPhyLan8770RegHeaderFile.setOutputName("drv_extphy_regs.h")
    drvExtPhyLan8770RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyLan8770RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyLan8770RegHeaderFile.setType("HEADER")
    drvExtPhyLan8770RegHeaderFile.setOverwrite(True)
    drvExtPhyLan8770RegHeaderFile.setEnabled(True)

    # file TCPIP_MAC_PHY_LAN8770_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_lan8770.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_lan8770.h"
    # Add drv_extphy_lan8770.h file to project
    drvExtPhyLan8770HeaderFile = drvExtPhyLan8770Component.createFileSymbol(None, None)
    drvExtPhyLan8770HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_lan8770.h")
    drvExtPhyLan8770HeaderFile.setOutputName("drv_extphy_lan8770.h")
    drvExtPhyLan8770HeaderFile.setDestPath("driver/ethphy/")
    drvExtPhyLan8770HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/")
    drvExtPhyLan8770HeaderFile.setType("HEADER")
    drvExtPhyLan8770HeaderFile.setOverwrite(True)
    drvExtPhyLan8770HeaderFile.setEnabled(True)

    # Add drv_extphy_lan8770_priv.h file to project
    drvExtPhyLan8770PrivFile = drvExtPhyLan8770Component.createFileSymbol(None, None)
    drvExtPhyLan8770PrivFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_lan8770_priv.h")
    drvExtPhyLan8770PrivFile.setOutputName("drv_extphy_lan8770_priv.h")
    drvExtPhyLan8770PrivFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyLan8770PrivFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyLan8770PrivFile.setType("HEADER")
    drvExtPhyLan8770PrivFile.setOverwrite(True)
    drvExtPhyLan8770PrivFile.setEnabled(True)


    # file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
    # Add drv_ethphy.c file
    drvExtPhySourceFile = drvExtPhyLan8770Component.createFileSymbol(None, None)
    drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
    drvExtPhySourceFile.setOutputName("drv_ethphy.c")
    drvExtPhySourceFile.setOverwrite(True)
    drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setType("SOURCE")
    drvExtPhySourceFile.setEnabled(True)

    
    # ifblock TCPIP_EMAC_PHY_TYPE = "LAN8770"
    # file TCPIP_MAC_PHY_LAN8770_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_lan8770.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_lan8770.c"
    # endif
    # Add drv_extphy_lan8770.c file
    drvExtPhyLan8770SourceFile = drvExtPhyLan8770Component.createFileSymbol(None, None)
    drvExtPhyLan8770SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_lan8770.c")
    drvExtPhyLan8770SourceFile.setOutputName("drv_extphy_lan8770.c")
    drvExtPhyLan8770SourceFile.setOverwrite(True)
    drvExtPhyLan8770SourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyLan8770SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyLan8770SourceFile.setType("SOURCE")
    drvExtPhyLan8770SourceFile.setEnabled(True)
    
    
def drvExtPhyLan8770MenuVisibleSingle(symbol, event):
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
