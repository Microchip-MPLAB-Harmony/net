/*******************************************************************************
Microchip TCP/IP Stack Demo Application Configuration Header

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_config.h

  Summary:
    TCP/IP Commands configuration file
    
  Description:
    This file contains the TCP/IP commands configuration options

*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2012 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED ?AS IS?WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
// DOM-IGNORE-END
#ifndef __TCPIP_CONFIG_H_
#define __TCPIP_CONFIG_H_

// =======================================================================
//   Stack Configuration/Build Options
// =======================================================================

// TCPIP Stack Module Selection
//   Uncomment or comment the following lines to enable or
//   disabled the following high-level application modules.

#define TCPIP_STACK_USE_IPV4                // enable IPv4 functionality
#define TCPIP_STACK_USE_ICMP_SERVER         // Ping query and response capability
#define TCPIP_STACK_USE_HTTP_SERVER        // HTTP server with POST, Cookies, Authentication, etc.
#define TCPIP_STACK_USE_DHCP_CLIENT         // Dynamic Host Configuration Protocol client for obtaining IP address and other parameters
//#define TCPIP_STACK_USE_SMTP_CLIENT       // Simple Mail Transfer Protocol for sending email
#define TCPIP_STACK_USE_TELNET_SERVER       // Telnet server
#define TCPIP_STACK_USE_ANNOUNCE            // Microchip Embedded Ethernet Device Discoverer server/client
#define TCPIP_STACK_USE_DNS                 // Domain Name Service Client for resolving hostname strings to IP addresses
#define TCPIP_STACK_USE_DNS_SERVER          // Domain Name Service Server for redirection to the local device
#define TCPIP_STACK_USE_NBNS                // NetBIOS Name Service Server for responding to NBNS hostname broadcast queries
//#define TCPIP_STACK_USE_REBOOT_SERVER     // Module for resetting this PIC remotely.  Primarily useful for a Bootloader.
#define TCPIP_STACK_USE_SNTP_CLIENT         // Simple Network Time Protocol for obtaining current date/time from Internet
//#define TCPIP_STACK_USE_DYNAMICDNS_CLIENT  // Dynamic DNS client updater module
//#define TCPIP_STACK_USE_BERKELEY_API       // Berkeley Sockets APIs are available
#define TCPIP_STACK_USE_IPV6                // enable IPv6 functionality
#define TCPIP_STACK_USE_TCP                 // Enable the TCP module
#define TCPIP_STACK_USE_UDP                 // Enable the UDP module
#define TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL // Zeroconf IPv4 Link-Local Addressing;
#define TCPIP_STACK_USE_ZEROCONF_MDNS_SD    // Zeroconf mDNS and mDNS service discovery

#define TCPIP_STACK_COMMAND_ENABLE          // TCPIP_COMMANDS for network configuration or debug
#define TCPIP_STACK_USE_IPERF               // Enable the Iperf module for standard network benchmarking
//#define TCPIP_STACK_USE_SNMP_SERVER        // Simple Network Management Protocol v2C Community Agent
//#define TCPIP_STACK_USE_SNMPV3_SERVER      // SNMP v3 agent
#define TCPIP_STACK_USE_FTP_SERVER          // File Transfer Protocol
#define TCPIP_STACK_USE_DHCP_SERVER         // DHCP server

// =======================================================================
//   Dynamic memory allocation Options
// =======================================================================

// Type of heap that the TCP/IP stack uses: internal heap, internal pool or external
// This is a private TCP/IP heap and is used by internal stack modules that need dynamic memory
// (and most of them do): MAC buffers/packets, TCP and UDP buffers, etc.
//
// Note:
//      - The internal heap provides reasonably fast allocation using a first fit algorithm
//        So its allocation is memory efficient but can suffer of fragmentation issues.
//      - The internal pool is very fast but works with fixed sizes.
//        So the allocation may be not very memory efficient but there is no fragmentation.
//        The pool creates blocks with different sizes that need to be defined separately.
//        The initial pool configuration may be difficult.
//        This pool works best when the pool entries can be increased at run time with new allocations.
//      - The external heap relies on allocation functions provided to the stack
//        (like the standard C malloc/calloc/free functions)
#define TCPIP_STACK_USE_INTERNAL_HEAP
//#define TCPIP_STACK_USE_EXTERNAL_HEAP
//#define TCPIP_STACK_USE_INTERNAL_HEAP_POOL

// The total amount of internal dynamic memory heap that the TCPIP stack will create at start up.
// This is the size of the internal heap, relevant only for the TCPIP_STACK_USE_INTERNAL_HEAP
// Note: an amount of memory is used for internal heap structure.
// So the actual size of the created heap will be smaller than this.
// Note:  the settings in other modules configuration parameters
// (especially the MAC driver, TCP, UDP, HTTP, etc.) should be consistent
// with the total amount of heap that's allocated to the stack.
// Note: this parameter should also be consistent with the project linker
// heap setting ( TCPIP_STACK_DRAM_SIZE < project heap)
// Note: the above notes apply for the TCPIP_STACK_USE_INTERNAL_HEAP_POOL selection too.
// However, for the pool heap this symbol is not used because
// the total memory that results from adding up all the pool entries
// plus the storage needed for internal heap structures.
#define TCPIP_STACK_DRAM_SIZE             (39250)

// The minimum amount of dynamic memory left for run time allocation by the stack (IP, UDP, etc)
// This is just a warning threshold.
// If after all the modules are initialized the amount of memory available in the TCPIP heap
// is less then TCPIP_STACK_DRAM_RUN_LIMIT then a warning will be displayed
// (if the debug channel is enabled)
// For proper operation there should be always a heap reserve of at least few KB.
// Note: this limit is used only for TCPIP_STACK_USE_INTERNAL_HEAP and TCPIP_STACK_USE_INTERNAL_HEAP_POOL
#define TCPIP_STACK_DRAM_RUN_LIMIT          (2048)

// The size of the expansion heap for a TCPIP_STACK_USE_INTERNAL_HEAP_POOL
// This is the amount of heap allocated besides the size resulting from the pool entries
// and allows expanding at run time.
// How a specific entry expands is controlled per entry
// If set to 0, then there is no expansion at run time.
// Note: this parameter is specific for TCPIP_STACK_USE_INTERNAL_HEAP_POOL
// Note: this parameter allows the auto configuration of the pool at run time
#define TCPIP_STACK_INTERNAL_HEAP_POOL_EXPANSION_SIZE   (0)


// Enable debugging of an allocation call that failed.
// If the system debug is enabled (SYS_DEBUG_ENABLE) the stack will issue a
// warning message at the system debug channel.
#define TCPIP_STACK_DRAM_DEBUG_ENABLE

    // Number of heap types at run time to enable debugging, statistics, trace on, etc.
    // Currently only 1 heap type is supported at run time, one of: internal heap, internal pool and external
#define TCPIP_STACK_SUPPORTED_HEAPS     1

    // Enable tracing of the allocated memory by each module.
    // The stack will trace all the memory allocated by a module
    // and various statistics.
    //#define TCPIP_STACK_DRAM_TRACE_ENABLE
    // Number of trace slots to be used.
    // There is on slot needed per module that allocates memory from the heap.
    #define TCPIP_STACK_DRAM_TRACE_SLOTS    12

// Stack allocation function, malloc style
// This is the function the stack will call to allocate memory
// needed for its own heap: TCPIP_STACK_DRAM_SIZE.
// Use standard C library 'malloc' as a default
#define TCPIP_STACK_MALLOC_FUNC         malloc

// Stack allocation function, calloc style
// This is the function the stack will call to allocate memory
// needed for its own heap: TCPIP_STACK_DRAM_SIZE.
// Use standard C library 'calloc' as a default
#define TCPIP_STACK_CALLOC_FUNC         calloc

// Stack deallocation function, free style
// This is the function the stack will call for freeing the allocated memory
// when the stack is deinitialized.
// Use standard C library 'free' as a default
#define TCPIP_STACK_FREE_FUNC         free



// =======================================================================
//   Event Notifications Options
// =======================================================================
// This setting enables the stack event notification.
// It allows the MAC drivers to report their TX/RX related events to the stack manager
// but also allows users of the stack to register notification handler so that
// they are notified of specific events.
// The operation of the stack is more efficient when event notification is turned on
// and this is how the stack is designed to run.
// Always leave this setting defined.
// The choice for selecting this parameter will eventually be removed.
// Maintained for backward compatibility. 
#define TCPIP_STACK_USE_EVENT_NOTIFICATION

// This setting enables the reporting of the events by the stack to the user
// using the notification system
// If enabled, then TCPIP_STACK_HandlerRegister and TCPIP_STACK_HandlerDeregister
// functions are compiled in and can be used
// If disabled, these functions do not exist and cannot be used/called 
// Relevant only when TCPIP_STACK_USE_EVENT_NOTIFICATION is enabled
#define TCPIP_STACK_USER_NOTIFICATION       true

// This setting enables the TCPIP_STACK_Deinitialize() operation
// If this symbol is false, then the TCPIP_STACK_Deinitialize is not built in
// Useful when stack stop and restart is not needed at run time - smaller code footprint for the TCP/IP stack.
// Notes:
//  - The TCP/IP stack can be initialized as usual (TCPIP_STACK_Initialize) but it cannot be
//    stopped and restarted
//  - None of the TCP/IP modules (except the MAC and PHY, see below) will have the corresponding Deinitialize function built in
//  - If the stack initialization failed (i.e. the TCPIP_STACK_Initialize returned SYS_MODULE_OBJ_INVALID or
//    TCPIP_STACK_Status returns SYS_STATUS_ERROR) the stack modules cannot be used and the application
//    should not make any attempt to call their API!
//
#define TCPIP_STACK_DOWN_OPERATION          true

// This setting enables the TCPIP_STACK_NetUp/TCPIP_STACK_NetDown operations
// When enabled, these functions are built in and can be used by an app
// Useful when interfaces do not need restarting at run time - results in smaller code footprint.
// Note:
//  - Combination  TCPIP_STACK_DOWN_OPERATION == false and  TCPIP_STACK_IF_UP_DOWN_OPERATION == true
//    is invalid and must not be used otherwise the behavior is undefined!
#define TCPIP_STACK_IF_UP_DOWN_OPERATION    true

// This setting specifies the behavior of stack regarding the MAC and PHY drivers when the 
// TCPIP_STACK_DOWN_OPERATION == false in the situation where the stack initialization failed
// and the stack cannot be started.
// If true, the MAC (and the corresponding PHY) TCPIP_MAC_Deinitialize will be called.
//  This operation is supposed to exist and this setting will conserve power.
// If false, the TCPIP_MAC_Deinitialize waon't be called and the code footprint could be smaller.
//  The TCPIP_MAC_Deinitialize operation, which is expensive, could be unimplemented.
// Note: the TCPIP_STACK_MAC_DOWN_OPERATION == false could be set only when TCPIP_STACK_DOWN_OPERATION == false!
// Otherwise the behavior is undefined!
#define TCPIP_STACK_MAC_DOWN_OPERATION            true

// This setting enables/disables the stack signaling when an interface address changes
// This could be triggered by the DHCP kicking into action, by ZCLL or other address service,
// or simply by the user changing the IP address of an interface
// If it is enabled, the socket modules (UDP, TCP) will be notified and they will 
// disconnect their sockets that are currently connected on that interface that has changed.
// User can be notified by registering an event handler either with socket events or
// with the general stack signaling mechanism
// Default setting is true
#define TCPIP_STACK_INTERFACE_CHANGE_SIGNALING  true

// This setting enables the configuration get/set operations:
// TCPIP_STACK_ModuleConfigGet, TCPIP_STACK_NetConfigGet, TCPIP_STACK_NetConfigSet
// If true, the functionality is built in and could be used by the application
// If false, these functions do not exist and the generated code is smaller 
#define TCPIP_STACK_CONFIGURATION_SAVE_RESTORE           true

//	The number of entries in the internally maintained secure port table
//	This table is populated at stack initialization with default
//	well-known port values
//	Currently this number should be >= 10
#define TCPIP_STACK_SECURE_PORT_ENTRIES 			(10)

// Enable the packet logger
// Simple module that logs the packets as they fly through the stack
// from creation until acknowledged
// Default is disabled
#define TCPIP_PACKET_LOG_ENABLE     0

// Number of log entries, if the packet logger is enabled
// Each entry takes approx 24 bytes
#define TCPIP_PKT_LOG_SIZE     40


// TCPIP task processing rate, in milliseconds.
// The TCP/IP task will require a timer event with this rate
// for maintaining its own state machine, processing timeouts for all modules,
// or dispatching the RX traffic if the interrupt operation is not enabled, etc.
// The lower this value (higher the frequency) the higher the priority of the TCP/IP stack
// and a higher performance can be obtained.
// Note: this is the time base for all TCP/IP modules, i.e.
// no TCP/IP module can run at a rate higher than this one.
// Note: the System Tick resolution has to be fine enough
// to allow for this TCP/IP rate granularity.  
#define TCPIP_STACK_TICK_RATE                       (5)

// Enable support for alias interfaces
// An alias interface uses the physical interface of
// another network interface.
// It allows multiple IP addresses for a host on the same interface, for example.
// Default is disabled.
#define TCPIP_STACK_ALIAS_INTERFACE_SUPPORT         0

// =======================================================================
//   Stack Modules Execution Options
// =======================================================================
// This setting enables the selection of how the stack module task functions get executed.
//
// If the symbol is not defined then the stack manager calls itself all the
//  modules Task functions as required when executing the TCPIP_STACK_Task() function.
// This is the default model for now.
//
// If this symbol is defined, then the application is responsible for calling the Task
// functions for all the TCP/IP stack modules.
// This is the preferred model especially when running under an RTOS environment
// and the app can adjust the priority levels for the execution of each task.
//
// Note: the TCPIP_STACK_Task is always called at the application level.
//
//#define TCPIP_STACK_APP_EXECUTE_MODULE_TASKS

#endif  // __TCPIP_CONFIG_H_


