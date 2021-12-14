# IPv6 Dynamic Host Configuration Protocol (DCHPv6)  Configurations

**DCHPv6 Specific User Configurations**

- **Select DHCP Unique Identifier Type:**
    - Default DUID type to be used by the client
    - Default value:TCPIP_DHCPV6_DUID_TYPE_LL 
    - Symbol TCPIP_DHCPV6_CLIENT_DUID_TYPE, TCPIP_DHCPV6_DUID_TYPE_CONFIG


- **Minimum UDP Tx Buffer Size (in bytes):**
    - Minimum size of the associated UDP buffer
    - Default value: 512
    - Symbol TCPIP_DHCPV6_MIN_UDP_TX_BUFFER_SIZE

- **Maximum Message Buffer Size:**
    - Default value of the buffer to assemble messages, etc.
    - Default value: 512
    - Symbol TCPIP_DHCPV6_MESSAGE_BUFFER_SIZE

- **Maximum Number of Message Buffers:**
    - Default number of buffers for processing messages
    - These buffers are used for the TX/RX operations
    - Enough buffers need to be allocated for gathering server advertisements and being able to transmit messages
    - Default value: 8
    - Symbol TCPIP_DHCPV6_MESSAGE_BUFFERS
number of message buffers to allocate for this client

- **Enable Console Commands:**
    - Enable the DCHPv6 TCP/IP Console Commands
    - Default value: true
    - Symbol: TCPIP_DHCPV6_CLIENT_CONSOLE_CMD

- **Enable DHCPv6 Client:**
    - Enable the DHCPv6 at stack start up
    - Default value: true
    - Symbol TCPIP_DHCPV6_FLAG_STACK_START_ENABLE

- **Skip DAD Processing:**
    - Disable the DAD processing for DHCP generated addresses
    - Use only for testing or in special cases
    - Default value: false
    - Symbol TCPIP_DHCPV6_SKIP_DAD_PROCESS, TCPIP_DHCPV6_FLAG_DAD_DISABLE

- **Ignore Renew Life Time:**
    - If enabled, the IA (and its associated address) renew process will be valid as dictated by t1/defaultIataT1 and its address preferred lifetime will be ignored
    - If disabled, the IA and its address will attempt renew when the minimum of address preferred lifetime and t1/defaultIataT1 expired
    - Default value: false
    - Symbol TCPIP_DHCPV6_FLAG_IA_IGNORE_RENEW_LTIME
   
- **Ignore Rebind Life Time:**
    - If enabled, the IA (and its associated address) rebind process will be valid as dictated by t2/defaultIataT2 and its address valid lifetime will be ignored
    - If disabled, the IA and its address will attempt rebind when the minimum of address valid lifetime and t2/defaultIataT2 expired
    - Default value: false
    - Symbol TCPIP_DHCPV6_FLAG_IA_IGNORE_REBIND_LTIME
   
- **IA Notification Substate Change:**
    - If enabled, the IA notifications will be generated for IA substate changes too (finer grain)
    - If disabled, notifications will be generated for IA state changes only
    - Default value: false
    - Symbol TCPIP_DHCPV6_FLAG_IA_NOTIFY_SUB_STATE

- **Maximum IANA Descriptors:**
    - Maximum number of IANA descriptors per client
    - Default value: 4
    - Symbol TCPIP_DHCPV6_IANA_DESCRIPTORS_NO

- **T1 Time Interval(in secs):**
    - Default value for IANA T1
    - The default time at which the client contacts the server to extend the lifetimes of the assigned IA_NA addresses
    - If the IANA t1 value received from the server is 0, then this value will be used to override
    - A value of 0 means the t1 is infinite
    - Value is in seconds
    - Default value: 0
    - Symbol TCPIP_DHCPV6_IANA_DEFAULT_T1

- **T2 Time Interval (in secs):**
    - Default value for IANA T2
    - The default time at which the client contacts any available server to extend the lifetimes of the assigned IA_NA addresses
    - If the IANA t2 value received from the server is 0, then this value will be used to override
    - If !0 it should be > defaultIanaT1!
    - A value of 0 means the t2 is infinite
    - Value is in seconds
    - Default value: 0
    - Symbol TCPIP_DHCPV6_IANA_DEFAULT_T2

- **Solicit T1  Time Interval(in secs):**
    - The default T1 time to solicit from the server
    - Value is in seconds
    - Default value: 0
    - Symbol TCPIP_DHCPV6_IANA_SOLICIT_T1

- **Solicit T2  Time Interval(in secs):**
    - The default T2 time to solicit from the server
    - Value is in seconds
    - Default value: 0
    - Symbol TCPIP_DHCPV6_IANA_SOLICIT_T2

- **Maximum IANA Solicit address:**
    - Default addresses for IANA in a solicit message
    - Default value: 0
    - Symbol TCPIP_DHCPV6_IANA_SOLICIT_ADDRESSES_NO

- **IANA Solicit address:**
    - Default values for the IANA Solicit addresses
    - Irrelevant if TCPIP_DHCPV6_IANA_SOLICIT_ADDRESSES_NO == 0
    - Should be a value for each TCPIP_DHCPV6_IANA_SOLICIT_ADDRESSES_NO 
    - Default value: "::0"
    - Symbol TCPIP_DHCPV6_IANA_SOLICIT_DEFAULT_ADDRESS

- **Maximum IATA Descriptors:**
    - Maximum number of IATA descriptors per client
    - Default value: 2
    - Symbol TCPIP_DHCPV6_IATA_DESCRIPTORS_NO

- **T1 Time Interval(in secs):**
    - Default values for IATA T1
    - The default time at which the client contacts the server to extend the lifetimes of the assigned IATA addresses
    - If 0, the timeout will be infinite (0xffffffff)
    - Value is in seconds
    - Default value: 0
    - Symbol TCPIP_DHCPV6_IATA_DEFAULT_T1

- **T2 Time Interval(in secs):**
    - Default values for IATA T2
    - The default time at which the client contacts any available server to extend the lifetimes of the assigned IA_TA addresses
    - if !0 it should be > defaultIataT1!
    - If 0, the timeout will be infinite (0xffffffff)
    - Value is in seconds
    - Default value: 0
    - Symbol TCPIP_DHCPV6_IATA_DEFAULT_T2

- **Maximum IATA Solicit address:**
    - Default addresses for IATA in a solicit message
    - Default value: 0
    - Symbol TCPIP_DHCPV6_IATA_SOLICIT_ADDRESSES_NO


- **IATA Solicit address:**
    - Default values for the IANA Solicit addresses
    - Irrelevant if TCPIP_DHCPV6_IATA_SOLICIT_ADDRESSES_NO == 0
    - Should be a value for each TCPIP_DHCPV6_IATA_SOLICIT_ADDRESSES_NO 
    - Default value: 0
    - Symbol TCPIP_DHCPV6_IATA_SOLICIT_DEFAULT_ADDRESS

- **Maximum IA Free Descriptors:**
    - Maximum number of free IA descriptors per client
    - Default value: 2
    - Symbol TCPIP_DHCPV6_IA_FREE_DESCRIPTORS_NO


- **IA Preferred Time Interval(in secs):**
    - Default lifetimes for the solicited addresses
    - Value is in seconds
    - Default value: 0
    - Symbol TCPIP_DHCPV6_IA_SOLICIT_ADDRESS_PREF_LTIME

- **IA valid Time Interval (in secs):**
    - Default lifetimes for the solicited addresses
    - Value is in seconds
    - Default value: 0
    - Symbol TCPIP_DHCPV6_IA_SOLICIT_ADDRESS_VALID_LTIME

- **Maximum Length for Server Status message:**
    - Number of character to reserve for a server status code associated message
    - If 0, no message will be saved for status reporting
    - Default value: 0
    - Symbol TCPIP_DHCPV6_STATUS_CODE_MESSAGE_LEN


- **Maximum Number of DNS Server Stored:**
    - Number of DNS servers to store from a DHCP server reply
    - Default value: 2
    - Symbol TCPIP_DHCPV6_DNS_SERVERS_NO

- **Maximum Size Domain Search List:**
    - Space for the Domain Search List option - multiple of 16
    - Default value: 64
    - Symbol TCPIP_DHCPV6_DOMAIN_SEARCH_LIST_SIZE

- **Maximum Prefered Server Selection:**
    - Preference value that forces the server selection
    - 8 bit value!
    - Default value: 255
    - Symbol TCPIP_DHCPV6_FORCED_SERVER_PREFERENCE


- **DCHPv6 Task Rate (in msec):**
    - DCHPv6 task processing rate, in milliseconds.
    - The DCHPv6 module will process a timer event with this rate for maintaining its own queues, processing timeouts, etc.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Value is in milliseconds
    - Default value: 100
    - Symbol TCPIP_DHCPV6_TASK_TICK_RATE


- **Client UDP Connection Port:**
    - UDP port on which clients listen for DHCP messages
    - Default value: 546
    - Symbol TCPIP_DHCPV6_CLIENT_PORT, TCPIP_DHCPV6_CLIENT_CONNECT_PORT

- **Remote Server UDP Listening Port:**
    - Remote server port for DHCPv6 server messages
    - Default value: 547
    - Symbol TCPIP_DHCPV6_SERVER_LISTEN_PORT

- **Enable User Notification:**
    - Allow DHCPV6 client user notification
    - If enabled then functions TCPIP_DHCPV6_HandlerRegister() and TCPIP_DHCPV6_HandlerDeRegister() exist and can be used
    - Default value: false
    - Symbol TCPIP_DHCPV6_USER_NOTIFICATION

- **Debug Level Configuration Flags:**
    - Set debugging capabilities
    - Default value: false
    - Symbol TCPIP_DHCPV6_CONFIG_DEBUG_LEVEL


- **Enable Basic Debugging Level:**
- **Report Debug IN Messages:**
- **Report Debug OUT Messages:**
- **State Change Messages:**
- **Report On Statistics:**
- **Report Client State On User Notification:**
- **Report IA State:**
- **Report IA Sub State:**
- **Report On Additional State:**
- **Report Static List Details:**
- **Report On Buffer Trace:**
    - Enable various debug levels
    - Symbol TCPIP_DHCPV6_DEBUG_MASK_BASIC, TCPIP_DHCPV6_DEBUG_MASK_IN, TCPIP_DHCPV6_DEBUG_MASK_OUT, TCPIP_DHCPV6_DEBUG_MASK_CLIENT_STATE
    - Symbol TCPIP_DHCPV6_DEBUG_MASK_CLIENT_STATS, TCPIP_DHCPV6_DEBUG_MASK_CLIENT_NOTIFY_STATE, TCPIP_DHCPV6_DEBUG_MASK_IA_STATE, TCPIP_DHCPV6_DEBUG_MASK_IA_SUBSTATE
    - Symbol TCPIP_DHCPV6_DEBUG_MASK_ADD_STATE, TCPIP_DHCPV6_DEBUG_MASK_LISTS, TCPIP_DHCPV6_DEBUG_MASK_BUFF_TRACE

