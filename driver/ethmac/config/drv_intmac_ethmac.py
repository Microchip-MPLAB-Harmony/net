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


__TCPIP_DIR = "D:/06_Repo/05_H3/h3/net"
def instantiateComponent(drvPic32mEthmacComponent):
    global tcpipEthmacInterruptVector
    global tcpipEthmacInterruptHandlerLock
    global tcpipEthmacInterruptHandler
    global tcpipEthmacInterruptVectorUpdate
    global tcpipEthmacInterruptEnable
    
    print("PIC32M Internal Ethernet MAC Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    
    #Clock enable
    Database.setSymbolValue("core", "ETH" + "_CLOCK_ENABLE", True, 1)
    
    # Use Internal Ethernet MAC Driver? 
    drvEthmac = drvPic32mEthmacComponent.createBooleanSymbol("TCPIP_USE_ETH_MAC", None)
    drvEthmac.setLabel("Use Internal Ethernet MAC Driver?")
    drvEthmac.setVisible(False)
    drvEthmac.setDescription("Use Internal Ethernet MAC Driver?")
    drvEthmac.setDefaultValue(True) 
    if "PIC32MZ" in Variables.get("__PROCESSOR"):
        # Enable RMII mode
        Database.setSymbolValue("core", "CONFIG_FMIIEN", "OFF", 1)
        # Enable Alternate Pins for Ethernet MAC
        Database.setSymbolValue("core", "CONFIG_FETHIO", "ON", 1)
    elif "PIC32MX" in Variables.get("__PROCESSOR"):
        # Enable RMII mode
        Database.setSymbolValue("core", "CONFIG_FMIIEN", "OFF", 1)
        # Enable Default Pins for Ethernet MAC
        Database.setSymbolValue("core", "CONFIG_FETHIO", "OFF", 1)

    # Number of Tx Descriptors to be created
    tcpipEthmacTxDescCount = drvPic32mEthmacComponent.createIntegerSymbol("TCPIP_EMAC_TX_DESCRIPTORS", None)
    tcpipEthmacTxDescCount.setLabel("Number of Tx Descriptors to be created")
    tcpipEthmacTxDescCount.setVisible(True)
    tcpipEthmacTxDescCount.setDescription("Number of Tx Descriptors to be created")
    tcpipEthmacTxDescCount.setDefaultValue(8)
    # tcpipEthmacTxDescCount.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_0"])

    # Number of Rx Descriptors to be created
    tcpipEthmacRxDescCount = drvPic32mEthmacComponent.createIntegerSymbol("TCPIP_EMAC_RX_DESCRIPTORS", None)
    tcpipEthmacRxDescCount.setLabel("Number of Rx Descriptors to be created")
    tcpipEthmacRxDescCount.setVisible(True)
    tcpipEthmacRxDescCount.setDescription("Number of Rx Descriptors to be created")
    tcpipEthmacRxDescCount.setDefaultValue(6)
    # tcpipEthmacRxDescCount.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_0"])

    # Number of Dedicated MAC RX Buffers
    tcpipEthmacRxOwnBuffer = drvPic32mEthmacComponent.createIntegerSymbol("TCPIP_EMAC_RX_DEDICATED_BUFFERS", None)
    tcpipEthmacRxOwnBuffer.setLabel("Number of Dedicated MAC RX Buffers")
    tcpipEthmacRxOwnBuffer.setVisible(True)
    tcpipEthmacRxOwnBuffer.setDescription("Number of Dedicated MAC RX Buffers")
    tcpipEthmacRxOwnBuffer.setDefaultValue(4)

    # Number of Non_Dedicated RX Buffers for MAC Initialization
    tcpipEthmacRxDynBuffer = drvPic32mEthmacComponent.createIntegerSymbol("TCPIP_EMAC_RX_INIT_BUFFERS", None)
    tcpipEthmacRxDynBuffer.setLabel("Number of Non_Dedicated RX Buffers")
    tcpipEthmacRxDynBuffer.setVisible(True)
    tcpipEthmacRxDynBuffer.setDescription("Number of Non_Dedicated RX Buffers for MAC Initialization")
    tcpipEthmacRxDynBuffer.setDefaultValue(0)

    # Minimum Threshold for the Buffer Replenish Process
    tcpipEthmacRxBufferThres = drvPic32mEthmacComponent.createIntegerSymbol("TCPIP_EMAC_RX_LOW_THRESHOLD", None)
    tcpipEthmacRxBufferThres.setLabel("Minimum Threshold for the Buffer Replenish Process")
    tcpipEthmacRxBufferThres.setVisible(True)
    tcpipEthmacRxBufferThres.setDescription("Minimum Threshold for the Buffer Replenish Process")
    tcpipEthmacRxBufferThres.setDefaultValue(1)

    # Number of RX Buffers to Allocate when Below Threshold Condition is Detected
    tcpipEthmacRxBufferReFill = drvPic32mEthmacComponent.createIntegerSymbol("TCPIP_EMAC_RX_LOW_FILL", None)
    tcpipEthmacRxBufferReFill.setLabel("Number of RX Buffers to Allocate when Below Threshold Condition is Detected")
    tcpipEthmacRxBufferReFill.setVisible(True)
    tcpipEthmacRxBufferReFill.setDescription("Number of RX Buffers to Allocate when Below Threshold Condition is Detected")
    tcpipEthmacRxBufferReFill.setDefaultValue(2)

    # Maximum Frame Size - Longer Frames Will Be Discarded
    tcpipEthmacMaxFrame = drvPic32mEthmacComponent.createIntegerSymbol("TCPIP_EMAC_MAX_FRAME", None)
    tcpipEthmacMaxFrame.setLabel("Maximum Frame Size - Longer Frames Will Be Discarded")
    tcpipEthmacMaxFrame.setVisible(True)
    tcpipEthmacMaxFrame.setDescription("Maximum Frame Size - Longer Frames Will Be Discarded")
    tcpipEthmacMaxFrame.setDefaultValue(1536)

    # Link Maximum Transmission Unit - (576 - 1500)
    tcpipEthmacLinkMTU = drvPic32mEthmacComponent.createIntegerSymbol("TCPIP_EMAC_LINK_MTU", None)
    tcpipEthmacLinkMTU.setLabel("Link Maximum Transmission Unit - (576 - 1500)")
    tcpipEthmacLinkMTU.setMin(576)
    tcpipEthmacLinkMTU.setMax(1500)
    tcpipEthmacLinkMTU.setVisible(True)
    tcpipEthmacLinkMTU.setDescription("Link Maximum Transmission Unit - (576 - 1500)")
    tcpipEthmacLinkMTU.setDefaultValue(1500)

    # Size of a RX Buffer. Should Be Multiple of 16
    tcpipEthmacRxBuffSize = drvPic32mEthmacComponent.createIntegerSymbol("TCPIP_EMAC_RX_BUFF_SIZE", None)
    tcpipEthmacRxBuffSize.setLabel("Size of a RX Buffer. Should Be Multiple of 16")
    tcpipEthmacRxBuffSize.setVisible(True)
    tcpipEthmacRxBuffSize.setDescription("Size of a RX Buffer. Should Be Multiple of 16")
    tcpipEthmacRxBuffSize.setDefaultValue(1536)

    # MAC Maximum Number of Supported RX Fragments
    tcpipEthmacRxFragMaxNum = drvPic32mEthmacComponent.createIntegerSymbol("TCPIP_EMAC_RX_FRAGMENTS", None)
    tcpipEthmacRxFragMaxNum.setLabel("MAC Maximum Number of Supported RX Fragments")
    tcpipEthmacRxFragMaxNum.setMin(1)
    tcpipEthmacRxFragMaxNum.setMax(6)
    tcpipEthmacRxFragMaxNum.setVisible(True)
    tcpipEthmacRxFragMaxNum.setDescription("MAC Maximum Number of Supported RX Fragments")
    tcpipEthmacRxFragMaxNum.setDefaultValue(1)
    
    # Ethernet RX Filters Selection Settings
    tcpipEthRxFilter = drvPic32mEthmacComponent.createMenuSymbol(None, None) 
    tcpipEthRxFilter.setLabel("Ethernet RX Filters Selection")
    tcpipEthRxFilter.setVisible(True)
    tcpipEthRxFilter.setDescription("Ethernet RX Filters Selection Settings")

    # Accept Broadcast Packets
    tcpipEthmacEthFilterBcastAccept = drvPic32mEthmacComponent.createBooleanSymbol("TCPIP_EMAC_ETH_FILTER_BCAST_ACCEPT", tcpipEthRxFilter)
    tcpipEthmacEthFilterBcastAccept.setLabel("Accept Broadcast Packets")
    tcpipEthmacEthFilterBcastAccept.setVisible(True)
    tcpipEthmacEthFilterBcastAccept.setDescription("Accept Broadcast Packets")
    tcpipEthmacEthFilterBcastAccept.setDefaultValue(True)
    
    # Accept Multicast Packets
    tcpipEthmacEthFilterMcastAccept = drvPic32mEthmacComponent.createBooleanSymbol("TCPIP_EMAC_ETH_FILTER_MCAST_ACCEPT", tcpipEthRxFilter)
    tcpipEthmacEthFilterMcastAccept.setLabel("Accept Multicast Packets")
    tcpipEthmacEthFilterMcastAccept.setVisible(True)
    tcpipEthmacEthFilterMcastAccept.setDescription("Accept Multicast Packets")
    tcpipEthmacEthFilterMcastAccept.setDefaultValue(True)
    
    # Accept Unicast Packets
    tcpipEthmacEthFilterUcastAccept = drvPic32mEthmacComponent.createBooleanSymbol("TCPIP_EMAC_ETH_FILTER_UCAST_ACCEPT", tcpipEthRxFilter)
    tcpipEthmacEthFilterUcastAccept.setLabel("Accept Unicast Packets")
    tcpipEthmacEthFilterUcastAccept.setVisible(True)
    tcpipEthmacEthFilterUcastAccept.setDescription("Accept Unicast Packets")
    tcpipEthmacEthFilterUcastAccept.setDefaultValue(True)

    # Accept Not Me Unicast Packets
    tcpipEthmacEthFilterNotmeUcastAccept = drvPic32mEthmacComponent.createBooleanSymbol("TCPIP_EMAC_ETH_FILTER_UCAST_OTHER_ACCEPT", tcpipEthRxFilter)
    tcpipEthmacEthFilterNotmeUcastAccept.setLabel("Accept Not Me Unicast Packets")
    tcpipEthmacEthFilterNotmeUcastAccept.setVisible(True)
    tcpipEthmacEthFilterNotmeUcastAccept.setDescription("Accept Not Me Unicast Packets")
    tcpipEthmacEthFilterNotmeUcastAccept.setDefaultValue(False)

    # Reject Runt Packets (< 64 bytes)
    tcpipEthmacEthFilterUcastHashReject = drvPic32mEthmacComponent.createBooleanSymbol("TCPIP_EMAC_ETH_FILTER_RUNT_REJECT", tcpipEthRxFilter)
    tcpipEthmacEthFilterUcastHashReject.setLabel("Reject Runt Packets (< 64 bytes)")
    tcpipEthmacEthFilterUcastHashReject.setVisible(True)
    tcpipEthmacEthFilterUcastHashReject.setDescription("Reject Runt Packets (< 64 bytes)")
    tcpipEthmacEthFilterUcastHashReject.setDefaultValue(True)
 
    # Accept Runt Packets (< 64 bytes)
    tcpipEthmacEthFilterRuntAccept = drvPic32mEthmacComponent.createBooleanSymbol("TCPIP_EMAC_ETH_FILTER_RUNT_ACCEPT", tcpipEthRxFilter)
    tcpipEthmacEthFilterRuntAccept.setLabel("Accept Runt Packets (< 64 bytes)")
    tcpipEthmacEthFilterRuntAccept.setVisible(True)
    tcpipEthmacEthFilterRuntAccept.setDescription("Accept Runt Packets (< 64 bytes)")
    tcpipEthmacEthFilterRuntAccept.setDefaultValue(False)
    
    # Reject Packets with Wrong CRC
    tcpipEthmacEthFilterCrcErrReject = drvPic32mEthmacComponent.createBooleanSymbol("TCPIP_EMAC_ETH_FILTER_CRC_ERROR_REJECT", tcpipEthRxFilter)
    tcpipEthmacEthFilterCrcErrReject.setLabel("Reject Packets with Wrong CRC")
    tcpipEthmacEthFilterCrcErrReject.setVisible(True)
    tcpipEthmacEthFilterCrcErrReject.setDescription("Reject Packets with Wrong CRC")
    tcpipEthmacEthFilterCrcErrReject.setDefaultValue(True)

    # Accept Packets with Wrong CRC
    tcpipEthmacEthFilterCrcErrAccept = drvPic32mEthmacComponent.createBooleanSymbol("TCPIP_EMAC_ETH_FILTER_CRC_ERROR_ACCEPT", tcpipEthRxFilter)
    tcpipEthmacEthFilterCrcErrAccept.setLabel("Accept Packets with Wrong CRC")
    tcpipEthmacEthFilterCrcErrAccept.setVisible(True)
    tcpipEthmacEthFilterCrcErrAccept.setDescription("Accept Packets with Wrong CRC")
    tcpipEthmacEthFilterCrcErrAccept.setDefaultValue(False)
    
    # Ethernet Auto Flow Control
    tcpipEthAutoFlowCtrl = drvPic32mEthmacComponent.createMenuSymbol(None, None) 
    tcpipEthAutoFlowCtrl.setLabel("Ethernet Auto Flow Control")
    tcpipEthAutoFlowCtrl.setVisible(True)
    tcpipEthAutoFlowCtrl.setDescription("Ethernet Auto Flow Control")

    # Auto Flow Control Enable
    tcpipEthAutoFlowCtrlEnable = drvPic32mEthmacComponent.createBooleanSymbol("TCPIP_EMAC_AUTO_FLOW_CONTROL_ENABLE", tcpipEthAutoFlowCtrl)
    tcpipEthAutoFlowCtrlEnable.setLabel("Auto Flow Control Enable")
    tcpipEthAutoFlowCtrlEnable.setVisible(True)
    tcpipEthAutoFlowCtrlEnable.setDescription("Auto Flow Control Enable")
    tcpipEthAutoFlowCtrlEnable.setDefaultValue(True)

    # Pause Time Value for the Auto Flow Control, bytes
    tcpipEthmacAutoFlowCtrlPauseTime = drvPic32mEthmacComponent.createIntegerSymbol("TCPIP_EMAC_FLOW_CONTROL_PAUSE_BYTES", tcpipEthAutoFlowCtrlEnable)
    tcpipEthmacAutoFlowCtrlPauseTime.setLabel("Pause Time Value for the Auto Flow Control, bytes")
    tcpipEthmacAutoFlowCtrlPauseTime.setMin(64)
    tcpipEthmacAutoFlowCtrlPauseTime.setMax(4194240)
    tcpipEthmacAutoFlowCtrlPauseTime.setVisible(True)
    tcpipEthmacAutoFlowCtrlPauseTime.setDescription("Pause Time Value for the Auto Flow Control, bytes")
    tcpipEthmacAutoFlowCtrlPauseTime.setDefaultValue(3072)
    tcpipEthmacAutoFlowCtrlPauseTime.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_EMAC_AUTO_FLOW_CONTROL_ENABLE"])

    # The Full Water Mark, in Number of Packets
    tcpipEthmacAutoFlowCtrlFullWtrMark = drvPic32mEthmacComponent.createIntegerSymbol("TCPIP_EMAC_FLOW_CONTROL_FULL_WMARK", tcpipEthAutoFlowCtrlEnable)
    tcpipEthmacAutoFlowCtrlFullWtrMark.setLabel("The Full Water Mark, in Number of Packets")
    tcpipEthmacAutoFlowCtrlFullWtrMark.setMin(1)
    tcpipEthmacAutoFlowCtrlFullWtrMark.setMax(255)
    tcpipEthmacAutoFlowCtrlFullWtrMark.setVisible(True)
    tcpipEthmacAutoFlowCtrlFullWtrMark.setDescription("The Full Water Mark, in Number of Packets")
    tcpipEthmacAutoFlowCtrlFullWtrMark.setDefaultValue(2)
    tcpipEthmacAutoFlowCtrlFullWtrMark.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_EMAC_AUTO_FLOW_CONTROL_ENABLE"])

    # The Empty Water Mark, in Number of Packets
    tcpipEthmacAutoFlowCtrlEmptyWtrMark = drvPic32mEthmacComponent.createIntegerSymbol("TCPIP_EMAC_FLOW_CONTROL_EMPTY_WMARK", tcpipEthAutoFlowCtrlEnable)
    tcpipEthmacAutoFlowCtrlEmptyWtrMark.setLabel("The Empty Water Mark, in Number of Packets")
    tcpipEthmacAutoFlowCtrlEmptyWtrMark.setMin(0)
    tcpipEthmacAutoFlowCtrlEmptyWtrMark.setMax(255)
    tcpipEthmacAutoFlowCtrlEmptyWtrMark.setVisible(True)
    tcpipEthmacAutoFlowCtrlEmptyWtrMark.setDescription("The Empty Water Mark, in Number of Packets")
    tcpipEthmacAutoFlowCtrlEmptyWtrMark.setDefaultValue(0)
    tcpipEthmacAutoFlowCtrlEmptyWtrMark.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_EMAC_AUTO_FLOW_CONTROL_ENABLE"])
 
    # Ethernet Connection Flags
    tcpipEthConnFlag = drvPic32mEthmacComponent.createMenuSymbol(None, None) 
    tcpipEthConnFlag.setLabel("Ethernet Connection Flags")
    tcpipEthConnFlag.setVisible(True)
    tcpipEthConnFlag.setDescription("Ethernet Connection Flags")
    
    # Use Auto Negotiation
    tcpipEthmacEthAutoNegotiate = drvPic32mEthmacComponent.createBooleanSymbol("TCPIP_EMAC_ETH_OF_AUTO_NEGOTIATION", tcpipEthConnFlag)
    tcpipEthmacEthAutoNegotiate.setLabel("Use Auto Negotiation")
    tcpipEthmacEthAutoNegotiate.setVisible(True) 
    tcpipEthmacEthAutoNegotiate.setDescription("Use Auto Negotiation")
    tcpipEthmacEthAutoNegotiate.setDefaultValue(True)
    
    # Use Full Duplex
    tcpipEthmacEthFullDuplex = drvPic32mEthmacComponent.createBooleanSymbol("TCPIP_EMAC_ETH_OF_FULL_DUPLEX", tcpipEthConnFlag)
    tcpipEthmacEthFullDuplex.setLabel("Use Full Duplex")
    tcpipEthmacEthFullDuplex.setVisible(True)
    tcpipEthmacEthFullDuplex.setDescription("Use Full Duplex")
    tcpipEthmacEthFullDuplex.setDefaultValue(True)
    
    # Use Half Duplex
    tcpipEthmacEthHalfDuplex = drvPic32mEthmacComponent.createBooleanSymbol("TCPIP_EMAC_ETH_OF_HALF_DUPLEX", tcpipEthConnFlag)
    tcpipEthmacEthHalfDuplex.setLabel("Use Half Duplex")
    tcpipEthmacEthHalfDuplex.setVisible(True)
    tcpipEthmacEthHalfDuplex.setDescription("Use Half Duplex")
    tcpipEthmacEthHalfDuplex.setDefaultValue(True)
    
    # Use 100MBps
    tcpipEthmacEthUse100 = drvPic32mEthmacComponent.createBooleanSymbol("TCPIP_EMAC_ETH_OF_100", tcpipEthConnFlag)
    tcpipEthmacEthUse100.setLabel("Use 100MBps")
    tcpipEthmacEthUse100.setVisible(True) 
    tcpipEthmacEthUse100.setDescription("Use 100MBps")
    tcpipEthmacEthUse100.setDefaultValue(True)
    
    # Use 10MBps
    tcpipEthmacEthUse10 = drvPic32mEthmacComponent.createBooleanSymbol("TCPIP_EMAC_ETH_OF_10", tcpipEthConnFlag)
    tcpipEthmacEthUse10.setLabel("Use 10MBps")
    tcpipEthmacEthUse10.setVisible(True)
    tcpipEthmacEthUse10.setDescription("Use 10MBps")
    tcpipEthmacEthUse10.setDefaultValue(True)
    
    # Allow Huge Packets
    tcpipEthmacEthHugePkt = drvPic32mEthmacComponent.createBooleanSymbol("TCPIP_EMAC_ETH_OF_HUGE_PKTS", tcpipEthConnFlag)
    tcpipEthmacEthHugePkt.setLabel("Allow Huge Packets")
    tcpipEthmacEthHugePkt.setVisible(True)
    tcpipEthmacEthHugePkt.setDescription("Allow Huge Packets")
    tcpipEthmacEthHugePkt.setDefaultValue(False)
    
    # Loopbacked At The MAC Level
    tcpipEthmacEthMacLoopBack = drvPic32mEthmacComponent.createBooleanSymbol("TCPIP_EMAC_ETH_OF_MAC_LOOPBACK", tcpipEthConnFlag)
    tcpipEthmacEthMacLoopBack.setLabel("Loopbacked At The MAC Level")
    tcpipEthmacEthMacLoopBack.setVisible(True) 
    tcpipEthmacEthMacLoopBack.setDescription("Loopbacked At The MAC Level")
    tcpipEthmacEthMacLoopBack.setDefaultValue(False)
    
    # Loopbacked At The PHY Level
    tcpipEthmacEthPhyLoopBack = drvPic32mEthmacComponent.createBooleanSymbol("TCPIP_EMAC_ETH_OF_PHY_LOOPBACK", tcpipEthConnFlag)
    tcpipEthmacEthPhyLoopBack.setLabel("Loopbacked At The PHY Level")
    tcpipEthmacEthPhyLoopBack.setVisible(True)
    tcpipEthmacEthPhyLoopBack.setDescription("Loopbacked At The PHY Level")
    tcpipEthmacEthPhyLoopBack.setDefaultValue(False)
    
    # Use Auto MDIX
    tcpipEthmacEthMdixAuto = drvPic32mEthmacComponent.createBooleanSymbol("TCPIP_EMAC_ETH_OF_MDIX_AUTO", tcpipEthConnFlag)
    tcpipEthmacEthMdixAuto.setLabel("Use Auto MDIX")
    tcpipEthmacEthMdixAuto.setVisible(True) 
    tcpipEthmacEthMdixAuto.setDescription("Use Auto MDIX")
    tcpipEthmacEthMdixAuto.setDefaultValue(True)
    
    # Use Swapped MDIX
    tcpipEthmacEthMdixSwap = drvPic32mEthmacComponent.createBooleanSymbol("TCPIP_EMAC_ETH_OF_MDIX_SWAP", tcpipEthConnFlag)
    tcpipEthmacEthMdixSwap.setLabel("Use Swapped MDIX")
    tcpipEthmacEthMdixSwap.setVisible(False)
    tcpipEthmacEthMdixSwap.setDescription("Use Swapped MDIX")
    tcpipEthmacEthMdixSwap.setDefaultValue(False)
    tcpipEthmacEthMdixSwap.setDependencies(tcpipEthMacMdixSwapVisible, ["TCPIP_EMAC_ETH_OF_MDIX_AUTO"])

    # RMII Connection
    tcpipEthmacEthRmii = drvPic32mEthmacComponent.createBooleanSymbol("TCPIP_EMAC_ETH_OF_RMII", tcpipEthConnFlag)
    tcpipEthmacEthRmii.setLabel("RMII Connection")
    tcpipEthmacEthRmii.setVisible(True)
    tcpipEthmacEthRmii.setDescription("RMII Connection")
    tcpipEthmacEthRmii.setDefaultValue(True)
    # todo default y if FMIIEN = "OFF" 
    
    # EMAC Module ID
    tcpipEmacModuleId = drvPic32mEthmacComponent.createStringSymbol("TCPIP_EMAC_MODULE_ID", None)
    tcpipEmacModuleId.setLabel("EMAC Module ID")
    tcpipEmacModuleId.setVisible(True)
    tcpipEmacModuleId.setDescription("EMAC Module ID")
    tcpipEmacModuleId.setDefaultValue("_ETH_BASE_ADDRESS")
    
    # Get register names, mask values, bit shifts based on vector number
    if "PIC32MZ" in Variables.get("__PROCESSOR"):
        tcpipEthmacInterruptVector = "ETHERNET_INTERRUPT_ENABLE"
        tcpipEthmacInterruptHandler = "ETHERNET_INTERRUPT_HANDLER"
        tcpipEthmacInterruptHandlerLock = "ETHERNET_INTERRUPT_HANDLER_LOCK"
        tcpipEthmacInterruptVectorUpdate = "ETHERNET_INTERRUPT_ENABLE_UPDATE"
        tcpipEthmacIrq_index = int(getIRQnumber("ETHERNET"))
    elif "PIC32MX" in Variables.get("__PROCESSOR"):
        tcpipEthmacInterruptVector = "ETH_INTERRUPT_ENABLE"
        tcpipEthmacInterruptHandler = "ETH_INTERRUPT_HANDLER"
        tcpipEthmacInterruptHandlerLock = "ETH_INTERRUPT_HANDLER_LOCK"
        tcpipEthmacInterruptVectorUpdate = "ETH_INTERRUPT_ENABLE_UPDATE"
        tcpipEthmacIrq_index = int(getIRQnumber("ETH"))
    
    
    #Configures the library for interrupt mode operations
    tcpipEthmacInterruptEnable = drvPic32mEthmacComponent.createBooleanSymbol("INTERRUPT_ENABLE", None)
    tcpipEthmacInterruptEnable.setLabel("Enable Interrupt?")
    tcpipEthmacInterruptEnable.setDefaultValue(True) 
    
    tcpipEthmacInterruptComment = drvPic32mEthmacComponent.createCommentSymbol("ETHERNET_INTRRUPT_ENABLE_COMMENT", None)
    tcpipEthmacInterruptComment.setLabel("Warning!!! Ethernet MAC Interrupt is Disabled in Interrupt Manager")
    tcpipEthmacInterruptComment.setVisible(False)
    tcpipEthmacInterruptComment.setDependencies(updateEthmacInterruptData, ["INTERRUPT_ENABLE", "core." + tcpipEthmacInterruptVectorUpdate])

    setEthmacInterruptData(True)    
    
    enblRegName, enblBitPosn = _get_enblReg_parms(tcpipEthmacIrq_index)
    statRegName, statBitPosn = _get_statReg_parms(tcpipEthmacIrq_index)  

    # Below create family-specific register names / masking needed by ftl file
    tcpipEthmacEnblRegWrt = drvPic32mEthmacComponent.createStringSymbol("ETHERNET_IEC_REG", None)
    tcpipEthmacEnblRegWrt.setDefaultValue(enblRegName)
    tcpipEthmacEnblRegWrt.setVisible(False)

    tcpipEthmacEnblRegVal = drvPic32mEthmacComponent.createStringSymbol("ETHERNET_IEC_REG_VALUE", None)
    tcpipEthmacEnblRegVal.setDefaultValue(str(hex(1<<int(enblBitPosn))))
    tcpipEthmacEnblRegVal.setVisible(False)

    tcpipEthmacStatRegRd = drvPic32mEthmacComponent.createStringSymbol("ETHERNET_IFS_REG", None)
    tcpipEthmacStatRegRd.setDefaultValue(statRegName)
    tcpipEthmacStatRegRd.setVisible(False)

    tcpipEthmacStatRegShiftVal = drvPic32mEthmacComponent.createStringSymbol("ETHERNET_IFS_REG_VALUE", None)
    tcpipEthmacStatRegShiftVal.setDefaultValue(str(hex(1<<int(statBitPosn))))
    tcpipEthmacStatRegShiftVal.setVisible(False)

    tcpipEthmacVectorNum = drvPic32mEthmacComponent.createIntegerSymbol("ETHERNET_VECTOR_NUMBER", None)
    tcpipEthmacVectorNum.setDefaultValue(tcpipEthmacIrq_index)
    tcpipEthmacVectorNum.setVisible(False)
    
    # ETH MAC Instances Number
    tcpipEthmacInstanceNum = drvPic32mEthmacComponent.createIntegerSymbol("DRV_ETHMAC_INSTANCES_NUMBER", None)
    tcpipEthmacInstanceNum.setLabel("ETH MAC Instances Number")
    tcpipEthmacInstanceNum.setVisible(True)
    tcpipEthmacInstanceNum.setDescription("ETH MAC Instances Number")
    tcpipEthmacInstanceNum.setDefaultValue(1)
    
    # ETH MAC Clients Number
    tcpipEthmacClientNum = drvPic32mEthmacComponent.createIntegerSymbol("DRV_ETHMAC_CLIENTS_NUMBER", None)
    tcpipEthmacClientNum.setLabel("ETH MAC Clients Number")
    tcpipEthmacClientNum.setVisible(True)
    tcpipEthmacClientNum.setDescription("ETH MAC Clients Number")
    tcpipEthmacClientNum.setDefaultValue(1)
    
    # Driver ETH MAC Index Number
    tcpipEthmacIndex = drvPic32mEthmacComponent.createIntegerSymbol("DRV_ETHMAC_INDEX", None)
    tcpipEthmacIndex.setLabel("ETH MAC Index Number") 
    tcpipEthmacIndex.setVisible(True)
    tcpipEthmacIndex.setDescription("ETH MAC Index Number")
    tcpipEthmacIndex.setDefaultValue(1)
    
    # ETH MAC Peripheral ID
    tcpipEthmacPeripheralId = drvPic32mEthmacComponent.createIntegerSymbol("DRV_ETHMAC_PERIPHERAL_ID", None)
    tcpipEthmacPeripheralId.setLabel("ETH MAC Peripheral ID") 
    tcpipEthmacPeripheralId.setVisible(True)
    tcpipEthmacPeripheralId.setDescription("ETH MAC Peripheral ID")
    tcpipEthmacPeripheralId.setDefaultValue(1)
    
    # PHY Connected to Ethernet MAC
    tcpipEthmacPhyType = drvPic32mEthmacComponent.createStringSymbol("DRV_INTMAC_PHY_TYPE", None)
    tcpipEthmacPhyType.setLabel("External PHY Device")
    tcpipEthmacPhyType.setVisible(True)
    tcpipEthmacPhyType.setDescription("PHY Connected to ETHERNET MAC")
    tcpipEthmacPhyType.setDefaultValue("")
    tcpipEthmacPhyType.setReadOnly(True)
    
    # Driver ETHMAC Interrupt Source
    tcpipEthmacIntrSource = drvPic32mEthmacComponent.createStringSymbol("DRV_ETHMAC_INTERRUPT_SOURCE", None)
    tcpipEthmacIntrSource.setLabel("ETHMAC Interrupt Source")
    tcpipEthmacIntrSource.setVisible(False)
    tcpipEthmacIntrSource.setDescription("Driver ETHMAC Interrupt Source")
    if "PIC32MZ" in Variables.get("__PROCESSOR"):
        tcpipEthmacIntrSource.setDefaultValue("_ETHERNET_VECTOR")
    elif "PIC32MX" in Variables.get("__PROCESSOR"):
        tcpipEthmacIntrSource.setDefaultValue("_ETHERNET_IRQ")
    
    
    tcpipEthmacheapdependency = ["TCPIP_EMAC_TX_DESCRIPTORS", "TCPIP_EMAC_RX_DESCRIPTORS", 
                                 "TCPIP_EMAC_RX_BUFF_SIZE", "TCPIP_EMAC_RX_DEDICATED_BUFFERS", 
                                 "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK" ]    
        
    # ETHMAC Heap Size
    tcpipEthmacHeapSize = drvPic32mEthmacComponent.createIntegerSymbol("DRV_ETHMAC_HEAP_SIZE", None)
    tcpipEthmacHeapSize.setLabel("ETHMAC Heap Size (bytes)")     
    tcpipEthmacHeapSize.setVisible(False)
    tcpipEthmacHeapSize.setDefaultValue(tcpipEthmacHeapCalc())
    tcpipEthmacHeapSize.setReadOnly(True)
    tcpipEthmacHeapSize.setDependencies(tcpipEthmacHeapUpdate, tcpipEthmacheapdependency)    
    
    #Add to system_config.h
    tcpipEthmacHeaderFtl = drvPic32mEthmacComponent.createFileSymbol(None, None)
    tcpipEthmacHeaderFtl.setSourcePath("driver/ethmac/config/drv_intmac_ethmac.h.ftl")
    tcpipEthmacHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipEthmacHeaderFtl.setMarkup(True)
    tcpipEthmacHeaderFtl.setType("STRING") 
    
    # file TCPIP_MAC_DRV_H "$HARMONY_VERSION_PATH/framework/driver/ethmac/drv_ethmac.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ethmac/drv_ethmac.h"
    # Add drv_ethmac.h file to project
    tcpipEthmacHeaderFile = drvPic32mEthmacComponent.createFileSymbol(None, None)
    tcpipEthmacHeaderFile.setSourcePath("driver/ethmac/drv_ethmac.h")
    tcpipEthmacHeaderFile.setOutputName("drv_ethmac.h")
    tcpipEthmacHeaderFile.setDestPath("driver/ethmac/")
    tcpipEthmacHeaderFile.setProjectPath("config/" + configName + "/driver/ethmac/")
    tcpipEthmacHeaderFile.setType("HEADER")
    tcpipEthmacHeaderFile.setOverwrite(True)
    
    # file TCPIP_MAC_LOCAL_H "$HARMONY_VERSION_PATH/framework/driver/ethmac/src/drv_ethmac_local.h" to           "$PROJECT_HEADER_FILES/framework/driver/ethmac/src/drv_ethmac_local.h"
    # Add drv_ethmac_local.h file to project
    tcpipEthmacLocalHeaderFile = drvPic32mEthmacComponent.createFileSymbol(None, None)
    tcpipEthmacLocalHeaderFile.setSourcePath("driver/ethmac/src/drv_ethmac_local.h")
    tcpipEthmacLocalHeaderFile.setOutputName("drv_ethmac_local.h")
    tcpipEthmacLocalHeaderFile.setDestPath("driver/ethmac/src/")
    tcpipEthmacLocalHeaderFile.setProjectPath("config/" + configName + "/driver/ethmac/src/")
    tcpipEthmacLocalHeaderFile.setType("HEADER")
    tcpipEthmacLocalHeaderFile.setOverwrite(True)
    
    # file TCPIP_MAC_DESC_H "$HARMONY_VERSION_PATH/framework/driver/ethmac/src/dynamic/_eth_dcpt_lists.h" to    "$PROJECT_HEADER_FILES/framework/driver/ethmac/src/dynamic/_eth_dcpt_lists.h"
    # Add _eth_dcpt_lists.h file to project
    tcpipEthmacDcptHeaderFile = drvPic32mEthmacComponent.createFileSymbol(None, None)
    tcpipEthmacDcptHeaderFile.setSourcePath("driver/ethmac/src/dynamic/_eth_dcpt_lists.h")
    tcpipEthmacDcptHeaderFile.setOutputName("_eth_dcpt_lists.h")
    tcpipEthmacDcptHeaderFile.setDestPath("driver/ethmac/src/dynamic/")
    tcpipEthmacDcptHeaderFile.setProjectPath("config/" + configName + "/driver/ethmac/src/dynamic/")
    tcpipEthmacDcptHeaderFile.setType("HEADER")
    tcpipEthmacDcptHeaderFile.setOverwrite(True)
    
    # file TCPIP_MAC_LIB_H "$HARMONY_VERSION_PATH/framework/driver/ethmac/src/dynamic/drv_ethmac_lib.h" to     "$PROJECT_HEADER_FILES/framework/driver/ethmac/src/dynamic/drv_ethmac_lib.h"
    # Add drv_ethmac_lib.h file to project
    tcpipEthmacLibHeaderFile = drvPic32mEthmacComponent.createFileSymbol(None, None)
    tcpipEthmacLibHeaderFile.setSourcePath("driver/ethmac/src/dynamic/drv_ethmac_lib.h")
    tcpipEthmacLibHeaderFile.setOutputName("drv_ethmac_lib.h")
    tcpipEthmacLibHeaderFile.setDestPath("driver/ethmac/src/dynamic/")
    tcpipEthmacLibHeaderFile.setProjectPath("config/" + configName + "/driver/ethmac/src/dynamic/")
    tcpipEthmacLibHeaderFile.setType("HEADER")
    tcpipEthmacLibHeaderFile.setOverwrite(True)

    tcpipEthmacPic32LibHeaderFile = drvPic32mEthmacComponent.createFileSymbol(None, None)
    tcpipEthmacPic32LibHeaderFile.setSourcePath("driver/ethmac/src/dynamic/drv_eth_pic32_lib.h")
    tcpipEthmacPic32LibHeaderFile.setOutputName("drv_eth_pic32_lib.h")
    tcpipEthmacPic32LibHeaderFile.setDestPath("driver/ethmac/src/dynamic/")
    tcpipEthmacPic32LibHeaderFile.setProjectPath("config/" + configName + "/driver/ethmac/src/dynamic/")
    tcpipEthmacPic32LibHeaderFile.setType("HEADER")
    tcpipEthmacPic32LibHeaderFile.setOverwrite(True)
    
    # file TCPIP_MAC_DRV_C "$HARMONY_VERSION_PATH/framework/driver/ethmac/src/dynamic/drv_ethmac.c" to         "$PROJECT_SOURCE_FILES/framework/driver/ethmac/drv_ethmac.c"
    # Add drv_ethmac.c file
    tcpipEthmacSourceFile = drvPic32mEthmacComponent.createFileSymbol(None, None)
    tcpipEthmacSourceFile.setSourcePath("driver/ethmac/src/dynamic/drv_ethmac.c")
    tcpipEthmacSourceFile.setOutputName("drv_ethmac.c")
    tcpipEthmacSourceFile.setOverwrite(True)
    tcpipEthmacSourceFile.setDestPath("driver/ethmac/src/dynamic/")
    tcpipEthmacSourceFile.setProjectPath("config/" + configName + "/driver/ethmac/src/dynamic/")
    tcpipEthmacSourceFile.setType("SOURCE")
    tcpipEthmacSourceFile.setEnabled(True)
    
    # file TCPIP_MAC_LIB_C "$HARMONY_VERSION_PATH/framework/driver/ethmac/src/dynamic/drv_ethmac_lib.c" to     "$PROJECT_SOURCE_FILES/framework/driver/ethmac/drv_ethmac_lib.c"    
    tcpipEthmacLibSourceFile = drvPic32mEthmacComponent.createFileSymbol(None, None)
    tcpipEthmacLibSourceFile.setSourcePath("driver/ethmac/src/dynamic/drv_ethmac_lib.c")
    tcpipEthmacLibSourceFile.setOutputName("drv_ethmac_lib.c")
    tcpipEthmacLibSourceFile.setOverwrite(True)
    tcpipEthmacLibSourceFile.setDestPath("driver/ethmac/src/dynamic/")
    tcpipEthmacLibSourceFile.setProjectPath("config/" + configName + "/driver/ethmac/src/dynamic/")
    tcpipEthmacLibSourceFile.setType("SOURCE")
    tcpipEthmacLibSourceFile.setEnabled(True)
    
    tcpipEthmacPic32LibSourceFile = drvPic32mEthmacComponent.createFileSymbol(None, None)
    tcpipEthmacPic32LibSourceFile.setSourcePath("driver/ethmac/src/dynamic/drv_eth_pic32_lib.c")
    tcpipEthmacPic32LibSourceFile.setOutputName("drv_eth_pic32_lib.c")
    tcpipEthmacPic32LibSourceFile.setOverwrite(True)
    tcpipEthmacPic32LibSourceFile.setDestPath("driver/ethmac/src/dynamic/")
    tcpipEthmacPic32LibSourceFile.setProjectPath("config/" + configName + "/driver/ethmac/src/dynamic/")
    tcpipEthmacPic32LibSourceFile.setType("SOURCE")
    tcpipEthmacPic32LibSourceFile.setEnabled(True)
    
def tcpipEthMacMenuVisibleSingle(symbol, event):
    if (event["value"] == True):
        print("EthMac Menu Visible.")       
        symbol.setVisible(True)
    else:
        print("EthMac Menu Invisible.")
        symbol.setVisible(False)

        
def tcpipEthMacMdixSwapVisible(symbol, event):
    tcpipEthMacAutoMdix = Database.getSymbolValue("drvPic32mEthmac","TCPIP_EMAC_ETH_OF_MDIX_AUTO")
    if (event["value"] == True):
        symbol.setVisible(False)
    else:
        symbol.setVisible(True)
        
def setEthmacInterruptData(status):

    Database.setSymbolValue("core", tcpipEthmacInterruptVector, status, 1)
    Database.setSymbolValue("core", tcpipEthmacInterruptHandlerLock, status, 1)
    #interruptName = tcpipEthmacInterruptHandler.split("_INTERRUPT_HANDLER")[0]
    
    if status == True:
        Database.setSymbolValue("core", tcpipEthmacInterruptHandler, "ETHERNET_InterruptHandler", 1)
    else:
        Database.setSymbolValue("core", tcpipEthmacInterruptHandler, "ETHERNET_Handler", 1)

def updateEthmacInterruptData(symbol, event):

    if event["id"] == "INTERRUPT_ENABLE":
        setEthmacInterruptData(event["value"])

    if tcpipEthmacInterruptEnable.getValue() == True and Database.getSymbolValue("core", tcpipEthmacInterruptVectorUpdate) == True:
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def _get_enblReg_parms(vectorNumber):

    # This takes in vector index for interrupt, and returns the IECx register name as well as
    # mask and bit location within it for given interrupt
    temp = float(vectorNumber) / 32.0
    index = int(vectorNumber / 32)
    regName = "IEC" + str(index)
    bit = float(temp % 1)
    bitPosn = int(32.0 * bit)
    return regName, str(bitPosn)

def _get_statReg_parms(vectorNumber):

    # This takes in vector index for interrupt, and returns the IFSx register name as well as
    # mask and bit location within it for given interrupt
    temp = float(vectorNumber) / 32.0
    index = int(vectorNumber / 32)
    regName = "IFS" + str(index)
    bit = float(temp % 1)
    bitPosn = int(32.0 * bit)
    return regName, str(bitPosn)

def getIRQnumber(string):

    interruptsChildren = ATDF.getNode('/avr-tools-device-file/devices/device/interrupts').getChildren()

    for param in interruptsChildren:
        modInst = param.getAttribute("name")
        if string == modInst:
            irq_index = param.getAttribute("index")

    return irq_index
    
def onAttachmentConnected(source, target):
    if (source["id"] == "ETHMAC_PHY_Dependency"): 
        Database.setSymbolValue("drvPic32mEthmac", "DRV_INTMAC_PHY_TYPE", target["component"].getDisplayName(),2)
        
def onAttachmentDisconnected(source, target):
    if (source["id"] == "ETHMAC_PHY_Dependency"): 
        Database.clearSymbolValue("drvPic32mEthmac", "DRV_INTMAC_PHY_TYPE")

def tcpipEthmacHeapCalc(): 
    nTxDescriptors = Database.getSymbolValue("drvPic32mEthmac","TCPIP_EMAC_TX_DESCRIPTORS")
    nRxDescriptors = Database.getSymbolValue("drvPic32mEthmac","TCPIP_EMAC_RX_DESCRIPTORS")
    rxBuffSize = Database.getSymbolValue("drvPic32mEthmac","TCPIP_EMAC_RX_BUFF_SIZE")
    nRxDedicatedBuffers = Database.getSymbolValue("drvPic32mEthmac","TCPIP_EMAC_RX_DEDICATED_BUFFERS")
    
    heap_size = ((nTxDescriptors + nRxDescriptors) * 24) + rxBuffSize * nRxDedicatedBuffers
    return heap_size    
    
def tcpipEthmacHeapUpdate(symbol, event): 
    heap_size = tcpipEthmacHeapCalc()
    symbol.setValue(heap_size)
    if(event["id"] == "TCPIP_STACK_HEAP_CALC_MASK"):
        symbol.setVisible(event["value"])
    
def destroyComponent(drvPic32mEthmacComponent):
    Database.setSymbolValue("drvPic32mEthmac", "TCPIP_USE_ETH_MAC", False, 2)