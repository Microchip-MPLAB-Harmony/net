/*******************************************************************************
  Transmission Control Protocol (TCP) Communications Layer

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    -Provides reliable, handshaked transport of application stream 
     oriented data with flow control
    -Reference: RFC 793
*******************************************************************************/

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








#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_TCP

#include "tcpip/src/tcpip_private.h"
#include "tcpip/src/tcp_private.h"

#if defined(TCPIP_STACK_USE_TCP)


/****************************************************************************
  Section:
    TCP Header Data Types
  ***************************************************************************/

#define FIN             (0x01U)      // FIN Flag as defined in RFC
#define SYN             (0x02U)      // SYN Flag as defined in RFC
#define RST             (0x04U)      // Reset Flag as defined in RFC
#define PSH             (0x08U)      // Push Flag as defined in RFC
#define ACK             (0x10U)      // Acknowledge Flag as defined in RFC
#define URG             (0x20U)      // Urgent Flag as defined in RFC



#define TCP_OPTIONS_END_OF_LIST     (0x00u)     // End of List TCP Option Flag
#define TCP_OPTIONS_NO_OP           (0x01u)     // No Op TCP Option
#define TCP_OPTIONS_MAX_SEG_SIZE    (0x02u)     // Maximum segment size TCP flag
typedef struct
{
    uint8_t        Kind;                            // Type of option
    uint8_t        Length;                          // Length
    TCPIP_UINT16_VAL    MaxSegSize;                     // Maximum segment size
} TCP_OPTIONS;                                  // TCP Options data structure                           

// Indicates if this packet is a retransmission (no reset) or a new packet (reset required)
#define SENDTCP_RESET_TIMERS    0x01U
// Instead of transmitting normal data, a garbage octet is transmitted according to RFC 1122 section 4.2.3.6
#define SENDTCP_KEEP_ALIVE      0x02U

// Internal F_TcpSend result
typedef enum
{
    // positive success codes
    TCP_SEND_NOT_CONN  = 1,    // socket is not connected, no data is actually sent 
    TCP_SEND_OK        = 0,    // success

    // negative failure codes
    TCP_SEND_IP_FAIL   = -1,   // IP layer rejected the packet
                                // most probably an ill formatted packet
    TCP_SEND_NO_PKT    = -2,   // socket has no packet, cannot send data
    TCP_SEND_NO_MEMORY = -3,   // out of memory; however the operation can be retried
    TCP_SEND_NO_IF     = -4,   // IP layer could not get an interface for this packet
    TCP_SEND_QUIET     = -5,   // TCP layer is in quiet time could not send this packet
}TCP_SEND_RES;

// abort operation flags
typedef enum
{
    TCP_ABORT_FLAG_REGULAR     = 0x00,     // regular abort
    TCP_ABORT_FLAG_FORCE_CLOSE = 0x01,     // kill the socket according to its state
    TCP_ABORT_FLAG_SHUTDOWN    = 0x02,     // shutdown: kill no matter what
}TCP_ABORT_FLAGS;
/****************************************************************************
  Section:
    TCB Definitions
  ***************************************************************************/

static TCB_STUB** TCBStubs = NULL;

static int        tcpLockCount = 0;                 // lock protection counter
static int        tcpInitCount = 0;                 // initialization counter

static const void*  tcpHeapH = NULL;                 // memory allocation handle
static unsigned int TcpSockets;                      // number of sockets in the current TCP configuration

static TCPIP_SIGNAL_HANDLE    tcpSignalHandle = NULL;

static uint16_t             tcpDefTxSize;               // default size of the TX buffer
static uint16_t             tcpDefRxSize;               // default size of the RX buffer

static OSAL_SEM_HANDLE_TYPE tcpSemaphore;

#if (TCPIP_TCP_QUIET_TIME != 0)
static uint32_t             tcpStartTime;               // time at which the TCP module starts, after the quiet time
static bool                 tcpQuietDone;               // the quiet time has elapsed
#endif  // (TCPIP_TCP_QUIET_TIME != 0)

#if (TCPIP_TCP_EXTERN_PACKET_PROCESS != 0)
static TCPIP_TCP_PACKET_HANDLER tcpPktHandler = NULL;
static const void* tcpPktHandlerParam;
#endif  // (TCPIP_TCP_EXTERN_PACKET_PROCESS != 0)

static uint32_t             sysTickFreq;            // the system tick counter frequency; frequently used 

/****************************************************************************
  Section:
    Function Prototypes
  ***************************************************************************/

static TCP_SEND_RES F_TcpSend(TCB_STUB* pSkt, uint8_t vTCPFlags, uint8_t vSendFlags);
static void F_TcpHandleSeg(TCB_STUB* pSkt, TCP_HEADER* h, uint16_t tcpLen, TCPIP_MAC_PACKET* pRxPkt, uint32_t* pSktEvent);
static TCB_STUB* F_TcpFindMatchingSocket(TCPIP_MAC_PACKET* pRxPkt, const void * remoteIP, const void * localIP, IP_ADDRESS_TYPE addressType);
static void F_TcpSwapHeader(TCP_HEADER* header);
static void F_TcpCloseSocket(TCB_STUB* pSkt, TCPIP_TCP_SIGNAL_TYPE tcpEvent);
static void F_TcpSocketInitialize(TCB_STUB* pSkt, TCP_SOCKET hTCP, uint8_t* txBuff, uint16_t txBuffSize, uint8_t* rxBuff, uint16_t rxBuffSize);
static void F_TcpSocketSetIdleState(TCB_STUB* pSkt);

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void F_TcpCleanup(void);
#else
#define F_TcpCleanup();
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

#if (TCPIP_STACK_DOWN_OPERATION != 0) || (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)
static void F_TCPAbortSockets(uint32_t netMask, TCPIP_TCP_SIGNAL_TYPE sigType); 
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0) || (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)

// function to get the socket signal in a consistent/safe way
static __inline__ uint16_t __attribute__((always_inline)) F_TcpSktGetSignalLocked(TCB_STUB* pSkt, TCPIP_TCP_SIGNAL_FUNCTION* pSigHandler, const void** pSigParam)
{
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    *pSigHandler = pSkt->sigHandler;
    *pSigParam = pSkt->sigParam;
    uint16_t sigMask = pSkt->sigMask;
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return sigMask;
}

// conversion helper functions
static __inline__ TCPIP_TCP_SIGNAL_HANDLE __attribute__((always_inline)) FC_SigFunc2SigHndl(TCPIP_TCP_SIGNAL_FUNCTION sFunc)
{
    union
    {
        TCPIP_TCP_SIGNAL_FUNCTION   sFunc;
        TCPIP_TCP_SIGNAL_HANDLE     sHandle;
    }U_SIG_FNC_HANDLE;

    U_SIG_FNC_HANDLE.sFunc = sFunc;
    return U_SIG_FNC_HANDLE.sHandle;
}

static __inline__ TCPIP_TCP_SIGNAL_FUNCTION __attribute__((always_inline)) FC_SigHndl2SigFunc(TCPIP_TCP_SIGNAL_HANDLE sHandle)
{
    union
    {
        TCPIP_TCP_SIGNAL_HANDLE     sHandle;
        TCPIP_TCP_SIGNAL_FUNCTION   sFunc;
    }U_SIG_HNDL_FNC;

    U_SIG_HNDL_FNC.sHandle = sHandle;
    return U_SIG_HNDL_FNC.sFunc;
}

static __inline__ TCPIP_TCP_PACKET_HANDLER __attribute__((always_inline)) FC_ProcHndl2PktHndl(TCPIP_TCP_PROCESS_HANDLE procHandle)
{
    union
    {
        TCPIP_TCP_PROCESS_HANDLE procHandle;
        TCPIP_TCP_PACKET_HANDLER pktHandler;
    }U_PROC_PKT_HANDLE;

    U_PROC_PKT_HANDLE.procHandle = procHandle;
    return U_PROC_PKT_HANDLE.pktHandler;
}

static __inline__ TCPIP_TCP_PROCESS_HANDLE __attribute__((always_inline)) FC_PktHndl2ProcHndl(TCPIP_TCP_PACKET_HANDLER pktHandler)
{
    union
    {
        TCPIP_TCP_PACKET_HANDLER pktHandler;
        TCPIP_TCP_PROCESS_HANDLE procHandle;
    }U_PKT_PROC_HANDLE;

    U_PKT_PROC_HANDLE.pktHandler = pktHandler;
    return U_PKT_PROC_HANDLE.procHandle;
}

static void F_TcpAbort(TCB_STUB* pSkt, TCP_ABORT_FLAGS abFlags, TCPIP_TCP_SIGNAL_TYPE tcpEvent);
static TCP_SEND_RES F_TcpDisconnect(TCB_STUB* pSkt, bool signalFIN);


static void         TCPIP_TCP_Tick(void);

static void         TCPIP_TCP_Process(void);

static TCP_PORT     F_TCP_EphemeralPortAllocate(void);
static bool         F_TCP_PortIsAvailable(TCP_PORT port);

static uint16_t     F_TCP_ClientIPV4RemoteHash(const IPV4_ADDR* pAdd, TCB_STUB* pSkt);

typedef enum
{
    TCP_OPEN_SERVER,    // create a server socket
    TCP_OPEN_CLIENT,    // create a client socket

}TCP_OPEN_TYPE;


static TCP_SOCKET   F_TCP_Open(IP_ADDRESS_TYPE addType, TCP_OPEN_TYPE opType, TCP_PORT port, IP_MULTI_ADDRESS* hostAddress);

static TCP_SOCKET_FLAGS F_TCP_SktFlagsGet(TCB_STUB* pSkt);

static uint32_t         F_TCP_SktSetSequenceNo(const TCB_STUB* pSkt);

#if defined (TCPIP_STACK_USE_IPV4)
static TCP_V4_PACKET* F_TcpAllocateTxPacket(TCB_STUB* pSkt, IP_ADDRESS_TYPE addType);
static TCP_V4_PACKET*   F_Tcpv4AllocateTxPacketIfQueued(TCB_STUB * pSkt, bool resetOldPkt);
static void             F_Tcpv4TxAckFnc (TCPIP_MAC_PACKET * pPkt, const void * param);
static void             F_Tcpv4UnlinkDataSeg(TCP_V4_PACKET* pTcpPkt);
static void             F_Tcpv4LinkDataSeg(TCP_V4_PACKET* pTcpPkt, uint8_t* pBuff1, uint16_t bSize1, uint8_t* pBuff2, uint16_t bSize2);
static bool             F_TCPv4Flush(TCB_STUB * pSkt, IPV4_PACKET* pv4Pkt, uint16_t hdrLen, uint16_t loadLen);
static TCP_V4_PACKET*   F_TxSktGetLockedV4Pkt(TCB_STUB* pSkt);
static TCPIP_MAC_PACKET *F_TxSktFreeLockedV4Pkt(TCB_STUB* pSkt);
static TCPIP_MAC_PKT_ACK_RES TCPIP_TCP_ProcessIPv4(TCPIP_MAC_PACKET* pRxPkt);


#endif  // defined (TCPIP_STACK_USE_IPV4)


#if defined (TCPIP_STACK_USE_IPV6)


static IPV6_PACKET*     F_TCPv6AllocateTxPacketStruct (TCB_STUB * pSkt);
static void             F_Tcpv6AckFnc (void * pkt, bool success, const void * param);
static void             F_Tcpv6MacAckFnc (TCPIP_MAC_PACKET* pPkt,  const void* param);
static IPV6_PACKET*     F_Tcpv6AllocateTxPacketIfQueued (TCB_STUB * pSkt, bool resetOldPkt);
static IPV6_PACKET*     F_TxSktGetLockedV6Pkt(TCB_STUB* pSkt, IPV6_PACKET** ppSktPkt, bool setQueued);
static IPV6_PACKET*     F_TxSktFreeLockedV6Pkt(TCB_STUB* pSkt);
static TCPIP_MAC_PKT_ACK_RES TCPIP_TCP_ProcessIPv6(TCPIP_MAC_PACKET* pRxPkt);


#endif  // defined (TCPIP_STACK_USE_IPV6)

static bool         F_TCP_RxPktValidate(TCPIP_MAC_PACKET* pRxPkt);

static bool         F_TCP_TxPktValid(TCB_STUB * pSkt);

static void         F_TCP_PayloadSet(TCB_STUB * pSkt, void* pPkt, uint8_t* payload1, uint16_t len1, uint8_t* payload2, uint16_t len2);
static bool         F_TCP_Flush(TCB_STUB * pSkt, void* pPkt, uint16_t hdrLen, uint16_t loadLen);

static bool         F_TcpFlush(TCB_STUB* pSkt);

static void         F_TcpDiscardTx(TCB_STUB* pSkt);

static uint16_t     F_TCPIsPutReady(TCB_STUB* pSkt);

static uint16_t     F_TCPIsGetReady(TCB_STUB* pSkt);

static uint16_t     F_TCPGetRxFIFOFree(TCB_STUB* pSkt);

static bool         F_TCPSendWinIncUpdate(TCB_STUB* pSkt);

static uint16_t     F_TCPSocketTxFreeSize(TCB_STUB* pSkt);

static bool         F_TCPNeedSend(TCB_STUB* pSkt);

static void         F_TCPSetHalfFlushFlag(TCB_STUB* pSkt);

static bool         F_TCPSetSourceAddress(TCB_STUB* pSkt, IP_ADDRESS_TYPE addType, const IP_MULTI_ADDRESS* localAddress)
{
    if(localAddress == NULL)
    {   // nothing to set
        return false;
    }

    while(pSkt->addType == (uint8_t)addType)
    {
#if defined (TCPIP_STACK_USE_IPV6)
        if (addType == IP_ADDRESS_TYPE_IPV6)
        {
            if(pSkt->pV6Pkt != NULL)
            {
               TCPIP_IPV6_SourceAddressSet(pSkt->pV6Pkt, &localAddress->v6Add);
               return true;
            }
            break;
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)

#if defined (TCPIP_STACK_USE_IPV4)
        if (addType == IP_ADDRESS_TYPE_IPV4)
        {
            pSkt->srcAddress.Val = localAddress->v4Add.Val;
            pSkt->flags.srcSet = 1;
            return true;
        }
#endif  // defined (TCPIP_STACK_USE_IPV4)

        break;
    }

    return false;

}

static bool F_TCP_TxPktValid(TCB_STUB * pSkt)
{

    bool res;

    switch(pSkt->addType)
    {
#if defined(TCPIP_STACK_USE_IPV6)
        case (uint8_t)IP_ADDRESS_TYPE_IPV6:
            res = pSkt->pV6Pkt != NULL;
            break;
#endif  // defined(TCPIP_STACK_USE_IPV6)

#if defined(TCPIP_STACK_USE_IPV4)
        case (uint8_t)IP_ADDRESS_TYPE_IPV4:
            res = pSkt->pV4Pkt != NULL;
            break;
            
#endif  // defined (TCPIP_STACK_USE_IPV4)

        default:
            res = false;
            break;
    }

    return res;

}


static void F_TcpSocketBind(TCB_STUB* pSkt, const TCPIP_NET_IF* pNet, const IP_MULTI_ADDRESS* srcAddress)
{
    pSkt->pSktNet = pNet;
#if defined (TCPIP_STACK_USE_IPV6)
    if(pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
    {
        if(pSkt->pV6Pkt != NULL)
        {
            pSkt->pV6Pkt->netIfH = pNet;
        }
    }
#endif  // defined (TCPIP_STACK_USE_IPV6)

   (void) F_TCPSetSourceAddress(pSkt, (IP_ADDRESS_TYPE)pSkt->addType, srcAddress);
}

/*static __inline__*/static  TCB_STUB* /*__attribute__((always_inline))*/ F_TcpSocketChk(TCP_SOCKET hTCP)
{
    if(hTCP >= 0 && (uint32_t)hTCP < TcpSockets)
    {
        return TCBStubs[hTCP];
    }

    return NULL;
}


static __inline__ bool __attribute__((always_inline)) F_TCP_IsConnected(TCB_STUB* pSkt)
{
    return (pSkt->smState == (uint8_t)TCPIP_TCP_STATE_ESTABLISHED || pSkt->smState == (uint8_t)TCPIP_TCP_STATE_FIN_WAIT_1 || pSkt->smState == (uint8_t)TCPIP_TCP_STATE_FIN_WAIT_2 || pSkt->smState == (uint8_t)TCPIP_TCP_STATE_CLOSE_WAIT);
} 

#if ((TCPIP_TCP_DEBUG_LEVEL & (TCPIP_TCP_DEBUG_MASK_TRACE_STATE | TCPIP_TCP_DEBUG_MASK_HANDLE_SEG)) != 0)
static const char* s_tcpTraceStateName[] = 
{
    "listen",       // TCPIP_TCP_STATE_LISTEN
    "syn-sent",     // TCPIP_TCP_STATE_SYN_SENT
    "syn-recv",     // TCPIP_TCP_STATE_SYN_RECEIVED
    "est",          // TCPIP_TCP_STATE_ESTABLISHED
    "fwait-1",      // TCPIP_TCP_STATE_FIN_WAIT_1
    "fwait-2",      // TCPIP_TCP_STATE_FIN_WAIT_2
    "closing",      // TCPIP_TCP_STATE_CLOSING
    "time-wait",    // TCPIP_TCP_STATE_TIME_WAIT
    "close-wait",   // TCPIP_TCP_STATE_CLOSE_WAIT
    "last-ack",     // TCPIP_TCP_STATE_LAST_ACK
    "wait-discon",  // TCPIP_TCP_STATE_CLIENT_WAIT_DISCONNECT
    "wait-conn",    // TCPIP_TCP_STATE_CLIENT_WAIT_CONNECT
    "killed",       // TCPIP_TCP_STATE_KILLED
};
#endif  // ((TCPIP_TCP_DEBUG_LEVEL & (TCPIP_TCP_DEBUG_MASK_TRACE_STATE | TCPIP_TCP_DEBUG_MASK_HANDLE_SEG)) != 0)

#if ((TCPIP_TCP_DEBUG_LEVEL & TCPIP_TCP_DEBUG_MASK_TRACE_STATE) != 0)
static void F_TcpSocketSetState(TCB_STUB* pSkt, TCPIP_TCP_STATE newState)
{
    if(pSkt->dbgFlags.traceStateFlag != 0)
    {   // socket state is traced
        if(pSkt->dbgFlags.tracePrevState != newState)
        {
            pSkt->dbgFlags.tracePrevState = newState;
            SYS_CONSOLE_PRINT("TCP socket: %d, state: %s\r\n", pSkt->sktIx, s_tcpTraceStateName[newState]);
        } 
    }
    pSkt->smState = (uint8_t)newState;
}

static uint32_t    tcpTraceMask = 0;      // currently only first 32 sockets could be traced from the creation moment
bool TCPIP_TCP_SocketTraceSet(TCP_SOCKET sktNo, bool enable)
{
    TCB_STUB* pSkt;

    if(sktNo >= 0 && sktNo < TcpSockets && (sktNo < sizeof(tcpTraceMask) * 8))
    {
        if(enable)
        {
            tcpTraceMask |= (1 << sktNo);
        }
        else
        {
            tcpTraceMask &= ~(1 << sktNo);
        }

        if((pSkt = TCBStubs[sktNo]) != 0)
        {
            pSkt->dbgFlags.traceStateFlag = enable ? 1 : 0;
        }
        return true;
    }

    return false;
}

#else
static __inline__ void __attribute__((always_inline)) F_TcpSocketSetState(TCB_STUB* pSkt, TCPIP_TCP_STATE newState)
{
    pSkt->smState = (uint8_t)newState;
}
bool TCPIP_TCP_SocketTraceSet(TCP_SOCKET sktNo, bool enable)
{
    return false;
}
#endif  // ((TCPIP_TCP_DEBUG_LEVEL & TCPIP_TCP_DEBUG_MASK_TRACE_STATE) != 0)

#if ((TCPIP_TCP_DEBUG_LEVEL & TCPIP_TCP_DEBUG_MASK_HANDLE_SEG) != 0)
static uint32_t hsegCount = 0;
static void F_TcpHandleSegEnter(TCB_STUB* pSkt)
{
    SYS_CONSOLE_PRINT("hseg enter - skt: %d, count: %d\r\n", pSkt->sktIx, hsegCount++);
}

static void F_TcpHandleSegDbg(TCB_STUB* pSkt, const char* msg, uint32_t code)
{
    SYS_CONSOLE_PRINT("hseg - skt: %d, stat: %s, msg: %s, code: %d\r\n", pSkt->sktIx, s_tcpTraceStateName[pSkt->smState], msg, code);
}
static void F_TcpHandleSegDbgData(TCB_STUB* pSkt, const char* msg, uint32_t dataSz, uint32_t code)
{
    SYS_CONSOLE_PRINT("hseg - skt: %d, stat: %s, msg: %s, data: %d, code: %d\r\n", pSkt->sktIx, s_tcpTraceStateName[pSkt->smState], msg, dataSz, code);
}
#else
#define F_TcpHandleSegEnter(pSkt)
#define F_TcpHandleSegDbg(pSkt, msg, code)
#define F_TcpHandleSegDbgData(pSkt, msg, dataSz, code)
#endif  // ((TCPIP_TCP_DEBUG_LEVEL & TCPIP_TCP_DEBUG_MASK_HANDLE_SEG) != 0)


#if ((TCPIP_TCP_DEBUG_LEVEL & TCPIP_TCP_DEBUG_MASK_RX_CHECK) != 0)
// check ports: 0 - irrelevant; otherwise it's considered in match
static uint16_t checkTcpSrcPort = 0; 
static uint16_t checkTcpDstPort = 80;

static bool checkStrict = false;    // if 0, then any match, src or dest will do
                                    // else both source and dest must match
static uint32_t checkTcpBkptCnt = 0;

static bool TCPIP_TCP_CheckRxPkt(TCP_HEADER* pHdr)
{
    TCP_PORT srcPort = pHdr->SourcePort;
    TCP_PORT destPort = pHdr->DestPort;

    bool srcMatch = (checkTcpSrcPort == 0 || srcPort == checkTcpSrcPort);
    bool destMatch = (checkTcpDstPort == 0 || destPort == checkTcpDstPort);

    bool match = 0;

    if(checkStrict)
    {
        match = srcMatch && destMatch;
    }
    else
    {
        match = srcMatch || destMatch;
    }

    if(match)
    {
        checkTcpBkptCnt++;
        return true;
    }

    return false;
}
#else
#define TCPIP_TCP_CheckRxPkt(pHdr)
#endif // ((TCPIP_TCP_DEBUG_LEVEL & TCPIP_TCP_DEBUG_MASK_RX_CHECK) != 0)


/*static __inline__*/static  void /*__attribute__((always_inline))*/ F_TcpSocketKill(TCB_STUB* pSkt)
{
    F_TcpSocketSetState(pSkt, TCPIP_TCP_STATE_KILLED);       // trace purpose only
    
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    TCBStubs[pSkt->sktIx] = NULL;
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);

    (void) TCPIP_HEAP_Free(tcpHeapH, (void*)pSkt->rxStart);
    (void) TCPIP_HEAP_Free(tcpHeapH, (void*)pSkt->txStart);
    (void) TCPIP_HEAP_Free(tcpHeapH, pSkt);
}

// returns:  0 if a SYN could be sent
//           > 0 if the operation failed but can be retried
//           < 0 if there is no valid destination or some other error
// client sockets only!
static int F_TcpClientSocketConnect(TCB_STUB* pSkt)
{
    int res = 0;
#if defined (TCPIP_STACK_USE_IPV6)
    const IPV6_ADDR* pV6Addr;
#endif // defined (TCPIP_STACK_USE_IPV6)

    switch(pSkt->addType)
    {
#if defined (TCPIP_STACK_USE_IPV4)
        case (uint8_t)IP_ADDRESS_TYPE_IPV4:
            if( pSkt->destAddress.Val == 0U)
            {
                res = -1;
            }
            else
            {   // destination known
                pSkt->remoteHash = F_TCP_ClientIPV4RemoteHash(&pSkt->destAddress, pSkt);
            }
            break;
#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV6)
        case (uint8_t)IP_ADDRESS_TYPE_IPV6:
            pV6Addr = TCPIP_IPV6_DestAddressGet(pSkt->pV6Pkt);
            if(memcmp(pV6Addr->v, IPV6_FIXED_ADDR_UNSPECIFIED.v, sizeof (IPV6_ADDR)) == 0)
            {
                res = -1;
            }
            else
            {   // destination known
                pSkt->remoteHash = TCPIP_IPV6_GetHash(pV6Addr, pSkt->remotePort, pSkt->localPort);
            }
            break;
#endif  // defined (TCPIP_STACK_USE_IPV6)

        default:
            res = -1;  // IP_ADDRESS_TYPE_ANY
            break;
    }

    if(res != 0)
    {   // failed
        return res;
    }
    // try to send SYN

    pSkt->retryCount = 0;
    pSkt->retryInterval = (sysTickFreq / 4U);
    TCP_SEND_RES sendRes = F_TcpSend(pSkt, SYN, SENDTCP_RESET_TIMERS);
    if(sendRes == TCP_SEND_OK)
    {   // success
        return 0;
    }

    return (int32_t)sendRes > 0 ? 1 : -1;
}

#if defined (TCPIP_STACK_USE_IPV4)
// following is the implementation for the TX thread lock
// This a global lock that needs to be very fast

// Disables the pre-emption during the socket changes in the TX process
// to protect between user threads and the TX thread
// It protects:
//  - the socket TX buffer resources which are updated by the user threads
//    and the TX thread via the F_Tcpv4TxAckFnc
//
// protects user vs TX threads when the user needs to allocate a new TX packet for a socket
// returns:
//  - valid packet : the socket has a packet that is no longer in use and can be safely modified
//  - 0: a new packet needs to be allocated and safely updated
//  Note: there's no corresponding unlock function!
static TCP_V4_PACKET* F_TxSktGetLockedV4Pkt(TCB_STUB* pSkt)
{
    TCP_V4_PACKET* pktValid = NULL;
    
    // don't let TX thread interfere
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    
    if(pSkt->pV4Pkt != NULL)
    {   // we have a current TX packet
        if((pSkt->pV4Pkt->macPkt.pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_QUEUED) == 0U)
        {   // mark it in use
            pSkt->pV4Pkt->macPkt.pktFlags |= (uint32_t)TCPIP_MAC_PKT_FLAG_QUEUED;
            pktValid = pSkt->pTcpV4Pkt;
        }
        // else queued, cannot use it
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
    return pktValid;
}

// returns a pointer to the packet to be freed or 0 if no free needed
static TCPIP_MAC_PACKET* F_TxSktFreeLockedV4Pkt(TCB_STUB* pSkt)
{
    TCPIP_MAC_PACKET* toFreePkt = NULL;
    
    // don't let TX thread interfere
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);


    if(pSkt->pV4Pkt != NULL)
    {   // socket has packet
        if((pSkt->pV4Pkt->macPkt.pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_QUEUED) == 0U)
        {   // not queued; could be freed
            toFreePkt = &pSkt->pV4Pkt->macPkt;
        }
        // else queued and cannot be freed; it will be freed by the acknowledge

        // one way or another the socket is done with this packet
        pSkt->pTxPkt = NULL;
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);

    return toFreePkt;
}
#endif  // defined (TCPIP_STACK_USE_IPV4)

// helper to set the socket retransmission timeout
// if reload, it initializes it
// else it doubles it (exp back off)
static void F_TCP_LoadRetxTmo(TCB_STUB* pSkt, bool reload)
{
    uint32_t retxTmo;
    if(reload)
    {
        retxTmo = TCP_SOCKET_RETX_TMO;
    }
    else
    {
        retxTmo = pSkt->retxTmo << 1;
        if(retxTmo > TCP_SOCKET_MAX_RETX_TIME)
        {
            retxTmo = TCP_SOCKET_MAX_RETX_TIME;
        } 
    }

    pSkt->retxTmo = retxTmo;
    pSkt->retxTime = SYS_TMR_TickCountGet() + (pSkt->retxTmo * sysTickFreq) / 1000U;
}


/*****************************************************************************
  Function:
    bool TCPIP_TCP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackInit, const TCPIP_TCP_MODULE_CONFIG* pTcpInit)

  Summary:
    Initializes the TCP module.

  Description:
    Initializes the TCP module.  This function allocates memory for the TCP module
    and registers itself with the stack manager.
    If the initialization failed it will return false.

  Precondition:
    None

  Parameters:
    stackInit   - pointer to stack initialization data; contains heap, interfaces, etc

    pTcpInit    - pointer to a TCP initialization structure containing:
                    - nSockets:         number of sockets to be created
                    - sktTxBuffSize:    default TX buffer size
                    - sktRxBuffSize:    default RX buffer size
  Returns:
    true if initialization succeeded
    false otherwise
    
  Remarks:
   Simple protection for calls to TCPIP_TCP_Initialize/TCPIP_TCP_Deinitialize
   from other threads is supported.

  ***************************************************************************/
bool TCPIP_TCP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackInit, const void* initData)
{
    unsigned int nSockets;
    OSAL_RESULT  tcpSemCreate;
    bool    initRes = false;
    bool    doInit = false;
    bool    initFault = false;
    
    // sanity check
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);  // System Lock
    if(tcpLockCount == 0)
    {   // performing 1st initialization
        if(tcpInitCount != 0 || stackInit->stackAction != (uint8_t)TCPIP_STACK_ACTION_INIT)
        {
            initFault = true;
        }
        else
        {
            doInit = true;
            tcpLockCount = 2;     // get a lock
        }
    }
    else if(tcpLockCount == 1)
    {   // already initialized
        if(tcpInitCount <= 0)
        {   // should not happen
            initFault = true;
        }
        else
        {
            initRes = true;
            if(stackInit->stackAction == (uint8_t)TCPIP_STACK_ACTION_INIT)
            {   // just another interface initializing
                tcpInitCount++;
            }
        }
    }
    else
    {   // else another init/deinit is in progress
        // Do Nothing
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);  // System unlock
    
    if(initFault)
    {   // should not happen
        SYS_ASSERT(false, "TCPIP_TCP_Initialize Failed!");
        return false;
    }

    if(!doInit)
    {
        return initRes;
    }

    // do the module initialization
    // check configuration data is not missing
    const TCPIP_TCP_MODULE_CONFIG* pTcpInit = (const TCPIP_TCP_MODULE_CONFIG*)initData;
    if(stackInit->memH == NULL || pTcpInit == NULL || pTcpInit->nSockets == 0U)
    {
        SYS_ERROR(SYS_ERROR_ERROR, "TCP NULL dynamic allocation handle/init data");
        tcpLockCount = 0; // leave it uninitialized
        return false;
    }

    sysTickFreq = SYS_TMR_TickCounterFrequencyGet(); 
    tcpHeapH = stackInit->memH;
    nSockets = pTcpInit->nSockets;
    // default initialization
    tcpDefTxSize = pTcpInit->sktTxBuffSize;
    tcpDefRxSize = pTcpInit->sktRxBuffSize;

    TCBStubs = (TCB_STUB**)TCPIP_HEAP_Calloc(tcpHeapH, (size_t)nSockets, sizeof(*TCBStubs));
    if(TCBStubs == NULL)
    {
        SYS_ERROR(SYS_ERROR_ERROR, " TCP Dynamic allocation failed");
        tcpLockCount = 0; // leave it uninitialized
        return false;
    }


    TcpSockets = nSockets;
#if (TCPIP_TCP_QUIET_TIME != 0)
    tcpQuietDone = false;
    tcpStartTime = 0;
#endif  // (TCPIP_TCP_QUIET_TIME != 0)
#if ((TCPIP_TCP_DEBUG_LEVEL & TCPIP_TCP_DEBUG_MASK_TRACE_STATE) != 0)
    tcpTraceMask = 0;
#endif  // ((TCPIP_TCP_DEBUG_LEVEL & TCPIP_TCP_DEBUG_MASK_TRACE_STATE) != 0)

    // create the TCP timer
    tcpSignalHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_TCP_Task, TCPIP_TCP_TASK_TICK_RATE);
    if(tcpSignalHandle == NULL)
    {   // cannot create the TCP timer
        F_TcpCleanup();
        tcpLockCount = 0; // leave it uninitialized
        return false;
    }
    
    // initialize some default data
    tcpInitCount++;

    tcpSemCreate = OSAL_SEM_Create(&tcpSemaphore, OSAL_SEM_TYPE_BINARY, 1, 1);
    (void)tcpSemaphore;  // Remove a warning
    if(tcpSemCreate != OSAL_RESULT_SUCCESS)
    {
        F_TcpCleanup();
        tcpLockCount = 0; // leave it uninitialized
        return false;
    }

#if (TCPIP_TCP_EXTERN_PACKET_PROCESS != 0)
    tcpPktHandler = NULL;
#endif  // (TCPIP_TCP_EXTERN_PACKET_PROCESS != 0)

    tcpInitCount = 1; // initialized
    tcpLockCount = 1; // release the lock

    return true;
}

/*****************************************************************************
  Function:
    void TCPIP_TCP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackInit)

  Summary:
    De-Initializes the TCP module.

  Description:
    De-Initializes the TCP module.
    This function initializes each socket to the CLOSED state.
    If dynamic memory was allocated for the TCP sockets, the function
    will deallocate it.

  Precondition:
    TCPIP_TCP_Initialize() should have been called

  Parameters:
    stackInit   - pointer to stack initialization data; contains heap, interfaces, etc
                  and interface that's going down

  Returns:
    None
    
  Remarks:
  ***************************************************************************/
#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_TCP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackInit)
{
    bool      deinitFault = false;
    bool      doDeinit = false;

    // sanity check
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);  // System Lock
    if(tcpLockCount == 1)
    {   // module properly initialized
        if(tcpInitCount <= 0)
        {
            deinitFault = true;
        }
        else
        {
            doDeinit = true;
            tcpLockCount = 2;   // get lock
        }
    }
    // else either not initialized or some other init/deinit op in progress
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);  // System unlock

    if(deinitFault)
    {   // should not happen
        SYS_ASSERT(false, "TCPIP_TCP_Deinitialize Failed!");
    }
    else if(doDeinit)
    {   // we're up and running
        // interface is going down
        F_TCPAbortSockets(1UL << (uint32_t)stackInit->netIx, TCPIP_TCP_SIGNAL_IF_DOWN); 

        bool doRelease = false;
        if(stackInit->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
        {
            if(--tcpInitCount == 0)
            {
                doRelease = true;
            }
        }
        if(doRelease)
        {   // stack shut down and all closed; release resources
            F_TcpCleanup();
            (void) OSAL_SEM_Delete(&tcpSemaphore);
            tcpLockCount = 0;   // leave it uninitialized
        }
        else
        {   // not yet
            tcpLockCount = 1;   // release lock
        }
    }
    else
    {
        /* Do Nothing */
    }
}

// performs the clean-up of resources
static void F_TcpCleanup(void)
{
    unsigned int ix;
    TCB_STUB* pSkt;

    if(TCBStubs != NULL)
    {
        // all sockets should be closed here
        // just a sanity check
        for(ix = 0; ix < TcpSockets; ix++)
        {
            pSkt = TCBStubs[ix]; 
            if(pSkt != NULL) 
            {
                F_TcpAbort(pSkt, TCP_ABORT_FLAG_SHUTDOWN, (TCPIP_TCP_SIGNAL_TYPE)0);
            }
        }
    }

    (void) TCPIP_HEAP_Free(tcpHeapH, TCBStubs);
    TCBStubs = NULL;

    TcpSockets = 0;

    if(tcpSignalHandle != NULL)
    {
        TCPIPStackSignalHandlerDeregister(tcpSignalHandle);
        tcpSignalHandle = NULL;
    }

}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

#if (TCPIP_STACK_DOWN_OPERATION != 0) || (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)
// netMask is the mask of interfaces going down: 1 << netIx
static void F_TCPAbortSockets(uint32_t netMask, TCPIP_TCP_SIGNAL_TYPE sigType)
{
    unsigned int ix;
    TCB_STUB* pSkt;

    for(ix = 0; ix < TcpSockets; ix++)
    {
        if((pSkt = TCBStubs[ix]) != NULL)  
        {
            int netIx = TCPIP_STACK_NetIxGet(pSkt->pSktNet);
            if(netIx >= 0 )
            {   // interface exists
                uint32_t sktIfMask = 1UL << (uint32_t)netIx;
                if((sktIfMask & netMask) != 0U)
                {   // match
                    // just disconnect, don't kill sockets
                    bool isServer = (pSkt->flags.bServer != 0U);
                    pSkt->flags.bServer = 1U;
                    F_TcpAbort(pSkt, TCP_ABORT_FLAG_REGULAR, sigType);
                    // restore the setting
                    pSkt->flags.bServer = isServer ? 1U : 0U;
                }
            }
        }
    }
} 
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0) || (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)

/*****************************************************************************
  Function:
     TCP_SOCKET TCPIP_TCP_ServerOpen(IP_ADDRESS_TYPE addType, TCP_PORT localPort,  IP_MULTI_ADDRESS* localAddress)

  Summary:
    Opens a TCP socket as a server.
    
  Description:
    Provides a unified method for opening TCP server sockets. 

    Sockets are statically/dynamically allocated on boot, and can be claimed with this
    function and freed using TCPIP_TCP_Close.

  Precondition:
    TCP is initialized.

  Parameters:
    IP_ADDRESS_TYPE addType         -   The type of address being used. Example: IP_ADDRESS_TYPE_IPV4.
    TCP_PORT localPort              -   TCP port to listen on for connections.
    IP_MULTI_ADDRESS* localAddress  -   Local address to use.
    
  Returns:
    INVALID_SOCKET -  No sockets of the specified type were available to be
                      opened.
    Otherwise -       A TCP_SOCKET handle. Save this handle and use it when
                      calling all other TCP APIs. 
 ***************************************************************************/
TCP_SOCKET TCPIP_TCP_ServerOpen(IP_ADDRESS_TYPE addType, TCP_PORT localPort,  IP_MULTI_ADDRESS* localAddress)
{
    TCP_SOCKET  skt;
    TCPIP_NET_IF* pDefIf = NULL;

#if !defined (TCPIP_STACK_USE_IPV6)
   if(addType == IP_ADDRESS_TYPE_IPV6)
   {
       return INVALID_SOCKET;
   } 
#endif  // defined (TCPIP_STACK_USE_IPV6)
    
#if !defined (TCPIP_STACK_USE_IPV4)
   if(addType == IP_ADDRESS_TYPE_IPV4)
   {
       return INVALID_SOCKET;
   }
#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV4)
    if(addType == IP_ADDRESS_TYPE_IPV4 && localAddress != NULL && localAddress->v4Add.Val != 0U)
    {
        pDefIf = TCPIP_STACK_IPAddToNet(&localAddress->v4Add, false);
        if(pDefIf == NULL)
        {    // no such interface
            return INVALID_SOCKET;
        }
    }
#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV6)
   if(addType == IP_ADDRESS_TYPE_IPV6 && localAddress != NULL)
   {
       pDefIf = TCPIPStackIPv6AddToNet(&localAddress->v6Add, IPV6_ADDR_TYPE_UNICAST, false);
       if(pDefIf == NULL)
       {    // no such interface
           return INVALID_SOCKET;
       }
   }
#endif  // defined (TCPIP_STACK_USE_IPV6)

    skt = F_TCP_Open(addType, TCP_OPEN_SERVER, localPort, NULL);
    if(skt != INVALID_SOCKET)
    {
        TCB_STUB* pSkt = TCBStubs[skt]; 
        F_TcpSocketBind(pSkt, pDefIf, localAddress);
        if(pDefIf != NULL)
        {
            pSkt->flags.openBindIf = 1;
        }
        if(localAddress != NULL)
        {
            pSkt->flags.openBindAdd = 1;
        }

    }

    return skt;

}

/*****************************************************************************
  Function:
     TCP_SOCKET TCPIP_TCP_ClientOpen(IP_ADDRESS_TYPE addType, TCP_PORT remotePort, IP_MULTI_ADDRESS* remoteAddress)

  Summary:
    Opens a TCP socket as a client.
    
  Description:
    Provides a unified method for opening TCP client sockets. 

    Sockets are statically/dynamically allocated on boot, and can be claimed with this
    function and freed using TCPIP_TCP_Abort or TCPIP_TCP_Close.

  Precondition:
    TCP is initialized.

  Parameters:
    IP_ADDRESS_TYPE addType         -   The type of address being used. Example: IP_ADDRESS_TYPE_IPV4.
    TCP_PORT remotePort             -   TCP port to connect to.
                                          The local port for client sockets will be automatically picked
                                          by the TCP module.
    IP_MULTI_ADDRESS* remoteAddress -   The remote address to be used.
    
  Returns:
    INVALID_SOCKET -  No sockets of the specified type were available to be
                      opened.
    Otherwise -       A TCP_SOCKET handle. Save this handle and use it when
                      calling all other TCP APIs. 

 Remarks:
    IP_ADDRESS_TYPE_ANY is not supported!
 ***************************************************************************/
TCP_SOCKET TCPIP_TCP_ClientOpen(IP_ADDRESS_TYPE addType, TCP_PORT remotePort, IP_MULTI_ADDRESS* remoteAddress)
{
    TCP_SOCKET  skt;

#if defined (TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_IPV6)
    if(addType != IP_ADDRESS_TYPE_IPV4 && addType != IP_ADDRESS_TYPE_IPV6)
    {
        return INVALID_SOCKET; 
    }
#elif defined (TCPIP_STACK_USE_IPV4)
    if(addType != IP_ADDRESS_TYPE_IPV4)
    {
        return INVALID_SOCKET; 
    }
#elif defined (TCPIP_STACK_USE_IPV6)
    if(addType != IP_ADDRESS_TYPE_IPV6)
    {
        return INVALID_SOCKET; 
    }
#else
    return INVALID_SOCKET; 
#endif  // defined (TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_IPV6)


    skt = F_TCP_Open(addType, TCP_OPEN_CLIENT, remotePort, remoteAddress);
    // leave it unbound

    return skt;
}


/****************************************************************************
  Section:
    Connection Management Functions
  ***************************************************************************/


/*****************************************************************************
  Function:
    TCP_SOCKET F_TCP_Open(IP_ADDRESS_TYPE addType, TCP_OPEN_TYPE opType, TCP_PORT port, IP_MULTI_ADDRESS* address)
    
  Summary:
    Opens a TCP socket for listening or as a client.

  Description:
    Provides a unified method for opening TCP sockets. This function can
    open both client and server sockets.
    It accepts an IP address in binary form.
    
    Sockets are dynamically allocated with this function
    and freed using TCPIP_TCP_Abort or TCPIP_TCP_Close.

  Conditions:
    TCP is initialized.

  Input:
    addType     - IPv4/IPv6 address type spec
    opType      - TCP_OPEN_SERVER/TCP_OPEN_CLIENT ro open a server or a client socket
    port        - for a client socket this is the remote port
                  for a server socket this is the local port the server is listening on
    hostAddress - A big endian IP address
                  for a client socket this is the address of the host to connect to
                  for a server socket this is not currently used
                  (it may be used in the future for local binding though).
                 

  Return Values:
    INVALID_SOCKET -  No sockets of the specified type were available to be
                      opened.
    Otherwise -       A TCP_SOCKET handle. Save this handle and use it when
                      calling all other TCP APIs.

  Remarks:
    When finished using the TCP socket handle, call the TCPIP_TCP_Close() function to free the 
    socket and delete the handle.

    IP_ADDRESS_TYPE_ANY is supported for server sockets only!

  *****************************************************************************/
static TCP_SOCKET F_TCP_Open(IP_ADDRESS_TYPE addType, TCP_OPEN_TYPE opType, TCP_PORT port, IP_MULTI_ADDRESS* hostAddress)
{
    TCB_STUB*  pSkt;
    TCP_SOCKET hTCP;
    TCP_PORT   localPort, remotePort;     
    uint8_t    *txBuff, *rxBuff;

    if(opType == TCP_OPEN_CLIENT)
    {
        localPort = 0;
        remotePort = port;
    }
    else
    {   // for server socket allow multiple sockets listening on the same port
        localPort = port;
        remotePort = 0;
    }

    // allocate an ephemeral port for both server and client
    if(localPort == 0U)
    {
        localPort = F_TCP_EphemeralPortAllocate();
        if(localPort  == 0u)
        {
            return INVALID_SOCKET;
        }
    }


    pSkt = (TCB_STUB*)1;
    // Shared Data Lock
    if (OSAL_SEM_Pend(&tcpSemaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
    {
        // SYS_DEBUG message
    }
    for(hTCP = 0; (uint32_t)hTCP < TcpSockets; hTCP++)
    {
        pSkt = TCBStubs[hTCP];
        if(pSkt == NULL)
        {   // found an empty slot
            break;
        }
    }


    if(pSkt != NULL)
    {   // all slots taken
        if (OSAL_SEM_Post(&tcpSemaphore) != OSAL_RESULT_SUCCESS)
        {
            // SYS_DEBUG message
        }
        return INVALID_SOCKET;
    }

    pSkt = (TCB_STUB*)TCPIP_HEAP_Calloc(tcpHeapH, 1, sizeof(*pSkt));
    txBuff = (uint8_t*)TCPIP_HEAP_Malloc(tcpHeapH, (uint16_t)(tcpDefTxSize + 1U));
    rxBuff = (uint8_t*)TCPIP_HEAP_Malloc(tcpHeapH, (uint16_t)(tcpDefRxSize + 1U));

    if(pSkt == NULL || txBuff == NULL || rxBuff == NULL)
    {   // out of memory
        (void) TCPIP_HEAP_Free(tcpHeapH, rxBuff);
        (void) TCPIP_HEAP_Free(tcpHeapH, txBuff);
        (void) TCPIP_HEAP_Free(tcpHeapH, pSkt);
        // Shared Data Lock
        if (OSAL_SEM_Post(&tcpSemaphore) != OSAL_RESULT_SUCCESS)
        {
            // SYS_DEBUG message
        }
        return INVALID_SOCKET;
    }

    F_TcpSocketInitialize(pSkt, hTCP, txBuff, tcpDefTxSize, rxBuff, tcpDefRxSize);
    // Shared Data Lock
    if (OSAL_SEM_Post(&tcpSemaphore) != OSAL_RESULT_SUCCESS)
    {
        // SYS_DEBUG message
    }
    // set all the proper members 
    F_TcpSocketSetIdleState(pSkt);

    pSkt->addType = (uint8_t)addType;
    switch(addType)
    {
#if defined (TCPIP_STACK_USE_IPV6)
        case IP_ADDRESS_TYPE_IPV6:
            pSkt->pTxPkt = F_TCPv6AllocateTxPacketStruct (pSkt);
            break;
#endif  // defined (TCPIP_STACK_USE_IPV6)

#if defined (TCPIP_STACK_USE_IPV4)
        case IP_ADDRESS_TYPE_IPV4:
            pSkt->pTxPkt = F_TcpAllocateTxPacket (pSkt, IP_ADDRESS_TYPE_IPV4);
            break;
#endif  // defined (TCPIP_STACK_USE_IPV4)

        default:
            pSkt->pTxPkt = NULL;   // default for IP_ADDRESS_TYPE_ANY
            break;
    }

    if (pSkt->pTxPkt == NULL && pSkt->addType != (uint8_t)IP_ADDRESS_TYPE_ANY)
    {   // failed to allocate memory
        F_TcpSocketKill(pSkt);
        return INVALID_SOCKET;
    }

    pSkt->flags.openAddType = (uint8_t)addType;

    if(opType == TCP_OPEN_SERVER)
    {
        pSkt->localPort = localPort;
        pSkt->flags.bServer = 1U;
        F_TcpSocketSetState(pSkt, TCPIP_TCP_STATE_LISTEN);
        pSkt->remoteHash = localPort;
    }
    // Handle all the client mode socket types
    else
    {   // TCP_CLIENT_SOCKET
        // Set the non-zero TCB fields
        pSkt->localPort = localPort;
        pSkt->remotePort = remotePort;

        // Flag to start the SYN process
        pSkt->eventTime = SYS_TMR_TickCountGet();
        pSkt->flags.bTimerEnabled = 1;

        switch(addType)
        {
#if defined (TCPIP_STACK_USE_IPV4)
            case IP_ADDRESS_TYPE_IPV4:
                // hostAddress is a literal IP address.
                if(hostAddress != NULL && hostAddress->v4Add.Val != 0U)
                {   // socket remote host known
                    pSkt->destAddress.Val = hostAddress->v4Add.Val;
                }
                break;
#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV6)
            case IP_ADDRESS_TYPE_IPV6:
                // hostAddress is a pointer to an IPv6 address. 
                TCPIP_IPV6_PacketIPProtocolSet (pSkt->pV6Pkt);
                TCPIP_IPV6_DestAddressSet(pSkt->pV6Pkt, (hostAddress != NULL) ? &hostAddress->v6Add : NULL);
                // IPv6 client socket needs a source interface for selecting a source address
                F_TcpSocketBind(pSkt, (const TCPIP_NET_IF*)TCPIP_STACK_NetDefaultGet(), NULL); 

                break;
#endif  // defined (TCPIP_STACK_USE_IPV6)

            default:
                /* Do Nothing */
                break;  // IP_ADDRESS_TYPE_ANY
        }

        F_TcpSocketSetState(pSkt, (F_TcpClientSocketConnect(pSkt) >= 0) ? TCPIP_TCP_STATE_SYN_SENT : TCPIP_TCP_STATE_CLIENT_WAIT_CONNECT);
    }

    return hTCP;        

}

void  TCPIP_TCP_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

#if (TCPIP_TCP_QUIET_TIME != 0)
    if(!tcpQuietDone)
    {
        if(tcpStartTime == 0)
        {   // make sure the TMR is started
            if((tcpStartTime = SYS_TMR_TickCountGet()) != 0)
            {   // calculate the timeout
                tcpStartTime += TCPIP_TCP_QUIET_TIME * sysTickFreq;
            }
        }
        else if((int32_t)(SYS_TMR_TickCountGet() - tcpStartTime) >= 0)
        {
            tcpQuietDone = true;
        }
    }
#endif  // (TCPIP_TCP_QUIET_TIME != 0)

#if (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)
    uint32_t  ifMask;

    sigPend = TCPIPStackModuleSignalParamGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL, &ifMask);

    if(((uint32_t)sigPend & (uint32_t)(TCPIP_MODULE_SIGNAL_IF_CHANGE)) != 0U)
    { // interface address change occurred
        F_TCPAbortSockets(ifMask, TCPIP_TCP_SIGNAL_IF_CHANGE); 
    }
#else
    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);
#endif  // (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)

    if(((uint32_t)sigPend & (uint32_t)TCPIP_MODULE_SIGNAL_RX_PENDING) != 0U)
    { //  RX signal occurred
        TCPIP_TCP_Process();
    }

    if(((uint32_t)sigPend & (uint32_t)TCPIP_MODULE_SIGNAL_TMO) != 0U)
    { // regular TMO occurred
        TCPIP_TCP_Tick();
    }
}

static void TCPIP_TCP_Process(void)
{
    TCPIP_MAC_PACKET*   pRxPkt;
    TCPIP_MAC_PKT_ACK_RES ackRes;

    // extract queued TCP packets
    while((pRxPkt = TCPIPStackModuleRxExtract(TCPIP_THIS_MODULE_ID)) != NULL)
    {
        TCPIP_PKT_FlightLogRx(pRxPkt, TCPIP_THIS_MODULE_ID);
#if (TCPIP_TCP_EXTERN_PACKET_PROCESS != 0)
        if(tcpPktHandler != NULL)
        {
            bool was_processed = (*tcpPktHandler)(pRxPkt->pktIf, pRxPkt, tcpPktHandlerParam);
            if(was_processed)
            {
                TCPIP_PKT_FlightLogAcknowledge(pRxPkt, TCPIP_THIS_MODULE_ID, TCPIP_MAC_PKT_ACK_EXTERN);
                continue;
            }
        }
#endif  // (TCPIP_TCP_EXTERN_PACKET_PROCESS != 0)

        ackRes = TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR;
#if (TCPIP_TCP_QUIET_TIME != 0)
        if(tcpQuietDone)
#endif  // (TCPIP_TCP_QUIET_TIME != 0)
        {
            if(!F_TCP_RxPktValidate(pRxPkt))
            {   // discard packet
                ackRes = TCPIP_MAC_PKT_ACK_STRUCT_ERR;
            }
#if defined (TCPIP_STACK_USE_IPV4)
            else if((pRxPkt->pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_NET_TYPE) == TCPIP_MAC_PKT_FLAG_IPV4) 
            {
                ackRes = TCPIP_TCP_ProcessIPv4(pRxPkt);
            }
#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV6)
            else if((pRxPkt->pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_NET_TYPE) == TCPIP_MAC_PKT_FLAG_IPV6) 
            {
                ackRes = TCPIP_TCP_ProcessIPv6(pRxPkt);
            }
#endif  // defined (TCPIP_STACK_USE_IPV6)
            else
            {
                /* Do Nothing */
            }
        }

        if(ackRes != TCPIP_MAC_PKT_ACK_NONE)
        {   // unknown/error; discard it.
            TCPIP_PKT_PacketAcknowledge(pRxPkt, ackRes);
        }
    }
}

// validates a rx-ed TCP packet
// returns true if OK, false if packet should be discarded
static bool F_TCP_RxPktValidate(TCPIP_MAC_PACKET* pRxPkt)
{
    while(true)
    {
        uint16_t tcpTotLength = pRxPkt->totTransportLen;
        if(tcpTotLength < sizeof(TCP_HEADER))
        {   // discard packet
            break;
        }

        // check options validity
        TCP_HEADER* pHdr = FC_U8Ptr2TcpHdr(pRxPkt->pTransportLayer);
        uint8_t optionsField = pHdr->DataOffset.Val;
        if(optionsField < TCP_DATA_OFFSET_VAL_MIN)
        {
           break;
        }

        if(tcpTotLength < (uint16_t)optionsField << 2)
        {   // no payload?
            break;
        }
        
        // OK
        return true;
    }

    return false;
}

#if defined (TCPIP_STACK_USE_IPV4)

static bool     F_TcpSocketSetSourceInterface(TCB_STUB * pSkt)
{
    if(pSkt->flags.srcSet == 0U || pSkt->pSktNet == NULL)
    {
        pSkt->pSktNet = (const TCPIP_NET_IF*)TCPIP_IPV4_SelectSourceInterface(pSkt->pSktNet, &pSkt->destAddress, &pSkt->srcAddress, pSkt->flags.srcSet != 0U);
        if(pSkt->pSktNet == NULL)
        {   // cannot find an route?
            return false;
        }
        pSkt->flags.srcSet = 1;
    }

    return true;
}

// allocates an(other) TX packet if none or the current one is queued
// returns true for success (including not queued), false otherwise
// resetOldPkt will reset the status of an old packet, if exists
static TCP_V4_PACKET* F_Tcpv4AllocateTxPacketIfQueued (TCB_STUB * pSkt, bool resetOldPkt)
{
    TCP_V4_PACKET* oldPkt = F_TxSktGetLockedV4Pkt(pSkt);

    if(oldPkt == NULL)
    {   // no packet or queued, try to get another
        TCP_V4_PACKET* pPkt = F_TcpAllocateTxPacket(pSkt, IP_ADDRESS_TYPE_IPV4);
        if(pPkt != NULL)
        {   // mark it as taken
            pPkt->v4Pkt.macPkt.pktFlags |= (uint32_t)TCPIP_MAC_PKT_FLAG_QUEUED;
            OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
            if(pSkt->pV4Pkt == NULL)
            {   // no current packet
                pSkt->pTxPkt = pPkt;
            }
            OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
        }
        // else cannot allocate packet
        // better chances next time or when the existing packet is unqueued
        return pPkt;
    }

    // else valid old packet
    if(resetOldPkt)
    {
        F_Tcpv4UnlinkDataSeg(oldPkt);  // clean packet
    }

    return oldPkt;
}

static TCP_V4_PACKET* F_TcpAllocateTxPacket(TCB_STUB* pSkt, IP_ADDRESS_TYPE addType)
{
    uint32_t allocFlags;
    TCP_V4_PACKET*  pv4Pkt;
    union
    {
        TCPIP_MAC_PACKET*   pMacPkt;
        TCP_V4_PACKET*      pTcp4Pkt;
        IPV4_PACKET*        pIp4Pkt;
    }U_TCPV4_PACKET;


    if(addType != IP_ADDRESS_TYPE_IPV4)
    {   // allocation occurs only for IP_ADDRESS_TYPE_IPV4
        return NULL;
    }

    // allocate IPv4 packet
    allocFlags  = ((uint32_t)TCPIP_MAC_PKT_FLAG_IPV4 | (uint32_t)TCPIP_MAC_PKT_FLAG_SPLIT | (uint32_t)TCPIP_MAC_PKT_FLAG_TX | (uint32_t)TCPIP_MAC_PKT_FLAG_TCP);
    // allocate from main packet pool
    // make sure there's enough room for TCP_OPTIONS
    U_TCPV4_PACKET.pMacPkt = TCPIP_PKT_SocketAlloc((uint16_t)(sizeof(TCP_V4_PACKET)), (uint16_t)(sizeof(TCP_HEADER)), (uint16_t)(sizeof(TCP_OPTIONS)), (TCPIP_MAC_PACKET_FLAGS)allocFlags);

    pv4Pkt = U_TCPV4_PACKET.pTcp4Pkt;
    if(pv4Pkt != NULL)
    {   // lazy linking of the data segments, when needed
        TCPIP_PKT_PacketAcknowledgeSet(&pv4Pkt->v4Pkt.macPkt, &F_Tcpv4TxAckFnc, pSkt);

        pv4Pkt->tcpSeg[0].segFlags = pv4Pkt->tcpSeg[1].segFlags = (uint16_t)TCPIP_MAC_SEG_FLAG_STATIC; // embedded in packet itself

    }


    return pv4Pkt;
}


static void F_Tcpv4TxAckFnc (TCPIP_MAC_PACKET * pPkt, const void * param)
{
    TCPIP_NET_HANDLE pktIf = NULL;
    uint32_t sigType = 0;
    TCP_SOCKET   sktIx = 0;
    const TCB_STUB* pSkt = (const TCB_STUB*)param;
    bool critLock = false;
    bool freePkt = true;
    TCPIP_TCP_SIGNAL_FUNCTION sigHandler = NULL;
    const void* sigParam = NULL;
    OSAL_CRITSECT_DATA_TYPE status = 0;

    while(pSkt != NULL)
    {  
        // make sure the user threads don't mess with the socket right now
        status =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        critLock = true;

        sktIx = pSkt->sktIx;
        if(TCBStubs == NULL || sktIx < 0 || (uint32_t)sktIx >= TcpSockets || pSkt != TCBStubs[sktIx])
        {   // no longer this socket/invalid
            break;
        }

        if((pPkt->pDSeg->segFlags & (uint16_t)TCPIP_MAC_SEG_FLAG_USER_PAYLOAD) != 0U)
        {
            sigType = (uint32_t)TCPIP_TCP_SIGNAL_TX_DATA_DONE | (uint32_t)TCPIP_TCP_SIGNAL_TX_DONE;
        }
        else
        {
            sigType = (uint32_t)TCPIP_TCP_SIGNAL_TX_DONE;
        }

        sigType &= (uint32_t)pSkt->sigMask;
        
        if (sigType != 0U)
        {
            sigHandler = pSkt->sigHandler;
            sigParam = pSkt->sigParam;
            pktIf = pPkt->pktIf;
        }

        union
        {
            TCPIP_MAC_PACKET*   pMacPkt;
            TCP_V4_PACKET*      pTcp4Pkt;
            IPV4_PACKET*        pIp4Pkt;
        }U_TCPV4_PACKET;


        U_TCPV4_PACKET.pMacPkt = pPkt; 
        if(pSkt->pTcpV4Pkt != U_TCPV4_PACKET.pTcp4Pkt)
        {   // no longer using this packet;
            break;
        }

        // OK, keep the current packet
        F_Tcpv4UnlinkDataSeg(pSkt->pTcpV4Pkt);
        pPkt->pktFlags &= ~((uint32_t)TCPIP_MAC_PKT_FLAG_QUEUED);
        freePkt = false;
        break;
    }

    if(critLock)
    {
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
    }


    if(freePkt)
    {   // discard packet
        // either closed socket or another packet already allocated
        TCPIP_PKT_PacketFree(pPkt);
    }

    if(sigHandler != NULL)
    {   // notify socket user
        (*sigHandler)(sktIx, pktIf, (TCPIP_TCP_SIGNAL_TYPE)sigType, sigParam);
    }

}

// unlinks the data segments
static void F_Tcpv4UnlinkDataSeg(TCP_V4_PACKET* pTcpPkt)
{
    TCPIP_MAC_DATA_SEGMENT* pSeg = pTcpPkt->tcpSeg;
    pSeg->segLen = pSeg->segSize = 0;
    pSeg->next = NULL;
    pSeg++;
    pSeg->segLen = pSeg->segSize = 0;
    pSeg->next = NULL;

    pTcpPkt->v4Pkt.macPkt.pDSeg->next = NULL;    // unlink to the 1st data seg; leave header only
    pTcpPkt->v4Pkt.macPkt.pDSeg->segLen = 0;  // clear the payload

}

// links at most 2 data segments to a TCP socket
static void F_Tcpv4LinkDataSeg(TCP_V4_PACKET* pTcpPkt, uint8_t* pBuff1, uint16_t bSize1, uint8_t* pBuff2, uint16_t bSize2)
{
    TCPIP_MAC_DATA_SEGMENT* pSeg0 = pTcpPkt->tcpSeg + 0;
    TCPIP_MAC_DATA_SEGMENT* pSeg1 = pTcpPkt->tcpSeg + 1;

    if(pBuff2 != NULL)
    {

        pSeg1->segLen = pSeg1->segSize = bSize2;
        pSeg1->segLoad = pBuff2;
        pSeg0->next = pSeg1;
    }
    else
    {   // unused
        pSeg0->next = NULL;
    }

    if(pBuff1 != NULL)
    {
        pSeg0->segLen = pSeg0->segSize = bSize1;
        pSeg0->segLoad = pBuff1;
        pTcpPkt->v4Pkt.macPkt.pDSeg->next = pSeg0;  // link to the 1st data seg to be transmitted
    }
    else
    {   // unused
        pTcpPkt->v4Pkt.macPkt.pDSeg->next = NULL;  // link to the 1st data seg to be transmitted
    }

}

static bool F_TCPv4Flush(TCB_STUB * pSkt, IPV4_PACKET* pv4Pkt, uint16_t hdrLen, uint16_t loadLen)
{
    TCP_HEADER*         pTCPHdr;
    IPV4_PSEUDO_HEADER  pseudoHdr;
    TCPIP_IPV4_PACKET_PARAMS pktParams;
    uint16_t            checksum;

    if(pSkt->destAddress.Val == 0U)
    {   // don't even bother
        return 0;
    }

    if(!F_TcpSocketSetSourceInterface(pSkt))
    {   // cannot find an route?
        return 0;
    }

    pv4Pkt->srcAddress.Val = pSkt->srcAddress.Val;
    pv4Pkt->destAddress.Val = pSkt->destAddress.Val;
    pv4Pkt->netIfH = pSkt->pSktNet;

    // start preparing the TCP packet
    pv4Pkt->macPkt.pDSeg->segLen += hdrLen;

    // add the TCP header
    pTCPHdr = FC_U8Ptr2TcpHdr(pv4Pkt->macPkt.pTransportLayer);
    pTCPHdr->Checksum = 0;

    if((pSkt->pSktNet->txOffload & (uint8_t)TCPIP_MAC_CHECKSUM_TCP) == 0U)
    {   // not handled by hardware
        // add the pseudo header
        union
        {
            TCPIP_MAC_PACKET*   pMacPkt;
            TCP_V4_PACKET*      pTcp4Pkt;
            IPV4_PACKET*        pIp4Pkt;
        }U_TCPV4_PACKET;

        U_TCPV4_PACKET.pMacPkt = &pv4Pkt->macPkt; 

        pseudoHdr.SourceAddress.Val = pv4Pkt->srcAddress.Val;
        pseudoHdr.DestAddress.Val = pv4Pkt->destAddress.Val;
        pseudoHdr.Zero = 0;
        pseudoHdr.Protocol = (uint8_t)IP_PROT_TCP;
        pseudoHdr.Length = TCPIP_Helper_htons(hdrLen + loadLen);
        checksum = ~TCPIP_Helper_CalcIPChecksum((uint8_t*)&pseudoHdr, (uint16_t)sizeof(pseudoHdr), 0U);

        checksum = ~TCPIP_Helper_CalcIPChecksum((uint8_t*)pTCPHdr, hdrLen, checksum);
        if(loadLen != 0U)
        {   // add the data segments
            pv4Pkt->macPkt.pDSeg->segFlags |= ((uint16_t)TCPIP_MAC_SEG_FLAG_USER_PAYLOAD);
            checksum = ~TCPIP_Helper_PacketChecksum(&pv4Pkt->macPkt, (U_TCPV4_PACKET.pTcp4Pkt)->tcpSeg[0].segLoad, loadLen, checksum);
        }
        else
        {   // packet not carying user payload
            pv4Pkt->macPkt.pDSeg->segFlags &= ~((uint16_t)TCPIP_MAC_SEG_FLAG_USER_PAYLOAD);
        }

        pTCPHdr->Checksum = ~checksum;
    }


    pktParams.ttl = pSkt->ttl;
    pktParams.tosFlags = (TCPIP_IPV4_TOS_FLAGS)pSkt->tos;
    pktParams.df = 0;

    // and we're done
    TCPIP_IPV4_PacketFormatTx(pv4Pkt, (uint8_t)IP_PROT_TCP, hdrLen + loadLen, &pktParams);
    pv4Pkt->macPkt.next = NULL;    // single packet
    TCPIP_PKT_FlightLogTxSkt(&pv4Pkt->macPkt, TCPIP_THIS_MODULE_ID, ((uint32_t)pSkt->localPort << 16) | pSkt->remotePort, (uint16_t)pSkt->sktIx);
    if(TCPIP_IPV4_PacketTransmit(pv4Pkt))
    {
        return true; 
    }
    // failed
    TCPIP_PKT_PacketAcknowledge(&pv4Pkt->macPkt, TCPIP_MAC_PKT_ACK_IP_REJECT_ERR);

    return false;
}

static TCPIP_MAC_PKT_ACK_RES TCPIP_TCP_ProcessIPv4(TCPIP_MAC_PACKET* pRxPkt)
{
    TCP_HEADER*         pTCPHdr;
    uint16_t            tcpTotLength;
    uint16_t            optionsSize;
    const IPV4_ADDR*    pPktSrcAdd;
    const IPV4_ADDR*    pPktDstAdd;
    IPV4_PSEUDO_HEADER  pseudoHdr;
    uint16_t            calcChkSum;
    TCB_STUB*           pSkt; 
    TCPIP_TCP_SIGNAL_FUNCTION sigHandler;
    const void*         sigParam;
    uint16_t            sigMask;
    TCPIP_MAC_PKT_ACK_RES ackRes;
    uint32_t            sktEvent = 0;

    pTCPHdr = FC_U8Ptr2TcpHdr(pRxPkt->pTransportLayer);
    tcpTotLength = pRxPkt->totTransportLen;

    pPktSrcAdd = TCPIP_IPV4_PacketGetSourceAddress(pRxPkt);
    pPktDstAdd = TCPIP_IPV4_PacketGetDestAddress(pRxPkt);

    if((pRxPkt->pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_RX_CHKSUM_TCP) == 0U)
    {
        // Calculate IP pseudoheader checksum.
        pseudoHdr.SourceAddress.Val = pPktSrcAdd->Val;
        pseudoHdr.DestAddress.Val = pPktDstAdd->Val;
        pseudoHdr.Zero  = 0;
        pseudoHdr.Protocol = (uint8_t)IP_PROT_TCP;
        pseudoHdr.Length = TCPIP_Helper_ntohs(tcpTotLength);

        calcChkSum = ~TCPIP_Helper_CalcIPChecksum((uint8_t*)&pseudoHdr, (uint16_t)(sizeof(pseudoHdr)), 0);
        // Note: pseudoHdr length is multiple of 4!
        if((pRxPkt->pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_SPLIT) != 0U)
        {
            calcChkSum = TCPIP_Helper_PacketChecksum(pRxPkt, (uint8_t*)pTCPHdr, tcpTotLength, calcChkSum);
        }
        else
        {
            calcChkSum = TCPIP_Helper_CalcIPChecksum((uint8_t*)pTCPHdr, tcpTotLength, calcChkSum);
        }

        if(calcChkSum != 0U)
        {   // discard packet
            return TCPIP_MAC_PKT_ACK_CHKSUM_ERR;
        }
    }


    F_TcpSwapHeader(pTCPHdr);
    TCPIP_TCP_CheckRxPkt(pTCPHdr);

    // Skip over options to retrieve data bytes
    optionsSize = (((uint16_t)pTCPHdr->DataOffset.Val << 2) - (uint16_t)(sizeof(*pTCPHdr)));

    while(true)
    {
        // Find matching socket.
        pSkt = F_TcpFindMatchingSocket(pRxPkt, pPktSrcAdd, pPktDstAdd, IP_ADDRESS_TYPE_IPV4);
        if(pSkt == NULL)
        {
            // If there is no matching socket, There is no one to handle
            // this data.  Discard it.
            ackRes = TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR;
            break;
        }


        // extract header
        pRxPkt->pDSeg->segLen -=  (uint16_t)(optionsSize + sizeof(*pTCPHdr));    
        F_TcpHandleSeg(pSkt, pTCPHdr, (tcpTotLength - optionsSize - (uint16_t)sizeof(*pTCPHdr)), pRxPkt, &sktEvent);

        sigMask = F_TcpSktGetSignalLocked(pSkt, &sigHandler, &sigParam);
        sktEvent &= (uint32_t)sigMask;
        if(sktEvent != 0U)
        {
            if(sigHandler != NULL)
            {
                (*sigHandler)(pSkt->sktIx, pRxPkt->pktIf, (TCPIP_TCP_SIGNAL_TYPE)sktEvent, sigParam);
            }
        }

        // OK, pass to user
        ackRes = TCPIP_MAC_PKT_ACK_RX_OK;
        break;
    }

    // log 
#if (TCPIP_PACKET_LOG_ENABLE)
    uint32_t logPort = (pSkt != NULL) ? ((uint32_t)pSkt->localPort << 16) | pSkt->remotePort : ((uint32_t)pTCPHdr->DestPort << 16) | pTCPHdr->SourcePort;
    TCPIP_PKT_FlightLogRxSkt(pRxPkt, TCPIP_MODULE_LAYER3, logPort, pSkt != NULL ? (uint16_t)pSkt->sktIx: (uint16_t)0xffffU);
#endif  // (TCPIP_PACKET_LOG_ENABLE)

    return ackRes;
}
#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV6)
// following is the implementation for the TX thread lock
// This a global lock that needs to be very fast

// Disables the pre-emption during the socket changes in the TX process
// to protect between user threads and the TX thread
// It protects:
//  - the socket TX buffer resources which are updated by the user threads
//    and the TX thread via the F_Tcpv4TxAckFnc
//
// protects user vs TX threads when the user needs to allocate a new TX packet for a socket
// returns:
//  - true: the socket has a packet that is no longer in use and can be safely modified
//  - false: a new packet needs to be allocated and safely updated
//  Note: there's no corresponding unlock function!
static IPV6_PACKET* F_TxSktGetLockedV6Pkt(TCB_STUB* pSkt, IPV6_PACKET** ppSktPkt, bool setQueued)
{
    IPV6_PACKET* pktValid = NULL;

    if(ppSktPkt != NULL)
    {
        *ppSktPkt = NULL;
    }

    // don't let TX thread interfere
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(pSkt->pV6Pkt != NULL)
    {   // we have a current TX packet
        if(ppSktPkt != NULL)
        {
            *ppSktPkt = pSkt->pV6Pkt;
        }

        if(TCPIP_IPV6_IsPacketQueued(pSkt->pV6Pkt) == 0U)
        {   
            if(setQueued)
            {   // mark in use
                TCPIP_IPV6_PacketSetQueued(pSkt->pV6Pkt);
            }
            pktValid = pSkt->pV6Pkt;
        }
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
    return pktValid;
}

// returns a pointer to the packet to be freed or 0 if no free needed
static IPV6_PACKET* F_TxSktFreeLockedV6Pkt(TCB_STUB* pSkt)
{
    IPV6_PACKET* toFreePkt = NULL;
    
    // don't let TX thread interfere
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);


    if(pSkt->pV6Pkt != NULL)
    {   // socket has packet
        if(TCPIP_IPV6_IsPacketQueued(pSkt->pV6Pkt) == 0U)
        {   // not queued; could be freed
            toFreePkt = pSkt->pV6Pkt;
        }
        // else queued and cannot be freed; it will be freed by the acknowledge

        // one way or another the socket is done with this packet
        pSkt->pTxPkt = NULL;
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);

    return toFreePkt;
}


//
//
// allocates an(other) TX packet if none or the current one is queued
// returns true for success (including not queued), false otherwise
// resetOldPkt will reset the status of an old packet, if exists
static IPV6_PACKET* F_Tcpv6AllocateTxPacketIfQueued (TCB_STUB * pSkt, bool resetOldPkt)
{

    IPV6_PACKET* pSktPkt;
    IPV6_PACKET* unqueuedPkt = F_TxSktGetLockedV6Pkt(pSkt, &pSktPkt, true);

    if(unqueuedPkt == NULL)
    {   // no packet or queued, try to get another
        IPV6_PACKET * tempPtr = F_TCPv6AllocateTxPacketStruct (pSkt);
        if(tempPtr != NULL)
        {   // success
            if(pSktPkt == NULL)
            {   // no current socket packet; use this one
                pSkt->pV6Pkt = tempPtr;
            }
            else
            {   // need to copy from old packet structure
                if (!TCPIP_IPV6_TxPacketStructCopy (tempPtr, pSkt->pV6Pkt))
                {   // failed
                    TCPIP_IPV6_PacketFree (tempPtr);
                    tempPtr = NULL;
                }
            }
        }
        // else cannot allocate packet
        // better chances next time or when the existing packet is unqueued
        return tempPtr;
    }

    // else valid unqueued old packet
    if(resetOldPkt)
    {
        TCPIP_IPV6_TransmitPacketStateReset (unqueuedPkt); // clean packet
    }

    return unqueuedPkt;
}


static IPV6_PACKET * F_TCPv6AllocateTxPacketStruct (TCB_STUB * pSkt)
{
    IPV6_PACKET * pkt;

    pkt = TCPIP_IPV6_TxPacketAllocate (pSkt->pSktNet, &F_Tcpv6AckFnc, pSkt);

    if (pkt == NULL)
    {
        return NULL;
    }

    if (TCPIP_IPV6_UpperLayerHeaderPut (pkt, NULL, (uint16_t)(sizeof (TCP_HEADER)), ((uint8_t)IP_PROT_TCP), TCP_CHECKSUM_OFFSET) == NULL)
    {
        TCPIP_IPV6_PacketFree (pkt);
        return NULL;
    }
    
    TCPIP_IPV6_SetPacketMacAcknowledge(pkt, &F_Tcpv6MacAckFnc);

    return pkt;
}

static void F_Tcpv6AckFnc (void * pkt, bool success, const void * param)
{
    IPV6_PACKET*    pV6Pkt;
    const TCB_STUB* pSkt = (const TCB_STUB*)param;
    TCP_SOCKET   sktIx;
    bool critLock = false;
    bool freePkt = true;
    OSAL_CRITSECT_DATA_TYPE status = 0;

    if((pV6Pkt = (IPV6_PACKET*)pkt) == NULL)
    {   // shouldn't happen
        return;
    }

    while(pSkt != NULL)
    {  
        // make sure the user threads don't mess with the socket right now
        status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        critLock = true;

        sktIx = pSkt->sktIx;
        if(TCBStubs == NULL || sktIx < 0 || (uint32_t)sktIx >= TcpSockets || pSkt != TCBStubs[sktIx])
        {   // no longer this socket?
            pSkt = NULL;
            break;
        }

        if(pSkt->pV6Pkt != pV6Pkt)
        {   // no longer using this packet;
            break;
        }

        // OK, keep the current packet
        TCPIP_IPV6_TransmitPacketStateReset (pV6Pkt);
        freePkt = false;
        break;
    }

    if(critLock)
    {
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
    }

    if(freePkt)
    {   // discard packet
        // either closed socket or another packet already allocated
        TCPIP_IPV6_PacketFree (pV6Pkt);
    }

}

static void F_Tcpv6MacAckFnc (TCPIP_MAC_PACKET* pPkt,  const void* param)
{
    TCPIP_NET_HANDLE pktIf = NULL;
    uint32_t sigType = 0;
    TCP_SOCKET   sktIx = 0;
    const TCB_STUB* pSkt = (const TCB_STUB*)param;
    bool critLock = false;
    TCPIP_TCP_SIGNAL_FUNCTION sigHandler = NULL;
    const void* sigParam = NULL;
    OSAL_CRITSECT_DATA_TYPE status = 0;

    while(pSkt != NULL)
    {  
        // make sure the user threads don't mess with the socket right now
        status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        critLock = true;

        sktIx = pSkt->sktIx;
        if(TCBStubs == NULL || sktIx < 0 || (uint32_t)sktIx >= TcpSockets || pSkt != TCBStubs[sktIx])
        {   // no longer this socket?
            break;
        }

        if((pPkt->pDSeg->segFlags & (uint16_t)TCPIP_MAC_SEG_FLAG_USER_PAYLOAD) != 0U)
        {
            sigType = (uint32_t)TCPIP_TCP_SIGNAL_TX_DATA_DONE | (uint32_t)TCPIP_TCP_SIGNAL_TX_DONE;
        }
        else
        {
            sigType = (uint32_t)TCPIP_TCP_SIGNAL_TX_DONE;
        }

        sigType &= (uint32_t)pSkt->sigMask; 
        if( sigType != 0U)
        {
            sigHandler = pSkt->sigHandler;
            sigParam = pSkt->sigParam;
            pktIf = pPkt->pktIf;
        }

        break;
    }

    if(critLock)
    {
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
    }

    TCPIP_PKT_PacketFree(pPkt);
    
    if(sigHandler != NULL)
    {   // notify socket user
        (*sigHandler)(sktIx, pktIf, (TCPIP_TCP_SIGNAL_TYPE)sigType, sigParam);

    }
}

#endif  // defined (TCPIP_STACK_USE_IPV6)


/*****************************************************************************
  Function:
    bool TCPIP_TCP_WasReset(TCP_SOCKET hTCP)

    This function was added due to the possibility of ambiguity when checking the socket state
    reported by TCPIP_TCP_IsConnected.
    If a socket is closed and then immediately reopened and a SYN received,
    a call to TCPIP_TCP_IsConnected could still return true although the socket has been disconnected.
    This can cause errors for protocols such as HTTP in which a client will 
    immediately open a new connection upon closing of a prior one.
  ***************************************************************************/
bool TCPIP_TCP_WasReset(TCP_SOCKET hTCP)
{
    TCB_STUB* pSkt = F_TcpSocketChk(hTCP); 

    if(pSkt != NULL)
    {
        if(pSkt->flags.bSocketReset != 0U)
        {
            pSkt->flags.bSocketReset = 0;
            return true;
        }   
        return false;
    }

    // if no such socket, probably it was closed and disappeared
    return true;
}

bool TCPIP_TCP_WasDisconnected(TCP_SOCKET hTCP)
{
    TCB_STUB* pSkt = F_TcpSocketChk(hTCP); 

    if(pSkt != NULL)
    {
        return pSkt->flags.bRxFin != 0U;
    }

    // no such socket
    return false;
}

/*****************************************************************************
  Function:
    bool TCPIP_TCP_IsConnected(TCP_SOCKET hTCP)

  Summary:
    Determines if a socket has an established connection.

  Description:
    This function determines if a socket has an established connection to 
    a remote node.  Call this function after calling F_TCP_Open to determine 
    when the connection is set up and ready for use.  This function was 
    historically used to check for disconnections, but TCPIP_TCP_WasReset is now a
    more appropriate solution. 

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket to check.

  Return Values:
    true - The socket has an established connection to a remote node.
    false - The socket is not currently connected.

  Remarks:
    A socket is said to be connected only if it is in one of the states:
    TCPIP_TCP_STATE_ESTABLISHED, TCPIP_TCP_STATE_FIN_WAIT_1, TCPIP_TCP_STATE_FIN_WAIT_2 or TCPIP_TCP_STATE_CLOSE_WAIT
    In all of these states the socket can exchange data with the other end of the connection (either full duplex or
    only TX/RX).

    Otherwise the call will return false.
  ***************************************************************************/
bool TCPIP_TCP_IsConnected(TCP_SOCKET hTCP)
{
    TCB_STUB* pSkt = F_TcpSocketChk(hTCP); 

    return (pSkt != NULL) ? F_TCP_IsConnected(pSkt) : false;
}

// This function closes a connection to a remote node by sending a FIN
// (if currently connected).
bool TCPIP_TCP_Disconnect(TCP_SOCKET hTCP)
{
    TCB_STUB* pSkt = F_TcpSocketChk(hTCP); 
    
    if(pSkt != NULL)
    {
        TCP_SEND_RES sRes = F_TcpDisconnect(pSkt, true);
        return (int32_t)sRes >= 0;
    }

    return false;
}


static TCP_SEND_RES F_TcpDisconnect(TCB_STUB* pSkt, bool signalFIN)
{
    uint8_t         tcpFlags;
    bool            sendData;
    TCP_SEND_RES   sendRes;

    switch(pSkt->smState)
    {

        case (uint8_t)TCPIP_TCP_STATE_SYN_RECEIVED:
        case (uint8_t)TCPIP_TCP_STATE_ESTABLISHED:

            // fall through
        case (uint8_t)TCPIP_TCP_STATE_CLOSE_WAIT:
            // Send the FIN.
            // If the socket linger is on we should keep the socket opened (for the specified timeout)
            // until all the queued TX data can be sent in the background
            // Otherwise the queued TX data is discarded.
            // However since we don't support background operation for now:
            // - if linger is on: we'll try to send all the data before sending FIN
            // - if linger is off: discard the TX data, just send FIN
            //
            //
            //
            if(pSkt->flags.nonLinger != 0U)
            {
                F_TcpDiscardTx(pSkt);
            }

            // send something out only if we need a FIN or we have some data
            sendData = (signalFIN || (pSkt->txHead != pSkt->txUnackedTail));

            if(sendData)
            {
                // Transmit our last data.
                // This is done in a loop to ensure that if we have 
                // more data wating in the TX FIFO than can be sent in a single 
                // packet (due to the remote Max Segment Size packet size limit), 
                // we will keep generating more packets until either all data gets 
                // transmitted or the remote node's receive window fills up.
                tcpFlags = signalFIN? FIN | ACK : ACK;
                do
                {   
                    sendRes = F_TcpSend(pSkt, tcpFlags, SENDTCP_RESET_TIMERS);
                    if((int32_t)sendRes < 0 || pSkt->remoteWindow == 0u)
                    {
                        break;
                    }
                } while(pSkt->txHead != pSkt->txUnackedTail);
            }
            else
            {
                sendRes = TCP_SEND_OK; 
            }
            
            if((int32_t)sendRes < 0)
            {   
                // let the user know
                // another attempt may be done
                // if it's followed by an close/abort won't matter anyway
                pSkt->flags.failedDisconnect = 1;
            }
            else
            {
                pSkt->flags.failedDisconnect = 0;
                F_TcpSocketSetState(pSkt, pSkt->smState == (uint8_t)TCPIP_TCP_STATE_CLOSE_WAIT ? TCPIP_TCP_STATE_LAST_ACK : TCPIP_TCP_STATE_FIN_WAIT_1);
            }
            break;

        case (uint8_t)TCPIP_TCP_STATE_FIN_WAIT_1:
        case (uint8_t)TCPIP_TCP_STATE_FIN_WAIT_2:
        case (uint8_t)TCPIP_TCP_STATE_CLOSING:
        case (uint8_t)TCPIP_TCP_STATE_TIME_WAIT:
        case (uint8_t)TCPIP_TCP_STATE_LAST_ACK:
            sendRes = TCP_SEND_OK;
            break;
            
        case (uint8_t)TCPIP_TCP_STATE_CLIENT_WAIT_DISCONNECT:
            // special client socket state
            F_TcpCloseSocket(pSkt, (TCPIP_TCP_SIGNAL_TYPE)0);
            sendRes = TCP_SEND_OK;
            break;

        default:
            sendRes = TCP_SEND_NOT_CONN;
            break;
    }

    return sendRes;

}

// aborts a connection sending a RST to the other end of the socket
void TCPIP_TCP_Abort(TCP_SOCKET hTCP, bool killSocket)
{
    TCB_STUB* pSkt = F_TcpSocketChk(hTCP); 
    
    if(pSkt != NULL)
    {
        F_TcpAbort(pSkt, killSocket ? TCP_ABORT_FLAG_FORCE_CLOSE : TCP_ABORT_FLAG_REGULAR, (TCPIP_TCP_SIGNAL_TYPE)0);
    }

}

// internal function to abort a connection
static void F_TcpAbort(TCB_STUB* pSkt, TCP_ABORT_FLAGS abFlags, TCPIP_TCP_SIGNAL_TYPE tcpEvent)
{
    bool    sktReset = false;

    switch(pSkt->smState)
    {
        case (uint8_t)TCPIP_TCP_STATE_SYN_RECEIVED:
        case (uint8_t)TCPIP_TCP_STATE_ESTABLISHED:
        case (uint8_t)TCPIP_TCP_STATE_CLOSE_WAIT:
        case (uint8_t)TCPIP_TCP_STATE_FIN_WAIT_1:
        case (uint8_t)TCPIP_TCP_STATE_FIN_WAIT_2:
        case (uint8_t)TCPIP_TCP_STATE_LAST_ACK:
            sktReset = true;
            break;

        case (uint8_t)TCPIP_TCP_STATE_TIME_WAIT:
            if(((uint32_t)abFlags & (uint32_t)TCP_ABORT_FLAG_SHUTDOWN) == 0U)
            {
                return; // nothing to do here
            }
            break;

        default:
            /* Do Nothing */
            break;
    }

    if(sktReset)
    {
        F_TcpDiscardTx(pSkt);
        (void) F_TcpSend(pSkt, RST | ACK, 0);
    }

    if(((uint32_t)abFlags & ((uint32_t)TCP_ABORT_FLAG_FORCE_CLOSE | (uint32_t)TCP_ABORT_FLAG_SHUTDOWN)) != 0U)
    {
        pSkt->flags.forceKill = 1;
    }

    F_TcpCloseSocket(pSkt, tcpEvent);
}

// tries to initiate a connection on a client socket
// that is in wait status
// returns true if succeded, false otherwise
bool TCPIP_TCP_Connect(TCP_SOCKET hTCP)
{
    TCB_STUB* pSkt = F_TcpSocketChk(hTCP); 
    
    if((pSkt != NULL) && pSkt->smState == (uint8_t)TCPIP_TCP_STATE_CLIENT_WAIT_CONNECT)
    {
        if(F_TcpClientSocketConnect(pSkt) >= 0)
        {
            F_TcpSocketSetState(pSkt, TCPIP_TCP_STATE_SYN_SENT);
            return true;
        }
    }

    return false;
}


// Disconnects an open socket and destroys the socket handle
// including server mode socket handles.
bool TCPIP_TCP_Close(TCP_SOCKET hTCP)
{
    TCB_STUB* pSkt = F_TcpSocketChk(hTCP); 

    if(pSkt != NULL)
    {
        bool needAbort = false;

        if(pSkt->flags.nonGraceful == 0U)
        {   // end gracefully but close anyway
            pSkt->flags.forceKill = 1;
            TCP_SEND_RES sRes = F_TcpDisconnect(pSkt, true);
            if(sRes != TCP_SEND_OK)
            {   // either failed or socket is not connected
                needAbort = true;
            }
        }
        else
        {
            needAbort = true;
        }

        if(needAbort)
        {
            F_TcpAbort(pSkt, TCP_ABORT_FLAG_FORCE_CLOSE, (TCPIP_TCP_SIGNAL_TYPE)0);
        }
        return true;
    }

    return false;
}


/*****************************************************************************
  Function:
    bool TCPIP_TCP_SocketInfoGet(TCP_SOCKET hTCP, TCP_SOCKET_INFO* pInfo)

  Summary:
    Obtains information about a currently open socket.

  Description:
    Fills the provided TCP_SOCKET_INFO structure associated with this socket.

  Precondition:
    TCP is initialized and the socket is connected.

  Parameters:
    hTCP - The socket to check.

  Returns:
    true if the call succeeded
    false if no such socket or the socket is not opened
  ***************************************************************************/
bool TCPIP_TCP_SocketInfoGet(TCP_SOCKET hTCP, TCP_SOCKET_INFO* pInfo)
{

    TCB_STUB* pSkt = F_TcpSocketChk(hTCP); 
    
    if(pSkt == NULL)
    {
        return false;
    }

    if(pInfo == NULL)
    {
        return true;
    }

    while(true)
    {
        (void) memset(pInfo, 0, sizeof(*pInfo));
#if defined (TCPIP_STACK_USE_IPV6)
        if (pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
        {
            (void) memcpy((void*)&pInfo->remoteIPaddress.v6Add.v, (void*)TCPIP_IPV6_DestAddressGet(pSkt->pV6Pkt), sizeof(IPV6_ADDR));
            (void) memcpy((void*)&pInfo->localIPaddress.v6Add.v, (void*)TCPIP_IPV6_SourceAddressGet(pSkt->pV6Pkt), sizeof(IPV6_ADDR));
            pInfo->addressType = IP_ADDRESS_TYPE_IPV6;
            break;
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)

#if defined (TCPIP_STACK_USE_IPV4)
        if (pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV4)
        {
            pInfo->remoteIPaddress.v4Add = pSkt->destAddress;
            pInfo->localIPaddress.v4Add = pSkt->srcAddress;
            pInfo->addressType = IP_ADDRESS_TYPE_IPV4;
        }
#endif  // defined (TCPIP_STACK_USE_IPV4)
        break;
    }

    pInfo->remotePort = pSkt->remotePort;
    pInfo->localPort = pSkt->localPort;
    pInfo->hNet = pSkt->pSktNet;
    pInfo->state = (TCPIP_TCP_STATE)pSkt->smState;
    pInfo->rxSize = (uint16_t)((uintptr_t)pSkt->rxEnd - (uintptr_t)pSkt->rxStart);
    pInfo->txSize = (uint16_t)((uintptr_t)pSkt->txEnd - (uintptr_t)pSkt->txStart);
    pInfo->rxPending = F_TCPIsGetReady(pSkt);
    pInfo->txPending = TCPIP_TCP_FifoTxFullGet(hTCP);
    pInfo->flags = F_TCP_SktFlagsGet(pSkt);

    return true;
}

TCP_SOCKET_FLAGS TCPIP_TCP_SocketFlagsGet(TCP_SOCKET hTCP)
{
    TCP_SOCKET_FLAGS flags = TCP_SOCKET_FLAG_NONE;

    TCB_STUB* pSkt = F_TcpSocketChk(hTCP); 
    
    if(pSkt != NULL)
    {
        flags = F_TCP_SktFlagsGet(pSkt);
    }

    return flags;
}

static TCP_SOCKET_FLAGS F_TCP_SktFlagsGet(TCB_STUB* pSkt)
{
    uint16_t sktFlags = (uint16_t)TCP_SOCKET_FLAG_VALID;

    if(F_TCP_IsConnected(pSkt))
    {
        sktFlags |= (uint16_t)TCP_SOCKET_FLAG_CONNECTED;
    }

    if(pSkt->flags.bSocketReset != 0U)
    {
        sktFlags |= (uint16_t)TCP_SOCKET_FLAG_RST;
    }

    if(pSkt->flags.bRxFin != 0U)
    {
        sktFlags |= (uint16_t)TCP_SOCKET_FLAG_FIN;
    }

    return (TCP_SOCKET_FLAGS)sktFlags;
}


unsigned int TCPIP_TCP_SocketsNumberGet(void)
{
    return TcpSockets;
}

#if defined(TCPIP_TCP_DISABLE_CRYPTO_USAGE) && (TCPIP_TCP_DISABLE_CRYPTO_USAGE != false)
// sets the TCP sequence number using a pseudo random number
static uint32_t F_TCP_SktSetSequenceNo(const TCB_STUB* pSkt)
{
    uint32_t m = (uint32_t)((SYS_TIME_Counter64Get() * 1000000U / 64U ) / SYS_TIME_FrequencyGet());   // 274 seconds period > MSL = 120 seconds
    uint32_t seq = (SYS_RANDOM_PseudoGet() << 16) | (uint16_t)SYS_RANDOM_PseudoGet();
    return seq + m;
}

#else
#include "crypto/crypto.h"
// sets the TCP sequence number based on RFC 6528
// The socket identity: local and remote IP addresses + ports should be known
static uint32_t F_TCP_SktSetSequenceNo(const TCB_STUB* pSkt)
{
    CRYPT_MD5_CTX md5Ctx;
    uint32_t secretKey[16 / 4] = {0};   // 128 bits secret key

    size_t dataSize = 0;    // actual data size

    union
    {
        uint8_t     data8[16];              // hash data size should be >= MD5_DIGEST_SIZE == 16 bytes
        uint32_t    data32[16 / 4];         // same in 32 bit words
#if defined (TCPIP_STACK_USE_IPV6)
        uint32_t    ipv6HashData[52 / 4];   // 16B srcAdd, 16B destAdd, 4B ports, 16B secret key
#endif

#if defined (TCPIP_STACK_USE_IPV4)
        uint32_t    ipv4HashData[28 / 4];   // 4B srcAdd, 4B destAdd, 4B ports, 16B secret key
#endif    
    }hashData = {0};

#if defined (TCPIP_STACK_USE_IPV6)
    const IPV6_ADDR* pDstAdd, *pSrcAdd;
#endif // defined (TCPIP_STACK_USE_IPV6)
        
    (void)memset(hashData.data8, 0, sizeof(hashData));
    (void)memset(secretKey, 0, sizeof(secretKey));

    // get secret key
    (void) SYS_RANDOM_CryptoBlockGet(secretKey, sizeof(secretKey));

    // calculate the hash
    (void) CRYPT_MD5_Initialize(&md5Ctx);

#if defined (TCPIP_STACK_USE_IPV4)
    if(pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV4)
    {
        // start adding the socket local and remote connection identity
        hashData.ipv4HashData[0] = pSkt->srcAddress.Val;
        hashData.ipv4HashData[1] = pSkt->destAddress.Val;
        hashData.ipv4HashData[2] = ((uint32_t)pSkt->localPort << 16) + (uint32_t)pSkt->remotePort;
        (void) memcpy(hashData.ipv4HashData + 3, secretKey, sizeof(secretKey));
        dataSize = sizeof(hashData.ipv4HashData);
    }
#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV6)
    if(pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
    {
        pSrcAdd = TCPIP_IPV6_SourceAddressGet(pSkt->pV6Pkt); 
        pDstAdd = TCPIP_IPV6_DestAddressGet(pSkt->pV6Pkt); 

        (void) memcpy((uint8_t*)hashData.ipv6HashData + 0, pSrcAdd->v, sizeof(IPV6_ADDR));
        (void) memcpy((uint8_t*)hashData.ipv6HashData + 4, pDstAdd->v, sizeof(IPV6_ADDR));
        hashData.ipv6HashData[8] = ((uint32_t)pSkt->localPort << 16) + (uint32_t)pSkt->remotePort;
        (void) memcpy(hashData.ipv6HashData + 9, secretKey, sizeof(secretKey));
        dataSize = sizeof(hashData.ipv6HashData);
    }
#endif  // defined (TCPIP_STACK_USE_IPV6)

    (void) CRYPT_MD5_DataAdd(&md5Ctx, hashData.data8, dataSize);
    (void) CRYPT_MD5_Finalize(&md5Ctx, hashData.data8);
    uint32_t m = (uint32_t)((SYS_TIME_Counter64Get() * 1000000U / 64U ) / SYS_TIME_FrequencyGet());   // 274 seconds period > MSL = 120 seconds
    uint32_t seq = hashData.data32[0] + m;

#if ((TCPIP_TCP_DEBUG_LEVEL & TCPIP_TCP_DEBUG_MASK_SEQ) != 0)
    SYS_CONSOLE_PRINT("TCP IPv4 seq - m: 0x%08x, hash: 0x%08x, seq: 0x%08x\r\n", m, hashData.data32[0], seq); 

#if defined (TCPIP_STACK_USE_IPV6)
    char srcAddBuff[40], dstAddBuff[40];
#else
    char srcAddBuff[20], dstAddBuff[20];
#endif

#if defined (TCPIP_STACK_USE_IPV4)
    if(pSkt->addType == IP_ADDRESS_TYPE_IPV4)
    {
        TCPIP_Helper_IPAddressToString(&pSkt->srcAddress, srcAddBuff, sizeof(srcAddBuff));
        TCPIP_Helper_IPAddressToString(&pSkt->destAddress, dstAddBuff, sizeof(dstAddBuff));
    }
#endif  // defined (TCPIP_STACK_USE_IPV4)

#if defined (TCPIP_STACK_USE_IPV6)
    if(pSkt->addType == IP_ADDRESS_TYPE_IPV6)
    {
        TCPIP_Helper_IPv6AddressToString(TCPIP_IPV6_SourceAddressGet(pSkt->pV6Pkt), srcAddBuff, sizeof(srcAddBuff));
        TCPIP_Helper_IPv6AddressToString(TCPIP_IPV6_SourceAddressGet(pSkt->pV6Pkt), dstAddBuff, sizeof(dstAddBuff));
    }
#endif // defined (TCPIP_STACK_USE_IPV6)
    SYS_CONSOLE_PRINT("\tseq - src: %s, dest: %s, lport: %d, rport: %d\r\n", srcAddBuff, dstAddBuff, pSkt->localPort, pSkt->remotePort); 

#endif  // ((TCPIP_TCP_DEBUG_LEVEL & TCPIP_TCP_DEBUG_MASK_SEQ) != 0)
        return seq;
}
#endif  // defined(TCPIP_TCP_DISABLE_CRYPTO_USAGE) && (TCPIP_TCP_DISABLE_CRYPTO_USAGE != false)

/****************************************************************************
  Section:
    Transmit Functions
  ***************************************************************************/

/*****************************************************************************
  Function:
    bool TCPIP_TCP_Flush(TCP_SOCKET hTCP)

  Summary:
    Immediately transmits all pending TX data.

  Description:
    This function immediately transmits all pending TX data with a PSH 
    flag.  If this function is not called, data will automatically be sent
    when either a) the TX buffer is half full or b) the 
    TCPIP_TCP_AUTO_TRANSMIT_TIMEOUT_VAL (default: 40ms) has elapsed.

  Precondition:
    TCP is initialized and the socket is connected.

  Parameters:
    hTCP - The socket whose data is to be transmitted.

  Returns:
    true if data is flushed from the socket
    false otherwise

  Remarks:
  ***************************************************************************/
bool TCPIP_TCP_Flush(TCP_SOCKET hTCP)
{
    TCB_STUB* pSkt = F_TcpSocketChk(hTCP); 

    if((pSkt != NULL) && F_TCP_TxPktValid(pSkt))
    {
        return F_TcpFlush(pSkt);
    }
    return false;
}

static bool F_TcpFlush(TCB_STUB* pSkt)
{
    if(pSkt->txHead != pSkt->txUnackedTail && pSkt->remoteWindow != 0U)
    {   // The check remoteWindow != 0 stops us sending lots of
        // ACKs with len == 0, when the other host is slow
        // Send the TCP segment with all unacked bytes
        return (int32_t)(F_TcpSend(pSkt, ACK, SENDTCP_RESET_TIMERS)) == 0;
    }

    return false;
}


static void F_TcpDiscardTx(TCB_STUB* pSkt)
{
    // Empty the TX buffer
    pSkt->txHead = pSkt->txTail = pSkt->txUnackedTail = pSkt->txStart;
}

/*****************************************************************************
  Function:
    uint16_t TCPIP_TCP_PutIsReady(TCP_SOCKET hTCP)

  Summary:
    Determines how much free space is available in the TCP TX buffer.

  Description:
    Call this function to determine how many bytes can be written to the 
    TCP TX buffer.  If this function returns zero, the application must 
    return to the main stack loop before continuing in order to transmit
    more data.

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket to check.

  Returns:
    The number of bytes available to be written in the TCP TX buffer.
  ***************************************************************************/
uint16_t TCPIP_TCP_PutIsReady(TCP_SOCKET hTCP)
{
    TCB_STUB* pSkt = F_TcpSocketChk(hTCP); 
    
    return (pSkt != NULL)? F_TCPIsPutReady(pSkt) : 0U;
}


static uint16_t F_TCPIsPutReady(TCB_STUB* pSkt)
{
    if(pSkt->pTxPkt == NULL)
    {   // can happen if it is a server socket and opened with IP_ADDRESS_TYPE_ANY
        // and no client connected to it
        return 0;
    }

    return F_TCPSocketTxFreeSize(pSkt);
}

static uint16_t F_TCPSocketTxFreeSize(TCB_STUB* pSkt)
{
    // Unconnected sockets shouldn't be transmitting anything.
    if(!( (pSkt->smState == (uint8_t)TCPIP_TCP_STATE_ESTABLISHED) || (pSkt->smState == (uint8_t)TCPIP_TCP_STATE_CLOSE_WAIT) ))
    {
        return 0;
    }
    
    // Calculate the free space in this socket's TX FIFO
    
    if(pSkt->txHead >= pSkt->txTail)
    {
        return (uint16_t)(((uintptr_t)pSkt->txEnd - (uintptr_t)pSkt->txStart - 1U) - ((uintptr_t)pSkt->txHead - (uintptr_t)pSkt->txTail));
    }

    return (uint16_t)((uintptr_t)pSkt->txTail - (uintptr_t)pSkt->txHead - 1U);
}

uint16_t TCPIP_TCP_Put(TCP_SOCKET hTCP, uint8_t data)
{
    return TCPIP_TCP_ArrayPut(hTCP, &data, 1);
}

/*****************************************************************************
  Function:
    uint16_t TCPIP_TCP_ArrayPut(TCP_SOCKET hTCP, const uint8_t* data, uint16_t len)

  Description:
    Writes an array from RAM to a TCP socket.

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket to which data is to be written.
    data - Pointer to the array to be written.
    len  - Number of bytes to be written.

  Returns:
    The number of bytes written to the socket.  If less than len, the
    buffer became full or the socket is not conected.
  ***************************************************************************/
uint16_t TCPIP_TCP_ArrayPut(TCP_SOCKET hTCP, const uint8_t* data, uint16_t len)
{
    uint16_t wActualLen;
    uint16_t wFreeTxSpace;
    uint16_t wRightLen = 0;
    TCB_STUB* pSkt; 
    
    if(len == 0U || data == NULL)
    {
        return 0;
    }
    if((pSkt = F_TcpSocketChk(hTCP)) == NULL)
    {
        return 0;
    }

    wFreeTxSpace = F_TCPIsPutReady(pSkt);
    if(wFreeTxSpace == 0U)
    {   // no room in the socket buffer
        if(F_TCP_TxPktValid(pSkt))
        {
            (void) F_TcpFlush(pSkt);
        }
        return 0;
    }

    wActualLen = len >= wFreeTxSpace ? wFreeTxSpace : len;
    wFreeTxSpace -= wActualLen; // new free space

    // See if we need a two part put
    if(pSkt->txHead + wActualLen >= pSkt->txEnd)
    {
        wRightLen = (uint16_t)((uintptr_t)pSkt->txEnd-(uintptr_t)pSkt->txHead);
        (void)TCPIP_Helper_Memcpy((uint8_t*)pSkt->txHead, data, (size_t)wRightLen);
        data += wRightLen;
        wActualLen -= wRightLen;
        pSkt->txHead = pSkt->txStart;
    }

    (void)TCPIP_Helper_Memcpy((uint8_t*)pSkt->txHead, data, (size_t)wActualLen);
    pSkt->txHead += wActualLen;

    bool    toFlush = false;
    bool    toSetFlag = false;
    if(pSkt->txHead != pSkt->txUnackedTail)
    {   // something to send

        if(pSkt->flags.halfThresFlush != 0U)
        {
            if(pSkt->flags.bHalfFullFlush == 0U && wFreeTxSpace <=  (uint16_t)(((uintptr_t)pSkt->txEnd - (uintptr_t)pSkt->txStart) >> 1U) )
            {   // Send current payload if crossing the half buffer threshold
                // This improves performance with the delayed acknowledgement algorithm
                toFlush = true;
                toSetFlag = true;
            }
        }

        if(toFlush == false)
        {
            toFlush = F_TCPNeedSend(pSkt);
        }
    }

    if(toFlush)
    {
        (void) F_TcpFlush(pSkt);
        if(toSetFlag)
        {
            pSkt->flags.bHalfFullFlush = 1U;
        }
    }
    // If not already enabled, start a timer so this data will 
    // eventually get sent even if the application doens't call
    // TCPIP_TCP_Flush()
    else if(pSkt->flags.bTimer2Enabled == 0U)
    {
        pSkt->flags.bTimer2Enabled = 1U;
        uint32_t tmout = ((uint32_t)TCPIP_TCP_AUTO_TRANSMIT_TIMEOUT_VAL * SYS_TMR_TickCounterFrequencyGet()) / 1000U;
        pSkt->eventTime2 = SYS_TMR_TickCountGet() + tmout; 
    }
    else
    {
        /* Do Nothing */
    }

    return wActualLen + wRightLen;
}

static bool F_TCPNeedSend(TCB_STUB* pSkt)
{

    if(pSkt->txHead != pSkt->txUnackedTail)
    {   // something to send
        uint16_t toSendData, canSend;

        // check how much we can send
        if(pSkt->txHead > pSkt->txUnackedTail)
        {
            toSendData = (uint16_t)((uintptr_t)pSkt->txHead - (uintptr_t)pSkt->txUnackedTail);
        }
        else
        {
            toSendData = (uint16_t)(((uintptr_t)pSkt->txEnd - (uintptr_t)pSkt->txUnackedTail) + ((uintptr_t)pSkt->txHead - (uintptr_t)pSkt->txStart));
        }

        if(toSendData > pSkt->remoteWindow)
        {
            canSend = pSkt->remoteWindow;
        }
        else
        {
            canSend = toSendData;
        }

        if(canSend >= pSkt->wRemoteMSS || canSend >= (pSkt->maxRemoteWindow >> 1))
        {
            return true;
        }

        if(canSend == toSendData)
        {   // can send all there is
            if((pSkt->flags.forceFlush != 0U) || (pSkt->flags.delayAckSend == 0U && pSkt->txTail == pSkt->txUnackedTail))
            {   // either Nagle disabled or no unacknowledged data
                return true;
            }
        }
    }

    return false;
}



/*****************************************************************************
  Function:
    const uint8_t* TCPIP_TCP_StringPut(TCP_SOCKET hTCP, const uint8_t* data)

  Description:
    Writes a null-terminated string from RAM to a TCP socket.  The 
    null-terminator is not copied to the socket.

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket to which data is to be written.
    data - Pointer to the string to be written.

  Returns:
    Pointer to the byte following the last byte written to the socket.  If
    this pointer does not dereference to a NUL byte, the buffer became full
    or the socket is not connected.

  Remarks:
    The return value of this function differs from that of TCPIP_TCP_ArrayPut.  To
    write long strings in a single state, initialize the *data pointer to the
    first byte, then call this function repeatedly (breaking to the main 
    stack loop after each call) until the return value dereferences to a NUL
    byte.  Save the return value as the new starting *data pointer otherwise.
  ***************************************************************************/
const uint8_t* TCPIP_TCP_StringPut(TCP_SOCKET hTCP, const uint8_t* data)
{
    return data + TCPIP_TCP_ArrayPut(hTCP, data, (uint16_t)(strlen((const char*)data)));
}

/*****************************************************************************
  Function:
    uint16_t TCPIP_TCP_FifoTxFullGet(TCP_SOCKET hTCP)

  Description:
    Determines how many bytes are pending in the TCP TX FIFO.

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket to check.

  Returns:
    Number of bytes pending to be flushed in the TCP TX FIFO.
  ***************************************************************************/
uint16_t TCPIP_TCP_FifoTxFullGet(TCP_SOCKET hTCP)
{
    TCB_STUB* pSkt = F_TcpSocketChk(hTCP); 

    if(pSkt != NULL && F_TCP_TxPktValid(pSkt))
    {
        if((pSkt->smState == (uint8_t)TCPIP_TCP_STATE_ESTABLISHED) || (pSkt->smState == (uint8_t)TCPIP_TCP_STATE_CLOSE_WAIT))
        {   // check the socket is connected
            uint16_t wDataLen;
            uint16_t wFIFOSize;

            // Calculate total usable FIFO size
            wFIFOSize = (uint16_t)((uintptr_t)pSkt->txEnd - (uintptr_t)pSkt->txStart - 1U);

            // Find out how many data bytes are free in the TX FIFO
            wDataLen = F_TCPSocketTxFreeSize(pSkt);

            return wFIFOSize - wDataLen;
        }
    }

    return 0;
}

uint16_t TCPIP_TCP_FifoTxFreeGet(TCP_SOCKET hTCP)
{
    TCB_STUB* pSkt = F_TcpSocketChk(hTCP); 

    if(pSkt != NULL && F_TCP_TxPktValid(pSkt))
    {
        return F_TCPSocketTxFreeSize(pSkt);
    }

    return 0;
}



/****************************************************************************
  Section:
    Receive Functions
  ***************************************************************************/

/*****************************************************************************
  Function:
    uint16_t TCPIP_TCP_Discard(TCP_SOCKET hTCP)

  Description:
    Discards any pending data in the TCP RX FIFO.

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket whose RX FIFO is to be cleared.

  Returns:
    Number of bytes that have been discarded from the RX buffer.
  ***************************************************************************/
uint16_t TCPIP_TCP_Discard(TCP_SOCKET hTCP)
{
    uint16_t nBytes = 0;

    TCB_STUB* pSkt = F_TcpSocketChk(hTCP); 

    if(pSkt != NULL)
    {
        nBytes = F_TCPIsGetReady(pSkt);
        if(nBytes != 0U)
        {
            // Delete all data in the RX buffer
            pSkt->rxTail = pSkt->rxHead;
            (void) F_TCPSendWinIncUpdate(pSkt);
        }
    }

    return nBytes;
}

// checks if a Window increased update is needed
// returns true if it was issued, false otherwise
// tries to avoid the Silly Window Syndrome (SWS) on the RX side
// it will send the Win update if all RX buffer is available
static bool F_TCPSendWinIncUpdate(TCB_STUB* pSkt)
{
    uint16_t    oldWin, newWin, minWinInc, rxBuffSz;
    bool    toAdvertise = false;

    // previously advertised window
    oldWin = pSkt->localWindow;

    // new window is all the available RX buffer
    newWin = F_TCPGetRxFIFOFree(pSkt);
    rxBuffSz = (uint16_t)((uintptr_t)pSkt->rxEnd - (uintptr_t)pSkt->rxStart);

    if(newWin == rxBuffSz)
    {   // whole RX buffer available, advertise
        toAdvertise = true;
    }
    else if(newWin > oldWin)
    {   // should always be the case; just a sanity check
        minWinInc = rxBuffSz >> 1; // half RX buffer
        if(minWinInc > pSkt->localMSS)
        {
            minWinInc = pSkt->localMSS;    // minimum of MSS, 1/2 RX buffer
        }

        if((newWin - oldWin) > minWinInc)
        {   // Send a Window update message to the remote node
            toAdvertise = true;
        }
    }
    else
    {
        /* Do Nothing */
    }

    if(toAdvertise)
    {
        (void) F_TcpSend(pSkt, ACK, SENDTCP_RESET_TIMERS);
        return true;
    }

    return false;
}

/*****************************************************************************
  Function:
    void uint16_t TCPIP_TCP_GetIsReady(TCP_SOCKET hTCP)

  Summary:
    Determines how many bytes can be read from the TCP RX buffer.

  Description:
    Call this function to determine how many bytes can be read from the 
    TCP RX buffer.  If this function returns zero, the application must 
    return to the main stack loop before continuing in order to wait for
    more data to arrive.

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket to check.

  Returns:
    The number of bytes available to be read from the TCP RX buffer.
  ***************************************************************************/
uint16_t TCPIP_TCP_GetIsReady(TCP_SOCKET hTCP)
{
    TCB_STUB* pSkt = F_TcpSocketChk(hTCP); 
    
    if(pSkt != NULL)
    {
        return F_TCPIsGetReady(pSkt);
    }

    return 0;
}

static uint16_t F_TCPIsGetReady(TCB_STUB* pSkt)
{   
    if(pSkt->rxHead >= pSkt->rxTail)
    {
        return (uint16_t)((uintptr_t)pSkt->rxHead - (uintptr_t)pSkt->rxTail);
    }

    return (uint16_t)(((uintptr_t)pSkt->rxEnd - (uintptr_t)pSkt->rxTail + 1U) + ((uintptr_t)pSkt->rxHead - (uintptr_t)pSkt->rxStart));
}

uint16_t TCPIP_TCP_Get(TCP_SOCKET hTCP, uint8_t* data)
{
    return TCPIP_TCP_ArrayGet(hTCP, data, 1);
}


uint16_t TCPIP_TCP_ArrayGet(TCP_SOCKET hTCP, uint8_t* dataBuff, uint16_t len)
{
    uint16_t wGetReadyCount;
    uint16_t RightLen = 0;
    TCB_STUB* pSkt; 
    
    // See if there is any data which can be read
    if(len == 0U)
    {
        return 0;
    }
    if((pSkt= F_TcpSocketChk(hTCP)) == NULL)
    {
        return 0;
    }
    if((wGetReadyCount = F_TCPIsGetReady(pSkt)) == 0U)
    {
        return 0;
    }

    // Make sure we don't try to read more data than is available
    if(len > wGetReadyCount)
    {
        len = wGetReadyCount;
    }

    // See if we need a two part get
    if(pSkt->rxTail + len > pSkt->rxEnd)
    {
        RightLen = (uint16_t)((uintptr_t)pSkt->rxEnd - (uintptr_t)pSkt->rxTail + 1U);
        if(dataBuff != NULL)
        {
            (void) TCPIP_Helper_Memcpy(dataBuff, (uint8_t*)pSkt->rxTail, (size_t)RightLen);
            dataBuff += RightLen;
        }
        len -= RightLen;
        pSkt->rxTail = pSkt->rxStart;
    }

    if(dataBuff != NULL)
    {
        (void) TCPIP_Helper_Memcpy(dataBuff, (uint8_t*)pSkt->rxTail, (size_t)len);
    }
    pSkt->rxTail += len;
    len += RightLen;

    if(!F_TCPSendWinIncUpdate(pSkt))
    {   // not enough data freed to generate a window update
        if(wGetReadyCount - len <= len)
        {   // Send a window update if we've run low on data
            pSkt->flags.bTXASAPWithoutTimerReset = 1;
        }
        else if(pSkt->flags.bTimer2Enabled == 0U)
            // If not already enabled, start a timer so a window 
            // update will get sent to the remote node at some point
        {
            pSkt->flags.bTimer2Enabled = 1U;
            pSkt->eventTime2 = SYS_TMR_TickCountGet() + ((uint32_t)TCPIP_TCP_WINDOW_UPDATE_TIMEOUT_VAL * sysTickFreq)/1000U;
        }
        else
        {
            /* Do Nothing */
        }
    }

    return len;
}


/*****************************************************************************
  Function:
    uint16_t TCPIP_TCP_FifoRxFreeGet(TCP_SOCKET hTCP)

  Description:
    Determines how many bytes are free in the RX FIFO.

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket to check.

  Returns:
    The number of bytes free in the TCP RX FIFO.  If zero, no additional 
    data can be received until the application removes some data using one
    of the TCPIP_TCP_Get family functions.
  ***************************************************************************/
uint16_t TCPIP_TCP_FifoRxFreeGet(TCP_SOCKET hTCP)
{
    TCB_STUB* pSkt = F_TcpSocketChk(hTCP); 
    
    if(pSkt != NULL)
    {
        return F_TCPGetRxFIFOFree(pSkt);
    }

    return 0;
}


static uint16_t F_TCPGetRxFIFOFree(TCB_STUB* pSkt)
{

    uint16_t wDataLen;
    uint16_t wFIFOSize;

    // Calculate total usable FIFO size
    wFIFOSize = (uint16_t)((uintptr_t)pSkt->rxEnd - (uintptr_t)pSkt->rxStart);

    // Find out how many data bytes are actually in the RX FIFO
    wDataLen = F_TCPIsGetReady(pSkt);

    // Perform the calculation  
    return wFIFOSize - wDataLen;
}

/*****************************************************************************
  Function:
    uint16_t TCPIP_TCP_ArrayPeek(TCP_SOCKET hTCP, uint8_t *vBuffer, uint16_t wLen, uint16_t wStart)

  Summary:
    Reads a specified number of data bytes from the TCP RX FIFO without 
    removing them from the buffer.

  Description:
    Reads a specified number of data bytes from the TCP RX FIFO without 
    removing them from the buffer.  No TCP control actions are taken as a 
    result of this function (ex: no window update is sent to the remote node).
    
  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket to peak from (read without removing from stream).
    vBuffer - Destination to write the peeked data bytes.
    wLen - Length of bytes to peak from the RX FIFO and copy to vBuffer.
    wStart - Zero-indexed starting position within the FIFO to start peeking 
        from.

  Return Values:
    Number of bytes actually peeked from the stream and copied to vBuffer.  
    This value can be less than wLen if wStart + wLen is greater than the 
    deepest possible character in the RX FIFO.

  Remarks:
    None
  ***************************************************************************/
uint16_t TCPIP_TCP_ArrayPeek(TCP_SOCKET hTCP, uint8_t *vBuffer, uint16_t wLen, uint16_t wStart)
{
    uint8_t* ptrRead;
    uint16_t w;
    uint16_t wBytesUntilWrap;
    TCB_STUB* pSkt = F_TcpSocketChk(hTCP); 

    if(pSkt == NULL || wLen == 0U)
    {
        return 0;
    }

    // Find out how many bytes are in the RX FIFO and decrease read length 
    // if the start offset + read length is beyond the end of the FIFO
    w = F_TCPIsGetReady(pSkt);
    if(wStart + wLen > w)
    {
        wLen = w - wStart;
    }

    // Find the read start location
    ptrRead = pSkt->rxTail + wStart;
    if(ptrRead > pSkt->rxEnd)
    {
        ptrRead -= pSkt->rxEnd - pSkt->rxStart + 1;
    }

    // Calculate how many bytes can be read in a single go
    wBytesUntilWrap = (uint16_t)((uintptr_t)pSkt->rxEnd - (uintptr_t)ptrRead + 1U);
    if(wLen <= wBytesUntilWrap)
    {
        // Read all at once
        (void) TCPIP_Helper_Memcpy(vBuffer, ptrRead, (size_t)wLen);
    }
    else
    {
        // Read all bytes up to the wrap position and then read remaining bytes 
        // at the start of the buffer
        (void) TCPIP_Helper_Memcpy(vBuffer, ptrRead, (size_t)wBytesUntilWrap);
        (void)TCPIP_Helper_Memcpy(vBuffer + wBytesUntilWrap, (uint8_t*)pSkt->rxStart, (size_t)((uint32_t)wLen - (uint32_t)wBytesUntilWrap));
    }

    return wLen;
}

/*****************************************************************************
  Function:
    uint8_t TCPIP_TCP_Peek(TCP_SOCKET hTCP, uint16_t wStart)

  Summary:
    Peaks at one byte in the TCP RX FIFO without removing it from the buffer.

  Description:
    Peaks at one byte in the TCP RX FIFO without removing it from the buffer.
    
  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket to peak from (read without removing from stream).
    wStart - Zero-indexed starting position within the FIFO to peek from.

  Return Values:
    Byte peeked from the RX FIFO.  If there is no data in the buffer or an 
    illegal wStart starting offset is given, then an indeterminate value is 
    returned.  The caller must ensure that valid parameters are passed to avoid 
    (i.e ensure that TCPIP_TCP_GetIsReady() returns a number that is less than wStart 
    before calling TCPIP_TCP_Peek()).

  Remarks:
    Use the TCPIP_TCP_ArrayPeek() function to read more than one byte.  It will 
    perform better than calling TCPIP_TCP_Peek() in a loop.
  ***************************************************************************/
uint8_t TCPIP_TCP_Peek(TCP_SOCKET hTCP, uint16_t wStart)
{
    uint8_t i = 0;
    
    (void) TCPIP_TCP_ArrayPeek(hTCP, &i, 1, wStart);
    return i;
}


/****************************************************************************
  Section:
    Search Functions
  ***************************************************************************/

/*****************************************************************************
  Function:
    uint16_t TCPIP_TCP_ArrayFind(TCP_SOCKET hTCP, uint8_t* cFindArray, uint16_t wLen, 
                        uint16_t wStart, uint16_t wSearchLen, bool bTextCompare)

  Summary:
    Searches for a string in the TCP RX buffer.

  Description:
    This function finds the first occurrance of an array of bytes in the
    TCP RX buffer.  It can be used by an application to abstract searches 
    out of their own application code.  For increased efficiency, the 
    function is capable of limiting the scope of search to a specific
    range of bytes.  It can also perform a case-insensitive search if
    required.
    
    For example, if the buffer contains "I love PIC MCUs!" and the search
    array is "love" with a length of 4, a value of 2 will be returned.

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket to search within.
    cFindArray - The array of bytes to find in the buffer.
    wLen - Length of cFindArray.
    wStart - Zero-indexed starting position within the buffer.
    wSearchLen - Length from wStart to search in the buffer.
    bTextCompare - true for case-insensitive text search, false for binary search

  Return Values:
    0xFFFF - Search array not found
    Otherwise - Zero-indexed position of the first occurrance

  Remarks:
    The performance of this function degrades when the buffer is full
    and the array is not found.
    For better performance, try to search for characters that are expected to exist or
    limit the scope of the search as much as possible.  The HTTP module, 
    for example, uses this function to parse headers.  However, it searches 
    for newlines, then the separating colon, then reads the header name to 
    RAM for final comparison.  This has proven to be significantly faster  
    than searching for full header name strings outright.
  ***************************************************************************/
uint16_t TCPIP_TCP_ArrayFind(TCP_SOCKET hTCP, const uint8_t* cFindArray, uint16_t wLen, uint16_t wStart, uint16_t wSearchLen, bool bTextCompare)
{
    uint8_t* ptrRead;
    uint16_t wDataLen;
    uint16_t wBytesUntilWrap;
    uint8_t* ptrLocation;
    uint16_t wLenStart;
    const uint8_t *cFindArrayStart;
    uint8_t i, j, k;
    bool isFinding;
    uint8_t dataBuff[32] = {0};

    TCB_STUB* pSkt = F_TcpSocketChk(hTCP); 
    
    if(pSkt == NULL || wLen == 0U)
    {
        return 0xFFFF;
    }

    // Find out how many bytes are in the RX FIFO and return
    // immediately if we won't possibly find a match
    wDataLen = F_TCPIsGetReady(pSkt) - wStart;

    if(wDataLen < wLen)
    {
        return 0xFFFFu;
    }

    if((wSearchLen != 0U) && (wDataLen > wSearchLen))
    {
        wDataLen = wSearchLen;
    }

    ptrLocation = pSkt->rxTail + wStart;
    if(ptrLocation > pSkt->rxEnd)
    {
        ptrLocation -= pSkt->rxEnd - pSkt->rxStart + 1;
    }
    wBytesUntilWrap = (uint16_t)((uintptr_t)pSkt->rxEnd - (uintptr_t)ptrLocation + 1U);

    ptrRead = ptrLocation;
    wLenStart = wLen;
    cFindArrayStart = cFindArray;
    j = *cFindArray++;
    isFinding = false;
    if (bTextCompare)
    {
        if (j >= (uint8_t)'a' && j <= (uint8_t)'z')
        {
            j += (uint8_t)'A' - (uint8_t)'a';
        }
    }

    // Search for the array
    while (true)
    {
        // Figure out how big of a chunk to read
        k = (uint8_t)(sizeof (dataBuff));
        if (k > wBytesUntilWrap)
        {
            k = (uint8_t)(wBytesUntilWrap);
        }
        if ((uint16_t) k > wDataLen)
        {
            k = (uint8_t)wDataLen;
        }

        // Read a chunk of data into the buffer
        (void)TCPIP_Helper_Memcpy(dataBuff, ptrRead, (size_t)k);
        ptrRead += k;
        wBytesUntilWrap -= k;

        if (wBytesUntilWrap == 0u)
        {
            ptrRead = pSkt->rxStart;
            wBytesUntilWrap = 0xFFFFu;
        }

        // Convert everything to uppercase
        if (bTextCompare)
        {
            for (i = 0; i < k; i++)
            {
                if (dataBuff[i] >= (uint8_t)'a' && dataBuff[i] <= (uint8_t)'z')
                {
                    dataBuff[i] += (uint8_t)'A' - (uint8_t)'a';
                }

                if (j == dataBuff[i])
                {
                    if (--wLen == 0u)
                    {
                        return (uint16_t)(wStart - wLenStart + i + 1U);
                    }
                    j = *cFindArray++;
                    isFinding = true;
                    if (j >= (uint8_t)'a' && j <= (uint8_t)'z')
                    {
                        j += (uint8_t)'A' - (uint8_t)'a';
                    }
                }
                else
                {
                    wLen = wLenStart;
                    if (isFinding)
                    {
                        cFindArray = cFindArrayStart;
                        j = *cFindArray++;
                        if (j >= (uint8_t)'a' && j <= (uint8_t)'z') 
                        {
                            j += (uint8_t)'A' - (uint8_t)'a';
                        }
                        isFinding = false;
                    }
                }
            }
        }
        else // Compare as is
        {
            for (i = 0; i < k; i++)
            {
                if (j == dataBuff[i])
                {
                    if (--wLen == 0u)
                    {
                        return (uint16_t)(wStart - wLenStart + i + 1U);
                    }
                    j = *cFindArray++;
                    isFinding = true;
                }
                else
                {
                    wLen = wLenStart;
                    if (isFinding)
                    {
                        cFindArray = cFindArrayStart;
                        j = *cFindArray++;
                        isFinding = false;
                    }
                }
            }
        }

        // Check to see if it is impossible to find a match
        wDataLen -= k;
        if (wDataLen < wLen)
        {
            return 0xFFFFu;
        }

        wStart += k;
    }
}

/*****************************************************************************
  Function:
    uint16_t TCPIP_TCP_Find(TCP_SOCKET hTCP, uint8_t cFind,
                        uint16_t wStart, uint16_t wSearchLen, bool bTextCompare)

  Summary:
    Searches for a byte in the TCP RX buffer.

  Description:
    This function finds the first occurrance of a byte in the TCP RX
    buffer.  It can be used by an application to abstract searches 
    out of their own application code.  For increased efficiency, the 
    function is capable of limiting the scope of search to a specific
    range of bytes.  It can also perform a case-insensitive search if
    required.
    
    For example, if the buffer contains "I love PIC MCUs!" and the cFind
    byte is ' ', a value of 1 will be returned.

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP - The socket to search within.
    cFind - The byte to find in the buffer.
    wStart - Zero-indexed starting position within the buffer.
    wSearchLen - Length from wStart to search in the buffer.
    bTextCompare - true for case-insensitive text search, false for binary search

  Return Values:
    0xFFFF - Search array not found
    Otherwise - Zero-indexed position of the first occurrance

  Remarks:
    For best performance try to search for characters that are expected to exist or
    limit the scope of the search as much as possible.  The HTTP module, 
    for example, uses this function to parse headers.  However, it searches 
    for newlines, then the separating colon, then reads the header name to 
    RAM for final comparison.  This has proven to be significantly faster  
    than searching for full header name strings outright.
  ***************************************************************************/
uint16_t TCPIP_TCP_Find(TCP_SOCKET hTCP, uint8_t cFind, uint16_t wStart, uint16_t wSearchLen, bool bTextCompare)
{
    return TCPIP_TCP_ArrayFind(hTCP, &cFind, (uint16_t)(sizeof(cFind)), wStart, wSearchLen, bTextCompare);
}



/****************************************************************************
  Section:
    Data Processing Functions
  ***************************************************************************/

// Performs periodic TCP tasks.
static void TCPIP_TCP_Tick(void)
{
    TCP_SOCKET hTCP;
    bool bRetransmit;
    bool bCloseSocket;
    uint8_t vFlags;
    uint16_t w;
    TCB_STUB* pSkt; 

    // Periodically all "not closed" sockets must perform timed operations
    for(hTCP = 0; (uint32_t)hTCP < TcpSockets; hTCP++)
    {
        pSkt = TCBStubs[hTCP];
        if(pSkt != NULL && pSkt->smState != (uint8_t)TCPIP_TCP_STATE_CLIENT_WAIT_CONNECT)
        {   // existing socket
            vFlags = 0x00;
            bRetransmit = false;
            bCloseSocket = false;

            // Transmit ASAP data 
            if((pSkt->flags.bTXASAP != 0U) || (pSkt->flags.bTXASAPWithoutTimerReset != 0U))
            {
                vFlags = ACK;
                bRetransmit = (pSkt->flags.bTXASAPWithoutTimerReset != 0U);
            }

            // Perform any needed window updates and data transmissions
            if(pSkt->flags.bTimer2Enabled != 0U)
            {
                // See if the timeout has occured, and we need to send a new window update and pending data
                if(((int32_t)SYS_TMR_TickCountGet() - (int32_t)pSkt->eventTime2) >= 0)
                {
                    vFlags = ACK;
                }
            }

            // Process Delayed ACKnowledgement timer
            if(pSkt->flags.bDelayedACKTimerEnabled != 0U)
            {
                // See if the timeout has occured and delayed ACK needs to be sent
                if(((int32_t)SYS_TMR_TickCountGet() - (int32_t)pSkt->delayedACKTime) >= 0)
                {
                    vFlags = ACK;
                }
            }

#if  (TCPIP_TCP_CLOSE_WAIT_TIMEOUT != 0)
            // Process TCPIP_TCP_STATE_CLOSE_WAIT timer
            if(pSkt->smState == TCPIP_TCP_STATE_CLOSE_WAIT)
            {
                // Automatically close the socket on our end if the application 
                // fails to call TCPIP_TCP_Disconnect() is a reasonable amount of time.
                if((int32_t)(SYS_TMR_TickCountGet() - pSkt->closeWaitTime) >= 0)
                {
                    vFlags = FIN | ACK;
                    F_TcpSocketSetState(pSkt, TCPIP_TCP_STATE_LAST_ACK);
                }
            }
#endif  // (TCPIP_TCP_CLOSE_WAIT_TIMEOUT != 0)

            // Process FIN_WAIT2 timer
            if(pSkt->smState == (uint8_t)TCPIP_TCP_STATE_FIN_WAIT_2)
            {
                if(((int32_t)SYS_TMR_TickCountGet() - (int32_t)pSkt->closeWaitTime) >= 0)
                {   // the other side failed to close its connection within the TCPIP_TCP_FIN_WAIT_2_TIMEOUT
                    (void) F_TcpSend(pSkt, RST | ACK, SENDTCP_RESET_TIMERS);
#if (TCPIP_TCP_MSL_TIMEOUT != 0)
                    F_TcpSocketSetState(pSkt, TCPIP_TCP_STATE_TIME_WAIT);
                    pSkt->closeWaitTime = SYS_TMR_TickCountGet() + (((uint32_t)TCPIP_TCP_MSL_TIMEOUT * 2U) * sysTickFreq);
#else
                    F_TcpCloseSocket(pSkt, (TCPIP_TCP_SIGNAL_TYPE)0);
#endif  // (TCPIP_TCP_MSL_TIMEOUT != 0)
                    continue;
                }
            }

#if (TCPIP_TCP_MSL_TIMEOUT != 0)
            // Process 2MSL timer
            if(pSkt->smState == TCPIP_TCP_STATE_TIME_WAIT)
            {
                if((int32_t)(SYS_TMR_TickCountGet() - pSkt->closeWaitTime) >= 0)
                {   // timeout expired, close the socket
                    F_TcpCloseSocket(pSkt, (TCPIP_TCP_SIGNAL_TYPE)0);
                    continue;
                }
            }
#endif  // (TCPIP_TCP_MSL_TIMEOUT != 0)

            if(vFlags != 0U)
            {
                (void) F_TcpSend(pSkt, vFlags, bRetransmit ? 0U : SENDTCP_RESET_TIMERS);
            }

            // The TCPIP_TCP_STATE_LISTEN, and sometimes the TCPIP_TCP_STATE_ESTABLISHED 
            // state don't need any timeout events, so see if the timer is enabled
            if(pSkt->flags.bTimerEnabled == 0U)
            {
                if(pSkt->flags.keepAlive != 0U)
                {
                    // Only the established state has any use for keep-alives
                    if(pSkt->smState == (uint8_t)TCPIP_TCP_STATE_ESTABLISHED)
                    {
                        // If timeout has not occured, do not do anything.
                        if(((int32_t)SYS_TMR_TickCountGet() - (int32_t)pSkt->eventTime) < 0)
                        {
                            continue;
                        }

                        // If timeout has occured and the connection appears to be dead (no 
                        // responses from remote node at all), close the connection so the 
                        // application doesn't sit around indefinitely with a useless socket 
                        // that it thinks is still open
                        if(pSkt->keepAliveCount == pSkt->keepAliveLim)
                        {
                            vFlags = (uint8_t)pSkt->flags.bServer;

                            // Force an immediate FIN and RST transmission
                            // Also back in the listening state immediately if a server socket.
                            (void) F_TcpDisconnect(pSkt, true);
                            pSkt->flags.bServer = 1U;    // force client socket non-closing
                            F_TcpAbort(pSkt, TCP_ABORT_FLAG_REGULAR, TCPIP_TCP_SIGNAL_KEEP_ALIVE_TMO);

                            // Prevent client mode sockets from getting reused by other applications.  
                            // The application must call TCPIP_TCP_Disconnect()/TCPIP_TCP_Abort() with the handle to free this 
                            // socket (and the handle associated with it)
                            if(vFlags == 0U)
                            {
                                pSkt->flags.bServer = 0U;    // restore the client socket
                                F_TcpSocketSetState(pSkt, TCPIP_TCP_STATE_CLIENT_WAIT_DISCONNECT);
                            }

                            continue;
                        }

                        // Otherwise, if a timeout occured, simply send a keep-alive packet
                        (void) F_TcpSend(pSkt, ACK, SENDTCP_KEEP_ALIVE);
                        pSkt->eventTime = SYS_TMR_TickCountGet() + (pSkt->keepAliveTmo * sysTickFreq) / 1000U;
                    }
                }
                continue;
            }

            // If timeout has not occured, do not do anything.
            if(((int32_t)SYS_TMR_TickCountGet() - (int32_t)pSkt->eventTime) < 0 )
            {
                continue;
            }
            
            // A timeout has occured.  Respond to this timeout condition
            // depending on what state this socket is in.
            switch(pSkt->smState)
            {
                case (uint8_t)TCPIP_TCP_STATE_SYN_SENT:
                    // Keep sending SYN until we hear from remote node.
                    // This may be for infinite time, in that case
                    // caller must detect it and do something.
                    vFlags = SYN;
                    bRetransmit = true;

                    // Exponentially increase timeout until we reach TCPIP_TCP_MAX_RETRIES attempts then stay constant
                    if(pSkt->retryCount >= ((uint8_t)TCPIP_TCP_MAX_RETRIES - 1U))
                    {
                        pSkt->retryCount = (uint8_t)TCPIP_TCP_MAX_RETRIES - 1U;
                        pSkt->retryInterval = (((uint32_t)TCPIP_TCP_START_TIMEOUT_VAL * sysTickFreq) / 1000U) << ((uint8_t)TCPIP_TCP_MAX_RETRIES - 1U);
                    }
                    break;

                case (uint8_t)TCPIP_TCP_STATE_SYN_RECEIVED:
                    // We must receive ACK before timeout expires.
                    // If not, resend SYN+ACK.
                    // Abort, if maximum attempts counts are reached.
                    if(pSkt->retryCount < (uint8_t)TCPIP_TCP_MAX_SYN_RETRIES)
                    {
                        vFlags = SYN | ACK;
                        bRetransmit = true;
                    }
                    else
                    {
                        if(pSkt->flags.bServer != 0U)
                        {
                            vFlags = RST | ACK;
                            bCloseSocket = true;
                        }
                        else
                        {
                            vFlags = SYN;
                        }
                    }
                    break;

                case (uint8_t)TCPIP_TCP_STATE_ESTABLISHED:
                    // Retransmit any unacknowledged data
                    if(pSkt->retryCount < (uint8_t)TCPIP_TCP_MAX_RETRIES)
                    {
                        vFlags = ACK;
                        bRetransmit = true;
                    }
                    else
                    {   // No response back for too long, close connection
                        // This could happen, for instance, if the communication 
                        // medium was lost
                        F_TcpSocketSetState(pSkt, TCPIP_TCP_STATE_FIN_WAIT_1);
                        vFlags = FIN | ACK;
                    }
                    break;

                case (uint8_t)TCPIP_TCP_STATE_FIN_WAIT_1:
                    if(pSkt->retryCount < (uint8_t)TCPIP_TCP_MAX_RETRIES)
                    {
                        // Send another FIN
                        vFlags = FIN | ACK;
                        bRetransmit = true;
                    }
                    else
                    {   // Close on our own, we can't seem to communicate 
                        // with the remote node anymore
                        vFlags = RST | ACK;
#if (TCPIP_TCP_MSL_TIMEOUT != 0)
                        F_TcpSocketSetState(pSkt, TCPIP_TCP_STATE_TIME_WAIT);
#else
                        bCloseSocket = true;
#endif  // (TCPIP_TCP_MSL_TIMEOUT != 0)
                    }
                    break;

                case (uint8_t)TCPIP_TCP_STATE_CLOSING:
                    if(pSkt->retryCount < (uint8_t)TCPIP_TCP_MAX_RETRIES)
                    {
                        // Send another ACK+FIN (the FIN is retransmitted 
                        // automatically since it hasn't been acknowledged by 
                        // the remote node yet)
                        vFlags = ACK;
                        bRetransmit = true;
                    }
                    else
                    {   // Close on our own, we can't seem to communicate 
                        // with the remote node anymore
                        vFlags = RST | ACK;
#if (TCPIP_TCP_MSL_TIMEOUT != 0)
                        F_TcpSocketSetState(pSkt, TCPIP_TCP_STATE_TIME_WAIT);
#else
                        bCloseSocket = true;
#endif  // (TCPIP_TCP_MSL_TIMEOUT != 0)
                    }
                    break;


                case (uint8_t)TCPIP_TCP_STATE_LAST_ACK:
                    // Send some more FINs or close anyway
                    if(pSkt->retryCount < (uint8_t)TCPIP_TCP_MAX_RETRIES)
                    {
                        vFlags = FIN | ACK;
                        bRetransmit = true;
                    }
                    else
                    {
                        vFlags = RST | ACK;
                        bCloseSocket = true;
                    }
                    break;

                default:    // case TCPIP_TCP_STATE_TIME_WAIT:
                    /* Do Nothing */
                    break;
            }

            if(vFlags != 0U)
            {
                // Transmit all unacknowledged data over again
                if(bRetransmit)
                {
                    // Set the appropriate retry time
                    pSkt->retryCount++;
                    pSkt->retryInterval <<= 1;

                    // Calculate how many bytes we have to roll back and retransmit
                    w = (uint16_t)((uintptr_t)pSkt->txUnackedTail - (uintptr_t)pSkt->txTail);
                    if(pSkt->txUnackedTail < pSkt->txTail)
                    {
                        w += (uint16_t)((uintptr_t)pSkt->txEnd - (uintptr_t)pSkt->txStart);
                    }

                    // Perform roll back of local SEQuence counter, remote window 
                    // adjustment, and cause all unacknowledged data to be 
                    // retransmitted by moving the unacked tail pointer.
                    pSkt->MySEQ -= w;
                    pSkt->remoteWindow += w;
                    pSkt->txUnackedTail = pSkt->txTail;     
                    (void) F_TcpSend(pSkt, vFlags, 0U);
                }
                else
                {
                    (void) F_TcpSend(pSkt, vFlags, SENDTCP_RESET_TIMERS);
                }

            }

            if(bCloseSocket)
            {
                F_TcpCloseSocket(pSkt, (TCPIP_TCP_SIGNAL_TYPE)0);
            }
        }
    }

}


#if defined (TCPIP_STACK_USE_IPV6)
static TCPIP_MAC_PKT_ACK_RES TCPIP_TCP_ProcessIPv6(TCPIP_MAC_PACKET* pRxPkt)
{
    TCP_HEADER      *pTCPHdr;
    uint16_t        dataLen;
    uint8_t         optionsSize;
    const IPV6_ADDR*    localIP;
    const IPV6_ADDR*    remoteIP;
    TCB_STUB*       pSkt; 
    TCPIP_TCP_SIGNAL_FUNCTION sigHandler;
    const void*         sigParam;
    uint16_t            sigMask;
    TCPIP_MAC_PKT_ACK_RES ackRes;
    uint32_t            sktEvent = 0;

    if(!TCPIP_IPV6_AddressesGet(pRxPkt, &localIP, &remoteIP))
    {
        return TCPIP_MAC_PKT_ACK_STRUCT_ERR;
    }

    dataLen = pRxPkt->totTransportLen;

    pTCPHdr = FC_U8Ptr2TcpHdr(pRxPkt->pTransportLayer);

    if((pRxPkt->pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_RX_CHKSUM_TCP) == 0U)
    {
        // Calculate IP pseudoheader checksum.
        uint16_t        calcChkSum;
        IPV6_PSEUDO_HEADER   pseudoHeader;

        (void)memcpy (&pseudoHeader.SourceAddress, remoteIP, sizeof (IPV6_ADDR));
        (void)memcpy (&pseudoHeader.DestAddress, localIP, sizeof (IPV6_ADDR));
        // Total payload length is the length of data + extension headers
        pseudoHeader.PacketLength = TCPIP_Helper_htons(dataLen);
        pseudoHeader.zero1 = 0;
        pseudoHeader.zero2 = 0;
        pseudoHeader.NextHeader = (uint8_t)IP_PROT_TCP;

        // Note: if((pRxPkt->pktFlags & (uint32_t)TCPIP_MAC_PKT_FLAG_SPLIT) != 0) not supported for now!


        calcChkSum = ~TCPIP_Helper_CalcIPChecksum((uint8_t*)&pseudoHeader, (uint16_t)(sizeof(pseudoHeader)), 0);
        calcChkSum = TCPIP_Helper_CalcIPChecksum((uint8_t*)pTCPHdr, dataLen, calcChkSum);
        if(calcChkSum != 0U)
        {   // discard packet
            return TCPIP_MAC_PKT_ACK_CHKSUM_ERR;
        }
    }

    F_TcpSwapHeader(pTCPHdr);

    // Skip over options to retrieve data bytes
    optionsSize = ((pTCPHdr->DataOffset.Val << 2) - (uint8_t)sizeof(*pTCPHdr));

    while(true)
    {
        // Find matching socket.
        pSkt = F_TcpFindMatchingSocket(pRxPkt, remoteIP, localIP, IP_ADDRESS_TYPE_IPV6);
        if(pSkt == NULL)
        {   // Send ICMP Destination Unreachable Code 4 (Port unreachable) and discard packet
            uint16_t headerLen = pRxPkt->ipv6PktData;
            TCPIP_IPV6_ErrorSend ((const TCPIP_NET_IF*)pRxPkt->pktIf, pRxPkt, localIP, remoteIP, (uint8_t)ICMPV6_ERR_DU_PORT_UNREACHABLE, (uint8_t)ICMPV6_ERROR_DEST_UNREACHABLE, 0x00000000, (dataLen + headerLen + (uint16_t)sizeof (IPV6_HEADER)));
            ackRes = TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR;
            break;
        }


        // extract header
        pRxPkt->pDSeg->segLen -=  (uint16_t)(optionsSize + sizeof(*pTCPHdr));    
        F_TcpHandleSeg(pSkt, pTCPHdr, (dataLen - optionsSize - (uint16_t)sizeof(*pTCPHdr)), pRxPkt, &sktEvent);

        sigMask = F_TcpSktGetSignalLocked(pSkt, &sigHandler, &sigParam);
        sktEvent &= (uint32_t)sigMask;
        if(sktEvent != 0U)
        {
            if(sigHandler != NULL)
            {
                (*sigHandler)(pSkt->sktIx, pRxPkt->pktIf, (TCPIP_TCP_SIGNAL_TYPE)sktEvent, sigParam);
            }
        }

        // OK, pass to user
        ackRes = TCPIP_MAC_PKT_ACK_RX_OK;
        break;
    }

    // log 
#if (TCPIP_PACKET_LOG_ENABLE)
    uint32_t logPort = (pSkt != NULL) ? ((uint32_t)pSkt->localPort << 16) | pSkt->remotePort : ((uint32_t)pTCPHdr->DestPort << 16) | pTCPHdr->SourcePort;
    TCPIP_PKT_FlightLogRxSkt(pRxPkt, TCPIP_MODULE_LAYER3, logPort, pSkt != NULL ? (uint16_t)pSkt->sktIx: (uint16_t)0xffffU);
#endif  // (TCPIP_PACKET_LOG_ENABLE)

    return ackRes;
}
#endif  // defined (TCPIP_STACK_USE_IPV6)

/*****************************************************************************
  Function:
    static bool F_TcpSend(pSkt, uint8_t vTCPFlags, uint8_t vSendFlags)

  Summary:
    Transmits a TPC segment.

  Description:
    This function assembles and transmits a TCP segment, including any 
    pending data.  It also supports retransmissions, keep-alives, and 
    other packet types.

  Precondition:
    TCP is initialized.

  Parameters:
    vTCPFlags - Additional TCP flags to include
    vSendFlags - Any combinations of SENDTCP_* constants to modify the
                 transmit behavior or contents.

  Returns:
    TCP_SEND_OK for success, a TCP_SEND_RES code  < 0 otherwise
  ***************************************************************************/
static TCP_SEND_RES F_TcpSend(TCB_STUB* pSkt, uint8_t vTCPFlags, uint8_t vSendFlags)
{
    TCP_OPTIONS     options;
    uint32_t        len, lenStart, lenEnd;
    uint16_t        loadLen, hdrLen, maxPayload;
    void*           pSendPkt;
    uint16_t        mss = 0;
    TCP_HEADER *    header = NULL;
    TCPIP_TCP_SIGNAL_FUNCTION sigHandler;
    const void*         sigParam;
    uint16_t            sigMask;
    TCP_SEND_RES sendRes;

#if (TCPIP_TCP_QUIET_TIME != 0)
    if(!tcpQuietDone)
    {
        return TCP_SEND_QUIET;
    }
#endif  // (TCPIP_TCP_QUIET_TIME != 0)

    //  Make sure that we have an allocated TX packet
    switch(pSkt->addType)
    {
#if defined (TCPIP_STACK_USE_IPV6)
        case (uint8_t)IP_ADDRESS_TYPE_IPV6:
            pSendPkt = F_Tcpv6AllocateTxPacketIfQueued(pSkt, true);
            if(pSendPkt == NULL)
            {   // cannot allocate packet
                sendRes = TCP_SEND_NO_MEMORY;
            }
            break;
#endif  // defined (TCPIP_STACK_USE_IPV6)

#if defined (TCPIP_STACK_USE_IPV4)
        case (uint8_t)IP_ADDRESS_TYPE_IPV4:
            pSendPkt = F_Tcpv4AllocateTxPacketIfQueued(pSkt, true);
            if(pSendPkt == NULL)
            {   // cannot allocate packet
                sendRes = TCP_SEND_NO_MEMORY;
            }
            break;
#endif  // defined (TCPIP_STACK_USE_IPV4)

        default:
            // cannot send with no address specified
            pSendPkt = NULL;
            sendRes = TCP_SEND_NO_PKT; 
            break;
    }

    if(pSendPkt == NULL)
    {   // cannot allocate packet
        return sendRes;
    }

    // packet was allocated/marked for TX; try to send it
    while(true)
    {
        // FINs must be handled specially
        if((vTCPFlags & FIN) != 0U)
        {
            pSkt->flags.bTXFIN = 1;
            vTCPFlags &= ~(uint8_t)FIN;
        }

        // Status will now be synched, disable automatic future 
        // status transmissions
        pSkt->flags.bTimer2Enabled = 0;
        pSkt->flags.bDelayedACKTimerEnabled = 0;
        pSkt->flags.bOneSegmentReceived = 0;
        pSkt->flags.bTXASAP = 0;
        pSkt->flags.bTXASAPWithoutTimerReset = 0;
        pSkt->flags.bHalfFullFlush = 0;

#if defined (TCPIP_STACK_USE_IPV6)
        if(pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
        {
            header = TCPIP_IPV6_UpperLayerHeaderPtrGet((IPV6_PACKET*)pSendPkt);
            if(header == NULL)
            {   // something was wrong
                sendRes = TCP_SEND_NO_PKT;
                break;
            }
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)

#if defined (TCPIP_STACK_USE_IPV4)
        if(pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV4)
        {
            header = FC_U8Ptr2TcpHdr(((TCP_V4_PACKET*)pSendPkt)->v4Pkt.macPkt.pTransportLayer);
        }
#endif  // defined (TCPIP_STACK_USE_IPV4)

        header->DataOffset.Val = 0;

        // Put all socket application data in the TX space
        if((vTCPFlags & (SYN | RST)) != 0U)
        {
            // Don't put any data in SYN and RST messages
            len = 0;

            // Insert the MSS (Maximum Segment Size) TCP option if this is SYN packet
            if((vTCPFlags & SYN) != 0U)
            {
                options.Kind = TCP_OPTIONS_MAX_SEG_SIZE;
                options.Length = 0x04;

                // Load MSS and swap to big endian
#if defined (TCPIP_STACK_USE_IPV6)
                if(pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
                {
                    mss = ((uint16_t)TCPIP_IPV6_MaxDatagramDataSizeGet(pSkt->pSktNet) - (uint16_t)sizeof(TCP_HEADER)); 
                }
#endif  // defined (TCPIP_STACK_USE_IPV6)

#if defined (TCPIP_STACK_USE_IPV4)
                if(pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV4)
                {
                    if(pSkt->pSktNet == NULL)
                    {   // client socket at 1st connect
                        if(!F_TcpSocketSetSourceInterface(pSkt))
                        {   // cannot find an route?
                            sendRes = TCP_SEND_NO_IF;
                            break;
                        }
                    }

                    mss = ((uint16_t)TCPIP_IPV4_MaxDatagramDataSizeGet(pSkt->pSktNet) - (uint16_t)sizeof(TCP_HEADER));
                }
#endif  // defined (TCPIP_STACK_USE_IPV4)

                options.MaxSegSize.Val = (((mss)&0x00FFU)<<8U) | (((mss)&0xFF00U)>>8U);
                pSkt->localMSS = mss;

                uint8_t optVal =  (uint8_t)sizeof(options) >> 2;
                optVal += header->DataOffset.Val;
                header->DataOffset.Val = optVal;

#if defined (TCPIP_STACK_USE_IPV6)
                if(pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
                {
                    if (TCPIP_IPV6_TxIsPutReady((IPV6_PACKET*)pSendPkt, (uint16_t)sizeof (options)) < sizeof (options))
                    {
                        sendRes = TCP_SEND_NO_MEMORY;
                        break;
                    }
                    (void)TCPIP_IPV6_PutArray((IPV6_PACKET*)pSendPkt, (uint8_t*)&options, (uint16_t)sizeof(options));
                }
#endif  // defined (TCPIP_STACK_USE_IPV6)

#if defined (TCPIP_STACK_USE_IPV4)
                if(pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV4)
                {
                    (void) memcpy((uint8_t*)(header + 1), (uint8_t*)&options, sizeof(options));
                }
#endif  // defined (TCPIP_STACK_USE_IPV4)

                if(pSkt->MySEQ == 0U)
                {   // Set Initial Sequence Number (ISN)
                    pSkt->MySEQ = F_TCP_SktSetSequenceNo(pSkt);
                }
            }
        }
        else
        {
            // Begin copying any application data over to the TX space
            maxPayload = pSkt->wRemoteMSS;
            if(pSkt->txHead == pSkt->txUnackedTail || pSkt->remoteWindow == 0U)
            {   // either all caught up on data TX or cannot send anything
                len = 0;
            }
            else
            {   // can transmit something
                bool isFragmSupported = false;

#if defined (TCPIP_STACK_USE_IPV4)
                if(pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV4)
                {
                    isFragmSupported = TCPIP_IPV4_IsFragmentationEnabled();
                }
#endif  // defined (TCPIP_STACK_USE_IPV4)
#if defined (TCPIP_STACK_USE_IPV6)
                if(pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
                {
                    isFragmSupported = TCPIP_IPV6_IsFragmentationEnabled();
                }
#endif  // defined (TCPIP_STACK_USE_IPV6)

                if(!isFragmSupported)
                {
                    if(pSkt->localMSS < maxPayload)
                    {   // don't go over the interface MTU if cannot fragment
                        maxPayload = pSkt->localMSS;
                    }
                }

                if(pSkt->txHead > pSkt->txUnackedTail)
                {
                    len = (uint32_t)((uintptr_t)pSkt->txHead - (uintptr_t)pSkt->txUnackedTail);
                    if(len > pSkt->remoteWindow)
                    {
                        len = pSkt->remoteWindow;
                    }

                    if(len > maxPayload)
                    {
                        len = maxPayload;
                        pSkt->flags.bTXASAPWithoutTimerReset = 1;
                    }

                    // link application data into the TX packet
                    F_TCP_PayloadSet(pSkt, pSendPkt, pSkt->txUnackedTail, (uint16_t)len, NULL, 0);
                    pSkt->txUnackedTail += len;
                }
                else
                {
                    lenEnd = (uint32_t)((uintptr_t)pSkt->txEnd - (uintptr_t)pSkt->txUnackedTail);
                    len = lenEnd + (uint32_t)((uintptr_t)pSkt->txHead - (uintptr_t)pSkt->txStart);

                    if(len > pSkt->remoteWindow)
                    {
                        len = pSkt->remoteWindow;
                    }

                    if(len > maxPayload)
                    {
                        len = maxPayload;
                        pSkt->flags.bTXASAPWithoutTimerReset = 1;
                    }

                    if (lenEnd > len)
                    {
                        lenEnd = len;
                    }
                    lenStart = len - lenEnd;

                    // link application data into the TX packet
                    if(lenStart != 0U)
                    {
                        F_TCP_PayloadSet(pSkt, pSendPkt, pSkt->txUnackedTail, (uint16_t)lenEnd, pSkt->txStart, (uint16_t)lenStart);
                    }
                    else
                    {
                        F_TCP_PayloadSet(pSkt, pSendPkt, pSkt->txUnackedTail, (uint16_t)lenEnd, NULL, 0);
                    }

                    pSkt->txUnackedTail += len;
                    if(pSkt->txUnackedTail >= pSkt->txEnd)
                    {
                        pSkt->txUnackedTail -= pSkt->txEnd-pSkt->txStart;
                    }
                }
            }

            // If we are to transmit a FIN, make sure we can put one in this packet
            if(pSkt->flags.bTXFIN != 0U)
            {
                if((len != pSkt->remoteWindow) && (len != maxPayload))
                {
                    vTCPFlags |= FIN;
                }
            }
        }

    loadLen = (uint16_t)len;  // save the TCP payload size

        // Ensure that all packets with data of some kind are 
        // retransmitted by TCPIP_TCP_Tick() until acknowledged
        // Pure ACK packets with no data are not ACKed back in TCP
        if((len != 0U) || ((vTCPFlags & (SYN | FIN)) != 0U))
        {
            // Transmitting data, update remote window variable to reflect smaller 
            // window.
            pSkt->remoteWindow -= (uint16_t)len;

            // Push (PSH) all data for enhanced responsiveness on 
            // the remote end, especially with GUIs
            if(len != 0U)
            {
                vTCPFlags |= PSH;
            }

            if((vSendFlags & SENDTCP_RESET_TIMERS) != 0U)
            {
                pSkt->retryCount = 0;
                pSkt->retryInterval = ((uint32_t)TCPIP_TCP_START_TIMEOUT_VAL * sysTickFreq) / 1000U;
            }   

            pSkt->eventTime = SYS_TMR_TickCountGet() + pSkt->retryInterval;
            pSkt->flags.bTimerEnabled = 1;
        }
        else if((vSendFlags & SENDTCP_KEEP_ALIVE) != 0U)
        {
            // Increment Keep Alive TX counter to handle disconnection if not response is returned
            pSkt->keepAliveCount++;

            // Generate a dummy byte
            pSkt->MySEQ -= 1U;
            len = 1;
        }
        else if(pSkt->flags.bTimerEnabled != 0U) 
        {
            // If we have data to transmit, but the remote RX window is zero, 
            // so we aren't transmitting any right now then make sure to not 
            // extend the retry counter or timer.  This will stall our TX 
            // with a periodic ACK sent to the remote node.
            if((vSendFlags & SENDTCP_RESET_TIMERS) == 0U)
            {
                // Roll back retry counters since we can't send anything, 
                // but only if we incremented it in the first place
                if(pSkt->retryCount != 0U)
                {
                    pSkt->retryCount--;
                    pSkt->retryInterval >>= 1;
                }
            }

            pSkt->eventTime = SYS_TMR_TickCountGet() + pSkt->retryInterval;
        }
        else
        {
            /* Do Nothing */
        }

        header->SourcePort          = pSkt->localPort;
        header->DestPort            = pSkt->remotePort;
        header->SeqNumber           = pSkt->MySEQ;
        header->AckNumber           = pSkt->RemoteSEQ;
        header->Flags.bits.Reserved2    = 0;
        header->DataOffset.Reserved3    = 0;
        header->Flags.byteVal       = vTCPFlags;
        header->UrgentPointer       = 0;
        header->Checksum            = 0;

        // Update our send sequence number and ensure retransmissions 
        // of SYNs and FINs use the right sequence number
        pSkt->MySEQ += (uint32_t)len;
        if((vTCPFlags & SYN) != 0U)
        {
            hdrLen = (uint16_t)(sizeof(options));

            // SEG.ACK needs to be zero for the first SYN packet for compatibility 
            // with certain paranoid TCP/IP stacks, even though the ACK flag isn't 
            // set (indicating that the AckNumber field is unused).
            if((vTCPFlags & ACK) == 0U)
            {
                header->AckNumber = 0;
            }

            if(pSkt->flags.bSYNSent != 0U)
            {
                header->SeqNumber--;
            }
            else
            {
                pSkt->MySEQ++;
                pSkt->flags.bSYNSent = 1;
            }
        }
        else
        {
            hdrLen = 0;
        }

        if((vTCPFlags & FIN) != 0U)
        {
            pSkt->flags.bFINSent = 1;   // do not advance the seq no for FIN!
        }

        if((vTCPFlags & ACK) != 0U)
        {
            pSkt->flags.ackSent = 1;   // store the ACK already sent
        }

        // Calculate the amount of free space in the RX buffer area of this socket
        if(pSkt->rxHead >= pSkt->rxTail)
        {
            header->Window = (uint16_t)(((uintptr_t)pSkt->rxEnd - (uintptr_t)pSkt->rxStart) - ((uintptr_t)pSkt->rxHead - (uintptr_t)pSkt->rxTail));
        }
        else
        {
            header->Window = (uint16_t)((uintptr_t)pSkt->rxTail - (uintptr_t)pSkt->rxHead - 1U);
        }
        pSkt->localWindow = header->Window; // store the last advertised window

        F_TcpSwapHeader(header);


        hdrLen += (uint16_t)(sizeof(TCP_HEADER));
        uint8_t optVal =  (uint8_t)sizeof(TCP_HEADER) >> 2;
        optVal += header->DataOffset.Val;
        header->DataOffset.Val = optVal;

#if defined (TCPIP_STACK_USE_IPV6)
        if(pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
        {   // Write IP header
            TCPIP_IPV6_HeaderPut((IPV6_PACKET*)pSendPkt, (uint8_t)IP_PROT_TCP, pSkt->dscp);
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)

        // transmit the packet over the network
        sendRes = F_TCP_Flush (pSkt, pSendPkt, hdrLen, loadLen) ? TCP_SEND_OK : TCP_SEND_IP_FAIL;
        if(loadLen != 0U && pSkt->retxTime == 0U)
        {   // sending some payload
            F_TCP_LoadRetxTmo(pSkt, true);
        }
        break;
    }

#if defined (TCPIP_STACK_USE_IPV4)
    if(sendRes != TCP_SEND_OK && pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV4)
    {   // release the packet marked for TX
        ((TCP_V4_PACKET*)pSendPkt)->v4Pkt.macPkt.pktFlags &= ~((uint32_t)TCPIP_MAC_PKT_FLAG_QUEUED) ;
    }
#endif  // defined (TCPIP_STACK_USE_IPV4)

    if(sendRes == TCP_SEND_OK && (vTCPFlags & RST) != 0U )
    {   // signal that we reset the connection
        sigMask = F_TcpSktGetSignalLocked(pSkt, &sigHandler, &sigParam);
        if(sigHandler != NULL && (sigMask & (uint16_t)TCPIP_TCP_SIGNAL_TX_RST) != 0U)
        {
            (*sigHandler)(pSkt->sktIx, pSkt->pSktNet, TCPIP_TCP_SIGNAL_TX_RST, sigParam);
        } 
    }

    return sendRes;
}

/*****************************************************************************
  Function:
    static TCB_STUB* F_TcpFindMatchingSocket(TCPIP_MAC_PACKET* pRxPkt, void * remoteIP, void * localIP, IP_ADDRESS_TYPE addressType)

  Summary:
    Finds a suitable socket for a TCP segment.

  Description:
    This function searches through the sockets and attempts to match one with
    a given TCP header.
    If a socket is found, a valid socket pointer it is returned. 
    Otherwise, a 0 pointer is returned.
    
  Precondition:
    TCP is initialized.

  Parameters:
    pRxPkt - TCP packet
    remote - The remote node who sent this header
    localIP - The IP address of the interface that received this packet
    addressTpe  - IPv4/IPv6

  Return Values:
    a socket pointer - A match was found 
    0 - No suitable socket was found 
  ***************************************************************************/
static TCB_STUB* F_TcpFindMatchingSocket(TCPIP_MAC_PACKET* pRxPkt, const void * remoteIP, const void * localIP, IP_ADDRESS_TYPE addressType)
{
    TCP_SOCKET hTCP;
    uint16_t hash;
    TCB_STUB* pSkt, *partialSkt;
    const TCPIP_NET_IF* pPktIf;

    TCP_HEADER* h = FC_U8Ptr2TcpHdr(pRxPkt->pTransportLayer);
    pPktIf = (const TCPIP_NET_IF*)pRxPkt->pktIf;

    // Prevent connections on invalid port 0
    if(h->DestPort == 0U)
    {
        return NULL;
    }

    partialSkt = NULL;

    bool addrMatch = false;
#if defined TCPIP_STACK_USE_IPV6
    if(addressType == IP_ADDRESS_TYPE_IPV6)
    {
        hash = TCPIP_IPV6_GetHash(remoteIP, h->SourcePort, h->DestPort);
        addrMatch = true;
    }
#endif  // defined (TCPIP_STACK_USE_IPV6)

#if defined (TCPIP_STACK_USE_IPV4)
    if(addressType == IP_ADDRESS_TYPE_IPV4)
    {
        hash = (((const IPV4_ADDR *)remoteIP)->w[1] + ((const IPV4_ADDR *)remoteIP)->w[0] + h->SourcePort) ^ h->DestPort;
        addrMatch = true;
    }
#endif  // defined (TCPIP_STACK_USE_IPV4)

    if(addrMatch == false)
    {   // should not happen
        return NULL;
    }

    // Loop through all sockets looking for a socket that is expecting this 
    // packet or can handle it.
    for(hTCP = 0; (uint32_t)hTCP < TcpSockets; hTCP++)
    {
        pSkt = TCBStubs[hTCP];

        if(pSkt == NULL || pSkt->smState == (uint8_t)TCPIP_TCP_STATE_CLIENT_WAIT_CONNECT)
        {
            continue;
        }

        if( (pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_ANY || pSkt->addType == (uint8_t)addressType) &&
                (pSkt->pSktNet == NULL || pSkt->pSktNet == pPktIf) )
        {   // both network interface and address type match

            bool found = false;

            if(pSkt->smState == (uint8_t)TCPIP_TCP_STATE_LISTEN)
            {
                // For listening ports, check if this is the correct port
                if(pSkt->remoteHash == h->DestPort && partialSkt == NULL)
                {
                    partialSkt = pSkt;
                }
                continue;
            }
            else if(pSkt->remoteHash != hash)
            {// Ignore if the hash doesn't match
                continue;
            }
            else
            {
                /* Possible match */
            }

            while(  h->DestPort == pSkt->localPort && h->SourcePort == pSkt->remotePort )  
            {

#if defined (TCPIP_STACK_USE_IPV6)
                if (addressType == IP_ADDRESS_TYPE_IPV6)
                {
                    const IPV6_ADDR* pDstAddr = TCPIP_IPV6_DestAddressGet(pSkt->pV6Pkt);
                    if (memcmp (pDstAddr->v, FC_Cvptr2Uptr(remoteIP), sizeof (IPV6_ADDR)) == 0)
                    {
                        found = true;
                    }
                    break;
                }
#if defined (TCPIP_STACK_USE_IPV4)
                else
#endif  // defined (TCPIP_STACK_USE_IPV4)
#endif  // defined (TCPIP_STACK_USE_IPV6)

#if defined (TCPIP_STACK_USE_IPV4)
                {
                    if (pSkt->destAddress.Val == ((const IPV4_ADDR *)remoteIP)->Val)
                    {
                        found = true;
                    }
                }
#endif  // defined (TCPIP_STACK_USE_IPV4)

                break;
            }

            if(found)
            { 
                pSkt->addType = (uint8_t)addressType;
                F_TcpSocketBind(pSkt, pPktIf, (const IP_MULTI_ADDRESS*)localIP);
                return pSkt;    // bind to the correct interface
            }
        }
    }


    // If there is a partial match, then a listening socket is currently 
    // available.  Set up the extended TCB with the info needed 
    // to establish a connection and return this socket to the 
    // caller.
    while(partialSkt != NULL)
    {
        pSkt = partialSkt;

        switch(addressType)
        {
#if defined (TCPIP_STACK_USE_IPV6)
            case IP_ADDRESS_TYPE_IPV6:
                if(pSkt->pV6Pkt == NULL)
                {   // could be a server socket opened with IP_ADDRESS_TYPE_ANY
                    if((pSkt->pV6Pkt = F_TCPv6AllocateTxPacketStruct (pSkt)) != NULL)
                    {
                        TCPIP_IPV6_PacketIPProtocolSet (pSkt->pV6Pkt);
                    }
                }

                if(pSkt->pV6Pkt == NULL)
                {   // failed to allocate memory   
                    return NULL;
                }

                TCPIP_IPV6_DestAddressSet (pSkt->pV6Pkt, (const IPV6_ADDR*)remoteIP);
                break;
#endif  // defined (TCPIP_STACK_USE_IPV6)

#if defined (TCPIP_STACK_USE_IPV4)
            case IP_ADDRESS_TYPE_IPV4:
                // IPv4 lazy allocation
                pSkt->destAddress.Val = ((const IPV4_ADDR *)remoteIP)->Val;
                break;
#endif  // defined (TCPIP_STACK_USE_IPV4)

            default:    // shouldn't happen
                break;
        }

        // success; bind it
        pSkt->addType = (uint8_t)addressType;
        F_TcpSocketBind(pSkt, pPktIf, (const IP_MULTI_ADDRESS*)localIP);
        pSkt->remoteHash = hash;
        pSkt->remotePort = h->SourcePort;
        pSkt->localPort = h->DestPort;
        pSkt->txUnackedTail = pSkt->txStart;

        // All done, and we have a match
        return pSkt;
    }
        
    return NULL;

}



/*****************************************************************************
  Function:
    static void F_TcpSwapHeader(TCP_HEADER* header)

  Summary:
    Swaps endian-ness of a TCP header.

  Description:
    This function changes the endian-ness of a given TCP header for comparison.

  Precondition:
    None

  Parameters:
    header - The TCP header that is to be swapped

  Returns:
    None
  ***************************************************************************/
static void F_TcpSwapHeader(TCP_HEADER* header)
{
    header->SourcePort      = TCPIP_Helper_ntohs(header->SourcePort);
    header->DestPort        = TCPIP_Helper_ntohs(header->DestPort);
    header->SeqNumber       = TCPIP_Helper_ntohl(header->SeqNumber);
    header->AckNumber       = TCPIP_Helper_ntohl(header->AckNumber);
    header->Window          = TCPIP_Helper_ntohs(header->Window);
    header->Checksum        = TCPIP_Helper_ntohs(header->Checksum);
    header->UrgentPointer   = TCPIP_Helper_ntohs(header->UrgentPointer);
}


// initialize a socket
// the socket index and the sizes of its TX/RX buffers are passed as parameters
static void F_TcpSocketInitialize(TCB_STUB* pSkt, TCP_SOCKET hTCP, uint8_t* txBuff, uint16_t txBuffSize, uint8_t* rxBuff, uint16_t rxBuffSize)
{
    pSkt->sktIx = hTCP;     // hTCP is the index of this socket!

    // set all socket FIFO addresses
    pSkt->txStart   = txBuff;
    pSkt->txEnd     = txBuff + txBuffSize + 1;
    pSkt->rxStart   = rxBuff;
    pSkt->rxEnd     = rxBuff + rxBuffSize;

    // Start out assuming worst case Maximum Segment Size (changes when MSS 
    // option is received from remote node)
    pSkt->wRemoteMSS = TCP_MIN_DEFAULT_MTU;

    TCBStubs[hTCP] = pSkt;  // store it
    
}

// set the default socket state
// socket should have been initialized
static void F_TcpSocketSetIdleState(TCB_STUB* pSkt)
{

    pSkt->remoteHash = pSkt->localPort;
    pSkt->txHead = pSkt->txStart;
    pSkt->txTail = pSkt->txStart;
    pSkt->txUnackedTail = pSkt->txStart;
    pSkt->rxHead = pSkt->rxStart;
    pSkt->rxTail = pSkt->rxStart;
    pSkt->flags.bTimerEnabled = 0;
    pSkt->flags.bTimer2Enabled = 0;
    pSkt->flags.bDelayedACKTimerEnabled = 0;
    pSkt->flags.bOneSegmentReceived = 0;
    pSkt->flags.bHalfFullFlush = 0;
    pSkt->flags.bTXASAP = 0;
    pSkt->flags.bTXASAPWithoutTimerReset = 0;
    pSkt->flags.bTXFIN = 0;
    pSkt->flags.bSocketReset = 1;
    pSkt->flags.bRxFin = 0;


    pSkt->flags.bFINSent = 0;
    pSkt->flags.seqInc = 0;
    pSkt->flags.bSYNSent = 0;
    pSkt->retxTmo = pSkt->retxTime = 0;
    pSkt->dupAckCnt = 0;
    pSkt->MySEQ = 0;
    pSkt->sHoleSize = -1;
    pSkt->remoteWindow = 1;
    pSkt->maxRemoteWindow = 1;


    // Note : no result of the explicit binding is maintained!
    pSkt->remotePort = 0;
    pSkt->destAddress.Val = 0;
    pSkt->keepAliveCount = 0;
    // restore initial settings
    pSkt->addType = (uint8_t)((IP_ADDRESS_TYPE)pSkt->flags.openAddType);
    if(pSkt->flags.openBindIf == 0U)
    {
        pSkt->pSktNet = NULL;
    }

    if(pSkt->flags.openBindAdd == 0U)
    {
        pSkt->srcAddress.Val = 0;
    }

#if ((TCPIP_TCP_DEBUG_LEVEL & TCPIP_TCP_DEBUG_MASK_TRACE_STATE) != 0)
    pSkt->dbgFlags.val = 0;
    pSkt->dbgFlags.tracePrevState = 0xf;
    if((tcpTraceMask & (1 << pSkt->sktIx)) != 0)
    {
        pSkt->dbgFlags.traceStateFlag = 1;
    }
#endif  // ((TCPIP_TCP_DEBUG_LEVEL & TCPIP_TCP_DEBUG_MASK_TRACE_STATE) != 0)

}

/*****************************************************************************
  Function:
    static void F_TcpCloseSocket(TCB_STUB* pSkt, TCPIP_TCP_SIGNAL_TYPE tcpEvent)

  Summary:
    Closes a TCP socket.

  Description:
    This function closes a TCP socket.  All socket state information is 
    reset, and any buffered bytes are discarded.
    If this is a server socket it will continue listening.
   
  Precondition:
    None.

  Parameters:
    pSkt
    tcpEvent

  Returns:
    None
  ***************************************************************************/
static void F_TcpCloseSocket(TCB_STUB* pSkt, TCPIP_TCP_SIGNAL_TYPE tcpEvent)
{
    // a socket server will listen after this Close operation
    bool    sktIsKilled;
    bool    freePkt;
    TCPIP_TCP_SIGNAL_FUNCTION sigHandler;
    const void* sigParam;
    uint16_t    sigMask;
    TCPIP_NET_HANDLE pSktNet = NULL;
    TCP_SOCKET   sktIx = 0; 
    uint32_t    uEvent = (uint32_t)tcpEvent;

    if(pSkt->flags.bServer !=  0U && pSkt->flags.forceKill == 0U)
    {   // server socket won't be killed
        sktIsKilled = false;
        // for unspecified address type the buffer gets de-allocated; next time it could be different
        freePkt = (pSkt->flags.openAddType == (uint16_t)IP_ADDRESS_TYPE_ANY) ? true : false;
    }
    else
    {
        sktIsKilled = true;
        freePkt = true;
    }
    while(pSkt->pTxPkt != NULL)
    {
#if defined (TCPIP_STACK_USE_IPV6)
        if(pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
        {
            if(freePkt)
            {
                IPV6_PACKET* pFreePkt = F_TxSktFreeLockedV6Pkt(pSkt);
                if(pFreePkt != NULL)
                {
                    TCPIP_IPV6_PacketFree (pFreePkt);
                }
            }
            else
            {
                IPV6_PACKET* reusePkt = F_TxSktGetLockedV6Pkt(pSkt, NULL, false); 
                if( reusePkt != NULL)
                {   // packet in place; reused
                    TCPIP_IPV6_TransmitPacketStateReset (pSkt->pV6Pkt);
                }
            }
                
            break;
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)

#if defined (TCPIP_STACK_USE_IPV4)
        if(pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV4)
        {
            if(freePkt)
            {
                TCPIP_MAC_PACKET* pFreePkt = F_TxSktFreeLockedV4Pkt(pSkt);
                if(pFreePkt != NULL)
                {
                    TCPIP_PKT_PacketFree(pFreePkt);
                }
            }
        }
#endif  // defined (TCPIP_STACK_USE_IPV4)
        break;
    }

    sigMask = F_TcpSktGetSignalLocked(pSkt, &sigHandler, &sigParam);
    uEvent &= (uint32_t)sigMask;
    if(uEvent != 0U)
    {
        if(sigHandler != NULL)
        {
            sktIx = pSkt->sktIx;
            pSktNet = pSkt->pSktNet;
        }
        else
        {
            uEvent = 0U;
        }
    }

    if(sktIsKilled)
    {
        F_TcpSocketKill(pSkt);
    }
    else
    {
        F_TcpSocketSetIdleState(pSkt);
        F_TcpSocketSetState(pSkt, TCPIP_TCP_STATE_LISTEN);
    }

    if(uEvent != 0U)
    {   // notify socket user
        (*sigHandler)(sktIx, pSktNet, (TCPIP_TCP_SIGNAL_TYPE)uEvent, sigParam);
    }
}


/*****************************************************************************
  Function:
    static uint16_t F_GetMaxSegSizeOption(TCP_HEADER* h)

  Summary:
    Obtains the Maximum Segment Size (MSS) TCP Option out of the TCP header 
    for the current socket.

  Description:
    Parses the current TCP packet header and extracts the Maximum Segment Size 
    option.  

  Precondition:
    Must be called while a TCP packet is present and being processed via 
    F_TcpHandleSeg() and only if the the TCP SYN flag is set.

  Parameters:
    h - pointer to the TCP header

  Returns:
    Maximum segment size option value.  If illegal or not present, a failsafe 
    value of TCP_MIN_DEFAULT_MTU is returned.  If the option is larger than the 
    TCPIP_TCP_MAX_SEG_SIZE_TX upper limit, then TCPIP_TCP_MAX_SEG_SIZE_TX is returned.

  Remarks:
    The internal MAC Read Pointer is moved but not restored.
  ***************************************************************************/
static uint16_t F_GetMaxSegSizeOption(TCP_HEADER* h)
{
    uint8_t vOptionsBytes;
    uint8_t vOption;
    uint16_t wMSS;
    uint8_t* pOption, *pEnd;
    TCPIP_UINT16_VAL u16Val;


    vOptionsBytes = ((h->DataOffset.Val << 2) - (uint8_t)sizeof(*h));
    // Return minimum default Maximum Segment Size value if no options present
    if(vOptionsBytes == 0u)
    {
        return TCP_MIN_DEFAULT_MTU;
    }

    // Seek to beginning of options
    pOption = (uint8_t*)(h + 1);
    pEnd = pOption + vOptionsBytes;

    // Search for the Maximum Segment Size option   
    while((vOptionsBytes-- != 0U) && (pOption < pEnd))
    {
        vOption = *pOption++;

        if(vOption == 0u)   // End of Options list
        {
            break;
        }

        if(vOption == 1u)   // NOP option
        {
            continue;
        }

        if(vOption == 2u)   // Maximum Segment Size option
        {
            if(vOptionsBytes < 3u)
            {
                break;
            }

            wMSS = 0;

            // Get option length
            vOption = *pOption++;
            if(vOption == 4u)
            {// Retrieve MSS and swap value to little endian
                u16Val.v[1] =  *pOption++;
                u16Val.v[0] =  *pOption++;
                wMSS =  u16Val.Val;
            }

            if(wMSS < (uint16_t)TCP_MIN_DEFAULT_MTU)
            {
                break;
            }
            if(wMSS > (uint16_t)TCPIP_TCP_MAX_SEG_SIZE_TX)
            {
                return TCPIP_TCP_MAX_SEG_SIZE_TX;
            }
            else 
            {
                return wMSS;
            }
        }
        else
        { // Assume this is a multi byte option and throw it way
            if(vOptionsBytes < 2u)
            {
                break;
            }
            vOption = *pOption++;
            if(vOptionsBytes < vOption)
            {
                break;
            }
            pOption += vOption;
            vOptionsBytes -= vOption;
        }

    }

    // Did not find MSS option, return worst case default
    return TCP_MIN_DEFAULT_MTU;
}

static void F_TCPSetHalfFlushFlag(TCB_STUB* pSkt)
{
    bool    clrFlushFlag = false;
    TCP_OPTION_THRES_FLUSH_TYPE flushType = (TCP_OPTION_THRES_FLUSH_TYPE)pSkt->flags.halfThresType;

    switch(flushType)
    {
        case TCP_OPTION_THRES_FLUSH_AUTO:
            if((uint32_t)((uintptr_t)pSkt->txEnd - (uintptr_t)pSkt->txStart) >= ((uint32_t)pSkt->wRemoteMSS * 3U) / 2U )
            {
                clrFlushFlag = true;
            }
            break;

        case TCP_OPTION_THRES_FLUSH_OFF:
            clrFlushFlag = true;
            break;

        default:
            /* Do Nothing */
            break;
    }

    pSkt->flags.halfThresFlush = clrFlushFlag ? 0U : 1U;
}


/*****************************************************************************
  Function:
    static void F_TcpHandleSeg(TCB_STUB* pSkt, TCP_HEADER* h, uint16_t len, TCPIP_MAC_PACKET* pRxPkt, uint32_t* pSktEvent)

  Summary:
    Processes an incoming TCP segment.

  Description:
    Once an incoming segment has been matched to a socket, this function
    performs the necessary processing with the data.  Depending on the 
    segment and the state, this may include copying data to the TCP buffer,
    re-assembling out-of order packets, continuing an initialization or 
    closing handshake, or closing the socket altogether.

  Precondition:
    TCP is initialized and the current TCP stub is already synced.

  Parameters:
    pSkt -  the TCP socket
    h - The TCP header for this packet
    tcpLen - The total buffer length of this segment
    pRxPkt - incoming MAC packet
    pSktEvent - address to store the RX event, if any


  Returns:
    None
  ***************************************************************************/
static void F_TcpHandleSeg(TCB_STUB* pSkt, TCP_HEADER* h, uint16_t tcpLen, TCPIP_MAC_PACKET* pRxPkt, uint32_t* pSktEvent)
{
    uint32_t dwTemp;
    uint8_t* ptrTemp;
    uint32_t wTemp;
    int32_t lMissingBytes;
    int32_t wMissingBytes;
    uint16_t wFreeSpace;
    uint8_t localHeaderFlags;
    uint32_t localAckNumber;
    uint32_t localSeqNumber;
    uint16_t len, wSegmentLength;
    bool bSegmentAcceptable;
    uint16_t wNewWindow;
    uint8_t* pSegSrc;
    uint16_t nCopiedBytes;
    uint8_t* newRxHead;
    bool doReturn;


    F_TcpHandleSegEnter(pSkt);
     
    localHeaderFlags = h->Flags.byteVal;
    localAckNumber = h->AckNumber;
    localSeqNumber = h->SeqNumber;

    // We received a packet, reset the keep alive timer and count
    if(pSkt->flags.keepAlive != 0U)
    {
        pSkt->keepAliveCount = 0;
        if(pSkt->flags.bTimerEnabled == 0U)
        {
            pSkt->eventTime = SYS_TMR_TickCountGet() + (pSkt->keepAliveTmo * sysTickFreq) / 1000U;
        }
    }

    pSkt->flags.ackSent = 0;   // clear the ACK already sent

    // Handle TCPIP_TCP_STATE_LISTEN state - will return
    if(pSkt->smState == (uint8_t)TCPIP_TCP_STATE_LISTEN)
    {
        // First: check RST flag
        if((localHeaderFlags & RST) != 0U)
        {
            F_TcpHandleSegDbg(pSkt, "close-RST", 1);
            F_TcpCloseSocket(pSkt, (TCPIP_TCP_SIGNAL_TYPE)0);   // Unbind remote IP address/port info
            return;
        }

        // Second: check ACK flag, which would be invalid
        if((localHeaderFlags & ACK) != 0U)
        {
            // Use a believable sequence number and reset the remote node
            F_TcpHandleSegDbg(pSkt, "close-ACK", 2);
            pSkt->MySEQ = localAckNumber;
            (void) F_TcpSend(pSkt, RST, 0);
            F_TcpCloseSocket(pSkt, (TCPIP_TCP_SIGNAL_TYPE)0);   // Unbind remote IP address/port info
            return;
        }

        // Third: check for SYN flag, which is what we're looking for
        if((localHeaderFlags & SYN) != 0U)
        {
            F_TcpHandleSegDbg(pSkt, "SYN-RCVD", 3);            
            // We now have a sequence number for the remote node
            pSkt->RemoteSEQ = localSeqNumber + 1U;

            // Set MSS option
            pSkt->wRemoteMSS = F_GetMaxSegSizeOption(h);
            F_TCPSetHalfFlushFlag(pSkt);

            // Respond with SYN + ACK
            (void) F_TcpSend(pSkt, SYN | ACK, SENDTCP_RESET_TIMERS);
            F_TcpSocketSetState(pSkt, TCPIP_TCP_STATE_SYN_RECEIVED);
        }
        else
        {
            F_TcpHandleSegDbg(pSkt, "close-unk", 4);                        
            F_TcpCloseSocket(pSkt, (TCPIP_TCP_SIGNAL_TYPE)0);   // Unbind remote IP address/port info
        }

        // Fourth: check for other text and control
        // Nothing to do since we don't support this
        return;
    }

    // Handle TCPIP_TCP_STATE_SYN_SENT state - will return
    if(pSkt->smState == (uint8_t)TCPIP_TCP_STATE_SYN_SENT)
    {
        // Second: check the RST bit
        // This is out of order because this stack has no API for 
        // notifying the application that the connection seems to 
        // be failing.  Instead, the application must time out and 
        // the stack will just keep trying in the mean time.
        if((localHeaderFlags & RST) != 0U)
        {
            F_TcpHandleSegDbg(pSkt, "got-RST", 5);                        
            return;
        }

        // First: check ACK bit
        if((localHeaderFlags & ACK) != 0U)
        {
            if(localAckNumber != pSkt->MySEQ)
            {
                F_TcpHandleSegDbg(pSkt, "wrong-SEQ", 6);
                // Send a RST packet with SEQ = SEG.ACK, but retain our SEQ 
                // number for arivial of any other SYN+ACK packets
                localSeqNumber = pSkt->MySEQ;   // Save our original SEQ number
                pSkt->MySEQ = localAckNumber;   // Set SEQ = SEG.ACK
                (void) F_TcpSend(pSkt, RST, SENDTCP_RESET_TIMERS);      // Send the RST
                pSkt->MySEQ = localSeqNumber;   // Restore original SEQ number
                return;
            }
        }

#if defined (TCPIP_STACK_USE_IPV6)
        if (pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
        {
            TCPIP_NDP_NborReachConfirm (pSkt->pSktNet, TCPIP_IPV6_DestAddressGet(pSkt->pV6Pkt));
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)

        // Third: check the security and precedence
        // No such feature in this stack.  We want to accept all connections.

        // Fourth: check the SYN bit
        if((localHeaderFlags & SYN) != 0U)
        {
            // We now have an initial sequence number and window size
            pSkt->RemoteSEQ = localSeqNumber + 1U;
            pSkt->remoteWindow = pSkt->maxRemoteWindow = h->Window;

            // Set MSS option
            pSkt->wRemoteMSS = F_GetMaxSegSizeOption(h);
            F_TCPSetHalfFlushFlag(pSkt);

            if((localHeaderFlags & ACK) != 0U)
            {
                F_TcpHandleSegDbg(pSkt, "got-ACK", 7);                
                (void) F_TcpSend(pSkt, ACK, SENDTCP_RESET_TIMERS);
                F_TcpSocketSetState(pSkt, TCPIP_TCP_STATE_ESTABLISHED);
                *pSktEvent |= (uint32_t)TCPIP_TCP_SIGNAL_ESTABLISHED;
                // Set up keep-alive timer
                if(pSkt->flags.keepAlive != 0U)
                {
                    pSkt->eventTime = SYS_TMR_TickCountGet() + (pSkt->keepAliveTmo * sysTickFreq) / 1000U;
                }
                pSkt->flags.bTimerEnabled = 0;
            }
            else
            {
                F_TcpHandleSegDbg(pSkt, "miss-ACK", 8);                
                (void) F_TcpSend(pSkt, SYN | ACK, SENDTCP_RESET_TIMERS);
                F_TcpSocketSetState(pSkt, TCPIP_TCP_STATE_SYN_RECEIVED);
            }
        }

        // Fifth: drop the segment if neither SYN or RST is set
        return;
    }

    //
    // First: check the sequence number
    //
    wSegmentLength = len = tcpLen;
    if((localHeaderFlags & FIN) != 0U)
    {
        wSegmentLength++;
    }
    if((localHeaderFlags & SYN) != 0U)
    {
        wSegmentLength++;
    }

    // Calculate the RX FIFO space
    if(pSkt->rxHead >= pSkt->rxTail)
    {
        wFreeSpace = (uint16_t)(((uintptr_t)pSkt->rxEnd - (uintptr_t)pSkt->rxStart) - ((uintptr_t)pSkt->rxHead - (uintptr_t)pSkt->rxTail));
    }
    else
    {
        wFreeSpace = (uint16_t)((uintptr_t)pSkt->rxTail - (uintptr_t)pSkt->rxHead - 1U);
    }

    // Calculate the number of bytes ahead of our head pointer this segment skips
    lMissingBytes = (int32_t)localSeqNumber - (int32_t)pSkt->RemoteSEQ;
    wMissingBytes = lMissingBytes; 

    // Run TCP acceptability tests to verify that this packet has a valid sequence number
    bSegmentAcceptable = false;
    if(wSegmentLength != 0U)
    {
        // Check to see if we have free space, and if so, if any of the data falls within the freespace
        if(wFreeSpace != 0U)
        {
            // RCV.NXT =< SEG.SEQ < RCV.NXT+RCV.WND
            if((lMissingBytes >= 0) && (wFreeSpace > (uint32_t)lMissingBytes))
            {
                bSegmentAcceptable = true;
            }
            else
            {
                // RCV.NXT =< SEG.SEQ+SEG.LEN-1 < RCV.NXT+RCV.WND
                if((lMissingBytes + (int32_t)wSegmentLength) > 0 && (lMissingBytes <= ((int32_t)wFreeSpace - (int32_t)wSegmentLength)))
                {
                    bSegmentAcceptable = true;
                }
            }

            if((lMissingBytes < (int32_t)wFreeSpace) && (wMissingBytes + (int32_t)wSegmentLength > 0))
            {
                bSegmentAcceptable = true;
            }
        }
        // Segments with data are not acceptable if we have no free space
    }
    else
    {
        // Zero length packets are acceptable if they fall within our free space window
        // SEG.SEQ = RCV.NXT
        if(lMissingBytes == 0)
        {
            bSegmentAcceptable = true;
        }
        else
        {
            // RCV.NXT =< SEG.SEQ < RCV.NXT+RCV.WND
            if((lMissingBytes >= 0) && ((int32_t)wFreeSpace > lMissingBytes))
            {
                bSegmentAcceptable = true;
            }
        }
    }

    if(!bSegmentAcceptable)
    {
        // Unacceptable segment, drop it and respond appropriately
        if((localHeaderFlags & RST) == 0U) 
        {
            F_TcpHandleSegDbg(pSkt, "seg-NA RST", 9);
            (void) F_TcpSend(pSkt, ACK, SENDTCP_RESET_TIMERS);
        }
        else
        {
            F_TcpHandleSegDbg(pSkt, "seg-NA", 10);
        }
        return;
    }


    //
    // Second: check the RST bit
    //
    //
    // Fourth: check the SYN bit
    //
    // Note, that since the third step is not implemented, we can 
    // combine this second and fourth step into a single operation.
    if((localHeaderFlags & (RST | SYN)) != 0U)
    {
        if((localHeaderFlags & RST) != 0U)
        {
            F_TcpHandleSegDbg(pSkt, "close-RST", 11);            
            *pSktEvent |= (uint32_t)TCPIP_TCP_SIGNAL_RX_RST;
            pSkt->flags.bSocketReset = 1;
        }
        else
        {
            F_TcpHandleSegDbg(pSkt, "close-SYN", 12);            
        }
        F_TcpCloseSocket(pSkt, (TCPIP_TCP_SIGNAL_TYPE)0);
        return;
    }

    //
    // Third: check the security and precedence
    //
    // Feature not supported.  Let's process this segment.

    //
    // Fifth: check the ACK bit
    //
    if((localHeaderFlags & ACK) == 0U)
    {
        F_TcpHandleSegDbg(pSkt, "no-ACK", 13);            
        return;
    }

#if defined (TCPIP_STACK_USE_IPV6)
    // If we've received an ACK, update neighbor reachability
    if (pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
    {
        TCPIP_NDP_NborReachConfirm (pSkt->pSktNet, TCPIP_IPV6_DestAddressGet(pSkt->pV6Pkt));
    }
#endif  // defined (TCPIP_STACK_USE_IPV6)

    doReturn = false;
    switch(pSkt->smState)
    {
        case (uint8_t)TCPIP_TCP_STATE_SYN_RECEIVED:
        case (uint8_t)TCPIP_TCP_STATE_ESTABLISHED:
        case (uint8_t)TCPIP_TCP_STATE_FIN_WAIT_1:
        case (uint8_t)TCPIP_TCP_STATE_FIN_WAIT_2:
        case (uint8_t)TCPIP_TCP_STATE_CLOSE_WAIT:
        case (uint8_t)TCPIP_TCP_STATE_CLOSING:

            if(pSkt->smState == (uint8_t)TCPIP_TCP_STATE_SYN_RECEIVED)
            {
                if(localAckNumber != pSkt->MySEQ)
                {
                    F_TcpHandleSegDbg(pSkt, "no-SEQ", 14);            
                    // Send a RST packet with SEQ = SEG.ACK, but retain our SEQ 
                    // number for arival of any other correct packets
                    localSeqNumber = pSkt->MySEQ;   // Save our original SEQ number
                    pSkt->MySEQ = localAckNumber;   // Set SEQ = SEG.ACK
                    (void) F_TcpSend(pSkt, RST, SENDTCP_RESET_TIMERS);      // Send the RST
                    pSkt->MySEQ = localSeqNumber;   // Restore original SEQ number
                    doReturn = true;
                    break;
                }
                else
                {
                    F_TcpHandleSegDbg(pSkt, "good-SEQ", 15);            
                }
                F_TcpSocketSetState(pSkt, TCPIP_TCP_STATE_ESTABLISHED);
                *pSktEvent |= (uint32_t)TCPIP_TCP_SIGNAL_ESTABLISHED;
            }

            // Calculate what the highest possible SEQ number in our TX FIFO is
            wTemp = (uint32_t)((uintptr_t)pSkt->txHead - (uintptr_t)pSkt->txUnackedTail);
            if((int32_t)wTemp < 0)
            {
                wTemp += (uint32_t)((uintptr_t)pSkt->txEnd - (uintptr_t)pSkt->txStart);
            }
            dwTemp = pSkt->MySEQ + wTemp;

            // Drop the packet if it ACKs something we haven't sent
            dwTemp = localAckNumber - dwTemp;
            if((int32_t)dwTemp > 0)
            {   // acknowledged more than we've sent??
                if((pSkt->flags.bFINSent == 0U) || pSkt->flags.seqInc != 0U || dwTemp != 1U)
                {
                    F_TcpHandleSegDbg(pSkt, "bad-ACK", 16);                                
                   (void) F_TcpSend(pSkt, ACK, 0);
                    doReturn = true;
                    break;
                }
                else
                {
                    F_TcpHandleSegDbg(pSkt, "inc-SEQ", 17);                                
                    pSkt->MySEQ++;      // since we didn't count the FIN
                    pSkt->flags.seqInc = 1;
                }
            }

            // Throw away all ACKnowledged TX data:
            // Calculate what the last acknowledged sequence number was (ignoring any FINs we sent)
            dwTemp = pSkt->MySEQ - (uint32_t)((uintptr_t)pSkt->txUnackedTail - (uintptr_t)pSkt->txTail);
            if(pSkt->txUnackedTail < pSkt->txTail)
            {
                dwTemp -= (uint32_t)((uintptr_t)pSkt->txEnd - (uintptr_t)pSkt->txStart);
            }

            // Calculate how many bytes were ACKed with this packet
            dwTemp = localAckNumber - dwTemp;
            if(((int32_t)(dwTemp) > 0) && (dwTemp <= (uint32_t)((uintptr_t)pSkt->txEnd - (uintptr_t)pSkt->txStart)))
            {   // ACK-ed some data
                F_TcpHandleSegDbg(pSkt, "data-ACK", 18);                                
                F_TCP_LoadRetxTmo(pSkt, true);
                pSkt->dupAckCnt = 0;
                pSkt->flags.bHalfFullFlush = 0U;

                // Bytes ACKed, free up the TX FIFO space
                ptrTemp = pSkt->txTail;
                pSkt->txTail += dwTemp;
                if(pSkt->txUnackedTail >= ptrTemp)
                {
                    if(pSkt->txUnackedTail < pSkt->txTail)
                    {
                        pSkt->MySEQ += (uint32_t)((uintptr_t)pSkt->txTail - (uintptr_t)pSkt->txUnackedTail);
                        pSkt->txUnackedTail = pSkt->txTail;
                    }
                }
                else
                {
                    ptrTemp = pSkt->txUnackedTail + ((uintptr_t)pSkt->txEnd - (uintptr_t)pSkt->txStart);
                    if(ptrTemp < pSkt->txTail)
                    {
                        pSkt->MySEQ += (uint32_t)((uintptr_t)pSkt->txTail - (uintptr_t)ptrTemp);
                        pSkt->txUnackedTail = pSkt->txTail;
                    }
                }
                if(pSkt->txTail >= pSkt->txEnd)
                {
                    pSkt->txTail -= pSkt->txEnd - pSkt->txStart;
                }
                if(pSkt->txUnackedTail >= pSkt->txEnd)
                {
                    pSkt->txUnackedTail -= pSkt->txEnd - pSkt->txStart;
                }

                if(pSkt->smState == (uint32_t)TCPIP_TCP_STATE_ESTABLISHED || pSkt->smState == (uint32_t)TCPIP_TCP_STATE_CLOSE_WAIT)
                {
                    *pSktEvent |= (uint32_t)TCPIP_TCP_SIGNAL_TX_SPACE;
                }
            }
            else
            {   // no acknowledge
                F_TcpHandleSegDbg(pSkt, "no data-ACK", 19);                                                
                // See if we have outstanding TX data that is waiting for an ACK
                if(pSkt->txTail != pSkt->txUnackedTail)
                {
                    bool fastRetransmit = false;
                    if(++pSkt->dupAckCnt >= 3U)
                    {
                        fastRetransmit = true; 
                    }
                    else if(pSkt->retxTime != 0U)
                    {
                        if(((int32_t)SYS_TMR_TickCountGet() - (int32_t)pSkt->retxTime) >= 0)
                        {   // ack timeout
                            F_TCP_LoadRetxTmo(pSkt, false);
                            fastRetransmit = true;
                        }
                    }
                    else
                    {
                        // do nothing
                    }

                    if(fastRetransmit != false)
                    {
                        // Set up to perform a fast retransmission
                        // Roll back unacknowledged TX tail pointer to cause retransmit to occur
                        pSkt->MySEQ -= (uint32_t)((uintptr_t)pSkt->txUnackedTail - (uintptr_t)pSkt->txTail);

                        if(pSkt->txUnackedTail < pSkt->txTail)
                        {
                            pSkt->MySEQ -= (uint32_t)((uintptr_t)pSkt->txEnd - (uintptr_t)pSkt->txStart);
                        }
                        pSkt->txUnackedTail = pSkt->txTail;
                        pSkt->flags.bTXASAPWithoutTimerReset = 1;
                    }
                }
            }

            // No need to keep our retransmit timer going if we have nothing that needs ACKing anymore
            if(pSkt->txTail == pSkt->txHead)
            {
                // Make sure there isn't a "FIN byte in our TX FIFO"
                if(pSkt->flags.bTXFIN == 0u)
                {
                    // Convert retransmission timer to keep-alive timer
                    if(pSkt->flags.keepAlive != 0U)
                    {
                        pSkt->eventTime = SYS_TMR_TickCountGet() + (pSkt->keepAliveTmo * sysTickFreq) / 1000U;
                    }
                    pSkt->flags.bTimerEnabled = 0;
                }
                else
                {
                    // "Throw away" FIN byte from our TX FIFO if it has been ACKed
                    if((pSkt->MySEQ == localAckNumber) && (pSkt->flags.bFINSent != 0U))
                    {
                        pSkt->flags.bTimerEnabled = 0;
                        pSkt->flags.bTXFIN = 0;
                    }
                }
            }

            // update the max window
            if(h->Window > pSkt->maxRemoteWindow)
            {
                pSkt->maxRemoteWindow = h->Window;
            }
            // The window size advertised in this packet is adjusted to account 
            // for any bytes that we have transmitted but haven't been ACKed yet 
            // by this segment.
            wNewWindow = h->Window - ((uint16_t)(pSkt->MySEQ - localAckNumber));

            // Update the local stored copy of the RemoteWindow.
            // If previously we had a zero window, and now we don't, then 
            // immediately send whatever was pending.
            if((pSkt->remoteWindow == 0u) && (wNewWindow != 0U))
            {
                pSkt->flags.bTXASAP = 1;
            }
            pSkt->remoteWindow = wNewWindow;

            // A couple of states must do all of the TCPIP_TCP_STATE_ESTABLISHED stuff, but also a little more
            if(pSkt->smState == (uint8_t)TCPIP_TCP_STATE_FIN_WAIT_1)
            {
                // Check to see if our FIN has been ACKnowledged
                if((pSkt->MySEQ == localAckNumber) && (pSkt->flags.bFINSent != 0U))
                {
                    // Reset our timer for forced closure if the remote node 
                    // doesn't send us a FIN in a timely manner.
                    F_TcpHandleSegDbg(pSkt, "tmr reset", 20);                                                
                    pSkt->closeWaitTime = SYS_TMR_TickCountGet() + ((uint32_t)TCPIP_TCP_FIN_WAIT_2_TIMEOUT * sysTickFreq) / 1000U;
                    F_TcpSocketSetState(pSkt, TCPIP_TCP_STATE_FIN_WAIT_2);
                }
            }
            else if(pSkt->smState == (uint8_t)TCPIP_TCP_STATE_CLOSING)
            {
                // Check to see if our FIN has been ACKnowledged
                if(pSkt->MySEQ == localAckNumber)
                {
                    F_TcpHandleSegDbg(pSkt, "FIN_ACK", 21);                                                                    
#if (TCPIP_TCP_MSL_TIMEOUT != 0)
                    F_TcpSocketSetState(pSkt, TCPIP_TCP_STATE_TIME_WAIT);
                    pSkt->closeWaitTime = SYS_TMR_TickCountGet() + (((uint32_t)TCPIP_TCP_MSL_TIMEOUT * 2U) * sysTickFreq);
#else
                    F_TcpCloseSocket(pSkt, (TCPIP_TCP_SIGNAL_TYPE)0);
#endif  // (TCPIP_TCP_MSL_TIMEOUT != 0)
                }
                doReturn = true;
            }
            else
            {
                F_TcpHandleSegDbg(pSkt, "do-nothing", 22);                                                                                    
                /* Do Nothing */
            }

            break;

        case (uint8_t)TCPIP_TCP_STATE_LAST_ACK:
            // Check to see if our FIN has been ACKnowledged
            if(pSkt->MySEQ + 1U == localAckNumber)
            {
                F_TcpHandleSegDbg(pSkt, "close-LAST_ACK", 23);                                                                                                    
                F_TcpCloseSocket(pSkt, (TCPIP_TCP_SIGNAL_TYPE)0);
            }
            doReturn = true;
            break;

        default:    // case TCPIP_TCP_STATE_TIME_WAIT:
            /* Do Nothing */
            break;
    }

    if(doReturn)
    {
        return;
    }

    //
    // Sixth: Check the URG bit
    //
    // Urgent packets are not supported in this stack, so we
    // will throw them away instead
    if((localHeaderFlags & URG) != 0U)
    {
        return;
    }

    //
    // Seventh: Process the segment text
    //
    // Throw data away if in a state that doesn't accept data
    if(pSkt->smState == (uint8_t)TCPIP_TCP_STATE_CLOSE_WAIT || pSkt->smState == (uint8_t)TCPIP_TCP_STATE_CLOSING || pSkt->smState == (uint8_t)TCPIP_TCP_STATE_LAST_ACK)
    {
        F_TcpHandleSegDbg(pSkt, "data-throw", 24);                                                                                                            
        return;
    }

    // Copy any valid segment data into our RX FIFO, if any
    if(len != 0U)
    {
        // See if there are bytes we must skip
        // wMissingBytes == 0: this new data is in sync with what we expect
        // wMissingBytes  < 0: this packet contains old data that needs to be skipped
        if(wMissingBytes <= 0)
        {
            F_TcpHandleSegDbg(pSkt, "data-skip", 25);
            // Position packet read pointer to start of useful data area.
            // Protect against too old data is done by checking the actual number of copied bytes from the packet
            pSegSrc = ((uint8_t*)h + (h->DataOffset.Val << 2)) - wMissingBytes;
            len = (uint16_t)((uint32_t)len + (uint32_t)wMissingBytes);      

            // Truncate packets that would overflow our TCP RX FIFO
            // and request a retransmit by sending a duplicate ACK
            if(len > wFreeSpace)
            {
                len = wFreeSpace;
            }

            // Copy the application data from the packet into the socket RX FIFO
            // See if we need a two part copy (spans rxEnd->rxStart)
            if(pSkt->rxHead + len > pSkt->rxEnd)
            {
                wTemp = (uint32_t)((uintptr_t)pSkt->rxEnd - (uintptr_t)pSkt->rxHead + 1U);
                nCopiedBytes = TCPIP_Helper_PacketCopy(pRxPkt, pSkt->rxHead, &pSegSrc, (uint16_t)wTemp, true);
                nCopiedBytes += TCPIP_Helper_PacketCopy(pRxPkt, pSkt->rxStart, &pSegSrc, (uint16_t)(len - wTemp), true);
                newRxHead = pSkt->rxStart + (len - wTemp);
            }
            else
            {
                nCopiedBytes = TCPIP_Helper_PacketCopy(pRxPkt, pSkt->rxHead, &pSegSrc, len, true);
                newRxHead = pSkt->rxHead + len;
            }

            if(nCopiedBytes == len)
            {   // successfully copied data
                pSkt->RemoteSEQ += (uint32_t)len;
                pSkt->rxHead = newRxHead;

                // we have some new data in the socket
                if(pSkt->smState == (uint8_t)TCPIP_TCP_STATE_ESTABLISHED || pSkt->smState == (uint8_t)TCPIP_TCP_STATE_FIN_WAIT_1 || pSkt->smState == (uint8_t)TCPIP_TCP_STATE_FIN_WAIT_2)
                {   // valid RX state
                    *pSktEvent |= (uint32_t)TCPIP_TCP_SIGNAL_RX_DATA;
                }

                // See if we have a hole and other data waiting already in the RX FIFO
                if(pSkt->sHoleSize != -1)
                {
                    pSkt->sHoleSize -= (int32_t)len;
                    wTemp = (uint32_t)pSkt->wFutureDataSize + (uint32_t)pSkt->sHoleSize;

                    // See if we just closed up a hole, and if so, advance head pointer
                    if((int32_t)wTemp < 0)
                    {
                        pSkt->sHoleSize = -1;
                    }
                    else if(pSkt->sHoleSize <= 0)
                    {
                        pSkt->RemoteSEQ += wTemp;
                        pSkt->rxHead += wTemp;
                        if(pSkt->rxHead > pSkt->rxEnd)
                        {
                            pSkt->rxHead -= pSkt->rxEnd - pSkt->rxStart + 1;                            
                        }
                        pSkt->sHoleSize = -1;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
            }
        } 
        else 
        {   // wMissingBytes  > 0: this packet contains ahead data
            // This packet is out of order or we lost a packet, see if we can generate a hole to accomodate it
            // Truncate packets that would overflow our TCP RX FIFO
            F_TcpHandleSegDbg(pSkt, "data-ahead", 26);
            if((int32_t)len + wMissingBytes > (int32_t)wFreeSpace)
            {
                len = (uint16_t)((uint32_t)wFreeSpace - (uint32_t)wMissingBytes);
            }

            // Position packet read pointer to start of useful data area.
            pSegSrc = (uint8_t*)h + (h->DataOffset.Val << 2);

            // See if we need a two part copy (spans rxEnd->rxStart)
            if(pSkt->rxHead + wMissingBytes + len > pSkt->rxEnd)
            {
                // Calculate number of data bytes to copy before wraparound
                wTemp = (uint32_t)((uintptr_t)pSkt->rxEnd - (uintptr_t)pSkt->rxHead + 1U) - (uint32_t)wMissingBytes;
                if((int32_t)wTemp >= 0)
                {
                    nCopiedBytes = TCPIP_Helper_PacketCopy(pRxPkt, pSkt->rxHead + wMissingBytes, &pSegSrc, (uint16_t)wTemp, true);
                    nCopiedBytes += TCPIP_Helper_PacketCopy(pRxPkt, pSkt->rxStart, &pSegSrc, len - (uint16_t)wTemp, true);
                }
                else
                {
                    nCopiedBytes = TCPIP_Helper_PacketCopy(pRxPkt, pSkt->rxHead + wMissingBytes - (pSkt->rxEnd - pSkt->rxStart + 1), &pSegSrc, len, true);
                }
            }
            else
            {
                nCopiedBytes = TCPIP_Helper_PacketCopy(pRxPkt, pSkt->rxHead + wMissingBytes, &pSegSrc, len, true);
            }

            F_TcpHandleSegDbgData(pSkt, "data-ahead", nCopiedBytes, 27);
            if(nCopiedBytes == len)
            {
                // Record the hole is here
                if(pSkt->sHoleSize == -1)
                {
                    pSkt->sHoleSize = wMissingBytes;
                    pSkt->wFutureDataSize = len;
                }
                else
                {
                    // We already have a hole, see if we can shrink the hole 
                    // or extend the future data size
                    if((uint32_t)wMissingBytes < (uint32_t)pSkt->sHoleSize)
                    {
                        if(((uint32_t)wMissingBytes + (uint32_t)len > (uint32_t)pSkt->sHoleSize + pSkt->wFutureDataSize) || ((uint32_t)wMissingBytes + (uint32_t)len < (uint32_t)pSkt->sHoleSize))
                        {
                            pSkt->wFutureDataSize = len;
                        }
                        else
                        {
                            pSkt->wFutureDataSize = (uint16_t)((uint32_t)pSkt->sHoleSize + (uint32_t)pSkt->wFutureDataSize - (uint32_t)wMissingBytes);
                        }
                        pSkt->sHoleSize = wMissingBytes;
                    }
                    else if((uint32_t)wMissingBytes + (uint32_t)len > (uint32_t)pSkt->sHoleSize + pSkt->wFutureDataSize)
                    {
                        // Make sure that there isn't a second hole between 
                        // our future data and this TCP segment's future data
                        if((uint32_t)wMissingBytes <= (uint32_t)pSkt->sHoleSize + pSkt->wFutureDataSize)
                        {
                            pSkt->wFutureDataSize += (uint16_t)(((uint32_t)wMissingBytes + len) - ((uint32_t)pSkt->sHoleSize + pSkt->wFutureDataSize));
                        }
                    }
                    else
                    {
                        /* Do Nothing */
                    }

                }
            }
        }
    }

    // Send back an ACK of the data (+SYN | FIN) we just received, 
    // if any.  To minimize bandwidth waste, we are implementing 
    // the delayed acknowledgement algorithm here, only sending 
    // back an immediate ACK if this is the second segment received.  
    // Otherwise, a 200ms timer will cause the ACK to be transmitted.
    if(wSegmentLength != 0U)
    {
        // For non-established sockets, delete all data in 
        // the RX buffer immediately after receiving it.
        // That'll ensure that the RX window is nonzero and 
        // the remote node will be able to send a FIN response, 
        // which needs an RX window of at least 1.
        if(pSkt->smState != (uint8_t)TCPIP_TCP_STATE_ESTABLISHED && pSkt->smState != (uint8_t)TCPIP_TCP_STATE_FIN_WAIT_1 && pSkt->smState != (uint8_t)TCPIP_TCP_STATE_FIN_WAIT_2)
        {
            pSkt->rxTail = pSkt->rxHead;
        }

        if(pSkt->flags.bOneSegmentReceived != 0U)
        {
            F_TcpHandleSegDbg(pSkt, "send-ACK", 28);
            (void) F_TcpSend(pSkt, ACK, SENDTCP_RESET_TIMERS);
            // bOneSegmentReceived is cleared in F_TcpSend(pSkt, ), so no need here
        }
        else
        {
            F_TcpHandleSegDbg(pSkt, "delay-ACK", 29);
            pSkt->flags.bOneSegmentReceived = 1U;   

            // Do not send an ACK immediately back.  Instead, we will 
            // perform delayed acknowledgements.  To do this, we will 
            // just start a timer
            if(pSkt->flags.bDelayedACKTimerEnabled == 0U)
            {
                pSkt->flags.bDelayedACKTimerEnabled = 1;
                pSkt->delayedACKTime = SYS_TMR_TickCountGet() + ((uint32_t)TCPIP_TCP_DELAYED_ACK_TIMEOUT * sysTickFreq) / 1000U;

            }
        }
    }

    //
    // Eighth: check the FIN bit
    //
    if((localHeaderFlags & FIN) != 0U)
    {
        // Note: Since we don't have a good means of storing "FIN bytes" 
        // in our TCP RX FIFO, we must ensure that FINs are processed 
        // in-order.
        if(pSkt->RemoteSEQ + 1U == localSeqNumber + (uint32_t)wSegmentLength)
        {
            // FINs are treated as one byte of data for ACK sequencing
            pSkt->RemoteSEQ++;

            *pSktEvent |= (uint32_t)TCPIP_TCP_SIGNAL_RX_FIN;
            pSkt->flags.bRxFin = 1;
            doReturn = false;
            switch(pSkt->smState)
            {
                case (uint8_t)TCPIP_TCP_STATE_SYN_RECEIVED:
                    // RFC in exact: Our API has no need for the user 
                    // to explicitly close a socket that never really 
                    // got opened fully in the first place, so just 
                    // transmit a FIN automatically and jump to 
                    // TCPIP_TCP_STATE_LAST_ACK
                    F_TcpHandleSegDbg(pSkt, "last-ACK", 30);
                    F_TcpSocketSetState(pSkt, TCPIP_TCP_STATE_LAST_ACK);
                    (void) F_TcpSend(pSkt, FIN | ACK, SENDTCP_RESET_TIMERS);
                    doReturn = true;
                    break;

                case (uint8_t)TCPIP_TCP_STATE_ESTABLISHED:
                    // Go to TCPIP_TCP_STATE_CLOSE_WAIT state
                    F_TcpHandleSegDbg(pSkt, "close-WAIT", 31);
                    F_TcpSocketSetState(pSkt, TCPIP_TCP_STATE_CLOSE_WAIT);

#if (TCPIP_TCP_CLOSE_WAIT_TIMEOUT != 0)
                    // If the application doesn't call 
                    // TCPIP_TCP_Disconnect() as needed and expects the 
                    // stack to automatically close sockets when the 
                    // remote node sends a FIN, a timer is started so 
                    // that the socket will eventually be closed automatically
                    pSkt->closeWaitTime = SYS_TMR_TickCountGet() + (TCPIP_TCP_CLOSE_WAIT_TIMEOUT * sysTickFreq) / 1000U;
#endif  // (TCPIP_TCP_CLOSE_WAIT_TIMEOUT != 0)

                    if(pSkt->flags.ackSent != 0U)
                    {   // don't send another ack to the FIN if we already did that
                        doReturn = true;
                    }

                    break;

                case (uint8_t)TCPIP_TCP_STATE_FIN_WAIT_1:
                    if(pSkt->MySEQ == localAckNumber)
                    {
                        F_TcpHandleSegDbg(pSkt, "close-WAIT", 32);
#if (TCPIP_TCP_MSL_TIMEOUT != 0)
                        F_TcpSocketSetState(pSkt, TCPIP_TCP_STATE_TIME_WAIT);
                        pSkt->closeWaitTime = SYS_TMR_TickCountGet() + (((uint32_t)TCPIP_TCP_MSL_TIMEOUT * 2U) * sysTickFreq);
#else
                        (void) F_TcpSend(pSkt, ACK, 0);
                        F_TcpCloseSocket(pSkt, (TCPIP_TCP_SIGNAL_TYPE)0);
                        doReturn = true;
#endif  // (TCPIP_TCP_MSL_TIMEOUT != 0)
                    }
                    else
                    {
                        F_TcpHandleSegDbg(pSkt, "closing", 33);
                        F_TcpSocketSetState(pSkt, TCPIP_TCP_STATE_CLOSING);
                    }
                    break;

                case (uint8_t)TCPIP_TCP_STATE_FIN_WAIT_2:
                    F_TcpHandleSegDbg(pSkt, "fin-wait2", 34);
#if (TCPIP_TCP_MSL_TIMEOUT != 0)
                    F_TcpSocketSetState(pSkt, TCPIP_TCP_STATE_TIME_WAIT);
                    pSkt->closeWaitTime = SYS_TMR_TickCountGet() + (((uint32_t)TCPIP_TCP_MSL_TIMEOUT * 2U) * sysTickFreq);
#else
                    (void) F_TcpSend(pSkt, ACK, 0);
                    F_TcpCloseSocket(pSkt, (TCPIP_TCP_SIGNAL_TYPE)0);
                    doReturn = true;
#endif  // (TCPIP_TCP_MSL_TIMEOUT != 0)
                    break;

                default:    // case TCPIP_TCP_STATE_TIME_WAIT: reacknowledge
                    /* Do Nothing */
                    break;
            }

            if(doReturn)
            {
                return;
            }

            // Acknowledge receipt of FIN
            F_TcpHandleSegDbg(pSkt, "fin-ack", 35);
            (void) F_TcpSend(pSkt, ACK, SENDTCP_RESET_TIMERS);
        }
    }
}


/****************************************************************************
  Section:
    Buffer Management Functions
  ***************************************************************************/

/*****************************************************************************
  Function:
    bool TCPIP_TCP_FifoSizeAdjust(TCP_SOCKET hTCP, uint16_t wMinRXSize, 
                            uint16_t wMinTXSize, TCP_ADJUST_FLAGS vFlags)

  Summary:
    Adjusts the relative sizes of the RX and TX buffers.

  Description:
    This function can be used to simultaneously adjust the sizes
    of the RX and TX FIFOs.

    Adjusting the size of the TX/RX FIFO on the fly can allow for optimal transmission speed 
    for one-sided application protocols.
    For example, HTTP typically begins by receiving large amounts of data from the client,
    then switches to serving large amounts of data back.
    Adjusting the FIFO at these points can increase performance in systems that have 
    limited resources.
    Once the FIFOs are adjusted, a window update is sent.
    
    The TCP_ADJUST_FLAGS control the distribution of the remaining available space
    between the TX and RX FIFOs.
    If neither or both of TCP_ADJUST_GIVE_REST_TO_TX and 
    TCP_ADJUST_GIVE_REST_TO_RX are set, the function distributes the
    remaining space (if any) equally.
    If the new requested FIFOs space is greater that the old existing FIFOs space
    the TCP_ADJUST_GIVE_REST_TO_TX and TCP_ADJUST_GIVE_REST_TO_RX are ignored.
    
    TCP_ADJUST_PRESERVE_RX and TCP_ADJUST_PRESERVE_TX request the
    preserving of the existing data.
    Existing data can be preserved as long as the old data in the buffer
    does not exceed the capacity of the new buffer.

  Precondition:
    TCP is initialized.

  Parameters:
    hTCP        - The socket to be adjusted
    wMinRXSize  - Minimum number of bytes for the RX FIFO
    wMinTXSize  - Minimum number of bytes for the TX FIFO
    vFlags      - If TCP_ADJUST_TX_ONLY or TCP_ADJUST_RX_ONLY  are not set,
                    then the TX and RX bufferrs are evaluated together and any
                    combination of the following flags is valid:
                    TCP_ADJUST_GIVE_REST_TO_RX, TCP_ADJUST_GIVE_REST_TO_TX
                    TCP_ADJUST_PRESERVE_RX, TCP_ADJUST_PRESERVE_TX
                - If TCP_ADJUST_TX_ONLY or TCP_ADJUST_RX_ONLY is set
                    then TX and RX buffers are treated individually
                    and TCP_ADJUST_GIVE_REST_TO_TX, TCP_ADJUST_GIVE_REST_TO_RX
                    values are irrelevant.
                - TCP_ADJUST_TX_ONLY and TCP_ADJUST_RX_ONLY both set is invalid.
                   

  Return Values:
    true - The FIFOs were adjusted successfully
    false - New RX and/or TX buffers could not be allocated
            and therefore the socket was left unchanged.

  Remarks:
    The function will automatically maintain minimal values for both TX and RX buffers.

    To avoid having a socket with no associated buffers, the function first
    allocates the new buffers and, if succeeded, it frees the old ones.

    The TX and RX FIFOs (buffers) associated with a socket
    are completely separate and independent.
    However, when TCP_ADJUST_TX_ONLY or TCP_ADJUST_RX_ONLY are not set,
    for the purpose of this function, the TX and RX FIFOs are considered
    to be contiguous so that the total FIFO space is divided between
    the TX and RX FIFOs.
    This provides backward compatibility with previous versions of this function.

    The new flags TCP_ADJUST_TX_ONLY and TCP_ADJUST_RX_ONLY allow changing
    the size of TX and RX buffers independently.
    This is the preferred option.

    TX or RX associated buffer sizes can be changed too using the socket options.
    See TCPIP_TCP_OptionsSet. 

    The size of the buffers should NOT be decreased when the socket has pending data
    to be sent to the remote party or to be received by the socket user.
    Doing this may disrupt the communication, make the TCP algorithm fail or have an 
    unpredicted behavior!

  ***************************************************************************/
#if (TCPIP_TCP_DYNAMIC_OPTIONS != 0)
bool TCPIP_TCP_FifoSizeAdjust(TCP_SOCKET hTCP, uint16_t wMinRXSize, uint16_t wMinTXSize, TCP_ADJUST_FLAGS vFlags)
{
    uint16_t    oldTxSize, pendTxEnd, pendTxBeg, txUnackOffs;
    uint16_t    oldRxSize, avlblRxEnd, avlblRxBeg;
    uint16_t    diffChange;
    uint8_t     *newTxBuff, *newRxBuff;
    bool        adjustFail;
    
    uint32_t uvFlags = (uint32_t)vFlags;
    if((uvFlags & ((uint32_t)TCP_ADJUST_TX_ONLY | (uint32_t)TCP_ADJUST_RX_ONLY)) == ((uint32_t)TCP_ADJUST_TX_ONLY | (uint32_t)TCP_ADJUST_RX_ONLY))
    {   // invalid option
        return false;
    }

    TCB_STUB* pSkt = F_TcpSocketChk(hTCP); 

    if(pSkt == NULL)
    {
        return false;
    }

    // minimum size check
    if(wMinRXSize < TCP_MIN_RX_BUFF_SIZE)
    {
        wMinRXSize = TCP_MIN_RX_BUFF_SIZE;
    }
    if(wMinTXSize < TCP_MIN_TX_BUFF_SIZE)
    {
        wMinTXSize = TCP_MIN_TX_BUFF_SIZE;
    }
    

    oldTxSize = (uint16_t)((uintptr_t)pSkt->txEnd - (uintptr_t)pSkt->txStart - 1U);
    oldRxSize = (uint16_t)((uintptr_t)pSkt->rxEnd - (uintptr_t)pSkt->rxStart);


    if((uvFlags & (uint32_t)TCP_ADJUST_TX_ONLY) != 0U)
    {   // preserve RX buffer
        wMinRXSize = oldRxSize;
    }
    else if((uvFlags & (uint32_t)TCP_ADJUST_RX_ONLY) != 0U)
    {   // preserve TX buffer
        wMinTXSize = oldTxSize;
    }
    else if(oldTxSize + oldRxSize > wMinRXSize + wMinTXSize)
    {   // change both buffers relative to the old cumulated size
        // OK, we have some available space left
        uint16_t leftSpace = (oldTxSize + oldRxSize) - (wMinRXSize + wMinTXSize);

        // Set both allocation flags if none set
        uint32_t equalMask = ((uint32_t)TCP_ADJUST_GIVE_REST_TO_TX | (uint32_t)TCP_ADJUST_GIVE_REST_TO_RX);

        if((uvFlags & equalMask) == 0U || (uvFlags & equalMask) == equalMask)
        {   // distribute what's left equally
            wMinRXSize += (leftSpace + 1U) >> 1U;
            wMinTXSize += (leftSpace +1U) >> 1U;
        }
        else if((uvFlags & equalMask) == (uint32_t)TCP_ADJUST_GIVE_REST_TO_TX)
        {
            wMinTXSize += leftSpace;
        }
        else
        {
            wMinRXSize += leftSpace;
        }
    }
    else
    {
        /* Do Nothing */
    }

    if(wMinTXSize >= oldTxSize)
    {
        diffChange = wMinTXSize - oldTxSize;
    }
    else
    {
        diffChange = oldTxSize - wMinTXSize;
    }

    if(diffChange >= TCP_MIN_BUFF_CHANGE)
    {
        newTxBuff = (uint8_t*)TCPIP_HEAP_Malloc(tcpHeapH, (size_t)((uint32_t)wMinTXSize + 1U));
        if(newTxBuff == NULL)
        {    // fail, out of memory
            return false;
        }
    }
    else
    {   // same TX size, no change needed
        newTxBuff = NULL;
    }

    if(wMinRXSize >= oldRxSize)
    {
        diffChange = wMinRXSize - oldRxSize;
    }
    else
    {
        diffChange = oldRxSize - wMinRXSize;
    }

    if(diffChange >= TCP_MIN_BUFF_CHANGE)
    {

        newRxBuff = (uint8_t*)TCPIP_HEAP_Malloc(tcpHeapH, (size_t)((uint32_t)wMinRXSize + 1U));
        if(newRxBuff == NULL)
        {    // fail, out of memory
            (void) TCPIP_HEAP_Free(tcpHeapH, newTxBuff);
            return false;
        }
    }
    else
    {   // same RX size, no change needed
        newRxBuff = NULL;
    }


    adjustFail = false;

    // process TX data; assume no copy, discard 
    pendTxEnd = pendTxBeg = 0;
    txUnackOffs = 0;
    while(newTxBuff != NULL)
    {
        if((uvFlags & (uint32_t)TCP_ADJUST_PRESERVE_TX) != 0U)
        {
            // calculate the pending data in the TX buffer 
            uint8_t*    txHead;
            uint8_t*    srcOffs = NULL;

            txHead = pSkt->txHead;
            
            if(txHead < pSkt->txTail)
            {
                pendTxEnd = (uint16_t)((uintptr_t)pSkt->txEnd - (uintptr_t)pSkt->txTail);
                pendTxBeg = (uint16_t)((uintptr_t)txHead - (uintptr_t)pSkt->txStart);
                srcOffs = pSkt->txStart;
            }
            else if(txHead > pSkt->txTail)
            {
                pendTxEnd = 0;
                pendTxBeg = (uint16_t)((uintptr_t)txHead - (uintptr_t)pSkt->txTail);
                srcOffs = pSkt->txTail;
            }
            else
            {
                /* Do Nothing */
            }

            if((pendTxEnd + pendTxBeg) != 0U)
            {   // need data copying
                if(pendTxEnd + pendTxBeg > wMinTXSize)
                {   // fail, not enough room
                    adjustFail = true;
                    break;
                }

                if(pendTxEnd != 0U)
                {
                    (void)TCPIP_Helper_Memcpy(newTxBuff, pSkt->txTail, (size_t)pendTxEnd);
                }
                if(pendTxBeg != 0U)
                {
                    (void)TCPIP_Helper_Memcpy(newTxBuff + pendTxEnd,  srcOffs, (size_t)pendTxBeg);
                }


                txUnackOffs = (uint16_t)((uintptr_t)pSkt->txUnackedTail - (uintptr_t)pSkt->txTail);
                if(pSkt->txUnackedTail < pSkt->txTail)
                {
                    txUnackOffs += (uint16_t)((uintptr_t)pSkt->txEnd - (uintptr_t)pSkt->txStart);
                }
            }
        }
        break;
    }

    // process the RX data
    // assume no copy, discard 
    avlblRxEnd = avlblRxBeg = 0;
    while(adjustFail != true && newRxBuff != NULL)
    {
        if((uvFlags & (uint32_t)TCP_ADJUST_PRESERVE_RX) != 0U)
        {   // calculate alavilable data in the RX buffer
            uint8_t*    rxHead;
            uint8_t*    srcOffs = NULL;

            rxHead = pSkt->rxHead;

            // preserve out-of-order pending data
            if(pSkt->sHoleSize != -1)
            {
                rxHead += pSkt->sHoleSize + (int32_t)pSkt->wFutureDataSize;
                if(rxHead > pSkt->rxEnd)
                {
                    rxHead -= pSkt->rxEnd - pSkt->rxStart + 1;
                }
            }

            if(rxHead > pSkt->rxTail)
            {
                avlblRxEnd = 0;
                avlblRxBeg = (uint16_t)((uintptr_t)rxHead - (uintptr_t)pSkt->rxTail);
                srcOffs = pSkt->rxTail;
            }
            else if(rxHead < pSkt->rxTail)
            {
                avlblRxEnd = (uint16_t)((uintptr_t)pSkt->rxEnd + 1U - (uintptr_t)pSkt->rxTail);
                avlblRxBeg = (uint16_t)((uintptr_t)rxHead - (uintptr_t)pSkt->rxStart);
                srcOffs = pSkt->rxStart;
            }
            else
            {
                /* Do Nothing */
            }

            if((avlblRxEnd + avlblRxBeg) != 0U)
            {   // need data copying

                if(avlblRxEnd + avlblRxBeg > wMinRXSize)
                {   // fail, not enough room
                    adjustFail = true;
                    break;
                }

                if(avlblRxEnd != 0U)
                {
                    (void)TCPIP_Helper_Memcpy(newRxBuff, pSkt->rxTail, (size_t)avlblRxEnd);
                }
                if(avlblRxBeg != 0U)
                {
                    (void)TCPIP_Helper_Memcpy(newRxBuff + avlblRxEnd, srcOffs, (size_t)avlblRxBeg);
                }
            }
        }

        break;
    }

    if(adjustFail == true)
    {
        (void) TCPIP_HEAP_Free(tcpHeapH, newRxBuff);
        (void) TCPIP_HEAP_Free(tcpHeapH, newTxBuff);
        return false;
    }

    // success

    // adjust new TX pointers
    if(newTxBuff != NULL)
    {
        (void) TCPIP_HEAP_Free(tcpHeapH, (void*)pSkt->txStart);
        pSkt->txStart =  newTxBuff;
        pSkt->txEnd = newTxBuff + wMinTXSize + 1;
        pSkt->txTail = pSkt->txStart;
        pSkt->txHead = pSkt->txStart + (pendTxEnd + pendTxBeg);
        pSkt->txUnackedTail = pSkt->txTail + txUnackOffs;
        F_TCPSetHalfFlushFlag(pSkt);
    }

    // adjust new RX pointers
    if(newRxBuff != NULL)
    {
        (void) TCPIP_HEAP_Free(tcpHeapH, (void*)pSkt->rxStart);
        pSkt->rxStart = newRxBuff;
        pSkt->rxEnd = newRxBuff + wMinRXSize;
        pSkt->rxTail = pSkt->rxStart;
        pSkt->rxHead = pSkt->rxStart + (avlblRxEnd + avlblRxBeg);
    }

    // Send a window update to notify remote node of change
    if((newTxBuff != NULL) || (newRxBuff != NULL))
    {
        if(pSkt->smState == (uint8_t)TCPIP_TCP_STATE_ESTABLISHED)
        {
            (void) F_TcpSend(pSkt, ACK, SENDTCP_RESET_TIMERS);
        }
    }
    

    return true;
}
#endif  // (TCPIP_TCP_DYNAMIC_OPTIONS != 0)


/*
  Function:
    bool TCPIP_TCP_SocketNetSet(TCP_SOCKET hTCP, TCPIP_NET_HANDLE hNet, bool persistent)

  Summary:
    Sets the interface for an TCP socket

  Description:
    This function sets the network interface for an TCP socket

  Precondition:
    TCP socket should have been opened with TCPIP_TCP_ClientOpen()/TCPIP_TCP_ServerOpen().
    hTCP - valid socket

  Parameters:
    hTCP        - The TCP socket
    hNet        - interface handle.
    persistent  - if true: 
                    when the socket connection is closed and it listens again, it will retain this network interface setting.
                    The same behavior is obtained by opening socket with TCPIP_TCP_ServerOpen() with a 
                    valid localAddress parameter
                - if false:
                    when a server socket that was created using TCPIP_TCP_ServerOpen() with localAddress == 0
                    closes the connection, the socket will re-listen on any interface

  Returns:
    - true  - Indicates success
    - false - Indicates failure

  Remarks:
    A NULL hNet can be passed (0) so that the current network interface selection 
    will be cleared.

    The persistent setting is applicable only to server sockets, as these sockets return to listen mode when a connection is closed.
    When a client socket connection is closed, the socket is destroyed and no information is maintained.
 */
bool TCPIP_TCP_SocketNetSet(TCP_SOCKET hTCP, TCPIP_NET_HANDLE hNet, bool persistent)
{
    TCPIP_NET_IF* pNetIf;
    TCB_STUB* pSkt = F_TcpSocketChk(hTCP); 
    
    if(pSkt != NULL)
    {
        if(hNet != NULL)
        {
            pNetIf = TCPIPStackHandleToNet(hNet);
            if(pNetIf == NULL)
            {   // wrong interface
                return false;
            }
        }
        else
        {
            pNetIf = NULL;
        }

        pSkt->pSktNet = pNetIf;
        if(persistent)
        {
            pSkt->flags.openBindIf = 1;
        }

#if defined (TCPIP_STACK_USE_IPV6)
        if(pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
        {
            if(pSkt->pV6Pkt == NULL)
            {
                return false;
            }

            pSkt->pV6Pkt->netIfH = pNetIf;
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)

        return true;
    }

    return false;
}

/*****************************************************************************
  Function:
    TCPIP_NET_IF* TCPIP_TCP_SocketNetGet(TCP_SOCKET hTCP)

  Summary:
    Gets the MAC interface of an TCP socket
    
  Description:
    This function returns the MAC interface id of an TCP socket

  Precondition:
    TCP socket should have been opened with F_TCP_Open().
    hTCP - valid socket

  Parameters:
    hTCP - The TCP socket
    
  Returns:
    None.
  ***************************************************************************/
TCPIP_NET_HANDLE  TCPIP_TCP_SocketNetGet(TCP_SOCKET hTCP)
{
    TCB_STUB* pSkt = F_TcpSocketChk(hTCP); 
    
    if(pSkt == NULL)
    {
        return NULL;
    }

    return pSkt->pSktNet;
}

// allocates a new ephemeral port number
// returns 0 on error
static TCP_PORT F_TCP_EphemeralPortAllocate(void)
{
    uint32_t    num_ephemeral;
    uint32_t    count;
    TCP_PORT    next_ephemeral;


    count = num_ephemeral = (uint32_t)TCPIP_TCP_LOCAL_PORT_END_NUMBER - (uint32_t)TCPIP_TCP_LOCAL_PORT_START_NUMBER + 1U;

#if defined(TCPIP_TCP_DISABLE_CRYPTO_USAGE) && (TCPIP_TCP_DISABLE_CRYPTO_USAGE != false)
    next_ephemeral = (uint16_t)((uint32_t)TCPIP_TCP_LOCAL_PORT_START_NUMBER + (SYS_RANDOM_PseudoGet() % num_ephemeral));
#else
    next_ephemeral = (uint16_t)((uint32_t)TCPIP_TCP_LOCAL_PORT_START_NUMBER + (SYS_RANDOM_CryptoGet() % num_ephemeral));
#endif  // defined(TCPIP_TCP_DISABLE_CRYPTO_USAGE) && (TCPIP_TCP_DISABLE_CRYPTO_USAGE != false)

    while(count-- != 0U)
    {
        if(F_TCP_PortIsAvailable(next_ephemeral))
        {
            return (TCP_PORT)next_ephemeral;
        }

        if (next_ephemeral == (uint16_t)TCPIP_TCP_LOCAL_PORT_END_NUMBER)
        {
            next_ephemeral = (uint16_t)TCPIP_TCP_LOCAL_PORT_START_NUMBER;
        }
        else
        {
            next_ephemeral++;
        }
    }

    return 0;   // not found

}

static bool F_TCP_PortIsAvailable(TCP_PORT port)
{
    unsigned int sktIx;
    TCB_STUB* pSkt; 

    // Find an available socket that matches the specified socket type
    for(sktIx = 0; sktIx < TcpSockets; sktIx++)
    {
        pSkt = TCBStubs[sktIx];
        // Sockets that are in use will be in a non-closed state
        if((pSkt != NULL) && pSkt->localPort == port)
        {
            return false;
        }
    }

    return true;
}

/*****************************************************************************
  Function:
    TCPIP_TCP_Bind(TCP_SOCKET s, IP_ADDRESS_TYPE addType, TCP_PORT localPort,  IP_MULTI_ADDRESS* localAddress)

  Summary:
    Bind a socket to a local address
    This function is meant for client sockets.
    It is similar to TCPIP_TCP_SocketNetSet() that assigns a specific source interface for a socket.
    If localPort is 0 the stack will assign a unique local port

  Description:
    Sockets don't need specific binding, it is done automatically by the stack
    However, specific binding can be requested using these functions.
    Works for both client and server sockets.
    The call will fail if the socket is already bound/connected
  ***************************************************************************/
bool TCPIP_TCP_Bind(TCP_SOCKET hTCP, IP_ADDRESS_TYPE addType, TCP_PORT localPort,  IP_MULTI_ADDRESS* localAddress)
{
    const TCPIP_NET_IF* pSktIf;
    TCB_STUB* pSkt; 

    pSkt = F_TcpSocketChk(hTCP); 
    if(pSkt == NULL || F_TCP_IsConnected(pSkt))
    {
        return false;
    }

    if(pSkt->addType != (uint8_t)IP_ADDRESS_TYPE_ANY && pSkt->addType != (uint8_t)addType)
    {   // address type mismatch
        return false;
    }

    // check for valid address

    if(localAddress != NULL)
    {
        pSktIf = NULL;
#if defined (TCPIP_STACK_USE_IPV4)
        if (addType == IP_ADDRESS_TYPE_IPV4)
        {
            pSktIf = TCPIP_STACK_IPAddToNet(&localAddress->v4Add, false);
        }
#else
        if (addType == IP_ADDRESS_TYPE_IPV6)
        {
            pSktIf = TCPIPStackIPv6AddToNet(&localAddress->v6Add, IPV6_ADDR_TYPE_UNICAST, false);
        }
#endif  // defined (TCPIP_STACK_USE_IPV4)

        if(pSktIf == NULL)
        {    // no such interface
            return false;
        }
    }
    else
    {   // maintain the old interface
        pSktIf = pSkt->pSktNet;
    }

    if(localPort == 0U)
    {
        localPort = F_TCP_EphemeralPortAllocate();
        if(localPort == 0U)
        {
            return false;
        }
    }
    else if(localPort != pSkt->localPort && !F_TCP_PortIsAvailable(localPort))
    {
        return false;
    }
    else
    {
        /* Do Nothing */
    }

    // success; bind
    pSkt->addType = (uint8_t)addType;
    F_TcpSocketBind(pSkt, pSktIf, localAddress);
    pSkt->localPort = localPort;

    // recalculate the MYTCBStub remote hash value
    if(pSkt->flags.bServer != 0U)
    {   // server socket
        pSkt->remoteHash = localPort;
    }
    else
    {   // client socket
        pSkt->remoteHash = F_TCP_ClientIPV4RemoteHash(&pSkt->destAddress, pSkt);
    }

    return true;
}

/*****************************************************************************
  Function:
    bool TCPIP_TCP_RemoteBind(TCP_SOCKET hTCP, IP_ADDRESS_TYPE addType, TCP_PORT remotePort,  IP_MULTI_ADDRESS* remoteAddress)

  Summary:
    Bind a socket to a remote address
    This function is meant for server sockets.

  Description:
    Sockets don't need specific remote binding, they should accept connections on any incoming interface.
    Thus the binding is done automatically by the stack.
    However, specific remote binding can be requested using these functions.
    For a server socket it can be used to restrict accepting connections from  a specific remote host.
    For a client socket it will just change the default binding done when the socket was opened.
    The call will fail if the socket is already connected.
  ***************************************************************************/
bool TCPIP_TCP_RemoteBind(TCP_SOCKET hTCP, IP_ADDRESS_TYPE addType, TCP_PORT remotePort,  IP_MULTI_ADDRESS* remoteAddress)
{
    TCB_STUB* pSkt = F_TcpSocketChk(hTCP); 

    if(pSkt != NULL && !F_TCP_IsConnected(pSkt))
    {

        if(remoteAddress == NULL || TCPIP_TCP_DestinationIPAddressSet(hTCP, addType, remoteAddress) == true)
        {
            if(remotePort != 0U)
            {
                pSkt->remotePort = remotePort;
            }
            return true;
        }
    }

    return false;
}

// Allows setting options to a socket like Rx/Tx buffer size, etc
#if (TCPIP_TCP_DYNAMIC_OPTIONS != 0)
bool TCPIP_TCP_OptionsSet(TCP_SOCKET hTCP, TCP_SOCKET_OPTION option, void* optParam)
{
    bool res;
    TCB_STUB* pSkt = F_TcpSocketChk(hTCP); 

    if(pSkt != NULL)
    {
        union
        {
            void*   optParam;
            uintptr_t   paramPtr;
            unsigned int uparam;
            uint32_t    param32;
            uint16_t    param16;
            uint8_t     param8;
        }U_OPT_PARAM;

        U_OPT_PARAM.optParam = optParam;
        res = true;
        switch(option)
        {
            case TCP_OPTION_LINGER:
                if(optParam != NULL)
                {
                    TCP_OPTION_LINGER_DATA* pLData = (TCP_OPTION_LINGER_DATA*)optParam;
                    pSkt->flags.nonLinger = pLData->lingerEnable != false ? 0U : 1U;
                    pSkt->flags.nonGraceful = pLData->gracefulEnable != false ? 0U : 1U;
                }
                else
                {
                    res = false;
                }
                break;
                
            case TCP_OPTION_KEEP_ALIVE:
                if(optParam != NULL)
                {
                    TCP_OPTION_KEEP_ALIVE_DATA* pKData = (TCP_OPTION_KEEP_ALIVE_DATA*)optParam;
                    pSkt->flags.keepAlive = pKData->keepAliveEnable != false ? 1U : 0U;
                    if(pSkt->flags.keepAlive != 0U)
                    {
                        pSkt->keepAliveCount = 0;
                        pSkt->keepAliveTmo = (pKData->keepAliveTmo != 0U) ? pKData->keepAliveTmo : (uint16_t)TCPIP_TCP_KEEP_ALIVE_TIMEOUT;
                        pSkt->keepAliveLim = (pKData->keepAliveUnackLim != 0U) ? pKData->keepAliveUnackLim : (uint8_t)TCPIP_TCP_MAX_UNACKED_KEEP_ALIVES;
                    }
                }
                else
                {
                    res = false;
                }
                break;

            case TCP_OPTION_RX_BUFF:
                if(U_OPT_PARAM.param32 > (uint32_t)TCP_MAX_RX_BUFF_SIZE)
                {
                   return false;
                } 
                res = TCPIP_TCP_FifoSizeAdjust(hTCP, U_OPT_PARAM.param16, 0, (TCP_ADJUST_FLAGS)((uint32_t)TCP_ADJUST_RX_ONLY | (uint32_t)TCP_ADJUST_PRESERVE_RX));
                break;

            case TCP_OPTION_TX_BUFF:
                if(U_OPT_PARAM.param32 > (uint32_t)TCP_MAX_RX_BUFF_SIZE)
                {
                   return false;
                } 
                res = TCPIP_TCP_FifoSizeAdjust(hTCP, 0, U_OPT_PARAM.param16, (TCP_ADJUST_FLAGS)((uint32_t)TCP_ADJUST_TX_ONLY | (uint32_t)TCP_ADJUST_PRESERVE_TX));
                break;

            case TCP_OPTION_NODELAY:
                pSkt->flags.forceFlush = (U_OPT_PARAM.paramPtr != 0U) ? 1U : 0U;
                break;

            case TCP_OPTION_THRES_FLUSH:
                pSkt->flags.halfThresType = U_OPT_PARAM.param8;
                break;

            case TCP_OPTION_DELAY_SEND_ALL_ACK:
                pSkt->flags.delayAckSend = (U_OPT_PARAM.param16 != 0U) ? 1U : 0U;
                break;
                
            case TCP_OPTION_TX_TTL:
                pSkt->ttl = U_OPT_PARAM.param8;
                break;
                
            case TCP_OPTION_TOS:
                pSkt->tos = U_OPT_PARAM.param8;
                break;
                
            case TCP_OPTION_DSCP:
                pSkt->dscp = U_OPT_PARAM.param8;
                break;
                
            default:
                res = false;   // not supported option
                break;
        }
    }
    else
    {
        res = false;
    }

    return res;
}

bool TCPIP_TCP_OptionsGet(TCP_SOCKET hTCP, TCP_SOCKET_OPTION option, void* optParam)
{
    bool res;
    TCB_STUB* pSkt = F_TcpSocketChk(hTCP); 

    if((pSkt != NULL) && (optParam != NULL))
    {
        res = true;
        switch(option)
        {
            case TCP_OPTION_LINGER:
                {
                    TCP_OPTION_LINGER_DATA* pLData = (TCP_OPTION_LINGER_DATA*)optParam;
                    pLData->lingerEnable = pSkt->flags.nonLinger == 0U;
                    pLData->gracefulEnable = pSkt->flags.nonGraceful == 0U;
                    pLData->lingerTmo = 0;  // not supported
                }
                break;

            case TCP_OPTION_KEEP_ALIVE:
                {
                    TCP_OPTION_KEEP_ALIVE_DATA* pKData = (TCP_OPTION_KEEP_ALIVE_DATA*)optParam;
                    pKData->keepAliveEnable = pSkt->flags.keepAlive != 0U;
                    pKData->keepAliveTmo = pSkt->keepAliveTmo;
                    pKData->keepAliveUnackLim = pSkt->keepAliveLim;
                }
                break;
                
            case TCP_OPTION_RX_BUFF:
                *(uint16_t*)optParam = (uint16_t)((uintptr_t)pSkt->rxEnd - (uintptr_t)pSkt->rxStart);
                break;

            case TCP_OPTION_TX_BUFF:
                *(uint16_t*)optParam = (uint16_t)((uintptr_t)pSkt->txEnd - (uintptr_t)pSkt->txStart + 1U);
                break;

            case TCP_OPTION_NODELAY:
                *(bool*)optParam = pSkt->flags.forceFlush != 0U;
                break;

            case TCP_OPTION_THRES_FLUSH:
                *(TCP_OPTION_THRES_FLUSH_TYPE*)optParam = (TCP_OPTION_THRES_FLUSH_TYPE)pSkt->flags.halfThresType;
                break;

            case TCP_OPTION_DELAY_SEND_ALL_ACK:
                *(bool*)optParam = pSkt->flags.delayAckSend != 0U;
                break;

            case TCP_OPTION_TX_TTL:
                *(uint8_t*)optParam = pSkt->ttl;
                break;

             case TCP_OPTION_TOS:
                *(uint8_t*)optParam = pSkt->tos;
                break;
                
             case TCP_OPTION_DSCP:
                *(uint8_t*)optParam = pSkt->dscp;
                break;
                
            default:
                res = false;   // not supported option
                break;
        }
    }
    else
    {
        res = false;
    }

    return res;
}
#endif  // (TCPIP_TCP_DYNAMIC_OPTIONS != 0)


bool TCPIP_TCP_DestinationIPAddressSet(TCP_SOCKET s, IP_ADDRESS_TYPE addType, IP_MULTI_ADDRESS* remoteAddress)
{
    TCB_STUB* pSkt; 

    if(remoteAddress == NULL)
    {
        return false;
    }

    pSkt = F_TcpSocketChk(s); 

    while(pSkt != NULL && pSkt->addType == (uint8_t)addType && !F_TCP_IsConnected(pSkt))
    {
#if defined (TCPIP_STACK_USE_IPV6)
        if (pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
        {
            if(pSkt->pV6Pkt != NULL)
            {
                TCPIP_IPV6_DestAddressSet (pSkt->pV6Pkt, &remoteAddress->v6Add);
                return true;
            }
            break;
        }
#endif  // defined (TCPIP_STACK_USE_IPV6)

#if defined (TCPIP_STACK_USE_IPV4)
        if (pSkt->addType == (uint8_t)IP_ADDRESS_TYPE_IPV4)
        {
            pSkt->destAddress.Val = remoteAddress->v4Add.Val;
            return true;
        }
#endif  // defined (TCPIP_STACK_USE_IPV4)
        break;
    }

    return false;
}


// sets the source IP address of a packet
bool TCPIP_TCP_SourceIPAddressSet(TCP_SOCKET s, IP_ADDRESS_TYPE addType, IP_MULTI_ADDRESS* localAddress)
{
    TCB_STUB* pSkt = F_TcpSocketChk(s); 
    if(pSkt == NULL || F_TCP_IsConnected(pSkt))
    {
        return false;
    }

    return F_TCPSetSourceAddress(pSkt, addType, localAddress);
}


// calculates a client socket remote hash value
static uint16_t F_TCP_ClientIPV4RemoteHash(const IPV4_ADDR* pAdd, TCB_STUB* pSkt)
{
    return (pAdd->w[1] + pAdd->w[0] + pSkt->remotePort) ^ pSkt->localPort;
}


static void F_TCP_PayloadSet(TCB_STUB * pSkt, void* pPkt, uint8_t* payload1, uint16_t len1, uint8_t* payload2, uint16_t len2)
{
    switch(pSkt->addType)
    {
#if defined (TCPIP_STACK_USE_IPV6)
        case (uint8_t)IP_ADDRESS_TYPE_IPV6:
            if(payload1 != NULL)
            {
                (void) TCPIP_IPV6_PayloadSet((IPV6_PACKET*)pPkt, payload1, len1);
            }
            if(payload2 != NULL)
            {
                (void) TCPIP_IPV6_PayloadSet((IPV6_PACKET*)pPkt, payload2, len2);
            }
            break;
#endif  // defined (TCPIP_STACK_USE_IPV6)

#if defined (TCPIP_STACK_USE_IPV4)
        case (uint8_t)IP_ADDRESS_TYPE_IPV4:
            F_Tcpv4LinkDataSeg((TCP_V4_PACKET*)pPkt, payload1, len1, payload2, len2);
            break;
#endif  // defined (TCPIP_STACK_USE_IPV4)

        default:
            /* Do Nothing */
            break;
    }
}


static bool F_TCP_Flush(TCB_STUB * pSkt, void* pPkt, uint16_t hdrLen, uint16_t loadLen)
{
    bool res;

    switch(pSkt->addType)
    {
#if defined (TCPIP_STACK_USE_IPV6)
        case (uint8_t)IP_ADDRESS_TYPE_IPV6:
            res = TCPIP_IPV6_Flush((IPV6_PACKET*)pPkt) >= 0; // sent or queued
            break;                                                             
#endif  // defined (TCPIP_STACK_USE_IPV6)

#if defined (TCPIP_STACK_USE_IPV4)
        case (uint8_t)IP_ADDRESS_TYPE_IPV4:
            res = F_TCPv4Flush(pSkt, (IPV4_PACKET*)pPkt, hdrLen, loadLen);
            break;
#endif  // defined (TCPIP_STACK_USE_IPV4)

        default:
            res = false;
            break;
    }

    return res;
}


TCPIP_TCP_SIGNAL_HANDLE TCPIP_TCP_SignalHandlerRegister(TCP_SOCKET s, TCPIP_TCP_SIGNAL_TYPE sigMask, TCPIP_TCP_SIGNAL_FUNCTION handler, const void* hParam)
{
    TCPIP_TCP_SIGNAL_FUNCTION sHandler = NULL;

    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    if(handler != NULL)
    {
        TCB_STUB* pSkt = F_TcpSocketChk(s);
        if(pSkt != NULL && pSkt->sigHandler == NULL)
        {
            pSkt->sigHandler = handler;
            pSkt->sigMask = (uint16_t)sigMask;
            pSkt->sigParam = hParam;
            sHandler = handler;
            // Note: this may change if multiple notfication handlers required 
        }
    }
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);

    return FC_SigFunc2SigHndl(sHandler);
}

bool TCPIP_TCP_SignalHandlerDeregister(TCP_SOCKET s, TCPIP_TCP_SIGNAL_HANDLE hSig)
{
    bool res = false;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    TCB_STUB* pSkt = F_TcpSocketChk(s);

    if(pSkt != NULL)
    {  
        if(pSkt->sigHandler == FC_SigHndl2SigFunc(hSig))
        {
            pSkt->sigHandler = NULL;
            pSkt->sigMask = 0;
            res = true;
        }
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return res;
}

bool TCPIP_TCP_IsReady(void)
{
#if (TCPIP_TCP_QUIET_TIME != 0)
    return tcpQuietDone;
#else
    return true;
#endif  // (TCPIP_TCP_QUIET_TIME != 0)
}

// external packet processing
#if (TCPIP_TCP_EXTERN_PACKET_PROCESS != 0)
TCPIP_TCP_PROCESS_HANDLE TCPIP_TCP_PacketHandlerRegister(TCPIP_TCP_PACKET_HANDLER pktHandler, const void* handlerParam)
{
    TCPIP_TCP_PACKET_HANDLER userHandler = NULL;

    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(tcpPktHandler == NULL)
    {
        tcpPktHandlerParam = handlerParam;
        tcpPktHandler = pktHandler;
        userHandler = pktHandler;
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return FC_PktHndl2ProcHndl(userHandler);
}

bool TCPIP_TCP_PacketHandlerDeregister(TCPIP_TCP_PROCESS_HANDLE procHandle)
{
    bool res = false;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(tcpPktHandler == FC_ProcHndl2PktHndl(procHandle))
    {
        tcpPktHandler = NULL;
        res = true;
    } 

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return res;
}
#else
TCPIP_TCP_PROCESS_HANDLE TCPIP_TCP_PacketHandlerRegister(TCPIP_TCP_PACKET_HANDLER pktHandler, const void* handlerParam)
{
    return NULL;
}
bool TCPIP_TCP_PacketHandlerDeregister(TCPIP_TCP_PROCESS_HANDLE procHandle)
{
    return false;
}
#endif  // (TCPIP_TCP_EXTERN_PACKET_PROCESS != 0)



#endif //#if defined(TCPIP_STACK_USE_TCP)

