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

/*****************************************************************************
 Copyright (C) 2015-2018 Microchip Technology Inc. and its subsidiaries.

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

#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_TFTP_CLIENT

#include "tcpip_private.h"

#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_TFTP_CLIENT)
#include "system/fs/sys_fs.h"
#include "tftpc_private.h"

#if defined(TCPIP_TFTPC_DEBUG)
#define TFTPC_DEBUG_PRINT(fmt, ...) do { SYS_CONSOLE_PRINT(fmt, ##__VA_ARGS__); } while (0)
#define TFTPC_DEBUG_MESSAGE(message)   do { SYS_CONSOLE_MESSAGE(message); } while (0)
#else
#define TFTPC_DEBUG_PRINT(fmt, ...)
#define TFTPC_DEBUG_MESSAGE(message)
#endif

#define mMIN(a, b)  ((a<b)?a:b)

static const void* volatile         tftpcMemH = NULL;           // memory handle
static TFTP_COMM_STATE              _tftpGetCmdState = SM_TFTP_COMM_END;
static TFTP_COMM_STATE              _tftpPutCmdState = SM_TFTP_COMM_END;
static tcpipSignalHandle            tftpcSignalHandle = 0;
static uint8_t                      tftpcInitCount=0;
static TFTP_CLIENT_VARS             gTFTPClientDcpt;
static TCPIP_NET_IF *               pTftpcIf = 0;               // we use only one interface for tftp (for now at least)
static TCPIP_NET_IF *               pTftpDefIf = 0;             // default TFTPC interface
static uint32_t                     tftpcTimer;
// Tracker variable for the number of TFTP retries
static uint8_t                      _tftpRetries;

static volatile uint16_t            _tftpError;                 // Variable to preserve error condition causes for later transmission

// TFTP Static functions
#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void TCPIP_TFTPC_Cleanup(void);
#else
#define TCPIP_TFTPC_Cleanup()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static void _TFTPSendFileName(TFTP_OPCODE opcode, uint8_t *fileName);
static bool TFTPOpenFile(const char *fileName, TFTP_FILE_MODE mode);
static TFTP_RESULT TFTPIsGetReady(uint8_t *getData, int32_t *len);
static void _TFTPSendAck(TCPIP_UINT16_VAL blockNumber);
static TFTP_RESULT TFTPIsPutReady(void);
#if (TCPIP_TFTPC_USER_NOTIFICATION != 0)
static void _TFTPNotifyClients(TCPIP_NET_IF* pNetIf, TCPIP_TFTPC_EVENT_TYPE evType,void* buff,uint32_t bufSize);
#else
#define _TFTPNotifyClients(pNetIf, evType, buff, bufSize)
#endif  // (TCPIP_TFTPC_USER_NOTIFICATION != 0)

static void TCPIP_TFTPC_Process(void);

static void _TFTPSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param);

// TFTP status flags
static union
{
    struct
    {
        unsigned int bIsFlushed : 1; // The one part of the data is flushed if it is true.
        unsigned int bIsAcked : 1;   // the client will enable Ack for data receive acknowledge or data sent acknowledge
        unsigned int bIsClosed : 1;  // The file is going to closed after PUT is completed
        unsigned int bIsClosing : 1; // The file is closed
        unsigned int bIsReading : 1; // The file is in read mode.       
    } bits;
    uint8_t Val;
} _tftpFlags;

static union
{
    struct
    {
        TCPIP_UINT16_VAL _tftpBlockNumber;
        TCPIP_UINT16_VAL _tftpDuplicateBlock;
        TCPIP_UINT16_VAL _tftpBlockLength;
    } group2;
} MutExVar;     // Mutually Exclusive variable groups to conserve RAM.

#if (TCPIP_TFTPC_USER_NOTIFICATION != 0)
static PROTECTED_SINGLE_LIST      tftpcRegisteredUsers = { {0} };
#endif  // (TCPIP_TFTPC_USER_NOTIFICATION != 0)


bool TCPIP_TFTPC_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackData,
                       const TCPIP_TFTPC_MODULE_CONFIG* pTftpcConfig)
{
    TFTP_CLIENT_VARS*   pClient;
    uint16_t            bufferSize;
    uint16_t            totalBufferSize;

    if(stackData->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }

    // stack start up
    if(tftpcInitCount == 0)
    {   // once per service

        if(pTftpcConfig == 0)
        {
            return false;
        }
        pTftpDefIf = (TCPIP_NET_IF*)TCPIP_STACK_NetHandleGet(pTftpcConfig->tftpc_interface);

        pClient = &gTFTPClientDcpt;
        
        pClient->hSocket = TCPIP_UDP_ClientOpen(IP_ADDRESS_TYPE_ANY, TCPIP_TFTP_SERVER_PORT, 0);
        if(pClient->hSocket == INVALID_UDP_SOCKET)
        {
            return false;
        }
       
        totalBufferSize = TCPIP_TFTP_CLIENT_MAX_BUFFER_SIZE+TCPIP_TFTPC_FILENAME_LEN+TCPIP_TFTP_CLIENT_OPCODE+TCPIP_TFTP_CLIENT_OCTET+1;
        bufferSize = TCPIP_UDP_TxPutIsReady(pClient->hSocket, totalBufferSize); //
        if(bufferSize < totalBufferSize)
        {
            TCPIP_UDP_OptionsSet(pClient->hSocket, UDP_OPTION_TX_BUFF, (void*)(unsigned int)totalBufferSize);
        }

        TCPIP_UDP_SignalHandlerRegister(pClient->hSocket, TCPIP_UDP_SIGNAL_RX_DATA, _TFTPSocketRxSignalHandler, 0);

        // create the TFTPC timer
        tftpcSignalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_TFTPC_Task, TCPIP_TFTPC_TASK_TICK_RATE);
        if(tftpcSignalHandle == 0)
        {   // cannot create the SNTP timer
            TCPIP_TFTPC_Cleanup();
            return false;
        }
        gTFTPClientDcpt.hSocket = pClient->hSocket;
        gTFTPClientDcpt.netH =  NULL;
        gTFTPClientDcpt.smState = SM_TFTP_END;
        gTFTPClientDcpt.callbackPos = 0;
        gTFTPClientDcpt.fileDescr = (int32_t) SYS_FS_HANDLE_INVALID;
        gTFTPClientDcpt.tftpServerAddr.v4Add.Val = 0;
        memset(gTFTPClientDcpt.fileName,0,sizeof(gTFTPClientDcpt.fileName));

#if (TCPIP_TFTPC_USER_NOTIFICATION != 0)
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

static  void _TFTPCReleaseSocket(TFTP_CLIENT_VARS* pClient)
{
    if(pClient->hSocket != INVALID_UDP_SOCKET)
    {
        TCPIP_UDP_Close(pClient->hSocket);
        pClient->hSocket = INVALID_UDP_SOCKET;
    }
}

static void TCPIP_TFTPC_Cleanup(void)
{
    TFTP_CLIENT_VARS    *pClient=NULL;
    pClient = &gTFTPClientDcpt;
    if(tftpcSignalHandle)
    {
        _TCPIPStackSignalHandlerDeregister(tftpcSignalHandle);
        tftpcSignalHandle = 0;
    }
    tftpcInitCount = 0;

    // close the socket -
    _TFTPCReleaseSocket(pClient);
    
#if (TCPIP_TFTPC_USER_NOTIFICATION != 0)
    // remove notification registered user details.
    TCPIP_Notification_Deinitialize(&tftpcRegisteredUsers, tftpcMemH);
#endif  // (TCPIP_TFTPC_USER_NOTIFICATION != 0)
    
    tftpcMemH = NULL;    
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)

void TCPIP_TFTPC_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData)
{        
    if(stackData->stackAction == TCPIP_STACK_ACTION_DEINIT)
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
        memcpy(pClient->tftpServerAddr.v6Add.v,ipAddr->v6Add.v,sizeof(IPV6_ADDR));
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
    if(strlen(fileName)> (sizeof(pClient->fileName)-1))
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
    memset(pClient->fileName,0,sizeof(pClient->fileName));
    strcpy(pClient->fileName,fileName);
    
    pClient->ipAddrType = ipType;
    pClient->smState = SM_TFTP_PROCESS_COMMAND;   

    return TFTPC_ERROR_NONE;   
}

static TFTP_RESULT TFTPIsPutReady(void)
{
    TCPIP_UINT16_VAL opCode;
    TCPIP_UINT16_VAL blockNumber;
    bool bTimeOut;
    TFTP_CLIENT_VARS*   pClient;
    int replyPktSize=0;

    pClient = &gTFTPClientDcpt;
    // Check to see if timeout has occurred.
    bTimeOut = false;   

    switch(_tftpPutCmdState)
    {
        case SM_TFTP_COMM_WAIT_FOR_ACK:
            replyPktSize = TCPIP_UDP_GetIsReady(pClient->hSocket);
            if(!replyPktSize)
            {
                if (SYS_TMR_TickCountGet() - tftpcTimer >= (TCPIP_TFTPC_CMD_PROCESS_TIMEOUT * SYS_TMR_TickCounterFrequencyGet()))
                {
                    bTimeOut = true;
                    tftpcTimer = SYS_TMR_TickCountGet();
                }
                 // When timeout occurs in this state, application must retry.
                if(bTimeOut)
                {
                    if (_tftpRetries++ > (TCPIP_TFTPC_MAX_RETRIES-1))
                    { // Forget about all previous attempts.
                        _tftpRetries = 1;
                        pClient->smState = SM_TFTP_END;
                        _TFTPNotifyClients(pTftpcIf,TFTPC_EVENT_TIMEOUT,0,0);
                        return TFTP_TIMEOUT;
                    }
                    else
                    {
                        pClient->smState = SM_TFTP_WAIT;
                        MutExVar.group2._tftpBlockNumber.Val--;	// Roll back by one so proper block number ID is sent for the next packet
                        pClient->smState = SM_TFTP_FILE_OPEN_AND_SEND_REQUEST;
                        return TFTP_RETRY;
                    }
                }
                return TFTP_PKT_NOT_RECEIVED;
            }
            // ACK is received.
            _TFTPNotifyClients(pTftpcIf,TFTPC_EVENT_ACKED,0,0);
            
            tftpcTimer = SYS_TMR_TickCountGet();
            // Get opCode.
            TCPIP_UDP_Get(pClient->hSocket,&opCode.v[1]);
            TCPIP_UDP_Get(pClient->hSocket,&opCode.v[0]);

            // Get block number.
            TCPIP_UDP_Get(pClient->hSocket,&blockNumber.v[1]);
            TCPIP_UDP_Get(pClient->hSocket,&blockNumber.v[0]);

            // Discard everything else.
            TCPIP_UDP_Discard(pClient->hSocket);

            // This must be ACK or else there is a problem.
            if (opCode.Val == (uint16_t)TFTP_OPCODE_ACK)
            {
                // Also the block number must match with what we are expecting.
                if (MutExVar.group2._tftpBlockNumber.Val == blockNumber.Val)
                {
                    // Mark that block we sent previously has been ack'ed.
                    _tftpFlags.bits.bIsAcked = true;

                    // Since we have ack, forget about previous retry count.
                    _tftpRetries = 1;

// If this file is being closed, this must be last ack. Declare it as closed.
                    if (_tftpFlags.bits.bIsClosing)
                    {
                        _tftpFlags.bits.bIsClosed = true;                        
                        return TFTP_END_OF_FILE;
                    }
                    // Or else, wait for put to become ready so that caller
                    // can transfer more data blocks.
                    _tftpPutCmdState = SM_TFTP_COMM_SEND_NEXT_DATA_PKT;
                    tftpcTimer = SYS_TMR_TickCountGet();
                    TFTPC_DEBUG_PRINT("Received Block Number : %d \r\n",blockNumber.Val);
                }
                else
                {
                    TFTPC_DEBUG_PRINT("TFTP: IsPutReady(): Unexpected block %d received - dropping it...\n",blockNumber.Val);
                    return TFTP_NOT_READY;
                }
            }
            else if (opCode.Val == (uint16_t)TFTP_OPCODE_ERROR)
            {
                // For error opCode, remember error code so that application
                // can read it later.
                _tftpError = blockNumber.Val;

                // Declare error.
                return TFTP_ERROR;
            }
            else
                break;


        case SM_TFTP_COMM_SEND_NEXT_DATA_PKT:
            // Wait for UDP is to be ready to transmit.
            if(TCPIP_UDP_PutIsReady(pClient->hSocket) < TCPIP_TFTP_CLIENT_MAX_BUFFER_SIZE)
            {
                TCPIP_UDP_OptionsSet(pClient->hSocket, UDP_OPTION_TX_BUFF, (void*)(unsigned int)TCPIP_TFTP_CLIENT_MAX_BUFFER_SIZE);
                break;
            }

         // Put next block of data.
            MutExVar.group2._tftpBlockNumber.Val++;
            TCPIP_UDP_Put(pClient->hSocket,0);
            TCPIP_UDP_Put(pClient->hSocket,TFTP_OPCODE_DATA);

            TCPIP_UDP_Put(pClient->hSocket,MutExVar.group2._tftpBlockNumber.v[1]);
            TCPIP_UDP_Put(pClient->hSocket,MutExVar.group2._tftpBlockNumber.v[0]);

            // Remember that this block is not yet flushed.
            _tftpFlags.bits.bIsFlushed = false;

            // Remember that this block is not acknowledged.
            _tftpFlags.bits.bIsAcked = false;
            
            _tftpPutCmdState = SM_TFTP_COMM_WAIT_FOR_ACK;
            TFTPC_DEBUG_PRINT("Transmitted Block Number : %d \r\n",MutExVar.group2._tftpBlockNumber.Val);
            return TFTP_OK;
	// Suppress compiler warnings on unhandled SM_TFTP_COMM_WAIT_FOR_DATA,
	// SM_TFTP_COMM_DUPLICATE_ACK, SM_TFTP_COMM_SEND_ACK, SM_TFTP_COMM_SEND_LAST_ACK enum
	// states.
        default:
            break;
    }

    return TFTP_NOT_READY;
}

void TCPIP_TFTPC_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != 0)
    { // regular TMO or RX signal occurred
        TCPIP_TFTPC_Process();
    }

}

// send a signal to the TFTP module that data is available
// no manager alert needed since this normally results as a higher layer (UDP) signal
static void _TFTPSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_UDP_SIGNAL_RX_DATA)
    {
        _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}


static void TCPIP_TFTPC_Process(void)
{
    TCPIP_NET_IF* pNetIf=NULL;
    TFTP_CLIENT_VARS*   pClient;
    int32_t wCount=0, wLen=0,status=0;
    uint8_t data[700];
    bool res=true;
    bool bTimeout=false;
    uint32_t    replyPktSize=0;
	bool bindRes = false;
    
    pClient = &gTFTPClientDcpt;
    pNetIf = _TCPIPStackAnyNetLinked(false);
    if(pNetIf == 0 || _TCPIPStackIsConfig(pNetIf) != 0)
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
                pTftpcIf = _TCPIPStackAnyNetLinked(true);
            }
            if(pTftpcIf == 0)
            {   // wait some more
                break;
            }
            
            bindRes = TCPIP_UDP_RemoteBind(pClient->hSocket, pClient->ipAddrType,TCPIP_TFTP_SERVER_PORT,&pClient->tftpServerAddr);
            if (bindRes)
            {
                 // receiving from multiple TFTP servers
                TCPIP_UDP_OptionsSet(pClient->hSocket, UDP_OPTION_STRICT_PORT, (void*)false);
                //TCPIP_UDP_DestinationIPAddressSet(pClient->hSocket, IP_ADDRESS_TYPE_IPV4, &pClient->tftpServerAddr);
                TCPIP_UDP_SocketNetSet(pClient->hSocket, pTftpcIf);
            }
            else
            {
                break;
            }
            pClient->smState++;
            tftpcTimer = SYS_TMR_TickCountGet();
            // connection established
            _TFTPNotifyClients(pTftpcIf,TFTPC_EVENT_CONN_ESTABLISHED,0,0);
            //break;
        case SM_TFTP_UDP_IS_OPENED:
             if(!TCPIP_UDP_TxPutIsReady(pClient->hSocket,(TCPIP_TFTPC_FILENAME_LEN+TCPIP_TFTP_CLIENT_OPCODE+TCPIP_TFTP_CLIENT_OCTET+1)))
             {
                if((SYS_TMR_TickCountGet() - tftpcTimer > TCPIP_TFTPC_ARP_TIMEOUT*SYS_TMR_TickCounterFrequencyGet()))
                {
                    bTimeout = true;
                }
                if(bTimeout)
                {
                    if ( _tftpRetries++ > (TCPIP_TFTPC_MAX_RETRIES-1) )
                    {
                        // Forget about all previous attempts.
                        _tftpRetries = 1;
                        pClient->smState = SM_TFTP_END;
                        // connection Timed out
                        _TFTPNotifyClients(pTftpcIf,TFTPC_EVENT_TIMEOUT,0,0);
                        return;
                    }
                }
                break;
             }
            _tftpRetries = 1;
            pClient->smState++;
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
                    _tftpPutCmdState = SM_TFTP_COMM_WAIT_FOR_ACK;
                }
                else
                {
                    pClient->smState = SM_TFTP_GET_COMMAND;
                    _tftpGetCmdState = SM_TFTP_COMM_WAIT_FOR_DATA;
                }
            }
            // Now TFTP client is busy.
            _TFTPNotifyClients(pTftpcIf,TFTPC_EVENT_BUSY,0,0);
            break;
        case SM_TFTP_PUT_COMMAND:        
            switch(TFTPIsPutReady())
            {
                case TFTP_OK:
                    if(pClient->callbackPos != 0x00u)
                    {// The file was already opened, so load up its ID and seek
                        if(pClient->fileDescr == SYS_FS_HANDLE_INVALID)
                        {// No file handles available, so wait for now
                            pClient->smState = SM_TFTP_END;
                            return;
                        }                        
                        SYS_FS_FileSeek(pClient->fileDescr,(int32_t)pClient->callbackPos,SYS_FS_SEEK_SET);                    
                    }
                                      
                    // Put as many bytes as possible
                    wCount = TCPIP_UDP_PutIsReady(pClient->hSocket);
                    wLen = 0;
                    if(wCount >= TCPIP_TFTP_BLOCK_SIZE_SUPPORTED)
                    {
                        wLen = SYS_FS_FileRead(pClient->fileDescr,data,TCPIP_TFTP_BLOCK_SIZE_SUPPORTED);
                        if((wLen == 0)||(wLen < TCPIP_TFTP_BLOCK_SIZE_SUPPORTED))
                        {// If no bytes were read, an EOF was reached
                            if((wLen != 0)||(wLen < TCPIP_TFTP_BLOCK_SIZE_SUPPORTED))
                            {
                                TCPIP_UDP_ArrayPut(pClient->hSocket, data, wLen);
                                // flush last chunk of less than 512 bytes
                                TCPIP_UDP_Flush(pClient->hSocket);
                            }
                            SYS_FS_FileClose(pClient->fileDescr);
                            SYS_CONSOLE_PRINT("\r\nNumber of bytes transmitted : %d bytes \r\n",pClient->callbackPos);
                            pClient->fileDescr = -1;
                            pClient->callbackPos = 0;
                            pClient->smState = SM_TFTP_END;
                            TCPIP_UDP_Disconnect(pClient->hSocket,false);
                            // TFTP Communication completed , that is File is uploaded properly.
                            _TFTPNotifyClients(pTftpcIf,TFTPC_EVENT_COMPLETED,0,0);
                            _tftpFlags.bits.bIsClosing =  true;
                            return ;
                        }
                        else
                        {// Write the bytes to the socket
                            TCPIP_UDP_ArrayPut(pClient->hSocket, data, wLen);
                            // flush these 512 bytes
                            TCPIP_UDP_Flush(pClient->hSocket);

                            _tftpFlags.bits.bIsFlushed = true;
                            // Save the new address and close the file
                            status = SYS_FS_FileTell(pClient->fileDescr);
                            if(status == -1)
                               pClient->callbackPos = 0;
                            else
                               pClient->callbackPos = (uint32_t)status;
                            SYS_CONSOLE_MESSAGE("#");
                        }
                    }
                    
                    pClient->smState = SM_TFTP_PUT_COMMAND;
                break;
                case TFTP_PKT_NOT_RECEIVED:
                    break;
                case TFTP_RETRY:
                    pClient->smState = SM_TFTP_FILE_OPEN_AND_SEND_REQUEST;
                    break;
                case TFTP_NOT_READY:
                    if(pClient->fileDescr != -1)
                    {
                        SYS_FS_FileClose(pClient->fileDescr);
                        pClient->fileDescr = -1;
                    }
                    pClient->callbackPos = 0;
                    pClient->smState = SM_TFTP_END;
                    // Declined due to Bad PDU
                    _TFTPNotifyClients(pTftpcIf,TFTPC_EVENT_DECLINE,0,0);
                    break;
                case  TFTP_END_OF_FILE:
                    SYS_FS_FileClose(pClient->fileDescr);
                    pClient->fileDescr = -1;
                    pClient->callbackPos = 0;
                    pClient->smState = SM_TFTP_END;
                    TCPIP_UDP_Disconnect(pClient->hSocket,false);
                    // TFTP Communication completed , that is File is uploaded properly.
                    _TFTPNotifyClients(pTftpcIf,TFTPC_EVENT_COMPLETED,0,0);
                    break;
                default:
                    break;
            }        
            break;
        case SM_TFTP_GET_COMMAND:
            switch(TFTPIsGetReady(data,&wCount))
			{
                case TFTP_OK:
                    // Check if their is any byte need to be written to the FS or Buffer-
                    if(wCount == 0)
                    {
                        break;
                    }
                    if(pClient->callbackPos != 0x00u)
                    {// The file was already opened, so load up its ID and seek
                        if(pClient->fileDescr == SYS_FS_HANDLE_INVALID)
                        {// No file handles available, so wait for now
                            pClient->smState = SM_TFTP_END;
                            return;
                        }                   
                        SYS_FS_FileSeek(pClient->fileDescr,(int32_t)pClient->callbackPos,SYS_FS_SEEK_SET);                     
                    }              
                    // first 512 bytes
                    wLen = SYS_FS_FileWrite(pClient->fileDescr,data,wCount);
                    if(wLen == -1)
                    {// If no bytes were read, an EOF was reached
                        SYS_FS_FileClose(pClient->fileDescr);
                        pClient->fileDescr = -1;
                        pClient->callbackPos = 0;
                        pClient->smState = SM_TFTP_END;
                        TCPIP_UDP_Disconnect(pClient->hSocket,false);
                        // TFTP Communication completed , that is File is uploaded properly.
                        _TFTPNotifyClients(pTftpcIf,TFTPC_EVENT_COMPLETED,0,0);
                        return ;
                    }                   
                    else
                    {   
                        // Save the new address
                        status = SYS_FS_FileTell(pClient->fileDescr);
                        if(status == -1)
                            pClient->callbackPos = 0;
                        else
                            pClient->callbackPos = (uint32_t)status;                        
                        // Data receive started.
                        _TFTPNotifyClients(pTftpcIf,TFTP_EVENT_DATA_RECEIVED,(void*)(pClient->callbackPos-wCount),wCount);
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
                        SYS_FS_FileClose(pClient->fileDescr);
                        pClient->fileDescr = -1;
                    }                
                    pClient->callbackPos = 0;
                    pClient->smState = SM_TFTP_END;
                    // Declined due to Bad PDU
                    _TFTPNotifyClients(pTftpcIf,TFTPC_EVENT_DECLINE,0,0);
                    break;
                case  TFTP_END_OF_FILE:                   
                    SYS_FS_FileClose(pClient->fileDescr);
                    pClient->fileDescr = -1;
                    pClient->callbackPos = 0;
                    pClient->smState = SM_TFTP_END;
                    TCPIP_UDP_Disconnect(pClient->hSocket,false);
                    // TFTP Communication completed , that is File is uploaded properly.
                    _TFTPNotifyClients(pTftpcIf,TFTPC_EVENT_COMPLETED,0,0);
                    break;
                default:
                    break;
            }
            break;
        case SM_TFTP_END:
            // remove all other UDP packets from the socket buffer
            // This also removes the last ack from the Socket buffer
            while(1)
            {
                replyPktSize = TCPIP_UDP_GetIsReady(pClient->hSocket);
                if(replyPktSize == 0)
                {
                    break;
                }
                else
                {
                    TCPIP_UDP_Discard(pClient->hSocket);
                }
            }
            break;
        default:
            break;
            
    }
}

static void _TFTPSendFileName(TFTP_OPCODE opcode, uint8_t *fileName)
{
    uint8_t c;
    TFTP_CLIENT_VARS*   pClient;
    pClient = &gTFTPClientDcpt;	

    
    // Write opCode
    TCPIP_UDP_Put(pClient->hSocket,0);
    TCPIP_UDP_Put(pClient->hSocket,opcode);

    // write file name, including NULL.
    do
    {
        c = *fileName++;
        TCPIP_UDP_Put(pClient->hSocket,c);
    } while ( c != '\0' );

    // Write mode - always use octet or binay mode to transmit files.
    TCPIP_UDP_Put(pClient->hSocket,'o');
    TCPIP_UDP_Put(pClient->hSocket,'c');
    TCPIP_UDP_Put(pClient->hSocket,'t');
    TCPIP_UDP_Put(pClient->hSocket,'e');
    TCPIP_UDP_Put(pClient->hSocket,'t');
    TCPIP_UDP_Put(pClient->hSocket,0);

    // Transmit it.
    TCPIP_UDP_Flush(pClient->hSocket);
    
}

static bool TFTPOpenFile(const char *fileName, TFTP_FILE_MODE mode)
{
    int32_t fp;
    TFTP_CLIENT_VARS*   pClient;
    
    pClient = &gTFTPClientDcpt;	
    
    fp = pClient->fileDescr;

    if(mode == TFTP_FILE_MODE_WRITE)
    {
        fp = SYS_FS_FileOpen((const char*)fileName,SYS_FS_FILE_OPEN_READ);
    }
    else
    {
        fp = SYS_FS_FileOpen((const char*)fileName,SYS_FS_FILE_OPEN_WRITE);
    }

    if(fp == SYS_FS_HANDLE_INVALID)
    {// File not found, so abort
        return false;
    }
    pClient->fileDescr = fp;
 
    pClient->callbackPos = 0;
    _tftpFlags.bits.bIsClosing = false;
    _tftpFlags.bits.bIsClosed = false;

    // Tell remote server about our intention.
    _TFTPSendFileName(mode, (uint8_t *)fileName);

    // Clear all flags.
    _tftpFlags.Val = 0;

    // Remember start tick for this operation.
    tftpcTimer = SYS_TMR_TickCountGet();

    // Depending on mode of operation, remote server will respond with
    // specific block number.
    if ( mode == TFTP_FILE_MODE_READ )
    {
        // Remember that we are reading a file.
        _tftpFlags.bits.bIsReading = true;

        // For read operation, server would respond with data block of 1.
        MutExVar.group2._tftpBlockNumber.Val = 1;

        // Next packet would be the data packet.
        _tftpGetCmdState = SM_TFTP_COMM_WAIT_FOR_DATA;
        // Request is sent to the Server  either to Get a file
        _TFTPNotifyClients(pTftpcIf,TFTPC_EVENT_GET_REQUEST,0,0);
    }

    else
    {
        // Remember that we are writing a file.
        _tftpFlags.bits.bIsReading = false;

        // For write operation, server would respond with data block of 0.
        MutExVar.group2._tftpBlockNumber.Val = 0;

        // Next packet would be the ACK packet.
        _tftpPutCmdState = SM_TFTP_COMM_WAIT_FOR_ACK;
        
        // Request is sent to the Server  either to Put a file
        _TFTPNotifyClients(pTftpcIf,TFTPC_EVENT_PUT_REQUEST,0,0);
    }
    
    return true;
}


static TFTP_RESULT TFTPIsGetReady(uint8_t *getData, int32_t *len)
{
    TCPIP_UINT16_VAL opCode;
    TCPIP_UINT16_VAL blockNumber;
    bool bTimeOut;
    TFTP_CLIENT_VARS*   pClient;
    uint32_t    replyPktSize=0;

    pClient = &gTFTPClientDcpt;
    // Check to see if timeout has occurred.
    bTimeOut = false;
    if ( SYS_TMR_TickCountGet() - tftpcTimer >= (TCPIP_TFTPC_CMD_PROCESS_TIMEOUT * SYS_TMR_TickCounterFrequencyGet()) )
    {
        bTimeOut = true;
        tftpcTimer = SYS_TMR_TickCountGet();
    }

    switch(_tftpGetCmdState)
    {
        case SM_TFTP_COMM_WAIT_FOR_DATA:
            // If timeout occurs in this state, it may be because, we have not
            // even received very first data block or some in between block.
            if ( bTimeOut == true )
            {
                bTimeOut = false;
                if ( _tftpRetries++ > (TCPIP_TFTPC_MAX_RETRIES-1) )
                {   // Forget about all previous attempts.
                    _tftpRetries = 1;
                    return TFTP_TIMEOUT;
                }

                // If we have not even received first block, ask application
                // retry.
                if ( MutExVar.group2._tftpBlockNumber.Val == 1u )
                {
                    return TFTP_RETRY;
                }
                else
                {
                    // Block number was already incremented in last ACK attempt,
                    // so decrement it.
                    MutExVar.group2._tftpBlockNumber.Val--;

                    // Do it.
                    _tftpGetCmdState = SM_TFTP_COMM_SEND_ACK;
                    break;
                }
            }
            tftpcTimer = SYS_TMR_TickCountGet();
            replyPktSize = TCPIP_UDP_GetIsReady(pClient->hSocket);
            // For Read operation, server will respond with data block.
            if ( !replyPktSize)
            {
                break;
            }
            
            // Get opCode
            TCPIP_UDP_Get(pClient->hSocket,&opCode.v[1]);
            TCPIP_UDP_Get(pClient->hSocket,&opCode.v[0]);

            // Get block number.
            TCPIP_UDP_Get(pClient->hSocket,&blockNumber.v[1]);
            TCPIP_UDP_Get(pClient->hSocket,&blockNumber.v[0]);
            
            // get the data array from the socket  
            *len = TCPIP_UDP_ArrayGet(pClient->hSocket,getData,replyPktSize-4);
            
            // In order to read file, this must be data with block number of 0.
            if ( opCode.Val == (uint16_t)TFTP_OPCODE_DATA )
            {
                // Make sure that this is not a duplicate block.
                if ( MutExVar.group2._tftpBlockNumber.Val == blockNumber.Val )
                {
                    // Mark that we have not acked this block.
                    _tftpFlags.bits.bIsAcked = false;

                    // Since we have a packet, forget about previous retry count.
                    _tftpRetries = 1;

                   _tftpGetCmdState = SM_TFTP_COMM_SEND_ACK;
                    if(replyPktSize < (TCPIP_TFTP_CLIENT_MAX_BUFFER_SIZE-1))
                    {
                        _tftpFlags.bits.bIsClosing =  true;
                        _tftpGetCmdState = SM_TFTP_COMM_SEND_LAST_ACK;
                    }
                    return TFTP_OK;
                }

                // If received block has already been received, simply ack it
                // so that Server can "get over" it and send next block.
                else if ( MutExVar.group2._tftpBlockNumber.Val > blockNumber.Val )
                {
                    TFTPC_DEBUG_PRINT("TFTP: IsGetReady(): Duplicate block %d received - dropping it...\n",blockNumber.Val);
                    MutExVar.group2._tftpDuplicateBlock.Val = blockNumber.Val;
                    _tftpGetCmdState = SM_TFTP_COMM_DUPLICATE_ACK;
                }
            }
            // Discard all unexpected and error blocks.
            TCPIP_UDP_Discard(pClient->hSocket);

            // If this was an error, remember error code for later delivery.
            if ( opCode.Val == (uint16_t)TFTP_OPCODE_ERROR )
            {
                _tftpError = blockNumber.Val;
                return TFTP_ERROR;
            }
            break;
            
    case SM_TFTP_COMM_DUPLICATE_ACK:
        if ( TCPIP_UDP_PutIsReady(pClient->hSocket))
        {
            _TFTPSendAck(MutExVar.group2._tftpDuplicateBlock);
            _tftpGetCmdState = SM_TFTP_COMM_WAIT_FOR_DATA;
        }
        break;

    case SM_TFTP_COMM_SEND_LAST_ACK:
        if(_tftpFlags.bits.bIsClosing)
        {
            _tftpFlags.bits.bIsClosed = true;
        }
        SYS_CONSOLE_PRINT("\r\nTFTP: Total Bytes received: %d bytes \r\n ",pClient->callbackPos);
    case SM_TFTP_COMM_SEND_ACK:
        if ( TCPIP_UDP_PutIsReady(pClient->hSocket) )
        {
            _TFTPSendAck(MutExVar.group2._tftpBlockNumber);

            // This is the next block we are expecting.
            MutExVar.group2._tftpBlockNumber.Val++;

            // Remember that we have already acked current block.
            _tftpFlags.bits.bIsAcked = true;

            if ( _tftpGetCmdState == SM_TFTP_COMM_SEND_LAST_ACK )
            {
                return TFTP_END_OF_FILE;
            }

            _tftpGetCmdState = SM_TFTP_COMM_WAIT_FOR_DATA;
            SYS_CONSOLE_MESSAGE("#");
            return TFTP_ACK_SEND;
        }
        break;

	// Suppress compiler warnings on unhandled SM_TFTP_WAIT and
	// SM_TFTP_WAIT_FOR_ACK states.
    default:
    	break;
    }
    return TFTP_NOT_READY;
}

static void _TFTPSendAck(TCPIP_UINT16_VAL blockNumber)
{
    TFTP_CLIENT_VARS*   pClient;
    pClient = &gTFTPClientDcpt;
    // Write opCode.
    TCPIP_UDP_Put(pClient->hSocket,0);
    TCPIP_UDP_Put(pClient->hSocket,TFTP_OPCODE_ACK);

    // Write block number for this ack.
    TCPIP_UDP_Put(pClient->hSocket,blockNumber.v[1]);
    TCPIP_UDP_Put(pClient->hSocket,blockNumber.v[0]);

    // Transmit it.
    TCPIP_UDP_Flush(pClient->hSocket);
}

// Register an TFTP event handler
// Use hNet == 0 to register on all interfaces available
// Returns a valid handle if the call succeeds,
// or a null handle if the call failed.
// Function has to be called after the TFTP is initialized
// The hParam is passed by the client and will be used by the TFTP when the notification is made.
// It is used for per-thread content or if more modules, for example, share the same handler
// and need a way to differentiate the callback.
#if (TCPIP_TFTPC_USER_NOTIFICATION != 0)
TCPIP_TFTPC_HANDLE TCPIP_TFTPC_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_TFTPC_EVENT_HANDLER handler, const void* hParam)
{
    if(handler && tftpcMemH)
    {
        TCPIP_TFTPC_LIST_NODE tftpNode;
        tftpNode.handler = handler;
        tftpNode.hParam = hParam;
        tftpNode.hNet = hNet;

        return (TCPIP_TFTPC_LIST_NODE*)TCPIP_Notification_Add(&tftpcRegisteredUsers, tftpcMemH, &tftpNode, sizeof(tftpNode));
    }

    return 0;
}


// deregister the event handler
bool TCPIP_TFTPC_HandlerDeRegister(TCPIP_TFTPC_HANDLE hTftpc)
{
    if(hTftpc && tftpcMemH)
    {
        if(TCPIP_Notification_Remove((SGL_LIST_NODE*)hTftpc, &tftpcRegisteredUsers, tftpcMemH))
        {
            return true;
        }
    }
    return false;
}

static void _TFTPNotifyClients(TCPIP_NET_IF* pNetIf, TCPIP_TFTPC_EVENT_TYPE evType,void *buf,uint32_t buffSize)
{
    TCPIP_TFTPC_LIST_NODE* dNode;

    TCPIP_Notification_Lock(&tftpcRegisteredUsers);
    for(dNode = (TCPIP_TFTPC_LIST_NODE*)tftpcRegisteredUsers.list.head; dNode != 0; dNode = dNode->next)
    {
        if(dNode->hNet == 0 || dNode->hNet == pNetIf)
        {   // trigger event
            (*dNode->handler)(pNetIf, evType,buf,buffSize,dNode->hParam);
        }
    }
    TCPIP_Notification_Unlock(&tftpcRegisteredUsers);
}
#endif  // (TCPIP_TFTPC_USER_NOTIFICATION != 0)

TCPIP_TFTPC_EVENT_TYPE TCPIP_TFTPC_GetEventNotification(void)
{
    TCPIP_TFTPC_EVENT_TYPE eventType=TFTPC_EVENT_NONE;
    TFTP_CLIENT_VARS*   pClient;
    pClient = &gTFTPClientDcpt;
    
    if(pClient->smState != SM_TFTP_END)
    {
        // The TFTP Client is busy with processing the file communication.
        eventType |=TFTPC_EVENT_BUSY;
    }
    
    if(_tftpFlags.bits.bIsReading==true)
    {
        eventType |= TFTPC_EVENT_GET_REQUEST;
        if(_tftpFlags.bits.bIsAcked)
        {
            // ACK is sent for the data received during the READ mode
            eventType |=TFTPC_EVENT_ACKED;
        }
        if(_tftpFlags.bits.bIsClosed)
        {
            // Last ACK is sent for the last block it is received.
            // Socket is not closed. only the file descriptor is closed
            eventType |=TFTPC_EVENT_COMPLETED;
        }        
    }
    else
    {
        eventType |= TFTPC_EVENT_PUT_REQUEST;
        if(_tftpFlags.bits.bIsAcked)
        {
            // ACK is received for the data transmitted during the WRITE mode
            eventType |=TFTPC_EVENT_ACKED;
        }
        if(_tftpFlags.bits.bIsClosed)
        {
            // The last ACK is received for the data transmitted during the WRITE mode.
            // Socket is not closed. only the file descriptor is closed
            eventType |=TFTPC_EVENT_COMPLETED;
        }        
    }
    return eventType;
}
#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_TFTP_CLIENT)

