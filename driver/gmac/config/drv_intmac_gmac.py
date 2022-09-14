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
screen1_filter_max_count = 4
screen1_filter_count = 0

screen2_filter_max_count = 8
screen2_que_filter_count = 0
screen2_filter_count = 0

noCache_MPU_index = 0xff

# Type 1 symbols
tcpipGmacQueScreen1Filt = []
tcpipGmacQueScreen1DSTCE = []
tcpipGmacQueScreen1DSTC = []
tcpipGmacQueScreen1UDPE = []
tcpipGmacQueScreen1UDPPortNum = []
tcpipGmacScreen1Que = []
tcpipGmacRxQueScreen1EnableComment = []
# Type 2 symbols
tcpipGmacQueScreen2Filt = []
tcpipGmacQueScreen2VlanPrioEn = []
tcpipGmacQueScreen2VlanPrio = []
tcpipGmacQueScreen2EthTypeEn = []
tcpipGmacQueScreen2EthType = []
tcpipGmacQueScreen2CompAEn = []
tcpipGmacQueScreen2CompA = []
tcpipGmacQueScreen2CompAMask = []
tcpipGmacQueScreen2CompAOffset = []
tcpipGmacQueScreen2CompAOffsetStart = []
tcpipGmacQueScreen2CompBEn = []
tcpipGmacQueScreen2CompB = []
tcpipGmacQueScreen2CompBMask = []
tcpipGmacQueScreen2CompBOffset = []
tcpipGmacQueScreen2CompBOffsetStart = []
tcpipGmacQueScreen2CompCEn = []
tcpipGmacQueScreen2CompC = []
tcpipGmacQueScreen2CompCMask = []
tcpipGmacQueScreen2CompCOffset = []
tcpipGmacQueScreen2CompCOffsetStart = []
tcpipGmacScreen2Que = []
tcpipGmacRxQueScreen2EnableComment = []

interfaceNum = []

gmacComponentId = ""
gmacComponentName = ""
processor = ""

def instantiateComponent(drvGmacComponent):
    global gmac_periphID
    global drvGmacNoCacheMemRegSize
    global tcpipGmacEthRmii
    global tcpipGmacEthMii
    global tcpipGmacEthGmii
    global tcpipGmacEthRgmii
    global gmacComponentId
    global gmacComponentName
    global processor
    
    gmacComponentId = drvGmacComponent.getID()
    gmacComponentName = drvGmacComponent.getDisplayName()
    print("TCPIP Ethernet MAC Component : " + gmacComponentId)
    print gmacComponentName  
    configName = Variables.get("__CONFIGURATION_NAME")      

    device_node = ATDF.getNode('/avr-tools-device-file/devices/device')
    dev_family = str(device_node.getAttribute("family"))
    dev_series = str(device_node.getAttribute("series"))   
    processor =  Variables.get("__PROCESSOR")  
    if "PIC32CZ" in dev_series:
        gmac_node = ATDF.getNode('/avr-tools-device-file/devices/device/peripherals/module@[name="ETH"]')
    else:
        gmac_node = ATDF.getNode('/avr-tools-device-file/devices/device/peripherals/module@[name="GMAC"]')
    gmac_periphID = str(gmac_node.getAttribute("id"))

        
    # GMAC device name
    tcpipGmacDevName = drvGmacComponent.createStringSymbol("TCPIP_INTMAC_DEVICE", None)
    tcpipGmacDevName.setVisible(False)
    if("SAMV7" in processor) or ("SAME7" in processor):
        tcpipGmacDevName.setDefaultValue("SAME7x_V7x")   
    elif ("SAMRH" in processor): # SAMRH71
        tcpipGmacDevName.setDefaultValue("SAMRH71")
    elif ("SAME5" in processor): 
        tcpipGmacDevName.setDefaultValue("SAME5x")  
    elif ("SAMA5" in processor): # SAMA5D2
        tcpipGmacDevName.setDefaultValue("SAMA5D2")       
    elif ("PIC32CZ" in processor):  # PIC32CZ
        tcpipGmacDevName.setDefaultValue("PIC32CZ")   
    elif ("SAMA7G" in processor): # SAMA7G4
        tcpipGmacDevName.setDefaultValue("SAMA7G")   
    
    if "PIC32CZ" in processor: # PIC32CZ    
        # Enable ETH_TX clock
        Database.setSymbolValue("core", "ETH_TX_CLOCK_ENABLE", True)
    else:
        # Enable GMAC clock
        Database.setSymbolValue("core", gmacComponentName + "_CLOCK_ENABLE", True)
    
    # Use Internal Ethernet MAC Driver? 
    drvGmac = drvGmacComponent.createBooleanSymbol("TCPIP_USE_ETH_MAC", None)
    drvGmac.setHelp("mcc_h3_gmac_configurations")
    drvGmac.setLabel("Use Internal Ethernet MAC Driver?")
    drvGmac.setVisible(False)
    drvGmac.setDescription("Use Internal Ethernet MAC Driver?")
    drvGmac.setDefaultValue(True)

    # GMAC instance
    drvGmacInstance = drvGmacComponent.createStringSymbol( "GMAC_PERIPHERAL_INSTANCE", None )
    drvGmacInstance.setLabel( "GMAC Peripheral Instance" )
    drvGmacInstance.setDefaultValue( gmacComponentName )
    drvGmacInstance.setVisible( False )
    drvGmacInstance.setReadOnly( True )

    drvGmacConfigSummary = drvGmacComponent.createMenuSymbol("DRV_"+ gmacComponentName + "_CONFIG_SUMMARY", None)
    drvGmacConfigSummary.setLabel("Configuration Summary")
    drvGmacConfigSummary.setVisible(False)
    
    # Internal Ethernet MAC Clock
    
    drvGmacClock = drvGmacComponent.createIntegerSymbol("DRV_"+ gmacComponentName + "_CLOCK", drvGmacConfigSummary)
    drvGmacClock.setLabel("Internal Ethernet MAC Clock")
    drvGmacClock.setVisible(False)    
    if ("SAME5" in processor):
        drvGmacClock.setDefaultValue(Database.getSymbolValue("core", "MAIN_CLOCK_FREQUENCY"))
        drvGmacClock.setDependencies(tcpipGmacClockUpdate, ["core.MAIN_CLOCK_FREQUENCY"])
        setVal("tcpipStack", "TCPIP_STACK_MAC_CLOCK", Database.getSymbolValue("core", "MAIN_CLOCK_FREQUENCY"))
    elif ("SAMV7" in processor) or ("SAME7" in processor):
        drvGmacClock.setDefaultValue(int(Database.getSymbolValue("core", "MASTER_CLOCK_FREQUENCY")))
        drvGmacClock.setDependencies(tcpipGmacClockUpdate, ["core.MASTER_CLOCK_FREQUENCY"])
        setVal("tcpipStack", "TCPIP_STACK_MAC_CLOCK", int(Database.getSymbolValue("core", "MASTER_CLOCK_FREQUENCY")))
    elif ("PIC32CZ" in processor):     
        coreComponent = Database.getComponentByID("core")
        clkdivSymbol = coreComponent.getSymbolByID("CONF_MCLK_CLKDIV1")
        main_clock = int(Database.getSymbolValue("core", "MAIN_CLOCK_FREQUENCY"))
        clock_div = int(clkdivSymbol.getSelectedValue(), 16)
        peripheral_clock = main_clock/clock_div
        drvGmacClock.setDefaultValue(peripheral_clock)
        drvGmacClock.setDependencies(tcpipPic32CZGmacClockUpdate, ["core.MAIN_CLOCK_FREQUENCY","core.CONF_MCLK_CLKDIV1"])
        setVal("tcpipStack", "TCPIP_STACK_MAC_CLOCK", int(peripheral_clock))
    elif ("SAMA5" in processor):
        drvGmacClock.setDefaultValue(int(Database.getSymbolValue("core", "PCLOCK_LS_CLOCK_FREQUENCY")))
        drvGmacClock.setDependencies(tcpipGmacClockUpdate, ["core.PCLOCK_LS_CLOCK_FREQUENCY"])
        setVal("tcpipStack", "TCPIP_STACK_MAC_CLOCK", int(Database.getSymbolValue("core", "PCLOCK_LS_CLOCK_FREQUENCY")))
    elif ("SAMA7G" in processor):
        drvGmacClock.setDefaultValue(int(Database.getSymbolValue("core", "MCK1_FREQUENCY")))
        drvGmacClock.setDependencies(tcpipGmacClockUpdate, ["core.MCK1_FREQUENCY"])
        setVal("tcpipStack", "TCPIP_STACK_MAC_CLOCK", int(Database.getSymbolValue("core", "MCK1_FREQUENCY")))  
 
    # Maximum Number MAC Queues
    tcpipGmacMaxQueCnt = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_MAX_MAC_QUE_CNT", None)
    tcpipGmacMaxQueCnt.setLabel("Maximum Number of Queues")
    tcpipGmacMaxQueCnt.setVisible(True)
    if (("SAME7" in processor) or ("SAMV7" in processor)):  
        tcpipGmacMaxQueCnt.setDefaultValue(6)
    elif ("SAME5" in processor):
        tcpipGmacMaxQueCnt.setDefaultValue(1)
    elif ("SAMA5" in processor):      
        tcpipGmacMaxQueCnt.setDefaultValue(3)
    elif ("PIC32CZ" in processor): 
        tcpipGmacMaxQueCnt.setDefaultValue(6)
    elif ("SAMA7G" in processor):
        if(gmacComponentName == "GMAC0"):
            tcpipGmacMaxQueCnt.setDefaultValue(6)
        else:
            tcpipGmacMaxQueCnt.setDefaultValue(2)
    else:
        tcpipGmacMaxQueCnt.setDefaultValue(1)
    tcpipGmacMaxQueCnt.setReadOnly( True )
        
    # Use Internal Ethernet MAC Driver? 
    tcpipGmacQue0 = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_QUEUE_0", None)
    tcpipGmacQue0.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacQue0.setLabel(gmacComponentName + " Queue 0")
    tcpipGmacQue0.setVisible(False)
    tcpipGmacQue0.setDescription(gmacComponentName + " Queue 0")
    tcpipGmacQue0.setDefaultValue(True)
    tcpipGmacQue0.setReadOnly(True)
    
    tcpipGmacTraffic = drvGmacComponent.createComboSymbol("TCPIP_"+ gmacComponentName + "_TRAFFIC",None,["Low", "Medium", "High"])
    tcpipGmacTraffic.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacTraffic.setVisible(True)
    tcpipGmacTraffic.setLabel(gmacComponentName + " Default Configuration for Network Traffic")
    tcpipGmacTraffic.setDefaultValue("Medium")

    # Enable Tx Priority of Queue 0 
    tcpipGmacQue0TxEn = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_TX_EN_QUE0", tcpipGmacTraffic)
    tcpipGmacQue0TxEn.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacQue0TxEn.setLabel(gmacComponentName + " Queue 0 Tx Enable")
    tcpipGmacQue0TxEn.setVisible(True)
    tcpipGmacQue0TxEn.setDescription(gmacComponentName + " Queue 0 Tx Enable")
    tcpipGmacQue0TxEn.setDefaultValue(True)
    tcpipGmacQue0TxEn.setReadOnly(True)

    # Enable Rx Priority of Queue 0 
    tcpipGmacQue0RxEn = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_RX_EN_QUE0", tcpipGmacTraffic)
    tcpipGmacQue0RxEn.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacQue0RxEn.setLabel(gmacComponentName + " Queue 0 Rx Enable")
    tcpipGmacQue0RxEn.setVisible(True)
    tcpipGmacQue0RxEn.setDescription(gmacComponentName + " Queue 0 Rx Enable")
    tcpipGmacQue0RxEn.setDefaultValue(True)
    tcpipGmacQue0RxEn.setReadOnly(True)
    
    # Number of Tx Descriptors for Queue0
    tcpipGmacTxDescCountQue0 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_QUE0", tcpipGmacQue0TxEn)
    tcpipGmacTxDescCountQue0.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacTxDescCountQue0.setLabel("Number of Tx Descriptors")
    tcpipGmacTxDescCountQue0.setVisible(True)
    tcpipGmacTxDescCountQue0.setDescription("Number of Tx Descriptors for Queue0")
    tcpipGmacTxDescCountQue0.setDefaultValue(8)
    tcpipGmacTxDescCountQue0.setDependencies(tcpipGmacTxDescCountQue0CallBack, ["TCPIP_"+ gmacComponentName + "_TRAFFIC"])
                    
    # Max Tx Packet size support for Queue0.
    tcpipGmacMaxTxPktSizeQue0 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_MAX_TX_PKT_SIZE_QUE0", tcpipGmacQue0TxEn)
    tcpipGmacMaxTxPktSizeQue0.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacMaxTxPktSizeQue0.setLabel("Max Tx Packet size supported")
    tcpipGmacMaxTxPktSizeQue0.setVisible(True)
    tcpipGmacMaxTxPktSizeQue0.setDescription("Max Tx Packet size")
    tcpipGmacMaxTxPktSizeQue0.setDefaultValue(1536)
    tcpipGmacMaxTxPktSizeQue0.setReadOnly(True)
        
    # Comment for Max Tx Packet size support for Queue0.
    tcpipGmacMaxTxPktSizeQue0Comment = drvGmacComponent.createCommentSymbol("TCPIP_"+ gmacComponentName + "_MAX_TX_PKT_SIZE_COMMENT_QUE0", tcpipGmacQue0TxEn)
    tcpipGmacMaxTxPktSizeQue0Comment.setLabel("*** Transmit packet size for this queue should be less than this limit")
    tcpipGmacMaxTxPktSizeQue0Comment.setVisible(True)
    # tcpipGmacMaxTxPktSizeQue0Comment.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_TX_EN_QUE0"])
    
    # Number of Rx Descriptors for Queue0
    tcpipGmacRxDescCountQue0 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_QUE0", tcpipGmacQue0RxEn)
    tcpipGmacRxDescCountQue0.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacRxDescCountQue0.setLabel("Number of Rx Descriptors")
    tcpipGmacRxDescCountQue0.setVisible(True)
    tcpipGmacRxDescCountQue0.setDescription("Number of Rx Descriptors for Queue0")
    tcpipGmacRxDescCountQue0.setDefaultValue(8)
    tcpipGmacRxDescCountQue0.setDependencies(tcpipGmacRxDescCountQue0CallBack, ["TCPIP_"+ gmacComponentName + "_TRAFFIC"])
    
    # Number of Dedicated MAC RX Buffers
    tcpipGmacRxOwnBufferQue0 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_DEDICATED_BUFFERS_QUE0", tcpipGmacQue0RxEn)
    tcpipGmacRxOwnBufferQue0.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacRxOwnBufferQue0.setLabel("Number of Dedicated MAC RX Buffers")
    tcpipGmacRxOwnBufferQue0.setVisible(True)
    tcpipGmacRxOwnBufferQue0.setDescription("Number of Dedicated MAC RX Buffers")
    tcpipGmacRxOwnBufferQue0.setDefaultValue(8)
    tcpipGmacRxOwnBufferQue0.setDependencies(tcpipGmacRxOwnBufferQue0CallBack, ["TCPIP_"+ gmacComponentName + "_TRAFFIC"])
    
    # Size Of RX Buffer for Queue0. Should Be Multiple Of 64.
    tcpipGmacRxBuffSizeQue0 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_QUE0", tcpipGmacQue0RxEn)
    tcpipGmacRxBuffSizeQue0.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacRxBuffSizeQue0.setLabel("Size of RX Buffer (multiple of 64)")
    tcpipGmacRxBuffSizeQue0.setVisible(True)
    tcpipGmacRxBuffSizeQue0.setDescription("Size Of RX Buffer for Queue0. Should Be Multiple Of 64.")
    tcpipGmacRxBuffSizeQue0.setDefaultValue(1536)
    tcpipGmacRxBuffSizeQue0.setDependencies(tcpipGmacRxBuffSizeQue0CallBack, ["TCPIP_"+ gmacComponentName + "_QUEUE_0"])
    
    # Number of additional Rx buffers for Queue0
    tcpipGmacRxAddlBuffCountQue0 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_ADDL_BUFF_COUNT_QUE0", tcpipGmacQue0RxEn)
    tcpipGmacRxAddlBuffCountQue0.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacRxAddlBuffCountQue0.setLabel("Number of additional Rx buffers")
    tcpipGmacRxAddlBuffCountQue0.setVisible(True)
    tcpipGmacRxAddlBuffCountQue0.setDescription("Number of additional Sticky Rx buffers for Queue0")
    tcpipGmacRxAddlBuffCountQue0.setDefaultValue(2)
    tcpipGmacRxAddlBuffCountQue0.setDependencies(tcpipGmacRxAddlBuffCountQue0CallBack, ["TCPIP_"+ gmacComponentName + "_TRAFFIC"]) 
        
    # Rx buffer Count Threshold to allocate new Rx buffers, for Queue0
    tcpipGmacRxBuffCountThresQue0 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_BUFF_COUNT_THRESHOLD_QUE0", tcpipGmacQue0RxEn)
    tcpipGmacRxBuffCountThresQue0.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacRxBuffCountThresQue0.setLabel("Minimum Threshold for Rx Buffer replenish process")
    tcpipGmacRxBuffCountThresQue0.setVisible(True)
    tcpipGmacRxBuffCountThresQue0.setDescription("Rx buffer Count Threshold to allocate new Rx buffers for Queue0")
    tcpipGmacRxBuffCountThresQue0.setDefaultValue(1)
    tcpipGmacRxBuffCountThresQue0.setDependencies(tcpipGmacRxBuffCountThresQue0CallBack, ["TCPIP_"+ gmacComponentName + "_TRAFFIC"])
        
    # Rx buffer allocate Count for Queue0
    tcpipGmacRxBuffAllocCountQue0 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_BUFF_ALLOC_COUNT_QUE0", tcpipGmacQue0RxEn)
    tcpipGmacRxBuffAllocCountQue0.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacRxBuffAllocCountQue0.setLabel("Rx Buffer allocate count during  replenish process")
    tcpipGmacRxBuffAllocCountQue0.setVisible(True)
    tcpipGmacRxBuffAllocCountQue0.setDescription("Rx buffer allocate Count for Queue0")
    tcpipGmacRxBuffAllocCountQue0.setDefaultValue(2)
    tcpipGmacRxBuffAllocCountQue0.setMin(1)
    tcpipGmacRxBuffAllocCountQue0.setDependencies(tcpipGmacRxBuffAllocCountQue0CallBack, ["TCPIP_"+ gmacComponentName + "_TRAFFIC"])    

    if "PIC32CZ" in processor:
        int_name = "ETH_PRI_Q_0"
    else:
        int_name = gmacComponentName
        
    interruptVector = int_name + "_INTERRUPT_ENABLE"
    interruptHandler = int_name + "_INTERRUPT_HANDLER"
    interruptHandlerLock = int_name + "_INTERRUPT_HANDLER_LOCK"
    
    
    drvGmacIntSrcQue0 = drvGmacComponent.createStringSymbol("DRV_"+ gmacComponentName + "_INT_SRC_QUE0", None)
    drvGmacIntSrcQue0.setLabel(int_name + " Interrupt Source")
    drvGmacIntSrcQue0.setVisible(False)
    drvGmacIntSrcQue0.setDefaultValue(int_name + "_IRQn")
    
    Database.clearSymbolValue("core", interruptVector)
    Database.setSymbolValue("core", interruptVector, True, 2)
    if not "PIC32CZ" in processor : 
        Database.clearSymbolValue("core", interruptHandler)
        Database.setSymbolValue("core", interruptHandler, gmacComponentName + "_InterruptHandler", 2)
    Database.clearSymbolValue("core", interruptHandlerLock)
    Database.setSymbolValue("core", interruptHandlerLock, True, 2)
    if("SAMV7" in processor) or ("SAME7" in processor) or ("SAMRH" in processor)or ("PIC32CZ" in processor) or (("SAMA7G" in processor) and (gmacComponentName == "GMAC0")):
        gmacheapdependency = [  "TCPIP_"+ gmacComponentName + "_QUEUE_0","TCPIP_"+ gmacComponentName + "_RX_EN_QUE0","TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_QUE0",
                                "TCPIP_"+ gmacComponentName + "_RX_ADDL_BUFF_COUNT_QUE0","TCPIP_"+ gmacComponentName + "_RX_BUFF_COUNT_THRESHOLD_QUE0","TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_QUE0",
                                "TCPIP_"+ gmacComponentName + "_TX_EN_QUE0","TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_QUE0","TCPIP_"+ gmacComponentName + "_QUEUE_1","TCPIP_"+ gmacComponentName + "_RX_EN_QUE1",
                                "TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_QUE1","TCPIP_"+ gmacComponentName + "_RX_ADDL_BUFF_COUNT_QUE1","TCPIP_"+ gmacComponentName + "_RX_BUFF_COUNT_THRESHOLD_QUE1",
                                "TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_QUE1","TCPIP_"+ gmacComponentName + "_TX_EN_QUE1","TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_QUE1","TCPIP_"+ gmacComponentName + "_QUEUE_2",
                                "TCPIP_"+ gmacComponentName + "_RX_EN_QUE2","TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_QUE2","TCPIP_"+ gmacComponentName + "_RX_ADDL_BUFF_COUNT_QUE2",
                                "TCPIP_"+ gmacComponentName + "_RX_BUFF_COUNT_THRESHOLD_QUE2","TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_QUE2","TCPIP_"+ gmacComponentName + "_TX_EN_QUE2",
                                "TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_QUE2","TCPIP_"+ gmacComponentName + "_QUEUE_3","TCPIP_"+ gmacComponentName + "_RX_EN_QUE3","TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_QUE3",
                                "TCPIP_"+ gmacComponentName + "_RX_ADDL_BUFF_COUNT_QUE3","TCPIP_"+ gmacComponentName + "_RX_BUFF_COUNT_THRESHOLD_QUE3","TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_QUE3",
                                "TCPIP_"+ gmacComponentName + "_TX_EN_QUE3","TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_QUE3","TCPIP_"+ gmacComponentName + "_QUEUE_4","TCPIP_"+ gmacComponentName + "_RX_EN_QUE4",
                                "TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_QUE4","TCPIP_"+ gmacComponentName + "_RX_ADDL_BUFF_COUNT_QUE4","TCPIP_"+ gmacComponentName + "_RX_BUFF_COUNT_THRESHOLD_QUE4",
                                "TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_QUE4","TCPIP_"+ gmacComponentName + "_TX_EN_QUE4","TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_QUE4",
                                "TCPIP_"+ gmacComponentName + "_QUEUE_5","TCPIP_"+ gmacComponentName + "_RX_EN_QUE5","TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_QUE5","TCPIP_"+ gmacComponentName + "_RX_ADDL_BUFF_COUNT_QUE5",
                                "TCPIP_"+ gmacComponentName + "_RX_BUFF_COUNT_THRESHOLD_QUE5","TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_QUE5","TCPIP_"+ gmacComponentName + "_TX_EN_QUE5",
                                "TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_QUE5","tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]
    if(("SAMA7G" in processor) and (gmacComponentName == "GMAC1")): # SAMA7G5 GMAC1
        gmacheapdependency = [  "TCPIP_"+ gmacComponentName + "_QUEUE_0","TCPIP_"+ gmacComponentName + "_RX_EN_QUE0","TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_QUE0","TCPIP_"+ gmacComponentName + "_RX_ADDL_BUFF_COUNT_QUE0",
                                "TCPIP_"+ gmacComponentName + "_RX_BUFF_COUNT_THRESHOLD_QUE0","TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_QUE0","TCPIP_"+ gmacComponentName + "_TX_EN_QUE0",
                                "TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_QUE0","TCPIP_"+ gmacComponentName + "_QUEUE_1","TCPIP_"+ gmacComponentName + "_RX_EN_QUE1","TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_QUE1",
                                "TCPIP_"+ gmacComponentName + "_RX_ADDL_BUFF_COUNT_QUE1","TCPIP_"+ gmacComponentName + "_RX_BUFF_COUNT_THRESHOLD_QUE1","TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_QUE1",
                                "TCPIP_"+ gmacComponentName + "_TX_EN_QUE1","TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_QUE1", "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]
    
    if("SAMA5" in processor): # SAMA5D2
        gmacheapdependency = [  "TCPIP_"+ gmacComponentName + "_QUEUE_0","TCPIP_"+ gmacComponentName + "_RX_EN_QUE0","TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_QUE0","TCPIP_"+ gmacComponentName + "_RX_ADDL_BUFF_COUNT_QUE0",
                                "TCPIP_"+ gmacComponentName + "_RX_BUFF_COUNT_THRESHOLD_QUE0","TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_QUE0","TCPIP_"+ gmacComponentName + "_TX_EN_QUE0",
                                "TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_QUE0","TCPIP_"+ gmacComponentName + "_QUEUE_1","TCPIP_"+ gmacComponentName + "_RX_EN_QUE1","TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_QUE1",
                                "TCPIP_"+ gmacComponentName + "_RX_ADDL_BUFF_COUNT_QUE1","TCPIP_"+ gmacComponentName + "_RX_BUFF_COUNT_THRESHOLD_QUE1","TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_QUE1",
                                "TCPIP_"+ gmacComponentName + "_TX_EN_QUE1","TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_QUE1","TCPIP_"+ gmacComponentName + "_QUEUE_2","TCPIP_"+ gmacComponentName + "_RX_EN_QUE2",
                                "TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_QUE2","TCPIP_"+ gmacComponentName + "_RX_ADDL_BUFF_COUNT_QUE2","TCPIP_"+ gmacComponentName + "_RX_BUFF_COUNT_THRESHOLD_QUE2",
                                "TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_QUE2","TCPIP_"+ gmacComponentName + "_TX_EN_QUE2","TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_QUE2",
                                "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]
    
    if("SAME5" in processor): # SAME54
        gmacheapdependency = [  "TCPIP_"+ gmacComponentName + "_QUEUE_0","TCPIP_"+ gmacComponentName + "_RX_EN_QUE0","TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_QUE0","TCPIP_"+ gmacComponentName + "_RX_ADDL_BUFF_COUNT_QUE0",
                                "TCPIP_"+ gmacComponentName + "_RX_BUFF_COUNT_THRESHOLD_QUE0","TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_QUE0","TCPIP_"+ gmacComponentName + "_TX_EN_QUE0",
                                "TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_QUE0", "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]
        

    # GMAC TX descriptors Dummy Count
    tcpipGmacTxDescCountDummmy = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_DUMMY", None)
    tcpipGmacTxDescCountDummmy.setLabel("") 
    tcpipGmacTxDescCountDummmy.setVisible(False)
    tcpipGmacTxDescCountDummmy.setDescription(gmacComponentName + " TX descriptors Dummy Count")
    tcpipGmacTxDescCountDummmy.setDefaultValue(1)

    # GMAC RX descriptors Dummy Count
    tcpipGmacRxDescCountDummmy = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_DUMMY", None)
    tcpipGmacRxDescCountDummmy.setLabel(gmacComponentName + " RX descriptors Dummy Count")  
    tcpipGmacRxDescCountDummmy.setVisible(False)
    tcpipGmacRxDescCountDummmy.setDescription(gmacComponentName + " RX descriptors Dummy Count")
    tcpipGmacRxDescCountDummmy.setDefaultValue(1)

    # GMAC Rx buffer size dummy
    tcpipGmacRxBuffSizeDummmy = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_DUMMY", None)
    tcpipGmacRxBuffSizeDummmy.setLabel(gmacComponentName + " Rx buffer size dummy") 
    tcpipGmacRxBuffSizeDummmy.setVisible(False)
    tcpipGmacRxBuffSizeDummmy.setDescription(gmacComponentName + " Rx buffer size dummy")
    tcpipGmacRxBuffSizeDummmy.setDefaultValue(64)
    
    # GMAC Tx buffer size dummy
    tcpipGmacTxBuffSizeDummmy = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_TX_BUFF_SIZE_DUMMY", None)
    tcpipGmacTxBuffSizeDummmy.setLabel(gmacComponentName + " Tx buffer size dummy")  
    tcpipGmacTxBuffSizeDummmy.setVisible(False)
    tcpipGmacTxBuffSizeDummmy.setDescription(gmacComponentName + " Tx buffer size dummy")
    tcpipGmacTxBuffSizeDummmy.setDefaultValue(64)
    
    # Ethernet RX Filters Selection Settings
    tcpipEthRxFilter = drvGmacComponent.createMenuSymbol(None, None) 
    tcpipEthRxFilter.setHelp("mcc_h3_gmac_configurations")
    tcpipEthRxFilter.setLabel("Ethernet RX Filters Selection")
    tcpipEthRxFilter.setVisible(True)
    tcpipEthRxFilter.setDescription("Ethernet RX Filters Selection Settings")

    # Accept Broadcast Packets
    tcpipGmacEthFilterBcastAccept = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_FILTER_BCAST_ACCEPT", tcpipEthRxFilter)
    tcpipGmacEthFilterBcastAccept.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacEthFilterBcastAccept.setLabel("Accept Broadcast Packets")
    tcpipGmacEthFilterBcastAccept.setVisible(True)
    tcpipGmacEthFilterBcastAccept.setDescription("Accept Broadcast Packets")
    tcpipGmacEthFilterBcastAccept.setDefaultValue(True)
    
    # Accept Multicast Packets
    tcpipGmacEthFilterMcastAccept = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_FILTER_MCAST_ACCEPT", tcpipEthRxFilter)
    tcpipGmacEthFilterMcastAccept.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacEthFilterMcastAccept.setLabel("Accept Multicast Packets")
    tcpipGmacEthFilterMcastAccept.setVisible(True)
    tcpipGmacEthFilterMcastAccept.setDescription("Accept Multicast Packets")
    tcpipGmacEthFilterMcastAccept.setDefaultValue(True)
    
    # Accept Unicast Packets
    tcpipGmacEthFilterUcastAccept = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_FILTER_UCAST_ACCEPT", tcpipEthRxFilter)
    tcpipGmacEthFilterUcastAccept.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacEthFilterUcastAccept.setLabel("Accept Unicast Packets")
    tcpipGmacEthFilterUcastAccept.setVisible(True)
    tcpipGmacEthFilterUcastAccept.setDescription("Accept Unicast Packets")
    tcpipGmacEthFilterUcastAccept.setDefaultValue(True)


    # Accept All Packets (Promiscuous Mode)
    tcpipGmacEthFilterAllAccept = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_FILTER_ALL_ACCEPT", tcpipEthRxFilter)
    tcpipGmacEthFilterAllAccept.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacEthFilterAllAccept.setLabel("Accept All Packets (Promiscuous Mode)")
    tcpipGmacEthFilterAllAccept.setVisible(True)
    tcpipGmacEthFilterAllAccept.setDescription("Accept All Packets (Promiscuous Mode)")
    tcpipGmacEthFilterAllAccept.setDefaultValue(False)
    
    # Ethernet Connection Flags
    tcpipEthConnFlag = drvGmacComponent.createMenuSymbol(None, None) 
    tcpipEthConnFlag.setLabel("Ethernet Connection Flags")
    tcpipEthConnFlag.setVisible(True)
    tcpipEthConnFlag.setDescription("Ethernet Connection Flags")
    
    # Use Auto Negotiation
    tcpipGmacEthAutoNegotiate = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_OF_AUTO_NEGOTIATION", tcpipEthConnFlag)
    tcpipGmacEthAutoNegotiate.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacEthAutoNegotiate.setLabel("Use Auto Negotiation")
    tcpipGmacEthAutoNegotiate.setVisible(True) 
    tcpipGmacEthAutoNegotiate.setDescription("Use Auto Negotiation")
    tcpipGmacEthAutoNegotiate.setDefaultValue(True)
    
    # Use Full Duplex
    tcpipGmacEthFullDuplex = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_OF_FULL_DUPLEX", tcpipEthConnFlag)
    tcpipGmacEthFullDuplex.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacEthFullDuplex.setLabel("Use Full Duplex")
    tcpipGmacEthFullDuplex.setVisible(True)
    tcpipGmacEthFullDuplex.setDescription("Use Full Duplex")
    tcpipGmacEthFullDuplex.setDefaultValue(True)
    
    # Use Half Duplex
    tcpipGmacEthHalfDuplex = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_OF_HALF_DUPLEX", tcpipEthConnFlag)
    tcpipGmacEthHalfDuplex.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacEthHalfDuplex.setLabel("Use Half Duplex")
    tcpipGmacEthHalfDuplex.setVisible(True)
    tcpipGmacEthHalfDuplex.setDescription("Use Half Duplex")
    tcpipGmacEthHalfDuplex.setDefaultValue(True)
    
    if ("PIC32CZ" in processor) or (("SAMA7G" in processor) and (gmacComponentName == "GMAC0")): # PIC32CZ or GMAC0 of SAMA7G54
        # Use 1000MBps
        tcpipGmacEthUse1000 = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_OF_1000", tcpipEthConnFlag)
        tcpipGmacEthUse1000.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacEthUse1000.setLabel("Use 1000MBps")
        tcpipGmacEthUse1000.setVisible(True) 
        tcpipGmacEthUse1000.setDescription("Use 1000MBps")
        tcpipGmacEthUse1000.setDefaultValue(True)
    
    # Use 100MBps
    tcpipGmacEthUse100 = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_OF_100", tcpipEthConnFlag)
    tcpipGmacEthUse100.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacEthUse100.setLabel("Use 100MBps")
    tcpipGmacEthUse100.setVisible(True) 
    tcpipGmacEthUse100.setDescription("Use 100MBps")
    tcpipGmacEthUse100.setDefaultValue(True)
    
    # Use 10MBps
    tcpipGmacEthUse10 = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_OF_10", tcpipEthConnFlag)
    tcpipGmacEthUse10.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacEthUse10.setLabel("Use 10MBps")
    tcpipGmacEthUse10.setVisible(True)
    tcpipGmacEthUse10.setDescription("Use 10MBps")
    tcpipGmacEthUse10.setDefaultValue(True)
    
    # Use Auto MDIX
    tcpipGmacEthMdixAuto = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_OF_MDIX_AUTO", tcpipEthConnFlag)
    tcpipGmacEthMdixAuto.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacEthMdixAuto.setLabel("Use Auto MDIX")
    tcpipGmacEthMdixAuto.setVisible(True) 
    tcpipGmacEthMdixAuto.setDescription("Use Auto MDIX")
    tcpipGmacEthMdixAuto.setDefaultValue(True)
    
    # Use Swapped MDIX
    tcpipGmacEthMdixSwap = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_OF_MDIX_SWAP", tcpipEthConnFlag)
    tcpipGmacEthMdixSwap.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacEthMdixSwap.setLabel("Use Swapped MDIX")
    tcpipGmacEthMdixSwap.setVisible(False)
    tcpipGmacEthMdixSwap.setDescription("Use Swapped MDIX")
    tcpipGmacEthMdixSwap.setDefaultValue(False)
    tcpipGmacEthMdixSwap.setDependencies(tcpipEthMacMdixSwapVisible, ["TCPIP_"+ gmacComponentName + "_ETH_OF_MDIX_AUTO"])

    # RMII Connection
    tcpipGmacEthRmii = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_OF_RMII", tcpipEthConnFlag)
    tcpipGmacEthRmii.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacEthRmii.setLabel("RMII Connection")
    tcpipGmacEthRmii.setVisible(True)
    tcpipGmacEthRmii.setDescription("RMII Connection")
    if (("SAMA7G" in processor) and (gmacComponentName == "GMAC0")): # GMAC0 of SAMA7G54
        tcpipGmacEthRmii.setDefaultValue(False)
    else:
        tcpipGmacEthRmii.setDefaultValue(True)
    tcpipGmacEthRmii.setDependencies( tcpipEthMacMIIMode, ["TCPIP_"+ gmacComponentName + "_ETH_OF_RMII"] )

    # MII Connection
    tcpipGmacEthMii = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_OF_MII", tcpipEthConnFlag)
    tcpipGmacEthMii.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacEthMii.setLabel("MII Connection")
    tcpipGmacEthMii.setVisible(True)
    tcpipGmacEthMii.setDescription("MII Connection")
    tcpipGmacEthMii.setDefaultValue(False)
    tcpipGmacEthMii.setDependencies( tcpipEthMacMIIMode, ["TCPIP_"+ gmacComponentName + "_ETH_OF_MII"] )
    
    if ("PIC32CZ" in processor): # PIC32CZ
        # GMII Connection
        tcpipGmacEthGmii = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_OF_GMII", tcpipEthConnFlag)
        tcpipGmacEthGmii.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacEthGmii.setLabel("GMII Connection")
        tcpipGmacEthGmii.setVisible(True)
        tcpipGmacEthGmii.setDescription("GMII Connection")
        tcpipGmacEthGmii.setDefaultValue(False)
        tcpipGmacEthGmii.setDependencies( tcpipEthMacMIIMode, ["TCPIP_"+ gmacComponentName + "_ETH_OF_GMII"] )
    elif (("SAMA7G" in processor) and (gmacComponentName == "GMAC0")): # GMAC0 of SAMA7G54
        # RGMII Connection
        tcpipGmacEthRgmii = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_OF_RGMII", tcpipEthConnFlag)
        tcpipGmacEthRgmii.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacEthRgmii.setLabel("RGMII Connection")
        tcpipGmacEthRgmii.setVisible(True)
        tcpipGmacEthRgmii.setDescription("RGMII Connection")
        tcpipGmacEthRgmii.setDefaultValue(True)
        tcpipGmacEthRgmii.setDependencies( tcpipEthMacMIIMode, ["TCPIP_"+ gmacComponentName + "_ETH_OF_RGMII"] )
        
    # Advanced Settings
    tcpipGmacAdvSettings = drvGmacComponent.createMenuSymbol("TCPIP_"+ gmacComponentName + "_ADV_SETTING", None)
    tcpipGmacAdvSettings.setLabel("Advanced Settings")
    tcpipGmacAdvSettings.setDescription("Advanced Settings")
    tcpipGmacAdvSettings.setVisible(True)
    
    # SAM E70/V71/RH71 has 6 Priority Queue
    # SAM A5D2 has 3 Priority Queue
    # SAM E54 has only 1 Queue
    if("SAMV7" in processor) or ("SAME7" in processor) or ("SAMRH" in processor) or ("SAMA5" in processor) or ("PIC32CZ" in processor) or ("SAMA7G" in processor): # SAME70 or SAMV71 or SAMRH71 or SAMA5D2 or PIC32CZ or SAMA7G5
        tcpipGmacPrioQueues = drvGmacComponent.createMenuSymbol("TCPIP_"+ gmacComponentName + "_PRIORITY_QUEUES", tcpipGmacAdvSettings)
        tcpipGmacPrioQueues.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacPrioQueues.setLabel(gmacComponentName + " Priority Queues")
        tcpipGmacPrioQueues.setDescription(gmacComponentName + " Priority Queues")
        tcpipGmacPrioQueues.setVisible(True)
    
        # GMAC Queue 1  
        tcpipGmacQue1 = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_QUEUE_1", tcpipGmacPrioQueues)
        tcpipGmacQue1.setLabel(gmacComponentName + " Queue 1")
        tcpipGmacQue1.setVisible(True)
        tcpipGmacQue1.setDescription(gmacComponentName + " Queue 1")
        tcpipGmacQue1.setDefaultValue(False)
        #tcpipGmacQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])        
                
        # Enable Tx Priority of Queue 1 
        tcpipGmacQue1TxEn = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_TX_EN_QUE1", tcpipGmacQue1)
        tcpipGmacQue1TxEn.setLabel(gmacComponentName + " Queue 1 Tx Enable")
        tcpipGmacQue1TxEn.setVisible(False)
        tcpipGmacQue1TxEn.setDescription(gmacComponentName + " Queue 1 Tx Enable")
        tcpipGmacQue1TxEn.setDefaultValue(False)
        tcpipGmacQue1TxEn.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_QUEUE_1"])
        
        # Enable Rx Priority of Queue 1 
        tcpipGmacQue1RxEn = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_RX_EN_QUE1", tcpipGmacQue1)
        tcpipGmacQue1RxEn.setLabel(gmacComponentName + " Queue 1 Rx Enable")
        tcpipGmacQue1RxEn.setVisible(False)
        tcpipGmacQue1RxEn.setDescription(gmacComponentName + " Queue 1 Rx Enable")
        tcpipGmacQue1RxEn.setDefaultValue(False)
        tcpipGmacQue1RxEn.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_QUEUE_1"])

        # Number of Tx Descriptors for Queue1
        tcpipGmacTxDescCountQue1 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_QUE1", tcpipGmacQue1TxEn)
        tcpipGmacTxDescCountQue1.setLabel("Number of Tx Descriptors")
        tcpipGmacTxDescCountQue1.setVisible(False)
        tcpipGmacTxDescCountQue1.setDescription("Number of Tx Descriptors for Queue1")
        tcpipGmacTxDescCountQue1.setDefaultValue(1)
        tcpipGmacTxDescCountQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_TX_EN_QUE1"])

        # Number of Rx Descriptors for Queue1
        tcpipGmacRxDescCountQue1 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_QUE1", tcpipGmacQue1RxEn)
        tcpipGmacRxDescCountQue1.setLabel("Number of Rx Descriptors")
        tcpipGmacRxDescCountQue1.setVisible(False)
        tcpipGmacRxDescCountQue1.setDescription("Number of Rx Descriptors for Queue1")
        tcpipGmacRxDescCountQue1.setDefaultValue(1)
        tcpipGmacRxDescCountQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE1"])

        # Number of Dedicated MAC RX Buffers
        tcpipGmacRxOwnBufferQue1 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_DEDICATED_BUFFERS_QUE1", tcpipGmacQue1RxEn)
        tcpipGmacRxOwnBufferQue1.setLabel("Number of Dedicated MAC RX Buffers")
        tcpipGmacRxOwnBufferQue1.setVisible(False)
        tcpipGmacRxOwnBufferQue1.setDescription("Number of Dedicated MAC RX Buffers")
        tcpipGmacRxOwnBufferQue1.setDefaultValue(8)
        tcpipGmacRxOwnBufferQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE1"])

        # Size Of RX Buffer for Queue1. Should Be Multiple Of 64.
        tcpipGmacRxBuffSizeQue1 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_QUE1", tcpipGmacQue1RxEn)
        tcpipGmacRxBuffSizeQue1.setLabel("Size of RX Buffer (multiple of 64)")
        tcpipGmacRxBuffSizeQue1.setVisible(False)
        tcpipGmacRxBuffSizeQue1.setDescription("Size Of RX Buffer for Queue1. Should Be Multiple Of 64.")
        tcpipGmacRxBuffSizeQue1.setDefaultValue(64)
        tcpipGmacRxBuffSizeQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE1"])
        
        # Max Tx Packet size support for Queue1.
        tcpipGmacMaxTxPktSizeQue1 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_MAX_TX_PKT_SIZE_QUE1", tcpipGmacQue1TxEn)
        tcpipGmacMaxTxPktSizeQue1.setLabel("Max Tx Packet size supported")
        tcpipGmacMaxTxPktSizeQue1.setVisible(False)
        tcpipGmacMaxTxPktSizeQue1.setDescription("Max Tx Packet size")
        if("SAMA5" in processor): #SAMA5D2
            tcpipGmacMaxTxPktSizeQue1.setDefaultValue(1536)
        else: # SAME70 or SAMV71
            tcpipGmacMaxTxPktSizeQue1.setDefaultValue(464)
        tcpipGmacMaxTxPktSizeQue1.setReadOnly(True)
        tcpipGmacMaxTxPktSizeQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_TX_EN_QUE1"])
        
        # Comment for Max Tx Packet size support for Queue1.
        tcpipGmacMaxTxPktSizeQue1Comment = drvGmacComponent.createCommentSymbol("TCPIP_"+ gmacComponentName + "_MAX_TX_PKT_SIZE_COMMENT_QUE1", tcpipGmacQue1TxEn)
        tcpipGmacMaxTxPktSizeQue1Comment.setLabel("*** Transmit packet size for this queue should be less than this limit")
        tcpipGmacMaxTxPktSizeQue1Comment.setVisible(False)
        tcpipGmacMaxTxPktSizeQue1Comment.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_TX_EN_QUE1"])
        
        # Number of additional Rx buffers for Queue1
        tcpipGmacRxAddlBuffCountQue1 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_ADDL_BUFF_COUNT_QUE1", tcpipGmacQue1RxEn)
        tcpipGmacRxAddlBuffCountQue1.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxAddlBuffCountQue1.setLabel("Number of additional Rx buffers")
        tcpipGmacRxAddlBuffCountQue1.setVisible(False)
        tcpipGmacRxAddlBuffCountQue1.setDescription("Number of additional Sticky Rx buffers for Queue1")
        tcpipGmacRxAddlBuffCountQue1.setDefaultValue(0)
        tcpipGmacRxAddlBuffCountQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE1"])  
            
        # Rx buffer Count Threshold to allocate new Rx buffers, for Queue1
        tcpipGmacRxBuffCountThresQue1 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_BUFF_COUNT_THRESHOLD_QUE1", tcpipGmacQue1RxEn)
        tcpipGmacRxBuffCountThresQue1.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxBuffCountThresQue1.setLabel("Minimum Threshold for Rx Buffer replenish process")
        tcpipGmacRxBuffCountThresQue1.setVisible(False)
        tcpipGmacRxBuffCountThresQue1.setDescription("Rx buffer Count Threshold to allocate new Rx buffers for Queue1")
        tcpipGmacRxBuffCountThresQue1.setDefaultValue(1)
        tcpipGmacRxBuffCountThresQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE1"]) 
            
        # Rx buffer allocate Count for Queue1
        tcpipGmacRxBuffAllocCountQue1 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_BUFF_ALLOC_COUNT_QUE1", tcpipGmacQue1RxEn)
        tcpipGmacRxBuffAllocCountQue1.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxBuffAllocCountQue1.setLabel("Rx Buffer allocate count during  replenish process")
        tcpipGmacRxBuffAllocCountQue1.setVisible(False)
        tcpipGmacRxBuffAllocCountQue1.setDescription("Rx buffer allocate Count for Queue1")
        tcpipGmacRxBuffAllocCountQue1.setDefaultValue(1)
        tcpipGmacRxBuffAllocCountQue1.setMin(1)
        tcpipGmacRxBuffAllocCountQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE1"])      
        
        drvGmacIntSrcQue1 = drvGmacComponent.createStringSymbol("DRV_"+ gmacComponentName + "_INT_SRC_QUE1", None)
        drvGmacIntSrcQue1.setVisible(False)
        drvGmacIntSrcQue1.setDefaultValue(gmacComponentName + "_Q1_IRQn")
        drvGmacIntSrcQue1.setDependencies(drvGmacIntSrcEnable, ["TCPIP_"+ gmacComponentName + "_QUEUE_1"])
        
    if("SAMV7" in processor) or ("SAME7" in processor) or ("SAMRH" in processor) or ("SAMA5" in processor) or ("PIC32CZ" in processor) or (("SAMA7G" in processor) and (gmacComponentName == "GMAC0")): # SAME70 or SAMV71 or SAMRH71 or SAMA5D2 or PIC32CZ or SAMA7G5-GMAC0
        # GMAC Queue 2
        tcpipGmacQue2 = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_QUEUE_2", tcpipGmacPrioQueues)
        tcpipGmacQue2.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacQue2.setLabel(gmacComponentName + " Queue 2")
        tcpipGmacQue2.setVisible(True)
        tcpipGmacQue2.setDescription(gmacComponentName + " Queue 2")
        tcpipGmacQue2.setDefaultValue(False)
        #tcpipGmacQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])
                
        # Enable Tx Priority of Queue 2 
        tcpipGmacQue2TxEn = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_TX_EN_QUE2", tcpipGmacQue2)
        tcpipGmacQue2TxEn.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacQue2TxEn.setLabel(gmacComponentName + " Queue 2 Tx Enable")
        tcpipGmacQue2TxEn.setVisible(False)
        tcpipGmacQue2TxEn.setDescription(gmacComponentName + " Queue 2 Tx Enable")
        tcpipGmacQue2TxEn.setDefaultValue(False)
        tcpipGmacQue2TxEn.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_QUEUE_2"])
        
        # Enable Rx Priority of Queue 2 
        tcpipGmacQue2RxEn = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_RX_EN_QUE2", tcpipGmacQue2)
        tcpipGmacQue2RxEn.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacQue2RxEn.setLabel(gmacComponentName + " Queue 2 Rx Enable")
        tcpipGmacQue2RxEn.setVisible(False)
        tcpipGmacQue2RxEn.setDescription(gmacComponentName + " Queue 2 Rx Enable")
        tcpipGmacQue2RxEn.setDefaultValue(False)
        tcpipGmacQue2RxEn.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_QUEUE_2"])
        
        # Number of Tx Descriptors for Queue2
        tcpipGmacTxDescCountQue2 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_QUE2", tcpipGmacQue2TxEn)
        tcpipGmacTxDescCountQue2.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacTxDescCountQue2.setLabel("Number of Tx Descriptors")
        tcpipGmacTxDescCountQue2.setVisible(False)
        tcpipGmacTxDescCountQue2.setDescription("Number of Tx Descriptors for Queue2")
        tcpipGmacTxDescCountQue2.setDefaultValue(1)
        tcpipGmacTxDescCountQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_TX_EN_QUE2"])

        # Number of Rx Descriptors for Queue2
        tcpipGmacRxDescCountQue2 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_QUE2", tcpipGmacQue2RxEn)
        tcpipGmacRxDescCountQue2.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxDescCountQue2.setLabel("Number of Rx Descriptors")
        tcpipGmacRxDescCountQue2.setVisible(False)
        tcpipGmacRxDescCountQue2.setDescription("Number of Rx Descriptors for Queue2")
        tcpipGmacRxDescCountQue2.setDefaultValue(1)
        tcpipGmacRxDescCountQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE2"])
    
        # Number of Dedicated MAC RX Buffers
        tcpipGmacRxOwnBufferQue2 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_DEDICATED_BUFFERS_QUE2", tcpipGmacQue2RxEn)
        tcpipGmacRxOwnBufferQue2.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxOwnBufferQue2.setLabel("Number of Dedicated MAC RX Buffers")
        tcpipGmacRxOwnBufferQue2.setVisible(False)
        tcpipGmacRxOwnBufferQue2.setDescription("Number of Dedicated MAC RX Buffers")
        tcpipGmacRxOwnBufferQue2.setDefaultValue(1)
        tcpipGmacRxOwnBufferQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE2"])

        # Size Of RX Buffer for Queue2. Should Be Multiple Of 64.
        tcpipGmacRxBuffSizeQue2 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_QUE2", tcpipGmacQue2RxEn)
        tcpipGmacRxBuffSizeQue2.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxBuffSizeQue2.setLabel("Size of RX Buffer (multiple of 64)")
        tcpipGmacRxBuffSizeQue2.setVisible(False)
        tcpipGmacRxBuffSizeQue2.setDescription("Size Of RX Buffer for Queue2. Should Be Multiple Of 64.")
        tcpipGmacRxBuffSizeQue2.setDefaultValue(64)
        tcpipGmacRxBuffSizeQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE2"])

        # Max Tx Packet size support for Queue2.
        tcpipGmacMaxTxPktSizeQue2 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_MAX_TX_PKT_SIZE_QUE2", tcpipGmacQue2TxEn)
        tcpipGmacMaxTxPktSizeQue2.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacMaxTxPktSizeQue2.setLabel("Max Tx Packet size supported")
        tcpipGmacMaxTxPktSizeQue2.setVisible(False)
        tcpipGmacMaxTxPktSizeQue2.setDescription("Max Tx Packet size")
        if("SAMA5" in processor): #SAMA5D2
            tcpipGmacMaxTxPktSizeQue2.setDefaultValue(1536)
        else: # SAME70 or SAMV71
            tcpipGmacMaxTxPktSizeQue2.setDefaultValue(464)
        tcpipGmacMaxTxPktSizeQue2.setReadOnly(True)
        tcpipGmacMaxTxPktSizeQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_TX_EN_QUE2"])
        
        # Comment for Max Tx Packet size support for Queue2.
        tcpipGmacMaxTxPktSizeQue2Comment = drvGmacComponent.createCommentSymbol("TCPIP_"+ gmacComponentName + "_MAX_TX_PKT_SIZE_COMMENT_QUE2", tcpipGmacQue2TxEn)
        tcpipGmacMaxTxPktSizeQue2Comment.setLabel("*** Transmit packet size for this queue should be less than this limit")
        tcpipGmacMaxTxPktSizeQue2Comment.setVisible(False)
        tcpipGmacMaxTxPktSizeQue2Comment.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_TX_EN_QUE2"])
        
        # Number of additional Rx buffers for Queue2
        tcpipGmacRxAddlBuffCountQue2 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_ADDL_BUFF_COUNT_QUE2", tcpipGmacQue2RxEn)
        tcpipGmacRxAddlBuffCountQue2.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxAddlBuffCountQue2.setLabel("Number of additional Rx buffers")
        tcpipGmacRxAddlBuffCountQue2.setVisible(False)
        tcpipGmacRxAddlBuffCountQue2.setDescription("Number of additional Sticky Rx buffers for Queue2")
        tcpipGmacRxAddlBuffCountQue2.setDefaultValue(0)
        tcpipGmacRxAddlBuffCountQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE2"])
            
        # Rx buffer Count Threshold to allocate new Rx buffers, for Queue2
        tcpipGmacRxBuffCountThresQue2 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_BUFF_COUNT_THRESHOLD_QUE2", tcpipGmacQue2RxEn)
        tcpipGmacRxBuffCountThresQue2.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxBuffCountThresQue2.setLabel("Minimum Threshold for Rx Buffer replenish process")
        tcpipGmacRxBuffCountThresQue2.setVisible(False)
        tcpipGmacRxBuffCountThresQue2.setDescription("Rx buffer Count Threshold to allocate new Rx buffers for Queue2")
        tcpipGmacRxBuffCountThresQue2.setDefaultValue(1)
        tcpipGmacRxBuffCountThresQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE2"]) 
            
        # Rx buffer allocate Count for Queue2
        tcpipGmacRxBuffAllocCountQue2 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_BUFF_ALLOC_COUNT_QUE2", tcpipGmacQue2RxEn)
        tcpipGmacRxBuffAllocCountQue2.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxBuffAllocCountQue2.setLabel("Rx Buffer allocate count during  replenish process")
        tcpipGmacRxBuffAllocCountQue2.setVisible(False)
        tcpipGmacRxBuffAllocCountQue2.setDescription("Rx buffer allocate Count for Queue2")
        tcpipGmacRxBuffAllocCountQue2.setDefaultValue(1)
        tcpipGmacRxBuffAllocCountQue2.setMin(1)
        tcpipGmacRxBuffAllocCountQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE2"]) 
                
        drvGmacIntSrcQue2 = drvGmacComponent.createStringSymbol("DRV_"+ gmacComponentName + "_INT_SRC_QUE2", None)
        drvGmacIntSrcQue2.setVisible(False)
        drvGmacIntSrcQue2.setDefaultValue(gmacComponentName + "_Q2_IRQn")
        drvGmacIntSrcQue2.setDependencies(drvGmacIntSrcEnable, ["TCPIP_"+ gmacComponentName + "_QUEUE_2"])
        
    if("SAMV7" in processor) or ("SAME7" in processor) or ("SAMRH" in processor) or ("PIC32CZ" in processor) or (("SAMA7G" in processor) and (gmacComponentName == "GMAC0")): # SAME70 or SAMV71 or SAMRH71 or PIC32CZ or SAMA7G5-GMAC0      
        # GMAC Queue 3
        tcpipGmacQue3 = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_QUEUE_3", tcpipGmacPrioQueues)
        tcpipGmacQue3.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacQue3.setLabel(gmacComponentName + " Queue 3")
        tcpipGmacQue3.setVisible(True)
        tcpipGmacQue3.setDescription(gmacComponentName + " Queue 3")
        tcpipGmacQue3.setDefaultValue(False)
        #tcpipGmacQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])
                
        # Enable Tx Priority of Queue 3 
        tcpipGmacQue3TxEn = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_TX_EN_QUE3", tcpipGmacQue3)
        tcpipGmacQue3TxEn.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacQue3TxEn.setLabel(gmacComponentName + " Queue 3 Tx Enable")
        tcpipGmacQue3TxEn.setVisible(False)
        tcpipGmacQue3TxEn.setDescription(gmacComponentName + " Queue 3 Tx Enable")
        tcpipGmacQue3TxEn.setDefaultValue(False)
        tcpipGmacQue3TxEn.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_QUEUE_3"])
        
        # Enable Rx Priority of Queue 3 
        tcpipGmacQue3RxEn = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_RX_EN_QUE3", tcpipGmacQue3)
        tcpipGmacQue3RxEn.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacQue3RxEn.setLabel(gmacComponentName + " Queue 3 Rx Enable")
        tcpipGmacQue3RxEn.setVisible(False)
        tcpipGmacQue3RxEn.setDescription(gmacComponentName + " Queue 3 Rx Enable")
        tcpipGmacQue3RxEn.setDefaultValue(False)
        tcpipGmacQue3RxEn.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_QUEUE_3"])        
        
        # Number of Tx Descriptors for Queue3
        tcpipGmacTxDescCountQue3 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_QUE3", tcpipGmacQue3TxEn)
        tcpipGmacTxDescCountQue3.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacTxDescCountQue3.setLabel("Number of Tx Descriptors")
        tcpipGmacTxDescCountQue3.setVisible(False)
        tcpipGmacTxDescCountQue3.setDescription("Number of Tx Descriptors for Queue3")
        tcpipGmacTxDescCountQue3.setDefaultValue(1)
        tcpipGmacTxDescCountQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_TX_EN_QUE3"])

        # Number of Rx Descriptors for Queue3
        tcpipGmacRxDescCountQue3 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_QUE3", tcpipGmacQue3RxEn)
        tcpipGmacRxDescCountQue3.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxDescCountQue3.setLabel("Number of Rx Descriptors")
        tcpipGmacRxDescCountQue3.setVisible(False)
        tcpipGmacRxDescCountQue3.setDescription("Number of Rx Descriptors for Queue3")
        tcpipGmacRxDescCountQue3.setDefaultValue(1)
        tcpipGmacRxDescCountQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE3"])
    
        # Number of Dedicated MAC RX Buffers
        tcpipGmacRxOwnBufferQue3 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_DEDICATED_BUFFERS_QUE3", tcpipGmacQue3RxEn)
        tcpipGmacRxOwnBufferQue3.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxOwnBufferQue3.setLabel("Number of Dedicated MAC RX Buffers")
        tcpipGmacRxOwnBufferQue3.setVisible(False)
        tcpipGmacRxOwnBufferQue3.setDescription("Number of Dedicated MAC RX Buffers")
        tcpipGmacRxOwnBufferQue3.setDefaultValue(1)
        tcpipGmacRxOwnBufferQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE3"])
        
        # Size Of RX Buffer for Queue3. Should Be Multiple Of 64.
        tcpipGmacRxBuffSizeQue3 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_QUE3", tcpipGmacQue3RxEn)
        tcpipGmacRxBuffSizeQue3.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxBuffSizeQue3.setLabel("Size of RX Buffer (multiple of 64)")
        tcpipGmacRxBuffSizeQue3.setVisible(False)
        tcpipGmacRxBuffSizeQue3.setDescription("Size Of RX Buffer for Queue3. Should Be Multiple Of 64.")
        tcpipGmacRxBuffSizeQue3.setDefaultValue(64)
        tcpipGmacRxBuffSizeQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE3"])
                
        # Max Tx Packet size support for Queue3.
        tcpipGmacMaxTxPktSizeQue3 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_MAX_TX_PKT_SIZE_QUE3", tcpipGmacQue3TxEn)
        tcpipGmacMaxTxPktSizeQue3.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacMaxTxPktSizeQue3.setLabel("Max Tx Packet size supported")
        tcpipGmacMaxTxPktSizeQue3.setVisible(False)
        tcpipGmacMaxTxPktSizeQue3.setDescription("Max Tx Packet size")
        tcpipGmacMaxTxPktSizeQue3.setDefaultValue(1536)
        tcpipGmacMaxTxPktSizeQue3.setReadOnly(True)
        tcpipGmacMaxTxPktSizeQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_TX_EN_QUE3"])
        
        # Comment for Max Tx Packet size support for Queue3.
        tcpipGmacMaxTxPktSizeQue3Comment = drvGmacComponent.createCommentSymbol("TCPIP_"+ gmacComponentName + "_MAX_TX_PKT_SIZE_COMMENT_QUE3", tcpipGmacQue3TxEn)
        tcpipGmacMaxTxPktSizeQue3Comment.setLabel("*** Transmit packet size for this queue should be less than this limit")
        tcpipGmacMaxTxPktSizeQue3Comment.setVisible(False)
        tcpipGmacMaxTxPktSizeQue3Comment.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_TX_EN_QUE3"])
                
        # Number of additional Rx buffers for Queue3
        tcpipGmacRxAddlBuffCountQue3 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_ADDL_BUFF_COUNT_QUE3", tcpipGmacQue3RxEn)
        tcpipGmacRxAddlBuffCountQue3.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxAddlBuffCountQue3.setLabel("Number of additional Rx buffers")
        tcpipGmacRxAddlBuffCountQue3.setVisible(False)
        tcpipGmacRxAddlBuffCountQue3.setDescription("Number of additional Sticky Rx buffers for Queue3")
        tcpipGmacRxAddlBuffCountQue3.setDefaultValue(0)
        tcpipGmacRxAddlBuffCountQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE3"])
            
        # Rx buffer Count Threshold to allocate new Rx buffers, for Queue3
        tcpipGmacRxBuffCountThresQue3 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_BUFF_COUNT_THRESHOLD_QUE3", tcpipGmacQue3RxEn)
        tcpipGmacRxBuffCountThresQue3.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxBuffCountThresQue3.setLabel("Minimum Threshold for Rx Buffer replenish process")
        tcpipGmacRxBuffCountThresQue3.setVisible(False)
        tcpipGmacRxBuffCountThresQue3.setDescription("Rx buffer Count Threshold to allocate new Rx buffers for Queue3")
        tcpipGmacRxBuffCountThresQue3.setDefaultValue(1)
        tcpipGmacRxBuffCountThresQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE3"]) 
            
        # Rx buffer allocate Count for Queue3
        tcpipGmacRxBuffAllocCountQue3 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_BUFF_ALLOC_COUNT_QUE3", tcpipGmacQue3RxEn)
        tcpipGmacRxBuffAllocCountQue3.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxBuffAllocCountQue3.setLabel("Rx Buffer allocate count during  replenish process")
        tcpipGmacRxBuffAllocCountQue3.setVisible(False)
        tcpipGmacRxBuffAllocCountQue3.setDescription("Rx buffer allocate Count for Queue3")
        tcpipGmacRxBuffAllocCountQue3.setDefaultValue(1)
        tcpipGmacRxBuffAllocCountQue3.setMin(1)
        tcpipGmacRxBuffAllocCountQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE3"]) 
        
        drvGmacIntSrcQue3 = drvGmacComponent.createStringSymbol("DRV_"+ gmacComponentName + "_INT_SRC_QUE3", None)
        drvGmacIntSrcQue3.setVisible(False)
        drvGmacIntSrcQue3.setDefaultValue(gmacComponentName + "_Q3_IRQn")
        drvGmacIntSrcQue3.setDependencies(drvGmacIntSrcEnable, ["TCPIP_"+ gmacComponentName + "_QUEUE_3"])
        # GMAC Queue 4
        tcpipGmacQue4 = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_QUEUE_4", tcpipGmacPrioQueues)
        tcpipGmacQue4.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacQue4.setLabel(gmacComponentName + " Queue 4")
        tcpipGmacQue4.setVisible(True)
        tcpipGmacQue4.setDescription(gmacComponentName + " Queue 4")
        tcpipGmacQue4.setDefaultValue(False)
        #tcpipGmacQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

        # Enable Tx Priority of Queue 4 
        tcpipGmacQue4TxEn = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_TX_EN_QUE4", tcpipGmacQue4)
        tcpipGmacQue4TxEn.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacQue4TxEn.setLabel(gmacComponentName + " Queue 4 Tx Enable")
        tcpipGmacQue4TxEn.setVisible(False)
        tcpipGmacQue4TxEn.setDescription(gmacComponentName + " Queue 4 Tx Enable")
        tcpipGmacQue4TxEn.setDefaultValue(False)
        tcpipGmacQue4TxEn.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_QUEUE_4"])
        
        # Enable Rx Priority of Queue 4 
        tcpipGmacQue4RxEn = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_RX_EN_QUE4", tcpipGmacQue4)
        tcpipGmacQue4RxEn.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacQue4RxEn.setLabel(gmacComponentName + " Queue 4 Rx Enable")
        tcpipGmacQue4RxEn.setVisible(False)
        tcpipGmacQue4RxEn.setDescription(gmacComponentName + " Queue 4 Rx Enable")
        tcpipGmacQue4RxEn.setDefaultValue(False)
        tcpipGmacQue4RxEn.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_QUEUE_4"])
        
        # Number of Tx Descriptors for Queue4
        tcpipGmacTxDescCountQue4 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_QUE4", tcpipGmacQue4TxEn)
        tcpipGmacTxDescCountQue4.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacTxDescCountQue4.setLabel("Number of Tx Descriptors")
        tcpipGmacTxDescCountQue4.setVisible(False)
        tcpipGmacTxDescCountQue4.setDescription("Number of Tx Descriptors for Queue4")
        tcpipGmacTxDescCountQue4.setDefaultValue(1)
        tcpipGmacTxDescCountQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_TX_EN_QUE4"])

        # Number of Rx Descriptors for Queue4
        tcpipGmacRxDescCountQue4 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_QUE4", tcpipGmacQue4RxEn)
        tcpipGmacRxDescCountQue4.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxDescCountQue4.setLabel("Number of Rx Descriptors")
        tcpipGmacRxDescCountQue4.setVisible(False)
        tcpipGmacRxDescCountQue4.setDescription("Number of Rx Descriptors for Queue4")
        tcpipGmacRxDescCountQue4.setDefaultValue(1)
        tcpipGmacRxDescCountQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE4"])
    
        # Number of Dedicated MAC RX Buffers
        tcpipGmacRxOwnBufferQue4 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_DEDICATED_BUFFERS_QUE4", tcpipGmacQue4RxEn)
        tcpipGmacRxOwnBufferQue4.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxOwnBufferQue4.setLabel("Number of Dedicated MAC RX Buffers")
        tcpipGmacRxOwnBufferQue4.setVisible(False)
        tcpipGmacRxOwnBufferQue4.setDescription("Number of Dedicated MAC RX Buffers")
        tcpipGmacRxOwnBufferQue4.setDefaultValue(1)
        tcpipGmacRxOwnBufferQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE4"])
        
        # Size Of RX Buffer for Queue4. Should Be Multiple Of 64.
        tcpipGmacRxBuffSizeQue4 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_QUE4", tcpipGmacQue4RxEn)
        tcpipGmacRxBuffSizeQue4.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxBuffSizeQue4.setLabel("Size of RX Buffer (multiple of 64)")
        tcpipGmacRxBuffSizeQue4.setVisible(False)
        tcpipGmacRxBuffSizeQue4.setDescription("Size Of RX Buffer for Queue4. Should Be Multiple Of 64.")
        tcpipGmacRxBuffSizeQue4.setDefaultValue(64)
        tcpipGmacRxBuffSizeQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE4"])
                
        # Max Tx Packet size support for Queue4.
        tcpipGmacMaxTxPktSizeQue4 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_MAX_TX_PKT_SIZE_QUE4", tcpipGmacQue4TxEn)
        tcpipGmacMaxTxPktSizeQue4.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacMaxTxPktSizeQue4.setLabel("Max Tx Packet size supported")
        tcpipGmacMaxTxPktSizeQue4.setVisible(False)
        tcpipGmacMaxTxPktSizeQue4.setDescription("Max Tx Packet size")
        tcpipGmacMaxTxPktSizeQue4.setDefaultValue(1536)
        tcpipGmacMaxTxPktSizeQue4.setReadOnly(True)
        tcpipGmacMaxTxPktSizeQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_TX_EN_QUE4"])
        
        # Comment for Max Tx Packet size support for Queue4.
        tcpipGmacMaxTxPktSizeQue4Comment = drvGmacComponent.createCommentSymbol("TCPIP_"+ gmacComponentName + "_MAX_TX_PKT_SIZE_COMMENT_QUE4", tcpipGmacQue4TxEn)
        tcpipGmacMaxTxPktSizeQue4Comment.setLabel("*** Transmit packet size for this queue should be less than this limit")
        tcpipGmacMaxTxPktSizeQue4Comment.setVisible(False)
        tcpipGmacMaxTxPktSizeQue4Comment.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_TX_EN_QUE4"])
        
        # Number of additional Rx buffers for Queue4
        tcpipGmacRxAddlBuffCountQue4 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_ADDL_BUFF_COUNT_QUE4", tcpipGmacQue4RxEn)
        tcpipGmacRxAddlBuffCountQue4.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxAddlBuffCountQue4.setLabel("Number of additional Rx buffers")
        tcpipGmacRxAddlBuffCountQue4.setVisible(False)
        tcpipGmacRxAddlBuffCountQue4.setDescription("Number of additional Sticky Rx buffers for Queue4")
        tcpipGmacRxAddlBuffCountQue4.setDefaultValue(0)
        tcpipGmacRxAddlBuffCountQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE4"])
            
        # Rx buffer Count Threshold to allocate new Rx buffers, for Queue4
        tcpipGmacRxBuffCountThresQue4 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_BUFF_COUNT_THRESHOLD_QUE4", tcpipGmacQue4RxEn)
        tcpipGmacRxBuffCountThresQue4.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxBuffCountThresQue4.setLabel("Minimum Threshold for Rx Buffer replenish process")
        tcpipGmacRxBuffCountThresQue4.setVisible(False)
        tcpipGmacRxBuffCountThresQue4.setDescription("Rx buffer Count Threshold to allocate new Rx buffers for Queue4")
        tcpipGmacRxBuffCountThresQue4.setDefaultValue(1)
        tcpipGmacRxBuffCountThresQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE4"]) 
            
        # Rx buffer allocate Count for Queue4
        tcpipGmacRxBuffAllocCountQue4 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_BUFF_ALLOC_COUNT_QUE4", tcpipGmacQue4RxEn)
        tcpipGmacRxBuffAllocCountQue4.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxBuffAllocCountQue4.setLabel("Rx Buffer allocate count during  replenish process")
        tcpipGmacRxBuffAllocCountQue4.setVisible(False)
        tcpipGmacRxBuffAllocCountQue4.setDescription("Rx buffer allocate Count for Queue4")
        tcpipGmacRxBuffAllocCountQue4.setDefaultValue(1)
        tcpipGmacRxBuffAllocCountQue4.setMin(1)
        tcpipGmacRxBuffAllocCountQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE4"])         
        
        drvGmacIntSrcQue4 = drvGmacComponent.createStringSymbol("DRV_"+ gmacComponentName + "_INT_SRC_QUE4", None)
        drvGmacIntSrcQue4.setVisible(False)
        drvGmacIntSrcQue4.setDefaultValue(gmacComponentName + "_Q4_IRQn")
        drvGmacIntSrcQue4.setDependencies(drvGmacIntSrcEnable, ["TCPIP_"+ gmacComponentName + "_QUEUE_4"])
        
        # GMAC Queue 5
        tcpipGmacQue5 = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_QUEUE_5", tcpipGmacPrioQueues)
        tcpipGmacQue5.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacQue5.setLabel(gmacComponentName + " Queue 5")
        tcpipGmacQue5.setVisible(True)
        tcpipGmacQue5.setDescription(gmacComponentName + " Queue 5")
        tcpipGmacQue5.setDefaultValue(False)
        #tcpipGmacQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

        # Enable Tx Priority of Queue 5 
        tcpipGmacQue5TxEn = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_TX_EN_QUE5", tcpipGmacQue5)
        tcpipGmacQue5TxEn.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacQue5TxEn.setLabel(gmacComponentName + " Queue 5 Tx Enable")
        tcpipGmacQue5TxEn.setVisible(False)
        tcpipGmacQue5TxEn.setDescription(gmacComponentName + " Queue 5 Tx Enable")
        tcpipGmacQue5TxEn.setDefaultValue(False)
        tcpipGmacQue5TxEn.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_QUEUE_5"])
        
        # Enable Rx Priority of Queue 5 
        tcpipGmacQue5RxEn = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_RX_EN_QUE5", tcpipGmacQue5)
        tcpipGmacQue5RxEn.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacQue5RxEn.setLabel(gmacComponentName + " Queue 5 Rx Enable")
        tcpipGmacQue5RxEn.setVisible(False)
        tcpipGmacQue5RxEn.setDescription(gmacComponentName + " Queue 5 Rx Enable")
        tcpipGmacQue5RxEn.setDefaultValue(False)
        tcpipGmacQue5RxEn.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_QUEUE_5"])
        
        # Number of Tx Descriptors for Queue5
        tcpipGmacTxDescCountQue5 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_QUE5", tcpipGmacQue5TxEn)
        tcpipGmacTxDescCountQue5.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacTxDescCountQue5.setLabel("Number of Tx Descriptors")
        tcpipGmacTxDescCountQue5.setVisible(False)
        tcpipGmacTxDescCountQue5.setDescription("Number of Tx Descriptors for Queue5")
        tcpipGmacTxDescCountQue5.setDefaultValue(1)
        tcpipGmacTxDescCountQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_TX_EN_QUE5"])

        # Number of Rx Descriptors for Queue5
        tcpipGmacRxDescCountQue5 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_QUE5", tcpipGmacQue5RxEn)
        tcpipGmacRxDescCountQue5.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxDescCountQue5.setLabel("Number of Rx Descriptors")
        tcpipGmacRxDescCountQue5.setVisible(False)
        tcpipGmacRxDescCountQue5.setDescription("Number of Rx Descriptors for Queue5")
        tcpipGmacRxDescCountQue5.setDefaultValue(1)
        tcpipGmacRxDescCountQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE5"])
    
        # Number of Dedicated MAC RX Buffers
        tcpipGmacRxOwnBufferQue5 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_DEDICATED_BUFFERS_QUE5", tcpipGmacQue5RxEn)
        tcpipGmacRxOwnBufferQue5.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxOwnBufferQue5.setLabel("Number of Dedicated MAC RX Buffers")
        tcpipGmacRxOwnBufferQue5.setVisible(False)
        tcpipGmacRxOwnBufferQue5.setDescription("Number of Dedicated MAC RX Buffers")
        tcpipGmacRxOwnBufferQue5.setDefaultValue(1)
        tcpipGmacRxOwnBufferQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE5"])
        
        # Size Of RX Buffer for Queue5. Should Be Multiple Of 64.
        tcpipGmacRxBuffSizeQue5 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_QUE5", tcpipGmacQue5RxEn)
        tcpipGmacRxBuffSizeQue5.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxBuffSizeQue5.setLabel("Size of RX Buffer (multiple of 64)")
        tcpipGmacRxBuffSizeQue5.setVisible(False)
        tcpipGmacRxBuffSizeQue5.setDescription("Size Of RX Buffer for Queue5. Should Be Multiple Of 64.")
        tcpipGmacRxBuffSizeQue5.setDefaultValue(64)
        tcpipGmacRxBuffSizeQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE5"])
                
        # Max Tx Packet size support for Queue5.
        tcpipGmacMaxTxPktSizeQue5 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_MAX_TX_PKT_SIZE_QUE5", tcpipGmacQue5TxEn)
        tcpipGmacMaxTxPktSizeQue5.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacMaxTxPktSizeQue5.setLabel("Max Tx Packet size supported")
        tcpipGmacMaxTxPktSizeQue5.setVisible(False)
        tcpipGmacMaxTxPktSizeQue5.setDescription("Max Tx Packet size")
        tcpipGmacMaxTxPktSizeQue5.setDefaultValue(976)
        tcpipGmacMaxTxPktSizeQue5.setReadOnly(True)
        tcpipGmacMaxTxPktSizeQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_TX_EN_QUE5"])
        
        # Comment for Max Tx Packet size support for Queue5.
        tcpipGmacMaxTxPktSizeQue5Comment = drvGmacComponent.createCommentSymbol("TCPIP_"+ gmacComponentName + "_MAX_TX_PKT_SIZE_COMMENT_QUE5", tcpipGmacQue5TxEn)
        tcpipGmacMaxTxPktSizeQue5Comment.setLabel("*** Transmit packet size for this queue should be less than this limit")
        tcpipGmacMaxTxPktSizeQue5Comment.setVisible(False)
        tcpipGmacMaxTxPktSizeQue5Comment.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_TX_EN_QUE5"])
        
        # Number of additional Rx buffers for Queue5
        tcpipGmacRxAddlBuffCountQue5 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_ADDL_BUFF_COUNT_QUE5", tcpipGmacQue5RxEn)
        tcpipGmacRxAddlBuffCountQue5.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxAddlBuffCountQue5.setLabel("Number of additional Rx buffers")
        tcpipGmacRxAddlBuffCountQue5.setVisible(False)
        tcpipGmacRxAddlBuffCountQue5.setDescription("Number of additional Sticky Rx buffers for Queue5")
        tcpipGmacRxAddlBuffCountQue5.setDefaultValue(0)
        tcpipGmacRxAddlBuffCountQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE5"])      
            
        # Rx buffer Count Threshold to allocate new Rx buffers, for Queue5
        tcpipGmacRxBuffCountThresQue5 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_BUFF_COUNT_THRESHOLD_QUE5", tcpipGmacQue5RxEn)
        tcpipGmacRxBuffCountThresQue5.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxBuffCountThresQue5.setLabel("Minimum Threshold for Rx Buffer replenish process")
        tcpipGmacRxBuffCountThresQue5.setVisible(False)
        tcpipGmacRxBuffCountThresQue5.setDescription("Rx buffer Count Threshold to allocate new Rx buffers for Queue5")
        tcpipGmacRxBuffCountThresQue5.setDefaultValue(1)
        tcpipGmacRxBuffCountThresQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE5"]) 
            
        # Rx buffer allocate Count for Queue5
        tcpipGmacRxBuffAllocCountQue5 = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_BUFF_ALLOC_COUNT_QUE5", tcpipGmacQue5RxEn)
        tcpipGmacRxBuffAllocCountQue5.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxBuffAllocCountQue5.setLabel("Rx Buffer allocate count during  replenish process")
        tcpipGmacRxBuffAllocCountQue5.setVisible(False)
        tcpipGmacRxBuffAllocCountQue5.setDescription("Rx buffer allocate Count for Queue5")
        tcpipGmacRxBuffAllocCountQue5.setDefaultValue(1)
        tcpipGmacRxBuffAllocCountQue5.setMin(1)
        tcpipGmacRxBuffAllocCountQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_EN_QUE5"])
        
        drvGmacIntSrcQue5 = drvGmacComponent.createStringSymbol("DRV_"+ gmacComponentName + "_INT_SRC_QUE5", None)
        drvGmacIntSrcQue5.setVisible(False)
        drvGmacIntSrcQue5.setDefaultValue(gmacComponentName + "_Q5_IRQn")
        drvGmacIntSrcQue5.setDependencies(drvGmacIntSrcEnable, ["TCPIP_"+ gmacComponentName + "_QUEUE_5"])
        
        # Number of Tx Priority Queues enabled
        tcpipGmacTxQueCnt = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_TX_PRIO_COUNT", tcpipGmacPrioQueues)
        tcpipGmacTxQueCnt.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacTxQueCnt.setLabel("Number of Tx Priority Queues Enabled")
        tcpipGmacTxQueCnt.setVisible(True)
        tcpipGmacTxQueCnt.setDescription("Number of Tx Priority Queues")
        tcpipGmacTxQueCnt.setDefaultValue(1)
        tcpipGmacTxQueCnt.setReadOnly(True)
        tcpipGmacTxQueCnt.setDependencies(tcpipEthMacTxPrioCnt, ["TCPIP_"+ gmacComponentName + "_QUEUE_1", "TCPIP_"+ gmacComponentName + "_QUEUE_2","TCPIP_"+ gmacComponentName + "_QUEUE_3","TCPIP_"+ gmacComponentName + "_QUEUE_4","TCPIP_"+ gmacComponentName + "_QUEUE_5","TCPIP_"+ gmacComponentName + "_TX_EN_QUE1","TCPIP_"+ gmacComponentName + "_TX_EN_QUE2","TCPIP_"+ gmacComponentName + "_TX_EN_QUE3","TCPIP_"+ gmacComponentName + "_TX_EN_QUE4","TCPIP_"+ gmacComponentName + "_TX_EN_QUE5"])

        # Number of Rx Priority Queues enabled
        tcpipGmacRxQueCnt = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_PRIO_COUNT", tcpipGmacPrioQueues)
        tcpipGmacRxQueCnt.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxQueCnt.setLabel("Number of Rx Priority Queues Enabled")
        tcpipGmacRxQueCnt.setVisible(True)
        tcpipGmacRxQueCnt.setDescription("Number of Rx Priority Queues")
        tcpipGmacRxQueCnt.setDefaultValue(1)
        tcpipGmacRxQueCnt.setReadOnly(True)
        tcpipGmacRxQueCnt.setDependencies(tcpipEthMacRxPrioCnt, ["TCPIP_"+ gmacComponentName + "_QUEUE_1", "TCPIP_"+ gmacComponentName + "_QUEUE_2","TCPIP_"+ gmacComponentName + "_QUEUE_3","TCPIP_"+ gmacComponentName + "_QUEUE_4","TCPIP_"+ gmacComponentName + "_QUEUE_5","TCPIP_"+ gmacComponentName + "_RX_EN_QUE1","TCPIP_"+ gmacComponentName + "_RX_EN_QUE2","TCPIP_"+ gmacComponentName + "_RX_EN_QUE3","TCPIP_"+ gmacComponentName + "_RX_EN_QUE4","TCPIP_"+ gmacComponentName + "_RX_EN_QUE5"])

    # Advanced Rx Filters
    tcpipGmacRxFilterMenu = drvGmacComponent.createMenuSymbol("TCPIP_"+ gmacComponentName + "_ADV_RX_FILTER_MENU", tcpipGmacAdvSettings)
    tcpipGmacRxFilterMenu.setLabel(gmacComponentName + " Advanced Rx Filters")
    tcpipGmacRxFilterMenu.setVisible(True)
    tcpipGmacRxFilterMenu.setDescription("Enable Advanced Rx Filter Configurations")
    
    # Accept Multicast Packets matching Hash
    tcpipGmacEthFilterMcastHashAccept = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_FILTER_MCAST_HASH_ACCEPT", tcpipGmacRxFilterMenu)
    tcpipGmacEthFilterMcastHashAccept.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacEthFilterMcastHashAccept.setLabel("Accept Multicast Packets matching Hash")
    tcpipGmacEthFilterMcastHashAccept.setVisible(True)
    tcpipGmacEthFilterMcastHashAccept.setDescription("Accept Multicast Packets matching Hash")
    tcpipGmacEthFilterMcastHashAccept.setDefaultValue(False)

    # Accept Unicast Packets matching Hash
    tcpipGmacEthFilterUcastHashAccept = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_FILTER_UCAST_HASH_ACCEPT", tcpipGmacRxFilterMenu)
    tcpipGmacEthFilterUcastHashAccept.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacEthFilterUcastHashAccept.setLabel("Accept Unicast Packets matching Hash")
    tcpipGmacEthFilterUcastHashAccept.setVisible(True)
    tcpipGmacEthFilterUcastHashAccept.setDescription("Accept Unicast Packets matching Hash")
    tcpipGmacEthFilterUcastHashAccept.setDefaultValue(False)

    # Reject Packets with Wrong CRC
    tcpipGmacEthFilterCrcErrReject = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_FILTER_CRC_ERROR_REJECT", tcpipGmacRxFilterMenu)
    tcpipGmacEthFilterCrcErrReject.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacEthFilterCrcErrReject.setLabel("Reject Packets with Wrong CRC")
    tcpipGmacEthFilterCrcErrReject.setVisible(True)
    tcpipGmacEthFilterCrcErrReject.setDescription("Reject Packets with Wrong CRC")
    tcpipGmacEthFilterCrcErrReject.setDefaultValue(True)

    # Accept Packets with Wrong CRC
    tcpipGmacEthFilterCrcErrAccept = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_FILTER_CRC_ERROR_ACCEPT", tcpipGmacRxFilterMenu)
    tcpipGmacEthFilterCrcErrAccept.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacEthFilterCrcErrAccept.setLabel("Accept Packets with Wrong CRC")
    tcpipGmacEthFilterCrcErrAccept.setVisible(True)
    tcpipGmacEthFilterCrcErrAccept.setDescription("Accept Packets with Wrong CRC")
    tcpipGmacEthFilterCrcErrAccept.setDefaultValue(False)

    # Accept Packets with Maximum Frame Size(1536 bytes)
    tcpipGmacEthFilteFrameAcceptMax = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_FILTER_MAX_FRAME_ACCEPT", tcpipGmacRxFilterMenu)
    tcpipGmacEthFilteFrameAcceptMax.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacEthFilteFrameAcceptMax.setLabel("Accept Packets with Maximum Frame Size(1536 bytes)")
    tcpipGmacEthFilteFrameAcceptMax.setVisible(True) 
    tcpipGmacEthFilteFrameAcceptMax.setDescription("Accept Packets with Maximum Frame Size(1536 bytes)")
    tcpipGmacEthFilteFrameAcceptMax.setDefaultValue(False)  

    # Accept Packets with Frame Error
    tcpipGmacEthFilterFrameErrAccept = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_FILTER_FRAME_ERROR_ACCEPT", tcpipGmacRxFilterMenu)
    tcpipGmacEthFilterFrameErrAccept.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacEthFilterFrameErrAccept.setLabel("Accept Packets with Frame Error")
    tcpipGmacEthFilterFrameErrAccept.setVisible(True)
    tcpipGmacEthFilterFrameErrAccept.setDescription("Accept Packets with Frame Error")
    tcpipGmacEthFilterFrameErrAccept.setDefaultValue(False)

    # Accept Jumbo Packets (upto 10240 bytes)
    tcpipGmacEthFilterJumboFrameAccept = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_FILTER_JUMBO_FRAME_ACCEPT", tcpipGmacRxFilterMenu)
    tcpipGmacEthFilterJumboFrameAccept.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacEthFilterJumboFrameAccept.setLabel("Accept Jumbo Packets (upto 10240 bytes)")
    tcpipGmacEthFilterJumboFrameAccept.setVisible(True)
    tcpipGmacEthFilterJumboFrameAccept.setDescription("Accept Jumbo Packets (upto 10240 bytes)")
    tcpipGmacEthFilterJumboFrameAccept.setDefaultValue(False)   

    if("SAMV7" in processor) or ("SAME7" in processor) or ("SAMRH" in processor) or ("SAMA5" in processor) or ("PIC32CZ" in processor) or ("SAMA7G" in processor): # SAME70 or SAMV71 or SAMA5D2 or PIC32CZ or SAMA7G5
        # Advanced Rx Queue Filters
        tcpipGmacRxQueFilterEnable = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_RX_QUE_FILTER_EN", tcpipGmacRxFilterMenu)
        tcpipGmacRxQueFilterEnable.setHelp("mcc_h3_gmac_configurations")
        tcpipGmacRxQueFilterEnable.setLabel("Advanced Rx Queue Filters")
        tcpipGmacRxQueFilterEnable.setVisible(True)
        tcpipGmacRxQueFilterEnable.setDescription("Enable Advanced Rx Queue Filters")
        tcpipGmacRxQueFilterEnable.setDefaultValue(False)

        tcpipGmacType1Filter(drvGmacComponent,tcpipGmacRxQueFilterEnable)
        tcpipGmacType2Filter(drvGmacComponent,tcpipGmacRxQueFilterEnable)    
        
        # Comment for Rx Filters 
        tcpipGmacRxQueFilterComment = drvGmacComponent.createCommentSymbol("TCPIP_"+ gmacComponentName + "_RX_FILTER_COMMENT_QUE", tcpipGmacRxQueFilterEnable)
        tcpipGmacRxQueFilterComment.setLabel("***When 2 or more filter matches: ")
        tcpipGmacRxQueFilterComment.setVisible(False)
        tcpipGmacRxQueFilterComment.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_QUE_FILTER_EN"])
            
        # Comment 1 for Rx Filters 
        tcpipGmacRxQueFilterComment1 = drvGmacComponent.createCommentSymbol("TCPIP_"+ gmacComponentName + "_RX_FILTER_COMMENT1_QUE", tcpipGmacRxQueFilterEnable)
        tcpipGmacRxQueFilterComment1.setLabel("***Type 1 filter has priority over Type 2 filter")
        tcpipGmacRxQueFilterComment1.setVisible(False)
        tcpipGmacRxQueFilterComment1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_QUE_FILTER_EN"])
            
        # Comment 2 for Rx Filters 
        tcpipGmacRxQueFilterComment2 = drvGmacComponent.createCommentSymbol("TCPIP_"+ gmacComponentName + "_RX_FILTER_COMMENT2_QUE", tcpipGmacRxQueFilterEnable)
        tcpipGmacRxQueFilterComment2.setLabel("***among same Type filters, lower index has priority")
        tcpipGmacRxQueFilterComment2.setVisible(False)
        tcpipGmacRxQueFilterComment2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_"+ gmacComponentName + "_RX_QUE_FILTER_EN"])
    
    # Advanced Connection Flag
    tcpipGmacAdvConnFlagMenu = drvGmacComponent.createMenuSymbol("TCPIP_"+ gmacComponentName + "_ADV_CONN_FLAG_MENU", tcpipGmacAdvSettings)
    tcpipGmacAdvConnFlagMenu.setLabel(gmacComponentName + " Advanced Connection Flag")
    tcpipGmacAdvConnFlagMenu.setVisible(True)
    tcpipGmacAdvConnFlagMenu.setDescription("Enable Advanced Connection Flag")
    
    # Allow Huge Packets
    tcpipGmacEthHugePkt = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_OF_HUGE_PKTS", tcpipGmacAdvConnFlagMenu)
    tcpipGmacEthHugePkt.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacEthHugePkt.setLabel("Allow Huge Packets")
    tcpipGmacEthHugePkt.setVisible(True)
    tcpipGmacEthHugePkt.setDescription("Allow Huge Packets")
    tcpipGmacEthHugePkt.setDefaultValue(False)
    
    # Loopbacked At The MAC Level
    tcpipGmacEthMacLoopBack = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_OF_MAC_LOOPBACK", tcpipGmacAdvConnFlagMenu)
    tcpipGmacEthMacLoopBack.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacEthMacLoopBack.setLabel("Loopbacked At The MAC Level")
    tcpipGmacEthMacLoopBack.setVisible(True) 
    tcpipGmacEthMacLoopBack.setDescription("Loopbacked At The MAC Level")
    tcpipGmacEthMacLoopBack.setDefaultValue(False)
    
    # Loopbacked At The PHY Level
    tcpipGmacEthPhyLoopBack = drvGmacComponent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_ETH_OF_PHY_LOOPBACK", tcpipGmacAdvConnFlagMenu)
    tcpipGmacEthPhyLoopBack.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacEthPhyLoopBack.setLabel("Loopbacked At The PHY Level")
    tcpipGmacEthPhyLoopBack.setVisible(True)
    tcpipGmacEthPhyLoopBack.setDescription("Loopbacked At The PHY Level")
    tcpipGmacEthPhyLoopBack.setDefaultValue(False)
    
    if("SAMA7G" in processor): # Option only for SAMA7G5 
        drvGmacRefClkSrc = drvGmacComponent.createKeyValueSetSymbol("DRV_"+ gmacComponentName + "_REF_CLK_SRC",tcpipGmacAdvSettings)
        drvGmacRefClkSrc.setVisible(True)
        drvGmacRefClkSrc.setLabel("Reference Clock Source")
        drvGmacRefClkSrc.addKey("INT_GCLK", "0", "Internal GCLK")
        drvGmacRefClkSrc.addKey("EXT_CLK", "1", "External Clock")
        drvGmacRefClkSrc.setDisplayMode("Description")
        drvGmacRefClkSrc.setOutputMode("Value")
        drvGmacRefClkSrc.setDefaultValue(0)
        
        drvGmacRefClkSrcComment = drvGmacComponent.createCommentSymbol("DRV_"+ gmacComponentName + "_REF_CLK_SRC_COMMENT", drvGmacRefClkSrc)
        drvGmacRefClkSrcComment.setLabel("*** ensure to setup the selected clock source")
        drvGmacRefClkSrcComment.setVisible(True)
    
    # GMAC RX Checksum offload Enable
    drvGmacRxChksmOffload = drvGmacComponent.createBooleanSymbol("DRV_"+ gmacComponentName + "_RX_CHKSM_OFFLOAD", tcpipGmacAdvSettings)
    drvGmacRxChksmOffload.setHelp("mcc_h3_gmac_configurations")
    drvGmacRxChksmOffload.setLabel("Enable Rx Checksum Offload")
    drvGmacRxChksmOffload.setVisible(True)
    drvGmacRxChksmOffload.setDescription("Enable Rx Checksum Offload")
    drvGmacRxChksmOffload.setDefaultValue(False)

    # GMAC TX Checksum offload Enable
    drvGmacTxChksmOffload = drvGmacComponent.createBooleanSymbol("DRV_"+ gmacComponentName + "_TX_CHKSM_OFFLOAD", tcpipGmacAdvSettings)
    drvGmacTxChksmOffload.setHelp("mcc_h3_gmac_configurations")
    drvGmacTxChksmOffload.setLabel("Enable Tx Checksum Offload")
    drvGmacTxChksmOffload.setVisible(True)
    drvGmacTxChksmOffload.setDescription("Enable Tx Checksum Offload")
    drvGmacTxChksmOffload.setDefaultValue(False)

    # Maximum MAC Supported RX Frame Size
    tcpipGmacRxFrameMax = drvGmacComponent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_RX_MAX_FRAME", tcpipGmacAdvSettings)
    tcpipGmacRxFrameMax.setHelp("mcc_h3_gmac_configurations")
    tcpipGmacRxFrameMax.setLabel("RX Frame Maximum Size")
    tcpipGmacRxFrameMax.setVisible(False)
    tcpipGmacRxFrameMax.setDescription("RX Frame Maximum Size Supported")
    tcpipGmacRxFrameMax.setDefaultValue(1536)
    
    # MAC Maximum Number of Supported Fragments
    tcpipEmacRxFragNumMax = drvGmacComponent.createIntegerSymbol("TCPIP_EMAC_RX_FRAGMENTS", tcpipGmacAdvSettings)
    tcpipEmacRxFragNumMax.setHelp("mcc_h3_gmac_configurations")
    tcpipEmacRxFragNumMax.setLabel("Maximum Number of Fragments")
    tcpipEmacRxFragNumMax.setMax(6)
    tcpipEmacRxFragNumMax.setVisible(True)
    tcpipEmacRxFragNumMax.setDescription("Maximum Number of Supported Fragments")
    tcpipEmacRxFragNumMax.setDefaultValue(1)    
        
    # GMAC Module ID
    tcpipEmacModuleId = drvGmacComponent.createStringSymbol("TCPIP_" + gmacComponentName + "_MODULE_ID", tcpipGmacAdvSettings)
    tcpipEmacModuleId.setHelp("mcc_h3_gmac_configurations")
    tcpipEmacModuleId.setLabel(gmacComponentName + " Module ID")
    tcpipEmacModuleId.setVisible(True)
    tcpipEmacModuleId.setDescription(gmacComponentName + " Module ID")
    if "PIC32CZ" in processor: # PIC32CZ
        tcpipEmacModuleId.setDefaultValue("ETH_BASE_ADDRESS")
    elif (("SAMA7G" in processor) and (gmacComponentName == "GMAC0")): # SAMA7G5-GMAC0
        tcpipEmacModuleId.setDefaultValue("GMAC0_BASE_ADDRESS") 
    elif (("SAMA7G" in processor) and (gmacComponentName == "GMAC1")): # SAMA7G5-GMAC1  
        tcpipEmacModuleId.setDefaultValue("GMAC1_BASE_ADDRESS")
    else:
        tcpipEmacModuleId.setDefaultValue("GMAC_BASE_ADDRESS")
        
    # Driver GMAC Number of queues
    drvGmacQueueNum = drvGmacComponent.createIntegerSymbol("DRV_"+ gmacComponentName + "_NUMBER_OF_QUEUES", None)
    drvGmacQueueNum.setHelp("mcc_h3_gmac_configurations")
    drvGmacQueueNum.setLabel(gmacComponentName + " Instances Number")
    drvGmacQueueNum.setVisible(False)
    drvGmacQueueNum.setDescription("Number of GMAC Queues")
    if("SAMV7" in processor) or ("SAME7" in processor) or ("SAMRH" in processor) or ("PIC32CZ" in processor) or (("SAMA7G" in processor) and (gmacComponentName == "GMAC0")): # SAME70, SAMV71, PIC32CZ, SAMA7G5-GMAC0
        drvGmacQueueNum.setDefaultValue(6)  
    elif (("SAMA7G" in processor) and (gmacComponentName == "GMAC1")): # SAMA7G5-GMAC1
        drvGmacQueueNum.setDefaultValue(2)
    elif ("SAME5" in processor): # SAME54
        drvGmacQueueNum.setDefaultValue(1)
    elif ("SAMA5" in processor): # SAMA5D2
        drvGmacQueueNum.setDefaultValue(3)
    
    # Driver GMAC RMII Mode Selection Value
    drvGmacRmiiVal = drvGmacComponent.createIntegerSymbol("DRV_"+ gmacComponentName + "_RMII_VALUE", None)
    drvGmacRmiiVal.setHelp("mcc_h3_gmac_configurations")
    drvGmacRmiiVal.setVisible(False)
    if("SAMV7" in processor) or ("SAME7" in processor) or ("SAMRH" in processor) or ("SAME5" in processor) :  # SAME70, SAMV71, SAMRH, SAME54
        drvGmacRmiiVal.setDefaultValue(0) 
    elif ("SAMA5" in processor): # SAMA5D2
        drvGmacRmiiVal.setDefaultValue(1)

    # PHY Connected to GMAC
    drvGmacPhyType = drvGmacComponent.createStringSymbol("DRV_"+ gmacComponentName + "_PHY_TYPE", tcpipGmacAdvSettings)
    drvGmacPhyType.setHelp("mcc_h3_gmac_configurations")
    drvGmacPhyType.setLabel("External PHY Connected to MAC")
    drvGmacPhyType.setVisible(True)
    drvGmacPhyType.setDescription("PHY Connected to GMAC")
    drvGmacPhyType.setDefaultValue("Not Connected")
    drvGmacPhyType.setReadOnly(True)
    
    if("SAMV7" in processor) or ("SAME7" in processor) or ("SAMRH" in processor) or ("PIC32CZ" in processor): # SAME70, SAMV71, SAMRH71, PIC32CZ
        # Menu for Non-Cacheable Memory region
        drvGmacNoCacheMenu = drvGmacComponent.createMenuSymbol("DRV_"+ gmacComponentName + "_NO_CACHE_MENU", tcpipGmacAdvSettings)
        drvGmacNoCacheMenu.setHelp("mcc_h3_gmac_configurations")
        drvGmacNoCacheMenu.setLabel("Non-Cacheable Memory Region for GMAC Descriptors")
        drvGmacNoCacheMenu.setVisible(True)
        drvGmacNoCacheMenu.setDescription(" Non-Cacheable Memory Region for GMAC Descriptors")
        
        drvGmacNoCacheConfig = drvGmacComponent.createBooleanSymbol("DRV_"+ gmacComponentName + "_NO_CACHE_CONFIG", drvGmacNoCacheMenu)
        drvGmacNoCacheConfig.setHelp("mcc_h3_gmac_configurations")
        drvGmacNoCacheConfig.setLabel("Create Non-Cacheable Memory Region in Data Memory")
        drvGmacNoCacheConfig.setVisible(True)
        drvGmacNoCacheConfig.setDefaultValue(True)
        drvGmacNoCacheConfig.setDescription("Create Non-Cacheable Memory Region in Data Memory (Internal SRAM)") 
            
        drvGmacNoCacheMemRegSize = drvGmacComponent.createKeyValueSetSymbol("DRV_"+ gmacComponentName + "_NOCACHE_REGION_SIZE", drvGmacNoCacheConfig)
        drvGmacNoCacheMemRegSize.setHelp("mcc_h3_gmac_configurations")
        drvGmacNoCacheMemRegSize.setLabel("Non-Cacheable Memory Size")
        drvGmacNoCacheMemRegSize.setOutputMode("Value")
        drvGmacNoCacheMemRegSize.setDisplayMode("Description")
        drvGmacNoCacheMemRegSize.addKey("32B", str(4) , "32 Bytes" )
        drvGmacNoCacheMemRegSize.addKey("64B", str(5) , "64 bytes" )
        drvGmacNoCacheMemRegSize.addKey("128B", str(6) , "128 bytes" )
        drvGmacNoCacheMemRegSize.addKey("256B", str(7) , "256 bytes" )
        drvGmacNoCacheMemRegSize.addKey("512B", str(8) , "512 bytes" )
        drvGmacNoCacheMemRegSize.addKey("1KB", str(9) , "1 KB" )
        drvGmacNoCacheMemRegSize.addKey("2KB", str(10) , "2 KB" )
        drvGmacNoCacheMemRegSize.addKey("4KB", str(11) , "4 KB" )
        drvGmacNoCacheMemRegSize.addKey("8KB", str(12) , "8 KB" )
        drvGmacNoCacheMemRegSize.addKey("16KB", str(13) , "16 KB" )
        drvGmacNoCacheMemRegSize.addKey("32KB", str(14) , "32 KB" )
        drvGmacNoCacheMemRegSize.setDefaultValue(7) 
        drvGmacNoCacheMemRegSize.setVisible(True) 
        drvGmacNoCacheMemRegSize.setDependencies(tcpipEthMacMenuVisibleSingle, ["DRV_"+ gmacComponentName + "_NO_CACHE_CONFIG"])

        drvGmacNoCacheMemAddressComment = drvGmacComponent.createCommentSymbol("DRV_"+ gmacComponentName + "_NOCACHE_MEM_ADDRESS_COMMENT", drvGmacNoCacheConfig)
        drvGmacNoCacheMemAddressComment.setLabel("***Start Address must be aligned to Memory Size***")
        drvGmacNoCacheMemAddressComment.setVisible(True)
        drvGmacNoCacheMemAddressComment.setDependencies(tcpipEthMacMenuVisibleSingle, ["DRV_"+ gmacComponentName + "_NO_CACHE_CONFIG"]) 
        
        drvGmacNoCacheMemAddress = drvGmacComponent.createHexSymbol("DRV_"+ gmacComponentName + "_NOCACHE_MEM_ADDRESS",drvGmacNoCacheConfig)
        drvGmacNoCacheMemAddress.setHelp("mcc_h3_gmac_configurations")
        drvGmacNoCacheMemAddress.setLabel("Start Address of Non-Cacheable Memory")
        drvGmacNoCacheMemAddress.setVisible(True)  
        drvGmacNoCacheMemAddress.setMin(0)
        if("SAMV7" in processor) or ("SAME7" in processor):
            drvGmacNoCacheMemAddress.setDefaultValue(0x2045F000)  
        elif ("SAMRH" in processor):
            drvGmacNoCacheMemAddress.setDefaultValue(0x2105F000)            
        elif ("PIC32CZ" in processor): # PIC32CZ
            drvGmacNoCacheMemAddress.setDefaultValue(0x2009F000)  
            
        drvGmacNoCacheMemAddress.setDependencies(tcpipEthMacMenuVisibleSingle, ["DRV_"+ gmacComponentName + "_NO_CACHE_CONFIG"]) 

        initNoCacheMPU()
        
        drvGmacNoCachebool = drvGmacComponent.createBooleanSymbol("DRV_"+ gmacComponentName + "_NO_CACHE_CONFIG_BOOL", drvGmacNoCacheConfig)
        drvGmacNoCachebool.setHelp("mcc_h3_gmac_configurations")
        drvGmacNoCachebool.setLabel("Non-Cacheable Memory Region Configuration")
        drvGmacNoCachebool.setVisible(False)
        drvGmacNoCachebool.setDefaultValue(False)
        drvGmacNoCachebool.setDependencies(drvMacMPUConfig, ["DRV_"+ gmacComponentName + "_NO_CACHE_CONFIG", "DRV_"+ gmacComponentName + "_NOCACHE_REGION_SIZE", "DRV_"+ gmacComponentName + "_NOCACHE_MEM_ADDRESS"]) 
        
        # Set global MPU symbols
        setVal("tcpipStack", "TCPIP_STACK_NO_CACHE_CONFIG", drvGmacNoCacheConfig.getValue())
        setVal("tcpipStack", "TCPIP_STACK_NOCACHE_MEM_ADDRESS", drvGmacNoCacheMemAddress.getValue())
        setVal("tcpipStack", "TCPIP_STACK_NOCACHE_SIZE",drvGmacNoCacheMemRegSize.getKeyForValue(drvGmacNoCacheMemRegSize.getSelectedValue()) )
               
    
    # Driver GMAC Heap Size
    drvGmacHeapSize = drvGmacComponent.createIntegerSymbol("DRV_"+ gmacComponentName + "_HEAP_SIZE", None)
    drvGmacHeapSize.setHelp("mcc_h3_gmac_configurations")
    drvGmacHeapSize.setLabel(gmacComponentName + " Heap Size (bytes)") 
    drvGmacHeapSize.setVisible(False)
    drvGmacHeapSize.setDescription(gmacComponentName + " Heap Size")
    drvGmacHeapSize.setDefaultValue(gmacHeapCalc())
    drvGmacHeapSize.setReadOnly(True)
    drvGmacHeapSize.setDependencies(drvGmacHeapUpdate, gmacheapdependency)
    
    #Add to definitions.h
    tcpipGmacSystemDefFile = drvGmacComponent.createFileSymbol("GMAC_H_FILE", None)
    tcpipGmacSystemDefFile.setType("STRING")
    tcpipGmacSystemDefFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    tcpipGmacSystemDefFile.setSourcePath("driver/gmac/templates/system/system_definitions.h.ftl")
    tcpipGmacSystemDefFile.setMarkup(True)  
    
    #Add forward declaration to initialization.c
    tcpipGmacInitDataSourceFtl = drvGmacComponent.createFileSymbol(None, None)
    tcpipGmacInitDataSourceFtl.setType("STRING")
    tcpipGmacInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    tcpipGmacInitDataSourceFtl.setSourcePath("driver/gmac/templates/system/system_driver_initialize.c.ftl")
    tcpipGmacInitDataSourceFtl.setMarkup(True)
    
    #Add to initialization.c
    tcpipGmacSysInitDataSourceFtl = drvGmacComponent.createFileSymbol(None, None)
    tcpipGmacSysInitDataSourceFtl.setType("STRING")
    tcpipGmacSysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    tcpipGmacSysInitDataSourceFtl.setSourcePath("driver/gmac/templates/system/system_data_initialize.c.ftl")
    tcpipGmacSysInitDataSourceFtl.setMarkup(True)
    
    #Add to system_config.h
    tcpipGmacHeaderFtl = drvGmacComponent.createFileSymbol(None, None)
    tcpipGmacHeaderFtl.setSourcePath("driver/gmac/config/drv_intmac_gmac.h.ftl")
    tcpipGmacHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipGmacHeaderFtl.setMarkup(True)
    tcpipGmacHeaderFtl.setType("STRING")
        
    # file TCPIP_MAC_DRV_H "$HARMONY_VERSION_PATH/framework/driver/gmac/drv_gmac.h" to                     "$PROJECT_HEADER_FILES/framework/driver/gmac/drv_gmac.h"
    # Add drv_gmac.h file to project
    drvGmacHeaderFile = drvGmacComponent.createFileSymbol(None, None)
    drvGmacHeaderFile.setSourcePath("driver/gmac/drv_gmac.h")
    drvGmacHeaderFile.setOutputName("drv_gmac.h")
    drvGmacHeaderFile.setDestPath("driver/gmac/")
    drvGmacHeaderFile.setProjectPath("config/" + configName + "/driver/gmac/")
    drvGmacHeaderFile.setType("HEADER")
    drvGmacHeaderFile.setOverwrite(True)

    # file TCPIP_MAC_LOCAL_H "$HARMONY_VERSION_PATH/framework/driver/gmac/src/drv_gmac_local.h" to         "$PROJECT_HEADER_FILES/framework/driver/gmac/src/drv_gmac_local.h"   

    # Add drv_gmac_local.h file to project
    drvGmacLocalHeaderFile = drvGmacComponent.createFileSymbol(None, None)
    drvGmacLocalHeaderFile.setSourcePath("driver/gmac/src/drv_gmac_local.h")
    drvGmacLocalHeaderFile.setOutputName("drv_gmac_local.h")
    drvGmacLocalHeaderFile.setDestPath("driver/gmac/src/")
    drvGmacLocalHeaderFile.setProjectPath("config/" + configName + "/driver/gmac/src/")
    drvGmacLocalHeaderFile.setType("HEADER")
    drvGmacLocalHeaderFile.setOverwrite(True)

    # file TCPIP_MAC_DESC_H "$HARMONY_VERSION_PATH/framework/driver/gmac/src/dynamic/_gmac_dcpt_lists.h" to "$PROJECT_HEADER_FILES/framework/driver/gmac/src/dynamic/_gmac_dcpt_lists.h"

    # Add _gmac_dcpt_lists.h file to project
    drvGmacDcptHeaderFile = drvGmacComponent.createFileSymbol(None, None)
    drvGmacDcptHeaderFile.setSourcePath("driver/gmac/src/dynamic/_gmac_dcpt_lists.h")
    drvGmacDcptHeaderFile.setOutputName("_gmac_dcpt_lists.h")
    drvGmacDcptHeaderFile.setDestPath("driver/gmac/src/dynamic/")
    drvGmacDcptHeaderFile.setProjectPath("config/" + configName + "/driver/gmac/src/dynamic/")
    drvGmacDcptHeaderFile.setType("HEADER")
    drvGmacDcptHeaderFile.setOverwrite(True)
    #drvGmacDcptHeaderFile.setDependencies(drvGmacGenHeaderFile, ["TCPIP_USE_ETH_MAC"])

    # Add drv_gmac_lib_sam.h file to project
    drvGmacLibSamHeaderFile = drvGmacComponent.createFileSymbol(None, None)
    drvGmacLibSamHeaderFile.setSourcePath("driver/gmac/src/dynamic/drv_gmac_lib_sam.h")
    drvGmacLibSamHeaderFile.setOutputName("drv_gmac_lib_sam.h")
    drvGmacLibSamHeaderFile.setDestPath("driver/gmac/src/dynamic/")
    drvGmacLibSamHeaderFile.setProjectPath("config/" + configName + "/driver/gmac/src/dynamic/")
    drvGmacLibSamHeaderFile.setType("HEADER")
    drvGmacLibSamHeaderFile.setOverwrite(True)

    # file TCPIP_MAC_LIB_H "$HARMONY_VERSION_PATH/framework/driver/gmac/src/dynamic/drv_gmac_lib.h" to     "$PROJECT_HEADER_FILES/framework/driver/gmac/src/dynamic/drv_gmac_lib.h"
    # Add drv_gmac_lib.h file to project
    drvGmacLibHeaderFile = drvGmacComponent.createFileSymbol(None, None)
    drvGmacLibHeaderFile.setSourcePath("driver/gmac/src/dynamic/drv_gmac_lib.h")
    drvGmacLibHeaderFile.setOutputName("drv_gmac_lib.h")
    drvGmacLibHeaderFile.setDestPath("driver/gmac/src/dynamic/")
    drvGmacLibHeaderFile.setProjectPath("config/" + configName + "/driver/gmac/src/dynamic/")
    drvGmacLibHeaderFile.setType("HEADER")
    drvGmacLibHeaderFile.setOverwrite(True)



    # file TCPIP_MAC_DRV_C "$HARMONY_VERSION_PATH/framework/driver/gmac/src/dynamic/drv_gmac.c" to         "$PROJECT_SOURCE_FILES/framework/driver/gmac/drv_gmac.c"
    # Add drv_gmac.c file
    drvGmacSourceFile = drvGmacComponent.createFileSymbol(None, None)
    drvGmacSourceFile.setSourcePath("driver/gmac/src/dynamic/drv_gmac.c")
    drvGmacSourceFile.setOutputName("drv_gmac.c")
    drvGmacSourceFile.setOverwrite(True)
    drvGmacSourceFile.setDestPath("driver/gmac/src/dynamic/")
    drvGmacSourceFile.setProjectPath("config/" + configName + "/driver/gmac/src/dynamic/")
    drvGmacSourceFile.setType("SOURCE")
    drvGmacSourceFile.setEnabled(True)

    # file TCPIP_MAC_LIB_C "$HARMONY_VERSION_PATH/framework/driver/gmac/src/dynamic/drv_gmac_lib.c" to     "$PROJECT_SOURCE_FILES/framework/driver/gmac/drv_gmac_lib.c"
    # Add drv_gmac_lib.c file
    drvGmacLibCFileStem = "drv_gmac_lib"

    if (("SAME7" in processor) or ("SAMV7" in processor)):  
        drvGmacLibCFileStem = drvGmacLibCFileStem + "_samE7x_V7x"
    elif ("SAME5" in processor):
        drvGmacLibCFileStem = drvGmacLibCFileStem + "_samE5x"
    elif ("SAMA5D2" in processor):      
        drvGmacLibCFileStem = drvGmacLibCFileStem + "_samA5D2"
    elif ("PIC32CZ" in processor): 
        drvGmacLibCFileStem = drvGmacLibCFileStem + "_pic32CZ"
    elif ("SAMA7G" in processor):
        drvGmacLibCFileStem = drvGmacLibCFileStem + "_samA7G5"

    drvGmacLibSourceFile = drvGmacComponent.createFileSymbol(None, None)
    drvGmacLibSourceFile.setSourcePath("driver/gmac/src/dynamic/" + drvGmacLibCFileStem + ".c")
    drvGmacLibSourceFile.setOutputName(drvGmacLibCFileStem + ".c")
    drvGmacLibSourceFile.setOverwrite(True)
    drvGmacLibSourceFile.setDestPath("driver/gmac/src/dynamic/")
    drvGmacLibSourceFile.setProjectPath("config/" + configName + "/driver/gmac/src/dynamic/")
    drvGmacLibSourceFile.setType("SOURCE")
    drvGmacLibSourceFile.setEnabled(True)

        
def tcpipEthMacMenuVisibleSingle(symbol, event):
    if (event["value"] == True):      
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)


def tcpipEthMacSetClear(symbol, event):
    if (event["value"] == True):     
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)
        symbol.clearValue()
        
def tcpipEthMacTxPrioCnt(symbol, event):
    global gmacComponentId
    global gmacComponentName
    tx_count = 1
    if((Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_QUEUE_1") == True) and (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_TX_EN_QUE1") == True)):
            tx_count = tx_count + 1
    if((Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_QUEUE_2") == True) and (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_TX_EN_QUE2") == True)):
            tx_count = tx_count + 1
    if((Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_QUEUE_3") == True) and (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_TX_EN_QUE3") == True)):
            tx_count = tx_count + 1     
    if((Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_QUEUE_4") == True) and (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_TX_EN_QUE4") == True)):
            tx_count = tx_count + 1    
    if((Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_QUEUE_5") == True) and (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_TX_EN_QUE5") == True)):
            tx_count = tx_count + 1            
    
    symbol.setValue(tx_count) 

def tcpipEthMacRxPrioCnt(symbol, event):
    global gmacComponentId
    global gmacComponentName
    rx_count = 1
    if((Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_QUEUE_1") == True) and (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_EN_QUE1") == True)):
            rx_count = rx_count + 1
    if((Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_QUEUE_2") == True) and (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_EN_QUE2") == True)):
            rx_count = rx_count + 1
    if((Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_QUEUE_3") == True) and (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_EN_QUE3") == True)):
            rx_count = rx_count + 1     
    if((Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_QUEUE_4") == True) and (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_EN_QUE4") == True)):
            rx_count = rx_count + 1    
    if((Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_QUEUE_5") == True) and (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_EN_QUE5") == True)):
            rx_count = rx_count + 1            
    
    symbol.setValue(rx_count)         
        
def tcpipEthMacMdixSwapVisible(symbol, event):
    global gmacComponentId
    global gmacComponentName
    tcpipEthMacAutoMdix = Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_ETH_OF_MDIX_AUTO")

    if (event["value"] == True):
        symbol.setVisible(False)
    else:
        symbol.setVisible(True)
        
def tcpipEthMacRMIIMode(symbol, event): 
    global tcpipGmacEthMii
    global tcpipGmacEthGmii
    if event["value"] == True:
        tcpipGmacEthMii.setReadOnly(True)
        tcpipGmacEthGmii.setReadOnly(True)
        for interface in range (0, len(interfaceNum)): 
            setVal("tcpipStack", "TCPIP_STACK_MII_MODE_IDX" + str(interfaceNum[interface]), "RMII")
    else:
        tcpipGmacEthMii.setReadOnly(False)
        tcpipGmacEthGmii.setReadOnly(False)
  

def tcpipEthMacMIIMode(symbol, event):     
    global gmacComponentId
    global gmacComponentName
    global interfaceNum
    interface = 0
    if (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_ETH_OF_RMII") != None):
        global tcpipGmacEthRmii
        if(Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_ETH_OF_RMII") == True):
            setVal("tcpipStack", "TCPIP_STACK_MII_MODE_IDX" + str(interfaceNum[interface]), "RMII")      
    if (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_ETH_OF_MII") != None):
        global tcpipGmacEthMii
        if(Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_ETH_OF_MII") == True):
            setVal("tcpipStack", "TCPIP_STACK_MII_MODE_IDX" + str(interfaceNum[interface]), "MII")
    if (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_ETH_OF_GMII") != None):
        global tcpipGmacEthGmii
        if(Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_ETH_OF_GMII") == True):
            setVal("tcpipStack", "TCPIP_STACK_MII_MODE_IDX" + str(interfaceNum[interface]), "GMII")
    if (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_ETH_OF_RGMII") != None):
        global tcpipGmacEthRgmii
        if(Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_ETH_OF_RGMII") == True):
            setVal("tcpipStack", "TCPIP_STACK_MII_MODE_IDX" + str(interfaceNum[interface]), "RGMII")
            
def tcpipGmacClockUpdate(symbol, event): 
    setVal("tcpipStack", "TCPIP_STACK_MAC_CLOCK", int(event["value"]))
    
def tcpipPic32CZGmacClockUpdate(symbol, event): 
    coreComponent = Database.getComponentByID("core")
    clkdivSymbol = coreComponent.getSymbolByID("CONF_MCLK_CLKDIV1")       
    main_clock = int(Database.getSymbolValue("core", "MAIN_CLOCK_FREQUENCY"))
    clock_div = int(clkdivSymbol.getSelectedValue(), 16)
    peripheral_clock = main_clock/clock_div
    setVal("tcpipStack", "TCPIP_STACK_MAC_CLOCK", int(peripheral_clock))
        
def tcpipGmacGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])

def drvGmacGenHeaderFile(headerFile, event):
    headerFile.setEnabled(event["value"])

def onAttachmentConnected(source, target):
    global tcpipGmacEthRmii
    global gmacComponentId
    global gmacComponentName
    global interfaceNum
    global processor
    if (source["id"] == gmacComponentName + "_PHY_Dependency"): 
        Database.setSymbolValue(gmacComponentId, "DRV_"+ gmacComponentName + "_PHY_TYPE", target["component"].getDisplayName(),2)
        extPhyComponent = "drvExtPhy" + target['component'].getDisplayName().capitalize()
        setVal(extPhyComponent, "DRV_ETHPHY_MAC_NAME", gmacComponentName)
        setVal(extPhyComponent, "DRV_ETHPHY_PERIPHERAL_ID", gmacComponentName + "_BASE_ADDRESS") 
    elif (target["id"] == "NETCONFIG_MAC_Dependency"):
        interface_number = int(target["component"].getID().strip("tcpipNetConfig_"))
        interfaceNum.append(interface_number)
        setVal("tcpipStack", "TCPIP_STACK_INT_MAC_IDX" + str(interface_number), True)
        incVal("tcpipStack", "TCPIP_STACK_INTMAC_INTERFACE_NUM")
        if (("SAMA7G" in processor) and (gmacComponentName == "GMAC0")): # GMAC0 of SAMA7G54
            setVal("tcpipStack", "TCPIP_STACK_MII_MODE_IDX" + str(interface_number), "RGMII")
        else:
            setVal("tcpipStack", "TCPIP_STACK_MII_MODE_IDX" + str(interface_number), "RMII" if tcpipGmacEthRmii.getValue() == True else "MII")
        
def onAttachmentDisconnected(source, target):
    global gmacComponentId
    global gmacComponentName
    global interfaceNum
    if (source["id"] == gmacComponentName + "_PHY_Dependency"): 
        Database.clearSymbolValue(gmacComponentId, "DRV_"+ gmacComponentName + "_PHY_TYPE")
        Database.setSymbolValue(gmacComponentId, "DRV_"+ gmacComponentName + "_PHY_TYPE", "Not Connected")
        extPhyComponent = "drvExtPhy" + target['component'].getDisplayName().capitalize()
        setVal(extPhyComponent, "DRV_ETHPHY_MAC_NAME", "")
        setVal(extPhyComponent, "DRV_ETHPHY_PERIPHERAL_ID", "")
    elif (target["id"] == "NETCONFIG_MAC_Dependency"):
        interface_number = int(target["component"].getID().strip("tcpipNetConfig_"))
        interfaceNum.remove(interface_number)
        setVal("tcpipStack", "TCPIP_STACK_INT_MAC_IDX" + str(interface_number), False)
        setVal("tcpipStack", "TCPIP_STACK_MII_MODE_IDX" + str(interface_number), "")    
        decVal("tcpipStack", "TCPIP_STACK_INTMAC_INTERFACE_NUM")


def tcpipGmacQueScreen1(symbol, event):
    global screen1_filter_count
    global gmacComponentName
    if "TCPIP_"+ gmacComponentName + "_RX_QUE_FILTER_EN" in event["id"]:
        if (event["value"] == True):
            symbol.setVisible(True)
        else:
            symbol.setVisible(False)
    else:
        count  = event["value"]       
        
        symCom = symbol.getComponent()               
        
        if(count > screen1_filter_count):    
            if (screen1_filter_count < screen1_filter_max_count):
                while (screen1_filter_count < count):         
                    symId = symCom.getSymbolByID("TCPIP_"+ gmacComponentName + "_QUE_SCREEN1_FILTER_IDX"+ str(screen1_filter_count))
                    symId.setVisible(True)
                    # screen1_que_filter_count[queIndex] = screen1_que_filter_count[queIndex] + 1
                    screen1_filter_count = screen1_filter_count + 1
        else:
            if (screen1_filter_count <= screen1_filter_max_count):
                while (screen1_filter_count > count): 
                    symId = symCom.getSymbolByID("TCPIP_"+ gmacComponentName + "_QUE_SCREEN1_FILTER_IDX"+ str(screen1_filter_count - 1))
                    symId.setVisible(False)
                    # screen1_que_filter_count[queIndex] = screen1_que_filter_count[queIndex] - 1
                    screen1_filter_count = screen1_filter_count - 1
            


def tcpipGmacQueScreen2(symbol, event):
    global screen2_filter_count
    global gmacComponentName
    if "TCPIP_"+ gmacComponentName + "_RX_QUE_FILTER_EN" in event["id"]:
        if (event["value"] == True):
            symbol.setVisible(True)
        else:
            symbol.setVisible(False)
    else:
        count  = event["value"]
        
        symCom = symbol.getComponent() 
       
        if(count > screen2_filter_count):    
            if (screen2_filter_count < screen2_filter_max_count):
                while (screen2_filter_count < count):         
                    symId = symCom.getSymbolByID("TCPIP_"+ gmacComponentName + "_QUE_SCREEN2_FILTER_IDX"+ str(screen2_filter_count))
                    symId.setVisible(True)
                    # screen2_que_filter_count[queIndex] = screen2_que_filter_count[queIndex] + 1
                    screen2_filter_count = screen2_filter_count + 1
        else:
            if (screen2_filter_count <= screen2_filter_max_count):
                while (screen2_filter_count > count): 
                    symId = symCom.getSymbolByID("TCPIP_"+ gmacComponentName + "_QUE_SCREEN2_FILTER_IDX"+ str(screen2_filter_count - 1))
                    symId.setVisible(False)
                    # screen2_que_filter_count[queIndex] = screen2_que_filter_count[queIndex] - 1
                    screen2_filter_count = screen2_filter_count - 1
              
def tcpipGmacType1Filter(parent,menu):     
    global gmac_periphID  
    global gmacComponentName
    global processor  
    # Queue1 Screen 1 filter count
    tcpipGmacQueScreen1Cnt = parent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_SCREEN1_COUNT_QUE", menu)
    tcpipGmacQueScreen1Cnt.setLabel("TYPE 1 Filter count")
    tcpipGmacQueScreen1Cnt.setVisible(False)
    tcpipGmacQueScreen1Cnt.setDescription("TYPE 1 Filter count")
    tcpipGmacQueScreen1Cnt.setDefaultValue(0)
    tcpipGmacQueScreen1Cnt.setMax(screen1_filter_max_count)
    tcpipGmacQueScreen1Cnt.setMin(0)
    tcpipGmacQueScreen1Cnt.setDependencies(tcpipGmacQueScreen1, ["TCPIP_"+ gmacComponentName + "_RX_QUE_FILTER_EN","TCPIP_"+ gmacComponentName + "_SCREEN1_COUNT_QUE"]) 

    for index in range(0,screen1_filter_max_count): 
        tcpipGmacQueScreen1Filt.append(parent.createMenuSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN1_FILTER_IDX" + str(index),tcpipGmacQueScreen1Cnt))
        tcpipGmacQueScreen1Filt[index].setLabel("TYPE 1 Filter Index "+ str(index))
        tcpipGmacQueScreen1Filt[index].setVisible(False)     
        
        # Comment for Rx Queue Enable 
        tcpipGmacRxQueScreen1EnableComment.append(parent.createCommentSymbol("TCPIP_"+ gmacComponentName + "_RX_QUE_SCREEN1_COMMENT_IDX" + str(index), tcpipGmacQueScreen1Filt[index]))
        tcpipGmacRxQueScreen1EnableComment[index].setLabel("***Make sure to enable the selected RX Queue***")
        tcpipGmacRxQueScreen1EnableComment[index].setVisible(True)
        
        tcpipGmacScreen1Que.append(parent.createKeyValueSetSymbol("TCPIP_"+ gmacComponentName + "_SCREEN1_QUE_IDX"+str(index),tcpipGmacQueScreen1Filt[index]))
        tcpipGmacScreen1Que[index].setLabel(gmacComponentName + " Rx Que")
        tcpipGmacScreen1Que[index].setVisible(True)  
        tcpipGmacScreen1Que[index].addKey(gmacComponentName + "_QUE_0", "0", "Rx Queue 0")
        tcpipGmacScreen1Que[index].addKey(gmacComponentName + "_QUE_1", "1", "Rx Queue 1")
        if not (("SAMA7G" in processor) and (gmacComponentName == "GMAC1")): # SAMA7G5-GMAC1 only support Q0 and Q1      
            tcpipGmacScreen1Que[index].addKey(gmacComponentName + "_QUE_2", "2", "Rx Queue 2")
        if("SAMV7" in processor) or ("SAME7" in processor) or ("SAMRH" in processor) or ("PIC32CZ" in processor) or (("SAMA7G" in processor) and (gmacComponentName == "GMAC0")): # SAME70 or SAMV71 or PIC32CZ or SAMA7G5-GMAC0   
            tcpipGmacScreen1Que[index].addKey(gmacComponentName + "_QUE_3", "3", "Rx Queue 3")
            tcpipGmacScreen1Que[index].addKey(gmacComponentName + "_QUE_4", "4", "Rx Queue 4")
            tcpipGmacScreen1Que[index].addKey(gmacComponentName + "_QUE_5", "5", "Rx Queue 5")
        tcpipGmacScreen1Que[index].setDisplayMode("Description")
        tcpipGmacScreen1Que[index].setOutputMode("Value")            
        tcpipGmacScreen1Que[index].setDefaultValue(0)           
    
        tcpipGmacQueScreen1DSTCE.append(parent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN1_DSTCEN_IDX" + str(index),tcpipGmacQueScreen1Filt[index]))
        tcpipGmacQueScreen1DSTCE[index].setLabel("Compare DS/TC field of IPv4/IPv6")
        tcpipGmacQueScreen1DSTCE[index].setVisible(True)
        
        tcpipGmacQueScreen1DSTC.append(parent.createHexSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN1_DSTC_IDX"+str(index),tcpipGmacQueScreen1DSTCE[index]))
        tcpipGmacQueScreen1DSTC[index].setLabel("DS/TC field of IPv4/IPv6")
        tcpipGmacQueScreen1DSTC[index].setVisible(False)  
        tcpipGmacQueScreen1DSTC[index].setMin(0)
        tcpipGmacQueScreen1DSTC[index].setDefaultValue(0)
        tcpipGmacQueScreen1DSTC[index].setMax(0xff)
        tcpipGmacQueScreen1DSTC[index].setDependencies(tcpipEthMacMenuVisibleSingle, [tcpipGmacQueScreen1DSTCE[index].getID()])       
        
        tcpipGmacQueScreen1UDPE.append(parent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN1_UDPEN_IDX" + str(index),tcpipGmacQueScreen1Filt[index]))
        tcpipGmacQueScreen1UDPE[index].setLabel("Compare destination UDP Port Number")
        tcpipGmacQueScreen1UDPE[index].setVisible(True)       
        
        tcpipGmacQueScreen1UDPPortNum.append(parent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN1_UDPPORTNUM_IDX" + str(index),tcpipGmacQueScreen1UDPE[index]))
        tcpipGmacQueScreen1UDPPortNum[index].setLabel("Destination UDP Port Number")
        tcpipGmacQueScreen1UDPPortNum[index].setVisible(False)  
        tcpipGmacQueScreen1UDPPortNum[index].setMin(0)
        tcpipGmacQueScreen1UDPPortNum[index].setDefaultValue(0)
        tcpipGmacQueScreen1UDPPortNum[index].setMax(65535)
        tcpipGmacQueScreen1UDPPortNum[index].setDependencies(tcpipEthMacMenuVisibleSingle, [tcpipGmacQueScreen1UDPE[index].getID()])
            
def tcpipGmacType2Filter(parent,menu):
    global gmac_periphID
    global gmacComponentName
    global processor 
    # Queue1 Screen 2 filter count
    tcpipGmacQueScreen2Cnt = parent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_SCREEN2_COUNT_QUE" , menu)
    tcpipGmacQueScreen2Cnt.setLabel("TYPE 2 Filter count")
    tcpipGmacQueScreen2Cnt.setVisible(False)
    tcpipGmacQueScreen2Cnt.setDescription("TYPE 2 Filter count")
    tcpipGmacQueScreen2Cnt.setDefaultValue(0)
    tcpipGmacQueScreen2Cnt.setMax(screen2_filter_max_count)
    tcpipGmacQueScreen2Cnt.setMin(0)
    tcpipGmacQueScreen2Cnt.setDependencies(tcpipGmacQueScreen2, ["TCPIP_"+ gmacComponentName + "_RX_QUE_FILTER_EN" ,"TCPIP_"+ gmacComponentName + "_SCREEN2_COUNT_QUE"])

    for index in range(0,screen2_filter_max_count): 
        tcpipGmacQueScreen2Filt.append(parent.createMenuSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN2_FILTER_IDX"+ str(index),tcpipGmacQueScreen2Cnt))
        tcpipGmacQueScreen2Filt[index].setLabel("TYPE 2 Filter Index "+ str(index))
        tcpipGmacQueScreen2Filt[index].setVisible(False)
        
        # Comment for Rx Queue Enable 
        tcpipGmacRxQueScreen2EnableComment.append(parent.createCommentSymbol("TCPIP_"+ gmacComponentName + "_RX_QUE_SCREEN2_COMMENT_IDX" + str(index), tcpipGmacQueScreen2Filt[index]))
        tcpipGmacRxQueScreen2EnableComment[index].setLabel("***Make sure to enable the selected RX Queue***")
        tcpipGmacRxQueScreen2EnableComment[index].setVisible(True)
        
        tcpipGmacScreen2Que.append(parent.createKeyValueSetSymbol("TCPIP_"+ gmacComponentName + "_SCREEN2_QUE_IDX"+str(index),tcpipGmacQueScreen2Filt[index]))
        tcpipGmacScreen2Que[index].setLabel(gmacComponentName + " Rx Que")
        tcpipGmacScreen2Que[index].setVisible(True)  
        tcpipGmacScreen2Que[index].addKey(gmacComponentName + "_QUE_0", "0", "Rx Queue 0")
        tcpipGmacScreen2Que[index].addKey(gmacComponentName + "_QUE_1", "1", "Rx Queue 1")
        if not (("SAMA7G" in processor) and (gmacComponentName == "GMAC1")): # SAMA7G5-GMAC1 only support Q0 and Q1     
            tcpipGmacScreen2Que[index].addKey(gmacComponentName + "_QUE_2", "2", "Rx Queue 2")
        if("SAMV7" in processor) or ("SAME7" in processor) or ("SAMRH" in processor) or ("PIC32CZ" in processor) or (("SAMA7G" in processor) and (gmacComponentName == "GMAC0")): # SAME70 or SAMV71 or PIC32CZ or SAMA7G5-GMAC0   
            tcpipGmacScreen2Que[index].addKey(gmacComponentName + "_QUE_3", "3", "Rx Queue 3")
            tcpipGmacScreen2Que[index].addKey(gmacComponentName + "_QUE_4", "4", "Rx Queue 4")
            tcpipGmacScreen2Que[index].addKey(gmacComponentName + "_QUE_5", "5", "Rx Queue 5")
        tcpipGmacScreen2Que[index].setDisplayMode("Description")
        tcpipGmacScreen2Que[index].setOutputMode("Value")            
        tcpipGmacScreen2Que[index].setDefaultValue(0)           

        tcpipGmacQueScreen2VlanPrioEn.append(parent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN2_VLANPRIOEN_IDX"+str(index),tcpipGmacQueScreen2Filt[index]))
        tcpipGmacQueScreen2VlanPrioEn[index].setLabel("Compare VLAN Priority")
        tcpipGmacQueScreen2VlanPrioEn[index].setVisible(True)   
        tcpipGmacQueScreen2VlanPrioEn[index].setDefaultValue(0)   
        tcpipGmacQueScreen2VlanPrioEn[index].setReadOnly(True) # Tempararily disabled, until VLAN is supported

        tcpipGmacQueScreen2VlanPrio.append(parent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN2_VLANPRIO_IDX"+str(index),tcpipGmacQueScreen2VlanPrioEn[index]))
        tcpipGmacQueScreen2VlanPrio[index].setLabel("VLAN Priority")
        tcpipGmacQueScreen2VlanPrio[index].setVisible(False)  
        tcpipGmacQueScreen2VlanPrio[index].setMin(0)
        tcpipGmacQueScreen2VlanPrio[index].setDefaultValue(0)
        tcpipGmacQueScreen2VlanPrio[index].setMax(7)
        tcpipGmacQueScreen2VlanPrio[index].setDependencies(tcpipEthMacMenuVisibleSingle, [tcpipGmacQueScreen2VlanPrioEn[index].getID()])      
        
        tcpipGmacQueScreen2EthTypeEn.append(parent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN2_ETHTYPEEN_IDX"+str(index),tcpipGmacQueScreen2Filt[index]))
        tcpipGmacQueScreen2EthTypeEn[index].setLabel("Compare Ether Type")
        tcpipGmacQueScreen2EthTypeEn[index].setVisible(True) 
        
        tcpipGmacQueScreen2EthType.append(parent.createHexSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN2_ETHTYPE_IDX"+str(index),tcpipGmacQueScreen2EthTypeEn[index]))
        tcpipGmacQueScreen2EthType[index].setLabel("Ether Type")
        tcpipGmacQueScreen2EthType[index].setVisible(False)  
        tcpipGmacQueScreen2EthType[index].setMin(0)
        tcpipGmacQueScreen2EthType[index].setDefaultValue(0)
        tcpipGmacQueScreen2EthType[index].setMax(0xffff)
        tcpipGmacQueScreen2EthType[index].setDependencies(tcpipEthMacMenuVisibleSingle, [tcpipGmacQueScreen2EthTypeEn[index].getID()])
        
        tcpipGmacQueScreen2CompAEn.append(parent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN2_COMPAEN_IDX"+str(index),tcpipGmacQueScreen2Filt[index]))
        tcpipGmacQueScreen2CompAEn[index].setLabel("Compare with COMPA")
        tcpipGmacQueScreen2CompAEn[index].setVisible(True)     
        
        tcpipGmacQueScreen2CompA.append(parent.createHexSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN2_COMPA_IDX"+str(index),tcpipGmacQueScreen2CompAEn[index]))
        tcpipGmacQueScreen2CompA[index].setLabel("Compare A Value")
        tcpipGmacQueScreen2CompA[index].setVisible(False)  
        tcpipGmacQueScreen2CompA[index].setMin(0)
        tcpipGmacQueScreen2CompA[index].setDefaultValue(0)
        tcpipGmacQueScreen2CompA[index].setMax(0xffff)
        tcpipGmacQueScreen2CompA[index].setDependencies(tcpipEthMacMenuVisibleSingle, [tcpipGmacQueScreen2CompAEn[index].getID()])        

        tcpipGmacQueScreen2CompBEn.append(parent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN2_COMPBEN_IDX"+str(index),tcpipGmacQueScreen2Filt[index]))
        tcpipGmacQueScreen2CompBEn[index].setLabel("Compare with COMPB")
        tcpipGmacQueScreen2CompBEn[index].setVisible(True)     
        
        tcpipGmacQueScreen2CompB.append(parent.createHexSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN2_COMPB_IDX"+str(index),tcpipGmacQueScreen2CompBEn[index]))
        tcpipGmacQueScreen2CompB[index].setLabel("Compare B Value")
        tcpipGmacQueScreen2CompB[index].setVisible(False)  
        tcpipGmacQueScreen2CompB[index].setMin(0)
        tcpipGmacQueScreen2CompB[index].setDefaultValue(0)
        tcpipGmacQueScreen2CompB[index].setMax(0xffff)
        tcpipGmacQueScreen2CompB[index].setDependencies(tcpipEthMacMenuVisibleSingle, [tcpipGmacQueScreen2CompBEn[index].getID()])         

        tcpipGmacQueScreen2CompCEn.append(parent.createBooleanSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN2_COMPCEN_IDX"+str(index),tcpipGmacQueScreen2Filt[index]))
        tcpipGmacQueScreen2CompCEn[index].setLabel("Compare with COMPC")
        tcpipGmacQueScreen2CompCEn[index].setVisible(True)     
        
        tcpipGmacQueScreen2CompC.append(parent.createHexSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN2_COMPC_IDX"+str(index),tcpipGmacQueScreen2CompCEn[index]))
        tcpipGmacQueScreen2CompC[index].setLabel("Compare C Value")
        tcpipGmacQueScreen2CompC[index].setVisible(False)  
        tcpipGmacQueScreen2CompC[index].setMin(0)
        tcpipGmacQueScreen2CompC[index].setDefaultValue(0)
        tcpipGmacQueScreen2CompC[index].setMax(0xffff)
        tcpipGmacQueScreen2CompC[index].setDependencies(tcpipEthMacMenuVisibleSingle, [tcpipGmacQueScreen2CompCEn[index].getID()])   
        
        tcpipGmacQueScreen2CompAMask.append(parent.createHexSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN2_COMPAMASK_IDX"+str(index),tcpipGmacQueScreen2CompAEn[index]))
        tcpipGmacQueScreen2CompAMask[index].setLabel("Compare A Mask")
        tcpipGmacQueScreen2CompAMask[index].setVisible(False)  
        tcpipGmacQueScreen2CompAMask[index].setMin(0)
        tcpipGmacQueScreen2CompAMask[index].setDefaultValue(0)
        tcpipGmacQueScreen2CompAMask[index].setMax(0xffff)
        tcpipGmacQueScreen2CompAMask[index].setDependencies(tcpipEthMacMenuVisibleSingle, [tcpipGmacQueScreen2CompAEn[index].getID()])        
        
        tcpipGmacQueScreen2CompBMask.append(parent.createHexSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN2_COMPBMASK_IDX"+str(index),tcpipGmacQueScreen2CompBEn[index]))
        tcpipGmacQueScreen2CompBMask[index].setLabel("Compare B Mask")
        tcpipGmacQueScreen2CompBMask[index].setVisible(False)  
        tcpipGmacQueScreen2CompBMask[index].setMin(0)
        tcpipGmacQueScreen2CompBMask[index].setDefaultValue(0)
        tcpipGmacQueScreen2CompBMask[index].setMax(0xffff)
        tcpipGmacQueScreen2CompBMask[index].setDependencies(tcpipEthMacMenuVisibleSingle, [tcpipGmacQueScreen2CompBEn[index].getID()]) 
        
        tcpipGmacQueScreen2CompCMask.append(parent.createHexSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN2_COMPCMASK_IDX"+str(index),tcpipGmacQueScreen2CompCEn[index]))
        tcpipGmacQueScreen2CompCMask[index].setLabel("Compare C Mask")
        tcpipGmacQueScreen2CompCMask[index].setVisible(False)  
        tcpipGmacQueScreen2CompCMask[index].setMin(0)
        tcpipGmacQueScreen2CompCMask[index].setDefaultValue(0)
        tcpipGmacQueScreen2CompCMask[index].setMax(0xffff)
        tcpipGmacQueScreen2CompCMask[index].setDependencies(tcpipEthMacMenuVisibleSingle, [tcpipGmacQueScreen2CompCEn[index].getID()])  
        
        tcpipGmacQueScreen2CompAOffset.append(parent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN2_COMPAOFFST_IDX"+str(index),tcpipGmacQueScreen2CompAEn[index]))
        tcpipGmacQueScreen2CompAOffset[index].setLabel("Offset (in bytes)")
        tcpipGmacQueScreen2CompAOffset[index].setVisible(False)  
        tcpipGmacQueScreen2CompAOffset[index].setMin(0)
        tcpipGmacQueScreen2CompAOffset[index].setDefaultValue(0)
        tcpipGmacQueScreen2CompAOffset[index].setMax(127)
        tcpipGmacQueScreen2CompAOffset[index].setDependencies(tcpipEthMacMenuVisibleSingle, [tcpipGmacQueScreen2CompAEn[index].getID()])
        
        tcpipGmacQueScreen2CompAOffsetStart.append(parent.createKeyValueSetSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN2_COMPAOFFSTSTRT_IDX"+str(index),tcpipGmacQueScreen2CompAEn[index]))
        tcpipGmacQueScreen2CompAOffsetStart[index].setLabel("Offset Starts from")
        tcpipGmacQueScreen2CompAOffsetStart[index].setVisible(False)  
        tcpipGmacQueScreen2CompAOffsetStart[index].addKey("TYPE2_START_OF_FRAME", "0", "Start of Frame")
        tcpipGmacQueScreen2CompAOffsetStart[index].addKey("TYPE2_ETHERTYPE", "1", "EtherType field")
        tcpipGmacQueScreen2CompAOffsetStart[index].addKey("TYPE2_IP_HEADER", "2", "IP Header field")
        tcpipGmacQueScreen2CompAOffsetStart[index].addKey("TYPE2_TCP_UDP_HEADER", "3", "TCP/UDP Header field")
        tcpipGmacQueScreen2CompAOffsetStart[index].setDisplayMode("Description")
        tcpipGmacQueScreen2CompAOffsetStart[index].setOutputMode("Value")            
        tcpipGmacQueScreen2CompAOffsetStart[index].setDefaultValue(0)
        tcpipGmacQueScreen2CompAOffsetStart[index].setDependencies(tcpipEthMacMenuVisibleSingle, [tcpipGmacQueScreen2CompAEn[index].getID()]) 
        
        tcpipGmacQueScreen2CompBOffset.append(parent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN2_COMPBOFFST_IDX"+str(index),tcpipGmacQueScreen2CompBEn[index]))
        tcpipGmacQueScreen2CompBOffset[index].setLabel("Offset (in bytes)")
        tcpipGmacQueScreen2CompBOffset[index].setVisible(False)  
        tcpipGmacQueScreen2CompBOffset[index].setMin(0)
        tcpipGmacQueScreen2CompBOffset[index].setDefaultValue(0)
        tcpipGmacQueScreen2CompBOffset[index].setMax(127)
        tcpipGmacQueScreen2CompBOffset[index].setDependencies(tcpipEthMacMenuVisibleSingle, [tcpipGmacQueScreen2CompBEn[index].getID()])
        
        tcpipGmacQueScreen2CompBOffsetStart.append(parent.createKeyValueSetSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN2_COMPBOFFSTSTRT_IDX"+str(index),tcpipGmacQueScreen2CompBEn[index]))
        tcpipGmacQueScreen2CompBOffsetStart[index].setLabel("Offset Starts from")
        tcpipGmacQueScreen2CompBOffsetStart[index].setVisible(False)  
        tcpipGmacQueScreen2CompBOffsetStart[index].addKey("TYPE2_START_OF_FRAME", "0", "Start of Frame")
        tcpipGmacQueScreen2CompBOffsetStart[index].addKey("TYPE2_ETHERTYPE", "1", "EtherType field")
        tcpipGmacQueScreen2CompBOffsetStart[index].addKey("TYPE2_IP_HEADER", "2", "IP Header field")
        tcpipGmacQueScreen2CompBOffsetStart[index].addKey("TYPE2_TCP_UDP_HEADER", "3", "TCP/UDP Header field")
        tcpipGmacQueScreen2CompBOffsetStart[index].setDisplayMode("Description")
        tcpipGmacQueScreen2CompBOffsetStart[index].setOutputMode("Value")            
        tcpipGmacQueScreen2CompBOffsetStart[index].setDefaultValue(0)
        tcpipGmacQueScreen2CompBOffsetStart[index].setDependencies(tcpipEthMacMenuVisibleSingle, [tcpipGmacQueScreen2CompBEn[index].getID()])
        
        tcpipGmacQueScreen2CompCOffset.append(parent.createIntegerSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN2_COMPCOFFST_IDX"+str(index),tcpipGmacQueScreen2CompCEn[index]))
        tcpipGmacQueScreen2CompCOffset[index].setLabel("Offset (in bytes)")
        tcpipGmacQueScreen2CompCOffset[index].setVisible(False)  
        tcpipGmacQueScreen2CompCOffset[index].setMin(0)
        tcpipGmacQueScreen2CompCOffset[index].setDefaultValue(0)
        tcpipGmacQueScreen2CompCOffset[index].setMax(127)
        tcpipGmacQueScreen2CompCOffset[index].setDependencies(tcpipEthMacMenuVisibleSingle, [tcpipGmacQueScreen2CompCEn[index].getID()])
        
        tcpipGmacQueScreen2CompCOffsetStart.append(parent.createKeyValueSetSymbol("TCPIP_"+ gmacComponentName + "_QUE_SCREEN2_COMPCOFFSTSTRT_IDX"+str(index),tcpipGmacQueScreen2CompCEn[index]))
        tcpipGmacQueScreen2CompCOffsetStart[index].setLabel("Offset Starts from")
        tcpipGmacQueScreen2CompCOffsetStart[index].setVisible(False)  
        tcpipGmacQueScreen2CompCOffsetStart[index].addKey("TYPE2_START_OF_FRAME", "0", "Start of Frame")
        tcpipGmacQueScreen2CompCOffsetStart[index].addKey("TYPE2_ETHERTYPE", "1", "EtherType field")
        tcpipGmacQueScreen2CompCOffsetStart[index].addKey("TYPE2_IP_HEADER", "2", "IP Header field")
        tcpipGmacQueScreen2CompCOffsetStart[index].addKey("TYPE2_TCP_UDP_HEADER", "3", "TCP/UDP Header field")
        tcpipGmacQueScreen2CompCOffsetStart[index].setDisplayMode("Description")
        tcpipGmacQueScreen2CompCOffsetStart[index].setOutputMode("Value")            
        tcpipGmacQueScreen2CompCOffsetStart[index].setDefaultValue(0)
        tcpipGmacQueScreen2CompCOffsetStart[index].setDependencies(tcpipEthMacMenuVisibleSingle, [tcpipGmacQueScreen2CompCEn[index].getID()])      

def tcpipGmacTxDescCountQue0CallBack(symbol, event):
    print event["value"]
    if (event["value"] == "Low"):     
        symbol.setValue(4)
    elif(event["value"] == "Medium"):   
        symbol.setValue(8)
    else:
        symbol.setValue(10)
            
def tcpipGmacRxDescCountQue0CallBack(symbol, event):
    print event["value"]
    if (event["value"] == "Low"):     
        symbol.setValue(4)
    elif(event["value"] == "Medium"):   
        symbol.setValue(8)
    else:
        symbol.setValue(10)

def tcpipGmacRxOwnBufferQue0CallBack(symbol, event):
    print event["value"]
    if (event["value"] == "Low"):     
        symbol.setValue(4)
    elif(event["value"] == "Medium"):   
        symbol.setValue(8)
    else:
        symbol.setValue(10)
        
def tcpipGmacRxBuffSizeQue0CallBack(symbol, event):
    print event["value"]
    if (event["value"] == "Low"):     
        symbol.setValue(1536)
    elif(event["value"] == "Medium"):   
        symbol.setValue(1536)
    else:
        symbol.setValue(1536)
        
def tcpipGmacRxAddlBuffCountQue0CallBack(symbol, event):
    print event["value"]
    if (event["value"] == "Low"):     
        symbol.setValue(0)
    elif(event["value"] == "Medium"):   
        symbol.setValue(2)
    else:
        symbol.setValue(4)

def tcpipGmacRxBuffCountThresQue0CallBack(symbol, event):
    print event["value"]
    if (event["value"] == "Low"):     
        symbol.setValue(1)
    elif(event["value"] == "Medium"):   
        symbol.setValue(1)
    else:
        symbol.setValue(2)
            
def tcpipGmacRxBuffAllocCountQue0CallBack(symbol, event):
    print event["value"]
    if (event["value"] == "Low"):     
        symbol.setValue(1)
    elif(event["value"] == "Medium"):   
        symbol.setValue(2)
    else:
        symbol.setValue(3)

def drvMacMPUConfig(symbol, event):
    global drvGmacNoCacheMemRegSize
    global noCache_MPU_index
    global gmacComponentId
    global gmacComponentName
    coreComponent = Database.getComponentByID("core")
    
    if (noCache_MPU_index != 0xff): 
        if(Database.getSymbolValue(gmacComponentId, "DRV_"+ gmacComponentName + "_NO_CACHE_CONFIG") == True):
            Database.setSymbolValue("core", ("MPU_Region_" + str(noCache_MPU_index) + "_Enable"), True)
            Database.setSymbolValue("core", ("MPU_Region_Name" + str(noCache_MPU_index)), "GMAC Descriptor")
            Database.setSymbolValue("core", ("MPU_Region_" + str(noCache_MPU_index) + "_Address"), Database.getSymbolValue(gmacComponentId, "DRV_"+ gmacComponentName + "_NOCACHE_MEM_ADDRESS"))
            index = drvGmacNoCacheMemRegSize.getValue()
            keyval = drvGmacNoCacheMemRegSize.getKeyValue(index)
            key = drvGmacNoCacheMemRegSize.getKeyForValue(keyval)
            setVal("tcpipStack", "TCPIP_STACK_NOCACHE_MEM_ADDRESS", Database.getSymbolValue(gmacComponentId, "DRV_"+ gmacComponentName + "_NOCACHE_MEM_ADDRESS"))
            setVal("tcpipStack", "TCPIP_STACK_NOCACHE_SIZE", key)
            setVal("tcpipStack", "TCPIP_STACK_NO_CACHE_CONFIG", True)
            mpuRegSize = coreComponent.getSymbolByID("MPU_Region_" + str(noCache_MPU_index) + "_Size")
            mpuRegSize.setSelectedKey(key)
        else:
            Database.setSymbolValue("core", ("MPU_Region_" + str(noCache_MPU_index) + "_Enable"), False)
            setVal("tcpipStack", "TCPIP_STACK_NO_CACHE_CONFIG", False)
    
    

def initNoCacheMPU():
    global noCache_MPU_index
    global gmacComponentId
    global gmacComponentName
    if (noCache_MPU_index == 0xff): 
        # Enable MPU Setting for GMAC descriptor
        if(Database.getSymbolValue("core", "CoreUseMPU") != True):
            Database.setSymbolValue("core", "CoreUseMPU", True)
        if(Database.getSymbolValue("core", "CoreMPU_DEFAULT") != True):  
            Database.setSymbolValue("core", "CoreMPU_DEFAULT", True)            
        mpuNumRegions = Database.getSymbolValue("core", "MPU_NUMBER_REGIONS")
        
        for i in range(0,mpuNumRegions):
            if(Database.getSymbolValue("core", ("MPU_Region_" + str(i) + "_Enable")) != True):
                noCache_MPU_index = i
                Database.setSymbolValue("core", ("MPU_Region_" + str(noCache_MPU_index) + "_Enable"), True)
                Database.setSymbolValue("core", ("MPU_Region_Name" + str(noCache_MPU_index)), "GMAC Descriptor")
                Database.setSymbolValue("core", ("MPU_Region_" + str(noCache_MPU_index) + "_Type"), 5)
                Database.setSymbolValue("core", ("MPU_Region_" + str(noCache_MPU_index) + "_Size"), 7)
                Database.setSymbolValue("core", ("MPU_Region_" + str(noCache_MPU_index) + "_Access"), 3)
                Database.setSymbolValue("core", ("MPU_Region_" + str(noCache_MPU_index) + "_Address"), Database.getSymbolValue(gmacComponentId, "DRV_"+ gmacComponentName + "_NOCACHE_MEM_ADDRESS"))
                break
                


def gmacHeapCalc():
    global gmacComponentId
    global gmacComponentName
    heapsize = 0
    # Rx Section Heap Calculation
    #Queue 0
    if((Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_QUEUE_0") == True)): # and (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_EN_QUE0") == True)):
        heapsize = heapsize + (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_QUE0") + Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_ADDL_BUFF_COUNT_QUE0") + Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_BUFF_COUNT_THRESHOLD_QUE0")) * (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_QUE0") + 128 + 16)
    else:
        heapsize = heapsize + (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_DUMMY") * (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_DUMMY") + 128 + 16))
    
    #Queue 1    
    if((Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_QUEUE_1") == True)): # and (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_EN_QUE1") == True)):
        heapsize = heapsize + (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_QUE1") + Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_ADDL_BUFF_COUNT_QUE1") + Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_BUFF_COUNT_THRESHOLD_QUE1")) * (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_QUE1") + 128 + 16)
    else:
        heapsize = heapsize + (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_DUMMY") * (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_DUMMY") + 128 + 16))
    
    #Queue 2    
    if((Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_QUEUE_2") == True)): #  and (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_EN_QUE2") == True)):
        heapsize = heapsize + (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_QUE2") + Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_ADDL_BUFF_COUNT_QUE2") + Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_BUFF_COUNT_THRESHOLD_QUE2")) * (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_QUE2") + 128 + 16)
    else:
        heapsize = heapsize + (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_DUMMY") * (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_DUMMY") + 128 + 16))   

    #Queue 3    
    if((Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_QUEUE_3") == True)): #  and (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_EN_QUE3") == True)):
        heapsize = heapsize + (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_QUE3") + Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_ADDL_BUFF_COUNT_QUE3") + Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_BUFF_COUNT_THRESHOLD_QUE3")) * (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_QUE3") + 128 + 16)
    else:
        heapsize = heapsize + (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_DUMMY") * (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_DUMMY") + 128 + 16)) 

    #Queue 4    
    if((Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_QUEUE_4") == True)): #  and (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_EN_QUE4") == True)):
        heapsize = heapsize + (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_QUE4") + Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_ADDL_BUFF_COUNT_QUE4") + Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_BUFF_COUNT_THRESHOLD_QUE4")) * (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_QUE4") + 128 + 16)
    else:
        heapsize = heapsize + (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_DUMMY") * (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_DUMMY") + 128 + 16))  
    
    #Queue 5    
    if((Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_QUEUE_5") == True)): #  and (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_EN_QUE5") == True)):
        heapsize = heapsize + (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_QUE5") + Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_ADDL_BUFF_COUNT_QUE5") + Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_BUFF_COUNT_THRESHOLD_QUE5")) * (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_QUE5") + 128 + 16)
    else:
        heapsize = heapsize + (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_DESCRIPTORS_COUNT_DUMMY") * (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_RX_BUFF_SIZE_DUMMY") + 128 + 16))  
        
    # Tx Section Heap Calculation
    #Queue 0
    if((Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_QUEUE_0") == True)): #  and (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_TX_EN_QUE0") == True)):
        heapsize = heapsize + (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_QUE0") * 16)
    else:
        heapsize = heapsize + (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_DUMMY") * 16)
 
    #Queue 1
    if((Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_QUEUE_1") == True)): #  and (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_TX_EN_QUE1") == True)):
        heapsize = heapsize + (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_QUE1") * 16)
    else:
        heapsize = heapsize + (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_DUMMY") * 16)

    #Queue 2
    if((Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_QUEUE_2") == True)): #  and (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_TX_EN_QUE2") == True)):
        heapsize = heapsize + (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_QUE2") * 16)
    else:
        heapsize = heapsize + (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_DUMMY") * 16)

    #Queue 3
    if((Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_QUEUE_3") == True)): #  and (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_TX_EN_QUE3") == True)):
        heapsize = heapsize + (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_QUE3") * 16)
    else:
        heapsize = heapsize + (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_DUMMY") * 16)

    #Queue 4
    if((Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_QUEUE_4") == True)): #  and (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_TX_EN_QUE4") == True)):
        heapsize = heapsize + (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_QUE4") * 16)
    else:
        heapsize = heapsize + (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_DUMMY") * 16)

    #Queue 5
    if((Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_QUEUE_5") == True)): #  and (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_TX_EN_QUE5") == True)):
        heapsize = heapsize + (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_QUE5") * 16)
    else:
        heapsize = heapsize + (Database.getSymbolValue(gmacComponentId,"TCPIP_"+ gmacComponentName + "_TX_DESCRIPTORS_COUNT_DUMMY") * 16)        
    
    return heapsize
 
def drvGmacHeapUpdate(symbol, event): 
    heap_size = gmacHeapCalc()
    symbol.setValue(heap_size)
    if(event["id"] == "TCPIP_STACK_HEAP_CALC_MASK"):
        symbol.setVisible(event["value"])

def drvGmacIntSrcEnable(symbol, event):
    global gmacComponentName
    global processor 
    
    que_index = int(event["id"].strip("TCPIP_"+ gmacComponentName + "_QUEUE_"))
    
    if "PIC32CZ" in processor :#PIC32CZ
        int_name = "ETH_PRI_Q_" + str(que_index)
    else:
        int_name = gmacComponentName + "_Q" + str(que_index)
        
    interruptVector = int_name + "_INTERRUPT_ENABLE"
    interruptHandlerLock = int_name + "_INTERRUPT_HANDLER_LOCK"
    
    if (event["value"] == True):    
        Database.setSymbolValue("core", interruptVector, True, 2)    
        Database.setSymbolValue("core", interruptHandlerLock, True, 2)    
    else: 
        Database.clearSymbolValue("core", interruptVector)
        Database.clearSymbolValue("core", interruptHandlerLock)

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

def destroyComponent(drvGmacComponent):
    global gmac_periphID
    global noCache_MPU_index
    global gmacComponentId
    global gmacComponentName
    
    Database.setSymbolValue(gmacComponentId, "TCPIP_USE_ETH_MAC", False, 2)    
    if "PIC32CZ" in processor:
        int_name = "ETH_PRI_Q_0"
    else:
        int_name = gmacComponentName
        
    interruptVector = int_name + "_INTERRUPT_ENABLE"    
    setVal("core", interruptVector, False)
    if("SAMV7" in processor) or ("SAME7" in processor) or ("SAMRH" in processor) or ("SAMA5" in processor) or ("PIC32CZ" in processor) or ("SAMA7G" in processor): # SAME70 or SAMV71 or SAMA5D2 or PIC32CZ or SAMA7G
        if "PIC32CZ" in processor :#PIC32CZ
            int_name = "ETH_PRI_Q_1"
        else:
            int_name = gmacComponentName + "_Q1"
        interruptVector = int_name + "_INTERRUPT_ENABLE"      
        setVal("core", interruptVector, False)
    if("SAMV7" in processor) or ("SAME7" in processor) or ("SAMRH" in processor) or ("SAMA5" in processor) or ("PIC32CZ" in processor) or (("SAMA7G" in processor) and (gmacComponentName == "GMAC0")): # SAME70 or SAMV71 or SAMA5D2 or PIC32CZ or SAMA7G-GMAC0
        if "PIC32CZ" in processor :#PIC32CZ
            int_name = "ETH_PRI_Q_2"
        else:
            int_name = gmacComponentName + "_Q2"
        interruptVector = int_name + "_INTERRUPT_ENABLE"      
        setVal("core", interruptVector, False)
    if("SAMV7" in processor) or ("SAME7" in processor) or ("SAMRH" in processor) or ("PIC32CZ" in processor): # SAME70, SAMV71, SAMRH71, PIC32CZ   
        if "PIC32CZ" in processor :#PIC32CZ
            int_name = "ETH_PRI_Q_3"
        else:
            int_name = gmacComponentName + "_Q3"
        interruptVector = int_name + "_INTERRUPT_ENABLE"      
        setVal("core", interruptVector, False)
        if "PIC32CZ" in processor :#PIC32CZ
            int_name = "ETH_PRI_Q_4"
        else:
            int_name = gmacComponentName + "_Q4"
        interruptVector = int_name + "_INTERRUPT_ENABLE"      
        setVal("core", interruptVector, False)
        if "PIC32CZ" in processor :#PIC32CZ
            int_name = "ETH_PRI_Q_5"
        else:
            int_name = gmacComponentName + "_Q5"
        interruptVector = int_name + "_INTERRUPT_ENABLE"      
        setVal("core", interruptVector, False)
        setVal("core", ("MPU_Region_" + str(noCache_MPU_index) + "_Enable"), False)
        