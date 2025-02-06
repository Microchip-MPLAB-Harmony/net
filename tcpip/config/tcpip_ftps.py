"""
Copyright (C) 2019-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

TCPIP_FTPS_AUTH_TYPES = ["Run Time Authentication", "(OBSOLETE) Init Time Authentication"]

def tcpipFtpsAuthInfoVisible(symbol, event):
    if (event["value"] == "Run Time Authentication"):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def tcpipFtpsObsAuthVisible(symbol, event):
    if (event["value"] == "Run Time Authentication"):
        symbol.setVisible(False)
    else:
        symbol.setVisible(True)

    
def instantiateComponent(tcpipFtpsComponent):
    print("TCPIP FTP Server Component")
    configName = Variables.get("__CONFIGURATION_NAME")
        
    # Use FTP Module
    tcpipFtpsModule = tcpipFtpsComponent.createBooleanSymbol("TCPIP_USE_FTP_MODULE", None)
    tcpipFtpsModule.setHelp("mcc_h3_ftps_configurations")
    tcpipFtpsModule.setLabel("Use FTP Module")
    tcpipFtpsModule.setVisible(False)
    tcpipFtpsModule.setDescription("Use FTP Module")
    tcpipFtpsModule.setDefaultValue(True)

    # Maximum Number of FTP Connections Allowed per Interface
    tcpipFtpsConnMaxNum = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTP_MAX_CONNECTIONS", None)
    tcpipFtpsConnMaxNum.setHelp("mcc_h3_ftps_configurations")
    tcpipFtpsConnMaxNum.setLabel("Maximum Number of FTP Connections Allowed per Interface")
    tcpipFtpsConnMaxNum.setVisible(True)
    tcpipFtpsConnMaxNum.setDescription("Maximum Number of FTP Connections Allowed per Interface")
    tcpipFtpsConnMaxNum.setDefaultValue(1)

    # Maximum Length for User Name
    tcpipFtpsUsrNameMaxLen = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTP_USER_NAME_LEN", None)
    tcpipFtpsUsrNameMaxLen.setHelp("mcc_h3_ftps_configurations")
    tcpipFtpsUsrNameMaxLen.setLabel("Maximum Length for User Name")
    tcpipFtpsUsrNameMaxLen.setVisible(True)
    tcpipFtpsUsrNameMaxLen.setDescription("Maximum Length for User Name")
    tcpipFtpsUsrNameMaxLen.setDefaultValue(10)

    # Transmit Buffer Size for the FTP Data Socket
    tcpipFtpsDataSktTxBuffSize = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTP_DATA_SKT_TX_BUFF_SIZE", None)
    tcpipFtpsDataSktTxBuffSize.setHelp("mcc_h3_ftps_configurations")
    tcpipFtpsDataSktTxBuffSize.setLabel("FTP Data Socket TX Buffer Size")
    tcpipFtpsDataSktTxBuffSize.setVisible(True)
    tcpipFtpsDataSktTxBuffSize.setDescription("Transmit Buffer Size for the FTP Data Socket")
    tcpipFtpsDataSktTxBuffSize.setDefaultValue(0)

    # Receive Buffer Size for the FTP Data Socket
    tcpipFtpsDataSktRxBuffSize = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTP_DATA_SKT_RX_BUFF_SIZE", None)
    tcpipFtpsDataSktRxBuffSize.setHelp("mcc_h3_ftps_configurations")
    tcpipFtpsDataSktRxBuffSize.setLabel("FTP Data Socket RX Buffer Size")
    tcpipFtpsDataSktRxBuffSize.setVisible(True)
    tcpipFtpsDataSktRxBuffSize.setDescription("Receive Buffer Size for the FTP Data Socket")
    tcpipFtpsDataSktRxBuffSize.setDefaultValue(0)

    # Default FTP Mount point directory
    tcpipFtpRootDir = tcpipFtpsComponent.createStringSymbol("TCPIP_FTP_MOUNT_POINT", None)
    tcpipFtpRootDir.setHelp("mcc_h3_ftps_configurations")
    tcpipFtpRootDir.setLabel("FTP Server Root Directory Path")
    tcpipFtpRootDir.setVisible(True)
    tcpipFtpRootDir.setDescription("FTP Server Root Directory Path")
    tcpipFtpRootDir.setDefaultValue("/mnt/mchpSite1/")
    
    # Advanced Settings
    tcpipFtpsAdvSettings = tcpipFtpsComponent.createMenuSymbol("TCPIP_FTPS_ADV_SETTING", None)
    tcpipFtpsAdvSettings.setLabel("Advanced Settings")
    tcpipFtpsAdvSettings.setDescription("Advanced Settings")
    tcpipFtpsAdvSettings.setVisible(True)

    # FTP Server Task Rate in msec
    tcpipFtpsTskTickRate = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTPS_TASK_TICK_RATE", tcpipFtpsAdvSettings)
    tcpipFtpsTskTickRate.setHelp("mcc_h3_ftps_configurations")
    tcpipFtpsTskTickRate.setLabel("FTP Server Task Rate (in msec)")
    tcpipFtpsTskTickRate.setVisible(True)
    tcpipFtpsTskTickRate.setDescription("FTP Server Task Rate in msec")
    tcpipFtpsTskTickRate.setDefaultValue(33)

    # FTPs Command Listening Port
    tcpipFtpsCmdPort = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTPS_COMMAND_LISTEN_PORT", tcpipFtpsAdvSettings)
    tcpipFtpsCmdPort.setHelp("mcc_h3_ftps_configurations")
    tcpipFtpsCmdPort.setLabel("Command Listening Port")
    tcpipFtpsCmdPort.setVisible(True)
    tcpipFtpsCmdPort.setDescription("FTP Server Command Listening Port")
    tcpipFtpsCmdPort.setDefaultValue(21)

    # FTPs Data Listening Port
    tcpipFtpsDataPort = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTPS_DATA_LISTEN_PORT", tcpipFtpsAdvSettings)
    tcpipFtpsDataPort.setHelp("mcc_h3_ftps_configurations")
    tcpipFtpsDataPort.setLabel("Data Listening Port")
    tcpipFtpsDataPort.setVisible(True)
    tcpipFtpsDataPort.setDescription("FTP Server Data Listening Port")
    tcpipFtpsDataPort.setDefaultValue(20)
    
    # FTP Server timeout in seconds
    tcpipFtpsTimeout = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTP_TIMEOUT", tcpipFtpsAdvSettings)
    tcpipFtpsTimeout.setHelp("mcc_h3_ftps_configurations")
    tcpipFtpsTimeout.setLabel("FTP Server timeout (in sec)")
    tcpipFtpsTimeout.setVisible(True)
    tcpipFtpsTimeout.setDescription("FTP Server timeout in seconds")
    tcpipFtpsTimeout.setDefaultValue(180)

    # Enable FTP File PUT Command
    tcpipFtpsFilePut = tcpipFtpsComponent.createBooleanSymbol("TCPIP_FTP_PUT_ENABLED", tcpipFtpsAdvSettings)
    tcpipFtpsFilePut.setHelp("mcc_h3_ftps_configurations")
    tcpipFtpsFilePut.setLabel("FTP File PUT Command")
    tcpipFtpsFilePut.setVisible(True)
    tcpipFtpsFilePut.setDescription("Enable FTP File PUT Command")
    tcpipFtpsFilePut.setDefaultValue(True) 
    
    # FTP Authentication Selection
    tcpipFtpsAuth= tcpipFtpsComponent.createComboSymbol("TCPIP_FTPS_AUTH_CONFIG", tcpipFtpsAdvSettings, TCPIP_FTPS_AUTH_TYPES)
    tcpipFtpsAuth.setHelp("mcc_h3_ftps_configurations")
    tcpipFtpsAuth.setLabel("Select FTP Authentication")
    tcpipFtpsAuth.setVisible(True)
    tcpipFtpsAuth.setDescription("FTP Authentication Selection")
    tcpipFtpsAuth.setDefaultValue("Run Time Authentication")

    # FTP Connection info Authentication
    tcpipFtpsAuthInfo = tcpipFtpsComponent.createBooleanSymbol("TCPIP_FTPS_AUTHENTICATION_CONN_INFO", tcpipFtpsAuth)
    tcpipFtpsAuthInfo.setHelp("mcc_h3_ftps_configurations")
    tcpipFtpsAuthInfo.setLabel("Connection Info Used for Authentication ")
    tcpipFtpsAuthInfo.setVisible(True)
    tcpipFtpsAuthInfo.setDescription("Connection Info is Passed to the Authentication Handler")
    tcpipFtpsAuthInfo.setDefaultValue(True)
    tcpipFtpsAuthInfo.setDependencies(tcpipFtpsAuthInfoVisible, ["TCPIP_FTPS_AUTH_CONFIG"])
    
    # FTP Login User Name
    tcpipFtpsLoginUsrName = tcpipFtpsComponent.createStringSymbol("TCPIP_FTP_USER_NAME", tcpipFtpsAuth)
    tcpipFtpsLoginUsrName.setHelp("mcc_h3_ftps_configurations")
    tcpipFtpsLoginUsrName.setLabel("FTP Login User Name")
    tcpipFtpsLoginUsrName.setVisible(False)
    tcpipFtpsLoginUsrName.setDescription("FTP Login User Name")
    tcpipFtpsLoginUsrName.setDefaultValue("Microchip")
    tcpipFtpsLoginUsrName.setDependencies(tcpipFtpsObsAuthVisible, ["TCPIP_FTPS_AUTH_CONFIG"])

    # FTP Login Password
    tcpipFtpsLoginUsrName = tcpipFtpsComponent.createStringSymbol("TCPIP_FTP_PASSWORD", tcpipFtpsAuth)
    tcpipFtpsLoginUsrName.setHelp("mcc_h3_ftps_configurations")
    tcpipFtpsLoginUsrName.setLabel("FTP Login Password")
    tcpipFtpsLoginUsrName.setVisible(False)
    tcpipFtpsLoginUsrName.setDescription("FTP Login Password")
    tcpipFtpsLoginUsrName.setDefaultValue("Harmony")
    tcpipFtpsLoginUsrName.setDependencies(tcpipFtpsObsAuthVisible, ["TCPIP_FTPS_AUTH_CONFIG"])

    # Maximum Length of FTP Login Password
    tcpipFtpsLoginPswdMaxLen = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTP_PASSWD_LEN", tcpipFtpsAuth)
    tcpipFtpsLoginPswdMaxLen.setHelp("mcc_h3_ftps_configurations")
    tcpipFtpsLoginPswdMaxLen.setLabel("Maximum Length of FTP Login Password")
    tcpipFtpsLoginPswdMaxLen.setVisible(False)
    tcpipFtpsLoginPswdMaxLen.setDescription("Maximum Length of FTP Login Password")
    tcpipFtpsLoginPswdMaxLen.setDefaultValue(10)
    tcpipFtpsLoginPswdMaxLen.setDependencies(tcpipFtpsObsAuthVisible, ["TCPIP_FTPS_AUTH_CONFIG"])
    
    tcpipFtpsheapdependency = ["tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]  
        
    # FTPS Heap Size
    tcpipFtpsHeapSize = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTPS_HEAP_SIZE", None)
    tcpipFtpsHeapSize.setLabel("FTP Server Heap Size (bytes)")  
    tcpipFtpsHeapSize.setVisible(False)
    tcpipFtpsHeapSize.setDefaultValue(tcpipFtpsHeapCalc())
    tcpipFtpsHeapSize.setReadOnly(True)
    tcpipFtpsHeapSize.setDependencies(tcpipFtpsHeapUpdate, tcpipFtpsheapdependency)   
    
    #Add to system_config.h
    tcpipFtpsHeaderFtl = tcpipFtpsComponent.createFileSymbol(None, None)
    tcpipFtpsHeaderFtl.setSourcePath("tcpip/config/ftp.h.ftl")
    tcpipFtpsHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipFtpsHeaderFtl.setMarkup(True)
    tcpipFtpsHeaderFtl.setType("STRING")    

    # Add ftp.c file
    tcpipFtpsSourceFile = tcpipFtpsComponent.createFileSymbol(None, None)
    tcpipFtpsSourceFile.setSourcePath("tcpip/src/ftp.c")
    tcpipFtpsSourceFile.setOutputName("ftp.c")
    tcpipFtpsSourceFile.setOverwrite(True)
    tcpipFtpsSourceFile.setDestPath("library/tcpip/src/")
    tcpipFtpsSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipFtpsSourceFile.setType("SOURCE")
    tcpipFtpsSourceFile.setEnabled(True)

    
def tcpipFtpsMenuVisible(symbol, event):
    if (event["value"] == True):
        print("FTP server Menu Visible.")       
        symbol.setVisible(True)
    else:
        print("FTP Server Menu Invisible.")
        symbol.setVisible(False)    
        
# make FTP Module option visible
def tcpipFtpsModuleMenuVisible(tcpipDependentSymbol, tcpipIPSymbol):
    tcpipIPv4 = Database.getSymbolValue("tcpipIPv4","TCPIP_STACK_USE_IPV4")
    tcpipTcp = Database.getSymbolValue("tcpipTcp","TCPIP_USE_TCP")

    if(tcpipIPv4 and tcpipTcp):
        tcpipDependentSymbol.setVisible(True)
    else:
        tcpipDependentSymbol.setVisible(False)

def tcpipFtpsHeapCalc(): 
    #heap allocation added in TCP module; hence 0     
    heap_size = 0
    return heap_size    
    
def tcpipFtpsHeapUpdate(symbol, event): 
    heap_size = tcpipFtpsHeapCalc()
    symbol.setValue(heap_size)
    if(event["id"] == "TCPIP_STACK_HEAP_CALC_MASK"):
        symbol.setVisible(event["value"])
    
def tcpipFtpsGenSourceFile(sourceFile, event):
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
    Database.setSymbolValue("tcpipFtps", "TCPIP_USE_FTP_MODULE", False, 2)
    
