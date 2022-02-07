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


def instantiateComponent(drvExtPhyKsz9031Component):
    print("KSZ9031 PHY Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # PHY Address
    drvExtPhyKsz9031Addr= drvExtPhyKsz9031Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
    drvExtPhyKsz9031Addr.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9031Addr.setLabel("PHY Address") 
    drvExtPhyKsz9031Addr.setVisible(True)
    drvExtPhyKsz9031Addr.setDescription("PHY Address")
    drvExtPhyKsz9031Addr.setDefaultValue(1)
    
    # Use a Function to be called at PHY Reset
    drvExtPhyKsz9031ResetCallbackEnable = drvExtPhyKsz9031Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
    drvExtPhyKsz9031ResetCallbackEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9031ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
    drvExtPhyKsz9031ResetCallbackEnable.setVisible(True)
    drvExtPhyKsz9031ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
    drvExtPhyKsz9031ResetCallbackEnable.setDefaultValue(False)
    
    # App Function
    drvExtPhyKsz9031ResetCallback = drvExtPhyKsz9031Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyKsz9031ResetCallbackEnable)
    drvExtPhyKsz9031ResetCallback.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9031ResetCallback.setLabel("App Function")
    drvExtPhyKsz9031ResetCallback.setVisible(False)
    drvExtPhyKsz9031ResetCallback.setDescription("App Function")
    drvExtPhyKsz9031ResetCallback.setDefaultValue("AppPhyResetFunction")
    drvExtPhyKsz9031ResetCallback.setDependencies(drvExtPhyKsz9031MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])  
    
    # External PHY Connection Flags
    drvExtPhyKsz9031ConnFlag = drvExtPhyKsz9031Component.createMenuSymbol(None, None) 
    drvExtPhyKsz9031ConnFlag.setLabel("External PHY Connection Flags")
    drvExtPhyKsz9031ConnFlag.setVisible(True)
    drvExtPhyKsz9031ConnFlag.setDescription("External PHY Connection Flags")
    
    # GMII Data Interface
    drvExtPhyKsz9031ConfigGmii = drvExtPhyKsz9031Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_GMII", drvExtPhyKsz9031ConnFlag)
    drvExtPhyKsz9031ConfigGmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9031ConfigGmii.setLabel("GMII Data Interface")
    drvExtPhyKsz9031ConfigGmii.setVisible(True)
    drvExtPhyKsz9031ConfigGmii.setDefaultValue(False)
    drvExtPhyKsz9031ConfigGmii.setDescription("GMII Data Interface")
    
    # RMII Data Interface
    drvExtPhyKsz9031ConfigRmii = drvExtPhyKsz9031Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyKsz9031ConnFlag)
    drvExtPhyKsz9031ConfigRmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9031ConfigRmii.setLabel("RMII Data Interface")
    drvExtPhyKsz9031ConfigRmii.setVisible(True)
    drvExtPhyKsz9031ConfigRmii.setDescription("RMII Data Interface")
    
    if Peripheral.moduleExists("GMAC"):
        drvExtPhyKsz9031ConfigRmii.setDefaultValue(True)
    elif "PIC32CZ" in Variables.get("__PROCESSOR"):
        drvExtPhyKsz9031ConfigRmii.setDefaultValue(True)
    elif "PIC32M" in Variables.get("__PROCESSOR"):
        drvExtPhyKsz9031ConfigRmii.setDefaultValue(False)
    
        # Configuration Fuses Is ALT
        drvExtPhyKsz9031ConfigAlt = drvExtPhyKsz9031Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_ALTERNATE", drvExtPhyKsz9031ConnFlag)
        drvExtPhyKsz9031ConfigAlt.setHelp("mcc_h3_phy_configurations")
        drvExtPhyKsz9031ConfigAlt.setLabel("Configuration Fuses Is ALT")
        drvExtPhyKsz9031ConfigAlt.setVisible(True)
        drvExtPhyKsz9031ConfigAlt.setDescription("Configuration Fuses Is ALT")
        drvExtPhyKsz9031ConfigAlt.setDefaultValue(False)
        
        # Use The Fuses Configuration
        drvExtPhyKsz9031ConfigAuto = drvExtPhyKsz9031Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTO", drvExtPhyKsz9031ConnFlag)
        drvExtPhyKsz9031ConfigAuto.setHelp("mcc_h3_phy_configurations")
        drvExtPhyKsz9031ConfigAuto.setLabel("Use The Fuses Configuration")
        drvExtPhyKsz9031ConfigAuto.setVisible(True)
        drvExtPhyKsz9031ConfigAuto.setDescription("Use The Fuses Configuration")
        drvExtPhyKsz9031ConfigAuto.setDefaultValue(True)  
    
    # Advanced Settings
    drvExtPhyKsz9031AdvSettings = drvExtPhyKsz9031Component.createMenuSymbol("TCPIP_INTMAC_PHY_ADV_SETTING", None)
    drvExtPhyKsz9031AdvSettings.setLabel("Advanced Settings")
    drvExtPhyKsz9031AdvSettings.setDescription("Advanced Settings")
    drvExtPhyKsz9031AdvSettings.setVisible(True)

    # Delay for the Link Initialization in ms
    drvExtPhyKsz9031LinkInitDelay= drvExtPhyKsz9031Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", drvExtPhyKsz9031AdvSettings)
    drvExtPhyKsz9031LinkInitDelay.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9031LinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
    drvExtPhyKsz9031LinkInitDelay.setVisible(True)
    drvExtPhyKsz9031LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
    drvExtPhyKsz9031LinkInitDelay.setDefaultValue(500)
   
    # External PHY Type
    drvExtPhyKsz9031PhyType = drvExtPhyKsz9031Component.createStringSymbol("TCPIP_EMAC_PHY_TYPE", drvExtPhyKsz9031AdvSettings)
    drvExtPhyKsz9031PhyType.setVisible(False)   
    drvExtPhyKsz9031PhyType.setDefaultValue("KSZ9031")
    
    # Driver PHY Negotiation Time-out - ms
    drvExtPhyKsz9031NegInitTimeout= drvExtPhyKsz9031Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", drvExtPhyKsz9031AdvSettings)
    drvExtPhyKsz9031NegInitTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9031NegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
    drvExtPhyKsz9031NegInitTimeout.setVisible(True)
    drvExtPhyKsz9031NegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
    drvExtPhyKsz9031NegInitTimeout.setDefaultValue(1)
    
    # Driver PHY Negotiation Done Time-out - ms
    drvExtPhyKsz9031NegDoneTimeout= drvExtPhyKsz9031Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", drvExtPhyKsz9031AdvSettings)
    drvExtPhyKsz9031NegDoneTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9031NegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
    drvExtPhyKsz9031NegDoneTimeout.setVisible(True)
    drvExtPhyKsz9031NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
    drvExtPhyKsz9031NegDoneTimeout.setDefaultValue(2000)
    
    # Driver PHY Reset Clear Time-out - ms
    drvExtPhyKsz9031ResetClearTimeout= drvExtPhyKsz9031Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", drvExtPhyKsz9031AdvSettings)
    drvExtPhyKsz9031ResetClearTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9031ResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
    drvExtPhyKsz9031ResetClearTimeout.setVisible(True)
    drvExtPhyKsz9031ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
    drvExtPhyKsz9031ResetClearTimeout.setDefaultValue(500)
 

    # Driver PHY Instances Number
    drvExtPhyKsz9031InstanceNum= drvExtPhyKsz9031Component.createIntegerSymbol("DRV_ETHPHY_INSTANCES_NUMBER", drvExtPhyKsz9031AdvSettings)
    drvExtPhyKsz9031InstanceNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9031InstanceNum.setLabel("PHY Instances Number") 
    drvExtPhyKsz9031InstanceNum.setVisible(True)
    drvExtPhyKsz9031InstanceNum.setDescription("Driver PHY Instances Number")
    drvExtPhyKsz9031InstanceNum.setDefaultValue(1)
    drvExtPhyKsz9031InstanceNum.setReadOnly(True)
    
    # Driver PHY Clients Number
    drvExtPhyKsz9031ClientNum= drvExtPhyKsz9031Component.createIntegerSymbol("DRV_ETHPHY_CLIENTS_NUMBER", drvExtPhyKsz9031AdvSettings)
    drvExtPhyKsz9031ClientNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9031ClientNum.setLabel("PHY Clients Number") 
    drvExtPhyKsz9031ClientNum.setVisible(True)
    drvExtPhyKsz9031ClientNum.setDescription("Driver PHY Clients Number")
    drvExtPhyKsz9031ClientNum.setDefaultValue(1)
    drvExtPhyKsz9031ClientNum.setReadOnly(True)
    
    # Driver PHY Peripheral Index Number
    drvExtPhyKsz9031IndexNum= drvExtPhyKsz9031Component.createIntegerSymbol("DRV_ETHPHY_INDEX", drvExtPhyKsz9031AdvSettings)
    drvExtPhyKsz9031IndexNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9031IndexNum.setLabel("PHY Peripheral Index Number") 
    drvExtPhyKsz9031IndexNum.setVisible(True)
    drvExtPhyKsz9031IndexNum.setDescription("Driver PHY Peripheral Index Number")
    drvExtPhyKsz9031IndexNum.setDefaultValue(1)
    drvExtPhyKsz9031IndexNum.setReadOnly(True)
    
    # Driver PHY Peripheral ID
    drvExtPhyKsz9031PeripheralId= drvExtPhyKsz9031Component.createHexSymbol("DRV_ETHPHY_PERIPHERAL_ID", drvExtPhyKsz9031AdvSettings)
    drvExtPhyKsz9031PeripheralId.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz9031PeripheralId.setLabel("PHY Peripheral ID") 
    drvExtPhyKsz9031PeripheralId.setVisible(True)
    drvExtPhyKsz9031PeripheralId.setDescription("Driver PHY Peripheral ID")
    drvExtPhyKsz9031PeripheralId.setDefaultValue(0x1009031)
    drvExtPhyKsz9031PeripheralId.setReadOnly(True)

    # External MAC Name
    drvExtPhyMacName= drvExtPhyKsz9031Component.createStringSymbol("DRV_ETHPHY_MAC_NAME", None)
    drvExtPhyMacName.setLabel("Mac Name") 
    drvExtPhyMacName.setVisible(False)   
    drvExtPhyMacName.setDefaultValue("")
    drvExtPhyMacName.setReadOnly(True) 
                            
    #Add forward declaration to initialization.c
    drvExtPhyKsz9031InitDataSourceFtl = drvExtPhyKsz9031Component.createFileSymbol(None, None)
    drvExtPhyKsz9031InitDataSourceFtl.setType("STRING")
    drvExtPhyKsz9031InitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    drvExtPhyKsz9031InitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_driver_initialize.c.ftl")
    drvExtPhyKsz9031InitDataSourceFtl.setMarkup(True)    
    
    #Add to initialization.c
    drvExtPhyKsz9031SysInitDataSourceFtl = drvExtPhyKsz9031Component.createFileSymbol(None, None)
    drvExtPhyKsz9031SysInitDataSourceFtl.setType("STRING")
    drvExtPhyKsz9031SysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    drvExtPhyKsz9031SysInitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_data_initialize.c.ftl")
    drvExtPhyKsz9031SysInitDataSourceFtl.setMarkup(True)    
    
    #Add to system_config.h
    drvExtPhyKsz9031HeaderFtl = drvExtPhyKsz9031Component.createFileSymbol(None, None)
    drvExtPhyKsz9031HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_ksz9031.h.ftl")
    drvExtPhyKsz9031HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    drvExtPhyKsz9031HeaderFtl.setMarkup(True)
    drvExtPhyKsz9031HeaderFtl.setType("STRING")
        
    # file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
    # Add drv_ethphy.h file to project
    drvExtPhyHeaderFile = drvExtPhyKsz9031Component.createFileSymbol(None, None)
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
    drvExtPhyLocalHeaderFile = drvExtPhyKsz9031Component.createFileSymbol(None, None)
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
    drvExtPhyKsz9031RegHeaderFile = drvExtPhyKsz9031Component.createFileSymbol(None, None)
    drvExtPhyKsz9031RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
    drvExtPhyKsz9031RegHeaderFile.setOutputName("drv_extphy_regs.h")
    drvExtPhyKsz9031RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyKsz9031RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyKsz9031RegHeaderFile.setType("HEADER")
    drvExtPhyKsz9031RegHeaderFile.setOverwrite(True)
    drvExtPhyKsz9031RegHeaderFile.setEnabled(True)
    
    # Add drv_extphy_ksz9031.h file to project
    drvExtPhyKsz9031HeaderFile = drvExtPhyKsz9031Component.createFileSymbol(None, None)
    drvExtPhyKsz9031HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz9031.h")
    drvExtPhyKsz9031HeaderFile.setOutputName("drv_extphy_ksz9031.h")
    drvExtPhyKsz9031HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyKsz9031HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyKsz9031HeaderFile.setType("HEADER")
    drvExtPhyKsz9031HeaderFile.setOverwrite(True)
    drvExtPhyKsz9031HeaderFile.setEnabled(True)



    # file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
    # Add drv_ethphy.c file
    drvExtPhySourceFile = drvExtPhyKsz9031Component.createFileSymbol(None, None)
    drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
    drvExtPhySourceFile.setOutputName("drv_ethphy.c")
    drvExtPhySourceFile.setOverwrite(True)
    drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setType("SOURCE")
    drvExtPhySourceFile.setEnabled(True)

    # Add drv_extphy_ksz9031.c file
    drvExtPhyKsz9031SourceFile = drvExtPhyKsz9031Component.createFileSymbol(None, None)
    drvExtPhyKsz9031SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz9031.c")
    drvExtPhyKsz9031SourceFile.setOutputName("drv_extphy_ksz9031.c")
    drvExtPhyKsz9031SourceFile.setOverwrite(True)
    drvExtPhyKsz9031SourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyKsz9031SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyKsz9031SourceFile.setType("SOURCE")
    drvExtPhyKsz9031SourceFile.setEnabled(True)

    
def drvExtPhyKsz9031MenuVisibleSingle(symbol, event):
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
