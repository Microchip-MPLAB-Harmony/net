/*******************************************************************************
  UDP Module private definitions

  Company:
    Microchip Technology Inc.
    
  File Name:
    udp_private.h

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

#ifndef H__UDP_PRIVATE_H_
#define H__UDP_PRIVATE_H_

/****************************************************************************
  Section:
    Debug Definitions
  ***************************************************************************/

#define TCPIP_UDP_DEBUG_MASK_RX_CHECK       (0x0100)

// enable UDP debugging levels
#define TCPIP_UDP_DEBUG_LEVEL               (0)



/****************************************************************************
  Section:
    Type Definitions
  ***************************************************************************/

#define UDP_CHECKSUM_OFFSET     6u

#if defined(TCPIP_IPV4_FRAGMENTATION) && (TCPIP_IPV4_FRAGMENTATION != 0)
#define TCPIP_IPV4_UDP_FRAGMENTATION    1
#else
#define TCPIP_IPV4_UDP_FRAGMENTATION    0
#endif

typedef struct
{
    IPV4_PACKET             v4Pkt;     // safe cast to IPV4_PACKET
}UDP_V4_PACKET;

typedef struct
{
    IPV4_PACKET             v4Pkt;      // safe cast to IPV4_PACKET
    TCPIP_MAC_DATA_SEGMENT  zcSeg[1];   // if BSD used, zero copy data segment
}UDP_V4_ZC_PACKET;


// flags the packet as belonging to the UDP pool
#define UDP_SOCKET_POOL_BUFFER_FLAG     (TCPIP_MAC_PKT_FLAG_USER)

// minimum size of buffer in the pool
#define UDP_SOCKET_POOL_BUFFER_MIN_SIZE 256U

// default TTL for multicast traffic
#define UDP_MULTICAST_DEFAULT_TTL       1U

// incoming packet match flags
// 8 bits
typedef enum
{
    TCPIP_UDP_PKT_MATCH_NONE        = 0x00,     // no match
    TCPIP_UDP_PKT_MATCH_IP_TYPE     = 0x01,     // match occurred on the packet type Ipv4/IPv6
    TCPIP_UDP_PKT_MATCH_SRC_PORT    = 0x02,     // match occurred on the packet source port
    TCPIP_UDP_PKT_MATCH_NET         = 0x04,     // match occurred on the packet incoming interface
    TCPIP_UDP_PKT_MATCH_SRC_ADD     = 0x08,     // match occurred on the packet source address

    //
    // good match mask
    TCPIP_UDP_PKT_MATCH_MASK        = (TCPIP_UDP_PKT_MATCH_IP_TYPE | TCPIP_UDP_PKT_MATCH_SRC_PORT | TCPIP_UDP_PKT_MATCH_NET | TCPIP_UDP_PKT_MATCH_SRC_ADD)

}TCPIP_UDP_PKT_MATCH;

// socket flags 
typedef union
{
    uint32_t     Val;
    struct __attribute__((packed))
    {
        unsigned int    bcastForceType   : 2;   // destination forced to broadcast, if any
        unsigned int    looseRemPort     : 1;   // allows receiving data from any socket having the same destination Port
                                                // but irrespective its local port
        unsigned int    looseNetIf       : 1;   // allows receiving data on any interface        
        unsigned int    looseRemAddress  : 1;   // allows receiving data from any host address        
        unsigned int    srcSet           : 1;   // source address is set/forced
        unsigned int    srcValid         : 1;   // source address is valid (could be even 0) 
        unsigned int    srcSolved        : 1;   // source address map to network interface is solved 

        unsigned int    destSet          : 1;   // destination address is set/forced
        unsigned int    txSplitAlloc     : 1;   // if set, will perform a split (ZC) TX allocation
        unsigned int    usePool          : 1;   // allows allocating from the private pool        
        unsigned int    stackConfig      : 1;   // socket allocated by some stack configuration module
        unsigned int    openAddType      : 2;   // the address type used at open
        unsigned int    openBindIf       : 1;   // socket is bound to interface when opened 
        unsigned int    openBindAdd      : 1;   // socket is bound to address when opened 
        
        unsigned int    rxAutoAdvance    : 1;   // automatic RX socket advance
        unsigned int    rxEnable         : 1;   // enable socket receive
        unsigned int    mcastLoop        : 1;   // loop internally the multicast traffic
        unsigned int    mcastSkipCheck   : 1;   // skip the checking of the incoming multicast traffic
        unsigned int    fixedDestAddress : 1;       // ignore packet source address
        unsigned int    fixedDestPort    : 1;   // ignore packet source port
        unsigned int    mcastOnly        : 1;   // ignore non multicast packets
        unsigned int    serverSkt        : 1;   // socket opened as server

        unsigned int    tos              : 6;   // Type of Service: 6 bits!
        unsigned int    df               : 1;   // Don't Fragment bit
        unsigned int    noNetStrict      : 1;   // do not enforce net strictness
    };
}TCPIP_UDP_SKT_FLAGS;

// additional socket flags
// used as discrete masks rather than bit fields because a bit field will ocupy 32 bits
// 8 bits
typedef enum
{
    UDP_EFLAG_NONE                      = 0x00,     // no flag set
    UDP_EFLAG_STICKY_LOOSE_REM_PORT     = 0x01,     // the looseRemPort is sticky, not changed by a remote bind operation
    UDP_EFLAG_STICKY_LOOSE_NET_IF       = 0x02,     // the looseNetIf is sticky, not changed by a bind or TCPIP_UDP_SocketNetSet operation 
    UDP_EFLAG_STICKY_LOOSE_REM_ADDRESS  = 0x04,     // the looseRemAddress is sticky, not changed by a remote bind operation
}UDP_EXT_FLAGS;

// Stores information about a current UDP socket
typedef struct
{
    // TX side
    uint8_t*        txStart;        // internal TX Buffer; both IPv4 and IPv6
    uint8_t*        txEnd;          // end of TX Buffer
    uint8_t*        txWrite;        // current write pointer into the TX Buffer
    union
    {
        IPV4_PACKET*  pV4Pkt;        // IPv4 use; UDP_V4_PACKET type
        IPV6_PACKET*  pV6Pkt;        // IPv6 use;
        void*         pPkt;          // generic
        // if txSplitAlloc == 0 a socket always has both txStart and pV4Pkt/pV6Pkt available
                // for IPv4 once we have pV4Pkt, txStart should be one of its segments!!!
            // or neither (ADDR_ANY, IPv4/IPv6 not decided yet!)
    };
    uint16_t        txSize;         // size of the txBuffer
    // socket info
    UDP_SOCKET      sktIx;
    IPV4_ADDR       destAddress;    // requested destination address
                                    // packet destination address. Set by:
                                    //  - packet rx _UDPsetPacketInfo(), if destSet == 0; copied from packet source address
                                    //      destSet left to 0!
                                    //  - client open with a host + destSet = 1
                                    //  - TCPIP_UDP_BcastIPV4AddressSet() + destSet = 1;
                                    //  - UDP_OPTION_BROADCAST + destSet = 1/0 if broadcast is set or cleared  
                                    //  - TCPIP_UDP_DestinationIPAddressSet or TCPIP_UDP_RemoteBind + destSet = 1
    IPV4_ADDR       srcAddress;     // only if bound, else 0
                                    // tx packet source address;
                                    // Set by TCPIP_IPV4_SelectSourceInterface when srcSolved == 0 || pSktNet == 0
                                    //      srcSolved = srcValid = 1;
                                    //      An interface address is used
                                    // Set by:
                                    //  - packet rx _UDPsetPacketInfo(), if srcSet == 0 && !bcast address; copied from packet destination address
                                    //          srcSolved = 0; srcValid = 1;
                                    //  TCPIP_UDP_SourceIPAddressSet() + TCPIP_UDP_Bind() + _UDPSetSourceAddress() + _UDPSocketBind() + srcSet = srvValid = 1; srcSolved =0; 
                                    //  - cleared by TCPIP_UDP_Disconnect() (if openBindAdd == 0): srcValid = srcSolved = 0; 
    IPV4_ADDR       pktSrcAddress;  // source address of last received packet
                                    // used in match find, if not looseRemAddress
    IPV4_ADDR       pktDestAddress; // destination address of last received packet
                                    // info only
    const TCPIP_NET_IF* pSktNet;    // local interface; 
                                    // srcAddress can be outside of pSktNet to allow
                                    // sending packets with chosen IP address  
                                    // Used by:
                                    //  - socket match if == packet arriving if: pPktIf
                                    //  - source interface for a TX packet
                                    // Set by:
                                    //  - packet rx: _UDPsetPacketInfo !
                                    //  - _UDPSocketBind() + looseNetIf = 0!
                                    //  - TCPIP_IPV4_SelectSourceInterface - selects a network interface when TX packt
                                    //  - cleared by TCPIP_UDP_Disconnect()
                                    //  - TCPIP_UDP_SocketNetSet + looseNetIf = 0
                                    //
                                    //
    UDP_PORT        remotePort;     // Remote node's UDP port number
                                    // Destination port of a tx packet
                                    // Used in socket match find against imcpming packet sourcePort
                                    // Set by:
                                    //  - packet rx _UDPsetPacketInfo() !
                                    //  - _UDPOpen()
                                    //  - Cleared in TCPIP_UDP_Disconnect() for a server socket
                                    //  - TCPIP_UDP_RemoteBind() + looseRemPort = 0!!!!
                                    //  - TCPIP_UDP_DestinationPortSet()
    UDP_PORT        localPort;      // Local UDP port number, or 0 when free
                                    // Source port for the tx packet
                                    // Used for skt match:  == h->DestinationPort 
                                    // Set by:
                                    //      - _UDPOpen for server; 0/ephemeral for client
                                    //      - TCPIP_UDP_Bind()
    // rx side
    TCPIP_MAC_PACKET       *pCurrRxPkt;   // current RX packet 
    TCPIP_MAC_DATA_SEGMENT *pCurrRxSeg;   // current segment in the current packet
                                          // support for multi segment packets
#if (TCPIP_IPV4_UDP_FRAGMENTATION != 0)
    TCPIP_MAC_PACKET       *pCurrFrag;    // current RX fragment; fragmentation support 
#endif  // (TCPIP_IPV4_UDP_FRAGMENTATION != 0)
    uint16_t        rxSegLen;       // current segment len
    uint16_t        rxTotLen;       // total remaining data length, across segments 
    uint8_t*        rxCurr;         // current RX pointer 
    // socket info + flags
    TCPIP_UDP_SKT_FLAGS flags;      // current socket flags; 32 bit value
    SINGLE_LIST     rxQueue;        // queued RX packets belonging to this socket
    TCPIP_UDP_SIGNAL_FUNCTION sigHandler;  // socket event handler
    const void*     sigParam;              // event handler parameter
    uint16_t        txAllocLimit;   // max number of packets that can be queued/allocated at a certain time
    uint16_t        txAllocCnt;     // current number of packets that are allocated for TX
    uint16_t        rxQueueLimit;   // max number of RX packets that can be queued at a certain time
    uint16_t        sigMask;        // TCPIP_UDP_SIGNAL_TYPE: active events

    uint8_t         addType;        // IPV4/6 socket type, IP_ADDRESS_TYPE;
    uint8_t         extFlags;       // a UDP_EXT_FLAGS value, 8 bit
    uint8_t         ttl;            // socket TTL value 
    uint8_t         dscp;           // IPv6 DSCP value
} UDP_SOCKET_DCPT;


#endif  // H__UDP_PRIVATE_H_



