# Dynamic Domain Name Service (DynDNS) Configurations

**Dynamic Domain Name Service (DynDNS) Specific User Configurations**


- **Default CheckIP Server:**
    - Default CheckIP server for determining current IP address
    - Default value: )"checkip.dyndns.com"
    - Symbol TCPIP_DDNS_CHECKIP_SERVER

- **dynDNS Task Rate (in msec):**
    - dynDNS task rate, ms
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Default value: 777
    - Symbol TCPIP_DDNS_TASK_TICK_RATE




