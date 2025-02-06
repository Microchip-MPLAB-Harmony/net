/*******************************************************************************
  File Transfer Protocol (FTP) Client
  Module for Microchip TCP/IP Stack

  Summary:
    Implementation of FTP Client protocol
    
  Description:
    - FTP client provides ability to send/receive files to/from FTP server.
    - Reference: RFC 959

*******************************************************************************/

/*
Copyright (C) 2019-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_FTP_CLIENT

#include "tcpip/src/tcpip_private.h"
#include "net_pres/pres/net_pres_socketapi.h"
#include "system/fs/sys_fs.h"

#if defined(TCPIP_STACK_USE_FTP_CLIENT)
#include "tcpip/src/ftpc_private.h"


/****************************************************************************
  Section:
    FTPC Internal Variables
  ***************************************************************************/
static int          ftpcInitCount = 0;                  // module initialization count
static TCPIP_SIGNAL_HANDLE    ftpcSignalHandle = NULL;
static TCPIP_FTPC_DCPT_TYPE* ftpcDcptPool = NULL;          // allocated pool of descriptors
static TCPIP_FTPC_MODULE_DCPT_TYPE ftpcGlobalConfig;// global FTP Client configuration
static SINGLE_LIST  ftpcDcptFreeList = { 0 };      // pool of free descriptors 
static SINGLE_LIST  ftpcDcptBusyList = { 0 };      // pool of descriptors in use

static const TCPIP_FTPC_STATE_FUNC ftpcStateTbl[] = 
{
    &F_ftpcStateHome,                  // TCPIP_FTPC_STATE_HOME
    //Connect Routines
    &F_ftpcStateStartConnect,          // TCPIP_FTPC_STATE_START_CONNECT
    &F_ftpcStateWaitConnect,           // TCPIP_FTPC_STATE_WAIT_CONNECT
    &F_ftpcStateDoneConnect,           // TCPIP_FTPC_STATE_DONE_CONNECT
    //Login Routines
    &F_ftpcStateSendUser,              //TCPIP_FTPC_STATE_LOGIN_SEND_USER  
    &F_ftpcStateWaitUser,              //TCPIP_FTPC_STATE_LOGIN_WAIT_USER
    &F_ftpcStateSendPass,              //TCPIP_FTPC_STATE_LOGIN_SEND_PASS
    &F_ftpcStateWaitPass,              //TCPIP_FTPC_STATE_LOGIN_WAIT_PASS
    &F_ftpcStateSendAcct,              //TCPIP_FTPC_STATE_LOGIN_SEND_ACCT
    &F_ftpcStateWaitAcct,              //TCPIP_FTPC_STATE_LOGIN_WAIT_ACCT
    &F_ftpcStateDoneLogin,             //TCPIP_FTPC_STATE_DONE_LOGIN
    //CWD Routines
    &F_ftpcStateSendCwd,               //TCPIP_FTPC_STATE_SEND_CWD  
    &F_ftpcStateWaitCwd,               //TCPIP_FTPC_STATE_WAIT_CWD
    //CDUP Routines        
    &F_ftpcStateSendCdup,              //TCPIP_FTPC_STATE_SEND_CDUP  
    &F_ftpcStateWaitCdup,              //TCPIP_FTPC_STATE_WAIT_CDUP
    //MKD Routines        
    &F_ftpcStateSendMkd,              //TCPIP_FTPC_STATE_SEND_MKD  
    &F_ftpcStateWaitMkd,              //TCPIP_FTPC_STATE_WAIT_MKD
    //RMD Routines        
    &F_ftpcStateSendRmd,              //TCPIP_FTPC_STATE_SEND_RMD,  
    &F_ftpcStateWaitRmd,              //TCPIP_FTPC_STATE_WAIT_RMD,
    //DELE Routines        
    &F_ftpcStateSendDele,             //TCPIP_FTPC_STATE_SEND_DELE,  
    &F_ftpcStateWaitDele,             //TCPIP_FTPC_STATE_WAIT_DELE,    
    //QUIT Routines 
    &F_ftpcStateSendQuit,             //TCPIP_FTPC_STATE_SEND_QUIT,
    &F_ftpcStateWaitQuit,             //TCPIP_FTPC_STATE_WAIT_QUIT,    
    //PWD Routines        
    &F_ftpcStateSendPwd,              //TCPIP_FTPC_STATE_SEND_PWD,  
    &F_ftpcStateWaitPwd,              //TCPIP_FTPC_STATE_WAIT_PWD,
    //TYPE Routines        
    &F_ftpcStateSendType,             //TCPIP_FTPC_STATE_SEND_TYPE,  
    &F_ftpcStateWaitType,             //TCPIP_FTPC_STATE_WAIT_TYPE,
    //STRU Routines 
    &F_ftpcStateSendStru,             //TCPIP_FTPC_STATE_SEND_STRU,  
    &F_ftpcStateWaitStru,             //TCPIP_FTPC_STATE_WAIT_STRU,
    //MODE Routines 
    &F_ftpcStateSendMode,             //TCPIP_FTPC_STATE_SEND_MODE,  
    &F_ftpcStateWaitMode,             //TCPIP_FTPC_STATE_WAIT_MODE,
    //PORT routines
    &F_ftpcStateSendPort,              //TCPIP_FTPC_STATE_SEND_PORT,  
    &F_ftpcStateWaitPort,              //TCPIP_FTPC_STATE_WAIT_PORT,
    //PASV routines
    &F_ftpcStateSendPasv,              //TCPIP_FTPC_STATE_SEND_PASV,  
    &F_ftpcStateWaitPasv,              //TCPIP_FTPC_STATE_WAIT_PASV, 
    &F_ftpcStateWaitPasvDataConnect,   //TCPIP_FTPC_STATE_PASV_WAIT_DATA_CONNECT    
    //RETR States        
    &F_ftpcStateSendRetr,              //TCPIP_FTPC_STATE_SEND_RETR,  
    &F_ftpcStateWaitRetrCtrlResp,      //TCPIP_FTPC_STATE_WAIT_RETR_CTRL_RESPONSE,
    &F_ftpcStateWaitRetrReadDataSkt,   //TCPIP_FTPC_STATE_WAIT_RETR_READ_DATA_SOCKET,
    &F_ftpcStateRetrEoF,               //TCPIP_FTPC_STATE_RETR_EOF
    //STOR States        
    &F_ftpcStateSendStor,              //TCPIP_FTPC_STATE_SEND_STOR,  
    &F_ftpcStateWaitStorCtrlResp,      //TCPIP_FTPC_STATE_WAIT_STOR_CTRL_RESPONSE,
    &F_ftpcStateStorWriteDataSkt,      //TCPIP_FTPC_STATE_STOR_WRITE_DATA_SOCKET
    &F_ftpcStateStorEoF,               //TCPIP_FTPC_STATE_STOR_EOF
    //Name List
    &F_ftpcStateStartLst,             //TCPIP_FTPC_STATE_START_LST
    &F_ftpcStateSendLst,              //TCPIP_FTPC_STATE_SEND_LST,  
    &F_ftpcStateWaitLstCtrlResp,      //TCPIP_FTPC_STATE_WAIT_LST_CTRL_RESPONSE,
    &F_ftpcStateWaitLstReadDataSkt,   //TCPIP_FTPC_STATE_WAIT_LST_READ_DATA_SOCKET,
    &F_ftpcStateLstEoT,              //TCPIP_FTPC_STATE_LST_EOT,
    //Disconnect Control socket connection                 
};

// conversion functions/helpers
//

static __inline__ TCPIP_FTPC_DCPT_TYPE*  __attribute__((always_inline)) FC_SglNode2FtpcDcpt(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        TCPIP_FTPC_DCPT_TYPE* pDcpt;
    }U_NODE_FTPC_DCPT;

    U_NODE_FTPC_DCPT.node = node;
    return U_NODE_FTPC_DCPT.pDcpt;
}

static __inline__ SGL_LIST_NODE*  __attribute__((always_inline)) FC_FtpcDcpt2SglNode(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    union
    {
        TCPIP_FTPC_DCPT_TYPE* pDcpt;
        SGL_LIST_NODE*  node;
    }U_FTPC_DCPT_SGL_NODE;

    U_FTPC_DCPT_SGL_NODE.pDcpt = pDcpt;
    return U_FTPC_DCPT_SGL_NODE.node;
}

static __inline__ TCPIP_FTPC_DCPT_TYPE*  __attribute__((always_inline)) FC_ConnHndl2FtpcDcpt(TCPIP_FTPC_CONN_HANDLE_TYPE hndl)
{
    union
    {
        TCPIP_FTPC_CONN_HANDLE_TYPE  hndl;
        TCPIP_FTPC_DCPT_TYPE* pDcpt;
    }U_CONN_HNDL_FTPC_DCPT;

    U_CONN_HNDL_FTPC_DCPT.hndl = hndl;
    return U_CONN_HNDL_FTPC_DCPT.pDcpt;
}

static __inline__ TCPIP_FTPC_DCPT_TYPE*  __attribute__((always_inline)) FC_CvPtr2FtpcDcpt(const void* param)
{
    union
    {
        const void* param;
        TCPIP_FTPC_DCPT_TYPE* pDcpt;
    }U_CV_PTR_FTPC_DCPT;

    U_CV_PTR_FTPC_DCPT.param = param;
    return U_CV_PTR_FTPC_DCPT.pDcpt;
}

static TCPIP_FTPC_DCPT_TYPE*  FtpcConnHndl2Dcpt(TCPIP_FTPC_CONN_HANDLE_TYPE hndl)
{
    if(hndl != NULL)
    {
        TCPIP_FTPC_DCPT_TYPE* pDcpt = FC_ConnHndl2FtpcDcpt(hndl);
        // debug mode can add further validation here
        return pDcpt;
    }
    return NULL;
}
////////////////////////////////////////////////////////////////////////////////
//////////////////////////// FTPC Core Functions ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool TCPIP_FTPC_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* initData)
{
    TCPIP_FTPC_DCPT_TYPE * pftpcDcpt;
    size_t index;
    
    if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }
    if(ftpcInitCount == 0)
    {   // stack start up; initialize just once
        // check configuration data is not missing
        if(initData == NULL)
        {
            return false;
        }
        
        const TCPIP_FTPC_MODULE_CONFIG_TYPE* pftpcConfig = (const TCPIP_FTPC_MODULE_CONFIG_TYPE*)initData;
        // create the FTPC timer
        ftpcSignalHandle = TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_FTPC_Task, TCPIP_FTPC_TASK_TICK_RATE);
        if(ftpcSignalHandle == NULL)
        {   // cannot create the FTPC timer
            return false;
        }
        // initialize the in-use list
        TCPIP_Helper_SingleListInitialize(&ftpcDcptBusyList);
        // initialize the free pool
        TCPIP_Helper_SingleListInitialize(&ftpcDcptFreeList);
        
        // allocate the descriptors pool
        ftpcDcptPool = (TCPIP_FTPC_DCPT_TYPE*)TCPIP_HEAP_Calloc(stackCtrl->memH, pftpcConfig->nMaxClients, sizeof(*ftpcDcptPool));
        if(ftpcDcptPool == NULL)
        {   // cannot create the FTPC pool, so remove FTPC timer
            TCPIPStackSignalHandlerDeregister(ftpcSignalHandle);
            ftpcSignalHandle = NULL;
            return false;
        }
        
        pftpcDcpt = ftpcDcptPool;
        
        for(index = 0; index < pftpcConfig->nMaxClients; index++)
        {
            F_ftpcDcptSetDefault(pftpcDcpt, index);
            TCPIP_Helper_SingleListTailAdd(&ftpcDcptFreeList, FC_FtpcDcpt2SglNode(pftpcDcpt));
            pftpcDcpt++;
        }
        ftpcGlobalConfig.nMaxClients = pftpcConfig->nMaxClients;
        ftpcGlobalConfig.memH = stackCtrl->memH;
        ftpcGlobalConfig.ftpcTmo = pftpcConfig->ftpcTmo > (uint32_t)TCPIP_FTPC_MIN_TMO ?  pftpcConfig->ftpcTmo : (uint32_t)TCPIP_FTPC_MIN_TMO;      
        ftpcGlobalConfig.data_tx_buffsize_dflt = pftpcConfig->data_tx_buffsize_dflt;
        ftpcGlobalConfig.data_rx_buffsize_dflt = pftpcConfig->data_rx_buffsize_dflt;
    }
    
    ftpcInitCount++;
    return true;
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void F_ftpcReleaseSockets(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    if(pDcpt->ftpcSocket.ftpcCtrlSkt != NET_PRES_INVALID_SOCKET)
    {
        //close control socket
        NET_PRES_SocketClose(pDcpt->ftpcSocket.ftpcCtrlSkt);
        pDcpt->ftpcSocket.ftpcCtrlSkt = NET_PRES_INVALID_SOCKET;
    }
    // Free Control Socket Buffers
    (void)TCPIP_HEAP_Free(ftpcGlobalConfig.memH,pDcpt->ftpcCtrlRxBuff);
    (void)TCPIP_HEAP_Free(ftpcGlobalConfig.memH,pDcpt->ftpcCtrlTxBuff);
    F_ftpcCloseDataSkt(pDcpt);
}

void TCPIP_FTPC_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    TCPIP_FTPC_DCPT_TYPE * pftpcDcpt;
    size_t index;
    
    if(ftpcInitCount > 0)
    {
        if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
        {   // stack shut down
            if(--ftpcInitCount == 0)
            {
                if(ftpcDcptPool != NULL)
                {
                    pftpcDcpt = ftpcDcptPool;

                    for(index = 0; index < ftpcGlobalConfig.nMaxClients; index++)
                    {
                        F_ftpcReleaseSockets(pftpcDcpt);  
                        pftpcDcpt++;
                    }
                    (void)TCPIP_HEAP_Free(ftpcGlobalConfig.memH, ftpcDcptPool);
                }
                if(ftpcSignalHandle != NULL)
                {
                    TCPIPStackSignalHandlerDeregister(ftpcSignalHandle);
                    ftpcSignalHandle = NULL;
                } 
            }
        }
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

void  TCPIP_FTPC_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != TCPIP_MODULE_SIGNAL_NONE)
    { // TMO or RX signals occurred
      TCPIP_FTPC_Process();
    }
}

static void TCPIP_FTPC_Process(void)
{
    TCPIP_FTPC_DCPT_TYPE* pCurrDcpt, *pNextDcpt;
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;

    OSAL_CRITSECT_DATA_TYPE ftpcLock = ftpcEnterCritical();
    pCurrDcpt = FC_SglNode2FtpcDcpt(ftpcDcptBusyList.head);
    pNextDcpt = pCurrDcpt != NULL ? pCurrDcpt->next : NULL;
    ftpcExitCritical(ftpcLock);
    
    while(pCurrDcpt != NULL)
    {
        // process this descriptor
        if(pCurrDcpt->ftpcActive)
        {
            res = (*ftpcStateTbl[pCurrDcpt->ftpcState])(pCurrDcpt);
           
            //error handling
            if(res == TCPIP_FTPC_RES_ERROR)
            {
                SYS_CONSOLE_MESSAGE(" FTPC Error\r\n");
            }                
        }

        ftpcLock = ftpcEnterCritical();
        pCurrDcpt = pNextDcpt;
        pNextDcpt = pCurrDcpt != NULL ? pCurrDcpt->next : NULL;
        ftpcExitCritical(ftpcLock);
    }
    
}

//FTPC lock section for protection
static __inline__ OSAL_CRITSECT_DATA_TYPE __attribute__((always_inline)) ftpcEnterCritical(void)
{
    return OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
}
//FTPC release lock of protected section
static __inline__ void __attribute__((always_inline)) ftpcExitCritical(OSAL_CRITSECT_DATA_TYPE critStatus)
{
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critStatus);
}

// send a signal to the FTPC module that data is available
// no manager alert needed since this normally results as a higher layer (TCP) signal
static void F_FTPCCtrlSktRxSignalHandler(NET_PRES_SKT_HANDLE_T hTCP, NET_PRES_SIGNAL_HANDLE hNet, 
                                                            uint16_t sigType, const void* param)
{
    TCPIP_FTPC_DCPT_TYPE* pDcpt = FC_CvPtr2FtpcDcpt(param);
    if((sigType & (uint16_t)TCPIP_TCP_SIGNAL_RX_DATA) != 0U)
    {
        (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
    if((sigType & (uint16_t)TCPIP_TCP_SIGNAL_RX_FIN) != 0U)
    {
        pDcpt->ftpcSignal |= (uint16_t)TCPIP_FTPC_SIGNAL_CTRL_RX_FIN;
        F_ftpcCtrlDisconnect(pDcpt);
    }

}

//Callback for data socket
static void F_FTPCDataSktRxSignalHandler(NET_PRES_SKT_HANDLE_T hTCP, NET_PRES_SIGNAL_HANDLE hNet, 
                                                            uint16_t sigType, const void* param)
{
    TCPIP_FTPC_DCPT_TYPE* pDcpt = FC_CvPtr2FtpcDcpt(param);
    
    if((sigType & (uint16_t)TCPIP_TCP_SIGNAL_RX_DATA) != 0U)
    {
        (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
    if((sigType & (uint16_t)TCPIP_TCP_SIGNAL_RX_FIN)!= 0U)
    {
        pDcpt->ftpcSignal |= (uint16_t)TCPIP_FTPC_SIGNAL_DATA_RX_FIN;           
    }
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////// FTPC External APIs  ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////

TCPIP_FTPC_CONN_HANDLE_TYPE TCPIP_FTPC_Connect(TCPIP_FTPC_CTRL_CONN_TYPE *pftpcConn, 
        TCPIP_FTPC_CTRL_EVENT_CALLBACK_TYPE ctrlCallback, TCPIP_FTPC_RETURN_TYPE* pResult)
{
    TCPIP_FTPC_DCPT_TYPE* pDcpt = NULL;
    
    // Get a free descriptor for new connection    
    OSAL_CRITSECT_DATA_TYPE ftpcLock = ftpcEnterCritical();    
    pDcpt = FC_SglNode2FtpcDcpt(TCPIP_Helper_SingleListHeadRemove(&ftpcDcptFreeList));
    ftpcExitCritical(ftpcLock);
    
    if(pDcpt != NULL)
    {
        *pResult = TCPIP_FTPC_RET_OK;
        pDcpt->ftpcState = TCPIP_FTPC_STATE_START_CONNECT;
        pDcpt->ftpcCtrlConnection.ftpcServerAddr = pftpcConn->ftpcServerAddr;
        pDcpt->ftpcCtrlConnection.ftpcServerIpAddrType = pftpcConn->ftpcServerIpAddrType;
        pDcpt->ftpcCtrlConnection.serverCtrlPort = (pftpcConn->serverCtrlPort == 0U)? 
                            (uint16_t)TCPIP_FTPC_DFLT_SRV_CTRL_PORT : pftpcConn->serverCtrlPort;
        pDcpt->ftpcActive = true;
        pDcpt->ctrlSktCallback = ctrlCallback;
        pDcpt->ftpcCommand = TCPIP_FTPC_CMD_CONNECT;
        pDcpt->error = TCPIP_FTPC_ERROR_NONE;
        ftpcLock = ftpcEnterCritical();
        TCPIP_Helper_SingleListTailAdd(&ftpcDcptBusyList, FC_FtpcDcpt2SglNode(pDcpt));
        ftpcExitCritical(ftpcLock);  
    }
    else
    {
        *pResult = TCPIP_FTPC_RET_FAILURE;
    }
    
    return (TCPIP_FTPC_CONN_HANDLE_TYPE)pDcpt;
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_Disconnect (TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = FtpcConnHndl2Dcpt(ftpcHandle);
    
    if(pDcpt != NULL)
    {
        if(!pDcpt->ftpcActive)
        {
            F_ftpcCtrlDisconnect(pDcpt);
            ftpcRet = TCPIP_FTPC_RET_OK;    
        }
        else
        {
            ftpcRet = TCPIP_FTPC_RET_BUSY;
        }
    }
    else
    {
        ftpcRet = TCPIP_FTPC_RET_BAD_HANDLE;
    }    
    
    return ftpcRet;
}


TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_Login(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, const char *username, 
                const char *password, const char *account)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = FtpcConnHndl2Dcpt(ftpcHandle);

    if(pDcpt != NULL)
    {
        if(!pDcpt->ftpcActive)
        {
            if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_CONNECTED) != 0U)
            {
                pDcpt->ftpcAccessControl.ftpcUserName = username;
                pDcpt->ftpcAccessControl.ftpcPassword = password;
                pDcpt->ftpcAccessControl.ftpcAccount = account;
                pDcpt->ftpcState = TCPIP_FTPC_STATE_LOGIN_SEND_USER;
                ftpcRet = TCPIP_FTPC_RET_OK;
                pDcpt->ftpcActive = true;
            }
            else
            {
                ftpcRet = TCPIP_FTPC_RET_NOT_CONNECT;
            }
        }
        else
        {
            ftpcRet = TCPIP_FTPC_RET_BUSY;
        }
    }
    else
    {
        ftpcRet = TCPIP_FTPC_RET_BAD_HANDLE;
    }    
    
    return ftpcRet;
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_Change_Dir(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, const char *pathname)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = FtpcConnHndl2Dcpt(ftpcHandle);
    if(pDcpt != NULL)
    {
        if(!pDcpt->ftpcActive)
        {
            if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_CONNECTED) != 0U)
            {
                if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_LOGGEDIN) != 0U)
                {
                    pDcpt->ftpcServerPathname = pathname;
                    pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_CWD;
                    pDcpt->ftpcCommand = TCPIP_FTPC_CMD_CWD;
                    pDcpt->error = TCPIP_FTPC_ERROR_NONE;
                    pDcpt->ftpcActive = true;       
                    ftpcRet = TCPIP_FTPC_RET_OK;
                }
                else
                {
                    ftpcRet = TCPIP_FTPC_RET_NOT_LOGIN;
                }
            }
            else
            {
                ftpcRet = TCPIP_FTPC_RET_NOT_CONNECT;
            }
        }
        else
        {
            ftpcRet = TCPIP_FTPC_RET_BUSY;
        }
    }
    else
    {
        ftpcRet = TCPIP_FTPC_RET_BAD_HANDLE;
    }
    
    return ftpcRet;
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_ChangeToParentDir(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = FtpcConnHndl2Dcpt(ftpcHandle);
    if(pDcpt != NULL)
    {
        if(!pDcpt->ftpcActive)
        {
            if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_CONNECTED) != 0U)
            {
                if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_LOGGEDIN) != 0U)
                {              
                    pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_CDUP;
                    pDcpt->ftpcCommand = TCPIP_FTPC_CMD_CDUP;
                    pDcpt->error = TCPIP_FTPC_ERROR_NONE;
                    pDcpt->ftpcActive = true;         
                    ftpcRet = TCPIP_FTPC_RET_OK;
                }
                else
                {
                    ftpcRet = TCPIP_FTPC_RET_NOT_LOGIN;
                }
            }
            else
            {
                ftpcRet = TCPIP_FTPC_RET_NOT_CONNECT;
            }
        }
        else
        {
            ftpcRet = TCPIP_FTPC_RET_BUSY;
        }
    }
    else
    {
        ftpcRet = TCPIP_FTPC_RET_BAD_HANDLE;
    }
    
    return ftpcRet;   
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_MakeDir(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, const char *dirname)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = FtpcConnHndl2Dcpt(ftpcHandle);
    
    if(pDcpt != NULL)
    {
        if(!pDcpt->ftpcActive)
        {
            if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_CONNECTED) != 0U)
            {
                if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_LOGGEDIN) != 0U)
                {
                    pDcpt->ftpcServerPathname = dirname;
                    pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_MKD;
                    pDcpt->ftpcCommand = TCPIP_FTPC_CMD_MKD;
                    pDcpt->error = TCPIP_FTPC_ERROR_NONE;
                    pDcpt->ftpcActive = true;    
                    ftpcRet = TCPIP_FTPC_RET_OK;
                }
                else
                {
                    ftpcRet = TCPIP_FTPC_RET_NOT_LOGIN;
                }
            }
            else
            {
                ftpcRet = TCPIP_FTPC_RET_NOT_CONNECT;
            }
        }
        else
        {
            ftpcRet = TCPIP_FTPC_RET_BUSY;
        }
    }
    else
    {
        ftpcRet = TCPIP_FTPC_RET_BAD_HANDLE;
    }
    
    return ftpcRet;
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_RemoveDir(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, const char *pathname)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = FtpcConnHndl2Dcpt(ftpcHandle);
    if(pDcpt != NULL)
    {
        if(!pDcpt->ftpcActive)
        {
            if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_CONNECTED) != 0U)
            {
                if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_LOGGEDIN) != 0U)
                {
                    pDcpt->ftpcServerPathname = pathname;
                    pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_RMD;
                    pDcpt->ftpcCommand = TCPIP_FTPC_CMD_RMD;
                    pDcpt->error = TCPIP_FTPC_ERROR_NONE;
                    pDcpt->ftpcActive = true;
                    ftpcRet = TCPIP_FTPC_RET_OK;            
                }
                else
                {
                    ftpcRet = TCPIP_FTPC_RET_NOT_LOGIN;
                }
            }
            else
            {
                ftpcRet = TCPIP_FTPC_RET_NOT_CONNECT;
            }
        }
        else
        {
            ftpcRet = TCPIP_FTPC_RET_BUSY;
        }
    }
    else
    {
        ftpcRet = TCPIP_FTPC_RET_BAD_HANDLE;
    }
    
    return ftpcRet;
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_Get_WorkingDir (TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = FtpcConnHndl2Dcpt(ftpcHandle);
    
    if(pDcpt != NULL)
    {
        if(!pDcpt->ftpcActive)
        {
            if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_CONNECTED) != 0U)
            {
                if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_LOGGEDIN) != 0U)
                {
                    pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_PWD;            
                    pDcpt->ftpcCommand = TCPIP_FTPC_CMD_PWD;
                    pDcpt->error = TCPIP_FTPC_ERROR_NONE;
                    pDcpt->ftpcActive = true;   
                    ftpcRet = TCPIP_FTPC_RET_OK;
                }
                else
                {
                    ftpcRet = TCPIP_FTPC_RET_NOT_LOGIN;
                }
            }
            else
            {
                ftpcRet = TCPIP_FTPC_RET_NOT_CONNECT;
            }
        }
        else
        {
            ftpcRet = TCPIP_FTPC_RET_BUSY;
        }
    }
    else
    {
        ftpcRet = TCPIP_FTPC_RET_BAD_HANDLE;
    }
    
    return ftpcRet;    
}


TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_GetFile(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
        TCPIP_FTPC_DATA_CONN_TYPE *pftpcDataConn, TCPIP_FTPC_FILE_OPT_TYPE *pfileOptions, 
        TCPIP_FTPC_DATA_EVENT_CALLBACK_TYPE dataCallback)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = FtpcConnHndl2Dcpt(ftpcHandle);
    if(pDcpt != NULL)
    {
        if(!pDcpt->ftpcActive)
        {
            if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_CONNECTED) != 0U)
            {
                if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_LOGGEDIN) != 0U)
                {
                    pDcpt->ftpcServerPathname = pfileOptions->serverPathName;
                    pDcpt->ftpcClientPathname = pfileOptions->clientPathName;
                    pDcpt->ftpcDataConnection.ftpcDataType = pftpcDataConn->ftpcDataType;
                    pDcpt->ftpcDataConnection.ftpcIsPassiveMode = pftpcDataConn->ftpcIsPassiveMode;
                    pDcpt->ftpcDataConnection.ftpcTransferMode = pftpcDataConn->ftpcTransferMode;
                    pDcpt->ftpcDataConnection.ftpcDataStructure = pftpcDataConn->ftpcDataStructure;   
                    pDcpt->ftpcDataConnection.ftpcDataTxBuffSize = pftpcDataConn->ftpcDataTxBuffSize;   
                    pDcpt->ftpcDataConnection.ftpcDataRxBuffSize = pftpcDataConn->ftpcDataRxBuffSize;  
                    pDcpt->dataSktCallback = dataCallback;
                    pDcpt->ftpcCommand = TCPIP_FTPC_CMD_GET;
                    pDcpt->error = TCPIP_FTPC_ERROR_NONE;
                    (void)F_ftpcStateStartGet(pDcpt);            
                    pDcpt->ftpcActive = true;
                    ftpcRet = TCPIP_FTPC_RET_OK;
                }
                else
                {
                    ftpcRet = TCPIP_FTPC_RET_NOT_LOGIN;
                }
            }
            else
            {
                ftpcRet = TCPIP_FTPC_RET_NOT_CONNECT;
            }
        }
        else
        {
            ftpcRet = TCPIP_FTPC_RET_BUSY;
        }
    }
    else
    {
        ftpcRet = TCPIP_FTPC_RET_BAD_HANDLE;
    }
    
    
    return ftpcRet;
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_PutFile(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
        TCPIP_FTPC_DATA_CONN_TYPE *pftpcDataConn, TCPIP_FTPC_FILE_OPT_TYPE *pfileOptions, 
        TCPIP_FTPC_DATA_EVENT_CALLBACK_TYPE dataCallback)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = FtpcConnHndl2Dcpt(ftpcHandle);
    if(pDcpt != NULL)
    {
        if(!pDcpt->ftpcActive)
        {
            if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_CONNECTED) != 0U)
            {
                if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_LOGGEDIN) != 0U)
                {
                    pDcpt->ftpcServerPathname = pfileOptions->serverPathName;
                    pDcpt->ftpcClientPathname = pfileOptions->clientPathName;
                    pDcpt->ftpcDataConnection.ftpcDataType = pftpcDataConn->ftpcDataType;
                    pDcpt->ftpcDataConnection.ftpcIsPassiveMode = pftpcDataConn->ftpcIsPassiveMode;
                    pDcpt->ftpcDataConnection.ftpcTransferMode = pftpcDataConn->ftpcTransferMode;
                    pDcpt->ftpcDataConnection.ftpcDataStructure = pftpcDataConn->ftpcDataStructure;     
                    pDcpt->ftpcDataConnection.ftpcDataTxBuffSize = pftpcDataConn->ftpcDataTxBuffSize;   
                    pDcpt->ftpcDataConnection.ftpcDataRxBuffSize = pftpcDataConn->ftpcDataRxBuffSize;  
                    pDcpt->dataSktCallback = dataCallback;
                    pDcpt->ftpcFileOptions.store_unique = pfileOptions->store_unique;
                    pDcpt->ftpcCommand = TCPIP_FTPC_CMD_PUT;
                    pDcpt->error = TCPIP_FTPC_ERROR_NONE;
                    (void)F_ftpcStateStartPut(pDcpt); 
                    pDcpt->ftpcActive = true;
                    ftpcRet = TCPIP_FTPC_RET_OK;
                }
                else
                {
                    ftpcRet = TCPIP_FTPC_RET_NOT_LOGIN;
                }
            }
            else
            {
                ftpcRet = TCPIP_FTPC_RET_NOT_CONNECT;
            }
        }
        else
        {
            ftpcRet = TCPIP_FTPC_RET_BUSY;
        }
    }
    else
    {
        ftpcRet = TCPIP_FTPC_RET_BAD_HANDLE;
    }

    return ftpcRet;
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_NameList(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle,
        TCPIP_FTPC_DATA_CONN_TYPE *pftpcDataConn, TCPIP_FTPC_FILE_OPT_TYPE *pfileOptions, 
        TCPIP_FTPC_DATA_EVENT_CALLBACK_TYPE dataCallback)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = FtpcConnHndl2Dcpt(ftpcHandle);
    if(pDcpt != NULL)
    {
        if(!pDcpt->ftpcActive)
        {
            if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_CONNECTED) != 0U)
            {
                if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_LOGGEDIN) != 0U)
                {
                    pDcpt->ftpcServerPathname = pfileOptions->serverPathName;
                    pDcpt->ftpcClientPathname = pfileOptions->clientPathName;
                    pDcpt->ftpcDataConnection.ftpcIsPassiveMode = pftpcDataConn->ftpcIsPassiveMode;
                    pDcpt->ftpcDataConnection.ftpcDataTxBuffSize = pftpcDataConn->ftpcDataTxBuffSize;   
                    pDcpt->ftpcDataConnection.ftpcDataRxBuffSize = pftpcDataConn->ftpcDataRxBuffSize;  

                    pDcpt->ftpcCommand = TCPIP_FTPC_CMD_NLST;
                    pDcpt->error = TCPIP_FTPC_ERROR_NONE;
                    pDcpt->ftpcState = TCPIP_FTPC_STATE_START_LST;                        
                    pDcpt->dataSktCallback = dataCallback;
                    pDcpt->ftpcActive = true;
                    ftpcRet = TCPIP_FTPC_RET_OK;
                }
                else
                {
                    ftpcRet = TCPIP_FTPC_RET_NOT_LOGIN;
                }
            }
            else
            {
                ftpcRet = TCPIP_FTPC_RET_NOT_CONNECT;
            }
        }
        else
        {
            ftpcRet = TCPIP_FTPC_RET_BUSY;
        }
    }
    else
    {
        ftpcRet = TCPIP_FTPC_RET_BAD_HANDLE;
    }

    return ftpcRet;
}


TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_List(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle,
        TCPIP_FTPC_DATA_CONN_TYPE *pftpcDataConn, TCPIP_FTPC_FILE_OPT_TYPE *pfileOptions,
        TCPIP_FTPC_DATA_EVENT_CALLBACK_TYPE dataCallback)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = FtpcConnHndl2Dcpt(ftpcHandle);
    if(pDcpt != NULL)
    {
        if(!pDcpt->ftpcActive)
        {
            if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_CONNECTED) != 0U)
            {
                if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_LOGGEDIN) != 0U)
                {
                    pDcpt->ftpcServerPathname = pfileOptions->serverPathName;
                    pDcpt->ftpcClientPathname = pfileOptions->clientPathName;
                    pDcpt->ftpcDataConnection.ftpcIsPassiveMode = pftpcDataConn->ftpcIsPassiveMode;
                    pDcpt->ftpcDataConnection.ftpcDataTxBuffSize = pftpcDataConn->ftpcDataTxBuffSize;   
                    pDcpt->ftpcDataConnection.ftpcDataRxBuffSize = pftpcDataConn->ftpcDataRxBuffSize;  
                    pDcpt->ftpcCommand = TCPIP_FTPC_CMD_LIST;
                    pDcpt->error = TCPIP_FTPC_ERROR_NONE;
                    pDcpt->ftpcState = TCPIP_FTPC_STATE_START_LST;                        
                    pDcpt->dataSktCallback = dataCallback;
                    pDcpt->ftpcActive = true;
                    ftpcRet = TCPIP_FTPC_RET_OK;
                }
                else
                {
                    ftpcRet = TCPIP_FTPC_RET_NOT_LOGIN;
                }
            }
            else
            {
                ftpcRet = TCPIP_FTPC_RET_NOT_CONNECT;
            }
        }
        else
        {
            ftpcRet = TCPIP_FTPC_RET_BUSY;
        }
    }
    else
    {
        ftpcRet = TCPIP_FTPC_RET_BAD_HANDLE;
    }

    return ftpcRet;
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_DeleteFile(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, const char *pathname)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = FtpcConnHndl2Dcpt(ftpcHandle);
    if(pDcpt != NULL)
    {
        if(!pDcpt->ftpcActive)
        {
            if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_CONNECTED) != 0U)
            {
                if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_LOGGEDIN) != 0U)
                {
                    pDcpt->ftpcServerPathname = pathname;

                    pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_DELE;
                    ftpcRet = TCPIP_FTPC_RET_OK;
                    pDcpt->ftpcActive = true;
                }
                else
                {
                    ftpcRet = TCPIP_FTPC_RET_NOT_LOGIN;
                }
            }
            else
            {
                ftpcRet = TCPIP_FTPC_RET_NOT_CONNECT;
            }
        }
        else
        {
            ftpcRet = TCPIP_FTPC_RET_BUSY;
        }
    }
    else
    {
        ftpcRet = TCPIP_FTPC_RET_BAD_HANDLE;
    }

    return ftpcRet;
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_Logout(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = FtpcConnHndl2Dcpt(ftpcHandle);
    if(pDcpt != NULL)
    {
        if(!pDcpt->ftpcActive)
        {
            if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_CONNECTED) != 0U)
            {
                if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_LOGGEDIN) != 0U)
                {              
                    pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_QUIT;
                    ftpcRet = TCPIP_FTPC_RET_OK;
                    pDcpt->ftpcActive = true;
                }
                else
                {
                    ftpcRet = TCPIP_FTPC_RET_NOT_LOGIN;
                }
            }
            else
            {
                ftpcRet = TCPIP_FTPC_RET_NOT_CONNECT;
            }
        }
        else
        {
            ftpcRet = TCPIP_FTPC_RET_BUSY;
        }
    }
    else
    {
        ftpcRet = TCPIP_FTPC_RET_BAD_HANDLE;
    }


    return ftpcRet;   
}

void TCPIP_FTPC_Get_Status(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, TCPIP_FTPC_STATUS_TYPE * ftpcStatus)
{  
    TCPIP_FTPC_DCPT_TYPE* pDcpt = FtpcConnHndl2Dcpt(ftpcHandle); 
    
    if(pDcpt != NULL && ftpcStatus != NULL)
    {
        ftpcStatus->busy = pDcpt->ftpcActive;
        ftpcStatus->isConnected = (pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_CONNECTED) == (uint16_t)TCPIP_FTPC_FLAG_CONNECTED;
        ftpcStatus->isLoggedIn = (pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_LOGGEDIN) == (uint16_t)TCPIP_FTPC_FLAG_LOGGEDIN;
        ftpcStatus->cmd = pDcpt->ftpcCommand;
        ftpcStatus->error = pDcpt->error;
        ftpcStatus->ctrlSocket = pDcpt->ftpcSocket.ftpcCtrlSkt;
        ftpcStatus->dataSocket = pDcpt->ftpcSocket.ftpcDataSkt;

        ftpcStatus->ctrlRxLen = pDcpt->ftpcCtrlRxLen;
        ftpcStatus->ctrlTxLen = pDcpt->ftpcCtrlTxLen;
        ftpcStatus->dataRxLen = pDcpt->ftpcDataRxLen;
        ftpcStatus->dataTxLen = pDcpt->ftpcDataTxLen;
    }

}


TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_SetType(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, TCPIP_FTPC_DATA_REP_TYPE type)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = FtpcConnHndl2Dcpt(ftpcHandle);
    if(pDcpt != NULL)
    {
        if(!pDcpt->ftpcActive)
        {
            if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_CONNECTED) != 0U)
            {
                if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_LOGGEDIN) != 0U)
                {
                    pDcpt->ftpcDataConnection.ftpcDataType = type;
                    pDcpt->ftpcCommand = TCPIP_FTPC_CMD_TYPE;
                    pDcpt->error = TCPIP_FTPC_ERROR_NONE;
                    pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_TYPE;
                    pDcpt->ftpcActive = true;
                    ftpcRet = TCPIP_FTPC_RET_OK;
                }
                else
                {
                    ftpcRet = TCPIP_FTPC_RET_NOT_LOGIN;
                }
            }
            else
            {
                ftpcRet = TCPIP_FTPC_RET_NOT_CONNECT;
            }

        }
        else
        {
            ftpcRet = TCPIP_FTPC_RET_BUSY;
        }
    }
    else
    {
        ftpcRet = TCPIP_FTPC_RET_BAD_HANDLE;
    }

    return ftpcRet;
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_SetStruct(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, TCPIP_FTPC_DATA_STRUCT_TYPE file_struct)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = FtpcConnHndl2Dcpt(ftpcHandle);
    if(pDcpt != NULL)
    {
        if(!pDcpt->ftpcActive)
        {
            if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_CONNECTED) != 0U)
            {
                if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_LOGGEDIN) != 0U)
                {
                    pDcpt->ftpcDataConnection.ftpcDataStructure = file_struct;
                    pDcpt->ftpcCommand = TCPIP_FTPC_CMD_STRU;
                    pDcpt->error = TCPIP_FTPC_ERROR_NONE;
                    pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_STRU;
                    pDcpt->ftpcActive = true;
                    ftpcRet = TCPIP_FTPC_RET_OK;
                }
                else
                {
                    ftpcRet = TCPIP_FTPC_RET_NOT_LOGIN;
                }
            }
            else
            {
                ftpcRet = TCPIP_FTPC_RET_NOT_CONNECT;
            }

        }
        else
        {
            ftpcRet = TCPIP_FTPC_RET_BUSY;
        }
    }
    else
    {
        ftpcRet = TCPIP_FTPC_RET_BAD_HANDLE;
    }

    return ftpcRet;
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_SetMode(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, TCPIP_FTPC_TRANSFER_MODE_TYPE mode)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = FtpcConnHndl2Dcpt(ftpcHandle);
    if(pDcpt != NULL)
    {
        if(!pDcpt->ftpcActive)
        {
            if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_CONNECTED) != 0U)
            {
                if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_LOGGEDIN) != 0U)
                {
                    pDcpt->ftpcDataConnection.ftpcTransferMode = mode;
                    pDcpt->ftpcCommand = TCPIP_FTPC_CMD_MODE;
                    pDcpt->error = TCPIP_FTPC_ERROR_NONE;
                    pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_MODE;
                    pDcpt->ftpcActive = true;
                    ftpcRet = TCPIP_FTPC_RET_OK;
                }
                else
                {
                    ftpcRet = TCPIP_FTPC_RET_NOT_LOGIN;
                }
            }
            else
            {
                ftpcRet = TCPIP_FTPC_RET_NOT_CONNECT;
            }
        }
        else
        {
            ftpcRet = TCPIP_FTPC_RET_BUSY;
        }
    }
    else
    {
        ftpcRet = TCPIP_FTPC_RET_BAD_HANDLE;
    }

    return ftpcRet;
}


TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_SetActiveMode(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, TCPIP_FTPC_DATA_CONN_TYPE *pftpcDataConn)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = FtpcConnHndl2Dcpt(ftpcHandle);
    if(pDcpt != NULL)
    {
        if(!pDcpt->ftpcActive)
        {
            if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_CONNECTED) != 0U)
            {
                if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_LOGGEDIN) != 0U)
                {        
                    pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_PORT;
                    pDcpt->ftpcDataConnection.ftpcIsPassiveMode = false;
                    pDcpt->ftpcDataConnection.dataServerAddr = pftpcDataConn->dataServerAddr;
                    pDcpt->ftpcDataConnection.dataServerPort = pftpcDataConn->dataServerPort;
                    pDcpt->ftpcDataConnection.dataServerIpAddrType = pftpcDataConn->dataServerIpAddrType;
                    pDcpt->ftpcCommand = TCPIP_FTPC_CMD_PORT;
                    pDcpt->error = TCPIP_FTPC_ERROR_NONE;
                    ftpcRet = TCPIP_FTPC_RET_OK;
                    pDcpt->ftpcActive = true;
                }
                else
                {
                    ftpcRet = TCPIP_FTPC_RET_NOT_LOGIN;
                }
            }
            else
            {
                ftpcRet = TCPIP_FTPC_RET_NOT_CONNECT;
            }

        }
        else
        {
            ftpcRet = TCPIP_FTPC_RET_BUSY;
        }
    }
    else
    {
        ftpcRet = TCPIP_FTPC_RET_BAD_HANDLE;
    }

    return ftpcRet;
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_SetPassiveMode(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = FtpcConnHndl2Dcpt(ftpcHandle);
    if(pDcpt != NULL)
    {
        if(!pDcpt->ftpcActive)
        {
            if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_CONNECTED) != 0U)
            {
                if((pDcpt->ftpcFlag & (uint16_t)TCPIP_FTPC_FLAG_LOGGEDIN) != 0U)
                {
                    pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_PASV;
                    pDcpt->ftpcDataConnection.ftpcIsPassiveMode = true;
                    pDcpt->ftpcCommand = TCPIP_FTPC_CMD_PASV;
                    pDcpt->error = TCPIP_FTPC_ERROR_NONE;
                    ftpcRet = TCPIP_FTPC_RET_OK;
                    pDcpt->ftpcActive = true;
                }
                else
                {
                    ftpcRet = TCPIP_FTPC_RET_NOT_LOGIN;
                }
            }
            else
            {
                ftpcRet = TCPIP_FTPC_RET_NOT_CONNECT;
            }
        }
        else
        {
            ftpcRet = TCPIP_FTPC_RET_BUSY;
        }
    }
    else
    {
        ftpcRet = TCPIP_FTPC_RET_BAD_HANDLE;
    }

    return ftpcRet;
}
////////////////////////////////////////////////////////////////////////////////
/////////////////// FTPC State Machine Routines  ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//FTP Client State Machine: Initial state
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateHome(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    if(pDcpt->ftpcActive == true)
    {
        //mark the completion of command
        pDcpt->ftpcActive = false; 
        //Clear event callbacks
        pDcpt->dataSktCallback = NULL;
        pDcpt->ftpcSignal = (uint16_t)TCPIP_FTPC_SIGNAL_NONE;
        pDcpt->fileDescr = SYS_FS_HANDLE_INVALID;
        
    }
    return TCPIP_FTPC_RES_OK;
}

//////////////////////////// CONNECT ROUTINES //////////////////////////////////
//FTP Client State Machine: Start Client-server connection process
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateStartConnect(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t sktRxSize = 0;
    uint16_t sktTxSize = 0;
    
    if(pDcpt->ftpcSocket.ftpcCtrlSkt == NET_PRES_INVALID_SOCKET)
    {   // get a new socket
        NET_PRES_SKT_HANDLE_T ctrlSkt;
        NET_PRES_SKT_T sktType =  NET_PRES_SKT_DEFAULT_STREAM_CLIENT;
        ctrlSkt = NET_PRES_SocketOpen(0, sktType, (NET_PRES_SKT_ADDR_T)pDcpt->ftpcCtrlConnection.ftpcServerIpAddrType, 
                                    pDcpt->ftpcCtrlConnection.serverCtrlPort, NULL, NULL);

        if(ctrlSkt != NET_PRES_INVALID_SOCKET)
        {   
            pDcpt->ftpcSocket.ftpcCtrlSkt = ctrlSkt;
            // alert of incoming traffic
            (void)NET_PRES_SocketSignalHandlerRegister(ctrlSkt, (uint16_t)TCPIP_TCP_SIGNAL_RX_DATA | (uint16_t)TCPIP_TCP_SIGNAL_RX_FIN, &F_FTPCCtrlSktRxSignalHandler, pDcpt);
            
            if(NET_PRES_SocketOptionsGet(ctrlSkt, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_TX_BUFF, &sktTxSize))
            {
                //mask it for 16 byte aligned
                sktTxSize = sktTxSize & (uint16_t)FTPC_BUFF_SIZE_ALIGN_MASK;
                //Allocate and Add Tx buffer for Ctrl socket for commands
                pDcpt->ftpcCtrlTxBuff = (char *)TCPIP_HEAP_Malloc(ftpcGlobalConfig.memH, sktTxSize);
             }
            
            if(NET_PRES_SocketOptionsGet(ctrlSkt, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_RX_BUFF, &sktRxSize))
            {                   
                //mask it for 16 byte aligned          
                sktRxSize = sktRxSize & (uint16_t)FTPC_BUFF_SIZE_ALIGN_MASK;
                //Allocate and Add Rx buffer for Ctrl socket for command response
                pDcpt->ftpcCtrlRxBuff = (char *)TCPIP_HEAP_Malloc(ftpcGlobalConfig.memH, sktRxSize);
                
            }             
            
            //bind to remote server
            if(NET_PRES_SocketRemoteBind(pDcpt->ftpcSocket.ftpcCtrlSkt, 
                (NET_PRES_SKT_ADDR_T)pDcpt->ftpcCtrlConnection.ftpcServerIpAddrType,  
                    pDcpt->ftpcCtrlConnection.serverCtrlPort, 
                    FC_MultiAdd2PresAdd(pDcpt->ftpcCtrlConnection.ftpcServerAddr)))
            {   // connect to remote socket
                if(NET_PRES_SocketConnect(pDcpt->ftpcSocket.ftpcCtrlSkt))
                {   
                    pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_CONNECT;
                    ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
                    res = TCPIP_FTPC_RES_OK;
                } 
                else
                {   //failed connecting to remote socket
                    pDcpt->error = TCPIP_FTPC_ERROR_CONNECT_CTRL_SOCKET;
                    F_ftpcCtrlDisconnect(pDcpt);
                    res = TCPIP_FTPC_RES_ERROR; 
                }
            }
            else
            {
                // failed binding to remote server
                pDcpt->error = TCPIP_FTPC_ERROR_BIND_CTRL_SOCKET;
                F_ftpcCtrlDisconnect(pDcpt);
                res = TCPIP_FTPC_RES_ERROR; 
            }
        }
        else
        {    // failed to open a socket
            pDcpt->error = TCPIP_FTPC_ERROR_OPEN_CTRL_SOCKET;
            res = TCPIP_FTPC_RES_ERROR;    
        }
    } 
    return res;
}

//FTP Client State Machine: Wait for Client-server connection
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateWaitConnect(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;

    //Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {   //process the control response  
        res =   ftpcParseReplyCode(pDcpt); 
        if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
        {   //connect process done
            pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_CONNECT;
            (void)F_ftpcStateDoneConnect(pDcpt);
        }
        else
        {
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;   
            F_ftpcCtrlDisconnect(pDcpt);
            res = TCPIP_FTPC_RES_ERROR;
        }
    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;        
            F_ftpcCtrlDisconnect(pDcpt);
            res = TCPIP_FTPC_RES_ERROR; 
        }  
        else
        {
            res = TCPIP_FTPC_RES_PENDING;
        }        
    }        
    return res;
}

//FTP Client State Machine: Mark the completion of Client-server connection
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateDoneConnect(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    // Add flag to indicate FTP Client connected 
    pDcpt->ftpcFlag  = (uint16_t)TCPIP_FTPC_FLAG_CONNECTED;    
    // trigger call back for control socket
    ftpcCtrlEvent(pDcpt,TCPIP_FTPC_CTRL_EVENT_SUCCESS, NULL,0);    
    // returns the state machine to default home
    pDcpt->ftpcState = TCPIP_FTPC_STATE_HOME; 
    return TCPIP_FTPC_RES_OK; 
}

///////////////////////////// LOGIN ROUTINES ///////////////////////////////////
//FTP Client State Machine: Send USER Command to FTP server
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateSendUser(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes = 0;
     
    nBytes = (uint16_t)FC_sprintf(pDcpt->ftpcCtrlTxBuff, sizeof(pDcpt->ftpcCtrlTxBuff), "USER %s\r\n", pDcpt->ftpcAccessControl.ftpcUserName);
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state     
        pDcpt->ftpcState = TCPIP_FTPC_STATE_LOGIN_WAIT_USER;        
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);        
    }  
    pDcpt->ftpcCommand = TCPIP_FTPC_CMD_USER;
    return res;
}

//FTP Client State Machine: wait for USER Command to FTP server
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateWaitUser(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //230
    //530
    //500, 501, 421
    //331, 332
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
    //Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {   //process the control response
        res =   ftpcParseReplyCode(pDcpt); 
        if(res == TCPIP_FTPC_SERVER_POS_INTRMDT_RESPONSE)
        {   //send the password
            pDcpt->ftpcState = TCPIP_FTPC_STATE_LOGIN_SEND_PASS;
            (void)F_ftpcStateSendPass(pDcpt);
        }
        else if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
        {            
            //no need of password; login is success
            pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_LOGIN;
            (void)F_ftpcStateDoneLogin(pDcpt);
        }
        else
        {
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
        }

    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR; 
        }  
        else
        {
            res = TCPIP_FTPC_RES_PENDING;
        }        
    }         
    return res;
}

//FTP Client State Machine: Send PASS Command to FTP server
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateSendPass(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes = 0;
    
    nBytes = (uint16_t)FC_sprintf(pDcpt->ftpcCtrlTxBuff, sizeof(pDcpt->ftpcCtrlTxBuff), "PASS %s\r\n", pDcpt->ftpcAccessControl.ftpcPassword);
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state     
        pDcpt->ftpcState = TCPIP_FTPC_STATE_LOGIN_WAIT_PASS;        
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);        
    } 
    pDcpt->ftpcCommand = TCPIP_FTPC_CMD_PASS;
    return res;
}

//FTP Client State Machine: wait for PASS Command to FTP server//FTP Client State Machine: wait for USER Command to FTP server
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateWaitPass(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //230
    //202
    //530
    //500, 501, 503, 421
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
    //Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {   //process the control response  
        res =   ftpcParseReplyCode(pDcpt); 
        if(res == TCPIP_FTPC_SERVER_POS_INTRMDT_RESPONSE)
        {
            //send the Account 
            pDcpt->ftpcState = TCPIP_FTPC_STATE_LOGIN_SEND_ACCT;
            (void)F_ftpcStateSendAcct(pDcpt);
        }
        else if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
        {
            //no need of account; login is success
            pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_LOGIN;
            (void)F_ftpcStateDoneLogin(pDcpt);
        }
        else
        {
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
        }

    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR; 
        }  
        else
        {
            res = TCPIP_FTPC_RES_PENDING;
        }        
    }        
    return res;
}

static TCPIP_FTPC_RESULT_TYPE F_ftpcStateSendAcct(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes = 0;
    
    nBytes = (uint16_t)FC_sprintf(pDcpt->ftpcCtrlTxBuff, sizeof(pDcpt->ftpcCtrlTxBuff), "ACCT %s\r\n", pDcpt->ftpcAccessControl.ftpcAccount);
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state         
        pDcpt->ftpcState = TCPIP_FTPC_STATE_LOGIN_WAIT_ACCT;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    } 
    pDcpt->ftpcCommand = TCPIP_FTPC_CMD_ACCT;
    return res;
}

static TCPIP_FTPC_RESULT_TYPE F_ftpcStateWaitAcct(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //230
    //202
    //530
    //500, 501, 503, 421
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
    //Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {   //process the control response  
        res =   ftpcParseReplyCode(pDcpt); 
        if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
        {
            //login is success
            pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_LOGIN;
            (void)F_ftpcStateDoneLogin(pDcpt);
        }
        else
        {
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
        }
    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR; 
        }  
        else
        {
            res = TCPIP_FTPC_RES_PENDING;
        }        
    }        
    return res;
}

//FTP Client State Machine: Mark the completion of Client-server Login process
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateDoneLogin(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    // Add flag to indicate FTP Client Logged In 
    pDcpt->ftpcFlag  |= (uint16_t)TCPIP_FTPC_FLAG_LOGGEDIN;    
    // trigger call back for control socket
    ftpcCtrlEvent(pDcpt,TCPIP_FTPC_CTRL_EVENT_SUCCESS, NULL,0);
    // returns the state machine to default home
    pDcpt->ftpcState = TCPIP_FTPC_STATE_HOME;
    return TCPIP_FTPC_RES_OK;     
}


////////////////////// CHANGE WORKING DIRECTORY ROUTINES ///////////////////////
//FTP Client State Machine: Send CWD Command to FTP server
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateSendCwd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes = 0;
    
    nBytes = (uint16_t)FC_sprintf(pDcpt->ftpcCtrlTxBuff, sizeof(pDcpt->ftpcCtrlTxBuff), "CWD %s\r\n", pDcpt->ftpcServerPathname);
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_CWD;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    }     
    return res;
}

//FTP Client State Machine: wait for response from FTP server, after CWD command
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateWaitCwd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{   
    //Response
    //250
    //500, 501, 502, 421, 530, 550    
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
    //Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {   //process the control response  
        res =   ftpcParseReplyCode(pDcpt); 
        if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
        {
            //no need of password; login is success
            (void)F_ftpcDoneCmd(pDcpt);
        }
        else
        {
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
        }
    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR; 
        }  
        else
        {
            res = TCPIP_FTPC_RES_PENDING;
        }        
    }        
    return res;
}

////////////////////// CHANGE TO PARENT DIRECTORY ROUTINES /////////////////////
//FTP Client State Machine: Send CDUP Command to FTP server
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateSendCdup(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes = 0;
    
    nBytes = (uint16_t)FC_sprintf(pDcpt->ftpcCtrlTxBuff, sizeof(pDcpt->ftpcCtrlTxBuff), "CDUP\r\n");
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_CDUP;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    }
    return res;   
}

//FTP Client State Machine: wait for response from FTP server, after CDUP command
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateWaitCdup(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //200
    //500, 501, 502, 421, 530, 550
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
    //Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {   //process the control response  
        res =   ftpcParseReplyCode(pDcpt); 
        if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
        {           
            //CDUP success
            (void)F_ftpcDoneCmd(pDcpt);
        }
        else
        {
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
        }
    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR; 
        }  
        else
        {
            res = TCPIP_FTPC_RES_PENDING;
        }        
    }
        
    return res;
}

/////////////////////////// MAKE DIRECTORY ROUTINES ////////////////////////////
//FTP Client State Machine: Send MKD Command to FTP server
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateSendMkd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes;
    
    nBytes = (uint16_t)FC_sprintf(pDcpt->ftpcCtrlTxBuff, sizeof(pDcpt->ftpcCtrlTxBuff), "MKD %s\r\n", pDcpt->ftpcServerPathname);
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_MKD;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    } 
    return res;    
}

//FTP Client State Machine: wait for response from FTP server, after MKD command
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateWaitMkd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //257
    //500, 501, 502, 421, 530, 550
    
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
    //Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {   //process the control response  
        res =   ftpcParseReplyCode(pDcpt); 
        if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
        {           
            //MKD success
            (void)F_ftpcDoneCmd(pDcpt);
        }
        else
        {
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
        }

    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR; 
        }  
        else
        {
            res = TCPIP_FTPC_RES_PENDING;
        }        
    }
        
    return res;
}

///////////////////////// REMOVE DIRECTORY ROUTINES ////////////////////////////
//FTP Client State Machine: Send RMD Command to FTP server
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateSendRmd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes;
    
    nBytes = (uint16_t)FC_sprintf(pDcpt->ftpcCtrlTxBuff, sizeof(pDcpt->ftpcCtrlTxBuff), "RMD %s\r\n", pDcpt->ftpcServerPathname);
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_RMD;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    }     
    return res;    
}

//FTP Client State Machine: wait for response from FTP server, after RMD command
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateWaitRmd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //250
    //500, 501, 502, 421, 530, 550
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
    //Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {   //process the control response  
        res =   ftpcParseReplyCode(pDcpt); 
        if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
        {           
            //RMD success
            (void)F_ftpcDoneCmd(pDcpt);
        }
        else
        {
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
        }

    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR; 
        }  
        else
        {
            res = TCPIP_FTPC_RES_PENDING;
        }        
    }
        
    return res;
}

///////////////////////// DELETE FILE ROUTINES ////////////////////////////
//FTP Client State Machine: Send DELE Command to FTP server
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateSendDele(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes;
    
    nBytes = (uint16_t)FC_sprintf(pDcpt->ftpcCtrlTxBuff, sizeof(pDcpt->ftpcCtrlTxBuff), "DELE %s\r\n", pDcpt->ftpcServerPathname);
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_DELE;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    }     
    return res;    
}

//FTP Client State Machine: wait for response from FTP server, after DELE command
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateWaitDele(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //250
    //450, 550
    //500, 501, 502, 421, 530
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
    //Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {   //process the control response  
        res =   ftpcParseReplyCode(pDcpt); 
        if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
        {           
            //RMD success
            (void)F_ftpcDoneCmd(pDcpt);
        }
        else
        {
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
        }

    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR; 
        }  
        else
        {
            res = TCPIP_FTPC_RES_PENDING;
        }        
    }
        
    return res;
}

////////////////////// LOGOUT ROUTINES /////////////////////
//FTP Client State Machine: Send QUIT Command to FTP server
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateSendQuit(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes;
    
    nBytes = (uint16_t)FC_sprintf(pDcpt->ftpcCtrlTxBuff, sizeof(pDcpt->ftpcCtrlTxBuff), "QUIT\r\n");
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_QUIT;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    }
    return res;   
}

//FTP Client State Machine: wait for response from FTP server, after QUIT command
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateWaitQuit(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //200
    //500, 501, 502, 421, 530, 550
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
    //Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {   //process the control response  
        res =   ftpcParseReplyCode(pDcpt); 
        if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
        {           
            //Quit success
            // Remove flag to indicate FTP Client Logged Out 
            pDcpt->ftpcFlag  &= ~(uint16_t)TCPIP_FTPC_FLAG_LOGGEDIN; 
            (void)F_ftpcDoneCmd(pDcpt);
        }
        else
        {
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
        }
    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR; 
        }  
        else
        {
            res = TCPIP_FTPC_RES_PENDING;
        }        
    }
        
    return res;
}

////////////////////// PRINT WORKING DIRECTORY ROUTINES ///////////////////////
//FTP Client State Machine: Send PWD Command to FTP server
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateSendPwd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes;
    
    nBytes = (uint16_t)FC_sprintf(pDcpt->ftpcCtrlTxBuff, sizeof(pDcpt->ftpcCtrlTxBuff), "PWD\r\n");
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_PWD;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    }
   
    return res;
}

//FTP Client State Machine: wait for response from FTP server, after PWD command
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateWaitPwd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{    //Response
    //257
    //500, 501, 502, 421, 550
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
    //Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {   //process the control response  
        res =   ftpcParseReplyCode(pDcpt); 
        if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
        {           
            //PWD success
            (void)F_ftpcDoneCmd(pDcpt);
        }
        else
        {
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
        }

    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR; 
        }  
        else
        {
            res = TCPIP_FTPC_RES_PENDING;
        }        
    }
        
    return res;
}

///////////////////////////// GET file ROUTINES ////////////////////////////
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateStartGet(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    pDcpt->filePos = 0;
    pDcpt->ftpcDataRxLen = 0;
    pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_TYPE;
    (void)F_ftpcStateSendType(pDcpt);
    return TCPIP_FTPC_RES_OK;
}
///////////////////////////// PUT file ROUTINES ////////////////////////////
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateStartPut(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    pDcpt->filePos = 0;    
    pDcpt->ftpcDataRxLen = 0;
    pDcpt->ftpcDataTxLen = 0;
    pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_TYPE;
    (void)F_ftpcStateSendType(pDcpt);
    return TCPIP_FTPC_RES_OK;
}

///////////////////////////// TYPE COMMAND ROUTINES ////////////////////////////
//FTP Client State Machine: Send TYPE Command to FTP server
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateSendType(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes;
    const char * type = NULL;
    
    if(pDcpt->ftpcDataConnection.ftpcDataType == TCPIP_FTPC_DATA_REP_ASCII)
    {
        type = "A";
    }
    else if(pDcpt->ftpcDataConnection.ftpcDataType == TCPIP_FTPC_DATA_REP_EBCDIC)
    {
        type = "E";  
    }
    else if(pDcpt->ftpcDataConnection.ftpcDataType == TCPIP_FTPC_DATA_REP_IMAGE)
    {
        type = "I";  
    }
    else
    {
        // do nothing
    }
    
    if (type != NULL)
    {
        nBytes = (uint16_t)FC_sprintf(pDcpt->ftpcCtrlTxBuff, sizeof(pDcpt->ftpcCtrlTxBuff), "TYPE %s\r\n", type);

        if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
        {   //switch to next state 
            pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_TYPE;
            //time out
            ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
        }
    }
       
    return res;
}

//FTP Client State Machine: wait for response from FTP server, after TYPE command
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateWaitType(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{    //Response
    //257
    //500, 501, 504, 421, 530
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
    //Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {   //process the control response  
        res =   ftpcParseReplyCode(pDcpt); 
        if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
        {           
            //TYPE success
            (void)F_ftpcStateDoneType(pDcpt);
        }
        else
        {
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
        }
    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR; 
        }  
        else
        {
            res = TCPIP_FTPC_RES_PENDING;
        }        
    }
        
    return res;
}

static TCPIP_FTPC_RESULT_TYPE F_ftpcStateDoneType(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    TCP_SOCKET_INFO ctrlSktInfo, dataSktInfo;
    
    if((pDcpt->ftpcCommand == TCPIP_FTPC_CMD_GET)|| (pDcpt->ftpcCommand == TCPIP_FTPC_CMD_PUT))
    {        
        if(pDcpt->ftpcDataConnection.ftpcIsPassiveMode)
        {
            //Send PASV command
            pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_PASV;
            (void)F_ftpcStateSendPasv(pDcpt);
        }
        else
        {
            //Open a Data socket, before PORT command
            res = F_ftpcStateOpenDataSkt(pDcpt);
            if(res == TCPIP_FTPC_RES_OK)
            {
                (void)NET_PRES_SocketInfoGet(pDcpt->ftpcSocket.ftpcCtrlSkt, &ctrlSktInfo);
                (void)NET_PRES_SocketInfoGet(pDcpt->ftpcSocket.ftpcDataSkt, &dataSktInfo);
                (void)memcpy(&(pDcpt->ftpcDataConnection.dataServerAddr.v4Add), 
                        &(ctrlSktInfo.localIPaddress.v4Add), sizeof(IPV4_ADDR));
                pDcpt->ftpcDataConnection.dataServerPort = dataSktInfo.localPort;
                
                pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_PORT;
                (void)F_ftpcStateSendPort(pDcpt);
            }
            else
            {
                // failed to open a socket
                pDcpt->error = TCPIP_FTPC_ERROR_OPEN_DATA_SOCKET; 
                F_ftpcErrorUpdate(pDcpt);
            }            
        }
    }
    else
    {
        //TYPE success notification
        ftpcCtrlEvent(pDcpt,TCPIP_FTPC_CTRL_EVENT_SUCCESS, NULL,0);
        // returns the state machine to default home
        pDcpt->ftpcState = TCPIP_FTPC_STATE_HOME;
    }
    
    return res;
}

///////////////////////// STRU ROUTINES ////////////////////////////
//FTP Client State Machine: Send STRU Command to FTP server
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateSendStru(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes;    
    const char * file_struct = NULL;
    
    if(pDcpt->ftpcDataConnection.ftpcDataStructure == TCPIP_FTPC_STRUCT_FILE)
    {
        file_struct = "F";
    }
    else if(pDcpt->ftpcDataConnection.ftpcDataStructure == TCPIP_FTPC_STRUCT_RECORD)
    {
        file_struct = "R";  
    }
    else if(pDcpt->ftpcDataConnection.ftpcDataStructure == TCPIP_FTPC_STRUCT_PAGE)
    {
        file_struct = "P"; 
    }
    else
    {
        // do nothing
    }
    
    nBytes = (uint16_t)FC_sprintf(pDcpt->ftpcCtrlTxBuff, sizeof(pDcpt->ftpcCtrlTxBuff), "STRU %s\r\n", file_struct);
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_STRU;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    }     
    return res;    
}

//FTP Client State Machine: wait for response from FTP server, after RMD command
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateWaitStru(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //200
    //500, 501, 504, 421, 530
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
    //Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {   //process the control response  
        res =   ftpcParseReplyCode(pDcpt); 
        if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
        {           
            //RMD success
            (void)F_ftpcDoneCmd(pDcpt);
        }
        else
        {
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
        }

    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR; 
        }  
        else
        {
            res = TCPIP_FTPC_RES_PENDING;
        }        
    }
        
    return res;
}

///////////////////////// MODE ROUTINES ////////////////////////////
//FTP Client State Machine: Send MODE Command to FTP server
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateSendMode(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes;    
    const char * mode = NULL;
    
    if(pDcpt->ftpcDataConnection.ftpcTransferMode == TCPIP_FTPC_TRANS_STREAM_MODE)
    {
        mode = "S";
    }
    else if(pDcpt->ftpcDataConnection.ftpcTransferMode == TCPIP_FTPC_TRANS_BLOCK_MODE)
    {
        mode = "B";  
    }
    else if(pDcpt->ftpcDataConnection.ftpcTransferMode == TCPIP_FTPC_TRANS_COMPRESS_MODE)
    {
        mode = "C"; 
    }
    else
    {
        // do nothing
    }
    
    nBytes = (uint16_t)FC_sprintf(pDcpt->ftpcCtrlTxBuff, sizeof(pDcpt->ftpcCtrlTxBuff), "MODE %s\r\n", mode);
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_MODE;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    }     
    return res;    
}

//FTP Client State Machine: wait for response from FTP server, after MODE command
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateWaitMode(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //200
    //500, 501, 504, 421, 530
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
    //Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {   //process the control response  
        res =   ftpcParseReplyCode(pDcpt); 
        if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
        {           
            //RMD success
            (void)F_ftpcDoneCmd(pDcpt);
        }
        else
        {
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
        }

    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR; 
        }  
        else
        {
            res = TCPIP_FTPC_RES_PENDING;
        }        
    }
        
    return res;
}

///////////////////////// PASV Command ROUTINES ////////////////////////////
//FTP Client State Machine: Send PASV Command to FTP server
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateSendPasv(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes;
    
    nBytes = (uint16_t)FC_sprintf(pDcpt->ftpcCtrlTxBuff, sizeof(pDcpt->ftpcCtrlTxBuff), "PASV\r\n");
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state         
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_PASV;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    }
    return res;
}

//FTP Client State Machine: wait for response from FTP server, after PASV command
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateWaitPasv(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{   //Response
    //227
    //500, 501, 502, 421, 530
    
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
    //Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {   //process the control response  
        res =   ftpcParseReplyCode(pDcpt); 
        if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
        {           
            //PASV command success
            (void)F_ftpcStateDonePasv(pDcpt);
        }
        else
        {
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
        }

    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR; 
        }  
        else
        {
            res = TCPIP_FTPC_RES_PENDING;
        }        
    }        
    return res;
}


static TCPIP_FTPC_RESULT_TYPE F_ftpcStateDonePasv(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{    
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK; 
    
    ftpc_process_pasv_response(pDcpt);    
    
    if((pDcpt->ftpcCommand == TCPIP_FTPC_CMD_GET)||(pDcpt->ftpcCommand == TCPIP_FTPC_CMD_PUT) || 
            (pDcpt->ftpcCommand == TCPIP_FTPC_CMD_NLST)|| (pDcpt->ftpcCommand == TCPIP_FTPC_CMD_LIST))
    {
        if(pDcpt->ftpcSocket.ftpcDataSkt == NET_PRES_INVALID_SOCKET)
        {   // get a new socket
            NET_PRES_SKT_HANDLE_T dataSkt;
            NET_PRES_SKT_T sktType =  NET_PRES_SKT_DEFAULT_STREAM_CLIENT;
            dataSkt = NET_PRES_SocketOpen(0, sktType, (NET_PRES_SKT_ADDR_T)pDcpt->ftpcDataConnection.dataServerIpAddrType, 
                                        pDcpt->ftpcDataConnection.dataServerPort, NULL, NULL);
            if(dataSkt != NET_PRES_INVALID_SOCKET)
            {   
                pDcpt->ftpcSocket.ftpcDataSkt = dataSkt;
                // alert of incoming traffic
                (void)NET_PRES_SocketSignalHandlerRegister(dataSkt, (uint16_t)TCPIP_TCP_SIGNAL_RX_DATA | (uint16_t)TCPIP_TCP_SIGNAL_RX_FIN, &F_FTPCDataSktRxSignalHandler, pDcpt);
                //set RX/TX buffers for Data Socket
                res = F_ftpcSetDataSktBuff(pDcpt);
                // point socket to the FTP server
                if(NET_PRES_SocketRemoteBind(pDcpt->ftpcSocket.ftpcDataSkt, 
                        (NET_PRES_SKT_ADDR_T)pDcpt->ftpcDataConnection.dataServerIpAddrType,  
                        pDcpt->ftpcDataConnection.dataServerPort, 
                        FC_MultiAdd2PresAdd(&(pDcpt->ftpcDataConnection.dataServerAddr))))
                {
                    if (NET_PRES_SocketConnect(pDcpt->ftpcSocket.ftpcDataSkt))
                    {
                        pDcpt->ftpcState = TCPIP_FTPC_STATE_PASV_WAIT_DATA_CONNECT;
                        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo); 
                        res = TCPIP_FTPC_RES_OK;
                    } 
                    else
                    {
                        pDcpt->error = TCPIP_FTPC_ERROR_CONNECT_DATA_SOCKET;
                        res = TCPIP_FTPC_RES_ERROR; 
                        F_ftpcCloseDataSkt(pDcpt);
                    }
                }
                else
                {   // failed
                    pDcpt->error = TCPIP_FTPC_ERROR_BIND_DATA_SOCKET;
                    res = TCPIP_FTPC_RES_ERROR; 
                    F_ftpcCloseDataSkt(pDcpt);
                }   
            }
            else
            {  
                // failed to open a socket
                pDcpt->error = TCPIP_FTPC_ERROR_OPEN_DATA_SOCKET;
                res = TCPIP_FTPC_RES_ERROR;
            }
        }      
    }
    else
    {
        //PASV success notification
        ftpcCtrlEvent(pDcpt,TCPIP_FTPC_CTRL_EVENT_SUCCESS, NULL,0);
        // returns the state machine to default home
        pDcpt->ftpcState = TCPIP_FTPC_STATE_HOME;
        res = TCPIP_FTPC_RES_OK;
    }   
    //Report the failure
    if(res !=TCPIP_FTPC_RES_OK)
    {
        F_ftpcErrorUpdate(pDcpt);   
    }
    return res;
}

//FTP Client State Machine: Wait for Client-server Data socket connection
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateWaitPasvDataConnect(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    
    if (NET_PRES_SocketIsConnected(pDcpt->ftpcSocket.ftpcDataSkt))
    {    
        if(pDcpt->ftpcCommand == TCPIP_FTPC_CMD_GET)
        {
            pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_RETR;
            (void)F_ftpcStateSendRetr(pDcpt);
        }
        else if (pDcpt->ftpcCommand == TCPIP_FTPC_CMD_PUT)
        {
            pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_STOR;
            (void)F_ftpcStateSendStor(pDcpt);
        }
        else if ((pDcpt->ftpcCommand == TCPIP_FTPC_CMD_NLST)||(pDcpt->ftpcCommand == TCPIP_FTPC_CMD_LIST))
        {
            pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_LST;
            (void)F_ftpcStateSendLst(pDcpt);
        } 
        else
        {
            // do nothing
        }
    }
    else
    {        
        if(ftpcIsDcptTmoExpired(pDcpt))
        { 
            //timeout == yes?
            F_ftpcCloseDataSkt(pDcpt);
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            F_ftpcErrorUpdate(pDcpt);
            res = TCPIP_FTPC_RES_ERROR;
        }
    } 
    return res;     
}


static void ftpc_process_pasv_response(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{    
    int i;
    char * buff; 
    int32_t temp32;
    char pasv_response[6];
    
    (void)memset(pasv_response, 0, sizeof(pasv_response));
    for(i = 6; i>0; i--)
    {
       buff = strrchr(pDcpt->ftpcCtrlRxBuff, (int)',');
       if(buff == NULL)
       {
           buff = strrchr(pDcpt->ftpcCtrlRxBuff, (int)'(');
           if(buff != NULL)
           {
               temp32 = 0;
               (void)FC_Str2L(buff + 1, 10, &temp32);         
               pasv_response[i-1] = (char)temp32;
           }
           break;
       }
       temp32 = 0;
       (void)FC_Str2L(buff + 1, 10, &temp32);
       pasv_response[i-1] = (char)temp32;
       *buff =  '\0';       
    }

    pDcpt->ftpcDataConnection.dataServerIpAddrType = IP_ADDRESS_TYPE_IPV4;
    pDcpt->ftpcDataConnection.dataServerPort  = ((uint16_t)pasv_response[4] * 256U) + (uint16_t)pasv_response[5];
    (void)memcpy(pDcpt->ftpcDataConnection.dataServerAddr.v4Add.v, (uint8_t*)pasv_response, sizeof(IPV4_ADDR));
    
}

///////////////////////// PORT Command ROUTINES ////////////////////////////
//FTP Client State Machine: Send PORT Command to FTP server
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateSendPort(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes;

    nBytes = (uint16_t)FC_sprintf(pDcpt->ftpcCtrlTxBuff, sizeof(pDcpt->ftpcCtrlTxBuff), "PORT %d,%d,%d,%d,%d,%d\r\n", 
                            pDcpt->ftpcDataConnection.dataServerAddr.v4Add.v[0],
                            pDcpt->ftpcDataConnection.dataServerAddr.v4Add.v[1],
                            pDcpt->ftpcDataConnection.dataServerAddr.v4Add.v[2],
                            pDcpt->ftpcDataConnection.dataServerAddr.v4Add.v[3],
                            (uint8_t)((pDcpt->ftpcDataConnection.dataServerPort >> 8) & 0xFFU),
                            (uint8_t)((pDcpt->ftpcDataConnection.dataServerPort) & 0xFFU) );
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state  
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_PORT;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    } 
    return res;
}

//FTP Client State Machine: wait for response from FTP server, after PORT command
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateWaitPort(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{    //Response
    //200
    //500, 501, 421, 530
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
    //Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {   //process the control response  
        res =   ftpcParseReplyCode(pDcpt); 
        if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
        {           
            //PORT success
            (void)F_ftpcStateDonePort(pDcpt);
        }
        else
        {
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
        }

    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;             
        }  
        else
        {
            res = TCPIP_FTPC_RES_PENDING;
        }        
    }        
    return res;
}

//FTP Client State Machine: Mark the completion of PORT Command
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateDonePort(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{    
    if((pDcpt->ftpcCommand == TCPIP_FTPC_CMD_GET))
    {
        pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_RETR;
        (void)F_ftpcStateSendRetr(pDcpt);
    }
    else if (pDcpt->ftpcCommand == TCPIP_FTPC_CMD_PUT)
    {
        pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_STOR;
        (void)F_ftpcStateSendStor(pDcpt);
    }
    else if ((pDcpt->ftpcCommand == TCPIP_FTPC_CMD_NLST)||(pDcpt->ftpcCommand == TCPIP_FTPC_CMD_LIST))
    {
        pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_LST;
        (void)F_ftpcStateSendLst(pDcpt);
    }
    else
    {
        //PORT success notification
        ftpcCtrlEvent(pDcpt,TCPIP_FTPC_CTRL_EVENT_SUCCESS, NULL,0);
        // returns the state machine to default home
        pDcpt->ftpcState = TCPIP_FTPC_STATE_HOME;        
    }    
    return TCPIP_FTPC_RES_OK;
}


///////////////////////// RETR Command ROUTINES ////////////////////////////
//FTP Client State Machine: Send RETR Command to FTP server
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateSendRetr(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes;
    
    nBytes = (uint16_t)FC_sprintf(pDcpt->ftpcCtrlTxBuff, sizeof(pDcpt->ftpcCtrlTxBuff), "RETR %s\r\n", pDcpt->ftpcServerPathname);
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state                
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_RETR_CTRL_RESPONSE;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    } 
    return res;  
}

//FTP Client State Machine: wait for control response from FTP server, after RETR command
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateWaitRetrCtrlResp(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //125,150,(110)
    //226,250
    //450,550
    //500, 501, 421, 530
    
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
    //Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);

    if (res == TCPIP_FTPC_RES_OK)
    {   
        //process the control response  
        res =   ftpcParseReplyCode(pDcpt);            
        if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
        {
            pDcpt->ftpcState = TCPIP_FTPC_STATE_RETR_EOF; 
            (void)F_ftpcStateRetrEoF(pDcpt);
        }
        else if(res == TCPIP_FTPC_SERVER_POS_PRELIM_RESPONSE)
        {     
            //Read Data socket
            pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_RETR_READ_DATA_SOCKET;
            (void)F_ftpcStateWaitRetrReadDataSkt(pDcpt);
        }  
        else
        {
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
        }
    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {   //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR; 
        }
        else
        {
            res = TCPIP_FTPC_RES_PENDING;
        } 
        
    }    
    return res;
}

//FTP Client State Machine: wait for data from FTP server, after RETR command
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateWaitRetrReadDataSkt(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //125,150,(110)
    //226,250
    //450,550
    //500, 501, 421, 530    
    
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;    
    uint16_t readLen;    

    //Read Data Socket
    if(ftpcReadDataSocket(pDcpt, &readLen) != TCPIP_FTPC_RES_ERROR)
    {
        //Read Control Socket
        res = ftpcReadCtrlSocket(pDcpt);
        if (res == TCPIP_FTPC_RES_OK)
        {   //process the control response  
            res =   ftpcParseReplyCode(pDcpt); 
            
            if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
            {
                pDcpt->ftpcState = TCPIP_FTPC_STATE_RETR_EOF; 
                (void)F_ftpcStateRetrEoF(pDcpt);
            }
            else
            {
                pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
                F_ftpcErrorUpdate(pDcpt);            
                res = TCPIP_FTPC_RES_ERROR;
            }
        }   
    }
   
    return res; 
}

//FTP Client State Machine: detect end-of-file data from FTP server, after RETR command
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateRetrEoF(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    uint16_t readLen = 0;
    
    //Read Data Socket
    if(ftpcReadDataSocket(pDcpt, &readLen) != TCPIP_FTPC_RES_ERROR)
    {    
        if((pDcpt->ftpcSignal & (uint16_t)TCPIP_FTPC_SIGNAL_DATA_RX_FIN ) != 0U)
        {
            while(readLen != 0U)
            {
                (void)ftpcReadDataSocket(pDcpt, &readLen);
            }
            if(pDcpt->fileDescr != SYS_FS_HANDLE_INVALID)
            { 
                //close the file
                (void)SYS_FS_FileClose(pDcpt->fileDescr); 
            }
            pDcpt->ftpcDataLength = 0;
            (void)ftpcDataEvent(pDcpt,TCPIP_FTPC_DATA_RCV_DONE, pDcpt->ftpcDataRxBuff, &(pDcpt->ftpcDataLength));
            res = F_ftpcDoneFileTransfer(pDcpt);      
            pDcpt->ftpcSignal &= ~(uint16_t)TCPIP_FTPC_SIGNAL_DATA_RX_FIN;
        }    
    }
    return res; 
}

////////////////////////////// STOR COMMAND ROUTINES ///////////////////////////
//FTP Client State Machine: Send STOR Command to FTP server
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateSendStor(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes;
    const char * pathname;
    
    if(pDcpt->ftpcServerPathname != NULL)
    {
        pathname = pDcpt->ftpcServerPathname;
    }
    else
    { 
        pathname = strrchr(pDcpt->ftpcClientPathname, (int)'/');
        if(pathname != NULL)
        {
            pathname = pDcpt->ftpcClientPathname;
        }
    }

    if(pDcpt->ftpcFileOptions.store_unique)
    {
        nBytes = (uint16_t)FC_sprintf(pDcpt->ftpcCtrlTxBuff, sizeof(pDcpt->ftpcCtrlTxBuff), "STOU %s\r\n", pathname);
    }
    else
    {
        nBytes = (uint16_t)FC_sprintf(pDcpt->ftpcCtrlTxBuff, sizeof(pDcpt->ftpcCtrlTxBuff), "STOR %s\r\n", pathname);
    }

    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_STOR_CTRL_RESPONSE;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    }   
    
    return res;    
}

//FTP Client State Machine: wait for response from FTP server, after STOR command
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateWaitStorCtrlResp(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //125,150,(110)
    //226,250
    //450,550
    //500, 501, 421, 530
    
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
    //Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    
    //Process Control Socket response and switch the state
    if (res == TCPIP_FTPC_RES_OK)
    {   //process the control response  
        res =   ftpcParseReplyCode(pDcpt); 
        if((res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)||(res == TCPIP_FTPC_SERVER_POS_PRELIM_RESPONSE))
        {
            //Read Data socket
            pDcpt->ftpcState = TCPIP_FTPC_STATE_STOR_WRITE_DATA_SOCKET;
            (void)F_ftpcStateStorWriteDataSkt(pDcpt);
            
        }
        else
        {
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
            F_ftpcCloseDataSkt(pDcpt);
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
        }
    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {   //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            F_ftpcCloseDataSkt(pDcpt);
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR; 
        }
        else
        {
            res = TCPIP_FTPC_RES_PENDING;
        }         
    } 
    return res; 
}

//FTP Client State Machine: Transfer file to FTP server till the end-of-file, after STOR command
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateStorWriteDataSkt(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //read file
    uint16_t   fileReadLen, writeLen, writeReadyLen;
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
    //Transfer File thru data socket, until end of file
    if (NET_PRES_SocketIsConnected(pDcpt->ftpcSocket.ftpcDataSkt))
    {
        writeReadyLen = NET_PRES_SocketWriteIsReady(pDcpt->ftpcSocket.ftpcDataSkt, pDcpt->ftpcDataConnection.ftpcDataTxBuffSize, 0);
        if(writeReadyLen > 0U)
        {
            pDcpt->ftpcDataLength = writeReadyLen;
            // Data Event
            if(!ftpcDataEvent(pDcpt,TCPIP_FTPC_DATA_SEND_READY,pDcpt->ftpcDataTxBuff, &(pDcpt->ftpcDataLength)))
            {
                if(pDcpt->fileDescr == SYS_FS_HANDLE_INVALID)
                {
                    pDcpt->fileDescr = SYS_FS_FileOpen(pDcpt->ftpcClientPathname,SYS_FS_FILE_OPEN_READ);
                    if( pDcpt->fileDescr == SYS_FS_HANDLE_INVALID )
                    {
                        pDcpt->error = TCPIP_FTPC_ERROR_FILE_OPEN;
                        F_ftpcCloseDataSkt(pDcpt);
                        F_ftpcErrorUpdate(pDcpt);            
                        res = TCPIP_FTPC_RES_ERROR;
                    }
                    
                }   
                // not reached end-of-file
                if(SYS_FS_FileEOF(pDcpt->fileDescr ) == false)
                {
                    // no file error
                    if(SYS_FS_Error() == SYS_FS_ERROR_OK)
                    {
                        // not end of file
                        fileReadLen = (uint16_t)SYS_FS_FileRead(pDcpt->fileDescr,pDcpt->ftpcDataTxBuff, (size_t)writeReadyLen);
                        if(fileReadLen > 0U)
                        {
                            writeLen =  NET_PRES_SocketWrite(pDcpt->ftpcSocket.ftpcDataSkt, pDcpt->ftpcDataTxBuff, fileReadLen);
                            pDcpt->ftpcDataTxLen += writeLen;
                        }
                    }
                    else
                    {
                        pDcpt->error = TCPIP_FTPC_ERROR_FILE_ACCESS;
                        F_ftpcCloseDataSkt(pDcpt);
                        F_ftpcErrorUpdate(pDcpt);            
                        res = TCPIP_FTPC_RES_ERROR;
                    }
                }
                else
                {
                    //end of file; close file
                    (void)SYS_FS_FileClose(pDcpt->fileDescr);  
                    //close data socket
                    F_ftpcCloseDataSkt(pDcpt);
                    pDcpt->ftpcDataLength = 0;
                    (void)ftpcDataEvent(pDcpt,TCPIP_FTPC_DATA_SEND_DONE,pDcpt->ftpcDataTxBuff, &(pDcpt->ftpcDataLength));
                    pDcpt->ftpcState = TCPIP_FTPC_STATE_STOR_EOF;
                    res = F_ftpcStateStorEoF(pDcpt); 
                }
            }
            else
            {
                if(pDcpt->ftpcDataLength > 0U)
                {
                    writeLen =  NET_PRES_SocketWrite(pDcpt->ftpcSocket.ftpcDataSkt, pDcpt->ftpcDataTxBuff, pDcpt->ftpcDataLength);
                    pDcpt->ftpcDataTxLen += writeLen;
                }
                else
                {
                    F_ftpcCloseDataSkt(pDcpt);
                    pDcpt->ftpcDataLength = 0;
                    (void)ftpcDataEvent(pDcpt,TCPIP_FTPC_DATA_SEND_DONE,pDcpt->ftpcDataTxBuff, &(pDcpt->ftpcDataLength));
                    pDcpt->ftpcState = TCPIP_FTPC_STATE_STOR_EOF;
                    res = F_ftpcStateStorEoF(pDcpt);
                }
            }
        }
    }       
    return res; 
}

static TCPIP_FTPC_RESULT_TYPE F_ftpcStateStorEoF(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{   
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
   
    //Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {   //process the control response  
        res =   ftpcParseReplyCode(pDcpt); 
        if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
        {            
            res = F_ftpcDoneFileTransfer(pDcpt);
        }
        else
        {
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
            F_ftpcCloseDataSkt(pDcpt);
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
        }
    }    
    return res;
}

////////////////////////////// NLST/LIST COMMAND ROUTINES ///////////////////////////
//FTP Client State Machine: Send NLST/LIST Command to FTP server
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateStartLst(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    TCP_SOCKET_INFO ctrlSktInfo, dataSktInfo;
    pDcpt->filePos = 0;
    pDcpt->ftpcDataRxLen = 0;
    
    if(pDcpt->ftpcDataConnection.ftpcIsPassiveMode)
    {
        pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_PASV;
        (void)F_ftpcStateSendPasv(pDcpt);
    }
    else
    {        
        res = F_ftpcStateOpenDataSkt(pDcpt);
        if(res == TCPIP_FTPC_RES_OK)
        {
            (void)NET_PRES_SocketInfoGet(pDcpt->ftpcSocket.ftpcCtrlSkt, &ctrlSktInfo);
            (void)NET_PRES_SocketInfoGet(pDcpt->ftpcSocket.ftpcDataSkt, &dataSktInfo);
            (void)memcpy(&(pDcpt->ftpcDataConnection.dataServerAddr.v4Add), 
                        &(ctrlSktInfo.localIPaddress.v4Add), sizeof(IPV4_ADDR));
            pDcpt->ftpcDataConnection.dataServerPort = dataSktInfo.localPort;
            
            pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_PORT;
            (void)F_ftpcStateSendPort(pDcpt);
        }
        else
        {
            // failed to open a socket
            pDcpt->error = TCPIP_FTPC_ERROR_OPEN_DATA_SOCKET; 
            F_ftpcErrorUpdate(pDcpt);
        }
    }
    return res;
}


static TCPIP_FTPC_RESULT_TYPE F_ftpcStateSendLst(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes;
    
    if (pDcpt->ftpcCommand == TCPIP_FTPC_CMD_NLST)
    {
        if(pDcpt->ftpcServerPathname != NULL)        
        {
            nBytes = (uint16_t)FC_sprintf(pDcpt->ftpcCtrlTxBuff, sizeof(pDcpt->ftpcCtrlTxBuff), "NLST %s\r\n", pDcpt->ftpcServerPathname);
        }
        else
        {
            nBytes = (uint16_t)FC_sprintf(pDcpt->ftpcCtrlTxBuff, sizeof(pDcpt->ftpcCtrlTxBuff), "NLST\r\n");
        }
    }
    else
    {
        if(pDcpt->ftpcServerPathname != NULL)   
        {
            nBytes = (uint16_t)FC_sprintf(pDcpt->ftpcCtrlTxBuff, sizeof(pDcpt->ftpcCtrlTxBuff), "LIST %s\r\n", pDcpt->ftpcServerPathname);
        }
        else
        {
            nBytes = (uint16_t)FC_sprintf(pDcpt->ftpcCtrlTxBuff, sizeof(pDcpt->ftpcCtrlTxBuff), "LIST\r\n");
        }
    }
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state   
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_LST_CTRL_RESPONSE;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    }     
    return res;  
}


static TCPIP_FTPC_RESULT_TYPE F_ftpcStateWaitLstCtrlResp(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //125,150,(110)
    //226,250
    //450,550
    //500, 501, 421, 530
    
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
    //Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    
    //Process Control Socket response and switch the state    
    if (res == TCPIP_FTPC_RES_OK)
    {   
        //process the control response  
        res =   ftpcParseReplyCode(pDcpt); 
        if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
        {
            pDcpt->ftpcState = TCPIP_FTPC_STATE_LST_EOT; 
            (void)F_ftpcStateLstEoT(pDcpt); 
        }
        else if (res == TCPIP_FTPC_SERVER_POS_PRELIM_RESPONSE)
        {
            //Read Data socket
            pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_LST_READ_DATA_SOCKET;
            (void)F_ftpcStateWaitLstReadDataSkt(pDcpt); 
        }
        else
        {
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
            F_ftpcCloseDataSkt(pDcpt);
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
        }
    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {   //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            F_ftpcCloseDataSkt(pDcpt);
            F_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR; 
        }
        else
        {
            res = TCPIP_FTPC_RES_PENDING;
        }         
    }    
    return res; 
}

static TCPIP_FTPC_RESULT_TYPE F_ftpcStateWaitLstReadDataSkt(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //125,150,(110)
    //226,250
    //450,550
    //500, 501, 421, 530    
    
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    uint16_t readLen;
    
    //Read Data Socket
    if(ftpcReadDataSocket(pDcpt, &readLen) != TCPIP_FTPC_RES_ERROR)
    {
        //Read Control Socket
        res = ftpcReadCtrlSocket(pDcpt);

        if (res == TCPIP_FTPC_RES_OK)
        {   //process the control response  
            res =   ftpcParseReplyCode(pDcpt); 
            if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
            {
                pDcpt->ftpcState = TCPIP_FTPC_STATE_LST_EOT; 
                (void)F_ftpcStateLstEoT(pDcpt);
            }
            else
            {
                pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
                F_ftpcErrorUpdate(pDcpt);            
                res = TCPIP_FTPC_RES_ERROR;
            }
        }  
    }
    return res; 
}

//FTP Client State Machine: detect end-of-transfer of data from FTP server, after NLST/LIST command
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateLstEoT(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    uint16_t readLen = 0;     
    
    //Read Data Socket
    if(ftpcReadDataSocket(pDcpt, &readLen) != TCPIP_FTPC_RES_ERROR)
    {     
        if((pDcpt->ftpcSignal & (uint16_t)TCPIP_FTPC_SIGNAL_DATA_RX_FIN ) != 0U)
        {
            while(readLen != 0U)
            {
                (void)ftpcReadDataSocket(pDcpt, &readLen);
            }
            
            if(pDcpt->fileDescr != SYS_FS_HANDLE_INVALID)
            { 
                //close the file
                (void)SYS_FS_FileClose(pDcpt->fileDescr); 
            }
            pDcpt->ftpcDataLength = 0;
            (void)ftpcDataEvent(pDcpt,TCPIP_FTPC_DATA_RCV_DONE,pDcpt->ftpcDataRxBuff, &(pDcpt->ftpcDataLength));
            res = F_ftpcDoneFileTransfer(pDcpt);
            pDcpt->ftpcSignal &= ~(uint16_t)TCPIP_FTPC_SIGNAL_DATA_RX_FIN;
        }
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// FTPC Helper Routines ///////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Parse the control socket response
static TCPIP_FTPC_RESULT_TYPE ftpcParseReplyCode(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;   
    int32_t serverReplyCode;
    
    serverReplyCode = 0;
    (void)FC_Str2L(pDcpt->ftpcCtrlRxBuff, 10, &serverReplyCode);
    
    if(FTPC_IS_REPLY_1YZ(serverReplyCode))
    {
        //preliminary positive response
        res = TCPIP_FTPC_SERVER_POS_PRELIM_RESPONSE;
        
    }
    else if(FTPC_IS_REPLY_2YZ(serverReplyCode))
    {
        //command success
        res = TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE;
    }
    else if(FTPC_IS_REPLY_3YZ(serverReplyCode))
    {
        //intermediate response; need more commands from client
        res = TCPIP_FTPC_SERVER_POS_INTRMDT_RESPONSE;
    }
    else if(FTPC_IS_REPLY_4YZ(serverReplyCode))
    {
        //error, transient negative response
        res = TCPIP_FTPC_SERVER_NEG_TRANS_RESPONSE;
    }
    else if(FTPC_IS_REPLY_5YZ(serverReplyCode))
    {
        //error, permanent negative response
        res = TCPIP_FTPC_SERVER_NEG_COMPL_RESPONSE;
    }
    else
    {
        //error, illegal response
        res = TCPIP_FTPC_SERVER_ILLEGAL_RESPONSE;
    }
    
    return res;
}

// sets the waitTick to the timeout value (in seconds!)
static void ftpcSetDcptTmo(TCPIP_FTPC_DCPT_TYPE* pDcpt, uint32_t tmo)
{
    uint32_t sysFreq = SYS_TMR_TickCounterFrequencyGet(); 
    pDcpt->waitTick = SYS_TMR_TickCountGet() + (tmo * sysFreq);
}

// returns true if timeout
static bool ftpcIsDcptTmoExpired(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    return ((int32_t)SYS_TMR_TickCountGet() - (int32_t)pDcpt->waitTick) >= 0;
}

// Open a Data Socket for a PORT command
static TCPIP_FTPC_RESULT_TYPE F_ftpcStateOpenDataSkt(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    
    if(pDcpt->ftpcSocket.ftpcDataSkt == NET_PRES_INVALID_SOCKET)
    {   // get a new socket
        NET_PRES_SKT_HANDLE_T dataSkt;
        NET_PRES_SKT_T sktType =  NET_PRES_SKT_DEFAULT_STREAM_SERVER;
        dataSkt = NET_PRES_SocketOpen(0, sktType, (NET_PRES_SKT_ADDR_T)pDcpt->ftpcCtrlConnection.ftpcServerIpAddrType, 0, NULL, NULL);

        if(dataSkt != NET_PRES_INVALID_SOCKET)
        {   
            pDcpt->ftpcSocket.ftpcDataSkt = dataSkt;
            // alert of incoming traffic
            (void)NET_PRES_SocketSignalHandlerRegister(dataSkt, (uint16_t)TCPIP_TCP_SIGNAL_RX_DATA | (uint16_t)TCPIP_TCP_SIGNAL_RX_FIN, &F_FTPCDataSktRxSignalHandler, pDcpt);
            res = F_ftpcSetDataSktBuff(pDcpt);
        }
        else
        {             
            // failed to open a socket           
            res = TCPIP_FTPC_RES_ERROR;
        }
    }   
    return res;
}

// Read Control Socket receive data
static TCPIP_FTPC_RESULT_TYPE ftpcReadCtrlSocket(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{    
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    uint16_t readLen;
    
    pDcpt->ftpcCtrlRxLen = 0;
    if (NET_PRES_SocketIsConnected(pDcpt->ftpcSocket.ftpcCtrlSkt))
    {
        if((readLen = NET_PRES_SocketReadIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt)) != 0U)  
        {        
            if((pDcpt->ftpcCtrlRxLen = NET_PRES_SocketRead(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlRxBuff, readLen)) != 0U)
            {   
                ftpcCtrlEvent(pDcpt,TCPIP_FTPC_CTRL_RCV,pDcpt->ftpcCtrlRxBuff, readLen);
                res = TCPIP_FTPC_RES_OK;      
            }
        }
    }
    return res;    
}

// Write to Control Socket 
static TCPIP_FTPC_RESULT_TYPE ftpcWriteCtrlSocket(TCPIP_FTPC_DCPT_TYPE* pDcpt, uint16_t writeByte_count)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    
    pDcpt->ftpcCtrlTxLen = 0; 
    if(NET_PRES_SocketWriteIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt, writeByte_count, 0) > 0U)
    {
        ftpcCtrlEvent(pDcpt,TCPIP_FTPC_CTRL_SEND,pDcpt->ftpcCtrlTxBuff, writeByte_count);
        pDcpt->ftpcCtrlTxLen = NET_PRES_SocketWrite(pDcpt->ftpcSocket.ftpcCtrlSkt, 
                                                pDcpt->ftpcCtrlTxBuff, writeByte_count); 
    }
    else
    {
        res = TCPIP_FTPC_RES_PENDING;
    }
    return res;
}


// Read Data Socket receive data and write it to file system
static TCPIP_FTPC_RESULT_TYPE ftpcReadDataSocket(TCPIP_FTPC_DCPT_TYPE* pDcpt, uint16_t * rxLen)
{   
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    SYS_FS_HANDLE fileDescr;
    uint16_t readLen;
    
    if (NET_PRES_SocketIsConnected(pDcpt->ftpcSocket.ftpcDataSkt))
    {
        readLen = NET_PRES_SocketReadIsReady(pDcpt->ftpcSocket.ftpcDataSkt);
        if( readLen  > 0U)  
        {  
            
            if((readLen = NET_PRES_SocketRead(pDcpt->ftpcSocket.ftpcDataSkt, pDcpt->ftpcDataRxBuff, readLen)) > 0U)
            {
                pDcpt->ftpcDataLength = readLen;
                if(!ftpcDataEvent(pDcpt,TCPIP_FTPC_DATA_RCV,pDcpt->ftpcDataRxBuff,&(pDcpt->ftpcDataLength)))
                {  
                    if(pDcpt->fileDescr == SYS_FS_HANDLE_INVALID)
                    {   
                        //file name for storing on FTP Client
                        if(pDcpt->ftpcClientPathname != NULL)
                        {
                            pDcpt->filePathname = pDcpt->ftpcClientPathname;
                        }
                        else
                        {
                            pDcpt->filePathname = strrchr(pDcpt->ftpcServerPathname, (int)'/');
                            if(pDcpt->filePathname == NULL)
                            {
                                pDcpt->filePathname = pDcpt->ftpcServerPathname;
                            }
                        }                        
                        
                        //open file
                        fileDescr = SYS_FS_FileOpen(pDcpt->filePathname,SYS_FS_FILE_OPEN_WRITE);
                        if( fileDescr != SYS_FS_HANDLE_INVALID )
                        {
                            pDcpt->fileDescr = fileDescr;
                            res = TCPIP_FTPC_RES_OK;
                        }
                        else
                        {
                            pDcpt->error = TCPIP_FTPC_ERROR_FILE_OPEN;
                            F_ftpcCloseDataSkt(pDcpt);
                            F_ftpcErrorUpdate(pDcpt);            
                            res = TCPIP_FTPC_RES_ERROR;
                        }                        
                    }                    
                
                    if(pDcpt->fileDescr != SYS_FS_HANDLE_INVALID)
                    {
                        if((int32_t)pDcpt->filePos > 0)
                        {
                            (void)SYS_FS_FileSeek(pDcpt->fileDescr,(int32_t)pDcpt->filePos,SYS_FS_SEEK_SET);
                        }
                        if(SYS_FS_FileWrite(pDcpt->fileDescr,pDcpt->ftpcDataRxBuff,readLen) == SYS_FS_HANDLE_INVALID)
                        {
                            (void)SYS_FS_FileClose(pDcpt->fileDescr);
                            pDcpt->error = TCPIP_FTPC_ERROR_FILE_WRITE;
                            F_ftpcCloseDataSkt(pDcpt);
                            F_ftpcErrorUpdate(pDcpt); 
                            res = TCPIP_FTPC_RES_ERROR;
                        }
                        else
                        {
                            res = TCPIP_FTPC_RES_OK;
                        }

                        pDcpt->filePos = SYS_FS_FileTell(pDcpt->fileDescr);
                    }   
                }
                else
                {
                    res = TCPIP_FTPC_RES_OK;
                }
            }
        }
        *rxLen = readLen;
        pDcpt->ftpcDataRxLen += readLen;
    }
    return res;    
}

//FTP Client Command completed
static TCPIP_FTPC_RESULT_TYPE F_ftpcDoneCmd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{  
    //Command Success notification
    ftpcCtrlEvent(pDcpt,TCPIP_FTPC_CTRL_EVENT_SUCCESS, NULL,0);
    
    // returns the state machine to default home
    pDcpt->ftpcState = TCPIP_FTPC_STATE_HOME;
    return TCPIP_FTPC_RES_OK;
}

//FTP Client data transfer command completed
static TCPIP_FTPC_RESULT_TYPE F_ftpcDoneFileTransfer(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{   
    F_ftpcCloseDataSkt(pDcpt);
    //discard any data in control socket
    (void)NET_PRES_SocketDiscard(pDcpt->ftpcSocket.ftpcCtrlSkt);
    pDcpt->filePathname = NULL;
    ftpcCtrlEvent(pDcpt,TCPIP_FTPC_CTRL_EVENT_SUCCESS, NULL,0);
    // returns the state machine to default home
    pDcpt->ftpcState = TCPIP_FTPC_STATE_HOME;
    return TCPIP_FTPC_RES_OK;
}

//trigger callback on FTPC error
static void F_ftpcErrorUpdate(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{ 
    // trigger call back for control socket
    ftpcCtrlEvent(pDcpt,TCPIP_FTPC_CTRL_EVENT_FAILURE, NULL,0);
    // returns the state machine to default home
    pDcpt->ftpcState = TCPIP_FTPC_STATE_HOME; 
    SYS_CONSOLE_PRINT("FTPC ERROR : %d\r\n", pDcpt->error);
}

// Disconnect Control socket and remove the descriptor
static void F_ftpcCtrlDisconnect(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{       
    if(pDcpt != NULL)
    {
        OSAL_CRITSECT_DATA_TYPE ftpcLock = ftpcEnterCritical();    
        (void)TCPIP_Helper_SingleListNodeRemove(&ftpcDcptBusyList, FC_FtpcDcpt2SglNode(pDcpt));
        ftpcExitCritical(ftpcLock);

        if(pDcpt->ftpcSocket.ftpcCtrlSkt != NET_PRES_INVALID_SOCKET)
        {
            //close control socket
            NET_PRES_SocketClose(pDcpt->ftpcSocket.ftpcCtrlSkt);
            pDcpt->ftpcSocket.ftpcCtrlSkt = NET_PRES_INVALID_SOCKET;
            
        }
        // Free Control Socket Buffers
        (void)TCPIP_HEAP_Free(ftpcGlobalConfig.memH,pDcpt->ftpcCtrlRxBuff);
        (void)TCPIP_HEAP_Free(ftpcGlobalConfig.memH,pDcpt->ftpcCtrlTxBuff);
        
        //Disconnect notification
        ftpcCtrlEvent(pDcpt,TCPIP_FTPC_CTRL_EVENT_DISCONNECTED, NULL,0);

        pDcpt->ftpcFlag = (uint16_t)TCPIP_FTPC_FLAG_NONE;
        //clear callback for control connection
        pDcpt->ctrlSktCallback = NULL;
        // returns the state machine to default home
        pDcpt->ftpcState = TCPIP_FTPC_STATE_HOME;

        ftpcLock = ftpcEnterCritical();
        TCPIP_Helper_SingleListTailAdd(&ftpcDcptFreeList, FC_FtpcDcpt2SglNode(pDcpt));
        ftpcExitCritical(ftpcLock); 
    }    
}

static void F_ftpcCloseDataSkt(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    if(pDcpt->ftpcSocket.ftpcDataSkt != NET_PRES_INVALID_SOCKET)
    {
        //close data socket
        NET_PRES_SocketClose(pDcpt->ftpcSocket.ftpcDataSkt);
        pDcpt->ftpcSocket.ftpcDataSkt = NET_PRES_INVALID_SOCKET;
    }
    if(pDcpt->ftpcDataConnection.ftpcDataTxBuffSize != 0U)
    {
        (void)TCPIP_HEAP_Free(ftpcGlobalConfig.memH,pDcpt->ftpcDataTxBuff);
        pDcpt->ftpcDataConnection.ftpcDataTxBuffSize = 0U;
    }
    if(pDcpt->ftpcDataConnection.ftpcDataRxBuffSize != 0U)
    {
        (void)TCPIP_HEAP_Free(ftpcGlobalConfig.memH,pDcpt->ftpcDataRxBuff);
        pDcpt->ftpcDataConnection.ftpcDataRxBuffSize = 0U;
    }
}

static void F_ftpcDcptSetDefault(TCPIP_FTPC_DCPT_TYPE* pDcpt, size_t index)
{
    pDcpt->ftpcSocket.ftpcCtrlSkt = NET_PRES_INVALID_SOCKET; 
    pDcpt->ftpcSocket.ftpcDataSkt = NET_PRES_INVALID_SOCKET; 
    pDcpt->fileDescr = SYS_FS_HANDLE_INVALID;
    pDcpt->ftpcDcptIndex = (uint16_t)index;
    pDcpt->error = TCPIP_FTPC_ERROR_NONE;
    pDcpt->ftpcFlag  = (uint16_t)TCPIP_FTPC_FLAG_NONE;
    pDcpt->ftpcState = TCPIP_FTPC_STATE_HOME;
    pDcpt->ftpcCommand = TCPIP_FTPC_CMD_NONE;
    pDcpt->dataSktCallback = NULL;
    pDcpt->ctrlSktCallback = NULL;
    pDcpt->filePos = 0;
    pDcpt->ftpcSignal = (uint16_t)TCPIP_FTPC_SIGNAL_NONE;
    pDcpt->waitTick = 0;
    pDcpt->ftpcActive = false; 
    (void)memset(&(pDcpt->ftpcDataConnection), 0, sizeof(TCPIP_FTPC_DATA_CONN_TYPE));
}

static TCPIP_FTPC_RESULT_TYPE F_ftpcSetDataSktBuff(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    uint16_t sktRxSize = 0U;
    uint16_t sktTxSize = 0U;
    
    if(pDcpt->ftpcDataConnection.ftpcDataTxBuffSize != 0U)
    {
        //set the socket Tx buffer size
        void* tcpBuffSize = FC_Uint2VPtr((uint32_t)pDcpt->ftpcDataConnection.ftpcDataTxBuffSize);
        (void)NET_PRES_SocketOptionsSet(pDcpt->ftpcSocket.ftpcDataSkt, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_TX_BUFF, tcpBuffSize); 
    }
    else
    {
        if(ftpcGlobalConfig.data_tx_buffsize_dflt != 0U)
        {
            pDcpt->ftpcDataConnection.ftpcDataTxBuffSize = ftpcGlobalConfig.data_tx_buffsize_dflt ;
            //set the socket Tx buffer size
            void* tcpBuffSize = FC_Uint2VPtr(pDcpt->ftpcDataConnection.ftpcDataTxBuffSize);
            (void)NET_PRES_SocketOptionsSet(pDcpt->ftpcSocket.ftpcDataSkt, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_TX_BUFF, tcpBuffSize); 
        }
        else if(NET_PRES_SocketOptionsGet(pDcpt->ftpcSocket.ftpcDataSkt, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_TX_BUFF, &sktTxSize))
        {
            pDcpt->ftpcDataConnection.ftpcDataTxBuffSize = sktTxSize;
        }
        else
        {
            // do nothing
        }
    }
    pDcpt->ftpcDataConnection.ftpcDataTxBuffSize  &= (uint16_t)FTPC_BUFF_SIZE_ALIGN_MASK;
    pDcpt->ftpcDataTxBuff = (char *)TCPIP_HEAP_Malloc(ftpcGlobalConfig.memH, 
                                    pDcpt->ftpcDataConnection.ftpcDataTxBuffSize );  
    
    if(pDcpt->ftpcDataConnection.ftpcDataRxBuffSize != 0U)
    {
        //set the socket Rx buffer size
        void* tcpBuffSize = FC_Uint2VPtr((uint32_t)pDcpt->ftpcDataConnection.ftpcDataRxBuffSize);
        (void)NET_PRES_SocketOptionsSet(pDcpt->ftpcSocket.ftpcDataSkt, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_RX_BUFF, tcpBuffSize);
    }
    else
    {
        if(ftpcGlobalConfig.data_rx_buffsize_dflt != 0U)
        {
            pDcpt->ftpcDataConnection.ftpcDataRxBuffSize  = ftpcGlobalConfig.data_rx_buffsize_dflt ;
            //set the socket Rx buffer size
            void* tcpBuffSize = FC_Uint2VPtr((uint32_t)pDcpt->ftpcDataConnection.ftpcDataRxBuffSize);
            (void)NET_PRES_SocketOptionsSet(pDcpt->ftpcSocket.ftpcDataSkt, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_RX_BUFF, tcpBuffSize); 
        }
        else if(NET_PRES_SocketOptionsGet(pDcpt->ftpcSocket.ftpcDataSkt, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_RX_BUFF, &sktRxSize))
        {
            pDcpt->ftpcDataConnection.ftpcDataRxBuffSize  = sktRxSize;
        }
        else
        {
            // do nothing
        }
    }
    
    (void)NET_PRES_SocketOptionsGet(pDcpt->ftpcSocket.ftpcDataSkt, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_RX_BUFF, &sktRxSize);
    pDcpt->ftpcDataConnection.ftpcDataRxBuffSize  &= (uint16_t)FTPC_BUFF_SIZE_ALIGN_MASK;
    pDcpt->ftpcDataRxBuff = (char *)TCPIP_HEAP_Malloc(ftpcGlobalConfig.memH, 
                                pDcpt->ftpcDataConnection.ftpcDataRxBuffSize ); 
    return TCPIP_FTPC_RES_OK;
}


// signals an FTPC Control Socket event
static void ftpcCtrlEvent(TCPIP_FTPC_DCPT_TYPE* pDcpt, TCPIP_FTPC_CTRL_EVENT_TYPE ftpcEvent, char * ctrlbuff, uint16_t ctrllen)
{
    if(pDcpt->ctrlSktCallback != NULL)
    {
        (*(pDcpt->ctrlSktCallback ))((TCPIP_FTPC_CONN_HANDLE_TYPE)pDcpt, ftpcEvent,
                pDcpt->ftpcCommand, ctrlbuff, ctrllen);
    }
}

// signals an FTPC Data Socket event
static bool ftpcDataEvent(TCPIP_FTPC_DCPT_TYPE* pDcpt, TCPIP_FTPC_DATA_EVENT_TYPE ftpcEvent,
                          char * databuff, uint16_t * datalen)
{
    if(pDcpt->dataSktCallback != NULL)
    {
        return (*(pDcpt->dataSktCallback ))((TCPIP_FTPC_CONN_HANDLE_TYPE)pDcpt, ftpcEvent, pDcpt->ftpcCommand, databuff, datalen);
    }

    return false;
}

#endif //#if defined(TCPIP_STACK_USE_FTP_CLIENT)
