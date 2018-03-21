    
def instantiateComponent(tcpipIgmpComponent):
	print("TCPIP IGMP Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	
	# Use IGMPv3 Module
	tcpipIgmp = tcpipIgmpComponent.createBooleanSymbol("TCPIP_USE_IGMP", None)
	tcpipIgmp.setLabel("Use IGMPv3 Module")
	tcpipIgmp.setVisible(True)
	tcpipIgmp.setDescription("Use IGMPv3 Module")
	tcpipIgmp.setDefaultValue(False)   
	tcpipIgmp.setDependencies(tcpipIgmpMenuVisibleSingle, ["tcpipIPv4.TCPIP_STACK_USE_IPV4"])

	# Number of IGMP Interfaces
	tcpipIgmpInterfaceNum = tcpipIgmpComponent.createIntegerSymbol("TCPIP_IGMP_INTERFACES", tcpipIgmp)
	tcpipIgmpInterfaceNum.setLabel("Number of IGMP Interfaces")
	tcpipIgmpInterfaceNum.setVisible(False)
	tcpipIgmpInterfaceNum.setDescription("Number of IGMP Interfaces")
	tcpipIgmpInterfaceNum.setDefaultValue(1)
	tcpipIgmpInterfaceNum.setDependencies(tcpipIgmpMenuVisibleSingle, ["TCPIP_USE_IGMP"])

	# Number of Multicast Groups
	tcpipIgmpMcastGrpNum = tcpipIgmpComponent.createIntegerSymbol("TCPIP_IGMP_MCAST_GROUPS", tcpipIgmp)
	tcpipIgmpMcastGrpNum.setLabel("Number of Multicast Groups")
	tcpipIgmpMcastGrpNum.setVisible(False)
	tcpipIgmpMcastGrpNum.setDescription("Number of Multicast Groups")
	tcpipIgmpMcastGrpNum.setDefaultValue(7)
	tcpipIgmpMcastGrpNum.setDependencies(tcpipIgmpMenuVisibleSingle, ["TCPIP_USE_IGMP"])

	# IGMPv2 Support Only
	tcpipIgmpv2Support = tcpipIgmpComponent.createBooleanSymbol("TCPIP_IGMPV2_SUPPORT_ONLY", tcpipIgmp)
	tcpipIgmpv2Support.setLabel("IGMPv2 Support Only")
	tcpipIgmpv2Support.setVisible(False)
	tcpipIgmpv2Support.setDescription("IGMPv2 Support Only")
	tcpipIgmpv2Support.setDefaultValue(False)   
	tcpipIgmpv2Support.setDependencies(tcpipIgmpMenuVisibleSingle, ["TCPIP_USE_IGMP"])

	# Number of Sources in Each Group
	tcpipIgmpSourceNum = tcpipIgmpComponent.createIntegerSymbol("TCPIP_IGMP_SOURCES_PER_GROUP", tcpipIgmp)
	tcpipIgmpSourceNum.setLabel("Number of Sources in Each Group")
	tcpipIgmpSourceNum.setVisible(False)
	tcpipIgmpSourceNum.setDescription("Number of Sources in Each Group")
	tcpipIgmpSourceNum.setDefaultValue(11)
	tcpipIgmpSourceNum.setDependencies(tcpipIgmpMenuVisibleSingle, ["TCPIP_USE_IGMP"])

	# Number of Sockets per Source
	tcpipIgmpSktNum = tcpipIgmpComponent.createIntegerSymbol("TCPIP_IGMP_SOCKET_RECORDS_PER_SOURCE", tcpipIgmp)
	tcpipIgmpSktNum.setLabel("Number of Sockets per Source")
	tcpipIgmpSktNum.setVisible(False)
	tcpipIgmpSktNum.setDescription("Number of Sockets per Source")
	tcpipIgmpSktNum.setDefaultValue(4)
	tcpipIgmpSktNum.setDependencies(tcpipIgmpMenuVisibleSingle, ["TCPIP_USE_IGMP"])

	# Default Robustness Variable Value
	tcpipIgmpRobustVarValue = tcpipIgmpComponent.createIntegerSymbol("TCPIP_IGMP_ROBUSTNESS_VARIABLE", tcpipIgmp)
	tcpipIgmpRobustVarValue.setLabel("Default Robustness Variable Value")
	tcpipIgmpRobustVarValue.setVisible(False)
	tcpipIgmpRobustVarValue.setDescription("Default Robustness Variable Value")
	tcpipIgmpRobustVarValue.setDefaultValue(2)
	tcpipIgmpRobustVarValue.setDependencies(tcpipIgmpMenuVisibleSingle, ["TCPIP_USE_IGMP"])

	# Default Unsolicited Report Interval in ms
	tcpipIgmpUnsoilicitReportInterval = tcpipIgmpComponent.createIntegerSymbol("TCPIP_IGMP_UNSOLICITED_REPORT_INTERVAL", tcpipIgmp)
	tcpipIgmpUnsoilicitReportInterval.setLabel("Default Unsolicited Report Interval - ms")
	tcpipIgmpUnsoilicitReportInterval.setVisible(False)
	tcpipIgmpUnsoilicitReportInterval.setDescription("Default Unsolicited Report Interval in ms")
	tcpipIgmpUnsoilicitReportInterval.setDefaultValue(1000)
	tcpipIgmpUnsoilicitReportInterval.setDependencies(tcpipIgmpMenuVisibleSingle, ["TCPIP_USE_IGMP"])

	# Enable User Notification Functions"
	tcpipIgmpUsrNotify = tcpipIgmpComponent.createBooleanSymbol("TCPIP_IGMP_USER_NOTIFICATION", tcpipIgmp)
	tcpipIgmpUsrNotify.setLabel("Enable User Notification Functions")
	tcpipIgmpUsrNotify.setVisible(False)
	tcpipIgmpUsrNotify.setDescription("Enable User Notification Functions")
	tcpipIgmpUsrNotify.setDefaultValue(False)   
	tcpipIgmpUsrNotify.setDependencies(tcpipIgmpMenuVisibleSingle, ["TCPIP_USE_IGMP"])

	# IGMP Task Rate
	tcpipIgmpTskTickRate = tcpipIgmpComponent.createIntegerSymbol("TCPIP_IGMP_TASK_TICK_RATE", tcpipIgmp)
	tcpipIgmpTskTickRate.setLabel("IGMP Task Rate")
	tcpipIgmpTskTickRate.setVisible(False)
	tcpipIgmpTskTickRate.setDescription("IGMP Task Rate")
	tcpipIgmpTskTickRate.setDefaultValue(33)
	tcpipIgmpTskTickRate.setDependencies(tcpipIgmpMenuVisibleSingle, ["TCPIP_USE_IGMP"])

	#Add to system_config.h
	tcpipIgmpHeaderFtl = tcpipIgmpComponent.createFileSymbol(None, None)
	tcpipIgmpHeaderFtl.setSourcePath("library/config/igmp.h.ftl")
	tcpipIgmpHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipIgmpHeaderFtl.setMarkup(True)
	tcpipIgmpHeaderFtl.setType("STRING")
	
	# Add igmp.h file to project
	tcpipIgmpHeaderFile = tcpipIgmpComponent.createFileSymbol(None, None)
	tcpipIgmpHeaderFile.setSourcePath("library/igmp.h")
	tcpipIgmpHeaderFile.setOutputName("igmp.h")
	tcpipIgmpHeaderFile.setDestPath("library/tcpip/")
	tcpipIgmpHeaderFile.setProjectPath("config/" + configName + "library/tcpip/")
	tcpipIgmpHeaderFile.setType("HEADER")
	tcpipIgmpHeaderFile.setOverwrite(True)

	# Add igmp_manager.h file to project
	tcpipIgmpManagerHeaderFile = tcpipIgmpComponent.createFileSymbol(None, None)
	tcpipIgmpManagerHeaderFile.setSourcePath("library/src/igmp_manager.h")
	tcpipIgmpManagerHeaderFile.setOutputName("igmp_manager.h")
	tcpipIgmpManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipIgmpManagerHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipIgmpManagerHeaderFile.setType("HEADER")
	tcpipIgmpManagerHeaderFile.setOverwrite(True)

	# Add igmp_private.h file to project
	tcpipIgmpPrivateHeaderFile = tcpipIgmpComponent.createFileSymbol(None, None)
	tcpipIgmpPrivateHeaderFile.setSourcePath("library/src/igmp_private.h")
	tcpipIgmpPrivateHeaderFile.setOutputName("igmp_private.h")
	tcpipIgmpPrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipIgmpPrivateHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipIgmpPrivateHeaderFile.setType("HEADER")
	tcpipIgmpPrivateHeaderFile.setOverwrite(True)

	# Add igmp.c file
	tcpipIgmpSourceFile = tcpipIgmpComponent.createFileSymbol(None, None)
	tcpipIgmpSourceFile.setSourcePath("library/src/igmp.c")
	tcpipIgmpSourceFile.setOutputName("igmp.c")
	tcpipIgmpSourceFile.setOverwrite(True)
	tcpipIgmpSourceFile.setDestPath("library/tcpip/src/")
	tcpipIgmpSourceFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipIgmpSourceFile.setType("SOURCE")
	tcpipIgmpSourceFile.setEnabled(True)
	tcpipIgmpSourceFile.setDependencies(tcpipIgmpGenSourceFile, ["TCPIP_USE_IGMP"])

def tcpipIgmpMenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("IGMP Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("IGMP Menu Invisible.")
		symbol.setVisible(False)

def tcpipIgmpGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])