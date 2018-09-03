TCPIP_STACK_HEAP_TYPE = ["TCPIP_STACK_HEAP_TYPE_EXTERNAL_HEAP", "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP", "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL"]
TCPIP_STACK_HEAP_FLAGS = ["TCPIP_STACK_HEAP_FLAG_NONE", "TCPIP_STACK_HEAP_FLAG_ALLOC_UNCACHED", "TCPIP_STACK_HEAP_FLAG_NO_MTHREAD_SYNC", "TCPIP_STACK_HEAP_FLAG_ALLOC_UNALIGN", "TCPIP_STACK_HEAP_FLAG_NO_WARN_MESSAGE"]
TCPIP_STACK_HEAP_USAGE = ["TCPIP_STACK_HEAP_USE_DEFAULT", "TCPIP_STACK_HEAP_USE_GENERIC", "TCPIP_STACK_HEAP_USE_PACKET", "TCPIP_STACK_HEAP_USE_SOCKET_TCP", "TCPIP_STACK_HEAP_USE_SOCKET_UDP","TCPIP_STACK_HEAP_USE_POOL_UDP"]

TCIPHEAP_POOL_DFLT_SIZE = [2064, 1616, 1024, 768, 512, 256, 128, 64, 32, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64]
TCIPHEAP_POOL_EXPBLK_NUM = [2, 2, 2, 2, 2, 4, 4, 8, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

tcpipStackHeapPoolNumMaximum = 20
tcpipPoolEntryInstancesNumPrev = 9
tcpipStackHeapPoolEntry = []
tcpipStackHeapPoolBlkSize = []
tcpipStackHeapPoolPreAllocBlkNum = []
tcpipStackHeapPoolExpBlkNum = []
#########################################################################################

def instantiateComponent(tcpipStackComponent):
	print("TCPIP main Stack Component")
	configName = Variables.get("__CONFIGURATION_NAME")

	# Enable dependent Harmony core components
	Database.clearSymbolValue("Harmony", "ENABLE_SYS_COMMON")
	Database.setSymbolValue("Harmony", "ENABLE_SYS_COMMON", True, 2)
	
	Database.clearSymbolValue("Harmony", "ENABLE_DRV_COMMON")
	Database.setSymbolValue("Harmony", "ENABLE_DRV_COMMON", True, 2)
	
	Database.clearSymbolValue("Harmony", "ENABLE_SYS_INT")
	Database.setSymbolValue("Harmony", "ENABLE_SYS_INT", True, 2)

	Database.clearSymbolValue("Harmony", "ENABLE_OSAL")
	Database.setSymbolValue("Harmony", "ENABLE_OSAL", True, 2)

	Database.clearSymbolValue("Harmony", "ENABLE_APP_FILE")
	Database.setSymbolValue("Harmony", "ENABLE_APP_FILE", True, 2)
	
	# TCP/IP Stack Enable 
	tcpipStackEnable = tcpipStackComponent.createBooleanSymbol("USE_TCPIP_STACK", None)
	tcpipStackEnable.setLabel("Use TCP/IP Stack?")
	tcpipStackEnable.setDescription("Enables TCP/IP stack")
	tcpipStackEnable.setReadOnly(True)
	tcpipStackEnable.setDefaultValue(True)
	tcpipStackEnable.setVisible(False)
	
	# Define number of network Configurations
	tcpipStackTickRate = tcpipStackComponent.createIntegerSymbol("TCPIP_STACK_TICK_RATE", None)
	tcpipStackTickRate.setLabel("TCP/IP Stack State Machine Tick Rate, ms.")
	tcpipStackTickRate.setVisible(True)
	tcpipStackTickRate.setDescription("TCP/IP Stack State Machine Tick Rate")
	tcpipStackTickRate.setDefaultValue(5)
	
	# Event Notification
	tcpipStackEventNotify = tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_EVENT_NOTIFICATION", None)
	tcpipStackEventNotify.setVisible(False)
	tcpipStackEventNotify.setDefaultValue(True)
	
	# Enable User Notification
	tcpipStackUserNotify= tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_USER_NOTIFICATION", None)
	tcpipStackUserNotify.setLabel("Enable User Notification")
	tcpipStackUserNotify.setVisible(True)
	tcpipStackUserNotify.setDescription("Enable User Notification")
	tcpipStackUserNotify.setDefaultValue(False)
	tcpipStackUserNotify.setDependencies(tcpipStackMenuVisible, ["TCPIP_STACK_EVENT_NOTIFICATION"])

	# Enable Deinitialize of TCP/IP stack
	tcpipStackDown= tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_DOWN_OPERATION", None)
	tcpipStackDown.setLabel("Enable Stack Deinitialize Operations")
	tcpipStackDown.setVisible(True)
	tcpipStackDown.setDescription("Enable Deinitialize of TCP/IP stack")
	tcpipStackDown.setDefaultValue(True)
	
	# Enable stack up/down of TCP/IP stack
	tcpipStackIfUpDown= tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_IF_UP_DOWN_OPERATION", tcpipStackDown)
	tcpipStackIfUpDown.setLabel("Enable IF Up/Down Operations")
	tcpipStackIfUpDown.setVisible(True)
	tcpipStackIfUpDown.setDescription("Enable interface up/down of TCP/IP stack")
	tcpipStackIfUpDown.setDefaultValue(True)
	tcpipStackIfUpDown.setDependencies(tcpipStackMenuVisible, ["TCPIP_STACK_DOWN_OPERATION"])

	# Enable stack Mac Down
	tcpipStackMacDown= tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_MAC_DOWN_OPERATION", None)
	tcpipStackMacDown.setLabel("Enable MAC Down on Failure")
	tcpipStackMacDown.setVisible(False)
	tcpipStackMacDown.setDescription("Enable MAC Down on Failure")
	tcpipStackMacDown.setDefaultValue(True)
	tcpipStackMacDown.setDependencies(tcpipStackMenuInvisible, ["TCPIP_STACK_DOWN_OPERATION"])

	# Enable Interface Address Change Signaling
	tcpipStackIfChange= tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_INTERFACE_CHANGE_SIGNALING", None)
	tcpipStackIfChange.setLabel("Enable Interface Address Change Signaling")
	tcpipStackIfChange.setVisible(True)
	tcpipStackIfChange.setDescription("Enable Interface Address Change Signaling")
	tcpipStackIfChange.setDefaultValue(False)
	tcpipStackIfChange.setDependencies(tcpipStackMenuVisible, ["tcpipIPv4.TCPIP_STACK_USE_IPV4"])

	# Enable Configuration Save/Restore Functionality
	tcpipStackConfigSaveRestore= tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_CONFIGURATION_SAVE_RESTORE", None)
	tcpipStackConfigSaveRestore.setLabel("Enable Configuration Save/Restore Functionality")
	tcpipStackConfigSaveRestore.setVisible(True)
	tcpipStackConfigSaveRestore.setDescription("Enable Configuration Save/Restore Functionality")
	tcpipStackConfigSaveRestore.setDefaultValue(True)	
		
	# Enable File System Wrapper
	#config TCPIP_SYS_FS_WRAPPER_NEEDED. This logic implemented in tcpip_sysfs_wrapper.py

	# Define number of entries in the secure port table
	tcpipStackSecPortNum = tcpipStackComponent.createIntegerSymbol("TCPIP_STACK_SECURE_PORT_ENTRIES", None)
	tcpipStackSecPortNum.setLabel("Number of entries in the secure port table")
	tcpipStackSecPortNum.setVisible(True)
	tcpipStackSecPortNum.setDescription("Number of entries in the secure port table")
	tcpipStackSecPortNum.setDefaultValue(10)

	# Enable Alias Interfaces Support
	tcpipStackAliasInterface= tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_ALIAS_INTERFACE_SUPPORT", None)
	tcpipStackAliasInterface.setLabel("Enable Alias Interfaces Support")
	tcpipStackAliasInterface.setVisible(True)
	tcpipStackAliasInterface.setDescription("Enable Alias Interfaces Support")
	tcpipStackAliasInterface.setDefaultValue(False)

	# Enable Packet Logger
	tcpipStackPktLogEnable= tcpipStackComponent.createBooleanSymbol("TCPIP_PACKET_LOG_ENABLE", None)
	tcpipStackPktLogEnable.setLabel("Enable Packet Logger")
	tcpipStackPktLogEnable.setVisible(True)
	tcpipStackPktLogEnable.setDescription("Enable Packet Logger")
	tcpipStackPktLogEnable.setDefaultValue(False)

	# Number of Entries in the Packet Logger
	tcpipStackPktLogSize= tcpipStackComponent.createIntegerSymbol("TCPIP_PKT_LOG_SIZE", tcpipStackPktLogEnable)
	tcpipStackPktLogSize.setLabel("Number of Entries in the Packet Logger")
	tcpipStackPktLogSize.setVisible(False)
	tcpipStackPktLogSize.setDescription("Number of Entries in the Packet Logger")
	tcpipStackPktLogSize.setDefaultValue(40)
	tcpipStackPktLogSize.setDependencies(tcpipStackMenuVisible, ["TCPIP_PACKET_LOG_ENABLE"])	
	
	# RTOS Configuration
	tcpipStackRtosMenu = tcpipStackComponent.createMenuSymbol("TCPIP_STACK_RTOS_MENU", None)
	tcpipStackRtosMenu.setLabel("RTOS Configuration")
	tcpipStackRtosMenu.setDescription("RTOS Configuration")
	tcpipStackRtosMenu.setVisible(False)
	tcpipStackRtosMenu.setVisible((Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != "BareMetal") and (Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != None))
	tcpipStackRtosMenu.setDependencies(tcpipStackshowRTOSMenu, ["HarmonyCore.SELECT_RTOS"])

	# Menu for RTOS options
	tcpipStackInstnExecMode = tcpipStackComponent.createComboSymbol("TCPIP_STACK_RTOS", tcpipStackRtosMenu, ["Standalone"]) 
	tcpipStackInstnExecMode.setLabel("Run Library Tasks as")
	tcpipStackInstnExecMode.setVisible(False)
	tcpipStackInstnExecMode.setDescription("Rtos Options")
	tcpipStackInstnExecMode.setDefaultValue("Standalone")
	
	# RTOS Task Size
	tcpipStackTaskSize = tcpipStackComponent.createIntegerSymbol("TCPIP_STACK_RTOS_TASK_SIZE", tcpipStackRtosMenu)
	tcpipStackTaskSize.setLabel("Task Size")
	tcpipStackTaskSize.setVisible(True)
	tcpipStackTaskSize.setDescription("Rtos Task Size")
	tcpipStackTaskSize.setDefaultValue(1024)
	tcpipStackTaskSize.setDependencies(tcpipStackRTOSStandaloneMenu, ["TCPIP_STACK_RTOS"])
	
	# RTOS Task Priority
	tcpipStackTaskPriority = tcpipStackComponent.createIntegerSymbol("TCPIP_STACK_RTOS_TASK_PRIORITY", tcpipStackRtosMenu)
	tcpipStackTaskPriority.setLabel("Task Priority")
	tcpipStackTaskPriority.setVisible(True)
	tcpipStackTaskPriority.setDescription("Rtos Task Priority")
	tcpipStackTaskPriority.setDefaultValue(1)
	tcpipStackTaskPriority.setDependencies(tcpipStackRTOSStandaloneMenu, ["TCPIP_STACK_RTOS"])
	
	# RTOS Use Task Delay?
	tcpipStackUseTaskDelay = tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_RTOS_USE_DELAY", tcpipStackRtosMenu)
	tcpipStackUseTaskDelay.setLabel("Use Task Delay?")
	tcpipStackUseTaskDelay.setVisible(True)
	tcpipStackUseTaskDelay.setDescription("Rtos Use Task Delay?")
	tcpipStackUseTaskDelay.setDefaultValue(True)
	tcpipStackUseTaskDelay.setDependencies(tcpipStackRTOSStandaloneMenu, ["TCPIP_STACK_RTOS"])

	# RTOS Task Delay
	tcpipStackTaskDelay = tcpipStackComponent.createIntegerSymbol("TCPIP_STACK_RTOS_DELAY", tcpipStackRtosMenu)
	tcpipStackTaskDelay.setLabel("Task Delay")
	tcpipStackTaskDelay.setVisible(True)
	tcpipStackTaskDelay.setDescription("MIIM Driver Task Delay")
	tcpipStackTaskDelay.setDefaultValue(100)
	tcpipStackTaskDelay.setDependencies(tcpipStackRTOSTaskDelayMenu, ["TCPIP_STACK_RTOS", "TCPIP_STACK_RTOS_USE_DELAY"])

	
	tcpipStackDeviceFamily = tcpipStackComponent.createStringSymbol("TCPIP_DEVICE_FAMILY", None)
	tcpipStackDeviceFamily.setVisible(False)
	if "SAME70" in Variables.get("__PROCESSOR"):
		tcpipStackDeviceFamily.setDefaultValue("SAME70")
	elif "PIC32M" in Variables.get("__PROCESSOR"):
		tcpipStackDeviceFamily.setDefaultValue("PIC32M")
		
	###########################################################################################
	###########################################################################################
	print("TCPIP Heap Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	# Select Heap Configuration
	tcpipStackHeap = tcpipStackComponent.createBooleanSymbol("TCPIP_USE_HEAP", None)
	tcpipStackHeap.setLabel("Select Heap Configuration")
	tcpipStackHeap.setVisible(True)
	tcpipStackHeap.setDescription("Select Heap Configuration")
	tcpipStackHeap.setDefaultValue(True)

	# Use Heap Config
	tcpipStackHeapConfig = tcpipStackComponent.createComboSymbol("TCPIP_STACK_USE_HEAP_CONFIG", tcpipStackHeap, TCPIP_STACK_HEAP_TYPE)
	tcpipStackHeapConfig.setLabel("Use Heap Config")
	tcpipStackHeapConfig.setVisible(True)
	tcpipStackHeapConfig.setDescription("Use Heap Config")
	tcpipStackHeapConfig.setDefaultValue("TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP")
	#tcpipStackHeapConfig.setDependencies(tcpipStackHeapMenuVisibleSingle, ["TCPIP_USE_HEAP"])

	# TCP/IP Stack Dynamic RAM Size
	tcpipStackHeapStackDramSize = tcpipStackComponent.createIntegerSymbol("TCPIP_STACK_DRAM_SIZE", tcpipStackHeap)
	tcpipStackHeapStackDramSize.setLabel("TCP/IP Stack Dynamic RAM Size")
	tcpipStackHeapStackDramSize.setVisible(True)
	tcpipStackHeapStackDramSize.setDescription("TCP/IP Stack Dynamic RAM Size")
	tcpipStackHeapStackDramSize.setDefaultValue(39250)
	tcpipStackHeapStackDramSize.setDependencies(tcpipStackHeapInternMenuVisible, ["TCPIP_STACK_USE_HEAP_CONFIG"])

	# Number of Heap Pool Entries
	tcpipStackHeapPoolEntryNum = tcpipStackComponent.createIntegerSymbol("TCPIP_HEAP_POOL_ENTRIES_NUMBER", tcpipStackHeap)
	tcpipStackHeapPoolEntryNum.setLabel("Number of Heap Pool Entries")
	tcpipStackHeapPoolEntryNum.setVisible(True)
	tcpipStackHeapPoolEntryNum.setDescription("Number of Heap Pool Entries")
	tcpipStackHeapPoolEntryNum.setMin(1)
	tcpipStackHeapPoolEntryNum.setMax(20)
	tcpipStackHeapPoolEntryNum.setDefaultValue(9)
	tcpipStackHeapPoolEntryNum.setDependencies(tcpipStackHeapPoolMenuVisible, ["TCPIP_STACK_USE_HEAP_CONFIG"])
	
	# The Expansion Size for the Internal Heap Pool
	tcpipStackHeapStackPoolExpnSize = tcpipStackComponent.createIntegerSymbol("TCPIP_STACK_POOL_EXPANSION_SIZE", tcpipStackHeap)
	tcpipStackHeapStackPoolExpnSize.setLabel("The Expansion Size for the Internal Heap Pool")
	tcpipStackHeapStackPoolExpnSize.setVisible(True)
	tcpipStackHeapStackPoolExpnSize.setDescription("The Expansion Size for the Internal Heap Pool")
	tcpipStackHeapStackPoolExpnSize.setDefaultValue(51200)
	tcpipStackHeapStackPoolExpnSize.setDependencies(tcpipStackHeapPoolMenuVisible, ["TCPIP_STACK_USE_HEAP_CONFIG"])
	
	# source "$HARMONY_VERSION_PATH/framework/tcpip/config/tcpip_pool_idx.ftl" 20 instances
#####################################################################################################
	for index in range(0,tcpipStackHeapPoolNumMaximum):	
		tcpipStackHeapPoolEntry.append(tcpipStackComponent.createBooleanSymbol("TCPIP_HEAP_POOL_ENTRY_NUMBERX"+str(index),tcpipStackHeap))
		tcpipStackHeapPoolEntry[index].setLabel("Pool Entry "+ str(index))
		tcpipStackHeapPoolEntry[index].setVisible(False)
		tcpipStackHeapPoolEntry[index].setDefaultValue(True)
		tcpipStackHeapPoolEntry[index].setDependencies(tcpipStackHeapPoolEntryInstance, ["TCPIP_HEAP_POOL_ENTRIES_NUMBER","TCPIP_STACK_USE_HEAP_CONFIG"])

		# Heap Block Size
		tcpipStackHeapPoolBlkSize.append(tcpipStackComponent.createIntegerSymbol("TCPIP_HEAP_POOL_ENTRY_SIZE_IDX" + str(index), tcpipStackHeapPoolEntry[index]))
		tcpipStackHeapPoolBlkSize[index].setLabel("Size of the Block, Bytes")
		tcpipStackHeapPoolBlkSize[index].setVisible(False)
		tcpipStackHeapPoolBlkSize[index].setDefaultValue(int(TCIPHEAP_POOL_DFLT_SIZE[index]))
		tcpipStackHeapPoolBlkSize[index].setMin(32)
		tcpipStackHeapPoolBlkSize[index].setMax(4096)
		tcpipStackHeapPoolBlkSize[index].setDependencies(tcpipStackHeapPoolInstnBlkSizeMenu, [tcpipStackHeapPoolEntry[index].getID(),"TCPIP_STACK_USE_HEAP_CONFIG"])

		# Heap Pre-allocated blocks
		tcpipStackHeapPoolPreAllocBlkNum.append(tcpipStackComponent.createIntegerSymbol("TCPIP_HEAP_POOL_ENTRY_BLOCKS_IDX" + str(index), tcpipStackHeapPoolEntry[index]))
		tcpipStackHeapPoolPreAllocBlkNum[index].setLabel("Number of Pre-allocated Blocks with this Size")
		tcpipStackHeapPoolPreAllocBlkNum[index].setVisible(False)
		tcpipStackHeapPoolPreAllocBlkNum[index].setDefaultValue(0)
		tcpipStackHeapPoolPreAllocBlkNum[index].setMin(0)
		tcpipStackHeapPoolPreAllocBlkNum[index].setMax(4096)
		tcpipStackHeapPoolPreAllocBlkNum[index].setDependencies(tcpipStackHeapPoolPreAllocBlkMenu, [tcpipStackHeapPoolEntry[index].getID(),"TCPIP_STACK_USE_HEAP_CONFIG"])
		
		# Heap Pre-allocated blocks
		tcpipStackHeapPoolExpBlkNum.append(tcpipStackComponent.createIntegerSymbol("TCPIP_HEAP_POOL_ENTRY_EXP_BLOCKS_IDX" + str(index), tcpipStackHeapPoolEntry[index]))
		tcpipStackHeapPoolExpBlkNum[index].setLabel("Number of Blocks to Dynamically Expand for this Pool Entry")
		tcpipStackHeapPoolExpBlkNum[index].setVisible(False)
		tcpipStackHeapPoolExpBlkNum[index].setDefaultValue(TCIPHEAP_POOL_EXPBLK_NUM[index])
		tcpipStackHeapPoolExpBlkNum[index].setMin(0)
		tcpipStackHeapPoolExpBlkNum[index].setMax(4096)
		tcpipStackHeapPoolExpBlkNum[index].setDependencies(tcpipStackHeapPoolExpBlkMenu, [tcpipStackHeapPoolEntry[index].getID(),"TCPIP_STACK_USE_HEAP_CONFIG"])
#####################################################################################################
	# Stack allocation function, malloc style
	tcpipStackHeapStackMalloc = tcpipStackComponent.createStringSymbol("TCPIP_STACK_MALLOC_FUNC", tcpipStackHeap)
	tcpipStackHeapStackMalloc.setLabel("Stack allocation function, malloc style")
	tcpipStackHeapStackMalloc.setVisible(True)
	tcpipStackHeapStackMalloc.setDescription("Stack allocation function, malloc style")
	tcpipStackHeapStackMalloc.setDefaultValue("malloc")
	#tcpipStackHeapStackMalloc.setDependencies(tcpipStackHeapMenuVisibleSingle, ["TCPIP_USE_HEAP"])

	# Stack allocation function, calloc style
	tcpipStackHeapStackCalloc = tcpipStackComponent.createStringSymbol("TCPIP_STACK_CALLOC_FUNC", tcpipStackHeap)
	tcpipStackHeapStackCalloc.setLabel("Stack allocation function, calloc style")
	tcpipStackHeapStackCalloc.setVisible(True)
	tcpipStackHeapStackCalloc.setDescription("Stack allocation function, calloc style")
	tcpipStackHeapStackCalloc.setDefaultValue("calloc")
	#tcpipStackHeapStackCalloc.setDependencies(tcpipStackHeapMenuVisibleSingle, ["TCPIP_USE_HEAP"])

	# Stack deallocation function, free style
	tcpipStackHeapStackFree = tcpipStackComponent.createStringSymbol("TCPIP_STACK_FREE_FUNC", tcpipStackHeap)
	tcpipStackHeapStackFree.setLabel("Stack deallocation function, free style")
	tcpipStackHeapStackFree.setVisible(True)
	tcpipStackHeapStackFree.setDescription("Stack deallocation function, free style")
	tcpipStackHeapStackFree.setDefaultValue("free")
	#tcpipStackHeapStackFree.setDependencies(tcpipStackHeapMenuVisibleSingle, ["TCPIP_USE_HEAP"])

	# Flags supported for heap allocation
	tcpipStackHeapFlag = tcpipStackComponent.createComboSymbol("TCPIP_STACK_HEAP_USE_FLAGS", tcpipStackHeap, TCPIP_STACK_HEAP_FLAGS)
	tcpipStackHeapFlag.setLabel("Flags supported for heap allocation")
	tcpipStackHeapFlag.setVisible(True)
	tcpipStackHeapFlag.setDescription("Flags supported for heap allocation")
	tcpipStackHeapFlag.setDefaultValue("TCPIP_STACK_HEAP_FLAG_ALLOC_UNCACHED")
	#tcpipStackHeapFlag.setDependencies(tcpipStackHeapMenuVisibleSingle, ["TCPIP_USE_HEAP"])

	# Heap area usage
	tcpipStackHeapUsageConfig = tcpipStackComponent.createComboSymbol("TCPIP_STACK_HEAP_USAGE_CONFIG", tcpipStackHeap, TCPIP_STACK_HEAP_USAGE)
	tcpipStackHeapUsageConfig.setLabel("Heap area usage")
	tcpipStackHeapUsageConfig.setVisible(True)
	tcpipStackHeapUsageConfig.setDescription("Heap area usage")
	tcpipStackHeapUsageConfig.setDefaultValue("TCPIP_STACK_HEAP_USE_DEFAULT")
	#tcpipStackHeapUsageConfig.setDependencies(tcpipStackHeapMenuVisibleSingle, ["TCPIP_USE_HEAP"])

	# Maximum heap types
	tcpipStackHeapStackSupportHeap = tcpipStackComponent.createIntegerSymbol("TCPIP_STACK_SUPPORTED_HEAPS", tcpipStackHeap)
	tcpipStackHeapStackSupportHeap.setLabel("Maximum heap types")
	tcpipStackHeapStackSupportHeap.setVisible(True)
	tcpipStackHeapStackSupportHeap.setDescription("Maximum heap types")
	tcpipStackHeapStackSupportHeap.setDefaultValue(1)
	#tcpipStackHeapStackSupportHeap.setDependencies(tcpipStackHeapMenuVisibleSingle, ["TCPIP_USE_HEAP"])

	# TCP/IP Stack Dynamic RAM Lower Limit
	tcpipStackHeapStackDramLim = tcpipStackComponent.createIntegerSymbol("TCPIP_STACK_DRAM_RUN_LIMIT", tcpipStackHeap)
	tcpipStackHeapStackDramLim.setLabel("TCP/IP Stack Dynamic RAM Lower Limit")
	tcpipStackHeapStackDramLim.setVisible(True)
	tcpipStackHeapStackDramLim.setDescription("TCP/IP Stack Dynamic RAM Lower Limit")
	tcpipStackHeapStackDramLim.setDefaultValue(2048)
	tcpipStackHeapStackDramLim.setDependencies(tcpipStackHeapExternMenuInvisible, ["TCPIP_STACK_USE_HEAP_CONFIG"])

	# TCP/IP Stack Dynamic Allocation Debug
	tcpipStackHeapStackDramDebug = tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_DRAM_DEBUG_ENABLE", tcpipStackHeap)
	tcpipStackHeapStackDramDebug.setLabel("TCP/IP Stack Dynamic Allocation Debug")
	tcpipStackHeapStackDramDebug.setVisible(True)
	tcpipStackHeapStackDramDebug.setDescription("TCP/IP Stack Dynamic Allocation Debug")
	tcpipStackHeapStackDramDebug.setDefaultValue(False)
	#tcpipStackHeapStackDramDebug.setDependencies(tcpipStackHeapMenuVisibleSingle, ["TCPIP_USE_HEAP"])

	# TCP/IP Stack Dynamic Allocation Trace
	tcpipStackHeapStackDramTrace = tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_DRAM_TRACE_ENABLE", tcpipStackHeapStackDramDebug)
	tcpipStackHeapStackDramTrace.setLabel("TCP/IP Stack Dynamic Allocation Trace")
	tcpipStackHeapStackDramTrace.setVisible(False)
	tcpipStackHeapStackDramTrace.setDescription("TCP/IP Stack Dynamic Allocation Trace")
	tcpipStackHeapStackDramTrace.setDefaultValue(False)
	tcpipStackHeapStackDramTrace.setDependencies(tcpipStackHeapStackDynAllocateTraceVisible, ["TCPIP_STACK_DRAM_DEBUG_ENABLE"])

	# TCP/IP Stack Number of Trace Slots
	tcpipStackHeapStackDramTraceSlotsNum = tcpipStackComponent.createIntegerSymbol("TCPIP_STACK_DRAM_TRACE_SLOTS", tcpipStackHeapStackDramDebug)
	tcpipStackHeapStackDramTraceSlotsNum.setLabel("TCP/IP Stack Number of Trace Slots")
	tcpipStackHeapStackDramTraceSlotsNum.setVisible(False)
	tcpipStackHeapStackDramTraceSlotsNum.setDescription("TCP/IP Stack Number of Trace Slots")
	tcpipStackHeapStackDramTraceSlotsNum.setDefaultValue(16)
	tcpipStackHeapStackDramTraceSlotsNum.setDependencies(tcpipStackHeapStackTraceSlotsNumVisible, ["TCPIP_STACK_DRAM_TRACE_ENABLE"])
	
	#Add to system_config.h
	tcpipStackHeapHeaderFtl = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackHeapHeaderFtl.setSourcePath("tcpip/config/tcpip_heap.h.ftl")
	tcpipStackHeapHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipStackHeapHeaderFtl.setMarkup(True)
	tcpipStackHeapHeaderFtl.setType("STRING")	

	# Add tcpip_heap_alloc.c file
	tcpipStackHeapAllocSourceFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackHeapAllocSourceFile.setSourcePath("tcpip/src/tcpip_heap_alloc.c")
	tcpipStackHeapAllocSourceFile.setOutputName("tcpip_heap_alloc.c")
	tcpipStackHeapAllocSourceFile.setOverwrite(True)
	tcpipStackHeapAllocSourceFile.setDestPath("library/tcpip/src/")
	tcpipStackHeapAllocSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackHeapAllocSourceFile.setType("SOURCE")
	tcpipStackHeapAllocSourceFile.setEnabled(True)
	#tcpipStackHeapAllocSourceFile.setDependencies(tcpipStackHeapAllocGenSourceFile, ["TCPIP_USE_HEAP"])

	# Add tcpip_heap_external.c file
	tcpipStackHeapExterncSourceFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackHeapExterncSourceFile.setSourcePath("tcpip/src/tcpip_heap_external.c")
	tcpipStackHeapExterncSourceFile.setOutputName("tcpip_heap_external.c")
	tcpipStackHeapExterncSourceFile.setOverwrite(True)
	tcpipStackHeapExterncSourceFile.setDestPath("library/tcpip/src/")
	tcpipStackHeapExterncSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackHeapExterncSourceFile.setType("SOURCE")
	tcpipStackHeapExterncSourceFile.setEnabled(False)
	tcpipStackHeapExterncSourceFile.setDependencies(tcpipStackHeapExternSourceFile, ["TCPIP_STACK_USE_HEAP_CONFIG"])	

	# Add tcpip_heap_internal.c file
	tcpipStackHeapInterncSourceFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackHeapInterncSourceFile.setSourcePath("tcpip/src/tcpip_heap_internal.c")
	tcpipStackHeapInterncSourceFile.setOutputName("tcpip_heap_internal.c")
	tcpipStackHeapInterncSourceFile.setOverwrite(True)
	tcpipStackHeapInterncSourceFile.setDestPath("library/tcpip/src/")
	tcpipStackHeapInterncSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackHeapInterncSourceFile.setType("SOURCE")
	tcpipStackHeapInterncSourceFile.setEnabled(True)
	tcpipStackHeapInterncSourceFile.setDependencies(tcpipStackHeapInternSourceFile, ["TCPIP_STACK_USE_HEAP_CONFIG"])

	# Add tcpip_heap_pool.c file
	tcpipStackHeapPoolcSourceFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackHeapPoolcSourceFile.setSourcePath("tcpip/src/tcpip_heap_pool.c")
	tcpipStackHeapPoolcSourceFile.setOutputName("tcpip_heap_pool.c")
	tcpipStackHeapPoolcSourceFile.setOverwrite(True)
	tcpipStackHeapPoolcSourceFile.setDestPath("library/tcpip/src/")
	tcpipStackHeapPoolcSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackHeapPoolcSourceFile.setType("SOURCE")
	tcpipStackHeapPoolcSourceFile.setEnabled(False)
	tcpipStackHeapPoolcSourceFile.setDependencies(tcpipStackHeapPoolSourceFile, ["TCPIP_STACK_USE_HEAP_CONFIG"])
	###########################################################################################
	# add "<#include \"/framework/tcpip/config/tcpip_stack.h.ftl\">"  to list SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION 	
	tcpipStackHeaderFtl = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackHeaderFtl.setSourcePath("tcpip/config/tcpip_stack.h.ftl")
	tcpipStackHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipStackHeaderFtl.setMarkup(True)
	tcpipStackHeaderFtl.setType("STRING")

	# file TCPIP_COMMON_PORTS_H "$HARMONY_VERSION_PATH/framework/tcpip/tcpip_common_ports.h" to "$PROJECT_HEADER_FILES/framework/tcpip/tcpip_common_ports.h"
	tcpipStackCommonPortHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackCommonPortHeaderFile.setSourcePath("tcpip/tcpip_common_ports.h")
	tcpipStackCommonPortHeaderFile.setOutputName("tcpip_common_ports.h")
	tcpipStackCommonPortHeaderFile.setDestPath("library/tcpip/")
	tcpipStackCommonPortHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackCommonPortHeaderFile.setType("HEADER")
	tcpipStackCommonPortHeaderFile.setOverwrite(True)
		
	# file TCPIP_MAC_H "$HARMONY_VERSION_PATH/framework/tcpip/tcpip_mac.h" to                                     "$PROJECT_HEADER_FILES/framework/tcpip/tcpip_mac.h"
	tcpipStackTcpipMacHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipMacHeaderFile.setSourcePath("tcpip/tcpip_mac.h")
	tcpipStackTcpipMacHeaderFile.setOutputName("tcpip_mac.h")
	tcpipStackTcpipMacHeaderFile.setDestPath("library/tcpip/")
	tcpipStackTcpipMacHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackTcpipMacHeaderFile.setType("HEADER")
	tcpipStackTcpipMacHeaderFile.setOverwrite(True)
		
	# file TCPIP_MAC_OBJECT_H "$HARMONY_VERSION_PATH/framework/tcpip/tcpip_mac_object.h" to                       "$PROJECT_HEADER_FILES/framework/tcpip/tcpip_mac_object.h"
	tcpipStackTcpipMacObjHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipMacObjHeaderFile.setSourcePath("tcpip/tcpip_mac_object.h")
	tcpipStackTcpipMacObjHeaderFile.setOutputName("tcpip_mac_object.h")
	tcpipStackTcpipMacObjHeaderFile.setDestPath("library/tcpip/")
	tcpipStackTcpipMacObjHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackTcpipMacObjHeaderFile.setType("HEADER")
	tcpipStackTcpipMacObjHeaderFile.setOverwrite(True)
	
			
	# file TCPIP_MANAGER_H "$HARMONY_VERSION_PATH/framework/tcpip/tcpip_manager.h" to "$PROJECT_HEADER_FILES/framework/tcpip/tcpip_manager.h"
	tcpipStackTcpipMngrHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipMngrHeaderFile.setSourcePath("tcpip/tcpip_manager.h")
	tcpipStackTcpipMngrHeaderFile.setOutputName("tcpip_manager.h")
	tcpipStackTcpipMngrHeaderFile.setDestPath("library/tcpip/")
	tcpipStackTcpipMngrHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackTcpipMngrHeaderFile.setType("HEADER")
	tcpipStackTcpipMngrHeaderFile.setOverwrite(True)
	
	# Add tcpip_heap.h file to project
	tcpipStackHeapHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackHeapHeaderFile.setSourcePath("tcpip/tcpip_heap.h")
	tcpipStackHeapHeaderFile.setOutputName("tcpip_heap.h")
	tcpipStackHeapHeaderFile.setDestPath("library/tcpip/")
	tcpipStackHeapHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackHeapHeaderFile.setType("HEADER")
	tcpipStackHeapHeaderFile.setOverwrite(True)
	
	# Add tcpip_heap_alloc.h file to project
	tcpipStackHeapAllocHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackHeapAllocHeaderFile.setSourcePath("tcpip/src/tcpip_heap_alloc.h")
	tcpipStackHeapAllocHeaderFile.setOutputName("tcpip_heap_alloc.h")
	tcpipStackHeapAllocHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackHeapAllocHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackHeapAllocHeaderFile.setType("HEADER")
	tcpipStackHeapAllocHeaderFile.setOverwrite(True)

			
	# file TCPIP_HELPERS_H "$HARMONY_VERSION_PATH/framework/tcpip/tcpip_helpers.h" to "$PROJECT_HEADER_FILES/framework/tcpip/tcpip_helpers.h"
	tcpipStackTcpipHelperHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipHelperHeaderFile.setSourcePath("tcpip/tcpip_helpers.h")
	tcpipStackTcpipHelperHeaderFile.setOutputName("tcpip_helpers.h")
	tcpipStackTcpipHelperHeaderFile.setDestPath("library/tcpip/")
	tcpipStackTcpipHelperHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackTcpipHelperHeaderFile.setType("HEADER")
	tcpipStackTcpipHelperHeaderFile.setOverwrite(True)

	# Add ndp.h file to project
	tcpipStackNdpHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackNdpHeaderFile.setSourcePath("tcpip/ndp.h")
	tcpipStackNdpHeaderFile.setOutputName("ndp.h")
	tcpipStackNdpHeaderFile.setDestPath("library/tcpip/")
	tcpipStackNdpHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackNdpHeaderFile.setType("HEADER")
	tcpipStackNdpHeaderFile.setOverwrite(True)

	# Add ndp_manager.h file to project
	tcpipStackNdpManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackNdpManagerHeaderFile.setSourcePath("tcpip/src/ndp_manager.h")
	tcpipStackNdpManagerHeaderFile.setOutputName("ndp_manager.h")
	tcpipStackNdpManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackNdpManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackNdpManagerHeaderFile.setType("HEADER")
	tcpipStackNdpManagerHeaderFile.setOverwrite(True)

	# Add ndp_private.h file to project
	tcpipStackNdpPrivateHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackNdpPrivateHeaderFile.setSourcePath("tcpip/src/ndp_private.h")
	tcpipStackNdpPrivateHeaderFile.setOutputName("ndp_private.h")
	tcpipStackNdpPrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackNdpPrivateHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackNdpPrivateHeaderFile.setType("HEADER")
	tcpipStackNdpPrivateHeaderFile.setOverwrite(True)
	
	# Add ipv4.h file to project
	tcpipStackIPv4HeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackIPv4HeaderFile.setSourcePath("tcpip/ipv4.h")
	tcpipStackIPv4HeaderFile.setOutputName("ipv4.h")
	tcpipStackIPv4HeaderFile.setDestPath("library/tcpip/")
	tcpipStackIPv4HeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackIPv4HeaderFile.setType("HEADER")
	tcpipStackIPv4HeaderFile.setOverwrite(True)

	# Add ipv4_manager.h file to project
	tcpipStackIPv4ManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackIPv4ManagerHeaderFile.setSourcePath("tcpip/src/ipv4_manager.h")
	tcpipStackIPv4ManagerHeaderFile.setOutputName("ipv4_manager.h")
	tcpipStackIPv4ManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackIPv4ManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackIPv4ManagerHeaderFile.setType("HEADER")
	tcpipStackIPv4ManagerHeaderFile.setOverwrite(True)

	# Add ipv4_private.h file to project
	tcpipStackIPv4PrivateHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackIPv4PrivateHeaderFile.setSourcePath("tcpip/src/ipv4_private.h")
	tcpipStackIPv4PrivateHeaderFile.setOutputName("ipv4_private.h")
	tcpipStackIPv4PrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackIPv4PrivateHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackIPv4PrivateHeaderFile.setType("HEADER")
	tcpipStackIPv4PrivateHeaderFile.setOverwrite(True)
	
	# Add ipv6.h file to project
	tcpipStackIPv6HeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackIPv6HeaderFile.setSourcePath("tcpip/ipv6.h")
	tcpipStackIPv6HeaderFile.setOutputName("ipv6.h")
	tcpipStackIPv6HeaderFile.setDestPath("library/tcpip/")
	tcpipStackIPv6HeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackIPv6HeaderFile.setType("HEADER")
	tcpipStackIPv6HeaderFile.setOverwrite(True)

	# Add ipv6_manager.h file to project
	tcpipStackIPv6ManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackIPv6ManagerHeaderFile.setSourcePath("tcpip/src/ipv6_manager.h")
	tcpipStackIPv6ManagerHeaderFile.setOutputName("ipv6_manager.h")
	tcpipStackIPv6ManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackIPv6ManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackIPv6ManagerHeaderFile.setType("HEADER")
	tcpipStackIPv6ManagerHeaderFile.setOverwrite(True)

	# Add ipv6_private.h file to project
	tcpipStackIPv6PrivateHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackIPv6PrivateHeaderFile.setSourcePath("tcpip/src/ipv6_private.h")
	tcpipStackIPv6PrivateHeaderFile.setOutputName("ipv6_private.h")
	tcpipStackIPv6PrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackIPv6PrivateHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackIPv6PrivateHeaderFile.setType("HEADER")
	tcpipStackIPv6PrivateHeaderFile.setOverwrite(True)

	# Add icmpv6.h file to project
	tcpipStackICMPv6HeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackICMPv6HeaderFile.setSourcePath("tcpip/icmpv6.h")
	tcpipStackICMPv6HeaderFile.setOutputName("icmpv6.h")
	tcpipStackICMPv6HeaderFile.setDestPath("library/tcpip/")
	tcpipStackICMPv6HeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackICMPv6HeaderFile.setType("HEADER")
	tcpipStackICMPv6HeaderFile.setOverwrite(True)

	# Add icmpv6_manager.h file to project
	tcpipStackICMPv6ManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackICMPv6ManagerHeaderFile.setSourcePath("tcpip/src/icmpv6_manager.h")
	tcpipStackICMPv6ManagerHeaderFile.setOutputName("icmpv6_manager.h")
	tcpipStackICMPv6ManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackICMPv6ManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackICMPv6ManagerHeaderFile.setType("HEADER")
	tcpipStackICMPv6ManagerHeaderFile.setOverwrite(True)

	# Add icmp_manager.h file to project
	tcpipStackIcmpManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackIcmpManagerHeaderFile.setSourcePath("tcpip/src/icmp_manager.h")
	tcpipStackIcmpManagerHeaderFile.setOutputName("icmp_manager.h")
	tcpipStackIcmpManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackIcmpManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackIcmpManagerHeaderFile.setType("HEADER")
	tcpipStackIcmpManagerHeaderFile.setOverwrite(True)
			
	# file DHCPV6_H "$HARMONY_VERSION_PATH/framework/tcpip/dhcpv6.h" to "$PROJECT_HEADER_FILES/framework/tcpip/dhcpv6.h"
	tcpipStackDhcpv6HeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackDhcpv6HeaderFile.setSourcePath("tcpip/dhcpv6.h")
	tcpipStackDhcpv6HeaderFile.setOutputName("dhcpv6.h")
	tcpipStackDhcpv6HeaderFile.setDestPath("library/tcpip/")
	tcpipStackDhcpv6HeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackDhcpv6HeaderFile.setType("HEADER")
	tcpipStackDhcpv6HeaderFile.setOverwrite(True)
	
	# Add arp.h file to project
	tcpipStackArpHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackArpHeaderFile.setSourcePath("tcpip/arp.h")
	tcpipStackArpHeaderFile.setOutputName("arp.h")
	tcpipStackArpHeaderFile.setDestPath("library/tcpip/")
	tcpipStackArpHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackArpHeaderFile.setType("HEADER")
	tcpipStackArpHeaderFile.setOverwrite(True)

	# Add arp_manager.h file to project
	tcpipStackArpManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackArpManagerHeaderFile.setSourcePath("tcpip/src/arp_manager.h")
	tcpipStackArpManagerHeaderFile.setOutputName("arp_manager.h")
	tcpipStackArpManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackArpManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackArpManagerHeaderFile.setType("HEADER")
	tcpipStackArpManagerHeaderFile.setOverwrite(True)

	# Add arp_private.h file to project
	tcpipStackArpPrivateHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackArpPrivateHeaderFile.setSourcePath("tcpip/src/arp_private.h")
	tcpipStackArpPrivateHeaderFile.setOutputName("arp_private.h")
	tcpipStackArpPrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackArpPrivateHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackArpPrivateHeaderFile.setType("HEADER")
	tcpipStackArpPrivateHeaderFile.setOverwrite(True)

	# Add udp.h file to project
	tcpipStackUdpHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackUdpHeaderFile.setSourcePath("tcpip/udp.h")
	tcpipStackUdpHeaderFile.setOutputName("udp.h")
	tcpipStackUdpHeaderFile.setDestPath("library/tcpip/")
	tcpipStackUdpHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackUdpHeaderFile.setType("HEADER")
	tcpipStackUdpHeaderFile.setOverwrite(True)
	
	# Add udp_manager.h file to project
	tcpipStackUdpManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackUdpManagerHeaderFile.setSourcePath("tcpip/src/udp_manager.h")
	tcpipStackUdpManagerHeaderFile.setOutputName("udp_manager.h")
	tcpipStackUdpManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackUdpManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackUdpManagerHeaderFile.setType("HEADER")
	tcpipStackUdpManagerHeaderFile.setOverwrite(True)

	# Add udp_private.h file to project
	tcpipStackUdpPrivateHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackUdpPrivateHeaderFile.setSourcePath("tcpip/src/udp_private.h")
	tcpipStackUdpPrivateHeaderFile.setOutputName("udp_private.h")
	tcpipStackUdpPrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackUdpPrivateHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackUdpPrivateHeaderFile.setType("HEADER")
	tcpipStackUdpPrivateHeaderFile.setOverwrite(True)
	
	# Add tcp.h file to project
	tcpipStackTcpHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpHeaderFile.setSourcePath("tcpip/tcp.h")
	tcpipStackTcpHeaderFile.setOutputName("tcp.h")
	tcpipStackTcpHeaderFile.setDestPath("library/tcpip/")
	tcpipStackTcpHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackTcpHeaderFile.setType("HEADER")
	tcpipStackTcpHeaderFile.setOverwrite(True)

	# Add tcp_manager.h file to project
	tcpipStackTcpManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpManagerHeaderFile.setSourcePath("tcpip/src/tcp_manager.h")
	tcpipStackTcpManagerHeaderFile.setOutputName("tcp_manager.h")
	tcpipStackTcpManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackTcpManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackTcpManagerHeaderFile.setType("HEADER")
	tcpipStackTcpManagerHeaderFile.setOverwrite(True)

	# Add tcp_private.h file to project
	tcpipStackTcpPrivateHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpPrivateHeaderFile.setSourcePath("tcpip/src/tcp_private.h")
	tcpipStackTcpPrivateHeaderFile.setOutputName("tcp_private.h")
	tcpipStackTcpPrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackTcpPrivateHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackTcpPrivateHeaderFile.setType("HEADER")
	tcpipStackTcpPrivateHeaderFile.setOverwrite(True)

	# Add berkeley_api.h file to project
	tcpipStackBerkeleyApiHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackBerkeleyApiHeaderFile.setSourcePath("tcpip/berkeley_api.h")
	tcpipStackBerkeleyApiHeaderFile.setOutputName("berkeley_api.h")
	tcpipStackBerkeleyApiHeaderFile.setDestPath("library/tcpip/")
	tcpipStackBerkeleyApiHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackBerkeleyApiHeaderFile.setType("HEADER")
	tcpipStackBerkeleyApiHeaderFile.setOverwrite(True)
	
	# Add dhcp.h file to project
	tcpipStackDhcpcHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackDhcpcHeaderFile.setSourcePath("tcpip/dhcp.h")
	tcpipStackDhcpcHeaderFile.setOutputName("dhcp.h")
	tcpipStackDhcpcHeaderFile.setDestPath("library/tcpip/")
	tcpipStackDhcpcHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackDhcpcHeaderFile.setType("HEADER")
	tcpipStackDhcpcHeaderFile.setOverwrite(True)

	# Add dhcp_manager.h file to project
	tcpipStackDhcpcManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackDhcpcManagerHeaderFile.setSourcePath("tcpip/src/dhcp_manager.h")
	tcpipStackDhcpcManagerHeaderFile.setOutputName("dhcp_manager.h")
	tcpipStackDhcpcManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackDhcpcManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackDhcpcManagerHeaderFile.setType("HEADER")
	tcpipStackDhcpcManagerHeaderFile.setOverwrite(True)

	# Add dhcp_private.h file to project
	tcpipStackDhcpcPrivateHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackDhcpcPrivateHeaderFile.setSourcePath("tcpip/src/dhcp_private.h")
	tcpipStackDhcpcPrivateHeaderFile.setOutputName("dhcp_private.h")
	tcpipStackDhcpcPrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackDhcpcPrivateHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackDhcpcPrivateHeaderFile.setType("HEADER")
	tcpipStackDhcpcPrivateHeaderFile.setOverwrite(True)
	
	# Add dhcps.h file to project
	tcpipStackDhcpsHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackDhcpsHeaderFile.setSourcePath("tcpip/dhcps.h")
	tcpipStackDhcpsHeaderFile.setOutputName("dhcps.h")
	tcpipStackDhcpsHeaderFile.setDestPath("library/tcpip/")
	tcpipStackDhcpsHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackDhcpsHeaderFile.setType("HEADER")
	tcpipStackDhcpsHeaderFile.setOverwrite(True)

	# Add dhcps_manager.h file to project
	tcpipStackDhcpsManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackDhcpsManagerHeaderFile.setSourcePath("tcpip/src/dhcps_manager.h")
	tcpipStackDhcpsManagerHeaderFile.setOutputName("dhcps_manager.h")
	tcpipStackDhcpsManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackDhcpsManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackDhcpsManagerHeaderFile.setType("HEADER")
	tcpipStackDhcpsManagerHeaderFile.setOverwrite(True)

	# Add dhcps_private.h file to project
	tcpipStackDhcpPrivateHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackDhcpPrivateHeaderFile.setSourcePath("tcpip/src/dhcps_private.h")
	tcpipStackDhcpPrivateHeaderFile.setOutputName("dhcps_private.h")
	tcpipStackDhcpPrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackDhcpPrivateHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackDhcpPrivateHeaderFile.setType("HEADER")
	tcpipStackDhcpPrivateHeaderFile.setOverwrite(True)

	# Add zero_conf_link_local.h file to project
	tcpipStackZcLlHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackZcLlHeaderFile.setSourcePath("tcpip/zero_conf_link_local.h")
	tcpipStackZcLlHeaderFile.setOutputName("zero_conf_link_local.h")
	tcpipStackZcLlHeaderFile.setDestPath("library/tcpip/")
	tcpipStackZcLlHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackZcLlHeaderFile.setType("HEADER")
	tcpipStackZcLlHeaderFile.setOverwrite(True)

	# Add zero_conf_multicast_dns.h file to project
	tcpipStackZcMdnsHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackZcMdnsHeaderFile.setSourcePath("tcpip/zero_conf_multicast_dns.h")
	tcpipStackZcMdnsHeaderFile.setOutputName("zero_conf_multicast_dns.h")
	tcpipStackZcMdnsHeaderFile.setDestPath("library/tcpip/")
	tcpipStackZcMdnsHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackZcMdnsHeaderFile.setType("HEADER")
	tcpipStackZcMdnsHeaderFile.setOverwrite(True)

	# Add zero_conf_helper.h file to project
	tcpipStackZcHelperHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackZcHelperHeaderFile.setSourcePath("tcpip/src/zero_conf_helper.h")
	tcpipStackZcHelperHeaderFile.setOutputName("zero_conf_helper.h")
	tcpipStackZcHelperHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackZcHelperHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackZcHelperHeaderFile.setType("HEADER")
	tcpipStackZcHelperHeaderFile.setOverwrite(True)

	# Add zero_conf_link_local_private.h file to project
	tcpipStackZcLlPrivateHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackZcLlPrivateHeaderFile.setSourcePath("tcpip/src/zero_conf_link_local_private.h")
	tcpipStackZcLlPrivateHeaderFile.setOutputName("zero_conf_link_local_private.h")
	tcpipStackZcLlPrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackZcLlPrivateHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackZcLlPrivateHeaderFile.setType("HEADER")
	tcpipStackZcLlPrivateHeaderFile.setOverwrite(True)

	# Add zero_conf_manager.h file to project
	tcpipStackZcManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackZcManagerHeaderFile.setSourcePath("tcpip/src/zero_conf_manager.h")
	tcpipStackZcManagerHeaderFile.setOutputName("zero_conf_manager.h")
	tcpipStackZcManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackZcManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackZcManagerHeaderFile.setType("HEADER")
	tcpipStackZcManagerHeaderFile.setOverwrite(True)
	
	# Add dns.h file to project
	tcpipStackDnscHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackDnscHeaderFile.setSourcePath("tcpip/dns.h")
	tcpipStackDnscHeaderFile.setOutputName("dns.h")
	tcpipStackDnscHeaderFile.setDestPath("library/tcpip/")
	tcpipStackDnscHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackDnscHeaderFile.setType("HEADER")
	tcpipStackDnscHeaderFile.setOverwrite(True)

	# Add dns_manager.h file to project
	tcpipStackDnscManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackDnscManagerHeaderFile.setSourcePath("tcpip/src/dns_manager.h")
	tcpipStackDnscManagerHeaderFile.setOutputName("dns_manager.h")
	tcpipStackDnscManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackDnscManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackDnscManagerHeaderFile.setType("HEADER")
	tcpipStackDnscManagerHeaderFile.setOverwrite(True)

	# Add dns_private.h file to project
	tcpipStackDnscPrivateHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackDnscPrivateHeaderFile.setSourcePath("tcpip/src/dns_private.h")
	tcpipStackDnscPrivateHeaderFile.setOutputName("dns_private.h")
	tcpipStackDnscPrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackDnscPrivateHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackDnscPrivateHeaderFile.setType("HEADER")
	tcpipStackDnscPrivateHeaderFile.setOverwrite(True)
	
	# Add dnss.h file to project
	tcpipStackDnssHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackDnssHeaderFile.setSourcePath("tcpip/dnss.h")
	tcpipStackDnssHeaderFile.setOutputName("dnss.h")
	tcpipStackDnssHeaderFile.setDestPath("library/tcpip/")
	tcpipStackDnssHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackDnssHeaderFile.setType("HEADER")
	tcpipStackDnssHeaderFile.setOverwrite(True)

	# Add dnss_manager.h file to project
	tcpipStackDnssManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackDnssManagerHeaderFile.setSourcePath("tcpip/src/dnss_manager.h")
	tcpipStackDnssManagerHeaderFile.setOutputName("dnss_manager.h")
	tcpipStackDnssManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackDnssManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackDnssManagerHeaderFile.setType("HEADER")
	tcpipStackDnssManagerHeaderFile.setOverwrite(True)

	# Add dnss_private.h file to project
	tcpipStackDnssPrivateHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackDnssPrivateHeaderFile.setSourcePath("tcpip/src/dnss_private.h")
	tcpipStackDnssPrivateHeaderFile.setOutputName("dnss_private.h")
	tcpipStackDnssPrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackDnssPrivateHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackDnssPrivateHeaderFile.setType("HEADER")
	tcpipStackDnssPrivateHeaderFile.setOverwrite(True)
	
	# Add ftp.h file to project
	tcpipStackFtpsHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackFtpsHeaderFile.setSourcePath("tcpip/ftp.h")
	tcpipStackFtpsHeaderFile.setOutputName("ftp.h")
	tcpipStackFtpsHeaderFile.setDestPath("library/tcpip/")
	tcpipStackFtpsHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackFtpsHeaderFile.setType("HEADER")
	tcpipStackFtpsHeaderFile.setOverwrite(True)

	# Add ftp_manager.h file to project
	tcpipStackFtpsManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackFtpsManagerHeaderFile.setSourcePath("tcpip/src/ftp_manager.h")
	tcpipStackFtpsManagerHeaderFile.setOutputName("ftp_manager.h")
	tcpipStackFtpsManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackFtpsManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackFtpsManagerHeaderFile.setType("HEADER")
	tcpipStackFtpsManagerHeaderFile.setOverwrite(True)

	# Add ftp_private.h file to project
	tcpipStackFtpsPrivateHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackFtpsPrivateHeaderFile.setSourcePath("tcpip/src/ftp_private.h")
	tcpipStackFtpsPrivateHeaderFile.setOutputName("ftp_private.h")
	tcpipStackFtpsPrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackFtpsPrivateHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackFtpsPrivateHeaderFile.setType("HEADER")
	tcpipStackFtpsPrivateHeaderFile.setOverwrite(True)
	
	# Add icmp.h file to project
	tcpipStackIcmpHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackIcmpHeaderFile.setSourcePath("tcpip/icmp.h")
	tcpipStackIcmpHeaderFile.setOutputName("icmp.h")
	tcpipStackIcmpHeaderFile.setDestPath("library/tcpip/")
	tcpipStackIcmpHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackIcmpHeaderFile.setType("HEADER")
	tcpipStackIcmpHeaderFile.setOverwrite(True)
	
	# Add nbns.h file to project
	tcpipStackNbnsHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackNbnsHeaderFile.setSourcePath("tcpip/nbns.h")
	tcpipStackNbnsHeaderFile.setOutputName("nbns.h")
	tcpipStackNbnsHeaderFile.setDestPath("library/tcpip/")
	tcpipStackNbnsHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackNbnsHeaderFile.setType("HEADER")
	tcpipStackNbnsHeaderFile.setOverwrite(True)

	# Add nbns_manager.h file to project
	tcpipStackNbnsManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackNbnsManagerHeaderFile.setSourcePath("tcpip/src/nbns_manager.h")
	tcpipStackNbnsManagerHeaderFile.setOutputName("nbns_manager.h")
	tcpipStackNbnsManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackNbnsManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackNbnsManagerHeaderFile.setType("HEADER")
	tcpipStackNbnsManagerHeaderFile.setOverwrite(True)
	
	# Add ddns.h file to project
	tcpipStackDdnsHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackDdnsHeaderFile.setSourcePath("tcpip/ddns.h")
	tcpipStackDdnsHeaderFile.setOutputName("ddns.h")
	tcpipStackDdnsHeaderFile.setDestPath("library/tcpip/")
	tcpipStackDdnsHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackDdnsHeaderFile.setType("HEADER")
	tcpipStackDdnsHeaderFile.setOverwrite(True)

	# Add ddns_manager.h file to project
	tcpipStackDdnsManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackDdnsManagerHeaderFile.setSourcePath("tcpip/src/ddns_manager.h")
	tcpipStackDdnsManagerHeaderFile.setOutputName("ddns_manager.h")
	tcpipStackDdnsManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackDdnsManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackDdnsManagerHeaderFile.setType("HEADER")
	tcpipStackDdnsManagerHeaderFile.setOverwrite(True)
	
	# Add telnet.h file to project
	tcpipStackTelnetHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTelnetHeaderFile.setSourcePath("tcpip/telnet.h")
	tcpipStackTelnetHeaderFile.setOutputName("telnet.h")
	tcpipStackTelnetHeaderFile.setDestPath("library/tcpip/")
	tcpipStackTelnetHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackTelnetHeaderFile.setType("HEADER")
	tcpipStackTelnetHeaderFile.setOverwrite(True)

	# Add telnet_manager.h file to project
	tcpipStackTelnetManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTelnetManagerHeaderFile.setSourcePath("tcpip/src/telnet_manager.h")
	tcpipStackTelnetManagerHeaderFile.setOutputName("telnet_manager.h")
	tcpipStackTelnetManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackTelnetManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackTelnetManagerHeaderFile.setType("HEADER")
	tcpipStackTelnetManagerHeaderFile.setOverwrite(True)
	
	# Add smtp.h file to project
	tcpipStackSmtpHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackSmtpHeaderFile.setSourcePath("tcpip/smtp.h")
	tcpipStackSmtpHeaderFile.setOutputName("smtp.h")
	tcpipStackSmtpHeaderFile.setDestPath("library/tcpip/")
	tcpipStackSmtpHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackSmtpHeaderFile.setType("HEADER")
	tcpipStackSmtpHeaderFile.setOverwrite(True)

	# Add smtp_manager.h file to project
	tcpipStackSmtpManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackSmtpManagerHeaderFile.setSourcePath("tcpip/src/smtp_manager.h")
	tcpipStackSmtpManagerHeaderFile.setOutputName("smtp_manager.h")
	tcpipStackSmtpManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackSmtpManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackSmtpManagerHeaderFile.setType("HEADER")
	tcpipStackSmtpManagerHeaderFile.setOverwrite(True)
	
	# Add sntp.h file to project
	tcpipStackSntpHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackSntpHeaderFile.setSourcePath("tcpip/sntp.h")
	tcpipStackSntpHeaderFile.setOutputName("sntp.h")
	tcpipStackSntpHeaderFile.setDestPath("library/tcpip/")
	tcpipStackSntpHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackSntpHeaderFile.setType("HEADER")
	tcpipStackSntpHeaderFile.setOverwrite(True)

	# Add sntp_manager.h file to project
	tcpipStackSntpManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackSntpManagerHeaderFile.setSourcePath("tcpip/src/sntp_manager.h")
	tcpipStackSntpManagerHeaderFile.setOutputName("sntp_manager.h")
	tcpipStackSntpManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackSntpManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackSntpManagerHeaderFile.setType("HEADER")
	tcpipStackSntpManagerHeaderFile.setOverwrite(True)
	
	# Add http.h file to project
	tcpipStackHttpHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackHttpHeaderFile.setSourcePath("tcpip/http.h")
	tcpipStackHttpHeaderFile.setOutputName("http.h")
	tcpipStackHttpHeaderFile.setDestPath("library/tcpip/")
	tcpipStackHttpHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackHttpHeaderFile.setType("HEADER")
	tcpipStackHttpHeaderFile.setOverwrite(True)

	# Add http_manager.h file to project
	tcpipStackHttpManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackHttpManagerHeaderFile.setSourcePath("tcpip/src/http_manager.h")
	tcpipStackHttpManagerHeaderFile.setOutputName("http_manager.h")
	tcpipStackHttpManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackHttpManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackHttpManagerHeaderFile.setType("HEADER")
	tcpipStackHttpManagerHeaderFile.setOverwrite(True)

	# Add http_private.h file to project
	tcpipStackHttpPrivateHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackHttpPrivateHeaderFile.setSourcePath("tcpip/src/http_private.h")
	tcpipStackHttpPrivateHeaderFile.setOutputName("http_private.h")
	tcpipStackHttpPrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackHttpPrivateHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackHttpPrivateHeaderFile.setType("HEADER")
	tcpipStackHttpPrivateHeaderFile.setOverwrite(True)
	
	# Add http_net.h file to project
	tcpipStackHttpNetHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackHttpNetHeaderFile.setSourcePath("tcpip/http_net.h")
	tcpipStackHttpNetHeaderFile.setOutputName("http_net.h")
	tcpipStackHttpNetHeaderFile.setDestPath("library/tcpip/")
	tcpipStackHttpNetHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackHttpNetHeaderFile.setType("HEADER")
	tcpipStackHttpNetHeaderFile.setOverwrite(True)

	# Add http_net_manager.h file to project
	tcpipStackHttpNetManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackHttpNetManagerHeaderFile.setSourcePath("tcpip/src/http_net_manager.h")
	tcpipStackHttpNetManagerHeaderFile.setOutputName("http_net_manager.h")
	tcpipStackHttpNetManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackHttpNetManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackHttpNetManagerHeaderFile.setType("HEADER")
	tcpipStackHttpNetManagerHeaderFile.setOverwrite(True)

	# Add http_net_private.h file to project
	tcpipStackHttpNetPrivateHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackHttpNetPrivateHeaderFile.setSourcePath("tcpip/src/http_net_private.h")
	tcpipStackHttpNetPrivateHeaderFile.setOutputName("http_net_private.h")
	tcpipStackHttpNetPrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackHttpNetPrivateHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackHttpNetPrivateHeaderFile.setType("HEADER")
	tcpipStackHttpNetPrivateHeaderFile.setOverwrite(True)

	# Add snmp.h file to project
	tcpipStackSnmpHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackSnmpHeaderFile.setSourcePath("tcpip/snmp.h")
	tcpipStackSnmpHeaderFile.setOutputName("snmp.h")
	tcpipStackSnmpHeaderFile.setDestPath("library/tcpip/")
	tcpipStackSnmpHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackSnmpHeaderFile.setType("HEADER")
	tcpipStackSnmpHeaderFile.setOverwrite(True)

	# Add snmp_manager.h file to project
	tcpipStackSnmpManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackSnmpManagerHeaderFile.setSourcePath("tcpip/src/snmp_manager.h")
	tcpipStackSnmpManagerHeaderFile.setOutputName("snmp_manager.h")
	tcpipStackSnmpManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackSnmpManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackSnmpManagerHeaderFile.setType("HEADER")
	tcpipStackSnmpManagerHeaderFile.setOverwrite(True)

	# Add snmp_private.h file to project
	tcpipStackSnmpPrivateHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackSnmpPrivateHeaderFile.setSourcePath("tcpip/src/snmp_private.h")
	tcpipStackSnmpPrivateHeaderFile.setOutputName("snmp_private.h")
	tcpipStackSnmpPrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackSnmpPrivateHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackSnmpPrivateHeaderFile.setType("HEADER")
	tcpipStackSnmpPrivateHeaderFile.setOverwrite(True)

	# Add snmpv3.h file to project
	tcpipStackSnmpV3HeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackSnmpV3HeaderFile.setSourcePath("tcpip/snmpv3.h")
	tcpipStackSnmpV3HeaderFile.setOutputName("snmpv3.h")
	tcpipStackSnmpV3HeaderFile.setDestPath("library/tcpip/")
	tcpipStackSnmpV3HeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackSnmpV3HeaderFile.setType("HEADER")
	tcpipStackSnmpV3HeaderFile.setOverwrite(True)	

	# Add snmpv3_private.h file to project
	tcpipStackSnmpV3PrivateHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackSnmpV3PrivateHeaderFile.setSourcePath("tcpip/src/snmpv3_private.h")
	tcpipStackSnmpV3PrivateHeaderFile.setOutputName("snmpv3_private.h")
	tcpipStackSnmpV3PrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackSnmpV3PrivateHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackSnmpV3PrivateHeaderFile.setType("HEADER")
	tcpipStackSnmpV3PrivateHeaderFile.setOverwrite(True)
			
	# file TCPIP_ANNOUNCE_H "$HARMONY_VERSION_PATH/framework/tcpip/tcpip_announce.h" to "$PROJECT_HEADER_FILES/framework/tcpip/tcpip_announce.h"
	tcpipStackAnnounceHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackAnnounceHeaderFile.setSourcePath("tcpip/tcpip_announce.h")
	tcpipStackAnnounceHeaderFile.setOutputName("tcpip_announce.h")
	tcpipStackAnnounceHeaderFile.setDestPath("library/tcpip/")
	tcpipStackAnnounceHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackAnnounceHeaderFile.setType("HEADER")
	tcpipStackAnnounceHeaderFile.setOverwrite(True)
	
	# Add tcpip_announce_manager.h file to project
	tcpipStackAnnounceManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackAnnounceManagerHeaderFile.setSourcePath("tcpip/src/tcpip_announce_manager.h")
	tcpipStackAnnounceManagerHeaderFile.setOutputName("tcpip_announce_manager.h")
	tcpipStackAnnounceManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackAnnounceManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackAnnounceManagerHeaderFile.setType("HEADER")
	tcpipStackAnnounceManagerHeaderFile.setOverwrite(True)


	# file LLDP_H "$HARMONY_VERSION_PATH/framework/tcpip/lldp.h" to "$PROJECT_HEADER_FILES/framework/tcpip/lldp.h"
	tcpipStackLldpHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackLldpHeaderFile.setSourcePath("tcpip/lldp.h")
	tcpipStackLldpHeaderFile.setOutputName("lldp.h")
	tcpipStackLldpHeaderFile.setDestPath("library/tcpip/")
	tcpipStackLldpHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackLldpHeaderFile.setType("HEADER")
	tcpipStackLldpHeaderFile.setOverwrite(True)
	
	# Add smtpc.h file to project
	tcpipStackSmtpcHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackSmtpcHeaderFile.setSourcePath("tcpip/smtpc.h")
	tcpipStackSmtpcHeaderFile.setOutputName("smtpc.h")
	tcpipStackSmtpcHeaderFile.setDestPath("library/tcpip/")
	tcpipStackSmtpcHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackSmtpcHeaderFile.setType("HEADER")
	tcpipStackSmtpcHeaderFile.setOverwrite(True)

	# Add smtpc_manager.h file to project
	tcpipStackSmtpcManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackSmtpcManagerHeaderFile.setSourcePath("tcpip/src/smtpc_manager.h")
	tcpipStackSmtpcManagerHeaderFile.setOutputName("smtpc_manager.h")
	tcpipStackSmtpcManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackSmtpcManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackSmtpcManagerHeaderFile.setType("HEADER")
	tcpipStackSmtpcManagerHeaderFile.setOverwrite(True)

	# Add smtpc_private.h file to project
	tcpipStackSmtpcPrivateHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackSmtpcPrivateHeaderFile.setSourcePath("tcpip/src/smtpc_private.h")
	tcpipStackSmtpcPrivateHeaderFile.setOutputName("smtpc_private.h")
	tcpipStackSmtpcPrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackSmtpcPrivateHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackSmtpcPrivateHeaderFile.setType("HEADER")
	tcpipStackSmtpcPrivateHeaderFile.setOverwrite(True)
	
	# Add igmp.h file to project
	tcpipStackIgmpHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackIgmpHeaderFile.setSourcePath("tcpip/igmp.h")
	tcpipStackIgmpHeaderFile.setOutputName("igmp.h")
	tcpipStackIgmpHeaderFile.setDestPath("library/tcpip/")
	tcpipStackIgmpHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackIgmpHeaderFile.setType("HEADER")
	tcpipStackIgmpHeaderFile.setOverwrite(True)

	# Add igmp_manager.h file to project
	tcpipStackIgmpManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackIgmpManagerHeaderFile.setSourcePath("tcpip/src/igmp_manager.h")
	tcpipStackIgmpManagerHeaderFile.setOutputName("igmp_manager.h")
	tcpipStackIgmpManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackIgmpManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackIgmpManagerHeaderFile.setType("HEADER")
	tcpipStackIgmpManagerHeaderFile.setOverwrite(True)

	# Add igmp_private.h file to project
	tcpipStackIgmpPrivateHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackIgmpPrivateHeaderFile.setSourcePath("tcpip/src/igmp_private.h")
	tcpipStackIgmpPrivateHeaderFile.setOutputName("igmp_private.h")
	tcpipStackIgmpPrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackIgmpPrivateHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackIgmpPrivateHeaderFile.setType("HEADER")
	tcpipStackIgmpPrivateHeaderFile.setOverwrite(True)
	
	# Add tftpc.h file to project
	tcpipStackTftpcHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTftpcHeaderFile.setSourcePath("tcpip/tftpc.h")
	tcpipStackTftpcHeaderFile.setOutputName("tftpc.h")
	tcpipStackTftpcHeaderFile.setDestPath("library/tcpip/")
	tcpipStackTftpcHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackTftpcHeaderFile.setType("HEADER")
	tcpipStackTftpcHeaderFile.setOverwrite(True)

	# Add tftpc_manager.h file to project
	tcpipStackTftpcManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTftpcManagerHeaderFile.setSourcePath("tcpip/src/tftpc_manager.h")
	tcpipStackTftpcManagerHeaderFile.setOutputName("tftpc_manager.h")
	tcpipStackTftpcManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackTftpcManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackTftpcManagerHeaderFile.setType("HEADER")
	tcpipStackTftpcManagerHeaderFile.setOverwrite(True)

	# Add tftpc_private.h file to project
	tcpipStackTftpcPrivateHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTftpcPrivateHeaderFile.setSourcePath("tcpip/src/tftpc_private.h")
	tcpipStackTftpcPrivateHeaderFile.setOutputName("tftpc_private.h")
	tcpipStackTftpcPrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackTftpcPrivateHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackTftpcPrivateHeaderFile.setType("HEADER")
	tcpipStackTftpcPrivateHeaderFile.setOverwrite(True)
		
	# file TCPIP_REBOOT_H "$HARMONY_VERSION_PATH/framework/tcpip/tcpip_reboot.h" to "$PROJECT_HEADER_FILES/framework/tcpip/tcpip_reboot.h"
	tcpipStackRebootHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackRebootHeaderFile.setSourcePath("tcpip/tcpip_reboot.h")
	tcpipStackRebootHeaderFile.setOutputName("tcpip_reboot.h")
	tcpipStackRebootHeaderFile.setDestPath("library/tcpip/")
	tcpipStackRebootHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackRebootHeaderFile.setType("HEADER")
	tcpipStackRebootHeaderFile.setOverwrite(True)
	
	# Add tcpip_reboot_manager.h file to project
	tcpipStackRebootManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackRebootManagerHeaderFile.setSourcePath("tcpip/src/tcpip_reboot_manager.h")
	tcpipStackRebootManagerHeaderFile.setOutputName("tcpip_reboot_manager.h")
	tcpipStackRebootManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackRebootManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackRebootManagerHeaderFile.setType("HEADER")
	tcpipStackRebootManagerHeaderFile.setOverwrite(True)
	
	# Add iperf.h file to project
	tcpipStackIperfHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackIperfHeaderFile.setSourcePath("tcpip/iperf.h")
	tcpipStackIperfHeaderFile.setOutputName("iperf.h")
	tcpipStackIperfHeaderFile.setDestPath("library/tcpip/")
	tcpipStackIperfHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackIperfHeaderFile.setType("HEADER")
	tcpipStackIperfHeaderFile.setOverwrite(True)

	# Add iperf_manager.h file to project
	tcpipStackIperfManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackIperfManagerHeaderFile.setSourcePath("tcpip/src/iperf_manager.h")
	tcpipStackIperfManagerHeaderFile.setOutputName("iperf_manager.h")
	tcpipStackIperfManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackIperfManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackIperfManagerHeaderFile.setType("HEADER")
	tcpipStackIperfManagerHeaderFile.setOverwrite(True)
	
	# Add tcpip_commands.h file to project
	tcpipStackCmdHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackCmdHeaderFile.setSourcePath("tcpip/tcpip_commands.h")
	tcpipStackCmdHeaderFile.setOutputName("tcpip_commands.h")
	tcpipStackCmdHeaderFile.setDestPath("library/tcpip/")
	tcpipStackCmdHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackCmdHeaderFile.setType("HEADER")
	tcpipStackCmdHeaderFile.setOverwrite(True)

	# Add tcpip_commands_manager.h file to project
	tcpipStackCmdManagerHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackCmdManagerHeaderFile.setSourcePath("tcpip/src/tcpip_commands_manager.h")
	tcpipStackCmdManagerHeaderFile.setOutputName("tcpip_commands_manager.h")
	tcpipStackCmdManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackCmdManagerHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackCmdManagerHeaderFile.setType("HEADER")
	tcpipStackCmdManagerHeaderFile.setOverwrite(True)
						
	# file TCPIP_COMMON_HELPERS_H "$HARMONY_VERSION_PATH/framework/tcpip/src/common/helpers.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/common/helpers.h"
	tcpipStackHelpersHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackHelpersHeaderFile.setSourcePath("tcpip/src/common/helpers.h")
	tcpipStackHelpersHeaderFile.setOutputName("helpers.h")
	tcpipStackHelpersHeaderFile.setDestPath("library/tcpip/src/common/")
	tcpipStackHelpersHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/common/")
	tcpipStackHelpersHeaderFile.setType("HEADER")
	tcpipStackHelpersHeaderFile.setOverwrite(True)
	
	# file TCPIP_HASH_FNV_H "$HARMONY_VERSION_PATH/framework/tcpip/src/hash_fnv.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/hash_fnv.h"
	tcpipStackHashFnvHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackHashFnvHeaderFile.setSourcePath("tcpip/src/hash_fnv.h")
	tcpipStackHashFnvHeaderFile.setOutputName("hash_fnv.h")
	tcpipStackHashFnvHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackHashFnvHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackHashFnvHeaderFile.setType("HEADER")
	tcpipStackHashFnvHeaderFile.setOverwrite(True)
		
	# file TCPIP_LINKED_LIST_H "$HARMONY_VERSION_PATH/framework/tcpip/src/link_list.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/link_list.h"
	tcpipStackLinkListHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackLinkListHeaderFile.setSourcePath("tcpip/src/link_list.h")
	tcpipStackLinkListHeaderFile.setOutputName("link_list.h")
	tcpipStackLinkListHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackLinkListHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackLinkListHeaderFile.setType("HEADER")
	tcpipStackLinkListHeaderFile.setOverwrite(True)
			
	# file TCPIP_OAHASH_H "$HARMONY_VERSION_PATH/framework/tcpip/src/oahash.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/oahash.h"
	tcpipStackOaHashHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackOaHashHeaderFile.setSourcePath("tcpip/src/oahash.h")
	tcpipStackOaHashHeaderFile.setOutputName("oahash.h")
	tcpipStackOaHashHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackOaHashHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackOaHashHeaderFile.setType("HEADER")
	tcpipStackOaHashHeaderFile.setOverwrite(True)

	# file TCPIP_HELPERS_PRIVATE_H "$HARMONY_VERSION_PATH/framework/tcpip/src/tcpip_helpers_private.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/tcpip_helpers_private.h"
	tcpipStackTcpipHelperPrivHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipHelperPrivHeaderFile.setSourcePath("tcpip/src/tcpip_helpers_private.h")
	tcpipStackTcpipHelperPrivHeaderFile.setOutputName("tcpip_helpers_private.h")
	tcpipStackTcpipHelperPrivHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackTcpipHelperPrivHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackTcpipHelperPrivHeaderFile.setType("HEADER")
	tcpipStackTcpipHelperPrivHeaderFile.setOverwrite(True)
		
	# file TCPIP_MANAGER_CONTROL_H "$HARMONY_VERSION_PATH/framework/tcpip/src/tcpip_manager_control.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/tcpip_manager_control.h"
	tcpipStackTcpipMngrCtrlHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipMngrCtrlHeaderFile.setSourcePath("tcpip/src/tcpip_manager_control.h")
	tcpipStackTcpipMngrCtrlHeaderFile.setOutputName("tcpip_manager_control.h")
	tcpipStackTcpipMngrCtrlHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackTcpipMngrCtrlHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackTcpipMngrCtrlHeaderFile.setType("HEADER")
	tcpipStackTcpipMngrCtrlHeaderFile.setOverwrite(True)
		
	# file TCPIP_MODULE_MANAGER_H "$HARMONY_VERSION_PATH/framework/tcpip/src/tcpip_module_manager.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/tcpip_module_manager.h"
	tcpipStackTcpipModlMngrHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipModlMngrHeaderFile.setSourcePath("tcpip/src/tcpip_module_manager.h")
	tcpipStackTcpipModlMngrHeaderFile.setOutputName("tcpip_module_manager.h")
	tcpipStackTcpipModlMngrHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackTcpipModlMngrHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackTcpipModlMngrHeaderFile.setType("HEADER")
	tcpipStackTcpipModlMngrHeaderFile.setOverwrite(True)
		
	# file TCPIP_NOTIFY_H "$HARMONY_VERSION_PATH/framework/tcpip/src/tcpip_notify.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/tcpip_notify.h"
	tcpipStackTcpipNotfyHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipNotfyHeaderFile.setSourcePath("tcpip/src/tcpip_notify.h")
	tcpipStackTcpipNotfyHeaderFile.setOutputName("tcpip_notify.h")
	tcpipStackTcpipNotfyHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackTcpipNotfyHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackTcpipNotfyHeaderFile.setType("HEADER")
	tcpipStackTcpipNotfyHeaderFile.setOverwrite(True)
	
	# file TCPIP_PACKET_H "$HARMONY_VERSION_PATH/framework/tcpip/src/tcpip_packet.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/tcpip_packet.h"
	tcpipStackTcpipPcktHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipPcktHeaderFile.setSourcePath("tcpip/src/tcpip_packet.h")
	tcpipStackTcpipPcktHeaderFile.setOutputName("tcpip_packet.h")
	tcpipStackTcpipPcktHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackTcpipPcktHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackTcpipPcktHeaderFile.setType("HEADER")
	tcpipStackTcpipPcktHeaderFile.setOverwrite(True)
		
	# file TCPIP_PRIVATE_H "$HARMONY_VERSION_PATH/framework/tcpip/src/tcpip_private.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/tcpip_private.h"
	tcpipStackTcpipPrivHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipPrivHeaderFile.setSourcePath("tcpip/config/tcpip_private.h.ftl")
	tcpipStackTcpipPrivHeaderFile.setOutputName("tcpip_private.h")
	tcpipStackTcpipPrivHeaderFile.setMarkup(True)
	tcpipStackTcpipPrivHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackTcpipPrivHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackTcpipPrivHeaderFile.setType("HEADER")
	tcpipStackTcpipPrivHeaderFile.setOverwrite(True)
			
	# file TCPIP_TYPES_H "$HARMONY_VERSION_PATH/framework/tcpip/src/tcpip_types.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/tcpip_types.h"
	tcpipStackTcpipTypeHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipTypeHeaderFile.setSourcePath("tcpip/src/tcpip_types.h")
	tcpipStackTcpipTypeHeaderFile.setOutputName("tcpip_types.h")
	tcpipStackTcpipTypeHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackTcpipTypeHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackTcpipTypeHeaderFile.setType("HEADER")
	tcpipStackTcpipTypeHeaderFile.setOverwrite(True)

	
	tcpipStackTcpipHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipHeaderFile.setSourcePath("tcpip/config/tcpip.h.ftl")
	tcpipStackTcpipHeaderFile.setOutputName("tcpip.h")
	tcpipStackTcpipHeaderFile.setMarkup(True)
	tcpipStackTcpipHeaderFile.setDestPath("library/tcpip/")
	tcpipStackTcpipHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackTcpipHeaderFile.setType("HEADER")
	tcpipStackTcpipHeaderFile.setOverwrite(True)	

	# file BERKELEY_MANAGER_H "$HARMONY_VERSION_PATH/framework/tcpip/src/berkeley_manager.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/berkeley_manager.h"
	tcpipStackBrklyMngrHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackBrklyMngrHeaderFile.setSourcePath("tcpip/src/berkeley_manager.h")
	tcpipStackBrklyMngrHeaderFile.setOutputName("berkeley_manager.h")
	tcpipStackBrklyMngrHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackBrklyMngrHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackBrklyMngrHeaderFile.setType("HEADER")
	tcpipStackBrklyMngrHeaderFile.setOverwrite(True)
				
	# file DHCPV6_MANAGER_H "$HARMONY_VERSION_PATH/framework/tcpip/src/dhcpv6_manager.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/dhcpv6_manager.h"
	tcpipStackDhcpv6MngrHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackDhcpv6MngrHeaderFile.setSourcePath("tcpip/src/dhcpv6_manager.h")
	tcpipStackDhcpv6MngrHeaderFile.setOutputName("dhcpv6_manager.h")
	tcpipStackDhcpv6MngrHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackDhcpv6MngrHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackDhcpv6MngrHeaderFile.setType("HEADER")
	tcpipStackDhcpv6MngrHeaderFile.setOverwrite(True)
				
	# file DHCPV6_PRIVATE_H "$HARMONY_VERSION_PATH/framework/tcpip/src/dhcpv6_private.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/dhcpv6_private.h"
	tcpipStackDhcpv6PrivHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackDhcpv6PrivHeaderFile.setSourcePath("tcpip/src/dhcpv6_private.h")
	tcpipStackDhcpv6PrivHeaderFile.setOutputName("dhcpv6_private.h")
	tcpipStackDhcpv6PrivHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackDhcpv6PrivHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackDhcpv6PrivHeaderFile.setType("HEADER")
	tcpipStackDhcpv6PrivHeaderFile.setOverwrite(True)
				
	# file LLDP_MANAGER_H "$HARMONY_VERSION_PATH/framework/tcpip/src/lldp_manager.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/lldp_manager.h"
	tcpipStackLldpMngrHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackLldpMngrHeaderFile.setSourcePath("tcpip/src/lldp_manager.h")
	tcpipStackLldpMngrHeaderFile.setOutputName("lldp_manager.h")
	tcpipStackLldpMngrHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackLldpMngrHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackLldpMngrHeaderFile.setType("HEADER")
	tcpipStackLldpMngrHeaderFile.setOverwrite(True)
			
	# file LLDP_PRIVATE_H "$HARMONY_VERSION_PATH/framework/tcpip/src/lldp_private.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/lldp_private.h"
	tcpipStackLldpPrivHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackLldpPrivHeaderFile.setSourcePath("tcpip/src/lldp_private.h")
	tcpipStackLldpPrivHeaderFile.setOutputName("lldp_private.h")
	tcpipStackLldpPrivHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackLldpPrivHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackLldpPrivHeaderFile.setType("HEADER")
	tcpipStackLldpPrivHeaderFile.setOverwrite(True)
			
	# file LLDP_TLV_H "$HARMONY_VERSION_PATH/framework/tcpip/src/lldp_tlv.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/lldp_tlv.h"
	tcpipStackLldpTlvHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackLldpTlvHeaderFile.setSourcePath("tcpip/src/lldp_tlv.h")
	tcpipStackLldpTlvHeaderFile.setOutputName("lldp_tlv.h")
	tcpipStackLldpTlvHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackLldpTlvHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackLldpTlvHeaderFile.setType("HEADER")
	tcpipStackLldpTlvHeaderFile.setOverwrite(True)

	# file TCPIP_ETHERNET_H "$HARMONY_VERSION_PATH/framework/tcpip/tcpip_ethernet.h" to                           "$PROJECT_HEADER_FILES/framework/tcpip/tcpip_ethernet.h"
	tcpipStackTcpipEthernetHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipEthernetHeaderFile.setSourcePath("tcpip/tcpip_ethernet.h")
	tcpipStackTcpipEthernetHeaderFile.setOutputName("tcpip_ethernet.h")
	tcpipStackTcpipEthernetHeaderFile.setDestPath("library/tcpip/")
	tcpipStackTcpipEthernetHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/")
	tcpipStackTcpipEthernetHeaderFile.setType("HEADER")
	tcpipStackTcpipEthernetHeaderFile.setOverwrite(True)
	
	# Add sys_fs_wrapper.h file to project
	tcpipStackSysFsWrapperHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackSysFsWrapperHeaderFile.setSourcePath("tcpip/src/common/sys_fs_wrapper.h")
	tcpipStackSysFsWrapperHeaderFile.setOutputName("sys_fs_wrapper.h")
	tcpipStackSysFsWrapperHeaderFile.setDestPath("library/tcpip/src/common/")
	tcpipStackSysFsWrapperHeaderFile.setProjectPath("config/" + configName + "/library/tcpip/src/common/")
	tcpipStackSysFsWrapperHeaderFile.setType("HEADER")
	tcpipStackSysFsWrapperHeaderFile.setOverwrite(True)

	# file NET_PRES1_HTTP_H "$HARMONY_VERSION_PATH/framework/net/pres/net_pres.h"  to "$PROJECT_HEADER_FILES/framework/net/pres/net_pres.h"
	tcpipStackNetPresHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackNetPresHeaderFile.setSourcePath("net/pres/net_pres.h")
	tcpipStackNetPresHeaderFile.setOutputName("net_pres.h")
	tcpipStackNetPresHeaderFile.setDestPath("/net/pres/")
	tcpipStackNetPresHeaderFile.setProjectPath("config/" + configName + "/net/pres/")
	tcpipStackNetPresHeaderFile.setType("HEADER")
	tcpipStackNetPresHeaderFile.setOverwrite(True)

	#file NET_PRES4_H "$HARMONY_VERSION_PATH/framework/net/pres/net_pres_socketapi.h"  to "$PROJECT_HEADER_FILES/framework/net/pres/net_pres_socketapi.h"
	tcpipStackNetPresSktApiHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackNetPresSktApiHeaderFile.setSourcePath("net/pres/net_pres_socketapi.h")
	tcpipStackNetPresSktApiHeaderFile.setOutputName("net_pres_socketapi.h")
	tcpipStackNetPresSktApiHeaderFile.setDestPath("/net/pres/")
	tcpipStackNetPresSktApiHeaderFile.setProjectPath("config/" + configName + "/net/pres/")
	tcpipStackNetPresSktApiHeaderFile.setType("HEADER")
	tcpipStackNetPresSktApiHeaderFile.setOverwrite(True)
	
	#file NET_PRES_SOCKETAPICONVERSION_H "$HARMONY_VERSION_PATH/framework/net/pres/net_pres_socketapiconversion.h"  to "$PROJECT_HEADER_FILES/framework/net/pres/net_pres_socketapiconversion.h"	
	tcpipStackNetPresSktApiConvHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackNetPresSktApiConvHeaderFile.setSourcePath("net/pres/net_pres_socketapiconversion.h")
	tcpipStackNetPresSktApiConvHeaderFile.setOutputName("net_pres_socketapiconversion.h")
	tcpipStackNetPresSktApiConvHeaderFile.setDestPath("/net/pres/")
	tcpipStackNetPresSktApiConvHeaderFile.setProjectPath("config/" + configName + "/net/pres/")
	tcpipStackNetPresSktApiConvHeaderFile.setType("HEADER")
	tcpipStackNetPresSktApiConvHeaderFile.setOverwrite(True)	
	
	# add "<#include \"/framework/tcpip/config/tcpip_stack.c.ftl\">"  to list SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA
	tcpipStackSourceFtl = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackSourceFtl.setType("STRING")
	tcpipStackSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
	tcpipStackSourceFtl.setSourcePath("tcpip/config/tcpip_stack.c.ftl")
	tcpipStackSourceFtl.setMarkup(True)
	
	# add "<#include \"/framework/tcpip/config/tcpip_stack_init.c.ftl\">"  to list SYSTEM_INIT_C_INITIALIZE_MIDDLEWARE
	tcpipStackInitSourceFtl = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackInitSourceFtl.setType("STRING")
	tcpipStackInitSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_INITIALIZE_MIDDLEWARE")
	tcpipStackInitSourceFtl.setSourcePath("tcpip/config/tcpip_stack_init.c.ftl")
	tcpipStackInitSourceFtl.setMarkup(True)
		
	# add "<#include \"/framework/tcpip/config/tcpip_stack_tasks.c.ftl\">"  to list SYSTEM_TASKS_C_CALL_LIB_TASKS
	tcpipStackTaskSourceFtl = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTaskSourceFtl.setType("STRING")
	tcpipStackTaskSourceFtl.setOutputName("core.LIST_SYSTEM_TASKS_C_CALL_LIB_TASKS")
	tcpipStackTaskSourceFtl.setSourcePath("tcpip/config/tcpip_stack_tasks.c.ftl")
	tcpipStackTaskSourceFtl.setMarkup(True)
		
	# file TCPIP_COMMON_HELPERS_C "$HARMONY_VERSION_PATH/framework/tcpip/src/common/helpers.c" to "$PROJECT_SOURCE_FILES/framework/tcpip/src/common/helpers.c"
	tcpipStackHelpersSourceFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackHelpersSourceFile.setSourcePath("tcpip/src/common/helpers.c")
	tcpipStackHelpersSourceFile.setOutputName("helpers.c")
	tcpipStackHelpersSourceFile.setOverwrite(True)
	tcpipStackHelpersSourceFile.setDestPath("library/tcpip/src/")
	tcpipStackHelpersSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/common/")
	tcpipStackHelpersSourceFile.setType("SOURCE")
	tcpipStackHelpersSourceFile.setEnabled(True)	
	
	# file TCPIP_HASH_FNV_C "$HARMONY_VERSION_PATH/framework/tcpip/src/hash_fnv.c" to "$PROJECT_SOURCE_FILES/framework/tcpip/src/hash_fnv.c"
	tcpipStackHashFnvSourceFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackHashFnvSourceFile.setSourcePath("tcpip/src/hash_fnv.c")
	tcpipStackHashFnvSourceFile.setOutputName("hash_fnv.c")
	tcpipStackHashFnvSourceFile.setOverwrite(True)
	tcpipStackHashFnvSourceFile.setDestPath("library/tcpip/src/")
	tcpipStackHashFnvSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackHashFnvSourceFile.setType("SOURCE")
	tcpipStackHashFnvSourceFile.setEnabled(True)	
		
	# file TCPIP_OAHASH_C "$HARMONY_VERSION_PATH/framework/tcpip/src/oahash.c" to "$PROJECT_SOURCE_FILES/framework/tcpip/src/oahash.c"
	tcpipStackOaHashSourceFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackOaHashSourceFile.setSourcePath("tcpip/src/oahash.c")
	tcpipStackOaHashSourceFile.setOutputName("oahash.c")
	tcpipStackOaHashSourceFile.setOverwrite(True)
	tcpipStackOaHashSourceFile.setDestPath("library/tcpip/src/")
	tcpipStackOaHashSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackOaHashSourceFile.setType("SOURCE")
	tcpipStackOaHashSourceFile.setEnabled(True)	
			
	# file TCPIP_HELPERS_C "$HARMONY_VERSION_PATH/framework/tcpip/src/tcpip_helpers.c" to "$PROJECT_SOURCE_FILES/framework/tcpip/src/tcpip_helpers.c"
	tcpipStackTcpipHelpersSourceFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipHelpersSourceFile.setSourcePath("tcpip/src/tcpip_helpers.c")
	tcpipStackTcpipHelpersSourceFile.setOutputName("tcpip_helpers.c")
	tcpipStackTcpipHelpersSourceFile.setOverwrite(True)
	tcpipStackTcpipHelpersSourceFile.setDestPath("library/tcpip/src/")
	tcpipStackTcpipHelpersSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackTcpipHelpersSourceFile.setType("SOURCE")
	tcpipStackTcpipHelpersSourceFile.setEnabled(True)	

	tcpipStackSystemDefFile = tcpipStackComponent.createFileSymbol("TCPIP_H_FILE", None)
	tcpipStackSystemDefFile.setType("STRING")
	tcpipStackSystemDefFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
	tcpipStackSystemDefFile.setSourcePath("tcpip/templates/system/system_definitions.h.ftl")
	tcpipStackSystemDefFile.setMarkup(True)

	tcpipStackSystemDefObjFile = tcpipStackComponent.createFileSymbol("TCPIP_DEF_OBJ", None)
	tcpipStackSystemDefObjFile.setType("STRING")
	tcpipStackSystemDefObjFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_OBJECTS")
	tcpipStackSystemDefObjFile.setSourcePath("tcpip/templates/system/system_definitions_objects.h.ftl")
	tcpipStackSystemDefObjFile.setMarkup(True)
	
	# ifblock !DSTBDPIC32CZ
	# file TCPIP_HELPERS_C_32 "$HARMONY_VERSION_PATH/framework/tcpip/src/tcpip_helper_c32.S" to "$PROJECT_SOURCE_FILES/framework/tcpip/src/tcpip_helper_c32.S"
	# endif
	
	if not Peripheral.moduleExists("GMAC"):
		tcpipStackTcpipHelpersC32SourceFile = tcpipStackComponent.createFileSymbol(None, None)
		tcpipStackTcpipHelpersC32SourceFile.setSourcePath("tcpip/src/tcpip_helper_c32.S")
		tcpipStackTcpipHelpersC32SourceFile.setOutputName("tcpip_helper_c32.S")
		tcpipStackTcpipHelpersC32SourceFile.setOverwrite(True)
		tcpipStackTcpipHelpersC32SourceFile.setDestPath("library/tcpip/src/")
		tcpipStackTcpipHelpersC32SourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
		tcpipStackTcpipHelpersC32SourceFile.setType("SOURCE")
		tcpipStackTcpipHelpersC32SourceFile.setEnabled(True)	
			
	# file TCPIP_MANAGER_C "$HARMONY_VERSION_PATH/framework/tcpip/src/tcpip_manager.c" to "$PROJECT_SOURCE_FILES/framework/tcpip/src/tcpip_manager.c"
	tcpipStackTcpipMngrSourceFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipMngrSourceFile.setSourcePath("tcpip/src/tcpip_manager.c")
	tcpipStackTcpipMngrSourceFile.setOutputName("tcpip_manager.c")
	tcpipStackTcpipMngrSourceFile.setOverwrite(True)
	tcpipStackTcpipMngrSourceFile.setDestPath("library/tcpip/src/")
	tcpipStackTcpipMngrSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackTcpipMngrSourceFile.setType("SOURCE")
	tcpipStackTcpipMngrSourceFile.setEnabled(True)	
			
	# file TCPIP_NOTIFY_C "$HARMONY_VERSION_PATH/framework/tcpip/src/tcpip_notify.c" to "$PROJECT_SOURCE_FILES/framework/tcpip/src/tcpip_notify.c"
	tcpipStackTcpipNotifySourceFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipNotifySourceFile.setSourcePath("tcpip/src/tcpip_notify.c")
	tcpipStackTcpipNotifySourceFile.setOutputName("tcpip_notify.c")
	tcpipStackTcpipNotifySourceFile.setOverwrite(True)
	tcpipStackTcpipNotifySourceFile.setDestPath("library/tcpip/src/")
	tcpipStackTcpipNotifySourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackTcpipNotifySourceFile.setType("SOURCE")
	tcpipStackTcpipNotifySourceFile.setEnabled(True)	
		
	# file TCPIP_PACKET_C "$HARMONY_VERSION_PATH/framework/tcpip/src/tcpip_packet.c" to "$PROJECT_SOURCE_FILES/framework/tcpip/src/tcpip_packet.c"
	tcpipStackTcpipPacketSourceFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipPacketSourceFile.setSourcePath("tcpip/src/tcpip_packet.c")
	tcpipStackTcpipPacketSourceFile.setOutputName("tcpip_packet.c")
	tcpipStackTcpipPacketSourceFile.setOverwrite(True)
	tcpipStackTcpipPacketSourceFile.setDestPath("library/tcpip/src/")
	tcpipStackTcpipPacketSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipStackTcpipPacketSourceFile.setType("SOURCE")
	tcpipStackTcpipPacketSourceFile.setEnabled(True)

	# set TCP/IP include paths
	tcpipStackInclPath = tcpipStackComponent.createSettingSymbol("TCPIP_INCLUDE_DIRS", None)
	tcpipStackInclPath.setCategory("C32")
	tcpipStackInclPath.setKey("extra-include-directories")
	tcpipStackInclPath.setValue("../src/config/"+configName+ "/library;../src/config/"+configName+ "/library/tcpip/src;../src/config/"+configName+ "/library/tcpip/src/common")
	tcpipStackInclPath.setAppend(True, ";")
#########################################################################################
#### H3TODO: Adding H2 sys adapters temporarily; this will be moved to respective modules #######
	tcpipStackSysTimeAdapterHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackSysTimeAdapterHeaderFile.setSourcePath("sys_adapter/sys_time_h2_adapter.h")
	tcpipStackSysTimeAdapterHeaderFile.setOutputName("sys_time_h2_adapter.h")
	tcpipStackSysTimeAdapterHeaderFile.setDestPath("system/")
	tcpipStackSysTimeAdapterHeaderFile.setProjectPath("config/" + configName + "/system/")
	tcpipStackSysTimeAdapterHeaderFile.setType("HEADER")
	tcpipStackSysTimeAdapterHeaderFile.setOverwrite(True)
	
	tcpipStackSysTimeAdapterSourceFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackSysTimeAdapterSourceFile.setSourcePath("sys_adapter/sys_time_h2_adapter.c")
	tcpipStackSysTimeAdapterSourceFile.setOutputName("sys_time_h2_adapter.c")
	tcpipStackSysTimeAdapterSourceFile.setOverwrite(True)
	tcpipStackSysTimeAdapterSourceFile.setDestPath("system/")
	tcpipStackSysTimeAdapterSourceFile.setProjectPath("config/" + configName + "/system/")
	tcpipStackSysTimeAdapterSourceFile.setType("SOURCE")
	tcpipStackSysTimeAdapterSourceFile.setEnabled(True)
	
	tcpipStackSysIntAdapterHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackSysIntAdapterHeaderFile.setSourcePath("sys_adapter/sys_int_h2_adapter.h")
	tcpipStackSysIntAdapterHeaderFile.setOutputName("sys_int_h2_adapter.h")
	tcpipStackSysIntAdapterHeaderFile.setDestPath("system/")
	tcpipStackSysIntAdapterHeaderFile.setProjectPath("config/" + configName + "/system/")
	tcpipStackSysIntAdapterHeaderFile.setType("HEADER")
	tcpipStackSysIntAdapterHeaderFile.setOverwrite(True)	
	
	tcpipStackSysClkAdapterHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackSysClkAdapterHeaderFile.setSourcePath("sys_adapter/sys_clk_h2_adapter.h")
	tcpipStackSysClkAdapterHeaderFile.setOutputName("sys_clk_h2_adapter.h")
	tcpipStackSysClkAdapterHeaderFile.setDestPath("system/")
	tcpipStackSysClkAdapterHeaderFile.setProjectPath("config/" + configName + "/system/")
	tcpipStackSysClkAdapterHeaderFile.setType("HEADER")
	tcpipStackSysClkAdapterHeaderFile.setOverwrite(True)	

	tcpipStackSysRandomAdapterHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackSysRandomAdapterHeaderFile.setSourcePath("sys_adapter/sys_random_h2_adapter.h")
	tcpipStackSysRandomAdapterHeaderFile.setOutputName("sys_random_h2_adapter.h")
	tcpipStackSysRandomAdapterHeaderFile.setDestPath("system/")
	tcpipStackSysRandomAdapterHeaderFile.setProjectPath("config/" + configName + "/system/")
	tcpipStackSysRandomAdapterHeaderFile.setType("HEADER")
	tcpipStackSysRandomAdapterHeaderFile.setOverwrite(True)		

	tcpipStackSysResetAdapterHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackSysResetAdapterHeaderFile.setSourcePath("sys_adapter/sys_reset_h2_adapter.h")
	tcpipStackSysResetAdapterHeaderFile.setOutputName("sys_reset_h2_adapter.h")
	tcpipStackSysResetAdapterHeaderFile.setDestPath("system/")
	tcpipStackSysResetAdapterHeaderFile.setProjectPath("config/" + configName + "/system/")
	tcpipStackSysResetAdapterHeaderFile.setType("HEADER")
	tcpipStackSysResetAdapterHeaderFile.setOverwrite(True)	
	
	tcpipStackSysConfigHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackSysConfigHeaderFile.setSourcePath("sys_adapter/system_config.h")
	tcpipStackSysConfigHeaderFile.setOutputName("system_config.h")
	tcpipStackSysConfigHeaderFile.setDestPath("")
	tcpipStackSysConfigHeaderFile.setProjectPath("config/" + configName + "/")
	tcpipStackSysConfigHeaderFile.setType("HEADER")
	tcpipStackSysConfigHeaderFile.setOverwrite(True)	
	
	tcpipStackSysDefHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackSysDefHeaderFile.setSourcePath("sys_adapter/system_definitions.h")
	tcpipStackSysDefHeaderFile.setOutputName("system_definitions.h")
	tcpipStackSysDefHeaderFile.setDestPath("")
	tcpipStackSysDefHeaderFile.setProjectPath("config/" + configName + "/")
	tcpipStackSysDefHeaderFile.setType("HEADER")
	tcpipStackSysDefHeaderFile.setOverwrite(True)
	
	tcpipStackSysAdapterSystemDefFile = tcpipStackComponent.createFileSymbol("ADAPTER_H_FILE", None)
	tcpipStackSysAdapterSystemDefFile.setType("STRING")
	tcpipStackSysAdapterSystemDefFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
	tcpipStackSysAdapterSystemDefFile.setSourcePath("sys_adapter/templates/system/system_definitions.h.ftl")
	tcpipStackSysAdapterSystemDefFile.setMarkup(True)
#########################################################################################	
		
def tcpipStackMenuVisible(symbol, event):
	if (event["value"] == True):
		print("TCPIP Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("TCPIP Menu Invisible.")
		symbol.setVisible(False)
		
def tcpipStackMenuInvisible(symbol, event):
	if (event["value"] == False):
		print("TCPIP Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("TCPIP Menu Invisible.")
		symbol.setVisible(False)	



######################heap###########################################
# make TCP/IP Stack Dynamic Allocation Trace enable
def tcpipStackHeapStackDynAllocateTraceVisible(tcpipDependentSymbol, event):
	if (event["value"] == True):
		tcpipDependentSymbol.setVisible(True)
	else:
		tcpipDependentSymbol.setVisible(False)
		

# make TCP/IP Stack Number of Trace Slots visible
def tcpipStackHeapStackTraceSlotsNumVisible(tcpipDependentSymbol, event):
	if (event["value"] == True):
		tcpipDependentSymbol.setVisible(True)
	else:
		tcpipDependentSymbol.setVisible(False)


def tcpipStackHeapMenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("Heap Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("Heap Menu Invisible.")
		symbol.setVisible(False)

		
def tcpipStackHeapInternMenuVisible(symbol, event):
	if (event["value"] == "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP"):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)

def tcpipStackHeapExternMenuInvisible(symbol, event):	
	if (event["value"] != "TCPIP_STACK_HEAP_TYPE_EXTERNAL_HEAP" ):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)
		
def tcpipStackHeapPoolMenuVisible(symbol, event):
	if (event["value"] == "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL" ):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)
		
def tcpipStackHeapAllocGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])
	
def tcpipStackHeapExternSourceFile(sourceFile, event):	
	if (event["value"] == "TCPIP_STACK_HEAP_TYPE_EXTERNAL_HEAP" ):
		sourceFile.setEnabled(True)
	else:
		sourceFile.setEnabled(False)
		
	
def tcpipStackHeapInternSourceFile(sourceFile, event):	
	if (event["value"] == "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP" ):
		sourceFile.setEnabled(True)
	else:
		sourceFile.setEnabled(False)
	
def tcpipStackHeapPoolSourceFile(sourceFile, event):	
	if (event["value"] == "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL" ):
		sourceFile.setEnabled(True)
	else:
		sourceFile.setEnabled(False)
		
###########################################################################################################
def tcpipStackHeapPoolEntryInstance(tcpipStackHeapSymbol, event):
	global tcpipPoolEntryInstancesNumPrev
	print("Start tcpipStackHeapPoolEntryInstance")
	
	if(event["id"] == "TCPIP_STACK_USE_HEAP_CONFIG" ):	
		tcpipStackHeapConfig = Database.getSymbolValue("tcpipStack","TCPIP_STACK_USE_HEAP_CONFIG")
		tcpipStackHeapPoolIndex = int(tcpipStackHeapSymbol.getID().strip("TCPIP_HEAP_POOL_ENTRY_NUMBERX"))
		if (tcpipStackHeapConfig == "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL" ):
			if(tcpipStackHeapPoolIndex < tcpipPoolEntryInstancesNumPrev ):
				tcpipStackHeapSymbol.setVisible(True)
		else:
			tcpipStackHeapSymbol.setVisible(False)
	
	else:				
		tcpipStackHeapPoolInstanceNumberValue = event["value"]
		if(tcpipStackHeapPoolInstanceNumberValue > tcpipPoolEntryInstancesNumPrev ):
			tcpipStackHeapPoolEntry[tcpipPoolEntryInstancesNumPrev].setVisible(True)
			tcpipStackHeapPoolEntry[tcpipPoolEntryInstancesNumPrev].setValue(True, 1)
			print("Set TRUE"+ str(tcpipPoolEntryInstancesNumPrev))
			tcpipPoolEntryInstancesNumPrev = tcpipPoolEntryInstancesNumPrev + 1
			#Add more network configurations
		else:
			if(tcpipStackHeapPoolInstanceNumberValue < tcpipPoolEntryInstancesNumPrev ):
				#Reduce network configurations
				tcpipPoolEntryInstancesNumPrev = tcpipPoolEntryInstancesNumPrev - 1
				tcpipStackHeapPoolEntry[tcpipPoolEntryInstancesNumPrev].setVisible(False)
				tcpipStackHeapPoolEntry[tcpipPoolEntryInstancesNumPrev].setValue(False, 1)
				print("Set FALSE"+ str(tcpipPoolEntryInstancesNumPrev))				
			#else:
				#Do Nothing
	print("END tcpipStackHeapPoolEntryInstance")
	
	
def tcpipStackHeapPoolInstnBlkSizeMenu(symbol, event):	
	tcpipStackHeapPoolInstnIndex = int(symbol.getID().strip("TCPIP_HEAP_POOL_ENTRY_SIZE_IDX"))	
	tcpipStackHeapPoolInstnEnable = tcpipStackHeapPoolEntry[tcpipStackHeapPoolInstnIndex].getValue()	
	tcpipStackHeapConfig = Database.getSymbolValue("tcpipStack","TCPIP_STACK_USE_HEAP_CONFIG")
	
	if tcpipStackHeapPoolInstnEnable and (tcpipStackHeapConfig == "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL" ):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)
		
def tcpipStackHeapPoolPreAllocBlkMenu(symbol, event):	
	tcpipStackHeapPoolInstnIndex = int(symbol.getID().strip("TCPIP_HEAP_POOL_ENTRY_BLOCKS_IDX"))	
	tcpipStackHeapPoolInstnEnable = tcpipStackHeapPoolEntry[tcpipStackHeapPoolInstnIndex].getValue()	
	tcpipStackHeapConfig = Database.getSymbolValue("tcpipStack","TCPIP_STACK_USE_HEAP_CONFIG")	
	
	if tcpipStackHeapPoolInstnEnable and (tcpipStackHeapConfig == "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL" ):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)
		
def tcpipStackHeapPoolExpBlkMenu(symbol, event):
	tcpipStackHeapPoolInstnIndex = int(symbol.getID().strip("TCPIP_HEAP_POOL_ENTRY_EXP_BLOCKS_IDX"))	
	tcpipStackHeapPoolInstnEnable = tcpipStackHeapPoolEntry[tcpipStackHeapPoolInstnIndex].getValue()
	tcpipStackHeapConfig = Database.getSymbolValue("tcpipStack","TCPIP_STACK_USE_HEAP_CONFIG")
	
	if tcpipStackHeapPoolInstnEnable and (tcpipStackHeapConfig == "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL" ):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)		

def tcpipStackshowRTOSMenu(symbol, event):
	if (event["value"] == None):
		symbol.setVisible(False)
		print("tcpip stack: OSAL Disabled")
	elif (event["value"] != "BareMetal"):
		# If not Bare Metal
		symbol.setVisible(True)
		print("tcpip stack rtos")
	else:
		symbol.setVisible(False)
		print("tcpip stack Bare Metal")
		
def tcpipStackRTOSStandaloneMenu(symbol, event):
	if (event["value"] == 'Standalone'):		
		symbol.setVisible(True)
		print("tcpip Stack Standalone")
	else:
		symbol.setVisible(False)
		print("tcpip Stack Combined")
		
def tcpipStackRTOSTaskDelayMenu(symbol, event):
	tcpipStackRtos = Database.getSymbolValue("tcpipStack","TCPIP_STACK_RTOS")
	tcpipStackRtosUseDelay = Database.getSymbolValue("tcpipStack","TCPIP_STACK_RTOS_USE_DELAY")
	if((tcpipStackRtos == 'Standalone') and tcpipStackRtosUseDelay):		
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)
		

def destroyComponent(component):
	Database.setSymbolValue("tcpipStack", "USE_TCPIP_STACK", False, 2)