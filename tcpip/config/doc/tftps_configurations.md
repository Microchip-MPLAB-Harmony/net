# TFTP Server  Configurations

**TFTP Server Specific User Configurations**


- **Maximum number of Clients:**
    - The number of Clients requests will be processed for the TFTP server stack.
    - Default value: 3
    - Symbol TCPIP_TFTPS_CLIENT_NUMBER


- **Default file name length:**
    - The default length of the file name
    - Default value: 64
    - Symbol TCPIP_TFTPS_FILENAME_LEN

- **Root Directory Path:**
    - The root of the TFTP server directory
    - The server will have access only to the files beneath this path
    - Default value: none
    - Symbol TCPIP_TFTPS_MOUNT_POINT

- **Enable Console Commands:**
    - Enable the TFTP TCP/IP Console Commands
    - Default value: false
    - Symbol: TCPIP_TFTPS_CONSOLE_CMD

- **TFTP Task Rate (in msec):**
    - TFTP task processing rate, in milliseconds.
    - The TFTP module will process a timer event with this rate for maintaining its own queues, processing timeouts, etc.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Default value: 60
    - Symbol TCPIP_TFTPS_TASK_TICK_RATE

- **Maximum Number of Retransmissions:**
    - The number of attempts to retransmit  the previous packet before declaring a TIMEOUT error.
    - Default value: 3
    - Symbol TCPIP_TFTPS_RETRANSMIT_COUNT

- **Maximum File Size (bytes):**
    - The maximum size for a file in bytes to be accepted.
    - Default value: 64000
    - Symbol TCPIP_TFTPS_DEFAULT_FILESIZE


- **Maximum Block Size (bytes):**
    - The maximum value for the file block size.
    - Even though the RFC allows blocks of up to 65464 bytes, in practice the limit is set to 1468 bytes:
        - the size of an Ethernet MTU minus the headers of TFTP (4 bytes), UDP (8 bytes) and IP (20 bytes)
    - Default value: 512
    - Symbol TCPIP_TFTPS_DEFAULT_BLOCK_SIZE

- **Default Retransmission timeout (in sec):**
    - This time is used during the TFTP retransmission 
    - The server will wait for the client for these many seconds before retransmitting.
    - Value is in seconds
    - Default value: 10
    - Symbol TCPIP_TFTPS_DEFAULT_TIMEOUT

- **Command process timeout (in sec):**
    - The number of seconds to wait before declaring a TIMEOUT when processing a GET/PUT command
    - Value is in seconds
    - Default value: 10
    - Symbol TCPIP_TFTPS_CMD_PROCESS_TIMEOUT

- **Enable User Notification:**
    - Allow TFTP Server user notification
    - If enabled, the TCPIP_TFTPS_HandlerRegister/TCPIP_TFTPS_HandlerDeRegister functions exist and can be used
    - Default value: false
    - Symbol TCPIP_TFTPS_USER_NOTIFICATION


