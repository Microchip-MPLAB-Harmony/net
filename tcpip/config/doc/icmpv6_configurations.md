# Internet Control Message Protocol for IPv6 (ICMPv6)  Configurations

**ICMPv6 Specific User Configurations**

- **Use ICMPv6 Server:**
    - Build the stack with the ICMPv6 Server
    - Default value: true
    - Symbol TCPIP_STACK_USE_ICMPV6_SERVER


- **Use ICMPv6 Client:**
    - Build the stack with the ICMPv6 Client
    - Default value: false
    - Symbol TCPIP_STACK_USE_ICMPV6_CLIENT

- **Enable Console Commands:**
    - Enable the ICMPv6 TCP/IP Console Commands
    - Note that if disabled, the tcpip console ping6 command will not be available
    - Default value: true
    - Symbol: TCPIP_ICMPV6_CLIENT_CONSOLE_CMD



