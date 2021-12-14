# TCPIP Reboot  Configurations

**Reboot Specific User Configurations**

- **Allow Only Same Subnet:**
    - For improved security, you might want to limit reboot capabilities to only users on the same IP subnet.
    - Defining  this symbol enables this access restriction.
    - Default value: true
    - Symbol TCPIP_REBOOT_SAME_SUBNET_ONLY


- **Reboot Message:**
    - The mesage needed to be sent accross the net to reboot the machine
    - Default value: "MCHP Reboot"
    - Symbol TCPIP_REBOOT_MESSAGE


- **Reboot Task Rate (in msec):**
    - Reboot task processing rate, in milliseconds.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - The Reboot module just listens for incoming reboot requests
    - A high operation frequency is not required.
    - Default value: 1130
    - Symbol TCPIP_REBOOT_TASK_TICK_RATE


