# Transmission Control Protocol (TCP)  Configurations

**TCP Specific User Configurations**

- **Maximum TX Segment Size:**
    - TCP Maximum Segment Size for TX.  The TX maximum segment size is actually governed by the remote node MSS option advertised during connection establishment.
        - However, if the remote node specifies an unmanageably large MSS (ex: > Ethernet MTU), this define sets a hard limit so that TX buffers are not overflowed.
       - If the remote node does not advertise a MSS option, all TX segments are fixed at 536 bytes maximum.
    - This symbol sets an upper bound on the TCP MSS on the transmit side which can ignore/override the remote node advertising.
    - Default value: 1460
    - Symbol TCPIP_TCP_MAX_SEG_SIZE_TX

- **Maximum Number of Sockets:**
    -	The maximum number of sockets to create in the stack.
    -	When defining TCPIP_TCP_MAX_SOCKETS take into account the number of interfaces the stack is supporting.
    - Default value: 10
    - Symbol TCPIP_TCP_MAX_SOCKETS

- **Default TX Buffer Size:**
    - Default socket TX buffer size
    - Note that this setting affects all TCP sockets that are created and, together with TCPIP_TCP_MAX_SOCKETS, has a great impact on the heap size that is used by the stack (see TCPIP_STACK_DRAM_SIZE setting).
    - When large TX bufferrs are needed, probably a dynamic, per socket approach, is a better choice (see TCPIP_TCP_OptionsSet function).
    - The performance of a socket is highly dependent on the size of its buffers so it is a good idea to use as large as possible buffers for the sockets that need high throughput. 
    - Note that some modules (like HTTP) use their own settings to specify the buffer size for their TCP sockets.
    - Default value: 512
    - Symbol TCPIP_TCP_SOCKET_DEFAULT_TX_SIZE

- **Default RX Buffer Size:**
    - Default socket RX buffer size
    - Note that this setting affects all TCP sockets that are created and, together with TCPIP_TCP_MAX_SOCKETS, has a great impact on the heap size that is used by the stack (see TCPIP_STACK_DRAM_SIZE setting).
    - When large RX bufferrs are needed, probably a dynamic, per socket approach, is a better choice (see TCPIP_TCP_OptionsSet function).
    - The performance of a socket is highly dependent on the size of its buffers so it is a good idea to use as large as possible buffers for the sockets that need high throughput. 
    - Note that some modules (like HTTP) use their own settings to specify the buffer size for their TCP sockets.
    - Default value: 512
    - Symbol TCPIP_TCP_SOCKET_DEFAULT_RX_SIZE

- **Default Keep Alive Time-out (in msec):**
    - Timeout for keep-alive messages when no traffic is sent
    - Value is in milliseconds
    - Default value: 10000
    - Symbol TCPIP_TCP_KEEP_ALIVE_TIMEOUT
		
- **Enable TCP Console Commands:**
    - Enables/Disables the TCP command feature (TCPIP TCP info commands)
    - This setting will add a TCP "info" command to the TCP/IP command menu allowing a run time inspection of the TCP sockets
    - Set the value to 1 to enable, 0 to disable the feature.
    - Note: to have the TCP commands available the TCP/IP commands need to be enabled.
    - Default value: false
    - Symbol TCPIP_TCP_COMMANDS

- **TCP Task Rate (in msec):**
    - TCP task processing rate, in milliseconds.
    - The TCP module will process a timer event with this rate for maintaining its own queues, processing timeouts, etc.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Value is in milliseconds
    - Default value: 5
    - Symbol TCPIP_TCP_TASK_TICK_RATE

- **Enable TCP Sockets Dynamic Options:**
    - Enable the TCP sockets dynamic options set/get functionality
    - If enabled, the functions: TCPIP_TCP_OptionsSet, TCPIP_TCP_OptionsGet and TCPIP_TCP_FifoSizeAdjust exist and are compiled in
    - If disabled, these functions do not exist and cannot be used/called 
    - Note that this setting can affect modules that use TCP sockets
    - Default value: true
    - Symbol TCPIP_TCP_DYNAMIC_OPTIONS

- **Start Time-out (in msec):**
    - Timeout to retransmit unacked data
    - Value is in milliseconds
    - Default value: 1000
    - Symbol TCPIP_TCP_START_TIMEOUT_VAL

- **Delayed Ack Time-out (in msec):**
// Timeout for delayed-acknowledgment algorithm
    - Value is in milliseconds
    - Default value: 100
    - Symbol TCPIP_TCP_DELAYED_ACK_TIMEOUT

- **Fin Wait 2 Time-out (in msec):**
    - Timeout for FIN WAIT 2 state
    - Value is in milliseconds
    - Default value: 5000
    - Symbol TCPIP_TCP_FIN_WAIT_2_TIMEOUT

- **Maximum Number of Transmission Retries:**
    - Maximum number of retransmission attempts
    - Default value: 5
    - Symbol TCPIP_TCP_MAX_RETRIES

- **Maximum Number of Keep Alive Retries:**
    - Maximum number of keep-alive messages that can be sent without receiving a response before automatically closing the connection
    - Default value: 6
    - Symbol TCPIP_TCP_MAX_UNACKED_KEEP_ALIVES

- **Maximum Number of SYN Retries:**
    - Number of retries for a client socket to connect to a server
    - Default value: 3
    - Symbol TCPIP_TCP_MAX_SYN_RETRIES

- **Auto Transmit Time-out (in msec):**
    - Timeout before automatically transmitting unflushed data
    - Value is in milliseconds
    - Default value: 40
    - Symbol TCPIP_TCP_AUTO_TRANSMIT_TIMEOUT_VAL

- **Window Update Time-out (in msec):**
    - Timeout before automatically transmitting a window update due to a TCPIP_TCP_Get() or TCPIP_TCP_ArrayGet() function call
    - Value is in milliseconds
    - Default value: 200
    - Symbol TCPIP_TCP_WINDOW_UPDATE_TIMEOUT_VAL

- **OBSOLETE! Close Wait Time-out (in msec). Should be 0!:**
    - Timeout for the CLOSE_WAIT state
    - If the remote socket closes the connection the TCP/IP stack will automatically close the socket after this timeout.
    - If the value is set to 0 then the socket will wait forever for the application to close the socket (default behavior). 
    - Note:  this is a legacy setting and sill be eventually removed.
        - Selecting a value != 0 is dangerous, especially in an RTOS environment when sockets could close by themselves without the socket owner being notified!
    - Always use 0 for this setting and close the socket specifically when needed!
    - Default value: 0
    - Symbol TCPIP_TCP_CLOSE_WAIT_TIMEOUT

- **Maximum Segment Length (MSL) Timeout (in sec):**
    - Maximum Segment Length (MSL) timeout
    - This value sets the time a socket will be in the TIME_WAIT/2MSL state after the socket performed an active close.
    - RFC recommended value is 120 sec.  Other common values are: 30/60 sec
    - Note that if this symbol is defined to 0, then the TIME_WAIT/2MSL state is skipped!
    - This could lead to duplicate sequence numbers problems if sockets are frequently/rapidly closed and reopened and normally should be avoided.
    - Value is in seconds
    - Default value: 30
    - Symbol TCPIP_TCP_MSL_TIMEOUT

- **TCP quiet time (in sec):**
    - This value specifies the TCP quiet time:
    - TCP will not send any data for TCPIP_TCP_QUIET_TIME seconds after rebooting!
    - Usualy this value should match the MSL value.
    - Note that if this symbol is defined to 0, then the TCP quiet time state is skipped!
    - During the quiet time there will be no TCP traffic initiated by TCP.  So, clients will not be able to connect to remote hosts and servers will not reply to client requests.
    - Value is in seconds
    - Default value: 30
    - Symbol TCPIP_TCP_QUIET_TIME

- **Enable External Packet Processing:**
    - This setting enables/disables the processing of the RX packets by an external handler
    - The user of the TCP can register an external function to process the incoming packets
    - If true, the functionality is built in and could be used by the application
    - If false, the functionality does not exist and the generated code is slightly smaller 
    - Default value: false
    - Symbol TCPIP_TCP_EXTERN_PACKET_PROCESS

- **Disable Crypto Dependency:**
    - This setting enables/disables the usage of the Crypto RNG + Hash by the TCP module
    - TCP uses Crypto RNG + MD5 hash to compute the socket ISN (Initial Sequence Number)
    - Crypto RNG is also used to allocate a new ephemeral port
    - If Crypto usage is disabled then TCP will use Pseudo Random calls instead.
    - This will require much less resources but it will be insecure and exposed to attacks.
    - !!Potential security vulnerability. See RFC 6528 and 6056.
    - Note: even with the Crypto usage disabled an application can make the ISN and port numbers
    - harder to guess by seeding periodically the pseudo random generator at run time with a good random value 
    - Note: For applications that use use the crypto library anyway (TLS for example), the impact in the resources when using the Crypto RNG is minimal and should be preferred
    - Default value: false
    - Symbol TCPIP_TCP_DISABLE_CRYPTO_USAGE

