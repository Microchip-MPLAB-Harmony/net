"""
Copyright (C) 2026, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

phyName = "VSC8541"
phyComponentName = ""

def instantiateComponent(drvExtPhyVsc8541Component):
    global phyComponentId
    configName = Variables.get("__CONFIGURATION_NAME")    
    phyComponentId = drvExtPhyVsc8541Component.getID()
    print("VSC8541 PHY Driver Component ID : " + phyComponentId)

    # PHY Address
    drvExtPhyVsc8541Addr= drvExtPhyVsc8541Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
    drvExtPhyVsc8541Addr.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8541Addr.setLabel("PHY Address") 
    drvExtPhyVsc8541Addr.setVisible(True)
    drvExtPhyVsc8541Addr.setDescription("PHY Address")
    drvExtPhyVsc8541Addr.setDefaultValue(1)
    
    # Use a Function to be called at PHY Reset
    drvExtPhyVsc8541ResetCallbackEnable = drvExtPhyVsc8541Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
    drvExtPhyVsc8541ResetCallbackEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8541ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
    drvExtPhyVsc8541ResetCallbackEnable.setVisible(True)
    drvExtPhyVsc8541ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
    drvExtPhyVsc8541ResetCallbackEnable.setDefaultValue(False)
    
    # App Function
    drvExtPhyVsc8541ResetCallback = drvExtPhyVsc8541Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyVsc8541ResetCallbackEnable)
    drvExtPhyVsc8541ResetCallback.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8541ResetCallback.setLabel("App Function")
    drvExtPhyVsc8541ResetCallback.setVisible(False)
    drvExtPhyVsc8541ResetCallback.setDescription("App Function")
    drvExtPhyVsc8541ResetCallback.setDefaultValue("App" + phyName + "ResetFunction")
    drvExtPhyVsc8541ResetCallback.setDependencies(drvExtPhyVsc8541MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])  
    
    # External PHY Connection Flags
    drvExtPhyVsc8541ConnFlag = drvExtPhyVsc8541Component.createMenuSymbol(None, None) 
    drvExtPhyVsc8541ConnFlag.setLabel("External PHY Connection Flags")
    drvExtPhyVsc8541ConnFlag.setVisible(True)
    drvExtPhyVsc8541ConnFlag.setDescription("External PHY Connection Flags")
    
    # RGMII Data Interface
    drvExtPhyVsc8541ConfigRgmii = drvExtPhyVsc8541Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RGMII", drvExtPhyVsc8541ConnFlag)
    drvExtPhyVsc8541ConfigRgmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8541ConfigRgmii.setLabel("RGMII Data Interface")
    drvExtPhyVsc8541ConfigRgmii.setVisible(True)
    drvExtPhyVsc8541ConfigRgmii.setDefaultValue(True)
    drvExtPhyVsc8541ConfigRgmii.setDescription("RGMII Data Interface")
        
    # RMII Data Interface
    drvExtPhyVsc8541ConfigRmii = drvExtPhyVsc8541Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyVsc8541ConnFlag)
    drvExtPhyVsc8541ConfigRmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8541ConfigRmii.setLabel("RMII Data Interface")
    drvExtPhyVsc8541ConfigRmii.setVisible(True)
    drvExtPhyVsc8541ConfigRmii.setDefaultValue(False)
    drvExtPhyVsc8541ConfigRmii.setDescription("RMII Data Interface")
    
    # MII Data Interface
    drvExtPhyVsc8541ConfigMii = drvExtPhyVsc8541Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_MII", drvExtPhyVsc8541ConnFlag)
    drvExtPhyVsc8541ConfigMii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8541ConfigMii.setLabel("MII Data Interface")
    drvExtPhyVsc8541ConfigMii.setVisible(True)
    drvExtPhyVsc8541ConfigMii.setDefaultValue(False)
    drvExtPhyVsc8541ConfigMii.setDescription("MII Data Interface")
    
    # GMII Data Interface
    drvExtPhyVsc8541ConfigGmii = drvExtPhyVsc8541Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_GMII", drvExtPhyVsc8541ConnFlag)
    drvExtPhyVsc8541ConfigGmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8541ConfigGmii.setLabel("GMII Data Interface")
    drvExtPhyVsc8541ConfigGmii.setVisible(True)
    drvExtPhyVsc8541ConfigGmii.setDefaultValue(False)
    drvExtPhyVsc8541ConfigGmii.setDescription("GMII Data Interface")
    
    if ("PIC32M" in Variables.get("__PROCESSOR")) or ("WFI32" in Variables.get("__PROCESSOR")):
        # default is Auto for PIC32M
        drvExtPhyVsc8541ConfigRgmii.setDefaultValue(False)
    
        # Configuration Fuses Is ALT
        drvExtPhyVsc8541ConfigAlt = drvExtPhyVsc8541Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_ALTERNATE", drvExtPhyVsc8541ConnFlag)
        drvExtPhyVsc8541ConfigAlt.setHelp("mcc_h3_phy_configurations")
        drvExtPhyVsc8541ConfigAlt.setLabel("Configuration Fuses Is ALT")
        drvExtPhyVsc8541ConfigAlt.setVisible(True)
        drvExtPhyVsc8541ConfigAlt.setDescription("Configuration Fuses Is ALT")
        drvExtPhyVsc8541ConfigAlt.setDefaultValue(False)
        
        # Use The Fuses Configuration
        drvExtPhyVsc8541ConfigAuto = drvExtPhyVsc8541Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTO", drvExtPhyVsc8541ConnFlag)
        drvExtPhyVsc8541ConfigAuto.setHelp("mcc_h3_phy_configurations")
        drvExtPhyVsc8541ConfigAuto.setLabel("Use The Fuses Configuration")
        drvExtPhyVsc8541ConfigAuto.setVisible(True)
        drvExtPhyVsc8541ConfigAuto.setDescription("Use The Fuses Configuration")
        drvExtPhyVsc8541ConfigAuto.setDefaultValue(True)  

    # Advanced Settings
    drvExtPhyVsc8541AdvSettings = drvExtPhyVsc8541Component.createMenuSymbol("TCPIP_INTMAC_PHY_ADV_SETTING", None)
    drvExtPhyVsc8541AdvSettings.setLabel("Advanced Settings")
    drvExtPhyVsc8541AdvSettings.setDescription("Advanced Settings")
    drvExtPhyVsc8541AdvSettings.setVisible(True)

    # Delay for the Link Initialization in ms
    drvExtPhyVsc8541LinkInitDelay= drvExtPhyVsc8541Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", drvExtPhyVsc8541AdvSettings)
    drvExtPhyVsc8541LinkInitDelay.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8541LinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
    drvExtPhyVsc8541LinkInitDelay.setVisible(True)
    drvExtPhyVsc8541LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
    drvExtPhyVsc8541LinkInitDelay.setDefaultValue(500)
   
    # External PHY Type
    drvExtPhyVsc8541PhyType = drvExtPhyVsc8541Component.createStringSymbol("TCPIP_EMAC_PHY_TYPE", drvExtPhyVsc8541AdvSettings)
    drvExtPhyVsc8541PhyType.setVisible(False)   
    drvExtPhyVsc8541PhyType.setDefaultValue(phyName)
    
    # Driver PHY Negotiation Time-out - ms
    drvExtPhyVsc8541NegInitTimeout= drvExtPhyVsc8541Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", drvExtPhyVsc8541AdvSettings)
    drvExtPhyVsc8541NegInitTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8541NegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
    drvExtPhyVsc8541NegInitTimeout.setVisible(True)
    drvExtPhyVsc8541NegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
    drvExtPhyVsc8541NegInitTimeout.setDefaultValue(1)
    
    # Driver PHY Negotiation Done Time-out - ms
    drvExtPhyVsc8541NegDoneTimeout= drvExtPhyVsc8541Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", drvExtPhyVsc8541AdvSettings)
    drvExtPhyVsc8541NegDoneTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8541NegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
    drvExtPhyVsc8541NegDoneTimeout.setVisible(True)
    drvExtPhyVsc8541NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
    drvExtPhyVsc8541NegDoneTimeout.setDefaultValue(2000)
    
    # Driver PHY Reset Clear Time-out - ms
    drvExtPhyVsc8541ResetClearTimeout= drvExtPhyVsc8541Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", drvExtPhyVsc8541AdvSettings)
    drvExtPhyVsc8541ResetClearTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8541ResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
    drvExtPhyVsc8541ResetClearTimeout.setVisible(True)
    drvExtPhyVsc8541ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
    drvExtPhyVsc8541ResetClearTimeout.setDefaultValue(500)
    
    # Driver PHY Peripheral ID
    drvExtPhyVsc8541PeripheralId= drvExtPhyVsc8541Component.createStringSymbol("DRV_ETHPHY_PERIPHERAL_ID", drvExtPhyVsc8541AdvSettings)
    drvExtPhyVsc8541PeripheralId.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8541PeripheralId.setLabel("PHY Peripheral ID") 
    drvExtPhyVsc8541PeripheralId.setVisible(True)
    drvExtPhyVsc8541PeripheralId.setDescription("Driver PHY Peripheral ID")
    drvExtPhyVsc8541PeripheralId.setDefaultValue("")
    drvExtPhyVsc8541PeripheralId.setReadOnly(True)

    # External MAC Name
    drvExtPhyMacName= drvExtPhyVsc8541Component.createStringSymbol("DRV_ETHPHY_MAC_NAME", None)
    drvExtPhyMacName.setLabel("Mac Name") 
    drvExtPhyMacName.setVisible(False)   
    drvExtPhyMacName.setDefaultValue("")
    drvExtPhyMacName.setReadOnly(True) 
    
    
    #Add forward declaration to initialization.c
    drvExtPhyVsc8541InitDataSourceFtl = drvExtPhyVsc8541Component.createFileSymbol(None, None)
    drvExtPhyVsc8541InitDataSourceFtl.setType("STRING")
    drvExtPhyVsc8541InitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    drvExtPhyVsc8541InitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_driver_initialize.c.ftl")
    drvExtPhyVsc8541InitDataSourceFtl.setMarkup(True)    
    
    #Add to initialization.c
    drvExtPhyVsc8541SysInitDataSourceFtl = drvExtPhyVsc8541Component.createFileSymbol(None, None)
    drvExtPhyVsc8541SysInitDataSourceFtl.setType("STRING")
    drvExtPhyVsc8541SysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    drvExtPhyVsc8541SysInitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_data_initialize.c.ftl")
    drvExtPhyVsc8541SysInitDataSourceFtl.setMarkup(True)    
    
    #Add to system_config.h
    drvExtPhyVsc8541HeaderFtl = drvExtPhyVsc8541Component.createFileSymbol(None, None)
    drvExtPhyVsc8541HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_vsc8541.h.ftl")
    drvExtPhyVsc8541HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    drvExtPhyVsc8541HeaderFtl.setMarkup(True)
    drvExtPhyVsc8541HeaderFtl.setType("STRING")
        
    #Add to definitions.h
    drvExtPhyVsc8541SysdefFtl = drvExtPhyVsc8541Component.createFileSymbol(None, None)
    drvExtPhyVsc8541SysdefFtl.setSourcePath("driver/ethphy/templates/system/system_definitions.h.ftl")
    drvExtPhyVsc8541SysdefFtl.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    drvExtPhyVsc8541SysdefFtl.setMarkup(True)
    drvExtPhyVsc8541SysdefFtl.setType("STRING")

    # file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
    # Add drv_ethphy.h file to project
    drvExtPhyHeaderFile = drvExtPhyVsc8541Component.createFileSymbol(None, None)
    drvExtPhyHeaderFile.setSourcePath("driver/ethphy/drv_ethphy.h")
    drvExtPhyHeaderFile.setOutputName("drv_ethphy.h")
    drvExtPhyHeaderFile.setDestPath("driver/ethphy/")
    drvExtPhyHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/")
    drvExtPhyHeaderFile.setType("HEADER")
    drvExtPhyHeaderFile.setOverwrite(True)
    drvExtPhyHeaderFile.setEnabled(True)

    # file TCPIP_ETH_PHY_LOCAL_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/drv_ethphy_local.h" to           "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/drv_ethphy_local.h"
    # Add drv_ethphy_local.h file to project
    drvExtPhyLocalHeaderFile = drvExtPhyVsc8541Component.createFileSymbol(None, None)
    drvExtPhyLocalHeaderFile.setSourcePath("driver/ethphy/src/drv_ethphy_local.h")
    drvExtPhyLocalHeaderFile.setOutputName("drv_ethphy_local.h")
    drvExtPhyLocalHeaderFile.setDestPath("driver/ethphy/src/")
    drvExtPhyLocalHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/")
    drvExtPhyLocalHeaderFile.setType("HEADER")
    drvExtPhyLocalHeaderFile.setOverwrite(True)
    drvExtPhyLocalHeaderFile.setEnabled(True)


    # file TCPIP_ETH_EXT_PHY_REGS_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h" to    "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h"
    # Add drv_extphy_regs.h file to project
    drvExtPhyVsc8541RegHeaderFile = drvExtPhyVsc8541Component.createFileSymbol(None, None)
    drvExtPhyVsc8541RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
    drvExtPhyVsc8541RegHeaderFile.setOutputName("drv_extphy_regs.h")
    drvExtPhyVsc8541RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyVsc8541RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyVsc8541RegHeaderFile.setType("HEADER")
    drvExtPhyVsc8541RegHeaderFile.setOverwrite(True)
    drvExtPhyVsc8541RegHeaderFile.setEnabled(True)
    
    # Add drv_extphy_vsc8541.h file to project
    drvExtPhyVsc8541HeaderFile = drvExtPhyVsc8541Component.createFileSymbol("PHY_VSC8541_HEADER", None)
    drvExtPhyVsc8541HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_vsc8541.h")
    drvExtPhyVsc8541HeaderFile.setOutputName("drv_extphy_vsc8541.h")
    drvExtPhyVsc8541HeaderFile.setDestPath("driver/ethphy/")
    drvExtPhyVsc8541HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/")
    drvExtPhyVsc8541HeaderFile.setType("HEADER")
    drvExtPhyVsc8541HeaderFile.setOverwrite(True)
    drvExtPhyVsc8541HeaderFile.setEnabled(True)

    # Add drv_extphy_vsc8541_priv.h file to project
    drvExtPhyVsc8541PrivFile = drvExtPhyVsc8541Component.createFileSymbol(None, None)
    drvExtPhyVsc8541PrivFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_vsc8541_priv.h")
    drvExtPhyVsc8541PrivFile.setOutputName("drv_extphy_vsc8541_priv.h")
    drvExtPhyVsc8541PrivFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyVsc8541PrivFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyVsc8541PrivFile.setType("HEADER")
    drvExtPhyVsc8541PrivFile.setOverwrite(True)
    drvExtPhyVsc8541PrivFile.setEnabled(True)

    # file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
    # Add drv_ethphy.c file
    drvExtPhySourceFile = drvExtPhyVsc8541Component.createFileSymbol(None, None)
    drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
    drvExtPhySourceFile.setOutputName("drv_ethphy.c")
    drvExtPhySourceFile.setOverwrite(True)
    drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setType("SOURCE")
    drvExtPhySourceFile.setEnabled(True)

    # Add drv_extphy_vsc8541.c file
    drvExtPhyVsc8541SourceFile = drvExtPhyVsc8541Component.createFileSymbol("PHY_VSC8541_SOURCE", None)
    drvExtPhyVsc8541SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_vsc8541.c")
    drvExtPhyVsc8541SourceFile.setOutputName("drv_extphy_vsc8541.c")
    drvExtPhyVsc8541SourceFile.setOverwrite(True)
    drvExtPhyVsc8541SourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyVsc8541SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyVsc8541SourceFile.setType("SOURCE")
    drvExtPhyVsc8541SourceFile.setEnabled(True)
    
    
def drvExtPhyVsc8541MenuVisibleSingle(symbol, event):
    if (event["value"] == True):
        print("EthPhy Menu Visible.")       
        symbol.setVisible(True)
    else:
        print("EthPhy Menu Invisible.")
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
    
