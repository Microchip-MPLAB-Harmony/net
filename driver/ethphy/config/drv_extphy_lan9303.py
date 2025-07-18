"""
Copyright (C) 2020-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

phyName = "LAN9303"
def instantiateComponent(drvExtPhyLan9303Component):
    print("LAN9303 PHY Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")

    # PHY Address
    drvExtPhyLan9303Addr= drvExtPhyLan9303Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
    drvExtPhyLan9303Addr.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan9303Addr.setLabel("PHY Address") 
    drvExtPhyLan9303Addr.setVisible(True)
    drvExtPhyLan9303Addr.setDescription("PHY Address")
    drvExtPhyLan9303Addr.setDefaultValue(0)
    
    # Use a Function to be called at PHY Reset
    drvExtPhyLan9303ResetCallbackEnable = drvExtPhyLan9303Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
    drvExtPhyLan9303ResetCallbackEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan9303ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
    drvExtPhyLan9303ResetCallbackEnable.setVisible(True)
    drvExtPhyLan9303ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
    drvExtPhyLan9303ResetCallbackEnable.setDefaultValue(False)
    
    # App Function
    drvExtPhyLan9303ResetCallback = drvExtPhyLan9303Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyLan9303ResetCallbackEnable)
    drvExtPhyLan9303ResetCallback.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan9303ResetCallback.setLabel("App Function")
    drvExtPhyLan9303ResetCallback.setVisible(False)
    drvExtPhyLan9303ResetCallback.setDescription("App Function")
    drvExtPhyLan9303ResetCallback.setDefaultValue("App" + phyName + "ResetFunction")
    drvExtPhyLan9303ResetCallback.setDependencies(drvExtPhyLan9303MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])

    # External PHY Connection Flags
    drvExtPhyLan9303ConnFlag = drvExtPhyLan9303Component.createMenuSymbol(None, None) 
    drvExtPhyLan9303ConnFlag.setLabel("External PHY Connection Flags")
    drvExtPhyLan9303ConnFlag.setVisible(True)
    drvExtPhyLan9303ConnFlag.setDescription("External PHY Connection Flags")
    
    # RMII Data Interface
    drvExtPhyLan9303ConfigRmii = drvExtPhyLan9303Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyLan9303ConnFlag)
    drvExtPhyLan9303ConfigRmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan9303ConfigRmii.setLabel("RMII Data Interface")
    drvExtPhyLan9303ConfigRmii.setVisible(True)
    drvExtPhyLan9303ConfigRmii.setDescription("RMII Data Interface")
    drvExtPhyLan9303ConfigRmii.setDefaultValue(True)

    if ("PIC32M" in Variables.get("__PROCESSOR")) or ("WFI32" in Variables.get("__PROCESSOR")):
        # default is Auto for PIC32M
        drvExtPhyLan9303ConfigRmii.setDefaultValue(False)
    
        # Configuration Fuses Is ALT
        drvExtPhyLan9303ConfigAlt = drvExtPhyLan9303Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_ALTERNATE", drvExtPhyLan9303ConnFlag)
        drvExtPhyLan9303ConfigAlt.setHelp("mcc_h3_phy_configurations")
        drvExtPhyLan9303ConfigAlt.setLabel("Configuration Fuses Is ALT")
        drvExtPhyLan9303ConfigAlt.setVisible(True)
        drvExtPhyLan9303ConfigAlt.setDescription("Configuration Fuses Is ALT")
        drvExtPhyLan9303ConfigAlt.setDefaultValue(False)
        
        # Use The Fuses Configuration
        drvExtPhyLan9303ConfigAuto = drvExtPhyLan9303Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTO", drvExtPhyLan9303ConnFlag)
        drvExtPhyLan9303ConfigAuto.setHelp("mcc_h3_phy_configurations")
        drvExtPhyLan9303ConfigAuto.setLabel("Use The Fuses Configuration")
        drvExtPhyLan9303ConfigAuto.setVisible(True)
        drvExtPhyLan9303ConfigAuto.setDescription("Use The Fuses Configuration")
        drvExtPhyLan9303ConfigAuto.setDefaultValue(True)
    
    # Advanced Settings
    drvExtPhyLan9303AdvSettings = drvExtPhyLan9303Component.createMenuSymbol("TCPIP_INTMAC_PHY_ADV_SETTING", None)
    drvExtPhyLan9303AdvSettings.setLabel("Advanced Settings")
    drvExtPhyLan9303AdvSettings.setDescription("Advanced Settings")
    drvExtPhyLan9303AdvSettings.setVisible(True)

    # Delay for the Link Initialization in ms
    drvExtPhyLan9303LinkInitDelay= drvExtPhyLan9303Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", drvExtPhyLan9303AdvSettings)
    drvExtPhyLan9303LinkInitDelay.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan9303LinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
    drvExtPhyLan9303LinkInitDelay.setVisible(True)
    drvExtPhyLan9303LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
    drvExtPhyLan9303LinkInitDelay.setDefaultValue(500)
    
    # External PHY Type
    drvExtPhyLan9303PhyType = drvExtPhyLan9303Component.createStringSymbol("TCPIP_EMAC_PHY_TYPE", drvExtPhyLan9303AdvSettings)
    drvExtPhyLan9303PhyType.setVisible(False)   
    drvExtPhyLan9303PhyType.setDefaultValue(phyName)
    
    # Driver PHY Negotiation Time-out (mSec)
    drvExtPhyLan9303NegInitTimeout= drvExtPhyLan9303Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", drvExtPhyLan9303AdvSettings)
    drvExtPhyLan9303NegInitTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan9303NegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
    drvExtPhyLan9303NegInitTimeout.setVisible(True)
    drvExtPhyLan9303NegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
    drvExtPhyLan9303NegInitTimeout.setDefaultValue(1)
    
    # Driver PHY Negotiation Done Time-out (mSec)
    drvExtPhyLan9303NegDoneTimeout= drvExtPhyLan9303Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", drvExtPhyLan9303AdvSettings)
    drvExtPhyLan9303NegDoneTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan9303NegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
    drvExtPhyLan9303NegDoneTimeout.setVisible(True)
    drvExtPhyLan9303NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
    drvExtPhyLan9303NegDoneTimeout.setDefaultValue(2000)
    
    # Driver PHY Reset Clear Time-out (mSec)
    drvExtPhyLan9303ResetClearTimeout= drvExtPhyLan9303Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", drvExtPhyLan9303AdvSettings)
    drvExtPhyLan9303ResetClearTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan9303ResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
    drvExtPhyLan9303ResetClearTimeout.setVisible(True)
    drvExtPhyLan9303ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
    drvExtPhyLan9303ResetClearTimeout.setDefaultValue(500)

    # Driver PHY Instances Number
    drvExtPhyLan9303InstanceNum= drvExtPhyLan9303Component.createIntegerSymbol("DRV_ETHPHY_INSTANCES_NUMBER", drvExtPhyLan9303AdvSettings)
    drvExtPhyLan9303InstanceNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan9303InstanceNum.setLabel("PHY Instances Number") 
    drvExtPhyLan9303InstanceNum.setVisible(True)
    drvExtPhyLan9303InstanceNum.setDescription("Driver PHY Instances Number")
    drvExtPhyLan9303InstanceNum.setDefaultValue(1)
    drvExtPhyLan9303InstanceNum.setReadOnly(True)
    
    # Driver PHY Clients Number
    drvExtPhyLan9303ClientNum= drvExtPhyLan9303Component.createIntegerSymbol("DRV_ETHPHY_CLIENTS_NUMBER", drvExtPhyLan9303AdvSettings)
    drvExtPhyLan9303ClientNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan9303ClientNum.setLabel("PHY Clients Number") 
    drvExtPhyLan9303ClientNum.setVisible(True)
    drvExtPhyLan9303ClientNum.setDescription("Driver PHY Clients Number")
    drvExtPhyLan9303ClientNum.setDefaultValue(1)
    drvExtPhyLan9303ClientNum.setReadOnly(True)
    
    # Driver PHY Peripheral Index Number
    drvExtPhyLan9303IndexNum= drvExtPhyLan9303Component.createIntegerSymbol("DRV_ETHPHY_INDEX", drvExtPhyLan9303AdvSettings)
    drvExtPhyLan9303IndexNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan9303IndexNum.setLabel("PHY Peripheral Index Number") 
    drvExtPhyLan9303IndexNum.setVisible(True)
    drvExtPhyLan9303IndexNum.setDescription("Driver PHY Peripheral Index Number")
    drvExtPhyLan9303IndexNum.setDefaultValue(1)
    drvExtPhyLan9303IndexNum.setReadOnly(True)
    
    # Driver PHY Peripheral ID
    drvExtPhyLan9303PeripheralId= drvExtPhyLan9303Component.createStringSymbol("DRV_ETHPHY_PERIPHERAL_ID", drvExtPhyLan9303AdvSettings)
    drvExtPhyLan9303PeripheralId.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan9303PeripheralId.setLabel("PHY Peripheral ID") 
    drvExtPhyLan9303PeripheralId.setVisible(True)
    drvExtPhyLan9303PeripheralId.setDescription("Driver PHY Peripheral ID")
    drvExtPhyLan9303PeripheralId.setDefaultValue("")
    drvExtPhyLan9303PeripheralId.setReadOnly(True)

    # External MAC Name
    drvExtPhyMacName= drvExtPhyLan9303Component.createStringSymbol("DRV_ETHPHY_MAC_NAME", None)
    drvExtPhyMacName.setLabel("Mac Name") 
    drvExtPhyMacName.setVisible(False)   
    drvExtPhyMacName.setDefaultValue("")
    drvExtPhyMacName.setReadOnly(True) 
                    
    #Add forward declaration to initialization.c
    drvExtPhyLan9303InitDataSourceFtl = drvExtPhyLan9303Component.createFileSymbol(None, None)
    drvExtPhyLan9303InitDataSourceFtl.setType("STRING")
    drvExtPhyLan9303InitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    drvExtPhyLan9303InitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_driver_initialize.c.ftl")
    drvExtPhyLan9303InitDataSourceFtl.setMarkup(True)    
    
    #Add to initialization.c
    drvExtPhyLan9303SysInitDataSourceFtl = drvExtPhyLan9303Component.createFileSymbol(None, None)
    drvExtPhyLan9303SysInitDataSourceFtl.setType("STRING")
    drvExtPhyLan9303SysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    drvExtPhyLan9303SysInitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_data_initialize.c.ftl")
    drvExtPhyLan9303SysInitDataSourceFtl.setMarkup(True)    
    
    #Add to system_config.h
    drvExtPhyLan9303HeaderFtl = drvExtPhyLan9303Component.createFileSymbol(None, None)
    drvExtPhyLan9303HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_lan9303.h.ftl")
    drvExtPhyLan9303HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    drvExtPhyLan9303HeaderFtl.setMarkup(True)
    drvExtPhyLan9303HeaderFtl.setType("STRING")
    
    #Add to definitions.h
    drvExtPhyLan9303SysdefFtl = drvExtPhyLan9303Component.createFileSymbol(None, None)
    drvExtPhyLan9303SysdefFtl.setSourcePath("driver/ethphy/templates/system/system_definitions.h.ftl")
    drvExtPhyLan9303SysdefFtl.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    drvExtPhyLan9303SysdefFtl.setMarkup(True)
    drvExtPhyLan9303SysdefFtl.setType("STRING")

    # file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
    # Add drv_ethphy.h file to project
    drvExtPhyHeaderFile = drvExtPhyLan9303Component.createFileSymbol(None, None)
    drvExtPhyHeaderFile.setSourcePath("driver/ethphy/drv_ethphy.h")
    drvExtPhyHeaderFile.setOutputName("drv_ethphy.h")
    drvExtPhyHeaderFile.setDestPath("driver/ethphy/")
    drvExtPhyHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/")
    drvExtPhyHeaderFile.setType("HEADER")
    drvExtPhyHeaderFile.setOverwrite(True)
    drvExtPhyHeaderFile.setEnabled(True)

    # file TCPIP_ETH_PHY_LOCAL_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/drv_ethphy_local.h" to           "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/drv_ethphy_local.h"
    # Add drv_ethphy_local.h file to project
    drvExtPhyLocalHeaderFile = drvExtPhyLan9303Component.createFileSymbol(None, None)
    drvExtPhyLocalHeaderFile.setSourcePath("driver/ethphy/src/drv_ethphy_local.h")
    drvExtPhyLocalHeaderFile.setOutputName("drv_ethphy_local.h")
    drvExtPhyLocalHeaderFile.setDestPath("driver/ethphy/src/")
    drvExtPhyLocalHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/")
    drvExtPhyLocalHeaderFile.setType("HEADER")
    drvExtPhyLocalHeaderFile.setOverwrite(True)
    drvExtPhyLocalHeaderFile.setEnabled(True)

    # file TCPIP_ETH_EXT_PHY_REGS_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h" to    "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h"
    # Add drv_extphy_regs.h file to project
    drvExtPhyLan9303RegHeaderFile = drvExtPhyLan9303Component.createFileSymbol(None, None)
    drvExtPhyLan9303RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
    drvExtPhyLan9303RegHeaderFile.setOutputName("drv_extphy_regs.h")
    drvExtPhyLan9303RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyLan9303RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyLan9303RegHeaderFile.setType("HEADER")
    drvExtPhyLan9303RegHeaderFile.setOverwrite(True)
    drvExtPhyLan9303RegHeaderFile.setEnabled(True)

    # Add drv_extphy_lan9303.h file to project
    drvExtPhyLan9303HeaderFile = drvExtPhyLan9303Component.createFileSymbol(None, None)
    drvExtPhyLan9303HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_lan9303.h")
    drvExtPhyLan9303HeaderFile.setOutputName("drv_extphy_lan9303.h")
    drvExtPhyLan9303HeaderFile.setDestPath("driver/ethphy/")
    drvExtPhyLan9303HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/")
    drvExtPhyLan9303HeaderFile.setType("HEADER")
    drvExtPhyLan9303HeaderFile.setOverwrite(True)
    drvExtPhyLan9303HeaderFile.setEnabled(True)

    # Add drv_extphy_lan9303_priv.h file to project
    drvExtPhyLan9303PrivFile = drvExtPhyLan9303Component.createFileSymbol(None, None)
    drvExtPhyLan9303PrivFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_lan9303_priv.h")
    drvExtPhyLan9303PrivFile.setOutputName("drv_extphy_lan9303_priv.h")
    drvExtPhyLan9303PrivFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyLan9303PrivFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyLan9303PrivFile.setType("HEADER")
    drvExtPhyLan9303PrivFile.setOverwrite(True)
    drvExtPhyLan9303PrivFile.setEnabled(True)


    # file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
    # Add drv_ethphy.c file
    drvExtPhySourceFile = drvExtPhyLan9303Component.createFileSymbol(None, None)
    drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
    drvExtPhySourceFile.setOutputName("drv_ethphy.c")
    drvExtPhySourceFile.setOverwrite(True)
    drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setType("SOURCE")
    drvExtPhySourceFile.setEnabled(True)

    # Add drv_extphy_lan9303.c file
    drvExtPhyLan9303SourceFile = drvExtPhyLan9303Component.createFileSymbol(None, None)
    drvExtPhyLan9303SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_lan9303.c")
    drvExtPhyLan9303SourceFile.setOutputName("drv_extphy_lan9303.c")
    drvExtPhyLan9303SourceFile.setOverwrite(True)
    drvExtPhyLan9303SourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyLan9303SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyLan9303SourceFile.setType("SOURCE")
    drvExtPhyLan9303SourceFile.setEnabled(True)

    
def drvExtPhyLan9303MenuVisibleSingle(symbol, event):
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
