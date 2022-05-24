"""*****************************************************************************
* Copyright (C) 2022 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************"""


def instantiateComponent(drvExtPhyKsz9131Component):
    print("KSZ9131 PHY Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")    
        
    drvExtPhyKsz9131PartName = drvExtPhyKsz9131Component.createComboSymbol("DRV_ETHPHY_KSZ9131_PART",None,["KSZ9131MNX", "KSZ9131RNX"])
    drvExtPhyKsz9131PartName.setHelp("mcc_h3_gmac_configurations")
    drvExtPhyKsz9131PartName.setVisible(True)
    drvExtPhyKsz9131PartName.setLabel("Part Name")
    drvExtPhyKsz9131PartName.setDefaultValue("KSZ9131RNX")
    
    # PHY Address
    drvExtPhyKsz9131Addr= drvExtPhyKsz9131Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
    drvExtPhyKsz9131Addr.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9131Addr.setLabel("PHY Address") 
    drvExtPhyKsz9131Addr.setVisible(True)
    drvExtPhyKsz9131Addr.setDescription("PHY Address")
    drvExtPhyKsz9131Addr.setDefaultValue(1)
    
    # Use a Function to be called at PHY Reset
    drvExtPhyKsz9131ResetCallbackEnable = drvExtPhyKsz9131Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
    drvExtPhyKsz9131ResetCallbackEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9131ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
    drvExtPhyKsz9131ResetCallbackEnable.setVisible(True)
    drvExtPhyKsz9131ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
    drvExtPhyKsz9131ResetCallbackEnable.setDefaultValue(False)
    
    # App Function
    drvExtPhyKsz9131ResetCallback = drvExtPhyKsz9131Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyKsz9131ResetCallbackEnable)
    drvExtPhyKsz9131ResetCallback.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9131ResetCallback.setLabel("App Function")
    drvExtPhyKsz9131ResetCallback.setVisible(False)
    drvExtPhyKsz9131ResetCallback.setDescription("App Function")
    drvExtPhyKsz9131ResetCallback.setDefaultValue("AppPhyResetFunction")
    drvExtPhyKsz9131ResetCallback.setDependencies(drvExtPhyKsz9131MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])  
    
    # External PHY Connection Flags
    drvExtPhyKsz9131ConnFlag = drvExtPhyKsz9131Component.createMenuSymbol(None, None) 
    drvExtPhyKsz9131ConnFlag.setLabel("External PHY Connection Flags")
    drvExtPhyKsz9131ConnFlag.setVisible(True)
    drvExtPhyKsz9131ConnFlag.setDescription("External PHY Connection Flags")
    
    # GMII Data Interface
    drvExtPhyKsz9131ConfigGmii = drvExtPhyKsz9131Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_GMII", drvExtPhyKsz9131ConnFlag)
    drvExtPhyKsz9131ConfigGmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9131ConfigGmii.setLabel("GMII Data Interface")
    drvExtPhyKsz9131ConfigGmii.setVisible(False)
    drvExtPhyKsz9131ConfigGmii.setDefaultValue(False)
    drvExtPhyKsz9131ConfigGmii.setDescription("GMII Data Interface")
    drvExtPhyKsz9131ConfigGmii.setDependencies(drvExtPhyKsz9131MiimInterface, ["DRV_ETHPHY_KSZ9131_PART"])
    
    # RGMII Data Interface
    drvExtPhyKsz9131ConfigRgmii = drvExtPhyKsz9131Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RGMII", drvExtPhyKsz9131ConnFlag)
    drvExtPhyKsz9131ConfigRgmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9131ConfigRgmii.setLabel("RGMII Data Interface")
    drvExtPhyKsz9131ConfigRgmii.setVisible(True)
    drvExtPhyKsz9131ConfigRgmii.setDefaultValue(True)
    drvExtPhyKsz9131ConfigRgmii.setDescription("RGMII Data Interface")
    drvExtPhyKsz9131ConfigRgmii.setDependencies(drvExtPhyKsz9131MiimInterface, ["DRV_ETHPHY_KSZ9131_PART"])
    
    # MII Data Interface
    drvExtPhyKsz9131ConfigMii = drvExtPhyKsz9131Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_MII", drvExtPhyKsz9131ConnFlag)
    drvExtPhyKsz9131ConfigMii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9131ConfigMii.setLabel("MII Data Interface")
    drvExtPhyKsz9131ConfigMii.setVisible(False)
    drvExtPhyKsz9131ConfigMii.setDescription("MII Data Interface")
    drvExtPhyKsz9131ConfigMii.setDefaultValue(False)
    drvExtPhyKsz9131ConfigMii.setDependencies(drvExtPhyKsz9131MiimInterface, ["DRV_ETHPHY_KSZ9131_PART"])
    
    # RMII Data Interface
    drvExtPhyKsz9131ConfigRmii = drvExtPhyKsz9131Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyKsz9131ConnFlag)
    drvExtPhyKsz9131ConfigRmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9131ConfigRmii.setLabel("RMII Data Interface")
    drvExtPhyKsz9131ConfigRmii.setVisible(False)
    drvExtPhyKsz9131ConfigRmii.setDescription("RMII Data Interface")
    drvExtPhyKsz9131ConfigRmii.setDefaultValue(False)
    drvExtPhyKsz9131ConfigRmii.setDependencies(drvExtPhyKsz9131MiimInterface, ["DRV_ETHPHY_KSZ9131_PART"])
        
    # Advanced Settings
    drvExtPhyKsz9131AdvSettings = drvExtPhyKsz9131Component.createMenuSymbol("TCPIP_INTMAC_PHY_ADV_SETTING", None)
    drvExtPhyKsz9131AdvSettings.setLabel("Advanced Settings")
    drvExtPhyKsz9131AdvSettings.setDescription("Advanced Settings")
    drvExtPhyKsz9131AdvSettings.setVisible(True)

    # Delay for the Link Initialization in ms
    drvExtPhyKsz9131LinkInitDelay= drvExtPhyKsz9131Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", drvExtPhyKsz9131AdvSettings)
    drvExtPhyKsz9131LinkInitDelay.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9131LinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
    drvExtPhyKsz9131LinkInitDelay.setVisible(True)
    drvExtPhyKsz9131LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
    drvExtPhyKsz9131LinkInitDelay.setDefaultValue(500)
   
    # External PHY Type
    drvExtPhyKsz9131PhyType = drvExtPhyKsz9131Component.createStringSymbol("TCPIP_EMAC_PHY_TYPE", drvExtPhyKsz9131AdvSettings)
    drvExtPhyKsz9131PhyType.setVisible(False)   
    drvExtPhyKsz9131PhyType.setDefaultValue("KSZ9131")
    
    # Driver PHY Negotiation Time-out - ms
    drvExtPhyKsz9131NegInitTimeout= drvExtPhyKsz9131Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", drvExtPhyKsz9131AdvSettings)
    drvExtPhyKsz9131NegInitTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9131NegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
    drvExtPhyKsz9131NegInitTimeout.setVisible(True)
    drvExtPhyKsz9131NegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
    drvExtPhyKsz9131NegInitTimeout.setDefaultValue(1)
    
    # Driver PHY Negotiation Done Time-out - ms
    drvExtPhyKsz9131NegDoneTimeout= drvExtPhyKsz9131Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", drvExtPhyKsz9131AdvSettings)
    drvExtPhyKsz9131NegDoneTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9131NegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
    drvExtPhyKsz9131NegDoneTimeout.setVisible(True)
    drvExtPhyKsz9131NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
    drvExtPhyKsz9131NegDoneTimeout.setDefaultValue(2000)
    
    # Driver PHY Reset Clear Time-out - ms
    drvExtPhyKsz9131ResetClearTimeout= drvExtPhyKsz9131Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", drvExtPhyKsz9131AdvSettings)
    drvExtPhyKsz9131ResetClearTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9131ResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
    drvExtPhyKsz9131ResetClearTimeout.setVisible(True)
    drvExtPhyKsz9131ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
    drvExtPhyKsz9131ResetClearTimeout.setDefaultValue(500)
    
    # Driver PHY Peripheral ID
    drvExtPhyKsz9131PeripheralId= drvExtPhyKsz9131Component.createStringSymbol("DRV_ETHPHY_PERIPHERAL_ID", drvExtPhyKsz9131AdvSettings) #todo# convert it to string for other PHYs too
    drvExtPhyKsz9131PeripheralId.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9131PeripheralId.setLabel("PHY Peripheral ID") 
    drvExtPhyKsz9131PeripheralId.setVisible(True)
    drvExtPhyKsz9131PeripheralId.setDescription("Driver PHY Peripheral ID")
    drvExtPhyKsz9131PeripheralId.setDefaultValue("")
    drvExtPhyKsz9131PeripheralId.setReadOnly(True)

    # External MAC Name
    drvExtPhyMacName= drvExtPhyKsz9131Component.createStringSymbol("DRV_ETHPHY_MAC_NAME", None)
    drvExtPhyMacName.setLabel("Mac Name") 
    drvExtPhyMacName.setVisible(False)   
    drvExtPhyMacName.setDefaultValue("")
    drvExtPhyMacName.setReadOnly(True) 
                            
    #Add forward declaration to initialization.c
    drvExtPhyKsz9131InitDataSourceFtl = drvExtPhyKsz9131Component.createFileSymbol(None, None)
    drvExtPhyKsz9131InitDataSourceFtl.setType("STRING")
    drvExtPhyKsz9131InitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    drvExtPhyKsz9131InitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_driver_initialize.c.ftl")
    drvExtPhyKsz9131InitDataSourceFtl.setMarkup(True)    
    
    #Add to initialization.c
    drvExtPhyKsz9131SysInitDataSourceFtl = drvExtPhyKsz9131Component.createFileSymbol(None, None)
    drvExtPhyKsz9131SysInitDataSourceFtl.setType("STRING")
    drvExtPhyKsz9131SysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    drvExtPhyKsz9131SysInitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_data_initialize.c.ftl")
    drvExtPhyKsz9131SysInitDataSourceFtl.setMarkup(True)    
    
    #Add to system_config.h
    drvExtPhyKsz9131HeaderFtl = drvExtPhyKsz9131Component.createFileSymbol(None, None)
    drvExtPhyKsz9131HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_ksz9131.h.ftl")
    drvExtPhyKsz9131HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    drvExtPhyKsz9131HeaderFtl.setMarkup(True)
    drvExtPhyKsz9131HeaderFtl.setType("STRING")
        
    # file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
    # Add drv_ethphy.h file to project
    drvExtPhyHeaderFile = drvExtPhyKsz9131Component.createFileSymbol(None, None)
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
    drvExtPhyLocalHeaderFile = drvExtPhyKsz9131Component.createFileSymbol(None, None)
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
    drvExtPhyKsz9131RegHeaderFile = drvExtPhyKsz9131Component.createFileSymbol(None, None)
    drvExtPhyKsz9131RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
    drvExtPhyKsz9131RegHeaderFile.setOutputName("drv_extphy_regs.h")
    drvExtPhyKsz9131RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyKsz9131RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyKsz9131RegHeaderFile.setType("HEADER")
    drvExtPhyKsz9131RegHeaderFile.setOverwrite(True)
    drvExtPhyKsz9131RegHeaderFile.setEnabled(True)
    
    # Add drv_extphy_ksz9131.h file to project
    drvExtPhyKsz9131HeaderFile = drvExtPhyKsz9131Component.createFileSymbol(None, None)
    drvExtPhyKsz9131HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz9131.h")
    drvExtPhyKsz9131HeaderFile.setOutputName("drv_extphy_ksz9131.h")
    drvExtPhyKsz9131HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyKsz9131HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyKsz9131HeaderFile.setType("HEADER")
    drvExtPhyKsz9131HeaderFile.setOverwrite(True)
    drvExtPhyKsz9131HeaderFile.setEnabled(True)



    # file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
    # Add drv_ethphy.c file
    drvExtPhySourceFile = drvExtPhyKsz9131Component.createFileSymbol(None, None)
    drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
    drvExtPhySourceFile.setOutputName("drv_ethphy.c")
    drvExtPhySourceFile.setOverwrite(True)
    drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setType("SOURCE")
    drvExtPhySourceFile.setEnabled(True)

    # Add drv_extphy_ksz9131.c file
    drvExtPhyKsz9131SourceFile = drvExtPhyKsz9131Component.createFileSymbol(None, None)
    drvExtPhyKsz9131SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz9131.c")
    drvExtPhyKsz9131SourceFile.setOutputName("drv_extphy_ksz9131.c")
    drvExtPhyKsz9131SourceFile.setOverwrite(True)
    drvExtPhyKsz9131SourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyKsz9131SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyKsz9131SourceFile.setType("SOURCE")
    drvExtPhyKsz9131SourceFile.setEnabled(True)

    
def drvExtPhyKsz9131MenuVisibleSingle(symbol, event):
    if (event["value"] == True):
        print("EthMac Menu Visible.")       
        symbol.setVisible(True)
    else:
        print("EthMac Menu Invisible.")
        symbol.setVisible(False)

def drvExtPhyKsz9131MiimInterface(symbol, event):
    if (event["value"] == "KSZ9131RNX"):
        if(symbol.getID() == "TCPIP_INTMAC_PHY_CONFIG_RGMII"):         
            symbol.setVisible(True)
        else:
            symbol.setVisible(False)
            symbol.clearValue()
    else:
        if(symbol.getID() == "TCPIP_INTMAC_PHY_CONFIG_GMII") or (symbol.getID() == "TCPIP_INTMAC_PHY_CONFIG_MII"):   
            symbol.setVisible(True)
        else:
            symbol.setVisible(False)
            symbol.clearValue()
            
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
