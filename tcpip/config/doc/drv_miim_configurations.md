# MIIM Driver  Configurations

**MIIM Driver Specific User Configurations**

- **Maximum Number of Clients:**
    - MIIM number of clients
    - This definition select the MIIM Maximum Number of Clients per instance.
    - Notes:
        - By default the 1st MIIM client is the DRV_ETHPHY.
        - An extra client is allowed (to perform operations from the console).
    - Default value: 2
    - Symbol DRV_MIIM_INSTANCE_CLIENTS

- **Stack Size (in bytes):**
    - The size of the stack for the task running the MIIM driver in an RTOS environment
    - Default value: 4096
    - Symbol DRV_MIIM_RTOS_STACK_SIZE

- **Task Priority:**
    - The priority of the task running the MIIM driver in an RTOS environment
    - Default value: 1
    - Symbol DRV_MIIM_RTOS_TASK_PRIORITY

- **Use Task Delay:**
    - Enable the delay to wait/sleep for the task running the MIIM driver in an RTOS environment
    - Default value: true
    - Symbol DRV_MIIM_RTOS_USE_DELAY

- **Use Task Delay:**
    - The delay value to wait/sleep for the task running the MIIM driver in an RTOS environment
    - Default value: 1
    - Symbol DRV_MIIM_RTOS_DELAY

- **ETH Module ID:**
    - The Ethernet Module ID of which this MIIM module belongs to
    - Default value: GMAC_BASE_ADDRESS/_ETH_BASE_ADDRESS/EMAC0_BASE_ADDRESS/EMAC1_BASE_ADDRESS
    - Symbol DRV_MIIM_ETH_MODULE_ID

- **Number of Driver Instances:**
    - This definition selects the maximum number of hardware instances that can be supported by the dynamic driver.
    - Usually set to 1.
    - Default value: 1
    - Symbol DRV_MIIM_INSTANCES_NUMBER

- **Maximum Number of Queued Operations:**
    - This definition selects the maximum number of simultaneous operations that can be supported by this driver.
    - Note that this represents operations for all clients
    - Default value: 4
    - Symbol DRV_MIIM_INSTANCE_OPERATIONS

- **Enable Client Operation Protection Feature:**
    - Because of the recirculation of the operation handles and client handles the possibility exists that a misbehaved client inadvertently gets the results of a previous completed operations that now belongs to a different client.
    - When this feature is enabled, extra protection is added for an operation handle to uniquely identify a client that has started the operation and extra check is done that operation belongs to the client that asks for the result.
    - Notes:
        - Set the value to 1 to enable, 0 to disable the feature.
        - Enabling this feature requires a small overhead in code and data size.
    - Default value: false
    - Symbol DRV_MIIM_CLIENT_OP_PROTECTION

- **Enable MIIM Commands:**
    - Enables/Disables MIIM commands feature.
    - It adds a MIIM command to the TCP/IP command menu allowing to read/write a PHY register.
    - Note:
        - Currently the MIIM commands are integrated in the TCP/IP commands. To have the MIIM commands available the TCP/IP commands need to be enabled.
    - Default value: false
    - Symbol DRV_MIIM_COMMANDS

- **MIIM Driver Object:**
    - The object to use for the MIIM driver.
    - Default value: DRV_MIIM_OBJECT_BASE_Default
    - Symbol DRV_MIIM_DRIVER_OBJECT

- **MIIM Driver Object Index:**
    - The index of the MIIM driver object
    - Currently this index is set to 0
    - Default value: 0
    - Symbol DRV_MIIM_DRIVER_INDEX

