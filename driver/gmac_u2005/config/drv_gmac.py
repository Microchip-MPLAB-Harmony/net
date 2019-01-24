def instantiateComponent(drvGmacComponent):
	global interruptVector
	global interruptHandler
	global interruptHandlerLock
	
	print("TCPIP Internal Ethernet MAC Component - SAME5x")	
	configName = Variables.get("__CONFIGURATION_NAME")		
	# Enable GMAC clock
	Database.setSymbolValue("core", "GMAC_CLOCK_ENABLE", True, 2)
	# Use Internal Ethernet MAC Driver?	
	drvGmac = drvGmacComponent.createBooleanSymbol("TCPIP_USE_ETH_MAC", None)
	drvGmac.setLabel("Use Internal Ethernet MAC Driver?")
	drvGmac.setVisible(False)
	drvGmac.setDescription("Use Internal Ethernet MAC Driver?")
	drvGmac.setDefaultValue(True)

	# Number of Tx Descriptors To Be Created 
	tcpipGmacTxDescCount = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_DESCRIPTORS_COUNT", None)
	tcpipGmacTxDescCount.setLabel("Number of Tx Descriptors To Be Created")
	tcpipGmacTxDescCount.setVisible(True)
	tcpipGmacTxDescCount.setDescription("Number of Tx Descriptors To Be Created")
	tcpipGmacTxDescCount.setDefaultValue(10)

	# Number of Rx Descriptors To Be Created
	tcpipGmacRxDescCount = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_DESCRIPTORS_COUNT", None)
	tcpipGmacRxDescCount.setLabel("Number of Rx Descriptors To Be Created")
	tcpipGmacRxDescCount.setVisible(True)
	tcpipGmacRxDescCount.setDescription("Number of Rx Descriptors To Be Created")
	tcpipGmacRxDescCount.setDefaultValue(10)

	# Size Of RX Buffer. Should Be Multiple Of 16.
	tcpipGmacRxBuffSize = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_SIZE", None)
	tcpipGmacRxBuffSize.setLabel("Size Of RX Buffer. Should Be Multiple Of 16.")
	tcpipGmacRxBuffSize.setVisible(True)
	tcpipGmacRxBuffSize.setDescription("Size Of RX Buffer. Should Be Multiple Of 16.")
	tcpipGmacRxBuffSize.setDefaultValue(1536)

	# Size Of TX Buffer. Should Be Multiple Of 16.
	tcpipGmacTxBuffSize = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_BUFF_SIZE", None)
	tcpipGmacTxBuffSize.setLabel("Size Of TX Buffer. Should Be Multiple Of 16.")
	tcpipGmacTxBuffSize.setVisible(True)
	tcpipGmacTxBuffSize.setDescription("Size Of TX Buffer. Should Be Multiple Of 16.")
	tcpipGmacTxBuffSize.setDefaultValue(1536)	
	
	# Maximum MAC Supported RX Frame Size
	tcpipGmacRxFrameMax = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_MAX_FRAME", None)
	tcpipGmacRxFrameMax.setLabel("Maximum MAC Supported RX Frame Size")
	tcpipGmacRxFrameMax.setVisible(True)
	tcpipGmacRxFrameMax.setDescription("Maximum MAC Supported RX Frame Size")
	tcpipGmacRxFrameMax.setDefaultValue(1536)
	
	# MAC Maximum Number of Supported Fragments
	tcpipEmacRxFragNumMax = drvGmacComponent.createIntegerSymbol("TCPIP_EMAC_RX_FRAGMENTS", None)
	tcpipEmacRxFragNumMax.setLabel("MAC Maximum Number of Supported Fragments")
	tcpipEmacRxFragNumMax.setMax(6)
	tcpipEmacRxFragNumMax.setVisible(True)
	tcpipEmacRxFragNumMax.setDescription("MAC Maximum Number of Supported Fragments")
	tcpipEmacRxFragNumMax.setDefaultValue(1)
	
	# Ethernet RX Filters Selection Settings
	tcpipEthRxFilter = drvGmacComponent.createMenuSymbol(None, None) 
	tcpipEthRxFilter.setLabel("Ethernet RX Filters Selection")
	tcpipEthRxFilter.setVisible(True)
	tcpipEthRxFilter.setDescription("Ethernet RX Filters Selection Settings")

	# Accept Broadcast Packets
	tcpipGmacEthFilterBcastAccept = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_BCAST_ACCEPT", tcpipEthRxFilter)
	tcpipGmacEthFilterBcastAccept.setLabel("Accept Broadcast Packets")
	tcpipGmacEthFilterBcastAccept.setVisible(True)
	tcpipGmacEthFilterBcastAccept.setDescription("Accept Broadcast Packets")
	tcpipGmacEthFilterBcastAccept.setDefaultValue(True)
	
	# Accept Multicast Packets
	tcpipGmacEthFilterMcastAccept = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_MCAST_ACCEPT", tcpipEthRxFilter)
	tcpipGmacEthFilterMcastAccept.setLabel("Accept Multicast Packets")
	tcpipGmacEthFilterMcastAccept.setVisible(True)
	tcpipGmacEthFilterMcastAccept.setDescription("Accept Multicast Packets")
	tcpipGmacEthFilterMcastAccept.setDefaultValue(True)
	
	# Accept Unicast Packets
	tcpipGmacEthFilterUcastAccept = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_UCAST_ACCEPT", tcpipEthRxFilter)
	tcpipGmacEthFilterUcastAccept.setLabel("Accept Unicast Packets")
	tcpipGmacEthFilterUcastAccept.setVisible(True)
	tcpipGmacEthFilterUcastAccept.setDescription("Accept Unicast Packets")
	tcpipGmacEthFilterUcastAccept.setDefaultValue(True)
	
	# Accept Multicast Packets matching Hash
	tcpipGmacEthFilterMcastHashAccept = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_MCAST_HASH_ACCEPT", tcpipEthRxFilter)
	tcpipGmacEthFilterMcastHashAccept.setLabel("Accept Multicast Packets matching Hash")
	tcpipGmacEthFilterMcastHashAccept.setVisible(True)
	tcpipGmacEthFilterMcastHashAccept.setDescription("Accept Multicast Packets matching Hash")
	tcpipGmacEthFilterMcastHashAccept.setDefaultValue(False)

	# Accept Unicast Packets matching Hash
	tcpipGmacEthFilterUcastHashAccept = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_UCAST_HASH_ACCEPT", tcpipEthRxFilter)
	tcpipGmacEthFilterUcastHashAccept.setLabel("Accept Unicast Packets matching Hash")
	tcpipGmacEthFilterUcastHashAccept.setVisible(True)
	tcpipGmacEthFilterUcastHashAccept.setDescription("Accept Unicast Packets matching Hash")
	tcpipGmacEthFilterUcastHashAccept.setDefaultValue(False)

	# Reject Packets with Wrong CRC
	tcpipGmacEthFilterCrcErrReject = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_CRC_ERROR_REJECT", tcpipEthRxFilter)
	tcpipGmacEthFilterCrcErrReject.setLabel("Reject Packets with Wrong CRC")
	tcpipGmacEthFilterCrcErrReject.setVisible(True)
	tcpipGmacEthFilterCrcErrReject.setDescription("Reject Packets with Wrong CRC")
	tcpipGmacEthFilterCrcErrReject.setDefaultValue(True)

	# Accept Packets with Wrong CRC
	tcpipGmacEthFilterCrcErrAccept = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_CRC_ERROR_ACCEPT", tcpipEthRxFilter)
	tcpipGmacEthFilterCrcErrAccept.setLabel("Accept Packets with Wrong CRC")
	tcpipGmacEthFilterCrcErrAccept.setVisible(True)
	tcpipGmacEthFilterCrcErrAccept.setDescription("Accept Packets with Wrong CRC")
	tcpipGmacEthFilterCrcErrAccept.setDefaultValue(False)


	# Accept Packets with Maximum Frame Size(1536 bytes)
	tcpipGmacEthFilteFrameAcceptMax = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_MAX_FRAME_ACCEPT", tcpipEthRxFilter)
	tcpipGmacEthFilteFrameAcceptMax.setLabel("Accept Packets with Maximum Frame Size(1536 bytes)")
	tcpipGmacEthFilteFrameAcceptMax.setVisible(True) 
	tcpipGmacEthFilteFrameAcceptMax.setDescription("Accept Packets with Maximum Frame Size(1536 bytes)")
	tcpipGmacEthFilteFrameAcceptMax.setDefaultValue(False)

	# Accept All Packets (Promiscuous Mode)
	tcpipGmacEthFilterAllAccept = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_ALL_ACCEPT", tcpipEthRxFilter)
	tcpipGmacEthFilterAllAccept.setLabel("Accept All Packets (Promiscuous Mode)")
	tcpipGmacEthFilterAllAccept.setVisible(True)
	tcpipGmacEthFilterAllAccept.setDescription("Accept All Packets (Promiscuous Mode)")
	tcpipGmacEthFilterAllAccept.setDefaultValue(False)

	# Accept Packets with Frame Error
	tcpipGmacEthFilterFrameErrAccept = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_FRAME_ERROR_ACCEPT", tcpipEthRxFilter)
	tcpipGmacEthFilterFrameErrAccept.setLabel("Accept Packets with Frame Error")
	tcpipGmacEthFilterFrameErrAccept.setVisible(True)
	tcpipGmacEthFilterFrameErrAccept.setDescription("Accept Packets with Frame Error")
	tcpipGmacEthFilterFrameErrAccept.setDefaultValue(False)

	# Accept Jumbo Packets (upto 10240 bytes)
	tcpipGmacEthFilterJumboFrameAccept = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_JUMBO_FRAME_ACCEPT", tcpipEthRxFilter)
	tcpipGmacEthFilterJumboFrameAccept.setLabel("Accept Jumbo Packets (upto 10240 bytes)")
	tcpipGmacEthFilterJumboFrameAccept.setVisible(True)
	tcpipGmacEthFilterJumboFrameAccept.setDescription("Accept Jumbo Packets (upto 10240 bytes)")
	tcpipGmacEthFilterJumboFrameAccept.setDefaultValue(False)


	# Ethernet Connection Flags
	tcpipEthConnFlag = drvGmacComponent.createMenuSymbol(None, None) 
	tcpipEthConnFlag.setLabel("Ethernet Connection Flags")
	tcpipEthConnFlag.setVisible(True)
	tcpipEthConnFlag.setDescription("Ethernet Connection Flags")
	
	# Use Auto Negotiation
	tcpipGmacEthAutoNegotiate = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_AUTO_NEGOTIATION", tcpipEthConnFlag)
	tcpipGmacEthAutoNegotiate.setLabel("Use Auto Negotiation")
	tcpipGmacEthAutoNegotiate.setVisible(True) 
	tcpipGmacEthAutoNegotiate.setDescription("Use Auto Negotiation")
	tcpipGmacEthAutoNegotiate.setDefaultValue(True)
	
	# Use Full Duplex
	tcpipGmacEthFullDuplex = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_FULL_DUPLEX", tcpipEthConnFlag)
	tcpipGmacEthFullDuplex.setLabel("Use Full Duplex")
	tcpipGmacEthFullDuplex.setVisible(True)
	tcpipGmacEthFullDuplex.setDescription("Use Full Duplex")
	tcpipGmacEthFullDuplex.setDefaultValue(True)
	
	# Use Half Duplex
	tcpipGmacEthHalfDuplex = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_HALF_DUPLEX", tcpipEthConnFlag)
	tcpipGmacEthHalfDuplex.setLabel("Use Half Duplex")
	tcpipGmacEthHalfDuplex.setVisible(True)
	tcpipGmacEthHalfDuplex.setDescription("Use Half Duplex")
	tcpipGmacEthHalfDuplex.setDefaultValue(True)
	
	# Use 100MBps
	tcpipGmacEthUse100 = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_100", tcpipEthConnFlag)
	tcpipGmacEthUse100.setLabel("Use 100MBps")
	tcpipGmacEthUse100.setVisible(True) 
	tcpipGmacEthUse100.setDescription("Use 100MBps")
	tcpipGmacEthUse100.setDefaultValue(True)
	
	# Use 10MBps
	tcpipGmacEthUse10 = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_10", tcpipEthConnFlag)
	tcpipGmacEthUse10.setLabel("Use 10MBps")
	tcpipGmacEthUse10.setVisible(True)
	tcpipGmacEthUse10.setDescription("Use 10MBps")
	tcpipGmacEthUse10.setDefaultValue(True)
	
	# Allow Huge Packets
	tcpipGmacEthHugePkt = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_HUGE_PKTS", tcpipEthConnFlag)
	tcpipGmacEthHugePkt.setLabel("Allow Huge Packets")
	tcpipGmacEthHugePkt.setVisible(True)
	tcpipGmacEthHugePkt.setDescription("Allow Huge Packets")
	tcpipGmacEthHugePkt.setDefaultValue(False)
	
	# Loopbacked At The MAC Level
	tcpipGmacEthMacLoopBack = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_MAC_LOOPBACK", tcpipEthConnFlag)
	tcpipGmacEthMacLoopBack.setLabel("Loopbacked At The MAC Level")
	tcpipGmacEthMacLoopBack.setVisible(True) 
	tcpipGmacEthMacLoopBack.setDescription("Loopbacked At The MAC Level")
	tcpipGmacEthMacLoopBack.setDefaultValue(False)
	
	# Loopbacked At The PHY Level
	tcpipGmacEthPhyLoopBack = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_PHY_LOOPBACK", tcpipEthConnFlag)
	tcpipGmacEthPhyLoopBack.setLabel("Loopbacked At The PHY Level")
	tcpipGmacEthPhyLoopBack.setVisible(True)
	tcpipGmacEthPhyLoopBack.setDescription("Loopbacked At The PHY Level")
	tcpipGmacEthPhyLoopBack.setDefaultValue(False)
	
	# Use Auto MDIX
	tcpipGmacEthMdixAuto = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_MDIX_AUTO", tcpipEthConnFlag)
	tcpipGmacEthMdixAuto.setLabel("Use Auto MDIX")
	tcpipGmacEthMdixAuto.setVisible(True) 
	tcpipGmacEthMdixAuto.setDescription("Use Auto MDIX")
	tcpipGmacEthMdixAuto.setDefaultValue(True)
	
	# Use Swapped MDIX
	tcpipGmacEthMdixSwap = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_MDIX_SWAP", tcpipEthConnFlag)
	tcpipGmacEthMdixSwap.setLabel("Use Swapped MDIX")
	tcpipGmacEthMdixSwap.setVisible(False)
	tcpipGmacEthMdixSwap.setDescription("Use Swapped MDIX")
	tcpipGmacEthMdixSwap.setDefaultValue(False)
	tcpipGmacEthMdixSwap.setDependencies(tcpipEthMacMdixSwapVisible, ["TCPIP_GMAC_ETH_OF_MDIX_AUTO"])

	# RMII Connection
	tcpipGmacEthRmii = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_RMII", tcpipEthConnFlag)
	tcpipGmacEthRmii.setLabel("RMII Connection")
	tcpipGmacEthRmii.setVisible(True)
	tcpipGmacEthRmii.setDescription("RMII Connection")
	tcpipGmacEthRmii.setDefaultValue(True)
	
	# GMAC Module ID
	tcpipEmacModuleId = drvGmacComponent.createStringSymbol("TCPIP_INTMAC_MODULE_ID", None)
	tcpipEmacModuleId.setLabel("GMAC Module ID")
	tcpipEmacModuleId.setVisible(True)
	tcpipEmacModuleId.setDescription("GMAC Module ID")
	tcpipEmacModuleId.setDefaultValue("GMAC_BASE_ADDRESS")

	# Driver GMAC Instances Number
	drvGmacInstanceNum = drvGmacComponent.createIntegerSymbol("DRV_GMAC_INSTANCES_NUMBER", None)
	drvGmacInstanceNum.setLabel("GMAC Instances Number")
	drvGmacInstanceNum.setVisible(True)
	drvGmacInstanceNum.setDescription("Driver GMAC Instances Number")
	drvGmacInstanceNum.setDefaultValue(1)
	
	# Driver GMAC Clients Number
	drvGmacClientNum = drvGmacComponent.createIntegerSymbol("DRV_GMAC_CLIENTS_NUMBER", None)
	drvGmacClientNum.setLabel("GMAC Clients Number")
	drvGmacClientNum.setVisible(True)
	drvGmacClientNum.setDescription("Driver GMAC Clients Number")
	drvGmacClientNum.setDefaultValue(1)
	
	# Driver GMAC Clients Number
	drvGmacIndex = drvGmacComponent.createIntegerSymbol("DRV_GMAC_INDEX", None)
	drvGmacIndex.setLabel("GMAC Index Number") 
	drvGmacIndex.setVisible(True)
	drvGmacIndex.setDescription("Driver GMAC Clients Number")
	drvGmacIndex.setDefaultValue(1)
	
	# Driver GMAC Peripheral ID
	drvGmacPeripheralId = drvGmacComponent.createIntegerSymbol("DRV_GMAC_PERIPHERAL_ID", None)
	drvGmacPeripheralId.setLabel("GMAC Peripheral ID") 
	drvGmacPeripheralId.setVisible(True)
	drvGmacPeripheralId.setDescription("Driver GMAC Peripheral ID")
	drvGmacPeripheralId.setDefaultValue(1)
	
	# Driver GMAC Interrupt Mode
	drvGmacIntrMode = drvGmacComponent.createBooleanSymbol("DRV_GMAC_INTERRUPT_MODE", None)
	drvGmacIntrMode.setLabel("GMAC Interrupt Mode")
	drvGmacIntrMode.setVisible(True)
	drvGmacIntrMode.setDescription("Driver GMAC Interrupt Mode")
	drvGmacIntrMode.setDefaultValue(True)
	
	interruptVector = "GMAC_INTERRUPT_ENABLE"
	interruptHandler = "GMAC_INTERRUPT_HANDLER"
	interruptHandlerLock = "GMAC_INTERRUPT_HANDLER_LOCK"

	Database.clearSymbolValue("core", interruptVector)
	Database.setSymbolValue("core", interruptVector, True, 2)
	Database.clearSymbolValue("core", interruptHandler)
	Database.setSymbolValue("core", interruptHandler, "GMAC_InterruptHandler", 2)
	Database.clearSymbolValue("core", interruptHandlerLock)
	Database.setSymbolValue("core", interruptHandlerLock, True, 2)

	# NVIC Dynamic settings
	drvGmacinterruptControl = drvGmacComponent.createBooleanSymbol("NVIC_GMAC_ENABLE", None)
	drvGmacinterruptControl.setDependencies(interruptControl, ["DRV_GMAC_INTERRUPT_MODE"])
	drvGmacinterruptControl.setVisible(False)
	
	# Driver GMAC Interrupt Source
	drvGmacIntrSource = drvGmacComponent.createStringSymbol("DRV_GMAC_INTERRUPT_SOURCE", drvGmacIntrMode)
	drvGmacIntrSource.setLabel("GMAC Interrupt Source")
	drvGmacIntrSource.setVisible(False)
	drvGmacIntrSource.setDescription("Driver GMAC Interrupt Source")
	drvGmacIntrSource.setDefaultValue("GMAC_IRQn")
	
	# PHY Connected to GMAC
	drvGmacPhyType = drvGmacComponent.createStringSymbol("DRV_GMAC_PHY_TYPE", None)
	drvGmacPhyType.setLabel("External PHY Device")
	drvGmacPhyType.setVisible(True)
	drvGmacPhyType.setDescription("PHY Connected to GMAC")
	drvGmacPhyType.setDefaultValue("")
	drvGmacPhyType.setReadOnly(True)
	
	#Add to system_config.h
	tcpipGmacHeaderFtl = drvGmacComponent.createFileSymbol(None, None)
	tcpipGmacHeaderFtl.setSourcePath("driver/gmac_u2005/config/drv_gmac.h.ftl")
	tcpipGmacHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipGmacHeaderFtl.setMarkup(True)
	tcpipGmacHeaderFtl.setType("STRING")
		
	# file TCPIP_MAC_DRV_H "$HARMONY_VERSION_PATH/framework/driver/gmac/drv_gmac.h" to                     "$PROJECT_HEADER_FILES/framework/driver/gmac/drv_gmac.h"
	# Add drv_gmac.h file to project
	drvGmacHeaderFile = drvGmacComponent.createFileSymbol(None, None)
	drvGmacHeaderFile.setSourcePath("driver/gmac_u2005/drv_gmac.h")
	drvGmacHeaderFile.setOutputName("drv_gmac.h")
	drvGmacHeaderFile.setDestPath("driver/gmac/")
	drvGmacHeaderFile.setProjectPath("config/" + configName + "/driver/gmac/")
	drvGmacHeaderFile.setType("HEADER")
	drvGmacHeaderFile.setOverwrite(True)

	# file TCPIP_MAC_LOCAL_H "$HARMONY_VERSION_PATH/framework/driver/gmac/src/drv_gmac_local.h" to         "$PROJECT_HEADER_FILES/framework/driver/gmac/src/drv_gmac_local.h"	

	# Add drv_gmac_local.h file to project
	drvGmacLocalHeaderFile = drvGmacComponent.createFileSymbol(None, None)
	drvGmacLocalHeaderFile.setSourcePath("driver/gmac_u2005/src/drv_gmac_local.h")
	drvGmacLocalHeaderFile.setOutputName("drv_gmac_local.h")
	drvGmacLocalHeaderFile.setDestPath("driver/gmac/src/")
	drvGmacLocalHeaderFile.setProjectPath("config/" + configName + "/driver/gmac/src/")
	drvGmacLocalHeaderFile.setType("HEADER")
	drvGmacLocalHeaderFile.setOverwrite(True)

	# file TCPIP_MAC_DESC_H "$HARMONY_VERSION_PATH/framework/driver/gmac/src/dynamic/_gmac_dcpt_lists.h" to "$PROJECT_HEADER_FILES/framework/driver/gmac/src/dynamic/_gmac_dcpt_lists.h"

	# Add _gmac_dcpt_lists.h file to project
	drvGmacDcptHeaderFile = drvGmacComponent.createFileSymbol(None, None)
	drvGmacDcptHeaderFile.setSourcePath("driver/gmac_u2005/src/dynamic/_gmac_dcpt_lists.h")
	drvGmacDcptHeaderFile.setOutputName("_gmac_dcpt_lists.h")
	drvGmacDcptHeaderFile.setDestPath("driver/gmac/src/dynamic/")
	drvGmacDcptHeaderFile.setProjectPath("config/" + configName + "/driver/gmac/src/dynamic/")
	drvGmacDcptHeaderFile.setType("HEADER")
	drvGmacDcptHeaderFile.setOverwrite(True)
	#drvGmacDcptHeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])

	# file TCPIP_MAC_LIB_H "$HARMONY_VERSION_PATH/framework/driver/gmac/src/dynamic/drv_gmac_lib.h" to     "$PROJECT_HEADER_FILES/framework/driver/gmac/src/dynamic/drv_gmac_lib.h"
	# Add drv_gmac_lib.h file to project
	drvGmacLibHeaderFile = drvGmacComponent.createFileSymbol(None, None)
	drvGmacLibHeaderFile.setSourcePath("driver/gmac_u2005/src/dynamic/drv_gmac_lib.h")
	drvGmacLibHeaderFile.setOutputName("drv_gmac_lib.h")
	drvGmacLibHeaderFile.setDestPath("driver/gmac/src/dynamic/")
	drvGmacLibHeaderFile.setProjectPath("config/" + configName + "/driver/gmac/src/dynamic/")
	drvGmacLibHeaderFile.setType("HEADER")
	drvGmacLibHeaderFile.setOverwrite(True)



	# file TCPIP_MAC_DRV_C "$HARMONY_VERSION_PATH/framework/driver/gmac/src/dynamic/drv_gmac.c" to         "$PROJECT_SOURCE_FILES/framework/driver/gmac/drv_gmac.c"
	# Add drv_gmac.c file
	drvGmacSourceFile = drvGmacComponent.createFileSymbol(None, None)
	drvGmacSourceFile.setSourcePath("driver/gmac_u2005/src/dynamic/drv_gmac.c")
	drvGmacSourceFile.setOutputName("drv_gmac.c")
	drvGmacSourceFile.setOverwrite(True)
	drvGmacSourceFile.setDestPath("driver/gmac/src/dynamic/")
	drvGmacSourceFile.setProjectPath("config/" + configName + "/driver/gmac/src/dynamic/")
	drvGmacSourceFile.setType("SOURCE")
	drvGmacSourceFile.setEnabled(True)

	# file TCPIP_MAC_LIB_C "$HARMONY_VERSION_PATH/framework/driver/gmac/src/dynamic/drv_gmac_lib.c" to     "$PROJECT_SOURCE_FILES/framework/driver/gmac/drv_gmac_lib.c"
	# Add drv_gmac_lib.c file
	
	processor = Variables.get("__PROCESSOR")
	sysIntCFileStem = "drv_gmac_lib"
	if "SAMA5" in processor:
	    sysIntCFileStem = sysIntCFileStem + "_samA5D2"

	drvGmacLibSourceFile = drvGmacComponent.createFileSymbol(None, None)
	drvGmacLibSourceFile.setSourcePath("driver/gmac_u2005/src/dynamic/" + sysIntCFileStem + ".c")
	drvGmacLibSourceFile.setOutputName("drv_gmac_lib.c")
	drvGmacLibSourceFile.setOverwrite(True)
	drvGmacLibSourceFile.setDestPath("driver/gmac/src/dynamic/")
	drvGmacLibSourceFile.setProjectPath("config/" + configName + "/driver/gmac/src/dynamic/")
	drvGmacLibSourceFile.setType("SOURCE")
	drvGmacLibSourceFile.setEnabled(True)
	
global interruptVector
global interruptHandler
global interruptHandlerLock
def interruptControl(gmacNVIC, event):
	global interruptVector
	global interruptHandler
	Database.clearSymbolValue("core", interruptVector)
	Database.clearSymbolValue("core", interruptHandler)
	Database.clearSymbolValue("core", interruptHandlerLock)
	if (event["value"] == True):
		Database.setSymbolValue("core", interruptVector, True, 2)
		Database.setSymbolValue("core", interruptHandler, "GMAC_InterruptHandler", 2)
		Database.setSymbolValue("core", interruptHandlerLock, True, 2)
	else :
		Database.setSymbolValue("core", interruptVector, False, 2)
		Database.setSymbolValue("core", interruptHandler, "GMAC_Handler", 2)
		Database.setSymbolValue("core", interruptHandlerLock, False, 2)
		
def tcpipEthMacMenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("EthMac Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("EthMac Menu Invisible.")
		symbol.setVisible(False)
		
def tcpipEthMacMdixSwapVisible(symbol, event):
	tcpipEthMacAutoMdix = Database.getSymbolValue("drvGmac","TCPIP_GMAC_ETH_OF_MDIX_AUTO")

	if (event["value"] == True):
		symbol.setVisible(False)
	else:
		symbol.setVisible(True)
	
		
def tcpipGmacGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])

def drvGmacGenHeaderFile(headerFile, event):
	headerFile.setEnabled(event["value"])

def onAttachmentConnected(source, target):
	if (source["id"] == "GMAC_PHY_Dependency"):	
		Database.setSymbolValue("drvGmac", "DRV_GMAC_PHY_TYPE", target["component"].getDisplayName(),2)
		
def onAttachmentDisconnected(source, target):
	if (source["id"] == "GMAC_PHY_Dependency"):	
		Database.clearSymbolValue("drvGmac", "DRV_GMAC_PHY_TYPE")

