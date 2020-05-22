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

def instantiateComponent(drvGmacComponent):
    global interruptVector
    global interruptHandler
    global interruptHandlerLock
    
    print("TCPIP Ethernet MAC Component")   
    configName = Variables.get("__CONFIGURATION_NAME")      
    
    gmac_node = ATDF.getNode('/avr-tools-device-file/devices/device/peripherals/module@[name="GMAC"]')
    gmac_periphID = str(gmac_node.getAttribute("id"))
    
    # GMAC device name
    tcpipGmacDevName = drvGmacComponent.createStringSymbol("TCPIP_INTMAC_DEVICE", None)
    tcpipGmacDevName.setVisible(False)
    if(gmac_periphID == "11046"): # SAME70, SAMV71
        tcpipGmacDevName.setDefaultValue("SAME7x_V7x")   
    elif (gmac_periphID == "U2005"): # SAME54
        tcpipGmacDevName.setDefaultValue("SAME5x")  
    elif (gmac_periphID == "44152"): # SAMA5D2
        tcpipGmacDevName.setDefaultValue("SAMA5D2")  
        
    if(gmac_periphID == "11046"): # SAME70, SAMV71  
        # Enable MPU Setting for GMAC descriptor
        if(Database.getSymbolValue("core", "CoreUseMPU") != True):
            Database.setSymbolValue("core", "CoreUseMPU", True)
        if(Database.getSymbolValue("core", "CoreMPU_DEFAULT") != True):  
            Database.setSymbolValue("core", "CoreMPU_DEFAULT", True)            
        mpuNumRegions = Database.getSymbolValue("core", "MPU_NUMBER_REGIONS")
        for i in range(0,mpuNumRegions):
            if(Database.getSymbolValue("core", ("MPU_Region_" + str(i) + "_Enable")) != True):
                Database.setSymbolValue("core", ("MPU_Region_" + str(i) + "_Enable"), True)
                Database.setSymbolValue("core", ("MPU_Region_Name" + str(i)), "GMAC Descriptor")
                Database.setSymbolValue("core", ("MPU_Region_" + str(i) + "_Address"), Database.getSymbolValue("core", ("MPU_Region_" + str(i) + "_Address")) + 0x2045F000) 
                Database.setSymbolValue("core", ("MPU_Region_" + str(i) + "_Size"), 7)
                Database.setSymbolValue("core", ("MPU_Region_" + str(i) + "_Type"), 5)
                Database.setSymbolValue("core", ("MPU_Region_" + str(i) + "_Access"), 3)
                break
        
    # Enable GMAC clock
    Database.setSymbolValue("core", "GMAC_CLOCK_ENABLE", True, 2)
    # Use Internal Ethernet MAC Driver? 
    drvGmac = drvGmacComponent.createBooleanSymbol("TCPIP_USE_ETH_MAC", None)
    drvGmac.setLabel("Use Internal Ethernet MAC Driver?")
    drvGmac.setVisible(False)
    drvGmac.setDescription("Use Internal Ethernet MAC Driver?")
    drvGmac.setDefaultValue(True)
        
    # Use Internal Ethernet MAC Driver? 
    tcpipGmacQue0 = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_QUEUE_0", None)
    tcpipGmacQue0.setLabel("GMAC Queue 0")
    tcpipGmacQue0.setVisible(True)
    tcpipGmacQue0.setDescription("GMAC Queue 0")
    tcpipGmacQue0.setDefaultValue(True)


    # Number of Tx Descriptors To Be Created for Queue0
    tcpipGmacTxDescCountQue0 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE0", tcpipGmacQue0)
    tcpipGmacTxDescCountQue0.setLabel("Number of Tx Descriptors To Be Created")
    tcpipGmacTxDescCountQue0.setVisible(True)
    tcpipGmacTxDescCountQue0.setDescription("Number of Tx Descriptors To Be Created for Queue0")
    tcpipGmacTxDescCountQue0.setDefaultValue(10)
    tcpipGmacTxDescCountQue0.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_0"])

    # Number of Rx Descriptors To Be Created for Queue0
    tcpipGmacRxDescCountQue0 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE0", tcpipGmacQue0)
    tcpipGmacRxDescCountQue0.setLabel("Number of Rx Descriptors To Be Created")
    tcpipGmacRxDescCountQue0.setVisible(True)
    tcpipGmacRxDescCountQue0.setDescription("Number of Rx Descriptors To Be Created for Queue0")
    tcpipGmacRxDescCountQue0.setDefaultValue(10)
    tcpipGmacRxDescCountQue0.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_0"])

    # Size Of RX Buffer for Queue0. Should Be Multiple Of 16.
    tcpipGmacRxBuffSizeQue0 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_SIZE_QUE0", tcpipGmacQue0)
    tcpipGmacRxBuffSizeQue0.setLabel("Size Of RX Buffer. Should Be Multiple Of 16.")
    tcpipGmacRxBuffSizeQue0.setVisible(True)
    tcpipGmacRxBuffSizeQue0.setDescription("Size Of RX Buffer for Queue0. Should Be Multiple Of 16.")
    tcpipGmacRxBuffSizeQue0.setDefaultValue(1536)
    tcpipGmacRxBuffSizeQue0.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_0"])

    # Size Of TX Buffer for Queue0. Should Be Multiple Of 16.
    tcpipGmacTxBuffSizeQue0 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_BUFF_SIZE_QUE0", tcpipGmacQue0)
    tcpipGmacTxBuffSizeQue0.setLabel("Size Of TX Buffer. Should Be Multiple Of 16.")
    tcpipGmacTxBuffSizeQue0.setVisible(True)
    tcpipGmacTxBuffSizeQue0.setDescription("Size Of TX Buffer for Queue0. Should Be Multiple Of 16.")
    tcpipGmacTxBuffSizeQue0.setDefaultValue(1536)
    tcpipGmacTxBuffSizeQue0.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_0"])
    
    # Number of additional Rx buffers To Be Created for Queue0
    tcpipGmacRxAddlBuffCountQue0 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE0", tcpipGmacQue0)
    tcpipGmacRxAddlBuffCountQue0.setLabel("Number of additional Rx buffers to be created")
    tcpipGmacRxAddlBuffCountQue0.setVisible(True)
    tcpipGmacRxAddlBuffCountQue0.setDescription("Number of additional Sticky Rx buffers to be created for Queue0")
    tcpipGmacRxAddlBuffCountQue0.setDefaultValue(2)
    tcpipGmacRxAddlBuffCountQue0.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_0"])  
        
    # Rx buffer Count Threshold to allocate new Rx buffers, for Queue0
    tcpipGmacRxBuffCountThresQue0 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE0", tcpipGmacQue0)
    tcpipGmacRxBuffCountThresQue0.setLabel("Minimum Threshold for Rx Buffer replenish process")
    tcpipGmacRxBuffCountThresQue0.setVisible(True)
    tcpipGmacRxBuffCountThresQue0.setDescription("Rx buffer Count Threshold to allocate new Rx buffers for Queue0")
    tcpipGmacRxBuffCountThresQue0.setDefaultValue(1)
    tcpipGmacRxBuffCountThresQue0.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_0"]) 
        
    # Rx buffer allocate Count for Queue0
    tcpipGmacRxBuffAllocCountQue0 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE0", tcpipGmacQue0)
    tcpipGmacRxBuffAllocCountQue0.setLabel("Rx Buffer allocate count during  replenish process")
    tcpipGmacRxBuffAllocCountQue0.setVisible(True)
    tcpipGmacRxBuffAllocCountQue0.setDescription("Rx buffer allocate Count for Queue0")
    tcpipGmacRxBuffAllocCountQue0.setDefaultValue(1)
    tcpipGmacRxBuffAllocCountQue0.setMin(1)
    tcpipGmacRxBuffAllocCountQue0.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_0"]) 
    
    # SAME70/V71 has 6 Priority Queue
    # SAMA5D2 has 3 Priority Queue
    # SAME54 has only 1 Queue
    if(gmac_periphID == "11046") or (gmac_periphID == "44152"): # SAME70 or SAMV71 or SAMA5D2
        # GMAC Queue 1  
        tcpipGmacQue1 = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_QUEUE_1", None)
        tcpipGmacQue1.setLabel("GMAC Queue 1")
        tcpipGmacQue1.setVisible(True)
        tcpipGmacQue1.setDescription("GMAC Queue 1")
        tcpipGmacQue1.setDefaultValue(False)
        #tcpipGmacQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

        # Number of Tx Descriptors To Be Created for Queue1
        tcpipGmacTxDescCountQue1 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE1", tcpipGmacQue1)
        tcpipGmacTxDescCountQue1.setLabel("Number of Tx Descriptors To Be Created")
        tcpipGmacTxDescCountQue1.setVisible(False)
        tcpipGmacTxDescCountQue1.setDescription("Number of Tx Descriptors To Be Created for Queue1")
        tcpipGmacTxDescCountQue1.setDefaultValue(1)
        tcpipGmacTxDescCountQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_1"])

        # Number of Rx Descriptors To Be Created for Queue1
        tcpipGmacRxDescCountQue1 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE1", tcpipGmacQue1)
        tcpipGmacRxDescCountQue1.setLabel("Number of Rx Descriptors To Be Created")
        tcpipGmacRxDescCountQue1.setVisible(False)
        tcpipGmacRxDescCountQue1.setDescription("Number of Rx Descriptors To Be Created for Queue1")
        tcpipGmacRxDescCountQue1.setDefaultValue(1)
        tcpipGmacRxDescCountQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_1"])

        # Size Of RX Buffer for Queue1. Should Be Multiple Of 16.
        tcpipGmacRxBuffSizeQue1 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_SIZE_QUE1", tcpipGmacQue1)
        tcpipGmacRxBuffSizeQue1.setLabel("Size Of RX Buffer. Should Be Multiple Of 16.")
        tcpipGmacRxBuffSizeQue1.setVisible(False)
        tcpipGmacRxBuffSizeQue1.setDescription("Size Of RX Buffer for Queue1. Should Be Multiple Of 16.")
        tcpipGmacRxBuffSizeQue1.setDefaultValue(64)
        tcpipGmacRxBuffSizeQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_1"])

        # Size Of RX Buffer for Queue1. Should Be Multiple Of 16.
        tcpipGmacTxBuffSizeQue1 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_BUFF_SIZE_QUE1", tcpipGmacQue1)
        tcpipGmacTxBuffSizeQue1.setLabel("Size Of TX Buffer. Should Be Multiple Of 16.")
        tcpipGmacTxBuffSizeQue1.setVisible(False)
        tcpipGmacTxBuffSizeQue1.setDescription("Size Of TX Buffer for Queue1. Should Be Multiple Of 16.")
        tcpipGmacTxBuffSizeQue1.setDefaultValue(64)
        tcpipGmacTxBuffSizeQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_1"])
        
        # Number of additional Rx buffers To Be Created for Queue1
        tcpipGmacRxAddlBuffCountQue1 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE1", tcpipGmacQue1)
        tcpipGmacRxAddlBuffCountQue1.setLabel("Number of additional Rx buffers to be created")
        tcpipGmacRxAddlBuffCountQue1.setVisible(False)
        tcpipGmacRxAddlBuffCountQue1.setDescription("Number of additional Sticky Rx buffers to Be Created for Queue1")
        tcpipGmacRxAddlBuffCountQue1.setDefaultValue(0)
        tcpipGmacRxAddlBuffCountQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_1"])  
            
        # Rx buffer Count Threshold to allocate new Rx buffers, for Queue1
        tcpipGmacRxBuffCountThresQue1 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE1", tcpipGmacQue1)
        tcpipGmacRxBuffCountThresQue1.setLabel("Minimum Threshold for Rx Buffer replenish process")
        tcpipGmacRxBuffCountThresQue1.setVisible(False)
        tcpipGmacRxBuffCountThresQue1.setDescription("Rx buffer Count Threshold to allocate new Rx buffers for Queue1")
        tcpipGmacRxBuffCountThresQue1.setDefaultValue(1)
        tcpipGmacRxBuffCountThresQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_1"]) 
            
        # Rx buffer allocate Count for Queue1
        tcpipGmacRxBuffAllocCountQue1 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE1", tcpipGmacQue1)
        tcpipGmacRxBuffAllocCountQue1.setLabel("Rx Buffer allocate count during  replenish process")
        tcpipGmacRxBuffAllocCountQue1.setVisible(False)
        tcpipGmacRxBuffAllocCountQue1.setDescription("Rx buffer allocate Count for Queue1")
        tcpipGmacRxBuffAllocCountQue1.setDefaultValue(1)
        tcpipGmacRxBuffAllocCountQue1.setMin(1)
        tcpipGmacRxBuffAllocCountQue1.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_1"]) 
    
        # GMAC Queue 2
        tcpipGmacQue2 = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_QUEUE_2", None)
        tcpipGmacQue2.setLabel("GMAC Queue 2")
        tcpipGmacQue2.setVisible(True)
        tcpipGmacQue2.setDescription("GMAC Queue 2")
        tcpipGmacQue2.setDefaultValue(False)
        #tcpipGmacQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

        # Number of Tx Descriptors To Be Created for Queue2
        tcpipGmacTxDescCountQue2 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE2", tcpipGmacQue2)
        tcpipGmacTxDescCountQue2.setLabel("Number of Tx Descriptors To Be Created")
        tcpipGmacTxDescCountQue2.setVisible(False)
        tcpipGmacTxDescCountQue2.setDescription("Number of Tx Descriptors To Be Created for Queue2")
        tcpipGmacTxDescCountQue2.setDefaultValue(1)
        tcpipGmacTxDescCountQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_2"])

        # Number of Rx Descriptors To Be Created for Queue2
        tcpipGmacRxDescCountQue2 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE2", tcpipGmacQue2)
        tcpipGmacRxDescCountQue2.setLabel("Number of Rx Descriptors To Be Created")
        tcpipGmacRxDescCountQue2.setVisible(False)
        tcpipGmacRxDescCountQue2.setDescription("Number of Rx Descriptors To Be Created for Queue2")
        tcpipGmacRxDescCountQue2.setDefaultValue(1)
        tcpipGmacRxDescCountQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_2"])

        # Size Of RX Buffer for Queue2. Should Be Multiple Of 16.
        tcpipGmacRxBuffSizeQue2 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_SIZE_QUE2", tcpipGmacQue2)
        tcpipGmacRxBuffSizeQue2.setLabel("Size Of RX Buffer. Should Be Multiple Of 16.")
        tcpipGmacRxBuffSizeQue2.setVisible(False)
        tcpipGmacRxBuffSizeQue2.setDescription("Size Of RX Buffer for Queue2. Should Be Multiple Of 16.")
        tcpipGmacRxBuffSizeQue2.setDefaultValue(64)
        tcpipGmacRxBuffSizeQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_2"])

        # Size Of TX Buffer for Queue2. Should Be Multiple Of 16.
        tcpipGmacTxBuffSizeQue2 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_BUFF_SIZE_QUE2", tcpipGmacQue2)
        tcpipGmacTxBuffSizeQue2.setLabel("Size Of TX Buffer. Should Be Multiple Of 16.")
        tcpipGmacTxBuffSizeQue2.setVisible(False)
        tcpipGmacTxBuffSizeQue2.setDescription("Size Of TX Buffer for Queue2. Should Be Multiple Of 16.")
        tcpipGmacTxBuffSizeQue2.setDefaultValue(64)
        tcpipGmacTxBuffSizeQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_2"])
        
        # Number of additional Rx buffers To Be Created for Queue2
        tcpipGmacRxAddlBuffCountQue2 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE2", tcpipGmacQue2)
        tcpipGmacRxAddlBuffCountQue2.setLabel("Number of additional Rx buffers to be created")
        tcpipGmacRxAddlBuffCountQue2.setVisible(False)
        tcpipGmacRxAddlBuffCountQue2.setDescription("Number of additional Sticky Rx buffers to Be Created for Queue2")
        tcpipGmacRxAddlBuffCountQue2.setDefaultValue(0)
        tcpipGmacRxAddlBuffCountQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_2"])
            
        # Rx buffer Count Threshold to allocate new Rx buffers, for Queue2
        tcpipGmacRxBuffCountThresQue2 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE2", tcpipGmacQue2)
        tcpipGmacRxBuffCountThresQue2.setLabel("Minimum Threshold for Rx Buffer replenish process")
        tcpipGmacRxBuffCountThresQue2.setVisible(False)
        tcpipGmacRxBuffCountThresQue2.setDescription("Rx buffer Count Threshold to allocate new Rx buffers for Queue2")
        tcpipGmacRxBuffCountThresQue2.setDefaultValue(1)
        tcpipGmacRxBuffCountThresQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_2"]) 
            
        # Rx buffer allocate Count for Queue2
        tcpipGmacRxBuffAllocCountQue2 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE2", tcpipGmacQue2)
        tcpipGmacRxBuffAllocCountQue2.setLabel("Rx Buffer allocate count during  replenish process")
        tcpipGmacRxBuffAllocCountQue2.setVisible(False)
        tcpipGmacRxBuffAllocCountQue2.setDescription("Rx buffer allocate Count for Queue2")
        tcpipGmacRxBuffAllocCountQue2.setDefaultValue(1)
        tcpipGmacRxBuffAllocCountQue2.setMin(1)
        tcpipGmacRxBuffAllocCountQue2.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_2"]) 
        
    if(gmac_periphID == "11046"): # SAME70 or SAMV71         
        # GMAC Queue 3
        tcpipGmacQue3 = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_QUEUE_3", None)
        tcpipGmacQue3.setLabel("GMAC Queue 3")
        tcpipGmacQue3.setVisible(True)
        tcpipGmacQue3.setDescription("GMAC Queue 3")
        tcpipGmacQue3.setDefaultValue(False)
        #tcpipGmacQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

        # Number of Tx Descriptors To Be Created for Queue3
        tcpipGmacTxDescCountQue3 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE3", tcpipGmacQue3)
        tcpipGmacTxDescCountQue3.setLabel("Number of Tx Descriptors To Be Created")
        tcpipGmacTxDescCountQue3.setVisible(False)
        tcpipGmacTxDescCountQue3.setDescription("Number of Tx Descriptors To Be Created for Queue3")
        tcpipGmacTxDescCountQue3.setDefaultValue(1)
        tcpipGmacTxDescCountQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_3"])

        # Number of Rx Descriptors To Be Created for Queue3
        tcpipGmacRxDescCountQue3 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE3", tcpipGmacQue3)
        tcpipGmacRxDescCountQue3.setLabel("Number of Rx Descriptors To Be Created")
        tcpipGmacRxDescCountQue3.setVisible(False)
        tcpipGmacRxDescCountQue3.setDescription("Number of Rx Descriptors To Be Created for Queue3")
        tcpipGmacRxDescCountQue3.setDefaultValue(1)
        tcpipGmacRxDescCountQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_3"])

        # Size Of RX Buffer for Queue3. Should Be Multiple Of 16.
        tcpipGmacRxBuffSizeQue3 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_SIZE_QUE3", tcpipGmacQue3)
        tcpipGmacRxBuffSizeQue3.setLabel("Size Of RX Buffer. Should Be Multiple Of 16.")
        tcpipGmacRxBuffSizeQue3.setVisible(False)
        tcpipGmacRxBuffSizeQue3.setDescription("Size Of RX Buffer for Queue3. Should Be Multiple Of 16.")
        tcpipGmacRxBuffSizeQue3.setDefaultValue(64)
        tcpipGmacRxBuffSizeQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_3"])

        # Size Of TX Buffer for Queue3. Should Be Multiple Of 16.
        tcpipGmacTxBuffSizeQue3 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_BUFF_SIZE_QUE3", tcpipGmacQue3)
        tcpipGmacTxBuffSizeQue3.setLabel("Size Of TX Buffer. Should Be Multiple Of 16.")
        tcpipGmacTxBuffSizeQue3.setVisible(False)
        tcpipGmacTxBuffSizeQue3.setDescription("Size Of TX Buffer for Queue3. Should Be Multiple Of 16.")
        tcpipGmacTxBuffSizeQue3.setDefaultValue(64)
        tcpipGmacTxBuffSizeQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_3"])
        
        # Number of additional Rx buffers To Be Created for Queue3
        tcpipGmacRxAddlBuffCountQue3 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE3", tcpipGmacQue3)
        tcpipGmacRxAddlBuffCountQue3.setLabel("Number of additional Rx buffers to be created")
        tcpipGmacRxAddlBuffCountQue3.setVisible(False)
        tcpipGmacRxAddlBuffCountQue3.setDescription("Number of additional Sticky Rx buffers to Be Created for Queue3")
        tcpipGmacRxAddlBuffCountQue3.setDefaultValue(0)
        tcpipGmacRxAddlBuffCountQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_3"])
            
        # Rx buffer Count Threshold to allocate new Rx buffers, for Queue3
        tcpipGmacRxBuffCountThresQue3 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE3", tcpipGmacQue3)
        tcpipGmacRxBuffCountThresQue3.setLabel("Minimum Threshold for Rx Buffer replenish process")
        tcpipGmacRxBuffCountThresQue3.setVisible(False)
        tcpipGmacRxBuffCountThresQue3.setDescription("Rx buffer Count Threshold to allocate new Rx buffers for Queue3")
        tcpipGmacRxBuffCountThresQue3.setDefaultValue(1)
        tcpipGmacRxBuffCountThresQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_3"]) 
            
        # Rx buffer allocate Count for Queue3
        tcpipGmacRxBuffAllocCountQue3 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE3", tcpipGmacQue3)
        tcpipGmacRxBuffAllocCountQue3.setLabel("Rx Buffer allocate count during  replenish process")
        tcpipGmacRxBuffAllocCountQue3.setVisible(False)
        tcpipGmacRxBuffAllocCountQue3.setDescription("Rx buffer allocate Count for Queue3")
        tcpipGmacRxBuffAllocCountQue3.setDefaultValue(1)
        tcpipGmacRxBuffAllocCountQue3.setMin(1)
        tcpipGmacRxBuffAllocCountQue3.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_3"]) 
        
        # GMAC Queue 4
        tcpipGmacQue4 = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_QUEUE_4", None)
        tcpipGmacQue4.setLabel("GMAC Queue 4")
        tcpipGmacQue4.setVisible(True)
        tcpipGmacQue4.setDescription("GMAC Queue 4")
        tcpipGmacQue4.setDefaultValue(False)
        #tcpipGmacQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

        # Number of Tx Descriptors To Be Created for Queue4
        tcpipGmacTxDescCountQue4 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE4", tcpipGmacQue4)
        tcpipGmacTxDescCountQue4.setLabel("Number of Tx Descriptors To Be Created")
        tcpipGmacTxDescCountQue4.setVisible(False)
        tcpipGmacTxDescCountQue4.setDescription("Number of Tx Descriptors To Be Created for Queue4")
        tcpipGmacTxDescCountQue4.setDefaultValue(1)
        tcpipGmacTxDescCountQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_4"])

        # Number of Rx Descriptors To Be Created for Queue4
        tcpipGmacRxDescCountQue4 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE4", tcpipGmacQue4)
        tcpipGmacRxDescCountQue4.setLabel("Number of Rx Descriptors To Be Created")
        tcpipGmacRxDescCountQue4.setVisible(False)
        tcpipGmacRxDescCountQue4.setDescription("Number of Rx Descriptors To Be Created for Queue4")
        tcpipGmacRxDescCountQue4.setDefaultValue(1)
        tcpipGmacRxDescCountQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_4"])

        # Size Of RX Buffer for Queue4. Should Be Multiple Of 16.
        tcpipGmacRxBuffSizeQue4 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_SIZE_QUE4", tcpipGmacQue4)
        tcpipGmacRxBuffSizeQue4.setLabel("Size Of RX Buffer. Should Be Multiple Of 16.")
        tcpipGmacRxBuffSizeQue4.setVisible(False)
        tcpipGmacRxBuffSizeQue4.setDescription("Size Of RX Buffer for Queue4. Should Be Multiple Of 16.")
        tcpipGmacRxBuffSizeQue4.setDefaultValue(64)
        tcpipGmacRxBuffSizeQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_4"])

        # Size Of TX Buffer for Queue4. Should Be Multiple Of 16.
        tcpipGmacTxBuffSizeQue4 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_BUFF_SIZE_QUE4", tcpipGmacQue4)
        tcpipGmacTxBuffSizeQue4.setLabel("Size Of TX Buffer. Should Be Multiple Of 16.")
        tcpipGmacTxBuffSizeQue4.setVisible(False)
        tcpipGmacTxBuffSizeQue4.setDescription("Size Of TX Buffer for Queue4. Should Be Multiple Of 16.")
        tcpipGmacTxBuffSizeQue4.setDefaultValue(64)
        tcpipGmacTxBuffSizeQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_4"])
        
        # Number of additional Rx buffers To Be Created for Queue4
        tcpipGmacRxAddlBuffCountQue4 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE4", tcpipGmacQue4)
        tcpipGmacRxAddlBuffCountQue4.setLabel("Number of additional Rx buffers to be created")
        tcpipGmacRxAddlBuffCountQue4.setVisible(False)
        tcpipGmacRxAddlBuffCountQue4.setDescription("Number of additional Sticky Rx buffers to Be Created for Queue4")
        tcpipGmacRxAddlBuffCountQue4.setDefaultValue(0)
        tcpipGmacRxAddlBuffCountQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_4"])
            
        # Rx buffer Count Threshold to allocate new Rx buffers, for Queue4
        tcpipGmacRxBuffCountThresQue4 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE4", tcpipGmacQue4)
        tcpipGmacRxBuffCountThresQue4.setLabel("Minimum Threshold for Rx Buffer replenish process")
        tcpipGmacRxBuffCountThresQue4.setVisible(False)
        tcpipGmacRxBuffCountThresQue4.setDescription("Rx buffer Count Threshold to allocate new Rx buffers for Queue4")
        tcpipGmacRxBuffCountThresQue4.setDefaultValue(1)
        tcpipGmacRxBuffCountThresQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_4"]) 
            
        # Rx buffer allocate Count for Queue4
        tcpipGmacRxBuffAllocCountQue4 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE4", tcpipGmacQue4)
        tcpipGmacRxBuffAllocCountQue4.setLabel("Rx Buffer allocate count during  replenish process")
        tcpipGmacRxBuffAllocCountQue4.setVisible(False)
        tcpipGmacRxBuffAllocCountQue4.setDescription("Rx buffer allocate Count for Queue4")
        tcpipGmacRxBuffAllocCountQue4.setDefaultValue(1)
        tcpipGmacRxBuffAllocCountQue4.setMin(1)
        tcpipGmacRxBuffAllocCountQue4.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_4"])     
        
        # GMAC Queue 5
        tcpipGmacQue5 = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_QUEUE_5", None)
        tcpipGmacQue5.setLabel("GMAC Queue 5")
        tcpipGmacQue5.setVisible(True)
        tcpipGmacQue5.setDescription("GMAC Queue 5")
        tcpipGmacQue5.setDefaultValue(False)
        #tcpipGmacQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_USE_ETH_MAC"])

        # Number of Tx Descriptors To Be Created for Queue5
        tcpipGmacTxDescCountQue5 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE5", tcpipGmacQue5)
        tcpipGmacTxDescCountQue5.setLabel("Number of Tx Descriptors To Be Created")
        tcpipGmacTxDescCountQue5.setVisible(False)
        tcpipGmacTxDescCountQue5.setDescription("Number of Tx Descriptors To Be Created for Queue5")
        tcpipGmacTxDescCountQue5.setDefaultValue(1)
        tcpipGmacTxDescCountQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_5"])

        # Number of Rx Descriptors To Be Created for Queue5
        tcpipGmacRxDescCountQue5 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE5", tcpipGmacQue5)
        tcpipGmacRxDescCountQue5.setLabel("Number of Rx Descriptors To Be Created")
        tcpipGmacRxDescCountQue5.setVisible(False)
        tcpipGmacRxDescCountQue5.setDescription("Number of Rx Descriptors To Be Created for Queue5")
        tcpipGmacRxDescCountQue5.setDefaultValue(1)
        tcpipGmacRxDescCountQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_5"])

        # Size Of RX Buffer for Queue5. Should Be Multiple Of 16.
        tcpipGmacRxBuffSizeQue5 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_SIZE_QUE5", tcpipGmacQue5)
        tcpipGmacRxBuffSizeQue5.setLabel("Size Of RX Buffer. Should Be Multiple Of 16.")
        tcpipGmacRxBuffSizeQue5.setVisible(False)
        tcpipGmacRxBuffSizeQue5.setDescription("Size Of RX Buffer for Queue5. Should Be Multiple Of 16.")
        tcpipGmacRxBuffSizeQue5.setDefaultValue(64)
        tcpipGmacRxBuffSizeQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_5"])

        # Size Of TX Buffer for Queue5. Should Be Multiple Of 16.
        tcpipGmacTxBuffSizeQue5 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_BUFF_SIZE_QUE5", tcpipGmacQue5)
        tcpipGmacTxBuffSizeQue5.setLabel("Size Of TX Buffer. Should Be Multiple Of 16.")
        tcpipGmacTxBuffSizeQue5.setVisible(False)
        tcpipGmacTxBuffSizeQue5.setDescription("Size Of TX Buffer for Queue5. Should Be Multiple Of 16.")
        tcpipGmacTxBuffSizeQue5.setDefaultValue(64)
        tcpipGmacTxBuffSizeQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_5"])
        
        # Number of additional Rx buffers To Be Created for Queue5
        tcpipGmacRxAddlBuffCountQue5 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE5", tcpipGmacQue5)
        tcpipGmacRxAddlBuffCountQue5.setLabel("Number of additional Rx buffers to be created")
        tcpipGmacRxAddlBuffCountQue5.setVisible(False)
        tcpipGmacRxAddlBuffCountQue5.setDescription("Number of additional Sticky Rx buffers to Be Created for Queue5")
        tcpipGmacRxAddlBuffCountQue5.setDefaultValue(0)
        tcpipGmacRxAddlBuffCountQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_5"])      
            
        # Rx buffer Count Threshold to allocate new Rx buffers, for Queue5
        tcpipGmacRxBuffCountThresQue5 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE5", tcpipGmacQue5)
        tcpipGmacRxBuffCountThresQue5.setLabel("Minimum Threshold for Rx Buffer replenish process")
        tcpipGmacRxBuffCountThresQue5.setVisible(False)
        tcpipGmacRxBuffCountThresQue5.setDescription("Rx buffer Count Threshold to allocate new Rx buffers for Queue5")
        tcpipGmacRxBuffCountThresQue5.setDefaultValue(1)
        tcpipGmacRxBuffCountThresQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_5"]) 
            
        # Rx buffer allocate Count for Queue5
        tcpipGmacRxBuffAllocCountQue5 = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE5", tcpipGmacQue5)
        tcpipGmacRxBuffAllocCountQue5.setLabel("Rx Buffer allocate count during  replenish process")
        tcpipGmacRxBuffAllocCountQue5.setVisible(False)
        tcpipGmacRxBuffAllocCountQue5.setDescription("Rx buffer allocate Count for Queue5")
        tcpipGmacRxBuffAllocCountQue5.setDefaultValue(1)
        tcpipGmacRxBuffAllocCountQue5.setMin(1)
        tcpipGmacRxBuffAllocCountQue5.setDependencies(tcpipEthMacMenuVisibleSingle, ["TCPIP_GMAC_QUEUE_5"])
        
        gmacheapdependency = [  "TCPIP_GMAC_QUEUE_0","TCPIP_GMAC_RX_EN_QUE0","TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE0",
                                "TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE0","TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE0","TCPIP_GMAC_RX_BUFF_SIZE_QUE0",
                                "TCPIP_GMAC_TX_EN_QUE0","TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE0","TCPIP_GMAC_QUEUE_1","TCPIP_GMAC_RX_EN_QUE1",
                                "TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE1","TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE1","TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE1",
                                "TCPIP_GMAC_RX_BUFF_SIZE_QUE1","TCPIP_GMAC_TX_EN_QUE1","TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE1","TCPIP_GMAC_QUEUE_2",
                                "TCPIP_GMAC_RX_EN_QUE2","TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE2","TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE2",
                                "TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE2","TCPIP_GMAC_RX_BUFF_SIZE_QUE2","TCPIP_GMAC_TX_EN_QUE2",
                                "TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE2","TCPIP_GMAC_QUEUE_3","TCPIP_GMAC_RX_EN_QUE3","TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE3",
                                "TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE3","TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE3","TCPIP_GMAC_RX_BUFF_SIZE_QUE3",
                                "TCPIP_GMAC_TX_EN_QUE3","TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE3","TCPIP_GMAC_QUEUE_4","TCPIP_GMAC_RX_EN_QUE4",
                                "TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE4","TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE4","TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE4",
                                "TCPIP_GMAC_RX_BUFF_SIZE_QUE4","TCPIP_GMAC_TX_EN_QUE4","TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE4",
                                "TCPIP_GMAC_QUEUE_5","TCPIP_GMAC_RX_EN_QUE5","TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE5","TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE5",
                                "TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE5","TCPIP_GMAC_RX_BUFF_SIZE_QUE5","TCPIP_GMAC_TX_EN_QUE5",
                                "TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE5","tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]
    
    if(gmac_periphID == "44152"): # SAMA5D2
        gmacheapdependency = [  "TCPIP_GMAC_QUEUE_0","TCPIP_GMAC_RX_EN_QUE0","TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE0","TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE0",
                                "TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE0","TCPIP_GMAC_RX_BUFF_SIZE_QUE0","TCPIP_GMAC_TX_EN_QUE0",
                                "TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE0","TCPIP_GMAC_QUEUE_1","TCPIP_GMAC_RX_EN_QUE1","TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE1",
                                "TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE1","TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE1","TCPIP_GMAC_RX_BUFF_SIZE_QUE1",
                                "TCPIP_GMAC_TX_EN_QUE1","TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE1","TCPIP_GMAC_QUEUE_2","TCPIP_GMAC_RX_EN_QUE2",
                                "TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE2","TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE2","TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE2",
                                "TCPIP_GMAC_RX_BUFF_SIZE_QUE2","TCPIP_GMAC_TX_EN_QUE2","TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE2",
                                "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]
    
    if(gmac_periphID == "U2005"): # SAME54
        gmacheapdependency = [  "TCPIP_GMAC_QUEUE_0","TCPIP_GMAC_RX_EN_QUE0","TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE0","TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE0",
                                "TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE0","TCPIP_GMAC_RX_BUFF_SIZE_QUE0","TCPIP_GMAC_TX_EN_QUE0",
                                "TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE0", "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]
        

    # GMAC TX descriptors Dummy Count
    tcpipGmacTxDescCountDummmy = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY", None)
    tcpipGmacTxDescCountDummmy.setLabel("") 
    tcpipGmacTxDescCountDummmy.setVisible(False)
    tcpipGmacTxDescCountDummmy.setDescription("GMAC TX descriptors Dummy Count")
    tcpipGmacTxDescCountDummmy.setDefaultValue(1)

    # GMAC RX descriptors Dummy Count
    tcpipGmacRxDescCountDummmy = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY", None)
    tcpipGmacRxDescCountDummmy.setLabel("GMAC RX descriptors Dummy Count")  
    tcpipGmacRxDescCountDummmy.setVisible(False)
    tcpipGmacRxDescCountDummmy.setDescription("GMAC RX descriptors Dummy Count")
    tcpipGmacRxDescCountDummmy.setDefaultValue(1)

    # GMAC Rx buffer size dummy
    tcpipGmacRxBuffSizeDummmy = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_BUFF_SIZE_DUMMY", None)
    tcpipGmacRxBuffSizeDummmy.setLabel("GMAC Rx buffer size dummy") 
    tcpipGmacRxBuffSizeDummmy.setVisible(False)
    tcpipGmacRxBuffSizeDummmy.setDescription("GMAC Rx buffer size dummy")
    tcpipGmacRxBuffSizeDummmy.setDefaultValue(64)
    
    # GMAC Tx buffer size dummy
    tcpipGmacTxBuffSizeDummmy = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_TX_BUFF_SIZE_DUMMY", None)
    tcpipGmacTxBuffSizeDummmy.setLabel("GMAC Tx buffer size dummy")  
    tcpipGmacTxBuffSizeDummmy.setVisible(False)
    tcpipGmacTxBuffSizeDummmy.setDescription("GMAC Tx buffer size dummy")
    tcpipGmacTxBuffSizeDummmy.setDefaultValue(64)
    
    # Maximum MAC Supported RX Frame Size
    tcpipGmacRxFrameMax = drvGmacComponent.createIntegerSymbol("TCPIP_GMAC_RX_MAX_FRAME", None)
    tcpipGmacRxFrameMax.setLabel("Maximum MAC Supported RX Frame Size")
    tcpipGmacRxFrameMax.setVisible(True)
    tcpipGmacRxFrameMax.setDescription("Maximum MAC Supported RX Frame Size")
    tcpipGmacRxFrameMax.setDefaultValue(1536)
    
    # MAC Maximum Number of Supported Fragments
    tcpipEmacRxFragNumMax = drvGmacComponent.createIntegerSymbol("TCPIP_EMAC_RX_FRAGMENTS", None)
    tcpipEmacRxFragNumMax.setLabel("MAC Maximum Number of Supported Fragments")
    tcpipEmacRxFragNumMax.setMax(6)
    tcpipEmacRxFragNumMax.setVisible(True)
    tcpipEmacRxFragNumMax.setDescription("MAC Maximum Number of Supported Fragments")
    tcpipEmacRxFragNumMax.setDefaultValue(1)
    
    # Ethernet RX Filters Selection Settings
    tcpipEthRxFilter = drvGmacComponent.createMenuSymbol(None, None) 
    tcpipEthRxFilter.setLabel("Ethernet RX Filters Selection")
    tcpipEthRxFilter.setVisible(True)
    tcpipEthRxFilter.setDescription("Ethernet RX Filters Selection Settings")

    # Accept Broadcast Packets
    tcpipGmacEthFilterBcastAccept = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_BCAST_ACCEPT", tcpipEthRxFilter)
    tcpipGmacEthFilterBcastAccept.setLabel("Accept Broadcast Packets")
    tcpipGmacEthFilterBcastAccept.setVisible(True)
    tcpipGmacEthFilterBcastAccept.setDescription("Accept Broadcast Packets")
    tcpipGmacEthFilterBcastAccept.setDefaultValue(True)
    
    # Accept Multicast Packets
    tcpipGmacEthFilterMcastAccept = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_MCAST_ACCEPT", tcpipEthRxFilter)
    tcpipGmacEthFilterMcastAccept.setLabel("Accept Multicast Packets")
    tcpipGmacEthFilterMcastAccept.setVisible(True)
    tcpipGmacEthFilterMcastAccept.setDescription("Accept Multicast Packets")
    tcpipGmacEthFilterMcastAccept.setDefaultValue(True)
    
    # Accept Unicast Packets
    tcpipGmacEthFilterUcastAccept = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_UCAST_ACCEPT", tcpipEthRxFilter)
    tcpipGmacEthFilterUcastAccept.setLabel("Accept Unicast Packets")
    tcpipGmacEthFilterUcastAccept.setVisible(True)
    tcpipGmacEthFilterUcastAccept.setDescription("Accept Unicast Packets")
    tcpipGmacEthFilterUcastAccept.setDefaultValue(True)
    
    # Accept Multicast Packets matching Hash
    tcpipGmacEthFilterMcastHashAccept = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_MCAST_HASH_ACCEPT", tcpipEthRxFilter)
    tcpipGmacEthFilterMcastHashAccept.setLabel("Accept Multicast Packets matching Hash")
    tcpipGmacEthFilterMcastHashAccept.setVisible(True)
    tcpipGmacEthFilterMcastHashAccept.setDescription("Accept Multicast Packets matching Hash")
    tcpipGmacEthFilterMcastHashAccept.setDefaultValue(False)

    # Accept Unicast Packets matching Hash
    tcpipGmacEthFilterUcastHashAccept = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_UCAST_HASH_ACCEPT", tcpipEthRxFilter)
    tcpipGmacEthFilterUcastHashAccept.setLabel("Accept Unicast Packets matching Hash")
    tcpipGmacEthFilterUcastHashAccept.setVisible(True)
    tcpipGmacEthFilterUcastHashAccept.setDescription("Accept Unicast Packets matching Hash")
    tcpipGmacEthFilterUcastHashAccept.setDefaultValue(False)

    # Reject Packets with Wrong CRC
    tcpipGmacEthFilterCrcErrReject = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_CRC_ERROR_REJECT", tcpipEthRxFilter)
    tcpipGmacEthFilterCrcErrReject.setLabel("Reject Packets with Wrong CRC")
    tcpipGmacEthFilterCrcErrReject.setVisible(True)
    tcpipGmacEthFilterCrcErrReject.setDescription("Reject Packets with Wrong CRC")
    tcpipGmacEthFilterCrcErrReject.setDefaultValue(True)

    # Accept Packets with Wrong CRC
    tcpipGmacEthFilterCrcErrAccept = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_CRC_ERROR_ACCEPT", tcpipEthRxFilter)
    tcpipGmacEthFilterCrcErrAccept.setLabel("Accept Packets with Wrong CRC")
    tcpipGmacEthFilterCrcErrAccept.setVisible(True)
    tcpipGmacEthFilterCrcErrAccept.setDescription("Accept Packets with Wrong CRC")
    tcpipGmacEthFilterCrcErrAccept.setDefaultValue(False)


    # Accept Packets with Maximum Frame Size(1536 bytes)
    tcpipGmacEthFilteFrameAcceptMax = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_MAX_FRAME_ACCEPT", tcpipEthRxFilter)
    tcpipGmacEthFilteFrameAcceptMax.setLabel("Accept Packets with Maximum Frame Size(1536 bytes)")
    tcpipGmacEthFilteFrameAcceptMax.setVisible(True) 
    tcpipGmacEthFilteFrameAcceptMax.setDescription("Accept Packets with Maximum Frame Size(1536 bytes)")
    tcpipGmacEthFilteFrameAcceptMax.setDefaultValue(False)

    # Accept All Packets (Promiscuous Mode)
    tcpipGmacEthFilterAllAccept = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_ALL_ACCEPT", tcpipEthRxFilter)
    tcpipGmacEthFilterAllAccept.setLabel("Accept All Packets (Promiscuous Mode)")
    tcpipGmacEthFilterAllAccept.setVisible(True)
    tcpipGmacEthFilterAllAccept.setDescription("Accept All Packets (Promiscuous Mode)")
    tcpipGmacEthFilterAllAccept.setDefaultValue(False)

    # Accept Packets with Frame Error
    tcpipGmacEthFilterFrameErrAccept = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_FRAME_ERROR_ACCEPT", tcpipEthRxFilter)
    tcpipGmacEthFilterFrameErrAccept.setLabel("Accept Packets with Frame Error")
    tcpipGmacEthFilterFrameErrAccept.setVisible(True)
    tcpipGmacEthFilterFrameErrAccept.setDescription("Accept Packets with Frame Error")
    tcpipGmacEthFilterFrameErrAccept.setDefaultValue(False)

    # Accept Jumbo Packets (upto 10240 bytes)
    tcpipGmacEthFilterJumboFrameAccept = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_FILTER_JUMBO_FRAME_ACCEPT", tcpipEthRxFilter)
    tcpipGmacEthFilterJumboFrameAccept.setLabel("Accept Jumbo Packets (upto 10240 bytes)")
    tcpipGmacEthFilterJumboFrameAccept.setVisible(True)
    tcpipGmacEthFilterJumboFrameAccept.setDescription("Accept Jumbo Packets (upto 10240 bytes)")
    tcpipGmacEthFilterJumboFrameAccept.setDefaultValue(False)


    # Ethernet Connection Flags
    tcpipEthConnFlag = drvGmacComponent.createMenuSymbol(None, None) 
    tcpipEthConnFlag.setLabel("Ethernet Connection Flags")
    tcpipEthConnFlag.setVisible(True)
    tcpipEthConnFlag.setDescription("Ethernet Connection Flags")
    
    # Use Auto Negotiation
    tcpipGmacEthAutoNegotiate = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_AUTO_NEGOTIATION", tcpipEthConnFlag)
    tcpipGmacEthAutoNegotiate.setLabel("Use Auto Negotiation")
    tcpipGmacEthAutoNegotiate.setVisible(True) 
    tcpipGmacEthAutoNegotiate.setDescription("Use Auto Negotiation")
    tcpipGmacEthAutoNegotiate.setDefaultValue(True)
    
    # Use Full Duplex
    tcpipGmacEthFullDuplex = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_FULL_DUPLEX", tcpipEthConnFlag)
    tcpipGmacEthFullDuplex.setLabel("Use Full Duplex")
    tcpipGmacEthFullDuplex.setVisible(True)
    tcpipGmacEthFullDuplex.setDescription("Use Full Duplex")
    tcpipGmacEthFullDuplex.setDefaultValue(True)
    
    # Use Half Duplex
    tcpipGmacEthHalfDuplex = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_HALF_DUPLEX", tcpipEthConnFlag)
    tcpipGmacEthHalfDuplex.setLabel("Use Half Duplex")
    tcpipGmacEthHalfDuplex.setVisible(True)
    tcpipGmacEthHalfDuplex.setDescription("Use Half Duplex")
    tcpipGmacEthHalfDuplex.setDefaultValue(True)
    
    # Use 100MBps
    tcpipGmacEthUse100 = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_100", tcpipEthConnFlag)
    tcpipGmacEthUse100.setLabel("Use 100MBps")
    tcpipGmacEthUse100.setVisible(True) 
    tcpipGmacEthUse100.setDescription("Use 100MBps")
    tcpipGmacEthUse100.setDefaultValue(True)
    
    # Use 10MBps
    tcpipGmacEthUse10 = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_10", tcpipEthConnFlag)
    tcpipGmacEthUse10.setLabel("Use 10MBps")
    tcpipGmacEthUse10.setVisible(True)
    tcpipGmacEthUse10.setDescription("Use 10MBps")
    tcpipGmacEthUse10.setDefaultValue(True)
    
    # Allow Huge Packets
    tcpipGmacEthHugePkt = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_HUGE_PKTS", tcpipEthConnFlag)
    tcpipGmacEthHugePkt.setLabel("Allow Huge Packets")
    tcpipGmacEthHugePkt.setVisible(True)
    tcpipGmacEthHugePkt.setDescription("Allow Huge Packets")
    tcpipGmacEthHugePkt.setDefaultValue(False)
    
    # Loopbacked At The MAC Level
    tcpipGmacEthMacLoopBack = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_MAC_LOOPBACK", tcpipEthConnFlag)
    tcpipGmacEthMacLoopBack.setLabel("Loopbacked At The MAC Level")
    tcpipGmacEthMacLoopBack.setVisible(True) 
    tcpipGmacEthMacLoopBack.setDescription("Loopbacked At The MAC Level")
    tcpipGmacEthMacLoopBack.setDefaultValue(False)
    
    # Loopbacked At The PHY Level
    tcpipGmacEthPhyLoopBack = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_PHY_LOOPBACK", tcpipEthConnFlag)
    tcpipGmacEthPhyLoopBack.setLabel("Loopbacked At The PHY Level")
    tcpipGmacEthPhyLoopBack.setVisible(True)
    tcpipGmacEthPhyLoopBack.setDescription("Loopbacked At The PHY Level")
    tcpipGmacEthPhyLoopBack.setDefaultValue(False)
    
    # Use Auto MDIX
    tcpipGmacEthMdixAuto = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_MDIX_AUTO", tcpipEthConnFlag)
    tcpipGmacEthMdixAuto.setLabel("Use Auto MDIX")
    tcpipGmacEthMdixAuto.setVisible(True) 
    tcpipGmacEthMdixAuto.setDescription("Use Auto MDIX")
    tcpipGmacEthMdixAuto.setDefaultValue(True)
    
    # Use Swapped MDIX
    tcpipGmacEthMdixSwap = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_MDIX_SWAP", tcpipEthConnFlag)
    tcpipGmacEthMdixSwap.setLabel("Use Swapped MDIX")
    tcpipGmacEthMdixSwap.setVisible(False)
    tcpipGmacEthMdixSwap.setDescription("Use Swapped MDIX")
    tcpipGmacEthMdixSwap.setDefaultValue(False)
    tcpipGmacEthMdixSwap.setDependencies(tcpipEthMacMdixSwapVisible, ["TCPIP_GMAC_ETH_OF_MDIX_AUTO"])

    # RMII Connection
    tcpipGmacEthRmii = drvGmacComponent.createBooleanSymbol("TCPIP_GMAC_ETH_OF_RMII", tcpipEthConnFlag)
    tcpipGmacEthRmii.setLabel("RMII Connection")
    tcpipGmacEthRmii.setVisible(True)
    tcpipGmacEthRmii.setDescription("RMII Connection")
    tcpipGmacEthRmii.setDefaultValue(True)

    # GMAC RX Checksum offload Enable
    drvGmacRxChksmOffload = drvGmacComponent.createBooleanSymbol("DRV_GMAC_RX_CHKSM_OFFLOAD", None)
    drvGmacRxChksmOffload.setLabel("Enable Rx Checksum Offload")
    drvGmacRxChksmOffload.setVisible(True)
    drvGmacRxChksmOffload.setDescription("Enable Rx Checksum Offload")
    drvGmacRxChksmOffload.setDefaultValue(False)

    # GMAC TX Checksum offload Enable
    drvGmacTxChksmOffload = drvGmacComponent.createBooleanSymbol("DRV_GMAC_TX_CHKSM_OFFLOAD", None)
    drvGmacTxChksmOffload.setLabel("Enable Tx Checksum Offload")
    drvGmacTxChksmOffload.setVisible(True)
    drvGmacTxChksmOffload.setDescription("Enable Tx Checksum Offload")
    drvGmacTxChksmOffload.setDefaultValue(False)
    
    # GMAC Module ID
    tcpipEmacModuleId = drvGmacComponent.createStringSymbol("TCPIP_INTMAC_MODULE_ID", None)
    tcpipEmacModuleId.setLabel("GMAC Module ID")
    tcpipEmacModuleId.setVisible(True)
    tcpipEmacModuleId.setDescription("GMAC Module ID")
    tcpipEmacModuleId.setDefaultValue("GMAC_BASE_ADDRESS")

    # Driver GMAC Instances Number
    drvGmacInstanceNum = drvGmacComponent.createIntegerSymbol("DRV_GMAC_INSTANCES_NUMBER", None)
    drvGmacInstanceNum.setLabel("GMAC Instances Number")
    drvGmacInstanceNum.setVisible(True)
    drvGmacInstanceNum.setDescription("Driver GMAC Instances Number")
    drvGmacInstanceNum.setDefaultValue(1)
    
    # Driver GMAC Clients Number
    drvGmacClientNum = drvGmacComponent.createIntegerSymbol("DRV_GMAC_CLIENTS_NUMBER", None)
    drvGmacClientNum.setLabel("GMAC Clients Number")
    drvGmacClientNum.setVisible(True)
    drvGmacClientNum.setDescription("Driver GMAC Clients Number")
    drvGmacClientNum.setDefaultValue(1)
    
    # Driver GMAC Clients Number
    drvGmacIndex = drvGmacComponent.createIntegerSymbol("DRV_GMAC_INDEX", None)
    drvGmacIndex.setLabel("GMAC Index Number") 
    drvGmacIndex.setVisible(True)
    drvGmacIndex.setDescription("Driver GMAC Clients Number")
    drvGmacIndex.setDefaultValue(1)
    
    # Driver GMAC Peripheral ID
    drvGmacPeripheralId = drvGmacComponent.createIntegerSymbol("DRV_GMAC_PERIPHERAL_ID", None)
    drvGmacPeripheralId.setLabel("GMAC Peripheral ID") 
    drvGmacPeripheralId.setVisible(True)
    drvGmacPeripheralId.setDescription("Driver GMAC Peripheral ID")
    drvGmacPeripheralId.setDefaultValue(1)
    
    # Driver GMAC Number of queues
    drvGmacQueueNum = drvGmacComponent.createIntegerSymbol("DRV_GMAC_NUMBER_OF_QUEUES", None)
    drvGmacQueueNum.setLabel("GMAC Instances Number")
    drvGmacQueueNum.setVisible(False)
    drvGmacQueueNum.setDescription("Number of GMAC Queues")
    if(gmac_periphID == "11046"): # SAME70, SAMV71
        drvGmacQueueNum.setDefaultValue(6)  
    elif (gmac_periphID == "U2005"): # SAME54
        drvGmacQueueNum.setDefaultValue(1)
    elif (gmac_periphID == "44152"): # SAMA5D2
        drvGmacQueueNum.setDefaultValue(3)
    
    # Driver GMAC RMII Mode Selection Value
    drvGmacRmiiVal = drvGmacComponent.createIntegerSymbol("DRV_GMAC_RMII_VALUE", None)
    drvGmacRmiiVal.setVisible(False)
    if(gmac_periphID == "11046"): # SAME70, SAMV71
        drvGmacRmiiVal.setDefaultValue(0)   
    elif (gmac_periphID == "U2005"): # SAME54
        drvGmacRmiiVal.setDefaultValue(0)
    elif (gmac_periphID == "44152"): # SAMA5D2
        drvGmacRmiiVal.setDefaultValue(1)
        
    # Driver GMAC Interrupt Mode
    drvGmacIntrMode = drvGmacComponent.createBooleanSymbol("DRV_GMAC_INTERRUPT_MODE", None)
    drvGmacIntrMode.setLabel("GMAC Interrupt Mode")
    drvGmacIntrMode.setVisible(True)
    drvGmacIntrMode.setDescription("Driver GMAC Interrupt Mode")
    drvGmacIntrMode.setDefaultValue(True)
    
    interruptVector = "GMAC_INTERRUPT_ENABLE"
    interruptHandler = "GMAC_INTERRUPT_HANDLER"
    interruptHandlerLock = "GMAC_INTERRUPT_HANDLER_LOCK"

    Database.clearSymbolValue("core", interruptVector)
    Database.setSymbolValue("core", interruptVector, True, 2)
    Database.clearSymbolValue("core", interruptHandler)
    Database.setSymbolValue("core", interruptHandler, "GMAC_InterruptHandler", 2)
    Database.clearSymbolValue("core", interruptHandlerLock)
    Database.setSymbolValue("core", interruptHandlerLock, True, 2)

    # NVIC Dynamic settings
    drvGmacinterruptControl = drvGmacComponent.createBooleanSymbol("NVIC_GMAC_ENABLE", None)
    drvGmacinterruptControl.setDependencies(interruptControl, ["DRV_GMAC_INTERRUPT_MODE"])
    drvGmacinterruptControl.setVisible(False)
    
    # Driver GMAC Interrupt Source
    drvGmacIntrSource = drvGmacComponent.createStringSymbol("DRV_GMAC_INTERRUPT_SOURCE", drvGmacIntrMode)
    drvGmacIntrSource.setLabel("GMAC Interrupt Source")
    drvGmacIntrSource.setVisible(False)
    drvGmacIntrSource.setDescription("Driver GMAC Interrupt Source")
    drvGmacIntrSource.setDefaultValue("GMAC_IRQn")
    
    # PHY Connected to GMAC
    drvGmacPhyType = drvGmacComponent.createStringSymbol("DRV_INTMAC_PHY_TYPE", None)
    drvGmacPhyType.setLabel("External PHY Device")
    drvGmacPhyType.setVisible(True)
    drvGmacPhyType.setDescription("PHY Connected to GMAC")
    drvGmacPhyType.setDefaultValue("")
    drvGmacPhyType.setReadOnly(True)
    
    # Driver GMAC Heap Size
    drvGmacHeapSize = drvGmacComponent.createIntegerSymbol("DRV_GMAC_HEAP_SIZE", None)
    drvGmacHeapSize.setLabel("GMAC Heap Size (bytes)") 
    drvGmacHeapSize.setVisible(False)
    drvGmacHeapSize.setDescription("GMAC Heap Size")
    drvGmacHeapSize.setDefaultValue(gmacHeapCalc())
    drvGmacHeapSize.setReadOnly(True)
    drvGmacHeapSize.setDependencies(drvGmacHeapUpdate, gmacheapdependency)
    
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

    if(gmac_periphID == "11046"): # SAME70, SAMV71
        drvGmacLibCFileStem = drvGmacLibCFileStem + "_samE7x_V7x"
    elif (gmac_periphID == "U2005"): # SAME54
        drvGmacLibCFileStem = drvGmacLibCFileStem + "_samE5x"
    elif (gmac_periphID == "44152"): # SAMA5D2
        drvGmacLibCFileStem = drvGmacLibCFileStem + "_samA5D2"
        
    drvGmacLibSourceFile = drvGmacComponent.createFileSymbol(None, None)
    drvGmacLibSourceFile.setSourcePath("driver/gmac/src/dynamic/" + drvGmacLibCFileStem + ".c")
    drvGmacLibSourceFile.setOutputName(drvGmacLibCFileStem + ".c")
    drvGmacLibSourceFile.setOverwrite(True)
    drvGmacLibSourceFile.setDestPath("driver/gmac/src/dynamic/")
    drvGmacLibSourceFile.setProjectPath("config/" + configName + "/driver/gmac/src/dynamic/")
    drvGmacLibSourceFile.setType("SOURCE")
    drvGmacLibSourceFile.setEnabled(True)
    
global interruptVector
global interruptHandler
global interruptHandlerLock
def interruptControl(gmacNVIC, event):
    global interruptVector
    global interruptHandler
    Database.clearSymbolValue("core", interruptVector)
    Database.clearSymbolValue("core", interruptHandler)
    Database.clearSymbolValue("core", interruptHandlerLock)
    if (event["value"] == True):
        Database.setSymbolValue("core", interruptVector, True, 2)
        Database.setSymbolValue("core", interruptHandler, "GMAC_InterruptHandler", 2)
        Database.setSymbolValue("core", interruptHandlerLock, True, 2)
    else :
        Database.setSymbolValue("core", interruptVector, False, 2)
        Database.setSymbolValue("core", interruptHandler, "GMAC_Handler", 2)
        Database.setSymbolValue("core", interruptHandlerLock, False, 2)
        
def tcpipEthMacMenuVisibleSingle(symbol, event):
    if (event["value"] == True):
        print("EthMac Menu Visible.")       
        symbol.setVisible(True)
    else:
        print("EthMac Menu Invisible.")
        symbol.setVisible(False)
        
def tcpipEthMacMdixSwapVisible(symbol, event):
    tcpipEthMacAutoMdix = Database.getSymbolValue("drvGmac","TCPIP_GMAC_ETH_OF_MDIX_AUTO")

    if (event["value"] == True):
        symbol.setVisible(False)
    else:
        symbol.setVisible(True)
    
        
def tcpipGmacGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])

def drvGmacGenHeaderFile(headerFile, event):
    headerFile.setEnabled(event["value"])

def onAttachmentConnected(source, target):
    if (source["id"] == "GMAC_PHY_Dependency"): 
        Database.setSymbolValue("drvGmac", "DRV_INTMAC_PHY_TYPE", target["component"].getDisplayName(),2)
        
def onAttachmentDisconnected(source, target):
    if (source["id"] == "GMAC_PHY_Dependency"): 
        Database.clearSymbolValue("drvGmac", "DRV_INTMAC_PHY_TYPE")

def gmacHeapCalc():
    heapsize = 0
    # Rx Section Heap Calculation
    #Queue 0
    if((Database.getSymbolValue("drvGmac","TCPIP_GMAC_QUEUE_0") == True)): # and (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_EN_QUE0") == True)):
        heapsize = heapsize + (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE0") + Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE0") + Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE0")) * (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_BUFF_SIZE_QUE0") + 128 + 16)
    else:
        heapsize = heapsize + (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY") * (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_BUFF_SIZE_DUMMY") + 128 + 16))
    
    #Queue 1    
    if((Database.getSymbolValue("drvGmac","TCPIP_GMAC_QUEUE_1") == True)): # and (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_EN_QUE1") == True)):
        heapsize = heapsize + (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE1") + Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE1") + Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE1")) * (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_BUFF_SIZE_QUE1") + 128 + 16)
    else:
        heapsize = heapsize + (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY") * (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_BUFF_SIZE_DUMMY") + 128 + 16))
    
    #Queue 2    
    if((Database.getSymbolValue("drvGmac","TCPIP_GMAC_QUEUE_2") == True)): #  and (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_EN_QUE2") == True)):
        heapsize = heapsize + (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE2") + Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE2") + Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE2")) * (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_BUFF_SIZE_QUE2") + 128 + 16)
    else:
        heapsize = heapsize + (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY") * (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_BUFF_SIZE_DUMMY") + 128 + 16))   

    #Queue 3    
    if((Database.getSymbolValue("drvGmac","TCPIP_GMAC_QUEUE_3") == True)): #  and (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_EN_QUE3") == True)):
        heapsize = heapsize + (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE3") + Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE3") + Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE3")) * (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_BUFF_SIZE_QUE3") + 128 + 16)
    else:
        heapsize = heapsize + (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY") * (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_BUFF_SIZE_DUMMY") + 128 + 16)) 

    #Queue 4    
    if((Database.getSymbolValue("drvGmac","TCPIP_GMAC_QUEUE_4") == True)): #  and (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_EN_QUE4") == True)):
        heapsize = heapsize + (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE4") + Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE4") + Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE4")) * (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_BUFF_SIZE_QUE4") + 128 + 16)
    else:
        heapsize = heapsize + (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY") * (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_BUFF_SIZE_DUMMY") + 128 + 16))  
    
    #Queue 5    
    if((Database.getSymbolValue("drvGmac","TCPIP_GMAC_QUEUE_5") == True)): #  and (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_EN_QUE5") == True)):
        heapsize = heapsize + (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE5") + Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUE5") + Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE5")) * (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_BUFF_SIZE_QUE5") + 128 + 16)
    else:
        heapsize = heapsize + (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY") * (Database.getSymbolValue("drvGmac","TCPIP_GMAC_RX_BUFF_SIZE_DUMMY") + 128 + 16))  
        
    # Tx Section Heap Calculation
    #Queue 0
    if((Database.getSymbolValue("drvGmac","TCPIP_GMAC_QUEUE_0") == True)): #  and (Database.getSymbolValue("drvGmac","TCPIP_GMAC_TX_EN_QUE0") == True)):
        heapsize = heapsize + (Database.getSymbolValue("drvGmac","TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE0") * 16)
    else:
        heapsize = heapsize + (Database.getSymbolValue("drvGmac","TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY") * 16)
 
    #Queue 1
    if((Database.getSymbolValue("drvGmac","TCPIP_GMAC_QUEUE_1") == True)): #  and (Database.getSymbolValue("drvGmac","TCPIP_GMAC_TX_EN_QUE1") == True)):
        heapsize = heapsize + (Database.getSymbolValue("drvGmac","TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE1") * 16)
    else:
        heapsize = heapsize + (Database.getSymbolValue("drvGmac","TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY") * 16)

    #Queue 2
    if((Database.getSymbolValue("drvGmac","TCPIP_GMAC_QUEUE_2") == True)): #  and (Database.getSymbolValue("drvGmac","TCPIP_GMAC_TX_EN_QUE2") == True)):
        heapsize = heapsize + (Database.getSymbolValue("drvGmac","TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE2") * 16)
    else:
        heapsize = heapsize + (Database.getSymbolValue("drvGmac","TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY") * 16)

    #Queue 3
    if((Database.getSymbolValue("drvGmac","TCPIP_GMAC_QUEUE_3") == True)): #  and (Database.getSymbolValue("drvGmac","TCPIP_GMAC_TX_EN_QUE3") == True)):
        heapsize = heapsize + (Database.getSymbolValue("drvGmac","TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE3") * 16)
    else:
        heapsize = heapsize + (Database.getSymbolValue("drvGmac","TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY") * 16)

    #Queue 4
    if((Database.getSymbolValue("drvGmac","TCPIP_GMAC_QUEUE_4") == True)): #  and (Database.getSymbolValue("drvGmac","TCPIP_GMAC_TX_EN_QUE4") == True)):
        heapsize = heapsize + (Database.getSymbolValue("drvGmac","TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE4") * 16)
    else:
        heapsize = heapsize + (Database.getSymbolValue("drvGmac","TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY") * 16)

    #Queue 5
    if((Database.getSymbolValue("drvGmac","TCPIP_GMAC_QUEUE_5") == True)): #  and (Database.getSymbolValue("drvGmac","TCPIP_GMAC_TX_EN_QUE5") == True)):
        heapsize = heapsize + (Database.getSymbolValue("drvGmac","TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE5") * 16)
    else:
        heapsize = heapsize + (Database.getSymbolValue("drvGmac","TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY") * 16)        
    
    return heapsize
 
def drvGmacHeapUpdate(symbol, event): 
    heap_size = gmacHeapCalc()
    symbol.setValue(heap_size)
    if(event["id"] == "TCPIP_STACK_HEAP_CALC_MASK"):
        symbol.setVisible(event["value"])

def destroyComponent(drvGmacComponent):
    Database.setSymbolValue("drvGmac", "TCPIP_USE_ETH_MAC", False, 2)