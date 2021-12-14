# Internet Control Message Protocol (ICMP)  Configurations

**ICMP Specific User Configurations**

- **Use ICMPv4 Server:**
    - Build with the ICMP server support
    - Default value: true
    - Symbol TCPIP_STACK_USE_ICMP_SERVER

- **Allow Replies to Echo Broadcast Requests:**
    - Allow replies to echo requests sent to a broadcast address
    - By default this should be disabled, to prevent broadcast flood attacks
    - Note that this could be a security issue
    - Default value: false
    - Symbol TCPIP_ICMP_ECHO_BROADCASTS, TCPIP_ICMP_ECHO_ALLOW_BROADCASTS

- **Use ICMPv4 Client:**
    - Build with the ICMP client support
    - Default value: false
    - Symbol TCPIP_STACK_USE_ICMP_CLIENT


- **ICMP Task Rate (in msec):**
    - ICMP task processing rate, in milliseconds.
    - The ICMP module will process a timer event with this rate for maintaining its own queues, processing timeouts, etc.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Default value: 33
    - Symbol TCPIP_ICMP_TASK_TICK_RATE


- **Maximum Number of Supported Client Requests:**
    - Maximum number of parallel outgoing ICMP echo requests supported at one time
    - Default value: 4
    - Symbol TCPIP_STACK_MAX_CLIENT_ECHO_REQUESTS

- **Echo request timeout (in msec):**
    - ICMP timeout for a request that was sent and no reply was received
    - Value is in milliseconds
    - Default value: 500
    - Symbol TCPIP_ICMP_ECHO_REQUEST_TIMEOUT

- **Enable Console Commands:**
    - Enable the ICMP TCP/IP Console ping commands
    - Default value: false
    - Symbol: TCPIP_ICMP_COMMAND_ENABLE

- **Number of ICMP Echo requests:**
    - The console ping command number of Echo requests to send
    - Default value: 4
    - Symbol TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUESTS

- **ICMP Request Delay (in msec):**
    - The console ping command delay between between 2 echo requests
    - Default value: 1000
    - Symbol TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DELAY

- **ICMP Give Up Time-out (in msec):**
    - The console ping command time out
    - If no reply received after this time, the command will give up
    - Default value: 5000
    - Symbol TCPIP_STACK_COMMANDS_ICMP_ECHO_TIMEOUT


- **Echo Request Data Buffer size - bytes:**
    - The console ping command data buffer size 
    - Default value: 2000
    - Symbol TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_BUFF_SIZE

- **Echo request Default Data Size - bytes:**
    - The console ping command default echo request size
    - Default value: 100
    - Symbol TCPIP_STACK_COMMANDS_ICMP_ECHO_REQUEST_DATA_SIZE


