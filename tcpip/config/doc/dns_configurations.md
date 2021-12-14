# Domain Name Service (DNS)  Configurations

**DNS Specific User Configurations**

- **Number of DNS Cache Entries:**
    - Number of DNS resolver entries 
    - Default value: 5
    - Symbol TCPIP_DNS_CLIENT_CACHE_ENTRIES

- **DNS Connection Type:**
    - This parameter can be used to choose the type of IP connection for the DNS client: IPv4 or IPv6. 
    - Currently only IPv4 is supported and this parameter is not used.
    - Reserved for future development 
    - Default value: IP_ADDRESS_TYPE_IPV4
    - Symbol TCPIP_DNS_CLIENT_ADDRESS_TYPE

- **DNS Host Name Maximum Length:**
    - Max DNS host name size
    - Use an appropriate value, depending on the names that need to be solved
    - Default value: 64
    - Symbol TCPIP_DNS_CLIENT_MAX_HOSTNAME_LEN

- **Enable Console Commands:**
    - Enable the DNS TCP/IP Console Commands
    - Default value: false
    - Symbol: TCPIP_DNS_CLIENT_CONSOLE_CMD


- **DNS Client Task Rate (in msec):**
    - DNS task processing rate, in milliseconds.
    - The DNS module will process a timer event with this rate  for maintaining its own queues, processing timeouts, etc.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Default value: 200
    - Symbol TCPIP_DNS_CLIENT_TASK_PROCESS_RATE

- **Default Cache Entry Timeout (in sec):**
    - DNS client cache entry time-out.
    - If this symbol is zero then the entry time-out will be the one specified by the DNS server when the name was solved.
    - Otherwise this value will be used as the cache entry time-out.
    - Default value: 0
    - Symbol TCPIP_DNS_CLIENT_CACHE_ENTRY_TMO

- **No Server Reply Timeout(in sec):**
    - When the DNS Client connected to the DNS Server this is the elapsed time after which an the communication is considered to have timed failed if there was no reply from the server
    - Value is in seconds
    - Default value: (1 * 60)
    - Symbol TCPIP_DNS_CLIENT_SERVER_TMO


- **Maximum Number of IPv4 Answers to be Considered:**
    - Maximum and default number of IPv4 answers to be considered while processing DNS response from server for a query.
    - Default value: 5
    - Symbol TCPIP_DNS_CLIENT_CACHE_PER_IPV4_ADDRESS

- **Maximum Number of IPv6 Answers to be Considered:**
    - Maximum and default number of IPv6 answers to be considered while processing DNS response from server for a query.
    - Default value: 1
    - Symbol TCPIP_DNS_CLIENT_CACHE_PER_IPV6_ADDRESS

- **Maximum Number of Interfaces Considered for DNS resolution:**
    - Max number of interfaces to take part in the DNS selection algorithm
    - Should be always greater than 1:
        - the default interface should always be considered for DNS resolution
    - Depending on how many active interfaces, select those to be considered for DNS resolution 
    - Default value: 4
    - Symbol TCPIP_DNS_CLIENT_MAX_SELECT_INTERFACES

- **Default TTL time (in sec):**
    - Default TTL time  for a solved entry in the cache
    - This value will be used when the DNS server TTL value for an entry is 0
    - Default value: (20 * 60)
    - Symbol TCPIP_DNS_CLIENT_CACHE_DEFAULT_TTL_VAL

- **Retry Unsolved Cache Entry Timeout (in sec):**
    - Retry lookup timeout for a unsolved entry in the cache
    - If the TCPIP_DNS_CLIENT_LOOKUP_RETRY_TMO seconds elapsed and the name has not been solved then the resolution will be retried.
    - Note: currently the stack will make 2 retries per interface using both DNS1 and DNS2 to solve the name
    - Value is in seconds
    - Default value: 2
    - Symbol TCPIP_DNS_CLIENT_LOOKUP_RETRY_TMO

- **Delete Old Entries:**
    - Delete the existent entries in the cache at the stack/interface restart
    - Default value: true
    - Symbol TCPIP_DNS_CLIENT_DELETE_OLD_ENTRIES

- **Enable User Notification:**
    - Allow DNS client user notification
    - If enabled, the TCPIP_DNS_HandlerRegister/TCPIP_DNS_HandlerDeRegister functions exist and can be used
    - Default value: false
    - Symbol TCPIP_DNS_CLIENT_USER_NOTIFICATION

