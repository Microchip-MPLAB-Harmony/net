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

def instantiateComponent(drvMiimComponent):
    print("MIIM Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    
    
    # Use Internal Ethernet MAC Driver? 
    drvUseMiim = drvMiimComponent.createBooleanSymbol("DRV_MIIM_USE_DRIVER", None) 
    drvUseMiim.setLabel("Use MIIM Driver?")
    drvUseMiim.setVisible(False)
    drvUseMiim.setDescription("Use MIIM Driver?")
    drvUseMiim.setDefaultValue(True)
    
    # Ethernet MAC Module ID
    drvMiimModuleId = drvMiimComponent.createStringSymbol("DRV_MIIM_ETH_MODULE_ID", None) 
    drvMiimModuleId.setLabel("ETH Module ID")
    drvMiimModuleId.setVisible(True)
    drvMiimModuleId.setDescription("Ethernet MAC Module ID")    
    if((Database.getSymbolValue("drvGmac", "TCPIP_USE_ETH_MAC") == True)):    
        drvMiimModuleId.setDefaultValue("GMAC_BASE_ADDRESS") 
    elif ((Database.getSymbolValue("drvPic32mEthmac", "TCPIP_USE_ETH_MAC") == True)):    
        drvMiimModuleId.setDefaultValue("_ETH_BASE_ADDRESS") 
    elif ((Database.getSymbolValue("drvEmac0", "TCPIP_USE_EMAC0") == True)):    
        drvMiimModuleId.setDefaultValue("EMAC0_BASE_ADDRESS") 
    elif ((Database.getSymbolValue("drvEmac1", "TCPIP_USE_EMAC1") == True)):    
        drvMiimModuleId.setDefaultValue("EMAC1_BASE_ADDRESS") 
    drvMiimModuleId.setDependencies(drvMiimSetIntMacId,["drvGmac.TCPIP_USE_ETH_MAC","drvPic32mEthmac.TCPIP_USE_ETH_MAC", "drvEmac0.TCPIP_USE_EMAC0", "drvEmac1.TCPIP_USE_EMAC1"])

    # Number of Driver Instances
    drvMiimInstnNum = drvMiimComponent.createIntegerSymbol("DRV_MIIM_INSTANCES_NUMBER", None)
    drvMiimInstnNum.setLabel("Number of Driver Instances")
    drvMiimInstnNum.setVisible(True)
    drvMiimInstnNum.setDescription("Number of Driver Instances")
    drvMiimInstnNum.setDefaultValue(1)
    #drvMiimInstnNum.setDependencies(drvMiimMenuVisibleSingle, ["DRV_MIIM_USE_DRIVER"])

    # Number of Instance Operations
    drvMiimOpsInstnNum = drvMiimComponent.createIntegerSymbol("DRV_MIIM_INSTANCE_OPERATIONS", None)
    drvMiimOpsInstnNum.setLabel("Number of Instance Operations")
    drvMiimOpsInstnNum.setVisible(True)
    drvMiimOpsInstnNum.setDescription("Number of Instance Operations")
    # H3_ToDo default 8 if TCPIP_EMAC_PHY_TYPE = "SMSC_LAN9303"
    drvMiimOpsInstnNum.setDefaultValue(4)
    #drvMiimOpsInstnNum.setDependencies(drvMiimMenuVisibleSingle, ["DRV_MIIM_USE_DRIVER"])

    # Maximum Number of Clients per Instance
    drvMiimMaxNumClientsInstn = drvMiimComponent.createIntegerSymbol("DRV_MIIM_INSTANCE_CLIENTS", None)
    drvMiimMaxNumClientsInstn.setLabel("Maximum Number of Clients per Instance")
    drvMiimMaxNumClientsInstn.setVisible(True)
    drvMiimMaxNumClientsInstn.setDescription("Maximum Number of Clients per Instance")
    drvMiimMaxNumClientsInstn.setDefaultValue(2)
    #drvMiimMaxNumClientsInstn.setDependencies(drvMiimMenuVisibleSingle, ["DRV_MIIM_USE_DRIVER"])
    
    # Enable Client Operation Protection Feature
    drvMiimOpsProtectEnable = drvMiimComponent.createBooleanSymbol("DRV_MIIM_CLIENT_OP_PROTECTION", None) 
    drvMiimOpsProtectEnable.setLabel("Enable Client Operation Protection Feature")
    drvMiimOpsProtectEnable.setVisible(True)
    drvMiimOpsProtectEnable.setDescription("Enable Client Operation Protection Feature")
    drvMiimOpsProtectEnable.setDefaultValue(False)
    #drvMiimOpsProtectEnable.setDependencies(drvMiimMenuVisibleSingle, ["DRV_MIIM_USE_DRIVER"])
    
    # Enable MIIM Commands
    drvMiimCmdEnable = drvMiimComponent.createBooleanSymbol("DRV_MIIM_COMMANDS", None) 
    drvMiimCmdEnable.setLabel("Enable MIIM Commands")
    drvMiimCmdEnable.setVisible(True)
    drvMiimCmdEnable.setDescription("Enable MIIM Commands")
    drvMiimCmdEnable.setDefaultValue(False)
    #drvMiimCmdEnable.setDependencies(drvMiimMenuVisibleSingle, ["DRV_MIIM_USE_DRIVER"])
    
    # MIIM Driver Object
    drvMiimDrvObj = drvMiimComponent.createStringSymbol("DRV_MIIM_DRIVER_OBJECT", None) 
    drvMiimDrvObj.setLabel("MIIM Driver Object")
    drvMiimDrvObj.setVisible(True)
    drvMiimDrvObj.setDescription("MIIM Driver Object")
    drvMiimDrvObj.setDefaultValue("DRV_MIIM_OBJECT_BASE_Default")
    #drvMiimDrvObj.setDependencies(drvMiimMenuVisibleSingle, ["DRV_MIIM_USE_DRIVER"])   

    # MIIM Driver Object Index
    drvMiimDrvIndex = drvMiimComponent.createIntegerSymbol("DRV_MIIM_DRIVER_INDEX", None)
    drvMiimDrvIndex.setLabel("MIIM Driver Object Index")
    drvMiimDrvIndex.setVisible(True)
    drvMiimDrvIndex.setDescription("MIIM Driver Object Index")
    drvMiimDrvIndex.setDefaultValue(0)
    #drvMiimDrvIndex.setDependencies(drvMiimMenuVisibleSingle, ["DRV_MIIM_USE_DRIVER"])
    
    # RTOS Configuration
    drvMiimRtosMenu = drvMiimComponent.createMenuSymbol("DRV_MIIM_RTOS_MENU", None)
    drvMiimRtosMenu.setLabel("RTOS Configuration")
    drvMiimRtosMenu.setDescription("RTOS Configuration")
    drvMiimRtosMenu.setVisible(False)
    drvMiimRtosMenu.setVisible((Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != "BareMetal") and (Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != None))
    drvMiimRtosMenu.setDependencies(drvMiimshowRTOSMenu, ["HarmonyCore.SELECT_RTOS"])
    
    # MIIM Driver Execution mode
    drvMiimInstnExecMode = drvMiimComponent.createComboSymbol("DRV_MIIM_RTOS", drvMiimRtosMenu, ["Standalone"]) 
    drvMiimInstnExecMode.setLabel("Run this driver instance as")
    drvMiimInstnExecMode.setVisible(False)
    drvMiimInstnExecMode.setDescription("MIIM Driver Execution mode")
    drvMiimInstnExecMode.setDefaultValue("Standalone")
    #drvMiimInstnExecMode.setDependencies(drvMiimMenuVisibleSingle, ["DRV_MIIM_USE_DRIVER"])

    # MIIM Driver Task Stack Size
    drvMiimDrvTaskSize = drvMiimComponent.createIntegerSymbol("DRV_MIIM_RTOS_STACK_SIZE", drvMiimRtosMenu)
    drvMiimDrvTaskSize.setLabel("Stack Size (in bytes)")
    drvMiimDrvTaskSize.setVisible(True)
    drvMiimDrvTaskSize.setDescription("MIIM Driver Task Stack Size")
    drvMiimDrvTaskSize.setDefaultValue(4096)
    drvMiimDrvTaskSize.setDependencies(drvMiimRTOSStandaloneMenu, ["DRV_MIIM_RTOS"])
    

    # MIIM Driver Task Priority
    drvMiimDrvTaskPriority = drvMiimComponent.createIntegerSymbol("DRV_MIIM_RTOS_TASK_PRIORITY", drvMiimRtosMenu)
    drvMiimDrvTaskPriority.setLabel("Task Priority")
    drvMiimDrvTaskPriority.setVisible(True)
    drvMiimDrvTaskPriority.setDescription("MIIM Driver Task Priority")
    drvMiimDrvTaskPriority.setDefaultValue(1)
    drvMiimDrvTaskPriority.setDependencies(drvMiimRTOSStandaloneMenu, ["DRV_MIIM_RTOS"])
    

    # MIIM Use Task Delay?
    drvMiimUseTaskDelay = drvMiimComponent.createBooleanSymbol("DRV_MIIM_RTOS_USE_DELAY", drvMiimRtosMenu)
    drvMiimUseTaskDelay.setLabel("Use Task Delay?")
    drvMiimUseTaskDelay.setVisible(True)
    drvMiimUseTaskDelay.setDescription("MIIM Use Task Delay?")
    drvMiimUseTaskDelay.setDefaultValue(True)
    drvMiimUseTaskDelay.setDependencies(drvMiimRTOSStandaloneMenu, ["DRV_MIIM_RTOS"])
    

    # MIIM Driver Task Delay
    drvMiimDrvTaskDelay = drvMiimComponent.createIntegerSymbol("DRV_MIIM_RTOS_DELAY", drvMiimRtosMenu)
    drvMiimDrvTaskDelay.setLabel("Task Delay")
    drvMiimDrvTaskDelay.setVisible(True)
    drvMiimDrvTaskDelay.setDescription("MIIM Driver Task Delay")
    drvMiimDrvTaskDelay.setDefaultValue(1)
    drvMiimDrvTaskDelay.setDependencies(drvMiimRTOSTaskDelayMenu, ["DRV_MIIM_RTOS", "DRV_MIIM_RTOS_USE_DELAY"])
        
    # Add drv_miim.h file to project
    #file DRV_MIIM_H "$HARMONY_VERSION_PATH/framework/driver/miim/drv_miim.h" to "$PROJECT_HEADER_FILES/framework/driver/miim/drv_miim.h"
    drvMiimHeaderFile = drvMiimComponent.createFileSymbol(None, None)
    drvMiimHeaderFile.setSourcePath("driver/miim/drv_miim.h")
    drvMiimHeaderFile.setOutputName("drv_miim.h")
    drvMiimHeaderFile.setDestPath("driver/miim/")
    drvMiimHeaderFile.setProjectPath("config/" + configName + "/driver/miim/")
    drvMiimHeaderFile.setType("HEADER")
    drvMiimHeaderFile.setOverwrite(True)
    
    #file DRV_MIIM_LOCAL_H "$HARMONY_VERSION_PATH/framework/driver/miim/src/drv_miim_local.h" to "$PROJECT_HEADER_FILES/framework/driver/miim/src/drv_miim_local.h"
    drvMiimLocalHeaderFile = drvMiimComponent.createFileSymbol(None, None)
    drvMiimLocalHeaderFile.setSourcePath("driver/miim/src/drv_miim_local.h")
    drvMiimLocalHeaderFile.setOutputName("drv_miim_local.h")
    drvMiimLocalHeaderFile.setDestPath("driver/miim/src/")
    drvMiimLocalHeaderFile.setProjectPath("config/" + configName + "/driver/miim/src/")
    drvMiimLocalHeaderFile.setType("HEADER")
    drvMiimLocalHeaderFile.setOverwrite(True)
    
    #add "<#include \"/framework/driver/miim/config/drv_miim_mapping.h.ftl\">"  to "$PROJECT_HEADER_FILES/framework/driver/miim/src/dynamic/drv_miim_mapping.h"
    drvMiimMappingHeaderFtl = drvMiimComponent.createFileSymbol(None, None)
    drvMiimMappingHeaderFtl.setSourcePath("driver/miim/config/drv_miim_mapping.h.ftl")
    drvMiimMappingHeaderFtl.setOutputName("drv_miim_mapping.h")
    drvMiimMappingHeaderFtl.setDestPath("driver/miim/src/dynamic/")
    drvMiimMappingHeaderFtl.setProjectPath("config/" + configName + "/driver/miim/src/dynamic/")
    drvMiimMappingHeaderFtl.setType("HEADER")
    drvMiimMappingHeaderFtl.setMarkup(True)
    
    #file DRV_MIIM_C "$HARMONY_VERSION_PATH/framework/driver/miim/src/dynamic/drv_miim.c" to "$PROJECT_SOURCE_FILES/framework/driver/miim/src/dynamic/drv_miim.c"
    drvMiimSourceFile = drvMiimComponent.createFileSymbol(None, None)
    drvMiimSourceFile.setSourcePath("driver/miim/src/dynamic/drv_miim.c")
    drvMiimSourceFile.setOutputName("drv_miim.c")
    drvMiimSourceFile.setOverwrite(True)
    drvMiimSourceFile.setDestPath("driver/miim/src/dynamic/")
    drvMiimSourceFile.setProjectPath("config/" + configName + "/driver/miim/src/dynamic/")
    drvMiimSourceFile.setType("SOURCE")
    drvMiimSourceFile.setEnabled(True)
    #drvMiimSourceFile.setDependencies(drvMiimGenSourceFile, ["DRV_MIIM_USE_DRIVER"])       

    #add "<#include \"/framework/driver/miim/config/drv_miim.c.ftl\">"  to list SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA
    drvMiimInitDataSourceFtl = drvMiimComponent.createFileSymbol(None, None)
    drvMiimInitDataSourceFtl.setType("STRING")
    drvMiimInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    drvMiimInitDataSourceFtl.setSourcePath("driver/miim/config/drv_miim.c.ftl")
    drvMiimInitDataSourceFtl.setMarkup(True)

    #add "<#include \"/framework/driver/miim/config/drv_miim_init.c.ftl\">"  to list SYSTEM_INIT_C_SYS_INITIALIZE_DRIVERS
    drvMiimInitSourceFtl = drvMiimComponent.createFileSymbol(None, None)
    drvMiimInitSourceFtl.setType("STRING")
    drvMiimInitSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_SYS_INITIALIZE_DRIVERS")
    drvMiimInitSourceFtl.setSourcePath("driver/miim/config/drv_miim_init.c.ftl")
    drvMiimInitSourceFtl.setMarkup(True)
    
    #add "<#include \"/framework/driver/miim/config/drv_miim_tasks.c.ftl\">"  to list SYSTEM_TASKS_C_CALL_DRIVER_TASKS
    # drvMiimSysTaskSourceFtl = drvMiimComponent.createFileSymbol(None, None)
    # drvMiimSysTaskSourceFtl.setType("STRING")
    # drvMiimSysTaskSourceFtl.setOutputName("core.LIST_SYSTEM_TASKS_C_CALL_DRIVER_TASKS")
    # drvMiimSysTaskSourceFtl.setSourcePath("driver/miim/config/drv_miim_tasks.c.ftl")
    # drvMiimSysTaskSourceFtl.setMarkup(True)
    
    #add "<#include \"/framework/driver/miim/config/drv_miim.h.ftl\">"  to list SYSTEM_CONFIG_H_DRIVER_CONFIGURATION 
    drvMiimSysConfigSourceFtl = drvMiimComponent.createFileSymbol(None, None)
    drvMiimSysConfigSourceFtl.setType("STRING")
    drvMiimSysConfigSourceFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_DRIVER_CONFIGURATION")
    drvMiimSysConfigSourceFtl.setSourcePath("driver/miim/config/drv_miim.h.ftl")
    drvMiimSysConfigSourceFtl.setMarkup(True)

    # drvMiimSystemDefFile = drvMiimComponent.createFileSymbol("MIIM_H_FILE", None)
    # drvMiimSystemDefFile.setType("STRING")
    # drvMiimSystemDefFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    # drvMiimSystemDefFile.setSourcePath("driver/miim/templates/system/system_definitions.h.ftl")
    # drvMiimSystemDefFile.setMarkup(True)  

    drvMiimSystemDefObjFile = drvMiimComponent.createFileSymbol("DRV_MIIM_DEF_OBJ", None)
    drvMiimSystemDefObjFile.setType("STRING")
    drvMiimSystemDefObjFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_OBJECTS")
    drvMiimSystemDefObjFile.setSourcePath("driver/miim/templates/system/system_definitions_objects.h.ftl")
    drvMiimSystemDefObjFile.setMarkup(True) 

    drvMiimSystemConfFile = drvMiimComponent.createFileSymbol("DRV_MIIM_CONFIGURATION_H", None)
    drvMiimSystemConfFile.setType("STRING")
    drvMiimSystemConfFile.setOutputName("core.LIST_SYSTEM_CONFIG_H_DRIVER_CONFIGURATION")
    drvMiimSystemConfFile.setSourcePath("driver/miim/templates/system/system_config.h.ftl")
    drvMiimSystemConfFile.setMarkup(True)   

    drvMiimSystemTaskFile = drvMiimComponent.createFileSymbol("DRV_SDHC_SYSTEM_TASKS_C", None)
    drvMiimSystemTaskFile.setType("STRING")
    drvMiimSystemTaskFile.setOutputName("core.LIST_SYSTEM_TASKS_C_CALL_DRIVER_TASKS")
    drvMiimSystemTaskFile.setSourcePath("driver/miim/templates/system/system_tasks.c.ftl")
    drvMiimSystemTaskFile.setMarkup(True)

    drvMiimSystemRtosTasksFile = drvMiimComponent.createFileSymbol("DRV_SDHC_SYS_RTOS_TASK", None)
    drvMiimSystemRtosTasksFile.setType("STRING")
    drvMiimSystemRtosTasksFile.setOutputName("core.LIST_SYSTEM_RTOS_TASKS_C_DEFINITIONS")
    drvMiimSystemRtosTasksFile.setSourcePath("driver/miim/templates/system/system_rtos_tasks.c.ftl")
    drvMiimSystemRtosTasksFile.setMarkup(True)
    drvMiimSystemRtosTasksFile.setEnabled((Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != "BareMetal"))
    drvMiimSystemRtosTasksFile.setDependencies(genRtosTask, ["HarmonyCore.SELECT_RTOS"])
    
def drvMiimMenuVisibleSingle(symbol, event):
    if (event["value"] == True):
        print("MIIM Menu Visible.")     
        symbol.setVisible(True)
    else:
        print("MIIM Menu Invisible.")
        symbol.setVisible(False)
        

def drvMiimshowRTOSMenu(symbol, event):

    if (event["value"] == None):
        symbol.setVisible(False)
        print("MIIM: OSAL Disabled")
    elif (event["value"] != "BareMetal"):
        # If not Bare Metal
        symbol.setVisible(True)
        print("MIIM rtos")
    else:
        symbol.setVisible(False)
        print("MIIM Bare Metal")
        
def drvMiimRTOSStandaloneMenu(symbol, event):
    if (event["value"] == 'Standalone'):        
        symbol.setVisible(True)
        print("MIIM Standalone")
    else:
        symbol.setVisible(False)
        print("MIIM Combined")
        
def drvMiimRTOSTaskDelayMenu(symbol, event):
    drvMiimRtos = Database.getSymbolValue("drvMiim","DRV_MIIM_RTOS")
    drvMiimRtosUseDelay = Database.getSymbolValue("drvMiim","DRV_MIIM_RTOS_USE_DELAY")
    if((drvMiimRtos == 'Standalone') and drvMiimRtosUseDelay):      
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)
        
def drvMiimSetIntMacId(symbol, event):
    if((Database.getSymbolValue("drvGmac", "TCPIP_USE_ETH_MAC") == True)): 
        symbol.setValue("GMAC_BASE_ADDRESS")
    elif ((Database.getSymbolValue("drvPic32mEthmac", "TCPIP_USE_ETH_MAC") == True)):
        symbol.setValue("_ETH_BASE_ADDRESS")
    elif ((Database.getSymbolValue("drvEmac0", "TCPIP_USE_EMAC0") == True)):    
        symbol.setValue("EMAC0_BASE_ADDRESS")
    elif ((Database.getSymbolValue("drvEmac1", "TCPIP_USE_EMAC1") == True)):    
        symbol.setValue("EMAC1_BASE_ADDRESS")    

def genRtosTask(symbol, event):
    symbol.setEnabled((Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != "BareMetal"))
    
def drvMiimGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])

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