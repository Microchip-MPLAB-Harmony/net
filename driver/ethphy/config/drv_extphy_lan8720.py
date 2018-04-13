__TCPIP_DIR = "D:/06_Repo/05_H3/h3/net"
def instantiateComponent(drvExtPhyLan8720Component):
	print("KSZ8061 PHY Driver Component")
	configName = Variables.get("__CONFIGURATION_NAME")

	# Delay for the Link Initialization in ms
	drvExtPhyLan8720LinkInitDelay= drvExtPhyLan8720Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", None)
	drvExtPhyLan8720LinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
	drvExtPhyLan8720LinkInitDelay.setVisible(True)
	drvExtPhyLan8720LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
	drvExtPhyLan8720LinkInitDelay.setDefaultValue(500)
	#drvExtPhyLan8720LinkInitDelay.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

	# PHY Address
	drvExtPhyLan8720Addr= drvExtPhyLan8720Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
	drvExtPhyLan8720Addr.setLabel("PHY Address") 
	drvExtPhyLan8720Addr.setVisible(True)
	drvExtPhyLan8720Addr.setDescription("PHY Address")
	drvExtPhyLan8720Addr.setDefaultValue(0)

	# External PHY Connection Flags
	drvExtPhyLan8720ConnFlag = drvExtPhyLan8720Component.createMenuSymbol(None, None) 
	drvExtPhyLan8720ConnFlag.setLabel("External PHY Connection Flags")
	drvExtPhyLan8720ConnFlag.setVisible(True)
	drvExtPhyLan8720ConnFlag.setDescription("External PHY Connection Flags")
	
	# RMII Data Interface
	drvExtPhyLan8720ConfigRmii = drvExtPhyLan8720Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyLan8720ConnFlag)
	drvExtPhyLan8720ConfigRmii.setLabel("RMII Data Interface")
	drvExtPhyLan8720ConfigRmii.setVisible(True)
	drvExtPhyLan8720ConfigRmii.setDescription("RMII Data Interface")
	drvExtPhyLan8720ConfigRmii.setDefaultValue(True)
	
	# Configuration Fuses Is ALT
	drvExtPhyLan8720ConfigAlt = drvExtPhyLan8720Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_ALTERNATE", drvExtPhyLan8720ConnFlag)
	drvExtPhyLan8720ConfigAlt.setLabel("Configuration Fuses Is ALT")
	drvExtPhyLan8720ConfigAlt.setVisible(True)
	drvExtPhyLan8720ConfigAlt.setDescription("Configuration Fuses Is ALT")
	drvExtPhyLan8720ConfigAlt.setDefaultValue(False)
	
	# Use The Fuses Configuration
	drvExtPhyLan8720ConfigAuto = drvExtPhyLan8720Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTO", drvExtPhyLan8720ConnFlag)
	drvExtPhyLan8720ConfigAuto.setLabel("Use The Fuses Configuration")
	drvExtPhyLan8720ConfigAuto.setVisible(True)
	drvExtPhyLan8720ConfigAuto.setDescription("Use The Fuses Configuration")
	drvExtPhyLan8720ConfigAuto.setDefaultValue(False)
	
	# External PHY Type
	#tcpipEmacPhyType = drvExtPhyLan8720Component.createComboSymbol("TCPIP_EMAC_PHY_TYPE", None, TCPIP_STACK_PHY_TYPE) # Niyas to check the range values
	#tcpipEmacPhyType.setLabel("External PHY Type")
	#tcpipEmacPhyType.setVisible(True)# Niyas to check the visibility 
	#tcpipEmacPhyType.setDescription("External PHY Type")
	#tcpipEmacPhyType.setDefaultValue("KSZ8061")
	#tcpipEmacPhyType.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

	# Driver PHY Instances Number
	drvExtPhyLan8720InstanceNum= drvExtPhyLan8720Component.createIntegerSymbol("DRV_ETHPHY_INSTANCES_NUMBER", None)
	drvExtPhyLan8720InstanceNum.setLabel("PHY Instances Number") 
	drvExtPhyLan8720InstanceNum.setVisible(True)
	drvExtPhyLan8720InstanceNum.setDescription("Driver PHY Instances Number")
	drvExtPhyLan8720InstanceNum.setDefaultValue(1)
	
	# Driver PHY Clients Number
	drvExtPhyLan8720ClientNum= drvExtPhyLan8720Component.createIntegerSymbol("DRV_ETHPHY_CLIENTS_NUMBER", None)
	drvExtPhyLan8720ClientNum.setLabel("PHY Clients Number") 
	drvExtPhyLan8720ClientNum.setVisible(True)
	drvExtPhyLan8720ClientNum.setDescription("Driver PHY Clients Number")
	drvExtPhyLan8720ClientNum.setDefaultValue(1)
	
	# Driver PHY Peripheral Index Number
	drvExtPhyLan8720IndexNum= drvExtPhyLan8720Component.createIntegerSymbol("DRV_ETHPHY_INDEX", None)
	drvExtPhyLan8720IndexNum.setLabel("PHY Peripheral Index Number") 
	drvExtPhyLan8720IndexNum.setVisible(True)
	drvExtPhyLan8720IndexNum.setDescription("Driver PHY Peripheral Index Number")
	drvExtPhyLan8720IndexNum.setDefaultValue(1)
	
	# Driver PHY Peripheral ID
	drvExtPhyLan8720PeripheralId= drvExtPhyLan8720Component.createIntegerSymbol("DRV_ETHPHY_PERIPHERAL_ID", None)
	drvExtPhyLan8720PeripheralId.setLabel("PHY Peripheral ID") 
	drvExtPhyLan8720PeripheralId.setVisible(True)
	drvExtPhyLan8720PeripheralId.setDescription("Driver PHY Peripheral ID")
	drvExtPhyLan8720PeripheralId.setDefaultValue(1)
	
	# Driver PHY Negotiation Time-out - ms
	drvExtPhyLan8720NegInitTimeout= drvExtPhyLan8720Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", None)
	drvExtPhyLan8720NegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
	drvExtPhyLan8720NegInitTimeout.setVisible(True)
	drvExtPhyLan8720NegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
	drvExtPhyLan8720NegInitTimeout.setDefaultValue(1)
	
	# Driver PHY Negotiation Done Time-out - ms
	drvExtPhyLan8720NegDoneTimeout= drvExtPhyLan8720Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", None)
	drvExtPhyLan8720NegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
	drvExtPhyLan8720NegDoneTimeout.setVisible(True)
	drvExtPhyLan8720NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
	drvExtPhyLan8720NegDoneTimeout.setDefaultValue(2000)
	
	# Driver PHY Reset Clear Time-out - ms
	drvExtPhyLan8720ResetClearTimeout= drvExtPhyLan8720Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", None)
	drvExtPhyLan8720ResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
	drvExtPhyLan8720ResetClearTimeout.setVisible(True)
	drvExtPhyLan8720ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
	drvExtPhyLan8720ResetClearTimeout.setDefaultValue(500)
	
	# Use a Function to be called at PHY Reset
	drvExtPhyLan8720ResetCallbackEnable = drvExtPhyLan8720Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
	drvExtPhyLan8720ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
	drvExtPhyLan8720ResetCallbackEnable.setVisible(True)
	drvExtPhyLan8720ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
	drvExtPhyLan8720ResetCallbackEnable.setDefaultValue(False)
	
	# App Function
	drvExtPhyLan8720ResetCallback = drvExtPhyLan8720Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyLan8720ResetCallbackEnable)
	drvExtPhyLan8720ResetCallback.setLabel("App Function")
	drvExtPhyLan8720ResetCallback.setVisible(False)# Niyas to check the visibility 
	drvExtPhyLan8720ResetCallback.setDescription("App Function")
	drvExtPhyLan8720ResetCallback.setDefaultValue("AppPhyResetFunction")
	drvExtPhyLan8720ResetCallback.setDependencies(drvExtPhyLan8720MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])

	#Add to system_config.h
	drvExtPhyLan8720HeaderFtl = drvExtPhyLan8720Component.createFileSymbol(None, None)
	drvExtPhyLan8720HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_lan8720.h.ftl")
	drvExtPhyLan8720HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	drvExtPhyLan8720HeaderFtl.setMarkup(True)
	drvExtPhyLan8720HeaderFtl.setType("STRING")
	
	# file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
	# Add drv_ethphy.h file to project
	drvExtPhyHeaderFile = drvExtPhyLan8720Component.createFileSymbol(None, None)
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
	drvExtPhyLocalHeaderFile = drvExtPhyLan8720Component.createFileSymbol(None, None)
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
	drvExtPhyLan8720RegHeaderFile = drvExtPhyLan8720Component.createFileSymbol(None, None)
	drvExtPhyLan8720RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
	drvExtPhyLan8720RegHeaderFile.setOutputName("drv_extphy_regs.h")
	drvExtPhyLan8720RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhyLan8720RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhyLan8720RegHeaderFile.setType("HEADER")
	drvExtPhyLan8720RegHeaderFile.setOverwrite(True)
	drvExtPhyLan8720RegHeaderFile.setEnabled(True)
	#drvExtPhyLan8720RegHeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])    

	# file TCPIP_MAC_PHY_LAN8720_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8720.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8720.h"
	# Add drv_extphy_smsc8720.h file to project
	drvExtPhyLan8720HeaderFile = drvExtPhyLan8720Component.createFileSymbol(None, None)
	drvExtPhyLan8720HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc8720.h")
	drvExtPhyLan8720HeaderFile.setOutputName("drv_extphy_smsc8720.h")
	drvExtPhyLan8720HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhyLan8720HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhyLan8720HeaderFile.setType("HEADER")
	drvExtPhyLan8720HeaderFile.setOverwrite(True)
	#drvExtPhyLan8720HeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])  


	# file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
	# Add drv_ethphy.c file
	drvExtPhySourceFile = drvExtPhyLan8720Component.createFileSymbol(None, None)
	drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
	drvExtPhySourceFile.setOutputName("drv_ethphy.c")
	drvExtPhySourceFile.setOverwrite(True)
	drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhySourceFile.setType("SOURCE")
	drvExtPhySourceFile.setEnabled(True)
	#drvExtPhySourceFile.setDependencies(tcpipGmacGenSourceFile, ["TCPIP_USE_ETH_MAC"])

	
	# ifblock TCPIP_EMAC_PHY_TYPE = "SMSC_LAN8720"
	# file TCPIP_MAC_PHY_LAN8720_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8720.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_smsc8720.c"
	# endif
	# Add drv_extphy_smsc8720.c file
	drvExtPhyLan8720SourceFile = drvExtPhyLan8720Component.createFileSymbol(None, None)
	drvExtPhyLan8720SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc8720.c")
	drvExtPhyLan8720SourceFile.setOutputName("drv_extphy_smsc8720.c")
	drvExtPhyLan8720SourceFile.setOverwrite(True)
	drvExtPhyLan8720SourceFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhyLan8720SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhyLan8720SourceFile.setType("SOURCE")
	drvExtPhyLan8720SourceFile.setEnabled(True)
	# drvExtPhySmscLan8720SourceFile.setDependencies(drvExtPhySmscLan8720GenSourceFile, ["TCPIP_USE_ETH_MAC","TCPIP_EMAC_PHY_TYPE"])
	
	
def drvExtPhyLan8720MenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("EthMac Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("EthMac Menu Invisible.")
		symbol.setVisible(False)
		
