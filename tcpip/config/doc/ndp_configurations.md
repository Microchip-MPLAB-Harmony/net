# Neighbor Discovery Protocol (NDP) Configurations

**NDP Specific User Configurations**


- **NDP Task Rate (in msec):**
    - NDP task processing rate, in milliseconds.
    - The NDP module will process a timer event with this rate for maintaining its own queues, processing timeouts, etc.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Value is in milliseconds
    - Default value: 32
    - Symbol TCPIP_NDP_TASK_TIMER_RATE, TCPIP_IPV6_NDP_TASK_TIMER_RATE

- **RTR Solicitation Delay (in sec):**
    - Router Solicitation Delay
    - Value is in seconds
    - Default value: 1
    - Symbol TCPIP_MAX_RTR_SOLICITATION_DELAY, TCPIP_IPV6_NDP_MAX_RTR_SOLICITATION_DELAY

- **RTR Solicitation Interval (in sec):**
    - Router Solicitation Interval
    - Value is in seconds
    - Default value: 4
    - Symbol TCPIP_RTR_SOLICITATION_INTERVAL, TCPIP_IPV6_NDP_RTR_SOLICITATION_INTERVAL

- **Maximum RTR Solicitations:**
    - Maximum number of router solicitations
    - Default value: 3
    - Symbol TCPIP_MAX_RTR_SOLICITATIONS, TCPIP_IPV6_NDP_MAX_RTR_SOLICITATIONS

- **Maximum Multicast Solicitations:**
    - Maximum number of multicast solicitations
    - Default value: 3
    - Symbol TCPIP_MAX_MULTICAST_SOLICIT, TCPIP_IPV6_NDP_MAX_MULTICAST_SOLICIT

- **Maximum Unicast Solicitations:**
    - Maximum number of unicast solicitations
    - Default value: 3
    - Symbol TCPIP_MAX_UNICAST_SOLICIT, TCPIP_IPV6_NDP_MAX_UNICAST_SOLICIT

- **Maximum Anycast Delay Time (in sec):**
    - Maximum anycast delay time
    - Value is in seconds
    - Default value: 1
    - Symbol TCPIP_MAX_ANYCAST_DELAY_TIME, TCPIP_IPV6_NDP_MAX_ANYCAST_DELAY_TIME

- **Maximum Neighbor Advertisement Transmissions:**
    - Maximum number of neighbor advertisement transmissions
    - Default value: 3
    - Symbol TCPIP_MAX_NEIGHBOR_ADVERTISEMENT, TCPIP_IPV6_NDP_MAX_NEIGHBOR_ADVERTISEMENT


- **Reachable Time (in sec):**
    - Value for the reachable time
    - Value is in seconds
    - Default value: 30
    - Symbol TCPIP_REACHABLE_TIME, TCPIP_IPV6_NDP_REACHABLE_TIME

- **Re-transmission Timer (in sec):**
    - Value of the retransmission timer
    - Value is in seconds
    - Default value: 1
    - Symbol TCPIP_RETRANS_TIMER, TCPIP_IPV6_NDP_RETRANS_TIMER

- **Delay First Probe Time (in sec):**
    - Value of the delay for the first probe
    - Value is in seconds
    - Default value: 5
    - Symbol TCPIP_DELAY_FIRST_PROBE_TIME, TCPIP_IPV6_NDP_DELAY_FIRST_PROBE_TIME

- **IPv6 MTU Increase Time-out (in sec):**
    - Sets the maximum transmit unit increase timeout
    - Value is in seconds
    - Default value: 600
    - Symbol TCPIP_IPV6_MTU_INCREASE_TIMEOUT

