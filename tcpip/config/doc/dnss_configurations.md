# Domain Name System Server (DNSS)  Configurations

**DNSS Specific User Configurations**

- **Maximum Number of IPv4 Cache Entries:**
    - Maximum and default number of IPv4 entries will be allowed to be configured from command prompt.
    - These many entries will be allowed to be sent in response for the DNS query with record type TCPIP_DNS_TYPE_A.
    - Default value: 2
    - Symbol TCPIP_DNSS_CACHE_PER_IPV4_ADDRESS

- **Maximum Number of IPv6 Cache Entries:**
    - Maximum and default number of IPv6 entries will be allowed to be configured from command prompt.
    - Tthese many entries will be allowed to be sent in response for the DNS query with record type TCPIP_DNS_TYPE_AAAA.
    - Default value: 1
    - Symbol TCPIP_DNSS_CACHE_PER_IPV6_ADDRESS

- **Default DNS Host Name Length:**
    - Default DNS host name length 
    - Default value: 64
    - Symbol TCPIP_DNSS_HOST_NAME_LEN

- **Enable Console Commands:**
    - Enable the DNSS TCP/IP Console Commands
    - Default value: false
    - Symbol: TCPIP_DNSS_CONSOLE_CMD

- **DNS Server Task Rate (in msec):**
    - DNSS task processing rate, in milliseconds.
    - The DNSS module will process a timer event with this rate  for maintaining its own queues, processing timeouts, etc.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Default value: 33
    - Symbol TCPIP_DNSS_TASK_PROCESS_RATE

- **Default DNS Server Time-To-Live(in sec):**
    - Default TTL time for a IP address
    - Value is in seconds
    - Default value: (10 * 60)
    - Symbol TCPIP_DNSS_TTL_TIME


- **Delete Old Cache if Still in Place:**
    - On initialization, delete the old cache if still in place
    - Else do not re-initialize
    - Default value: true
    - Symbol TCPIP_DNSS_DELETE_OLD_LEASE


- **Reply to Client Request with Board Info:**
    - Reply DNS info with Board info only if the requested DNS host name is not present
    - if TCPIP_DNSS_REPLY_BOARD_ADDR != true, then return no such name
    - Default value: true
    - Symbol TCPIP_DNSS_REPLY_BOARD_ADDR


