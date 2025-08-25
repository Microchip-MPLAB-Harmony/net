"""
Copyright (C) 2019-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
"""
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
interfaceNum = []

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
    global tcpipMacEthRmii
    interface_number = 0
    if((source["id"] == "MAC_PHY_Dependency" + str( userMacChoiceId )) or  ((userMacChoiceId == 0) and (source["id"] == "MAC_PHY_Dependency"))):
        phyComponent = target[ 'component' ].getID()
        Database.setSymbolValue( macComponentId, "DRV_MAC_PHY_TYPE", target['component'].getDisplayName() )
        extPhyComponent = "drvExtPhy" + target['component'].getDisplayName().capitalize()
        setVal(extPhyComponent, "DRV_ETHPHY_MAC_NAME", atdfMacInstanceName)
        setVal(extPhyComponent, "DRV_ETHPHY_PERIPHERAL_ID", atdfMacInstanceName + "_BASE_ADDRESS") 
    elif (target["id"] == "NETCONFIG_MAC_Dependency"):
        interface_number = int(target["component"].getID().strip("tcpipNetConfig_"))
        interfaceNum.append(interface_number)
        setVal("tcpipStack", "TCPIP_STACK_INT_MAC_IDX" + str(interface_number), True)
        incVal("tcpipStack", "TCPIP_STACK_INTMAC_INTERFACE_NUM")
        setVal("tcpipStack", "TCPIP_STACK_MII_MODE_IDX" + str(interface_number), "RMII" if tcpipMacEthRmii.getValue() == True else "MII")

def onAttachmentDisconnected( source, target ):
    global userMacChoiceId
    global macComponentId

    if((source['id'] == "MAC_PHY_Dependency" + str( userMacChoiceId )) or  ((userMacChoiceId == 0) and (source['id'] == "MAC_PHY_Dependency"))):
        phyComponent = ""
        Database.clearSymbolValue( macComponentId, 'DRV_MAC_PHY_TYPE' )
        extPhyComponent = "drvExtPhy" + target['component'].getDisplayName().capitalize()
        setVal(extPhyComponent, "DRV_ETHPHY_MAC_NAME", "")
        setVal(extPhyComponent, "DRV_ETHPHY_PERIPHERAL_ID", "")
    elif (target["id"] == "NETCONFIG_MAC_Dependency"):
        interface_number = int(target["component"].getID().strip("tcpipNetConfig_"))
        interfaceNum.remove(interface_number)
        setVal("tcpipStack", "TCPIP_STACK_INT_MAC_IDX" + str(interface_number), False)
        decVal("tcpipStack", "TCPIP_STACK_INTMAC_INTERFACE_NUM")
        setVal("tcpipStack", "TCPIP_STACK_MII_MODE_IDX" + str(interface_number), "")


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

    for interface in range (0, len(interfaceNum)): 
        setVal("tcpipStack", "TCPIP_STACK_MII_MODE_IDX" + str(interfaceNum[interface]), "RMII" if event["value"] == True else "MII")
    # if len( phyComponent ):
        # Database.sendMessage( phyComponent, 'MII_CHANGE',
                            # {   'PhyComponent':phyComponent,
                                # 'UseRmii':event[ 'value' ],
                            # }
                        # )


def setVisibleMatching( symbol, event ):
    symbol.setVisible( event["value"] )


def setVisibleNotMatching( symbol, event ):
    symbol.setVisible( not event["value"] )


def destroyComponent( macComponent ):
    # This keeps tcpip_configurator_driver.py code in sync with this object existence
    sendStateChangeMessage( 'tcpip_driver_config', 'MAC_STATE_DESTROYED' )
    setVal("core", atdfMacInstanceName + "_INTERRUPT_ENABLE", False)



def finalizeComponent( macComponent ):
    # This keeps tcpip_configurator_driver.py code in sync with this object existence
    sendStateChangeMessage( 'tcpip_driver_config', 'MAC_STATE_IN_EXISTANCE' )
    if( None != Database.getSymbolValue( 'tcpipNetConfig_0', 'TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX0' ) ):
        sendStateChangeMessage( 'tcpipNetConfig_0', 'MAC_STATE_IN_EXISTANCE' )


def sendStateChangeMessage( aRecipient, aState ):
    global macComponentId
    global macBaseAddressValue
    # return Database.sendMessage( aRecipient, 'MAC_STATE_CHANGE',
                                    # {   'ComponentId':macComponentId,
                                        # 'ChoiceId':userMacChoiceId,
                                        # 'MacBaseAddress':macBaseAddressValue,
                                        # 'DefaultMacDriverCode':defaultMacDriverCode,
                                        # 'State':aState,
                                    # }
                                # )

def txDescriptorCountCallBack(symbol, event):
    if (event["value"] == "Low"):     
        symbol.setValue(40)
    elif(event["value"] == "Medium"):   
        symbol.setValue(80)
    else:
        symbol.setValue(160)
        
def rxDescriptorCountCallBack(symbol, event):
    if (event["value"] == "Low"):     
        symbol.setValue(50)
    elif(event["value"] == "Medium"):   
        symbol.setValue(100)
    else:
        symbol.setValue(256)
        
def rxStaticBuffersCallBack(symbol, event):
    if (event["value"] == "Low"):     
        symbol.setValue(0)
    elif(event["value"] == "Medium"):   
        symbol.setValue(0)
    else:
        symbol.setValue(10)
        
def rxBufferAllocationThresholdCallBack(symbol, event):
    if (event["value"] == "Low"):     
        symbol.setValue(15)
    elif(event["value"] == "Medium"):   
        symbol.setValue(15)
    else:
        symbol.setValue(30)
        
def rxBufferAllocationCallBack(symbol, event):
    if (event["value"] == "Low"):     
        symbol.setValue(15)
    elif(event["value"] == "Medium"):   
        symbol.setValue(30)
    else:
        symbol.setValue(45)

def emacClockUpdate(symbol, event): 
    setVal("tcpipStack", "TCPIP_STACK_MAC_CLOCK", event["value"])

def emacHeapCalc():
    global macComponentId
    rx_static_buffers = Database.getSymbolValue( macComponentId, 'RX_STATIC_BUFFERS_0' )
    rx_alloc_count = Database.getSymbolValue( macComponentId, 'RX_BUFFER_ALLOCATION_0' )
    rx_alloc_thres = Database.getSymbolValue( macComponentId, 'RX_BUFFER_ALLOCATION_THRESHOLD_0' )
    rx_buffer_size = Database.getSymbolValue( macComponentId, 'RX_BUFFER_SIZE_0' )
    
    heapsize = (rx_static_buffers + rx_alloc_count + rx_alloc_thres) * rx_buffer_size
    return heapsize
 
def drvEmacHeapUpdate(symbol, event): 
    heap_size = emacHeapCalc()
    symbol.setValue(heap_size)
    if(event["id"] == "TCPIP_STACK_HEAP_CALC_MASK"):
        symbol.setVisible(event["value"])


#Set symbols of other components
def setVal(component, symbol, value):
    triggerDict = {"Component":component,"Id":symbol, "Value":value}
    if(Database.sendMessage(component, "SET_SYMBOL", triggerDict) == None):
        print "Set Symbol Failure" + component + ":" + symbol + ":" + str(value)
        return False
    else:
        return True

#Increment symbols of other components
def incVal(component, symbol):
    triggerDict = {"Component":component,"Id":symbol}
    if(Database.sendMessage(component, "INC_SYMBOL", triggerDict) == None):
        print "Increment Symbol Failure" + component + ":" + symbol
        return False
    else:
        return True

#Increment symbols of other components
def decVal(component, symbol):
    triggerDict = {"Component":component,"Id":symbol}
    if(Database.sendMessage(component, "DEC_SYMBOL", triggerDict) == None):
        print "Decrement Symbol Failure" + component + ":" + symbol
        return False
    else:
        return True
        
#Handle messages from other components
def handleMessage(messageID, args):
    retDict= {}
    if (messageID == "SET_SYMBOL"):
        print "handleMessage: Set Symbol"
        retDict= {"Return": "Success"}
        Database.setSymbolValue(args["Component"], args["Id"], args["Value"])
    else:
        retDict= {"Return": "UnImplemented Command"}
    return retDict

def instantiateComponent( macComponent ):
    global atdfMacInstanceName
    global macComponentId
    global macBaseAddressValue
    global defaultMacDriverCode
    global showSuperfluous
    global tcpipMacEthRmii

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
    macBaseAddress.setHelp("mcc_h3_emac_configurations")
    macBaseAddress.setLabel( "MAC Base Address" )
    macBaseAddress.setDefaultValue( macBaseAddressValue )
    macBaseAddress.setReadOnly( True )

    # MAC Network Driver -- for export to NETCONFIG
    driverMacCode = macComponent.createStringSymbol( "DRV_MAC_CODE", driverInfoMenu )
    driverMacCode.setHelp("mcc_h3_emac_configurations")
    driverMacCode.setLabel( "Default Network MAC Driver" )
    driverMacCode.setDefaultValue( defaultMacDriverCode )
    driverMacCode.setReadOnly( True )

    # MAC Interrupt Source
    interruptSource = macComponent.createStringSymbol( "MAC_INTERRUPT_SOURCE", driverInfoMenu )
    interruptSource.setHelp("mcc_h3_emac_configurations")
    interruptSource.setLabel( "Interrupt Source" )
    interruptSource.setDefaultValue( atdfMacInstanceName + "_IRQn" )
    interruptSource.setReadOnly( True )
    interruptSetHandler( True )
    interruptSource.setDependencies( interruptControl, ["MAC_INTERRUPT_SOURCE"] )
    intIndex = int(atdfMacInstanceName.strip("EMAC"))
    setVal("tcpipStack", "TCPIP_STACK_INTERRUPT_EN_IDX" + str(intIndex), True)

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
            queueEnable0Alternative = macComponent.createMenuSymbol( "QUEUE_ENABLE_0_ALTERNATIVE", None )
            queueEnable0Alternative.setLabel( "Queue Info" )
            queueEnable0Alternative.setVisible( False )
            if queueCountValue == 1:
                queueEnable.setVisible( False )
                queueEnable0Alternative.setVisible( True )
                queueMenu = queueEnable0Alternative
                
        emacTraffic = macComponent.createComboSymbol("TCPIP_EMAC_TRAFFIC",queueMenu,["Low", "Medium", "High"])
        emacTraffic.setHelp("mcc_h3_emac_configurations")
        emacTraffic.setVisible(True)
        emacTraffic.setLabel("MAC Default Configuration for Network Traffic")
        emacTraffic.setDefaultValue("Medium")
    
        # TX descriptors count
        txDescriptorCount = macComponent.createIntegerSymbol( "TX_DESCRIPTOR_COUNT_" + qqStr, emacTraffic )
        txDescriptorCount.setHelp("mcc_h3_emac_configurations")
        txDescriptorCount.setLabel( "TX descriptor count" )
        txDescriptorCount.setMax( TX_DEVICE_MAX_DESCRIPTORS )
        txDescriptorCount.setDefaultValue(80)
        txDescriptorCount.setVisible( queueEnable.getValue() )
        txDescriptorCount.setDependencies(txDescriptorCountCallBack, ["TCPIP_EMAC_TRAFFIC"])
        

        # TX buffer size
        txBufferSize = macComponent.createIntegerSymbol( "TX_BUFFER_SIZE_" + qqStr, emacTraffic )
        txBufferSize.setHelp("mcc_h3_emac_configurations")
        txBufferSize.setLabel( "TX buffer size" )
        txBufferSize.setDescription( "Should be a multiple of 16" )
        txBufferSize.setMin( MIN_TX_BUFFER_SIZE )
        txBufferSize.setMax( MAX_TX_BUFFER_SIZE )
        txBufferSize.setDefaultValue( MAX_FRAME_SIZE if qq == 0 else MIN_TX_BUFFER_SIZE )
        txBufferSize.setVisible( queueEnable.getValue() )        

        # RX buffer size
        rxBufferSize = macComponent.createIntegerSymbol( "RX_BUFFER_SIZE_" + qqStr, emacTraffic )
        rxBufferSize.setHelp("mcc_h3_emac_configurations")
        rxBufferSize.setLabel( "RX buffer size" )
        rxBufferSize.setDescription( "Should be a multiple of 16" )
        rxBufferSize.setMin( MIN_RX_BUFFER_SIZE )
        rxBufferSize.setMax( MAX_RX_BUFFER_SIZE )
        rxBufferSize.setDefaultValue( MAX_RX_BUFFER_SIZE if qq == 0 else MIN_RX_BUFFER_SIZE )
        rxBufferSize.setVisible( queueEnable.getValue() )
        rxBufferSize.setReadOnly( True )

        # RX descriptors count
        rxDescriptorCountName = "RX_DESCRIPTOR_COUNT_" + qqStr
        rxDescriptorCount = macComponent.createIntegerSymbol( rxDescriptorCountName, emacTraffic )
        rxDescriptorCount.setHelp("mcc_h3_emac_configurations")
        rxDescriptorCount.setLabel( "RX descriptor count" )
        rxDescriptorCount.setDescription( "Dictates the minimum buffer total -- static and dynamic buffers" )
        rxDescriptorCount.setMin( MIN_RX_DESCRIPTOR_COUNT )
        rxDescriptorCount.setMax( RX_DEVICE_MAX_DESCRIPTORS )
        rxDescriptorCount.setDefaultValue( 100 )
        rxDescriptorCount.setVisible( queueEnable.getValue() )
        rxDescriptorCount.setDependencies(rxDescriptorCountCallBack, ["TCPIP_EMAC_TRAFFIC"])
        if rxDescriptorCount.getValue() > largestRxQueueDescriptorCount:
            largestRxQueueDescriptorCount = rxDescriptorCount.getValue()
        maxRxPacketPointersDependencies.append( rxDescriptorCountName )

        # RX static buffers
        rxStaticBuffersName = "RX_STATIC_BUFFERS_" + qqStr
        rxStaticBuffers = macComponent.createIntegerSymbol( rxStaticBuffersName, emacTraffic )
        rxStaticBuffers.setHelp("mcc_h3_emac_configurations")
        rxStaticBuffers.setLabel( "RX static buffers" )
        rxStaticBuffers.setDescription( "RX buffers statically allocated" )
        rxStaticBuffers.setMin( 0 )
        rxStaticBuffers.setDefaultValue( 0 )
        rxStaticBuffers.setVisible( queueEnable.getValue() )
        rxStaticBuffers.setDependencies(rxStaticBuffersCallBack, ["TCPIP_EMAC_TRAFFIC"])

        # RX dynamic allocation threshold
        rxBufferAllocationThresholdName = "RX_BUFFER_ALLOCATION_THRESHOLD_" + qqStr
        rxBufferAllocationThreshold = macComponent.createIntegerSymbol( rxBufferAllocationThresholdName, emacTraffic )
        rxBufferAllocationThreshold.setHelp("mcc_h3_emac_configurations")
        rxBufferAllocationThreshold.setLabel( "RX dynamic allocation threshold" )
        rxBufferAllocationThreshold.setDefaultValue( 15)
        rxBufferAllocationThreshold.setMin( 0 )
        rxBufferAllocationThreshold.setVisible( queueEnable.getValue() )
        rxBufferAllocationThreshold.setDependencies(rxBufferAllocationThresholdCallBack, ["TCPIP_EMAC_TRAFFIC"])

        # RX dynamic buffer allocation
        rxBufferAllocationName = "RX_BUFFER_ALLOCATION_" + qqStr
        rxBufferAllocation = macComponent.createIntegerSymbol( rxBufferAllocationName, emacTraffic )
        rxBufferAllocation.setHelp("mcc_h3_emac_configurations")
        rxBufferAllocation.setLabel( "RX dynamic buffer allocation" )
        rxBufferAllocation.setMin( 1 )
        rxBufferAllocation.setDefaultValue( 30 )
        rxBufferAllocation.setVisible( queueEnable.getValue() )
        rxBufferAllocation.setDependencies(rxBufferAllocationCallBack, ["TCPIP_EMAC_TRAFFIC"])

    maxRxPacketPointers = macComponent.createIntegerSymbol( "MAX_RX_PACKET_POINTERS", None )
    maxRxPacketPointers.setLabel( "Max RX Packet Pointers" )
    maxRxPacketPointers.setDescription( "Max handle count expected for RX descriptors" )
    maxRxPacketPointers.setVisible( False )
    maxRxPacketPointers.setValue( largestRxQueueDescriptorCount )
    maxRxPacketPointers.setDependencies( maxRxPacketPointerUpdate, maxRxPacketPointersDependencies )

    # RX Link Filters
    rxLinkFilters = macComponent.createMenuSymbol( None, None )
    rxLinkFilters.setHelp("mcc_h3_emac_configurations")
    rxLinkFilters.setLabel( "RX Link Filters" )

    # Accept broadcast packets
    linkAcceptBcast = macComponent.createBooleanSymbol( "LINK_ACCEPT_BCAST", rxLinkFilters )
    linkAcceptBcast.setHelp("mcc_h3_emac_configurations")
    linkAcceptBcast.setLabel( "Accept broadcast packets" )
    linkAcceptBcast.setDefaultValue( True )

    # Accept multicast packets
    linkAcceptMcast = macComponent.createBooleanSymbol( "LINK_ACCEPT_MCAST", rxLinkFilters )
    linkAcceptMcast.setHelp("mcc_h3_emac_configurations")
    linkAcceptMcast.setLabel( "Accept multicast packets" )
    linkAcceptMcast.setDefaultValue( True )

    # Accept unicast packets
    linkAcceptUcast = macComponent.createBooleanSymbol( "LINK_ACCEPT_UCAST", rxLinkFilters )
    linkAcceptUcast.setHelp("mcc_h3_emac_configurations")
    linkAcceptUcast.setLabel( "Accept unicast packets" )
    linkAcceptUcast.setDefaultValue( True )

    # Accept multicast packets matching hash
    linkAcceptMcastHash = macComponent.createBooleanSymbol( "LINK_ACCEPT_MCAST_HASH", rxLinkFilters )
    linkAcceptMcastHash.setHelp("mcc_h3_emac_configurations")
    linkAcceptMcastHash.setLabel( "Accept multicast packets matching hash" )
    linkAcceptMcastHash.setDefaultValue( False )

    # Accept unicast packets matching hash
    linkAcceptUcastHash = macComponent.createBooleanSymbol( "LINK_ACCEPT_UCAST_HASH", rxLinkFilters )
    linkAcceptUcastHash.setHelp("mcc_h3_emac_configurations")
    linkAcceptUcastHash.setLabel( "Accept unicast packets matching hash" )
    linkAcceptUcastHash.setDefaultValue( False )

    # Reject packets with wrong CRC
    linkRejectCrcError = macComponent.createBooleanSymbol( "LINK_REJECT_CRC_ERROR", rxLinkFilters )
    linkRejectCrcError.setHelp("mcc_h3_emac_configurations")
    linkRejectCrcError.setLabel( "Reject packets with wrong CRC" )
    linkRejectCrcError.setDefaultValue( True )

    # Accept packets with wrong CRC
    linkAcceptCrcError = macComponent.createBooleanSymbol( "LINK_ACCEPT_CRC_ERROR", rxLinkFilters )
    linkAcceptCrcError.setHelp("mcc_h3_emac_configurations")
    linkAcceptCrcError.setLabel( "Accept packets with wrong CRC" )
    linkAcceptCrcError.setDefaultValue( False )

    # Accept maximum frame size packets
    linkAcceptMaxFrameSizePackets = macComponent.createBooleanSymbol( "LINK_ACCEPT_MAX_FRAME_SIZE_PACKETS", rxLinkFilters )
    linkAcceptMaxFrameSizePackets.setHelp("mcc_h3_emac_configurations")
    linkAcceptMaxFrameSizePackets.setLabel( "Accept maximum frame size packets" )
    linkAcceptMaxFrameSizePackets.setDescription( "Maximum frame size = " + str( MAX_FRAME_SIZE ) + " bytes" )
    linkAcceptMaxFrameSizePackets.setDefaultValue( False )

    # Accept packets with frame error
    linkAcceptFrameError = macComponent.createBooleanSymbol( "LINK_ACCEPT_FRAME_ERROR", rxLinkFilters )
    linkAcceptFrameError.setHelp("mcc_h3_emac_configurations")
    linkAcceptFrameError.setLabel( "Accept packets with frame error" )
    linkAcceptFrameError.setDefaultValue( False )

    # Accept jumbo packets
    linkAcceptJumboFrame = macComponent.createBooleanSymbol( "LINK_ACCEPT_JUMBO_FRAME", rxLinkFilters )
    linkAcceptJumboFrame.setHelp("mcc_h3_emac_configurations")
    linkAcceptJumboFrame.setLabel( "Accept jumbo packets" )
    linkAcceptJumboFrame.setDescription( "Maximum jumbo packet size = " + str( MAX_JUMBO_PACKET_SIZE ) + " bytes" )
    linkAcceptJumboFrame.setDefaultValue( False )

    # Promiscuous mode enable
    linkAcceptAll = macComponent.createBooleanSymbol( "LINK_ACCEPT_ALL", rxLinkFilters )
    linkAcceptAll.setHelp("mcc_h3_emac_configurations")
    linkAcceptAll.setLabel( "Promiscuous mode enable" )
    linkAcceptAll.setDefaultValue( False )

    # Link Connection
    linkConnection = macComponent.createMenuSymbol( None, None )
    linkConnection.setLabel( "Link Connection" )

    # Auto negotiation enable
    linkAutoNegotiateEnableName = "LINK_AUTO_NEGOTIATION"
    linkAutoNegotiateEnable = macComponent.createBooleanSymbol( linkAutoNegotiateEnableName, linkConnection )
    linkAutoNegotiateEnable.setHelp("mcc_h3_emac_configurations")
    linkAutoNegotiateEnable.setLabel( "Auto negotiation enable" )
    linkAutoNegotiateEnable.setDefaultValue( True )
    #### Auto negotiation options
    # Use auto full duplex
    linkAutoFullDuplex = macComponent.createBooleanSymbol( "LINK_AUTO_FULL_DUPLEX", linkAutoNegotiateEnable )
    linkAutoFullDuplex.setHelp("mcc_h3_emac_configurations")
    linkAutoFullDuplex.setLabel( "Allow full duplex" )
    linkAutoFullDuplex.setDefaultValue( True )
    linkAutoFullDuplex.setVisible( linkAutoNegotiateEnable.getValue() )
    linkAutoFullDuplex.setDependencies( setVisibleMatching, [linkAutoNegotiateEnableName] )

    # Allow half duplex
    linkAutoHalfDuplex = macComponent.createBooleanSymbol( "LINK_AUTO_HALF_DUPLEX", linkAutoNegotiateEnable )
    linkAutoHalfDuplex.setHelp("mcc_h3_emac_configurations")
    linkAutoHalfDuplex.setLabel( "Allow half duplex" )
    linkAutoHalfDuplex.setDefaultValue( True )
    linkAutoHalfDuplex.setVisible( linkAutoNegotiateEnable.getValue() )
    linkAutoHalfDuplex.setDependencies( setVisibleMatching, [linkAutoNegotiateEnableName] )

    # Allow 100Mbps
    linkAuto100 = macComponent.createBooleanSymbol( "LINK_AUTO_100", linkAutoNegotiateEnable )
    linkAuto100.setHelp("mcc_h3_emac_configurations")
    linkAuto100.setLabel( "Allow 100Mbps" )
    linkAuto100.setDefaultValue( True )
    linkAuto100.setVisible( linkAutoNegotiateEnable.getValue() )
    linkAuto100.setDependencies( setVisibleMatching, [linkAutoNegotiateEnableName] )

    # Allow 10Mbps
    linkAuto10 = macComponent.createBooleanSymbol( "LINK_AUTO_10", linkAutoNegotiateEnable )
    linkAuto10.setHelp("mcc_h3_emac_configurations")
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
    linkManualChoice.setHelp("mcc_h3_emac_configurations")
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
    allowHugePackets.setHelp("mcc_h3_emac_configurations")
    allowHugePackets.setLabel( "Allow huge packets" )
    allowHugePackets.setDefaultValue( False )

    # MDIX auto enable
    mdixAutoEnable = macComponent.createBooleanSymbol( "MDIX_AUTO_ENABLE", linkConnection )
    mdixAutoEnable.setHelp("mcc_h3_emac_configurations")
    mdixAutoEnable.setLabel( "MDIX auto enable" )
    mdixAutoEnable.setDefaultValue( True )

    # MDIX swap
    mdixSwap = macComponent.createBooleanSymbol( "MDIX_SWAP", linkConnection )
    mdixSwap.setHelp("mcc_h3_emac_configurations")
    mdixSwap.setLabel( "MDIX swapped" )
    mdixSwap.setDescription( "Only used when MDIX auto is disabled" )
    mdixSwap.setDefaultValue( False )

    # MAC level loop back
    macLoopBack = macComponent.createBooleanSymbol( "MAC_LOOPBACK", linkConnection )
    macLoopBack.setHelp("mcc_h3_emac_configurations")
    macLoopBack.setLabel( "MAC level loop back" )
    macLoopBack.setDefaultValue( False )

    # PHY level loop back
    phyLoopBack = macComponent.createBooleanSymbol( "PHY_LOOPBACK", linkConnection )
    phyLoopBack.setHelp("mcc_h3_emac_configurations")
    phyLoopBack.setLabel( "PHY level loop back" )
    phyLoopBack.setDefaultValue( False )

    # EXTERNAL PHY INFO
    # Use RMII Connection to PHY
    tcpipMacEthRmii = macComponent.createBooleanSymbol( "USE_RMII", linkConnection )
    tcpipMacEthRmii.setHelp("mcc_h3_emac_configurations")
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
    phyType = macComponent.createStringSymbol( "DRV_MAC_PHY_TYPE", linkConnection )
    phyType.setHelp("mcc_h3_emac_configurations")
    phyType.setLabel( "External PHY Device" )
    phyType.setDefaultValue( "" )
    phyType.setReadOnly( True )
    
    # Multiple Client Support
    tcpipEmacMultiCliEnable = macComponent.createBooleanSymbol("DRV_EMAC_MULTI_CLIENT", None)
    tcpipEmacMultiCliEnable.setHelp("mcc_h3_emac_configurations")
    tcpipEmacMultiCliEnable.setLabel("Enable Multiple EMAC Clients")
    tcpipEmacMultiCliEnable.setDefaultValue(False) 
    
    # Pin Settings
    tcpipEmacPinConfig = macComponent.createMenuSymbol("TCPIP_EMAC_PIN_CONFIG", None)
    tcpipEmacPinConfig.setLabel("Pin Configurations")
    tcpipEmacPinConfig.setDescription("Pin Configurations for Internal Ethernet MAC")
    tcpipEmacPinConfig.setVisible(True)
    
    # Comment for MII Pin Configuration
    tcpipEmacMIIPinConfig = macComponent.createCommentSymbol("TCPIP_EMAC_MII_PIN_CONFIG", tcpipEmacPinConfig)
    tcpipEmacMIIPinConfig.setLabel("*** Configure "+ atdfMacInstanceName + " MII/RMII pins from Plugins -> Pin Configuration")
    tcpipEmacMIIPinConfig.setVisible(True)
    
    # Comment for MDIO Pin Configuration
    tcpipEmacMDIOPinConfig = macComponent.createCommentSymbol("TCPIP_EMAC_MDIO_PIN_CONFIG", tcpipEmacPinConfig)
    tcpipEmacMDIOPinConfig.setLabel("*** Configure "+ atdfMacInstanceName + " management(MDIO) pins from Plugins -> Pin Configuration")
    tcpipEmacMDIOPinConfig.setVisible(True)
    
    # Note for PHY RESET, Interrupt Pins
    tcpipEmacRSTIntPinNote = macComponent.createCommentSymbol("TCPIP_EMAC_RSTINT_PIN_NOTE", tcpipEmacPinConfig)
    tcpipEmacRSTIntPinNote.setLabel("*** Reset and Interrupt pins of Ethernet PHY are not used by "+ atdfMacInstanceName + " driver")
    tcpipEmacRSTIntPinNote.setVisible(True)
    
    emacConfigSummary = macComponent.createMenuSymbol("DRV_EMAC_CONFIG_SUMMARY", None)
    emacConfigSummary.setLabel("Configuration Summary")
    emacConfigSummary.setVisible(False)

    emacClock = macComponent.createIntegerSymbol("DRV_EMAC_CLOCK", emacConfigSummary)
    emacClock.setLabel("Internal Ethernet MAC Clock")
    emacClock.setVisible(False) 
    emacClock.setDefaultValue(Database.getSymbolValue("core", "MCK_FREQUENCY"))
    emacClock.setDependencies(emacClockUpdate, ["core.MCK_FREQUENCY"])
    setVal("tcpipStack", "TCPIP_STACK_MAC_CLOCK", int(Database.getSymbolValue("core", "MCK_FREQUENCY")))



    emacheapdependency = ["RX_STATIC_BUFFERS_0","RX_BUFFER_ALLOCATION_THRESHOLD_0", 
                          "RX_BUFFER_ALLOCATION_0", "RX_BUFFER_SIZE_0", 
                          "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]
    
    # Driver EMAC Heap Size
    drvEmacHeapSize = macComponent.createIntegerSymbol("DRV_EMAC_HEAP_SIZE", None)
    drvEmacHeapSize.setHelp("mcc_h3_emac_configurations")
    drvEmacHeapSize.setLabel("EMAC Heap Size (bytes)") 
    drvEmacHeapSize.setVisible(False)
    drvEmacHeapSize.setDescription("EMAC Heap Size")
    drvEmacHeapSize.setDefaultValue(emacHeapCalc())
    drvEmacHeapSize.setReadOnly(True)
    drvEmacHeapSize.setDependencies(drvEmacHeapUpdate, emacheapdependency)
    
    
    ## File Generation and deployment #########################################
    macFName =              atdfMacModule.lower()
    basePath =              "/driver/" + macFName + "/"
    basePathSrc =           basePath + "src/"
    basePathSrcDynamic =    basePathSrc + "dynamic/"
    cfgPath =               basePath + "/config/"
    prjPath =               "config/" + Variables.get( "__CONFIGURATION_NAME" ) + basePath
    prjPathSrc =            prjPath + "src/"
    prjPathSrcDynamic =     prjPathSrc + "dynamic/"
    macLibDeviceCore =      getMacDeviceCoreName()

        
    #Add to definitions.h
    systemDefFile = macComponent.createFileSymbol("EMAC_H_FILE", None)
    systemDefFile.setType("STRING")
    systemDefFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    systemDefFile.setSourcePath(basePath + "templates/system/system_definitions.h.ftl")
    systemDefFile.setMarkup(True) 
    
    #Add forward declaration to initialization.c
    initDriverSourceFtl = macComponent.createFileSymbol(None, None)
    initDriverSourceFtl.setType("STRING")
    initDriverSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    initDriverSourceFtl.setSourcePath(basePath + "templates/system/system_driver_initialize.c.ftl")
    initDriverSourceFtl.setMarkup(True)
    
    #Add to initialization.c
    sysInitDataSourceFtl = macComponent.createFileSymbol(None, None)
    sysInitDataSourceFtl.setType("STRING")
    sysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    sysInitDataSourceFtl.setSourcePath(basePath + "templates/system/system_data_initialize.c.ftl")
    sysInitDataSourceFtl.setMarkup(True)    

    # Add to system_config.h
    headerFileFtl = macComponent.createFileSymbol( None, None )
    headerFileFtl.setSourcePath( cfgPath + "drv_intmac_emac.h.ftl" )
    headerFileFtl.setOutputName( "core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION" )
    headerFileFtl.setMarkup( True )
    headerFileFtl.setType( "STRING" )

    # file drv_emac.h
    headerFile = macComponent.createFileSymbol( None, None )
    headerFile.setSourcePath( basePath + "drv_emac.h" )
    headerFile.setOutputName( "drv_emac.h" )
    headerFile.setDestPath( basePath )
    headerFile.setProjectPath( prjPath )
    headerFile.setType( "HEADER" )
    headerFile.setOverwrite( True )

    # file TCPIP_MAC_LOCAL_H
    localHeaderFile = macComponent.createFileSymbol( None, None )
    localHeaderFile.setSourcePath( basePathSrc + "drv_emac_local.h" )
    localHeaderFile.setOutputName( "drv_emac_local.h" )
    localHeaderFile.setDestPath( basePathSrc )
    localHeaderFile.setProjectPath( prjPathSrc )
    localHeaderFile.setType( "HEADER" )
    localHeaderFile.setOverwrite( True )

    dynamicFiles = [
        [ "drv_emac_dcpt_lists.h",                  "HEADER" ],
        [ "drv_emac_lib.h",                         "HEADER" ],
        [ "drv_emac.c",                             "SOURCE" ],
        [ "drv_emac_statistics.c",                  "SOURCE" ],
        [ "drv_emac_lib_" + macLibDeviceCore.lower() + ".c","SOURCE" ],
    ]
    for ff in dynamicFiles:
        fileSymbol = macComponent.createFileSymbol( None, None )
        fileSymbol.setSourcePath( basePathSrcDynamic + ff[ 0 ] )
        fileSymbol.setOutputName( ff[ 0 ] )
        fileSymbol.setDestPath( basePathSrcDynamic )
        fileSymbol.setProjectPath( prjPathSrcDynamic )
        fileSymbol.setType( ff[ 1 ] )
        fileSymbol.setOverwrite( True )
