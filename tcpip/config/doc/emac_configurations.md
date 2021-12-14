# SAM9x60 EMAC (EMAC)  Configurations

**EMAC Specific User Configurations**

- **MAC Base Address:**
    - Selected EMAC peripheral base address
    - Default value: EMACx_BASE_ADDRESS
    - Symbol MAC_BASE_ADDRESS

- **Default Network MAC Driver:**
    - Name of the MAC driver
    - Default value: "DRV_EMACx_Object"
    - Symbol DRV_MAC_CODE

- **Interrupt Source:**
    - The EMAC peripheral interrupt source
    - Default value: "EMACn_IRQn"
    - Symbol MAC_INTERRUPT_SOURCE

- **MAC Default Configuration for Network Traffic:**
    - Default configuration settings for low/medium/high network traffic
    - Default value: "Medium"
    - Symbol TCPIP_EMAC_TRAFFIC

- **TX descriptor count:**
    - Number of the TX descriptors to be created for this queue.
    - Default value: 80
    - Symbol TX_DESCRIPTOR_COUNT_n

- **TX buffer size:**
    - Size of the TX buffer for this queue
    - Note: should be a multiple of 16
    - Default value: 1536/64
    - Symbol TX_BUFFER_SIZE_n

- **RX buffer size:**
    - Size of a RX packet buffer for this queue. 
    - Note: Should be multiple of 16.
    - Default value: 128
    - Symbol RX_BUFFER_SIZE_n

- **RX descriptor count:**
    - Number of the RX descriptors to be created for this queue
    - Acceptable values: 12 to 1024
    - Default value: 100
    - Symbol RX_DESCRIPTOR_COUNT_n

- **RX static buffers:**
    - Number of static RX packet buffers for this queue.
    - These buffers are always owned by the EMAC.
    - Default value: 0
    - Symbol RX_STATIC_BUFFERS_n

- **RX dynamic allocation threshold:**
    - Minumum threshold for the buffer replenish process for this queue.
    - Whenever the number of RX scheduled buffers is <= than this threshold the EMAC driver will allocate new non-dedicated buffers (meaning that they will be released to the TCP/IP heap once they are processed).
    - Setting this value to 0 disables the buffer replenishing process.
    - Default value: 15
    - Symbol RX_BUFFER_ALLOCATION_THRESHOLD_n

- **RX dynamic buffer allocation:**
    - Number of buffers allocated dynamically for this queue 
    - Default value: 30
    - Symbol RX_BUFFER_ALLOCATION_n
    
- **RX Link Filters:**
    - EMAC RX Filters
    - These filters define the packets that are accepted and rejected by the EMAC driver
    - Adjust to your needs
    - The default value allows the processing of unicast, multicast and broadcast packets that have a valid CRC
    - Default value: none
    - Symbol DRV_EMACn_RX_FILTERS

- **Accept Broadcast Packets:**
    - Allow the receiving of broadcast packets
    - Default value: true
    - Symbol LINK_ACCEPT_BCAST, DRV_EMACn_RX_FILTERS

- **Accept Multicast Packets:**
    - Allow the receiving of multicast packets
    - Default value: true
    - Symbol LINK_ACCEPT_MCAST, DRV_EMACn_RX_FILTERS

- **Accept Unicast Packets:**
    - Allow the receiving of unicast packets
    - Default value: true
    - Symbol LINK_ACCEPT_UCAST, DRV_EMACn_RX_FILTERS

- **Accept Multicast Packets matching Hash:**
    - Enable EMAC multicast Hash RX filter
    - Default value: false
    - Symbol LINK_ACCEPT_MCAST_HASH, DRV_EMACn_RX_FILTERS

- **Accept Unicast Packets matching Hash:**
    - Enable EMAC unicast Hash RX filter
    - Default value: false
    - Symbol LINK_ACCEPT_UCAST_HASH, DRV_EMACn_RX_FILTERS

- **Reject Packets with Wrong CRC:**
    - The EMAC RX should reject packets with wrong CRC
    - Default value: true
    - Symbol LINK_REJECT_CRC_ERROR, DRV_EMACn_RX_FILTERS

- **Accept Packets with Wrong CRC:**
    - The EMAC RX should Accept packets with wrong CRC
    - Default value: false
    - Symbol LINK_ACCEPT_CRC_ERROR, DRV_EMACn_RX_FILTERS

- **Accept maximum frame size packets:**
    - The EMAC should accept maximum size frame packets        
    - Default value: false
    - Symbol LINK_ACCEPT_MAX_FRAME_SIZE_PACKETS,DRV_EMACn_RX_FILTERS 

- **Accept Packets with Frame Error:**
    - The EMAC should accept packets with frame error
    - Default value: false
    - Symbol LINK_ACCEPT_FRAME_ERROR, DRV_EMACn_RX_FILTERS

- **Accept Jumbo Packets:**
    - The EMAC should accept jumbo packets
    - Default value: false
    - Symbol LINK_ACCEPT_JUMBO_FRAME, DRV_EMACn_RX_FILTERS

- **Promiscuous mode enable:**
    - Allow the receiving of not me unicast packets (promiscuous mode)
    - Default value: false
    - Symbol LINK_ACCEPT_ALL, DRV_EMACn_RX_FILTERS

- **Auto negotiation enable:**
    - Use auto-negotiation to select the link speed and duplex
    - Default value: true
    - Symbol LINK_AUTO_NEGOTIATION

- **Allow full duplex:**
    - Use full-duplex on the link
    - Default value: true
    - Symbol LINK_AUTO_FULL_DUPLEX

- **Allow half duplex:**
    - Use half-duplex on the link
    - Default value: true
    - Symbol LINK_AUTO_HALF_DUPLEX

- **Allow 100MBps:**
    - Use 100 Mbps on the link
    - Default value: true
    - Symbol LINK_AUTO_100

- **Allow 10MBps:**
    - Use 10 Mbps on the link
    - Default value: true
    - Symbol LINK_AUTO_10

- **Manual link type:**
    - Allow manual selection of the link parameters
    - Select one of:
        - Full Duplex, 100 Mbps
        - Full Duplex, 10  Mbps
        - Half Duplex, 100 Mbps
        - Half Duplex, 10 Mbps
    - Default value: "Full Duplex, 100 Mbps"
    - Symbol LINK_MANUAL_CHOICE

- **Allow Huge Packets:**
    - Allow the transfer of huge/jumbo packets
    - Default value: false
    - Symbol ALLOW_HUGE_PACKETS

- **MDIX auto enable:**
    - Use Auto MDIX on the link
    - Default value: true
    - Symbol MDIX_AUTO_ENABLE

- **MDIX swapped:**
    - Swap the MDIX wires
    - Default value: false
    - Symbol MDIX_SWAP

- **MAC level loop back:**
    - Enable EMAC loopback
    - Default value: false
    - Symbol MAC_LOOPBACK

- **PHY level loop back:**
    - Enable PHY loopback
    - Default value: false
    - Symbol PHY_LOOPBACK
    
- **Use RMII:**
    - Use RMII  EMAC <-> PHY connection (otherwise MII)
    - Default value: true
    - Symbol USE_RMII

- **External PHY Device:**
    - The external PHY connected to the EMAC device
    - Default value: ""
    - Symbol DRV_MAC_PHY_TYPE

- **EMAC Heap Size (bytes):**
    - Estimation of the heap size taken by the EMAC driver
    - Default value: none
    - Symbol DRV_EMAC_HEAP_SIZE

