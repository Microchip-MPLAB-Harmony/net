/*******************************************************************************
  File Transfer Protocol (FTP) Client
  Module for Microchip TCP/IP Stack

  Summary:
    Implementation of FTP Client protocol
    
  Description:
    - FTP client provides ability to send/receive files to/from FTP server.
    - Reference: RFC 959

*******************************************************************************/

/*******************************************************************************
File Name:  ftpc.c
Copyright © 2019 released Microchip Technology Inc.  All rights
reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED ?AS IS? WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
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
static int          ftpcInitCount = 0;       			// module initialization count
static tcpipSignalHandle    ftpcSignalHandle = 0;
static TCPIP_FTPC_DCPT_TYPE* ftpcDcptPool = 0;    		// allocated pool of descriptors
static TCPIP_FTPC_MODULE_DCPT_TYPE ftpcGlobalConfig;// global FTP Client configuration
static SINGLE_LIST  ftpcDcptFreeList = { 0 };      // pool of free descriptors 
static SINGLE_LIST  ftpcDcptBusyList = { 0 };      // pool of descriptors in use

static const TCPIP_FTPC_STATE_FUNC ftpcStateTbl[] = 
{
    _ftpcStateHome,                  // TCPIP_FTPC_STATE_HOME
    //Connect Routines
    _ftpcStateStartConnect,          // TCPIP_FTPC_STATE_START_CONNECT
    _ftpcStateWaitConnect,           // TCPIP_FTPC_STATE_WAIT_CONNECT
    _ftpcStateDoneConnect,           // TCPIP_FTPC_STATE_DONE_CONNECT
    //Login Routines
    _ftpcStateSendUser,              //TCPIP_FTPC_STATE_LOGIN_SEND_USER  
    _ftpcStateWaitUser,              //TCPIP_FTPC_STATE_LOGIN_WAIT_USER
    _ftpcStateSendPass,              //TCPIP_FTPC_STATE_LOGIN_SEND_PASS
    _ftpcStateWaitPass,              //TCPIP_FTPC_STATE_LOGIN_WAIT_PASS
    _ftpcStateSendAcct,              //TCPIP_FTPC_STATE_LOGIN_SEND_ACCT
    _ftpcStateWaitAcct,              //TCPIP_FTPC_STATE_LOGIN_WAIT_ACCT
    _ftpcStateDoneLogin,             //TCPIP_FTPC_STATE_DONE_LOGIN
    //CWD Routines
    _ftpcStateSendCwd,               //TCPIP_FTPC_STATE_SEND_CWD  
    _ftpcStateWaitCwd,               //TCPIP_FTPC_STATE_WAIT_CWD
    //CDUP Routines        
    _ftpcStateSendCdup,              //TCPIP_FTPC_STATE_SEND_CDUP  
    _ftpcStateWaitCdup,              //TCPIP_FTPC_STATE_WAIT_CDUP
    //MKD Routines        
    _ftpcStateSendMkd,              //TCPIP_FTPC_STATE_SEND_MKD  
    _ftpcStateWaitMkd,              //TCPIP_FTPC_STATE_WAIT_MKD
    //RMD Routines        
    _ftpcStateSendRmd,              //TCPIP_FTPC_STATE_SEND_RMD,  
    _ftpcStateWaitRmd,              //TCPIP_FTPC_STATE_WAIT_RMD,
    //DELE Routines        
    _ftpcStateSendDele,             //TCPIP_FTPC_STATE_SEND_DELE,  
    _ftpcStateWaitDele,             //TCPIP_FTPC_STATE_WAIT_DELE,    
    //QUIT Routines 
    _ftpcStateSendQuit,             //TCPIP_FTPC_STATE_SEND_QUIT,
    _ftpcStateWaitQuit,             //TCPIP_FTPC_STATE_WAIT_QUIT,    
    //PWD Routines        
    _ftpcStateSendPwd,              //TCPIP_FTPC_STATE_SEND_PWD,  
    _ftpcStateWaitPwd,              //TCPIP_FTPC_STATE_WAIT_PWD,
    //TYPE Routines        
    _ftpcStateSendType,             //TCPIP_FTPC_STATE_SEND_TYPE,  
    _ftpcStateWaitType,             //TCPIP_FTPC_STATE_WAIT_TYPE,
    //STRU Routines 
    _ftpcStateSendStru,             //TCPIP_FTPC_STATE_SEND_STRU,  
    _ftpcStateWaitStru,             //TCPIP_FTPC_STATE_WAIT_STRU,
    //MODE Routines 
    _ftpcStateSendMode,             //TCPIP_FTPC_STATE_SEND_MODE,  
    _ftpcStateWaitMode,             //TCPIP_FTPC_STATE_WAIT_MODE,
    //PORT routines
    _ftpcStateSendPort,              //TCPIP_FTPC_STATE_SEND_PORT,  
    _ftpcStateWaitPort,              //TCPIP_FTPC_STATE_WAIT_PORT,
    //PASV routines
    _ftpcStateSendPasv,              //TCPIP_FTPC_STATE_SEND_PASV,  
    _ftpcStateWaitPasv,              //TCPIP_FTPC_STATE_WAIT_PASV, 
    _ftpcStateWaitPasvDataConnect,   //TCPIP_FTPC_STATE_PASV_WAIT_DATA_CONNECT    
    //RETR States        
    _ftpcStateSendRetr,              //TCPIP_FTPC_STATE_SEND_RETR,  
    _ftpcStateWaitRetrCtrlResp,      //TCPIP_FTPC_STATE_WAIT_RETR_CTRL_RESPONSE,
    _ftpcStateWaitRetrReadDataSkt,   //TCPIP_FTPC_STATE_WAIT_RETR_READ_DATA_SOCKET,
    _ftpcStateRetrEoF,               //TCPIP_FTPC_STATE_RETR_EOF
    //STOR States        
    _ftpcStateSendStor,              //TCPIP_FTPC_STATE_SEND_STOR,  
    _ftpcStateWaitStorCtrlResp,      //TCPIP_FTPC_STATE_WAIT_STOR_CTRL_RESPONSE,
    _ftpcStateStorWriteDataSkt,      //TCPIP_FTPC_STATE_STOR_WRITE_DATA_SOCKET
    _ftpcStateStorEoF,               //TCPIP_FTPC_STATE_STOR_EOF
    //Name List
	_ftpcStateStartLst,				//TCPIP_FTPC_STATE_START_LST
    _ftpcStateSendLst,              //TCPIP_FTPC_STATE_SEND_LST,  
    _ftpcStateWaitLstCtrlResp,      //TCPIP_FTPC_STATE_WAIT_LST_CTRL_RESPONSE,
    _ftpcStateWaitLstReadDataSkt,   //TCPIP_FTPC_STATE_WAIT_LST_READ_DATA_SOCKET,
    _ftpcStateLstEoT,              //TCPIP_FTPC_STATE_LST_EOT,
    //Disconnect Control socket connection                 
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////// FTPC Core Functions ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool TCPIP_FTPC_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, 
                                const TCPIP_FTPC_MODULE_CONFIG_TYPE* pftpcConfig)
{
    TCPIP_FTPC_DCPT_TYPE * pftpcDcpt;
	int index;
	
	if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }
	if(ftpcInitCount == 0)
    {   // stack start up; initialize just once
        // check configuration data is not missing
        if(pftpcConfig == 0)
        {
            return false;
        }
		
		// create the FTPC timer
        ftpcSignalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, 
                                        TCPIP_FTPC_Task, TCPIP_FTPC_TASK_TICK_RATE);
		if(ftpcSignalHandle == 0)
        {   // cannot create the FTPC timer
            return false;
        }
		// initialize the in-use list
        TCPIP_Helper_SingleListInitialize(&ftpcDcptBusyList);
		// initialize the free pool
        TCPIP_Helper_SingleListInitialize(&ftpcDcptFreeList);
		
		// allocate the descriptors pool
        ftpcDcptPool = (TCPIP_FTPC_DCPT_TYPE*)TCPIP_HEAP_Calloc(stackCtrl->memH, 
                                pftpcConfig->nMaxClients, sizeof(*ftpcDcptPool));
		if(ftpcDcptPool == 0)
        {   // cannot create the FTPC pool, so remove FTPC timer
            _TCPIPStackSignalHandlerDeregister(ftpcSignalHandle);
			ftpcSignalHandle = 0;
            return false;
        }
		
		pftpcDcpt = ftpcDcptPool;
		
		for(index = 0; index < pftpcConfig->nMaxClients; index++, pftpcDcpt++)
        {
            _ftpcDcptSetDefault(pftpcDcpt, index);
            TCPIP_Helper_SingleListTailAdd(&ftpcDcptFreeList, (SGL_LIST_NODE*)pftpcDcpt);
        }
        ftpcGlobalConfig.nMaxClients = pftpcConfig->nMaxClients;
        ftpcGlobalConfig.memH = stackCtrl->memH;
        ftpcGlobalConfig.ftpcTmo = pftpcConfig->ftpcTmo > TCPIP_FTPC_MIN_TMO ? 
                                        pftpcConfig->ftpcTmo : TCPIP_FTPC_MIN_TMO;		
        ftpcGlobalConfig.data_tx_buffsize_dflt = pftpcConfig->data_tx_buffsize_dflt;
        ftpcGlobalConfig.data_rx_buffsize_dflt = pftpcConfig->data_rx_buffsize_dflt;
	}
	
	ftpcInitCount++;
    return true;
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void _ftpcReleaseSockets(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    if(pDcpt->ftpcSocket.ftpcCtrlSkt != NET_PRES_INVALID_SOCKET)
    {
        //close control socket
        NET_PRES_SocketClose(pDcpt->ftpcSocket.ftpcCtrlSkt);
        pDcpt->ftpcSocket.ftpcCtrlSkt = NET_PRES_INVALID_SOCKET;
    }
    // Free Control Socket Buffers
    TCPIP_HEAP_Free(ftpcGlobalConfig.memH,pDcpt->ftpcCtrlRxBuff);
    TCPIP_HEAP_Free(ftpcGlobalConfig.memH,pDcpt->ftpcCtrlTxBuff);
    _ftpcCloseDataSkt(pDcpt);
}

void TCPIP_FTPC_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    TCPIP_FTPC_DCPT_TYPE * pftpcDcpt;
    int index;
    
    if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
    {   // stack shut down
        if(ftpcDcptPool != 0)
        {
            pftpcDcpt = ftpcDcptPool;
		
            for(index = 0; index < ftpcGlobalConfig.nMaxClients; index++, pftpcDcpt++)
            {
                _ftpcReleaseSockets(pftpcDcpt);  
            }
            if(ftpcSignalHandle)
            {
                _TCPIPStackSignalHandlerDeregister(ftpcSignalHandle);
                ftpcSignalHandle = 0;
            } 
            TCPIP_HEAP_Free(ftpcGlobalConfig.memH, ftpcDcptPool);
            ftpcInitCount--;
        }
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

void  TCPIP_FTPC_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != 0)
    { // TMO or RX signals occurred
      TCPIP_FTPC_Process();
    }
}

static void TCPIP_FTPC_Process(void)
{
	TCPIP_FTPC_DCPT_TYPE* pCurrDcpt, *pNextDcpt;
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;

    OSAL_CRITSECT_DATA_TYPE ftpcLock = ftpcEnterCritical();
    pCurrDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcDcptBusyList.head;
    pNextDcpt = pCurrDcpt? pCurrDcpt->next : 0;
    ftpcExitCritical(ftpcLock);
    
	while(pCurrDcpt != 0)
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
        pNextDcpt = pCurrDcpt? pCurrDcpt->next : 0;
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
static void _FTPCCtrlSktRxSignalHandler(NET_PRES_SKT_HANDLE_T hTCP, NET_PRES_SIGNAL_HANDLE hNet, 
                                                            uint16_t sigType, const void* param)
{
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)param;
    if(sigType & TCPIP_TCP_SIGNAL_RX_DATA)
    {
        _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
    if(sigType & TCPIP_TCP_SIGNAL_RX_FIN)
    {
        pDcpt->ftpcSignal |= TCPIP_FTPC_SIGNAL_CTRL_RX_FIN;
        _ftpcCtrlDisconnect(pDcpt);
    }

}

//Callback for data socket
static void _FTPCDataSktRxSignalHandler(NET_PRES_SKT_HANDLE_T hTCP, NET_PRES_SIGNAL_HANDLE hNet, 
                                                            uint16_t sigType, const void* param)
{
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)param;
    
    if(sigType & TCPIP_TCP_SIGNAL_RX_DATA)
    {
        _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
    if(sigType & TCPIP_TCP_SIGNAL_RX_FIN)
    {
        pDcpt->ftpcSignal |= TCPIP_FTPC_SIGNAL_DATA_RX_FIN;           
    }
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////// FTPC External APIs  ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////

TCPIP_FTPC_CONN_HANDLE_TYPE TCPIP_FTPC_Connect(TCPIP_FTPC_CTRL_CONN_TYPE *pftpcConn, 
        TCPIP_FTPC_CTRL_EVENT_CALLBACK_TYPE ctrlCallback, TCPIP_FTPC_RETURN_TYPE* pRet)
{
    TCPIP_FTPC_DCPT_TYPE* pDcpt = 0;
    
    // Get a free descriptor for new connection    
    OSAL_CRITSECT_DATA_TYPE ftpcLock = ftpcEnterCritical();    
    pDcpt = (TCPIP_FTPC_DCPT_TYPE*)TCPIP_Helper_SingleListHeadRemove(&ftpcDcptFreeList);
    ftpcExitCritical(ftpcLock);
    
    if(pDcpt)
    {
        *pRet = TCPIP_FTPC_RET_OK;
        pDcpt->ftpcState = TCPIP_FTPC_STATE_START_CONNECT;
        pDcpt->ftpcCtrlConnection.ftpcServerAddr = pftpcConn->ftpcServerAddr;
        pDcpt->ftpcCtrlConnection.ftpcServerIpAddrType = pftpcConn->ftpcServerIpAddrType;
        pDcpt->ftpcCtrlConnection.serverCtrlPort = (pftpcConn->serverCtrlPort == 0)? 
                            TCPIP_FTPC_DFLT_SRV_CTRL_PORT:pftpcConn->serverCtrlPort;
        pDcpt->ftpcActive = true;
        pDcpt->ctrlSktCallback = ctrlCallback;
        pDcpt->ftpcCommand = TCPIP_FTPC_CMD_CONNECT;
        pDcpt->error = TCPIP_FTPC_ERROR_NONE;
        ftpcLock = ftpcEnterCritical();
        TCPIP_Helper_SingleListTailAdd(&ftpcDcptBusyList, (SGL_LIST_NODE*)pDcpt);
        ftpcExitCritical(ftpcLock);  
    }
    else
    {
        *pRet = TCPIP_FTPC_RET_FAILURE;
    }
    
    return (TCPIP_FTPC_CONN_HANDLE_TYPE)pDcpt;
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_Disconnect (TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet = TCPIP_FTPC_RET_OK;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    
    if(!pDcpt->ftpcActive)
    {
        _ftpcCtrlDisconnect(pDcpt);
    }
    else
    {
        ftpcRet = TCPIP_FTPC_RET_BUSY;
    }
    
    return ftpcRet;
}


TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_Login(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, const char *username, 
                const char *password, const char *account)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    if(!pDcpt->ftpcActive)
    {
        if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_CONNECTED) != 0 )
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
    
    return ftpcRet;
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_Change_Dir(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
                const char *pathname)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    if(!pDcpt->ftpcActive)
    {
        if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_CONNECTED) != 0 )
        {
            if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 )
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
    
    return ftpcRet;
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_ChangeToParentDir(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    if(!pDcpt->ftpcActive)
    {
        if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_CONNECTED) != 0 )
        {
            if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 )
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
    
    return ftpcRet;   
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_MakeDir(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
            const char *dirname)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    
    if(!pDcpt->ftpcActive)
    {
        if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_CONNECTED) != 0 )
        {
            if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 )
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
    return ftpcRet;
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_RemoveDir(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
                const char *pathname)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    if(!pDcpt->ftpcActive)
    {
        if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_CONNECTED) != 0 )
        {
            if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 )
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
    return ftpcRet;
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_Get_WorkingDir (TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    
    if(!pDcpt->ftpcActive)
    {
        if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_CONNECTED) != 0 )
        {
            if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 )
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
    return ftpcRet;    
}


TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_GetFile(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
        TCPIP_FTPC_DATA_CONN_TYPE *pftpcDataConn, TCPIP_FTPC_FILE_OPT_TYPE *pfileOptions, 
        TCPIP_FTPC_DATA_EVENT_CALLBACK_TYPE dataCallback)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    if(!pDcpt->ftpcActive)
    {
        if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_CONNECTED) != 0 )
        {
            if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 )
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
                _ftpcStateStartGet(pDcpt);            
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
    
    return ftpcRet;
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_PutFile(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
        TCPIP_FTPC_DATA_CONN_TYPE *pftpcDataConn, TCPIP_FTPC_FILE_OPT_TYPE *pfileOptions, 
        TCPIP_FTPC_DATA_EVENT_CALLBACK_TYPE dataCallback)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    if(!pDcpt->ftpcActive)
    {
        if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_CONNECTED) != 0 )
        {
            if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 )
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
                _ftpcStateStartPut(pDcpt); 
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
    return ftpcRet;
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_NameList(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle,
        TCPIP_FTPC_DATA_CONN_TYPE *pftpcDataConn, TCPIP_FTPC_FILE_OPT_TYPE *pfileOptions, 
        TCPIP_FTPC_DATA_EVENT_CALLBACK_TYPE dataCallback)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    if(!pDcpt->ftpcActive)
    {
        if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_CONNECTED) != 0 )
        {
            if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 )
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
    return ftpcRet;
}


TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_List(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle,
        TCPIP_FTPC_DATA_CONN_TYPE *pftpcDataConn, TCPIP_FTPC_FILE_OPT_TYPE *pfileOptions,
        TCPIP_FTPC_DATA_EVENT_CALLBACK_TYPE dataCallback)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    if(!pDcpt->ftpcActive)
    {
        if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_CONNECTED) != 0 )
        {
            if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 )
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
    return ftpcRet;
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_DeleteFile(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
                const char *pathname)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    if(!pDcpt->ftpcActive)
    {
        if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_CONNECTED) != 0 )
        {
            if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 )
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
    return ftpcRet;
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_Logout(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    if(!pDcpt->ftpcActive)
    {
        if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_CONNECTED) != 0 )
        {
            if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 )
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
    
    return ftpcRet;   
}

void TCPIP_FTPC_Get_Status(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, TCPIP_FTPC_STATUS_TYPE * ftpcStatus)
{  
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle; 
    
    ftpcStatus->busy = pDcpt->ftpcActive;
    ftpcStatus->isConnected = (pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_CONNECTED) == TCPIP_FTPC_FLAG_CONNECTED;
    ftpcStatus->isLoggedIn = (pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) == TCPIP_FTPC_FLAG_LOGGEDIN;
    ftpcStatus->cmd = pDcpt->ftpcCommand;
    ftpcStatus->error = pDcpt->error;
    ftpcStatus->ctrlSocket = pDcpt->ftpcSocket.ftpcCtrlSkt;
    ftpcStatus->dataSocket = pDcpt->ftpcSocket.ftpcDataSkt;
    
    ftpcStatus->ctrlRxLen = pDcpt->ftpcCtrlRxLen;
    ftpcStatus->ctrlTxLen = pDcpt->ftpcCtrlTxLen;
    ftpcStatus->dataRxLen = pDcpt->ftpcDataRxLen;
    ftpcStatus->dataTxLen = pDcpt->ftpcDataTxLen;

}


TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_SetType(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
        TCPIP_FTPC_DATA_REP_TYPE type)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    if(!pDcpt->ftpcActive)
    {
        if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_CONNECTED) != 0 )
        {
            if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 )
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
    return ftpcRet;
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_SetStruct(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
        TCPIP_FTPC_DATA_STRUCT_TYPE file_struct)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    if(!pDcpt->ftpcActive)
    {
        if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_CONNECTED) != 0 )
        {
            if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 )
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
    return ftpcRet;
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_SetMode(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
        TCPIP_FTPC_TRANSFER_MODE_TYPE mode)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    if(!pDcpt->ftpcActive)
    {
        if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_CONNECTED) != 0 )
        {
            if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 )
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
    return ftpcRet;
}


TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_SetActiveMode(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
        TCPIP_FTPC_DATA_CONN_TYPE *pftpcDataConn)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    if(!pDcpt->ftpcActive)
    {
        if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_CONNECTED) != 0 )
        {
            if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 )
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
    return ftpcRet;
}

TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_SetPassiveMode(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle)
{
    TCPIP_FTPC_RETURN_TYPE ftpcRet;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    if(!pDcpt->ftpcActive)
    {
        if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_CONNECTED) != 0 )
        {
            if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 )
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
    return ftpcRet;
}
////////////////////////////////////////////////////////////////////////////////
/////////////////// FTPC State Machine Routines  ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//FTP Client State Machine: Initial state
static TCPIP_FTPC_RESULT_TYPE _ftpcStateHome(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    if(pDcpt->ftpcActive == true)
    {
        //mark the completion of command
        pDcpt->ftpcActive = false; 
        //Clear event callbacks
        pDcpt->dataSktCallback = 0;
        pDcpt->ftpcSignal = TCPIP_FTPC_SIGNAL_NONE;
        pDcpt->fileDescr = (int32_t) SYS_FS_HANDLE_INVALID;
        
    }
    return TCPIP_FTPC_RES_OK;
}

//////////////////////////// CONNECT ROUTINES //////////////////////////////////
//FTP Client State Machine: Start Client-server connection process
static TCPIP_FTPC_RESULT_TYPE _ftpcStateStartConnect(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
	TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t sktRxSize = 0;
    uint16_t sktTxSize = 0;
    
    if(pDcpt->ftpcSocket.ftpcCtrlSkt == NET_PRES_INVALID_SOCKET)
    {   // get a new socket
        NET_PRES_SKT_HANDLE_T ctrlSkt;
        NET_PRES_SKT_T sktType =  NET_PRES_SKT_DEFAULT_STREAM_CLIENT;
        ctrlSkt = NET_PRES_SocketOpen(0, sktType, 
                (NET_PRES_SKT_ADDR_T)pDcpt->ftpcCtrlConnection.ftpcServerIpAddrType, 
                                    pDcpt->ftpcCtrlConnection.serverCtrlPort, 0, 0);

        if(ctrlSkt != NET_PRES_INVALID_SOCKET)
        {   
            pDcpt->ftpcSocket.ftpcCtrlSkt = ctrlSkt;
            // alert of incoming traffic
            NET_PRES_SocketSignalHandlerRegister(ctrlSkt, TCPIP_TCP_SIGNAL_RX_DATA | 
                    TCPIP_TCP_SIGNAL_RX_FIN, _FTPCCtrlSktRxSignalHandler, (TCPIP_FTPC_DCPT_TYPE*) pDcpt);           
            
            if(NET_PRES_SocketOptionsGet(ctrlSkt, TCP_OPTION_TX_BUFF, &sktTxSize))
            {
                //mask it for 16 byte aligned
                sktTxSize = sktTxSize & FTPC_BUFF_SIZE_ALIGN_MASK;
                //Allocate and Add Tx buffer for Ctrl socket for commands
                pDcpt->ftpcCtrlTxBuff = (char *)TCPIP_HEAP_Malloc(ftpcGlobalConfig.memH, sktTxSize);
             }
            
            if(NET_PRES_SocketOptionsGet(ctrlSkt, TCP_OPTION_RX_BUFF, &sktRxSize))
            {                   
                //mask it for 16 byte aligned          
                sktRxSize = sktRxSize & FTPC_BUFF_SIZE_ALIGN_MASK;
                //Allocate and Add Rx buffer for Ctrl socket for command response
                pDcpt->ftpcCtrlRxBuff = (char *)TCPIP_HEAP_Malloc(ftpcGlobalConfig.memH, sktRxSize);
                
            }             
            
            //bind to remote server
            if(NET_PRES_SocketRemoteBind(pDcpt->ftpcSocket.ftpcCtrlSkt, 
                (NET_PRES_SKT_ADDR_T)pDcpt->ftpcCtrlConnection.ftpcServerIpAddrType,  
                    pDcpt->ftpcCtrlConnection.serverCtrlPort, 
                    (NET_PRES_ADDRESS*)(pDcpt->ftpcCtrlConnection.ftpcServerAddr)))
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
                    _ftpcCtrlDisconnect(pDcpt);
                    res = TCPIP_FTPC_RES_ERROR; 
                }
            }
            else
            {
                // failed binding to remote server
                pDcpt->error = TCPIP_FTPC_ERROR_BIND_CTRL_SOCKET;
                _ftpcCtrlDisconnect(pDcpt);
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
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitConnect(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;

    //Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {	//process the control response	
        res =   ftpcParseReplyCode(pDcpt); 
		if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
		{	//connect process done
			pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_CONNECT;
			_ftpcStateDoneConnect(pDcpt);
		}
		else
		{
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;   
            _ftpcCtrlDisconnect(pDcpt);
            res = TCPIP_FTPC_RES_ERROR;
		}
    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;        
            _ftpcCtrlDisconnect(pDcpt);
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
static TCPIP_FTPC_RESULT_TYPE _ftpcStateDoneConnect(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    // Add flag to indicate FTP Client connected 
    pDcpt->ftpcFlag  = TCPIP_FTPC_FLAG_CONNECTED;    
    // trigger call back for control socket
    ftpcCtrlEvent(pDcpt,TCPIP_FTPC_CTRL_EVENT_SUCCESS,0,0);    
    // returns the state machine to default home
    pDcpt->ftpcState = TCPIP_FTPC_STATE_HOME; 
    return TCPIP_FTPC_RES_OK; 
}

///////////////////////////// LOGIN ROUTINES ///////////////////////////////////
//FTP Client State Machine: Send USER Command to FTP server
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendUser(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes = 0;
     
    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "USER %s\r\n", pDcpt->ftpcAccessControl.ftpcUserName);
    
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
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitUser(TCPIP_FTPC_DCPT_TYPE* pDcpt)
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
    {	//process the control response
        res =   ftpcParseReplyCode(pDcpt); 
		if(res == TCPIP_FTPC_SERVER_POS_INTRMDT_RESPONSE)
		{   //send the password
			pDcpt->ftpcState = TCPIP_FTPC_STATE_LOGIN_SEND_PASS;
			_ftpcStateSendPass(pDcpt);
		}
		else if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
		{            
			//no need of password; login is success
			pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_LOGIN;
			_ftpcStateDoneLogin(pDcpt);
		}
		else
		{
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
			_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
		}

    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            _ftpcErrorUpdate(pDcpt);            
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
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendPass(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes = 0;
    
    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "PASS %s\r\n", pDcpt->ftpcAccessControl.ftpcPassword);
    
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
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitPass(TCPIP_FTPC_DCPT_TYPE* pDcpt)
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
    {	//process the control response	
        res =   ftpcParseReplyCode(pDcpt); 
		if(res == TCPIP_FTPC_SERVER_POS_INTRMDT_RESPONSE)
		{
			//send the Account 
            pDcpt->ftpcState = TCPIP_FTPC_STATE_LOGIN_SEND_ACCT;
            _ftpcStateSendAcct(pDcpt);
		}
		else if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
		{
			//no need of account; login is success
			pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_LOGIN;
			_ftpcStateDoneLogin(pDcpt);
		}
		else
		{
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
			_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
		}

    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            _ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR; 
        }  
        else
        {
            res = TCPIP_FTPC_RES_PENDING;
        }        
    }        
    return res;
}

static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendAcct(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes = 0;
    
    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "ACCT %s\r\n", pDcpt->ftpcAccessControl.ftpcAccount);
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state         
        pDcpt->ftpcState = TCPIP_FTPC_STATE_LOGIN_WAIT_ACCT;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    } 
    pDcpt->ftpcCommand = TCPIP_FTPC_CMD_ACCT;
    return res;
}

static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitAcct(TCPIP_FTPC_DCPT_TYPE* pDcpt)
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
    {	//process the control response	
        res =   ftpcParseReplyCode(pDcpt); 
		if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
		{
			//login is success
			pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_LOGIN;
			_ftpcStateDoneLogin(pDcpt);
		}
		else
		{
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
			_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
		}
    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            _ftpcErrorUpdate(pDcpt);            
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
static TCPIP_FTPC_RESULT_TYPE _ftpcStateDoneLogin(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    // Add flag to indicate FTP Client Logged In 
    pDcpt->ftpcFlag  |= TCPIP_FTPC_FLAG_LOGGEDIN;    
    // trigger call back for control socket
    ftpcCtrlEvent(pDcpt,TCPIP_FTPC_CTRL_EVENT_SUCCESS,0,0);
    // returns the state machine to default home
    pDcpt->ftpcState = TCPIP_FTPC_STATE_HOME;
    return TCPIP_FTPC_RES_OK;     
}


////////////////////// CHANGE WORKING DIRECTORY ROUTINES ///////////////////////
//FTP Client State Machine: Send CWD Command to FTP server
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendCwd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes = 0;
    
    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "CWD %s\r\n", pDcpt->ftpcServerPathname);
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_CWD;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    }     
    return res;
}

//FTP Client State Machine: wait for response from FTP server, after CWD command
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitCwd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{   
    //Response
    //250
    //500, 501, 502, 421, 530, 550    
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
	//Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {	//process the control response	
        res =   ftpcParseReplyCode(pDcpt); 
		if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
		{
			//no need of password; login is success
			_ftpcDoneCmd(pDcpt);
		}
		else
		{
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
			_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
		}
    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            _ftpcErrorUpdate(pDcpt);            
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
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendCdup(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes = 0;
    
    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "CDUP\r\n");
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_CDUP;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    }
    return res;   
}

//FTP Client State Machine: wait for response from FTP server, after CDUP command
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitCdup(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //200
    //500, 501, 502, 421, 530, 550
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
	//Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {	//process the control response	
        res =   ftpcParseReplyCode(pDcpt); 
		if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
		{			
			//CDUP success
			_ftpcDoneCmd(pDcpt);
		}
		else
		{
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
			_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
		}
    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            _ftpcErrorUpdate(pDcpt);            
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
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendMkd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes = 0;
    
    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "MKD %s\r\n", pDcpt->ftpcServerPathname);
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_MKD;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    } 
    return res;    
}

//FTP Client State Machine: wait for response from FTP server, after MKD command
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitMkd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //257
    //500, 501, 502, 421, 530, 550
    
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
	//Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {	//process the control response	
        res =   ftpcParseReplyCode(pDcpt); 
		if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
		{			
			//MKD success
			_ftpcDoneCmd(pDcpt);
		}
		else
		{
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
			_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
		}

    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            _ftpcErrorUpdate(pDcpt);            
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
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendRmd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes = 0;
    
    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "RMD %s\r\n", pDcpt->ftpcServerPathname);
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_RMD;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    }     
    return res;    
}

//FTP Client State Machine: wait for response from FTP server, after RMD command
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitRmd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //250
    //500, 501, 502, 421, 530, 550
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
	//Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {	//process the control response	
        res =   ftpcParseReplyCode(pDcpt); 
		if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
		{			
			//RMD success
			_ftpcDoneCmd(pDcpt);
		}
		else
		{
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
			_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
		}

    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            _ftpcErrorUpdate(pDcpt);            
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
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendDele(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes = 0;
    
    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "DELE %s\r\n", pDcpt->ftpcServerPathname);
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_DELE;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    }     
    return res;    
}

//FTP Client State Machine: wait for response from FTP server, after DELE command
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitDele(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //250
    //450, 550
    //500, 501, 502, 421, 530
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
	//Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {	//process the control response	
        res =   ftpcParseReplyCode(pDcpt); 
		if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
		{			
			//RMD success
			_ftpcDoneCmd(pDcpt);
		}
		else
		{
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
			_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
		}

    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            _ftpcErrorUpdate(pDcpt);            
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
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendQuit(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes = 0;
    
    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "QUIT\r\n");
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_QUIT;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    }
    return res;   
}

//FTP Client State Machine: wait for response from FTP server, after QUIT command
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitQuit(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //200
    //500, 501, 502, 421, 530, 550
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
	//Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {	//process the control response	
        res =   ftpcParseReplyCode(pDcpt); 
		if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
		{			
			//Quit success
            // Remove flag to indicate FTP Client Logged Out 
            pDcpt->ftpcFlag  &= ~TCPIP_FTPC_FLAG_LOGGEDIN; 
			_ftpcDoneCmd(pDcpt);
		}
		else
		{
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
			_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
		}
    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            _ftpcErrorUpdate(pDcpt);            
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
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendPwd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes = 0;   
    
    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "PWD\r\n");
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_PWD;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    }
   
    return res;
}

//FTP Client State Machine: wait for response from FTP server, after PWD command
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitPwd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{    //Response
    //257
    //500, 501, 502, 421, 550
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
	//Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {	//process the control response	
        res =   ftpcParseReplyCode(pDcpt); 
		if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
		{			
			//PWD success
			_ftpcDoneCmd(pDcpt);
		}
		else
		{
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
			_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
		}

    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            _ftpcErrorUpdate(pDcpt);            
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
static TCPIP_FTPC_RESULT_TYPE _ftpcStateStartGet(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    pDcpt->filePos = 0;
    pDcpt->ftpcDataRxLen = 0;
    pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_TYPE;
    _ftpcStateSendType(pDcpt);
    return TCPIP_FTPC_RES_OK;
}
///////////////////////////// PUT file ROUTINES ////////////////////////////
static TCPIP_FTPC_RESULT_TYPE _ftpcStateStartPut(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    pDcpt->filePos = 0;    
    pDcpt->ftpcDataRxLen = 0;
    pDcpt->ftpcDataTxLen = 0;
    pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_TYPE;
    _ftpcStateSendType(pDcpt);
    return TCPIP_FTPC_RES_OK;
}

///////////////////////////// TYPE COMMAND ROUTINES ////////////////////////////
//FTP Client State Machine: Send TYPE Command to FTP server
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendType(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes = 0;    
    char * type = 0;
    
    if(pDcpt->ftpcDataConnection.ftpcDataType == TCPIP_FTPC_DATA_REP_ASCII)
        type = "A";
    else if(pDcpt->ftpcDataConnection.ftpcDataType == TCPIP_FTPC_DATA_REP_EBCDIC)
        type = "E";  
    else if(pDcpt->ftpcDataConnection.ftpcDataType == TCPIP_FTPC_DATA_REP_IMAGE)
        type = "I";  
    
    if (type)
    {
        nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "TYPE %s\r\n", type);

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
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitType(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{    //Response
    //257
    //500, 501, 504, 421, 530
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
	//Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {	//process the control response	
        res =   ftpcParseReplyCode(pDcpt); 
		if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
		{			
			//TYPE success
			_ftpcStateDoneType(pDcpt);
		}
		else
		{
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
			_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
		}
    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            _ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR; 
        }  
        else
        {
            res = TCPIP_FTPC_RES_PENDING;
        }        
    }
        
    return res;
}

static TCPIP_FTPC_RESULT_TYPE _ftpcStateDoneType(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    TCP_SOCKET_INFO ctrlSktInfo, dataSktInfo;
    
    if((pDcpt->ftpcCommand == TCPIP_FTPC_CMD_GET)|| (pDcpt->ftpcCommand == TCPIP_FTPC_CMD_PUT))
    {        
        if(pDcpt->ftpcDataConnection.ftpcIsPassiveMode)
        {
            //Send PASV command
            pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_PASV;
            _ftpcStateSendPasv(pDcpt);
        }
        else
        {
            //Open a Data socket, before PORT command
            res = _ftpcStateOpenDataSkt(pDcpt);
            if(res == TCPIP_FTPC_RES_OK)
            {
                NET_PRES_SocketInfoGet(pDcpt->ftpcSocket.ftpcCtrlSkt, &ctrlSktInfo);
                NET_PRES_SocketInfoGet(pDcpt->ftpcSocket.ftpcDataSkt, &dataSktInfo);
                memcpy(&(pDcpt->ftpcDataConnection.dataServerAddr.v4Add), 
                        &(ctrlSktInfo.localIPaddress.v4Add), sizeof(IPV4_ADDR));
                pDcpt->ftpcDataConnection.dataServerPort = dataSktInfo.localPort;
                
                pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_PORT;
                _ftpcStateSendPort(pDcpt);
            }
            else
            {
                // failed to open a socket
                pDcpt->error = TCPIP_FTPC_ERROR_OPEN_DATA_SOCKET; 
                _ftpcErrorUpdate(pDcpt);
            }            
        }
    }
    else
    {
        //TYPE success notification
        ftpcCtrlEvent(pDcpt,TCPIP_FTPC_CTRL_EVENT_SUCCESS,0,0);
        // returns the state machine to default home
        pDcpt->ftpcState = TCPIP_FTPC_STATE_HOME;
    }
    
    return res;
}

///////////////////////// STRU ROUTINES ////////////////////////////
//FTP Client State Machine: Send STRU Command to FTP server
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendStru(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes = 0;    
    char * file_struct = 0;
    
    if(pDcpt->ftpcDataConnection.ftpcDataStructure == TCPIP_FTPC_STRUCT_FILE)
        file_struct = "F";
    else if(pDcpt->ftpcDataConnection.ftpcDataStructure == TCPIP_FTPC_STRUCT_RECORD)
        file_struct = "R";  
    else if(pDcpt->ftpcDataConnection.ftpcDataStructure == TCPIP_FTPC_STRUCT_PAGE)
        file_struct = "P"; 
    
    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "STRU %s\r\n", file_struct);
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_STRU;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    }     
    return res;    
}

//FTP Client State Machine: wait for response from FTP server, after RMD command
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitStru(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //200
    //500, 501, 504, 421, 530
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
	//Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {	//process the control response	
        res =   ftpcParseReplyCode(pDcpt); 
		if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
		{			
			//RMD success
			_ftpcDoneCmd(pDcpt);
		}
		else
		{
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
			_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
		}

    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            _ftpcErrorUpdate(pDcpt);            
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
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendMode(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes = 0;    
    char * mode = 0;
    
    if(pDcpt->ftpcDataConnection.ftpcTransferMode == TCPIP_FTPC_TRANS_STREAM_MODE)
        mode = "S";
    else if(pDcpt->ftpcDataConnection.ftpcTransferMode == TCPIP_FTPC_TRANS_BLOCK_MODE)
        mode = "B";  
    else if(pDcpt->ftpcDataConnection.ftpcTransferMode == TCPIP_FTPC_TRANS_COMPRESS_MODE)
        mode = "C"; 
    
    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "MODE %s\r\n", mode);
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_MODE;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    }     
    return res;    
}

//FTP Client State Machine: wait for response from FTP server, after MODE command
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitMode(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //200
    //500, 501, 504, 421, 530
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
	//Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {	//process the control response	
        res =   ftpcParseReplyCode(pDcpt); 
		if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
		{			
			//RMD success
			_ftpcDoneCmd(pDcpt);
		}
		else
		{
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
			_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
		}

    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            _ftpcErrorUpdate(pDcpt);            
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
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendPasv(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes = 0;
    
    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "PASV\r\n");
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state         
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_PASV;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    }
    return res;
}

//FTP Client State Machine: wait for response from FTP server, after PASV command
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitPasv(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{   //Response
    //227
    //500, 501, 502, 421, 530
    
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
	//Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {	//process the control response	
        res =   ftpcParseReplyCode(pDcpt); 
		if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
		{			
			//PASV command success
			_ftpcStateDonePasv(pDcpt);
		}
		else
		{
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
			_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
		}

    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            _ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR; 
        }  
        else
        {
            res = TCPIP_FTPC_RES_PENDING;
        }        
    }        
    return res;
}


static TCPIP_FTPC_RESULT_TYPE _ftpcStateDonePasv(TCPIP_FTPC_DCPT_TYPE* pDcpt)
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
            dataSkt = NET_PRES_SocketOpen(0, sktType, 
                    (NET_PRES_SKT_ADDR_T)pDcpt->ftpcDataConnection.dataServerIpAddrType, 
                                        pDcpt->ftpcDataConnection.dataServerPort, 0, 0);
            if(dataSkt != NET_PRES_INVALID_SOCKET)
            {   
                pDcpt->ftpcSocket.ftpcDataSkt = dataSkt;
                // alert of incoming traffic
                NET_PRES_SocketSignalHandlerRegister(dataSkt, TCPIP_TCP_SIGNAL_RX_DATA 
                            | TCPIP_TCP_SIGNAL_RX_FIN, _FTPCDataSktRxSignalHandler,(TCPIP_FTPC_DCPT_TYPE*) pDcpt);
                //set RX/TX buffers for Data Socket
                res = _ftpcSetDataSktBuff(pDcpt);
                // point socket to the FTP server
                if(NET_PRES_SocketRemoteBind(pDcpt->ftpcSocket.ftpcDataSkt, 
                        (NET_PRES_SKT_ADDR_T)pDcpt->ftpcDataConnection.dataServerIpAddrType,  
                        pDcpt->ftpcDataConnection.dataServerPort, 
                        (NET_PRES_ADDRESS*)(&(pDcpt->ftpcDataConnection.dataServerAddr))))
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
                        _ftpcCloseDataSkt(pDcpt);
                    }
                }
                else
                {   // failed
                    pDcpt->error = TCPIP_FTPC_ERROR_BIND_DATA_SOCKET;
                    res = TCPIP_FTPC_RES_ERROR; 
                    _ftpcCloseDataSkt(pDcpt);
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
        ftpcCtrlEvent(pDcpt,TCPIP_FTPC_CTRL_EVENT_SUCCESS,0,0);
        // returns the state machine to default home
        pDcpt->ftpcState = TCPIP_FTPC_STATE_HOME;
        res = TCPIP_FTPC_RES_OK;
    }   
    //Report the failure
    if(res !=TCPIP_FTPC_RES_OK)
    {
        _ftpcErrorUpdate(pDcpt);   
    }
    return res;
}

//FTP Client State Machine: Wait for Client-server Data socket connection
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitPasvDataConnect(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    
    if (NET_PRES_SocketIsConnected(pDcpt->ftpcSocket.ftpcDataSkt))
    {    
        if(pDcpt->ftpcCommand == TCPIP_FTPC_CMD_GET)
        {
            pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_RETR;
            _ftpcStateSendRetr(pDcpt);
        }
        else if (pDcpt->ftpcCommand == TCPIP_FTPC_CMD_PUT)
        {
            pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_STOR;
            _ftpcStateSendStor(pDcpt);
        }
        else if ((pDcpt->ftpcCommand == TCPIP_FTPC_CMD_NLST)||(pDcpt->ftpcCommand == TCPIP_FTPC_CMD_LIST))
        {
            pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_LST;
            _ftpcStateSendLst(pDcpt);
        } 
    }
    else
    {        
        if(ftpcIsDcptTmoExpired(pDcpt))
        { 
            //timeout == yes?
            _ftpcCloseDataSkt(pDcpt);
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            _ftpcErrorUpdate(pDcpt);
            res = TCPIP_FTPC_RES_ERROR;
        }
    } 
    return res;     
}


static void ftpc_process_pasv_response(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{    
    int i;
    char * buff; 
    char pasv_response[6];
    
    for(i = 6; i>0; i--)
    {
       buff = strrchr(pDcpt->ftpcCtrlRxBuff, ',');
       if(!buff)
       {
           buff = strrchr(pDcpt->ftpcCtrlRxBuff, '(');
           if(!buff)
               break;
           pasv_response[i-1] = atoi(buff + 1);         
           break;
       }
       pasv_response[i-1] = atoi(buff + 1);
       *buff =  '\0';       
    }

    pDcpt->ftpcDataConnection.dataServerIpAddrType = IP_ADDRESS_TYPE_IPV4;
    pDcpt->ftpcDataConnection.dataServerPort  = (pasv_response[4]*256) + pasv_response[5];
    memcpy(&(pDcpt->ftpcDataConnection.dataServerAddr.v4Add), pasv_response, sizeof(IPV4_ADDR));
    
}

///////////////////////// PORT Command ROUTINES ////////////////////////////
//FTP Client State Machine: Send PORT Command to FTP server
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendPort(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes = 0;

    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "PORT %d,%d,%d,%d,%d,%d\r\n", 
                            pDcpt->ftpcDataConnection.dataServerAddr.v4Add.v[0],
                            pDcpt->ftpcDataConnection.dataServerAddr.v4Add.v[1],
                            pDcpt->ftpcDataConnection.dataServerAddr.v4Add.v[2],
                            pDcpt->ftpcDataConnection.dataServerAddr.v4Add.v[3],
                            (uint8_t)((pDcpt->ftpcDataConnection.dataServerPort >> 8)&0xFF),
                            (uint8_t)((pDcpt->ftpcDataConnection.dataServerPort)&0xFF) );
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state  
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_PORT;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    } 
    return res;
}

//FTP Client State Machine: wait for response from FTP server, after PORT command
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitPort(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{    //Response
    //200
    //500, 501, 421, 530
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
	//Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {	//process the control response	
        res =   ftpcParseReplyCode(pDcpt); 
		if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
		{			
			//PORT success
			_ftpcStateDonePort(pDcpt);
		}
		else
		{
			pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
			_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
		}

    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            _ftpcErrorUpdate(pDcpt);            
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
static TCPIP_FTPC_RESULT_TYPE _ftpcStateDonePort(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{    
    if((pDcpt->ftpcCommand == TCPIP_FTPC_CMD_GET))
    {
        pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_RETR;
        _ftpcStateSendRetr(pDcpt);
    }
    else if (pDcpt->ftpcCommand == TCPIP_FTPC_CMD_PUT)
    {
        pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_STOR;
        _ftpcStateSendStor(pDcpt);
    }
    else if ((pDcpt->ftpcCommand == TCPIP_FTPC_CMD_NLST)||(pDcpt->ftpcCommand == TCPIP_FTPC_CMD_LIST))
    {
        pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_LST;
        _ftpcStateSendLst(pDcpt);
    }
    else
    {
        //PORT success notification
        ftpcCtrlEvent(pDcpt,TCPIP_FTPC_CTRL_EVENT_SUCCESS,0,0);
        // returns the state machine to default home
        pDcpt->ftpcState = TCPIP_FTPC_STATE_HOME;        
    }    
    return TCPIP_FTPC_RES_OK;
}


///////////////////////// RETR Command ROUTINES ////////////////////////////
//FTP Client State Machine: Send RETR Command to FTP server
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendRetr(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes = 0;
    
    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "RETR %s\r\n", pDcpt->ftpcServerPathname);
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state                
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_RETR_CTRL_RESPONSE;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    } 
    return res;  
}

//FTP Client State Machine: wait for control response from FTP server, after RETR command
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitRetrCtrlResp(TCPIP_FTPC_DCPT_TYPE* pDcpt)
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
            _ftpcStateRetrEoF(pDcpt);
        }
        else if(res == TCPIP_FTPC_SERVER_POS_PRELIM_RESPONSE)
        {     
            //Read Data socket
            pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_RETR_READ_DATA_SOCKET;
            _ftpcStateWaitRetrReadDataSkt(pDcpt);
        }  
        else
		{
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
			_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
		}
    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {   //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            _ftpcErrorUpdate(pDcpt);            
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
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitRetrReadDataSkt(TCPIP_FTPC_DCPT_TYPE* pDcpt)
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
        {	//process the control response	
            res =   ftpcParseReplyCode(pDcpt); 
            
            if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
            {
                pDcpt->ftpcState = TCPIP_FTPC_STATE_RETR_EOF; 
                _ftpcStateRetrEoF(pDcpt);
            }
            else
            {
                pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
                _ftpcErrorUpdate(pDcpt);            
                res = TCPIP_FTPC_RES_ERROR;
            }
        }   
    }
   
    return res; 
}

//FTP Client State Machine: detect end-of-file data from FTP server, after RETR command
static TCPIP_FTPC_RESULT_TYPE _ftpcStateRetrEoF(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    uint16_t readLen;
    
    //Read Data Socket
    if(ftpcReadDataSocket(pDcpt, &readLen) != TCPIP_FTPC_RES_ERROR)
    {    
        if(pDcpt->ftpcSignal & TCPIP_FTPC_SIGNAL_DATA_RX_FIN )
        {
            while(readLen)
            {
                ftpcReadDataSocket(pDcpt, &readLen);
            }
            if(pDcpt->fileDescr != SYS_FS_HANDLE_INVALID)
            { 
                //close the file
                SYS_FS_FileClose(pDcpt->fileDescr); 
            }
            pDcpt->ftpcDataLength = 0;
            ftpcDataEvent(pDcpt,TCPIP_FTPC_DATA_RCV_DONE, pDcpt->ftpcDataRxBuff, &(pDcpt->ftpcDataLength));
            res = _ftpcDoneFileTransfer(pDcpt);      
            pDcpt->ftpcSignal &= ~TCPIP_FTPC_SIGNAL_DATA_RX_FIN;
        }    
    }
    return res; 
}

////////////////////////////// STOR COMMAND ROUTINES ///////////////////////////
//FTP Client State Machine: Send STOR Command to FTP server
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendStor(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes = 0;
    const char * pathname;
    
    if(pDcpt->ftpcServerPathname)
        pathname = pDcpt->ftpcServerPathname;
    else
    { 
        pathname = strrchr(pDcpt->ftpcClientPathname, '/');
        if(!pathname)
        {
            pathname = pDcpt->ftpcClientPathname;
        }
    }

    if(pDcpt->ftpcFileOptions.store_unique)
        nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "STOU %s\r\n", pathname);
    else
        nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "STOR %s\r\n", pathname);

    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_STOR_CTRL_RESPONSE;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    }   
    
    return res;    
}

//FTP Client State Machine: wait for response from FTP server, after STOR command
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitStorCtrlResp(TCPIP_FTPC_DCPT_TYPE* pDcpt)
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
    {	//process the control response	
        res =   ftpcParseReplyCode(pDcpt); 
		if((res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)||(res == TCPIP_FTPC_SERVER_POS_PRELIM_RESPONSE))
        {
            //Read Data socket
            pDcpt->ftpcState = TCPIP_FTPC_STATE_STOR_WRITE_DATA_SOCKET;
            _ftpcStateStorWriteDataSkt(pDcpt);
            
        }
        else
		{
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
            _ftpcCloseDataSkt(pDcpt);
			_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
		}
    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {   //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            _ftpcCloseDataSkt(pDcpt);
            _ftpcErrorUpdate(pDcpt);            
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
static TCPIP_FTPC_RESULT_TYPE _ftpcStateStorWriteDataSkt(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //read file
    uint16_t   fileReadLen, writeLen, writeReadyLen;
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
	
	//Transfer File thru data socket, until end of file
	if (NET_PRES_SocketIsConnected(pDcpt->ftpcSocket.ftpcDataSkt))
	{
		writeReadyLen = NET_PRES_SocketWriteIsReady(pDcpt->ftpcSocket.ftpcDataSkt, pDcpt->ftpcDataConnection.ftpcDataTxBuffSize, 0);
		if(writeReadyLen > 0)
		{
			pDcpt->ftpcDataLength = writeReadyLen;
			// Data Event
			if(!ftpcDataEvent(pDcpt,TCPIP_FTPC_DATA_SEND_READY,pDcpt->ftpcDataTxBuff, &(pDcpt->ftpcDataLength)))
			{
				if(pDcpt->fileDescr == SYS_FS_HANDLE_INVALID)
				{
					pDcpt->fileDescr = SYS_FS_FileOpen(pDcpt->ftpcClientPathname,SYS_FS_FILE_OPEN_READ);
					if( pDcpt->fileDescr == (int32_t) SYS_FS_HANDLE_INVALID )
					{
						pDcpt->error = TCPIP_FTPC_ERROR_FILE_OPEN;
						_ftpcCloseDataSkt(pDcpt);
						_ftpcErrorUpdate(pDcpt);            
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
						fileReadLen = SYS_FS_FileRead(pDcpt->fileDescr,pDcpt->ftpcDataTxBuff,writeReadyLen);
						if(fileReadLen > 0)
						{
							writeLen =  NET_PRES_SocketWrite(pDcpt->ftpcSocket.ftpcDataSkt, pDcpt->ftpcDataTxBuff, fileReadLen);
							pDcpt->ftpcDataTxLen += writeLen;
						}
					}
					else
					{
						pDcpt->error = TCPIP_FTPC_ERROR_FILE_ACCESS;
						_ftpcCloseDataSkt(pDcpt);
						_ftpcErrorUpdate(pDcpt);            
						res = TCPIP_FTPC_RES_ERROR;
					}
				}
				else
				{
					//end of file; close file
					SYS_FS_FileClose(pDcpt->fileDescr);  
					//close data socket
					_ftpcCloseDataSkt(pDcpt);
					pDcpt->ftpcDataLength = 0;
					ftpcDataEvent(pDcpt,TCPIP_FTPC_DATA_SEND_DONE,pDcpt->ftpcDataTxBuff, &(pDcpt->ftpcDataLength));
					pDcpt->ftpcState = TCPIP_FTPC_STATE_STOR_EOF;
					res = _ftpcStateStorEoF(pDcpt); 
				}
			}
			else
			{
				if(pDcpt->ftpcDataLength > 0)
				{
					writeLen =  NET_PRES_SocketWrite(pDcpt->ftpcSocket.ftpcDataSkt, pDcpt->ftpcDataTxBuff, pDcpt->ftpcDataLength);
					pDcpt->ftpcDataTxLen += writeLen;
				}
				else
				{
					_ftpcCloseDataSkt(pDcpt);
					pDcpt->ftpcDataLength = 0;
					ftpcDataEvent(pDcpt,TCPIP_FTPC_DATA_SEND_DONE,pDcpt->ftpcDataTxBuff, &(pDcpt->ftpcDataLength));
					pDcpt->ftpcState = TCPIP_FTPC_STATE_STOR_EOF;
					res = _ftpcStateStorEoF(pDcpt);
				}
			}
		}
	}		
    return res; 
}

static TCPIP_FTPC_RESULT_TYPE _ftpcStateStorEoF(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{	
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
   
    //Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    if (res == TCPIP_FTPC_RES_OK)
    {	//process the control response	
        res =   ftpcParseReplyCode(pDcpt); 
		if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
		{            
            res = _ftpcDoneFileTransfer(pDcpt);
        }
        else
        {
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
            _ftpcCloseDataSkt(pDcpt);
			_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
        }
    }    
    return res;
}

////////////////////////////// NLST/LIST COMMAND ROUTINES ///////////////////////////
//FTP Client State Machine: Send NLST/LIST Command to FTP server
static TCPIP_FTPC_RESULT_TYPE _ftpcStateStartLst(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
	TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    TCP_SOCKET_INFO ctrlSktInfo, dataSktInfo;
    pDcpt->filePos = 0;
    pDcpt->ftpcDataRxLen = 0;
    
    if(pDcpt->ftpcDataConnection.ftpcIsPassiveMode)
    {
        pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_PASV;
        _ftpcStateSendPasv(pDcpt);
    }
    else
    {        
        res = _ftpcStateOpenDataSkt(pDcpt);
        if(res == TCPIP_FTPC_RES_OK)
        {
            NET_PRES_SocketInfoGet(pDcpt->ftpcSocket.ftpcCtrlSkt, &ctrlSktInfo);
            NET_PRES_SocketInfoGet(pDcpt->ftpcSocket.ftpcDataSkt, &dataSktInfo);
            memcpy(&(pDcpt->ftpcDataConnection.dataServerAddr.v4Add), 
                        &(ctrlSktInfo.localIPaddress.v4Add), sizeof(IPV4_ADDR));
            pDcpt->ftpcDataConnection.dataServerPort = dataSktInfo.localPort;
            
            pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_PORT;
            _ftpcStateSendPort(pDcpt);
        }
        else
        {
            // failed to open a socket
            pDcpt->error = TCPIP_FTPC_ERROR_OPEN_DATA_SOCKET; 
            _ftpcErrorUpdate(pDcpt);
        }
    }
    return res;
}


static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendLst(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    uint16_t nBytes = 0;
    
    if (pDcpt->ftpcCommand == TCPIP_FTPC_CMD_NLST)
    {
        if(pDcpt->ftpcServerPathname)        
            nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "NLST %s\r\n", pDcpt->ftpcServerPathname);
        else
            nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "NLST\r\n");
    }
    else
    {
        if(pDcpt->ftpcServerPathname)   
            nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "LIST %s\r\n", pDcpt->ftpcServerPathname);
        else
            nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "LIST\r\n");
    }
    
    if((res = ftpcWriteCtrlSocket(pDcpt, nBytes)) == TCPIP_FTPC_RES_OK)
    {   //switch to next state   
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_LST_CTRL_RESPONSE;
        //time out
        ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.ftpcTmo);
    }     
    return pDcpt->ftpcState;    
}


static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitLstCtrlResp(TCPIP_FTPC_DCPT_TYPE* pDcpt)
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
            _ftpcStateLstEoT(pDcpt); 
        }
        else if (res == TCPIP_FTPC_SERVER_POS_PRELIM_RESPONSE)
		{
            //Read Data socket
            pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_LST_READ_DATA_SOCKET;
            _ftpcStateWaitLstReadDataSkt(pDcpt); 
        }
        else
        {
            pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
            _ftpcCloseDataSkt(pDcpt);
			_ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR;
		}
    }
    else
    {
        if(ftpcIsDcptTmoExpired(pDcpt))
        {   //timeout == Yes?
            pDcpt->error = TCPIP_FTPC_ERROR_TIMEOUT;
            _ftpcCloseDataSkt(pDcpt);
            _ftpcErrorUpdate(pDcpt);            
            res = TCPIP_FTPC_RES_ERROR; 
        }
        else
        {
            res = TCPIP_FTPC_RES_PENDING;
        }         
    }    
    return res; 
}

static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitLstReadDataSkt(TCPIP_FTPC_DCPT_TYPE* pDcpt)
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
        {	//process the control response	
            res =   ftpcParseReplyCode(pDcpt); 
            if(res == TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE)
            {
                pDcpt->ftpcState = TCPIP_FTPC_STATE_LST_EOT; 
                _ftpcStateLstEoT(pDcpt);
            }
            else
            {
                pDcpt->error = TCPIP_FTPC_ERROR_CTRL_RESPONSE;
                _ftpcErrorUpdate(pDcpt);            
                res = TCPIP_FTPC_RES_ERROR;
            }
        }  
    }
    return res; 
}

//FTP Client State Machine: detect end-of-transfer of data from FTP server, after NLST/LIST command
static TCPIP_FTPC_RESULT_TYPE _ftpcStateLstEoT(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    uint16_t readLen;     
    
    //Read Data Socket
    if(ftpcReadDataSocket(pDcpt, &readLen) != TCPIP_FTPC_RES_ERROR)
    {     
        if(pDcpt->ftpcSignal & TCPIP_FTPC_SIGNAL_DATA_RX_FIN )
        {
            while(readLen)
            {
                ftpcReadDataSocket(pDcpt, &readLen);
            }
            
            if(pDcpt->fileDescr != SYS_FS_HANDLE_INVALID)
            { 
                //close the file
                SYS_FS_FileClose(pDcpt->fileDescr); 
            }
            pDcpt->ftpcDataLength = 0;
            ftpcDataEvent(pDcpt,TCPIP_FTPC_DATA_RCV_DONE,pDcpt->ftpcDataRxBuff, &(pDcpt->ftpcDataLength));
            res = _ftpcDoneFileTransfer(pDcpt);
            pDcpt->ftpcSignal &= ~TCPIP_FTPC_SIGNAL_DATA_RX_FIN;
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
    int serverReplyCode;
    
    serverReplyCode = atoi((const char*)(pDcpt->ftpcCtrlRxBuff));
    
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
    pDcpt->waitTick = SYS_TMR_TickCountGet() + (tmo * SYS_TMR_TickCounterFrequencyGet());
}

// returns true if timeout
static bool ftpcIsDcptTmoExpired(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    return (int32_t)(SYS_TMR_TickCountGet() - pDcpt->waitTick) >= 0;
}

// Open a Data Socket for a PORT command
static TCPIP_FTPC_RESULT_TYPE _ftpcStateOpenDataSkt(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_OK;
    
    if(pDcpt->ftpcSocket.ftpcDataSkt == NET_PRES_INVALID_SOCKET)
    {   // get a new socket
        NET_PRES_SKT_HANDLE_T dataSkt;
        NET_PRES_SKT_T sktType =  NET_PRES_SKT_DEFAULT_STREAM_SERVER;
        dataSkt = NET_PRES_SocketOpen(0, sktType, 
                (NET_PRES_SKT_ADDR_T)pDcpt->ftpcCtrlConnection.ftpcServerIpAddrType, 0, 0, 0);

        if(dataSkt != NET_PRES_INVALID_SOCKET)
        {   
            pDcpt->ftpcSocket.ftpcDataSkt = dataSkt;
            // alert of incoming traffic
            NET_PRES_SocketSignalHandlerRegister(dataSkt, 
                    TCPIP_TCP_SIGNAL_RX_DATA | TCPIP_TCP_SIGNAL_RX_FIN, _FTPCDataSktRxSignalHandler, 
                                                    (TCPIP_FTPC_DCPT_TYPE*) pDcpt);             
            res = _ftpcSetDataSktBuff(pDcpt);
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
    uint16_t readLen = 0;
    
    pDcpt->ftpcCtrlRxLen = 0;
    if (NET_PRES_SocketIsConnected(pDcpt->ftpcSocket.ftpcCtrlSkt))
    {
        if((readLen = NET_PRES_SocketReadIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt)))  
        {        
            if((pDcpt->ftpcCtrlRxLen = NET_PRES_SocketRead(pDcpt->ftpcSocket.ftpcCtrlSkt, 
                                                        pDcpt->ftpcCtrlRxBuff, readLen)))
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
	if(NET_PRES_SocketWriteIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt, writeByte_count, 0) > 0)
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
    int32_t    fileDescr;
    uint16_t readLen = 0;
    
    if (NET_PRES_SocketIsConnected(pDcpt->ftpcSocket.ftpcDataSkt))
    {
        readLen = NET_PRES_SocketReadIsReady(pDcpt->ftpcSocket.ftpcDataSkt);
        if( readLen  > 0)  
        {  
            
            if((readLen = NET_PRES_SocketRead(pDcpt->ftpcSocket.ftpcDataSkt, 
                                            pDcpt->ftpcDataRxBuff, readLen)) > 0)
            {
                pDcpt->ftpcDataLength = readLen;
                if(!ftpcDataEvent(pDcpt,TCPIP_FTPC_DATA_RCV,pDcpt->ftpcDataRxBuff,&(pDcpt->ftpcDataLength)))
                {  
                    if(pDcpt->fileDescr == SYS_FS_HANDLE_INVALID)
                    {   
                        //file name for storing on FTP Client
                        if(pDcpt->ftpcClientPathname)
                        {
                            pDcpt->filePathname = pDcpt->ftpcClientPathname;
                        }
                        else
                        {
                            pDcpt->filePathname = strrchr(pDcpt->ftpcServerPathname, '/');
                            if(!(pDcpt->filePathname))
                            {
                                pDcpt->filePathname = pDcpt->ftpcServerPathname;
                            }
                        }                        
                        
                        //open file
                        fileDescr = SYS_FS_FileOpen(pDcpt->filePathname,SYS_FS_FILE_OPEN_WRITE);
                        if( fileDescr != (int32_t) SYS_FS_HANDLE_INVALID )
                        {
                            pDcpt->fileDescr = fileDescr;
                            res = TCPIP_FTPC_RES_OK;
                        }
                        else
                        {
                            pDcpt->error = TCPIP_FTPC_ERROR_FILE_OPEN;
                            _ftpcCloseDataSkt(pDcpt);
                            _ftpcErrorUpdate(pDcpt);            
                            res = TCPIP_FTPC_RES_ERROR;
                        }                        
                    }                    
                
                    if(pDcpt->fileDescr != SYS_FS_HANDLE_INVALID)
                    {
                        if((int32_t)pDcpt->filePos > 0)
                        {
                            SYS_FS_FileSeek(pDcpt->fileDescr,(int32_t)pDcpt->filePos,SYS_FS_SEEK_SET);
                        }
                        if(SYS_FS_FileWrite(pDcpt->fileDescr,pDcpt->ftpcDataRxBuff,readLen) 
                                                                    == SYS_FS_HANDLE_INVALID)
                        {
                            SYS_FS_FileClose(pDcpt->fileDescr);
                            pDcpt->error = TCPIP_FTPC_ERROR_FILE_WRITE;
                            _ftpcCloseDataSkt(pDcpt);
                            _ftpcErrorUpdate(pDcpt); 
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
static TCPIP_FTPC_RESULT_TYPE _ftpcDoneCmd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{  
    //Command Success notification
    ftpcCtrlEvent(pDcpt,TCPIP_FTPC_CTRL_EVENT_SUCCESS,0,0);
    
    // returns the state machine to default home
    pDcpt->ftpcState = TCPIP_FTPC_STATE_HOME;
    return TCPIP_FTPC_RES_OK;
}

//FTP Client data transfer command completed
static TCPIP_FTPC_RESULT_TYPE _ftpcDoneFileTransfer(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{	
    _ftpcCloseDataSkt(pDcpt);
    //discard any data in control socket
    NET_PRES_SocketDiscard(pDcpt->ftpcSocket.ftpcCtrlSkt);
    pDcpt->filePathname = 0;
    ftpcCtrlEvent(pDcpt,TCPIP_FTPC_CTRL_EVENT_SUCCESS,0,0);
    // returns the state machine to default home
    pDcpt->ftpcState = TCPIP_FTPC_STATE_HOME;
    return TCPIP_FTPC_RES_OK;
}

//trigger callback on FTPC error
static void _ftpcErrorUpdate(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{ 
    // trigger call back for control socket
    ftpcCtrlEvent(pDcpt,TCPIP_FTPC_CTRL_EVENT_FAILURE,0,0);
    // returns the state machine to default home
    pDcpt->ftpcState = TCPIP_FTPC_STATE_HOME; 
    SYS_CONSOLE_PRINT("FTPC ERROR : %d\r\n", pDcpt->error);
}

// Disconnect Control socket and remove the descriptor
static void _ftpcCtrlDisconnect(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{       
    OSAL_CRITSECT_DATA_TYPE ftpcLock = ftpcEnterCritical();    
    pDcpt = (TCPIP_FTPC_DCPT_TYPE*)TCPIP_Helper_SingleListNodeRemove(&ftpcDcptBusyList, (SGL_LIST_NODE*)pDcpt);
    ftpcExitCritical(ftpcLock);

    if(pDcpt)
    {
        if(pDcpt->ftpcSocket.ftpcCtrlSkt != NET_PRES_INVALID_SOCKET)
        {
            //close control socket
            NET_PRES_SocketClose(pDcpt->ftpcSocket.ftpcCtrlSkt);
            pDcpt->ftpcSocket.ftpcCtrlSkt = NET_PRES_INVALID_SOCKET;
            
        }
        // Free Control Socket Buffers
        TCPIP_HEAP_Free(ftpcGlobalConfig.memH,pDcpt->ftpcCtrlRxBuff);
        TCPIP_HEAP_Free(ftpcGlobalConfig.memH,pDcpt->ftpcCtrlTxBuff);
        
        //Disconnect notification
        ftpcCtrlEvent(pDcpt,TCPIP_FTPC_CTRL_EVENT_DISCONNECTED,0,0);

        pDcpt->ftpcFlag = TCPIP_FTPC_FLAG_NONE;
        //clear callback for control connection
        pDcpt->ctrlSktCallback = 0;
        // returns the state machine to default home
        pDcpt->ftpcState = TCPIP_FTPC_STATE_HOME;

        ftpcLock = ftpcEnterCritical();
        TCPIP_Helper_SingleListTailAdd(&ftpcDcptFreeList, (SGL_LIST_NODE*)pDcpt);
        ftpcExitCritical(ftpcLock); 
    }    
}

static void _ftpcCloseDataSkt(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    if(pDcpt->ftpcSocket.ftpcDataSkt != NET_PRES_INVALID_SOCKET)
    {
        //close data socket
        NET_PRES_SocketClose(pDcpt->ftpcSocket.ftpcDataSkt);
        pDcpt->ftpcSocket.ftpcDataSkt = NET_PRES_INVALID_SOCKET;
    }
    if(pDcpt->ftpcDataConnection.ftpcDataTxBuffSize)
    {
        TCPIP_HEAP_Free(ftpcGlobalConfig.memH,pDcpt->ftpcDataTxBuff);
        pDcpt->ftpcDataConnection.ftpcDataTxBuffSize = 0;
    }
    if(pDcpt->ftpcDataConnection.ftpcDataRxBuffSize)
    {
        TCPIP_HEAP_Free(ftpcGlobalConfig.memH,pDcpt->ftpcDataRxBuff);
        pDcpt->ftpcDataConnection.ftpcDataRxBuffSize = 0;
    }
}

static void _ftpcDcptSetDefault(TCPIP_FTPC_DCPT_TYPE* pDcpt, uint16_t index)
{
    pDcpt->ftpcSocket.ftpcCtrlSkt = NET_PRES_INVALID_SOCKET; 
    pDcpt->ftpcSocket.ftpcDataSkt = NET_PRES_INVALID_SOCKET; 
    pDcpt->fileDescr = (int32_t) SYS_FS_HANDLE_INVALID;
    pDcpt->ftpcDcptIndex = index;
    pDcpt->error = TCPIP_FTPC_ERROR_NONE;
    pDcpt->ftpcFlag  = TCPIP_FTPC_FLAG_NONE;
    pDcpt->ftpcState = TCPIP_FTPC_STATE_HOME;
    pDcpt->ftpcCommand = TCPIP_FTPC_CMD_NONE;
    pDcpt->dataSktCallback = 0;
    pDcpt->ctrlSktCallback = 0;
    pDcpt->filePos = 0;
    pDcpt->ftpcSignal = TCPIP_FTPC_SIGNAL_NONE;
    pDcpt->waitTick = 0;
    pDcpt->ftpcActive = false; 
    memset(&(pDcpt->ftpcDataConnection), 0, sizeof(TCPIP_FTPC_DATA_CONN_TYPE));
}

static TCPIP_FTPC_RESULT_TYPE _ftpcSetDataSktBuff(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    uint16_t sktRxSize = 0;
    uint16_t sktTxSize = 0;
    
    if(pDcpt->ftpcDataConnection.ftpcDataTxBuffSize)
    {
        //set the socket Tx buffer size
        NET_PRES_SocketOptionsSet(pDcpt->ftpcSocket.ftpcDataSkt, TCP_OPTION_TX_BUFF, 
                (void *)(unsigned int)(pDcpt->ftpcDataConnection.ftpcDataTxBuffSize));
    }
    else
    {
        if(ftpcGlobalConfig.data_tx_buffsize_dflt )
        {
            pDcpt->ftpcDataConnection.ftpcDataTxBuffSize = ftpcGlobalConfig.data_tx_buffsize_dflt ;
            //set the socket Tx buffer size
            NET_PRES_SocketOptionsSet(pDcpt->ftpcSocket.ftpcDataSkt, 
                TCP_OPTION_TX_BUFF, (void *)(unsigned int)(pDcpt->ftpcDataConnection.ftpcDataTxBuffSize));
        }
        else if(NET_PRES_SocketOptionsGet(pDcpt->ftpcSocket.ftpcDataSkt, 
                                                TCP_OPTION_TX_BUFF, &sktTxSize))
        {
            pDcpt->ftpcDataConnection.ftpcDataTxBuffSize = sktTxSize;
        }
    }
    pDcpt->ftpcDataConnection.ftpcDataTxBuffSize  &= FTPC_BUFF_SIZE_ALIGN_MASK;
    pDcpt->ftpcDataTxBuff = (char *)TCPIP_HEAP_Malloc(ftpcGlobalConfig.memH, 
                                    pDcpt->ftpcDataConnection.ftpcDataTxBuffSize );  
    
    if(pDcpt->ftpcDataConnection.ftpcDataRxBuffSize )
    {
        //set the socket Rx buffer size
        NET_PRES_SocketOptionsSet(pDcpt->ftpcSocket.ftpcDataSkt, TCP_OPTION_RX_BUFF, 
                (void *)(unsigned int)(pDcpt->ftpcDataConnection.ftpcDataRxBuffSize ));
    }
    else
    {
        if(ftpcGlobalConfig.data_rx_buffsize_dflt )
        {
            pDcpt->ftpcDataConnection.ftpcDataRxBuffSize  = ftpcGlobalConfig.data_rx_buffsize_dflt ;
            //set the socket Rx buffer size
            NET_PRES_SocketOptionsSet(pDcpt->ftpcSocket.ftpcDataSkt, TCP_OPTION_RX_BUFF, 
                    (void *)(unsigned int)(pDcpt->ftpcDataConnection.ftpcDataRxBuffSize ));
        }
        else if(NET_PRES_SocketOptionsGet(pDcpt->ftpcSocket.ftpcDataSkt, TCP_OPTION_RX_BUFF, &sktRxSize))
        {
            pDcpt->ftpcDataConnection.ftpcDataRxBuffSize  = sktRxSize;
        }
    }
    
    NET_PRES_SocketOptionsGet(pDcpt->ftpcSocket.ftpcDataSkt, TCP_OPTION_RX_BUFF, &sktRxSize);
    pDcpt->ftpcDataConnection.ftpcDataRxBuffSize  &= FTPC_BUFF_SIZE_ALIGN_MASK;
    pDcpt->ftpcDataRxBuff = (char *)TCPIP_HEAP_Malloc(ftpcGlobalConfig.memH, 
                                pDcpt->ftpcDataConnection.ftpcDataRxBuffSize ); 
    return TCPIP_FTPC_RES_OK;
}


// signals an FTPC Control Socket event
static void ftpcCtrlEvent(TCPIP_FTPC_DCPT_TYPE* pDcpt, TCPIP_FTPC_CTRL_EVENT_TYPE ftpcEvent,
                          char * ctrlbuff, uint16_t ctrllen)
{
    if(pDcpt->ctrlSktCallback != 0)
    {
        (*(pDcpt->ctrlSktCallback ))((TCPIP_FTPC_CONN_HANDLE_TYPE)pDcpt, ftpcEvent,
                pDcpt->ftpcCommand, ctrlbuff, ctrllen);
    }
}

// signals an FTPC Data Socket event
static bool ftpcDataEvent(TCPIP_FTPC_DCPT_TYPE* pDcpt, TCPIP_FTPC_DATA_EVENT_TYPE ftpcEvent,
                          char * databuff, uint16_t * datalen)
{
    if(pDcpt->dataSktCallback != 0)
    {
        return (*(pDcpt->dataSktCallback ))((TCPIP_FTPC_CONN_HANDLE_TYPE)pDcpt, ftpcEvent,
                                                    pDcpt->ftpcCommand, databuff, datalen);
    }
    else
        return false;
}

#endif //#if defined(TCPIP_STACK_USE_FTP_CLIENT)
