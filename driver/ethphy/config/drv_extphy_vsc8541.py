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

def instantiateComponent(drvExtPhyVsc8541Component, index):
    global phyComponentId
    configName = Variables.get("__CONFIGURATION_NAME")    
    phyComponentId = drvExtPhyVsc8541Component.getID()
    print("VSC8541 PHY Driver Component ID : " + phyComponentId)
    
    drvExtPhyVsc8541InstnIndex = drvExtPhyVsc8541Component.createIntegerSymbol("INDEX", None)
    drvExtPhyVsc8541InstnIndex.setLabel("VSC8541 Instance Number")
    drvExtPhyVsc8541InstnIndex.setVisible(True)
    drvExtPhyVsc8541InstnIndex.setDefaultValue(index)
    drvExtPhyVsc8541InstnIndex.setReadOnly(True)

    # PHY Address
    drvExtPhyVsc8541Addr= drvExtPhyVsc8541Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS_IDX" + str(index), None)
    drvExtPhyVsc8541Addr.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8541Addr.setLabel("PHY Address") 
    drvExtPhyVsc8541Addr.setVisible(True)
    drvExtPhyVsc8541Addr.setDescription("PHY Address")
    drvExtPhyVsc8541Addr.setDefaultValue(1)
    
    # Use a Function to be called at PHY Reset
    drvExtPhyVsc8541ResetCallbackEnable = drvExtPhyVsc8541Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK_IDX" + str(index), None)
    drvExtPhyVsc8541ResetCallbackEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8541ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
    drvExtPhyVsc8541ResetCallbackEnable.setVisible(True)
    drvExtPhyVsc8541ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
    drvExtPhyVsc8541ResetCallbackEnable.setDefaultValue(False)
    
    # App Function
    drvExtPhyVsc8541ResetCallback = drvExtPhyVsc8541Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK_IDX" + str(index), drvExtPhyVsc8541ResetCallbackEnable)
    drvExtPhyVsc8541ResetCallback.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8541ResetCallback.setLabel("App Function")
    drvExtPhyVsc8541ResetCallback.setVisible(False)
    drvExtPhyVsc8541ResetCallback.setDescription("App Function")
    drvExtPhyVsc8541ResetCallback.setDefaultValue("App" + phyName + "ResetFunction")
    drvExtPhyVsc8541ResetCallback.setDependencies(drvExtPhyVsc8541MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK_IDX" + str(index)])  
    
    # External PHY Connection Flags
    drvExtPhyVsc8541ConnFlag = drvExtPhyVsc8541Component.createMenuSymbol(None, None) 
    drvExtPhyVsc8541ConnFlag.setLabel("External PHY Connection Flags")
    drvExtPhyVsc8541ConnFlag.setVisible(True)
    drvExtPhyVsc8541ConnFlag.setDescription("External PHY Connection Flags")
    
    # RGMII Data Interface
    drvExtPhyVsc8541ConfigRgmii = drvExtPhyVsc8541Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RGMII_IDX" + str(index), drvExtPhyVsc8541ConnFlag)
    drvExtPhyVsc8541ConfigRgmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8541ConfigRgmii.setLabel("RGMII Data Interface")
    drvExtPhyVsc8541ConfigRgmii.setVisible(True)
    drvExtPhyVsc8541ConfigRgmii.setDefaultValue(True)
    drvExtPhyVsc8541ConfigRgmii.setDescription("RGMII Data Interface")
        
    # RMII Data Interface
    drvExtPhyVsc8541ConfigRmii = drvExtPhyVsc8541Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII_IDX" + str(index), drvExtPhyVsc8541ConnFlag)
    drvExtPhyVsc8541ConfigRmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8541ConfigRmii.setLabel("RMII Data Interface")
    drvExtPhyVsc8541ConfigRmii.setVisible(True)
    drvExtPhyVsc8541ConfigRmii.setDefaultValue(False)
    drvExtPhyVsc8541ConfigRmii.setDescription("RMII Data Interface")

    # Advanced Settings
    drvExtPhyVsc8541AdvSettings = drvExtPhyVsc8541Component.createMenuSymbol("TCPIP_INTMAC_PHY_ADV_SETTING_IDX" + str(index), None)
    drvExtPhyVsc8541AdvSettings.setLabel("Advanced Settings")
    drvExtPhyVsc8541AdvSettings.setDescription("Advanced Settings")
    drvExtPhyVsc8541AdvSettings.setVisible(True)

    # Delay for the Link Initialization in ms
    drvExtPhyVsc8541LinkInitDelay= drvExtPhyVsc8541Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY_IDX" + str(index), drvExtPhyVsc8541AdvSettings)
    drvExtPhyVsc8541LinkInitDelay.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8541LinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
    drvExtPhyVsc8541LinkInitDelay.setVisible(True)
    drvExtPhyVsc8541LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
    drvExtPhyVsc8541LinkInitDelay.setDefaultValue(500)
   
    # External PHY Type
    drvExtPhyVsc8541PhyType = drvExtPhyVsc8541Component.createStringSymbol("TCPIP_EMAC_PHY_TYPE_IDX" + str(index), drvExtPhyVsc8541AdvSettings)
    drvExtPhyVsc8541PhyType.setVisible(False)   
    drvExtPhyVsc8541PhyType.setDefaultValue(phyName)
    
    # Driver PHY Negotiation Time-out - ms
    drvExtPhyVsc8541NegInitTimeout= drvExtPhyVsc8541Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO_IDX" + str(index), drvExtPhyVsc8541AdvSettings)
    drvExtPhyVsc8541NegInitTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8541NegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
    drvExtPhyVsc8541NegInitTimeout.setVisible(True)
    drvExtPhyVsc8541NegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
    drvExtPhyVsc8541NegInitTimeout.setDefaultValue(1)
    
    # Driver PHY Negotiation Done Time-out - ms
    drvExtPhyVsc8541NegDoneTimeout= drvExtPhyVsc8541Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO_IDX" + str(index), drvExtPhyVsc8541AdvSettings)
    drvExtPhyVsc8541NegDoneTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8541NegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
    drvExtPhyVsc8541NegDoneTimeout.setVisible(True)
    drvExtPhyVsc8541NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
    drvExtPhyVsc8541NegDoneTimeout.setDefaultValue(2000)
    
    # Driver PHY Reset Clear Time-out - ms
    drvExtPhyVsc8541ResetClearTimeout= drvExtPhyVsc8541Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO_IDX" + str(index), drvExtPhyVsc8541AdvSettings)
    drvExtPhyVsc8541ResetClearTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8541ResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
    drvExtPhyVsc8541ResetClearTimeout.setVisible(True)
    drvExtPhyVsc8541ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
    drvExtPhyVsc8541ResetClearTimeout.setDefaultValue(500)
    
    # Driver PHY Peripheral ID
    drvExtPhyVsc8541PeripheralId= drvExtPhyVsc8541Component.createStringSymbol("DRV_ETHPHY_PERIPHERAL_ID_IDX" + str(index), drvExtPhyVsc8541AdvSettings)
    drvExtPhyVsc8541PeripheralId.setHelp("mcc_h3_phy_configurations")
    drvExtPhyVsc8541PeripheralId.setLabel("PHY Peripheral ID") 
    drvExtPhyVsc8541PeripheralId.setVisible(True)
    drvExtPhyVsc8541PeripheralId.setDescription("Driver PHY Peripheral ID")
    drvExtPhyVsc8541PeripheralId.setDefaultValue("")
    drvExtPhyVsc8541PeripheralId.setReadOnly(True)

    # External MAC Name
    drvExtPhyMacName= drvExtPhyVsc8541Component.createStringSymbol("DRV_ETHPHY_MAC_NAME_IDX" + str(index), None)
    drvExtPhyMacName.setLabel("Mac Name") 
    drvExtPhyMacName.setVisible(False)   
    drvExtPhyMacName.setDefaultValue("")
    drvExtPhyMacName.setReadOnly(True) 
    
    
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
    
