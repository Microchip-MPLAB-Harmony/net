    
def instantiateComponent(tcpipNdpComponent):
	print("TCPIP NDP Component")
	configName = Variables.get("__CONFIGURATION_NAME")
	
	# Settings for Neighbor Discovery Protocol
	#tcpipNdp = tcpipNdpComponent.createMenuSymbol("TCPIP_IPV6_NDP", tcpipIPv6)
	tcpipNdp = tcpipNdpComponent.createMenuSymbol("TCPIP_IPV6_NDP", None)
	tcpipNdp.setLabel("Neighbor Discovery Protocol")
	tcpipNdp.setVisible(False)
	tcpipNdp.setDescription("Neighbor Discovery ProtocolSettings")
	tcpipNdp.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# RTR Solicitation Delay in seconds
	tcpipNdpRtrDelay = tcpipNdpComponent.createIntegerSymbol("TCPIP_MAX_RTR_SOLICITATION_DELAY", tcpipNdp)
	tcpipNdpRtrDelay.setLabel("RTR Solicitation Delay - Seconds")
	tcpipNdpRtrDelay.setVisible(False)
	tcpipNdpRtrDelay.setDescription("RTR Solicitation Delay in seconds")
	tcpipNdpRtrDelay.setDefaultValue(1)
	tcpipNdpRtrDelay.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# RTR Solicitation Interval in seconds
	tcpipNdpRtrSolicitInterval = tcpipNdpComponent.createIntegerSymbol("TCPIP_RTR_SOLICITATION_INTERVAL", tcpipNdp)
	tcpipNdpRtrSolicitInterval.setLabel("RTR Solicitation Interval - Seconds")
	tcpipNdpRtrSolicitInterval.setVisible(False)
	tcpipNdpRtrSolicitInterval.setDescription("RTR Solicitation Interval in seconds")
	tcpipNdpRtrSolicitInterval.setDefaultValue(4)
	tcpipNdpRtrSolicitInterval.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# Maximum RTR Solicitations
	tcpipNdpRtrSolicitMax = tcpipNdpComponent.createIntegerSymbol("TCPIP_MAX_RTR_SOLICITATIONS", tcpipNdp)
	tcpipNdpRtrSolicitMax.setLabel("Max RTR Solicitations")
	tcpipNdpRtrSolicitMax.setVisible(False)
	tcpipNdpRtrSolicitMax.setDescription("Maximum RTR Solicitations")
	tcpipNdpRtrSolicitMax.setDefaultValue(3)
	tcpipNdpRtrSolicitMax.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# Maximum Multicast Solicitations
	tcpipNdpMcastSolicitMax = tcpipNdpComponent.createIntegerSymbol("TCPIP_MAX_MULTICAST_SOLICIT", tcpipNdp)
	tcpipNdpMcastSolicitMax.setLabel("Max Multicast Solicitations")
	tcpipNdpMcastSolicitMax.setVisible(False)
	tcpipNdpMcastSolicitMax.setDescription("Maximum Multicast Solicitations")
	tcpipNdpMcastSolicitMax.setDefaultValue(3)
	tcpipNdpMcastSolicitMax.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# Maximum Unicast Solicitations
	tcpipNdpUcastSolicitMax = tcpipNdpComponent.createIntegerSymbol("TCPIP_MAX_UNICAST_SOLICIT", tcpipNdp)
	tcpipNdpUcastSolicitMax.setLabel("Max Unicast Solicitations")
	tcpipNdpUcastSolicitMax.setVisible(False)
	tcpipNdpUcastSolicitMax.setDescription("Maximum Unicast Solicitations")
	tcpipNdpUcastSolicitMax.setDefaultValue(3)
	tcpipNdpUcastSolicitMax.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# Maximum Anycast Delay Time in Seconds
	tcpipNdpMaxAnycastDelayTime = tcpipNdpComponent.createIntegerSymbol("TCPIP_MAX_ANYCAST_DELAY_TIME", tcpipNdp)
	tcpipNdpMaxAnycastDelayTime.setLabel("Max Anycast Delay Time in Seconds")
	tcpipNdpMaxAnycastDelayTime.setVisible(False)
	tcpipNdpMaxAnycastDelayTime.setDescription("Maximum Anycast Delay Time in Seconds")
	tcpipNdpMaxAnycastDelayTime.setDefaultValue(1)
	tcpipNdpMaxAnycastDelayTime.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# Maximum Neighbor Advertisement Transmissions
	tcpipNdpMaxNeighbAdvTxn = tcpipNdpComponent.createIntegerSymbol("TCPIP_MAX_NEIGHBOR_ADVERTISEMENT", tcpipNdp)
	tcpipNdpMaxNeighbAdvTxn.setLabel("Max Neighbor Advertisement Transmissions")
	tcpipNdpMaxNeighbAdvTxn.setVisible(False)
	tcpipNdpMaxNeighbAdvTxn.setDescription("Maximum Neighbor Advertisement Transmissions")
	tcpipNdpMaxNeighbAdvTxn.setDefaultValue(3)
	tcpipNdpMaxNeighbAdvTxn.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# Reachable Time in Seconds
	tcpipNdpReachTime = tcpipNdpComponent.createIntegerSymbol("TCPIP_REACHABLE_TIME", tcpipNdp)
	tcpipNdpReachTime.setLabel("Reachable Time in Seconds")
	tcpipNdpReachTime.setVisible(False)
	tcpipNdpReachTime.setDescription("Reachable Time in Seconds")
	tcpipNdpReachTime.setDefaultValue(30)
	tcpipNdpReachTime.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# Re-transmission Timer in Seconds
	tcpipNdpRetxnTime = tcpipNdpComponent.createIntegerSymbol("TCPIP_RETRANS_TIMER", tcpipNdp)
	tcpipNdpRetxnTime.setLabel("Re-transmission Timer - Seconds")
	tcpipNdpRetxnTime.setVisible(False)
	tcpipNdpRetxnTime.setDescription("Re-transmission Timer in Seconds")
	tcpipNdpRetxnTime.setDefaultValue(1)
	tcpipNdpRetxnTime.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# Delay First Probe Time in Seconds
	tcpipNdpDlyFirstProbeTime = tcpipNdpComponent.createIntegerSymbol("TCPIP_DELAY_FIRST_PROBE_TIME", tcpipNdp)
	tcpipNdpDlyFirstProbeTime.setLabel("Delay First Probe Time in Seconds")
	tcpipNdpDlyFirstProbeTime.setVisible(False)
	tcpipNdpDlyFirstProbeTime.setDescription("Delay First Probe Time in Seconds")
	tcpipNdpDlyFirstProbeTime.setDefaultValue(5)
	tcpipNdpDlyFirstProbeTime.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# IPv6 Maximum Transmission Unit Increase Time-out in Seconds
	tcpipNdpMtuIncreaseTimeout = tcpipNdpComponent.createIntegerSymbol("TCPIP_IPV6_MTU_INCREASE_TIMEOUT", tcpipNdp)
	tcpipNdpMtuIncreaseTimeout.setLabel("IPv6 Maximum Transmission Unit Increase Time-out in Seconds")
	tcpipNdpMtuIncreaseTimeout.setVisible(False)
	tcpipNdpMtuIncreaseTimeout.setDescription("IPv6 Maximum Transmission Unit Increase Time-out in Seconds")
	tcpipNdpMtuIncreaseTimeout.setDefaultValue(600)
	tcpipNdpMtuIncreaseTimeout.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	# NDP Task Timer in msec
	tcpipNdpTaskTimer = tcpipNdpComponent.createIntegerSymbol("TCPIP_NDP_TASK_TIMER_RATE", tcpipNdp)
	tcpipNdpTaskTimer.setLabel("NDP Task Timer in msec")
	tcpipNdpTaskTimer.setVisible(False)
	tcpipNdpTaskTimer.setDescription("NDP Task Timer in msec")
	tcpipNdpTaskTimer.setDefaultValue(32)
	tcpipNdpTaskTimer.setDependencies(tcpipNdpMenuVisible, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	#Add to system_config.h
	tcpipNdpHeaderFtl = tcpipNdpComponent.createFileSymbol(None, None)
	tcpipNdpHeaderFtl.setSourcePath("library/config/ndp.h.ftl")
	tcpipNdpHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	tcpipNdpHeaderFtl.setMarkup(True)
	tcpipNdpHeaderFtl.setType("STRING")

	# Add ndp.h file to project
	tcpipNdpHeaderFile = tcpipNdpComponent.createFileSymbol(None, None)
	tcpipNdpHeaderFile.setSourcePath("library/ndp.h")
	tcpipNdpHeaderFile.setOutputName("ndp.h")
	tcpipNdpHeaderFile.setDestPath("library/tcpip/")
	tcpipNdpHeaderFile.setProjectPath("config/" + configName + "library/tcpip/")
	tcpipNdpHeaderFile.setType("HEADER")
	tcpipNdpHeaderFile.setOverwrite(True)

	# Add ndp_manager.h file to project
	tcpipNdpManagerHeaderFile = tcpipNdpComponent.createFileSymbol(None, None)
	tcpipNdpManagerHeaderFile.setSourcePath("library/src/ndp_manager.h")
	tcpipNdpManagerHeaderFile.setOutputName("ndp_manager.h")
	tcpipNdpManagerHeaderFile.setDestPath("library/tcpip/src/")
	tcpipNdpManagerHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipNdpManagerHeaderFile.setType("HEADER")
	tcpipNdpManagerHeaderFile.setOverwrite(True)

	# Add ndp_private.h file to project
	tcpipNdpPrivateHeaderFile = tcpipNdpComponent.createFileSymbol(None, None)
	tcpipNdpPrivateHeaderFile.setSourcePath("library/src/ndp_private.h")
	tcpipNdpPrivateHeaderFile.setOutputName("ndp_private.h")
	tcpipNdpPrivateHeaderFile.setDestPath("library/tcpip/src/")
	tcpipNdpPrivateHeaderFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipNdpPrivateHeaderFile.setType("HEADER")
	tcpipNdpPrivateHeaderFile.setOverwrite(True)

	# Add ndp.c file
	tcpipNdpSourceFile = tcpipNdpComponent.createFileSymbol(None, None)
	tcpipNdpSourceFile.setSourcePath("library/src/ndp.c")
	tcpipNdpSourceFile.setOutputName("ndp.c")
	tcpipNdpSourceFile.setOverwrite(True)
	tcpipNdpSourceFile.setDestPath("library/tcpip/src/")
	tcpipNdpSourceFile.setProjectPath("config/" + configName + "library/tcpip/src/")
	tcpipNdpSourceFile.setType("SOURCE")
	tcpipNdpSourceFile.setEnabled(False)
	tcpipNdpSourceFile.setDependencies(tcpipNdpGenSourceFile, ["tcpipIPv6.TCPIP_STACK_USE_IPV6"])

	

def tcpipNdpMenuVisible(symbol, event):
	if (event["value"] == True):
		print("TCPIP Menu Visible.")		
		symbol.setVisible(True)
	else:
		print("TCPIP Menu Invisible.")
		symbol.setVisible(False)
		
def tcpipNdpGenSourceFile(sourceFile, event):
	sourceFile.setEnabled(event["value"])