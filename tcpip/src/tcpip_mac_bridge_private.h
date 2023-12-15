/*******************************************************************************
  TCP/IP MAC bridge file

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_mac_bridge_private.h

  Summary:
    Internal TCP/IP stack MAC bridge file
    
  Description:
    This header file contains the internal definitions of the 
    TCP/IP stack MAC Layer 2 bridge 
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

#ifndef __TCPIP_MAC_BRIDGE_PRIVATE_H_
#define __TCPIP_MAC_BRIDGE_PRIVATE_H_


// definitions
// 

// debug symbols
#define MAC_BRIDGE_DEBUG_MASK_BASIC         0x0001
#define MAC_BRIDGE_DEBUG_MASK_INIT          0x0002
#define MAC_BRIDGE_DEBUG_MASK_FDB_INTEGRITY 0x0004
#define MAC_BRIDGE_DEBUG_MASK_RX_IP_CHECK   0x0008
#define MAC_BRIDGE_DEBUG_MASK_TRACE_PKT     0x0010


#define MAC_BRIDGE_DEBUG_MASK       (1)

// because of the forwardMap,
// no more than 32 ports supported currently
// to minimize the run time allocated resources 
#define MAC_BRIDGE_MAX_SUPPORTED_PORTS      32

// local define symbols
#if defined(TCPIP_MAC_BRIDGE_STATISTICS) && (TCPIP_MAC_BRIDGE_STATISTICS != 0)
#define _TCPIP_MAC_BRIDGE_STATISTICS 1
#else
#define _TCPIP_MAC_BRIDGE_STATISTICS 0
#endif

#if defined(TCPIP_MAC_BRIDGE_EVENT_NOTIFY) && (TCPIP_MAC_BRIDGE_EVENT_NOTIFY  != 0) 
#define _TCPIP_MAC_BRIDGE_EVENT_NOTIFY   1
#else
#define _TCPIP_MAC_BRIDGE_EVENT_NOTIFY   0
#endif

#if defined(TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS) &&(TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS != 0)
#define _TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS 1
#else
#define _TCPIP_MAC_BRIDGE_DYNAMIC_FDB_ACCESS 0
#endif


// enumeration for statistics function
// specify the statistics index (type)
// keep it in sync with TCPIP_MAC_BRIDGE_STAT 
typedef enum
{
    MAC_BRIDGE_STAT_TYPE_FAIL_PKT_ALLOC = 0,
    MAC_BRIDGE_STAT_TYPE_FAIL_DCPT_ALLOC,
    MAC_BRIDGE_STAT_TYPE_FAIL_MAC,
    MAC_BRIDGE_STAT_TYPE_FAIL_MTU,
    MAC_BRIDGE_STAT_TYPE_FAIL_SIZE,
    MAC_BRIDGE_STAT_TYPE_FAIL_LOCK,
    MAC_BRIDGE_STAT_TYPE_FDB_FULL,

    MAC_BRIDGE_STAT_TYPE_ALLOC_PKTS,
    MAC_BRIDGE_STAT_TYPE_ALLOC_DCPTS,
    MAC_BRIDGE_STAT_TYPE_FREE_PKTS,
    MAC_BRIDGE_STAT_TYPE_ACK_PKTS,
    MAC_BRIDGE_STAT_TYPE_DELAY_PKTS,
    MAC_BRIDGE_STAT_TYPE_PKT_POOL_EMPTY,
    MAC_BRIDGE_STAT_TYPE_PKT_POOL_LOW,
    MAC_BRIDGE_STAT_TYPE_PKT_POOL_SIZE,
    MAC_BRIDGE_STAT_TYPE_DCPT_POOL_EMPTY,
    MAC_BRIDGE_STAT_TYPE_DCPT_POOL_SIZE,
    MAC_BRIDGE_STAT_TYPE_DCPT_POOL_LOW_SIZE,
    // per interface
    MAC_BRIDGE_STAT_TYPE_RX_PKTS = 0,
    MAC_BRIDGE_STAT_TYPE_RX_DEST_ME_UCAST,
    MAC_BRIDGE_STAT_TYPE_RX_DEST_NOT_ME_UCAST,
    MAC_BRIDGE_STAT_TYPE_RX_DEST_MCAST,
    MAC_BRIDGE_STAT_TYPE_RESERVED_PKTS,
    MAC_BRIDGE_STAT_TYPE_FWD_UCAST_PKTS,
    MAC_BRIDGE_STAT_TYPE_FWD_MCAST_PKTS,
    MAC_BRIDGE_STAT_TYPE_FWD_DIRECT_PKTS,
    
}MAC_BRIDGE_STAT_TYPE;


// Bridge input/output port descriptor
// This is advanced/not currently supported
typedef struct
{
    uint8_t userPriority;       // priority associated with this port
                                // IEEE 802.3 frames do not encode priority
                                // Value 0 - 7; Default is 0; Can be set by management
    uint8_t portState;          // whether the port forwards or not, it learns or not
                                // Discarding: excluded from forwarding and learning
                                // Learning: learning enabled but forwarding disabled
                                // Forwarding: learns and forwards
    


}MAC_BRIDGE_PORT_DCPT;

// static filtering entry in the filtering database
// under management control
// allows forwarding of frames with particular dest address 
// contains separate entry for each MAC address for each input port!
// The local interfaces addresses are added to the static FDB
typedef struct
{
    TCPIP_MAC_ADDR              destAdd;        // - individual MAC address, OR
                                                // - a group of MAC addresses, OR
                                                // ( optional, no current support - for EFS Extended Filtering Services:
                                                // - all Group addresses with no more specific Static Filtering Entry
                                                // - All unregistered Group Addresses for which no more Specific Static Filtering Entry exists)

    uint8_t                     outPortMap[];   // outPortMap[nPorts][nPorts]; nPorts is the number of ports in the bridge
                                                // for each input port, a TCPIP_MAC_BRIDGE_CONTROL_TYPE value for each outbound port specifying
                                                // what to do for an destAdd match
                                                // here 'n' is the number of the bridge ports - 2nd param is also 'n' for easier indexing (should be n - 1) 
}MAC_BRIDGE_STATIC_ENTRY;

// dynamic filtering entry in the filtering database
// created and updated by the learning process
// subject to ageing and removal
// max 1 entry for a given MAC address
// IEEE-802.1D-2004: 7.9.2. Dynamic Filtering Entries cannot be created or updated by management
// IEEE-802.1D-2004: 7.9.2. A Dynamic Filtering Entry shall not be created or updated by the Learning Process if any Static Filtering
// Entry already exists for this MAC Address with a control element specification, for the outbound Port
// specified by the Learning Process, that specifies forwarding or filtering irrespective of dynamic filtering information.

typedef struct
{
    TCPIP_MAC_ADDR              destAdd;        // individual MAC address
    uint8_t                     outPort;        // output forward port number
    uint8_t                     learnPort;      // port number this address has been learned on

    uint32_t                    tExpire;        // expiration time - range: [10 - 1,000,000] sec. Default 300s. Granularity 1 sec.
}MAC_BRIDGE_DYNAMIC_ENTRY;


// hash probe step
#define     MAC_BRIDGE_HASH_PROBE_STEP      1
#define     MAC_BRIDGE_HASH_KEY_SIZE        (sizeof(((MAC_BRIDGE_HASH_ENTRY*)0)->destAdd))


// bridge hash flags; part of the OA_HASH_ENTRY
// NOTE: the TCPIP_MAC_FDB_FLAGS are the low 8 bit part of the MAC_BRIDGE_HASH_FLAGS
// Keep them in sync!
typedef enum
{
    MAC_BRIDGE_HFLAG_NONE        = 0,           // no flag set
    MAC_BRIDGE_HFLAG_STATIC      = 0x0100,      // entry is static and permanent, not from learning; set by administrator
                                                // otherwise dynamic entry
    MAC_BRIDGE_HFLAG_HOST        = 0x0200,      // entry is for one of the interfaces belonging to this host
    MAC_BRIDGE_HFLAG_PORT_VALID  = 0x0400,      // the learnPort value is valid - mainly for a static entry

    MAC_BRIDGE_HFLAG_MASK        = 0x0700,      // all flags mask
}MAC_BRIDGE_HASH_FLAGS;

// Bridge FDB entry
typedef struct _TAG_MAC_BRIDGE_HASH_ENTRY 
{
    OA_HASH_ENTRY       hEntry;         // hash header;
    // use the same filtering database for both static and dynamic entries
    uint32_t            tExpire;        // dynamic entry: expiration time - range: [10 - 1,000,000] sec. Default 300s. Granularity 1 sec.
    uint32_t            fwdPackets;     // number of packets forwarded for this destination address
    TCPIP_MAC_ADDR      destAdd;        // dynamic entry - individual MAC address
                                        // static entry - individual MAC address or a group of MAC addresses
    uint8_t             learnPort;      // dynamic entry: port number this address has been learned on:
                                        //          this is where the packets with this destination will be forwarded
                                        // static entry: port number this address has been learned on
                                        //          if the static control for the input port specifies default,
                                        //          the learnPort (if valid) is used to forward the packet, and only on this port
                                        //          if the learnPort is invalid, then default is flood
                                        //  a static entry updates the learnPort as well and mark it as valid
                                        //  and then uses it when a static entry specifies 'default' behavior for a port...
                                        //
    uint8_t             outPortMap[TCPIP_MAC_BRIDGE_MAX_PORTS_NO][TCPIP_MAC_BRIDGE_MAX_PORTS_NO];
                                        // static entry, TCPIP_MAC_BRIDGE_CONTROL_TYPE type
                                        // for static entries, the outPotMap[k][j] describes the behavior (process/discard) 
                                        // on the port j of a packet arriving on port k
                                        // this map entry is not otherwise used
}MAC_BRIDGE_HASH_ENTRY;


// the MAC bridge hash descriptor
// the FDB is built of hash entries
// access to the FDB has to be protected against multi-thread access
// but in such a way as not to halt the bridge processing of the packets because of the user intervention!
//
typedef struct
{
    OA_HASH_DCPT*       hashDcpt;       // contiguous space for a hash descriptor
                                        // and hash table entries

    const void*         memH;           // memory allocation handle
    uint32_t            purgeTmo;       // purge timeout: expiration time for a dynamic entry, in SYS_TICKS
                                        // range: [10 - 1,000,000] sec. Default 300s. Granularity 1 sec.
    uint32_t            transitDelay;   // allowed bridge transit delay, in SYS_TICKS
    tcpipSignalHandle   tmrSigHandle;   // registration with the manager for periodic timer purposes
    uint16_t            pktPoolSize;    // number of packets in the pool for run time forwarding
    uint16_t            pktSize;        // size of the packets in the pool
    uint8_t             pktReplenish;   // Number of packets to replenish the pool, when it becomes empty
    uint8_t             nPorts;         // number of ports in the bridge
    uint8_t             bridgeLock;     // access to FDB is locked
    int8_t              status;         // current bridge status: SYS_STATUS
    
    SINGLE_LIST         pktPool;        // packet pool
                                        // no protection enforced on access to the packet pool 
                                        // for the (remove, replenish, ack) operations
                                        // These should all occur on the TCPIP-STACK_Task context
                                        // as long as there's not other thread feeding the bridge
    SINGLE_LIST         dcptPool;       // MAC_BRIDGE_FWD_DCPT descriptor pool
                                        // each packet to be processed by the bridge has an associated descriptor
                                        // no protection enforced on access to the descriptor pool 
    // statistics
#if (_TCPIP_MAC_BRIDGE_STATISTICS != 0)
    union
    {
        TCPIP_MAC_BRIDGE_STAT   stat;    
        uint32_t                stat_array[sizeof(TCPIP_MAC_BRIDGE_STAT)];
    };
#endif  // (_TCPIP_MAC_BRIDGE_STATISTICS != 0)
    
#if (_TCPIP_MAC_BRIDGE_EVENT_NOTIFY  != 0) 
    TCPIP_MAC_BRIDGE_EVENT_HANDLER  evHandler;
#endif  // (_TCPIP_MAC_BRIDGE_EVENT_NOTIFY  != 0) 

    uint8_t             bridgeFlags;        // TCPIP_MAC_BRIDGE_FLAGS value
    uint8_t             dcptReplenish;      // Number of descriptors to replenish the pool, when it becomes empty
    uint8_t             port2IfIx[TCPIP_MAC_BRIDGE_MAX_PORTS_NO];   // table with the interface indexes corresponding to the port
    
}MAC_BRIDGE_DCPT;

typedef enum
{
    MAC_BRIDGE_FWD_FLAG_NONE    = 0x00,     // no flag set
    MAC_BRIDGE_FWD_FLAG_MCAST   = 0x01,     // forward for a multicast address
    //
    // ...
}MAC_BRIDGE_FWD_FLAGS;

typedef enum
{
    MAC_BRIDGE_ALLOC_FLAG_NONE          = 0x00,     // no flag set
    MAC_BRIDGE_ALLOC_FLAG_BRIDGE_OWN    = 0x01,     // the packet belongs to the bridge
                                                    // otherwise is just the packet coming from the MAC driver
    MAC_BRIDGE_ALLOC_FLAG_STICKY        = 0x02,     // packet belongs to the pool, should be returned
                                                    // otherwise should be freed    
                                                    // only if MAC_BRIDGE_ALLOC_FLAG_BRIDGE_OWN set
    //
    // ...
}MAC_BRIDGE_ALLOC_FLAGS;


// map of the ports need to be forwarded/flooded
// limits the bridge ports to <= 32!
typedef struct _tag_MAC_BRIDGE_FWD_DCPT
{
    struct _tag_MAC_BRIDGE_FWD_DCPT* next;  // safe cast to SGL_LIST_NODE   
    const void*               ownAckParam;  
    TCPIP_MAC_PACKET_ACK_FUNC ownAckFunc;   // when processed packet does not belong to the bridge

    uint32_t        tReceive;       // SYS_TICK when frame was received
                                    // monitored for ensuring the bridge transit delay - Table 3.7
    uint32_t        fwdMap32;       // map of ports to be forwarded/flooded
    uint16_t        pktLen;         // packet length
    uint16_t        fwdFlags;       // MAC_BRIDGE_FWD_FLAGS value
}MAC_BRIDGE_FWD_DCPT;
    
// special port control for static FDB entries that rely on the learnt behavior as a default
// Note: has to be different from the TCPIP_MAC_BRIDGE_CONTROL_TYPE values! 
#define TCPIP_MAC_BRIDGE_CONTROL_TYPE_DYN_LEARN (TCPIP_MAC_BRIDGE_CONTROL_TYPES + 1) 


#endif //  __TCPIP_MAC_BRIDGE_PRIVATE_H_


