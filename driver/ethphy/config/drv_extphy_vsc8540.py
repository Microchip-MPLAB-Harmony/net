"""
Copyright (C) 2022-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

phyName = "VSC8540"
def instantiateComponent(drvExtPhyVsc8540Component):
    print("VSC8540 PHY Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # PHY Address
    drvExtPhyVsc8540Addr= drvExtPhyVsc8540Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
    drvExtPhyVsc8540Addr.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8540Addr.setLabel("PHY Address") 
    drvExtPhyVsc8540Addr.setVisible(True)
    drvExtPhyVsc8540Addr.setDescription("PHY Address")
    drvExtPhyVsc8540Addr.setDefaultValue(0)
    
    # Use a Function to be called at PHY Reset
    drvExtPhyVsc8540ResetCallbackEnable = drvExtPhyVsc8540Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
    drvExtPhyVsc8540ResetCallbackEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8540ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
    drvExtPhyVsc8540ResetCallbackEnable.setVisible(True)
    drvExtPhyVsc8540ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
    drvExtPhyVsc8540ResetCallbackEnable.setDefaultValue(False)
    
    # App Function
    drvExtPhyVsc8540ResetCallback = drvExtPhyVsc8540Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyVsc8540ResetCallbackEnable)
    drvExtPhyVsc8540ResetCallback.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8540ResetCallback.setLabel("App Function")
    drvExtPhyVsc8540ResetCallback.setVisible(False)
    drvExtPhyVsc8540ResetCallback.setDescription("App Function")
    drvExtPhyVsc8540ResetCallback.setDefaultValue("App" + phyName + "ResetFunction")
    drvExtPhyVsc8540ResetCallback.setDependencies(drvExtPhyVsc8540MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])  
    
    # External PHY Connection Flags
    drvExtPhyVsc8540ConnFlag = drvExtPhyVsc8540Component.createMenuSymbol(None, None) 
    drvExtPhyVsc8540ConnFlag.setLabel("External PHY Connection Flags")
    drvExtPhyVsc8540ConnFlag.setVisible(True)
    drvExtPhyVsc8540ConnFlag.setDescription("External PHY Connection Flags")

    # RMII Data Interface
    drvExtPhyVsc8540ConfigRmii = drvExtPhyVsc8540Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyVsc8540ConnFlag)
    drvExtPhyVsc8540ConfigRmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8540ConfigRmii.setLabel("RMII Data Interface")
    drvExtPhyVsc8540ConfigRmii.setVisible(True)
    drvExtPhyVsc8540ConfigRmii.setDescription("RMII Data Interface")
    drvExtPhyVsc8540ConfigRmii.setDefaultValue(True)
    drvExtPhyVsc8540ConfigRmii.setDefaultValue(True)

    if ("PIC32M" in Variables.get("__PROCESSOR")) or ("WFI32" in Variables.get("__PROCESSOR")):
        # default is Auto for PIC32M
        drvExtPhyVsc8540ConfigRmii.setDefaultValue(False)
    
        # Configuration Fuses Is ALT
        drvExtPhyVsc8540ConfigAlt = drvExtPhyVsc8540Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_ALTERNATE", drvExtPhyVsc8540ConnFlag)
        drvExtPhyVsc8540ConfigAlt.setHelp("mcc_h3_phy_configurations")
        drvExtPhyVsc8540ConfigAlt.setLabel("Configuration Fuses Is ALT")
        drvExtPhyVsc8540ConfigAlt.setVisible(True)
        drvExtPhyVsc8540ConfigAlt.setDescription("Configuration Fuses Is ALT")
        drvExtPhyVsc8540ConfigAlt.setDefaultValue(False)
        
        # Use The Fuses Configuration
        drvExtPhyVsc8540ConfigAuto = drvExtPhyVsc8540Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTO", drvExtPhyVsc8540ConnFlag)
        drvExtPhyVsc8540ConfigAuto.setHelp("mcc_h3_phy_configurations")
        drvExtPhyVsc8540ConfigAuto.setLabel("Use The Fuses Configuration")
        drvExtPhyVsc8540ConfigAuto.setVisible(True)
        drvExtPhyVsc8540ConfigAuto.setDescription("Use The Fuses Configuration")
        drvExtPhyVsc8540ConfigAuto.setDefaultValue(True)  

    # Managed Mode interface
    drvExtPhyVsc8540ConfigManagedMode = drvExtPhyVsc8540Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_MANAGED_MODE", drvExtPhyVsc8540ConnFlag)
    drvExtPhyVsc8540ConfigManagedMode.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8540ConfigManagedMode.setLabel("Choose Managed Mode")
    drvExtPhyVsc8540ConfigManagedMode.setVisible(True)
    drvExtPhyVsc8540ConfigManagedMode.setDescription("Choose the Managed mode")
    drvExtPhyVsc8540ConfigManagedMode.setDefaultValue(False)

    #ComboValue for TX and RX CLK delay settings
    RX_TX_Combo_Values = [
        "0.2ns",
        "0.8ns",
        "1.1ns",
        "1.7ns",
        "2.0ns",
        "2.3ns",
        "2.6ns",
        "3.4ns"
    ]

    #RX_CLK Delay Settings
    drvExtPhyVsc8540ConfigRxClkDelay = drvExtPhyVsc8540Component.createComboSymbol("TCPIP_INTMAC_PHY_CONFIG_RX_CLK_DELAY", drvExtPhyVsc8540ConfigManagedMode, RX_TX_Combo_Values)
    drvExtPhyVsc8540ConfigRxClkDelay.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8540ConfigRxClkDelay.setLabel("RX CLK Delay Settings")
    drvExtPhyVsc8540ConfigRxClkDelay.setVisible(False)
    drvExtPhyVsc8540ConfigRxClkDelay.setDescription("RX CLK Delay Settings")
    drvExtPhyVsc8540ConfigRxClkDelay.setDependencies(drvExtPhyVsc8540MenuVisibleSingle, ["TCPIP_INTMAC_PHY_CONFIG_MANAGED_MODE"])

    #TX_CLK Delay Settings
    drvExtPhyVsc8540ConfigRxClkDelay = drvExtPhyVsc8540Component.createComboSymbol("TCPIP_INTMAC_PHY_CONFIG_TX_CLK_DELAY", drvExtPhyVsc8540ConfigManagedMode, RX_TX_Combo_Values)
    drvExtPhyVsc8540ConfigRxClkDelay.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8540ConfigRxClkDelay.setLabel("TX CLK Delay Settings")
    drvExtPhyVsc8540ConfigRxClkDelay.setVisible(False)
    drvExtPhyVsc8540ConfigRxClkDelay.setDescription("TX CLK Delay Settings")
    drvExtPhyVsc8540ConfigRxClkDelay.setDependencies(drvExtPhyVsc8540MenuVisibleSingle, ["TCPIP_INTMAC_PHY_CONFIG_MANAGED_MODE"])

    #CLKOUT enable
    drvExtPhyVsc8540ConfigCLKOUTEn = drvExtPhyVsc8540Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_CLKOUT_EN", drvExtPhyVsc8540ConfigManagedMode)
    drvExtPhyVsc8540ConfigCLKOUTEn.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8540ConfigCLKOUTEn.setLabel("CLKOUT Enable")
    drvExtPhyVsc8540ConfigCLKOUTEn.setVisible(False)
    drvExtPhyVsc8540ConfigCLKOUTEn.setDescription("CLKOUT Enable")
    drvExtPhyVsc8540ConfigCLKOUTEn.setDependencies(drvExtPhyVsc8540MenuVisibleSingle, ["TCPIP_INTMAC_PHY_CONFIG_MANAGED_MODE"])

    #ComboValue for CLKOUT frequency
    CLKOUT_FREQ_Combo_Values = [
        "25MHz",
        "50MHz",
        "125MHz"
    ]

    #CLKOUT frequency
    drvExtPhyVsc8540ConfigCLKOUTFreq = drvExtPhyVsc8540Component.createComboSymbol("TCPIP_INTMAC_PHY_CONFIG_CLKOUT_FREQ", drvExtPhyVsc8540ConfigCLKOUTEn, CLKOUT_FREQ_Combo_Values)
    drvExtPhyVsc8540ConfigCLKOUTFreq.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8540ConfigCLKOUTFreq.setLabel("CLKOUT Frequency")
    drvExtPhyVsc8540ConfigCLKOUTFreq.setVisible(False)
    drvExtPhyVsc8540ConfigCLKOUTFreq.setDescription("CLKOUT Frequency")
    drvExtPhyVsc8540ConfigCLKOUTFreq.setDependencies(drvExtPhyVsc8540MenuVisibleSingle, ["TCPIP_INTMAC_PHY_CONFIG_CLKOUT_EN"])

    #Autonegotiation Enbale
    drvExtPhyVsc8540ConfigAutoNegEn = drvExtPhyVsc8540Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTONEG_EN", drvExtPhyVsc8540ConfigManagedMode)
    drvExtPhyVsc8540ConfigAutoNegEn.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8540ConfigAutoNegEn.setLabel("AutoNegotiation Enable")
    drvExtPhyVsc8540ConfigAutoNegEn.setVisible(False)
    drvExtPhyVsc8540ConfigAutoNegEn.setDescription("AutoNegotiation Enable")
    drvExtPhyVsc8540ConfigAutoNegEn.setDependencies(drvExtPhyVsc8540MenuVisibleSingle, ["TCPIP_INTMAC_PHY_CONFIG_MANAGED_MODE"])
    drvExtPhyVsc8540ConfigAutoNegEn.setDefaultValue(True) 

    #Force Speed Selection
    drvExtPhyVsc8540ConfigForceSpdSel = drvExtPhyVsc8540Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_FORCE_SPD_SEL", drvExtPhyVsc8540ConfigManagedMode)
    drvExtPhyVsc8540ConfigForceSpdSel.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8540ConfigForceSpdSel.setLabel("Speed 100Mbps (10Mbps by default)")
    drvExtPhyVsc8540ConfigForceSpdSel.setVisible(False)
    drvExtPhyVsc8540ConfigForceSpdSel.setDescription("Force Speed to 100Mbps (10Mbit by default)")
    drvExtPhyVsc8540ConfigForceSpdSel.setDependencies(drvExtPhyVsc8540MenuVisibleSingle, ["TCPIP_INTMAC_PHY_CONFIG_MANAGED_MODE"])
    drvExtPhyVsc8540ConfigForceSpdSel.setDefaultValue(False)

    #Duplex Mode
    drvExtPhyVsc8540ConfigDuplexSel = drvExtPhyVsc8540Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_DUPLEX_SEL", drvExtPhyVsc8540ConfigManagedMode)
    drvExtPhyVsc8540ConfigDuplexSel.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8540ConfigDuplexSel.setLabel("Mode Full Duplex (Half by default)")
    drvExtPhyVsc8540ConfigDuplexSel.setVisible(False)
    drvExtPhyVsc8540ConfigDuplexSel.setDescription("Mode Full Duplex (Half by default)")
    drvExtPhyVsc8540ConfigDuplexSel.setDependencies(drvExtPhyVsc8540MenuVisibleSingle, ["TCPIP_INTMAC_PHY_CONFIG_MANAGED_MODE"])
    drvExtPhyVsc8540ConfigDuplexSel.setDefaultValue(False)

    # Advanced Settings
    drvExtPhyVsc8540AdvSettings = drvExtPhyVsc8540Component.createMenuSymbol("TCPIP_INTMAC_PHY_ADV_SETTING", None)
    drvExtPhyVsc8540AdvSettings.setLabel("Advanced Settings")
    drvExtPhyVsc8540AdvSettings.setDescription("Advanced Settings")
    drvExtPhyVsc8540AdvSettings.setVisible(True)

    # Delay for the Link Initialization in ms
    drvExtPhyVsc8540LinkInitDelay= drvExtPhyVsc8540Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", drvExtPhyVsc8540AdvSettings)
    drvExtPhyVsc8540LinkInitDelay.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8540LinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
    drvExtPhyVsc8540LinkInitDelay.setVisible(True)
    drvExtPhyVsc8540LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
    drvExtPhyVsc8540LinkInitDelay.setDefaultValue(500)
   
    # External PHY Type
    drvExtPhyVsc8540PhyType = drvExtPhyVsc8540Component.createStringSymbol("TCPIP_EMAC_PHY_TYPE", drvExtPhyVsc8540AdvSettings)
    drvExtPhyVsc8540PhyType.setVisible(False)   
    drvExtPhyVsc8540PhyType.setDefaultValue(phyName)
    
    # Driver PHY Negotiation Time-out - ms
    drvExtPhyVsc8540NegInitTimeout= drvExtPhyVsc8540Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", drvExtPhyVsc8540AdvSettings)
    drvExtPhyVsc8540NegInitTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8540NegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
    drvExtPhyVsc8540NegInitTimeout.setVisible(True)
    drvExtPhyVsc8540NegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
    drvExtPhyVsc8540NegInitTimeout.setDefaultValue(1)
    
    # Driver PHY Negotiation Done Time-out - ms
    drvExtPhyVsc8540NegDoneTimeout= drvExtPhyVsc8540Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", drvExtPhyVsc8540AdvSettings)
    drvExtPhyVsc8540NegDoneTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8540NegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
    drvExtPhyVsc8540NegDoneTimeout.setVisible(True)
    drvExtPhyVsc8540NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
    drvExtPhyVsc8540NegDoneTimeout.setDefaultValue(2000)
    
    # Driver PHY Reset Clear Time-out - ms
    drvExtPhyVsc8540ResetClearTimeout= drvExtPhyVsc8540Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", drvExtPhyVsc8540AdvSettings)
    drvExtPhyVsc8540ResetClearTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8540ResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
    drvExtPhyVsc8540ResetClearTimeout.setVisible(True)
    drvExtPhyVsc8540ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
    drvExtPhyVsc8540ResetClearTimeout.setDefaultValue(500)
 
    # Driver PHY Peripheral ID
    drvExtPhyVsc8540PeripheralId= drvExtPhyVsc8540Component.createStringSymbol("DRV_ETHPHY_PERIPHERAL_ID", drvExtPhyVsc8540AdvSettings)
    drvExtPhyVsc8540PeripheralId.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8540PeripheralId.setLabel("PHY Peripheral ID") 
    drvExtPhyVsc8540PeripheralId.setVisible(True)
    drvExtPhyVsc8540PeripheralId.setDescription("Driver PHY Peripheral ID")
    drvExtPhyVsc8540PeripheralId.setDefaultValue("")
    drvExtPhyVsc8540PeripheralId.setReadOnly(True)

    # External MAC Name
    drvExtPhyMacName= drvExtPhyVsc8540Component.createStringSymbol("DRV_ETHPHY_MAC_NAME", None)
    drvExtPhyMacName.setLabel("Mac Name") 
    drvExtPhyMacName.setVisible(False)   
    drvExtPhyMacName.setDefaultValue("")
    drvExtPhyMacName.setReadOnly(True) 
                            
    #Add forward declaration to initialization.c
    drvExtPhyVsc8540InitDataSourceFtl = drvExtPhyVsc8540Component.createFileSymbol(None, None)
    drvExtPhyVsc8540InitDataSourceFtl.setType("STRING")
    drvExtPhyVsc8540InitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    drvExtPhyVsc8540InitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_driver_initialize.c.ftl")
    drvExtPhyVsc8540InitDataSourceFtl.setMarkup(True)    
    
    #Add to initialization.c
    drvExtPhyVsc8540SysInitDataSourceFtl = drvExtPhyVsc8540Component.createFileSymbol(None, None)
    drvExtPhyVsc8540SysInitDataSourceFtl.setType("STRING")
    drvExtPhyVsc8540SysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    drvExtPhyVsc8540SysInitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_data_initialize.c.ftl")
    drvExtPhyVsc8540SysInitDataSourceFtl.setMarkup(True)    
    
    #Add to system_config.h
    drvExtPhyVsc8540HeaderFtl = drvExtPhyVsc8540Component.createFileSymbol(None, None)
    drvExtPhyVsc8540HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_vsc8540.h.ftl")
    drvExtPhyVsc8540HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    drvExtPhyVsc8540HeaderFtl.setMarkup(True)
    drvExtPhyVsc8540HeaderFtl.setType("STRING")
        
    #Add to definitions.h
    drvExtPhyVsc8540SysdefFtl = drvExtPhyVsc8540Component.createFileSymbol(None, None)
    drvExtPhyVsc8540SysdefFtl.setSourcePath("driver/ethphy/templates/system/system_definitions.h.ftl")
    drvExtPhyVsc8540SysdefFtl.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    drvExtPhyVsc8540SysdefFtl.setMarkup(True)
    drvExtPhyVsc8540SysdefFtl.setType("STRING")

    # file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
    # Add drv_ethphy.h file to project
    drvExtPhyHeaderFile = drvExtPhyVsc8540Component.createFileSymbol(None, None)
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
    drvExtPhyLocalHeaderFile = drvExtPhyVsc8540Component.createFileSymbol(None, None)
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
    drvExtPhyVsc8540RegHeaderFile = drvExtPhyVsc8540Component.createFileSymbol(None, None)
    drvExtPhyVsc8540RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
    drvExtPhyVsc8540RegHeaderFile.setOutputName("drv_extphy_regs.h")
    drvExtPhyVsc8540RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyVsc8540RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyVsc8540RegHeaderFile.setType("HEADER")
    drvExtPhyVsc8540RegHeaderFile.setOverwrite(True)
    drvExtPhyVsc8540RegHeaderFile.setEnabled(True)
    
    # Add drv_extphy_Vsc8540.h file to project
    drvExtPhyVsc8540HeaderFile = drvExtPhyVsc8540Component.createFileSymbol(None, None)
    drvExtPhyVsc8540HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_vsc8540.h")
    drvExtPhyVsc8540HeaderFile.setOutputName("drv_extphy_vsc8540.h")
    drvExtPhyVsc8540HeaderFile.setDestPath("driver/ethphy/")
    drvExtPhyVsc8540HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/")
    drvExtPhyVsc8540HeaderFile.setType("HEADER")
    drvExtPhyVsc8540HeaderFile.setOverwrite(True)
    drvExtPhyVsc8540HeaderFile.setEnabled(True)

    # Add drv_extphy_Vsc8540_priv.h file to project
    drvExtPhyVsc8540PrivFile = drvExtPhyVsc8540Component.createFileSymbol(None, None)
    drvExtPhyVsc8540PrivFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_vsc8540_priv.h")
    drvExtPhyVsc8540PrivFile.setOutputName("drv_extphy_vsc8540_priv.h")
    drvExtPhyVsc8540PrivFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyVsc8540PrivFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyVsc8540PrivFile.setType("HEADER")
    drvExtPhyVsc8540PrivFile.setOverwrite(True)
    drvExtPhyVsc8540PrivFile.setEnabled(True)



    # file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
    # Add drv_ethphy.c file
    drvExtPhySourceFile = drvExtPhyVsc8540Component.createFileSymbol(None, None)
    drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
    drvExtPhySourceFile.setOutputName("drv_ethphy.c")
    drvExtPhySourceFile.setOverwrite(True)
    drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setType("SOURCE")
    drvExtPhySourceFile.setEnabled(True)

    # Add drv_extphy_Vsc8540.c file
    drvExtPhyVsc8540SourceFile = drvExtPhyVsc8540Component.createFileSymbol(None, None)
    drvExtPhyVsc8540SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_vsc8540.c")
    drvExtPhyVsc8540SourceFile.setOutputName("drv_extphy_vsc8540.c")
    drvExtPhyVsc8540SourceFile.setOverwrite(True)
    drvExtPhyVsc8540SourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyVsc8540SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyVsc8540SourceFile.setType("SOURCE")
    drvExtPhyVsc8540SourceFile.setEnabled(True)

    
def drvExtPhyVsc8540MenuVisibleSingle(symbol, event):
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
           
