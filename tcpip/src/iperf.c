/*******************************************************************************
  TCP/IP iperf implementation

  Summary:
    Runs iperf client and server
    
  Description:
    - Implements iperf benchmarking
*******************************************************************************/

/*
Copyright (C) 2016-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#define UDP_FIN_RETRANSMIT_COUNT        10u     // iperf retransmits 10 times the last UDP packet,
#define UDP_FIN_RETRANSMIT_PERIOD       10      // at 10ms apart.


// TCP Maximum Segment Size - MSS;
#define IPERF_TCP_MSS  TCPIP_TCP_MAX_SEG_SIZE_TX


typedef enum {
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
    IPV4_ADDR        localAddr;     // local address/interface to use

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

//  long        mPendingACK;        // number of outstanding TCP ACKs
//  uint8_t     mRetransmit;

    uint32_t      timer;
    uint16_t      remainingTxData;
    uint16_t      availUDPTxSpace;


    uint32_t    txWaitTick;

    uint32_t    txBuffSize;
    uint32_t    rxBuffSize;

    tcpipSignalHandle  signalHandle; 

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

#define HEADER_VERSION1 0x80000000

typedef struct
{
    long id;
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
uint8_t  g_bfr[ IPERF_HEADER_BUFFER ];

// the transfer buffer size
// make it bigger than the Ethernet packet size for efficient reading from the socket
#define IPERF_TXFER_BUFFER_SIZE    1600
uint8_t txfer_buffer[IPERF_TXFER_BUFFER_SIZE];


static tIperfState gIperfState[TCPIP_IPERF_MAX_INSTANCES];

static int    nIperfSessions = 0;       // number of currently running instances

static int    iperfInitCount = 0;      // iperf module initialization count

static void CommandIperfStart(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void CommandIperfStop(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void CommandIperfNetIf(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void CommandIperfSize(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
// Iperf command table
static const SYS_CMD_DESCRIPTOR    iperfCmdTbl[]=
{
    {"iperf",       CommandIperfStart,  ":  <iperf> start cmd"},
    {"iperfk",      CommandIperfStop,   ": <iperfk> kill cmd"},
    {"iperfi",      CommandIperfNetIf,  ": <iperfi address>  interface cmd"},
    {"iperfs",      CommandIperfSize,   ": <iperfs tx/rx size> tx/rx size cmd"}
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

static void _IperfTCPRxSignalHandler(TCP_SOCKET hTCP, TCPIP_NET_HANDLE hNet, TCPIP_TCP_SIGNAL_TYPE sigType, const void* param);
static void _IperfUDPRxSignalHandler(TCP_SOCKET hTCP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param);

//****************************************************************************
// Implementation: public functions
//****************************************************************************

bool TCPIP_IPERF_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* initData)
{
    if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }

    // stack init

    if(iperfInitCount == 0)
    {   // first time we run
        int i;
        nIperfSessions = sizeof(gIperfState) / sizeof(*gIperfState);

        tIperfState* pIState;   
        for(i = 0, pIState = gIperfState; i < nIperfSessions; i++, pIState++)
        {
            memset( pIState, 0, sizeof(*pIState) );

            pIState->state = IPERF_STANDBY_STATE;
            pIState->stopRequested = false;

            pIState->tcpClientSock = INVALID_SOCKET;
            pIState->tcpServerSock = INVALID_SOCKET;
            pIState->udpSock = INVALID_SOCKET;
            pIState->txBuffSize = TCPIP_IPERF_TX_BUFFER_SIZE;
            pIState->rxBuffSize = TCPIP_IPERF_RX_BUFFER_SIZE;
               pIState->waitCount = 0;
               pIState->sockWaitToSend = 0;
               pIState->mTypeOfService = 0xFF;

            pIState->signalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_IPERF_Task, 0);
            if(pIState->signalHandle == 0)
            {   // failed
                return false;
            }

        }
        if(!SYS_CMD_ADDGRP(iperfCmdTbl, sizeof(iperfCmdTbl)/sizeof(*iperfCmdTbl), "iperf", ": iperf commands"))
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
        if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--iperfInitCount == 0)
            {   // all closed
                // release resources
                int i;
                tIperfState* pIState;   
                for(i = 0, pIState = gIperfState; i < nIperfSessions; i++, pIState++)
                {
                    if(pIState->signalHandle != 0)
                    {
                        _TCPIPStackSignalHandlerDeregister(pIState->signalHandle);
                        pIState->signalHandle = 0;
                    }
                }
            }
        }
    }

}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

void TCPIP_IPERF_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != 0)
    {
        int i = 0;
        // ASYNC or RX signals occurred
        tIperfState* pIState;   
        for(i = 0, pIState = gIperfState; i < nIperfSessions; i++, pIState++)
        {
            TCPIP_IPERF_Process(pIState);
        }
    }

}


// send a signal to the iperf module that data is available
// no manager alert needed since this normally results as a higher layer (TCP) signal
static void _IperfTCPRxSignalHandler(TCP_SOCKET hTCP, TCPIP_NET_HANDLE hNet, TCPIP_TCP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_TCP_SIGNAL_RX_DATA)
    {
        _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}

// send a signal to the iperf module that data is available
// no manager alert needed since this normally results as a higher layer (TCP) signal
static void _IperfUDPRxSignalHandler(TCP_SOCKET hTCP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_UDP_SIGNAL_RX_DATA)
    {
        _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}




static void TCPIP_IPERF_Process(tIperfState* pIState)   
{
    if (pIState->state == IPERF_STANDBY_STATE)
    { 
       return;
    }

    switch ( pIState->state )
    {
        /********************/
        /* RX Client States */
        /********************/

        case IPERF_RX_START_STATE:

            StateMachineRxStart(pIState);

            break;

#if defined(TCPIP_STACK_USE_UDP)
        case IPERF_UDP_RX_STATE:

            StateMachineUdpRx(pIState);

            break;

        case IPERF_UDP_RX_DRAIN_STATE:

            StateMachineUdpRxDrain(pIState);

            break;
#endif  // defined(TCPIP_STACK_USE_UDP)


#if defined(TCPIP_STACK_USE_UDP)
        case IPERF_UDP_RX_DONE_STATE:

            StateMachineUdpRxDone(pIState);

            break;
#endif  // defined(TCPIP_STACK_USE_UDP)


#if defined(TCPIP_STACK_USE_TCP)
        case IPERF_TCP_RX_LISTEN_STATE:

            StateMachineTcpListen(pIState);

            break;

        case IPERF_TCP_RX_STATE:

            StateMachineTcpRx(pIState);

            break;

        case IPERF_TCP_RX_DONE_STATE:

            StateMachineTcpRxDone(pIState);

#endif  // defined(TCPIP_STACK_USE_TCP)
            break;

        case IPERF_RX_DONE_STATE:

            StateMachineRxDone(pIState);

            break;


       /********************/
       /* TX Client states */
       /********************/


        case IPERF_TX_START_STATE:

            StateMachineTxStart(pIState);

            break;


        case IPERF_TX_ARP_RESOLVE_STATE:

           StateMachineTxArpResolve(pIState);

           break;

#if defined(TCPIP_STACK_USE_UDP)
        case IPERF_UDP_TX_OPEN_STATE:

            StateMachineUDPTxOpen(pIState);

            break;
#endif  // defined(TCPIP_STACK_USE_UDP)

#if defined(TCPIP_STACK_USE_TCP)
        case IPERF_TCP_TX_OPEN_STATE:

            StateMachineTCPTxOpen(pIState);

            break;

        case IPERF_TCP_TX_CONNECT_STATE:

            StateMachineTCPTxConnect(pIState);

            break;

        case IPERF_TCP_TX_SEGMENT_STATE:

            StateMachineTcpTxSegment(pIState);

            break;
#endif  // defined(TCPIP_STACK_USE_TCP)

#if defined(TCPIP_STACK_USE_UDP)
        case IPERF_UDP_TX_DATAGRAM_STATE:

            StateMachineUdpTxDatagram(pIState);

            break;
#endif  // defined(TCPIP_STACK_USE_UDP)

#if defined(TCPIP_STACK_USE_TCP)
        case IPERF_TCP_TX_DONE_STATE:

            StateMachineTcpTxDone(pIState);

            break;
#endif  // defined(TCPIP_STACK_USE_TCP)


#if defined(TCPIP_STACK_USE_UDP)
        case IPERF_UDP_TX_DONE_STATE:

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
    // pIState->mAmount = 0;
    // pIState->mDuration = 10*1000; // -t: default 10 sec
    // pIState->mInterval = 1000;   // -i: default 1 sec
    pIState->mMSS = IPERF_TCP_MSS;
    pIState->mDatagramSize = 1470; // -l: default 1470 bytes. UDP datagram size.
    pIState->totalLen = 0;
    pIState->pktId = 0;
    pIState->lastPktId = 0;
    pIState->errorCount = 0;
    pIState->outofOrder = 0;
    pIState->pktCount = 0;
    pIState->statusReported = 0;
    pIState->startTime = 0;
    pIState->stopTime = 0;

    pIState->lastCheckPktCount = 0;
    pIState->lastCheckPktId = 0;
    pIState->lastCheckErrorCount = 0;
    pIState->lastCheckTotalLen = 0;
    pIState->lastCheckTime = 0;

    pIState->isLastTransmit = false;

    pIState->txWaitTick = 0;
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

    if(ptr == 0)
    {
        *values = 0;
        return;
    }

    for (i = 0; i < count; i++)
    {
        tmp = 0;

        while ( (*ptr > (int8_t)'9') || (*ptr < (int8_t)'0') )
        {
            if ( (*ptr == (int8_t)' ') || (*ptr == (int8_t)0) ) return; // terminates at blank or NULL.

            ptr++;
        }

        while ( (*ptr <= (int8_t)'9') && (*ptr >= (int8_t)'0') )
        {
            tmp = tmp*10 + *ptr - '0';
            ptr++;
        }
        if ( (*ptr == (int8_t)'k') || (*ptr == (int8_t)'K') )
        {
            tmp = tmp * 1000;
            ptr++;
        }
        else if ( (*ptr == (int8_t)'m') || (*ptr == (int8_t)'M') )
        {
            tmp = tmp * 1000 * 1000;
            ptr++;
        }

        values[i] = tmp;
    }
}

//
// Convert the given BANDWIDTH in decimal with 'K' or 'M'
//
static void bw_conversion(SYS_CMD_DEVICE_NODE* pCmdIO, char *ptr, uint32_t *values)
{
    uint8_t j, len, decimal=1;
    uint32_t bw1=0, bw2=0,  mpf1=0, mpf2 = 1000000;

    len = strlen(ptr);

    if(strcmp((ptr+(len-1)),"M")==0)
        mpf1 = 1000 * 1000;
    
    if(strcmp((ptr+(len-1)),"K")==0)
        mpf1 = 100 * 10;
        
    if(mpf1!=0)    
    { 
        mpf2 = mpf1;
        for(j=0;j<len-1;j++)
        {
            //until "."
            if ( (*(ptr+j) <= (int8_t)'9') && (*(ptr+j) >= (int8_t)'0') && (decimal == 1) )
            {
                bw1 = bw1*10 + *(ptr+j) - '0';
            }
            if(*(ptr+j) == (int8_t)'.')
            {
                decimal = 0;
                bw1 = bw1 * mpf1;
            }
            if ( (*(ptr+j) <= (int8_t)'9') && (*(ptr+j) >= (int8_t)'0') && (decimal == 0) )
            {
                 bw2 = bw2*10 + *(ptr+j) - '0';
                 mpf2 = mpf2/10;
            }
        }
        bw2 = bw2 * mpf2;
        if(bw1 < mpf1)
            bw1 = bw1 * mpf1;
        
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
    uint32_t nAttempted;
    uint32_t nDropped;
    double kbps;
    uint32_t currentTime;
    uint32_t sec;
    uint32_t msec = 0;
    const void* cmdIoParam = NULL;

    uint32_t tickFreq = SYS_TMR_TickCounterFrequencyGet(); 
    currentTime = SYS_TMR_TickCountGet();

    cmdIoParam = pIState->pCmdIO->cmdIoParam;

    switch ( reportType )
    {
        case INTERVAL_REPORT:

            nDropped = pIState->errorCount - pIState->lastCheckErrorCount;

            // bits-per-msec == Kbps



            sec = (currentTime- pIState->lastCheckTime) / tickFreq;
            msec = (uint32_t)(((double) (currentTime - pIState->lastCheckTime)) / (((double)(tickFreq))/1000));

            if ( pIState->state == (uint8_t)IPERF_UDP_TX_DONE_STATE )
            {
               nAttempted = (pIState->lastPktId - pIState->lastCheckPktId) + nDropped;
            }
            else
            {
                nAttempted = pIState->pktId - pIState->lastCheckPktId;
            }

            if ( msec == 0u )
            {
                kbps = 0;
            }
            else
            {
                kbps = ((pIState->totalLen - pIState->lastCheckTotalLen)*((double) 8)) / msec;
            }

            sec = (pIState->lastCheckTime - pIState->startTime) / tickFreq;

            (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "    - [%2lu- %2lu sec] %3lu/ %3lu (%2lu%%)    %4lu Kbps\r\n",
                      (unsigned long)sec, 
                      (unsigned long)sec + ( (unsigned long) (pIState->mInterval / tickFreq) ),
                      (unsigned long)nDropped,
                      (unsigned long)nAttempted,
                      (nAttempted == 0u) ? 0 : ((unsigned long)nDropped*100/(unsigned long)nAttempted),
                      (unsigned long) (kbps + ((double) 0.5)));
            break;


        case SUBTOTAL_REPORT:
            // intentional fall-through
        case SESSION_REPORT:

           nDropped = pIState->errorCount;

           if (pIState->state == (uint8_t)IPERF_UDP_TX_DONE_STATE)
            {
               nAttempted = pIState->lastPktId + nDropped;
            }
            else
            {
                nAttempted = pIState->lastPktId;
            }

            msec = (uint32_t)(((double) (pIState->stopTime - pIState->startTime)) / (((double)(tickFreq))/1000));


            if ( msec == 0u )
            {
                kbps = 0;
            }
            else
            {
                kbps = (pIState->totalLen * ((double) 8)) / msec;
            }

            (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "    - [0.0- %lu.%lu sec] %3lu/ %3lu (%2lu%%)    %4lu Kbps\r\n",
                             (unsigned long)(msec/1000),
                             (unsigned long)((msec%1000)/100),
                             (unsigned long)nDropped,
                             (unsigned long)nAttempted,
                             (nAttempted == 0u) ? 0 : ((unsigned long)nDropped*100/(unsigned long)nAttempted),
                             (unsigned long) (kbps + ((double) 0.5)));
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
    if ( !pIState->mServerMode )
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
        if ( (pIState->tcpServerSock == INVALID_SOCKET) &&
            (pIState->tcpServerSock = TCPIP_TCP_ServerOpen(IP_ADDRESS_TYPE_IPV4, pIState->mServerPort, (IP_MULTI_ADDRESS*)&pIState->localAddr)) == INVALID_SOCKET )
        {
            /* error case */
            (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Create TCP socket failed\r\n");
            IperfSetState(pIState, IPERF_STANDBY_STATE);
            return;
        }

        TCPIP_TCP_SignalHandlerRegister(pIState->tcpServerSock, TCPIP_TCP_SIGNAL_RX_DATA, _IperfTCPRxSignalHandler, 0);
#if (TCPIP_TCP_DYNAMIC_OPTIONS != 0)
        if(!TCPIP_TCP_OptionsSet(pIState->tcpServerSock, TCP_OPTION_RX_BUFF, (void*)pIState->rxBuffSize))
        {
            (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Set of RX buffer size failed\r\n");
        }
#endif  // (TCPIP_TCP_DYNAMIC_OPTIONS != 0)

        IperfSetState(pIState, IPERF_TCP_RX_LISTEN_STATE);
        break;
#endif  // defined(TCPIP_STACK_USE_TCP)

#if defined(TCPIP_STACK_USE_UDP)
    case UDP_PROTOCOL:  // UDP
        if ( (pIState->udpSock = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_IPV4, pIState->mServerPort, (IP_MULTI_ADDRESS*)&pIState->localAddr)) == INVALID_UDP_SOCKET )
        {
            /* error case */
            (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Create UDP socket failed\r\n");
             IperfSetState(pIState, IPERF_STANDBY_STATE);
            return;
        }
        TCPIP_UDP_SignalHandlerRegister(pIState->udpSock, TCPIP_UDP_SIGNAL_RX_DATA, _IperfUDPRxSignalHandler, 0);

        IperfSetState(pIState, IPERF_UDP_RX_STATE);
        break;
#endif  // defined(TCPIP_STACK_USE_UDP)

    default:
        (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Protocol error\r\n");
        IperfSetState(pIState, IPERF_STANDBY_STATE);
        return;
    }
}












static void StateMachineRxDone(tIperfState* pIState)
{
    const void* cmdIoParam = pIState->pCmdIO->cmdIoParam;

   switch( pIState->mProtocol)
   {
#if defined(TCPIP_STACK_USE_UDP)
       case UDP_PROTOCOL:
           TCPIP_UDP_Close(  pIState->udpSock );
           break;
#endif  // defined(TCPIP_STACK_USE_UDP)

#if defined(TCPIP_STACK_USE_TCP)
       case TCP_PROTOCOL:
           TCPIP_TCP_Close( pIState->tcpServerSock );
           pIState->tcpServerSock = INVALID_SOCKET;
           break;
#endif  // defined(TCPIP_STACK_USE_TCP)

       default:
           break;
   }

    (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "\r\niperf instance %d: Rx done. Socket closed.\r\n", pIState - gIperfState);

    // Clear statistics
    ResetIperfCounters(pIState);

    // In server mode, continue to accept new session requests ...

    if ((pIState->mServerMode == true)  &&
        (pIState->stopRequested == false) )
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
       TCPIP_ARP_Resolve(pIState->pNetIf, &pIState->remoteSide.remoteIPaddress.v4Add);
   }
   IperfSetState(pIState, IPERF_TX_ARP_RESOLVE_STATE);
   pIState->timer = SYS_TMR_TickCountGet();
}

static void StateMachineTxArpResolve(tIperfState* pIState)
{
    const void* cmdIoParam = pIState->pCmdIO->cmdIoParam;

  if ( pIState->stopRequested == true )
  {
     (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: client session closed.\r\n");
     IperfSetState(pIState, IPERF_STANDBY_STATE);
     return;
  }

  if(TCPIP_STACK_NetMACTypeGet(pIState->pNetIf) == TCPIP_MAC_TYPE_PPP)
  {
      memset(pIState->remoteMACAddr.v, 0, sizeof(pIState->remoteMACAddr));
  }
  else if(!(TCPIP_Helper_IsMcastAddress(&pIState->remoteSide.remoteIPaddress.v4Add)))
  {
    if(!TCPIP_ARP_IsResolved(pIState->pNetIf, &pIState->remoteSide.remoteIPaddress.v4Add, &pIState->remoteMACAddr))
    {
        /* every 3 seconds print a warning */
        if( SYS_TMR_TickCountGet() - pIState->timer > 5 * SYS_TMR_TickCounterFrequencyGet() )
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
    long tmp2;

    if ( pIState->pktId == 0 ) {
        // The first tx packet
    }

    switch ( pIState->mProtocol )
    {
#if defined(TCPIP_STACK_USE_TCP)
        case TCP_PROTOCOL: // TCP

            // Required by iperf.
            pClientHdr = (tClientHdr *) pData;

            // We borrow the same tIperfPktInfo structure to embed
            // some useful (non-standard iperf) meta info.
            // Unfortunately, the order has to be reversed.

            pPktInfo = (tIperfPktInfo *) (pClientHdr + 1);

            break;
#endif  // defined(TCPIP_STACK_USE_TCP)

#if defined(TCPIP_STACK_USE_UDP)
        case UDP_PROTOCOL: // UDP

            // Both are required by iperf.

            pPktInfo = (tIperfPktInfo *) pData;
            pClientHdr = (tClientHdr *) (pPktInfo + 1);

            break;
#endif  // defined(TCPIP_STACK_USE_UDP)

        default:
            break;
    }

    // Client header:
    // Needed for all UDP packets.
    // For TCP, only the first two segments need this info. However,
    // there seems to be no harm to put it to all segments though.

    pClientHdr->flags = TCPIP_Helper_htonl( (uint32_t) 0);
    pClientHdr->numThreads = TCPIP_Helper_htonl((uint32_t) 1);
    pClientHdr->mPort = TCPIP_Helper_htonl((uint32_t) pIState->mServerPort);
    pClientHdr->bufferlen = TCPIP_Helper_htonl( (uint32_t) 0);
    pClientHdr->mWinBand = TCPIP_Helper_htonl(pIState->mTxRate);

    if ( pIState->mAmount != 0u )
    {
        pClientHdr->mAmount = TCPIP_Helper_htonl(pIState->mAmount);
    }
    else
    {
        pClientHdr->mAmount = TCPIP_Helper_htonl( - (long) (pIState->mDuration/10) );
    }

    // Additional info: needed for UDP only.
    // No harm to put it to all TCP segments though.

    if ( isTheLastTransmit == true )
    {
        // The last UDP tx packet. Some caveats:
        // 1. Iperf uses a negative Id for the last tx packet.
        // 2. Its id should not change during retransmit.

        pPktInfo->id = - ( (long) (pIState->pktId - pIState->nAttempts) );
    }
    else
    {
        pPktInfo->id = pIState->pktId;
    }

    pPktInfo->id = TCPIP_Helper_htonl(pPktInfo->id);

    currentTime = SYS_TMR_TickCountGet();

    uint32_t tickFreq = SYS_TMR_TickCounterFrequencyGet(); 
    pPktInfo->tv_sec = TCPIP_Helper_htonl(currentTime / tickFreq);

    /* get the remainder of the ticks using modulus */
    tmp2 = ((pIState->stopTime - pIState->startTime) % tickFreq);

    /* normalize  to uSecs */
    tmp2 =  tmp2 * 1000 / tickFreq; /* Convert to mSec */
    tmp2 *= 1000;   /* 1000 uSecs per mSec */


    pPktInfo->tv_usec = TCPIP_Helper_htonl( tmp2 );

    return;
}


static tIperfTxResult GenericTxStart(tIperfState* pIState)
{
    uint32_t currentTime;
    bool iperfKilled;
    const void* cmdIoParam = pIState->pCmdIO->cmdIoParam;    
    tIperfState* mpIState;
    uint8_t i;
    
    currentTime = SYS_TMR_TickCountGet();

    if ( currentTime < (pIState->nextTxTime - TCPIP_IPERF_TIMING_ERROR_MARGIN))
    {
        // Wait until we are scheduled to Tx.
        return IPERF_TX_WAIT;
    }

    iperfKilled = pIState->stopRequested;
    if ((iperfKilled == true) ||
            ((pIState->mDuration != 0u) &&
             (currentTime > (pIState->startTime + pIState->mDuration))) ||
            ((pIState->mAmount != 0u) &&
             (pIState->totalLen > pIState->mAmount)))
    {
        // Prepare to transmit the last packet.
        // Although the last packet needs to be retransmitted UDP_FIN_RETRANSMIT_COUNT times,
        // if we are in UDP mode.
        pIState->isLastTransmit = true;
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
        pIState->nAttempts = 0;
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
                TCPIP_TCP_Discard(pIState->tcpClientSock);
                return IPERF_TX_WAIT;
            }

            if ( TCPIP_TCP_WasReset(pIState->tcpClientSock) )
            {
                // We don't close the socket. We wait for user to "kill iperf" explicitly.
                (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "\r\niperf: Warning, TCP server disconnect detected\r\n");
                pIState->stopRequested = 1;
            }

            if  (( TCPIP_TCP_PutIsReady(pIState->tcpClientSock) <= pIState->mMSS ) && (!iperfKilled))
                return IPERF_TX_WAIT;

            break;
#endif  // defined(TCPIP_STACK_USE_TCP)

#if defined(TCPIP_STACK_USE_UDP)
        case UDP_PROTOCOL:
            /* Manage socket */
            if( TCPIP_UDP_GetIsReady(pIState->udpSock) > 0u )
            {
                TCPIP_UDP_Discard(pIState->udpSock);
                return IPERF_TX_WAIT;
            }
      
            //(pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "\n..Present TOS:%d, Prev: ",gIperfState[iperf_task_index].mTypeOfService); 
            for(i = 0, mpIState = gIperfState; i < nIperfSessions; i++, mpIState++)
             {            
                if(mpIState == pIState) continue;
                
                //if((mpIState != pIState) && (tie_break[i]<3))
                if(mpIState->waitCount < 3)    
                {
                //(pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "%d(wait:%d), mpt:%d  ",pIState->mTypeOfService,pIState->sockWaitToSend, mpIState->mTypeOfService);

                switch(pIState->mTypeOfService)
                {
                    case TCPIP_IPV4_DELAY_LOW:  /*0x20 BK, check for other 3-AC's - VI,V0,BE*/
                        if(mpIState->mTypeOfService == TCPIP_IPV4_RELIABILITY_HIGH || mpIState->mTypeOfService == TCPIP_IPV4_THROUGHPUT_HIGH || mpIState->mTypeOfService == TCPIP_IPV4_TOS_DEFAULT)
                            if(mpIState->sockWaitToSend)
                            {
                                mpIState->waitCount += 1;
                                return IPERF_TX_WAIT;
                            }
                         break;   
                    case TCPIP_IPV4_RELIABILITY_HIGH:  /*0xA0 Video, check for only Voice*/
                        if(mpIState->mTypeOfService == TCPIP_IPV4_THROUGHPUT_HIGH)
                            if(mpIState->sockWaitToSend)
                            {
                                mpIState->waitCount += 1;
                                //(pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "%d, ",mpIState->sockWaitToSend);
                                return IPERF_TX_WAIT;
                            }
                        break;
                    case TCPIP_IPV4_TOS_DEFAULT: /* 0x00  BE*/
                        if(mpIState->mTypeOfService == TCPIP_IPV4_RELIABILITY_HIGH || mpIState->mTypeOfService == TCPIP_IPV4_THROUGHPUT_HIGH )
                            if(mpIState->sockWaitToSend)
                            {
                                mpIState->waitCount += 1;
                                return IPERF_TX_WAIT;
                            }
                        break;
                    default:
                            /*Voice packet: process*/
                        break;
                }
                }
                if(mpIState->waitCount == 3) mpIState->waitCount = 0;
            }            

            if ( TCPIP_UDP_TxPutIsReady(pIState->udpSock, pIState->mDatagramSize) < pIState->mDatagramSize )
            {
                pIState->sockWaitToSend += 1;
                
                if(pIState->txWaitTick == 0)
                {     // beginning wait period
                    pIState->txWaitTick = SYS_TMR_TickCountGet() + ((TCPIP_IPERF_TX_WAIT_TMO * SYS_TMR_TickCounterFrequencyGet()) + 999)/1000;
                    return IPERF_TX_WAIT;
                }
                else if((int32_t)(SYS_TMR_TickCountGet() - pIState->txWaitTick) < 0)
                { // wait some more
                    return IPERF_TX_WAIT;
                }

                // TX ready timeout
                (pIState->pCmdIO->pCmdApi->print)(cmdIoParam, "iperf: Failed to get %d bytes socket TX space\r\n", pIState->mDatagramSize);
                return IPERF_TX_FAIL;
            }
            else
            {    // reset retry tick counter
                pIState->txWaitTick = 0;
                pIState->sockWaitToSend -= 1;                
            }
            break;
#endif  // defined(TCPIP_STACK_USE_UDP)

        default:
            break;
    }



    GenericTxHeaderPreparation(pIState, g_bfr, pIState->isLastTransmit);

    switch( pIState->mProtocol)
    {
#if defined(TCPIP_STACK_USE_TCP)
        case TCP_PROTOCOL:
            pIState->remainingTxData = (pIState->mMSS - IPERF_HEADER_BUFFER);

            if (( TCPIP_TCP_ArrayPut(pIState->tcpClientSock, (uint8_t*) g_bfr, IPERF_HEADER_BUFFER) != IPERF_HEADER_BUFFER ) && (!iperfKilled))
            {
                (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Socket send failed\r\n");
                pIState->errorCount++;
                return IPERF_TX_FAIL;
            }

            break;
#endif  // defined(TCPIP_STACK_USE_TCP)


#if defined(TCPIP_STACK_USE_UDP)
        case UDP_PROTOCOL:

            pIState->remainingTxData = (pIState->mDatagramSize - IPERF_HEADER_BUFFER);

            if ( TCPIP_UDP_ArrayPut(pIState->udpSock, g_bfr, IPERF_HEADER_BUFFER) != IPERF_HEADER_BUFFER )
            {
                (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Socket send failed\r\n");
                pIState->errorCount++;
                return IPERF_TX_FAIL;
            }
            break;
#endif  // defined(TCPIP_STACK_USE_UDP)

        default:
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
                    (unsigned long)(pIState->mPktPeriod*1000/SYS_TMR_TickCounterFrequencyGet()) );

        }

        pIState->pktId++;
        pIState->pktCount++;

#if defined(TCPIP_STACK_USE_UDP)
        if ( pIState->mProtocol == UDP_PROTOCOL )
        {
            pIState->totalLen += pIState->mDatagramSize;
        }
#endif  // defined(TCPIP_STACK_USE_UDP)

#if defined(TCPIP_STACK_USE_TCP)
        if ( pIState->mProtocol == TCP_PROTOCOL )
        {

            pIState->totalLen += pIState->mMSS;
        }
#endif  // defined(TCPIP_STACK_USE_TCP)


    }

    pIState->lastPktId = pIState->pktId - 1;



    if ( (int32_t)(SYS_TMR_TickCountGet() - pIState->lastCheckTime) >= (pIState->mInterval - TCPIP_IPERF_TIMING_ERROR_MARGIN) )
    {
        // Time to report statistics
        ReportBW_Jitter_Loss(pIState, INTERVAL_REPORT);

        //pIState->lastCheckPktCount = pIState->pktCount;
    }


    if ( pIState->isLastTransmit == true )
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
                break;
        }
        pIState->sockWaitToSend = 0;

        pIState->stopTime = SYS_TMR_TickCountGet();
    }

}




static void GenericTxDone(tIperfState* pIState)
{
    const void* cmdIoParam = pIState->pCmdIO->cmdIoParam;
    if ( pIState->statusReported == 0u )
    {
        ReportBW_Jitter_Loss(pIState, SESSION_REPORT);
        pIState->statusReported = 1;
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
    TCPIP_TCP_Close(pIState->tcpClientSock);
    pIState->tcpClientSock = INVALID_SOCKET;
}

static void StateMachineTcpTxSegment(tIperfState* pIState)
{
    tIperfTxResult txRes = GenericTxStart(pIState);

    if(txRes == IPERF_TX_OK)
    {   // go ahead and transmit
       TcpTxFillSegment(pIState);
       TCPIP_TCP_Flush(pIState->tcpClientSock);
       GenericTxEnd(pIState);
    }
    else if(txRes == IPERF_TX_FAIL)
    {
        IperfSetState(pIState, IPERF_TCP_TX_DONE_STATE);
        pIState->stopTime = SYS_TMR_TickCountGet();
    }
    // else wait...
}

/* This routine does a piecewise send, because the entire RAM buffer may not be available for putArray */
static void TcpTxFillSegment(tIperfState* pIState)
{
    uint16_t chunk_size, sent_bytes;



    while( pIState->remainingTxData > 0u )
    {
        chunk_size = pIState->remainingTxData;
        if(chunk_size > sizeof(txfer_buffer))
        {
            chunk_size = sizeof(txfer_buffer);
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

   if  ( (pIState->tcpClientSock = TCPIP_TCP_ClientOpen(IP_ADDRESS_TYPE_IPV4, pIState->mServerPort, 0)) == INVALID_SOCKET )
   {
       /* error case */
        (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Create TCP socket failed\r\n");
        IperfSetState(pIState, IPERF_STANDBY_STATE);
        return;
    }
   TCPIP_TCP_SignalHandlerRegister(pIState->tcpClientSock, TCPIP_TCP_SIGNAL_RX_DATA, _IperfTCPRxSignalHandler, 0);

   if(pIState->localAddr.Val != 0)
   {
       TCPIP_TCP_Bind(pIState->tcpClientSock, IP_ADDRESS_TYPE_IPV4, pIState->mServerPort, (IP_MULTI_ADDRESS*)&pIState->localAddr);
   }
   TCPIP_TCP_RemoteBind(pIState->tcpClientSock, IP_ADDRESS_TYPE_IPV4, 0,  (IP_MULTI_ADDRESS*)&pIState->remoteSide.remoteIPaddress);
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
    if(!TCPIP_TCP_OptionsSet(pIState->tcpClientSock, TCP_OPTION_TX_BUFF, (void*)pIState->txBuffSize))
    {
        (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Set of TX buffer size failed\r\n");
    }
#endif  // (TCPIP_TCP_DYNAMIC_OPTIONS != 0)

    TCPIP_TCP_Connect(pIState->tcpClientSock);
    pIState->timer = SYS_TMR_TickCountGet();
    
    /* Fill the buffer with ASCII char T */
    memset( txfer_buffer, 0x54, sizeof(txfer_buffer));
}

static void StateMachineTcpRxDone(tIperfState* pIState)
{
    if ( pIState->statusReported == 0u )
    {
        ReportBW_Jitter_Loss(pIState, SESSION_REPORT);
        pIState->statusReported = 1;
    }

    IperfSetState(pIState, IPERF_RX_DONE_STATE);
}

static void StateMachineTcpRx(tIperfState* pIState)
{
    uint16_t length;
    const void* cmdIoParam = pIState->pCmdIO->cmdIoParam;

    if( (length = TCPIP_TCP_GetIsReady(pIState->tcpServerSock)) == 0 )
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
       pIState->totalLen += length;

       /* read the remaining datagram payload */
       /* a UdpDiscard would be disingenuous, because it would not reflect the bandwidth at L7 */
       while ( length > 0 )
       {
           uint16_t chunk_size;

           if ( length <  (uint16_t)sizeof(txfer_buffer) )
           {
               chunk_size = length;
           }
           else
           {
               chunk_size = sizeof(txfer_buffer);
           }

           TCPIP_TCP_ArrayGet( pIState->tcpServerSock, txfer_buffer, chunk_size);
           length -= chunk_size;
       }

    }

    if ((pIState->pktId != (long)0) &&
       ((int32_t)(SYS_TMR_TickCountGet() - pIState->lastCheckTime) > pIState->mInterval) )
    {
         // Time to report statistics
         ReportBW_Jitter_Loss(pIState, INTERVAL_REPORT);
    }

    if ( pIState->stopRequested == true )
    {
       IperfSetState(pIState, IPERF_TCP_RX_DONE_STATE);
       pIState->stopTime = SYS_TMR_TickCountGet();

       return;
    }
}

static void StateMachineTcpListen(tIperfState* pIState)
{

   if ( pIState->stopRequested == true )
   {
        IperfSetState(pIState, IPERF_RX_DONE_STATE);
        return;
   }

   if( TCPIP_TCP_IsConnected(pIState->tcpServerSock) )
   {
      TCP_SOCKET_INFO tcpSocketInfo = {0};
      TCPIP_TCP_SocketInfoGet( pIState->tcpServerSock, &tcpSocketInfo);
      memcpy ( (void *) &pIState->remoteSide, &tcpSocketInfo, sizeof ( TCP_SOCKET_INFO) );
      IperfSetState(pIState, IPERF_TCP_RX_STATE);

      /* clear the stack's reset flag */
      TCPIP_TCP_WasReset(pIState->tcpServerSock);
   }
}

static void StateMachineTCPTxConnect(tIperfState* pIState)
{

    const void* cmdIoParam = pIState->pCmdIO->cmdIoParam;
    if ( pIState->stopRequested == true )
    {
        IperfSetState(pIState, IPERF_TCP_TX_DONE_STATE);
        return;
    }

    if( !TCPIP_TCP_IsConnected(pIState->tcpClientSock) )
    {

        // Time out if too much time is spent in this state
        if(SYS_TMR_TickCountGet()- pIState->timer > 5*SYS_TMR_TickCounterFrequencyGet())
        {
            TCPIP_TCP_Close(pIState->tcpClientSock);
            pIState->tcpClientSock = INVALID_SOCKET;
            (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: TCP Client connection timeout\r\n");
            IperfSetState(pIState, IPERF_TCP_TX_DONE_STATE);
        }

        return;
    }

    /* reset the reset flag so we don't get a false positive */
    TCPIP_TCP_WasReset(pIState->tcpClientSock);

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

    TCPIP_UDP_Close(pIState->udpSock );
}


static void StateMachineUdpTxDatagram(tIperfState* pIState)
{

    TCPIP_UDP_OptionsSet(pIState->udpSock, UDP_OPTION_TOS, (void*)(unsigned int)pIState->mTypeOfService);   

    tIperfTxResult txRes = GenericTxStart(pIState);

    if ( txRes == IPERF_TX_OK )
    {   // go ahead and transmit
       uint16_t txData = UdpTxFillDatagram(pIState);
       if(TCPIP_UDP_Flush(pIState->udpSock) == 0)
       {   // failed; discard data
           TCPIP_UDP_TxOffsetSet(pIState->udpSock, 0, 0);
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
    // else wait...
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
        if(chunk_size > sizeof(txfer_buffer))
        {
            chunk_size = sizeof(txfer_buffer);
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
    
    if ( (pIState->udpSock = TCPIP_UDP_ClientOpen(IP_ADDRESS_TYPE_IPV4, pIState->mServerPort, (IP_MULTI_ADDRESS*)&pIState->remoteSide.remoteIPaddress.v4Add)) == INVALID_UDP_SOCKET )
    {
        /* error case */
        (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Create UDP socket failed\r\n");
        IperfSetState(pIState, IPERF_STANDBY_STATE);
        return;
    }

    TCPIP_UDP_SignalHandlerRegister(pIState->udpSock, TCPIP_UDP_SIGNAL_RX_DATA, _IperfUDPRxSignalHandler, 0);
    
    if(!TCPIP_UDP_OptionsSet(pIState->udpSock, UDP_OPTION_TX_BUFF, (void*)pIState->mDatagramSize))
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
    if(!TCPIP_UDP_OptionsSet(pIState->udpSock, UDP_OPTION_TX_QUEUE_LIMIT, (void*)txQLimit))
    {
        (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Set of TX queuing limit failed\r\n");
    }

    TCPIP_UDP_SocketNetSet(pIState->udpSock, pIState->pNetIf);
    TCPIP_UDP_SocketInfoGet(pIState->udpSock, &UdpSkt);
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
    memset( txfer_buffer, 0x55, sizeof(txfer_buffer));

}

static void StateMachineUdpRxDone(tIperfState* pIState)
{
    tIperfPktInfo *pPktInfo;
    tServerHdr *pServer_hdr;
    float tmp2;


    if ( pIState->statusReported == 0u )
    {
        ReportBW_Jitter_Loss(pIState, SESSION_REPORT);
        pIState->statusReported = 1;
    }

    /* Drain any waiting pkts */
    if (  TCPIP_UDP_GetIsReady(pIState->udpSock)  )
    {
        TCPIP_UDP_Discard(pIState->udpSock);
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

      pPktInfo = (tIperfPktInfo *) g_bfr;
      pServer_hdr = (tServerHdr *) (pPktInfo +1);

      pPktInfo->id = TCPIP_Helper_htonl( -pIState->lastPktId );
      pPktInfo->tv_sec = 0;
      pPktInfo->tv_usec = 0;

      pServer_hdr->flags = TCPIP_Helper_htonl(HEADER_VERSION1);
      pServer_hdr->total_len1 = 0;
      pServer_hdr->total_len2 = TCPIP_Helper_htonl( (uint32_t) pIState->totalLen);

      uint32_t tickFreq = SYS_TMR_TickCounterFrequencyGet(); 
      pServer_hdr->stop_sec =  TCPIP_Helper_htonl( (uint32_t) (pIState->stopTime - pIState->startTime) / tickFreq);

      /* get the remainder of the ticks using modulus */
      tmp2 = ((pIState->stopTime - pIState->startTime) % tickFreq);

      /* normalize  to uSecs */
      tmp2 =  tmp2 * 1000 / tickFreq; /* Convert to mSec */
      tmp2 *= 1000;   /* 1000 uSecs per mSec */


      pServer_hdr->stop_usec = TCPIP_Helper_htonl( (uint32_t) tmp2 );
      pServer_hdr->error_cnt = TCPIP_Helper_htonl( (uint32_t)  pIState->errorCount);;
      pServer_hdr->outorder_cnt = TCPIP_Helper_htonl( (uint32_t) pIState->outofOrder);
      pServer_hdr->datagrams = TCPIP_Helper_htonl( (uint32_t) pIState->lastPktId);
      pServer_hdr->jitter1 = 0;
      pServer_hdr->jitter2 = 0;

      TCPIP_UDP_ArrayPut(pIState->udpSock, (uint8_t*)g_bfr, IPERF_HEADER_BUFFER);

      uint8_t tmpBuffer[128-IPERF_HEADER_BUFFER];
      memset(tmpBuffer, 0, sizeof(tmpBuffer));
      TCPIP_UDP_ArrayPut(pIState->udpSock, tmpBuffer, sizeof(tmpBuffer));
      
      TCPIP_UDP_Flush(pIState->udpSock );

    }

}

static void StateMachineUdpRxDrain(tIperfState* pIState)
{
    if( TCPIP_UDP_GetIsReady(pIState->udpSock) > (uint8_t)0 )
    {
         TCPIP_UDP_Discard(pIState->udpSock);
         return;
    }

   /* If iperf kill was done, just jump to closing the socket */
   if ( pIState->stopRequested )
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
    uint16_t length =0;
    tIperfPktInfo *pPktInfo;
    UDP_SOCKET_INFO UdpSkt;
    IPV4_ADDR      lclIpAddr, remIpAddr;
    const void* cmdIoParam = pIState->pCmdIO->cmdIoParam;


    // Do nothing if no data is waiting
    while( (length = TCPIP_UDP_GetIsReady(pIState->udpSock)) >= (uint16_t)(sizeof(tIperfPktInfo)) )
    {
       /* The GetArray should not fail... */
       if ( TCPIP_UDP_ArrayGet(pIState->udpSock, (uint8_t*)g_bfr, sizeof(tIperfPktInfo)) != sizeof(tIperfPktInfo) )
       {
          (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: UDP Get Array Failed\r\n");
          IperfSetState(pIState, IPERF_UDP_RX_DRAIN_STATE);
          return;
       }

       pPktInfo = (tIperfPktInfo *) g_bfr;
       pIState->pktId = TCPIP_Helper_htonl(pPktInfo->id);

       if ( (pIState->pktCount == (uint32_t)0) && (pIState->pktId < (long)0) )
       {
          // Ignore retransmits from previous session.
          TCPIP_UDP_Discard(pIState->udpSock);
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
             pIState->errorCount    +=  pIState->pktId - 1;
          }

          pIState->lastPktId = pIState->pktId;

          TCPIP_UDP_SocketInfoGet(pIState->udpSock, &UdpSkt);
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

          TCPIP_UDP_Discard(pIState->udpSock);

          continue;
      }

      pIState->totalLen += length;

      if ( pIState->pktId < 0 )
      {
         // this is the last datagram
         pIState->pktId = - pIState->pktId;

         pIState->stopTime = SYS_TMR_TickCountGet();
        //pIState->remoteStopTime = IPERFTOHL(pPktInfo->tv_sec);

        pIState->nAttempts = 0;
        IperfSetState(pIState, IPERF_UDP_RX_DRAIN_STATE);
      }

      if ( pIState->pktId != pIState->lastPktId+1 )
      {
         if ( pIState->pktId < pIState->lastPktId+1 )
         {
            pIState->outofOrder++;
         }
         else
         {
            pIState->errorCount += pIState->pktId - (pIState->lastPktId+1);
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
      length -=  sizeof(tIperfPktInfo);
      while ( length > 0 )
      {
          uint16_t chunk_size;

          if ( length <  (uint16_t)sizeof(txfer_buffer) )
          {
              chunk_size = length;
          }
          else
          {
              chunk_size = sizeof(txfer_buffer);
          }

          TCPIP_UDP_ArrayGet(pIState->udpSock, txfer_buffer, chunk_size);
          length -= chunk_size;
      }


    }  /* end got a datagram */

    if ( (pIState->pktCount != (uint32_t)0) &&
         ((int32_t)(SYS_TMR_TickCountGet() - pIState->lastCheckTime) > pIState->mInterval) )
    {
        if ( pIState->pktCount == pIState->lastCheckPktCount )
        {
          // No events in pIState->mInterval msec, we timed out
          (pIState->pCmdIO->pCmdApi->msg)(cmdIoParam, "iperf: Rx timed out\r\n");

          pIState->stopTime = SYS_TMR_TickCountGet();

          pIState->nAttempts = 0;
          IperfSetState(pIState, IPERF_UDP_RX_DRAIN_STATE);
        }
        else
        {
          ReportBW_Jitter_Loss(pIState, INTERVAL_REPORT);
        }
    }

    if ( pIState->stopRequested == true )
    {
        IperfSetState(pIState, IPERF_UDP_RX_DRAIN_STATE);
        return;
    }

}

#endif  // defined(TCPIP_STACK_USE_UDP)

static tIperfState* GetIperfSession(void)
{

    int i = 0;

    tIperfState* pIState;   
    for(i = 0, pIState = gIperfState; i < nIperfSessions; i++, pIState++)
    {
        if(pIState->state == (uint8_t)IPERF_STANDBY_STATE)
        {
            return pIState;
        }
    }
    return 0;
}

static void CommandIperfStart(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t i, tos, len;
    char *ptr;
    uint32_t tickFreq;
    uint32_t values[4] = {0}, bw=0;
    
    float pktRate;
    uint16_t payloadSize = 0, asciTos;
    const void* cmdIoParam = pCmdIO->cmdIoParam;

    tIperfState* pIState = GetIperfSession();   
    
    if(pIState == 0)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "\r\niperf: All instances busy. Retry later!\r\n");
        return;
    }

    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\r\niperf: Starting session instance %d\r\n", pIState - gIperfState);

    // preparation for new iperf test
    pIState->mServerMode = false;
#if defined(TCPIP_STACK_USE_TCP)
    pIState->mProtocol = TCP_PROTOCOL;              // default is TCP mode.
#else
    pIState->mProtocol = UDP_PROTOCOL;
#endif  // defined(TCPIP_STACK_USE_TCP)
    pIState->stopRequested = false;

    pIState->mServerPort = TCPIP_IPERF_SERVER_PORT;     // -p. default: server port 5001

    pIState->mTxRate = ((uint32_t) TCPIP_IPERF_TX_BW_LIMIT*1000)*((uint32_t) 1000);     // -b or -x. Target tx rate.
    // KS: default tx rate for iperf is actually 1Mbps.

    tickFreq = SYS_TMR_TickCounterFrequencyGet(); 
    pIState->mAmount = 0;           // -n: default 0.
    pIState->mDuration = ((uint32_t) 10)*(tickFreq); // -t: default 10 sec.
    pIState->mInterval =  tickFreq;     // -i: default 1 sec.

    pIState->mTypeOfService = 0;       //-S, --tos (Type Of Service): default 0: BestEffort
    // remember the console we've been invoked from
    pIState->pCmdIO = pCmdIO;

    
    // Initialize statistics

    ResetIperfCounters(pIState);

    for (i = 1; i < argc; i++)
    {
        if ((memcmp(argv[i], "-s", 2) == 0) || (memcmp(argv[i], "--server", 5) == 0) )
        {
            // Function as an iperf server.

            pIState->mServerMode = true;
        }
#if defined(TCPIP_STACK_USE_UDP)
        else if ((memcmp(argv[i], "-u", 2) == 0) || (memcmp(argv[i], "--udp", 5) == 0) )
        {
            // iperf UDP mode.
            pIState->mProtocol = UDP_PROTOCOL;
        }
        else if ((memcmp(argv[i], "-b", 2) == 0) || (memcmp(argv[i], "--bandwidth", 5) == 0) )
        {
            // iperf UDP mode.
            pIState->mProtocol = UDP_PROTOCOL;

            // Next argument should be the target rate, in bps.
            i++;
            ptr = argv[i];
            len = strlen(ptr);
            
            if((strcmp((ptr+(len-1)),"M")==0) || (strcmp((ptr+(len-1)),"K")==0))
            {  
                bw_conversion(pCmdIO, ptr, values);
                bw = values[0]+values[1];
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
        else if ((memcmp(argv[i], "-x", 2) == 0) || (memcmp(argv[i], "--xmitrate", 5) == 0) )
        {
            // NON-STANDARD IPERF OPTION. Set the max TCP tx rate.
            // Next argument should be the target rate, in bps.
            i++;
            ptr = argv[i];

            ascii_to_u32s(ptr, values, 1);

            pIState->mTxRate = values[0];
        }
#endif  // defined(TCPIP_STACK_USE_TCP)
        else if ((memcmp(argv[i], "-c", 2) == 0) || (memcmp(argv[i], "--client", 5) == 0) )
        {
            // Function as an iperf client.
            pIState->mServerMode = false;

            // Next argument should be the server IP, such as "192.168.1.100".
            i++;
            ptr = argv[i];
            ascii_to_u32s(ptr, values, 4);

            pIState->remoteSide.remoteIPaddress.v4Add.v[0] = values[0];
            pIState->remoteSide.remoteIPaddress.v4Add.v[1] = values[1];
            pIState->remoteSide.remoteIPaddress.v4Add.v[2] = values[2];
            pIState->remoteSide.remoteIPaddress.v4Add.v[3] = values[3]; 
        }
        else if ((memcmp(argv[i], "-t", 2) == 0) || (memcmp(argv[i], "--time", 5) == 0) )
        {
            // Next argument should be the (client tx) duration, in seconds.
            i++;
            ptr = argv[i];
            ascii_to_u32s(ptr, values, 1);

            pIState->mDuration = values[0] * tickFreq;
            pIState->mAmount = 0;
        }
        else if ((memcmp(argv[i], "-n", 2) == 0) || (memcmp(argv[i], "--num", 5) == 0) )
        {
            // Next argument should be the (client tx) size, in bytes.
            i++;
            ptr = argv[i];
            ascii_to_u32s(ptr, values, 1);

            pIState->mAmount = values[0];
            pIState->mDuration = 0;
        }
        else if ((memcmp(argv[i], "-S", 2) == 0) || (memcmp(argv[i], "--tos", 5) == 0) )
        {
            // Next argument should be the (client tx) TOS
            i++;
            if(argv[i][1] == 'x')
            {
                ((uint8_t*)&asciTos)[1] = argv[i][2];
                ((uint8_t*)&asciTos)[0] = argv[i][3];                    
                tos = hexatob(asciTos);
            }
            else
            {
                ptr = argv[i];
                ascii_to_u32s(ptr, values, 1);
                tos = values[0];                
            }

            switch(tos)
            {
                case IPERF_TOS_VO:
                    pIState->mTypeOfService = TCPIP_IPV4_THROUGHPUT_HIGH;                  
                    break;
                case IPERF_TOS_VI:
                    pIState->mTypeOfService = TCPIP_IPV4_RELIABILITY_HIGH;                                      
                    break;
                case IPERF_TOS_BK:
                    pIState->mTypeOfService = TCPIP_IPV4_DELAY_LOW;                                      
                    break;
                case IPERF_TOS_BE:
                    pIState->mTypeOfService = TCPIP_IPV4_TOS_DEFAULT;                                      
                    break;
                 default:   
                    pIState->mTypeOfService = TCPIP_IPV4_TOS_DEFAULT;                                                          
            }

            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\nGiven TOS value is %d, mTypeOfService:%d",tos,pIState->mTypeOfService);
            
        }
        
#if defined(TCPIP_STACK_USE_TCP)
        else if ((memcmp(argv[i], "-M", 2) == 0) ||
                (memcmp(argv[i], "--mss", 5) == 0) )
        {
            // Next argument should be the (client tcp tx) MSS size, in bytes.

            i++;
            ptr = argv[i];

            ascii_to_u32s(ptr, values, 1);

            pIState->mMSS = values[0];
        }
#endif  // defined(TCPIP_STACK_USE_TCP)

        else if ((memcmp(argv[i], "-i", 2) == 0) || (memcmp(argv[i], "--interval", 5) == 0) )
        {
            // Next argument should be the report interval, in seconds.
            i++;
            ptr = argv[i];
            ascii_to_u32s(ptr, values, 1);

            pIState->mInterval = values[0] * tickFreq; // Convert to msec
        }
        else if ((memcmp(argv[i], "-p", 2) == 0) || (memcmp(argv[i], "--port", 6) == 0) )
        {
            // Next argument should be the port number.
            i++;
            ptr = argv[i];
            ascii_to_u32s(ptr, values, 1);

            pIState->mServerPort = values[0];       // -p. default: server port 5001
        }
#if defined(TCPIP_STACK_USE_UDP)
        else if ((memcmp(argv[i], "-l", 2) == 0) || (memcmp(argv[i], "--len", 5) == 0) )
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
    }

    switch (pIState->mServerMode)
    {
        case 0:
            // iperf client

            // set the interface to work on
            if((pIState->pNetIf = TCPIP_STACK_IPAddToNet(&pIState->localAddr, false)) == 0)
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
                payloadSize =   pIState->mDatagramSize;
            }
#endif  // defined(TCPIP_STACK_USE_UDP)

            pktRate =  (float) (pIState->mTxRate / 8) / (float) payloadSize;
            pIState->mPktPeriod =  (uint32_t) ( (float) tickFreq / pktRate );

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
    }
    
}

static void CommandIperfStop(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    tIperfState* pIState;   
    bool okParam = false;
    int iperfIndex = 0;

    if(argc == 1)
    {   // no params passed
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "iperfk: Using index 0\r\n");
        okParam = true;
    }
    else if(argc == 3)
    {   // valid number of args
        if((strcmp(argv[1], "-i") == 0) || (strcmp(argv[1], "--index") == 0))
        {
            iperfIndex = atoi(argv[2]);
            okParam = true;
        }
    }

    if(!okParam)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: iperfk <-i index>\r\n");
        return;
    }

    if(iperfIndex < 0 || iperfIndex >= nIperfSessions)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\r\niperf: Invalid session number. Min: 0, Max: %d\r\n", nIperfSessions - 1);
        return;
    }

    pIState = gIperfState + iperfIndex; 

    if(pIState->state == IPERF_STANDBY_STATE)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "\r\niperf session: not started!\r\n");
    }
    else
    {
        pIState->stopRequested = true;
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\r\niperf: trying to stop iperf instance %d...\r\n", iperfIndex);
    }

}

static void CommandIperfNetIf(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    IPV4_ADDR   ipAddr;
    tIperfState* pIState;   
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    int iperfIndex = 0;     // assume index 0 if not specified
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
                iperfIndex = atoi(paramVal);
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

    if(iperfIndex < 0 || iperfIndex >= nIperfSessions)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\r\niperf: Invalid session number. Min: 0, Max: %d\r\n", nIperfSessions - 1);
        return;
    }

    pIState = gIperfState + iperfIndex; 

    if (pIState->state != IPERF_STANDBY_STATE)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\r\niperfi: cannot change the ip address while session: %d running!\r\n", iperfIndex);
        return;
    }

    pIState->localAddr.Val = ipAddr.Val;
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "iperfi: OK, set the IP address to instance: %d\r\n", iperfIndex);

}

static void CommandIperfSize(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    bool        setTx, setRx;
    uint32_t    txBuffSize, rxBuffSize;
    tIperfState* pIState;   
    int iperfIndex;

    const void* cmdIoParam = pCmdIO->cmdIoParam;


    setTx = setRx = 0;
    txBuffSize = rxBuffSize = 0;
    iperfIndex = 0;     // assume index 0 if not specified

    int currIx = 1; // 1st param is always the command name, skip it
    while(currIx + 1 < argc)
    { 
        char* param = argv[currIx];
        char* paramVal = argv[currIx + 1];

        if(strcmp(param, "-tx") == 0)
        {
            setTx = true;
            txBuffSize = atoi(paramVal);
        }
        else if(strcmp(param, "-rx") == 0)
        {
            setRx = true;
            rxBuffSize = atoi(paramVal);
        }
        else if((strcmp(param, "-i") == 0) || (strcmp(param, "--index") == 0))
        {
            iperfIndex = atoi(paramVal);
        }
        else
        {
            (*pCmdIO->pCmdApi->msg)(cmdIoParam, "Usage: iperfs <-tx size> <-rx size> <-i index>\r\n");
        }

        currIx += 2;
    }

    if(iperfIndex < 0 || iperfIndex >= nIperfSessions)
    {
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "\r\niperf: Invalid session number. Min: 0, Max: %d\r\n", nIperfSessions - 1);
        return;
    }

    if((setTx && (txBuffSize <= 0 || txBuffSize >= 65536)) || (setRx && (rxBuffSize <= 0 || rxBuffSize >= 65536)))
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
    static uint8_t  iperf_async_request = 0;
    
    if(newState == IPERF_STANDBY_STATE)
    {
        if(oldState != IPERF_STANDBY_STATE)
        {   // clear the async request
            iperf_async_request--;
        }
    }
    else if (oldState == IPERF_STANDBY_STATE)
    {   // going busy; set the async request
        iperf_async_request++;
        _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_ASYNC, 0); 
    }

    if(iperf_async_request == 0)
        _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_ASYNC); 
    
    pIState->state = (uint8_t)newState;

}


#endif  // defined(TCPIP_STACK_USE_IPERF)

