# Internet Protocol (IP) Version 6  Configurations

**IPv6 Specific User Configurations**


- **IPv6 Task Rate (in msec):**
    - IPv6 task processing rate, in milliseconds.
    - The IPv6 module will process a timer event with this rate for maintaining its own queues, processing timeouts, etc.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Value is in milliseconds
    - Default value: 1000
    - Symbol TCPIP_IPV6_TASK_PROCESS_RATE

- **IPv6 Init Task Processing Rate (in msec):**
    - IPv6 initialize task processing rate
    - Value is in milliseconds
    - Default value: 32
    - Symbol TCPIP_IPV6_INIT_TASK_PROCESS_RATE


- **Minimum Allocation Unit For The Payload:**
    - Sets the minimum allocation unit for the payload size
    - Default value: 64
    - Symbol TCPIP_IPV6_DEFAULT_ALLOCATION_BLOCK_SIZE

- **Lower Bounds Of The Maximum Transmission Unit:**
    - Sets the lower bounds of the Maximum Transmission Unit
    - Default value: 1280
    - Symbol TCPIP_IPV6_MINIMUM_LINK_MTU

- **Maximum Transmission Unit:**
    - Default Maximum Transmission Unit on the link
    - Default value: 1500
    - Symbol TCPIP_IPV6_DEFAULT_LINK_MTU

- **IPv6 Default Hop Limit:**
    - IPv6 Time-to-Live parameter
    - Default value: 64
    - Symbol TCPIP_IPV6_DEFAULT_CUR_HOP_LIMIT

- **Router Advertisement Reachable Time (in sec):**
    - Router advertisement reachable time
    - Value is in seconds
    - Default value: 30
    - Symbol TCPIP_IPV6_DEFAULT_BASE_REACHABLE_TIME

- **Router Advertisement Retransmission Time (in msec):**
    - Process the router advertisement retransmission time
    - Value is in milliseconds
    - Default value: 1000
    - Symbol TCPIP_IPV6_DEFAULT_RETRANSMIT_TIME

- **Maximum Number of Queued Packets per Remote:**
    - This option defines the maximum number of queued packets per remote.
    - If an additional packet needs to be queued, the oldest packet in the queue will be removed.
    - Default value: 1
    - Symbol TCPIP_IPV6_QUEUE_NEIGHBOR_PACKET_LIMIT


- **Stale Neighbor Discovery Packet Time-out (in sec):**
    - Time-out of stale neighbor discovery packets.
    - 0 will cause packets to persist indefinitely.
    - Value is in seconds
    - Default value: 600
    - Symbol TCPIP_IPV6_NEIGHBOR_CACHE_ENTRY_STALE_TIMEOUT

- **Maximum Number of Multicast Queued IPv6:**
    - This option defines the maximum number of multicast queued IPv6
    - If an additional packet is queued, the oldest packet in the queue will be removed.
    - Default value: 4
    - Symbol TCPIP_IPV6_QUEUE_MCAST_PACKET_LIMIT

- **Multicast Packet Time-out (in sec):**
    - This option defines the number of seconds an IPv6 multicast packet will remain in the queue before being timed out
    - Value is in seconds
    - Default value: 10
    - Symbol TCPIP_IPV6_QUEUED_MCAST_PACKET_TIMEOUT

- **Enable ULA Generation:**
    - Enable the IPv6 ULA address generation
    - SNTP protocol needs to be enabled
    - Default value: false
    - Symbol TCPIP_IPV6_ULA_GENERATE_ENABLE

- **NTP Access Time-out For The IPv6 ULA Address Generation (in msec):**
    - NTP access time-out for the IPv6 ULA address generation
    - Value is in milliseconds
    - Default value: 12000
    - Symbol TCPIP_IPV6_ULA_NTP_ACCESS_TMO

- **NTP Time Stamp Validity Window (in msec):**
    - The NTP time stamp validity window
    - If a stamp was obtained outside this interval from the moment of the request a new request will be issued
    - Value is in milliseconds
    - Default value: 1000
    - Symbol TCPIP_IPV6_ULA_NTP_VALID_WINDOW

- **Fragmentation Packet Time-out (in sec):**
    - Fragmentation packet time-out value.
    - Value is in seconds
    - Default value: 60
    - Symbol TCPIP_IPV6_FRAGMENT_PKT_TIMEOUT

- **Maximum RX Fragmented Buffer Size:**
    - RX fragmented buffer size should be equal to the total original packet size of  ICMPv6 ECHO request packets .
    - For ex:
        -Transmit ICMPv6 Echo request packet whose original packet size is 1500 bytes from the Global address of HOST1 to the global address of HOST2 and if the packet is going to be fragmented then packet will be broken more than packets.
        - Each packet will have IPv6 header (40 bytes) + Fragmentation header (8 bytes) + ICMPv6 Echo request header(8 bytes) + Payload (data packet).
        - PING6 (1500 = 40+8+8+1452 bytes). Here data packet size is 1452.
        - If the data packet size is getting changed then this following macro should be rectified to get proper ICMPv6 ECHO response.
    - This is the Maximum RX fragmented buffer size.
    - Default value: 1514
    - Symbol TCPIP_IPV6_RX_FRAGMENTED_BUFFER_SIZE

- **Enable External Packet Processing:**
    - This setting enables/disables the processing of the RX packets by an external handler
    - The user of the IPv6 can register an external function to process the incoming packets
    - If true, the functionality is built in and could be used by the application
    - If false, the functionality does not exist and the generated code is slightly smaller 
    - Default value: false
    - Symbol TCPIP_IPV6_EXTERN_PACKET_PROCESS


