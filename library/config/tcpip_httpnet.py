
		
def instantiateComponent(tcpipHttpNetComponent):
	print("TCPIP HTTP NET Component")
	configName = Variables.get("__CONFIGURATION_NAME")			
	# Enable HTTP NET Server
	tcpipHttpNetSrv = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_STACK_USE_HTTP_NET_SERVER", None)
	tcpipHttpNetSrv.setLabel("HTTP NET Server")
	tcpipHttpNetSrv.setVisible(True)
	tcpipHttpNetSrv.setDescription("Enable HTTP NET Server")
	tcpipHttpNetSrv.setDefaultValue(False)
	# Niyas to do  
		# select USE_SYS_FS_NEEDED
		# select NET_PRES_NEEDED
		# select USE_CRYPTO_LIB_NEEDED
		# select USE_CRYPTO_MD5_NEEDED
		# select USE_CRYPTO_RANDOM_NEEDED
	tcpipHttpNetSrv.setDependencies(tcpipHttpNetSrvVisible, ["tcpipHttp.TCPIP_STACK_USE_HTTP_SERVER" , "tcpipTcp.TCPIP_USE_TCP"]) # Niyas to verify the dependency function

	# Maximum Header Length
	tcpipHttpNetHdrLenMax = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_MAX_HEADER_LEN", tcpipHttpNetSrv)
	tcpipHttpNetHdrLenMax.setLabel("Max Header Length")
	tcpipHttpNetHdrLenMax.setVisible(False)
	tcpipHttpNetHdrLenMax.setDescription("Maximum Header Length")
	tcpipHttpNetHdrLenMax.setDefaultValue(15)
	tcpipHttpNetHdrLenMax.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Maximum Lifetime of Static Responses in Seconds
	tcpipHttpNetCacheLen = tcpipHttpNetComponent.createStringSymbol("TCPIP_HTTP_NET_CACHE_LEN", tcpipHttpNetSrv)
	tcpipHttpNetCacheLen.setLabel("Max Lifetime of Static Responses in Seconds")
	tcpipHttpNetCacheLen.setVisible(False)
	tcpipHttpNetCacheLen.setDescription("Maximum Lifetime of Static Responses in Seconds")
	tcpipHttpNetCacheLen.setDefaultValue("600")
	tcpipHttpNetCacheLen.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Socket Disconnect Time-out
	tcpipHttpNetTimeout = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_TIMEOUT", tcpipHttpNetSrv)
	tcpipHttpNetTimeout.setLabel("Socket Disconnect Time-out")
	tcpipHttpNetTimeout.setVisible(False)
	tcpipHttpNetTimeout.setDescription("Socket Disconnect Time-out")
	tcpipHttpNetTimeout.setDefaultValue(45)
	tcpipHttpNetTimeout.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Maximum Number of Simultaneous Connections
	tcpipHttpNetConnMaxNum = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_MAX_CONNECTIONS", tcpipHttpNetSrv)
	tcpipHttpNetConnMaxNum.setLabel("Max Number of Simultaneous Connections")
	tcpipHttpNetConnMaxNum.setVisible(False)
	tcpipHttpNetConnMaxNum.setDescription("Maximum Number of Simultaneous Connections")
	tcpipHttpNetConnMaxNum.setDefaultValue(4)
	tcpipHttpNetConnMaxNum.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Default HTTP NET File
	tcpipHttpNetDefaultFile = tcpipHttpNetComponent.createStringSymbol("TCPIP_HTTP_NET_DEFAULT_FILE", tcpipHttpNetSrv)
	tcpipHttpNetDefaultFile.setLabel("Default HTTP NET File")
	tcpipHttpNetDefaultFile.setVisible(False)
	tcpipHttpNetDefaultFile.setDescription("Default HTTP NET File")
	tcpipHttpNetDefaultFile.setDefaultValue("index.htm")
	tcpipHttpNetDefaultFile.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Maximum Size of a HTTP File Name
	tcpipHttpNetFilenameLenMax = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_FILENAME_MAX_LEN", tcpipHttpNetSrv)
	tcpipHttpNetFilenameLenMax.setLabel("Maximum Size of a HTTP File Name")
	tcpipHttpNetFilenameLenMax.setVisible(False)
	tcpipHttpNetFilenameLenMax.setDescription("Maximum Size of a HTTP File Name")
	tcpipHttpNetFilenameLenMax.setDefaultValue(25)
	tcpipHttpNetFilenameLenMax.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Enable Update via HTTP NET
	tcpipHttpNetFileUpload = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_FILE_UPLOAD_ENABLE", tcpipHttpNetSrv)
	tcpipHttpNetFileUpload.setLabel("Enable Update via HTTP NET")
	tcpipHttpNetFileUpload.setVisible(False)
	tcpipHttpNetFileUpload.setDescription("Enable Update via HTTP NET")
	tcpipHttpNetFileUpload.setDefaultValue(False)
	tcpipHttpNetFileUpload.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# MPFS Upload Page Name
	tcpipHttpNetFileUploadName = tcpipHttpNetComponent.createStringSymbol("TCPIP_HTTP_NET_FILE_UPLOAD_NAME", tcpipHttpNetFileUpload)
	tcpipHttpNetFileUploadName.setLabel("MPFS Upload Page Name")
	tcpipHttpNetFileUploadName.setVisible(False)
	tcpipHttpNetFileUploadName.setDescription("MPFS Upload Page Name")
	tcpipHttpNetFileUploadName.setDefaultValue("mpfsupload")
	tcpipHttpNetFileUploadName.setDependencies(tcpipHttpNetFileUploadVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_FILE_UPLOAD_ENABLE"])

	# Enable POST Support
	tcpipHttpNetPostSupport = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_USE_POST", tcpipHttpNetSrv)
	tcpipHttpNetPostSupport.setLabel("Enable POST Support")
	tcpipHttpNetPostSupport.setVisible(False)
	tcpipHttpNetPostSupport.setDescription("Enable POST Support")
	tcpipHttpNetPostSupport.setDefaultValue(True)
	tcpipHttpNetPostSupport.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Enable Cookie Support
	tcpipHttpNetCookieSupport = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_USE_COOKIES", tcpipHttpNetSrv)
	tcpipHttpNetCookieSupport.setLabel("Enable Cookie Support")
	tcpipHttpNetCookieSupport.setVisible(False)
	tcpipHttpNetCookieSupport.setDescription("Enable Cookie Support")
	tcpipHttpNetCookieSupport.setDefaultValue(True)
	tcpipHttpNetCookieSupport.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Use Base 64 Decode
	tcpipHttpNetBase64Decode = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_USE_BASE64_DECODE", tcpipHttpNetSrv)
	tcpipHttpNetBase64Decode.setLabel("Use Base 64 Decode")
	tcpipHttpNetBase64Decode.setVisible(False)
	tcpipHttpNetBase64Decode.setDescription("Use Base 64 Decode")
	tcpipHttpNetBase64Decode.setDefaultValue(False) # Niyas to check the default value
	tcpipHttpNetBase64Decode.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Enable Basic Authenication Support
	tcpipHttpNetAuth = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_USE_AUTHENTICATION", tcpipHttpNetSrv)
	tcpipHttpNetAuth.setLabel("Enable Basic Authenication Support")
	tcpipHttpNetAuth.setVisible(False)
	tcpipHttpNetAuth.setDescription("Enable Basic Authenication Support")
	tcpipHttpNetAuth.setDefaultValue(True)
	# Niyas to do
	# select TCPIP_HTTP_NET_USE_BASE64_DECODE
	tcpipHttpNetAuth.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Maximum Data Length (bytes) for Reading Cookie and GET/POST Arguments
	tcpipHttpNetDataLenMax = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_MAX_DATA_LEN", tcpipHttpNetSrv)
	tcpipHttpNetDataLenMax.setLabel("Max Data Length (bytes) for Reading Cookie and GET/POST Arguments")
	tcpipHttpNetDataLenMax.setVisible(False)
	tcpipHttpNetDataLenMax.setDescription("Maximum Data Length (bytes) for Reading Cookie and GET/POST Arguments")
	tcpipHttpNetDataLenMax.setDefaultValue(100)
	tcpipHttpNetDataLenMax.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# HTTP NET Socket TX Buffer Size
	tcpipHttpNetSktTxBuffsize = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_SKT_TX_BUFF_SIZE", tcpipHttpNetSrv)
	tcpipHttpNetSktTxBuffsize.setLabel("HTTP NET Socket TX Buffer Size")
	tcpipHttpNetSktTxBuffsize.setVisible(False)
	tcpipHttpNetSktTxBuffsize.setDescription("HTTP NET Socket TX Buffer Size")
	tcpipHttpNetSktTxBuffsize.setDefaultValue(1024)
	tcpipHttpNetSktTxBuffsize.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# HTTP NET Socket RX Buffer Size
	tcpipHttpNetSktRxBuffsize = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_SKT_RX_BUFF_SIZE", tcpipHttpNetSrv)
	tcpipHttpNetSktRxBuffsize.setLabel("HTTP NET Socket RX Buffer Size")
	tcpipHttpNetSktRxBuffsize.setVisible(False)
	tcpipHttpNetSktRxBuffsize.setDescription("HTTP NET Socket RX Buffer Size")
	tcpipHttpNetSktRxBuffsize.setDefaultValue(1024)
	tcpipHttpNetSktRxBuffsize.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# HTTP NET Listening Port
	tcpipHttpNetListenPort = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_LISTEN_PORT", tcpipHttpNetSrv)
	tcpipHttpNetListenPort.setLabel("HTTP NET Listening Port")
	tcpipHttpNetListenPort.setVisible(False)
	tcpipHttpNetListenPort.setDescription("HTTP NET Listening Port")
	tcpipHttpNetListenPort.setDefaultValue(80)
	tcpipHttpNetListenPort.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# HTTP NET Configuration Flags Settings
	tcpipHttpNetConfigFlag = tcpipHttpNetComponent.createMenuSymbol(None, tcpipHttpNetSrv)
	tcpipHttpNetConfigFlag.setLabel("HTTP NET Configuration Flags")
	tcpipHttpNetConfigFlag.setVisible(True)
	tcpipHttpNetConfigFlag.setDescription("HTTP NET Configuration Flags Settings")
	tcpipHttpNetConfigFlag.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Use non-persistent connections
	tcpipHttpNetConfigFlagNonpersistent = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_CONFIG_FLAG_NON_PERSISTENT", tcpipHttpNetConfigFlag)
	tcpipHttpNetConfigFlagNonpersistent.setLabel("Use non-persistent connections")
	tcpipHttpNetConfigFlagNonpersistent.setVisible(False)
	tcpipHttpNetConfigFlagNonpersistent.setDescription("Use non-persistent connections")
	tcpipHttpNetConfigFlagNonpersistent.setDefaultValue(False)
	tcpipHttpNetConfigFlagNonpersistent.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# HTTP sockets created with NO-DELAY option
	tcpipHttpNetConfigFlagNoDly = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_CONFIG_FLAG_NO_DELAY", tcpipHttpNetConfigFlag)
	tcpipHttpNetConfigFlagNoDly.setLabel("HTTP sockets created with NO-DELAY option")
	tcpipHttpNetConfigFlagNoDly.setVisible(False)
	tcpipHttpNetConfigFlagNoDly.setDescription("HTTP sockets created with NO-DELAY option")
	tcpipHttpNetConfigFlagNoDly.setDefaultValue(False)
	tcpipHttpNetConfigFlagNoDly.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# All HTTP connections have to be secure
	tcpipHttpNetConfigFlagSecureOn = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_ON", tcpipHttpNetConfigFlag)
	tcpipHttpNetConfigFlagSecureOn.setLabel("All HTTP connections have to be secure")
	tcpipHttpNetConfigFlagSecureOn.setVisible(False)
	tcpipHttpNetConfigFlagSecureOn.setDescription("All HTTP connections have to be secure")
	tcpipHttpNetConfigFlagSecureOn.setDefaultValue(False)

	# All HTTP connections have to be non-secure
	tcpipHttpNetConfigFlagSecureOff = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_OFF", tcpipHttpNetConfigFlag)
	tcpipHttpNetConfigFlagSecureOff.setLabel("All HTTP connections have to be non-secure")
	tcpipHttpNetConfigFlagSecureOff.setVisible(False)
	tcpipHttpNetConfigFlagSecureOff.setDescription("All HTTP connections have to be non-secure")
	tcpipHttpNetConfigFlagSecureOff.setDefaultValue(False)

	# HTTP security is based on the port numbers
	tcpipHttpNetConfigFlagSecureDefault = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_DEFAULT", tcpipHttpNetConfigFlag)
	tcpipHttpNetConfigFlagSecureDefault.setLabel("HTTP security is based on the port numbers")
	tcpipHttpNetConfigFlagSecureDefault.setVisible(False)
	tcpipHttpNetConfigFlagSecureDefault.setDescription("HTTP security is based on the port numbers")
	tcpipHttpNetConfigFlagSecureDefault.setDefaultValue(True)

	# Niyas to verify this dependency
	tcpipHttpNetConfigFlagSecureDefault.setDependencies(tcpipHttpNetSrvConfigFlagSecureDefaultEnable, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_OFF", "TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_ON"])
	tcpipHttpNetConfigFlagSecureOn.setDependencies(tcpipHttpNetSrvConfigFlagSecureOnEnable, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_OFF", "TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_DEFAULT"])
	tcpipHttpNetConfigFlagSecureOff.setDependencies(tcpipHttpNetSrvConfigFlagSecureOffEnable, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_ON", "TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_DEFAULT"])

	# HTTP NET Task Rate - ms
	tcpipHttpNetTskRate = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_TASK_RATE", tcpipHttpNetSrv)
	tcpipHttpNetTskRate.setLabel("HTTP NET Task Rate - ms")
	tcpipHttpNetTskRate.setVisible(False)
	tcpipHttpNetTskRate.setDescription("HTTP NET Task Rate - ms")
	tcpipHttpNetTskRate.setDefaultValue(33)
	tcpipHttpNetTskRate.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Size of the Buffer Used for Sending Response Messages to the Client
	tcpipHttpNetRespBuffSize = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_RESPONSE_BUFFER_SIZE", tcpipHttpNetSrv)
	tcpipHttpNetRespBuffSize.setLabel("Size of the Buffer Used for Sending Response Messages to the Client")
	tcpipHttpNetRespBuffSize.setVisible(False)
	tcpipHttpNetRespBuffSize.setDescription("Size of the Buffer Used for Sending Response Messages to the Client")
	tcpipHttpNetRespBuffSize.setDefaultValue(300)
	tcpipHttpNetRespBuffSize.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Size of the Buffer Used for Sending Cookies to the Client
	tcpipHttpNetCookieBuffSize = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_COOKIE_BUFFER_SIZE", tcpipHttpNetSrv)
	tcpipHttpNetCookieBuffSize.setLabel("Size of the Buffer Used for Sending Cookies to the Client")
	tcpipHttpNetCookieBuffSize.setVisible(False)
	tcpipHttpNetCookieBuffSize.setDescription("Size of the Buffer Used for Sending Cookies to the Client")
	tcpipHttpNetCookieBuffSize.setDefaultValue(200)
	tcpipHttpNetCookieBuffSize.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Size of the Peek Buffer for Performing Searches
	tcpipHttpNetFindPeekBuffSize = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_FIND_PEEK_BUFF_SIZE", tcpipHttpNetSrv)
	tcpipHttpNetFindPeekBuffSize.setLabel("Size of the Peek Buffer for Performing Searches")
	tcpipHttpNetFindPeekBuffSize.setVisible(False)
	tcpipHttpNetFindPeekBuffSize.setDescription("Size of the Peek Buffer for Performing Searches")
	tcpipHttpNetFindPeekBuffSize.setDefaultValue(512)
	tcpipHttpNetFindPeekBuffSize.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Size of the Buffer for Processing HTML, Dynamic Variable and Binary Files
	tcpipHttpNetFileProcessBuffSize = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_FILE_PROCESS_BUFFER_SIZE", tcpipHttpNetSrv)
	tcpipHttpNetFileProcessBuffSize.setLabel("Size of the Buffer for Processing HTML, Dynamic Variable and Binary Files")
	tcpipHttpNetFileProcessBuffSize.setVisible(False)
	tcpipHttpNetFileProcessBuffSize.setDescription("Size of the Buffer for Processing HTML, Dynamic Variable and Binary Files")
	tcpipHttpNetFileProcessBuffSize.setDefaultValue(512)
	tcpipHttpNetFileProcessBuffSize.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Number of File Buffers to be Created
	tcpipHttpNetFileProcessBuffNum = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_FILE_PROCESS_BUFFERS_NUMBER", tcpipHttpNetSrv)
	tcpipHttpNetFileProcessBuffNum.setLabel("Number of File Buffers to be Created")
	tcpipHttpNetFileProcessBuffNum.setVisible(False)
	tcpipHttpNetFileProcessBuffNum.setDescription("Number of File Buffers to be Created")
	tcpipHttpNetFileProcessBuffNum.setDefaultValue(4)
	tcpipHttpNetFileProcessBuffNum.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Retry Limit for Allocating a File Buffer from the Pool
	tcpipHttpNetFileProcessBuffRetry = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_FILE_PROCESS_BUFFER_RETRIES", tcpipHttpNetSrv)
	tcpipHttpNetFileProcessBuffRetry.setLabel("Retry Limit for Allocating a File Buffer from the Pool")
	tcpipHttpNetFileProcessBuffRetry.setVisible(False)
	tcpipHttpNetFileProcessBuffRetry.setDescription("Retry Limit for Allocating a File Buffer from the Pool")
	tcpipHttpNetFileProcessBuffRetry.setDefaultValue(10)
	tcpipHttpNetFileProcessBuffRetry.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Retry Limit for Allocating a File Buffer from the Pool
	tcpipHttpNetChunksNum = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_CHUNKS_NUMBER", tcpipHttpNetSrv)
	tcpipHttpNetChunksNum.setLabel("Number of Chunks to be Created")
	tcpipHttpNetChunksNum.setVisible(False)
	tcpipHttpNetChunksNum.setDescription("Number of Chunks to be Created")
	tcpipHttpNetChunksNum.setDefaultValue(10)
	tcpipHttpNetChunksNum.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Retry Limit for Allocating a Chunk from the Pool
	tcpipHttpNetChunkRetry = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_CHUNK_RETRIES", tcpipHttpNetSrv)
	tcpipHttpNetChunkRetry.setLabel("Retry Limit for Allocating a Chunk from the Pool")
	tcpipHttpNetChunkRetry.setVisible(False)
	tcpipHttpNetChunkRetry.setDescription("Retry Limit for Allocating a Chunk from the Pool")
	tcpipHttpNetChunkRetry.setDefaultValue(10)
	tcpipHttpNetChunkRetry.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# The Maximum Depth of Recursive Calls for Serving a Web Page
	tcpipHttpNetRecursiveLvl = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_MAX_RECURSE_LEVEL", tcpipHttpNetSrv)
	tcpipHttpNetRecursiveLvl.setLabel("The Maximum Depth of Recursive Calls for Serving a Web Page")
	tcpipHttpNetRecursiveLvl.setVisible(False)
	tcpipHttpNetRecursiveLvl.setDescription("The Maximum Depth of Recursive Calls for Serving a Web Page")
	tcpipHttpNetRecursiveLvl.setDefaultValue(3)
	tcpipHttpNetRecursiveLvl.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Enable the Processing of Dynamic Variables
	tcpipHttpNetDynVarProc = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_DYNVAR_PROCESS", tcpipHttpNetSrv)
	tcpipHttpNetDynVarProc.setLabel("Enable the Processing of Dynamic Variables")
	tcpipHttpNetDynVarProc.setVisible(False)
	tcpipHttpNetDynVarProc.setDescription("Enable the Processing of Dynamic Variables")
	tcpipHttpNetDynVarProc.setDefaultValue(True)
	tcpipHttpNetDynVarProc.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Number of the Descriptors for Dynamic Variables Processing
	tcpipHttpNetDynVarDescNum = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_DYNVAR_DESCRIPTORS_NUMBER", tcpipHttpNetDynVarProc)
	tcpipHttpNetDynVarDescNum.setLabel("Number of the Descriptors for Dynamic Variables Processing")
	tcpipHttpNetDynVarDescNum.setVisible(False)
	tcpipHttpNetDynVarDescNum.setDescription("Number of the Descriptors for Dynamic Variables Processing")
	tcpipHttpNetDynVarDescNum.setDefaultValue(10)
	tcpipHttpNetDynVarDescNum.setDependencies(tcpipHttpNetDynVarVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_DYNVAR_PROCESS"])

	# Maximum Size for a Complete Dynamic Variable: Name + Args
	tcpipHttpNetDynVarLenMax = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_DYNVAR_MAX_LEN", tcpipHttpNetDynVarProc)
	tcpipHttpNetDynVarLenMax.setLabel("Maximum Size for a Complete Dynamic Variable: Name + Args")
	tcpipHttpNetDynVarLenMax.setVisible(False)
	tcpipHttpNetDynVarLenMax.setDescription("Maximum Size for a Complete Dynamic Variable: Name + Args")
	tcpipHttpNetDynVarLenMax.setDefaultValue(50)
	tcpipHttpNetDynVarLenMax.setDependencies(tcpipHttpNetDynVarVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_DYNVAR_PROCESS"])

	# Maximum Number of Arguments for a Dynamic Variable
	tcpipHttpNetDynVarArgNumMax = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_DYNVAR_ARG_MAX_NUMBER", tcpipHttpNetDynVarProc)
	tcpipHttpNetDynVarArgNumMax.setLabel("Maximum Number of Arguments for a Dynamic Variable")
	tcpipHttpNetDynVarArgNumMax.setVisible(False)
	tcpipHttpNetDynVarArgNumMax.setDescription("Maximum Number of Arguments for a Dynamic Variable")
	tcpipHttpNetDynVarArgNumMax.setDefaultValue(4)
	tcpipHttpNetDynVarArgNumMax.setDependencies(tcpipHttpNetDynVarVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_DYNVAR_PROCESS"])

	# Retry Limit for a Dynamic Variable Processing
	tcpipHttpNetDynVarProcRetry = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_DYNVAR_PROCESS_RETRIES", tcpipHttpNetDynVarProc)
	tcpipHttpNetDynVarProcRetry.setLabel("Retry Limit for a Dynamic Variable Processing")
	tcpipHttpNetDynVarProcRetry.setVisible(False)
	tcpipHttpNetDynVarProcRetry.setDescription("Retry Limit for a Dynamic Variable Processing")
	tcpipHttpNetDynVarProcRetry.setDefaultValue(10)
	tcpipHttpNetDynVarProcRetry.setDependencies(tcpipHttpNetDynVarVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_DYNVAR_PROCESS"])

	# Enable the Processing of SSI Commands
	tcpipHttpNetSsiProc = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_SSI_PROCESS", tcpipHttpNetSrv)
	tcpipHttpNetSsiProc.setLabel("Enable the Processing of SSI Commands")
	tcpipHttpNetSsiProc.setVisible(False)
	tcpipHttpNetSsiProc.setDescription("Enable the Processing of SSI Commands")
	tcpipHttpNetSsiProc.setDefaultValue(True)
	tcpipHttpNetSsiProc.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Maximum Number of Attributes for a SSI Command
	tcpipHttpNetSsiAttrNumMax = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_SSI_ATTRIBUTES_MAX_NUMBER", tcpipHttpNetSsiProc)
	tcpipHttpNetSsiAttrNumMax.setLabel("Maximum Number of Attributes for a SSI Command")
	tcpipHttpNetSsiAttrNumMax.setVisible(False)
	tcpipHttpNetSsiAttrNumMax.setDescription("Maximum Number of Attributes for a SSI Command")
	tcpipHttpNetSsiAttrNumMax.setDefaultValue(4)
	tcpipHttpNetSsiAttrNumMax.setDependencies(tcpipHttpNetSsiAttrVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_SSI_PROCESS"])

	# Number of Static Attributes Associated to a SSI Command
	tcpipHttpNetSsiStaticAttrNum = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_SSI_STATIC_ATTTRIB_NUMBER", tcpipHttpNetSsiProc)
	tcpipHttpNetSsiStaticAttrNum.setLabel("Number of Static Attributes Associated to a SSI Command")
	tcpipHttpNetSsiStaticAttrNum.setVisible(False)
	tcpipHttpNetSsiStaticAttrNum.setDescription("Number of Static Attributes Associated to a SSI Command")
	tcpipHttpNetSsiStaticAttrNum.setDefaultValue(2)
	tcpipHttpNetSsiStaticAttrNum.setDependencies(tcpipHttpNetSsiAttrVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_SSI_PROCESS"])

	# Maximum Size for a SSI Command Line: Command + Attribute/Value Pairs
	tcpipHttpNetSsiCmdLenMax = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_SSI_CMD_MAX_LEN", tcpipHttpNetSsiProc)
	tcpipHttpNetSsiCmdLenMax.setLabel("Maximum Size for a SSI Command Line: Command + Attribute/Value Pairs")
	tcpipHttpNetSsiCmdLenMax.setVisible(False)
	tcpipHttpNetSsiCmdLenMax.setDescription("Maximum Size for a SSI Command Line: Command + Attribute/Value Pairs")
	tcpipHttpNetSsiCmdLenMax.setDefaultValue(100)
	tcpipHttpNetSsiCmdLenMax.setDependencies(tcpipHttpNetSsiAttrVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_SSI_PROCESS"])

	# Maximum Number of SSI Variables that Can Be Created at Run Time
	tcpipHttpNetSsiVarNum = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_SSI_VARIABLES_NUMBER", tcpipHttpNetSsiProc)
	tcpipHttpNetSsiVarNum.setLabel("Maximum Number of SSI Variables that Can Be Created at Run Time")
	tcpipHttpNetSsiVarNum.setVisible(False)
	tcpipHttpNetSsiVarNum.setDescription("Maximum Number of SSI Variables that Can Be Created at Run Time")
	tcpipHttpNetSsiVarNum.setDefaultValue(13)
	tcpipHttpNetSsiVarNum.setDependencies(tcpipHttpNetSsiAttrVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_SSI_PROCESS"])

	# Maximum Number of SSI Variables that Can Be Created at Run Time
	tcpipHttpNetSsiVarNameLenMax = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_SSI_VARIABLE_NAME_MAX_LENGTH", tcpipHttpNetSsiProc)
	tcpipHttpNetSsiVarNameLenMax.setLabel("Maximum Length of a SSI Variable Name")
	tcpipHttpNetSsiVarNameLenMax.setVisible(False)
	tcpipHttpNetSsiVarNameLenMax.setDescription("Maximum Length of a SSI Variable Name")
	tcpipHttpNetSsiVarNameLenMax.setDefaultValue(10)
	tcpipHttpNetSsiVarNameLenMax.setDependencies(tcpipHttpNetSsiAttrVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_SSI_PROCESS"])

	# Maximum Size of a SSI String Variable Value
	tcpipHttpNetSsiVarStrLenMax = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_SSI_VARIABLE_STRING_MAX_LENGTH", tcpipHttpNetSsiProc)
	tcpipHttpNetSsiVarStrLenMax.setLabel("Maximum Size of a SSI String Variable Value")
	tcpipHttpNetSsiVarStrLenMax.setVisible(False)
	tcpipHttpNetSsiVarStrLenMax.setDescription("Maximum Size of a SSI String Variable Value")
	tcpipHttpNetSsiVarStrLenMax.setDefaultValue(10)
	tcpipHttpNetSsiVarStrLenMax.setDependencies(tcpipHttpNetSsiAttrVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_SSI_PROCESS"])

	# Message to Echo when Echoing a Not Found Variable
	tcpipHttpNetSsiEchoNotFoundMsg = tcpipHttpNetComponent.createStringSymbol("TCPIP_HTTP_NET_SSI_ECHO_NOT_FOUND_MESSAGE", tcpipHttpNetSsiProc)
	tcpipHttpNetSsiEchoNotFoundMsg.setLabel("Message to Echo when Echoing a Not Found Variable")
	tcpipHttpNetSsiEchoNotFoundMsg.setVisible(False)
	tcpipHttpNetSsiEchoNotFoundMsg.setDescription("Message to Echo when Echoing a Not Found Variable")
	tcpipHttpNetSsiEchoNotFoundMsg.setDefaultValue("SSI Echo - Not Found: ")
	tcpipHttpNetSsiEchoNotFoundMsg.setDependencies(tcpipHttpNetSsiAttrVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_SSI_PROCESS"])

	# Include HTTP NET Custom Template
	tcpipHttpNetCustTemplate = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_CUSTOM_TEMPLATE", tcpipHttpNetSrv)
	tcpipHttpNetCustTemplate.setLabel("Include HTTP NET Custom Template")
	tcpipHttpNetCustTemplate.setVisible(False)
	tcpipHttpNetCustTemplate.setDescription("Include HTTP NET Custom Template")
	tcpipHttpNetCustTemplate.setDefaultValue(True)
	tcpipHttpNetCustTemplate.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])


	# Niyas to check below component
	# Include HTTP NET Custom Template SL
	tcpipHttpNetCustTemplateSl = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_CUSTOM_TEMPLATE_SL", tcpipHttpNetSrv)
	tcpipHttpNetCustTemplateSl.setVisible(False)
	tcpipHttpNetCustTemplateSl.setDescription("Include HTTP NET Custom Template SL")
	tcpipHttpNetCustTemplateSl.setDefaultValue(False)
	# # Niyas to check below
	# # default y if (USE_SYS_FS = y) && (SYS_FS_MPFS = y)
	#tcpipHttpNetCustTemplateSl.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Persistent Connection Idle Time-out
	tcpipHttpNetConnTimeout = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_CONNECTION_TIMEOUT", tcpipHttpNetSrv)
	tcpipHttpNetConnTimeout.setLabel("Persistent Connection Idle Time-out")
	tcpipHttpNetConnTimeout.setVisible(False)
	tcpipHttpNetConnTimeout.setDescription("Persistent Connection Idle Time-out")
	tcpipHttpNetConnTimeout.setDefaultValue(0)
	tcpipHttpNetConnTimeout.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# HTTP NET allocation function, malloc style
	tcpipHttpNetMallocFunct = tcpipHttpNetComponent.createStringSymbol("TCPIP_HTTP_NET_MALLOC_FUNC", tcpipHttpNetSrv)
	tcpipHttpNetMallocFunct.setLabel("HTTP NET allocation function, malloc style")
	tcpipHttpNetMallocFunct.setVisible(False)
	tcpipHttpNetMallocFunct.setDescription("HTTP NET allocation function, malloc style")
	#tcpipHttpNetMallocFunct.setDefaultValue("")
	tcpipHttpNetMallocFunct.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# HTTP NET deallocation function, free style
	tcpipHttpNetFreeFunct = tcpipHttpNetComponent.createStringSymbol("TCPIP_HTTP_NET_FREE_FUNC", tcpipHttpNetSrv)
	tcpipHttpNetFreeFunct.setLabel("HTTP NET deallocation function, free style")
	tcpipHttpNetFreeFunct.setVisible(False)
	tcpipHttpNetFreeFunct.setDescription("HTTP NET deallocation function, free style")
	#tcpipHttpNetFreeFunct.setDefaultValue("")
	tcpipHttpNetFreeFunct.setDependencies(tcpipHttpNetMenuVisibleSingle, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Niyas to check the dummy 
	# config TCPIP_STACK_USE_HTTP_NET_SERVER_DUMMY
	# bool "HTTP NET Server"
	# depends on USE_TCPIP_STACK && TCPIP_USE_TCP && TCPIP_STACK_USE_HTTP_SERVER
	# default n
	# persistent

	#Add to system_config.h
	tcpipHttpNetHeaderFtl = tcpipHttpNetComponent.createFileSymbol(None, None)
	tcpipHttpNetHeaderFtl.setSourcePath("library/config/http_net.h.ftl")
	tcpipHttpNetHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipHttpNetHeaderFtl.setMarkup(True)
	tcpipHttpNetHeaderFtl.setType("STRING")

	

	# Add http_net.c file
	tcpipHttpNetSourceFile = tcpipHttpNetComponent.createFileSymbol(None, None)
	tcpipHttpNetSourceFile.setSourcePath("library/src/http_net.c")
	tcpipHttpNetSourceFile.setOutputName("http_net.c")
	tcpipHttpNetSourceFile.setOverwrite(True)
	tcpipHttpNetSourceFile.setDestPath("library/tcpip/src/")
	tcpipHttpNetSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipHttpNetSourceFile.setType("SOURCE")
	tcpipHttpNetSourceFile.setEnabled(True)
	tcpipHttpNetSourceFile.setDependencies(tcpipHttpNetGenSourceFile, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])

	# Niyas to add
	# file DRV_NVM_STATIC_HTTP_NET_H "$HARMONY_VERSION_PATH/framework/driver/nvm/drv_nvm.h" to "$PROJECT_HEADER_FILES/framework/driver/nvm/drv_nvm.h"
	# file DRV_NVM_STATIC_LOCAL_HTTP_NET_H "$HARMONY_VERSION_PATH/framework/driver/nvm/src/drv_nvm_local.h" to "$PROJECT_HEADER_FILES/framework/driver/nvm/src/drv_nvm_local.h"
	# file DRV_NVM_STATIC_VAR_MAP_HTTP_NET_H "$HARMONY_VERSION_PATH/framework/driver/nvm/src/drv_nvm_variant_mapping.h" to "$PROJECT_HEADER_FILES/framework/driver/nvm/src/drv_nvm_variant_mapping.h"

	# file DRV_NVM_HTTP_NET_H "$HARMONY_VERSION_PATH/framework/driver/nvm/drv_nvm.h" to "$PROJECT_HEADER_FILES/framework/driver/nvm/drv_nvm.h"
	# file DRV_NVM_LOCAL_HTTP_NET_H "$HARMONY_VERSION_PATH/framework/driver/nvm/src/drv_nvm_local.h" to "$PROJECT_HEADER_FILES/framework/driver/nvm/src/drv_nvm_local.h"
	# file DRV_NVM_VAR_MAP_HTTP_NET_H "$HARMONY_VERSION_PATH/framework/driver/nvm/src/drv_nvm_variant_mapping.h" to "$PROJECT_HEADER_FILES/framework/driver/nvm/src/drv_nvm_variant_mapping.h"


	# ifblock TCPIP_HTTP_NET_CUSTOM_TEMPLATE

	# template HTTP_NET_PRINT_C "$HARMONY_VERSION_PATH/framework/tcpip/config/custom_app/http_net_print.c.ftl" to "$PROJECT_SOURCE_FILES/app/http_net_print.c"
	tcpipHttpNetPrintSourceFile = tcpipHttpNetComponent.createFileSymbol(None, None)
	tcpipHttpNetPrintSourceFile.setSourcePath("library/config/custom_app/http_net_print.c.ftl")
	tcpipHttpNetPrintSourceFile.setOutputName("http_net_print.c")
	tcpipHttpNetPrintSourceFile.setDestPath("app/")
	tcpipHttpNetPrintSourceFile.setProjectPath("app/")
	tcpipHttpNetPrintSourceFile.setType("SOURCE")
	tcpipHttpNetPrintSourceFile.setMarkup(True)
	tcpipHttpNetPrintSourceFile.setDependencies(tcpipHttpNetGenSourceFile, ["TCPIP_HTTP_NET_CUSTOM_TEMPLATE"])
	
	# template HTTP_NET_CUSTOM_APP_C "$HARMONY_VERSION_PATH/framework/tcpip/config/custom_app/custom_http_net_app.c.ftl" to "$PROJECT_SOURCE_FILES/app/custom_http_net_app.c"
	tcpipHttpNetCstmAppSourceFile = tcpipHttpNetComponent.createFileSymbol(None, None)
	tcpipHttpNetCstmAppSourceFile.setSourcePath("library/config/custom_app/custom_http_net_app.c.ftl")
	tcpipHttpNetCstmAppSourceFile.setOutputName("custom_http_net_app.c")
	tcpipHttpNetCstmAppSourceFile.setDestPath("app/")
	tcpipHttpNetCstmAppSourceFile.setProjectPath("app/")
	tcpipHttpNetCstmAppSourceFile.setType("SOURCE")
	tcpipHttpNetCstmAppSourceFile.setMarkup(True)
	tcpipHttpNetCstmAppSourceFile.setDependencies(tcpipHttpNetGenSourceFile, ["TCPIP_HTTP_NET_CUSTOM_TEMPLATE"])
	
	# ifblock TCPIP_HTTP_NET_CUSTOM_TEMPLATE_SL
	# template HTTP_NET_MPFS_IMG "$HARMONY_VERSION_PATH/framework/tcpip/config/custom_app/mpfs_img2.c.ftl" to "$PROJECT_SOURCE_FILES/app/mpfs_img2.c"
	tcpipHttpNetMpfsImg2SourceFile = tcpipHttpNetComponent.createFileSymbol(None, None)
	tcpipHttpNetMpfsImg2SourceFile.setSourcePath("library/config/custom_app/mpfs_img2.c.ftl")
	tcpipHttpNetMpfsImg2SourceFile.setOutputName("mpfs_img2.c")
	tcpipHttpNetMpfsImg2SourceFile.setDestPath("app/")
	tcpipHttpNetMpfsImg2SourceFile.setProjectPath("app/")
	tcpipHttpNetMpfsImg2SourceFile.setType("SOURCE")
	tcpipHttpNetMpfsImg2SourceFile.setMarkup(True)
	tcpipHttpNetMpfsImg2SourceFile.setDependencies(tcpipHttpNetGenSourceFile, ["TCPIP_HTTP_NET_CUSTOM_TEMPLATE_SL"])	
	# endif
	
	
# make Http Net Server option visible
def tcpipHttpNetSrvVisible(tcpipDependentSymbol, tcpipIPSymbol):	
	tcpipHttp = Database.getSymbolValue("tcpipHttp","TCPIP_STACK_USE_HTTP_SERVER")
	tcpipTcp = Database.getSymbolValue("tcpipTcp","TCPIP_USE_TCP")

	if(tcpipTcp):
		tcpipDependentSymbol.setVisible(True)		
		if(tcpipHttp):			
			tcpipDependentSymbol.setReadOnly(True)
		else:			
			tcpipDependentSymbol.setReadOnly(False)
	else:
		tcpipDependentSymbol.setVisible(False)		

# make Http Net Module file upload name option visible
def tcpipHttpNetFileUploadVisible(tcpipDependentSymbol, tcpipIPSymbol):	
	tcpipHttpNet = Database.getSymbolValue("tcpipHttpNet","TCPIP_STACK_USE_HTTP_NET_SERVER")
	tcpipHttpNetFileUpload = Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_FILE_UPLOAD_ENABLE")

	if(tcpipHttpNet and tcpipHttpNetFileUpload):
		tcpipDependentSymbol.setVisible(True)
	else:
		tcpipDependentSymbol.setVisible(False)

# make Http Net Server Config Flag Secure options visible
def tcpipHttpNetSrvConfigFlagSecureOnEnable(tcpipDependentSymbol, tcpipIPSymbol):	
	tcpipHttpNet = Database.getSymbolValue("tcpipHttpNet","TCPIP_STACK_USE_HTTP_NET_SERVER")
	tcpipHttpNetSrvConfigFlagSecureOff = Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_OFF")
	tcpipHttpNetSrvConfigFlagSecureDefault = Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_DEFAULT")

	if(tcpipHttpNet):		
		if(tcpipHttpNetSrvConfigFlagSecureOff or tcpipHttpNetSrvConfigFlagSecureDefault):
			tcpipDependentSymbol.setVisible(False)
			print("SecureOn disable")
		else:
			tcpipDependentSymbol.setVisible(True)
			print("SecureOn enable")
	else:
		tcpipDependentSymbol.setVisible(False)	
		print("SecureOn disable")	
	

def tcpipHttpNetSrvConfigFlagSecureOffEnable(tcpipDependentSymbol, tcpipIPSymbol):	
	tcpipHttpNet = Database.getSymbolValue("tcpipHttpNet","TCPIP_STACK_USE_HTTP_NET_SERVER")
	tcpipHttpNetSrvConfigFlagSecureOn = Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_ON")
	tcpipHttpNetSrvConfigFlagSecureDefault = Database.getSymbolValue("v","TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_DEFAULT")
	
	if(tcpipHttpNet):		
		if(tcpipHttpNetSrvConfigFlagSecureOn or tcpipHttpNetSrvConfigFlagSecureDefault):
			tcpipDependentSymbol.setVisible(False)
			print("SecureOff disable")
		else:
			tcpipDependentSymbol.setVisible(True)
			print("SecureOff enable")
	else:
		tcpipDependentSymbol.setVisible(False)	
		print("SecureOff disable")	
	

def tcpipHttpNetSrvConfigFlagSecureDefaultEnable(tcpipDependentSymbol, tcpipIPSymbol):	
	tcpipHttpNet = Database.getSymbolValue("tcpipHttpNet","TCPIP_STACK_USE_HTTP_NET_SERVER")
	tcpipHttpNetSrvConfigFlagSecureOn = Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_ON")
	tcpipHttpNetSrvConfigFlagSecureOff = Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_OFF")
	
	if(tcpipHttpNet):		
		if(tcpipHttpNetSrvConfigFlagSecureOn or tcpipHttpNetSrvConfigFlagSecureOff):
			tcpipDependentSymbol.setVisible(False)
			print("Securedefault disable")
		else:
			tcpipDependentSymbol.setVisible(True)
			print("Securedefault enable")
	else:
		tcpipDependentSymbol.setVisible(False)	
		print("Securedefault disable")

# make Http Net Module Dynamic Variable options visible
def tcpipHttpNetDynVarVisible(tcpipDependentSymbol, tcpipIPSymbol):	
	tcpipHttpNet = Database.getSymbolValue("tcpipHttpNet","TCPIP_STACK_USE_HTTP_NET_SERVER")
	tcpipHttpNetDynVarProcess = Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_DYNVAR_PROCESS")

	if(tcpipHttpNet and tcpipHttpNetDynVarProcess):
		tcpipDependentSymbol.setVisible(True)
	else:
		tcpipDependentSymbol.setVisible(False)	

# make Http Net Module SSI Attribute options visible
def tcpipHttpNetSsiAttrVisible(tcpipDependentSymbol, tcpipIPSymbol):	
	tcpipHttpNet = Database.getSymbolValue("tcpipHttpNet","TCPIP_STACK_USE_HTTP_NET_SERVER")
	tcpipHttpNetSsiProcess= Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_SSI_PROCESS")

	if(tcpipHttpNet and tcpipHttpNetSsiProcess):
		tcpipDependentSymbol.setVisible(True)
	else:
		tcpipDependentSymbol.setVisible(False)	

		
def tcpipHttpNetMenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("TFTPC Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("TFTPC Menu Invisible.")
		symbol.setVisible(False)
		
def tcpipHttpNetGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])