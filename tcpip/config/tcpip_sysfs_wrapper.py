FS_DRIVE = ["FLASH", "SDCARD", "FLASH & SDCARD"]

def instantiateComponent(tcpipSysFsWrapperComponent):
	print("TCPIP Sys FS Wrapper Component")
	configName = Variables.get("__CONFIGURATION_NAME")	
	
	# Enable TCPIP File System Wrapper Menu
	tcpipFileSysWrapper= tcpipSysFsWrapperComponent.createMenuSymbol(None, None)
	tcpipFileSysWrapper.setLabel("TCPIP File System Wrapper")
	tcpipFileSysWrapper.setVisible(False)
	tcpipFileSysWrapper.setDescription("TCPIP File System Wrapper Menu")
	#tcpipFileSysWrapper.setDependencies(tcpipSysFsWrapperVisible, ["tcpipTcp.TCPIP_USE_TCP" , "tcpipUdp.TCPIP_USE_UDP"])

	# Use TCPIP File System Wrapper
	tcpipSysFsWrapper = tcpipSysFsWrapperComponent.createBooleanSymbol("TCPIP_STACK_USE_FS_WRAPPER", None)
	tcpipSysFsWrapper.setVisible(False)
	tcpipSysFsWrapper.setDescription("Use TCPIP File System Wrapper")
	tcpipSysFsWrapper.setDefaultValue(True) 
	#tcpipSysFsWrapper.setDependencies(tcpipSysFsWrapperNeeded, ["tcpipSnmp.TCPIP_USE_SNMP","tcpipHttp.TCPIP_STACK_USE_HTTP_SERVER","tcpipHttpNet.TCPIP_STACK_USE_HTTP_NET_SERVER", "tcpipFtps.TCPIP_USE_FTP_MODULE","tcpipTftpc.TCPIP_USE_TFTPC_MODULE"])
	
	# Maximum Length of Full Web Path
	tcpipSysFsPathLenMax = tcpipSysFsWrapperComponent.createIntegerSymbol("TCPIP_SYS_FS_MAX_PATH", None)
	tcpipSysFsPathLenMax.setLabel("Max Length of Full Web Path")
	tcpipSysFsPathLenMax.setVisible(True)
	tcpipSysFsPathLenMax.setDescription("Maximum Length of Full Web Path")
	tcpipSysFsPathLenMax.setDefaultValue(80)
	#tcpipSysFsPathLenMax.setDependencies(tcpipSysFsWrapperMenuVisibleSingle, ["TCPIP_STACK_USE_FS_WRAPPER"])

	# Web Server Mount Path
	tcpipSysFsLocalWebsitePath = tcpipSysFsWrapperComponent.createStringSymbol("TCPIP_LOCAL_WEBSITE_PATH", None)
	tcpipSysFsLocalWebsitePath.setLabel("Web Server Mount Path")
	tcpipSysFsLocalWebsitePath.setVisible(True)
	tcpipSysFsLocalWebsitePath.setDescription("Web Server Mount Path")
	tcpipSysFsLocalWebsitePath.setDefaultValue("/mnt/mchpSite1")
	#tcpipSysFsLocalWebsitePath.setDependencies(tcpipSysFsWrapperMenuVisibleSingle, ["TCPIP_STACK_USE_FS_WRAPPER"])

	# Memory Drive 
	tcpipSysFsDrive = tcpipSysFsWrapperComponent.createComboSymbol("TCPIP_SYS_FS_DRIVE", None, FS_DRIVE)
	tcpipSysFsDrive.setLabel("Memory Drive")
	tcpipSysFsDrive.setVisible(True)
	tcpipSysFsDrive.setDescription("Memory Drive")
	tcpipSysFsDrive.setDefaultValue("SDCARD")
	tcpipSysFsDrive.setDependencies(tcpipSysFsWrapperMemDrive, ["sys_fs.SYS_FS_FAT" , "sys_fs.SYS_FS_MPFS"])

	# NVM Disk Path
	tcpipSysFsNvmVol = tcpipSysFsWrapperComponent.createStringSymbol("TCPIP_SYS_FS_NVM_VOL", None)
	tcpipSysFsNvmVol.setLabel("NVM Disk Path")
	tcpipSysFsNvmVol.setVisible(True)
	tcpipSysFsNvmVol.setDescription("NVM Disk Path")
	tcpipSysFsNvmVol.setDefaultValue("/dev/nvma1")
	#tcpipSysFsNvmVol.setDependencies(tcpipSysFsWrapperMenuVisibleSingle, ["TCPIP_STACK_USE_FS_WRAPPER"])

	# SDCARD Disk Path
	tcpipSysFsSdVol = tcpipSysFsWrapperComponent.createStringSymbol("TCPIP_SYS_FS_SD_VOL", None)
	tcpipSysFsSdVol.setLabel("SDCARD Disk Path")
	tcpipSysFsSdVol.setVisible(True)
	tcpipSysFsSdVol.setDescription("SDCARD Disk Path")
	tcpipSysFsSdVol.setDefaultValue("/dev/mmcblka1")
	#tcpipSysFsSdVol.setDependencies(tcpipSysFsWrapperMenuVisibleSingle, ["TCPIP_STACK_USE_FS_WRAPPER"])

	# File system MPFS String
	tcpipSysFsMpfsStr = tcpipSysFsWrapperComponent.createStringSymbol("TCPIP_SYS_FS_MPFS_STRING", None)
	tcpipSysFsMpfsStr.setVisible(True)
	tcpipSysFsMpfsStr.setDescription("File system MPFS String")
	tcpipSysFsMpfsStr.setDefaultValue("MPFS2")
	#tcpipSysFsMpfsStr.setDependencies(tcpipSysFsWrapperMenuVisibleSingle, ["TCPIP_STACK_USE_FS_WRAPPER"])

	# File system FATFS String
	tcpipSysFsFatfsStr = tcpipSysFsWrapperComponent.createStringSymbol("TCPIP_SYS_FS_FATFS_STRING", None)
	tcpipSysFsFatfsStr.setVisible(True)
	tcpipSysFsFatfsStr.setDescription("File system FATFS String")
	tcpipSysFsFatfsStr.setDefaultValue("FATFS")
	#tcpipSysFsFatfsStr.setDependencies(tcpipSysFsWrapperMenuVisibleSingle, ["TCPIP_STACK_USE_FS_WRAPPER"])

	#Add to system_config.h
	tcpipSysFsHeaderFtl = tcpipSysFsWrapperComponent.createFileSymbol(None, None)
	tcpipSysFsHeaderFtl.setSourcePath("tcpip/config/sys_fs_wrapper.h.ftl")
	tcpipSysFsHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipSysFsHeaderFtl.setMarkup(True)
	tcpipSysFsHeaderFtl.setType("STRING")

	# Add sys_fs_wrapper.c file
	tcpipSysFsWrapperSourceFile = tcpipSysFsWrapperComponent.createFileSymbol(None, None)
	tcpipSysFsWrapperSourceFile.setSourcePath("tcpip/src/common/sys_fs_wrapper.c")
	tcpipSysFsWrapperSourceFile.setOutputName("sys_fs_wrapper.c")
	tcpipSysFsWrapperSourceFile.setOverwrite(True)
	tcpipSysFsWrapperSourceFile.setDestPath("library/tcpip/src/common/")
	tcpipSysFsWrapperSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/common/")
	tcpipSysFsWrapperSourceFile.setType("SOURCE")
	tcpipSysFsWrapperSourceFile.setEnabled(True)
	#tcpipSysFsWrapperSourceFile.setDependencies(tcpipSysFsWrapperGenSourceFile, ["TCPIP_STACK_USE_FS_WRAPPER"])

def tcpipSysFsWrapperNeeded(symbol, event):	
	tcpipSnmp = Database.getSymbolValue("tcpipSnmp","TCPIP_USE_SNMP")
	tcpipHttp = Database.getSymbolValue("tcpipHttp","TCPIP_STACK_USE_HTTP_SERVER")
	tcpipHttpNet = Database.getSymbolValue("tcpipHttpNet","TCPIP_STACK_USE_HTTP_NET_SERVER")
	tcpipFtps = Database.getSymbolValue("tcpipFtps","TCPIP_USE_FTP_MODULE")
	tcpipTftpc = Database.getSymbolValue("tcpipTftpc","TCPIP_USE_TFTPC_MODULE")

	if(tcpipSnmp or tcpipHttp or tcpipHttpNet or tcpipFtps or tcpipTftpc):
		print("SysFsWrapper Needed.")
		symbol.setVisible(True)
		symbol.setValue(True, 1)
	else:
		print("SysFsWrapper Not Needed.")
		symbol.setVisible(False)
		symbol.setValue(False, 1)
		
# make sys_fs_wrapper options visible
def tcpipSysFsWrapperVisible(tcpipDependentSymbol, tcpipIPSymbol):	
	tcpipTcp = Database.getSymbolValue("tcpipTcp","TCPIP_USE_TCP")
	tcpipUdp= Database.getSymbolValue("tcpipUdp","TCPIP_USE_UDP")
	
	if(tcpipTcp and tcpipUdp):
		tcpipDependentSymbol.setVisible(True)
	else:
		tcpipDependentSymbol.setVisible(False)
		
def tcpipSysFsWrapperMenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("SysFsWrapper Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("SysFsWrapper Menu Invisible.")
		symbol.setVisible(False)	
	
def tcpipSysFsWrapperMemDrive(symbol, event):
	sysFs_FAT = Database.getSymbolValue("sys_fs","SYS_FS_FAT")
	sysFs_MPFS = Database.getSymbolValue("sys_fs","SYS_FS_MPFS")
	if(sysFs_MPFS):
		symbol.setValue("FLASH", 2)
	elif(sysFs_FAT):
		symbol.setValue("SDCARD", 2)
	
		
def tcpipSysFsWrapperGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])		