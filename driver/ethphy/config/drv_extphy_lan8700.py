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


def instantiateComponent(drvExtPhyLan8700Component):
    print("LAN8700 PHY Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")

    # Delay for the Link Initialization in ms
    drvExtPhyLan8700LinkInitDelay= drvExtPhyLan8700Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", None)
    drvExtPhyLan8700LinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
    drvExtPhyLan8700LinkInitDelay.setVisible(True)
    drvExtPhyLan8700LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
    drvExtPhyLan8700LinkInitDelay.setDefaultValue(500)
    #drvExtPhyLan8700LinkInitDelay.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

    # PHY Address
    drvExtPhyLan8700Addr= drvExtPhyLan8700Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
    drvExtPhyLan8700Addr.setLabel("PHY Address") 
    drvExtPhyLan8700Addr.setVisible(True)
    drvExtPhyLan8700Addr.setDescription("PHY Address")
    drvExtPhyLan8700Addr.setDefaultValue(0)

    # External PHY Connection Flags
    drvExtPhyLan8700ConnFlag = drvExtPhyLan8700Component.createMenuSymbol(None, None) 
    drvExtPhyLan8700ConnFlag.setLabel("External PHY Connection Flags")
    drvExtPhyLan8700ConnFlag.setVisible(True)
    drvExtPhyLan8700ConnFlag.setDescription("External PHY Connection Flags")
    
    # RMII Data Interface
    drvExtPhyLan8700ConfigRmii = drvExtPhyLan8700Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyLan8700ConnFlag)
    drvExtPhyLan8700ConfigRmii.setLabel("RMII Data Interface")
    drvExtPhyLan8700ConfigRmii.setVisible(True)
    drvExtPhyLan8700ConfigRmii.setDescription("RMII Data Interface")
    if Peripheral.moduleExists("GMAC"):
        drvExtPhyLan8700ConfigRmii.setDefaultValue(True)
    elif "PIC32M" in Variables.get("__PROCESSOR"):
        drvExtPhyLan8700ConfigRmii.setDefaultValue(False)
    
        # Configuration Fuses Is ALT
        drvExtPhyLan8700ConfigAlt = drvExtPhyLan8700Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_ALTERNATE", drvExtPhyLan8700ConnFlag)
        drvExtPhyLan8700ConfigAlt.setLabel("Configuration Fuses Is ALT")
        drvExtPhyLan8700ConfigAlt.setVisible(True)
        drvExtPhyLan8700ConfigAlt.setDescription("Configuration Fuses Is ALT")
        drvExtPhyLan8700ConfigAlt.setDefaultValue(False)
        
        # Use The Fuses Configuration
        drvExtPhyLan8700ConfigAuto = drvExtPhyLan8700Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTO", drvExtPhyLan8700ConnFlag)
        drvExtPhyLan8700ConfigAuto.setLabel("Use The Fuses Configuration")
        drvExtPhyLan8700ConfigAuto.setVisible(True)
        drvExtPhyLan8700ConfigAuto.setDescription("Use The Fuses Configuration")
        drvExtPhyLan8700ConfigAuto.setDefaultValue(True)    
    

    # External PHY Type
    drvExtPhyLan8700PhyType = drvExtPhyLan8700Component.createStringSymbol("TCPIP_EMAC_PHY_TYPE", None)
    drvExtPhyLan8700PhyType.setVisible(False)   
    drvExtPhyLan8700PhyType.setDefaultValue("SMSC_LAN8700")
    
    # Driver PHY Instances Number
    drvExtPhyLan8700InstanceNum= drvExtPhyLan8700Component.createIntegerSymbol("DRV_ETHPHY_INSTANCES_NUMBER", None)
    drvExtPhyLan8700InstanceNum.setLabel("PHY Instances Number") 
    drvExtPhyLan8700InstanceNum.setVisible(True)
    drvExtPhyLan8700InstanceNum.setDescription("Driver PHY Instances Number")
    drvExtPhyLan8700InstanceNum.setDefaultValue(1)
    
    # Driver PHY Clients Number
    drvExtPhyLan8700ClientNum= drvExtPhyLan8700Component.createIntegerSymbol("DRV_ETHPHY_CLIENTS_NUMBER", None)
    drvExtPhyLan8700ClientNum.setLabel("PHY Clients Number") 
    drvExtPhyLan8700ClientNum.setVisible(True)
    drvExtPhyLan8700ClientNum.setDescription("Driver PHY Clients Number")
    drvExtPhyLan8700ClientNum.setDefaultValue(1)
    
    # Driver PHY Peripheral Index Number
    drvExtPhyLan8700IndexNum= drvExtPhyLan8700Component.createIntegerSymbol("DRV_ETHPHY_INDEX", None)
    drvExtPhyLan8700IndexNum.setLabel("PHY Peripheral Index Number") 
    drvExtPhyLan8700IndexNum.setVisible(True)
    drvExtPhyLan8700IndexNum.setDescription("Driver PHY Peripheral Index Number")
    drvExtPhyLan8700IndexNum.setDefaultValue(1)
    
    # Driver PHY Peripheral ID
    drvExtPhyLan8700PeripheralId= drvExtPhyLan8700Component.createIntegerSymbol("DRV_ETHPHY_PERIPHERAL_ID", None)
    drvExtPhyLan8700PeripheralId.setLabel("PHY Peripheral ID") 
    drvExtPhyLan8700PeripheralId.setVisible(True)
    drvExtPhyLan8700PeripheralId.setDescription("Driver PHY Peripheral ID")
    drvExtPhyLan8700PeripheralId.setDefaultValue(1)
    
    # Driver PHY Negotiation Time-out (mSec)
    drvExtPhyLan8700NegInitTimeout= drvExtPhyLan8700Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", None)
    drvExtPhyLan8700NegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
    drvExtPhyLan8700NegInitTimeout.setVisible(True)
    drvExtPhyLan8700NegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
    drvExtPhyLan8700NegInitTimeout.setDefaultValue(1)
    
    # Driver PHY Negotiation Done Time-out (mSec)
    drvExtPhyLan8700NegDoneTimeout= drvExtPhyLan8700Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", None)
    drvExtPhyLan8700NegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
    drvExtPhyLan8700NegDoneTimeout.setVisible(True)
    drvExtPhyLan8700NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
    drvExtPhyLan8700NegDoneTimeout.setDefaultValue(2000)
    
    # Driver PHY Reset Clear Time-out (mSec)
    drvExtPhyLan8700ResetClearTimeout= drvExtPhyLan8700Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", None)
    drvExtPhyLan8700ResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
    drvExtPhyLan8700ResetClearTimeout.setVisible(True)
    drvExtPhyLan8700ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
    drvExtPhyLan8700ResetClearTimeout.setDefaultValue(500)
    
    # Use a Function to be called at PHY Reset
    drvExtPhyLan8700ResetCallbackEnable = drvExtPhyLan8700Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
    drvExtPhyLan8700ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
    drvExtPhyLan8700ResetCallbackEnable.setVisible(True)
    drvExtPhyLan8700ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
    drvExtPhyLan8700ResetCallbackEnable.setDefaultValue(False)
    
    # App Function
    drvExtPhyLan8700ResetCallback = drvExtPhyLan8700Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyLan8700ResetCallbackEnable)
    drvExtPhyLan8700ResetCallback.setLabel("App Function")
    drvExtPhyLan8700ResetCallback.setVisible(False)
    drvExtPhyLan8700ResetCallback.setDescription("App Function")
    drvExtPhyLan8700ResetCallback.setDefaultValue("AppPhyResetFunction")
    drvExtPhyLan8700ResetCallback.setDependencies(drvExtPhyLan8700MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])

    #Add to system_config.h
    drvExtPhyLan8700HeaderFtl = drvExtPhyLan8700Component.createFileSymbol(None, None)
    drvExtPhyLan8700HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_lan8700.h.ftl")
    drvExtPhyLan8700HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    drvExtPhyLan8700HeaderFtl.setMarkup(True)
    drvExtPhyLan8700HeaderFtl.setType("STRING")
    
    # file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
    # Add drv_ethphy.h file to project
    drvExtPhyHeaderFile = drvExtPhyLan8700Component.createFileSymbol(None, None)
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
    drvExtPhyLocalHeaderFile = drvExtPhyLan8700Component.createFileSymbol(None, None)
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
    drvExtPhyLan8700RegHeaderFile = drvExtPhyLan8700Component.createFileSymbol(None, None)
    drvExtPhyLan8700RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
    drvExtPhyLan8700RegHeaderFile.setOutputName("drv_extphy_regs.h")
    drvExtPhyLan8700RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyLan8700RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyLan8700RegHeaderFile.setType("HEADER")
    drvExtPhyLan8700RegHeaderFile.setOverwrite(True)
    drvExtPhyLan8700RegHeaderFile.setEnabled(True)

    # Add drv_extphy_smsc8700.h file to project
    drvExtPhyLan8700HeaderFile = drvExtPhyLan8700Component.createFileSymbol(None, None)
    drvExtPhyLan8700HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc8700.h")
    drvExtPhyLan8700HeaderFile.setOutputName("drv_extphy_smsc8700.h")
    drvExtPhyLan8700HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyLan8700HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyLan8700HeaderFile.setType("HEADER")
    drvExtPhyLan8700HeaderFile.setOverwrite(True)


    # file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
    # Add drv_ethphy.c file
    drvExtPhySourceFile = drvExtPhyLan8700Component.createFileSymbol(None, None)
    drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
    drvExtPhySourceFile.setOutputName("drv_ethphy.c")
    drvExtPhySourceFile.setOverwrite(True)
    drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setType("SOURCE")
    drvExtPhySourceFile.setEnabled(True)

    # Add drv_extphy_smsc8700.c file
    drvExtPhyLan8700SourceFile = drvExtPhyLan8700Component.createFileSymbol(None, None)
    drvExtPhyLan8700SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc8700.c")
    drvExtPhyLan8700SourceFile.setOutputName("drv_extphy_smsc8700.c")
    drvExtPhyLan8700SourceFile.setOverwrite(True)
    drvExtPhyLan8700SourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyLan8700SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyLan8700SourceFile.setType("SOURCE")
    drvExtPhyLan8700SourceFile.setEnabled(True)

    
def drvExtPhyLan8700MenuVisibleSingle(symbol, event):
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