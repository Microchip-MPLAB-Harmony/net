# Manager Module

The TCP/IP manager provides access to the stack initialization, configuration and status functions.

It also process the packets received from different network interfaces \(MAC drivers\) and dispatches them based on their type to the proper higher\_layers of the stack.

![TCPIP MANAGER abstract](GUID-20E79F9A-D74D-458E-B381-F0D5B82B8AA3-low.png)

**How the Library Works**

The Manager module of the TCP/IP stack is the central module in the stack. It is this module that allows the client to configure and initialize the stack and also to get current status and parameters of the network interfaces that the stack supports.

The stack manager controls the communication to the underlying virtual MAC driver that implements the interface to a specific network channel. The various network interfaces that the TCP/IP stack supports are brought up or torn down by the stack manager in response to the stack client commands.

The manager is also responsible for providing the TCP/IP client with notifications of the events that occur in the stack at the network interface level. A dynamic registration mechanism is provided for this.

Another important functionality of the stack manager is the dispatch of the incoming network packets. The manager is notified by the underlying interface drivers of the traffic related event \(transmit, receive, errors, etc.\) As a response to those events the manager gets the pending receive packets from the virtual MAC driver and, based on their type, dispatches them to the appropriate higher level protocol for further processing \(IPv4, ARP, IPv6\).

As part of the event notification provided by the MAC driver the TCP/IP stack manager calls back into the MAC driver when this one needs processing time and can maintain statistics counters, link status, etc.

The MPLAB Harmony framework on the system running the TCP/IP stack interfaces to the stack manager mainly by using three functions, TCPIP\_STACK\_Initialize and TCPIP\_STACK\_Deinitialize for TCP/IP stack initialization, and TCPIP\_STACK\_Task for periodic processing and the advance of the stack state machine.

The stack is capable of running either under an Operating System \(RTOS\) or in a bare-metal environment. When running in a bare-metal environment a "cooperative multi-tasking" approach is needed by the TCP/IP stack, meaning that control has to reach the TCPIP\_STACK\_Task function. That is the system will need to make periodic calls to this function which allows the TCP/IP stack to dequeue incoming packets and the corresponding state machines in the stack to advance. The application has to be aware that it cannot monopolize the processor for long periods of time, and needs to relinquish control so that the system loops perform their tasks.

In an RTOS environment it is preferred to have the stack manager run in its own thread. This thread will be blocked waiting for network interface events and when such an event occurs it will wake up and process the data. This is a more efficient way of using the CPU resources.

Refer to MPLAB Harmony Framework Reference for information on configuring the MPLAB Harmony framework to run in a RTOS or bare-metal environment.

**Core Functionality**

The following diagram provides an overview for the operation and use of the Manager module in a typical MPLAB Harmony framework \(bare-metal, non-OS environment\).

![TCPIP MANAGER Flow](GUID-05DD21AD-E7CE-406D-957E-0E6FD80F8E52-low.png)

The TCP/IP stack is initialized as a part of the system initialization by a call to TCPIP\_STACK\_Initialize. By configuring the stack, the application can pass run-time initialization data to the stack to select both the network parameters of the interfaces \(network addresses, masks, physical addresses, NetBIOS names, DHCP operation, etc.\) and the parameters for the modules contained in the stack \(TCP, UDP, ARP, IPv4, IPv6, etc.\).

Once the stack is initialized the application can use the stack manager calls to inquire about the stack current settings \(current addresses obtained by DHCP or IPv6 advertisements messages, etc.\), to check the physical link status or to set new parameters to the stack.

It should be noted that different interfaces can be stopped and restarted dynamically \(TCPIP\_STACK\_NetDown, TCPIP\_STACK\_NetUp\) without the stack being killed. When a particular interface is brought up only the network parameters for that interface need to be supplied \(no parameters for the TCP/IP stack modules are needed\).

The entire stack can be stopped and restarted \(TCPIP\_STACK\_Deinitialize, TCPIP\_STACK\_Initialize\). Reinitializing the stack allows the stack initialization with a complete new set of network and/or module parameters.

The stack processing is done in the TCPIP\_STACK\_Task function. As explained in the previous paragraph, processing time is required by the stack, meaning that periodic calls have to be made by the system/framework to the TCPIP\_STACK\_Task function. When execution reaches this function the stack maintains and advances its internal state machine. The most important part of this is the processing of the events that are reported by the underlying MAC drivers \(through the use of the virtual MAC driver, which provides an interrupt signaling/event notification mechanism\). As a response to the MAC events, the stack manager will retrieve packets from the internal MAC receive queues and dispatch them for processing. It will also call into MAC processing functions if required and provide processing for the stack modules that are the recipients of the dispatched network packets. The stack manager will also notify the application of the network events if the application dynamically registered an event notification handler with the stack.

It should be noted that the stack manager also requires a periodic tick signal for which it uses the MPLAB Harmony system timer services \(see Timer System Service Library\).

If the application no longer uses the TCP/IP stack, it is important that it calls the TCPIP\_STACK\_Deinitialize function. This function closes any open network connections \(sockets, etc.\), closes the underlying MAC and PHY drivers and all of the modules in the stack, and frees all of the resources that were internally allocated by the stack as part of the initialization and run-time processing.

**Library Interface**

|Name|Description|
|----|-----------|
|**Macros**||
|TCPIP\_PACKET\_LOG\_ENABLE|Enable the packet logger Simple module that logs the packets as they fly through the stack from creation until acknowledged Default is disabled|
|TCPIP\_PKT\_LOG\_SIZE|Number of log entries, if the packet logger is enabled Each entry takes approx 24 bytes|
|TCPIP\_STACK\_ALIAS\_INTERFACE\_SUPPORT|Enable support for alias interfaces An alias interface uses the physical interface of another network interface. It allows multiple IP addresses for a host on the same interface, for example. Default is disabled.|
|TCPIP\_STACK\_CALLOC\_FUNC|Stack allocation function, calloc style This is the function the stack will call to allocate memory needed for its own heap: TCPIP\_STACK\_DRAM\_SIZE. Use standard C library 'calloc' as a default|
|TCPIP\_STACK\_COMMAND\_ENABLE|TCP/IP commands support - requires the system console and the command processor|
|TCPIP\_STACK\_CONFIGURATION\_SAVE\_RESTORE|This setting enables the configuration get/set operations: TCPIP\_STACK\_ModuleConfigGet, TCPIP\_STACK\_NetConfigGet, TCPIP\_STACK\_NetConfigSet If true, the functionality is built in and could be used by the application If false, these functions do not exist and the generated code is smaller|
|TCPIP\_STACK\_DOWN\_OPERATION|This setting enables the TCPIP\_STACK\_Deinitialize\(\) operation If this symbol is false, then the TCPIP\_STACK\_Deinitialize is not built in Useful when stack stop and restart is not needed at run time - smaller code footprint for the TCP/IP stack.|
|TCPIP\_STACK\_DRAM\_DEBUG\_ENABLE|Enable debugging of an allocation call that failed. If the system debug is enabled \(SYS\_DEBUG\_ENABLE\) the stack will issue a warning message at the system debug channel.|
|TCPIP\_STACK\_DRAM\_RUN\_LIMIT|The minimum amount of dynamic memory left for run time allocation by the stack \(IP, UDP, etc\) This is just a warning threshold. If after all the modules are initialized the amount of memory available in the TCPIP heap is less then TCPIP\_STACK\_DRAM\_RUN\_LIMIT then a warning will be displayed \(if the debug channel is enabled\) For proper operation there should be always a heap reserve of at least few KB.|
|TCPIP\_STACK\_DRAM\_SIZE|The total amount of internal dynamic memory heap that the TCPIP stack will create at start up. This is the size of the internal heap, relevant only for the TCPIP\_STACK\_USE\_INTERNAL\_HEAP|
|TCPIP\_STACK\_DRAM\_TRACE\_SLOTS|Enable tracing of the allocated memory by each module. The stack will trace all the memory allocated by a module and various statistics. \#define TCPIP\_STACK\_DRAM\_TRACE\_ENABLE Number of trace slots to be used. There is on slot needed per module that allocates memory from the heap.|
|TCPIP\_STACK\_FREE\_FUNC|Stack deallocation function, free style This is the function the stack will call for freeing the allocated memory when the stack is deinitialized. Use standard C library 'free' as a default|
|TCPIP\_STACK\_IF\_UP\_DOWN\_OPERATION|This setting enables the TCPIP\_STACK\_NetUp/TCPIP\_STACK\_NetDown operations When enabled, these functions are built in and can be used by an app Useful when interfaces do not need restarting at run time - results in smaller code footprint.|
|TCPIP\_STACK\_INTERFACE\_CHANGE\_SIGNALING|This setting enables/disables the stack signaling when an interface address changes This could be triggered by the DHCP kicking into action, by ZCLL or other address service, or simply by the user changing the IP address of an interface If it is enabled, the socket modules \(UDP, TCP\) will be notified and they will disconnect their sockets that are currently connected on that interface that has changed. User can be notified by registering an event handler either with socket events or with the general stack signaling mechanism Default setting is true|
|TCPIP\_STACK\_INTERNAL\_HEAP\_POOL\_EXPANSION\_SIZE|The size of the expansion heap for a TCPIP\_STACK\_USE\_INTERNAL\_HEAP\_POOL This is the amount of heap allocated besides the size resulting from the pool entries and allows expanding at run time. How a specific entry expands is controlled per entry If set to 0, then there is no expansion at run time.|
|TCPIP\_STACK\_MAC\_DOWN\_OPERATION|This setting specifies the behavior of stack regarding the MAC and PHY drivers when the TCPIP\_STACK\_DOWN\_OPERATION == false in the situation where the stack initialization failed and the stack cannot be started. If true, the MAC \(and the corresponding PHY\) TCPIP\_MAC\_Deinitialize will be called. This operation is supposed to exist and this setting will conserve power. If false, the TCPIP\_MAC\_Deinitialize waon't be called and the code footprint could be smaller. The TCPIP\_MAC\_Deinitialize operation, which is expensive, could be unimplemented.|
|TCPIP\_STACK\_MALLOC\_FUNC|Stack allocation function, malloc style This is the function the stack will call to allocate memory needed for its own heap: TCPIP\_STACK\_DRAM\_SIZE. Use standard C library 'malloc' as a default|
|TCPIP\_STACK\_SECURE\_PORT\_ENTRIES|The number of entries in the internally maintained secure port table This table is populated at stack initialization with default well-known port values Currently this number should be \>= 10|
|TCPIP\_STACK\_SUPPORTED\_HEAPS|Number of heap types at run time to enable debugging, statistics, trace on, etc. Currently only 1 heap type is supported at run time, one of: internal heap, internal pool and external|
|TCPIP\_STACK\_TICK\_RATE|TCPIP task processing rate, in milliseconds. The TCP/IP task will require a timer event with this rate for maintaining its own state machine, processing timeouts for all modules, or dispatching the RX traffic if the interrupt operation is not enabled, etc. The lower this value \(higher the frequency\) the higher the priority of the TCP/IP stack and a higher performance can be obtained.|
|TCPIP\_STACK\_USE\_ANNOUNCE|Microchip Embedded Ethernet Device Discoverer server/client|
|TCPIP\_STACK\_USE\_BERKELEY\_API|Berkeley Sockets APIs are available|
|TCPIP\_STACK\_USE\_DHCP\_CLIENT|Dynamic Host Configuration Protocol client for obtaining IP address and other parameters|
|TCPIP\_STACK\_USE\_DHCP\_SERVER|DHCP server|
|TCPIP\_STACK\_USE\_DNS|Domain Name Service Client for resolving hostname strings to IP addresses|
|TCPIP\_STACK\_USE\_DNS\_SERVER|Domain Name Service Server for redirection to the local device|
|TCPIP\_STACK\_USE\_EVENT\_NOTIFICATION|Event Notifications Options ======================================================================= This setting enables the stack event notification. It allows the MAC drivers to report their TX/RX related events to the stack manager but also allows users of the stack to register notification handler so that they are notified of specific events. The operation of the stack is more efficient when event notification is turned on and this is how the stack is designed to run. Always leave this setting defined. The choice for selecting this parameter will eventually be removed. Maintained for backward compatibility.|
|TCPIP\_STACK\_USE\_FTP\_SERVER|File Transfer Protocol|
|TCPIP\_STACK\_USE\_HTTP\_NET\_SERVER|HTTP NET server with POST, Cookies, Authentication, etc. and encryption support|
|TCPIP\_STACK\_USE\_ICMP\_CLIENT|Ping query capability|
|TCPIP\_STACK\_USE\_ICMP\_SERVER|Ping response capability|
|TCPIP\_STACK\_USE\_ICMPV6\_SERVER|enable IPv6 ICMP functionality|
|TCPIP\_STACK\_USE\_INTERNAL\_HEAP|define TCPIP\_STACK\_USE\_EXTERNAL\_HEAP define TCPIP\_STACK\_USE\_INTERNAL\_HEAP\_POOL|
|TCPIP\_STACK\_USE\_IPERF|Enable the Iperf module for standard network benchmarking|
|TCPIP\_STACK\_USE\_IPV4|enable IPv4 functionality|
|TCPIP\_STACK\_USE\_IPV6|enable IPv6 functionality|
|TCPIP\_STACK\_USE\_LLDP|LLDP limited beta functionality - not supported through MHC configuration|
|TCPIP\_STACK\_USE\_NBNS|NetBIOS Name Service Server for responding to NBNS hostname broadcast queries|
|TCPIP\_STACK\_USE\_REBOOT\_SERVER|Module for resetting this PIC remotely. Primarily useful for a Bootloader.|
|TCPIP\_STACK\_USE\_SMTPC|Simple Mail Transfer Protocol for sending email with encryption support|
|TCPIP\_STACK\_USE\_SNMP\_SERVER|Simple Network Management Protocol v2C Community Agent|
|TCPIP\_STACK\_USE\_SNMPV3\_SERVER|SNMP v3 agent|
|TCPIP\_STACK\_USE\_SNTP\_CLIENT|Simple Network Time Protocol for obtaining current date/time from Internet|
|TCPIP\_STACK\_USE\_TCP|Enable the TCP module|
|TCPIP\_STACK\_USE\_TELNET\_SERVER|Telnet server|
|TCPIP\_STACK\_USE\_TFTP\_CLIENT|TFTP client functionality|
|TCPIP\_STACK\_USE\_UDP|Enable the UDP module|
|TCPIP\_STACK\_USE\_ZEROCONF\_LINK\_LOCAL|Zeroconf IPv4 Link-Local Addressing;|
|TCPIP\_STACK\_USE\_ZEROCONF\_MDNS\_SD|Zeroconf mDNS and mDNS service discovery|
|TCPIP\_STACK\_USER\_NOTIFICATION|This setting enables the reporting of the events by the stack to the user using the notification system If enabled, then TCPIP\_STACK\_HandlerRegister and TCPIP\_STACK\_HandlerDeregister functions are compiled in and can be used If disabled, these functions do not exist and cannot be used/called Relevant only when TCPIP\_STACK\_USE\_EVENT\_NOTIFICATION is enabled|
|TCPIP\_STACK\_EXTERN\_PACKET\_PROCESS|This setting enables/disables the processing of the RX packets by an external handler The user of the stack can register an external function to process the incoming packets If true, the functionality is built in and could be used by the application If false, the functionality does not exist and the generated code is slightly smaller|
|**Helper Functions**||
|TCPIP\_Helper\_FormatNetBIOSName|Formats a string to a valid NetBIOS name.|
|TCPIP\_Helper\_htonl|This is function TCPIP\_Helper\_htonl.|
|TCPIP\_Helper\_htonll|This is function TCPIP\_Helper\_htonll.|
|TCPIP\_Helper\_htons|This is function TCPIP\_Helper\_htons.|
|TCPIP\_Helper\_IPAddressToString|Converts an IPV4 address to an ASCII string.|
|TCPIP\_Helper\_IPv6AddressToString|Converts an IPv6 address to a string representation.|
|TCPIP\_Helper\_IsBcastAddress|This is function TCPIP\_Helper\_IsBcastAddress.|
|TCPIP\_Helper\_IsMcastAddress|This is function TCPIP\_Helper\_IsMcastAddress.|
|TCPIP\_Helper\_IsPrivateAddress|Detects a private \(non-routable\) address.|
|TCPIP\_Helper\_MACAddressToString|Converts a MAC address to a string.|
|TCPIP\_Helper\_ntohl|!defined\(\_*mips\_*\)|
|TCPIP\_Helper\_ntohll|This is function TCPIP\_Helper\_ntohll.|
|TCPIP\_Helper\_ntohs|This is function TCPIP\_Helper\_ntohs.|
|TCPIP\_Helper\_SecurePortGetByIndex|Returns the secure port belonging to a specified index.|
|TCPIP\_Helper\_SecurePortSet|Sets the required port secure connection status.|
|TCPIP\_Helper\_StringToIPAddress|Converts an ASCII string to an IPV4 address.|
|TCPIP\_Helper\_StringToIPv6Address|Converts a string to an IPv6 address.|
|TCPIP\_Helper\_StringToMACAddress|Converts a string to an MAC address.|
|TCPIP\_Helper\_TCPSecurePortGet|Checks if the required TCP port is a secure port.|
|TCPIP\_Helper\_UDPSecurePortGet|Checks if the required UDP port is a secure port.|
|TCPIP\_Helper\_Base64Decode|Helper function to decode a Base-64 encoded array.|
|TCPIP\_Helper\_Base64Encode|Helper function to encode to Base-64.|
|**Task and Initialize Functions**||
|TCPIP\_STACK\_HandlerDeregister|Deregisters an event notification handler.|
|TCPIP\_STACK\_HandlerRegister|Sets a new event notification handler.|
|TCPIP\_MODULE\_SignalFunctionDeregister|Deregisters a signal function for a stack module.|
|TCPIP\_MODULE\_SignalFunctionRegister|Registers a new signal function for a stack module.|
|TCPIP\_MODULE\_SignalGet|Returns the current signals for a TCP/IP module.|
|TCPIP\_STACK\_VersionGet|Gets the TCP/IP stack version in numerical format.|
|TCPIP\_STACK\_VersionStrGet|Gets the TCP/IP stack version in string format.|
|TCPIP\_STACK\_Task|TCP/IP Stack task function.|
|TCPIP\_STACK\_Deinitialize|Stack deinitialization function.|
|TCPIP\_STACK\_Initialize|Stack initialization function.|
|TCPIP\_STACK\_MACObjectGet|Returns the network MAC driver object of this interface.|
|TCPIP\_STACK\_PacketHandlerDeregister|Deregisters a previously registered packet handler.|
|TCPIP\_STACK\_PacketHandlerRegister|Sets a new packet processing handler.|
|**Network Status and Control Functions**||
|TCPIP\_STACK\_IndexToNet|Network interface handle from interface number.|
|TCPIP\_STACK\_NetBIOSName|Network interface NetBIOS name.|
|TCPIP\_STACK\_NetBiosNameSet|Sets network interface NetBIOS name.|
|TCPIP\_STACK\_NetDefaultGet|Default network interface handle.|
|TCPIP\_STACK\_NetDefaultSet|Sets the default network interface handle.|
|TCPIP\_STACK\_NetHandleGet|Network interface handle from a name.|
|TCPIP\_STACK\_NetIndexGet|Network interface number from interface handle.|
|TCPIP\_STACK\_NetMask|Network interface IPv4 address mask.|
|TCPIP\_STACK\_NetNameGet|Network interface name from a handle.|
|TCPIP\_STACK\_NumberOfNetworksGet|Number of network interfaces in the stack.|
|TCPIP\_STACK\_EventsPendingGet|Returns the currently pending events.|
|TCPIP\_STACK\_ModuleConfigGet|Get stack module configuration data.|
|TCPIP\_STACK\_NetMACIdGet|Get the MAC ID of the network interface.|
|TCPIP\_STACK\_NetMACStatisticsGet|Get the MAC statistics data.|
|TCPIP\_STACK\_InitializeDataGet|Get the TCP/IP stack initialization data.|
|TCPIP\_STACK\_NetMulticastGet|Default multicast network interface handle.|
|TCPIP\_STACK\_NetMulticastSet|Sets the default multicast network interface.|
|TCPIP\_STACK\_NetGetType|Gets the network interface type: primary, alias, etc.|
|**Network Up/Down/Linked Functions**||
|TCPIP\_STACK\_NetDown|Turns down a network interface.|
|TCPIP\_STACK\_NetIsLinked|Gets the network interface link status.|
|TCPIP\_STACK\_NetIsUp|Gets the network interface up or down status.|
|TCPIP\_STACK\_NetUp|Turns up a network interface. As part of this process, the corresponding MAC driver is initialized.|
|TCPIP\_STACK\_NetAliasNameGet|Network interface alias name from a handle.|
|TCPIP\_STACK\_NetIsReady|Gets the network interface configuration status.|
|TCPIP\_STACK\_NetMACRegisterStatisticsGet|Get the MAC statistics register data.|
|**Network Address Status and Control Functions**||
|TCPIP\_STACK\_NetAddress|Network interface IPv4 address.|
|TCPIP\_STACK\_NetAddressBcast|Network interface broadcast address.|
|TCPIP\_STACK\_NetAddressDnsPrimary|Network interface DNS address.|
|TCPIP\_STACK\_NetAddressDnsPrimarySet|Sets network interface IPv4 DNS address.|
|TCPIP\_STACK\_NetAddressDnsSecond|Network interface secondary DNS address.|
|TCPIP\_STACK\_NetAddressDnsSecondSet|Sets network interface IPv4 secondary DNS address.|
|TCPIP\_STACK\_NetAddressGateway|Network interface IPv4 gateway address.|
|TCPIP\_STACK\_NetAddressGatewaySet|Sets network interface IPv4 gateway address.|
|TCPIP\_STACK\_NetAddressMac|Network interface MAC address.|
|TCPIP\_STACK\_NetAddressSet|Sets network interface IPv4 address.|
|TCPIP\_STACK\_NetIPv6AddressGet|Gets network interface IPv6 address.|
|TCPIP\_STACK\_NetAddressMacSet|Sets network interface MAC address.|
|TCPIP\_STACK\_Status|Provides the current status of the TCPIP stack module.|
|**Network Structure Storage Functions**||
|TCPIP\_STACK\_NetConfigGet|Get stack network interface configuration data.|
|TCPIP\_STACK\_NetConfigSet|Restores stack network interface configuration data.|
|**Heap Functions**||
|TCPIP\_STACK\_HEAP\_FreeSize|Returns the size of the space currently available in the heap.|
|TCPIP\_STACK\_HEAP\_LastError|Returns the last error encountered in a heap operation and clears the value of the last error variable.|
|TCPIP\_STACK\_HEAP\_MaxSize|Returns the maximum size of a block that can be currently allocated from this heap.|
|TCPIP\_STACK\_HEAP\_Size|Returns the size of the heap.|
|TCPIP\_STACK\_HeapHandleGet|Returns the current TCP/IP stack heap handle.|
|TCPIP\_STACK\_HEAP\_HighWatermark|Returns the current heap watermark.|
|**Data Types and Constants**||
|TCPIP\_NET\_HANDLE|Defines a network interface handle.|
|TCPIP\_EVENT|Defines the possible TCPIP event types.|
|TCPIP\_EVENT\_HANDLE|Defines a TCPIP stack event handle.|
|TCPIP\_STACK\_EVENT\_HANDLER|Pointer to a function\(handler\) that will get called to process an event.|
|TCPIP\_MODULE\_SIGNAL|Lists the signals that are generated by the TCP/IP stack manager and processed by the stack modules.|
|TCPIP\_MODULE\_SIGNAL\_FUNC|Pointer to a function\(handler\) that will get called when a stack internal signal occurred.|
|TCPIP\_MODULE\_SIGNAL\_HANDLE|Defines a TCPIP stack signal function handle.|
|TCPIP\_Helper\_ntohl|This is macro TCPIP\_Helper\_ntohl.|
|TCPIP\_Helper\_ntohll|This is macro TCPIP\_Helper\_ntohll.|
|TCPIP\_Helper\_ntohs|This is macro TCPIP\_Helper\_ntohs.|
|\_*TCPIP\_HELPERS\_H*|This is macro \_*TCPIP\_HELPERS\_H*.|
|\_*TCPIP\_MANAGER\_H*|This is macro \_*TCPIP\_MANAGER\_H*.|
|TCPIP\_NETWORK\_TYPE|Returns the type of a network interface.|
|TCPIP\_STACK\_HEAP\_CONFIG|Defines the data required to initialize the TCP/IP stack heap.|
|TCPIP\_STACK\_HEAP\_EXTERNAL\_CONFIG|Defines the data required to initialize the TCP/IP stack external heap.|
|TCPIP\_STACK\_HEAP\_FLAGS|Defines the flags supported by a TCP/IP heap.|
|TCPIP\_STACK\_HEAP\_HANDLE|Defines a TCP/IP stack heap handle type.|
|TCPIP\_STACK\_HEAP\_INTERNAL\_CONFIG|Defines the data required to initialize the TCP/IP stack internal heap.|
|TCPIP\_STACK\_HEAP\_POOL\_CONFIG|This is type TCPIP\_STACK\_HEAP\_POOL\_CONFIG.|
|TCPIP\_STACK\_HEAP\_POOL\_ENTRY|Defines the data required to initialize the TCP/IP stack internal memory pool.|
|TCPIP\_STACK\_HEAP\_RES|Defines the results of a TCP/IP heap error function.|
|TCPIP\_STACK\_HEAP\_TYPE|Defines a TCP/IP stack heap type.|
|TCPIP\_STACK\_HEAP\_USAGE|Defines a TCP/IP stack heap purpose.|
|TCPIP\_STACK\_PACKET\_HANDLER|Pointer to a function\(handler\) that will get called to process an incoming packet.|
|TCPIP\_STACK\_PROCESS\_HANDLE|Defines a TCPIP stack packet processing handle.|

-   **[\_TCPIP\_HELPERS\_H Macro](GUID-2F8F95F8-C947-4540-BD50-952613FACF05.md)**  

-   **[\_TCPIP\_MANAGER\_H Macro](GUID-46322265-3A95-4FFC-8EFE-BC410F8C8A31.md)**  

-   **[TCPIP\_NET\_HANDLE Type](GUID-283B4B5C-2894-4C43-B264-436B46EA69F0.md)**  

-   **[TCPIP\_EVENT Enumeration](GUID-A75996C6-067E-4462-A86A-4EC05528DC77.md)**  

-   **[TCPIP\_Helper\_Base64Decode Function](GUID-694C6E9D-E570-4398-94F5-B49A04A8C96C.md)**  

-   **[TCPIP\_Helper\_Base64Encode Function](GUID-FCA5F18C-18CF-436D-9050-AD57B705A8E9.md)**  

-   **[TCPIP\_Helper\_FormatNetBIOSName Function](GUID-62EC969A-A8DA-490C-9CA3-8CDB5E41D08F.md)**  

-   **[TCPIP\_Helper\_htonl Function](GUID-18319F8A-E30C-4BD0-B85B-F66B70C1EB51.md)**  

-   **[TCPIP\_Helper\_htonll Function](GUID-DFB63280-015A-471A-9A04-C11C47F9A6A5.md)**  

-   **[TCPIP\_Helper\_htons Function](GUID-1A4F0B14-A85A-4C9F-AC0A-D688F09C7E86.md)**  

-   **[TCPIP\_Helper\_IPAddressToString Function](GUID-55B68821-A274-44F0-B2D9-9C12D85F6F51.md)**  

-   **[TCPIP\_Helper\_IPv6AddressToString Function](GUID-24FC9E41-3466-48F6-A06D-D8EE58E41449.md)**  

-   **[TCPIP\_Helper\_IsBcastAddress Function](GUID-E685F7FD-F66F-46C5-AA31-23417CB67780.md)**  

-   **[TCPIP\_Helper\_IsMcastAddress Function](GUID-A1DC8B70-9EAD-48F3-B86B-F8BF2AC71585.md)**  

-   **[TCPIP\_Helper\_IsPrivateAddress Function](GUID-F0687E5F-50D9-4E2D-8281-39B30D488230.md)**  

-   **[TCPIP\_Helper\_MACAddressToString Function](GUID-055CF601-37B7-43DF-AA7D-6A55C687FE49.md)**  

-   **[TCPIP\_Helper\_ntohl Macro](GUID-990162EC-DD14-462F-8336-B5D2B089AE87.md)**  

-   **[TCPIP\_Helper\_ntohl Function](GUID-B4D41F33-BB5D-4A3F-A2FF-4AEA0FEB2372.md)**  

-   **[TCPIP\_Helper\_ntohll Function](GUID-261D3A77-3851-4DA2-8CD7-5E5420F6E71F.md)**  

-   **[TCPIP\_Helper\_ntohll Macro](GUID-05D1A6F5-39B8-4726-B6AA-C20DB2C696AE.md)**  

-   **[TCPIP\_Helper\_ntohs Function](GUID-94B64443-29E3-4B31-98FF-279E1626C22C.md)**  

-   **[TCPIP\_Helper\_htohs Macro](GUID-DA0AF76A-1428-4889-AAAF-21FE2A10B2FC.md)**  

-   **[TCPIP\_Helper\_SecurePortGetByIndex Function](GUID-91AA78F3-377B-479E-8B41-BEDF662C201C.md)**  

-   **[TCPIP\_Helper\_SecurePortSet Function](GUID-7283A146-83D0-485D-9A58-307266046894.md)**  

-   **[TCPIP\_Helper\_StringToIPAddress Function](GUID-FC9871E4-046B-4E18-B026-4B0237848D30.md)**  

-   **[TCPIP\_Helper\_StringToIPv6Address Function](GUID-31B65FA8-3006-400B-8C92-CDEDE2B938FC.md)**  

-   **[TCPIP\_Helper\_StringToMACAddress Function](GUID-7798753E-5953-45DE-9E0F-FF6EA043E124.md)**  

-   **[TCPIP\_Helper\_TCPSecurePortGet Function](GUID-A1BE117B-3066-4131-A622-A1018B8AEA86.md)**  

-   **[TCPIP\_Helper\_UDPSecurePortGet Function](GUID-5358F353-839E-4D94-B6D9-FACF57A53EB2.md)**  

-   **[TCPIP\_MODULE\_SIGNAL Enumeration](GUID-B320800F-2E77-4D0A-BABE-36E417E0EC71.md)**  

-   **[TCPIP\_MODULE\_SIGNAL\_FUNC Type](GUID-DB7FF793-16EC-4766-8C2F-F53C44BD44A9.md)**  

-   **[TCPIP\_MODULE\_SIGNAL\_HANDLE Type](GUID-751A65BB-A8A4-41BB-8888-6F92E2B6D511.md)**  

-   **[TCPIP\_MODULE\_SignalFunctionDeregister Function](GUID-D4D7486A-7AD8-44E9-874D-F8569A499FE4.md)**  

-   **[TCPIP\_MODULE\_SignalFunctionRegister Function](GUID-E10CF207-A406-40AA-AC13-A6C69CF32285.md)**  

-   **[TCPIP\_MODULE\_SignalGet Function](GUID-74B3E36D-F31D-4FF9-81AE-F536511AFB2D.md)**  

-   **[TCPIP\_NET\_HANDLE Type](GUID-C89ABCA4-B0CE-4510-9C1D-A0E9A15DD1C5.md)**  

-   **[TCPIP\_NETWORK\_TYPE Enumeration](GUID-90D1F0E4-81C0-43A8-9E5D-099F5E63C835.md)**  

-   **[TCPIP\_STACK\_Deinitialize Function](GUID-55FA4C69-CFF5-4C45-9269-E64A9F995E5C.md)**  

-   **[TCPIP\_STACK\_EVENT\_HANDLER Type](GUID-AA215625-E4A7-4921-8AF1-6B318699201A.md)**  

-   **[TCPIP\_STACK\_EventsPendingGet Function](GUID-E4B2C61A-89A3-4D7B-A7B1-A35443FD57AE.md)**  

-   **[TCPIP\_STACK\_HandlerDeregister Function](GUID-6A97CE3B-076A-4EDE-B53D-E8C62ED03406.md)**  

-   **[TCPIP\_STACK\_HandlerRegister Function](GUID-DCEBAF5C-5CC2-44B7-AB7C-CEA8D11A491D.md)**  

-   **[TCPIP\_STACK\_HEAP\_CONFIG Structure](GUID-06EB7E2C-E72B-4633-94E2-B8330020A036.md)**  

-   **[TCPIP\_STACK\_HEAP\_EXTERNAL\_CONFIG Structure](GUID-A3B3EFA8-4DBC-46CC-BEB1-896D4B5BF9F2.md)**  

-   **[TCPIP\_STACK\_HEAP\_FLAGS Enumeration](GUID-DBD0596C-894B-4108-964E-E51E43D9EF22.md)**  

-   **[TCPIP\_STACK\_HEAP\_FreeSize Function](GUID-75EF7F93-F4BC-4BED-B58A-80FA9E8FD2E0.md)**  

-   **[TCPIP\_STACK\_HEAP\_HANDLE Type](GUID-D287E269-82F2-43B8-A149-F8D3BDCC32AB.md)**  

-   **[TCPIP\_STACK\_HEAP\_HighWatermark Function](GUID-8E0841B0-D052-4CCA-BD91-6A04641B4432.md)**  

-   **[TCPIP\_STACK\_HEAP\_INTERNAL\_CONFIG Structure](GUID-7FD0C77D-7855-4FC0-A2CA-96ADBBC79CCD.md)**  

-   **[TCPIP\_STACK\_HEAP\_LastError Function](GUID-488FF282-E9CC-4548-8140-E3C76AEBEF61.md)**  

-   **[TCPIP\_STACK\_HEAP\_MaxSize Function](GUID-869E8FAE-90E5-4359-BF3E-75605CA82E73.md)**  

-   **[TCPIP\_STACK\_HEAP\_POOL\_CONFIG Structure](GUID-1898B92C-FB85-462A-A0E7-17213981E7E5.md)**  

-   **[TCPIP\_STACK\_HEAP\_POOL\_ENTRY Structure](GUID-8A824AA3-4E4C-4C37-9500-79FC5B8D6BF4.md)**  

-   **[TCPIP\_STACK\_HEAP\_RES Enumeration](GUID-3447E5F3-4B4F-4BCB-B899-C7DC2903D957.md)**  

-   **[TCPIP\_STACK\_HEAP\_Size Function](GUID-8DA21A5F-888A-4335-991A-63C8DBA52DF6.md)**  

-   **[TCPIP\_STACK\_HEAP\_TYPE Enumeration](GUID-6DF9F69D-E6E7-4570-B1C2-110CCB0395EE.md)**  

-   **[TCPIP\_STACK\_HEAP\_USAGE Enumeration](GUID-B8F99ECD-7F8D-4D3F-84DF-6286DEBC968B.md)**  

-   **[TCPIP\_STACK\_HeapHandleGet Function](GUID-5963FE64-3A2B-4A3C-BD88-77A533DF882B.md)**  

-   **[TCPIP\_STACK\_IndexToNet Function](GUID-E938DC01-4BA2-4CD7-B16F-BDA586847122.md)**  

-   **[TCPIP\_STACK\_Initialize Function](GUID-715E33FC-50B7-4837-AFB1-55BCD0D15884.md)**  

-   **[TCPIP\_STACK\_InitializeDataGet Function](GUID-D76B51D2-8422-4DEA-BA25-6106A4C02D7A.md)**  

-   **[TCPIP\_STACK\_MACObjectGet Function](GUID-E5FEA58A-6877-488F-A347-63728F777CDC.md)**  

-   **[TCPIP\_STACK\_ModuleConfigGet Function](GUID-C720F980-BF70-46B0-A03D-727DF79A7714.md)**  

-   **[TCPIP\_STACK\_NetAddress Function](GUID-92879E40-8A1F-42B2-A9AB-C0FF32184F2A.md)**  

-   **[TCPIP\_STACK\_NetAddressBcast Function](GUID-B7D9F946-C7E1-4054-B8FF-30934B80350E.md)**  

-   **[TCPIP\_STACK\_NetAddressDnsPrimary Function](GUID-0848A8D1-F2A5-4B53-94C1-1E78F7FB5E24.md)**  

-   **[TCPIP\_STACK\_NetAddressDnsPrimarySet Function](GUID-6A8B96F2-9F5F-4D1C-A35D-0B3EF3B2FC70.md)**  

-   **[TCPIP\_STACK\_NetAddressDnsSecond Function](GUID-E7A44303-FFAE-49EC-B989-02201197DE9E.md)**  

-   **[TCPIP\_STACK\_NetAddressDnsSecondSet Function](GUID-B5EBA2EA-F93C-474F-B24A-8FB0591EDE55.md)**  

-   **[TCPIP\_STACK\_NetAddressGateway Function](GUID-4D827C17-9623-4A0D-B3B2-9D58016C4859.md)**  

-   **[TCPIP\_STACK\_NetAddressGatewaySet Function](GUID-C192D8F2-C86F-4CFE-AFF7-FB63CBB34A78.md)**  

-   **[TCPIP\_STACK\_NetAddressMac Function](GUID-41573635-4285-44CF-9670-BFC3104F657B.md)**  

-   **[TCPIP\_STACK\_NetAddressMacSet Function](GUID-7B19DEA3-70DD-4DD6-8A58-89A6C62B494F.md)**  

-   **[TCPIP\_STACK\_NetAddressSet Function](GUID-81D408B7-8548-4EC8-A955-C5B136C1D921.md)**  

-   **[TCPIP\_STACK\_NetAliasNameGet Function](GUID-079F091B-9D66-4E75-8600-092EDC9EB19E.md)**  

-   **[TCPIP\_STACK\_NetBIOSName Function](GUID-EDB7B286-21C6-4C78-A40C-3F49946A933E.md)**  

-   **[TCPIP\_STACK\_NetBiosNameSet Function](GUID-81B09C27-C145-4FB9-9025-F5986566841D.md)**  

-   **[TCPIP\_STACK\_NetConfigGet Function](GUID-6C4EA2C1-CD13-481B-ACBF-2EB77E7CFF3D.md)**  

-   **[TCPIP\_STACK\_NetConfigSet Function](GUID-7C3A21A5-3499-4AE5-A6B0-63F9D83F2A17.md)**  

-   **[TCPIP\_STACK\_NetDefaultGet Function](GUID-EF46A6E9-E73E-4228-BD7E-FFFF27B07096.md)**  

-   **[TCPIP\_STACK\_NetDefaultSet Function](GUID-5A16EA54-44B2-4444-97F0-7BF4D756E430.md)**  

-   **[TCPIP\_STACK\_NetDown Function](GUID-142C96D7-5850-49D7-A472-A6476E209175.md)**  

-   **[TCPIP\_STACK\_NetGetType Function](GUID-1063D7A8-FA74-4F24-815B-FB44CC5F88BF.md)**  

-   **[TCPIP\_STACK\_NetHandleGet Function](GUID-DDD32672-28E9-4E86-9C2E-4FC04877C4E9.md)**  

-   **[TCPIP\_STACK\_NetIndexGet Function](GUID-B183E319-7BDA-4453-8677-7D5BD046FB86.md)**  

-   **[TCPIP\_STACK\_NetIPv6AddressGet Function](GUID-227A05EB-3744-4516-8DAD-E3BCC78691B5.md)**  

-   **[TCPIP\_STACK\_NetIsLinked Function](GUID-234477DB-5BB9-4A5D-8765-9B295575F9D8.md)**  

-   **[TCPIP\_STACK\_NetIsReady Function](GUID-3327D2C7-8C69-48E0-817A-65C7282FFFDF.md)**  

-   **[TCPIP\_STACK\_NetIsUp Function](GUID-703BC1FC-C9DB-4AF8-B1B8-E4457C8311B9.md)**  

-   **[TCPIP\_STACK\_NetMACIdGet Function](GUID-780F540D-D266-4F0B-BC42-09955B80F4A4.md)**  

-   **[TCPIP\_STACK\_NetMACRegisterStatisticsGet Function](GUID-696B13AA-788B-45E4-A916-8EAC98896853.md)**  

-   **[TCPIP\_STACK\_NetMACStatisticsGet Function](GUID-469981DB-5DBF-4E8B-BFFE-6EB386F32BB2.md)**  

-   **[TCPIP\_STACK\_NetMask Function](GUID-26EDA78F-3A2B-491C-8C31-41635D1D08F5.md)**  

-   **[TCPIP\_STACK\_NetMulticastGet Function](GUID-6003DBC5-C656-4E47-B6C5-EBE5B35226E7.md)**  

-   **[TCPIP\_STACK\_NetMulticastSet Function](GUID-9D96E9B8-75C1-40F1-B918-C38B500E2B46.md)**  

-   **[TCPIP\_STACK\_NetNameGet Function](GUID-8A346581-6B4A-4831-9E5E-4E33235E3036.md)**  

-   **[TCPIP\_STACK\_NetUp Function](GUID-FC4974C8-56F6-430F-AE98-9409FDE066CF.md)**  

-   **[TCPIP\_STACK\_NumberOfNetworksGet Function](GUID-A8B35265-E439-4CEB-94F0-55E2DAF9668D.md)**  

-   **[TCPIP\_STACK\_PACKET\_HANDLER Type](GUID-82356347-DE35-4FC5-93A2-98DC156891F0.md)**  

-   **[TCPIP\_STACK\_PacketHandlerDeregister Function](GUID-6D26931C-538F-4F1F-80A6-2CB1DE1D59F1.md)**  

-   **[TCPIP\_STACK\_PacketHandlerRegister Function](GUID-192B7F3D-20C7-485D-874D-0407D3628458.md)**  

-   **[TCPIP\_STACK\_PROCESS\_HANDLE Type](GUID-A35D9196-ED2D-4A09-B26D-F3B68ABDA206.md)**  

-   **[TCPIP\_STACK\_Status Function](GUID-F837EC61-F4E6-458F-8B87-ED8158565C14.md)**  

-   **[TCPIP\_STACK\_Task Function](GUID-AC14B6B7-E080-427F-8265-8E58085BCD67.md)**  

-   **[TCPIP\_STACK\_VersionGet Function](GUID-91F02427-3FB3-4759-9D6F-511B88EE8C32.md)**  

-   **[TCPIP\_STACK\_VersionStrGet Function](GUID-C6AE4B40-72A3-409B-9BE4-B8D555302C85.md)**  


**Parent topic:**[MPLAB® Harmony TCP/IP Library](GUID-01A0A1D8-EC9B-4EFF-B8E4-D154B555FEF2.md)

