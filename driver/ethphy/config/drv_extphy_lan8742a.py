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

def instantiateComponent(drvExtPhyLan8742aComponent):
    print("LAN8742a PHY Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")

    # PHY Address
    drvExtPhyLan8742aAddr= drvExtPhyLan8742aComponent.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
    drvExtPhyLan8742aAddr.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8742aAddr.setLabel("PHY Address") 
    drvExtPhyLan8742aAddr.setVisible(True)
    drvExtPhyLan8742aAddr.setDescription("PHY Address")
    drvExtPhyLan8742aAddr.setDefaultValue(0)
    
    # Use a Function to be called at PHY Reset
    drvExtPhyLan8742aResetCallbackEnable = drvExtPhyLan8742aComponent.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
    drvExtPhyLan8742aResetCallbackEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8742aResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
    drvExtPhyLan8742aResetCallbackEnable.setVisible(True)
    drvExtPhyLan8742aResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
    drvExtPhyLan8742aResetCallbackEnable.setDefaultValue(False)
    
    # App Function
    drvExtPhyLan8742aResetCallback = drvExtPhyLan8742aComponent.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyLan8742aResetCallbackEnable)
    drvExtPhyLan8742aResetCallback.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8742aResetCallback.setLabel("App Function")
    drvExtPhyLan8742aResetCallback.setVisible(False)
    drvExtPhyLan8742aResetCallback.setDescription("App Function")
    drvExtPhyLan8742aResetCallback.setDefaultValue("AppPhyResetFunction")
    drvExtPhyLan8742aResetCallback.setDependencies(drvExtPhyLan8742aMenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])
    
    # External PHY Connection Flags
    drvExtPhyLan8742aConnFlag = drvExtPhyLan8742aComponent.createMenuSymbol(None, None) 
    drvExtPhyLan8742aConnFlag.setLabel("External PHY Connection Flags")
    drvExtPhyLan8742aConnFlag.setVisible(True)
    drvExtPhyLan8742aConnFlag.setDescription("External PHY Connection Flags")
    
    # RMII Data Interface
    drvExtPhyLan8742aConfigRmii = drvExtPhyLan8742aComponent.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyLan8742aConnFlag)
    drvExtPhyLan8742aConfigRmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8742aConfigRmii.setLabel("RMII Data Interface")
    drvExtPhyLan8742aConfigRmii.setDefaultValue(True)
    drvExtPhyLan8742aConfigRmii.setVisible(True)
    drvExtPhyLan8742aConfigRmii.setDescription("RMII Data Interface")
    drvExtPhyLan8742aConfigRmii.setReadOnly(True)
    
        # Configuration Fuses Is ALT
        drvExtPhyLan8742aConfigAlt = drvExtPhyLan8742aComponent.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_ALTERNATE", drvExtPhyLan8742aConnFlag)
        drvExtPhyLan8742aConfigAlt.setHelp("mcc_h3_phy_configurations")
        drvExtPhyLan8742aConfigAlt.setLabel("Configuration Fuses Is ALT")
        drvExtPhyLan8742aConfigAlt.setVisible(True)
        drvExtPhyLan8742aConfigAlt.setDescription("Configuration Fuses Is ALT")
        drvExtPhyLan8742aConfigAlt.setDefaultValue(False)
        
        # Use The Fuses Configuration
        drvExtPhyLan8742aConfigAuto = drvExtPhyLan8742aComponent.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTO", drvExtPhyLan8742aConnFlag)
        drvExtPhyLan8742aConfigAuto.setHelp("mcc_h3_phy_configurations")
        drvExtPhyLan8742aConfigAuto.setLabel("Use The Fuses Configuration")
        drvExtPhyLan8742aConfigAuto.setVisible(True)
        drvExtPhyLan8742aConfigAuto.setDescription("Use The Fuses Configuration")
        drvExtPhyLan8742aConfigAuto.setDefaultValue(True)
    
    # Advanced Settings
    drvExtPhyLan8742aAdvSettings = drvExtPhyLan8742aComponent.createMenuSymbol("TCPIP_INTMAC_PHY_ADV_SETTING", None)
    drvExtPhyLan8742aAdvSettings.setLabel("Advanced Settings")
    drvExtPhyLan8742aAdvSettings.setDescription("Advanced Settings")
    drvExtPhyLan8742aAdvSettings.setVisible(True)

    # Delay for the Link Initialization in ms
    drvExtPhyLan8742aLinkInitDelay= drvExtPhyLan8742aComponent.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", drvExtPhyLan8742aAdvSettings)
    drvExtPhyLan8742aLinkInitDelay.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8742aLinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
    drvExtPhyLan8742aLinkInitDelay.setVisible(True)
    drvExtPhyLan8742aLinkInitDelay.setDescription("Delay for the Link Initialization in ms")
    drvExtPhyLan8742aLinkInitDelay.setDefaultValue(500)
      
    # External PHY Type
    drvExtPhyLan8742aPhyType = drvExtPhyLan8742aComponent.createStringSymbol("TCPIP_EMAC_PHY_TYPE", drvExtPhyLan8742aAdvSettings)
    #drvExtPhyLan8742aPhyType.setLabel("External PHY Type")
    drvExtPhyLan8742aPhyType.setVisible(False)
    #drvExtPhyLan8742aPhyType.setDescription("External PHY Type")
    drvExtPhyLan8742aPhyType.setDefaultValue("LAN8742a")
    
    # Driver PHY Negotiation Time-out - ms
    drvExtPhyLan8742aNegInitTimeout= drvExtPhyLan8742aComponent.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", drvExtPhyLan8742aAdvSettings)
    drvExtPhyLan8742aNegInitTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8742aNegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
    drvExtPhyLan8742aNegInitTimeout.setVisible(True)
    drvExtPhyLan8742aNegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
    drvExtPhyLan8742aNegInitTimeout.setDefaultValue(1)
    
    # Driver PHY Negotiation Done Time-out - ms
    drvExtPhyLan8742aNegDoneTimeout= drvExtPhyLan8742aComponent.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", drvExtPhyLan8742aAdvSettings)
    drvExtPhyLan8742aNegDoneTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8742aNegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
    drvExtPhyLan8742aNegDoneTimeout.setVisible(True)
    drvExtPhyLan8742aNegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
    drvExtPhyLan8742aNegDoneTimeout.setDefaultValue(2000)
    
    # Driver PHY Reset Clear Time-out - ms
    drvExtPhyLan8742aResetClearTimeout= drvExtPhyLan8742aComponent.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", drvExtPhyLan8742aAdvSettings)
    drvExtPhyLan8742aResetClearTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8742aResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
    drvExtPhyLan8742aResetClearTimeout.setVisible(True)
    drvExtPhyLan8742aResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
    drvExtPhyLan8742aResetClearTimeout.setDefaultValue(500)
    
    # Driver PHY Instances Number
    drvExtPhyLan8742aInstanceNum= drvExtPhyLan8742aComponent.createIntegerSymbol("DRV_ETHPHY_INSTANCES_NUMBER", drvExtPhyLan8742aAdvSettings)
    drvExtPhyLan8742aInstanceNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8742aInstanceNum.setLabel("PHY Instances Number") 
    drvExtPhyLan8742aInstanceNum.setVisible(True)
    drvExtPhyLan8742aInstanceNum.setDescription("Driver PHY Instances Number")
    drvExtPhyLan8742aInstanceNum.setDefaultValue(1)
    drvExtPhyLan8742aInstanceNum.setReadOnly(True)
    
    # Driver PHY Clients Number
    drvExtPhyLan8742aClientNum= drvExtPhyLan8742aComponent.createIntegerSymbol("DRV_ETHPHY_CLIENTS_NUMBER", drvExtPhyLan8742aAdvSettings)
    drvExtPhyLan8742aClientNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8742aClientNum.setLabel("PHY Clients Number") 
    drvExtPhyLan8742aClientNum.setVisible(True)
    drvExtPhyLan8742aClientNum.setDescription("Driver PHY Clients Number")
    drvExtPhyLan8742aClientNum.setDefaultValue(1)
    drvExtPhyLan8742aClientNum.setReadOnly(True)
    
    # Driver PHY Peripheral Index Number
    drvExtPhyLan8742aIndexNum= drvExtPhyLan8742aComponent.createIntegerSymbol("DRV_ETHPHY_INDEX", drvExtPhyLan8742aAdvSettings)
    drvExtPhyLan8742aIndexNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8742aIndexNum.setLabel("PHY Peripheral Index Number") 
    drvExtPhyLan8742aIndexNum.setVisible(True)
    drvExtPhyLan8742aIndexNum.setDescription("Driver PHY Peripheral Index Number")
    drvExtPhyLan8742aIndexNum.setDefaultValue(1)
    drvExtPhyLan8742aIndexNum.setReadOnly(True)
    
    # Driver PHY Peripheral ID
    drvExtPhyLan8742aPeripheralId= drvExtPhyLan8742aComponent.createIntegerSymbol("DRV_ETHPHY_PERIPHERAL_ID", drvExtPhyLan8742aAdvSettings)
    drvExtPhyLan8742aPeripheralId.setHelp("mcc_h3_phy_configurations")
    drvExtPhyLan8742aPeripheralId.setLabel("PHY Peripheral ID") 
    drvExtPhyLan8742aPeripheralId.setVisible(True)
    drvExtPhyLan8742aPeripheralId.setDescription("Driver PHY Peripheral ID")
    drvExtPhyLan8742aPeripheralId.setDefaultValue(1)
    drvExtPhyLan8742aPeripheralId.setReadOnly(True)

    # External MAC Name
    drvExtPhyMacName= drvExtPhyLan8742aComponent.createStringSymbol("DRV_ETHPHY_MAC_NAME", None)
    drvExtPhyMacName.setLabel("Mac Name") 
    drvExtPhyMacName.setVisible(False)   
    drvExtPhyMacName.setDefaultValue("")
    drvExtPhyMacName.setReadOnly(True) 
                
    #Add forward declaration to initialization.c
    drvExtPhyLan8742aInitDataSourceFtl = drvExtPhyLan8742aComponent.createFileSymbol(None, None)
    drvExtPhyLan8742aInitDataSourceFtl.setType("STRING")
    drvExtPhyLan8742aInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    drvExtPhyLan8742aInitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_driver_initialize.c.ftl")
    drvExtPhyLan8742aInitDataSourceFtl.setMarkup(True)    
    
    #Add to initialization.c
    drvExtPhyLan8742aSysInitDataSourceFtl = drvExtPhyLan8742aComponent.createFileSymbol(None, None)
    drvExtPhyLan8742aSysInitDataSourceFtl.setType("STRING")
    drvExtPhyLan8742aSysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    drvExtPhyLan8742aSysInitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_data_initialize.c.ftl")
    drvExtPhyLan8742aSysInitDataSourceFtl.setMarkup(True)    
    
    #Add to system_config.h
    drvExtPhyLan8742aHeaderFtl = drvExtPhyLan8742aComponent.createFileSymbol(None, None)
    drvExtPhyLan8742aHeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_lan8742a.h.ftl")
    drvExtPhyLan8742aHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    drvExtPhyLan8742aHeaderFtl.setMarkup(True)
    drvExtPhyLan8742aHeaderFtl.setType("STRING") 
    
    # file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
    # Add drv_ethphy.h file to project
    drvExtPhyHeaderFile = drvExtPhyLan8742aComponent.createFileSymbol(None, None)
    drvExtPhyHeaderFile.setSourcePath("driver/ethphy/drv_ethphy.h")
    drvExtPhyHeaderFile.setOutputName("drv_ethphy.h")
    drvExtPhyHeaderFile.setDestPath("driver/ethphy/")
    drvExtPhyHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/")
    drvExtPhyHeaderFile.setType("HEADER")
    drvExtPhyHeaderFile.setOverwrite(True)
    drvExtPhyHeaderFile.setEnabled(True)

    # file TCPIP_ETH_PHY_LOCAL_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/drv_ethphy_local.h" to           "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/drv_ethphy_local.h"
    # Add drv_ethphy_local.h file to project
    drvExtPhyLocalHeaderFile = drvExtPhyLan8742aComponent.createFileSymbol(None, None)
    drvExtPhyLocalHeaderFile.setSourcePath("driver/ethphy/src/drv_ethphy_local.h")
    drvExtPhyLocalHeaderFile.setOutputName("drv_ethphy_local.h")
    drvExtPhyLocalHeaderFile.setDestPath("driver/ethphy/src/")
    drvExtPhyLocalHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/")
    drvExtPhyLocalHeaderFile.setType("HEADER")
    drvExtPhyLocalHeaderFile.setOverwrite(True)
    drvExtPhyLocalHeaderFile.setEnabled(True)

    # file TCPIP_ETH_EXT_PHY_REGS_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h" to    "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h"
    # Add drv_extphy_regs.h file to project
    drvExtPhyLan8742aRegHeaderFile = drvExtPhyLan8742aComponent.createFileSymbol(None, None)
    drvExtPhyLan8742aRegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
    drvExtPhyLan8742aRegHeaderFile.setOutputName("drv_extphy_regs.h")
    drvExtPhyLan8742aRegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyLan8742aRegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyLan8742aRegHeaderFile.setType("HEADER")
    drvExtPhyLan8742aRegHeaderFile.setOverwrite(True)
    drvExtPhyLan8742aRegHeaderFile.setEnabled(True)

    # file TCPIP_MAC_PHY_LAN8742a_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_lan8742a.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_lan8742a.h"
    # Add drv_extphy_lan8742a.h file to project
    drvExtPhyLan8742aHeaderFile = drvExtPhyLan8742aComponent.createFileSymbol(None, None)
    drvExtPhyLan8742aHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_lan8742a.h")
    drvExtPhyLan8742aHeaderFile.setOutputName("drv_extphy_lan8742a.h")
    drvExtPhyLan8742aHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyLan8742aHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyLan8742aHeaderFile.setType("HEADER")
    drvExtPhyLan8742aHeaderFile.setOverwrite(True)
    drvExtPhyLan8742aHeaderFile.setEnabled(True)


    # file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
    # Add drv_ethphy.c file
    drvExtPhySourceFile = drvExtPhyLan8742aComponent.createFileSymbol(None, None)
    drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
    drvExtPhySourceFile.setOutputName("drv_ethphy.c")
    drvExtPhySourceFile.setOverwrite(True)
    drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setType("SOURCE")
    drvExtPhySourceFile.setEnabled(True)

    
    # ifblock TCPIP_EMAC_PHY_TYPE = "LAN8742A"
    # file TCPIP_MAC_PHY_LAN8742a_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_lan8742a.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_lan8742a.c"
    # endif
    # Add drv_extphy_lan8742a.c file
    drvExtPhyLan8742aSourceFile = drvExtPhyLan8742aComponent.createFileSymbol(None, None)
    drvExtPhyLan8742aSourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_lan8742a.c")
    drvExtPhyLan8742aSourceFile.setOutputName("drv_extphy_lan8742a.c")
    drvExtPhyLan8742aSourceFile.setOverwrite(True)
    drvExtPhyLan8742aSourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyLan8742aSourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyLan8742aSourceFile.setType("SOURCE")
    drvExtPhyLan8742aSourceFile.setEnabled(True)
    
    
def drvExtPhyLan8742aMenuVisibleSingle(symbol, event):
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
