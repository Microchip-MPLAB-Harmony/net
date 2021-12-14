# Internet Management Protocol (IGMP)  Configurations

**IGMP Specific User Configurations**


- **Number of IGMP Interfaces:**
    - The maximum number of interfaces that support IGMP
    - Should be <= interfaces that are present at the stack initialization
    - Default value: 1
    - Symbol TCPIP_IGMP_INTERFACES

- **Number of Multicast Groups:**
    - Maximum number of multicast groups (addresses) for which a IGMP application can subscribe
    - Each group can listen to multiple sources
    - Should be prime number for best performance
    - Default value: 7
    - Symbol TCPIP_IGMP_MCAST_GROUPS


- **Number of Sources in Each Group:**
    - Maximum number of multicast sources per group a IGMP application can listen to
    - Each multicast source can be used by multiple sockets
    - Should be prime number for best performance
    - Note:  the 'all sources' (TCPIP_IGMP_ASM_ALL_SOURCES) may take a slot!
    - Note: Should be <= 32
    - Note: if TCPIP_IGMPV2_SUPPORT_ONLY is enabled then this value is ignored 
    - Default value: 11
    - Symbol TCPIP_IGMP_SOURCES_PER_GROUP

- **Number of Socket Records per Source:**
    - Maximum number of socket records that are part of a IGMP source multicast address for a specific group G
    - Note that same socket may need multiple records if it listens on multiple interfaces!
    - When operating in IGMPv2 style, it represents the total number of sockets that can join/leave a specific group.
    - Default value: 4
    - Symbol TCPIP_IGMP_SOCKET_RECORDS_PER_SOURCE


- **IGMP Task Rate (in msec):**
    - IGMP task processing rate, in milliseconds.
    - The IGMP module will process a timer event with this rate for maintaining its own queues, processing timeouts, etc.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Default value: 33
    - Symbol TCPIP_IGMP_TASK_TICK_RATE

- **Default Robustness Variable Value:**
    - The default Robustness Variable value
    - The Robustness Variable allows tuning for the expected packet loss on a network.
    - If a network is expected to be lossy, the Robustness Variable may be increased.
    - IGMP is robust to (Robustness Variable - 1) packet losses.
    - RFC 3376 IGMPv3: The Robustness Variable MUST NOT be zero, and SHOULD NOT be one. Default: 2
    - This is just a default value. The actual value is taken from a router query.
    - Default value: 2
    - Symbol TCPIP_IGMP_ROBUSTNESS_VARIABLE


- **Default Unsolicited Report Interval (in msec):**
    -  The default Unsolicited Report Interval
    -  The Unsolicited Report Interval is the time between repetitions of a host’s initial report of membership in a group.
    -  RFC 3376 IGMPv3: Default: 1 second.
    - Value is in milliseconds
    - Default value: 1000
    - Symbol TCPIP_IGMP_UNSOLICITED_REPORT_INTERVAL

- **IGMPv2 Support Only:**
    - If this symbol is !0, then only Join and Leave operations are supported, without support for source addresses required by IGMPv3
    - This results in smaller code and less RAM needed by the IGMP module.  
    - Default value: false
    - Symbol TCPIP_IGMPV2_SUPPORT_ONLY

- **Enable User Notification Functions:**
    - Enable/disable user notification functions
    - Default value: false
    - Symbol TCPIP_IGMP_USER_NOTIFICATION


