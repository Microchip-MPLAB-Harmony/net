/*******************************************************************************
  Trivial File Transfer Protocol (TFTP) Server

  Summary:
    TFTP server implementation file
    
  Description:
    This source file contains the functions and storage of the 
    TFTP Server routines
    
    TFTP protocol RFC support - 1350, TFTP option code RFC - 2347, 2348, 2349.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*
Copyright (C) 2013-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************


#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_TFTP_SERVER

#include "tcpip/src/tcpip_private.h"

#if defined(TCPIP_STACK_USE_TFTP_SERVER)
#include "tcpip/src/common/sys_fs_shell.h"
#include "tftps_private.h"


// *****************************************************************************
// *****************************************************************************
// Section: Static Data
// *****************************************************************************
// *****************************************************************************


static const char *tftpsLocalWebPath = TCPIP_TFTPS_MOUNT_POINT;
static TFTPS_CB            gTftpClientCB[TCPIP_TFTPS_CLIENT_NUMBER];
static TCPIP_TFTPS_DCPT    gTftpsDcpt;
static int                gTftpsInitCount = 0;
// TFTP server task signal registration handler
static TCPIP_SIGNAL_HANDLE   gTftpsSignalHandle = NULL;
// TFTP server UDP RX packet signal handle
static TCPIP_UDP_SIGNAL_HANDLE gUdpRxSigHandle = NULL;

// conversion functions/helpers
//

static __inline__ TCPIP_TFTPS_LIST_NODE*  __attribute__((always_inline)) FC_SglNode2TftpsNode(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        TCPIP_TFTPS_LIST_NODE* tftpsNode;
    }U_NODE_TFTPS_NODE;

    U_NODE_TFTPS_NODE.node = node;
    return U_NODE_TFTPS_NODE.tftpsNode;
}


// *****************************************************************************
// *****************************************************************************
// Section: Static functions 
// *****************************************************************************
// *****************************************************************************
static void F_TFTPSNotifyUserClients(TFTPS_CB* pTftpCB, TCPIP_TFTPS_EVENT_TYPE evType);
static bool F_TFTPS_Enable(TCPIP_NET_HANDLE hNet, bool checkIfUp,IP_ADDRESS_TYPE ipType);
static void TCPIP_TFTPS_Cleanup(void);
static void TCPIP_TFTPS_Process(void);
static TCPIP_TFTPS_RESULT F_TFTPS_Ack(TFTPS_CB *tftp_con);
static uint32_t F_TFTPS_Retransmit(TFTPS_CB *tftp_con);
static TCPIP_TFTPS_RESULT F_TFTPS_RRecv(TFTPS_CB** tftp_con, uint16_t *byteReceived);
static TCPIP_TFTPS_RESULT F_TFTPS_Process_RequestPacket(TFTPS_CB *tftp_con, uint16_t bytes_received);
static TCPIP_TFTPS_RESULT F_TFTPS_Process_Data(TFTPS_CB *tftp_con, uint16_t bytes_received);
static void F_TFTPS_ReleaseResource(TFTPS_CB   *tftp_con);
static uint16_t F_TFTPS_Send_Data(TFTPS_CB *tftp_con, uint16_t bytes_received);
static TCPIP_TFTPS_RESULT F_TFTPS_Error(UDP_SOCKET skt, TFTP_ERROR_CODE error_code, const char *err_string);
static TCPIP_TFTPS_RESULT F_TFTPS_Check_Options(TFTPS_CB *tftp_con, uint16_t bytes_received, uint16_t count, uint8_t* rx_buf);
static TFTPS_CB  *F_TFTPS_GetClientCB(uint8_t client_cnt);
static TCPIP_TFTPS_RESULT F_TFTPS_Process_Ack(TFTPS_CB *tftp_con,uint16_t bytes_received);
static TCPIP_TFTPS_RESULT F_TFTPS_validateClientRquest(UDP_SOCKET_INFO *sktInfo,TFTPS_CB *pClientCB);

static bool F_TFTPS_Recv_Request_State(TFTPS_CB* tftp_con);
static bool F_TFTPS_Process_Data_State(TFTPS_CB* tftp_con);
static bool F_TFTPS_Send_Data_State(TFTPS_CB* tftp_con);
static bool F_TFTPS_ShellRegister(TFTPS_CB   *pClientCB);
static void F_TFTPS_ShellDeRegister(TFTPS_CB   *pClientCB);
// TFTPS server state function
// returns >= 0 the size of the option added
// returns < 0 for failure
typedef bool (*F_TFTPS_StateFnc)(TFTPS_CB* tftp_con);
typedef struct
{
    //unsigned uint8_t        state;         // implicit state option
    F_TFTPS_StateFnc    tftpServStateFnc;   // TFTP server State function
}S_TFTPS_STATE_TBL_ENTRY;

static const S_TFTPS_STATE_TBL_ENTRY   T_TFTPSStateTbl[SM_TFTPS_END] = 
{
    // Note: this table entries are processed in Sequential order!
    // State                                     // TFTP Server State function 
    {  /* SM_TFTPS_HOME */                  NULL                      }, 
    {  /* SM_TFTPS_RRECV */                 &F_TFTPS_Recv_Request_State },  // always the 1st!
    {  /* SM_TFTPS_PROCESS_DATA */          &F_TFTPS_Process_Data_State },
    {  /* SM_TFTPS_SEND_DATA */             &F_TFTPS_Send_Data_State },   
};

/*
 * This Function is for TFTP server state SM_TFTPS_RRECV
 */
static bool F_TFTPS_Recv_Request_State(TFTPS_CB* tftp_con)
{
    TCPIP_TFTPS_RESULT res=TFTPS_RES_OK;
    uint16_t bytes_received=0;
    
    res = F_TFTPS_RRecv(&tftp_con,&bytes_received);
    if(res != TFTPS_RES_OK)
    {
        /* if return is equal to TFTPS_RES_CLIENT_ERROR, then close the socket*/
        if(res == TFTPS_RES_CLIENT_ERROR)
        {
            F_TFTPS_ReleaseResource(tftp_con);
        }
        return false;
    }
// check if the TFTP client control block , if the status TFTPS_CB_FREE, then client process should not proceed
    if(tftp_con->status == TFTPS_CB_FREE)
    {
        return false;
    }
    if(tftp_con->trans_buf == NULL)
    {
        tftp_con->smState = (uint8_t)SM_TFTPS_RRECV;
        return false;
    }

    if((tftp_con->status == TFTPS_TRANSFERRING_FILE)&& (tftp_con->type == TFTPS_WRITE_TYPE))
    {
        tftp_con->smState = (uint8_t)SM_TFTPS_PROCESS_DATA;
    }
    else if((tftp_con->status == TFTPS_TRANSFERRING_FILE)&& (tftp_con->type == TFTPS_READ_TYPE))
    {
        tftp_con->smState = (uint8_t)SM_TFTPS_SEND_DATA;
    }
    else
    {
        if(F_TFTPS_Process_RequestPacket(tftp_con, bytes_received) != TFTPS_RES_OK)
        {
            tftp_con->smState = (uint8_t)SM_TFTPS_RRECV;
            return false;
        }
    }
    tftp_con->reply_timeout = SYS_TMR_TickCountGet();
    return true;
}

/*
 * This Function is for TFTP server state SM_TFTPS_PROCESS_DATA
 */
static bool F_TFTPS_Process_Data_State(TFTPS_CB* tftp_con)
{
    uint16_t bytes_received = 0U;
    
    if((tftp_con == NULL) ||(tftp_con->status == TFTPS_CB_FREE))
    {
        tftp_con->smState = (uint8_t)SM_TFTPS_RRECV;
        return false;
    }
    bytes_received = TCPIP_UDP_GetIsReady(tftp_con->cSkt);
    if(bytes_received == 0U)
    {
        /* Retransmit the last packet until we receive something from
        * the client or until we have retransmitted TCPIP_TFTPS_RETRANSMIT_COUNT 
        * times
        */
        if(tftp_con->retransmits < (uint8_t)TCPIP_TFTPS_RETRANSMIT_COUNT)
        {
            if(F_TFTPS_Retransmit(tftp_con) == 0U)
            {
                return false;
            }
            tftp_con->retransmits++;                   
        }

        tftp_con->smState = (uint8_t)SM_TFTPS_RRECV;
        return false;
    }

    /* Process the first data packet that was sent to the server. */
    if(F_TFTPS_Process_Data(tftp_con, bytes_received) != TFTPS_RES_OK)
    {
        tftp_con->status = TFTPS_PROCESS_INPROGRESS;
    }

    if(tftp_con->status == TFTPS_TRANSFER_COMPLETE)
    {
        // send event notification after file transfer completion
        F_TFTPSNotifyUserClients(tftp_con,TCPIP_TFTPS_EVENT_WRITE_COMPLETED);
        // release data socket, close file descriptor and close other parameters
        F_TFTPS_ReleaseResource(tftp_con);                    
    }
    tftp_con->smState = (uint8_t)SM_TFTPS_RRECV;
    
    return true;
}

/*
 * This Function is for TFTP server state SM_TFTPS_SEND_DATA
 */
static bool F_TFTPS_Send_Data_State(TFTPS_CB* tftp_con)
{
    uint16_t bytes_received=0;
    
     // check if there is valid TFTP control block.
    if((tftp_con == NULL) ||(tftp_con->status == TFTPS_CB_FREE))
    {
        tftp_con->smState = (uint8_t)SM_TFTPS_RRECV;
        return false;
    }
    // verify ACK packet and this happens after first DATA send.
    if(((tftp_con->status == TFTPS_TRANSFERRING_FILE) 
            || (tftp_con->status == TFTPS_TRANSFER_COMPLETE)) && (tftp_con->block_number > 0U))
    {
        bytes_received = TCPIP_UDP_GetIsReady(tftp_con->cSkt);
        if(bytes_received == 0U)
        {
            tftp_con->smState = (uint8_t)SM_TFTPS_RRECV;
            return false;
        }
    }
    
    /* Process the first data packet that was sent to the server. */
    if(F_TFTPS_Send_Data(tftp_con, bytes_received) != 0U)
    {
        if(tftp_con->status != TFTPS_TRANSFER_COMPLETE)
        {
            tftp_con->status = TFTPS_PROCESS_INPROGRESS;
        }
    }

    tftp_con->smState = (uint8_t)SM_TFTPS_RRECV;
    
    return true;
}

// send a signal to the TFTP server module that data is available
// no manager alert needed since this normally results as a higher layer (UDP) signal
static void F_TFTPSSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_UDP_SIGNAL_RX_DATA)
    {
        (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}

static inline void __attribute__((always_inline)) F_TFTPS_Put16(uint8_t* ptr, unsigned int offset, uint16_t value)
{
    uint16_t* p16 = FC_Cptr82Ptr16(ptr + offset);
    TCPIPStack_Assert(((uint32_t)p16 & 0x1U) == 0U, __FILE__, __func__, __LINE__);    // check the proper alignment
    *p16 = TCPIP_Helper_htons(value);
} /* F_TFTPS_Put16 */

static inline uint16_t __attribute__((always_inline)) F_TFTPS_Get16(uint8_t* ptr, unsigned int offset)
{
    uint16_t* p16 = FC_Cptr82Ptr16(ptr + offset);
    TCPIPStack_Assert(((uint32_t)p16 & 0x1U) == 0U, __FILE__, __func__, __LINE__);    // check the proper alignment
    return TCPIP_Helper_ntohs(*p16);
} /* F_TFTPS_Get16 */


static bool F_TFTP_Server_start(TCPIP_NET_HANDLE hNet,IP_ADDRESS_TYPE ipType)
{
    UDP_SOCKET s;
    uint16_t minTftpsTxSize = (uint16_t)TCPIP_TFTPS_MIN_UDP_TX_BUFFER_SIZE;
    uint16_t   txBuffSize=0;
    TCPIP_TFTPS_DCPT     *pTftpDcpt=NULL;
    pTftpDcpt = &gTftpsDcpt;
    
    pTftpDcpt->uSkt = INVALID_UDP_SOCKET;

    s = TCPIP_UDP_ServerOpen(ipType, TCPIP_TFTP_SERVER_PORT, NULL);
    if ( s == INVALID_UDP_SOCKET)
    {
        return false;
    }

    // this is the initial buffer size for TFTP server communication
    // make sure the socket is created with enough TX space
    (void)TCPIP_UDP_OptionsGet(s, UDP_OPTION_TX_BUFF, (void*)&txBuffSize);
    if(txBuffSize < (uint16_t)minTftpsTxSize)
    {
        if(!TCPIP_UDP_OptionsSet(s, UDP_OPTION_TX_BUFF, FC_Uint2VPtr((uint32_t)minTftpsTxSize)))
        {  // close the server socket
            (void)TCPIP_UDP_Close(s);
            return false;
        }
    }
    
    // set the socket net
    if(!TCPIP_UDP_SocketNetSet(s,hNet))
    {
        // close the server socket
        (void)TCPIP_UDP_Close(s);
        return false;
    }
    // UDP RX Receive signal register
    gUdpRxSigHandle = TCPIP_UDP_SignalHandlerRegister(s, TCPIP_UDP_SIGNAL_RX_DATA, &F_TFTPSSocketRxSignalHandler, NULL);
    if(gUdpRxSigHandle == NULL)
    {
        // close the server socket
        (void)TCPIP_UDP_Close(s);
        return false;
    }
    
    pTftpDcpt->uSkt = s;
    return true;
}

// *****************************************************************************
// *****************************************************************************
// Section: Initialize and de-initialize functions 
// *****************************************************************************
// *****************************************************************************
bool TCPIP_TFTPS_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* initData)
{ 
    bool    initFail = false;
    uint8_t client_cnt;
    
    if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {  // interface restart
        return true;
    }
    // stack initialization
    while(gTftpsInitCount == 0)
    {
        gUdpRxSigHandle = NULL;
        gTftpsDcpt.uSkt    = INVALID_UDP_SOCKET;
        gTftpsDcpt.memH    = stackCtrl->memH;

        if(initData == NULL)
        {
            return false;
        }

        const TCPIP_TFTPS_MODULE_CONFIG* tftpsData = (const TCPIP_TFTPS_MODULE_CONFIG*)initData;
        if(tftpsData->mountPath != NULL)
        {
            tftpsLocalWebPath = tftpsData->mountPath;
        }
        // create the TFTP signal handler
        gTftpsSignalHandle = TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_TFTPS_Task, TCPIP_TFTPS_TASK_TICK_RATE); 
        if(gTftpsSignalHandle == NULL)
        {   // cannot create the TFTP server signal handler
            initFail = true;
        }
        for(client_cnt = 0; client_cnt < (uint8_t)TCPIP_TFTPS_CLIENT_NUMBER; client_cnt++)
        {
            (void)memset(&gTftpClientCB[client_cnt],0,sizeof(gTftpClientCB[client_cnt]));
            gTftpClientCB[client_cnt].status = TFTPS_CB_FREE;
            gTftpClientCB[client_cnt].cSkt = INVALID_UDP_SOCKET;
            gTftpClientCB[client_cnt].file_desc = SYS_FS_HANDLE_INVALID;            
        }
#if (M_TFTPS_USER_NOTIFICATION != 0)
        if(TCPIP_Notification_Initialize(&gTftpsDcpt.tftpsRegisteredUsers) == false)
        {
            break;
        }
#endif  // (M_TFTPS_USER_NOTIFICATION != 0)
   
        break;
    }
    if(initFail)
    {
        TCPIP_TFTPS_Cleanup();
        return false;
    }
    return(true);
}

static bool F_TFTPS_ShellRegister(TFTPS_CB   *pClientCB)
{
    SYS_FS_SHELL_RES    res;
    // SYSFS shell registration object
    if(pClientCB != NULL)
    {
        if(pClientCB->tftps_shell_obj == NULL)
        {
            pClientCB->tftps_shell_obj = SYS_FS_Shell_Create(tftpsLocalWebPath, SYS_FS_SHELL_FLAG_NONE, NULL, NULL, &res);
            if( pClientCB->tftps_shell_obj == NULL)
            {
                SYS_ERROR(SYS_ERROR_ERROR, " TFTP Server: Wrapper object failure : %d ",res);
                return false;
            }
        }
    }
    
    return true;
}

static void F_TFTPS_ShellDeRegister(TFTPS_CB   *pClientCB)
{
    // clean up Shell registered clients    

    if(pClientCB != NULL)
    {
        // Wrapper object cleanup
        if(pClientCB->tftps_shell_obj != NULL)
        {
            (void)(pClientCB->tftps_shell_obj->delete)(pClientCB->tftps_shell_obj);
            pClientCB->tftps_shell_obj = NULL;
        }
    }
}

/*
 * clean up the TFTP server that includes close the TFTP server socket,
 * deregister UDP signal handle, 
 * deregister TFTP server stack signal handle
 * de-initialize notification
 */
static void TCPIP_TFTPS_Cleanup(void)
{
    TCPIP_TFTPS_DCPT        *pTftpDcpt=NULL;
    uint8_t                 clientCnt;
    TFTPS_CB                *pClientCB=NULL;
    pTftpDcpt = &gTftpsDcpt;
    if(pTftpDcpt->uSkt != INVALID_UDP_SOCKET)
    {
        // remove UDP signal handler
        if(gUdpRxSigHandle != NULL)
        {
            (void)TCPIP_UDP_SignalHandlerDeregister(pTftpDcpt->uSkt, gUdpRxSigHandle);
            gUdpRxSigHandle = NULL;
        }
        (void)TCPIP_UDP_Close(pTftpDcpt->uSkt);
        pTftpDcpt->uSkt = INVALID_UDP_SOCKET;
    }

    if(gTftpsSignalHandle != NULL)
    {
        (void)TCPIPStackSignalHandlerDeregister(gTftpsSignalHandle);
        gTftpsSignalHandle = NULL;
    }
    
    for(clientCnt = 0; clientCnt < (uint8_t)TCPIP_TFTPS_CLIENT_NUMBER; clientCnt++)
    {
        pClientCB = &gTftpClientCB[clientCnt];
        F_TFTPS_ShellDeRegister(pClientCB);
    }
    
    // Remove TFTP client register users
#if (M_TFTPS_USER_NOTIFICATION != 0)
    TCPIP_Notification_Deinitialize(&pTftpDcpt->tftpsRegisteredUsers, pTftpDcpt->memH);
#endif  // (M_TFTPS_USER_NOTIFICATION != 0)

}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_TFTPS_Deinitialize (const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN) // interface down

    if(gTftpsInitCount > 0)
    {   // we're up and running
        // one way or another this interface is going down
        if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--gTftpsInitCount == 0)
            {   // all closed
                // release resources
                TCPIP_TFTPS_Cleanup();
            }
        }
    }
}


#endif

/*
 * This API is used to return if the TFTP server is running or not.
 */
bool TCPIP_TFTPS_IsEnabled(void)
{
    TCPIP_TFTPS_DCPT     *pTftpDcpt=NULL;
    pTftpDcpt = &gTftpsDcpt;
    if(pTftpDcpt->uSkt == INVALID_UDP_SOCKET)
    {
        return false;
    }
    return true;
}

/*
 * This API is used to return if the number of TFTP client is running for the file access.
 * This will return the number of client sockets which are running and the status is not 
 * TFTPS_CB_FREE. 
 */
uint8_t TCPIP_TFTPS_ClientsNumber(void)
{
    TFTPS_CB  *pClientCB=NULL;
    uint8_t  clientCnt;
    uint8_t  clientIsRunning = 0;
    
    pClientCB = gTftpClientCB;
    for(clientCnt = 0; clientCnt < (uint8_t)TCPIP_TFTPS_CLIENT_NUMBER; clientCnt++)
    {
        if(pClientCB->status != TFTPS_CB_FREE)
        {
            clientIsRunning++;
        }
        pClientCB++;
    }
    return clientIsRunning;
}

/*
 * TFTP server task
 */
void TCPIP_TFTPS_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;
    
    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != TCPIP_MODULE_SIGNAL_NONE)
    { // TMO or RX signal occurred
        TCPIP_TFTPS_Process();
    }
}


/*
 * TFTP server process
 */
static void TCPIP_TFTPS_Process(void)
{    
    TFTPS_CB*   tftp_con;
    uint8_t     clientCnt;
    

     // loop over all the clients in the global array of clients.
    //step 1. TFTP server stack  is using a array of client blocks and that is gTftpClientCB . 
    // One of the client block is selected from the status properties.
    // If the TFTP client block status is TFTPS_CB_FREE, then client is not yet used 
    //and it has Invalid socket value
    //step 2. if client control block is  NULL , then look into the TCPIP_UDP_GetIsReady()
    // for server socket from F_TFTPS_RRecv(), if there is any client request received 
    // by the server socket and it is different from the existing client 
    // and the incoming client request will be compared with exiting port number and client IP address.
    // and new client block will be created from the global array gTftpClientCB[] 
    // if the above condition is passed and also the number of client block should
    // be less than the value of TCPIP_TFTPS_CLIENT_NUMBER.

    for(clientCnt = 0; clientCnt < (uint8_t)TCPIP_TFTPS_CLIENT_NUMBER; clientCnt++)
    {
        tftp_con = F_TFTPS_GetClientCB(clientCnt);
        if(tftp_con == NULL)
        {
            continue;
        }
        if((size_t)tftp_con->smState < sizeof(T_TFTPSStateTbl) / sizeof(*T_TFTPSStateTbl))
        {
            F_TFTPS_StateFnc stateFnc = (T_TFTPSStateTbl + tftp_con->smState)->tftpServStateFnc;
            if(stateFnc != NULL)
            {
                (void)(*stateFnc)(tftp_con);
            }
        }
        else
        {   // wrong internal state!
            // if the client block is reached in a undefined state, then release that client .
             F_TFTPS_ReleaseResource(tftp_con);
        }        
    }
} /* end TFTP_Server_Task */


/*
 * Validate Remote port and client IP address , the packet should not be from the same source
 * This check is required in a multi client situation and 
 * when a client tries to connect server with the same client port which is already in process
 */
static TCPIP_TFTPS_RESULT F_TFTPS_validateClientRquest(UDP_SOCKET_INFO *sktInfo,TFTPS_CB *pClientCB)
{
    // check the local port it should be for the TFTP server port Number 69
    // Then only find the FREE client Free Block.
    if(sktInfo->localPort != (UDP_PORT)TCPIP_TFTP_SERVER_PORT)
    {
        return TFTPS_RES_RECEIVE_ERROR;
    }
#if defined (TCPIP_STACK_USE_IPV6)
    if(sktInfo->addressType == IP_ADDRESS_TYPE_IPV6)
    {
        if ((memcmp (sktInfo->localIPaddress.v6Add.v, pClientCB->local_addr.v6Add.v, sizeof (IPV6_ADDR)) == 0) 
                        && (sktInfo->remotePort == pClientCB->tid))
        {
            return TFTPS_RES_RCVD_FROM_SAME_CLIENT;
        }
    }
#endif 
#if defined (TCPIP_STACK_USE_IPV4)    
    if(sktInfo->addressType == IP_ADDRESS_TYPE_IPV4)
    {
        if ((memcmp (sktInfo->localIPaddress.v4Add.v, pClientCB->local_addr.v4Add.v, sizeof (IPV4_ADDR)) == 0) 
                        && (sktInfo->remotePort == pClientCB->tid))
        {
            return TFTPS_RES_RCVD_FROM_SAME_CLIENT;
        }
    }
#endif 
    return TFTPS_RES_OK;
}

// Get free TFTP server control block
static TFTPS_CB* F_TFTPS_GetFreeTFTPSCB(UDP_SOCKET_INFO *sktInfo)
{
    TFTPS_CB* pClientCB;
    uint8_t client_cnt;
    
    pClientCB = gTftpClientCB;
    for(client_cnt = 0; client_cnt < (uint8_t)TCPIP_TFTPS_CLIENT_NUMBER; client_cnt++)
    {
        if(pClientCB->status != TFTPS_CB_FREE)
        {
            // validate the TFTP client request 
            if(F_TFTPS_validateClientRquest(sktInfo,pClientCB) != TFTPS_RES_OK)
            {
                // Notify User client
                F_TFTPSNotifyUserClients(pClientCB,TCPIP_TFTPS_EVENT_SAME_REQUEST_RCVD);
                return NULL;
            }
            pClientCB++;
            continue;
        }
        (void)memset(pClientCB,0,sizeof(TFTPS_CB));
        /* initialize the buffer size fields of the TFTP CB. */
        pClientCB->options.tsize = TCPIP_TFTPS_DEFAULT_FILESIZE;
        pClientCB->options.blksize = TCPIP_TFTPS_DEFAULT_BLOCK_SIZE;
        pClientCB->options.timeout = (uint16_t)TCPIP_TFTPS_DEFAULT_TIMEOUT;
        pClientCB->retransmits = 0U;
        pClientCB->lastTxPktSize = 0;
        pClientCB->status = TFTPS_READY_TO_SENDRECEIVE;
        pClientCB->type =  TFTPS_TYPE_NONE;
        pClientCB->cSkt = INVALID_UDP_SOCKET;
        pClientCB->file_desc = SYS_FS_HANDLE_INVALID;
        return pClientCB;
    }
    return NULL;
}

/*
 * Get the Client control block
 */
static TFTPS_CB  *F_TFTPS_GetClientCB(uint8_t client_cnt)
{
    TFTPS_CB* pClientCB;
    uint16_t  byteCnt;
    
    if(client_cnt >= (uint8_t)TCPIP_TFTPS_CLIENT_NUMBER)
    {
        return NULL;
    }
    pClientCB = &gTftpClientCB[client_cnt];
//  check if the client block has a valid socket value and 
//  the status flag for this client block should not be free (TFTPS_CB_FREE))
    if((pClientCB->status != TFTPS_CB_FREE)&&(pClientCB->cSkt != INVALID_UDP_SOCKET))
    {
        byteCnt = TCPIP_UDP_PutIsReady(pClientCB->cSkt);
        if( byteCnt < (pClientCB->options.blksize + (uint16_t)TCPIP_TFTP_HEADER_MINSIZE))
        {
            if(!TCPIP_UDP_OptionsSet(pClientCB->cSkt, UDP_OPTION_TX_BUFF, FC_Uint2VPtr((uint32_t)pClientCB->options.blksize + (uint32_t)TCPIP_TFTP_HEADER_MINSIZE)))
            {
                return NULL;
            }           
        }
         //Get the write pointer:
        pClientCB->trans_buf = TCPIP_UDP_TxPointerGet(pClientCB->cSkt);
        if(pClientCB->trans_buf == NULL)
        {
            return NULL; 
        }
    }
    else
    {
        pClientCB->smState = (uint8_t)SM_TFTPS_RRECV;
    }

    return pClientCB;
}

// Release the client communication socket and free the data base.
static void F_TFTPS_ReleaseResource(TFTPS_CB   *tftp_con)
{
    if(tftp_con->cSkt != INVALID_UDP_SOCKET)
    {
        (void)TCPIP_UDP_Close(tftp_con->cSkt);
        tftp_con->cSkt = INVALID_UDP_SOCKET;
    }
    
    
    tftp_con->status = TFTPS_CB_FREE;
    tftp_con->retransmits = 0U;
    if(tftp_con->file_desc != SYS_FS_HANDLE_INVALID)
    {
        (void)tftp_con->tftps_shell_obj->fileClose(tftp_con->tftps_shell_obj,tftp_con->file_desc);
        tftp_con->file_desc = SYS_FS_HANDLE_INVALID;
    }
    tftp_con->callbackPos = 0;
    tftp_con->trans_buf = NULL;
    
    // Release TFTP server Shell registered Users
    F_TFTPS_ShellDeRegister(tftp_con);
}

// create the Client socket for data communication after TFTP client request accepted.
static bool F_TFTPS_CreateClientSocket(TFTPS_CB   *tftp_con,UDP_SOCKET_INFO *sktInfo,TCPIP_TFTPS_DCPT *pTftpDcpt)
{
    uint16_t    txBufSize=0;
    // create a client socket for further communication
    tftp_con->cSkt = TCPIP_UDP_ClientOpen(sktInfo->addressType, sktInfo->remotePort,&sktInfo->sourceIPaddress);
    if(tftp_con->cSkt == INVALID_UDP_SOCKET)
    {
        return false;
    }
    if(sktInfo->addressType == IP_ADDRESS_TYPE_IPV4)
    {
        tftp_con->local_addr.v4Add.Val = sktInfo->localIPaddress.v4Add.Val;
    }
    else
    {
        (void)memcpy(&tftp_con->local_addr.v6Add,&sktInfo->localIPaddress.v6Add,sizeof(IPV6_ADDR));
    }
    // signal handler for UDP RX data for the socket
    tftp_con->cSigHandlr = TCPIP_UDP_SignalHandlerRegister(tftp_con->cSkt, TCPIP_UDP_SIGNAL_RX_DATA, &F_TFTPSSocketRxSignalHandler, NULL);
    if( tftp_con->cSigHandlr == NULL)
    {
        return false;
    }
    (void)TCPIP_UDP_OptionsSet(tftp_con->cSkt, UDP_OPTION_STRICT_ADDRESS, FC_Uint2VPtr(0UL));
    (void)TCPIP_UDP_OptionsSet(tftp_con->cSkt, UDP_OPTION_STRICT_PORT, FC_Uint2VPtr(0UL));
    (void)TCPIP_UDP_SocketNetSet(tftp_con->cSkt,pTftpDcpt->prefNet);
    txBufSize = tftp_con->options.blksize+TCPIP_TFTP_HEADER_MINSIZE;
    if(TCPIP_UDP_PutIsReady(tftp_con->cSkt) < txBufSize)
    {
        if(!TCPIP_UDP_OptionsSet(tftp_con->cSkt, UDP_OPTION_TX_BUFF, FC_Uint2VPtr((uint32_t)txBufSize)))
        {
            return 0;
        }           
    }
    // setup the TX buffer pointer
    // this will put the start pointer at the beginning of the TX buffer
    (void)TCPIP_UDP_TxOffsetSet(tftp_con->cSkt, 0U, false);    

    // Get the write pointer:
    tftp_con->trans_buf = TCPIP_UDP_TxPointerGet(tftp_con->cSkt);
    
    return true;
}

/*
 * validate the TFTP server READ request when Client control block is not NULL.
 * READ request will be processed, with respect to the client block status value.
 * if the status == TFTPS_WAITINGFOR_OPTIONACK, then Server is waiting for a acknowledge packet from client
 * to send the first data packet with block size value of 0. 
 * 
 * if status == TFTPS_PROCESS_INPROGRESS - wait for the  valid ACK packet , if it is not received, then send the 
 * Retransmit previous packet for TCPIP_TFTPS_RETRANSMIT_COUNT (3) times.
 */
static TCPIP_TFTPS_RESULT F_TFTPS_RRecv_ReadAccess(TFTPS_CB  *tftp_con,uint16_t nBytes)
{
    TCPIP_TFTPS_RESULT res = TFTPS_RES_OK;
        
    /* Process Option acknowledge before transmitting any data packet*/
    if(tftp_con->status == TFTPS_WAITINGFOR_OPTIONACK)
    {     
        // check if the option flag is false , else 
        // client need to send the Option ack packet to the client
        if((tftp_con->options.tftpoptionAckflag.bits.blksize_ack == 1U) ||
        (tftp_con->options.tftpoptionAckflag.bits.tsize_ack == 1U) ||
        (tftp_con->options.tftpoptionAckflag.bits.timeout_ack == 1U))
        {
            (void)F_TFTPS_Ack(tftp_con);
        }
        else
        {
            if(nBytes  >= TCPIP_TFTP_HEADER_MINSIZE)
            {
                (void)F_TFTPS_Process_Ack(tftp_con,nBytes);
            }
            tftp_con->status = TFTPS_TRANSFERRING_FILE;
        }
    } /* if the READ request in progress , that is at-least one packet 
       * has been sent out to client from server */
    else if((tftp_con->status == TFTPS_PROCESS_INPROGRESS)|| 
                (tftp_con->status == TFTPS_TRANSFER_COMPLETE))
    {   
        if(nBytes  < TCPIP_TFTP_HEADER_MINSIZE)
        {
            if(tftp_con->retransmits == (uint8_t)TCPIP_TFTPS_RETRANSMIT_COUNT)
            {
                res = TFTPS_RES_CLIENT_ERROR;
            }
            else
            {
                (void)F_TFTPS_Retransmit(tftp_con);
                res = TFTPS_RES_RECEIVE_ERROR;
            }
        }
        else
        {// close the socket if the transfer is completed and 
            // call process ACk to check if the packet has ACK or not
            if(tftp_con->status == TFTPS_TRANSFER_COMPLETE)
            {
                res = F_TFTPS_Process_Ack(tftp_con,nBytes);
            }
            else
            {
                tftp_con->status = TFTPS_TRANSFERRING_FILE;
            }
        }
    }
    else
    {
        // do nothing
    }
    
    return res;
}

/*
 * validate the TFTP server Write request when Client control block is not NULL.
 * After Receiving the a block of data, server should send the ACK for the Write request,
 * with respect to the client block status value.
 * Release the client socket if the retransmit count is equal to TCPIP_TFTPS_RETRANSMIT_COUNT (3)
 * if the status == TFTPS_PROCESS_INPROGRESS, then Server will send the ACK packet for the successful process of 
 * write request.
 *  
 */
static TCPIP_TFTPS_RESULT F_TFTPS_RRecv_WriteAccess(TFTPS_CB  *tftp_con,uint16_t nBytes)
{
    TCPIP_TFTPS_RESULT  res=TFTPS_RES_OK;
    
    // release the socket if the retransmit count is equal to TCPIP_TFTPS_RETRANSMIT_COUNT
    if(tftp_con->retransmits == (uint8_t)TCPIP_TFTPS_RETRANSMIT_COUNT)
    {
        if(nBytes == 0U)
        {
            F_TFTPS_ReleaseResource(tftp_con);
        }
        return res;
    }
    if(tftp_con->status == TFTPS_PROCESS_INPROGRESS)
    {                  
        /* Acknowledge Write Request Packet */
        res = F_TFTPS_Ack(tftp_con);
        if(res != TFTPS_RES_OK)
        {
            tftp_con->status = TFTPS_PROCESS_INPROGRESS;
            return res; 
        }
        tftp_con->status = TFTPS_TRANSFERRING_FILE;
    }
    
    return res;
}

/************************************************************************
*   Function:                                                             
*       TFTPS_RRecv                                                      
*   Description:                                                          
*       This function is responsible for receiving a TFTP request from a 
*       TFTP Client.                                                   
*   Parameters:                                                                                                                                  
*       tftp_con        The pointer to TFTP Control Block                        
*   Outputs:                                                               
*       The number of bytes received when successful.                   
************************************************************************/
static TCPIP_TFTPS_RESULT F_TFTPS_RRecv(TFTPS_CB** tftp_con, uint16_t *byteReceived)
{
    uint16_t            nBytes=0;
    UDP_SOCKET_INFO     sktInfo;
    TCPIP_TFTPS_DCPT    *pTftpDcpt=NULL;    
    TFTPS_CB            *pTftpCon=*tftp_con;
    TCPIP_TFTPS_RESULT  res=TFTPS_RES_OK;
    
    pTftpDcpt = &gTftpsDcpt;
    // If the Control block status is TFTPS_CB_FREE, check if there is any request from the client.
    if(pTftpCon->status == TFTPS_CB_FREE)
    { // new Client request is received.
        // Create a client socket for this request for next communications.
        nBytes = TCPIP_UDP_GetIsReady(pTftpDcpt->uSkt);
        if(nBytes < TCPIP_TFTP_HEADER_MINSIZE)
        {
            return TFTPS_RES_IS_NOT_READY;
        }
        // check if the packet request came from a valid interface type
        (void)memset(&sktInfo, 0, sizeof(sktInfo));
        (void)TCPIP_UDP_SocketInfoGet(pTftpDcpt->uSkt, &sktInfo);
        
        //allocate one TFTP server control block
        pTftpCon = F_TFTPS_GetFreeTFTPSCB(&sktInfo);
        if(pTftpCon == NULL)
        {// There is no space for the new TFTP client control block data
            (void)TCPIP_UDP_Discard(pTftpDcpt->uSkt);
            return TFTPS_RES_NO_CLIENT_CB;
        }
        // update the source port number
        pTftpCon->tid = sktInfo.remotePort;
        if(F_TFTPS_CreateClientSocket(pTftpCon,&sktInfo,pTftpDcpt) != true)
        {
            // release the client block, as client socket failed while opening.
            F_TFTPS_ReleaseResource(pTftpCon);
            return TFTPS_RES_SKT_ERR;
        }
        if(F_TFTPS_ShellRegister(pTftpCon) != true)
        {
            // release the client block, as client socket failed while opening.
            F_TFTPS_ReleaseResource(pTftpCon);
            return TFTPS_RES_MEMORY_ALLOC_ERR;
        }
        *tftp_con = pTftpCon;
    }
    else if(pTftpCon->trans_buf == NULL)
    {
        return TFTPS_RES_MEMORY_ALLOC_ERR;
    }
    else
    {
        // read the ACK and update the status to transferring file
        nBytes = TCPIP_UDP_GetIsReady(pTftpCon->cSkt);
        switch(pTftpCon->type)
        {
            case TFTPS_READ_TYPE:
                res = F_TFTPS_RRecv_ReadAccess(pTftpCon,nBytes);                
                break;
            case TFTPS_WRITE_TYPE:                
                res = F_TFTPS_RRecv_WriteAccess(pTftpCon,nBytes);
                break;
            default:
                // should not get here
                TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
                res = TFTPS_RES_ILLIGAL_OPERN; 
                break;
        }        
    }
    *byteReceived = nBytes;
    return res;
    
} /* end TFTPS_RRecv */
  
/************************************************************************
*   Function                                                              
*       F_TFTPS_Process_Data
*                                          
*   Description                                                           
*       This function is responsible for processing a data packet        
*       whenever a read request is in progress.                          
*                                                                       
*   Parameters                                                                
*       tftp_con        The pointer to TFTP Control Block                        
*       bytes_received  Number of bytes in the packet
*                                                                       
*   Returns                                                               
*       TFTPS_RES_OK - for successful Data process
*       TFTPS_RES_ILLIGAL_OPERN - illegal operation
************************************************************************/
static TCPIP_TFTPS_RESULT F_TFTPS_Process_Data(TFTPS_CB *tftp_con, uint16_t bytes_received)
{
    UDP_SOCKET_INFO sktInfo;
    TCPIP_UINT16_VAL tOpcode,blockNum;
    uint8_t* pBuff;
    uint16_t wCnt;
    uint16_t maxRecvByte = bytes_received;
    union
    {
        uint8_t     u8Buff[TCPIP_TFTPS_MIN_UDP_TX_BUFFER_SIZE];
        uint16_t    u16Buff[(TCPIP_TFTPS_MIN_UDP_TX_BUFFER_SIZE + 1U) / 2U];       // enforce alignment
    }rxBuf;

    uint16_t    bufferSize = (uint16_t)sizeof(rxBuf.u8Buff);
    
    (void)memset(rxBuf.u8Buff, 0, (size_t)bufferSize);
    (void)memset(&sktInfo, 0, sizeof(sktInfo));
    // get the received packet. Not require to check again the byte count. 
    // This has been already checked from the called function.
    // first packet
    
    wCnt = TCPIP_UDP_ArrayGet(tftp_con->cSkt, rxBuf.u8Buff, FC_MinU16(maxRecvByte, bufferSize));
    // get the socket info.
    (void)TCPIP_UDP_SocketInfoGet(tftp_con->cSkt, &sktInfo);
   
    tOpcode.Val = F_TFTPS_Get16(rxBuf.u8Buff, (unsigned int)TFTP_OPCODE_OFFSET);    

    TCPIP_TFTPS_RESULT res = TFTPS_RES_OK; 
    switch(tOpcode.Val)
    {
        case TFTPS_DATA_OPCODE:

        /* If the current block number is greater than the block number
        * of the ACK packet we are receiving, then this is an old
        * packet and we have already processed it - we do not want to
        * exit, error or acknowledge the packet (because we have 
        * already processed it) so we will get the next packet
        */
            blockNum.Val = F_TFTPS_Get16(rxBuf.u8Buff, (unsigned int)TFTP_DATA_BLOCKNUM_OFFSET);
            if((tftp_con->block_number > blockNum.Val) && (tftp_con->tid == sktInfo.remotePort))
            {
               break;
            }
        /* If data was received make sure that block number and TID are
        * correct. 
        */
            if((tftp_con->block_number == blockNum.Val) && (tftp_con->tid == sktInfo.remotePort))
            {
                // this is to skip the TFTP data opcode and block number
                // And Data starts from the TFTP_DATA_OFFSET

                pBuff = rxBuf.u8Buff + (unsigned int)TFTP_DATA_OFFSET;
                /* Calculate the amount of data in the packet. */
                wCnt = wCnt - TCPIP_TFTP_HEADER_MINSIZE;
                (void)tftp_con->tftps_shell_obj->fileSeek(tftp_con->tftps_shell_obj, tftp_con->file_desc, tftp_con->callbackPos, SYS_FS_SEEK_SET);
                maxRecvByte = bytes_received - (uint16_t)TFTP_DATA_OFFSET;
                while(maxRecvByte > 0U)
                {
                    if(wCnt != 0U)
                    {                        
                        if(tftp_con->tftps_shell_obj->fileWrite(tftp_con->tftps_shell_obj, tftp_con->file_desc, pBuff, (size_t)wCnt) == SYS_FS_HANDLE_INVALID) 
                        {                                              
                            break;
                        }
                        if(wCnt <= maxRecvByte)
                        {
                            maxRecvByte -= wCnt;
                        }
                        (void)memset(rxBuf.u8Buff, 0, sizeof(rxBuf.u8Buff));
                        if(maxRecvByte > 0U)
                        {
                            wCnt = TCPIP_UDP_ArrayGet(tftp_con->cSkt, rxBuf.u8Buff, FC_MinU16(maxRecvByte, (bufferSize - (uint16_t)TCPIP_TFTP_HEADER_MINSIZE)));
                        }
                        tftp_con->callbackPos = tftp_con->tftps_shell_obj->fileTell(tftp_con->tftps_shell_obj,tftp_con->file_desc);
                        pBuff = rxBuf.u8Buff;
                        // send the error packet if SYS_FS_FileTell returns -1
                        if(tftp_con->callbackPos == -1)
                        {
                            (void)F_TFTPS_Error(tftp_con->cSkt,TFTP_ALLOCATION_ERROR,"Error: No space available for the new data");
                        }
                    }
                }

                /* If all the bytes of data was copied then send an ACK.  We know
                 * that the other side will send at least one more data packet,
                 * and that all data in the current packet was accepted. 
                 */
                if(bytes_received == (tftp_con->options.blksize+TCPIP_TFTP_HEADER_MINSIZE))
                {
                    (void)F_TFTPS_Ack(tftp_con);
                }
                /* Else the last data packet has been received.  We are done.
                 * Send the last ack. 
                 */
                else
                {
                    (void)F_TFTPS_Ack(tftp_con);
                   
                    tftp_con->status = TFTPS_TRANSFER_COMPLETE;                            
                    F_TFTPS_ReleaseResource(tftp_con);                    
                }        

                /* Initialize retransmits. */
                tftp_con->retransmits = 0U;
            }

            else
            {
                (void)F_TFTPS_Error(tftp_con->cSkt, TFTP_UNKNOWN_ID_ERROR, "Error: Unknown Transfer ID");
                res = TFTPS_RES_ILLIGAL_OPERN;
            }
            break;
        case TFTPS_ERROR_OPCODE:
            tftp_con->errCode = (uint8_t)F_TFTPS_Get16(rxBuf.u8Buff,  (unsigned int)TFTP_ERROR_VAL_OFFSET);
            if ( tftp_con->errCode <= (uint8_t)TFTP_BAD_TFTP_ERROR)
            {                 
                res = TFTPS_RES_CLIENT_ERROR;
            }        
            else
            {
                res = TFTPS_RES_ILLIGAL_OPERN;
            }
            break;
        case TFTPS_RRQ_OPCODE:
        case TFTPS_WRQ_OPCODE:
        case TFTPS_ACK_OPCODE:
        default:
            (void)F_TFTPS_Error(tftp_con->cSkt, TFTP_ILLIGAL_OPERATION_ERROR, "Error: Illegal TFTP Operation");
            res = TFTPS_RES_ILLIGAL_OPERN;
            break;
    }
     
    return res;
}  /* TFTPS_Process_Data */
  
/*
 * TFTP server will process the READ option request from the client.
 */
static TCPIP_TFTPS_RESULT F_TFTPS_Process_ReadReqPacket(TFTPS_CB *tftp_con, uint16_t bytes_received, uint8_t *rxBuf)
{
    char *p = NULL;
    uint16_t   count = 2U, count1 = 0U;   
    char    temp[TCPIP_TFTPS_FILENAME_LEN+1];    
    TCPIP_TFTPS_RESULT  res = TFTPS_RES_OK;
    SYS_FS_HANDLE file_desc;    /* File Descriptor for in-memory file system */
    
    
    // count variable is used to number of bytes processed
    (void)memset(temp,0,sizeof(temp));
    if(bytes_received == 0U)
    {
        return TFTPS_RES_RECEIVE_ERROR;
    }
        
     /*  get the Filename */
    p = (char*)rxBuf + (unsigned int)TFTP_FILENAME_OFFSET;
    while ((*p != '\0') && (count1 < (uint16_t)TCPIP_TFTPS_FILENAME_LEN))
    {
        temp[count1] = *p;
        count ++;
        count1 ++;
        p++;
    }
    if(*p != '\0')
    {
        (void)F_TFTPS_Error(tftp_con->cSkt, TFTP_FILE_NOT_FOUND_ERROR, "Error: File length is exceeds the Max size");
        return TFTPS_RES_FILE_LENGTH_ERROR;        
    }

    temp[count1] = '\0';
    (void)strncpy((char*)tftp_con->file_name, temp, sizeof(tftp_con->file_name) - 1U);
    tftp_con->file_name[sizeof(tftp_con->file_name) - 1U] = 0;
    // increase the count value for the null terminated value
    count++;
    count1 = 0;

    /* Get the transfer mode.  Only Octet supported at this time. */
    p = (char*)rxBuf + count;
    while ((*p != '\0') && 
        (count1 < TCPIP_TFTP_OCTET_SIZE))
    {
        temp[count1] = *p;
        count ++;
        count1 ++;
        p++;
    }

    temp[count1]='\0';
    (void)strncpy((char*)tftp_con->mode, temp, sizeof(tftp_con->mode) - 1U);
    tftp_con->mode[sizeof(tftp_con->mode) - 1U] = 0;
    if(strcmp((char*)tftp_con->mode, TCPIP_TFTP_TRANSFERMODE_OCTET) != 0)
    {
        (void)F_TFTPS_Error(tftp_con->cSkt, TFTP_ERROR, "Error: transfer mode is not defined");
        tftp_con->type = TFTPS_TYPE_NONE;
        return TFTPS_RES_TRANSFERMODE_NOT_SUPPORTED;
    }
    count++;

    file_desc = tftp_con->tftps_shell_obj->fileOpen(tftp_con->tftps_shell_obj,(char*)tftp_con->file_name,SYS_FS_FILE_OPEN_READ);
    if (file_desc == SYS_FS_HANDLE_INVALID)
    {
        (void)F_TFTPS_Error(tftp_con->cSkt, TFTP_FILE_NOT_FOUND_ERROR, "Error: File not Found");
        tftp_con->type =TFTPS_TYPE_NONE;
        return TFTPS_RES_FILE_NOT_FOUND_ERROR;
    }         
    tftp_con->file_desc = file_desc;
    // create a client data socket
    /* If the client is RFC 2347 compliant */
    if (count != bytes_received)
    {
        res = F_TFTPS_Check_Options(tftp_con, bytes_received, count,rxBuf);

        /*  Set Block Number to 0  */
        tftp_con->block_number = 0;

        /* Acknowledge Read request Packet */
        if(res == TFTPS_RES_OK)
        {
            if(F_TFTPS_Ack(tftp_con)!= TFTPS_RES_OK)
            {
                tftp_con->smState = (uint8_t)SM_TFTPS_RRECV;
            }
        }
        /* Set whether a Read or Write from the Server.  */
        tftp_con->status = TFTPS_WAITINGFOR_OPTIONACK;
    }

    /* Else, the client is not compliant, and block number should
     * begin at 1
     */
    else
    {
        tftp_con->block_number = 0;
        tftp_con->status = TFTPS_TRANSFERRING_FILE;
        tftp_con->smState = (uint8_t)SM_TFTPS_SEND_DATA;
    }            
    tftp_con->type = TFTPS_READ_TYPE;
    
    return res;
}

/*
 * TFTP server will process the WRITE option request from the client.
 */
static TCPIP_TFTPS_RESULT F_TFTPS_Process_WriteReqPacket(TFTPS_CB *tftp_con, uint16_t bytes_received, uint8_t *rxBuf)
{
    char* p;
    uint16_t   count = 2, count1 = 0;   
    char    temp[TCPIP_TFTPS_FILENAME_LEN+1];
    TCPIP_TFTPS_RESULT  res = TFTPS_RES_OK;
    SYS_FS_HANDLE file_desc;    /* File Descriptor for in-memory file system */
    
     /* Get the Filename */
    p = (char*)rxBuf + (unsigned int)TFTP_FILENAME_OFFSET;
    while ((*p != '\0') && 
        (count1 < (uint16_t)TCPIP_TFTPS_FILENAME_LEN))
    {
        temp[count1] = *p;
        count ++;
        count1 ++;
        p++;
    }
    if(*p != '\0')
    {
        (void)F_TFTPS_Error(tftp_con->cSkt, TFTP_FILE_NOT_FOUND_ERROR, "Error: File length is exceeds the Max size");
        return TFTPS_RES_FILE_LENGTH_ERROR;        
    }
    temp[count1] = '\0';
    (void)strncpy((char*)tftp_con->file_name, temp, sizeof(tftp_con->file_name) - 1U);
    tftp_con->file_name[sizeof(tftp_con->file_name) - 1U] = 0;

    // increase the count value for the null terminated value
    count++;
    count1 = 0;

    /* Get the transfer mode.  Only Octet supported at this time. */
    p = (char*)rxBuf + count;
    while ((*p != '\0') && 
        (count1 < (uint16_t)TCPIP_TFTP_OCTET_SIZE))
    {
        temp[count1] = *p;
        count ++;
        count1 ++;
        p++;
    }

    temp[count1]='\0';
    (void)strncpy((char*)tftp_con->mode, temp, sizeof(tftp_con->mode) - 1U);
    tftp_con->mode[sizeof(tftp_con->mode) - 1U] = 0;

    if(strcmp((char*)tftp_con->mode, TCPIP_TFTP_TRANSFERMODE_OCTET) != 0)
    {
        (void)F_TFTPS_Error(tftp_con->cSkt, TFTP_ERROR, "Error: transfer mode is not defined");
        tftp_con->type = TFTPS_TYPE_NONE;
        return TFTPS_RES_TRANSFERMODE_NOT_SUPPORTED;
    }
    count++;

    /*  Retrieve File_Descriptor */
    file_desc = tftp_con->tftps_shell_obj->fileOpen(tftp_con->tftps_shell_obj,(char*)tftp_con->file_name,SYS_FS_FILE_OPEN_WRITE);
    if (file_desc == SYS_FS_HANDLE_INVALID)
    {
        (void)F_TFTPS_Error(tftp_con->cSkt, TFTP_FILE_NOT_FOUND_ERROR, "Error: File not Found");
        tftp_con->type = TFTPS_TYPE_NONE;
        return TFTPS_RES_FILE_NOT_FOUND_ERROR;
    }         
    tftp_con->file_desc = file_desc;

     /* If there is more left in the packet, then the client is
     * RFC 2347 compliant and requesting options for us to evaluate
     */
    if (count != bytes_received)
    {
        res = F_TFTPS_Check_Options(tftp_con, bytes_received, count,rxBuf);

        if (res == TFTPS_RES_MEMORY_ALLOC_ERR)
        {
            (void)F_TFTPS_Error(tftp_con->cSkt, TFTP_ALLOCATION_ERROR, "Error: Not Enough Memory");
        }
    }

    tftp_con->block_number = 0;
    tftp_con->type = TFTPS_WRITE_TYPE; 

    /* Acknowledge Write Request Packet */
    res = F_TFTPS_Ack(tftp_con);
    if(res != TFTPS_RES_OK)
    {
        tftp_con->status = TFTPS_PROCESS_INPROGRESS;
        return res; 
    }

    /*  Set the Status to transferring file */
    tftp_con->status = TFTPS_TRANSFERRING_FILE;

    return res;
}

/************************************************************************
*   Function                                                              
*       F_TFTPS_Process_RequestPacket                                     
*   Description                                                           
*       This function is responsible processing a request packet whenever
*       a TFTP Client sends one.                                         
*   Inputs                                                                
*       tftp_con        The pointer to TFTP Control Block                        
*       bytes_received  Number of bytes in the packet
*   Returns                                                               
*       TFTPS_RES_CONN_ERR          Error has occurred
*       TFTPS_RES_RECEIVE_ERROR     Receive error
************************************************************************/
static TCPIP_TFTPS_RESULT F_TFTPS_Process_RequestPacket(TFTPS_CB *tftp_con, uint16_t bytes_received)
{
    TCPIP_UINT16_VAL tOpcode;
    TCPIP_TFTPS_DCPT* pTftpDcpt;
    TCPIP_TFTPS_RESULT res;
    
    union
    {
        uint8_t     u8Buff[TCPIP_TFTPS_MIN_UDP_RX_BUFFER_SIZE + 1U];
        uint16_t    u16Buff[(TCPIP_TFTPS_MIN_UDP_RX_BUFFER_SIZE + 2U) / 2U];       // enforce alignment
    }rxBuf;

    pTftpDcpt = &gTftpsDcpt;
    // count variable is used to number of bytes processed
    
    (void)memset(rxBuf.u8Buff, 0, sizeof(rxBuf.u8Buff));
   
    if(bytes_received == 0U)
    {
        bytes_received = TCPIP_UDP_GetIsReady(pTftpDcpt->uSkt);
        if(bytes_received == 0U)
        {
            return TFTPS_RES_RECEIVE_ERROR;
        }
    }
    
   // check the length and truncate to the rxBuf length if the 
    // rxBuf size is less than the length of the received bytes.
    if(bytes_received > (uint16_t)sizeof(rxBuf.u8Buff))
    {
        bytes_received =  (uint16_t)sizeof(rxBuf.u8Buff);
    }
    (void)TCPIP_UDP_ArrayGet(pTftpDcpt->uSkt, rxBuf.u8Buff, bytes_received);
      
    // opcode value
    tOpcode.Val = F_TFTPS_Get16(rxBuf.u8Buff, (unsigned int)TFTP_OPCODE_OFFSET);
    
    /* What kind of packet is this. */
    bool doReturn = false;
    switch(tOpcode.Val)
    {
        case TFTPS_RRQ_OPCODE:
            res = F_TFTPS_Process_ReadReqPacket(tftp_con, bytes_received, rxBuf.u8Buff);
            break;
            
        case TFTPS_WRQ_OPCODE: 
            res = F_TFTPS_Process_WriteReqPacket(tftp_con, bytes_received, rxBuf.u8Buff);
            break;

        case TFTPS_ACK_OPCODE:
        case TFTPS_ERROR_OPCODE:
        case TFTPS_DATA_OPCODE:
        default:
            (void)F_TFTPS_Error(tftp_con->cSkt, TFTP_ILLIGAL_OPERATION_ERROR, "Error: Illegal TFTP Operation");
            res = TFTPS_RES_CONN_ERR;
            doReturn = true;
            break;
    }

    if(doReturn == false)
    {
        if(tftp_con->status != TFTPS_PROCESS_INPROGRESS)
        {
            if(res != TFTPS_RES_OK)
            {
                // discard the server socket received packet
                (void)TCPIP_UDP_Discard(pTftpDcpt->uSkt);
                // close client socket ?
                F_TFTPS_ReleaseResource(tftp_con);
            }
        }
    }
    
    return (res);
}  /* TFTPS_Process_Request_PACKET */

/************************************************************************
*   Function                                                              
*       F_TFTPS_Ack                                                        
*   Description                                                           
*       This function is responsible for sending an acknowledgement of   
*       a TFTP data packet.                                              
*   Inputs                                                                
*       tftp_con        The pointer to TFTP Control Block                        
*   Return                                                               
*       The Number of bytes sent on success.                             
************************************************************************/
static TCPIP_TFTPS_RESULT F_TFTPS_Ack(TFTPS_CB *tftp_con)
{
    uint16_t    send_size = 2U;
    char        temp[10];
    TCPIP_TFTPS_RESULT res=TFTPS_RES_OK;
    
    /* If the options acknowledged flag is set, then the client is
    * RFC 2347 compliant, and an OACK must be sent back to the client
    */
    // check the transfer before before sending any ACK.
    if(tftp_con->trans_buf == NULL)
    {
        return TFTPS_RES_MEMORY_ALLOC_ERR;
    }
    if ((tftp_con->options.tftpoptionAckflag.bits.blksize_ack == 1U) ||
        (tftp_con->options.tftpoptionAckflag.bits.tsize_ack == 1U) ||
        (tftp_con->options.tftpoptionAckflag.bits.timeout_ack == 1U))
    {
         /* Setup the OACK packet. */
        F_TFTPS_Put16(tftp_con->trans_buf, 0U, (uint16_t)TFTPS_OACK_OPCODE);
        
        /* Check if there is a blksize, timeout or tsize option indicated, 
         * if so, append the option name and option value to the end of 
         * the packet - all null terminated 
         */   
        if (tftp_con->options.tftpoptionAckflag.bits.blksize_ack == 1U)
        {
            /* send_size += sprintf(&(tftp_con->trans_buf[send_size]), 
            "%s%c%d%c", "blksize", 0, tftp_con->options.blksize, 0); */
            (void)strcpy((char*)&(tftp_con->trans_buf[send_size]), "blksize");
            send_size += (uint16_t)strlen("blksize");
            tftp_con->trans_buf[send_size++] = 0;
            uitoa(tftp_con->options.blksize, temp);
            (void)strcpy((char*)&(tftp_con->trans_buf[send_size]),temp);
            send_size += (uint16_t)strlen(temp);
            tftp_con->trans_buf[send_size++] = 0;            
        }
        
        if (tftp_con->options.tftpoptionAckflag.bits.timeout_ack == 1U)
        {
            /*  send_size += sprintf(&(tftp_con->trans_buf[send_size]), 
            "%s%c%d%c", "timeout", 0, tftp_con->options.timeout, 0); */
            (void)strcpy((char*)&(tftp_con->trans_buf[send_size]), "timeout");
            send_size += (uint16_t)strlen("timeout");
            tftp_con->trans_buf[send_size++] = 0;
            uitoa(tftp_con->options.timeout, temp);
            (void)strcpy((char*)&(tftp_con->trans_buf[send_size]),temp);
            send_size += (uint16_t)strlen(temp);
            tftp_con->trans_buf[send_size++] = 0;            
        }
        
        if (tftp_con->options.tftpoptionAckflag.bits.tsize_ack == 1U)
        {
            /* send_size += sprintf(&(tftp_con->trans_buf[send_size]), 
            "%s%c%ld%c", "tsize", 0, tftp_con->options.tsize, 0);*/
            (void)strcpy((char*)&(tftp_con->trans_buf[send_size]), "tsize");
            send_size += (uint16_t)strlen("tsize");
            tftp_con->trans_buf[send_size++] = 0;
            uitoa((uint16_t)tftp_con->options.tsize, temp);
            (void)strcpy((char*)&(tftp_con->trans_buf[send_size]),temp);
            send_size += (uint16_t)strlen(temp);
            tftp_con->trans_buf[send_size++] = 0;            
        }
        
        /* Set them back to false, because we only send an OACK
         * after the initial request 
         */
        tftp_con->options.tftpoptionAckflag.bits.tsize_ack = 0U;
        tftp_con->options.tftpoptionAckflag.bits.timeout_ack = 0U;
        tftp_con->options.tftpoptionAckflag.bits.blksize_ack = 0U;      
    }    
    else
    {
        /* Setup the ACK packet. */
        F_TFTPS_Put16(tftp_con->trans_buf, 0U, (uint16_t)TFTPS_ACK_OPCODE);
        F_TFTPS_Put16(tftp_con->trans_buf, 2U, (uint16_t)tftp_con->block_number); 
        send_size += 2U; // adding the number of byte count for block_number bytes.
    }
    
    // Put complete TFTP error packet buffer to the UDP buffer
    // Once writing is completed into the buffer, TX offset need to be updated again,
    // because the socket flush function calculates how many bytes are in 
    // the buffer using the current write pointer:
    (void)TCPIP_UDP_TxOffsetSet(tftp_con->cSkt, send_size, false);

    // send ACK or write request
    if(TCPIP_UDP_Flush(tftp_con->cSkt) != send_size)
    {
        res = TFTPS_RES_SKT_ERR;
    }
    tftp_con->lastTxPktSize = send_size;
    /* Increment the block number. */       
    tftp_con->block_number++;
    return res;
} /* end F_TFTPS_Ack */
   
/************************************************************************
*   Function                                                              
*       F_TFTPS_Process_Ack                                                
*                                                                       
*   Description                                                           
*       This function is responsible processing an ack packet whenever   
*       a read request is in progress.                                   
*                                                                       
*   Parameters                                                               
*       tftp_con        The pointer to TFTP Control Block                        
*                                                                       
*   Returns                                                               
*       TFTPS_RES_OK whenever the expected data was received.
************************************************************************/
static TCPIP_TFTPS_RESULT F_TFTPS_Process_Ack(TFTPS_CB *tftp_con, uint16_t bytes_received)
{
    TCPIP_UINT16_VAL tOpcode,blockNum;
    UDP_SOCKET_INFO sktInfo;
    TCPIP_TFTPS_RESULT retval = TFTPS_RES_OK;
    
    union
    {
        uint8_t     u8Buff[TCPIP_TFTPS_MIN_UDP_RX_BUFFER_SIZE + 1U];
        uint16_t    u16Buff[(TCPIP_TFTPS_MIN_UDP_RX_BUFFER_SIZE + 2U) / 2U];       // enforce alignment
    }rxBuf;

    // check the length 
    if(bytes_received > (uint16_t)sizeof(rxBuf.u8Buff))
    {
        bytes_received = (uint16_t)sizeof(rxBuf.u8Buff);
    }
        
    (void)memset(rxBuf.u8Buff, 0, sizeof(rxBuf.u8Buff));
    (void)memset(&sktInfo, 0, sizeof(sktInfo));
    // collect the TFTP ack receive bytes
    (void)TCPIP_UDP_ArrayGet(tftp_con->cSkt, rxBuf.u8Buff, bytes_received);
    // opcode value
    tOpcode.Val = F_TFTPS_Get16(rxBuf.u8Buff, (unsigned int)TFTP_OPCODE_OFFSET);
    // get the socket info details
    (void)TCPIP_UDP_SocketInfoGet(tftp_con->cSkt, &sktInfo);
    /* What kind of packet is this. */
    switch(tOpcode.Val)
    {
        case TFTPS_ACK_OPCODE:
            /* If the current block number is greater than the block number
             * of the ACK packet we are receiving, then this is an old
             * packet and we have already processed it - we do not want to
             * exit, error or acknowledge the packet (because we have
             * already processed it) so we will get the next packet
             */
            blockNum.Val = F_TFTPS_Get16(rxBuf.u8Buff, (unsigned int)TFTP_DATA_BLOCKNUM_OFFSET);
            if((tftp_con->block_number > blockNum.Val)
                && (tftp_con->tid == sktInfo.remotePort))
            {
                // do we need to retransmit the last packet?
                (void)F_TFTPS_Retransmit(tftp_con);
                // discard the last packet
                (void)TCPIP_UDP_Discard(tftp_con->cSkt);
                break;
            }
             // release the TFTP socket for the read type and transfer is completed
            if((tftp_con->type == TFTPS_READ_TYPE) && (tftp_con->status == TFTPS_TRANSFER_COMPLETE))
            {
                // send event notification after file transfer completion
                F_TFTPSNotifyUserClients(tftp_con,TCPIP_TFTPS_EVENT_READ_COMPLETED);
                 // release data socket, close file descriptor and close other parameters
                F_TFTPS_ReleaseResource(tftp_con);
                break;
            }
            /* Make sure the block number and TID are correct. */
            if((tftp_con->block_number == blockNum.Val)
                         && (tftp_con->tid == sktInfo.remotePort))
            {
                tftp_con->block_number++;
            }

            else
            {
                retval = TFTPS_RES_ILLIGAL_OPERN;
            }
            
            break;

        case TFTPS_ERROR_OPCODE:
            tftp_con->errCode = (uint8_t)F_TFTPS_Get16(rxBuf.u8Buff, (unsigned int)TFTP_ERROR_VAL_OFFSET);
            if(tftp_con->errCode <= (uint8_t)TFTP_BAD_TFTP_ERROR)
            {
                retval = TFTPS_RES_CLIENT_ERROR;
            }
            else
            {
                retval = TFTPS_RES_ILLIGAL_OPERN;
            }
            break;

        case TFTPS_RRQ_OPCODE:
        case TFTPS_WRQ_OPCODE:
        case TFTPS_DATA_OPCODE:
        default:
            (void)F_TFTPS_Error(tftp_con->cSkt,TFTP_ILLIGAL_OPERATION_ERROR, "Error: Illegal TFTP Operation");
            retval = TFTPS_RES_ILLIGAL_OPERN;
            break;
    }
    return retval;
}
   
/************************************************************************
*   Function                                                             
*       TFTPS_Send_Data                                                  
*                                                                       
*   Description                                                           
*       This function is responsible for sending data for the Client's   
*       Read Request.  It will read data from the file and send data     
*       until all bytes in the file have been sent.                      
*                                                                       
*   Parameters                                                                
*       tftp_con        The pointer to TFTP Control Block                        
*                                                                       
*   Return                                                               
*       The Number of bytes sent on success.                             
************************************************************************/
static uint16_t F_TFTPS_Send_Data(TFTPS_CB *tftp_con, uint16_t bytes_received)
{
    uint16_t    num_bytes;
    uint8_t     *wrPtr;
    uint16_t    wCnt;
    uint16_t    maxReadByte;
    
    /* If we received something, setup the tid and process the ACK. */
   if (bytes_received != 0U)
   {
       if(F_TFTPS_Process_Ack(tftp_con,bytes_received)!= TFTPS_RES_OK)
       {
           tftp_con->status = TFTPS_PROCESS_INPROGRESS;
           return 0;
       }
   }
    // make sure that this is the first data packet is being send from the Server
    if((bytes_received == 0U) && (tftp_con->block_number == 0U))
    {
        tftp_con->block_number = 1U;
    }
    wrPtr = tftp_con->trans_buf;
    if(wrPtr == NULL)
    {        
        return 0;
    }
    /* Fill in the opcode and block number. */
    F_TFTPS_Put16(wrPtr, (unsigned int)TFTP_OPCODE_OFFSET, (uint16_t)TFTPS_DATA_OPCODE);
    F_TFTPS_Put16(wrPtr, (unsigned int)TFTP_DATA_BLOCKNUM_OFFSET, tftp_con->block_number);   
    wrPtr=wrPtr + (unsigned int)TFTP_DATA_OFFSET;
    maxReadByte = tftp_con->options.blksize;
/*  Read data from the file into the the TFTP CB send buffer. */     
    wCnt = (uint16_t)tftp_con->tftps_shell_obj->fileRead(tftp_con->tftps_shell_obj, tftp_con->file_desc, wrPtr, (size_t)maxReadByte);
    
    /* If this is the last packet update the status. */
    if (wCnt < tftp_con->options.blksize)
    {
        tftp_con->status = TFTPS_TRANSFER_COMPLETE;
    }
    
     /*  Read data from the file into the the TFTP CB send buffer. */     
    num_bytes = wCnt + (uint16_t)TCPIP_TFTP_HEADER_MINSIZE;
    // Put complete TFTP error packet buffer to the UDP buffer
    // Once writing is completed into the buffer, TX offset need to be updated again,
    // because the socket flush function calculates how many bytes are in 
    // the buffer using the current write pointer:
    (void)TCPIP_UDP_TxOffsetSet(tftp_con->cSkt, num_bytes, false);
    tftp_con->lastTxPktSize = num_bytes;
   
    /* Send the data. */
    if(TCPIP_UDP_Flush(tftp_con->cSkt) != num_bytes)
    {
        num_bytes = 0U;
    }
    
    return num_bytes;
} /* end TFTPS_Send_Data */
   
/************************************************************************
*   Function                                                              
*       F_TFTPS_Retransmit                                                 
*                                                                       
*   Description                                                           
*       This function will retransmit the last packet sent.              
*                                                                       
*   Parameters                                                                
*       tftp_con        The pointer to TFTP Control Block                        
*                                                                       
*   Return                                                               
*       The Number of bytes sent on success.                             
*                                                                       
************************************************************************/
static uint32_t F_TFTPS_Retransmit(TFTPS_CB *tftp_con)
{
    uint16_t    send_size;
    
    /* While an ack of the data packet has not been received 
    * and the maximum number of retransmits has not yet been 
    * reached, retransmit the last data packet. 
    */    
    uint32_t sysFreq = SYS_TMR_TickCounterFrequencyGet();
    if((SYS_TMR_TickCountGet() - tftp_con->reply_timeout) < ((uint32_t)TCPIP_TFTPS_DEFAULT_TIMEOUT * sysFreq))
    {        
        return 0;
    }

    //Get the TFTP last packet size length
    send_size = tftp_con->lastTxPktSize;
    // Put complete TFTP error packet buffer to the UDP buffer
    // Once writing is completed into the buffer, TX offset need to be updated again,
    // because the socket flush function calculates how many bytes are in 
    // the buffer using the current write pointer:
    (void)TCPIP_UDP_TxOffsetSet(tftp_con->cSkt, send_size, false);
    
    
    /* Retransmit the last packet. */
    if(TCPIP_UDP_Flush(tftp_con->cSkt) != send_size)
    {
        send_size = 0U;
    }
    if(tftp_con->retransmits < (uint8_t)TCPIP_TFTPS_RETRANSMIT_COUNT)
    {
        tftp_con->retransmits++;
    }
    tftp_con->reply_timeout = SYS_TMR_TickCountGet();
    return send_size;
}/* TFTPS_Retransmit */
   
/************************************************************************
* Function                                                              
*       TFTPS_Error                                   
*                    
* Description                                                          
*       This function will send an error packet.                         
*
* parameters 
*       skt = socket value                                                        
*       error_code      The TFTP error code.         
*       err_string      The error message to send.   
*
* Return                                                               
*       The Number of bytes sent on success.                             
************************************************************************/
static TCPIP_TFTPS_RESULT F_TFTPS_Error(UDP_SOCKET skt, TFTP_ERROR_CODE error_code, const char *err_string)
{
    uint16_t   send_size;
    uint8_t    *p;
    
// this will put the start pointer at the beginning of the TX buffer
    (void)TCPIP_UDP_TxOffsetSet(skt, 0U, false);    

   //Get the write pointer:
    p = TCPIP_UDP_TxPointerGet(skt);
    if(p == NULL)
    {
        return TFTPS_RES_MEMORY_ALLOC_ERR;
    }
    /* Fill in the opcode and block number. */
    F_TFTPS_Put16(p,  (unsigned int)TFTP_OPCODE_OFFSET, (uint16_t)TFTPS_ERROR_OPCODE);
    F_TFTPS_Put16(p,  (unsigned int)TFTP_ERROR_VAL_OFFSET, (uint16_t)error_code);

    
    (void)strcpy((char*)&(p[(unsigned int)TFTP_DATA_OFFSET]), err_string);
    send_size = (uint16_t)strlen(err_string) + (uint16_t)TFTP_DATA_OFFSET;
    p[send_size++] = 0U;
    
    /* Send the datagram. */
   
    // Put complete TFTP error packet buffer to the UDP buffer
    // Once writing is completed into the buffer, TX offset need to be updated again,
    // because the socket flush function calculates how many bytes are in 
    // the buffer using the current write pointer:
    (void)TCPIP_UDP_TxOffsetSet(skt, send_size, false);

    if(TCPIP_UDP_Flush(skt) != send_size)
    {
        return TFTPS_RES_SKT_ERR;
    }
    
    return TFTPS_RES_OK;
} /* end TFTPS_Error */
 
   
/************************************************************************
* Function                                                              
*    F_TFTPS_Check_Options                                              
*
* Description                                                           
*    Accepts/Rejects the options requested by a client.               
*    If a block size more than TCPIP_TFTPS_DEFAULT_BLOCK_SIZE is requested, 
*    the client socket will reallocate to the new block size of the 
*    transmission buffer.   
* 
* Parameters                                                                
*    tftp_con        Pointer to the control block.
*    bytes_received  Number of bytes in the request packet, minus the    
*                    opcode and mode.             
*    count           Index into the request packet, starting at the 
*                    first option.                      
*    rx_buf          Pointer to the receive buffer's memory space.       
* 
* Return                                                               
*     TCPIP_TFTPS_RESULT                                                             
************************************************************************/
static TCPIP_TFTPS_RESULT F_TFTPS_Check_Options(TFTPS_CB *tftp_con, uint16_t bytes_received, uint16_t count, uint8_t* rx_buf)
{
    uint16_t tftpTxBufSize = 0, configTftpTxBufSize = 0;
    uint16_t count1;
    char temp2[TCPIP_TFTP_OPTION_SIZE + 1];
    char temp3[TCPIP_TFTP_OPTION_SIZE + 1];
    char *op_holder;
    char *value_holder;
    char* p;
    uint32_t temp32;
    
    //tftp_con->options.blksize = TCPIP_TFTPS_DEFAULT_BLOCK_SIZE+TCPIP_TFTP_HEADER_MINSIZE+1;
    tftp_con->options.tftpoptionAckflag.bits.timeout_ack = 0U;
    tftp_con->options.tftpoptionAckflag.bits.blksize_ack = 0U;
    tftp_con->options.tftpoptionAckflag.bits.tsize_ack = 0U;
    
   
    (void)memset(temp2, 0, sizeof(temp2));
    /* Parse the options */
    while (count < (bytes_received - 1U))
    {       
        count1 = 0U;
        p = (char*)&rx_buf[count];
        /* Parse the first option from the transmission buffer */
        while ((*p != '\0') && (count1 < (uint16_t)TCPIP_TFTP_OPTION_SIZE))
        {
            temp2[count1] = *p;
            count ++;
            count1 ++;
            p++;
        }
        
        temp2[count1] = '\0';
        op_holder = temp2;
        
        count++;
        count1 = 0U;
        
        p = (char*)&rx_buf[count];
        /* Parse the first value from the transmission buffer */
        while ((*p != '\0') && (count1 < (uint16_t)TCPIP_TFTP_OPTION_SIZE))
        {
            temp3[count1] = *p;
            count ++;
            count1 ++;
            p++;
        }
        
        temp3[count1] = '\0';
        value_holder = temp3;
        
        count++;
        
        /* The server has the authority to change the value of some
         * options we specified.
         */
        if (strcmp(op_holder, "timeout") == 0)
        {
            /* The server may not change the timeout value */
            temp32 = 0UL;
            (void)FC_Str2UL(value_holder, 10, &temp32);
            tftp_con->options.timeout = (uint16_t)temp32;
               
            tftp_con->options.tftpoptionAckflag.bits.timeout_ack = 1U;
        }
        else if (strcmp(op_holder, "tsize") == 0)
        {
            temp32 = 0UL;
            (void)FC_Str2UL(value_holder, 10, &temp32);

            if (temp32 == 0UL)
            {
                int32_t txSize = tftp_con->tftps_shell_obj->fileSize(tftp_con->tftps_shell_obj,tftp_con->file_desc);
                if(txSize > 0)
                {
                    tftp_con->options.tsize = (uint32_t)txSize;
                    /* We only return a tsize acknowledgement on a RRQ */
                    tftp_con->options.tftpoptionAckflag.bits.tsize_ack = 1U;
                }
            }               
            else
            {
                tftp_con->options.tsize = temp32;
            }
        }
        else if (strcmp(op_holder, "blksize") == 0)
        {           
            /* Check that the requested blksize is <= 1468 and >= 8*/
            uint32_t reqSize = 0UL;
            (void)FC_Str2UL(value_holder, 10, &reqSize);
            if ((reqSize <= (uint32_t)TCPIP_TFTP_BLOCK_SIZE_MAX) && (reqSize >= (uint32_t)TCPIP_TFTP_BLOCK_SIZE_MIN))
            {
                tftp_con->options.blksize = (uint16_t)temp32; //+TCPIP_TFTP_HEADER_MINSIZE+1; 
            }
            else
            {
                // TFTP block size fixed to 1468 bytes
                tftp_con->options.blksize = TCPIP_TFTPS_DEFAULT_BLOCK_SIZE;
            }               
            tftp_con->options.tftpoptionAckflag.bits.blksize_ack = 1U;
        }
        else
        {
            // do nothing
        }
    }
       
    if (tftp_con->options.tftpoptionAckflag.bits.blksize_ack == 1U)
    {
        tftpTxBufSize = tftp_con->options.blksize+TCPIP_TFTP_HEADER_MINSIZE;
        (void)TCPIP_UDP_OptionsGet(tftp_con->cSkt, UDP_OPTION_TX_BUFF, (void*)&configTftpTxBufSize);

        if(tftpTxBufSize != configTftpTxBufSize)
        {       
            /* Reallocate the original socket memory */            
            if(!TCPIP_UDP_OptionsSet(tftp_con->cSkt, UDP_OPTION_TX_BUFF, FC_Uint2VPtr((uint32_t)tftpTxBufSize)))
            {
                return TFTPS_RES_SKT_ERR;
            }
        }         
    }
    /* Allocate memory for the transmission buffer as specified 
      * by the blksize option 
      * */
    //Get the write pointer:
    tftp_con->trans_buf = TCPIP_UDP_TxPointerGet(tftp_con->cSkt);
    
    return TFTPS_RES_OK;
}

bool TCPIP_TFTPS_Enable(TCPIP_NET_HANDLE hNet,IP_ADDRESS_TYPE ipType)
{
    return F_TFTPS_Enable(hNet, true,ipType);
}

static bool F_TFTPS_Enable(TCPIP_NET_HANDLE hNet, bool checkIfUp,IP_ADDRESS_TYPE ipType)
{
    TCPIP_TFTPS_DCPT* pTftpsDcpt;
    TCPIP_NET_IF* pNetIf;

    pTftpsDcpt = &gTftpsDcpt;

    if(checkIfUp)
    {
        pNetIf = TCPIPStackHandleToNetUp(hNet);
    }
    else
    {
        pNetIf = TCPIPStackHandleToNet(hNet);
    }

    if(pNetIf == NULL)
    {
        return false;
    }
    
    pTftpsDcpt->prefNet =  pNetIf;
   
    
    pTftpsDcpt->ipAddrType = ipType;
    
    if(F_TFTP_Server_start(pTftpsDcpt->prefNet,pTftpsDcpt->ipAddrType) != true)
    {
        return false;
    }
//    // start TFTP server
//    pTftpsDcpt->smState = (uint8_t)SM_TFTPS_RRECV;
    return true;
}

bool TCPIP_TFTPS_Disable(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNet(hNet);
    TCPIP_TFTPS_DCPT        *pTftpsDcpt;

    pTftpsDcpt = &gTftpsDcpt;
    if(pNetIf == NULL)
    {
        return false;
    }
    // close socket
    if(pTftpsDcpt->uSkt != INVALID_UDP_SOCKET)
    {
        // remove UDP signal handler
        if(gUdpRxSigHandle != NULL)
        {
            (void)TCPIP_UDP_SignalHandlerDeregister(pTftpsDcpt->uSkt, gUdpRxSigHandle);
            gUdpRxSigHandle = NULL;
        }
        // disable RX of further packets
        (void)TCPIP_UDP_OptionsSet(pTftpsDcpt->uSkt, UDP_OPTION_RX_QUEUE_LIMIT, FC_Uint2VPtr(0UL));
        // flush any pending data
        (void)TCPIP_UDP_Disconnect(pTftpsDcpt->uSkt, true);
        pTftpsDcpt->uSkt = INVALID_UDP_SOCKET;
    }

    if(pTftpsDcpt->prefNet == pNetIf)
    {
        pTftpsDcpt->prefNet = NULL;
    }

    return true;    
}

// Register an TFTPS event handler
// Use hNet == 0 to register on all interfaces available
// Returns a valid handle if the call succeeds,
// or a null handle if the call failed.
// Function has to be called after the TFTP server is initialized
// The hParam is passed by the client and will be used by the TFTP server when the notification is made.
// It is used for per-thread content or if more modules, for example, share the same handler
// and need a way to differentiate the callback.
#if (M_TFTPS_USER_NOTIFICATION != 0)
TCPIP_TFTPS_HANDLE TCPIP_TFTPS_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_TFTPS_EVENT_HANDLER handler, const void* hParam)
{
    TCPIP_TFTPS_DCPT        *pTftpsDcpt;

    pTftpsDcpt = &gTftpsDcpt;
    if(handler != NULL && pTftpsDcpt->memH != NULL)
    {
        TCPIP_TFTPS_LIST_NODE tftpNode;
        tftpNode.handler = handler;
        tftpNode.hParam = hParam;
        tftpNode.hNet = hNet;
        tftpNode.next = NULL;

        return FC_SglNode2CvPtr(TCPIP_Notification_Add(&pTftpsDcpt->tftpsRegisteredUsers, pTftpsDcpt->memH, &tftpNode, sizeof(tftpNode)));
    }
    return NULL;
}

// deregister the event handler
bool TCPIP_TFTPS_HandlerDeRegister(TCPIP_TFTPS_HANDLE hTftps)
{
    TCPIP_TFTPS_DCPT        *pTftpsDcpt;

    pTftpsDcpt = &gTftpsDcpt;
    if(pTftpsDcpt->memH != NULL && hTftps != NULL)
    {
        if(TCPIP_Notification_Remove(FC_CvPtr2SglNode(hTftps), &pTftpsDcpt->tftpsRegisteredUsers, pTftpsDcpt->memH))
        {
            return true;
        }
    }
    return false;
}
#else
TCPIP_TFTPS_HANDLE TCPIP_TFTPS_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_TFTPS_EVENT_HANDLER handler, const void* hParam)
{
    return NULL;
}
bool TCPIP_TFTPS_HandlerDeRegister(TCPIP_TFTPS_HANDLE hTftps)
{
    return false;
}
#endif  // (M_TFTPS_USER_NOTIFICATION != 0)

static void F_TFTPSNotifyUserClients(TFTPS_CB* pTftpCB, TCPIP_TFTPS_EVENT_TYPE evType)
{
#if (M_TFTPS_USER_NOTIFICATION != 0)
    TCPIP_TFTPS_DCPT* pTftpsDcpt;
    TCPIP_TFTPS_LIST_NODE* dNode;
    bool     triggerNotify;

    pTftpsDcpt = &gTftpsDcpt;
    
    TCPIP_Notification_Lock(&pTftpsDcpt->tftpsRegisteredUsers);
    for(dNode = FC_SglNode2TftpsNode(pTftpsDcpt->tftpsRegisteredUsers.list.head); dNode != NULL; dNode = dNode->next)
    {
        if(dNode->hNet == NULL || dNode->hNet == pTftpsDcpt->prefNet)
        {   // trigger event
            triggerNotify = dNode->hParam == NULL ? true : strcmp(dNode->hParam, (char*)pTftpCB->file_name) == 0;
            if(triggerNotify)
            {
                (*dNode->handler)(pTftpsDcpt->prefNet, evType,(char*)pTftpCB->file_name,pTftpCB);
            }
        }
    }    
    TCPIP_Notification_Unlock(&pTftpsDcpt->tftpsRegisteredUsers);
#endif  // (TCPIP_DNS_CLIENT_USER_NOTIFICATION != 0)
}

#endif  // defined(TCPIP_STACK_USE_TFTP_SERVER)

