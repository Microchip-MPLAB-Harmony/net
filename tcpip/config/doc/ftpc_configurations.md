# FTP Client  Configurations

**FTP Client Specific User Configurations**

- **Maximum number of simultaneous client:**
    - Specify the maximum number of supported simultaneous FTP client connections.
    - Default value: 3
    - Symbol Maximum number of simultaneous client

- **Default Data Socket Transmit Buffer Size:**
    - Specify the Default Transmit Buffer Size for the FTP Client Data Socket
    - if 0, the TCP Socket Transmit buffer size will be used by FTPC data socket
    - The default recommended value for high throughput is > 2MSS (3 KB).
    - Default value: 0
    - Symbol TCPIP_FTPC_DATA_SKT_TX_BUFF_SIZE_DFLT

- **Default Data Socket Receive Buffer Size:**
    - Specify the Default Receive Buffer Size for the FTP Client Data Socket
    - if 0, the TCP Socket Receive buffer size will be used by FTPC data socket
    - The default recommended value for high throughput is > 2MSS (3 KB).
    - Default value: 0
    - Symbol TCPIP_FTPC_DATA_SKT_RX_BUFF_SIZE_DFLT


- **Enable Console Commands:**
    - Enable the FTP client TCP/IP Console Commands
    - Default value: false
    - Symbol: TCPIP_FTPC_COMMANDS

- **FTP Client Task Rate (in msec):**
    - FTP task processing rate, in milliseconds.
    - The FTP module will process a timer event with this rate for maintaining its own queues, processing timeouts, etc.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Default value: 5
    - Symbol TCPIP_FTPC_TASK_TICK_RATE

- **FTP Request Time-out (in sec):**
    - Specify the time-out for FTP Client commands
    - This is the time to wait for a response from FTP server
    - Value is in seconds
    - Default value: 2
    - Symbol TCPIP_FTPC_TMO


