"""*****************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
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

phyName = "DP83640"
def instantiateComponent(drvExtPhyDp83640Component):
    print("DP83640 PHY Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")

    # PHY Address
    drvExtPhyDp83640Addr= drvExtPhyDp83640Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
    drvExtPhyDp83640Addr.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDp83640Addr.setLabel("PHY Address") 
    drvExtPhyDp83640Addr.setVisible(True)
    drvExtPhyDp83640Addr.setDescription("PHY Address")
    drvExtPhyDp83640Addr.setDefaultValue(0)
    
    # Use a Function to be called at PHY Reset
    drvExtPhyDp83640ResetCallbackEnable = drvExtPhyDp83640Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
    drvExtPhyDp83640ResetCallbackEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDp83640ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
    drvExtPhyDp83640ResetCallbackEnable.setVisible(True)
    drvExtPhyDp83640ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
    drvExtPhyDp83640ResetCallbackEnable.setDefaultValue(False)
    
    # App Function
    drvExtPhyDp83640ResetCallback = drvExtPhyDp83640Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyDp83640ResetCallbackEnable)
    drvExtPhyDp83640ResetCallback.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDp83640ResetCallback.setLabel("App Function")
    drvExtPhyDp83640ResetCallback.setVisible(False)
    drvExtPhyDp83640ResetCallback.setDescription("App Function")
    drvExtPhyDp83640ResetCallback.setDefaultValue("App" + phyName + "ResetFunction")
    drvExtPhyDp83640ResetCallback.setDependencies(drvExtPhyDp83640MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])

    # External PHY Connection Flags
    drvExtPhyDp83640ConnFlag = drvExtPhyDp83640Component.createMenuSymbol(None, None) 
    drvExtPhyDp83640ConnFlag.setLabel("External PHY Connection Flags")
    drvExtPhyDp83640ConnFlag.setVisible(True)
    drvExtPhyDp83640ConnFlag.setDescription("External PHY Connection Flags")
    
    # RMII Data Interface
    drvExtPhyDp83640ConfigRmii = drvExtPhyDp83640Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyDp83640ConnFlag)
    drvExtPhyDp83640ConfigRmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDp83640ConfigRmii.setLabel("RMII Data Interface")
    drvExtPhyDp83640ConfigRmii.setVisible(True)
    drvExtPhyDp83640ConfigRmii.setDescription("RMII Data Interface")
    if Peripheral.moduleExists("GMAC"):
        drvExtPhyDp83640ConfigRmii.setDefaultValue(True)
    elif "PIC32M" in Variables.get("__PROCESSOR"):
        drvExtPhyDp83640ConfigRmii.setDefaultValue(False)
    
        # Configuration Fuses Is ALT
        drvExtPhyDp83640ConfigAlt = drvExtPhyDp83640Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_ALTERNATE", drvExtPhyDp83640ConnFlag)
        drvExtPhyDp83640ConfigAlt.setHelp("mcc_h3_phy_configurations")
        drvExtPhyDp83640ConfigAlt.setLabel("Configuration Fuses Is ALT")
        drvExtPhyDp83640ConfigAlt.setVisible(True)
        drvExtPhyDp83640ConfigAlt.setDescription("Configuration Fuses Is ALT")
        drvExtPhyDp83640ConfigAlt.setDefaultValue(False)
        
        # Use The Fuses Configuration
        drvExtPhyDp83640ConfigAuto = drvExtPhyDp83640Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTO", drvExtPhyDp83640ConnFlag)
        drvExtPhyDp83640ConfigAuto.setHelp("mcc_h3_phy_configurations")
        drvExtPhyDp83640ConfigAuto.setLabel("Use The Fuses Configuration")
        drvExtPhyDp83640ConfigAuto.setVisible(True)
        drvExtPhyDp83640ConfigAuto.setDescription("Use The Fuses Configuration")
        drvExtPhyDp83640ConfigAuto.setDefaultValue(False)
        
    # Advanced Settings
    drvExtPhyDp83640AdvSettings = drvExtPhyDp83640Component.createMenuSymbol("TCPIP_INTMAC_PHY_ADV_SETTING", None)
    drvExtPhyDp83640AdvSettings.setLabel("Advanced Settings")
    drvExtPhyDp83640AdvSettings.setDescription("Advanced Settings")
    drvExtPhyDp83640AdvSettings.setVisible(True)

    # Delay for the Link Initialization in ms
    drvExtPhyDp83640LinkInitDelay= drvExtPhyDp83640Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", drvExtPhyDp83640AdvSettings)
    drvExtPhyDp83640LinkInitDelay.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDp83640LinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
    drvExtPhyDp83640LinkInitDelay.setVisible(True)
    drvExtPhyDp83640LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
    drvExtPhyDp83640LinkInitDelay.setDefaultValue(500)
    
    # External PHY Type
    drvExtPhyDp83640PhyType = drvExtPhyDp83640Component.createStringSymbol("TCPIP_EMAC_PHY_TYPE", drvExtPhyDp83640AdvSettings)
    drvExtPhyDp83640PhyType.setVisible(False)   
    drvExtPhyDp83640PhyType.setDefaultValue(phyName)
    
    # Driver PHY Negotiation Time-out (mSec)
    drvExtPhyDp83640NegInitTimeout= drvExtPhyDp83640Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", drvExtPhyDp83640AdvSettings)
    drvExtPhyDp83640NegInitTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDp83640NegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
    drvExtPhyDp83640NegInitTimeout.setVisible(True)
    drvExtPhyDp83640NegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
    drvExtPhyDp83640NegInitTimeout.setDefaultValue(1)
    
    # Driver PHY Negotiation Done Time-out (mSec)
    drvExtPhyDp83640NegDoneTimeout= drvExtPhyDp83640Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", drvExtPhyDp83640AdvSettings)
    drvExtPhyDp83640NegDoneTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDp83640NegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
    drvExtPhyDp83640NegDoneTimeout.setVisible(True)
    drvExtPhyDp83640NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
    drvExtPhyDp83640NegDoneTimeout.setDefaultValue(2000)
    
    # Driver PHY Reset Clear Time-out (mSec)
    drvExtPhyDp83640ResetClearTimeout= drvExtPhyDp83640Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", drvExtPhyDp83640AdvSettings)
    drvExtPhyDp83640ResetClearTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDp83640ResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
    drvExtPhyDp83640ResetClearTimeout.setVisible(True)
    drvExtPhyDp83640ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
    drvExtPhyDp83640ResetClearTimeout.setDefaultValue(500)
    
    # Driver PHY Instances Number
    drvExtPhyDp83640InstanceNum= drvExtPhyDp83640Component.createIntegerSymbol("DRV_ETHPHY_INSTANCES_NUMBER", drvExtPhyDp83640AdvSettings)
    drvExtPhyDp83640InstanceNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDp83640InstanceNum.setLabel("PHY Instances Number") 
    drvExtPhyDp83640InstanceNum.setVisible(True)
    drvExtPhyDp83640InstanceNum.setDescription("Driver PHY Instances Number")
    drvExtPhyDp83640InstanceNum.setDefaultValue(1)
    drvExtPhyDp83640InstanceNum.setReadOnly(True)
    
    # Driver PHY Clients Number
    drvExtPhyDp83640ClientNum= drvExtPhyDp83640Component.createIntegerSymbol("DRV_ETHPHY_CLIENTS_NUMBER", drvExtPhyDp83640AdvSettings)
    drvExtPhyDp83640ClientNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDp83640ClientNum.setLabel("PHY Clients Number") 
    drvExtPhyDp83640ClientNum.setVisible(True)
    drvExtPhyDp83640ClientNum.setDescription("Driver PHY Clients Number")
    drvExtPhyDp83640ClientNum.setDefaultValue(1)
    drvExtPhyDp83640ClientNum.setReadOnly(True)
    
    # Driver PHY Peripheral Index Number
    drvExtPhyDp83640IndexNum= drvExtPhyDp83640Component.createIntegerSymbol("DRV_ETHPHY_INDEX", drvExtPhyDp83640AdvSettings)
    drvExtPhyDp83640IndexNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDp83640IndexNum.setLabel("PHY Peripheral Index Number") 
    drvExtPhyDp83640IndexNum.setVisible(True)
    drvExtPhyDp83640IndexNum.setDescription("Driver PHY Peripheral Index Number")
    drvExtPhyDp83640IndexNum.setDefaultValue(1)
    drvExtPhyDp83640IndexNum.setReadOnly(True)
    
    # Driver PHY Peripheral ID
    drvExtPhyDp83640PeripheralId= drvExtPhyDp83640Component.createStringSymbol("DRV_ETHPHY_PERIPHERAL_ID", drvExtPhyDp83640AdvSettings)
    drvExtPhyDp83640PeripheralId.setHelp("mcc_h3_phy_configurations")
    drvExtPhyDp83640PeripheralId.setLabel("PHY Peripheral ID") 
    drvExtPhyDp83640PeripheralId.setVisible(True)
    drvExtPhyDp83640PeripheralId.setDescription("Driver PHY Peripheral ID")
    drvExtPhyDp83640PeripheralId.setDefaultValue("")
    drvExtPhyDp83640PeripheralId.setReadOnly(True)

    # External MAC Name
    drvExtPhyMacName= drvExtPhyDp83640Component.createStringSymbol("DRV_ETHPHY_MAC_NAME", None)
    drvExtPhyMacName.setLabel("Mac Name") 
    drvExtPhyMacName.setVisible(False)   
    drvExtPhyMacName.setDefaultValue("")
    drvExtPhyMacName.setReadOnly(True) 
    
    #Add forward declaration to initialization.c
    drvExtPhyDp83640InitDataSourceFtl = drvExtPhyDp83640Component.createFileSymbol(None, None)
    drvExtPhyDp83640InitDataSourceFtl.setType("STRING")
    drvExtPhyDp83640InitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    drvExtPhyDp83640InitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_driver_initialize.c.ftl")
    drvExtPhyDp83640InitDataSourceFtl.setMarkup(True)    
    
    #Add to initialization.c
    drvExtPhyDp83640SysInitDataSourceFtl = drvExtPhyDp83640Component.createFileSymbol(None, None)
    drvExtPhyDp83640SysInitDataSourceFtl.setType("STRING")
    drvExtPhyDp83640SysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    drvExtPhyDp83640SysInitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_data_initialize.c.ftl")
    drvExtPhyDp83640SysInitDataSourceFtl.setMarkup(True)    
    
    #Add to system_config.h
    drvExtPhyDp83640HeaderFtl = drvExtPhyDp83640Component.createFileSymbol(None, None)
    drvExtPhyDp83640HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_dp83640.h.ftl")
    drvExtPhyDp83640HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    drvExtPhyDp83640HeaderFtl.setMarkup(True)
    drvExtPhyDp83640HeaderFtl.setType("STRING")
    
    # file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
    # Add drv_ethphy.h file to project
    drvExtPhyHeaderFile = drvExtPhyDp83640Component.createFileSymbol(None, None)
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
    drvExtPhyLocalHeaderFile = drvExtPhyDp83640Component.createFileSymbol(None, None)
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
    drvExtPhyDp83640RegHeaderFile = drvExtPhyDp83640Component.createFileSymbol(None, None)
    drvExtPhyDp83640RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
    drvExtPhyDp83640RegHeaderFile.setOutputName("drv_extphy_regs.h")
    drvExtPhyDp83640RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyDp83640RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyDp83640RegHeaderFile.setType("HEADER")
    drvExtPhyDp83640RegHeaderFile.setOverwrite(True)
    drvExtPhyDp83640RegHeaderFile.setEnabled(True)

    # Add drv_extphy_dp83640.h file to project
    drvExtPhyDp83640HeaderFile = drvExtPhyDp83640Component.createFileSymbol(None, None)
    drvExtPhyDp83640HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_dp83640.h")
    drvExtPhyDp83640HeaderFile.setOutputName("drv_extphy_dp83640.h")
    drvExtPhyDp83640HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyDp83640HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyDp83640HeaderFile.setType("HEADER")
    drvExtPhyDp83640HeaderFile.setOverwrite(True)
    drvExtPhyDp83640HeaderFile.setEnabled(True)

    # file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
    # Add drv_ethphy.c file
    drvExtPhySourceFile = drvExtPhyDp83640Component.createFileSymbol(None, None)
    drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
    drvExtPhySourceFile.setOutputName("drv_ethphy.c")
    drvExtPhySourceFile.setOverwrite(True)
    drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setType("SOURCE")
    drvExtPhySourceFile.setEnabled(True)

    # Add drv_extphy_dp83640.c file
    drvExtPhyDp83640SourceFile = drvExtPhyDp83640Component.createFileSymbol(None, None)
    drvExtPhyDp83640SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_dp83640.c")
    drvExtPhyDp83640SourceFile.setOutputName("drv_extphy_dp83640.c")
    drvExtPhyDp83640SourceFile.setOverwrite(True)
    drvExtPhyDp83640SourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyDp83640SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyDp83640SourceFile.setType("SOURCE")
    drvExtPhyDp83640SourceFile.setEnabled(True)

    
def drvExtPhyDp83640MenuVisibleSingle(symbol, event):
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
