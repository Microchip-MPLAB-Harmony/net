# Network Time Protocol (SNTP) Configurations

**SNTP Specific User Configurations**

- **NTP Server Name:**
    - The NTP Server to use
    - These are normally available network time servers.
    - The actual IP returned from the pool will vary every minute so as to spread the load around stratum 1 time servers.
    - For best accuracy and network overhead you should locate the pool server closest to your geography, but it will still work if you use the global pool.ntp.org address or choose the wrong one or ship your embedded device to another geography.
    - A direct IP address works too
    - Default value: "pool.ntp.org"
    - Symbol TCPIP_NTP_SERVER

- **Default Connection Type:**
    - The default connection type to use: IPv4/IPv6
    - Default value: IP_ADDRESS_TYPE_IPV4
    - Symbol TCPIP_NTP_DEFAULT_CONNECTION_TYPE


- **SNTP Task Rate (in msec):**
    - SNTP task processing rate, in milliseconds.
    - The SNTP module will process a timer event with this rate for maintaining its own queues, processing timeouts, etc.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - This module contacts an NTP server and a high operation frequency is not required.
    - Default value: 1100
    - Symbol TCPIP_NTP_TASK_TICK_RATE


- **RX Queue Limit:**
    - The NTP RX queue limit
    - Defines the maximum number of packets that can wait in the NTP queue
    - Default value: 2
    - Symbol TCPIP_NTP_RX_QUEUE_LIMIT


- **Server Address Maximum Length:**
    - Maximum number of characters allowed for the NTP server name
    - Default value: 30
    - Symbol TCPIP_NTP_SERVER_MAX_LENGTH


- **Query Interval (in sec):**
    - Defines how frequently to resynchronize the date/time
    - Value is in seconds
    - Default value: (10 * 60)
    - Symbol TCPIP_NTP_QUERY_INTERVAL
    

- **Fast Query Interval (in sec):**
    - Defines how long to wait to retry an update after a failure
    - Updates may take up to 6 seconds to fail, so this 14 second delay is actually only an 8-second retry.
    - Value is in seconds
    - Default value: 14
    - Symbol TCPIP_NTP_FAST_QUERY_INTERVAL


- **Epoch:**
    - Reference Epoch to use (default: 01-Jan-1970 00:00:00).
    - Default value: (86400ul * (365ul * 70ul + 17ul))
    - Symbol TCPIP_NTP_EPOCH
  

- **NTP Reply Timeout (in sec):**
    - Defines how long to wait before assuming the query has failed
    - Value is in seconds
    - Default value: 6
    - Symbol TCPIP_NTP_REPLY_TIMEOUT

- **Max Stratum:**
    - The maximum acceptable NTP stratum number
    - Should be less than 16 (unsynchronized server) 
    - Default value: 15
    - Symbol TCPIP_NTP_MAX_STRATUM

- **Time Stamp Timeout (in sec):**
    - Elapsed time that qualifies a time stamp as stale
    - Nnormally it should be correlated with TCPIP_NTP_QUERY_INTERVAL
    - Default value: (11 * 60)
    - Symbol TCPIP_NTP_TIME_STAMP_TMO


- **SNTP Version:**
    - The default NTP version to use (3 or 4)
    - Default value: 4
    - Symbol TCPIP_NTP_VERSION 


- **Default Interface:**
    - For multi-homed hosts, the default SNTP interface
    - Default value: interface 0
    - Symbol TCPIP_NTP_DEFAULT_IF

