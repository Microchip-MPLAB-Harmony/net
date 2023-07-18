"""
Copyright (C) 2019-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

def instantiateComponent(tcpipHttpNetComponent):
    import re
    import os
    import sys
    configName = Variables.get("__CONFIGURATION_NAME")
    # Enable HTTP NET Server
    tcpipHttpNetSrv = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_STACK_USE_HTTP_NET_SERVER", None)
    tcpipHttpNetSrv.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetSrv.setLabel("HTTP NET Server")
    tcpipHttpNetSrv.setVisible(False)
    tcpipHttpNetSrv.setDescription("Enable HTTP NET Server")
    tcpipHttpNetSrv.setDefaultValue(True)

    # HTTP NET Socket TX Buffer Size
    tcpipHttpNetSktTxBuffsize = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_SKT_TX_BUFF_SIZE", None)
    tcpipHttpNetSktTxBuffsize.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetSktTxBuffsize.setLabel("HTTP NET Socket TX Buffer Size")
    tcpipHttpNetSktTxBuffsize.setVisible(True)
    tcpipHttpNetSktTxBuffsize.setDescription("HTTP NET Socket TX Buffer Size")
    tcpipHttpNetSktTxBuffsize.setDefaultValue(1024)

    # HTTP NET Socket RX Buffer Size
    tcpipHttpNetSktRxBuffsize = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_SKT_RX_BUFF_SIZE", None)
    tcpipHttpNetSktRxBuffsize.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetSktRxBuffsize.setLabel("HTTP NET Socket RX Buffer Size")
    tcpipHttpNetSktRxBuffsize.setVisible(True)
    tcpipHttpNetSktRxBuffsize.setDescription("HTTP NET Socket RX Buffer Size")
    tcpipHttpNetSktRxBuffsize.setDefaultValue(1024)

    # Maximum Number of Simultaneous Connections
    tcpipHttpNetConnMaxNum = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_MAX_CONNECTIONS", None)
    tcpipHttpNetConnMaxNum.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetConnMaxNum.setLabel("Maximum Number of Simultaneous Connections")
    tcpipHttpNetConnMaxNum.setVisible(True)
    tcpipHttpNetConnMaxNum.setDescription("Maximum Number of Simultaneous Connections")
    tcpipHttpNetConnMaxNum.setDefaultValue(4)
    
    # HTTP NET Listening Port
    tcpipHttpNetListenPort = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_LISTEN_PORT", None)
    tcpipHttpNetListenPort.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetListenPort.setLabel("HTTP NET Listening Port")
    tcpipHttpNetListenPort.setVisible(True)
    tcpipHttpNetListenPort.setDescription("HTTP NET Listening Port")
    tcpipHttpNetListenPort.setDefaultValue(80)

    # Default Web pages directory
    tcpipHttpNetWebDir = tcpipHttpNetComponent.createStringSymbol("TCPIP_HTTP_NET_WEB_DIR", None)
    tcpipHttpNetWebDir.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetWebDir.setLabel("Web Pages Directory")
    tcpipHttpNetWebDir.setVisible(True)
    tcpipHttpNetWebDir.setDescription("Web Pages Directory")
    tcpipHttpNetWebDir.setDefaultValue("/mnt/mchpSite1/")
    
    # Enable HTTPNET Console Commands
    tcpipHttpNetCmdEnable = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_CONSOLE_CMD", None) 
    tcpipHttpNetCmdEnable.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetCmdEnable.setLabel("Enable Console Commands")
    tcpipHttpNetCmdEnable.setVisible(True)
    tcpipHttpNetCmdEnable.setDescription("Enable HTTPNET Console Commands")
    tcpipHttpNetCmdEnable.setDefaultValue(False)
        
    # Include HTTP NET Custom Template
    tcpipHttpNetCustTemplate = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_CUSTOM_TEMPLATE", None)
    tcpipHttpNetCustTemplate.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetCustTemplate.setLabel("Include HTTP NET Custom Template")
    tcpipHttpNetCustTemplate.setVisible(True)
    tcpipHttpNetCustTemplate.setDescription("Include HTTP NET Custom Template")
    tcpipHttpNetCustTemplate.setDefaultValue(True)


    # Message to provide the source web page path which will be used for the webpage.py
    tcpipHttpNetWebPageDirPath = tcpipHttpNetComponent.createStringSymbol("TCPIP_HTTP_NET_WEBPAGE_DIRECTORY_PATH", tcpipHttpNetCustTemplate)
    tcpipHttpNetWebPageDirPath.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetWebPageDirPath.setLabel("Web pages source directory path")
    tcpipHttpNetWebPageDirPath.setVisible(True)
    tcpipHttpNetWebPageDirPath.setDescription("Configure Webpage directory path")
    tcpipHttpNetWebPageDirPath.setDefaultValue(Module.getPath() + "web_pages")
    tcpipHttpNetWebPageDirPath.setDependencies(tcpipHttpNetWebServerPathVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_CUSTOM_TEMPLATE"])

    tcpipHttpNetDestWebPageDirPath = tcpipHttpNetComponent.createKeyValueSetSymbol("TCPIP_HTTP_NET_DEST_WEBPAGE_DIRECTORY_PATH",tcpipHttpNetCustTemplate)
    tcpipHttpNetDestWebPageDirPath.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetDestWebPageDirPath.setLabel("Web pages destination directory path")
    tcpipHttpNetDestWebPageDirPath.setVisible(True)
    # tcpipHttpNetDestWebPageDirPath.addKey("DESTINATION PATH", "0", Module.getPath() + "apps"+os.path.sep+"<project>"+os.path.sep+"firmware"+os.path.sep+"src"+os.path.sep+"web_pages")
    tcpipHttpNetDestWebPageDirPath.addKey("DESTINATION PATH", "0", "<project_path>"+os.path.sep+"firmware"+os.path.sep+"src"+os.path.sep+"web_pages")
    tcpipHttpNetDestWebPageDirPath.setDisplayMode("Description")
    tcpipHttpNetDestWebPageDirPath.setOutputMode("Key")
    tcpipHttpNetDestWebPageDirPath.setDefaultValue(0)
    tcpipHttpNetDestWebPageDirPath.setReadOnly(True)
    tcpipHttpNetDestWebPageDirPath.setDependencies(tcpipHttpNetWebServerPathVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_CUSTOM_TEMPLATE"])

    # Message that Number of webpage files accepted
    tcpipHttpNetWebPageFileCount = tcpipHttpNetComponent.createCommentSymbol("TCPIP_WEBPAGE_FILES_COUNT_COMMENT", tcpipHttpNetCustTemplate)
    tcpipHttpNetWebPageFileCount.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetWebPageFileCount.setVisible(True)
    tcpipHttpNetWebPageFileCount.setLabel("*** Maximum 100 web page files currently supported ***")
    tcpipHttpNetWebPageFileCount.setDependencies(tcpipHttpNetWebServerPathVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_CUSTOM_TEMPLATE"])

    # Advanced Settings
    tcpipHttpNetAdvSettings = tcpipHttpNetComponent.createMenuSymbol("TCPIP_HTTP_NET_ADV_SETTING", None)
    tcpipHttpNetAdvSettings.setLabel("Advanced Settings")
    tcpipHttpNetAdvSettings.setDescription("Advanced Settings")
    tcpipHttpNetAdvSettings.setVisible(True)

    # HTTP NET Task Rate - ms
    tcpipHttpNetTskRate = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_TASK_RATE", tcpipHttpNetAdvSettings)
    tcpipHttpNetTskRate.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetTskRate.setLabel("HTTP NET Task Rate (in msec)")
    tcpipHttpNetTskRate.setVisible(True)
    tcpipHttpNetTskRate.setDescription("HTTP NET Task Rate - ms")
    tcpipHttpNetTskRate.setDefaultValue(33)
    
    # Maximum Data Length (bytes) for Reading Cookie and GET/POST Arguments
    tcpipHttpNetDataLenMax = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_MAX_DATA_LEN", tcpipHttpNetAdvSettings)
    tcpipHttpNetDataLenMax.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetDataLenMax.setLabel("Maximum Data Length (bytes) for Reading Cookie and GET/POST Arguments")
    tcpipHttpNetDataLenMax.setVisible(True)
    tcpipHttpNetDataLenMax.setDescription("Maximum Data Length (bytes) for Reading Cookie and GET/POST Arguments")
    tcpipHttpNetDataLenMax.setDefaultValue(100)

    # Size of the Buffer Used for Sending Response Messages to the Client
    tcpipHttpNetRespBuffSize = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_RESPONSE_BUFFER_SIZE", tcpipHttpNetAdvSettings)
    tcpipHttpNetRespBuffSize.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetRespBuffSize.setLabel("Buffer Size for Sending Response Messages to the Client")
    tcpipHttpNetRespBuffSize.setVisible(True)
    tcpipHttpNetRespBuffSize.setDescription("Size of the Buffer Used for Sending Response Messages to the Client")
    tcpipHttpNetRespBuffSize.setDefaultValue(300)

    # Size of the Buffer Used for Sending Cookies to the Client
    tcpipHttpNetCookieBuffSize = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_COOKIE_BUFFER_SIZE", tcpipHttpNetAdvSettings)
    tcpipHttpNetCookieBuffSize.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetCookieBuffSize.setLabel("Buffer Size for Sending Cookies to the Client")
    tcpipHttpNetCookieBuffSize.setVisible(True)
    tcpipHttpNetCookieBuffSize.setDescription("Size of the Buffer Used for Sending Cookies to the Client")
    tcpipHttpNetCookieBuffSize.setDefaultValue(200)

    # Size of the Peek Buffer for Performing Searches
    tcpipHttpNetFindPeekBuffSize = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_FIND_PEEK_BUFF_SIZE", tcpipHttpNetAdvSettings)
    tcpipHttpNetFindPeekBuffSize.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetFindPeekBuffSize.setLabel("Peek Buffer Size for Performing Searches")
    tcpipHttpNetFindPeekBuffSize.setVisible(True)
    tcpipHttpNetFindPeekBuffSize.setDescription("Size of the Peek Buffer for Performing Searches")
    tcpipHttpNetFindPeekBuffSize.setDefaultValue(512)

    # Size of the Buffer for Processing HTML, Dynamic Variable and Binary Files
    tcpipHttpNetFileProcessBuffSize = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_FILE_PROCESS_BUFFER_SIZE", tcpipHttpNetAdvSettings)
    tcpipHttpNetFileProcessBuffSize.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetFileProcessBuffSize.setLabel("File Processing Buffer Size")
    tcpipHttpNetFileProcessBuffSize.setVisible(True)
    tcpipHttpNetFileProcessBuffSize.setDescription("Size of the Buffer for Processing HTML, Dynamic Variable and Binary Files")
    tcpipHttpNetFileProcessBuffSize.setDefaultValue(512)

    # Number of File Buffers to be Created
    tcpipHttpNetFileProcessBuffNum = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_FILE_PROCESS_BUFFERS_NUMBER", tcpipHttpNetAdvSettings)
    tcpipHttpNetFileProcessBuffNum.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetFileProcessBuffNum.setLabel("Number of File Buffers to be Created")
    tcpipHttpNetFileProcessBuffNum.setVisible(True)
    tcpipHttpNetFileProcessBuffNum.setDescription("Number of File Buffers to be Created")
    tcpipHttpNetFileProcessBuffNum.setDefaultValue(4)

    # Retry Limit for Allocating a File Buffer from the Pool
    tcpipHttpNetFileProcessBuffRetry = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_FILE_PROCESS_BUFFER_RETRIES", tcpipHttpNetAdvSettings)
    tcpipHttpNetFileProcessBuffRetry.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetFileProcessBuffRetry.setLabel("Retry Limit for Allocating a File Buffer from the Pool")
    tcpipHttpNetFileProcessBuffRetry.setVisible(True)
    tcpipHttpNetFileProcessBuffRetry.setDescription("Retry Limit for Allocating a File Buffer from the Pool")
    tcpipHttpNetFileProcessBuffRetry.setDefaultValue(10)

    # Retry Limit for Allocating a File Buffer from the Pool
    tcpipHttpNetChunksNum = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_CHUNKS_NUMBER", tcpipHttpNetAdvSettings)
    tcpipHttpNetChunksNum.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetChunksNum.setLabel("Number of Chunks to be Created")
    tcpipHttpNetChunksNum.setVisible(True)
    tcpipHttpNetChunksNum.setDescription("Number of Chunks to be Created")
    tcpipHttpNetChunksNum.setDefaultValue(10)

    # Retry Limit for Allocating a Chunk from the Pool
    tcpipHttpNetChunkRetry = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_CHUNK_RETRIES", tcpipHttpNetAdvSettings)
    tcpipHttpNetChunkRetry.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetChunkRetry.setLabel("Retry Limit for Allocating a Chunk from the Pool")
    tcpipHttpNetChunkRetry.setVisible(True)
    tcpipHttpNetChunkRetry.setDescription("Retry Limit for Allocating a Chunk from the Pool")
    tcpipHttpNetChunkRetry.setDefaultValue(10)

    # The Maximum Depth of Recursive Calls for Serving a Web Page
    tcpipHttpNetRecursiveLvl = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_MAX_RECURSE_LEVEL", tcpipHttpNetAdvSettings)
    tcpipHttpNetRecursiveLvl.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetRecursiveLvl.setLabel("Maximum Depth of Recursive Calls for Serving a Web Page")
    tcpipHttpNetRecursiveLvl.setVisible(True)
    tcpipHttpNetRecursiveLvl.setDescription("The Maximum Depth of Recursive Calls for Serving a Web Page")
    tcpipHttpNetRecursiveLvl.setDefaultValue(3)
    
    # Maximum Header Length
    tcpipHttpNetHdrLenMax = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_MAX_HEADER_LEN", tcpipHttpNetAdvSettings)
    tcpipHttpNetHdrLenMax.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetHdrLenMax.setLabel("Maximum Header Length")
    tcpipHttpNetHdrLenMax.setVisible(True)
    tcpipHttpNetHdrLenMax.setDescription("Maximum Header Length")
    tcpipHttpNetHdrLenMax.setDefaultValue(15)

    # Maximum Lifetime of Static Responses in Seconds
    tcpipHttpNetCacheLen = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_CACHE_LEN", tcpipHttpNetAdvSettings)
    tcpipHttpNetCacheLen.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetCacheLen.setLabel("Maximum Lifetime of Static Responses (in sec)")
    tcpipHttpNetCacheLen.setVisible(True)
    tcpipHttpNetCacheLen.setDescription("Maximum Lifetime of Static Responses in Seconds")
    tcpipHttpNetCacheLen.setDefaultValue(600)

    # Socket Disconnect Time-out
    tcpipHttpNetTimeout = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_TIMEOUT", tcpipHttpNetAdvSettings)
    tcpipHttpNetTimeout.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetTimeout.setLabel("Socket Disconnect Timeout (in sec)")
    tcpipHttpNetTimeout.setVisible(True)
    tcpipHttpNetTimeout.setDescription("Socket Disconnect Time-out")
    tcpipHttpNetTimeout.setDefaultValue(45)


    # Default HTTP NET File
    tcpipHttpNetDefaultFile = tcpipHttpNetComponent.createStringSymbol("TCPIP_HTTP_NET_DEFAULT_FILE", tcpipHttpNetAdvSettings)
    tcpipHttpNetDefaultFile.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetDefaultFile.setLabel("Default HTTP NET File")
    tcpipHttpNetDefaultFile.setVisible(True)
    tcpipHttpNetDefaultFile.setDescription("Default HTTP NET File")
    tcpipHttpNetDefaultFile.setDefaultValue("index.htm")

    # Maximum Size of a HTTP File Name
    tcpipHttpNetFilenameLenMax = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_FILENAME_MAX_LEN", tcpipHttpNetAdvSettings)
    tcpipHttpNetFilenameLenMax.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetFilenameLenMax.setLabel("Maximum Size of a HTTP File Name")
    tcpipHttpNetFilenameLenMax.setVisible(True)
    tcpipHttpNetFilenameLenMax.setDescription("Maximum Size of a HTTP File Name")
    tcpipHttpNetFilenameLenMax.setDefaultValue(25)


    # Enable MPFS Update via HTTP NET
    tcpipHttpNetFileUpload = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_FILE_UPLOAD_ENABLE", tcpipHttpNetAdvSettings)
    tcpipHttpNetFileUpload.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetFileUpload.setLabel("Enable MPFS Update via HTTP NET")
    tcpipHttpNetFileUpload.setVisible(True)
    tcpipHttpNetFileUpload.setDescription("Enable MPFS Update via HTTP NET")
    tcpipHttpNetFileUpload.setDefaultValue(False)

    # MPFS Upload Page Name
    tcpipHttpNetFileUploadName = tcpipHttpNetComponent.createStringSymbol("TCPIP_HTTP_NET_FILE_UPLOAD_NAME", tcpipHttpNetFileUpload)
    tcpipHttpNetFileUploadName.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetFileUploadName.setLabel("MPFS Upload Page Name")
    tcpipHttpNetFileUploadName.setVisible(False)
    tcpipHttpNetFileUploadName.setDescription("MPFS Upload Page Name")
    tcpipHttpNetFileUploadName.setDefaultValue("mpfsupload")
    tcpipHttpNetFileUploadName.setDependencies(tcpipHttpNetFileUploadVisible, ["TCPIP_HTTP_NET_FILE_UPLOAD_ENABLE"])
    
    # MPFS NVM Mount Path
    tcpipHttpNetMpfsNvmMountPath = tcpipHttpNetComponent.createStringSymbol("TCPIP_HTTP_NET_MPFS_NVM_PATH", tcpipHttpNetFileUpload)
    tcpipHttpNetMpfsNvmMountPath.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetMpfsNvmMountPath.setLabel("MPFS NVM Mount Path")
    tcpipHttpNetMpfsNvmMountPath.setVisible(False)
    tcpipHttpNetMpfsNvmMountPath.setDescription("MPFS NVM Mount Path")
    tcpipHttpNetMpfsNvmMountPath.setDefaultValue("/mnt/mchpSite1")
    tcpipHttpNetMpfsNvmMountPath.setDependencies(tcpipHttpNetFileUploadVisible, ["TCPIP_HTTP_NET_FILE_UPLOAD_ENABLE"])
    
    # MPFS NVM Disk Path
    tcpipHttpNetMpfsNvmDiskPath = tcpipHttpNetComponent.createStringSymbol("TCPIP_HTTP_NET_MPFS_NVM_VOL", tcpipHttpNetFileUpload)
    tcpipHttpNetMpfsNvmDiskPath.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetMpfsNvmDiskPath.setLabel("MPFS NVM Disk Path")
    tcpipHttpNetMpfsNvmDiskPath.setVisible(False)
    tcpipHttpNetMpfsNvmDiskPath.setDescription("MPFS NVM Disk Path")
    tcpipHttpNetMpfsNvmDiskPath.setDefaultValue("/dev/nvma1")    
    tcpipHttpNetMpfsNvmDiskPath.setDependencies(tcpipHttpNetFileUploadVisible, ["TCPIP_HTTP_NET_FILE_UPLOAD_ENABLE"])   
    
    # MPFS NVM Disk Number
    tcpipHttpNetMpfsNvmDiskNum= tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_MPFS_NVM_NUM", tcpipHttpNetFileUpload)
    tcpipHttpNetMpfsNvmDiskNum.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetMpfsNvmDiskNum.setLabel("MPFS NVM Disk Number")
    tcpipHttpNetMpfsNvmDiskNum.setVisible(False)
    tcpipHttpNetMpfsNvmDiskNum.setDescription("MPFS NVM Disk Number")
    tcpipHttpNetMpfsNvmDiskNum.setDefaultValue(0)    
    tcpipHttpNetMpfsNvmDiskNum.setDependencies(tcpipHttpNetFileUploadVisible, ["TCPIP_HTTP_NET_FILE_UPLOAD_ENABLE"])   
    
    # Enable POST Support
    tcpipHttpNetPostSupport = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_USE_POST", tcpipHttpNetAdvSettings)
    tcpipHttpNetPostSupport.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetPostSupport.setLabel("Enable POST Support")
    tcpipHttpNetPostSupport.setVisible(True)
    tcpipHttpNetPostSupport.setDescription("Enable POST Support")
    tcpipHttpNetPostSupport.setDefaultValue(True)

    # Enable Cookie Support
    tcpipHttpNetCookieSupport = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_USE_COOKIES", tcpipHttpNetAdvSettings)
    tcpipHttpNetCookieSupport.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetCookieSupport.setLabel("Enable Cookie Support")
    tcpipHttpNetCookieSupport.setVisible(True)
    tcpipHttpNetCookieSupport.setDescription("Enable Cookie Support")
    tcpipHttpNetCookieSupport.setDefaultValue(True)

    # Use Base 64 Decode
    tcpipHttpNetBase64Decode = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_USE_BASE64_DECODE", tcpipHttpNetAdvSettings)
    tcpipHttpNetBase64Decode.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetBase64Decode.setLabel("Use Base 64 Decode")
    tcpipHttpNetBase64Decode.setVisible(True)
    tcpipHttpNetBase64Decode.setDescription("Use Base 64 Decode")
    tcpipHttpNetBase64Decode.setDefaultValue(False) 


    # Enable Basic Authentication Support
    tcpipHttpNetAuth = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_USE_AUTHENTICATION", tcpipHttpNetAdvSettings)
    tcpipHttpNetAuth.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetAuth.setLabel("Enable Basic Authentication Support")
    tcpipHttpNetAuth.setVisible(True)
    tcpipHttpNetAuth.setDescription("Enable Basic Authentication Support")
    tcpipHttpNetAuth.setDefaultValue(True)
    tcpipHttpNetBase64Decode.setDependencies(tcpipHttpNetBase64DecodeOpt, ["TCPIP_HTTP_NET_USE_AUTHENTICATION"])


    # HTTP NET Configuration Flags Settings
    tcpipHttpNetConfigFlag = tcpipHttpNetComponent.createMenuSymbol(None, tcpipHttpNetAdvSettings)
    tcpipHttpNetConfigFlag.setLabel("HTTP NET Configuration Flags")
    tcpipHttpNetConfigFlag.setVisible(True)
    tcpipHttpNetConfigFlag.setDescription("HTTP NET Configuration Flags Settings")

    # Use non-persistent connections
    tcpipHttpNetConfigFlagNonpersistent = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_CONFIG_FLAG_NON_PERSISTENT", tcpipHttpNetConfigFlag)
    tcpipHttpNetConfigFlagNonpersistent.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetConfigFlagNonpersistent.setLabel("Use non-persistent connections")
    tcpipHttpNetConfigFlagNonpersistent.setVisible(True)
    tcpipHttpNetConfigFlagNonpersistent.setDescription("Use non-persistent connections")
    tcpipHttpNetConfigFlagNonpersistent.setDefaultValue(False)

    # HTTP sockets created with NO-DELAY option
    tcpipHttpNetConfigFlagNoDly = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_CONFIG_FLAG_NO_DELAY", tcpipHttpNetConfigFlag)
    tcpipHttpNetConfigFlagNoDly.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetConfigFlagNoDly.setLabel("HTTP sockets created with NO-DELAY option")
    tcpipHttpNetConfigFlagNoDly.setVisible(True)
    tcpipHttpNetConfigFlagNoDly.setDescription("HTTP sockets created with NO-DELAY option")
    tcpipHttpNetConfigFlagNoDly.setDefaultValue(False)

    # All HTTP connections have to be secure
    tcpipHttpNetConfigFlagSecureOn = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_ON", tcpipHttpNetConfigFlag)
    tcpipHttpNetConfigFlagSecureOn.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetConfigFlagSecureOn.setLabel("All HTTP connections have to be secure")
    tcpipHttpNetConfigFlagSecureOn.setVisible(False)
    tcpipHttpNetConfigFlagSecureOn.setDescription("All HTTP connections have to be secure")
    tcpipHttpNetConfigFlagSecureOn.setDefaultValue(False)

    # All HTTP connections have to be non-secure
    tcpipHttpNetConfigFlagSecureOff = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_OFF", tcpipHttpNetConfigFlag)
    tcpipHttpNetConfigFlagSecureOff.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetConfigFlagSecureOff.setLabel("All HTTP connections have to be non-secure")
    tcpipHttpNetConfigFlagSecureOff.setVisible(False)
    tcpipHttpNetConfigFlagSecureOff.setDescription("All HTTP connections have to be non-secure")
    tcpipHttpNetConfigFlagSecureOff.setDefaultValue(False)

    # HTTP security is based on the port numbers
    tcpipHttpNetConfigFlagSecureDefault = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_DEFAULT", tcpipHttpNetConfigFlag)
    tcpipHttpNetConfigFlagSecureDefault.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetConfigFlagSecureDefault.setLabel("HTTP security is based on the port numbers")
    tcpipHttpNetConfigFlagSecureDefault.setVisible(True)
    tcpipHttpNetConfigFlagSecureDefault.setDescription("HTTP security is based on the port numbers")
    tcpipHttpNetConfigFlagSecureDefault.setDefaultValue(True)

    tcpipHttpNetConfigFlagSecureDefault.setDependencies(tcpipHttpNetSrvConfigFlagSecureDefaultEnable, ["TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_OFF", "TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_ON"])
    tcpipHttpNetConfigFlagSecureOn.setDependencies(tcpipHttpNetSrvConfigFlagSecureOnEnable, ["TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_OFF", "TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_DEFAULT"])
    tcpipHttpNetConfigFlagSecureOff.setDependencies(tcpipHttpNetSrvConfigFlagSecureOffEnable, ["TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_ON", "TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_DEFAULT"])

    # Enable the Processing of Dynamic Variables
    tcpipHttpNetDynVarProc = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_DYNVAR_PROCESS", tcpipHttpNetAdvSettings)
    tcpipHttpNetDynVarProc.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetDynVarProc.setLabel("Enable the Processing of Dynamic Variables")
    tcpipHttpNetDynVarProc.setVisible(True)
    tcpipHttpNetDynVarProc.setDescription("Enable the Processing of Dynamic Variables")
    tcpipHttpNetDynVarProc.setDefaultValue(True)

    # Number of the Descriptors for Dynamic Variables Processing
    tcpipHttpNetDynVarDescNum = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_DYNVAR_DESCRIPTORS_NUMBER", tcpipHttpNetDynVarProc)
    tcpipHttpNetDynVarDescNum.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetDynVarDescNum.setLabel("Number of the Descriptors for Dynamic Variables Processing")
    tcpipHttpNetDynVarDescNum.setVisible(True)
    tcpipHttpNetDynVarDescNum.setDescription("Number of the Descriptors for Dynamic Variables Processing")
    tcpipHttpNetDynVarDescNum.setDefaultValue(10)
    tcpipHttpNetDynVarDescNum.setDependencies(tcpipHttpNetDynVarVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_DYNVAR_PROCESS"])

    # Maximum Size for a Complete Dynamic Variable: Name + Args
    tcpipHttpNetDynVarLenMax = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_DYNVAR_MAX_LEN", tcpipHttpNetDynVarProc)
    tcpipHttpNetDynVarLenMax.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetDynVarLenMax.setLabel("Maximum Size for a Complete Dynamic Variable: Name + Args")
    tcpipHttpNetDynVarLenMax.setVisible(True)
    tcpipHttpNetDynVarLenMax.setDescription("Maximum Size for a Complete Dynamic Variable: Name + Args")
    tcpipHttpNetDynVarLenMax.setDefaultValue(50)
    tcpipHttpNetDynVarLenMax.setDependencies(tcpipHttpNetDynVarVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_DYNVAR_PROCESS"])

    # Maximum Number of Arguments for a Dynamic Variable
    tcpipHttpNetDynVarArgNumMax = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_DYNVAR_ARG_MAX_NUMBER", tcpipHttpNetDynVarProc)
    tcpipHttpNetDynVarArgNumMax.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetDynVarArgNumMax.setLabel("Maximum Number of Arguments for a Dynamic Variable")
    tcpipHttpNetDynVarArgNumMax.setVisible(True)
    tcpipHttpNetDynVarArgNumMax.setDescription("Maximum Number of Arguments for a Dynamic Variable")
    tcpipHttpNetDynVarArgNumMax.setDefaultValue(4)
    tcpipHttpNetDynVarArgNumMax.setDependencies(tcpipHttpNetDynVarVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_DYNVAR_PROCESS"])

    # Retry Limit for a Dynamic Variable Processing
    tcpipHttpNetDynVarProcRetry = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_DYNVAR_PROCESS_RETRIES", tcpipHttpNetDynVarProc)
    tcpipHttpNetDynVarProcRetry.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetDynVarProcRetry.setLabel("Retry Limit for a Dynamic Variable Processing")
    tcpipHttpNetDynVarProcRetry.setVisible(True)
    tcpipHttpNetDynVarProcRetry.setDescription("Retry Limit for a Dynamic Variable Processing")
    tcpipHttpNetDynVarProcRetry.setDefaultValue(10)
    tcpipHttpNetDynVarProcRetry.setDependencies(tcpipHttpNetDynVarVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_DYNVAR_PROCESS"])

    # Enable the Processing of SSI Commands
    tcpipHttpNetSsiProc = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_SSI_PROCESS", tcpipHttpNetAdvSettings)
    tcpipHttpNetSsiProc.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetSsiProc.setLabel("Enable the Processing of SSI Commands")
    tcpipHttpNetSsiProc.setVisible(True)
    tcpipHttpNetSsiProc.setDescription("Enable the Processing of SSI Commands")
    tcpipHttpNetSsiProc.setDefaultValue(True)

    # Maximum Number of Attributes for a SSI Command
    tcpipHttpNetSsiAttrNumMax = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_SSI_ATTRIBUTES_MAX_NUMBER", tcpipHttpNetSsiProc)
    tcpipHttpNetSsiAttrNumMax.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetSsiAttrNumMax.setLabel("Maximum Number of Attributes for a SSI Command")
    tcpipHttpNetSsiAttrNumMax.setVisible(True)
    tcpipHttpNetSsiAttrNumMax.setDescription("Maximum Number of Attributes for a SSI Command")
    tcpipHttpNetSsiAttrNumMax.setDefaultValue(4)
    tcpipHttpNetSsiAttrNumMax.setDependencies(tcpipHttpNetSsiAttrVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_SSI_PROCESS"])

    # Number of Static Attributes Associated to a SSI Command
    tcpipHttpNetSsiStaticAttrNum = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_SSI_STATIC_ATTTRIB_NUMBER", tcpipHttpNetSsiProc)
    tcpipHttpNetSsiStaticAttrNum.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetSsiStaticAttrNum.setLabel("Number of Static Attributes Associated to a SSI Command")
    tcpipHttpNetSsiStaticAttrNum.setVisible(False)
    tcpipHttpNetSsiStaticAttrNum.setDescription("Number of Static Attributes Associated to a SSI Command")
    tcpipHttpNetSsiStaticAttrNum.setDefaultValue(2)
    tcpipHttpNetSsiStaticAttrNum.setDependencies(tcpipHttpNetSsiAttrVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_SSI_PROCESS"])

    # Maximum Size for a SSI Command Line: Command + Attribute/Value Pairs
    tcpipHttpNetSsiCmdLenMax = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_SSI_CMD_MAX_LEN", tcpipHttpNetSsiProc)
    tcpipHttpNetSsiCmdLenMax.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetSsiCmdLenMax.setLabel("Maximum Size for a SSI Command Line: Command + Attribute/Value Pairs")
    tcpipHttpNetSsiCmdLenMax.setVisible(True)
    tcpipHttpNetSsiCmdLenMax.setDescription("Maximum Size for a SSI Command Line: Command + Attribute/Value Pairs")
    tcpipHttpNetSsiCmdLenMax.setDefaultValue(100)
    tcpipHttpNetSsiCmdLenMax.setDependencies(tcpipHttpNetSsiAttrVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_SSI_PROCESS"])

    # Maximum Number of SSI Variables that Can Be Created at Run Time
    tcpipHttpNetSsiVarNum = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_SSI_VARIABLES_NUMBER", tcpipHttpNetSsiProc)
    tcpipHttpNetSsiVarNum.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetSsiVarNum.setLabel("Maximum Number of SSI Variables that Can Be Created at Runtime")
    tcpipHttpNetSsiVarNum.setVisible(True)
    tcpipHttpNetSsiVarNum.setDescription("Maximum Number of SSI Variables that Can Be Created at Run Time")
    tcpipHttpNetSsiVarNum.setDefaultValue(13)
    tcpipHttpNetSsiVarNum.setDependencies(tcpipHttpNetSsiAttrVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_SSI_PROCESS"])

    # Maximum Length of a SSI Variable Name
    tcpipHttpNetSsiVarNameLenMax = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_SSI_VARIABLE_NAME_MAX_LENGTH", tcpipHttpNetSsiProc)
    tcpipHttpNetSsiVarNameLenMax.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetSsiVarNameLenMax.setLabel("Maximum Length of a SSI Variable Name")
    tcpipHttpNetSsiVarNameLenMax.setVisible(True)
    tcpipHttpNetSsiVarNameLenMax.setDescription("Maximum Length of a SSI Variable Name")
    tcpipHttpNetSsiVarNameLenMax.setDefaultValue(10)
    tcpipHttpNetSsiVarNameLenMax.setDependencies(tcpipHttpNetSsiAttrVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_SSI_PROCESS"])

    # Maximum Size of a SSI String Variable Value
    tcpipHttpNetSsiVarStrLenMax = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_SSI_VARIABLE_STRING_MAX_LENGTH", tcpipHttpNetSsiProc)
    tcpipHttpNetSsiVarStrLenMax.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetSsiVarStrLenMax.setLabel("Maximum Size of a SSI String Variable Value")
    tcpipHttpNetSsiVarStrLenMax.setVisible(True)
    tcpipHttpNetSsiVarStrLenMax.setDescription("Maximum Size of a SSI String Variable Value")
    tcpipHttpNetSsiVarStrLenMax.setDefaultValue(10)
    tcpipHttpNetSsiVarStrLenMax.setDependencies(tcpipHttpNetSsiAttrVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_SSI_PROCESS"])

    # Message to Echo when Echoing a Not Found Variable
    tcpipHttpNetSsiEchoNotFoundMsg = tcpipHttpNetComponent.createStringSymbol("TCPIP_HTTP_NET_SSI_ECHO_NOT_FOUND_MESSAGE", tcpipHttpNetSsiProc)
    tcpipHttpNetSsiEchoNotFoundMsg.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetSsiEchoNotFoundMsg.setLabel("Message to Echo when Echoing a Not Found Variable")
    tcpipHttpNetSsiEchoNotFoundMsg.setVisible(True)
    tcpipHttpNetSsiEchoNotFoundMsg.setDescription("Message to Echo when Echoing a Not Found Variable")
    tcpipHttpNetSsiEchoNotFoundMsg.setDefaultValue("SSI Echo - Not Found: ")
    tcpipHttpNetSsiEchoNotFoundMsg.setDependencies(tcpipHttpNetSsiAttrVisible, ["TCPIP_STACK_USE_HTTP_NET_SERVER" , "TCPIP_HTTP_NET_SSI_PROCESS"])
    
    # Include HTTP NET Custom Template SL
    tcpipHttpNetCustTemplateSl = tcpipHttpNetComponent.createBooleanSymbol("TCPIP_HTTP_NET_CUSTOM_TEMPLATE_SL", None)
    tcpipHttpNetCustTemplateSl.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetCustTemplateSl.setVisible(False)
    tcpipHttpNetCustTemplateSl.setDescription("Include HTTP NET Custom Template SL")
    tcpipHttpNetCustTemplateSl.setDefaultValue((Database.getSymbolValue("sys_fs", "SYS_FS_MPFS") == True))
    tcpipHttpNetCustTemplateSl.setDependencies(tcpipHttpNetCustomSlSet, ["sys_fs.SYS_FS_MPFS"])

    # Persistent Connection Idle Time-out
    tcpipHttpNetConnTimeout = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_CONNECTION_TIMEOUT", tcpipHttpNetAdvSettings)
    tcpipHttpNetConnTimeout.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetConnTimeout.setLabel("Persistent Connection Idle Timeout (in sec)")
    tcpipHttpNetConnTimeout.setVisible(True)
    tcpipHttpNetConnTimeout.setDescription("Persistent Connection Idle Time-out")
    tcpipHttpNetConnTimeout.setDefaultValue(0)

    # HTTP NET allocation function, malloc style
    tcpipHttpNetMallocFunct = tcpipHttpNetComponent.createStringSymbol("TCPIP_HTTP_NET_MALLOC_FUNC", tcpipHttpNetAdvSettings)
    tcpipHttpNetMallocFunct.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetMallocFunct.setLabel("HTTP NET allocation function, malloc style")
    tcpipHttpNetMallocFunct.setVisible(True)
    tcpipHttpNetMallocFunct.setDescription("HTTP NET allocation function, malloc style")
    tcpipHttpNetMallocFunct.setDefaultValue("malloc")

    # HTTP NET deallocation function, free style
    tcpipHttpNetFreeFunct = tcpipHttpNetComponent.createStringSymbol("TCPIP_HTTP_NET_FREE_FUNC", tcpipHttpNetAdvSettings)
    tcpipHttpNetFreeFunct.setHelp("mcc_h3_httpnet_configurations")
    tcpipHttpNetFreeFunct.setLabel("HTTP NET deallocation function, free style")
    tcpipHttpNetFreeFunct.setVisible(True)
    tcpipHttpNetFreeFunct.setDescription("HTTP NET deallocation function, free style")
    tcpipHttpNetFreeFunct.setDefaultValue("free")


    tcpipHttpNetheapdependency = [  "TCPIP_HTTP_NET_MAX_CONNECTIONS", "TCPIP_HTTP_NET_MAX_DATA_LEN", 
                                    "TCPIP_HTTP_NET_CHUNKS_NUMBER", "TCPIP_HTTP_NET_DYNVAR_DESCRIPTORS_NUMBER", 
                                    "TCPIP_HTTP_NET_FILE_PROCESS_BUFFERS_NUMBER", "TCPIP_HTTP_NET_FILE_PROCESS_BUFFER_SIZE", 
                                    "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]    
        
    # HTTP NET Heap Size
    tcpipHttpNetHeapSize = tcpipHttpNetComponent.createIntegerSymbol("TCPIP_HTTP_NET_HEAP_SIZE", None)
    tcpipHttpNetHeapSize.setLabel("HTTP NET Heap Size (bytes)") 
    tcpipHttpNetHeapSize.setVisible(False)
    tcpipHttpNetHeapSize.setDefaultValue(tcpipHttpNetHeapCalc())
    tcpipHttpNetHeapSize.setReadOnly(True)
    tcpipHttpNetHeapSize.setDependencies(tcpipHttpNetHeapUpdate, tcpipHttpNetheapdependency)  

    #Add to system_config.h
    tcpipHttpNetHeaderFtl = tcpipHttpNetComponent.createFileSymbol(None, None)
    tcpipHttpNetHeaderFtl.setSourcePath("tcpip/config/http_net.h.ftl")
    tcpipHttpNetHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipHttpNetHeaderFtl.setMarkup(True)
    tcpipHttpNetHeaderFtl.setType("STRING")


    # Add http_net.c file
    tcpipHttpNetSourceFile = tcpipHttpNetComponent.createFileSymbol(None, None)
    tcpipHttpNetSourceFile.setSourcePath("tcpip/src/http_net.c")
    tcpipHttpNetSourceFile.setOutputName("http_net.c")
    tcpipHttpNetSourceFile.setOverwrite(True)
    tcpipHttpNetSourceFile.setDestPath("library/tcpip/src/")
    tcpipHttpNetSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipHttpNetSourceFile.setType("SOURCE")
    tcpipHttpNetSourceFile.setEnabled(True)
    tcpipHttpNetSourceFile.setDependencies(tcpipHttpNetGenSourceFile, ["TCPIP_STACK_USE_HTTP_NET_SERVER"])


    # ifblock TCPIP_HTTP_NET_CUSTOM_TEMPLATE

    # template HTTP_NET_PRINT_C "$HARMONY_VERSION_PATH/framework/tcpip/config/custom_app/http_net_print.c.ftl" to "$PROJECT_SOURCE_FILES/app/http_net_print.c"
    tcpipHttpNetPrintSourceFile = tcpipHttpNetComponent.createFileSymbol(None, None)
    tcpipHttpNetPrintSourceFile.setSourcePath("tcpip/config/custom_app/http_net_print.c.ftl")
    tcpipHttpNetPrintSourceFile.setOutputName("http_net_print.c")
    tcpipHttpNetPrintSourceFile.setDestPath("../../")
    tcpipHttpNetPrintSourceFile.setProjectPath("")
    tcpipHttpNetPrintSourceFile.setType("SOURCE")
    tcpipHttpNetPrintSourceFile.setMarkup(True)
    tcpipHttpNetPrintSourceFile.setDependencies(tcpipHttpNetGenSourceFile, ["TCPIP_HTTP_NET_CUSTOM_TEMPLATE"])
    
    # template HTTP_NET_PRINT_H "$HARMONY_VERSION_PATH/framework/tcpip/config/custom_app/http_net_print.h.ftl" to "$PROJECT_SOURCE_FILES/app/http_net_print.h"
    tcpipHttpNetPrintHeaderFile = tcpipHttpNetComponent.createFileSymbol(None, None)
    tcpipHttpNetPrintHeaderFile.setSourcePath("tcpip/config/custom_app/http_net_print.h.ftl")
    tcpipHttpNetPrintHeaderFile.setOutputName("http_net_print.h")
    tcpipHttpNetPrintHeaderFile.setDestPath("../../")
    tcpipHttpNetPrintHeaderFile.setProjectPath("")
    tcpipHttpNetPrintHeaderFile.setType("HEADER")
    tcpipHttpNetPrintHeaderFile.setMarkup(True)
    tcpipHttpNetPrintHeaderFile.setDependencies(tcpipHttpNetGenSourceFile, ["TCPIP_HTTP_NET_CUSTOM_TEMPLATE"])
    
    # template HTTP_NET_CUSTOM_APP_C "$HARMONY_VERSION_PATH/framework/tcpip/config/custom_app/custom_http_net_app.c.ftl" to "$PROJECT_SOURCE_FILES/app/custom_http_net_app.c"
    tcpipHttpNetCstmAppSourceFile = tcpipHttpNetComponent.createFileSymbol(None, None)
    tcpipHttpNetCstmAppSourceFile.setSourcePath("tcpip/config/custom_app/custom_http_net_app.c.ftl")
    tcpipHttpNetCstmAppSourceFile.setOutputName("custom_http_net_app.c")
    tcpipHttpNetCstmAppSourceFile.setDestPath("../../")
    tcpipHttpNetCstmAppSourceFile.setProjectPath("")
    tcpipHttpNetCstmAppSourceFile.setType("SOURCE")
    tcpipHttpNetCstmAppSourceFile.setMarkup(True)
    tcpipHttpNetCstmAppSourceFile.setDependencies(tcpipHttpNetGenSourceFile, ["TCPIP_HTTP_NET_CUSTOM_TEMPLATE"])
    
    # ifblock TCPIP_HTTP_NET_CUSTOM_TEMPLATE_SL
    # template HTTP_NET_MPFS_IMG "$HARMONY_VERSION_PATH/framework/tcpip/config/custom_app/mpfs_img2_net.c.ftl" to "$PROJECT_SOURCE_FILES/app/mpfs_img2_net.c"
    # endif
    tcpipHttpNetMpfsImg2SourceFile = tcpipHttpNetComponent.createFileSymbol(None, None)
    tcpipHttpNetMpfsImg2SourceFile.setSourcePath("tcpip/config/custom_app/mpfs_img2_net.c.ftl")
    tcpipHttpNetMpfsImg2SourceFile.setOutputName("mpfs_net_img.c")
    tcpipHttpNetMpfsImg2SourceFile.setDestPath("../../")
    tcpipHttpNetMpfsImg2SourceFile.setProjectPath("")
    tcpipHttpNetMpfsImg2SourceFile.setType("SOURCE")
    tcpipHttpNetMpfsImg2SourceFile.setMarkup(True)
    tcpipHttpNetMpfsImg2SourceFile.setEnabled(False)
    tcpipHttpNetMpfsImg2SourceFile.setDependencies(tcpipHttpNetGenSourceFile, ["TCPIP_HTTP_NET_CUSTOM_TEMPLATE_SL"])    

    print("Module.getPath(): ")
    print(Module.getPath())
    execfile(Module.getPath() + "/tcpip/config/webpage.py")

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
def tcpipHttpNetFileUploadVisible(symbol, event):
    if (event["value"] == True):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

# make Http Net Server Config Flag Secure options visible
def tcpipHttpNetSrvConfigFlagSecureOnEnable(tcpipDependentSymbol, tcpipIPSymbol):   
    #tcpipHttpNet = Database.getSymbolValue("tcpipHttpNet","TCPIP_STACK_USE_HTTP_NET_SERVER")
    tcpipHttpNetSrvConfigFlagSecureOff = Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_OFF")
    tcpipHttpNetSrvConfigFlagSecureDefault = Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_DEFAULT")

    #if(tcpipHttpNet):
    if(tcpipHttpNetSrvConfigFlagSecureOff or tcpipHttpNetSrvConfigFlagSecureDefault):
        tcpipDependentSymbol.setVisible(False)
        print("SecureOn disable")
    else:
        tcpipDependentSymbol.setVisible(True)
        print("SecureOn enable")
    # else:
        # tcpipDependentSymbol.setVisible(False)
        # print("SecureOn disable") 


def tcpipHttpNetSrvConfigFlagSecureOffEnable(tcpipDependentSymbol, tcpipIPSymbol):  
    #tcpipHttpNet = Database.getSymbolValue("tcpipHttpNet","TCPIP_STACK_USE_HTTP_NET_SERVER")
    tcpipHttpNetSrvConfigFlagSecureOn = Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_ON")
    tcpipHttpNetSrvConfigFlagSecureDefault = Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_DEFAULT")
    
    #if(tcpipHttpNet):
    if(tcpipHttpNetSrvConfigFlagSecureOn or tcpipHttpNetSrvConfigFlagSecureDefault):
        tcpipDependentSymbol.setVisible(False)
        print("SecureOff disable")
    else:
        tcpipDependentSymbol.setVisible(True)
        print("SecureOff enable")
    # else:
        # tcpipDependentSymbol.setVisible(False)
        # print("SecureOff disable")


def tcpipHttpNetSrvConfigFlagSecureDefaultEnable(tcpipDependentSymbol, tcpipIPSymbol):  
    #tcpipHttpNet = Database.getSymbolValue("tcpipHttpNet","TCPIP_STACK_USE_HTTP_NET_SERVER")
    tcpipHttpNetSrvConfigFlagSecureOn = Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_ON")
    tcpipHttpNetSrvConfigFlagSecureOff = Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_OFF")
    
    #if(tcpipHttpNet):      
    if(tcpipHttpNetSrvConfigFlagSecureOn or tcpipHttpNetSrvConfigFlagSecureOff):
        tcpipDependentSymbol.setVisible(False)
        print("Securedefault disable")
    else:
        tcpipDependentSymbol.setVisible(True)
        print("Securedefault enable")
    # else:
        # tcpipDependentSymbol.setVisible(False)
        # print("Securedefault disable")

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

# make Http Net Module custom http net Attribute options visible
def tcpipHttpNetWebServerPathVisible(tcpipDependentSymbol, tcpipIPSymbol):
    tcpipHttpNet = Database.getSymbolValue("tcpipHttpNet","TCPIP_STACK_USE_HTTP_NET_SERVER")
    tcpipHttpNetCustomHTTPNetProcess = Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_CUSTOM_TEMPLATE")

    if(tcpipHttpNet and tcpipHttpNetCustomHTTPNetProcess):
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

def tcpipHttpNetBase64DecodeOpt(symbol, event):
    symbol.clearValue()
    if (event["value"] == True):
        symbol.setValue(True,2)
    else:
        symbol.setValue(False,2)

def tcpipHttpNetGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])

def tcpipHttpNetCustomSlSet(symbol, event):
    symbol.clearValue()
    if (event["value"] == True):
        symbol.setValue(True,2)
    else:
        symbol.setValue(False,2)


def tcpipHttpNetHeapCalc(): 
    nConnections = Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_MAX_CONNECTIONS")
    dataLen = Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_MAX_DATA_LEN")
    nChunks = Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_CHUNKS_NUMBER")
    nDescriptors = Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_DYNVAR_DESCRIPTORS_NUMBER")
    nFileBuffers = Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_FILE_PROCESS_BUFFERS_NUMBER")
    fileBufferSize = Database.getSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_FILE_PROCESS_BUFFER_SIZE")
    
    heap_size = (nConnections * (352 + dataLen)) + (nChunks * 80) + (nDescriptors * 16) + (nFileBuffers * (fileBufferSize + 32)) 
    return heap_size    
    
def tcpipHttpNetHeapUpdate(symbol, event): 
    heap_size = tcpipHttpNetHeapCalc()
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
    Database.setSymbolValue("tcpipHttpNet", "TCPIP_STACK_USE_HTTP_NET_SERVER", False, 2)
