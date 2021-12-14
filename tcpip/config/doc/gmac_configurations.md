# SAM MAC (GMAC)  Configurations

**GMAC Specific User Configurations**

- **GMAC Default Configuration for Network Traffic:**
    - Default configuration settings for low/medium/high network traffic
    - Default value: "Medium"
    - Symbol TCPIP_GMAC_TRAFFIC

- **GMAC Queue n Tx Enable:**
    - Enable this GMAC TX Queue
    - Default value: true
    - Symbol TCPIP_GMAC_TX_EN_QUEn

- **GMAC Queue n Rx Enable:**
    - Enable this GMAC RX Queue
    - Default value: true
    - Symbol TCPIP_GMAC_RX_EN_QUEn

- **Number of Tx Descriptors:**
    - Number of the TX descriptors to be created for this queue.
    - The amount of memory needed per descriptor is not high so when high GMAC TX performance is needed make sure that this number is >= 8.
    - Default value: 8
    - Symbol TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUEn

- **Max Tx Packet size supported:**
    - Maximum size of a TX packet for this queue
    - Note: Transmit packet size for this queue should be less than this limit!
    - Default value: 1536
    - Symbol TCPIP_GMAC_MAX_TX_PKT_SIZE_QUEn

- **Number of Rx Descriptors:**
    - Number of the RX descriptors to be created for this queue
    - Default value: 8
    - Symbol TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUEn

- **Number of Dedicated MAC RX Buffers:**
    - Number of GMAC dedicated RX packet buffers for this queue.
    - These buffers are always owned by the GMAC.
    - Note that the GMAC driver allocates these buffers for storing the incoming network packets.
    - The bigger the storage capacity, the higher data throughput can be obtained.
    - Note that these packet buffers are allocated from the private TCP/IP heap that is specified by the TCPIP_STACK_DRAM_SIZE setting.
    - Default value: 8
    - Symbol TCPIP_GMAC_RX_DEDICATED_BUFFERS_QUEn

- **Size of RX Buffer (multiple of 64):**
    - Size of a RX packet buffer for this queue. Should be multiple of 64.
    - The size should be enough to accommodate any network received packet.
    - If the packets are larger, they will have to take multiple RX buffers and the packet manipulation is less efficient.
    - Default value: 1536
    - Symbol TCPIP_GMAC_RX_BUFF_SIZE_QUEn

- **Number of additional Rx buffers:**
    - Number of additional (sticky) buffers for this queue 
    - Buffers allocated at the GMAC driver initialization.
    - Default value: 2
    - Symbol TCPIP_GMAC_RX_ADDL_BUFF_COUNT_QUEn

- **Minimum Threshold for Rx Buffer replenish process:**
    - Minumum threshold for the buffer replenish process for this queue.
    - Whenever the number of RX scheduled buffers is <= than this threshold the GMAC driver will allocate new non-dedicated buffers (meaning that they will be released to the TCP/IP heap once they are processed).
    - Setting this value to 0 disables the buffer replenishing process.
    - Default value: 1
    - Symbol TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUEn

- **Rx Buffer allocate count during  replenish process:**
    - Number of RX buffers to allocate when below threshold condition is detected for this queue.
    - If 0, the GMAC driver will allocate (scheduled buffers - rxThres)
    - If !0, the GMAC driver will allocate exactly TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUEn buffers
    - Default value: 2
    - Symbol TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUEn

- **Ethernet RX Filters Selection:**
    - GMAC RX Filters
    - These filters define the packets that are accepted and rejected by the GMAC driver
    - Adjust to your needs
    - The default value allows the processing of unicast, multicast and broadcast packets that have a valid CRC
    - Default value: none
    - Symbol TCPIP_GMAC_RX_FILTERS

- **Accept Broadcast Packets:**
    - Allow the receiving of broadcast packets
    - Default value: true
    - Symbol TCPIP_GMAC_ETH_FILTER_BCAST_ACCEPT, TCPIP_GMAC_RX_FILTERS

- **Accept Multicast Packets:**
    - Allow the receiving of multicast packets
    - Default value: true
    - Symbol TCPIP_GMAC_ETH_FILTER_MCAST_ACCEPT, TCPIP_GMAC_RX_FILTERS

- **Accept Unicast Packets:**
    - Allow the receiving of unicast packets
    - Default value: true
    - Symbol TCPIP_GMAC_ETH_FILTER_UCAST_ACCEPT, TCPIP_GMAC_RX_FILTERS

- **Accept All Packets (Promiscuous Mode):**
    - Allow the receiving of not me unicast packets (promiscuous mode)
    - Default value: false
    - Symbol TCPIP_GMAC_ETH_FILTER_ALL_ACCEPT, TCPIP_GMAC_RX_FILTERS

- **Use Auto Negotiation:**
    - Use auto-negotiation to select the link speed and duplex
    - Default value: true
    - Symbol TCPIP_GMAC_ETH_OF_AUTO_NEGOTIATION

- **Use Full Duplex:**
    - Use/Advertise full-duplex on the link
    - Default value: true
    - Symbol TCPIP_GMAC_ETH_OF_FULL_DUPLEX

- **Use Half Duplex:**
    - Use/Advertise half-duplex on the link
    - Default value: true
    - Symbol TCPIP_GMAC_ETH_OF_HALF_DUPLEX

- **Use 100MBps:**
    - Use/Advertise 100 Mbps on the link
    - Default value: true
    - Symbol TCPIP_GMAC_ETH_OF_100

- **Use 10MBps:**
    - Use/Advertise 10 Mbps on the link
    - Default value: true
    - Symbol TCPIP_GMAC_ETH_OF_10

- **Use Auto MDIX:**
    - Use Auto MDIX on the link
    - Default value: true
    - Symbol TCPIP_GMAC_ETH_OF_MDIX_AUTO

- **Use Swapped MDIX:**
    - Swap the MDIX wires
    - Default value: false
    - Symbol TCPIP_GMAC_ETH_OF_MDIX_SWAP

- **RMII Connection:**
    - Use RMII  GMAC <-> PHY connection (otherwise MII)
    - Default value: true
    - Symbol TCPIP_GMAC_ETH_OF_RMII

- **GMAC Priority Queues:**
    - Number of GMAC priority queues
    - Default value: processor dependant
    - Symbol TCPIP_GMAC_PRIORITY_QUEUES

- **Number of Tx Priority Queues Enabled:**
    - Current number of Tx Priority Queues Enabled 
    - Default value: 1
    - Symbol TCPIP_GMAC_TX_PRIO_COUNT

- **Number of Rx Priority Queues Enabled:**
    - Current number of Rx Priority Queues Enabled 
    - Default value: 1
    - Symbol TCPIP_GMAC_RX_PRIO_COUNT

- **Accept Multicast Packets matching Hash:**
    - Enable GMAC multicast Hash RX filter
    - Default value: false
    - Symbol TCPIP_GMAC_ETH_FILTER_MCAST_HASH_ACCEPT

- **Accept Unicast Packets matching Hash:**
    - Enable GMAC unicast Hash RX filter
    - Default value: false
    - Symbol TCPIP_GMAC_ETH_FILTER_UCAST_HASH_ACCEPT

- **Reject Packets with Wrong CRC:**
    - The GMAC RX should reject packets with wrong CRC
    - Default value: true
    - Symbol TCPIP_GMAC_ETH_FILTER_CRC_ERROR_REJECT, TCPIP_GMAC_RX_FILTERS

- **Accept Packets with Wrong CRC:**
    - The GMAC RX should Accept packets with wrong CRC
    - Default value: false
    - Symbol TCPIP_GMAC_ETH_FILTER_CRC_ERROR_ACCEPT, TCPIP_GMAC_RX_FILTERS

- **Accept Packets with Maximum Frame Size(1536 bytes):**
    - The GMAC should accept maximum size frame packets        
    - Default value: false
    - Symbol TCPIP_GMAC_ETH_FILTER_MAX_FRAME_ACCEPT

- **Accept Packets with Frame Error:**
    - The GMAC should accept packets with frame error
    - Default value: false
    - Symbol TCPIP_GMAC_ETH_FILTER_FRAME_ERROR_ACCEPT

- **Accept Jumbo Packets (upto 10240 bytes):**
    - Allow the transfer of huge/jumbo packets
    - Default value: false
    - Symbol TCPIP_GMAC_ETH_FILTER_JUMBO_FRAME_ACCEPT

- **Advanced Rx Queue Filters:**
    - Enable GMAC Advanced Rx Queue Filters
    - Default value: false
    - Symbol TCPIP_GMAC_RX_QUE_FILTER_EN


- **Allow Huge Packets:**
    - Allow the transfer of huge/jumbo packets
    - Default value: false
    - Symbol TCPIP_GMAC_ETH_OF_HUGE_PKTS

- **Loopbacked At The MAC Level:**
    - Enable GMAC loopback
    - Default value: false
    - Symbol TCPIP_GMAC_ETH_OF_MAC_LOOPBACK

- **Loopbacked At The PHY Level:**
    - Enable PHY loopback
    - Default value: false
    - Symbol TCPIP_GMAC_ETH_OF_PHY_LOOPBACK

- **Enable Rx Checksum Offload:**
    - Enable RX checksum calculation by GMAC
    - Default value: false
    - Symbol DRV_GMAC_RX_CHKSM_OFFLOAD

- **Enable Tx Checksum Offload:**
    - Enable TX checksum calculation by GMAC
    - Default value: false
    - Symbol DRV_GMAC_TX_CHKSM_OFFLOAD

- **RX Frame Maximum Size:**
    - Maximum GMAC supported RX frame size.
    - The default value is 1536 (allows for VLAN tagged frames, although the VLAN tagged frames are discarded on RX).
    - On RX: any incoming ETH frame that is longer than this size will be discarded.
    - On TX: any frame that is longer than this size will be aborted by the GMAC.
    - Normally there is no need to touch this value unless you know exactly the maximum size of the frames you want to process on your network.
    - Notes:
        - Always multiple of 16.
    - Default value: 1536
    - Symbol TCPIP_GMAC_RX_MAX_FRAME

- **Maximum Number of Fragments:**
    - GMAC maximum number of supported RX fragments.
    - Note that excessive fragmentation leads to performance degradation.
    - The default and recommended value should be 1.
    - Default value: 1
    - Symbol TCPIP_EMAC_RX_FRAGMENTS

- **GMAC Module ID:**
    - The ID of the GMAC module
    - Default value: "GMAC_BASE_ADDRESS"
    - Symbol TCPIP_INTMAC_MODULE_ID

- **External PHY Connected to MAC:**
    - The external PHY connected to the GMAC device
    - Default value: "Not Connected"
    - Symbol DRV_INTMAC_PHY_TYPE

- **Non-Cacheable Memory Region for GMAC Descriptors:**
    - Create a non-cacheable memory region in data memory
    - Default value: true
    - Symbol DRV_GMAC_NO_CACHE_CONFIG

- **Non-Cacheable Memory Size:**
    - The size of the region to be created
    - Default value: 32 B - 32 KB 
    - Symbol DRV_GMAC_NOCACHE_REGION_SIZE

- **Start Address of Non-Cacheable Memory:**
    - The start address of the non cacheable region
    - Note: Start Address must be aligned to Memory Size
    - Default value: 0x2045F000 (SAME54, E70, etc.)
    - Symbol DRV_GMAC_NOCACHE_MEM_ADDRESS

- **GMAC Heap Size (bytes):**
    - Estimation of the heap size taken by the GMAC driver
    - Default value: none
    - Symbol DRV_GMAC_HEAP_SIZE


