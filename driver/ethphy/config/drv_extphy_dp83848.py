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

phyName = "DP83848"
def instantiateComponent(drvExtPhyDp83848Component):
    print("DP83848 PHY Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")

    # PHY Address
    drvExtPhyDp83848Addr= drvExtPhyDp83848Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
    drvExtPhyDp83848Addr.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDp83848Addr.setLabel("PHY Address") 
    drvExtPhyDp83848Addr.setVisible(True)
    drvExtPhyDp83848Addr.setDescription("PHY Address")
    drvExtPhyDp83848Addr.setDefaultValue(0)
    
    # Use a Function to be called at PHY Reset
    drvExtPhyDp83848ResetCallbackEnable = drvExtPhyDp83848Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
    drvExtPhyDp83848ResetCallbackEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDp83848ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
    drvExtPhyDp83848ResetCallbackEnable.setVisible(True)
    drvExtPhyDp83848ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
    drvExtPhyDp83848ResetCallbackEnable.setDefaultValue(False)
    
    # App Function
    drvExtPhyDp83848ResetCallback = drvExtPhyDp83848Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyDp83848ResetCallbackEnable)
    drvExtPhyDp83848ResetCallback.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDp83848ResetCallback.setLabel("App Function")
    drvExtPhyDp83848ResetCallback.setVisible(False)
    drvExtPhyDp83848ResetCallback.setDescription("App Function")
    drvExtPhyDp83848ResetCallback.setDefaultValue("App" + phyName + "ResetFunction")
    drvExtPhyDp83848ResetCallback.setDependencies(drvExtPhyDp83848MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])

    # External PHY Connection Flags
    drvExtPhyDp83848ConnFlag = drvExtPhyDp83848Component.createMenuSymbol(None, None) 
    drvExtPhyDp83848ConnFlag.setLabel("External PHY Connection Flags")
    drvExtPhyDp83848ConnFlag.setVisible(True)
    drvExtPhyDp83848ConnFlag.setDescription("External PHY Connection Flags")
    
    # RMII Data Interface
    drvExtPhyDp83848ConfigRmii = drvExtPhyDp83848Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyDp83848ConnFlag)
    drvExtPhyDp83848ConfigRmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDp83848ConfigRmii.setLabel("RMII Data Interface")
    drvExtPhyDp83848ConfigRmii.setVisible(True)
    drvExtPhyDp83848ConfigRmii.setDescription("RMII Data Interface")
    if Peripheral.moduleExists("GMAC"):
        drvExtPhyDp83848ConfigRmii.setDefaultValue(True)
    elif "PIC32M" in Variables.get("__PROCESSOR"):
        drvExtPhyDp83848ConfigRmii.setDefaultValue(False)
    
        # Configuration Fuses Is ALT
        drvExtPhyDp83848ConfigAlt = drvExtPhyDp83848Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_ALTERNATE", drvExtPhyDp83848ConnFlag)
        drvExtPhyDp83848ConfigAlt.setHelp("mcc_h3_phy_configurations")
        drvExtPhyDp83848ConfigAlt.setLabel("Configuration Fuses Is ALT")
        drvExtPhyDp83848ConfigAlt.setVisible(True)
        drvExtPhyDp83848ConfigAlt.setDescription("Configuration Fuses Is ALT")
        drvExtPhyDp83848ConfigAlt.setDefaultValue(False)
        
        # Use The Fuses Configuration
        drvExtPhyDp83848ConfigAuto = drvExtPhyDp83848Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTO", drvExtPhyDp83848ConnFlag)
        drvExtPhyDp83848ConfigAuto.setHelp("mcc_h3_phy_configurations")
        drvExtPhyDp83848ConfigAuto.setLabel("Use The Fuses Configuration")
        drvExtPhyDp83848ConfigAuto.setVisible(True)
        drvExtPhyDp83848ConfigAuto.setDescription("Use The Fuses Configuration")
        drvExtPhyDp83848ConfigAuto.setDefaultValue(False)
        
    # Advanced Settings
    drvExtPhyDp83848AdvSettings = drvExtPhyDp83848Component.createMenuSymbol("TCPIP_INTMAC_PHY_ADV_SETTING", None)
    drvExtPhyDp83848AdvSettings.setLabel("Advanced Settings")
    drvExtPhyDp83848AdvSettings.setDescription("Advanced Settings")
    drvExtPhyDp83848AdvSettings.setVisible(True)
    
    # Delay for the Link Initialization in ms
    drvExtPhyDp83848LinkInitDelay= drvExtPhyDp83848Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", drvExtPhyDp83848AdvSettings)
    drvExtPhyDp83848LinkInitDelay.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDp83848LinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
    drvExtPhyDp83848LinkInitDelay.setVisible(True)
    drvExtPhyDp83848LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
    drvExtPhyDp83848LinkInitDelay.setDefaultValue(500)
    
    # External PHY Type
    drvExtPhyDp83848PhyType = drvExtPhyDp83848Component.createStringSymbol("TCPIP_EMAC_PHY_TYPE", drvExtPhyDp83848AdvSettings)
    drvExtPhyDp83848PhyType.setVisible(False)   
    drvExtPhyDp83848PhyType.setDefaultValue(phyName)
        
    # Driver PHY Negotiation Time-out (mSec)
    drvExtPhyDp83848NegInitTimeout= drvExtPhyDp83848Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", drvExtPhyDp83848AdvSettings)
    drvExtPhyDp83848NegInitTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDp83848NegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
    drvExtPhyDp83848NegInitTimeout.setVisible(True)
    drvExtPhyDp83848NegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
    drvExtPhyDp83848NegInitTimeout.setDefaultValue(1)
    
    # Driver PHY Negotiation Done Time-out (mSec)
    drvExtPhyDp83848NegDoneTimeout= drvExtPhyDp83848Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", drvExtPhyDp83848AdvSettings)
    drvExtPhyDp83848NegDoneTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDp83848NegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
    drvExtPhyDp83848NegDoneTimeout.setVisible(True)
    drvExtPhyDp83848NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
    drvExtPhyDp83848NegDoneTimeout.setDefaultValue(2000)
    
    # Driver PHY Reset Clear Time-out (mSec)
    drvExtPhyDp83848ResetClearTimeout= drvExtPhyDp83848Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", drvExtPhyDp83848AdvSettings)
    drvExtPhyDp83848ResetClearTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDp83848ResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
    drvExtPhyDp83848ResetClearTimeout.setVisible(True)
    drvExtPhyDp83848ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
    drvExtPhyDp83848ResetClearTimeout.setDefaultValue(500)

    # Driver PHY Instances Number
    drvExtPhyDp83848InstanceNum= drvExtPhyDp83848Component.createIntegerSymbol("DRV_ETHPHY_INSTANCES_NUMBER", drvExtPhyDp83848AdvSettings)
    drvExtPhyDp83848InstanceNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDp83848InstanceNum.setLabel("PHY Instances Number") 
    drvExtPhyDp83848InstanceNum.setVisible(True)
    drvExtPhyDp83848InstanceNum.setDescription("Driver PHY Instances Number")
    drvExtPhyDp83848InstanceNum.setDefaultValue(1)
    drvExtPhyDp83848InstanceNum.setReadOnly(True)
    
    # Driver PHY Clients Number
    drvExtPhyDp83848ClientNum= drvExtPhyDp83848Component.createIntegerSymbol("DRV_ETHPHY_CLIENTS_NUMBER", drvExtPhyDp83848AdvSettings)
    drvExtPhyDp83848ClientNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDp83848ClientNum.setLabel("PHY Clients Number") 
    drvExtPhyDp83848ClientNum.setVisible(True)
    drvExtPhyDp83848ClientNum.setDescription("Driver PHY Clients Number")
    drvExtPhyDp83848ClientNum.setDefaultValue(1)
    drvExtPhyDp83848ClientNum.setReadOnly(True)
    
    # Driver PHY Peripheral Index Number
    drvExtPhyDp83848IndexNum= drvExtPhyDp83848Component.createIntegerSymbol("DRV_ETHPHY_INDEX", drvExtPhyDp83848AdvSettings)
    drvExtPhyDp83848IndexNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDp83848IndexNum.setLabel("PHY Peripheral Index Number") 
    drvExtPhyDp83848IndexNum.setVisible(True)
    drvExtPhyDp83848IndexNum.setDescription("Driver PHY Peripheral Index Number")
    drvExtPhyDp83848IndexNum.setDefaultValue(1)
    drvExtPhyDp83848IndexNum.setReadOnly(True)
    
    # Driver PHY Peripheral ID
    drvExtPhyDp83848PeripheralId= drvExtPhyDp83848Component.createStringSymbol("DRV_ETHPHY_PERIPHERAL_ID", drvExtPhyDp83848AdvSettings)
    drvExtPhyDp83848PeripheralId.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDp83848PeripheralId.setLabel("PHY Peripheral ID") 
    drvExtPhyDp83848PeripheralId.setVisible(True)
    drvExtPhyDp83848PeripheralId.setDescription("Driver PHY Peripheral ID")
    drvExtPhyDp83848PeripheralId.setDefaultValue("")
    drvExtPhyDp83848PeripheralId.setReadOnly(True)

    # External MAC Name
    drvExtPhyMacName= drvExtPhyDp83848Component.createStringSymbol("DRV_ETHPHY_MAC_NAME", None)
    drvExtPhyMacName.setLabel("Mac Name") 
    drvExtPhyMacName.setVisible(False)   
    drvExtPhyMacName.setDefaultValue("")
    drvExtPhyMacName.setReadOnly(True) 
                                        
    #Add forward declaration to initialization.c
    drvExtPhyDp83848InitDataSourceFtl = drvExtPhyDp83848Component.createFileSymbol(None, None)
    drvExtPhyDp83848InitDataSourceFtl.setType("STRING")
    drvExtPhyDp83848InitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    drvExtPhyDp83848InitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_driver_initialize.c.ftl")
    drvExtPhyDp83848InitDataSourceFtl.setMarkup(True)    
    
    #Add to initialization.c
    drvExtPhyDp83848SysInitDataSourceFtl = drvExtPhyDp83848Component.createFileSymbol(None, None)
    drvExtPhyDp83848SysInitDataSourceFtl.setType("STRING")
    drvExtPhyDp83848SysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    drvExtPhyDp83848SysInitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_data_initialize.c.ftl")
    drvExtPhyDp83848SysInitDataSourceFtl.setMarkup(True)    
    
    #Add to system_config.h
    drvExtPhyDp83848HeaderFtl = drvExtPhyDp83848Component.createFileSymbol(None, None)
    drvExtPhyDp83848HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_dp83848.h.ftl")
    drvExtPhyDp83848HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    drvExtPhyDp83848HeaderFtl.setMarkup(True)
    drvExtPhyDp83848HeaderFtl.setType("STRING")
    
    # file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
    # Add drv_ethphy.h file to project
    drvExtPhyHeaderFile = drvExtPhyDp83848Component.createFileSymbol(None, None)
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
    drvExtPhyLocalHeaderFile = drvExtPhyDp83848Component.createFileSymbol(None, None)
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
    drvExtPhyDp83848RegHeaderFile = drvExtPhyDp83848Component.createFileSymbol(None, None)
    drvExtPhyDp83848RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
    drvExtPhyDp83848RegHeaderFile.setOutputName("drv_extphy_regs.h")
    drvExtPhyDp83848RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyDp83848RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyDp83848RegHeaderFile.setType("HEADER")
    drvExtPhyDp83848RegHeaderFile.setOverwrite(True)
    drvExtPhyDp83848RegHeaderFile.setEnabled(True)

    # Add drv_extphy_dp83848.h file to project
    drvExtPhyDp83848HeaderFile = drvExtPhyDp83848Component.createFileSymbol(None, None)
    drvExtPhyDp83848HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_dp83848.h")
    drvExtPhyDp83848HeaderFile.setOutputName("drv_extphy_dp83848.h")
    drvExtPhyDp83848HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyDp83848HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyDp83848HeaderFile.setType("HEADER")
    drvExtPhyDp83848HeaderFile.setOverwrite(True)
    drvExtPhyDp83848HeaderFile.setEnabled(True)

    # file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
    # Add drv_ethphy.c file
    drvExtPhySourceFile = drvExtPhyDp83848Component.createFileSymbol(None, None)
    drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
    drvExtPhySourceFile.setOutputName("drv_ethphy.c")
    drvExtPhySourceFile.setOverwrite(True)
    drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setType("SOURCE")
    drvExtPhySourceFile.setEnabled(True)

    # Add drv_extphy_dp83848.c file
    drvExtPhyDp83848SourceFile = drvExtPhyDp83848Component.createFileSymbol(None, None)
    drvExtPhyDp83848SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_dp83848.c")
    drvExtPhyDp83848SourceFile.setOutputName("drv_extphy_dp83848.c")
    drvExtPhyDp83848SourceFile.setOverwrite(True)
    drvExtPhyDp83848SourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyDp83848SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyDp83848SourceFile.setType("SOURCE")
    drvExtPhyDp83848SourceFile.setEnabled(True)

    
def drvExtPhyDp83848MenuVisibleSingle(symbol, event):
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
