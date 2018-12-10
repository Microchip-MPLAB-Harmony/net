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

	# Maximum Header Length
	tcpipHttpHeaderLenMax = tcpipHttpComponent.createIntegerSymbol("TCPIP_HTTP_MAX_HEADER_LEN", None)
	tcpipHttpHeaderLenMax.setLabel("Max Header Length")
	tcpipHttpHeaderLenMax.setVisible(True)
	tcpipHttpHeaderLenMax.setDescription("Maximum Header Length")
	tcpipHttpHeaderLenMax.setDefaultValue(15)
	#tcpipHttpHeaderLenMax.setDependencies(tcpipHttpMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_SERVER"])

	# Maximum Lifetime of Static Responses in Seconds
	tcpipHttpCacheLen = tcpipHttpComponent.createStringSymbol("TCPIP_HTTP_CACHE_LEN", None)
	tcpipHttpCacheLen.setLabel("Max Lifetime of Static Responses in Seconds")
	tcpipHttpCacheLen.setVisible(True)
	tcpipHttpCacheLen.setDescription("Maximum Lifetime of Static Responses in Seconds")
	tcpipHttpCacheLen.setDefaultValue("600")
	#tcpipHttpCacheLen.setDependencies(tcpipHttpMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_SERVER"])

	# Socket Disconnect Time-out
	tcpipHttpSktDisconnectTimeout= tcpipHttpComponent.createIntegerSymbol("TCPIP_HTTP_TIMEOUT", None)
	tcpipHttpSktDisconnectTimeout.setLabel("Socket Disconnect Time-out")
	tcpipHttpSktDisconnectTimeout.setVisible(True)
	tcpipHttpSktDisconnectTimeout.setDescription("Socket Disconnect Time-out")
	tcpipHttpSktDisconnectTimeout.setDefaultValue(45)
	#tcpipHttpSktDisconnectTimeout.setDependencies(tcpipHttpMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_SERVER"])

	# Maximum Number of Simultaneous Connections
	tcpipHttpConnMaxNum= tcpipHttpComponent.createIntegerSymbol("TCPIP_HTTP_MAX_CONNECTIONS", None)
	tcpipHttpConnMaxNum.setLabel("Max Number of Simultaneous Connections")
	tcpipHttpConnMaxNum.setVisible(True)
	tcpipHttpConnMaxNum.setDescription("Maximum Number of Simultaneous Connections")
	tcpipHttpConnMaxNum.setDefaultValue(4)
	#tcpipHttpConnMaxNum.setDependencies(tcpipHttpMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_SERVER"])

	# Default HTTP File
	tcpipHttpDefaultFile = tcpipHttpComponent.createStringSymbol("TCPIP_HTTP_DEFAULT_FILE", None)
	tcpipHttpDefaultFile.setLabel("Default HTTP File")
	tcpipHttpDefaultFile.setVisible(True)
	tcpipHttpDefaultFile.setDescription("Default HTTP File")
	tcpipHttpDefaultFile.setDefaultValue("index.htm")
	#tcpipHttpDefaultFile.setDependencies(tcpipHttpMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_SERVER"])

	# Default HTTPS File
	tcpipHttpsDefaultFile = tcpipHttpComponent.createStringSymbol("TCPIP_HTTPS_DEFAULT_FILE", None)
	tcpipHttpsDefaultFile.setLabel("Default HTTPS File")
	tcpipHttpsDefaultFile.setVisible(True)
	tcpipHttpsDefaultFile.setDescription("Default HTTPS File")
	tcpipHttpsDefaultFile.setDefaultValue("index.htm")
	#tcpipHttpsDefaultFile.setDependencies(tcpipHttpMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_SERVER"])

	# Maximum Default File String Length
	tcpipHttpDefaultLenMax= tcpipHttpComponent.createIntegerSymbol("TCPIP_HTTP_DEFAULT_LEN", None)
	tcpipHttpDefaultLenMax.setLabel("Max Default File String Length")
	tcpipHttpDefaultLenMax.setVisible(True)
	tcpipHttpDefaultLenMax.setDescription("Maximum Default File String Length")
	tcpipHttpDefaultLenMax.setDefaultValue(10)
	#tcpipHttpDefaultLenMax.setDependencies(tcpipHttpMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_SERVER"])

	# Enable Update via HTTP
	tcpipHttpFileUpload = tcpipHttpComponent.createBooleanSymbol("TCPIP_HTTP_FILE_UPLOAD_ENABLE", None)
	tcpipHttpFileUpload.setLabel("Enable Update via HTTP")
	tcpipHttpFileUpload.setVisible(True)
	tcpipHttpFileUpload.setDescription("Enable Update via HTTP")
	tcpipHttpFileUpload.setDefaultValue(False)
	#tcpipHttpFileUpload.setDependencies(tcpipHttpMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_SERVER"])

	# MPFS Upload Page Name
	tcpipHttpFileUploadName = tcpipHttpComponent.createStringSymbol("TCPIP_HTTP_FILE_UPLOAD_NAME", tcpipHttpFileUpload)
	tcpipHttpFileUploadName.setLabel("MPFS Upload Page Name")
	tcpipHttpFileUploadName.setVisible(False)
	tcpipHttpFileUploadName.setDescription("MPFS Upload Page Name")
	tcpipHttpFileUploadName.setDefaultValue("mpfsupload")
	tcpipHttpFileUploadName.setDependencies(tcpipHttpFileUploadVisible, ["TCPIP_HTTP_FILE_UPLOAD_ENABLE"])
	# tcpipHttpFileUploadName.setDependencies(tcpipHttpFileUploadVisible, ["TCPIP_STACK_USE_HTTP_SERVER" , "TCPIP_HTTP_FILE_UPLOAD_ENABLE"])

	# Enable POST Support
	tcpipHttpPostSupport = tcpipHttpComponent.createBooleanSymbol("TCPIP_HTTP_USE_POST", None)
	tcpipHttpPostSupport.setLabel("Enable POST Support")
	tcpipHttpPostSupport.setVisible(True)
	tcpipHttpPostSupport.setDescription("Enable POST Support")
	tcpipHttpPostSupport.setDefaultValue(True)
	#tcpipHttpPostSupport.setDependencies(tcpipHttpMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_SERVER"])

	# Enable Cookie Support
	tcpipHttpCookieSupport = tcpipHttpComponent.createBooleanSymbol("TCPIP_HTTP_USE_COOKIES", None)
	tcpipHttpCookieSupport.setLabel("Enable Cookie Support")
	tcpipHttpCookieSupport.setVisible(True)
	tcpipHttpCookieSupport.setDescription("Enable Cookie Support")
	tcpipHttpCookieSupport.setDefaultValue(True)
	#tcpipHttpCookieSupport.setDependencies(tcpipHttpMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_SERVER"])

	# Use Base 64 Decode
	tcpipHttpBase64Decode = tcpipHttpComponent.createBooleanSymbol("TCPIP_HTTP_USE_BASE64_DECODE", None)
	tcpipHttpBase64Decode.setLabel("Use Base 64 Decode")
	tcpipHttpBase64Decode.setVisible(True)
	tcpipHttpBase64Decode.setDescription("Use Base 64 Decode")
	tcpipHttpBase64Decode.setDefaultValue(True)
	

	# Enable Basic Authenication Support
	tcpipHttpAuth = tcpipHttpComponent.createBooleanSymbol("TCPIP_HTTP_USE_AUTHENTICATION", None)
	tcpipHttpAuth.setLabel("Enable Basic Authenication Support")
	tcpipHttpAuth.setVisible(True)
	tcpipHttpAuth.setDescription("Enable Basic Authenication Support")
	tcpipHttpAuth.setDefaultValue(True)
	tcpipHttpBase64Decode.setDependencies(tcpipHttpBase64DecodeOpt, ["TCPIP_HTTP_USE_AUTHENTICATION"])

	# Require Secure Connection Before Requesting a Password
	tcpipHttpNoAuthWithoutSsl = tcpipHttpComponent.createBooleanSymbol("TCPIP_HTTP_NO_AUTH_WITHOUT_SSL", None)
	tcpipHttpNoAuthWithoutSsl.setLabel("Require Secure Connection Before Requesting a Password")
	tcpipHttpNoAuthWithoutSsl.setVisible(True)
	tcpipHttpNoAuthWithoutSsl.setDescription("Require Secure Connection Before Requesting a Password")
	tcpipHttpNoAuthWithoutSsl.setDefaultValue(False)
	#tcpipHttpNoAuthWithoutSsl.setDependencies(tcpipHttpMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_SERVER"])

	# Maximum Data Length in bytes for Reading Cookie and GET/POST Arguments
	tcpipHttpDataLenMax= tcpipHttpComponent.createIntegerSymbol("TCPIP_HTTP_MAX_DATA_LEN", None)
	tcpipHttpDataLenMax.setLabel("Max Data Length (bytes) for Reading Cookie and GET/POST Arguments")
	tcpipHttpDataLenMax.setVisible(True)
	tcpipHttpDataLenMax.setDescription("Maximum Data Length in bytes for Reading Cookie and GET/POST Arguments")
	tcpipHttpDataLenMax.setDefaultValue(100)
	#tcpipHttpDataLenMax.setDependencies(tcpipHttpMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_SERVER"])

	# Minimum Number of Bytes Free in TX FIFO Before Executing Callbacks
	tcpipHttpCallbackFreeMin= tcpipHttpComponent.createIntegerSymbol("TCPIP_HTTP_MIN_CALLBACK_FREE", None)
	tcpipHttpCallbackFreeMin.setLabel("Min Number of Bytes Free in TX FIFO Before Executing Callbacks")
	tcpipHttpCallbackFreeMin.setVisible(True)
	tcpipHttpCallbackFreeMin.setDescription("Minimum Number of Bytes Free in TX FIFO Before Executing Callbacks")
	tcpipHttpCallbackFreeMin.setDefaultValue(16)
	#tcpipHttpCallbackFreeMin.setDependencies(tcpipHttpMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_SERVER"])

	# HTTP Socket Transmit Buffer Size
	tcpipHttpSktTxBuffSize= tcpipHttpComponent.createIntegerSymbol("TCPIP_HTTP_SKT_TX_BUFF_SIZE", None)
	tcpipHttpSktTxBuffSize.setLabel("HTTP Socket TX Buffer Size")
	tcpipHttpSktTxBuffSize.setVisible(True)
	tcpipHttpSktTxBuffSize.setDescription("HTTP Socket Transmit Buffer Size")
	tcpipHttpSktTxBuffSize.setDefaultValue(0)
	#tcpipHttpSktTxBuffSize.setDependencies(tcpipHttpMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_SERVER"])

	# HTTP Socket Receive Buffer Size
	tcpipHttpSktRxBuffSize= tcpipHttpComponent.createIntegerSymbol("TCPIP_HTTP_SKT_RX_BUFF_SIZE", None)
	tcpipHttpSktRxBuffSize.setLabel("HTTP Socket RX Buffer Size")
	tcpipHttpSktRxBuffSize.setVisible(True)
	tcpipHttpSktRxBuffSize.setDescription("HTTP Socket Receive Buffer Size")
	tcpipHttpSktRxBuffSize.setDefaultValue(0)
	#tcpipHttpSktRxBuffSize.setDependencies(tcpipHttpMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_SERVER"])

	# HTTP Module Configuration Flags
	tcpipHttpConfigFlags = tcpipHttpComponent.createComboSymbol("TCPIP_HTTP_CFG_FLAGS", None, TCPIP_HTTP_MODULE_FLAGS)
	tcpipHttpConfigFlags.setLabel("HTTP Module Configuration Flags")
	tcpipHttpConfigFlags.setVisible(True)
	tcpipHttpConfigFlags.setDescription("HTTP Module Configuration Flags")
	tcpipHttpConfigFlags.setDefaultValue("TCPIP_HTTP_MODULE_FLAG_ADJUST_SKT_FIFOS")
	#tcpipHttpConfigFlags.setDependencies(tcpipHttpMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_SERVER"])

	# HTTP Config Flag 
	tcpipHttpConfigFlag = tcpipHttpComponent.createIntegerSymbol("TCPIP_HTTP_CONFIG_FLAGS", None)
	tcpipHttpConfigFlag.setVisible(False)	
	tcpipHttpConfigFlag.setDefaultValue(1)
		
	# HTTP Task Rate in ms
	tcpipHttpTskRate= tcpipHttpComponent.createIntegerSymbol("TCPIP_HTTP_TASK_RATE", None)
	tcpipHttpTskRate.setLabel("HTTP Task Rate - ms")
	tcpipHttpTskRate.setVisible(True)
	tcpipHttpTskRate.setDescription("HTTP Task Rate in ms")
	tcpipHttpTskRate.setDefaultValue(33)
	#tcpipHttpTskRate.setDependencies(tcpipHttpMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_SERVER"])

	# HTTP allocation function, malloc style
	tcpipHttpMallocFunction = tcpipHttpComponent.createStringSymbol("TCPIP_HTTP_MALLOC_FUNC", None)
	tcpipHttpMallocFunction.setLabel("HTTP allocation function, malloc style")
	tcpipHttpMallocFunction.setVisible(True)
	tcpipHttpMallocFunction.setDescription("HTTP allocation function, malloc style")
	#tcpipHttpMallocFunction.setDependencies(tcpipHttpMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_SERVER"])

	# HTTP deallocation function, free style
	tcpipHttpFreeFunction = tcpipHttpComponent.createStringSymbol("TCPIP_HTTP_FREE_FUNC", None)
	tcpipHttpFreeFunction.setLabel("HTTP deallocation function, free style")
	tcpipHttpFreeFunction.setVisible(True)
	tcpipHttpFreeFunction.setDescription("HTTP deallocation function, free style")
	#tcpipHttpFreeFunction.setDependencies(tcpipHttpMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_SERVER"])

	# Include HTTP Custom Template
	tcpipHttpCustomTemplate = tcpipHttpComponent.createBooleanSymbol("TCPIP_HTTP_CUSTOM_TEMPLATE", None)
	tcpipHttpCustomTemplate.setLabel("Include HTTP Custom Template")
	tcpipHttpCustomTemplate.setVisible(True)
	tcpipHttpCustomTemplate.setDescription("Include HTTP Custom Template")
	tcpipHttpCustomTemplate.setDefaultValue(True)
	#tcpipHttpCustomTemplate.setDependencies(tcpipHttpMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_SERVER"])

	tcpipHttpCustomTemplateSl = tcpipHttpComponent.createBooleanSymbol("TCPIP_HTTP_CUSTOM_TEMPLATE_SL", None)
	tcpipHttpCustomTemplateSl.setVisible(False)	
	tcpipHttpCustomTemplateSl.setDefaultValue((Database.getSymbolValue("sys_fs", "SYS_FS_MPFS") == True))
	tcpipHttpCustomTemplateSl.setDependencies(tcpipHttpCustomSlSet, ["sys_fs.SYS_FS_MPFS"])
	
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

# make Http Module file upload name option visible
# def tcpipHttpFileUploadVisible(tcpipDependentSymbol, tcpipIPSymbol):	
	# tcpipHttp = Database.getSymbolValue("tcpipHttp","TCPIP_STACK_USE_HTTP_SERVER")
	# tcpipHttpFileUpload = Database.getSymbolValue("tcpipHttp","TCPIP_HTTP_FILE_UPLOAD_ENABLE")

	# if(tcpipHttp and tcpipHttpFileUpload):
		# tcpipDependentSymbol.setVisible(True)
	# else:
		# tcpipDependentSymbol.setVisible(False)
		
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

def destroyComponent(component):
	Database.setSymbolValue("tcpipHttp", "TCPIP_STACK_USE_HTTP_SERVER", False, 2)