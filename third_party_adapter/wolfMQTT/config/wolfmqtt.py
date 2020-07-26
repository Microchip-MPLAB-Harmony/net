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
def instantiateComponent(wolfmqttComponent):
    print("wolfMQTT Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    res = Database.activateComponents(["HarmonyCore"])
    # Enable "Generate Harmony Driver Common Files" option in MHC
    if (Database.getSymbolValue("HarmonyCore", "ENABLE_DRV_COMMON") == False):
        Database.setSymbolValue("HarmonyCore", "ENABLE_DRV_COMMON", True)

    # Enable "Generate Harmony System Service Common Files" option in MHC
    if (Database.getSymbolValue("HarmonyCore", "ENABLE_SYS_COMMON") == False):
        Database.setSymbolValue("HarmonyCore", "ENABLE_SYS_COMMON", True)
    if(Database.getComponentByID("netPres") == None):
        res = Database.activateComponents(["netPres"])
    # wolfMQTT Library Configuration
    wolfMqttLibMenu = wolfmqttComponent.createMenuSymbol(None, None) 
    wolfMqttLibMenu.setLabel("wolfMQTT Library Configuration")
    wolfMqttLibMenu.setVisible(True)
    wolfMqttLibMenu.setDescription("wolfMQTT Library Configurations")

    # Enable Diconnect Callback
    wolfMqttLibDisconnectCb = wolfmqttComponent.createBooleanSymbol("WOLFMQTT_DISCONNECT_CB", wolfMqttLibMenu)
    wolfMqttLibDisconnectCb.setLabel("Enable Disconnect Callback")
    wolfMqttLibDisconnectCb.setVisible(True)
    wolfMqttLibDisconnectCb.setDescription("Enable Disconnect Callback")
    wolfMqttLibDisconnectCb.setDefaultValue(True)

    # Disable Error String
    wolfMqttLibDisErrString = wolfmqttComponent.createBooleanSymbol("WOLFMQTT_NO_ERROR_STRINGS", wolfMqttLibMenu)
    wolfMqttLibDisErrString.setLabel("Disable Error Strings")
    wolfMqttLibDisErrString.setVisible(True)
    wolfMqttLibDisErrString.setDescription("Disable Error Strings")
    wolfMqttLibDisErrString.setDefaultValue(True)

    # Enable Sensor Network Client Support
    wolfMqttLibSnEn = wolfmqttComponent.createBooleanSymbol("WOLFMQTT_SN", wolfMqttLibMenu)
    wolfMqttLibSnEn.setLabel("Enable Sensor Network Client")
    wolfMqttLibSnEn.setVisible(True)
    wolfMqttLibSnEn.setDescription("Enable Sensor Network Client")
    wolfMqttLibSnEn.setDefaultValue(False)

    # Enable MQTT v5.0
    wolfMqttLibV5En = wolfmqttComponent.createBooleanSymbol("WOLFMQTT_V5", wolfMqttLibMenu)
    wolfMqttLibV5En.setLabel("Enable MQTT v5.0")
    wolfMqttLibV5En.setVisible(True)
    wolfMqttLibV5En.setDescription("Enable MQTT v5.0")
    wolfMqttLibV5En.setDefaultValue(False)

    # Enable Property Callback
    wolfMqttLibPrprtyCbEn = wolfmqttComponent.createBooleanSymbol("WOLFMQTT_PROPERTY_CB", wolfMqttLibV5En)
    wolfMqttLibPrprtyCbEn.setLabel("Enable Property Callback")
    wolfMqttLibPrprtyCbEn.setVisible(False)
    wolfMqttLibPrprtyCbEn.setDescription("Enable Property Callback")
    wolfMqttLibPrprtyCbEn.setDefaultValue(False)
    wolfMqttLibPrprtyCbEn.setDependencies(wolfMqttLibMenuVisible, ["WOLFMQTT_V5"])

    # Disable standard I/O
    wolfMqttLibNoStdio = wolfmqttComponent.createBooleanSymbol("WOLFMQTT_NO_STDIO", wolfMqttLibMenu)
    wolfMqttLibNoStdio.setLabel("Disable Std I/O")
    wolfMqttLibNoStdio.setVisible(True)
    wolfMqttLibNoStdio.setDescription("Disable Std I/O")
    wolfMqttLibNoStdio.setDefaultValue(True)
    
    # Enable Client Debug
    wolfMqttLibClientDbgEn = wolfmqttComponent.createBooleanSymbol("WOLFMQTT_DEBUG_CLIENT", wolfMqttLibMenu)
    wolfMqttLibClientDbgEn.setLabel("Enable Client Debug")
    wolfMqttLibClientDbgEn.setVisible(False)
    wolfMqttLibClientDbgEn.setDescription("Enable Client Debug")
    wolfMqttLibClientDbgEn.setDefaultValue(True)
    wolfMqttLibClientDbgEn.setDependencies(wolfMqttLibMenuInvisible, ["WOLFMQTT_NO_STDIO"])
    
    # Enable Socket Debug
    wolfMqttLibSktDbgEn = wolfmqttComponent.createBooleanSymbol("WOLFMQTT_DEBUG_SOCKET", wolfMqttLibMenu)
    wolfMqttLibSktDbgEn.setLabel("Enable Socket Debug")
    wolfMqttLibSktDbgEn.setVisible(False)
    wolfMqttLibSktDbgEn.setDescription("Enable Socket Debug")
    wolfMqttLibSktDbgEn.setDefaultValue(True)
    wolfMqttLibSktDbgEn.setDependencies(wolfMqttLibMenuInvisible, ["WOLFMQTT_NO_STDIO"])
    
    # Enable MQTT TLS
    wolfMqttLibTlsEn = wolfmqttComponent.createBooleanSymbol("WOLFMQTT_TLS_ENABLE", wolfMqttLibMenu)
    wolfMqttLibTlsEn.setLabel("Enable MQTT TLS")
    wolfMqttLibTlsEn.setVisible(False)
    wolfMqttLibTlsEn.setDescription("Enable MQTT TLS")
    wolfMqttLibTlsEn.setDefaultValue(False)
    wolfMqttLibTlsEn.setDependencies(wolfMqttNetGlueInvisible, ["WMQTT_NET_GLUE"])
    
    # Use MQTT Net Glue module
    wolfMqttNetGlue = wolfmqttComponent.createBooleanSymbol("WMQTT_NET_GLUE", None)
    wolfMqttNetGlue.setLabel("Use the wolfMQTT NET Glue Module")
    wolfMqttNetGlue.setVisible(True)
    wolfMqttNetGlue.setDescription("Enable the wolfMQTT NET Glue Code")
    wolfMqttNetGlue.setDefaultValue(True)

    # Enable NET_PRES
    wolfMqttNetPresEnable = wolfmqttComponent.createBooleanSymbol("WMQTT_NET_GLUE_FORCE_NET_PRES", wolfMqttNetGlue)
    wolfMqttNetPresEnable.setVisible(False)
    wolfMqttNetPresEnable.setDefaultValue(False)
    wolfMqttNetPresEnable.setDependencies(wolfMqttNetPresEnableFnc, ["WMQTT_NET_GLUE"])

    # Enable TLS
    wolfMqttNetForceTls = wolfmqttComponent.createBooleanSymbol("WMQTT_NET_GLUE_FORCE_TLS", wolfMqttNetGlue)
    wolfMqttNetForceTls.setLabel("MQTT Enable TLS")
    wolfMqttNetForceTls.setVisible(True)
    wolfMqttNetForceTls.setDescription("MQTT Enable TLS")
    wolfMqttNetForceTls.setDefaultValue(False)
    wolfMqttNetForceTls.setDependencies(wolfMqttNetGlueVisible, ["WMQTT_NET_GLUE"])

    # Enable IPv6
    wolfMqttNetIPv6En = wolfmqttComponent.createBooleanSymbol("WMQTT_NET_GLUE_IPV6", wolfMqttNetGlue)
    wolfMqttNetIPv6En.setLabel("MQTT Enable IPv6")
    wolfMqttNetIPv6En.setVisible(True)
    wolfMqttNetIPv6En.setDescription("MQTT Enable IPv6")
    wolfMqttNetIPv6En.setDefaultValue(False)
    wolfMqttNetIPv6En.setDependencies(wolfMqttNetGlueVisible, ["WMQTT_NET_GLUE"])
    
    # Max Broker name
    wolfMqttNetMaxBrkrName = wolfmqttComponent.createIntegerSymbol("WMQTT_NET_GLUE_MAX_BROKER_NAME", wolfMqttNetGlue)
    wolfMqttNetMaxBrkrName.setLabel("MQTT max. broker name") 
    wolfMqttNetMaxBrkrName.setVisible(True)
    wolfMqttNetMaxBrkrName.setDescription("MQTT max. broker name")
    wolfMqttNetMaxBrkrName.setDefaultValue(64)  
    wolfMqttNetMaxBrkrName.setDependencies(wolfMqttNetGlueVisible, ["WMQTT_NET_GLUE"])

    # Debug enable
    wolfMqttNetDbgEn = wolfmqttComponent.createBooleanSymbol("WMQTT_NET_GLUE_DEBUG_ENABLE", wolfMqttNetGlue)
    wolfMqttNetDbgEn.setLabel("MQTT Debug Enable")
    wolfMqttNetDbgEn.setVisible(True)
    wolfMqttNetDbgEn.setDescription("MQTT Debug Enable")
    wolfMqttNetDbgEn.setDefaultValue(False)
    wolfMqttNetDbgEn.setDependencies(wolfMqttNetGlueVisible, ["WMQTT_NET_GLUE"])

    # Enable Error Strings
    wolfMqttNetErrStrngEn = wolfmqttComponent.createBooleanSymbol("WMQTT_NET_GLUE_ERROR_STRINGS", wolfMqttNetGlue)
    wolfMqttNetErrStrngEn.setLabel("MQTT Error String Enable")
    wolfMqttNetErrStrngEn.setVisible(True)
    wolfMqttNetErrStrngEn.setDescription("MQTT Error Strings Enable")
    wolfMqttNetErrStrngEn.setDefaultValue(True)
    wolfMqttNetErrStrngEn.setDependencies(wolfMqttNetGlueVisible, ["WMQTT_NET_GLUE"])
    
    # Memory allocation function
    wolfMqttNetGlueMalloc = wolfmqttComponent.createStringSymbol("WMQTT_NET_GLUE_MALLOC", wolfMqttNetGlue)
    wolfMqttNetGlueMalloc.setLabel("MQTT Memory allocation function")
    wolfMqttNetGlueMalloc.setVisible(True)
    wolfMqttNetGlueMalloc.setDescription("Memory allocation function")
    wolfMqttNetGlueMalloc.setDefaultValue("malloc")
    wolfMqttNetGlueMalloc.setDependencies(wolfMqttNetGlueVisible, ["WMQTT_NET_GLUE"])
        
    # Memory free function
    wolfMqttNetGlueFree = wolfmqttComponent.createStringSymbol("WMQTT_NET_GLUE_FREE", wolfMqttNetGlue)
    wolfMqttNetGlueFree.setLabel("MQTT Memory free function")
    wolfMqttNetGlueFree.setVisible(True)
    wolfMqttNetGlueFree.setDescription("Memory de-allocation function")
    wolfMqttNetGlueFree.setDefaultValue("free")
    wolfMqttNetGlueFree.setDependencies(wolfMqttNetGlueVisible, ["WMQTT_NET_GLUE"])
    
    # Tx Buffer size
    wolfMqttNetSktTxBuff = wolfmqttComponent.createIntegerSymbol("WMQTT_NET_SKT_TX_BUFF", wolfMqttNetGlue)
    wolfMqttNetSktTxBuff.setLabel("MQTT Tx Buffer Size") 
    wolfMqttNetSktTxBuff.setVisible(True)
    wolfMqttNetSktTxBuff.setDescription("MQTT Tx Buffer Size")
    wolfMqttNetSktTxBuff.setDefaultValue(2048)  
    wolfMqttNetSktTxBuff.setDependencies(wolfMqttNetGlueVisible, ["WMQTT_NET_GLUE"])
    
    # Rx Buffer size
    wolfMqttNetSktRxBuff = wolfmqttComponent.createIntegerSymbol("WMQTT_NET_SKT_RX_BUFF", wolfMqttNetGlue)
    wolfMqttNetSktRxBuff.setLabel("MQTT Rx Buffer Size") 
    wolfMqttNetSktRxBuff.setVisible(True)
    wolfMqttNetSktRxBuff.setDescription("MQTT Rx Buffer Size")
    wolfMqttNetSktRxBuff.setDefaultValue(2048)
    wolfMqttNetSktRxBuff.setDependencies(wolfMqttNetGlueVisible, ["WMQTT_NET_GLUE"])
    
    #Add to configuration.h
    wolfMqttNetConfigFtl = wolfmqttComponent.createFileSymbol(None, None)
    wolfMqttNetConfigFtl.setSourcePath("third_party_adapter/wolfMQTT/config/wolfmqtt.h.ftl")
    wolfMqttNetConfigFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    wolfMqttNetConfigFtl.setMarkup(True)
    wolfMqttNetConfigFtl.setType("STRING")
    
    wolfMqttPath = wolfmqttComponent.createSettingSymbol("WOLFMQTT_INCLUDE_PATH", None)
    wolfMqttPath.setValue("../src/third_party/wolfmqtt;../src/config/"+configName+ "/third_party_adapter/wolfMQTT/src/")
    wolfMqttPath.setCategory("C32")
    wolfMqttPath.setKey("extra-include-directories")
    wolfMqttPath.setAppend(True, ";")

    wolfMqttPreProc = wolfmqttComponent.createSettingSymbol("FREERTOS_XC32_AS_INCLUDE_PRE_PROC_DIRS", None)
    wolfMqttPreProc.setCategory("C32")
    wolfMqttPreProc.setKey("preprocessor-macros")
    wolfMqttPreProc.setValue("HAVE_CONFIG_H")
    wolfMqttPreProc.setAppend(True, ";")
    
    coreHeapSize = Database.getSymbolValue("core","XC32_HEAP_SIZE")
    if (coreHeapSize < 102400): # heap size less than 100K
        Database.setSymbolValue("core","XC32_HEAP_SIZE", 102400)
   
    # third_party_adapter/wolfMQTT/src/mqtt_net_glue.h to config/<configName>/third_party_adapter/wolfMQTT/src/mqtt_net_glue.h 
    wolfMqttNetGlueHeaderFile = wolfmqttComponent.createFileSymbol(None, None)
    wolfMqttNetGlueHeaderFile.setSourcePath("third_party_adapter/wolfMQTT/src/mqtt_net_glue.h")
    wolfMqttNetGlueHeaderFile.setOutputName("mqtt_net_glue.h")
    wolfMqttNetGlueHeaderFile.setDestPath("third_party_adapter/wolfMQTT/src/")
    wolfMqttNetGlueHeaderFile.setProjectPath("config/" + configName + "/third_party_adapter/wolfMQTT/src/")
    wolfMqttNetGlueHeaderFile.setType("HEADER")
    wolfMqttNetGlueHeaderFile.setOverwrite(True)
    wolfMqttNetGlueHeaderFile.setEnabled(False)
    wolfMqttNetGlueHeaderFile.setDependencies(wolfMqttNetGlueEnabled, ["WMQTT_NET_GLUE"])

    # third_party_adapter/wolfMQTT/src/mqtt_net_glue.c to config/<configName>/third_party_adapter/wolfMQTT/src/mqtt_net_glue.c
    wolfMqttNetGlueSourceFile = wolfmqttComponent.createFileSymbol(None, None)
    wolfMqttNetGlueSourceFile.setSourcePath("third_party_adapter/wolfMQTT/src/mqtt_net_glue.c")
    wolfMqttNetGlueSourceFile.setOutputName("mqtt_net_glue.c")
    wolfMqttNetGlueSourceFile.setOverwrite(True)
    wolfMqttNetGlueSourceFile.setDestPath("third_party_adapter/wolfMQTT/src/")
    wolfMqttNetGlueSourceFile.setProjectPath("config/" + configName + "/third_party_adapter/wolfMQTT/src/")
    wolfMqttNetGlueSourceFile.setType("SOURCE")
    wolfMqttNetGlueSourceFile.setEnabled(False)
    wolfMqttNetGlueSourceFile.setDependencies(wolfMqttNetGlueEnabled, ["WMQTT_NET_GLUE"])

    # wolfMQTT/src/mqtt_client.c to third_party/wolfMQTT/src/mqtt_client.c
    wolfMqttClientSourceFile = wolfmqttComponent.createFileSymbol(None, None)
    wolfMqttClientSourceFile.setSourcePath("../wolfMQTT/src/mqtt_client.c")
    wolfMqttClientSourceFile.setOutputName("mqtt_client.c")
    wolfMqttClientSourceFile.setOverwrite(True)
    wolfMqttClientSourceFile.setDestPath("../../third_party/wolfMQTT/src/")
    wolfMqttClientSourceFile.setProjectPath("third_party/wolfMQTT/src/")
    wolfMqttClientSourceFile.setType("SOURCE")
    wolfMqttClientSourceFile.setEnabled(True)

    # wolfMQTT/src/mqtt_packet.c to third_party/wolfMQTT/src/mqtt_packet.c
    wolfMqttPacketSourceFile = wolfmqttComponent.createFileSymbol(None, None)
    wolfMqttPacketSourceFile.setSourcePath("../wolfMQTT/src/mqtt_packet.c")
    wolfMqttPacketSourceFile.setOutputName("mqtt_packet.c")
    wolfMqttPacketSourceFile.setOverwrite(True)
    wolfMqttPacketSourceFile.setDestPath("../../third_party/wolfMQTT/src/")
    wolfMqttPacketSourceFile.setProjectPath("third_party/wolfMQTT/src/")
    wolfMqttPacketSourceFile.setType("SOURCE")
    wolfMqttPacketSourceFile.setEnabled(True)

    # wolfMQTT/src/mqtt_socket.c to third_party/wolfMQTT/src/mqtt_socket.c
    wolfMqttSocketSourceFile = wolfmqttComponent.createFileSymbol(None, None)
    wolfMqttSocketSourceFile.setSourcePath("../wolfMQTT/src/mqtt_socket.c")
    wolfMqttSocketSourceFile.setOutputName("mqtt_socket.c")
    wolfMqttSocketSourceFile.setOverwrite(True)
    wolfMqttSocketSourceFile.setDestPath("../../third_party/wolfMQTT/src/")
    wolfMqttSocketSourceFile.setProjectPath("third_party/wolfMQTT/src/")
    wolfMqttSocketSourceFile.setType("SOURCE")
    wolfMqttSocketSourceFile.setEnabled(True) 

    wolfMqttClientHeaderFile = wolfmqttComponent.createFileSymbol(None, None)
    wolfMqttClientHeaderFile.setSourcePath("../wolfMQTT/wolfmqtt/mqtt_client.h")
    wolfMqttClientHeaderFile.setOutputName("mqtt_client.h")
    wolfMqttClientHeaderFile.setDestPath("../../third_party/wolfMQTT/wolfmqtt/")
    wolfMqttClientHeaderFile.setProjectPath("third_party/wolfMQTT/wolfmqtt/")
    wolfMqttClientHeaderFile.setType("HEADER")
    wolfMqttClientHeaderFile.setOverwrite(True)
    wolfMqttClientHeaderFile.setEnabled(True)

    wolfMqttPacketHeaderFile = wolfmqttComponent.createFileSymbol(None, None)
    wolfMqttPacketHeaderFile.setSourcePath("../wolfMQTT/wolfmqtt/mqtt_packet.h")
    wolfMqttPacketHeaderFile.setOutputName("mqtt_packet.h")
    wolfMqttPacketHeaderFile.setDestPath("../../third_party/wolfMQTT/wolfmqtt/")
    wolfMqttPacketHeaderFile.setProjectPath("third_party/wolfMQTT/wolfmqtt/")
    wolfMqttPacketHeaderFile.setType("HEADER")
    wolfMqttPacketHeaderFile.setOverwrite(True)
    wolfMqttPacketHeaderFile.setEnabled(True)

    wolfMqttSocketHeaderFile = wolfmqttComponent.createFileSymbol(None, None)
    wolfMqttSocketHeaderFile.setSourcePath("../wolfMQTT/wolfmqtt/mqtt_socket.h")
    wolfMqttSocketHeaderFile.setOutputName("mqtt_socket.h")
    wolfMqttSocketHeaderFile.setDestPath("../../third_party/wolfMQTT/wolfmqtt/")
    wolfMqttSocketHeaderFile.setProjectPath("third_party/wolfMQTT/wolfmqtt/")
    wolfMqttSocketHeaderFile.setType("HEADER")
    wolfMqttSocketHeaderFile.setOverwrite(True)
    wolfMqttSocketHeaderFile.setEnabled(True)

    wolfMqttTypesHeaderFile = wolfmqttComponent.createFileSymbol(None, None)
    wolfMqttTypesHeaderFile.setSourcePath("../wolfMQTT/wolfmqtt/mqtt_types.h")
    wolfMqttTypesHeaderFile.setOutputName("mqtt_types.h")
    wolfMqttTypesHeaderFile.setDestPath("../../third_party/wolfMQTT/wolfmqtt/")
    wolfMqttTypesHeaderFile.setProjectPath("third_party/wolfMQTT/wolfmqtt/")
    wolfMqttTypesHeaderFile.setType("HEADER")
    wolfMqttTypesHeaderFile.setOverwrite(True)
    wolfMqttTypesHeaderFile.setEnabled(True)

    wolfMqttVersionHeaderFile = wolfmqttComponent.createFileSymbol(None, None)
    wolfMqttVersionHeaderFile.setSourcePath("../wolfMQTT/wolfmqtt/version.h")
    wolfMqttVersionHeaderFile.setOutputName("version.h")
    wolfMqttVersionHeaderFile.setDestPath("../../third_party/wolfMQTT/wolfmqtt/")
    wolfMqttVersionHeaderFile.setProjectPath("third_party/wolfMQTT/wolfmqtt/")
    wolfMqttVersionHeaderFile.setType("HEADER")
    wolfMqttVersionHeaderFile.setOverwrite(True)
    wolfMqttVersionHeaderFile.setEnabled(True)

    wolfMqttVisibilityHeaderFile = wolfmqttComponent.createFileSymbol(None, None)
    wolfMqttVisibilityHeaderFile.setSourcePath("../wolfMQTT/wolfmqtt/visibility.h")
    wolfMqttVisibilityHeaderFile.setOutputName("visibility.h")
    wolfMqttVisibilityHeaderFile.setDestPath("../../third_party/wolfMQTT/wolfmqtt/")
    wolfMqttVisibilityHeaderFile.setProjectPath("third_party/wolfMQTT/wolfmqtt/")
    wolfMqttVisibilityHeaderFile.setType("HEADER")
    wolfMqttVisibilityHeaderFile.setOverwrite(True)
    wolfMqttVisibilityHeaderFile.setEnabled(True)   

    wolfMqttVsSettingHeaderFile = wolfmqttComponent.createFileSymbol(None, None)
    wolfMqttVsSettingHeaderFile.setSourcePath("../wolfMQTT/wolfmqtt/vs_settings.h")
    wolfMqttVsSettingHeaderFile.setOutputName("vs_settings.h")
    wolfMqttVsSettingHeaderFile.setDestPath("../../third_party/wolfMQTT/wolfmqtt/")
    wolfMqttVsSettingHeaderFile.setProjectPath("third_party/wolfMQTT/wolfmqtt/")
    wolfMqttVsSettingHeaderFile.setType("HEADER")
    wolfMqttVsSettingHeaderFile.setOverwrite(True)
    wolfMqttVsSettingHeaderFile.setEnabled(True)    
        
    wolfMqttConfigHeaderFile = wolfmqttComponent.createFileSymbol(None, None)
    wolfMqttConfigHeaderFile.setProjectPath("config/" + configName)
    wolfMqttConfigHeaderFile.setSourcePath("third_party_adapter/wolfMQTT/config/config.h")
    wolfMqttConfigHeaderFile.setDestPath("")
    wolfMqttConfigHeaderFile.setOutputName("config.h")
    wolfMqttConfigHeaderFile.setType("HEADER")
    wolfMqttConfigHeaderFile.setOverwrite(True)
    wolfMqttConfigHeaderFile.setEnabled(True)   
        
    # Generate Custom MQTT Application Template
    wolfMqttCstmAppTmplt = wolfmqttComponent.createBooleanSymbol("WMQTT_CSTM_APP_TEMPLATE", wolfMqttNetGlue)
    wolfMqttCstmAppTmplt.setLabel("Generate Custom MQTT Application Template")
    wolfMqttCstmAppTmplt.setVisible(True)
    wolfMqttCstmAppTmplt.setDescription("Generate MQTT Custom Application Template")
    wolfMqttCstmAppTmplt.setDefaultValue(False)
    wolfMqttCstmAppTmplt.setDependencies(wolfMqttNetGlueVisible, ["WMQTT_NET_GLUE"])
    
    wolfMqttCstmCmdAppSrcFile = wolfmqttComponent.createFileSymbol(None, None)
    wolfMqttCstmCmdAppSrcFile.setSourcePath("tcpip/config/custom_app/wolfmqtt_app_template/app_mqtt_commands.c")  
    wolfMqttCstmCmdAppSrcFile.setProjectPath("")
    wolfMqttCstmCmdAppSrcFile.setDestPath("../../")
    wolfMqttCstmCmdAppSrcFile.setOutputName("app_mqtt_commands.c")
    wolfMqttCstmCmdAppSrcFile.setType("SOURCE")
    wolfMqttCstmCmdAppSrcFile.setOverwrite(True)
    wolfMqttCstmCmdAppSrcFile.setEnabled(False)   
    wolfMqttCstmCmdAppSrcFile.setDependencies(wolfMqttCstmApp, ["WMQTT_CSTM_APP_TEMPLATE","WMQTT_NET_GLUE"])
        
    wolfMqttCstmTaskAppSrcFile = wolfmqttComponent.createFileSymbol(None, None)
    wolfMqttCstmTaskAppSrcFile.setSourcePath("tcpip/config/custom_app/wolfmqtt_app_template/app_mqtt_task.c")  
    wolfMqttCstmTaskAppSrcFile.setProjectPath("")
    wolfMqttCstmTaskAppSrcFile.setDestPath("../../")
    wolfMqttCstmTaskAppSrcFile.setOutputName("app_mqtt_task.c")
    wolfMqttCstmTaskAppSrcFile.setType("SOURCE")
    wolfMqttCstmTaskAppSrcFile.setOverwrite(True)
    wolfMqttCstmTaskAppSrcFile.setEnabled(False)   
    wolfMqttCstmTaskAppSrcFile.setDependencies(wolfMqttCstmApp, ["WMQTT_CSTM_APP_TEMPLATE","WMQTT_NET_GLUE"])
        
    wolfMqttCstmTaskAppHeaderFile = wolfmqttComponent.createFileSymbol(None, None)
    wolfMqttCstmTaskAppHeaderFile.setSourcePath("tcpip/config/custom_app/wolfmqtt_app_template/app_mqtt_task.h")  
    wolfMqttCstmTaskAppHeaderFile.setProjectPath("")
    wolfMqttCstmTaskAppHeaderFile.setDestPath("../../")
    wolfMqttCstmTaskAppHeaderFile.setOutputName("app_mqtt_task.h")
    wolfMqttCstmTaskAppHeaderFile.setType("HEADER")
    wolfMqttCstmTaskAppHeaderFile.setOverwrite(True)
    wolfMqttCstmTaskAppHeaderFile.setEnabled(False)   
    wolfMqttCstmTaskAppHeaderFile.setDependencies(wolfMqttCstmApp, ["WMQTT_CSTM_APP_TEMPLATE","WMQTT_NET_GLUE"]) 
        
    wolfMqttDummyTaskAppSrcFile = wolfmqttComponent.createFileSymbol(None, None)
    wolfMqttDummyTaskAppSrcFile.setSourcePath("tcpip/config/custom_app/wolfmqtt_app_template/app_mqtt_task_dummy.c")  
    wolfMqttDummyTaskAppSrcFile.setProjectPath("")
    wolfMqttDummyTaskAppSrcFile.setDestPath("../../")
    wolfMqttDummyTaskAppSrcFile.setOutputName("app_mqtt_task.c")
    wolfMqttDummyTaskAppSrcFile.setType("SOURCE")
    wolfMqttDummyTaskAppSrcFile.setOverwrite(True)
    wolfMqttDummyTaskAppSrcFile.setEnabled(True)   
    wolfMqttDummyTaskAppSrcFile.setDependencies(wolfMqttDummyApp, ["WMQTT_CSTM_APP_TEMPLATE","WMQTT_NET_GLUE"])
    
    wolfMqttDummyTaskAppHeaderFile = wolfmqttComponent.createFileSymbol(None, None)
    wolfMqttDummyTaskAppHeaderFile.setSourcePath("tcpip/config/custom_app/wolfmqtt_app_template/app_mqtt_task_dummy.h")  
    wolfMqttDummyTaskAppHeaderFile.setProjectPath("")
    wolfMqttDummyTaskAppHeaderFile.setDestPath("../../")
    wolfMqttDummyTaskAppHeaderFile.setOutputName("app_mqtt_task.h")
    wolfMqttDummyTaskAppHeaderFile.setType("HEADER")
    wolfMqttDummyTaskAppHeaderFile.setOverwrite(True)
    wolfMqttDummyTaskAppHeaderFile.setEnabled(True)   
    wolfMqttDummyTaskAppHeaderFile.setDependencies(wolfMqttDummyApp, ["WMQTT_CSTM_APP_TEMPLATE","WMQTT_NET_GLUE"])
    
def finalizeComponent(wolfmqttComponent):  
    Database.setActiveGroup("__ROOTVIEW")
    Database.selectComponent("lib_wolfmqtt")
    
def wolfMqttLibMenuVisible(symbol, event): 
    symbol.setVisible(event["value"])
    
def wolfMqttNetGlueVisible(symbol, event): 
    symbol.setVisible(event["value"])

def wolfMqttNetGlueInvisible(symbol, event): 
    if (event["value"] == True):
        symbol.setVisible(False)
    else:
        symbol.setVisible(True)

def wolfMqttNetGlueEnabled(symbol, event): 
    symbol.setEnabled(event["value"]) 
    
def wolfMqttLibMenuInvisible(symbol, event): 
    if (event["value"] == True):
        symbol.setVisible(False)
    else:
        symbol.setVisible(True)
    
def wolfMqttNetForceTlsEnable(symbol, event):
    if (event["value"] == True): 
        setVal("netPres_0","NET_PRES_SUPPORT_ENCRYPTION0", True)
        setVal("netPres_0","NET_PRES_ENC_PROVIDE_IDX0", 0)
        
def wolfMqttCstmApp(symbol, event):
    if (Database.getSymbolValue("lib_wolfmqtt", "WMQTT_CSTM_APP_TEMPLATE") == True) and (Database.getSymbolValue("lib_wolfmqtt", "WMQTT_NET_GLUE") == True):
        symbol.setEnabled(True) 
    else:
        symbol.setEnabled(False) 
    

def wolfMqttDummyApp(symbol, event):
    if (Database.getSymbolValue("lib_wolfmqtt", "WMQTT_CSTM_APP_TEMPLATE") == True) and (Database.getSymbolValue("lib_wolfmqtt", "WMQTT_NET_GLUE") == True):
        symbol.setEnabled(False) 
    else:
        symbol.setEnabled(True) 
    
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
      
def wolfMqttNetPresEnableFnc(symbol, event): 
    if (event["value"] == True):
        if(Database.getComponentByID("netPres") == None):
            res = Database.activateComponents(["netPres"])  
  
