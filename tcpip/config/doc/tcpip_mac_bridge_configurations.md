# Layer 2 MAC bridge Configurations

**MAC Bridge Specific User Configurations**

- **Enable the MAC Bridge Commands:**
    - Enable the MAC Bridge TCP/IP Console Commands
    - Commands to se the contents of the FDB, to add static entries, etc.
    - Default value: false
    - Symbol: TCPIP_STACK_MAC_BRIDGE_COMMANDS

- **Number of Entries in the FDB:**
    - Number of entries in the filtering database (FDB)
    - Depends on the traffic to be handled, how many hosts on that network
    - For best performance the number of entries should be prime
    - Also, keep in mind that the performance degrades when the load factor gets >= 0.7
    - Adjust accordingly!
    - Notes:
        - the host own interfaces/ports have their MAC addresses automatically added to the FDB
        - The FDB has to be big enough to allow this, otherwise the bridge operation will result in error
        - by default packets with destination for these internal ports
        - are excluded from the bridge forwarding
        - setting can be overridden by the initialization
    - Default value: 17
    - Symbol TCPIP_MAC_BRIDGE_FDB_TABLE_ENTRIES

- **Maximum Number of Ports in the Bridge:**
    - Maximum number of ports supported by the bridge
    - Needed for data allocation, etc.
    - Corresponds to the maximum number of interfaces that are bridged
    - It should normally match the TCPIP_MAC_BRIDGE_CONFIG::bridgeTableSize
    - Maximum supported value is currently 32
    - Default value: 2
    - Symbol TCPIP_MAC_BRIDGE_MAX_PORTS_NO

- **Number of Packets in the Pool:**
    - Number of packets in the pool for run time forwarding
    - This pool is allocated at the bridge initialization time and then replenished when needed
    - if 0, then there is no pool and allocation occurs as needed at run time, each packet being allocated exactly the needed size
    - Adjust based on the network traffic
    - Note: the packet pool is used only for packets that need to be processed by the host too. Otherwise the packets are directly forwarded to the destination ports
    - Default value: 8
    - Symbol TCPIP_MAC_BRIDGE_PACKET_POOL_SIZE

- **Size of the Packets in the Pool:**
    - Size of the packets in the packet pool
    - Note that incoming packets that are larger than this size will be dropped
    - Relevant only when TCPIP_MAC_BRIDGE_PACKET_POOL_SIZE != 0
    - Default value: 1536
    - Symbol TCPIP_MAC_BRIDGE_PACKET_SIZE

- **Number of packets to Replenish the pool:**
    - Number of packets to replenish the pool, when it becomes empty
    - Relevant only when TCPIP_MAC_BRIDGE_PACKET_POOL_SIZE != 0
    - When running out of packets a new allocation of this number of packets will be attempted 
    - Default value: 2
    - Symbol TCPIP_MAC_BRIDGE_PACKET_POOL_REPLENISH

- **Number of Descriptors in the Pool:**
    - Number of descriptors in the pool for run time forwarding
    - This pool is allocated at the bridge initialization time and then replenished when needed
    - Each packet to be forwarded needs an descriptor
    - Could be greater than the TCPIP_MAC_BRIDGE_PACKET_POOL_SIZE 
    - Default value: 16
    - Symbol TCPIP_MAC_BRIDGE_DCPT_POOL_SIZE

- **Number of Descriptors to Replenish the pool:**
    - Number of descriptors to replenish the pool, when it becomes empty
    - Extra allocation of this number of descriptors will be attempted
    - Default value: 4
    - Symbol TCPIP_MAC_BRIDGE_DCPT_POOL_REPLENISH

- **Timeout for an Entry to be Purged:**
    - Timeout for an entry to be purged from the table, in seconds
    - The entry will be removed if it has been inactive for this long
    - Recommended default is 300 seconds
    - Value is in seconds
    - Default value: 300
    - Symbol TCPIP_MAC_BRIDGE_ENTRY_TIMEOUT

- **Maximum Transit Delay:**
    - The maximum transit delay of a packet within the bridge
    - Packet will be dropped if not completely forwarded within this time
    - Maximum suggested value is 4 seconds
    - Value is in seconds
    - Default value: 1
    - Symbol TCPIP_MAC_BRIDGE_MAX_TRANSIT_DELAY

- **Do not Learn Dynamic Addresses:**
    - Do not learn dynamic addresses, use only the static/permanent FDB entries
    - Useful for testing or restricting traffic to few addresses only  
    - Default value: false
    - Symbol TCPIP_MAC_BRIDGE_NO_DYNAMIC_LEARN

- **Forward Traffic Only if Entry Exists:**
    - Forward traffic only if the source or destination address of a packet is in FDB
    - Forwarding is still done using the destination address
    - Useful for testing or restricting traffic to few addresses only  
    - Default value: false
    - Symbol TCPIP_MAC_BRIDGE_FDB_ENTRY_EXISTS

- **Use Interface Names for Initialization:**
    - The initialization bridge table contains the interface names, ASCII strings
    - Otherwise the initialization bridge table contains the interface indexes
    - Default value: false
    - Symbol TCPIP_MAC_BRIDGE_IF_NAME_TABLE


- **Enable Statistics:**
    - Enable the MAC bridge statistics 
    - The bridge module will keep counts of the forwarded or rejected packets, etc.
    - See the TCPIP_MAC_BRIDGE_STAT data structure 
    - Default value: false
    - Symbol TCPIP_MAC_BRIDGE_STATISTICS

- **Enable Event Notify:**
    - Enable the MAC bridge event notification 
    - Allows the registration of an event handler to be notified of bridge events
    - See TCPIP_MAC_BRIDGE_EVENT, TCPIP_MAC_BRIDGE_EVENT_HANDLER 
    - Default value: false
    - Symbol TCPIP_MAC_BRIDGE_EVENT_NOTIFY

- **MAC Bridge Task Rate (in msec):**
    - MAC Bridge task processing rate, in milliseconds.
    - The MAC Bridge module will process a timer event with this rate for maintaining its own queues, processing timeouts, etc.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The default value is around 300 milliseconds.
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Value is in milliseconds
    - Default value: 333
    - Symbol TCPIP_MAC_BRIDGE_TASK_RATE

- **Disable the MAC Bridge Ports Glue:**
    - When the 'glue' is enabled, the bridge host ports/interfaces can be reached from each bridge port
        - i.e. you can ping bridge port j while connected to port i.
    - However, the IP addresses of the 2 ports should be part of the same network for this to work
    - Default value: false
    - Symbol TCPIP_STACK_MAC_BRIDGE_DISABLE_GLUE

- **Enable Dynamic Access:**
    - Enable run time access to the FDB
    - If enabled dynamic FDB access API is enabled
    - Otherwise the API doe not exist
    - Note that this option is expensive because it requires synchronization when accessing the FDB from user threads and can interfere with the packet bridge dispatch process
    - Default value: false
    - Symbol TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS

