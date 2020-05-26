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
    tcpipFtpsModule.setLabel("Use FTP Module")
    tcpipFtpsModule.setVisible(False)
    tcpipFtpsModule.setDescription("Use FTP Module")
    tcpipFtpsModule.setDefaultValue(True)
    #tcpipFtpsModule.setDependencies(tcpipFtpsModuleMenuVisible, ["tcpipIPv4.TCPIP_STACK_USE_IPV4", "tcpipTcp.TCPIP_USE_TCP"])

    # FTPs Command Listening Port
    tcpipFtpsCmdPort = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTPS_COMMAND_LISTEN_PORT", None)
    tcpipFtpsCmdPort.setLabel("FTP Server Command Listening Port")
    tcpipFtpsCmdPort.setVisible(True)
    tcpipFtpsCmdPort.setDescription("FTP Server Command Listening Port")
    tcpipFtpsCmdPort.setDefaultValue(21)
    #tcpipFtpsCmdPort.setDependencies(tcpipFtpsModuleMenuVisible, ["TCPIP_USE_FTP_MODULE"])

    # FTPs Data Listening Port
    tcpipFtpsDataPort = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTPS_DATA_LISTEN_PORT", None)
    tcpipFtpsDataPort.setLabel("FTP Data Listening Port")
    tcpipFtpsDataPort.setVisible(True)
    tcpipFtpsDataPort.setDescription("FTP Server Data Listening Port")
    tcpipFtpsDataPort.setDefaultValue(20)
    #tcpipFtpsDataPort.setDependencies(tcpipFtpsModuleMenuVisible, ["TCPIP_USE_FTP_MODULE"])

    # Maximum Length for User Name
    tcpipFtpsUsrNameMaxLen = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTP_USER_NAME_LEN", None)
    tcpipFtpsUsrNameMaxLen.setLabel("Maximum Length for User Name")
    tcpipFtpsUsrNameMaxLen.setVisible(True)
    tcpipFtpsUsrNameMaxLen.setDescription("Maximum Length for User Name")
    tcpipFtpsUsrNameMaxLen.setDefaultValue(10)
    #tcpipFtpsUsrNameMaxLen.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])

    # Maximum Number of FTP Connections Allowed per Interface
    tcpipFtpsConnMaxNum = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTP_MAX_CONNECTIONS", None)
    tcpipFtpsConnMaxNum.setLabel("Maximum Number of FTP Connections Allowed per Interface")
    tcpipFtpsConnMaxNum.setVisible(True)
    tcpipFtpsConnMaxNum.setDescription("Maximum Number of FTP Connections Allowed per Interface")
    tcpipFtpsConnMaxNum.setDefaultValue(1)
    #tcpipFtpsConnMaxNum.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])

    # Transmit Buffer Size for the FTP Data Socket
    tcpipFtpsDataSktTxBuffSize = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTP_DATA_SKT_TX_BUFF_SIZE", None)
    tcpipFtpsDataSktTxBuffSize.setLabel("TX Buffer for the FTP Data Socket")
    tcpipFtpsDataSktTxBuffSize.setVisible(True)
    tcpipFtpsDataSktTxBuffSize.setDescription("Transmit Buffer Size for the FTP Data Socket")
    tcpipFtpsDataSktTxBuffSize.setDefaultValue(0)
    #tcpipFtpsDataSktTxBuffSize.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])

    # Receive Buffer Size for the FTP Data Socket
    tcpipFtpsDataSktRxBuffSize = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTP_DATA_SKT_RX_BUFF_SIZE", None)
    tcpipFtpsDataSktRxBuffSize.setLabel("RX Buffer for the FTP Data Socket")
    tcpipFtpsDataSktRxBuffSize.setVisible(True)
    tcpipFtpsDataSktRxBuffSize.setDescription("Receive Buffer Size for the FTP Data Socket")
    tcpipFtpsDataSktRxBuffSize.setDefaultValue(0)
    #tcpipFtpsDataSktRxBuffSize.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])

    # FTP Server Task Rate in msec
    tcpipFtpsTskTickRate = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTPS_TASK_TICK_RATE", None)
    tcpipFtpsTskTickRate.setLabel("FTP Server Task Rate in msec")
    tcpipFtpsTskTickRate.setVisible(True)
    tcpipFtpsTskTickRate.setDescription("FTP Server Task Rate in msec")
    tcpipFtpsTskTickRate.setDefaultValue(33)
    #tcpipFtpsTskTickRate.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])

    # FTP Server timeout in seconds
    tcpipFtpsTimeout = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTP_TIMEOUT", None)
    tcpipFtpsTimeout.setLabel("FTP Server timeout in sec")
    tcpipFtpsTimeout.setVisible(True)
    tcpipFtpsTimeout.setDescription("FTP Server timeout in seconds")
    tcpipFtpsTimeout.setDefaultValue(180)
    #tcpipFtpsTimeout.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])

    # FTP Authentication Selection
    tcpipFtpsAuth= tcpipFtpsComponent.createComboSymbol("TCPIP_FTPS_AUTH_CONFIG", None, TCPIP_FTPS_AUTH_TYPES)
    tcpipFtpsAuth.setLabel("Select FTP Authentication")
    tcpipFtpsAuth.setVisible(True)
    tcpipFtpsAuth.setDescription("FTP Authentication Selection")
    tcpipFtpsAuth.setDefaultValue("Run Time Authentication")

    # FTP Connection info Authentication
    tcpipFtpsAuthInfo = tcpipFtpsComponent.createBooleanSymbol("TCPIP_FTPS_AUTHENTICATION_CONN_INFO", tcpipFtpsAuth)
    tcpipFtpsAuthInfo.setLabel("Connection Info Used for Authentication ")
    tcpipFtpsAuthInfo.setVisible(True)
    tcpipFtpsAuthInfo.setDescription("Connection Info is Passed to the Authentication Handler")
    tcpipFtpsAuthInfo.setDefaultValue(True)
    tcpipFtpsAuthInfo.setDependencies(tcpipFtpsAuthInfoVisible, ["TCPIP_FTPS_AUTH_CONFIG"])


    # FTP Login User Name
    tcpipFtpsLoginUsrName = tcpipFtpsComponent.createStringSymbol("TCPIP_FTP_USER_NAME", tcpipFtpsAuth)
    tcpipFtpsLoginUsrName.setLabel("FTP Login User Name")
    tcpipFtpsLoginUsrName.setVisible(True)
    tcpipFtpsLoginUsrName.setDescription("FTP Login User Name")
    tcpipFtpsLoginUsrName.setDefaultValue("Microchip")
    tcpipFtpsLoginUsrName.setDependencies(tcpipFtpsObsAuthVisible, ["TCPIP_FTPS_AUTH_CONFIG"])

    # FTP Login Password
    tcpipFtpsLoginUsrName = tcpipFtpsComponent.createStringSymbol("TCPIP_FTP_PASSWORD", tcpipFtpsAuth)
    tcpipFtpsLoginUsrName.setLabel("FTP Login Password")
    tcpipFtpsLoginUsrName.setVisible(True)
    tcpipFtpsLoginUsrName.setDescription("FTP Login Password")
    tcpipFtpsLoginUsrName.setDefaultValue("Harmony")
    tcpipFtpsLoginUsrName.setDependencies(tcpipFtpsObsAuthVisible, ["TCPIP_FTPS_AUTH_CONFIG"])

    # Maximum Length of FTP Login Password
    tcpipFtpsLoginPswdMaxLen = tcpipFtpsComponent.createIntegerSymbol("TCPIP_FTP_PASSWD_LEN", tcpipFtpsAuth)
    tcpipFtpsLoginPswdMaxLen.setLabel("Maximum Length of FTP Login Password")
    tcpipFtpsLoginPswdMaxLen.setVisible(True)
    tcpipFtpsLoginPswdMaxLen.setDescription("Maximum Length of FTP Login Password")
    tcpipFtpsLoginPswdMaxLen.setDefaultValue(10)
    tcpipFtpsLoginPswdMaxLen.setDependencies(tcpipFtpsObsAuthVisible, ["TCPIP_FTPS_AUTH_CONFIG"])

    # Enable FTP File PUT Command
    tcpipFtpsFilePut = tcpipFtpsComponent.createBooleanSymbol("TCPIP_FTP_PUT_ENABLED", None)
    tcpipFtpsFilePut.setLabel("FTP File PUT Command")
    tcpipFtpsFilePut.setVisible(True)
    tcpipFtpsFilePut.setDescription("Enable FTP File PUT Command")
    tcpipFtpsFilePut.setDefaultValue(True)
    #tcpipFtpsFilePut.setDependencies(tcpipFtpsMenuVisible, ["TCPIP_USE_FTP_MODULE"])    
    
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
    #tcpipFtpsSourceFile.setDependencies(tcpipFtpsGenSourceFile, ["TCPIP_USE_FTP_MODULE"])

    # Default FTP Mount point directory
    tcpipFtpRootDir = tcpipFtpsComponent.createStringSymbol("TCPIP_FTP_MOUNT_POINT", None)
    tcpipFtpRootDir.setLabel("FTP Server Root Directory Path")
    tcpipFtpRootDir.setVisible(True)
    tcpipFtpRootDir.setDescription("FTP Server Root Directory Path")
    tcpipFtpRootDir.setDefaultValue("/mnt/mchpSite1/")

    
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
    
