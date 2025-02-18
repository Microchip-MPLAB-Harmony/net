/*******************************************************************************
  TCP/IP iperf implementation

  Summary:
    Runs iperf client and server
    
  Description:
    - Implements iperf benchmarking
*******************************************************************************/

/*
Copyright (C) 2016-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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


#include <string.h> 

#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_IPERF

#include "tcpip/src/tcpip_private.h"


#if defined(TCPIP_STACK_USE_IPERF)

#include "system/command/sys_command.h"

//****************************************************************************
// CONSTANTS (Defines and enums)
//****************************************************************************

#define UDP_FIN_RETRANSMIT_COUNT        10U     // iperf retransmits 10 times the last UDP packet,
#define UDP_FIN_RETRANSMIT_PERIOD       10U     // at 10ms apart.


// TCP Maximum Segment Size - MSS;
#define IPERF_TCP_MSS  (uint16_t)TCPIP_TCP_MAX_SEG_SIZE_TX


typedef enum
{
    UDP_PROTOCOL = 1,
    TCP_PROTOCOL
} tIperfProto;

enum {
    IPERF_STANDBY_STATE=1,

    IPERF_RX_START_STATE,
    IPERF_UDP_RX_STATE,
    IPERF_UDP_RX_DRAIN_STATE,
    IPERF_UDP_RX_DONE_STATE,
    IPERF_TCP_RX_LISTEN_STATE,
    IPERF_TCP_RX_STATE,
    IPERF_TCP_RX_DONE_STATE,
    IPERF_RX_DONE_STATE,

    IPERF_TX_START_STATE,
    IPERF_TX_ARP_RESOLVE_STATE,

    IPERF_TCP_TX_OPEN_STATE,
    IPERF_TCP_TX_CONNECT_STATE,
    IPERF_TCP_TX_SEGMENT_STATE,
    IPERF_TCP_TX_DONE_STATE,

    IPERF_UDP_TX_OPEN_STATE,
    IPERF_UDP_TX_DATAGRAM_STATE,
    IPERF_UDP_TX_DONE_STATE
};

typedef enum
{
    INTERVAL_REPORT,
    SUBTOTAL_REPORT,
    SESSION_REPORT
} tIperfReport;

typedef enum
{
    IPERF_TX_WAIT   = 0,    // wait some more for transmitting
    IPERF_TX_OK,            // can go ahead and transmit
    IPERF_TX_FAIL,          // cannot get TX resources, abort
}tIperfTxResult;

typedef enum
{
    IPERF_TOS_VO   = 192,    // VOICE
    IPERF_TOS_VI    = 160,    // VIDEO
    IPERF_TOS_BK   = 32,      // BACKGROUND
    IPERF_TOS_BE   = 0,    // BEST EFFORT

}tIperfTOSvalues;

//****************************************************************************
// LOCAL DATA TYPES                                                             
//****************************************************************************

/* tIperfState */
typedef struct
{
    uint32_t        mInterval;      // -i
    uint32_t        mAmount;        // -n
    uint32_t        mDuration;      // -t. Default = 10*TICK_SECOND msec
    uint32_t        mDatagramSize;  // -l
    tIperfProto     mProtocol;      // -b or -u
    uint16_t        mServerPort;    // -p
    uint8_t         mTypeOfService; // -S
    
    uint32_t        mTxRate;        // -b or
                                // -x: NONE-STANDARD IPERF OPTION. Max Tx bps rate for TCP.

    double          totalLen; // mTotalLen
    long            pktId;      // datagramID
    long            lastPktId; // lastDatagramID
    uint32_t        errorCount;
    uint32_t        outofOrder;

    TCP_SOCKET tcpServerSock;
    TCP_SOCKET tcpClientSock;

    UDP_SOCKET udpSock;

    TCP_SOCKET_INFO  remoteSide;
    TCPIP_MAC_ADDR   remoteMACAddr; // remote host MAC address
    uint16_t         localPort;
    IP_MULTI_ADDRESS localAddr;     // local address/interface to use

    //struct sockaddr_in remoteAddr;
    // int      remoteAddrlen;
    // tSocketAddr remoteAddr;

    // Calculated packet period, in msec, to reflect the target bit rate.
    uint32_t        mPktPeriod;

    uint32_t        startTime;
    uint32_t        stopTime;
    uint32_t        nextTxTime;
    //uint32_t      remoteStartTime;
    //uint32_t      remoteStopTime;

    uint32_t        pktCount;


    uint32_t        lastCheckPktCount;  // Check if pktCount changes within mInterval; or kIperfRxTimeOut.
    long            lastCheckPktId;
    uint32_t        lastCheckErrorCount;
    uint32_t        lastCheckTotalLen;
    uint32_t        lastCheckTime;

//  uint32_t        mPendingACK;        // number of outstanding TCP ACKs
//  uint8_t     mRetransmit;

    uint32_t      timer;
    uint16_t      remainingTxData;
    uint16_t      availUDPTxSpace;


    uint32_t    txWaitTick;

    uint32_t    txBuffSize;
    uint32_t    rxBuffSize;

    TCPIP_SIGNAL_HANDLE  signalHandle; 

    // console that invoked iperf
    SYS_CMD_DEVICE_NODE* pCmdIO;
    // interface to use
    TCPIP_NET_HANDLE pNetIf;
    // 
    uint8_t     nAttempts;
    uint8_t     statusReported;
    uint8_t     state;
    uint8_t     stopRequested;

    uint8_t     isLastTransmit;
    uint8_t     mServerMode;    // -s or -c
    uint16_t    mMSS;           // -M

    uint8_t     sockWaitToSend;
    uint8_t     waitCount;

} tIperfState;



//
// Data structure used by iperf protocol
//

#define HEADER_VERSION1 0x80000000U

typedef struct
{
    long     id;
    uint32_t tv_sec;
    uint32_t tv_usec;
} tIperfPktInfo;    // In the original Iperf, this is the "UDP_datagram" structure.

// tUDP_datagram
typedef struct
{
    uint32_t flags;
    uint32_t total_len1;
    uint32_t total_len2;
    uint32_t stop_sec;
    uint32_t stop_usec;
    uint32_t error_cnt;
    uint32_t outorder_cnt;
    uint32_t datagrams;
    uint32_t jitter1;
    uint32_t jitter2;
} tServerHdr;

typedef struct
{
    uint32_t flags;
    uint32_t numThreads;
    uint32_t mPort;
    uint32_t bufferlen;
    uint32_t mWinBand;
    uint32_t mAmount;
} tClientHdr;

//****************************************************************************
// LOCAL GLOBALS                                                             
//****************************************************************************

#define IPERF_HEADER_BUFFER   (sizeof(tIperfPktInfo) + sizeof(tServerHdr))

static union
{
    tIperfPktInfo   pktInfo[1];
    uint8_t         buff[ IPERF_HEADER_BUFFER ];
}g_bfr;



// the transfer buffer size
// make it bigger than the Ethernet packet size for efficient reading from the socket
#define IPERF_TXFER_BUFFER_SIZE    1600
static uint8_t txfer_buffer[IPERF_TXFER_BUFFER_SIZE];


static tIperfState gIperfState[TCPIP_IPERF_MAX_INSTANCES];

static size_t   nIperfSessions = 0U;        // number of currently running instances

static int      iperfInitCount = 0;         // iperf module initialization count

static uint32_t sysFreq = 0U; 

static void CommandIperfStart(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void CommandIperfStop(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void CommandIperfNetIf(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void CommandIperfSize(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
// Iperf command table
static const SYS_CMD_DESCRIPTOR    iperfCmdTbl[]=
{
    {"iperf",       &CommandIperfStart,  ":  <iperf> start cmd"},
    {"iperfk",      &CommandIperfStop,   ": <iperfk> kill cmd"},
    {"iperfi",      &CommandIperfNetIf,  ": <iperfi address>  interface cmd"},
    {"iperfs",      &CommandIperfSize,   ": <iperfs tx/rx size> tx/rx size cmd"}
};


//****************************************************************************
// LOCAL Prototypes                                                             
//****************************************************************************
#if defined(TCPIP_STACK_USE_UDP)
static void StateMachineUdpTxDone(tIperfState* pIState);
static void StateMachineUdpTxDatagram(tIperfState* pIState);
static uint16_t UdpTxFillDatagram(tIperfState* pIState);
static void StateMachineUDPTxOpen(tIperfState* pIState);
static void StateMachineUdpRxDone(tIperfState* pIState);
static void StateMachineUdpRxDrain(tIperfState* pIState);
static void StateMachineUdpRx(tIperfState* pIState);
#endif  // defined(TCPIP_STACK_USE_UDP)

#if defined(TCPIP_STACK_USE_TCP)
static void StateMachineTcpTxDone(tIperfState* pIState);
static void StateMachineTcpTxSegment(tIperfState* pIState);
static void TcpTxFillSegment(tIperfState* pIState);
static void StateMachineTCPTxOpen(tIperfState* pIState);
static void StateMachineTcpRxDone(tIperfState* pIState);
static void StateMachineTcpRx(tIperfState* pIState);
static void StateMachineTcpListen(tIperfState* pIState);
static void StateMachineTCPTxConnect(tIperfState* pIState);
#endif  // defined(TCPIP_STACK_USE_TCP)

static void GenericTxDone(tIperfState* pIState);
static void GenericTxEnd(tIperfState* pIState);
static tIperfTxResult GenericTxStart(tIperfState* pIState);
static void GenericTxHeaderPreparation(tIperfState* pIState, uint8_t *pData, bool isTheLastTransmit);
static void StateMachineTxArpResolve(tIperfState* pIState);
static void StateMachineTxStart(tIperfState* pIState);
static void StateMachineRxDone(tIperfState* pIState);
static void StateMachineRxStart(tIperfState* pIState);
static void ReportBW_Jitter_Loss(tIperfState* pIState, tIperfReport reportType);
static void ascii_to_u32s(char *ptr, uint32_t *values, uint8_t count);
static void ResetIperfCounters(tIperfState* pIState);

static void IperfSetState(tIperfState* pIState, int newState);

static void TCPIP_IPERF_Process(tIperfState* pIState);  

static void F_IperfTCPRxSignalHandler(TCP_SOCKET hTCP, TCPIP_NET_HANDLE hNet, TCPIP_TCP_SIGNAL_TYPE sigType, const void* param);
static void F_IperfUDPRxSignalHandler(TCP_SOCKET hTCP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param);

// helpers
static __inline__  tServerHdr*  __attribute__((always_inline)) FC_PktInfo2SrvHdr(tIperfPktInfo* pPktInfo)
{
    union
    {
        tIperfPktInfo* pPktInfo;
        tServerHdr*    pHdr;
    }U_PKT_INFO_SRV_HDR;

    U_PKT_INFO_SRV_HDR.pPktInfo = pPktInfo;
    return U_PKT_INFO_SRV_HDR.pHdr;
} 

static __inline__  tClientHdr*  __attribute__((always_inline)) FC_PktInfo2CliHdr(tIperfPktInfo* pPktInfo)
{
    union
    {
        tIperfPktInfo* pPktInfo;
        tClientHdr*    pHdr;
    }U_PKT_INFO_CLI_HDR;

    U_PKT_INFO_CLI_HDR.pPktInfo = pPktInfo;
    return U_PKT_INFO_CLI_HDR.pHdr;
} 

static __inline__  tIperfPktInfo*  __attribute__((always_inline)) FC_CliHdr2PktInfo(tClientHdr* pHdr)
{
    union
    {
        tClientHdr*    pHdr;
        tIperfPktInfo* pPktInfo;
    }U_CLI_HDR_PKT_INFO;

    U_CLI_HDR_PKT_INFO.pHdr = pHdr;
    return U_CLI_HDR_PKT_INFO.pPktInfo;
} 

static __inline__  tClientHdr*  __attribute__((always_inline)) FC_U8Ptr2CliHdr(uint8_t* pData)
{
    union
    {
        uint8_t* pData;
        tClientHdr*    pHdr;
    }U_U8_PTR_CLI_HDR;

    U_U8_PTR_CLI_HDR.pData = pData;
    return U_U8_PTR_CLI_HDR.pHdr;
} 

static __inline__  tIperfPktInfo*  __attribute__((always_inline)) FC_U8Ptr2PktInfo(uint8_t* pData)
{
    union
    {
        uint8_t* pData;
        tIperfPktInfo*    pPktInfo;
    }U_U8_PTR_PKT_INFO;

    U_U8_PTR_PKT_INFO.pData = pData;
    return U_U8_PTR_PKT_INFO.pPktInfo;
} 

static __inline__  double  __attribute__((always_inline)) FC_U32Val2Double(uint32_t u32Data)
{
    double dVal = (double)u32Data;
    return dVal;
} 

static __inline__  uint32_t  __attribute__((always_inline)) FC_Double2U32Val(double dVal)
{
    uint32_t u32Data = (uint32_t)dVal;
    return u32Data;
} 

static __inline__  float  __attribute__((always_inline)) FC_U32Val2Float(uint32_t u32Data)
{
    float fVal = (float)u32Data;
    return fVal;
} 

static __inline__  uint32_t  __attribute__((always_inline)) FC_Float2U32Val(float fVal)
{
    uint32_t u32Data = (uint32_t)fVal;
    return u32Data;
} 


//****************************************************************************
// Implementation: public functions
//****************************************************************************

bool TCPIP_IPERF_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* initData)
{
    if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }

    // stack init

    if(iperfInitCount == 0)
    {   // first time we run
        sysFreq = SYS_TMR_TickCounterFrequencyGet(); 
        
        size_t i;
        nIperfSessions = sizeof(gIperfState) / sizeof(*gIperfState);

        tIperfState* pIState = gIperfState;   
        for(i = 0; i < nIperfSessions; i++)
        {
            (void)memset( pIState, 0, sizeof(*pIState) );

            pIState->state = (uint8_t)IPERF_STANDBY_STATE;
            pIState->stopRequested = 0U;

            pIState->tcpClientSock = INVALID_SOCKET;
            pIState->tcpServerSock = INVALID_SOCKET;
            pIState->udpSock = INVALID_SOCKET;
            pIState->txBuffSize = (uint32_t)TCPIP_IPERF_TX_BUFFER_SIZE;
            pIState->rxBuffSize = (uint32_t)TCPIP_IPERF_RX_BUFFER_SIZE;
            pIState->waitCount = 0U;
            pIState->sockWaitToSend = 0U;
            pIState->mTypeOfService = 0xFF;

            pIState->signalHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_IPERF_Task, 0);
            if(pIState->signalHandle == NULL)
            {   // failed
                return false;
            }

            pIState++;
        }
        if(!SYS_CMD_ADDGRP(iperfCmdTbl, (int)(sizeof(iperfCmdTbl) / sizeof(*iperfCmdTbl)), "iperf", ": iperf commands"))
        {
            return false;
        }
    }

    iperfInitCount++;

    return true;
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_IPERF_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN) // interface down

    if(iperfInitCount > 0)
    {   // we're up and running
        if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--iperfInitCount == 0)
            {   // all closed
                // release resources
                size_t i;
                tIperfState* pIState = gIperfState;   
                for(i = 0; i < nIperfSessions; i++)
                {
                    if(pIState->signalHandle != NULL)
                    {
                        TCPIPStackSignalHandlerDeregister(pIState->signalHandle);
                        pIState->signalHandle = NULL;
                    }
                    pIState++;
                }
            }
        }
    }

}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

void TCPIP_IPERF_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != TCPIP_MODULE_SIGNAL_NONE)
    {
        size_t i = 0;
        // ASYNC or RX signals occurred
        tIperfState* pIState = gIperfState;   
        for(i = 0; i < nIperfSessions; i++)
        {
            TCPIP_IPERF_Process(pIState);
            pIState++;
        }
    }

}


// send a signal to the iperf module that data is available
// no manager alert needed since this normally results as a higher layer (TCP) signal
static void F_IperfTCPRxSignalHandler(TCP_SOCKET hTCP, TCPIP_NET_HANDLE hNet, TCPIP_TCP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_TCP_SIGNAL_RX_DATA)
    {
        (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}

// send a signal to the iperf module that data is available
// no manager alert needed since this normally results as a higher layer (TCP) signal
static void F_IperfUDPRxSignalHandler(TCP_SOCKET hTCP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_UDP_SIGNAL_RX_DATA)
    {
        (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}




static void TCPIP_IPERF_Process(tIperfState* pIState)   
{
    if (pIState->state == (uint8_t)IPERF_STANDBY_STATE)
    { 
       return;
    }

    switch ( pIState->state )
    {
        /********************/
        /* RX Client States */
        /********************/

        case (uint8_t)IPERF_RX_START_STATE:

            StateMachineRxStart(pIState);

            break;

#if defined(TCPIP_STACK_USE_UDP)
        case (uint8_t)IPERF_UDP_RX_STATE:

            StateMachineUdpRx(pIState);

            break;

        case (uint8_t)IPERF_UDP_RX_DRAIN_STATE:

            StateMachineUdpRxDrain(pIState);

            break;
#endif  // defined(TCPIP_STACK_USE_UDP)


#if defined(TCPIP_STACK_USE_UDP)
        case (uint8_t)IPERF_UDP_RX_DONE_STATE:

            StateMachineUdpRxDone(pIState);

            break;
#endif  // defined(TCPIP_STACK_USE_UDP)


#if defined(TCPIP_STACK_USE_TCP)
        case (uint8_t)IPERF_TCP_RX_LISTEN_STATE:

            StateMachineTcpListen(pIState);

            break;

        case (uint8_t)IPERF_TCP_RX_STATE:

            StateMachineTcpRx(pIState);

            break;

        case (uint8_t)IPERF_TCP_RX_DONE_STATE:

            StateMachineTcpRxDone(pIState);

#endif  // defined(TCPIP_STACK_USE_TCP)
            break;

        case (uint8_t)IPERF_RX_DONE_STATE:

            StateMachineRxDone(pIState);

            break;


       /********************/
       /* TX Client states */
       /********************/


        case (uint8_t)IPERF_TX_START_STATE:

            StateMachineTxStart(pIState);

            break;


        case (uint8_t)IPERF_TX_ARP_RESOLVE_STATE:

           StateMachineTxArpResolve(pIState);

           break;

#if defined(TCPIP_STACK_USE_UDP)
        case (uint8_t)IPERF_UDP_TX_OPEN_STATE:

            StateMachineUDPTxOpen(pIState);

            break;
#endif  // defined(TCPIP_STACK_USE_UDP)

#if defined(TCPIP_STACK_USE_TCP)
        case (uint8_t)IPERF_TCP_TX_OPEN_STATE:

            StateMachineTCPTxOpen(pIState);

            break;

        case (uint8_t)IPERF_TCP_TX_CONNECT_STATE:

            StateMachineTCPTxConnect(pIState);

            break;

        case (uint8_t)IPERF_TCP_TX_SEGMENT_STATE:

            StateMachineTcpTxSegment(pIState);

            break;
#endif  // defined(TCPIP_STACK_USE_TCP)

#if defined(TCPIP_STACK_USE_UDP)
        case (uint8_t)IPERF_UDP_TX_DATAGRAM_STATE:

            StateMachineUdpTxDatagram(pIState);

            break;
#endif  // defined(TCPIP_STACK_USE_UDP)

#if defined(TCPIP_STACK_USE_TCP)
        case (uint8_t)IPERF_TCP_TX_DONE_STATE:

            StateMachineTcpTxDone(pIState);

            break;
#endif  // defined(TCPIP_STACK_USE_TCP)


#if defined(TCPIP_STACK_USE_UDP)
        case (uint8_t)IPERF_UDP_TX_DONE_STATE:

            StateMachineUdpTxDone(pIState);

            break;
#endif  // defined(TCPIP_STACK_USE_UDP)

        default:
            IperfSetState(pIState, IPERF_STANDBY_STATE);
            break;

        }
}


//****************************************************************************
// Implementation: local functions
//****************************************************************************

static void ResetIperfCounters(tIperfState* pIState)
{
    // pIState->mAmount = 0U;
    // pIState->mDuration = 10U*1000U; // -t: default 10 sec
    // pIState->mInterval = 1000U;   // -i: default 1 sec
    pIState->mMSS = IPERF_TCP_MSS;
    pIState->mDatagramSize = 1470U; // -l: default 1470 bytes. UDP datagram size.
    pIState->totalLen = 0.0;
    pIState->pktId = 0;
    pIState->lastPktId = 0;
    pIState->errorCount = 0U;
    pIState->outofOrder = 0U;
    pIState->pktCount = 0U;
    pIState->statusReported = 0U;
    pIState->startTime = 0U;
    pIState->stopTime = 0U;

    pIState->lastCheckPktCount = 0U;
    pIState->lastCheckPktId = 0;
    pIState->lastCheckErrorCount = 0U;
    pIState->lastCheckTotalLen = 0U;
    pIState->lastCheckTime = 0U;

    pIState->isLastTransmit = 0U;

    pIState->txWaitTick = 0U;
//  pIState->mPendingACK = 0;
//  pIState->mRetransmit = 0;

}

static void ascii_to_u32s(char *ptr, uint32_t *values, uint8_t count)
{
    uint8_t i;
    uint32_t tmp;

    // Convert "123.456_78_90k", with count set to 4,  to
    // unsigned 32-bit numbers 123, 456, 78 and 90000, and
    // store them in the values array.

    if(ptr == NULL)
    {
        *values = 0;
        return;
    }

    for (i = 0; i < count; i++)
    {
        tmp = 0U;

        while ( (*ptr > '9') || (*ptr < '0') )
        {
            if ( (*ptr == ' ') || (*ptr == '\0') )
            {
                return; // terminates at blank or NULL.
            }

            ptr++;
        }

        while ( (*ptr <= '9') && (*ptr >= '0') )
        {
            uint32_t cDiff = (uint32_t)*ptr - (uint32_t)'0';
            tmp = tmp * 10U + cDiff;
            ptr++;
        }
        if ( (*ptr == 'k') || (*ptr == 'K') )
        {
            tmp = tmp * 1000U;
            ptr++;
        }
        else if ( (*ptr == 'm') || (*ptr == 'M') )
        {
            tmp = tmp * 1000U * 1000U;
            ptr++;
        }
        else
        {
            // do nothing
        }

        values[i] = tmp;
    }
}

//
// Convert the given BANDWIDTH in decimal with 'K' or 'M'
//
static void bw_conversion(SYS_CMD_DEVICE_NODE* pCmdIO, char *ptr, uint32_t *values)
{
    size_t j, len, decimal = 1U;
    uint32_t bw1 = 0U, bw2 = 0U,  mpf1 = 0U, mpf2 = 1000000U;
    uint32_t cDiff;

    len = strlen(ptr);

    if(strcmp((ptr + (len - 1U)), "M") == 0)
    {
        mpf1 = 1000U * 1000U;
    }
    
    if(strcmp((ptr + (len - 1U)), "K")==0)
    {
        mpf1 = 100U * 10U;
    }
        
    if(mpf1 != 0U)    
    { 
        mpf2 = mpf1;
        for(j = 0; j < len - 1U; j++)
        {
            //until "."
            if ( (*(ptr + j) <= '9') && (*(ptr + j) >= '0') && (decimal == 1U) )
            {
                cDiff = (uint32_t)*(ptr + j) - (uint32_t)'0';
                bw1 = bw1 * 10U + cDiff;
            }
            if(*(ptr + j) == '.')
            {
                decimal = 0U;
                bw1 = bw1 * mpf1;
            }
            if ( (*(ptr + j) <= '9') && (*(ptr + j) >= '0') && (decimal == 0U) )
            {
                cDiff = (uint32_t)*(ptr + j) - (uint32_t)'0';
                bw2 = bw2 * 10U + cDiff;
                mpf2 = mpf2 / 10U;
            }
        }
        bw2 = bw2 * mpf2;
        if(bw1 < mpf1)
        {
            bw1 = bw1 * mpf1;
        }
        
        values[0] = bw1;
        values[1] = bw2;
        (*pCmdIO->pCmdApi->print)(pCmdIO->cmdIoParam, "\nGiven in BW: %lu+%lu=%lu",bw1,bw2,bw1+bw2);
    }
}

//
// Report bandwith, jitter, and packet loss stastistics.
// Used by in both server and client modes.
//

static void ReportBW_Jitter_Loss(tIperfState* pIState, tIperfReport reportType)
{
    int32_t nAttempted;
    uint32_t nDropped;
    double kbps;
    uint32_t currentTime;
    uint32_t sec;
    double fMsec;
    uint32_t msec = 0U;
    const void* cmdIoParam = NULL;

    currentTime = SYS_TMR_TickCountGet();

    cmdIoParam = pIState->pCmdIO->cmdIoParam;

    switch ( reportType )
    {
        case INTERVAL_REPORT:

            nDropped = pIState->errorCount - pIState->lastCheckErrorCount;

            // bits-per-msec == Kbps

            fMsec = FC_U32Val2Double(currentTime - pIState->lastCheckTime) / (FC_U32Val2Double(sysFreq) / (double)1000.0);
            msec = FC_Double2U32Val(fMsec);

            if ( pIState->state == (uint8_t)IPERF_UDP_TX_DONE_STATE )
            {
                uint32_t uAttempt = FC_I32Val2U32Val(pIState->lastPktId - pIState->lastCheckPktId) + nDropped;
                nAttempted = FC_U32Val2I32Val(uAttempt);
            }
            else
            {
                nAttempted = pIState->pktId - pIState->lastCheckPktId;
            }

            if ( msec == 0u )
            {
                kbps = 0.0;
            }
            else
            {
                kbps = ((pIState->totalLen - (double)pIState->lastCheckTotalLen)*((double) 8.0)) / (double)msec;
            }

            sec = (pIState->lastCheckTime - pIState->startTime) / sysFreq;

            (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "    - [%2lu- %2lu sec] %3lu/ %3lu (%2lu%%)    %4lu Kbps\r\n",
                      (unsigned long)sec, 
                      (unsigned long)sec + ( (unsigned long) (pIState->mInterval / sysFreq) ),
                      (unsigned long)nDropped,
                      (unsigned long)nAttempted,
                      (nAttempted == 0) ? 0U : ((unsigned long)nDropped * 100U /(unsigned long)nAttempted),
                      FC_Double2U32Val(kbps + (double)0.5));
            break;


        case SUBTOTAL_REPORT:
            // intentional fall-through
        case SESSION_REPORT:

           nDropped = pIState->errorCount;

           if (pIState->state == (uint8_t)IPERF_UDP_TX_DONE_STATE)
            {
               nAttempted = pIState->lastPktId + (int32_t)nDropped;
            }
            else
            {
                nAttempted = pIState->lastPktId;
            }

            fMsec = FC_U32Val2Double(pIState->stopTime - pIState->startTime) / (FC_U32Val2Double(sysFreq) / (double)1000.0);
            msec = FC_Double2U32Val(fMsec);

            if ( msec == 0u )
            {
                kbps = 0.0;
            }
            else
            {
                kbps = (pIState->totalLen * ((double) 8.0)) / (double)msec;
            }

            (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "    - [0.0- %lu.%lu sec] %3lu/ %3lu (%2lu%%)    %4lu Kbps\r\n",
                             (unsigned long)(msec / 1000U),
                             (unsigned long)((msec % 1000U) / 100U),
                             (unsigned long)nDropped,
                             (unsigned long)nAttempted,
                             (nAttempted == 0) ? 0U : ((unsigned long)nDropped * 100U / (unsigned long)nAttempted),
                             FC_Double2U32Val(kbps + (double)0.5));
            break;

        default:
            // do nothing
            break;
    }

    if ( reportType ==  SESSION_REPORT )
    {
      (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "iperf: instance %d completed ...", pIState - gIperfState);
    }

    pIState->lastCheckPktId = pIState->pktId;
    pIState->lastCheckErrorCount = pIState->errorCount;
    pIState->lastCheckPktCount = pIState->pktCount;
    pIState->lastCheckTime = currentTime;
    pIState->lastCheckTotalLen = (uint32_t)pIState->totalLen;
}


static void StateMachineRxStart(tIperfState* pIState)
{
    const void* cmdIoParam = pIState->pCmdIO->cmdIoParam;
    if ( pIState->mServerMode == 0U)
    {
        (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Unsupported Configuration\r\n");
        IperfSetState(pIState, IPERF_STANDBY_STATE);
        return;
    }


    switch ( pIState->mProtocol )
    {
#if defined(TCPIP_STACK_USE_TCP)
        case TCP_PROTOCOL:  // TCP
            /* TCP Server sockets are allocated for entire runtime duration, a call to disconnect does not free them */
            /* therefore a subsequent N+1 open will fail */
            if (pIState->tcpServerSock == INVALID_SOCKET)
            {
                pIState->tcpServerSock = TCPIP_TCP_ServerOpen(IP_ADDRESS_TYPE_IPV4, pIState->mServerPort, &pIState->localAddr);

                if (pIState->tcpServerSock == INVALID_SOCKET)
                {
                    /* error case */
                    (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Create TCP socket failed\r\n");
                    IperfSetState(pIState, IPERF_STANDBY_STATE);
                    break;
                }
            }

            (void)TCPIP_TCP_SignalHandlerRegister(pIState->tcpServerSock, TCPIP_TCP_SIGNAL_RX_DATA, &F_IperfTCPRxSignalHandler, NULL);
#if (TCPIP_TCP_DYNAMIC_OPTIONS != 0)
            if(!TCPIP_TCP_OptionsSet(pIState->tcpServerSock, TCP_OPTION_RX_BUFF, FC_Uint2VPtr(pIState->rxBuffSize)))
            {
                (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Set of RX buffer size failed\r\n");
            }
#endif  // (TCPIP_TCP_DYNAMIC_OPTIONS != 0)

            IperfSetState(pIState, IPERF_TCP_RX_LISTEN_STATE);
            break;
#endif  // defined(TCPIP_STACK_USE_TCP)

#if defined(TCPIP_STACK_USE_UDP)
    case UDP_PROTOCOL:  // UDP
            if ( (pIState->udpSock = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_IPV4, pIState->mServerPort, &pIState->localAddr)) == INVALID_UDP_SOCKET )
            {
                /* error case */
                (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Create UDP socket failed\r\n");
                IperfSetState(pIState, IPERF_STANDBY_STATE);
                break;
            }

            (void)TCPIP_UDP_SignalHandlerRegister(pIState->udpSock, TCPIP_UDP_SIGNAL_RX_DATA, &F_IperfUDPRxSignalHandler, NULL);
            IperfSetState(pIState, IPERF_UDP_RX_STATE);
            break;
#endif  // defined(TCPIP_STACK_USE_UDP)

    default:
            (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Protocol error\r\n");
            IperfSetState(pIState, IPERF_STANDBY_STATE);
            break;
    }
}












static void StateMachineRxDone(tIperfState* pIState)
{
    const void* cmdIoParam = pIState->pCmdIO->cmdIoParam;

   switch( pIState->mProtocol)
   {
#if defined(TCPIP_STACK_USE_UDP)
       case UDP_PROTOCOL:
           (void)TCPIP_UDP_Close(  pIState->udpSock );
           break;
#endif  // defined(TCPIP_STACK_USE_UDP)

#if defined(TCPIP_STACK_USE_TCP)
       case TCP_PROTOCOL:
           (void)TCPIP_TCP_Close( pIState->tcpServerSock );
           pIState->tcpServerSock = INVALID_SOCKET;
           break;
#endif  // defined(TCPIP_STACK_USE_TCP)

       default:
           // do nothing
           break;
   }

    (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "\r\niperf instance %d: Rx done. Socket closed.\r\n", pIState - gIperfState);

    // Clear statistics
    ResetIperfCounters(pIState);

    // In server mode, continue to accept new session requests ...

    if ((pIState->mServerMode != 0U)  && (pIState->stopRequested == 0U) )
    {
        (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "iperf instance %d: Ready for the next session.\r\n", pIState - gIperfState);

        IperfSetState(pIState, IPERF_RX_START_STATE);
    }
    else
    {
        (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "iperf instance %d: completed.\r\n", pIState - gIperfState);
        IperfSetState(pIState, IPERF_STANDBY_STATE);
    }

}




/******************************/
/* TX CLIENT CODE BEGINS HERE */
/******************************/


static void StateMachineTxStart(tIperfState* pIState)
{

   if(TCPIP_STACK_NetMACTypeGet(pIState->pNetIf) != TCPIP_MAC_TYPE_PPP)
   {
       (void)TCPIP_ARP_Resolve(pIState->pNetIf, &pIState->remoteSide.remoteIPaddress.v4Add);
   }
   IperfSetState(pIState, IPERF_TX_ARP_RESOLVE_STATE);
   pIState->timer = SYS_TMR_TickCountGet();
}

static void StateMachineTxArpResolve(tIperfState* pIState)
{
    const void* cmdIoParam = pIState->pCmdIO->cmdIoParam;

  if ( pIState->stopRequested != 0U )
  {
     (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: client session closed.\r\n");
     IperfSetState(pIState, IPERF_STANDBY_STATE);
     return;
  }

  if(TCPIP_STACK_NetMACTypeGet(pIState->pNetIf) == TCPIP_MAC_TYPE_PPP)
  {
      (void)memset(pIState->remoteMACAddr.v, 0, sizeof(pIState->remoteMACAddr));
  }
  else if(!(TCPIP_Helper_IsMcastAddress(&pIState->remoteSide.remoteIPaddress.v4Add)))
  {
    if(!TCPIP_ARP_IsResolved(pIState->pNetIf, &pIState->remoteSide.remoteIPaddress.v4Add, &pIState->remoteMACAddr))
    {
        /* every 3 seconds print a warning */
        if( SYS_TMR_TickCountGet() - pIState->timer > 5U * sysFreq )
        {
            (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: ARP unable to resolve the MAC address of remote side.\r\n");
            pIState->timer = SYS_TMR_TickCountGet();
        }
        return;
    }

    (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "    - RemoteNode MAC: %x %x %x %x %x %x\r\n",
            pIState->remoteMACAddr.v[0],
            pIState->remoteMACAddr.v[1],
            pIState->remoteMACAddr.v[2],
            pIState->remoteMACAddr.v[3],
            pIState->remoteMACAddr.v[4],
            pIState->remoteMACAddr.v[5]);
  }
  else
  {
      // do nothing
  }
    
#if defined(TCPIP_STACK_USE_UDP)
  if ( pIState->mProtocol == UDP_PROTOCOL )
  {
     IperfSetState(pIState, IPERF_UDP_TX_OPEN_STATE);
  }
#endif  // defined(TCPIP_STACK_USE_UDP)

#if defined(TCPIP_STACK_USE_TCP)
  if ( pIState->mProtocol == TCP_PROTOCOL )
  {
     IperfSetState(pIState, IPERF_TCP_TX_OPEN_STATE);
  }
#endif  // defined(TCPIP_STACK_USE_TCP)

}

static void GenericTxHeaderPreparation(tIperfState* pIState, uint8_t *pData, bool isTheLastTransmit)
{
    tIperfPktInfo *pPktInfo = NULL;
    uint32_t currentTime;
    tClientHdr *pClientHdr = NULL;
    uint32_t tmp2;

    if ( pIState->pktId == 0 )
    {
        // The first tx packet
    }

    switch ( pIState->mProtocol )
    {
#if defined(TCPIP_STACK_USE_TCP)
        case TCP_PROTOCOL: // TCP

            // Required by iperf.
            pClientHdr = FC_U8Ptr2CliHdr(pData);

            // We borrow the same tIperfPktInfo structure to embed
            // some useful (non-standard iperf) meta info.
            // Unfortunately, the order has to be reversed.

            pPktInfo = FC_CliHdr2PktInfo(pClientHdr + 1);

            break;
#endif  // defined(TCPIP_STACK_USE_TCP)

#if defined(TCPIP_STACK_USE_UDP)
        case UDP_PROTOCOL: // UDP

            // Both are required by iperf.

            pPktInfo = FC_U8Ptr2PktInfo(pData);
            pClientHdr = FC_PktInfo2CliHdr(pPktInfo + 1);

            break;
#endif  // defined(TCPIP_STACK_USE_UDP)

        default:
            // do nothing
            break;
    }

    // Client header:
    // Needed for all UDP packets.
    // For TCP, only the first two segments need this info. However,
    // there seems to be no harm to put it to all segments though.

    pClientHdr->flags = TCPIP_Helper_htonl( (uint32_t) 0U);
    pClientHdr->numThreads = TCPIP_Helper_htonl((uint32_t) 1U);
    pClientHdr->mPort = TCPIP_Helper_htonl((uint32_t) pIState->mServerPort);
    pClientHdr->bufferlen = TCPIP_Helper_htonl( (uint32_t) 0U);
    pClientHdr->mWinBand = TCPIP_Helper_htonl(pIState->mTxRate);

    if ( pIState->mAmount != 0U )
    {
        pClientHdr->mAmount = TCPIP_Helper_htonl(pIState->mAmount);
    }
    else
    {
        uint32_t uAmount = FC_I32Val2U32Val(-((int32_t)pIState->mDuration / 10));
        pClientHdr->mAmount = TCPIP_Helper_htonl(uAmount);
    }

    // Additional info: needed for UDP only.
    // No harm to put it to all TCP segments though.

    if ( isTheLastTransmit == true )
    {
        // The last UDP tx packet. Some caveats:
        // 1. Iperf uses a negative Id for the last tx packet.
        // 2. Its id should not change during retransmit.

        pPktInfo->id = - (pIState->pktId - (long)pIState->nAttempts);
    }
    else
    {
        pPktInfo->id = pIState->pktId;
    }

    pPktInfo->id = (int32_t)TCPIP_Helper_htonl((uint32_t)pPktInfo->id);

    currentTime = SYS_TMR_TickCountGet();

    pPktInfo->tv_sec = TCPIP_Helper_htonl(currentTime / sysFreq);

    /* get the remainder of the ticks using modulus */
    tmp2 = ((pIState->stopTime - pIState->startTime) % sysFreq);

    /* normalize  to uSecs */
    tmp2 =  tmp2 * 1000U / sysFreq; /* Convert to mSec */
    tmp2 *= 1000U;   /* 1000 uSecs per mSec */


    pPktInfo->tv_usec = TCPIP_Helper_htonl( tmp2 );

    return;
}


static tIperfTxResult GenericTxStart(tIperfState* pIState)
{
    uint32_t currentTime;
    bool iperfKilled;
    const void* cmdIoParam = pIState->pCmdIO->cmdIoParam;    
    tIperfState* mpIState;
    size_t i;
    
    currentTime = SYS_TMR_TickCountGet();

    if ( currentTime < (pIState->nextTxTime - (uint32_t)TCPIP_IPERF_TIMING_ERROR_MARGIN))
    {
        // Wait until we are scheduled to Tx.
        return IPERF_TX_WAIT;
    }

    iperfKilled = pIState->stopRequested != 0U;
    if ((iperfKilled == true) ||
            ((pIState->mDuration != 0u) &&
             (currentTime > (pIState->startTime + pIState->mDuration))) ||
            ((pIState->mAmount != 0u) &&
             (pIState->totalLen > (double)pIState->mAmount)))
    {
        // Prepare to transmit the last packet.
        // Although the last packet needs to be retransmitted UDP_FIN_RETRANSMIT_COUNT times,
        // if we are in UDP mode.
        pIState->isLastTransmit = 1U;
    }

    if ( pIState->pktId == 0 )
    {
        // The first pkt is going out ...

        // Reset startTime, to get a more accurate report.

        pIState->startTime = currentTime;
        pIState->nextTxTime = pIState->startTime;

        pIState->lastCheckTime =    pIState->startTime;

        pIState->lastCheckPktId = pIState->pktId;
        pIState->lastCheckPktCount = pIState->pktCount;
        pIState->lastCheckErrorCount = pIState->errorCount;
        pIState->nAttempts = 0U;
    }

    // One Tx per mPktPeriod msec.
    pIState->nextTxTime = currentTime + pIState->mPktPeriod;
    
    switch(pIState->mProtocol)
    {
#if defined(TCPIP_STACK_USE_TCP)
        case TCP_PROTOCOL:
            /* Manage socket */
            if( TCPIP_TCP_GetIsReady(pIState->tcpClientSock) > 0u )
            {
                (void)TCPIP_TCP_Discard(pIState->tcpClientSock);
                return IPERF_TX_WAIT;
            }

            if ( TCPIP_TCP_WasReset(pIState->tcpClientSock) )
            {
                // We don't close the socket. We wait for user to "kill iperf" explicitly.
                (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "\r\niperf: Warning, TCP server disconnect detected\r\n");
                pIState->stopRequested = 1U;
            }

            if  (( TCPIP_TCP_PutIsReady(pIState->tcpClientSock) <= pIState->mMSS ) && (!iperfKilled))
            {
                return IPERF_TX_WAIT;
            }

            break;
#endif  // defined(TCPIP_STACK_USE_TCP)

#if defined(TCPIP_STACK_USE_UDP)
        case UDP_PROTOCOL:
            /* Manage socket */
            if( TCPIP_UDP_GetIsReady(pIState->udpSock) > 0u )
            {
                (void)TCPIP_UDP_Discard(pIState->udpSock);
                return IPERF_TX_WAIT;
            }
            //(pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "\n..Present TOS:%d, Prev: ",gIperfState[iperf_task_index].mTypeOfService); 
            mpIState = gIperfState;
            for(i = 0; i < nIperfSessions; i++)
            {            
                if(mpIState == pIState)
                {
                    continue;
                }
                
                //if((mpIState != pIState) && (tie_break[i]<3))
                if(mpIState->waitCount < 3U)    
                {
                    //(pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "%d(wait:%d), mpt:%d  ",pIState->mTypeOfService,pIState->sockWaitToSend, mpIState->mTypeOfService);
                    switch(pIState->mTypeOfService)
                    {
                        case (uint8_t)TCPIP_IPV4_DELAY_LOW:  /*0x20 BK, check for other 3-AC's - VI,V0,BE*/
                            if(mpIState->mTypeOfService == (uint8_t)TCPIP_IPV4_RELIABILITY_HIGH || mpIState->mTypeOfService == (uint8_t)TCPIP_IPV4_THROUGHPUT_HIGH || mpIState->mTypeOfService == (uint8_t)TCPIP_IPV4_TOS_DEFAULT)
                            {
                                if(mpIState->sockWaitToSend != 0U)
                                {
                                    mpIState->waitCount += 1U;
                                    return IPERF_TX_WAIT;
                                }
                            }
                            break;   

                        case (uint8_t)TCPIP_IPV4_RELIABILITY_HIGH:  /*0xA0 Video, check for only Voice*/
                            if(mpIState->mTypeOfService == (uint8_t)TCPIP_IPV4_THROUGHPUT_HIGH)
                            {
                                if(mpIState->sockWaitToSend != 0U)
                                {
                                    mpIState->waitCount += 1U;
                                    //(pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "%d, ",mpIState->sockWaitToSend);
                                    return IPERF_TX_WAIT;
                                }
                            }
                            break;

                        case (uint8_t)TCPIP_IPV4_TOS_DEFAULT: /* 0x00  BE*/
                            if(mpIState->mTypeOfService == (uint8_t)TCPIP_IPV4_RELIABILITY_HIGH || mpIState->mTypeOfService == (uint8_t)TCPIP_IPV4_THROUGHPUT_HIGH )
                            {
                                if(mpIState->sockWaitToSend != 0U)
                                {
                                    mpIState->waitCount += 1U;
                                    return IPERF_TX_WAIT;
                                }
                            }
                            break;

                        default:
                            /*Voice packet: process*/
                            break;
                    }
                }
                if(mpIState->waitCount == 3U)
                {
                    mpIState->waitCount = 0U;
                }
                mpIState++;
            }            

            if ( TCPIP_UDP_TxPutIsReady(pIState->udpSock, (uint16_t)pIState->mDatagramSize) < (uint16_t)pIState->mDatagramSize )
            {
                pIState->sockWaitToSend += 1U;
                
                if(pIState->txWaitTick == 0U)
                {     // beginning wait period
                    pIState->txWaitTick = SYS_TMR_TickCountGet() + (((uint32_t)TCPIP_IPERF_TX_WAIT_TMO * sysFreq) + 999U) / 1000U;
                    return IPERF_TX_WAIT;
                }
                else if( ((int32_t)SYS_TMR_TickCountGet() - (int32_t)pIState->txWaitTick) < 0 )
                { // wait some more
                    return IPERF_TX_WAIT;
                }
                else
                {
                    // do nothing
                }

                // TX ready timeout
                (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "iperf: Failed to get %d bytes socket TX space\r\n", pIState->mDatagramSize);
                return IPERF_TX_FAIL;
            }
            else
            {    // reset retry tick counter
                pIState->txWaitTick = 0U;
                pIState->sockWaitToSend -= 1U;                
            }
            break;
#endif  // defined(TCPIP_STACK_USE_UDP)

        default:
            // do nothing
            break;
    }



    GenericTxHeaderPreparation(pIState, g_bfr.buff, pIState->isLastTransmit != 0U);

    switch( pIState->mProtocol)
    {
#if defined(TCPIP_STACK_USE_TCP)
        case TCP_PROTOCOL:
            pIState->remainingTxData = (pIState->mMSS - (uint16_t)IPERF_HEADER_BUFFER);

            if (( TCPIP_TCP_ArrayPut(pIState->tcpClientSock, g_bfr.buff, IPERF_HEADER_BUFFER) != IPERF_HEADER_BUFFER ) && (!iperfKilled))
            {
                (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Socket send failed\r\n");
                pIState->errorCount++;
                return IPERF_TX_FAIL;
            }

            break;
#endif  // defined(TCPIP_STACK_USE_TCP)


#if defined(TCPIP_STACK_USE_UDP)
        case UDP_PROTOCOL:

            pIState->remainingTxData = ((uint16_t)pIState->mDatagramSize - (uint16_t)IPERF_HEADER_BUFFER);

            if ( TCPIP_UDP_ArrayPut(pIState->udpSock, g_bfr.buff, IPERF_HEADER_BUFFER) != IPERF_HEADER_BUFFER )
            {
                (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Socket send failed\r\n");
                pIState->errorCount++;
                return IPERF_TX_FAIL;
            }
            break;
#endif  // defined(TCPIP_STACK_USE_UDP)

        default:
            // do nothing
            break;
    }

    return IPERF_TX_OK;

}






static void GenericTxEnd(tIperfState* pIState)
{
    const void* cmdIoParam = pIState->pCmdIO->cmdIoParam;

    if(  pIState->remainingTxData  > 0u )
    {
        /* unhandled error */
        (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Socket send failed\r\n");
        pIState->errorCount++;
    }
    else
    {
        // send successful.

        if ( pIState->pktCount == 0u )
        {
            // first tx pkt

            IPV4_ADDR lclAddress;

            (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "\r\niperf: instance %d started ...\r\n", pIState - gIperfState);

            lclAddress.Val = TCPIP_STACK_NetAddress(pIState->pNetIf);

            (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "    - Local  %u.%u.%u.%u port %u connected with\r\n",
                    lclAddress.v[0],
                    lclAddress.v[1],
                    lclAddress.v[2],
                    lclAddress.v[3],
                    pIState->localPort);

            (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "    - Remote %u.%u.%u.%u port %u\r\n",
                    pIState->remoteSide.remoteIPaddress.v4Add.v[0],
                    pIState->remoteSide.remoteIPaddress.v4Add.v[1],
                    pIState->remoteSide.remoteIPaddress.v4Add.v[2],
                    pIState->remoteSide.remoteIPaddress.v4Add.v[3],
                    pIState->mServerPort );

            (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "    - Target rate = %ld bps, period = %ld ms\r\n",
                    (unsigned long)pIState->mTxRate, 
                    (unsigned long)(pIState->mPktPeriod * 1000U / sysFreq) );

        }

        pIState->pktId++;
        pIState->pktCount++;

#if defined(TCPIP_STACK_USE_UDP)
        if ( pIState->mProtocol == UDP_PROTOCOL )
        {
            pIState->totalLen += (double)pIState->mDatagramSize;
        }
#endif  // defined(TCPIP_STACK_USE_UDP)

#if defined(TCPIP_STACK_USE_TCP)
        if ( pIState->mProtocol == TCP_PROTOCOL )
        {

            pIState->totalLen += (double)pIState->mMSS;
        }
#endif  // defined(TCPIP_STACK_USE_TCP)


    }

    pIState->lastPktId = pIState->pktId - 1;



    if ( (SYS_TMR_TickCountGet() - pIState->lastCheckTime) >= (pIState->mInterval - (uint32_t)TCPIP_IPERF_TIMING_ERROR_MARGIN) )
    {
        // Time to report statistics
        ReportBW_Jitter_Loss(pIState, INTERVAL_REPORT);

        //pIState->lastCheckPktCount = pIState->pktCount;
    }


    if ( pIState->isLastTransmit != 0U )
    {       
        switch(pIState->mProtocol)
        {
#if defined(TCPIP_STACK_USE_UDP)
            case UDP_PROTOCOL:
                if(++pIState->nAttempts < UDP_FIN_RETRANSMIT_COUNT)
                {

                    if ( pIState->nAttempts == 1u )
                    {
                        // So the normal pkt statistics is not mixed with the retransmited last pkt.
                        pIState->stopTime = SYS_TMR_TickCountGet();

                        ReportBW_Jitter_Loss(pIState, SUBTOTAL_REPORT);
                        (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "    -----------------------------------------\r\n"); 
                    }

                    // Don't follow the same transmision rate during retransmit.
                    pIState->mPktPeriod = UDP_FIN_RETRANSMIT_PERIOD;
                }
                else
                {
                    IperfSetState(pIState, IPERF_UDP_TX_DONE_STATE);
                }
                break;
#endif  // defined(TCPIP_STACK_USE_UDP)

#if defined(TCPIP_STACK_USE_TCP)
            case TCP_PROTOCOL:
                IperfSetState(pIState, IPERF_TCP_TX_DONE_STATE);
                break;
#endif  // defined(TCPIP_STACK_USE_TCP)

            default:
                // do nothing
                break;
        }
        pIState->sockWaitToSend = 0U;

        pIState->stopTime = SYS_TMR_TickCountGet();
    }

}




static void GenericTxDone(tIperfState* pIState)
{
    const void* cmdIoParam = pIState->pCmdIO->cmdIoParam;
    if ( pIState->statusReported == 0u )
    {
        ReportBW_Jitter_Loss(pIState, SESSION_REPORT);
        pIState->statusReported = 1U;
    }

    IperfSetState(pIState, IPERF_STANDBY_STATE);

    (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "iperf instance %d: Tx done. Socket closed.\r\n", pIState - gIperfState);

    // Clear statistics
    ResetIperfCounters(pIState);

    //(pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "    Back to standby mode.\r\n");
    (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "iperf: instance %d completed.\r\n", pIState - gIperfState);

}

// TCP state machine functions
#if defined(TCPIP_STACK_USE_TCP)
static void StateMachineTcpTxDone(tIperfState* pIState)
{
    GenericTxDone(pIState);

    // close gracefully
    (void)TCPIP_TCP_Close(pIState->tcpClientSock);
    pIState->tcpClientSock = INVALID_SOCKET;
}

static void StateMachineTcpTxSegment(tIperfState* pIState)
{
    tIperfTxResult txRes = GenericTxStart(pIState);

    if(txRes == IPERF_TX_OK)
    {   // go ahead and transmit
       TcpTxFillSegment(pIState);
       (void)TCPIP_TCP_Flush(pIState->tcpClientSock);
       GenericTxEnd(pIState);
    }
    else if(txRes == IPERF_TX_FAIL)
    {
        IperfSetState(pIState, IPERF_TCP_TX_DONE_STATE);
        pIState->stopTime = SYS_TMR_TickCountGet();
    }
    else
    {
        // else wait...
    }
}

/* This routine does a piecewise send, because the entire RAM buffer may not be available for putArray */
static void TcpTxFillSegment(tIperfState* pIState)
{
    uint16_t chunk_size, sent_bytes;



    while( pIState->remainingTxData > 0u )
    {
        chunk_size = pIState->remainingTxData;
        if(chunk_size > (uint16_t)sizeof(txfer_buffer))
        {
            chunk_size = (uint16_t)sizeof(txfer_buffer);
        }


        sent_bytes = TCPIP_TCP_ArrayPut( pIState->tcpClientSock, txfer_buffer, chunk_size);
        pIState->remainingTxData -= sent_bytes;

        if(sent_bytes < chunk_size)
        {
            return;
        }
    }

}

static void StateMachineTCPTxOpen(tIperfState* pIState)
{
    const void* cmdIoParam = pIState->pCmdIO->cmdIoParam;

   if  ( (pIState->tcpClientSock = TCPIP_TCP_ClientOpen(IP_ADDRESS_TYPE_IPV4, pIState->mServerPort, NULL)) == INVALID_SOCKET )
   {
       /* error case */
        (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Create TCP socket failed\r\n");
        IperfSetState(pIState, IPERF_STANDBY_STATE);
        return;
    }
   (void)TCPIP_TCP_SignalHandlerRegister(pIState->tcpClientSock, TCPIP_TCP_SIGNAL_RX_DATA, &F_IperfTCPRxSignalHandler, NULL);

   if(pIState->localAddr.v4Add.Val != 0U)
   {
       (void)TCPIP_TCP_Bind(pIState->tcpClientSock, IP_ADDRESS_TYPE_IPV4, pIState->mServerPort, &pIState->localAddr);
   }
   (void)TCPIP_TCP_RemoteBind(pIState->tcpClientSock, IP_ADDRESS_TYPE_IPV4, 0U,  &pIState->remoteSide.remoteIPaddress);
    pIState->localPort = TCPIP_IPERF_TCP_LOCAL_PORT_START_NUMBER;

    (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "---------------------------------------------------------\r\n");
    (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "iperf: Client connecting to %u.%u.%u.%u, TCP port %u\r\n",
              pIState->remoteSide.remoteIPaddress.v4Add.v[0],
              pIState->remoteSide.remoteIPaddress.v4Add.v[1],
              pIState->remoteSide.remoteIPaddress.v4Add.v[2],
              pIState->remoteSide.remoteIPaddress.v4Add.v[3],
              pIState->mServerPort );

    IperfSetState(pIState, IPERF_TCP_TX_CONNECT_STATE);

#if (TCPIP_TCP_DYNAMIC_OPTIONS != 0)
    if(!TCPIP_TCP_OptionsSet(pIState->tcpClientSock, TCP_OPTION_TX_BUFF, FC_Uint2VPtr(pIState->txBuffSize)))
    {
        (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Set of TX buffer size failed\r\n");
    }
#endif  // (TCPIP_TCP_DYNAMIC_OPTIONS != 0)

    (void)TCPIP_TCP_Connect(pIState->tcpClientSock);
    pIState->timer = SYS_TMR_TickCountGet();
    
    /* Fill the buffer with ASCII char T */
    (void)memset( txfer_buffer, 0x54, sizeof(txfer_buffer));
}

static void StateMachineTcpRxDone(tIperfState* pIState)
{
    if ( pIState->statusReported == 0u )
    {
        ReportBW_Jitter_Loss(pIState, SESSION_REPORT);
        pIState->statusReported = 1U;
    }

    IperfSetState(pIState, IPERF_RX_DONE_STATE);
}

static void StateMachineTcpRx(tIperfState* pIState)
{
    uint16_t length;
    const void* cmdIoParam = pIState->pCmdIO->cmdIoParam;

    if( (length = TCPIP_TCP_GetIsReady(pIState->tcpServerSock)) == 0U )
    {

      if ( TCPIP_TCP_WasReset(pIState->tcpServerSock) || TCPIP_TCP_WasDisconnected(pIState->tcpServerSock)  )
      {
          pIState->stopTime = SYS_TMR_TickCountGet();
          IperfSetState(pIState, IPERF_TCP_RX_DONE_STATE);
          return;
      }

    }
    else
    {
       if ( pIState->pktId == 0)
       {
          IPV4_ADDR lclAddress;
          
          // This is the first rx pkt.
          (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "\r\niperf: instance %d session started ...\r\n", pIState - gIperfState);

          pIState->startTime = SYS_TMR_TickCountGet();
          pIState->lastCheckTime =  pIState->startTime;

          pIState->lastCheckPktId = pIState->pktId;
          lclAddress.Val = TCPIP_STACK_NetAddress(TCPIP_TCP_SocketNetGet(pIState->tcpServerSock));

          (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "    - Local  %u.%u.%u.%u port %u connected with\r\n",
                   lclAddress.v[0],
                   lclAddress.v[1],
                   lclAddress.v[2],
                   lclAddress.v[3],
                   pIState->mServerPort);

          (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "    - Remote %u.%u.%u.%u port %u\r\n",
                   pIState->remoteSide.remoteIPaddress.v4Add.v[0],
                   pIState->remoteSide.remoteIPaddress.v4Add.v[1],
                   pIState->remoteSide.remoteIPaddress.v4Add.v[2],
                   pIState->remoteSide.remoteIPaddress.v4Add.v[3],
                   pIState->remoteSide.remotePort );
       }

       pIState->pktId++;
       pIState->pktCount++;
       pIState->lastPktId = pIState->pktId;
       pIState->totalLen += (double)length;

       /* read the remaining datagram payload */
       /* a UdpDiscard would be disingenuous, because it would not reflect the bandwidth at L7 */
       while ( length > 0U )
       {
           uint16_t chunk_size;

           if ( length <  (uint16_t)sizeof(txfer_buffer) )
           {
               chunk_size = length;
           }
           else
           {
               chunk_size = (uint16_t)sizeof(txfer_buffer);
           }

           (void)TCPIP_TCP_ArrayGet( pIState->tcpServerSock, txfer_buffer, chunk_size);
           length -= chunk_size;
       }

    }

    if (pIState->pktId != 0)
    {
        uint32_t tickCount = SYS_TMR_TickCountGet(); 
       if((tickCount - pIState->lastCheckTime) > pIState->mInterval)
       {
           // Time to report statistics
           ReportBW_Jitter_Loss(pIState, INTERVAL_REPORT);
       }
    }

    if ( pIState->stopRequested != 0U )
    {
       IperfSetState(pIState, IPERF_TCP_RX_DONE_STATE);
       pIState->stopTime = SYS_TMR_TickCountGet();

       return;
    }
}

static void StateMachineTcpListen(tIperfState* pIState)
{

   if ( pIState->stopRequested != 0U )
   {
        IperfSetState(pIState, IPERF_RX_DONE_STATE);
        return;
   }

   if( TCPIP_TCP_IsConnected(pIState->tcpServerSock) )
   {
      TCP_SOCKET_INFO tcpSocketInfo = {0};
      (void)TCPIP_TCP_SocketInfoGet( pIState->tcpServerSock, &tcpSocketInfo);
      pIState->remoteSide = tcpSocketInfo;
      IperfSetState(pIState, IPERF_TCP_RX_STATE);

      /* clear the stack's reset flag */
      (void)TCPIP_TCP_WasReset(pIState->tcpServerSock);
   }
}

static void StateMachineTCPTxConnect(tIperfState* pIState)
{

    const void* cmdIoParam = pIState->pCmdIO->cmdIoParam;
    if ( pIState->stopRequested != 0U )
    {
        IperfSetState(pIState, IPERF_TCP_TX_DONE_STATE);
        return;
    }

    if( !TCPIP_TCP_IsConnected(pIState->tcpClientSock) )
    {

        // Time out if too much time is spent in this state
        if(SYS_TMR_TickCountGet()- pIState->timer > 5U * sysFreq)
        {
            (void)TCPIP_TCP_Close(pIState->tcpClientSock);
            pIState->tcpClientSock = INVALID_SOCKET;
            (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: TCP Client connection timeout\r\n");
            IperfSetState(pIState, IPERF_TCP_TX_DONE_STATE);
        }

        return;
    }

    /* reset the reset flag so we don't get a false positive */
    (void)TCPIP_TCP_WasReset(pIState->tcpClientSock);

    pIState->startTime = SYS_TMR_TickCountGet();
    pIState->nextTxTime = pIState->startTime + pIState->mPktPeriod;
    IperfSetState(pIState, IPERF_TCP_TX_SEGMENT_STATE);
}

#endif  // defined(TCPIP_STACK_USE_TCP)

// UDP state machine functions
#if defined(TCPIP_STACK_USE_UDP)
static void StateMachineUdpTxDone(tIperfState* pIState)
{

    GenericTxDone(pIState);

    (void)TCPIP_UDP_Close(pIState->udpSock );
}


static void StateMachineUdpTxDatagram(tIperfState* pIState)
{

    (void)TCPIP_UDP_OptionsSet(pIState->udpSock, UDP_OPTION_TOS, FC_Uint2VPtr((uint32_t)pIState->mTypeOfService));   

    tIperfTxResult txRes = GenericTxStart(pIState);

    if ( txRes == IPERF_TX_OK )
    {   // go ahead and transmit
       uint16_t txData = UdpTxFillDatagram(pIState);
       if(TCPIP_UDP_Flush(pIState->udpSock) == 0U)
       {   // failed; discard data
           (void)TCPIP_UDP_TxOffsetSet(pIState->udpSock, 0U, false);
       }
       else
       {
           pIState->remainingTxData -= txData;
           GenericTxEnd(pIState);
       }
    }
    else if(txRes == IPERF_TX_FAIL)
    {
        IperfSetState(pIState, IPERF_UDP_TX_DONE_STATE);
        pIState->stopTime = SYS_TMR_TickCountGet();
    }
    else
    {
        // else wait...
    }
}

/* This routine does a piece wis send, because the entire RAM pkt buffer may not be available for putArray */
static uint16_t UdpTxFillDatagram(tIperfState* pIState)
{

    uint16_t chunk_size, sent_bytes;
    uint16_t remainingTxData;
    uint16_t txData = 0;



    remainingTxData = pIState->remainingTxData;
    while( remainingTxData > 0u )
    {
        chunk_size = remainingTxData;
        if(chunk_size > (uint16_t)sizeof(txfer_buffer))
        {
            chunk_size = (uint16_t)sizeof(txfer_buffer);
        }

        sent_bytes = TCPIP_UDP_ArrayPut(pIState->udpSock, txfer_buffer, chunk_size);
        txData += sent_bytes;
        remainingTxData -= sent_bytes;

        if (sent_bytes != chunk_size )
        {
            break;
        }
    }

    return txData;

}

static void StateMachineUDPTxOpen(tIperfState* pIState)
{   
    UDP_SOCKET_INFO UdpSkt;
    const void* cmdIoParam = pIState->pCmdIO->cmdIoParam;
    
    if ( (pIState->udpSock = TCPIP_UDP_ClientOpen(IP_ADDRESS_TYPE_IPV4, pIState->mServerPort, &pIState->remoteSide.remoteIPaddress)) == INVALID_UDP_SOCKET )
    {
        /* error case */
        (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Create UDP socket failed\r\n");
        IperfSetState(pIState, IPERF_STANDBY_STATE);
        return;
    }

    (void)TCPIP_UDP_SignalHandlerRegister(pIState->udpSock, TCPIP_UDP_SIGNAL_RX_DATA, &F_IperfUDPRxSignalHandler, NULL);
    
    if(!TCPIP_UDP_OptionsSet(pIState->udpSock, UDP_OPTION_TX_BUFF, FC_Uint2VPtr(pIState->mDatagramSize)))
    {
        (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Set of TX buffer size failed\r\n");
    }
    size_t txQLimit;
#if defined(TCPIP_STACK_USE_PPP_INTERFACE)
    if(TCPIP_STACK_NetMACTypeGet(pIState->pNetIf) == TCPIP_MAC_TYPE_PPP)
    {
        txQLimit = TCPIP_IPERF_PPP_TX_QUEUE_LIMIT;
    }
    else
    {
        txQLimit = TCPIP_IPERF_TX_QUEUE_LIMIT;
    }
#else
    txQLimit = TCPIP_IPERF_TX_QUEUE_LIMIT;
#endif  // defined(TCPIP_STACK_USE_PPP_INTERFACE)
    if(!TCPIP_UDP_OptionsSet(pIState->udpSock, UDP_OPTION_TX_QUEUE_LIMIT, FC_Uint2VPtr((uint32_t)txQLimit)))
    {
        (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Set of TX queuing limit failed\r\n");
    }

    (void)TCPIP_UDP_SocketNetSet(pIState->udpSock, pIState->pNetIf);
    (void)memset(&UdpSkt, 0, sizeof(UdpSkt));
    (void)TCPIP_UDP_SocketInfoGet(pIState->udpSock, &UdpSkt);
    pIState->localPort = UdpSkt.localPort;

    (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "---------------------------------------------------------\r\n");
    (pIState->pCmdIO->pCmdApi->print)(cmdIoParam,  "iperf: Client connecting to %u.%u.%u.%u, UDP port %u\r\n",
              pIState->remoteSide.remoteIPaddress.v4Add.v[0],
              pIState->remoteSide.remoteIPaddress.v4Add.v[1],
              pIState->remoteSide.remoteIPaddress.v4Add.v[2],
              pIState->remoteSide.remoteIPaddress.v4Add.v[3],
              pIState->mServerPort );
    IperfSetState(pIState, IPERF_UDP_TX_DATAGRAM_STATE);

    pIState->startTime = SYS_TMR_TickCountGet();

     // Wait for a few seconds before first TCP tx, so we can resolve ARP.
    pIState->nextTxTime = pIState->startTime + pIState->mPktPeriod;

    /* Fill the buffer with ASCII char U */
    (void)memset( txfer_buffer, 0x55, sizeof(txfer_buffer));

}

static void StateMachineUdpRxDone(tIperfState* pIState)
{
    tIperfPktInfo *pPktInfo;
    tServerHdr *pServer_hdr;
    float tmp2;


    if ( pIState->statusReported == 0u )
    {
        ReportBW_Jitter_Loss(pIState, SESSION_REPORT);
        pIState->statusReported = 1U;
    }

    /* Drain any waiting pkts */
    if (  TCPIP_UDP_GetIsReady(pIState->udpSock) != 0U )
    {
        (void)TCPIP_UDP_Discard(pIState->udpSock);
        return;
    }

    // Send the iperf UDP "FIN-ACK" 10 times.
    if ( pIState->nAttempts++ > 10u )
    {
        IperfSetState(pIState, IPERF_RX_DONE_STATE);
        return;
    }

     /* Make sure space is available to TX the ACK packet of 128 bytes */
    if ( TCPIP_UDP_TxPutIsReady(pIState->udpSock, 128 ) > 0u )
    {

      pPktInfo = g_bfr.pktInfo;
      pServer_hdr = FC_PktInfo2SrvHdr(pPktInfo + 1);

      pPktInfo->id = (int32_t)TCPIP_Helper_htonl((uint32_t)(-pIState->lastPktId));
      pPktInfo->tv_sec = 0U;
      pPktInfo->tv_usec = 0U;

      pServer_hdr->flags = TCPIP_Helper_htonl(HEADER_VERSION1);
      pServer_hdr->total_len1 = 0U;
      pServer_hdr->total_len2 = TCPIP_Helper_htonl( (uint32_t) pIState->totalLen);

      pServer_hdr->stop_sec =  TCPIP_Helper_htonl( (uint32_t) (pIState->stopTime - pIState->startTime) / sysFreq);

      /* get the remainder of the ticks using modulus */
      tmp2 = FC_U32Val2Float((pIState->stopTime - pIState->startTime) % sysFreq);

      /* normalize  to uSecs */
      tmp2 =  tmp2 * (float)1000.0 / FC_U32Val2Float(sysFreq); /* Convert to mSec */
      tmp2 *= 1000.0;   /* 1000 uSecs per mSec */


      pServer_hdr->stop_usec = TCPIP_Helper_htonl( (uint32_t) tmp2 );
      pServer_hdr->error_cnt = TCPIP_Helper_htonl( (uint32_t)  pIState->errorCount);;
      pServer_hdr->outorder_cnt = TCPIP_Helper_htonl( (uint32_t) pIState->outofOrder);
      pServer_hdr->datagrams = TCPIP_Helper_htonl( (uint32_t) pIState->lastPktId);
      pServer_hdr->jitter1 = 0U;
      pServer_hdr->jitter2 = 0U;

      (void)TCPIP_UDP_ArrayPut(pIState->udpSock, g_bfr.buff, IPERF_HEADER_BUFFER);

      uint8_t tmpBuffer[128-IPERF_HEADER_BUFFER];
      (void)memset(tmpBuffer, 0, sizeof(tmpBuffer));
      (void)TCPIP_UDP_ArrayPut(pIState->udpSock, tmpBuffer, (uint16_t)sizeof(tmpBuffer));
      
      (void)TCPIP_UDP_Flush(pIState->udpSock );

    }

}

static void StateMachineUdpRxDrain(tIperfState* pIState)
{
    if( TCPIP_UDP_GetIsReady(pIState->udpSock) > (uint8_t)0 )
    {
         (void)TCPIP_UDP_Discard(pIState->udpSock);
         return;
    }

   /* If iperf kill was done, just jump to closing the socket */
   if ( pIState->stopRequested != 0U)
   {
       IperfSetState(pIState, IPERF_RX_DONE_STATE);
   }
   else /* go ahead an generate reports, etc */
   {
       IperfSetState(pIState, IPERF_UDP_RX_DONE_STATE);
   }

}

static void StateMachineUdpRx(tIperfState* pIState)
{
    uint16_t length = 0U;
    tIperfPktInfo *pPktInfo;
    UDP_SOCKET_INFO UdpSkt;
    IPV4_ADDR      lclIpAddr, remIpAddr;
    const void* cmdIoParam = pIState->pCmdIO->cmdIoParam;


    // Do nothing if no data is waiting
    while( (length = TCPIP_UDP_GetIsReady(pIState->udpSock)) >= (uint16_t)(sizeof(tIperfPktInfo)) )
    {
       /* The GetArray should not fail... */
       if ( TCPIP_UDP_ArrayGet(pIState->udpSock, g_bfr.buff, (uint16_t)sizeof(tIperfPktInfo)) != (uint16_t)sizeof(tIperfPktInfo) )
       {
          (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: UDP Get Array Failed\r\n");
          IperfSetState(pIState, IPERF_UDP_RX_DRAIN_STATE);
          return;
       }

       pPktInfo = g_bfr.pktInfo;
       pIState->pktId = (long)TCPIP_Helper_htonl((uint32_t)pPktInfo->id);

       if ( (pIState->pktCount == 0U) && (pIState->pktId < 0) )
       {
          // Ignore retransmits from previous session.
          (void)TCPIP_UDP_Discard(pIState->udpSock);
          return;
       }

       pIState->pktCount++;
       if (pIState->pktCount == (uint32_t)1 )
       {
          // The first pkt is used to set up the server,
          // does not count as a data pkt.

          (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "\r\niperf: instance %d session started ...\r\n", pIState - gIperfState);

          if ( pIState->pktId != 0 )
          {
             // We have lost a few packets before the first pkt arrived.
             (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "iperf: - First pkt id = %ld (should be 0)\r\n",
                              pIState->pktId);
             // The first data pkt starts with id = 1.
             pIState->errorCount    +=  (uint32_t)pIState->pktId - 1U;
          }

          pIState->lastPktId = pIState->pktId;

          (void)memset(&UdpSkt, 0, sizeof(UdpSkt));
          (void)TCPIP_UDP_SocketInfoGet(pIState->udpSock, &UdpSkt);
          lclIpAddr.Val = TCPIP_STACK_NetAddress(TCPIP_UDP_SocketNetGet(pIState->udpSock));
          remIpAddr.Val = UdpSkt.remoteIPaddress.v4Add.Val;

          (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "    - Local  %u.%u.%u.%u port %u connected with\r\n",
                           lclIpAddr.v[0],
                           lclIpAddr.v[1],
                           lclIpAddr.v[2],
                           lclIpAddr.v[3],
                           pIState->mServerPort);
          
          (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "    - Remote %u.%u.%u.%u port %u\r\n",      
                           remIpAddr.v[0],
                           remIpAddr.v[1],
                           remIpAddr.v[2],
                           remIpAddr.v[3],
                           UdpSkt.remotePort);

          // Store the remote info so we can send the iperf "UDP-FIN-ACK" msg
          pIState->remoteSide.remoteIPaddress.v4Add.Val = remIpAddr.Val;
          pIState->remoteSide.remotePort =  UdpSkt.remotePort;

          pIState->startTime = SYS_TMR_TickCountGet();
          //pIState->remoteStartTime = IPERFTOHL(pPktInfo->tv_sec);

          pIState->lastCheckTime =  pIState->startTime;

          pIState->lastCheckPktId = pIState->pktId;
          pIState->lastCheckPktCount = pIState->pktCount;
          pIState->lastCheckErrorCount = pIState->errorCount;

          (void)TCPIP_UDP_Discard(pIState->udpSock);

          continue;
      }

      pIState->totalLen += (double)length;

      if ( pIState->pktId < 0 )
      {
         // this is the last datagram
         pIState->pktId = - pIState->pktId;

         pIState->stopTime = SYS_TMR_TickCountGet();
        //pIState->remoteStopTime = IPERFTOHL(pPktInfo->tv_sec);

        pIState->nAttempts = 0U;
        IperfSetState(pIState, IPERF_UDP_RX_DRAIN_STATE);
      }

      if ( pIState->pktId != pIState->lastPktId + 1 )
      {
         if ( pIState->pktId < pIState->lastPktId + 1 )
         {
            pIState->outofOrder++;
         }
         else
         {
            pIState->errorCount += (uint32_t)pIState->pktId - ((uint32_t)pIState->lastPktId + 1U);
         }
      }

      // never decrease pktId (e.g. if we get an out-of-order packet)
      if ( pIState->pktId == pIState->lastPktId )
      {
         (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Recv duplicated pkt\r\n");
      }

      if ( pIState->pktId > pIState->lastPktId )
      {
         pIState->lastPktId = pIState->pktId;
      }

      /* read the remaining datagram payload - the full payload */
      /* a UdpDiscard would be disingenuous, because it would not reflect the bandwidth at L7 */
      length -=  (uint16_t)sizeof(tIperfPktInfo);
      while ( length > 0U )
      {
          uint16_t chunk_size;

          if ( length <  (uint16_t)sizeof(txfer_buffer) )
          {
              chunk_size = length;
          }
          else
          {
              chunk_size = (uint16_t)sizeof(txfer_buffer);
          }

          (void)TCPIP_UDP_ArrayGet(pIState->udpSock, txfer_buffer, chunk_size);
          length -= chunk_size;
      }


    }  /* end got a datagram */

    if (pIState->pktCount != (uint32_t)0)
    {
        uint32_t tickCount = SYS_TMR_TickCountGet();
        if((tickCount - pIState->lastCheckTime) > pIState->mInterval)
        {
            if ( pIState->pktCount == pIState->lastCheckPktCount )
            {
                // No events in pIState->mInterval msec, we timed out
                (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Rx timed out\r\n");

                pIState->stopTime = SYS_TMR_TickCountGet();

                pIState->nAttempts = 0U;
                IperfSetState(pIState, IPERF_UDP_RX_DRAIN_STATE);
            }
            else
            {
                ReportBW_Jitter_Loss(pIState, INTERVAL_REPORT);
            }
        }
    }

    if ( pIState->stopRequested != 0U )
    {
        IperfSetState(pIState, IPERF_UDP_RX_DRAIN_STATE);
        return;
    }

}

#endif  // defined(TCPIP_STACK_USE_UDP)

static tIperfState* GetIperfSession(void)
{

    size_t i = 0;

    tIperfState* pIState = gIperfState;
    for(i = 0; i < nIperfSessions; i++)
    {
        if(pIState->state == (uint8_t)IPERF_STANDBY_STATE)
        {
            return pIState;
        }
        pIState++;
    }
    return NULL;
}

static void CommandIperfStart(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    size_t i, len;
    uint8_t tos;
    char *ptr;
    uint32_t values[4] = {0}, bw=0;
    
    float pktRate;
    uint16_t payloadSize = 0;
    TCPIP_UINT16_VAL asciTos;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    tIperfState* pIState = GetIperfSession();   
    
    if(pIState == NULL)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "\r\niperf: All instances busy. Retry later!\r\n");
        return;
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\r\niperf: Starting session instance %d\r\n", pIState - gIperfState);

    // preparation for new iperf test
    pIState->mServerMode = 0U;
#if defined(TCPIP_STACK_USE_TCP)
    pIState->mProtocol = TCP_PROTOCOL;              // default is TCP mode.
#else
    pIState->mProtocol = UDP_PROTOCOL;
#endif  // defined(TCPIP_STACK_USE_TCP)
    pIState->stopRequested = 0U;

    pIState->mServerPort = TCPIP_IPERF_SERVER_PORT;     // -p. default: server port 5001

    pIState->mTxRate = ((uint32_t)TCPIP_IPERF_TX_BW_LIMIT * 1000U) * ((uint32_t)1000U);     // -b or -x. Target tx rate.
    // KS: default tx rate for iperf is actually 1Mbps.

    pIState->mAmount = 0U;           // -n: default 0.
    pIState->mDuration = 10U * sysFreq; // -t: default 10 sec.
    pIState->mInterval =  sysFreq;     // -i: default 1 sec.

    pIState->mTypeOfService = 0U;       //-S, --tos (Type Of Service): default 0: BestEffort
    // remember the console we've been invoked from
    pIState->pCmdIO = pCmdIO;

    
    // Initialize statistics

    ResetIperfCounters(pIState);

    i = 1U;
    while(i < (size_t)argc)
    {
        if ((strcmp(argv[i], "-s") == 0) || (strcmp(argv[i], "--server") == 0) )
        {
            // Function as an iperf server.

            pIState->mServerMode = 1U;
        }
#if defined(TCPIP_STACK_USE_UDP)
        else if ((strcmp(argv[i], "-u") == 0) || (strcmp(argv[i], "--udp") == 0) )
        {
            // iperf UDP mode.
            pIState->mProtocol = UDP_PROTOCOL;
        }
        else if ((strcmp(argv[i], "-b") == 0) || (strcmp(argv[i], "--bandwidth") == 0) )
        {
            // iperf UDP mode.
            pIState->mProtocol = UDP_PROTOCOL;

            // Next argument should be the target rate, in bps.
            i++;
            ptr = argv[i];
            len = strlen(ptr);
            
            if((strcmp((ptr + (len - 1U)), "M") == 0) || (strcmp((ptr + (len - 1U)), "K") == 0))
            {  
                bw_conversion(pCmdIO, ptr, values);
                bw = values[0] + values[1];
            }
            else
            {
                ascii_to_u32s(ptr, values, 1);
                bw = values[0];
            }

            //(*pCmdIO->pCmdApi->print)(cmdIoParam, "\nGiven BW:%d",bw);            
            pIState->mTxRate = bw;
        }
#endif  // defined(TCPIP_STACK_USE_UDP)
#if defined(TCPIP_STACK_USE_TCP)
        else if ((strcmp(argv[i], "-x") == 0) || (strcmp(argv[i], "--xmitrate") == 0) )
        {
            // NON-STANDARD IPERF OPTION. Set the max TCP tx rate.
            // Next argument should be the target rate, in bps.
            i++;
            ptr = argv[i];

            ascii_to_u32s(ptr, values, 1);

            pIState->mTxRate = values[0];
        }
#endif  // defined(TCPIP_STACK_USE_TCP)
        else if ((strcmp(argv[i], "-c") == 0) || (strcmp(argv[i], "--client") == 0) )
        {
            // Function as an iperf client.
            pIState->mServerMode = 0U;

            // Next argument should be the server IP, such as "192.168.1.100".
            i++;
            ptr = argv[i];
            ascii_to_u32s(ptr, values, 4);

            pIState->remoteSide.remoteIPaddress.v4Add.v[0] = (uint8_t)values[0];
            pIState->remoteSide.remoteIPaddress.v4Add.v[1] = (uint8_t)values[1];
            pIState->remoteSide.remoteIPaddress.v4Add.v[2] = (uint8_t)values[2];
            pIState->remoteSide.remoteIPaddress.v4Add.v[3] = (uint8_t)values[3]; 
        }
        else if ((strcmp(argv[i], "-t") == 0) || (strcmp(argv[i], "--time") == 0) )
        {
            // Next argument should be the (client tx) duration, in seconds.
            i++;
            ptr = argv[i];
            ascii_to_u32s(ptr, values, 1);

            pIState->mDuration = values[0] * sysFreq;
            pIState->mAmount = 0U;
        }
        else if ((strcmp(argv[i], "-n") == 0) || (strcmp(argv[i], "--num") == 0) )
        {
            // Next argument should be the (client tx) size, in bytes.
            i++;
            ptr = argv[i];
            ascii_to_u32s(ptr, values, 1);

            pIState->mAmount = values[0];
            pIState->mDuration = 0;
        }
        else if ((strcmp(argv[i], "-S") == 0) || (strcmp(argv[i], "--tos") == 0) )
        {
            // Next argument should be the (client tx) TOS
            i++;
            if(argv[i][1] == 'x')
            {
                asciTos.v[1] = (uint8_t)argv[i][2];
                asciTos.v[0] = (uint8_t)argv[i][3];                    
                tos = hexatob(asciTos.Val);
            }
            else
            {
                ptr = argv[i];
                ascii_to_u32s(ptr, values, 1);
                tos = (uint8_t)values[0];                
            }

            switch(tos)
            {
                case (uint8_t)IPERF_TOS_VO:
                    pIState->mTypeOfService = (uint8_t)TCPIP_IPV4_THROUGHPUT_HIGH;                  
                    break;
                case (uint8_t)IPERF_TOS_VI:
                    pIState->mTypeOfService = (uint8_t)TCPIP_IPV4_RELIABILITY_HIGH;                                      
                    break;
                case (uint8_t)IPERF_TOS_BK:
                    pIState->mTypeOfService = (uint8_t)TCPIP_IPV4_DELAY_LOW;                                      
                    break;
                case (uint8_t)IPERF_TOS_BE:
                    pIState->mTypeOfService = (uint8_t)TCPIP_IPV4_TOS_DEFAULT;                                      
                    break;
                 default:   
                    pIState->mTypeOfService = (uint8_t)TCPIP_IPV4_TOS_DEFAULT;                                                          
                    break;
            }

            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\nGiven TOS value is %d, mTypeOfService:%d",tos,pIState->mTypeOfService);
            
        }
        
#if defined(TCPIP_STACK_USE_TCP)
        else if ((strcmp(argv[i], "-M") == 0) ||
                (strcmp(argv[i], "--mss") == 0) )
        {
            // Next argument should be the (client tcp tx) MSS size, in bytes.

            i++;
            ptr = argv[i];

            ascii_to_u32s(ptr, values, 1);

            pIState->mMSS = (uint16_t)values[0];
        }
#endif  // defined(TCPIP_STACK_USE_TCP)

        else if ((strcmp(argv[i], "-i") == 0) || (strcmp(argv[i], "--interval") == 0) )
        {
            // Next argument should be the report interval, in seconds.
            i++;
            ptr = argv[i];
            ascii_to_u32s(ptr, values, 1);

            pIState->mInterval = values[0] * sysFreq; // Convert to msec
        }
        else if ((strcmp(argv[i], "-p") == 0) || (strcmp(argv[i], "--port") == 0) )
        {
            // Next argument should be the port number.
            i++;
            ptr = argv[i];
            ascii_to_u32s(ptr, values, 1);

            pIState->mServerPort = (uint16_t)values[0];       // -p. default: server port 5001
        }
#if defined(TCPIP_STACK_USE_UDP)
        else if ((strcmp(argv[i], "-l") == 0) || (strcmp(argv[i], "--len") == 0) )
        {
            // Next argument should be the buffer length, in bytes.
            // This is used as the UDP datagram size.
            i++;
            ptr = argv[i];
            ascii_to_u32s(ptr, values, 1);

            if ( values[0] <  IPERF_HEADER_BUFFER  )
            {
               (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "iperf: The minimum datagram size is %d\r\n", (int)IPERF_HEADER_BUFFER);
               return;
            }

            pIState->mDatagramSize = values[0];
        }
#endif  // defined(TCPIP_STACK_USE_UDP)
        else
        {
            // do nothing
        }
        i++;
    }

    switch (pIState->mServerMode)
    {
        case 0:
            // iperf client

            // set the interface to work on
            if((pIState->pNetIf = TCPIP_STACK_IPAddToNet(&pIState->localAddr.v4Add, false)) == NULL)
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Using the default interface!\r\n");
                pIState->pNetIf = TCPIP_STACK_NetDefaultGet();
            }

#if defined(TCPIP_STACK_USE_TCP)
            if(pIState->mProtocol == TCP_PROTOCOL)
            {
                payloadSize =   pIState->mMSS;
            }
#endif  // defined(TCPIP_STACK_USE_TCP)

#if defined(TCPIP_STACK_USE_UDP)
            if(pIState->mProtocol == UDP_PROTOCOL)
            {
                payloadSize =   (uint16_t)pIState->mDatagramSize;
            }
#endif  // defined(TCPIP_STACK_USE_UDP)

            pktRate =  FC_U32Val2Float(pIState->mTxRate / 8U) / FC_U32Val2Float((uint32_t)payloadSize);
            pIState->mPktPeriod =  FC_Float2U32Val( FC_U32Val2Float(sysFreq) / pktRate );

            IperfSetState(pIState, IPERF_TX_START_STATE);
            break;

        case 1:
            // iperf server
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "---------------------------------------------------------\r\n");

            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Server listening on ");
#if defined(TCPIP_STACK_USE_UDP)
            if (pIState->mProtocol == UDP_PROTOCOL)
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, (const char *)"UDP");
            }    
#endif  // defined(TCPIP_STACK_USE_UDP)
#if defined(TCPIP_STACK_USE_TCP)
            if (pIState->mProtocol == TCP_PROTOCOL)
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, (const char *)"TCP");
            }    
#endif  // defined(TCPIP_STACK_USE_TCP)

            (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, " port %d\r\n", pIState->mServerPort);
            IperfSetState(pIState, IPERF_RX_START_STATE);
            break;

        default:
            // do nothing
            break;
    }
    
}

static void CommandIperfStop(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    tIperfState* pIState;   
    bool okParam = false;
    uint32_t iperfIndex = 0U;

    if(argc == 1)
    {   // no params passed
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "iperfk: Using index 0\r\n");
        okParam = true;
    }
    else if(argc == 3)
    {   // valid number of args
        if((strcmp(argv[1], "-i") == 0) || (strcmp(argv[1], "--index") == 0))
        {
            if (FC_Str2UL(argv[2], 10, &iperfIndex) < 0)
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "iperfk: index range error!\r\n");
                return;
            }
            okParam = true;
        }
    }
    else
    {
        // do nothing
    }

    if(!okParam)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: iperfk <-i index>\r\n");
        return;
    }

    if(iperfIndex >= nIperfSessions)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\r\niperf: Invalid session number. Min: 0, Max: %d\r\n", nIperfSessions - 1U);
        return;
    }

    pIState = gIperfState + iperfIndex; 

    if(pIState->state == (uint8_t)IPERF_STANDBY_STATE)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "\r\niperf session: not started!\r\n");
    }
    else
    {
        pIState->stopRequested = 1U;
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\r\niperf: trying to stop iperf instance %d...\r\n", iperfIndex);
    }

}

static void CommandIperfNetIf(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    IPV4_ADDR   ipAddr;
    tIperfState* pIState;   
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    uint32_t iperfIndex = 0U;     // assume index 0 if not specified
    bool addFound = false;

    ipAddr.Val = 0;

    if (argc >= 3)
    {
        int currIx = 1; // 1st param is always the command name, skip it
        while(currIx + 1 < argc)
        { 
            char* param = argv[currIx];
            char* paramVal = argv[currIx + 1];

            if(strcmp(param, "-a") == 0)
            {
                // argument should be the IP address, such as "192.168.1.100".
                // use "0" for any interface (server mode only)
                if(!TCPIP_Helper_StringToIPAddress(paramVal, &ipAddr))
                {
                    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "iperfi: use a valid IP address!\r\n");
                    return;
                }
                else
                {
                    addFound = true;
                }
            }
            else if((strcmp(param, "-i") == 0) || (strcmp(param, "--index") == 0))
            {
                if (FC_Str2UL(paramVal, 10, &iperfIndex) < 0)
                {
                    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "iperfi: index range error!\r\n");
                    return;
                }
            }
            else
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "iperfi: Unknown parameter\r\n");
            }

            currIx += 2;
        }
    }


    if (!addFound)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: iperfi -a address <-i index>\r\n");
        return;
    }

    if(iperfIndex >= nIperfSessions)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\r\niperf: Invalid session number. Min: 0, Max: %d\r\n", nIperfSessions - 1U);
        return;
    }

    pIState = gIperfState + iperfIndex; 

    if (pIState->state != (uint8_t)IPERF_STANDBY_STATE)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\r\niperfi: cannot change the ip address while session: %d running!\r\n", iperfIndex);
        return;
    }

    pIState->localAddr.v4Add.Val = ipAddr.Val;
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "iperfi: OK, set the IP address to instance: %d\r\n", iperfIndex);

}

static void CommandIperfSize(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    bool        setTx, setRx;
    uint32_t    txBuffSize, rxBuffSize;
    tIperfState* pIState;   
    uint32_t iperfIndex;

    const void* cmdIoParam = pCmdIO->cmdIoParam;


    setTx = setRx = 0;
    txBuffSize = rxBuffSize = 0U;
    iperfIndex = 0U;     // assume index 0 if not specified

    int currIx = 1; // 1st param is always the command name, skip it
    while(currIx + 1 < argc)
    { 
        char* param = argv[currIx];
        char* paramVal = argv[currIx + 1];

        if(strcmp(param, "-tx") == 0)
        {
            setTx = true;
            if (FC_Str2UL(paramVal, 10, &txBuffSize) < 0)                
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "iperfs: size range error!\r\n");
                return;
            }
        }
        else if(strcmp(param, "-rx") == 0)
        {
            setRx = true;
            if (FC_Str2UL(paramVal, 10, &rxBuffSize) < 0)                
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "iperfs: size range error!\r\n");
                return;
            }
        }
        else if((strcmp(param, "-i") == 0) || (strcmp(param, "--index") == 0))
        {
            if (FC_Str2UL(paramVal, 10, &iperfIndex) < 0)            
            {
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, "iperfs: index range error!\r\n");
                return;
            }
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: iperfs <-tx size> <-rx size> <-i index>\r\n");
        }

        currIx += 2;
    }

    if(iperfIndex >= nIperfSessions)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\r\niperf: Invalid session number. Min: 0, Max: %d\r\n", nIperfSessions - 1U);
        return;
    }

    if((setTx && (txBuffSize >= 65536U)) || (setRx && (rxBuffSize >= 65536U)))
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "iperfs: 0 < size < 65536\r\n");
        return;
    }


    pIState = gIperfState + iperfIndex; 
    if(setTx)
    {
        pIState->txBuffSize = txBuffSize;
    }

    if(setRx)
    {
        pIState->rxBuffSize = rxBuffSize;
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "iperfs: OK, instance %d tx size is %d\r\n", iperfIndex, pIState->txBuffSize);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "iperfs: OK, instance %d rx size is %d\r\n", iperfIndex, pIState->rxBuffSize);

}

static void IperfSetState(tIperfState* pIState, int newState)
{
    uint8_t oldState = pIState->state;
    static uint8_t  iperf_async_request = 0U;

    if(newState == IPERF_STANDBY_STATE)
    {
        if(oldState != (uint8_t)IPERF_STANDBY_STATE)
        {   // clear the async request
            iperf_async_request--;
        }
    }
    else if (oldState == (uint8_t)IPERF_STANDBY_STATE)
    {   // going busy; set the async request
        iperf_async_request++;
        (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_ASYNC, 0); 
    }
    else
    {
        // do nothing
    }

    if(iperf_async_request == 0U)
    {
        (void)TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_ASYNC); 
    }

    pIState->state = (uint8_t)newState;

}


#endif  // defined(TCPIP_STACK_USE_IPERF)

