"""
Copyright (C) 2024-2025 , Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

phyName = "LAN9354"
def instantiateComponent(drvExtPhyLan9354Component):
    print("LAN9354 PHY Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")

    # PHY Address
    drvExtPhyLan9354Addr= drvExtPhyLan9354Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
    drvExtPhyLan9354Addr.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan9354Addr.setLabel("PHY Address") 
    drvExtPhyLan9354Addr.setVisible(True)
    drvExtPhyLan9354Addr.setDescription("PHY Address")
    drvExtPhyLan9354Addr.setDefaultValue(0)
    
    # Use a Function to be called at PHY Reset
    drvExtPhyLan9354ResetCallbackEnable = drvExtPhyLan9354Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
    drvExtPhyLan9354ResetCallbackEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan9354ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
    drvExtPhyLan9354ResetCallbackEnable.setVisible(True)
    drvExtPhyLan9354ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
    drvExtPhyLan9354ResetCallbackEnable.setDefaultValue(False)
    
    # App Function
    drvExtPhyLan9354ResetCallback = drvExtPhyLan9354Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyLan9354ResetCallbackEnable)
    drvExtPhyLan9354ResetCallback.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan9354ResetCallback.setLabel("App Function")
    drvExtPhyLan9354ResetCallback.setVisible(False)
    drvExtPhyLan9354ResetCallback.setDescription("App Function")
    drvExtPhyLan9354ResetCallback.setDefaultValue("App" + phyName + "ResetFunction")
    drvExtPhyLan9354ResetCallback.setDependencies(drvExtPhyLan9354MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])
    
    # External PHY Connection Flags
    drvExtPhyLan9354ConnFlag = drvExtPhyLan9354Component.createMenuSymbol(None, None) 
    drvExtPhyLan9354ConnFlag.setLabel("External PHY Connection Flags")
    drvExtPhyLan9354ConnFlag.setVisible(True)
    drvExtPhyLan9354ConnFlag.setDescription("External PHY Connection Flags")
    
    # RMII Data Interface
    drvExtPhyLan9354ConfigRmii = drvExtPhyLan9354Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyLan9354ConnFlag)
    drvExtPhyLan9354ConfigRmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan9354ConfigRmii.setLabel("RMII Data Interface")
    drvExtPhyLan9354ConfigRmii.setVisible(True)
    drvExtPhyLan9354ConfigRmii.setReadOnly(True)
    drvExtPhyLan9354ConfigRmii.setDescription("RMII Data Interface")
    drvExtPhyLan9354ConfigRmii.setDefaultValue(True)
    
    if ("PIC32M" in Variables.get("__PROCESSOR")) or ("WFI32" in Variables.get("__PROCESSOR")):
        # default is Auto for PIC32M
        drvExtPhyLan9354ConfigRmii.setDefaultValue(False)
    
        # Configuration Fuses Is ALT
        drvExtPhyLan9354ConfigAlt = drvExtPhyLan9354Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_ALTERNATE", drvExtPhyLan9354ConnFlag)
        drvExtPhyLan9354ConfigAlt.setHelp("mcc_h3_phy_configurations")
        drvExtPhyLan9354ConfigAlt.setLabel("Configuration Fuses Is ALT")
        drvExtPhyLan9354ConfigAlt.setVisible(True)
        drvExtPhyLan9354ConfigAlt.setDescription("Configuration Fuses Is ALT")
        drvExtPhyLan9354ConfigAlt.setDefaultValue(False)
        
        # Use The Fuses Configuration
        drvExtPhyLan9354ConfigAuto = drvExtPhyLan9354Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTO", drvExtPhyLan9354ConnFlag)
        drvExtPhyLan9354ConfigAuto.setHelp("mcc_h3_phy_configurations")
        drvExtPhyLan9354ConfigAuto.setLabel("Use The Fuses Configuration")
        drvExtPhyLan9354ConfigAuto.setVisible(True)
        drvExtPhyLan9354ConfigAuto.setDescription("Use The Fuses Configuration")
        drvExtPhyLan9354ConfigAuto.setDefaultValue(True)
    
    # Advanced Settings
    drvExtPhyLan9354AdvSettings = drvExtPhyLan9354Component.createMenuSymbol("TCPIP_INTMAC_PHY_ADV_SETTING", None)
    drvExtPhyLan9354AdvSettings.setLabel("Advanced Settings")
    drvExtPhyLan9354AdvSettings.setDescription("Advanced Settings")
    drvExtPhyLan9354AdvSettings.setVisible(True)

    # Delay for the Link Initialization in ms
    drvExtPhyLan9354LinkInitDelay= drvExtPhyLan9354Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", drvExtPhyLan9354AdvSettings)
    drvExtPhyLan9354LinkInitDelay.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan9354LinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
    drvExtPhyLan9354LinkInitDelay.setVisible(True)
    drvExtPhyLan9354LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
    drvExtPhyLan9354LinkInitDelay.setDefaultValue(500)
      
    # External PHY Type
    drvExtPhyLan9354PhyType = drvExtPhyLan9354Component.createStringSymbol("TCPIP_EMAC_PHY_TYPE", drvExtPhyLan9354AdvSettings)
    #drvExtPhyLan9354PhyType.setLabel("External PHY Type")
    drvExtPhyLan9354PhyType.setVisible(False)
    #drvExtPhyLan9354PhyType.setDescription("External PHY Type")
    drvExtPhyLan9354PhyType.setDefaultValue(phyName)
    
    # Driver PHY Negotiation Time-out - ms
    drvExtPhyLan9354NegInitTimeout= drvExtPhyLan9354Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", drvExtPhyLan9354AdvSettings)
    drvExtPhyLan9354NegInitTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan9354NegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
    drvExtPhyLan9354NegInitTimeout.setVisible(True)
    drvExtPhyLan9354NegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
    drvExtPhyLan9354NegInitTimeout.setDefaultValue(1)
    
    # Driver PHY Negotiation Done Time-out - ms
    drvExtPhyLan9354NegDoneTimeout= drvExtPhyLan9354Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", drvExtPhyLan9354AdvSettings)
    drvExtPhyLan9354NegDoneTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan9354NegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
    drvExtPhyLan9354NegDoneTimeout.setVisible(True)
    drvExtPhyLan9354NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
    drvExtPhyLan9354NegDoneTimeout.setDefaultValue(2000)
    
    # Driver PHY Reset Clear Time-out - ms
    drvExtPhyLan9354ResetClearTimeout= drvExtPhyLan9354Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", drvExtPhyLan9354AdvSettings)
    drvExtPhyLan9354ResetClearTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan9354ResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
    drvExtPhyLan9354ResetClearTimeout.setVisible(True)
    drvExtPhyLan9354ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
    drvExtPhyLan9354ResetClearTimeout.setDefaultValue(500)
    
    # Driver PHY Peripheral ID
    drvExtPhyLan9354PeripheralId= drvExtPhyLan9354Component.createStringSymbol("DRV_ETHPHY_PERIPHERAL_ID", drvExtPhyLan9354AdvSettings)
    drvExtPhyLan9354PeripheralId.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan9354PeripheralId.setLabel("PHY Peripheral ID") 
    drvExtPhyLan9354PeripheralId.setVisible(True)
    drvExtPhyLan9354PeripheralId.setDescription("Driver PHY Peripheral ID")
    drvExtPhyLan9354PeripheralId.setDefaultValue("")
    drvExtPhyLan9354PeripheralId.setReadOnly(True)

    # External MAC Name
    drvExtPhyMacName= drvExtPhyLan9354Component.createStringSymbol("DRV_ETHPHY_MAC_NAME", None)
    drvExtPhyMacName.setLabel("Mac Name") 
    drvExtPhyMacName.setVisible(False)   
    drvExtPhyMacName.setDefaultValue("")
    drvExtPhyMacName.setReadOnly(True) 
                
    #Add forward declaration to initialization.c
    drvExtPhyLan9354InitDataSourceFtl = drvExtPhyLan9354Component.createFileSymbol(None, None)
    drvExtPhyLan9354InitDataSourceFtl.setType("STRING")
    drvExtPhyLan9354InitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    drvExtPhyLan9354InitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_driver_initialize.c.ftl")
    drvExtPhyLan9354InitDataSourceFtl.setMarkup(True)    
    
    #Add to initialization.c
    drvExtPhyLan9354SysInitDataSourceFtl = drvExtPhyLan9354Component.createFileSymbol(None, None)
    drvExtPhyLan9354SysInitDataSourceFtl.setType("STRING")
    drvExtPhyLan9354SysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    drvExtPhyLan9354SysInitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_data_initialize.c.ftl")
    drvExtPhyLan9354SysInitDataSourceFtl.setMarkup(True)    
    
    #Add to system_config.h
    drvExtPhyLan9354HeaderFtl = drvExtPhyLan9354Component.createFileSymbol(None, None)
    drvExtPhyLan9354HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_lan9354.h.ftl")
    drvExtPhyLan9354HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    drvExtPhyLan9354HeaderFtl.setMarkup(True)
    drvExtPhyLan9354HeaderFtl.setType("STRING") 
    
    #Add to definitions.h
    drvExtPhyLan9354SysdefFtl = drvExtPhyLan9354Component.createFileSymbol(None, None)
    drvExtPhyLan9354SysdefFtl.setSourcePath("driver/ethphy/templates/system/system_definitions.h.ftl")
    drvExtPhyLan9354SysdefFtl.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    drvExtPhyLan9354SysdefFtl.setMarkup(True)
    drvExtPhyLan9354SysdefFtl.setType("STRING")

    # file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
    # Add drv_ethphy.h file to project
    drvExtPhyHeaderFile = drvExtPhyLan9354Component.createFileSymbol(None, None)
    drvExtPhyHeaderFile.setSourcePath("driver/ethphy/drv_ethphy.h")
    drvExtPhyHeaderFile.setOutputName("drv_ethphy.h")
    drvExtPhyHeaderFile.setDestPath("driver/ethphy/")
    drvExtPhyHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/")
    drvExtPhyHeaderFile.setType("HEADER")
    drvExtPhyHeaderFile.setOverwrite(True)
    drvExtPhyHeaderFile.setEnabled(True)

    # file TCPIP_ETH_PHY_LOCAL_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/drv_ethphy_local.h" to           "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/drv_ethphy_local.h"
    # Add drv_ethphy_local.h file to project
    drvExtPhyLocalHeaderFile = drvExtPhyLan9354Component.createFileSymbol(None, None)
    drvExtPhyLocalHeaderFile.setSourcePath("driver/ethphy/src/drv_ethphy_local.h")
    drvExtPhyLocalHeaderFile.setOutputName("drv_ethphy_local.h")
    drvExtPhyLocalHeaderFile.setDestPath("driver/ethphy/src/")
    drvExtPhyLocalHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/")
    drvExtPhyLocalHeaderFile.setType("HEADER")
    drvExtPhyLocalHeaderFile.setOverwrite(True)
    drvExtPhyLocalHeaderFile.setEnabled(True)

    # file TCPIP_ETH_EXT_PHY_REGS_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h" to    "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h"
    # Add drv_extphy_regs.h file to project
    drvExtPhyLan9354RegHeaderFile = drvExtPhyLan9354Component.createFileSymbol(None, None)
    drvExtPhyLan9354RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
    drvExtPhyLan9354RegHeaderFile.setOutputName("drv_extphy_regs.h")
    drvExtPhyLan9354RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyLan9354RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyLan9354RegHeaderFile.setType("HEADER")
    drvExtPhyLan9354RegHeaderFile.setOverwrite(True)
    drvExtPhyLan9354RegHeaderFile.setEnabled(True)

    # file TCPIP_MAC_PHY_LAN9354_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_lan9354.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_lan9354.h"
    # Add drv_extphy_lan9354.h file to project
    drvExtPhyLan9354HeaderFile = drvExtPhyLan9354Component.createFileSymbol(None, None)
    drvExtPhyLan9354HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_lan9354.h")
    drvExtPhyLan9354HeaderFile.setOutputName("drv_extphy_lan9354.h")
    drvExtPhyLan9354HeaderFile.setDestPath("driver/ethphy/")
    drvExtPhyLan9354HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/")
    drvExtPhyLan9354HeaderFile.setType("HEADER")
    drvExtPhyLan9354HeaderFile.setOverwrite(True)
    drvExtPhyLan9354HeaderFile.setEnabled(True)

    # Add drv_extphy_lan9354_priv.h file to project
    drvExtPhyLan9354PrivFile = drvExtPhyLan9354Component.createFileSymbol(None, None)
    drvExtPhyLan9354PrivFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_lan9354_priv.h")
    drvExtPhyLan9354PrivFile.setOutputName("drv_extphy_lan9354_priv.h")
    drvExtPhyLan9354PrivFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyLan9354PrivFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyLan9354PrivFile.setType("HEADER")
    drvExtPhyLan9354PrivFile.setOverwrite(True)
    drvExtPhyLan9354PrivFile.setEnabled(True)


    # file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
    # Add drv_ethphy.c file
    drvExtPhySourceFile = drvExtPhyLan9354Component.createFileSymbol(None, None)
    drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
    drvExtPhySourceFile.setOutputName("drv_ethphy.c")
    drvExtPhySourceFile.setOverwrite(True)
    drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setType("SOURCE")
    drvExtPhySourceFile.setEnabled(True)

    
    # ifblock TCPIP_EMAC_PHY_TYPE = "LAN9354"
    # file TCPIP_MAC_PHY_LAN9354_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_lan9354.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_lan9354.c"
    # endif
    # Add drv_extphy_lan9354.c file
    drvExtPhyLan9354SourceFile = drvExtPhyLan9354Component.createFileSymbol(None, None)
    drvExtPhyLan9354SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_lan9354.c")
    drvExtPhyLan9354SourceFile.setOutputName("drv_extphy_lan9354.c")
    drvExtPhyLan9354SourceFile.setOverwrite(True)
    drvExtPhyLan9354SourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyLan9354SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyLan9354SourceFile.setType("SOURCE")
    drvExtPhyLan9354SourceFile.setEnabled(True)
    
    
def drvExtPhyLan9354MenuVisibleSingle(symbol, event):
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
