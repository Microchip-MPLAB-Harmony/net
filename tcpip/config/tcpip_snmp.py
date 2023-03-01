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

TCPIP_SNMP_VERSIONS = ["SNMP_V1", "SNMP_V2C", "SNMP_V3"]
tcpipSnmpMaxCommNum = 3
tcpipSnmpConfigNumPrev = 3
tcpipSnmpStackConfigIdx = []
tcpipSnmpReadCommName = []
tcpipSnmpWriteCommName = []
def instantiateComponent(tcpipSnmpComponent):
    print("TCPIP SNMP V1 Component")
    configName = Variables.get("__CONFIGURATION_NAME")      


    # Use SNMP
    tcpipSnmp = tcpipSnmpComponent.createBooleanSymbol("TCPIP_USE_SNMP", None)
    tcpipSnmp.setHelp("mcc_h3_snmp_configurations")
    tcpipSnmp.setLabel("Use SNMP")
    tcpipSnmp.setVisible(False)
    tcpipSnmp.setDescription("Use SNMP")
    tcpipSnmp.setDefaultValue(True)        
    
    # Default SNMP Mount point directory
    tcpipSnmpRootDir = tcpipSnmpComponent.createStringSymbol("TCPIP_SNMP_MOUNT_POINT", None)
    tcpipSnmpRootDir.setHelp("mcc_h3_snmp_configurations")
    tcpipSnmpRootDir.setLabel("SNMP Server Root Directory Path")
    tcpipSnmpRootDir.setVisible(True)
    tcpipSnmpRootDir.setDescription("SNMP Server Root Directory Path")
    tcpipSnmpRootDir.setDefaultValue("/mnt/mchpSite1/")
    
    # BIB file name
    tcpipSnmpBibFileName = tcpipSnmpComponent.createStringSymbol("TCPIP_SNMP_BIB_FILE_NAME", None)
    tcpipSnmpBibFileName.setHelp("mcc_h3_snmp_configurations")
    tcpipSnmpBibFileName.setLabel("BIB file name")
    tcpipSnmpBibFileName.setVisible(True)
    tcpipSnmpBibFileName.setDescription("BIB file name")
    tcpipSnmpBibFileName.setDefaultValue("snmp.bib")

    # Maximum SNMP message size
    tcpipSnmpMsgSizeMax = tcpipSnmpComponent.createIntegerSymbol("TCPIP_SNMP_MAX_MSG_SIZE", None)
    tcpipSnmpMsgSizeMax.setHelp("mcc_h3_snmp_configurations")
    tcpipSnmpMsgSizeMax.setLabel("Maximum SNMP message size")
    tcpipSnmpMsgSizeMax.setVisible(True)
    tcpipSnmpMsgSizeMax.setDescription("Maximum SNMP message size")
    tcpipSnmpMsgSizeMax.setDefaultValue(480)

    # Maximum Community Length
    tcpipSnmpCommunityLenMax = tcpipSnmpComponent.createIntegerSymbol("TCPIP_SNMP_COMMUNITY_MAX_LEN", None)
    tcpipSnmpCommunityLenMax.setHelp("mcc_h3_snmp_configurations")
    tcpipSnmpCommunityLenMax.setLabel("Maximum Community Length")
    tcpipSnmpCommunityLenMax.setVisible(True)
    tcpipSnmpCommunityLenMax.setDescription("Maximum Community Length")
    tcpipSnmpCommunityLenMax.setDefaultValue(16)
    
    # Maximum Community Support
    tcpipSnmpCommunitySuppMax = tcpipSnmpComponent.createIntegerSymbol("TCPIP_SNMP_MAX_COMMUNITY_SUPPORT", None)
    tcpipSnmpCommunitySuppMax.setHelp("mcc_h3_snmp_configurations")
    tcpipSnmpCommunitySuppMax.setLabel("Maximum Community Entries")
    tcpipSnmpCommunitySuppMax.setMax(tcpipSnmpMaxCommNum)
    tcpipSnmpCommunitySuppMax.setMin(0)
    tcpipSnmpCommunitySuppMax.setVisible(True)
    tcpipSnmpCommunitySuppMax.setDescription("Maximum Community Support")
    tcpipSnmpCommunitySuppMax.setDefaultValue(3)
    
####-----------------------------------------------------------------------------------------##########
    for index in range(0,tcpipSnmpMaxCommNum):  
        tcpipSnmpStackConfigIdx.append(tcpipSnmpComponent.createBooleanSymbol("TCPIP_SNMP_STACK_CONFIG_IDX"+str(index),tcpipSnmpCommunitySuppMax))
        tcpipSnmpStackConfigIdx[index].setHelp("mcc_h3_snmp_configurations")
        tcpipSnmpStackConfigIdx[index].setLabel("Community Entry "+ str(index))
        tcpipSnmpStackConfigIdx[index].setVisible(True)
        if (index < tcpipSnmpCommunitySuppMax.getValue()):  
            tcpipSnmpStackConfigIdx[index].setDefaultValue(True)
        else:           
            tcpipSnmpStackConfigIdx[index].setDefaultValue(False)
            
        tcpipSnmpStackConfigIdx[index].setDependencies(tcpipSnmpEnableConfig, ["TCPIP_USE_SNMP","TCPIP_SNMP_MAX_COMMUNITY_SUPPORT"])
        
        # Snmp Read Community Name
        tcpipSnmpReadCommName.append(tcpipSnmpComponent.createStringSymbol("TCPIP_SNMP_STACK_READCOMMUNITY_NAME_IDX" + str(index), tcpipSnmpStackConfigIdx[index]))
        tcpipSnmpReadCommName[index].setHelp("mcc_h3_snmp_configurations")
        tcpipSnmpReadCommName[index].setLabel("Read Community Name")
        tcpipSnmpReadCommName[index].setVisible(True)
        if(index == 0):
            tcpipSnmpReadCommName[index].setDefaultValue("harmony")
        else:
            if(index == 1):
                tcpipSnmpReadCommName[index].setDefaultValue("read")
            else:
                if(index == 2):
                    tcpipSnmpReadCommName[index].setDefaultValue("")
                    
        tcpipSnmpReadCommName[index].setDependencies(tcpipSnmpMenuVisibleSingle, [tcpipSnmpStackConfigIdx[index].getID()])
        
        # Snmp Write Community Name
        tcpipSnmpWriteCommName.append(tcpipSnmpComponent.createStringSymbol("TCPIP_SNMP_STACK_WRITECOMMUNITY_NAME_IDX" + str(index), tcpipSnmpStackConfigIdx[index]))
        tcpipSnmpWriteCommName[index].setHelp("mcc_h3_snmp_configurations")
        tcpipSnmpWriteCommName[index].setLabel("Write Community Name")
        tcpipSnmpWriteCommName[index].setVisible(True)
        if(index == 0):
            tcpipSnmpWriteCommName[index].setDefaultValue("unicorn")
        else:
            if(index == 1):
                tcpipSnmpWriteCommName[index].setDefaultValue("write")
            else:
                if(index == 2):
                    tcpipSnmpWriteCommName[index].setDefaultValue("harmony")
                    
        tcpipSnmpWriteCommName[index].setDependencies(tcpipSnmpMenuVisibleSingle, [tcpipSnmpStackConfigIdx[index].getID()])
####-----------------------------------------------------------------------------------------##########    

    # Trap Support
    tcpipSnmpTrapSupport = tcpipSnmpComponent.createBooleanSymbol("TCPIP_SNMP_USE_TRAP_SUPPORT", None)
    tcpipSnmpTrapSupport.setHelp("mcc_h3_snmp_configurations")
    tcpipSnmpTrapSupport.setLabel("Trap Support")
    tcpipSnmpTrapSupport.setVisible(True)
    tcpipSnmpTrapSupport.setDescription("Trap Support")
    tcpipSnmpTrapSupport.setDefaultValue(True) 

    # Trap table size
    tcpipSnmpTrapTblSize = tcpipSnmpComponent.createIntegerSymbol("TCPIP_SNMP_TRAP_TABLE_SIZE", tcpipSnmpTrapSupport)
    tcpipSnmpTrapTblSize.setHelp("mcc_h3_snmp_configurations")
    tcpipSnmpTrapTblSize.setLabel("Trap table size")
    tcpipSnmpTrapTblSize.setVisible(True)
    tcpipSnmpTrapTblSize.setDescription("Trap table size")
    tcpipSnmpTrapTblSize.setDefaultValue(2)
    
    # V2 Trap Support
    tcpipSnmpV2TrapSupport = tcpipSnmpComponent.createBooleanSymbol("TCPIP_SNMP_STACK_USE_V2_TRAP", tcpipSnmpTrapSupport)
    tcpipSnmpV2TrapSupport.setHelp("mcc_h3_snmp_configurations")
    tcpipSnmpV2TrapSupport.setLabel("V2 Trap Support")
    tcpipSnmpV2TrapSupport.setVisible(True)
    tcpipSnmpV2TrapSupport.setDescription("V2 Trap Support")
    tcpipSnmpV2TrapSupport.setDefaultValue(True) 
    tcpipSnmpV2TrapSupport.setDependencies(tcpipSnmpMenuVisibleSingle, ["TCPIP_SNMP_USE_TRAP_SUPPORT"])
    
    # Include SNMP Custom Template
    tcpipSnmpCustTemplate = tcpipSnmpComponent.createBooleanSymbol("TCPIP_SNMP_CUSTOM_TEMPLATE", None)
    tcpipSnmpCustTemplate.setHelp("mcc_h3_snmp_configurations")
    tcpipSnmpCustTemplate.setLabel("Include SNMP Custom Template")
    tcpipSnmpCustTemplate.setVisible(True)
    tcpipSnmpCustTemplate.setDescription("Include SNMP Custom Template")
    tcpipSnmpCustTemplate.setDefaultValue(True) 

    tcpipSnmpCustTemplateSl = tcpipSnmpComponent.createBooleanSymbol("TCPIP_SNMP_CUSTOM_TEMPLATE_SL", None)
    tcpipSnmpCustTemplateSl.setVisible(False)   
    tcpipSnmpCustTemplateSl.setDefaultValue((Database.getSymbolValue("sys_fs", "SYS_FS_MPFS") == True))
    tcpipSnmpCustTemplateSl.setDependencies(tcpipSnmpCustomSlSet, ["sys_fs.SYS_FS_MPFS"])
    
    # Advanced Settings
    tcpipSnmpAdvSettings = tcpipSnmpComponent.createMenuSymbol("TCPIP_SNMP_ADV_SETTING", None)
    tcpipSnmpAdvSettings.setLabel("Advanced Settings")
    tcpipSnmpAdvSettings.setDescription("Advanced Settings")
    tcpipSnmpAdvSettings.setVisible(True)
    
    # SNMP process rate
    tcpipSnmpTskProcRate = tcpipSnmpComponent.createIntegerSymbol("TCPIP_SNMP_TASK_PROCESS_RATE", tcpipSnmpAdvSettings)
    tcpipSnmpTskProcRate.setHelp("mcc_h3_snmp_configurations")
    tcpipSnmpTskProcRate.setLabel("SNMP Task Rate (in msec)")
    tcpipSnmpTskProcRate.setVisible(True)
    tcpipSnmpTskProcRate.setDescription("SNMP process rate")
    tcpipSnmpTskProcRate.setDefaultValue(200)

    # Maximum OID Length
    tcpipSnmpOldLenMax = tcpipSnmpComponent.createIntegerSymbol("TCPIP_SNMP_OID_MAX_LEN", tcpipSnmpAdvSettings)
    tcpipSnmpOldLenMax.setHelp("mcc_h3_snmp_configurations")
    tcpipSnmpOldLenMax.setLabel("Maximum OID Length")
    tcpipSnmpOldLenMax.setVisible(True)
    tcpipSnmpOldLenMax.setDescription("Maximum OID Length")
    tcpipSnmpOldLenMax.setDefaultValue(18)

    # Maximum non record OID ID
    tcpipSnmpNonRecOldIdMax = tcpipSnmpComponent.createIntegerSymbol("TCPIP_SNMP_MAX_NON_REC_ID_OID", tcpipSnmpAdvSettings)
    tcpipSnmpNonRecOldIdMax.setHelp("mcc_h3_snmp_configurations")
    tcpipSnmpNonRecOldIdMax.setLabel("Maximum non record OID")
    tcpipSnmpNonRecOldIdMax.setVisible(True)
    tcpipSnmpNonRecOldIdMax.setDescription("Maximum non record OID ID")
    tcpipSnmpNonRecOldIdMax.setDefaultValue(3)

    
    tcpipSnmpCommMaxNum= tcpipSnmpComponent.createIntegerSymbol("TCPIP_SNMP_COMMUNITY_MAX_NUM", tcpipSnmpAdvSettings)
    tcpipSnmpCommMaxNum.setHelp("mcc_h3_snmp_configurations")
    tcpipSnmpCommMaxNum.setDefaultValue(tcpipSnmpMaxCommNum)
    tcpipSnmpCommMaxNum.setVisible(False)


    # Notify Community Length
    tcpipSnmpNotifyCommunLen = tcpipSnmpComponent.createIntegerSymbol("TCPIP_SNMP_NOTIFY_COMMUNITY_LEN", tcpipSnmpAdvSettings)
    tcpipSnmpNotifyCommunLen.setHelp("mcc_h3_snmp_configurations")
    tcpipSnmpNotifyCommunLen.setLabel("Notify Community Length")
    tcpipSnmpNotifyCommunLen.setVisible(True)
    tcpipSnmpNotifyCommunLen.setDescription("Notify Community Length")
    tcpipSnmpNotifyCommunLen.setDefaultValue(tcpipSnmpCommunityLenMax.getValue())

    # Trap community max length
    tcpipSnmpTrapCommunMemUseLenMax = tcpipSnmpComponent.createIntegerSymbol("TCPIP_SNMP_TRAP_COMMUNITY_MAX_LEN_MEM_USE", tcpipSnmpAdvSettings)
    tcpipSnmpTrapCommunMemUseLenMax.setHelp("mcc_h3_snmp_configurations")
    tcpipSnmpTrapCommunMemUseLenMax.setLabel("Trap community maximum length")
    tcpipSnmpTrapCommunMemUseLenMax.setVisible(True)
    tcpipSnmpTrapCommunMemUseLenMax.setDescription("Trap community max length")
    tcpipSnmpTrapCommunMemUseLenMax.setDefaultValue(16)

    tcpipSnmpheapdependency = ["tcpipUdp.TCPIP_UDP_SOCKET_DEFAULT_TX_SIZE", "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"] 
    
    # SNMP Heap Size
    tcpipSnmpHeapSize = tcpipSnmpComponent.createIntegerSymbol("TCPIP_SNMP_HEAP_SIZE", None)
    tcpipSnmpHeapSize.setLabel("SNMP Heap Size (bytes)") 
    tcpipSnmpHeapSize.setVisible(False)
    tcpipSnmpHeapSize.setDefaultValue(tcpipSnmpHeapCalc())
    tcpipSnmpHeapSize.setReadOnly(True)
    tcpipSnmpHeapSize.setDependencies(tcpipSnmpHeapUpdate, tcpipSnmpheapdependency)  
    
    # Add snmp.c file
    tcpipSnmpSourceFile = tcpipSnmpComponent.createFileSymbol(None, None)
    tcpipSnmpSourceFile.setSourcePath("tcpip/src/snmp.c")
    tcpipSnmpSourceFile.setOutputName("snmp.c")
    tcpipSnmpSourceFile.setOverwrite(True)
    tcpipSnmpSourceFile.setDestPath("library/tcpip/src/")
    tcpipSnmpSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipSnmpSourceFile.setType("SOURCE")
    tcpipSnmpSourceFile.setEnabled(True)

    #Add to system_config.h
    tcpipSnmpHeaderFtl = tcpipSnmpComponent.createFileSymbol(None, None)
    tcpipSnmpHeaderFtl.setSourcePath("tcpip/config/snmp.h.ftl")
    tcpipSnmpHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipSnmpHeaderFtl.setMarkup(True)
    tcpipSnmpHeaderFtl.setType("STRING")

# ifblock TCPIP_SNMP_CUSTOM_TEMPLATE
# template SNMP_CUSTOM_APP_C "$HARMONY_VERSION_PATH/framework/tcpip/config/custom_app/custom_snmp_app.c.ftl" to "$PROJECT_SOURCE_FILES/app/custom_snmp_app.c"
    tcpipSnmpCstmAppSourceFile = tcpipSnmpComponent.createFileSymbol(None, None)
    tcpipSnmpCstmAppSourceFile.setSourcePath("tcpip/config/custom_app/custom_snmp_app.c.ftl")
    tcpipSnmpCstmAppSourceFile.setOutputName("custom_snmp_app.c")
    tcpipSnmpCstmAppSourceFile.setDestPath("../../")
    tcpipSnmpCstmAppSourceFile.setProjectPath("")
    tcpipSnmpCstmAppSourceFile.setType("SOURCE")
    tcpipSnmpCstmAppSourceFile.setMarkup(True)
    tcpipSnmpCstmAppSourceFile.setDependencies(tcpipSnmpGenSourceFile, ["TCPIP_SNMP_CUSTOM_TEMPLATE"])
    
# template SNMP_MIB_H "$HARMONY_VERSION_PATH/framework/tcpip/config/custom_app/mib.h.ftl" to "$PROJECT_HEADER_FILES/app/mib.h"
    tcpipSnmpMibHeaderFile = tcpipSnmpComponent.createFileSymbol(None, None)
    tcpipSnmpMibHeaderFile.setSourcePath("tcpip/config/custom_app/mib.h.ftl")
    tcpipSnmpMibHeaderFile.setOutputName("mib.h")
    tcpipSnmpMibHeaderFile.setMarkup(True)
    tcpipSnmpMibHeaderFile.setDestPath("../../")
    tcpipSnmpMibHeaderFile.setProjectPath("")
    tcpipSnmpMibHeaderFile.setType("HEADER")
    tcpipSnmpMibHeaderFile.setOverwrite(True)
    tcpipSnmpMibHeaderFile.setDependencies(tcpipSnmpGenSourceFile, ["TCPIP_SNMP_CUSTOM_TEMPLATE"])
# ifblock (USE_SYS_FS) && (SYS_FS_MPFS) && (!TCPIP_STACK_USE_HTTP_SERVER)
# template SNMP_MPFS_IMG "$HARMONY_VERSION_PATH/framework/tcpip/config/custom_app/mpfs_img2_net.c.ftl" to "$PROJECT_SOURCE_FILES/app/mpfs_net_img.c"
# endif
    tcpipSnmpMpfsImg2SourceFile = tcpipSnmpComponent.createFileSymbol(None, None)
    tcpipSnmpMpfsImg2SourceFile.setSourcePath("tcpip/config/custom_app/mpfs_img2_net.c.ftl")
    tcpipSnmpMpfsImg2SourceFile.setOutputName("mpfs_net_img.c")
    tcpipSnmpMpfsImg2SourceFile.setDestPath("../../")
    tcpipSnmpMpfsImg2SourceFile.setProjectPath("")
    tcpipSnmpMpfsImg2SourceFile.setType("SOURCE")
    tcpipSnmpMpfsImg2SourceFile.setMarkup(True)
    tcpipSnmpMpfsImg2SourceFile.setEnabled(False)
    tcpipSnmpMpfsImg2SourceFile.setDependencies(tcpipSnmpGenSourceFile, ["TCPIP_SNMP_CUSTOM_TEMPLATE_SL"])


def tcpipSnmpHeapCalc():     
    min_SNMP_tx_size = 480
    udpsktTxBuffSize = Database.getSymbolValue("tcpipUdp","TCPIP_UDP_SOCKET_DEFAULT_TX_SIZE")
    if(udpsktTxBuffSize >= min_SNMP_tx_size):
        min_SNMP_tx_size = 0
        
    heap_size = 120 + min_SNMP_tx_size        
    return heap_size    
    
def tcpipSnmpHeapUpdate(symbol, event): 
    heap_size = tcpipSnmpHeapCalc()
    symbol.setValue(heap_size)
    if(event["id"] == "TCPIP_STACK_HEAP_CALC_MASK"):
        symbol.setVisible(event["value"])
        
def tcpipSnmpMenuVisibleSingle(symbol, event):
    if (event["value"] == True):
        print("SNMP Menu Visible.")     
        symbol.setVisible(True)
    else:
        print("SNMP Menu Invisible.")
        symbol.setVisible(False)


def tcpipSnmpCustomSlSet(symbol, event):
    symbol.clearValue()
    if (event["value"] == True):
        symbol.setValue(True,2)
    else:
        symbol.setValue(False,2)
        
def tcpipSnmpGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])

def tcpipSnmpEnableConfig(symbol, event):
    global tcpipSnmpConfigNumPrev
    print("Start tcpipSnmpEnableConfig")
    if(event["id"] == "TCPIP_USE_SNMP" ):
        tcpipSnmpEnable = Database.getSymbolValue("tcpipSnmp","TCPIP_USE_SNMP")
        tcpipSnmpIndex = int(symbol.getID().strip("TCPIP_SNMP_STACK_CONFIG_IDX"))
        print("Snmp Index: " + str(tcpipSnmpIndex) )
        print(tcpipSnmpConfigNumPrev)
        if(tcpipSnmpEnable == True):
            if(tcpipSnmpIndex < tcpipSnmpConfigNumPrev ):
                symbol.setVisible(True)
        else:
            symbol.setVisible(False)
        
    else:       
        print(symbol.getID())
        print(event["id"])
        tcpipSnmpConfigNumberValue = event["value"]
        print(tcpipSnmpConfigNumberValue)
        print(tcpipSnmpConfigNumPrev)
        if(tcpipSnmpConfigNumberValue > tcpipSnmpConfigNumPrev ):
            tcpipSnmpStackConfigIdx[tcpipSnmpConfigNumPrev].setVisible(True)
            tcpipSnmpStackConfigIdx[tcpipSnmpConfigNumPrev].setValue(True, 1)
            print("Set TRUE"+ str(tcpipSnmpConfigNumPrev))
            tcpipSnmpConfigNumPrev = tcpipSnmpConfigNumPrev + 1
            #Add more network configurations
        else:
            if(tcpipSnmpConfigNumberValue < tcpipSnmpConfigNumPrev ):
                #Reduce network configurations
                tcpipSnmpConfigNumPrev = tcpipSnmpConfigNumPrev - 1
                tcpipSnmpStackConfigIdx[tcpipSnmpConfigNumPrev].setVisible(False)
                tcpipSnmpStackConfigIdx[tcpipSnmpConfigNumPrev].setValue(False, 1)
                print("Set FALSE"+ str(tcpipSnmpConfigNumPrev))
                
            #else:
                #Do Nothing
            
    print("END tcpipSnmpEnableConfig")
    

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
    Database.setSymbolValue("tcpipSnmp", "TCPIP_USE_SNMP", False, 2)
    
