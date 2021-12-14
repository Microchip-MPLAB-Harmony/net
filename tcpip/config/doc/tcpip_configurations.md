# TCP/IP Stack Manager  Configurations

**TCP/IP Manager Specific User Configurations**


- **TCP/IP Stack State Machine Tick Rate, ms.:**
    - The TCP/IP task will require a timer event with this rate for maintaining its own state machine, processing timeouts for all modules, or dispatching the RX traffic if the interrupt operation is not enabled, etc.
    - The lower this value (higher the frequency) the higher the priority of the TCP/IP stack and a higher performance can be obtained.
    - Note: this is the time base for all TCP/IP modules, i.e. no TCP/IP module can run at a rate higher than this one.
    - Note: the System Tick resolution has to be fine enough to allow for this TCP/IP rate granularity.  
    - Value is in milliseconds
    - Default value: 5
    - Symbol TCPIP_STACK_TICK_RATE

- **Stack Size (in bytes):**
    - The size of the stack for the TCP/IP RTOS task
    - Default value: 4096
    - Symbol TCPIP_RTOS_TASK_STACK_SIZE

- **Task Priority:**
    - The priority of the TCP/IP RTOS task
    - Default value: 1
    - Symbol TCPIP_RTOS_TASK_PRIORITY

- **Use Task Delay?:**
    - Use delay to sleep the TCP/IP RTOS task
    - Default value: true
    - Symbol TCPIP_STACK_RTOS_USE_DELAY

- **Task Delay:**
    - The delay for the TCP/IP RTOS task
    - Default value: 1
    - Symbol TCPIP_STACK_RTOS_DELAY

- **Use Heap Config:**
    - Type of heap that the TCP/IP stack uses: internal heap, internal pool or external
    - This is a private TCP/IP heap and is used by internal stack modules that need dynamic memory (and most of them do): MAC buffers/packets, TCP and UDP buffers, etc.
    - Notes:
        - The internal heap provides reasonably fast allocation using a first fit algorithm
          So its allocation is memory efficient but can suffer of fragmentation issues.
        - The internal pool is very fast but works with fixed sizes.
          So the allocation may be not very memory efficient but there is no fragmentation.
          The pool creates blocks with different sizes that need to be defined separately.
          The initial pool configuration may be difficult.
          This pool works best when the pool entries can be increased at run time with new allocations.
        - The external heap relies on allocation functions provided to the stack (like the standard C malloc/calloc/free functions)
    - Default value: TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP
    - Symbol TCPIP_STACK_USE_HEAP_CONFIG (TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP, TCPIP_STACK_USE_EXTERNAL_HEAP, TCPIP_STACK_USE_INTERNAL_HEAP_POOL)

- **TCP/IP Stack Dynamic RAM Size:**
    - The total amount of internal dynamic memory heap that the TCPIP stack will create at start up.
    - This is the size of the internal heap, relevant only for the TCPIP_STACK_USE_INTERNAL_HEAP
    - Note: a small amount of memory is used for internal heap structure.
        - So the actual size of the created heap will be smaller than this.
    - Note:  the settings in other modules configuration parameters (especially the MAC driver, TCP, UDP, HTTP, etc.) should be consistent with the total amount of heap that is allocated to the stack.
    - Note: this parameter should also be consistent with the project linker heap setting ( TCPIP_STACK_DRAM_SIZE < project heap)
    - Note: the above notes apply for the TCPIP_STACK_USE_INTERNAL_HEAP_POOL selection too.
        - However, for the pool heap this symbol is not used because the total memory that results from adding up all the pool entries plus the storage needed for internal heap structures.
    - Default value: 39250
    - Symbol TCPIP_STACK_DRAM_SIZE

- **The Expansion Size for the Internal Heap Pool:**
    - The size of the expansion heap for a TCPIP_STACK_USE_INTERNAL_HEAP_POOL
    - This is the amount of heap allocated besides the size resulting from the pool entries and allows expanding at run time.
    - How a specific entry expands is controlled per entry
    - If set to 0, then there is no expansion at run time.
    - Note: this parameter is specific for TCPIP_STACK_USE_INTERNAL_HEAP_POOL
    - Note: this parameter allows the auto configuration of the pool at run time
    - Default value: 4096
    - Symbol TCPIP_STACK_POOL_EXPANSION_SIZE

- **Number of Heap Pool Entries:**
    - Number of pool entries that the pool will contain (this cannot be changed after the creation)
    - Default value: 8
    - Symbol TCPIP_HEAP_POOL_ENTRIES_NUMBER

- **Size of the Block, Bytes:**
    - Size of the blocks in this pool entry
    - Default value: 64 - 2064
    - Symbol TCPIP_HEAP_POOL_ENTRY_SIZE_IDXn

- **Number of Blocks to Dynamically Expand for this Pool Entry:**
    - Number of blocks to expand/allocate when an allocation in this entry failed
    - Default value: 0 - 10
    - Symbol TCPIP_HEAP_POOL_ENTRY_EXP_BLOCKS_IDXn

- **Stack allocation function, malloc style:**
    - Stack allocation function, malloc style
    - This is the function the stack will call to allocate memory needed for its own heap: TCPIP_STACK_DRAM_SIZE.
    - Note: if running under an RTOS environment and the stack is initialized at run time, the allocation function should be thread safe
    - Default value: "malloc"
    - Symbol TCPIP_STACK_MALLOC_FUNC

- **Stack allocation function, calloc style:**
    - Stack allocation function, calloc style
    - This is the function the stack will call to allocate memory needed for its own heap: TCPIP_STACK_DRAM_SIZE.
    - Note: if running under an RTOS environment and the stack is initialized at run time, the allocation function should be thread safe
    - Default value: "calloc"
    - Symbol TCPIP_STACK_CALLOC_FUNC

- **Stack deallocation function, free style:**
    - Stack deallocation function, free style
    - This is the function the stack will call for freeing the allocated memory when the stack is deinitialized.
    - Default value: "free"
    - Symbol TCPIP_STACK_FREE_FUNC

- **Flags supported for heap allocation:**
    -TCP/IP heap allocation flags
    - Default value: "TCPIP_STACK_HEAP_FLAG_ALLOC_UNCACHED"
    - Symbol TCPIP_STACK_HEAP_USE_FLAGS

- **Heap area usage:**
    - Definition of the heap usage/purpose.
    - It defines what modules in the TCP/IP stack will use this heap.
    - Notes:
        - This is an advanced definition that is currently not used.
        - Multiple heaps usage will be added in the future.
        - For now only one heap type is supported and its used for all allocation operations.
    - Default value: TCPIP_STACK_HEAP_USE_DEFAULT
    - Symbol TCPIP_STACK_HEAP_USAGE_CONFIG

- **Maximum heap types:**
    - Number of heap types at run time to enable debugging, statistics, trace on, etc.
    - Currently only 1 heap type is supported at run time, one of: internal heap, internal pool and external
    - Default value: 1
    - Symbol TCPIP_STACK_SUPPORTED_HEAPS

- **TCP/IP Stack Dynamic RAM Lower Limit:**
    - The minimum amount of dynamic memory left for run time allocation by the stack (IP, UDP, etc)
    - This is just a warning threshold.
    - If after all the modules are initialized the amount of memory available in the TCPIP heap is less then TCPIP_STACK_DRAM_RUN_LIMIT then a warning will be displayed (if the debug channel is enabled)
    - For proper operation there should be always a heap reserve of at least few KB.
    - Note: this limit is used only for TCPIP_STACK_USE_INTERNAL_HEAP and TCPIP_STACK_USE_INTERNAL_HEAP_POOL
    - Default value: 2048
    - Symbol TCPIP_STACK_DRAM_RUN_LIMIT

- **TCP/IP Stack Dynamic Allocation Debug:**
    - Enable debugging of an allocation call that failed.
    - If the system debug is enabled (SYS_DEBUG_ENABLE) the stack will issue a warning message at the system debug channel.
    - Default value: false
    - Symbol TCPIP_STACK_DRAM_DEBUG_ENABLE

- **TCP/IP Stack Dynamic Allocation Trace:**
    - Enable tracing of the allocated memory by each module.
    - The stack will trace all the memory allocated by a module and various statistics.
    - Default value: false
    - Symbol TCPIP_STACK_DRAM_TRACE_ENABLE

- **TCP/IP Stack Number of Trace Slots:**
    - Number of trace slots to be used.
    - There is on slot needed per module that allocates memory from the heap.
    - Default value: 16
    - Symbol TCPIP_STACK_DRAM_TRACE_SLOTS

- **TCP/IP Heap Size Estimate(in kilobytes):**
    - An estimation of the TCP/IP stack heap requirements based on the project settings
    - Default value: 39250
    - Symbol TCPIP_STACK_HEAP_SIZE_RECOMMEND

- **Module-wise Heap Estimate(show with each module):**
    - Show heap estimation for each stack module
    - Default value: false
    - Symbol TCPIP_STACK_HEAP_CALC_MASK

- **Enable User Notification:**
    - This setting enables the reporting of the events by the stack to the user using the notification system
    - If enabled, then TCPIP_STACK_HandlerRegister and TCPIP_STACK_HandlerDeregister functions are compiled in and can be used
    - If disabled, these functions do not exist and cannot be used/called 
    - Relevant only when TCPIP_STACK_USE_EVENT_NOTIFICATION is enabled
    - Default value: false
    - Symbol TCPIP_STACK_USER_NOTIFICATION

- **Enable Stack Deinitialize Operations:**
    - This setting enables the TCPIP_STACK_Deinitialize() operation
    - If this symbol is false, then the TCPIP_STACK_Deinitialize is not built in
    - Useful when stack stop and restart is not needed at run time - smaller code footprint for the TCP/IP stack.
    - Notes:
        - The TCP/IP stack can be initialized as usual (TCPIP_STACK_Initialize) but it cannot be stopped and restarted
        - None of the TCP/IP modules (except the MAC and PHY, see below) will have the corresponding Deinitialize function built in
        - If the stack initialization failed (i.e. the TCPIP_STACK_Initialize returned SYS_MODULE_OBJ_INVALID or TCPIP_STACK_Status returns SYS_STATUS_ERROR) the stack modules cannot be used and the application should not make any attempt to call their API!
    - Default value: true
    - Symbol TCPIP_STACK_DOWN_OPERATION

- **Enable IF Up/Down Operations:**
    - This setting enables the TCPIP_STACK_NetUp/TCPIP_STACK_NetDown operations
    - When enabled, these functions are built in and can be used by an app
    - Useful when interfaces do not need restarting at run time - results in smaller code footprint.
    - Note:
        - Combination  TCPIP_STACK_DOWN_OPERATION == false and  TCPIP_STACK_IF_UP_DOWN_OPERATION == true is invalid and must not be used otherwise the behavior is undefined!
    - Default value: true
    - Symbol TCPIP_STACK_IF_UP_DOWN_OPERATION

- **Enable MAC Down on Failure:**
    - This setting specifies the behavior of stack regarding the MAC and PHY drivers when the TCPIP_STACK_DOWN_OPERATION == false in the situation where the stack initialization failed and the stack cannot be started.
    - If true, the MAC (and the corresponding PHY) TCPIP_MAC_Deinitialize will be called.
        - This operation is supposed to exist and this setting will conserve power.
    - If false, the TCPIP_MAC_Deinitialize will not be called and the code footprint could be smaller.
        - The TCPIP_MAC_Deinitialize operation, which is expensive, could be unimplemented.
    - Note: the TCPIP_STACK_MAC_DOWN_OPERATION == false could be set only when TCPIP_STACK_DOWN_OPERATION == false! Otherwise the behavior is undefined!
    - Default value: true
    - Symbol TCPIP_STACK_MAC_DOWN_OPERATION

- **Enable Interface Address Change Signaling:**
    - This setting enables/disables the stack signaling when an interface address changes
    - This could be triggered by the DHCP kicking into action, by ZCLL or other address service, or simply by the user changing the IP address of an interface
    - If it is enabled, the socket modules (UDP, TCP) will be notified and they will disconnect their sockets that are currently connected on that interface that has changed.
    - User can be notified by registering an event handler either with socket events or with the general stack signaling mechanism
    - Default value: false
    - Symbol TCPIP_STACK_INTERFACE_CHANGE_SIGNALING

- **Enable Configuration Save/Restore Functionality:**
    - This setting enables the configuration get/set operations: TCPIP_STACK_ModuleConfigGet, TCPIP_STACK_NetConfigGet, TCPIP_STACK_NetConfigSet
    - If true, the functionality is built in and could be used by the application
    - If false, these functions do not exist and the generated code is smaller 
    - Default value: true
    - Symbol TCPIP_STACK_CONFIGURATION_SAVE_RESTORE

- **Number of entries in the secure port table:**
    -	The number of entries in the internally maintained secure port table
    -	This table is populated at stack initialization with default well-known port values
    -	Currently this number should be >= 10
    - Default value: 10
    - Symbol TCPIP_STACK_SECURE_PORT_ENTRIES

- **Enable Alias Interfaces Support:**
    - Enable support for alias interfaces
    - An alias interface uses the physical interface of another network interface.
    - It allows multiple IP addresses for a host on the same interface, for example.
    - Default value: false
    - Symbol TCPIP_STACK_ALIAS_INTERFACE_SUPPORT

- **Enable Packet Logger:**
    - Enable the packet logger
    - Simple module that logs the packets as they fly through the stack from creation until acknowledged
    - Note: the logger supports console commands for selecting logging modules/sockets
    - Default value: false
    - Symbol TCPIP_PACKET_LOG_ENABLE

- **Number of Entries in the Packet Logger:**
    - Number of log entries, if the packet logger is enabled
    - Each entry takes approx 24 bytes
    - Default value: 40
    - Symbol TCPIP_PKT_LOG_SIZE

- **Enable External Packet Processing:**
    - This setting enables/disables the processing of the RX packets by an external handler
    - The user of the stack can register an external function to process the incoming packets
    - If true, the functionality is built in and could be used by the application
    - If false, the functionality does not exist and the generated code is slightly smaller 
    - Default value: false
    - Symbol TCPIP_STACK_EXTERN_PACKET_PROCESS

- **Enable Stack Initialization Callback:**
    - This callback is part of the initialization data that is passed to the stack at initialization
    - If the callback is NULL, then the stack initialization will proceed immediately with the data presented in the TCPIP_STACK_INIT.
    -If the callback is not NULL, then the initialization of the stack will be delayed and it will be performed in the TCPIP_STACK_Task() function.
    - The TCPIP_STACK_Task() will keep calling the callback function until this returns ready or an error condition.
    - Default value: false
    - Symbol TCPIP_STACK_ENABLE_INIT_CALLBACK

- **Stack Initialization Callback Function:**
    - Name of the initialization callback function
    - Default value: "TCPIP_STACK_InitCallback"
    - Symbol TCPIP_STACK_INIT_CALLBACK

- **Number of Network interfaces:**
    - Number of Network interfaces that are part of the project
    - Default value: 1
    - Symbol TCPIP_STACK_NET_INTERFACE_NUM

- **Interface Name:**
    - Name of the network interface
    - Default value: ""
    - Symbol TCPIP_STACK_NET_INTERFACE_NAME_IDXn

- **Internal MAC?:**
    - Use the internally built in MAC for this interface
    - Default value: false
    - Symbol TCPIP_STACK_INT_MAC_IDXn

- **PHY Interface:**
    - The MAC <-> PHY interface to use for this network interface
    - Default value: ""
    - Symbol TCPIP_STACK_MII_MODE_IDXn

- **External Interface:**
    - Setting the network with an external interface
    - Default value: ""
    - Symbol TCPIP_STACK_OTH_INT_IDXn

