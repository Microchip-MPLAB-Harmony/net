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

def instantiateComponent(drvExtMacEncx24j600CommonComponent):
    print("Exernal Ethernet Controller ENCX24J600 Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")      
            
    # Enable ENCX24J600
    drvEncx24j600Module = drvExtMacEncx24j600CommonComponent.createBooleanSymbol("DRV_ENCX24J600_USE_DRIVER", None)
    drvEncx24j600Module.setLabel("Enable ENCX24J600")
    drvEncx24j600Module.setVisible(False)
    drvEncx24j600Module.setDescription("Enable ENCX24J600")
    drvEncx24j600Module.setDefaultValue(True)
    
    # add "<#include \"/framework/driver/encx24j600/config/drv_encx24j600.c.ftl\">"  to list SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA
    drvEncx24j600InitDataSourceFtl = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600InitDataSourceFtl.setType("STRING")
    drvEncx24j600InitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    drvEncx24j600InitDataSourceFtl.setSourcePath("driver/encx24j600/config/drv_encx24j600.c.ftl")
    drvEncx24j600InitDataSourceFtl.setMarkup(True) 
    
    drvEncx24j600SystemDefFile = drvExtMacEncx24j600CommonComponent.createFileSymbol("DRV_ENCX24J600_H_FILE", None)
    drvEncx24j600SystemDefFile.setType("STRING")
    drvEncx24j600SystemDefFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    drvEncx24j600SystemDefFile.setSourcePath("driver/encx24j600/templates/system/system_definitions.h.ftl")
    drvEncx24j600SystemDefFile.setMarkup(True)  

    # add "<#include \"/framework/driver/encx24j600/config/drv_encx24j600.h.ftl\">"  to list SYSTEM_CONFIG_H_DRIVER_CONFIGURATION 
    drvEncx24j600SysConfigSourceFtl = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600SysConfigSourceFtl.setType("STRING")
    drvEncx24j600SysConfigSourceFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_DRIVER_CONFIGURATION")
    drvEncx24j600SysConfigSourceFtl.setSourcePath("driver/encx24j600/config/drv_encx24j600.h.ftl")
    drvEncx24j600SysConfigSourceFtl.setMarkup(True)
    
    drvEncx24j600SystemDefObjFile = drvExtMacEncx24j600CommonComponent.createFileSymbol("DRV_ENCX24J600_DEF_OBJ", None)
    drvEncx24j600SystemDefObjFile.setType("STRING")
    drvEncx24j600SystemDefObjFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_OBJECTS")
    drvEncx24j600SystemDefObjFile.setSourcePath("driver/encx24j600/templates/system/system_definitions_objects.h.ftl")
    drvEncx24j600SystemDefObjFile.setMarkup(True) 

    drvEncx24j600SystemTaskFile = drvExtMacEncx24j600CommonComponent.createFileSymbol("DRV_ENCX24J600_TASKS_C", None)
    drvEncx24j600SystemTaskFile.setType("STRING")
    drvEncx24j600SystemTaskFile.setOutputName("core.LIST_SYSTEM_TASKS_C_CALL_DRIVER_TASKS")
    drvEncx24j600SystemTaskFile.setSourcePath("driver/encx24j600/templates/system/system_tasks.c.ftl")
    drvEncx24j600SystemTaskFile.setMarkup(True) 
        
    # file DRV_ENCX24J600_H "$HARMONY_VERSION_PATH/framework/driver/encx24j600/drv_encx24j600.h" to "$PROJECT_HEADER_FILES/framework/driver/encx24j600/drv_encx24j600.h"
    drvEncx24j600HeaderFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600HeaderFile.setSourcePath("driver/encx24j600/drv_encx24j600.h")
    drvEncx24j600HeaderFile.setOutputName("drv_encx24j600.h")
    drvEncx24j600HeaderFile.setDestPath("driver/encx24j600/")
    drvEncx24j600HeaderFile.setProjectPath("config/" + configName + "/driver/encx24j600/")
    drvEncx24j600HeaderFile.setType("HEADER")
    drvEncx24j600HeaderFile.setOverwrite(True)
    
    # file DRV_ENCX24J600_API_C "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/drv_encx24j600_api.c" to "$PROJECT_SOURCE_FILES/framework/driver/encx24j600/drv_encx24j600_api.c"
    drvEncx24j600ApiSourceFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600ApiSourceFile.setSourcePath("driver/encx24j600/src/dynamic/drv_encx24j600_api.c")
    drvEncx24j600ApiSourceFile.setOutputName("drv_encx24j600_api.c")
    drvEncx24j600ApiSourceFile.setOverwrite(True)
    drvEncx24j600ApiSourceFile.setDestPath("driver/encx24j600/src/dynamic/")
    drvEncx24j600ApiSourceFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/")
    drvEncx24j600ApiSourceFile.setType("SOURCE")
    drvEncx24j600ApiSourceFile.setEnabled(True)
    
    # file DRV_ENCX24J600_DS_DEFS_H "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/drv_encx24j600_ds_defs.h" to "$PROJECT_HEADER_FILES/framework/driver/encx24j600/impl/drv_encx24j600_ds_defs.h"
    drvEncx24j600DsDefHeaderFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600DsDefHeaderFile.setSourcePath("driver/encx24j600/src/dynamic/drv_encx24j600_ds_defs.h")
    drvEncx24j600DsDefHeaderFile.setOutputName("drv_encx24j600_ds_defs.h")
    drvEncx24j600DsDefHeaderFile.setDestPath("driver/encx24j600/src/dynamic/")
    drvEncx24j600DsDefHeaderFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/")
    drvEncx24j600DsDefHeaderFile.setType("HEADER")
    drvEncx24j600DsDefHeaderFile.setOverwrite(True)    
    
    # file DRV_ENCX24J600_LOCAL_H "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/drv_encx24j600_local.h" to "$PROJECT_HEADER_FILES/framework/driver/encx24j600/impl/drv_encx24j600_local.h"
    drvEncx24j600LocalHeaderFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600LocalHeaderFile.setSourcePath("driver/encx24j600/src/dynamic/drv_encx24j600_local.h")
    drvEncx24j600LocalHeaderFile.setOutputName("drv_encx24j600_local.h")
    drvEncx24j600LocalHeaderFile.setDestPath("driver/encx24j600/src/dynamic/")
    drvEncx24j600LocalHeaderFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/")
    drvEncx24j600LocalHeaderFile.setType("HEADER")
    drvEncx24j600LocalHeaderFile.setOverwrite(True) 
    
    # file DRV_ENCX24J600_MAIN_STATE_C "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/drv_encx24j600_main_state.c" to "$PROJECT_SOURCE_FILES/framework/driver/encx24j600/drv_encx24j600_main_state.c"
    drvEncx24j600MainStateSourceFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600MainStateSourceFile.setSourcePath("driver/encx24j600/src/dynamic/drv_encx24j600_main_state.c")
    drvEncx24j600MainStateSourceFile.setOutputName("drv_encx24j600_main_state.c")
    drvEncx24j600MainStateSourceFile.setOverwrite(True)
    drvEncx24j600MainStateSourceFile.setDestPath("driver/encx24j600/src/dynamic/")
    drvEncx24j600MainStateSourceFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/")
    drvEncx24j600MainStateSourceFile.setType("SOURCE")
    drvEncx24j600MainStateSourceFile.setEnabled(True)
    
    # file DRV_ENCX24J600_MAIN_STATE_H "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/drv_encx24j600_main_state.h" to "$PROJECT_HEADER_FILES/framework/driver/encx24j600/impl/drv_encx24j600_main_state.h"
    drvEncx24j600MainStateHeaderFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600MainStateHeaderFile.setSourcePath("driver/encx24j600/src/dynamic/drv_encx24j600_main_state.h")
    drvEncx24j600MainStateHeaderFile.setOutputName("drv_encx24j600_main_state.h")
    drvEncx24j600MainStateHeaderFile.setDestPath("driver/encx24j600/src/dynamic/")
    drvEncx24j600MainStateHeaderFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/")
    drvEncx24j600MainStateHeaderFile.setType("HEADER")
    drvEncx24j600MainStateHeaderFile.setOverwrite(True) 
    
    # file DRV_ENCX24J600_UTILS_C "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/drv_encx24j600_utils.c" to "$PROJECT_SOURCE_FILES/framework/driver/encx24j600/drv_encx24j600_utils.c"
    drvEncx24j600UtilsSourceFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600UtilsSourceFile.setSourcePath("driver/encx24j600/src/dynamic/drv_encx24j600_utils.c")
    drvEncx24j600UtilsSourceFile.setOutputName("drv_encx24j600_utils.c")
    drvEncx24j600UtilsSourceFile.setOverwrite(True)
    drvEncx24j600UtilsSourceFile.setDestPath("driver/encx24j600/src/dynamic/")
    drvEncx24j600UtilsSourceFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/")
    drvEncx24j600UtilsSourceFile.setType("SOURCE")
    drvEncx24j600UtilsSourceFile.setEnabled(True)
    
    # file DRV_ENCX24J600_UTILS_H "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/drv_encx24j600_utils.h" to "$PROJECT_HEADER_FILES/framework/driver/encx24j600/impl/drv_encx24j600_utils.h"
    drvEncx24j600UtilsHeaderFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600UtilsHeaderFile.setSourcePath("driver/encx24j600/src/dynamic/drv_encx24j600_utils.h")
    drvEncx24j600UtilsHeaderFile.setOutputName("drv_encx24j600_utils.h")
    drvEncx24j600UtilsHeaderFile.setDestPath("driver/encx24j600/src/dynamic/")
    drvEncx24j600UtilsHeaderFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/")
    drvEncx24j600UtilsHeaderFile.setType("HEADER")
    drvEncx24j600UtilsHeaderFile.setOverwrite(True) 
    
    # file DRV_ENCX24J600_BUS_H "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/bus/drv_encx24j600_bus.h" to "$PROJECT_HEADER_FILES/framework/driver/encx24j600/impl/bus/drv_encx24j600_bus.h"
    drvEncx24j600BusHeaderFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600BusHeaderFile.setSourcePath("driver/encx24j600/src/dynamic/bus/drv_encx24j600_bus.h")
    drvEncx24j600BusHeaderFile.setOutputName("drv_encx24j600_bus.h")
    drvEncx24j600BusHeaderFile.setDestPath("driver/encx24j600/src/dynamic/bus/")
    drvEncx24j600BusHeaderFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/bus/")
    drvEncx24j600BusHeaderFile.setType("HEADER")
    drvEncx24j600BusHeaderFile.setOverwrite(True) 
    
    # file DRV_ENCX24J600_SPI_BUS_C "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/bus/spi/drv_encx24j600_spi_bus.c" to "$PROJECT_SOURCE_FILES/framework/driver/encx24j600/bus/spi/drv_encx24j600_spi_bus.c"
    drvEncx24j600SpiBusSourceFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600SpiBusSourceFile.setSourcePath("driver/encx24j600/src/dynamic/bus/spi/drv_encx24j600_spi_bus.c")
    drvEncx24j600SpiBusSourceFile.setOutputName("drv_encx24j600_spi_bus.c")
    drvEncx24j600SpiBusSourceFile.setOverwrite(True)
    drvEncx24j600SpiBusSourceFile.setDestPath("driver/encx24j600/src/dynamic/bus/spi/")
    drvEncx24j600SpiBusSourceFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/bus/spi/")
    drvEncx24j600SpiBusSourceFile.setType("SOURCE")
    drvEncx24j600SpiBusSourceFile.setEnabled(True)
    
    # file DRV_ENCX24J600_SPI_BUS_H "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/bus/spi/drv_encx24j600_spi_bus.h" to "$PROJECT_HEADER_FILES/framework/driver/encx24j600/impl/bus/spi/drv_encx24j600_spi_bus.h"
    drvEncx24j600SpiBusHeaderFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600SpiBusHeaderFile.setSourcePath("driver/encx24j600/src/dynamic/bus/spi/drv_encx24j600_spi_bus.h")
    drvEncx24j600SpiBusHeaderFile.setOutputName("drv_encx24j600_spi_bus.h")
    drvEncx24j600SpiBusHeaderFile.setDestPath("driver/encx24j600/src/dynamic/bus/spi/")
    drvEncx24j600SpiBusHeaderFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/bus/spi/")
    drvEncx24j600SpiBusHeaderFile.setType("HEADER")
    drvEncx24j600SpiBusHeaderFile.setOverwrite(True) 
    
    # file DRV_ENCX24J600_CLOSED_STATE_C "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/closed_state/drv_encx24j600_closed_state.c" to "$PROJECT_SOURCE_FILES/framework/driver/encx24j600/closed_state/drv_encx24j600_closed_state.c"
    drvEncx24j600CloseStateSourceFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600CloseStateSourceFile.setSourcePath("driver/encx24j600/src/dynamic/closed_state/drv_encx24j600_closed_state.c")
    drvEncx24j600CloseStateSourceFile.setOutputName("drv_encx24j600_closed_state.c")
    drvEncx24j600CloseStateSourceFile.setOverwrite(True)
    drvEncx24j600CloseStateSourceFile.setDestPath("driver/encx24j600/src/dynamic/closed_state/")
    drvEncx24j600CloseStateSourceFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/closed_state/")
    drvEncx24j600CloseStateSourceFile.setType("SOURCE")
    drvEncx24j600CloseStateSourceFile.setEnabled(True)
    
    # file DRV_ENCX24J600_CLOSED_STATE_H "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/closed_state/drv_encx24j600_closed_state.h" to "$PROJECT_HEADER_FILES/framework/driver/encx24j600/impl/closed_state/drv_encx24j600_closed_state.h"
    drvEncx24j600CloseStateHeaderFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600CloseStateHeaderFile.setSourcePath("driver/encx24j600/src/dynamic/closed_state/drv_encx24j600_closed_state.h")
    drvEncx24j600CloseStateHeaderFile.setOutputName("drv_encx24j600_closed_state.h")
    drvEncx24j600CloseStateHeaderFile.setDestPath("driver/encx24j600/src/dynamic/closed_state/")
    drvEncx24j600CloseStateHeaderFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/closed_state/")
    drvEncx24j600CloseStateHeaderFile.setType("HEADER")
    drvEncx24j600CloseStateHeaderFile.setOverwrite(True) 
    
    # file DRV_ENCX24J600_INIT_STATE_C "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/initialization_state/drv_encx24j600_initialization_state.c" to "$PROJECT_SOURCE_FILES/framework/driver/encx24j600/initialization_state/drv_encx24j600_initialization_state.c"
    drvEncx24j600InitStateSourceFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600InitStateSourceFile.setSourcePath("driver/encx24j600/src/dynamic/initialization_state/drv_encx24j600_initialization_state.c")
    drvEncx24j600InitStateSourceFile.setOutputName("drv_encx24j600_initialization_state.c")
    drvEncx24j600InitStateSourceFile.setOverwrite(True)
    drvEncx24j600InitStateSourceFile.setDestPath("driver/encx24j600/src/dynamic/initialization_state/")
    drvEncx24j600InitStateSourceFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/initialization_state/")
    drvEncx24j600InitStateSourceFile.setType("SOURCE")
    drvEncx24j600InitStateSourceFile.setEnabled(True)
    
    # file DRV_ENCX24J600_INIT_STATE_H "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/initialization_state/drv_encx24j600_initialization_state.h" to "$PROJECT_HEADER_FILES/framework/driver/encx24j600/impl/initialization_state/drv_encx24j600_initialization_state.h"
    drvEncx24j600InitStateHeaderFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600InitStateHeaderFile.setSourcePath("driver/encx24j600/src/dynamic/initialization_state/drv_encx24j600_initialization_state.h")
    drvEncx24j600InitStateHeaderFile.setOutputName("drv_encx24j600_initialization_state.h")
    drvEncx24j600InitStateHeaderFile.setDestPath("driver/encx24j600/src/dynamic/initialization_state/")
    drvEncx24j600InitStateHeaderFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/initialization_state/")
    drvEncx24j600InitStateHeaderFile.setType("HEADER")
    drvEncx24j600InitStateHeaderFile.setOverwrite(True)
    
    # file DRV_ENCX24J600_CONFIG_STATE_C "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/initialization_state/drv_encx24j600_configure_state.c" to "$PROJECT_SOURCE_FILES/framework/driver/encx24j600/initialization_state/drv_encx24j600_configure_state.c"
    drvEncx24j600ConfigStateSourceFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600ConfigStateSourceFile.setSourcePath("driver/encx24j600/src/dynamic/initialization_state/drv_encx24j600_configure_state.c")
    drvEncx24j600ConfigStateSourceFile.setOutputName("drv_encx24j600_configure_state.c")
    drvEncx24j600ConfigStateSourceFile.setOverwrite(True)
    drvEncx24j600ConfigStateSourceFile.setDestPath("driver/encx24j600/src/dynamic/initialization_state/")
    drvEncx24j600ConfigStateSourceFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/initialization_state/")
    drvEncx24j600ConfigStateSourceFile.setType("SOURCE")
    drvEncx24j600ConfigStateSourceFile.setEnabled(True)
    
    # file DRV_ENCX24J600_CONFIG_STATE_H "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/initialization_state/drv_encx24j600_configure_state.h" to "$PROJECT_HEADER_FILES/framework/driver/encx24j600/impl/initialization_state/drv_encx24j600_configure_state.h"
    drvEncx24j600ConfigStateHeaderFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600ConfigStateHeaderFile.setSourcePath("driver/encx24j600/src/dynamic/initialization_state/drv_encx24j600_configure_state.h")
    drvEncx24j600ConfigStateHeaderFile.setOutputName("drv_encx24j600_configure_state.h")
    drvEncx24j600ConfigStateHeaderFile.setDestPath("driver/encx24j600/src/dynamic/initialization_state/")
    drvEncx24j600ConfigStateHeaderFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/initialization_state/")
    drvEncx24j600ConfigStateHeaderFile.setType("HEADER")
    drvEncx24j600ConfigStateHeaderFile.setOverwrite(True)
    
    # file DRV_ENCX24J600_DETECT_STATE_C "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/initialization_state/drv_encx24j600_detect_state.c" to "$PROJECT_SOURCE_FILES/framework/driver/encx24j600/initialization_state/drv_encx24j600_detect_state.c"
    drvEncx24j600DetectStateSourceFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600DetectStateSourceFile.setSourcePath("driver/encx24j600/src/dynamic/initialization_state/drv_encx24j600_detect_state.c")
    drvEncx24j600DetectStateSourceFile.setOutputName("drv_encx24j600_detect_state.c")
    drvEncx24j600DetectStateSourceFile.setOverwrite(True)
    drvEncx24j600DetectStateSourceFile.setDestPath("driver/encx24j600/src/dynamic/initialization_state/")
    drvEncx24j600DetectStateSourceFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/initialization_state/")
    drvEncx24j600DetectStateSourceFile.setType("SOURCE")
    drvEncx24j600DetectStateSourceFile.setEnabled(True)
    
    # file DRV_ENCX24J600_DETECT_STATE_H "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/initialization_state/drv_encx24j600_detect_state.h" to "$PROJECT_HEADER_FILES/framework/driver/encx24j600/impl/initialization_state/drv_encx24j600_detect_state.h"
    drvEncx24j600DetectStateHeaderFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600DetectStateHeaderFile.setSourcePath("driver/encx24j600/src/dynamic/initialization_state/drv_encx24j600_detect_state.h")
    drvEncx24j600DetectStateHeaderFile.setOutputName("drv_encx24j600_detect_state.h")
    drvEncx24j600DetectStateHeaderFile.setDestPath("driver/encx24j600/src/dynamic/initialization_state/")
    drvEncx24j600DetectStateHeaderFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/initialization_state/")
    drvEncx24j600DetectStateHeaderFile.setType("HEADER")
    drvEncx24j600DetectStateHeaderFile.setOverwrite(True)
    
    # file DRV_ENCX24J600_RESET_STATE_C "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/initialization_state/drv_encx24j600_reset_state.c" to "$PROJECT_SOURCE_FILES/framework/driver/encx24j600/initialization_state/drv_encx24j600_reset_state.c"
    drvEncx24j600ResetStateSourceFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600ResetStateSourceFile.setSourcePath("driver/encx24j600/src/dynamic/initialization_state/drv_encx24j600_reset_state.c")
    drvEncx24j600ResetStateSourceFile.setOutputName("drv_encx24j600_reset_state.c")
    drvEncx24j600ResetStateSourceFile.setOverwrite(True)
    drvEncx24j600ResetStateSourceFile.setDestPath("driver/encx24j600/src/dynamic/initialization_state/")
    drvEncx24j600ResetStateSourceFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/initialization_state/")
    drvEncx24j600ResetStateSourceFile.setType("SOURCE")
    drvEncx24j600ResetStateSourceFile.setEnabled(True)
    
    # file DRV_ENCX24J600_RESET_STATE_H "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/initialization_state/drv_encx24j600_reset_state.h" to "$PROJECT_HEADER_FILES/framework/driver/encx24j600/impl/initialization_state/drv_encx24j600_reset_state.h"
    drvEncx24j600ResetStateHeaderFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600ResetStateHeaderFile.setSourcePath("driver/encx24j600/src/dynamic/initialization_state/drv_encx24j600_reset_state.h")
    drvEncx24j600ResetStateHeaderFile.setOutputName("drv_encx24j600_reset_state.h")
    drvEncx24j600ResetStateHeaderFile.setDestPath("driver/encx24j600/src/dynamic/initialization_state/")
    drvEncx24j600ResetStateHeaderFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/initialization_state/")
    drvEncx24j600ResetStateHeaderFile.setType("HEADER")
    drvEncx24j600ResetStateHeaderFile.setOverwrite(True)
    
    # file DRV_ENCX24J600_TX_PACKET_C "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/packet/drv_encx24j600_tx_packet.c" to "$PROJECT_SOURCE_FILES/framework/driver/encx24j600/packet/drv_encx24j600_tx_packet.c"
    drvEncx24j600TxPktSourceFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600TxPktSourceFile.setSourcePath("driver/encx24j600/src/dynamic/packet/drv_encx24j600_tx_packet.c")
    drvEncx24j600TxPktSourceFile.setOutputName("drv_encx24j600_tx_packet.c")
    drvEncx24j600TxPktSourceFile.setOverwrite(True)
    drvEncx24j600TxPktSourceFile.setDestPath("driver/encx24j600/src/dynamic/packet/")
    drvEncx24j600TxPktSourceFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/packet/")
    drvEncx24j600TxPktSourceFile.setType("SOURCE")
    drvEncx24j600TxPktSourceFile.setEnabled(True)
    
    # file DRV_ENCX24J600_TX_PACKET_H "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/packet/drv_encx24j600_tx_packet.h" to "$PROJECT_HEADER_FILES/framework/driver/encx24j600/impl/packet/drv_encx24j600_tx_packet.h"
    drvEncx24j600TxPktHeaderFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600TxPktHeaderFile.setSourcePath("driver/encx24j600/src/dynamic/packet/drv_encx24j600_tx_packet.h")
    drvEncx24j600TxPktHeaderFile.setOutputName("drv_encx24j600_tx_packet.h")
    drvEncx24j600TxPktHeaderFile.setDestPath("driver/encx24j600/src/dynamic/packet/")
    drvEncx24j600TxPktHeaderFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/packet/")
    drvEncx24j600TxPktHeaderFile.setType("HEADER")
    drvEncx24j600TxPktHeaderFile.setOverwrite(True)
    
    # file DRV_ENCX24J600_RX_PACKET_C "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/packet/drv_encx24j600_rx_packet.c" to "$PROJECT_SOURCE_FILES/framework/driver/encx24j600/packet/drv_encx24j600_rx_packet.c"
    drvEncx24j600RxPktSourceFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600RxPktSourceFile.setSourcePath("driver/encx24j600/src/dynamic/packet/drv_encx24j600_rx_packet.c")
    drvEncx24j600RxPktSourceFile.setOutputName("drv_encx24j600_rx_packet.c")
    drvEncx24j600RxPktSourceFile.setOverwrite(True)
    drvEncx24j600RxPktSourceFile.setDestPath("driver/encx24j600/src/dynamic/packet/")
    drvEncx24j600RxPktSourceFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/packet/")
    drvEncx24j600RxPktSourceFile.setType("SOURCE")
    drvEncx24j600RxPktSourceFile.setEnabled(True)
    
    # file DRV_ENCX24J600_RX_PACKET_H "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/packet/drv_encx24j600_rx_packet.h" to "$PROJECT_HEADER_FILES/framework/driver/encx24j600/impl/packet/drv_encx24j600_rx_packet.h"
    drvEncx24j600RxPktHeaderFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600RxPktHeaderFile.setSourcePath("driver/encx24j600/src/dynamic/packet/drv_encx24j600_rx_packet.h")
    drvEncx24j600RxPktHeaderFile.setOutputName("drv_encx24j600_rx_packet.h")
    drvEncx24j600RxPktHeaderFile.setDestPath("driver/encx24j600/src/dynamic/packet/")
    drvEncx24j600RxPktHeaderFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/packet/")
    drvEncx24j600RxPktHeaderFile.setType("HEADER")
    drvEncx24j600RxPktHeaderFile.setOverwrite(True)
    
    # file DRV_ENCX24J600_RUNNING_STATE_C "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/running_state/drv_encx24j600_running_state.c" to "$PROJECT_SOURCE_FILES/framework/driver/encx24j600/running_state/drv_encx24j600_running_state.c"
    drvEncx24j600RunStateSourceFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600RunStateSourceFile.setSourcePath("driver/encx24j600/src/dynamic/running_state/drv_encx24j600_running_state.c")
    drvEncx24j600RunStateSourceFile.setOutputName("drv_encx24j600_running_state.c")
    drvEncx24j600RunStateSourceFile.setOverwrite(True)
    drvEncx24j600RunStateSourceFile.setDestPath("driver/encx24j600/src/dynamic/running_state/")
    drvEncx24j600RunStateSourceFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/running_state/")
    drvEncx24j600RunStateSourceFile.setType("SOURCE")
    drvEncx24j600RunStateSourceFile.setEnabled(True)
    
    # file DRV_ENCX24J600_RUNNING_STATE_H "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/running_state/drv_encx24j600_running_state.h" to "$PROJECT_HEADER_FILES/framework/driver/encx24j600/impl/running_state/drv_encx24j600_running_state.h"
    drvEncx24j600RunStateHeaderFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600RunStateHeaderFile.setSourcePath("driver/encx24j600/src/dynamic/running_state/drv_encx24j600_running_state.h")
    drvEncx24j600RunStateHeaderFile.setOutputName("drv_encx24j600_running_state.h")
    drvEncx24j600RunStateHeaderFile.setDestPath("driver/encx24j600/src/dynamic/running_state/")
    drvEncx24j600RunStateHeaderFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/running_state/")
    drvEncx24j600RunStateHeaderFile.setType("HEADER")
    drvEncx24j600RunStateHeaderFile.setOverwrite(True)
    
    # file DRV_ENCX24J600_CHDUP_STATE_C "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/running_state/drv_encx24j600_change_duplex_state.c" to "$PROJECT_SOURCE_FILES/framework/driver/encx24j600/running_state/drv_encx24j600_change_duplex_state.c"
    drvEncx24j600DuplexStateSourceFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600DuplexStateSourceFile.setSourcePath("driver/encx24j600/src/dynamic/running_state/drv_encx24j600_change_duplex_state.c")
    drvEncx24j600DuplexStateSourceFile.setOutputName("drv_encx24j600_change_duplex_state.c")
    drvEncx24j600DuplexStateSourceFile.setOverwrite(True)
    drvEncx24j600DuplexStateSourceFile.setDestPath("driver/encx24j600/src/dynamic/running_state/")
    drvEncx24j600DuplexStateSourceFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/running_state/")
    drvEncx24j600DuplexStateSourceFile.setType("SOURCE")
    drvEncx24j600DuplexStateSourceFile.setEnabled(True)
    
    # file DRV_ENCX24J600_CHDUP_STATE_H "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/running_state/drv_encx24j600_change_duplex_state.h" to "$PROJECT_HEADER_FILES/framework/driver/encx24j600/impl/running_state/drv_encx24j600_change_duplex_state.h"
    drvEncx24j600DuplexStateHeaderFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600DuplexStateHeaderFile.setSourcePath("driver/encx24j600/src/dynamic/running_state/drv_encx24j600_change_duplex_state.h")
    drvEncx24j600DuplexStateHeaderFile.setOutputName("drv_encx24j600_change_duplex_state.h")
    drvEncx24j600DuplexStateHeaderFile.setDestPath("driver/encx24j600/src/dynamic/running_state/")
    drvEncx24j600DuplexStateHeaderFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/running_state/")
    drvEncx24j600DuplexStateHeaderFile.setType("HEADER")
    drvEncx24j600DuplexStateHeaderFile.setOverwrite(True)
    
    # file DRV_ENCX24J600_CHKINI_STATE_C "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/running_state/drv_encx24j600_check_int_state.c" to "$PROJECT_SOURCE_FILES/framework/driver/encx24j600/running_state/drv_encx24j600_check_int_state.c"
    drvEncx24j600CheckIntStateSourceFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600CheckIntStateSourceFile.setSourcePath("driver/encx24j600/src/dynamic/running_state/drv_encx24j600_check_int_state.c")
    drvEncx24j600CheckIntStateSourceFile.setOutputName("drv_encx24j600_check_int_state.c")
    drvEncx24j600CheckIntStateSourceFile.setOverwrite(True)
    drvEncx24j600CheckIntStateSourceFile.setDestPath("driver/encx24j600/src/dynamic/running_state/")
    drvEncx24j600CheckIntStateSourceFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/running_state/")
    drvEncx24j600CheckIntStateSourceFile.setType("SOURCE")
    drvEncx24j600CheckIntStateSourceFile.setEnabled(True)
    
    # file DRV_ENCX24J600_CHKINI_STATE_H "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/running_state/drv_encx24j600_check_int_state.h" to "$PROJECT_HEADER_FILES/framework/driver/encx24j600/impl/running_state/drv_encx24j600_check_int_state.h"
    drvEncx24j600CheckIntStateHeaderFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600CheckIntStateHeaderFile.setSourcePath("driver/encx24j600/src/dynamic/running_state/drv_encx24j600_check_int_state.h")
    drvEncx24j600CheckIntStateHeaderFile.setOutputName("drv_encx24j600_check_int_state.h")
    drvEncx24j600CheckIntStateHeaderFile.setDestPath("driver/encx24j600/src/dynamic/running_state/")
    drvEncx24j600CheckIntStateHeaderFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/running_state/")
    drvEncx24j600CheckIntStateHeaderFile.setType("HEADER")
    drvEncx24j600CheckIntStateHeaderFile.setOverwrite(True)
    
    # file DRV_ENCX24J600_CHKSTA_STATE_C "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/running_state/drv_encx24j600_check_status_state.c" to "$PROJECT_SOURCE_FILES/framework/driver/encx24j600/running_state/drv_encx24j600_check_status_state.c"
    drvEncx24j600CheckStatusStateSourceFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600CheckStatusStateSourceFile.setSourcePath("driver/encx24j600/src/dynamic/running_state/drv_encx24j600_check_status_state.c")
    drvEncx24j600CheckStatusStateSourceFile.setOutputName("drv_encx24j600_check_status_state.c")
    drvEncx24j600CheckStatusStateSourceFile.setOverwrite(True)
    drvEncx24j600CheckStatusStateSourceFile.setDestPath("driver/encx24j600/src/dynamic/running_state/")
    drvEncx24j600CheckStatusStateSourceFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/running_state/")
    drvEncx24j600CheckStatusStateSourceFile.setType("SOURCE")
    drvEncx24j600CheckStatusStateSourceFile.setEnabled(True)
    
    # file DRV_ENCX24J600_CHKSTA_STATE_H "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/running_state/drv_encx24j600_check_status_state.h" to "$PROJECT_HEADER_FILES/framework/driver/encx24j600/impl/running_state/drv_encx24j600_check_tx_status_state.h"
    drvEncx24j600CheckStatusStateHeaderFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600CheckStatusStateHeaderFile.setSourcePath("driver/encx24j600/src/dynamic/running_state/drv_encx24j600_check_status_state.h")
    drvEncx24j600CheckStatusStateHeaderFile.setOutputName("drv_encx24j600_check_status_state.h")
    drvEncx24j600CheckStatusStateHeaderFile.setDestPath("driver/encx24j600/src/dynamic/running_state/")
    drvEncx24j600CheckStatusStateHeaderFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/running_state/")
    drvEncx24j600CheckStatusStateHeaderFile.setType("HEADER")
    drvEncx24j600CheckStatusStateHeaderFile.setOverwrite(True)
    
    # file DRV_ENCX24J600_CHKTXSTA_STATE_C "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/running_state/drv_encx24j600_check_tx_status_state.c" to "$PROJECT_SOURCE_FILES/framework/driver/encx24j600/running_state/drv_encx24j600_check_tx_status_state.c"
    drvEncx24j600CheckTxStatusStateSourceFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600CheckTxStatusStateSourceFile.setSourcePath("driver/encx24j600/src/dynamic/running_state/drv_encx24j600_check_tx_status_state.c")
    drvEncx24j600CheckTxStatusStateSourceFile.setOutputName("drv_encx24j600_check_tx_status_state.c")
    drvEncx24j600CheckTxStatusStateSourceFile.setOverwrite(True)
    drvEncx24j600CheckTxStatusStateSourceFile.setDestPath("driver/encx24j600/src/dynamic/running_state/")
    drvEncx24j600CheckTxStatusStateSourceFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/running_state/")
    drvEncx24j600CheckTxStatusStateSourceFile.setType("SOURCE")
    drvEncx24j600CheckTxStatusStateSourceFile.setEnabled(True)
    
    # file DRV_ENCX24J600_CHKTXSTA_STATE_H "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/running_state/drv_encx24j600_check_tx_status_state.h" to "$PROJECT_HEADER_FILES/framework/driver/encx24j600/impl/running_state/drv_encx24j600_check_status_state.h"
    drvEncx24j600CheckTxStatusStateHeaderFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600CheckTxStatusStateHeaderFile.setSourcePath("driver/encx24j600/src/dynamic/running_state/drv_encx24j600_check_tx_status_state.h")
    drvEncx24j600CheckTxStatusStateHeaderFile.setOutputName("drv_encx24j600_check_tx_status_state.h")
    drvEncx24j600CheckTxStatusStateHeaderFile.setDestPath("driver/encx24j600/src/dynamic/running_state/")
    drvEncx24j600CheckTxStatusStateHeaderFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/running_state/")
    drvEncx24j600CheckTxStatusStateHeaderFile.setType("HEADER")
    drvEncx24j600CheckTxStatusStateHeaderFile.setOverwrite(True)
    
    # file DRV_ENCX24J600_RESET_RX_STATE_C "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/running_state/drv_encx24j600_reset_rx_state.c" to "$PROJECT_SOURCE_FILES/framework/driver/encx24j600/running_state/drv_encx24j600_reset_rx_state.c"
    drvEncx24j600CheckRxStatusStateSourceFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600CheckRxStatusStateSourceFile.setSourcePath("driver/encx24j600/src/dynamic/running_state/drv_encx24j600_reset_rx_state.c")
    drvEncx24j600CheckRxStatusStateSourceFile.setOutputName("drv_encx24j600_reset_rx_state.c")
    drvEncx24j600CheckRxStatusStateSourceFile.setOverwrite(True)
    drvEncx24j600CheckRxStatusStateSourceFile.setDestPath("driver/encx24j600/src/dynamic/running_state/")
    drvEncx24j600CheckRxStatusStateSourceFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/running_state/")
    drvEncx24j600CheckRxStatusStateSourceFile.setType("SOURCE")
    drvEncx24j600CheckRxStatusStateSourceFile.setEnabled(True)
    
    # file DRV_ENCX24J600_RESET_RX_STATE_H "$HARMONY_VERSION_PATH/framework/driver/encx24j600/src/dynamic/running_state/drv_encx24j600_reset_rx_state.h" to "$PROJECT_HEADER_FILES/framework/driver/encx24j600/impl/running_state/drv_encx24j600_reset_rx_state.h"    
    drvEncx24j600CheckRxStatusStateHeaderFile = drvExtMacEncx24j600CommonComponent.createFileSymbol(None, None)
    drvEncx24j600CheckRxStatusStateHeaderFile.setSourcePath("driver/encx24j600/src/dynamic/running_state/drv_encx24j600_reset_rx_state.h")
    drvEncx24j600CheckRxStatusStateHeaderFile.setOutputName("drv_encx24j600_reset_rx_state.h")
    drvEncx24j600CheckRxStatusStateHeaderFile.setDestPath("driver/encx24j600/src/dynamic/running_state/")
    drvEncx24j600CheckRxStatusStateHeaderFile.setProjectPath("config/" + configName + "/driver/encx24j600/src/dynamic/running_state/")
    drvEncx24j600CheckRxStatusStateHeaderFile.setType("HEADER")
    drvEncx24j600CheckRxStatusStateHeaderFile.setOverwrite(True)
        

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






