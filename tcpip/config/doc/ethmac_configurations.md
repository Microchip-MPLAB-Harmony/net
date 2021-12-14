# PIC32M MAC (ETHMAC)  Configurations

**ETHMAC Specific User Configurations**

- **MAC Default Configuration for Network Traffic:**
    - Default configuration settings for low/medium/high network traffic
    - Default value: "Medium"
    - Symbol TCPIP_EMAC_TRAFFIC

- **Number of Tx Descriptors to be created:**
    - Number of the TX descriptors to be created.
    - Because a TCP packet can span at most 3 buffers, the value should always be >= 4
    - The amount of memory needed per descriptor is not high (around 24 bytes) so when high MAC TX performance is needed make sure that this number is >= 8.
    - Default value: 8
    - Symbol TCPIP_EMAC_TX_DESCRIPTORS

- **Number of Rx Descriptors to be created:**
    - Number of the RX descriptors to be created.
    - If not using the run time replenish mechanism (see below) it should match the number of dedicated buffers: TCPIP_EMAC_RX_DEDICATED_BUFFERS;
    - Otherwise it should be bigger than the sum of dedicated + non-dedicated buffers:
        - TCPIP_EMAC_RX_DESCRIPTORS > TCPIP_EMAC_RX_DEDICATED_BUFFERS + replenish_buffers
    - Default value: 8
    - Symbol TCPIP_EMAC_RX_DESCRIPTORS

- **Number of Dedicated MAC RX Buffers:**
    - Number of MAC dedicated RX packet buffers.
    - These buffers are always owned by the MAC.
    - Note that the MAC driver allocates these buffers for storing the incoming network packets.
    - The bigger the storage capacity, the higher data throughput can be obtained.
    - Note that these packet buffers are allocated from the private TCP/IP heap that is specified by the TCPIP_STACK_DRAM_SIZE setting.
    - Default value: 4
    - Symbol TCPIP_EMAC_RX_DEDICATED_BUFFERS

- **Number of Non_Dedicated RX Buffers:**
    - Number of non-dedicated buffers for the MAC initialization
    - Buffers allocated at the MAC driver initialization.
    - Default value: 0
    - Symbol TCPIP_EMAC_RX_INIT_BUFFERS

- **Minimum Threshold for the Buffer Replenish Process:**
    - Minumum threshold for the buffer replenish process.
    - Whenever the number of RX scheduled buffers is <= than this threshold the MAC driver will allocate new non-dedicated buffers (meaning that they will be released to the TCP/IP heap once they are processed).
    - Setting this value to 0 disables the buffer replenishing process.
    - Default value: 1
    - Symbol TCPIP_EMAC_RX_LOW_THRESHOLD

- **Rx Buffer Allocate Count during  Replenish Process:**
    - Number of RX buffers to allocate when below threshold condition is detected.
    - If 0, the MAC driver will allocate (scheduled buffers - rxThres)
    - If !0, the MAC driver will allocate exactly TCPIP_EMAC_RX_LOW_FILL buffers
    - Default value: 2
    - Symbol TCPIP_EMAC_RX_LOW_FILL

- **Size of a RX Buffer. Should Be Multiple of 16:**
    - Size of a RX packet buffer. Should be multiple of 16.
    - This is the size of all receive packet buffers processed by the ETHC.
    - The size should be enough to accommodate any network received packet.
    - If the packets are larger, they will have to take multiple RX buffers and the packet manipulation is less efficient.
    - Note: Together with TCPIP_EMAC_RX_DEDICATED_BUFFERS it has impact on TCPIP_STACK_DRAM_SIZE setting.
    - Default value: 1536
    - Symbol TCPIP_EMAC_RX_BUFF_SIZE

- **Ethernet RX Filters Selection:**
    - MAC RX Filters
    - These filters define the packets that are accepted and rejected by the MAC driver
    - Adjust to your needs
    - The default value allows the processing of unicast, multicast and broadcast packets that have a valid CRC
    - Default value: none
    - Symbol TCPIP_EMAC_RX_FILTERS

- **Accept Broadcast Packets:**
    - Allow the receiving of broadcast packets
    - Default value: true
    - Symbol TCPIP_EMAC_ETH_FILTER_BCAST_ACCEPT, TCPIP_EMAC_RX_FILTERS

- **Accept Multicast Packets:**
    - Allow the receiving of multicast packets
    - Default value: true
    - Symbol TCPIP_EMAC_ETH_FILTER_MCAST_ACCEPT, TCPIP_EMAC_RX_FILTERS

- **Accept Unicast Packets:**
    - Allow the receiving of unicast packets
    - Default value: true
    - Symbol TCPIP_EMAC_ETH_FILTER_UCAST_ACCEPT, TCPIP_EMAC_RX_FILTERS

- **Accept Not Me Unicast Packets:**
    - Allow the receiving of not me unicast packets (promiscuous mode)
    - Default value: true
    - Symbol TCPIP_EMAC_ETH_FILTER_UCAST_OTHER_ACCEPT, TCPIP_EMAC_RX_FILTERS

- **Reject Runt Packets (< 64 bytes):**
    - Reject small (runt) packets
    - Default value: true
    - Symbol TCPIP_EMAC_ETH_FILTER_RUNT_REJECT, TCPIP_EMAC_RX_FILTERS

- **Accept Runt Packets (< 64 bytes):**
    - Accept small (runt) packets
    - Default value: false
    - Symbol TCPIP_EMAC_ETH_FILTER_RUNT_ACCEPT, TCPIP_EMAC_RX_FILTERS

- **Reject Packets with Wrong CRC:**
    - The MAC RX should reject packets with wrong CRC
    - Default value: true
    - Symbol TCPIP_EMAC_ETH_FILTER_CRC_ERROR_REJECT, TCPIP_EMAC_RX_FILTERS

- **Accept Packets with Wrong CRC:**
    - The MAC RX should Accept packets with wrong CRC
    - Default value: false
    - Symbol TCPIP_EMAC_ETH_FILTER_CRC_ERROR_ACCEPT, TCPIP_EMAC_RX_FILTERS

- **Use Auto Negotiation:**
    - Use auto-negotiation to select the link speed and duplex
    - Default value: true
    - Symbol TCPIP_EMAC_ETH_OF_AUTO_NEGOTIATION

- **Use Full Duplex:**
    - Use/Advertise full-duplex on the link
    - Default value: true
    - Symbol TCPIP_EMAC_ETH_OF_FULL_DUPLEX

- **Use Half Duplex:**
    - Use/Advertise half-duplex on the link
    - Default value: true
    - Symbol TCPIP_EMAC_ETH_OF_HALF_DUPLEX

- **Use 100MBps:**
    - Use/Advertise 100 Mbps on the link
    - Default value: true
    - Symbol TCPIP_EMAC_ETH_OF_100

- **Use 10MBps:**
    - Use/Advertise 10 Mbps on the link
    - Default value: true
    - Symbol TCPIP_EMAC_ETH_OF_10

- **Allow Huge Packets:**
    - Allow the transfer of huge/jumbo packets
    - Default value: false
    - Symbol TCPIP_EMAC_ETH_OF_HUGE_PKTS

- **Loopbacked At The MAC Level:**
    - Enable MAC loopback
    - Default value: false
    - Symbol TCPIP_EMAC_ETH_OF_MAC_LOOPBACK

- **Loopbacked At The PHY Level:**
    - Enable PHY loopback
    - Default value: false
    - Symbol TCPIP_EMAC_ETH_OF_PHY_LOOPBACK

- **Use Auto MDIX:**
    - Use Auto MDIX on the link
    - Default value: true
    - Symbol TCPIP_EMAC_ETH_OF_MDIX_AUTO

- **Use Swapped MDIX:**
    - Swap the MDIX wires
    - Default value: false
    - Symbol TCPIP_EMAC_ETH_OF_MDIX_SWAP

- **RMII Connection:**
    - Use RMII  MAC <-> PHY connection (otherwise MII)
    - Default value: true
    - Symbol TCPIP_EMAC_ETH_OF_RMII

- **EMAC Module ID:**
    - The ID of the MAC module
    - Default value: "_ETH_BASE_ADDRESS"
    - Symbol TCPIP_EMAC_MODULE_ID

- **Enable Interrupt?:**
    - Enable the MAC/Ethernet Controller interrupt
    - Default value: true
    - Symbol INTERRUPT_ENABLE

- **Maximum Frame Size - Longer Frames Will Be Discarded:**
    - Maximum MAC supported RX/TX frame size.
    - The default value is 1536 (allows for VLAN tagged frames, although the VLAN tagged frames are discarded on RX).
    - On RX: any incoming ETH frame that is longer than this size will be discarded.
    - On TX: any frame that is longer than this size will be aborted by the MAC.
    - Normally there is no need to touch this value unless you know exactly the maximum size of the frames you want to process on your network.
    - Notes:
        - It can be used to control RX packets fragmentation (together with the TCPIP_EMAC_RX_BUFF_SIZE).
        - Always multiple of 16.
    - Default value: 1536
    - Symbol TCPIP_EMAC_MAX_FRAME

- **Link Maximum Transmission Unit - (576 - 1500):**
    - Link Maximum Transmission Unit (MTU)
    - This symbol defines the largest network protocol data unit that can be transmitted over this link in a single frame.
    - It relates to the TCPIP_EMAC_MAX_FRAME value - see above.
    - The default value for an Ethernet link should be 1500.
    - The minimum value for an Eternet link should be 576.
    - If need to change this, make sure that the 
    - TCPIP_EMAC_MAX_FRAME >= TCPIP_EMAC_LINK_MTU + 18
    - where the value 18 represents the Ethernet frame header:
        - 12 bytes    - destination + source address
        - 2 bytes     - frame type
        - 4 bytes     - FCS
    - Note: setting this value to 0, the maximum possible MTU will be used
    - Default value: 1500
    - Symbol TCPIP_EMAC_LINK_MTU

- **MAC Maximum Number of Supported RX Fragments:**
    - MAC maximum number of supported RX fragments.
    - Based on the values of TCPIP_EMAC_MAX_FRAME and TCPIP_EMAC_RX_BUFF_SIZE an incoming frame may span multiple RX buffers (fragments).
    - Note that excessive fragmentation leads to performance degradation.
    - The default and recommended value should be 1.
    - Alternatively you can use the calculation of the number of fragments based on the selected RX sizes:
        - TCPIP_EMAC_RX_FRAGMENTS = ((TCPIP_EMAC_MAX_FRAME + (TCPIP_EMAC_RX_BUFF_SIZE -1 )) / (TCPIP_EMAC_RX_BUFF_SIZE))
    - Default value: 1
    - Symbol TCPIP_EMAC_RX_FRAGMENTS

- **Auto Flow Control Enable:**
    - Enable MAC Auto Flow Control
    - This symbol enables/disables the auto flow control
    - When in full-duplex mode and the auto flow control is enabled, the MAC will send pause frames whenever the number of pending RX packets reached the full watermark.
    - Once this number drops to the empty watermark, a pause frame with pause value of 0 is transmitted, resuming the normal traffic
    - Default value: true
    - Symbol TCPIP_EMAC_AUTO_FLOW_CONTROL_ENABLE

- **Pause Time Value for the Auto Flow Control, bytes:**
    - This represents the number of bytes to request the pause for when the auto flow control kicks in.
    - It is normally thought of in terms of RX packets: pauseBytes / rxBuffSize = pausePackets
    - Using bytes instead of packets allows better granularity 
    - The value should be a multiple of 64 bytes 
    - The range is : 64 bytes <= TCPIP_EMAC_FLOW_CONTROL_PAUSE_BYTES <=  (4 MB - 64) bytes (0x3fffc0) 
    - An usual value is 2 full packets. For example 2 * 1536 bytes 
    - Used only when flow control is enabled 
    - Default value: 3072
    - Symbol TCPIP_EMAC_FLOW_CONTROL_PAUSE_BYTES

- **The Full Water Mark, in Number of Packets:**
    - An 8 bit value representing the full water mark, in number of packets 
    - When the number of pending received packets reaches this threshold the auto flow control kicks in 
    - A good rule to use for avoiding any packet overflow is to have enough receive room for at least 2 maximum packets, i.e. 
    - fullWMMark = (rxScheduledBuffers x rxBuffSize) / TCPIP_EMAC_MAX_FRAME - (2 x TCPIP_EMAC_MAX_FRAME ) / rxBuffSize; 
    - Always TCPIP_EMAC_FLOW_CONTROL_FULL_WMARK > TCPIP_EMAC_FLOW_CONTROL_EMPTY_WMARK 
    - Used only when auto flow control is enabled 
    - Adjust to your own value 
    - Default value should use the calculation of the watermark based on the selected RX sizes and buffers:
        - TCPIP_EMAC_FLOW_CONTROL_FULL_WMARK = ((TCPIP_EMAC_RX_DEDICATED_BUFFERS * TCPIP_EMAC_RX_BUFF_SIZE) / TCPIP_EMAC_MAX_FRAME - (2 x TCPIP_EMAC_MAX_FRAME ) / TCPIP_EMAC_RX_BUFF_SIZE)
    - Default value: 2
    - Symbol TCPIP_EMAC_FLOW_CONTROL_FULL_WMARK

- **The Empty Water Mark, in Number of Packets:**
    - An 8 bit value representing the empty water mark, in number of packets 
    - When the auto flow control is active and the number of pending received packets falls reaching this threshold the auto flow control pause frames will stop 
    - Always TCPIP_EMAC_FLOW_CONTROL_FULL_WMARK > TCPIP_EMAC_FLOW_CONTROL_EMPTY_WMARK 
    - Used only when auto flow control is enabled 
    - Default value could be 0: resume normal traffic when there is no pending RX packet 
    - Default value: 0
    - Symbol TCPIP_EMAC_FLOW_CONTROL_EMPTY_WMARK

- **External PHY Device:**
    - The external PHY connected to the MAC device
    - Default value: ""
    - Symbol DRV_INTMAC_PHY_TYPE

- **ETHMAC Heap Size (bytes):**
    - Estimation of the heap size taken by the MAC driver
    - Default value: none
    - Symbol DRV_ETHMAC_HEAP_SIZE

