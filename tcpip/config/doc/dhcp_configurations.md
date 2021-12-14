# Dynamic Host Configuration Protocol (DCHP) Configurations

**DCHP Client Specific User Configurations**


- **DHCP Request Time-out (in sec):**
    - Defines how long to wait before a DHCP lease is acquired when the DHCP module is enabled
    - Value is in seconds
    - Default value: 10
    - Symbol TCPIP_DHCP_TIMEOUT

- **DHCP Host Name Maximum Size:**
    - Maximum size of a host name to be advertised to the DHCP server
    - Default value: 20
    - Symbol TCPIP_DHCP_HOST_NAME_SIZE

- **Enable Console Commands:**
    - Enable the DHCP TCP/IP Console Commands
    - Default value: false
    - Symbol: TCPIP_DHCP_CLIENT_CONSOLE_CMD


- **DHCP Client Task Rate (in msec):**
    - The DHCP task processing rate: number of milliseconds to generate an DHCP tick.
    - Used by the DHCP state machine
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Default value: 200
    - Symbol TCPIP_DHCP_TASK_TICK_RATE

- **Client Port for DHCP Client Transactions:**
    - Local UDP port number for the DHCP Client Transactions
    - Default value: 68
    - Symbol TCPIP_DHCP_CLIENT_CONNECT_PORT


- **Remote Server Port for DHCP Server Messages:**
    - Remote UDP port number for the DHCP Client Transactions
    - Default value: 67
    - Symbol TCPIP_DHCP_SERVER_LISTEN_PORT

- **Enable the time server option:**
    - Enables the request of the time server option
    - Default value: 0
    - Symbol TCPIP_DHCP_USE_OPTION_TIME_SERVER

- **Maximum Number of Time Servers to Store:**
    - Max number of time server addresses to store
    - Should be less than 256!
    - Only if TCPIP_DHCP_USE_OPTION_TIME_SERVER is enabled
    - Default value: 1
    - Symbol TCPIP_DHCP_TIME_SERVER_ADDRESSES, TCPIP_DHCP_OPTION_TIME_SERVER_MAX 

- **Enable the NTP server option:**
    - Enables the request of the NTP server option
    - Default value: 0
    - Symbol TCPIP_DHCP_USE_OPTION_NTP_SERVER


- **Maximum Number of NTP Servers to Store:**
    - Max number of NTP server addresses to store
    - Should be less than 256!
    - Only if TCPIP_DHCP_USE_OPTION_NTP_SERVER is enabled
    - Default value: 1
    - Symbol TCPIP_DHCP_NTP_SERVER_ADDRESSES, TCPIP_DHCP_OPTION_NTP_SERVER_MAX 

- **Enable the DHCP client:**
    - Default value for the enable/disable the DHCP client at stack start-up.
    - Note: the interface initialization setting in TCPIP_NETWORK_CONFIG takes precedence!
    - Default value: none
    - Symbol TCPIP_DHCP_CLIENT_ENABLED

- **Boot File Name Option:**
    - Enable the usage of the Boot file name received from the DHCP server
    - Default value: false
    - Symbol TCPIP_DHCP_STORE_BOOT_FILE_NAME

- **Boot file name storage size:**
    - Size of the storage for the Boot file name
    - should always be <= 128
    - Default value: 128
    - Symbol TCPIP_DHCP_BOOT_FILE_NAME_SIZE


