FS_DRIVE = ["FLASH", "SDCARD", "FLASH & SDCARD"]

def instantiateComponent(tcpipSysFsWrapperComponent):
	print("TCPIP Sys FS Wrapper Component")
	configName = Variables.get("__CONFIGURATION_NAME")	
	
	# Enable TCPIP File System Wrapper Menu
	tcpipFileSysWrapper= tcpipSysFsWrapperComponent.createMenuSymbol(None, None)
	tcpipFileSysWrapper.setLabel("TCPIP File System Wrapper")
	tcpipFileSysWrapper.setVisible(True) # niyas to change the visibility to false
	tcpipFileSysWrapper.setDescription("TCPIP File System Wrapper Menu")
	tcpipFileSysWrapper.setDependencies(tcpipSysFsWrapperVisible, ["tcpipTcp.TCPIP_USE_TCP" , "tcpipUdp.TCPIP_USE_UDP"])

	# Use TCPIP File System Wrapper
	tcpipSysFsWrapper = tcpipSysFsWrapperComponent.createBooleanSymbol("TCPIP_STACK_USE_FS_WRAPPER", tcpipFileSysWrapper)
	tcpipSysFsWrapper.setVisible(False)
	tcpipSysFsWrapper.setDescription("Use TCPIP File System Wrapper")
	tcpipSysFsWrapper.setDefaultValue(False) 
	tcpipSysFsWrapper.setDependencies(tcpipSysFsWrapperNeeded, ["tcpipSnmp.TCPIP_USE_SNMP","tcpipHttp.TCPIP_STACK_USE_HTTP_SERVER","tcpipHttpNet.TCPIP_STACK_USE_HTTP_NET_SERVER", "tcpipFtps.TCPIP_USE_FTP_MODULE","tcpipTftpc.TCPIP_USE_TFTPC_MODULE"])
	
	# Maximum Length of Full Web Path
	tcpipSysFsPathLenMax = tcpipSysFsWrapperComponent.createIntegerSymbol("TCPIP_SYS_FS_MAX_PATH", tcpipFileSysWrapper)
	tcpipSysFsPathLenMax.setLabel("Max Length of Full Web Path")
	tcpipSysFsPathLenMax.setVisible(False)
	tcpipSysFsPathLenMax.setDescription("Maximum Length of Full Web Path")
	tcpipSysFsPathLenMax.setDefaultValue(80)
	tcpipSysFsPathLenMax.setDependencies(tcpipSysFsWrapperMenuVisibleSingle, ["TCPIP_STACK_USE_FS_WRAPPER"])

	# Web Server Mount Path
	tcpipSysFsLocalWebsitePath = tcpipSysFsWrapperComponent.createStringSymbol("TCPIP_LOCAL_WEBSITE_PATH", tcpipFileSysWrapper)
	tcpipSysFsLocalWebsitePath.setLabel("Web Server Mount Path")
	tcpipSysFsLocalWebsitePath.setVisible(False)
	tcpipSysFsLocalWebsitePath.setDescription("Web Server Mount Path")
	tcpipSysFsLocalWebsitePath.setDefaultValue("/mnt/mchpSite1")
	tcpipSysFsLocalWebsitePath.setDependencies(tcpipSysFsWrapperMenuVisibleSingle, ["TCPIP_STACK_USE_FS_WRAPPER"])

	# Memory Drive 
	tcpipSysFsDrive = tcpipSysFsWrapperComponent.createComboSymbol("TCPIP_SYS_FS_DRIVE", tcpipFileSysWrapper, FS_DRIVE)
	tcpipSysFsDrive.setLabel("Memory Drive")
	tcpipSysFsDrive.setVisible(False)
	tcpipSysFsDrive.setDescription("Memory Drive")
	tcpipSysFsDrive.setDefaultValue("FLASH")
	# Niyas to do
	  # default "FLASH" if SYS_FS_MPFS = y
	  # default "SDCARD" if SYS_FS_FAT = y
	tcpipSysFsDrive.setDependencies(tcpipSysFsWrapperMenuVisibleSingle, ["TCPIP_STACK_USE_FS_WRAPPER"])

	# NVM Disk Path
	tcpipSysFsNvmVol = tcpipSysFsWrapperComponent.createStringSymbol("TCPIP_SYS_FS_NVM_VOL", tcpipFileSysWrapper)
	tcpipSysFsNvmVol.setLabel("NVM Disk Path")
	tcpipSysFsNvmVol.setVisible(False)
	tcpipSysFsNvmVol.setDescription("NVM Disk Path")
	tcpipSysFsNvmVol.setDefaultValue("/dev/nvma1")
	tcpipSysFsNvmVol.setDependencies(tcpipSysFsWrapperMenuVisibleSingle, ["TCPIP_STACK_USE_FS_WRAPPER"])

	# SDCARD Disk Path
	tcpipSysFsSdVol = tcpipSysFsWrapperComponent.createStringSymbol("TCPIP_SYS_FS_SD_VOL", tcpipFileSysWrapper)
	tcpipSysFsSdVol.setLabel("SDCARD Disk Path")
	tcpipSysFsSdVol.setVisible(False)
	tcpipSysFsSdVol.setDescription("SDCARD Disk Path")
	tcpipSysFsSdVol.setDefaultValue("/dev/mmcblka1")
	tcpipSysFsSdVol.setDependencies(tcpipSysFsWrapperMenuVisibleSingle, ["TCPIP_STACK_USE_FS_WRAPPER"])

	# File system MPFS String
	tcpipSysFsMpfsStr = tcpipSysFsWrapperComponent.createStringSymbol("TCPIP_SYS_FS_MPFS_STRING", tcpipFileSysWrapper)
	tcpipSysFsMpfsStr.setVisible(False)
	tcpipSysFsMpfsStr.setDescription("File system MPFS String")
	tcpipSysFsMpfsStr.setDefaultValue("MPFS2")
	tcpipSysFsMpfsStr.setDependencies(tcpipSysFsWrapperMenuVisibleSingle, ["TCPIP_STACK_USE_FS_WRAPPER"])

	# File system FATFS String
	tcpipSysFsFatfsStr = tcpipSysFsWrapperComponent.createStringSymbol("TCPIP_SYS_FS_FATFS_STRING", tcpipFileSysWrapper)
	tcpipSysFsFatfsStr.setVisible(False)
	tcpipSysFsFatfsStr.setDescription("File system FATFS String")
	tcpipSysFsFatfsStr.setDefaultValue("FATFS")
	tcpipSysFsFatfsStr.setDependencies(tcpipSysFsWrapperMenuVisibleSingle, ["TCPIP_STACK_USE_FS_WRAPPER"])

	#Add to system_config.h
	tcpipSysFsHeaderFtl = tcpipSysFsWrapperComponent.createFileSymbol(None, None)
	tcpipSysFsHeaderFtl.setSourcePath("library/config/sys_fs_wrapper.h.ftl")
	tcpipSysFsHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipSysFsHeaderFtl.setMarkup(True)
	tcpipSysFsHeaderFtl.setType("STRING")
	
	# Add sys_fs_wrapper.h file to project
	tcpipSysFsWrapperHeaderFile = tcpipSysFsWrapperComponent.createFileSymbol(None, None)
	tcpipSysFsWrapperHeaderFile.setSourcePath("library/src/common/sys_fs_wrapper.h")
	tcpipSysFsWrapperHeaderFile.setOutputName("sys_fs_wrapper.h")
	tcpipSysFsWrapperHeaderFile.setDestPath("library/tcpip/src/common/")
	tcpipSysFsWrapperHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/common/")
	tcpipSysFsWrapperHeaderFile.setType("HEADER")
	tcpipSysFsWrapperHeaderFile.setOverwrite(True)

	# Add sys_fs_wrapper.c file
	tcpipSysFsWrapperSourceFile = tcpipSysFsWrapperComponent.createFileSymbol(None, None)
	tcpipSysFsWrapperSourceFile.setSourcePath("library/src/common/sys_fs_wrapper.c")
	tcpipSysFsWrapperSourceFile.setOutputName("sys_fs_wrapper.c")
	tcpipSysFsWrapperSourceFile.setOverwrite(True)
	tcpipSysFsWrapperSourceFile.setDestPath("library/tcpip/src/common/")
	tcpipSysFsWrapperSourceFile.setProjectPath("config/" + configName + "library/tcpip/src/common/")
	tcpipSysFsWrapperSourceFile.setType("SOURCE")
	tcpipSysFsWrapperSourceFile.setEnabled(False)
	tcpipSysFsWrapperSourceFile.setDependencies(tcpipSysFsWrapperGenSourceFile, ["TCPIP_STACK_USE_FS_WRAPPER"])

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
		
def tcpipSysFsWrapperGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])		