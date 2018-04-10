
def tcpipEthMacMenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("EthMac Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("EthMac Menu Invisible.")
		symbol.setVisible(False)
		
def tcpipEthMacMdixSwapVisible(symbol, event):
	tcpipEthMacAutoMdix = Database.getSymbolValue("tcpipEthMac","TCPIP_GMAC_ETH_OF_MDIX_AUTO")
	tcpipEthMac = Database.getSymbolValue("tcpipEthMac","TCPIP_USE_ETH_MAC")
	
	gmac = ATDF.getNode("/avr-tools-device-file/modules/module@[name=\"GMAC\"]")
	gmacID = gmac.getAttribute("id")
	print("niyas test" + str(gmacID))
	
	if(tcpipEthMac):	
		if(tcpipEthMacAutoMdix):
			symbol.setVisible(False)
		else:
			symbol.setVisible(True)
	else:
		symbol.setVisible(False)
		
def tcpipGmacGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])

def drvGmacGenHeaderFile(headerFile, event):
	headerFile.setEnabled(event["value"])

def drvExtPhyDp83640GenSourceFile(sourceFile, event):
	tcpipUseEthMac = Database.getSymbolValue("tcpipEthMac","TCPIP_USE_ETH_MAC")
	drvExtPhy = Database.getSymbolValue("tcpipEthMac","TCPIP_EMAC_PHY_TYPE")
	print("Niyas File: " + str(drvExtPhy))
	if (tcpipUseEthMac == True) and (drvExtPhy == "National_DP83640" ):
		sourceFile.setEnabled(True)
	else:
		sourceFile.setEnabled(False)


def drvExtPhyDp83848GenSourceFile(sourceFile, event):
	tcpipUseEthMac = Database.getSymbolValue("tcpipEthMac","TCPIP_USE_ETH_MAC")
	drvExtPhy = Database.getSymbolValue("tcpipEthMac","TCPIP_EMAC_PHY_TYPE")
	print("Niyas File: " + str(drvExtPhy))
	if (tcpipUseEthMac == True) and (drvExtPhy == "National_DP83848" ):
		sourceFile.setEnabled(True)
	else:
		sourceFile.setEnabled(False)
		
def drvExtPhyIp101grGenSourceFile(sourceFile, event):
	tcpipUseEthMac = Database.getSymbolValue("tcpipEthMac","TCPIP_USE_ETH_MAC")
	drvExtPhy = Database.getSymbolValue("tcpipEthMac","TCPIP_EMAC_PHY_TYPE")
	print("Niyas File: " + str(drvExtPhy))
	if (tcpipUseEthMac == True) and (drvExtPhy == "IP_IP101GR" ):
		sourceFile.setEnabled(True)
	else:
		sourceFile.setEnabled(False)
		
def drvExtPhySmscLan8700GenSourceFile(sourceFile, event):
	tcpipUseEthMac = Database.getSymbolValue("tcpipEthMac","TCPIP_USE_ETH_MAC")
	drvExtPhy = Database.getSymbolValue("tcpipEthMac","TCPIP_EMAC_PHY_TYPE")
	print("Niyas File: " + str(drvExtPhy))
	if (tcpipUseEthMac == True) and (drvExtPhy == "SMSC_LAN8700" ):
		sourceFile.setEnabled(True)
	else:
		sourceFile.setEnabled(False)
		
def drvExtPhySmscLan8720GenSourceFile(sourceFile, event):
	tcpipUseEthMac = Database.getSymbolValue("tcpipEthMac","TCPIP_USE_ETH_MAC")
	drvExtPhy = Database.getSymbolValue("tcpipEthMac","TCPIP_EMAC_PHY_TYPE")
	print("Niyas File: " + str(drvExtPhy))
	if (tcpipUseEthMac == True) and (drvExtPhy == "SMSC_LAN8720" ):
		sourceFile.setEnabled(True)
	else:
		sourceFile.setEnabled(False)
		
def drvExtPhySmscLan8740GenSourceFile(sourceFile, event):
	tcpipUseEthMac = Database.getSymbolValue("tcpipEthMac","TCPIP_USE_ETH_MAC")
	drvExtPhy = Database.getSymbolValue("tcpipEthMac","TCPIP_EMAC_PHY_TYPE")
	print("Niyas File: " + str(drvExtPhy))
	if (tcpipUseEthMac == True) and (drvExtPhy == "SMSC_LAN8740" ):
		sourceFile.setEnabled(True)
	else:
		sourceFile.setEnabled(False)
		
def drvExtPhyKsz8061GenSourceFile(sourceFile, event):
	tcpipUseEthMac = Database.getSymbolValue("tcpipEthMac","TCPIP_USE_ETH_MAC")
	drvExtPhy = Database.getSymbolValue("tcpipEthMac","TCPIP_EMAC_PHY_TYPE")
	print("Niyas File: " + str(drvExtPhy))
	if (tcpipUseEthMac == True) and (drvExtPhy == "KSZ8061" ):
		sourceFile.setEnabled(True)
	else:
		sourceFile.setEnabled(False)

def drvExtPhyKsz8041GenSourceFile(sourceFile, event):
	tcpipUseEthMac = Database.getSymbolValue("tcpipEthMac","TCPIP_USE_ETH_MAC")
	drvExtPhy = Database.getSymbolValue("tcpipEthMac","TCPIP_EMAC_PHY_TYPE")
	print("Niyas File: " + str(drvExtPhy))
	if (tcpipUseEthMac == True) and (drvExtPhy == "KSZ8041" ):
		sourceFile.setEnabled(True)
	else:
		sourceFile.setEnabled(False)
		
def drvExtPhySmscLan9303GenSourceFile(sourceFile, event):
	tcpipUseEthMac = Database.getSymbolValue("tcpipEthMac","TCPIP_USE_ETH_MAC")
	drvExtPhy = Database.getSymbolValue("tcpipEthMac","TCPIP_EMAC_PHY_TYPE")
	print("Niyas File: " + str(drvExtPhy))
	if (tcpipUseEthMac == True) and (drvExtPhy == "SMSC_LAN9303"):
		sourceFile.setEnabled(True)
	else:
		sourceFile.setEnabled(False)
		
# Use Internal Ethernet MAC Driver?	
tcpipGmacQue0 = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_QUEUE_0", tcpipEthMac)
tcpipGmacQue0.setLabel("GMAC Queue 0")
tcpipGmacQue0.setVisible(True)
tcpipGmacQue0.setDescription("GMAC Queue 0")
tcpipGmacQue0.setDefaultValue(True)
# Niyas to do
# default y if USE_PIC32CINT_ETH_MAC_NEEDED
tcpipGmacQue0.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])


# Number of Tx Descriptors To Be Created for Queue0
tcpipGmacTxDescCountQue0 = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE0", tcpipGmacQue0)
tcpipGmacTxDescCountQue0.setLabel("Number of Tx Descriptors To Be Created")
tcpipGmacTxDescCountQue0.setVisible(True)
tcpipGmacTxDescCountQue0.setDescription("Number of Tx Descriptors To Be Created for Queue0")
tcpipGmacTxDescCountQue0.setDefaultValue(10)
tcpipGmacTxDescCountQue0.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_0"])

# Number of Rx Descriptors To Be Created for Queue0
tcpipGmacRxDescCountQue0 = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE0", tcpipGmacQue0)
tcpipGmacRxDescCountQue0.setLabel("Number of Rx Descriptors To Be Created")
tcpipGmacRxDescCountQue0.setVisible(True)
tcpipGmacRxDescCountQue0.setDescription("Number of Rx Descriptors To Be Created for Queue0")
tcpipGmacRxDescCountQue0.setDefaultValue(10)
tcpipGmacRxDescCountQue0.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_0"])

# Size Of RX Buffer for Queue0. Should Be Multiple Of 16.
tcpipGmacRxBuffSizeQue0 = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_SIZE_QUE0", tcpipGmacQue0)
tcpipGmacRxBuffSizeQue0.setLabel("Size Of RX Buffer. Should Be Multiple Of 16.")
tcpipGmacRxBuffSizeQue0.setVisible(True)
tcpipGmacRxBuffSizeQue0.setDescription("Size Of RX Buffer for Queue0. Should Be Multiple Of 16.")
tcpipGmacRxBuffSizeQue0.setDefaultValue(1536)
tcpipGmacRxBuffSizeQue0.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_0"])

# Size Of TX Buffer for Queue0. Should Be Multiple Of 16.
tcpipGmacTxBuffSizeQue0 = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_TX_BUFF_SIZE_QUE0", tcpipGmacQue0)
tcpipGmacTxBuffSizeQue0.setLabel("Size Of TX Buffer. Should Be Multiple Of 16.")
tcpipGmacTxBuffSizeQue0.setVisible(True)
tcpipGmacTxBuffSizeQue0.setDescription("Size Of TX Buffer for Queue0. Should Be Multiple Of 16.")
tcpipGmacTxBuffSizeQue0.setDefaultValue(1536)
tcpipGmacTxBuffSizeQue0.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_0"])

# GMAC Queue 1	
tcpipGmacQue1 = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_QUEUE_1", tcpipEthMac)
tcpipGmacQue1.setLabel("GMAC Queue 1")
tcpipGmacQue1.setVisible(True)
tcpipGmacQue1.setDescription("GMAC Queue 1")
tcpipGmacQue1.setDefaultValue(False)
tcpipGmacQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Number of Tx Descriptors To Be Created for Queue1
tcpipGmacTxDescCountQue1 = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE1", tcpipGmacQue1)
tcpipGmacTxDescCountQue1.setLabel("Number of Tx Descriptors To Be Created")
tcpipGmacTxDescCountQue1.setVisible(False)
tcpipGmacTxDescCountQue1.setDescription("Number of Tx Descriptors To Be Created for Queue1")
tcpipGmacTxDescCountQue1.setDefaultValue(1)
tcpipGmacTxDescCountQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_1"])

# Number of Rx Descriptors To Be Created for Queue1
tcpipGmacRxDescCountQue1 = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE1", tcpipGmacQue1)
tcpipGmacRxDescCountQue1.setLabel("Number of Rx Descriptors To Be Created")
tcpipGmacRxDescCountQue1.setVisible(False)
tcpipGmacRxDescCountQue1.setDescription("Number of Rx Descriptors To Be Created for Queue1")
tcpipGmacRxDescCountQue1.setDefaultValue(1)
tcpipGmacRxDescCountQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_1"])

# Size Of RX Buffer for Queue1. Should Be Multiple Of 16.
tcpipGmacRxBuffSizeQue1 = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_SIZE_QUE1", tcpipGmacQue1)
tcpipGmacRxBuffSizeQue1.setLabel("Size Of RX Buffer. Should Be Multiple Of 16.")
tcpipGmacRxBuffSizeQue1.setVisible(False)
tcpipGmacRxBuffSizeQue1.setDescription("Size Of RX Buffer for Queue1. Should Be Multiple Of 16.")
tcpipGmacRxBuffSizeQue1.setDefaultValue(64)
tcpipGmacRxBuffSizeQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_1"])

# Size Of RX Buffer for Queue1. Should Be Multiple Of 16.
tcpipGmacTxBuffSizeQue1 = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_TX_BUFF_SIZE_QUE1", tcpipGmacQue1)
tcpipGmacTxBuffSizeQue1.setLabel("Size Of TX Buffer. Should Be Multiple Of 16.")
tcpipGmacTxBuffSizeQue1.setVisible(False)
tcpipGmacTxBuffSizeQue1.setDescription("Size Of TX Buffer for Queue1. Should Be Multiple Of 16.")
tcpipGmacTxBuffSizeQue1.setDefaultValue(64)
tcpipGmacTxBuffSizeQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_1"])

# GMAC Queue 2
tcpipGmacQue2 = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_QUEUE_2", tcpipEthMac)
tcpipGmacQue2.setLabel("GMAC Queue 2")
tcpipGmacQue2.setVisible(True)
tcpipGmacQue2.setDescription("GMAC Queue 2")
tcpipGmacQue2.setDefaultValue(False)
tcpipGmacQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Number of Tx Descriptors To Be Created for Queue2
tcpipGmacTxDescCountQue2 = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE2", tcpipGmacQue2)
tcpipGmacTxDescCountQue2.setLabel("Number of Tx Descriptors To Be Created")
tcpipGmacTxDescCountQue2.setVisible(False)
tcpipGmacTxDescCountQue2.setDescription("Number of Tx Descriptors To Be Created for Queue2")
tcpipGmacTxDescCountQue2.setDefaultValue(1)
tcpipGmacTxDescCountQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_2"])

# Number of Rx Descriptors To Be Created for Queue2
tcpipGmacRxDescCountQue2 = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE2", tcpipGmacQue2)
tcpipGmacRxDescCountQue2.setLabel("Number of Rx Descriptors To Be Created")
tcpipGmacRxDescCountQue2.setVisible(False)
tcpipGmacRxDescCountQue2.setDescription("Number of Rx Descriptors To Be Created for Queue2")
tcpipGmacRxDescCountQue2.setDefaultValue(1)
tcpipGmacRxDescCountQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_2"])

# Size Of RX Buffer for Queue2. Should Be Multiple Of 16.
tcpipGmacRxBuffSizeQue2 = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_SIZE_QUE2", tcpipGmacQue2)
tcpipGmacRxBuffSizeQue2.setLabel("Size Of RX Buffer. Should Be Multiple Of 16.")
tcpipGmacRxBuffSizeQue2.setVisible(False)
tcpipGmacRxBuffSizeQue2.setDescription("Size Of RX Buffer for Queue2. Should Be Multiple Of 16.")
tcpipGmacRxBuffSizeQue2.setDefaultValue(64)
tcpipGmacRxBuffSizeQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_2"])

# Size Of TX Buffer for Queue2. Should Be Multiple Of 16.
tcpipGmacTxBuffSizeQue2 = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_TX_BUFF_SIZE_QUE2", tcpipGmacQue2)
tcpipGmacTxBuffSizeQue2.setLabel("Size Of TX Buffer. Should Be Multiple Of 16.")
tcpipGmacTxBuffSizeQue2.setVisible(False)
tcpipGmacTxBuffSizeQue2.setDescription("Size Of TX Buffer for Queue2. Should Be Multiple Of 16.")
tcpipGmacTxBuffSizeQue2.setDefaultValue(64)
tcpipGmacTxBuffSizeQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_2"])

# GMAC Queue 3
tcpipGmacQue3 = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_QUEUE_3", tcpipEthMac)
tcpipGmacQue3.setLabel("GMAC Queue 3")
tcpipGmacQue3.setVisible(True)
tcpipGmacQue3.setDescription("GMAC Queue 3")
tcpipGmacQue3.setDefaultValue(False)
tcpipGmacQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Number of Tx Descriptors To Be Created for Queue3
tcpipGmacTxDescCountQue3 = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE3", tcpipGmacQue3)
tcpipGmacTxDescCountQue3.setLabel("Number of Tx Descriptors To Be Created")
tcpipGmacTxDescCountQue3.setVisible(False)
tcpipGmacTxDescCountQue3.setDescription("Number of Tx Descriptors To Be Created for Queue3")
tcpipGmacTxDescCountQue3.setDefaultValue(1)
tcpipGmacTxDescCountQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_3"])

# Number of Rx Descriptors To Be Created for Queue3
tcpipGmacRxDescCountQue3 = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE3", tcpipGmacQue3)
tcpipGmacRxDescCountQue3.setLabel("Number of Rx Descriptors To Be Created")
tcpipGmacRxDescCountQue3.setVisible(False)
tcpipGmacRxDescCountQue3.setDescription("Number of Rx Descriptors To Be Created for Queue3")
tcpipGmacRxDescCountQue3.setDefaultValue(1)
tcpipGmacRxDescCountQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_3"])

# Size Of RX Buffer for Queue3. Should Be Multiple Of 16.
tcpipGmacRxBuffSizeQue3 = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_SIZE_QUE3", tcpipGmacQue3)
tcpipGmacRxBuffSizeQue3.setLabel("Size Of RX Buffer. Should Be Multiple Of 16.")
tcpipGmacRxBuffSizeQue3.setVisible(False)
tcpipGmacRxBuffSizeQue3.setDescription("Size Of RX Buffer for Queue3. Should Be Multiple Of 16.")
tcpipGmacRxBuffSizeQue3.setDefaultValue(64)
tcpipGmacRxBuffSizeQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_3"])

# Size Of TX Buffer for Queue3. Should Be Multiple Of 16.
tcpipGmacTxBuffSizeQue3 = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_TX_BUFF_SIZE_QUE3", tcpipGmacQue3)
tcpipGmacTxBuffSizeQue3.setLabel("Size Of TX Buffer. Should Be Multiple Of 16.")
tcpipGmacTxBuffSizeQue3.setVisible(False)
tcpipGmacTxBuffSizeQue3.setDescription("Size Of TX Buffer for Queue3. Should Be Multiple Of 16.")
tcpipGmacTxBuffSizeQue3.setDefaultValue(64)
tcpipGmacTxBuffSizeQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_3"])

# GMAC Queue 4
tcpipGmacQue4 = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_QUEUE_4", tcpipEthMac)
tcpipGmacQue4.setLabel("GMAC Queue 4")
tcpipGmacQue4.setVisible(True)
tcpipGmacQue4.setDescription("GMAC Queue 4")
tcpipGmacQue4.setDefaultValue(False)
tcpipGmacQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Number of Tx Descriptors To Be Created for Queue4
tcpipGmacTxDescCountQue4 = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE4", tcpipGmacQue4)
tcpipGmacTxDescCountQue4.setLabel("Number of Tx Descriptors To Be Created")
tcpipGmacTxDescCountQue4.setVisible(False)
tcpipGmacTxDescCountQue4.setDescription("Number of Tx Descriptors To Be Created for Queue4")
tcpipGmacTxDescCountQue4.setDefaultValue(1)
tcpipGmacTxDescCountQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_4"])

# Number of Rx Descriptors To Be Created for Queue4
tcpipGmacRxDescCountQue4 = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE4", tcpipGmacQue4)
tcpipGmacRxDescCountQue4.setLabel("Number of Rx Descriptors To Be Created")
tcpipGmacRxDescCountQue4.setVisible(False)
tcpipGmacRxDescCountQue4.setDescription("Number of Rx Descriptors To Be Created for Queue4")
tcpipGmacRxDescCountQue4.setDefaultValue(1)
tcpipGmacRxDescCountQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_4"])

# Size Of RX Buffer for Queue4. Should Be Multiple Of 16.
tcpipGmacRxBuffSizeQue4 = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_SIZE_QUE4", tcpipGmacQue4)
tcpipGmacRxBuffSizeQue4.setLabel("Size Of RX Buffer. Should Be Multiple Of 16.")
tcpipGmacRxBuffSizeQue4.setVisible(False)
tcpipGmacRxBuffSizeQue4.setDescription("Size Of RX Buffer for Queue4. Should Be Multiple Of 16.")
tcpipGmacRxBuffSizeQue4.setDefaultValue(64)
tcpipGmacRxBuffSizeQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_4"])

# Size Of TX Buffer for Queue4. Should Be Multiple Of 16.
tcpipGmacTxBuffSizeQue4 = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_TX_BUFF_SIZE_QUE4", tcpipGmacQue4)
tcpipGmacTxBuffSizeQue4.setLabel("Size Of TX Buffer. Should Be Multiple Of 16.")
tcpipGmacTxBuffSizeQue4.setVisible(False)
tcpipGmacTxBuffSizeQue4.setDescription("Size Of TX Buffer for Queue4. Should Be Multiple Of 16.")
tcpipGmacTxBuffSizeQue4.setDefaultValue(64)
tcpipGmacTxBuffSizeQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_4"])

# GMAC Queue 5
tcpipGmacQue5 = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_QUEUE_5", tcpipEthMac)
tcpipGmacQue5.setLabel("GMAC Queue 5")
tcpipGmacQue5.setVisible(True)
tcpipGmacQue5.setDescription("GMAC Queue 5")
tcpipGmacQue5.setDefaultValue(False)
tcpipGmacQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Number of Tx Descriptors To Be Created for Queue5
tcpipGmacTxDescCountQue5 = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE5", tcpipGmacQue5)
tcpipGmacTxDescCountQue5.setLabel("Number of Tx Descriptors To Be Created")
tcpipGmacTxDescCountQue5.setVisible(False)
tcpipGmacTxDescCountQue5.setDescription("Number of Tx Descriptors To Be Created for Queue5")
tcpipGmacTxDescCountQue5.setDefaultValue(1)
tcpipGmacTxDescCountQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_5"])

# Number of Rx Descriptors To Be Created for Queue5
tcpipGmacRxDescCountQue5 = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE5", tcpipGmacQue5)
tcpipGmacRxDescCountQue5.setLabel("Number of Rx Descriptors To Be Created")
tcpipGmacRxDescCountQue5.setVisible(False)
tcpipGmacRxDescCountQue5.setDescription("Number of Rx Descriptors To Be Created for Queue5")
tcpipGmacRxDescCountQue5.setDefaultValue(1)
tcpipGmacRxDescCountQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_5"])

# Size Of RX Buffer for Queue5. Should Be Multiple Of 16.
tcpipGmacRxBuffSizeQue5 = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_SIZE_QUE5", tcpipGmacQue5)
tcpipGmacRxBuffSizeQue5.setLabel("Size Of RX Buffer. Should Be Multiple Of 16.")
tcpipGmacRxBuffSizeQue5.setVisible(False)
tcpipGmacRxBuffSizeQue5.setDescription("Size Of RX Buffer for Queue5. Should Be Multiple Of 16.")
tcpipGmacRxBuffSizeQue5.setDefaultValue(64)
tcpipGmacRxBuffSizeQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_5"])

# Size Of TX Buffer for Queue5. Should Be Multiple Of 16.
tcpipGmacTxBuffSizeQue5 = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_TX_BUFF_SIZE_QUE5", tcpipGmacQue5)
tcpipGmacTxBuffSizeQue5.setLabel("Size Of TX Buffer. Should Be Multiple Of 16.")
tcpipGmacTxBuffSizeQue5.setVisible(False)
tcpipGmacTxBuffSizeQue5.setDescription("Size Of TX Buffer for Queue5. Should Be Multiple Of 16.")
tcpipGmacTxBuffSizeQue5.setDefaultValue(64)
tcpipGmacTxBuffSizeQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_5"])

# GMAC TX descriptors Dummy Count
tcpipGmacTxDescCountDummmy = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY", tcpipEthMac)
tcpipGmacTxDescCountDummmy.setLabel("")  # Niyas to check this label value and other settings for this dummy variables
tcpipGmacTxDescCountDummmy.setVisible(False)
tcpipGmacTxDescCountDummmy.setDescription("GMAC TX descriptors Dummy Count")
tcpipGmacTxDescCountDummmy.setDefaultValue(1)
tcpipGmacTxDescCountDummmy.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# GMAC RX descriptors Dummy Count
tcpipGmacRxDescCountDummmy = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY", tcpipEthMac)
tcpipGmacRxDescCountDummmy.setLabel("")  # Niyas to check this label value and other settings for this dummy variables
tcpipGmacRxDescCountDummmy.setVisible(False)
tcpipGmacRxDescCountDummmy.setDescription("GMAC RX descriptors Dummy Count")
tcpipGmacRxDescCountDummmy.setDefaultValue(1)
tcpipGmacRxDescCountDummmy.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# GMAC Rx buffer size dummy
tcpipGmacRxBuffSizeDummmy = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_SIZE_DUMMY", tcpipEthMac)
tcpipGmacRxBuffSizeDummmy.setLabel("")  # Niyas to check this label value and other settings for this dummy variables
tcpipGmacRxBuffSizeDummmy.setVisible(False)
tcpipGmacRxBuffSizeDummmy.setDescription("GMAC Rx buffer size dummy")
tcpipGmacRxBuffSizeDummmy.setDefaultValue(64)
tcpipGmacRxBuffSizeDummmy.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# GMAC Tx buffer size dummy
tcpipGmacTxBuffSizeDummmy = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_TX_BUFF_SIZE_DUMMY", tcpipEthMac)
tcpipGmacTxBuffSizeDummmy.setLabel("")  # Niyas to check this label value and other settings for this dummy variables
tcpipGmacTxBuffSizeDummmy.setVisible(False)
tcpipGmacTxBuffSizeDummmy.setDescription("GMAC Tx buffer size dummy")
tcpipGmacTxBuffSizeDummmy.setDefaultValue(64)
tcpipGmacTxBuffSizeDummmy.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Maximum MAC Supported RX Frame Size
tcpipGmacRxFrameMax = tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_RX_MAX_FRAME", tcpipEthMac)
tcpipGmacRxFrameMax.setLabel("Maximum MAC Supported RX Frame Size")
tcpipGmacRxFrameMax.setVisible(True)
tcpipGmacRxFrameMax.setDescription("Maximum MAC Supported RX Frame Size")
tcpipGmacRxFrameMax.setDefaultValue(1536)
tcpipGmacRxFrameMax.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# MAC Maximum Number of Supported Fragments
tcpipEmacRxFragNumMax = tcpipEthMacComponent.createIntegerSymbol("TCPIP_EMAC_RX_FRAGMENTS", tcpipEthMac)
tcpipEmacRxFragNumMax.setLabel("MAC Maximum Number of Supported Fragments")
tcpipEmacRxFragNumMax.setMax(6) # Niyas to do review
tcpipEmacRxFragNumMax.setVisible(True)
tcpipEmacRxFragNumMax.setDescription("MAC Maximum Number of Supported Fragments")
tcpipEmacRxFragNumMax.setDefaultValue(1)
tcpipEmacRxFragNumMax.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Ethernet RX Filters Selection Settings
tcpipEthRxFilter = tcpipEthMacComponent.createMenuSymbol(None, tcpipEthMac)  # Niyas to check the component name and dependency
tcpipEthRxFilter.setLabel("Ethernet RX Filters Selection")
tcpipEthRxFilter.setVisible(True)
tcpipEthRxFilter.setDescription("Ethernet RX Filters Selection Settings")
# Niyas to check this dependency
tcpipEthRxFilter.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Accept Broadcast Packets
tcpipGmacEthFilterBcastAccept = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_BCAST_ACCEPT", tcpipEthRxFilter)
tcpipGmacEthFilterBcastAccept.setLabel("Accept Broadcast Packets")
tcpipGmacEthFilterBcastAccept.setVisible(True) # Niyas to check the visibility 
tcpipGmacEthFilterBcastAccept.setDescription("Accept Broadcast Packets")
tcpipGmacEthFilterBcastAccept.setDefaultValue(True)
tcpipGmacEthFilterBcastAccept.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Accept Multicast Packets
tcpipGmacEthFilterMcastAccept = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_MCAST_ACCEPT", tcpipEthRxFilter)
tcpipGmacEthFilterMcastAccept.setLabel("Accept Multicast Packets")
tcpipGmacEthFilterMcastAccept.setVisible(True) # Niyas to check the visibility 
tcpipGmacEthFilterMcastAccept.setDescription("Accept Multicast Packets")
tcpipGmacEthFilterMcastAccept.setDefaultValue(True)
tcpipGmacEthFilterMcastAccept.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Accept Unicast Packets
tcpipGmacEthFilterUcastAccept = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_UCAST_ACCEPT", tcpipEthRxFilter)
tcpipGmacEthFilterUcastAccept.setLabel("Accept Unicast Packets")
tcpipGmacEthFilterUcastAccept.setVisible(True) # Niyas to check the visibility 
tcpipGmacEthFilterUcastAccept.setDescription("Accept Unicast Packets")
tcpipGmacEthFilterUcastAccept.setDefaultValue(True)
tcpipGmacEthFilterUcastAccept.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Accept Multicast Packets matching Hash
tcpipGmacEthFilterMcastHashAccept = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_MCAST_HASH_ACCEPT", tcpipEthRxFilter)
tcpipGmacEthFilterMcastHashAccept.setLabel("Accept Multicast Packets matching Hash")
tcpipGmacEthFilterMcastHashAccept.setVisible(True) # Niyas to check the visibility 
tcpipGmacEthFilterMcastHashAccept.setDescription("Accept Multicast Packets matching Hash")
tcpipGmacEthFilterMcastHashAccept.setDefaultValue(False)
tcpipGmacEthFilterMcastHashAccept.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Accept Unicast Packets matching Hash
tcpipGmacEthFilterUcastHashAccept = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_UCAST_HASH_ACCEPT", tcpipEthRxFilter)
tcpipGmacEthFilterUcastHashAccept.setLabel("Accept Unicast Packets matching Hash")
tcpipGmacEthFilterUcastHashAccept.setVisible(True) # Niyas to check the visibility 
tcpipGmacEthFilterUcastHashAccept.setDescription("Accept Unicast Packets matching Hash")
tcpipGmacEthFilterUcastHashAccept.setDefaultValue(False)
tcpipGmacEthFilterUcastHashAccept.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Reject Packets with Wrong CRC
tcpipGmacEthFilterCrcErrReject = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_CRC_ERROR_REJECT", tcpipEthRxFilter)
tcpipGmacEthFilterCrcErrReject.setLabel("Reject Packets with Wrong CRC")
tcpipGmacEthFilterCrcErrReject.setVisible(True) # Niyas to check the visibility 
tcpipGmacEthFilterCrcErrReject.setDescription("Reject Packets with Wrong CRC")
tcpipGmacEthFilterCrcErrReject.setDefaultValue(True)
tcpipGmacEthFilterCrcErrReject.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Accept Packets with Wrong CRC
tcpipGmacEthFilterCrcErrAccept = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_CRC_ERROR_ACCEPT", tcpipEthRxFilter)
tcpipGmacEthFilterCrcErrAccept.setLabel("Accept Packets with Wrong CRC")
tcpipGmacEthFilterCrcErrAccept.setVisible(True) # Niyas to check the visibility 
tcpipGmacEthFilterCrcErrAccept.setDescription("Accept Packets with Wrong CRC")
tcpipGmacEthFilterCrcErrAccept.setDefaultValue(False)
tcpipGmacEthFilterCrcErrAccept.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Accept Packets with Maximum Frame Size(1536 bytes)
tcpipGmacEthFilteFrameAcceptMax = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_MAX_FRAME_ACCEPT", tcpipEthRxFilter)
tcpipGmacEthFilteFrameAcceptMax.setLabel("Accept Packets with Maximum Frame Size(1536 bytes)")
tcpipGmacEthFilteFrameAcceptMax.setVisible(True) # Niyas to check the visibility 
tcpipGmacEthFilteFrameAcceptMax.setDescription("Accept Packets with Maximum Frame Size(1536 bytes)")
tcpipGmacEthFilteFrameAcceptMax.setDefaultValue(False)
tcpipGmacEthFilteFrameAcceptMax.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Accept All Packets (Promiscuous Mode)
tcpipGmacEthFilterAllAccept = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_ALL_ACCEPT", tcpipEthRxFilter)
tcpipGmacEthFilterAllAccept.setLabel("Accept All Packets (Promiscuous Mode)")
tcpipGmacEthFilterAllAccept.setVisible(True) # Niyas to check the visibility 
tcpipGmacEthFilterAllAccept.setDescription("Accept All Packets (Promiscuous Mode)")
tcpipGmacEthFilterAllAccept.setDefaultValue(False)
tcpipGmacEthFilterAllAccept.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Accept Packets with Frame Error
tcpipGmacEthFilterFrameErrAccept = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_FRAME_ERROR_ACCEPT", tcpipEthRxFilter)
tcpipGmacEthFilterFrameErrAccept.setLabel("Accept Packets with Frame Error")
tcpipGmacEthFilterFrameErrAccept.setVisible(True) # Niyas to check the visibility 
tcpipGmacEthFilterFrameErrAccept.setDescription("Accept Packets with Frame Error")
tcpipGmacEthFilterFrameErrAccept.setDefaultValue(False)
tcpipGmacEthFilterFrameErrAccept.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Accept Jumbo Packets (upto 10240 bytes)
tcpipGmacEthFilterJumboFrameAccept = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_JUMBO_FRAME_ACCEPT", tcpipEthRxFilter)
tcpipGmacEthFilterJumboFrameAccept.setLabel("Accept Jumbo Packets (upto 10240 bytes)")
tcpipGmacEthFilterJumboFrameAccept.setVisible(True) # Niyas to check the visibility 
tcpipGmacEthFilterJumboFrameAccept.setDescription("Accept Jumbo Packets (upto 10240 bytes)")
tcpipGmacEthFilterJumboFrameAccept.setDefaultValue(False)
tcpipGmacEthFilterJumboFrameAccept.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Ethernet Connection Flags
tcpipEthConnFlag = tcpipEthMacComponent.createMenuSymbol(None, tcpipEthMac)  # Niyas to check the component name and dependency
tcpipEthConnFlag.setLabel("Ethernet Connection Flags")
tcpipEthConnFlag.setVisible(True)
tcpipEthConnFlag.setDescription("Ethernet Connection Flags")
# Niyas to check this dependency
tcpipEthConnFlag.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Use Auto Negotiation
tcpipGmacEthAutoNegotiate = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_AUTO_NEGOTIATION", tcpipEthConnFlag)
tcpipGmacEthAutoNegotiate.setLabel("Use Auto Negotiation")
tcpipGmacEthAutoNegotiate.setVisible(True) # Niyas to check the visibility 
tcpipGmacEthAutoNegotiate.setDescription("Use Auto Negotiation")
tcpipGmacEthAutoNegotiate.setDefaultValue(True)
tcpipGmacEthAutoNegotiate.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Use Full Duplex
tcpipGmacEthFullDuplex = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_FULL_DUPLEX", tcpipEthConnFlag)
tcpipGmacEthFullDuplex.setLabel("Use Full Duplex")
tcpipGmacEthFullDuplex.setVisible(True) # Niyas to check the visibility 
tcpipGmacEthFullDuplex.setDescription("Use Full Duplex")
tcpipGmacEthFullDuplex.setDefaultValue(True)
tcpipGmacEthFullDuplex.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Use Half Duplex
tcpipGmacEthHalfDuplex = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_HALF_DUPLEX", tcpipEthConnFlag)
tcpipGmacEthHalfDuplex.setLabel("Use Half Duplex")
tcpipGmacEthHalfDuplex.setVisible(True) # Niyas to check the visibility 
tcpipGmacEthHalfDuplex.setDescription("Use Half Duplex")
tcpipGmacEthHalfDuplex.setDefaultValue(True)
tcpipGmacEthHalfDuplex.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Use 100MBps
tcpipGmacEthUse100 = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_100", tcpipEthConnFlag)
tcpipGmacEthUse100.setLabel("Use 100MBps")
tcpipGmacEthUse100.setVisible(True) # Niyas to check the visibility 
tcpipGmacEthUse100.setDescription("Use 100MBps")
tcpipGmacEthUse100.setDefaultValue(True)
tcpipGmacEthUse100.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Use 10MBps
tcpipGmacEthUse10 = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_10", tcpipEthConnFlag)
tcpipGmacEthUse10.setLabel("Use 10MBps")
tcpipGmacEthUse10.setVisible(True) # Niyas to check the visibility 
tcpipGmacEthUse10.setDescription("Use 10MBps")
tcpipGmacEthUse10.setDefaultValue(True)
tcpipGmacEthUse10.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Allow Huge Packets
tcpipGmacEthHugePkt = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_HUGE_PKTS", tcpipEthConnFlag)
tcpipGmacEthHugePkt.setLabel("Allow Huge Packets")
tcpipGmacEthHugePkt.setVisible(True) # Niyas to check the visibility 
tcpipGmacEthHugePkt.setDescription("Allow Huge Packets")
tcpipGmacEthHugePkt.setDefaultValue(False)
tcpipGmacEthHugePkt.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Loopbacked At The MAC Level
tcpipGmacEthMacLoopBack = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_MAC_LOOPBACK", tcpipEthConnFlag)
tcpipGmacEthMacLoopBack.setLabel("Loopbacked At The MAC Level")
tcpipGmacEthMacLoopBack.setVisible(True) # Niyas to check the visibility 
tcpipGmacEthMacLoopBack.setDescription("Loopbacked At The MAC Level")
tcpipGmacEthMacLoopBack.setDefaultValue(False)
tcpipGmacEthMacLoopBack.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Loopbacked At The PHY Level
tcpipGmacEthPhyLoopBack = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_PHY_LOOPBACK", tcpipEthConnFlag)
tcpipGmacEthPhyLoopBack.setLabel("Loopbacked At The PHY Level")
tcpipGmacEthPhyLoopBack.setVisible(True) # Niyas to check the visibility 
tcpipGmacEthPhyLoopBack.setDescription("Loopbacked At The PHY Level")
tcpipGmacEthPhyLoopBack.setDefaultValue(False)
tcpipGmacEthPhyLoopBack.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Use Auto MDIX
tcpipGmacEthMdixAuto = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_MDIX_AUTO", tcpipEthConnFlag)
tcpipGmacEthMdixAuto.setLabel("Use Auto MDIX")
tcpipGmacEthMdixAuto.setVisible(True) # Niyas to check the visibility 
tcpipGmacEthMdixAuto.setDescription("Use Auto MDIX")
tcpipGmacEthMdixAuto.setDefaultValue(True)
tcpipGmacEthMdixAuto.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Use Swapped MDIX
tcpipGmacEthMdixSwap = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_MDIX_SWAP", tcpipEthConnFlag)
tcpipGmacEthMdixSwap.setLabel("Use Swapped MDIX")
tcpipGmacEthMdixSwap.setVisible(False) # Niyas to check the visibility 
tcpipGmacEthMdixSwap.setDescription("Use Swapped MDIX")
tcpipGmacEthMdixSwap.setDefaultValue(False)
tcpipGmacEthMdixSwap.setDependencies(tcpipEthMacMdixSwapVisible, ["TCPIP_USE_ETH_MAC","TCPIP_GMAC_ETH_OF_MDIX_AUTO"])

# RMII Connection
tcpipGmacEthRmii = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_RMII", tcpipEthConnFlag)
tcpipGmacEthRmii.setLabel("RMII Connection")
tcpipGmacEthRmii.setVisible(True) # Niyas to check the visibility 
tcpipGmacEthRmii.setDescription("RMII Connection")
tcpipGmacEthRmii.setDefaultValue(True)
tcpipGmacEthRmii.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# GMAC Module ID
tcpipEmacModuleId = tcpipEthMacComponent.createStringSymbol("TCPIP_EMAC_MODULE_ID", tcpipEthMac) # Niyas to use the below combo symbol instead of string
#niyas Do we need this module id?
#tcpipEmacModuleId = tcpipEthMacComponent.createComboSymbol("TCPIP_EMAC_MODULE_ID", tcpipEthMac, GMAC_MODULE_ID)
tcpipEmacModuleId.setLabel("GMAC Module ID")
tcpipEmacModuleId.setVisible(True)
tcpipEmacModuleId.setDescription("GMAC Module ID")
tcpipEmacModuleId.setDefaultValue("GMAC_ID_0")
tcpipEmacModuleId.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# GMAC Interrupt mode 
#niyas : this sysmbol need more review
tcpipGmacIntrMode = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_INTERRUPT_MODE", tcpipEthMac)
tcpipGmacIntrMode.setLabel("")
tcpipGmacIntrMode.setVisible(False) # Niyas to check the visibility and label
tcpipGmacIntrMode.setDescription("GMAC Interrupt mode")
tcpipGmacIntrMode.setDefaultValue(True)
#tcpipGmacIntrMode.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Driver GMAC Instances Number
drvGmacInstanceNum = tcpipEthMacComponent.createIntegerSymbol("DRV_GMAC_INSTANCES_NUMBER", tcpipEthMac)
drvGmacInstanceNum.setLabel("GMAC Instances Number")
drvGmacInstanceNum.setVisible(True) # Niyas to check the visibility
drvGmacInstanceNum.setDescription("Driver GMAC Instances Number")
drvGmacInstanceNum.setDefaultValue(1)
drvGmacInstanceNum.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Driver GMAC Clients Number
drvGmacClientNum = tcpipEthMacComponent.createIntegerSymbol("DRV_GMAC_CLIENTS_NUMBER", tcpipEthMac)
drvGmacClientNum.setLabel("GMAC Clients Number")
drvGmacClientNum.setVisible(True) # Niyas to check the visibility
drvGmacClientNum.setDescription("Driver GMAC Clients Number")
drvGmacClientNum.setDefaultValue(1)
drvGmacClientNum.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Driver GMAC Clients Number
drvGmacIndex = tcpipEthMacComponent.createIntegerSymbol("DRV_GMAC_INDEX", tcpipEthMac)
drvGmacIndex.setLabel("GMAC Index Number") # Niyas to check the label name repetation with the above one
drvGmacIndex.setVisible(True) # Niyas to check the visibility
drvGmacIndex.setDescription("Driver GMAC Clients Number")
drvGmacIndex.setDefaultValue(1)
drvGmacIndex.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Driver GMAC Peripheral ID
drvGmacPeripheralId = tcpipEthMacComponent.createIntegerSymbol("DRV_GMAC_PERIPHERAL_ID", tcpipEthMac)
drvGmacPeripheralId.setLabel("GMAC Peripheral ID") 
drvGmacPeripheralId.setVisible(True) # Niyas to check the visibility
drvGmacPeripheralId.setDescription("Driver GMAC Peripheral ID")
drvGmacPeripheralId.setDefaultValue(1)
drvGmacPeripheralId.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Driver GMAC Interrupt Mode
drvGmacIntrMode = tcpipEthMacComponent.createBooleanSymbol("DRV_GMAC_INTERRUPT_MODE", tcpipEthMac)
drvGmacIntrMode.setLabel("GMAC Interrupt Mode")
drvGmacIntrMode.setVisible(True) # Niyas to check the visibility 
drvGmacIntrMode.setDescription("Driver GMAC Interrupt Mode")
drvGmacIntrMode.setDefaultValue(True)
drvGmacIntrMode.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Driver GMAC Interrupt Source
drvGmacIntrSource = tcpipEthMacComponent.createStringSymbol("DRV_GMAC_INTERRUPT_SOURCE", drvGmacIntrMode)
drvGmacIntrSource.setLabel("GMAC Interrupt Source")
drvGmacIntrSource.setVisible(True) # Niyas to check the visibility 
drvGmacIntrSource.setDescription("Driver GMAC Interrupt Source")
drvGmacIntrSource.setDefaultValue("GMAC_IRQn")
drvGmacIntrSource.setDependencies(tcpipEthMacMenuVisibleSingle, ["DRV_GMAC_INTERRUPT_MODE"])

# Driver GMAC Interrupt Vector
drvGmacIntrVector = tcpipEthMacComponent.createStringSymbol("DRV_GMAC_INTERRUPT_VECTOR", drvGmacIntrMode)
drvGmacIntrVector.setLabel("GMAC Interrupt Vector")
drvGmacIntrVector.setVisible(True) # Niyas to check the visibility 
drvGmacIntrVector.setDescription("Driver GMAC Interrupt Vector")
drvGmacIntrVector.setDefaultValue("GMAC_IRQn")
drvGmacIntrVector.setDependencies(tcpipEthMacMenuVisibleSingle, ["DRV_GMAC_INTERRUPT_MODE"])

# Interrupt Priority
tcpipGmacIntrPriority = tcpipEthMacComponent.createStringSymbol("TCPIP_GMAC_INTERRUPT_PRIORITY", tcpipGmacIntrMode)  # Niyas to use the below combo symbol instead of string
#tcpipGmacIntrPriority = tcpipEthMacComponent.createComboSymbol("TCPIP_GMAC_INTERRUPT_PRIORITY", tcpipGmacIntrMode, INT_PRIORITY_LEVEL)
tcpipGmacIntrPriority.setLabel("Interrupt Priority")
tcpipGmacIntrPriority.setVisible(False)# Niyas to check the visibility 
tcpipGmacIntrPriority.setDescription("Interrupt Priority")
tcpipGmacIntrPriority.setDefaultValue("INT_PRIORITY_LEVEL1")
tcpipGmacIntrPriority.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_INTERRUPT_MODE"])

# Power State
#drvGmacPowerState = tcpipEthMacComponent.createStringSymbol("DRV_GMAC_POWER_STATE", tcpipEthMac)# Niyas to use the below combo symbol instead of string
drvGmacPowerState = tcpipEthMacComponent.createComboSymbol("DRV_GMAC_POWER_STATE", tcpipEthMac, SYS_MODULE_POWER_STATE)
drvGmacPowerState.setLabel("Power State")
drvGmacPowerState.setVisible(True) # Niyas to check the visibility 
drvGmacPowerState.setDescription("Power State")
drvGmacPowerState.setDefaultValue("SYS_MODULE_POWER_RUN_FULL")
drvGmacPowerState.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# External PHY Configuration
tcpipExPhy = tcpipEthMacComponent.createMenuSymbol(None, tcpipEthMac) 
tcpipExPhy.setLabel("External PHY Configuration")
tcpipExPhy.setVisible(True)
tcpipExPhy.setDescription("External PHY Configuration")
tcpipExPhy.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Delay for the Link Initialization in ms
tcpipGmacPhyLinkInitDelay= tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_PHY_LINK_INIT_DELAY", tcpipExPhy)
tcpipGmacPhyLinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
tcpipGmacPhyLinkInitDelay.setVisible(True)
tcpipGmacPhyLinkInitDelay.setDescription("Delay for the Link Initialization in ms")
tcpipGmacPhyLinkInitDelay.setDefaultValue(500)
tcpipGmacPhyLinkInitDelay.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# PHY Address
tcpipGmacPhyAddr= tcpipEthMacComponent.createIntegerSymbol("TCPIP_GMAC_PHY_ADDRESS", tcpipExPhy)
tcpipGmacPhyAddr.setLabel("PHY Address") 
tcpipGmacPhyAddr.setVisible(True)
tcpipGmacPhyAddr.setDescription("PHY Address")
tcpipGmacPhyAddr.setDefaultValue(1)
# Niyas to do
	# default 0 if TCPIP_EMAC_PHY_TYPE = "SMSC_LAN8700"
    # default 0 if TCPIP_EMAC_PHY_TYPE = "SMSC_LAN8720"
    # default 0 if TCPIP_EMAC_PHY_TYPE = "SMSC_LAN8740"
    # default 0 if TCPIP_EMAC_PHY_TYPE = "SMSC_LAN9303"
    # default 1 if TCPIP_EMAC_PHY_TYPE = "KSZ8061"
    # default 1 if TCPIP_EMAC_PHY_TYPE = "IP_IP101GR"
    # default 1 if TCPIP_EMAC_PHY_TYPE = "National_DP83640"
    # default 1 if TCPIP_EMAC_PHY_TYPE = "National_DP83848"
    # default 0 if TCPIP_EMAC_PHY_TYPE = "KSZ8041"
tcpipGmacPhyAddr.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# External PHY Connection Flags
tcpipExPhyConnFlag = tcpipEthMacComponent.createMenuSymbol(None, tcpipExPhy) 
tcpipExPhyConnFlag.setLabel("External PHY Connection Flags")
tcpipExPhyConnFlag.setVisible(True)
tcpipExPhyConnFlag.setDescription("External PHY Connection Flags")
tcpipExPhyConnFlag.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# RMII Data Interface
tcpipGmacPhyConfigRmii = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_PHY_CONFIG_RMII", tcpipExPhyConnFlag)
tcpipGmacPhyConfigRmii.setLabel("RMII Data Interface")
tcpipGmacPhyConfigRmii.setVisible(True)
tcpipGmacPhyConfigRmii.setDescription("RMII Data Interface")
tcpipGmacPhyConfigRmii.setDefaultValue(True)
tcpipGmacPhyConfigRmii.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Configuration Fuses Is ALT
tcpipGmacPhyConfigAlt = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_PHY_CONFIG_ALTERNATE", tcpipExPhyConnFlag)
tcpipGmacPhyConfigAlt.setLabel("Configuration Fuses Is ALT")
tcpipGmacPhyConfigAlt.setVisible(True)
tcpipGmacPhyConfigAlt.setDescription("Configuration Fuses Is ALT")
tcpipGmacPhyConfigAlt.setDefaultValue(False)
tcpipGmacPhyConfigAlt.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Use The Fuses Configuration
tcpipGmacPhyConfigAuto = tcpipEthMacComponent.createBooleanSymbol("TCPIP_GMAC_PHY_CONFIG_AUTO", tcpipExPhyConnFlag)
tcpipGmacPhyConfigAuto.setLabel("Use The Fuses Configuration")
tcpipGmacPhyConfigAuto.setVisible(True)
tcpipGmacPhyConfigAuto.setDescription("Use The Fuses Configuration")
tcpipGmacPhyConfigAuto.setDefaultValue(False)
tcpipGmacPhyConfigAuto.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# External PHY Type
tcpipEmacPhyType = tcpipEthMacComponent.createComboSymbol("TCPIP_EMAC_PHY_TYPE", tcpipExPhy, TCPIP_STACK_PHY_TYPE) # Niyas to check the range values
tcpipEmacPhyType.setLabel("External PHY Type")
tcpipEmacPhyType.setVisible(True)# Niyas to check the visibility 
tcpipEmacPhyType.setDescription("External PHY Type")
tcpipEmacPhyType.setDefaultValue("KSZ8061")
tcpipEmacPhyType.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Driver PHY Instances Number
drvEthPhyInstanceNum= tcpipEthMacComponent.createIntegerSymbol("DRV_ETHPHY_INSTANCES_NUMBER", tcpipExPhy)
drvEthPhyInstanceNum.setLabel("PHY Instances Number") 
drvEthPhyInstanceNum.setVisible(True)
drvEthPhyInstanceNum.setDescription("Driver PHY Instances Number")
drvEthPhyInstanceNum.setDefaultValue(1)
drvEthPhyInstanceNum.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Driver PHY Clients Number
drvEthPhyClientNum= tcpipEthMacComponent.createIntegerSymbol("DRV_ETHPHY_CLIENTS_NUMBER", tcpipExPhy)
drvEthPhyClientNum.setLabel("PHY Clients Number") 
drvEthPhyClientNum.setVisible(True)
drvEthPhyClientNum.setDescription("Driver PHY Clients Number")
drvEthPhyClientNum.setDefaultValue(1)
drvEthPhyClientNum.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Driver PHY Peripheral Index Number
drvEthPhyIndexNum= tcpipEthMacComponent.createIntegerSymbol("DRV_ETHPHY_INDEX", tcpipExPhy)
drvEthPhyIndexNum.setLabel("PHY Peripheral Index Number") 
drvEthPhyIndexNum.setVisible(True)
drvEthPhyIndexNum.setDescription("Driver PHY Peripheral Index Number")
drvEthPhyIndexNum.setDefaultValue(1)
drvEthPhyIndexNum.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Driver PHY Peripheral ID
drvEthPhyPeripheralId= tcpipEthMacComponent.createIntegerSymbol("DRV_ETHPHY_PERIPHERAL_ID", tcpipExPhy)
drvEthPhyPeripheralId.setLabel("PHY Peripheral ID") 
drvEthPhyPeripheralId.setVisible(True)
drvEthPhyPeripheralId.setDescription("Driver PHY Peripheral ID")
drvEthPhyPeripheralId.setDefaultValue(1)
drvEthPhyPeripheralId.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Driver PHY Negotiation Time-out - ms
drvEthPhyNegInitTimeout= tcpipEthMacComponent.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", tcpipExPhy)
drvEthPhyNegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
drvEthPhyNegInitTimeout.setVisible(True)
drvEthPhyNegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
drvEthPhyNegInitTimeout.setDefaultValue(1)
drvEthPhyNegInitTimeout.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Driver PHY Negotiation Done Time-out - ms
drvEthPhyNegDoneTimeout= tcpipEthMacComponent.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", tcpipExPhy)
drvEthPhyNegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
drvEthPhyNegDoneTimeout.setVisible(True)
drvEthPhyNegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
drvEthPhyNegDoneTimeout.setDefaultValue(2000)
drvEthPhyNegDoneTimeout.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Driver PHY Reset Clear Time-out - ms
drvEthPhyResetClearTimeout= tcpipEthMacComponent.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", tcpipExPhy)
drvEthPhyResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
drvEthPhyResetClearTimeout.setVisible(True)
drvEthPhyResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
drvEthPhyResetClearTimeout.setDefaultValue(500)
drvEthPhyResetClearTimeout.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# Use a Function to be called at PHY Reset
drvEthPhyResetCallbackEnable = tcpipEthMacComponent.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", tcpipExPhy)
drvEthPhyResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
drvEthPhyResetCallbackEnable.setVisible(True)
drvEthPhyResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
drvEthPhyResetCallbackEnable.setDefaultValue(False)
drvEthPhyResetCallbackEnable.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

# App Function
drvEthPhyResetCallback = tcpipEthMacComponent.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvEthPhyResetCallbackEnable)
drvEthPhyResetCallback.setLabel("App Function")
drvEthPhyResetCallback.setVisible(False)# Niyas to check the visibility 
drvEthPhyResetCallback.setDescription("App Function")
drvEthPhyResetCallback.setDefaultValue("AppPhyResetFunction")
drvEthPhyResetCallback.setDependencies(tcpipEthMacMenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])

#Add to system_config.h
tcpipGmacHeaderFtl = tcpipEthMacComponent.createFileSymbol(None, None)
tcpipGmacHeaderFtl.setSourcePath("library/config/tcpip_mac_pic32c.h.ftl")
tcpipGmacHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
tcpipGmacHeaderFtl.setMarkup(True)
tcpipGmacHeaderFtl.setType("STRING")
	
# file TCPIP_MAC_DRV_H "$HARMONY_VERSION_PATH/framework/driver/gmac/drv_gmac.h" to                     "$PROJECT_HEADER_FILES/framework/driver/gmac/drv_gmac.h"
# Add drv_gmac.h file to project
drvGmacHeaderFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvGmacHeaderFile.setSourcePath("driver/gmac/drv_gmac.h")
drvGmacHeaderFile.setOutputName("drv_gmac.h")
drvGmacHeaderFile.setDestPath("driver/gmac/")
drvGmacHeaderFile.setProjectPath("config/" + configName + "driver/gmac/")
drvGmacHeaderFile.setType("HEADER")
drvGmacHeaderFile.setOverwrite(True)
drvGmacHeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])

# file TCPIP_MAC_LOCAL_H "$HARMONY_VERSION_PATH/framework/driver/gmac/src/drv_gmac_local.h" to         "$PROJECT_HEADER_FILES/framework/driver/gmac/src/drv_gmac_local.h"	

# Add drv_gmac_local.h file to project
drvGmacLocalHeaderFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvGmacLocalHeaderFile.setSourcePath("driver/gmac/src/drv_gmac_local.h")
drvGmacLocalHeaderFile.setOutputName("drv_gmac_local.h")
drvGmacLocalHeaderFile.setDestPath("driver/gmac/src/")
drvGmacLocalHeaderFile.setProjectPath("config/" + configName + "driver/gmac/src/")
drvGmacLocalHeaderFile.setType("HEADER")
drvGmacLocalHeaderFile.setOverwrite(True)
drvGmacLocalHeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])

# file TCPIP_MAC_DESC_H "$HARMONY_VERSION_PATH/framework/driver/gmac/src/dynamic/_gmac_dcpt_lists.h" to "$PROJECT_HEADER_FILES/framework/driver/gmac/src/dynamic/_gmac_dcpt_lists.h"

# Add _gmac_dcpt_lists.h file to project
drvGmacDcptHeaderFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvGmacDcptHeaderFile.setSourcePath("driver/gmac/src/dynamic/_gmac_dcpt_lists.h")
drvGmacDcptHeaderFile.setOutputName("_gmac_dcpt_lists.h")
drvGmacDcptHeaderFile.setDestPath("driver/gmac/src/dynamic/")
drvGmacDcptHeaderFile.setProjectPath("config/" + configName + "driver/gmac/src/dynamic/")
drvGmacDcptHeaderFile.setType("HEADER")
drvGmacDcptHeaderFile.setOverwrite(True)
drvGmacDcptHeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])

# file TCPIP_MAC_LIB_H "$HARMONY_VERSION_PATH/framework/driver/gmac/src/dynamic/drv_gmac_lib.h" to     "$PROJECT_HEADER_FILES/framework/driver/gmac/src/dynamic/drv_gmac_lib.h"
# Add drv_gmac_lib.h file to project
drvGmacLibHeaderFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvGmacLibHeaderFile.setSourcePath("driver/gmac/src/dynamic/drv_gmac_lib.h")
drvGmacLibHeaderFile.setOutputName("drv_gmac_lib.h")
drvGmacLibHeaderFile.setDestPath("driver/gmac/src/dynamic/")
drvGmacLibHeaderFile.setProjectPath("config/" + configName + "driver/gmac/src/dynamic/")
drvGmacLibHeaderFile.setType("HEADER")
drvGmacLibHeaderFile.setOverwrite(True)
drvGmacLibHeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])

# file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
# Add drv_ethphy.h file to project
drvEthPhyHeaderFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvEthPhyHeaderFile.setSourcePath("driver/ethphy/drv_ethphy.h")
drvEthPhyHeaderFile.setOutputName("drv_ethphy.h")
drvEthPhyHeaderFile.setDestPath("driver/ethphy/")
drvEthPhyHeaderFile.setProjectPath("config/" + configName + "driver/ethphy/")
drvEthPhyHeaderFile.setType("HEADER")
drvEthPhyHeaderFile.setOverwrite(True)
drvEthPhyHeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])

# file TCPIP_ETH_PHY_LOCAL_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/drv_ethphy_local.h" to           "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/drv_ethphy_local.h"
# Add drv_ethphy_local.h file to project
drvEthPhyLocalHeaderFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvEthPhyLocalHeaderFile.setSourcePath("driver/ethphy/src/drv_ethphy_local.h")
drvEthPhyLocalHeaderFile.setOutputName("drv_ethphy_local.h")
drvEthPhyLocalHeaderFile.setDestPath("driver/ethphy/src/")
drvEthPhyLocalHeaderFile.setProjectPath("config/" + configName + "driver/ethphy/src/")
drvEthPhyLocalHeaderFile.setType("HEADER")
drvEthPhyLocalHeaderFile.setOverwrite(True)
drvEthPhyLocalHeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])


# file TCPIP_ETH_EXT_PHY_REGS_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h" to    "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h"
# Add drv_extphy_regs.h file to project
drvExtPhyRegHeaderFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvExtPhyRegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
drvExtPhyRegHeaderFile.setOutputName("drv_extphy_regs.h")
drvExtPhyRegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
drvExtPhyRegHeaderFile.setProjectPath("config/" + configName + "driver/ethphy/src/dynamic/")
drvExtPhyRegHeaderFile.setType("HEADER")
drvExtPhyRegHeaderFile.setOverwrite(True)
drvExtPhyRegHeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])    
	
# file TCPIP_MAC_PHY_DP83640_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_dp83640.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_dp83640.h"
# Add drv_extphy_dp83640.h file to project
drvExtPhyDp83640HeaderFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvExtPhyDp83640HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_dp83640.h")
drvExtPhyDp83640HeaderFile.setOutputName("drv_extphy_dp83640.h")
drvExtPhyDp83640HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
drvExtPhyDp83640HeaderFile.setProjectPath("config/" + configName + "driver/ethphy/src/dynamic/")
drvExtPhyDp83640HeaderFile.setType("HEADER")
drvExtPhyDp83640HeaderFile.setOverwrite(True)
drvExtPhyDp83640HeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])  

# file TCPIP_MAC_PHY_DP83848_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_dp83848.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_dp83848.h"
# Add drv_extphy_dp83848.h file to project
drvExtPhyDp83848HeaderFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvExtPhyDp83848HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_dp83848.h")
drvExtPhyDp83848HeaderFile.setOutputName("drv_extphy_dp83848.h")
drvExtPhyDp83848HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
drvExtPhyDp83848HeaderFile.setProjectPath("config/" + configName + "driver/ethphy/src/dynamic/")
drvExtPhyDp83848HeaderFile.setType("HEADER")
drvExtPhyDp83848HeaderFile.setOverwrite(True)
drvExtPhyDp83848HeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])  

# file TCPIP_MAC_PHY_IP101GR_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_ip101gr.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_ip101gr.h"
# Add drv_extphy_ip101gr.h file to project
drvExtPhyIp101grHeaderFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvExtPhyIp101grHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ip101gr.h")
drvExtPhyIp101grHeaderFile.setOutputName("drv_extphy_ip101gr.h")
drvExtPhyIp101grHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
drvExtPhyIp101grHeaderFile.setProjectPath("config/" + configName + "driver/ethphy/src/dynamic/")
drvExtPhyIp101grHeaderFile.setType("HEADER")
drvExtPhyIp101grHeaderFile.setOverwrite(True)
drvExtPhyIp101grHeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])  

# file TCPIP_MAC_PHY_LAN8700_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8700.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8700.h"
# Add drv_extphy_smsc8700.h file to project
drvExtPhySmsc8700HeaderFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvExtPhySmsc8700HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc8700.h")
drvExtPhySmsc8700HeaderFile.setOutputName("drv_extphy_smsc8700.h")
drvExtPhySmsc8700HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
drvExtPhySmsc8700HeaderFile.setProjectPath("config/" + configName + "driver/ethphy/src/dynamic/")
drvExtPhySmsc8700HeaderFile.setType("HEADER")
drvExtPhySmsc8700HeaderFile.setOverwrite(True)
drvExtPhySmsc8700HeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])  

# file TCPIP_MAC_PHY_LAN8720_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8720.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8720.h"
# Add drv_extphy_smsc8720.h file to project
drvExtPhySmsc8720HeaderFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvExtPhySmsc8720HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc8720.h")
drvExtPhySmsc8720HeaderFile.setOutputName("drv_extphy_smsc8720.h")
drvExtPhySmsc8720HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
drvExtPhySmsc8720HeaderFile.setProjectPath("config/" + configName + "driver/ethphy/src/dynamic/")
drvExtPhySmsc8720HeaderFile.setType("HEADER")
drvExtPhySmsc8720HeaderFile.setOverwrite(True)
drvExtPhySmsc8720HeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])  

# file TCPIP_MAC_PHY_LAN8740_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8740.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8740.h"
# Add drv_extphy_smsc8740.h file to project
drvExtPhySmsc8740HeaderFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvExtPhySmsc8740HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc8740.h")
drvExtPhySmsc8740HeaderFile.setOutputName("drv_extphy_smsc8740.h")
drvExtPhySmsc8740HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
drvExtPhySmsc8740HeaderFile.setProjectPath("config/" + configName + "driver/ethphy/src/dynamic/")
drvExtPhySmsc8740HeaderFile.setType("HEADER")
drvExtPhySmsc8740HeaderFile.setOverwrite(True)
drvExtPhySmsc8740HeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])  


# file TCPIP_MAC_PHY_LAN9303_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_smsc9303.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_smsc9303.h"
# Add drv_extphy_smsc9303.h file to project
drvExtPhySmsc9303HeaderFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvExtPhySmsc9303HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc9303.h")
drvExtPhySmsc9303HeaderFile.setOutputName("drv_extphy_smsc9303.h")
drvExtPhySmsc9303HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
drvExtPhySmsc9303HeaderFile.setProjectPath("config/" + configName + "driver/ethphy/src/dynamic/")
drvExtPhySmsc9303HeaderFile.setType("HEADER")
drvExtPhySmsc9303HeaderFile.setOverwrite(True)
drvExtPhySmsc9303HeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])

# file TCPIP_MAC_PHY_KSZ8061_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_ksz8061.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_ksz8061.h"
# Add drv_extphy_ksz8061.h file to project
drvExtPhyKsz8061HeaderFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvExtPhyKsz8061HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz8061.h")
drvExtPhyKsz8061HeaderFile.setOutputName("drv_extphy_ksz8061.h")
drvExtPhyKsz8061HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
drvExtPhyKsz8061HeaderFile.setProjectPath("config/" + configName + "driver/ethphy/src/dynamic/")
drvExtPhyKsz8061HeaderFile.setType("HEADER")
drvExtPhyKsz8061HeaderFile.setOverwrite(True)
drvExtPhyKsz8061HeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])

# file TCPIP_MAC_PHY_KSZ8041_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_ksz8041.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_ksz8041.h"
# Add drv_extphy_ksz8041.h file to project
drvExtPhyKsz8041HeaderFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvExtPhyKsz8041HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz8041.h")
drvExtPhyKsz8041HeaderFile.setOutputName("drv_extphy_ksz8041.h")
drvExtPhyKsz8041HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
drvExtPhyKsz8041HeaderFile.setProjectPath("config/" + configName + "driver/ethphy/src/dynamic/")
drvExtPhyKsz8041HeaderFile.setType("HEADER")
drvExtPhyKsz8041HeaderFile.setOverwrite(True)
drvExtPhyKsz8041HeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])


# file TCPIP_MAC_DRV_C "$HARMONY_VERSION_PATH/framework/driver/gmac/src/dynamic/drv_gmac.c" to         "$PROJECT_SOURCE_FILES/framework/driver/gmac/drv_gmac.c"
# Add drv_gmac.c file
drvGmacSourceFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvGmacSourceFile.setSourcePath("driver/gmac/src/dynamic/drv_gmac.c")
drvGmacSourceFile.setOutputName("drv_gmac.c")
drvGmacSourceFile.setOverwrite(True)
drvGmacSourceFile.setDestPath("driver/gmac/src/dynamic/")
drvGmacSourceFile.setProjectPath("config/" + configName + "driver/gmac/src/dynamic/")
drvGmacSourceFile.setType("SOURCE")
drvGmacSourceFile.setEnabled(True)
drvGmacSourceFile.setDependencies(tcpipGmacGenSourceFile, ["TCPIP_USE_ETH_MAC"])

# file TCPIP_MAC_LIB_C "$HARMONY_VERSION_PATH/framework/driver/gmac/src/dynamic/drv_gmac_lib.c" to     "$PROJECT_SOURCE_FILES/framework/driver/gmac/drv_gmac_lib.c"
# Add drv_gmac_lib.c file
drvGmacLibSourceFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvGmacLibSourceFile.setSourcePath("driver/gmac/src/dynamic/drv_gmac_lib.c")
drvGmacLibSourceFile.setOutputName("drv_gmac_lib.c")
drvGmacLibSourceFile.setOverwrite(True)
drvGmacLibSourceFile.setDestPath("driver/gmac/src/dynamic/")
drvGmacLibSourceFile.setProjectPath("config/" + configName + "driver/gmac/src/dynamic/")
drvGmacLibSourceFile.setType("SOURCE")
drvGmacLibSourceFile.setEnabled(True)
drvGmacLibSourceFile.setDependencies(tcpipGmacGenSourceFile, ["TCPIP_USE_ETH_MAC"])

# file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
# Add drv_ethphy.c file
drvEthPhySourceFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvEthPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
drvEthPhySourceFile.setOutputName("drv_ethphy.c")
drvEthPhySourceFile.setOverwrite(True)
drvEthPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
drvEthPhySourceFile.setProjectPath("config/" + configName + "driver/ethphy/src/dynamic/")
drvEthPhySourceFile.setType("SOURCE")
drvEthPhySourceFile.setEnabled(True)
drvEthPhySourceFile.setDependencies(tcpipGmacGenSourceFile, ["TCPIP_USE_ETH_MAC"])

# ifblock TCPIP_USE_ETH_MAC




# ifblock TCPIP_EMAC_PHY_TYPE = "National_DP83640"
# file TCPIP_MAC_PHY_DP83640_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_dp83640.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_dp83640.c"
# endif
# Add drv_extphy_dp83640.c file
drvExtPhyDp83640SourceFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvExtPhyDp83640SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_dp83640.c")
drvExtPhyDp83640SourceFile.setOutputName("drv_extphy_dp83640.c")
drvExtPhyDp83640SourceFile.setOverwrite(True)
drvExtPhyDp83640SourceFile.setDestPath("driver/ethphy/src/dynamic/")
drvExtPhyDp83640SourceFile.setProjectPath("config/" + configName + "driver/ethphy/src/dynamic/")
drvExtPhyDp83640SourceFile.setType("SOURCE")
drvExtPhyDp83640SourceFile.setEnabled(False)
drvExtPhyDp83640SourceFile.setDependencies(drvExtPhyDp83640GenSourceFile, ["TCPIP_USE_ETH_MAC","TCPIP_EMAC_PHY_TYPE"])


# ifblock TCPIP_EMAC_PHY_TYPE = "National_DP83848"
# file TCPIP_MAC_PHY_DP83848_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_dp83848.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_dp83848.c"
# endif
# Add drv_extphy_dp83848.c file
drvExtPhyDp83848SourceFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvExtPhyDp83848SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_dp83848.c")
drvExtPhyDp83848SourceFile.setOutputName("drv_extphy_dp83848.c")
drvExtPhyDp83848SourceFile.setOverwrite(True)
drvExtPhyDp83848SourceFile.setDestPath("driver/ethphy/src/dynamic/")
drvExtPhyDp83848SourceFile.setProjectPath("config/" + configName + "driver/ethphy/src/dynamic/")
drvExtPhyDp83848SourceFile.setType("SOURCE")
drvExtPhyDp83848SourceFile.setEnabled(False)
drvExtPhyDp83848SourceFile.setDependencies(drvExtPhyDp83848GenSourceFile, ["TCPIP_USE_ETH_MAC","TCPIP_EMAC_PHY_TYPE"])

# ifblock TCPIP_EMAC_PHY_TYPE = "IP_IP101GR"
# file TCPIP_MAC_PHY_IP101GR_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_ip101gr.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_ip101gr.c"
# endif
# Add drv_extphy_ip101gr.c file
drvExtPhyIp101grSourceFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvExtPhyIp101grSourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ip101gr.c")
drvExtPhyIp101grSourceFile.setOutputName("drv_extphy_ip101gr.c")
drvExtPhyIp101grSourceFile.setOverwrite(True)
drvExtPhyIp101grSourceFile.setDestPath("driver/ethphy/src/dynamic/")
drvExtPhyIp101grSourceFile.setProjectPath("config/" + configName + "driver/ethphy/src/dynamic/")
drvExtPhyIp101grSourceFile.setType("SOURCE")
drvExtPhyIp101grSourceFile.setEnabled(False)
drvExtPhyIp101grSourceFile.setDependencies(drvExtPhyIp101grGenSourceFile, ["TCPIP_USE_ETH_MAC","TCPIP_EMAC_PHY_TYPE"])

# ifblock TCPIP_EMAC_PHY_TYPE = "SMSC_LAN8700"
# file TCPIP_MAC_PHY_LAN8700_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8700.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_smsc8700.c"
# endif
# Add drv_extphy_smsc8700.c file
drvExtPhySmscLan8700SourceFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvExtPhySmscLan8700SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc8700.c")
drvExtPhySmscLan8700SourceFile.setOutputName("drv_extphy_smsc8700.c")
drvExtPhySmscLan8700SourceFile.setOverwrite(True)
drvExtPhySmscLan8700SourceFile.setDestPath("driver/ethphy/src/dynamic/")
drvExtPhySmscLan8700SourceFile.setProjectPath("config/" + configName + "driver/ethphy/src/dynamic/")
drvExtPhySmscLan8700SourceFile.setType("SOURCE")
drvExtPhySmscLan8700SourceFile.setEnabled(False)
drvExtPhySmscLan8700SourceFile.setDependencies(drvExtPhySmscLan8700GenSourceFile, ["TCPIP_USE_ETH_MAC","TCPIP_EMAC_PHY_TYPE"])

# ifblock TCPIP_EMAC_PHY_TYPE = "SMSC_LAN8720"
# file TCPIP_MAC_PHY_LAN8720_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8720.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_smsc8720.c"
# endif
# Add drv_extphy_smsc8720.c file
drvExtPhySmscLan8720SourceFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvExtPhySmscLan8720SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc8720.c")
drvExtPhySmscLan8720SourceFile.setOutputName("drv_extphy_smsc8720.c")
drvExtPhySmscLan8720SourceFile.setOverwrite(True)
drvExtPhySmscLan8720SourceFile.setDestPath("driver/ethphy/src/dynamic/")
drvExtPhySmscLan8720SourceFile.setProjectPath("config/" + configName + "driver/ethphy/src/dynamic/")
drvExtPhySmscLan8720SourceFile.setType("SOURCE")
drvExtPhySmscLan8720SourceFile.setEnabled(False)
drvExtPhySmscLan8720SourceFile.setDependencies(drvExtPhySmscLan8720GenSourceFile, ["TCPIP_USE_ETH_MAC","TCPIP_EMAC_PHY_TYPE"])


# ifblock TCPIP_EMAC_PHY_TYPE = "SMSC_LAN8740"
# file TCPIP_MAC_PHY_LAN8740_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8740.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_smsc8740.c"
# endif
# Add drv_extphy_smsc8740.c file
drvExtPhySmscLan8740SourceFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvExtPhySmscLan8740SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc8740.c")
drvExtPhySmscLan8740SourceFile.setOutputName("drv_extphy_smsc8740.c")
drvExtPhySmscLan8740SourceFile.setOverwrite(True)
drvExtPhySmscLan8740SourceFile.setDestPath("driver/ethphy/src/dynamic/")
drvExtPhySmscLan8740SourceFile.setProjectPath("config/" + configName + "driver/ethphy/src/dynamic/")
drvExtPhySmscLan8740SourceFile.setType("SOURCE")
drvExtPhySmscLan8740SourceFile.setEnabled(False)
drvExtPhySmscLan8740SourceFile.setDependencies(drvExtPhySmscLan8740GenSourceFile, ["TCPIP_USE_ETH_MAC","TCPIP_EMAC_PHY_TYPE"])

# ifblock TCPIP_EMAC_PHY_TYPE = "KSZ8061"
# file TCPIP_MAC_PHY_KSZ8061_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_ksz8061.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_ksz8061.c"
# endif
# Add drv_extphy_ksz8061.c file
drvExtPhyKsz8061SourceFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvExtPhyKsz8061SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz8061.c")
drvExtPhyKsz8061SourceFile.setOutputName("drv_extphy_ksz8061.c")
drvExtPhyKsz8061SourceFile.setOverwrite(True)
drvExtPhyKsz8061SourceFile.setDestPath("driver/ethphy/src/dynamic/")
drvExtPhyKsz8061SourceFile.setProjectPath("config/" + configName + "driver/ethphy/src/dynamic/")
drvExtPhyKsz8061SourceFile.setType("SOURCE")
drvExtPhyKsz8061SourceFile.setEnabled(True)
drvExtPhyKsz8061SourceFile.setDependencies(drvExtPhyKsz8061GenSourceFile, ["TCPIP_USE_ETH_MAC","TCPIP_EMAC_PHY_TYPE"])

# ifblock TCPIP_EMAC_PHY_TYPE = "KSZ8041"
# file TCPIP_MAC_PHY_KSZ8041_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_ksz8041.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_ksz8041.c"
# endif
# Add drv_extphy_ksz8041.c file
drvExtPhyKsz8041SourceFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvExtPhyKsz8041SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz8041.c")
drvExtPhyKsz8041SourceFile.setOutputName("drv_extphy_ksz8041.c")
drvExtPhyKsz8041SourceFile.setOverwrite(True)
drvExtPhyKsz8041SourceFile.setDestPath("driver/ethphy/src/dynamic/")
drvExtPhyKsz8041SourceFile.setProjectPath("config/" + configName + "driver/ethphy/src/dynamic/")
drvExtPhyKsz8041SourceFile.setType("SOURCE")
drvExtPhyKsz8041SourceFile.setEnabled(False)
drvExtPhyKsz8041SourceFile.setDependencies(drvExtPhyKsz8041GenSourceFile, ["TCPIP_USE_ETH_MAC","TCPIP_EMAC_PHY_TYPE"])

# ifblock TCPIP_EMAC_PHY_TYPE = "SMSC_LAN9303"
# file TCPIP_MAC_PHY_LAN9303_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_smsc9303.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_smsc9303.c"
# endif
# Add drv_extphy_smsc9303.c file
drvExtPhySmscLan9303SourceFile = tcpipEthMacComponent.createFileSymbol(None, None)
drvExtPhySmscLan9303SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc9303.c")
drvExtPhySmscLan9303SourceFile.setOutputName("drv_extphy_smsc9303.c")
drvExtPhySmscLan9303SourceFile.setOverwrite(True)
drvExtPhySmscLan9303SourceFile.setDestPath("driver/ethphy/src/dynamic/")
drvExtPhySmscLan9303SourceFile.setProjectPath("config/" + configName + "driver/ethphy/src/dynamic/")
drvExtPhySmscLan9303SourceFile.setType("SOURCE")
drvExtPhySmscLan9303SourceFile.setEnabled(False)
drvExtPhySmscLan9303SourceFile.setDependencies(drvExtPhySmscLan9303GenSourceFile, ["TCPIP_USE_ETH_MAC","TCPIP_EMAC_PHY_TYPE"])

# ifblock TCPIP_GMAC_INTERRUPT_MODE = y
# add "pfnGMAC_Handler GMAC_Handler"  to list SYSTEM_STARTUP_PIC32C_INTERRUPT_HANDLERS
# endif
# niyas GMAC interrupt Handler 
global peripId
global NVICVector
global NVICHandler
global NVICHandlerLock

peripId = Interrupt.getInterruptIndex("GMAC")
NVICVector = "NVIC_" + str(peripId) + "_ENABLE"
NVICHandler = "NVIC_" + str(peripId) + "_HANDLER"
NVICHandlerLock = "NVIC_" + str(peripId) + "_HANDLER_LOCK"

Database.clearSymbolValue("core", "PMC_ID_GMAC")
Database.setSymbolValue("core", "PMC_ID_GMAC", True, 2)
Database.clearSymbolValue("core", NVICVector)
Database.setSymbolValue("core", NVICVector, True, 2)
Database.clearSymbolValue("core", NVICHandler)
Database.setSymbolValue("core", NVICHandler, "GMAC_InterruptHandler", 2)
Database.clearSymbolValue("core", NVICHandlerLock)
Database.setSymbolValue("core", NVICHandlerLock, True, 2)

def NVICControl(NVIC, event):
	global NVICVector
	global NVICHandler
	Database.clearSymbolValue("core", NVICVector)
	Database.clearSymbolValue("core", NVICHandler)
	if (event["value"] == True):
		Database.setSymbolValue("core", NVICVector, True, 2)
		Database.setSymbolValue("core", NVICHandler, "GMAC_InterruptHandler", 2)
	else :
		Database.setSymbolValue("core", NVICVector, False, 2)
		Database.setSymbolValue("core", NVICHandler, "GMAC_Handler", 2)
		
# NVIC Dynamic settings
drvGmacNVICControl = tcpipEthMacComponent.createBooleanSymbol("NVIC_GMAC_ENABLE", None)
#niyas check dependency value
#drvGmacNVICControl.setDependencies(NVICControl, ["TCPIP_GMAC_INTERRUPT_MODE"])
drvGmacNVICControl.setDependencies(NVICControl, ["DRV_GMAC_INTERRUPT_MODE"])
drvGmacNVICControl.setVisible(False)



