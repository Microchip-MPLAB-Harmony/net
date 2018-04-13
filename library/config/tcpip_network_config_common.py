tcpipNetConfigNumMaximum = 10

def instantiateComponent(tcpipNetConfigComponent):

    tcpipNetConfigNumMax = tcpipNetConfigComponent.createIntegerSymbol("TCPIP_STACK_NETWORK_CONFIG_NUMBER_MAX", None)
    tcpipNetConfigNumMax.setLabel("Number of Instances")
    #tcpipNetConfigNumMax.setMin(1)
   # tcpipNetConfigNumMax.setMax(tcpipNetConfigNumMaximum)
    tcpipNetConfigNumMax.setDefaultValue(tcpipNetConfigNumMaximum)
    tcpipNetConfigNumMax.setVisible(False)

