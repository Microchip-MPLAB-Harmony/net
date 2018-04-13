__TCPIP_DIR = "D:/06_Repo/05_H3/h3/net"
def instantiateComponent(drvExtPhyLan8740Component):
	print("KSZ8061 PHY Driver Component")
	configName = Variables.get("__CONFIGURATION_NAME")

	# Delay for the Link Initialization in ms
	drvExtPhyLan8740LinkInitDelay= drvExtPhyLan8740Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", None)
	drvExtPhyLan8740LinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
	drvExtPhyLan8740LinkInitDelay.setVisible(True)
	drvExtPhyLan8740LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
	drvExtPhyLan8740LinkInitDelay.setDefaultValue(500)
	#drvExtPhyLan8740LinkInitDelay.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

	# PHY Address
	drvExtPhyLan8740Addr= drvExtPhyLan8740Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
	drvExtPhyLan8740Addr.setLabel("PHY Address") 
	drvExtPhyLan8740Addr.setVisible(True)
	drvExtPhyLan8740Addr.setDescription("PHY Address")
	drvExtPhyLan8740Addr.setDefaultValue(0)

	# External PHY Connection Flags
	drvExtPhyLan8740ConnFlag = drvExtPhyLan8740Component.createMenuSymbol(None, None) 
	drvExtPhyLan8740ConnFlag.setLabel("External PHY Connection Flags")
	drvExtPhyLan8740ConnFlag.setVisible(True)
	drvExtPhyLan8740ConnFlag.setDescription("External PHY Connection Flags")
	
	# RMII Data Interface
	drvExtPhyLan8740ConfigRmii = drvExtPhyLan8740Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyLan8740ConnFlag)
	drvExtPhyLan8740ConfigRmii.setLabel("RMII Data Interface")
	drvExtPhyLan8740ConfigRmii.setVisible(True)
	drvExtPhyLan8740ConfigRmii.setDescription("RMII Data Interface")
	drvExtPhyLan8740ConfigRmii.setDefaultValue(True)
	
	# Configuration Fuses Is ALT
	drvExtPhyLan8740ConfigAlt = drvExtPhyLan8740Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_ALTERNATE", drvExtPhyLan8740ConnFlag)
	drvExtPhyLan8740ConfigAlt.setLabel("Configuration Fuses Is ALT")
	drvExtPhyLan8740ConfigAlt.setVisible(True)
	drvExtPhyLan8740ConfigAlt.setDescription("Configuration Fuses Is ALT")
	drvExtPhyLan8740ConfigAlt.setDefaultValue(False)
	
	# Use The Fuses Configuration
	drvExtPhyLan8740ConfigAuto = drvExtPhyLan8740Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTO", drvExtPhyLan8740ConnFlag)
	drvExtPhyLan8740ConfigAuto.setLabel("Use The Fuses Configuration")
	drvExtPhyLan8740ConfigAuto.setVisible(True)
	drvExtPhyLan8740ConfigAuto.setDescription("Use The Fuses Configuration")
	drvExtPhyLan8740ConfigAuto.setDefaultValue(False)
	
	# External PHY Type
	#tcpipEmacPhyType = drvExtPhyLan8740Component.createComboSymbol("TCPIP_EMAC_PHY_TYPE", None, TCPIP_STACK_PHY_TYPE) # Niyas to check the range values
	#tcpipEmacPhyType.setLabel("External PHY Type")
	#tcpipEmacPhyType.setVisible(True)# Niyas to check the visibility 
	#tcpipEmacPhyType.setDescription("External PHY Type")
	#tcpipEmacPhyType.setDefaultValue("KSZ8061")
	#tcpipEmacPhyType.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

	# Driver PHY Instances Number
	drvExtPhyLan8740InstanceNum= drvExtPhyLan8740Component.createIntegerSymbol("DRV_ETHPHY_INSTANCES_NUMBER", None)
	drvExtPhyLan8740InstanceNum.setLabel("PHY Instances Number") 
	drvExtPhyLan8740InstanceNum.setVisible(True)
	drvExtPhyLan8740InstanceNum.setDescription("Driver PHY Instances Number")
	drvExtPhyLan8740InstanceNum.setDefaultValue(1)
	
	# Driver PHY Clients Number
	drvExtPhyLan8740ClientNum= drvExtPhyLan8740Component.createIntegerSymbol("DRV_ETHPHY_CLIENTS_NUMBER", None)
	drvExtPhyLan8740ClientNum.setLabel("PHY Clients Number") 
	drvExtPhyLan8740ClientNum.setVisible(True)
	drvExtPhyLan8740ClientNum.setDescription("Driver PHY Clients Number")
	drvExtPhyLan8740ClientNum.setDefaultValue(1)
	
	# Driver PHY Peripheral Index Number
	drvExtPhyLan8740IndexNum= drvExtPhyLan8740Component.createIntegerSymbol("DRV_ETHPHY_INDEX", None)
	drvExtPhyLan8740IndexNum.setLabel("PHY Peripheral Index Number") 
	drvExtPhyLan8740IndexNum.setVisible(True)
	drvExtPhyLan8740IndexNum.setDescription("Driver PHY Peripheral Index Number")
	drvExtPhyLan8740IndexNum.setDefaultValue(1)
	
	# Driver PHY Peripheral ID
	drvExtPhyLan8740PeripheralId= drvExtPhyLan8740Component.createIntegerSymbol("DRV_ETHPHY_PERIPHERAL_ID", None)
	drvExtPhyLan8740PeripheralId.setLabel("PHY Peripheral ID") 
	drvExtPhyLan8740PeripheralId.setVisible(True)
	drvExtPhyLan8740PeripheralId.setDescription("Driver PHY Peripheral ID")
	drvExtPhyLan8740PeripheralId.setDefaultValue(1)
	
	# Driver PHY Negotiation Time-out - ms
	drvExtPhyLan8740NegInitTimeout= drvExtPhyLan8740Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", None)
	drvExtPhyLan8740NegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
	drvExtPhyLan8740NegInitTimeout.setVisible(True)
	drvExtPhyLan8740NegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
	drvExtPhyLan8740NegInitTimeout.setDefaultValue(1)
	
	# Driver PHY Negotiation Done Time-out - ms
	drvExtPhyLan8740NegDoneTimeout= drvExtPhyLan8740Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", None)
	drvExtPhyLan8740NegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
	drvExtPhyLan8740NegDoneTimeout.setVisible(True)
	drvExtPhyLan8740NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
	drvExtPhyLan8740NegDoneTimeout.setDefaultValue(2000)
	
	# Driver PHY Reset Clear Time-out - ms
	drvExtPhyLan8740ResetClearTimeout= drvExtPhyLan8740Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", None)
	drvExtPhyLan8740ResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
	drvExtPhyLan8740ResetClearTimeout.setVisible(True)
	drvExtPhyLan8740ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
	drvExtPhyLan8740ResetClearTimeout.setDefaultValue(500)
	
	# Use a Function to be called at PHY Reset
	drvExtPhyLan8740ResetCallbackEnable = drvExtPhyLan8740Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
	drvExtPhyLan8740ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
	drvExtPhyLan8740ResetCallbackEnable.setVisible(True)
	drvExtPhyLan8740ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
	drvExtPhyLan8740ResetCallbackEnable.setDefaultValue(False)
	
	# App Function
	drvExtPhyLan8740ResetCallback = drvExtPhyLan8740Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyLan8740ResetCallbackEnable)
	drvExtPhyLan8740ResetCallback.setLabel("App Function")
	drvExtPhyLan8740ResetCallback.setVisible(False)# Niyas to check the visibility 
	drvExtPhyLan8740ResetCallback.setDescription("App Function")
	drvExtPhyLan8740ResetCallback.setDefaultValue("AppPhyResetFunction")
	drvExtPhyLan8740ResetCallback.setDependencies(drvExtPhyLan8740MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])

	#Add to system_config.h
	drvExtPhyLan8740HeaderFtl = drvExtPhyLan8740Component.createFileSymbol(None, None)
	drvExtPhyLan8740HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_lan8740.h.ftl")
	drvExtPhyLan8740HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	drvExtPhyLan8740HeaderFtl.setMarkup(True)
	drvExtPhyLan8740HeaderFtl.setType("STRING")
	
	# file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
	# Add drv_ethphy.h file to project
	drvExtPhyHeaderFile = drvExtPhyLan8740Component.createFileSymbol(None, None)
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
	drvExtPhyLocalHeaderFile = drvExtPhyLan8740Component.createFileSymbol(None, None)
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
	drvExtPhyLan8740RegHeaderFile = drvExtPhyLan8740Component.createFileSymbol(None, None)
	drvExtPhyLan8740RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
	drvExtPhyLan8740RegHeaderFile.setOutputName("drv_extphy_regs.h")
	drvExtPhyLan8740RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhyLan8740RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhyLan8740RegHeaderFile.setType("HEADER")
	drvExtPhyLan8740RegHeaderFile.setOverwrite(True)
	drvExtPhyLan8740RegHeaderFile.setEnabled(True)
	#drvExtPhyLan8740RegHeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])    

	# file TCPIP_MAC_PHY_LAN8740_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8740.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8740.h"
	# Add drv_extphy_smsc8740.h file to project
	drvExtPhyLan8740HeaderFile = drvExtPhyLan8740Component.createFileSymbol(None, None)
	drvExtPhyLan8740HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc8740.h")
	drvExtPhyLan8740HeaderFile.setOutputName("drv_extphy_smsc8740.h")
	drvExtPhyLan8740HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhyLan8740HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhyLan8740HeaderFile.setType("HEADER")
	drvExtPhyLan8740HeaderFile.setOverwrite(True)
	#drvExtPhyLan8740HeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])  


	# file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
	# Add drv_ethphy.c file
	drvExtPhySourceFile = drvExtPhyLan8740Component.createFileSymbol(None, None)
	drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
	drvExtPhySourceFile.setOutputName("drv_ethphy.c")
	drvExtPhySourceFile.setOverwrite(True)
	drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhySourceFile.setType("SOURCE")
	drvExtPhySourceFile.setEnabled(True)
	#drvExtPhySourceFile.setDependencies(tcpipGmacGenSourceFile, ["TCPIP_USE_ETH_MAC"])

	
	# ifblock TCPIP_EMAC_PHY_TYPE = "SMSC_LAN8740"
	# file TCPIP_MAC_PHY_LAN8740_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8740.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_smsc8740.c"
	# endif
	# Add drv_extphy_smsc8740.c file
	drvExtPhyLan8740SourceFile = drvExtPhyLan8740Component.createFileSymbol(None, None)
	drvExtPhyLan8740SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc8740.c")
	drvExtPhyLan8740SourceFile.setOutputName("drv_extphy_smsc8740.c")
	drvExtPhyLan8740SourceFile.setOverwrite(True)
	drvExtPhyLan8740SourceFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhyLan8740SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhyLan8740SourceFile.setType("SOURCE")
	drvExtPhyLan8740SourceFile.setEnabled(True)
	# drvExtPhySmscLan8740SourceFile.setDependencies(drvExtPhySmscLan8740GenSourceFile, ["TCPIP_USE_ETH_MAC","TCPIP_EMAC_PHY_TYPE"])
	
	
def drvExtPhyLan8740MenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("EthMac Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("EthMac Menu Invisible.")
		symbol.setVisible(False)
		
