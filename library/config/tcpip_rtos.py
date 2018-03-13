# Global definitions  
thirdparty_rtos_sys_tasks_options = ["Standalone","Combined with System Tasks"]
def instantiateComponent(tcpipRtosComponent):
	print("TCPIP RTOS Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	
	# Enable Rtos menu
	tcpipRtosMenu = tcpipRtosComponent.createMenuSymbol(None, None)
	tcpipRtosMenu.setLabel("RTOS Configuration")
	tcpipRtosMenu.setVisible(True)
	tcpipRtosMenu.setDescription("Enable IPv4")
	# niyas set dependencies to 3rd party RTOS also
	#tcpipRtosMenu.setDependencies(tcpipMenuVisibleSingleTrigger, ["USE_3RDPARTY_RTOS"])

	# Menu for RTOS options
	tcpipRtosOptions = tcpipRtosComponent.createComboSymbol("TCPIP_RTOS", tcpipRtosMenu, thirdparty_rtos_sys_tasks_options)
	tcpipRtosOptions.setLabel("Run Library Tasks as")
	tcpipRtosOptions.setVisible(True)
	tcpipRtosOptions.setDescription("Rtos Options")
	tcpipRtosOptions.setDefaultValue("Standalone")
	# niyas set dependencies to 3rd party RTOS also
	#tcpipRtosOptions.setDependencies(tcpipMenuVisibleSingleTrigger, ["USE_3RDPARTY_RTOS"])

			
	# Menu for RTOS Task Size
	tcpipRtosTaskSize = tcpipRtosComponent.createIntegerSymbol("TCPIP_RTOS_TASK_SIZE", tcpipRtosMenu)
	tcpipRtosTaskSize.setLabel("Task Size")
	tcpipRtosTaskSize.setVisible(True)
	tcpipRtosTaskSize.setDescription("Rtos Task Size")
	tcpipRtosTaskSize.setDefaultValue(1024)
	# niyas set dependencies to 3rd party RTOS also
	tcpipRtosTaskSize.setDependencies(tcpipRtosMenuStandalone, ["TCPIP_RTOS"])

	# Menu for RTOS Task Priority
	tcpipRtosTaskPriority = tcpipRtosComponent.createIntegerSymbol("TCPIP_RTOS_TASK_PRIORITY", tcpipRtosMenu)
	tcpipRtosTaskPriority.setLabel("Task Priority")
	tcpipRtosTaskPriority.setVisible(True)
	tcpipRtosTaskPriority.setDescription("Rtos Task Priority")
	tcpipRtosTaskPriority.setDefaultValue(1)
	# niyas set dependencies to 3rd party RTOS also
	tcpipRtosTaskPriority.setDependencies(tcpipRtosMenuStandalone, ["TCPIP_RTOS"])

	# Use RTOS Task Delay 
	tcpipUseRtosDelay = tcpipRtosComponent.createBooleanSymbol("TCPIP_RTOS_USE_DELAY", tcpipRtosMenu)
	tcpipUseRtosDelay.setLabel("Use Task Delay?")
	tcpipUseRtosDelay.setVisible(True)
	tcpipUseRtosDelay.setDescription("Use RTOS Task Delay")
	tcpipUseRtosDelay.setDefaultValue(1000)
	tcpipUseRtosDelay.setDependencies(tcpipRtosMenuStandalone, ["TCPIP_RTOS"])

	# RTOS Task Delay
	tcpipRtosDelay = tcpipRtosComponent.createIntegerSymbol("TCPIP_RTOS_DELAY", tcpipUseRtosDelay)
	tcpipRtosDelay.setLabel("Task Delay")
	tcpipRtosDelay.setVisible(True)
	tcpipRtosDelay.setDescription("Rtos Task Delay")
	tcpipRtosDelay.setDefaultValue(1000)
	# niyas set dependencies to 3rd party RTOS also
	tcpipRtosDelay.setDependencies(tcpipRtosMenuVisibleSingle, ["TCPIP_RTOS_USE_DELAY"])


def tcpipRtosMenuStandalone(symbol, event):
	if (event["value"] == "Standalone"):
		symbol.setVisible(True)
		print("TCP Rtos Standalone menu Visible.")	
	else:
		symbol.setVisible(False)	
		print("TCP Rtos Standalone menu Invisible.")	
		
def tcpipRtosMenuVisibleSingle(symbol, event):
	if (event["value"] == True):
		print("TCP Rtos Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("TCP Rtos Menu Invisible.")
		symbol.setVisible(False)	