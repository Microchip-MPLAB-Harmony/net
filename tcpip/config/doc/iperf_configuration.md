# Iperf  Configurations

**Iperf Specific User Configurations**


- **Number of Iperf Instances:**
    - Maximum number of simultaneously running iperf instances
    - Default value: 1
    - Symbol TCPIP_IPERF_MAX_INSTANCES


- **Socket TX Buffer Size:**
    - Default size of the Iperf TX buffer
    - The performance of a socket is highly dependent on the size of its buffers so it is a good idea to use as large as possible buffers for the sockets that need high throughput. 
    - Bigger buffers will help obtain higher performance numbers
    - Default value: 4 KB
    - Symbol TCPIP_IPERF_TX_BUFFER_SIZE

- **Socket RX Buffer Size:**
    - Default size of the Iperf RX buffer
    - The performance of a socket is highly dependent on the size of its buffers so it is a good idea to use as large as possible buffers for the sockets that need high throughput. 
    - Bigger buffers will help obtain higher performance numbers
    - Default value: 4 KB
    - Symbol TCPIP_IPERF_RX_BUFFER_SIZE

- **Timeout for TX Channel to Become Ready (in msec):**
    - Timeout to wait for TX channel to be ready to transmit a new packet
    - Depends on the channel bandwidth
    - Value is in milliseconds
    - Default value: 100
    - Symbol TCPIP_IPERF_TX_WAIT_TMO

- **Maximum Number of UDP TX Packet Queue:**
    - For Iperf UDP client, the limit to set to avoid memory allocation overflow on slow connections
    - Default value: 2
    - Symbol TCPIP_IPERF_TX_QUEUE_LIMIT

- **Iperf Timing Error (in msec):**
    - iperf timing error
    - Accounts for timing uncertainty
    - The higher the value, the more relaxed is the iperf timing
    - Lower limit is 0
    - Value is in milliseconds
    - Default value: 0
    - Symbol TCPIP_IPERF_TIMING_ERROR_MARGIN

- **Default TX Bandwidth(in Mbps):**
    - Default TCP Tx Bandwidth
    - Value is in Mbps
    - Default value: 1
    - Symbol TCPIP_IPERF_TX_BW_LIMIT

