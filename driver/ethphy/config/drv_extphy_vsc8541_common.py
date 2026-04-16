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
def instantiateComponent(drvExtPhyVsc8541CommonComponent):
    print("VSC8541 PHY Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")    

    # Enable VSC8541
    drvExtPhyVsc8541Module = drvExtPhyVsc8541CommonComponent.createBooleanSymbol("DRV_VSC8541_USE_DRIVER", None)
    drvExtPhyVsc8541Module.setLabel("Enable VSC8541")
    drvExtPhyVsc8541Module.setVisible(False)
    drvExtPhyVsc8541Module.setDescription("Enable VSC8541")
    drvExtPhyVsc8541Module.setDefaultValue(True)

                                
    #Add forward declaration to initialization.c
    drvExtPhyVsc8541InitDataSourceFtl = drvExtPhyVsc8541CommonComponent.createFileSymbol(None, None)
    drvExtPhyVsc8541InitDataSourceFtl.setType("STRING")
    drvExtPhyVsc8541InitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    drvExtPhyVsc8541InitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_driver_initialize.c.ftl")
    drvExtPhyVsc8541InitDataSourceFtl.setMarkup(True)    
    
    #Add to initialization.c
    drvExtPhyVsc8541SysInitDataSourceFtl = drvExtPhyVsc8541CommonComponent.createFileSymbol(None, None)
    drvExtPhyVsc8541SysInitDataSourceFtl.setType("STRING")
    drvExtPhyVsc8541SysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    drvExtPhyVsc8541SysInitDataSourceFtl.setSourcePath("driver/ethphy/templates/system/system_data_initialize.c.ftl")
    drvExtPhyVsc8541SysInitDataSourceFtl.setMarkup(True)    
    
    #Add to system_config.h
    drvExtPhyVsc8541HeaderFtl = drvExtPhyVsc8541CommonComponent.createFileSymbol(None, None)
    drvExtPhyVsc8541HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_vsc8541.h.ftl")
    drvExtPhyVsc8541HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    drvExtPhyVsc8541HeaderFtl.setMarkup(True)
    drvExtPhyVsc8541HeaderFtl.setType("STRING")
        
    #Add to definitions.h
    drvExtPhyVsc8541SysdefFtl = drvExtPhyVsc8541CommonComponent.createFileSymbol(None, None)
    drvExtPhyVsc8541SysdefFtl.setSourcePath("driver/ethphy/templates/system/system_definitions.h.ftl")
    drvExtPhyVsc8541SysdefFtl.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    drvExtPhyVsc8541SysdefFtl.setMarkup(True)
    drvExtPhyVsc8541SysdefFtl.setType("STRING")

    # file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
    # Add drv_ethphy.h file to project
    drvExtPhyHeaderFile = drvExtPhyVsc8541CommonComponent.createFileSymbol(None, None)
    drvExtPhyHeaderFile.setSourcePath("driver/ethphy/drv_ethphy.h")
    drvExtPhyHeaderFile.setOutputName("drv_ethphy.h")
    drvExtPhyHeaderFile.setDestPath("driver/ethphy/")
    drvExtPhyHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/")
    drvExtPhyHeaderFile.setType("HEADER")
    drvExtPhyHeaderFile.setOverwrite(True)
    drvExtPhyHeaderFile.setEnabled(True)

    # file TCPIP_ETH_PHY_LOCAL_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/drv_ethphy_local.h" to           "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/drv_ethphy_local.h"
    # Add drv_ethphy_local.h file to project
    drvExtPhyLocalHeaderFile = drvExtPhyVsc8541CommonComponent.createFileSymbol(None, None)
    drvExtPhyLocalHeaderFile.setSourcePath("driver/ethphy/src/drv_ethphy_local.h")
    drvExtPhyLocalHeaderFile.setOutputName("drv_ethphy_local.h")
    drvExtPhyLocalHeaderFile.setDestPath("driver/ethphy/src/")
    drvExtPhyLocalHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/")
    drvExtPhyLocalHeaderFile.setType("HEADER")
    drvExtPhyLocalHeaderFile.setOverwrite(True)
    drvExtPhyLocalHeaderFile.setEnabled(True)


    # file TCPIP_ETH_EXT_PHY_REGS_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h" to    "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h"
    # Add drv_extphy_regs.h file to project
    drvExtPhyVsc8541RegHeaderFile = drvExtPhyVsc8541CommonComponent.createFileSymbol(None, None)
    drvExtPhyVsc8541RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
    drvExtPhyVsc8541RegHeaderFile.setOutputName("drv_extphy_regs.h")
    drvExtPhyVsc8541RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyVsc8541RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyVsc8541RegHeaderFile.setType("HEADER")
    drvExtPhyVsc8541RegHeaderFile.setOverwrite(True)
    drvExtPhyVsc8541RegHeaderFile.setEnabled(True)
    
    # Add drv_extphy_vsc8541.h file to project
    drvExtPhyVsc8541HeaderFile = drvExtPhyVsc8541CommonComponent.createFileSymbol("PHY_VSC8541_HEADER", None)
    drvExtPhyVsc8541HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_vsc8541.h")
    drvExtPhyVsc8541HeaderFile.setOutputName("drv_extphy_vsc8541.h")
    drvExtPhyVsc8541HeaderFile.setDestPath("driver/ethphy/")
    drvExtPhyVsc8541HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/")
    drvExtPhyVsc8541HeaderFile.setType("HEADER")
    drvExtPhyVsc8541HeaderFile.setOverwrite(True)
    drvExtPhyVsc8541HeaderFile.setEnabled(True)

    # Add drv_extphy_vsc8541_priv.h file to project
    drvExtPhyVsc8541PrivFile = drvExtPhyVsc8541CommonComponent.createFileSymbol(None, None)
    drvExtPhyVsc8541PrivFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_vsc8541_priv.h")
    drvExtPhyVsc8541PrivFile.setOutputName("drv_extphy_vsc8541_priv.h")
    drvExtPhyVsc8541PrivFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyVsc8541PrivFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyVsc8541PrivFile.setType("HEADER")
    drvExtPhyVsc8541PrivFile.setOverwrite(True)
    drvExtPhyVsc8541PrivFile.setEnabled(True)

    # file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
    # Add drv_ethphy.c file
    drvExtPhySourceFile = drvExtPhyVsc8541CommonComponent.createFileSymbol(None, None)
    drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
    drvExtPhySourceFile.setOutputName("drv_ethphy.c")
    drvExtPhySourceFile.setOverwrite(True)
    drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhySourceFile.setType("SOURCE")
    drvExtPhySourceFile.setEnabled(True)

    # Add drv_extphy_vsc8541.c file
    drvExtPhyVsc8541SourceFile = drvExtPhyVsc8541CommonComponent.createFileSymbol("PHY_VSC8541_SOURCE", None)
    drvExtPhyVsc8541SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_vsc8541.c")
    drvExtPhyVsc8541SourceFile.setOutputName("drv_extphy_vsc8541.c")
    drvExtPhyVsc8541SourceFile.setOverwrite(True)
    drvExtPhyVsc8541SourceFile.setDestPath("driver/ethphy/src/dynamic/")
    drvExtPhyVsc8541SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
    drvExtPhyVsc8541SourceFile.setType("SOURCE")
    drvExtPhyVsc8541SourceFile.setEnabled(True)


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
