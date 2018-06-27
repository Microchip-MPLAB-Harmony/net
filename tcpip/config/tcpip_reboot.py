    
def instantiateComponent(tcpipRebootComponent):
	print("TCPIP FTP Server Component")
	configName = Variables.get("__CONFIGURATION_NAME")
		
	# Use Reboot Server
	tcpipReboot = tcpipRebootComponent.createBooleanSymbol("TCPIP_USE_REBOOT_SERVER", None)
	tcpipReboot.setLabel("Use Reboot Server")
	tcpipReboot.setVisible(False)
	tcpipReboot.setDescription("Use Reboot Server")
	tcpipReboot.setDefaultValue(True)
	tcpipReboot.setDependencies(tcpipRebootServerMenuVisible, ["tcpipIPv4.TCPIP_STACK_USE_IPV4", "tcpipUdp.TCPIP_USE_UDP"])

	# Allow Only Same Subnet
	tcpipRebootAllowSameSubnet = tcpipRebootComponent.createBooleanSymbol("TCPIP_REBOOT_SAME_SUBNET_ONLY", None)
	tcpipRebootAllowSameSubnet.setLabel("Allow Only Same Subnet")
	tcpipRebootAllowSameSubnet.setVisible(True)
	tcpipRebootAllowSameSubnet.setDescription("Allow Only Same Subnet")
	tcpipRebootAllowSameSubnet.setDefaultValue(True)
	#tcpipRebootAllowSameSubnet.setDependencies(tcpipRebootMenuSingle, ["TCPIP_USE_REBOOT_SERVER"])

	# Reboot Message
	tcpipRebootMessage = tcpipRebootComponent.createStringSymbol("TCPIP_REBOOT_MESSAGE", None)
	tcpipRebootMessage.setLabel("Reboot Message")
	tcpipRebootMessage.setVisible(True)
	tcpipRebootMessage.setDescription("Reboot Message")
	tcpipRebootMessage.setDefaultValue("MCHP Reboot")
	#tcpipRebootMessage.setDependencies(tcpipRebootMenuSingle, ["TCPIP_USE_REBOOT_SERVER"])

	# Reboot Server Tick Rate in ms
	tcpipRebootTskTickRate = tcpipRebootComponent.createIntegerSymbol("TCPIP_REBOOT_TASK_TICK_RATE", None)
	tcpipRebootTskTickRate.setLabel("Reboot Server Tick Rate - ms")
	tcpipRebootTskTickRate.setVisible(True)
	tcpipRebootTskTickRate.setDescription("Reboot Server Tick Rate in ms")
	tcpipRebootTskTickRate.setDefaultValue(1130)
	#tcpipRebootTskTickRate.setDependencies(tcpipRebootMenuSingle, ["TCPIP_USE_REBOOT_SERVER"])

	#Add to system_config.h
	tcpipRebootHeaderFtl = tcpipRebootComponent.createFileSymbol(None, None)
	tcpipRebootHeaderFtl.setSourcePath("tcpip/config/tcpip_reboot.h.ftl")
	tcpipRebootHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipRebootHeaderFtl.setMarkup(True)
	tcpipRebootHeaderFtl.setType("STRING")

	# Add tcpip_reboot.c file
	tcpipRebootSourceFile = tcpipRebootComponent.createFileSymbol(None, None)
	tcpipRebootSourceFile.setSourcePath("tcpip/src/tcpip_reboot.c")
	tcpipRebootSourceFile.setOutputName("tcpip_reboot.c")
	tcpipRebootSourceFile.setOverwrite(True)
	tcpipRebootSourceFile.setDestPath("library/tcpip/src/")
	tcpipRebootSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipRebootSourceFile.setType("SOURCE")
	tcpipRebootSourceFile.setEnabled(True)
	#tcpipRebootSourceFile.setDependencies(tcpipRebootGenSourceFile, ["TCPIP_USE_REBOOT_SERVER"])

# make Reboot Server option visible
def tcpipRebootServerMenuVisible(symbol, event):
	tcpipIPv4 = Database.getSymbolValue("tcpipIPv4","TCPIP_STACK_USE_IPV4")
	tcpipUdp = Database.getSymbolValue("tcpipUdp","TCPIP_USE_UDP")

	if(tcpipIPv4 and tcpipUdp):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)
		
def tcpipRebootMenuSingle(symbol, event):
	if (event["value"] == True):
		print("Reboot Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("Reboot Menu Invisible.")
		symbol.setVisible(False)
		
		
def tcpipRebootGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])