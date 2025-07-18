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

phyName = "KSZ8081"
def instantiateComponent(drvExtPhyKsz8081Component):
    print("KSZ8081 PHY Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")

    # PHY Address
    drvExtPhyKsz8081Addr= drvExtPhyKsz8081Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
    drvExtPhyKsz8081Addr.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8081Addr.setLabel("PHY Address") 
    drvExtPhyKsz8081Addr.setVisible(True)
    drvExtPhyKsz8081Addr.setDescription("PHY Address")
    drvExtPhyKsz8081Addr.setDefaultValue(1)    
        
    # Use a Function to be called at PHY Reset
    drvExtPhyKsz8081ResetCallbackEnable = drvExtPhyKsz8081Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
    drvExtPhyKsz8081ResetCallbackEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8081ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
    drvExtPhyKsz8081ResetCallbackEnable.setVisible(True)
    drvExtPhyKsz8081ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
    drvExtPhyKsz8081ResetCallbackEnable.setDefaultValue(False)
    
    # App Function
    drvExtPhyKsz8081ResetCallback = drvExtPhyKsz8081Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyKsz8081ResetCallbackEnable)
    drvExtPhyKsz8081ResetCallback.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8081ResetCallback.setLabel("App Function")
    drvExtPhyKsz8081ResetCallback.setVisible(False)
    drvExtPhyKsz8081ResetCallback.setDescription("App Function")
    drvExtPhyKsz8081ResetCallback.setDefaultValue("App" + phyName + "ResetFunction")
    drvExtPhyKsz8081ResetCallback.setDependencies(drvExtPhyKsz8081MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])

    # External PHY Connection Flags
    drvExtPhyKsz8081ConnFlag = drvExtPhyKsz8081Component.createMenuSymbol(None, None) 
    drvExtPhyKsz8081ConnFlag.setLabel("External PHY Connection Flags")
    drvExtPhyKsz8081ConnFlag.setVisible(True)
    drvExtPhyKsz8081ConnFlag.setDescription("External PHY Connection Flags")
    
    # RMII Data Interface
    drvExtPhyKsz8081ConfigRmii = drvExtPhyKsz8081Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyKsz8081ConnFlag)
    drvExtPhyKsz8081ConfigRmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8081ConfigRmii.setLabel("RMII Data Interface")
    drvExtPhyKsz8081ConfigRmii.setVisible(True)
    drvExtPhyKsz8081ConfigRmii.setDescription("RMII Data Interface")
    drvExtPhyKsz8081ConfigRmii.setDefaultValue(True)
    
    if ("PIC32M" in Variables.get("__PROCESSOR")) or ("WFI32" in Variables.get("__PROCESSOR")):
        # default is Auto for PIC32M
        drvExtPhyKsz8081ConfigRmii.setDefaultValue(False)
    
        # Configuration Fuses Is ALT
        drvExtPhyKsz8081ConfigAlt = drvExtPhyKsz8081Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_ALTERNATE", drvExtPhyKsz8081ConnFlag)
        drvExtPhyKsz8081ConfigAlt.setHelp("mcc_h3_phy_configurations")
        drvExtPhyKsz8081ConfigAlt.setLabel("Configuration Fuses Is ALT")
        drvExtPhyKsz8081ConfigAlt.setVisible(True)
        drvExtPhyKsz8081ConfigAlt.setDescription("Configuration Fuses Is ALT")
        drvExtPhyKsz8081ConfigAlt.setDefaultValue(False)
        
        # Use The Fuses Configuration
        drvExtPhyKsz8081ConfigAuto = drvExtPhyKsz8081Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTO", drvExtPhyKsz8081ConnFlag)
        drvExtPhyKsz8081ConfigAuto.setHelp("mcc_h3_phy_configurations")
        drvExtPhyKsz8081ConfigAuto.setLabel("Use The Fuses Configuration")
        drvExtPhyKsz8081ConfigAuto.setVisible(True)
        drvExtPhyKsz8081ConfigAuto.setDescription("Use The Fuses Configuration")
        drvExtPhyKsz8081ConfigAuto.setDefaultValue(True)    
            
    # Advanced Settings
    drvExtPhyKsz8081AdvSettings = drvExtPhyKsz8081Component.createMenuSymbol("TCPIP_INTMAC_PHY_ADV_SETTING", None)
    drvExtPhyKsz8081AdvSettings.setLabel("Advanced Settings")
    drvExtPhyKsz8081AdvSettings.setDescription("Advanced Settings")
    drvExtPhyKsz8081AdvSettings.setVisible(True)

    # Delay for the Link Initialization in ms
    drvExtPhyKsz8081LinkInitDelay= drvExtPhyKsz8081Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", drvExtPhyKsz8081AdvSettings)
    drvExtPhyKsz8081LinkInitDelay.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8081LinkInitDelay.setLabel("Delay for the Link Initialization (mSec)") 
    drvExtPhyKsz8081LinkInitDelay.setVisible(True)
    drvExtPhyKsz8081LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
    drvExtPhyKsz8081LinkInitDelay.setDefaultValue(500)
    
    # External PHY Type
    drvExtPhyKsz8081PhyType = drvExtPhyKsz8081Component.createStringSymbol("TCPIP_EMAC_PHY_TYPE", drvExtPhyKsz8081AdvSettings)
    drvExtPhyKsz8081PhyType.setVisible(False)   
    drvExtPhyKsz8081PhyType.setDefaultValue(phyName)

    # Driver PHY Negotiation Time-out (mSec)
    drvExtPhyKsz8081NegInitTimeout= drvExtPhyKsz8081Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", drvExtPhyKsz8081AdvSettings)
    drvExtPhyKsz8081NegInitTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8081NegInitTimeout.setLabel("PHY Negotiation Time-out (mSec)") 
    drvExtPhyKsz8081NegInitTimeout.setVisible(True)
    drvExtPhyKsz8081NegInitTimeout.setDescription("Driver PHY Negotiation Time-out (mSec)")
    drvExtPhyKsz8081NegInitTimeout.setDefaultValue(1)
    
    # Driver PHY Negotiation Done Time-out (mSec)
    drvExtPhyKsz8081NegDoneTimeout= drvExtPhyKsz8081Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", drvExtPhyKsz8081AdvSettings)
    drvExtPhyKsz8081NegDoneTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8081NegDoneTimeout.setLabel("PHY Negotiation Done Time-out (mSec)") 
    drvExtPhyKsz8081NegDoneTimeout.setVisible(True)
    drvExtPhyKsz8081NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out (mSec)")
    drvExtPhyKsz8081NegDoneTimeout.setDefaultValue(2000)
    
    # Driver PHY Reset Clear Time-out (mSec)
    drvExtPhyKsz8081ResetClearTimeout= drvExtPhyKsz8081Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", drvExtPhyKsz8081AdvSettings)
    drvExtPhyKsz8081ResetClearTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8081ResetClearTimeout.setLabel("PHY Reset Clear Time-out (mSec)") 
    drvExtPhyKsz8081ResetClearTimeout.setVisible(True)
    drvExtPhyKsz8081ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out (mSec)")
    drvExtPhyKsz8081ResetClearTimeout.setDefaultValue(500)

    # Driver PHY Peripheral ID
    drvExtPhyKsz8081PeripheralId= drvExtPhyKsz8081Component.createStringSymbol("DRV_ETHPHY_PERIPHERAL_ID", drvExtPhyKsz8081AdvSettings)
    drvExtPhyKsz8081PeripheralId.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8081PeripheralId.setLabel("PHY Peripheral ID") 
    drvExtPhyKsz8081PeripheralId.setVisible(True)
    drvExtPhyKsz8081PeripheralId.setDescription("Driver PHY Peripheral ID")
    drvExtPhyKsz8081PeripheralId.setDefaultValue("")
    drvExtPhyKsz8081PeripheralId.setReadOnly(True)        
    
    # External MAC Name
    drvExtPhyMacName= drvExtPhyKsz8081Component.createStringSymbol("DRV_ETHPHY_MAC_NAME", None)
    drvExtPhyMacName.setLabel("Mac Name") 
    drvExtPhyMacName.setVisible(False)   
    drvExtPhyMacName.setDefaultValue("")
    drvExtPhyMacName.setReadOnly(True)  
    
    #Add forward declaration to initialization.c
    drvExtPhyKsz8081InitDataSourceFtl = drvExtPhyKsz8081Component.createFileSymbol(None, None)
    drvExtPhyKsz8081InitDataSourceFtl.setType("STRING")
    drvExtPhyKsz8081InitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    drvExtPhyKsz8081InitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_driver_initialize.c.ftl")
    drvExtPhyKsz8081InitDataSourceFtl.setMarkup(True)    
    
    #Add to initialization.c
    drvExtPhyKsz8081SysInitDataSourceFtl = drvExtPhyKsz8081Component.createFileSymbol(None, None)
    drvExtPhyKsz8081SysInitDataSourceFtl.setType("STRING")
    drvExtPhyKsz8081SysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    drvExtPhyKsz8081SysInitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_data_initialize.c.ftl")
    drvExtPhyKsz8081SysInitDataSourceFtl.setMarkup(True)    
    
    #Add to system_config.h
    drvExtPhyKsz8081HeaderFtl = drvExtPhyKsz8081Component.createFileSymbol(None, None)
    drvExtPhyKsz8081HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_ksz8081.h.ftl")
    drvExtPhyKsz8081HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    drvExtPhyKsz8081HeaderFtl.setMarkup(True)
    drvExtPhyKsz8081HeaderFtl.setType("STRING")
        
    #Add to definitions.h
    drvExtPhyKsz8081SysdefFtl = drvExtPhyKsz8081Component.createFileSymbol(None, None)
    drvExtPhyKsz8081SysdefFtl.setSourcePath("driver/ethphy/templates/system/system_definitions.h.ftl")
    drvExtPhyKsz8081SysdefFtl.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    drvExtPhyKsz8081SysdefFtl.setMarkup(True)
    drvExtPhyKsz8081SysdefFtl.setType("STRING")

    # file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
    # Add drv_ethphy.h file to project
    drvExtPhyHeaderFile = drvExtPhyKsz8081Component.createFileSymbol(None, None)
    drvExtPhyHeaderFile.setSourcePath("driver/ethphy/drv_ethphy.h")
    drvExtPhyHeaderFile.setOutputName("drv_ethphy.h")
    drvExtPhyHeaderFile.setDestPath("driver/ethphy/")
    drvExtPhyHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/")
    drvExtPhyHeaderFile.setType("HEADER")
    drvExtPhyHeaderFile.setOverwrite(True)
    drvExtPhyHeaderFile.setEnabled(True)
    #drvEthPhyHeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])

    # file TCPIP_ETH_PHY_LOCAL_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/drv_ethphy_local.h" to           "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/drv_ethphy_local.h"
    # Add drv_ethphy_local.h file to project
    drvExtPhyLocalHeaderFile = drvExtPhyKsz8081Component.createFileSymbol(None, None)
    drvExtPhyLocalHeaderFile.setSourcePath("driver/ethphy/src/drv_ethphy_local.h")
    drvExtPhyLocalHeaderFile.setOutputName("drv_ethphy_local.h")
    drvExtPhyLocalHeaderFile.setDestPath("driver/ethphy/src/")
    drvExtPhyLocalHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/")
    drvExtPhyLocalHeaderFile.setType("HEADER")
    drvExtPhyLocalHeaderFile.setOverwrite(True)
    drvExtPhyLocalHeaderFile.setEnabled(True)
    #drvEthPhyLocalHeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])

    # file TCPIP_ETH_EXT_PHY_REGS_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h" to    "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h"
    # Add drv_extphy_regs.h file to project
    drvExtPhyKsz8081RegHeaderFile = drvExtPhyKsz8081Component.createFileSymbol(None, None)
    drvExtPhyKsz8081RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
    drvExtPhyKsz8081RegHeaderFile.setOutputName("drv_extphy_regs.h")
    drvExtPhyKsz8081RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyKsz8081RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyKsz8081RegHeaderFile.setType("HEADER")
    drvExtPhyKsz8081RegHeaderFile.setOverwrite(True)
    drvExtPhyKsz8081RegHeaderFile.setEnabled(True)
    
    # Add drv_extphy_ksz8081.h file to project
    drvExtPhyKsz8081HeaderFile = drvExtPhyKsz8081Component.createFileSymbol(None, None)
    drvExtPhyKsz8081HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz8081.h")
    drvExtPhyKsz8081HeaderFile.setOutputName("drv_extphy_ksz8081.h")
    drvExtPhyKsz8081HeaderFile.setDestPath("driver/ethphy/")
    drvExtPhyKsz8081HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/")
    drvExtPhyKsz8081HeaderFile.setType("HEADER")
    drvExtPhyKsz8081HeaderFile.setOverwrite(True)
    drvExtPhyKsz8081HeaderFile.setEnabled(True)

    # Add drv_extphy_ksz8081_priv.h file to project
    drvExtPhyKsz8081PrivFile = drvExtPhyKsz8081Component.createFileSymbol(None, None)
    drvExtPhyKsz8081PrivFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz8081_priv.h")
    drvExtPhyKsz8081PrivFile.setOutputName("drv_extphy_ksz8081_priv.h")
    drvExtPhyKsz8081PrivFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyKsz8081PrivFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyKsz8081PrivFile.setType("HEADER")
    drvExtPhyKsz8081PrivFile.setOverwrite(True)
    drvExtPhyKsz8081PrivFile.setEnabled(True)

    # file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
    # Add drv_ethphy.c file
    drvExtPhySourceFile = drvExtPhyKsz8081Component.createFileSymbol(None, None)
    drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
    drvExtPhySourceFile.setOutputName("drv_ethphy.c")
    drvExtPhySourceFile.setOverwrite(True)
    drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setType("SOURCE")
    drvExtPhySourceFile.setEnabled(True)

    # Add drv_extphy_ksz8081.c file
    drvExtPhyKsz8081SourceFile = drvExtPhyKsz8081Component.createFileSymbol(None, None)
    drvExtPhyKsz8081SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz8081.c")
    drvExtPhyKsz8081SourceFile.setOutputName("drv_extphy_ksz8081.c")
    drvExtPhyKsz8081SourceFile.setOverwrite(True)
    drvExtPhyKsz8081SourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyKsz8081SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyKsz8081SourceFile.setType("SOURCE")
    drvExtPhyKsz8081SourceFile.setEnabled(True)

    
def drvExtPhyKsz8081MenuVisibleSingle(symbol, event):
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
