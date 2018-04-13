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
	#drvExtPhyKsz8061PhyType.setLabel("External PHY Type")
	drvExtPhyKsz8061PhyType.setVisible(False)# Niyas to check the visibility 
	#tcpipEmacPhyType.setDescription("External PHY Type")
	drvExtPhyKsz8061PhyType.setDefaultValue("KSZ8061")
	#tcpipEmacPhyType.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

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
	drvExtPhyKsz8061ResetCallback.setVisible(False)# Niyas to check the visibility 
	drvExtPhyKsz8061ResetCallback.setDescription("App Function")
	drvExtPhyKsz8061ResetCallback.setDefaultValue("AppPhyResetFunction")
	drvExtPhyKsz8061ResetCallback.setDependencies(drvExtPhyKsz8061MenuVisibleSingle, ["DRV_ETHPHY_USE_RESET_CALLBACK"])

	#Add to system_config.h
	drvExtPhyKsz8061HeaderFtl = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	drvExtPhyKsz8061HeaderFtl.setSourcePath("driver/ethphy/config/drv_extphy_ksz8061.h.ftl")
	drvExtPhyKsz8061HeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	drvExtPhyKsz8061HeaderFtl.setMarkup(True)
	drvExtPhyKsz8061HeaderFtl.setType("STRING")
	
	#niyas to do
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
	#drvExtPhyKsz8061RegHeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])    
		
	# file TCPIP_MAC_PHY_DP83640_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_dp83640.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_dp83640.h"
	# Add drv_extphy_dp83640.h file to project
	# drvExtPhyDp83640HeaderFile = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	# drvExtPhyDp83640HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_dp83640.h")
	# drvExtPhyDp83640HeaderFile.setOutputName("drv_extphy_dp83640.h")
	# drvExtPhyDp83640HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
	# drvExtPhyDp83640HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	# drvExtPhyDp83640HeaderFile.setType("HEADER")
	# drvExtPhyDp83640HeaderFile.setOverwrite(True)
	# drvExtPhyDp83640HeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])  

	# file TCPIP_MAC_PHY_DP83848_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_dp83848.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_dp83848.h"
	# Add drv_extphy_dp83848.h file to project
	# drvExtPhyDp83848HeaderFile = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	# drvExtPhyDp83848HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_dp83848.h")
	# drvExtPhyDp83848HeaderFile.setOutputName("drv_extphy_dp83848.h")
	# drvExtPhyDp83848HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
	# drvExtPhyDp83848HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	# drvExtPhyDp83848HeaderFile.setType("HEADER")
	# drvExtPhyDp83848HeaderFile.setOverwrite(True)
	# drvExtPhyDp83848HeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])  

	# file TCPIP_MAC_PHY_IP101GR_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_ip101gr.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_ip101gr.h"
	# Add drv_extphy_ip101gr.h file to project
	# drvExtPhyIp101grHeaderFile = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	# drvExtPhyIp101grHeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ip101gr.h")
	# drvExtPhyIp101grHeaderFile.setOutputName("drv_extphy_ip101gr.h")
	# drvExtPhyIp101grHeaderFile.setDestPath("driver/ethphy/src/dynamic/")
	# drvExtPhyIp101grHeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	# drvExtPhyIp101grHeaderFile.setType("HEADER")
	# drvExtPhyIp101grHeaderFile.setOverwrite(True)
	# drvExtPhyIp101grHeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])  

	# file TCPIP_MAC_PHY_LAN8700_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8700.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8700.h"
	# Add drv_extphy_smsc8700.h file to project
	# drvExtPhySmsc8700HeaderFile = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	# drvExtPhySmsc8700HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc8700.h")
	# drvExtPhySmsc8700HeaderFile.setOutputName("drv_extphy_smsc8700.h")
	# drvExtPhySmsc8700HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
	# drvExtPhySmsc8700HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	# drvExtPhySmsc8700HeaderFile.setType("HEADER")
	# drvExtPhySmsc8700HeaderFile.setOverwrite(True)
	# drvExtPhySmsc8700HeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])  

	# file TCPIP_MAC_PHY_LAN8720_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8720.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8720.h"
	# Add drv_extphy_smsc8720.h file to project
	# drvExtPhySmsc8720HeaderFile = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	# drvExtPhySmsc8720HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc8720.h")
	# drvExtPhySmsc8720HeaderFile.setOutputName("drv_extphy_smsc8720.h")
	# drvExtPhySmsc8720HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
	# drvExtPhySmsc8720HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	# drvExtPhySmsc8720HeaderFile.setType("HEADER")
	# drvExtPhySmsc8720HeaderFile.setOverwrite(True)
	# drvExtPhySmsc8720HeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])  

	# file TCPIP_MAC_PHY_LAN8740_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8740.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8740.h"
	# Add drv_extphy_smsc8740.h file to project
	# drvExtPhySmsc8740HeaderFile = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	# drvExtPhySmsc8740HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc8740.h")
	# drvExtPhySmsc8740HeaderFile.setOutputName("drv_extphy_smsc8740.h")
	# drvExtPhySmsc8740HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
	# drvExtPhySmsc8740HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	# drvExtPhySmsc8740HeaderFile.setType("HEADER")
	# drvExtPhySmsc8740HeaderFile.setOverwrite(True)
	# drvExtPhySmsc8740HeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])  


	# file TCPIP_MAC_PHY_LAN9303_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_smsc9303.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_smsc9303.h"
	# Add drv_extphy_smsc9303.h file to project
	# drvExtPhySmsc9303HeaderFile = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	# drvExtPhySmsc9303HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc9303.h")
	# drvExtPhySmsc9303HeaderFile.setOutputName("drv_extphy_smsc9303.h")
	# drvExtPhySmsc9303HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
	# drvExtPhySmsc9303HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	# drvExtPhySmsc9303HeaderFile.setType("HEADER")
	# drvExtPhySmsc9303HeaderFile.setOverwrite(True)
	# drvExtPhySmsc9303HeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])

	# file TCPIP_MAC_PHY_KSZ8061_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_ksz8061.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_ksz8061.h"
	# Add drv_extphy_ksz8061.h file to project
	drvExtPhyKsz8061HeaderFile = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	drvExtPhyKsz8061HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz8061.h")
	drvExtPhyKsz8061HeaderFile.setOutputName("drv_extphy_ksz8061.h")
	drvExtPhyKsz8061HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhyKsz8061HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhyKsz8061HeaderFile.setType("HEADER")
	drvExtPhyKsz8061HeaderFile.setOverwrite(True)
	drvExtPhyKsz8061HeaderFile.setEnabled(True)
	#drvExtPhyKsz8061HeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])

	# file TCPIP_MAC_PHY_KSZ8041_H "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_ksz8041.h" to "$PROJECT_HEADER_FILES/framework/driver/ethphy/src/dynamic/drv_extphy_ksz8041.h"
	# Add drv_extphy_ksz8041.h file to project
	# drvExtPhyKsz8041HeaderFile = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	# drvExtPhyKsz8041HeaderFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz8041.h")
	# drvExtPhyKsz8041HeaderFile.setOutputName("drv_extphy_ksz8041.h")
	# drvExtPhyKsz8041HeaderFile.setDestPath("driver/ethphy/src/dynamic/")
	# drvExtPhyKsz8041HeaderFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	# drvExtPhyKsz8041HeaderFile.setType("HEADER")
	# drvExtPhyKsz8041HeaderFile.setOverwrite(True)
	# drvExtPhyKsz8041HeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])




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
	#drvExtPhySourceFile.setDependencies(tcpipGmacGenSourceFile, ["TCPIP_USE_ETH_MAC"])

	# ifblock TCPIP_USE_ETH_MAC




	# ifblock TCPIP_EMAC_PHY_TYPE = "National_DP83640"
	# file TCPIP_MAC_PHY_DP83640_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_dp83640.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_dp83640.c"
	# endif
	# Add drv_extphy_dp83640.c file
	# drvExtPhyDp83640SourceFile = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	# drvExtPhyDp83640SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_dp83640.c")
	# drvExtPhyDp83640SourceFile.setOutputName("drv_extphy_dp83640.c")
	# drvExtPhyDp83640SourceFile.setOverwrite(True)
	# drvExtPhyDp83640SourceFile.setDestPath("driver/ethphy/src/dynamic/")
	# drvExtPhyDp83640SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	# drvExtPhyDp83640SourceFile.setType("SOURCE")
	# drvExtPhyDp83640SourceFile.setEnabled(False)
	# drvExtPhyDp83640SourceFile.setDependencies(drvExtPhyDp83640GenSourceFile, ["TCPIP_USE_ETH_MAC","TCPIP_EMAC_PHY_TYPE"])


	# ifblock TCPIP_EMAC_PHY_TYPE = "National_DP83848"
	# file TCPIP_MAC_PHY_DP83848_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_dp83848.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_dp83848.c"
	# endif
	# Add drv_extphy_dp83848.c file
	# drvExtPhyDp83848SourceFile = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	# drvExtPhyDp83848SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_dp83848.c")
	# drvExtPhyDp83848SourceFile.setOutputName("drv_extphy_dp83848.c")
	# drvExtPhyDp83848SourceFile.setOverwrite(True)
	# drvExtPhyDp83848SourceFile.setDestPath("driver/ethphy/src/dynamic/")
	# drvExtPhyDp83848SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	# drvExtPhyDp83848SourceFile.setType("SOURCE")
	# drvExtPhyDp83848SourceFile.setEnabled(False)
	# drvExtPhyDp83848SourceFile.setDependencies(drvExtPhyDp83848GenSourceFile, ["TCPIP_USE_ETH_MAC","TCPIP_EMAC_PHY_TYPE"])

	# ifblock TCPIP_EMAC_PHY_TYPE = "IP_IP101GR"
	# file TCPIP_MAC_PHY_IP101GR_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_ip101gr.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_ip101gr.c"
	# endif
	# Add drv_extphy_ip101gr.c file
	# drvExtPhyIp101grSourceFile = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	# drvExtPhyIp101grSourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ip101gr.c")
	# drvExtPhyIp101grSourceFile.setOutputName("drv_extphy_ip101gr.c")
	# drvExtPhyIp101grSourceFile.setOverwrite(True)
	# drvExtPhyIp101grSourceFile.setDestPath("driver/ethphy/src/dynamic/")
	# drvExtPhyIp101grSourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	# drvExtPhyIp101grSourceFile.setType("SOURCE")
	# drvExtPhyIp101grSourceFile.setEnabled(False)
	# drvExtPhyIp101grSourceFile.setDependencies(drvExtPhyIp101grGenSourceFile, ["TCPIP_USE_ETH_MAC","TCPIP_EMAC_PHY_TYPE"])

	# ifblock TCPIP_EMAC_PHY_TYPE = "SMSC_LAN8700"
	# file TCPIP_MAC_PHY_LAN8700_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8700.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_smsc8700.c"
	# endif
	# Add drv_extphy_smsc8700.c file
	# drvExtPhySmscLan8700SourceFile = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	# drvExtPhySmscLan8700SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc8700.c")
	# drvExtPhySmscLan8700SourceFile.setOutputName("drv_extphy_smsc8700.c")
	# drvExtPhySmscLan8700SourceFile.setOverwrite(True)
	# drvExtPhySmscLan8700SourceFile.setDestPath("driver/ethphy/src/dynamic/")
	# drvExtPhySmscLan8700SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	# drvExtPhySmscLan8700SourceFile.setType("SOURCE")
	# drvExtPhySmscLan8700SourceFile.setEnabled(False)
	# drvExtPhySmscLan8700SourceFile.setDependencies(drvExtPhySmscLan8700GenSourceFile, ["TCPIP_USE_ETH_MAC","TCPIP_EMAC_PHY_TYPE"])

	# ifblock TCPIP_EMAC_PHY_TYPE = "SMSC_LAN8720"
	# file TCPIP_MAC_PHY_LAN8720_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8720.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_smsc8720.c"
	# endif
	# Add drv_extphy_smsc8720.c file
	# drvExtPhySmscLan8720SourceFile = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	# drvExtPhySmscLan8720SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc8720.c")
	# drvExtPhySmscLan8720SourceFile.setOutputName("drv_extphy_smsc8720.c")
	# drvExtPhySmscLan8720SourceFile.setOverwrite(True)
	# drvExtPhySmscLan8720SourceFile.setDestPath("driver/ethphy/src/dynamic/")
	# drvExtPhySmscLan8720SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	# drvExtPhySmscLan8720SourceFile.setType("SOURCE")
	# drvExtPhySmscLan8720SourceFile.setEnabled(False)
	# drvExtPhySmscLan8720SourceFile.setDependencies(drvExtPhySmscLan8720GenSourceFile, ["TCPIP_USE_ETH_MAC","TCPIP_EMAC_PHY_TYPE"])


	# ifblock TCPIP_EMAC_PHY_TYPE = "SMSC_LAN8740"
	# file TCPIP_MAC_PHY_LAN8740_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_smsc8740.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_smsc8740.c"
	# endif
	# Add drv_extphy_smsc8740.c file
	# drvExtPhySmscLan8740SourceFile = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	# drvExtPhySmscLan8740SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc8740.c")
	# drvExtPhySmscLan8740SourceFile.setOutputName("drv_extphy_smsc8740.c")
	# drvExtPhySmscLan8740SourceFile.setOverwrite(True)
	# drvExtPhySmscLan8740SourceFile.setDestPath("driver/ethphy/src/dynamic/")
	# drvExtPhySmscLan8740SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	# drvExtPhySmscLan8740SourceFile.setType("SOURCE")
	# drvExtPhySmscLan8740SourceFile.setEnabled(False)
	# drvExtPhySmscLan8740SourceFile.setDependencies(drvExtPhySmscLan8740GenSourceFile, ["TCPIP_USE_ETH_MAC","TCPIP_EMAC_PHY_TYPE"])

	# ifblock TCPIP_EMAC_PHY_TYPE = "KSZ8061"
	# file TCPIP_MAC_PHY_KSZ8061_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_ksz8061.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_ksz8061.c"
	# endif
	# Add drv_extphy_ksz8061.c file
	drvExtPhyKsz8061SourceFile = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	drvExtPhyKsz8061SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz8061.c")
	drvExtPhyKsz8061SourceFile.setOutputName("drv_extphy_ksz8061.c")
	drvExtPhyKsz8061SourceFile.setOverwrite(True)
	drvExtPhyKsz8061SourceFile.setDestPath("driver/ethphy/src/dynamic/")
	drvExtPhyKsz8061SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	drvExtPhyKsz8061SourceFile.setType("SOURCE")
	drvExtPhyKsz8061SourceFile.setEnabled(True)
	#drvExtPhyKsz8061SourceFile.setDependencies(drvExtPhyKsz8061GenSourceFile, ["TCPIP_USE_ETH_MAC","TCPIP_EMAC_PHY_TYPE"])
	# ifblock TCPIP_EMAC_PHY_TYPE = "KSZ8041"
	# file TCPIP_MAC_PHY_KSZ8041_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_ksz8041.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_ksz8041.c"
	# endif
	# Add drv_extphy_ksz8041.c file
	# drvExtPhyKsz8041SourceFile = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	# drvExtPhyKsz8041SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_ksz8041.c")
	# drvExtPhyKsz8041SourceFile.setOutputName("drv_extphy_ksz8041.c")
	# drvExtPhyKsz8041SourceFile.setOverwrite(True)
	# drvExtPhyKsz8041SourceFile.setDestPath("driver/ethphy/src/dynamic/")
	# drvExtPhyKsz8041SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	# drvExtPhyKsz8041SourceFile.setType("SOURCE")
	# drvExtPhyKsz8041SourceFile.setEnabled(False)
	# drvExtPhyKsz8041SourceFile.setDependencies(drvExtPhyKsz8041GenSourceFile, ["TCPIP_USE_ETH_MAC","TCPIP_EMAC_PHY_TYPE"])

	# ifblock TCPIP_EMAC_PHY_TYPE = "SMSC_LAN9303"
	# file TCPIP_MAC_PHY_LAN9303_C "$HARMONY_VERSION_PATH/framework/driver/ethphy/src/dynamic/drv_extphy_smsc9303.c" to "$PROJECT_SOURCE_FILES/framework/driver/ethphy/drv_extphy_smsc9303.c"
	# endif
	# Add drv_extphy_smsc9303.c file
	# drvExtPhySmscLan9303SourceFile = drvExtPhyKsz8061Component.createFileSymbol(None, None)
	# drvExtPhySmscLan9303SourceFile.setSourcePath("driver/ethphy/src/dynamic/drv_extphy_smsc9303.c")
	# drvExtPhySmscLan9303SourceFile.setOutputName("drv_extphy_smsc9303.c")
	# drvExtPhySmscLan9303SourceFile.setOverwrite(True)
	# drvExtPhySmscLan9303SourceFile.setDestPath("driver/ethphy/src/dynamic/")
	# drvExtPhySmscLan9303SourceFile.setProjectPath("config/" + configName + "/driver/ethphy/src/dynamic/")
	# drvExtPhySmscLan9303SourceFile.setType("SOURCE")
	# drvExtPhySmscLan9303SourceFile.setEnabled(False)
	# drvExtPhySmscLan9303SourceFile.setDependencies(drvExtPhySmscLan9303GenSourceFile, ["TCPIP_USE_ETH_MAC","TCPIP_EMAC_PHY_TYPE"])	
	
def drvExtPhyKsz8061MenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("EthMac Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("EthMac Menu Invisible.")
		symbol.setVisible(False)
		
	

# def onDependentComponentAdded(drvExtPhyKsz8061Component, id, remoteComponent):
	
	# if id == "KSZ8061_MIIM_Dependency":
		# print("test niyas")
		# #tcpipNetConfigIndex = int(tcpipNetConfigComponent.getID().strip("tcpipNetConfig_"))
		# #MiimInterface = drvMiimComponent.getSymbolByID("DRV_MIIM_ETH_MODULE_ID")
		# print(remoteComponent.getDisplayName())
		# #print(remoteComponent.getID().DRV_MIIM_ETH_MODULE_ID)	
		# Database.setSymbolValue("drvMiim", "DRV_MIIM_ETH_MODULE_ID", "test", 2)
		# #print(remoteComponent.getSymbolByID("DRV_MIIM_ETH_MODULE_ID"))
		# #drvMiimModuleId.clearValue()
		# #print(tcpipNetConfigIndex)
		# #drvMiimModuleId.setValue("GMAC_ID_0", 2)