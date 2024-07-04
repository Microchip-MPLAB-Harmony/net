"""
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

phyName = "LAN8840"
def instantiateComponent(drvExtPhyLan8840Component):
    print("LAN8840 PHY Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")    
    
    # PHY Address
    drvExtPhyLan8840Addr= drvExtPhyLan8840Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
    drvExtPhyLan8840Addr.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840Addr.setLabel("PHY Address") 
    drvExtPhyLan8840Addr.setVisible(True)
    drvExtPhyLan8840Addr.setDescription("PHY Address")
    drvExtPhyLan8840Addr.setDefaultValue(1)
    
    # Use a Function to be called at PHY Reset
    drvExtPhyLan8840ResetCallbackEnable = drvExtPhyLan8840Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
    drvExtPhyLan8840ResetCallbackEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
    drvExtPhyLan8840ResetCallbackEnable.setVisible(True)
    drvExtPhyLan8840ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
    drvExtPhyLan8840ResetCallbackEnable.setDefaultValue(False)
    
    # App Function
    drvExtPhyLan8840ResetCallback = drvExtPhyLan8840Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyLan8840ResetCallbackEnable)
    drvExtPhyLan8840ResetCallback.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840ResetCallback.setLabel("App Function")
    drvExtPhyLan8840ResetCallback.setVisible(False)
    drvExtPhyLan8840ResetCallback.setDescription("App Function")
    drvExtPhyLan8840ResetCallback.setDefaultValue("App" + phyName + "ResetFunction")
    drvExtPhyLan8840ResetCallback.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])  
    
    # External PHY Connection Flags
    drvExtPhyLan8840ConnFlag = drvExtPhyLan8840Component.createMenuSymbol(None, None) 
    drvExtPhyLan8840ConnFlag.setLabel("External PHY Connection Flags")
    drvExtPhyLan8840ConnFlag.setVisible(True)
    drvExtPhyLan8840ConnFlag.setDescription("External PHY Connection Flags")
    
    # RGMII Data Interface
    drvExtPhyLan8840ConfigRgmii = drvExtPhyLan8840Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RGMII", drvExtPhyLan8840ConnFlag)
    drvExtPhyLan8840ConfigRgmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840ConfigRgmii.setLabel("RGMII Data Interface")
    drvExtPhyLan8840ConfigRgmii.setVisible(True)
    drvExtPhyLan8840ConfigRgmii.setDefaultValue(True)
    drvExtPhyLan8840ConfigRgmii.setDescription("RGMII Data Interface")
        
    # Advanced Settings
    drvExtPhyLan8840AdvSettings = drvExtPhyLan8840Component.createMenuSymbol("TCPIP_INTMAC_PHY_ADV_SETTING", None)
    drvExtPhyLan8840AdvSettings.setLabel("Advanced Settings")
    drvExtPhyLan8840AdvSettings.setDescription("Advanced Settings")
    drvExtPhyLan8840AdvSettings.setVisible(True)

    # Delay for the Link Initialization in ms
    drvExtPhyLan8840LinkInitDelay= drvExtPhyLan8840Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", drvExtPhyLan8840AdvSettings)
    drvExtPhyLan8840LinkInitDelay.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840LinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
    drvExtPhyLan8840LinkInitDelay.setVisible(True)
    drvExtPhyLan8840LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
    drvExtPhyLan8840LinkInitDelay.setDefaultValue(500)
   
    # External PHY Type
    drvExtPhyLan8840PhyType = drvExtPhyLan8840Component.createStringSymbol("TCPIP_EMAC_PHY_TYPE", drvExtPhyLan8840AdvSettings)
    drvExtPhyLan8840PhyType.setVisible(False)   
    drvExtPhyLan8840PhyType.setDefaultValue(phyName)
    
    # Driver PHY Negotiation Time-out - ms
    drvExtPhyLan8840NegInitTimeout= drvExtPhyLan8840Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", drvExtPhyLan8840AdvSettings)
    drvExtPhyLan8840NegInitTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840NegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
    drvExtPhyLan8840NegInitTimeout.setVisible(True)
    drvExtPhyLan8840NegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
    drvExtPhyLan8840NegInitTimeout.setDefaultValue(1)
    
    # Driver PHY Negotiation Done Time-out - ms
    drvExtPhyLan8840NegDoneTimeout= drvExtPhyLan8840Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", drvExtPhyLan8840AdvSettings)
    drvExtPhyLan8840NegDoneTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840NegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
    drvExtPhyLan8840NegDoneTimeout.setVisible(True)
    drvExtPhyLan8840NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
    drvExtPhyLan8840NegDoneTimeout.setDefaultValue(2000)
    
    # Driver PHY Reset Clear Time-out - ms
    drvExtPhyLan8840ResetClearTimeout= drvExtPhyLan8840Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", drvExtPhyLan8840AdvSettings)
    drvExtPhyLan8840ResetClearTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840ResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
    drvExtPhyLan8840ResetClearTimeout.setVisible(True)
    drvExtPhyLan8840ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
    drvExtPhyLan8840ResetClearTimeout.setDefaultValue(500)
    
    # Driver PHY Peripheral ID
    drvExtPhyLan8840PeripheralId= drvExtPhyLan8840Component.createStringSymbol("DRV_ETHPHY_PERIPHERAL_ID", drvExtPhyLan8840AdvSettings)
    drvExtPhyLan8840PeripheralId.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840PeripheralId.setLabel("PHY Peripheral ID") 
    drvExtPhyLan8840PeripheralId.setVisible(True)
    drvExtPhyLan8840PeripheralId.setDescription("Driver PHY Peripheral ID")
    drvExtPhyLan8840PeripheralId.setDefaultValue("")
    drvExtPhyLan8840PeripheralId.setReadOnly(True)

    # External MAC Name
    drvExtPhyMacName= drvExtPhyLan8840Component.createStringSymbol("DRV_ETHPHY_MAC_NAME", None)
    drvExtPhyMacName.setLabel("Mac Name") 
    drvExtPhyMacName.setVisible(False)   
    drvExtPhyMacName.setDefaultValue("")
    drvExtPhyMacName.setReadOnly(True) 
    
    # LAN8840 PHY Skew Settings
    drvExtPhyLan8840SkewSet = drvExtPhyLan8840Component.createMenuSymbol(None, drvExtPhyLan8840AdvSettings) 
    drvExtPhyLan8840SkewSet.setLabel("Skew Settings")
    drvExtPhyLan8840SkewSet.setVisible(True)
    drvExtPhyLan8840SkewSet.setDescription("Driver PHY Skew Settings")
    
    # TX_CLK_SKEW 
    drvExtPhyLan8840TxClkSkewEnable= drvExtPhyLan8840Component.createBooleanSymbol("DRV_ETHPHY_TX_CLK_SKEW_SET", drvExtPhyLan8840SkewSet)
    drvExtPhyLan8840TxClkSkewEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840TxClkSkewEnable.setLabel("TX Clock Skew") 
    drvExtPhyLan8840TxClkSkewEnable.setVisible(True)
    drvExtPhyLan8840TxClkSkewEnable.setDescription("Driver PHY TX Clock Skew Set")
    drvExtPhyLan8840TxClkSkewEnable.setDefaultValue(False)
    #Value 
    drvExtPhyLan8840TxClkSkew = drvExtPhyLan8840Component.createHexSymbol("DRV_ETHPHY_TX_CLK_SKEW", drvExtPhyLan8840TxClkSkewEnable)
    drvExtPhyLan8840TxClkSkew.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840TxClkSkew.setLabel("Skew Value")
    drvExtPhyLan8840TxClkSkew.setVisible(False)
    drvExtPhyLan8840TxClkSkew.setDescription("Driver PHY TX Clock Skew")
    drvExtPhyLan8840TxClkSkew.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_TX_CLK_SKEW_SET"])  
    # Comment 
    drvExtPhyLan8840TxClkSkewCmt = drvExtPhyLan8840Component.createCommentSymbol(None, drvExtPhyLan8840TxClkSkewEnable)
    drvExtPhyLan8840TxClkSkewCmt.setLabel("*** Range 0x00 - 0x1F")
    drvExtPhyLan8840TxClkSkewCmt.setVisible(False)  
    drvExtPhyLan8840TxClkSkewCmt.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_TX_CLK_SKEW_SET"]) 

    # RX_CLK_SKEW 
    drvExtPhyLan8840RxClkSkewEnable= drvExtPhyLan8840Component.createBooleanSymbol("DRV_ETHPHY_RX_CLK_SKEW_SET", drvExtPhyLan8840SkewSet)
    drvExtPhyLan8840RxClkSkewEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840RxClkSkewEnable.setLabel("RX Clock Skew") 
    drvExtPhyLan8840RxClkSkewEnable.setVisible(True)
    drvExtPhyLan8840RxClkSkewEnable.setDescription("Driver PHY RX Clock Skew Set")
    drvExtPhyLan8840RxClkSkewEnable.setDefaultValue(False)
    #Value 
    drvExtPhyLan8840RxClkSkew = drvExtPhyLan8840Component.createHexSymbol("DRV_ETHPHY_RX_CLK_SKEW", drvExtPhyLan8840RxClkSkewEnable)
    drvExtPhyLan8840RxClkSkew.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840RxClkSkew.setLabel("Skew Value")
    drvExtPhyLan8840RxClkSkew.setVisible(False)
    drvExtPhyLan8840RxClkSkew.setDescription("Driver PHY RX Clock Skew")
    drvExtPhyLan8840RxClkSkew.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_RX_CLK_SKEW_SET"])  
    # Comment 
    drvExtPhyLan8840RxClkSkewCmt = drvExtPhyLan8840Component.createCommentSymbol(None, drvExtPhyLan8840RxClkSkewEnable)
    drvExtPhyLan8840RxClkSkewCmt.setLabel("*** Range 0x00 - 0x1F")
    drvExtPhyLan8840RxClkSkewCmt.setVisible(False)  
    drvExtPhyLan8840RxClkSkewCmt.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_RX_CLK_SKEW_SET"]) 

    # RX_CTL_SKEW 
    drvExtPhyLan8840RxCtlSkewEnable= drvExtPhyLan8840Component.createBooleanSymbol("DRV_ETHPHY_RX_CTL_SKEW_SET", drvExtPhyLan8840SkewSet)
    drvExtPhyLan8840RxCtlSkewEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840RxCtlSkewEnable.setLabel("RX Control Signal Skew") 
    drvExtPhyLan8840RxCtlSkewEnable.setVisible(True)
    drvExtPhyLan8840RxCtlSkewEnable.setDescription("Driver PHY RX Control Signal Skew Set")
    drvExtPhyLan8840RxCtlSkewEnable.setDefaultValue(False)
    #Value 
    drvExtPhyLan8840RxCtlSkew = drvExtPhyLan8840Component.createHexSymbol("DRV_ETHPHY_RX_CTL_SKEW", drvExtPhyLan8840RxCtlSkewEnable)
    drvExtPhyLan8840RxCtlSkew.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840RxCtlSkew.setLabel("Skew Value")
    drvExtPhyLan8840RxCtlSkew.setVisible(False)
    drvExtPhyLan8840RxCtlSkew.setDescription("Driver PHY RX Control Signal Skew")
    drvExtPhyLan8840RxCtlSkew.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_RX_CTL_SKEW_SET"])  
    # Comment 
    drvExtPhyLan8840RxCtlSkewCmt = drvExtPhyLan8840Component.createCommentSymbol(None, drvExtPhyLan8840RxCtlSkewEnable)
    drvExtPhyLan8840RxCtlSkewCmt.setLabel("*** Range 0x00 - 0x0F")
    drvExtPhyLan8840RxCtlSkewCmt.setVisible(False)  
    drvExtPhyLan8840RxCtlSkewCmt.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_RX_CTL_SKEW_SET"]) 

    # TX_CTL_SKEW 
    drvExtPhyLan8840TxCtlSkewEnable= drvExtPhyLan8840Component.createBooleanSymbol("DRV_ETHPHY_TX_CTL_SKEW_SET", drvExtPhyLan8840SkewSet)
    drvExtPhyLan8840TxCtlSkewEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840TxCtlSkewEnable.setLabel("TX Control Signal Skew") 
    drvExtPhyLan8840TxCtlSkewEnable.setVisible(True)
    drvExtPhyLan8840TxCtlSkewEnable.setDescription("Driver PHY TX Control Signal Skew Set")
    drvExtPhyLan8840TxCtlSkewEnable.setDefaultValue(False)
    #Value 
    drvExtPhyLan8840TxCtlSkew = drvExtPhyLan8840Component.createHexSymbol("DRV_ETHPHY_TX_CTL_SKEW", drvExtPhyLan8840TxCtlSkewEnable)
    drvExtPhyLan8840TxCtlSkew.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840TxCtlSkew.setLabel("Skew Value")
    drvExtPhyLan8840TxCtlSkew.setVisible(False)
    drvExtPhyLan8840TxCtlSkew.setDescription("Driver PHY TX Control Signal Skew")
    drvExtPhyLan8840TxCtlSkew.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_TX_CTL_SKEW_SET"])  
    # Comment 
    drvExtPhyLan8840TxCtlSkewCmt = drvExtPhyLan8840Component.createCommentSymbol(None, drvExtPhyLan8840TxCtlSkewEnable)
    drvExtPhyLan8840TxCtlSkewCmt.setLabel("*** Range 0x00 - 0x0F")
    drvExtPhyLan8840TxCtlSkewCmt.setVisible(False)  
    drvExtPhyLan8840TxCtlSkewCmt.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_TX_CTL_SKEW_SET"]) 


    # RX Data Skew
    drvExtPhyLan8840RxDataSkew = drvExtPhyLan8840Component.createMenuSymbol(None, drvExtPhyLan8840SkewSet) 
    drvExtPhyLan8840RxDataSkew.setLabel("RX Data Skew")
    drvExtPhyLan8840RxDataSkew.setVisible(True)
    drvExtPhyLan8840RxDataSkew.setDescription("RGMII RX DATA SKEW")

    # RXD3_SKEW 
    drvExtPhyLan8840RxD3SkewEnable= drvExtPhyLan8840Component.createBooleanSymbol("DRV_ETHPHY_RXD3_SKEW_SET", drvExtPhyLan8840RxDataSkew)
    drvExtPhyLan8840RxD3SkewEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840RxD3SkewEnable.setLabel("RXD3 Skew") 
    drvExtPhyLan8840RxD3SkewEnable.setVisible(True)
    drvExtPhyLan8840RxD3SkewEnable.setDescription("Driver PHY RXD3 Skew Set")
    drvExtPhyLan8840RxD3SkewEnable.setDefaultValue(False)
    #Value 
    drvExtPhyLan8840RxD3Skew = drvExtPhyLan8840Component.createHexSymbol("DRV_ETHPHY_RXD3_SKEW", drvExtPhyLan8840RxD3SkewEnable)
    drvExtPhyLan8840RxD3Skew.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840RxD3Skew.setLabel("Skew Value")
    drvExtPhyLan8840RxD3Skew.setVisible(False)
    drvExtPhyLan8840RxD3Skew.setDescription("Driver PHY RXD3 Skew")
    drvExtPhyLan8840RxD3Skew.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_RXD3_SKEW_SET"])  
    # Comment 
    drvExtPhyLan8840RxD3SkewCmt = drvExtPhyLan8840Component.createCommentSymbol(None, drvExtPhyLan8840RxD3SkewEnable)
    drvExtPhyLan8840RxD3SkewCmt.setLabel("*** Range 0x00 - 0x0F")
    drvExtPhyLan8840RxD3SkewCmt.setVisible(False)  
    drvExtPhyLan8840RxD3SkewCmt.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_RXD3_SKEW_SET"]) 

    # RXD2_SKEW 
    drvExtPhyLan8840RxD2SkewEnable= drvExtPhyLan8840Component.createBooleanSymbol("DRV_ETHPHY_RXD2_SKEW_SET", drvExtPhyLan8840RxDataSkew)
    drvExtPhyLan8840RxD2SkewEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840RxD2SkewEnable.setLabel("RXD2 Skew") 
    drvExtPhyLan8840RxD2SkewEnable.setVisible(True)
    drvExtPhyLan8840RxD2SkewEnable.setDescription("Driver PHY RXD2 Skew Set")
    drvExtPhyLan8840RxD2SkewEnable.setDefaultValue(False)
    #Value 
    drvExtPhyLan8840RxD2Skew = drvExtPhyLan8840Component.createHexSymbol("DRV_ETHPHY_RXD2_SKEW", drvExtPhyLan8840RxD2SkewEnable)
    drvExtPhyLan8840RxD2Skew.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840RxD2Skew.setLabel("Skew Value")
    drvExtPhyLan8840RxD2Skew.setVisible(False)
    drvExtPhyLan8840RxD2Skew.setDescription("Driver PHY RXD2 Skew")
    drvExtPhyLan8840RxD2Skew.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_RXD2_SKEW_SET"])  
    # Comment 
    drvExtPhyLan8840RxD2SkewCmt = drvExtPhyLan8840Component.createCommentSymbol(None, drvExtPhyLan8840RxD2SkewEnable)
    drvExtPhyLan8840RxD2SkewCmt.setLabel("*** Range 0x00 - 0x0F")
    drvExtPhyLan8840RxD2SkewCmt.setVisible(False)  
    drvExtPhyLan8840RxD2SkewCmt.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_RXD2_SKEW_SET"]) 

    # RXD1_SKEW 
    drvExtPhyLan8840RxD1SkewEnable= drvExtPhyLan8840Component.createBooleanSymbol("DRV_ETHPHY_RXD1_SKEW_SET", drvExtPhyLan8840RxDataSkew)
    drvExtPhyLan8840RxD1SkewEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840RxD1SkewEnable.setLabel("RXD1 Skew") 
    drvExtPhyLan8840RxD1SkewEnable.setVisible(True)
    drvExtPhyLan8840RxD1SkewEnable.setDescription("Driver PHY RXD1 Skew Set")
    drvExtPhyLan8840RxD1SkewEnable.setDefaultValue(False)
    #Value 
    drvExtPhyLan8840RxD1Skew = drvExtPhyLan8840Component.createHexSymbol("DRV_ETHPHY_RXD1_SKEW", drvExtPhyLan8840RxD1SkewEnable)
    drvExtPhyLan8840RxD1Skew.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840RxD1Skew.setLabel("Skew Value")
    drvExtPhyLan8840RxD1Skew.setVisible(False)
    drvExtPhyLan8840RxD1Skew.setDescription("Driver PHY RXD1 Skew")
    drvExtPhyLan8840RxD1Skew.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_RXD1_SKEW_SET"])  
    # Comment 
    drvExtPhyLan8840RxD1SkewCmt = drvExtPhyLan8840Component.createCommentSymbol(None, drvExtPhyLan8840RxD1SkewEnable)
    drvExtPhyLan8840RxD1SkewCmt.setLabel("*** Range 0x00 - 0x0F")
    drvExtPhyLan8840RxD1SkewCmt.setVisible(False)  
    drvExtPhyLan8840RxD1SkewCmt.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_RXD1_SKEW_SET"]) 

    # RXD0_SKEW 
    drvExtPhyLan8840RxD0SkewEnable= drvExtPhyLan8840Component.createBooleanSymbol("DRV_ETHPHY_RXD0_SKEW_SET", drvExtPhyLan8840RxDataSkew)
    drvExtPhyLan8840RxD0SkewEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840RxD0SkewEnable.setLabel("RXD0 Skew") 
    drvExtPhyLan8840RxD0SkewEnable.setVisible(True)
    drvExtPhyLan8840RxD0SkewEnable.setDescription("Driver PHY RXD0 Skew Set")
    drvExtPhyLan8840RxD0SkewEnable.setDefaultValue(False)
    #Value 
    drvExtPhyLan8840RxD0Skew = drvExtPhyLan8840Component.createHexSymbol("DRV_ETHPHY_RXD0_SKEW", drvExtPhyLan8840RxD0SkewEnable)
    drvExtPhyLan8840RxD0Skew.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840RxD0Skew.setLabel("Skew Value")
    drvExtPhyLan8840RxD0Skew.setVisible(False)
    drvExtPhyLan8840RxD0Skew.setDescription("Driver PHY RXD0 Skew")
    drvExtPhyLan8840RxD0Skew.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_RXD0_SKEW_SET"])  
    # Comment 
    drvExtPhyLan8840RxD0SkewCmt = drvExtPhyLan8840Component.createCommentSymbol(None, drvExtPhyLan8840RxD0SkewEnable)
    drvExtPhyLan8840RxD0SkewCmt.setLabel("*** Range 0x00 - 0x0F")
    drvExtPhyLan8840RxD0SkewCmt.setVisible(False)  
    drvExtPhyLan8840RxD0SkewCmt.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_RXD0_SKEW_SET"]) 


    # TX Data Skew
    drvExtPhyLan8840TxDataSkew = drvExtPhyLan8840Component.createMenuSymbol(None, drvExtPhyLan8840SkewSet) 
    drvExtPhyLan8840TxDataSkew.setLabel("TX Data Skew")
    drvExtPhyLan8840TxDataSkew.setVisible(True)
    drvExtPhyLan8840TxDataSkew.setDescription("RGMII TX DATA SKEW")

    # TXD3_SKEW 
    drvExtPhyLan8840TxD3SkewEnable= drvExtPhyLan8840Component.createBooleanSymbol("DRV_ETHPHY_TXD3_SKEW_SET", drvExtPhyLan8840TxDataSkew)
    drvExtPhyLan8840TxD3SkewEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840TxD3SkewEnable.setLabel("TXD3 Skew") 
    drvExtPhyLan8840TxD3SkewEnable.setVisible(True)
    drvExtPhyLan8840TxD3SkewEnable.setDescription("Driver PHY TXD3 Skew Set")
    drvExtPhyLan8840TxD3SkewEnable.setDefaultValue(False)
    #Value 
    drvExtPhyLan8840TxD3Skew = drvExtPhyLan8840Component.createHexSymbol("DRV_ETHPHY_TXD3_SKEW", drvExtPhyLan8840TxD3SkewEnable)
    drvExtPhyLan8840TxD3Skew.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840TxD3Skew.setLabel("Skew Value")
    drvExtPhyLan8840TxD3Skew.setVisible(False)
    drvExtPhyLan8840TxD3Skew.setDescription("Driver PHY TXD3 Skew")
    drvExtPhyLan8840TxD3Skew.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_TXD3_SKEW_SET"])  
    # Comment 
    drvExtPhyLan8840TxD3SkewCmt = drvExtPhyLan8840Component.createCommentSymbol(None, drvExtPhyLan8840TxD3SkewEnable)
    drvExtPhyLan8840TxD3SkewCmt.setLabel("*** Range 0x00 - 0x0F")
    drvExtPhyLan8840TxD3SkewCmt.setVisible(False)  
    drvExtPhyLan8840TxD3SkewCmt.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_TXD3_SKEW_SET"]) 

    # TXD2_SKEW 
    drvExtPhyLan8840TxD2SkewEnable= drvExtPhyLan8840Component.createBooleanSymbol("DRV_ETHPHY_TXD2_SKEW_SET", drvExtPhyLan8840TxDataSkew)
    drvExtPhyLan8840TxD2SkewEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840TxD2SkewEnable.setLabel("TXD2 Skew") 
    drvExtPhyLan8840TxD2SkewEnable.setVisible(True)
    drvExtPhyLan8840TxD2SkewEnable.setDescription("Driver PHY TXD2 Skew Set")
    drvExtPhyLan8840TxD2SkewEnable.setDefaultValue(False)
    #Value 
    drvExtPhyLan8840TxD2Skew = drvExtPhyLan8840Component.createHexSymbol("DRV_ETHPHY_TXD2_SKEW", drvExtPhyLan8840TxD2SkewEnable)
    drvExtPhyLan8840TxD2Skew.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840TxD2Skew.setLabel("Skew Value")
    drvExtPhyLan8840TxD2Skew.setVisible(False)
    drvExtPhyLan8840TxD2Skew.setDescription("Driver PHY TXD2 Skew")
    drvExtPhyLan8840TxD2Skew.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_TXD2_SKEW_SET"])  
    # Comment 
    drvExtPhyLan8840TxD2SkewCmt = drvExtPhyLan8840Component.createCommentSymbol(None, drvExtPhyLan8840TxD2SkewEnable)
    drvExtPhyLan8840TxD2SkewCmt.setLabel("*** Range 0x00 - 0x0F")
    drvExtPhyLan8840TxD2SkewCmt.setVisible(False)  
    drvExtPhyLan8840TxD2SkewCmt.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_TXD2_SKEW_SET"]) 

    # TXD1_SKEW 
    drvExtPhyLan8840TxD1SkewEnable= drvExtPhyLan8840Component.createBooleanSymbol("DRV_ETHPHY_TXD1_SKEW_SET", drvExtPhyLan8840TxDataSkew)
    drvExtPhyLan8840TxD1SkewEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840TxD1SkewEnable.setLabel("TXD1 Skew") 
    drvExtPhyLan8840TxD1SkewEnable.setVisible(True)
    drvExtPhyLan8840TxD1SkewEnable.setDescription("Driver PHY TXD1 Skew Set")
    drvExtPhyLan8840TxD1SkewEnable.setDefaultValue(False)
    #Value 
    drvExtPhyLan8840TxD1Skew = drvExtPhyLan8840Component.createHexSymbol("DRV_ETHPHY_TXD1_SKEW", drvExtPhyLan8840TxD1SkewEnable)
    drvExtPhyLan8840TxD1Skew.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840TxD1Skew.setLabel("Skew Value")
    drvExtPhyLan8840TxD1Skew.setVisible(False)
    drvExtPhyLan8840TxD1Skew.setDescription("Driver PHY TXD1 Skew")
    drvExtPhyLan8840TxD1Skew.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_TXD1_SKEW_SET"])  
    # Comment 
    drvExtPhyLan8840TxD1SkewCmt = drvExtPhyLan8840Component.createCommentSymbol(None, drvExtPhyLan8840TxD1SkewEnable)
    drvExtPhyLan8840TxD1SkewCmt.setLabel("*** Range 0x00 - 0x0F")
    drvExtPhyLan8840TxD1SkewCmt.setVisible(False)  
    drvExtPhyLan8840TxD1SkewCmt.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_TXD1_SKEW_SET"]) 

    # TXD0_SKEW 
    drvExtPhyLan8840TxD0SkewEnable= drvExtPhyLan8840Component.createBooleanSymbol("DRV_ETHPHY_TXD0_SKEW_SET", drvExtPhyLan8840TxDataSkew)
    drvExtPhyLan8840TxD0SkewEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840TxD0SkewEnable.setLabel("TXD0 Skew") 
    drvExtPhyLan8840TxD0SkewEnable.setVisible(True)
    drvExtPhyLan8840TxD0SkewEnable.setDescription("Driver PHY TXD0 Skew Set")
    drvExtPhyLan8840TxD0SkewEnable.setDefaultValue(False)
    #Value 
    drvExtPhyLan8840TxD0Skew = drvExtPhyLan8840Component.createHexSymbol("DRV_ETHPHY_TXD0_SKEW", drvExtPhyLan8840TxD0SkewEnable)
    drvExtPhyLan8840TxD0Skew.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8840TxD0Skew.setLabel("Skew Value")
    drvExtPhyLan8840TxD0Skew.setVisible(False)
    drvExtPhyLan8840TxD0Skew.setDescription("Driver PHY TXD0 Skew")
    drvExtPhyLan8840TxD0Skew.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_TXD0_SKEW_SET"])  
    # Comment 
    drvExtPhyLan8840TxD0SkewCmt = drvExtPhyLan8840Component.createCommentSymbol(None, drvExtPhyLan8840TxD0SkewEnable)
    drvExtPhyLan8840TxD0SkewCmt.setLabel("*** Range 0x00 - 0x0F")
    drvExtPhyLan8840TxD0SkewCmt.setVisible(False)  
    drvExtPhyLan8840TxD0SkewCmt.setDependencies(drvExtPhyLan8840MenuVisibleSingle, ["DRV_ETHPHY_TXD0_SKEW_SET"])     
                            
    #Add forward declaration to initialization.c
    drvExtPhyLan8840InitDataSourceFtl = drvExtPhyLan8840Component.createFileSymbol(None, None)
    drvExtPhyLan8840InitDataSourceFtl.setType("STRING")
    drvExtPhyLan8840InitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    drvExtPhyLan8840InitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_driver_initialize.c.ftl")
    drvExtPhyLan8840InitDataSourceFtl.setMarkup(True)    
    
    #Add to initialization.c
    drvExtPhyLan8840SysInitDataSourceFtl = drvExtPhyLan8840Component.createFileSymbol(None, None)
    drvExtPhyLan8840SysInitDataSourceFtl.setType("STRING")
    drvExtPhyLan8840SysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    drvExtPhyLan8840SysInitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_data_initialize.c.ftl")
    drvExtPhyLan8840SysInitDataSourceFtl.setMarkup(True)    
    
    #Add to system_config.h
    drvExtPhyLan8840HeaderFtl = drvExtPhyLan8840Component.createFileSymbol(None, None)
    drvExtPhyLan8840HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_lan8840.h.ftl")
    drvExtPhyLan8840HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    drvExtPhyLan8840HeaderFtl.setMarkup(True)
    drvExtPhyLan8840HeaderFtl.setType("STRING")
        
    # file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
    # Add drv_ethphy.h file to project
    drvExtPhyHeaderFile = drvExtPhyLan8840Component.createFileSymbol(None, None)
    drvExtPhyHeaderFile.setSourcePath("driver/ethphy/drv_ethphy.h")
    drvExtPhyHeaderFile.setOutputName("drv_ethphy.h")
    drvExtPhyHeaderFile.setDestPath("driver/ethphy/")
    drvExtPhyHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/")
    drvExtPhyHeaderFile.setType("HEADER")
    drvExtPhyHeaderFile.setOverwrite(True)
    drvExtPhyHeaderFile.setEnabled(True)

    # file TCPIP_ETH_PHY_LOCAL_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/drv_ethphy_local.h" to           "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/drv_ethphy_local.h"
    # Add drv_ethphy_local.h file to project
    drvExtPhyLocalHeaderFile = drvExtPhyLan8840Component.createFileSymbol(None, None)
    drvExtPhyLocalHeaderFile.setSourcePath("driver/ethphy/src/drv_ethphy_local.h")
    drvExtPhyLocalHeaderFile.setOutputName("drv_ethphy_local.h")
    drvExtPhyLocalHeaderFile.setDestPath("driver/ethphy/src/")
    drvExtPhyLocalHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/")
    drvExtPhyLocalHeaderFile.setType("HEADER")
    drvExtPhyLocalHeaderFile.setOverwrite(True)
    drvExtPhyLocalHeaderFile.setEnabled(True)


    # file TCPIP_ETH_EXT_PHY_REGS_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h" to    "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h"
    # Add drv_extphy_regs.h file to project
    drvExtPhyLan8840RegHeaderFile = drvExtPhyLan8840Component.createFileSymbol(None, None)
    drvExtPhyLan8840RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
    drvExtPhyLan8840RegHeaderFile.setOutputName("drv_extphy_regs.h")
    drvExtPhyLan8840RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyLan8840RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyLan8840RegHeaderFile.setType("HEADER")
    drvExtPhyLan8840RegHeaderFile.setOverwrite(True)
    drvExtPhyLan8840RegHeaderFile.setEnabled(True)
    
    # Add drv_extphy_lan8840.h file to project
    drvExtPhyLan8840HeaderFile = drvExtPhyLan8840Component.createFileSymbol(None, None)
    drvExtPhyLan8840HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_lan8840.h")
    drvExtPhyLan8840HeaderFile.setOutputName("drv_extphy_lan8840.h")
    drvExtPhyLan8840HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyLan8840HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyLan8840HeaderFile.setType("HEADER")
    drvExtPhyLan8840HeaderFile.setOverwrite(True)
    drvExtPhyLan8840HeaderFile.setEnabled(True)



    # file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
    # Add drv_ethphy.c file
    drvExtPhySourceFile = drvExtPhyLan8840Component.createFileSymbol(None, None)
    drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
    drvExtPhySourceFile.setOutputName("drv_ethphy.c")
    drvExtPhySourceFile.setOverwrite(True)
    drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setType("SOURCE")
    drvExtPhySourceFile.setEnabled(True)

    # Add drv_extphy_lan8840.c file
    drvExtPhyLan8840SourceFile = drvExtPhyLan8840Component.createFileSymbol(None, None)
    drvExtPhyLan8840SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_lan8840.c")
    drvExtPhyLan8840SourceFile.setOutputName("drv_extphy_lan8840.c")
    drvExtPhyLan8840SourceFile.setOverwrite(True)
    drvExtPhyLan8840SourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyLan8840SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyLan8840SourceFile.setType("SOURCE")
    drvExtPhyLan8840SourceFile.setEnabled(True)

    
def drvExtPhyLan8840MenuVisibleSingle(symbol, event):
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
