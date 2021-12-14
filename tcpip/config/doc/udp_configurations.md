# User Datagram Protocol (UDP)  Configurations

**UDP Specific User Configurations**

- **Maximum number of Sockets:**
    - Maximum number of UDP sockets that can be opened simultaneously
    - These sockets will be created when the module is initialized.
    - Default value: 10
    - Symbol TCPIP_UDP_MAX_SOCKETS

- **Default TX Buffer Size:**
    - Default socket TX buffer size.
    - Note that this setting affects all UDP sockets that are created and, together with TCPIP_UDP_MAX_SOCKETS, has a great impact on the heap size that is used by the stack (see TCPIP_STACK_DRAM_SIZE setting).
    - When large TX bufferrs are needed, probably a dynamic, per socket approach, is a better choice (see TCPIP_UDP_OptionsSet function).
    - Default value: 512
    - Symbol TCPIP_UDP_SOCKET_DEFAULT_TX_SIZE

- **Enable Console Commands:**
    - Enable the UDP TCP/IP Console Commands
    - This setting will add a UDP "info" command to the TCP/IP command menu allowing a run time inspection of the UDP sockets
    - Set the value to 1 to enable, 0 to disable the feature.
    - Note: to have the UDP commands available the TCP/IP commands need to be enabled.
    - Default value: false
    - Symbol: TCPIP_UDP_COMMANDS

- **Calculate TX Checksum:**
    - Calculate and transmit a checksum when sending data.
    - Checksum is not mandatory for UDP packets but is highly recommended.
    - This will affect the UDP TX performance.
    - Default value: true
    - Symbol TCPIP_UDP_USE_TX_CHECKSUM

- **Calculate RX Checksum:**
    - Check incoming packets to have proper checksum.
    - Checksum is not mandatory for UDP packets but is highly recommended.
    - This will affect the UDP RX performance.
    - Default value: true
    - Symbol TCPIP_UDP_USE_RX_CHECKSUM

- **TX Queue Limit:**
    - The maximum number of TX packets that can be queued by an UDP socket at a certain time.
    - For sockets that need to transfer a lot of data (Iperf, for example), especially on slow connections this limit prevents running out of memory because the MAC/PHY transfer cannot keep up with the UDP packet allocation rate imposed by the application.
    - Adjust depending on the TCPIP_UDP_SOCKET_DEFAULT_TX_SIZE, the connection speed and the amount of memory available to the stack.
    - Default value: 3
    - Symbol TCPIP_UDP_SOCKET_DEFAULT_TX_QUEUE_LIMIT
- **RX Queue Limit:**
    - The maximum number of RX packets that can be queued by an UDP socket at a certain time.
    - Note that UDP sockets do not use their own RX buffers but instead use the network driver supplied packets and a timely processing is critical to avoid packet memory starvation for the whole stack.
    - This symbol sets the maximum number of UDP buffers/packets that can be queued for a UDP socket at a certain time.
    - Once this limit is reached further incoming packets are silently discarded.
    - Adjust depending on the number of RX buffers that are available for the stack and the amount of memory available to the stack.
    - Default value: 3
    - Symbol TCPIP_UDP_SOCKET_DEFAULT_RX_QUEUE_LIMIT

- **Enable Pool Buffers:**
    - Enable the build of the pre-allocated pool buffers option
    - Default value: false
    - Symbol TCPIP_UDP_USE_POOL_BUFFERS

- **Number of Socket Pool Buffers:**
    - Number of buffers in the private UDP pool.
    - These are preallocated buffers that are to be used by UDP sockets only.
    - This improves the UDP socket throughput and is meant only for UDP sockets that have to sustain high TX traffic rate.
    - However, this memory is not returned to the stack heap, it always belongs to UDP.
    - A socket needs to have an option set in order to use the buffers pool (see the UDPSetOptions()).
    - Use 0 to disable the feature.
    - Default value: 4
    - Symbol TCPIP_UDP_SOCKET_POOL_BUFFERS

- **Pool Buffer Size:**
    - Size of the buffers in the UDP pool.
    - Any UDP socket that is enabled to use the pool and has the TX size <= than this size can use a buffer from the pool.
    - Note that this setting, together with TCPIP_UDP_SOCKET_POOL_BUFFERS, has impact on the heap size that is used by the stack (see TCPIP_STACK_DRAM_SIZE setting).
    - Default value: 512
    - Symbol TCPIP_UDP_SOCKET_POOL_BUFFER_SIZE

- **Enable External Packet Processing:**
// This setting enables/disables the processing of the RX packets by an external handler
// The user of the UDP can register an external function to process the incoming packets
// If true, the functionality is built in and could be used by the application
// If false, the functionality does not exist and the generated code is slightly smaller 
    - Default value: false
    - Symbol TCPIP_UDP_EXTERN_PACKET_PROCESS

