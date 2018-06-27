TCPIP_STACK_HEAP_TYPE = ["TCPIP_STACK_HEAP_TYPE_EXTERNAL_HEAP", "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP", "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL"]
TCPIP_STACK_HEAP_FLAGS = ["TCPIP_STACK_HEAP_FLAG_NONE", "TCPIP_STACK_HEAP_FLAG_ALLOC_UNCACHED", "TCPIP_STACK_HEAP_FLAG_NO_MTHREAD_SYNC", "TCPIP_STACK_HEAP_FLAG_ALLOC_UNALIGN", "TCPIP_STACK_HEAP_FLAG_NO_WARN_MESSAGE"]
TCPIP_STACK_HEAP_USAGE = ["TCPIP_STACK_HEAP_USE_DEFAULT", "TCPIP_STACK_HEAP_USE_GENERIC", "TCPIP_STACK_HEAP_USE_PACKET", "TCPIP_STACK_HEAP_USE_SOCKET_TCP", "TCPIP_STACK_HEAP_USE_SOCKET_UDP","TCPIP_STACK_HEAP_USE_POOL_UDP"]

TCIPHEAP_POOL_DFLT_SIZE = [2064, 1616, 1024, 768, 512, 256, 128, 64, 32, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64]
TCIPHEAP_POOL_EXPBLK_NUM = [2, 2, 2, 2, 2, 4, 4, 8, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

tcpipHeapPoolNumMaximum = 20
tcpipPoolEntryInstancesNumPrev = 9
tcpipHeapPoolEntry = []
tcpipHeapPoolBlkSize = []
tcpipHeapPoolPreAllocBlkNum = []
tcpipHeapPoolExpBlkNum = []
def instantiateComponent(tcpipHeapComponent):
	print("TCPIP Heap Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	# Select Heap Configuration
	tcpipHeap = tcpipHeapComponent.createBooleanSymbol("TCPIP_USE_HEAP", None)
	tcpipHeap.setLabel("Select Heap Configuration")
	tcpipHeap.setVisible(False)
	tcpipHeap.setDescription("Select Heap Configuration")
	tcpipHeap.setDefaultValue(True)

	# Use Heap Config
	tcpipHeapConfig = tcpipHeapComponent.createComboSymbol("TCPIP_STACK_USE_HEAP_CONFIG", None, TCPIP_STACK_HEAP_TYPE)
	tcpipHeapConfig.setLabel("Use Heap Config")
	tcpipHeapConfig.setVisible(True)
	tcpipHeapConfig.setDescription("Use Heap Config")
	tcpipHeapConfig.setDefaultValue("TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP")
	#tcpipHeapConfig.setDependencies(tcpipHeapMenuVisibleSingle, ["TCPIP_USE_HEAP"])

	# TCP/IP Stack Dynamic RAM Size
	tcpipHeapStackDramSize = tcpipHeapComponent.createIntegerSymbol("TCPIP_STACK_DRAM_SIZE", None)
	tcpipHeapStackDramSize.setLabel("TCP/IP Stack Dynamic RAM Size")
	tcpipHeapStackDramSize.setVisible(True)
	tcpipHeapStackDramSize.setDescription("TCP/IP Stack Dynamic RAM Size")
	tcpipHeapStackDramSize.setDefaultValue(39250)
	tcpipHeapStackDramSize.setDependencies(tcpipHeapInternMenuVisible, ["TCPIP_STACK_USE_HEAP_CONFIG"])

	# Number of Heap Pool Entries
	tcpipHeapPoolEntryNum = tcpipHeapComponent.createIntegerSymbol("TCPIP_HEAP_POOL_ENTRIES_NUMBER", None)
	tcpipHeapPoolEntryNum.setLabel("Number of Heap Pool Entries")
	tcpipHeapPoolEntryNum.setVisible(True)
	tcpipHeapPoolEntryNum.setDescription("Number of Heap Pool Entries")
	tcpipHeapPoolEntryNum.setMin(1)
	tcpipHeapPoolEntryNum.setMax(20)
	tcpipHeapPoolEntryNum.setDefaultValue(9)
	tcpipHeapPoolEntryNum.setDependencies(tcpipHeapPoolMenuVisible, ["TCPIP_STACK_USE_HEAP_CONFIG"])
	
	# The Expansion Size for the Internal Heap Pool
	tcpipHeapStackPoolExpnSize = tcpipHeapComponent.createIntegerSymbol("TCPIP_STACK_POOL_EXPANSION_SIZE", None)
	tcpipHeapStackPoolExpnSize.setLabel("The Expansion Size for the Internal Heap Pool")
	tcpipHeapStackPoolExpnSize.setVisible(True)
	tcpipHeapStackPoolExpnSize.setDescription("The Expansion Size for the Internal Heap Pool")
	tcpipHeapStackPoolExpnSize.setDefaultValue(51200)
	tcpipHeapStackPoolExpnSize.setDependencies(tcpipHeapPoolMenuVisible, ["TCPIP_STACK_USE_HEAP_CONFIG"])
	
	# source "$HARMONY_VERSION_PATH/framework/tcpip/config/tcpip_pool_idx.ftl" 20 instances
#####################################################################################################
	for index in range(0,tcpipHeapPoolNumMaximum):	
		tcpipHeapPoolEntry.append(tcpipHeapComponent.createBooleanSymbol("TCPIP_HEAP_POOL_ENTRY_NUMBERX"+str(index),None))
		tcpipHeapPoolEntry[index].setLabel("Pool Entry "+ str(index))
		tcpipHeapPoolEntry[index].setVisible(False)
		tcpipHeapPoolEntry[index].setDefaultValue(True)
		tcpipHeapPoolEntry[index].setDependencies(tcpipHeapPoolEntryInstance, ["TCPIP_HEAP_POOL_ENTRIES_NUMBER","TCPIP_STACK_USE_HEAP_CONFIG"])

		# Heap Block Size
		tcpipHeapPoolBlkSize.append(tcpipHeapComponent.createIntegerSymbol("TCPIP_HEAP_POOL_ENTRY_SIZE_IDX" + str(index), tcpipHeapPoolEntry[index]))
		tcpipHeapPoolBlkSize[index].setLabel("Size of the Block, Bytes")
		tcpipHeapPoolBlkSize[index].setVisible(False)
		tcpipHeapPoolBlkSize[index].setDefaultValue(int(TCIPHEAP_POOL_DFLT_SIZE[index]))
		tcpipHeapPoolBlkSize[index].setMin(32)
		tcpipHeapPoolBlkSize[index].setMax(4096)
		tcpipHeapPoolBlkSize[index].setDependencies(tcpipHeapPoolInstnBlkSizeMenu, [tcpipHeapPoolEntry[index].getID(),"TCPIP_STACK_USE_HEAP_CONFIG"])

		# Heap Pre-allocated blocks
		tcpipHeapPoolPreAllocBlkNum.append(tcpipHeapComponent.createIntegerSymbol("TCPIP_HEAP_POOL_ENTRY_BLOCKS_IDX" + str(index), tcpipHeapPoolEntry[index]))
		tcpipHeapPoolPreAllocBlkNum[index].setLabel("Number of Pre-allocated Blocks with this Size")
		tcpipHeapPoolPreAllocBlkNum[index].setVisible(False)
		tcpipHeapPoolPreAllocBlkNum[index].setDefaultValue(0)
		tcpipHeapPoolPreAllocBlkNum[index].setMin(0)
		tcpipHeapPoolPreAllocBlkNum[index].setMax(4096)
		tcpipHeapPoolPreAllocBlkNum[index].setDependencies(tcpipHeapPoolPreAllocBlkMenu, [tcpipHeapPoolEntry[index].getID(),"TCPIP_STACK_USE_HEAP_CONFIG"])
		
		# Heap Pre-allocated blocks
		tcpipHeapPoolExpBlkNum.append(tcpipHeapComponent.createIntegerSymbol("TCPIP_HEAP_POOL_ENTRY_EXP_BLOCKS_IDX" + str(index), tcpipHeapPoolEntry[index]))
		tcpipHeapPoolExpBlkNum[index].setLabel("Number of Blocks to Dynamically Expand for this Pool Entry")
		tcpipHeapPoolExpBlkNum[index].setVisible(False)
		tcpipHeapPoolExpBlkNum[index].setDefaultValue(TCIPHEAP_POOL_EXPBLK_NUM[index])
		tcpipHeapPoolExpBlkNum[index].setMin(0)
		tcpipHeapPoolExpBlkNum[index].setMax(4096)
		tcpipHeapPoolExpBlkNum[index].setDependencies(tcpipHeapPoolExpBlkMenu, [tcpipHeapPoolEntry[index].getID(),"TCPIP_STACK_USE_HEAP_CONFIG"])
#####################################################################################################
	# Stack allocation function, malloc style
	tcpipHeapStackMalloc = tcpipHeapComponent.createStringSymbol("TCPIP_STACK_MALLOC_FUNC", None)
	tcpipHeapStackMalloc.setLabel("Stack allocation function, malloc style")
	tcpipHeapStackMalloc.setVisible(True)
	tcpipHeapStackMalloc.setDescription("Stack allocation function, malloc style")
	tcpipHeapStackMalloc.setDefaultValue("malloc")
	#tcpipHeapStackMalloc.setDependencies(tcpipHeapMenuVisibleSingle, ["TCPIP_USE_HEAP"])

	# Stack allocation function, calloc style
	tcpipHeapStackCalloc = tcpipHeapComponent.createStringSymbol("TCPIP_STACK_CALLOC_FUNC", None)
	tcpipHeapStackCalloc.setLabel("Stack allocation function, calloc style")
	tcpipHeapStackCalloc.setVisible(True)
	tcpipHeapStackCalloc.setDescription("Stack allocation function, calloc style")
	tcpipHeapStackCalloc.setDefaultValue("calloc")
	#tcpipHeapStackCalloc.setDependencies(tcpipHeapMenuVisibleSingle, ["TCPIP_USE_HEAP"])

	# Stack deallocation function, free style
	tcpipHeapStackFree = tcpipHeapComponent.createStringSymbol("TCPIP_STACK_FREE_FUNC", None)
	tcpipHeapStackFree.setLabel("Stack deallocation function, free style")
	tcpipHeapStackFree.setVisible(True)
	tcpipHeapStackFree.setDescription("Stack deallocation function, free style")
	tcpipHeapStackFree.setDefaultValue("free")
	#tcpipHeapStackFree.setDependencies(tcpipHeapMenuVisibleSingle, ["TCPIP_USE_HEAP"])

	# Flags supported for heap allocation
	tcpipHeapFlag = tcpipHeapComponent.createComboSymbol("TCPIP_STACK_HEAP_USE_FLAGS", None, TCPIP_STACK_HEAP_FLAGS)
	tcpipHeapFlag.setLabel("Flags supported for heap allocation")
	tcpipHeapFlag.setVisible(True)
	tcpipHeapFlag.setDescription("Flags supported for heap allocation")
	tcpipHeapFlag.setDefaultValue("TCPIP_STACK_HEAP_FLAG_ALLOC_UNCACHED")
	#tcpipHeapFlag.setDependencies(tcpipHeapMenuVisibleSingle, ["TCPIP_USE_HEAP"])

	# Heap area usage
	tcpipHeapUsageConfig = tcpipHeapComponent.createComboSymbol("TCPIP_STACK_HEAP_USAGE_CONFIG", None, TCPIP_STACK_HEAP_USAGE)
	tcpipHeapUsageConfig.setLabel("Heap area usage")
	tcpipHeapUsageConfig.setVisible(True)
	tcpipHeapUsageConfig.setDescription("Heap area usage")
	tcpipHeapUsageConfig.setDefaultValue("TCPIP_STACK_HEAP_USE_DEFAULT")
	#tcpipHeapUsageConfig.setDependencies(tcpipHeapMenuVisibleSingle, ["TCPIP_USE_HEAP"])

	# Maximum heap types
	tcpipHeapStackSupportHeap = tcpipHeapComponent.createIntegerSymbol("TCPIP_STACK_SUPPORTED_HEAPS", None)
	tcpipHeapStackSupportHeap.setLabel("Maximum heap types")
	tcpipHeapStackSupportHeap.setVisible(True)
	tcpipHeapStackSupportHeap.setDescription("Maximum heap types")
	tcpipHeapStackSupportHeap.setDefaultValue(1)
	#tcpipHeapStackSupportHeap.setDependencies(tcpipHeapMenuVisibleSingle, ["TCPIP_USE_HEAP"])

	# TCP/IP Stack Dynamic RAM Lower Limit
	tcpipHeapStackDramLim = tcpipHeapComponent.createIntegerSymbol("TCPIP_STACK_DRAM_RUN_LIMIT", None)
	tcpipHeapStackDramLim.setLabel("TCP/IP Stack Dynamic RAM Lower Limit")
	tcpipHeapStackDramLim.setVisible(True)
	tcpipHeapStackDramLim.setDescription("TCP/IP Stack Dynamic RAM Lower Limit")
	tcpipHeapStackDramLim.setDefaultValue(2048)
	tcpipHeapStackDramLim.setDependencies(tcpipHeapExternMenuInvisible, ["TCPIP_STACK_USE_HEAP_CONFIG"])

	# TCP/IP Stack Dynamic Allocation Debug
	tcpipHeapStackDramDebug = tcpipHeapComponent.createBooleanSymbol("TCPIP_STACK_DRAM_DEBUG_ENABLE", None)
	tcpipHeapStackDramDebug.setLabel("TCP/IP Stack Dynamic Allocation Debug")
	tcpipHeapStackDramDebug.setVisible(True)
	tcpipHeapStackDramDebug.setDescription("TCP/IP Stack Dynamic Allocation Debug")
	tcpipHeapStackDramDebug.setDefaultValue(False)
	#tcpipHeapStackDramDebug.setDependencies(tcpipHeapMenuVisibleSingle, ["TCPIP_USE_HEAP"])

	# TCP/IP Stack Dynamic Allocation Trace
	tcpipHeapStackDramTrace = tcpipHeapComponent.createBooleanSymbol("TCPIP_STACK_DRAM_TRACE_ENABLE", tcpipHeapStackDramDebug)
	tcpipHeapStackDramTrace.setLabel("TCP/IP Stack Dynamic Allocation Trace")
	tcpipHeapStackDramTrace.setVisible(False)
	tcpipHeapStackDramTrace.setDescription("TCP/IP Stack Dynamic Allocation Trace")
	tcpipHeapStackDramTrace.setDefaultValue(False)
	tcpipHeapStackDramTrace.setDependencies(tcpipHeapStackDynAllocateTraceVisible, ["TCPIP_STACK_DRAM_DEBUG_ENABLE"])

	# TCP/IP Stack Number of Trace Slots
	tcpipHeapStackDramTraceSlotsNum = tcpipHeapComponent.createIntegerSymbol("TCPIP_STACK_DRAM_TRACE_SLOTS", tcpipHeapStackDramDebug)
	tcpipHeapStackDramTraceSlotsNum.setLabel("TCP/IP Stack Number of Trace Slots")
	tcpipHeapStackDramTraceSlotsNum.setVisible(False)
	tcpipHeapStackDramTraceSlotsNum.setDescription("TCP/IP Stack Number of Trace Slots")
	tcpipHeapStackDramTraceSlotsNum.setDefaultValue(16)
	tcpipHeapStackDramTraceSlotsNum.setDependencies(tcpipHeapStackTraceSlotsNumVisible, ["TCPIP_STACK_DRAM_TRACE_ENABLE"])
	
	#Add to system_config.h
	tcpipHeapHeaderFtl = tcpipHeapComponent.createFileSymbol(None, None)
	tcpipHeapHeaderFtl.setSourcePath("tcpip/config/tcpip_heap.h.ftl")
	tcpipHeapHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipHeapHeaderFtl.setMarkup(True)
	tcpipHeapHeaderFtl.setType("STRING")	

	# Add tcpip_heap_alloc.c file
	tcpipHeapAllocSourceFile = tcpipHeapComponent.createFileSymbol(None, None)
	tcpipHeapAllocSourceFile.setSourcePath("tcpip/src/tcpip_heap_alloc.c")
	tcpipHeapAllocSourceFile.setOutputName("tcpip_heap_alloc.c")
	tcpipHeapAllocSourceFile.setOverwrite(True)
	tcpipHeapAllocSourceFile.setDestPath("library/tcpip/src/")
	tcpipHeapAllocSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipHeapAllocSourceFile.setType("SOURCE")
	tcpipHeapAllocSourceFile.setEnabled(True)
	#tcpipHeapAllocSourceFile.setDependencies(tcpipHeapAllocGenSourceFile, ["TCPIP_USE_HEAP"])

	# Add tcpip_heap_external.c file
	tcpipHeapExterncSourceFile = tcpipHeapComponent.createFileSymbol(None, None)
	tcpipHeapExterncSourceFile.setSourcePath("tcpip/src/tcpip_heap_external.c")
	tcpipHeapExterncSourceFile.setOutputName("tcpip_heap_external.c")
	tcpipHeapExterncSourceFile.setOverwrite(True)
	tcpipHeapExterncSourceFile.setDestPath("library/tcpip/src/")
	tcpipHeapExterncSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipHeapExterncSourceFile.setType("SOURCE")
	tcpipHeapExterncSourceFile.setEnabled(False)
	tcpipHeapExterncSourceFile.setDependencies(tcpipHeapExternSourceFile, ["TCPIP_STACK_USE_HEAP_CONFIG"])	

	# Add tcpip_heap_internal.c file
	tcpipHeapInterncSourceFile = tcpipHeapComponent.createFileSymbol(None, None)
	tcpipHeapInterncSourceFile.setSourcePath("tcpip/src/tcpip_heap_internal.c")
	tcpipHeapInterncSourceFile.setOutputName("tcpip_heap_internal.c")
	tcpipHeapInterncSourceFile.setOverwrite(True)
	tcpipHeapInterncSourceFile.setDestPath("library/tcpip/src/")
	tcpipHeapInterncSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipHeapInterncSourceFile.setType("SOURCE")
	tcpipHeapInterncSourceFile.setEnabled(True)
	tcpipHeapInterncSourceFile.setDependencies(tcpipHeapInternSourceFile, ["TCPIP_STACK_USE_HEAP_CONFIG"])

	# Add tcpip_heap_pool.c file
	tcpipHeapPoolcSourceFile = tcpipHeapComponent.createFileSymbol(None, None)
	tcpipHeapPoolcSourceFile.setSourcePath("tcpip/src/tcpip_heap_pool.c")
	tcpipHeapPoolcSourceFile.setOutputName("tcpip_heap_pool.c")
	tcpipHeapPoolcSourceFile.setOverwrite(True)
	tcpipHeapPoolcSourceFile.setDestPath("library/tcpip/src/")
	tcpipHeapPoolcSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipHeapPoolcSourceFile.setType("SOURCE")
	tcpipHeapPoolcSourceFile.setEnabled(False)
	tcpipHeapPoolcSourceFile.setDependencies(tcpipHeapPoolSourceFile, ["TCPIP_STACK_USE_HEAP_CONFIG"])


# make TCP/IP Stack Dynamic Allocation Trace enable
def tcpipHeapStackDynAllocateTraceVisible(tcpipDependentSymbol, tcpipIPSymbol):
	if (event["value"] == True):
		tcpipDependentSymbol.setVisible(True)
	else:
		tcpipDependentSymbol.setVisible(False)
		

# make TCP/IP Stack Number of Trace Slots visible
def tcpipHeapStackTraceSlotsNumVisible(tcpipDependentSymbol, tcpipIPSymbol):
	if (event["value"] == True):
		tcpipDependentSymbol.setVisible(True)
	else:
		tcpipDependentSymbol.setVisible(False)


def tcpipHeapMenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("Heap Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("Heap Menu Invisible.")
		symbol.setVisible(False)

		
def tcpipHeapInternMenuVisible(symbol, event):
	if (event["value"] == "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP"):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)

def tcpipHeapExternMenuInvisible(symbol, event):	
	if (event["value"] != "TCPIP_STACK_HEAP_TYPE_EXTERNAL_HEAP" ):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)
		
def tcpipHeapPoolMenuVisible(symbol, event):
	if (event["value"] == "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL" ):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)
		
def tcpipHeapAllocGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])
	
def tcpipHeapExternSourceFile(sourceFile, event):	
	if (event["value"] == "TCPIP_STACK_HEAP_TYPE_EXTERNAL_HEAP" ):
		sourceFile.setEnabled(True)
	else:
		sourceFile.setEnabled(False)
		
	
def tcpipHeapInternSourceFile(sourceFile, event):	
	if (event["value"] == "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP" ):
		sourceFile.setEnabled(True)
	else:
		sourceFile.setEnabled(False)
	
def tcpipHeapPoolSourceFile(sourceFile, event):	
	if (event["value"] == "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL" ):
		sourceFile.setEnabled(True)
	else:
		sourceFile.setEnabled(False)
		
###########################################################################################################
def tcpipHeapPoolEntryInstance(tcpipHeapSymbol, event):
	global tcpipPoolEntryInstancesNumPrev
	print("Start tcpipHeapPoolEntryInstance")
	
	if(event["id"] == "TCPIP_STACK_USE_HEAP_CONFIG" ):	
		tcpipHeapConfig = Database.getSymbolValue("tcpipHeap","TCPIP_STACK_USE_HEAP_CONFIG")
		tcpipHeapPoolIndex = int(tcpipHeapSymbol.getID().strip("TCPIP_HEAP_POOL_ENTRY_NUMBERX"))
		if (tcpipHeapConfig == "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL" ):
			if(tcpipHeapPoolIndex < tcpipPoolEntryInstancesNumPrev ):
				tcpipHeapSymbol.setVisible(True)
		else:
			tcpipHeapSymbol.setVisible(False)
	
	else:				
		tcpipHeapPoolInstanceNumberValue = event["value"]
		if(tcpipHeapPoolInstanceNumberValue > tcpipPoolEntryInstancesNumPrev ):
			tcpipHeapPoolEntry[tcpipPoolEntryInstancesNumPrev].setVisible(True)
			tcpipHeapPoolEntry[tcpipPoolEntryInstancesNumPrev].setValue(True, 1)
			print("Set TRUE"+ str(tcpipPoolEntryInstancesNumPrev))
			tcpipPoolEntryInstancesNumPrev = tcpipPoolEntryInstancesNumPrev + 1
			#Add more network configurations
		else:
			if(tcpipHeapPoolInstanceNumberValue < tcpipPoolEntryInstancesNumPrev ):
				#Reduce network configurations
				tcpipPoolEntryInstancesNumPrev = tcpipPoolEntryInstancesNumPrev - 1
				tcpipHeapPoolEntry[tcpipPoolEntryInstancesNumPrev].setVisible(False)
				tcpipHeapPoolEntry[tcpipPoolEntryInstancesNumPrev].setValue(False, 1)
				print("Set FALSE"+ str(tcpipPoolEntryInstancesNumPrev))				
			#else:
				#Do Nothing
	print("END tcpipHeapPoolEntryInstance")
	
	
def tcpipHeapPoolInstnBlkSizeMenu(symbol, event):	
	tcpipHeapPoolInstnIndex = int(symbol.getID().strip("TCPIP_HEAP_POOL_ENTRY_SIZE_IDX"))	
	tcpipHeapPoolInstnEnable = tcpipHeapPoolEntry[tcpipHeapPoolInstnIndex].getValue()	
	tcpipHeapConfig = Database.getSymbolValue("tcpipHeap","TCPIP_STACK_USE_HEAP_CONFIG")
	
	if tcpipHeapPoolInstnEnable and (tcpipHeapConfig == "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL" ):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)
		
def tcpipHeapPoolPreAllocBlkMenu(symbol, event):	
	tcpipHeapPoolInstnIndex = int(symbol.getID().strip("TCPIP_HEAP_POOL_ENTRY_BLOCKS_IDX"))	
	tcpipHeapPoolInstnEnable = tcpipHeapPoolEntry[tcpipHeapPoolInstnIndex].getValue()	
	tcpipHeapConfig = Database.getSymbolValue("tcpipHeap","TCPIP_STACK_USE_HEAP_CONFIG")	
	
	if tcpipHeapPoolInstnEnable and (tcpipHeapConfig == "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL" ):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)
		
def tcpipHeapPoolExpBlkMenu(symbol, event):
	tcpipHeapPoolInstnIndex = int(symbol.getID().strip("TCPIP_HEAP_POOL_ENTRY_EXP_BLOCKS_IDX"))	
	tcpipHeapPoolInstnEnable = tcpipHeapPoolEntry[tcpipHeapPoolInstnIndex].getValue()
	tcpipHeapConfig = Database.getSymbolValue("tcpipHeap","TCPIP_STACK_USE_HEAP_CONFIG")
	
	if tcpipHeapPoolInstnEnable and (tcpipHeapConfig == "TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL" ):
		symbol.setVisible(True)
	else:
		symbol.setVisible(False)