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

def instantiateComponent(drvExtPhyKsz8863Component):
    print("KSZ8863 PHY Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")

    # PHY Address
    drvExtPhyKsz8863Addr= drvExtPhyKsz8863Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
    drvExtPhyKsz8863Addr.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8863Addr.setLabel("PHY Address") 
    drvExtPhyKsz8863Addr.setVisible(True)
    drvExtPhyKsz8863Addr.setDescription("PHY Address")
    drvExtPhyKsz8863Addr.setDefaultValue(1)

    # Use a Function to be called at PHY Reset
    drvExtPhyKsz8863ResetCallbackEnable = drvExtPhyKsz8863Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
    drvExtPhyKsz8863ResetCallbackEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8863ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
    drvExtPhyKsz8863ResetCallbackEnable.setVisible(True)
    drvExtPhyKsz8863ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
    drvExtPhyKsz8863ResetCallbackEnable.setDefaultValue(False)
    
    # App Function
    drvExtPhyKsz8863ResetCallback = drvExtPhyKsz8863Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyKsz8863ResetCallbackEnable)
    drvExtPhyKsz8863ResetCallback.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8863ResetCallback.setLabel("App Function")
    drvExtPhyKsz8863ResetCallback.setVisible(False)
    drvExtPhyKsz8863ResetCallback.setDescription("App Function")
    drvExtPhyKsz8863ResetCallback.setDefaultValue("AppPhyResetFunction")
    drvExtPhyKsz8863ResetCallback.setDependencies(drvExtPhyKsz8863MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])

    # External PHY Connection Flags
    drvExtPhyKsz8863ConnFlag = drvExtPhyKsz8863Component.createMenuSymbol(None, None) 
    drvExtPhyKsz8863ConnFlag.setLabel("External PHY Connection Flags")
    drvExtPhyKsz8863ConnFlag.setVisible(True)
    drvExtPhyKsz8863ConnFlag.setDescription("External PHY Connection Flags")
    
    # RMII Data Interface
    drvExtPhyKsz8863ConfigRmii = drvExtPhyKsz8863Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyKsz8863ConnFlag)
    drvExtPhyKsz8863ConfigRmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8863ConfigRmii.setLabel("RMII Data Interface")
    drvExtPhyKsz8863ConfigRmii.setVisible(True)
    drvExtPhyKsz8863ConfigRmii.setDescription("RMII Data Interface")
    
    if Peripheral.moduleExists("GMAC"):
        drvExtPhyKsz8863ConfigRmii.setDefaultValue(True)
    elif "PIC32M" in Variables.get("__PROCESSOR"):
        drvExtPhyKsz8863ConfigRmii.setDefaultValue(False)
    
        # Configuration Fuses Is ALT
        drvExtPhyKsz8863ConfigAlt = drvExtPhyKsz8863Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_ALTERNATE", drvExtPhyKsz8863ConnFlag)
        drvExtPhyKsz8863ConfigAlt.setHelp("mcc_h3_phy_configurations")
        drvExtPhyKsz8863ConfigAlt.setLabel("Configuration Fuses Is ALT")
        drvExtPhyKsz8863ConfigAlt.setVisible(True)
        drvExtPhyKsz8863ConfigAlt.setDescription("Configuration Fuses Is ALT")
        drvExtPhyKsz8863ConfigAlt.setDefaultValue(False)
        
        # Use The Fuses Configuration
        drvExtPhyKsz8863ConfigAuto = drvExtPhyKsz8863Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTO", drvExtPhyKsz8863ConnFlag)
        drvExtPhyKsz8863ConfigAuto.setHelp("mcc_h3_phy_configurations")
        drvExtPhyKsz8863ConfigAuto.setLabel("Use The Fuses Configuration")
        drvExtPhyKsz8863ConfigAuto.setVisible(True)
        drvExtPhyKsz8863ConfigAuto.setDescription("Use The Fuses Configuration")
        drvExtPhyKsz8863ConfigAuto.setDefaultValue(True)    
                
    # Advanced Settings
    drvExtPhyKsz8863AdvSettings = drvExtPhyKsz8863Component.createMenuSymbol("TCPIP_INTMAC_PHY_ADV_SETTING", None)
    drvExtPhyKsz8863AdvSettings.setLabel("Advanced Settings")
    drvExtPhyKsz8863AdvSettings.setDescription("Advanced Settings")
    drvExtPhyKsz8863AdvSettings.setVisible(True)    
    
    # Delay for the Link Initialization in ms
    drvExtPhyKsz8863LinkInitDelay= drvExtPhyKsz8863Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", drvExtPhyKsz8863AdvSettings)
    drvExtPhyKsz8863LinkInitDelay.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8863LinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
    drvExtPhyKsz8863LinkInitDelay.setVisible(True)
    drvExtPhyKsz8863LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
    drvExtPhyKsz8863LinkInitDelay.setDefaultValue(500)
    
    # External PHY Type
    drvExtPhyKsz8863PhyType = drvExtPhyKsz8863Component.createStringSymbol("TCPIP_EMAC_PHY_TYPE", drvExtPhyKsz8863AdvSettings)
    drvExtPhyKsz8863PhyType.setVisible(False)   
    drvExtPhyKsz8863PhyType.setDefaultValue("KSZ8863")
        
    # Driver PHY Negotiation Time-out - ms
    drvExtPhyKsz8863NegInitTimeout= drvExtPhyKsz8863Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", drvExtPhyKsz8863AdvSettings)
    drvExtPhyKsz8863NegInitTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8863NegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
    drvExtPhyKsz8863NegInitTimeout.setVisible(True)
    drvExtPhyKsz8863NegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
    drvExtPhyKsz8863NegInitTimeout.setDefaultValue(1)
    
    # Driver PHY Negotiation Done Time-out - ms
    drvExtPhyKsz8863NegDoneTimeout= drvExtPhyKsz8863Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", drvExtPhyKsz8863AdvSettings)
    drvExtPhyKsz8863NegDoneTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8863NegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
    drvExtPhyKsz8863NegDoneTimeout.setVisible(True)
    drvExtPhyKsz8863NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
    drvExtPhyKsz8863NegDoneTimeout.setDefaultValue(2000)
    
    # Driver PHY Reset Clear Time-out - ms
    drvExtPhyKsz8863ResetClearTimeout= drvExtPhyKsz8863Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", drvExtPhyKsz8863AdvSettings)
    drvExtPhyKsz8863ResetClearTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8863ResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
    drvExtPhyKsz8863ResetClearTimeout.setVisible(True)
    drvExtPhyKsz8863ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
    drvExtPhyKsz8863ResetClearTimeout.setDefaultValue(500)
    
    # Driver PHY Instances Number
    drvExtPhyKsz8863InstanceNum= drvExtPhyKsz8863Component.createIntegerSymbol("DRV_ETHPHY_INSTANCES_NUMBER", drvExtPhyKsz8863AdvSettings)
    drvExtPhyKsz8863InstanceNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8863InstanceNum.setLabel("PHY Instances Number") 
    drvExtPhyKsz8863InstanceNum.setVisible(True)
    drvExtPhyKsz8863InstanceNum.setDescription("Driver PHY Instances Number")
    drvExtPhyKsz8863InstanceNum.setDefaultValue(1)
    drvExtPhyKsz8863InstanceNum.setReadOnly(True)
    
    # Driver PHY Clients Number
    drvExtPhyKsz8863ClientNum= drvExtPhyKsz8863Component.createIntegerSymbol("DRV_ETHPHY_CLIENTS_NUMBER", drvExtPhyKsz8863AdvSettings)
    drvExtPhyKsz8863ClientNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8863ClientNum.setLabel("PHY Clients Number") 
    drvExtPhyKsz8863ClientNum.setVisible(True)
    drvExtPhyKsz8863ClientNum.setDescription("Driver PHY Clients Number")
    drvExtPhyKsz8863ClientNum.setDefaultValue(1)
    drvExtPhyKsz8863ClientNum.setReadOnly(True)
    
    # Driver PHY Peripheral Index Number
    drvExtPhyKsz8863IndexNum= drvExtPhyKsz8863Component.createIntegerSymbol("DRV_ETHPHY_INDEX", drvExtPhyKsz8863AdvSettings)
    drvExtPhyKsz8863IndexNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8863IndexNum.setLabel("PHY Peripheral Index Number") 
    drvExtPhyKsz8863IndexNum.setVisible(True)
    drvExtPhyKsz8863IndexNum.setDescription("Driver PHY Peripheral Index Number")
    drvExtPhyKsz8863IndexNum.setDefaultValue(1)
    drvExtPhyKsz8863IndexNum.setReadOnly(True)
    
    # Driver PHY Peripheral ID
    drvExtPhyKsz8863PeripheralId= drvExtPhyKsz8863Component.createStringSymbol("DRV_ETHPHY_PERIPHERAL_ID", drvExtPhyKsz8863AdvSettings)
    drvExtPhyKsz8863PeripheralId.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8863PeripheralId.setLabel("PHY Peripheral ID") 
    drvExtPhyKsz8863PeripheralId.setVisible(True)
    drvExtPhyKsz8863PeripheralId.setDescription("Driver PHY Peripheral ID")
    drvExtPhyKsz8863PeripheralId.setDefaultValue("")
    drvExtPhyKsz8863PeripheralId.setReadOnly(True)

    # External MAC Name
    drvExtPhyMacName= drvExtPhyKsz8863Component.createStringSymbol("DRV_ETHPHY_MAC_NAME", None)
    drvExtPhyMacName.setLabel("Mac Name") 
    drvExtPhyMacName.setVisible(False)   
    drvExtPhyMacName.setDefaultValue("")
    drvExtPhyMacName.setReadOnly(True) 
        
    #Add forward declaration to initialization.c
    drvExtPhyKsz8863InitDataSourceFtl = drvExtPhyKsz8863Component.createFileSymbol(None, None)
    drvExtPhyKsz8863InitDataSourceFtl.setType("STRING")
    drvExtPhyKsz8863InitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    drvExtPhyKsz8863InitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_driver_initialize.c.ftl")
    drvExtPhyKsz8863InitDataSourceFtl.setMarkup(True)    
    
    #Add to initialization.c
    drvExtPhyKsz8863SysInitDataSourceFtl = drvExtPhyKsz8863Component.createFileSymbol(None, None)
    drvExtPhyKsz8863SysInitDataSourceFtl.setType("STRING")
    drvExtPhyKsz8863SysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    drvExtPhyKsz8863SysInitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_data_initialize.c.ftl")
    drvExtPhyKsz8863SysInitDataSourceFtl.setMarkup(True)    
        
    #Add to system_config.h
    drvExtPhyKsz8863HeaderFtl = drvExtPhyKsz8863Component.createFileSymbol(None, None)
    drvExtPhyKsz8863HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_ksz8863.h.ftl")
    drvExtPhyKsz8863HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    drvExtPhyKsz8863HeaderFtl.setMarkup(True)
    drvExtPhyKsz8863HeaderFtl.setType("STRING")
    
    # file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
    # Add drv_ethphy.h file to project
    drvExtPhyHeaderFile = drvExtPhyKsz8863Component.createFileSymbol(None, None)
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
    drvExtPhyLocalHeaderFile = drvExtPhyKsz8863Component.createFileSymbol(None, None)
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
    drvExtPhyKsz8863RegHeaderFile = drvExtPhyKsz8863Component.createFileSymbol(None, None)
    drvExtPhyKsz8863RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
    drvExtPhyKsz8863RegHeaderFile.setOutputName("drv_extphy_regs.h")
    drvExtPhyKsz8863RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyKsz8863RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyKsz8863RegHeaderFile.setType("HEADER")
    drvExtPhyKsz8863RegHeaderFile.setOverwrite(True)
    drvExtPhyKsz8863RegHeaderFile.setEnabled(True)

    #Add drv_extphy_ksz8863.h file to project
    drvExtPhyKsz8863HeaderFile = drvExtPhyKsz8863Component.createFileSymbol(None, None)
    drvExtPhyKsz8863HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz8863.h")
    drvExtPhyKsz8863HeaderFile.setOutputName("drv_extphy_ksz8863.h")
    drvExtPhyKsz8863HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyKsz8863HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyKsz8863HeaderFile.setType("HEADER")
    drvExtPhyKsz8863HeaderFile.setOverwrite(True)
    drvExtPhyKsz8863HeaderFile.setEnabled(True)

    # file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
    # Add drv_ethphy.c file
    drvExtPhySourceFile = drvExtPhyKsz8863Component.createFileSymbol(None, None)
    drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
    drvExtPhySourceFile.setOutputName("drv_ethphy.c")
    drvExtPhySourceFile.setOverwrite(True)
    drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setType("SOURCE")
    drvExtPhySourceFile.setEnabled(True)

    # Add drv_extphy_ksz8863.c file
    drvExtPhyKsz8863SourceFile = drvExtPhyKsz8863Component.createFileSymbol(None, None)
    drvExtPhyKsz8863SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz8863.c")
    drvExtPhyKsz8863SourceFile.setOutputName("drv_extphy_ksz8863.c")
    drvExtPhyKsz8863SourceFile.setOverwrite(True)
    drvExtPhyKsz8863SourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyKsz8863SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyKsz8863SourceFile.setType("SOURCE")
    drvExtPhyKsz8863SourceFile.setEnabled(True)

    
def drvExtPhyKsz8863MenuVisibleSingle(symbol, event):
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
