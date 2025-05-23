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

TCPIP_SNMPV3_SECURITY_LEVEL = ["NO_AUTH_NO_PRIV", "AUTH_NO_PRIV", "AUTH_PRIV"]
TCPIP_SNMPV3_PRIVACY_PROTOCOL_TYPE = ["SNMPV3_DES_PRIV", "SNMPV3_AES_PRIV", "SNMPV3_NO_PRIV"]
TCPIP_SNMPV3_AUTH_PROTOCOL_TYPE = ["SNMPV3_HMAC_MD5", "SNMPV3_HMAC_SHA1", "SNMPV3_NO_HMAC_AUTH"]
TCPIP_SNMPV3_MESSAGE_PROCESSING_MODEL_TYPE = ["SNMPV1_MSG_PROCESSING_MODEL", "SNMPV2C_MSG_PROCESSING_MODEL", "SNMPV2U_V2_MSG_PROCESSING_MODEL", "SNMPV3_MSG_PROCESSING_MODEL"]
TCPIP_SNMPV3_SECURITY_MODEL_TYPE = ["ANY_SECUTIRY_MODEL", "SNMPV1_SECURITY_MODEL", "SNMPV2C_SECURITY_MODEL", "SNMPV3_USM_SECURITY_MODEL"]

tcpipSnmpV3MaxUsrNum = 3
tcpipSnmpV3ConfigNumPrev = 3
tcpipSnmpV3StackConfigIdx = []
tcpipSnmpv3UsrName = []
tcpipSnmpv3SecLvl = []
tcpipSnmpv3AuthProtocol = []
tcpipSnmpv3AuthProtocolPswd = []
tcpipSnmpv3PrivProtocol = []
tcpipSnmpv3PrivProtocolPswd = []
tcpipSnmpv3TrgtEntryMsgProtType =[]
tcpipSnmpv3TrgtEntrySecModelType = []
tcpipSnmpv3TrgtEntrySecName = []
tcpipSnmpv3TrgtEntrySecLvl = []

def instantiateComponent(tcpipSnmpv3Component):
    print("TCPIP SNMP V3 Component")
    configName = Variables.get("__CONFIGURATION_NAME")              
    # Use SNMP Version 3
    tcpipSnmpV3 = tcpipSnmpv3Component.createBooleanSymbol("TCPIP_USE_SNMPv3", None)
    tcpipSnmpV3.setHelp("mcc_h3_snmpv3_configurations")
    tcpipSnmpV3.setLabel("Use SNMP Version 3")
    tcpipSnmpV3.setVisible(False)
    tcpipSnmpV3.setDescription("Use SNMP Version 3")
    tcpipSnmpV3.setDefaultValue(True) 

    # Maximum Users
    tcpipSnmpV3UsmUsrMax = tcpipSnmpv3Component.createIntegerSymbol("TCPIP_SNMPV3_USM_MAX_USER", None)
    tcpipSnmpV3UsmUsrMax.setHelp("mcc_h3_snmpv3_configurations")
    tcpipSnmpV3UsmUsrMax.setLabel("Maximum Security Entries")
    tcpipSnmpV3UsmUsrMax.setVisible(True)
    tcpipSnmpV3UsmUsrMax.setDescription("Maximum Security Entries")
    tcpipSnmpV3UsmUsrMax.setMax(tcpipSnmpV3MaxUsrNum)
    tcpipSnmpV3UsmUsrMax.setMin(0)
    tcpipSnmpV3UsmUsrMax.setDefaultValue(3)

    ####-----------------------------------------------------------------------------------------##########
    for index in range(0,tcpipSnmpV3MaxUsrNum): 
        tcpipSnmpV3StackConfigIdx.append(tcpipSnmpv3Component.createBooleanSymbol("TCPIP_SNMPV3_STACK_CONFIG_IDX"+str(index),tcpipSnmpV3UsmUsrMax))
        tcpipSnmpV3StackConfigIdx[index].setHelp("mcc_h3_snmpv3_configurations")
        tcpipSnmpV3StackConfigIdx[index].setLabel("Security Entry "+ str(index))
        tcpipSnmpV3StackConfigIdx[index].setVisible(True)
        if (index < tcpipSnmpV3UsmUsrMax.getValue()):               
            tcpipSnmpV3StackConfigIdx[index].setDefaultValue(True)
        else:           
            tcpipSnmpV3StackConfigIdx[index].setDefaultValue(False)         
        tcpipSnmpV3StackConfigIdx[index].setDependencies(tcpipSnmpv3EnableConfig, ["TCPIP_USE_SNMPv3","TCPIP_SNMPV3_USM_MAX_USER"]) 
        
        # Snmpv3 User Name
        tcpipSnmpv3UsrName.append(tcpipSnmpv3Component.createStringSymbol("TCPIP_SNMPV3_STACK_USM_NAME_IDX" + str(index), tcpipSnmpV3StackConfigIdx[index]))
        tcpipSnmpv3UsrName[index].setHelp("mcc_h3_snmpv3_configurations")
        tcpipSnmpv3UsrName[index].setLabel("User Name")
        tcpipSnmpv3UsrName[index].setVisible(True)
        if(index == 0):
            tcpipSnmpv3UsrName[index].setDefaultValue("microchip")
        else:
            if(index == 1):
                tcpipSnmpv3UsrName[index].setDefaultValue("SnmpAdmin")
            else:
                if(index == 2):
                    tcpipSnmpv3UsrName[index].setDefaultValue("root")                   
        tcpipSnmpv3UsrName[index].setDependencies(tcpipSnmpv3MenuVisibleSingle, [tcpipSnmpV3StackConfigIdx[index].getID()])
        
        # Snmpv3 Security Level
        tcpipSnmpv3SecLvl.append(tcpipSnmpv3Component.createComboSymbol("TCPIP_SNMPV3_STACK_SECURITY_LEVEL_IDX" + str(index), tcpipSnmpV3StackConfigIdx[index],TCPIP_SNMPV3_SECURITY_LEVEL))
        tcpipSnmpv3SecLvl[index].setHelp("mcc_h3_snmpv3_configurations")
        tcpipSnmpv3SecLvl[index].setLabel("Security Level")
        tcpipSnmpv3SecLvl[index].setVisible(True)
        if(index == 0):
            tcpipSnmpv3SecLvl[index].setDefaultValue("AUTH_PRIV")
        else:
            if(index == 1):
                tcpipSnmpv3SecLvl[index].setDefaultValue("AUTH_NO_PRIV")
            else:
                if(index == 2):
                    tcpipSnmpv3SecLvl[index].setDefaultValue("NO_AUTH_NO_PRIV")                 
        tcpipSnmpv3SecLvl[index].setDependencies(tcpipSnmpv3MenuVisibleSingle, [tcpipSnmpV3StackConfigIdx[index].getID()])  
        
        # Snmpv3 Authentication Protocol
        tcpipSnmpv3AuthProtocol.append(tcpipSnmpv3Component.createComboSymbol("TCPIP_SNMPV3_STACK_AUTH_PROTOCOL_IDX" + str(index), tcpipSnmpV3StackConfigIdx[index],TCPIP_SNMPV3_AUTH_PROTOCOL_TYPE))
        tcpipSnmpv3AuthProtocol[index].setHelp("mcc_h3_snmpv3_configurations")
        tcpipSnmpv3AuthProtocol[index].setLabel("Authentication Protocol")
        tcpipSnmpv3AuthProtocol[index].setVisible(True)
        if(index == 0):
            tcpipSnmpv3AuthProtocol[index].setDefaultValue("SNMPV3_HMAC_MD5")
        else:
            if(index == 1):
                tcpipSnmpv3AuthProtocol[index].setDefaultValue("SNMPV3_HMAC_SHA1")
            else:
                if(index == 2):
                    tcpipSnmpv3AuthProtocol[index].setDefaultValue("SNMPV3_NO_HMAC_AUTH")                   
        tcpipSnmpv3AuthProtocol[index].setDependencies(tcpipSnmpv3MenuVisibleSingle, [tcpipSnmpV3StackConfigIdx[index].getID()])    

        # Snmpv3 Authentication Protocol Password
        tcpipSnmpv3AuthProtocolPswd.append(tcpipSnmpv3Component.createStringSymbol("TCPIP_SNMPV3_STACK_AUTH_PASSWORD_IDX" + str(index), tcpipSnmpV3StackConfigIdx[index]))
        tcpipSnmpv3AuthProtocolPswd[index].setHelp("mcc_h3_snmpv3_configurations")
        tcpipSnmpv3AuthProtocolPswd[index].setLabel("Authentication Protocol Password")
        tcpipSnmpv3AuthProtocolPswd[index].setVisible(True)
        if(index == 0):
            tcpipSnmpv3AuthProtocolPswd[index].setDefaultValue("auth12345")
        else:
            if(index == 1):
                tcpipSnmpv3AuthProtocolPswd[index].setDefaultValue("ChandlerUS")
            else:
                if(index == 2):
                    tcpipSnmpv3AuthProtocolPswd[index].setDefaultValue("")                  
        tcpipSnmpv3AuthProtocolPswd[index].setDependencies(tcpipSnmpv3MenuVisibleSingle, [tcpipSnmpV3StackConfigIdx[index].getID()])    

        
        # Snmpv3 Privacy Protocol
        tcpipSnmpv3PrivProtocol.append(tcpipSnmpv3Component.createComboSymbol("TCPIP_SNMPV3_STACK_PRIV_PROTOCOL_IDX" + str(index), tcpipSnmpV3StackConfigIdx[index],TCPIP_SNMPV3_PRIVACY_PROTOCOL_TYPE))
        tcpipSnmpv3PrivProtocol[index].setHelp("mcc_h3_snmpv3_configurations")
        tcpipSnmpv3PrivProtocol[index].setLabel("Privacy Protocol")
        tcpipSnmpv3PrivProtocol[index].setVisible(True)
        if(index == 0):
            tcpipSnmpv3PrivProtocol[index].setDefaultValue("SNMPV3_AES_PRIV")
        else:
            if(index == 1):
                tcpipSnmpv3PrivProtocol[index].setDefaultValue("SNMPV3_NO_PRIV")
            else:
                if(index == 2):
                    tcpipSnmpv3PrivProtocol[index].setDefaultValue("SNMPV3_NO_PRIV")                    
        tcpipSnmpv3PrivProtocol[index].setDependencies(tcpipSnmpv3MenuVisibleSingle, [tcpipSnmpV3StackConfigIdx[index].getID()])    

        # Snmpv3 Privacy Protocol Password
        tcpipSnmpv3PrivProtocolPswd.append(tcpipSnmpv3Component.createStringSymbol("TCPIP_SNMPV3_STACK_PRIV_PASSWORD_IDX" + str(index), tcpipSnmpV3StackConfigIdx[index]))
        tcpipSnmpv3PrivProtocolPswd[index].setHelp("mcc_h3_snmpv3_configurations")
        tcpipSnmpv3PrivProtocolPswd[index].setLabel("Privacy Protocol Password")
        tcpipSnmpv3PrivProtocolPswd[index].setVisible(True)
        if(index == 0):
            tcpipSnmpv3PrivProtocolPswd[index].setDefaultValue("priv12345")
        else:
            if(index == 1):
                tcpipSnmpv3PrivProtocolPswd[index].setDefaultValue("")
            else:
                if(index == 2):
                    tcpipSnmpv3PrivProtocolPswd[index].setDefaultValue("")                  
        tcpipSnmpv3PrivProtocolPswd[index].setDependencies(tcpipSnmpv3MenuVisibleSingle, [tcpipSnmpV3StackConfigIdx[index].getID()])

        # Snmpv3 Target Entry Security Name     
        tcpipSnmpv3TrgtEntrySecName.append(tcpipSnmpv3Component.createStringSymbol("TCPIP_SNMPV3_TARGET_ENTRY_SEC_NAME_IDX" + str(index), tcpipSnmpV3StackConfigIdx[index]))
        tcpipSnmpv3TrgtEntrySecName[index].setHelp("mcc_h3_snmpv3_configurations")
        tcpipSnmpv3TrgtEntrySecName[index].setLabel("Target Entry Security Name")
        tcpipSnmpv3TrgtEntrySecName[index].setVisible(False)
        
        # Snmpv3 Target Entry Message Protocol Type
        tcpipSnmpv3TrgtEntryMsgProtType.append(tcpipSnmpv3Component.createComboSymbol("TCPIP_SNMPV3_TARGET_ENTRY_MESSAGE_PROTOCOL_TYPE_IDX" + str(index), tcpipSnmpV3StackConfigIdx[index],TCPIP_SNMPV3_MESSAGE_PROCESSING_MODEL_TYPE))
        tcpipSnmpv3TrgtEntryMsgProtType[index].setHelp("mcc_h3_snmpv3_configurations")
        tcpipSnmpv3TrgtEntryMsgProtType[index].setLabel("Target Entry Message Protocol Type")
        tcpipSnmpv3TrgtEntryMsgProtType[index].setVisible(True)
        tcpipSnmpv3TrgtEntryMsgProtType[index].setDefaultValue("SNMPV3_MSG_PROCESSING_MODEL")
        tcpipSnmpv3TrgtEntryMsgProtType[index].setDependencies(tcpipSnmpv3MenuVisibleSingle, [tcpipSnmpV3StackConfigIdx[index].getID()])
        
        # Snmpv3 Target Entry Security Model Type
        tcpipSnmpv3TrgtEntrySecModelType.append(tcpipSnmpv3Component.createComboSymbol("TCPIP_SNMPV3_TARGET_ENTRY_SEC_MODEL_TYPE_IDX" + str(index), tcpipSnmpV3StackConfigIdx[index],TCPIP_SNMPV3_SECURITY_MODEL_TYPE))
        tcpipSnmpv3TrgtEntrySecModelType[index].setHelp("mcc_h3_snmpv3_configurations")
        tcpipSnmpv3TrgtEntrySecModelType[index].setLabel("Target Entry Security Model Type")
        tcpipSnmpv3TrgtEntrySecModelType[index].setVisible(True)
        tcpipSnmpv3TrgtEntrySecModelType[index].setDefaultValue("SNMPV3_USM_SECURITY_MODEL")
        tcpipSnmpv3TrgtEntrySecModelType[index].setDependencies(tcpipSnmpv3MenuVisibleSingle, [tcpipSnmpV3StackConfigIdx[index].getID()])

        # Snmpv3 Target Entry Security Level    
        tcpipSnmpv3TrgtEntrySecLvl.append(tcpipSnmpv3Component.createStringSymbol("TCPIP_SNMPV3_TARGET_ENTRY_SEC_LEVEL_IDX" + str(index), tcpipSnmpV3StackConfigIdx[index]))
        tcpipSnmpv3TrgtEntrySecLvl[index].setLabel("Target Entry Security Level")
        tcpipSnmpv3TrgtEntrySecLvl[index].setVisible(False)
    ####-----------------------------------------------------------------------------------------##########
    
    # V1 and V2 Trap Support
    tcpipSnmpV3StackV1V2Trap = tcpipSnmpv3Component.createBooleanSymbol("TCPIP_SNMPV3_STACK_USE_V1_V2_TRAP", None)
    tcpipSnmpV3StackV1V2Trap.setHelp("mcc_h3_snmpv3_configurations")
    tcpipSnmpV3StackV1V2Trap.setLabel("V1 and V2 Trap Support")
    tcpipSnmpV3StackV1V2Trap.setVisible(True)
    tcpipSnmpV3StackV1V2Trap.setDescription("V1 and V2 Trap Support")
    tcpipSnmpV3StackV1V2Trap.setDefaultValue(True) 
    tcpipSnmpV3StackV1V2Trap.setDependencies(tcpipSnmpV3StackConfigV1V2TrapVisible, ["TCPIP_USE_SNMPv3" , "tcpipSnmp.TCPIP_SNMP_USE_TRAP_SUPPORT", "tcpipSnmp.TCPIP_SNMP_STACK_USE_V2_TRAP"])
    
    # Advanced Settings
    tcpipSnmpV3AdvSettings = tcpipSnmpv3Component.createMenuSymbol("TCPIP_SNMPV3_ADV_SETTING", None)
    tcpipSnmpV3AdvSettings.setLabel("Advanced Settings")
    tcpipSnmpV3AdvSettings.setDescription("Advanced Settings")
    tcpipSnmpV3AdvSettings.setVisible(True)
    
    # Security User Name Length
    tcpipSnmpV3UsrSecureNameLen = tcpipSnmpv3Component.createIntegerSymbol("TCPIP_SNMPV3_USER_SECURITY_NAME_LEN", tcpipSnmpV3AdvSettings)
    tcpipSnmpV3UsrSecureNameLen.setHelp("mcc_h3_snmpv3_configurations")
    tcpipSnmpV3UsrSecureNameLen.setLabel("User Security Name Length")
    tcpipSnmpV3UsrSecureNameLen.setVisible(True)
    tcpipSnmpV3UsrSecureNameLen.setDescription("User Security Name Length")
    tcpipSnmpV3UsrSecureNameLen.setDefaultValue(16)

    # Localized Password Key Length
    tcpipSnmpV3LocalPswdKeyLen = tcpipSnmpv3Component.createIntegerSymbol("TCPIP_SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN", tcpipSnmpV3AdvSettings)
    tcpipSnmpV3LocalPswdKeyLen.setHelp("mcc_h3_snmpv3_configurations")
    tcpipSnmpV3LocalPswdKeyLen.setLabel("Localized Password Key Length")
    tcpipSnmpV3LocalPswdKeyLen.setVisible(True)
    tcpipSnmpV3LocalPswdKeyLen.setDescription("Localized Password Key Length")
    tcpipSnmpV3LocalPswdKeyLen.setDefaultValue(20)

    # Privacy Password Key Length
    tcpipSnmpV3PrivacyPswdKeyLen = tcpipSnmpv3Component.createIntegerSymbol("TCPIP_SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN", tcpipSnmpV3AdvSettings)
    tcpipSnmpV3PrivacyPswdKeyLen.setHelp("mcc_h3_snmpv3_configurations")
    tcpipSnmpV3PrivacyPswdKeyLen.setLabel("Privacy Password Key Length")
    tcpipSnmpV3PrivacyPswdKeyLen.setVisible(True)
    tcpipSnmpV3PrivacyPswdKeyLen.setDescription("Privacy Password Key Length")
    tcpipSnmpV3PrivacyPswdKeyLen.setDefaultValue(20)
        
    tcpipSnmpV3CommMaxNum= tcpipSnmpv3Component.createIntegerSymbol("TCPIP_SNMPV3_USE_MAX_NUM", None)
    tcpipSnmpV3CommMaxNum.setDefaultValue(tcpipSnmpV3MaxUsrNum)
    tcpipSnmpV3CommMaxNum.setVisible(False)

    tcpipSnmpV3heapdependency = ["tcpipNetConfig.TCPIP_STACK_NETWORK_INTERAFCE_COUNT", "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]    
        
    # SNMPv3 Heap Size
    tcpipSnmpV3HeapSize = tcpipSnmpv3Component.createIntegerSymbol("TCPIP_SNMPV3_HEAP_SIZE", None)
    tcpipSnmpV3HeapSize.setLabel("SNMPv3 Heap Size (bytes)") 
    tcpipSnmpV3HeapSize.setVisible(False)
    tcpipSnmpV3HeapSize.setDefaultValue(tcpipSnmpV3HeapCalc())
    tcpipSnmpV3HeapSize.setReadOnly(True)
    tcpipSnmpV3HeapSize.setDependencies(tcpipSnmpV3HeapUpdate, tcpipSnmpV3heapdependency)    
    

    #Add to system_config.h
    tcpipSnmpV3HeaderFtl = tcpipSnmpv3Component.createFileSymbol(None, None)
    tcpipSnmpV3HeaderFtl.setSourcePath("tcpip/config/snmpv3.h.ftl")
    tcpipSnmpV3HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipSnmpV3HeaderFtl.setMarkup(True)
    tcpipSnmpV3HeaderFtl.setType("STRING")
        
    # Add snmpv3.c file
    tcpipSnmpV3SourceFile = tcpipSnmpv3Component.createFileSymbol(None, None)
    tcpipSnmpV3SourceFile.setSourcePath("tcpip/src/snmpv3.c")
    tcpipSnmpV3SourceFile.setOutputName("snmpv3.c")
    tcpipSnmpV3SourceFile.setOverwrite(True)
    tcpipSnmpV3SourceFile.setDestPath("library/tcpip/src/")
    tcpipSnmpV3SourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipSnmpV3SourceFile.setType("SOURCE")
    tcpipSnmpV3SourceFile.setEnabled(True)
    tcpipSnmpV3SourceFile.setDependencies(tcpipSnmpv3GenSourceFile, ["TCPIP_USE_SNMPv3"])
    
    # file TCPIP_SNMPV3_USM_C "$HARMONY_VERSION_PATH/framework/tcpip/src/snmpv3_usm.c" to "$PROJECT_SOURCE_FILES/framework/tcpip/src/snmpv3_usm.c"
    tcpipSnmpV3UsmSourceFile = tcpipSnmpv3Component.createFileSymbol(None, None)
    tcpipSnmpV3UsmSourceFile.setSourcePath("tcpip/src/snmpv3_usm.c")
    tcpipSnmpV3UsmSourceFile.setOutputName("snmpv3_usm.c")
    tcpipSnmpV3UsmSourceFile.setOverwrite(True)
    tcpipSnmpV3UsmSourceFile.setDestPath("library/tcpip/src/")
    tcpipSnmpV3UsmSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipSnmpV3UsmSourceFile.setType("SOURCE")
    tcpipSnmpV3UsmSourceFile.setEnabled(True)
    tcpipSnmpV3UsmSourceFile.setDependencies(tcpipSnmpv3GenSourceFile, ["TCPIP_USE_SNMPv3"])

    


        
# make SnmpV3 Module Stack Config V1-V2 Trap visible
def tcpipSnmpV3StackConfigV1V2TrapVisible(tcpipDependentSymbol, tcpipIPSymbol): 
    #tcpipSnmpV3 = Database.getSymbolValue("tcpipSnmpv3","TCPIP_USE_SNMPv3")
    tcpipSnmpTrapSupport = Database.getSymbolValue("tcpipSnmp","TCPIP_SNMP_USE_TRAP_SUPPORT")
    tcpipSnmpV2TrapSupport = Database.getSymbolValue("tcpipSnmp","TCPIP_SNMP_STACK_USE_V2_TRAP")

    # if(tcpipSnmpV3 and tcpipSnmpTrapSupport and tcpipSnmpV2TrapSupport ):
    if(tcpipSnmpTrapSupport and tcpipSnmpV2TrapSupport ):
        tcpipDependentSymbol.setVisible(True)
    else:
        tcpipDependentSymbol.setVisible(False)
        
        
def tcpipSnmpv3MenuVisibleSingle(symbol, event):
    if (event["value"] == True):
        print("SNMP V3 Menu Visible.")      
        symbol.setVisible(True)
    else:
        print("SNMP V3 Menu Invisible.")
        symbol.setVisible(False)
        
def tcpipSnmpv3GenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])
    
def tcpipSnmpv3EnableConfig(symbol, event):
    global tcpipSnmpV3ConfigNumPrev
    print("Start tcpipSnmpv3EnableConfig")
    if(event["id"] == "TCPIP_USE_SNMPv3" ):
        tcpipSnmpv3Enable = Database.getSymbolValue("tcpipSnmpv3","TCPIP_USE_SNMPv3")
        tcpipSnmpv3Index = int(symbol.getID().strip("TCPIP_SNMPV3_STACK_CONFIG_IDX"))
        print("Snmp Index: " + str(tcpipSnmpv3Index) )
        print(tcpipSnmpV3ConfigNumPrev)
        if(tcpipSnmpv3Enable == True):
            if(tcpipSnmpv3Index < tcpipSnmpV3ConfigNumPrev ):
                symbol.setVisible(True)
        else:
            symbol.setVisible(False)
        
    else:       
        print(symbol.getID())
        print(event["id"])
        tcpipSnmpv3ConfigNumberValue = event["value"]
        print(tcpipSnmpv3ConfigNumberValue)
        print(tcpipSnmpV3ConfigNumPrev)
        if(tcpipSnmpv3ConfigNumberValue > tcpipSnmpV3ConfigNumPrev ):
            tcpipSnmpV3StackConfigIdx[tcpipSnmpV3ConfigNumPrev].setVisible(True)
            tcpipSnmpV3StackConfigIdx[tcpipSnmpV3ConfigNumPrev].setValue(True, 1)
            print("Set TRUE"+ str(tcpipSnmpV3ConfigNumPrev))
            tcpipSnmpV3ConfigNumPrev = tcpipSnmpV3ConfigNumPrev + 1
            #Add more network configurations
        else:
            if(tcpipSnmpv3ConfigNumberValue < tcpipSnmpV3ConfigNumPrev ):
                #Reduce network configurations
                tcpipSnmpV3ConfigNumPrev = tcpipSnmpV3ConfigNumPrev - 1
                tcpipSnmpV3StackConfigIdx[tcpipSnmpV3ConfigNumPrev].setVisible(False)
                tcpipSnmpV3StackConfigIdx[tcpipSnmpV3ConfigNumPrev].setValue(False, 1)
                print("Set FALSE"+ str(tcpipSnmpV3ConfigNumPrev))
                
            #else:
                #Do Nothing
            
    print("END tcpipSnmpv3EnableConfig")

def tcpipSnmpV3HeapCalc(): 
    heap_size = 975
    return heap_size    
    
def tcpipSnmpV3HeapUpdate(symbol, event): 
    heap_size = tcpipSnmpV3HeapCalc()
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
    Database.setSymbolValue("tcpipSnmpv3", "TCPIP_USE_SNMPv3", False, 2)
