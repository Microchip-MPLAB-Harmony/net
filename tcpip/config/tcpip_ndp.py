    
def instantiateComponent(tcpipNdpComponent):
	print("TCPIP NDP Component")
	configName = Variables.get("__CONFIGURATION_NAME")	
	
	# Enable Neighbor Discovery Protocol Settings 	
	tcpipNdp = tcpipNdpComponent.createBooleanSymbol("TCPIP_USE_NDP", None)
	tcpipNdp.setLabel("NDP")
	tcpipNdp.setVisible(False)
	tcpipNdp.setDescription("Enable Neighbor Discovery Protocol Settings")
	tcpipNdp.setDefaultValue(True)
	
	# RTR Solicitation Delay in seconds
	tcpipNdpRtrDelay = tcpipNdpComponent.createIntegerSymbol("TCPIP_MAX_RTR_SOLICITATION_DELAY", None)
	tcpipNdpRtrDelay.setLabel("RTR Solicitation Delay - Seconds")
	tcpipNdpRtrDelay.setVisible(True)
	tcpipNdpRtrDelay.setDescription("RTR Solicitation Delay in seconds")
	tcpipNdpRtrDelay.setDefaultValue(1)
	#tcpipNdpRtrDelay.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# RTR Solicitation Interval in seconds
	tcpipNdpRtrSolicitInterval = tcpipNdpComponent.createIntegerSymbol("TCPIP_RTR_SOLICITATION_INTERVAL", None)
	tcpipNdpRtrSolicitInterval.setLabel("RTR Solicitation Interval - Seconds")
	tcpipNdpRtrSolicitInterval.setVisible(True)
	tcpipNdpRtrSolicitInterval.setDescription("RTR Solicitation Interval in seconds")
	tcpipNdpRtrSolicitInterval.setDefaultValue(4)
	#tcpipNdpRtrSolicitInterval.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# Maximum RTR Solicitations
	tcpipNdpRtrSolicitMax = tcpipNdpComponent.createIntegerSymbol("TCPIP_MAX_RTR_SOLICITATIONS", None)
	tcpipNdpRtrSolicitMax.setLabel("Max RTR Solicitations")
	tcpipNdpRtrSolicitMax.setVisible(True)
	tcpipNdpRtrSolicitMax.setDescription("Maximum RTR Solicitations")
	tcpipNdpRtrSolicitMax.setDefaultValue(3)
	#tcpipNdpRtrSolicitMax.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# Maximum Multicast Solicitations
	tcpipNdpMcastSolicitMax = tcpipNdpComponent.createIntegerSymbol("TCPIP_MAX_MULTICAST_SOLICIT", None)
	tcpipNdpMcastSolicitMax.setLabel("Max Multicast Solicitations")
	tcpipNdpMcastSolicitMax.setVisible(True)
	tcpipNdpMcastSolicitMax.setDescription("Maximum Multicast Solicitations")
	tcpipNdpMcastSolicitMax.setDefaultValue(3)
	#tcpipNdpMcastSolicitMax.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# Maximum Unicast Solicitations
	tcpipNdpUcastSolicitMax = tcpipNdpComponent.createIntegerSymbol("TCPIP_MAX_UNICAST_SOLICIT", None)
	tcpipNdpUcastSolicitMax.setLabel("Max Unicast Solicitations")
	tcpipNdpUcastSolicitMax.setVisible(True)
	tcpipNdpUcastSolicitMax.setDescription("Maximum Unicast Solicitations")
	tcpipNdpUcastSolicitMax.setDefaultValue(3)
	#tcpipNdpUcastSolicitMax.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# Maximum Anycast Delay Time in Seconds
	tcpipNdpMaxAnycastDelayTime = tcpipNdpComponent.createIntegerSymbol("TCPIP_MAX_ANYCAST_DELAY_TIME", None)
	tcpipNdpMaxAnycastDelayTime.setLabel("Max Anycast Delay Time in Seconds")
	tcpipNdpMaxAnycastDelayTime.setVisible(True)
	tcpipNdpMaxAnycastDelayTime.setDescription("Maximum Anycast Delay Time in Seconds")
	tcpipNdpMaxAnycastDelayTime.setDefaultValue(1)
	#tcpipNdpMaxAnycastDelayTime.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# Maximum Neighbor Advertisement Transmissions
	tcpipNdpMaxNeighbAdvTxn = tcpipNdpComponent.createIntegerSymbol("TCPIP_MAX_NEIGHBOR_ADVERTISEMENT", None)
	tcpipNdpMaxNeighbAdvTxn.setLabel("Max Neighbor Advertisement Transmissions")
	tcpipNdpMaxNeighbAdvTxn.setVisible(True)
	tcpipNdpMaxNeighbAdvTxn.setDescription("Maximum Neighbor Advertisement Transmissions")
	tcpipNdpMaxNeighbAdvTxn.setDefaultValue(3)
	#tcpipNdpMaxNeighbAdvTxn.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# Reachable Time in Seconds
	tcpipNdpReachTime = tcpipNdpComponent.createIntegerSymbol("TCPIP_REACHABLE_TIME", None)
	tcpipNdpReachTime.setLabel("Reachable Time in Seconds")
	tcpipNdpReachTime.setVisible(True)
	tcpipNdpReachTime.setDescription("Reachable Time in Seconds")
	tcpipNdpReachTime.setDefaultValue(30)
	#tcpipNdpReachTime.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# Re-transmission Timer in Seconds
	tcpipNdpRetxnTime = tcpipNdpComponent.createIntegerSymbol("TCPIP_RETRANS_TIMER", None)
	tcpipNdpRetxnTime.setLabel("Re-transmission Timer - Seconds")
	tcpipNdpRetxnTime.setVisible(True)
	tcpipNdpRetxnTime.setDescription("Re-transmission Timer in Seconds")
	tcpipNdpRetxnTime.setDefaultValue(1)
	#tcpipNdpRetxnTime.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# Delay First Probe Time in Seconds
	tcpipNdpDlyFirstProbeTime = tcpipNdpComponent.createIntegerSymbol("TCPIP_DELAY_FIRST_PROBE_TIME", None)
	tcpipNdpDlyFirstProbeTime.setLabel("Delay First Probe Time in Seconds")
	tcpipNdpDlyFirstProbeTime.setVisible(True)
	tcpipNdpDlyFirstProbeTime.setDescription("Delay First Probe Time in Seconds")
	tcpipNdpDlyFirstProbeTime.setDefaultValue(5)
	#tcpipNdpDlyFirstProbeTime.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# IPv6 Maximum Transmission Unit Increase Time-out in Seconds
	tcpipNdpMtuIncreaseTimeout = tcpipNdpComponent.createIntegerSymbol("TCPIP_IPV6_MTU_INCREASE_TIMEOUT", None)
	tcpipNdpMtuIncreaseTimeout.setLabel("IPv6 Maximum Transmission Unit Increase Time-out in Seconds")
	tcpipNdpMtuIncreaseTimeout.setVisible(True)
	tcpipNdpMtuIncreaseTimeout.setDescription("IPv6 Maximum Transmission Unit Increase Time-out in Seconds")
	tcpipNdpMtuIncreaseTimeout.setDefaultValue(600)
	#tcpipNdpMtuIncreaseTimeout.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# NDP Task Timer in msec
	tcpipNdpTaskTimer = tcpipNdpComponent.createIntegerSymbol("TCPIP_NDP_TASK_TIMER_RATE", None)
	tcpipNdpTaskTimer.setLabel("NDP Task Timer in msec")
	tcpipNdpTaskTimer.setVisible(True)
	tcpipNdpTaskTimer.setDescription("NDP Task Timer in msec")
	tcpipNdpTaskTimer.setDefaultValue(32)
	#tcpipNdpTaskTimer.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	#Add to system_config.h
	tcpipNdpHeaderFtl = tcpipNdpComponent.createFileSymbol(None, None)
	tcpipNdpHeaderFtl.setSourcePath("tcpip/config/ndp.h.ftl")
	tcpipNdpHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipNdpHeaderFtl.setMarkup(True)
	tcpipNdpHeaderFtl.setType("STRING")

	# Add ndp.c file
	tcpipNdpSourceFile = tcpipNdpComponent.createFileSymbol(None, None)
	tcpipNdpSourceFile.setSourcePath("tcpip/src/ndp.c")
	tcpipNdpSourceFile.setOutputName("ndp.c")
	tcpipNdpSourceFile.setOverwrite(True)
	tcpipNdpSourceFile.setDestPath("library/tcpip/src/")
	tcpipNdpSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
	tcpipNdpSourceFile.setType("SOURCE")
	tcpipNdpSourceFile.setEnabled(True)
	#tcpipNdpSourceFile.setDependencies(tcpipNdpGenSourceFile, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	

def tcpipNdpMenuVisible(symbol, event):
	if (event["value"] == True):
		print("TCPIP Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("TCPIP Menu Invisible.")
		symbol.setVisible(False)
		
def tcpipNdpGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])
	

def destroyComponent(component):
	Database.setSymbolValue("tcpipNdp", "TCPIP_USE_NDP", False, 2)