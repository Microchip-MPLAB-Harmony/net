# TCPIP Announce  Configurations

**Announce Specific User Configurations**


- **Announce Broadcast Type:**
    - Type of the broadcast used by the Announce module
    - This type enables the network directed broadcast
    - If it is not defined or it is 0, the network limited broadcast is used
    - The default is nework limited broadcast
    - Default value: 0
    - Symbol TCPIP_ANNOUNCE_NETWORK_DIRECTED_BCAST



- **Announce Task Rate (in msec):**
    - Announce task processing rate, in milliseconds.
    - The Announce module will process a timer event with this rate for maintaining its own queues, processing timeouts, etc.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Default value: 333
    - Symbol TCPIP_ANNOUNCE_TASK_RATE


- **Maximum Payload Size:**
    - Maximum size of a payload sent once
    - Adjust to your needs
    - Default value: 512
    - Symbol TCPIP_ANNOUNCE_MAX_PAYLOAD



