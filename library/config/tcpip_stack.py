
def instantiateComponent(tcpipStackComponent):
	print("TCPIP main Stack Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	
	# TCP/IP Stack Enable 
	tcpipStackEnable = tcpipStackComponent.createBooleanSymbol("USE_TCPIP_STACK", None)
	tcpipStackEnable.setLabel("Use TCP/IP Stack?")
	tcpipStackEnable.setDescription("Enables TCP/IP stack")
	tcpipStackEnable.setReadOnly(True)
	tcpipStackEnable.setDefaultValue(True)
	tcpipStackEnable.setVisible(False)
	
	# Define number of network Configurations
	tcpipStackTickRate = tcpipStackComponent.createIntegerSymbol("TCPIP_STACK_TICK_RATE", None)
	tcpipStackTickRate.setLabel("TCP/IP Stack State Machine Tick Rate, ms.")
	tcpipStackTickRate.setVisible(True)
	tcpipStackTickRate.setDescription("TCP/IP Stack State Machine Tick Rate")
	tcpipStackTickRate.setDefaultValue(5)
	
	# Event Notification
	tcpipStackEventNotify = tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_EVENT_NOTIFICATION", None)
	tcpipStackEventNotify.setVisible(False)
	tcpipStackEventNotify.setDefaultValue(True)
	
	# Enable User Notification
	tcpipStackUserNotify= tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_USER_NOTIFICATION", None)
	tcpipStackUserNotify.setLabel("Enable User Notification")
	tcpipStackUserNotify.setVisible(True)
	tcpipStackUserNotify.setDescription("Enable User Notification")
	tcpipStackUserNotify.setDefaultValue(False)
	tcpipStackUserNotify.setDependencies(tcpipStackMenuVisible, ["TCPIP_STACK_EVENT_NOTIFICATION"])

	# Enable Deinitialize of TCP/IP stack
	tcpipStackDown= tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_DOWN_OPERATION", None)
	tcpipStackDown.setLabel("Enable Stack Deinitialize Operations")
	tcpipStackDown.setVisible(True)
	tcpipStackDown.setDescription("Enable Deinitialize of TCP/IP stack")
	tcpipStackDown.setDefaultValue(True)
	
	# Enable stack up/down of TCP/IP stack
	tcpipStackIfUpDown= tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_IF_UP_DOWN_OPERATION", tcpipStackDown)
	tcpipStackIfUpDown.setLabel("Enable IF Up/Down Operations")
	tcpipStackIfUpDown.setVisible(True)
	tcpipStackIfUpDown.setDescription("Enable interface up/down of TCP/IP stack")
	tcpipStackIfUpDown.setDefaultValue(True)
	tcpipStackIfUpDown.setDependencies(tcpipStackMenuVisible, ["TCPIP_STACK_DOWN_OPERATION"])

	# Enable stack Mac Down
	tcpipStackMacDown= tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_MAC_DOWN_OPERATION", None)
	tcpipStackMacDown.setLabel("Enable MAC Down on Failure")
	tcpipStackMacDown.setVisible(False)
	tcpipStackMacDown.setDescription("Enable MAC Down on Failure")
	tcpipStackMacDown.setDefaultValue(True)
	tcpipStackMacDown.setDependencies(tcpipStackMenuInvisible, ["TCPIP_STACK_DOWN_OPERATION"])

	# Enable Interface Address Change Signaling
	tcpipStackIfChange= tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_INTERFACE_CHANGE_SIGNALING", None)
	tcpipStackIfChange.setLabel("Enable Interface Address Change Signaling")
	tcpipStackIfChange.setVisible(True)
	tcpipStackIfChange.setDescription("Enable Interface Address Change Signaling")
	tcpipStackIfChange.setDefaultValue(False)
	tcpipStackIfChange.setDependencies(tcpipStackMenuVisible, ["tcpipIPv4.TCPIP_STACK_USE_IPV4"])

	# Enable Configuration Save/Restore Functionality
	tcpipStackConfigSaveRestore= tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_CONFIGURATION_SAVE_RESTORE", None)
	tcpipStackConfigSaveRestore.setLabel("Enable Configuration Save/Restore Functionality")
	tcpipStackConfigSaveRestore.setVisible(True)
	tcpipStackConfigSaveRestore.setDescription("Enable Configuration Save/Restore Functionality")
	tcpipStackConfigSaveRestore.setDefaultValue(True)	
		
	# Enable File System Wrapper
	#config TCPIP_SYS_FS_WRAPPER_NEEDED. This logic implemented in tcpip_sysfs_wrapper.py

	# Define number of entries in the secure port table
	tcpipStackSecPortNum = tcpipStackComponent.createIntegerSymbol("TCPIP_STACK_SECURE_PORT_ENTRIES", None)
	tcpipStackSecPortNum.setLabel("Number of entries in the secure port table")
	tcpipStackSecPortNum.setVisible(True)
	tcpipStackSecPortNum.setDescription("Number of entries in the secure port table")
	tcpipStackSecPortNum.setDefaultValue(10)

	# Enable Alias Interfaces Support
	tcpipStackAliasInterface= tcpipStackComponent.createBooleanSymbol("TCPIP_STACK_ALIAS_INTERFACE_SUPPORT", None)
	tcpipStackAliasInterface.setLabel("Enable Alias Interfaces Support")
	tcpipStackAliasInterface.setVisible(True)
	tcpipStackAliasInterface.setDescription("Enable Alias Interfaces Support")
	tcpipStackAliasInterface.setDefaultValue(False)

	# Enable Packet Logger
	tcpipStackPktLogEnable= tcpipStackComponent.createBooleanSymbol("TCPIP_PACKET_LOG_ENABLE", None)
	tcpipStackPktLogEnable.setLabel("Enable Packet Logger")
	tcpipStackPktLogEnable.setVisible(True)
	tcpipStackPktLogEnable.setDescription("Enable Packet Logger")
	tcpipStackPktLogEnable.setDefaultValue(False)

	# Number of Entries in the Packet Logger
	tcpipStackPktLogSize= tcpipStackComponent.createIntegerSymbol("TCPIP_PKT_LOG_SIZE", tcpipStackPktLogEnable)
	tcpipStackPktLogSize.setLabel("Number of Entries in the Packet Logger")
	tcpipStackPktLogSize.setVisible(False)
	tcpipStackPktLogSize.setDescription("Number of Entries in the Packet Logger")
	tcpipStackPktLogSize.setDefaultValue(40)
	tcpipStackPktLogSize.setDependencies(tcpipStackMenuVisible, ["TCPIP_PACKET_LOG_ENABLE"])	

	tcpipStackDeviceFamily = tcpipStackComponent.createStringSymbol("TCPIP_DEVICE_FAMILY", None)
	tcpipStackDeviceFamily.setVisible(False)
	if "PIC32C" in Variables.get("__PROCESSOR"):
		tcpipStackDeviceFamily.setDefaultValue("PIC32C")
	else:
		tcpipStackDeviceFamily.setDefaultValue("PIC32M")
	
	#niyas  ifblock USE_TCPIP_STACK
	# add "<#include \"/framework/tcpip/config/tcpip_stack.h.ftl\">"  to list SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION 	
	tcpipStackHeaderFtl = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackHeaderFtl.setSourcePath("library/config/tcpip_stack.h.ftl")
	tcpipStackHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipStackHeaderFtl.setMarkup(True)
	tcpipStackHeaderFtl.setType("STRING")
	
	# file TCPIP_COMMON_HELPERS_H "$HARMONY_VERSION_PATH/framework/tcpip/src/common/helpers.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/common/helpers.h"
	tcpipStackHelpersHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackHelpersHeaderFile.setSourcePath("library/src/common/helpers.h")
	tcpipStackHelpersHeaderFile.setOutputName("helpers.h")
	tcpipStackHelpersHeaderFile.setDestPath("library/tcpip/src/common/")
	tcpipStackHelpersHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/common/")
	tcpipStackHelpersHeaderFile.setType("HEADER")
	tcpipStackHelpersHeaderFile.setOverwrite(True)
	
	# file TCPIP_HASH_FNV_H "$HARMONY_VERSION_PATH/framework/tcpip/src/hash_fnv.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/hash_fnv.h"
	tcpipStackHashFnvHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackHashFnvHeaderFile.setSourcePath("library/src/hash_fnv.h")
	tcpipStackHashFnvHeaderFile.setOutputName("hash_fnv.h")
	tcpipStackHashFnvHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackHashFnvHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipStackHashFnvHeaderFile.setType("HEADER")
	tcpipStackHashFnvHeaderFile.setOverwrite(True)
		
	# file TCPIP_LINKED_LIST_H "$HARMONY_VERSION_PATH/framework/tcpip/src/link_list.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/link_list.h"
	tcpipStackLinkListHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackLinkListHeaderFile.setSourcePath("library/src/link_list.h")
	tcpipStackLinkListHeaderFile.setOutputName("link_list.h")
	tcpipStackLinkListHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackLinkListHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipStackLinkListHeaderFile.setType("HEADER")
	tcpipStackLinkListHeaderFile.setOverwrite(True)
			
	# file TCPIP_OAHASH_H "$HARMONY_VERSION_PATH/framework/tcpip/src/oahash.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/oahash.h"
	tcpipStackOaHashHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackOaHashHeaderFile.setSourcePath("library/src/oahash.h")
	tcpipStackOaHashHeaderFile.setOutputName("oahash.h")
	tcpipStackOaHashHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackOaHashHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipStackOaHashHeaderFile.setType("HEADER")
	tcpipStackOaHashHeaderFile.setOverwrite(True)
			
	# file TCPIP_HELPERS_H "$HARMONY_VERSION_PATH/framework/tcpip/tcpip_helpers.h" to "$PROJECT_HEADER_FILES/framework/tcpip/tcpip_helpers.h"
	tcpipStackTcpipHelperHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipHelperHeaderFile.setSourcePath("library/tcpip_helpers.h")
	tcpipStackTcpipHelperHeaderFile.setOutputName("tcpip_helpers.h")
	tcpipStackTcpipHelperHeaderFile.setDestPath("library/tcpip/")
	tcpipStackTcpipHelperHeaderFile.setProjectPath("config/" + configName + "library/tcpip/")
	tcpipStackTcpipHelperHeaderFile.setType("HEADER")
	tcpipStackTcpipHelperHeaderFile.setOverwrite(True)
			
	# file TCPIP_HELPERS_PRIVATE_H "$HARMONY_VERSION_PATH/framework/tcpip/src/tcpip_helpers_private.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/tcpip_helpers_private.h"
	tcpipStackTcpipHelperPrivHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipHelperPrivHeaderFile.setSourcePath("library/src/tcpip_helpers_private.h")
	tcpipStackTcpipHelperPrivHeaderFile.setOutputName("tcpip_helpers_private.h")
	tcpipStackTcpipHelperPrivHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackTcpipHelperPrivHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipStackTcpipHelperPrivHeaderFile.setType("HEADER")
	tcpipStackTcpipHelperPrivHeaderFile.setOverwrite(True)
		
	# file TCPIP_MANAGER_H "$HARMONY_VERSION_PATH/framework/tcpip/tcpip_manager.h" to "$PROJECT_HEADER_FILES/framework/tcpip/tcpip_manager.h"
	tcpipStackTcpipMngrHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipMngrHeaderFile.setSourcePath("library/tcpip_manager.h")
	tcpipStackTcpipMngrHeaderFile.setOutputName("tcpip_manager.h")
	tcpipStackTcpipMngrHeaderFile.setDestPath("library/tcpip/")
	tcpipStackTcpipMngrHeaderFile.setProjectPath("config/" + configName + "library/tcpip/")
	tcpipStackTcpipMngrHeaderFile.setType("HEADER")
	tcpipStackTcpipMngrHeaderFile.setOverwrite(True)
				
	# file TCPIP_MANAGER_CONTROL_H "$HARMONY_VERSION_PATH/framework/tcpip/src/tcpip_manager_control.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/tcpip_manager_control.h"
	tcpipStackTcpipMngrCtrlHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipMngrCtrlHeaderFile.setSourcePath("library/src/tcpip_manager_control.h")
	tcpipStackTcpipMngrCtrlHeaderFile.setOutputName("tcpip_manager_control.h")
	tcpipStackTcpipMngrCtrlHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackTcpipMngrCtrlHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipStackTcpipMngrCtrlHeaderFile.setType("HEADER")
	tcpipStackTcpipMngrCtrlHeaderFile.setOverwrite(True)
		
	# file TCPIP_MODULE_MANAGER_H "$HARMONY_VERSION_PATH/framework/tcpip/src/tcpip_module_manager.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/tcpip_module_manager.h"
	tcpipStackTcpipModlMngrHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipModlMngrHeaderFile.setSourcePath("library/src/tcpip_module_manager.h")
	tcpipStackTcpipModlMngrHeaderFile.setOutputName("tcpip_module_manager.h")
	tcpipStackTcpipModlMngrHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackTcpipModlMngrHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipStackTcpipModlMngrHeaderFile.setType("HEADER")
	tcpipStackTcpipModlMngrHeaderFile.setOverwrite(True)
		
	# file TCPIP_NOTIFY_H "$HARMONY_VERSION_PATH/framework/tcpip/src/tcpip_notify.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/tcpip_notify.h"
	tcpipStackTcpipNotfyHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipNotfyHeaderFile.setSourcePath("library/src/tcpip_notify.h")
	tcpipStackTcpipNotfyHeaderFile.setOutputName("tcpip_notify.h")
	tcpipStackTcpipNotfyHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackTcpipNotfyHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipStackTcpipNotfyHeaderFile.setType("HEADER")
	tcpipStackTcpipNotfyHeaderFile.setOverwrite(True)
	
	# file TCPIP_PACKET_H "$HARMONY_VERSION_PATH/framework/tcpip/src/tcpip_packet.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/tcpip_packet.h"
	tcpipStackTcpipPcktHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipPcktHeaderFile.setSourcePath("library/src/tcpip_packet.h")
	tcpipStackTcpipPcktHeaderFile.setOutputName("tcpip_packet.h")
	tcpipStackTcpipPcktHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackTcpipPcktHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipStackTcpipPcktHeaderFile.setType("HEADER")
	tcpipStackTcpipPcktHeaderFile.setOverwrite(True)
		
	# file TCPIP_PRIVATE_H "$HARMONY_VERSION_PATH/framework/tcpip/src/tcpip_private.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/tcpip_private.h"
	tcpipStackTcpipPrivHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipPrivHeaderFile.setSourcePath("library/src/tcpip_private.h")
	tcpipStackTcpipPrivHeaderFile.setOutputName("tcpip_private.h")
	tcpipStackTcpipPrivHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackTcpipPrivHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipStackTcpipPrivHeaderFile.setType("HEADER")
	tcpipStackTcpipPrivHeaderFile.setOverwrite(True)
			
	# file TCPIP_TYPES_H "$HARMONY_VERSION_PATH/framework/tcpip/src/tcpip_types.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/tcpip_types.h"
	tcpipStackTcpipTypeHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipTypeHeaderFile.setSourcePath("library/src/tcpip_types.h")
	tcpipStackTcpipTypeHeaderFile.setOutputName("tcpip_types.h")
	tcpipStackTcpipTypeHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackTcpipTypeHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipStackTcpipTypeHeaderFile.setType("HEADER")
	tcpipStackTcpipTypeHeaderFile.setOverwrite(True)
		
	# file TCPIP_H "$HARMONY_VERSION_PATH/framework/tcpip/tcpip.h" to "$PROJECT_HEADER_FILES/framework/tcpip/tcpip.h"
	tcpipStackTcpipHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipHeaderFile.setSourcePath("library/tcpip.h")
	tcpipStackTcpipHeaderFile.setOutputName("tcpip.h")
	tcpipStackTcpipHeaderFile.setDestPath("library/tcpip/")
	tcpipStackTcpipHeaderFile.setProjectPath("config/" + configName + "library/tcpip/")
	tcpipStackTcpipHeaderFile.setType("HEADER")
	tcpipStackTcpipHeaderFile.setOverwrite(True)
			
	# file DHCPV6_H "$HARMONY_VERSION_PATH/framework/tcpip/dhcpv6.h" to "$PROJECT_HEADER_FILES/framework/tcpip/dhcpv6.h"
	tcpipStackDhcpv6HeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackDhcpv6HeaderFile.setSourcePath("library/dhcpv6.h")
	tcpipStackDhcpv6HeaderFile.setOutputName("dhcpv6.h")
	tcpipStackDhcpv6HeaderFile.setDestPath("library/tcpip/")
	tcpipStackDhcpv6HeaderFile.setProjectPath("config/" + configName + "library/tcpip/")
	tcpipStackDhcpv6HeaderFile.setType("HEADER")
	tcpipStackDhcpv6HeaderFile.setOverwrite(True)
		
	# file LLDP_H "$HARMONY_VERSION_PATH/framework/tcpip/lldp.h" to "$PROJECT_HEADER_FILES/framework/tcpip/lldp.h"
	tcpipStackLldpHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackLldpHeaderFile.setSourcePath("library/lldp.h")
	tcpipStackLldpHeaderFile.setOutputName("lldp.h")
	tcpipStackLldpHeaderFile.setDestPath("library/tcpip/")
	tcpipStackLldpHeaderFile.setProjectPath("config/" + configName + "library/tcpip/")
	tcpipStackLldpHeaderFile.setType("HEADER")
	tcpipStackLldpHeaderFile.setOverwrite(True)
			
	# file BERKELEY_MANAGER_H "$HARMONY_VERSION_PATH/framework/tcpip/src/berkeley_manager.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/berkeley_manager.h"
	tcpipStackBrklyMngrHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackBrklyMngrHeaderFile.setSourcePath("library/src/berkeley_manager.h")
	tcpipStackBrklyMngrHeaderFile.setOutputName("berkeley_manager.h")
	tcpipStackBrklyMngrHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackBrklyMngrHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipStackBrklyMngrHeaderFile.setType("HEADER")
	tcpipStackBrklyMngrHeaderFile.setOverwrite(True)
				
	# file DHCPV6_MANAGER_H "$HARMONY_VERSION_PATH/framework/tcpip/src/dhcpv6_manager.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/dhcpv6_manager.h"
	tcpipStackDhcpv6MngrHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackDhcpv6MngrHeaderFile.setSourcePath("library/src/dhcpv6_manager.h")
	tcpipStackDhcpv6MngrHeaderFile.setOutputName("dhcpv6_manager.h")
	tcpipStackDhcpv6MngrHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackDhcpv6MngrHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipStackDhcpv6MngrHeaderFile.setType("HEADER")
	tcpipStackDhcpv6MngrHeaderFile.setOverwrite(True)
				
	# file DHCPV6_PRIVATE_H "$HARMONY_VERSION_PATH/framework/tcpip/src/dhcpv6_private.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/dhcpv6_private.h"
	tcpipStackDhcpv6PrivHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackDhcpv6PrivHeaderFile.setSourcePath("library/src/dhcpv6_private.h")
	tcpipStackDhcpv6PrivHeaderFile.setOutputName("dhcpv6_private.h")
	tcpipStackDhcpv6PrivHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackDhcpv6PrivHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipStackDhcpv6PrivHeaderFile.setType("HEADER")
	tcpipStackDhcpv6PrivHeaderFile.setOverwrite(True)
				
	# file LLDP_MANAGER_H "$HARMONY_VERSION_PATH/framework/tcpip/src/lldp_manager.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/lldp_manager.h"
	tcpipStackLldpMngrHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackLldpMngrHeaderFile.setSourcePath("library/src/lldp_manager.h")
	tcpipStackLldpMngrHeaderFile.setOutputName("lldp_manager.h")
	tcpipStackLldpMngrHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackLldpMngrHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipStackLldpMngrHeaderFile.setType("HEADER")
	tcpipStackLldpMngrHeaderFile.setOverwrite(True)
			
	# file LLDP_PRIVATE_H "$HARMONY_VERSION_PATH/framework/tcpip/src/lldp_private.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/lldp_private.h"
	tcpipStackLldpPrivHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackLldpPrivHeaderFile.setSourcePath("library/src/lldp_private.h")
	tcpipStackLldpPrivHeaderFile.setOutputName("lldp_private.h")
	tcpipStackLldpPrivHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackLldpPrivHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipStackLldpPrivHeaderFile.setType("HEADER")
	tcpipStackLldpPrivHeaderFile.setOverwrite(True)
			
	# file LLDP_TLV_H "$HARMONY_VERSION_PATH/framework/tcpip/src/lldp_tlv.h" to "$PROJECT_HEADER_FILES/framework/tcpip/src/lldp_tlv.h"
	tcpipStackLldpTlvHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackLldpTlvHeaderFile.setSourcePath("library/src/lldp_tlv.h")
	tcpipStackLldpTlvHeaderFile.setOutputName("lldp_tlv.h")
	tcpipStackLldpTlvHeaderFile.setDestPath("library/tcpip/src/")
	tcpipStackLldpTlvHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipStackLldpTlvHeaderFile.setType("HEADER")
	tcpipStackLldpTlvHeaderFile.setOverwrite(True)
			
	# file TCPIP_ANNOUNCE_H "$HARMONY_VERSION_PATH/framework/tcpip/tcpip_announce.h" to "$PROJECT_HEADER_FILES/framework/tcpip/tcpip_announce.h"
	tcpipStackAnnounceHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackAnnounceHeaderFile.setSourcePath("library/tcpip_announce.h")
	tcpipStackAnnounceHeaderFile.setOutputName("tcpip_announce.h")
	tcpipStackAnnounceHeaderFile.setDestPath("library/tcpip/")
	tcpipStackAnnounceHeaderFile.setProjectPath("config/" + configName + "library/tcpip/")
	tcpipStackAnnounceHeaderFile.setType("HEADER")
	tcpipStackAnnounceHeaderFile.setOverwrite(True)
		
	# file TCPIP_REBOOT_H "$HARMONY_VERSION_PATH/framework/tcpip/tcpip_reboot.h" to "$PROJECT_HEADER_FILES/framework/tcpip/tcpip_reboot.h"
	tcpipStackRebootHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackRebootHeaderFile.setSourcePath("library/tcpip_reboot.h")
	tcpipStackRebootHeaderFile.setOutputName("tcpip_reboot.h")
	tcpipStackRebootHeaderFile.setDestPath("library/tcpip/")
	tcpipStackRebootHeaderFile.setProjectPath("config/" + configName + "library/tcpip/")
	tcpipStackRebootHeaderFile.setType("HEADER")
	tcpipStackRebootHeaderFile.setOverwrite(True)
		
	# file TCPIP_COMMON_PORTS_H "$HARMONY_VERSION_PATH/framework/tcpip/tcpip_common_ports.h" to "$PROJECT_HEADER_FILES/framework/tcpip/tcpip_common_ports.h"
	tcpipStackCommonPortHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackCommonPortHeaderFile.setSourcePath("library/tcpip_common_ports.h")
	tcpipStackCommonPortHeaderFile.setOutputName("tcpip_common_ports.h")
	tcpipStackCommonPortHeaderFile.setDestPath("library/tcpip/")
	tcpipStackCommonPortHeaderFile.setProjectPath("config/" + configName + "library/tcpip/")
	tcpipStackCommonPortHeaderFile.setType("HEADER")
	tcpipStackCommonPortHeaderFile.setOverwrite(True)
		
	# file TCPIP_MAC_H "$HARMONY_VERSION_PATH/framework/tcpip/tcpip_mac.h" to                                     "$PROJECT_HEADER_FILES/framework/tcpip/tcpip_mac.h"
	tcpipStackTcpipMacHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipMacHeaderFile.setSourcePath("library/tcpip_mac.h")
	tcpipStackTcpipMacHeaderFile.setOutputName("tcpip_mac.h")
	tcpipStackTcpipMacHeaderFile.setDestPath("library/tcpip/")
	tcpipStackTcpipMacHeaderFile.setProjectPath("config/" + configName + "library/tcpip/")
	tcpipStackTcpipMacHeaderFile.setType("HEADER")
	tcpipStackTcpipMacHeaderFile.setOverwrite(True)
		
	# file TCPIP_MAC_OBJECT_H "$HARMONY_VERSION_PATH/framework/tcpip/tcpip_mac_object.h" to                       "$PROJECT_HEADER_FILES/framework/tcpip/tcpip_mac_object.h"
	tcpipStackTcpipMacObjHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipMacObjHeaderFile.setSourcePath("library/tcpip_mac_object.h")
	tcpipStackTcpipMacObjHeaderFile.setOutputName("tcpip_mac_object.h")
	tcpipStackTcpipMacObjHeaderFile.setDestPath("library/tcpip/")
	tcpipStackTcpipMacObjHeaderFile.setProjectPath("config/" + configName + "library/tcpip/")
	tcpipStackTcpipMacObjHeaderFile.setType("HEADER")
	tcpipStackTcpipMacObjHeaderFile.setOverwrite(True)
		
	# file TCPIP_ETHERNET_H "$HARMONY_VERSION_PATH/framework/tcpip/tcpip_ethernet.h" to                           "$PROJECT_HEADER_FILES/framework/tcpip/tcpip_ethernet.h"
	tcpipStackTcpipEthernetHeaderFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipEthernetHeaderFile.setSourcePath("library/tcpip_ethernet.h")
	tcpipStackTcpipEthernetHeaderFile.setOutputName("tcpip_ethernet.h")
	tcpipStackTcpipEthernetHeaderFile.setDestPath("library/tcpip/")
	tcpipStackTcpipEthernetHeaderFile.setProjectPath("config/" + configName + "library/tcpip/")
	tcpipStackTcpipEthernetHeaderFile.setType("HEADER")
	tcpipStackTcpipEthernetHeaderFile.setOverwrite(True)
	
	# #Add to system_config.h
	# tcpipTcpHeaderFtl = tcpipTcpComponent.createFileSymbol(None, None)
	# tcpipTcpHeaderFtl.setSourcePath("library/config/tcp.h.ftl")
	# tcpipTcpHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	# tcpipTcpHeaderFtl.setMarkup(True)
	# tcpipTcpHeaderFtl.setType("STRING")
	
	#niyas to do
	# add "<#include \"/framework/tcpip/config/tcpip_stack.c.ftl\">"  to list SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA
	tcpipStackSourceFtl = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackSourceFtl.setType("STRING")
	tcpipStackSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
	tcpipStackSourceFtl.setSourcePath("library/config/tcpip_stack.c.ftl")
	tcpipStackSourceFtl.setMarkup(True)
	
	# add "<#include \"/framework/tcpip/config/tcpip_stack_init.c.ftl\">"  to list SYSTEM_INIT_C_INITIALIZE_MIDDLEWARE
	tcpipStackInitSourceFtl = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackInitSourceFtl.setType("STRING")
	tcpipStackInitSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_INITIALIZE_MIDDLEWARE")
	tcpipStackInitSourceFtl.setSourcePath("library/config/tcpip_stack_init.c.ftl")
	tcpipStackInitSourceFtl.setMarkup(True)
		
	# add "<#include \"/framework/tcpip/config/tcpip_stack_tasks.c.ftl\">"  to list SYSTEM_TASKS_C_CALL_LIB_TASKS
	tcpipStackTaskSourceFtl = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTaskSourceFtl.setType("STRING")
	tcpipStackTaskSourceFtl.setOutputName("core.LIST_SYSTEM_TASKS_C_CALL_LIB_TASKS")
	tcpipStackTaskSourceFtl.setSourcePath("library/config/tcpip_stack_tasks.c.ftl")
	tcpipStackTaskSourceFtl.setMarkup(True)
		
	# file TCPIP_COMMON_HELPERS_C "$HARMONY_VERSION_PATH/framework/tcpip/src/common/helpers.c" to "$PROJECT_SOURCE_FILES/framework/tcpip/src/common/helpers.c"
	tcpipStackHelpersSourceFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackHelpersSourceFile.setSourcePath("library/src/common/helpers.c")
	tcpipStackHelpersSourceFile.setOutputName("helpers.c")
	tcpipStackHelpersSourceFile.setOverwrite(True)
	tcpipStackHelpersSourceFile.setDestPath("library/tcpip/src/")
	tcpipStackHelpersSourceFile.setProjectPath("config/" + configName + "library/src/common/")
	tcpipStackHelpersSourceFile.setType("SOURCE")
	tcpipStackHelpersSourceFile.setEnabled(True)	
	
	# file TCPIP_HASH_FNV_C "$HARMONY_VERSION_PATH/framework/tcpip/src/hash_fnv.c" to "$PROJECT_SOURCE_FILES/framework/tcpip/src/hash_fnv.c"
	tcpipStackHashFnvSourceFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackHashFnvSourceFile.setSourcePath("library/src/hash_fnv.c")
	tcpipStackHashFnvSourceFile.setOutputName("hash_fnv.c")
	tcpipStackHashFnvSourceFile.setOverwrite(True)
	tcpipStackHashFnvSourceFile.setDestPath("library/tcpip/src/")
	tcpipStackHashFnvSourceFile.setProjectPath("config/" + configName + "library/src/")
	tcpipStackHashFnvSourceFile.setType("SOURCE")
	tcpipStackHashFnvSourceFile.setEnabled(True)	
		
	# file TCPIP_OAHASH_C "$HARMONY_VERSION_PATH/framework/tcpip/src/oahash.c" to "$PROJECT_SOURCE_FILES/framework/tcpip/src/oahash.c"
	tcpipStackOaHashSourceFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackOaHashSourceFile.setSourcePath("library/src/oahash.c")
	tcpipStackOaHashSourceFile.setOutputName("oahash.c")
	tcpipStackOaHashSourceFile.setOverwrite(True)
	tcpipStackOaHashSourceFile.setDestPath("library/tcpip/src/")
	tcpipStackOaHashSourceFile.setProjectPath("config/" + configName + "library/src/")
	tcpipStackOaHashSourceFile.setType("SOURCE")
	tcpipStackOaHashSourceFile.setEnabled(True)	
			
	# file TCPIP_HELPERS_C "$HARMONY_VERSION_PATH/framework/tcpip/src/tcpip_helpers.c" to "$PROJECT_SOURCE_FILES/framework/tcpip/src/tcpip_helpers.c"
	tcpipStackTcpipHelpersSourceFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipHelpersSourceFile.setSourcePath("library/src/tcpip_helpers.c")
	tcpipStackTcpipHelpersSourceFile.setOutputName("tcpip_helpers.c")
	tcpipStackTcpipHelpersSourceFile.setOverwrite(True)
	tcpipStackTcpipHelpersSourceFile.setDestPath("library/tcpip/src/")
	tcpipStackTcpipHelpersSourceFile.setProjectPath("config/" + configName + "library/src/")
	tcpipStackTcpipHelpersSourceFile.setType("SOURCE")
	tcpipStackTcpipHelpersSourceFile.setEnabled(True)	
		
	# ifblock !DSTBDPIC32CZ
	# file TCPIP_HELPERS_C_32 "$HARMONY_VERSION_PATH/framework/tcpip/src/tcpip_helper_c32.S" to "$PROJECT_SOURCE_FILES/framework/tcpip/src/tcpip_helper_c32.S"
	# endif
	if "PIC32C" not in Variables.get("__PROCESSOR"):
		tcpipStackTcpipHelpersC32SourceFile = tcpipStackComponent.createFileSymbol(None, None)
		tcpipStackTcpipHelpersC32SourceFile.setSourcePath("library/src/tcpip_helper_c32.S")
		tcpipStackTcpipHelpersC32SourceFile.setOutputName("tcpip_helper_c32.S")
		tcpipStackTcpipHelpersC32SourceFile.setOverwrite(True)
		tcpipStackTcpipHelpersC32SourceFile.setDestPath("library/tcpip/src/")
		tcpipStackTcpipHelpersC32SourceFile.setProjectPath("config/" + configName + "library/src/")
		tcpipStackTcpipHelpersC32SourceFile.setType("SOURCE")
		tcpipStackTcpipHelpersC32SourceFile.setEnabled(True)	
			
	# file TCPIP_MANAGER_C "$HARMONY_VERSION_PATH/framework/tcpip/src/tcpip_manager.c" to "$PROJECT_SOURCE_FILES/framework/tcpip/src/tcpip_manager.c"
	tcpipStackTcpipMngrSourceFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipMngrSourceFile.setSourcePath("library/src/tcpip_manager.c")
	tcpipStackTcpipMngrSourceFile.setOutputName("tcpip_manager.c")
	tcpipStackTcpipMngrSourceFile.setOverwrite(True)
	tcpipStackTcpipMngrSourceFile.setDestPath("library/tcpip/src/")
	tcpipStackTcpipMngrSourceFile.setProjectPath("config/" + configName + "library/src/")
	tcpipStackTcpipMngrSourceFile.setType("SOURCE")
	tcpipStackTcpipMngrSourceFile.setEnabled(True)	
			
	# file TCPIP_NOTIFY_C "$HARMONY_VERSION_PATH/framework/tcpip/src/tcpip_notify.c" to "$PROJECT_SOURCE_FILES/framework/tcpip/src/tcpip_notify.c"
	tcpipStackTcpipNotifySourceFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipNotifySourceFile.setSourcePath("library/src/tcpip_notify.c")
	tcpipStackTcpipNotifySourceFile.setOutputName("tcpip_notify.c")
	tcpipStackTcpipNotifySourceFile.setOverwrite(True)
	tcpipStackTcpipNotifySourceFile.setDestPath("library/tcpip/src/")
	tcpipStackTcpipNotifySourceFile.setProjectPath("config/" + configName + "library/src/")
	tcpipStackTcpipNotifySourceFile.setType("SOURCE")
	tcpipStackTcpipNotifySourceFile.setEnabled(True)	
		
	# file TCPIP_PACKET_C "$HARMONY_VERSION_PATH/framework/tcpip/src/tcpip_packet.c" to "$PROJECT_SOURCE_FILES/framework/tcpip/src/tcpip_packet.c"
	tcpipStackTcpipPacketSourceFile = tcpipStackComponent.createFileSymbol(None, None)
	tcpipStackTcpipPacketSourceFile.setSourcePath("library/src/tcpip_packet.c")
	tcpipStackTcpipPacketSourceFile.setOutputName("tcpip_packet.c")
	tcpipStackTcpipPacketSourceFile.setOverwrite(True)
	tcpipStackTcpipPacketSourceFile.setDestPath("library/tcpip/src/")
	tcpipStackTcpipPacketSourceFile.setProjectPath("config/" + configName + "library/src/")
	tcpipStackTcpipPacketSourceFile.setType("SOURCE")
	tcpipStackTcpipPacketSourceFile.setEnabled(True)	
		
def tcpipStackMenuVisible(symbol, event):
	if (event["value"] == True):
		print("TCPIP Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("TCPIP Menu Invisible.")
		symbol.setVisible(False)
		
def tcpipStackMenuInvisible(symbol, event):
	if (event["value"] == False):
		print("TCPIP Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("TCPIP Menu Invisible.")
		symbol.setVisible(False)		
