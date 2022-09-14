"""*****************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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

def instantiateComponent(drvExtPhyKsz8041Component):
    print("KSZ8041 PHY Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")

    # PHY Address
    drvExtPhyKsz8041Addr= drvExtPhyKsz8041Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
    drvExtPhyKsz8041Addr.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8041Addr.setLabel("PHY Address") 
    drvExtPhyKsz8041Addr.setVisible(True)
    drvExtPhyKsz8041Addr.setDescription("PHY Address")
    drvExtPhyKsz8041Addr.setDefaultValue(0)
    
    # Use a Function to be called at PHY Reset
    drvExtPhyKsz8041ResetCallbackEnable = drvExtPhyKsz8041Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
    drvExtPhyKsz8041ResetCallbackEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8041ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
    drvExtPhyKsz8041ResetCallbackEnable.setVisible(True)
    drvExtPhyKsz8041ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
    drvExtPhyKsz8041ResetCallbackEnable.setDefaultValue(False)
    
    # App Function
    drvExtPhyKsz8041ResetCallback = drvExtPhyKsz8041Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyKsz8041ResetCallbackEnable)
    drvExtPhyKsz8041ResetCallback.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8041ResetCallback.setLabel("App Function")
    drvExtPhyKsz8041ResetCallback.setVisible(False)
    drvExtPhyKsz8041ResetCallback.setDescription("App Function")
    drvExtPhyKsz8041ResetCallback.setDefaultValue("AppPhyResetFunction")
    drvExtPhyKsz8041ResetCallback.setDependencies(drvExtPhyKsz8041MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])


    # External PHY Connection Flags
    drvExtPhyKsz8041ConnFlag = drvExtPhyKsz8041Component.createMenuSymbol(None, None) 
    drvExtPhyKsz8041ConnFlag.setLabel("External PHY Connection Flags")
    drvExtPhyKsz8041ConnFlag.setVisible(True)
    drvExtPhyKsz8041ConnFlag.setDescription("External PHY Connection Flags")
    
    # RMII Data Interface
    drvExtPhyKsz8041ConfigRmii = drvExtPhyKsz8041Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyKsz8041ConnFlag)
    drvExtPhyKsz8041ConfigRmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8041ConfigRmii.setLabel("RMII Data Interface")
    drvExtPhyKsz8041ConfigRmii.setVisible(True)
    drvExtPhyKsz8041ConfigRmii.setDescription("RMII Data Interface")
    
    if Peripheral.moduleExists("GMAC"):
        drvExtPhyKsz8041ConfigRmii.setDefaultValue(True)
    elif "PIC32M" in Variables.get("__PROCESSOR"):
        drvExtPhyKsz8041ConfigRmii.setDefaultValue(False)
    
        # Configuration Fuses Is ALT
        drvExtPhyKsz8041ConfigAlt = drvExtPhyKsz8041Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_ALTERNATE", drvExtPhyKsz8041ConnFlag)
        drvExtPhyKsz8041ConfigAlt.setHelp("mcc_h3_phy_configurations")
        drvExtPhyKsz8041ConfigAlt.setLabel("Configuration Fuses Is ALT")
        drvExtPhyKsz8041ConfigAlt.setVisible(True)
        drvExtPhyKsz8041ConfigAlt.setDescription("Configuration Fuses Is ALT")
        drvExtPhyKsz8041ConfigAlt.setDefaultValue(False)
        
        # Use The Fuses Configuration
        drvExtPhyKsz8041ConfigAuto = drvExtPhyKsz8041Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTO", drvExtPhyKsz8041ConnFlag)
        drvExtPhyKsz8041ConfigAuto.setHelp("mcc_h3_phy_configurations")
        drvExtPhyKsz8041ConfigAuto.setLabel("Use The Fuses Configuration")
        drvExtPhyKsz8041ConfigAuto.setVisible(True)
        drvExtPhyKsz8041ConfigAuto.setDescription("Use The Fuses Configuration")
        drvExtPhyKsz8041ConfigAuto.setDefaultValue(True)    
    
        
    # Advanced Settings
    drvExtPhyKsz8041AdvSettings = drvExtPhyKsz8041Component.createMenuSymbol("TCPIP_INTMAC_PHY_ADV_SETTING", None)
    drvExtPhyKsz8041AdvSettings.setLabel("Advanced Settings")
    drvExtPhyKsz8041AdvSettings.setDescription("Advanced Settings")
    drvExtPhyKsz8041AdvSettings.setVisible(True)
    
    # Delay for the Link Initialization in ms
    drvExtPhyKsz8041LinkInitDelay= drvExtPhyKsz8041Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", drvExtPhyKsz8041AdvSettings)
    drvExtPhyKsz8041LinkInitDelay.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8041LinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
    drvExtPhyKsz8041LinkInitDelay.setVisible(True)
    drvExtPhyKsz8041LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
    drvExtPhyKsz8041LinkInitDelay.setDefaultValue(500)
    
    # External PHY Type
    drvExtPhyKsz8041PhyType = drvExtPhyKsz8041Component.createStringSymbol("TCPIP_EMAC_PHY_TYPE", drvExtPhyKsz8041AdvSettings)
    drvExtPhyKsz8041PhyType.setVisible(False)   
    drvExtPhyKsz8041PhyType.setDefaultValue("KSZ8041")
    
    # Driver PHY Negotiation Time-out - ms
    drvExtPhyKsz8041NegInitTimeout= drvExtPhyKsz8041Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", drvExtPhyKsz8041AdvSettings)
    drvExtPhyKsz8041NegInitTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8041NegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
    drvExtPhyKsz8041NegInitTimeout.setVisible(True)
    drvExtPhyKsz8041NegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
    drvExtPhyKsz8041NegInitTimeout.setDefaultValue(1)
    
    # Driver PHY Negotiation Done Time-out - ms
    drvExtPhyKsz8041NegDoneTimeout= drvExtPhyKsz8041Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", drvExtPhyKsz8041AdvSettings)
    drvExtPhyKsz8041NegDoneTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8041NegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
    drvExtPhyKsz8041NegDoneTimeout.setVisible(True)
    drvExtPhyKsz8041NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
    drvExtPhyKsz8041NegDoneTimeout.setDefaultValue(2000)
    
    # Driver PHY Reset Clear Time-out - ms
    drvExtPhyKsz8041ResetClearTimeout= drvExtPhyKsz8041Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", drvExtPhyKsz8041AdvSettings)
    drvExtPhyKsz8041ResetClearTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8041ResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
    drvExtPhyKsz8041ResetClearTimeout.setVisible(True)
    drvExtPhyKsz8041ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
    drvExtPhyKsz8041ResetClearTimeout.setDefaultValue(500)
    
    # Driver PHY Instances Number
    drvExtPhyKsz8041InstanceNum= drvExtPhyKsz8041Component.createIntegerSymbol("DRV_ETHPHY_INSTANCES_NUMBER", drvExtPhyKsz8041AdvSettings)
    drvExtPhyKsz8041InstanceNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8041InstanceNum.setLabel("PHY Instances Number") 
    drvExtPhyKsz8041InstanceNum.setVisible(True)
    drvExtPhyKsz8041InstanceNum.setDescription("Driver PHY Instances Number")
    drvExtPhyKsz8041InstanceNum.setDefaultValue(1)
    drvExtPhyKsz8041InstanceNum.setReadOnly(True)
    
    # Driver PHY Clients Number
    drvExtPhyKsz8041ClientNum= drvExtPhyKsz8041Component.createIntegerSymbol("DRV_ETHPHY_CLIENTS_NUMBER", drvExtPhyKsz8041AdvSettings)
    drvExtPhyKsz8041ClientNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8041ClientNum.setLabel("PHY Clients Number") 
    drvExtPhyKsz8041ClientNum.setVisible(True)
    drvExtPhyKsz8041ClientNum.setDescription("Driver PHY Clients Number")
    drvExtPhyKsz8041ClientNum.setDefaultValue(1)
    drvExtPhyKsz8041ClientNum.setReadOnly(True)
    
    # Driver PHY Peripheral Index Number
    drvExtPhyKsz8041IndexNum= drvExtPhyKsz8041Component.createIntegerSymbol("DRV_ETHPHY_INDEX", drvExtPhyKsz8041AdvSettings)
    drvExtPhyKsz8041IndexNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8041IndexNum.setLabel("PHY Peripheral Index Number") 
    drvExtPhyKsz8041IndexNum.setVisible(True)
    drvExtPhyKsz8041IndexNum.setDescription("Driver PHY Peripheral Index Number")
    drvExtPhyKsz8041IndexNum.setDefaultValue(1)
    drvExtPhyKsz8041IndexNum.setReadOnly(True)
    
    # Driver PHY Peripheral ID
    drvExtPhyKsz8041PeripheralId= drvExtPhyKsz8041Component.createStringSymbol("DRV_ETHPHY_PERIPHERAL_ID", drvExtPhyKsz8041AdvSettings)
    drvExtPhyKsz8041PeripheralId.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8041PeripheralId.setLabel("PHY Peripheral ID") 
    drvExtPhyKsz8041PeripheralId.setVisible(True)
    drvExtPhyKsz8041PeripheralId.setDescription("Driver PHY Peripheral ID")
    drvExtPhyKsz8041PeripheralId.setDefaultValue("")
    drvExtPhyKsz8041PeripheralId.setReadOnly(True)

    # External MAC Name
    drvExtPhyMacName= drvExtPhyKsz8041Component.createStringSymbol("DRV_ETHPHY_MAC_NAME", None)
    drvExtPhyMacName.setLabel("Mac Name") 
    drvExtPhyMacName.setVisible(False)   
    drvExtPhyMacName.setDefaultValue("")
    drvExtPhyMacName.setReadOnly(True) 
                                
    #Add forward declaration to initialization.c
    drvExtPhyKsz8041InitDataSourceFtl = drvExtPhyKsz8041Component.createFileSymbol(None, None)
    drvExtPhyKsz8041InitDataSourceFtl.setType("STRING")
    drvExtPhyKsz8041InitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    drvExtPhyKsz8041InitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_driver_initialize.c.ftl")
    drvExtPhyKsz8041InitDataSourceFtl.setMarkup(True)    
    
    #Add to initialization.c
    drvExtPhyKsz8041SysInitDataSourceFtl = drvExtPhyKsz8041Component.createFileSymbol(None, None)
    drvExtPhyKsz8041SysInitDataSourceFtl.setType("STRING")
    drvExtPhyKsz8041SysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    drvExtPhyKsz8041SysInitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_data_initialize.c.ftl")
    drvExtPhyKsz8041SysInitDataSourceFtl.setMarkup(True)    
    
    #Add to system_config.h
    drvExtPhyKsz8041HeaderFtl = drvExtPhyKsz8041Component.createFileSymbol(None, None)
    drvExtPhyKsz8041HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_ksz8041.h.ftl")
    drvExtPhyKsz8041HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    drvExtPhyKsz8041HeaderFtl.setMarkup(True)
    drvExtPhyKsz8041HeaderFtl.setType("STRING")
    
    # file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
    # Add drv_ethphy.h file to project
    drvExtPhyHeaderFile = drvExtPhyKsz8041Component.createFileSymbol(None, None)
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
    drvExtPhyLocalHeaderFile = drvExtPhyKsz8041Component.createFileSymbol(None, None)
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
    drvExtPhyKsz8041RegHeaderFile = drvExtPhyKsz8041Component.createFileSymbol(None, None)
    drvExtPhyKsz8041RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
    drvExtPhyKsz8041RegHeaderFile.setOutputName("drv_extphy_regs.h")
    drvExtPhyKsz8041RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyKsz8041RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyKsz8041RegHeaderFile.setType("HEADER")
    drvExtPhyKsz8041RegHeaderFile.setOverwrite(True)
    drvExtPhyKsz8041RegHeaderFile.setEnabled(True)

    #Add drv_extphy_ksz8041.h file to project
    drvExtPhyKsz8041HeaderFile = drvExtPhyKsz8041Component.createFileSymbol(None, None)
    drvExtPhyKsz8041HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz8041.h")
    drvExtPhyKsz8041HeaderFile.setOutputName("drv_extphy_ksz8041.h")
    drvExtPhyKsz8041HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyKsz8041HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyKsz8041HeaderFile.setType("HEADER")
    drvExtPhyKsz8041HeaderFile.setOverwrite(True)
    drvExtPhyKsz8041HeaderFile.setEnabled(True)

    # file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
    # Add drv_ethphy.c file
    drvExtPhySourceFile = drvExtPhyKsz8041Component.createFileSymbol(None, None)
    drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
    drvExtPhySourceFile.setOutputName("drv_ethphy.c")
    drvExtPhySourceFile.setOverwrite(True)
    drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setType("SOURCE")
    drvExtPhySourceFile.setEnabled(True)

    # Add drv_extphy_ksz8041.c file
    drvExtPhyKsz8041SourceFile = drvExtPhyKsz8041Component.createFileSymbol(None, None)
    drvExtPhyKsz8041SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz8041.c")
    drvExtPhyKsz8041SourceFile.setOutputName("drv_extphy_ksz8041.c")
    drvExtPhyKsz8041SourceFile.setOverwrite(True)
    drvExtPhyKsz8041SourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyKsz8041SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyKsz8041SourceFile.setType("SOURCE")
    drvExtPhyKsz8041SourceFile.setEnabled(True)

    
def drvExtPhyKsz8041MenuVisibleSingle(symbol, event):
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
