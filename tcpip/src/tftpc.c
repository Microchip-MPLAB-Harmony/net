/*******************************************************************************
  Trivial File Transfer Protocol (TFTP) Client

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    -Provides unreliable file upload and download services to other 
     applications which wish to connect to a remote UDP based TFTP 
     server.
    -Reference: RFC 1350
*******************************************************************************/

/*
Copyright (C) 2015-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_TFTP_CLIENT

#include "tcpip_private.h"

#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_TFTP_CLIENT)
#include "system/fs/sys_fs.h"
#include "tftpc_private.h"

#if defined(TCPIP_TFTPC_DEBUG)
#define TFTPC_DEBUG_PRINT   SYS_CONSOLE_PRINT  
#else
#define TFTPC_DEBUG_PRINT(fmt, ...)
#endif

static const void* volatile         tftpcMemH = NULL;           // memory handle
static TFTP_COMM_STATE              tftpGetCmdState = SM_TFTP_COMM_END;
static TFTP_COMM_STATE              tftpPutCmdState = SM_TFTP_COMM_END;
static TCPIP_SIGNAL_HANDLE          tftpcSignalHandle = NULL;
static int                          tftpcInitCount=0;
static TFTP_CLIENT_VARS             gTFTPClientDcpt;
static const TCPIP_NET_IF *         pTftpcIf = NULL;               // we use only one interface for tftp (for now at least)
static const TCPIP_NET_IF *         pTftpDefIf = NULL;             // default TFTPC interface
static uint32_t                     tftpcTimer;
// Tracker variable for the number of TFTP retries
static uint8_t                      tftpRetries;

static volatile uint16_t            tftpError;                 // Variable to preserve error condition causes for later transmission

// TFTP Static functions
#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void TCPIP_TFTPC_Cleanup(void);
#else
#define TCPIP_TFTPC_Cleanup()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static void F_TFTPSendFileName(TFTP_FILE_MODE mode, const uint8_t *fileName);
static bool TFTPOpenFile(const char *fileName, TFTP_FILE_MODE mode);
static TFTP_RESULT TFTPIsGetReady(uint8_t *getData, uint16_t *len);
static void F_TFTPSendAck(TCPIP_UINT16_VAL blockNumber);
static TFTP_RESULT TFTPIsPutReady(void);
#if defined(TCPIP_TFTPC_USER_NOTIFICATION) && (TCPIP_TFTPC_USER_NOTIFICATION != 0)
static void F_TFTPNotifyClients(const TCPIP_NET_IF* pNetIf, TCPIP_TFTPC_EVENT_TYPE evType, void* dBuff, uint32_t buffSize);
#else
#define F_TFTPNotifyClients(pNetIf, evType, buff, bufSize)
#endif  // (TCPIP_TFTPC_USER_NOTIFICATION != 0)

static void TCPIP_TFTPC_Process(void);

static void F_TFTPSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param);

// TFTP status flags

static union
{
    struct __attribute__((packed))
    {
        unsigned int bIsFlushed : 1; // The one part of the data is flushed if it is true.
        unsigned int bIsAcked : 1;   // the client will enable Ack for data receive acknowledge or data sent acknowledge
        unsigned int bIsClosed : 1;  // The file is going to closed after PUT is completed
        unsigned int bIsClosing : 1; // The file is closed
        unsigned int bIsReading : 1; // The file is in read mode.       
    } bits;
    uint8_t Val;
} tftpFlags;

static union
{
    struct
    {
        TCPIP_UINT16_VAL tftpBlockNumber;
        TCPIP_UINT16_VAL tftpDuplicateBlock;
        TCPIP_UINT16_VAL tftpBlockLength;
    } group2;
} MutExVar;     // Mutually Exclusive variable groups to conserve RAM.

#if defined(TCPIP_TFTPC_USER_NOTIFICATION) && (TCPIP_TFTPC_USER_NOTIFICATION != 0)
static PROTECTED_SINGLE_LIST      tftpcRegisteredUsers = { {0} };
#endif  // (TCPIP_TFTPC_USER_NOTIFICATION != 0)

// conversion functions/helpers
//
static __inline__ TFTP_STATE __attribute__((always_inline)) FC_TftpStateInc(TFTP_STATE tState, int inc)
{
    union
    {
        TFTP_STATE tState;
        int        iState;
    }U_T_STATE_INT;

    U_T_STATE_INT.tState = tState;
    U_T_STATE_INT.iState += inc;
    return U_T_STATE_INT.tState;
}

static __inline__ TCPIP_TFTPC_LIST_NODE* __attribute__((always_inline)) FC_SglNode2TftpNode(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        TCPIP_TFTPC_LIST_NODE* tftpcNode;
    }U_SGL_NODE_TFTPC_NODE;

    U_SGL_NODE_TFTPC_NODE.node = node;
    return U_SGL_NODE_TFTPC_NODE.tftpcNode;
}

static __inline__ TCPIP_TFTPC_HANDLE __attribute__((always_inline)) FC_SglNode2TftpHndl(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        TCPIP_TFTPC_HANDLE tftpHndl;
    }U_SGL_NODE_TFTPC_HNDL;

    U_SGL_NODE_TFTPC_HNDL.node = node;
    return U_SGL_NODE_TFTPC_HNDL.tftpHndl;
}

static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_TftpHndl2SglNode(TCPIP_TFTPC_HANDLE tftpHndl)
{
    union
    {
        TCPIP_TFTPC_HANDLE tftpHndl;
        SGL_LIST_NODE*  node;
    }U_TFTPC_HNDL_SGL_NODE;

    U_TFTPC_HNDL_SGL_NODE.tftpHndl = tftpHndl;
    return U_TFTPC_HNDL_SGL_NODE.node;
}



bool TCPIP_TFTPC_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackData, const void* initData)
{
    TFTP_CLIENT_VARS*   pClient;
    uint16_t            bufferSize;
    uint16_t            totalBufferSize;

    if(stackData->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }

    // stack start up
    if(tftpcInitCount == 0)
    {   // once per service

        if(initData == NULL)
        {
            return false;
        }
        const TCPIP_TFTPC_MODULE_CONFIG* pTftpcConfig = (const TCPIP_TFTPC_MODULE_CONFIG*)initData;
        pTftpDefIf = (const TCPIP_NET_IF*)TCPIP_STACK_NetHandleGet(pTftpcConfig->tftpc_interface);

        pClient = &gTFTPClientDcpt;
        
        pClient->hSocket = TCPIP_UDP_ClientOpen(IP_ADDRESS_TYPE_ANY, TCPIP_TFTP_SERVER_PORT, NULL);
        if(pClient->hSocket == INVALID_UDP_SOCKET)
        {
            return false;
        }
       
        totalBufferSize = TCPIP_TFTP_CLIENT_MAX_BUFFER_SIZE + (uint16_t)TCPIP_TFTPC_FILENAME_LEN + TCPIP_TFTP_CLIENT_OPCODE + TCPIP_TFTP_CLIENT_OCTET + 1U;
        bufferSize = TCPIP_UDP_TxPutIsReady(pClient->hSocket, totalBufferSize);
        if(bufferSize < totalBufferSize)
        {
            (void)TCPIP_UDP_OptionsSet(pClient->hSocket, UDP_OPTION_TX_BUFF, FC_Uint2VPtr((uint32_t)totalBufferSize));
        }

        (void)TCPIP_UDP_SignalHandlerRegister(pClient->hSocket, TCPIP_UDP_SIGNAL_RX_DATA, &F_TFTPSocketRxSignalHandler, NULL);

        // create the TFTPC timer
        tftpcSignalHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_TFTPC_Task, TCPIP_TFTPC_TASK_TICK_RATE);
        if(tftpcSignalHandle == NULL)
        {   // cannot create the SNTP timer
            TCPIP_TFTPC_Cleanup();
            return false;
        }
        gTFTPClientDcpt.hSocket = pClient->hSocket;
        gTFTPClientDcpt.netH =  NULL;
        gTFTPClientDcpt.smState = SM_TFTP_END;
        gTFTPClientDcpt.callbackPos = 0U;
        gTFTPClientDcpt.fileDescr = (int32_t) SYS_FS_HANDLE_INVALID;
        gTFTPClientDcpt.tftpServerAddr.v4Add.Val = 0;
        (void)memset(gTFTPClientDcpt.fileName,0,sizeof(gTFTPClientDcpt.fileName));

#if defined(TCPIP_TFTPC_USER_NOTIFICATION) && (TCPIP_TFTPC_USER_NOTIFICATION != 0)
        if(!TCPIP_Notification_Initialize(&tftpcRegisteredUsers))
        {   
            TCPIP_TFTPC_Cleanup();
            return false;
        }
#endif  // (TCPIP_TFTPC_USER_NOTIFICATION != 0)

        // store the memory allocation handle
        tftpcMemH = stackData->memH;
    }

    tftpcInitCount++;
    return true;
}

static  void F_TFTPCReleaseSocket(TFTP_CLIENT_VARS* pClient)
{
    if(pClient->hSocket != INVALID_UDP_SOCKET)
    {
        (void)TCPIP_UDP_Close(pClient->hSocket);
        pClient->hSocket = INVALID_UDP_SOCKET;
    }
}

static void TCPIP_TFTPC_Cleanup(void)
{
    TFTP_CLIENT_VARS    *pClient=NULL;
    pClient = &gTFTPClientDcpt;
    if(tftpcSignalHandle != NULL)
    {
        TCPIPStackSignalHandlerDeregister(tftpcSignalHandle);
        tftpcSignalHandle = NULL;
    }
    tftpcInitCount = 0;

    // close the socket -
    F_TFTPCReleaseSocket(pClient);
    
#if defined(TCPIP_TFTPC_USER_NOTIFICATION) && (TCPIP_TFTPC_USER_NOTIFICATION != 0)
    // remove notification registered user details.
    TCPIP_Notification_Deinitialize(&tftpcRegisteredUsers, tftpcMemH);
#endif  // (TCPIP_TFTPC_USER_NOTIFICATION != 0)
    
    tftpcMemH = NULL;    
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)

void TCPIP_TFTPC_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData)
{        
    if(stackData->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
    {   // stack shut down
        if(tftpcInitCount > 0)
        {   // we're up and running

            if(--tftpcInitCount == 0)
            {   // all closed
                // release resources
                TCPIP_TFTPC_Cleanup();
            }
        }
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

void TCPIP_TFTPC_SetServerAddress(IP_MULTI_ADDRESS* ipAddr,IP_ADDRESS_TYPE ipType)
{
    TFTP_CLIENT_VARS*   pClient;
    pClient = &gTFTPClientDcpt;
    
    if(ipType == IP_ADDRESS_TYPE_IPV4)
    {
        pClient->tftpServerAddr.v4Add.Val = ipAddr->v4Add.Val;
    }
    else if(ipType == IP_ADDRESS_TYPE_IPV6)
    {
        (void)memcpy(pClient->tftpServerAddr.v6Add.v,ipAddr->v6Add.v,sizeof(IPV6_ADDR));
    }
    else
    {
        // do nothing
    }
}

TCPIP_TFTPC_OPERATION_RESULT TCPIP_TFTPC_SetCommand(IP_MULTI_ADDRESS* mAddr,IP_ADDRESS_TYPE ipType,TCPIP_TFTP_CMD_TYPE cmdType,const char * fileName )
{
    TFTP_CLIENT_VARS*   pClient;
 
    pClient = &gTFTPClientDcpt;
    //TFTP client is in progress. Retry later
    if(pClient->smState != SM_TFTP_END)
    {
        return TFTPC_ERROR_BUSY;
    }
    if(strlen(fileName)> (sizeof(pClient->fileName)-1U))
    {
        return TFTPC_ERROR_INVALID_FILE_LENGTH;
    }
    if(mAddr != NULL)
    {
        TCPIP_TFTPC_SetServerAddress(mAddr,ipType);
    }
    
    if(cmdType == TFTP_CMD_PUT_TYPE)
    {
        pClient->modeType = TFTP_FILE_MODE_WRITE;
    }
    else
    {
        pClient->modeType = TFTP_FILE_MODE_READ;
    }
    (void)memset(pClient->fileName,0,sizeof(pClient->fileName));
    (void)strcpy(pClient->fileName,fileName);
    
    pClient->ipAddrType = ipType;
    pClient->smState = SM_TFTP_PROCESS_COMMAND;   

    return TFTPC_ERROR_NONE;   
}

static TFTP_RESULT TFTPIsPutReady(void)
{
    TCPIP_UINT16_VAL opCode;
    TCPIP_UINT16_VAL blockNumber;
    TFTP_CLIENT_VARS*   pClient;
    uint16_t replyPktSize = 0U;

    pClient = &gTFTPClientDcpt;
    // Check to see if timeout has occurred.
    bool bTimeOut = false;   
    TFTP_RESULT tRes = TFTP_NOT_READY; 
    bool redoSwitch;

    while(true)
    {
        redoSwitch = false; 
        switch(tftpPutCmdState)
        {
            case SM_TFTP_COMM_WAIT_FOR_ACK:
                replyPktSize = TCPIP_UDP_GetIsReady(pClient->hSocket);
                if(replyPktSize == 0U)
                {
                    uint32_t sysTickFreq = SYS_TMR_TickCounterFrequencyGet();
                    if (SYS_TMR_TickCountGet() - tftpcTimer >= ((uint32_t)TCPIP_TFTPC_CMD_PROCESS_TIMEOUT * sysTickFreq))
                    {
                        bTimeOut = true;
                        tftpcTimer = SYS_TMR_TickCountGet();
                    }
                    // When timeout occurs in this state, application must retry.
                    if(bTimeOut)
                    {
                        if (tftpRetries++ > ((uint8_t)TCPIP_TFTPC_MAX_RETRIES - 1U))
                        { // Forget about all previous attempts.
                            tftpRetries = 1U;
                            pClient->smState = SM_TFTP_END;
                            F_TFTPNotifyClients(pTftpcIf, TFTPC_EVENT_TIMEOUT, NULL, 0);
                            tRes = TFTP_TIMEOUT;
                            break;
                        }
                        else
                        {
                            pClient->smState = SM_TFTP_WAIT;
                            MutExVar.group2.tftpBlockNumber.Val--; // Roll back by one so proper block number ID is sent for the next packet
                            pClient->smState = SM_TFTP_FILE_OPEN_AND_SEND_REQUEST;
                            tRes = TFTP_RETRY;
                            break;
                        }
                    }
                    tRes = TFTP_PKT_NOT_RECEIVED;
                    break;
                }
                // ACK is received.
                F_TFTPNotifyClients(pTftpcIf, TFTPC_EVENT_ACKED, NULL, 0);

                tftpcTimer = SYS_TMR_TickCountGet();
                // Get opCode.
                (void)TCPIP_UDP_Get(pClient->hSocket,&opCode.v[1]);
                (void)TCPIP_UDP_Get(pClient->hSocket,&opCode.v[0]);

                // Get block number.
                (void)TCPIP_UDP_Get(pClient->hSocket,&blockNumber.v[1]);
                (void)TCPIP_UDP_Get(pClient->hSocket,&blockNumber.v[0]);

                // Discard everything else.
                (void)TCPIP_UDP_Discard(pClient->hSocket);

                // This must be ACK or else there is a problem.
                if (opCode.Val == (uint16_t)TFTP_OPCODE_ACK)
                {
                    // Also the block number must match with what we are expecting.
                    if (MutExVar.group2.tftpBlockNumber.Val == blockNumber.Val)
                    {
                        // Mark that block we sent previously has been ack'ed.
                        tftpFlags.bits.bIsAcked = 1U;

                        // Since we have ack, forget about previous retry count.
                        tftpRetries = 1;

                        // If this file is being closed, this must be last ack. Declare it as closed.
                        if (tftpFlags.bits.bIsClosing != 0U)
                        {
                            tftpFlags.bits.bIsClosed = 1U;                        
                            tRes = TFTP_END_OF_FILE;
                            break;
                        }
                        // Or else, wait for put to become ready so that caller
                        // can transfer more data blocks.
                        tftpPutCmdState = SM_TFTP_COMM_SEND_NEXT_DATA_PKT;
                        tftpcTimer = SYS_TMR_TickCountGet();
                        TFTPC_DEBUG_PRINT("Received Block Number : %d \r\n",blockNumber.Val);
                        redoSwitch = true; 
                    }
                    else
                    {
                        TFTPC_DEBUG_PRINT("TFTP: IsPutReady(): Unexpected block %d received - dropping it...\n",blockNumber.Val);
                        tRes = TFTP_NOT_READY;
                        break;
                    }
                }
                else if (opCode.Val == (uint16_t)TFTP_OPCODE_ERROR)
                {
                    // For error opCode, remember error code so that application
                    // can read it later.
                    tftpError = blockNumber.Val;

                    // Declare error.
                    tRes = TFTP_ERROR;
                    break;
                }
                else
                {
                    // do nothing
                }
                break;

            case SM_TFTP_COMM_SEND_NEXT_DATA_PKT:
                // Wait for UDP is to be ready to transmit.
                if(TCPIP_UDP_PutIsReady(pClient->hSocket) < (uint16_t)TCPIP_TFTP_CLIENT_MAX_BUFFER_SIZE)
                {
                    (void)TCPIP_UDP_OptionsSet(pClient->hSocket, UDP_OPTION_TX_BUFF, FC_Uint162VPtr(TCPIP_TFTP_CLIENT_MAX_BUFFER_SIZE));
                    break;
                }

                // Put next block of data.
                MutExVar.group2.tftpBlockNumber.Val++;
                (void)TCPIP_UDP_Put(pClient->hSocket, 0U);
                (void)TCPIP_UDP_Put(pClient->hSocket, (uint8_t)TFTP_OPCODE_DATA);

                (void)TCPIP_UDP_Put(pClient->hSocket, MutExVar.group2.tftpBlockNumber.v[1]);
                (void)TCPIP_UDP_Put(pClient->hSocket, MutExVar.group2.tftpBlockNumber.v[0]);

                // Remember that this block is not yet flushed.
                tftpFlags.bits.bIsFlushed = 0U;

                // Remember that this block is not acknowledged.
                tftpFlags.bits.bIsAcked = 0U;

                tftpPutCmdState = SM_TFTP_COMM_WAIT_FOR_ACK;
                TFTPC_DEBUG_PRINT("Transmitted Block Number : %d \r\n",MutExVar.group2.tftpBlockNumber.Val);
                tRes = TFTP_OK;
                break;

            default:
                // do nothing
                break;
        }

        if(redoSwitch == false)
        {   // done
           break;
        } 
    }

    return tRes;
}

void TCPIP_TFTPC_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != TCPIP_MODULE_SIGNAL_NONE)
    { // regular TMO or RX signal occurred
        TCPIP_TFTPC_Process();
    }

}

// send a signal to the TFTP module that data is available
// no manager alert needed since this normally results as a higher layer (UDP) signal
static void F_TFTPSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_UDP_SIGNAL_RX_DATA)
    {
        (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}


static void TCPIP_TFTPC_Process(void)
{
    TCPIP_NET_IF* pNetIf=NULL;
    TFTP_CLIENT_VARS*   pClient;
    uint16_t wCount = 0;
    size_t wLen;
    int32_t fsStat;
    uint8_t data[700];
    bool res=true;
    bool bTimeout=false;
    uint16_t  replyPktSize = 0U;
    bool bindRes = false;
    
    pClient = &gTFTPClientDcpt;
    pNetIf = TCPIPStackAnyNetLinked(false);
    if(pNetIf == NULL || TCPIPStackIsConfig(pNetIf))
    {   // not yet up and running
        return;
    }

    switch(pClient->smState)
    {
        case SM_TFTP_PROCESS_COMMAND:
            // select a running interface
            pTftpcIf = pTftpDefIf;
            if(!TCPIP_STACK_NetworkIsLinked(pTftpcIf))
            {
                pTftpcIf = TCPIPStackAnyNetLinked(true);
            }
            if(pTftpcIf == NULL)
            {   // wait some more
                break;
            }
            
            bindRes = TCPIP_UDP_RemoteBind(pClient->hSocket, pClient->ipAddrType,TCPIP_TFTP_SERVER_PORT,&pClient->tftpServerAddr);
            if (bindRes)
            {
                 // receiving from multiple TFTP servers
                (void)TCPIP_UDP_OptionsSet(pClient->hSocket, UDP_OPTION_STRICT_PORT, FC_Uint2VPtr(0UL));
                //TCPIP_UDP_DestinationIPAddressSet(pClient->hSocket, IP_ADDRESS_TYPE_IPV4, &pClient->tftpServerAddr);
                (void)TCPIP_UDP_SocketNetSet(pClient->hSocket, pTftpcIf);
            }
            else
            {
                break;
            }
            pClient->smState = FC_TftpStateInc(pClient->smState, 1);
            tftpcTimer = SYS_TMR_TickCountGet();
            // connection established
            F_TFTPNotifyClients(pTftpcIf, TFTPC_EVENT_CONN_ESTABLISHED, NULL, 0);
            break;

        case SM_TFTP_UDP_IS_OPENED:
            if(TCPIP_UDP_TxPutIsReady(pClient->hSocket, ((uint16_t)TCPIP_TFTPC_FILENAME_LEN + TCPIP_TFTP_CLIENT_OPCODE + TCPIP_TFTP_CLIENT_OCTET + 1U)) == 0U)
            {
                uint32_t sysTickFreq = SYS_TMR_TickCounterFrequencyGet();
                if((SYS_TMR_TickCountGet() - tftpcTimer > (uint32_t)TCPIP_TFTPC_ARP_TIMEOUT * sysTickFreq))
                {
                    bTimeout = true;
                }
                if(bTimeout)
                {
                    if ( tftpRetries++ > ((uint8_t)TCPIP_TFTPC_MAX_RETRIES - 1U) )
                    {
                        // Forget about all previous attempts.
                        tftpRetries = 1U;
                        pClient->smState = SM_TFTP_END;
                        // connection Timed out
                        F_TFTPNotifyClients(pTftpcIf, TFTPC_EVENT_TIMEOUT, NULL, 0);
                    }
                }
                break;
            }
            tftpRetries = 1U;
            pClient->smState = FC_TftpStateInc(pClient->smState, 1);
            break;

        case SM_TFTP_FILE_OPEN_AND_SEND_REQUEST:
            res = TFTPOpenFile(pClient->fileName, pClient->modeType);
            if(res==false)
            {
                pClient->smState = SM_TFTP_END;
            }
            else
            {
                if(pClient->modeType == TFTP_FILE_MODE_WRITE)
                {
                    pClient->smState = SM_TFTP_PUT_COMMAND;
                    tftpPutCmdState = SM_TFTP_COMM_WAIT_FOR_ACK;
                }
                else
                {
                    pClient->smState = SM_TFTP_GET_COMMAND;
                    tftpGetCmdState = SM_TFTP_COMM_WAIT_FOR_DATA;
                }
            }
            // Now TFTP client is busy.
            F_TFTPNotifyClients(pTftpcIf, TFTPC_EVENT_BUSY, NULL, 0);
            break;

        case SM_TFTP_PUT_COMMAND:        
            switch(TFTPIsPutReady())
            {
                case TFTP_OK:
                    if(pClient->callbackPos != 0U)
                    {   // The file was already opened, so load up its ID and seek
                        if(pClient->fileDescr == (int32_t)SYS_FS_HANDLE_INVALID)
                        {// No file handles available, so wait for now
                            pClient->smState = SM_TFTP_END;
                            break;
                        }                        
                        (void)SYS_FS_FileSeek((SYS_FS_HANDLE)pClient->fileDescr,(int32_t)pClient->callbackPos, SYS_FS_SEEK_SET);
                    }
                                      
                    // Put as many bytes as possible
                    wCount = TCPIP_UDP_PutIsReady(pClient->hSocket);
                    if(wCount >= (uint16_t)TCPIP_TFTP_BLOCK_SIZE_SUPPORTED)
                    {
                        wLen = SYS_FS_FileRead((SYS_FS_HANDLE)pClient->fileDescr, data, (size_t)TCPIP_TFTP_BLOCK_SIZE_SUPPORTED);
                        if(wLen < TCPIP_TFTP_BLOCK_SIZE_SUPPORTED)
                        {   // If no bytes were read, an EOF was reached
                            if(wLen != 0U)
                            {
                                (void)TCPIP_UDP_ArrayPut(pClient->hSocket, data, (uint16_t)wLen);
                                // flush last chunk of less than 512 bytes
                                (void)TCPIP_UDP_Flush(pClient->hSocket);
                            }
                            (void)SYS_FS_FileClose((SYS_FS_HANDLE)pClient->fileDescr);
                            SYS_CONSOLE_PRINT("\r\nNumber of bytes transmitted : %d bytes \r\n", pClient->callbackPos);
                            pClient->fileDescr = -1;
                            pClient->callbackPos = 0U;
                            pClient->smState = SM_TFTP_END;
                            (void)TCPIP_UDP_Disconnect(pClient->hSocket,false);
                            // TFTP Communication completed , that is File is uploaded properly.
                            F_TFTPNotifyClients(pTftpcIf, TFTPC_EVENT_COMPLETED, NULL, 0);
                            tftpFlags.bits.bIsClosing =  1U;
                            break;
                        }
                        else
                        {   // Write the bytes to the socket
                            (void)TCPIP_UDP_ArrayPut(pClient->hSocket, data, (uint16_t)wLen);
                            // flush these 512 bytes
                            (void)TCPIP_UDP_Flush(pClient->hSocket);

                            tftpFlags.bits.bIsFlushed = 1U;
                            // Save the new address and close the file
                            fsStat = SYS_FS_FileTell((SYS_FS_HANDLE)pClient->fileDescr);
                            if(fsStat == -1)
                            {
                                pClient->callbackPos = 0U;
                            }
                            else
                            {
                                pClient->callbackPos = (uint32_t)fsStat;
                            }
                            SYS_CONSOLE_MESSAGE("#");
                        }
                    }
                    pClient->smState = SM_TFTP_PUT_COMMAND;
                    break;

                case TFTP_PKT_NOT_RECEIVED:
                    // do nothing
                    break;

                case TFTP_RETRY:
                    pClient->smState = SM_TFTP_FILE_OPEN_AND_SEND_REQUEST;
                    break;

                case TFTP_NOT_READY:
                    if(pClient->fileDescr != -1)
                    {
                        (void)SYS_FS_FileClose((SYS_FS_HANDLE)pClient->fileDescr);
                        pClient->fileDescr = -1;
                    }
                    pClient->callbackPos = 0U;
                    pClient->smState = SM_TFTP_END;
                    // Declined due to Bad PDU
                    F_TFTPNotifyClients(pTftpcIf, TFTPC_EVENT_DECLINE, NULL, 0);
                    break;

                case  TFTP_END_OF_FILE:
                    (void)SYS_FS_FileClose((SYS_FS_HANDLE)pClient->fileDescr);
                    pClient->fileDescr = -1;
                    pClient->callbackPos = 0U;
                    pClient->smState = SM_TFTP_END;
                    (void)TCPIP_UDP_Disconnect(pClient->hSocket,false);
                    // TFTP Communication completed , that is File is uploaded properly.
                    F_TFTPNotifyClients(pTftpcIf, TFTPC_EVENT_COMPLETED, NULL, 0);
                    break;

                default:
                    // do nothing
                    break;
            }        
            break;

        case SM_TFTP_GET_COMMAND:
            switch(TFTPIsGetReady(data, &wCount))
            {
                case TFTP_OK:
                    // Check if their is any byte need to be written to the FS or Buffer-
                    if(wCount == 0U)
                    {
                        break;
                    }
                    if(pClient->callbackPos != 0U)
                    {   // The file was already opened, so load up its ID and seek
                        if(pClient->fileDescr == (int32_t)SYS_FS_HANDLE_INVALID)
                        {   // No file handles available, so wait for now
                            pClient->smState = SM_TFTP_END;
                            break;
                        }                   
                        (void)SYS_FS_FileSeek((SYS_FS_HANDLE)pClient->fileDescr, (int32_t)pClient->callbackPos, SYS_FS_SEEK_SET);                     
                    }              
                    // first 512 bytes
                    wLen = SYS_FS_FileWrite((SYS_FS_HANDLE)pClient->fileDescr, data, (size_t)wCount);
                    if((ssize_t)wLen == -1)
                    {   // If no bytes were read, an EOF was reached
                        (void)SYS_FS_FileClose((SYS_FS_HANDLE)pClient->fileDescr);
                        pClient->fileDescr = -1;
                        pClient->callbackPos = 0U;
                        pClient->smState = SM_TFTP_END;
                        (void)TCPIP_UDP_Disconnect(pClient->hSocket, false);
                        // TFTP Communication completed , that is File is uploaded properly.
                        F_TFTPNotifyClients(pTftpcIf, TFTPC_EVENT_COMPLETED, NULL, 0);
                    }                   
                    else
                    {   
                        // Save the new address
                        fsStat = SYS_FS_FileTell((SYS_FS_HANDLE)pClient->fileDescr);
                        if(fsStat == -1)
                        {
                            pClient->callbackPos = 0U;
                        }
                        else
                        {
                            pClient->callbackPos = (uint32_t)fsStat;                        
                        }
                        // Data receive started.
                        F_TFTPNotifyClients(pTftpcIf, TFTP_EVENT_DATA_RECEIVED, FC_Uint2VPtr(pClient->callbackPos - (uint32_t)wCount), wCount);
                    }
                    break;

                case TFTP_ACK_SEND:
                     pClient->smState = SM_TFTP_GET_COMMAND;
                     break;

                case TFTP_RETRY:
                     pClient->smState = SM_TFTP_FILE_OPEN_AND_SEND_REQUEST;
                    break;

                case TFTP_NOT_READY:                
                    if(pClient->fileDescr != -1)
                    {
                        (void)SYS_FS_FileClose((SYS_FS_HANDLE)pClient->fileDescr);
                        pClient->fileDescr = -1;
                    }                
                    pClient->callbackPos = 0;
                    pClient->smState = SM_TFTP_END;
                    // Declined due to Bad PDU
                    F_TFTPNotifyClients(pTftpcIf, TFTPC_EVENT_DECLINE, NULL, 0);
                    break;

                case  TFTP_END_OF_FILE:                   
                    (void)SYS_FS_FileClose((SYS_FS_HANDLE)pClient->fileDescr);
                    pClient->fileDescr = -1;
                    pClient->callbackPos = 0;
                    pClient->smState = SM_TFTP_END;
                    (void)TCPIP_UDP_Disconnect(pClient->hSocket,false);
                    // TFTP Communication completed , that is File is uploaded properly.
                    F_TFTPNotifyClients(pTftpcIf, TFTPC_EVENT_COMPLETED, NULL, 0);
                    break;

                default:
                    // do nothing
                    break;
            }
            break;

        case SM_TFTP_END:
            // remove all other UDP packets from the socket buffer
            // This also removes the last ack from the Socket buffer
            while(true)
            {
                replyPktSize = TCPIP_UDP_GetIsReady(pClient->hSocket);
                if(replyPktSize == 0U)
                {
                    break;
                }
                else
                {
                    (void)TCPIP_UDP_Discard(pClient->hSocket);
                }
            }
            break;

        default:
            // do nothing
            break;
    }
}

static void F_TFTPSendFileName(TFTP_FILE_MODE mode, const uint8_t *fileName)
{
    uint8_t c;
    TFTP_CLIENT_VARS*   pClient;
    pClient = &gTFTPClientDcpt; 

    
    // Write opCode
    (void)TCPIP_UDP_Put(pClient->hSocket, 0U);
    (void)TCPIP_UDP_Put(pClient->hSocket, (uint8_t)mode);

    // write file name, including NULL.
    do
    {
        c = *fileName++;
        (void)TCPIP_UDP_Put(pClient->hSocket,c);
    } while ( c != 0U );

    // Write mode - always use octet or binay mode to transmit files.
    (void)TCPIP_UDP_Put(pClient->hSocket, (uint8_t)'o');
    (void)TCPIP_UDP_Put(pClient->hSocket, (uint8_t)'c');
    (void)TCPIP_UDP_Put(pClient->hSocket, (uint8_t)'t');
    (void)TCPIP_UDP_Put(pClient->hSocket, (uint8_t)'e');
    (void)TCPIP_UDP_Put(pClient->hSocket, (uint8_t)'t');
    (void)TCPIP_UDP_Put(pClient->hSocket, 0U);

    // Transmit it.
    (void)TCPIP_UDP_Flush(pClient->hSocket);
    
}

static bool TFTPOpenFile(const char *fileName, TFTP_FILE_MODE mode)
{
    SYS_FS_HANDLE fp;
    TFTP_CLIENT_VARS*   pClient;
    
    pClient = &gTFTPClientDcpt; 
    
    if(mode == TFTP_FILE_MODE_WRITE)
    {
        fp = SYS_FS_FileOpen(fileName, SYS_FS_FILE_OPEN_READ);
    }
    else
    {
        fp = SYS_FS_FileOpen(fileName, SYS_FS_FILE_OPEN_WRITE);
    }

    if(fp == SYS_FS_HANDLE_INVALID)
    {// File not found, so abort
        return false;
    }
    pClient->fileDescr = (int32_t)fp;
 
    pClient->callbackPos = 0U;
    tftpFlags.bits.bIsClosing = 0U;
    tftpFlags.bits.bIsClosed = 0U;

    // Tell remote server about our intention.
    F_TFTPSendFileName(mode, (const uint8_t *)fileName);

    // Clear all flags.
    tftpFlags.Val = 0;

    // Remember start tick for this operation.
    tftpcTimer = SYS_TMR_TickCountGet();

    // Depending on mode of operation, remote server will respond with
    // specific block number.
    if ( mode == TFTP_FILE_MODE_READ )
    {
        // Remember that we are reading a file.
        tftpFlags.bits.bIsReading = 1U;

        // For read operation, server would respond with data block of 1.
        MutExVar.group2.tftpBlockNumber.Val = 1;

        // Next packet would be the data packet.
        tftpGetCmdState = SM_TFTP_COMM_WAIT_FOR_DATA;
        // Request is sent to the Server  either to Get a file
        F_TFTPNotifyClients(pTftpcIf, TFTPC_EVENT_GET_REQUEST, NULL, 0);
    }

    else
    {
        // Remember that we are writing a file.
        tftpFlags.bits.bIsReading = 0U;

        // For write operation, server would respond with data block of 0.
        MutExVar.group2.tftpBlockNumber.Val = 0;

        // Next packet would be the ACK packet.
        tftpPutCmdState = SM_TFTP_COMM_WAIT_FOR_ACK;
        
        // Request is sent to the Server  either to Put a file
        F_TFTPNotifyClients(pTftpcIf, TFTPC_EVENT_PUT_REQUEST, NULL, 0);
    }
    
    return true;
}


static TFTP_RESULT TFTPIsGetReady(uint8_t *getData, uint16_t* len)
{
    TCPIP_UINT16_VAL opCode;
    TCPIP_UINT16_VAL blockNumber;
    TFTP_CLIENT_VARS*   pClient;
    uint16_t replyPktSize = 0U;

    pClient = &gTFTPClientDcpt;
    // Check to see if timeout has occurred.
    bool bTimeOut = false;
    uint32_t sysTickFreq = SYS_TMR_TickCounterFrequencyGet();
    if ( SYS_TMR_TickCountGet() - tftpcTimer >= ((uint32_t)TCPIP_TFTPC_CMD_PROCESS_TIMEOUT * sysTickFreq) )
    {
        bTimeOut = true;
        tftpcTimer = SYS_TMR_TickCountGet();
    }

    TFTP_RESULT tRes = TFTP_NOT_READY; 
    switch(tftpGetCmdState)
    {
        case SM_TFTP_COMM_WAIT_FOR_DATA:
            // If timeout occurs in this state, it may be because, we have not
            // even received very first data block or some in between block.
            if ( bTimeOut == true )
            {
                if ( tftpRetries++ > ((uint8_t)TCPIP_TFTPC_MAX_RETRIES - 1U) )
                {   // Forget about all previous attempts.
                    tftpRetries = 1U;
                    tRes = TFTP_TIMEOUT;
                    break;
                }

                // If we have not even received first block, ask application
                // retry.
                if ( MutExVar.group2.tftpBlockNumber.Val == 1u )
                {
                    tRes = TFTP_RETRY;
                    break;
                }
                else
                {
                    // Block number was already incremented in last ACK attempt,
                    // so decrement it.
                    MutExVar.group2.tftpBlockNumber.Val--;

                    // Do it.
                    tftpGetCmdState = SM_TFTP_COMM_SEND_ACK;
                    break;
                }
            }
            tftpcTimer = SYS_TMR_TickCountGet();
            replyPktSize = TCPIP_UDP_GetIsReady(pClient->hSocket);
            // For Read operation, server will respond with data block.
            if (replyPktSize == 0U)
            {
                break;
            }

            // Get opCode
            (void)TCPIP_UDP_Get(pClient->hSocket,&opCode.v[1]);
            (void)TCPIP_UDP_Get(pClient->hSocket,&opCode.v[0]);

            // Get block number.
            (void)TCPIP_UDP_Get(pClient->hSocket,&blockNumber.v[1]);
            (void)TCPIP_UDP_Get(pClient->hSocket,&blockNumber.v[0]);

            // get the data array from the socket  
            *len = TCPIP_UDP_ArrayGet(pClient->hSocket, getData, replyPktSize - 4U);

            // In order to read file, this must be data with block number of 0.
            if ( opCode.Val == (uint16_t)TFTP_OPCODE_DATA )
            {
                // Make sure that this is not a duplicate block.
                if ( MutExVar.group2.tftpBlockNumber.Val == blockNumber.Val )
                {
                    // Mark that we have not acked this block.
                    tftpFlags.bits.bIsAcked = 0U;

                    // Since we have a packet, forget about previous retry count.
                    tftpRetries = 1;

                    tftpGetCmdState = SM_TFTP_COMM_SEND_ACK;
                    if(replyPktSize < (TCPIP_TFTP_CLIENT_MAX_BUFFER_SIZE - 1U))
                    {
                        tftpFlags.bits.bIsClosing =  1U;
                        tftpGetCmdState = SM_TFTP_COMM_SEND_LAST_ACK;
                    }
                    tRes = TFTP_OK;
                    break;
                }
                // If received block has already been received, simply ack it
                // so that Server can "get over" it and send next block.
                else if ( MutExVar.group2.tftpBlockNumber.Val > blockNumber.Val )
                {
                    TFTPC_DEBUG_PRINT("TFTP: IsGetReady(): Duplicate block %d received - dropping it...\n",blockNumber.Val);
                    MutExVar.group2.tftpDuplicateBlock.Val = blockNumber.Val;
                    tftpGetCmdState = SM_TFTP_COMM_DUPLICATE_ACK;
                }
                else
                {
                    // do nothing
                }
            }
            // Discard all unexpected and error blocks.
            (void)TCPIP_UDP_Discard(pClient->hSocket);

            // If this was an error, remember error code for later delivery.
            if ( opCode.Val == (uint16_t)TFTP_OPCODE_ERROR )
            {
                tftpError = blockNumber.Val;
                tRes = TFTP_ERROR;
            }
            break;

        case SM_TFTP_COMM_DUPLICATE_ACK:
            if ( TCPIP_UDP_PutIsReady(pClient->hSocket) != 0U)
            {
                F_TFTPSendAck(MutExVar.group2.tftpDuplicateBlock);
                tftpGetCmdState = SM_TFTP_COMM_WAIT_FOR_DATA;
            }
            break;

        case SM_TFTP_COMM_SEND_LAST_ACK:
        case SM_TFTP_COMM_SEND_ACK:
            if(tftpGetCmdState == SM_TFTP_COMM_SEND_LAST_ACK)
            { 
                if(tftpFlags.bits.bIsClosing != 0U)
                {
                    tftpFlags.bits.bIsClosed = 1U;
                }
                SYS_CONSOLE_PRINT("\r\nTFTP: Total Bytes received: %d bytes \r\n ",pClient->callbackPos);
            }

            if ( TCPIP_UDP_PutIsReady(pClient->hSocket) != 0U )
            {
                F_TFTPSendAck(MutExVar.group2.tftpBlockNumber);

                // This is the next block we are expecting.
                MutExVar.group2.tftpBlockNumber.Val++;

                // Remember that we have already acked current block.
                tftpFlags.bits.bIsAcked = 1U;

                if ( tftpGetCmdState == SM_TFTP_COMM_SEND_LAST_ACK )
                {
                    tRes = TFTP_END_OF_FILE;
                }
                else
                {
                    tftpGetCmdState = SM_TFTP_COMM_WAIT_FOR_DATA;
                    SYS_CONSOLE_MESSAGE("#");
                    tRes = TFTP_ACK_SEND;
                }
            }
            break;

        default:
            // SM_TFTP_WAIT, SM_TFTP_WAIT_FOR_ACK states.
            break;
    }
    return tRes;
}

static void F_TFTPSendAck(TCPIP_UINT16_VAL blockNumber)
{
    TFTP_CLIENT_VARS*   pClient;
    pClient = &gTFTPClientDcpt;
    // Write opCode.
    (void)TCPIP_UDP_Put(pClient->hSocket, 0U);
    (void)TCPIP_UDP_Put(pClient->hSocket, (uint8_t)TFTP_OPCODE_ACK);

    // Write block number for this ack.
    (void)TCPIP_UDP_Put(pClient->hSocket, blockNumber.v[1]);
    (void)TCPIP_UDP_Put(pClient->hSocket, blockNumber.v[0]);

    // Transmit it.
    (void)TCPIP_UDP_Flush(pClient->hSocket);
}

// Register an TFTP event handler
// Use hNet == 0 to register on all interfaces available
// Returns a valid handle if the call succeeds,
// or a null handle if the call failed.
// Function has to be called after the TFTP is initialized
// The hParam is passed by the client and will be used by the TFTP when the notification is made.
// It is used for per-thread content or if more modules, for example, share the same handler
// and need a way to differentiate the callback.
#if defined(TCPIP_TFTPC_USER_NOTIFICATION) && (TCPIP_TFTPC_USER_NOTIFICATION != 0)
TCPIP_TFTPC_HANDLE TCPIP_TFTPC_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_TFTPC_EVENT_HANDLER handler, const void* hParam)
{
    if(handler != NULL)
    {
        if(tftpcMemH != NULL)
        {
            TCPIP_TFTPC_LIST_NODE tftpNode;
            tftpNode.handler = handler;
            tftpNode.hParam = hParam;
            tftpNode.hNet = hNet;
            tftpNode.next = NULL;

            return FC_SglNode2TftpHndl(TCPIP_Notification_Add(&tftpcRegisteredUsers, tftpcMemH, &tftpNode, sizeof(tftpNode)));
        }
    }

    return NULL;
}


// deregister the event handler
bool TCPIP_TFTPC_HandlerDeRegister(TCPIP_TFTPC_HANDLE hTftpc)
{
    if(hTftpc != NULL)
    {
        if(tftpcMemH != NULL)
        {
            if(TCPIP_Notification_Remove(FC_TftpHndl2SglNode(hTftpc), &tftpcRegisteredUsers, tftpcMemH))
            {
                return true;
            }
        }
    }
    return false;
}

static void F_TFTPNotifyClients(const TCPIP_NET_IF* pNetIf, TCPIP_TFTPC_EVENT_TYPE evType, void* dBuff, uint32_t buffSize)
{
    TCPIP_TFTPC_LIST_NODE* dNode;

    TCPIP_Notification_Lock(&tftpcRegisteredUsers);
    for(dNode = FC_SglNode2TftpNode(tftpcRegisteredUsers.list.head); dNode != NULL; dNode = dNode->next)
    {
        if(dNode->hNet == NULL || dNode->hNet == pNetIf)
        {   // trigger event
            (*dNode->handler)(pNetIf, evType, dBuff, buffSize, dNode->hParam);
        }
    }
    TCPIP_Notification_Unlock(&tftpcRegisteredUsers);
}
#else
TCPIP_TFTPC_HANDLE TCPIP_TFTPC_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_TFTPC_EVENT_HANDLER handler, const void* hParam)
{
    return NULL;
}
bool TCPIP_TFTPC_HandlerDeRegister(TCPIP_TFTPC_HANDLE hTftpc)
{
    return false;
}
#endif  // (TCPIP_TFTPC_USER_NOTIFICATION != 0)

TCPIP_TFTPC_EVENT_TYPE TCPIP_TFTPC_GetEventNotification(void)
{
    uint16_t eventType = (uint16_t)TFTPC_EVENT_NONE;
    TFTP_CLIENT_VARS*   pClient;
    pClient = &gTFTPClientDcpt;
    
    if(pClient->smState != SM_TFTP_END)
    {
        // The TFTP Client is busy with processing the file communication.
        eventType |= (uint16_t)TFTPC_EVENT_BUSY;
    }
    
    if(tftpFlags.bits.bIsReading == 1U)
    {
        eventType |= (uint16_t)TFTPC_EVENT_GET_REQUEST;
        if(tftpFlags.bits.bIsAcked != 0U)
        {
            // ACK is sent for the data received during the READ mode
            eventType |= (uint16_t)TFTPC_EVENT_ACKED;
        }
        if(tftpFlags.bits.bIsClosed != 0U)
        {
            // Last ACK is sent for the last block it is received.
            // Socket is not closed. only the file descriptor is closed
            eventType |= (uint16_t)TFTPC_EVENT_COMPLETED;
        }        
    }
    else
    {
        eventType |= (uint16_t)TFTPC_EVENT_PUT_REQUEST;
        if(tftpFlags.bits.bIsAcked != 0U)
        {
            // ACK is received for the data transmitted during the WRITE mode
            eventType |= (uint16_t)TFTPC_EVENT_ACKED;
        }
        if(tftpFlags.bits.bIsClosed != 0U)
        {
            // The last ACK is received for the data transmitted during the WRITE mode.
            // Socket is not closed. only the file descriptor is closed
            eventType |= (uint16_t)TFTPC_EVENT_COMPLETED;
        }        
    }
    return (TCPIP_TFTPC_EVENT_TYPE)eventType;
}
#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_TFTP_CLIENT)

