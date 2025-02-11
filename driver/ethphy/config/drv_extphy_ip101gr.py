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

phyName = "IP101GR"
def instantiateComponent(drvExtPhyIp101grComponent):
    print("IP101GR PHY Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")

    # PHY Address
    drvExtPhyIp101grAddr= drvExtPhyIp101grComponent.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
    drvExtPhyIp101grAddr.setHelp("mcc_h3_phy_configurations")
    drvExtPhyIp101grAddr.setLabel("PHY Address") 
    drvExtPhyIp101grAddr.setVisible(True)
    drvExtPhyIp101grAddr.setDescription("PHY Address")
    drvExtPhyIp101grAddr.setDefaultValue(0)
    
    # Use a Function to be called at PHY Reset
    drvExtPhyIp101grResetCallbackEnable = drvExtPhyIp101grComponent.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
    drvExtPhyIp101grResetCallbackEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyIp101grResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
    drvExtPhyIp101grResetCallbackEnable.setVisible(True)
    drvExtPhyIp101grResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
    drvExtPhyIp101grResetCallbackEnable.setDefaultValue(False)
    
    # App Function
    drvExtPhyIp101grResetCallback = drvExtPhyIp101grComponent.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyIp101grResetCallbackEnable)
    drvExtPhyIp101grResetCallback.setHelp("mcc_h3_phy_configurations")
    drvExtPhyIp101grResetCallback.setLabel("App Function")
    drvExtPhyIp101grResetCallback.setVisible(False)
    drvExtPhyIp101grResetCallback.setDescription("App Function")
    drvExtPhyIp101grResetCallback.setDefaultValue("App" + phyName + "ResetFunction")
    drvExtPhyIp101grResetCallback.setDependencies(drvExtPhyIp101grMenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])

    # External PHY Connection Flags
    drvExtPhyIp101grConnFlag = drvExtPhyIp101grComponent.createMenuSymbol(None, None) 
    drvExtPhyIp101grConnFlag.setLabel("External PHY Connection Flags")
    drvExtPhyIp101grConnFlag.setVisible(True)
    drvExtPhyIp101grConnFlag.setDescription("External PHY Connection Flags")
    
    # RMII Data Interface
    drvExtPhyIp101grConfigRmii = drvExtPhyIp101grComponent.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyIp101grConnFlag)
    drvExtPhyIp101grConfigRmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyIp101grConfigRmii.setLabel("RMII Data Interface")
    drvExtPhyIp101grConfigRmii.setVisible(True)
    drvExtPhyIp101grConfigRmii.setDescription("RMII Data Interface")
    if Peripheral.moduleExists("GMAC"):
        drvExtPhyIp101grConfigRmii.setDefaultValue(True)
    elif "PIC32M" in Variables.get("__PROCESSOR"):
        drvExtPhyIp101grConfigRmii.setDefaultValue(False)
    
        # Configuration Fuses Is ALT
        drvExtPhyIp101grConfigAlt = drvExtPhyIp101grComponent.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_ALTERNATE", drvExtPhyIp101grConnFlag)
        drvExtPhyIp101grConfigAlt.setHelp("mcc_h3_phy_configurations")
        drvExtPhyIp101grConfigAlt.setLabel("Configuration Fuses Is ALT")
        drvExtPhyIp101grConfigAlt.setVisible(True)
        drvExtPhyIp101grConfigAlt.setDescription("Configuration Fuses Is ALT")
        drvExtPhyIp101grConfigAlt.setDefaultValue(False)
    
        # Use The Fuses Configuration
        drvExtPhyIp101grConfigAuto = drvExtPhyIp101grComponent.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTO", drvExtPhyIp101grConnFlag)
        drvExtPhyIp101grConfigAuto.setHelp("mcc_h3_phy_configurations")
        drvExtPhyIp101grConfigAuto.setLabel("Use The Fuses Configuration")
        drvExtPhyIp101grConfigAuto.setVisible(True)
        drvExtPhyIp101grConfigAuto.setDescription("Use The Fuses Configuration")
        drvExtPhyIp101grConfigAuto.setDefaultValue(False)
        
    # Advanced Settings
    drvExtPhyIp101grAdvSettings = drvExtPhyIp101grComponent.createMenuSymbol("TCPIP_INTMAC_PHY_ADV_SETTING", None)
    drvExtPhyIp101grAdvSettings.setLabel("Advanced Settings")
    drvExtPhyIp101grAdvSettings.setDescription("Advanced Settings")
    drvExtPhyIp101grAdvSettings.setVisible(True)

    # Delay for the Link Initialization in ms
    drvExtPhyIp101grLinkInitDelay= drvExtPhyIp101grComponent.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", drvExtPhyIp101grAdvSettings)
    drvExtPhyIp101grLinkInitDelay.setHelp("mcc_h3_phy_configurations")
    drvExtPhyIp101grLinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
    drvExtPhyIp101grLinkInitDelay.setVisible(True)
    drvExtPhyIp101grLinkInitDelay.setDescription("Delay for the Link Initialization in ms")
    drvExtPhyIp101grLinkInitDelay.setDefaultValue(500)

    # External PHY Type
    drvExtPhyIp101grPhyType = drvExtPhyIp101grComponent.createStringSymbol("TCPIP_EMAC_PHY_TYPE", drvExtPhyIp101grAdvSettings)
    drvExtPhyIp101grPhyType.setVisible(False)   
    drvExtPhyIp101grPhyType.setDefaultValue(phyName)
    
    # Driver PHY Negotiation Time-out (mSec)
    drvExtPhyIp101grNegInitTimeout= drvExtPhyIp101grComponent.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", drvExtPhyIp101grAdvSettings)
    drvExtPhyIp101grNegInitTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyIp101grNegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
    drvExtPhyIp101grNegInitTimeout.setVisible(True)
    drvExtPhyIp101grNegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
    drvExtPhyIp101grNegInitTimeout.setDefaultValue(1)
    
    # Driver PHY Negotiation Done Time-out (mSec)
    drvExtPhyIp101grNegDoneTimeout= drvExtPhyIp101grComponent.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", drvExtPhyIp101grAdvSettings)
    drvExtPhyIp101grNegDoneTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyIp101grNegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
    drvExtPhyIp101grNegDoneTimeout.setVisible(True)
    drvExtPhyIp101grNegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
    drvExtPhyIp101grNegDoneTimeout.setDefaultValue(2000)
    
    # Driver PHY Reset Clear Time-out (mSec)
    drvExtPhyIp101grResetClearTimeout= drvExtPhyIp101grComponent.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", drvExtPhyIp101grAdvSettings)
    drvExtPhyIp101grResetClearTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyIp101grResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
    drvExtPhyIp101grResetClearTimeout.setVisible(True)
    drvExtPhyIp101grResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
    drvExtPhyIp101grResetClearTimeout.setDefaultValue(500)
    
    # Driver PHY Instances Number
    drvExtPhyIp101grInstanceNum= drvExtPhyIp101grComponent.createIntegerSymbol("DRV_ETHPHY_INSTANCES_NUMBER", drvExtPhyIp101grAdvSettings)
    drvExtPhyIp101grInstanceNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyIp101grInstanceNum.setLabel("PHY Instances Number") 
    drvExtPhyIp101grInstanceNum.setVisible(True)
    drvExtPhyIp101grInstanceNum.setDescription("Driver PHY Instances Number")
    drvExtPhyIp101grInstanceNum.setDefaultValue(1)
    drvExtPhyIp101grInstanceNum.setReadOnly(True)
    
    # Driver PHY Clients Number
    drvExtPhyIp101grClientNum= drvExtPhyIp101grComponent.createIntegerSymbol("DRV_ETHPHY_CLIENTS_NUMBER", drvExtPhyIp101grAdvSettings)
    drvExtPhyIp101grClientNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyIp101grClientNum.setLabel("PHY Clients Number") 
    drvExtPhyIp101grClientNum.setVisible(True)
    drvExtPhyIp101grClientNum.setDescription("Driver PHY Clients Number")
    drvExtPhyIp101grClientNum.setDefaultValue(1)
    drvExtPhyIp101grClientNum.setReadOnly(True)
    
    # Driver PHY Peripheral Index Number
    drvExtPhyIp101grIndexNum= drvExtPhyIp101grComponent.createIntegerSymbol("DRV_ETHPHY_INDEX", drvExtPhyIp101grAdvSettings)
    drvExtPhyIp101grIndexNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyIp101grIndexNum.setLabel("PHY Peripheral Index Number") 
    drvExtPhyIp101grIndexNum.setVisible(True)
    drvExtPhyIp101grIndexNum.setDescription("Driver PHY Peripheral Index Number")
    drvExtPhyIp101grIndexNum.setDefaultValue(1)
    drvExtPhyIp101grIndexNum.setReadOnly(True)
    
    # Driver PHY Peripheral ID
    drvExtPhyIp101grPeripheralId= drvExtPhyIp101grComponent.createStringSymbol("DRV_ETHPHY_PERIPHERAL_ID", drvExtPhyIp101grAdvSettings)
    drvExtPhyIp101grPeripheralId.setHelp("mcc_h3_phy_configurations")
    drvExtPhyIp101grPeripheralId.setLabel("PHY Peripheral ID") 
    drvExtPhyIp101grPeripheralId.setVisible(True)
    drvExtPhyIp101grPeripheralId.setDescription("Driver PHY Peripheral ID")
    drvExtPhyIp101grPeripheralId.setDefaultValue("")
    drvExtPhyIp101grPeripheralId.setReadOnly(True)

    # External MAC Name
    drvExtPhyMacName= drvExtPhyIp101grComponent.createStringSymbol("DRV_ETHPHY_MAC_NAME", None)
    drvExtPhyMacName.setLabel("Mac Name") 
    drvExtPhyMacName.setVisible(False)   
    drvExtPhyMacName.setDefaultValue("")
    drvExtPhyMacName.setReadOnly(True) 
                                    
    #Add forward declaration to initialization.c
    drvExtPhyIp101grInitDataSourceFtl = drvExtPhyIp101grComponent.createFileSymbol(None, None)
    drvExtPhyIp101grInitDataSourceFtl.setType("STRING")
    drvExtPhyIp101grInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    drvExtPhyIp101grInitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_driver_initialize.c.ftl")
    drvExtPhyIp101grInitDataSourceFtl.setMarkup(True)    
    
    #Add to initialization.c
    drvExtPhyIp101grSysInitDataSourceFtl = drvExtPhyIp101grComponent.createFileSymbol(None, None)
    drvExtPhyIp101grSysInitDataSourceFtl.setType("STRING")
    drvExtPhyIp101grSysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    drvExtPhyIp101grSysInitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_data_initialize.c.ftl")
    drvExtPhyIp101grSysInitDataSourceFtl.setMarkup(True)    
    
    #Add to system_config.h
    drvExtPhyIp101grHeaderFtl = drvExtPhyIp101grComponent.createFileSymbol(None, None)
    drvExtPhyIp101grHeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_ip101gr.h.ftl")
    drvExtPhyIp101grHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    drvExtPhyIp101grHeaderFtl.setMarkup(True)
    drvExtPhyIp101grHeaderFtl.setType("STRING")
    
    #Add to definitions.h
    drvExtPhyIp101grSysdefFtl = drvExtPhyIp101grComponent.createFileSymbol(None, None)
    drvExtPhyIp101grSysdefFtl.setSourcePath("driver/ethphy/templates/system/system_definitions.h.ftl")
    drvExtPhyIp101grSysdefFtl.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    drvExtPhyIp101grSysdefFtl.setMarkup(True)
    drvExtPhyIp101grSysdefFtl.setType("STRING")

    # file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
    # Add drv_ethphy.h file to project
    drvExtPhyHeaderFile = drvExtPhyIp101grComponent.createFileSymbol(None, None)
    drvExtPhyHeaderFile.setSourcePath("driver/ethphy/drv_ethphy.h")
    drvExtPhyHeaderFile.setOutputName("drv_ethphy.h")
    drvExtPhyHeaderFile.setDestPath("driver/ethphy/")
    drvExtPhyHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/")
    drvExtPhyHeaderFile.setType("HEADER")
    drvExtPhyHeaderFile.setOverwrite(True)
    drvExtPhyHeaderFile.setEnabled(True)

    # file TCPIP_ETH_PHY_LOCAL_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/drv_ethphy_local.h" to           "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/drv_ethphy_local.h"
    # Add drv_ethphy_local.h file to project
    drvExtPhyLocalHeaderFile = drvExtPhyIp101grComponent.createFileSymbol(None, None)
    drvExtPhyLocalHeaderFile.setSourcePath("driver/ethphy/src/drv_ethphy_local.h")
    drvExtPhyLocalHeaderFile.setOutputName("drv_ethphy_local.h")
    drvExtPhyLocalHeaderFile.setDestPath("driver/ethphy/src/")
    drvExtPhyLocalHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/")
    drvExtPhyLocalHeaderFile.setType("HEADER")
    drvExtPhyLocalHeaderFile.setOverwrite(True)
    drvExtPhyLocalHeaderFile.setEnabled(True)

    # file TCPIP_ETH_EXT_PHY_REGS_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h" to    "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h"
    # Add drv_extphy_regs.h file to project
    drvExtPhyIp101grRegHeaderFile = drvExtPhyIp101grComponent.createFileSymbol(None, None)
    drvExtPhyIp101grRegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
    drvExtPhyIp101grRegHeaderFile.setOutputName("drv_extphy_regs.h")
    drvExtPhyIp101grRegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyIp101grRegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyIp101grRegHeaderFile.setType("HEADER")
    drvExtPhyIp101grRegHeaderFile.setOverwrite(True)
    drvExtPhyIp101grRegHeaderFile.setEnabled(True)

    # Add drv_extphy_ip101gr.h file to project
    drvExtPhyIp101grHeaderFile = drvExtPhyIp101grComponent.createFileSymbol(None, None)
    drvExtPhyIp101grHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ip101gr.h")
    drvExtPhyIp101grHeaderFile.setOutputName("drv_extphy_ip101gr.h")
    drvExtPhyIp101grHeaderFile.setDestPath("driver/ethphy/")
    drvExtPhyIp101grHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/")
    drvExtPhyIp101grHeaderFile.setType("HEADER")
    drvExtPhyIp101grHeaderFile.setOverwrite(True)
    drvExtPhyIp101grHeaderFile.setEnabled(True)

    # Add drv_extphy_ip101gr_priv.h file to project
    drvExtPhyIp101grPrivFile = drvExtPhyIp101grComponent.createFileSymbol(None, None)
    drvExtPhyIp101grPrivFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ip101gr_priv.h")
    drvExtPhyIp101grPrivFile.setOutputName("drv_extphy_ip101gr_priv.h")
    drvExtPhyIp101grPrivFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyIp101grPrivFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyIp101grPrivFile.setType("HEADER")
    drvExtPhyIp101grPrivFile.setOverwrite(True)
    drvExtPhyIp101grPrivFile.setEnabled(True)

    # file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
    # Add drv_ethphy.c file
    drvExtPhySourceFile = drvExtPhyIp101grComponent.createFileSymbol(None, None)
    drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
    drvExtPhySourceFile.setOutputName("drv_ethphy.c")
    drvExtPhySourceFile.setOverwrite(True)
    drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setType("SOURCE")
    drvExtPhySourceFile.setEnabled(True)

    # Add drv_extphy_ip101gr.c file
    drvExtPhyIp101grSourceFile = drvExtPhyIp101grComponent.createFileSymbol(None, None)
    drvExtPhyIp101grSourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ip101gr.c")
    drvExtPhyIp101grSourceFile.setOutputName("drv_extphy_ip101gr.c")
    drvExtPhyIp101grSourceFile.setOverwrite(True)
    drvExtPhyIp101grSourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyIp101grSourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyIp101grSourceFile.setType("SOURCE")
    drvExtPhyIp101grSourceFile.setEnabled(True)

    
def drvExtPhyIp101grMenuVisibleSingle(symbol, event):
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
