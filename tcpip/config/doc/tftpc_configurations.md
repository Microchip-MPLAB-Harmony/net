# Trivial File Transfer Protocol (TFTP) Client  Configurations

**TFTP Client Specific User Configurations**

- **TFTP Client Maximum Retries:**
    - The number of attempts before declaring a TIMEOUT error.
    - Default value: 3
    - Symbol TCPIP_TFTPC_MAX_RETRIES
 
- **Maximum Length for File name:**
    - The maximum value for the file name size.
    - Default value: 32
    - Symbol TCPIP_TFTPC_FILENAME_LEN

- **Enable Console Commands:**
    - Enable the TFTP TCP/IP Console Commands
    - Default value: false
    - Symbol: TCPIP_TFTPC_CONSOLE_CMD

- **TFTP Task Rate (in msec):**
    - TFTP task processing rate, in milliseconds.
    - The TFTP module will process a timer event with this rate for maintaining its own queues, processing timeouts, etc.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Default value: 100
    - Symbol TCPIP_TFTPC_TASK_TICK_RATE


- **Enable User Notification:**
    - Allow TFTP client user notification
    - If enabled, the TCPIP_TFTPC_HandlerRegister/TCPIP_TFTPC_HandlerDeRegister functions exist and can be used
    - Default value: false
    - Symbol TCPIP_TFTPC_USER_NOTIFICATION


- **Maximum Length for Server Address:**
    - The maximum length to store the server address
    - Default value: 40
    - Symbol TCPIP_TFTPC_SERVERADDRESS_LEN


- **TFTP Client Socket Connection Timeout (in sec):**
    - The number of seconds to wait before declaring a TIMEOUT error on PUT.
    - Value is in seconds
    - Default value: 3
    - Symbol TCPIP_TFTPC_ARP_TIMEOUT


- **TFTP Client Command Process Timeout (in sec):**
    - The number of seconds to wait before declaring a TIMEOUT error on GET.
    - Value is in seconds
    - Default value: 3
    - Symbol TCPIP_TFTPC_CMD_PROCESS_TIMEOUT

- **Default Interface:**
    - The default TFTP interface for multi-homed hosts.
    - Default value: interface 0
    - Symbol TCPIP_TFTPC_DEFAULT_IF


