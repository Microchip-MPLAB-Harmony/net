"""
Copyright (C) 2019-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

phyName = "KSZ8091"
def instantiateComponent(drvExtPhyKsz8091Component):
    print("KSZ8091 PHY Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")

    # PHY Address
    drvExtPhyKsz8091Addr= drvExtPhyKsz8091Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
    drvExtPhyKsz8091Addr.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8091Addr.setLabel("PHY Address") 
    drvExtPhyKsz8091Addr.setVisible(True)
    drvExtPhyKsz8091Addr.setDescription("PHY Address")
    drvExtPhyKsz8091Addr.setDefaultValue(0)
    
    # Use a Function to be called at PHY Reset
    drvExtPhyKsz8091ResetCallbackEnable = drvExtPhyKsz8091Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
    drvExtPhyKsz8091ResetCallbackEnable.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8091ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
    drvExtPhyKsz8091ResetCallbackEnable.setVisible(True)
    drvExtPhyKsz8091ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
    drvExtPhyKsz8091ResetCallbackEnable.setDefaultValue(False)
    
    # App Function
    drvExtPhyKsz8091ResetCallback = drvExtPhyKsz8091Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyKsz8091ResetCallbackEnable)
    drvExtPhyKsz8091ResetCallback.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8091ResetCallback.setLabel("App Function")
    drvExtPhyKsz8091ResetCallback.setVisible(False)
    drvExtPhyKsz8091ResetCallback.setDescription("App Function")
    drvExtPhyKsz8091ResetCallback.setDefaultValue("App" + phyName + "ResetFunction")
    drvExtPhyKsz8091ResetCallback.setDependencies(drvExtPhyKsz8091MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])

    # External PHY Connection Flags
    drvExtPhyKsz8091ConnFlag = drvExtPhyKsz8091Component.createMenuSymbol(None, None) 
    drvExtPhyKsz8091ConnFlag.setLabel("External PHY Connection Flags")
    drvExtPhyKsz8091ConnFlag.setVisible(True)
    drvExtPhyKsz8091ConnFlag.setDescription("External PHY Connection Flags")
    
    # RMII Data Interface
    drvExtPhyKsz8091ConfigRmii = drvExtPhyKsz8091Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyKsz8091ConnFlag)
    drvExtPhyKsz8091ConfigRmii.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8091ConfigRmii.setLabel("RMII Data Interface")
    drvExtPhyKsz8091ConfigRmii.setVisible(True)
    drvExtPhyKsz8091ConfigRmii.setDescription("RMII Data Interface")
    drvExtPhyKsz8091ConfigRmii.setDefaultValue(True)
    
    if ("PIC32M" in Variables.get("__PROCESSOR")) or ("WFI32" in Variables.get("__PROCESSOR")):
        # default is Auto for PIC32M
        drvExtPhyKsz8091ConfigRmii.setDefaultValue(False)
    
        # Configuration Fuses Is ALT
        drvExtPhyKsz8091ConfigAlt = drvExtPhyKsz8091Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_ALTERNATE", drvExtPhyKsz8091ConnFlag)
        drvExtPhyKsz8091ConfigAlt.setHelp("mcc_h3_phy_configurations")
        drvExtPhyKsz8091ConfigAlt.setLabel("Configuration Fuses Is ALT")
        drvExtPhyKsz8091ConfigAlt.setVisible(True)
        drvExtPhyKsz8091ConfigAlt.setDescription("Configuration Fuses Is ALT")
        drvExtPhyKsz8091ConfigAlt.setDefaultValue(False)
        
        # Use The Fuses Configuration
        drvExtPhyKsz8091ConfigAuto = drvExtPhyKsz8091Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTO", drvExtPhyKsz8091ConnFlag)
        drvExtPhyKsz8091ConfigAuto.setHelp("mcc_h3_phy_configurations")
        drvExtPhyKsz8091ConfigAuto.setLabel("Use The Fuses Configuration")
        drvExtPhyKsz8091ConfigAuto.setVisible(True)
        drvExtPhyKsz8091ConfigAuto.setDescription("Use The Fuses Configuration")
        drvExtPhyKsz8091ConfigAuto.setDefaultValue(True)    
            
    # Advanced Settings
    drvExtPhyKsz8091AdvSettings = drvExtPhyKsz8091Component.createMenuSymbol("TCPIP_INTMAC_PHY_ADV_SETTING", None)
    drvExtPhyKsz8091AdvSettings.setLabel("Advanced Settings")
    drvExtPhyKsz8091AdvSettings.setDescription("Advanced Settings")
    drvExtPhyKsz8091AdvSettings.setVisible(True)    

    # Delay for the Link Initialization in ms
    drvExtPhyKsz8091LinkInitDelay= drvExtPhyKsz8091Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", drvExtPhyKsz8091AdvSettings)
    drvExtPhyKsz8091LinkInitDelay.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8091LinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
    drvExtPhyKsz8091LinkInitDelay.setVisible(True)
    drvExtPhyKsz8091LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
    drvExtPhyKsz8091LinkInitDelay.setDefaultValue(500)
      
    # External PHY Type
    drvExtPhyKsz8091PhyType = drvExtPhyKsz8091Component.createStringSymbol("TCPIP_EMAC_PHY_TYPE", drvExtPhyKsz8091AdvSettings)
    drvExtPhyKsz8091PhyType.setVisible(False)   
    drvExtPhyKsz8091PhyType.setDefaultValue(phyName)
    
    # Driver PHY Negotiation Time-out - ms
    drvExtPhyKsz8091NegInitTimeout= drvExtPhyKsz8091Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", drvExtPhyKsz8091AdvSettings)
    drvExtPhyKsz8091NegInitTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8091NegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
    drvExtPhyKsz8091NegInitTimeout.setVisible(True)
    drvExtPhyKsz8091NegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
    drvExtPhyKsz8091NegInitTimeout.setDefaultValue(1)
    
    # Driver PHY Negotiation Done Time-out - ms
    drvExtPhyKsz8091NegDoneTimeout= drvExtPhyKsz8091Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", drvExtPhyKsz8091AdvSettings)
    drvExtPhyKsz8091NegDoneTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8091NegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
    drvExtPhyKsz8091NegDoneTimeout.setVisible(True)
    drvExtPhyKsz8091NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
    drvExtPhyKsz8091NegDoneTimeout.setDefaultValue(2000)
    
    # Driver PHY Reset Clear Time-out - ms
    drvExtPhyKsz8091ResetClearTimeout= drvExtPhyKsz8091Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", drvExtPhyKsz8091AdvSettings)
    drvExtPhyKsz8091ResetClearTimeout.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8091ResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
    drvExtPhyKsz8091ResetClearTimeout.setVisible(True)
    drvExtPhyKsz8091ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
    drvExtPhyKsz8091ResetClearTimeout.setDefaultValue(500)    
    
    # Driver PHY Instances Number
    drvExtPhyKsz8091InstanceNum= drvExtPhyKsz8091Component.createIntegerSymbol("DRV_ETHPHY_INSTANCES_NUMBER", drvExtPhyKsz8091AdvSettings)
    drvExtPhyKsz8091InstanceNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8091InstanceNum.setLabel("PHY Instances Number") 
    drvExtPhyKsz8091InstanceNum.setVisible(True)
    drvExtPhyKsz8091InstanceNum.setDescription("Driver PHY Instances Number")
    drvExtPhyKsz8091InstanceNum.setDefaultValue(1)
    drvExtPhyKsz8091InstanceNum.setReadOnly(True)
    
    # Driver PHY Clients Number
    drvExtPhyKsz8091ClientNum= drvExtPhyKsz8091Component.createIntegerSymbol("DRV_ETHPHY_CLIENTS_NUMBER", drvExtPhyKsz8091AdvSettings)
    drvExtPhyKsz8091ClientNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8091ClientNum.setLabel("PHY Clients Number") 
    drvExtPhyKsz8091ClientNum.setVisible(True)
    drvExtPhyKsz8091ClientNum.setDescription("Driver PHY Clients Number")
    drvExtPhyKsz8091ClientNum.setDefaultValue(1)
    drvExtPhyKsz8091ClientNum.setReadOnly(True)
    
    # Driver PHY Peripheral Index Number
    drvExtPhyKsz8091IndexNum= drvExtPhyKsz8091Component.createIntegerSymbol("DRV_ETHPHY_INDEX", drvExtPhyKsz8091AdvSettings)
    drvExtPhyKsz8091IndexNum.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8091IndexNum.setLabel("PHY Peripheral Index Number") 
    drvExtPhyKsz8091IndexNum.setVisible(True)
    drvExtPhyKsz8091IndexNum.setDescription("Driver PHY Peripheral Index Number")
    drvExtPhyKsz8091IndexNum.setDefaultValue(1)
    drvExtPhyKsz8091IndexNum.setReadOnly(True)
    
    # Driver PHY Peripheral ID
    drvExtPhyKsz8091PeripheralId= drvExtPhyKsz8091Component.createStringSymbol("DRV_ETHPHY_PERIPHERAL_ID", drvExtPhyKsz8091AdvSettings)
    drvExtPhyKsz8091PeripheralId.setHelp("mcc_h3_phy_configurations")
    drvExtPhyKsz8091PeripheralId.setLabel("PHY Peripheral ID") 
    drvExtPhyKsz8091PeripheralId.setVisible(True)
    drvExtPhyKsz8091PeripheralId.setDescription("Driver PHY Peripheral ID")
    drvExtPhyKsz8091PeripheralId.setDefaultValue("")
    drvExtPhyKsz8091PeripheralId.setReadOnly(True)

    # External MAC Name
    drvExtPhyMacName= drvExtPhyKsz8091Component.createStringSymbol("DRV_ETHPHY_MAC_NAME", None)
    drvExtPhyMacName.setLabel("Mac Name") 
    drvExtPhyMacName.setVisible(False)   
    drvExtPhyMacName.setDefaultValue("")
    drvExtPhyMacName.setReadOnly(True) 
        
    #Add forward declaration to initialization.c
    drvExtPhyKsz8091InitDataSourceFtl = drvExtPhyKsz8091Component.createFileSymbol(None, None)
    drvExtPhyKsz8091InitDataSourceFtl.setType("STRING")
    drvExtPhyKsz8091InitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    drvExtPhyKsz8091InitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_driver_initialize.c.ftl")
    drvExtPhyKsz8091InitDataSourceFtl.setMarkup(True)    
    
    #Add to initialization.c
    drvExtPhyKsz8091SysInitDataSourceFtl = drvExtPhyKsz8091Component.createFileSymbol(None, None)
    drvExtPhyKsz8091SysInitDataSourceFtl.setType("STRING")
    drvExtPhyKsz8091SysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    drvExtPhyKsz8091SysInitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_data_initialize.c.ftl")
    drvExtPhyKsz8091SysInitDataSourceFtl.setMarkup(True)    
    
    #Add to system_config.h
    drvExtPhyKsz8091HeaderFtl = drvExtPhyKsz8091Component.createFileSymbol(None, None)
    drvExtPhyKsz8091HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_ksz8091.h.ftl")
    drvExtPhyKsz8091HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    drvExtPhyKsz8091HeaderFtl.setMarkup(True)
    drvExtPhyKsz8091HeaderFtl.setType("STRING")
        
    #Add to definitions.h
    drvExtPhyKsz8091SysdefFtl = drvExtPhyKsz8091Component.createFileSymbol(None, None)
    drvExtPhyKsz8091SysdefFtl.setSourcePath("driver/ethphy/templates/system/system_definitions.h.ftl")
    drvExtPhyKsz8091SysdefFtl.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    drvExtPhyKsz8091SysdefFtl.setMarkup(True)
    drvExtPhyKsz8091SysdefFtl.setType("STRING")

    # file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
    # Add drv_ethphy.h file to project
    drvExtPhyHeaderFile = drvExtPhyKsz8091Component.createFileSymbol(None, None)
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
    drvExtPhyLocalHeaderFile = drvExtPhyKsz8091Component.createFileSymbol(None, None)
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
    drvExtPhyKsz8091RegHeaderFile = drvExtPhyKsz8091Component.createFileSymbol(None, None)
    drvExtPhyKsz8091RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
    drvExtPhyKsz8091RegHeaderFile.setOutputName("drv_extphy_regs.h")
    drvExtPhyKsz8091RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyKsz8091RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyKsz8091RegHeaderFile.setType("HEADER")
    drvExtPhyKsz8091RegHeaderFile.setOverwrite(True)
    drvExtPhyKsz8091RegHeaderFile.setEnabled(True)
    
    # Add drv_extphy_ksz8091.h file to project
    drvExtPhyKsz8091HeaderFile = drvExtPhyKsz8091Component.createFileSymbol(None, None)
    drvExtPhyKsz8091HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz8091.h")
    drvExtPhyKsz8091HeaderFile.setOutputName("drv_extphy_ksz8091.h")
    drvExtPhyKsz8091HeaderFile.setDestPath("driver/ethphy/")
    drvExtPhyKsz8091HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/")
    drvExtPhyKsz8091HeaderFile.setType("HEADER")
    drvExtPhyKsz8091HeaderFile.setOverwrite(True)
    drvExtPhyKsz8091HeaderFile.setEnabled(True)

    # Add drv_extphy_ksz8091_priv.h file to project
    drvExtPhyKsz8091PrivFile = drvExtPhyKsz8091Component.createFileSymbol(None, None)
    drvExtPhyKsz8091PrivFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz8091_priv.h")
    drvExtPhyKsz8091PrivFile.setOutputName("drv_extphy_ksz8091_priv.h")
    drvExtPhyKsz8091PrivFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyKsz8091PrivFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyKsz8091PrivFile.setType("HEADER")
    drvExtPhyKsz8091PrivFile.setOverwrite(True)
    drvExtPhyKsz8091PrivFile.setEnabled(True)

    # file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
    # Add drv_ethphy.c file
    drvExtPhySourceFile = drvExtPhyKsz8091Component.createFileSymbol(None, None)
    drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
    drvExtPhySourceFile.setOutputName("drv_ethphy.c")
    drvExtPhySourceFile.setOverwrite(True)
    drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setType("SOURCE")
    drvExtPhySourceFile.setEnabled(True)

    # Add drv_extphy_ksz8091.c file
    drvExtPhyKsz8091SourceFile = drvExtPhyKsz8091Component.createFileSymbol(None, None)
    drvExtPhyKsz8091SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz8091.c")
    drvExtPhyKsz8091SourceFile.setOutputName("drv_extphy_ksz8091.c")
    drvExtPhyKsz8091SourceFile.setOverwrite(True)
    drvExtPhyKsz8091SourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyKsz8091SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyKsz8091SourceFile.setType("SOURCE")
    drvExtPhyKsz8091SourceFile.setEnabled(True)

    
def drvExtPhyKsz8091MenuVisibleSingle(symbol, event):
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
