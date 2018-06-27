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
	tcpipSnmp.setLabel("Use SNMP")
	tcpipSnmp.setVisible(False)
	tcpipSnmp.setDescription("Use SNMP")
	tcpipSnmp.setDefaultValue(True) 
	# H3_ToDo   
	# select USE_SYS_FS_NEEDED
	# select USE_CRYPTO_LIB_NEEDED
	#tcpipSnmp.setDependencies(tcpipSnmpMenuVisibleSingle, ["tcpipUdp.TCPIP_USE_UDP"])

	# SNMP process rate
	tcpipSnmpTskProcRate = tcpipSnmpComponent.createIntegerSymbol("TCPIP_SNMP_TASK_PROCESS_RATE", None)
	tcpipSnmpTskProcRate.setLabel("SNMP process rate")
	tcpipSnmpTskProcRate.setVisible(True)
	tcpipSnmpTskProcRate.setDescription("SNMP process rate")
	tcpipSnmpTskProcRate.setDefaultValue(200)
	#tcpipSnmpTskProcRate.setDependencies(tcpipSnmpMenuVisibleSingle, ["TCPIP_USE_SNMP"])

	# BIB file name
	tcpipSnmpBibFileName = tcpipSnmpComponent.createStringSymbol("TCPIP_SNMP_BIB_FILE_NAME", None)
	tcpipSnmpBibFileName.setLabel("BIB file name")
	tcpipSnmpBibFileName.setVisible(True)
	tcpipSnmpBibFileName.setDescription("BIB file name")
	tcpipSnmpBibFileName.setDefaultValue("snmp.bib")
	#tcpipSnmpBibFileName.setDependencies(tcpipSnmpMenuVisibleSingle, ["TCPIP_USE_SNMP"])

	# Maximum OID Length
	tcpipSnmpOldLenMax = tcpipSnmpComponent.createIntegerSymbol("TCPIP_SNMP_OID_MAX_LEN", None)
	tcpipSnmpOldLenMax.setLabel("Max OID Length")
	tcpipSnmpOldLenMax.setVisible(True)
	tcpipSnmpOldLenMax.setDescription("Maximum OID Length")
	tcpipSnmpOldLenMax.setDefaultValue(18)
	#tcpipSnmpOldLenMax.setDependencies(tcpipSnmpMenuVisibleSingle, ["TCPIP_USE_SNMP"])

	# Maximum SNMP message size
	tcpipSnmpMsgSizeMax = tcpipSnmpComponent.createIntegerSymbol("TCPIP_SNMP_MAX_MSG_SIZE", None)
	tcpipSnmpMsgSizeMax.setLabel("Max SNMP message size")
	tcpipSnmpMsgSizeMax.setVisible(True)
	tcpipSnmpMsgSizeMax.setDescription("Maximum SNMP message size")
	tcpipSnmpMsgSizeMax.setDefaultValue(480)
	#tcpipSnmpMsgSizeMax.setDependencies(tcpipSnmpMenuVisibleSingle, ["TCPIP_USE_SNMP"])

	# Maximum non record OID ID
	tcpipSnmpNonRecOldIdMax = tcpipSnmpComponent.createIntegerSymbol("TCPIP_SNMP_MAX_NON_REC_ID_OID", None)
	tcpipSnmpNonRecOldIdMax.setLabel("Max non record OID ID")
	tcpipSnmpNonRecOldIdMax.setVisible(True)
	tcpipSnmpNonRecOldIdMax.setDescription("Maximum non record OID ID")
	tcpipSnmpNonRecOldIdMax.setDefaultValue(3)
	#tcpipSnmpNonRecOldIdMax.setDependencies(tcpipSnmpMenuVisibleSingle, ["TCPIP_USE_SNMP"])

	# Maximum Community Length
	tcpipSnmpCommunityLenMax = tcpipSnmpComponent.createIntegerSymbol("TCPIP_SNMP_COMMUNITY_MAX_LEN", None)
	tcpipSnmpCommunityLenMax.setLabel("Max Community Length")
	tcpipSnmpCommunityLenMax.setVisible(True)
	tcpipSnmpCommunityLenMax.setDescription("Maximum Community Length")
	tcpipSnmpCommunityLenMax.setDefaultValue(8)
	#tcpipSnmpCommunityLenMax.setDependencies(tcpipSnmpMenuVisibleSingle, ["TCPIP_USE_SNMP"])

	
	tcpipSnmpCommMaxNum= tcpipSnmpComponent.createIntegerSymbol("TCPIP_SNMP_COMMUNITY_MAX_NUM", None)
	tcpipSnmpCommMaxNum.setDefaultValue(tcpipSnmpMaxCommNum)
	tcpipSnmpCommMaxNum.setVisible(False)
	
	# Maximum Community Support
	tcpipSnmpCommunitySuppMax = tcpipSnmpComponent.createIntegerSymbol("TCPIP_SNMP_MAX_COMMUNITY_SUPPORT", None)
	tcpipSnmpCommunitySuppMax.setLabel("Max Community Support")
	tcpipSnmpCommunitySuppMax.setMax(tcpipSnmpMaxCommNum)
	tcpipSnmpCommunitySuppMax.setMin(0)
	tcpipSnmpCommunitySuppMax.setVisible(True)
	tcpipSnmpCommunitySuppMax.setDescription("Maximum Community Support")
	tcpipSnmpCommunitySuppMax.setDefaultValue(3)
	#tcpipSnmpCommunitySuppMax.setDependencies(tcpipSnmpMenuVisibleSingle, ["TCPIP_USE_SNMP"])
	
####-----------------------------------------------------------------------------------------##########
	for index in range(0,tcpipSnmpMaxCommNum):	
		tcpipSnmpStackConfigIdx.append(tcpipSnmpComponent.createBooleanSymbol("TCPIP_SNMP_STACK_CONFIG_IDX"+str(index),None))
		tcpipSnmpStackConfigIdx[index].setLabel("SNMP Stack Config Number "+ str(index))
		tcpipSnmpStackConfigIdx[index].setVisible(True)
		if (index < tcpipSnmpCommunitySuppMax.getValue()):	
			tcpipSnmpStackConfigIdx[index].setDefaultValue(True)
		else:			
			tcpipSnmpStackConfigIdx[index].setDefaultValue(False)
			
		tcpipSnmpStackConfigIdx[index].setDependencies(tcpipSnmpEnableConfig, ["TCPIP_USE_SNMP","TCPIP_SNMP_MAX_COMMUNITY_SUPPORT"])
		
		# Snmp Read Community Name
		tcpipSnmpReadCommName.append(tcpipSnmpComponent.createStringSymbol("TCPIP_SNMP_STACK_READCOMMUNITY_NAME_IDX" + str(index), tcpipSnmpStackConfigIdx[index]))
		tcpipSnmpReadCommName[index].setLabel("Read Community Name")
		tcpipSnmpReadCommName[index].setVisible(True)
		if(index == 0):
			tcpipSnmpReadCommName[index].setDefaultValue("public")
		else:
			if(index == 1):
				tcpipSnmpReadCommName[index].setDefaultValue("read")
			else:
				if(index == 2):
					tcpipSnmpReadCommName[index].setDefaultValue("")
					
		tcpipSnmpReadCommName[index].setDependencies(tcpipSnmpMenuVisibleSingle, [tcpipSnmpStackConfigIdx[index].getID()])
		
		# Snmp Write Community Name
		tcpipSnmpWriteCommName.append(tcpipSnmpComponent.createStringSymbol("TCPIP_SNMP_STACK_WRITECOMMUNITY_NAME_IDX" + str(index), tcpipSnmpStackConfigIdx[index]))
		tcpipSnmpWriteCommName[index].setLabel("Write Community Name")
		tcpipSnmpWriteCommName[index].setVisible(True)
		if(index == 0):
			tcpipSnmpWriteCommName[index].setDefaultValue("private")
		else:
			if(index == 1):
				tcpipSnmpWriteCommName[index].setDefaultValue("write")
			else:
				if(index == 2):
					tcpipSnmpWriteCommName[index].setDefaultValue("public")
					
		tcpipSnmpWriteCommName[index].setDependencies(tcpipSnmpMenuVisibleSingle, [tcpipSnmpStackConfigIdx[index].getID()])
####-----------------------------------------------------------------------------------------##########

	# Notify Community Length
	tcpipSnmpNotifyCommunLen = tcpipSnmpComponent.createIntegerSymbol("TCPIP_SNMP_NOTIFY_COMMUNITY_LEN", None)
	tcpipSnmpNotifyCommunLen.setLabel("Notify Community Length")
	tcpipSnmpNotifyCommunLen.setVisible(True)
	tcpipSnmpNotifyCommunLen.setDescription("Notify Community Length")
	tcpipSnmpNotifyCommunLen.setDefaultValue(tcpipSnmpCommunityLenMax.getValue())
	#tcpipSnmpNotifyCommunLen.setDependencies(tcpipSnmpMenuVisibleSingle, ["TCPIP_USE_SNMP"])

	# Trap community max length
	tcpipSnmpTrapCommunMemUseLenMax = tcpipSnmpComponent.createIntegerSymbol("TCPIP_SNMP_TRAP_COMMUNITY_MAX_LEN_MEM_USE", None)
	tcpipSnmpTrapCommunMemUseLenMax.setLabel("Trap community max length")
	tcpipSnmpTrapCommunMemUseLenMax.setVisible(True)
	tcpipSnmpTrapCommunMemUseLenMax.setDescription("Trap community max length")
	tcpipSnmpTrapCommunMemUseLenMax.setDefaultValue(8)
	#tcpipSnmpTrapCommunMemUseLenMax.setDependencies(tcpipSnmpMenuVisibleSingle, ["TCPIP_USE_SNMP"])

	# Trap table size
	tcpipSnmpTrapTblSize = tcpipSnmpComponent.createIntegerSymbol("TCPIP_SNMP_TRAP_TABLE_SIZE", None)
	tcpipSnmpTrapTblSize.setLabel("Trap table size")
	tcpipSnmpTrapTblSize.setVisible(True)
	tcpipSnmpTrapTblSize.setDescription("Trap table size")
	tcpipSnmpTrapTblSize.setDefaultValue(2)
	#tcpipSnmpTrapTblSize.setDependencies(tcpipSnmpMenuVisibleSingle, ["TCPIP_USE_SNMP"])

	# Trap Support
	tcpipSnmpTrapSupport = tcpipSnmpComponent.createBooleanSymbol("TCPIP_SNMP_USE_TRAP_SUPPORT", None)
	tcpipSnmpTrapSupport.setLabel("Trap Support")
	tcpipSnmpTrapSupport.setVisible(True)
	tcpipSnmpTrapSupport.setDescription("Trap Support")
	tcpipSnmpTrapSupport.setDefaultValue(True) 
	#tcpipSnmpTrapSupport.setDependencies(tcpipSnmpMenuVisibleSingle, ["TCPIP_USE_SNMP"])

	# V2 Trap Support
	tcpipSnmpV2TrapSupport = tcpipSnmpComponent.createBooleanSymbol("TCPIP_SNMP_STACK_USE_V2_TRAP", tcpipSnmpTrapSupport)
	tcpipSnmpV2TrapSupport.setLabel("V2 Trap Support")
	tcpipSnmpV2TrapSupport.setVisible(True)
	tcpipSnmpV2TrapSupport.setDescription("V2 Trap Support")
	tcpipSnmpV2TrapSupport.setDefaultValue(True) 
	tcpipSnmpV2TrapSupport.setDependencies(tcpipSnmpMenuVisibleSingle, ["TCPIP_SNMP_USE_TRAP_SUPPORT"])

	# Include SNMP Custom Template
	tcpipSnmpCustTemplate = tcpipSnmpComponent.createBooleanSymbol("TCPIP_SNMP_CUSTOM_TEMPLATE", None)
	tcpipSnmpCustTemplate.setLabel("Include SNMP Custom Template")
	tcpipSnmpCustTemplate.setVisible(True)
	tcpipSnmpCustTemplate.setDescription("Include SNMP Custom Template")
	tcpipSnmpCustTemplate.setDefaultValue(True) 
	#tcpipSnmpCustTemplate.setDependencies(tcpipSnmpMenuVisibleSingle, ["TCPIP_USE_SNMP"])

	# Add snmp.c file
	tcpipSnmpSourceFile = tcpipSnmpComponent.createFileSymbol(None, None)
	tcpipSnmpSourceFile.setSourcePath("tcpip/src/snmp.c")
	tcpipSnmpSourceFile.setOutputName("snmp.c")
	tcpipSnmpSourceFile.setOverwrite(True)
	tcpipSnmpSourceFile.setDestPath("library/tcpip/src/")
	tcpipSnmpSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipSnmpSourceFile.setType("SOURCE")
	tcpipSnmpSourceFile.setEnabled(True)
	#tcpipSnmpSourceFile.setDependencies(tcpipSnmpGenSourceFile, ["TCPIP_USE_SNMP"])


# ifblock TCPIP_SNMP_CUSTOM_TEMPLATE
# template SNMP_CUSTOM_APP_C "$HARMONY_VERSION_PATH/framework/tcpip/config/custom_app/custom_snmp_app.c.ftl" to "$PROJECT_SOURCE_FILES/app/custom_snmp_app.c"
	tcpipSnmpCstmAppSourceFile = tcpipSnmpComponent.createFileSymbol(None, None)
	tcpipSnmpCstmAppSourceFile.setSourcePath("tcpip/config/custom_app/custom_snmp_app.c.ftl")
	tcpipSnmpCstmAppSourceFile.setOutputName("custom_snmp_app.c")
	tcpipSnmpCstmAppSourceFile.setDestPath("app/")
	tcpipSnmpCstmAppSourceFile.setProjectPath("app/")
	tcpipSnmpCstmAppSourceFile.setType("SOURCE")
	tcpipSnmpCstmAppSourceFile.setMarkup(True)
	tcpipSnmpCstmAppSourceFile.setDependencies(tcpipSnmpGenSourceFile, ["TCPIP_SNMP_CUSTOM_TEMPLATE"])
	
# template SNMP_MIB_H "$HARMONY_VERSION_PATH/framework/tcpip/config/custom_app/mib.h.ftl" to "$PROJECT_HEADER_FILES/app/mib.h"
	tcpipSnmpMibHeaderFile = tcpipSnmpComponent.createFileSymbol(None, None)
	tcpipSnmpMibHeaderFile.setSourcePath("tcpip/config/custom_app/mib.h.ftl")
	tcpipSnmpMibHeaderFile.setOutputName("mib.h")
	tcpipSnmpMibHeaderFile.setMarkup(True)
	tcpipSnmpMibHeaderFile.setDestPath("app/")
	tcpipSnmpMibHeaderFile.setProjectPath("app/")
	tcpipSnmpMibHeaderFile.setType("HEADER")
	tcpipSnmpMibHeaderFile.setOverwrite(True)
	tcpipSnmpMibHeaderFile.setDependencies(tcpipSnmpGenSourceFile, ["TCPIP_SNMP_CUSTOM_TEMPLATE"])
# ifblock (USE_SYS_FS) && (SYS_FS_MPFS) && (!TCPIP_STACK_USE_HTTP_SERVER)
# template SNMP_MPFS_IMG "$HARMONY_VERSION_PATH/framework/tcpip/config/custom_app/mpfs_img2.c.ftl" to "$PROJECT_SOURCE_FILES/app/mpfs_img2.c"
# endif
	tcpipSnmpMpfsImg2SourceFile = tcpipSnmpComponent.createFileSymbol(None, None)
	tcpipSnmpMpfsImg2SourceFile.setSourcePath("tcpip/config/custom_app/mpfs_img2.c.ftl")
	tcpipSnmpMpfsImg2SourceFile.setOutputName("mpfs_img2.c")
	tcpipSnmpMpfsImg2SourceFile.setDestPath("app/")
	tcpipSnmpMpfsImg2SourceFile.setProjectPath("app/")
	tcpipSnmpMpfsImg2SourceFile.setType("SOURCE")
	tcpipSnmpMpfsImg2SourceFile.setMarkup(True)
	tcpipSnmpMpfsImg2SourceFile.setDependencies(tcpipSnmpGenSourceFile, ["TCPIP_SNMP_CUSTOM_TEMPLATE"])
		
		
def tcpipSnmpMenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("SNMP Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("SNMP Menu Invisible.")
		symbol.setVisible(False)
		
def tcpipSnmpGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])

#H3_ToDo revisit this implementataion
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
	
