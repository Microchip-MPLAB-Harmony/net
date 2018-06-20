__TCPIP_DIR = "D:/06_Repo/05_H3/h3/net"
def instantiateComponent(drvExtPhyKsz8061Component):
	print("KSZ8061 PHY Driver Component")
	configName = Variables.get("__CONFIGURATION_NAME")

	# Delay for the Link Initialization in ms
	drvExtPhyKsz8061LinkInitDelay= drvExtPhyKsz8061Component.createIntegerSymbol("TCPIP_INTMAC_PHY_LINK_INIT_DELAY", None)
	drvExtPhyKsz8061LinkInitDelay.setLabel("Delay for the Link Initialization - ms") 
	drvExtPhyKsz8061LinkInitDelay.setVisible(True)
	drvExtPhyKsz8061LinkInitDelay.setDescription("Delay for the Link Initialization in ms")
	drvExtPhyKsz8061LinkInitDelay.setDefaultValue(500)
	#drvExtPhyKsz8061LinkInitDelay.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

	# PHY Address
	drvExtPhyKsz8061Addr= drvExtPhyKsz8061Component.createIntegerSymbol("TCPIP_INTMAC_PHY_ADDRESS", None)
	drvExtPhyKsz8061Addr.setLabel("PHY Address") 
	drvExtPhyKsz8061Addr.setVisible(True)
	drvExtPhyKsz8061Addr.setDescription("PHY Address")
	drvExtPhyKsz8061Addr.setDefaultValue(1)

	# External PHY Connection Flags
	drvExtPhyKsz8061ConnFlag = drvExtPhyKsz8061Component.createMenuSymbol(None, None) 
	drvExtPhyKsz8061ConnFlag.setLabel("External PHY Connection Flags")
	drvExtPhyKsz8061ConnFlag.setVisible(True)
	drvExtPhyKsz8061ConnFlag.setDescription("External PHY Connection Flags")
	
	# RMII Data Interface
	drvExtPhyKsz8061ConfigRmii = drvExtPhyKsz8061Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_RMII", drvExtPhyKsz8061ConnFlag)
	drvExtPhyKsz8061ConfigRmii.setLabel("RMII Data Interface")
	drvExtPhyKsz8061ConfigRmii.setVisible(True)
	drvExtPhyKsz8061ConfigRmii.setDescription("RMII Data Interface")
	drvExtPhyKsz8061ConfigRmii.setDefaultValue(True)
	
	# Configuration Fuses Is ALT
	drvExtPhyKsz8061ConfigAlt = drvExtPhyKsz8061Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_ALTERNATE", drvExtPhyKsz8061ConnFlag)
	drvExtPhyKsz8061ConfigAlt.setLabel("Configuration Fuses Is ALT")
	drvExtPhyKsz8061ConfigAlt.setVisible(True)
	drvExtPhyKsz8061ConfigAlt.setDescription("Configuration Fuses Is ALT")
	drvExtPhyKsz8061ConfigAlt.setDefaultValue(False)
	
	# Use The Fuses Configuration
	drvExtPhyKsz8061ConfigAuto = drvExtPhyKsz8061Component.createBooleanSymbol("TCPIP_INTMAC_PHY_CONFIG_AUTO", drvExtPhyKsz8061ConnFlag)
	drvExtPhyKsz8061ConfigAuto.setLabel("Use The Fuses Configuration")
	drvExtPhyKsz8061ConfigAuto.setVisible(True)
	drvExtPhyKsz8061ConfigAuto.setDescription("Use The Fuses Configuration")
	drvExtPhyKsz8061ConfigAuto.setDefaultValue(False)
	
	# External PHY Type
	drvExtPhyKsz8061PhyType = drvExtPhyKsz8061Component.createStringSymbol("TCPIP_EMAC_PHY_TYPE", None)
	drvExtPhyKsz8061PhyType.setVisible(False)	
	drvExtPhyKsz8061PhyType.setDefaultValue("KSZ8061")
	

	# Driver PHY Instances Number
	drvExtPhyKsz8061InstanceNum= drvExtPhyKsz8061Component.createIntegerSymbol("DRV_ETHPHY_INSTANCES_NUMBER", None)
	drvExtPhyKsz8061InstanceNum.setLabel("PHY Instances Number") 
	drvExtPhyKsz8061InstanceNum.setVisible(True)
	drvExtPhyKsz8061InstanceNum.setDescription("Driver PHY Instances Number")
	drvExtPhyKsz8061InstanceNum.setDefaultValue(1)
	
	# Driver PHY Clients Number
	drvExtPhyKsz8061ClientNum= drvExtPhyKsz8061Component.createIntegerSymbol("DRV_ETHPHY_CLIENTS_NUMBER", None)
	drvExtPhyKsz8061ClientNum.setLabel("PHY Clients Number") 
	drvExtPhyKsz8061ClientNum.setVisible(True)
	drvExtPhyKsz8061ClientNum.setDescription("Driver PHY Clients Number")
	drvExtPhyKsz8061ClientNum.setDefaultValue(1)
	
	# Driver PHY Peripheral Index Number
	drvExtPhyKsz8061IndexNum= drvExtPhyKsz8061Component.createIntegerSymbol("DRV_ETHPHY_INDEX", None)
	drvExtPhyKsz8061IndexNum.setLabel("PHY Peripheral Index Number") 
	drvExtPhyKsz8061IndexNum.setVisible(True)
	drvExtPhyKsz8061IndexNum.setDescription("Driver PHY Peripheral Index Number")
	drvExtPhyKsz8061IndexNum.setDefaultValue(1)
	
	# Driver PHY Peripheral ID
	drvExtPhyKsz8061PeripheralId= drvExtPhyKsz8061Component.createIntegerSymbol("DRV_ETHPHY_PERIPHERAL_ID", None)
	drvExtPhyKsz8061PeripheralId.setLabel("PHY Peripheral ID") 
	drvExtPhyKsz8061PeripheralId.setVisible(True)
	drvExtPhyKsz8061PeripheralId.setDescription("Driver PHY Peripheral ID")
	drvExtPhyKsz8061PeripheralId.setDefaultValue(1)
	
	# Driver PHY Negotiation Time-out - ms
	drvExtPhyKsz8061NegInitTimeout= drvExtPhyKsz8061Component.createIntegerSymbol("DRV_ETHPHY_NEG_INIT_TMO", None)
	drvExtPhyKsz8061NegInitTimeout.setLabel("PHY Negotiation Time-out - ms") 
	drvExtPhyKsz8061NegInitTimeout.setVisible(True)
	drvExtPhyKsz8061NegInitTimeout.setDescription("Driver PHY Negotiation Time-out - ms")
	drvExtPhyKsz8061NegInitTimeout.setDefaultValue(1)
	
	# Driver PHY Negotiation Done Time-out - ms
	drvExtPhyKsz8061NegDoneTimeout= drvExtPhyKsz8061Component.createIntegerSymbol("DRV_ETHPHY_NEG_DONE_TMO", None)
	drvExtPhyKsz8061NegDoneTimeout.setLabel("PHY Negotiation Done Time-out - ms") 
	drvExtPhyKsz8061NegDoneTimeout.setVisible(True)
	drvExtPhyKsz8061NegDoneTimeout.setDescription("Driver PHY Negotiation Done Time-out - ms")
	drvExtPhyKsz8061NegDoneTimeout.setDefaultValue(2000)
	
	# Driver PHY Reset Clear Time-out - ms
	drvExtPhyKsz8061ResetClearTimeout= drvExtPhyKsz8061Component.createIntegerSymbol("DRV_ETHPHY_RESET_CLR_TMO", None)
	drvExtPhyKsz8061ResetClearTimeout.setLabel("PHY Reset Clear Time-out - ms") 
	drvExtPhyKsz8061ResetClearTimeout.setVisible(True)
	drvExtPhyKsz8061ResetClearTimeout.setDescription("Driver PHY Reset Clear Time-out - ms")
	drvExtPhyKsz8061ResetClearTimeout.setDefaultValue(500)
	
	# Use a Function to be called at PHY Reset
	drvExtPhyKsz8061ResetCallbackEnable = drvExtPhyKsz8061Component.createBooleanSymbol("DRV_ETHPHY_USE_RESET_CALLBACK", None)
	drvExtPhyKsz8061ResetCallbackEnable.setLabel("Use a Function to be called at PHY Reset")
	drvExtPhyKsz8061ResetCallbackEnable.setVisible(True)
	drvExtPhyKsz8061ResetCallbackEnable.setDescription("Use a Function to be called at PHY Reset")
	drvExtPhyKsz8061ResetCallbackEnable.setDefaultValue(False)
	
	# App Function
	drvExtPhyKsz8061ResetCallback = drvExtPhyKsz8061Component.createStringSymbol("DRV_ETHPHY_RESET_CALLBACK", drvExtPhyKsz8061ResetCallbackEnable)
	drvExtPhyKsz8061ResetCallback.setLabel("App Function")
	drvExtPhyKsz8061ResetCallback.setVisible(False)
	drvExtPhyKsz8061ResetCallback.setDescription("App Function")
	drvExtPhyKsz8061ResetCallback.setDefaultValue("AppPhyResetFunction")
	drvExtPhyKsz8061ResetCallback.setDependencies(drvExtPhyKsz8061MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])

	#Add to system_config.h
	drvExtPhyKsz8061HeaderFtl = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	drvExtPhyKsz8061HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_ksz8061.h.ftl")
	drvExtPhyKsz8061HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	drvExtPhyKsz8061HeaderFtl.setMarkup(True)
	drvExtPhyKsz8061HeaderFtl.setType("STRING")
	
	# add "<#include \"/framework/tcpip/config/tcpip_stack.c.ftl\">"  to list SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA
	drvExtPhyKsz8061SourceFtl = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	drvExtPhyKsz8061SourceFtl.setType("STRING")
	drvExtPhyKsz8061SourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
	drvExtPhyKsz8061SourceFtl.setSourcePath("driver/ethphy/config/drv_extphy_ksz8061.c.ftl")
	drvExtPhyKsz8061SourceFtl.setMarkup(True)
	
	# file TCPIP_ETH_PHY_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/drv_ethphy.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethphy/drv_ethphy.h"
	# Add drv_ethphy.h file to project
	drvExtPhyHeaderFile = drvExtPhyKsz8061Component.createFileSymbol(None, None)
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
	drvExtPhyLocalHeaderFile = drvExtPhyKsz8061Component.createFileSymbol(None, None)
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
	drvExtPhyKsz8061RegHeaderFile = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	drvExtPhyKsz8061RegHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_regs.h")
	drvExtPhyKsz8061RegHeaderFile.setOutputName("drv_extphy_regs.h")
	drvExtPhyKsz8061RegHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhyKsz8061RegHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhyKsz8061RegHeaderFile.setType("HEADER")
	drvExtPhyKsz8061RegHeaderFile.setOverwrite(True)
	drvExtPhyKsz8061RegHeaderFile.setEnabled(True)
	
	# Add drv_extphy_ksz8061.h file to project
	drvExtPhyKsz8061HeaderFile = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	drvExtPhyKsz8061HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz8061.h")
	drvExtPhyKsz8061HeaderFile.setOutputName("drv_extphy_ksz8061.h")
	drvExtPhyKsz8061HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhyKsz8061HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhyKsz8061HeaderFile.setType("HEADER")
	drvExtPhyKsz8061HeaderFile.setOverwrite(True)
	drvExtPhyKsz8061HeaderFile.setEnabled(True)



	# file TCPIP_ETH_PHY_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_ethphy.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_ethphy.c"
	# Add drv_ethphy.c file
	drvExtPhySourceFile = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	drvExtPhySourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_ethphy.c")
	drvExtPhySourceFile.setOutputName("drv_ethphy.c")
	drvExtPhySourceFile.setOverwrite(True)
	drvExtPhySourceFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhySourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhySourceFile.setType("SOURCE")
	drvExtPhySourceFile.setEnabled(True)

	# Add drv_extphy_ksz8061.c file
	drvExtPhyKsz8061SourceFile = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	drvExtPhyKsz8061SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz8061.c")
	drvExtPhyKsz8061SourceFile.setOutputName("drv_extphy_ksz8061.c")
	drvExtPhyKsz8061SourceFile.setOverwrite(True)
	drvExtPhyKsz8061SourceFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhyKsz8061SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhyKsz8061SourceFile.setType("SOURCE")
	drvExtPhyKsz8061SourceFile.setEnabled(True)

	
def drvExtPhyKsz8061MenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("EthMac Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("EthMac Menu Invisible.")
		symbol.setVisible(False)
		