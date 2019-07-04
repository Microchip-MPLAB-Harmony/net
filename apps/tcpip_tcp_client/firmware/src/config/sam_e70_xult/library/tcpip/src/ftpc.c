/*******************************************************************************
  File Transfer Protocol (SMTP) Client
  Module for Microchip TCP/IP Stack

  Summary:
    Implementation of FTP Client protocol
    
  Description:
    - SMTP client provides ability to send user emails.
    - Reference: RFC 5321, 5322, 6152, 6409, 6854, 2034, 3207, 4616, 4648, 4954, 5248,
                 RFC 2045, 2046,2047, 4289, 2049, 2183, 2387, 2557, 6552, 6838

*******************************************************************************/

/*******************************************************************************
File Name:  ftpc.c
Copyright © 2016 released Microchip Technology Inc.  All rights
reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
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
#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_FTPC

#include "stdlib.h"
#include "ctype.h"

#include "tcpip/src/tcpip_private.h"
#include "net_pres/pres/net_pres_socketapi.h"

#include "tcpip/src/common/sys_fs_wrapper.h"

#if defined(TCPIP_STACK_USE_FTPC)

#include "tcpip/src/ftpc_private.h"
static void TCPIP_FTPC_Process(void);
/****************************************************************************
  Section:
	FTPC definitions
  ***************************************************************************/

/****************************************************************************
  Section:
	FTPC Internal Variables
  ***************************************************************************/
static int          ftpcInitCount = 0;       			// module initialization count
static tcpipSignalHandle    ftpcSignalHandle = 0;
static TCPIP_FTPC_DCPT_TYPE* ftpcPool = 0;    		// allocated pool of descriptors
static TCPIP_FTPC_MODULE_CONFIG_TYPE ftpcGlobalConfig;// global FTP Client configuration
static SINGLE_LIST  ftpcDcptFreeList = { 0 };      // pool of free descriptors 
static SINGLE_LIST  ftpcDcptBusyList = { 0 };      // pool of descriptors in use


 
// status functions
// processes a current state and returns a new state
typedef TCPIP_FTPC_STATE_TYPE(*TCPIP_FTPC_STATE_FUNC)(TCPIP_FTPC_DCPT_TYPE* pDcpt);

static TCPIP_FTPC_STATE_TYPE ftpcStateHome(TCPIP_FTPC_DCPT_TYPE* pDcpt);
//Connect Routines
static TCPIP_FTPC_STATE_TYPE ftpcStateStartConnect(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitConnect(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateGetConnectGreeting(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateDoneConnect(TCPIP_FTPC_DCPT_TYPE* pDcpt);
//Login Routines
static TCPIP_FTPC_STATE_TYPE ftpcStateSendUser(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitUser(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateSendPass(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitPass(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateSendAcct(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitAcct(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateDoneLogin(TCPIP_FTPC_DCPT_TYPE* pDcpt);
//CWD Routines
static TCPIP_FTPC_STATE_TYPE ftpcStateSendCwd(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitCwd(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateDoneCwd(TCPIP_FTPC_DCPT_TYPE* pDcpt);
//CDUP Routines
static TCPIP_FTPC_STATE_TYPE ftpcStateSendCdup(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitCdup(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateDoneCdup(TCPIP_FTPC_DCPT_TYPE* pDcpt);
//MKD Routines
static TCPIP_FTPC_STATE_TYPE ftpcStateSendMkd(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitMkd(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateDoneMkd(TCPIP_FTPC_DCPT_TYPE* pDcpt);
//RMD Routines
static TCPIP_FTPC_STATE_TYPE ftpcStateSendRmd(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitRmd(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateDoneRmd(TCPIP_FTPC_DCPT_TYPE* pDcpt);
//PWD Routines
static TCPIP_FTPC_STATE_TYPE ftpcStateSendPwd(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitPwd(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateDonePwd(TCPIP_FTPC_DCPT_TYPE* pDcpt);
//TYPE Routines
static TCPIP_FTPC_STATE_TYPE ftpcStateSendType(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitType(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateDoneType(TCPIP_FTPC_DCPT_TYPE* pDcpt);
//GET routines
static TCPIP_FTPC_STATE_TYPE ftpcStateStartGet(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateDoneGet(TCPIP_FTPC_DCPT_TYPE* pDcpt);
//Open Data Socket
static TCPIP_FTPC_STATE_TYPE ftpcStateOpenDataSkt(TCPIP_FTPC_DCPT_TYPE* pDcpt);
//PORT Routines
static TCPIP_FTPC_STATE_TYPE ftpcStateSendPort(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitPort(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateDonePort(TCPIP_FTPC_DCPT_TYPE* pDcpt);
//PASV Routines
static TCPIP_FTPC_STATE_TYPE ftpcStateSendPasv(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitPasv(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateDonePasv(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitPasvConnect(TCPIP_FTPC_DCPT_TYPE* pDcpt);
//RETR Routines
static TCPIP_FTPC_STATE_TYPE ftpcStateSendRetr(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitRetrCtrlResp(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitRetrReadDataSkt(TCPIP_FTPC_DCPT_TYPE* pDcpt);
//static TCPIP_FTPC_STATE_TYPE ftpcStateWaitRetr(TCPIP_FTPC_DCPT_TYPE* pDcpt);
//static TCPIP_FTPC_STATE_TYPE ftpcStateWaitRetr1(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateDoneRetr(TCPIP_FTPC_DCPT_TYPE* pDcpt);
//STOR Routines
static TCPIP_FTPC_STATE_TYPE ftpcStateSendStor(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitStor(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateDoneStor(TCPIP_FTPC_DCPT_TYPE* pDcpt);
//NLST Routines
static TCPIP_FTPC_STATE_TYPE ftpcStateSendNlst(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitNlst(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateDoneNlst(TCPIP_FTPC_DCPT_TYPE* pDcpt);
//Process Control Socket reply and return the REPLY CODE
static int ftpcGetReplyCode(char* buff);
//Process Control Socket reply and return the pathname
static int ftpcGetReplyPathname(TCPIP_FTPC_DCPT_TYPE* pDcpt, char* pathname, size_t length);

static void ftpcSetDcptTmo(TCPIP_FTPC_DCPT_TYPE* pDcpt, uint32_t tmo);
static bool ftpcIsDcptTmoExpired(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcStateDisconnect(TCPIP_FTPC_DCPT_TYPE* pDcpt);
//Error Handling master routine
static TCPIP_FTPC_STATE_TYPE ftpcStateError(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE ftpcReadCtrlSocket(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_STATE_TYPE ftpcProcessCtrlResponse(TCPIP_FTPC_DCPT_TYPE* pDcpt);

static const TCPIP_FTPC_STATE_FUNC ftpcStateTbl[] = 
{
    ftpcStateHome,                  // TCPIP_FTPC_STATE_HOME
    //Connect Routines
    ftpcStateStartConnect,          // TCPIP_FTPC_STATE_START_CONNECT
    ftpcStateWaitConnect,           // TCPIP_FTPC_STATE_WAIT_CONNECT
    ftpcStateGetConnectGreeting,    // TCPIP_FTPC_STATE_GET_GREETING
    ftpcStateDoneConnect,           //TCPIP_FTPC_STATE_DONE_CONNECT
    //Login Routines
    ftpcStateSendUser,              //TCPIP_FTPC_STATE_LOGIN_SEND_USER  
    ftpcStateWaitUser,              //TCPIP_FTPC_STATE_LOGIN_WAIT_USER
    ftpcStateSendPass,              //TCPIP_FTPC_STATE_LOGIN_SEND_PASS
    ftpcStateWaitPass,              //TCPIP_FTPC_STATE_LOGIN_WAIT_PASS
    ftpcStateSendAcct,              //TCPIP_FTPC_STATE_LOGIN_SEND_ACCT
    ftpcStateWaitAcct,              //TCPIP_FTPC_STATE_LOGIN_WAIT_ACCT
    ftpcStateDoneLogin,             //TCPIP_FTPC_STATE_DONE_LOGIN
    //CWD Routines
    ftpcStateSendCwd,               //TCPIP_FTPC_STATE_SEND_CWD  
    ftpcStateWaitCwd,               //TCPIP_FTPC_STATE_WAIT_CWD
    ftpcStateDoneCwd,               //TCPIP_FTPC_STATE_DONE_CWD
    //CDUP Routines        
    ftpcStateSendCdup,              //TCPIP_FTPC_STATE_SEND_CDUP  
    ftpcStateWaitCdup,              //TCPIP_FTPC_STATE_WAIT_CDUP
    ftpcStateDoneCdup,              //TCPIP_FTPC_STATE_DONE_CDUP
    //MKD Routines        
    ftpcStateSendMkd,              //TCPIP_FTPC_STATE_SEND_MKD  
    ftpcStateWaitMkd,              //TCPIP_FTPC_STATE_WAIT_MKD
    ftpcStateDoneMkd,              //TCPIP_FTPC_STATE_DONE_MKD
    //RMD Routines        
    ftpcStateSendRmd,              //TCPIP_FTPC_STATE_SEND_RMD,  
    ftpcStateWaitRmd,              //TCPIP_FTPC_STATE_WAIT_RMD,
    ftpcStateDoneRmd,              //TCPIP_FTPC_STATE_DONE_RMD,
    //PWD Routines        
    ftpcStateSendPwd,              //TCPIP_FTPC_STATE_SEND_PWD,  
    ftpcStateWaitPwd,              //TCPIP_FTPC_STATE_WAIT_PWD,
    ftpcStateDonePwd,              //TCPIP_FTPC_STATE_DONE_PWD,
    //TYPE Routines        
    ftpcStateSendType,              //TCPIP_FTPC_STATE_SEND_TYPE,  
    ftpcStateWaitType,              //TCPIP_FTPC_STATE_WAIT_TYPE,
    ftpcStateDoneType,              //TCPIP_FTPC_STATE_DONE_TYPE,  
    //Get File Routines
    ftpcStateStartGet,              //TCPIP_FTPC_STATE_START_GET
    ftpcStateDoneGet,              //TCPIP_FTPC_STATE_START_GET
    //Open Data Socket
    ftpcStateOpenDataSkt,           //TCPIP_FTPC_STATE_OPEN_DATA_SKT
    //PORT routines
    ftpcStateSendPort,              //TCPIP_FTPC_STATE_SEND_PORT,  
    ftpcStateWaitPort,              //TCPIP_FTPC_STATE_WAIT_PORT,
    ftpcStateDonePort,              //TCPIP_FTPC_STATE_DONE_PORT, 
    //PASV routines
    ftpcStateSendPasv,              //TCPIP_FTPC_STATE_SEND_PASV,  
    ftpcStateWaitPasv,              //TCPIP_FTPC_STATE_WAIT_PASV,
    ftpcStateDonePasv,              //TCPIP_FTPC_STATE_DONE_PASV,  
    ftpcStateWaitPasvConnect,       //TCPIP_FTPC_STATE_PASV_WAIT_CONNECT
    
    //RETR States        
    ftpcStateSendRetr,              //TCPIP_FTPC_STATE_SEND_RETR,  
    ftpcStateWaitRetrCtrlResp,      //TCPIP_FTPC_STATE_WAIT_RETR_CTRL_RESPONSE,
    ftpcStateWaitRetrReadDataSkt,   //TCPIP_FTPC_STATE_WAIT_RETR_READ_DATA_SOCKET,
    //ftpcStateWaitRetr,              //TCPIP_FTPC_STATE_WAIT_RETR,
    //ftpcStateWaitRetr1,
    ftpcStateDoneRetr,              //TCPIP_FTPC_STATE_DONE_RETR,
    ////////////////////////////////////
    //STOR States        
    //ftpcStateSendStor,              //TCPIP_FTPC_STATE_SEND_STOR,  
   // ftpcStateWaitStor,              //TCPIP_FTPC_STATE_WAIT_STOR,
    //ftpcStateDoneStor,              //TCPIP_FTPC_STATE_DONE_STOR,
    //ftpcStateSendNlst,              //TCPIP_FTPC_STATE_SEND_NLST,  
    //ftpcStateWaitNlst,              //TCPIP_FTPC_STATE_WAIT_NLST,
    //ftpcStateDoneNlst,              //TCPIP_FTPC_STATE_DONE_NLST,
    
    
    ftpcStateDisconnect,           //TCPIP_FTPC_STATE_DISCONNECT
    //Error Routine
    ftpcStateError,                 // TCPIP_FTPC_STATE_ERROR
};
  
static __inline__ OSAL_CRITSECT_DATA_TYPE __attribute__((always_inline)) ftpcEnterCritical(void)
{
    return OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
}

static __inline__ void __attribute__((always_inline)) ftpcExitCritical(OSAL_CRITSECT_DATA_TYPE critStatus)
{
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critStatus);
}


bool TCPIP_FTPC_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_FTPC_MODULE_CONFIG_TYPE* pftpcConfig)
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
        ftpcSignalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_FTPC_Task, TCPIP_FTPC_TASK_TICK_RATE);
		if(ftpcSignalHandle == 0)
        {   // cannot create the FTPC timer
            return false;
        }
		// initialize the in-use list
        TCPIP_Helper_SingleListInitialize(&ftpcDcptBusyList);
		// initialize the free pool
        TCPIP_Helper_SingleListInitialize(&ftpcDcptFreeList);
		
		// allocate the descriptors pool
        ftpcPool = (TCPIP_FTPC_DCPT_TYPE*)TCPIP_HEAP_Calloc(stackCtrl->memH, pftpcConfig->nMaxClients, sizeof(*ftpcPool));
		if(ftpcPool == 0)
        {   // cannot create the FTPC pool, so remove FTPC timer
            _TCPIPStackSignalHandlerDeregister(ftpcSignalHandle);
			ftpcSignalHandle = 0;
            return false;
        }
		
		pftpcDcpt = ftpcPool;
		
		for(index = 0; index < pftpcConfig->nMaxClients; index++, pftpcDcpt++)
        {
            pftpcDcpt->ftpcSocket.ftpcCtrlSkt = NET_PRES_INVALID_SOCKET; 
            pftpcDcpt->ftpcSocket.ftpcDataSkt = NET_PRES_INVALID_SOCKET; 
            pftpcDcpt->ftpcDcptIndex = index;
            pftpcDcpt->ftpcError = TCPIP_FTPC_ERROR_NONE;
            pftpcDcpt->ftpcFlag  = TCPIP_FTPC_FLAG_NONE;
            pftpcDcpt->ftpcState = TCPIP_FTPC_STATE_HOME;
            TCPIP_Helper_SingleListTailAdd(&ftpcDcptFreeList, (SGL_LIST_NODE*)pftpcDcpt);
        }
        ftpcGlobalConfig.nMaxClients = pftpcConfig->nMaxClients;
        ftpcGlobalConfig.memH = stackCtrl->memH;
        ftpcGlobalConfig.dataTxBuffSize = pftpcConfig->dataTxBuffSize;
		ftpcGlobalConfig.dataRxBuffSize = pftpcConfig->dataRxBuffSize;		
        ftpcGlobalConfig.ctrlCmdRxBuffSize = pftpcConfig->ctrlCmdRxBuffSize > TCPIP_FTPC_CMD_RX_BUFFSIZE_DFLT ? pftpcConfig->ctrlCmdRxBuffSize : TCPIP_FTPC_CMD_RX_BUFFSIZE_DFLT;
		ftpcGlobalConfig.ctrlCmdTxBuffSize = pftpcConfig->ctrlCmdTxBuffSize > TCPIP_FTPC_CMD_TX_BUFFSIZE_DFLT ? pftpcConfig->ctrlCmdTxBuffSize : TCPIP_FTPC_CMD_TX_BUFFSIZE_DFLT;
		ftpcGlobalConfig.serverCmdTmo = pftpcConfig->serverCmdTmo > TCPIP_FTPC_MIN_SERVERREPLY_TMO ? pftpcConfig->serverCmdTmo : TCPIP_FTPC_MIN_SERVERREPLY_TMO;
		ftpcGlobalConfig.serverDataTmo = pftpcConfig->serverDataTmo > TCPIP_FTPC_MIN_SERVERDATA_TMO ? pftpcConfig->serverDataTmo : TCPIP_FTPC_MIN_SERVERDATA_TMO;
	}
	
	ftpcInitCount++;
    return true;
}

void TCPIP_FTPC_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    
}

void  TCPIP_FTPC_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != 0)
    { // TMO or RX signals occurred
      TCPIP_FTPC_Process();
    }
}

// send a signal to the FTPC module that data is available
// no manager alert needed since this normally results as a higher layer (TCP) signal
//niyas : you might need to use this handler for control socket and data socket
static void _FTPCSocketRxSignalHandler(NET_PRES_SKT_HANDLE_T hTCP, NET_PRES_SIGNAL_HANDLE hNet, uint16_t sigType, const void* param)
{
    if(sigType == TCPIP_TCP_SIGNAL_RX_DATA)
    {
        _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}

static void TCPIP_FTPC_Process(void)
{
	TCPIP_FTPC_DCPT_TYPE* pCurrDcpt, *pNextDcpt, *pPrevDcpt;
    TCPIP_FTPC_STATE_TYPE newStat;

    OSAL_CRITSECT_DATA_TYPE ftpcLock = ftpcEnterCritical();
    pCurrDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcDcptBusyList.head;
    pNextDcpt = pCurrDcpt? pCurrDcpt->next : 0;
    ftpcExitCritical(ftpcLock);
    pPrevDcpt = 0;
	
	while(pCurrDcpt != 0)
    {
        // process this descriptor
        // check for possible disconnect
       /* if((pCurrDcpt->ftpcFlag & TCPIP_FTPC_FLAG_CONNECTED) != 0 )
        {   // lost connection to the server
            //pCurrDcpt->ftpcFlag &= ~(TCPIP_SMTPC_SERVER_FLAG_CONN_MASK);
            newStat = TCPIP_FTPC_STATE_DISCONNECT; //todo niyas : check this condition
        }
        else*/
        if(pCurrDcpt->ftpcBusy)
        {
            newStat = (*ftpcStateTbl[pCurrDcpt->ftpcState])(pCurrDcpt);
        }
        else
        {
           ; /*ftpcLock = ftpcEnterCritical();
            pCurrDcpt = pNextDcpt;
            pNextDcpt = pCurrDcpt? pCurrDcpt->next : 0;
            ftpcExitCritical(ftpcLock);*/
        }
        //smtpcSetStatus(pCurrDcpt, newStat);

        /*ftpcLock = ftpcEnterCritical();
        if(newStat == TCPIP_FTPC_STATE_DISCONNECT)//todo niyas : check this condition
        {
            TCPIP_Helper_SingleListNextRemove(&ftpcDcptBusyList, (SGL_LIST_NODE*)pPrevDcpt);
            TCPIP_Helper_SingleListTailAdd(&ftpcDcptFreeList, (SGL_LIST_NODE*)pCurrDcpt);
        }
        else
        {
            pPrevDcpt = pCurrDcpt;
        }
        pCurrDcpt = pNextDcpt;
        pNextDcpt = pCurrDcpt? pCurrDcpt->next : 0;
        ftpcExitCritical(ftpcLock);*/
        ftpcLock = ftpcEnterCritical();
        pCurrDcpt = pNextDcpt;
        pNextDcpt = pCurrDcpt? pCurrDcpt->next : 0;
        ftpcExitCritical(ftpcLock);
    }
    
}


TCPIP_FTPC_CONN_HANDLE_TYPE TCPIP_FTPC_Connect(TCPIP_FTPC_CTRL_CONN_TYPE *pftpcConn, TCPIP_FTPC_RESULT_TYPE* pResult)
{
    TCPIP_FTPC_DCPT_TYPE* pDcpt = 0;
    char *rxBuffer;
    char *txBuffer;
    
    // Get a free descriptor for new connection
    OSAL_CRITSECT_DATA_TYPE ftpcLock = ftpcEnterCritical();
    pDcpt = (TCPIP_FTPC_DCPT_TYPE*)TCPIP_Helper_SingleListHeadRemove(&ftpcDcptFreeList);
    ftpcExitCritical(ftpcLock);
    SYS_CONSOLE_MESSAGE("TCPIP_FTPC_Connect()\r\n");
    if(pDcpt)
    {
        *pResult = TCPIP_FTPC_RES_OK;
       // if(pDcpt->ftpcState == TCPIP_FTPC_STATE_HOME)
        {
            SYS_CONSOLE_MESSAGE("state home\r\n");
            pDcpt->ftpcState = TCPIP_FTPC_STATE_START_CONNECT;
            //Initialize the descriptor Connection details
           // pDcpt->ftpcCtrlConnection.cmdSktCallback = pftpcConn->cmdSktCallback;
           // pDcpt->ftpcCtrlConnection.dataSktCallback = pftpcConn->dataSktCallback;
            //pDcpt->ftpcCtrlConnection.ftpcDataStructure = pftpcConn->ftpcDataStructure;
            //pDcpt->ftpcCtrlConnection.ftpcDataType = pftpcConn->ftpcDataType;
           // pDcpt->ftpcCtrlConnection.ftpcIsPassiveMode = pftpcConn->ftpcIsPassiveMode;
            pDcpt->ftpcCtrlConnection.ftpcServerAddr = pftpcConn->ftpcServerAddr;
            pDcpt->ftpcCtrlConnection.ftpcServerIpAddrType = pftpcConn->ftpcServerIpAddrType;
            //pDcpt->ftpcCtrlConnection.ftpcTransferMode = pftpcConn->ftpcTransferMode;
            pDcpt->ftpcCtrlConnection.serverCtrlPort = (pftpcConn->serverCtrlPort == 0)? TCPIP_FTPC_DFLT_SRV_CTRL_PORT:pftpcConn->serverCtrlPort;
           // pDcpt->ftpcCtrlConnection.neth = pftpcConn->neth;
            pDcpt->ftpcBusy = true;
                        
            //Allocate and Add Tx buffer for Ctrl socket for commands
            txBuffer = (char *)TCPIP_HEAP_Malloc(ftpcGlobalConfig.memH, ftpcGlobalConfig.ctrlCmdTxBuffSize);
            //Allocate and Add Rx buffer for Ctrl socket for command response
            rxBuffer = (char *)TCPIP_HEAP_Malloc(ftpcGlobalConfig.memH, ftpcGlobalConfig.ctrlCmdRxBuffSize);
            
            pDcpt->ftpcCtrlTxBuff = txBuffer;
            pDcpt->ftpcCtrlRxBuff = rxBuffer;            
            ftpcLock = ftpcEnterCritical();
            TCPIP_Helper_SingleListTailAdd(&ftpcDcptBusyList, (SGL_LIST_NODE*)pDcpt);
            ftpcExitCritical(ftpcLock);
            
            //pDcpt->ftpcFlag =  TCPIP_FTPC_FLAG_NEW_REQ;
        }
       /* else
        {
            SYS_CONSOLE_MESSAGE("state error\r\n");
            *pResult = TCPIP_FTPC_RES_CONNECT_INIT_STATE_ERROR;
            pDcpt->ftpcError = TCPIP_FTPC_ERROR_INIT_STATE;
            pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR;
        }  */      
    }
    else
    {
        *pResult = TCPIP_FTPC_RES_CONNECT_DCPT_ERROR;
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_DCPT;
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR;
    }

    
    return (TCPIP_FTPC_CONN_HANDLE_TYPE)pDcpt;
}

TCPIP_FTPC_RESULT_TYPE TCPIP_FTPC_Is_Connected(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle)
{
    TCPIP_FTPC_RESULT_TYPE ftpcResult;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    
    if(pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_CONNECTED)
    {
        ftpcResult = TCPIP_FTPC_RES_OK;
    }
    else
    {
        ftpcResult = TCPIP_FTPC_RES_ERROR;
    }
        
    return ftpcResult;
}

TCPIP_FTPC_RESULT_TYPE TCPIP_FTPC_Disconnect (TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle)
{
    return (TCPIP_FTPC_RESULT_TYPE)0;
}

bool TCPIP_FTPC_Is_Ready(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle)
{
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    return !pDcpt->ftpcBusy;
}

char * TCPIP_FTPC_Read_CMD_REPLY(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle)
{    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
   
    return pDcpt->ftpcCtrlRxBuff;
}

TCPIP_FTPC_RESULT_TYPE TCPIP_FTPC_Login(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, const char *username, const char *password, const char *account)
{
    TCPIP_FTPC_RESULT_TYPE ftpcResult;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    
    if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_CONNECTED) != 0 ) //niyas see whether this condition is right
    {
        pDcpt->ftpcAccessControl.ftpcUserName = username;
        pDcpt->ftpcAccessControl.ftpcPassword = password;
        pDcpt->ftpcAccessControl.ftpcAccount = account;
        //memset(pDcpt->ftpcCtrlRxBuff, 0, ftpcGlobalConfig.ctrlCmdRxBuffSize);
        pDcpt->ftpcState = TCPIP_FTPC_STATE_LOGIN_SEND_USER;
        ftpcResult = TCPIP_FTPC_RES_OK;
        pDcpt->ftpcBusy = true;
    }
    else
    {
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_INIT_STATE; //niyas see whether this condition is right
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR;
        ftpcResult = TCPIP_FTPC_RES_LOGIN_INIT_STATE_ERROR;
    }
    
    return ftpcResult;
}

TCPIP_FTPC_RESULT_TYPE TCPIP_FTPC_Is_LoggedIn(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle)
{
    TCPIP_FTPC_RESULT_TYPE ftpcResult;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    
    if(pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN)
    {
        ftpcResult = TCPIP_FTPC_RES_OK;
    }
    else
    {
        ftpcResult = TCPIP_FTPC_RES_ERROR;
    }
        
    return ftpcResult;
}

TCPIP_FTPC_RESULT_TYPE TCPIP_FTPC_Change_Dir(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, const char *pathname)
{
    TCPIP_FTPC_RESULT_TYPE ftpcResult;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    
    if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 ) //niyas see whether this condition is right
    {
        pDcpt->ftpcServerPathname = pathname;
      
        pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_CWD;
        ftpcResult = TCPIP_FTPC_RES_OK;
        pDcpt->ftpcBusy = true;
    }
    else
    {
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_INIT_STATE; //niyas see whether this condition is right
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR;
        ftpcResult = TCPIP_FTPC_RES_CWD_INIT_STATE_ERROR;
    }
    
    return ftpcResult;
}

TCPIP_FTPC_RESULT_TYPE TCPIP_FTPC_ChangeToParentDir(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle)
{
    TCPIP_FTPC_RESULT_TYPE ftpcResult;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    
    if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 ) //niyas see whether this condition is right
    {              
        pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_CDUP;
        ftpcResult = TCPIP_FTPC_RES_OK;
        pDcpt->ftpcBusy = true;
    }
    else
    {
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_INIT_STATE; //niyas see whether this condition is right
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR;
        ftpcResult = TCPIP_FTPC_RES_CDUP_INIT_STATE_ERROR;
    }
    
    return ftpcResult;   
}

TCPIP_FTPC_RESULT_TYPE TCPIP_FTPC_MakeDir(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, const char *pathname)
{
    TCPIP_FTPC_RESULT_TYPE ftpcResult;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    
    if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 ) //niyas see whether this condition is right
    {
        pDcpt->ftpcServerPathname = pathname;
      
        pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_MKD;
        ftpcResult = TCPIP_FTPC_RES_OK;
        pDcpt->ftpcBusy = true;
    }
    else
    {
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_INIT_STATE; //niyas see whether this condition is right
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR;
        ftpcResult = TCPIP_FTPC_RES_MKD_INIT_STATE_ERROR;
    }
    
    return ftpcResult;
}

TCPIP_FTPC_RESULT_TYPE TCPIP_FTPC_RemoveDir(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, const char *pathname)
{
    TCPIP_FTPC_RESULT_TYPE ftpcResult;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    
    if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 ) //niyas see whether this condition is right
    {
        pDcpt->ftpcServerPathname = pathname;
      
        pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_RMD;
        ftpcResult = TCPIP_FTPC_RES_OK;
        pDcpt->ftpcBusy = true;
    }
    else
    {
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_INIT_STATE; //niyas see whether this condition is right
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR;
        ftpcResult = TCPIP_FTPC_RES_MKD_INIT_STATE_ERROR;
    }
    
    return ftpcResult;
}

TCPIP_FTPC_RESULT_TYPE TCPIP_FTPC_Get_WorkingDir (TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle)
{
    TCPIP_FTPC_RESULT_TYPE ftpcResult;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    
    if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 ) //niyas see whether this condition is right
    {
        pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_PWD;
        ftpcResult = TCPIP_FTPC_RES_OK;
       // memset(pDcpt->ftpcCtrlRxBuff, 0, ftpcGlobalConfig.ctrlCmdRxBuffSize);
        pDcpt->ftpcBusy = true;
    }
    else
    {
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_INIT_STATE; //niyas see whether this condition is right
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR;
        ftpcResult = TCPIP_FTPC_RES_MKD_INIT_STATE_ERROR;
    }
    
    return ftpcResult;    
}
TCPIP_FTPC_RESULT_TYPE TCPIP_FTPC_SetType(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, TCPIP_FTPC_DATA_REP_TYPE type)
{
    TCPIP_FTPC_RESULT_TYPE ftpcResult;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    
    if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 ) //niyas see whether this condition is right
    {
        pDcpt->ftpcDataConnection.ftpcDataType = type;
      
        pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_TYPE;
        ftpcResult = TCPIP_FTPC_RES_OK;
        pDcpt->ftpcBusy = true;
    }
    else
    {
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_INIT_STATE; //niyas see whether this condition is right
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR;
        ftpcResult = TCPIP_FTPC_RES_MKD_INIT_STATE_ERROR;
    }
    
    return ftpcResult;
}

TCPIP_FTPC_RESULT_TYPE TCPIP_FTPC_SetActiveMode(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle)
{
    TCPIP_FTPC_RESULT_TYPE ftpcResult;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    
    if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 ) //niyas see whether this condition is right
    {        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_PORT;
        pDcpt->ftpcDataConnection.ftpcIsPassiveMode = false;
        ftpcResult = TCPIP_FTPC_RES_OK;
        pDcpt->ftpcBusy = true;
    }
    else
    {
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_INIT_STATE; //niyas see whether this condition is right
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR;
        ftpcResult = TCPIP_FTPC_RES_MKD_INIT_STATE_ERROR;
    }
    
    return ftpcResult;
}

TCPIP_FTPC_RESULT_TYPE TCPIP_FTPC_SetPassiveMode(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle)
{
    TCPIP_FTPC_RESULT_TYPE ftpcResult;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    
    if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 ) //niyas see whether this condition is right
    {
        pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_PASV;
        pDcpt->ftpcDataConnection.ftpcIsPassiveMode = true;
        ftpcResult = TCPIP_FTPC_RES_OK;
        pDcpt->ftpcBusy = true;
    }
    else
    {
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_INIT_STATE; //niyas see whether this condition is right
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR;
        ftpcResult = TCPIP_FTPC_RES_MKD_INIT_STATE_ERROR;
    }
    
    return ftpcResult;
}

TCPIP_FTPC_RESULT_TYPE TCPIP_FTPC_GetFile(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, const char *src_pathname, TCPIP_FTPC_DATA_CONN_TYPE *pftpcDataConn, const char *dst_pathname)
{
    TCPIP_FTPC_RESULT_TYPE ftpcResult;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    
    if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 ) //niyas see whether this condition is right
    {
        pDcpt->ftpcServerPathname = src_pathname;
        pDcpt->ftpcClientPathname = dst_pathname;
        //pDcpt->ftpcDataRxBuff = pFtpcRxBuffer;
        pDcpt->ftpcDataConnection.ftpcDataType = pftpcDataConn->ftpcDataType;
        pDcpt->ftpcDataConnection.ftpcIsPassiveMode = pftpcDataConn->ftpcIsPassiveMode;
        pDcpt->ftpcDataConnection.ftpcTransferMode = pftpcDataConn->ftpcTransferMode;
        pDcpt->ftpcDataConnection.ftpcDataStructure = pftpcDataConn->ftpcDataStructure;
        pDcpt->ftpcCommand = CMD_GET;
        pDcpt->ftpcState = TCPIP_FTPC_STATE_START_GET;
        ftpcResult = TCPIP_FTPC_RES_OK;
        pDcpt->ftpcBusy = true;
    }
    else
    {
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_INIT_STATE; //niyas see whether this condition is right
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR;
        ftpcResult = TCPIP_FTPC_RES_MKD_INIT_STATE_ERROR;
    }
    
    return ftpcResult;
}

TCPIP_FTPC_RESULT_TYPE TCPIP_FTPC_NameList(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, const char *pathname)
{
    TCPIP_FTPC_RESULT_TYPE ftpcResult;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    
    if((pDcpt->ftpcFlag & TCPIP_FTPC_FLAG_LOGGEDIN) != 0 ) //niyas see whether this condition is right
    {
        pDcpt->ftpcServerPathname = pathname;
      
        //pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_NLST;
        ftpcResult = TCPIP_FTPC_RES_OK;
        pDcpt->ftpcBusy = true;
    }
    else
    {
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_INIT_STATE; //niyas see whether this condition is right
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR;
        ftpcResult = TCPIP_FTPC_RES_MKD_INIT_STATE_ERROR;
    }
    
    return ftpcResult;
}

TCPIP_FTPC_RESULT_TYPE TCPIP_FTPC_Read_DirName (TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, char *pathname, size_t length )
{
    TCPIP_FTPC_RESULT_TYPE ftpcResult;    
    TCPIP_FTPC_DCPT_TYPE* pDcpt = (TCPIP_FTPC_DCPT_TYPE*)ftpcHandle;
    
    if((pDcpt->ftpcState & TCPIP_FTPC_STATE_DONE_PWD) != 0 ) //niyas see whether this condition is right
    {
        pathname = (char*)pDcpt->ftpcServerPathname;
        length = pDcpt->ftpcNameLen;
        ftpcResult = TCPIP_FTPC_RES_OK;
    }
    return ftpcResult; 
}

static TCPIP_FTPC_STATE_TYPE ftpcStateHome(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
	// but continue
    return pDcpt->ftpcState;
}
//TCP_SOCKET_INFO sktInfo;
static TCPIP_FTPC_STATE_TYPE ftpcStateStartConnect(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
	TCPIP_FTPC_ERROR_TYPE res = TCPIP_FTPC_ERROR_NONE;

    
    if(pDcpt->ftpcSocket.ftpcCtrlSkt == NET_PRES_INVALID_SOCKET)
    {   // get a new socket
        NET_PRES_SKT_HANDLE_T ctrlSkt;
        NET_PRES_SKT_T sktType =  NET_PRES_SKT_DEFAULT_STREAM_CLIENT;//(pDcpt->smtpMailMessage.messageFlags &TCPIP_SMTPC_MAIL_FLAG_CONNECT_TLS) != 0 ? NET_PRES_SKT_ENCRYPTED_STREAM_CLIENT : NET_PRES_SKT_UNENCRYPTED_STREAM_CLIENT;
        ctrlSkt = NET_PRES_SocketOpen(0, sktType, (NET_PRES_SKT_ADDR_T)pDcpt->ftpcCtrlConnection.ftpcServerIpAddrType, pDcpt->ftpcCtrlConnection.serverCtrlPort, 0, 0);

        if(ctrlSkt == NET_PRES_INVALID_SOCKET)
        {   // failed to open a socket
            res = TCPIP_FTPC_ERROR_OPEN_CTRL_SOCKET;
        }
        else
        {   // success; get the socket TX buffer size 
            //TCP_SOCKET_INFO sktInfo;
            //NET_PRES_SocketInfoGet(ctrlSkt, &sktInfo);

            //if needed, modify the tx and rx buffer size of socket
            
            // alert of incoming traffic
            NET_PRES_SocketSignalHandlerRegister(ctrlSkt, TCPIP_TCP_SIGNAL_RX_DATA, _FTPCSocketRxSignalHandler, 0);

            pDcpt->ftpcSocket.ftpcCtrlSkt = ctrlSkt;
        }
    }
    
    if(res == TCPIP_FTPC_ERROR_NONE)
    {   // point socket to the SMTP server
        if(!NET_PRES_SocketRemoteBind(pDcpt->ftpcSocket.ftpcCtrlSkt, (NET_PRES_SKT_ADDR_T)pDcpt->ftpcCtrlConnection.ftpcServerIpAddrType,  pDcpt->ftpcCtrlConnection.serverCtrlPort, (NET_PRES_ADDRESS*)(pDcpt->ftpcCtrlConnection.ftpcServerAddr)))
        {   // failed
            res = TCPIP_FTPC_ERROR_BIND_CTRL_SOCKET; 
            NET_PRES_SocketClose(pDcpt->ftpcSocket.ftpcCtrlSkt);
        }
    }


    // advance
    // clear the disconnect condition
    //NET_PRES_SocketWasReset(pDcpt->skt);
    if(res == TCPIP_FTPC_ERROR_NONE)
    {
        memset(pDcpt->ftpcCtrlRxBuff, 0, ftpcGlobalConfig.ctrlCmdRxBuffSize);
        if (!NET_PRES_SocketConnect(pDcpt->ftpcSocket.ftpcCtrlSkt))
        {
            res = TCPIP_FTPC_ERROR_CONNECT_CTRL_SOCKET; 
            NET_PRES_SocketClose(pDcpt->ftpcSocket.ftpcCtrlSkt);
        }   
    }
    //NET_PRES_SocketInfoGet(pDcpt->ftpcSocket.ftpcCtrlSkt, &sktInfo);
    //NET_PRES_SocketConnect(pDcpt->skt);
    ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.serverCmdTmo);
    //return TCPIP_SMTPC_STAT_SOCKET_GET + 1;
    /*pDcpt->ftpcSocket.ftpcCtrlSkt = TCPIP_TCP_ClientOpen(pDcpt->ftpcCtrlConnection.ftpcServerIpAddrType,
                                                         pDcpt->ftpcServerCtrlPort,
                                                          (IP_MULTI_ADDRESS*) pDcpt->ftpcCtrlConnection.ftpcServerAddr);
    if (pDcpt->ftpcSocket.ftpcCtrlSkt == INVALID_SOCKET)
    {
        //SYS_CONSOLE_MESSAGE("Could not start connection\r\n");
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_INVALID_CTRL_SOCKET;
    }*/
    
    // Dynamically assign buffer for Control socket Command Buffer
	/*pDcpt->ftpcCtrlCmdBuff = (char *)TCPIP_HEAP_Malloc(pDcpt->memH, TCPIP_FTPC_CTRL_CMD_BUFFER_SIZE);
    
    // Dynamically assign buffer for Control socket Reply Buffer
	pDcpt->ftpcCtrlReplyBuff = (char *)TCPIP_HEAP_Malloc(pDcpt->memH, TCPIP_FTPC_CTRL_REPLY_BUFFER_SIZE);    
    
    if((!pDcpt->ftpcCtrlCmdBuff) || (!pDcpt->ftpcCtrlReplyBuff))
    {
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_MEM_ALLOC;
    }*/
    
    SYS_CONSOLE_MESSAGE("Starting connection\r\n");
    pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_CONNECT;
    //todo niyas : start timeout
    return pDcpt->ftpcState;
}

static TCPIP_FTPC_STATE_TYPE ftpcStateWaitConnect(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //uint16_t lenA;

    // Check how much data is waiting
    //lenA = NET_PRES_SocketReadIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt);
   // SYS_CONSOLE_MESSAGE(" wait connection\r\n");
    if (!NET_PRES_SocketIsConnected(pDcpt->ftpcSocket.ftpcCtrlSkt))
    { //SYS_CONSOLE_MESSAGE(" wait connection - is connected\r\n");
        if(ftpcIsDcptTmoExpired(pDcpt))
        {//SYS_CONSOLE_MESSAGE(" wait connection - tmo\r\n");
            //niyas timeout == yes?
            NET_PRES_SocketClose(pDcpt->ftpcSocket.ftpcCtrlSkt);
            // pDcpt->ftpcSocket.ftpcCtrlSkt = INVALID_SOCKET;
            pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
            pDcpt->ftpcError = TCPIP_FTPC_ERROR_TIMEOUT_CTRL_SOCKET; 
        }
        else
        {
            //niyas timeout == No?
            //do nothing//pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_CONNECT;
        }
         
    }
    else
    {
        SYS_CONSOLE_MESSAGE(" wait connection- get connect greeting\r\n");
        pDcpt->ftpcState = TCPIP_FTPC_STATE_GET_CONNECT_GREETING;
        //todo niyas : start timeout
    }
    

    return pDcpt->ftpcState; 
    
}
//char tempRxbuff[50];
static TCPIP_FTPC_STATE_TYPE ftpcStateGetConnectGreeting(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    uint16_t readLen =  NET_PRES_SocketReadIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt);
    
    if (readLen == 0)
    {        
        if(ftpcIsDcptTmoExpired(pDcpt))
        {
            //niyas timeout == yes?
            NET_PRES_SocketClose(pDcpt->ftpcSocket.ftpcCtrlSkt);
            // pDcpt->ftpcSocket.ftpcCtrlSkt = INVALID_SOCKET;
            pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
            pDcpt->ftpcError = TCPIP_FTPC_ERROR_TIMEOUT_GET_GREETING; 
        }
        else
        {
            //niyas timeout == No?
            //do nothing//pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_CONNECT;
        }
    }
    else
    {
        //pDcpt->ftpcCtrlRxBuff = tempRxbuff;
        NET_PRES_SocketRead(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlRxBuff, readLen);
        NET_PRES_SocketDiscard(pDcpt->ftpcSocket.ftpcCtrlSkt);
        pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_CONNECT;
    }
        
    return pDcpt->ftpcState;    
        
#if 0    
        //niyas pDcpt->ftpcCtrlReplyLen =  TCPIP_TCP_ArrayGet(pDcpt->ftpcSocket.ftpcCtrlSkt, (uint8_t*)pDcpt->ftpcCtrlReplyBuff, TCPIP_FTPC_CTRL_REPLY_BUFFER_SIZE);
        if(pDcpt->ftpcCtrlReplyLen)
        {
            //Process the reply on ctrl socket
            if(1/* niyas success*/)
            {
                pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_CONNECT;
            }
            else
            {
                //error                
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_GREETING_REPLY;
            }
        }
        else
        {
            //error
            pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
            pDcpt->ftpcError = TCPIP_FTPC_ERROR_GREETING_REPLY_LEN; 
        }
        //SYS_CONSOLE_PRINT("%s", buffer);
    }
    else
    {
        //niyas if timeout == Yes?
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_TIMEOUT_GET_GREETING; 
        //niyas if timeout == No?
            //do nothing
    }
    return pDcpt->ftpcState;
#endif
}

static TCPIP_FTPC_STATE_TYPE ftpcStateDoneConnect(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //niyas verify the need for this function
    //trigger callback function for successful connection
    pDcpt->ftpcFlag  = TCPIP_FTPC_FLAG_CONNECTED;
    pDcpt->ftpcBusy = false;
    SYS_CONSOLE_MESSAGE("FTPC Connected\r\n");
    return pDcpt->ftpcState; 
}

//char tempTxbuff[100];
static TCPIP_FTPC_STATE_TYPE ftpcStateSendUser(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    uint16_t nBytes = 0;
    //Format the USER command
    //sprintf(pDcpt->, "USER %s\r\n", username);
    //pDcpt->ftpcCtrlTxBuff = tempTxbuff;
    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "USER %s\r\n", pDcpt->ftpcAccessControl.ftpcUserName);
    
    if(NET_PRES_SocketWriteIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt, nBytes, 0) >= nBytes)
    {
        memset(pDcpt->ftpcCtrlRxBuff, 0, ftpcGlobalConfig.ctrlCmdRxBuffSize);
        NET_PRES_SocketWrite(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlTxBuff, nBytes);
        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_LOGIN_WAIT_USER;
        //NET_PRES_SocketFlush(pDcpt->skt);
        
        // advance
        //return pDcpt->currStat + 1;
    }   
    return pDcpt->ftpcState;
}


static TCPIP_FTPC_STATE_TYPE ftpcStateWaitUser(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //230
    //530
    //500, 501, 421
    //331, 332
    int     serverReplyCode;
    uint16_t readLen = 0;
    
    if((readLen = NET_PRES_SocketReadIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt))  >= 4)  
    {
        //pDcpt->ftpcCtrlRxBuff = tempRxbuff;
        
        if(NET_PRES_SocketRead(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlRxBuff, readLen) >= 3)
        {
            NET_PRES_SocketDiscard(pDcpt->ftpcSocket.ftpcCtrlSkt);
            //Process the reply on ctrl socket
            serverReplyCode = ftpcGetReplyCode(pDcpt->ftpcCtrlRxBuff);
            
            if(FTPC_IS_REPLY_3YZ(serverReplyCode))
            {
                //send the password
                pDcpt->ftpcState = TCPIP_FTPC_STATE_LOGIN_SEND_PASS;
            }
            else if(FTPC_IS_REPLY_2YZ(serverReplyCode))
            {
                //no need of password; login is success
                pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_LOGIN;
            }
            else if(FTPC_IS_REPLY_5YZ(serverReplyCode))
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_USER_SYNTAX; 
            }
            else if(FTPC_IS_REPLY_4YZ(serverReplyCode))
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_USER_CMD_NOT_IMPL; 
            }
            else
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_USER_UNDEF;
            }
        }
        else
        {
            //error
            pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
            pDcpt->ftpcError = TCPIP_FTPC_ERROR_USER_REPLY_LEN; 
        }
        //SYS_CONSOLE_PRINT("%s", buffer);
    }
    else
    {
        //niyas if timeout == Yes?
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_TIMEOUT_USER; 
        //niyas if timeout == No?
            //do nothing
    }
        
    return pDcpt->ftpcState;
}

static TCPIP_FTPC_STATE_TYPE ftpcStateSendPass(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    uint16_t nBytes = 0;
    
    //pDcpt->ftpcCtrlTxBuff = tempTxbuff;
    
    
    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "PASS %s\r\n", pDcpt->ftpcAccessControl.ftpcPassword);
    
    if(NET_PRES_SocketWriteIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt, nBytes, 0) >= nBytes)
    {
        memset(pDcpt->ftpcCtrlRxBuff, 0, ftpcGlobalConfig.ctrlCmdRxBuffSize);
        NET_PRES_SocketWrite(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlTxBuff, nBytes);
        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_LOGIN_WAIT_PASS;
    }
   /* if(TCPIP_TCP_PutIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt) != 0)
    {
        //Format the PASS command
        sprintf(pDcpt->ftpcCtrlCmdBuff, "PASS %s\r\n", pDcpt->ftpcAccessControl.ftpcPassword);
        
        TCPIP_TCP_ArrayPut(pDcpt->ftpcSocket.ftpcCtrlSkt, (uint8_t*)pDcpt->ftpcCtrlCmdBuff, strlen(pDcpt->ftpcCtrlCmdBuff));
        pDcpt->ftpcState = TCPIP_FTPC_STATE_LOGIN_WAIT_PASS;
    }
    */  
    return pDcpt->ftpcState;
}
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitPass(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //230
    //202
    //530
    //500, 501, 503, 421
    
    int     serverReplyCode;
    uint16_t readLen = 0;
    
    if((readLen = NET_PRES_SocketReadIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt))  >= 4)  
    {
        //pDcpt->ftpcCtrlRxBuff = tempRxbuff;
        
        if(NET_PRES_SocketRead(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlRxBuff, readLen) >= 3)
        {
            NET_PRES_SocketDiscard(pDcpt->ftpcSocket.ftpcCtrlSkt);
            //Process the reply on ctrl socket
            serverReplyCode = ftpcGetReplyCode(pDcpt->ftpcCtrlRxBuff);

            
            if(FTPC_IS_REPLY_3YZ(serverReplyCode))
            {
                //send the Account 
                pDcpt->ftpcState = TCPIP_FTPC_STATE_LOGIN_SEND_ACCT;
            }
            else if(FTPC_IS_REPLY_2YZ(serverReplyCode))
            {
                //login success;no need of ACCT command
                pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_LOGIN;
            }
            else if(FTPC_IS_REPLY_5YZ(serverReplyCode))
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_PASS_SYNTAX; 
            }
            else if(FTPC_IS_REPLY_4YZ(serverReplyCode))
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_PASS_CMD_NOT_IMPL; 
            }
            else
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_PASS_UNDEF;
            }
        }
        else
        {
            //error
            pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
            pDcpt->ftpcError = TCPIP_FTPC_ERROR_PASS_REPLY_LEN; 
        }
        
    }
    else
    {
        //niyas if timeout == Yes?
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_TIMEOUT_PASS; 
        //niyas if timeout == No?
            //do nothing
    }
    return pDcpt->ftpcState;
}
static TCPIP_FTPC_STATE_TYPE ftpcStateSendAcct(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
  /*  if(TCPIP_TCP_PutIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt) != 0)
    {
        //Format the ACCT command
        sprintf(pDcpt->ftpcCtrlCmdBuff, "ACCT %s\r\n", pDcpt->ftpcAccessControl.ftpcAccount);
        
        TCPIP_TCP_ArrayPut(pDcpt->ftpcSocket.ftpcCtrlSkt, (uint8_t*)pDcpt->ftpcCtrlCmdBuff, strlen(pDcpt->ftpcCtrlCmdBuff));
        pDcpt->ftpcState = TCPIP_FTPC_STATE_LOGIN_WAIT_ACCT;
    }
    */
    return pDcpt->ftpcState;
}
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitAcct(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //230
    //202
    //530
    //500, 501, 503, 421
  /*  int     serverReplyCode;
    if (TCPIP_TCP_GetIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt))
    {
       // pDcpt->ftpcCtrlReplyLen =  TCPIP_TCP_ArrayGet(pDcpt->ftpcSocket.ftpcCtrlSkt, (uint8_t*)pDcpt->ftpcCtrlReplyBuff, TCPIP_FTPC_CTRL_REPLY_BUFFER_SIZE);
        if(pDcpt->ftpcCtrlReplyLen >= 4)
        {
            //Process the reply on ctrl socket
            serverReplyCode = ftpcGetReplyCode(pDcpt);
                        
            if(FTPC_IS_REPLY_2YZ(serverReplyCode))
            {
                //login success;no need of ACCT command
                pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_LOGIN;
            }
            else if(FTPC_IS_REPLY_5YZ(serverReplyCode))
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_ACCT_SYNTAX; 
            }
            else if(FTPC_IS_REPLY_4YZ(serverReplyCode))
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_ACCT_CMD_NOT_IMPL; 
            }
            else
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_ACCT_UNDEF;
            }
            
        }
        else
        {
            //error
            pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
            pDcpt->ftpcError = TCPIP_FTPC_ERROR_PASS_REPLY_LEN; 
        }
        
    }
    else
    {
        //niyas if timeout == Yes?
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_TIMEOUT_ACCT; 
        //niyas if timeout == No?
            //do nothing
    }*/
    return pDcpt->ftpcState;
}
static TCPIP_FTPC_STATE_TYPE ftpcStateDoneLogin(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    
    //niyas verify the need for this function
    //trigger callback function for successful connection
    pDcpt->ftpcFlag  |= TCPIP_FTPC_FLAG_LOGGEDIN;
    pDcpt->ftpcBusy = false;
    return pDcpt->ftpcState;
}

static TCPIP_FTPC_STATE_TYPE ftpcStateSendCwd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    uint16_t nBytes = 0;
    
    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "CWD %s\r\n", pDcpt->ftpcServerPathname);
    
    if(NET_PRES_SocketWriteIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt, nBytes, 0) >= nBytes)
    {
        memset(pDcpt->ftpcCtrlRxBuff, 0, ftpcGlobalConfig.ctrlCmdRxBuffSize);
        NET_PRES_SocketWrite(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlTxBuff, nBytes);
        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_MKD;
    } 
    
 /*   if(TCPIP_TCP_PutIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt) != 0)
    {
        sprintf(pDcpt->ftpcCtrlCmdBuff, "CWD %s\r\n", pDcpt->ftpcServerPathname);
        
        TCPIP_TCP_ArrayPut(pDcpt->ftpcSocket.ftpcCtrlSkt, (uint8_t*)pDcpt->ftpcCtrlCmdBuff, strlen(pDcpt->ftpcCtrlCmdBuff));
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_CWD;
        //todo niyas : start timeout
    }
    */
    return pDcpt->ftpcState;
}
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitCwd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{   
    //Response
    //250
    //500, 501, 502, 421, 530, 550
    int     serverReplyCode;
    uint16_t readLen = 0;
    
    if( (readLen = NET_PRES_SocketReadIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt))  >= 4)  
    {
        //pDcpt->ftpcCtrlRxBuff = tempRxbuff;
        
        if(NET_PRES_SocketRead(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlRxBuff, readLen) >= 3)
        {
            NET_PRES_SocketDiscard(pDcpt->ftpcSocket.ftpcCtrlSkt);
            //Process the reply on ctrl socket
            serverReplyCode = ftpcGetReplyCode(pDcpt->ftpcCtrlRxBuff);
                        
            if(FTPC_IS_REPLY_2YZ(serverReplyCode))
            {
                //CDUP success
                pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_MKD;
            }
            else if(FTPC_IS_REPLY_5YZ(serverReplyCode))
            {
                //error 
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition                 
                if(serverReplyCode == 550)
                {                    
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_REQ_ACTION_NOT_TAKEN;  // path not found
                }
                else if(serverReplyCode == 530)
                {
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_NOT_LOGGEDIN;  // path not found
                }
                else if(serverReplyCode == 502)
                {
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_PWD_CMD_NOT_IMPL;  // path not found
                }
                else
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_PWD_SYNTAX;
            }
            else if(FTPC_IS_REPLY_4YZ(serverReplyCode))
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_SRVC_NOT_AVLBL; 
            }
            else
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_CWD_UNDEF;
            }
            
        }
        else
        {
            //error
            pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
            pDcpt->ftpcError = TCPIP_FTPC_ERROR_CWD_REPLY_LEN; 
        }
        
    }
    else
    {
        //niyas if timeout == Yes?
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_TIMEOUT_CWD; 
        //niyas if timeout == No?
            //do nothing
    }
    return pDcpt->ftpcState;
}
static TCPIP_FTPC_STATE_TYPE ftpcStateDoneCwd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //niyas verify the need for this function
    //trigger callback function for successful connection
    pDcpt->ftpcBusy = false;
    return pDcpt->ftpcState;
}

static TCPIP_FTPC_STATE_TYPE ftpcStateSendCdup(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    uint16_t nBytes = 0;
    
    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "CDUP\r\n");
    
    if(NET_PRES_SocketWriteIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt, nBytes, 0) >= nBytes)
    {
        memset(pDcpt->ftpcCtrlRxBuff, 0, ftpcGlobalConfig.ctrlCmdRxBuffSize);
        NET_PRES_SocketWrite(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlTxBuff, nBytes);
        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_CDUP;
    }
    /* if(TCPIP_TCP_PutIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt) != 0)
    {
        sprintf(pDcpt->ftpcCtrlCmdBuff, "CDUP\r\n");
        
        TCPIP_TCP_ArrayPut(pDcpt->ftpcSocket.ftpcCtrlSkt, (uint8_t*)pDcpt->ftpcCtrlCmdBuff, strlen(pDcpt->ftpcCtrlCmdBuff));
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_CDUP;
        //todo niyas : start timeout
    }
    */
    return pDcpt->ftpcState;    
}

static TCPIP_FTPC_STATE_TYPE ftpcStateWaitCdup(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //200
    //500, 501, 502, 421, 530, 550
    int     serverReplyCode;
    uint16_t readLen = 0;
    
    if( (readLen = NET_PRES_SocketReadIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt))  >= 4)  
    {
        //pDcpt->ftpcCtrlRxBuff = tempRxbuff;
        
        if(NET_PRES_SocketRead(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlRxBuff, readLen) >= 3)
        {
            NET_PRES_SocketDiscard(pDcpt->ftpcSocket.ftpcCtrlSkt);
            //Process the reply on ctrl socket
            serverReplyCode = ftpcGetReplyCode(pDcpt->ftpcCtrlRxBuff);
                        
            if(FTPC_IS_REPLY_2YZ(serverReplyCode))
            {
                //CDUP success
                pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_CDUP;
            }
            else if(FTPC_IS_REPLY_5YZ(serverReplyCode))
            {
                //error 
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition                 
                if(serverReplyCode == 550)
                {                    
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_REQ_ACTION_NOT_TAKEN;  // path not found
                }
                else if(serverReplyCode == 530)
                {
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_NOT_LOGGEDIN;  // path not found
                }
                else if(serverReplyCode == 502)
                {
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_PWD_CMD_NOT_IMPL;  // path not found
                }
                else
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_PWD_SYNTAX;
            }
            else if(FTPC_IS_REPLY_4YZ(serverReplyCode))
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_SRVC_NOT_AVLBL; 
            }
            else
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_CDUP_UNDEF;
            }
            
        }
        else
        {
            //error
            pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
            pDcpt->ftpcError = TCPIP_FTPC_ERROR_CDUP_REPLY_LEN; 
        }
        
    }
    else
    {
        //niyas if timeout == Yes?
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_TIMEOUT_CDUP; 
        //niyas if timeout == No?
            //do nothing
    }
    return pDcpt->ftpcState;   
}
static TCPIP_FTPC_STATE_TYPE ftpcStateDoneCdup(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //niyas verify the need for this function
    //trigger callback function for successful connection
    pDcpt->ftpcBusy = false;
    return pDcpt->ftpcState;
}

static TCPIP_FTPC_STATE_TYPE ftpcStateSendMkd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    uint16_t nBytes = 0;
    
    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "MKD %s\r\n", pDcpt->ftpcServerPathname);
    
    if(NET_PRES_SocketWriteIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt, nBytes, 0) >= nBytes)
    {
        memset(pDcpt->ftpcCtrlRxBuff, 0, ftpcGlobalConfig.ctrlCmdRxBuffSize);
        NET_PRES_SocketWrite(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlTxBuff, nBytes);
        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_MKD;
    } 
  /*  if(TCPIP_TCP_PutIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt) != 0)
    {
        sprintf(pDcpt->ftpcCtrlCmdBuff, "MKD %s\r\n", pDcpt->ftpcServerPathname);
        
        TCPIP_TCP_ArrayPut(pDcpt->ftpcSocket.ftpcCtrlSkt, (uint8_t*)pDcpt->ftpcCtrlCmdBuff, strlen(pDcpt->ftpcCtrlCmdBuff));
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_MKD;
        //todo niyas : start timeout
    }
    */
    return pDcpt->ftpcState;    
}

static TCPIP_FTPC_STATE_TYPE ftpcStateWaitMkd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //257
    //500, 501, 502, 421, 530, 550
    int     serverReplyCode;
    uint16_t readLen = 0;
    
    if( (readLen = NET_PRES_SocketReadIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt))  >= 4)  
    {
        //pDcpt->ftpcCtrlRxBuff = tempRxbuff;
        
        if(NET_PRES_SocketRead(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlRxBuff, readLen) >= 3)
        {
            NET_PRES_SocketDiscard(pDcpt->ftpcSocket.ftpcCtrlSkt);
            //Process the reply on ctrl socket
            serverReplyCode = ftpcGetReplyCode(pDcpt->ftpcCtrlRxBuff);
                        
            if(FTPC_IS_REPLY_2YZ(serverReplyCode))
            {
                //CDUP success
                pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_MKD;
            }
            else if(FTPC_IS_REPLY_5YZ(serverReplyCode))
            {
                //error 
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition                 
                if(serverReplyCode == 550)
                {                    
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_REQ_ACTION_NOT_TAKEN;  // path not found
                }
                else if(serverReplyCode == 530)
                {
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_NOT_LOGGEDIN;  // path not found
                }
                else if(serverReplyCode == 502)
                {
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_PWD_CMD_NOT_IMPL;  // path not found
                }
                else
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_PWD_SYNTAX;
            }
            else if(FTPC_IS_REPLY_4YZ(serverReplyCode))
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_SRVC_NOT_AVLBL; 
            }
            else
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_MKD_UNDEF;
            }
            
        }
        else
        {
            //error
            pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
            pDcpt->ftpcError = TCPIP_FTPC_ERROR_MKD_REPLY_LEN; 
        }
        
    }
    else
    {
        //niyas if timeout == Yes?
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_TIMEOUT_MKD; 
        //niyas if timeout == No?
            //do nothing
    }
    return pDcpt->ftpcState;     
}

static TCPIP_FTPC_STATE_TYPE ftpcStateDoneMkd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //niyas verify the need for this function
    //trigger callback function for successful connection
    pDcpt->ftpcBusy = false;
    return pDcpt->ftpcState;
}

static TCPIP_FTPC_STATE_TYPE ftpcStateSendRmd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    uint16_t nBytes = 0;
    
    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "RMD %s\r\n", pDcpt->ftpcServerPathname);
    
    if(NET_PRES_SocketWriteIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt, nBytes, 0) >= nBytes)
    {
        memset(pDcpt->ftpcCtrlRxBuff, 0, ftpcGlobalConfig.ctrlCmdRxBuffSize);
        NET_PRES_SocketWrite(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlTxBuff, nBytes);
        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_RMD;
    } 
    /*if(TCPIP_TCP_PutIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt) != 0)
    {
        sprintf(pDcpt->ftpcCtrlCmdBuff, "RMD %s\r\n", pDcpt->ftpcServerPathname);
        
        TCPIP_TCP_ArrayPut(pDcpt->ftpcSocket.ftpcCtrlSkt, (uint8_t*)pDcpt->ftpcCtrlCmdBuff, strlen(pDcpt->ftpcCtrlCmdBuff));
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_RMD;
        //todo niyas : start timeout
    }*/
    
    return pDcpt->ftpcState;    
}
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitRmd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //250
    //500, 501, 502, 421, 530, 550
    int     serverReplyCode;
    uint16_t readLen = 0;
    
    if( (readLen = NET_PRES_SocketReadIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt))  >= 4)  
    {
        //pDcpt->ftpcCtrlRxBuff = tempRxbuff;
        
        if(NET_PRES_SocketRead(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlRxBuff, readLen) >= 3)
        {
            NET_PRES_SocketDiscard(pDcpt->ftpcSocket.ftpcCtrlSkt);
            //Process the reply on ctrl socket
            serverReplyCode = ftpcGetReplyCode(pDcpt->ftpcCtrlRxBuff);
                        
            if(FTPC_IS_REPLY_2YZ(serverReplyCode))
            {
                //CDUP success
                pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_RMD;
            }
            else if(FTPC_IS_REPLY_5YZ(serverReplyCode))
            {
                //error 
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition                 
                if(serverReplyCode == 550)
                {                    
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_REQ_ACTION_NOT_TAKEN;  // path not found
                }
                else if(serverReplyCode == 530)
                {
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_NOT_LOGGEDIN;  // path not found
                }
                else if(serverReplyCode == 502)
                {
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_PWD_CMD_NOT_IMPL;  // path not found
                }
                else
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_PWD_SYNTAX;
            }
            else if(FTPC_IS_REPLY_4YZ(serverReplyCode))
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_SRVC_NOT_AVLBL; 
            }
            else
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_RMD_UNDEF;
            }
            
        }
        else
        {
            //error
            pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
            pDcpt->ftpcError = TCPIP_FTPC_ERROR_RMD_REPLY_LEN; 
        }
        
    }
    else
    {
        //niyas if timeout == Yes?
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_TIMEOUT_RMD; 
        //niyas if timeout == No?
            //do nothing
    }
    return pDcpt->ftpcState;         
}
static TCPIP_FTPC_STATE_TYPE ftpcStateDoneRmd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //niyas verify the need for this function
    //trigger callback function for successful connection
    pDcpt->ftpcBusy = false;
    return pDcpt->ftpcState;
}

static TCPIP_FTPC_STATE_TYPE ftpcStateSendPwd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    uint16_t nBytes = 0;    
    
    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "PWD\r\n");
    
    if(NET_PRES_SocketWriteIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt, nBytes, 0) >= nBytes)
    {
        memset(pDcpt->ftpcCtrlRxBuff, 0, ftpcGlobalConfig.ctrlCmdRxBuffSize);
        NET_PRES_SocketWrite(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlTxBuff, nBytes);
        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_PWD;
    }
   
    return pDcpt->ftpcState;
}
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitPwd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{    //Response
    //257
    //500, 501, 502, 421, 550
    int     serverReplyCode;
    uint16_t readLen = 0;
    
    if( (readLen = NET_PRES_SocketReadIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt))  >= 4)  
    {
        //pDcpt->ftpcCtrlRxBuff = tempRxbuff;
        
        if(NET_PRES_SocketRead(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlRxBuff, readLen) >= 3)
        {
            NET_PRES_SocketDiscard(pDcpt->ftpcSocket.ftpcCtrlSkt);
            //Process the reply on ctrl socket
            serverReplyCode = ftpcGetReplyCode(pDcpt->ftpcCtrlRxBuff);
                        
            if(FTPC_IS_REPLY_2YZ(serverReplyCode))
            {
                //PWD success
                pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_PWD;
            }
            else if(FTPC_IS_REPLY_5YZ(serverReplyCode))
            {
                //error 
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition                 
                if(serverReplyCode == 550)
                {                    
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_REQ_ACTION_NOT_TAKEN;  // path not found
                }
                else if(serverReplyCode == 502)
                {
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_PWD_CMD_NOT_IMPL;  // path not found
                }
                else
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_PWD_SYNTAX;
            }
            else if(FTPC_IS_REPLY_4YZ(serverReplyCode))
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_SRVC_NOT_AVLBL; 
            }
            else
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_PWD_UNDEF;
            }
            
        }
        else
        {
            //error
            pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
            pDcpt->ftpcError = TCPIP_FTPC_ERROR_PWD_REPLY_LEN; 
        }
        
    }
    else
    {
        //niyas if timeout == Yes?
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_TIMEOUT_PWD; 
        //niyas if timeout == No?
            //do nothing
    }
    return pDcpt->ftpcState;         
}
static TCPIP_FTPC_STATE_TYPE ftpcStateDonePwd(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //niyas verify the need for this function
    //trigger callback function for successful connection
   // (pDcpt->ftpcCtrlRxBuff);
    pDcpt->ftpcBusy = false;
    return pDcpt->ftpcState;
}

static TCPIP_FTPC_STATE_TYPE ftpcStateSendType(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    uint16_t nBytes = 0;    
    char type = 0;
    
    if(pDcpt->ftpcDataConnection.ftpcDataType == ASCII)
        type = 'A';
    else if(pDcpt->ftpcDataConnection.ftpcDataType == EBCDIC)
        type = 'E';  
    else if(pDcpt->ftpcDataConnection.ftpcDataType == IMAGE)
        type = 'I';  
    
    if (type)
    {
        nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "TYPE %s\r\n", &type);

        if(NET_PRES_SocketWriteIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt, nBytes, 0) >= nBytes)
        {
            memset(pDcpt->ftpcCtrlRxBuff, 0, ftpcGlobalConfig.ctrlCmdRxBuffSize);
            NET_PRES_SocketWrite(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlTxBuff, nBytes);

            pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_TYPE;
            SYS_CONSOLE_MESSAGE(" TYPE - Sent\r\n");
        }
    }
    else
    {
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR;
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_TYPE_UNSUPPORTED;
    }
   
    return pDcpt->ftpcState;
}
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitType(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{    //Response
    //257
    //500, 501, 504, 421, 530
    int     serverReplyCode;
    uint16_t readLen = 0;
    
    if( (readLen = NET_PRES_SocketReadIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt))  >= 4)  
    {
        //pDcpt->ftpcCtrlRxBuff = tempRxbuff;
        SYS_CONSOLE_MESSAGE(" TYPE - Read ready\r\n");
        if(NET_PRES_SocketRead(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlRxBuff, readLen) >= 3)
        {
            NET_PRES_SocketDiscard(pDcpt->ftpcSocket.ftpcCtrlSkt);
            //Process the reply on ctrl socket
            serverReplyCode = ftpcGetReplyCode(pDcpt->ftpcCtrlRxBuff);
                        
            if(FTPC_IS_REPLY_2YZ(serverReplyCode))
            {
                //PWD success
                pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_TYPE;
            }
            else if(FTPC_IS_REPLY_5YZ(serverReplyCode))
            {
                //error 
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition                 
                if(serverReplyCode == 530)
                {                    
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_TYPE_NOT_LOGGEDIN;  // path not found
                }
                else if(serverReplyCode == 504)
                {
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_TYPE_CMD_NOT_IMPL;  // path not found
                }
                else
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_TYPE_SYNTAX;
            }
            else if(FTPC_IS_REPLY_4YZ(serverReplyCode))
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_TYPE_SRVC_NOT_AVLBL; 
            }
            else
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_TYPE_UNDEF;
            }
            
        }
        else
        {
            //error
            pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
            pDcpt->ftpcError = TCPIP_FTPC_ERROR_TYPE_REPLY_LEN; 
        }
        
    }
    else
    {
        //niyas if timeout == Yes?
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_TYPE_TIMEOUT; 
        //niyas if timeout == No?
            //do nothing
    }
    return pDcpt->ftpcState;         
}
static TCPIP_FTPC_STATE_TYPE ftpcStateDoneType(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //niyas verify the need for this function
    //trigger callback function for successful connection
   // (pDcpt->ftpcCtrlRxBuff);
    SYS_CONSOLE_MESSAGE(" TYPE - Done\r\n");
    if((pDcpt->ftpcCommand == CMD_GET)|| (pDcpt->ftpcCommand == CMD_PUT))
    {
        
        if(pDcpt->ftpcDataConnection.ftpcIsPassiveMode)
            pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_PASV;
        else
            pDcpt->ftpcState = TCPIP_FTPC_STATE_OPEN_DATA_SKT;
    }
    else
    {
        pDcpt->ftpcBusy = false;
    }
    
    return pDcpt->ftpcState;
}


static TCPIP_FTPC_STATE_TYPE ftpcStateStartGet(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
	/*TCPIP_FTPC_ERROR_TYPE res = TCPIP_FTPC_ERROR_NONE;
    
    if(pDcpt->ftpcSocket.ftpcDataSkt == NET_PRES_INVALID_SOCKET)
    {   // get a new socket
        NET_PRES_SKT_HANDLE_T dataSkt;
        NET_PRES_SKT_T sktType =  NET_PRES_SKT_DEFAULT_STREAM_CLIENT;
        dataSkt = NET_PRES_SocketOpen(0, sktType, (NET_PRES_SKT_ADDR_T)pDcpt->ftpcCtrlConnection.ftpcServerIpAddrType, 0, 0, 0);

        if(dataSkt == NET_PRES_INVALID_SOCKET)
        {   // failed to open a socket
            res = TCPIP_FTPC_ERROR_OPEN_DATA_SOCKET;
        }
        else
        {   // success; get the socket TX buffer size 
            //TCP_SOCKET_INFO sktInfo;
            //NET_PRES_SocketInfoGet(dataSkt, &sktInfo);

            //if needed, modify the tx and rx buffer size of socket
            
            // alert of incoming traffic
            NET_PRES_SocketSignalHandlerRegister(dataSkt, TCPIP_TCP_SIGNAL_RX_DATA, _FTPCSocketRxSignalHandler, 0);

            pDcpt->ftpcSocket.ftpcDataSkt = dataSkt;
            //if(pDcpt->ftpcDataConnection.ftpcDataType == ASCII)
            pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_TYPE;
        }
    }*/
    pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_TYPE;
    return pDcpt->ftpcState;
}


static TCPIP_FTPC_STATE_TYPE ftpcStateDoneGet(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{	
    //close data socket
    NET_PRES_SocketClose(pDcpt->ftpcSocket.ftpcDataSkt);
    pDcpt->ftpcSocket.ftpcDataSkt = NET_PRES_INVALID_SOCKET; 
    pDcpt->ftpcDataRxBuff = 0;
    //discard any data in control socket
    NET_PRES_SocketDiscard(pDcpt->ftpcSocket.ftpcCtrlSkt);
    pDcpt->ftpcCommand = CMD_NONE;    
    pDcpt->ftpcBusy = false;
    return pDcpt->ftpcState;
}

static TCPIP_FTPC_STATE_TYPE ftpcStateOpenDataSkt(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    TCPIP_FTPC_ERROR_TYPE res = TCPIP_FTPC_ERROR_NONE;
    
    if(pDcpt->ftpcSocket.ftpcDataSkt == NET_PRES_INVALID_SOCKET)
    {   // get a new socket
        NET_PRES_SKT_HANDLE_T dataSkt;
        NET_PRES_SKT_T sktType =  NET_PRES_SKT_DEFAULT_STREAM_SERVER;
        dataSkt = NET_PRES_SocketOpen(0, sktType, (NET_PRES_SKT_ADDR_T)pDcpt->ftpcCtrlConnection.ftpcServerIpAddrType, 0, 0, 0);

        if(dataSkt == NET_PRES_INVALID_SOCKET)
        {   // failed to open a socket
            res = TCPIP_FTPC_ERROR_OPEN_DATA_SOCKET;
            pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
        }
        else
        {   // success; get the socket TX buffer size 
           // TCP_SOCKET_INFO dataSktInfo;
           // NET_PRES_SocketInfoGet(dataSkt, &dataSktInfo);

            //if needed, modify the tx and rx buffer size of socket
            SYS_CONSOLE_MESSAGE(" Open Data Socket\r\n");
            // alert of incoming traffic
            NET_PRES_SocketSignalHandlerRegister(dataSkt, TCPIP_TCP_SIGNAL_RX_DATA, _FTPCSocketRxSignalHandler, 0);

            pDcpt->ftpcSocket.ftpcDataSkt = dataSkt;
            //if(pDcpt->ftpcDataConnection.ftpcDataType == ASCII)
            //pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_TYPE;
            if((pDcpt->ftpcCommand == CMD_GET)|| (pDcpt->ftpcCommand == CMD_PUT))
            {
                if(pDcpt->ftpcDataConnection.ftpcIsPassiveMode)
                    pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_PASV;
                else
                    pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_PORT;
            }            
                
        }
    }
    pDcpt->ftpcError = res;
    return pDcpt->ftpcState;
}
static TCPIP_FTPC_STATE_TYPE ftpcStateSendPort(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    uint16_t nBytes = 0;
    //char *address;
    TCP_SOCKET_INFO ctrlSktInfo, dataSktInfo;
    TCPIP_FTPC_ERROR_TYPE res = TCPIP_FTPC_ERROR_NONE;
    
    NET_PRES_SocketInfoGet(pDcpt->ftpcSocket.ftpcCtrlSkt, &ctrlSktInfo);
    NET_PRES_SocketInfoGet(pDcpt->ftpcSocket.ftpcDataSkt, &dataSktInfo);
    /*if(pDcpt->ftpcSocket.ftpcDataSkt == NET_PRES_INVALID_SOCKET)
    {   // get a new socket
        NET_PRES_SKT_HANDLE_T dataSkt;
        NET_PRES_SKT_T sktType =  NET_PRES_SKT_DEFAULT_STREAM_SERVER;
        dataSkt = NET_PRES_SocketOpen(0, sktType, (NET_PRES_SKT_ADDR_T)pDcpt->ftpcCtrlConnection.ftpcServerIpAddrType, 0, 0, 0);

        if(dataSkt == NET_PRES_INVALID_SOCKET)
        {   // failed to open a socket
            res = TCPIP_FTPC_ERROR_OPEN_DATA_SOCKET;
        }
        else
        {   // success; get the socket TX buffer size 
           // TCP_SOCKET_INFO dataSktInfo;
            NET_PRES_SocketInfoGet(dataSkt, &dataSktInfo);

            //if needed, modify the tx and rx buffer size of socket
            
            // alert of incoming traffic
            NET_PRES_SocketSignalHandlerRegister(dataSkt, TCPIP_TCP_SIGNAL_RX_DATA, _FTPCSocketRxSignalHandler, 0);

            pDcpt->ftpcSocket.ftpcDataSkt = dataSkt;
            //if(pDcpt->ftpcDataConnection.ftpcDataType == ASCII)
            pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_TYPE;
        }
    }*/
     
    //if(res == TCPIP_FTPC_ERROR_NONE)
    {
        nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "PORT %d,%d,%d,%d,%d,%d\r\n", ctrlSktInfo.localIPaddress.v4Add.v[0],
                                                                        ctrlSktInfo.localIPaddress.v4Add.v[1],
                                                                        ctrlSktInfo.localIPaddress.v4Add.v[2],
                                                                        ctrlSktInfo.localIPaddress.v4Add.v[3],
                                                                        (uint8_t)((dataSktInfo.localPort >> 8)&0xFF),
                                                                        (uint8_t)((dataSktInfo.localPort)&0xFF) );

        if(NET_PRES_SocketWriteIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt, nBytes, 0) >= nBytes)
        {
            memset(pDcpt->ftpcCtrlRxBuff, 0, ftpcGlobalConfig.ctrlCmdRxBuffSize);
            NET_PRES_SocketWrite(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlTxBuff, nBytes);

            pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_PORT;
            SYS_CONSOLE_MESSAGE(" PORT - Sent\r\n");
        } 
    }
   
    pDcpt->ftpcError = res;
    return pDcpt->ftpcState;
}
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitPort(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{    //Response
    //200
    //500, 501, 421, 530
    int     serverReplyCode;
    uint16_t readLen = 0;
    
    if( (readLen = NET_PRES_SocketReadIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt))  >= 4)  
    {
        //pDcpt->ftpcCtrlRxBuff = tempRxbuff;
        SYS_CONSOLE_MESSAGE(" PORT - Read Ready\r\n");
        if(NET_PRES_SocketRead(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlRxBuff, readLen) >= 3)
        {
            //NET_PRES_SocketDiscard(pDcpt->ftpcSocket.ftpcCtrlSkt);
            //Process the reply on ctrl socket
            serverReplyCode = ftpcGetReplyCode(pDcpt->ftpcCtrlRxBuff);
                        
            if(FTPC_IS_REPLY_2YZ(serverReplyCode))
            {
                //PWD success
                pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_PORT;
            }
            else if(FTPC_IS_REPLY_5YZ(serverReplyCode))
            {
                //error 
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition                 
                if(serverReplyCode == 530)
                {                    
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_PORT_NOT_LOGGEDIN;  // path not found
                }
                else
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_PORT_SYNTAX;
            }
            else if(FTPC_IS_REPLY_4YZ(serverReplyCode))
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_PORT_SRVC_NOT_AVLBL; 
            }
            else
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_PORT_UNDEF;
            }
            
        }
        else
        {
            //error
            pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
            pDcpt->ftpcError = TCPIP_FTPC_ERROR_PORT_REPLY_LEN; 
        }
        
    }
    else
    {
        //niyas if timeout == Yes?
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_PORT_TIMEOUT; 
        //niyas if timeout == No?
            //do nothing
    }
    return pDcpt->ftpcState;         
}
static TCPIP_FTPC_STATE_TYPE ftpcStateDonePort(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //niyas verify the need for this function
    //trigger callback function for successful connection
   // (pDcpt->ftpcCtrlRxBuff);
    SYS_CONSOLE_MESSAGE(" PORT - Done\r\n");
    if((pDcpt->ftpcCommand == CMD_GET))
    {
        pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_RETR;
    }
    else if (pDcpt->ftpcCommand == CMD_PUT)
    {
        pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_STOR;
    }
    else
    {
        pDcpt->ftpcBusy = false;
    }
    
    return pDcpt->ftpcState;
}



static TCPIP_FTPC_STATE_TYPE ftpcStateSendPasv(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    uint16_t nBytes = 0;
    //char *address;
    
    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "PASV\r\n");
    
    if(NET_PRES_SocketWriteIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt, nBytes, 0) >= nBytes)
    {
        memset(pDcpt->ftpcCtrlRxBuff, 0, ftpcGlobalConfig.ctrlCmdRxBuffSize);
        NET_PRES_SocketWrite(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlTxBuff, nBytes);
        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_PASV;
    }
   
    return pDcpt->ftpcState;
}
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitPasv(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{    //Response
    //227
    //500, 501, 502, 421, 530
    int     serverReplyCode;
    uint16_t readLen = 0;
    
    if( (readLen = NET_PRES_SocketReadIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt))  >= 4)  
    {
        //pDcpt->ftpcCtrlRxBuff = tempRxbuff;
        
        if(NET_PRES_SocketRead(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlRxBuff, readLen) >= 3)
        {
            NET_PRES_SocketDiscard(pDcpt->ftpcSocket.ftpcCtrlSkt);
            //Process the reply on ctrl socket
            serverReplyCode = ftpcGetReplyCode(pDcpt->ftpcCtrlRxBuff);
                        
            if(FTPC_IS_REPLY_2YZ(serverReplyCode))
            {
                //PWD success
                pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_PASV;
            }
            else if(FTPC_IS_REPLY_5YZ(serverReplyCode))
            {
                //error 
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition                 
                if(serverReplyCode == 530)
                {
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_PASV_NOT_LOGGEDIN;  // path not found
                }
                else if(serverReplyCode == 502)
                {
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_PASV_CMD_NOT_IMPL;  // path not found
                }
                else
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_PASV_SYNTAX;
            }
            else if(FTPC_IS_REPLY_4YZ(serverReplyCode))
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_PASV_SRVC_NOT_AVLBL; 
            }
            else
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_PASV_UNDEF;
            }
            
        }
        else
        {
            //error
            pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
            pDcpt->ftpcError = TCPIP_FTPC_ERROR_PASV_REPLY_LEN; 
        }
        
    }
    else
    {
        //niyas if timeout == Yes?
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_PASV_TIMEOUT; 
        //niyas if timeout == No?
            //do nothing
    }
    return pDcpt->ftpcState;         
}

char pasv_response[6];
uint16_t port;
static TCPIP_FTPC_STATE_TYPE ftpcStateDonePasv(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //niyas verify the need for this function
    //trigger callback function for successful connection
   // (pDcpt->ftpcCtrlRxBuff);
    /*char ip_addr_port[6];
    char * ptr;
    ptr = strtok (pDcpt->ftpcCtrlRxBuff,",");
    for(uint8_t i = 0; i<6; i++)
    {
        ip_addr_port[i] = atoi(ptr);
        SYS_CONSOLE_PRINT("%d \r\n", &ip_addr_port[i]);
        //printf("%d\n", a);
        ptr = strtok (NULL, ",");
    }*/
    TCPIP_FTPC_ERROR_TYPE res = TCPIP_FTPC_ERROR_NONE;
    char * buff;    

    for(unsigned int i = 6; i>0; i--)
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
    port = (pasv_response[4]*256) + pasv_response[5];
    
    
    if(pDcpt->ftpcSocket.ftpcDataSkt == NET_PRES_INVALID_SOCKET)
    {   // get a new socket
        NET_PRES_SKT_HANDLE_T dataSkt;
        NET_PRES_SKT_T sktType =  NET_PRES_SKT_DEFAULT_STREAM_CLIENT;
        dataSkt = NET_PRES_SocketOpen(0, sktType, (NET_PRES_SKT_ADDR_T)pDcpt->ftpcCtrlConnection.ftpcServerIpAddrType, port, 0, 0);
        if(dataSkt == NET_PRES_INVALID_SOCKET)
        {   // failed to open a socket
            res = TCPIP_FTPC_ERROR_OPEN_DATA_SOCKET;
        }
        else
        {   // success; get the socket TX buffer size 
            //TCP_SOCKET_INFO sktInfo;
            //NET_PRES_SocketInfoGet(ctrlSkt, &sktInfo);

            //if needed, modify the tx and rx buffer size of socket
            
            // alert of incoming traffic
            NET_PRES_SocketSignalHandlerRegister(dataSkt, TCPIP_TCP_SIGNAL_RX_DATA, _FTPCSocketRxSignalHandler, 0);

            pDcpt->ftpcSocket.ftpcDataSkt = dataSkt;
        }
    
    }
    if((pDcpt->ftpcCommand == CMD_GET)||(pDcpt->ftpcCommand == CMD_PUT))
    {
        
        if(res == TCPIP_FTPC_ERROR_NONE)
        {
            // point socket to the SMTP server
            if(!NET_PRES_SocketRemoteBind(pDcpt->ftpcSocket.ftpcDataSkt, (NET_PRES_SKT_ADDR_T)pDcpt->ftpcCtrlConnection.ftpcServerIpAddrType,  port, (NET_PRES_ADDRESS*)(pasv_response)))
            {   // failed
                res = TCPIP_FTPC_ERROR_BIND_DATA_SOCKET; 
                NET_PRES_SocketClose(pDcpt->ftpcSocket.ftpcDataSkt);
                SYS_CONSOLE_MESSAGE("Data Socket - error remote bind\r\n");
            }
            SYS_CONSOLE_MESSAGE("Data Socket - remote bind done\r\n");
            if(res == TCPIP_FTPC_ERROR_NONE)
            {
               // memset(pDcpt->ftpcCtrlRxBuff, 0, ftpcGlobalConfig.ctrlCmdRxBuffSize);
                if (!NET_PRES_SocketConnect(pDcpt->ftpcSocket.ftpcDataSkt))
                {
                    res = TCPIP_FTPC_ERROR_CONNECT_DATA_SOCKET; 
                    NET_PRES_SocketClose(pDcpt->ftpcSocket.ftpcDataSkt);
                    SYS_CONSOLE_MESSAGE("Data Socket - error connect\r\n");
                }   
                SYS_CONSOLE_MESSAGE("Data Socket - connect done\r\n");
            }
            ftpcSetDcptTmo(pDcpt, ftpcGlobalConfig.serverDataTmo);
            pDcpt->ftpcState = TCPIP_FTPC_STATE_PASV_WAIT_CONNECT;
        }
        
    }
    else
    {
        pDcpt->ftpcBusy = false;
    }
    
    

    
    return pDcpt->ftpcState;
}

static TCPIP_FTPC_STATE_TYPE ftpcStateWaitPasvConnect(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{

    if (!NET_PRES_SocketIsConnected(pDcpt->ftpcSocket.ftpcDataSkt))
    { //SYS_CONSOLE_MESSAGE(" wait connection - is connected\r\n");
        if(ftpcIsDcptTmoExpired(pDcpt))
        { SYS_CONSOLE_MESSAGE(" wait data connection - tmo\r\n");
            //niyas timeout == yes?
            NET_PRES_SocketClose(pDcpt->ftpcSocket.ftpcDataSkt);
            // pDcpt->ftpcSocket.ftpcCtrlSkt = INVALID_SOCKET;
            pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
            pDcpt->ftpcError = TCPIP_FTPC_ERROR_TIMEOUT_DATA_SOCKET; 
        }
        else
        {
            //niyas timeout == No?
            //do nothing//pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_CONNECT;
        }
         
    }
    else
    {
        SYS_CONSOLE_MESSAGE(" data connection - success\r\n");    
        if(pDcpt->ftpcCommand == CMD_GET)
        {
            pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_RETR;
        }
        else if (pDcpt->ftpcCommand == CMD_PUT)
        {
            pDcpt->ftpcState = TCPIP_FTPC_STATE_SEND_STOR;
        }
    }
    

    return pDcpt->ftpcState; 
    
}


static TCPIP_FTPC_STATE_TYPE ftpcStateSendRetr(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    uint16_t nBytes = 0;
    
    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "RETR %s\r\n", pDcpt->ftpcServerPathname);
    
    if(NET_PRES_SocketWriteIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt, nBytes, 0) >= nBytes)
    {
        memset(pDcpt->ftpcCtrlRxBuff, 0, ftpcGlobalConfig.ctrlCmdRxBuffSize);
        NET_PRES_SocketWrite(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlTxBuff, nBytes);
        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_RETR;
    } 

    return pDcpt->ftpcState;    
}

static TCPIP_FTPC_STATE_TYPE ftpcStateWaitRetrCtrlResp(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //125,150,(110)
    //226,250
    //450,550
    //500, 501, 421, 530
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    int     serverReplyCode;
    int32_t    fileDescr;
    char * buff;
    
    //Read Control Socket
    res = ftpcReadCtrlSocket(pDcpt);
    
    //Process Control Socket response and switch the state
    if(res == TCPIP_FTPC_RES_ERROR)
    {
        ;//handle error niyas
    }
    else if (res == TCPIP_FTPC_RES_OK)
    {
        serverReplyCode = ftpcGetReplyCode(pDcpt->ftpcCtrlRxBuff);
        
        if(FTPC_IS_REPLY_1YZ(serverReplyCode) || FTPC_IS_REPLY_2YZ(serverReplyCode))
        {
            //Open File system
            buff = strrchr(pDcpt->ftpcServerPathname, '/');
            if(!buff)
                buff = (char *)pDcpt->ftpcServerPathname;

            fileDescr = SYS_FS_FileOpen_Wrapper((const char*)buff,SYS_FS_FILE_OPEN_WRITE);
            if( fileDescr != (int32_t) SYS_FS_HANDLE_INVALID )
            {
                pDcpt->fileDescr = fileDescr;
                //Read Data socket
                pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_RETR_READ_DATA_SOCKET;
            }
            else
            {
                ;//error state
            }           
            
        }
        else if(FTPC_IS_REPLY_5YZ(serverReplyCode))
        {
            //error 
            pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition                 
            if(serverReplyCode == 550)
            {                    
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_REQ_ACTION_NOT_TAKEN;  // path not found
            }
            else if(serverReplyCode == 530)
            {
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_NOT_LOGGEDIN;  // path not found
            }
            else if(serverReplyCode == 502)
            {
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_PWD_CMD_NOT_IMPL;  // path not found
            }
            else
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_PWD_SYNTAX;
        }
        else if(FTPC_IS_REPLY_4YZ(serverReplyCode))
        {
            //error
            pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
            pDcpt->ftpcError = TCPIP_FTPC_ERROR_SRVC_NOT_AVLBL; 
        }
        else
        {
            //error
            pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
            pDcpt->ftpcError = TCPIP_FTPC_ERROR_MKD_UNDEF;
        }
    }
    
    return pDcpt->ftpcState; 
}

static TCPIP_FTPC_STATE_TYPE ftpcStateWaitRetrReadDataSkt(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //125,150,(110)
    //226,250
    //450,550
    //500, 501, 421, 530    
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    if (NET_PRES_SocketIsConnected(pDcpt->ftpcSocket.ftpcDataSkt))
    {
        //Read Data Socket
        res = ftpcReadDataSocket(pDcpt);
    }
    else
    {
        
    }
    
    //Read Control Socket
    
    //Process Control Socket response and switch the state
    return pDcpt->ftpcState; 
}

static TCPIP_FTPC_STATE_TYPE ftpcStateWaitRetr(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //125,150,(110)
    //226,250
    //450,550
    //500, 501, 421, 530
    int     serverReplyCode;
    uint16_t readLen = 0;
    
    if( (readLen = NET_PRES_SocketReadIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt))  >= 4)  
    {
                
        if(NET_PRES_SocketRead(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlRxBuff, readLen) >= 3)
        {
            //NET_PRES_SocketDiscard(pDcpt->ftpcSocket.ftpcCtrlSkt);
            //Process the reply on ctrl socket
            serverReplyCode = ftpcGetReplyCode(pDcpt->ftpcCtrlRxBuff);
                        
            if(FTPC_IS_REPLY_1YZ(serverReplyCode))
            {
                //CDUP success
                pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_RETR1;
                
            }
            else if(FTPC_IS_REPLY_2YZ(serverReplyCode))
            {
                //CDUP success
                pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_RETR;
            }
            else if(FTPC_IS_REPLY_5YZ(serverReplyCode))
            {
                //error 
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition                 
                if(serverReplyCode == 550)
                {                    
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_REQ_ACTION_NOT_TAKEN;  // path not found
                }
                else if(serverReplyCode == 530)
                {
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_NOT_LOGGEDIN;  // path not found
                }
                else if(serverReplyCode == 502)
                {
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_PWD_CMD_NOT_IMPL;  // path not found
                }
                else
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_PWD_SYNTAX;
            }
            else if(FTPC_IS_REPLY_4YZ(serverReplyCode))
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_SRVC_NOT_AVLBL; 
            }
            else
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_MKD_UNDEF;
            }
            
        }
        else
        {
            //error
            pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_RETR; //niyas revisit for error condition
            pDcpt->ftpcError = TCPIP_FTPC_ERROR_MKD_REPLY_LEN; 
        }
        
    }
    else
    {
        //niyas if timeout == Yes?
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
        pDcpt->ftpcError = TCPIP_FTPC_STATE_WAIT_RETR; 
        //niyas if timeout == No?
            //do nothing
    }
    return pDcpt->ftpcState;     
}

static TCPIP_FTPC_STATE_TYPE ftpcStateWaitRetr1(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //125,150,(110)
    //226,250
    //450,550
    //500, 501, 421, 530
    int     serverReplyCode;
    uint16_t readLen = 0;

    //if( (readLen = NET_PRES_SocketReadIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt))  >= 4)  
    {
        readLen =  NET_PRES_SocketRead(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlRxBuff, 4);       
        if(readLen >= 4)
        {
            //NET_PRES_SocketDiscard(pDcpt->ftpcSocket.ftpcCtrlSkt);
            //Process the reply on ctrl socket
            serverReplyCode = ftpcGetReplyCode(pDcpt->ftpcCtrlRxBuff);
                        
            if(FTPC_IS_REPLY_2YZ(serverReplyCode))
            {
                //CDUP success
                pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_RETR;
            }
            else if(FTPC_IS_REPLY_5YZ(serverReplyCode))
            {
                //error 
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition                 
                if(serverReplyCode == 550)
                {                    
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_REQ_ACTION_NOT_TAKEN;  // path not found
                }
                else if(serverReplyCode == 530)
                {
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_NOT_LOGGEDIN;  // path not found
                }
                else if(serverReplyCode == 502)
                {
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_PWD_CMD_NOT_IMPL;  // path not found
                }
                else
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_PWD_SYNTAX;
            }
            else if(FTPC_IS_REPLY_4YZ(serverReplyCode))
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_SRVC_NOT_AVLBL; 
            }
            else
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_MKD_UNDEF;
            }
            
        }
        else
        {
            //error
            pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_RETR1; //niyas revisit for error condition
            pDcpt->ftpcError = TCPIP_FTPC_ERROR_MKD_REPLY_LEN; 
        }
        
    }

    return pDcpt->ftpcState;     
}

char test_buffer[30];
static TCPIP_FTPC_STATE_TYPE ftpcStateDoneRetr(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{ 
    //niyas verify the need for this function
    //trigger callback function for successful connection
    uint16_t readLen = 0;
    int32_t    fp = pDcpt->fileDescr;
    char * buff;
    
    buff = strrchr(pDcpt->ftpcServerPathname, '/');
    if(!buff)
        buff = (char *)pDcpt->ftpcServerPathname;
        
    //test code SYS_FS_DirectoryMake("mchp");//testing code niyas 
       
    fp = SYS_FS_FileOpen_Wrapper((const char*)buff,SYS_FS_FILE_OPEN_WRITE);
    if( fp == (int32_t) SYS_FS_HANDLE_INVALID )
    {// File not found, so abort
        // error condition
        ;// niyas 
    }
    pDcpt->fileDescr = fp;
    
    if( (readLen = NET_PRES_SocketReadIsReady(pDcpt->ftpcSocket.ftpcDataSkt))  > 0)  
    {
        //pDcpt->ftpcCtrlRxBuff = tempRxbuff;
        
        if(NET_PRES_SocketRead(pDcpt->ftpcSocket.ftpcDataSkt, pDcpt->ftpcDataRxBuff, readLen) > 0)
        {
            //NET_PRES_SocketDiscard(pDcpt->ftpcSocket.ftpcCtrlSkt);
            //NET_PRES_SocketClose(pDcpt->ftpcSocket.ftpcDataSkt);
            strncpy(test_buffer,pDcpt->ftpcDataRxBuff,20 );
            SYS_CONSOLE_PRINT("FTPC File GET: %s\r\n", test_buffer);
            if(SYS_FS_FileWrite(fp,pDcpt->ftpcDataRxBuff,readLen) == SYS_FS_HANDLE_INVALID)
            {
                SYS_FS_FileClose(pDcpt->fileDescr);
                pDcpt->fileDescr = (int32_t) SYS_FS_HANDLE_INVALID;
            }
            SYS_FS_FileClose(pDcpt->fileDescr);

        }        
        pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_GET;
    }

    return pDcpt->ftpcState;
}










static TCPIP_FTPC_STATE_TYPE ftpcStateSendNlst(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    uint16_t nBytes = 0;
    
    nBytes = sprintf(pDcpt->ftpcCtrlTxBuff, "NLST %s\r\n", pDcpt->ftpcServerPathname);
    
    if(NET_PRES_SocketWriteIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt, nBytes, 0) >= nBytes)
    {
        memset(pDcpt->ftpcCtrlRxBuff, 0, ftpcGlobalConfig.ctrlCmdRxBuffSize);
        NET_PRES_SocketWrite(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlTxBuff, nBytes);
        
//        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_NLST;
    } 
    /*if(TCPIP_TCP_PutIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt) != 0)
    {
        sprintf(pDcpt->ftpcCtrlCmdBuff, "RMD %s\r\n", pDcpt->ftpcServerPathname);
        
        TCPIP_TCP_ArrayPut(pDcpt->ftpcSocket.ftpcCtrlSkt, (uint8_t*)pDcpt->ftpcCtrlCmdBuff, strlen(pDcpt->ftpcCtrlCmdBuff));
        pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_RMD;
        //todo niyas : start timeout
    }*/
    
    return pDcpt->ftpcState;    
}
static TCPIP_FTPC_STATE_TYPE ftpcStateWaitNlst(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //Response
    //250
    //500, 501, 502, 421, 530, 550
    int     serverReplyCode;
    uint16_t readLen = 0;
    
    if( (readLen = NET_PRES_SocketReadIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt))  >= 4)  
    {
        //pDcpt->ftpcCtrlRxBuff = tempRxbuff;
        
        if(NET_PRES_SocketRead(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlRxBuff, readLen) >= 3)
        {
            NET_PRES_SocketDiscard(pDcpt->ftpcSocket.ftpcCtrlSkt);
            //Process the reply on ctrl socket
            serverReplyCode = ftpcGetReplyCode(pDcpt->ftpcCtrlRxBuff);
                        
            if(FTPC_IS_REPLY_2YZ(serverReplyCode))
            {
                //CDUP success
//                pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_NLST;
            }
            else if(FTPC_IS_REPLY_5YZ(serverReplyCode))
            {
                //error 
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition                 
                if(serverReplyCode == 550)
                {                    
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_REQ_ACTION_NOT_TAKEN;  // path not found
                }
                else if(serverReplyCode == 530)
                {
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_NOT_LOGGEDIN;  // path not found
                }
                else if(serverReplyCode == 502)
                {
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_PWD_CMD_NOT_IMPL;  // path not found
                }
                else
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_PWD_SYNTAX;
            }
            else if(FTPC_IS_REPLY_4YZ(serverReplyCode))
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_SRVC_NOT_AVLBL; 
            }
            else
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_RMD_UNDEF;
            }
            
        }
        else
        {
            //error
            pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
            pDcpt->ftpcError = TCPIP_FTPC_ERROR_RMD_REPLY_LEN; 
        }
        
    }
    else
    {
        //niyas if timeout == Yes?
        pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
        pDcpt->ftpcError = TCPIP_FTPC_ERROR_TIMEOUT_RMD; 
        //niyas if timeout == No?
            //do nothing
    }
    return pDcpt->ftpcState;         
}
static TCPIP_FTPC_STATE_TYPE ftpcStateDoneNlst(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //niyas verify the need for this function
    //trigger callback function for successful connection
    pDcpt->ftpcBusy = false;
    return pDcpt->ftpcState;
}





static TCPIP_FTPC_STATE_TYPE ftpcStateDisconnect(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //todo niyas Handle different errors using a func pointer table
    
    //release busy flag
    pDcpt->ftpcBusy = false;
    
    return pDcpt->ftpcState;
}

static TCPIP_FTPC_STATE_TYPE ftpcStateError(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //todo niyas Handle different errors using a func pointer table
    
    //release busy flag
    pDcpt->ftpcBusy = false;
    
    return pDcpt->ftpcState;
}

/*static TCPIP_FTPC_FLAG_TYPE ftpcIsState(TCPIP_FTPC_DCPT_TYPE* pDcpt, TCPIP_FTPC_FLAG_TYPE ftpcFlag)
{
    return (TCPIP_FTPC_FLAG_TYPE)(pDcpt->ftpcFlag & ftpcFlag);
}*/

static int ftpcGetReplyCode(char* buff)
{
    int serverReplyCode = 0;
    //char *serverResponse;
    
    
   // serverResponse = pDcpt->ftpcCtrlReplyBuff;
    //Properly terminate the string with a NULL character
   // serverResponse[pDcpt->ftpcCtrlReplyLen] = '\0';
    // convert ascii to integer
    serverReplyCode = atoi(buff);
    //todo niyas : handle multiline response
    return serverReplyCode;
}

static int ftpcGetReplyPathname(TCPIP_FTPC_DCPT_TYPE* pDcpt, char* pathname, size_t length)
{
    return 0; //niyas
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

static TCPIP_FTPC_RESULT_TYPE ftpcReadCtrlSocket(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    //int     serverReplyCode;
    uint16_t readLen = 0;
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    
    if( (readLen = NET_PRES_SocketReadIsReady(pDcpt->ftpcSocket.ftpcCtrlSkt))  >= 4)  
    {
        //pDcpt->ftpcCtrlRxBuff = tempRxbuff;
        
        if(NET_PRES_SocketRead(pDcpt->ftpcSocket.ftpcCtrlSkt, pDcpt->ftpcCtrlRxBuff, readLen) >= 3)
        {
            NET_PRES_SocketDiscard(pDcpt->ftpcSocket.ftpcCtrlSkt);
            res = TCPIP_FTPC_RES_OK;
            //Process the reply on ctrl socket
            //serverReplyCode = ftpcGetReplyCode(pDcpt->ftpcCtrlRxBuff);
        }
    }
    return res;
    
}

static TCPIP_FTPC_RESULT_TYPE ftpcReadDataSocket(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{    
    uint16_t readLen = 0;
    TCPIP_FTPC_RESULT_TYPE res = TCPIP_FTPC_RES_PENDING;
    int32_t    fileDescr = pDcpt->fileDescr;
    
    if( (readLen = NET_PRES_SocketReadIsReady(pDcpt->ftpcSocket.ftpcDataSkt))  > 0)  
    {  
        
        if(NET_PRES_SocketRead(pDcpt->ftpcSocket.ftpcDataSkt, pDcpt->ftpcDataRxBuff, readLen) > 0)
        {
            //NET_PRES_SocketDiscard(pDcpt->ftpcSocket.ftpcCtrlSkt);
            //NET_PRES_SocketClose(pDcpt->ftpcSocket.ftpcDataSkt);
            //strncpy(test_buffer,pDcpt->ftpcDataRxBuff,20 );
            //SYS_CONSOLE_PRINT("FTPC File GET: %s\r\n", test_buffer);
            if(SYS_FS_FileWrite(fileDescr,pDcpt->ftpcDataRxBuff,readLen) == SYS_FS_HANDLE_INVALID)
            {
                SYS_FS_FileClose(pDcpt->fileDescr);
                pDcpt->fileDescr = (int32_t) SYS_FS_HANDLE_INVALID;
            }
            //SYS_FS_FileClose(pDcpt->fileDescr);

        }        
        //pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_GET;
    }
    return res;
    
}

static TCPIP_FTPC_STATE_TYPE ftpcProcessCtrlResponse(TCPIP_FTPC_DCPT_TYPE* pDcpt)
{
    int     serverReplyCode;
    TCPIP_FTPC_STATE_TYPE current_state, new_state;
    
    serverReplyCode = ftpcGetReplyCode(pDcpt->ftpcCtrlRxBuff);
    current_state = pDcpt->ftpcState;
    switch (current_state)
    {
        case TCPIP_FTPC_STATE_WAIT_RETR_CTRL_RESPONSE:
            if(FTPC_IS_REPLY_1YZ(serverReplyCode))
            {
                //CDUP success
                pDcpt->ftpcState = TCPIP_FTPC_STATE_WAIT_RETR1;
                
            }
            else if(FTPC_IS_REPLY_2YZ(serverReplyCode))
            {
                //CDUP success
                pDcpt->ftpcState = TCPIP_FTPC_STATE_DONE_RETR;
            }
            else if(FTPC_IS_REPLY_5YZ(serverReplyCode))
            {
                //error 
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition                 
                if(serverReplyCode == 550)
                {                    
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_REQ_ACTION_NOT_TAKEN;  // path not found
                }
                else if(serverReplyCode == 530)
                {
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_NOT_LOGGEDIN;  // path not found
                }
                else if(serverReplyCode == 502)
                {
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_PWD_CMD_NOT_IMPL;  // path not found
                }
                else
                    pDcpt->ftpcError = TCPIP_FTPC_ERROR_PWD_SYNTAX;
            }
            else if(FTPC_IS_REPLY_4YZ(serverReplyCode))
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_SRVC_NOT_AVLBL; 
            }
            else
            {
                //error
                pDcpt->ftpcState = TCPIP_FTPC_STATE_ERROR; //niyas revisit for error condition
                pDcpt->ftpcError = TCPIP_FTPC_ERROR_MKD_UNDEF;
            }
            break;
        default:
            break;
    }
           
    
}

#endif //#if defined(TCPIP_STACK_USE_FTPC)
