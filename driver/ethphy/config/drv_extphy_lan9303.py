__TCPIP_DIR = "D:/06_Repo/05_H3/h3/net"
def instantiateComponent(drvExtPhyLan9303Component):
	print("KSZ8061 PHY Driver Component")
	configName = Variables.get("__CONFIGURATION_NAME")

	# Delay for the Link Initialization in ms
	drvExtPhyLan9303LinkInitDelay= drvExtPhyLan9303Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", None)
	drvExtPhyLan9303LinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
	drvExtPhyLan9303LinkInitDelay.setVisible(True)
	drvExtPhyLan9303LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
	drvExtPhyLan9303LinkInitDelay.setDefaultValue(500)
	#drvExtPhyLan9303LinkInitDelay.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

	# PHY Address
	drvExtPhyLan9303Addr= drvExtPhyLan9303Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
	drvExtPhyLan9303Addr.setLabel("PHY Address") 
	drvExtPhyLan9303Addr.setVisible(True)
	drvExtPhyLan9303Addr.setDescription("PHY Address")
	drvExtPhyLan9303Addr.setDefaultValue(0)


	# External PHY Connection Flags
	drvExtPhyLan9303ConnFlag = drvExtPhyLan9303Component.createMenuSymbol(None, None) 
	drvExtPhyLan9303ConnFlag.setLabel("External PHY Connection Flags")
	drvExtPhyLan9303ConnFlag.setVisible(True)
	drvExtPhyLan9303ConnFlag.setDescription("External PHY Connection Flags")
	
	# RMII Data Interface
	drvExtPhyLan9303ConfigRmii = drvExtPhyLan9303Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyLan9303ConnFlag)
	drvExtPhyLan9303ConfigRmii.setLabel("RMII Data Interface")
	drvExtPhyLan9303ConfigRmii.setVisible(True)
	drvExtPhyLan9303ConfigRmii.setDescription("RMII Data Interface")
	drvExtPhyLan9303ConfigRmii.setDefaultValue(True)
	
	# Configuration Fuses Is ALT
	drvExtPhyLan9303ConfigAlt = drvExtPhyLan9303Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_ALTERNATE", drvExtPhyLan9303ConnFlag)
	drvExtPhyLan9303ConfigAlt.setLabel("Configuration Fuses Is ALT")
	drvExtPhyLan9303ConfigAlt.setVisible(True)
	drvExtPhyLan9303ConfigAlt.setDescription("Configuration Fuses Is ALT")
	drvExtPhyLan9303ConfigAlt.setDefaultValue(False)
	
	# Use The Fuses Configuration
	drvExtPhyLan9303ConfigAuto = drvExtPhyLan9303Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTO", drvExtPhyLan9303ConnFlag)
	drvExtPhyLan9303ConfigAuto.setLabel("Use The Fuses Configuration")
	drvExtPhyLan9303ConfigAuto.setVisible(True)
	drvExtPhyLan9303ConfigAuto.setDescription("Use The Fuses Configuration")
	drvExtPhyLan9303ConfigAuto.setDefaultValue(False)

	# Driver PHY Instances Number
	drvExtPhyLan9303InstanceNum= drvExtPhyLan9303Component.createIntegerSymbol("DRV_ETHPHY_INSTANCES_NUMBER", None)
	drvExtPhyLan9303InstanceNum.setLabel("PHY Instances Number") 
	drvExtPhyLan9303InstanceNum.setVisible(True)
	drvExtPhyLan9303InstanceNum.setDescription("Driver PHY Instances Number")
	drvExtPhyLan9303InstanceNum.setDefaultValue(1)
	
	# Driver PHY Clients Number
	drvExtPhyLan9303ClientNum= drvExtPhyLan9303Component.createIntegerSymbol("DRV_ETHPHY_CLIENTS_NUMBER", None)
	drvExtPhyLan9303ClientNum.setLabel("PHY Clients Number") 
	drvExtPhyLan9303ClientNum.setVisible(True)
	drvExtPhyLan9303ClientNum.setDescription("Driver PHY Clients Number")
	drvExtPhyLan9303ClientNum.setDefaultValue(1)
	
	# Driver PHY Peripheral Index Number
	drvExtPhyLan9303IndexNum= drvExtPhyLan9303Component.createIntegerSymbol("DRV_ETHPHY_INDEX", None)
	drvExtPhyLan9303IndexNum.setLabel("PHY Peripheral Index Number") 
	drvExtPhyLan9303IndexNum.setVisible(True)
	drvExtPhyLan9303IndexNum.setDescription("Driver PHY Peripheral Index Number")
	drvExtPhyLan9303IndexNum.setDefaultValue(1)
	
	# Driver PHY Peripheral ID
	drvExtPhyLan9303PeripheralId= drvExtPhyLan9303Component.createIntegerSymbol("DRV_ETHPHY_PERIPHERAL_ID", None)
	drvExtPhyLan9303PeripheralId.setLabel("PHY Peripheral ID") 
	drvExtPhyLan9303PeripheralId.setVisible(True)
	drvExtPhyLan9303PeripheralId.setDescription("Driver PHY Peripheral ID")
	drvExtPhyLan9303PeripheralId.setDefaultValue(1)
	
	# Driver PHY Negotiation Time-out - ms
	drvExtPhyLan9303NegInitTimeout= drvExtPhyLan9303Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", None)
	drvExtPhyLan9303NegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
	drvExtPhyLan9303NegInitTimeout.setVisible(True)
	drvExtPhyLan9303NegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
	drvExtPhyLan9303NegInitTimeout.setDefaultValue(1)
	
	# Driver PHY Negotiation Done Time-out - ms
	drvExtPhyLan9303NegDoneTimeout= drvExtPhyLan9303Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", None)
	drvExtPhyLan9303NegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
	drvExtPhyLan9303NegDoneTimeout.setVisible(True)
	drvExtPhyLan9303NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
	drvExtPhyLan9303NegDoneTimeout.setDefaultValue(2000)
	
	# Driver PHY Reset Clear Time-out - ms
	drvExtPhyLan9303ResetClearTimeout= drvExtPhyLan9303Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", None)
	drvExtPhyLan9303ResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
	drvExtPhyLan9303ResetClearTimeout.setVisible(True)
	drvExtPhyLan9303ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
	drvExtPhyLan9303ResetClearTimeout.setDefaultValue(500)
	
	# Use a Function to be called at PHY Reset
	drvExtPhyLan9303ResetCallbackEnable = drvExtPhyLan9303Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
	drvExtPhyLan9303ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
	drvExtPhyLan9303ResetCallbackEnable.setVisible(True)
	drvExtPhyLan9303ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
	drvExtPhyLan9303ResetCallbackEnable.setDefaultValue(False)
	
	# App Function
	drvExtPhyLan9303ResetCallback = drvExtPhyLan9303Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyLan9303ResetCallbackEnable)
	drvExtPhyLan9303ResetCallback.setLabel("App Function")
	drvExtPhyLan9303ResetCallback.setVisible(False)
	drvExtPhyLan9303ResetCallback.setDescription("App Function")
	drvExtPhyLan9303ResetCallback.setDefaultValue("AppPhyResetFunction")
	drvExtPhyLan9303ResetCallback.setDependencies(drvExtPhyLan9303MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])

	#Add to system_config.h
	drvExtPhyLan9303HeaderFtl = drvExtPhyLan9303Component.createFileSymbol(None, None)
	drvExtPhyLan9303HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_lan9303.h.ftl")
	drvExtPhyLan9303HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	drvExtPhyLan9303HeaderFtl.setMarkup(True)
	drvExtPhyLan9303HeaderFtl.setType("STRING")
	
	# file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
	# Add drv_ethphy.h file to project
	drvExtPhyHeaderFile = drvExtPhyLan9303Component.createFileSymbol(None, None)
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
	drvExtPhyLocalHeaderFile = drvExtPhyLan9303Component.createFileSymbol(None, None)
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
	drvExtPhyLan9303RegHeaderFile = drvExtPhyLan9303Component.createFileSymbol(None, None)
	drvExtPhyLan9303RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
	drvExtPhyLan9303RegHeaderFile.setOutputName("drv_extphy_regs.h")
	drvExtPhyLan9303RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhyLan9303RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhyLan9303RegHeaderFile.setType("HEADER")
	drvExtPhyLan9303RegHeaderFile.setOverwrite(True)
	drvExtPhyLan9303RegHeaderFile.setEnabled(True)
	#drvExtPhyLan9303RegHeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])    

	# file TCPIP_MAC_PHY_LAN9303_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_smsc9303.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_smsc9303.h"
	# Add drv_extphy_smsc9303.h file to project
	drvExtPhyLan9303HeaderFile = drvExtPhyLan9303Component.createFileSymbol(None, None)
	drvExtPhyLan9303HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc9303.h")
	drvExtPhyLan9303HeaderFile.setOutputName("drv_extphy_smsc9303.h")
	drvExtPhyLan9303HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhyLan9303HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhyLan9303HeaderFile.setType("HEADER")
	drvExtPhyLan9303HeaderFile.setOverwrite(True)
	#drvExtPhyLan9303HeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])  


	# file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
	# Add drv_ethphy.c file
	drvExtPhySourceFile = drvExtPhyLan9303Component.createFileSymbol(None, None)
	drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
	drvExtPhySourceFile.setOutputName("drv_ethphy.c")
	drvExtPhySourceFile.setOverwrite(True)
	drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhySourceFile.setType("SOURCE")
	drvExtPhySourceFile.setEnabled(True)
	#drvExtPhySourceFile.setDependencies(tcpipGmacGenSourceFile, ["TCPIP_USE_ETH_MAC"])

	
	# ifblock TCPIP_EMAC_PHY_TYPE = "SMSC_LAN9303"
	# file TCPIP_MAC_PHY_LAN9303_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_smsc9303.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_smsc9303.c"
	# endif
	# Add drv_extphy_smsc9303.c file
	drvExtPhyLan9303SourceFile = drvExtPhyLan9303Component.createFileSymbol(None, None)
	drvExtPhyLan9303SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc9303.c")
	drvExtPhyLan9303SourceFile.setOutputName("drv_extphy_smsc9303.c")
	drvExtPhyLan9303SourceFile.setOverwrite(True)
	drvExtPhyLan9303SourceFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhyLan9303SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhyLan9303SourceFile.setType("SOURCE")
	drvExtPhyLan9303SourceFile.setEnabled(True)
	# drvExtPhySmscLan9303SourceFile.setDependencies(drvExtPhySmscLan9303GenSourceFile, ["TCPIP_USE_ETH_MAC","TCPIP_EMAC_PHY_TYPE"])
	
	
def drvExtPhyLan9303MenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("EthMac Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("EthMac Menu Invisible.")
		symbol.setVisible(False)
		
