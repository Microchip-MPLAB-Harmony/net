"""
Copyright (C) 2020-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

phyName = "LAN8720"
def instantiateComponent(drvExtPhyLan8720Component):
    print("LAN8720 PHY Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")

    # PHY Address
    drvExtPhyLan8720Addr= drvExtPhyLan8720Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
    drvExtPhyLan8720Addr.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8720Addr.setLabel("PHY Address") 
    drvExtPhyLan8720Addr.setVisible(True)
    drvExtPhyLan8720Addr.setDescription("PHY Address")
    drvExtPhyLan8720Addr.setDefaultValue(0)
    
    # Use a Function to be called at PHY Reset
    drvExtPhyLan8720ResetCallbackEnable = drvExtPhyLan8720Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
    drvExtPhyLan8720ResetCallbackEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8720ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
    drvExtPhyLan8720ResetCallbackEnable.setVisible(True)
    drvExtPhyLan8720ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
    drvExtPhyLan8720ResetCallbackEnable.setDefaultValue(False)
    
    # App Function
    drvExtPhyLan8720ResetCallback = drvExtPhyLan8720Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyLan8720ResetCallbackEnable)
    drvExtPhyLan8720ResetCallback.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8720ResetCallback.setLabel("App Function")
    drvExtPhyLan8720ResetCallback.setVisible(False)
    drvExtPhyLan8720ResetCallback.setDescription("App Function")
    drvExtPhyLan8720ResetCallback.setDefaultValue("App" + phyName + "ResetFunction")
    drvExtPhyLan8720ResetCallback.setDependencies(drvExtPhyLan8720MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])
    
    # External PHY Connection Flags
    drvExtPhyLan8720ConnFlag = drvExtPhyLan8720Component.createMenuSymbol(None, None) 
    drvExtPhyLan8720ConnFlag.setLabel("External PHY Connection Flags")
    drvExtPhyLan8720ConnFlag.setVisible(True)
    drvExtPhyLan8720ConnFlag.setDescription("External PHY Connection Flags")
    
    # RMII Data Interface
    drvExtPhyLan8720ConfigRmii = drvExtPhyLan8720Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyLan8720ConnFlag)
    drvExtPhyLan8720ConfigRmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8720ConfigRmii.setLabel("RMII Data Interface")
    drvExtPhyLan8720ConfigRmii.setVisible(True)
    drvExtPhyLan8720ConfigRmii.setDescription("RMII Data Interface")
    if Peripheral.moduleExists("GMAC"):
        drvExtPhyLan8720ConfigRmii.setDefaultValue(True)
    elif "PIC32M" in Variables.get("__PROCESSOR"):
        drvExtPhyLan8720ConfigRmii.setDefaultValue(False)
    
        # Configuration Fuses Is ALT
        drvExtPhyLan8720ConfigAlt = drvExtPhyLan8720Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_ALTERNATE", drvExtPhyLan8720ConnFlag)
        drvExtPhyLan8720ConfigAlt.setHelp("mcc_h3_phy_configurations")
        drvExtPhyLan8720ConfigAlt.setLabel("Configuration Fuses Is ALT")
        drvExtPhyLan8720ConfigAlt.setVisible(True)
        drvExtPhyLan8720ConfigAlt.setDescription("Configuration Fuses Is ALT")
        drvExtPhyLan8720ConfigAlt.setDefaultValue(False)
    
        # Use The Fuses Configuration
        drvExtPhyLan8720ConfigAuto = drvExtPhyLan8720Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTO", drvExtPhyLan8720ConnFlag)
        drvExtPhyLan8720ConfigAuto.setHelp("mcc_h3_phy_configurations")
        drvExtPhyLan8720ConfigAuto.setLabel("Use The Fuses Configuration")
        drvExtPhyLan8720ConfigAuto.setVisible(True)
        drvExtPhyLan8720ConfigAuto.setDescription("Use The Fuses Configuration")
        drvExtPhyLan8720ConfigAuto.setDefaultValue(False)
    
    # Advanced Settings
    drvExtPhyLan8720AdvSettings = drvExtPhyLan8720Component.createMenuSymbol("TCPIP_INTMAC_PHY_ADV_SETTING", None)
    drvExtPhyLan8720AdvSettings.setLabel("Advanced Settings")
    drvExtPhyLan8720AdvSettings.setDescription("Advanced Settings")
    drvExtPhyLan8720AdvSettings.setVisible(True)
    
    # Delay for the Link Initialization in ms
    drvExtPhyLan8720LinkInitDelay= drvExtPhyLan8720Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", drvExtPhyLan8720AdvSettings)
    drvExtPhyLan8720LinkInitDelay.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8720LinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
    drvExtPhyLan8720LinkInitDelay.setVisible(True)
    drvExtPhyLan8720LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
    drvExtPhyLan8720LinkInitDelay.setDefaultValue(500)

    # External PHY Type
    drvExtPhyLan8720PhyType = drvExtPhyLan8720Component.createStringSymbol("TCPIP_EMAC_PHY_TYPE", drvExtPhyLan8720AdvSettings)
    drvExtPhyLan8720PhyType.setVisible(False)   
    drvExtPhyLan8720PhyType.setDefaultValue(phyName)
        
    # Driver PHY Negotiation Time-out (mSec)
    drvExtPhyLan8720NegInitTimeout= drvExtPhyLan8720Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", drvExtPhyLan8720AdvSettings)
    drvExtPhyLan8720NegInitTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8720NegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
    drvExtPhyLan8720NegInitTimeout.setVisible(True)
    drvExtPhyLan8720NegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
    drvExtPhyLan8720NegInitTimeout.setDefaultValue(1)
    
    # Driver PHY Negotiation Done Time-out (mSec)
    drvExtPhyLan8720NegDoneTimeout= drvExtPhyLan8720Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", drvExtPhyLan8720AdvSettings)
    drvExtPhyLan8720NegDoneTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8720NegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
    drvExtPhyLan8720NegDoneTimeout.setVisible(True)
    drvExtPhyLan8720NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
    drvExtPhyLan8720NegDoneTimeout.setDefaultValue(2000)
    
    # Driver PHY Reset Clear Time-out (mSec)
    drvExtPhyLan8720ResetClearTimeout= drvExtPhyLan8720Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", drvExtPhyLan8720AdvSettings)
    drvExtPhyLan8720ResetClearTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8720ResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
    drvExtPhyLan8720ResetClearTimeout.setVisible(True)
    drvExtPhyLan8720ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
    drvExtPhyLan8720ResetClearTimeout.setDefaultValue(500)
    
    # Driver PHY Instances Number
    drvExtPhyLan8720InstanceNum= drvExtPhyLan8720Component.createIntegerSymbol("DRV_ETHPHY_INSTANCES_NUMBER", drvExtPhyLan8720AdvSettings)
    drvExtPhyLan8720InstanceNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8720InstanceNum.setLabel("PHY Instances Number") 
    drvExtPhyLan8720InstanceNum.setVisible(True)
    drvExtPhyLan8720InstanceNum.setDescription("Driver PHY Instances Number")
    drvExtPhyLan8720InstanceNum.setDefaultValue(1)
    drvExtPhyLan8720InstanceNum.setReadOnly(True)
    
    # Driver PHY Clients Number
    drvExtPhyLan8720ClientNum= drvExtPhyLan8720Component.createIntegerSymbol("DRV_ETHPHY_CLIENTS_NUMBER", drvExtPhyLan8720AdvSettings)
    drvExtPhyLan8720ClientNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8720ClientNum.setLabel("PHY Clients Number") 
    drvExtPhyLan8720ClientNum.setVisible(True)
    drvExtPhyLan8720ClientNum.setDescription("Driver PHY Clients Number")
    drvExtPhyLan8720ClientNum.setDefaultValue(1)
    drvExtPhyLan8720ClientNum.setReadOnly(True)
    
    # Driver PHY Peripheral Index Number
    drvExtPhyLan8720IndexNum= drvExtPhyLan8720Component.createIntegerSymbol("DRV_ETHPHY_INDEX", drvExtPhyLan8720AdvSettings)
    drvExtPhyLan8720IndexNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8720IndexNum.setLabel("PHY Peripheral Index Number") 
    drvExtPhyLan8720IndexNum.setVisible(True)
    drvExtPhyLan8720IndexNum.setDescription("Driver PHY Peripheral Index Number")
    drvExtPhyLan8720IndexNum.setDefaultValue(1)
    drvExtPhyLan8720IndexNum.setReadOnly(True)
    
    # Driver PHY Peripheral ID
    drvExtPhyLan8720PeripheralId= drvExtPhyLan8720Component.createStringSymbol("DRV_ETHPHY_PERIPHERAL_ID", drvExtPhyLan8720AdvSettings)
    drvExtPhyLan8720PeripheralId.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8720PeripheralId.setLabel("PHY Peripheral ID") 
    drvExtPhyLan8720PeripheralId.setVisible(True)
    drvExtPhyLan8720PeripheralId.setDescription("Driver PHY Peripheral ID")
    drvExtPhyLan8720PeripheralId.setDefaultValue("")
    drvExtPhyLan8720PeripheralId.setReadOnly(True)
    
    # External MAC Name
    drvExtPhyMacName= drvExtPhyLan8720Component.createStringSymbol("DRV_ETHPHY_MAC_NAME", None)
    drvExtPhyMacName.setLabel("Mac Name") 
    drvExtPhyMacName.setVisible(False)   
    drvExtPhyMacName.setDefaultValue("")
    drvExtPhyMacName.setReadOnly(True)
        
    #Add forward declaration to initialization.c
    drvExtPhyLan8720InitDataSourceFtl = drvExtPhyLan8720Component.createFileSymbol(None, None)
    drvExtPhyLan8720InitDataSourceFtl.setType("STRING")
    drvExtPhyLan8720InitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    drvExtPhyLan8720InitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_driver_initialize.c.ftl")
    drvExtPhyLan8720InitDataSourceFtl.setMarkup(True)    
    
    #Add to initialization.c
    drvExtPhyLan8720SysInitDataSourceFtl = drvExtPhyLan8720Component.createFileSymbol(None, None)
    drvExtPhyLan8720SysInitDataSourceFtl.setType("STRING")
    drvExtPhyLan8720SysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    drvExtPhyLan8720SysInitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_data_initialize.c.ftl")
    drvExtPhyLan8720SysInitDataSourceFtl.setMarkup(True)    
    
    #Add to system_config.h
    drvExtPhyLan8720HeaderFtl = drvExtPhyLan8720Component.createFileSymbol(None, None)
    drvExtPhyLan8720HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_lan8720.h.ftl")
    drvExtPhyLan8720HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    drvExtPhyLan8720HeaderFtl.setMarkup(True)
    drvExtPhyLan8720HeaderFtl.setType("STRING")
    
    # file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
    # Add drv_ethphy.h file to project
    drvExtPhyHeaderFile = drvExtPhyLan8720Component.createFileSymbol(None, None)
    drvExtPhyHeaderFile.setSourcePath("driver/ethphy/drv_ethphy.h")
    drvExtPhyHeaderFile.setOutputName("drv_ethphy.h")
    drvExtPhyHeaderFile.setDestPath("driver/ethphy/")
    drvExtPhyHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/")
    drvExtPhyHeaderFile.setType("HEADER")
    drvExtPhyHeaderFile.setOverwrite(True)
    drvExtPhyHeaderFile.setEnabled(True)

    # file TCPIP_ETH_PHY_LOCAL_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/drv_ethphy_local.h" to           "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/drv_ethphy_local.h"
    # Add drv_ethphy_local.h file to project
    drvExtPhyLocalHeaderFile = drvExtPhyLan8720Component.createFileSymbol(None, None)
    drvExtPhyLocalHeaderFile.setSourcePath("driver/ethphy/src/drv_ethphy_local.h")
    drvExtPhyLocalHeaderFile.setOutputName("drv_ethphy_local.h")
    drvExtPhyLocalHeaderFile.setDestPath("driver/ethphy/src/")
    drvExtPhyLocalHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/")
    drvExtPhyLocalHeaderFile.setType("HEADER")
    drvExtPhyLocalHeaderFile.setOverwrite(True)
    drvExtPhyLocalHeaderFile.setEnabled(True)

    # file TCPIP_ETH_EXT_PHY_REGS_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h" to    "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h"
    # Add drv_extphy_regs.h file to project
    drvExtPhyLan8720RegHeaderFile = drvExtPhyLan8720Component.createFileSymbol(None, None)
    drvExtPhyLan8720RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
    drvExtPhyLan8720RegHeaderFile.setOutputName("drv_extphy_regs.h")
    drvExtPhyLan8720RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyLan8720RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyLan8720RegHeaderFile.setType("HEADER")
    drvExtPhyLan8720RegHeaderFile.setOverwrite(True)
    drvExtPhyLan8720RegHeaderFile.setEnabled(True)

    # Add drv_extphy_lan8720.h file to project
    drvExtPhyLan8720HeaderFile = drvExtPhyLan8720Component.createFileSymbol(None, None)
    drvExtPhyLan8720HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_lan8720.h")
    drvExtPhyLan8720HeaderFile.setOutputName("drv_extphy_lan8720.h")
    drvExtPhyLan8720HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyLan8720HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyLan8720HeaderFile.setType("HEADER")
    drvExtPhyLan8720HeaderFile.setOverwrite(True)
    drvExtPhyLan8720HeaderFile.setEnabled(True)


    # file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
    # Add drv_ethphy.c file
    drvExtPhySourceFile = drvExtPhyLan8720Component.createFileSymbol(None, None)
    drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
    drvExtPhySourceFile.setOutputName("drv_ethphy.c")
    drvExtPhySourceFile.setOverwrite(True)
    drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setType("SOURCE")
    drvExtPhySourceFile.setEnabled(True)

    # Add drv_extphy_lan8720.c file
    drvExtPhyLan8720SourceFile = drvExtPhyLan8720Component.createFileSymbol(None, None)
    drvExtPhyLan8720SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_lan8720.c")
    drvExtPhyLan8720SourceFile.setOutputName("drv_extphy_lan8720.c")
    drvExtPhyLan8720SourceFile.setOverwrite(True)
    drvExtPhyLan8720SourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyLan8720SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyLan8720SourceFile.setType("SOURCE")
    drvExtPhyLan8720SourceFile.setEnabled(True)

    
def drvExtPhyLan8720MenuVisibleSingle(symbol, event):
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
