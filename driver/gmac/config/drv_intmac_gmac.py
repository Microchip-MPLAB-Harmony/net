def instantiateComponent(drvSamv71GmacComponent):
	global interruptVector
	global interruptHandler
	global interruptHandlerLock
	
	print("TCPIP Ethernet MAC Component")	
	configName = Variables.get("__CONFIGURATION_NAME")		
	# Enable GMAC clock
	Database.setSymbolValue("core", "GMAC_CLOCK_ENABLE", True, 2)
	# Use Internal Ethernet MAC Driver?	
	drvSamv71Gmac = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_USE_ETH_MAC", None)
	drvSamv71Gmac.setLabel("Use Internal Ethernet MAC Driver?")
	drvSamv71Gmac.setVisible(False)
	drvSamv71Gmac.setDescription("Use Internal Ethernet MAC Driver?")
	drvSamv71Gmac.setDefaultValue(True)
		
	# Use Internal Ethernet MAC Driver?	
	tcpipGmacQue0 = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_QUEUE_0", None)
	tcpipGmacQue0.setLabel("GMAC Queue 0")
	tcpipGmacQue0.setVisible(True)
	tcpipGmacQue0.setDescription("GMAC Queue 0")
	tcpipGmacQue0.setDefaultValue(True)


	# Number of Tx Descriptors To Be Created for Queue0
	tcpipGmacTxDescCountQue0 = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE0", tcpipGmacQue0)
	tcpipGmacTxDescCountQue0.setLabel("Number of Tx Descriptors To Be Created")
	tcpipGmacTxDescCountQue0.setVisible(True)
	tcpipGmacTxDescCountQue0.setDescription("Number of Tx Descriptors To Be Created for Queue0")
	tcpipGmacTxDescCountQue0.setDefaultValue(10)
	tcpipGmacTxDescCountQue0.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_0"])

	# Number of Rx Descriptors To Be Created for Queue0
	tcpipGmacRxDescCountQue0 = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE0", tcpipGmacQue0)
	tcpipGmacRxDescCountQue0.setLabel("Number of Rx Descriptors To Be Created")
	tcpipGmacRxDescCountQue0.setVisible(True)
	tcpipGmacRxDescCountQue0.setDescription("Number of Rx Descriptors To Be Created for Queue0")
	tcpipGmacRxDescCountQue0.setDefaultValue(10)
	tcpipGmacRxDescCountQue0.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_0"])

	# Size Of RX Buffer for Queue0. Should Be Multiple Of 16.
	tcpipGmacRxBuffSizeQue0 = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_SIZE_QUE0", tcpipGmacQue0)
	tcpipGmacRxBuffSizeQue0.setLabel("Size Of RX Buffer. Should Be Multiple Of 16.")
	tcpipGmacRxBuffSizeQue0.setVisible(True)
	tcpipGmacRxBuffSizeQue0.setDescription("Size Of RX Buffer for Queue0. Should Be Multiple Of 16.")
	tcpipGmacRxBuffSizeQue0.setDefaultValue(1536)
	tcpipGmacRxBuffSizeQue0.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_0"])

	# Size Of TX Buffer for Queue0. Should Be Multiple Of 16.
	tcpipGmacTxBuffSizeQue0 = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_BUFF_SIZE_QUE0", tcpipGmacQue0)
	tcpipGmacTxBuffSizeQue0.setLabel("Size Of TX Buffer. Should Be Multiple Of 16.")
	tcpipGmacTxBuffSizeQue0.setVisible(True)
	tcpipGmacTxBuffSizeQue0.setDescription("Size Of TX Buffer for Queue0. Should Be Multiple Of 16.")
	tcpipGmacTxBuffSizeQue0.setDefaultValue(1536)
	tcpipGmacTxBuffSizeQue0.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_0"])

	# GMAC Queue 1	
	tcpipGmacQue1 = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_QUEUE_1", None)
	tcpipGmacQue1.setLabel("GMAC Queue 1")
	tcpipGmacQue1.setVisible(True)
	tcpipGmacQue1.setDescription("GMAC Queue 1")
	tcpipGmacQue1.setDefaultValue(False)
	#tcpipGmacQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

	# Number of Tx Descriptors To Be Created for Queue1
	tcpipGmacTxDescCountQue1 = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE1", tcpipGmacQue1)
	tcpipGmacTxDescCountQue1.setLabel("Number of Tx Descriptors To Be Created")
	tcpipGmacTxDescCountQue1.setVisible(False)
	tcpipGmacTxDescCountQue1.setDescription("Number of Tx Descriptors To Be Created for Queue1")
	tcpipGmacTxDescCountQue1.setDefaultValue(1)
	tcpipGmacTxDescCountQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_1"])

	# Number of Rx Descriptors To Be Created for Queue1
	tcpipGmacRxDescCountQue1 = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE1", tcpipGmacQue1)
	tcpipGmacRxDescCountQue1.setLabel("Number of Rx Descriptors To Be Created")
	tcpipGmacRxDescCountQue1.setVisible(False)
	tcpipGmacRxDescCountQue1.setDescription("Number of Rx Descriptors To Be Created for Queue1")
	tcpipGmacRxDescCountQue1.setDefaultValue(1)
	tcpipGmacRxDescCountQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_1"])

	# Size Of RX Buffer for Queue1. Should Be Multiple Of 16.
	tcpipGmacRxBuffSizeQue1 = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_SIZE_QUE1", tcpipGmacQue1)
	tcpipGmacRxBuffSizeQue1.setLabel("Size Of RX Buffer. Should Be Multiple Of 16.")
	tcpipGmacRxBuffSizeQue1.setVisible(False)
	tcpipGmacRxBuffSizeQue1.setDescription("Size Of RX Buffer for Queue1. Should Be Multiple Of 16.")
	tcpipGmacRxBuffSizeQue1.setDefaultValue(64)
	tcpipGmacRxBuffSizeQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_1"])

	# Size Of RX Buffer for Queue1. Should Be Multiple Of 16.
	tcpipGmacTxBuffSizeQue1 = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_BUFF_SIZE_QUE1", tcpipGmacQue1)
	tcpipGmacTxBuffSizeQue1.setLabel("Size Of TX Buffer. Should Be Multiple Of 16.")
	tcpipGmacTxBuffSizeQue1.setVisible(False)
	tcpipGmacTxBuffSizeQue1.setDescription("Size Of TX Buffer for Queue1. Should Be Multiple Of 16.")
	tcpipGmacTxBuffSizeQue1.setDefaultValue(64)
	tcpipGmacTxBuffSizeQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_1"])

	# GMAC Queue 2
	tcpipGmacQue2 = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_QUEUE_2", None)
	tcpipGmacQue2.setLabel("GMAC Queue 2")
	tcpipGmacQue2.setVisible(True)
	tcpipGmacQue2.setDescription("GMAC Queue 2")
	tcpipGmacQue2.setDefaultValue(False)
	#tcpipGmacQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

	# Number of Tx Descriptors To Be Created for Queue2
	tcpipGmacTxDescCountQue2 = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE2", tcpipGmacQue2)
	tcpipGmacTxDescCountQue2.setLabel("Number of Tx Descriptors To Be Created")
	tcpipGmacTxDescCountQue2.setVisible(False)
	tcpipGmacTxDescCountQue2.setDescription("Number of Tx Descriptors To Be Created for Queue2")
	tcpipGmacTxDescCountQue2.setDefaultValue(1)
	tcpipGmacTxDescCountQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_2"])

	# Number of Rx Descriptors To Be Created for Queue2
	tcpipGmacRxDescCountQue2 = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE2", tcpipGmacQue2)
	tcpipGmacRxDescCountQue2.setLabel("Number of Rx Descriptors To Be Created")
	tcpipGmacRxDescCountQue2.setVisible(False)
	tcpipGmacRxDescCountQue2.setDescription("Number of Rx Descriptors To Be Created for Queue2")
	tcpipGmacRxDescCountQue2.setDefaultValue(1)
	tcpipGmacRxDescCountQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_2"])

	# Size Of RX Buffer for Queue2. Should Be Multiple Of 16.
	tcpipGmacRxBuffSizeQue2 = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_SIZE_QUE2", tcpipGmacQue2)
	tcpipGmacRxBuffSizeQue2.setLabel("Size Of RX Buffer. Should Be Multiple Of 16.")
	tcpipGmacRxBuffSizeQue2.setVisible(False)
	tcpipGmacRxBuffSizeQue2.setDescription("Size Of RX Buffer for Queue2. Should Be Multiple Of 16.")
	tcpipGmacRxBuffSizeQue2.setDefaultValue(64)
	tcpipGmacRxBuffSizeQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_2"])

	# Size Of TX Buffer for Queue2. Should Be Multiple Of 16.
	tcpipGmacTxBuffSizeQue2 = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_BUFF_SIZE_QUE2", tcpipGmacQue2)
	tcpipGmacTxBuffSizeQue2.setLabel("Size Of TX Buffer. Should Be Multiple Of 16.")
	tcpipGmacTxBuffSizeQue2.setVisible(False)
	tcpipGmacTxBuffSizeQue2.setDescription("Size Of TX Buffer for Queue2. Should Be Multiple Of 16.")
	tcpipGmacTxBuffSizeQue2.setDefaultValue(64)
	tcpipGmacTxBuffSizeQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_2"])

	# GMAC Queue 3
	tcpipGmacQue3 = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_QUEUE_3", None)
	tcpipGmacQue3.setLabel("GMAC Queue 3")
	tcpipGmacQue3.setVisible(True)
	tcpipGmacQue3.setDescription("GMAC Queue 3")
	tcpipGmacQue3.setDefaultValue(False)
	#tcpipGmacQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

	# Number of Tx Descriptors To Be Created for Queue3
	tcpipGmacTxDescCountQue3 = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE3", tcpipGmacQue3)
	tcpipGmacTxDescCountQue3.setLabel("Number of Tx Descriptors To Be Created")
	tcpipGmacTxDescCountQue3.setVisible(False)
	tcpipGmacTxDescCountQue3.setDescription("Number of Tx Descriptors To Be Created for Queue3")
	tcpipGmacTxDescCountQue3.setDefaultValue(1)
	tcpipGmacTxDescCountQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_3"])

	# Number of Rx Descriptors To Be Created for Queue3
	tcpipGmacRxDescCountQue3 = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE3", tcpipGmacQue3)
	tcpipGmacRxDescCountQue3.setLabel("Number of Rx Descriptors To Be Created")
	tcpipGmacRxDescCountQue3.setVisible(False)
	tcpipGmacRxDescCountQue3.setDescription("Number of Rx Descriptors To Be Created for Queue3")
	tcpipGmacRxDescCountQue3.setDefaultValue(1)
	tcpipGmacRxDescCountQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_3"])

	# Size Of RX Buffer for Queue3. Should Be Multiple Of 16.
	tcpipGmacRxBuffSizeQue3 = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_SIZE_QUE3", tcpipGmacQue3)
	tcpipGmacRxBuffSizeQue3.setLabel("Size Of RX Buffer. Should Be Multiple Of 16.")
	tcpipGmacRxBuffSizeQue3.setVisible(False)
	tcpipGmacRxBuffSizeQue3.setDescription("Size Of RX Buffer for Queue3. Should Be Multiple Of 16.")
	tcpipGmacRxBuffSizeQue3.setDefaultValue(64)
	tcpipGmacRxBuffSizeQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_3"])

	# Size Of TX Buffer for Queue3. Should Be Multiple Of 16.
	tcpipGmacTxBuffSizeQue3 = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_BUFF_SIZE_QUE3", tcpipGmacQue3)
	tcpipGmacTxBuffSizeQue3.setLabel("Size Of TX Buffer. Should Be Multiple Of 16.")
	tcpipGmacTxBuffSizeQue3.setVisible(False)
	tcpipGmacTxBuffSizeQue3.setDescription("Size Of TX Buffer for Queue3. Should Be Multiple Of 16.")
	tcpipGmacTxBuffSizeQue3.setDefaultValue(64)
	tcpipGmacTxBuffSizeQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_3"])

	# GMAC Queue 4
	tcpipGmacQue4 = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_QUEUE_4", None)
	tcpipGmacQue4.setLabel("GMAC Queue 4")
	tcpipGmacQue4.setVisible(True)
	tcpipGmacQue4.setDescription("GMAC Queue 4")
	tcpipGmacQue4.setDefaultValue(False)
	#tcpipGmacQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

	# Number of Tx Descriptors To Be Created for Queue4
	tcpipGmacTxDescCountQue4 = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE4", tcpipGmacQue4)
	tcpipGmacTxDescCountQue4.setLabel("Number of Tx Descriptors To Be Created")
	tcpipGmacTxDescCountQue4.setVisible(False)
	tcpipGmacTxDescCountQue4.setDescription("Number of Tx Descriptors To Be Created for Queue4")
	tcpipGmacTxDescCountQue4.setDefaultValue(1)
	tcpipGmacTxDescCountQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_4"])

	# Number of Rx Descriptors To Be Created for Queue4
	tcpipGmacRxDescCountQue4 = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE4", tcpipGmacQue4)
	tcpipGmacRxDescCountQue4.setLabel("Number of Rx Descriptors To Be Created")
	tcpipGmacRxDescCountQue4.setVisible(False)
	tcpipGmacRxDescCountQue4.setDescription("Number of Rx Descriptors To Be Created for Queue4")
	tcpipGmacRxDescCountQue4.setDefaultValue(1)
	tcpipGmacRxDescCountQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_4"])

	# Size Of RX Buffer for Queue4. Should Be Multiple Of 16.
	tcpipGmacRxBuffSizeQue4 = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_SIZE_QUE4", tcpipGmacQue4)
	tcpipGmacRxBuffSizeQue4.setLabel("Size Of RX Buffer. Should Be Multiple Of 16.")
	tcpipGmacRxBuffSizeQue4.setVisible(False)
	tcpipGmacRxBuffSizeQue4.setDescription("Size Of RX Buffer for Queue4. Should Be Multiple Of 16.")
	tcpipGmacRxBuffSizeQue4.setDefaultValue(64)
	tcpipGmacRxBuffSizeQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_4"])

	# Size Of TX Buffer for Queue4. Should Be Multiple Of 16.
	tcpipGmacTxBuffSizeQue4 = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_BUFF_SIZE_QUE4", tcpipGmacQue4)
	tcpipGmacTxBuffSizeQue4.setLabel("Size Of TX Buffer. Should Be Multiple Of 16.")
	tcpipGmacTxBuffSizeQue4.setVisible(False)
	tcpipGmacTxBuffSizeQue4.setDescription("Size Of TX Buffer for Queue4. Should Be Multiple Of 16.")
	tcpipGmacTxBuffSizeQue4.setDefaultValue(64)
	tcpipGmacTxBuffSizeQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_4"])

	# GMAC Queue 5
	tcpipGmacQue5 = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_QUEUE_5", None)
	tcpipGmacQue5.setLabel("GMAC Queue 5")
	tcpipGmacQue5.setVisible(True)
	tcpipGmacQue5.setDescription("GMAC Queue 5")
	tcpipGmacQue5.setDefaultValue(False)
	#tcpipGmacQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

	# Number of Tx Descriptors To Be Created for Queue5
	tcpipGmacTxDescCountQue5 = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE5", tcpipGmacQue5)
	tcpipGmacTxDescCountQue5.setLabel("Number of Tx Descriptors To Be Created")
	tcpipGmacTxDescCountQue5.setVisible(False)
	tcpipGmacTxDescCountQue5.setDescription("Number of Tx Descriptors To Be Created for Queue5")
	tcpipGmacTxDescCountQue5.setDefaultValue(1)
	tcpipGmacTxDescCountQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_5"])

	# Number of Rx Descriptors To Be Created for Queue5
	tcpipGmacRxDescCountQue5 = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE5", tcpipGmacQue5)
	tcpipGmacRxDescCountQue5.setLabel("Number of Rx Descriptors To Be Created")
	tcpipGmacRxDescCountQue5.setVisible(False)
	tcpipGmacRxDescCountQue5.setDescription("Number of Rx Descriptors To Be Created for Queue5")
	tcpipGmacRxDescCountQue5.setDefaultValue(1)
	tcpipGmacRxDescCountQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_5"])

	# Size Of RX Buffer for Queue5. Should Be Multiple Of 16.
	tcpipGmacRxBuffSizeQue5 = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_SIZE_QUE5", tcpipGmacQue5)
	tcpipGmacRxBuffSizeQue5.setLabel("Size Of RX Buffer. Should Be Multiple Of 16.")
	tcpipGmacRxBuffSizeQue5.setVisible(False)
	tcpipGmacRxBuffSizeQue5.setDescription("Size Of RX Buffer for Queue5. Should Be Multiple Of 16.")
	tcpipGmacRxBuffSizeQue5.setDefaultValue(64)
	tcpipGmacRxBuffSizeQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_5"])

	# Size Of TX Buffer for Queue5. Should Be Multiple Of 16.
	tcpipGmacTxBuffSizeQue5 = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_BUFF_SIZE_QUE5", tcpipGmacQue5)
	tcpipGmacTxBuffSizeQue5.setLabel("Size Of TX Buffer. Should Be Multiple Of 16.")
	tcpipGmacTxBuffSizeQue5.setVisible(False)
	tcpipGmacTxBuffSizeQue5.setDescription("Size Of TX Buffer for Queue5. Should Be Multiple Of 16.")
	tcpipGmacTxBuffSizeQue5.setDefaultValue(64)
	tcpipGmacTxBuffSizeQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_5"])

	# GMAC TX descriptors Dummy Count
	tcpipGmacTxDescCountDummmy = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY", None)
	tcpipGmacTxDescCountDummmy.setLabel("") 
	tcpipGmacTxDescCountDummmy.setVisible(False)
	tcpipGmacTxDescCountDummmy.setDescription("GMAC TX descriptors Dummy Count")
	tcpipGmacTxDescCountDummmy.setDefaultValue(1)

	# GMAC RX descriptors Dummy Count
	tcpipGmacRxDescCountDummmy = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY", None)
	tcpipGmacRxDescCountDummmy.setLabel("GMAC RX descriptors Dummy Count")  
	tcpipGmacRxDescCountDummmy.setVisible(False)
	tcpipGmacRxDescCountDummmy.setDescription("GMAC RX descriptors Dummy Count")
	tcpipGmacRxDescCountDummmy.setDefaultValue(1)

	# GMAC Rx buffer size dummy
	tcpipGmacRxBuffSizeDummmy = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_SIZE_DUMMY", None)
	tcpipGmacRxBuffSizeDummmy.setLabel("GMAC Rx buffer size dummy") 
	tcpipGmacRxBuffSizeDummmy.setVisible(False)
	tcpipGmacRxBuffSizeDummmy.setDescription("GMAC Rx buffer size dummy")
	tcpipGmacRxBuffSizeDummmy.setDefaultValue(64)
	
	# GMAC Tx buffer size dummy
	tcpipGmacTxBuffSizeDummmy = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_BUFF_SIZE_DUMMY", None)
	tcpipGmacTxBuffSizeDummmy.setLabel("GMAC Tx buffer size dummy")  
	tcpipGmacTxBuffSizeDummmy.setVisible(False)
	tcpipGmacTxBuffSizeDummmy.setDescription("GMAC Tx buffer size dummy")
	tcpipGmacTxBuffSizeDummmy.setDefaultValue(64)
	
	# Maximum MAC Supported RX Frame Size
	tcpipGmacRxFrameMax = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_MAX_FRAME", None)
	tcpipGmacRxFrameMax.setLabel("Maximum MAC Supported RX Frame Size")
	tcpipGmacRxFrameMax.setVisible(True)
	tcpipGmacRxFrameMax.setDescription("Maximum MAC Supported RX Frame Size")
	tcpipGmacRxFrameMax.setDefaultValue(1536)
	
	# MAC Maximum Number of Supported Fragments
	tcpipEmacRxFragNumMax = drvSamv71GmacComponent.createIntegerSymbol("TCPIP_EMAC_RX_FRAGMENTS", None)
	tcpipEmacRxFragNumMax.setLabel("MAC Maximum Number of Supported Fragments")
	tcpipEmacRxFragNumMax.setMax(6)
	tcpipEmacRxFragNumMax.setVisible(True)
	tcpipEmacRxFragNumMax.setDescription("MAC Maximum Number of Supported Fragments")
	tcpipEmacRxFragNumMax.setDefaultValue(1)
	
	# Ethernet RX Filters Selection Settings
	tcpipEthRxFilter = drvSamv71GmacComponent.createMenuSymbol(None, None) 
	tcpipEthRxFilter.setLabel("Ethernet RX Filters Selection")
	tcpipEthRxFilter.setVisible(True)
	tcpipEthRxFilter.setDescription("Ethernet RX Filters Selection Settings")

	# Accept Broadcast Packets
	tcpipGmacEthFilterBcastAccept = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_BCAST_ACCEPT", tcpipEthRxFilter)
	tcpipGmacEthFilterBcastAccept.setLabel("Accept Broadcast Packets")
	tcpipGmacEthFilterBcastAccept.setVisible(True)
	tcpipGmacEthFilterBcastAccept.setDescription("Accept Broadcast Packets")
	tcpipGmacEthFilterBcastAccept.setDefaultValue(True)
	
	# Accept Multicast Packets
	tcpipGmacEthFilterMcastAccept = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_MCAST_ACCEPT", tcpipEthRxFilter)
	tcpipGmacEthFilterMcastAccept.setLabel("Accept Multicast Packets")
	tcpipGmacEthFilterMcastAccept.setVisible(True)
	tcpipGmacEthFilterMcastAccept.setDescription("Accept Multicast Packets")
	tcpipGmacEthFilterMcastAccept.setDefaultValue(True)
	
	# Accept Unicast Packets
	tcpipGmacEthFilterUcastAccept = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_UCAST_ACCEPT", tcpipEthRxFilter)
	tcpipGmacEthFilterUcastAccept.setLabel("Accept Unicast Packets")
	tcpipGmacEthFilterUcastAccept.setVisible(True)
	tcpipGmacEthFilterUcastAccept.setDescription("Accept Unicast Packets")
	tcpipGmacEthFilterUcastAccept.setDefaultValue(True)
	
	# Accept Multicast Packets matching Hash
	tcpipGmacEthFilterMcastHashAccept = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_MCAST_HASH_ACCEPT", tcpipEthRxFilter)
	tcpipGmacEthFilterMcastHashAccept.setLabel("Accept Multicast Packets matching Hash")
	tcpipGmacEthFilterMcastHashAccept.setVisible(True)
	tcpipGmacEthFilterMcastHashAccept.setDescription("Accept Multicast Packets matching Hash")
	tcpipGmacEthFilterMcastHashAccept.setDefaultValue(False)

	# Accept Unicast Packets matching Hash
	tcpipGmacEthFilterUcastHashAccept = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_UCAST_HASH_ACCEPT", tcpipEthRxFilter)
	tcpipGmacEthFilterUcastHashAccept.setLabel("Accept Unicast Packets matching Hash")
	tcpipGmacEthFilterUcastHashAccept.setVisible(True)
	tcpipGmacEthFilterUcastHashAccept.setDescription("Accept Unicast Packets matching Hash")
	tcpipGmacEthFilterUcastHashAccept.setDefaultValue(False)

	# Reject Packets with Wrong CRC
	tcpipGmacEthFilterCrcErrReject = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_CRC_ERROR_REJECT", tcpipEthRxFilter)
	tcpipGmacEthFilterCrcErrReject.setLabel("Reject Packets with Wrong CRC")
	tcpipGmacEthFilterCrcErrReject.setVisible(True)
	tcpipGmacEthFilterCrcErrReject.setDescription("Reject Packets with Wrong CRC")
	tcpipGmacEthFilterCrcErrReject.setDefaultValue(True)

	# Accept Packets with Wrong CRC
	tcpipGmacEthFilterCrcErrAccept = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_CRC_ERROR_ACCEPT", tcpipEthRxFilter)
	tcpipGmacEthFilterCrcErrAccept.setLabel("Accept Packets with Wrong CRC")
	tcpipGmacEthFilterCrcErrAccept.setVisible(True)
	tcpipGmacEthFilterCrcErrAccept.setDescription("Accept Packets with Wrong CRC")
	tcpipGmacEthFilterCrcErrAccept.setDefaultValue(False)


	# Accept Packets with Maximum Frame Size(1536 bytes)
	tcpipGmacEthFilteFrameAcceptMax = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_MAX_FRAME_ACCEPT", tcpipEthRxFilter)
	tcpipGmacEthFilteFrameAcceptMax.setLabel("Accept Packets with Maximum Frame Size(1536 bytes)")
	tcpipGmacEthFilteFrameAcceptMax.setVisible(True) 
	tcpipGmacEthFilteFrameAcceptMax.setDescription("Accept Packets with Maximum Frame Size(1536 bytes)")
	tcpipGmacEthFilteFrameAcceptMax.setDefaultValue(False)

	# Accept All Packets (Promiscuous Mode)
	tcpipGmacEthFilterAllAccept = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_ALL_ACCEPT", tcpipEthRxFilter)
	tcpipGmacEthFilterAllAccept.setLabel("Accept All Packets (Promiscuous Mode)")
	tcpipGmacEthFilterAllAccept.setVisible(True)
	tcpipGmacEthFilterAllAccept.setDescription("Accept All Packets (Promiscuous Mode)")
	tcpipGmacEthFilterAllAccept.setDefaultValue(False)

	# Accept Packets with Frame Error
	tcpipGmacEthFilterFrameErrAccept = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_FRAME_ERROR_ACCEPT", tcpipEthRxFilter)
	tcpipGmacEthFilterFrameErrAccept.setLabel("Accept Packets with Frame Error")
	tcpipGmacEthFilterFrameErrAccept.setVisible(True)
	tcpipGmacEthFilterFrameErrAccept.setDescription("Accept Packets with Frame Error")
	tcpipGmacEthFilterFrameErrAccept.setDefaultValue(False)

	# Accept Jumbo Packets (upto 10240 bytes)
	tcpipGmacEthFilterJumboFrameAccept = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_JUMBO_FRAME_ACCEPT", tcpipEthRxFilter)
	tcpipGmacEthFilterJumboFrameAccept.setLabel("Accept Jumbo Packets (upto 10240 bytes)")
	tcpipGmacEthFilterJumboFrameAccept.setVisible(True)
	tcpipGmacEthFilterJumboFrameAccept.setDescription("Accept Jumbo Packets (upto 10240 bytes)")
	tcpipGmacEthFilterJumboFrameAccept.setDefaultValue(False)


	# Ethernet Connection Flags
	tcpipEthConnFlag = drvSamv71GmacComponent.createMenuSymbol(None, None) 
	tcpipEthConnFlag.setLabel("Ethernet Connection Flags")
	tcpipEthConnFlag.setVisible(True)
	tcpipEthConnFlag.setDescription("Ethernet Connection Flags")
	
	# Use Auto Negotiation
	tcpipGmacEthAutoNegotiate = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_AUTO_NEGOTIATION", tcpipEthConnFlag)
	tcpipGmacEthAutoNegotiate.setLabel("Use Auto Negotiation")
	tcpipGmacEthAutoNegotiate.setVisible(True) 
	tcpipGmacEthAutoNegotiate.setDescription("Use Auto Negotiation")
	tcpipGmacEthAutoNegotiate.setDefaultValue(True)
	
	# Use Full Duplex
	tcpipGmacEthFullDuplex = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_FULL_DUPLEX", tcpipEthConnFlag)
	tcpipGmacEthFullDuplex.setLabel("Use Full Duplex")
	tcpipGmacEthFullDuplex.setVisible(True)
	tcpipGmacEthFullDuplex.setDescription("Use Full Duplex")
	tcpipGmacEthFullDuplex.setDefaultValue(True)
	
	# Use Half Duplex
	tcpipGmacEthHalfDuplex = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_HALF_DUPLEX", tcpipEthConnFlag)
	tcpipGmacEthHalfDuplex.setLabel("Use Half Duplex")
	tcpipGmacEthHalfDuplex.setVisible(True)
	tcpipGmacEthHalfDuplex.setDescription("Use Half Duplex")
	tcpipGmacEthHalfDuplex.setDefaultValue(True)
	
	# Use 100MBps
	tcpipGmacEthUse100 = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_100", tcpipEthConnFlag)
	tcpipGmacEthUse100.setLabel("Use 100MBps")
	tcpipGmacEthUse100.setVisible(True) 
	tcpipGmacEthUse100.setDescription("Use 100MBps")
	tcpipGmacEthUse100.setDefaultValue(True)
	
	# Use 10MBps
	tcpipGmacEthUse10 = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_10", tcpipEthConnFlag)
	tcpipGmacEthUse10.setLabel("Use 10MBps")
	tcpipGmacEthUse10.setVisible(True)
	tcpipGmacEthUse10.setDescription("Use 10MBps")
	tcpipGmacEthUse10.setDefaultValue(True)
	
	# Allow Huge Packets
	tcpipGmacEthHugePkt = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_HUGE_PKTS", tcpipEthConnFlag)
	tcpipGmacEthHugePkt.setLabel("Allow Huge Packets")
	tcpipGmacEthHugePkt.setVisible(True)
	tcpipGmacEthHugePkt.setDescription("Allow Huge Packets")
	tcpipGmacEthHugePkt.setDefaultValue(False)
	
	# Loopbacked At The MAC Level
	tcpipGmacEthMacLoopBack = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_MAC_LOOPBACK", tcpipEthConnFlag)
	tcpipGmacEthMacLoopBack.setLabel("Loopbacked At The MAC Level")
	tcpipGmacEthMacLoopBack.setVisible(True) 
	tcpipGmacEthMacLoopBack.setDescription("Loopbacked At The MAC Level")
	tcpipGmacEthMacLoopBack.setDefaultValue(False)
	
	# Loopbacked At The PHY Level
	tcpipGmacEthPhyLoopBack = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_PHY_LOOPBACK", tcpipEthConnFlag)
	tcpipGmacEthPhyLoopBack.setLabel("Loopbacked At The PHY Level")
	tcpipGmacEthPhyLoopBack.setVisible(True)
	tcpipGmacEthPhyLoopBack.setDescription("Loopbacked At The PHY Level")
	tcpipGmacEthPhyLoopBack.setDefaultValue(False)
	
	# Use Auto MDIX
	tcpipGmacEthMdixAuto = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_MDIX_AUTO", tcpipEthConnFlag)
	tcpipGmacEthMdixAuto.setLabel("Use Auto MDIX")
	tcpipGmacEthMdixAuto.setVisible(True) 
	tcpipGmacEthMdixAuto.setDescription("Use Auto MDIX")
	tcpipGmacEthMdixAuto.setDefaultValue(True)
	
	# Use Swapped MDIX
	tcpipGmacEthMdixSwap = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_MDIX_SWAP", tcpipEthConnFlag)
	tcpipGmacEthMdixSwap.setLabel("Use Swapped MDIX")
	tcpipGmacEthMdixSwap.setVisible(False)
	tcpipGmacEthMdixSwap.setDescription("Use Swapped MDIX")
	tcpipGmacEthMdixSwap.setDefaultValue(False)
	tcpipGmacEthMdixSwap.setDependencies(tcpipEthMacMdixSwapVisible, ["TCPIP_GMAC_ETH_OF_MDIX_AUTO"])

	# RMII Connection
	tcpipGmacEthRmii = drvSamv71GmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_RMII", tcpipEthConnFlag)
	tcpipGmacEthRmii.setLabel("RMII Connection")
	tcpipGmacEthRmii.setVisible(True)
	tcpipGmacEthRmii.setDescription("RMII Connection")
	tcpipGmacEthRmii.setDefaultValue(True)
	
	# GMAC Module ID
	tcpipEmacModuleId = drvSamv71GmacComponent.createStringSymbol("TCPIP_INTMAC_MODULE_ID", None)
	tcpipEmacModuleId.setLabel("GMAC Module ID")
	tcpipEmacModuleId.setVisible(True)
	tcpipEmacModuleId.setDescription("GMAC Module ID")
	tcpipEmacModuleId.setDefaultValue("GMAC_BASE_ADDRESS")

	# Driver GMAC Instances Number
	drvGmacInstanceNum = drvSamv71GmacComponent.createIntegerSymbol("DRV_GMAC_INSTANCES_NUMBER", None)
	drvGmacInstanceNum.setLabel("GMAC Instances Number")
	drvGmacInstanceNum.setVisible(True)
	drvGmacInstanceNum.setDescription("Driver GMAC Instances Number")
	drvGmacInstanceNum.setDefaultValue(1)
	
	# Driver GMAC Clients Number
	drvGmacClientNum = drvSamv71GmacComponent.createIntegerSymbol("DRV_GMAC_CLIENTS_NUMBER", None)
	drvGmacClientNum.setLabel("GMAC Clients Number")
	drvGmacClientNum.setVisible(True)
	drvGmacClientNum.setDescription("Driver GMAC Clients Number")
	drvGmacClientNum.setDefaultValue(1)
	
	# Driver GMAC Clients Number
	drvGmacIndex = drvSamv71GmacComponent.createIntegerSymbol("DRV_GMAC_INDEX", None)
	drvGmacIndex.setLabel("GMAC Index Number") 
	drvGmacIndex.setVisible(True)
	drvGmacIndex.setDescription("Driver GMAC Clients Number")
	drvGmacIndex.setDefaultValue(1)
	
	# Driver GMAC Peripheral ID
	drvGmacPeripheralId = drvSamv71GmacComponent.createIntegerSymbol("DRV_GMAC_PERIPHERAL_ID", None)
	drvGmacPeripheralId.setLabel("GMAC Peripheral ID") 
	drvGmacPeripheralId.setVisible(True)
	drvGmacPeripheralId.setDescription("Driver GMAC Peripheral ID")
	drvGmacPeripheralId.setDefaultValue(1)
	
	# Driver GMAC Interrupt Mode
	drvGmacIntrMode = drvSamv71GmacComponent.createBooleanSymbol("DRV_GMAC_INTERRUPT_MODE", None)
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
	drvGmacinterruptControl = drvSamv71GmacComponent.createBooleanSymbol("NVIC_GMAC_ENABLE", None)
	drvGmacinterruptControl.setDependencies(interruptControl, ["DRV_GMAC_INTERRUPT_MODE"])
	drvGmacinterruptControl.setVisible(False)
	
	# Driver GMAC Interrupt Source
	drvGmacIntrSource = drvSamv71GmacComponent.createStringSymbol("DRV_GMAC_INTERRUPT_SOURCE", drvGmacIntrMode)
	drvGmacIntrSource.setLabel("GMAC Interrupt Source")
	drvGmacIntrSource.setVisible(False)
	drvGmacIntrSource.setDescription("Driver GMAC Interrupt Source")
	drvGmacIntrSource.setDefaultValue("GMAC_IRQn")
	
	# PHY Connected to GMAC
	drvGmacPhyType = drvSamv71GmacComponent.createStringSymbol("DRV_GMAC_PHY_TYPE", None)
	drvGmacPhyType.setLabel("External PHY Device")
	drvGmacPhyType.setVisible(True)
	drvGmacPhyType.setDescription("PHY Connected to GMAC")
	drvGmacPhyType.setDefaultValue("")
	drvGmacPhyType.setReadOnly(True)
	
	#Add to system_config.h
	tcpipGmacHeaderFtl = drvSamv71GmacComponent.createFileSymbol(None, None)
	tcpipGmacHeaderFtl.setSourcePath("driver/gmac/config/drv_intmac_gmac.h.ftl")
	tcpipGmacHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipGmacHeaderFtl.setMarkup(True)
	tcpipGmacHeaderFtl.setType("STRING")
		
	# file TCPIP_MAC_DRV_H "$HARMONY_VERSION_PATH/framework/driver/gmac/drv_gmac.h" to                     "$PROJECT_HEADER_FILES/framework/driver/gmac/drv_gmac.h"
	# Add drv_gmac.h file to project
	drvGmacHeaderFile = drvSamv71GmacComponent.createFileSymbol(None, None)
	drvGmacHeaderFile.setSourcePath("driver/gmac/drv_gmac.h")
	drvGmacHeaderFile.setOutputName("drv_gmac.h")
	drvGmacHeaderFile.setDestPath("driver/gmac/")
	drvGmacHeaderFile.setProjectPath("config/" + configName + "/driver/gmac/")
	drvGmacHeaderFile.setType("HEADER")
	drvGmacHeaderFile.setOverwrite(True)

	# file TCPIP_MAC_LOCAL_H "$HARMONY_VERSION_PATH/framework/driver/gmac/src/drv_gmac_local.h" to         "$PROJECT_HEADER_FILES/framework/driver/gmac/src/drv_gmac_local.h"	

	# Add drv_gmac_local.h file to project
	drvGmacLocalHeaderFile = drvSamv71GmacComponent.createFileSymbol(None, None)
	drvGmacLocalHeaderFile.setSourcePath("driver/gmac/src/drv_gmac_local.h")
	drvGmacLocalHeaderFile.setOutputName("drv_gmac_local.h")
	drvGmacLocalHeaderFile.setDestPath("driver/gmac/src/")
	drvGmacLocalHeaderFile.setProjectPath("config/" + configName + "/driver/gmac/src/")
	drvGmacLocalHeaderFile.setType("HEADER")
	drvGmacLocalHeaderFile.setOverwrite(True)

	# file TCPIP_MAC_DESC_H "$HARMONY_VERSION_PATH/framework/driver/gmac/src/dynamic/_gmac_dcpt_lists.h" to "$PROJECT_HEADER_FILES/framework/driver/gmac/src/dynamic/_gmac_dcpt_lists.h"

	# Add _gmac_dcpt_lists.h file to project
	drvGmacDcptHeaderFile = drvSamv71GmacComponent.createFileSymbol(None, None)
	drvGmacDcptHeaderFile.setSourcePath("driver/gmac/src/dynamic/_gmac_dcpt_lists.h")
	drvGmacDcptHeaderFile.setOutputName("_gmac_dcpt_lists.h")
	drvGmacDcptHeaderFile.setDestPath("driver/gmac/src/dynamic/")
	drvGmacDcptHeaderFile.setProjectPath("config/" + configName + "/driver/gmac/src/dynamic/")
	drvGmacDcptHeaderFile.setType("HEADER")
	drvGmacDcptHeaderFile.setOverwrite(True)
	#drvGmacDcptHeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])

	# file TCPIP_MAC_LIB_H "$HARMONY_VERSION_PATH/framework/driver/gmac/src/dynamic/drv_gmac_lib.h" to     "$PROJECT_HEADER_FILES/framework/driver/gmac/src/dynamic/drv_gmac_lib.h"
	# Add drv_gmac_lib.h file to project
	drvGmacLibHeaderFile = drvSamv71GmacComponent.createFileSymbol(None, None)
	drvGmacLibHeaderFile.setSourcePath("driver/gmac/src/dynamic/drv_gmac_lib.h")
	drvGmacLibHeaderFile.setOutputName("drv_gmac_lib.h")
	drvGmacLibHeaderFile.setDestPath("driver/gmac/src/dynamic/")
	drvGmacLibHeaderFile.setProjectPath("config/" + configName + "/driver/gmac/src/dynamic/")
	drvGmacLibHeaderFile.setType("HEADER")
	drvGmacLibHeaderFile.setOverwrite(True)



	# file TCPIP_MAC_DRV_C "$HARMONY_VERSION_PATH/framework/driver/gmac/src/dynamic/drv_gmac.c" to         "$PROJECT_SOURCE_FILES/framework/driver/gmac/drv_gmac.c"
	# Add drv_gmac.c file
	drvGmacSourceFile = drvSamv71GmacComponent.createFileSymbol(None, None)
	drvGmacSourceFile.setSourcePath("driver/gmac/src/dynamic/drv_gmac.c")
	drvGmacSourceFile.setOutputName("drv_gmac.c")
	drvGmacSourceFile.setOverwrite(True)
	drvGmacSourceFile.setDestPath("driver/gmac/src/dynamic/")
	drvGmacSourceFile.setProjectPath("config/" + configName + "/driver/gmac/src/dynamic/")
	drvGmacSourceFile.setType("SOURCE")
	drvGmacSourceFile.setEnabled(True)

	# file TCPIP_MAC_LIB_C "$HARMONY_VERSION_PATH/framework/driver/gmac/src/dynamic/drv_gmac_lib.c" to     "$PROJECT_SOURCE_FILES/framework/driver/gmac/drv_gmac_lib.c"
	# Add drv_gmac_lib.c file
	drvGmacLibSourceFile = drvSamv71GmacComponent.createFileSymbol(None, None)
	drvGmacLibSourceFile.setSourcePath("driver/gmac/src/dynamic/drv_gmac_lib.c")
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
	tcpipEthMacAutoMdix = Database.getSymbolValue("drvSamv71Gmac","TCPIP_GMAC_ETH_OF_MDIX_AUTO")

	if (event["value"] == True):
		symbol.setVisible(False)
	else:
		symbol.setVisible(True)
	
		
def tcpipGmacGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])

def drvGmacGenHeaderFile(headerFile, event):
	headerFile.setEnabled(event["value"])


def onGenericDependencySatisfied(dependencyID, satisfierID):
	print("satisfied: " + dependencyID + ", " + satisfierID)
	satisfierComponent = Database.getComponentByID(satisfierID)
	Database.clearSymbolValue("drvSamv71Gmac", "DRV_GMAC_PHY_TYPE")
	Database.setSymbolValue("drvSamv71Gmac", "DRV_GMAC_PHY_TYPE", satisfierComponent.getDisplayName(),2)

def onGenericDependencyUnsatisfied(dependencyID, satisfierID):
	print("unsatisfied: " + dependencyID + ", " + satisfierID)
	Database.clearSymbolValue("drvSamv71Gmac", "DRV_GMAC_PHY_TYPE")
