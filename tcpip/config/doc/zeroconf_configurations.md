# Zero Configuration Link Local (ZCLL)  Configurations

**ZCLL Specific User Configurations**
**MDNS/Bonjour Specific User Configurations**


- **ZCLL Task Rate (in msec):**
    - ZCLL task processing rate, in milliseconds.
    - The ZCLL module will process a timer event with this rate for maintaining its own queues, processing timeouts, etc.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Default value: 113
    - Symbol TCPIP_ZC_LL_TASK_TICK_RATE

- **Enable Link Local Warning Messages:**
    - Enable printing of the ZCLL warning messages
    - Default value: false
    - Symbol TCPIP_ZC_WARN_ZCLL


- **Enable Link Local Information Messages:**
    - Enable printing of the ZCLL info messages
    - Default value: false
    - Symbol TCPIP_ZC_INFO_ZCLL


- **Enable Link Local Debug Messages:**
    - Enable printing of the ZCLL debug messages
    - Default value: false
    - Symbol TCPIP_ZC_DEBUG_ZCLL

- **Probe Wait (in sec):**
    - Initial random delay
    - Value is in seconds
    - Default value: 1
    - Symbol TCPIP_ZC_LL_PROBE_WAIT

- **Minimum Time Between Probes (in sec):**
    - Minimum delay between probes
    - Value is in seconds
    - Default value: 1
    - Symbol TCPIP_ZC_LL_PROBE_MIN

- **Maximum Time Between Probes (in sec):**
    - Maximum delay between probes
    - Value is in seconds
    - Default value: 2
    - Symbol TCPIP_ZC_LL_PROBE_MAX

- **Number of Probes:**
    - Number of probe packets to be sent
    - Default value: 3
    - Symbol TCPIP_ZC_LL_PROBE_NUM

- **Delay Before Announcing:**
    - Delay before announcing
    - Value is in seconds
    - Default value: 2
    - Symbol TCPIP_ZC_LL_ANNOUNCE_WAIT

- **Number of Announcement Packets:**
    - Number of announcement packets
    - Default value: 2
    - Symbol TCPIP_ZC_LL_ANNOUNCE_NUM

- **Time Between Announcement Packets (in sec):**
    - time between announcement packets
    - Value is in seconds
    - Default value: 2
    - Symbol TCPIP_ZC_LL_ANNOUNCE_INTERVAL

- **Maximum Conflicts Before Rate Limiting:**
    - The maximum number of conflicts before rate limiting
    - Default value: 10
    - Symbol TCPIP_ZC_LL_MAX_CONFLICTS

- **Delay Between Attempts (in sec):**
    - Delay between successive attempts
    - Value is in seconds
    - Default value: 60
    - Symbol TCPIP_ZC_LL_RATE_LIMIT_INTERVAL

- **Rate Between Defensive ARPs (in sec):**
    - Wait time between defensive ARPs
    - Value is in seconds
    - Default value: 10
    - Symbol TCPIP_ZC_LL_DEFEND_INTERVAL

- **Base IP:**
    - Base IP address for ZCLL
    - Value is in Network order (BE) 
    - Default value: 0xa9fe0100 (169.254.1.0)
    - Symbol TCPIP_ZC_LL_IPV4_LLBASE

- **Base Netmask:**
    - Base IP network mask for ZCLL
    - Value is in Network order (BE) 
    - Default value: 0x0000FFFF (255.255.0.0)
    - Symbol TCPIP_ZC_LL_IPV4_LLBASE_MASK


- **ZC MDNS Task Rate (in msec):**
    - ZC MDNS task processing rate, in milliseconds.
    - The ZC MDNS module will process a timer event with this rate for maintaining its own queues, processing timeouts, etc.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Default value: 63
    - Symbol TCPIP_ZC_MDNS_TASK_TICK_RATE


- **Enable Multicast DNS Information Messages:**
    - Enable ZC MDNS info messages
    - Default value: false
    - Symbol TCPIP_ZC_INFO_MDNS


- **Enable Multicast DNS Warning Messages:**
    - Enable ZC MDNS warning messages
    - Default value: false
    - Symbol TCPIP_ZC_WARN_MDNS


- **Enable Multicast DNS Debug Messages:**
    - Enable ZC MDNS debug messages
    - Default value: false
    - Symbol TCPIP_ZC_DEBUG_MDNS

- **Port:**
    - ZC MDNS local port
    - Default value: 5353
    - Symbol TCPIP_ZC_MDNS_PORT

- **Maximum Host Name Size:**
    - Maximum Host name size
    - Default value: 32
    - Symbol TCPIP_ZC_MDNS_MAX_HOST_NAME_SIZE


- **Maximum Size Allowed For a Label:**
    - Maximum size allowed for a label. RFC 1035 (2.3.4) == 63
    - Default value: 63
    - Symbol TCPIP_ZC_MDNS_MAX_LABEL_SIZE

- **Maximum Resource Record Name Size:**
    - Maximum Resource Recd Name size. RFC 1035 (2.3.4) == 255
    - Default value: 256 (255 + '\0')
    - Symbol TCPIP_ZC_MDNS_MAX_RR_NAME_SIZE


- **Maximum Server Type Size:**
    - Maximum size of the Server Type. Eg. "_http._tcp.local"
    - Maximum value is 255
    - Default value: 32
    - Symbol TCPIP_ZC_MDNS_MAX_SRV_TYPE_SIZE

- **Maximum Server Name Size:**
    - Maximum size of the Server Name. Eg. "My Web server"
    - Maximum value is 255
    - Default value: 64
    - Symbol TCPIP_ZC_MDNS_MAX_SRV_NAME_SIZE
    
    
- **Maximum Text Data Size:**
    - Maximum size of the text data. Eg. "path=/index.htm"
    - Default value: 128
    - Symbol TCPIP_ZC_MDNS_MAX_TXT_DATA_SIZE
    
- **Time-To-Live for a Resource-Record (in sec):**
    - Time-To-Live for a Resource-Record
    - Value is in seconds
    - Default value: 3600
    - Symbol TCPIP_ZC_MDNS_RESOURCE_RECORD_TTL_VAL

- **Maximum Number of Resource-Records/Service:**
    - Maximum number of Resource-Records/Service for A, PTR, SRV, and TXT
    - Default value: 4
    - Symbol TCPIP_ZC_MDNS_MAX_RR_NUM

- **Initial Random Delay (in msec):**
    - ZC MDNS initial random delay
    - Value is in seconds
    - Default value: 750
    - Symbol TCPIP_ZC_MDNS_PROBE_WAIT

- **Maximum Delay Till Repeated Probe (in msec):**
    - ZC MDNS maximum delay till repeated probe
    - Value is in milliseconds
    - Default value: 250
    - Symbol TCPIP_ZC_MDNS_PROBE_INTERVAL


- **Number of Probes:**
    - ZC MDNS Number of probe packets
    - Default value: 3
    - Symbol TCPIP_ZC_MDNS_PROBE_NUM

- **Maximum Number of Conflicts:**
    - Maximum number of conflicts before move on to announce
    - Default value: 30
    - Symbol TCPIP_ZC_MDNS_MAX_PROBE_CONFLICT_NUM


- **Number of Announcement Packets:**
    - Number of announcement packets
    - Default value: 3
    - Symbol TCPIP_ZC_MDNS_ANNOUNCE_NUM

- **Time Between Announcement Packets (in msec):**
    - Time between announcement packets
    - Value is in milliseconds
    - Default value: 250
    - Symbol TCPIP_ZC_MDNS_ANNOUNCE_INTERVAL

- **Delay Before Announcing (in msec):**
    - delay before announcing
    - Value is in milliseconds
    - Default value: 250
    - Symbol TCPIP_ZC_MDNS_ANNOUNCE_WAIT


