    /*******************************************************************************
  File Transfer Protocol (FTP) Server

  Summary:
    Module for Microchip TCP/IP Stack

  Description:
    - Provides ability to remotely upload MPFS2 image (web pages)
      to external EEPROM or external Flash memory
    - Reference: RFC 959
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

#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_FTP_SERVER
#include <stdarg.h>

#include "tcpip/src/tcpip_private.h"

#include "tcpip/src/common/sys_fs_shell.h"
#include "system/fs/src/sys_fs_local.h"

#include "net_pres/pres/net_pres_socketapi.h"

#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_FTP_SERVER)
#include "tcpip/src/ftp_private.h"

// local copy of the FTP configuration data
static TCPIP_FTP_MODULE_CONFIG_DATA ftpConfigData;

// Each entry in following table must match with that of FTP_COMMAND enum.
static const char * const sTCPIPFTPCmdString[] =
{
    "USER",                         // TCPIP_FTP_CMD_USER
    "PASS",                         // TCPIP_FTP_CMD_PASS
    "QUIT",                         // TCPIP_FTP_CMD_QUIT
    "STOR",                         // TCPIP_FTP_CMD_STOR
    "PORT",                         // TCPIP_FTP_CMD_PORT
    "ABOR",                         // TCPIP_FTP_CMD_ABORT
    "PWD",                          // TCPIP_FTP_CMD_PWD
    "CWD",                          // TCPIP_FTP_CMD_CWD
    "TYPE",                         // TCPIP_FTP_CMD_TYPE
    "RETR",                         // TCPIP_FTP_CMD_RETR // GET command
    "SIZE",                         // TCPIP_FTP_CMD_SIZE
    "PASV",                         // TCPIP_FTP_CMD_PASV
    "NLST",                         // TCPIP_FTP_CMD_NLST  // MGET command
    "EPSV",                         // TCPIP_FTP_CMD_EPSV
    "EPRT",                         // TCPIP_FTP_CMD_EPRT
    "LIST",                         // TCPIP_FTP_CMD_EXTND_LIST
    "XPWD",                         // TCPIP_FTP_CMD_XPWD
    "FEAT",                         // TCPIP_FTP_CMD_FEAT
    "SYST",                         // TCPIP_FTP_CMD_SYST
    "MDTM",                         // TCPIP_FTP_CMD_MDTM
    "MLST",                         // TCPIP_FTP_CMD_MLST
    "MLSD",                         // TCPIP_FTP_CMD_MLSD
    "DELE",                         // TCPIP_FTP_CMD_DELE
    "NOOP",                         // TCPIP_FTP_CMD_NOOP
    "MKD ",                         // TCPIP_FTP_CMD_MKD
    "XMKD",                         // TCPIP_FTP_CMD_XMKD
    "XRMD",                         // TCPIP_FTP_CMD_XRMD
    ""
};
#define TCPIP_FTP_CMD_TBL_SIZE  ( sizeof(sTCPIPFTPCmdString)/sizeof(sTCPIPFTPCmdString[0]) )

static PROTECTED_SINGLE_LIST      DirectoryFileList = { {0} };

// Each entry in following table must match with TCPIP_FTP_RESP enum
static const char * const sTCPIPFTPRespStr[] =
{
    "220 Ready\r\n",                    // TCPIP_FTP_RESP_BANNER
    "331 Password required\r\n",        // TCPIP_FTP_RESP_USER_OK
    "230 Logged in\r\n",                // TCPIP_FTP_RESP_PASS_OK
    "221 Bye\r\n",                      // TCPIP_FTP_RESP_QUIT_OK
    "500 Syntax error, command unrecognized\r\n",  // TCPIP_FTP_SYNTAX_ERROR
    "502 Not implemented\r\n",          // TCPIP_FTP_RESP_UNKNOWN
    "530 Login required\r\n",           // TCPIP_FTP_RESP_LOGIN
    "150 Transferring data...\r\n",     // TCPIP_FTP_RESP_DATA_OPEN
    "125 File status okay; about to open data connection\r\n",  // TCPIP_FTP_RESP_DATA_READY
    "226 Transfer Complete\r\n",        // TCPIP_FTP_RESP_DATA_CLOSE
    "425 Can't create data socket.\r\n",// TCPIP_FTP_RESP_DATA_NO_SOCKET
    "257 Current working Directory \r\n",   // TCPIP_FTP_RESP_PWD
    "200 Command Ok\r\n",                   // TCPIP_FTP_RESP_OK
    "550 Requested action not taken. \r\n", // TCPIP_FTP_RESP_NO_FILE_NO_ACTION
    "150 File status okay; about to open data connection\r\n", //TCPIP_FTP_RESP_FILE_IS_PRESENT
    "227 Entering passive mode ", // TCPIP_FTP_RESP_ENTER_PASV_MODE
    "226 Closing data connection. Requested file action successful.\r\n", //TCPIP_FTP_RESP_FILE_CLOSE_DATA_CONN,
    "213 ", // TCPIP_FTP_RESP_FILE_STATUS
    "522 Extended PORT failure - Unknown network protocol\r\n", //TCPIP_FTP_RESP_EXTND_PORT_FAILURE
    "421 Service not available, closing control connection\r\n", // TCPIP_FTP_RESP_FILESYSTEM_FAIL
    "215 UNIX Type: L8\r\n",  // TCPIP_FTP_RESP_SYST
    "250 Requested File Action Okay,Completed\r\n", // TCPIP_FTP_FILE_ACTION_OK
    "",  // TCPIP_FTP_RESP_NONE
};

static const char *month[]= {"Jan\0","Feb\0","Mar\0","Apr\0","May\0","Jun\0","Jul\0","Aug\0","Sep\0","Oct\0","Nov\0","Dec\0"};

// NLST arguments
#define TCPIP_FTP_MAX_NLST_ARGS  3
static uint8_t gFTPNLSTArgIndex=0xFF;
static uint8_t TCPIP_FTP_CMD_NLSTArgs[TCPIP_FTP_MAX_NLST_ARGS][5]={"-h\0","-a\0","-ha\0"};


static TCPIP_FTP_DCPT* sTCPIPFTPDcpt = NULL;

static int              sTCPIPFTPServerCount=0;
static const void*      sTCPIPFtpMemH = NULL;  // memory handle
static TCPIP_SIGNAL_HANDLE ftpsSignalHandle = NULL;


#if (M_FTP_USE_AUTHENTICATION_CALLBACK != 0)
static TCPIP_FTP_AUTH_HANDLER FTPAuthHandler;
static const void* FTPAuthHParam;
#endif  // (M_FTP_USE_AUTHENTICATION_CALLBACK != 0)


// Private helper functions.
static TCPIP_FTP_CMD TCPIP_FTP_CmdsParse(char *cmd);
static void TCPIP_FTP_CmdStringParse(TCPIP_FTP_DCPT* pFTPDcpt);
static bool TCPIP_FTP_CmdsExecute(TCPIP_FTP_CMD cmd, TCPIP_FTP_DCPT* pFTPDcpt);
#ifdef TCPIP_FTP_PUT_ENABLED
static bool TCPIP_FTP_FilePut(TCPIP_FTP_DCPT* pFTPDcpt);
#endif
static bool TCPIP_FTP_Quit(TCPIP_FTP_DCPT* pFTPDcpt);
static bool TCPIP_FTP_FileGet(TCPIP_FTP_DCPT* pFTPDcpt, char *cFile);
static bool TCPIP_FTP_MakeDirectory(TCPIP_FTP_DCPT* pFTPDcpt);
static bool TCPIP_FTP_ExecuteCmdGet(TCPIP_FTP_DCPT* pFTPDcpt, char *cFile);
static bool TCPIP_FTP_CmdList(TCPIP_FTP_DCPT* pFTPDcpt);
static bool TCPIP_FTP_LSCmd(TCPIP_FTP_DCPT* pFTPDcpt);
static SYS_FS_RESULT TCPIP_FTP_RemoveFile(TCPIP_FTP_DCPT * pFTPDcpt);

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void TCPIP_FTP_Cleanup(void);
#else
#define TCPIP_FTP_Cleanup()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static void F_FTP_ReleaseDataSocket(TCPIP_FTP_DCPT* pFTPDcpt);

static bool TCPIP_FTP_CreateDataSocket(TCPIP_FTP_DCPT* pFTPDcpt);
static bool TCPIP_FTP_Verify(TCPIP_FTP_DCPT* pFTPDcpt, const char* login,const char* password);
static bool F_FTP_Send_ErrorResponse(TCPIP_FTP_DCPT* pFTPDcpt);

static void TCPIP_FTP_ServerProcess(void);

static void F_FTPSocketRxSignalHandler(NET_PRES_SKT_HANDLE_T hTCP, NET_PRES_SIGNAL_HANDLE hNet, uint16_t sigType, const void* param);

// conversion functions/helpers
//

static __inline__ TCPIP_FTP_HANDLE __attribute__((always_inline)) FC_AuthHndl2FtpHndl(TCPIP_FTP_AUTH_HANDLER authHndl)
{
    union
    {
        TCPIP_FTP_AUTH_HANDLER authHndl;
        TCPIP_FTP_HANDLE  hndl;
    }U_AUTH_HNDL_FTP_HNDL;

    U_AUTH_HNDL_FTP_HNDL.authHndl = authHndl;
    return U_AUTH_HNDL_FTP_HNDL.hndl;
}

static __inline__ FTP_LIST_NODE*  __attribute__((always_inline)) FC_SglNode2FtpNode(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        FTP_LIST_NODE* ftpNode;
    }U_NODE_FTP_NODE;

    U_NODE_FTP_NODE.node = node;
    return U_NODE_FTP_NODE.ftpNode;
}

#if (M_FTP_USE_AUTHENTICATION_CALLBACK == 0)
#define TCPIP_FTP_ANNONYMOUS_USER_NAME "anonymous"
#endif

static const uint16_t ftpClientSignals = (uint16_t)TCPIP_TCP_SIGNAL_RX_DATA | (uint16_t)TCPIP_TCP_SIGNAL_TX_SPACE | (uint16_t)TCPIP_TCP_SIGNAL_RX_FIN;   // TCPIP_TCP_SIGNAL_TYPE

static SGL_LIST_NODE* TCPIP_FTP_LIST_Add(PROTECTED_SINGLE_LIST* FileList, TCPIP_STACK_HEAP_HANDLE heapH, size_t nBytes)
{
    SGL_LIST_NODE* newNode = TCPIP_HEAP_Malloc(heapH, nBytes);

    if(newNode != NULL)
    {
        TCPIP_Helper_ProtSglListTailAdd(FileList, newNode);
    }
    return newNode;

}

static bool TCPIP_FTP_Verify(TCPIP_FTP_DCPT* pFTPDcpt, const char* login, const char* password)
{
#if (M_FTP_USE_AUTHENTICATION_CALLBACK != 0)
    bool authRes = false;

    if(FTPAuthHandler != NULL)
    {
#if defined(TCPIP_FTPS_AUTHENTICATION_CONN_INFO) && (TCPIP_FTPS_AUTHENTICATION_CONN_INFO != 0)
        // we need connection info
        TCPIP_FTP_CONN_INFO connInfo;
        connInfo.connIx = FC_PtrDiff2UI16(pFTPDcpt - sTCPIPFTPDcpt);
        connInfo.presSkt = -1;
        connInfo.tcpSkt = pFTPDcpt->ftpCmdskt;
        (void)NET_PRES_SocketInfoGet(connInfo.tcpSkt, &connInfo.tcpInfo);
        authRes = (*FTPAuthHandler)(login, password, &connInfo, FTPAuthHParam);
#else
        authRes = (*FTPAuthHandler)(login, password, NULL, FTPAuthHParam);

#endif  // defined(TCPIP_FTPS_AUTHENTICATION_CONN_INFO) && (TCPIP_FTPS_AUTHENTICATION_CONN_INFO != 0)

    }

    return authRes;
#else
    // OBSOLETE authentication method
    // use the init time data

    if ( strncmp(login, (const char*)TCPIP_FTP_ANNONYMOUS_USER_NAME, TCPIP_FTP_USER_NAME_LEN) == 0 )
        return true;

    if ( strncmp(login, (const char*)ftpConfigData.userName, TCPIP_FTP_USER_NAME_LEN) == 0 )
    {
        if ( strncmp(password, ftpConfigData.password, TCPIP_FTP_PASSWD_LEN) == 0)
            return true;
    }
    return false;
#endif  // (M_FTP_USE_AUTHENTICATION_CALLBACK != 0)
}


static bool TCPIP_FTP_MakeDirectory(TCPIP_FTP_DCPT* pFTPDcpt)
{
    char ftpMsg[SYS_FS_FILE_NAME_LEN+10]; // 10 for extra parameter
    SYS_FS_RESULT fsRes;
  

    fsRes = (pFTPDcpt->ftp_shell_obj->dirMake)(pFTPDcpt->ftp_shell_obj, pFTPDcpt->ftp_argv[1]);
    if(fsRes != SYS_FS_RES_SUCCESS)
    {// No such directory
        pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NO_FILE_NO_ACTION;
        return true;
    }
    
    (void)FC_sprintf(ftpMsg, sizeof(ftpMsg), "250 %s is directory is added\r\n", pFTPDcpt->ftp_argv[1]);
    if(NET_PRES_SocketWriteIsReady(pFTPDcpt->ftpCmdskt, (uint16_t)strlen(ftpMsg), 0U) == 0U)
    {
        pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NONE;
        pFTPDcpt->ftpSm = (uint8_t)TCPIP_FTP_SM_CONNECTED;
        return true;
    }
    (void)NET_PRES_SocketWrite(pFTPDcpt->ftpCmdskt, (const uint8_t*)ftpMsg, (uint16_t)strlen(ftpMsg));
    (void)NET_PRES_SocketFlush(pFTPDcpt->ftpCmdskt);
    pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NONE;
    pFTPDcpt->ftpSm = (uint8_t)TCPIP_FTP_SM_CONNECTED;
    return true;
}


bool TCPIP_FTP_ServerInitialize(const TCPIP_STACK_MODULE_CTRL* const stackData, const void* initData)
{
    size_t  nServers;
    bool    initFail;
    const char* ftpLocalWebPath;
    TCPIP_FTP_DCPT* pDcpt = NULL;
    SYS_FS_SHELL_RES res;

    if(stackData->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
       return true;
    }

    // Check to see whether we have been brought up previously. If so, we will move on.
    if(sTCPIPFTPServerCount == 0)
    {
        // Check if there is any FTP server connection configured
        const TCPIP_FTP_MODULE_CONFIG* ftpData = (const TCPIP_FTP_MODULE_CONFIG*)initData;
        if(ftpData == NULL || ftpData->nConnections == 0U || ftpData->cmdPort == 0U || ftpData->dataPort == 0U)
        {
            SYS_ERROR(SYS_ERROR_ERROR, " FTP: Configuration data error");
            return false;
        }

        // state init
        ftpsSignalHandle = NULL;

        // basic sanity check
        ftpConfigData.cmdPort = ftpData->cmdPort;
        ftpConfigData.dataPort = ftpData->dataPort;
        ftpConfigData.nConnections = ftpData->nConnections;
        ftpConfigData.dataSktRxBuffSize = ftpData->dataSktRxBuffSize;
        ftpConfigData.dataSktTxBuffSize = ftpData->dataSktTxBuffSize;

#if (M_FTP_USE_AUTHENTICATION_CALLBACK == 0)
        (void)strncpy(ftpConfigData.userName, ftpData->userName, sizeof(ftpConfigData.userName) - 1);
        (void)strncpy(ftpConfigData.password, ftpData->password, sizeof(ftpConfigData.password) - 1);
#endif  // (M_FTP_USE_AUTHENTICATION_CALLBACK == 0)

        // Get the mount path from the Initialization data
        if(ftpData->mountPath != NULL)
        {
            ftpLocalWebPath = ftpData->mountPath;
        }
        else
        {
            ftpLocalWebPath = TCPIP_FTP_MOUNT_POINT;
        }

        sTCPIPFtpMemH = stackData->memH;

        sTCPIPFTPDcpt = (TCPIP_FTP_DCPT*)TCPIP_HEAP_Calloc(sTCPIPFtpMemH, (size_t)ftpConfigData.nConnections, sizeof(TCPIP_FTP_DCPT));
        if(sTCPIPFTPDcpt == NULL)
        {   // failed
            return false;
        }

        // initialize data structures
        initFail = false;
        pDcpt = sTCPIPFTPDcpt;
        for(nServers = 0; nServers < ftpConfigData.nConnections; nServers++)
        {
            pDcpt->ftpCmdskt = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_STREAM_SERVER, (NET_PRES_SKT_ADDR_T)IP_ADDRESS_TYPE_ANY, ftpConfigData.cmdPort, NULL, NULL);
            if(pDcpt->ftpCmdskt == NET_PRES_INVALID_SOCKET)
            {   // failed
                SYS_ERROR(SYS_ERROR_ERROR, " FTP: Socket creation failed");
                initFail = true;
                break;
            }
            // alert of incoming traffic
            pDcpt->ftpTcpCmdSocketSignal = NET_PRES_SocketSignalHandlerRegister(pDcpt->ftpCmdskt, (uint16_t)TCPIP_TCP_SIGNAL_RX_DATA, &F_FTPSocketRxSignalHandler, NULL);  
            if(pDcpt->ftpTcpCmdSocketSignal == NULL)
            {
                SYS_ERROR(SYS_ERROR_ERROR, " FTP: TCP Signal creation failed");
                initFail = true;
                break;
            }
            pDcpt->ftpFlag.val = 0;
            pDcpt->ftpDataPort = 0;
            pDcpt->ftpStringLen = 0U;
            pDcpt->ftpDataskt = NET_PRES_INVALID_SOCKET ;
            pDcpt->ftpSm = (uint8_t)TCPIP_FTP_SM_NOT_CONNECTED;
            pDcpt->ftpCommand = (uint8_t)TCPIP_FTP_CMD_NONE;
            pDcpt->ftpCommandSm = (uint8_t)TCPIP_FTP_CMD_SM_IDLE;
            pDcpt->callbackPos = 0UL;
            pDcpt->ftpTcpDataSocketSignal = NULL;
            pDcpt->fileDescr = SYS_FS_HANDLE_INVALID;

            pDcpt->ftp_shell_obj = SYS_FS_Shell_Create(ftpLocalWebPath, SYS_FS_SHELL_FLAG_NONE, NULL, NULL, &res);
            if(pDcpt->ftp_shell_obj == NULL)
            {
                SYS_ERROR(SYS_ERROR_ERROR, " FTP: Wrapper object failure : %d",res);
                initFail = true;
                break;
            }            
            pDcpt++;
        }

        if(initFail == false)
        {   // create the FTP timer
            ftpsSignalHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_FTP_ServerTask, TCPIP_FTPS_TASK_TICK_RATE);
            if(ftpsSignalHandle == NULL)
            {   // cannot create the FTP timer
                SYS_ERROR(SYS_ERROR_ERROR, " FTP: Signal Handler failed");
                initFail = true;
            }
        }

        if(initFail == true)
        {
            TCPIP_FTP_Cleanup();
            return false;
        }

#if (M_FTP_USE_AUTHENTICATION_CALLBACK != 0)
        FTPAuthHandler = NULL;
#endif  // (M_FTP_USE_AUTHENTICATION_CALLBACK != 0)
    }   

    sTCPIPFTPServerCount++;

    return true;
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_FTP_ServerDeinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData)
{
    if(sTCPIPFTPServerCount > 0)
    {
        if(stackData->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--sTCPIPFTPServerCount == 0)
            {   // all closed
                // release resources
                TCPIP_FTP_Cleanup();
            }
        }
    }
}

static void TCPIP_FTP_Cleanup(void)
{
    size_t nServers;
    TCPIP_FTP_DCPT* pFTPDcpt;

    if(sTCPIPFTPDcpt != NULL)
    {
        pFTPDcpt = sTCPIPFTPDcpt;
        for(nServers = 0; nServers < ftpConfigData.nConnections; nServers++)
        {
            if(pFTPDcpt->ftpTcpCmdSocketSignal != NULL)
            {
                (void)NET_PRES_SocketSignalHandlerDeregister(pFTPDcpt->ftpCmdskt, pFTPDcpt->ftpTcpCmdSocketSignal);
                pFTPDcpt->ftpTcpCmdSocketSignal = NULL;
            }
            if(pFTPDcpt->ftpCmdskt != NET_PRES_INVALID_SOCKET )
            {
                NET_PRES_SocketClose(pFTPDcpt->ftpCmdskt);
                pFTPDcpt->ftpCmdskt = NET_PRES_INVALID_SOCKET ;
            }
            // Wrapper object cleanup
            if(pFTPDcpt->ftp_shell_obj != NULL)
            {
                (void)(pFTPDcpt->ftp_shell_obj->delete)(pFTPDcpt->ftp_shell_obj);
                pFTPDcpt->ftp_shell_obj = NULL;
            }
            pFTPDcpt++;
        }
        
        (void)TCPIP_HEAP_Free(sTCPIPFtpMemH, sTCPIPFTPDcpt);
        sTCPIPFTPDcpt = NULL;
    }

    if(ftpsSignalHandle != NULL)
    {
        TCPIPStackSignalHandlerDeregister(ftpsSignalHandle);
        ftpsSignalHandle = NULL;
    }
    
    sTCPIPFtpMemH = NULL;
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static bool F_FTP_Send_ErrorResponse(TCPIP_FTP_DCPT* pFTPDcpt)
{
    // Make sure there is enough TCP TX FIFO space to put our response
    if (NET_PRES_SocketWriteIsReady(pFTPDcpt->ftpCmdskt, (uint16_t)strlen(sTCPIPFTPRespStr[pFTPDcpt->ftpResponse]), 0U) == 0U)
    {
        return false;
    }
    (void)NET_PRES_SocketWrite(pFTPDcpt->ftpCmdskt, (const uint8_t*)sTCPIPFTPRespStr[pFTPDcpt->ftpResponse], (uint16_t)strlen(sTCPIPFTPRespStr[pFTPDcpt->ftpResponse]));
    (void)NET_PRES_SocketFlush(pFTPDcpt->ftpCmdskt);

    if(pFTPDcpt->ftpDataskt != NET_PRES_INVALID_SOCKET )
    {
        F_FTP_ReleaseDataSocket(pFTPDcpt);  
    }
    return true;
}

void TCPIP_FTP_ServerTask(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != TCPIP_MODULE_SIGNAL_NONE)
    { // TMO or RX signals occurred
        TCPIP_FTP_ServerProcess();
    }
}


// send a signal to the FTP module that data is available
// no manager alert needed since this normally results as a higher layer (TCP) signal
static void F_FTPSocketRxSignalHandler(NET_PRES_SKT_HANDLE_T hTCP, NET_PRES_SIGNAL_HANDLE hNet, uint16_t sigType, const void* param)
{
    size_t nServers;
    TCPIP_FTP_DCPT* pFTPDcpt = NULL; //= (TCPIP_FTP_DCPT*)param;
    if((sigType & (uint16_t)TCPIP_TCP_SIGNAL_RX_DATA) != 0U)
    {
        (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
    for(nServers = 0; nServers < ftpConfigData.nConnections; nServers++)
    {
        pFTPDcpt = sTCPIPFTPDcpt + nServers;
        if(pFTPDcpt->ftpDataskt == (TCP_SOCKET)hTCP)
        {
            if(((uint16_t)sigType & (uint16_t)TCPIP_TCP_SIGNAL_RX_FIN) != 0U)
            {
                pFTPDcpt->ftpFlag.Bits.dataRxFin = 1U;
                break;
            }
        }
        else
        {
            continue;
        }
     }
}

static void TCPIP_FTP_ServerProcess(void)
{
    size_t      nServers;
    uint32_t    currentTick;
    TCPIP_FTP_DCPT* pFTPDcpt;
    bool disconRes;

    for(nServers = 0; nServers < ftpConfigData.nConnections; nServers++)
    {
        pFTPDcpt = sTCPIPFTPDcpt + nServers;
        if(pFTPDcpt->ftpCmdskt == NET_PRES_INVALID_SOCKET )
        {
            continue;
        }

        if((NET_PRES_SocketWasDisconnected(pFTPDcpt->ftpCmdskt))
                || (NET_PRES_SocketWasReset(pFTPDcpt->ftpCmdskt)))
        {
            if((disconRes = NET_PRES_SocketDisconnect(pFTPDcpt->ftpCmdskt)) == true)
            {              
                pFTPDcpt->ftpSm = (uint8_t)TCPIP_FTP_SM_NOT_CONNECTED;
                pFTPDcpt->ftpCommand = (uint8_t)TCPIP_FTP_CMD_NONE;
                pFTPDcpt->ftpFlag.val = 0;
                pFTPDcpt->ftpCommandSm = (uint8_t)TCPIP_FTP_CMD_SM_IDLE;
                if(pFTPDcpt->ftpDataskt != NET_PRES_INVALID_SOCKET )
                {
                    F_FTP_ReleaseDataSocket(pFTPDcpt);  
                }
                // close the file descriptor when the connection is getting closed 
                // during file transfer
                if(pFTPDcpt->fileDescr != SYS_FS_HANDLE_INVALID)
                {
                    (void)pFTPDcpt->ftp_shell_obj->fileClose(pFTPDcpt->ftp_shell_obj,pFTPDcpt->fileDescr);
                    pFTPDcpt->fileDescr = SYS_FS_HANDLE_INVALID;
                }
            }
            
            continue;
        }


        if (NET_PRES_SocketReadIsReady(pFTPDcpt->ftpCmdskt) != 0U)
        {
            pFTPDcpt->ftpSysTicklastActivity = SYS_TMR_TickCountGet();
            pFTPDcpt->ftpStringLen = (uint16_t)NET_PRES_SocketRead(pFTPDcpt->ftpCmdskt, (uint8_t*)pFTPDcpt->ftpCmdString, (uint16_t)TCPIP_FTP_CMD_MAX_STRING_LEN);
            if(pFTPDcpt->ftpStringLen >= (uint16_t)TCPIP_FTP_CMD_MAX_STRING_LEN)
            {   // reject overflow
                pFTPDcpt->ftpStringLen = 0U;
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_UNKNOWN;
                (void)F_FTP_Send_ErrorResponse(pFTPDcpt);
                continue;
            }
            if(pFTPDcpt->ftpStringLen != 0U && pFTPDcpt->ftpCmdString[pFTPDcpt->ftpStringLen - 1U] == '\n' )
            {
                pFTPDcpt->ftpCmdString[pFTPDcpt->ftpStringLen] = '\0';
                pFTPDcpt->ftpStringLen = 0U;
                TCPIP_FTP_CmdStringParse(pFTPDcpt);
                pFTPDcpt->ftpCommand = (uint8_t)TCPIP_FTP_CmdsParse(pFTPDcpt->ftp_argv[0]);
            }
        }
        else if(pFTPDcpt->ftpSm != (uint8_t)TCPIP_FTP_SM_NOT_CONNECTED )
        {
            currentTick = SYS_TMR_TickCountGet();
            currentTick = currentTick - pFTPDcpt->ftpSysTicklastActivity;
            if(currentTick >= ((uint32_t)TCPIP_FTP_TIMEOUT * SYS_TMR_TickCounterFrequencyGet()) )
            {
                pFTPDcpt->ftpSysTicklastActivity = SYS_TMR_TickCountGet();
                pFTPDcpt->ftpCommand    = (uint8_t)TCPIP_FTP_CMD_QUIT;
                pFTPDcpt->ftpSm  = (uint8_t)TCPIP_FTP_SM_CONNECTED;
                pFTPDcpt->ftpCommandSm = (uint8_t)TCPIP_FTP_CMD_SM_IDLE;
            }
        }
        else
        {
            // do nothing
        }

        if(pFTPDcpt->ftpSm == (uint8_t)TCPIP_FTP_SM_HOME || pFTPDcpt->ftpSm == (uint8_t)TCPIP_FTP_SM_NOT_CONNECTED || pFTPDcpt->ftpSm == (uint8_t)TCPIP_FTP_SM_RESPOND)
        {
            if(pFTPDcpt->ftpSm == (uint8_t)TCPIP_FTP_SM_HOME || pFTPDcpt->ftpSm == (uint8_t)TCPIP_FTP_SM_NOT_CONNECTED)
            {
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_BANNER;
                pFTPDcpt->ftpSysTicklastActivity = SYS_TMR_TickCountGet();
            }

            // Make sure there is enough TCP TX FIFO space to put our response
            if (NET_PRES_SocketWriteIsReady(pFTPDcpt->ftpCmdskt, (uint16_t)strlen(sTCPIPFTPRespStr[pFTPDcpt->ftpResponse]), 0U) != 0U)
            {
                (void)NET_PRES_SocketWrite(pFTPDcpt->ftpCmdskt, (const uint8_t*)sTCPIPFTPRespStr[pFTPDcpt->ftpResponse], (uint16_t)strlen(sTCPIPFTPRespStr[pFTPDcpt->ftpResponse]));
                (void)NET_PRES_SocketFlush(pFTPDcpt->ftpCmdskt);
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NONE;
                pFTPDcpt->ftpSm = (uint8_t)TCPIP_FTP_SM_CONNECTED;
            }
        }
        else if(pFTPDcpt->ftpSm == (uint8_t)TCPIP_FTP_SM_CONNECTED)
        {
            if(pFTPDcpt->ftpCommand != (uint8_t)TCPIP_FTP_CMD_NONE )
            {
                if(TCPIP_FTP_CmdsExecute((TCPIP_FTP_CMD)pFTPDcpt->ftpCommand, pFTPDcpt) )
                {
                    if(pFTPDcpt->ftpResponse != (uint8_t)TCPIP_FTP_RESP_NONE )
                    {
                        pFTPDcpt->ftpSm = (uint8_t)TCPIP_FTP_SM_RESPOND;
                    }
                    else if(pFTPDcpt->ftpCommand == (uint8_t)TCPIP_FTP_CMD_QUIT )
                    {
                        pFTPDcpt->ftpSm = (uint8_t)TCPIP_FTP_SM_NOT_CONNECTED;
                    }
                    else
                    {
                        // do nothing
                    }

                    pFTPDcpt->ftpCommand = (uint8_t)TCPIP_FTP_CMD_NONE;
                    pFTPDcpt->ftpCommandSm = (uint8_t)TCPIP_FTP_CMD_SM_IDLE;
                }
                else if(pFTPDcpt->ftpResponse != (uint8_t)TCPIP_FTP_RESP_NONE )
                {
                    pFTPDcpt->ftpSm = (uint8_t)TCPIP_FTP_SM_RESPOND;
                }
                else
                {
                    // do nothing
                }
            }
        }
        else
        {
            // do nothing
        }
    }
}

/*****************************************************************************
  Function:
    static bool TCPIP_FTP_Execute_Cmds(TCPIP_FTP_CMD cmd, TCPIP_FTP_DCPT* pFTPDcpt)

  Summary:
    Execute different FTP commands

  Description:
    Execute different FTP commands . Like UserName,Password,QUIT,GET,MGET. But PUT command
    is in place. It is not tested. FTp works on both Active and Passive mode.
  Precondition:
    None

  Parameters:
    cmd - FTP Command
    pFTPDcpt - FTP descriptor
  Returns:
    true or false

  Remarks:
    Users should not call this function directly.
  ***************************************************************************/
static bool TCPIP_FTP_CmdsExecute(TCPIP_FTP_CMD cmd, TCPIP_FTP_DCPT* pFTPDcpt)
{
    SYS_FS_HANDLE fp;
    TCP_SOCKET_INFO remoteSockInfo,dataSockInfo;
    char passiveMsg[64];
    int32_t fileSize;
    char tempMsg[20];
    SYS_FS_SHELL_RES fsWrapperRes, shellRes;
    uint32_t port32;
    char ftpMsg[SYS_FS_FILE_NAME_LEN + 18]; // 18 characters for extra parameter
    char cwd[SYS_FS_FILE_NAME_LEN+1];

    bool execRes =  true;
    switch(cmd)
    {
        case TCPIP_FTP_CMD_USER:
            pFTPDcpt->ftpFlag.Bits.userSupplied = 1U;
            pFTPDcpt->ftpFlag.Bits.loggedIn = 0U;
            pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_USER_OK;
            if(pFTPDcpt->ftp_argv[1] != NULL)
            {
                (void)strncpy(pFTPDcpt->ftpUserName, pFTPDcpt->ftp_argv[1], TCPIP_FTP_USER_NAME_LEN);
            }
            else
            {
                pFTPDcpt->ftpUserName[0] = '\0';    // empty user name
            }
        break;

        case TCPIP_FTP_CMD_PASS:
            if ( pFTPDcpt->ftpFlag.Bits.userSupplied == 0U)
            {
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_LOGIN;
            }
            else
            {
                if(TCPIP_FTP_Verify(pFTPDcpt, pFTPDcpt->ftpUserName, pFTPDcpt->ftp_argv[1]))
                {
                    pFTPDcpt->ftpFlag.Bits.loggedIn = 1U;
                    pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_PASS_OK;
                }
                else
                {
                    pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_LOGIN;
                }
            }
        break;

        case TCPIP_FTP_CMD_QUIT:
            execRes = TCPIP_FTP_Quit(pFTPDcpt);
            break;

        case TCPIP_FTP_CMD_PORT:

            (void)FC_Str2UL(pFTPDcpt->ftp_argv[5], 10, &port32);
            pFTPDcpt->ftpDataPort = (uint8_t)port32;
            (void)FC_Str2UL(pFTPDcpt->ftp_argv[6], 10, &port32);
            pFTPDcpt->ftpDataPort = (pFTPDcpt->ftpDataPort << 8) | (uint8_t)port32;
            pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_OK;
            break;

        case TCPIP_FTP_CMD_FEAT:
            (void)memset(ftpMsg,0,sizeof(ftpMsg));
            (void)FC_sprintf(ftpMsg, sizeof(ftpMsg), "211-features:\r\n");
            if (NET_PRES_SocketWriteIsReady(pFTPDcpt->ftpCmdskt, (uint16_t)strlen(ftpMsg), 0U) == 0U)
            {
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NONE;
                pFTPDcpt->ftpSm = (uint8_t)TCPIP_FTP_SM_CONNECTED;
                break;
            }
            (void)NET_PRES_SocketWrite(pFTPDcpt->ftpCmdskt, (const uint8_t*)ftpMsg, (uint16_t)strlen(ftpMsg));
            (void)NET_PRES_SocketFlush(pFTPDcpt->ftpCmdskt);
            // send SIZE
            (void)memset(ftpMsg,0,sizeof(ftpMsg));
            (void)FC_sprintf(ftpMsg, sizeof(ftpMsg), "SIZE\r\n");
            if (NET_PRES_SocketWriteIsReady(pFTPDcpt->ftpCmdskt, (uint16_t)strlen(ftpMsg), 0U) == 0U)
            {
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NONE;
                pFTPDcpt->ftpSm = (uint8_t)TCPIP_FTP_SM_CONNECTED;
                break;
            }
            (void)NET_PRES_SocketWrite(pFTPDcpt->ftpCmdskt, (const uint8_t*)ftpMsg, (uint16_t)strlen(ftpMsg));
            (void)NET_PRES_SocketFlush(pFTPDcpt->ftpCmdskt);

            // End of FEAT fetures support
            (void)memset(ftpMsg,0,sizeof(ftpMsg));
            (void)FC_sprintf(ftpMsg, sizeof(ftpMsg), "211 End\r\n");
            if (NET_PRES_SocketWriteIsReady(pFTPDcpt->ftpCmdskt, (uint16_t)strlen(ftpMsg), 0U) == 0U)
            {
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NONE;
                pFTPDcpt->ftpSm = (uint8_t)TCPIP_FTP_SM_CONNECTED;
                break;
            }
            (void)NET_PRES_SocketWrite(pFTPDcpt->ftpCmdskt, (const uint8_t*)ftpMsg, (uint16_t)strlen(ftpMsg));
            (void)NET_PRES_SocketFlush(pFTPDcpt->ftpCmdskt);

            pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NONE;
            pFTPDcpt->ftpSm = (uint8_t)TCPIP_FTP_SM_CONNECTED;
            break;

        case TCPIP_FTP_CMD_SYST:
            pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_SYST;
            break;

        case TCPIP_FTP_CMD_MLST:
        case TCPIP_FTP_CMD_MLSD:
        case TCPIP_FTP_CMD_MDTM:
            // do nothing
            break;

        case TCPIP_FTP_CMD_STOR:
        case TCPIP_FTP_CMD_XPWD:
        case TCPIP_FTP_CMD_PWD:
            if(cmd == TCPIP_FTP_CMD_STOR)
            {
#ifdef TCPIP_FTP_PUT_ENABLED
                execRes = TCPIP_FTP_FilePut(pFTPDcpt);
                break;
#else
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_OK;
#endif
            }

            (void)memset(cwd,0,sizeof(cwd));
            // Get current directory -
            shellRes = (pFTPDcpt->ftp_shell_obj->cwdGet)(pFTPDcpt->ftp_shell_obj,cwd,SYS_FS_FILE_NAME_LEN);
            if(shellRes != SYS_FS_SHELL_RES_OK)
            {
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NO_FILE_NO_ACTION;
                break;
            }
            (void)memset(ftpMsg,0,sizeof(ftpMsg));
            (void)FC_sprintf(ftpMsg, sizeof(ftpMsg), "257 \"%s\" is cwd\r\n",cwd);
            if (NET_PRES_SocketWriteIsReady(pFTPDcpt->ftpCmdskt, (uint16_t)strlen(ftpMsg), 0U) == 0U)
            {
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NONE;
                pFTPDcpt->ftpSm = (uint8_t)TCPIP_FTP_SM_CONNECTED;
                break;
            }
            (void)NET_PRES_SocketWrite(pFTPDcpt->ftpCmdskt, (const uint8_t*)ftpMsg, (uint16_t)strlen(ftpMsg));
            (void)NET_PRES_SocketFlush(pFTPDcpt->ftpCmdskt);
            pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NONE;
            pFTPDcpt->ftpSm = (uint8_t)TCPIP_FTP_SM_CONNECTED;
            break;

        case TCPIP_FTP_CMD_CWD:
            // check if the CWD is Same as the LOCAL_WEBSITE_PATH
            // change the directory to root path and then change path

            fsWrapperRes = (pFTPDcpt->ftp_shell_obj->cwdSet)(pFTPDcpt->ftp_shell_obj, pFTPDcpt->ftp_argv[1]);
            if(fsWrapperRes != SYS_FS_SHELL_RES_OK)
            {
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NO_FILE_NO_ACTION;
                break;
            }
            fsWrapperRes = (pFTPDcpt->ftp_shell_obj->cwdGet)(pFTPDcpt->ftp_shell_obj,cwd,SYS_FS_FILE_NAME_LEN);
            if(fsWrapperRes != SYS_FS_SHELL_RES_OK)
            {
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NO_FILE_NO_ACTION;
                break;
            }

            (void)FC_sprintf(ftpMsg, sizeof(ftpMsg), "250 %s is new cwd\r\n",cwd);
            if (NET_PRES_SocketWriteIsReady(pFTPDcpt->ftpCmdskt, (uint16_t)strlen(ftpMsg), 0U) == 0U)
            {
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NONE;
                pFTPDcpt->ftpSm = (uint8_t)TCPIP_FTP_SM_CONNECTED;
                break;
            }
            (void)NET_PRES_SocketWrite(pFTPDcpt->ftpCmdskt, (const uint8_t*)ftpMsg, (uint16_t)strlen(ftpMsg));
            (void)NET_PRES_SocketFlush(pFTPDcpt->ftpCmdskt);
            pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NONE;
            pFTPDcpt->ftpSm = (uint8_t)TCPIP_FTP_SM_CONNECTED;
            break;

        case TCPIP_FTP_CMD_XMKD:
        case TCPIP_FTP_CMD_MKD:
            execRes = TCPIP_FTP_MakeDirectory(pFTPDcpt);
            break;
       
        case TCPIP_FTP_CMD_TYPE:
            pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_OK;
            break;

        case TCPIP_FTP_CMD_ABORT:
            pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_OK;
            if (pFTPDcpt->ftpDataskt!= NET_PRES_INVALID_SOCKET )
            {
                F_FTP_ReleaseDataSocket(pFTPDcpt);  
            }
            break;

        case TCPIP_FTP_CMD_NLST:
            execRes = TCPIP_FTP_LSCmd(pFTPDcpt);
            break;

        case TCPIP_FTP_CMD_EXTND_LIST:
            execRes = TCPIP_FTP_CmdList(pFTPDcpt);
            break;

        case TCPIP_FTP_CMD_RETR:
            if(pFTPDcpt->callbackPos == 0UL)
            {
                fp = (pFTPDcpt->ftp_shell_obj->fileOpen)(pFTPDcpt->ftp_shell_obj, pFTPDcpt->ftp_argv[1],SYS_FS_FILE_OPEN_READ);
                if(fp == SYS_FS_HANDLE_INVALID)
                {// File not found, so abort
                    pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NO_FILE_NO_ACTION;
                    break;
                }
                (void)pFTPDcpt->ftp_shell_obj->fileClose(pFTPDcpt->ftp_shell_obj,fp);
            }
            execRes = TCPIP_FTP_ExecuteCmdGet(pFTPDcpt, pFTPDcpt->ftp_argv[1]);
            break;

        case TCPIP_FTP_CMD_SIZE:
            fp = (pFTPDcpt->ftp_shell_obj->fileOpen)(pFTPDcpt->ftp_shell_obj, pFTPDcpt->ftp_argv[1],SYS_FS_FILE_OPEN_READ);
            if(fp == SYS_FS_HANDLE_INVALID)
            {// File not found, so abort
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NO_FILE_NO_ACTION;
                break;
            }
            fileSize = pFTPDcpt->ftp_shell_obj->fileSize(pFTPDcpt->ftp_shell_obj,fp);
            (void)strcpy(tempMsg,"");
            (void)FC_sprintf(tempMsg, sizeof(tempMsg), "213 %u\r\n", fileSize);

            if (NET_PRES_SocketWriteIsReady(pFTPDcpt->ftpCmdskt, (uint16_t)strlen(tempMsg), 0U) == 0U)
            {
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NONE;
                pFTPDcpt->ftpSm = (uint8_t)TCPIP_FTP_SM_CONNECTED;
                (void)pFTPDcpt->ftp_shell_obj->fileClose(pFTPDcpt->ftp_shell_obj,fp);
                break;
            }
            (void)NET_PRES_SocketWrite(pFTPDcpt->ftpCmdskt, (const uint8_t*)tempMsg, (uint16_t)strlen(tempMsg));
            (void)NET_PRES_SocketFlush(pFTPDcpt->ftpCmdskt);
            
            pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NONE;
            pFTPDcpt->ftpSm = (uint8_t)TCPIP_FTP_SM_CONNECTED;
            (void)pFTPDcpt->ftp_shell_obj->fileClose(pFTPDcpt->ftp_shell_obj,fp);
            break;

        case TCPIP_FTP_CMD_PASV:
            // create a server socket with a available port number and send this port number to the client with Response string.
            pFTPDcpt->ftpDataskt = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_STREAM_SERVER, (NET_PRES_SKT_ADDR_T)IP_ADDRESS_TYPE_IPV4, 0, NULL, NULL);
            // Make sure that a valid socket was available and returned
            // If not, return with an error
            if(pFTPDcpt->ftpDataskt == NET_PRES_INVALID_SOCKET)
            {
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_DATA_NO_SOCKET;
                break;
            }

            // catch the RX/TX signals
            pFTPDcpt->ftpTcpDataSocketSignal = NET_PRES_SocketSignalHandlerRegister(pFTPDcpt->ftpDataskt, ftpClientSignals, &F_FTPSocketRxSignalHandler, NULL);
            if(pFTPDcpt->ftpTcpDataSocketSignal == NULL)
            {
                NET_PRES_SocketClose(pFTPDcpt->ftpDataskt);
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_DATA_NO_SOCKET;
                break;
            }
            // modify the response string.
            //response string should have server ip address and the new data port number.

            (void)NET_PRES_SocketInfoGet(pFTPDcpt->ftpCmdskt, &remoteSockInfo);
            (void)NET_PRES_SocketInfoGet(pFTPDcpt->ftpDataskt, &dataSockInfo);
            //prepare additional message IPaddress + Port
            (void)strcpy(passiveMsg,"");
            // add IPv4 address
            (void)FC_sprintf(passiveMsg, sizeof(passiveMsg), "227 Entering passive mode (%u,%u,%u,%u,%u,%u)\r\n",remoteSockInfo.localIPaddress.v4Add.v[0],
                                                            remoteSockInfo.localIPaddress.v4Add.v[1],
                                                            remoteSockInfo.localIPaddress.v4Add.v[2],
                                                            remoteSockInfo.localIPaddress.v4Add.v[3],
                                                            dataSockInfo.localPort>>8 & 0xFFU,
                                                            dataSockInfo.localPort & 0xFFU);
            pFTPDcpt->ftpDataPort = dataSockInfo.localPort;

            if (NET_PRES_SocketWriteIsReady(pFTPDcpt->ftpCmdskt, (uint16_t)strlen(passiveMsg), 0U) == 0U)
            {
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NONE;
                pFTPDcpt->ftpSm = (uint8_t)TCPIP_FTP_SM_CONNECTED;
                break;
            }
            (void)NET_PRES_SocketWrite(pFTPDcpt->ftpCmdskt, (const uint8_t*)passiveMsg, (uint16_t)strlen(passiveMsg));
            (void)NET_PRES_SocketFlush(pFTPDcpt->ftpCmdskt);
            
            pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NONE;
            pFTPDcpt->ftpSm = (uint8_t)TCPIP_FTP_SM_CONNECTED;
#if (TCPIP_TCP_DYNAMIC_OPTIONS != 0)
            if(ftpConfigData.dataSktTxBuffSize != 0U)
            {
                void* tcpBuffSize = FC_Uint2VPtr((uint32_t)ftpConfigData.dataSktTxBuffSize);
                if(NET_PRES_SocketOptionsSet(pFTPDcpt->ftpDataskt, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_TX_BUFF, tcpBuffSize)==false)
                {
                    execRes = false;
                    break;
                }
            }

            if(ftpConfigData.dataSktRxBuffSize != 0U)
            {
                void* tcpBuffSize = FC_Uint2VPtr((uint32_t)ftpConfigData.dataSktRxBuffSize);
                if(NET_PRES_SocketOptionsSet(pFTPDcpt->ftpDataskt, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_RX_BUFF, tcpBuffSize)==false)
                {
                    execRes = false;
                    break;
                }
            }
#endif  // (TCPIP_TCP_DYNAMIC_OPTIONS != 0)
            pFTPDcpt->ftpFlag.Bits.pasvMode = 1U;
            break;

        case TCPIP_FTP_CMD_EPSV:
            // RFC 2428 , EPSV<space><net-prt>  or EPSV<space>ALL need to be handled properly.
            // for time being If there is any argument return Error code 522. dont start any data communication.
            if(pFTPDcpt->ftp_argc > 1U)
            {
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_EXTND_PORT_FAILURE;
                break;
            }
            (void)NET_PRES_SocketInfoGet(pFTPDcpt->ftpCmdskt, &remoteSockInfo);
            // create a server socket with a available port number and send this port number to the client with Response string.
            pFTPDcpt->ftpDataskt = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_STREAM_SERVER, (NET_PRES_SKT_ADDR_T)remoteSockInfo.addressType, 0, NULL, NULL);
            // Make sure that a valid socket was available and returned
            // If not, return with an error
            if(pFTPDcpt->ftpDataskt == NET_PRES_INVALID_SOCKET)
            {
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_DATA_NO_SOCKET;
                break;
            }

            // catch the RX/TX signals
            pFTPDcpt->ftpTcpDataSocketSignal = NET_PRES_SocketSignalHandlerRegister(pFTPDcpt->ftpDataskt, ftpClientSignals, &F_FTPSocketRxSignalHandler, NULL);
            
            if(pFTPDcpt->ftpTcpDataSocketSignal == NULL)
            {
                NET_PRES_SocketClose(pFTPDcpt->ftpDataskt);
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_DATA_NO_SOCKET;
                break;
            }
            // modify the response string.
            //response string should have server ip address and the new data port number.

            (void)NET_PRES_SocketInfoGet(pFTPDcpt->ftpDataskt, &dataSockInfo);
            //prepare additional message IPaddress + Port
            (void)strcpy(passiveMsg,"");


            (void)FC_sprintf(passiveMsg, sizeof(passiveMsg), "229 Extended passive mode entered (|||%u|)\r\n",dataSockInfo.localPort);
            pFTPDcpt->ftpDataPort = dataSockInfo.localPort;

            if (NET_PRES_SocketWriteIsReady(pFTPDcpt->ftpCmdskt, (uint16_t)strlen(passiveMsg), 0U) == 0U)
            {
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NONE;
                pFTPDcpt->ftpSm = (uint8_t)TCPIP_FTP_SM_CONNECTED;
                break;
            }
            (void)NET_PRES_SocketWrite(pFTPDcpt->ftpCmdskt, (const uint8_t*)passiveMsg, (uint16_t)strlen(passiveMsg));
            (void)NET_PRES_SocketFlush(pFTPDcpt->ftpCmdskt);
            pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NONE;
            pFTPDcpt->ftpSm = (uint8_t)TCPIP_FTP_SM_CONNECTED;
#if (TCPIP_TCP_DYNAMIC_OPTIONS != 0)
            if(ftpConfigData.dataSktTxBuffSize != 0U)
            {
                void* tcpBuffSize = FC_Uint2VPtr((uint32_t)ftpConfigData.dataSktTxBuffSize);
                if(NET_PRES_SocketOptionsSet(pFTPDcpt->ftpDataskt, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_TX_BUFF, tcpBuffSize)==false)
                {
                    execRes = false;
                    break;
                }
            }

            if(ftpConfigData.dataSktRxBuffSize != 0U)
            {
                void* tcpBuffSize = FC_Uint2VPtr((uint32_t)ftpConfigData.dataSktRxBuffSize);
                if(NET_PRES_SocketOptionsSet(pFTPDcpt->ftpDataskt, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_RX_BUFF, tcpBuffSize)==false)
                {
                    execRes = false;
                    break;
                }
            }
#endif  // (TCPIP_TCP_DYNAMIC_OPTIONS != 0)
            pFTPDcpt->ftpFlag.Bits.pasvMode = 1U;
            break;

        case TCPIP_FTP_CMD_EPRT:
            (void)FC_Str2UL(pFTPDcpt->ftp_argv[1], 10, &port32);
            pFTPDcpt->adressFamilyProtocol = (uint16_t)port32;
            (void)FC_Str2UL(pFTPDcpt->ftp_argv[3], 10, &port32);
            pFTPDcpt->ftpDataPort = (uint16_t)port32;
            pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_OK;
            break;

        case TCPIP_FTP_CMD_XRMD:
        case TCPIP_FTP_CMD_DELE: // remove a file or a directory
            if(pFTPDcpt->ftpFlag.Bits.loggedIn != 0U)
            {                
                SYS_FS_RESULT   result = TCPIP_FTP_RemoveFile(pFTPDcpt);                

                if(result == SYS_FS_RES_SUCCESS)
                {
                    pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_FILE_ACTION_OK;
                }
                else
                {
                    pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NO_FILE_NO_ACTION;
                }
            }
            else
            {
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_LOGIN;
            }

            break;

     case TCPIP_FTP_CMD_NOOP:
        if(pFTPDcpt->ftpFlag.Bits.loggedIn != 0U)
        {
            pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_OK;
        }
        else
        {
            pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_LOGIN;
        }
        break;

    default:
        pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_UNKNOWN;
        break;

    }
    return execRes;
}

static bool TCPIP_FTP_Quit(TCPIP_FTP_DCPT* pFTPDcpt)
{
    switch(pFTPDcpt->ftpCommandSm)
    {
        case (uint8_t)TCPIP_FTP_CMD_SM_IDLE:
            if(pFTPDcpt->ftpCommandSm == (uint8_t)TCPIP_FTP_CMD_SM_RECEIVE)
            {
                (void)pFTPDcpt->ftp_shell_obj->fileClose(pFTPDcpt->ftp_shell_obj,pFTPDcpt->fileDescr);
            }

            if(pFTPDcpt->ftpDataskt != NET_PRES_INVALID_SOCKET )
            {
                if(pFTPDcpt->ftpTcpDataSocketSignal != NULL)
                {
                    (void)NET_PRES_SocketSignalHandlerDeregister(pFTPDcpt->ftpDataskt, pFTPDcpt->ftpTcpDataSocketSignal);
                    pFTPDcpt->ftpTcpDataSocketSignal = NULL;
                }
                (void)pFTPDcpt->ftp_shell_obj->fileClose(pFTPDcpt->ftp_shell_obj,pFTPDcpt->fileDescr);
                NET_PRES_SocketClose(pFTPDcpt->ftpDataskt);
                pFTPDcpt->ftpCommandSm = (uint8_t)TCPIP_FTP_CMD_SM_WAIT;
            }
            else
            {
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_QUIT_OK;
                pFTPDcpt->ftpCommandSm = (uint8_t)TCPIP_FTP_CMD_SM_WAIT_FOR_DISCONNECT;
            }
            break;

        case (uint8_t)TCPIP_FTP_CMD_SM_WAIT:
            if(!NET_PRES_SocketIsConnected(pFTPDcpt->ftpDataskt) )
            {
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_QUIT_OK;
                pFTPDcpt->ftpCommandSm = (uint8_t)TCPIP_FTP_CMD_SM_WAIT_FOR_DISCONNECT;
            }
            break;

        case (uint8_t)TCPIP_FTP_CMD_SM_WAIT_FOR_DISCONNECT:
            if (NET_PRES_SocketWriteIsReady(pFTPDcpt->ftpCmdskt, 1U, 0U) != 0U)
            {
                if(NET_PRES_SocketIsConnected(pFTPDcpt->ftpCmdskt) )
                {
                    (void)NET_PRES_SocketDisconnect(pFTPDcpt->ftpCmdskt);
                }
            }
            break;
        default:
            // do nothing
            break;

    }
    return false;
}
/*
 * Release FTP data socket and also release signal.
 */
static void F_FTP_ReleaseDataSocket(TCPIP_FTP_DCPT* pFTPDcpt)
{
    if(pFTPDcpt->ftpTcpDataSocketSignal != NULL)
    {
        (void)NET_PRES_SocketSignalHandlerDeregister(pFTPDcpt->ftpDataskt, pFTPDcpt->ftpTcpDataSocketSignal);
        pFTPDcpt->ftpTcpDataSocketSignal = NULL;
    }
    if(pFTPDcpt->ftpDataskt != NET_PRES_INVALID_SOCKET )
    {
        NET_PRES_SocketClose(pFTPDcpt->ftpDataskt);
    }
    pFTPDcpt->ftpDataskt = NET_PRES_INVALID_SOCKET ;
}

static bool TCPIP_FTP_CreateDataSocket(TCPIP_FTP_DCPT* pFTPDcpt)
{
    TCP_SOCKET_INFO remoteSockInfo;
    bool releaseDataSkt=false;

    (void)NET_PRES_SocketInfoGet(pFTPDcpt->ftpCmdskt, &remoteSockInfo);
    pFTPDcpt->ftpDataskt = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_STREAM_CLIENT, (NET_PRES_SKT_ADDR_T)remoteSockInfo.addressType, pFTPDcpt->ftpDataPort, NULL, NULL);

    // Make sure that a valid socket was available and returned
    // If not, return with an error
    if(pFTPDcpt->ftpDataskt == NET_PRES_INVALID_SOCKET)
    {
        pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_DATA_NO_SOCKET;
        return false;
    }
    // catch the RX/TX signals
    pFTPDcpt->ftpTcpDataSocketSignal = NET_PRES_SocketSignalHandlerRegister(pFTPDcpt->ftpDataskt, ftpClientSignals, &F_FTPSocketRxSignalHandler, NULL);    
    
    if(pFTPDcpt->ftpTcpDataSocketSignal == NULL)
    {
        releaseDataSkt = true;
    }
    
        
    if(NET_PRES_SocketBind(pFTPDcpt->ftpDataskt, (NET_PRES_SKT_ADDR_T)remoteSockInfo.addressType, ftpConfigData.dataPort, NULL) == false)
    {
        releaseDataSkt = true;       
    }
    
    if(NET_PRES_SocketRemoteBind(pFTPDcpt->ftpDataskt, (NET_PRES_SKT_ADDR_T)remoteSockInfo.addressType, 0, FC_MultiAdd2PresAdd(&remoteSockInfo.remoteIPaddress))== false)
    {
        releaseDataSkt = true;        
    }    
   
    if(releaseDataSkt)
    {
        pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_DATA_NO_SOCKET;
        F_FTP_ReleaseDataSocket(pFTPDcpt);
        return false;
    }
    
    // Establish a connection
    if(NET_PRES_SocketConnect(pFTPDcpt->ftpDataskt) == false)
    {
        return false;
    }
#if (TCPIP_TCP_DYNAMIC_OPTIONS != 0)
    if(ftpConfigData.dataSktTxBuffSize != 0U)
    {
        void* tcpBuffSize = FC_Uint2VPtr((uint32_t)ftpConfigData.dataSktTxBuffSize);
        if(NET_PRES_SocketOptionsSet(pFTPDcpt->ftpDataskt, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_TX_BUFF, tcpBuffSize)==false)
        {
            return false;
        }
    }

    if(ftpConfigData.dataSktRxBuffSize != 0U)
    {
        void* tcpBuffSize = FC_Uint2VPtr((uint32_t)ftpConfigData.dataSktRxBuffSize);
        if(NET_PRES_SocketOptionsSet(pFTPDcpt->ftpDataskt, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_RX_BUFF, tcpBuffSize)==false)
        {
            return false;
        }
    }
#endif  // (TCPIP_TCP_DYNAMIC_OPTIONS != 0)
    
   
    return true;
}

#if defined(TCPIP_FTP_PUT_ENABLED)
static bool TCPIP_FTP_FilePut(TCPIP_FTP_DCPT* pFTPDcpt)
{
    uint8_t data[512]; // write buffer
    SYS_FS_HANDLE fp;
    uint16_t wCount, wLen;
    int32_t status;

    fp = pFTPDcpt->fileDescr;
    if(pFTPDcpt->ftpCommandSm == (uint8_t)TCPIP_FTP_CMD_SM_IDLE)
    {
        if ( pFTPDcpt->ftpFlag.Bits.loggedIn == 0U )
        {
            pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_LOGIN;
            return true;
        }

        if(pFTPDcpt->ftpFlag.Bits.pasvMode != 1U)
        {
            if(TCPIP_FTP_CreateDataSocket(pFTPDcpt) != true)
            {
                return true;
            }
        }
        pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_FILE_IS_PRESENT;
        pFTPDcpt->ftpCommandSm = (uint8_t)TCPIP_FTP_CMD_SM_WAIT;
        return false;
    }

    if(pFTPDcpt->ftpCommandSm == (uint8_t)TCPIP_FTP_CMD_SM_WAIT || pFTPDcpt->ftpCommandSm == (uint8_t)TCPIP_FTP_CMD_SM_RECEIVE)
    {
        if(pFTPDcpt->ftpCommandSm == (uint8_t)TCPIP_FTP_CMD_SM_WAIT)
        {
            if(NET_PRES_SocketIsConnected(pFTPDcpt->ftpDataskt) && (pFTPDcpt->callbackPos == 0UL))
            {
                fp = (pFTPDcpt->ftp_shell_obj->fileOpen)(pFTPDcpt->ftp_shell_obj, pFTPDcpt->ftp_argv[1], SYS_FS_FILE_OPEN_WRITE);
                if(fp == SYS_FS_HANDLE_INVALID)
                {// File not found, so abort
                    pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NO_FILE_NO_ACTION;
                    F_FTP_ReleaseDataSocket(pFTPDcpt);
                    return false;;
                }
                pFTPDcpt->fileDescr = fp;
                pFTPDcpt->ftpCommandSm    = (uint8_t)TCPIP_FTP_CMD_SM_RECEIVE;
            }
            else
            {
                return false;
            }
        }

        if(NET_PRES_SocketIsConnected(pFTPDcpt->ftpDataskt) && (pFTPDcpt->callbackPos != 0UL))
        {// The file was already opened, so load up its ID and seek
            if(fp == SYS_FS_HANDLE_INVALID)
            {// No file handles available, so wait for now
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_FILESYSTEM_FAIL;
                pFTPDcpt->callbackPos = 0;
                F_FTP_ReleaseDataSocket(pFTPDcpt);                    
                return true;
            }
            (void)pFTPDcpt->ftp_shell_obj->fileSeek(pFTPDcpt->ftp_shell_obj,fp,(int32_t)pFTPDcpt->callbackPos,SYS_FS_SEEK_SET);
        }
        else if(!NET_PRES_SocketIsConnected(pFTPDcpt->ftpDataskt))
        {
            // If no bytes were read, an EOF was reached
            (void)pFTPDcpt->ftp_shell_obj->fileClose(pFTPDcpt->ftp_shell_obj,fp);
            pFTPDcpt->fileDescr = SYS_FS_HANDLE_INVALID;
            pFTPDcpt->callbackPos = 0;
            pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_FILE_CLOSE_DATA_CONN;
            pFTPDcpt->ftpFlag.Bits.pasvMode = 0U;
            F_FTP_ReleaseDataSocket(pFTPDcpt);                    
            return true;
        }
        else
        {
            // do nothing
        }

        if(pFTPDcpt->ftpFlag.Bits.dataRxFin == 1U)
        {
            pFTPDcpt->ftpFlag.Bits.endCommunication = 1U;
        }

        // Get/put as many bytes as possible
        while(true)
        {
            wCount = NET_PRES_SocketReadIsReady(pFTPDcpt->ftpDataskt);
            if(wCount == 0U)
            {
                if(pFTPDcpt->ftpFlag.Bits.endCommunication == 1U)
                {
                    pFTPDcpt->ftpFlag.Bits.endCommunication = 0U;
                    (void)SYS_FS_FileClose(pFTPDcpt->fileDescr);
                    pFTPDcpt->fileDescr = SYS_FS_HANDLE_INVALID;
                    pFTPDcpt->callbackPos = 0;
                    pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_DATA_CLOSE;
                    pFTPDcpt->ftpFlag.Bits.pasvMode = 0U;
                    F_FTP_ReleaseDataSocket(pFTPDcpt);
                    pFTPDcpt->ftpFlag.Bits.dataRxFin = 0U;
                    return true;
                }
                break;
            }
            (void)memset(data, 0, sizeof(data));
            wLen = NET_PRES_SocketRead(pFTPDcpt->ftpDataskt, data, FC_MinU16(wCount, (uint16_t)sizeof(data)));
            if(wLen == 0U)
            {// If no bytes were read, an EOF was reached
                (void)pFTPDcpt->ftp_shell_obj->fileClose(pFTPDcpt->ftp_shell_obj,fp);
                pFTPDcpt->fileDescr = SYS_FS_HANDLE_INVALID;
                pFTPDcpt->callbackPos = 0;
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_DATA_CLOSE;
                pFTPDcpt->ftpFlag.Bits.pasvMode = 0U;
                F_FTP_ReleaseDataSocket(pFTPDcpt);                    
                return true;
            }
            else  //if(wLen != 0)
            {// Write the bytes to the socket
                if(pFTPDcpt->ftp_shell_obj->fileWrite(pFTPDcpt->ftp_shell_obj,fp,data,wLen) == SYS_FS_HANDLE_INVALID)
                {
                    (void)pFTPDcpt->ftp_shell_obj->fileClose(pFTPDcpt->ftp_shell_obj,fp);
                    pFTPDcpt->fileDescr = SYS_FS_HANDLE_INVALID;
                    pFTPDcpt->callbackPos = 0;
                    pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_DATA_CLOSE;
                    pFTPDcpt->ftpFlag.Bits.pasvMode = 0U;
                    F_FTP_ReleaseDataSocket(pFTPDcpt);                    
                    return true;
                }
                else
                {
                    pFTPDcpt->ftpSysTicklastActivity = SYS_TMR_TickCountGet();
                }
            }
        }
        status = pFTPDcpt->ftp_shell_obj->fileTell(pFTPDcpt->ftp_shell_obj,fp);
        if(status == -1)
        {
            pFTPDcpt->callbackPos = 0;
        }
        else
        {
            pFTPDcpt->callbackPos = (uint32_t)status;
        }
        return false;
    }

    return false;
}
#endif

static bool TCPIP_FTP_CmdList(TCPIP_FTP_DCPT* pFTPDcpt)
{
    uint16_t wCount=0;
    char longFileName[SYS_FS_FILE_NAME_LEN];
    static char fileNameList[TCPIP_FTP_MAX_FILE_NAME_LEN +
                    TCPIP_FTP_MAX_FILE_DATE_TIME_STR_LEN +
                    TCPIP_FTP_MAX_FILE_SIZE_STR_LEN +
                    + 40];
    char FileRecordsDateTime[TCPIP_FTP_MAX_FILE_DATE_TIME_STR_LEN];
    char fileRecrdTime[13];
    char FileRecordssize[TCPIP_FTP_MAX_FILE_SIZE_STR_LEN];
    const char fileHeaderStr[]= "Date\t\t\tType\tFileSize\tfilename\t\r\n\0";
    
    uint8_t FileRecordInformation;
    FTP_LIST_NODE* newNode=NULL;
    uint16_t    lfNameLen = 0U;
    char    *lfnamePtr = NULL;
    static uint16_t  remainingBytes=0;
    SYS_FS_FSTAT fs_stat={0};
    SYS_FS_HANDLE fp;
    char filePermission[2][11]={"-rwx------\0","drwx------\0"};
    const char *link = "0\0";
    const char *owner = "0\0";
    const char *group = "0\0";

    bool cmdRes = false;
    bool doBreak = false;

    switch(pFTPDcpt->ftpCommandSm)
    {
        case (uint8_t)TCPIP_FTP_CMD_SM_IDLE:
            if(pFTPDcpt->ftpFlag.Bits.pasvMode != 1U)
            {
                if(TCPIP_FTP_CreateDataSocket(pFTPDcpt) != true)
                {
                    cmdRes = true;
                    break;
                }
            }
            if ( pFTPDcpt->ftpFlag.Bits.loggedIn  == 0U)
            {
                pFTPDcpt->ftpResponse    = (uint8_t)TCPIP_FTP_RESP_LOGIN;
                F_FTP_ReleaseDataSocket(pFTPDcpt);                    
                cmdRes = true;
                break;
            }
            pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_DATA_READY;
            pFTPDcpt->ftpCommandSm = (uint8_t)TCPIP_FTP_CMD_SM_WAIT;
            break;

        case (uint8_t)TCPIP_FTP_CMD_SM_WAIT:
            if(NET_PRES_SocketIsConnected(pFTPDcpt->ftpDataskt))
            {
                pFTPDcpt->ftpCommandSm   = (uint8_t)TCPIP_FTP_CMD_SM_SEND_DIR;
            }
            break;

       case (uint8_t)TCPIP_FTP_CMD_SM_SEND_DIR:
            // get the current working directory to get the directory details
            // used longFileName variable to get the path details
            fp = (pFTPDcpt->ftp_shell_obj->dirOpen)(pFTPDcpt->ftp_shell_obj,".");
            if (fp == SYS_FS_HANDLE_INVALID)
            {   // File not found, so abort
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NO_FILE_NO_ACTION;
                F_FTP_ReleaseDataSocket(pFTPDcpt);                    
                cmdRes = true;
                break;
            }
            pFTPDcpt->fileDescr = fp;
            pFTPDcpt->callbackPos = 0UL;
            pFTPDcpt->ftpCommandSm  = (uint8_t)TCPIP_FTP_CMD_SM_SEND_DIR_HEADER;
            break;

        case (uint8_t)TCPIP_FTP_CMD_SM_SEND_DIR_HEADER:
            wCount = NET_PRES_SocketWriteIsReady(pFTPDcpt->ftpDataskt, (uint16_t)strlen(fileNameList), 1U);
             // send the Header details of the table
            if(pFTPDcpt->callbackPos != 0UL)
            {
                if(wCount < remainingBytes)
                {
                    (void)NET_PRES_SocketWrite(pFTPDcpt->ftpDataskt, fileNameList+(strlen(fileNameList) - (size_t)remainingBytes), wCount);
                    remainingBytes = remainingBytes - wCount;
                }
                else
                {
                    (void)NET_PRES_SocketWrite(pFTPDcpt->ftpDataskt, fileNameList+(strlen(fileNameList) - (size_t)remainingBytes), remainingBytes);
                    remainingBytes = 0U;
                    pFTPDcpt->callbackPos = 0UL;
                }
                break;
            }
            else
            {
                (void)memset(fileNameList,0,sizeof(fileNameList));
                (void)strncpy(fileNameList, fileHeaderStr, FC_MinU16((uint16_t)sizeof(fileNameList), (uint16_t)sizeof(fileHeaderStr)));
                if(wCount > (uint16_t)strlen(fileNameList))
                {
                    remainingBytes = 0 ;
                    pFTPDcpt->callbackPos = 0UL;
                }
                else
                {
                    pFTPDcpt->callbackPos = 1UL;
                    (void)NET_PRES_SocketWrite(pFTPDcpt->ftpDataskt, fileNameList, FC_MinU16(wCount, (uint16_t)strlen(fileNameList)));
                    remainingBytes = (uint16_t)strlen(fileNameList)- wCount;
                    break;
                }
            }
            // If long file name is used, the following elements of the "stat"
             // structure needs to be initialized with address of proper buffer.
            (void)memset(longFileName,0,sizeof(longFileName));
            fs_stat.lfname = longFileName;
            fs_stat.lfsize = SYS_FS_FILE_NAME_LEN;
            fp = pFTPDcpt->fileDescr;
            if(TCPIP_Helper_ProtSglListInitialize(&DirectoryFileList)!= true)
            {
                cmdRes = true;
                break;
            }
            while(true)
            {
                (void)memset(fs_stat.fname,0,13);
                (void)memset(longFileName,0,sizeof(longFileName));
                if(SYS_FS_DirSearch(fp, "*", SYS_FS_ATTR_FILE, &fs_stat) == SYS_FS_RES_FAILURE)
                {
                    (void)pFTPDcpt->ftp_shell_obj->dirClose(pFTPDcpt->ftp_shell_obj,fp);
                    pFTPDcpt->fileDescr = SYS_FS_HANDLE_INVALID;
                    break;
                }
                newNode = FC_SglNode2FtpNode(TCPIP_FTP_LIST_Add(&DirectoryFileList,sTCPIPFtpMemH , sizeof(*newNode)));
                if(newNode == NULL)
                {
                    (void)pFTPDcpt->ftp_shell_obj->dirClose(pFTPDcpt->ftp_shell_obj,fp);
                    pFTPDcpt->fileDescr = SYS_FS_HANDLE_INVALID;
                    pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NO_FILE_NO_ACTION;
                    F_FTP_ReleaseDataSocket(pFTPDcpt);                    
                    while(DirectoryFileList.list.nNodes > 0U)
                    {   // Free if already there are node in the list
                        newNode = FC_SglNode2FtpNode(TCPIP_Helper_ProtSglListHeadRemove(&DirectoryFileList));
                        if(newNode != NULL)
                        {
                            if(newNode->file_stat.lfname != NULL)
                            {
                                (void)TCPIP_HEAP_Free(sTCPIPFtpMemH,newNode->file_stat.lfname);
                            }
                            (void)TCPIP_HEAP_Free(sTCPIPFtpMemH, newNode);
                        }
                    }
                    TCPIP_Helper_ProtSglListDeinitialize(&DirectoryFileList);
                    cmdRes = true;
                    doBreak = true;
                    break;
                }
                newNode->file_stat.fattrib = fs_stat.fattrib;
                newNode->file_stat.fdate = fs_stat.fdate;
                newNode->file_stat.ftime = fs_stat.ftime;
                (void)memset(newNode->file_stat.fname,0,sizeof(newNode->file_stat.fname));
                (void)strcpy((char*)newNode->file_stat.fname ,(char*)fs_stat.fname);
                newNode->file_stat.fsize = fs_stat.fsize;
                newNode->file_stat.lfsize = fs_stat.lfsize;
                lfNameLen = (uint16_t)strlen((char*)fs_stat.lfname);
                newNode->file_stat.lfname = NULL;
                if(lfNameLen>0U)
                {
                    lfnamePtr = TCPIP_HEAP_Calloc(sTCPIPFtpMemH, 1UL, (size_t)lfNameLen + 1UL);
                    if(lfnamePtr != NULL)
                    {
                        newNode->file_stat.lfname = lfnamePtr;
                        (void)memcpy(newNode->file_stat.lfname,fs_stat.lfname,lfNameLen);
                        newNode->file_stat.lfname[lfNameLen+1U]='\0';
                    }
                    else
                    {
                        (void)pFTPDcpt->ftp_shell_obj->dirClose(pFTPDcpt->ftp_shell_obj,fp);
                        pFTPDcpt->fileDescr = SYS_FS_HANDLE_INVALID;
                        pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NO_FILE_NO_ACTION;
                        F_FTP_ReleaseDataSocket(pFTPDcpt);                    
                        if(DirectoryFileList.list.nNodes > 0U)
                        {   // Free if already there are node in the list
                            while(newNode != NULL)
                            {
                                if(newNode->file_stat.lfname != NULL)
                                {
                                    (void)TCPIP_HEAP_Free(sTCPIPFtpMemH,newNode->file_stat.lfname);
                                }
                                newNode = FC_SglNode2FtpNode(TCPIP_Helper_ProtSglListHeadRemove(&DirectoryFileList));
                                (void)TCPIP_HEAP_Free(sTCPIPFtpMemH, newNode);
                            }
                        }
                        TCPIP_Helper_ProtSglListDeinitialize(&DirectoryFileList);
                        cmdRes = true;
                        doBreak = true;
                        break;
                    }
                }
            }
            if(doBreak == false)
            {
                pFTPDcpt->ftpCommandSm  = (uint8_t)TCPIP_FTP_CMD_SM_SEND_DIR_DETAIL;
            }
            break;

        case (uint8_t)TCPIP_FTP_CMD_SM_SEND_DIR_DETAIL:
            newNode = FC_SglNode2FtpNode(DirectoryFileList.list.head);
            wCount = NET_PRES_SocketWriteIsReady(pFTPDcpt->ftpDataskt, (uint16_t)strlen(fileNameList), 1U);
            doBreak = false;
            while(newNode != NULL)
            {
                if(wCount == 0U)
                {
                    doBreak = true;
                    break;
                }
                // transmit the remaining bytes
                if(pFTPDcpt->callbackPos != 0UL)
                {                    
                    if(wCount < remainingBytes)
                    {
                        (void)NET_PRES_SocketWrite(pFTPDcpt->ftpDataskt, fileNameList+(strlen(fileNameList)-remainingBytes), wCount);
                        remainingBytes = remainingBytes - wCount;
                        wCount=0;
                        continue;
                    }
                    else
                    {
                        (void)NET_PRES_SocketWrite(pFTPDcpt->ftpDataskt, fileNameList+(strlen(fileNameList)-remainingBytes), remainingBytes);
                        wCount = wCount - remainingBytes;
                        remainingBytes =0;
                        pFTPDcpt->callbackPos = 0UL;

                        // remove the head from the list
                        newNode = FC_SglNode2FtpNode(TCPIP_Helper_ProtSglListHeadRemove(&DirectoryFileList));
                        (void)TCPIP_HEAP_Free(sTCPIPFtpMemH, newNode);
                        newNode = FC_SglNode2FtpNode(DirectoryFileList.list.head);
                        if(newNode == NULL)
                        {
                            break;
                        }
                    }
                }
                (void)memset(FileRecordsDateTime,0,sizeof(FileRecordsDateTime));
                (void)memset(fileRecrdTime,0,sizeof(fileRecrdTime));
                (void)memset(FileRecordssize,0,sizeof(FileRecordssize));
                (void)memset(fileNameList,0,sizeof(fileNameList));

                (void)FC_sprintf(FileRecordsDateTime, sizeof(FileRecordsDateTime), "%3s %02d %04d",
                                    month[(((newNode->file_stat.fdate & 0x01E0U) >> 5) - 1U)],
                                    (newNode->file_stat.fdate & 0x001FU),
                                    ((newNode->file_stat.fdate & 0xFE00U) >> 9) + 1980U);
                (void)FC_sprintf(fileRecrdTime, sizeof(fileRecrdTime),"%2d:%2d",
                                    (newNode->file_stat.ftime & 0xF800U) >> 11,
                                    (newNode->file_stat.ftime & 0x07E0U) >> 5);
 //                                 (newNode->file_stat.ftime & 0x001FU) << 1 );

                (void)FC_sprintf(FileRecordssize, sizeof(FileRecordssize), "%d", (int)newNode->file_stat.fsize );
                if((newNode->file_stat.fattrib & (uint8_t)SYS_FS_ATTR_DIR) != 0U)
                {
                    FileRecordInformation = 1;
                }
                else
                {
                    FileRecordInformation = 0;
                }

                (void)memset(fileNameList,0,sizeof(fileNameList));
                if(newNode->file_stat.lfname != NULL)
                {
                    (void)FC_sprintf(fileNameList, sizeof(fileNameList),"%-10s %3s %-8s %-8s %7s %s %s\r\n",filePermission[FileRecordInformation],link,owner,group,FileRecordssize,FileRecordsDateTime,newNode->file_stat.lfname);
                    // Now Free the allocated memory for the long file name
                    (void)TCPIP_HEAP_Free(sTCPIPFtpMemH,newNode->file_stat.lfname);
                }
                else
                {
                    (void)FC_sprintf(fileNameList, sizeof(fileNameList), "%-10s %3s %-8s %-8s %7s %s %s\r\n",filePermission[FileRecordInformation],link,owner,group,FileRecordssize,FileRecordsDateTime,newNode->file_stat.fname);
                }
                /* total name byte name length list */
                lfNameLen = (uint16_t)strlen(fileNameList);
                if(wCount > lfNameLen)
                {
                    (void)NET_PRES_SocketWrite(pFTPDcpt->ftpDataskt, fileNameList, FC_MinU16(wCount, lfNameLen));
                    remainingBytes =0;
                    pFTPDcpt->callbackPos = 0UL;
                    wCount = wCount - lfNameLen;
                    // remove the head from the list
                    newNode = FC_SglNode2FtpNode(TCPIP_Helper_ProtSglListHeadRemove(&DirectoryFileList));
                    (void)TCPIP_HEAP_Free(sTCPIPFtpMemH, newNode);
                    newNode = FC_SglNode2FtpNode(DirectoryFileList.list.head);
                    if(newNode == NULL)
                    {
                        break;
                    }
                }
                else
                {
                    pFTPDcpt->callbackPos = 1UL;
                    (void)NET_PRES_SocketWrite(pFTPDcpt->ftpDataskt, fileNameList, wCount);
                    remainingBytes = lfNameLen - wCount;
                    wCount=0;
                    continue;
                   // break;
                }
            }
            if(doBreak == false)
            {
                TCPIP_Helper_ProtSglListDeinitialize(&DirectoryFileList);
                pFTPDcpt->ftpCommandSm  = (uint8_t)TCPIP_FTP_CMD_SM_WAIT_FOR_DISCONNECT;
            }
            break;

        case (uint8_t)TCPIP_FTP_CMD_SM_WAIT_FOR_DISCONNECT:
            F_FTP_ReleaseDataSocket(pFTPDcpt);                    
            pFTPDcpt->ftpResponse   = (uint8_t)TCPIP_FTP_RESP_FILE_CLOSE_DATA_CONN;
            cmdRes = true;
            break;

        default:
            // do nothing
            break;
    }
    return cmdRes;
}

static bool TCPIP_FTP_ExecuteCmdGet(TCPIP_FTP_DCPT* pFTPDcpt, char *cFile)
{
    switch(pFTPDcpt->ftpCommandSm)
    {
        case (uint8_t)TCPIP_FTP_CMD_SM_IDLE:
            if ( pFTPDcpt->ftpFlag.Bits.loggedIn == 0U )
            {
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_LOGIN;
                return true;
            }
            else
            {
                if(pFTPDcpt->ftpFlag.Bits.pasvMode != 1U)
                {
                    if(TCPIP_FTP_CreateDataSocket(pFTPDcpt) != true)
                    {
                        return true;
                    }
                }
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_FILE_IS_PRESENT;
                pFTPDcpt->ftpCommandSm = (uint8_t)TCPIP_FTP_CMD_SM_WAIT;
            }
            break;

        case (uint8_t)TCPIP_FTP_CMD_SM_WAIT:
            if(NET_PRES_SocketIsConnected(pFTPDcpt->ftpDataskt))
            {
                pFTPDcpt->ftpCommandSm    = (uint8_t)TCPIP_FTP_CMD_SM_SEND;
            }
        break;

        case (uint8_t)TCPIP_FTP_CMD_SM_SEND:
        // Get/put as many bytes as possible
            if(TCPIP_FTP_FileGet(pFTPDcpt, cFile)== false)
            {
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_FILE_CLOSE_DATA_CONN;
                pFTPDcpt->callbackPos = 0UL;
                F_FTP_ReleaseDataSocket(pFTPDcpt);            
                pFTPDcpt->ftpFlag.Bits.pasvMode = 0U;
                return true;
            }
            else if(pFTPDcpt->callbackPos == 0UL)
            {
                F_FTP_ReleaseDataSocket(pFTPDcpt);       
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_DATA_CLOSE;
                pFTPDcpt->ftpFlag.Bits.pasvMode = 0U;
                return true;
            }
            else
            {
                // do nothing
            }
            break;

        default:
            // do nothing
            break;
    }
    return false;
}

static TCPIP_FTP_RESP TCPIP_FTP_CMDLowerCase(const char *upperCmd , char *lowerCmd )
{
    size_t i;
    uint8_t uCh;
    
    if(upperCmd == NULL)
    {
        return TCPIP_FTP_COMMAND_UNKNOWN;
    }
    
    for(i = 0; i <= strlen(upperCmd); i++)
    {
        uCh = (uint8_t)upperCmd[i];
      if(uCh >= 65U && uCh <= 90U)
      {
         uCh += 32U;
         lowerCmd[i] = (char)uCh;
      }
    }

    return  TCPIP_FTP_RESP_OK;
}

static TCPIP_FTP_CMD TCPIP_FTP_CmdsParse(char *cmd)
{
    size_t ix;   // TCPIP_FTP_CMD
    char lowerCommandStr[5];

    for ( ix = 0; ix < TCPIP_FTP_CMD_TBL_SIZE; ix++ )
    {
        // check the incoming command with upper case command
        if(strcmp(cmd, sTCPIPFTPCmdString[ix]) == 0)
        {
            // return the valid FTP number command 
            return (TCPIP_FTP_CMD)ix;
        }
        // memset the local command string array
        (void)memset(lowerCommandStr,0,sizeof(lowerCommandStr));
        //get the lowercase command for the upper case global command table sTCPIPFTPCmdString[]
        if(TCPIP_FTP_CMDLowerCase(sTCPIPFTPCmdString[ix], lowerCommandStr)!= TCPIP_FTP_RESP_OK)
        {
            return TCPIP_FTP_CMD_UNKNOWN;
        }
        // check the lowercase command 
        if(strcmp(cmd, lowerCommandStr) == 0)
        {
            return (TCPIP_FTP_CMD)ix;
        }
    }

    return TCPIP_FTP_CMD_UNKNOWN;
}

static void TCPIP_FTP_CmdStringParse(TCPIP_FTP_DCPT* pFTPDcpt)
{
    char *p;
    char v;
    enum { SM_FTP_PARSE_PARAM, SM_FTP_PARSE_SPACE } smParseFTP;

    smParseFTP  = SM_FTP_PARSE_PARAM;
    p           = pFTPDcpt->ftpCmdString;

    // Skip white blanks
    while( *p == ' ' )
    {
        p++;
    }

    pFTPDcpt->ftp_argv[0]  = p;
    pFTPDcpt->ftp_argc     = 1U;

    while( (v = *p) != '\0' )
    {
        switch(smParseFTP)
        {
            case SM_FTP_PARSE_PARAM:
                if ( v == ' ' || v == ',' || v=='|')
                {
                    *p = '\0';
                    smParseFTP = SM_FTP_PARSE_SPACE;
                }
                else if ( v == '\r' || v == '\n' )
                {
                    *p = '\0';
                }
                else
                {
                    // do nothing
                }
                break;

            case SM_FTP_PARSE_SPACE:
                if (( v != ' ' ) && (v != '|'))
                {
                    pFTPDcpt->ftp_argv[pFTPDcpt->ftp_argc++] = p;
                    smParseFTP = SM_FTP_PARSE_PARAM;
                }
                break;

            default:
                // do nothing
                break;
        }
        p++;
        if(pFTPDcpt->ftp_argc == (uint8_t)TCPIP_FTP_MAX_ARGS)
        {
            break;
        }
    }
}

/*****************************************************************************
  Function:
    static bool TCPIP_FTP_FileGet(TCPIP_FTP_DCPT* pFTPDcpt, char *cFile)

  Summary:
    Writes a file byte-for-byte to the currently loaded TCP socket.

  Description:
    Allows an entire file to be included as a dynamic variable, providing
    a basic templating system for HTML web pages.  This reduces unneeded
    duplication of visual elements such as headers, menus, etc.

    When pFTPDcpt->callbackPos is 0, the file is opened and as many bytes
    as possible are written.  The current position is then saved to
    pFTPDcpt->callbackPos and the file is closed.  On subsequent calls,
    reading begins at the saved location and continues.  Once the end of
    the input file is reached, pFTPDcpt->callbackPos is set back to 0 to
    indicate completion.

  Precondition:
    None

  Parameters:
    cFile - the name of the file to be sent
    pFTPDcpt - FTP descriptor
  Returns:
    None

  Remarks:
    Users should not call this function directly.
  ***************************************************************************/
static bool TCPIP_FTP_FileGet(TCPIP_FTP_DCPT* pFTPDcpt, char *cFile)
{
    uint16_t wCount, wLen;
    int32_t status;
    uint8_t data[512];
    SYS_FS_HANDLE fp;

    fp = pFTPDcpt->fileDescr;

    // Check if this is a first round call
    if(pFTPDcpt->callbackPos == 0UL)
    {// On initial call, open the file and save its ID
        fp = (pFTPDcpt->ftp_shell_obj->fileOpen)(pFTPDcpt->ftp_shell_obj, cFile, SYS_FS_FILE_OPEN_READ);
        if(fp == SYS_FS_HANDLE_INVALID)
        {// File not found, so abort
            return false;
        }
        pFTPDcpt->fileDescr = fp;
        pFTPDcpt->callbackPos = 0UL;
    }
    else if(pFTPDcpt->callbackPos != 0UL)
    {// The file was already opened, so load up its ID and seek
        if(fp == SYS_FS_HANDLE_INVALID)
        {// No file handles available, so wait for now
            return false;
        }
        (void)pFTPDcpt->ftp_shell_obj->fileSeek(pFTPDcpt->ftp_shell_obj,fp,(int32_t)pFTPDcpt->callbackPos,SYS_FS_SEEK_SET);
    }
    else
    {
        // do nothing
    }

    // Get/put as many bytes as possible
    wCount = NET_PRES_SocketWriteIsReady(pFTPDcpt->ftpDataskt, (uint16_t)sizeof(data), 1U);
    while(wCount > 0u)
    {
        wLen = (uint16_t)pFTPDcpt->ftp_shell_obj->fileRead(pFTPDcpt->ftp_shell_obj,fp,data, FC_MinU16(wCount, (uint16_t)sizeof(data)));
        if(wLen == 0U)
        {// If no bytes were read, an EOF was reached
            (void)pFTPDcpt->ftp_shell_obj->fileClose(pFTPDcpt->ftp_shell_obj,fp);
            pFTPDcpt->fileDescr = SYS_FS_HANDLE_INVALID;
            pFTPDcpt->callbackPos = 0UL;
            return true;
        }
        else
        {// Write the bytes to the socket
            (void)NET_PRES_SocketWrite(pFTPDcpt->ftpDataskt, data, wLen);
            wCount -= wLen;
            pFTPDcpt->ftpSysTicklastActivity = SYS_TMR_TickCountGet();
        }
    }

    // Save the new address and close the file
    status = pFTPDcpt->ftp_shell_obj->fileTell(pFTPDcpt->ftp_shell_obj,fp);
    if(status == -1L)
    {
        pFTPDcpt->callbackPos = 0UL;
    }
    else
    {
        pFTPDcpt->callbackPos = (uint32_t)status;
    }

    return true;
}

static bool TCPIP_FTP_LSCmd(TCPIP_FTP_DCPT* pFTPDcpt)
{
    char longFileName[SYS_FS_FILE_NAME_LEN];
    uint16_t wCount, bytestoBeSent;
    static char fileNameList[TCPIP_FTP_MAX_FILE_NAME_LEN+
                    TCPIP_FTP_MAX_FILE_DATE_TIME_STR_LEN+
                    TCPIP_FTP_MAX_FILE_SIZE_STR_LEN+
                    +10];
    SYS_FS_HANDLE fp;
    static uint16_t  remainingBytes = 0U;
    SYS_FS_FSTAT fs_stat;
    (void)memset(&fs_stat, 0, sizeof(SYS_FS_FSTAT));
    const char fileHeaderStr[] = "Date\t\t\tType\tFileSize\tfilename\t\r\n\0";
    uint8_t FileRecordInformation;
    char FileRecordsDateTime[TCPIP_FTP_MAX_FILE_DATE_TIME_STR_LEN];
    char FileRecordssize[TCPIP_FTP_MAX_FILE_SIZE_STR_LEN];
    static bool  cmdWithArg=false;
    uint8_t  nlstArgIndex;
    FTP_LIST_NODE* newNode=NULL;
    uint16_t    lfNameLen=0;
    char    *lfnamePtr=NULL;
    char fileType[2][5]={"file\0","dir\0"};

    bool cmdRes = false;
    bool doBreak = false;
    switch(pFTPDcpt->ftpCommandSm)
    {
        case (uint8_t)TCPIP_FTP_CMD_SM_IDLE:
            if(pFTPDcpt->ftpFlag.Bits.pasvMode != 1U)
            {
                if(TCPIP_FTP_CreateDataSocket(pFTPDcpt) != true)
                {
                    cmdRes = true;
                    break;
                }
            }
            if ( pFTPDcpt->ftpFlag.Bits.loggedIn == 0U )
            {
                pFTPDcpt->ftpResponse    = (uint8_t)TCPIP_FTP_RESP_LOGIN;
                F_FTP_ReleaseDataSocket(pFTPDcpt);  
                cmdRes = true;
                break;
            }
            pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_DATA_READY;
            pFTPDcpt->ftpCommandSm = (uint8_t)TCPIP_FTP_CMD_SM_WAIT;
            break;

        case (uint8_t)TCPIP_FTP_CMD_SM_WAIT:
            if(NET_PRES_SocketIsConnected(pFTPDcpt->ftpDataskt))
            {
                pFTPDcpt->ftpCommandSm = (uint8_t)TCPIP_FTP_CMD_SM_SEND;
            }
            else
            {
                break;
            }
            //reset NLST argument flag
            gFTPNLSTArgIndex = 0xFF;
            
            if(pFTPDcpt->ftp_argc == 1U)
            {
                pFTPDcpt->ftpCommandSm   = (uint8_t)TCPIP_FTP_CMD_SM_SEND_DIR;
                break;
            }
            else
            { // check if the NLST command has any argument
            
                for(nlstArgIndex = 0; nlstArgIndex < (uint8_t)TCPIP_FTP_MAX_NLST_ARGS; nlstArgIndex++)
                {
                    if(strcmp(pFTPDcpt->ftp_argv[1],(char*)TCPIP_FTP_CMD_NLSTArgs[nlstArgIndex])==0)
                    {
                        cmdWithArg = true;
                        gFTPNLSTArgIndex = nlstArgIndex;
                        break;
                    }
                    else
                    {
                        cmdWithArg = false;
                        gFTPNLSTArgIndex = 0xFF;
                    }
                }
                if(cmdWithArg)
                { // argument two will be the file name
                    if(strlen(pFTPDcpt->ftp_argv[2]) > SYS_FS_FILE_NAME_LEN)
                    {// File not found, so abort
                        pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NO_FILE_NO_ACTION;
                        F_FTP_ReleaseDataSocket(pFTPDcpt);  
                        cmdWithArg = false;
                        cmdRes = true;
                        break;
                    }
                }
                else
                { // argument first will be the file name
                    if(strlen(pFTPDcpt->ftp_argv[1]) > SYS_FS_FILE_NAME_LEN)
                    {// File not found, so abort
                        pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NO_FILE_NO_ACTION;
                        F_FTP_ReleaseDataSocket(pFTPDcpt);  
                        cmdWithArg = false;
                        cmdRes = true;
                        break;
                    }
                }
            }
            break;

        case (uint8_t)TCPIP_FTP_CMD_SM_SEND:
            if(cmdWithArg)
            { // argument two will be the file name
                if((pFTPDcpt->ftp_shell_obj->fileStat)(pFTPDcpt->ftp_shell_obj,pFTPDcpt->ftp_argv[2], &fs_stat) == SYS_FS_RES_SUCCESS)
                {
                    if((fs_stat.fattrib & (uint8_t)SYS_FS_ATTR_DIR) == 0U)
                    {  // transmit the remaining bytes
                        pFTPDcpt->ftpCommandSm   = (uint8_t)TCPIP_FTP_CMD_SM_SEND_FILE;
                    }
                    else
                    {
                         pFTPDcpt->ftpCommandSm   = (uint8_t)TCPIP_FTP_CMD_SM_SEND_DIR;
                    }
                }
                else
                {
                    pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NO_FILE_NO_ACTION;
                    F_FTP_ReleaseDataSocket(pFTPDcpt);  
                    cmdWithArg = false;
                    cmdRes = true;
                    break;
                }
            }
            else
            {
                if((pFTPDcpt->ftp_shell_obj->fileStat)(pFTPDcpt->ftp_shell_obj,pFTPDcpt->ftp_argv[1], &fs_stat) == SYS_FS_RES_SUCCESS)
                {
                    if((fs_stat.fattrib & (uint8_t)SYS_FS_ATTR_DIR) == 0U)
                    {  // transmit the remaining bytes
                        pFTPDcpt->ftpCommandSm   = (uint8_t)TCPIP_FTP_CMD_SM_SEND_FILE;
                    }
                    else
                    {
                         pFTPDcpt->ftpCommandSm   = (uint8_t)TCPIP_FTP_CMD_SM_SEND_DIR;
                    }
                }
                else
                {
                    pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NO_FILE_NO_ACTION;
                    F_FTP_ReleaseDataSocket(pFTPDcpt);  
                    cmdWithArg = false;
                    cmdRes = true;
                    break;
                }
            }
            break;

        case (uint8_t)TCPIP_FTP_CMD_SM_SEND_FILE:
            wCount = NET_PRES_SocketWriteIsReady(pFTPDcpt->ftpDataskt, (uint16_t)strlen(fileNameList), 1U);
            (void)memset(fileNameList, 0, sizeof(fileNameList));
            // If there is no argument
            if((gFTPNLSTArgIndex == 0xFFU) && (pFTPDcpt->callbackPos == 0UL))
            {
                // Expected wcount should not be less than ftp argument 1
                (void)FC_sprintf(fileNameList, sizeof(fileNameList),"%s\r\n", pFTPDcpt->ftp_argv[1]);
                bytestoBeSent = (uint16_t)strlen(fileNameList);
                if(bytestoBeSent > wCount)
                {
                    while(bytestoBeSent > 0U)
                    {
                        (void)NET_PRES_SocketWrite(pFTPDcpt->ftpDataskt, fileNameList, FC_MinU16(wCount, (uint16_t)strlen(fileNameList)));
                        bytestoBeSent -= wCount;
                    }
                }
                else
                {
                     (void)NET_PRES_SocketWrite(pFTPDcpt->ftpDataskt, fileNameList, (uint16_t)strlen(fileNameList));
                }
                pFTPDcpt->ftpCommandSm  = (uint8_t)TCPIP_FTP_CMD_SM_WAIT_FOR_DISCONNECT;
                break;
            }
            // send the complete details of a File
            // for Windows ftp client , NLST command considers -a as a file argument and
            // it ignores the correct file argument.
            if(pFTPDcpt->callbackPos != 0UL)
            {
                if(wCount < remainingBytes)
                {
                    (void)NET_PRES_SocketWrite(pFTPDcpt->ftpDataskt, fileNameList+(strlen(fileNameList)-remainingBytes), wCount);
                    remainingBytes = remainingBytes - wCount;
                }
                else
                {
                    (void)NET_PRES_SocketWrite(pFTPDcpt->ftpDataskt, fileNameList+(strlen(fileNameList)-remainingBytes), remainingBytes);
                    remainingBytes =0;
                    pFTPDcpt->callbackPos = 0UL;
                }
                break;
            }
            else
            {
                (void)memset(fileNameList,0,sizeof(fileNameList));
                (void)strncpy(fileNameList, fileHeaderStr, FC_MinU16((uint16_t)sizeof(fileNameList), (uint16_t)sizeof(fileHeaderStr)));
                if(wCount > strlen(fileNameList))
                {
                    (void)NET_PRES_SocketWrite(pFTPDcpt->ftpDataskt, fileNameList, (uint16_t)strlen(fileNameList));
                    remainingBytes =0;
                    pFTPDcpt->callbackPos = 0;
                }
                else
                {
                    pFTPDcpt->callbackPos = 1;
                    (void)NET_PRES_SocketWrite(pFTPDcpt->ftpDataskt, fileNameList, FC_MinU16(wCount, (uint16_t)strlen(fileNameList)));
                    remainingBytes = (uint16_t)strlen(fileNameList)- wCount;
                    break;
                }
            }
            (void)memset(FileRecordsDateTime,0,sizeof(FileRecordsDateTime));
            (void)memset(FileRecordssize,0,sizeof(FileRecordssize));
            (void)FC_sprintf(FileRecordsDateTime, sizeof(FileRecordsDateTime), "%02d-%02d-%04d %02d:%02d:%02d",
                                    (((fs_stat.fdate & 0x01E0U) >> 5) - 1U),   // month
                                    (fs_stat.fdate & 0x001FU),  // date
                                    ((fs_stat.fdate & 0xFE00U) >> 9) + 1980U, // year
                                    (fs_stat.ftime & 0xF800U) >> 11,  // hour
                                    (fs_stat.ftime & 0x07E0U) >> 5, // min
                                    (fs_stat.ftime & 0x001FU) << 1 ); // sec

            (void)FC_sprintf(FileRecordssize, sizeof(FileRecordssize), "%d",(int) fs_stat.fsize );

            (void)memset(fileNameList,0,sizeof(fileNameList));
            (void)FC_sprintf(fileNameList, sizeof(fileNameList),"%s\t file \t %s\t\t%s\r\n",FileRecordsDateTime,FileRecordssize,pFTPDcpt->ftp_argv[2]);

            if(wCount > (uint16_t)strlen(fileNameList))
            {
                (void)NET_PRES_SocketWrite(pFTPDcpt->ftpDataskt, fileNameList, FC_MinU16(wCount, (uint16_t)strlen(fileNameList)));
                remainingBytes =0;
                pFTPDcpt->callbackPos = 0;
                pFTPDcpt->ftpCommandSm  = (uint8_t)TCPIP_FTP_CMD_SM_WAIT_FOR_DISCONNECT;
            }
            else
            {
                pFTPDcpt->callbackPos = 1;
                (void)NET_PRES_SocketWrite(pFTPDcpt->ftpDataskt, fileNameList, FC_MinU16(wCount, (uint16_t)strlen(fileNameList)));
                remainingBytes = (uint16_t)strlen(fileNameList)- wCount;
                break;
            }
            pFTPDcpt->ftpCommandSm  = (uint8_t)TCPIP_FTP_CMD_SM_WAIT_FOR_DISCONNECT;
            break;

        case (uint8_t)TCPIP_FTP_CMD_SM_SEND_DIR:
            (void)memset(fileNameList,0,sizeof(fileNameList));
            if(gFTPNLSTArgIndex == 0xFFU)
            {
                if(pFTPDcpt->ftp_argc == 1U)
                {// No specific directory path
                    fp = (pFTPDcpt->ftp_shell_obj->dirOpen)(pFTPDcpt->ftp_shell_obj,".");
                }
                else
                {
                    fp = (pFTPDcpt->ftp_shell_obj->dirOpen)(pFTPDcpt->ftp_shell_obj,pFTPDcpt->ftp_argv[1]);
                }
            }
            else
            {
                fp = (pFTPDcpt->ftp_shell_obj->dirOpen)(pFTPDcpt->ftp_shell_obj,pFTPDcpt->ftp_argv[2]);
            }
            if (fp == SYS_FS_HANDLE_INVALID)
            {// File not found, so abort
                pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NO_FILE_NO_ACTION;
                F_FTP_ReleaseDataSocket(pFTPDcpt);  
                cmdWithArg = false;
                cmdRes = true;
                break;
            }
            pFTPDcpt->fileDescr = fp;
            pFTPDcpt->callbackPos = 0;
            pFTPDcpt->ftpCommandSm  = (uint8_t)TCPIP_FTP_CMD_SM_SEND_DIR_HEADER;
            break;

         case (uint8_t)TCPIP_FTP_CMD_SM_SEND_DIR_HEADER:
             wCount = NET_PRES_SocketWriteIsReady(pFTPDcpt->ftpDataskt, (uint16_t)strlen(fileNameList), 1U);
             // send the Header details of the table
            if(pFTPDcpt->callbackPos != 0UL)
            {
                if(wCount < remainingBytes)
                {
                    (void)NET_PRES_SocketWrite(pFTPDcpt->ftpDataskt, fileNameList+(strlen(fileNameList)-remainingBytes), wCount);
                    remainingBytes = remainingBytes - wCount;
                }
                else
                {
                    (void)NET_PRES_SocketWrite(pFTPDcpt->ftpDataskt, fileNameList+(strlen(fileNameList)-remainingBytes), remainingBytes);
                    remainingBytes =0;
                    pFTPDcpt->callbackPos = 0UL;
                }
                break;
            }
            else
            {
                (void)memset(fileNameList,0,sizeof(fileNameList));
                (void)strncpy(fileNameList, fileHeaderStr, FC_MinU16((uint16_t)sizeof(fileNameList), (uint16_t)sizeof(fileHeaderStr)));                
                if(wCount > strlen(fileNameList))
                {
                    (void)NET_PRES_SocketWrite(pFTPDcpt->ftpDataskt, fileNameList, (uint16_t)strlen(fileNameList));
                    remainingBytes =0;
                    pFTPDcpt->callbackPos = 0UL;
                }
                else
                {
                    pFTPDcpt->callbackPos = 1UL;
                    (void)NET_PRES_SocketWrite(pFTPDcpt->ftpDataskt, fileNameList, FC_MinU16(wCount, (uint16_t)strlen(fileNameList)));
                    remainingBytes = (uint16_t)strlen(fileNameList)- wCount;
                    break;
                }
            }
            // If long file name is used, the following elements of the "stat"
             // structure needs to be initialized with address of proper buffer.
            fs_stat.lfname = longFileName;
            fs_stat.lfsize = SYS_FS_FILE_NAME_LEN;
            fp = pFTPDcpt->fileDescr;
            if(TCPIP_Helper_ProtSglListInitialize(&DirectoryFileList) != true)
            {
                cmdRes = true;
                break;
            }

            while(true)
            {
                (void)memset(fs_stat.fname,0,13);
                (void)memset(longFileName,0,sizeof(longFileName));
                if(SYS_FS_DirSearch(fp, "*", SYS_FS_ATTR_ARC, &fs_stat) == SYS_FS_RES_FAILURE)
                {
                    (void)pFTPDcpt->ftp_shell_obj->dirClose(pFTPDcpt->ftp_shell_obj,fp);
                    pFTPDcpt->fileDescr = SYS_FS_HANDLE_INVALID;
                    break;
                }
                newNode = FC_SglNode2FtpNode(TCPIP_FTP_LIST_Add(&DirectoryFileList,sTCPIPFtpMemH , sizeof(*newNode)));
                if(newNode == NULL)
                {
                    (void)pFTPDcpt->ftp_shell_obj->dirClose(pFTPDcpt->ftp_shell_obj,fp);
                    pFTPDcpt->fileDescr = SYS_FS_HANDLE_INVALID;
                    pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NO_FILE_NO_ACTION;
                    F_FTP_ReleaseDataSocket(pFTPDcpt);  
                    while(DirectoryFileList.list.nNodes > 0U)
                    {   // Free if already there are node in the list
                        newNode = FC_SglNode2FtpNode(TCPIP_Helper_ProtSglListHeadRemove(&DirectoryFileList));
                        if(newNode != NULL)
                        {
                            if(newNode->file_stat.lfname != NULL)
                            {
                                (void)TCPIP_HEAP_Free(sTCPIPFtpMemH,newNode->file_stat.lfname);
                            }
                            (void)TCPIP_HEAP_Free(sTCPIPFtpMemH, newNode);
                        }
                    }
                    TCPIP_Helper_ProtSglListDeinitialize(&DirectoryFileList);
                    cmdRes = true;
                    doBreak = true; 
                    break;
                }
                newNode->file_stat.fattrib = fs_stat.fattrib;
                newNode->file_stat.fdate = fs_stat.fdate;
                newNode->file_stat.ftime = fs_stat.ftime;
                (void)memset(newNode->file_stat.fname,0,sizeof(newNode->file_stat.fname));
                (void)strcpy((char*)newNode->file_stat.fname ,(char*)fs_stat.fname);
                newNode->file_stat.fsize = fs_stat.fsize;
                newNode->file_stat.lfsize = fs_stat.lfsize;
                lfNameLen = (uint16_t)strlen((char*)fs_stat.lfname);
                newNode->file_stat.lfname = NULL;
                if(lfNameLen > 0U)
                {
                    lfnamePtr = TCPIP_HEAP_Calloc(sTCPIPFtpMemH, 1UL, (size_t)lfNameLen + 1UL);
                    if(lfnamePtr != NULL)
                    {
                        newNode->file_stat.lfname = lfnamePtr;
                        (void)memcpy(newNode->file_stat.lfname,fs_stat.lfname,lfNameLen);
                        newNode->file_stat.lfname[lfNameLen + 1U]='\0';
                    }
                    else
                    {
                        (void)pFTPDcpt->ftp_shell_obj->dirClose(pFTPDcpt->ftp_shell_obj,fp);
                        pFTPDcpt->fileDescr = SYS_FS_HANDLE_INVALID;
                        pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NO_FILE_NO_ACTION;
                        F_FTP_ReleaseDataSocket(pFTPDcpt);  
                        if(DirectoryFileList.list.nNodes > 0U)
                        {   // Free if already there are node in the list
                            while(newNode != NULL)
                            {
                                if(newNode->file_stat.lfname != NULL)
                                {
                                    (void)TCPIP_HEAP_Free(sTCPIPFtpMemH,newNode->file_stat.lfname);
                                }
                                newNode = FC_SglNode2FtpNode(TCPIP_Helper_ProtSglListHeadRemove(&DirectoryFileList));
                                (void)TCPIP_HEAP_Free(sTCPIPFtpMemH, newNode);
                            }
                        }
                        TCPIP_Helper_ProtSglListDeinitialize(&DirectoryFileList);
                        cmdRes = true;
                        doBreak = true; 
                        break;
                    }
                }
            }
            if(doBreak)
            {
                break;
            }  

            pFTPDcpt->ftpCommandSm  = (uint8_t)TCPIP_FTP_CMD_SM_SEND_DIR_DETAIL;
            break;
            
        case (uint8_t)TCPIP_FTP_CMD_SM_SEND_DIR_DETAIL:
            newNode = FC_SglNode2FtpNode(DirectoryFileList.list.head);
            wCount = NET_PRES_SocketWriteIsReady(pFTPDcpt->ftpDataskt, (uint16_t)strlen(fileNameList), 1U);
            doBreak = false;
            while(newNode != NULL)
            {
                if(wCount == 0U)
                {
                    cmdRes = false;
                    doBreak = true;
                    break;
                }
                // transmit the remaining bytes
                if(pFTPDcpt->callbackPos != 0UL)
                {
                    if(wCount < remainingBytes)
                    {
                        (void)NET_PRES_SocketWrite(pFTPDcpt->ftpDataskt, fileNameList+(strlen(fileNameList)-remainingBytes), wCount);
                        remainingBytes = remainingBytes - wCount;
                        wCount=0;
                        continue;
                    }
                    else
                    {
                        (void)NET_PRES_SocketWrite(pFTPDcpt->ftpDataskt, fileNameList+(strlen(fileNameList)-remainingBytes), remainingBytes);
                        wCount = wCount - remainingBytes;
                        remainingBytes =0;
                        pFTPDcpt->callbackPos = 0UL;
                        // remove the head from the list
                        newNode = FC_SglNode2FtpNode(TCPIP_Helper_ProtSglListHeadRemove(&DirectoryFileList));
                        (void)TCPIP_HEAP_Free(sTCPIPFtpMemH, newNode);
                        newNode = FC_SglNode2FtpNode(DirectoryFileList.list.head);
                        if(newNode == NULL)
                        {
                            break;
                        }
                    }
                }
                (void)memset(FileRecordsDateTime,0,sizeof(FileRecordsDateTime));
                (void)memset(FileRecordssize,0,sizeof(FileRecordssize));
                (void)memset(fileNameList,0,sizeof(fileNameList));

                (void)FC_sprintf(FileRecordsDateTime, sizeof(FileRecordsDateTime), "%02d-%02d-%04d %02d:%02d:%02d",
                                    (newNode->file_stat.fdate & 0x01E0U) >> 5,
                                    (newNode->file_stat.fdate & 0x001FU),
                                    ((newNode->file_stat.fdate & 0xFE00U) >> 9) + 1980U,
                                    (newNode->file_stat.ftime & 0xF800U) >> 11,
                                    (newNode->file_stat.ftime & 0x07E0U) >> 5,
                                    (newNode->file_stat.ftime & 0x001FU) << 1 );

                (void)FC_sprintf(FileRecordssize, sizeof(FileRecordssize), "%d",(int) newNode->file_stat.fsize );
                if((newNode->file_stat.fattrib & (uint8_t)SYS_FS_ATTR_DIR) != 0U)
                {
                    FileRecordInformation = 1;
                }
                else
                {
                    FileRecordInformation = 0;
                }

                (void)memset(fileNameList,0,sizeof(fileNameList));
                if(newNode->file_stat.lfname != NULL)
                {
                    (void)FC_sprintf(fileNameList, sizeof(fileNameList),"%s\t %s \t %s\t\t%s\r\n",FileRecordsDateTime,fileType[FileRecordInformation],FileRecordssize,newNode->file_stat.lfname);
                    // Now Free the allocated memory for the long file name
                    (void)TCPIP_HEAP_Free(sTCPIPFtpMemH,newNode->file_stat.lfname);
                }
                else
                {
                    (void)FC_sprintf(fileNameList, sizeof(fileNameList),"%s\t %s \t %s\t\t%s\r\n",FileRecordsDateTime,fileType[FileRecordInformation],FileRecordssize,newNode->file_stat.fname);
                }

                lfNameLen = (uint16_t)strlen(fileNameList);
                if(wCount > lfNameLen)
                {
                    (void)NET_PRES_SocketWrite(pFTPDcpt->ftpDataskt, fileNameList, FC_MinU16(wCount, lfNameLen));
                    remainingBytes =0;
                    pFTPDcpt->callbackPos = 0UL;
                    wCount = wCount - lfNameLen;
                    // remove the head from the list
                    newNode = FC_SglNode2FtpNode(TCPIP_Helper_ProtSglListHeadRemove(&DirectoryFileList));
                    (void)TCPIP_HEAP_Free(sTCPIPFtpMemH, newNode);
                    newNode = FC_SglNode2FtpNode(DirectoryFileList.list.head);
                    if(newNode == NULL)
                    {
                        break;
                    }
                }
                else
                {
                    pFTPDcpt->callbackPos = 1UL;
                    (void)NET_PRES_SocketWrite(pFTPDcpt->ftpDataskt, fileNameList, wCount);
                    remainingBytes = lfNameLen - wCount;
                    wCount=0;
                    continue;
                }
            }
            if(doBreak)
            {
                break;
            }  
            TCPIP_Helper_ProtSglListDeinitialize(&DirectoryFileList);
            pFTPDcpt->ftpCommandSm  = (uint8_t)TCPIP_FTP_CMD_SM_WAIT_FOR_DISCONNECT;
            break;

        case (uint8_t)TCPIP_FTP_CMD_SM_WAIT_FOR_DISCONNECT:
            F_FTP_ReleaseDataSocket(pFTPDcpt);  
            pFTPDcpt->ftpResponse   = (uint8_t)TCPIP_FTP_RESP_FILE_CLOSE_DATA_CONN;
            gFTPNLSTArgIndex = 0xFF;
            remainingBytes = 0;
            cmdWithArg = false;
            cmdRes = true;
            break;

        default:
            // do nothing
            break;
    }
    return cmdRes;
}
/*
 * Remove the file or directory for the FTP command "delete" or "rmdir"
 */
static SYS_FS_RESULT TCPIP_FTP_RemoveFile(TCPIP_FTP_DCPT * pFTPDcpt)
{
    SYS_FS_ERROR    fs_err;
    SYS_FS_RESULT   result;
    char            ftpMsg[128];

    result = (pFTPDcpt->ftp_shell_obj->fileDelete)(pFTPDcpt->ftp_shell_obj,pFTPDcpt->ftp_argv[1]);
    if(result != SYS_FS_RES_SUCCESS)
    {
        fs_err = SYS_FS_Error();
        (void)memset(ftpMsg,0,sizeof(ftpMsg));
        if(fs_err == SYS_FS_ERROR_DENIED)
        {            
            (void)FC_sprintf(ftpMsg, sizeof(ftpMsg),"550 \"%s\" Directory is not empty! \r\n",pFTPDcpt->ftp_argv[1]);
        }
        else if(fs_err == SYS_FS_ERROR_LOCKED)
        {           
            (void)FC_sprintf(ftpMsg, sizeof(ftpMsg),"550 \"%s\" Directory or File is Locked! \r\n",pFTPDcpt->ftp_argv[1]);
        }
        else
        {
            // do nothing
        }
        if (NET_PRES_SocketWriteIsReady(pFTPDcpt->ftpCmdskt, (uint16_t)strlen(ftpMsg), 0U) == 0U) 
        {
            pFTPDcpt->ftpResponse = (uint8_t)TCPIP_FTP_RESP_NONE;
            pFTPDcpt->ftpSm = (uint8_t)TCPIP_FTP_SM_CONNECTED;
            return result;
        }
        (void)NET_PRES_SocketWrite(pFTPDcpt->ftpCmdskt, (const uint8_t*)ftpMsg, (uint16_t)strlen(ftpMsg));
        (void)NET_PRES_SocketFlush(pFTPDcpt->ftpCmdskt);
    }
    return result;
}

#if (M_FTP_USE_AUTHENTICATION_CALLBACK != 0)
TCPIP_FTP_HANDLE TCPIP_FTP_AuthenticationRegister(TCPIP_FTP_AUTH_HANDLER authHandler, const void* handlerParam)
{
    TCPIP_FTP_HANDLE tHandle = NULL;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(FTPAuthHandler == NULL)
    {
        FTPAuthHParam = handlerParam;
        FTPAuthHandler = authHandler;
        tHandle = FC_AuthHndl2FtpHndl(authHandler);
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return tHandle;
}

bool TCPIP_FTP_AuthenticationDeregister(TCPIP_FTP_HANDLE authHandle)
{
    bool res = false;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(FC_AuthHndl2FtpHndl(FTPAuthHandler) == authHandle)
    {
        FTPAuthHandler = NULL;
        res = true;
    } 

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return res;
}

#endif  // (M_FTP_USE_AUTHENTICATION_CALLBACK != 0)




#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_FTP_SERVER)

