/*******************************************************************************
  TCPIP network packet manager - private stack API

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_packet.h

  Summary:
    
  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2012-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef H_TCPIP_PACKET_H_
#define H_TCPIP_PACKET_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "tcpip/tcpip_mac.h"

// enables packet trace
// This is used to trace packet allocation, by module
//#define TCPIP_PACKET_ALLOCATION_TRACE_ENABLE

// global trace info
typedef struct
{
    size_t nEntries;           // all trace entries
    size_t nUsed;              // used ones
    size_t traceFails;         // failed to allocate/find a packet entry
    size_t traceAckErrors;     // packets acknowledged with an error code
    size_t traceAckOwnerFails; // packets acknowledged but no module owner found
}TCPIP_PKT_TRACE_INFO;

// packet trace
// the moduleId is the one from tcpip.h::TCPIP_STACK_MODULE
// only if TCPIP_PACKET_ALLOCATION_TRACE_ENABLE defined
typedef struct
{
    TCPIP_STACK_MODULE  moduleId;   // info belonging to this module; <0 means slot free
    uint32_t    totAllocated;       // total number of packets allocated successfully by this module
    uint32_t    currAllocated;      // number of packets still allocated by this module
    uint32_t    currSize;           // number of bytes currently allocated
    uint32_t    totFailed;          // total number of packets that failed for this module
    uint32_t    nAcks;              // number of acknowledgments
}TCPIP_PKT_TRACE_ENTRY;

// number of trace slots
// each module that allocates packets should have its slot.
// currently: tcp, udp, icmp, arp, ipv6
#define TCPIP_PKT_TRACE_SIZE        8

// module and packet logging flags
// only if TCPIP_PACKET_LOG_ENABLE is enabled
//

// global attributes that can be applied to logging
// Note: TCPIP_PKT_LOG_TYPE_RX_ONLY and TCPIP_PKT_LOG_TYPE_TX_ONLY are mutually exclusive and
// cannot be both set!
typedef enum
{
    TCPIP_PKT_LOG_TYPE_RX_TX        = 0x0000,             // log both RX and TX traffic - default
    TCPIP_PKT_LOG_TYPE_RX_ONLY      = 0x0001,             // log only RX traffic
    TCPIP_PKT_LOG_TYPE_TX_ONLY      = 0x0002,             // log only TX traffic
    TCPIP_PKT_LOG_TYPE_SKT_ONLY     = 0x0004,             // log only socket traffic

    //
    //
    TCPIP_PKT_LOG_TYPE_HANDLER_ALL  = 0x1000,             // forward to the external handler all logs,
                                                          // regardless of the log mask

}TCPIP_PKT_LOG_TYPE;


typedef enum
{
    // flags that are stored as part of the entry
    TCPIP_PKT_LOG_FLAG_NONE         = 0x0000,   // no flag set
    TCPIP_PKT_LOG_FLAG_RX           = 0x0001,   // This is a RX packet
    TCPIP_PKT_LOG_FLAG_TX           = 0x0002,   // This is a TX packet
    TCPIP_PKT_LOG_FLAG_SKT_PARAM    = 0x0004,   // This is log containing socket info
    TCPIP_PKT_LOG_FLAG_PERSISTENT   = 0x0008,   // the log will be kept even after the packet is acknowledged
                                                // by default, once the packet is acknowledged, the packet will be discarded from the log
                                                // to make room for other packets

    // internal flags, not used in a log call: 0x1000 - 0x8000
    TCPIP_PKT_LOG_FLAG_DONE         = 0x1000,   // the log for this packet is completed, no other module will log it
                                                // used when packet is acknowledged
    TCPIP_PKT_LOG_FLAG_NEW          = 0x2000,   // a new log was created for this packet
    TCPIP_PKT_LOG_FLAG_REPLACE      = 0x4000,   // an existing log entry was used for this packet


    TCPIP_PKT_LOG_FLAG_INT_MASK     = 0xf000,   // internal flags mask
}TCPIP_PKT_LOG_FLAGS;

// structure describing the log of a packet
typedef struct
{
    TCPIP_MAC_PACKET*   pPkt;               // packet that's logged
    uint16_t            moduleLog;          // 16 bit mask of modules that processed the packet
                                            // each module that processes the packet sets its corresponding bit (1 << moduleId) in this mask
                                            // only the basic modules log their packets
                                            // TCPIP_MODULE_LAYER3 will show up as the module ID for a packet handed to a higher layer
    uint16_t            pktOwner;           // TCPIP_STACK_MODULE: module that owns the packet/allocated it, etc.
                                            // usually this is the 1st module that calls the log
    uint16_t            macId;              // TCPIP_STACK_MODULE: the ID of the MAC involved in the packet processing
                                            // These IDs are too big to be represented as a bit in the moduleLog
    uint16_t            netMask;            // corresponding network mask
    uint16_t            pktAcker;           // TCPIP_STACK_MODULE: module that acknowledges the packet.
    uint16_t            sktNo;              // for UDP and TCP sockets, the socket number
    uint16_t            lclPort;            // for TCP and UDP sockets, the local port number
    uint16_t            remPort;            // for TCP and UDP sockets, the remote port number

    int16_t             ackRes;             // acknowledge result; TCPIP_MAC_PKT_ACK_RES
    uint16_t            logFlags;           // log flags;     
    // time stamps; SYS_TIME_CounterGet()
    uint32_t            macStamp;           // MAC driver tstamp
    uint32_t            ackStamp;           // acknowledge tstamp
    uint32_t            moduleStamp[TCPIP_MODULE_LAYER3];   // each module tstamp

}TCPIP_PKT_LOG_ENTRY;


// function that can receive notification when a new log is done for a packet
// the moduleId is the module making the log in the pLogEntry
// if pLogEntry == 0, then no entry could be found for that log (all used)
typedef void    (*TCPIP_PKT_LOG_HANDLER)(TCPIP_STACK_MODULE moduleId, const TCPIP_PKT_LOG_ENTRY* pLogEntry);


// global logging info
typedef struct
{
    uint16_t                nEntries;           // all log entries
    uint16_t                nUsed;              // used ones
    uint16_t                nPersistent;        // non deletable ones
    uint16_t                nFailed;            // failed to log because all the slots were taken...
    uint32_t                persistMask;        // current persistent mask
    uint32_t                logModuleMask;      // current module logging mask
    uint32_t                netLogMask;         // current mask of networks the logging is enabled on
    uint32_t                sktLogMask;         // current mask of sockets to be logged
    TCPIP_PKT_LOG_TYPE      logType;            // current log type
    TCPIP_PKT_LOG_HANDLER   logHandler;         // current log handler

}TCPIP_PKT_LOG_INFO;

// Extra TX/RX packet flags
// NOTE: // 16 bits only packet flags!

#define    TCPIP_MAC_PKT_FLAG_ARP           ((uint32_t)TCPIP_MAC_PKT_FLAG_USER << 0)    // ARP packet data
                                                                            // set when the packet is ARP
                                                                            //
#define    TCPIP_MAC_PKT_FLAG_NET_TYPE      ((uint32_t)TCPIP_MAC_PKT_FLAG_USER << 1)    // the network type: IPv4/IPv6 packet
#define    TCPIP_MAC_PKT_FLAG_IPV4          (0U)                                        // IPv4 packet data; cleared when the packet is IPV4
#define    TCPIP_MAC_PKT_FLAG_IPV6          ((uint32_t)TCPIP_MAC_PKT_FLAG_USER << 1)    // IPv6 packet data; set when the packet is IPV6

#define    TCPIP_MAC_PKT_FLAG_LLDP          ((uint32_t)TCPIP_MAC_PKT_FLAG_USER << 2)    // LLDP packet data; set when the packet is LLDP

#define    TCPIP_MAC_PKT_FLAG_ICMP_TYPE     ((uint32_t)TCPIP_MAC_PKT_FLAG_USER << 3)    // ICMP packet type
#define    TCPIP_MAC_PKT_FLAG_ICMPV4        (0U)                                        // ICMPv4 packet data; cleared when the packet is ICMPv4
#define    TCPIP_MAC_PKT_FLAG_ICMPV6        ((uint32_t)TCPIP_MAC_PKT_FLAG_USER << 3)    // ICMPv6 packet data; set when the packet is ICMPv6

#define    TCPIP_MAC_PKT_FLAG_NDP           ((uint32_t)TCPIP_MAC_PKT_FLAG_USER << 4)    // NDP packet data; set when the packet is NDP

#define    TCPIP_MAC_PKT_FLAG_TRANSP_TYPE   ((uint32_t)TCPIP_MAC_PKT_FLAG_USER << 5)    // UDP/TCP packet type
#define    TCPIP_MAC_PKT_FLAG_UDP           (0U)                                        // UDP packet data; cleared when the packet is UDP
#define    TCPIP_MAC_PKT_FLAG_TCP           ((uint32_t)TCPIP_MAC_PKT_FLAG_USER << 5)    // TCP packet data; set when the packet is TCP

#define    TCPIP_MAC_PKT_FLAG_IGMP          ((uint32_t)TCPIP_MAC_PKT_FLAG_USER << 6)    // IGMP packet


                                            // packet type extraction mask
#define    TCPIP_MAC_PKT_FLAG_TYPE_MASK     (TCPIP_MAC_PKT_FLAG_ARP | TCPIP_MAC_PKT_FLAG_NET_TYPE | TCPIP_MAC_PKT_FLAG_LLDP | TCPIP_MAC_PKT_FLAG_ICMP_TYPE | TCPIP_MAC_PKT_FLAG_NDP | TCPIP_MAC_PKT_FLAG_TRANSP_TYPE)

#define    TCPIP_MAC_PKT_FLAG_CONFIG        ((uint32_t)TCPIP_MAC_PKT_FLAG_USER << 7)  // packet needs to be transmitted even when the stack
                                                                            // is not properly configured
                                                                            // probably a stack configuration packet


// initialization API

// sets the heap handle to be used for packet allocations
bool            TCPIP_PKT_Initialize(TCPIP_STACK_HEAP_HANDLE heapH, const TCPIP_NETWORK_CONFIG* pNetConf, size_t nNets);

void            TCPIP_PKT_Deinitialize(void);


// packet helpers
//
// set the packet ack function
static __inline__ void __attribute__((always_inline)) TCPIP_PKT_PacketAcknowledgeSet(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PACKET_ACK_FUNC ackFunc, const void* ackParam)
{
    pPkt->ackFunc = ackFunc;
    pPkt->ackParam = ackParam;
}

// adds a segment to the tail of segments of a packet
// segment should be fully constructed, with flags updated
void            TCPIP_PKT_SegmentAppend(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_DATA_SEGMENT* pSeg);           

// sets the proper source, destination and type for a packet
// it also updated the packet length to include the MAC header size
// dstAddr can be 0 if not known
// packet should have been properly allocated and pMacLayer set
// returns false if the srcAddress is 0 though
// (intended for checking that a network interface is down)
bool             TCPIP_PKT_PacketMACFormat(TCPIP_MAC_PACKET* pPkt, const TCPIP_MAC_ADDR* dstAddr, const TCPIP_MAC_ADDR* srcAddr, uint16_t pktType);


// returns the segment to which dataAddress belongs
// the search occurs in every segment of the packet
// if srchTransport is set, the search starts with the transport data, i.e.
//      startPoint = pTransportLayer,
//      endPoint = startPoint + totTransportLen
// otherwise
//      startPoint = segLoad
//      endPoint = startPoint + segSize
// 0 if not in this packet
TCPIP_MAC_DATA_SEGMENT* TCPIP_PKT_DataSegmentGet(TCPIP_MAC_PACKET* pPkt, const uint8_t* dataAddress, bool srchTransport);


// simple helper to calculate the payload length of a packet
uint16_t        TCPIP_PKT_PayloadLen(TCPIP_MAC_PACKET* pPkt);

// returns the allocation gapDcptOffset:
// Offset in bytes between the address pointed by segBuffer
// and the address of the TCPIP_MAC_SEGMENT_GAP_DCPT for this segment buffer is stored.
// This offset has the same value for all packets RX/TX packets passed to the driver.
//  TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = (TCPIP_MAC_SEGMENT_GAP_DCPT*)(TCPIP_MAC_DATA_SEGMENT::segBuffer + gapDcptOffset);
//  TCPIP_MAC_PACKET* pMacPkt = pGap->segmentPktPtr;
int16_t    TCPIP_PKT_GapDcptOffset(void);

// returns the allocation gapDcptSize:
// Size of the TCPIP_MAC_SEGMENT_GAP_DCPT structure (the segmentDataGap size is variable) 
// It specifies the gap space present in the TCPIP_MAC_SEGMENT_GAP_DCPT
// gapDcptSize ==  sizeof(TCPIP_MAC_SEGMENT_GAP_DCPT) == 
// == sizeof(TCPIP_MAC_SEGMENT_GAP_DCPT::segmentPktPtr) + sizeof(TCPIP_MAC_SEGMENT_GAP_DCPT::segmentDataGap)
// It has the same value for all packets allocated by the stack.
uint16_t    TCPIP_PKT_GapDcptSize(void);


#if !defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)
// allocation functions, no trace version

// proto
// allocates a TCPIP_MAC_PACKET packet (TCPIP_MAC_ETHERNET_HEADER always added);
// pktLen - the size of the packet (it will be 32 bits rounded up)
// segLoadLen - the payload size for the segment associated to this packet; Payload is always cache line aligned
//              if 0 no segment is created
// flags are attached to the 1st segment too 
TCPIP_MAC_PACKET*   F_TCPIP_PKT_PacketAlloc(uint16_t pktLen, uint16_t segLoadLen, TCPIP_MAC_PACKET_FLAGS flags);

// forces freeing a previously allocated packet
// note that neither the packet nor segments marked
// with TCPIP_MAC_PKT_FLAG_STATIC are not freed
// Also note that this function does not free explicitly the segment payload.
// A payload that was created contiguously when the segment was created
// will be automatically freed by this function.
void                F_TCPIP_PKT_PacketFree(TCPIP_MAC_PACKET* pPkt);

// acknowledges a packet
// clears the TCPIP_MAC_PKT_FLAG_QUEUED flag!
// a packet should always have an acknowledgment function
// packet's ackRes is updated only if the parameter ackRes != TCPIP_MAC_PKT_ACK_NONE.
void                F_TCPIP_PKT_PacketAcknowledge(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes, int moduleId);

// allocates a socket/transport IPv4/IPv6 packet
// The MAC, IPv4/IPv6 and transport headers (tHdrLen != 0) are all located in the 1st segment
// if payloadLen != 0 then the 1st segment will contain the/some payload too
// if needed, extra segments could be eventually added to the packet
// pktLen - size of the packet structure (at least TCPIP_MAC_PACKET will be allocated)
// tHdrLen - optional transport header length to be part of the 1st data segment
// payloadLen - optional transport payload to be part of the 1st data segment
// flags     - packet + 1st segment flags
TCPIP_MAC_PACKET*   F_TCPIP_PKT_SocketAlloc(uint16_t pktLen, uint16_t transpHdrLen, uint16_t payloadLen, TCPIP_MAC_PACKET_FLAGS flags);

// allocates a segment with payload following immediately the segment header 
// this segment can be added to a packet using TCPIP_PKT_SegmentAppend
// loadLen specifies the segment allocated payload (could be 0)
// The segment payload is always allocated to be cache line aligned.
// The segment payload pointer will point TCPIP_MAC_MODULE_CTRL::gapDcptOffset bytes after this address 
TCPIP_MAC_DATA_SEGMENT* F_TCPIP_PKT_SegmentAlloc(uint16_t loadLen, TCPIP_MAC_SEGMENT_FLAGS flags);

// frees a created segment
void                F_TCPIP_PKT_SegmentFree(TCPIP_MAC_DATA_SEGMENT* pSeg);



// macro calls

#define TCPIP_PKT_PacketAlloc(pktLen, segLoadLen, flags) F_TCPIP_PKT_PacketAlloc(pktLen, segLoadLen, flags)
#define TCPIP_PKT_PacketFree(pPkt) F_TCPIP_PKT_PacketFree(pPkt)
#define TCPIP_PKT_PacketAcknowledge(pPkt, ackRes) F_TCPIP_PKT_PacketAcknowledge(pPkt, ackRes, (int)TCPIP_THIS_MODULE_ID)
#define TCPIP_PKT_SocketAlloc(pktLen, tHdrLen, payloadLen, flags) F_TCPIP_PKT_SocketAlloc(pktLen, tHdrLen, payloadLen, flags)
#define TCPIP_PKT_SegmentAlloc(loadLen, flags) F_TCPIP_PKT_SegmentAlloc(loadLen, flags)
#define TCPIP_PKT_SegmentFree(pSeg) F_TCPIP_PKT_SegmentFree(TCPIP_MAC_DATA_SEGMENT* pSeg)

#define TCPIP_PKT_TraceGetEntriesNo(pTraceInfo)


// populates a trace entry with data for a index
#define TCPIP_PKT_TraceGetEntry(entryIx, tEntry) do{return false;}while(0)


#else   // defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)
// debug version (trace) of packet allocation functions

// proto
TCPIP_MAC_PACKET*   F_TCPIP_PKT_PacketAllocDebug(uint16_t pktLen, uint16_t segLoadLen, TCPIP_MAC_PACKET_FLAGS flags, TCPIP_STACK_MODULE moduleId);
void                F_TCPIP_PKT_PacketFreeDebug(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId);
void                F_TCPIP_PKT_PacketAcknowledgeDebug(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes, TCPIP_STACK_MODULE moduleId);

TCPIP_MAC_PACKET*   F_TCPIP_PKT_SocketAllocDebug(uint16_t pktLen, uint16_t tHdrLen, uint16_t payloadLen, TCPIP_MAC_PACKET_FLAGS flags, TCPIP_STACK_MODULE moduleId);

TCPIP_MAC_DATA_SEGMENT* F_TCPIP_PKT_SegmentAllocDebug(uint16_t loadLen, TCPIP_MAC_SEGMENT_FLAGS flags, TCPIP_STACK_MODULE moduleId);
void                F_TCPIP_PKT_SegmentFreeDebug(TCPIP_MAC_DATA_SEGMENT* pSeg, TCPIP_STACK_MODULE moduleId);

// macro calls
#define TCPIP_PKT_PacketAlloc(pktLen, segLoadLen, flags) F_TCPIP_PKT_PacketAllocDebug(pktLen, segLoadLen, flags, TCPIP_THIS_MODULE_ID)
#define TCPIP_PKT_PacketFree(pPkt) F_TCPIP_PKT_PacketFreeDebug(pPkt, TCPIP_THIS_MODULE_ID)
#define TCPIP_PKT_PacketAcknowledge(pPkt, ackRes) F_TCPIP_PKT_PacketAcknowledgeDebug(pPkt, ackRes, TCPIP_THIS_MODULE_ID)
#define TCPIP_PKT_SocketAlloc(pktLen, tHdrLen, payloadLen, flags) F_TCPIP_PKT_SocketAllocDebug(pktLen, tHdrLen, payloadLen, flags, TCPIP_THIS_MODULE_ID)
#define TCPIP_PKT_SegmentAlloc(loadLen, flags) F_TCPIP_PKT_SegmentAllocDebug(loadLen, flags, TCPIP_THIS_MODULE_ID)
#define TCPIP_PKT_SegmentFree(pSeg) F_TCPIP_PKT_SegmentFreeDebug(TCPIP_MAC_DATA_SEGMENT* pSeg, TCPIP_THIS_MODULE_ID)

typedef TCPIP_MAC_PACKET* (*TCPIP_PKT_ALLOC_FNC_DBG)(uint16_t pktLen, uint16_t segLoadLen, TCPIP_MAC_PACKET_FLAGS flags, TCPIP_STACK_MODULE moduleId);
typedef void (*TCPIP_PKT_FREE_FNC_DBG)(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId);
typedef void (*TCPIP_PKT_ACK_FNC_DBG)(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes, TCPIP_STACK_MODULE moduleId);

// returns the number of entries in the trace
size_t     TCPIP_PKT_TraceGetEntriesNo(TCPIP_PKT_TRACE_INFO* pTraceInfo);

// populates a trace entry with data for a index
// returns true if the entry is active - has valid data
bool    TCPIP_PKT_TraceGetEntry(size_t entryIx, TCPIP_PKT_TRACE_ENTRY* tEntry);


#endif  // !defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)

#if (TCPIP_PACKET_LOG_ENABLE != 0)
// logs a TX packet info
void    TCPIP_PKT_FlightLogTx(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId);

// logs a RX packet info;
void    TCPIP_PKT_FlightLogRx(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId);

// logs a TX packet info for a socket
// lclRemPort contains both ports: (lcl << 16 | remPort)
void    TCPIP_PKT_FlightLogTxSkt(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId, uint32_t lclRemPort, uint16_t sktNo );

// logs a RX packet info for a socket; on RX the pkt IF is already known 
// lclRemPort contains both ports: (lcl << 16 | remPort)
void    TCPIP_PKT_FlightLogRxSkt(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId, uint32_t lclRemPort, uint16_t sktNo );

// acknowledges a packet when a module is done with it
// this should be the last log call for this packet
void    TCPIP_PKT_FlightLogAcknowledge(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId, TCPIP_MAC_PKT_ACK_RES ackRes);

// gets logger info
// returns true if there's log info available, false otherwise
bool     TCPIP_PKT_FlightLogGetInfo(TCPIP_PKT_LOG_INFO* pLogInfo);

// populates a log entry with data for a index
// returns true if the entry is active - has valid data
bool    TCPIP_PKT_FlightLogGetEntry(size_t entryIx, TCPIP_PKT_LOG_ENTRY* pLEntry);

// registers a log handler
// there is only one handler in the system, no protection
// call with 0 to deregister
// if logAll is true, then the log type and the log mask are ignored
// and all packets will be reported as they are received
// returns true if success, false otherwise
bool    TCPIP_PKT_FlightLogRegister(TCPIP_PKT_LOG_HANDLER logHandler, bool logAll);


// updates the persistence attribute to all existent logs based on the new resulting persistence mask
// any log that has persistent records will have its persistent flag set/cleared
// according to the resulting mask
// The persistMask will be updated: persistMask = (persistMask & andModuleMask) | orModuleMask
// The andModuleMask and orModuleMask are constructed using the moduleId (like the moduleLog) i.e. 1 << moduleId
// If clrNonPersist is true, the log entries that are made non persistent will be discarded
// The module mask should be limited only to the basic modules: <= TCPIP_MODULE_LAYER3
// Default value: at start up the persistMask is cleared i.e. no log is persistent.
// Note: the call will search and apply the new attribute in all stored log entries
// so it can be expensive!
// Note: the persistent mask takes precedence over the logging one
void    TCPIP_PKT_FlightLogUpdatePersistMask(uint32_t andModuleMask, uint32_t orModuleMask, bool clrNonPersist);

// updates the mask of the logged modules in the logging service
// any completed log that has recorded modules not matching the resulting log mask
// will be discarded
// By default the completed logs are kept until replaced by a new log
// (unless the log is persistent)
// The andModuleMask and orModuleMask are constructed using the moduleId (like the moduleLog) i.e. 1 << moduleId
// The logModuleMask will be updated: logModuleMask = (logModuleMask & andModuleMask) | orModuleMask
// If clrPersist is true, the non matching log entries will be discarded even if they are persistent
// Else only non persistent entries will be deleted.
// The module mask should be limited only to the basic modules: <= TCPIP_MODULE_LAYER3
// Default value: at start up the logModuleMask is cleared i.e. no module is logged.
// Note: the call will search and discard logs in all stored log entries
// so it can be expensive!
// Note: the persistent mask takes precedence over the logging one
void    TCPIP_PKT_FlightLogUpdateModuleMask(uint32_t andModuleMask, uint32_t orModuleMask, bool clrPersist);

// updates the mask of logged interfaces in the logging service
// The andNetMask and orNetMask are constructed as the 1 << netIx
// where the network index is the number of that specific interface
// The netLogMask mask will be updated:  netLogMask = (netLogMask & andNetMask) | orNetMask
// If clrPersist is true, the matching log entries will be discarded even if they are persistent
// Else only non persistent entries will be deleted.
// Default value: at start up the logger will be enabled on a specific interface as directed by
// TCPIP_NETWORK_CONFIG_FLAGS::TCPIP_NETWORK_CONFIG_PKT_LOG_ON flag
void    TCPIP_PKT_FlightLogUpdateNetMask(uint32_t andNetMask, uint32_t orNetMask, bool clrPersist);

// updates the mask of logged sockets in the logging service
// The andSktMask and orSktMask are constructed as the 1 << sktIx
// where the socket index is the number of that specific socket
// The sktLogMask mask will be updated:  sktLogMask = (sktLogMask & andSktMask ) | orSktMask;
// If clrPersist is true, the matching log entries will be discarded even if they are persistent
// Else only non persistent entries will be deleted.
// Default value: at start up the sktLogMask is cleared i.e. no sockets are logged.
void    TCPIP_PKT_FlightLogUpdateSocketMask(uint32_t andSktMask, uint32_t orSktMask, bool clrPersist);

// applies the attributes to all logs
// - 
// - TCPIP_PKT_LOG_TYPE_RX_TX is global and is applied/removed to all logs:
//   both RX and TX packets will be logged - this is the default
//
// - TCPIP_PKT_LOG_TYPE_RX_ONLY is global and is applied/removed to all logs:
//   only RX packets will be logged
//   and the log will be cleared of TX packets
//
// - TCPIP_PKT_LOG_TYPE_TX_ONLY is global and is applied/removed to all logs:
//   only TX packets will be logged
//   and the log will be cleared of RX packets
//
// - TCPIP_PKT_LOG_TYPE_SKT_ONLY is global and is applied/removed to all logs:
//   only socket packets will be logged
//   and the log will be cleared of nonsocket packets
//
// Returns:
//      true if the call succeeded
//      false otherwise (wrong parameter)
//
// Notes:
//      - The RX_ONLY and TX_ONLY  are global.
//        They are applied before TCPIP_PKT_LOG_TYPE_SKT_ONLY;
//
//        These filters are applied before the PERSIST and MODULE masks.
//
//      - Setting a type to RX or TX only will search and discard logs in all stored log entries
//        so it can be expensive!
//        If clrPersist is true the persistent entries will be deleted if they do not match the type!
//
//      - TCPIP_PKT_LOG_TYPE_RX_ONLY and TCPIP_PKT_LOG_TYPE_TX_ONLY are mutually exclusive and
//        cannot be both set!
//
bool    TCPIP_PKT_FlightLogTypeSet(TCPIP_PKT_LOG_TYPE logType, bool clrPersist);


// clears all log
// if clrPersist is true, it clears persistent entries too
// Note: the operation clears only completed packets. Packets not yet acknowledged 
// will not be removed from the log
void    TCPIP_PKT_FlightLogClear(bool clrPersist);

// resets the log
// the operation will forcefully initialize the log and any previous data will be lost
// if resetMasks is true, the current log masks are cleared too
// otherwise just the log data is initialized
// Note: this may result in losing some information for packets that are in traffic
// at the time the reset is called
void    TCPIP_PKT_FlightLogReset(bool resetMasks);

#else   // !(TCPIP_PACKET_LOG_ENABLE)

#define TCPIP_PKT_FlightLogTx(pPkt, moduleId)

#define TCPIP_PKT_FlightLogRx(pPkt, moduleId)

#define TCPIP_PKT_FlightLogTxSkt(pPkt, moduleId, lclRemPort, sktNo )
#define TCPIP_PKT_FlightLogRxSkt(pPkt, moduleId, lclRemPort, sktNo )

#define TCPIP_PKT_FlightLogAcknowledge(pPkt, moduleId, ackRes)

#endif  // (TCPIP_PACKET_LOG_ENABLE != 0)


#endif // H_TCPIP_PACKET_H_


