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

def instantiateComponent(tcpipSnmpv3Component):
	print("TCPIP SNMP V3 Component")
	configName = Variables.get("__CONFIGURATION_NAME")				
	# Use SNMP Version 3
	tcpipSnmpV3 = tcpipSnmpv3Component.createBooleanSymbol("TCPIP_USE_SNMPv3", None)
	tcpipSnmpV3.setLabel("Use SNMP Version 3")
	tcpipSnmpV3.setVisible(False)
	tcpipSnmpV3.setDescription("Use SNMP Version 3")
	tcpipSnmpV3.setDefaultValue(True) 
	#tcpipSnmpV3.setDependencies(tcpipSnmpv3MenuVisibleSingle, ["tcpipSnmp.TCPIP_USE_SNMP"])

	# Security User Name Length
	tcpipSnmpV3UsrSecureNameLen = tcpipSnmpv3Component.createIntegerSymbol("TCPIP_SNMPV3_USER_SECURITY_NAME_LEN", None)
	tcpipSnmpV3UsrSecureNameLen.setLabel("Security User Name Length")
	tcpipSnmpV3UsrSecureNameLen.setVisible(True)
	tcpipSnmpV3UsrSecureNameLen.setDescription("Security User Name Length")
	tcpipSnmpV3UsrSecureNameLen.setDefaultValue(16)
	#tcpipSnmpV3UsrSecureNameLen.setDependencies(tcpipSnmpv3MenuVisibleSingle, ["TCPIP_USE_SNMPv3"])

	# Localized Password Key Length
	tcpipSnmpV3LocalPswdKeyLen = tcpipSnmpv3Component.createIntegerSymbol("TCPIP_SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN", None)
	tcpipSnmpV3LocalPswdKeyLen.setLabel("Localized Password Key Length")
	tcpipSnmpV3LocalPswdKeyLen.setVisible(True)
	tcpipSnmpV3LocalPswdKeyLen.setDescription("Localized Password Key Length")
	tcpipSnmpV3LocalPswdKeyLen.setDefaultValue(20)
	#tcpipSnmpV3LocalPswdKeyLen.setDependencies(tcpipSnmpv3MenuVisibleSingle, ["TCPIP_USE_SNMPv3"])

	# Privacy Password Key Length
	tcpipSnmpV3PrivacyPswdKeyLen = tcpipSnmpv3Component.createIntegerSymbol("TCPIP_SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN", None)
	tcpipSnmpV3PrivacyPswdKeyLen.setLabel("Privacy Password Key Length")
	tcpipSnmpV3PrivacyPswdKeyLen.setVisible(True)
	tcpipSnmpV3PrivacyPswdKeyLen.setDescription("Privacy Password Key Length")
	tcpipSnmpV3PrivacyPswdKeyLen.setDefaultValue(20)
	#tcpipSnmpV3PrivacyPswdKeyLen.setDependencies(tcpipSnmpv3MenuVisibleSingle, ["TCPIP_USE_SNMPv3"])
		
	tcpipSnmpV3CommMaxNum= tcpipSnmpv3Component.createIntegerSymbol("TCPIP_SNMPV3_USE_MAX_NUM", None)
	tcpipSnmpV3CommMaxNum.setDefaultValue(tcpipSnmpV3MaxUsrNum)
	tcpipSnmpV3CommMaxNum.setVisible(False)

	# Maximum Users
	tcpipSnmpV3UsmUsrMax = tcpipSnmpv3Component.createIntegerSymbol("TCPIP_SNMPV3_USM_MAX_USER", None)
	tcpipSnmpV3UsmUsrMax.setLabel("Max Users")
	tcpipSnmpV3UsmUsrMax.setVisible(True)
	tcpipSnmpV3UsmUsrMax.setDescription("Maximum Users")
	tcpipSnmpV3UsmUsrMax.setMax(tcpipSnmpV3MaxUsrNum)
	tcpipSnmpV3UsmUsrMax.setMin(0)
	tcpipSnmpV3UsmUsrMax.setDefaultValue(3)
	#tcpipSnmpV3UsmUsrMax.setDependencies(tcpipSnmpv3MenuVisibleSingle, ["TCPIP_USE_SNMPv3"])

	####-----------------------------------------------------------------------------------------##########
	for index in range(0,tcpipSnmpV3MaxUsrNum):	
		tcpipSnmpV3StackConfigIdx.append(tcpipSnmpv3Component.createBooleanSymbol("TCPIP_SNMPV3_STACK_CONFIG_IDX"+str(index),None))
		tcpipSnmpV3StackConfigIdx[index].setLabel("SNMPV3 Stack Config Number "+ str(index))
		tcpipSnmpV3StackConfigIdx[index].setVisible(True)
		if (index < tcpipSnmpV3UsmUsrMax.getValue()):				
			tcpipSnmpV3StackConfigIdx[index].setDefaultValue(True)
		else:			
			tcpipSnmpV3StackConfigIdx[index].setDefaultValue(False)			
		tcpipSnmpV3StackConfigIdx[index].setDependencies(tcpipSnmpv3EnableConfig, ["TCPIP_USE_SNMPv3","TCPIP_SNMPV3_USM_MAX_USER"])	
		
		# Snmpv3 User Name
		tcpipSnmpv3UsrName.append(tcpipSnmpv3Component.createStringSymbol("TCPIP_SNMPV3_STACK_USM_NAME_IDX" + str(index), tcpipSnmpV3StackConfigIdx[index]))
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
		tcpipSnmpv3PrivProtocol[index].setLabel("Privacy Protocol")
		tcpipSnmpv3PrivProtocol[index].setVisible(True)
		if(index == 0):
			tcpipSnmpv3PrivProtocol[index].setDefaultValue("SNMPV3_AES_PRIV")
		else:
			if(index == 1):
				tcpipSnmpv3PrivProtocol[index].setDefaultValue("SNMPV3_NO_PRIV")
			else:
				if(index == 2):
					tcpipSnmpv3PrivProtocol[index].setDefaultValue("")					
		tcpipSnmpv3PrivProtocol[index].setDependencies(tcpipSnmpv3MenuVisibleSingle, [tcpipSnmpV3StackConfigIdx[index].getID()])	

		# Snmpv3 Privacy Protocol Password
		tcpipSnmpv3PrivProtocolPswd.append(tcpipSnmpv3Component.createStringSymbol("TCPIP_SNMPV3_STACK_PRIV_PASSWORD_IDX" + str(index), tcpipSnmpV3StackConfigIdx[index]))
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
		
		# H3_ToDo; review this with Snmp owner
		# config TCPIP_SNMPV3_TARGET_ENTRY_SEC_NAME_IDX0
		# string "Target Entry Security Name"
		# depends on TCPIP_SNMPV3_STACK_CONFIG_IDX0
		# depends on TCPIP_SNMPV3_STACK_USM_NAME_IDX0		
		
		# Snmpv3 Target Entry Message Protocol Type
		tcpipSnmpv3TrgtEntryMsgProtType.append(tcpipSnmpv3Component.createComboSymbol("TCPIP_SNMPV3_TARGET_ENTRY_MESSAGE_PROTOCOL_TYPE_IDX" + str(index), tcpipSnmpV3StackConfigIdx[index],TCPIP_SNMPV3_MESSAGE_PROCESSING_MODEL_TYPE))
		tcpipSnmpv3TrgtEntryMsgProtType[index].setLabel("Target Entry Message Protocol Type")
		tcpipSnmpv3TrgtEntryMsgProtType[index].setVisible(True)
		tcpipSnmpv3TrgtEntryMsgProtType[index].setDefaultValue("SNMPV3_MSG_PROCESSING_MODEL")
		tcpipSnmpv3TrgtEntryMsgProtType[index].setDependencies(tcpipSnmpv3MenuVisibleSingle, [tcpipSnmpV3StackConfigIdx[index].getID()])
		
		# Snmpv3 Target Entry Security Model Type
		tcpipSnmpv3TrgtEntrySecModelType.append(tcpipSnmpv3Component.createComboSymbol("TCPIP_SNMPV3_TARGET_ENTRY_SEC_MODEL_TYPE_IDX" + str(index), tcpipSnmpV3StackConfigIdx[index],TCPIP_SNMPV3_SECURITY_MODEL_TYPE))
		tcpipSnmpv3TrgtEntrySecModelType[index].setLabel("Target Entry Security Model Type")
		tcpipSnmpv3TrgtEntrySecModelType[index].setVisible(True)
		tcpipSnmpv3TrgtEntrySecModelType[index].setDefaultValue("SNMPV3_USM_SECURITY_MODEL")
		tcpipSnmpv3TrgtEntrySecModelType[index].setDependencies(tcpipSnmpv3MenuVisibleSingle, [tcpipSnmpV3StackConfigIdx[index].getID()])

		# H3_ToDo; review this with Snmp owner
		# config TCPIP_SNMPV3_TARGET_ENTRY_SEC_LEVEL_IDX0
		# string "Target Entry Security Level"
		# depends on TCPIP_SNMPV3_STACK_CONFIG_IDX0
		# depends on TCPIP_SNMPV3_STACK_SECURITY_LEVEL_IDX0
	####-----------------------------------------------------------------------------------------##########
	
	# V1 and V2 Trap Support
	tcpipSnmpV3StackV1V2Trap = tcpipSnmpv3Component.createBooleanSymbol("TCPIP_SNMPV3_STACK_USE_V1_V2_TRAP", None)
	tcpipSnmpV3StackV1V2Trap.setLabel("V1 and V2 Trap Support")
	tcpipSnmpV3StackV1V2Trap.setVisible(True)
	tcpipSnmpV3StackV1V2Trap.setDescription("V1 and V2 Trap Support")
	tcpipSnmpV3StackV1V2Trap.setDefaultValue(True) 
	tcpipSnmpV3StackV1V2Trap.setDependencies(tcpipSnmpV3StackConfigV1V2TrapVisible, ["TCPIP_USE_SNMPv3" , "tcpipSnmp.TCPIP_SNMP_USE_TRAP_SUPPORT", "tcpipSnmp.TCPIP_SNMP_STACK_USE_V2_TRAP"])

	# H3_ToDo
	# file SNMPV3_AES_H "$HARMONY_VERSION_PATH/framework/tcpip/src/crypto/aes.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/crypto/aes.h"
	# file SNMPV3_AES_CBC_H "$HARMONY_VERSION_PATH/framework/tcpip/src/crypto/aes_cbc.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/crypto/aes_cbc.h"
	# file SNMPV3_AES_CFB_H "$HARMONY_VERSION_PATH/framework/tcpip/src/crypto/aes_cfb.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/crypto/aes_cfb.h"
	# file SNMPV3_AES_CTR_H "$HARMONY_VERSION_PATH/framework/tcpip/src/crypto/aes_ctr.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/crypto/aes_ctr.h"
	# file SNMPV3_AES_ECB_H "$HARMONY_VERSION_PATH/framework/tcpip/src/crypto/aes_ecb.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/crypto/aes_ecb.h"
	# file SNMPV3_AES_ECB_PIC32_H "$HARMONY_VERSION_PATH/framework/tcpip/src/crypto/aes_ecb_pic32.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/crypto/aes_ecb_pic32.h"
	# file SNMPV3_AES_OFB_H "$HARMONY_VERSION_PATH/framework/tcpip/src/crypto/aes_ofb.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/crypto/aes_ofb.h"
	# file SNMPV3_DES_H "$HARMONY_VERSION_PATH/framework/crypto/src/des3.h" to "$PROJECT_HEADER_FILES/framework/crypto/src/des3.h"

	# Add snmpv3.c file
	tcpipSnmpV3SourceFile = tcpipSnmpv3Component.createFileSymbol(None, None)
	tcpipSnmpV3SourceFile.setSourcePath("tcpip/src/snmpv3.c")
	tcpipSnmpV3SourceFile.setOutputName("snmpv3.c")
	tcpipSnmpV3SourceFile.setOverwrite(True)
	tcpipSnmpV3SourceFile.setDestPath("library/tcpip/src/")
	tcpipSnmpV3SourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipSnmpV3SourceFile.setType("SOURCE")
	tcpipSnmpV3SourceFile.setEnabled(True)
	tcpipSnmpV3SourceFile.setDependencies(tcpipSnmpv3GenSourceFile, ["TCPIP_USE_SNMP"])

	# H3_ToDo
	# file TCPIP_SNMPV3_USM_C "$HARMONY_VERSION_PATH/framework/tcpip/src/snmpv3_usm.c" to "$PROJECT_SOURCE_FILES/framework/tcpip/src/snmpv3_usm.c"
	# ifblock (TCPIP_SNMPV3_STACK_PRIV_PROTOCOL_IDX0 = "SNMPV3_AES_PRIV" || TCPIP_SNMPV3_STACK_PRIV_PROTOCOL_IDX1 = "SNMPV3_AES_PRIV" || TCPIP_SNMPV3_STACK_PRIV_PROTOCOL_IDX2 = "SNMPV3_AES_PRIV") 
	# library TCPIP_SNMPV3_A "$HARMONY_VERSION_PATH/framework/tcpip/src/crypto/aes_pic32mx.a"
	# endif
	# endif

	# ifblock (TCPIP_SNMPV3_STACK_PRIV_PROTOCOL_IDX0 = "SNMPV3_DES_PRIV" || TCPIP_SNMPV3_STACK_PRIV_PROTOCOL_IDX1 = "SNMPV3_DES_PRIV" || TCPIP_SNMPV3_STACK_PRIV_PROTOCOL_IDX2 = "SNMPV3_DES_PRIV") 
	# file SNMPV3_DES_C "$HARMONY_VERSION_PATH/framework/crypto/src/des3.c" to "$PROJECT_SOURCE_FILES/framework/crypto/src/des3.c"
	# endif


		
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
	
#H3_ToDo revisit this implementataion
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