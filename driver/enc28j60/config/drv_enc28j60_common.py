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

def instantiateComponent(drvExtMacEnc28j60CommonComponent):
    print("Exernal Ethernet Controller ENC28J60 Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")      
            
    # Enable ENC28J60
    drvEnc28j60Module = drvExtMacEnc28j60CommonComponent.createBooleanSymbol("DRV_ENC28J60_USE_DRIVER", None)
    drvEnc28j60Module.setLabel("Enable ENC28J60")
    drvEnc28j60Module.setVisible(False)
    drvEnc28j60Module.setDescription("Enable ENC28J60")
    drvEnc28j60Module.setDefaultValue(True)    

    drvEnc28j60Mode = drvExtMacEnc28j60CommonComponent.createKeyValueSetSymbol("DRV_ENC28J60_MODE", None)
    drvEnc28j60Mode.setVisible(True)
    drvEnc28j60Mode.setLabel("Driver Mode")
    drvEnc28j60Mode.addKey("DYNAMIC", "0", "Dynamic")
    drvEnc28j60Mode.setDisplayMode("Key")
    drvEnc28j60Mode.setOutputMode("Key")
    drvEnc28j60Mode.setDefaultValue(0)
    
    # add "<#include \"/framework/driver/enc28j60/config/drv_enc28j60.c.ftl\">"  to list SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA
    drvEnc28j60InitDataSourceFtl = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60InitDataSourceFtl.setType("STRING")
    drvEnc28j60InitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    drvEnc28j60InitDataSourceFtl.setSourcePath("driver/enc28j60/config/drv_enc28j60.c.ftl")
    drvEnc28j60InitDataSourceFtl.setMarkup(True)        
    
    drvEnc28j60SystemDefFile = drvExtMacEnc28j60CommonComponent.createFileSymbol("DRV_ENC28J60_H_FILE", None)
    drvEnc28j60SystemDefFile.setType("STRING")
    drvEnc28j60SystemDefFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    drvEnc28j60SystemDefFile.setSourcePath("driver/enc28j60/templates/system/system_definitions.h.ftl")
    drvEnc28j60SystemDefFile.setMarkup(True)  
    
    # add "<#include \"/framework/driver/enc28j60/config/drv_enc28j60.h.ftl\">"  to list SYSTEM_CONFIG_H_DRIVER_CONFIGURATION 
    drvEnc28j60SysConfigSourceFtl = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60SysConfigSourceFtl.setType("STRING")
    drvEnc28j60SysConfigSourceFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_DRIVER_CONFIGURATION")
    drvEnc28j60SysConfigSourceFtl.setSourcePath("driver/enc28j60/config/drv_enc28j60.h.ftl")
    drvEnc28j60SysConfigSourceFtl.setMarkup(True)
    
    drvEnc28j60SystemDefObjFile = drvExtMacEnc28j60CommonComponent.createFileSymbol("DRV_ENC28J60_DEF_OBJ", None)
    drvEnc28j60SystemDefObjFile.setType("STRING")
    drvEnc28j60SystemDefObjFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_OBJECTS")
    drvEnc28j60SystemDefObjFile.setSourcePath("driver/enc28j60/templates/system/system_definitions_objects.h.ftl")
    drvEnc28j60SystemDefObjFile.setMarkup(True) 

    drvEnc28j60SystemTaskFile = drvExtMacEnc28j60CommonComponent.createFileSymbol("DRV_ENC28J60_TASKS_C", None)
    drvEnc28j60SystemTaskFile.setType("STRING")
    drvEnc28j60SystemTaskFile.setOutputName("core.LIST_SYSTEM_TASKS_C_CALL_DRIVER_TASKS")
    drvEnc28j60SystemTaskFile.setSourcePath("driver/enc28j60/templates/system/system_tasks.c.ftl")
    drvEnc28j60SystemTaskFile.setMarkup(True) 
        
    # file DRV_ENC28J60_H "$HARMONY_VERSION_PATH/framework/driver/enc28j60/drv_enc28j60.h" to "$PROJECT_HEADER_FILES/framework/driver/enc28j60/drv_enc28j60.h"
    drvEnc28j60HeaderFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60HeaderFile.setSourcePath("driver/enc28j60/drv_enc28j60.h")
    drvEnc28j60HeaderFile.setOutputName("drv_enc28j60.h")
    drvEnc28j60HeaderFile.setDestPath("driver/enc28j60/")
    drvEnc28j60HeaderFile.setProjectPath("config/" + configName + "/driver/enc28j60/")
    drvEnc28j60HeaderFile.setType("HEADER")
    drvEnc28j60HeaderFile.setOverwrite(True)
    
    # file DRV_ENC28J60_API_C "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/drv_enc28j60_api.c" to "$PROJECT_SOURCE_FILES/framework/driver/enc28j60/drv_enc28j60_api.c"
    drvEnc28j60ApiSourceFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60ApiSourceFile.setSourcePath("driver/enc28j60/src/dynamic/drv_enc28j60_api.c")
    drvEnc28j60ApiSourceFile.setOutputName("drv_enc28j60_api.c")
    drvEnc28j60ApiSourceFile.setOverwrite(True)
    drvEnc28j60ApiSourceFile.setDestPath("driver/enc28j60/src/dynamic/")
    drvEnc28j60ApiSourceFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/")
    drvEnc28j60ApiSourceFile.setType("SOURCE")
    drvEnc28j60ApiSourceFile.setEnabled(True)
    
    # file DRV_ENC28J60_DS_DEFS_H "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/drv_enc28j60_ds_defs.h" to "$PROJECT_HEADER_FILES/framework/driver/enc28j60/impl/drv_enc28j60_ds_defs.h"
    drvEnc28j60DsDefHeaderFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60DsDefHeaderFile.setSourcePath("driver/enc28j60/src/dynamic/drv_enc28j60_ds_defs.h")
    drvEnc28j60DsDefHeaderFile.setOutputName("drv_enc28j60_ds_defs.h")
    drvEnc28j60DsDefHeaderFile.setDestPath("driver/enc28j60/src/dynamic/")
    drvEnc28j60DsDefHeaderFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/")
    drvEnc28j60DsDefHeaderFile.setType("HEADER")
    drvEnc28j60DsDefHeaderFile.setOverwrite(True)    
    
    # file DRV_ENC28J60_LOCAL_H "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/drv_enc28j60_local.h" to "$PROJECT_HEADER_FILES/framework/driver/enc28j60/impl/drv_enc28j60_local.h"
    drvEnc28j60LocalHeaderFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60LocalHeaderFile.setSourcePath("driver/enc28j60/src/dynamic/drv_enc28j60_local.h")
    drvEnc28j60LocalHeaderFile.setOutputName("drv_enc28j60_local.h")
    drvEnc28j60LocalHeaderFile.setDestPath("driver/enc28j60/src/dynamic/")
    drvEnc28j60LocalHeaderFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/")
    drvEnc28j60LocalHeaderFile.setType("HEADER")
    drvEnc28j60LocalHeaderFile.setOverwrite(True) 
    
    # file DRV_ENC28J60_MAIN_STATE_C "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/drv_enc28j60_main_state.c" to "$PROJECT_SOURCE_FILES/framework/driver/enc28j60/drv_enc28j60_main_state.c"
    drvEnc28j60MainStateSourceFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60MainStateSourceFile.setSourcePath("driver/enc28j60/src/dynamic/drv_enc28j60_main_state.c")
    drvEnc28j60MainStateSourceFile.setOutputName("drv_enc28j60_main_state.c")
    drvEnc28j60MainStateSourceFile.setOverwrite(True)
    drvEnc28j60MainStateSourceFile.setDestPath("driver/enc28j60/src/dynamic/")
    drvEnc28j60MainStateSourceFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/")
    drvEnc28j60MainStateSourceFile.setType("SOURCE")
    drvEnc28j60MainStateSourceFile.setEnabled(True)
    
    # file DRV_ENC28J60_MAIN_STATE_H "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/drv_enc28j60_main_state.h" to "$PROJECT_HEADER_FILES/framework/driver/enc28j60/impl/drv_enc28j60_main_state.h"
    drvEnc28j60MainStateHeaderFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60MainStateHeaderFile.setSourcePath("driver/enc28j60/src/dynamic/drv_enc28j60_main_state.h")
    drvEnc28j60MainStateHeaderFile.setOutputName("drv_enc28j60_main_state.h")
    drvEnc28j60MainStateHeaderFile.setDestPath("driver/enc28j60/src/dynamic/")
    drvEnc28j60MainStateHeaderFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/")
    drvEnc28j60MainStateHeaderFile.setType("HEADER")
    drvEnc28j60MainStateHeaderFile.setOverwrite(True) 
    
    # file DRV_ENC28J60_UTILS_C "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/drv_enc28j60_utils.c" to "$PROJECT_SOURCE_FILES/framework/driver/enc28j60/drv_enc28j60_utils.c"
    drvEnc28j60UtilsSourceFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60UtilsSourceFile.setSourcePath("driver/enc28j60/src/dynamic/drv_enc28j60_utils.c")
    drvEnc28j60UtilsSourceFile.setOutputName("drv_enc28j60_utils.c")
    drvEnc28j60UtilsSourceFile.setOverwrite(True)
    drvEnc28j60UtilsSourceFile.setDestPath("driver/enc28j60/src/dynamic/")
    drvEnc28j60UtilsSourceFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/")
    drvEnc28j60UtilsSourceFile.setType("SOURCE")
    drvEnc28j60UtilsSourceFile.setEnabled(True)
    
    # file DRV_ENC28J60_UTILS_H "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/drv_enc28j60_utils.h" to "$PROJECT_HEADER_FILES/framework/driver/enc28j60/impl/drv_enc28j60_utils.h"
    drvEnc28j60UtilsHeaderFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60UtilsHeaderFile.setSourcePath("driver/enc28j60/src/dynamic/drv_enc28j60_utils.h")
    drvEnc28j60UtilsHeaderFile.setOutputName("drv_enc28j60_utils.h")
    drvEnc28j60UtilsHeaderFile.setDestPath("driver/enc28j60/src/dynamic/")
    drvEnc28j60UtilsHeaderFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/")
    drvEnc28j60UtilsHeaderFile.setType("HEADER")
    drvEnc28j60UtilsHeaderFile.setOverwrite(True) 
    
    # file DRV_ENC28J60_BUS_H "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/bus/drv_enc28j60_bus.h" to "$PROJECT_HEADER_FILES/framework/driver/enc28j60/impl/bus/drv_enc28j60_bus.h"
    drvEnc28j60BusHeaderFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60BusHeaderFile.setSourcePath("driver/enc28j60/src/dynamic/bus/drv_enc28j60_bus.h")
    drvEnc28j60BusHeaderFile.setOutputName("drv_enc28j60_bus.h")
    drvEnc28j60BusHeaderFile.setDestPath("driver/enc28j60/src/dynamic/bus/")
    drvEnc28j60BusHeaderFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/bus/")
    drvEnc28j60BusHeaderFile.setType("HEADER")
    drvEnc28j60BusHeaderFile.setOverwrite(True) 
    
    # file DRV_ENC28J60_SPI_BUS_C "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/bus/spi/drv_enc28j60_spi_bus.c" to "$PROJECT_SOURCE_FILES/framework/driver/enc28j60/bus/spi/drv_enc28j60_spi_bus.c"
    drvEnc28j60SpiBusSourceFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60SpiBusSourceFile.setSourcePath("driver/enc28j60/src/dynamic/bus/spi/drv_enc28j60_spi_bus.c")
    drvEnc28j60SpiBusSourceFile.setOutputName("drv_enc28j60_spi_bus.c")
    drvEnc28j60SpiBusSourceFile.setOverwrite(True)
    drvEnc28j60SpiBusSourceFile.setDestPath("driver/enc28j60/src/dynamic/bus/spi/")
    drvEnc28j60SpiBusSourceFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/bus/spi/")
    drvEnc28j60SpiBusSourceFile.setType("SOURCE")
    drvEnc28j60SpiBusSourceFile.setEnabled(True)
    
    # file DRV_ENC28J60_SPI_BUS_H "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/bus/spi/drv_enc28j60_spi_bus.h" to "$PROJECT_HEADER_FILES/framework/driver/enc28j60/impl/bus/spi/drv_enc28j60_spi_bus.h"
    drvEnc28j60SpiBusHeaderFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60SpiBusHeaderFile.setSourcePath("driver/enc28j60/src/dynamic/bus/spi/drv_enc28j60_spi_bus.h")
    drvEnc28j60SpiBusHeaderFile.setOutputName("drv_enc28j60_spi_bus.h")
    drvEnc28j60SpiBusHeaderFile.setDestPath("driver/enc28j60/src/dynamic/bus/spi/")
    drvEnc28j60SpiBusHeaderFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/bus/spi/")
    drvEnc28j60SpiBusHeaderFile.setType("HEADER")
    drvEnc28j60SpiBusHeaderFile.setOverwrite(True) 
    
    # file DRV_ENC28J60_CLOSED_STATE_C "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/closed_state/drv_enc28j60_closed_state.c" to "$PROJECT_SOURCE_FILES/framework/driver/enc28j60/closed_state/drv_enc28j60_closed_state.c"
    drvEnc28j60CloseStateSourceFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60CloseStateSourceFile.setSourcePath("driver/enc28j60/src/dynamic/closed_state/drv_enc28j60_closed_state.c")
    drvEnc28j60CloseStateSourceFile.setOutputName("drv_enc28j60_closed_state.c")
    drvEnc28j60CloseStateSourceFile.setOverwrite(True)
    drvEnc28j60CloseStateSourceFile.setDestPath("driver/enc28j60/src/dynamic/closed_state/")
    drvEnc28j60CloseStateSourceFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/closed_state/")
    drvEnc28j60CloseStateSourceFile.setType("SOURCE")
    drvEnc28j60CloseStateSourceFile.setEnabled(True)
    
    # file DRV_ENC28J60_CLOSED_STATE_H "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/closed_state/drv_enc28j60_closed_state.h" to "$PROJECT_HEADER_FILES/framework/driver/enc28j60/impl/closed_state/drv_enc28j60_closed_state.h"
    drvEnc28j60CloseStateHeaderFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60CloseStateHeaderFile.setSourcePath("driver/enc28j60/src/dynamic/closed_state/drv_enc28j60_closed_state.h")
    drvEnc28j60CloseStateHeaderFile.setOutputName("drv_enc28j60_closed_state.h")
    drvEnc28j60CloseStateHeaderFile.setDestPath("driver/enc28j60/src/dynamic/closed_state/")
    drvEnc28j60CloseStateHeaderFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/closed_state/")
    drvEnc28j60CloseStateHeaderFile.setType("HEADER")
    drvEnc28j60CloseStateHeaderFile.setOverwrite(True) 
    
    # file DRV_ENC28J60_INIT_STATE_C "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/initialization_state/drv_enc28j60_initialization_state.c" to "$PROJECT_SOURCE_FILES/framework/driver/enc28j60/initialization_state/drv_enc28j60_initialization_state.c"
    drvEnc28j60InitStateSourceFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60InitStateSourceFile.setSourcePath("driver/enc28j60/src/dynamic/initialization_state/drv_enc28j60_initialization_state.c")
    drvEnc28j60InitStateSourceFile.setOutputName("drv_enc28j60_initialization_state.c")
    drvEnc28j60InitStateSourceFile.setOverwrite(True)
    drvEnc28j60InitStateSourceFile.setDestPath("driver/enc28j60/src/dynamic/initialization_state/")
    drvEnc28j60InitStateSourceFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/initialization_state/")
    drvEnc28j60InitStateSourceFile.setType("SOURCE")
    drvEnc28j60InitStateSourceFile.setEnabled(True)
    
    # file DRV_ENC28J60_INIT_STATE_H "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/initialization_state/drv_enc28j60_initialization_state.h" to "$PROJECT_HEADER_FILES/framework/driver/enc28j60/impl/initialization_state/drv_enc28j60_initialization_state.h"
    drvEnc28j60InitStateHeaderFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60InitStateHeaderFile.setSourcePath("driver/enc28j60/src/dynamic/initialization_state/drv_enc28j60_initialization_state.h")
    drvEnc28j60InitStateHeaderFile.setOutputName("drv_enc28j60_initialization_state.h")
    drvEnc28j60InitStateHeaderFile.setDestPath("driver/enc28j60/src/dynamic/initialization_state/")
    drvEnc28j60InitStateHeaderFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/initialization_state/")
    drvEnc28j60InitStateHeaderFile.setType("HEADER")
    drvEnc28j60InitStateHeaderFile.setOverwrite(True)
    
    # file DRV_ENC28J60_CONFIG_STATE_C "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/initialization_state/drv_enc28j60_configure_state.c" to "$PROJECT_SOURCE_FILES/framework/driver/enc28j60/initialization_state/drv_enc28j60_configure_state.c"
    drvEnc28j60ConfigStateSourceFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60ConfigStateSourceFile.setSourcePath("driver/enc28j60/src/dynamic/initialization_state/drv_enc28j60_configure_state.c")
    drvEnc28j60ConfigStateSourceFile.setOutputName("drv_enc28j60_configure_state.c")
    drvEnc28j60ConfigStateSourceFile.setOverwrite(True)
    drvEnc28j60ConfigStateSourceFile.setDestPath("driver/enc28j60/src/dynamic/initialization_state/")
    drvEnc28j60ConfigStateSourceFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/initialization_state/")
    drvEnc28j60ConfigStateSourceFile.setType("SOURCE")
    drvEnc28j60ConfigStateSourceFile.setEnabled(True)
    
    # file DRV_ENC28J60_CONFIG_STATE_H "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/initialization_state/drv_enc28j60_configure_state.h" to "$PROJECT_HEADER_FILES/framework/driver/enc28j60/impl/initialization_state/drv_enc28j60_configure_state.h"
    drvEnc28j60ConfigStateHeaderFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60ConfigStateHeaderFile.setSourcePath("driver/enc28j60/src/dynamic/initialization_state/drv_enc28j60_configure_state.h")
    drvEnc28j60ConfigStateHeaderFile.setOutputName("drv_enc28j60_configure_state.h")
    drvEnc28j60ConfigStateHeaderFile.setDestPath("driver/enc28j60/src/dynamic/initialization_state/")
    drvEnc28j60ConfigStateHeaderFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/initialization_state/")
    drvEnc28j60ConfigStateHeaderFile.setType("HEADER")
    drvEnc28j60ConfigStateHeaderFile.setOverwrite(True)
    
    # file DRV_ENC28J60_DETECT_STATE_C "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/initialization_state/drv_enc28j60_detect_state.c" to "$PROJECT_SOURCE_FILES/framework/driver/enc28j60/initialization_state/drv_enc28j60_detect_state.c"
    drvEnc28j60DetectStateSourceFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60DetectStateSourceFile.setSourcePath("driver/enc28j60/src/dynamic/initialization_state/drv_enc28j60_detect_state.c")
    drvEnc28j60DetectStateSourceFile.setOutputName("drv_enc28j60_detect_state.c")
    drvEnc28j60DetectStateSourceFile.setOverwrite(True)
    drvEnc28j60DetectStateSourceFile.setDestPath("driver/enc28j60/src/dynamic/initialization_state/")
    drvEnc28j60DetectStateSourceFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/initialization_state/")
    drvEnc28j60DetectStateSourceFile.setType("SOURCE")
    drvEnc28j60DetectStateSourceFile.setEnabled(True)
    
    # file DRV_ENC28J60_DETECT_STATE_H "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/initialization_state/drv_enc28j60_detect_state.h" to "$PROJECT_HEADER_FILES/framework/driver/enc28j60/impl/initialization_state/drv_enc28j60_detect_state.h"
    drvEnc28j60DetectStateHeaderFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60DetectStateHeaderFile.setSourcePath("driver/enc28j60/src/dynamic/initialization_state/drv_enc28j60_detect_state.h")
    drvEnc28j60DetectStateHeaderFile.setOutputName("drv_enc28j60_detect_state.h")
    drvEnc28j60DetectStateHeaderFile.setDestPath("driver/enc28j60/src/dynamic/initialization_state/")
    drvEnc28j60DetectStateHeaderFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/initialization_state/")
    drvEnc28j60DetectStateHeaderFile.setType("HEADER")
    drvEnc28j60DetectStateHeaderFile.setOverwrite(True)
    
    # file DRV_ENC28J60_RESET_STATE_C "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/initialization_state/drv_enc28j60_reset_state.c" to "$PROJECT_SOURCE_FILES/framework/driver/enc28j60/initialization_state/drv_enc28j60_reset_state.c"
    drvEnc28j60ResetStateSourceFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60ResetStateSourceFile.setSourcePath("driver/enc28j60/src/dynamic/initialization_state/drv_enc28j60_reset_state.c")
    drvEnc28j60ResetStateSourceFile.setOutputName("drv_enc28j60_reset_state.c")
    drvEnc28j60ResetStateSourceFile.setOverwrite(True)
    drvEnc28j60ResetStateSourceFile.setDestPath("driver/enc28j60/src/dynamic/initialization_state/")
    drvEnc28j60ResetStateSourceFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/initialization_state/")
    drvEnc28j60ResetStateSourceFile.setType("SOURCE")
    drvEnc28j60ResetStateSourceFile.setEnabled(True)
    
    # file DRV_ENC28J60_RESET_STATE_H "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/initialization_state/drv_enc28j60_reset_state.h" to "$PROJECT_HEADER_FILES/framework/driver/enc28j60/impl/initialization_state/drv_enc28j60_reset_state.h"
    drvEnc28j60ResetStateHeaderFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60ResetStateHeaderFile.setSourcePath("driver/enc28j60/src/dynamic/initialization_state/drv_enc28j60_reset_state.h")
    drvEnc28j60ResetStateHeaderFile.setOutputName("drv_enc28j60_reset_state.h")
    drvEnc28j60ResetStateHeaderFile.setDestPath("driver/enc28j60/src/dynamic/initialization_state/")
    drvEnc28j60ResetStateHeaderFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/initialization_state/")
    drvEnc28j60ResetStateHeaderFile.setType("HEADER")
    drvEnc28j60ResetStateHeaderFile.setOverwrite(True)
    
    # file DRV_ENC28J60_TX_PACKET_C "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/packet/drv_enc28j60_tx_packet.c" to "$PROJECT_SOURCE_FILES/framework/driver/enc28j60/packet/drv_enc28j60_tx_packet.c"
    drvEnc28j60TxPktSourceFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60TxPktSourceFile.setSourcePath("driver/enc28j60/src/dynamic/packet/drv_enc28j60_tx_packet.c")
    drvEnc28j60TxPktSourceFile.setOutputName("drv_enc28j60_tx_packet.c")
    drvEnc28j60TxPktSourceFile.setOverwrite(True)
    drvEnc28j60TxPktSourceFile.setDestPath("driver/enc28j60/src/dynamic/packet/")
    drvEnc28j60TxPktSourceFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/packet/")
    drvEnc28j60TxPktSourceFile.setType("SOURCE")
    drvEnc28j60TxPktSourceFile.setEnabled(True)
    
    # file DRV_ENC28J60_TX_PACKET_H "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/packet/drv_enc28j60_tx_packet.h" to "$PROJECT_HEADER_FILES/framework/driver/enc28j60/impl/packet/drv_enc28j60_tx_packet.h"
    drvEnc28j60TxPktHeaderFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60TxPktHeaderFile.setSourcePath("driver/enc28j60/src/dynamic/packet/drv_enc28j60_tx_packet.h")
    drvEnc28j60TxPktHeaderFile.setOutputName("drv_enc28j60_tx_packet.h")
    drvEnc28j60TxPktHeaderFile.setDestPath("driver/enc28j60/src/dynamic/packet/")
    drvEnc28j60TxPktHeaderFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/packet/")
    drvEnc28j60TxPktHeaderFile.setType("HEADER")
    drvEnc28j60TxPktHeaderFile.setOverwrite(True)
    
    # file DRV_ENC28J60_RX_PACKET_C "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/packet/drv_enc28j60_rx_packet.c" to "$PROJECT_SOURCE_FILES/framework/driver/enc28j60/packet/drv_enc28j60_rx_packet.c"
    drvEnc28j60RxPktSourceFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60RxPktSourceFile.setSourcePath("driver/enc28j60/src/dynamic/packet/drv_enc28j60_rx_packet.c")
    drvEnc28j60RxPktSourceFile.setOutputName("drv_enc28j60_rx_packet.c")
    drvEnc28j60RxPktSourceFile.setOverwrite(True)
    drvEnc28j60RxPktSourceFile.setDestPath("driver/enc28j60/src/dynamic/packet/")
    drvEnc28j60RxPktSourceFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/packet/")
    drvEnc28j60RxPktSourceFile.setType("SOURCE")
    drvEnc28j60RxPktSourceFile.setEnabled(True)
    
    # file DRV_ENC28J60_RX_PACKET_H "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/packet/drv_enc28j60_rx_packet.h" to "$PROJECT_HEADER_FILES/framework/driver/enc28j60/impl/packet/drv_enc28j60_rx_packet.h"
    drvEnc28j60RxPktHeaderFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60RxPktHeaderFile.setSourcePath("driver/enc28j60/src/dynamic/packet/drv_enc28j60_rx_packet.h")
    drvEnc28j60RxPktHeaderFile.setOutputName("drv_enc28j60_rx_packet.h")
    drvEnc28j60RxPktHeaderFile.setDestPath("driver/enc28j60/src/dynamic/packet/")
    drvEnc28j60RxPktHeaderFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/packet/")
    drvEnc28j60RxPktHeaderFile.setType("HEADER")
    drvEnc28j60RxPktHeaderFile.setOverwrite(True)
    
    # file DRV_ENC28J60_RUNNING_STATE_C "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/running_state/drv_enc28j60_running_state.c" to "$PROJECT_SOURCE_FILES/framework/driver/enc28j60/running_state/drv_enc28j60_running_state.c"
    drvEnc28j60RunStateSourceFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60RunStateSourceFile.setSourcePath("driver/enc28j60/src/dynamic/running_state/drv_enc28j60_running_state.c")
    drvEnc28j60RunStateSourceFile.setOutputName("drv_enc28j60_running_state.c")
    drvEnc28j60RunStateSourceFile.setOverwrite(True)
    drvEnc28j60RunStateSourceFile.setDestPath("driver/enc28j60/src/dynamic/running_state/")
    drvEnc28j60RunStateSourceFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/running_state/")
    drvEnc28j60RunStateSourceFile.setType("SOURCE")
    drvEnc28j60RunStateSourceFile.setEnabled(True)
    
    # file DRV_ENC28J60_RUNNING_STATE_H "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/running_state/drv_enc28j60_running_state.h" to "$PROJECT_HEADER_FILES/framework/driver/enc28j60/impl/running_state/drv_enc28j60_running_state.h"
    drvEnc28j60RunStateHeaderFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60RunStateHeaderFile.setSourcePath("driver/enc28j60/src/dynamic/running_state/drv_enc28j60_running_state.h")
    drvEnc28j60RunStateHeaderFile.setOutputName("drv_enc28j60_running_state.h")
    drvEnc28j60RunStateHeaderFile.setDestPath("driver/enc28j60/src/dynamic/running_state/")
    drvEnc28j60RunStateHeaderFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/running_state/")
    drvEnc28j60RunStateHeaderFile.setType("HEADER")
    drvEnc28j60RunStateHeaderFile.setOverwrite(True)
    
    # file DRV_ENC28J60_CHDUP_STATE_C "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/running_state/drv_enc28j60_change_duplex_state.c" to "$PROJECT_SOURCE_FILES/framework/driver/enc28j60/running_state/drv_enc28j60_change_duplex_state.c"
    drvEnc28j60DuplexStateSourceFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60DuplexStateSourceFile.setSourcePath("driver/enc28j60/src/dynamic/running_state/drv_enc28j60_change_duplex_state.c")
    drvEnc28j60DuplexStateSourceFile.setOutputName("drv_enc28j60_change_duplex_state.c")
    drvEnc28j60DuplexStateSourceFile.setOverwrite(True)
    drvEnc28j60DuplexStateSourceFile.setDestPath("driver/enc28j60/src/dynamic/running_state/")
    drvEnc28j60DuplexStateSourceFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/running_state/")
    drvEnc28j60DuplexStateSourceFile.setType("SOURCE")
    drvEnc28j60DuplexStateSourceFile.setEnabled(True)
    
    # file DRV_ENC28J60_CHDUP_STATE_H "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/running_state/drv_enc28j60_change_duplex_state.h" to "$PROJECT_HEADER_FILES/framework/driver/enc28j60/impl/running_state/drv_enc28j60_change_duplex_state.h"
    drvEnc28j60DuplexStateHeaderFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60DuplexStateHeaderFile.setSourcePath("driver/enc28j60/src/dynamic/running_state/drv_enc28j60_change_duplex_state.h")
    drvEnc28j60DuplexStateHeaderFile.setOutputName("drv_enc28j60_change_duplex_state.h")
    drvEnc28j60DuplexStateHeaderFile.setDestPath("driver/enc28j60/src/dynamic/running_state/")
    drvEnc28j60DuplexStateHeaderFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/running_state/")
    drvEnc28j60DuplexStateHeaderFile.setType("HEADER")
    drvEnc28j60DuplexStateHeaderFile.setOverwrite(True)
    
    # file DRV_ENC28J60_CHKINI_STATE_C "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/running_state/drv_enc28j60_check_int_state.c" to "$PROJECT_SOURCE_FILES/framework/driver/enc28j60/running_state/drv_enc28j60_check_int_state.c"
    drvEnc28j60CheckIntStateSourceFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60CheckIntStateSourceFile.setSourcePath("driver/enc28j60/src/dynamic/running_state/drv_enc28j60_check_int_state.c")
    drvEnc28j60CheckIntStateSourceFile.setOutputName("drv_enc28j60_check_int_state.c")
    drvEnc28j60CheckIntStateSourceFile.setOverwrite(True)
    drvEnc28j60CheckIntStateSourceFile.setDestPath("driver/enc28j60/src/dynamic/running_state/")
    drvEnc28j60CheckIntStateSourceFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/running_state/")
    drvEnc28j60CheckIntStateSourceFile.setType("SOURCE")
    drvEnc28j60CheckIntStateSourceFile.setEnabled(True)
    
    # file DRV_ENC28J60_CHKINI_STATE_H "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/running_state/drv_enc28j60_check_int_state.h" to "$PROJECT_HEADER_FILES/framework/driver/enc28j60/impl/running_state/drv_enc28j60_check_int_state.h"
    drvEnc28j60CheckIntStateHeaderFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60CheckIntStateHeaderFile.setSourcePath("driver/enc28j60/src/dynamic/running_state/drv_enc28j60_check_int_state.h")
    drvEnc28j60CheckIntStateHeaderFile.setOutputName("drv_enc28j60_check_int_state.h")
    drvEnc28j60CheckIntStateHeaderFile.setDestPath("driver/enc28j60/src/dynamic/running_state/")
    drvEnc28j60CheckIntStateHeaderFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/running_state/")
    drvEnc28j60CheckIntStateHeaderFile.setType("HEADER")
    drvEnc28j60CheckIntStateHeaderFile.setOverwrite(True)
    
    # file DRV_ENC28J60_CHKSTA_STATE_C "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/running_state/drv_enc28j60_check_status_state.c" to "$PROJECT_SOURCE_FILES/framework/driver/enc28j60/running_state/drv_enc28j60_check_status_state.c"
    drvEnc28j60CheckStatusStateSourceFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60CheckStatusStateSourceFile.setSourcePath("driver/enc28j60/src/dynamic/running_state/drv_enc28j60_check_status_state.c")
    drvEnc28j60CheckStatusStateSourceFile.setOutputName("drv_enc28j60_check_status_state.c")
    drvEnc28j60CheckStatusStateSourceFile.setOverwrite(True)
    drvEnc28j60CheckStatusStateSourceFile.setDestPath("driver/enc28j60/src/dynamic/running_state/")
    drvEnc28j60CheckStatusStateSourceFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/running_state/")
    drvEnc28j60CheckStatusStateSourceFile.setType("SOURCE")
    drvEnc28j60CheckStatusStateSourceFile.setEnabled(True)
    
    # file DRV_ENC28J60_CHKSTA_STATE_H "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/running_state/drv_enc28j60_check_status_state.h" to "$PROJECT_HEADER_FILES/framework/driver/enc28j60/impl/running_state/drv_enc28j60_check_tx_status_state.h"
    drvEnc28j60CheckStatusStateHeaderFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60CheckStatusStateHeaderFile.setSourcePath("driver/enc28j60/src/dynamic/running_state/drv_enc28j60_check_status_state.h")
    drvEnc28j60CheckStatusStateHeaderFile.setOutputName("drv_enc28j60_check_status_state.h")
    drvEnc28j60CheckStatusStateHeaderFile.setDestPath("driver/enc28j60/src/dynamic/running_state/")
    drvEnc28j60CheckStatusStateHeaderFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/running_state/")
    drvEnc28j60CheckStatusStateHeaderFile.setType("HEADER")
    drvEnc28j60CheckStatusStateHeaderFile.setOverwrite(True)
    
    # file DRV_ENC28J60_CHKTXSTA_STATE_C "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/running_state/drv_enc28j60_check_tx_status_state.c" to "$PROJECT_SOURCE_FILES/framework/driver/enc28j60/running_state/drv_enc28j60_check_tx_status_state.c"
    drvEnc28j60CheckTxStatusStateSourceFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60CheckTxStatusStateSourceFile.setSourcePath("driver/enc28j60/src/dynamic/running_state/drv_enc28j60_check_tx_status_state.c")
    drvEnc28j60CheckTxStatusStateSourceFile.setOutputName("drv_enc28j60_check_tx_status_state.c")
    drvEnc28j60CheckTxStatusStateSourceFile.setOverwrite(True)
    drvEnc28j60CheckTxStatusStateSourceFile.setDestPath("driver/enc28j60/src/dynamic/running_state/")
    drvEnc28j60CheckTxStatusStateSourceFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/running_state/")
    drvEnc28j60CheckTxStatusStateSourceFile.setType("SOURCE")
    drvEnc28j60CheckTxStatusStateSourceFile.setEnabled(True)
    
    # file DRV_ENC28J60_CHKTXSTA_STATE_H "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/running_state/drv_enc28j60_check_tx_status_state.h" to "$PROJECT_HEADER_FILES/framework/driver/enc28j60/impl/running_state/drv_enc28j60_check_status_state.h"
    drvEnc28j60CheckTxStatusStateHeaderFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60CheckTxStatusStateHeaderFile.setSourcePath("driver/enc28j60/src/dynamic/running_state/drv_enc28j60_check_tx_status_state.h")
    drvEnc28j60CheckTxStatusStateHeaderFile.setOutputName("drv_enc28j60_check_tx_status_state.h")
    drvEnc28j60CheckTxStatusStateHeaderFile.setDestPath("driver/enc28j60/src/dynamic/running_state/")
    drvEnc28j60CheckTxStatusStateHeaderFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/running_state/")
    drvEnc28j60CheckTxStatusStateHeaderFile.setType("HEADER")
    drvEnc28j60CheckTxStatusStateHeaderFile.setOverwrite(True)
    
    # file DRV_ENC28J60_RESET_RX_STATE_C "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/running_state/drv_enc28j60_reset_rx_state.c" to "$PROJECT_SOURCE_FILES/framework/driver/enc28j60/running_state/drv_enc28j60_reset_rx_state.c"
    drvEnc28j60CheckRxStatusStateSourceFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60CheckRxStatusStateSourceFile.setSourcePath("driver/enc28j60/src/dynamic/running_state/drv_enc28j60_reset_rx_state.c")
    drvEnc28j60CheckRxStatusStateSourceFile.setOutputName("drv_enc28j60_reset_rx_state.c")
    drvEnc28j60CheckRxStatusStateSourceFile.setOverwrite(True)
    drvEnc28j60CheckRxStatusStateSourceFile.setDestPath("driver/enc28j60/src/dynamic/running_state/")
    drvEnc28j60CheckRxStatusStateSourceFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/running_state/")
    drvEnc28j60CheckRxStatusStateSourceFile.setType("SOURCE")
    drvEnc28j60CheckRxStatusStateSourceFile.setEnabled(True)
    
    # file DRV_ENC28J60_RESET_RX_STATE_H "$HARMONY_VERSION_PATH/framework/driver/enc28j60/src/dynamic/running_state/drv_enc28j60_reset_rx_state.h" to "$PROJECT_HEADER_FILES/framework/driver/enc28j60/impl/running_state/drv_enc28j60_reset_rx_state.h"    
    drvEnc28j60CheckRxStatusStateHeaderFile = drvExtMacEnc28j60CommonComponent.createFileSymbol(None, None)
    drvEnc28j60CheckRxStatusStateHeaderFile.setSourcePath("driver/enc28j60/src/dynamic/running_state/drv_enc28j60_reset_rx_state.h")
    drvEnc28j60CheckRxStatusStateHeaderFile.setOutputName("drv_enc28j60_reset_rx_state.h")
    drvEnc28j60CheckRxStatusStateHeaderFile.setDestPath("driver/enc28j60/src/dynamic/running_state/")
    drvEnc28j60CheckRxStatusStateHeaderFile.setProjectPath("config/" + configName + "/driver/enc28j60/src/dynamic/running_state/")
    drvEnc28j60CheckRxStatusStateHeaderFile.setType("HEADER")
    drvEnc28j60CheckRxStatusStateHeaderFile.setOverwrite(True)
        
        






