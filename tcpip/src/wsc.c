/*******************************************************************************
  WebSocket Protocol Client (WSC)

  Summary:
    WSC implementation file
    
  Description:
    This source file contains the functions of the WSC routines
    
    Provides WebSocket Ciient implementation
    Reference: RFC 6455
*******************************************************************************/

/*
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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









#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_WS_CLIENT

#include "tcpip/src/tcpip_private.h"

#if defined(TCPIP_STACK_USE_WS_CLIENT)
#include "net_pres/pres/net_pres_socketapi.h"
#include "tcpip/src/wsc_private.h"
#include "crypto/crypto.h"

/****************************************************************************
  Section:
    Prototypes
  ***************************************************************************/
#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void F_WSC_Cleanup(const TCPIP_STACK_MODULE_CTRL* const stackCtrl);
static void F_WSC_AbortConnections(TCPIP_NET_IF* pNetIf);
#else
#define F_WSC_Cleanup(stackCtrl)
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)



// task functions
static void WSC_Task_Idle(TCPIP_WSC_CONN_CTRL* pWsc);
static void WSC_Task_StartDns(TCPIP_WSC_CONN_CTRL* pWsc);
static void WSC_Task_WaitDns(TCPIP_WSC_CONN_CTRL* pWsc);
static void WSC_Task_StartConnect(TCPIP_WSC_CONN_CTRL* pWsc);
static void WSC_Task_WaitConnect(TCPIP_WSC_CONN_CTRL* pWsc);
static void WSC_Task_WaitTlsConnect(TCPIP_WSC_CONN_CTRL* pWsc);
static void WSC_Task_StartHandshake(TCPIP_WSC_CONN_CTRL* pWsc);
static void WSC_Task_WaitHandshake(TCPIP_WSC_CONN_CTRL* pWsc);
static void WSC_Task_Process(TCPIP_WSC_CONN_CTRL* pWsc);
static void WSC_Task_Closed(TCPIP_WSC_CONN_CTRL* pWsc);


static void     WSC_OpenConnection(TCPIP_WSC_CONN_CTRL* pWsc);
static TCPIP_WSC_RES WSC_OpenSocket(TCPIP_WSC_CONN_CTRL* pConn);
static void     WSC_RxSignalHandler(NET_PRES_SKT_HANDLE_T hTCP, NET_PRES_SIGNAL_HANDLE hNet, uint16_t sigType, const void* param);

static void     WSC_CloseConnection(TCPIP_WSC_CONN_CTRL* pWsc, TCPIP_WSC_RES errRes);
static void     WSC_FailConnection(TCPIP_WSC_CONN_CTRL* pWsc, TCPIP_WSC_RES errRes);
static void     WSC_AbortConnection(TCPIP_WSC_CONN_CTRL* pWsc, TCPIP_WSC_RES errRes);
static int      WSC_GenerateKey(uint8_t* base64Buffer, size_t buffLen);
static uint32_t WSC_ValidateResponse(TCPIP_WSC_CONN_CTRL* pWsc, char* srvResponse);

static TCPIP_WSC_RES WSC_SendCtrlFrame(TCPIP_WSC_CONN_CTRL* pWsc, uint8_t opCode, uint8_t* payloadBuff, uint16_t payloadLen);
static TCPIP_WSC_RES WSC_SendCloseFrame(TCPIP_WSC_CONN_CTRL* pWsc, TCPIP_WSC_CLOSE_CODE closeCode, const char* reason);
static TCPIP_WSC_RES WSC_WriteFrameHeader(TCPIP_WSC_CONN_CTRL* pWsc, WSC_PEND_TX_MSG* pTxMsg);
static size_t WSC_SendMsgFrame(TCPIP_WSC_CONN_CTRL* pWsc, WSC_PEND_TX_MSG* pTxMsg, TCPIP_WSC_RES* pRes);

static bool WSC_RxFrame(TCPIP_WSC_CONN_CTRL* pWsc);
static bool WSC_ProcPendMsg(TCPIP_WSC_CONN_CTRL* pWsc);
static uint16_t WSC_ProcPendCtrl(TCPIP_WSC_CONN_CTRL* pWsc);
static bool WSC_DiscardFrame(TCPIP_WSC_CONN_CTRL* pWsc, TCPIP_WSC_RES dReason);
static bool WSC_SrvRxUpdate(TCPIP_WSC_CONN_CTRL* pWsc, TCPIP_WSC_PEND_MSG_DCPT* rxDcpt);
static bool WSC_Notify_Event(TCPIP_WSC_CONN_CTRL* pWsc, TCPIP_WSC_EVENT_TYPE evType, TCPIP_WSC_EV_INFO evInfo);
static bool WSC_ConnMatch(TCPIP_WSC_CONN_CTRL* pW1, TCPIP_WSC_CONN_CTRL* pW2);

static bool WSC_IsSrvTmo(TCPIP_WSC_CONN_CTRL* pWsc); 
static bool WSC_IsUsrTmo(TCPIP_WSC_CONN_CTRL* pWsc); 

static void Wsc_ConnInit(TCPIP_WSC_CONN_CTRL* pWsc);
static uint8_t WSC_MsgFlags(WS_FRAME_HEADER frameHdr);
static TCPIP_WSC_EVENT_TYPE WSC_CtrlEvent(TCPIP_WS_OP_CODE opCode);

static bool WSC_ConnectionLock(TCPIP_WSC_CONN_CTRL* pWsc, bool taskProcess);
static void WSC_ConnectionUnlock(TCPIP_WSC_CONN_CTRL* pWsc, bool taskProcess);

int WSC_RngGenerate(uint8_t* rngBuff, size_t buffSize);

static bool strcany(const char* delim, int ch);
static const char* strdelim(const char* str, const char* delim);
static const char* strdelimr(const char* str, const char* delim);
static const char* strstrany(const char* str, const char* delim, const char** pEndDelim);

int FC_sprintf(char* buff, size_t buffSize, const char* fmt, ...)
{
    int needBytes;
    
    va_list args;
    va_start( args, fmt );

    needBytes = vsnprintf(buff, buffSize, fmt, args);
    _TCPIPStack_Assert(needBytes >= 0, __FILE__, __func__, __LINE__);

    va_end( args );

    _TCPIPStack_Assert((size_t)needBytes <= buffSize, __FILE__, __func__, __LINE__);
    return needBytes;
}


/****************************************************************************
  Section:
    WSC module descriptor global Variable
  ***************************************************************************/
typedef struct TAG_TCPIP_WSC_MODULE_DCPT
{
    TCPIP_WSC_CONN_CTRL*    wscConnCtrl;        // pointer to all WSC connections
    const void*             wscMemH;            // handle to be used in the TCPIP_HEAP_ calls
    tcpipSignalHandle       wscSignalHandle;    // WSC task signal handle
    uint16_t                wscConnNo;          // number of WSC connections
    uint16_t                wscConfigFlags;     // run time flags: TCPIP_WSC_MODULE_FLAGS value
    uint16_t                sktTxBuffSize;      // size of TX buffer for the associated socket
    uint16_t                sktRxBuffSize;      // size of RX buffer for the associated socket
    uint16_t                srvTmoMs;           // server timeout value, ms
    uint16_t                usrTmoMs;           // user timeout value, ms
    TCPIP_WSC_EVENT_HANDLER evHandler;          // global event handler, for all connections
    const void*             hParam;             // handler user parameter

    // user threads protection semaphore
    OSAL_SEM_HANDLE_TYPE    gblSem;             // global/user synchronization semaphore
    uint8_t                 gblSemValid;        // the global semaphore is valid
    uint8_t                 rtosUsed;           // WSC running under an RTOS
#if (M_WSC_SEM_LOCK != 0) && (M_WSC_SEM_PER_CONNECTION == 0)
    uint8_t                 gblSemCount;        // counter for using the global user sem for connection lock
                                                // i.e. when  (M_WSC_SEM_LOCK != 0) and (M_WSC_SEM_PER_CONNECTION == 0)
#endif  // (M_WSC_SEM_LOCK != 0) && (M_WSC_SEM_PER_CONNECTION == 0)

}TCPIP_WSC_MODULE_DCPT;

// The User/WSC (dispatcher) threads protection
// There is a global semaphore to protect access to the TCPIP_WSC_MODULE_DCPT.
// That's always used for TCPIP_WSC_ConnOpen() and TCPIP_WSC_ConnClose() APIs. 
//
// For the WSC connection data there are two types of RTOS protection:
//  - M_WSC_SEM_LOCK == 1:
//      This is the default setting.
//      Connection protection is made using a semaphore.
//      If TCPIP_WSC_CONNECTION_SEM != 0
//              then each connection has its own semaphore.
//      If TCPIP_WSC_CONNECTION_SEM == 0
//              For efficiency reasons (a semaphore seems to be 'heavy' in most RTOS-es implementations)
//              the one and only global semaphore is used for all connections.
//
//      The user threads will wait for their access and the operation result will NEVER be TCPIP_WSC_RES_BUSY! 
//      Advantages:
//          - The calling thread is transparently blocked by the native RTOS without any kind of API usage impact for the user
//          - The operations do not need retries.
//      Disadvantages:
//          - If TCPIP_WSC_CONNECTION_SEM == 0
//                  - A connection can be ready (while another is busy), but it must wait because of the shared semaphore.
//          - The WSC module RX thread (driven from the stack) can be blocked waiting for the user to release the semaphores.
//          - Higher memory usage when TCPIP_WSC_CONNECTION_SEM != 0
//
//  - M_WSC_SEM_LOCK == 0:
//      In order to avoid conflict to the resources the WSC API does not block
//      but can return TCPIP_WSC_RES_BUSY and the operation returns.
//      An internal lock is maintained for each connections.
//      Advantages:
//          - Very quick and fast operation.
//          - The lock is maintained per connection so there's no interference between connections (and their users).
//      Disadvantages:
//          - The user needs to check the operation return value and retry the operation if TCPIP_WSC_RES_BUSY received. 
//
//
//
// Some restrictions:
//  - A WSC handle cannot be used concurrently from multiple threads!
//    It's ok to pass a handle from one thread to another as long as
//    there's is no access from more than one thread at a time
//
static __inline__ bool  __attribute__((always_inline))   UserGblLockCreate(TCPIP_WSC_MODULE_DCPT* pDcpt)
{
    // create the shared Data Lock
    OSAL_RESULT res = OSAL_SEM_Create(&pDcpt->gblSem, OSAL_SEM_TYPE_BINARY, 1, 1);
    if(res == OSAL_RESULT_SUCCESS)
    {
        pDcpt->gblSemValid = 1U;
        return true;
    }

    return false;
}    

static __inline__ void  __attribute__((always_inline)) UserGblLockDelete(TCPIP_WSC_MODULE_DCPT* pDcpt)
{
    pDcpt->gblSemValid = 0U;
    (void) OSAL_SEM_Delete(&pDcpt->gblSem);
}    

// locks access to shared resources
static __inline__ void  __attribute__((always_inline))  UserGblLock(TCPIP_WSC_MODULE_DCPT* pDcpt)
{
    // Shared Data Lock
    (void)OSAL_SEM_Pend(&pDcpt->gblSem, OSAL_WAIT_FOREVER);
}    

// unlocks access to shared resources
static __inline__ void  __attribute__((always_inline))  UserGblUnlock(TCPIP_WSC_MODULE_DCPT* pDcpt)
{
    // Shared Data unlock
    (void)OSAL_SEM_Post(&pDcpt->gblSem);
}

#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_STAT) != 0)
static const char* intState_Tbl[] = 
{
    "idle",         // WSC_INT_STAT_IDLE
    "busy",         // WSC_INT_STAT_BUSY
    "sdns",         // WSC_INT_STAT_START_DNS
    "wdns",         // WSC_INT_STAT_WAIT_DNS
    "sconn",        // WSC_INT_STAT_START_CONNECT
    "wconn",        // WSC_INT_STAT_WAIT_CONNECT
    "wtls",         // WSC_INT_STAT_WAIT_TLS_CONNECT
    "shhk",         // WSC_INT_STAT_START_HSHAKE
    "wshk",         // WSC_INT_STAT_WAIT_HSHAKE
    "open",         // WSC_INT_STAT_OPEN
    "closing",      // WSC_INT_STAT_CLOSING
    "closed",       // WSC_INT_STAT_CLOSED
};

static const char* connState_Tbl[] = 
{
    "idle",         // TCPIP_WSC_CONN_STAT_IDLE
    "conn",         // TCPIP_WSC_CONN_STAT_CONNECTING
    "open",         // TCPIP_WSC_CONN_STAT_OPEN
    "closing",      // TCPIP_WSC_CONN_STAT_CLOSING
    "closed",       // TCPIP_WSC_CONN_STAT_CLOSED
};

static void  SetIntState(TCPIP_WSC_CONN_CTRL* pWsc, WSC_INT_STATUS intState)
{
    if((uint16_t)intState != pWsc->intState)
    {
        SYS_CONSOLE_PRINT("WSC Int state: %s\r\n", intState_Tbl[intState]); 
    } 
    pWsc->intState = (uint16_t)intState;
}
static void  SetWscState(TCPIP_WSC_CONN_CTRL* pWsc, TCPIP_WSC_CONN_STATUS connState)
{
    if((uint16_t)connState != pWsc->connState)
    {
        SYS_CONSOLE_PRINT("WSC Conn state: %s\r\n", connState_Tbl[connState]); 
    } 
    pWsc->connState = (uint16_t)connState;
}
#else
static __inline__ void  __attribute__((always_inline))  SetIntState(TCPIP_WSC_CONN_CTRL* pWsc, WSC_INT_STATUS intState)
{
    pWsc->intState = (uint16_t)intState;
}
static __inline__ void  __attribute__((always_inline))  SetWscState(TCPIP_WSC_CONN_CTRL* pWsc, TCPIP_WSC_CONN_STATUS connState)
{
    pWsc->connState = (uint16_t)connState;
}
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_STAT) != 0)

static __inline__ TCPIP_WSC_CONN_CTRL* __attribute__((always_inline)) FC_ConnHndl2ConnPtr(TCPIP_WSC_CONN_HANDLE hConn)
{
    union
    {
        TCPIP_WSC_CONN_HANDLE hConn;
        TCPIP_WSC_CONN_CTRL* pWsc;
    }U_CONN_HNDL_PTR;

    U_CONN_HNDL_PTR.hConn = hConn;
    return U_CONN_HNDL_PTR.pWsc;
}

static TCPIP_WSC_EV_HANDLE FC_EvHndler2Handle(TCPIP_WSC_EVENT_HANDLER handler)
{
    union
    {
        TCPIP_WSC_EVENT_HANDLER handler;
        TCPIP_WSC_EV_HANDLE     evHandle;
    }U_EV_HANDLER_EV_HANDLE;
    
    U_EV_HANDLER_EV_HANDLE.handler = handler;
    return U_EV_HANDLER_EV_HANDLE.evHandle;
}

static __inline__ void __attribute__((always_inline)) WSC_StartSrvWaitTimer(TCPIP_WSC_CONN_CTRL* pWsc)
{
    if(pWsc->parent->srvTmoMs != 0U)
    {
        pWsc->startSrvWaitMs = _TCPIP_MsecCountGet();
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SRV_TMO) != 0)
        SYS_CONSOLE_PRINT("WSC conn: %d, Srv Start Wait: %d\r\n", pWsc->connIx, pWsc->startSrvWaitMs);
#endif  //((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SRV_TMO) != 0)
    }
}
static bool WSC_IsSrvTmo(TCPIP_WSC_CONN_CTRL* pWsc)
{
    if(pWsc->parent->srvTmoMs != 0U)
    {
        uint32_t currMsec = _TCPIP_MsecCountGet();
        if((currMsec - pWsc->startSrvWaitMs) >= (uint32_t)pWsc->parent->srvTmoMs)
        {
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SRV_TMO) != 0)
            SYS_CONSOLE_PRINT("WSC Srv Tmo - conn: %d, time: %d, tmo: %d\r\n", pWsc->connIx, currMsec, pWsc->parent->srvTmoMs);
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SRV_TMO) != 0)
            return true;
        }
    }
    return false;
}

static __inline__ void __attribute__((always_inline)) WSC_StartUsrWaitTimer(TCPIP_WSC_CONN_CTRL* pWsc)
{
    if(pWsc->parent->usrTmoMs != 0U)
    {
        pWsc->startUsrWaitMs = _TCPIP_MsecCountGet();
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_USR_TMO) != 0)
        SYS_CONSOLE_PRINT("WSC conn: %d, Usr Start Wait: %d\r\n", pWsc->connIx, pWsc->startUsrWaitMs);
#endif  //((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_USR_TMO) != 0)
    }
}
static bool WSC_IsUsrTmo(TCPIP_WSC_CONN_CTRL* pWsc)
{
    if(pWsc->parent->usrTmoMs != 0U)
    {
        uint32_t currMsec = _TCPIP_MsecCountGet();
        if((currMsec - pWsc->startUsrWaitMs) >= (uint32_t)pWsc->parent->usrTmoMs)
        {
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_USR_TMO) != 0)
            SYS_CONSOLE_PRINT("WSC Usr Tmo - conn: %d, time: %d, tmo: %d\r\n", pWsc->connIx, currMsec, pWsc->parent->usrTmoMs);
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_USR_TMO) != 0)
            return true;
        }
    }
    return false;
}


static int                  wscInitCount = 0;      // module init counter

static TCPIP_WSC_MODULE_DCPT wscModDcpt;            // global WSC descriptor
static TCPIP_WSC_MODULE_DCPT* gWscDcpt = NULL;      // quick reference


static TCPIP_WSC_CONN_CTRL* WSC_ValidateConn(TCPIP_WSC_CONN_HANDLE hConn)
{
    TCPIP_WSC_CONN_CTRL* pWsc = FC_ConnHndl2ConnPtr(hConn);

    if(gWscDcpt != NULL && pWsc != NULL)
    {
        int connIx = pWsc - gWscDcpt->wscConnCtrl;
        if(connIx >= 0 && connIx < gWscDcpt->wscConnNo)
        {
            TCPIP_WSC_CONN_CTRL* pCtrl = gWscDcpt->wscConnCtrl + connIx;
            if(pCtrl == pWsc)
            {
                return pWsc;
            }
        }
    }

    return NULL;
}

// returns true if could lock
// false otherwise
// if taskProcess == true:
//      then it's the WSC task that tries to lock access
//      and it's better to avoid repeatedly getting/releasing the global semaphore
//      if using it for all connections!
// if taskProcess == false:
//      regular individual, per connection access is needed
//      usually from the user API
// Note: always true when no RTOS or using a semaphore
//      Could be false when using a critical section, not a semaphore 
static bool WSC_ConnectionLock(TCPIP_WSC_CONN_CTRL* pWsc, bool taskProcess)
{
    if(pWsc->parent->rtosUsed != 0U)
    {
#if (M_WSC_SEM_LOCK != 0)
#if (M_WSC_SEM_PER_CONNECTION != 0)
        (void)OSAL_SEM_Pend(&pWsc->connSem, OSAL_WAIT_FOREVER);
#else
        if(!taskProcess)
        {
            UserGblLock(pWsc->parent);
        }
        else
        {
            if(pWsc->parent->gblSemCount++ == 0U)
            {   // take the semaphore only for the 1st connection
                UserGblLock(pWsc->parent);
            }
        }
#endif  // (M_WSC_SEM_PER_CONNECTION != 0)
        return true; 
#else // (M_WSC_SEM_LOCK == 0)
        bool lockRes;
        OSAL_CRITSECT_DATA_TYPE critStat = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        if(pWsc->connLock == 0U)
        {
            pWsc->connLock = 1;
            lockRes = true; 
        }
        else
        {
            lockRes = false; 
        }
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critStat);
        return lockRes;
#endif  // (M_WSC_SEM_LOCK != 0)
    }

    return true; 
}

static void WSC_ConnectionUnlock(TCPIP_WSC_CONN_CTRL* pWsc, bool taskProcess)
{
    if(pWsc->parent->rtosUsed != 0U)
    {
#if (M_WSC_SEM_LOCK != 0)
#if (M_WSC_SEM_PER_CONNECTION != 0)
        (void)OSAL_SEM_Post(&pWsc->connSem);
#else
        if(!taskProcess)
        {
            UserGblUnlock(pWsc->parent);
        }
        else
        {
            if(--pWsc->parent->gblSemCount == 0U)
            {   // release the semaphore for the last connection
                UserGblUnlock(pWsc->parent);
            }
        }
#endif  // (M_WSC_SEM_PER_CONNECTION != 0)
#else // (M_WSC_SEM_LOCK == 0)
        OSAL_CRITSECT_DATA_TYPE critStat = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        pWsc->connLock = 0;
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critStat);
#endif  // (M_WSC_SEM_LOCK != 0)
    }

}


// Implementation
//
// Initialization and interface functions

#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SRV_TMO) != 0)
uint32_t dbgSrvTmo = 0;
#endif // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SRV_TMO) != 0)

#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_USR_TMO) != 0)
uint32_t dbgUsrTmo = 0;
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_USR_TMO) != 0)

#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SKT_SIZE) != 0)
uint32_t dbgSktRxSize = 512;
uint32_t dbgSktTxSize = TCPIP_WSC_SKT_TX_BUFF_SIZE;
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SKT_SIZE) != 0)
bool  TCPIP_WSC_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* initData)
{
    bool        initFail;
    uint16_t    connIx, nConns;
    TCPIP_WSC_CONN_CTRL *wscConnCtrl, *pWscCon;

    if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }

    // stack init
    initFail = false;
    while(wscInitCount == 0)
    {   // first time we're run
        (void)memset(&wscModDcpt, 0, sizeof(wscModDcpt));
        _TCPIPStack_Assert(gWscDcpt == NULL, __FILE__, __func__, __LINE__);

        const TCPIP_WSC_MODULE_CONFIG* wscInitData = (const TCPIP_WSC_MODULE_CONFIG*)initData;
        if(wscInitData == NULL || wscInitData->nConnections == 0U)
        {
            return false;
        }

        nConns = wscInitData->nConnections;
        const void* wscMemH = stackCtrl->memH;
        wscConnCtrl = (TCPIP_WSC_CONN_CTRL*)TCPIP_HEAP_Calloc(wscMemH, nConns, sizeof(*wscConnCtrl));

        if(wscConnCtrl == NULL)
        {   // failed
            SYS_ERROR(SYS_ERROR_ERROR, " WSC: Dynamic allocation failed\r\n");
            return false;
        }

        wscModDcpt.wscConnCtrl = wscConnCtrl;
        wscModDcpt.wscMemH = wscMemH;
        wscModDcpt.wscConnNo = nConns;

        // create the WSC timer
        tcpipSignalHandle wscSignalHandle = _TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_WSC_Task, TCPIP_WSC_TASK_RATE);
        if(wscSignalHandle == NULL)
        {   // cannot create the WSC timer
            initFail = true;
            break;
        }
        wscModDcpt.wscSignalHandle = wscSignalHandle;

        if(!UserGblLockCreate(&wscModDcpt))
        {   // failed to create the semaphore
            initFail = true;
            break;
        }
        // initialize all connections

        pWscCon = wscConnCtrl + 0;
        for(connIx = 0; connIx < nConns; connIx++)
        {
            pWscCon->parent = &wscModDcpt;
            pWscCon->netSocket = NET_PRES_INVALID_SOCKET;
            pWscCon->connIx = connIx;
#if (M_WSC_SEM_LOCK != 0) && (M_WSC_SEM_PER_CONNECTION != 0) 
            OSAL_RESULT res = OSAL_SEM_Create(&pWscCon->connSem, OSAL_SEM_TYPE_BINARY, 1, 1);
            if(res == OSAL_RESULT_SUCCESS)
            {
                pWscCon->connSemValid = 1U;
            }
            else
            {
                initFail = true;
                break;
            }
#endif  // (M_WSC_SEM_LOCK != 0) && (M_WSC_SEM_PER_CONNECTION != 0) 
            pWscCon++;
        }

        if(initFail)
        {
            break;
        }
        // make sure the sockets have a minimum buffer size
        uint16_t sktBuffSize = wscInitData->sktTxBuffSize == 0U ? (uint16_t)TCPIP_TCP_SOCKET_DEFAULT_TX_SIZE : wscInitData->sktTxBuffSize;
        if(sktBuffSize < WSC_MIN_SOCKET_BUFF_SIZE)
        {
            sktBuffSize = WSC_MIN_SOCKET_BUFF_SIZE;
        } 
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SKT_SIZE) != 0)
        wscModDcpt.sktTxBuffSize = (uint16_t)dbgSktTxSize;
#else
        wscModDcpt.sktTxBuffSize = sktBuffSize;
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SKT_SIZE) != 0)

        sktBuffSize = wscInitData->sktRxBuffSize == 0U ? (uint16_t)TCPIP_TCP_SOCKET_DEFAULT_RX_SIZE : wscInitData->sktRxBuffSize;
        if(sktBuffSize < WSC_MIN_SOCKET_BUFF_SIZE)
        {
            sktBuffSize = WSC_MIN_SOCKET_BUFF_SIZE;
        } 
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SKT_SIZE) != 0)
        wscModDcpt.sktRxBuffSize = (uint16_t)dbgSktRxSize;
#else
        wscModDcpt.sktRxBuffSize = sktBuffSize;
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SKT_SIZE) != 0)


        wscModDcpt.wscConfigFlags = wscInitData->configFlags;
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SRV_TMO) != 0)
        wscModDcpt.srvTmoMs = (uint16_t)dbgSrvTmo;
#else
        wscModDcpt.srvTmoMs = wscInitData->srvTmo;
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SRV_TMO) != 0)

#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_USR_TMO) != 0)
        wscModDcpt.usrTmoMs = (uint16_t)dbgUsrTmo;
#else
        wscModDcpt.usrTmoMs = wscInitData->usrTmo;
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_USR_TMO) != 0)

        const char* rtosName = OSAL_Name(); 
        if(stricmp("BASIC", rtosName) == 0)
        {
            wscModDcpt.rtosUsed = 0;
        }
        else
        {
            wscModDcpt.rtosUsed = 1;
        }
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_RTOS) != 0)
        bool semLock = false;
        bool semConn = false;
#if (M_WSC_SEM_LOCK != 0)
        semLock = true;
#if (M_WSC_SEM_PER_CONNECTION != 0)
        semConn = true;
#endif  // (M_WSC_SEM_PER_CONNECTION != 0)
#endif  // (M_WSC_SEM_LOCK != 0)
        SYS_CONSOLE_PRINT("WSC - rtosUsed: %d, name %s, semLock: %d, semConn: %d\r\n", wscModDcpt.rtosUsed, rtosName, semLock, semConn);
#endif // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_RTOS) != 0)
        gWscDcpt = &wscModDcpt;

        break;
    }

    if(initFail)
    {
        F_WSC_Cleanup(stackCtrl);
        return false;
    }

    // allow user access
    UserGblUnlock(gWscDcpt);
    
    wscInitCount++;
    return true;    
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_WSC_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN) // interface down

    // one way or another this interface is going down

    if(wscInitCount > 0)
    {   // we're up and running
        F_WSC_AbortConnections(stackCtrl->pNetIf);

        if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
        {   // stack shut down
            if(--wscInitCount == 0)
            {   // all closed
                // release resources
                F_WSC_Cleanup(stackCtrl);
            }
        }
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)



TCPIP_WSC_CONN_HANDLE TCPIP_WSC_ConnOpen(const TCPIP_WSC_CONN_DCPT* connDcpt, TCPIP_WSC_RES* pRes)
{
    uint16_t    connIx, protoSrcIx, protoDstIx;
    TCPIP_WSC_CONN_CTRL* pWsc, *pWsConn;
    const char* protoSrc;
    TCPIP_WSC_RES res;
    TCPIP_WSC_CONN_HANDLE connH;

    connH = NULL;

    while(true)
    {
        if(gWscDcpt == NULL)
        {
            res = TCPIP_WSC_RES_MODULE_ERROR;
            break;
        }

        if(connDcpt == NULL || connDcpt->server == NULL || connDcpt->server[0] == '\0')
        {
            res = TCPIP_WSC_RES_BAD_ARG;
            break;
        }
        // because of the 'state machine' approach we store the server and resource names
        // rather than asking the user to maintain those constant!
        // sanity check
        if(strlen(connDcpt->server) >= sizeof(pWsConn->srvName) - 1U)
        {
            res = TCPIP_WSC_RES_SRV_NAME_ERROR;
            break;
        } 
        if(connDcpt->resource != NULL && strlen(connDcpt->resource) >= sizeof(pWsConn->resource) - 1U)
        {
            res = TCPIP_WSC_RES_RESOURCE_ERROR;
            break;
        } 
        if(connDcpt->protocols != NULL && connDcpt->nProtocols != 0)
        {   // check the protocols
            if(connDcpt->nProtocols > TCPIP_WSC_PROTO_MAX_NO)
            {
                res = TCPIP_WSC_RES_PROTO_ERROR;
                break;
            }

            protoSrc = connDcpt->protocols[0];
            res = TCPIP_WSC_RES_OK; 
            for(protoSrcIx = 0; protoSrcIx < connDcpt->nProtocols; protoSrcIx++) 
            {
                if(protoSrc != NULL && strlen(protoSrc) >= TCPIP_WSC_PROTO_MAX_LEN)
                {
                    res = TCPIP_WSC_RES_PROTO_ERROR;
                    break;
                }
                protoSrc++;
            }

            if(res != TCPIP_WSC_RES_OK)
            {
                break;
            }
        } 

        // find an available slot
        pWsConn = NULL;
        // lock access to the gWscDcpt->wscConnCtrl resource!!!
        UserGblLock(gWscDcpt);
        pWsc = gWscDcpt->wscConnCtrl;
        for(connIx = 0; connIx < gWscDcpt->wscConnNo; connIx++)
        {
            if(pWsc->intState == (uint16_t)WSC_INT_STAT_IDLE)
            {
                SetIntState(pWsc, WSC_INT_STAT_BUSY);
                pWsConn = pWsc;
                break;
            }
            pWsc++;
        }

        UserGblUnlock(gWscDcpt);

        if(pWsConn == NULL)
        {
            res = TCPIP_WSC_RES_CONN_UNAVAIL;
            break;
        }

        Wsc_ConnInit(pWsConn);
        pWsConn->connPort = connDcpt->port;
        pWsConn->connFlags = connDcpt->connFlags;
        if((connDcpt->connFlags & (uint16_t)TCPIP_WSC_CONN_FLAG_USE_IPV6) != 0U)
        {
            pWsConn->addType = (uint8_t)IP_ADDRESS_TYPE_IPV6;
        }
        else
        {
            pWsConn->addType = (uint8_t)IP_ADDRESS_TYPE_IPV4;
        }
        res = WSC_OpenSocket(pWsConn);

        if(res == TCPIP_WSC_RES_OK)
        {
            (void)strncpy(pWsConn->srvName, connDcpt->server, sizeof(pWsConn->srvName) - 1U);
            pWsConn->srvName[sizeof(pWsConn->srvName) - 1U] = '\0';

            pWsConn->resource[0] = '\0';
            if(connDcpt->resource != NULL)
            {
                const char* resource = connDcpt->resource;
                if(resource[0] == '/')
                {
                    resource++;
                }
                if(resource[0] != '\0')
                {
                    (void)strncpy(pWsConn->resource, resource, sizeof(pWsConn->resource) - 1U);
                    pWsConn->resource[sizeof(pWsConn->resource) - 1U] = '\0';
                }
            }

            pWsConn->nProtocols = 0;
            pWsConn->protInUseIx = -1;
            if(connDcpt->protocols != NULL && connDcpt->nProtocols != 0)
            {   // copy the protocols
                protoDstIx = 0;
                for(protoSrcIx = 0; protoSrcIx < connDcpt->nProtocols; protoSrcIx++) 
                {
                    protoSrc = connDcpt->protocols[protoSrcIx];
                    if(protoSrc != NULL && strlen(protoSrc) != 0U)
                    {
                        char* protoDst = pWsConn->protocols[protoDstIx];
                        (void)strncpy(protoDst, protoSrc, TCPIP_WSC_PROTO_MAX_LEN);
                        protoDst[TCPIP_WSC_PROTO_MAX_LEN] = '\0';
                        protoDstIx++;
                        pWsConn->nProtocols++;
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_PROTO_LIST) != 0)
                        SYS_CONSOLE_PRINT("WSC Open - proto: %d, src: %s, dst: %s\r\n", pWsConn->nProtocols, protoSrc, protoDst); 
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_PROTO_LIST) != 0)
                    }
                    protoSrc++;
                }
            } 

            // advance the connection state machine...
            SetIntState(pWsConn, WSC_INT_STAT_START_DNS);
            SetWscState(pWsConn, TCPIP_WSC_CONN_STAT_CONNECTING);

            connH = (TCPIP_WSC_CONN_HANDLE)pWsConn; 
        } 
        else
        {   // failed to open the socket...leave the connection available
            SetIntState(pWsConn, WSC_INT_STAT_IDLE);
            SetWscState(pWsConn, TCPIP_WSC_CONN_STAT_IDLE);
        }

        break;
    }

    if(pRes != NULL)
    {
        *pRes = res;
    }
    return connH;
}


TCPIP_WSC_CONN_STATUS TCPIP_WSC_ConnStatus(TCPIP_WSC_CONN_HANDLE hConn)
{
    TCPIP_WSC_CONN_CTRL* pWsc = WSC_ValidateConn(hConn);

    if(pWsc == NULL)
    {
        return TCPIP_WSC_CONN_STAT_IDLE;
    }
    return (TCPIP_WSC_CONN_STATUS)pWsc->connState;
}

const char* TCPIP_WSC_ConnProtoInUse(TCPIP_WSC_CONN_HANDLE hConn)
{
    TCPIP_WSC_CONN_CTRL* pWsc = WSC_ValidateConn(hConn);

    if(pWsc == NULL)
    {
        return NULL;
    }

    if(pWsc->connState != (uint16_t)TCPIP_WSC_CONN_STAT_OPEN || pWsc->protInUseIx < 0)
    {   // no proto in use
        return NULL;
    }

    return pWsc->protocols[pWsc->protInUseIx];
}

int16_t  TCPIP_WSC_ConnSocketGet(TCPIP_WSC_CONN_HANDLE hConn)
{
    int16_t skt = -1;
    TCPIP_WSC_CONN_CTRL* pWsc = WSC_ValidateConn(hConn);

    if(pWsc != NULL)
    {
        skt = pWsc->netSocket;
    }

    return skt;
}

TCPIP_WSC_RES TCPIP_WSC_ConnClose(TCPIP_WSC_CONN_HANDLE hConn, TCPIP_WSC_CLOSE_CODE closeCode, const char* reason)
{
    TCPIP_WSC_CONN_CTRL* pWsc = WSC_ValidateConn(hConn);

    if(pWsc == NULL)
    {
        return TCPIP_WSC_RES_BAD_CONN_HANDLE;
    }

    TCPIP_WSC_RES res;
    bool connLocked = false;
    while(true)
    {
        if(!WSC_ConnectionLock(pWsc, false))
        {
            res = TCPIP_WSC_RES_BUSY;
            break; 
        }
        connLocked = true;

        if(pWsc->connState != (uint16_t)TCPIP_WSC_CONN_STAT_OPEN)
        {
            res = TCPIP_WSC_RES_BAD_STATE;
            break;
        }

        res = WSC_SendCloseFrame(pWsc, closeCode, reason);
        if(res == TCPIP_WSC_RES_OK)
        {   // wait for server's response
            pWsc->cliCloseSent = 1;
            SetIntState(pWsc, WSC_INT_STAT_CLOSING);
            SetWscState(pWsc, TCPIP_WSC_CONN_STAT_CLOSING);
        }
        break;
    }

    if(connLocked)
    {
        WSC_ConnectionUnlock(pWsc, false);
    }
    return res;
}

// Note: closeCode == TCPIP_WSC_CLOSE_NONE will not send any payload!
static TCPIP_WSC_RES WSC_SendCloseFrame(TCPIP_WSC_CONN_CTRL* pWsc, TCPIP_WSC_CLOSE_CODE closeCode, const char* reason)
{
    union
    {
        uint8_t uBuff[WS_FRAME_CTRL_MAX_PAYLEN];
        char    cBuff[WS_FRAME_CTRL_MAX_PAYLEN];
    }U_CLOSE_BUFF;

    // the first two bytes of the body MUST be a 2-byte unsigned integer representing the status code
    size_t msgSize;
    uint8_t* msgBuff;

    if(closeCode != TCPIP_WSC_CLOSE_NONE)
    {   // message sent to the server
        msgSize = 2U + (reason == NULL ? 0U : strlen(reason));

        if(msgSize > WS_FRAME_CTRL_MAX_PAYLEN)
        {
            return TCPIP_WSC_RES_EXCESS_DATA; 
        }
        TCPIP_UINT16_VAL code;  // convert the closeCode
        code.Val = (uint16_t)closeCode;
        U_CLOSE_BUFF.uBuff[0] = code.v[1];  // convert to net order
        U_CLOSE_BUFF.uBuff[1] = code.v[0];  // convert to net order
        if(reason != NULL)
        {
            (void)strncpy(U_CLOSE_BUFF.cBuff + 2, reason, sizeof(U_CLOSE_BUFF.cBuff) - 2U);
        }
        msgBuff = U_CLOSE_BUFF.uBuff;
    }
    else
    {
        msgSize = 0;
        msgBuff = NULL;
    }

    // send a close frame
    return WSC_SendCtrlFrame(pWsc, (uint8_t)TCPIP_WS_CTRL_CODE_CLOSE, msgBuff, (uint16_t)msgSize);
}

// RFC notes:
// All control frames MUST have a payload length of 125 bytes or less and MUST NOT be fragmented.
//
// sends a ping to the server
size_t TCPIP_WSC_ConnPing(TCPIP_WSC_CONN_HANDLE hConn, const uint8_t* pingData, size_t dataSize, TCPIP_WSC_RES* pRes)
{
    TCPIP_WSC_SEND_MSG_DCPT msgDcpt;
    msgDcpt.msgData = pingData;
    msgDcpt.msgSize = dataSize;
    // avoid calling ping again mid message if this doesn't get through!
    // That's why we set the TCPIP_WSC_MSG_FLAG_TX_WHOLE 
    msgDcpt.msgFlags = (uint8_t)TCPIP_WSC_MSG_FLAG_CTRL | (uint8_t)TCPIP_WSC_MSG_FLAG_TX_WHOLE;
    msgDcpt.ctrlCode = (uint8_t)TCPIP_WS_CTRL_CODE_PING;
    msgDcpt.msgId = 0;

    return TCPIP_WSC_MessageSend(hConn, &msgDcpt, pRes);
}

// helper for a pong control frame
size_t TCPIP_WSC_ConnPong(TCPIP_WSC_CONN_HANDLE hConn, const uint8_t* pongData, size_t dataSize, TCPIP_WSC_RES* pRes)
{
    TCPIP_WSC_SEND_MSG_DCPT msgDcpt;
    msgDcpt.msgData = pongData;
    msgDcpt.msgSize = dataSize;
    // avoid calling ping again mid message if this doesn't get through!
    // That's why we set the TCPIP_WSC_MSG_FLAG_TX_WHOLE 
    msgDcpt.msgFlags = (uint8_t)TCPIP_WSC_MSG_FLAG_CTRL | (uint8_t)TCPIP_WSC_MSG_FLAG_TX_WHOLE;
    msgDcpt.ctrlCode = (uint8_t)TCPIP_WS_CTRL_CODE_PONG;
    msgDcpt.msgId = 0;

    return TCPIP_WSC_MessageSend(hConn, &msgDcpt, pRes);
}


size_t TCPIP_WSC_MessageSend(TCPIP_WSC_CONN_HANDLE hConn, TCPIP_WSC_SEND_MSG_DCPT* msgDcpt, TCPIP_WSC_RES* pRes)
{
    union
    {
        uint32_t    u32;
        uint8_t     u8[4];
    }U_MASK; // mask, LE order

    TCPIP_WSC_RES res = TCPIP_WSC_RES_OK;
    size_t sentBytes = 0;

    uint16_t frameLen;
    size_t payloadLen;
    WSC_PEND_TX_MSG* pTxMsg;

    TCPIP_WSC_CONN_CTRL* pWsc = NULL;
    bool connLocked = false;
    while(true)
    {
        if(msgDcpt == NULL)
        {
            res = TCPIP_WSC_RES_BAD_ARG;
            break;  
        }

        pWsc = WSC_ValidateConn(hConn);

        if(pWsc == NULL)
        {
            res = TCPIP_WSC_RES_BAD_CONN_HANDLE;
            break;
        }

        if(!WSC_ConnectionLock(pWsc, false))
        {
            res = TCPIP_WSC_RES_BUSY;
            break; 
        }
        connLocked = true;

        if(pWsc->connState != (uint16_t)TCPIP_WSC_CONN_STAT_OPEN)
        {
            res = TCPIP_WSC_RES_BAD_STATE;
            break;
        }

        _TCPIPStack_Assert(pWsc->intState == (uint16_t)WSC_INT_STAT_OPEN, __FILE__, __func__, __LINE__);

        // sanity check
        pTxMsg = &pWsc->pendTxMsg;
        if(msgDcpt->msgId != pTxMsg->msgId)
        {
            res = TCPIP_WSC_RES_BAD_MSG_ID;
            break;
        }

        if(pTxMsg->msgId == 0U)
        {   // brand new message
            if(msgDcpt->msgData != NULL && msgDcpt->msgSize > WS_FRAME_MEDIUM_MAX_EXT_PAYLEN)
            {   // no support for large frames
                res = TCPIP_WSC_RES_NOT_IMPLEMENTED;
                break;
            } 

            if((msgDcpt->msgFlags & (uint8_t)TCPIP_WSC_MSG_FLAG_FRAG_MID) != 0U && (msgDcpt->msgFlags & ((uint8_t)TCPIP_WSC_MSG_FLAG_FRAG_START | (uint16_t)TCPIP_WSC_MSG_FLAG_FRAG_END)) != 0U)
            {   // mid fragment  cannot be set with start/end
                res = TCPIP_WSC_RES_BAD_ARG;
                break;
            }

            bool isFragment = 
                (msgDcpt->msgFlags & (uint8_t)TCPIP_WSC_MSG_FLAG_FRAG_MID) != 0U ||
                ((msgDcpt->msgFlags & ((uint8_t)TCPIP_WSC_MSG_FLAG_FRAG_START | (uint16_t)TCPIP_WSC_MSG_FLAG_FRAG_END)) != 0U &&
                 (msgDcpt->msgFlags & ((uint8_t)TCPIP_WSC_MSG_FLAG_FRAG_START | (uint16_t)TCPIP_WSC_MSG_FLAG_FRAG_END)) != ((uint16_t)TCPIP_WSC_MSG_FLAG_FRAG_START | (uint16_t)TCPIP_WSC_MSG_FLAG_FRAG_END));


#if (M_WSC_FRAGMENT_SUPPORT == 0)
            if(isFragment)
            {   // currently fragmentation not supported
                res = TCPIP_WSC_RES_NOT_IMPLEMENTED;
                break;
            }
#else
            if((isFragment && msgDcpt->msgFlags & (uint8_t)TCPIP_WSC_MSG_FLAG_CTRL) != 0U)
            {   // control message cannot be fragmented!
                res = TCPIP_WSC_RES_BAD_ARG;
                break;
            }
#endif  // (M_WSC_FRAGMENT_SUPPORT == 0)

            payloadLen = msgDcpt->msgData == NULL ? 0UL : msgDcpt->msgSize;

            // basic sanity check about the message and data size:
            if((msgDcpt->msgFlags & (uint8_t)TCPIP_WSC_MSG_FLAG_CTRL) != 0U)
            {
                if(payloadLen > (size_t)WS_FRAME_CTRL_MAX_PAYLEN)
                {
                    res = TCPIP_WSC_RES_EXCESS_DATA; 
                    break;
                }

                frameLen = (uint16_t)sizeof(WS_CLIENT_FRAME_CTRL);
            }
            else
            {   // data frame
                frameLen = payloadLen > (size_t)WS_FRAME_SMALL_MAX_PAYLEN ? (uint16_t)sizeof(WS_CLIENT_FRAME_MEDIUM) : (uint16_t)sizeof(WS_FRAME_SMALL_MAX_PAYLEN);
            }

            uint16_t avlblWrLen = NET_PRES_SocketWriteIsReady(pWsc->netSocket, frameLen, 0U);
            if(avlblWrLen < frameLen)
            {   // we should at least have space to write the header!
                // leave the msgId = 0 so we do all this again next time!
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_FAIL_WRITE_HDR) != 0)
                SYS_CONSOLE_PRINT("WSC Send - Failed write header len: %d\r\n", frameLen); 
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_FAIL_WRITE_HDR) != 0)
                res = TCPIP_WSC_RES_MORE_DATA; 
                break;
            }

            // finally, check the message size and that the socket can handle it

            if((msgDcpt->msgFlags & (uint8_t)TCPIP_WSC_MSG_FLAG_TX_WHOLE)!= 0U)
            {   // request to to send all in one message
                size_t totLen = (size_t)frameLen + payloadLen;
                if(totLen > (size_t)pWsc->parent->sktTxBuffSize)
                {   // exceed the socket largest possible size
                    res = TCPIP_WSC_RES_MSG_TOO_LARGE;
                    break;
                }

                // we do not exceed the buffer capacity; check for enough space
                avlblWrLen = NET_PRES_SocketWriteIsReady(pWsc->netSocket, (uint16_t)totLen, 0U);
                if(avlblWrLen < (uint16_t)totLen)
                {   // not enough space; retry
                    res = TCPIP_WSC_RES_NO_WHOLE_SPACE;
                    break;
                }
            }

            // looks like we can start sending the message
            pTxMsg->msgData = msgDcpt->msgData;
            pTxMsg->msgSize = payloadLen;
            pTxMsg->msgFlags = msgDcpt->msgFlags;
            pTxMsg->ctrlCode = msgDcpt->ctrlCode;
            pTxMsg->sentBytes = 0;
            // generate the message mask
            U_MASK.u32 = 0;
            (void)WSC_RngGenerate(U_MASK.u8, sizeof(U_MASK.u8));
            (void)memcpy(pTxMsg->maskKey, U_MASK.u8, sizeof(U_MASK));
            pTxMsg->maskIx = 0;
        }

        size_t txLeftBytes = pTxMsg->msgSize - pTxMsg->sentBytes; 
        if(txLeftBytes > (size_t)pWsc->parent->sktTxBuffSize)
        {   // no sense to exceed the buffer size; also keep it to 16 bits
            pTxMsg->toSendBytes = pWsc->parent->sktTxBuffSize;
        }
        else
        {
            pTxMsg->toSendBytes = (uint16_t)txLeftBytes;
        }

        // send the message
        sentBytes = WSC_SendMsgFrame(pWsc, pTxMsg, &res);

        bool msgDone = false;
        if(res == TCPIP_WSC_RES_OK) 
        {
            // adjust the buffers to be sent
            pTxMsg->sentBytes += sentBytes;

            if(pTxMsg->sentBytes == pTxMsg->msgSize)
            {   // all done
                msgDone = true;
            }
            else
            {   // more data to be sent needed
                if(pTxMsg->msgSize != 0U)
                {   // adjust the buffer for next send operation
                    pTxMsg->msgData += sentBytes;
                }

                if(pTxMsg->msgId == 0U)
                {   // need to generate a proper message ID
                    U_MASK.u32 = 0;
                    (void)WSC_RngGenerate(U_MASK.u8, sizeof(U_MASK.u8));
                    pTxMsg->msgId = (uint16_t)U_MASK.u32;
                    msgDcpt->msgId = (uint16_t)U_MASK.u32;
                }
                res = TCPIP_WSC_RES_MORE_DATA; 
            }
        }
        else
        {   // else some error has occurred
            msgDone = true;
        }

        if(msgDone)
        {
            pTxMsg->msgId = 0;
            msgDcpt->msgId = 0;
        }

        break;
    }

    if(connLocked)
    {
        WSC_ConnectionUnlock(pWsc, false);
    }

    if(pRes != NULL)
    {
        *pRes = res;
    }

    return sentBytes;

}

TCPIP_WSC_EV_HANDLE TCPIP_WSC_HandlerRegister(TCPIP_WSC_CONN_HANDLE hConn, TCPIP_WSC_EVENT_HANDLER handler, const void* hParam)
{
    TCPIP_WSC_EV_HANDLE evHandle = NULL;
    if(hConn != NULL)
    {   // registration per connection
        TCPIP_WSC_CONN_CTRL* pWsc = WSC_ValidateConn(hConn);
        if(pWsc != NULL)
        {   // valid connection handle
            if(WSC_ConnectionLock(pWsc, false))
            {
                if(pWsc->evHandler == NULL)
                {   // OK, none already registered.
                    pWsc->evHandler = handler;
                    pWsc->hParam = hParam;
                    evHandle = FC_EvHndler2Handle(handler); 
                }
                WSC_ConnectionUnlock(pWsc, false);
            }
        }
    }
    else if(gWscDcpt != NULL)
    {   // global one
        UserGblLock(gWscDcpt);
        if(gWscDcpt->evHandler == NULL)
        {
            gWscDcpt->evHandler = handler;
            gWscDcpt->hParam = hParam;
            evHandle = FC_EvHndler2Handle(handler);
        }
        UserGblUnlock(gWscDcpt);
    }
    else
    {
        // do nothing
    }
    return evHandle;
}

bool TCPIP_WSC_HandlerDeRegister(TCPIP_WSC_CONN_HANDLE hConn, TCPIP_WSC_EV_HANDLE hEvent)
{
    bool evRes = false;
    if(hConn != NULL)
    {   // de-registration per connection
        TCPIP_WSC_CONN_CTRL* pWsc = WSC_ValidateConn(hConn);
        if(pWsc != NULL)
        {   // valid connection handle
            if(WSC_ConnectionLock(pWsc, false))
            {
                if(pWsc->evHandler == hEvent)
                {   // OK, match
                    pWsc->evHandler = NULL;
                    pWsc->hParam = NULL;;
                    evRes = true; 
                }
                WSC_ConnectionUnlock(pWsc, false);
            }
        }
    }
    else if(gWscDcpt != NULL)
    {   // de-register the global handler
        UserGblLock(gWscDcpt);
        if(gWscDcpt->evHandler == hEvent)
        {   // OK, match
            gWscDcpt->evHandler = NULL;
            gWscDcpt->hParam = NULL;
            evRes = true; 
        }
        UserGblUnlock(gWscDcpt);
    }
    else
    {
        // do nothing
    }
    return evRes;
}

size_t TCPIP_WSC_MessageRead(TCPIP_WSC_CONN_HANDLE hConn, const void* msgHandle, uint8_t* pBuffer, size_t bufferSize, TCPIP_WSC_RES* pRes)
{
    TCPIP_WSC_RES res;
    size_t readBytes = 0;

    TCPIP_WSC_CONN_CTRL* pWsc = NULL;
    bool connLocked = false;
    while(true)
    {
        if(bufferSize == 0U)
        {   // simply discard
            pBuffer = NULL;
        }

        pWsc = WSC_ValidateConn(hConn);
        if(pWsc == NULL)
        {
            res = TCPIP_WSC_RES_BAD_CONN_HANDLE;
            break;
        }
        if(!WSC_ConnectionLock(pWsc, false))
        {
            res = TCPIP_WSC_RES_BUSY;
            break; 
        }
        connLocked = true;

        if(pWsc->connState != (uint16_t)TCPIP_WSC_CONN_STAT_OPEN)
        {
            res = TCPIP_WSC_RES_BAD_STATE;
            break;
        }

        // the current pending message descriptor
        TCPIP_WSC_PEND_MSG_DCPT* pendDcpt = &pWsc->pendRxMsg;

        if(pendDcpt->msgHandle == NULL)
        {   // no message pending
            res = TCPIP_WSC_RES_NO_MSG;
            break;
        }
        _TCPIPStack_Assert(pendDcpt->info.frameType != (uint8_t)TCPIP_WS_FRAME_TYPE_NONE, __FILE__, __func__, __LINE__);

        if(msgHandle != NULL && msgHandle != pendDcpt->msgHandle)
        {
            res = TCPIP_WSC_RES_BAD_MSG_HANDLE;
            break;
        }

        // user performing a read - restart the user read frequency timer
        WSC_StartUsrWaitTimer(pWsc);

        (void)WSC_SrvRxUpdate(pWsc, pendDcpt);
        uint16_t usrLeftBytes = pendDcpt->payloadLen - pendDcpt->renderedLen;

        if(pendDcpt->payloadLen != 0U)
        {   // should not be 0, as it should have been cleared by user read!
            _TCPIPStack_Assert(usrLeftBytes != 0U, __FILE__, __func__, __LINE__);
        }

        if(usrLeftBytes > pendDcpt->sktPendLen)
        {   // not more than currently available
            usrLeftBytes = pendDcpt->sktPendLen;
        }

        if(pBuffer != NULL)
        {
            usrLeftBytes = ((size_t)usrLeftBytes > bufferSize) ? (uint16_t)bufferSize : usrLeftBytes;
        }
        // else discard whatever available

        uint16_t wasRead = 0;
        if(usrLeftBytes != 0U)
        {
            // just in case the server was locked (buffer full), restart its timer
            WSC_StartSrvWaitTimer(pWsc);
            wasRead = NET_PRES_SocketRead(pWsc->netSocket, pBuffer, usrLeftBytes);
            pendDcpt->sktPendLen -= wasRead;
        }

        // update the pending message info
        pendDcpt->renderedLen += wasRead;
        if(pendDcpt->renderedLen == pendDcpt->payloadLen)
        {   // completed
            res = TCPIP_WSC_RES_OK;
            pendDcpt->info.frameType = (uint8_t)TCPIP_WS_FRAME_TYPE_NONE;
            pendDcpt->msgHandle = NULL;    // no longer valid
        }
        else
        {   // more to come
            res = TCPIP_WSC_RES_MORE_DATA;
        }
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SRV_UPDATE) != 0)
        if(wasRead != 0U)
        {
            SYS_CONSOLE_PRINT("WSC Read Update - wasRead: %d, payloadLen: %d, srvAvlblLen: %d, sktPendLen: %d, renderedLen: %d\r\n", wasRead, pendDcpt->payloadLen, pendDcpt->srvAvlblLen, pendDcpt->sktPendLen, pendDcpt->renderedLen);
        }
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SRV_UPDATE) != 0)

        readBytes = (size_t)wasRead;
        break;
    }

    if(connLocked)
    {
        WSC_ConnectionUnlock(pWsc, false);
    }

    if(pRes != NULL)
    {
        *pRes = res;
    }
    return readBytes;
    
}

TCPIP_WSC_RES TCPIP_WSC_MessageInfo(TCPIP_WSC_CONN_HANDLE hConn, const void* msgHandle, TCPIP_WSC_PEND_MSG_DCPT* msgDcpt)
{
    TCPIP_WSC_CONN_CTRL* pWsc = WSC_ValidateConn(hConn);
    union
    {
        TCPIP_WSC_PEND_MSG_DCPT dcpt;
        uint8_t dcptBuff[sizeof(TCPIP_WSC_PEND_MSG_DCPT)];
    }pendDcpt1, pendDcpt2;  // read the current pending message descriptor

    if(pWsc == NULL)
    {
        return TCPIP_WSC_RES_BAD_CONN_HANDLE;
    }

    if(pWsc->connState != (uint16_t)TCPIP_WSC_CONN_STAT_OPEN)
    {
        return TCPIP_WSC_RES_BAD_STATE;
    }

    // read only: try to get a consistent state without locking
    do
    {
        pendDcpt1.dcpt = pWsc->pendRxMsg;
        pendDcpt2.dcpt = pWsc->pendRxMsg;
    } while(memcmp(pendDcpt1.dcptBuff, pendDcpt2.dcptBuff, sizeof(pendDcpt1)) != 0);

    // got a consistent reading
    if(pendDcpt1.dcpt.msgHandle == NULL)
    {   // no message pending
        return TCPIP_WSC_RES_NO_MSG;
    }

    _TCPIPStack_Assert(pendDcpt1.dcpt.info.frameType != (uint8_t)TCPIP_WS_FRAME_TYPE_NONE, __FILE__, __func__, __LINE__);
    // should not be done, as it should have been cleared by user read!
    _TCPIPStack_Assert(pendDcpt1.dcpt.renderedLen < pendDcpt1.dcpt.payloadLen, __FILE__, __func__, __LINE__);
    
    if(msgHandle != NULL && msgHandle != pendDcpt1.dcpt.msgHandle)
    {
        return TCPIP_WSC_RES_BAD_MSG_HANDLE;
    }

    if(msgDcpt != NULL)
    {   // update the info
        *msgDcpt = pendDcpt1.dcpt;
    }
    // make sure state hasn't changed
    if(pWsc->connState != (uint16_t)TCPIP_WSC_CONN_STAT_OPEN)
    {
        return TCPIP_WSC_RES_BAD_STATE;
    }

    return TCPIP_WSC_RES_OK;
}

// Internal functionality   

#if (TCPIP_STACK_DOWN_OPERATION != 0)
// if pNetIf == 0, all connections are closed, stack is going down
static void F_WSC_AbortConnections(TCPIP_NET_IF* pNetIf)
{
    TCPIP_WSC_CONN_CTRL* pWscCon;
    size_t  connIx;
    TCP_SOCKET_INFO sktInfo;
    bool closeSkt;

    pWscCon = gWscDcpt->wscConnCtrl;
    for (connIx = 0; connIx < gWscDcpt->wscConnNo; connIx++)
    {
        // Close the connections that were associated with this interface
        if (pWscCon->netSocket != NET_PRES_INVALID_SOCKET)
        {
            if(pNetIf == NULL)
            {
                closeSkt = true;
            }
            else
            {
                (void)NET_PRES_SocketInfoGet(pWscCon->netSocket, &sktInfo);
                closeSkt = pNetIf == sktInfo.hNet;
            }

            if(closeSkt)
            {
                WSC_CloseConnection(pWscCon, TCPIP_WSC_RES_STACK_DOWN);
            }
        }
        pWscCon++;
    }
}

static void F_WSC_Cleanup(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    if(gWscDcpt != NULL)
    {
        if(gWscDcpt->wscConnCtrl != NULL)
        {
            F_WSC_AbortConnections(NULL);
        }

#if (M_WSC_SEM_LOCK != 0) && (M_WSC_SEM_PER_CONNECTION != 0) 
        size_t  connIx;
        TCPIP_WSC_CONN_CTRL* pWsc = gWscDcpt->wscConnCtrl;
        for (connIx = 0; connIx < gWscDcpt->wscConnNo; connIx++)
        {
            if(pWsc->connSemValid != 0U)
            {
                (void)OSAL_SEM_Delete(&pWsc->connSem);
                pWsc->connSemValid = 0;
            }
            pWsc++;
        }
#endif  // (M_WSC_SEM_LOCK != 0) && (M_WSC_SEM_PER_CONNECTION != 0) 

        _TCPIPStack_Assert(gWscDcpt->wscMemH == stackCtrl->memH, __FILE__, __func__, __LINE__);

        if(gWscDcpt->wscConnCtrl != NULL)
        {
            (void)TCPIP_HEAP_Free(gWscDcpt->wscMemH, gWscDcpt->wscConnCtrl);
            gWscDcpt->wscConnCtrl = NULL;
        }

        if(gWscDcpt->wscSignalHandle != NULL)
        {
            _TCPIPStackSignalHandlerDeregister(gWscDcpt->wscSignalHandle);
            gWscDcpt->wscSignalHandle = NULL;
        }

        if(gWscDcpt->gblSemValid != 0U)
        {
            UserGblLockDelete(gWscDcpt);
        }
        gWscDcpt = NULL;
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)


// opens a net socket and configures it
// it updates the pCon->netSocket
// if error, the socket is deleted!
// the pConn->parent should be set!
// the pConn->connPort, connFlags should be set!
static TCPIP_WSC_RES WSC_OpenSocket(TCPIP_WSC_CONN_CTRL* pConn)
{
    TCPIP_WSC_RES res;
    NET_PRES_SKT_T  sktType;
    NET_PRES_SKT_HANDLE_T netSkt;
    TCPIP_WSC_MODULE_DCPT*  pDcpt = pConn->parent;


    if((pConn->connFlags & (uint16_t)TCPIP_WSC_CONN_FLAG_SECURE_ON) != 0U)
    {   // encrypted
        sktType = (NET_PRES_SKT_T)((uint16_t)NET_PRES_SKT_STREAM | (uint16_t)NET_PRES_SKT_CLIENT | (uint16_t)NET_PRES_SKT_ENCRYPTED);
    }
    else if((pConn->connFlags & (uint16_t)TCPIP_WSC_CONN_FLAG_SECURE_OFF) != 0U)
    {   // unencrypted
        sktType = (NET_PRES_SKT_T)((uint16_t)NET_PRES_SKT_STREAM | (uint16_t)NET_PRES_SKT_CLIENT | (uint16_t)NET_PRES_SKT_UNENCRYPTED);
    }
    else
    {   // use default port number
        sktType = (NET_PRES_SKT_T)((uint16_t)NET_PRES_SKT_STREAM | (uint16_t)NET_PRES_SKT_CLIENT);
    }

    res = TCPIP_WSC_RES_OK;
    while(true)
    {
        netSkt =  NET_PRES_SocketOpen(0, sktType, (NET_PRES_SKT_ADDR_T)pConn->addType, pConn->connPort, NULL, NULL);

        if(netSkt == NET_PRES_INVALID_SOCKET)
        {   // failed to open the socket
            res = TCPIP_WSC_RES_SKT_UNAVAIL; 
            break;
        }

        // attach a signal to our socket
        NET_PRES_SIGNAL_HANDLE socketSignal = NET_PRES_SocketSignalHandlerRegister(netSkt, WSC_RX_SIGNALS, &WSC_RxSignalHandler, pConn);
        if(socketSignal == NULL)
        {   // failed
            res = TCPIP_WSC_RES_SKT_SIG_ERROR; 
            break;
        }
        
        // success
        // set socket options
        if((pConn->connFlags & (uint16_t)TCPIP_WSC_CONN_FLAG_NO_DELAY) != 0U)
        {
            void* tcpForceFlush = (void*)(1UL);
            (void)NET_PRES_SocketOptionsSet(netSkt, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_NODELAY, tcpForceFlush);
        }

        if(pDcpt->sktTxBuffSize != (uint16_t)TCPIP_TCP_SOCKET_DEFAULT_TX_SIZE)
        {
            void* tcpBuffSize = (void*)((uint32_t)pDcpt->sktTxBuffSize);
            if(!NET_PRES_SocketOptionsSet(netSkt, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_TX_BUFF, tcpBuffSize))
            {   // just a warning....well, since this is critical for the message size, an error should be better
                res = TCPIP_WSC_RES_SKT_TX_BUFF_ERROR;
                break;
            }
        }

        if(pDcpt->sktRxBuffSize != (uint16_t)TCPIP_TCP_SOCKET_DEFAULT_RX_SIZE)
        {
            void* tcpBuffSize = (void*)((uint32_t)pDcpt->sktRxBuffSize);
            if(!NET_PRES_SocketOptionsSet(netSkt, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_RX_BUFF, tcpBuffSize))
            {   // just a warning
                res = TCPIP_WSC_RES_SKT_RX_BUFF_ERROR;
                break;
            }
        }

        break;
    }

    if(res < 0)
    {   // failed
        if(netSkt != NET_PRES_INVALID_SOCKET)
        { 
            NET_PRES_SocketClose(netSkt);
            netSkt = NET_PRES_INVALID_SOCKET;
        }
    }
    else
    {
        (void)NET_PRES_SocketWasReset(netSkt);
    }
    pConn->netSocket = netSkt;

    return res;
}

static void WSC_RxSignalHandler(NET_PRES_SKT_HANDLE_T hTCP, NET_PRES_SIGNAL_HANDLE hNet, uint16_t sigType, const void* param)
{
    TCPIP_WSC_CONN_CTRL* pWsc = WSC_ValidateConn(param);
    if(pWsc != NULL)
    {
        bool doAbort = false;
        TCPIP_WSC_EVENT_TYPE evType = WSC_EVENT_NONE; 

        switch(sigType)
        {
            case (uint16_t)TCPIP_TCP_SIGNAL_ESTABLISHED:
                evType = WSC_EVENT_RAW_ESTABLISHED;
                break;

            case (uint16_t)TCPIP_TCP_SIGNAL_RX_DATA:
                evType = WSC_EVENT_RAW_RX_DATA;
                break;

            case (uint16_t)TCPIP_TCP_SIGNAL_RX_FIN:
                evType = WSC_EVENT_RAW_RX_FIN;
                doAbort = true;
                break;

            case (uint16_t)TCPIP_TCP_SIGNAL_RX_RST:
                evType = WSC_EVENT_RAW_RX_RST;
                doAbort = true;
                break;

            default:
                evType = WSC_EVENT_RAW_UNKNOWN;
                break;
        }
        TCPIP_WSC_EV_INFO evInfo;
        evInfo.evPtr = NULL;
        (void)WSC_Notify_Event(pWsc, evType, evInfo);

        if(doAbort)
        {
            WSC_AbortConnection(pWsc, TCPIP_WSC_RES_SRV_DISCONNECT);
        }
    }
}

// helper to set the connection in the open state
static void WSC_OpenConnection(TCPIP_WSC_CONN_CTRL* pWsc)
{
    SetIntState(pWsc, WSC_INT_STAT_OPEN);
    SetWscState(pWsc, TCPIP_WSC_CONN_STAT_OPEN);

    // send the open event
    TCP_SOCKET_INFO tcpInfo;
    TCPIP_WSC_OPEN_INFO openInfo;
    (void)NET_PRES_SocketInfoGet(pWsc->netSocket, &tcpInfo);

    openInfo.srvAddress = tcpInfo.remoteIPaddress;
    openInfo.hNet = tcpInfo.hNet;
    openInfo.srvPort = tcpInfo.remotePort;
    openInfo.ipType = (uint8_t)tcpInfo.addressType;

    TCPIP_WSC_EV_INFO evInfo;
    evInfo.openInfo = &openInfo;
    (void)WSC_Notify_Event(pWsc, WSC_EVENT_OPEN, evInfo);
}

// implements the '_Close the WS Connection_'
static void WSC_CloseConnection(TCPIP_WSC_CONN_CTRL* pWsc, TCPIP_WSC_RES errRes)
{
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_FAIL_CONN) != 0)
    SYS_CONSOLE_PRINT("WSC CloseConnection - err: %d\r\n", errRes);
#endif // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_FAIL_CONN) != 0)
    if(pWsc->socketSignal != NULL)
    {
        (void)NET_PRES_SocketSignalHandlerDeregister(pWsc->netSocket, pWsc->socketSignal);
    }
    NET_PRES_SocketClose(pWsc->netSocket);
    pWsc->netSocket = NET_PRES_INVALID_SOCKET;
    pWsc->isConnected = 0;
    SetIntState(pWsc, WSC_INT_STAT_IDLE);
    SetWscState(pWsc, TCPIP_WSC_CONN_STAT_IDLE);
}

// implements the '_Fail the WebSocket Connection_'
static void WSC_FailConnection(TCPIP_WSC_CONN_CTRL* pWsc, TCPIP_WSC_RES errRes)
{

#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_FAIL_CONN) != 0)
    SYS_CONSOLE_PRINT("WSC FailConnection - err: %d\r\n", errRes);
#endif // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_FAIL_CONN) != 0)
    if(pWsc->connState == (uint16_t)TCPIP_WSC_CONN_STAT_OPEN)
    {   // try to send a close frame
        TCPIP_WSC_CLOSE_CODE closeCode = (errRes == TCPIP_WSC_RES_MSG_TOO_LARGE) ? TCPIP_WSC_CLOSE_DATA_SIZE_ERR : TCPIP_WSC_CLOSE_PROTO_ERR;
        (void) WSC_SendCloseFrame(pWsc, closeCode, NULL);
    }

    WSC_AbortConnection(pWsc, errRes);
}

// aborts a connection
static void WSC_AbortConnection(TCPIP_WSC_CONN_CTRL* pWsc, TCPIP_WSC_RES errRes)
{
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_FAIL_CONN) != 0)
    SYS_CONSOLE_PRINT("WSC AbortConnection - err: %d\r\n", errRes);
#endif // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_FAIL_CONN) != 0)
    TCPIP_WSC_FAIL_INFO failInfo;
    failInfo.failRes = errRes;
    TCPIP_WSC_EV_INFO evInfo;
    evInfo.failInfo = &failInfo;
    (void)WSC_Notify_Event(pWsc, WSC_EVENT_FAIL_CONN, evInfo);
    WSC_CloseConnection(pWsc, errRes);
    SetIntState(pWsc, WSC_INT_STAT_IDLE);
    SetWscState(pWsc, TCPIP_WSC_CONN_STAT_IDLE);
}


typedef void (*WSC_TASK_FUNC)(TCPIP_WSC_CONN_CTRL* pWsc);

static WSC_TASK_FUNC wsc_task_tbl[] = 
{
    &WSC_Task_Idle,             // WSC_INT_STAT_IDLE
    &WSC_Task_Idle,             // WSC_INT_STAT_BUSY
    &WSC_Task_StartDns,         // WSC_INT_STAT_START_DNS
    &WSC_Task_WaitDns,          // WSC_INT_STAT_WAIT_DNS
    &WSC_Task_StartConnect,     // WSC_INT_STAT_START_CONNECT
    &WSC_Task_WaitConnect,      // WSC_INT_STAT_WAIT_CONNECT
    &WSC_Task_WaitTlsConnect,   // WSC_INT_STAT_WAIT_TLS_CONNECT
    &WSC_Task_StartHandshake,   // WSC_INT_STAT_START_HSHAKE
    &WSC_Task_WaitHandshake,    // WSC_INT_STAT_WAIT_HSHAKE
    &WSC_Task_Process,          // WSC_INT_STAT_OPEN
    &WSC_Task_Process,          // WSC_INT_STAT_CLOSING
    &WSC_Task_Closed,           // WSC_INT_STAT_CLOSED

};

// processing task

void  TCPIP_WSC_Task(void)
{
    uint16_t connIx;
    TCPIP_WSC_CONN_CTRL* pWsc;

    if(gWscDcpt != NULL)
    {
        pWsc = gWscDcpt->wscConnCtrl;
        for(connIx = 0; connIx < gWscDcpt->wscConnNo; connIx++)
        {
            if(pWsc->isConnected != 0U)
            {
                if(NET_PRES_SocketWasReset(pWsc->netSocket) || NET_PRES_SocketWasDisconnected(pWsc->netSocket))
                {
                    WSC_FailConnection(pWsc, TCPIP_WSC_RES_SRV_DISCONNECT);
                }
            }
            // process the internal state
            wsc_task_tbl[pWsc->intState](pWsc);
            pWsc++;
        }
    }

}

// WSC_INT_STAT_IDLE
// WSC_INT_STAT_BUSY
static void WSC_Task_Idle(TCPIP_WSC_CONN_CTRL* pWsc)
{
    // nothing to be done here
}

// WSC_INT_STAT_START_DNS
static void WSC_Task_StartDns(TCPIP_WSC_CONN_CTRL* pWsc)
{
    TCPIP_DNS_RESOLVE_TYPE resType;

    if(pWsc->addType == (uint8_t)IP_ADDRESS_TYPE_IPV6)
    {
        resType = TCPIP_DNS_TYPE_AAAA;
    }
    else
    {
        resType = TCPIP_DNS_TYPE_A;
    }
    TCPIP_DNS_RESULT dnsRes = TCPIP_DNS_Resolve(pWsc->srvName, resType);
    
    if((int)dnsRes < 0)
    {
        WSC_FailConnection(pWsc, TCPIP_WSC_RES_DNS_ERROR);
    }
    else
    {   // ok, move on
        SetIntState(pWsc, WSC_INT_STAT_WAIT_DNS);
    }
    
}

// WSC_INT_STAT_WAIT_DNS
static void WSC_Task_WaitDns(TCPIP_WSC_CONN_CTRL* pWsc)
{
    uint16_t    connIx;
    TCPIP_WSC_CONN_CTRL* pWx;
    IP_MULTI_ADDRESS serverIP;

    TCPIP_DNS_RESULT dnsRes = TCPIP_DNS_IsResolved(pWsc->srvName, &serverIP, (IP_ADDRESS_TYPE)pWsc->addType);
    if((int)dnsRes < 0)
    {
        WSC_FailConnection(pWsc, TCPIP_WSC_RES_DNS_ERROR);
        return;
    } 
    else if(dnsRes == TCPIP_DNS_RES_PENDING)
    {   // not ready, wait some more
        return;
    }
    else
    {
        // do nothing
    }

    // DNS done; advance
    // check for another conn in 'connecting' state with the same server IP address...
    pWx = pWsc->parent->wscConnCtrl;
    for(connIx = 0; connIx < pWsc->parent->wscConnNo; connIx++)
    {
        if(pWx != pWsc)
        {
            if(pWx->connState == (uint16_t)TCPIP_WSC_CONN_STAT_CONNECTING && pWx->connPort == pWsc->connPort)
            {   // only one conn to the same (host, port) allowed in the 'connecting' state!
                if(WSC_ConnMatch(pWsc, pWx))
                {
                    WSC_FailConnection(pWsc, TCPIP_WSC_RES_CONNECTING);
                    return;
                }
            }
        }
    }

    // all good
    pWsc->serverIP = serverIP;
    SetIntState(pWsc, WSC_INT_STAT_START_CONNECT); 
}

// WSC_INT_STAT_START_CONNECT
static void WSC_Task_StartConnect(TCPIP_WSC_CONN_CTRL* pWsc)
{
    NET_PRES_SKT_HANDLE_T skt = pWsc->netSocket;

    if(!NET_PRES_SocketRemoteBind(skt, (NET_PRES_SKT_ADDR_T)pWsc->addType,  pWsc->connPort, &pWsc->presServerIP))
    {   // failed
        WSC_FailConnection(pWsc, TCPIP_WSC_RES_SKT_BIND_ERROR);
        return;
    }

    // clear the disconnect condition
    (void)NET_PRES_SocketWasReset(skt);    
    if(!NET_PRES_SocketConnect(skt))
    {
        WSC_FailConnection(pWsc, TCPIP_WSC_RES_SKT_CONNECT_ERROR);
        return;
    }

    WSC_StartSrvWaitTimer(pWsc);
    pWsc->isConnected = 1U; // so that the error is reported...
    SetIntState(pWsc, WSC_INT_STAT_WAIT_CONNECT); 
}

// WSC_INT_STAT_WAIT_CONNECT
static void WSC_Task_WaitConnect(TCPIP_WSC_CONN_CTRL* pWsc)
{
    NET_PRES_SKT_HANDLE_T skt = pWsc->netSocket;

    if(NET_PRES_SocketIsConnected(skt))
    {   // success
        bool forceSecure = (pWsc->connFlags & TCPIP_WSC_CONN_FLAG_SECURE_ON) != 0U;
        bool portSecure = TCPIP_Helper_TCPSecurePortGet(pWsc->connPort);
        if(forceSecure || portSecure)
        {   // secure connection
            if(!portSecure)
            {   // forced secure connection
                if(!NET_PRES_SocketEncryptSocket(skt))
                {   // TLS failure
                    WSC_FailConnection(pWsc, TCPIP_WSC_RES_TLS_ERROR);
                    return;
                }
            }
            // all good
            WSC_StartSrvWaitTimer(pWsc);
            SetIntState(pWsc, WSC_INT_STAT_WAIT_TLS_CONNECT); 
        }
        else
        {   // unencrypted socket
            SetIntState(pWsc, WSC_INT_STAT_START_HSHAKE); 
        }
        return;
    }

    // not yet connected; check for timeout
    if(WSC_IsSrvTmo(pWsc))
    {   // timeout
        WSC_FailConnection(pWsc, TCPIP_WSC_RES_SKT_CONNECT_TMO);
    }
    // else wait some more...
}

// WSC_INT_STAT_WAIT_TLS_CONNECT
static void WSC_Task_WaitTlsConnect(TCPIP_WSC_CONN_CTRL* pWsc)
{
    NET_PRES_SKT_HANDLE_T skt = pWsc->netSocket;

    if(NET_PRES_SocketIsNegotiatingEncryption(skt))
    {   // still negotiating...
        if(WSC_IsSrvTmo(pWsc))
        {   // negotiation timeout; fatal
            WSC_FailConnection(pWsc, TCPIP_WSC_RES_TLS_NEG_TMO);
        }
        // else // wait some more
        return;
    }

    // negotiation done
    if(!NET_PRES_SocketIsSecure(skt))
    {   // negotiation failed; fatal
        WSC_FailConnection(pWsc, TCPIP_WSC_RES_TLS_NEG_ERROR);
        return;
    }

    // TLS success; advance
    SetIntState(pWsc, WSC_INT_STAT_START_HSHAKE); 
}

// WS client headers
static const char* wsc_req_header = "GET /%s HTTP/1.1\r\n";  // GET /resource HTTP/1.1
static const char* wsc_host_header = "Host: %s:%d\r\n";  // Host: server.example.com:port
static const char* wsc_upgrade_header = "Upgrade: websocket\r\n";
static const char* wsc_connection_header = "Connection: Upgrade\r\n";
static const char* wsc_protocol_header = "Sec-WebSocket-Protocol:";
static const char* wsc_key_header = "Sec-WebSocket-Key: %s\r\n";  // Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==
static const char* wsc_version_header = "Sec-WebSocket-Version: 13\r\n";

// WS server response headers
static const char wss_status_line[] = "HTTP/1.1";    // "HTTP/1.1 101 Switching Protocols\r\n";
static const char wss_upgrade_line[] = "Upgrade:";   // "Upgrade: websocket"
static const char wss_connection_line[] = "Connection:"; // "Connection: Upgrade"
static const char wss_accept_line[] = "Sec-WebSocket-Accept:";  // "Sec-WebSocket-Accept: dGhlIHNhbXBsZSBub25jZQ==\r\n"
static const char wss_proto_line[] = "Sec-WebSocket-Protocol:";    // "Sec-WebSocket-Protocol: prot1, prot2\r\n"
static const char wss_extension_line[] = "Sec-WebSocket-Extensions:";    // "Sec-WebSocket-Extensions: ex1, ex2\r\n"

// general HTTP strings
static const char* ws_end_line = "\r\n";  // http headers line end
static const char* ws_end_header = "\r\n\r\n";  // http headers end
static const char* ws_alt_end_header = "\n\n";  // some servers end like this...
static const char* ws_list_delim = ", ";        // list words delimiter
static const char* ws_guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";    // WS GUID
static int ws_word_delim = (int)' ';                 // word delimiter

// Origin: http://example.com - optional for non web clients
// static const char wsc_origin_header = "Origin: %s"; // Origin: http://example.com

// Sec-WebSocket-Protocol: chat, superchat - optional, not supported yet
// Sec-WebSocket-Extensions:  - optional, not supported yet
        
// The request MAY include any other header fields, for example, cookies [RFC6265] and/or authentication-related header fields
// such as the |Authorization| header field [RFC2616], which are processed according to documents that define them.

// start sending greetings to the server
// WSC_INT_STAT_START_HSHAKE
static void WSC_Task_StartHandshake(TCPIP_WSC_CONN_CTRL* pWsc)
{
    union
    {
        char resourceBuff[TCPIP_WSC_RESOURCE_MAX_LEN + 25U];    // resource space
        char srvBuff[20U + TCPIP_WSC_SERVER_MAX_LEN + 25U];     // srvName and all other plain headers
        char keyBuff[20U + WSC_KEY_BASE64_BUFFER_SIZE + 25U];   // key space
    }U_HDR_BUFF;

    int len = WSC_GenerateKey(pWsc->uBase64Key, sizeof(pWsc->uBase64Key)); 

    if(len < 0)
    {
        WSC_FailConnection(pWsc, TCPIP_WSC_RES_KEY_ERROR);
        return;
    }
    pWsc->base64KeyLen = (uint16_t)len;

    NET_PRES_SKT_HANDLE_T skt = pWsc->netSocket;

    // start writing to the socket; just open, should be enough room for the handshake message
    uint16_t totReqLen = 0;     // total required length to be written to the socket
    uint16_t outLen = 0;        // total length written

    // GET /resource HTTP/1.1
    int hdrLen = FC_sprintf(U_HDR_BUFF.resourceBuff, sizeof(U_HDR_BUFF.resourceBuff), wsc_req_header, pWsc->resource);
    _TCPIPStack_Assert(hdrLen >= 0, __FILE__, __func__, __LINE__);
    totReqLen += (uint16_t)hdrLen;
    outLen += NET_PRES_SocketWrite(skt, U_HDR_BUFF.resourceBuff, (uint16_t)hdrLen);
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_TX_HSHAKE) != 0)
     U_HDR_BUFF.resourceBuff[hdrLen] = '\0';
     SYS_CONSOLE_PRINT("WSC TX hdr-> %s\r\n", U_HDR_BUFF.resourceBuff);
#endif // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_TX_HSHAKE) != 0)
        
    // Host: server.example.com :port
    hdrLen = FC_sprintf(U_HDR_BUFF.srvBuff, sizeof(U_HDR_BUFF.srvBuff), wsc_host_header, pWsc->srvName, pWsc->connPort);
    _TCPIPStack_Assert(hdrLen >= 0, __FILE__, __func__, __LINE__);
    totReqLen += (uint16_t)hdrLen;
    outLen += NET_PRES_SocketWrite(skt, U_HDR_BUFF.srvBuff, (uint16_t)hdrLen);
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_TX_HSHAKE) != 0)
     U_HDR_BUFF.srvBuff[hdrLen] = '\0';
     SYS_CONSOLE_PRINT("WSC TX hdr-> %s\r\n", U_HDR_BUFF.srvBuff);
#endif // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_TX_HSHAKE) != 0)

    // "Upgrade: websocket";
    uint16_t sLen = (uint16_t)strlen(wsc_upgrade_header); 
    totReqLen += sLen;
    (void)strcpy(U_HDR_BUFF.srvBuff, wsc_upgrade_header);
    outLen += NET_PRES_SocketWrite(skt, U_HDR_BUFF.srvBuff, sLen);
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_TX_HSHAKE) != 0)
     U_HDR_BUFF.srvBuff[sLen] = '\0';
     SYS_CONSOLE_PRINT("WSC TX hdr-> %s\r\n", U_HDR_BUFF.srvBuff);
#endif // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_TX_HSHAKE) != 0)
    
    // "Connection: Upgrade";
    sLen = (uint16_t)strlen(wsc_connection_header);
    totReqLen += sLen;
    (void)strcpy(U_HDR_BUFF.srvBuff, wsc_connection_header);
    outLen += NET_PRES_SocketWrite(skt, U_HDR_BUFF.srvBuff, sLen);
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_TX_HSHAKE) != 0)
     U_HDR_BUFF.srvBuff[sLen] = '\0';
     SYS_CONSOLE_PRINT("WSC TX hdr-> %s\r\n", U_HDR_BUFF.srvBuff);
#endif // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_TX_HSHAKE) != 0)
    
     // "Sec-WebSocket-Protocol: "
     if(pWsc->nProtocols != 0U)
     {
         sLen = (uint16_t)strlen(wsc_protocol_header);
         (void)strcpy(U_HDR_BUFF.srvBuff, wsc_protocol_header);
         U_HDR_BUFF.srvBuff[sLen] = ' ';    // add start proto list delimiter
         sLen++;
         totReqLen += sLen;
         outLen += NET_PRES_SocketWrite(skt, U_HDR_BUFF.srvBuff, sLen);
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_TX_HSHAKE) != 0)
         U_HDR_BUFF.srvBuff[sLen] = '\0';
         SYS_CONSOLE_PRINT("WSC TX hdr-> %s", U_HDR_BUFF.srvBuff);
#endif // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_TX_HSHAKE) != 0)

         uint16_t protoIx;
         const char* protoSrc;
         for(protoIx = 0; protoIx < pWsc->nProtocols; protoIx++)
         {
             protoSrc = pWsc->protocols[protoIx];
             sLen = (uint16_t)strlen(protoSrc);
             (void)strcpy(U_HDR_BUFF.srvBuff, protoSrc);
             if(protoIx == pWsc->nProtocols - 1U)
             {  // add ws_end_line: "\r\n"
                 U_HDR_BUFF.srvBuff[sLen] = ws_end_line[0];
                 U_HDR_BUFF.srvBuff[sLen + 1U] = ws_end_line[1];
             }
             else
             {  // add ws_list_delim: ", "
                 U_HDR_BUFF.srvBuff[sLen] = ws_list_delim[0];
                 U_HDR_BUFF.srvBuff[sLen + 1U] = ws_list_delim[1];
             }
             sLen += 2;     // protocol separator len: ", " or "\r\n"
             totReqLen += sLen;
             outLen += NET_PRES_SocketWrite(skt, U_HDR_BUFF.srvBuff, sLen);
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_TX_HSHAKE) != 0)
             U_HDR_BUFF.srvBuff[sLen] = '\0';
             SYS_CONSOLE_PRINT("%sn", U_HDR_BUFF.srvBuff);
#endif // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_TX_HSHAKE) != 0)
         }
     } 

    // Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==
    hdrLen = FC_sprintf(U_HDR_BUFF.keyBuff, sizeof(U_HDR_BUFF.keyBuff), wsc_key_header, pWsc->cBase64Key);
    _TCPIPStack_Assert(hdrLen >= 0, __FILE__, __func__, __LINE__);
    totReqLen += (uint16_t)hdrLen;
    outLen += NET_PRES_SocketWrite(skt, U_HDR_BUFF.keyBuff, (uint16_t)hdrLen);
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_TX_HSHAKE) != 0)
     U_HDR_BUFF.keyBuff[hdrLen] = '\0';
     SYS_CONSOLE_PRINT("WSC TX hdr-> %s\r\n", U_HDR_BUFF.keyBuff);
#endif // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_TX_HSHAKE) != 0)

    // Sec-WebSocket-Version: 13
    if((pWsc->connFlags & TCPIP_WSC_CONN_FLAG_SKIP_VERSION) == 0U) 
    {
        sLen = (uint16_t)strlen(wsc_version_header);
        totReqLen += sLen;
        (void)strcpy(U_HDR_BUFF.srvBuff, wsc_version_header);
        outLen += NET_PRES_SocketWrite(skt, U_HDR_BUFF.srvBuff, sLen);
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_TX_HSHAKE) != 0)
        U_HDR_BUFF.srvBuff[sLen] = '\0';
        SYS_CONSOLE_PRINT("WSC TX hdr-> %s\r\n", U_HDR_BUFF.srvBuff);
#endif // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_TX_HSHAKE) != 0)
    }

    // end of headers
    sLen = (uint16_t)strlen(ws_end_line);
    totReqLen += sLen;
    (void)strcpy(U_HDR_BUFF.srvBuff, ws_end_line);
    outLen += NET_PRES_SocketWrite(skt, U_HDR_BUFF.srvBuff, sLen);
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_TX_HSHAKE) != 0)
        U_HDR_BUFF.srvBuff[sLen] = '\0';
        SYS_CONSOLE_PRINT("WSC TX hdr-> %s\r\n", U_HDR_BUFF.srvBuff);
#endif // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_TX_HSHAKE) != 0)

    if(outLen != totReqLen)
    {   // failed
        WSC_FailConnection(pWsc, TCPIP_WSC_RES_SKT_WR_ERROR);
    }
    else
    {   // all good
        (void)NET_PRES_SocketFlush(skt); 
        WSC_StartSrvWaitTimer(pWsc);
        pWsc->valMask = 0;
        SetIntState(pWsc, WSC_INT_STAT_WAIT_HSHAKE);
    }
    
}

// generate the WS key
// returns > 0 == length of the base 64 key
// < 0 - error
static int WSC_GenerateKey(uint8_t* base64Buffer, size_t buffLen)
{
    uint8_t wsKeyBuffer[WSC_KEY_BUFFER_SIZE];
    
    int res = WSC_RngGenerate(wsKeyBuffer, sizeof(wsKeyBuffer));

    if (res < 0)
    {   // failure
        return res;
    }

    // base64 encode
    uint16_t base64Len = TCPIP_Helper_Base64Encode(wsKeyBuffer, (uint16_t)sizeof(wsKeyBuffer), base64Buffer, (uint16_t)buffLen);
    base64Buffer[base64Len] = 0;    // end string properly

    // success
    return (int)base64Len;
}

// server handshake validation functions
typedef uint32_t (*VALIDATE_SRV_FNC)(TCPIP_WSC_CONN_CTRL* pWsc, char* lineBuff, char* kwrdEnd);
static uint32_t Validate_StatusLine(TCPIP_WSC_CONN_CTRL* pWsc, char* lineBuff, char* kwrdEnd);
static uint32_t Validate_UpgradeLine(TCPIP_WSC_CONN_CTRL* pWsc, char* lineBuff, char* kwrdEnd);
static uint32_t Validate_ConnectionLine(TCPIP_WSC_CONN_CTRL* pWsc, char* lineBuff, char* kwrdEnd);
static uint32_t Validate_AcceptLine(TCPIP_WSC_CONN_CTRL* pWsc, char* lineBuff, char* kwrdEnd);
static uint32_t Validate_ProtoLine(TCPIP_WSC_CONN_CTRL* pWsc, char* lineBuff, char* kwrdEnd);
static uint32_t Validate_ExtensionLine(TCPIP_WSC_CONN_CTRL* pWsc, char* lineBuff, char* kwrdEnd);

typedef struct
{
    const char*         kWord;      // line start keyword, space separated
    VALIDATE_SRV_FNC    valFnc;
}WSC_VALIDATE_ENTRY;


static const WSC_VALIDATE_ENTRY wsc_validate_tbl[] =
{
    {wss_status_line, &Validate_StatusLine},
    {wss_upgrade_line, &Validate_UpgradeLine},
    {wss_connection_line, &Validate_ConnectionLine},
    {wss_accept_line, &Validate_AcceptLine},
    {wss_proto_line, &Validate_ProtoLine},
    {wss_extension_line, &Validate_ExtensionLine},
};

// WSC_INT_STAT_WAIT_HSHAKE
static void WSC_Task_WaitHandshake(TCPIP_WSC_CONN_CTRL* pWsc)
{
    uint16_t    avlblBytes, peekBytes;
    char*       endPeek;

    NET_PRES_SKT_HANDLE_T skt = pWsc->netSocket;
    
    avlblBytes = NET_PRES_SocketReadIsReady(skt);

    while(avlblBytes != 0U)
    {   // data waiting; peek into available data
        char* hdrBuff = pWsc->c_peekBuff; 
        peekBytes = NET_PRES_SocketPeek(skt, pWsc->u_peekBuff, (uint16_t)sizeof(pWsc->u_peekBuff));
        hdrBuff[peekBytes] = '\0';
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_RX_HSHAKE) != 0)
        SYS_CONSOLE_PRINT("WSC Srv hshake -> %s\r\n", hdrBuff); 
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_RX_HSHAKE) != 0)
        // search for the header end
        char* hdrEnd = strstr(hdrBuff, ws_end_header);
        size_t hdrEndLen = 0;
        if(hdrEnd != NULL)
        {
            hdrEndLen = strlen(ws_end_header);
        }
        else
        {   // check for alt ending
            hdrEnd = strstr(hdrBuff, ws_alt_end_header);
            if(hdrEnd != NULL)
            {
                hdrEndLen = strlen(ws_alt_end_header);
            }
        }

        if(hdrEnd != NULL)
        {
            hdrEnd[1] = '\0';   // end properly but leave the end of line in place
            hdrEnd += hdrEndLen;
            peekBytes = (uint16_t)(hdrEnd - hdrBuff);
        }
        else
        {   // find at least an end of line
            endPeek = (char*)strdelimr(hdrBuff, ws_end_line);
            if(endPeek == NULL)
            {   // we need at least a line to process
                if(peekBytes == (uint16_t)sizeof(pWsc->u_peekBuff))
                {   // line longer than our whole peek buffer; just discard it!
                    (void)NET_PRES_SocketRead(skt, NULL, peekBytes);  // shouldn't be part of the standard headers anyway
                }
                // wait for more header characters
                break;
            }
            else
            {   // got an end of line
                *(++endPeek) = '\0';
                peekBytes = (uint16_t)(endPeek - hdrBuff);
            }
        }

        // discard whatever we read
        (void)NET_PRES_SocketRead(skt, NULL, peekBytes);
        // ok, process buffer
        pWsc->valMask |= WSC_ValidateResponse(pWsc, hdrBuff);

        if(hdrEnd != NULL)
        {   // we're done
            uint32_t valMask = pWsc->valMask & WSC_VALID_MASK_ALL;   // the detected validation mask
            uint32_t expMask = WSC_VALID_MASK_MANDATORY;   // expected validation mask
            if(pWsc->nProtocols != 0U)
            {   // sub-protocols present
                if((pWsc->connFlags & (uint16_t)TCPIP_WSC_CONN_FLAG_PROTO_OPTIONAL) != 0U)
                {   // sub-protocols are optional
                    if((valMask & WSC_VALID_MASK_PROTO_PRESENT) != 0U)
                    {   // server replied with a sub-protocol
                        expMask |= (WSC_VALID_MASK_PROTO_PRESENT | WSC_VALID_MASK_PROTO_MATCH);
                    }
                    // else ok: server didn't choose any of our sub-protocols
                }
                else
                {   // sub-protocols are not optional
                    expMask |= (WSC_VALID_MASK_PROTO_PRESENT | WSC_VALID_MASK_PROTO_MATCH);
                }
            }

            bool valOk = valMask == expMask;
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_VALIDATE) != 0)
            SYS_CONSOLE_PRINT("WSC Validation %s - Expected: 0x%x, Got: 0x%x\r\n", valOk ? "Success" : "Failed", expMask, valMask);
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_VALIDATE) != 0)

            if(valOk)
            {   // success
                WSC_OpenConnection(pWsc);
            }
            else
            {   // failed to validate the server's response
                WSC_FailConnection(pWsc, TCPIP_WSC_RES_VALIDATION_ERROR);
            }
            return;
        }

        // more header to be processed
        break;
    }

    // check for timeout
    if(WSC_IsSrvTmo(pWsc))
    {   // timeout
        WSC_FailConnection(pWsc, TCPIP_WSC_RES_SRV_TMO);
    }
    // else wait some more...
}

// data is avaliable
// validate the server's response
// srvResponse is a buffer that can be modified!
static uint32_t WSC_ValidateResponse(TCPIP_WSC_CONN_CTRL* pWsc, char* srvResponse)
{
    size_t  ix;
    const char *lineStart, *eol, *lineEnd;
    char    lineBuff[WSC_LINE_BUFFER_SIZE + 1U];
    char    wordBuff[WSC_WORD_BUFFER_SIZE + 1U];

    lineStart = srvResponse;
    uint32_t    valMask = 0;
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_VALIDATE) != 0)
    SYS_CONSOLE_PRINT("WSC Validate -> Processing: %s\r\n", srvResponse);
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_VALIDATE) != 0)

    while(true)
    {
        if(lineStart == NULL)
        {   // done
            break;
        }

        while(lineStart[0] == ' ')
        {   // skip ws
            lineStart++;
        }
        eol = strstrany(lineStart, ws_end_line, &lineEnd); 
        if(eol == NULL)
        {   // no more lines found
            break;
        }

        int lineSize = eol - lineStart;
        if(lineSize == 0 || lineSize > (int)sizeof(lineBuff) - 1U)
        {   // ignore empty/very long lines - not our headers!
            lineStart = lineEnd;
            continue;
        }
        // process this line;

        (void)strncpy(lineBuff, lineStart, (size_t)lineSize);
        lineBuff[lineSize] = '\0'; 
        lineStart = lineEnd;

#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_VALIDATE) != 0)
        SYS_CONSOLE_PRINT("WSC Validate -> Processing line: %s\r\n", lineBuff);
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_VALIDATE) != 0)
        char* wordEnd = strchr(lineBuff, ws_word_delim);
        if(wordEnd == NULL)
        {   // ignore unknown lines ?
            continue;
        }
        // check if known word
        // dummy linear search
        int wordLen = wordEnd - lineBuff;
        (void)strncpy(wordBuff, lineBuff, (size_t)wordLen);
        wordBuff[wordLen] = '\0'; 

        const WSC_VALIDATE_ENTRY* pEntry = wsc_validate_tbl; 
        for(ix = 0; ix < sizeof(wsc_validate_tbl) / sizeof(*wsc_validate_tbl); ix++)
        {
            if(stricmp(wordBuff, pEntry->kWord) == 0)
            {   // match; process
                uint32_t entryMask = pEntry->valFnc(pWsc, lineBuff, wordEnd);
                valMask |= entryMask;

#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_VALIDATE) != 0)
                SYS_CONSOLE_PRINT("WSC Validate header: %s, returned mask: 0x%x\r\n", wordBuff, entryMask);
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_VALIDATE) != 0)
                break;
            }
            pEntry++;
        }
    }
    
    return valMask;
}

// validate "HTTP/1.1 101 Switching Protocols"
static uint32_t Validate_StatusLine(TCPIP_WSC_CONN_CTRL* pWsc, char* lineBuff, char* kwrdEnd)
{
    return (stricmp(kwrdEnd + 1, "101 Switching Protocols" ) == 0)? WSC_VALID_MASK_STATUS_OK : WSC_VALID_MASK_NONE; 
}

// validate "Upgrade: websocket"
static uint32_t Validate_UpgradeLine(TCPIP_WSC_CONN_CTRL* pWsc, char* lineBuff, char* kwrdEnd)
{
    return (stricmp(kwrdEnd + 1, "websocket") == 0)? WSC_VALID_MASK_UPGRADE_OK : WSC_VALID_MASK_NONE;
}

// validate "Connection: Upgrade"
static uint32_t Validate_ConnectionLine(TCPIP_WSC_CONN_CTRL* pWsc, char* lineBuff, char* kwrdEnd)
{
    return (stricmp(kwrdEnd + 1, "Upgrade") == 0)? WSC_VALID_MASK_CONNECTION_OK : WSC_VALID_MASK_NONE;
}

// validate "Sec-WebSocket-Accept: dGhlIHNhbXBsZSBub25jZQ==\r\n"
// |Sec-WebSocket-Accept| contains the value of the base64-encoded SHA-1
// of the concatenation of the |Sec-WebSocket-Key| (as a string, not base64-decoded)
// with the WSS_GUID string
static uint32_t Validate_AcceptLine(TCPIP_WSC_CONN_CTRL* pWsc, char* lineBuff, char* kwrdEnd)
{
    // verify the accept key
    //  get the SHA-1 of the (base64 encoded key, ws_guid) - concatenated
    CRYPT_SHA_CTX shaCtx;
    uint8_t shaDigest[20];  // SHA1 size is 160 bits
    union
    {
        uint8_t ub64Buffer[28 + 1]; // a 20 byte value results in 21 * 4 / 3 = 28 bytes base64 encoding
        char    cb64Buffer[28 + 1];
    }U_64_BUFF;
    union
    {
        const char*     cPtr;
        const uint8_t*  uPtr;
    }U_CH_UINT_PTR;
    
    int shaRes = CRYPT_SHA_Initialize(&shaCtx);
    if(shaRes >= 0)
    {   
        shaRes = CRYPT_SHA_DataAdd(&shaCtx, pWsc->uBase64Key, (size_t)pWsc->base64KeyLen);
        if(shaRes >= 0)
        {
            U_CH_UINT_PTR.cPtr = ws_guid;
            shaRes = CRYPT_SHA_DataAdd(&shaCtx, U_CH_UINT_PTR.uPtr, strlen(ws_guid));
            if(shaRes >= 0)
            { // done
                shaRes = CRYPT_SHA_Finalize(&shaCtx, shaDigest);
            }
        }
    }

    if(shaRes < 0)
    {   // failed
        return WSC_VALID_MASK_NONE;
    }

    // base64 encode the SHA-1 digest 
    uint16_t base64Len = TCPIP_Helper_Base64Encode(shaDigest, (uint16_t)sizeof(shaDigest), U_64_BUFF.ub64Buffer, (uint16_t)sizeof(U_64_BUFF.ub64Buffer ) - 1U);
    U_64_BUFF.ub64Buffer[base64Len] = 0;    // end string properly

    // that should be the value in the accept key.
    return (strcmp(U_64_BUFF.cb64Buffer, kwrdEnd + 1U) == 0)? WSC_VALID_MASK_ACCEPT_OK : WSC_VALID_MASK_NONE;
}

// validate "Sec-WebSocket-Protocol: prot"
// RFC: Server should reply with:
//      - Either a single value representing the subprotocol the server is ready to use
//              The value chosen MUST be derived from the client’s handshake, specifically by selecting one of
//              the values from the |Sec-WebSocket-Protocol| field that the server is willing
//              to use for this connection (if any).
//      - or NULL.
static uint32_t Validate_ProtoLine(TCPIP_WSC_CONN_CTRL* pWsc, char* lineBuff, char* kwrdEnd)
{
    // check that the server replied with one of our requested sub-protocols
    uint16_t protoIx;
    const char* protoSrc;
    for(protoIx = 0; protoIx < pWsc->nProtocols; protoIx++)
    {
        protoSrc = pWsc->protocols[protoIx];
        if(strcmp(protoSrc, kwrdEnd + 1) == 0)
        {   // match; save the '_Subprotocol In Use_' 
            pWsc->protInUseIx = (int16_t)protoIx;
            return WSC_VALID_MASK_PROTO_PRESENT | WSC_VALID_MASK_PROTO_MATCH;
        }
    }

    // present but no match
    return WSC_VALID_MASK_PROTO_PRESENT;
}

// validate "Sec-WebSocket-Extensions: ex1, ex2"
static uint32_t Validate_ExtensionLine(TCPIP_WSC_CONN_CTRL* pWsc, char* lineBuff, char* kwrdEnd)
{
    // currently not supported and there should not be in the servers' response
    return WSC_VALID_MASK_EXT_PRESENT;
}


// WSC_INT_STAT_OPEN
// WSC_INT_STAT_CLOSING
// Receive and process messages
static void WSC_Task_Process(TCPIP_WSC_CONN_CTRL* pWsc)
{
    // get a lock
    if(!WSC_ConnectionLock(pWsc, true))
    {   // try next time
        return;
    }

    while(true)
    {
        // discard data that's pending in the socket RX buffer
        if(!WSC_DiscardFrame(pWsc, TCPIP_WSC_RES_MORE_DATA))
        {   // more data needed to be discarded
            break;
        }

        // process any pending frame
        // for now do not grab other messages until the current one is processed!
        // RFC: An endpoint MUST be capable of handling control frames in the middle of a fragmented message!
        // However, pendMsg.msgHandle != 0 means we are within a frame!
        if(!WSC_ProcPendMsg(pWsc))
        {
            break;
        }

        // receive incoming frames/messages
        if(!WSC_RxFrame(pWsc))
        {   // no full frame to process
            break;
        }

        // complete frame header available. May NOT have the payload available yet!
        // start a timer just in case we don't get a whole frame from the server
        WSC_StartSrvWaitTimer(pWsc);
        // same for user: needs to read the message!
        WSC_StartUsrWaitTimer(pWsc);

        TCPIP_WSC_PEND_MSG_DCPT* pendDcpt = &pWsc->pendRxMsg;
        TCPIP_WS_FRAME_TYPE frameType = (TCPIP_WS_FRAME_TYPE)pendDcpt->info.frameType;
        if(frameType != TCPIP_WS_FRAME_TYPE_CTRL)
        {   // the pending control message is handled separately...
            // pending user data frame
            if(pWsc->intState == WSC_INT_STAT_CLOSING)
            {   // no longer acceppting messages; discard
                pendDcpt->info.frameType = (uint8_t)TCPIP_WS_FRAME_TYPE_NONE;
                pendDcpt->msgHandle = NULL;   // show no data message is pending
                pWsc->discardRxMsg = pendDcpt; 
                (void)WSC_DiscardFrame(pWsc, TCPIP_WSC_RES_CLOSING);
            }
            else
            {   // valid user data message; fire an event
                TCPIP_WSC_PEND_MSG_DCPT evMsgDcpt = *pendDcpt;
                TCPIP_WSC_EV_INFO evInfo;
                evInfo.pendMsgDcpt = &evMsgDcpt;
                (void)WSC_Notify_Event(pWsc, WSC_EVENT_MSG_READY, evInfo);
            }
        }
        break;
    }

    WSC_ConnectionUnlock(pWsc, true);
}

// process a pending message
// pendRxMsg->msgHandle != NULL
// returns:
//      true if all processing done and 
//          new message can be rx-ed/processed
//      false if more pending data or processing is not to be continued 
static bool WSC_ProcPendMsg(TCPIP_WSC_CONN_CTRL* pWsc)
{

    TCPIP_WSC_PEND_MSG_DCPT* pendDcpt = &pWsc->pendRxMsg;
    if(pendDcpt->msgHandle == NULL)
    {   // nothing to do
        return true;
    }

    TCPIP_WS_FRAME_TYPE frameType = (TCPIP_WS_FRAME_TYPE)pendDcpt->info.frameType;
    // msgHandle != NULL only if a pending frame!
    _TCPIPStack_Assert(frameType != TCPIP_WS_FRAME_TYPE_NONE, __FILE__, __func__, __LINE__);

    // update the data from the server
    (void)WSC_SrvRxUpdate(pWsc, pendDcpt);

    uint16_t msgAct;
    // process control message first:
    //  - you may want to send some event/data to the user
    //  - more importantly, do the internal control processing!
    if(frameType == TCPIP_WS_FRAME_TYPE_CTRL)
    {
        msgAct = WSC_ProcPendCtrl(pWsc);
    }
    else
    {   // for data messages default is checking all
        msgAct = (uint16_t)WSC_MSG_ACTION_SRV_TMO | (uint16_t)WSC_MSG_ACTION_USR_TMO | (uint16_t)WSC_MSG_ACTION_CLOSING; 
    }

    // process the pending data frame
    if((msgAct & (uint16_t)WSC_MSG_ACTION_DONE) != 0U)
    {   // processing done
        return true;
    }

    TCPIP_WSC_EVENT_TYPE discardEv = WSC_EVENT_NONE;
    TCPIP_WSC_RES discardRes;
    while(true)
    {
        if((msgAct & (uint16_t)WSC_MSG_ACTION_CLOSING) != 0U)
        {
           if(pWsc->intState == WSC_INT_STAT_CLOSING)
           {   // no longer accepting reads from the user; discard
               discardEv = WSC_EVENT_MSG_DISCARD_CLOSE;
               discardRes = TCPIP_WSC_RES_CLOSING; 
               break;
           }
        }


        if((msgAct & (uint16_t)WSC_MSG_ACTION_SRV_TMO) != 0U)
        {   // check if server timeout
            if(pendDcpt->srvAvlblLen != pendDcpt->payloadLen && pendDcpt->sktPendLen != pWsc->parent->sktRxBuffSize)
            {   // more data needed from the server and the socket buffer not full
                if(WSC_IsSrvTmo(pWsc))
                {
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SRV_TMO) != 0)
                    SYS_CONSOLE_PRINT("WSC Srv Tmo Update - payloadLen: %d, srvAvlblLen: %d, sktPendLen: %d, renderedLen: %d\r\n", pendDcpt->payloadLen, pendDcpt->srvAvlblLen, pendDcpt->sktPendLen, pendDcpt->renderedLen);
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SRV_TMO) != 0)
                    WSC_FailConnection(pWsc, TCPIP_WSC_RES_SRV_TMO);
                    return false;
                }
            }
        }
                
        if((msgAct & (uint16_t)WSC_MSG_ACTION_USR_TMO) != 0U)
        {   // check if user is late in retrieving the message
            // Note: currently this is NOT a global timeout for receiving the complete message
            // it is a timer that's restarted every time the user performs a read!
            if(WSC_IsUsrTmo(pWsc))
            {   // timeout in processing the message
#if (M_WSC_DISCARD_USR_TMO_ON_RX_PENDING == 0)
                // discard anyway if timeout exceeded
                discardEv = WSC_EVENT_MSG_DISCARD_USR_TMO;
                discardRes = TCPIP_WSC_RES_USR_TMO; 
#else
                // discard only if there's pending data
                if (pendDcpt->sktPendLen >= (uint16_t)sizeof(WS_SERVER_FRAME_CTRL))
                {   // other frame pending...
                    discardEv = WSC_EVENT_MSG_DISCARD_USR_TMO;
                    discardRes = TCPIP_WSC_RES_USR_TMO; 
                }
#endif  // (M_WSC_DISCARD_USR_TMO_ON_RX_PENDING == 0)
            }
        }

        // else OK to wait some more
        break;
    }

    if(discardEv != WSC_EVENT_NONE)
    {
        TCPIP_WSC_EV_INFO evInfo;
        TCPIP_WSC_ERR_MSG_DCPT errDcpt;
        errDcpt.info = pendDcpt->info;
        errDcpt.payloadLen = (uint64_t)pendDcpt->payloadLen;

        pendDcpt->info.frameType = (uint8_t)TCPIP_WS_FRAME_TYPE_NONE;
        pendDcpt->msgHandle = NULL;   // show no data message is pending
        pWsc->discardRxMsg = pendDcpt;
        (void)WSC_DiscardFrame(pWsc, discardRes);

        evInfo.errMsgDcpt = &errDcpt;
        (void)WSC_Notify_Event(pWsc, discardEv, evInfo);
    }

    return false;
}

// receives incoming frames/messages
// places the pending info in the pWsc->pendRxMsg for further processing
// returns
//      true if a frame header is successfully received.
//          NOTE: The complete frame payload may not be available in the socket buffer!
//      false otherwise
static bool WSC_RxFrame(TCPIP_WSC_CONN_CTRL* pWsc)
{

    union
    {
        WS_SERVER_FRAME_CTRL    cFrame;
        WS_SERVER_FRAME_SMALL   sFrame;
        WS_SERVER_FRAME_MEDIUM  mFrame;
        WS_SERVER_FRAME_LARGE   lFrame;
        uint8_t u8Buff[sizeof(WS_SERVER_FRAME_LARGE)];
    }U_FRAME_BUFF;
    TCPIP_UINT64_VAL payloadLen;    // keep a 64 bit value to accommodate the (possible) large frames

    // check data available
    NET_PRES_SKT_HANDLE_T skt = pWsc->netSocket;

    uint16_t peekBytes;
    uint16_t avlblBytes = NET_PRES_SocketReadIsReady(skt);

    if (avlblBytes < sizeof(U_FRAME_BUFF.cFrame))
    {   // we need at least a minimum frame to process
        // no start for checking for the server timeout yet
        return false; // wait for more data
    }

    if(avlblBytes > (uint16_t)sizeof(U_FRAME_BUFF))
    {
        peekBytes = (uint16_t)sizeof(U_FRAME_BUFF);
    }
    else
    {
        peekBytes = avlblBytes;
    }

    TCPIP_WS_FRAME_TYPE frameType;
    uint16_t  frameLen;
    int badFrameType = 0;   // 1 - proto error; 2 - length error; 3 - large frame length; 4 - large frame, too big to process 
    (void)NET_PRES_SocketPeek(skt, U_FRAME_BUFF.u8Buff, peekBytes);
    // check what frame we have to handle
    if(U_FRAME_BUFF.cFrame.hdr.payLen <= WS_FRAME_CTRL_MAX_PAYLEN)
    {   // control/small frame
        if(U_FRAME_BUFF.cFrame.hdr.opcode >= WS_OPCODE_START_CTRL)
        {   // control frame 
            frameType = TCPIP_WS_FRAME_TYPE_CTRL;
            frameLen = (uint16_t)sizeof( U_FRAME_BUFF.cFrame);
        }
        else
        {   // small frame
            frameType = TCPIP_WS_FRAME_TYPE_SMALL;
            frameLen = (uint16_t)sizeof( U_FRAME_BUFF.sFrame);
        }
    }
    else if(U_FRAME_BUFF.cFrame.hdr.payLen == WS_FRAME_MEDIUM_PAYLEN)
    {   // medium
        frameType = TCPIP_WS_FRAME_TYPE_MEDIUM;
        frameLen = (uint16_t)sizeof( U_FRAME_BUFF.mFrame);
    }
    else
    {   // large
        _TCPIPStack_Assert(U_FRAME_BUFF.cFrame.hdr.payLen == WS_FRAME_LARGE_PAYLEN, __FILE__, __func__, __LINE__);
        frameType = TCPIP_WS_FRAME_TYPE_LARGE;
        frameLen = (uint16_t)sizeof(U_FRAME_BUFF.lFrame);
    }

    if(peekBytes < frameLen)
    {   // not yet enough data; need the whole frame header
        return false;
    }

    // get the payload length
    switch(frameType)
    {
        case TCPIP_WS_FRAME_TYPE_CTRL:
            payloadLen.Val = U_FRAME_BUFF.cFrame.hdr.payLen; 
            // minimal sanity check
            if(U_FRAME_BUFF.cFrame.hdr.mask != 0U || U_FRAME_BUFF.cFrame.hdr.fin == 0U)
            {
                badFrameType = 1;
            }
            break;

        case TCPIP_WS_FRAME_TYPE_SMALL:
            payloadLen.Val = U_FRAME_BUFF.sFrame.hdr.payLen; 
            break;

        case TCPIP_WS_FRAME_TYPE_MEDIUM:
            payloadLen.Val = TCPIP_Helper_ntohs(U_FRAME_BUFF.mFrame.extPayLen); 
            // sanity check
            if(payloadLen.Val < 128U || payloadLen.Val > WS_FRAME_MEDIUM_MAX_EXT_PAYLEN)
            {   // wrong length
                badFrameType = 2;
            } 
            break;

        default:    // TCPIP_WS_FRAME_TYPE_LARGE
            payloadLen.w[0] = TCPIP_Helper_ntohs(U_FRAME_BUFF.lFrame.extPayLenLow2);
            payloadLen.w[1] = TCPIP_Helper_ntohs(U_FRAME_BUFF.lFrame.extPayLenLow1);
            payloadLen.w[2] = TCPIP_Helper_ntohs(U_FRAME_BUFF.lFrame.extPayLenHi2);
            payloadLen.w[3] = TCPIP_Helper_ntohs(U_FRAME_BUFF.lFrame.extPayLenHi1);

            if(payloadLen.Val < 0x10000ULL)
            {
                badFrameType = 3;
            }
#if (M_WSC_LARGE_FRAME_SUPPORT == 0)
            if(badFrameType == 0)
            {
                badFrameType = 4;    // not supported (yet)
            }
#endif  // (M_WSC_LARGE_FRAME_SUPPORT == 0)

            break;
    }

#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_RX_MSG) != 0)
    SYS_CONSOLE_PRINT("WSC RX msg -> frameType: %d, frameLen: %d, payloadLen: %llu, bad: %d\r\n", frameType, frameLen, payloadLen.Val, badFrameType);
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_RX_MSG) != 0)

    TCPIP_WSC_MSG_INFO msgInfo;
    msgInfo.flags = WSC_MsgFlags(U_FRAME_BUFF.cFrame.hdr);
    msgInfo.opCode =  U_FRAME_BUFF.cFrame.hdr.opcode;
    msgInfo.frameType = (uint8_t)frameType;
    msgInfo.frameLen = (uint8_t)frameLen;

    if(badFrameType != 0)
    {   // fail the connection
        TCPIP_WSC_ERR_MSG_DCPT errDcpt;
        TCPIP_WSC_EV_INFO evInfo;

        errDcpt.info = msgInfo;
        errDcpt.payloadLen = payloadLen.Val;
        evInfo.errMsgDcpt = &errDcpt;
        (void)WSC_Notify_Event(pWsc, WSC_EVENT_MSG_ERROR, evInfo);

        WSC_FailConnection(pWsc, badFrameType < 4 ? TCPIP_WSC_RES_MSG_PROTO_ERR : TCPIP_WSC_RES_MSG_TOO_LARGE);
        return false;
    }

#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_RX_MSG) != 0) && ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_RX_LOAD) != 0)
    // show payload too 
    union
    {
        uint8_t uBuff[WSC_DBG_RX_BUFFER_SIZE];
        char    cBuff[WSC_DBG_RX_BUFFER_SIZE + 1U];
    }U_PEEK_BUFF;

    peekBytes = frameLen + (uint16_t)payloadLen.Val;
    if(sizeof(U_PEEK_BUFF.uBuff) < peekBytes)
    {
       peekBytes =  (uint16_t)sizeof(U_PEEK_BUFF.uBuff); 
    }
    peekBytes = NET_PRES_SocketPeek(skt, U_PEEK_BUFF.uBuff, peekBytes);
    U_PEEK_BUFF.cBuff[peekBytes] = '\0';
    if(peekBytes > frameLen)
    {
        SYS_CONSOLE_PRINT("WSC RX payload -> %s\r\n", U_PEEK_BUFF.cBuff + frameLen);
    }
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_RX_MSG) != 0) && ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_RX_LOAD) != 0)


    (void)NET_PRES_SocketRead(skt, NULL, frameLen);   // discard the frame header, shouldn't be needed!

    pWsc->pendRxMsg.info = msgInfo;
    pWsc->pendRxMsg.payloadLen = (uint16_t)payloadLen.Val;

    // for a data/control message; save the message details for the user to process
    union
    {
        uint8_t u8[4];
        const void* handle;
    }U_HANDLE;

    U_HANDLE.handle = NULL;
    (void)WSC_RngGenerate(U_HANDLE.u8, sizeof(U_HANDLE.u8));
    pWsc->pendRxMsg.msgHandle = U_HANDLE.handle;    // generate handle and show message is pending
    pWsc->pendRxMsg.renderedLen = 0U;
    avlblBytes -= frameLen; // keep just the message payload
    if(avlblBytes > (uint16_t)payloadLen.Val)
    {
        avlblBytes = (uint16_t)payloadLen.Val;
    }
    pWsc->pendRxMsg.sktPendLen = avlblBytes;
    pWsc->pendRxMsg.srvAvlblLen = avlblBytes;
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SRV_UPDATE) != 0)
    SYS_CONSOLE_PRINT("WSC Frame Update - avlblBytes: %d, payloadLen: %d, srvAvlblLen: %d, sktPendLen: %d\r\n", avlblBytes, (uint16_t)payloadLen.Val, avlblBytes, avlblBytes);
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SRV_UPDATE) != 0)
    pWsc->peekCtrlLen = 0;

    return true;
}

// WSC_INT_STAT_CLOSED
// Closed state - just cleanup
// we have submitted or received a close frame
static void WSC_Task_Closed(TCPIP_WSC_CONN_CTRL* pWsc)
{
    TCPIP_WSC_EV_INFO evInfo;
    TCPIP_WSC_FAIL_INFO failInfo;

    TCPIP_WSC_EVENT_TYPE evType = WSC_EVENT_NONE; 
    TCPIP_WSC_RES res = TCPIP_WSC_RES_OK;
    bool doClose = false;

    while(true)
    {
        if(pWsc->cliCloseSent != 0U && pWsc->srvCloseRxed != 0U) 
        {   // sent and received a close frame; we're done
            // wait for the server to first close the TCP connection (with some timeout)
            // however the RFC states that the client MAY close the TCP connection too
            evInfo.evPtr = NULL;
            evType = WSC_EVENT_CONN_CLOSE;
            res = TCPIP_WSC_RES_CLOSING; 
            doClose = true;
            break;

        }

        if(pWsc->srvCloseRxed != 0U) 
        {   // received a close frame; have to send ours
            res = WSC_SendCloseFrame(pWsc, (TCPIP_WSC_CLOSE_CODE)pWsc->srvCloseCode, NULL);
            if(res == TCPIP_WSC_RES_OK)
            {   // all good; will be closed the next execution
                pWsc->cliCloseSent = 1;
                break;
            }

            if(res == TCPIP_WSC_RES_NO_WHOLE_SPACE)
            {   // will do a retry; and check the timeout!
                break;
            }

            // fatal error. abort
            _TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
            res = TCPIP_WSC_RES_FATAL_ERROR;
            break;
        }

        if(pWsc->cliCloseSent != 0U) 
        {   // just wait to hear from the server...
            // check the timeout
            break;
        }

        // should not happen!
        _TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
        res = TCPIP_WSC_RES_FATAL_ERROR;
        break;
    }

    if(res == TCPIP_WSC_RES_FATAL_ERROR)
    {
        failInfo.failRes = TCPIP_WSC_RES_FATAL_ERROR;
        evInfo.failInfo = &failInfo;
        evType = WSC_EVENT_FAIL_CONN;
        doClose = true;
    }

    if(evType != WSC_EVENT_NONE)
    {
        (void)WSC_Notify_Event(pWsc, evType, evInfo);
    }

    if(doClose)
    {
        WSC_CloseConnection(pWsc, res);
    }
}

// helper to translate an control opcode to an ctrl event
static TCPIP_WSC_EVENT_TYPE WSC_CtrlEvent(TCPIP_WS_OP_CODE opCode) 
{
    TCPIP_WSC_EVENT_TYPE evType;

    switch(opCode)
    {
        case TCPIP_WS_CTRL_CODE_CLOSE:
            evType = WSC_EVENT_CTRL_CLOSE;
            break;

        case TCPIP_WS_CTRL_CODE_PING:
            evType = WSC_EVENT_CTRL_PING;
            break;

        case TCPIP_WS_CTRL_CODE_PONG:
            evType = WSC_EVENT_CTRL_PONG;
            break;

        default:
            evType = WSC_EVENT_CTRL_UNKNOWN;
            break;
    }

    return evType;
}

// helper to initialize a connection
// Note: some members are set at the module initialization,
// so a plain memset will delete those!
static void Wsc_ConnInit(TCPIP_WSC_CONN_CTRL* pWsc)
{
    // save the pre-set members
    uint16_t connIx = pWsc->connIx;
    TCPIP_WSC_MODULE_DCPT* parent = pWsc->parent;
    (void)memset(pWsc, 0, sizeof(*pWsc));
    // restore
    pWsc->connIx = connIx;
    pWsc->parent = parent;
    pWsc->netSocket = NET_PRES_INVALID_SOCKET;
}

// calculates a TCPIP_WSC_MSG_FLAGS value from a message FIN and opCode settings
static uint8_t WSC_MsgFlags(WS_FRAME_HEADER frameHdr)
{
    unsigned int fin = frameHdr.fin;
    unsigned int opCode = frameHdr.opcode;
    uint8_t flags = 0;

    if(opCode >= WS_OPCODE_START_CTRL)
    {   // control frame 
        return (uint8_t)TCPIP_WSC_MSG_FLAG_CTRL;
    }

    // regular data frame
    // set the type flags
    switch(opCode)
    {
        case (unsigned int)TCPIP_WS_OPCODE_TEXT_FRAME:
            flags = (uint8_t)TCPIP_WSC_MSG_FLAG_TEXT;
            break;

        case (unsigned int)TCPIP_WS_OPCODE_BINARY_FRAME:
            flags = (uint8_t)TCPIP_WSC_MSG_FLAG_BINARY;
            break;

        case (unsigned int)TCPIP_WS_OPCODE_CONT_FRAME:
            // ok, no binary/text needed here
            break;

        default:
            // unknown/extension?
            flags = (uint8_t)TCPIP_WSC_MSG_FLAG_RX_UNKNOWN;
            break;
    }

    // set the fragmentation flags
    if(fin != 0U)
    {   // Fin == 1
        if(opCode == (unsigned int)TCPIP_WS_OPCODE_CONT_FRAME)
        {   // Fin + continuation: end of fragment
            flags |= (uint8_t)TCPIP_WSC_MSG_FLAG_FRAG_END;
        }
        else
        {   // Fin + No cont: full frame
            flags |= (uint8_t)TCPIP_WSC_MSG_FLAG_FRAG_START | (uint8_t)TCPIP_WSC_MSG_FLAG_FRAG_END;
        }
    } 
    else
    {   // Fin == 0
        if(opCode == (unsigned int)TCPIP_WS_OPCODE_CONT_FRAME)
        {   // Fin = 0 + continuation: mid of fragment
            flags |= (uint8_t)TCPIP_WSC_MSG_FLAG_FRAG_MID;
        }
        else
        {   // Fin = 0 + No cont: start frame
            flags |= (uint8_t)TCPIP_WSC_MSG_FLAG_FRAG_START;
        }
    }
    return flags;
}  

// process a pending control message
// returns:
//      a WSC_MSG_ACTION value to continue/not processing
// Note: the pWsc->pendRxMsg lenghths should be updated!
static uint16_t WSC_ProcPendCtrl(TCPIP_WSC_CONN_CTRL* pWsc)
{
    uint16_t peekBytes = 0;
    TCPIP_WSC_EV_INFO evInfo;

    NET_PRES_SKT_HANDLE_T skt = pWsc->netSocket;
    TCPIP_WSC_PEND_MSG_DCPT* pendDcpt = &pWsc->pendRxMsg;
    uint16_t ctrlLen = pendDcpt->payloadLen;

    if(ctrlLen != 0U)
    {   // some control message is there
        if(pWsc->peekCtrlLen == 0U)
        {   // ctrl message hasn't bean read into the peek buffer
            if(pendDcpt->sktPendLen != ctrlLen)
            {   // if the control message is not yet available, we should wait for it. Shouldn't happen too frequently though!
                // continue message processing (check for server timeouts, etc.)
                return (uint16_t)WSC_MSG_ACTION_SRV_TMO;
            }
            // got the whole control message
            peekBytes = NET_PRES_SocketPeek(skt, pWsc->u_peekBuff, ctrlLen);
            pWsc->c_peekBuff[peekBytes] = '\0';
            pWsc->peekCtrlLen = (uint8_t)ctrlLen;
        }
        evInfo.evCtrlMsg = pWsc->c_peekBuff;
    }
    else
    {
        peekBytes = 0;
        evInfo.evCtrlMsg = NULL;
    }

    // full control message available
    uint16_t retAct = (uint16_t)WSC_MSG_ACTION_NONE;
    // all good now; signal the user
    TCPIP_WS_OP_CODE opCode = (TCPIP_WS_OP_CODE)pendDcpt->info.opCode; 
    TCPIP_WSC_EVENT_TYPE ctrlEv = WSC_CtrlEvent(opCode);
    if(WSC_Notify_Event(pWsc, ctrlEv, evInfo))
    {   // event delivered; done!
        pendDcpt->info.frameType = (uint8_t)TCPIP_WS_FRAME_TYPE_NONE;
        pendDcpt->msgHandle = NULL;
        if(peekBytes != 0U)
        {
            (void)NET_PRES_SocketRead(skt, NULL, peekBytes);   // discard the control message, not needed anymore
        }
        // all done
        retAct = (uint16_t)WSC_MSG_ACTION_DONE;
    }
    else
    {   // leave it pending, needs to be read by user!
        retAct = (uint16_t)WSC_MSG_ACTION_USR_TMO;
    }

    if(opCode == TCPIP_WS_CTRL_CODE_CLOSE)
    {   // server wants to close the connection
        if(ctrlLen == 0U)
        {
            pWsc->srvCloseCode = (uint16_t)TCPIP_WSC_CLOSE_NO_CODE; 
        }
        else
        {
            TCPIP_UINT16_VAL code;
            code.v[1] = pWsc->u_peekBuff[0]; 
            code.v[0] = pWsc->u_peekBuff[1]; 
            pWsc->srvCloseCode = code.Val;
        }
        pWsc->srvCloseRxed = 1;
        SetIntState(pWsc, WSC_INT_STAT_CLOSED);
        SetWscState(pWsc, TCPIP_WSC_CONN_STAT_CLOSED);
        retAct = (uint16_t)WSC_MSG_ACTION_DONE; // do not continue other processing 
    } 

    return retAct;
}

// if the whole payload is not available yet, we may need to discard as it becomes available
// return:  true if everything was discarded
//          false if more data needed or processing stopped
static bool WSC_DiscardFrame(TCPIP_WSC_CONN_CTRL* pWsc, TCPIP_WSC_RES dReason)
{
    TCPIP_WSC_PEND_MSG_DCPT* rxDcpt = pWsc->discardRxMsg;
    if(rxDcpt == NULL)
    {   // nothing to discard
        return true;
    }

    if(dReason != TCPIP_WSC_RES_MORE_DATA)
    {   // just starting the discard
        WSC_StartSrvWaitTimer(pWsc);
    }

    // bytes left to be discarded
    uint16_t dLeftBytes = rxDcpt->payloadLen - rxDcpt->renderedLen;

    NET_PRES_SKT_HANDLE_T skt = pWsc->netSocket;
    uint16_t avlblBytes = NET_PRES_SocketReadIsReady(skt);

   if(avlblBytes != 0U)
   {    // there is some data pending 
       uint16_t dLen;   // bytes to actually discard
       if(avlblBytes > dLeftBytes)
       {
           dLen = dLeftBytes;
       }
       else
       {
           dLen = avlblBytes;
       }

       (void)NET_PRES_SocketRead(skt, NULL, dLen);
       rxDcpt->renderedLen += dLen;
       bool dRes = rxDcpt->renderedLen == rxDcpt->payloadLen;
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_RX_DISCARD) != 0) 
       SYS_CONSOLE_PRINT("WSC Discard - dLen: %d, dRes: %d, reason: %d\r\n", dLen, dRes, dReason); 
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_RX_DISCARD) != 0) 
       if(dRes)
       {    // done
           rxDcpt->info.frameType = (uint8_t)TCPIP_WS_FRAME_TYPE_NONE; 
           pWsc->discardRxMsg = NULL;
       }
       else
       {    // check if server timeout
           if(WSC_IsSrvTmo(pWsc))
           {
               WSC_FailConnection(pWsc, TCPIP_WSC_RES_SRV_TMO);
           }
       }
       return dRes;
   }

   return false;
}

// updates the message descriptor with server data for this connection
// returns true if there's new data available for this message
// false otherwise
// P = payload (payloadLen);
// R = rendered to user (renderedLen)
// S = skt pending bytes for this message (sktPendLen) 
// SA = server available data so far (srvAvlblLen);
// D = new server data
// Initially: P is known, SA = S; R = 0;
// For server update:
//      server left to be read bytes = P - SA
//      n = skt available bytes
//      n = min(n, P - SA)
//      D = n - S -> new server data
//      if(D != 0) { SA += D; S = n;}
//
//      P == SA -> server message complete
//      P < SA  -> server data still needed
//      P < SA condition could be used for the server timeout condition
//          but only if skt RX buffer not full i.e. D == 0 && S == skt size
//
//      
// For user update:
//      user left to read: P - R
//      P == R -> user read complete
//
static bool WSC_SrvRxUpdate(TCPIP_WSC_CONN_CTRL* pWsc, TCPIP_WSC_PEND_MSG_DCPT* rxDcpt)
{
    // never exceed the payload length
    _TCPIPStack_Assert(rxDcpt->srvAvlblLen <= rxDcpt->payloadLen, __FILE__, __func__, __LINE__);

    // bytes left to be received from the server 
    uint16_t srvLeftBytes = rxDcpt->payloadLen - rxDcpt->srvAvlblLen;

    if(srvLeftBytes != 0U)
    {   // more data is needed from the server; check if available
        uint16_t sktBytes = NET_PRES_SocketReadIsReady(pWsc->netSocket);
        if(sktBytes > rxDcpt->payloadLen)
        {   // use only what belongs to this message
            sktBytes = rxDcpt->payloadLen;
        }
        // cannot go backwards!
        _TCPIPStack_Assert(sktBytes >= rxDcpt->sktPendLen, __FILE__, __func__, __LINE__);
        uint16_t newSrvData = sktBytes - rxDcpt->sktPendLen;    // what was last available
        if(newSrvData != 0U)
        {   // received some new data
            rxDcpt->srvAvlblLen += newSrvData;
            rxDcpt->sktPendLen = sktBytes;
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SRV_UPDATE) != 0)
            SYS_CONSOLE_PRINT("WSC RxUpdate - sktBytes: %d, payloadLen: %d, srvAvlblLen: %d, sktPendLen: %d, newSrvData: %d\r\n", sktBytes, rxDcpt->payloadLen, rxDcpt->srvAvlblLen, rxDcpt->sktPendLen, newSrvData);
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SRV_UPDATE) != 0)
            WSC_StartSrvWaitTimer(pWsc);    // new data available means the server is still active
            return true;
        }
    }

    return false;
}

// checks that the 2 connections share the same server IP address that
// It is assumed:
//      - both connections are in the 'connecting' state TCPIP_WSC_CONN_STAT_CONNECTING
//      - both use the the same port for connecting to the server
//  returns true for a match, false otherwise     
static bool WSC_ConnMatch(TCPIP_WSC_CONN_CTRL* pW1, TCPIP_WSC_CONN_CTRL* pW2)
{
    if(pW1->addType == pW2->addType)
    {
        if(pW1->addType == (uint8_t)IP_ADDRESS_TYPE_IPV4)
        {
            return (pW1->serverIP.v4Add.Val == pW2->serverIP.v4Add.Val);
        }
        // IPv6
        return (memcmp(pW1->serverIP.v6Add.v, pW2->serverIP.v6Add.v, sizeof(pW1->serverIP.v6Add)) == 0);
    }

    // 2 connections using different IP types: IPv4, IPv6 - are considered different 
    return false;
}

// internal function to send a control frame
// doesn't do state checks, etc.
static TCPIP_WSC_RES WSC_SendCtrlFrame(TCPIP_WSC_CONN_CTRL* pWsc, uint8_t opCode, uint8_t* payloadBuff, uint16_t payloadLen)
{
    WS_CLIENT_FRAME_CTRL cFrame;
    uint8_t maskBuff[WS_MASK_BUFFER_SIZE];  // masking buffer, for sending the data

    // check the WrIsReady space first!
    // if not available don't start or do not call!!!
    // write the frame header

    uint16_t frameLen = (uint16_t)sizeof(cFrame);
    uint16_t totLen = frameLen + payloadLen;
    // we should always have enough space for a complete control frame!
    _TCPIPStack_Assert(totLen <= (uint16_t)sizeof(maskBuff), __FILE__, __func__, __LINE__);

    // check for enough space; a control message should go all at once!
    NET_PRES_SKT_HANDLE_T skt = pWsc->netSocket;
    uint16_t avlblWrLen = NET_PRES_SocketWriteIsReady(skt, totLen, 0U);
    if(avlblWrLen < totLen)
    {   // not enough space; retry
        return TCPIP_WSC_RES_NO_WHOLE_SPACE;
    }

    // populate the frame
    WS_CLIENT_FRAME_CTRL* ctrlFrame = &cFrame;
    (void)memset(ctrlFrame, 0, sizeof(*ctrlFrame));
    ctrlFrame->hdr.fin = 1U;
    ctrlFrame->hdr.opcode = (uint8_t)opCode;
    ctrlFrame->hdr.mask = 1U;
    ctrlFrame->hdr.payLen = (uint8_t)payloadLen;

    // generate the mask; this is an independent, complete message!
    union
    {
        uint32_t    u32;
        uint8_t     u8[4];
    }U_MASK; // mask, LE order
    U_MASK.u32 = 0;
    (void)WSC_RngGenerate(U_MASK.u8, sizeof(U_MASK.u8));
    (void)memcpy(ctrlFrame->maskKey, U_MASK.u8, sizeof(U_MASK));

    // frame complete; send it as one message
    uint8_t* pDst = maskBuff;
    (void)memcpy(pDst, ctrlFrame->hdr.b8, frameLen);
    pDst += frameLen;
    // set the masked payload
    if(payloadLen != 0U)
    {
        size_t ix, maskIx;
        maskIx = 0;
        for(ix = 0; ix < payloadLen; ix++)
        {
            *pDst++ = *payloadBuff++ ^ U_MASK.u8[maskIx]; 
            if(++maskIx == 4U)
            {   // modulo 4
                maskIx = 0;
            }
        }
    }
    //

    uint16_t wrLen =  NET_PRES_SocketWrite(skt, maskBuff, totLen);
    if(wrLen != totLen)
    {   // fatal, should not happen!
        _TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
        {   // cannot recover from this...connection should be aborted!
            return TCPIP_WSC_RES_FATAL_ERROR; 
        }
    }

    return TCPIP_WSC_RES_OK;
}

// Sends data that's part of a small/medium frame!
// Returns the size of payload that was sent
// Note: in a m-threaded env, this is called from a user thread.
//      Using the pWsc->peek/scratch buffer could be tricky or a locking mechanism UserGblLock
//      should be used for this shared buffer access!
static size_t WSC_SendMsgFrame(TCPIP_WSC_CONN_CTRL* pWsc, WSC_PEND_TX_MSG* pTxMsg, TCPIP_WSC_RES* pRes) 
{
    uint16_t wrLen, totWrLen;
    NET_PRES_SKT_HANDLE_T skt = pWsc->netSocket;

    uint16_t payloadLen = pTxMsg->toSendBytes;

    if(pTxMsg->msgId == 0U)
    {   // brand new message; write the frame header
         TCPIP_WSC_RES res = WSC_WriteFrameHeader(pWsc, pTxMsg);
         if(res != TCPIP_WSC_RES_OK)
         {  // failure has occurred
             *pRes = res;
             return 0;
         }
    }

    // OK, countinue with the payload
    totWrLen = 0;

    while(payloadLen != 0U)
    {
        // mask the data
        uint8_t maskBuff[WS_MASK_BUFFER_SIZE];
        size_t chnkIx, ix;
        uint8_t* pDst;
        size_t nChunks = payloadLen / sizeof(maskBuff);         // complete size chunks
        size_t nRem = payloadLen - nChunks * sizeof(maskBuff);  // remainder: payloadLen % sizeof(maskBuff)
        const uint8_t* dataBuff = pTxMsg->msgData;
        bool sendDone = false;
        for(chnkIx = 0; chnkIx < nChunks; chnkIx++)
        {
            pDst = maskBuff;
            for(ix = 0; ix < sizeof(maskBuff); ix++)
            {
                *pDst++ = *dataBuff++ ^ pTxMsg->maskKey[pTxMsg->maskIx];
                if(++pTxMsg->maskIx == 4U)
                {   // modulo 4
                    pTxMsg->maskIx = 0;
                }
            }
            wrLen = NET_PRES_SocketWrite(skt, maskBuff, (uint16_t)sizeof(maskBuff));
            totWrLen += wrLen;
            if(wrLen != (uint16_t)sizeof(maskBuff))
            {   // could not write all data; abort
                sendDone = true;
                break;
            }
        }

        if(sendDone == false && nRem != 0U)
        {
            pDst = maskBuff;
            for(ix = 0; ix < nRem; ix++)
            {
                *pDst++ = *dataBuff++ ^ pTxMsg->maskKey[pTxMsg->maskIx]; 
                if(++pTxMsg->maskIx == 4U)
                {   // modulo 4
                    pTxMsg->maskIx = 0;
                }
            }
            totWrLen += NET_PRES_SocketWrite(skt, maskBuff, (uint16_t)nRem);
        }
        break;
    }
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_TX_FRAME) != 0)
    SYS_CONSOLE_PRINT("WSC TX send message - payload: %d, written: %d\r\n", payloadLen, totWrLen);
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_TX_FRAME) != 0)

    *pRes = TCPIP_WSC_RES_OK;
    return (size_t)totWrLen; 

}

// helper to write a frame header
static TCPIP_WSC_RES WSC_WriteFrameHeader(TCPIP_WSC_CONN_CTRL* pWsc, WSC_PEND_TX_MSG* pTxMsg)
{
    NET_PRES_SKT_HANDLE_T skt = pWsc->netSocket;

    union
    {
        WS_CLIENT_FRAME_CTRL    cFrame;
        WS_CLIENT_FRAME_SMALL   sFrame;
        WS_CLIENT_FRAME_MEDIUM  mFrame;
        uint8_t u8Buff[sizeof(WS_CLIENT_FRAME_MEDIUM)];
    }U_FRAME_BUFF;

    TCPIP_WS_FRAME_TYPE frameType;
    uint16_t frameLen;
    size_t payloadLen = pTxMsg->msgSize;

    if((pTxMsg->msgFlags & (uint8_t)TCPIP_WSC_MSG_FLAG_CTRL) != 0U) 
    {
        frameLen = (uint16_t)sizeof(WS_CLIENT_FRAME_CTRL);
        frameType = TCPIP_WS_FRAME_TYPE_CTRL; 
    }
    else if(payloadLen <= WS_FRAME_SMALL_MAX_PAYLEN)
    {
        frameLen = (uint16_t)sizeof(WS_CLIENT_FRAME_SMALL);
        frameType = TCPIP_WS_FRAME_TYPE_SMALL; 
    }
    else if(payloadLen <= WS_FRAME_MEDIUM_MAX_EXT_PAYLEN)
    {
        frameLen = (uint16_t)sizeof(WS_CLIENT_FRAME_MEDIUM);
        frameType = TCPIP_WS_FRAME_TYPE_MEDIUM; 
    }
    else
    {   // unsupported large frames (yet)
#if (M_WSC_LARGE_FRAME_SUPPORT == 0)
        _TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
        return TCPIP_WSC_RES_NOT_IMPLEMENTED;
#else
        frameType = TCPIP_WS_FRAME_TYPE_LARGE; 
#endif  // (M_WSC_LARGE_FRAME_SUPPORT == 0)
    } 

    // calculate the finCode
    unsigned int finCode;
    if((pTxMsg->msgFlags & (uint8_t)TCPIP_WSC_MSG_FLAG_CTRL) != 0U) 
    {
        finCode = 1;
    }
    else if((pTxMsg->msgFlags & (uint8_t)TCPIP_WSC_MSG_FLAG_FRAG_MID) != 0U) 
    {
        finCode = 0;
    }
    else if((pTxMsg->msgFlags & (uint8_t)TCPIP_WSC_MSG_FLAG_FRAG_START) != 0U) 
    {
        finCode = 0;
    }
    else if((pTxMsg->msgFlags & (uint8_t)TCPIP_WSC_MSG_FLAG_FRAG_END) != 0U) 
    {
        finCode = 1;
    }
    else
    {   // if no frag flags set
        finCode = 1;
    }

    // calculate the opCode:
    uint8_t opCode;
    if((pTxMsg->msgFlags & (uint8_t)TCPIP_WSC_MSG_FLAG_CTRL) != 0U) 
    {
        opCode = pTxMsg->ctrlCode;
    }
    else if((pTxMsg->msgFlags & (uint8_t)TCPIP_WSC_MSG_FLAG_FRAG_MID) != 0U) 
    {
        opCode = (uint8_t)TCPIP_WS_OPCODE_CONT_FRAME; 
    }
    else if((pTxMsg->msgFlags & (uint8_t)TCPIP_WSC_MSG_FLAG_BINARY) != 0U) 
    {
        opCode = (uint8_t)TCPIP_WS_OPCODE_BINARY_FRAME; 
    }
    else
    {
        opCode = (uint8_t)TCPIP_WS_OPCODE_TEXT_FRAME; 
    }


    // populate the frame
    (void)memset(U_FRAME_BUFF.u8Buff, 0, sizeof(U_FRAME_BUFF.u8Buff));
    if(frameType == TCPIP_WS_FRAME_TYPE_CTRL || frameType == TCPIP_WS_FRAME_TYPE_SMALL)
    {   // ctrl/small frame
        WS_CLIENT_FRAME_CTRL* cFrame = &U_FRAME_BUFF.cFrame;
        cFrame->hdr.fin = (uint8_t)finCode;
        cFrame->hdr.opcode = opCode;
        cFrame->hdr.mask = 1U;
        cFrame->hdr.payLen = (uint8_t)payloadLen;
        (void)memcpy(cFrame->maskKey, pTxMsg->maskKey, 4U);
    }
    else
    {
        // _TCPIPStack_Assert(frameType == TCPIP_WS_FRAME_TYPE_MEDIUM, __FILE__, __func__, __LINE__);
        WS_CLIENT_FRAME_MEDIUM* mFrame = &U_FRAME_BUFF.mFrame;
        mFrame->hdr.fin = (uint8_t)finCode;
        mFrame->hdr.opcode = opCode;
        mFrame->hdr.mask = 1U;
        mFrame->hdr.payLen = WS_FRAME_MEDIUM_PAYLEN;
        mFrame->extPayLen = TCPIP_Helper_htons((uint16_t)payloadLen); 
        (void)memcpy(mFrame->maskKey, pTxMsg->maskKey, 4U);
    }

    // send the frame header
    uint16_t wrLen =  NET_PRES_SocketWrite(skt, U_FRAME_BUFF.u8Buff, frameLen);
#if ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_TX_FRAME) != 0)
    SYS_CONSOLE_PRINT("WSC TX - send frame type: %d, hdr len: %d, out len: %d\r\n", frameType, frameLen, wrLen);
#endif  // ((WSC_DEBUG_LEVEL & WSC_DEBUG_MASK_SHOW_TX_FRAME) != 0)

    if(wrLen != frameLen)
    {   // fatal, should not happen!
        _TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
        if(wrLen == 0U)
        {   // OK, at least nothing went on the wire
            return TCPIP_WSC_RES_SKT_WR_ERROR; 
        }

        // cannot recover from this...connection should be aborted!
        return TCPIP_WSC_RES_FATAL_ERROR; 
    }

    return TCPIP_WSC_RES_OK; 
}

// returns true if the message delivered
static bool WSC_Notify_Event(TCPIP_WSC_CONN_CTRL* pWsc, TCPIP_WSC_EVENT_TYPE evType, TCPIP_WSC_EV_INFO evInfo)
{
    TCPIP_WSC_EVENT_HANDLER evHandler;
    const void* hParam;

    if(pWsc->evHandler != NULL)
    {
       evHandler = pWsc->evHandler;
       hParam = pWsc->hParam;
    }
    else
    {
        evHandler = pWsc->parent->evHandler;
        hParam = pWsc->parent->hParam;
    }

    if(evHandler != NULL)
    {   // fire the event
        evHandler(pWsc, evType, evInfo, hParam);
        return true;
    }
    return false;
}

// generate a random block of specified size
// returns 0 for success, < 0 for failure
int WSC_RngGenerate(uint8_t* rngBuff, size_t buffSize)
{
    CRYPT_RNG_CTX wsCtx;
    
    if(CRYPT_RNG_Initialize(&wsCtx) < 0)
    {   // failure ...
        _TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
        return -1;
    }

    int res = CRYPT_RNG_BlockGenerate(&wsCtx, rngBuff, buffSize);

    if (res < 0)
    {   // failure
        _TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
        return res;
    }
    return 0;
}


// helper
// find any of delim in str and return where found
static const char* strdelim(const char* str, const char* delim)
{
    const char* pD;
    while(*str != '\0')
    {
        pD = delim;
        while(*pD != '\0')
        {
            if(*pD++ == *str)
            {
                return str;
            }
        }
        str++;
    }

    return NULL;
}

// helper
// find any of delim in str and return where found starting from the end
static const char* strdelimr(const char* str, const char* delim)
{
    const char* pD;
    const char* pEndStr = str + strlen(str);

    while(pEndStr != str)
    {
        pD = delim;
        while(*pD != '\0')
        {
            if(*pD++ == *pEndStr)
            {
                return pEndStr;
            }
        }
        pEndStr--;
    }

    return NULL;
}

// helper
// return true if the character is any of the delim str
static bool strcany(const char* delim, int ch)
{
    const char* pD = delim;
    while(*pD != '\0')
    {
        if(*pD++ == ch)
        {
            return true;
        }
    }

    return false;
}

// searches the str for any occurrence of characters from delim
// returns a pointer to the first occurrence of a delim;
// if endDelim != NULL it will store the address past the last delimiter!
//      if multiple sequential delimiters, endDelim will pass them;
// Note: not using strtok as it is not thread safe!
static const char* strstrany(const char* str, const char* delim, const char** pEndDelim)
{
    const char* endDelim = NULL;
    const char* startDelim = strdelim(str, delim);
    if(startDelim != NULL && pEndDelim != NULL)
    {   // skip all other delimiters from then on
        endDelim = startDelim + 1;
        while(strcany(delim, (int)endDelim[0]))
        {
            endDelim++;
        }
    }

    if(pEndDelim != NULL)
    {
        *pEndDelim = endDelim;
    }

    return startDelim;
}


#endif  // defined(TCPIP_STACK_USE_WS_CLIENT)

