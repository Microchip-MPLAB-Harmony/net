	
__TCPIP_DIR = "D:/06_Repo/05_H3/h3/net"
def instantiateComponent(drvExtPhyKsz8041Component):
	print("KSZ8041 PHY Driver Component")
	configName = Variables.get("__CONFIGURATION_NAME")

	# Delay for the Link Initialization in ms
	drvExtPhyKsz8041LinkInitDelay= drvExtPhyKsz8041Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", None)
	drvExtPhyKsz8041LinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
	drvExtPhyKsz8041LinkInitDelay.setVisible(True)
	drvExtPhyKsz8041LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
	drvExtPhyKsz8041LinkInitDelay.setDefaultValue(500)
	#drvExtPhyKsz8041LinkInitDelay.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

	# PHY Address
	drvExtPhyKsz8041Addr= drvExtPhyKsz8041Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
	drvExtPhyKsz8041Addr.setLabel("PHY Address") 
	drvExtPhyKsz8041Addr.setVisible(True)
	drvExtPhyKsz8041Addr.setDescription("PHY Address")
	drvExtPhyKsz8041Addr.setDefaultValue(0)

	# External PHY Connection Flags
	drvExtPhyKsz8041ConnFlag = drvExtPhyKsz8041Component.createMenuSymbol(None, None) 
	drvExtPhyKsz8041ConnFlag.setLabel("External PHY Connection Flags")
	drvExtPhyKsz8041ConnFlag.setVisible(True)
	drvExtPhyKsz8041ConnFlag.setDescription("External PHY Connection Flags")
	
	# RMII Data Interface
	drvExtPhyKsz8041ConfigRmii = drvExtPhyKsz8041Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyKsz8041ConnFlag)
	drvExtPhyKsz8041ConfigRmii.setLabel("RMII Data Interface")
	drvExtPhyKsz8041ConfigRmii.setVisible(True)
	drvExtPhyKsz8041ConfigRmii.setDescription("RMII Data Interface")
	drvExtPhyKsz8041ConfigRmii.setDefaultValue(True)
	
	# Configuration Fuses Is ALT
	drvExtPhyKsz8041ConfigAlt = drvExtPhyKsz8041Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_ALTERNATE", drvExtPhyKsz8041ConnFlag)
	drvExtPhyKsz8041ConfigAlt.setLabel("Configuration Fuses Is ALT")
	drvExtPhyKsz8041ConfigAlt.setVisible(True)
	drvExtPhyKsz8041ConfigAlt.setDescription("Configuration Fuses Is ALT")
	drvExtPhyKsz8041ConfigAlt.setDefaultValue(False)
	
	# Use The Fuses Configuration
	drvExtPhyKsz8041ConfigAuto = drvExtPhyKsz8041Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTO", drvExtPhyKsz8041ConnFlag)
	drvExtPhyKsz8041ConfigAuto.setLabel("Use The Fuses Configuration")
	drvExtPhyKsz8041ConfigAuto.setVisible(True)
	drvExtPhyKsz8041ConfigAuto.setDescription("Use The Fuses Configuration")
	drvExtPhyKsz8041ConfigAuto.setDefaultValue(False)
	
	# External PHY Type
	#tcpipEmacPhyType = drvExtPhyKsz8041Component.createComboSymbol("TCPIP_EMAC_PHY_TYPE", None, TCPIP_STACK_PHY_TYPE) # Niyas to check the range values
	#tcpipEmacPhyType.setLabel("External PHY Type")
	#tcpipEmacPhyType.setVisible(True)# Niyas to check the visibility 
	#tcpipEmacPhyType.setDescription("External PHY Type")
	#tcpipEmacPhyType.setDefaultValue("KSZ8041")
	#tcpipEmacPhyType.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

	# Driver PHY Instances Number
	drvExtPhyKsz8041InstanceNum= drvExtPhyKsz8041Component.createIntegerSymbol("DRV_ETHPHY_INSTANCES_NUMBER", None)
	drvExtPhyKsz8041InstanceNum.setLabel("PHY Instances Number") 
	drvExtPhyKsz8041InstanceNum.setVisible(True)
	drvExtPhyKsz8041InstanceNum.setDescription("Driver PHY Instances Number")
	drvExtPhyKsz8041InstanceNum.setDefaultValue(1)
	
	# Driver PHY Clients Number
	drvExtPhyKsz8041ClientNum= drvExtPhyKsz8041Component.createIntegerSymbol("DRV_ETHPHY_CLIENTS_NUMBER", None)
	drvExtPhyKsz8041ClientNum.setLabel("PHY Clients Number") 
	drvExtPhyKsz8041ClientNum.setVisible(True)
	drvExtPhyKsz8041ClientNum.setDescription("Driver PHY Clients Number")
	drvExtPhyKsz8041ClientNum.setDefaultValue(1)
	
	# Driver PHY Peripheral Index Number
	drvExtPhyKsz8041IndexNum= drvExtPhyKsz8041Component.createIntegerSymbol("DRV_ETHPHY_INDEX", None)
	drvExtPhyKsz8041IndexNum.setLabel("PHY Peripheral Index Number") 
	drvExtPhyKsz8041IndexNum.setVisible(True)
	drvExtPhyKsz8041IndexNum.setDescription("Driver PHY Peripheral Index Number")
	drvExtPhyKsz8041IndexNum.setDefaultValue(1)
	
	# Driver PHY Peripheral ID
	drvExtPhyKsz8041PeripheralId= drvExtPhyKsz8041Component.createIntegerSymbol("DRV_ETHPHY_PERIPHERAL_ID", None)
	drvExtPhyKsz8041PeripheralId.setLabel("PHY Peripheral ID") 
	drvExtPhyKsz8041PeripheralId.setVisible(True)
	drvExtPhyKsz8041PeripheralId.setDescription("Driver PHY Peripheral ID")
	drvExtPhyKsz8041PeripheralId.setDefaultValue(1)
	
	# Driver PHY Negotiation Time-out - ms
	drvExtPhyKsz8041NegInitTimeout= drvExtPhyKsz8041Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", None)
	drvExtPhyKsz8041NegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
	drvExtPhyKsz8041NegInitTimeout.setVisible(True)
	drvExtPhyKsz8041NegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
	drvExtPhyKsz8041NegInitTimeout.setDefaultValue(1)
	
	# Driver PHY Negotiation Done Time-out - ms
	drvExtPhyKsz8041NegDoneTimeout= drvExtPhyKsz8041Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", None)
	drvExtPhyKsz8041NegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
	drvExtPhyKsz8041NegDoneTimeout.setVisible(True)
	drvExtPhyKsz8041NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
	drvExtPhyKsz8041NegDoneTimeout.setDefaultValue(2000)
	
	# Driver PHY Reset Clear Time-out - ms
	drvExtPhyKsz8041ResetClearTimeout= drvExtPhyKsz8041Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", None)
	drvExtPhyKsz8041ResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
	drvExtPhyKsz8041ResetClearTimeout.setVisible(True)
	drvExtPhyKsz8041ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
	drvExtPhyKsz8041ResetClearTimeout.setDefaultValue(500)
	
	# Use a Function to be called at PHY Reset
	drvExtPhyKsz8041ResetCallbackEnable = drvExtPhyKsz8041Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
	drvExtPhyKsz8041ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
	drvExtPhyKsz8041ResetCallbackEnable.setVisible(True)
	drvExtPhyKsz8041ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
	drvExtPhyKsz8041ResetCallbackEnable.setDefaultValue(False)
	
	# App Function
	drvExtPhyKsz8041ResetCallback = drvExtPhyKsz8041Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyKsz8041ResetCallbackEnable)
	drvExtPhyKsz8041ResetCallback.setLabel("App Function")
	drvExtPhyKsz8041ResetCallback.setVisible(False)# Niyas to check the visibility 
	drvExtPhyKsz8041ResetCallback.setDescription("App Function")
	drvExtPhyKsz8041ResetCallback.setDefaultValue("AppPhyResetFunction")
	drvExtPhyKsz8041ResetCallback.setDependencies(drvExtPhyKsz8041MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])

	#Add to system_config.h
	drvExtPhyKsz8041HeaderFtl = drvExtPhyKsz8041Component.createFileSymbol(None, None)
	drvExtPhyKsz8041HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_ksz8041.h.ftl")
	drvExtPhyKsz8041HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	drvExtPhyKsz8041HeaderFtl.setMarkup(True)
	drvExtPhyKsz8041HeaderFtl.setType("STRING")
	
	# file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
	# Add drv_ethphy.h file to project
	drvExtPhyHeaderFile = drvExtPhyKsz8041Component.createFileSymbol(None, None)
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
	drvExtPhyLocalHeaderFile = drvExtPhyKsz8041Component.createFileSymbol(None, None)
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
	drvExtPhyKsz8041RegHeaderFile = drvExtPhyKsz8041Component.createFileSymbol(None, None)
	drvExtPhyKsz8041RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
	drvExtPhyKsz8041RegHeaderFile.setOutputName("drv_extphy_regs.h")
	drvExtPhyKsz8041RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhyKsz8041RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhyKsz8041RegHeaderFile.setType("HEADER")
	drvExtPhyKsz8041RegHeaderFile.setOverwrite(True)
	drvExtPhyKsz8041RegHeaderFile.setEnabled(True)
	#drvExtPhyKsz8041RegHeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])    

	# file TCPIP_MAC_PHY_KSZ8041_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_ksz8041.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_ksz8041.h"
	#Add drv_extphy_ksz8041.h file to project
	drvExtPhyKsz8041HeaderFile = drvExtPhyKsz8041Component.createFileSymbol(None, None)
	drvExtPhyKsz8041HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz8041.h")
	drvExtPhyKsz8041HeaderFile.setOutputName("drv_extphy_ksz8041.h")
	drvExtPhyKsz8041HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhyKsz8041HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhyKsz8041HeaderFile.setType("HEADER")
	drvExtPhyKsz8041HeaderFile.setOverwrite(True)
	# drvExtPhyKsz8041HeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])

	# file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
	# Add drv_ethphy.c file
	drvExtPhySourceFile = drvExtPhyKsz8041Component.createFileSymbol(None, None)
	drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
	drvExtPhySourceFile.setOutputName("drv_ethphy.c")
	drvExtPhySourceFile.setOverwrite(True)
	drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhySourceFile.setType("SOURCE")
	drvExtPhySourceFile.setEnabled(True)
	#drvExtPhySourceFile.setDependencies(tcpipGmacGenSourceFile, ["TCPIP_USE_ETH_MAC"])

	# ifblock TCPIP_EMAC_PHY_TYPE = "KSZ8041"
	# file TCPIP_MAC_PHY_KSZ8041_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_ksz8041.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_ksz8041.c"
	# endif
	# Add drv_extphy_ksz8041.c file
	drvExtPhyKsz8041SourceFile = drvExtPhyKsz8041Component.createFileSymbol(None, None)
	drvExtPhyKsz8041SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz8041.c")
	drvExtPhyKsz8041SourceFile.setOutputName("drv_extphy_ksz8041.c")
	drvExtPhyKsz8041SourceFile.setOverwrite(True)
	drvExtPhyKsz8041SourceFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhyKsz8041SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhyKsz8041SourceFile.setType("SOURCE")
	drvExtPhyKsz8041SourceFile.setEnabled(True)
	# drvExtPhyKsz8041SourceFile.setDependencies(drvExtPhyKsz8041GenSourceFile, ["TCPIP_USE_ETH_MAC","TCPIP_EMAC_PHY_TYPE"])

	
def drvExtPhyKsz8041MenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("EthMac Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("EthMac Menu Invisible.")
		symbol.setVisible(False)
		
	
