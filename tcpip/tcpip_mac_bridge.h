/*******************************************************************************
  TCP/IP MAC bridge file

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_mac_bridge.h

  Summary:
    Internal TCP/IP stack MAC bridge file
    
  Description:
    This header file contains the function prototypes and definitions of the 
    TCP/IP stack MAC Layer 2 bridge 

    Based on the IEEE 802.1D - 2004 standard
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2012-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/

// DOM-IGNORE-END

#ifndef __TCPIP_MAC_BRIDGE_H_
#define __TCPIP_MAC_BRIDGE_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  
/* The implementation of the bridge functionality is a subset of the IEEE 802.1D - 2004 standard.
 * Not implemented functionality and known limitations:
 *  - IEEE 802.3 and IEEE 802.11 support only
 *  - No STP/RSTP support
 *  - no QoS support
 *  - no support for Extended Filtering Services
 *  - no GARP Multicast Registration Protocol (GMRP)
 *  */

// *****************************************************************************
/* MAC bridging operation result

  Summary:
    Result of an MAC bridging operation

  Description:
    These are codes returned by MAC bridge initialization and API functions

  Remarks:
    None.
 */

typedef enum
{
    TCPIP_MAC_BRIDGE_RES_OK     = 0,        // operation successful

    // errors
    TCPIP_MAC_BRIDGE_RES_FDB_ALLOC_ERROR    = -1,       // FDB allocation failed
    TCPIP_MAC_BRIDGE_RES_PKT_ALLOC_ERROR    = -2,       // packet allocation failed
    TCPIP_MAC_BRIDGE_RES_DCPT_ALLOC_ERROR   = -3,       // descriptor allocation failed
    TCPIP_MAC_BRIDGE_RES_NO_INIT_DATA       = -4,       // missing initialization data
    TCPIP_MAC_BRIDGE_RES_INIT_DATA_ERROR    = -5,       // initialization data error
    TCPIP_MAC_BRIDGE_RES_INIT_PORT_ERROR    = -6,       // bridge port initialization data error
    TCPIP_MAC_BRIDGE_RES_IF_NOT_BRIDGED     = -7,       // a specified interface is not part of the bridge
    TCPIP_MAC_BRIDGE_RES_FDB_FULL           = -8,       // Filtering Data Base full, could not add to it
    TCPIP_MAC_BRIDGE_RES_IF_ADDRESS_ERROR   = -9,       // an interface on this host has an invalid address
    TCPIP_MAC_BRIDGE_RES_TIMEOUT_ERROR      = -10,      // requested timeout cannot be satified
    TCPIP_MAC_BRIDGE_RES_SIGNAL_ERROR       = -11,      // failed to register a timer signal with the stack
    TCPIP_MAC_BRIDGE_RES_HANDLE_ERROR       = -12,      // invalid bridge handle
    TCPIP_MAC_BRIDGE_RES_PARAM_ERROR        = -13,      // invalid parameter supplied
    TCPIP_MAC_BRIDGE_RES_INDEX_ERROR        = -14,      // invalid FDB index
    TCPIP_MAC_BRIDGE_RES_INDEX_NO_ENTRY     = -15,      // FDB index exists but the entry is not used
    TCPIP_MAC_BRIDGE_RES_LOCK_ERROR         = -16,      // another FDB operation is in progress and the FDB is locked
                                                        // retry

}TCPIP_MAC_BRIDGE_RESULT;



// *****************************************************************************
/* MAC bridge event

  Summary:
    Run time MAC bridge event

  Description:
    These are events that the MAC bridge reports at run time

  Remarks:
    Each event can carry different information
    See the TCPIP_MAC_BRIDGE_EVENT_HANDLER definition 
 */

typedef enum
{
    TCPIP_MAC_BRIDGE_EVENT_NONE     = 0,    // no event

    // errors
    TCPIP_MAC_BRIDGE_EVENT_FDB_FULL,        // an entry could not be added because he FDP was full
    TCPIP_MAC_BRIDGE_EVENT_FAIL_PKT_ALLOC,  // a packet allocation failed
    TCPIP_MAC_BRIDGE_EVENT_FAIL_DCPT_ALLOC, // a descriptor allocation failed
    TCPIP_MAC_BRIDGE_EVENT_FAIL_MTU,        // packet was dropped because of MTU
    TCPIP_MAC_BRIDGE_EVENT_FAIL_SIZE,       // packet was dropped because of its size
    TCPIP_MAC_BRIDGE_EVENT_PKT_POOL_EMPTY,  // the packet pool has been depleted
    TCPIP_MAC_BRIDGE_EVENT_DCPT_POOL_EMPTY, // the descriptor pool has been depleted
    TCPIP_MAC_BRIDGE_EVENT_FAIL_LOCK,       // FDB lock failed

    // maintenance events
    TCPIP_MAC_BRIDGE_EVENT_ENTRY_ADDED,     // dynamic MAC source address added to the FDB
    TCPIP_MAC_BRIDGE_EVENT_ENTRY_EXPIRED,   // FDB dynamic entry expired
}TCPIP_MAC_BRIDGE_EVENT;

// *****************************************************************************
/*
  Type:
    TCPIP_MAC_BRIDGE_EVENT_HANDLER

  Summary:
    MAC bridge event handler prototype.

  Description:
    Prototype of a MAC bridge event handler.
    Users can register a handler with the MAC bridge.
    Once a bridge event occurs the registered handler will be called
    The handler has to be short and fast. It is meant for
    setting an event flag, not for lengthy processing!

    Value of the evInfo in the call is different per event type:
    TCPIP_MAC_BRIDGE_EVENT_FDB_FULL     - evInfo == const TCPIP_MAC_ADD
                                          pointer to a constant  MAC address that should have been added
    TCPIP_MAC_BRIDGE_EVENT_FAIL_PKT_ALLOC - evInfo == number of packets for which the allocation has failed 
    TCPIP_MAC_BRIDGE_EVENT_FAIL_DCPT_ALLOC - evInfo == number of descriptors for which the allocation has failed 
    TCPIP_MAC_BRIDGE_EVENT_FAIL_MTU     - evInfo == size of the packet that exceeded the MTU 
    TCPIP_MAC_BRIDGE_EVENT_FAIL_SIZE    - evInfo == size of the packet that exceeded the pool packets size 
    TCPIP_MAC_BRIDGE_EVENT_PKT_POOL_EMPTY - evInfo == 0 
    TCPIP_MAC_BRIDGE_EVENT_DCPT_POOL_EMPTY - evInfo == 0 
    TCPIP_MAC_BRIDGE_EVENT_FAIL_LOCK    - evInfo == 0 

    TCPIP_MAC_BRIDGE_EVENT_ENTRY_ADDED  - evInfo == const TCPIP_MAC_ADD
                                          pointer to a constant  MAC address that's been added
    TCPIP_MAC_BRIDGE_EVENT_ENTRY_EXPIRED- evInfo 

 */

typedef void    (*TCPIP_MAC_BRIDGE_EVENT_HANDLER)(TCPIP_MAC_BRIDGE_EVENT evType, const void* evInfo);

// *****************************************************************************
/*
  Type:
    TCPIP_MAC_BRIDGE_HANDLE

  Summary:
    Bridge handle.

  Description:
    A handle that a client can use to access the bridge functionality
 */

typedef const void* TCPIP_MAC_BRIDGE_HANDLE;

// *****************************************************************************
/*
  Type:
    TCPIP_MAC_BRIDGE_EVENT_HANDLE

  Summary:
    Bridge event handle.

  Description:
    A handle that a client can use to register event handlers with the bridge
 */

typedef const void* TCPIP_MAC_BRIDGE_EVENT_HANDLE;

typedef struct
{
    /* The name of the interface to be part of the bridge
     * For ex: "eth0", wlan0", etc. */
    const char*     ifName;
}TCPIP_MAC_BRIDGE_ENTRY_ASCII;

typedef struct
{
    /* The number of the interface to be part of the bridge */
    uint8_t         ifIx;
}TCPIP_MAC_BRIDGE_ENTRY_BIN;


typedef union
{
    TCPIP_MAC_BRIDGE_ENTRY_ASCII   entryAscii;
    TCPIP_MAC_BRIDGE_ENTRY_BIN     entryBin;
}TCPIP_MAC_BRIDGE_ENTRY;

// *****************************************************************************
/* MAC bridging flags

  Summary:
    Flags for the configuration of the MAC bridging

  Description:
    These are flags used to configure the run time operation of the
    MAC bridging module

  Remarks:
    8 bits only supported.
 */

typedef enum
{
    TCPIP_MAC_BRIDGE_FLAG_NONE              = 0x00,     // no flag set, the default
    TCPIP_MAC_BRIDGE_FLAG_NO_DYNAMIC_LEARN  = 0x01,     // Do not learn dynamic addresses, use only the static/permanent FDB entries
                                                        // useful for testing or restricting traffic to few addresses only  

    TCPIP_MAC_BRIDGE_FLAG_FDB_ENTRY_EXISTS  = 0x02,     // Forward traffic only if the source or destination address of a packet is in FDB
                                                        // Forwarding is still done using the destination address
                                                        // useful for testing or restricting traffic to few addresses only  

    TCPIP_MAC_BRIDGE_FLAG_IF_IX_TABLE       = 0x00,     // The initialization bridge table contains the interface indexes - default setting
    TCPIP_MAC_BRIDGE_FLAG_IF_NAME_TABLE     = 0x80,     // The initialization bridge table contains the interface names, ASCII strings

}TCPIP_MAC_BRIDGE_FLAGS;


// *****************************************************************************
/* MAC bridging outbound port control type

  Summary:
    Types of port control for the forwarding/filtering

  Description:
    These are flags used to statically control the run-time forwarding/filtering
    operation of the MAC bridging module ports

  Remarks:
    The meaning of the TCPIP_MAC_BRIDGE_CONTROL_TYPE_DEFAULT could be different
    depending on the type of entry in the FDB
 */

typedef enum
{
    TCPIP_MAC_BRIDGE_CONTROL_TYPE_DEFAULT     = 0,      // default behavior:
                                                        //  - when controlling an out port (a different port than the one the packet arrived on)
                                                        //    this referrs to the forwarding/filtering of packets on the output ports/interfaces:
                                                        //      forward or filter based on the dynamic filtering info
                                                        //  - when controlling an input port (the port the packet arrived on) this refers to 
                                                        //    the processing of the packets by the host (on which the bridge is running):
                                                        //         - host processes multicast packets
                                                        //         - host discards unicast packets not address to this host
                                                        //         - host processes unicast packets addressed to this host
    TCPIP_MAC_BRIDGE_CONTROL_TYPE_FORWARD,              // out port: forward irrespective of the dynamic filtering info
                                                        // in port: host should process this packet
    TCPIP_MAC_BRIDGE_CONTROL_TYPE_FILTER,               // out port: filter irrespective of the dynamic filtering info
                                                        // in port: host should discard this packet
    // TCPIP_MAC_BRIDGE_CONTROL_TYPE_GROUP,             // out port: currently not supported - forward or filter based on the Default Group filtering behavior

    TCPIP_MAC_BRIDGE_CONTROL_TYPES,                     // number of supported types
}TCPIP_MAC_BRIDGE_CONTROL_TYPE;


// *****************************************************************************
/* MAC bridge control descriptor

  Summary:
    Data structure for the bridge configuration

  Description:
    Data structure for the bridge configuration
    Descriptor of the control to be applied to an output port
    as part of a static/permanent entry

  Remarks:
    None

 */
typedef struct
{
    uint8_t     outIx;              // index of the output interface that this data refers to
                                    // this interface should be part of the bridge
    uint8_t     portControl;        // a TCPIP_MAC_BRIDGE_CONTROL_TYPE value describing 
                                    // the filtering rule for that output port     
}TCPIP_MAC_BRIDGE_CONTROL_DCPT;

// *****************************************************************************
/* MAC bridge control entry

  Summary:
    Data structure for the bridge configuration

  Description:
    Data structure for the bridge configuration
    Descriptor of the control to be applied to a bridge port
    as part of a static/permanent entry

  Remarks:
    None

 */
typedef struct
{
    uint8_t                                 inIx;       // the input interface index this entry refers to 
                                                        // interface should be part of the bridge
    uint8_t                                 dcptMapEntries; // entries in the pDcptMap
    const TCPIP_MAC_BRIDGE_CONTROL_DCPT*    pDcptMap;   // array of control values, dcptMapEntries entries
                                                        // describing the control for each output port
                                                        // This should <= nPorts (the number of bridge ports derived from TCPIP_MAC_BRIDGE_CONFIG::bridgeTable).
                                                        // excess entries are ignored
                                                        // missing entries will have the default control type:  TCPIP_MAC_BRIDGE_CONTROL_TYPE_DEFAULT
                                                        //
                                                        // pDcptMap->outIx should be different than inIx
                                                        // if it's the same, then the setting it is used only for a MAC address belonging to the host
                                                        // otherwise it's ignored
}TCPIP_MAC_BRIDGE_CONTROL_ENTRY;


// *****************************************************************************
/* MAC bridge permanent entry

  Summary:
    Data structure for the bridge configuration

  Description:
    Data structure for the bridge configuration
    Descriptor of a permanent entry for bridge configuration

  Remarks:
    None

 */
typedef struct
{
    TCPIP_MAC_ADDR                          destAdd;    // - individual MAC address, OR
                                                        // - a group of MAC addresses
                                                        // this entry refers to
                                                    
    uint8_t                                 controlEntries; // items in the pControlEntry
    const TCPIP_MAC_BRIDGE_CONTROL_ENTRY*   pControlEntry;  // array of control entries, controlEntries entries
}TCPIP_MAC_BRIDGE_PERMANENT_ENTRY;



// *****************************************************************************
/* MAC bridge configuration data

  Summary:
    Data structure for the bridge configuration

  Description:
    Data structure containing the bridge initialization

  Remarks:
    The bridge accepts this data as part of its initialization

    The bridge accepts static/permanent entries
    as part of the bridge configuration 

    The FDB is updated internally to contain entries for the bridge ports themselves.
    Those ports will have the default behavior (see TCPIP_MAC_BRIDGE_CONTROL_TYPE definition).
    The behavior can be altered with bridgePermTable entries.

    The host own interfaces/ports have their MAC addresses automatically added to the FDB
    The FDB has to be big enough to allow this, otherwise the bridge operation will result in error

 */

typedef struct
{
    uint16_t                                purgeTimeout;   // timeout for an entry to be purged from the table, in seconds
                                                            // an inactive entry will be removed after this timeout
                                                            // Recommended default is 300 seconds
                                                            // Recommended range: [1, 1,000,000]
                                                            // Supported range: [1, 65535]
                                                            // If 0, the default build time value TCPIP_MAC_BRIDGE_ENTRY_TIMEOUT will be used

    uint16_t                                transitDelay;   // maximum transit delay for a packet through the bridge, in seconds
                                                            // a packet that was not completely forwarded within this time will be dropped
                                                            // recommended default: 1 second
                                                            // Maximum recommended value: 4 seconds
                                                            // If 0, the default build time value TCPIP_MAC_BRIDGE_MAX_TRANSIT_DELAY will be used

    uint16_t                                fdbEntries;     // number of entries in the FDB
                                                            // the Filtering Database will contain this number of entries
                                                            // for both static (created under management control) and dynamic (learned) entries

    uint16_t                                pktPoolSize;    // number of packets in the pool for run time forwarding
                                                            // this pool is allocated at the bridge initialization time
                                                            // and then replenished when needed
                                                            // if 0, then there's no pool and allocation occurs as needed
                                                            // at run time, each packet being allocated exactly the needed size
                                                            // Note: the packet pool is used only for packets that need to be processed by the host too.
                                                            // Otherwise the packets are directly forwarded to the destination ports
    uint16_t                                pktSize;        // size of the packets in the pool
                                                            // Note that incoming packets that are larger than this size will be dropped
                                                            // Relevant only when pktPoolSize != 0
    uint16_t                                dcptPoolSize;   // number of descriptors in the pool for run time forwarding
                                                            // this pool is allocated at the bridge initialization time
                                                            // each forwarded packet has an associated descriptor
                                                            // Could be larger than the pktPoolSize
                                                            // Cannot be 0, a descriptor pool is always needed!
    uint8_t                                 pktReplenish;   // Number of packets to replenish the pool, when it becomes empty
                                                            // If 0, then no other packets will be allocated
                                                            // Relevant only when pktPoolSize != 0
    uint8_t                                 dcptReplenish;  // Number of descriptors to replenish the pool, when it becomes empty
                                                            // If 0, then no other descriptors will be allocated
    uint8_t                                 bridgeFlags;    // TCPIP_MAC_BRIDGE_FLAGS value for configuration flags


    /* The number of entries in the initialization bridge table.
     * Should <= TCPIP_MAC_BRIDGE_MAX_PORTS_NO */ 
    size_t                                  bridgeTableSize;
    
    /* the interfaces that are part of the bridge
     * The type of the table is given by the flag TCPIP_MAC_BRIDGE_FLAG_IF_IX_TABLE/TCPIP_MAC_BRIDGE_FLAG_IF_NAME_TABLE
     * This table gives the nPorts - the actual number of ports that are part of the bridge */
    const TCPIP_MAC_BRIDGE_ENTRY*           bridgeTable;   

    /* The number of entries in the initialization permanent table
     * Not mandatory, could be 0 */
    size_t                                  bridgePermTableSize;
    
    /* Bridge permanent table. Not mandatory, could be NULL.
     * An array of TCPIP_MAC_BRIDGE_PERMANENT_ENTRY */
    const TCPIP_MAC_BRIDGE_PERMANENT_ENTRY* bridgePermTable;
                                                                                          

}TCPIP_MAC_BRIDGE_CONFIG;

// *****************************************************************************
/* MAC bridge statistics per port

  Summary:
    Structure describing the statistics maintained by the MAC bridge
    per bridge port

  Description:
    Data structure updated by the bridge forwarding process
    when TCPIP_MAC_BRIDGE_STATISTICS != 0 

  Remarks:
    None
*/

typedef struct
{
    uint32_t    rxPackets;          // counter of total received packets
    uint32_t    rxDestMeUcast;      // counter of received packets with host unicast destination
    uint32_t    rxDestNotMeUcast;   // counter of received packets with non-host unicast destination
    uint32_t    rxDestMcast;        // counter of received packets with multicast destination
    uint32_t    reservedPackets;    // discarded reserved destination packets
    uint32_t    fwdUcastPackets;    // total forwarded unicast packets
    uint32_t    fwdMcastPackets;    // total forwarded multicast packets
    uint32_t    fwdDirectPackets;   // total directly forwarded packets;
                                    // these are packets not processed by the host so a packet copy was not necessary
}TCPIP_MAC_BRIDGE_PORT_STAT;

// *****************************************************************************
/* MAC bridge statistics

  Summary:
    Structure describing the statistics maintained by the MAC bridge

  Description:
    Data structure updated by the bridge forwarding process
    when TCPIP_MAC_BRIDGE_STATISTICS != 0 

  Remarks:
    None
*/
typedef struct
{
    uint32_t    failPktAlloc;      // forwarding failures due to packet allocation errors
    uint32_t    failDcptAlloc;     // forwarding failures due to descriptor allocation errors
    uint32_t    failMac;           // forwarding failures due to MAC rejecting the packet
    uint32_t    failMtu;           // forwarding failures due to MTU
    uint32_t    failSize;          // forwarding failures due to packet size
    uint32_t    failLocks;         // FDB was locked
    uint32_t    fdbFull;           // FDB was full

    uint32_t    allocPackets;      // number of run time allocated packets
    uint32_t    allocDcpts;        // number of run time allocated descriptors
    uint32_t    freedPackets;      // number of freed packets
    uint32_t    ackPackets;        // number of packets acknowledged after transmission
    uint32_t    delayPackets;      // number of packets dropped because of the transit delay 
    uint32_t    pktPoolEmpty;      // number of times the packet pool was empty, needed to replenish
    uint32_t    pktPoolLowSize;    // lowest number of packets in the packet pool
    uint32_t    pktPoolSize;       // the current packet pool size
    uint32_t    dcptPoolEmpty;     // number of times the descriptor pool was empty, needed to replenish
    uint32_t    dcptPoolSize;      // the current descriptor pool size
    uint32_t    dcptPoolLowSize;   // lowest number of descriptors in the descriptor pool
    TCPIP_MAC_BRIDGE_PORT_STAT portStat[TCPIP_MAC_BRIDGE_MAX_PORTS_NO];
}TCPIP_MAC_BRIDGE_STAT;


// *****************************************************************************
/* MAC FDB entry flags

  Summary:
    Flags maintained for the FDB entries

  Description:
    These are the flags associated with the FDB entries maintened by the bridge

  Remarks:
    8 bit values only supported
 */

typedef enum
{
    TCPIP_MAC_FDB_FLAG_NONE         = 0x00,     // no flag set
    TCPIP_MAC_FDB_FLAG_STATIC       = 0x01,     // entry is static and permanent; set by initialization/administrator
                                                // otherwise it's a dynamic entry
    TCPIP_MAC_FDB_FLAG_HOST         = 0x02,     // entry is for one of the interfaces belonging to this host
    TCPIP_MAC_FDB_FLAG_PORT_VALID   = 0x04,     // the learnPort value is valid 

}TCPIP_MAC_FDB_FLAGS;



// *****************************************************************************
/* MAC bridge FDB entry description

  Summary:
    Structure describing an entry in the MAC bridge FDB

  Description:
    User info about the FDB contents
    This gives details about maintained info as part of the bridge forwarding process
    Could be used for exploring the contents of the FDB

  Remarks:
    None
*/
typedef struct
{
    TCPIP_MAC_ADDR      destAdd;        // MAC address the entry refers to
    uint8_t             flags;          // a TCPIP_MAC_FDB_FLAGS value
    uint8_t             learnPort;      // port number this address has been learned on
    uint32_t            tExpire;        // dynamic entry expiration time translated in system ticks
    uint32_t            fwdPackets;     // number of packets forwarded for this destination address
    uint8_t             outPortMap[TCPIP_MAC_BRIDGE_MAX_PORTS_NO][TCPIP_MAC_BRIDGE_MAX_PORTS_NO];
                                        // the TCPIP_MAC_BRIDGE_CONTROL_TYPE type for a static entry
                                        // the outPotMap[k][j] describes the behavior (forward/discard/default) 
                                        // on the port j for a packet arriving on port k
                                        // Note that the valid info is outPortMap[nPorts][nPorts]
                                        // where nPorts is the number of bridge ports (derived from TCPIP_MAC_BRIDGE_CONFIG::bridgeTable).

}TCPIP_MAC_FDB_ENTRY;

// *****************************************************************************
/* MAC bridge dynamic operation

  Summary:
    List of the MAC bridge supported operations

  Description:
    These are the dynamic operations supported by the MAC bridge

  Remarks:
    None.
 */

typedef enum
{
    TCPIP_MAC_BRIDGE_OP_NONE  = 0,      // no operation
    TCPIP_MAC_BRIDGE_OP_RESET,          // reset the FDB
                                        // All the dynamic entries and data will be cleared.
                                        // Only the static FDB entries are kept
    TCPIP_MAC_BRIDGE_OP_DELETE,         // delete an entry in the FDB
                                        // the entry is selected using the destAdd MAC address 
    TCPIP_MAC_BRIDGE_OP_CREATE,         // create/update an entry in the FDB selected by the destAdd MAC address
                                        // nPorts and outPortMap should carry the FDB entry data
}TCPIP_MAC_BRIDGE_OPER;

// API

// *****************************************************************************
/*
  Function:
    TCPIP_MAC_BRIDGE_HANDLE TCPIP_MAC_Bridge_Open(const SYS_MODULE_INDEX bridgeIndex);

  Summary:
    Opens the bridge and gets a bridge handle
   
  Description:
    The function opens the bridge and gets a bridge handle
   
  Precondition:
    TCPIP_MAC_BRIDGE properly initialized

  Parameters:
    bridgeIndex  - identifier for the bridge instance to be opened.
    
  Returns:
    - a non NULL handle if successful
    - 0 if failed
      
  Remarks:
    Currently there's only one supported bridge in the TCP/IP stack
    so the bridgeIndex should be == 0

    Once the bridge is initialized, a call to the TCPIP_MAC_Bridge_Open()
    will return a valid handle to the bridge
    There is no protection for multiple clients,
    all clients use the same bridge handle

 */
TCPIP_MAC_BRIDGE_HANDLE     TCPIP_MAC_Bridge_Open(const SYS_MODULE_INDEX bridgeIndex);


// *****************************************************************************
/*
  Function:
    SYS_STATUS TCPIP_MAC_Bridge_Status(TCPIP_MAC_BRIDGE_HANDLE brHandle);

  Summary:
    Helper to get the bridge status
   
  Description:
    The function is a helper that returns the current bridge status
   
  Precondition:
    TCPIP_MAC_BRIDGE properly initialized
        

  Parameters:
    brHandle    - bridge handle obtained with TCPIP_MAC_Bridge_Open()

  Returns:
    current bridge status
      
  Remarks:
    None

 */
SYS_STATUS TCPIP_MAC_Bridge_Status(TCPIP_MAC_BRIDGE_HANDLE brHandle);


// *****************************************************************************
/*
  Function:
    bool TCPIP_MAC_Bridge_StatisticsGet(TCPIP_MAC_BRIDGE_HANDLE brHandle, TCPIP_MAC_BRIDGE_STAT* pStat, bool clear);

  Summary:
    Helper to get the bridge statistics
   
  Description:
    The function is a helper that returns the contents of the
    bridge statistics
   
  Precondition:
    TCPIP_MAC_BRIDGE properly initialized
    Bridge statistics enabled (TCPIP_MAC_BRIDGE_STATISTICS != 0) 
        

  Parameters:
    brHandle    - bridge handle obtained with TCPIP_MAC_Bridge_Open()

    pStat       - pointer to a structure to store the current statistics
                  Could be NULL
    clear       - if true, the statistics are cleared

  Returns:
    - true if success
    - false if failed
      
  Remarks:
    Function exists only if MAC bridge statistics are enabled

 */
bool TCPIP_MAC_Bridge_StatisticsGet(TCPIP_MAC_BRIDGE_HANDLE brHandle, TCPIP_MAC_BRIDGE_STAT* pStat, bool clear);


// *****************************************************************************
/* Function:
    TCPIP_MAC_BRIDGE_EVENT_HANDLE TCPIP_MAC_Bridge_EventHandlerRegister(TCPIP_MAC_BRIDGE_HANDLE brHandle, TCPIP_MAC_BRIDGE_EVENT_HANDLER evHandler)

  Summary:
    Registers a bridge Handler.

  Description:
    This function registers a bridge event handler.
    The bridge module will call the registered handler when a
    bridge event (TCPIP_MAC_BRIDGE_EVENT) occurs.

  Precondition:
    The bridge module must be initialized.

  Parameters:
    brHandle    - bridge handle obtained with TCPIP_MAC_Bridge_Open()
    evHandler   - Handler to be called when a bridge event occurs.

  Returns:
    Returns a valid event handle if the call succeeds, or a null handle if
    the call failed (another registered handler, for example).

  Remarks:
    The handler has to be short and fast. It is meant for
    setting an event flag, not for lengthy processing!

    Currently only one handler is supported.
    A handler must be first de-registered before registering another one

    The API exists only if TCPIP_MAC_BRIDGE_EVENT_NOTIFY != 0
 */

TCPIP_MAC_BRIDGE_EVENT_HANDLE TCPIP_MAC_Bridge_EventHandlerRegister(TCPIP_MAC_BRIDGE_HANDLE brHandle, TCPIP_MAC_BRIDGE_EVENT_HANDLER evHandler);


// *****************************************************************************
/* Function:
    bool TCPIP_MAC_Bridge_EventHandlerDeregister(TCPIP_MAC_BRIDGE_HANDLE brHandle, TCPIP_MAC_BRIDGE_EVENT_HANDLE evHandle);

  Summary:
    Deregisters a previously registered bridge handler.
    
  Description:
    This function deregisters the bridge event handler.

  Precondition:
    The bridge module must be initialized.

  Parameters:
    brHandle    - bridge hadle obtained with TCPIP_MAC_Bridge_Open()
    evHandle    - An event handle returned by a previous call to TCPIP_MAC_Bridge_EventHandlerRegister.

  Returns:
    - true  - if the call succeeds
    - false - if no such handler is registered


  Remarks:
    The API exists only if TCPIP_MAC_BRIDGE_EVENT_NOTIFY != 0
 */

bool    TCPIP_MAC_Bridge_EventHandlerDeregister(TCPIP_MAC_BRIDGE_HANDLE brHandle, TCPIP_MAC_BRIDGE_EVENT_HANDLE evHandle);


// *****************************************************************************
/* Function:
    size_t TCPIP_MAC_Bridge_FDBEntries(TCPIP_MAC_BRIDGE_HANDLE brHandle)

  Summary:
    Returns the number of the entries in the bridge FDB
    
  Description:
    This function returns the number of the entries in the bridge FDB

  Precondition:
    The bridge module must be initialized.

  Parameters:
    brHandle    - bridge hadle obtained with TCPIP_MAC_Bridge_Open()

  Returns:
    - the number of entries in the bridge FDB
    


  Remarks:
    None
 */

size_t    TCPIP_MAC_Bridge_FDBEntries(TCPIP_MAC_BRIDGE_HANDLE brHandle);

// *****************************************************************************
/* Function:
    TCPIP_MAC_BRIDGE_RESULT TCPIP_MAC_Bridge_FDBIndexRead(TCPIP_MAC_BRIDGE_HANDLE brHandle, size_t ix, TCPIP_MAC_FDB_ENTRY* pEntry)

  Summary:
    Returns a specific FDB entry contents
    
  Description:
    This function returns the contents of the entry with the specified index in the FDB

  Precondition:
    The bridge module must be initialized.

  Parameters:
    brHandle    - bridge hadle obtained with TCPIP_MAC_Bridge_Open()
    ix          - the entry index
    pEntry      - address of a data structure to store the FDB entry
                  Could be NULL


  Returns:
    - TCPIP_MAC_BRIDGE_RES_OK if the bridge handle is valid and the entry with that index exists
            the pEntry has been updated (if !NULL)
    - TCPIP_MAC_BRIDGE_RES_HANDLE_ERROR an invalid handle was supplied
    - TCPIP_MAC_BRIDGE_RES_INDEX_ERROR if the index is invalid
    - TCPIP_MAC_BRIDGE_RES_INDEX_NO_ENTRY if the entry with this index is not currently in use
    

  Remarks:
    None
 */

TCPIP_MAC_BRIDGE_RESULT    TCPIP_MAC_Bridge_FDBIndexRead(TCPIP_MAC_BRIDGE_HANDLE brHandle, size_t ix, TCPIP_MAC_FDB_ENTRY* pEntry);

// *****************************************************************************
/* Function:
    TCPIP_MAC_BRIDGE_RESULT TCPIP_MAC_Bridge_FDBReset(TCPIP_MAC_BRIDGE_HANDLE brHandle);

  Summary:
    Reset MAC bridge FDB
    
  Description:
    This function will reset the bridge FDB
    All the dynamic entries and data will be cleared.
    Only the static FDB entries are kept

  Precondition:
    The bridge module must be initialized.

  Parameters:
    brHandle    - bridge hadle obtained with TCPIP_MAC_Bridge_Open()


  Returns:
    - TCPIP_MAC_BRIDGE_RES_OK if the bridge handle is valid and the operation is successful
    - TCPIP_MAC_BRIDGE_RES_HANDLE_ERROR an invalid handle was supplied
    - TCPIP_MAC_BRIDGE_RES_LOCK_ERROR if FDB is locked and a retry is needed 
    
  Remarks:
    Only if TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0 

    This operation locks access to the bridge FDB.
    While this operation executes, the bridge packet processing stopped!
    Use sparingly.
 */

TCPIP_MAC_BRIDGE_RESULT TCPIP_MAC_Bridge_FDBReset(TCPIP_MAC_BRIDGE_HANDLE brHandle);

// *****************************************************************************
/* Function:
    TCPIP_MAC_BRIDGE_RESULT TCPIP_MAC_Bridge_FDBDeleteEntry(TCPIP_MAC_BRIDGE_HANDLE brHandle, const TCPIP_MAC_ADDR* pDestAddr);

  Summary:
    Deletes an entry from the FDB
    
  Description:
    This function will delete the entry corresponding to the destination address from the bridge FDB

  Precondition:
    The bridge module must be initialized.

  Parameters:
    brHandle    - bridge hadle obtained with TCPIP_MAC_Bridge_Open()
    pDestAddr   - valid pointer to a destination address selecting the entry to be deleted

  Returns:
    - TCPIP_MAC_BRIDGE_RES_OK if the bridge handle is valid and the operation is successful
    - TCPIP_MAC_BRIDGE_RES_HANDLE_ERROR an invalid handle was supplied
    - TCPIP_MAC_BRIDGE_RES_PARAM_ERROR is pDestAddr is NULL
    - TCPIP_MAC_BRIDGE_RES_LOCK_ERROR if FDB is locked and a retry is needed 
    
  Remarks:
    Only if TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0 

    This operation locks access to the bridge FDB.
    While this operation executes, the bridge packet processing stopped!
    Use sparingly.
 */

TCPIP_MAC_BRIDGE_RESULT TCPIP_MAC_Bridge_FDBDeleteEntry(TCPIP_MAC_BRIDGE_HANDLE brHandle, const TCPIP_MAC_ADDR* pDestAddr);

// *****************************************************************************
/* Function:
    TCPIP_MAC_BRIDGE_RESULT TCPIP_MAC_Bridge_FDBAddEntry(TCPIP_MAC_BRIDGE_HANDLE brHandle, const TCPIP_MAC_BRIDGE_PERMANENT_ENTRY* pPermEntry);

  Summary:
    Adds an entry to the FDB
    
  Description:
    This function will add the entry corresponding to the destination address to the bridge FDB
    If the FDB already contains the selected address entry, the entry will be updated. 

  Precondition:
    The bridge module must be initialized.

  Parameters:
    brHandle    - bridge hadle obtained with TCPIP_MAC_Bridge_Open()
    pPermEntry   - valid pointer to a bridge paermanent entry to be added

  Returns:
    - TCPIP_MAC_BRIDGE_RES_OK if the bridge handle is valid and the operation is successful
    - TCPIP_MAC_BRIDGE_RES_HANDLE_ERROR an invalid handle was supplied
    - TCPIP_MAC_BRIDGE_RES_PARAM_ERROR is pPermEntry is NULL
    - TCPIP_MAC_BRIDGE_RES_LOCK_ERROR if FDB is locked and a retry is needed 
    
  Remarks:
    Only if TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0 

    This operation locks access to the bridge FDB.
    While this operation executes, the bridge packet processing stopped!
    Use sparingly.
 */

TCPIP_MAC_BRIDGE_RESULT TCPIP_MAC_Bridge_FDBAddEntry(TCPIP_MAC_BRIDGE_HANDLE brHandle, const TCPIP_MAC_BRIDGE_PERMANENT_ENTRY* pPermEntry);

// *****************************************************************************
/*
  Function:
    void  TCPIP_MAC_Bridge_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs periodic bridge FDB maintenance

  Precondition:
    The bridge module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/

void  TCPIP_MAC_Bridge_Task(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif //  __TCPIP_MAC_BRIDGE_H_








