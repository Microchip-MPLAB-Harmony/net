FS_DRIVE = ["FLASH", "SDCARD", "FLASH & SDCARD"]

# make sys_fs_wrapper options visible
def tcpipSysFsWrapperVisible(tcpipDependentSymbol, tcpipIPSymbol):	
	tcpipTcp = Database.getSymbolValue("tcpip","TCPIP_USE_TCP")
	tcpipUdp= Database.getSymbolValue("tcpip","TCPIP_USE_UDP")

	if(tcpipTcp and tcpipUdp):
		tcpipDependentSymbol.setVisible(True)
	else:
		tcpipDependentSymbol.setVisible(False)	
		
# Enable TCPIP File System Wrapper Menu
tcpipFileSysWrapper= tcpipComponent.createMenuSymbol(None, None)
tcpipFileSysWrapper.setLabel("TCPIP File System Wrapper")
tcpipFileSysWrapper.setVisible(True) # niyas to change the visibility to false
tcpipFileSysWrapper.setDescription("TCPIP File System Wrapper Menu")
tcpipFileSysWrapper.setDependencies(tcpipSysFsWrapperVisible, ["TCPIP_USE_TCP" , "TCPIP_USE_UDP"])

# Use TCPIP File System Wrapper
tcpipSysFsWrapper = tcpipComponent.createBooleanSymbol("TCPIP_STACK_USE_FS_WRAPPER", tcpipFileSysWrapper)
tcpipSysFsWrapper.setVisible(False)
tcpipSysFsWrapper.setDescription("Use TCPIP File System Wrapper")
tcpipSysFsWrapper.setDefaultValue(True) 

# Maximum Length of Full Web Path
tcpipSysFsPathLenMax = tcpipComponent.createIntegerSymbol("TCPIP_SYS_FS_MAX_PATH", tcpipFileSysWrapper)
tcpipSysFsPathLenMax.setLabel("Max Length of Full Web Path")
tcpipSysFsPathLenMax.setVisible(True)
tcpipSysFsPathLenMax.setDescription("Maximum Length of Full Web Path")
tcpipSysFsPathLenMax.setDefaultValue(80)
tcpipSysFsPathLenMax.setDependencies(tcpipMenuVisibleSingleTrigger, ["TCPIP_STACK_USE_FS_WRAPPER"])

# Web Server Mount Path
tcpipSysFsLocalWebsitePath = tcpipComponent.createStringSymbol("TCPIP_LOCAL_WEBSITE_PATH", tcpipFileSysWrapper)
tcpipSysFsLocalWebsitePath.setLabel("Web Server Mount Path")
tcpipSysFsLocalWebsitePath.setVisible(True)
tcpipSysFsLocalWebsitePath.setDescription("Web Server Mount Path")
tcpipSysFsLocalWebsitePath.setDefaultValue("/mnt/mchpSite1")
tcpipSysFsLocalWebsitePath.setDependencies(tcpipMenuVisibleSingleTrigger, ["TCPIP_STACK_USE_FS_WRAPPER"])

# Memory Drive 
tcpipSysFsDrive = tcpipComponent.createComboSymbol("TCPIP_SYS_FS_DRIVE", tcpipFileSysWrapper, FS_DRIVE)
tcpipSysFsDrive.setLabel("Memory Drive")
tcpipSysFsDrive.setVisible(True)
tcpipSysFsDrive.setDescription("Memory Drive")
tcpipSysFsDrive.setDefaultValue("FLASH")
# Niyas to do
  # default "FLASH" if SYS_FS_MPFS = y
  # default "SDCARD" if SYS_FS_FAT = y
tcpipSysFsDrive.setDependencies(tcpipMenuVisibleSingleTrigger, ["TCPIP_STACK_USE_FS_WRAPPER"])

# NVM Disk Path
tcpipSysFsNvmVol = tcpipComponent.createStringSymbol("TCPIP_SYS_FS_NVM_VOL", tcpipFileSysWrapper)
tcpipSysFsNvmVol.setLabel("NVM Disk Path")
tcpipSysFsNvmVol.setVisible(True)
tcpipSysFsNvmVol.setDescription("NVM Disk Path")
tcpipSysFsNvmVol.setDefaultValue("/dev/nvma1")
tcpipSysFsNvmVol.setDependencies(tcpipMenuVisibleSingleTrigger, ["TCPIP_STACK_USE_FS_WRAPPER"])

# SDCARD Disk Path
tcpipSysFsSdVol = tcpipComponent.createStringSymbol("TCPIP_SYS_FS_SD_VOL", tcpipFileSysWrapper)
tcpipSysFsSdVol.setLabel("SDCARD Disk Path")
tcpipSysFsSdVol.setVisible(True)
tcpipSysFsSdVol.setDescription("SDCARD Disk Path")
tcpipSysFsSdVol.setDefaultValue("/dev/mmcblka1")
tcpipSysFsSdVol.setDependencies(tcpipMenuVisibleSingleTrigger, ["TCPIP_STACK_USE_FS_WRAPPER"])

# File system MPFS String
tcpipSysFsMpfsStr = tcpipComponent.createStringSymbol("TCPIP_SYS_FS_MPFS_STRING", tcpipFileSysWrapper)
tcpipSysFsMpfsStr.setVisible(False)
tcpipSysFsMpfsStr.setDescription("File system MPFS String")
tcpipSysFsMpfsStr.setDefaultValue("MPFS2")
tcpipSysFsMpfsStr.setDependencies(tcpipMenuVisibleSingleTrigger, ["TCPIP_STACK_USE_FS_WRAPPER"])

# File system FATFS String
tcpipSysFsFatfsStr = tcpipComponent.createStringSymbol("TCPIP_SYS_FS_FATFS_STRING", tcpipFileSysWrapper)
tcpipSysFsFatfsStr.setVisible(False)
tcpipSysFsFatfsStr.setDescription("File system FATFS String")
tcpipSysFsFatfsStr.setDefaultValue("FATFS")
tcpipSysFsFatfsStr.setDependencies(tcpipMenuVisibleSingleTrigger, ["TCPIP_STACK_USE_FS_WRAPPER"])


# Add sys_fs_wrapper.h file to project
tcpipSysFsWrapperHeaderFile = tcpipComponent.createFileSymbol(None, None)
tcpipSysFsWrapperHeaderFile.setSourcePath("library/sys_fs_wrapper.h")
tcpipSysFsWrapperHeaderFile.setOutputName("sys_fs_wrapper.h")
tcpipSysFsWrapperHeaderFile.setDestPath("library/tcpip/")
tcpipSysFsWrapperHeaderFile.setProjectPath("config/" + configName + "library/tcpip/")
tcpipSysFsWrapperHeaderFile.setType("HEADER")
tcpipSysFsWrapperHeaderFile.setOverwrite(True)

# Add sys_fs_wrapper.c file
tcpipSysFsWrapperSourceFile = tcpipComponent.createFileSymbol(None, None)
tcpipSysFsWrapperSourceFile.setSourcePath("library/src/sys_fs_wrapper.c")
tcpipSysFsWrapperSourceFile.setOutputName("sys_fs_wrapper.c")
tcpipSysFsWrapperSourceFile.setOverwrite(True)
tcpipSysFsWrapperSourceFile.setDestPath("library/tcpip/src/")
tcpipSysFsWrapperSourceFile.setProjectPath("config/" + configName + "library/tcpip/src/")
tcpipSysFsWrapperSourceFile.setType("SOURCE")
tcpipSysFsWrapperSourceFile.setEnabled(False)
tcpipSysFsWrapperSourceFile.setDependencies(tcpipGenSourceFile, ["TCPIP_STACK_USE_FS_WRAPPER"])