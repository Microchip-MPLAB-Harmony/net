__TCPIP_DIR = "D:/06_Repo/05_H3/h3/net"
def instantiateComponent(drvExtPhyLan8700Component):
	print("KSZ8061 PHY Driver Component")
	configName = Variables.get("__CONFIGURATION_NAME")

	# Delay for the Link Initialization in ms
	drvExtPhyLan8700LinkInitDelay= drvExtPhyLan8700Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", None)
	drvExtPhyLan8700LinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
	drvExtPhyLan8700LinkInitDelay.setVisible(True)
	drvExtPhyLan8700LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
	drvExtPhyLan8700LinkInitDelay.setDefaultValue(500)
	#drvExtPhyLan8700LinkInitDelay.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

	# PHY Address
	drvExtPhyLan8700Addr= drvExtPhyLan8700Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
	drvExtPhyLan8700Addr.setLabel("PHY Address") 
	drvExtPhyLan8700Addr.setVisible(True)
	drvExtPhyLan8700Addr.setDescription("PHY Address")
	drvExtPhyLan8700Addr.setDefaultValue(0)

	# External PHY Connection Flags
	drvExtPhyLan8700ConnFlag = drvExtPhyLan8700Component.createMenuSymbol(None, None) 
	drvExtPhyLan8700ConnFlag.setLabel("External PHY Connection Flags")
	drvExtPhyLan8700ConnFlag.setVisible(True)
	drvExtPhyLan8700ConnFlag.setDescription("External PHY Connection Flags")
	
	# RMII Data Interface
	drvExtPhyLan8700ConfigRmii = drvExtPhyLan8700Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyLan8700ConnFlag)
	drvExtPhyLan8700ConfigRmii.setLabel("RMII Data Interface")
	drvExtPhyLan8700ConfigRmii.setVisible(True)
	drvExtPhyLan8700ConfigRmii.setDescription("RMII Data Interface")
	drvExtPhyLan8700ConfigRmii.setDefaultValue(True)
	
	# Configuration Fuses Is ALT
	drvExtPhyLan8700ConfigAlt = drvExtPhyLan8700Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_ALTERNATE", drvExtPhyLan8700ConnFlag)
	drvExtPhyLan8700ConfigAlt.setLabel("Configuration Fuses Is ALT")
	drvExtPhyLan8700ConfigAlt.setVisible(True)
	drvExtPhyLan8700ConfigAlt.setDescription("Configuration Fuses Is ALT")
	drvExtPhyLan8700ConfigAlt.setDefaultValue(False)
	
	# Use The Fuses Configuration
	drvExtPhyLan8700ConfigAuto = drvExtPhyLan8700Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTO", drvExtPhyLan8700ConnFlag)
	drvExtPhyLan8700ConfigAuto.setLabel("Use The Fuses Configuration")
	drvExtPhyLan8700ConfigAuto.setVisible(True)
	drvExtPhyLan8700ConfigAuto.setDescription("Use The Fuses Configuration")
	drvExtPhyLan8700ConfigAuto.setDefaultValue(False)
	
	# External PHY Type
	#tcpipEmacPhyType = drvExtPhyLan8700Component.createComboSymbol("TCPIP_EMAC_PHY_TYPE", None, TCPIP_STACK_PHY_TYPE) # Niyas to check the range values
	#tcpipEmacPhyType.setLabel("External PHY Type")
	#tcpipEmacPhyType.setVisible(True)# Niyas to check the visibility 
	#tcpipEmacPhyType.setDescription("External PHY Type")
	#tcpipEmacPhyType.setDefaultValue("KSZ8061")
	#tcpipEmacPhyType.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

	# Driver PHY Instances Number
	drvExtPhyLan8700InstanceNum= drvExtPhyLan8700Component.createIntegerSymbol("DRV_ETHPHY_INSTANCES_NUMBER", None)
	drvExtPhyLan8700InstanceNum.setLabel("PHY Instances Number") 
	drvExtPhyLan8700InstanceNum.setVisible(True)
	drvExtPhyLan8700InstanceNum.setDescription("Driver PHY Instances Number")
	drvExtPhyLan8700InstanceNum.setDefaultValue(1)
	
	# Driver PHY Clients Number
	drvExtPhyLan8700ClientNum= drvExtPhyLan8700Component.createIntegerSymbol("DRV_ETHPHY_CLIENTS_NUMBER", None)
	drvExtPhyLan8700ClientNum.setLabel("PHY Clients Number") 
	drvExtPhyLan8700ClientNum.setVisible(True)
	drvExtPhyLan8700ClientNum.setDescription("Driver PHY Clients Number")
	drvExtPhyLan8700ClientNum.setDefaultValue(1)
	
	# Driver PHY Peripheral Index Number
	drvExtPhyLan8700IndexNum= drvExtPhyLan8700Component.createIntegerSymbol("DRV_ETHPHY_INDEX", None)
	drvExtPhyLan8700IndexNum.setLabel("PHY Peripheral Index Number") 
	drvExtPhyLan8700IndexNum.setVisible(True)
	drvExtPhyLan8700IndexNum.setDescription("Driver PHY Peripheral Index Number")
	drvExtPhyLan8700IndexNum.setDefaultValue(1)
	
	# Driver PHY Peripheral ID
	drvExtPhyLan8700PeripheralId= drvExtPhyLan8700Component.createIntegerSymbol("DRV_ETHPHY_PERIPHERAL_ID", None)
	drvExtPhyLan8700PeripheralId.setLabel("PHY Peripheral ID") 
	drvExtPhyLan8700PeripheralId.setVisible(True)
	drvExtPhyLan8700PeripheralId.setDescription("Driver PHY Peripheral ID")
	drvExtPhyLan8700PeripheralId.setDefaultValue(1)
	
	# Driver PHY Negotiation Time-out - ms
	drvExtPhyLan8700NegInitTimeout= drvExtPhyLan8700Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", None)
	drvExtPhyLan8700NegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
	drvExtPhyLan8700NegInitTimeout.setVisible(True)
	drvExtPhyLan8700NegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
	drvExtPhyLan8700NegInitTimeout.setDefaultValue(1)
	
	# Driver PHY Negotiation Done Time-out - ms
	drvExtPhyLan8700NegDoneTimeout= drvExtPhyLan8700Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", None)
	drvExtPhyLan8700NegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
	drvExtPhyLan8700NegDoneTimeout.setVisible(True)
	drvExtPhyLan8700NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
	drvExtPhyLan8700NegDoneTimeout.setDefaultValue(2000)
	
	# Driver PHY Reset Clear Time-out - ms
	drvExtPhyLan8700ResetClearTimeout= drvExtPhyLan8700Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", None)
	drvExtPhyLan8700ResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
	drvExtPhyLan8700ResetClearTimeout.setVisible(True)
	drvExtPhyLan8700ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
	drvExtPhyLan8700ResetClearTimeout.setDefaultValue(500)
	
	# Use a Function to be called at PHY Reset
	drvExtPhyLan8700ResetCallbackEnable = drvExtPhyLan8700Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
	drvExtPhyLan8700ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
	drvExtPhyLan8700ResetCallbackEnable.setVisible(True)
	drvExtPhyLan8700ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
	drvExtPhyLan8700ResetCallbackEnable.setDefaultValue(False)
	
	# App Function
	drvExtPhyLan8700ResetCallback = drvExtPhyLan8700Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyLan8700ResetCallbackEnable)
	drvExtPhyLan8700ResetCallback.setLabel("App Function")
	drvExtPhyLan8700ResetCallback.setVisible(False)# Niyas to check the visibility 
	drvExtPhyLan8700ResetCallback.setDescription("App Function")
	drvExtPhyLan8700ResetCallback.setDefaultValue("AppPhyResetFunction")
	drvExtPhyLan8700ResetCallback.setDependencies(drvExtPhyLan8700MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])

	#Add to system_config.h
	drvExtPhyLan8700HeaderFtl = drvExtPhyLan8700Component.createFileSymbol(None, None)
	drvExtPhyLan8700HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_lan8700.h.ftl")
	drvExtPhyLan8700HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	drvExtPhyLan8700HeaderFtl.setMarkup(True)
	drvExtPhyLan8700HeaderFtl.setType("STRING")
	
	# file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
	# Add drv_ethphy.h file to project
	drvExtPhyHeaderFile = drvExtPhyLan8700Component.createFileSymbol(None, None)
	drvExtPhyHeaderFile.setSourcePath("driver/ethphy/drv_ethphy.h")
	drvExtPhyHeaderFile.setOutputName("drv_ethphy.h")
	drvExtPhyHeaderFile.setDestPath("driver/ethphy/")
	drvExtPhyHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/")
	drvExtPhyHeaderFile.setType("HEADER")
	drvExtPhyHeaderFile.setOverwrite(True)
	drvExtPhyHeaderFile.setEnabled(True)
	#drvEthPhyHeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])

	# file TCPIP_ETH_PHY_LOCAL_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/drv_ethphy_local.h" to           "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/drv_ethphy_local.h"
	# Add drv_ethphy_local.h file to project
	drvExtPhyLocalHeaderFile = drvExtPhyLan8700Component.createFileSymbol(None, None)
	drvExtPhyLocalHeaderFile.setSourcePath("driver/ethphy/src/drv_ethphy_local.h")
	drvExtPhyLocalHeaderFile.setOutputName("drv_ethphy_local.h")
	drvExtPhyLocalHeaderFile.setDestPath("driver/ethphy/src/")
	drvExtPhyLocalHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/")
	drvExtPhyLocalHeaderFile.setType("HEADER")
	drvExtPhyLocalHeaderFile.setOverwrite(True)
	drvExtPhyLocalHeaderFile.setEnabled(True)
	#drvEthPhyLocalHeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])

	# file TCPIP_ETH_EXT_PHY_REGS_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h" to    "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_regs.h"
	# Add drv_extphy_regs.h file to project
	drvExtPhyLan8700RegHeaderFile = drvExtPhyLan8700Component.createFileSymbol(None, None)
	drvExtPhyLan8700RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
	drvExtPhyLan8700RegHeaderFile.setOutputName("drv_extphy_regs.h")
	drvExtPhyLan8700RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhyLan8700RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhyLan8700RegHeaderFile.setType("HEADER")
	drvExtPhyLan8700RegHeaderFile.setOverwrite(True)
	drvExtPhyLan8700RegHeaderFile.setEnabled(True)
	#drvExtPhyLan8700RegHeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])    

	# file TCPIP_MAC_PHY_LAN8700_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8700.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8700.h"
	# Add drv_extphy_smsc8700.h file to project
	drvExtPhyLan8700HeaderFile = drvExtPhyLan8700Component.createFileSymbol(None, None)
	drvExtPhyLan8700HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc8700.h")
	drvExtPhyLan8700HeaderFile.setOutputName("drv_extphy_smsc8700.h")
	drvExtPhyLan8700HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhyLan8700HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhyLan8700HeaderFile.setType("HEADER")
	drvExtPhyLan8700HeaderFile.setOverwrite(True)
	#drvExtPhyLan8700HeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])  


	# file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
	# Add drv_ethphy.c file
	drvExtPhySourceFile = drvExtPhyLan8700Component.createFileSymbol(None, None)
	drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
	drvExtPhySourceFile.setOutputName("drv_ethphy.c")
	drvExtPhySourceFile.setOverwrite(True)
	drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhySourceFile.setType("SOURCE")
	drvExtPhySourceFile.setEnabled(True)
	#drvExtPhySourceFile.setDependencies(tcpipGmacGenSourceFile, ["TCPIP_USE_ETH_MAC"])

	
	# ifblock TCPIP_EMAC_PHY_TYPE = "SMSC_LAN8700"
	# file TCPIP_MAC_PHY_LAN8700_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8700.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_smsc8700.c"
	# endif
	# Add drv_extphy_smsc8700.c file
	drvExtPhyLan8700SourceFile = drvExtPhyLan8700Component.createFileSymbol(None, None)
	drvExtPhyLan8700SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc8700.c")
	drvExtPhyLan8700SourceFile.setOutputName("drv_extphy_smsc8700.c")
	drvExtPhyLan8700SourceFile.setOverwrite(True)
	drvExtPhyLan8700SourceFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhyLan8700SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhyLan8700SourceFile.setType("SOURCE")
	drvExtPhyLan8700SourceFile.setEnabled(True)
	# drvExtPhySmscLan8700SourceFile.setDependencies(drvExtPhySmscLan8700GenSourceFile, ["TCPIP_USE_ETH_MAC","TCPIP_EMAC_PHY_TYPE"])
	
	
def drvExtPhyLan8700MenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("EthMac Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("EthMac Menu Invisible.")
		symbol.setVisible(False)
		
