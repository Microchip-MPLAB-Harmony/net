"""
Copyright (C) 2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

def instantiateComponent(tcpipHttpSrvV2Component):
    import re
    import os
    import sys
    configName = Variables.get("__CONFIGURATION_NAME")
    # Enable HTTP Server
    tcpipHttpEnableV2 = tcpipHttpSrvV2Component.createBooleanSymbol("TCPIP_STACK_USE_HTTP_SERVER_V2", None)
    tcpipHttpEnableV2.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpEnableV2.setLabel("HTTP Server V2")
    tcpipHttpEnableV2.setVisible(False)
    tcpipHttpEnableV2.setDescription("Enable HTTP Server V2")
    tcpipHttpEnableV2.setDefaultValue(True)

    # HTTP Socket TX Buffer Size
    tcpipHttpSrvSktTxBuffsize = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_SKT_TX_BUFF_SIZE", None)
    tcpipHttpSrvSktTxBuffsize.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvSktTxBuffsize.setLabel("HTTP Socket TX Buffer Size")
    tcpipHttpSrvSktTxBuffsize.setVisible(True)
    tcpipHttpSrvSktTxBuffsize.setDescription("HTTP Socket TX Buffer Size")
    tcpipHttpSrvSktTxBuffsize.setDefaultValue(1024)

    # HTTP Socket RX Buffer Size
    tcpipHttpSrvSktRxBuffsize = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_SKT_RX_BUFF_SIZE", None)
    tcpipHttpSrvSktRxBuffsize.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvSktRxBuffsize.setLabel("HTTP Socket RX Buffer Size")
    tcpipHttpSrvSktRxBuffsize.setVisible(True)
    tcpipHttpSrvSktRxBuffsize.setDescription("HTTP Socket RX Buffer Size")
    tcpipHttpSrvSktRxBuffsize.setDefaultValue(1024)

    # Maximum Number of Simultaneous Connections
    tcpipHttpSrvConnMaxNum = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_MAX_CONNECTIONS", None)
    tcpipHttpSrvConnMaxNum.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvConnMaxNum.setLabel("Maximum Number of Simultaneous Connections")
    tcpipHttpSrvConnMaxNum.setVisible(True)
    tcpipHttpSrvConnMaxNum.setDescription("Maximum Number of Simultaneous Connections")
    tcpipHttpSrvConnMaxNum.setDefaultValue(4)
    
    # HTTP Listening Port
    tcpipHttpSrvListenPort = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_LISTEN_PORT", None)
    tcpipHttpSrvListenPort.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvListenPort.setLabel("HTTP Listening Port")
    tcpipHttpSrvListenPort.setVisible(True)
    tcpipHttpSrvListenPort.setDescription("HTTP Listening Port")
    tcpipHttpSrvListenPort.setDefaultValue(80)

    # Default Web pages directory
    tcpipHttpSrvWebDir = tcpipHttpSrvV2Component.createStringSymbol("TCPIP_HTTP_WEB_DIR", None)
    tcpipHttpSrvWebDir.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvWebDir.setLabel("Web Pages Directory")
    tcpipHttpSrvWebDir.setVisible(True)
    tcpipHttpSrvWebDir.setDescription("Web Pages Directory")
    tcpipHttpSrvWebDir.setDefaultValue("/mnt/mchpSite1/")
    
    # Enable HTTP Console Commands
    tcpipHttpSrvCmdEnable = tcpipHttpSrvV2Component.createBooleanSymbol("TCPIP_HTTP_CONSOLE_CMD", None) 
    tcpipHttpSrvCmdEnable.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvCmdEnable.setLabel("Enable Console Commands")
    tcpipHttpSrvCmdEnable.setVisible(True)
    tcpipHttpSrvCmdEnable.setDescription("Enable HTTP Server Console Commands")
    tcpipHttpSrvCmdEnable.setDefaultValue(False)
        
    # Include HTTP Custom Template
    tcpipHttpSrvCustTemplate = tcpipHttpSrvV2Component.createBooleanSymbol("TCPIP_HTTP_V2_CUSTOM_TEMPLATE", None)
    tcpipHttpSrvCustTemplate.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvCustTemplate.setLabel("Include HTTP Custom Template")
    tcpipHttpSrvCustTemplate.setVisible(True)
    tcpipHttpSrvCustTemplate.setDescription("Include HTTP Custom Template")
    tcpipHttpSrvCustTemplate.setDefaultValue(True)


    # Message to provide the source web page path which will be used for the webpage_v2.py
    tcpipHttpV2WebPageDirPath = tcpipHttpSrvV2Component.createStringSymbol("TCPIP_HTTP_WEBPAGE_DIRECTORY_PATH", tcpipHttpSrvCustTemplate)
    tcpipHttpV2WebPageDirPath.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpV2WebPageDirPath.setLabel("Web pages source directory path")
    tcpipHttpV2WebPageDirPath.setVisible(True)
    tcpipHttpV2WebPageDirPath.setDescription("Configure Webpage directory path")
    tcpipHttpV2WebPageDirPath.setDefaultValue(Module.getPath() + "web_pages")
    tcpipHttpV2WebPageDirPath.setDependencies(tcpipHttpSrvWebServerPathVisible, ["TCPIP_STACK_USE_HTTP_SERVER_V2" , "TCPIP_HTTP_V2_CUSTOM_TEMPLATE"])

    tcpipHttpSrvDestWebPageDirPath = tcpipHttpSrvV2Component.createKeyValueSetSymbol("TCPIP_HTTP_DEST_WEBPAGE_DIRECTORY_PATH",tcpipHttpSrvCustTemplate)
    tcpipHttpSrvDestWebPageDirPath.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvDestWebPageDirPath.setLabel("Web pages destination directory path")
    tcpipHttpSrvDestWebPageDirPath.setVisible(True)
    # tcpipHttpSrvDestWebPageDirPath.addKey("DESTINATION PATH", "0", Module.getPath() + "apps"+os.path.sep+"<project>"+os.path.sep+"firmware"+os.path.sep+"src"+os.path.sep+"web_pages")
    tcpipHttpSrvDestWebPageDirPath.addKey("DESTINATION PATH", "0", "<project_path>"+os.path.sep+"firmware"+os.path.sep+"src"+os.path.sep+"web_pages")
    tcpipHttpSrvDestWebPageDirPath.setDisplayMode("Description")
    tcpipHttpSrvDestWebPageDirPath.setOutputMode("Key")
    tcpipHttpSrvDestWebPageDirPath.setDefaultValue(0)
    tcpipHttpSrvDestWebPageDirPath.setReadOnly(True)
    tcpipHttpSrvDestWebPageDirPath.setDependencies(tcpipHttpSrvWebServerPathVisible, ["TCPIP_STACK_USE_HTTP_SERVER_V2" , "TCPIP_HTTP_V2_CUSTOM_TEMPLATE"])

    # Message that Number of webpage_v2 files accepted
    tcpipHttpSrvWebPageFileCount = tcpipHttpSrvV2Component.createCommentSymbol("TCPIP_WEBPAGE_FILES_COUNT_COMMENT", tcpipHttpSrvCustTemplate)
    tcpipHttpSrvWebPageFileCount.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvWebPageFileCount.setVisible(True)
    tcpipHttpSrvWebPageFileCount.setLabel("*** Maximum 100 web page files currently supported ***")
    tcpipHttpSrvWebPageFileCount.setDependencies(tcpipHttpSrvWebServerPathVisible, ["TCPIP_STACK_USE_HTTP_SERVER_V2" , "TCPIP_HTTP_V2_CUSTOM_TEMPLATE"])

    # Advanced Settings
    tcpipHttpSrvAdvSettings = tcpipHttpSrvV2Component.createMenuSymbol("TCPIP_HTTP_ADV_SETTING", None)
    tcpipHttpSrvAdvSettings.setLabel("Advanced Settings")
    tcpipHttpSrvAdvSettings.setDescription("Advanced Settings")
    tcpipHttpSrvAdvSettings.setVisible(True)

    # HTTP Task Rate - ms
    tcpipHttpSrvTskRate = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_TASK_RATE", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvTskRate.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvTskRate.setLabel("HTTP Task Rate (in msec)")
    tcpipHttpSrvTskRate.setVisible(True)
    tcpipHttpSrvTskRate.setDescription("HTTP Task Rate - ms")
    tcpipHttpSrvTskRate.setDefaultValue(33)
    
    # Maximum Data Length (bytes) for Reading Cookie and GET/POST Arguments
    tcpipHttpSrvDataLenMax = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_MAX_DATA_LEN", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvDataLenMax.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvDataLenMax.setLabel("Maximum Data Length (bytes) for Reading Cookie and GET/POST Arguments")
    tcpipHttpSrvDataLenMax.setVisible(True)
    tcpipHttpSrvDataLenMax.setDescription("Maximum Data Length (bytes) for Reading Cookie and GET/POST Arguments")
    tcpipHttpSrvDataLenMax.setDefaultValue(100)

    # Size of the Buffer Used for Sending Response Messages to the Client
    tcpipHttpSrvRespBuffSize = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_RESPONSE_BUFFER_SIZE", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvRespBuffSize.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvRespBuffSize.setLabel("Buffer Size for Sending Response Messages to the Client")
    tcpipHttpSrvRespBuffSize.setVisible(True)
    tcpipHttpSrvRespBuffSize.setDescription("Size of the Buffer Used for Sending Response Messages to the Client")
    tcpipHttpSrvRespBuffSize.setDefaultValue(300)

    # Size of the Buffer Used for Sending Cookies to the Client
    tcpipHttpSrvCookieBuffSize = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_COOKIE_BUFFER_SIZE", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvCookieBuffSize.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvCookieBuffSize.setLabel("Buffer Size for Sending Cookies to the Client")
    tcpipHttpSrvCookieBuffSize.setVisible(True)
    tcpipHttpSrvCookieBuffSize.setDescription("Size of the Buffer Used for Sending Cookies to the Client")
    tcpipHttpSrvCookieBuffSize.setDefaultValue(200)

    # Size of the Peek Buffer for Performing Searches
    tcpipHttpSrvFindPeekBuffSize = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_FIND_PEEK_BUFF_SIZE", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvFindPeekBuffSize.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvFindPeekBuffSize.setLabel("Peek Buffer Size for Performing Searches")
    tcpipHttpSrvFindPeekBuffSize.setVisible(True)
    tcpipHttpSrvFindPeekBuffSize.setDescription("Size of the Peek Buffer for Performing Searches")
    tcpipHttpSrvFindPeekBuffSize.setDefaultValue(512)

    # Size of the Buffer for Processing HTML, Dynamic Variable and Binary Files
    tcpipHttpSrvFileProcessBuffSize = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_FILE_PROCESS_BUFFER_SIZE", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvFileProcessBuffSize.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvFileProcessBuffSize.setLabel("File Processing Buffer Size")
    tcpipHttpSrvFileProcessBuffSize.setVisible(True)
    tcpipHttpSrvFileProcessBuffSize.setDescription("Size of the Buffer for Processing HTML, Dynamic Variable and Binary Files")
    tcpipHttpSrvFileProcessBuffSize.setDefaultValue(512)

    # Number of File Buffers to be Created
    tcpipHttpSrvFileProcessBuffNum = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_FILE_PROCESS_BUFFERS_NUMBER", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvFileProcessBuffNum.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvFileProcessBuffNum.setLabel("Number of File Buffers to be Created")
    tcpipHttpSrvFileProcessBuffNum.setVisible(True)
    tcpipHttpSrvFileProcessBuffNum.setDescription("Number of File Buffers to be Created")
    tcpipHttpSrvFileProcessBuffNum.setDefaultValue(4)

    # Retry Limit for Allocating a File Buffer from the Pool
    tcpipHttpSrvFileProcessBuffRetry = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_FILE_PROCESS_BUFFER_RETRIES", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvFileProcessBuffRetry.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvFileProcessBuffRetry.setLabel("Retry Limit for Allocating a File Buffer from the Pool")
    tcpipHttpSrvFileProcessBuffRetry.setVisible(True)
    tcpipHttpSrvFileProcessBuffRetry.setDescription("Retry Limit for Allocating a File Buffer from the Pool")
    tcpipHttpSrvFileProcessBuffRetry.setDefaultValue(10)

    # Retry Limit for Allocating a File Buffer from the Pool
    tcpipHttpSrvChunksNum = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_CHUNKS_NUMBER", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvChunksNum.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvChunksNum.setLabel("Number of Chunks to be Created")
    tcpipHttpSrvChunksNum.setVisible(True)
    tcpipHttpSrvChunksNum.setDescription("Number of Chunks to be Created")
    tcpipHttpSrvChunksNum.setDefaultValue(10)

    # Retry Limit for Allocating a Chunk from the Pool
    tcpipHttpSrvChunkRetry = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_CHUNK_RETRIES", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvChunkRetry.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvChunkRetry.setLabel("Retry Limit for Allocating a Chunk from the Pool")
    tcpipHttpSrvChunkRetry.setVisible(True)
    tcpipHttpSrvChunkRetry.setDescription("Retry Limit for Allocating a Chunk from the Pool")
    tcpipHttpSrvChunkRetry.setDefaultValue(10)

    # The Maximum Depth of Recursive Calls for Serving a Web Page
    tcpipHttpSrvRecursiveLvl = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_MAX_RECURSE_LEVEL", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvRecursiveLvl.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvRecursiveLvl.setLabel("Maximum Depth of Recursive Calls for Serving a Web Page")
    tcpipHttpSrvRecursiveLvl.setVisible(True)
    tcpipHttpSrvRecursiveLvl.setDescription("The Maximum Depth of Recursive Calls for Serving a Web Page")
    tcpipHttpSrvRecursiveLvl.setDefaultValue(3)
    
    # Maximum Header Length
    tcpipHttpSrvHdrLenMax = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_MAX_HEADER_LEN", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvHdrLenMax.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvHdrLenMax.setLabel("Maximum Header Length")
    tcpipHttpSrvHdrLenMax.setVisible(True)
    tcpipHttpSrvHdrLenMax.setDescription("Maximum Header Length")
    tcpipHttpSrvHdrLenMax.setDefaultValue(15)

    # Maximum Lifetime of Static Responses in Seconds
    tcpipHttpSrvCacheLen = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_CACHE_LEN", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvCacheLen.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvCacheLen.setLabel("Maximum Lifetime of Static Responses (in sec)")
    tcpipHttpSrvCacheLen.setVisible(True)
    tcpipHttpSrvCacheLen.setDescription("Maximum Lifetime of Static Responses in Seconds")
    tcpipHttpSrvCacheLen.setDefaultValue(600)

    # Socket Disconnect Time-out
    tcpipHttpSrvTimeout = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_TIMEOUT", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvTimeout.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvTimeout.setLabel("Socket Disconnect Timeout (in sec)")
    tcpipHttpSrvTimeout.setVisible(True)
    tcpipHttpSrvTimeout.setDescription("Socket Disconnect Time-out")
    tcpipHttpSrvTimeout.setDefaultValue(45)


    # Default HTTP File
    tcpipHttpSrvDefaultFile = tcpipHttpSrvV2Component.createStringSymbol("TCPIP_HTTP_DEFAULT_FILE", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvDefaultFile.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvDefaultFile.setLabel("Default HTTP File")
    tcpipHttpSrvDefaultFile.setVisible(True)
    tcpipHttpSrvDefaultFile.setDescription("Default HTTP File")
    tcpipHttpSrvDefaultFile.setDefaultValue("index.htm")

    # Maximum Size of a HTTP File Name
    tcpipHttpSrvFilenameLenMax = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_FILENAME_MAX_LEN", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvFilenameLenMax.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvFilenameLenMax.setLabel("Maximum Size of a HTTP File Name")
    tcpipHttpSrvFilenameLenMax.setVisible(True)
    tcpipHttpSrvFilenameLenMax.setDescription("Maximum Size of a HTTP File Name")
    tcpipHttpSrvFilenameLenMax.setDefaultValue(25)


    # Enable MPFS Update via HTTP 
    tcpipHttpSrvFileUpload = tcpipHttpSrvV2Component.createBooleanSymbol("TCPIP_HTTP_FILE_UPLOAD_ENABLE", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvFileUpload.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvFileUpload.setLabel("Enable MPFS Update via HTTP")
    tcpipHttpSrvFileUpload.setVisible(True)
    tcpipHttpSrvFileUpload.setDescription("Enable MPFS Update via HTTP")
    tcpipHttpSrvFileUpload.setDefaultValue(False)

    # MPFS Upload Page Name
    tcpipHttpSrvFileUploadName = tcpipHttpSrvV2Component.createStringSymbol("TCPIP_HTTP_FILE_UPLOAD_NAME", tcpipHttpSrvFileUpload)
    tcpipHttpSrvFileUploadName.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvFileUploadName.setLabel("MPFS Upload Page Name")
    tcpipHttpSrvFileUploadName.setVisible(False)
    tcpipHttpSrvFileUploadName.setDescription("MPFS Upload Page Name")
    tcpipHttpSrvFileUploadName.setDefaultValue("mpfsupload")
    tcpipHttpSrvFileUploadName.setDependencies(tcpipHttpSrvFileUploadVisible, ["TCPIP_HTTP_FILE_UPLOAD_ENABLE"])
    
    # MPFS NVM Mount Path
    tcpipHttpSrvMpfsNvmMountPath = tcpipHttpSrvV2Component.createStringSymbol("TCPIP_HTTP_MPFS_NVM_PATH", tcpipHttpSrvFileUpload)
    tcpipHttpSrvMpfsNvmMountPath.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvMpfsNvmMountPath.setLabel("MPFS NVM Mount Path")
    tcpipHttpSrvMpfsNvmMountPath.setVisible(False)
    tcpipHttpSrvMpfsNvmMountPath.setDescription("MPFS NVM Mount Path")
    tcpipHttpSrvMpfsNvmMountPath.setDefaultValue("/mnt/mchpSite1")
    tcpipHttpSrvMpfsNvmMountPath.setDependencies(tcpipHttpSrvFileUploadVisible, ["TCPIP_HTTP_FILE_UPLOAD_ENABLE"])
    
    # MPFS NVM Disk Path
    tcpipHttpSrvMpfsNvmDiskPath = tcpipHttpSrvV2Component.createStringSymbol("TCPIP_HTTP_MPFS_NVM_VOL", tcpipHttpSrvFileUpload)
    tcpipHttpSrvMpfsNvmDiskPath.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvMpfsNvmDiskPath.setLabel("MPFS NVM Disk Path")
    tcpipHttpSrvMpfsNvmDiskPath.setVisible(False)
    tcpipHttpSrvMpfsNvmDiskPath.setDescription("MPFS NVM Disk Path")
    tcpipHttpSrvMpfsNvmDiskPath.setDefaultValue("/dev/nvma1")    
    tcpipHttpSrvMpfsNvmDiskPath.setDependencies(tcpipHttpSrvFileUploadVisible, ["TCPIP_HTTP_FILE_UPLOAD_ENABLE"])   
    
    # MPFS NVM Disk Number
    tcpipHttpSrvMpfsNvmDiskNum= tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_MPFS_NVM_NUM", tcpipHttpSrvFileUpload)
    tcpipHttpSrvMpfsNvmDiskNum.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvMpfsNvmDiskNum.setLabel("MPFS NVM Disk Number")
    tcpipHttpSrvMpfsNvmDiskNum.setVisible(False)
    tcpipHttpSrvMpfsNvmDiskNum.setDescription("MPFS NVM Disk Number")
    tcpipHttpSrvMpfsNvmDiskNum.setDefaultValue(0)    
    tcpipHttpSrvMpfsNvmDiskNum.setDependencies(tcpipHttpSrvFileUploadVisible, ["TCPIP_HTTP_FILE_UPLOAD_ENABLE"])   
    
    # Enable POST Support
    tcpipHttpSrvPostSupport = tcpipHttpSrvV2Component.createBooleanSymbol("TCPIP_HTTP_USE_POST", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvPostSupport.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvPostSupport.setLabel("Enable POST Support")
    tcpipHttpSrvPostSupport.setVisible(True)
    tcpipHttpSrvPostSupport.setDescription("Enable POST Support")
    tcpipHttpSrvPostSupport.setDefaultValue(True)

    # Enable Cookie Support
    tcpipHttpSrvCookieSupport = tcpipHttpSrvV2Component.createBooleanSymbol("TCPIP_HTTP_USE_COOKIES", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvCookieSupport.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvCookieSupport.setLabel("Enable Cookie Support")
    tcpipHttpSrvCookieSupport.setVisible(True)
    tcpipHttpSrvCookieSupport.setDescription("Enable Cookie Support")
    tcpipHttpSrvCookieSupport.setDefaultValue(True)

    # Use Base 64 Decode
    tcpipHttpSrvBase64Decode = tcpipHttpSrvV2Component.createBooleanSymbol("TCPIP_HTTP_USE_BASE64_DECODE", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvBase64Decode.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvBase64Decode.setLabel("Use Base 64 Decode")
    tcpipHttpSrvBase64Decode.setVisible(True)
    tcpipHttpSrvBase64Decode.setDescription("Use Base 64 Decode")
    tcpipHttpSrvBase64Decode.setDefaultValue(False) 


    # Enable Basic Authentication Support
    tcpipHttpSrvAuth = tcpipHttpSrvV2Component.createBooleanSymbol("TCPIP_HTTP_USE_AUTHENTICATION", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvAuth.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvAuth.setLabel("Enable Basic Authentication Support")
    tcpipHttpSrvAuth.setVisible(True)
    tcpipHttpSrvAuth.setDescription("Enable Basic Authentication Support")
    tcpipHttpSrvAuth.setDefaultValue(True)
    tcpipHttpSrvBase64Decode.setDependencies(tcpipHttpSrvBase64DecodeOpt, ["TCPIP_HTTP_USE_AUTHENTICATION"])


    # HTTP Configuration Flags Settings
    tcpipHttpSrvConfigFlag = tcpipHttpSrvV2Component.createMenuSymbol(None, tcpipHttpSrvAdvSettings)
    tcpipHttpSrvConfigFlag.setLabel("HTTP Configuration Flags")
    tcpipHttpSrvConfigFlag.setVisible(True)
    tcpipHttpSrvConfigFlag.setDescription("HTTP Configuration Flags Settings")

    # Use non-persistent connections
    tcpipHttpSrvConfigFlagNonpersistent = tcpipHttpSrvV2Component.createBooleanSymbol("TCPIP_HTTP_CONFIG_FLAG_NON_PERSISTENT", tcpipHttpSrvConfigFlag)
    tcpipHttpSrvConfigFlagNonpersistent.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvConfigFlagNonpersistent.setLabel("Use non-persistent connections")
    tcpipHttpSrvConfigFlagNonpersistent.setVisible(True)
    tcpipHttpSrvConfigFlagNonpersistent.setDescription("Use non-persistent connections")
    tcpipHttpSrvConfigFlagNonpersistent.setDefaultValue(False)

    # HTTP sockets created with NO-DELAY option
    tcpipHttpSrvConfigFlagNoDly = tcpipHttpSrvV2Component.createBooleanSymbol("TCPIP_HTTP_CONFIG_FLAG_NO_DELAY", tcpipHttpSrvConfigFlag)
    tcpipHttpSrvConfigFlagNoDly.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvConfigFlagNoDly.setLabel("HTTP sockets created with NO-DELAY option")
    tcpipHttpSrvConfigFlagNoDly.setVisible(True)
    tcpipHttpSrvConfigFlagNoDly.setDescription("HTTP sockets created with NO-DELAY option")
    tcpipHttpSrvConfigFlagNoDly.setDefaultValue(False)

    # All HTTP connections have to be secure
    tcpipHttpSrvConfigFlagSecureOn = tcpipHttpSrvV2Component.createBooleanSymbol("TCPIP_HTTP_CONFIG_FLAG_SECURE_ON", tcpipHttpSrvConfigFlag)
    tcpipHttpSrvConfigFlagSecureOn.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvConfigFlagSecureOn.setLabel("All HTTP connections have to be secure")
    tcpipHttpSrvConfigFlagSecureOn.setVisible(False)
    tcpipHttpSrvConfigFlagSecureOn.setDescription("All HTTP connections have to be secure")
    tcpipHttpSrvConfigFlagSecureOn.setDefaultValue(False)

    # All HTTP connections have to be non-secure
    tcpipHttpSrvConfigFlagSecureOff = tcpipHttpSrvV2Component.createBooleanSymbol("TCPIP_HTTP_CONFIG_FLAG_SECURE_OFF", tcpipHttpSrvConfigFlag)
    tcpipHttpSrvConfigFlagSecureOff.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvConfigFlagSecureOff.setLabel("All HTTP connections have to be non-secure")
    tcpipHttpSrvConfigFlagSecureOff.setVisible(False)
    tcpipHttpSrvConfigFlagSecureOff.setDescription("All HTTP connections have to be non-secure")
    tcpipHttpSrvConfigFlagSecureOff.setDefaultValue(False)

    # HTTP security is based on the port numbers
    tcpipHttpSrvConfigFlagSecureDefault = tcpipHttpSrvV2Component.createBooleanSymbol("TCPIP_HTTP_CONFIG_FLAG_SECURE_DEFAULT", tcpipHttpSrvConfigFlag)
    tcpipHttpSrvConfigFlagSecureDefault.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvConfigFlagSecureDefault.setLabel("HTTP security is based on the port numbers")
    tcpipHttpSrvConfigFlagSecureDefault.setVisible(True)
    tcpipHttpSrvConfigFlagSecureDefault.setDescription("HTTP security is based on the port numbers")
    tcpipHttpSrvConfigFlagSecureDefault.setDefaultValue(True)

    tcpipHttpSrvConfigFlagSecureDefault.setDependencies(tcpipHttpV2ConfigFlagSecureDefaultEnable, ["TCPIP_HTTP_CONFIG_FLAG_SECURE_OFF", "TCPIP_HTTP_CONFIG_FLAG_SECURE_ON"])
    tcpipHttpSrvConfigFlagSecureOn.setDependencies(tcpipHttpSrvV2ConfigFlagSecureOnEnable, ["TCPIP_HTTP_CONFIG_FLAG_SECURE_OFF", "TCPIP_HTTP_CONFIG_FLAG_SECURE_DEFAULT"])
    tcpipHttpSrvConfigFlagSecureOff.setDependencies(tcpipHttpSrvV2ConfigFlagSecureOffEnable, ["TCPIP_HTTP_CONFIG_FLAG_SECURE_ON", "TCPIP_HTTP_CONFIG_FLAG_SECURE_DEFAULT"])

    # Enable the Processing of Dynamic Variables
    tcpipHttpSrvDynVarProc = tcpipHttpSrvV2Component.createBooleanSymbol("TCPIP_HTTP_DYNVAR_PROCESS", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvDynVarProc.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvDynVarProc.setLabel("Enable the Processing of Dynamic Variables")
    tcpipHttpSrvDynVarProc.setVisible(True)
    tcpipHttpSrvDynVarProc.setDescription("Enable the Processing of Dynamic Variables")
    tcpipHttpSrvDynVarProc.setDefaultValue(True)

    # Number of the Descriptors for Dynamic Variables Processing
    tcpipHttpSrvDynVarDescNum = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_DYNVAR_DESCRIPTORS_NUMBER", tcpipHttpSrvDynVarProc)
    tcpipHttpSrvDynVarDescNum.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvDynVarDescNum.setLabel("Number of the Descriptors for Dynamic Variables Processing")
    tcpipHttpSrvDynVarDescNum.setVisible(True)
    tcpipHttpSrvDynVarDescNum.setDescription("Number of the Descriptors for Dynamic Variables Processing")
    tcpipHttpSrvDynVarDescNum.setDefaultValue(10)
    tcpipHttpSrvDynVarDescNum.setDependencies(tcpipHttpSrvDynVarVisible, ["TCPIP_STACK_USE_HTTP_SERVER_V2" , "TCPIP_HTTP_DYNVAR_PROCESS"])

    # Maximum Size for a Complete Dynamic Variable: Name + Args
    tcpipHttpSrvDynVarLenMax = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_DYNVAR_MAX_LEN", tcpipHttpSrvDynVarProc)
    tcpipHttpSrvDynVarLenMax.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvDynVarLenMax.setLabel("Maximum Size for a Complete Dynamic Variable: Name + Args")
    tcpipHttpSrvDynVarLenMax.setVisible(True)
    tcpipHttpSrvDynVarLenMax.setDescription("Maximum Size for a Complete Dynamic Variable: Name + Args")
    tcpipHttpSrvDynVarLenMax.setDefaultValue(50)
    tcpipHttpSrvDynVarLenMax.setDependencies(tcpipHttpSrvDynVarVisible, ["TCPIP_STACK_USE_HTTP_SERVER_V2" , "TCPIP_HTTP_DYNVAR_PROCESS"])

    # Maximum Number of Arguments for a Dynamic Variable
    tcpipHttpSrvDynVarArgNumMax = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_DYNVAR_ARG_MAX_NUMBER", tcpipHttpSrvDynVarProc)
    tcpipHttpSrvDynVarArgNumMax.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvDynVarArgNumMax.setLabel("Maximum Number of Arguments for a Dynamic Variable")
    tcpipHttpSrvDynVarArgNumMax.setVisible(True)
    tcpipHttpSrvDynVarArgNumMax.setDescription("Maximum Number of Arguments for a Dynamic Variable")
    tcpipHttpSrvDynVarArgNumMax.setDefaultValue(4)
    tcpipHttpSrvDynVarArgNumMax.setDependencies(tcpipHttpSrvDynVarVisible, ["TCPIP_STACK_USE_HTTP_SERVER_V2" , "TCPIP_HTTP_DYNVAR_PROCESS"])

    # Retry Limit for a Dynamic Variable Processing
    tcpipHttpSrvDynVarProcRetry = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_DYNVAR_PROCESS_RETRIES", tcpipHttpSrvDynVarProc)
    tcpipHttpSrvDynVarProcRetry.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvDynVarProcRetry.setLabel("Retry Limit for a Dynamic Variable Processing")
    tcpipHttpSrvDynVarProcRetry.setVisible(True)
    tcpipHttpSrvDynVarProcRetry.setDescription("Retry Limit for a Dynamic Variable Processing")
    tcpipHttpSrvDynVarProcRetry.setDefaultValue(10)
    tcpipHttpSrvDynVarProcRetry.setDependencies(tcpipHttpSrvDynVarVisible, ["TCPIP_STACK_USE_HTTP_SERVER_V2" , "TCPIP_HTTP_DYNVAR_PROCESS"])

    # Enable the Processing of SSI Commands
    tcpipHttpSrvSsiProc = tcpipHttpSrvV2Component.createBooleanSymbol("TCPIP_HTTP_SSI_PROCESS", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvSsiProc.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvSsiProc.setLabel("Enable the Processing of SSI Commands")
    tcpipHttpSrvSsiProc.setVisible(True)
    tcpipHttpSrvSsiProc.setDescription("Enable the Processing of SSI Commands")
    tcpipHttpSrvSsiProc.setDefaultValue(True)

    # Maximum Number of Attributes for a SSI Command
    tcpipHttpSrvSsiAttrNumMax = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_SSI_ATTRIBUTES_MAX_NUMBER", tcpipHttpSrvSsiProc)
    tcpipHttpSrvSsiAttrNumMax.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvSsiAttrNumMax.setLabel("Maximum Number of Attributes for a SSI Command")
    tcpipHttpSrvSsiAttrNumMax.setVisible(True)
    tcpipHttpSrvSsiAttrNumMax.setDescription("Maximum Number of Attributes for a SSI Command")
    tcpipHttpSrvSsiAttrNumMax.setDefaultValue(4)
    tcpipHttpSrvSsiAttrNumMax.setDependencies(tcpipHttpSrvSsiAttrVisible, ["TCPIP_STACK_USE_HTTP_SERVER_V2" , "TCPIP_HTTP_SSI_PROCESS"])

    # Number of Static Attributes Associated to a SSI Command
    tcpipHttpSrvSsiStaticAttrNum = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_SSI_STATIC_ATTTRIB_NUMBER", tcpipHttpSrvSsiProc)
    tcpipHttpSrvSsiStaticAttrNum.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvSsiStaticAttrNum.setLabel("Number of Static Attributes Associated to a SSI Command")
    tcpipHttpSrvSsiStaticAttrNum.setVisible(False)
    tcpipHttpSrvSsiStaticAttrNum.setDescription("Number of Static Attributes Associated to a SSI Command")
    tcpipHttpSrvSsiStaticAttrNum.setDefaultValue(2)
    tcpipHttpSrvSsiStaticAttrNum.setDependencies(tcpipHttpSrvSsiAttrVisible, ["TCPIP_STACK_USE_HTTP_SERVER_V2" , "TCPIP_HTTP_SSI_PROCESS"])

    # Maximum Size for a SSI Command Line: Command + Attribute/Value Pairs
    tcpipHttpSrvSsiCmdLenMax = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_SSI_CMD_MAX_LEN", tcpipHttpSrvSsiProc)
    tcpipHttpSrvSsiCmdLenMax.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvSsiCmdLenMax.setLabel("Maximum Size for a SSI Command Line: Command + Attribute/Value Pairs")
    tcpipHttpSrvSsiCmdLenMax.setVisible(True)
    tcpipHttpSrvSsiCmdLenMax.setDescription("Maximum Size for a SSI Command Line: Command + Attribute/Value Pairs")
    tcpipHttpSrvSsiCmdLenMax.setDefaultValue(100)
    tcpipHttpSrvSsiCmdLenMax.setDependencies(tcpipHttpSrvSsiAttrVisible, ["TCPIP_STACK_USE_HTTP_SERVER_V2" , "TCPIP_HTTP_SSI_PROCESS"])

    # Maximum Number of SSI Variables that Can Be Created at Run Time
    tcpipHttpSrvSsiVarNum = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_SSI_VARIABLES_NUMBER", tcpipHttpSrvSsiProc)
    tcpipHttpSrvSsiVarNum.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvSsiVarNum.setLabel("Maximum Number of SSI Variables that Can Be Created at Runtime")
    tcpipHttpSrvSsiVarNum.setVisible(True)
    tcpipHttpSrvSsiVarNum.setDescription("Maximum Number of SSI Variables that Can Be Created at Run Time")
    tcpipHttpSrvSsiVarNum.setDefaultValue(13)
    tcpipHttpSrvSsiVarNum.setDependencies(tcpipHttpSrvSsiAttrVisible, ["TCPIP_STACK_USE_HTTP_SERVER_V2" , "TCPIP_HTTP_SSI_PROCESS"])

    # Maximum Length of a SSI Variable Name
    tcpipHttpSrvSsiVarNameLenMax = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_SSI_VARIABLE_NAME_MAX_LENGTH", tcpipHttpSrvSsiProc)
    tcpipHttpSrvSsiVarNameLenMax.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvSsiVarNameLenMax.setLabel("Maximum Length of a SSI Variable Name")
    tcpipHttpSrvSsiVarNameLenMax.setVisible(True)
    tcpipHttpSrvSsiVarNameLenMax.setDescription("Maximum Length of a SSI Variable Name")
    tcpipHttpSrvSsiVarNameLenMax.setDefaultValue(10)
    tcpipHttpSrvSsiVarNameLenMax.setDependencies(tcpipHttpSrvSsiAttrVisible, ["TCPIP_STACK_USE_HTTP_SERVER_V2" , "TCPIP_HTTP_SSI_PROCESS"])

    # Maximum Size of a SSI String Variable Value
    tcpipHttpSrvSsiVarStrLenMax = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_SSI_VARIABLE_STRING_MAX_LENGTH", tcpipHttpSrvSsiProc)
    tcpipHttpSrvSsiVarStrLenMax.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvSsiVarStrLenMax.setLabel("Maximum Size of a SSI String Variable Value")
    tcpipHttpSrvSsiVarStrLenMax.setVisible(True)
    tcpipHttpSrvSsiVarStrLenMax.setDescription("Maximum Size of a SSI String Variable Value")
    tcpipHttpSrvSsiVarStrLenMax.setDefaultValue(10)
    tcpipHttpSrvSsiVarStrLenMax.setDependencies(tcpipHttpSrvSsiAttrVisible, ["TCPIP_STACK_USE_HTTP_SERVER_V2" , "TCPIP_HTTP_SSI_PROCESS"])

    # Message to Echo when Echoing a Not Found Variable
    tcpipHttpSrvSsiEchoNotFoundMsg = tcpipHttpSrvV2Component.createStringSymbol("TCPIP_HTTP_SSI_ECHO_NOT_FOUND_MESSAGE", tcpipHttpSrvSsiProc)
    tcpipHttpSrvSsiEchoNotFoundMsg.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvSsiEchoNotFoundMsg.setLabel("Message to Echo when Echoing a Not Found Variable")
    tcpipHttpSrvSsiEchoNotFoundMsg.setVisible(True)
    tcpipHttpSrvSsiEchoNotFoundMsg.setDescription("Message to Echo when Echoing a Not Found Variable")
    tcpipHttpSrvSsiEchoNotFoundMsg.setDefaultValue("SSI Echo - Not Found: ")
    tcpipHttpSrvSsiEchoNotFoundMsg.setDependencies(tcpipHttpSrvSsiAttrVisible, ["TCPIP_STACK_USE_HTTP_SERVER_V2" , "TCPIP_HTTP_SSI_PROCESS"])
    
    # Include HTTP Custom Template SL
    tcpipHttpSrvCustTemplateSl = tcpipHttpSrvV2Component.createBooleanSymbol("TCPIP_HTTP_CUSTOM_TEMPLATE_SL", None)
    tcpipHttpSrvCustTemplateSl.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvCustTemplateSl.setVisible(False)
    tcpipHttpSrvCustTemplateSl.setDescription("Include HTTP Custom Template SL")
    tcpipHttpSrvCustTemplateSl.setDefaultValue((Database.getSymbolValue("sys_fs", "SYS_FS_MPFS") == True))
    tcpipHttpSrvCustTemplateSl.setDependencies(tcpipHttpSrvCustomSlSet, ["sys_fs.SYS_FS_MPFS"])

    # Persistent Connection Idle Time-out
    tcpipHttpSrvConnTimeout = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_CONNECTION_TIMEOUT", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvConnTimeout.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvConnTimeout.setLabel("Persistent Connection Idle Timeout (in sec)")
    tcpipHttpSrvConnTimeout.setVisible(True)
    tcpipHttpSrvConnTimeout.setDescription("Persistent Connection Idle Time-out")
    tcpipHttpSrvConnTimeout.setDefaultValue(0)

    # HTTP allocation function, malloc style
    tcpipHttpSrvMallocFunct = tcpipHttpSrvV2Component.createStringSymbol("TCPIP_HTTP_MALLOC_FUNC", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvMallocFunct.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvMallocFunct.setLabel("HTTP allocation function, malloc style")
    tcpipHttpSrvMallocFunct.setVisible(True)
    tcpipHttpSrvMallocFunct.setDescription("HTTP allocation function, malloc style")
    tcpipHttpSrvMallocFunct.setDefaultValue("malloc")

    # HTTP deallocation function, free style
    tcpipHttpSrvFreeFunct = tcpipHttpSrvV2Component.createStringSymbol("TCPIP_HTTP_FREE_FUNC", tcpipHttpSrvAdvSettings)
    tcpipHttpSrvFreeFunct.setHelp("mcc_h3_http_server_configurations")
    tcpipHttpSrvFreeFunct.setLabel("HTTP deallocation function, free style")
    tcpipHttpSrvFreeFunct.setVisible(True)
    tcpipHttpSrvFreeFunct.setDescription("HTTP deallocation function, free style")
    tcpipHttpSrvFreeFunct.setDefaultValue("free")


    tcpipHttpSrvheapdependency = [  "TCPIP_HTTP_MAX_CONNECTIONS", "TCPIP_HTTP_MAX_DATA_LEN", 
                                    "TCPIP_HTTP_CHUNKS_NUMBER", "TCPIP_HTTP_DYNVAR_DESCRIPTORS_NUMBER", 
                                    "TCPIP_HTTP_FILE_PROCESS_BUFFERS_NUMBER", "TCPIP_HTTP_FILE_PROCESS_BUFFER_SIZE", 
                                    "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]    
        
    # HTTP Heap Size
    tcpipHttpSrvHeapSize = tcpipHttpSrvV2Component.createIntegerSymbol("TCPIP_HTTP_HEAP_SIZE", None)
    tcpipHttpSrvHeapSize.setLabel("HTTP Heap Size (bytes)") 
    tcpipHttpSrvHeapSize.setVisible(False)
    tcpipHttpSrvHeapSize.setDefaultValue(tcpipHttpSrvHeapCalc())
    tcpipHttpSrvHeapSize.setReadOnly(True)
    tcpipHttpSrvHeapSize.setDependencies(tcpipHttpSrvHeapUpdate, tcpipHttpSrvheapdependency)  

    #Add to system_config.h
    tcpipHttpSrvHeaderFtl = tcpipHttpSrvV2Component.createFileSymbol(None, None)
    tcpipHttpSrvHeaderFtl.setSourcePath("tcpip/config/http_server.h.ftl")
    tcpipHttpSrvHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipHttpSrvHeaderFtl.setMarkup(True)
    tcpipHttpSrvHeaderFtl.setType("STRING")


    # Add http_server.c file
    tcpipHttpSrvSourceFile = tcpipHttpSrvV2Component.createFileSymbol(None, None)
    tcpipHttpSrvSourceFile.setSourcePath("tcpip/src/http_server.c")
    tcpipHttpSrvSourceFile.setOutputName("http_server.c")
    tcpipHttpSrvSourceFile.setOverwrite(True)
    tcpipHttpSrvSourceFile.setDestPath("library/tcpip/src/")
    tcpipHttpSrvSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipHttpSrvSourceFile.setType("SOURCE")
    tcpipHttpSrvSourceFile.setEnabled(True)
    tcpipHttpSrvSourceFile.setDependencies(tcpipHttpSrvGenSourceFile, ["TCPIP_STACK_USE_HTTP_SERVER_V2"])


    # ifblock TCPIP_HTTP_V2_CUSTOM_TEMPLATE

    # template HTTP_SERVER_PRINT_C "$HARMONY_VERSION_PATH/framework/tcpip/config/custom_app/http_server_print.c.ftl" to "$PROJECT_SOURCE_FILES/app/http_server_print.c"
    tcpipHttpSrvPrintSourceFile = tcpipHttpSrvV2Component.createFileSymbol(None, None)
    tcpipHttpSrvPrintSourceFile.setSourcePath("tcpip/config/custom_app/http_server_print.c.ftl")
    tcpipHttpSrvPrintSourceFile.setOutputName("http_server_print.c")
    tcpipHttpSrvPrintSourceFile.setDestPath("../../")
    tcpipHttpSrvPrintSourceFile.setProjectPath("")
    tcpipHttpSrvPrintSourceFile.setType("SOURCE")
    tcpipHttpSrvPrintSourceFile.setMarkup(True)
    tcpipHttpSrvPrintSourceFile.setDependencies(tcpipHttpSrvGenSourceFile, ["TCPIP_HTTP_V2_CUSTOM_TEMPLATE"])
    
    # template HTTP_SERVER_PRINT_H "$HARMONY_VERSION_PATH/framework/tcpip/config/custom_app/http_server_print.h.ftl" to "$PROJECT_SOURCE_FILES/app/http_server_print.h"
    tcpipHttpSrvPrintHeaderFile = tcpipHttpSrvV2Component.createFileSymbol(None, None)
    tcpipHttpSrvPrintHeaderFile.setSourcePath("tcpip/config/custom_app/http_server_print.h.ftl")
    tcpipHttpSrvPrintHeaderFile.setOutputName("http_server_print.h")
    tcpipHttpSrvPrintHeaderFile.setDestPath("../../")
    tcpipHttpSrvPrintHeaderFile.setProjectPath("")
    tcpipHttpSrvPrintHeaderFile.setType("HEADER")
    tcpipHttpSrvPrintHeaderFile.setMarkup(True)
    tcpipHttpSrvPrintHeaderFile.setDependencies(tcpipHttpSrvGenSourceFile, ["TCPIP_HTTP_V2_CUSTOM_TEMPLATE"])
    
    # template HTTP_SERVER_CUSTOM_APP_C "$HARMONY_VERSION_PATH/framework/tcpip/config/custom_app/custom_http_server_app.c.ftl" to "$PROJECT_SOURCE_FILES/app/custom_http_server_app.c"
    tcpipHttpSrvCstmAppSourceFile = tcpipHttpSrvV2Component.createFileSymbol(None, None)
    tcpipHttpSrvCstmAppSourceFile.setSourcePath("tcpip/config/custom_app/custom_http_server_app.c.ftl")
    tcpipHttpSrvCstmAppSourceFile.setOutputName("custom_http_server_app.c")
    tcpipHttpSrvCstmAppSourceFile.setDestPath("../../")
    tcpipHttpSrvCstmAppSourceFile.setProjectPath("")
    tcpipHttpSrvCstmAppSourceFile.setType("SOURCE")
    tcpipHttpSrvCstmAppSourceFile.setMarkup(True)
    tcpipHttpSrvCstmAppSourceFile.setDependencies(tcpipHttpSrvGenSourceFile, ["TCPIP_HTTP_V2_CUSTOM_TEMPLATE"])
    
    # ifblock TCPIP_HTTP_CUSTOM_TEMPLATE_SL
    # template HTTP_SERVER_MPFS_IMG "$HARMONY_VERSION_PATH/framework/tcpip/config/custom_app/mpfs_server_img.c.ftl" to "$PROJECT_SOURCE_FILES/app/mpfs_server_img.c"
    # endif
    tcpipHttpSrvMpfsImg2SourceFile = tcpipHttpSrvV2Component.createFileSymbol(None, None)
    tcpipHttpSrvMpfsImg2SourceFile.setSourcePath("tcpip/config/custom_app/mpfs_server_img.c.ftl")
    tcpipHttpSrvMpfsImg2SourceFile.setOutputName("mpfs_server_img.c")
    tcpipHttpSrvMpfsImg2SourceFile.setDestPath("../../")
    tcpipHttpSrvMpfsImg2SourceFile.setProjectPath("")
    tcpipHttpSrvMpfsImg2SourceFile.setType("SOURCE")
    tcpipHttpSrvMpfsImg2SourceFile.setMarkup(True)
    tcpipHttpSrvMpfsImg2SourceFile.setEnabled(False)
    tcpipHttpSrvMpfsImg2SourceFile.setDependencies(tcpipHttpSrvGenSourceFile, ["TCPIP_HTTP_CUSTOM_TEMPLATE_SL"])    

    execfile(Module.getPath() + "/tcpip/config/webpage_v2.py")

# make Http Server option visible
def tcpipHttpSrvV2Visible(tcpipDependentSymbol, tcpipIPSymbol):    
    tcpipHttp = Database.getSymbolValue("tcpipHttp","TCPIP_STACK_USE_HTTP_SERVER")
    tcpipHttpNetSrv = Database.getSymbolValue("tcpipHttpNetSrv","TCPIP_STACK_USE_HTTP_NET_SERVER")
    tcpipTcp = Database.getSymbolValue("tcpipTcp","TCPIP_USE_TCP")

    if(tcpipTcp):
        tcpipDependentSymbol.setVisible(True)
        if(tcpipHttp or tcpipHttpNetSrv):
            tcpipDependentSymbol.setReadOnly(True)
        else:           
            tcpipDependentSymbol.setReadOnly(False)
    else:
        tcpipDependentSymbol.setVisible(False)


# make Http Module file upload name option visible
def tcpipHttpSrvFileUploadVisible(symbol, event):
    if (event["value"] == True):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

# make Http Server Config Flag Secure options visible
def tcpipHttpSrvV2ConfigFlagSecureOnEnable(tcpipDependentSymbol, tcpipIPSymbol):   
    #tcpipHttpSrvV2 = Database.getSymbolValue("tcpipHttpSrvV2","TCPIP_STACK_USE_HTTP_SERVER_V2")
    tcpipHttpSrvV2ConfigFlagSecureOff = Database.getSymbolValue("tcpipHttpSrvV2","TCPIP_HTTP_CONFIG_FLAG_SECURE_OFF")
    tcpipHttpSrvV2ConfigFlagSecureDefault = Database.getSymbolValue("tcpipHttpSrvV2","TCPIP_HTTP_CONFIG_FLAG_SECURE_DEFAULT")

    #if(tcpipHttpSrvV2):
    if(tcpipHttpSrvV2ConfigFlagSecureOff or tcpipHttpSrvV2ConfigFlagSecureDefault):
        tcpipDependentSymbol.setVisible(False)
        print("SecureOn disable")
    else:
        tcpipDependentSymbol.setVisible(True)
        print("SecureOn enable")
    # else:
        # tcpipDependentSymbol.setVisible(False)
        # print("SecureOn disable") 


def tcpipHttpSrvV2ConfigFlagSecureOffEnable(tcpipDependentSymbol, tcpipIPSymbol):  
    #tcpipHttpSrvV2 = Database.getSymbolValue("tcpipHttpSrvV2","TCPIP_STACK_USE_HTTP_SERVER_V2")
    tcpipHttpSrvV2ConfigFlagSecureOn = Database.getSymbolValue("tcpipHttpSrvV2","TCPIP_HTTP_CONFIG_FLAG_SECURE_ON")
    tcpipHttpSrvV2ConfigFlagSecureDefault = Database.getSymbolValue("tcpipHttpSrvV2","TCPIP_HTTP_CONFIG_FLAG_SECURE_DEFAULT")
    
    #if(tcpipHttpSrvV2):
    if(tcpipHttpSrvV2ConfigFlagSecureOn or tcpipHttpSrvV2ConfigFlagSecureDefault):
        tcpipDependentSymbol.setVisible(False)
        print("SecureOff disable")
    else:
        tcpipDependentSymbol.setVisible(True)
        print("SecureOff enable")
    # else:
        # tcpipDependentSymbol.setVisible(False)
        # print("SecureOff disable")


def tcpipHttpV2ConfigFlagSecureDefaultEnable(tcpipDependentSymbol, tcpipIPSymbol):  
    #tcpipHttpSrvV2 = Database.getSymbolValue("tcpipHttpSrvV2","TCPIP_STACK_USE_HTTP_SERVER_V2")
    tcpipHttpSrvV2ConfigFlagSecureOn = Database.getSymbolValue("tcpipHttpSrvV2","TCPIP_HTTP_CONFIG_FLAG_SECURE_ON")
    tcpipHttpSrvV2ConfigFlagSecureOff = Database.getSymbolValue("tcpipHttpSrvV2","TCPIP_HTTP_CONFIG_FLAG_SECURE_OFF")
    
    #if(tcpipHttpSrvV2):      
    if(tcpipHttpSrvV2ConfigFlagSecureOn or tcpipHttpSrvV2ConfigFlagSecureOff):
        tcpipDependentSymbol.setVisible(False)
        print("Securedefault disable")
    else:
        tcpipDependentSymbol.setVisible(True)
        print("Securedefault enable")
    # else:
        # tcpipDependentSymbol.setVisible(False)
        # print("Securedefault disable")

# make Http Module Dynamic Variable options visible
def tcpipHttpSrvDynVarVisible(tcpipDependentSymbol, tcpipIPSymbol):
    tcpipHttpSrvV2 = Database.getSymbolValue("tcpipHttpSrvV2","TCPIP_STACK_USE_HTTP_SERVER_V2")
    tcpipHttpSrvDynVarProcess = Database.getSymbolValue("tcpipHttpSrvV2","TCPIP_HTTP_DYNVAR_PROCESS")

    if(tcpipHttpSrvV2 and tcpipHttpSrvDynVarProcess):
        tcpipDependentSymbol.setVisible(True)
    else:
        tcpipDependentSymbol.setVisible(False)  

# make Http Module SSI Attribute options visible
def tcpipHttpSrvSsiAttrVisible(tcpipDependentSymbol, tcpipIPSymbol):    
    tcpipHttpSrvV2 = Database.getSymbolValue("tcpipHttpSrvV2","TCPIP_STACK_USE_HTTP_SERVER_V2")
    tcpipHttpSrvSsiProcess= Database.getSymbolValue("tcpipHttpSrvV2","TCPIP_HTTP_SSI_PROCESS")

    if(tcpipHttpSrvV2 and tcpipHttpSrvSsiProcess):
        tcpipDependentSymbol.setVisible(True)
    else:
        tcpipDependentSymbol.setVisible(False)

# make Http Module custom http Attribute options visible
def tcpipHttpSrvWebServerPathVisible(tcpipDependentSymbol, tcpipIPSymbol):
    tcpipHttpSrvV2 = Database.getSymbolValue("tcpipHttpSrvV2","TCPIP_STACK_USE_HTTP_SERVER_V2")
    tcpipHttpSrvCustomHTTPProcess = Database.getSymbolValue("tcpipHttpSrvV2","TCPIP_HTTP_V2_CUSTOM_TEMPLATE")

    if(tcpipHttpSrvV2 and tcpipHttpSrvCustomHTTPProcess):
        tcpipDependentSymbol.setVisible(True)
    else:
        tcpipDependentSymbol.setVisible(False)


def tcpipHttpSrvMenuVisibleSingle(symbol, event):
    if (event["value"] == True):
        print("TFTPC Menu Visible.")
        symbol.setVisible(True)
    else:
        print("TFTPC Menu Invisible.")
        symbol.setVisible(False)

def tcpipHttpSrvBase64DecodeOpt(symbol, event):
    symbol.clearValue()
    if (event["value"] == True):
        symbol.setValue(True,2)
    else:
        symbol.setValue(False,2)

def tcpipHttpSrvGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])

def tcpipHttpSrvCustomSlSet(symbol, event):
    symbol.clearValue()
    if (event["value"] == True):
        symbol.setValue(True,2)
    else:
        symbol.setValue(False,2)


def tcpipHttpSrvHeapCalc(): 
    nConnections = Database.getSymbolValue("tcpipHttpSrvV2","TCPIP_HTTP_MAX_CONNECTIONS")
    dataLen = Database.getSymbolValue("tcpipHttpSrvV2","TCPIP_HTTP_MAX_DATA_LEN")
    nChunks = Database.getSymbolValue("tcpipHttpSrvV2","TCPIP_HTTP_CHUNKS_NUMBER")
    nDescriptors = Database.getSymbolValue("tcpipHttpSrvV2","TCPIP_HTTP_DYNVAR_DESCRIPTORS_NUMBER")
    nFileBuffers = Database.getSymbolValue("tcpipHttpSrvV2","TCPIP_HTTP_FILE_PROCESS_BUFFERS_NUMBER")
    fileBufferSize = Database.getSymbolValue("tcpipHttpSrvV2","TCPIP_HTTP_FILE_PROCESS_BUFFER_SIZE")
    
    heap_size = (nConnections * (352 + dataLen)) + (nChunks * 80) + (nDescriptors * 16) + (nFileBuffers * (fileBufferSize + 32)) 
    return heap_size    
    
def tcpipHttpSrvHeapUpdate(symbol, event): 
    heap_size = tcpipHttpSrvHeapCalc()
    symbol.setValue(heap_size)
    if(event["id"] == "TCPIP_STACK_HEAP_CALC_MASK"):
        symbol.setVisible(event["value"])

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
    Database.setSymbolValue("tcpipHttpSrvV2", "TCPIP_STACK_USE_HTTP_SERVER_V2", False, 2)
