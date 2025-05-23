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

def instantiateComponent(tcpipTftpsComponent):
    print("TCPIP TFTPS Component")
    configName = Variables.get("__CONFIGURATION_NAME")

    # Use TFTP Server Module
    tcpipTftps = tcpipTftpsComponent.createBooleanSymbol("TCPIP_USE_TFTPS_MODULE", None)
    tcpipTftps.setHelp("mcc_h3_tftps_configurations")
    tcpipTftps.setLabel("Use TFTP Server Module")
    tcpipTftps.setVisible(False)
    tcpipTftps.setDescription("Use TFTP Server Module")
    tcpipTftps.setDefaultValue(True) 
    
    # TFTP Server number of client can be supported at a time
    tcpipTftpsClientCnt= tcpipTftpsComponent.createIntegerSymbol("TCPIP_TFTPS_CLIENT_NUMBER", None)
    tcpipTftpsClientCnt.setHelp("mcc_h3_tftps_configurations")
    tcpipTftpsClientCnt.setLabel("Maximum number of Clients")
    tcpipTftpsClientCnt.setVisible(True)
    tcpipTftpsClientCnt.setDescription("TFTP Server can access number of Clients")
    tcpipTftpsClientCnt.setDefaultValue(3)    

    # TFTP Server command source file name length
    tcpipTftpsFileNameLength= tcpipTftpsComponent.createIntegerSymbol("TCPIP_TFTPS_FILENAME_LEN", None)
    tcpipTftpsFileNameLength.setHelp("mcc_h3_tftps_configurations")
    tcpipTftpsFileNameLength.setLabel("Default file name length")
    tcpipTftpsFileNameLength.setVisible(True)
    tcpipTftpsFileNameLength.setDescription("TFTP Server default file name length")
    tcpipTftpsFileNameLength.setDefaultValue(64)
    
    # Default TFTP Server Mount point directory
    tcpipTftpsRootDir = tcpipTftpsComponent.createStringSymbol("TCPIP_TFTPS_MOUNT_POINT", None)
    tcpipTftpsRootDir.setHelp("mcc_h3_tftps_configurations")
    tcpipTftpsRootDir.setLabel("Root Directory Path")
    tcpipTftpsRootDir.setVisible(True)
    tcpipTftpsRootDir.setDescription("TFTP Server Root Directory Path")
    tcpipTftpsRootDir.setDefaultValue("/mnt/mchpSite1/")

    # Enable TFTP Server Console Command
    tcpipTftpsCmdEnable = tcpipTftpsComponent.createBooleanSymbol("TCPIP_TFTPS_CONSOLE_CMD", None)
    tcpipTftpsCmdEnable.setHelp("mcc_h3_tftps_configurations")
    tcpipTftpsCmdEnable.setLabel("Enable Console Commands")
    tcpipTftpsCmdEnable.setVisible(True)
    tcpipTftpsCmdEnable.setDescription("Enable TFTP Server Console Commands")
    tcpipTftpsCmdEnable.setDefaultValue(False)
            
    # Advanced Settings
    tcpipTftpsAdvSettings = tcpipTftpsComponent.createMenuSymbol("TCPIP_TFTPS_ADV_SETTING", None)
    tcpipTftpsAdvSettings.setLabel("Advanced Settings")
    tcpipTftpsAdvSettings.setDescription("Advanced Settings")
    tcpipTftpsAdvSettings.setVisible(True)

    # TFTP Server Task Rate in msec
    tcpipTftpsTskTickRate= tcpipTftpsComponent.createIntegerSymbol("TCPIP_TFTPS_TASK_TICK_RATE", tcpipTftpsAdvSettings)
    tcpipTftpsTskTickRate.setHelp("mcc_h3_tftps_configurations")
    tcpipTftpsTskTickRate.setLabel("TFTP Server Task Rate (in msec)")
    tcpipTftpsTskTickRate.setVisible(True)
    tcpipTftpsTskTickRate.setDescription("TFTP Server Task Rate in msec")
    tcpipTftpsTskTickRate.setDefaultValue(60)
    
    # TFTP Server Retransmit count value to retransmit the last packet sent
    tcpipTftpsRetransmitCnt= tcpipTftpsComponent.createIntegerSymbol("TCPIP_TFTPS_RETRANSMIT_COUNT", tcpipTftpsAdvSettings)
    tcpipTftpsRetransmitCnt.setHelp("mcc_h3_tftps_configurations")
    tcpipTftpsRetransmitCnt.setLabel("Maximum Number of Retransmissions")
    tcpipTftpsRetransmitCnt.setVisible(True)
    tcpipTftpsRetransmitCnt.setDescription("TFTP Server retransmit count")
    tcpipTftpsRetransmitCnt.setDefaultValue(3)
    
    # Maximum Length for a file size
    tcpipTftpsFileSizeLen= tcpipTftpsComponent.createIntegerSymbol("TCPIP_TFTPS_DEFAULT_FILESIZE", tcpipTftpsAdvSettings)
    tcpipTftpsFileSizeLen.setHelp("mcc_h3_tftps_configurations")
    tcpipTftpsFileSizeLen.setLabel("Maximum File Size (bytes)")
    tcpipTftpsFileSizeLen.setVisible(True)
    tcpipTftpsFileSizeLen.setDescription("Maximum Length for a file transmission")
    tcpipTftpsFileSizeLen.setDefaultValue(64000)
        
    # Maximum Length for block size configuration
    tcpipTftpsDefBockSizeLen= tcpipTftpsComponent.createIntegerSymbol("TCPIP_TFTPS_DEFAULT_BLOCK_SIZE", tcpipTftpsAdvSettings)
    tcpipTftpsDefBockSizeLen.setHelp("mcc_h3_tftps_configurations")
    tcpipTftpsDefBockSizeLen.setLabel("Maximum Block Size (bytes)")
    tcpipTftpsDefBockSizeLen.setVisible(True)
    tcpipTftpsDefBockSizeLen.setDescription("Maximum block size Length for a file name")
    tcpipTftpsDefBockSizeLen.setDefaultValue(512)
    
    # TFTP Server default timeout in sec
    tcpipTftpsDefTimeout= tcpipTftpsComponent.createIntegerSymbol("TCPIP_TFTPS_DEFAULT_TIMEOUT", tcpipTftpsAdvSettings)
    tcpipTftpsDefTimeout.setHelp("mcc_h3_tftps_configurations")
    tcpipTftpsDefTimeout.setLabel("Default Retransmission timeout (in sec)")
    tcpipTftpsDefTimeout.setVisible(True)
    tcpipTftpsDefTimeout.setDescription("TFTP Server default timeout option")
    tcpipTftpsDefTimeout.setDefaultValue(10)
    
    # TFTP Server command process timeout in sec
    tcpipTftpsCmdProcessTimeout= tcpipTftpsComponent.createIntegerSymbol("TCPIP_TFTPS_CMD_PROCESS_TIMEOUT", tcpipTftpsAdvSettings)
    tcpipTftpsCmdProcessTimeout.setHelp("mcc_h3_tftps_configurations")
    tcpipTftpsCmdProcessTimeout.setLabel("Command process timeout (in sec)")
    tcpipTftpsCmdProcessTimeout.setVisible(True)
    tcpipTftpsCmdProcessTimeout.setDescription("TFTP Server command process timeout in sec")
    tcpipTftpsCmdProcessTimeout.setDefaultValue(10)
    
    # Enable User Notification
    tcpipTftpsUsrNotify = tcpipTftpsComponent.createBooleanSymbol("TCPIP_TFTPS_USER_NOTIFICATION", tcpipTftpsAdvSettings)
    tcpipTftpsUsrNotify.setHelp("mcc_h3_tftps_configurations")
    tcpipTftpsUsrNotify.setLabel("Enable User Notification")
    tcpipTftpsUsrNotify.setVisible(True)
    tcpipTftpsUsrNotify.setDescription("Enable User Notification")
    tcpipTftpsUsrNotify.setDefaultValue(False) 

    #Add to system_config.h
    tcpipTftpsHeaderFtl = tcpipTftpsComponent.createFileSymbol(None, None)
    tcpipTftpsHeaderFtl.setSourcePath("tcpip/config/tftps.h.ftl")
    tcpipTftpsHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipTftpsHeaderFtl.setMarkup(True)
    tcpipTftpsHeaderFtl.setType("STRING")

    # Add tftps.c file
    tcpipTftpsSourceFile = tcpipTftpsComponent.createFileSymbol(None, None)
    tcpipTftpsSourceFile.setSourcePath("tcpip/src/tftps.c")
    tcpipTftpsSourceFile.setOutputName("tftps.c")
    tcpipTftpsSourceFile.setOverwrite(True)
    tcpipTftpsSourceFile.setDestPath("library/tcpip/src/")
    tcpipTftpsSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipTftpsSourceFile.setType("SOURCE")
    tcpipTftpsSourceFile.setEnabled(True)

# make Reboot Server option visible
def tcpipTftpsModuleVisible(tcpipDependentSymbol, tcpipIPSymbol):
    tcpipIPv4 = Database.getSymbolValue("tcpipIPv4","TCPIP_STACK_USE_IPV4")
    tcpipUdp = Database.getSymbolValue("tcpipUdp","TCPIP_USE_UDP")

    if(tcpipIPv4 and tcpipUdp):
        tcpipDependentSymbol.setVisible(True)
    else:
        tcpipDependentSymbol.setVisible(False)
        
def tcpipTftpsMenuVisibleSingle(symbol, event):
    if (event["value"] == True):
        print("TFTPS Menu Visible.")        
        symbol.setVisible(True)
    else:
        print("TFTPS Menu Invisible.")
        symbol.setVisible(False)

def tcpipTftpsGenSourceFile(sourceFile, event):
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
    Database.setSymbolValue("tcpipTftps", "TCPIP_USE_TFTPS_MODULE", False, 2)
    
