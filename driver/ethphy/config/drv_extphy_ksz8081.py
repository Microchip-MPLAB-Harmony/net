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


def instantiateComponent(drvExtPhyKsz8081Component):
    print("KSZ8081 PHY Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")

    # Delay for the Link Initialization in ms
    drvExtPhyKsz8081LinkInitDelay= drvExtPhyKsz8081Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", None)
    drvExtPhyKsz8081LinkInitDelay.setLabel("Delay for the Link Initialization (mSec)") 
    drvExtPhyKsz8081LinkInitDelay.setVisible(True)
    drvExtPhyKsz8081LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
    drvExtPhyKsz8081LinkInitDelay.setDefaultValue(500)
    #drvExtPhyKsz8081LinkInitDelay.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

    # PHY Address
    drvExtPhyKsz8081Addr= drvExtPhyKsz8081Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
    drvExtPhyKsz8081Addr.setLabel("PHY Address") 
    drvExtPhyKsz8081Addr.setVisible(True)
    drvExtPhyKsz8081Addr.setDescription("PHY Address")
    drvExtPhyKsz8081Addr.setDefaultValue(1)

    # External PHY Connection Flags
    drvExtPhyKsz8081ConnFlag = drvExtPhyKsz8081Component.createMenuSymbol(None, None) 
    drvExtPhyKsz8081ConnFlag.setLabel("External PHY Connection Flags")
    drvExtPhyKsz8081ConnFlag.setVisible(True)
    drvExtPhyKsz8081ConnFlag.setDescription("External PHY Connection Flags")
    
    # RMII Data Interface
    drvExtPhyKsz8081ConfigRmii = drvExtPhyKsz8081Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyKsz8081ConnFlag)
    drvExtPhyKsz8081ConfigRmii.setLabel("RMII Data Interface")
    drvExtPhyKsz8081ConfigRmii.setVisible(True)
    drvExtPhyKsz8081ConfigRmii.setDescription("RMII Data Interface")
    
    if Peripheral.moduleExists("GMAC"):
        drvExtPhyKsz8081ConfigRmii.setDefaultValue(True)
    elif "PIC32M" in Variables.get("__PROCESSOR"):
        drvExtPhyKsz8081ConfigRmii.setDefaultValue(False)
    
        # Configuration Fuses Is ALT
        drvExtPhyKsz8081ConfigAlt = drvExtPhyKsz8081Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_ALTERNATE", drvExtPhyKsz8081ConnFlag)
        drvExtPhyKsz8081ConfigAlt.setLabel("Configuration Fuses Is ALT")
        drvExtPhyKsz8081ConfigAlt.setVisible(True)
        drvExtPhyKsz8081ConfigAlt.setDescription("Configuration Fuses Is ALT")
        drvExtPhyKsz8081ConfigAlt.setDefaultValue(False)
        
        # Use The Fuses Configuration
        drvExtPhyKsz8081ConfigAuto = drvExtPhyKsz8081Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTO", drvExtPhyKsz8081ConnFlag)
        drvExtPhyKsz8081ConfigAuto.setLabel("Use The Fuses Configuration")
        drvExtPhyKsz8081ConfigAuto.setVisible(True)
        drvExtPhyKsz8081ConfigAuto.setDescription("Use The Fuses Configuration")
        drvExtPhyKsz8081ConfigAuto.setDefaultValue(True)    
    
    
    # External PHY Type
    drvExtPhyKsz8081PhyType = drvExtPhyKsz8081Component.createStringSymbol("TCPIP_EMAC_PHY_TYPE", None)
    drvExtPhyKsz8081PhyType.setVisible(False)   
    drvExtPhyKsz8081PhyType.setDefaultValue("KSZ8081")
    
    # Driver PHY Instances Number
    drvExtPhyKsz8081InstanceNum= drvExtPhyKsz8081Component.createIntegerSymbol("DRV_ETHPHY_INSTANCES_NUMBER", None)
    drvExtPhyKsz8081InstanceNum.setLabel("PHY Instances Number") 
    drvExtPhyKsz8081InstanceNum.setVisible(True)
    drvExtPhyKsz8081InstanceNum.setDescription("Driver PHY Instances Number")
    drvExtPhyKsz8081InstanceNum.setDefaultValue(1)
    
    # Driver PHY Clients Number
    drvExtPhyKsz8081ClientNum= drvExtPhyKsz8081Component.createIntegerSymbol("DRV_ETHPHY_CLIENTS_NUMBER", None)
    drvExtPhyKsz8081ClientNum.setLabel("PHY Clients Number") 
    drvExtPhyKsz8081ClientNum.setVisible(True)
    drvExtPhyKsz8081ClientNum.setDescription("Driver PHY Clients Number")
    drvExtPhyKsz8081ClientNum.setDefaultValue(1)
    
    # Driver PHY Peripheral Index Number
    drvExtPhyKsz8081IndexNum= drvExtPhyKsz8081Component.createIntegerSymbol("DRV_ETHPHY_INDEX", None)
    drvExtPhyKsz8081IndexNum.setLabel("PHY Peripheral Index Number") 
    drvExtPhyKsz8081IndexNum.setVisible(True)
    drvExtPhyKsz8081IndexNum.setDescription("Driver PHY Peripheral Index Number")
    drvExtPhyKsz8081IndexNum.setDefaultValue(1)
    
    # Driver PHY Peripheral ID
    drvExtPhyKsz8081PeripheralId= drvExtPhyKsz8081Component.createIntegerSymbol("DRV_ETHPHY_PERIPHERAL_ID", None)
    drvExtPhyKsz8081PeripheralId.setLabel("PHY Peripheral ID") 
    drvExtPhyKsz8081PeripheralId.setVisible(True)
    drvExtPhyKsz8081PeripheralId.setDescription("Driver PHY Peripheral ID")
    drvExtPhyKsz8081PeripheralId.setDefaultValue(1)
    
    # Driver PHY Negotiation Time-out (mSec)
    drvExtPhyKsz8081NegInitTimeout= drvExtPhyKsz8081Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", None)
    drvExtPhyKsz8081NegInitTimeout.setLabel("PHY Negotiation Time-out (mSec)") 
    drvExtPhyKsz8081NegInitTimeout.setVisible(True)
    drvExtPhyKsz8081NegInitTimeout.setDescription("Driver PHY Negotiation Time-out (mSec)")
    drvExtPhyKsz8081NegInitTimeout.setDefaultValue(1)
    
    # Driver PHY Negotiation Done Time-out (mSec)
    drvExtPhyKsz8081NegDoneTimeout= drvExtPhyKsz8081Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", None)
    drvExtPhyKsz8081NegDoneTimeout.setLabel("PHY Negotiation Done Time-out (mSec)") 
    drvExtPhyKsz8081NegDoneTimeout.setVisible(True)
    drvExtPhyKsz8081NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out (mSec)")
    drvExtPhyKsz8081NegDoneTimeout.setDefaultValue(2000)
    
    # Driver PHY Reset Clear Time-out (mSec)
    drvExtPhyKsz8081ResetClearTimeout= drvExtPhyKsz8081Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", None)
    drvExtPhyKsz8081ResetClearTimeout.setLabel("PHY Reset Clear Time-out (mSec)") 
    drvExtPhyKsz8081ResetClearTimeout.setVisible(True)
    drvExtPhyKsz8081ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out (mSec)")
    drvExtPhyKsz8081ResetClearTimeout.setDefaultValue(500)
    
    # Use a Function to be called at PHY Reset
    drvExtPhyKsz8081ResetCallbackEnable = drvExtPhyKsz8081Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
    drvExtPhyKsz8081ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
    drvExtPhyKsz8081ResetCallbackEnable.setVisible(True)
    drvExtPhyKsz8081ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
    drvExtPhyKsz8081ResetCallbackEnable.setDefaultValue(False)
    
    # App Function
    drvExtPhyKsz8081ResetCallback = drvExtPhyKsz8081Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyKsz8081ResetCallbackEnable)
    drvExtPhyKsz8081ResetCallback.setLabel("App Function")
    drvExtPhyKsz8081ResetCallback.setVisible(False)
    drvExtPhyKsz8081ResetCallback.setDescription("App Function")
    drvExtPhyKsz8081ResetCallback.setDefaultValue("AppPhyResetFunction")
    drvExtPhyKsz8081ResetCallback.setDependencies(drvExtPhyKsz8081MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])

    #Add to system_config.h
    drvExtPhyKsz8081HeaderFtl = drvExtPhyKsz8081Component.createFileSymbol(None, None)
    drvExtPhyKsz8081HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_ksz8081.h.ftl")
    drvExtPhyKsz8081HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    drvExtPhyKsz8081HeaderFtl.setMarkup(True)
    drvExtPhyKsz8081HeaderFtl.setType("STRING")
        
    # file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
    # Add drv_ethphy.h file to project
    drvExtPhyHeaderFile = drvExtPhyKsz8081Component.createFileSymbol(None, None)
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
    drvExtPhyLocalHeaderFile = drvExtPhyKsz8081Component.createFileSymbol(None, None)
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
    drvExtPhyKsz8081RegHeaderFile = drvExtPhyKsz8081Component.createFileSymbol(None, None)
    drvExtPhyKsz8081RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
    drvExtPhyKsz8081RegHeaderFile.setOutputName("drv_extphy_regs.h")
    drvExtPhyKsz8081RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyKsz8081RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyKsz8081RegHeaderFile.setType("HEADER")
    drvExtPhyKsz8081RegHeaderFile.setOverwrite(True)
    drvExtPhyKsz8081RegHeaderFile.setEnabled(True)
    
    # Add drv_extphy_ksz8081.h file to project
    drvExtPhyKsz8081HeaderFile = drvExtPhyKsz8081Component.createFileSymbol(None, None)
    drvExtPhyKsz8081HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz8081.h")
    drvExtPhyKsz8081HeaderFile.setOutputName("drv_extphy_ksz8081.h")
    drvExtPhyKsz8081HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyKsz8081HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyKsz8081HeaderFile.setType("HEADER")
    drvExtPhyKsz8081HeaderFile.setOverwrite(True)
    drvExtPhyKsz8081HeaderFile.setEnabled(True)

    # file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
    # Add drv_ethphy.c file
    drvExtPhySourceFile = drvExtPhyKsz8081Component.createFileSymbol(None, None)
    drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
    drvExtPhySourceFile.setOutputName("drv_ethphy.c")
    drvExtPhySourceFile.setOverwrite(True)
    drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setType("SOURCE")
    drvExtPhySourceFile.setEnabled(True)

    # Add drv_extphy_ksz8081.c file
    drvExtPhyKsz8081SourceFile = drvExtPhyKsz8081Component.createFileSymbol(None, None)
    drvExtPhyKsz8081SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz8081.c")
    drvExtPhyKsz8081SourceFile.setOutputName("drv_extphy_ksz8081.c")
    drvExtPhyKsz8081SourceFile.setOverwrite(True)
    drvExtPhyKsz8081SourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyKsz8081SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyKsz8081SourceFile.setType("SOURCE")
    drvExtPhyKsz8081SourceFile.setEnabled(True)

    
def drvExtPhyKsz8081MenuVisibleSingle(symbol, event):
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
        print "Set Symbol Failure"
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