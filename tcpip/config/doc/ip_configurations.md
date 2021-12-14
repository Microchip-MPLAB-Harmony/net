# Internet Protocol (IP) Configurations

**IPv4 Specific User Configurations**

	
- **Enable Console Commands:**
    - Enable the IPv4 TCP/IP Console Commands
    - Default value: false
    - Symbol: TCPIP_IPV4_COMMANDS_ENABLE, TCPIP_IPV4_COMMANDS



- **Number of ARP Requests IPv4 can queue:**
    - The number of entries that IPv4 can queue up for ARP resolution.
    - Usually it should be <= the number of total ARP cache entries for all interfaces
    - Default value: 10
    - Symbol TCPIP_IPV4_ARP_QUEUE_SLOTS, TCPIP_IPV4_ARP_SLOTS

- **Enable IPv4 Fragmentation Support:**
    - If 1, enable IPv4 fragmentation processing, RX + TX
    - If 0 (default), IPv4 fragmentation not supported
    - Default value: false
    - Symbol TCPIP_IPV4_FRAGMENTATION


- **Initial fragment timeout (in sec):**
    - Initial fragment timeout
    - Timer Lower Bound (RFC 791)
    - The RFC states this value to be 15 seconds
    - Value is in seconds
    - Default value: 15
    - Symbol TCPIP_IPV4_FRAGMENT_TIMEOUT

- **Maximum number of fragmented streams:**
    - Upper limit for the number of fragmented streams that could be handled simultaneously
    - Default value: 3
    - Symbol TCPIP_IPV4_FRAGMENT_MAX_STREAMS

- **Maximum number of fragments:**
    - Upper limit for the number of fragments in a fragmented stream
    - This also imposes a limit on how large a packet could be depending on the MTU and fragmentations that occur
    - If more fragments received for a certain stream, the stream will be discarded
    - Default value: 4
    - Symbol TCPIP_IPV4_FRAGMENT_MAX_NUMBER

- **Enable External Packet Processing:**
    - This setting enables/disables the processing of the RX packets by an external handler
    - The user of the IPv4 can register an external function to process the incoming packets
    - If true, the functionality is built in and could be used by the application
    - If false, the functionality does not exist and the generated code is slightly smaller 
    - Default value: false
    - Symbol TCPIP_IPV4_EXTERN_PACKET_PROCESS


- **IPv4 Fragmentation Task Rate (in msec):**
    - IPv4 task processing rate, in milliseconds.
    - The IPv4 module will process a timer event with this rate for maintaining its own queues, processing timeouts, etc.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The default value is around 30 milliseconds.
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Value is in milliseconds
    - Default value: 37
    - Symbol TCPIP_IPV4_TASK_TICK_RATE

- **Enable IPv4 Forwarding Support:**
    - Builds the IPv4 module with forwarding support
    - Default value: false
    - Symbol TCPIP_IPV4_FORWARD, TCPIP_IPV4_FORWARDING_ENABLE

- **Support Forwarding Table with ASCII strings:**
    - The initialization forwarding table is in ASCII format, using strings
    - Default value: true
    - Symbol TCPIP_IPV4_FORWARD_ASCII_ENABLE, TCPIP_IPV4_FORWARDING_TABLE_ASCII

- **Support Dynamic Forwarding Table Manipulation:**
    - Enables run time API for forwarding tables manipulation
    - Certain API functions (TCPIP_IPV4_FwdTableAddAscii, TCPIP_IPV4_FwdTableRemoveAscii, ...) exists only if they are enabled
    - Default value: true
    - Symbol TCPIP_IPV4_FORWARD_DYN_ENABLE, TCPIP_IPV4_FORWARDING_DYNAMIC_API


- **Enable Broadcast Traffic Forwarding:**
    - Enable the forwarding of the broadcast packets
    - The default should be disabled
    - Default value: false
    - Symbol TCPIP_IPV4_FORWARD_BCAST_ENABLE


- **Enable Forwarding Statistics:**
    - Enable the IPv4 module to keep statistics counts
    - Default value: false
    - Symbol TCPIP_IPV4_FORWARD_STATS_ENABLE, TCPIP_IPV4_FORWARDING_STATS

- **Forwarding TX Queue Slots:**
    - The maximum number of broadcast and multicast packets that can be queued waiting for the output interface to transmit them.
    - This applies only for packets that have to be both forwarded and processed internally which is only broadcast or multicast
    - Adjust depending on your traffic
    - Note that if this limit is exceeded, the packets will not be forwarded but still processed internally
    - If 0, packets will not be forwarded, just processed internally.
    - Default value: 0
    - Symbol TCPIP_IPV4_FWD_QUEUE_SLOTS

- **Forwarding Table Maximum Size:**
    - The maximum number of entries in the forwarding table for each interface
    - Default value: 10
    - Symbol TCPIP_IPV4_FORWARD_TABLE_MAX_SIZE, TCPIP_IPV4_FORWARDING_TABLE_MAX_SIZE

- **Use ASCII Init Forwarding Table:**
    - Initialization table built with ASCII strings
    - Default value: true
    - Symbol TCPIP_IPV4_FORWARD_ASCII_SELECT

- **Number of Entries in the Init Forwarding table:**
    - Number of entries in the routing table as part of the initialization
    - Default value: none
    - Symbol TCPIP_IPV4_FORWARD_TABLE_CNT, TCPIP_IPV4_FORWARDING_TABLE_ENTRIES


- **Network Destination Address:**
    - Network destination address.
    - This address is used for matching the IPv4 packet destination address. For ex: "192.168.2.0"
    - Default value: "0.0.0.0" 
    - Symbol TCPIP_IPV4_FWD_ENTRY_NET_ADD_IDX

- **Network Mask:**
    - Associated mask for this destination address. For ex: "255.255.255.0"
    - The value needs to be a valid network mask, with contiguous ones in its binary representation followed by contiguous zeros
    - Default value: "0.0.0.0" 
    - Symbol TCPIP_IPV4_FWD_ENTRY_NET_MASK_IDX


- **Gateway Destination Address:**
    - Gateway destination address. For ex: "192.168.2.1"
    - Used as packets destination */
    - Default value: "0.0.0.0" 
    - Symbol TCPIP_IPV4_FWD_ENTRY_GW_ADD_IDX

- **Source Interface Name:**
    - The name of the input interface that this entry applies to.
    - The forwarding tables are built per interface. For ex: "eth0", "wlan0", etc.
    - Default value: ""
    - Symbol TCPIP_IPV4_FWD_ENTRY_SRC_IF_NAME_IDX

- **Destination Interface Name:**
    - The name of the interface to go out on if this entry is selected.  For ex: "eth0", "wlan0", etc.
    - Default value: ""
    - Symbol TCPIP_IPV4_FWD_ENTRY_DEST_IF_NAME_IDX

- **Entry Metric Value:**
    - The path efficiency. The smaller the value, the higher priority for the path.
    - Note that this value is useful only when there are multiple entries in the forwarding table that have the same number of mask bits set
    - Default value: 10
    - Symbol TCPIP_IPV4_FWD_ENTRY_METRIC_IDX

