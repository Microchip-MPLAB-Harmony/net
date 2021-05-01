/*******************************************************************************
  IGMP private file for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    igmp_private.h

  Summary:
    
  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2016-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/








// DOM-IGNORE-END

#ifndef _IGMP_PRIVATE_H_
#define _IGMP_PRIVATE_H_

// internal

// The size of the ifMask in the IGMP descriptor is uint32_t
#if (TCPIP_IGMP_INTERFACES > 32)
#error "IGMP error: maximum 32 interfaces supported!"
#endif

// debugging
#define TCPIP_IGMP_DEBUG_MASK_BASIC           (0x0001)
#define TCPIP_IGMP_DEBUG_MASK_CONSISTENCY     (0x0002)
#define TCPIP_IGMP_DEBUG_MASK_EVENTS          (0x0004)
#define TCPIP_IGMP_DEBUG_MASK_REPORT_LISTS    (0x0008)
#define TCPIP_IGMP_DEBUG_MASK_QUERY_MSG       (0x0010)
#define TCPIP_IGMP_DEBUG_MASK_THREAD_LOCK     (0x0020)

// enable IPV4 debugging levels
#define TCPIP_IGMP_DEBUG_LEVEL                (0)

// step to advance for hash collision
#define TCPIP_IGMP_GROUP_HASH_PROBE_STEP           1

// step to advance for hash collision
#define TCPIP_IGMP_SOURCE_HASH_PROBE_STEP          1

// default IANA SSM range
#define TCPIP_IGMP_IANA_SSM_RESERVED                0x000000e8
#define TCPIP_IGMP_IANA_SSM_LOW                     0x010000e8
#define TCPIP_IGMP_IANA_SSM_HIGH                    0xffffffe8

// other IANA reserved addresses
// All systems multicast: 224.0.0.1
#define TCPIP_IGMP_IANA_ALL_SYSTEMS_MULTICAST       0x010000e0
// Routers multicast listen: 224.0.0.22
#define TCPIP_IGMP_IANA_ALL_ROUTERS_MULTICAST       0x160000e0


// Only 1 source for IGMPv2 style
#if (TCPIP_IGMPV2_SUPPORT_ONLY != 0)
#define     _TCPIP_IGMP_SOURCES_PER_GROUP           1
#else
#define     _TCPIP_IGMP_SOURCES_PER_GROUP           TCPIP_IGMP_SOURCES_PER_GROUP          
#endif

// TCPIP_IGMP_SOURCES_PER_GROUP * 2 for v3, 1 for v2
#if (TCPIP_IGMPV2_SUPPORT_ONLY != 0)
#define     _TCPIP_IGMP_SOURCE_ADDRESSES_PER_GROUP  1
#else
#define     _TCPIP_IGMP_SOURCE_ADDRESSES_PER_GROUP  (_TCPIP_IGMP_SOURCES_PER_GROUP * 2)
#endif

// reserved source address for exclude nothing/include all
// NOT a mcast address!
#define TCPIP_IGMP_ASM_ALL_SOURCES                  0xffffffff

// limit of maxRespCode 
#define TCPIP_IGMP_MAX_RESP_CODE_LIMIT              128

// packet TTL for IGMP. 
// RFC requires it to be 1
#define TCPIP_IGMP_PACKET_TTL                       1

// number of query report transmissions
#define TCPIP_IGMP_QUERY_TRANSMITS                  1


// pool of General Query messages
#define TCPIP_IGMP_GEN_QUERY_POOL                   2

// pool of Group Query messages
// at most one per each group
#define TCPIP_IGMP_GROUP_QUERY_POOL         TCPIP_IGMP_MCAST_GROUPS
// implementation
//

// default testing value
#define TCPIP_IGMP_QQIC_INTERVAL_CODE           0x7f


// lower limit of the robustness variable, as imposed by standard
#define     TCPIP_IGMP_ROBUSTNESS_LOW_LIMIT     2


#if (TCPIP_IGMP_ROBUSTNESS_VARIABLE < TCPIP_IGMP_ROBUSTNESS_LOW_LIMIT)
#warn "Robustness variable should be greater than 2"
#endif


// IGMP socket record entry
// Describes the socket filtering mode and options
typedef struct
{
    uint8_t     filter;     // 0 means entry not busy; else include, exclude
    uint8_t     ifIndex;    // interface index to which this record refers to
    uint16_t    sktNo;      // socket assigned to this record
}TCPIP_IGMP_SKT_RECORD;


// IGMP source address cache entry
typedef struct 
{
    OA_HASH_ENTRY               hEntry;         // hash header;
    IPV4_ADDR                   srcAddress;     // the hash key: the source IP address
    int                         nRecords;       // active socket records
    TCPIP_IGMP_SKT_RECORD       sktRec[TCPIP_IGMP_SOCKET_RECORDS_PER_SOURCE];     // socket listeners on this source address
                                // Note: same socket can take multiple records for the same source,
                                // each for different interface!
}TCPIP_IGMP_SOURCE_ENTRY;


// IGMP G multicast group entry
typedef struct 
{
    OA_HASH_ENTRY           hEntry;         // hash header;
    IPV4_ADDR               gAddress;       // the multicast destination address; the hash key
    uint32_t                groupTimer[TCPIP_IGMP_INTERFACES]; // group timer per interface; 0 if not active/scheduled
    TCPIP_IGMP_SOURCE_ENTRY srcEntryTbl[_TCPIP_IGMP_SOURCES_PER_GROUP];  // array of source addresses entries for this group
                                                                        // TCPIP_IGMP_ASM_ALL_SOURCES may take a slot!
}TCPIP_IGMP_GROUP_ENTRY;


// descriptor for all G IGMP sources
//
typedef struct
{
    OA_HASH_DCPT            gHashDcpt;       // hash descriptor for Group entries (TCPIP_IGMP_GROUP_ENTRY)
    OA_HASH_DCPT            srcHashDcpt;     // hash descriptor for Source entries (TCPIP_IGMP_SOURCE_ENTRY)
    uint32_t                ifMask;         // in use interfaces mask; 
                                            // Note: slots are not released once taken!
    TCPIP_IGMP_GROUP_ENTRY  gEntryTbl[TCPIP_IGMP_MCAST_GROUPS];
}TCPIP_IGMP_GROUPS_DCPT;

// structure to gather group sources addresses
typedef struct
{
    int         nSources;                                       // how many sources populated
    uint32_t    sourceAddresses[_TCPIP_IGMP_SOURCE_ADDRESSES_PER_GROUP];  // include/exclude sources
}TCPIP_IGMP_GROUP_SOURCE_ADDRESSES;

// action to be taken when calculating a (S, if) filter
typedef enum
{
    TCPIP_IGMP_SIF_ACTION_NONE   = 0,        // do nothing, i.e. do not add
    TCPIP_IGMP_SIF_ACTION_ADD,               // source needs to be added
    TCPIP_IGMP_SIF_ACTION_EXCLUDE_NONE,      // no source is excluded; discards all other exclude sources
}TCPIP_IGMP_SIF_FILTER_ACTION;

// structure describing the (G, if) pair state: filter + source list
typedef struct
{
    TCPIP_IGMP_FILTER_TYPE          ifFilter;       // interface filter
    TCPIP_IGMP_GROUP_SOURCE_ADDRESSES groupSources;   // include/exclude sources corresponding to the filter mode
}TCPIP_IGMP_GIF_STATE_DCPT;


// Group Record types
typedef enum
{
    TCPIP_IGMP_RECORD_NONE      = 0,    // invalid/none

    // Current State
    TCPIP_IGMP_MODE_IS_INCLUDE,         // mode is now include
    TCPIP_IGMP_MODE_IS_EXCLUDE,         // mode is now exclude

    // Filter mode change
    TCPIP_IGMP_FILTER_TO_INCLUDE,       // filter change to include
    TCPIP_IGMP_FILTER_TO_EXCLUDE,       // filter change to exclude

    // Source list change
    TCPIP_IGMP_SOURCE_ALLOW_NEW,        // new sources allowed
    TCPIP_IGMP_SOURCE_BLOCK_OLD,        // old sources blocked

}TCPIP_IGMPv3_RECORD_TYPE;

// data structure to queue a IGMP SC report
typedef struct _tag_IGMP_SC_REPORT_NODE
{
    struct _tag_IGMP_SC_REPORT_NODE*    next;          // safe cast to single list
    uint8_t                             nTransmit;     // transmission counter
    uint8_t                             repType;       // type of the report: TCPIP_IGMPv3_RECORD_TYPE
    uint8_t                             ifIx;          // interface on which to transmit the report
    uint8_t                             active;        // needs to be transmitted;
    uint32_t                            tTransmit;     // time when the next transmission occurs
    uint32_t                            repGroup;      // mcast address
    TCPIP_IGMP_GROUP_SOURCE_ADDRESSES   repSources;    // sources to report
}TCPIP_IGMP_SC_REPORT_NODE;


// sources for a query report
typedef struct
{
    uint32_t                             repGroup;      // query mcast group address
    TCPIP_IGMPv3_RECORD_TYPE             repType;       // either TCPIP_IGMP_MODE_IS_INCLUDE/TCPIP_IGMP_MODE_IS_EXCLUDE
                                                        // or TCPIP_IGMP_RECORD_NONE if invalid!
    TCPIP_IGMP_GROUP_SOURCE_ADDRESSES    repSources;    // sources to report for this group    
}TCPIP_IGMP_QUERY_SOURCES;


// type of query
typedef enum __attribute__((packed))
{
    TCPIP_IGMP_QUERY_INVALID    = 0,        // invalid query
    TCPIP_IGMP_QUERY_GENERAL,               // general query: group address == 0, nSources == 0
    TCPIP_IGMP_QUERY_GROUP_SPEC,            // group specific query: group address != 0, nSources == 0
    TCPIP_IGMP_QUERY_GROUP_SOURCE_SPEC,     // group and source specific query: group address != 0, nSources != 0
}TCPIP_IGMP_QUERY_TYPE;

// generic data structure for a IGMP query report
typedef struct _tag_IGMP_QUERY_REPORT_NODE
{
    struct _tag_IGMP_QUERY_REPORT_NODE*  next;          // safe cast to single list
    uint8_t                              ifIx;          // interface on which to transmit the report
    uint8_t                              queryType;     // TCPIP_IGMP_QUERY_TYPE value
    uint16_t                             nQSources;     // number of qSources in report node!; i.e. 1 for a GSQ, GSSQ; 
    uint32_t                             tTransmit;     // time when the next transmission occurs
    TCPIP_IGMP_QUERY_SOURCES             qSources[];    // if GSQ or GSSQ, there 's only one set of sources (for that group address)
                                                        // for GQ though, it can be up to TCPIP_IGMP_MCAST_GROUPS 
}TCPIP_IGMP_QUERY_REPORT_NODE;



// data structure to queue a IGMP Group query report
// NOTE: identical to TCPIP_IGMP_QUERY_REPORT_NODE. Keep it in sync!
typedef struct _tag_IGMP_GROUP_QUERY_REPORT_NODE
{
    struct _tag_IGMP_GROUP_QUERY_REPORT_NODE*  next;    // safe cast to single list
    uint8_t                              ifIx;          // interface on which to transmit the report
    uint8_t                              queryType;     // TCPIP_IGMP_QUERY_TYPE value
    uint16_t                             nQSources;     // number of qSources in report node!; i.e. 1 for a GSQ, GSSQ; 
    uint32_t                             tTransmit;     // time when the next transmission occurs
    TCPIP_IGMP_QUERY_SOURCES             qSources[1];   // if GSQ or GSSQ, there 's only one set of sources (for that group address)
                                                        // for GQ though, it can be up to TCPIP_IGMP_MCAST_GROUPS 
}TCPIP_IGMP_GROUP_QUERY_REPORT_NODE;

// data structure to queue a IGMP General query report
// NOTE: identical to TCPIP_IGMP_QUERY_REPORT_NODE. Keep it in sync!
typedef struct _tag_IGMP_GEN_QUERY_REPORT_NODE
{
    struct _tag_IGMP_GEN_QUERY_REPORT_NODE* next;       // safe cast to single list
    uint8_t                              ifIx;          // interface on which to transmit the report
    uint8_t                              queryType;     // TCPIP_IGMP_QUERY_TYPE value;
    uint16_t                             nQSources;     // number of qSources in report node!; i.e. TCPIP_IGMP_MCAST_GROUPS for a GQ; 
    uint32_t                             tTransmit;     // ifTimer; time when the next transmission occurs
    TCPIP_IGMP_QUERY_SOURCES             qSources[TCPIP_IGMP_MCAST_GROUPS];   // if GSQ or GSSQ, there 's only one set of sources (for that group address)
                                                        // for GQ though, it can be up to TCPIP_IGMP_MCAST_GROUPS 
}TCPIP_IGMP_GEN_QUERY_REPORT_NODE;




// IGMP packets
//

// type of IGMP messages
typedef enum
{
    TCPIP_IGMP_MESSAGE_QUERY            = 0x11,     // Membership Query
    TCPIP_IGMP_MESSAGE_V3_MEMBERSHIP    = 0x22,     // v3 Membership Report
    TCPIP_IGMP_MESSAGE_V1_MEMBERSHIP    = 0x12,     // v1 Membership Report
    TCPIP_IGMP_MESSAGE_V2_MEMBERSHIP    = 0x16,     // v2 Membership Report
    TCPIP_IGMP_MESSAGE_V2_LEAVE         = 0x17,     // v2 Leave Group
}TCPIP_IGMP_MESSAGE_TYPE;

// generic IGMP header
typedef struct __attribute__((aligned(2), packed))
{
    uint8_t     type;   // message type: TCPIP_IGMP_MESSAGE_TYPE
    uint8_t     maxRespCode;    // max time for sending a report
    uint16_t    checksum;
    uint32_t    igmpData;       // IGMP version specific data
}TCPIP_IGMP_HEADER;

// IGMP query message
typedef struct __attribute__((aligned(2), packed))
{
    uint8_t     type;           // message type: TCPIP_IGMP_MESSAGE_TYPE
    uint8_t     maxRespCode;    // max time for sending a report
    uint16_t    checksum;
    uint32_t    groupAddress;   // multicast address it refers to
}TCPIP_IGMP_QUERY_MESSAGE;

// IGMPv3 query message
typedef struct __attribute__((aligned(2), packed))
{
    uint8_t     type;           // message type: TCPIP_IGMP_MESSAGE_TYPE
    uint8_t     maxRespCode;    // max time for sending a report
    uint16_t    checksum;
    uint32_t    groupAddress;   // multicast address it refers to
    struct
    {
        uint8_t    qrv:    3;      // Querier's Robustness Variable
        uint8_t    srp:    1;      // Supress Router side processing
        uint8_t    resv:   4;      // reserved
    };
    uint8_t     qqic;               // Querier's Query Interval Code
    uint16_t    nSources;           // number of sources
    uint32_t    sources[];          // sources for this query
}TCPIP_IGMPv3_QUERY_MESSAGE;

typedef union
{
    uint8_t val;
    struct
    {
        uint8_t mant:   4;      // mantissa
        uint8_t exp:    3;      // exponent
        uint8_t unit:   1;      // set to 1
    };
}TCPIP_IGMP_RESP_CODE_FLOAT;


// Group Record structure
typedef struct __attribute__((aligned(2), packed))
{
    uint8_t     recordType;         // a TCPIP_IGMPv3_RECORD_TYPE type
    uint8_t     auxLen;
    uint16_t    nSources;           // number of sources in this record
    uint32_t    groupAddress;       // multicast address it refers to
    uint32_t    sourceAddress[];    // source addresses part of the group
}TCPIP_IGMPv3_GROUP_RECORD;

// membership IGMPv3 packet
typedef struct __attribute__((aligned(2), packed))
{
    uint8_t     type;   // TCPIP_IGMP_MESSAGE_V3_MEMBERSHIP
    uint8_t     reserved1;
    uint16_t    checksum;
    uint16_t    reserved2;
    uint16_t    nGroupRecords;   // number of group records
    //
    TCPIP_IGMPv3_GROUP_RECORD   groupRecords[];     // group records part of this report
}TCPIP_IGMPv3_REPORT_MESSAGE;

// IGMP event registration

typedef struct  _TAG_TCPIP_IGMP_LIST_NODE
{
    struct _TAG_TCPIP_IGMP_LIST_NODE*    next;      // next node in list
                                                    // makes it valid SGL_LIST_NODE node
    TCPIP_IGMP_EVENT_HANDLER            handler;    // handler to be called for event
    const void*                         hParam;     // handler parameter
    IPV4_ADDR                           mcastAddress;// IPV4 address that's registered for
                                                    // 0 if all    
}TCPIP_IGMP_LIST_NODE;


#endif // _IGMP_PRIVATE_H_



