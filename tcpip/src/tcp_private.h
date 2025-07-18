/*******************************************************************************
  TCP Module private definitions for the Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcp_private.h

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

#ifndef H_TCP_PRIVATE_H_
#define H_TCP_PRIVATE_H_

/****************************************************************************
  Section:
    Debug Definitions
  ***************************************************************************/
#define TCPIP_TCP_DEBUG_MASK_BASIC          (0x0001)
#define TCPIP_TCP_DEBUG_MASK_TRACE_STATE    (0x0002)
#define TCPIP_TCP_DEBUG_MASK_SEQ            (0x0004)
#define TCPIP_TCP_DEBUG_MASK_HANDLE_SEG     (0x0008)

#define TCPIP_TCP_DEBUG_MASK_RX_CHECK       (0x0100)

// enable TCP debugging levels
#define TCPIP_TCP_DEBUG_LEVEL               (0x01)



/****************************************************************************
  Section:
    Type Definitions
  ***************************************************************************/


#define TCP_CHECKSUM_OFFSET (16u)


// The minimum/maximum value a RX or TX TCP buffer can have
// (w/o window scaling)
#define TCP_MIN_RX_BUFF_SIZE    (256U)
#define TCP_MAX_RX_BUFF_SIZE    (65535U)

#define TCP_MIN_TX_BUFF_SIZE    (256U)
#define TCP_MAX_TX_BUFF_SIZE    (65535U)

// for efficiency reasons, 
// any request to change a TX/RX buffer size
// that results in a difference less than this limit
// will be ignored
#define TCP_MIN_BUFF_CHANGE     (32U)

// the minimum MTU value that needs to be supported
#define TCP_MIN_DEFAULT_MTU     (536U)

// the min value of the data offset field, in 32 bit words
#define TCP_DATA_OFFSET_VAL_MIN    5U      // 20 bytes


// maximum retransmission time for exp backoff - 64 seconds
#define TCP_SOCKET_MAX_RETX_TIME       64000U

#if defined(TCP_RETRANSMISSION_TMO) && (TCP_RETRANSMISSION_TMO != 0)
#define TCP_SOCKET_RETX_TMO    TCP_RETRANSMISSION_TMO
#else
#define TCP_SOCKET_RETX_TMO    1500U       // default value, 1.5 sec
#endif


/****************************************************************************
  Section:
    State Machine Variables
  ***************************************************************************/


typedef struct
{
    IPV4_PACKET             v4Pkt;      // safe cast to IPV4_PACKET
    TCPIP_MAC_DATA_SEGMENT  tcpSeg[2];  // always zero copy data for TCP state machine
}TCP_V4_PACKET;

/****************************************************************************
  Section:
    TCB Definitions
  ***************************************************************************/

// TCP Control Block (TCB) stub data storage. 
typedef struct
{
    uint8_t*            txStart;                    // First byte of skt TX buffer
    uint8_t*            txEnd;                      // Last byte of skt TX buffer
    uint8_t*            txHead;                     // Head pointer for TX - user write pointer
    uint8_t*            txTail;                     // Tail pointer for TX - socket read pointer
    uint8_t*            txUnackedTail;              // TX tail pointer for data that is not yet acked
                                                    // Note: This TX buffer is for the user/app to write data, and the skt to read and transmit it
                                                    // So:
                                                    //      - tx total size: txEnd - txStart
                                                    //      - txBuffSize = txEnd - txStart - 1;     usable size  
                                                    //      - put space = txTail - txHead - 1 (+ txEnd - txStart; if txHead is behind txTail)
                                                    //      - unack data = txUnackedTail - txTail
                                                    //      - can send data = txHead - txUnackedTail
                                                    //      - init: txBuff = alloc txBuffSize + 1)
                                                    //              txStart = txBuff; txEnd = txBuff + txBuffSize + 1;
                                                    //
    uint8_t*            rxStart;                    // First byte of the socket RX buffer.
    uint8_t*            rxEnd;                      // Last byte of the socket RX buffer
    uint8_t*            rxHead;                     // Head pointer for RX - socket write pointer
    uint8_t*            rxTail;                     // Tail pointer for RX - user read pointer
                                                    // Note: This RX buffer is for the skt to write data (as it receives), and the user/app to read it
                                                    // So:
                                                    //      - rx total size: rxEnd - rxStart + 1 (created with 1 extra byte) 
                                                    //      - rxBuffSize = avlbl slots =  rxEnd - rxStart;     usable size  
                                                    //      - avlbl read bytes == rxHead - rxTail (+ rxEnd - rxStart + 1; if rxHead is behind rxTail)
                                                    //      - init: rxBuff = alloc(rxBuffSize + 1);
                                                    //              rxStart = rxBuff; rxEnd = rxBuff + rxBuffSize; 
                                                    //
    uint32_t            eventTime;                  // Packet retransmissions, state changes
    uint32_t            eventTime2;                 // Window updates, automatic transmission
    uint32_t            delayedACKTime;             // Delayed Acknowledgment timer
    uint32_t            closeWaitTime;              // TCP_CLOSE_WAIT, TCP_FIN_WAIT_2, TCP_TIME_WAIT timeout
    uint32_t            retxTmo;                    // current retransmission timeout, ms
    uint32_t            retxTime;                   // current retransmission time, ticks

    IPV4_ADDR           destAddress;                // socket destination address
    IPV4_ADDR           srcAddress;                 // socket source address
    const TCPIP_NET_IF* pSktNet;                    // which interface this socket is bound to
    union
    {
        IPV4_PACKET*  pV4Pkt;                       // IPv4 use; TCP_V4_PACKET type
        TCP_V4_PACKET*  pTcpV4Pkt;                  // IPv4 use; TCP_V4_PACKET type
        IPV6_PACKET*  pV6Pkt;                       // IPv6 use;
        void*         pTxPkt;                       // generic
    };
    // 
    uint32_t            retryInterval;              // How long to wait before retrying transmission
    uint32_t            MySEQ;                      // Local sequence number
    uint32_t            RemoteSEQ;                  // Remote sequence number
    int32_t             sHoleSize;                  // Size of the hole, or -1 for none exists.  (0 indicates hole has just been filled)
    TCP_PORT            remotePort;                 // Remote port number
    TCP_PORT            localPort;                  // Local port number
    uint16_t            remoteWindow;               // Remote window size
    uint16_t            localWindow;                // last advertised window size
    uint16_t            wFutureDataSize;            // How much out-of-order data has been received
    uint16_t            wRemoteMSS;                 // Maximum Segment Size option advertised by the remote node during initial handshaking
    uint16_t            localMSS;                   // our advertised MSS
    uint16_t            maxRemoteWindow;            // max advertised remote window size
    uint16_t            keepAliveTmo;               // timeout, ms
    uint16_t            remoteHash;                 // Consists of remoteIP, remotePort, localPort for connected sockets.
    uint8_t             smState;                    // TCPIP_TCP_STATE: State of this socket
    uint8_t             addType;                    // IPV4/6 socket type; IP_ADDRESS_TYPE enum type
    uint8_t             retryCount;                 // Counter for transmission retries
    uint8_t             keepAliveCount;             // current counter
    TCP_SOCKET          sktIx;                      // socket number
    uint16_t            sigMask;                    // TCPIP_TCP_SIGNAL_TYPE: mask of active events
    TCPIP_TCP_SIGNAL_FUNCTION sigHandler;           // socket signal handler
    const void*         sigParam;                   // socket signal parameter
    uint8_t             keepAliveLim;               // current limit
    uint8_t             ttl;                        // socket TTL value
    uint8_t             tos;                        // socket TOS value
    uint8_t             dupAckCnt;                  // duplicate ack count for fast retransmission
    union
    {
        uint32_t    val;
        struct
        {
            unsigned int bServer : 1;                       // Socket should return to listening state when closed
            unsigned int bTimerEnabled  : 1;                // Timer is enabled
            unsigned int bTimer2Enabled : 1;                // Second timer is enabled
            unsigned int bDelayedACKTimerEnabled : 1;       // DelayedACK timer is enabled
            unsigned int bOneSegmentReceived : 1;           // A segment has been received
            unsigned int bHalfFullFlush : 1;                // Flush is for being half full
            unsigned int bTXASAP : 1;                       // Transmit as soon as possible (for Flush)
            unsigned int bTXASAPWithoutTimerReset : 1;      // Transmit as soon as possible (for Flush), but do not reset retransmission timers
            unsigned int bTXFIN : 1;                        // FIN needs to be transmitted
            unsigned int bSocketReset : 1;                  // Socket has been reset (self-clearing semaphore)
            unsigned int bRxFin  : 1;                       // FIN received
            unsigned int failedDisconnect : 1;              // Failed to send a FIN
            unsigned int halfThresType : 2;                 // a TCP_OPTION_THRES_FLUSH_TYPE value
            unsigned int keepAlive : 1;                     // keep alive option enabled
            unsigned int delayAckSend : 1;                  // delay sending all acknowledged data
            unsigned int openAddType    : 2;                // the address type used at open
            unsigned int bFINSent       : 1;                // A FIN has been sent
            unsigned int bSYNSent       : 1;                // A SYN has been sent
            unsigned int nonLinger      : 1;                // linger option
            unsigned int nonGraceful    : 1;                // graceful close
            unsigned int ackSent        : 1;                // acknowledge sent in this pass
            unsigned int seqInc         : 1;                // sequence number incremented after FIN ack 
            unsigned int forceKill      : 1;                // socket should be killed, even if it's server socket 
            unsigned int forceFlush     : 1;                // flush data any time caller writes to the socket
                                                            // i.e., disable Nagle
            unsigned int srcSet         : 1;                // source address is set 
            unsigned int openBindIf     : 1;                // socket is bound to interface when opened 
            unsigned int openBindAdd    : 1;                // socket is bound to address when opened 
            unsigned int halfThresFlush : 1;                // when set, socket will flush at half TX buffer threshold
            unsigned int res1           : 1;                // not used
            unsigned int res2           : 1;                // not used
        } flags;
    };
    uint8_t             dscp;                        // socket DSCP value

#if ((TCPIP_TCP_DEBUG_LEVEL & TCPIP_TCP_DEBUG_MASK_TRACE_STATE) != 0)
    union
    {
        uint8_t         val;
        struct
        {
            unsigned int tracePrevState  : 4;            // socket previous traced state
            unsigned int traceStateFlag  : 1;            // socket state is traced
            unsigned int reserved        : 3;            // padding; not used
        };
    }dbgFlags;

#endif  // ((TCPIP_TCP_DEBUG_LEVEL & TCPIP_TCP_DEBUG_MASK_TRACE_STATE) != 0)
} TCB_STUB;

#endif  // H_TCP_PRIVATE_H_
