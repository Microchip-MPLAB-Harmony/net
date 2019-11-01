"""*****************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************"""
global atdfMacModule
global atdfMacModuleId
global atdfMacInstanceName
global userMacChoiceId
global macComponentId
global macBaseAddressValue
global defaultMacDriverCode
global phyComponent
global showSuperfluous

# Constants
MODULE_ID_SAM_9X60 =        "6119"

MAX_QUEUE_COUNT =           1
MAX_JUMBO_PACKET_SIZE =     10240
MAX_FRAME_SIZE =            1536
MIN_TX_BUFFER_SIZE =        64
MAX_TX_BUFFER_SIZE =        2047
MIN_TX_DESCRIPTOR_COUNT =   (MAX_JUMBO_PACKET_SIZE/MIN_TX_BUFFER_SIZE)
TX_DEVICE_MAX_DESCRIPTORS = 1024

MIN_RX_BUFFER_SIZE =        128
MAX_RX_BUFFER_SIZE =        MIN_RX_BUFFER_SIZE
MIN_RX_DESCRIPTOR_COUNT =   (MAX_FRAME_SIZE / MIN_RX_BUFFER_SIZE)
RX_DEVICE_MAX_DESCRIPTORS = 1024

atdfMacModuleCandidates =   [ "EMAC" ] # search atdf and use to assign atdfMacInstanceName
atdfMacModule =             ""
atdfMacModuleId =           ""
atdfMacInstanceName =       ""
userMacChoiceId =           0
macComponentId =            ""
macBaseAddressValue =       ""
defaultMacDriverCode =      ""
phyComponent =              ""
showSuperfluous =           False

def initializeGlobals( aComponent, theModuleCandidates ):
    global atdfMacModule
    global atdfMacModuleId
    global atdfMacInstanceName
    global userMacChoiceId
    global macComponentId
    global macBaseAddressValue
    global defaultMacDriverCode

    macComponentId = aComponent.getID()
    # the first instance, instance 0, may not have number on the end of the component name
    userMacChoiceId = 0
    ch = macComponentId[-1:]
    if ch.isdigit():
        userMacChoiceId = userMacChoiceId + int( ch )

    atdfMacInstanceName = 'unknownModuleName'
    atdfModuleStr = '/avr-tools-device-file/devices/device/peripherals/module@[name="'
    atdfModuleStrEnd = '"]'
    for module in theModuleCandidates:
        node = ATDF.getNode( atdfModuleStr + module + atdfModuleStrEnd )
        if node:
            atdfMacModule = module
            atdfMacModuleId = node.getAttribute( 'id' )
            atdfMacInstanceName = node.getChildren()[ userMacChoiceId ].getAttribute( 'name' )
            break;
    macBaseAddressValue = atdfMacInstanceName + "_BASE_ADDRESS"
    # default with atdfMacModule appropriate for now; could use atdfMacInstanceName if needed
    defaultMacDriverCode = 'DRV_' + atdfMacInstanceName + "_Object"
    #'''
    print "initializeGlobals entry ======================================="
    print "atdfMacModule = " + atdfMacModule
    print "atdfMacModuleId = " + atdfMacModuleId
    print "atdfMacInstanceName = " + atdfMacInstanceName
    print "macComponentId = " + macComponentId
    print "userMacChoiceId = <" + str( userMacChoiceId ) + ">"
    print "defaultMacDriverCode = " + defaultMacDriverCode
    print "macBaseAddressValue = " + macBaseAddressValue
    print "initializeGlobals exit ========================================"
    #'''


global interruptSetHandler
def interruptSetHandler( aBool ):
    global atdfMacInstanceName

    interruptVector =           atdfMacInstanceName + "_INTERRUPT_ENABLE"
    interruptHandler =          atdfMacInstanceName + "_INTERRUPT_HANDLER"
    interruptHandlerLock =      atdfMacInstanceName + "_INTERRUPT_HANDLER_LOCK"
    interruptCallback =         atdfMacInstanceName + "_InterruptHandler"
    interruptDefaultCallback =  atdfMacInstanceName + "_Handler"
    cb = interruptCallback if( aBool == True ) else interruptDefaultCallback
    Database.clearSymbolValue( "core", interruptVector )
    Database.clearSymbolValue( "core", interruptHandler )
    Database.clearSymbolValue( "core", interruptHandlerLock )

    Database.setSymbolValue( "core", interruptVector, aBool )
    Database.setSymbolValue( "core", interruptHandler, cb )
    Database.setSymbolValue( "core", interruptHandlerLock, aBool )


def interruptControl( symbol, event ):
    global interruptSetHandler
    interruptSetHandler( event["value"] == True )


def onAttachmentConnected( source, target ):
    global atdfMacInstanceName
    global userMacChoiceId
    global macComponentId
    global phyComponent

    if(     (source["id"] == "MAC_PHY_Dependency" + str( userMacChoiceId ))
        or  ((userMacChoiceId == 0) and (source["id"] == "MAC_PHY_Dependency"))
    ):
        phyComponent = target[ 'component' ].getID()
        Database.setSymbolValue( macComponentId, "DRV_MAC_PHY_TYPE", target['component'].getDisplayName() )


def onAttachmentDisconnected( source, target ):
    global userMacChoiceId
    global macComponentId

    if(     (source['id'] == "MAC_PHY_Dependency" + str( userMacChoiceId ))
        or  ((userMacChoiceId == 0) and (source['id'] == "MAC_PHY_Dependency"))
    ):
        phyComponent = ""
        Database.clearSymbolValue( macComponentId, 'DRV_MAC_PHY_TYPE' )


def getMacDeviceCoreName():
    global atdfMacModuleId
    deviceNames = {
        MODULE_ID_SAM_9X60: "SAM9X60",
    }
    return deviceNames.get( atdfMacModuleId, 'unknownMac' )


def getNumberOfMacQueues():
    global atdfMacModuleId
    numMacQueues = {
        MODULE_ID_SAM_9X60: 1,
    }
    return numMacQueues.get( atdfMacModuleId, 0 )


def maxRxPacketPointerUpdate( symbol, event ):
    namespace = event[ 'namespace' ]
    largestRxQueueDescriptorCount = 0
    maxQueueCount = Database.getSymbolValue( namespace, 'MAC_MAX_QUEUE_COUNT' )
    for qq in range( 0, maxQueueCount ):
        qqStr = str( qq )
        descriptorCnt = Database.getSymbolValue( namespace, 'RX_DESCRIPTOR_COUNT_' + qqStr )
        if descriptorCnt > largestRxQueueDescriptorCount:
            largestRxQueueDescriptorCount = descriptorCnt
    symbol.setValue( largestRxQueueDescriptorCount )


def getRmiiMskCmpValueFor( rmiiEnabled ):
    global atdfMacModuleId
    retval = 0xFF
    if rmiiEnabled:
        rmiiCmpValues = {
            MODULE_ID_SAM_9X60: 1,
        }
        retval = rmiiCmpValues.get( atdfMacModuleId, retval )
    return retval


def sendRmiiUpdate( symbol, event ):
    global macComponentId
    global userMacChoiceId
    global phyComponent

    if len( phyComponent ):
        Database.sendMessage( phyComponent, 'MII_CHANGE',
                            {   'PhyComponent':phyComponent,
                                'UseRmii':event[ 'value' ],
                            }
                        )


def setVisibleMatching( symbol, event ):
    symbol.setVisible( event["value"] )


def setVisibleNotMatching( symbol, event ):
    symbol.setVisible( not event["value"] )


def setRxDeficitValue( symbol, event ):
    # we must have at least 1 buffer per descriptor, if we can allocate then we can always work enough to work
    # otherwise tell the user a deficit is present
    id = event[ 'id' ]
    if "QUEUE_ENABLE_" in id :
        setVisibleMatching( symbol, event )

    namespace = event[ 'namespace' ]
    index = str( filter( lambda func: func.isdigit(), id ) )
    descriptors =    Database.getSymbolValue( namespace, 'RX_DESCRIPTOR_COUNT_' + index )
    staticBuffers =  Database.getSymbolValue( namespace, 'RX_STATIC_BUFFERS_' + index )
    threshold =      Database.getSymbolValue( namespace, 'RX_BUFFER_ALLOCATION_THRESHOLD_' + index )
    if( (descriptors > staticBuffers) and (0 == threshold) ):
        symbol.setValue( descriptors - staticBuffers )
    else:
        symbol.setValue( 0 )


def destroyComponent( macComponent ):
    # This keeps tcpip_configurator_driver.py code in sync with this object existence
    sendStateChangeMessage( 'tcpip_driver_config', 'MAC_STATE_DESTROYED' )


def finalizeComponent( macComponent ):
    # This keeps tcpip_configurator_driver.py code in sync with this object existence
    sendStateChangeMessage( 'tcpip_driver_config', 'MAC_STATE_IN_EXISTANCE' )
    if( None != Database.getSymbolValue( 'tcpipNetConfig_0', 'TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX0' ) ):
        sendStateChangeMessage( 'tcpipNetConfig_0', 'MAC_STATE_IN_EXISTANCE' )


def sendStateChangeMessage( aRecipient, aState ):
    global macComponentId
    global macBaseAddressValue
    return Database.sendMessage( aRecipient, 'MAC_STATE_CHANGE',
                                    {   'ComponentId':macComponentId,
                                        'ChoiceId':userMacChoiceId,
                                        'MacBaseAddress':macBaseAddressValue,
                                        'DefaultMacDriverCode':defaultMacDriverCode,
                                        'State':aState,
                                    }
                                )


def instantiateComponent( macComponent ):
    global atdfMacInstanceName
    global macComponentId
    global macBaseAddressValue
    global defaultMacDriverCode
    global showSuperfluous

    initializeGlobals( macComponent, atdfMacModuleCandidates )
    # Enable MAC clock
    Database.setSymbolValue( "core", atdfMacInstanceName + "_CLOCK_ENABLE", True )

    useMacDriver = macComponent.createBooleanSymbol( "TCPIP_USE_" + atdfMacInstanceName, None )
    useMacDriver.setLabel( "Use internal MAC driver" )
    useMacDriver.setVisible( False )
    useMacDriver.setDefaultValue( True )

    #####
    # MAC Driver Info
    driverInfoMenu = macComponent.createMenuSymbol( None, None )
    driverInfoMenu.setLabel( "Driver Info" )

    # MAC Index Number -- superfluous
    index = macComponent.createIntegerSymbol( "MAC_INDEX", driverInfoMenu )
    index.setLabel( "MAC Index Number" )
    index.setDefaultValue( userMacChoiceId )
    index.setVisible( showSuperfluous )
    index.setReadOnly( True )

    # MAC device core name -- superfluous
    macDeviceCoreName = macComponent.createStringSymbol( "TCPIP_MAC_DEVICE", driverInfoMenu )
    macDeviceCoreName.setLabel( "MAC Device Core Name" )
    macDeviceCoreName.setDescription( "Name of the CPU core with the internal MAC" )
    macDeviceCoreName.setDefaultValue( getMacDeviceCoreName() )
    macDeviceCoreName.setVisible( showSuperfluous )
    macDeviceCoreName.setReadOnly( True )

    # MAC component -- superfluous
    macComponentIdSymbol = macComponent.createStringSymbol( "MAC_COMPONENT_ID", driverInfoMenu )
    macComponentIdSymbol.setLabel( "MAC Component ID" )
    macComponentIdSymbol.setDefaultValue( macComponentId )
    macComponentIdSymbol.setVisible( showSuperfluous )
    macComponentIdSymbol.setReadOnly( True )

    # MAC Peripheral ID -- superfluous
    peripheralId = macComponent.createStringSymbol( "MAC_PERIPHERAL_ID", driverInfoMenu )
    peripheralId.setLabel( "MAC Peripheral ID" )
    peripheralId.setDefaultValue( atdfMacModuleId )
    peripheralId.setVisible( showSuperfluous )
    peripheralId.setReadOnly( True )

    # MAC instance
    instance = macComponent.createStringSymbol( "MAC_PERIPHERAL_INSTANCE", driverInfoMenu )
    instance.setLabel( "MAC Peripheral Instance" )
    instance.setDefaultValue( atdfMacInstanceName )
    instance.setVisible( showSuperfluous )
    instance.setReadOnly( True )

    # MAC base address
    macBaseAddress = macComponent.createStringSymbol( "MAC_BASE_ADDRESS", driverInfoMenu )
    macBaseAddress.setLabel( "MAC Base Address" )
    macBaseAddress.setDefaultValue( macBaseAddressValue )
    macBaseAddress.setReadOnly( True )

    # MAC Network Driver -- for export to NETCONFIG
    driverMacCode = macComponent.createStringSymbol( "DRV_MAC_CODE", driverInfoMenu )
    driverMacCode.setLabel( "Default Network MAC Driver" )
    driverMacCode.setDefaultValue( defaultMacDriverCode )
    driverMacCode.setReadOnly( True )

    # MAC Interrupt Source
    interruptSource = macComponent.createStringSymbol( "MAC_INTERRUPT_SOURCE", driverInfoMenu )
    interruptSource.setLabel( "Interrupt Source" )
    interruptSource.setDefaultValue( atdfMacInstanceName + "_IRQn" )
    interruptSource.setReadOnly( True )
    interruptSetHandler( True )
    interruptSource.setDependencies( interruptControl, ["MAC_INTERRUPT_SOURCE"] )

    # Max Clients
    maxClients = macComponent.createIntegerSymbol( "MAC_MAX_CLIENTS", driverInfoMenu )
    maxClients.setLabel( "Maximum number of clients" )
    maxClients.setDefaultValue( 1 )
    maxClients.setVisible( showSuperfluous )

    # MAC Max queue count
    maxQueueCount = macComponent.createIntegerSymbol( "MAC_MAX_QUEUE_COUNT", None )
    maxQueueCount.setVisible( False )
    maxQueueCount.setDefaultValue( MAX_QUEUE_COUNT )

    # MAC queue count
    queueCount = macComponent.createIntegerSymbol( "MAC_QUEUE_COUNT", driverInfoMenu )
    queueCount.setLabel( "Queue Count" )
    queueCount.setVisible( False )
    queueCount.setDefaultValue( getNumberOfMacQueues() )

    # RX descriptors count
    rxDeviceMaxDescriptors = macComponent.createIntegerSymbol( "RX_DEVICE_MAX_DESCRIPTORS", driverInfoMenu )
    rxDeviceMaxDescriptors.setLabel( "RX Device Maximum Descriptors" )
    rxDeviceMaxDescriptors.setVisible( False )
    rxDeviceMaxDescriptors.setDefaultValue( RX_DEVICE_MAX_DESCRIPTORS )

    largestRxQueueDescriptorCount = 0
    maxRxPacketPointersDependencies = []
    maxQueueCount = maxQueueCount.getValue()
    queueCountValue = queueCount.getValue()
    for qq in range( 0, maxQueueCount ):
        qqStr = str( qq )
        # Queue enable
        queueEnableName = "QUEUE_ENABLE_" + qqStr
        queueEnable = macComponent.createBooleanSymbol( queueEnableName, None )
        queueEnable.setLabel( "Queue " + qqStr + " enable" )
        queueEnable.setVisible( True if qq < queueCountValue else False )
        queueEnable.setDefaultValue( False )
        queueMenu = queueEnable
        if qq == 0:
            queueEnable.setDefaultValue( True )
            queueEnable.setReadOnly( True )
            queueEnable0Alternative = macComponent.createCommentSymbol( "QUEUE_ENABLE_0_ALTERNATIVE", None )
            queueEnable0Alternative.setLabel( "Queue Info" )
            queueEnable0Alternative.setVisible( False )
            if queueCountValue == 1:
                queueEnable.setVisible( False )
                queueEnable0Alternative.setVisible( True )
                queueMenu = queueEnable0Alternative

        # TX descriptors count
        txDescriptorCount = macComponent.createIntegerSymbol( "TX_DESCRIPTOR_COUNT_" + qqStr, queueMenu )
        txDescriptorCount.setLabel( "TX descriptor count" )
        txDescriptorCount.setMin( MIN_TX_DESCRIPTOR_COUNT )
        txDescriptorCount.setMax( TX_DEVICE_MAX_DESCRIPTORS )
        if qq == 0:
            txDescriptorCount.setDefaultValue( 10 )
        else:
            txDescriptorCount.setDefaultValue( MIN_TX_DESCRIPTOR_COUNT if queueEnable.getValue() else 0 )
        txDescriptorCount.setVisible( queueEnable.getValue() )
        txDescriptorCount.setDependencies( setVisibleMatching, [queueEnableName] )

        # TX buffer size
        txBufferSize = macComponent.createIntegerSymbol( "TX_BUFFER_SIZE_" + qqStr, queueMenu )
        txBufferSize.setLabel( "TX buffer size" )
        txBufferSize.setDescription( "Should be a multiple of 16" )
        txBufferSize.setMin( MIN_TX_BUFFER_SIZE )
        txBufferSize.setMax( MAX_TX_BUFFER_SIZE )
        txBufferSize.setDefaultValue( MAX_FRAME_SIZE if qq == 0 else MIN_TX_BUFFER_SIZE )
        txBufferSize.setVisible( queueEnable.getValue() )
        txBufferSize.setDependencies( setVisibleMatching, [queueEnableName] )

        # RX buffer size
        rxBufferSize = macComponent.createIntegerSymbol( "RX_BUFFER_SIZE_" + qqStr, queueMenu )
        rxBufferSize.setLabel( "RX buffer size" )
        rxBufferSize.setDescription( "Should be a multiple of 16" )
        rxBufferSize.setMin( MIN_RX_BUFFER_SIZE )
        rxBufferSize.setMax( MAX_RX_BUFFER_SIZE )
        rxBufferSize.setDefaultValue( MAX_RX_BUFFER_SIZE if qq == 0 else MIN_RX_BUFFER_SIZE )
        rxBufferSize.setVisible( queueEnable.getValue() )
        rxBufferSize.setReadOnly( True )
        rxBufferSize.setDependencies( setVisibleMatching, [queueEnableName] )

        # RX buffer deficit
        rxBufferDeficit = macComponent.createIntegerSymbol( "RX_BUFFER_DEFICIT_" + qqStr, queueMenu )
        rxBufferDeficit.setLabel( "RX buffer deficit" )
        rxBufferDeficit.setDescription( "static and dynamic buffers must satisfy descriptor count, if static count < descriptor count we must be able to allocate" )
        rxBufferDeficit.setDefaultValue( 0 )
        rxBufferDeficit.setReadOnly( True )

        # RX descriptors count
        rxDescriptorCountName = "RX_DESCRIPTOR_COUNT_" + qqStr
        rxDescriptorCount = macComponent.createIntegerSymbol( rxDescriptorCountName, queueMenu )
        rxDescriptorCount.setLabel( "RX descriptor count" )
        rxDescriptorCount.setDescription( "Dictates the minimum buffer total -- static and dynamic buffers" )
        rxDescriptorCount.setMin( MIN_RX_DESCRIPTOR_COUNT )
        rxDescriptorCount.setMax( RX_DEVICE_MAX_DESCRIPTORS )
        if qq == 0:
            rxDescriptorCount.setDefaultValue( RX_DEVICE_MAX_DESCRIPTORS )
        else:
            rxDescriptorCount.setDefaultValue( MIN_RX_DESCRIPTOR_COUNT )
        rxDescriptorCount.setVisible( queueEnable.getValue() )
        rxDescriptorCount.setDependencies( setVisibleMatching, [queueEnableName] )
        if rxDescriptorCount.getValue() > largestRxQueueDescriptorCount:
            largestRxQueueDescriptorCount = rxDescriptorCount.getValue()
        maxRxPacketPointersDependencies.append( rxDescriptorCountName )

        # RX static buffers
        rxStaticBuffersName = "RX_STATIC_BUFFERS_" + qqStr
        rxStaticBuffers = macComponent.createIntegerSymbol( rxStaticBuffersName, queueMenu )
        rxStaticBuffers.setLabel( "RX static buffers" )
        rxStaticBuffers.setDescription( "RX buffers statically allocated" )
        rxStaticBuffers.setMin( 0 )
        rxStaticBuffers.setDefaultValue( 0 )
        rxStaticBuffers.setVisible( queueEnable.getValue() )
        rxStaticBuffers.setDependencies( setVisibleMatching, [queueEnableName] )

        # RX dynamic allocation threshold
        rxBufferAllocationThresholdName = "RX_BUFFER_ALLOCATION_THRESHOLD_" + qqStr
        rxBufferAllocationThreshold = macComponent.createIntegerSymbol( rxBufferAllocationThresholdName, queueMenu )
        rxBufferAllocationThreshold.setLabel( "RX dynamic allocation threshold" )
        rxBufferAllocationThreshold.setDefaultValue( 10 if queueEnable.getValue() else 0 )
        rxBufferAllocationThreshold.setMin( 0 )
        rxBufferAllocationThreshold.setVisible( queueEnable.getValue() )
        rxBufferAllocationThreshold.setDependencies( setVisibleMatching, [queueEnableName] )

        # RX dynamic buffer allocation
        rxBufferAllocationName = "RX_BUFFER_ALLOCATION_" + qqStr
        rxBufferAllocation = macComponent.createIntegerSymbol( rxBufferAllocationName, queueMenu )
        rxBufferAllocation.setLabel( "RX dynamic buffer allocation" )
        rxBufferAllocation.setMin( 1 )
        rxBufferAllocation.setDefaultValue( 10 )
        rxBufferAllocation.setVisible( queueEnable.getValue() )
        rxBufferAllocation.setDependencies( setVisibleMatching, [queueEnableName] )

        # delayed setup
        rxBufferDeficit.setDependencies( setRxDeficitValue, [queueEnableName, rxDescriptorCountName, rxStaticBuffersName, rxBufferAllocationThresholdName] )

    maxRxPacketPointers = macComponent.createIntegerSymbol( "MAX_RX_PACKET_POINTERS", None )
    maxRxPacketPointers.setLabel( "Max RX Packet Pointers" )
    maxRxPacketPointers.setDescription( "Max handle count expected for RX descriptors" )
    maxRxPacketPointers.setVisible( False )
    maxRxPacketPointers.setValue( largestRxQueueDescriptorCount )
    maxRxPacketPointers.setDependencies( maxRxPacketPointerUpdate, maxRxPacketPointersDependencies )

    # RX Link Filters
    rxLinkFilters = macComponent.createMenuSymbol( None, None )
    rxLinkFilters.setLabel( "RX Link Filters" )

    # Accept broadcast packets
    linkAcceptBcast = macComponent.createBooleanSymbol( "LINK_ACCEPT_BCAST", rxLinkFilters )
    linkAcceptBcast.setLabel( "Accept broadcast packets" )
    linkAcceptBcast.setDefaultValue( True )

    # Accept multicast packets
    linkAcceptMcast = macComponent.createBooleanSymbol( "LINK_ACCEPT_MCAST", rxLinkFilters )
    linkAcceptMcast.setLabel( "Accept multicast packets" )
    linkAcceptMcast.setDefaultValue( True )

    # Accept unicast packets
    linkAcceptUcast = macComponent.createBooleanSymbol( "LINK_ACCEPT_UCAST", rxLinkFilters )
    linkAcceptUcast.setLabel( "Accept unicast packets" )
    linkAcceptUcast.setDefaultValue( True )

    # Accept multicast packets matching hash
    linkAcceptMcastHash = macComponent.createBooleanSymbol( "LINK_ACCEPT_MCAST_HASH", rxLinkFilters )
    linkAcceptMcastHash.setLabel( "Accept multicast packets matching hash" )
    linkAcceptMcastHash.setDefaultValue( False )

    # Accept unicast packets matching hash
    linkAcceptUcastHash = macComponent.createBooleanSymbol( "LINK_ACCEPT_UCAST_HASH", rxLinkFilters )
    linkAcceptUcastHash.setLabel( "Accept unicast packets matching hash" )
    linkAcceptUcastHash.setDefaultValue( False )

    # Reject packets with wrong CRC
    linkRejectCrcError = macComponent.createBooleanSymbol( "LINK_REJECT_CRC_ERROR", rxLinkFilters )
    linkRejectCrcError.setLabel( "Reject packets with wrong CRC" )
    linkRejectCrcError.setDefaultValue( True )

    # Accept packets with wrong CRC
    linkAcceptCrcError = macComponent.createBooleanSymbol( "LINK_ACCEPT_CRC_ERROR", rxLinkFilters )
    linkAcceptCrcError.setLabel( "Accept packets with wrong CRC" )
    linkAcceptCrcError.setDefaultValue( False )

    # Accept maximum frame size packets
    linkAcceptMaxFrameSizePackets = macComponent.createBooleanSymbol( "LINK_ACCEPT_MAX_FRAME_SIZE_PACKETS", rxLinkFilters )
    linkAcceptMaxFrameSizePackets.setLabel( "Accept maximum frame size packets" )
    linkAcceptMaxFrameSizePackets.setDescription( "Maximum frame size = " + str( MAX_FRAME_SIZE ) + " bytes" )
    linkAcceptMaxFrameSizePackets.setDefaultValue( False )

    # Accept packets with frame error
    linkAcceptFrameError = macComponent.createBooleanSymbol( "LINK_ACCEPT_FRAME_ERROR", rxLinkFilters )
    linkAcceptFrameError.setLabel( "Accept packets with frame error" )
    linkAcceptFrameError.setDefaultValue( False )

    # Accept jumbo packets
    linkAcceptJumboFrame = macComponent.createBooleanSymbol( "LINK_ACCEPT_JUMBO_FRAME", rxLinkFilters )
    linkAcceptJumboFrame.setLabel( "Accept jumbo packets" )
    linkAcceptJumboFrame.setDescription( "Maximum jumbo packet size = " + str( MAX_JUMBO_PACKET_SIZE ) + " bytes" )
    linkAcceptJumboFrame.setDefaultValue( False )

    # Promiscuous mode enable
    linkAcceptAll = macComponent.createBooleanSymbol( "LINK_ACCEPT_ALL", rxLinkFilters )
    linkAcceptAll.setLabel( "Promiscuous mode enable" )
    linkAcceptAll.setDefaultValue( False )

    # Link Connection
    linkConnection = macComponent.createMenuSymbol( None, None )
    linkConnection.setLabel( "Link Connection" )

    # Auto negotiation enable
    linkAutoNegotiateEnableName = "LINK_AUTO_NEGOTIATION"
    linkAutoNegotiateEnable = macComponent.createBooleanSymbol( linkAutoNegotiateEnableName, linkConnection )
    linkAutoNegotiateEnable.setLabel( "Auto negotiation enable" )
    linkAutoNegotiateEnable.setDefaultValue( True )
    #### Auto negotiation options
    # Use auto full duplex
    linkAutoFullDuplex = macComponent.createBooleanSymbol( "LINK_AUTO_FULL_DUPLEX", linkAutoNegotiateEnable )
    linkAutoFullDuplex.setLabel( "Allow full duplex" )
    linkAutoFullDuplex.setDefaultValue( True )
    linkAutoFullDuplex.setVisible( linkAutoNegotiateEnable.getValue() )
    linkAutoFullDuplex.setDependencies( setVisibleMatching, [linkAutoNegotiateEnableName] )

    # Allow half duplex
    linkAutoHalfDuplex = macComponent.createBooleanSymbol( "LINK_AUTO_HALF_DUPLEX", linkAutoNegotiateEnable )
    linkAutoHalfDuplex.setLabel( "Allow half duplex" )
    linkAutoHalfDuplex.setDefaultValue( True )
    linkAutoHalfDuplex.setVisible( linkAutoNegotiateEnable.getValue() )
    linkAutoHalfDuplex.setDependencies( setVisibleMatching, [linkAutoNegotiateEnableName] )

    # Allow 100Mbps
    linkAuto100 = macComponent.createBooleanSymbol( "LINK_AUTO_100", linkAutoNegotiateEnable )
    linkAuto100.setLabel( "Allow 100Mbps" )
    linkAuto100.setDefaultValue( True )
    linkAuto100.setVisible( linkAutoNegotiateEnable.getValue() )
    linkAuto100.setDependencies( setVisibleMatching, [linkAutoNegotiateEnableName] )

    # Allow 10Mbps
    linkAuto10 = macComponent.createBooleanSymbol( "LINK_AUTO_10", linkAutoNegotiateEnable )
    linkAuto10.setLabel( "Allow 10Mbps" )
    linkAuto10.setDefaultValue( True )
    linkAuto10.setVisible( linkAutoNegotiateEnable.getValue() )
    linkAuto10.setDependencies( setVisibleMatching, [linkAutoNegotiateEnableName] )

    #### Manual link choices
    linkChoices = [
        ( "Full_100", "0x00", "Full Duplex, 100 Mbps" ),
        ( "Full_10",  "0x01", "Full Duplex, 10  Mbps" ),
        ( "Half_100", "0x02", "Half Duplex, 100 Mbps" ),
        ( "Half_10",  "0x03", "Half Duplex, 10  Mbps" ),
    ]
    linkManualChoice = macComponent.createKeyValueSetSymbol( "LINK_MANUAL_CHOICE", linkConnection )
    linkManualChoice.setLabel( "Manual link type" )
    for tupleElem in linkChoices:
        linkManualChoice.addKey( tupleElem[ 0 ], tupleElem[ 1 ], tupleElem[ 2 ] )
    linkManualChoice.setOutputMode( "Value" )
    linkManualChoice.setDisplayMode( "Description" )
    linkManualChoice.setDescription( "Used when auto-negotiation is not enabled" )
    linkManualChoice.setDefaultValue( 0 )
    linkManualChoice.setVisible( not linkAutoNegotiateEnable.getValue() )
    linkManualChoice.setDependencies( setVisibleNotMatching, [linkAutoNegotiateEnableName] )
    #####

    # Allow huge packets
    allowHugePackets = macComponent.createBooleanSymbol( "ALLOW_HUGE_PACKETS", linkConnection )
    allowHugePackets.setLabel( "Allow huge packets" )
    allowHugePackets.setDefaultValue( False )

    # MDIX auto enable
    mdixAutoEnable = macComponent.createBooleanSymbol( "MDIX_AUTO_ENABLE", linkConnection )
    mdixAutoEnable.setLabel( "MDIX auto enable" )
    mdixAutoEnable.setDefaultValue( True )

    # MDIX swap
    mdixSwap = macComponent.createBooleanSymbol( "MDIX_SWAP", linkConnection )
    mdixSwap.setLabel( "MDIX swapped" )
    mdixSwap.setDescription( "Only used when MDIX auto is disabled" )
    mdixSwap.setDefaultValue( False )

    # MAC level loop back
    macLoopBack = macComponent.createBooleanSymbol( "MAC_LOOPBACK", linkConnection )
    macLoopBack.setLabel( "MAC level loop back" )
    macLoopBack.setDefaultValue( False )

    # PHY level loop back
    phyLoopBack = macComponent.createBooleanSymbol( "PHY_LOOPBACK", linkConnection )
    phyLoopBack.setLabel( "PHY level loop back" )
    phyLoopBack.setDefaultValue( False )

    # EXTERNAL PHY INFO
    # Use RMII Connection to PHY
    tcpipMacEthRmii = macComponent.createBooleanSymbol( "USE_RMII", None )
    tcpipMacEthRmii.setLabel( "Use RMII" )
    tcpipMacEthRmii.setDefaultValue( True )
    if atdfMacInstanceName == 'EMAC1':
        tcpipMacEthRmii.setReadOnly( True )         # Emac1 only supports RMII
    tcpipMacEthRmii.setDependencies( sendRmiiUpdate, ["USE_RMII"] )

    # RMII Mask Compare Value
    rmiiMskCmpValue = macComponent.createIntegerSymbol( "MAC_RMII_VALUE", driverInfoMenu )
    rmiiMskCmpValue.setVisible( False )
    rmiiMskCmpValue.setDefaultValue( getRmiiMskCmpValueFor( tcpipMacEthRmii.getValue() ) )

    # PHY Connected to MAC
    phyType = macComponent.createStringSymbol( "DRV_MAC_PHY_TYPE", None )
    phyType.setLabel( "External PHY Device" )
    phyType.setDefaultValue( "" )
    phyType.setReadOnly( True )

    ## File Generation and deployment #########################################
    macFName =              atdfMacModule.lower()
    basePath =              "/driver/" + macFName + "/"
    basePathSrc =           basePath + "src/"
    basePathSrcDynamic =    basePathSrc + "dynamic/"
    cfgPath =               basePath + "/config/"
    prjPath =               cfgPath + Variables.get( "__CONFIGURATION_NAME" ) + basePath
    prjPathSrc =            prjPath + "src/"
    prjPathSrcDynamic =     prjPathSrc + "dynamic/"
    macLibDeviceCore =      getMacDeviceCoreName()

    # Add to system_config.h
    headerFileFtl = macComponent.createFileSymbol( None, None )
    headerFileFtl.setSourcePath( cfgPath + "mac_drvr_configuration.h.ftl" )
    headerFileFtl.setOutputName( "core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION" )
    headerFileFtl.setMarkup( True )
    headerFileFtl.setType( "STRING" )

    # file mac_drvr.h
    headerFile = macComponent.createFileSymbol( None, None )
    headerFile.setSourcePath( basePathSrc + "mac_drvr.h" )
    headerFile.setOutputName( "mac_drvr.h" )
    headerFile.setDestPath( basePath )
    headerFile.setProjectPath( prjPath )
    headerFile.setType( "HEADER" )
    headerFile.setOverwrite( True )

    # file TCPIP_MAC_LOCAL_H
    localHeaderFile = macComponent.createFileSymbol( None, None )
    localHeaderFile.setSourcePath( basePathSrc + "mac_drvr_local.h" )
    localHeaderFile.setOutputName( "mac_drvr_local.h" )
    localHeaderFile.setDestPath( basePathSrc )
    localHeaderFile.setProjectPath( prjPathSrc )
    localHeaderFile.setType( "HEADER" )
    localHeaderFile.setOverwrite( True )

    dynamicFiles = [
        [ "mac_dcpt_lists.h",                       "HEADER" ],
        [ "mac_drvr_lib.h",                         "HEADER" ],
        [ "mac_drvr.c",                             "SOURCE" ],
        [ "mac_drvr_statistics.c",                  "SOURCE" ],
        [ "mac_drvr_lib_" + macLibDeviceCore + ".c","SOURCE" ],
    ]
    for ff in dynamicFiles:
        fileSymbol = macComponent.createFileSymbol( None, None )
        fileSymbol.setSourcePath( basePathSrcDynamic + ff[ 0 ] )
        fileSymbol.setOutputName( ff[ 0 ] )
        fileSymbol.setDestPath( basePathSrcDynamic )
        fileSymbol.setProjectPath( prjPathSrcDynamic )
        fileSymbol.setType( ff[ 1 ] )
        fileSymbol.setOverwrite( True )
