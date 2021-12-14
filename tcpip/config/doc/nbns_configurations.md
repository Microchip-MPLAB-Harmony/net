# NetBIOS Name Service (NBNS)  Configurations

**NBNS Specific User Configurations**


- **NBNS Task Rate (in msec):**
    - NBNS task processing rate, in milliseconds.
    - The NBNS module will process a timer event with this rate for maintaining its own queues, processing timeouts, etc.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Default value: 110
    - Symbol TCPIP_NBNS_TASK_TICK_RATE

