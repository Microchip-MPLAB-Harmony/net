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

# Global definitions  
TCPIP_HTTP_MODULE_FLAGS = ["TCPIP_HTTP_MODULE_FLAG_DEFAULT", "TCPIP_HTTP_MODULE_FLAG_ADJUST_SKT_FIFOS", "TCPIP_HTTP_MODULE_FLAG_NO_DELAY"]

def instantiateComponent(tcpipHttpComponent):
    print("TCPIP HTTP Component")
    configName = Variables.get("__CONFIGURATION_NAME")      
        
    # Enable HTTP Server
    tcpipHttp = tcpipHttpComponent.createBooleanSymbol("TCPIP_STACK_USE_HTTP_SERVER", None)
    tcpipHttp.setLabel("HTTP Server")
    tcpipHttp.setVisible(False)
    tcpipHttp.setDescription("Enable HTTP Server")
    tcpipHttp.setDefaultValue(True)

    # Maximum Number of Simultaneous Connections
    tcpipHttpConnMaxNum= tcpipHttpComponent.createIntegerSymbol("TCPIP_HTTP_MAX_CONNECTIONS", None)
    tcpipHttpConnMaxNum.setLabel("Maximum Number of Simultaneous Connections")
    tcpipHttpConnMaxNum.setVisible(True)
    tcpipHttpConnMaxNum.setDescription("Maximum Number of Simultaneous Connections")
    tcpipHttpConnMaxNum.setDefaultValue(4)

    # Maximum Default File String Length
    tcpipHttpDefaultLenMax= tcpipHttpComponent.createIntegerSymbol("TCPIP_HTTP_DEFAULT_LEN", None)
    tcpipHttpDefaultLenMax.setLabel("Maximum File String Length")
    tcpipHttpDefaultLenMax.setVisible(True)
    tcpipHttpDefaultLenMax.setDescription("Default Maximum File String Length")
    tcpipHttpDefaultLenMax.setDefaultValue(10)

    # HTTP Socket Transmit Buffer Size
    tcpipHttpSktTxBuffSize= tcpipHttpComponent.createIntegerSymbol("TCPIP_HTTP_SKT_TX_BUFF_SIZE", None)
    tcpipHttpSktTxBuffSize.setLabel("HTTP Socket TX Buffer Size")
    tcpipHttpSktTxBuffSize.setVisible(True)
    tcpipHttpSktTxBuffSize.setDescription("HTTP Socket Transmit Buffer Size")
    tcpipHttpSktTxBuffSize.setDefaultValue(0)

    # HTTP Socket Receive Buffer Size
    tcpipHttpSktRxBuffSize= tcpipHttpComponent.createIntegerSymbol("TCPIP_HTTP_SKT_RX_BUFF_SIZE", None)
    tcpipHttpSktRxBuffSize.setLabel("HTTP Socket RX Buffer Size")
    tcpipHttpSktRxBuffSize.setVisible(True)
    tcpipHttpSktRxBuffSize.setDescription("HTTP Socket Receive Buffer Size")
    tcpipHttpSktRxBuffSize.setDefaultValue(0)

    # Default Web pages directory
    tcpipHttpWebDir = tcpipHttpComponent.createStringSymbol("TCPIP_HTTP_WEB_DIR", None)
    tcpipHttpWebDir.setLabel("Web Pages Directory")
    tcpipHttpWebDir.setVisible(True)
    tcpipHttpWebDir.setDescription("Web Pages Directory")
    tcpipHttpWebDir.setDefaultValue("/mnt/mchpSite1/")

    # Include HTTP Custom Template
    tcpipHttpCustomTemplate = tcpipHttpComponent.createBooleanSymbol("TCPIP_HTTP_CUSTOM_TEMPLATE", None)
    tcpipHttpCustomTemplate.setLabel("Include HTTP Custom Template")
    tcpipHttpCustomTemplate.setVisible(True)
    tcpipHttpCustomTemplate.setDescription("Include HTTP Custom Template")
    tcpipHttpCustomTemplate.setDefaultValue(True)

    tcpipHttpCustomTemplateSl = tcpipHttpComponent.createBooleanSymbol("TCPIP_HTTP_CUSTOM_TEMPLATE_SL", None)
    tcpipHttpCustomTemplateSl.setVisible(False) 
    tcpipHttpCustomTemplateSl.setDefaultValue((Database.getSymbolValue("sys_fs", "SYS_FS_MPFS") == True))
    tcpipHttpCustomTemplateSl.setDependencies(tcpipHttpCustomSlSet, ["sys_fs.SYS_FS_MPFS"])
    
    
    # Advanced Settings
    tcpipHttpAdvSettings = tcpipHttpComponent.createMenuSymbol("TCPIP_HTTP_ADV_SETTING", None)
    tcpipHttpAdvSettings.setLabel("Advanced Settings")
    tcpipHttpAdvSettings.setDescription("Advanced Settings")
    tcpipHttpAdvSettings.setVisible(True)
        
    # HTTP Task Rate in ms
    tcpipHttpTskRate= tcpipHttpComponent.createIntegerSymbol("TCPIP_HTTP_TASK_RATE", tcpipHttpAdvSettings)
    tcpipHttpTskRate.setLabel("HTTP Task Rate - ms")
    tcpipHttpTskRate.setVisible(True)
    tcpipHttpTskRate.setDescription("HTTP Task Rate in ms")
    tcpipHttpTskRate.setDefaultValue(33)

    # Maximum Header Length
    tcpipHttpHeaderLenMax = tcpipHttpComponent.createIntegerSymbol("TCPIP_HTTP_MAX_HEADER_LEN", tcpipHttpAdvSettings)
    tcpipHttpHeaderLenMax.setLabel("Maximum Header Length")
    tcpipHttpHeaderLenMax.setVisible(True)
    tcpipHttpHeaderLenMax.setDescription("Maximum Header Length")
    tcpipHttpHeaderLenMax.setDefaultValue(15)

    # Maximum Lifetime of Static Responses in Seconds
    tcpipHttpCacheLen = tcpipHttpComponent.createIntegerSymbol("TCPIP_HTTP_CACHE_LEN", tcpipHttpAdvSettings)
    tcpipHttpCacheLen.setLabel("Maximum Lifetime of Static Responses (in sec)")
    tcpipHttpCacheLen.setVisible(True)
    tcpipHttpCacheLen.setDescription("Maximum Lifetime of Static Responses in Seconds")
    tcpipHttpCacheLen.setDefaultValue(600)

    # Socket Disconnect Time-out
    tcpipHttpSktDisconnectTimeout= tcpipHttpComponent.createIntegerSymbol("TCPIP_HTTP_TIMEOUT", tcpipHttpAdvSettings)
    tcpipHttpSktDisconnectTimeout.setLabel("Socket Disconnect Timeout (in sec)")
    tcpipHttpSktDisconnectTimeout.setVisible(True)
    tcpipHttpSktDisconnectTimeout.setDescription("Socket Disconnect Time-out")
    tcpipHttpSktDisconnectTimeout.setDefaultValue(45)
    
    # Maximum Data Length in bytes for Reading Cookie and GET/POST Arguments
    tcpipHttpDataLenMax= tcpipHttpComponent.createIntegerSymbol("TCPIP_HTTP_MAX_DATA_LEN", tcpipHttpAdvSettings)
    tcpipHttpDataLenMax.setLabel("Maximum Data Length (bytes)")
    tcpipHttpDataLenMax.setVisible(True)
    tcpipHttpDataLenMax.setDescription("Maximum Data Length in bytes for Reading Cookie and GET/POST Arguments")
    tcpipHttpDataLenMax.setDefaultValue(100)

    # Minimum Number of Bytes Free in TX FIFO Before Executing Callbacks
    tcpipHttpCallbackFreeMin= tcpipHttpComponent.createIntegerSymbol("TCPIP_HTTP_MIN_CALLBACK_FREE", tcpipHttpAdvSettings)
    tcpipHttpCallbackFreeMin.setLabel("Minimum TX FIFO Threshold (bytes)")
    tcpipHttpCallbackFreeMin.setVisible(True)
    tcpipHttpCallbackFreeMin.setDescription("Minimum Number of Bytes Free in TX FIFO Before Executing Callbacks")
    tcpipHttpCallbackFreeMin.setDefaultValue(16)

    # Default HTTP File
    tcpipHttpDefaultFile = tcpipHttpComponent.createStringSymbol("TCPIP_HTTP_DEFAULT_FILE", tcpipHttpAdvSettings)
    tcpipHttpDefaultFile.setLabel("Default HTTP File")
    tcpipHttpDefaultFile.setVisible(True)
    tcpipHttpDefaultFile.setDescription("Default HTTP File")
    tcpipHttpDefaultFile.setDefaultValue("index.htm")
    
    # HTTP Module Configuration Flags
    tcpipHttpConfigFlags = tcpipHttpComponent.createComboSymbol("TCPIP_HTTP_CFG_FLAGS", tcpipHttpAdvSettings, TCPIP_HTTP_MODULE_FLAGS)
    tcpipHttpConfigFlags.setLabel("HTTP Module Configuration Flags")
    tcpipHttpConfigFlags.setVisible(True)
    tcpipHttpConfigFlags.setDescription("HTTP Module Configuration Flags")
    tcpipHttpConfigFlags.setDefaultValue("TCPIP_HTTP_MODULE_FLAG_ADJUST_SKT_FIFOS")

    # HTTP Config Flag 
    tcpipHttpConfigFlag = tcpipHttpComponent.createIntegerSymbol("TCPIP_HTTP_CONFIG_FLAGS", tcpipHttpAdvSettings)
    tcpipHttpConfigFlag.setVisible(False)   
    tcpipHttpConfigFlag.setDefaultValue(1)

    # Enable MPFS Update via HTTP
    tcpipHttpFileUpload = tcpipHttpComponent.createBooleanSymbol("TCPIP_HTTP_FILE_UPLOAD_ENABLE", tcpipHttpAdvSettings)
    tcpipHttpFileUpload.setLabel("Enable MPFS Update via HTTP")
    tcpipHttpFileUpload.setVisible(True)
    tcpipHttpFileUpload.setDescription("Enable MPFS Update via HTTP")
    tcpipHttpFileUpload.setDefaultValue(False)

    # MPFS Upload Page Name
    tcpipHttpFileUploadName = tcpipHttpComponent.createStringSymbol("TCPIP_HTTP_FILE_UPLOAD_NAME", tcpipHttpFileUpload)
    tcpipHttpFileUploadName.setLabel("MPFS Upload Page Name")
    tcpipHttpFileUploadName.setVisible(False)
    tcpipHttpFileUploadName.setDescription("MPFS Upload Page Name")
    tcpipHttpFileUploadName.setDefaultValue("mpfsupload")
    tcpipHttpFileUploadName.setDependencies(tcpipHttpFileUploadVisible, ["TCPIP_HTTP_FILE_UPLOAD_ENABLE"])
    
    # MPFS NVM Mount Path
    tcpipHttpMpfsNvmMountPath = tcpipHttpComponent.createStringSymbol("TCPIP_HTTP_MPFS_NVM_PATH", tcpipHttpFileUpload)
    tcpipHttpMpfsNvmMountPath.setLabel("MPFS NVM Mount Path")
    tcpipHttpMpfsNvmMountPath.setVisible(False)
    tcpipHttpMpfsNvmMountPath.setDescription("MPFS NVM Mount Path")
    tcpipHttpMpfsNvmMountPath.setDefaultValue("/mnt/mchpSite1")
    tcpipHttpMpfsNvmMountPath.setDependencies(tcpipHttpFileUploadVisible, ["TCPIP_HTTP_FILE_UPLOAD_ENABLE"])
    
    # MPFS NVM Disk Path
    tcpipHttpMpfsNvmDiskPath = tcpipHttpComponent.createStringSymbol("TCPIP_HTTP_MPFS_NVM_VOL", tcpipHttpFileUpload)
    tcpipHttpMpfsNvmDiskPath.setLabel("MPFS NVM Disk Path")
    tcpipHttpMpfsNvmDiskPath.setVisible(False)
    tcpipHttpMpfsNvmDiskPath.setDescription("MPFS NVM Disk Path")
    tcpipHttpMpfsNvmDiskPath.setDefaultValue("/dev/nvma1")    
    tcpipHttpMpfsNvmDiskPath.setDependencies(tcpipHttpFileUploadVisible, ["TCPIP_HTTP_FILE_UPLOAD_ENABLE"])   
    
    # MPFS NVM Disk Number
    tcpipHttpMpfsNvmDiskNum = tcpipHttpComponent.createIntegerSymbol("TCPIP_HTTP_MPFS_NVM_NUM", tcpipHttpFileUpload)
    tcpipHttpMpfsNvmDiskNum.setLabel("MPFS NVM Disk Number")
    tcpipHttpMpfsNvmDiskNum.setVisible(False)
    tcpipHttpMpfsNvmDiskNum.setDescription("MPFS NVM Disk Number")
    tcpipHttpMpfsNvmDiskNum.setDefaultValue(0)    
    tcpipHttpMpfsNvmDiskNum.setDependencies(tcpipHttpFileUploadVisible, ["TCPIP_HTTP_FILE_UPLOAD_ENABLE"])   
    
    # Enable POST Support
    tcpipHttpPostSupport = tcpipHttpComponent.createBooleanSymbol("TCPIP_HTTP_USE_POST", tcpipHttpAdvSettings)
    tcpipHttpPostSupport.setLabel("Enable POST Support")
    tcpipHttpPostSupport.setVisible(True)
    tcpipHttpPostSupport.setDescription("Enable POST Support")
    tcpipHttpPostSupport.setDefaultValue(True)

    # Enable Cookie Support
    tcpipHttpCookieSupport = tcpipHttpComponent.createBooleanSymbol("TCPIP_HTTP_USE_COOKIES", tcpipHttpAdvSettings)
    tcpipHttpCookieSupport.setLabel("Enable Cookie Support")
    tcpipHttpCookieSupport.setVisible(True)
    tcpipHttpCookieSupport.setDescription("Enable Cookie Support")
    tcpipHttpCookieSupport.setDefaultValue(True)

    # Use Base 64 Decode
    tcpipHttpBase64Decode = tcpipHttpComponent.createBooleanSymbol("TCPIP_HTTP_USE_BASE64_DECODE", tcpipHttpAdvSettings)
    tcpipHttpBase64Decode.setLabel("Use Base 64 Decode")
    tcpipHttpBase64Decode.setVisible(True)
    tcpipHttpBase64Decode.setDescription("Use Base 64 Decode")
    tcpipHttpBase64Decode.setDefaultValue(True)
    

    # Enable Basic Authentication Support
    tcpipHttpAuth = tcpipHttpComponent.createBooleanSymbol("TCPIP_HTTP_USE_AUTHENTICATION", tcpipHttpAdvSettings)
    tcpipHttpAuth.setLabel("Enable Basic Authentication Support")
    tcpipHttpAuth.setVisible(True)
    tcpipHttpAuth.setDescription("Enable Basic Authentication Support")
    tcpipHttpAuth.setDefaultValue(True)
    tcpipHttpBase64Decode.setDependencies(tcpipHttpBase64DecodeOpt, ["TCPIP_HTTP_USE_AUTHENTICATION"])

    # Require Secure Connection Before Requesting a Password
    tcpipHttpNoAuthWithoutSsl = tcpipHttpComponent.createBooleanSymbol("TCPIP_HTTP_NO_AUTH_WITHOUT_SSL", tcpipHttpAdvSettings)
    tcpipHttpNoAuthWithoutSsl.setLabel("No Secure Connection without Authentication")
    tcpipHttpNoAuthWithoutSsl.setVisible(True)
    tcpipHttpNoAuthWithoutSsl.setDescription("Require Secure Connection Before Requesting a Password")
    tcpipHttpNoAuthWithoutSsl.setDefaultValue(False)
    
    # HTTP allocation function, malloc style
    tcpipHttpMallocFunction = tcpipHttpComponent.createStringSymbol("TCPIP_HTTP_MALLOC_FUNC", tcpipHttpAdvSettings)
    tcpipHttpMallocFunction.setLabel("HTTP allocation function, malloc style")
    tcpipHttpMallocFunction.setVisible(True)
    tcpipHttpMallocFunction.setDescription("HTTP allocation function, malloc style")
    tcpipHttpMallocFunction.setDefaultValue("malloc")

    # HTTP deallocation function, free style
    tcpipHttpFreeFunction = tcpipHttpComponent.createStringSymbol("TCPIP_HTTP_FREE_FUNC", tcpipHttpAdvSettings)
    tcpipHttpFreeFunction.setLabel("HTTP deallocation function, free style")
    tcpipHttpFreeFunction.setVisible(True)
    tcpipHttpFreeFunction.setDescription("HTTP deallocation function, free style")
    tcpipHttpFreeFunction.setDefaultValue("free")

    
    tcpipHttpheapdependency = ["TCPIP_HTTP_MAX_CONNECTIONS", "TCPIP_HTTP_MAX_DATA_LEN", "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]    
        
    # HTTP Heap Size
    tcpipHttpHeapSize = tcpipHttpComponent.createIntegerSymbol("TCPIP_HTTP_HEAP_SIZE", None)
    tcpipHttpHeapSize.setLabel("HTTP Heap Size (bytes)") 
    tcpipHttpHeapSize.setVisible(False)
    tcpipHttpHeapSize.setDefaultValue(tcpipHttpHeapCalc())
    tcpipHttpHeapSize.setReadOnly(True)
    tcpipHttpHeapSize.setDependencies(tcpipHttpHeapUpdate, tcpipHttpheapdependency)   
    
    
    #Add to system_config.h
    tcpipHttpHeaderFtl = tcpipHttpComponent.createFileSymbol(None, None)
    tcpipHttpHeaderFtl.setSourcePath("tcpip/config/http.h.ftl")
    tcpipHttpHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipHttpHeaderFtl.setMarkup(True)
    tcpipHttpHeaderFtl.setType("STRING")

    # Add http.c file
    tcpipHttpSourceFile = tcpipHttpComponent.createFileSymbol(None, None)
    tcpipHttpSourceFile.setSourcePath("tcpip/src/http.c")
    tcpipHttpSourceFile.setOutputName("http.c")
    tcpipHttpSourceFile.setOverwrite(True)
    tcpipHttpSourceFile.setDestPath("library/tcpip/src/")
    tcpipHttpSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipHttpSourceFile.setType("SOURCE")
    tcpipHttpSourceFile.setEnabled(True)
    tcpipHttpSourceFile.setDependencies(tcpipHttpGenSourceFile, ["TCPIP_STACK_USE_HTTP_SERVER"])

    # ifblock TCPIP_HTTP_CUSTOM_TEMPLATE

    # template HTTP_PRINT_C "$HARMONY_VERSION_PATH/framework/tcpip/config/custom_app/http_print.c.ftl" to "$PROJECT_SOURCE_FILES/app/http_print.c"
    tcpipHttpPrintSourceFile = tcpipHttpComponent.createFileSymbol(None, None)
    tcpipHttpPrintSourceFile.setSourcePath("tcpip/config/custom_app/http_print.c.ftl")
    tcpipHttpPrintSourceFile.setOutputName("http_print.c")
    tcpipHttpPrintSourceFile.setDestPath("../../")
    tcpipHttpPrintSourceFile.setProjectPath("")
    tcpipHttpPrintSourceFile.setType("SOURCE")
    tcpipHttpPrintSourceFile.setMarkup(True)
    tcpipHttpPrintSourceFile.setDependencies(tcpipHttpGenSourceFile, ["TCPIP_HTTP_CUSTOM_TEMPLATE"])
    
    # template HTTP_CUSTOM_APP_C "$HARMONY_VERSION_PATH/framework/tcpip/config/custom_app/custom_http_app.c.ftl" to "$PROJECT_SOURCE_FILES/app/custom_http_app.c"
    tcpipHttpCstmAppSourceFile = tcpipHttpComponent.createFileSymbol(None, None)
    tcpipHttpCstmAppSourceFile.setSourcePath("tcpip/config/custom_app/custom_http_app.c.ftl")
    tcpipHttpCstmAppSourceFile.setOutputName("custom_http_app.c")
    tcpipHttpCstmAppSourceFile.setDestPath("../../")
    tcpipHttpCstmAppSourceFile.setProjectPath("")   
    tcpipHttpCstmAppSourceFile.setType("SOURCE")
    tcpipHttpCstmAppSourceFile.setMarkup(True)
    tcpipHttpCstmAppSourceFile.setDependencies(tcpipHttpGenSourceFile, ["TCPIP_HTTP_CUSTOM_TEMPLATE"])
    
    # ifblock TCPIP_HTTP_CUSTOM_TEMPLATE_SL
    # template HTTP_MPFS_IMG "$HARMONY_VERSION_PATH/framework/tcpip/config/custom_app/mpfs_img2.c.ftl" to "$PROJECT_SOURCE_FILES/app/mpfs_img2.c"
    # endif
    tcpipHttpMpfsImg2SourceFile = tcpipHttpComponent.createFileSymbol(None, None)
    tcpipHttpMpfsImg2SourceFile.setSourcePath("tcpip/config/custom_app/mpfs_img2.c.ftl")
    tcpipHttpMpfsImg2SourceFile.setOutputName("mpfs_img2.c")
    tcpipHttpMpfsImg2SourceFile.setDestPath("../../")
    tcpipHttpMpfsImg2SourceFile.setProjectPath("")
    tcpipHttpMpfsImg2SourceFile.setType("SOURCE")
    tcpipHttpMpfsImg2SourceFile.setMarkup(True)
    tcpipHttpMpfsImg2SourceFile.setEnabled(False)
    tcpipHttpMpfsImg2SourceFile.setDependencies(tcpipHttpGenSourceFile, ["TCPIP_HTTP_CUSTOM_TEMPLATE_SL"])


def tcpipHttpHeapCalc(): 
    nConnections = Database.getSymbolValue("tcpipHttp","TCPIP_HTTP_MAX_CONNECTIONS")
    dataLen = Database.getSymbolValue("tcpipHttp","TCPIP_HTTP_MAX_DATA_LEN")
    heap_size = nConnections * (360 + dataLen)
    return heap_size    
    
def tcpipHttpHeapUpdate(symbol, event): 
    heap_size = tcpipHttpHeapCalc()
    symbol.setValue(heap_size)
    if(event["id"] == "TCPIP_STACK_HEAP_CALC_MASK"):
        symbol.setVisible(event["value"])
    
    
# make Http Server option visible
def tcpipHttpSrvVisible(tcpipDependentSymbol, tcpipIPSymbol):   
    tcpipHttpNet = Database.getSymbolValue("tcpipHttpNet","TCPIP_STACK_USE_HTTP_NET_SERVER")
    tcpipTcp = Database.getSymbolValue("tcpipTcp","TCPIP_USE_TCP")

    if(tcpipTcp):
        tcpipDependentSymbol.setVisible(True)       
        if(tcpipHttpNet):           
            tcpipDependentSymbol.setReadOnly(True)
        else:           
            tcpipDependentSymbol.setReadOnly(False)
    else:
        tcpipDependentSymbol.setVisible(False)  

        
def tcpipHttpFileUploadVisible(symbol, event):  
    if (event["value"] == True):                
        symbol.setVisible(True)
    else:       
        symbol.setVisible(False)
        
        
def tcpipHttpMenuVisibleSingle(symbol, event):
    if (event["value"] == True):    
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def tcpipHttpBase64DecodeOpt(symbol, event):
    symbol.clearValue()
    if (event["value"] == True):        
        symbol.setValue(True,2)
    else:
        symbol.setValue(False,2)


def tcpipHttpCustomSlSet(symbol, event):
    symbol.clearValue()
    if (event["value"] == True):
        symbol.setValue(True,2)
    else:
        symbol.setValue(False,2)
        
def tcpipHttpGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])

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
    
def destroyComponent(component):
    Database.setSymbolValue("tcpipHttp", "TCPIP_STACK_USE_HTTP_SERVER", False, 2)
    