/*******************************************************************************
  File Transfer Protocol (FTP) Client

  Summary:
    Module for Microchip TCP/IP Stack

  Description:
    - Provides ability to remotely upload MPFS2 image (web pages)
      to external EEPROM or external Flash memory
    - Reference: RFC 959
*******************************************************************************/

/*******************************************************************************
File Name:  FTP.c
Copyright © 2012 released Microchip Technology Inc.  All rights
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
#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_FTP_SERVER

#include "tcpip/src/tcpip_private.h"

#include "tcpip/src/common/sys_fs_wrapper.h"
#include "system/fs/src/sys_fs_local.h"


#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_FTP_SERVER)
#include "tcpip/src/ftp_private.h"

static TCPIP_FTP_MODULE_CONFIG_DATA ftpConfigData;

// Maximum size for a file name in a SD card is expected to be 100
#define TCPIP_FTP_MAX_FILE_NAME_LEN 100
// Maximum string size for a file date and time in a SD card is expected to be 25
#define TCPIP_FTP_MAX_FILE_DATE_TIME_STR_LEN 25
// Maximum string size for a file size in a SD card is expected to be 5
#define TCPIP_FTP_MAX_FILE_SIZE_STR_LEN 5

// Each entry in following table must match with that of FTP_COMMAND enum.
static const char * const sTCPIPFTPCmdString[] =
{
    "USER",                         // TCPIP_FTP_CMD_USER
    "PASS",                         // TCPIP_FTP_CMD_PASS
    "QUIT",                         // TCPIP_FTP_CMD_QUIT
    "STOR",                         // TCPIP_FTP_CMD_STOR
    "PORT",                         // TCPIP_FTP_CMD_PORT
    "ABOR",                         // TCPIP_FTP_CMD_ABORT
    "PWD ",                         // TCPIP_FTP_CMD_PWD
    "CWD ",                         // TCPIP_FTP_CMD_CWD
    "TYPE",                         // TCPIP_FTP_CMD_TYPE
    "RETR",							// TCPIP_FTP_CMD_RETR // GET command
    "SIZE",							// TCPIP_FTP_CMD_SIZE
    "PASV",							//TCPIP_FTP_CMD_PASSIVE
    "NLST",							// TCPIP_FTP_CMD_NAME_LIST  // MGET command
    "EPSV",							// TCPIP_FTP_CMD_EXTND_PASSIVE
    "EPRT",							// TCPIP_FTP_CMD_EXTND_PORT
    "LIST",							// TCPIP_FTP_CMD_EXTND_LIST
    "XPWD",                         // TCPIP_FTP_CMD_XPWD
    "FEAT",                         // TCPIP_FTP_CMD_FEAT
    "SYST",                         // TCPIP_FTP_CMD_SYST
    "MDTM",                         // TCPIP_FTP_CMD_MDMT
    "MLST",                         // TCPIP_FTP_CMD_MLST
    "MLSD",                         // TCPIP_FTP_CMD_MLSD
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
    "500 \r\n",                         // TCPIP_FTP_RESP_STOR_OK
    "502 Not implemented\r\n",          // TCPIP_FTP_RESP_UNKNOWN
    "530 Login required\r\n",           // TCPIP_FTP_RESP_LOGIN
    "150 Transferring data...\r\n",     // TCPIP_FTP_RESP_DATA_OPEN
    "125 File status okay; about to open data connection\r\n",                    	// TCPIP_FTP_RESP_DATA_READY
    "226 Transfer Complete\r\n",        // TCPIP_FTP_RESP_DATA_CLOSE
    "425 Can't create data socket.\r\n",// TCPIP_FTP_RESP_DATA_NO_SOCKET
    "257 Current working Directory \r\n",         // TCPIP_FTP_RESP_PWD
    "200 Ok\r\n",                        // TCPIP_FTP_RESP_OK
    "550 Requested action not taken. \r\n",			// TCPIP_FTP_RESP_FILE_NOT_EXIST
    "150 File status okay; about to open data connection\r\n", //TCPIP_FTP_RESP_FILE_IS_PRESENT
    "227 Entering passive mode ", // TCPIP_FTP_RESP_ENTER_PASV_MODE
    "226 Closing data connection. Requested file action successful.\r\n", //TCPIP_FTP_RESP_FILE_ACTION_SUCCESSFUL_CLOSING_DATA_CONNECTION,
    "213 ",
    "522 Extended PORT failure - Unknown network protocol\r\n", //TCPIP_FTP_RESP_EXTND_PORT_FAILURE
    "421 Service not available, closing control connection\r\n", // TCPIP_FTP_RESP_FILESYSTEM_FAIL
    "215 UNIX Type: L8\r\n",  // TCPIP_FTP_RESP_SYST
    "",  // TCPIP_FTP_RESP_NONE
};

static char *month[]= {"Jan\0","Feb\0","Mar\0","Apr\0","May\0","Jun\0","Jul\0","Aug\0","Sep\0","Oct\0","Nov\0","Dec\0"};

// NLST arguments
#define TCPIP_FTP_MAX_NLST_ARGS  3
static uint8_t gFTPNLSTArgIndex=0xFF;
static uint8_t TCPIP_FTP_CMD_NLSTArgs[TCPIP_FTP_MAX_NLST_ARGS][5]={"-h\0","-a\0","-ha\0"};



static TCPIP_FTP_DCPT* sTCPIPFTPDcpt = 0;

static int              sTCPIPFTPServerCount=0;
static const void*      sTCPIPFtpMemH = 0;  // memory handle
static tcpipSignalHandle ftpsSignalHandle = 0;

// Private helper functions.
static TCPIP_FTP_CMD TCPIP_FTP_CmdsParse(uint8_t *cmd);
static void TCPIP_FTP_CmdStringParse(TCPIP_FTP_DCPT* pFTPDcpt);
static bool TCPIP_FTP_CmdsExecute(TCPIP_FTP_CMD cmd, TCPIP_FTP_DCPT* pFTPDcpt);
#ifdef TCPIP_FTP_PUT_ENABLED
static bool TCPIP_FTP_FilePut(TCPIP_FTP_DCPT* pFTPDcpt);
#endif
static bool TCPIP_FTP_Quit(TCPIP_FTP_DCPT* pFTPDcpt);
static bool TCPIP_FTP_FileGet(TCPIP_FTP_DCPT* pFTPDcpt, uint8_t *cFile);
static bool TCPIP_FTP_ExecuteCmdGet(TCPIP_FTP_DCPT* pFTPDcpt, uint8_t *cFile);
//static bool TCPIP_FTP_NameListCmd(TCPIP_FTP_DCPT* pFTPDcpt, uint8_t *cFile);
static bool TCPIP_FTP_CmdList(TCPIP_FTP_DCPT* pFTPDcpt);
static bool TCPIP_FTP_LSCmd(TCPIP_FTP_DCPT* pFTPDcpt);

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void TCPIP_FTP_Cleanup(const TCPIP_STACK_MODULE_CTRL* const stackData);
#else
#define TCPIP_FTP_Cleanup(stackData)
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static void _FTP_ReleaseDataSocket(TCPIP_FTP_DCPT* pFTPDcpt);

static bool TCPIP_FTP_CreateDataSocket(TCPIP_FTP_DCPT* pFTPDcpt);
static bool TCPIP_FTP_Verify(uint8_t* login,uint8_t* password);
static bool _FTP_Send_ErrorResponse(TCPIP_FTP_DCPT* pFTPDcpt);

static void TCPIP_FTP_ServerProcess(void);

static void _FTPSocketRxSignalHandler(TCP_SOCKET hTCP, TCPIP_NET_HANDLE hNet, TCPIP_TCP_SIGNAL_TYPE sigType, const void* param);

#define mMIN(a, b)	((a<b)?a:b)


static const char TCPIP_FTP_USER_NAME_DEFAULT[]    = TCPIP_FTP_USER_NAME;
static const char TCPIP_FTP_USER_PASS_DEFAULT[]    = TCPIP_FTP_PASSWORD;
static const char TCPIP_FTP_ANNONYMOUS_USER_NAME[]    = "anonymous";

static const TCPIP_TCP_SIGNAL_TYPE ftpClientSignals = TCPIP_TCP_SIGNAL_RX_DATA | TCPIP_TCP_SIGNAL_TX_SPACE;

SGL_LIST_NODE* TCPIP_FTP_LIST_Add(PROTECTED_SINGLE_LIST* FileList, TCPIP_STACK_HEAP_HANDLE heapH, size_t nBytes)
{
    SGL_LIST_NODE* newNode = TCPIP_HEAP_Malloc(heapH, nBytes);

    if(newNode)
    {
        TCPIP_Helper_ProtectedSingleListTailAdd(FileList, newNode);
    }
    return newNode;

}

static bool TCPIP_FTP_Verify(uint8_t* login,uint8_t* password)
{
    if ( strncmp((const char*)login, (const char*)TCPIP_FTP_ANNONYMOUS_USER_NAME, TCPIP_FTP_USER_NAME_LEN) == 0 )
		return true;

    if ( strncmp((const char*)login, (const char*)ftpConfigData.userName, TCPIP_FTP_USER_NAME_LEN) == 0 )
    {
        if ( strncmp((const char*)password, (const char*)ftpConfigData.password, TCPIP_FTP_PASSWD_LEN) == 0)
            return true;
    }
    return false;
}

bool TCPIP_FTP_ServerInitialize(const TCPIP_STACK_MODULE_CTRL* const stackData,
             const TCPIP_FTP_MODULE_CONFIG* ftpData)
{
    int     nServers;
    bool    initFail;
    TCPIP_FTP_DCPT *pDcpt = 0;

    if(stackData->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
       return true;
    }

    // Check to see whether we have been brought up previously. If so, we will move on.
    if(sTCPIPFTPServerCount == 0)
    {
        if(ftpData == 0)
        {
            return false;
        }
        // Check if there is any FTP server connection configured
        if(ftpData->nConnections == 0)
        {
            SYS_ERROR(SYS_ERROR_ERROR, " FTP: No Connection is configured");
            return false;
        }
        // basic sanity check
        if((strlen(ftpData->userName)> TCPIP_FTP_USER_NAME_LEN) ||
                (strlen(ftpData->password)> TCPIP_FTP_PASSWD_LEN))
        {
            return false;
        }
        ftpConfigData.nConnections = ftpData->nConnections;
        ftpConfigData.dataSktRxBuffSize = ftpData->dataSktRxBuffSize;
        ftpConfigData.dataSktTxBuffSize = ftpData->dataSktTxBuffSize;
        strncpy(ftpConfigData.userName, ftpData->userName, TCPIP_FTP_USER_NAME_LEN);
        strncpy(ftpConfigData.password, ftpData->password, TCPIP_FTP_PASSWD_LEN);

        sTCPIPFtpMemH = stackData->memH;
        
        sTCPIPFTPDcpt = (TCPIP_FTP_DCPT*)TCPIP_HEAP_Calloc(sTCPIPFtpMemH, ftpConfigData.nConnections, sizeof(TCPIP_FTP_DCPT));
        if(sTCPIPFTPDcpt == 0)
        {   // failed
            return false;
        }

        // initialize data structures
        initFail = false;
        pDcpt = sTCPIPFTPDcpt+0;
        for(nServers = 0; nServers < ftpConfigData.nConnections; nServers++, pDcpt++)
        {
            pDcpt->ftpCmdskt = TCPIP_TCP_ServerOpen(IP_ADDRESS_TYPE_IPV4, TCPIP_FTP_COMMAND_PORT, 0);
            if(pDcpt->ftpCmdskt == INVALID_SOCKET)
            {   // failed
                SYS_ERROR(SYS_ERROR_ERROR, " FTP: Socket creation failed");
                initFail = true;
                break;
            }
            pDcpt->ftpTcpCmdSocketSignal = TCPIP_TCP_SignalHandlerRegister(pDcpt->ftpCmdskt, TCPIP_TCP_SIGNAL_RX_DATA, _FTPSocketRxSignalHandler, 0);
            if(pDcpt->ftpTcpCmdSocketSignal == 0)
            {
                SYS_ERROR(SYS_ERROR_ERROR, " FTP: TCP Signal creation failed");
                initFail = true;
                break;
            }
            pDcpt->ftpFlag.val = 0;
            pDcpt->ftpDataPort = 0;
            pDcpt->ftpStringLen = 0;
            pDcpt->ftpDataskt = INVALID_SOCKET;
            pDcpt->ftpSm = TCPIP_FTP_SM_NOT_CONNECTED;
            pDcpt->ftpCommand = TCPIP_FTP_CMD_NONE;
            pDcpt->ftpCommandSm = TCPIP_FTP_CMD_SM_IDLE;
            pDcpt->callbackPos = 0;
            pDcpt->ftpTcpDataSocketSignal=0;
            
        }
        if(initFail == true)
        {
            TCPIP_FTP_Cleanup(stackData);
            return false;
        }

        // create the FTP timer
        ftpsSignalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_FTP_ServerTask, TCPIP_FTPS_TASK_TICK_RATE);
        if(ftpsSignalHandle == 0)
        {   // cannot create the FTP timer
            TCPIP_FTP_Cleanup(stackData);
            return false;
        }
    }   

    sTCPIPFTPServerCount++;

    return true;
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_FTP_ServerDeinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData)
{


    if(stackData->stackAction == TCPIP_STACK_ACTION_DEINIT)
    {   // whole stack is going down
        if(--sTCPIPFTPServerCount == 0)
        {   // all closed
            // release resources
            TCPIP_FTP_Cleanup(stackData);
        }
    }
}

static void TCPIP_FTP_Cleanup(const TCPIP_STACK_MODULE_CTRL* const stackData)
{
    int nServers;
    TCPIP_FTP_DCPT* pFTPDcpt;

    if(sTCPIPFTPDcpt != 0)
    {
        for(nServers = 0; nServers < ftpConfigData.nConnections; nServers++)
        {
            pFTPDcpt = sTCPIPFTPDcpt+nServers;
            if(pFTPDcpt->ftpTcpCmdSocketSignal != 0)
            {
                TCPIP_TCP_SignalHandlerDeregister(pFTPDcpt->ftpCmdskt, pFTPDcpt->ftpTcpCmdSocketSignal);
                pFTPDcpt->ftpTcpCmdSocketSignal = 0;
            }
            if(pFTPDcpt->ftpCmdskt != INVALID_SOCKET)
            {
                TCPIP_TCP_Close(pFTPDcpt->ftpCmdskt);
                pFTPDcpt->ftpCmdskt = INVALID_SOCKET;
            }            
        }
        
        TCPIP_HEAP_Free(sTCPIPFtpMemH, sTCPIPFTPDcpt);
        sTCPIPFTPDcpt = 0;
    }

    if(ftpsSignalHandle)
    {
        _TCPIPStackSignalHandlerDeregister(ftpsSignalHandle);
        ftpsSignalHandle = 0;
    }
    
    sTCPIPFtpMemH = 0;
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static bool _FTP_Send_ErrorResponse(TCPIP_FTP_DCPT* pFTPDcpt)
{
    // Make sure there is enough TCP TX FIFO space to put our response
    if(TCPIP_TCP_PutIsReady(pFTPDcpt->ftpCmdskt) < strlen(sTCPIPFTPRespStr[pFTPDcpt->ftpResponse]))
    {
        return false;
    }
    TCPIP_TCP_StringPut(pFTPDcpt->ftpCmdskt, (const uint8_t*)sTCPIPFTPRespStr[pFTPDcpt->ftpResponse]);
    TCPIP_TCP_Flush(pFTPDcpt->ftpCmdskt);

    if(pFTPDcpt->ftpDataskt != INVALID_SOCKET)
    {
        _FTP_ReleaseDataSocket(pFTPDcpt);  
    }
    return true;
}

void TCPIP_FTP_ServerTask(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != 0)
    { // TMO or RX signals occurred
        TCPIP_FTP_ServerProcess();
    }
}


// send a signal to the FTP module that data is available
// no manager alert needed since this normally results as a higher layer (TCP) signal
static void _FTPSocketRxSignalHandler(TCP_SOCKET hTCP, TCPIP_NET_HANDLE hNet, TCPIP_TCP_SIGNAL_TYPE sigType, const void* param)
{
    _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
}

static void TCPIP_FTP_ServerProcess(void)
{
    int         nServers;
    uint32_t 	currentTick;
    TCPIP_FTP_DCPT* pFTPDcpt;

    for(nServers = 0; nServers < ftpConfigData.nConnections; nServers++)
    {
        pFTPDcpt = sTCPIPFTPDcpt + nServers;
        if(pFTPDcpt->ftpCmdskt == INVALID_SOCKET)
        {
            continue;
        }

        if(!TCPIP_TCP_IsConnected(pFTPDcpt->ftpCmdskt) )
        {
            pFTPDcpt->ftpSm = TCPIP_FTP_SM_NOT_CONNECTED;
            pFTPDcpt->ftpCommand = TCPIP_FTP_CMD_NONE;
            pFTPDcpt->ftpFlag.val = 0;
            pFTPDcpt->ftpCommandSm = TCPIP_FTP_CMD_SM_IDLE;
            if(pFTPDcpt->ftpDataskt != INVALID_SOCKET)
            {
                _FTP_ReleaseDataSocket(pFTPDcpt);  
            }
            continue;
        }


        if(TCPIP_TCP_GetIsReady(pFTPDcpt->ftpCmdskt) )
        {
            pFTPDcpt->ftpSysTicklastActivity = SYS_TMR_TickCountGet();
            pFTPDcpt->ftpStringLen = TCPIP_TCP_ArrayGet(pFTPDcpt->ftpCmdskt, pFTPDcpt->ftpCmdString, TCPIP_FTP_CMD_MAX_STRING_LEN);
            if(pFTPDcpt->ftpStringLen == TCPIP_FTP_CMD_MAX_STRING_LEN)
            {
                pFTPDcpt->ftpStringLen = 0;
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_UNKNOWN;
                _FTP_Send_ErrorResponse(pFTPDcpt);
                continue;
            }
            if ( pFTPDcpt->ftpCmdString[pFTPDcpt->ftpStringLen-1] == '\n' )
            {
                pFTPDcpt->ftpCmdString[pFTPDcpt->ftpStringLen] = '\0';
                pFTPDcpt->ftpStringLen = 0;
                TCPIP_FTP_CmdStringParse(pFTPDcpt);
                pFTPDcpt->ftpCommand = TCPIP_FTP_CmdsParse(pFTPDcpt->ftp_argv[0]);
            }
        }
        else if ( pFTPDcpt->ftpSm != TCPIP_FTP_SM_NOT_CONNECTED )
        {
            currentTick = SYS_TMR_TickCountGet();
            currentTick = currentTick - pFTPDcpt->ftpSysTicklastActivity;
            if ( currentTick >= (TCPIP_FTP_TIMEOUT * SYS_TMR_TickCounterFrequencyGet()) )
            {
                pFTPDcpt->ftpSysTicklastActivity = SYS_TMR_TickCountGet();
                pFTPDcpt->ftpCommand	= TCPIP_FTP_CMD_QUIT;
                pFTPDcpt->ftpSm  = TCPIP_FTP_SM_CONNECTED;
            }
        }

        switch(pFTPDcpt->ftpSm)
        {
            case TCPIP_FTP_SM_HOME:
                //No break statement
            case TCPIP_FTP_SM_NOT_CONNECTED:
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_BANNER;
                pFTPDcpt->ftpSysTicklastActivity = SYS_TMR_TickCountGet();
                // No break - Continue...

            case TCPIP_FTP_SM_RESPOND:
                // Make sure there is enough TCP TX FIFO space to put our response
                if(TCPIP_TCP_PutIsReady(pFTPDcpt->ftpCmdskt) < strlen(sTCPIPFTPRespStr[pFTPDcpt->ftpResponse]))
                {
                    break;
                }
                TCPIP_TCP_StringPut(pFTPDcpt->ftpCmdskt, (const uint8_t*)sTCPIPFTPRespStr[pFTPDcpt->ftpResponse]);
                TCPIP_TCP_Flush(pFTPDcpt->ftpCmdskt);
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_NONE;
                pFTPDcpt->ftpSm = TCPIP_FTP_SM_CONNECTED;
                // No break - this will speed up little bit

            case TCPIP_FTP_SM_CONNECTED:
                if ( pFTPDcpt->ftpCommand != TCPIP_FTP_CMD_NONE )
                {
                    if ( TCPIP_FTP_CmdsExecute(pFTPDcpt->ftpCommand, pFTPDcpt) )
                    {
                        if ( pFTPDcpt->ftpResponse != TCPIP_FTP_RESP_NONE )
                        {
                            pFTPDcpt->ftpSm = TCPIP_FTP_SM_RESPOND;
                        }
                        else if ( pFTPDcpt->ftpCommand == TCPIP_FTP_CMD_QUIT )
                        {
                            pFTPDcpt->ftpSm = TCPIP_FTP_SM_NOT_CONNECTED;
                        }

                        pFTPDcpt->ftpCommand = TCPIP_FTP_CMD_NONE;
                        pFTPDcpt->ftpCommandSm = TCPIP_FTP_CMD_SM_IDLE;
                    }
                    else if ( pFTPDcpt->ftpResponse != TCPIP_FTP_RESP_NONE )
                    {
                        pFTPDcpt->ftpSm = TCPIP_FTP_SM_RESPOND;
                    }
                }
                break;
            case TCPIP_FTP_SM_USER_NAME:
            case TCPIP_FTP_SM_USER_PASS:
                break;
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
    int32_t		fp;
    TCP_SOCKET_INFO remoteSockInfo,dataSockInfo;
    char passiveMsg[64];
    uint32_t fileSize=0;
    char tempMsg[10];
    SYS_FS_RESULT   fsRes;

    switch(cmd)
    {
        case TCPIP_FTP_CMD_USER:
            pFTPDcpt->ftpFlag.Bits.userSupplied = true;
            pFTPDcpt->ftpFlag.Bits.loggedIn = false;
            pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_USER_OK;
            if(pFTPDcpt->ftp_argv[1] != 0)
            {
                strncpy((char*)pFTPDcpt->ftpUserName, (char*)pFTPDcpt->ftp_argv[1], TCPIP_FTP_USER_NAME_LEN);
            }
            else
            {
                pFTPDcpt->ftpUserName[0] = '\0';    // empty user name
            }
        break;
        case TCPIP_FTP_CMD_PASS:
            if ( !pFTPDcpt->ftpFlag.Bits.userSupplied )
            {
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_LOGIN;
            }
            else
            {
                if(TCPIP_FTP_Verify(pFTPDcpt->ftpUserName, pFTPDcpt->ftp_argv[1]))
                {
                    pFTPDcpt->ftpFlag.Bits.loggedIn = true;
                    pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_PASS_OK;
                }
                else
                    pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_LOGIN;
            }
        break;
        case TCPIP_FTP_CMD_QUIT:
            return TCPIP_FTP_Quit(pFTPDcpt);

        case TCPIP_FTP_CMD_PORT:
            pFTPDcpt->ftpDataPort = (uint8_t)atoi((char*)pFTPDcpt->ftp_argv[5]);
            pFTPDcpt->ftpDataPort =
                            (pFTPDcpt->ftpDataPort << 8)|(uint8_t)atoi((char*)pFTPDcpt->ftp_argv[6]);
            pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_OK;
            break;
        case TCPIP_FTP_CMD_FEAT:
            {
                char ftpMsg[SYS_FS_MAX_PATH+10]; // 10 for extra parameter
                memset(ftpMsg,0,sizeof(ftpMsg));
                sprintf(ftpMsg,"211-features:\r\n");
                if(TCPIP_TCP_PutIsReady(pFTPDcpt->ftpCmdskt) < strlen(ftpMsg))
                {
                    pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_NONE;
                    pFTPDcpt->ftpSm = TCPIP_FTP_SM_CONNECTED;
                    return true;
                }
                TCPIP_TCP_StringPut(pFTPDcpt->ftpCmdskt, (const uint8_t*)ftpMsg);
                TCPIP_TCP_Flush(pFTPDcpt->ftpCmdskt);
                // send SIZE
                memset(ftpMsg,0,sizeof(ftpMsg));
                sprintf(ftpMsg,"SIZE\r\n");
                if(TCPIP_TCP_PutIsReady(pFTPDcpt->ftpCmdskt) < strlen(ftpMsg))
                {
                    pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_NONE;
                    pFTPDcpt->ftpSm = TCPIP_FTP_SM_CONNECTED;
                    return true;
                }
                TCPIP_TCP_StringPut(pFTPDcpt->ftpCmdskt, (const uint8_t*)ftpMsg);
                TCPIP_TCP_Flush(pFTPDcpt->ftpCmdskt);
//                 // send MLSD
//                memset(ftpMsg,0,sizeof(ftpMsg));
//                sprintf(ftpMsg,"MLSD\r\n");
//                if(TCPIP_TCP_PutIsReady(pFTPDcpt->ftpCmdskt) < strlen(ftpMsg))
//                {
//                    pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_NONE;
//                    pFTPDcpt->ftpSm = TCPIP_FTP_SM_CONNECTED;
//                    return true;
//                }
//                TCPIP_TCP_StringPut(pFTPDcpt->ftpCmdskt, (const uint8_t*)ftpMsg);
//                TCPIP_TCP_Flush(pFTPDcpt->ftpCmdskt);
//
//                 // send MLST
//                memset(ftpMsg,0,sizeof(ftpMsg));
//                sprintf(ftpMsg,"MLST type*;size*;modify*;\r\n");
//                if(TCPIP_TCP_PutIsReady(pFTPDcpt->ftpCmdskt) < strlen(ftpMsg))
//                {
//                    pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_NONE;
//                    pFTPDcpt->ftpSm = TCPIP_FTP_SM_CONNECTED;
//                    return true;
//                }
//                TCPIP_TCP_StringPut(pFTPDcpt->ftpCmdskt, (const uint8_t*)ftpMsg);
//                TCPIP_TCP_Flush(pFTPDcpt->ftpCmdskt);

                //End of FEAT fetures support
                 memset(ftpMsg,0,sizeof(ftpMsg));
                sprintf(ftpMsg,"211 End\r\n");
                if(TCPIP_TCP_PutIsReady(pFTPDcpt->ftpCmdskt) < strlen(ftpMsg))
                {
                    pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_NONE;
                    pFTPDcpt->ftpSm = TCPIP_FTP_SM_CONNECTED;
                    return true;
                }
                TCPIP_TCP_StringPut(pFTPDcpt->ftpCmdskt, (const uint8_t*)ftpMsg);
                TCPIP_TCP_Flush(pFTPDcpt->ftpCmdskt);

                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_NONE;
                pFTPDcpt->ftpSm = TCPIP_FTP_SM_CONNECTED;
            }
            break;
        case TCPIP_FTP_CMD_SYST:
            pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_SYST;
            break;
        case TCPIP_FTP_CMD_MLST:
            break;
        case TCPIP_FTP_CMD_MLSD:
            break;
        case TCPIP_FTP_CMD_MDTM:
            break;
        case TCPIP_FTP_CMD_STOR:
#ifdef TCPIP_FTP_PUT_ENABLED
            return TCPIP_FTP_FilePut(pFTPDcpt);
#else
            pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_OK;
#endif
        case TCPIP_FTP_CMD_XPWD:
        case TCPIP_FTP_CMD_PWD:
        {
            char pwdbuf[SYS_FS_MAX_PATH]={0};
            char ftpMsg[SYS_FS_MAX_PATH+10]; // 10 for extra parameter
            memset(pwdbuf,0,sizeof(pwdbuf));
            fsRes = SYS_FS_CurrentWorkingDirectoryGet(pwdbuf,sizeof(pwdbuf));
            if(fsRes == SYS_FS_RES_FAILURE)
            {
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_FILE_NOT_EXIST;
                return true;
            }
            memset(ftpMsg,0,sizeof(ftpMsg));
            sprintf(ftpMsg,"257 \"%s\" is cwd\r\n",pwdbuf);
            if(TCPIP_TCP_PutIsReady(pFTPDcpt->ftpCmdskt) < strlen(ftpMsg))
            {
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_NONE;
                pFTPDcpt->ftpSm = TCPIP_FTP_SM_CONNECTED;
                return true;
            }
            TCPIP_TCP_StringPut(pFTPDcpt->ftpCmdskt, (const uint8_t*)ftpMsg);
            TCPIP_TCP_Flush(pFTPDcpt->ftpCmdskt);
            pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_NONE;
            pFTPDcpt->ftpSm = TCPIP_FTP_SM_CONNECTED;
        }
        break;

        case TCPIP_FTP_CMD_CWD:
        {
            char pwdbuf[SYS_FS_MAX_PATH]={0};
            char ftpMsg[SYS_FS_MAX_PATH+10]; // 10 for extra parameter
            int  localPathlen=0,strlencmp=0;
            // check if the CWD is Same as the LOCAL_WEBSITE_PATH
            // change the directory to root path and then change path
            {
                char pwdbuf1[SYS_FS_MAX_PATH]={0};
                memset(pwdbuf1,0,sizeof(pwdbuf1));
                fsRes = SYS_FS_CurrentWorkingDirectoryGet(pwdbuf1,sizeof(pwdbuf1));
            	if(fsRes == SYS_FS_RES_FAILURE)
                {
                    pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_FILE_NOT_EXIST;
                    return true;
                }
                localPathlen = strlen((char*)pwdbuf1);
                strlencmp = memcmp(pwdbuf1,LOCAL_WEBSITE_PATH,localPathlen);
                while(strlencmp != 0)
                {
                    if(strlencmp > 0)
                    {
                        SYS_FS_DirectoryChange((const char*)"..");
                    }
                    else
                    {
                        break;
                    }
                    memset(pwdbuf1,0,sizeof(pwdbuf1));
                    fsRes = SYS_FS_CurrentWorkingDirectoryGet(pwdbuf1,sizeof(pwdbuf1));
                    if(fsRes == SYS_FS_RES_FAILURE)
                    {
		                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_FILE_NOT_EXIST;
		                return true;
	            	}
                    localPathlen = strlen((char*)pwdbuf1);
                    strlencmp = memcmp(pwdbuf1,LOCAL_WEBSITE_PATH,localPathlen);
                }
            }

            fsRes = SYS_FS_DirectoryChange((const char*)pFTPDcpt->ftp_argv[1]);
            if(fsRes == SYS_FS_RES_FAILURE)
            {// No such directory
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_FILE_NOT_EXIST;
                return true;
            }
            memset(pwdbuf,0,sizeof(pwdbuf));
            fsRes = SYS_FS_CurrentWorkingDirectoryGet(pwdbuf,sizeof(pwdbuf));
            if(fsRes == SYS_FS_RES_FAILURE)
            {
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_FILE_NOT_EXIST;
                return true;
            }
            sprintf(ftpMsg,"250 %s is new cwd\r\n",pwdbuf);
            if(TCPIP_TCP_PutIsReady(pFTPDcpt->ftpCmdskt) < strlen(ftpMsg))
            {
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_NONE;
                pFTPDcpt->ftpSm = TCPIP_FTP_SM_CONNECTED;
                return true;
            }
            TCPIP_TCP_StringPut(pFTPDcpt->ftpCmdskt, (const uint8_t*)ftpMsg);
            TCPIP_TCP_Flush(pFTPDcpt->ftpCmdskt);
            pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_NONE;
            pFTPDcpt->ftpSm = TCPIP_FTP_SM_CONNECTED;
        }
        break;

        case TCPIP_FTP_CMD_TYPE:
            pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_OK;
            break;

        case TCPIP_FTP_CMD_ABORT:
            pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_OK;
            if (pFTPDcpt->ftpDataskt!= INVALID_SOCKET)
            {
                _FTP_ReleaseDataSocket(pFTPDcpt);  
            }
            break;
        case TCPIP_FTP_CMD_NLST:
            return TCPIP_FTP_LSCmd(pFTPDcpt);
        case TCPIP_FTP_CMD_EXTND_LIST:
            return TCPIP_FTP_CmdList(pFTPDcpt);
            break;
        case TCPIP_FTP_CMD_RETR:
            if(pFTPDcpt->callbackPos == 0x00u)
            {
                fp = SYS_FS_FileOpen_Wrapper((const char*)pFTPDcpt->ftp_argv[1],SYS_FS_FILE_OPEN_READ);
                if (fp == SYS_FS_HANDLE_INVALID)
                {// File not found, so abort
                    pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_FILE_NOT_EXIST;
                    return true;
                }
                SYS_FS_FileClose(fp);
            }
            return TCPIP_FTP_ExecuteCmdGet(pFTPDcpt, pFTPDcpt->ftp_argv[1]);

        case TCPIP_FTP_CMD_SIZE:
            fp = SYS_FS_FileOpen_Wrapper((const char*)pFTPDcpt->ftp_argv[1],SYS_FS_FILE_OPEN_READ);
            if(fp == SYS_FS_HANDLE_INVALID)
            {// File not found, so abort
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_FILE_NOT_EXIST;
                return true;
            }
            fileSize = SYS_FS_FileSize(fp);
            strcpy(tempMsg,"");
            sprintf(tempMsg,"213 %u\r\n",fileSize);

            if(TCPIP_TCP_PutIsReady(pFTPDcpt->ftpCmdskt) < strlen(tempMsg))
            {
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_NONE;
                pFTPDcpt->ftpSm = TCPIP_FTP_SM_CONNECTED;
                SYS_FS_FileClose(fp);
                return true;
            }
            TCPIP_TCP_StringPut(pFTPDcpt->ftpCmdskt, (const uint8_t*)tempMsg);
            TCPIP_TCP_Flush(pFTPDcpt->ftpCmdskt);
            pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_NONE;
            pFTPDcpt->ftpSm = TCPIP_FTP_SM_CONNECTED;
            SYS_FS_FileClose(fp);
            break;

        case TCPIP_FTP_CMD_PASV:
            // create a server socket with a available port number and send this port number to the client with Response string.
            pFTPDcpt->ftpDataskt = TCPIP_TCP_ServerOpen(IP_ADDRESS_TYPE_IPV4, 0,0);
            // Make sure that a valid socket was available and returned
            // If not, return with an error
            if(pFTPDcpt->ftpDataskt == INVALID_SOCKET)
            {
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_DATA_NO_SOCKET;
                return true;
            }

            // catch the RX/TX signals
            pFTPDcpt->ftpTcpDataSocketSignal = 
                    TCPIP_TCP_SignalHandlerRegister(pFTPDcpt->ftpDataskt, ftpClientSignals, _FTPSocketRxSignalHandler, 0);
            if(pFTPDcpt->ftpTcpDataSocketSignal == 0)
            {
                TCPIP_TCP_Close(pFTPDcpt->ftpDataskt);
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_DATA_NO_SOCKET;
                return true;
            }
            // modify the response string.
            //response string should have server ip address and the new data port number.

            TCPIP_TCP_SocketInfoGet(pFTPDcpt->ftpCmdskt, &remoteSockInfo);

            TCPIP_TCP_SocketInfoGet(pFTPDcpt->ftpDataskt, &dataSockInfo);
            //prepare additional message IPaddress + Port
            strcpy(passiveMsg,"");
            // add IPv4 address
            sprintf(passiveMsg,"227 Entering passive mode (%u,%u,%u,%u,%u,%u)\r\n",remoteSockInfo.localIPaddress.v4Add.v[0],
                                                            remoteSockInfo.localIPaddress.v4Add.v[1],
                                                            remoteSockInfo.localIPaddress.v4Add.v[2],
                                                            remoteSockInfo.localIPaddress.v4Add.v[3],
                                                            dataSockInfo.localPort>>8 & 0xFF,
                                                            dataSockInfo.localPort & 0xFF);
            //strcat(&sTCPIPFTPRespStr[pFTPDcpt->ftpResponse],passiveMsg);
            pFTPDcpt->ftpDataPort = dataSockInfo.localPort;

            if(TCPIP_TCP_PutIsReady(pFTPDcpt->ftpCmdskt) < strlen(passiveMsg))
            {
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_NONE;
                pFTPDcpt->ftpSm = TCPIP_FTP_SM_CONNECTED;
                return true;
            }
            TCPIP_TCP_StringPut(pFTPDcpt->ftpCmdskt, (const uint8_t*)passiveMsg);
            TCPIP_TCP_Flush(pFTPDcpt->ftpCmdskt);
            pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_NONE;
            pFTPDcpt->ftpSm = TCPIP_FTP_SM_CONNECTED;
#if (TCPIP_TCP_DYNAMIC_OPTIONS != 0)
            if(ftpConfigData.dataSktTxBuffSize != 0)
            {
                void* tcpBuffSize = (void*)(unsigned int)ftpConfigData.dataSktTxBuffSize;
                if(TCPIP_TCP_OptionsSet(pFTPDcpt->ftpDataskt, TCP_OPTION_TX_BUFF, tcpBuffSize)==false)
                {
                    return false;
                }
            }

            if(ftpConfigData.dataSktRxBuffSize != 0)
            {
                void* tcpBuffSize = (void*)(unsigned int)ftpConfigData.dataSktRxBuffSize;
                if(TCPIP_TCP_OptionsSet(pFTPDcpt->ftpDataskt, TCP_OPTION_RX_BUFF, tcpBuffSize)==false)
                {
                    return false;
                }
            }
#endif  // (TCPIP_TCP_DYNAMIC_OPTIONS != 0)
            pFTPDcpt->ftpFlag.Bits.pasvMode = true;
            break;
        case TCPIP_FTP_CMD_EPSV:
            // RFC 2428 , EPSV<space><net-prt>  or EPSV<space>ALL nned to be handled properly.
            // for Time being If there is any argument return Error code 522. dont start any data communication.
            if(pFTPDcpt->ftp_argc > 1)
            {
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_EXTND_PORT_FAILURE;
                return true;
            }
            // create a server socket with a available port number and send this port number to the client with Response string.
            pFTPDcpt->ftpDataskt = TCPIP_TCP_ServerOpen(IP_ADDRESS_TYPE_IPV4, 0,0);
            // Make sure that a valid socket was available and returned
            // If not, return with an error
            if(pFTPDcpt->ftpDataskt == INVALID_SOCKET)
            {
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_DATA_NO_SOCKET;
                return true;
            }

            // catch the RX/TX signals
            pFTPDcpt->ftpTcpDataSocketSignal = 
                    TCPIP_TCP_SignalHandlerRegister(pFTPDcpt->ftpDataskt, ftpClientSignals, _FTPSocketRxSignalHandler, 0);
            
            if(pFTPDcpt->ftpTcpDataSocketSignal == 0)
            {
                TCPIP_TCP_Close(pFTPDcpt->ftpDataskt);
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_DATA_NO_SOCKET;
                return true;
            }
            // modify the response string.
            //response string should have server ip address and the new data port number.

            TCPIP_TCP_SocketInfoGet(pFTPDcpt->ftpCmdskt, &remoteSockInfo);

            TCPIP_TCP_SocketInfoGet(pFTPDcpt->ftpDataskt, &dataSockInfo);
            //prepare additional message IPaddress + Port
            strcpy(passiveMsg,"");


            sprintf(passiveMsg,"229 Extended passive mode entered (|||%u|)\r\n",dataSockInfo.localPort);
            pFTPDcpt->ftpDataPort = dataSockInfo.localPort;

            if(TCPIP_TCP_PutIsReady(pFTPDcpt->ftpCmdskt) < strlen(passiveMsg))
            {
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_NONE;
                pFTPDcpt->ftpSm = TCPIP_FTP_SM_CONNECTED;
                return true;
            }
            TCPIP_TCP_StringPut(pFTPDcpt->ftpCmdskt, (const uint8_t*)passiveMsg);
            TCPIP_TCP_Flush(pFTPDcpt->ftpCmdskt);
            pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_NONE;
            pFTPDcpt->ftpSm = TCPIP_FTP_SM_CONNECTED;
#if (TCPIP_TCP_DYNAMIC_OPTIONS != 0)
            if(ftpConfigData.dataSktTxBuffSize != 0)
            {
                void* tcpBuffSize = (void*)(unsigned int)ftpConfigData.dataSktTxBuffSize;
                if(TCPIP_TCP_OptionsSet(pFTPDcpt->ftpDataskt, TCP_OPTION_TX_BUFF, tcpBuffSize)==false)
                {
                    return false;
                }
            }

            if(ftpConfigData.dataSktRxBuffSize != 0)
            {
                void* tcpBuffSize = (void*)(unsigned int)ftpConfigData.dataSktRxBuffSize;
                if(TCPIP_TCP_OptionsSet(pFTPDcpt->ftpDataskt, TCP_OPTION_RX_BUFF, tcpBuffSize)==false)
                {
                    return false;
                }
            }
#endif  // (TCPIP_TCP_DYNAMIC_OPTIONS != 0)
            pFTPDcpt->ftpFlag.Bits.pasvMode = true;
            break;
        case TCPIP_FTP_CMD_EPRT:
            pFTPDcpt->adressFamilyProtocol = atoi((char *)pFTPDcpt->ftp_argv[1]);
            pFTPDcpt->ftpDataPort = atoi((char *)pFTPDcpt->ftp_argv[3]);
            pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_OK;
            break;
/*
        case TCPIP_FTP_CMD_NLST:
            return TCPIP_FTP_NameListCmd(pFTPDcpt, pFTPDcpt->ftp_argv[1]);
            break;
*/
	default:
	    pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_UNKNOWN;
	    break;
    }
    return true;
}

static bool TCPIP_FTP_Quit(TCPIP_FTP_DCPT* pFTPDcpt)
{
    switch(pFTPDcpt->ftpCommandSm)
    {
        case TCPIP_FTP_CMD_SM_IDLE:
            if ( pFTPDcpt->ftpCommandSm == TCPIP_FTP_CMD_SM_RECEIVE )
            {
                SYS_FS_FileClose(pFTPDcpt->fileDescr);
            }

            if ( pFTPDcpt->ftpDataskt != INVALID_SOCKET )
            {
                if(pFTPDcpt->ftpTcpDataSocketSignal != 0)
                {
                    TCPIP_TCP_SignalHandlerDeregister(pFTPDcpt->ftpDataskt, pFTPDcpt->ftpTcpDataSocketSignal);
                }
                SYS_FS_FileClose(pFTPDcpt->fileDescr);
                TCPIP_TCP_Close(pFTPDcpt->ftpDataskt);
                pFTPDcpt->ftpCommandSm = TCPIP_FTP_CMD_SM_WAIT;
            }
            else
            {
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_QUIT_OK;
                pFTPDcpt->ftpCommandSm = TCPIP_FTP_CMD_SM_WAIT_FOR_DISCONNECT;
            }
            break;

        case TCPIP_FTP_CMD_SM_WAIT:
            if ( !TCPIP_TCP_IsConnected(pFTPDcpt->ftpDataskt) )
            {
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_QUIT_OK;
                pFTPDcpt->ftpCommandSm = TCPIP_FTP_CMD_SM_WAIT_FOR_DISCONNECT;
            }
            break;

        case TCPIP_FTP_CMD_SM_WAIT_FOR_DISCONNECT:
            if ( TCPIP_TCP_PutIsReady(pFTPDcpt->ftpCmdskt) )
            {
                if ( TCPIP_TCP_IsConnected(pFTPDcpt->ftpCmdskt) )
                    TCPIP_TCP_Disconnect(pFTPDcpt->ftpCmdskt);
            }
            break;
        default:
            break;

    }
    return false;
}
/*
 * Release FTP data socket and also release signal.
 */
static void _FTP_ReleaseDataSocket(TCPIP_FTP_DCPT* pFTPDcpt)
{
    pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_DATA_NO_SOCKET;
    
    if(pFTPDcpt->ftpTcpDataSocketSignal != 0)
    {
        TCPIP_TCP_SignalHandlerDeregister(pFTPDcpt->ftpDataskt, pFTPDcpt->ftpTcpDataSocketSignal);
        pFTPDcpt->ftpTcpDataSocketSignal = 0;
    }
    if(pFTPDcpt->ftpDataskt != INVALID_SOCKET)
    {
        TCPIP_TCP_Close(pFTPDcpt->ftpDataskt);
    }
    pFTPDcpt->ftpDataskt = INVALID_SOCKET;
}

static bool TCPIP_FTP_CreateDataSocket(TCPIP_FTP_DCPT* pFTPDcpt)
{
    TCP_SOCKET_INFO remoteSockInfo;
    bool releaseDataSkt=false;

    TCPIP_TCP_SocketInfoGet(pFTPDcpt->ftpCmdskt, &remoteSockInfo);
    pFTPDcpt->ftpDataskt   =
        TCPIP_TCP_ClientOpen(IP_ADDRESS_TYPE_IPV4,pFTPDcpt->ftpDataPort,0);

    // Make sure that a valid socket was available and returned
    // If not, return with an error
    if(pFTPDcpt->ftpDataskt == INVALID_SOCKET)
    {
        pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_DATA_NO_SOCKET;
        return false;
    }
    // catch the RX/TX signals
    pFTPDcpt->ftpTcpDataSocketSignal = 
            TCPIP_TCP_SignalHandlerRegister(pFTPDcpt->ftpDataskt, ftpClientSignals, _FTPSocketRxSignalHandler, 0);
    if(pFTPDcpt->ftpTcpDataSocketSignal == 0)
    {
        releaseDataSkt = true;               
    }
    
    if(TCPIP_TCP_Bind(pFTPDcpt->ftpDataskt,IP_ADDRESS_TYPE_IPV4,TCPIP_FTP_DATA_PORT,0) == false)
    {
        releaseDataSkt = true;       
    }
    if(TCPIP_TCP_RemoteBind(pFTPDcpt->ftpDataskt, IP_ADDRESS_TYPE_IPV4, 0, &remoteSockInfo.remoteIPaddress)== false)
    {
        releaseDataSkt = true;        
    }    
   
    if(releaseDataSkt)
    {
        _FTP_ReleaseDataSocket(pFTPDcpt);
        return false;
    }
    
    // Establish a connection
    if(TCPIP_TCP_Connect(pFTPDcpt->ftpDataskt) == false)
    {
        return false;
    }
#if (TCPIP_TCP_DYNAMIC_OPTIONS != 0)
    if(ftpConfigData.dataSktTxBuffSize != 0)
    {
        void* tcpBuffSize = (void*)(unsigned int)ftpConfigData.dataSktTxBuffSize;
        if(TCPIP_TCP_OptionsSet(pFTPDcpt->ftpDataskt, TCP_OPTION_TX_BUFF, tcpBuffSize)==false)
        {
            return false;
        }
    }

    if(ftpConfigData.dataSktRxBuffSize != 0)
    {
        void* tcpBuffSize = (void*)(unsigned int)ftpConfigData.dataSktRxBuffSize;
        if(TCPIP_TCP_OptionsSet(pFTPDcpt->ftpDataskt, TCP_OPTION_RX_BUFF, tcpBuffSize)== false)
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
    int32_t    fp;
    int32_t wCount, wLen,status;

    fp = pFTPDcpt->fileDescr;
    switch(pFTPDcpt->ftpCommandSm)
    {
        case TCPIP_FTP_CMD_SM_IDLE:
            if ( !pFTPDcpt->ftpFlag.Bits.loggedIn )
            {
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_LOGIN;
                return true;
            }
            else
            {
                if(pFTPDcpt->ftpFlag.Bits.pasvMode != true)
                {
                    if(TCPIP_FTP_CreateDataSocket(pFTPDcpt) != true)
                        return true;
                }
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_FILE_IS_PRESENT;
                pFTPDcpt->ftpCommandSm = TCPIP_FTP_CMD_SM_WAIT;
            }
            break;

        case TCPIP_FTP_CMD_SM_WAIT:
            if ( TCPIP_TCP_IsConnected(pFTPDcpt->ftpDataskt) && (pFTPDcpt->callbackPos == 0x00u))
            {
                fp = SYS_FS_FileOpen_Wrapper((const char*)pFTPDcpt->ftp_argv[1],SYS_FS_FILE_OPEN_WRITE);
                if (fp == SYS_FS_HANDLE_INVALID)
                {// File not found, so abort
                    _FTP_ReleaseDataSocket(pFTPDcpt);
                    break;
                }
                pFTPDcpt->fileDescr = fp;
                pFTPDcpt->ftpCommandSm    = TCPIP_FTP_CMD_SM_RECEIVE;
            }
            else
            {
            	break;
            }
        case TCPIP_FTP_CMD_SM_RECEIVE:
            if(TCPIP_TCP_IsConnected(pFTPDcpt->ftpDataskt) && pFTPDcpt->callbackPos != 0x00u)
            {// The file was already opened, so load up its ID and seek
                if(fp == SYS_FS_HANDLE_INVALID)
                {// No file handles available, so wait for now
                    pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_FILESYSTEM_FAIL;
                    pFTPDcpt->callbackPos = 0;
                    _FTP_ReleaseDataSocket(pFTPDcpt);                    
                    return true;
                }
                SYS_FS_FileSeek(fp,(int32_t)pFTPDcpt->callbackPos,SYS_FS_SEEK_SET);
            }
            else if(!TCPIP_TCP_IsConnected(pFTPDcpt->ftpDataskt))
            {
            // If no bytes were read, an EOF was reached
                SYS_FS_FileClose(pFTPDcpt->fileDescr);
                pFTPDcpt->fileDescr = SYS_FS_HANDLE_INVALID;
                pFTPDcpt->callbackPos = 0;
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_FILE_ACTION_SUCCESSFUL_CLOSING_DATA_CONNECTION;
                pFTPDcpt->ftpFlag.Bits.pasvMode = false;
                _FTP_ReleaseDataSocket(pFTPDcpt);                    
                return true;
            }
            // Get/put as many bytes as possible
            while(1)
            {
            	wCount = TCPIP_TCP_GetIsReady(pFTPDcpt->ftpDataskt);
                if(wCount == 0)
            	{
                    break;
                }
                memset(data,'\0',sizeof(data));
                wLen = TCPIP_TCP_ArrayGet(pFTPDcpt->ftpDataskt, data, mMIN(wCount, sizeof(data)));
                if(wLen == 0)
                {// If no bytes were read, an EOF was reached
                    SYS_FS_FileClose(pFTPDcpt->fileDescr);
                    pFTPDcpt->fileDescr = SYS_FS_HANDLE_INVALID;
                    pFTPDcpt->callbackPos = 0;
                    pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_DATA_CLOSE;
                    pFTPDcpt->ftpFlag.Bits.pasvMode = false;
                    _FTP_ReleaseDataSocket(pFTPDcpt);                    
                    return true;
                }
                else  //if(wLen != 0)
                {// Write the bytes to the socket
                    if(SYS_FS_FileWrite(fp,data,wLen) == SYS_FS_HANDLE_INVALID)
                    {
                        SYS_FS_FileClose(pFTPDcpt->fileDescr);
                        pFTPDcpt->fileDescr = SYS_FS_HANDLE_INVALID;
                        pFTPDcpt->callbackPos = 0;
                        pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_DATA_CLOSE;
                        pFTPDcpt->ftpFlag.Bits.pasvMode = false;
                        _FTP_ReleaseDataSocket(pFTPDcpt);                    
                        return true;
                    }
                    wCount -= wLen;
                }
            }
            status = SYS_FS_FileTell(fp);
            if(status == -1)
                pFTPDcpt->callbackPos = 0;
            else
                pFTPDcpt->callbackPos = (uint32_t)status;
	default:
            break;
    }
    return false;
}
#endif

static bool TCPIP_FTP_CmdList(TCPIP_FTP_DCPT* pFTPDcpt)
{
    uint16_t wCount=0;
    char longFileName[SYS_FS_MAX_PATH];
    static uint8_t fileNameList[TCPIP_FTP_MAX_FILE_NAME_LEN+
                    TCPIP_FTP_MAX_FILE_DATE_TIME_STR_LEN+
                    TCPIP_FTP_MAX_FILE_SIZE_STR_LEN+
                    +20];
    char FileRecordsDateTime[TCPIP_FTP_MAX_FILE_DATE_TIME_STR_LEN];
    char fileRecrdTime[13];
    char FileRecordssize[TCPIP_FTP_MAX_FILE_SIZE_STR_LEN];
    char    *fileHeaderStr= "Date\t\t\tType\tFileSize\tfilename\t\r\n\0";
    uint8_t FileRecordInformation;
    FTP_LIST_NODE* newNode=NULL;
    uint16_t    lfNameLen=0;
    char    *lfnamePtr=NULL;
    static uint16_t  remainingBytes=0;
    SYS_FS_FSTAT fs_stat={0};
    int32_t fp;
    char filePermission[2][11]={"-rwx------\0","drwx------\0"};
    char *link = "0\0";
    char *owner = "0\0";
    char *group = "0\0";

    switch(pFTPDcpt->ftpCommandSm)
    {
        case TCPIP_FTP_CMD_SM_IDLE:
            if(pFTPDcpt->ftpFlag.Bits.pasvMode != true)
            {
                if(TCPIP_FTP_CreateDataSocket(pFTPDcpt) != true)
                {
                    return true;
                }
            }
            if ( !pFTPDcpt->ftpFlag.Bits.loggedIn )
            {
                pFTPDcpt->ftpResponse    = TCPIP_FTP_RESP_LOGIN;
                _FTP_ReleaseDataSocket(pFTPDcpt);                    
                return true;
            }
            pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_DATA_READY;
            pFTPDcpt->ftpCommandSm = TCPIP_FTP_CMD_SM_WAIT;
            break;
        case TCPIP_FTP_CMD_SM_WAIT:
            if ( TCPIP_TCP_IsConnected(pFTPDcpt->ftpDataskt) )
            {
                pFTPDcpt->ftpCommandSm   = TCPIP_FTP_CMD_SM_SEND_DIR;
            }
            break;
       case TCPIP_FTP_CMD_SM_SEND_DIR:
            // get the current working directory to get the directory details
            // used longFileName variable to get the path details
            fp = SYS_FS_DirOpen_Wrapper(LOCAL_WEBSITE_PATH);
            if (fp == SYS_FS_HANDLE_INVALID)
            {// File not found, so abort
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_FILE_NOT_EXIST;
                _FTP_ReleaseDataSocket(pFTPDcpt);                    
                return true;
            }
            pFTPDcpt->fileDescr = fp;
            pFTPDcpt->callbackPos = 0;
            pFTPDcpt->ftpCommandSm  = TCPIP_FTP_CMD_SM_SEND_DIR_HEADER;
        case TCPIP_FTP_CMD_SM_SEND_DIR_HEADER:
            wCount = TCPIP_TCP_PutIsReady(pFTPDcpt->ftpDataskt);
             // send the Header details of the table
            if(pFTPDcpt->callbackPos != 0)
            {
                if(wCount < remainingBytes)
                {
                    TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, fileNameList+(strlen((char *)fileNameList)-remainingBytes), wCount);
                    remainingBytes = remainingBytes - wCount;
                }
                else
                {
                    TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, fileNameList+(strlen((char *)fileNameList)-remainingBytes), remainingBytes);
                    remainingBytes =0;
                    pFTPDcpt->callbackPos = 0;
                }
                break;
            }
            else
            {
                memset(fileNameList,0,sizeof(fileNameList));
                strncpy((char*)fileNameList,fileHeaderStr,sizeof(fileNameList));
                if(wCount > strlen((char *)fileNameList))
                {
                    //TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, fileNameList, strlen((char *)fileNameList));
                    remainingBytes =0;
                    pFTPDcpt->callbackPos = 0;
                }
                else
                {
                    pFTPDcpt->callbackPos = 1;
                    TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, fileNameList, mMIN(wCount, strlen((char *)fileNameList)));
                    remainingBytes = strlen((char *)fileNameList)- wCount;
                    break;
                }
            }
            // If long file name is used, the following elements of the "stat"
             // structure needs to be initialized with address of proper buffer.
            memset(longFileName,0,sizeof(longFileName));
            fs_stat.lfname = longFileName;
            fs_stat.lfsize = SYS_FS_MAX_PATH;
            fp = pFTPDcpt->fileDescr;
            if(TCPIP_Helper_ProtectedSingleListInitialize(&DirectoryFileList)!= true)
            {
                return true;
            }
            while(1)
            {
                memset(fs_stat.fname,0,13);
                memset(longFileName,0,sizeof(longFileName));
                if(SYS_FS_DirSearch(fp, "*", SYS_FS_ATTR_ARC | SYS_FS_ATTR_DIR, &fs_stat) == SYS_FS_RES_FAILURE)
                {
                    SYS_FS_DirClose(fp);
                    pFTPDcpt->fileDescr = SYS_FS_HANDLE_INVALID;
                    break;
                }
                newNode = (FTP_LIST_NODE*)TCPIP_FTP_LIST_Add(&DirectoryFileList,sTCPIPFtpMemH , sizeof(*newNode));
                if(newNode == NULL)
                {
                    SYS_FS_DirClose(fp);
                    pFTPDcpt->fileDescr = SYS_FS_HANDLE_INVALID;
                    pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_FILE_NOT_EXIST;
                    _FTP_ReleaseDataSocket(pFTPDcpt);                    
                    if(DirectoryFileList.list.nNodes > 0)
                    {	// Free if already there are node in the list
                        while(newNode != 0)
                        {
                            if(newNode->file_stat.lfname != NULL)
                            {
                                TCPIP_HEAP_Free(sTCPIPFtpMemH,newNode->file_stat.lfname);
                            }
                            newNode = (FTP_LIST_NODE*)TCPIP_Helper_ProtectedSingleListHeadRemove(&DirectoryFileList);
                            TCPIP_HEAP_Free(sTCPIPFtpMemH, newNode);
                        }
                    }
                    TCPIP_Helper_ProtectedSingleListDeinitialize(&DirectoryFileList);
                    return true;
                }
                newNode->file_stat.fattrib = fs_stat.fattrib;
                newNode->file_stat.fdate = fs_stat.fdate;
                newNode->file_stat.ftime = fs_stat.ftime;
                memset(newNode->file_stat.fname,0,sizeof(newNode->file_stat.fname));
                strcpy((char*)newNode->file_stat.fname ,(char*)fs_stat.fname);
                newNode->file_stat.fsize = fs_stat.fsize;
                newNode->file_stat.lfsize = fs_stat.lfsize;
                lfNameLen = strlen((char*)fs_stat.lfname);
                newNode->file_stat.lfname = NULL;
                if(lfNameLen>0)
                {
                    lfnamePtr = TCPIP_HEAP_Calloc(sTCPIPFtpMemH,1, lfNameLen+1);
                    if(lfnamePtr != NULL)
                    {
                        newNode->file_stat.lfname = lfnamePtr;
                        memcpy(newNode->file_stat.lfname,fs_stat.lfname,lfNameLen);
                        newNode->file_stat.lfname[lfNameLen+1]='\0';
                    }
                    else
                    {
                        SYS_FS_DirClose(fp);
                        pFTPDcpt->fileDescr = SYS_FS_HANDLE_INVALID;
                        pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_FILE_NOT_EXIST;
                        _FTP_ReleaseDataSocket(pFTPDcpt);                    
                        if(DirectoryFileList.list.nNodes > 0)
                        {	// Free if already there are node in the list
                            while(newNode != 0)
                            {
                                if(newNode->file_stat.lfname != NULL)
                                {
                                    TCPIP_HEAP_Free(sTCPIPFtpMemH,newNode->file_stat.lfname);
                                }
                                newNode = (FTP_LIST_NODE*)TCPIP_Helper_ProtectedSingleListHeadRemove(&DirectoryFileList);
                                TCPIP_HEAP_Free(sTCPIPFtpMemH, newNode);
                            }
                        }
                        TCPIP_Helper_ProtectedSingleListDeinitialize(&DirectoryFileList);
                        return true;
                    }
                }
            }
            pFTPDcpt->ftpCommandSm  = TCPIP_FTP_CMD_SM_SEND_DIR_DETAIL;
            // continue
        case TCPIP_FTP_CMD_SM_SEND_DIR_DETAIL:
            wCount = 0;
            newNode = (FTP_LIST_NODE*)DirectoryFileList.list.head;
            wCount = TCPIP_TCP_PutIsReady(pFTPDcpt->ftpDataskt);
            while(newNode != 0)
            {
                if(wCount == 0)
                {
                    return false;
                }
                // transmit the remaining bytes
                if(pFTPDcpt->callbackPos != 0)
                {
                    if(wCount < remainingBytes)
                    {
                        TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, fileNameList+(strlen((char *)fileNameList)-remainingBytes), wCount);
                        remainingBytes = remainingBytes - wCount;
                        wCount=0;
                        continue;
                    }
                    else
                    {
                        TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, fileNameList+(strlen((char *)fileNameList)-remainingBytes), remainingBytes);
                        wCount = wCount - remainingBytes;
                        remainingBytes =0;
                        pFTPDcpt->callbackPos = 0;

                        // remove the head from the list
                        newNode = (FTP_LIST_NODE*)TCPIP_Helper_ProtectedSingleListHeadRemove(&DirectoryFileList);
                        TCPIP_HEAP_Free(sTCPIPFtpMemH, newNode);
                        newNode = (FTP_LIST_NODE*)DirectoryFileList.list.head;
                        if(newNode == 0)
                        {
                            break;
                        }
                    }
                }
                memset(FileRecordsDateTime,0,sizeof(FileRecordsDateTime));
                memset(fileRecrdTime,0,sizeof(fileRecrdTime));
                memset(FileRecordssize,0,sizeof(FileRecordssize));
                memset(fileNameList,0,sizeof(fileNameList));

                sprintf(FileRecordsDateTime, "%3s %02d %04d",
                                    month[((newNode->file_stat.fdate & 0x01E0) >> 5)],
                                    (newNode->file_stat.fdate & 0x001F),
                                    ((newNode->file_stat.fdate & 0xFE00) >> 9) + 1980);
                sprintf(fileRecrdTime,"%2d:%2d",
                                    (newNode->file_stat.ftime & 0xF800) >> 11,
                                    (newNode->file_stat.ftime & 0x07E0) >> 5);
 //                                   (newNode->file_stat.ftime & 0x001F) << 1 );

                sprintf(FileRecordssize, "%d", newNode->file_stat.fsize );
                if(newNode->file_stat.fattrib & SYS_FS_ATTR_DIR)
                {
                    FileRecordInformation = 1;
                }
                else
                {
                    FileRecordInformation = 0;
                }

                memset(fileNameList,0,sizeof(fileNameList));
                if(newNode->file_stat.lfname != NULL)
                {
                    sprintf((char*)fileNameList,"%-10s %3s %-8s %-8s %7s %s %s\r\n",filePermission[FileRecordInformation],link,owner,group,FileRecordssize,FileRecordsDateTime,newNode->file_stat.lfname);
                    // Now Free the allocated memory for the long file name
                    TCPIP_HEAP_Free(sTCPIPFtpMemH,newNode->file_stat.lfname);
                }
                else
                {
                    sprintf((char*)fileNameList,"%-10s %3s %-8s %-8s %7s %s %s\r\n",filePermission[FileRecordInformation],link,owner,group,FileRecordssize,FileRecordsDateTime,newNode->file_stat.fname);
                }

                lfNameLen = strlen((char *)fileNameList);
                if(wCount > lfNameLen)
                {
                    TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, fileNameList,
                                                      mMIN(wCount, lfNameLen));
                    remainingBytes =0;
                    pFTPDcpt->callbackPos = 0;
                    wCount = wCount - lfNameLen;
                    // remove the head from the list
                    newNode = (FTP_LIST_NODE*)TCPIP_Helper_ProtectedSingleListHeadRemove(&DirectoryFileList);
                    TCPIP_HEAP_Free(sTCPIPFtpMemH, newNode);
                    newNode = (FTP_LIST_NODE*)DirectoryFileList.list.head;
                    if(newNode == 0)
                    {
                        break;
                    }
                }
                else
                {
                    pFTPDcpt->callbackPos = 1;
                    TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, fileNameList, wCount);
                    remainingBytes = lfNameLen - wCount;
                    wCount=0;
                    continue;
                   // break;
                }
            }
            TCPIP_Helper_ProtectedSingleListDeinitialize(&DirectoryFileList);
            pFTPDcpt->ftpCommandSm  = TCPIP_FTP_CMD_SM_WAIT_FOR_DISCONNECT;
            break;
        case TCPIP_FTP_CMD_SM_WAIT_FOR_DISCONNECT:
            _FTP_ReleaseDataSocket(pFTPDcpt);                    
            pFTPDcpt->ftpResponse   = TCPIP_FTP_RESP_FILE_ACTION_SUCCESSFUL_CLOSING_DATA_CONNECTION;
            return true;
        default:
            break;
    }
    return false;
}

static bool TCPIP_FTP_ExecuteCmdGet(TCPIP_FTP_DCPT* pFTPDcpt, uint8_t *cFile)
{
    switch(pFTPDcpt->ftpCommandSm)
    {
        case TCPIP_FTP_CMD_SM_IDLE:
            if ( !pFTPDcpt->ftpFlag.Bits.loggedIn )
            {
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_LOGIN;
                return true;
            }
            else
            {
                if(pFTPDcpt->ftpFlag.Bits.pasvMode != true)
                {
                    if(TCPIP_FTP_CreateDataSocket(pFTPDcpt) != true)
                    {
                        return true;
                    }
                }
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_FILE_IS_PRESENT;
                pFTPDcpt->ftpCommandSm = TCPIP_FTP_CMD_SM_WAIT;
            }
            break;

        case TCPIP_FTP_CMD_SM_WAIT:
            if ( TCPIP_TCP_IsConnected(pFTPDcpt->ftpDataskt) )
            {
                pFTPDcpt->ftpCommandSm    = TCPIP_FTP_CMD_SM_SEND;
            }
        break;

        case TCPIP_FTP_CMD_SM_SEND:
		// Get/put as many bytes as possible
            if(TCPIP_FTP_FileGet(pFTPDcpt, cFile)== false)
            {
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_FILE_ACTION_SUCCESSFUL_CLOSING_DATA_CONNECTION;
                pFTPDcpt->callbackPos = 0;
                _FTP_ReleaseDataSocket(pFTPDcpt);            
                pFTPDcpt->ftpFlag.Bits.pasvMode = false;
                return true;
            }
            else if(pFTPDcpt->callbackPos == 0)
            {
                _FTP_ReleaseDataSocket(pFTPDcpt);       
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_DATA_CLOSE;
                pFTPDcpt->ftpFlag.Bits.pasvMode = false;
                return true;
            }
        default:
            break;
    }
    return false;
}

static TCPIP_FTP_CMD TCPIP_FTP_CmdsParse(uint8_t *cmd)
{
    TCPIP_FTP_CMD i;

    for ( i = 0; i < (TCPIP_FTP_CMD)TCPIP_FTP_CMD_TBL_SIZE; i++ )
    {
        if ( !memcmp((void*)cmd, (const void*)sTCPIPFTPCmdString[i], strlen((char*)cmd)) )
            return i;
    }

    return TCPIP_FTP_CMD_UNKNOWN;
}

static void TCPIP_FTP_CmdStringParse(TCPIP_FTP_DCPT* pFTPDcpt)
{
    uint8_t *p;
    uint8_t v;
    enum { SM_FTP_PARSE_PARAM, SM_FTP_PARSE_SPACE } smParseFTP;

    smParseFTP  = SM_FTP_PARSE_PARAM;
    p           = (uint8_t*)&pFTPDcpt->ftpCmdString[0];

    // Skip white blanks
    while( *p == ' ' )
    {
        p++;
    }

    pFTPDcpt->ftp_argv[0]  = (uint8_t*)p;
    pFTPDcpt->ftp_argc     = 1;

    while( (v = *p) )
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
                *p = '\0';
            break;

        case SM_FTP_PARSE_SPACE:
            if (( v != ' ' ) && (v != '|'))
            {
                pFTPDcpt->ftp_argv[pFTPDcpt->ftp_argc++] = (uint8_t*)p;
                smParseFTP = SM_FTP_PARSE_PARAM;
            }
            break;
        }
        p++;
        if(pFTPDcpt->ftp_argc == TCPIP_FTP_MAX_ARGS)
        	break;
    }
}

/*****************************************************************************
  Function:
	static bool TCPIP_FTP_FileGet(TCPIP_FTP_DCPT* pFTPDcpt, uint8_t *cFile)

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
static bool TCPIP_FTP_FileGet(TCPIP_FTP_DCPT* pFTPDcpt, uint8_t *cFile)
{
    int32_t wCount, wLen,status;
    uint8_t data[512];
    int32_t fp;

    fp = pFTPDcpt->fileDescr;

    // Check if this is a first round call
    if(pFTPDcpt->callbackPos == 0x00u)
    {// On initial call, open the file and save its ID
        fp = SYS_FS_FileOpen_Wrapper((const char*)cFile,SYS_FS_FILE_OPEN_READ);
        if(fp == SYS_FS_HANDLE_INVALID)
        {// File not found, so abort
            return false;
        }
        pFTPDcpt->fileDescr = fp;
        pFTPDcpt->callbackPos = 0;
    }
    else if(pFTPDcpt->callbackPos != 0x00u)
    {// The file was already opened, so load up its ID and seek
        if(fp == SYS_FS_HANDLE_INVALID)
        {// No file handles available, so wait for now
            return false;
        }
        SYS_FS_FileSeek(fp,(int32_t)pFTPDcpt->callbackPos,SYS_FS_SEEK_SET);
    }

    // Get/put as many bytes as possible
    wCount = TCPIP_TCP_PutIsReady(pFTPDcpt->ftpDataskt);
    while(wCount > 0u)
    {
        wLen = SYS_FS_FileRead(fp,data,mMIN(wCount, sizeof(data)));
        if(wLen == 0)
        {// If no bytes were read, an EOF was reached
            SYS_FS_FileClose(fp);
            pFTPDcpt->fileDescr = -1;
            pFTPDcpt->callbackPos = 0;
            return true;
        }
        else
        {// Write the bytes to the socket
            TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, data, wLen);
            wCount -= wLen;
        }
    }

    // Save the new address and close the file
    status = SYS_FS_FileTell(fp);
    if(status == -1)
        pFTPDcpt->callbackPos = 0;
    else
        pFTPDcpt->callbackPos = (uint32_t)status;

    return true;
}

static bool TCPIP_FTP_LSCmd(TCPIP_FTP_DCPT* pFTPDcpt)
{
    char longFileName[SYS_FS_MAX_PATH];
    uint16_t wCount=0;
    static uint8_t fileNameList[TCPIP_FTP_MAX_FILE_NAME_LEN+
                    TCPIP_FTP_MAX_FILE_DATE_TIME_STR_LEN+
                    TCPIP_FTP_MAX_FILE_SIZE_STR_LEN+
                    +10];
    int32_t fp;
    int16_t     bytestoBeSent=0;
    static uint16_t  remainingBytes=0;
    SYS_FS_FSTAT fs_stat;
	memset(&fs_stat, 0, sizeof(SYS_FS_FSTAT));
    char    *fileHeaderStr= "Date\t\t\tType\tFileSize\tfilename\t\r\n\0";
    uint8_t FileRecordInformation;
    char FileRecordsDateTime[TCPIP_FTP_MAX_FILE_DATE_TIME_STR_LEN];
    char FileRecordssize[TCPIP_FTP_MAX_FILE_SIZE_STR_LEN];
    static bool  cmdWithArg=false;
    uint8_t  nlstArgIndex=0;
    FTP_LIST_NODE* newNode=NULL;
    uint16_t    lfNameLen=0;
    char    *lfnamePtr=NULL;
    char fileType[2][5]={"file\0","dir\0"};

    switch(pFTPDcpt->ftpCommandSm)
    {
        case TCPIP_FTP_CMD_SM_IDLE:
            if(pFTPDcpt->ftpFlag.Bits.pasvMode != true)
            {
                if(TCPIP_FTP_CreateDataSocket(pFTPDcpt) != true)
                {
                    return true;
                }
            }
            if ( !pFTPDcpt->ftpFlag.Bits.loggedIn )
            {
                pFTPDcpt->ftpResponse    = TCPIP_FTP_RESP_LOGIN;
                 _FTP_ReleaseDataSocket(pFTPDcpt);  
                return true;
            }
            pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_DATA_READY;
            pFTPDcpt->ftpCommandSm = TCPIP_FTP_CMD_SM_WAIT;
            break;
        case TCPIP_FTP_CMD_SM_WAIT:
            if ( TCPIP_TCP_IsConnected(pFTPDcpt->ftpDataskt) )
            {
                pFTPDcpt->ftpCommandSm   = TCPIP_FTP_CMD_SM_SEND;
            }
            else
            {
                break;
            }
            if(strlen((char*)pFTPDcpt->ftp_argv[1])==0)
            {
                pFTPDcpt->ftpCommandSm   = TCPIP_FTP_CMD_SM_SEND_DIR;
                break;
            }
            // check if the NLST command has any argument
            for(nlstArgIndex=0;nlstArgIndex<TCPIP_FTP_MAX_NLST_ARGS;nlstArgIndex++)
            {
                if(strcmp((char*)pFTPDcpt->ftp_argv[1],(char*)TCPIP_FTP_CMD_NLSTArgs[nlstArgIndex])==0)
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
                if(strlen((char*)pFTPDcpt->ftp_argv[2]) > SYS_FS_MAX_PATH)
                {// File not found, so abort
                    pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_FILE_NOT_EXIST;
                    _FTP_ReleaseDataSocket(pFTPDcpt);  
                    cmdWithArg = false;
                    return true;
                }
            }
            else
            { // argument first will be the file name
                if(strlen((char*)pFTPDcpt->ftp_argv[1]) > SYS_FS_MAX_PATH)
                {// File not found, so abort
                    pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_FILE_NOT_EXIST;
                    _FTP_ReleaseDataSocket(pFTPDcpt);  
                    cmdWithArg = false;
                    return true;
                }
            }
            break;
        case TCPIP_FTP_CMD_SM_SEND:
            if(cmdWithArg)
            { // argument two will be the file name
                if(SYS_FS_FileStat_Wrapper((const char *)pFTPDcpt->ftp_argv[2], &fs_stat) != SYS_FS_HANDLE_INVALID)
                {
                    if(!(fs_stat.fattrib & SYS_FS_ATTR_DIR))
                    {  // transmit the remaining bytes
                        pFTPDcpt->ftpCommandSm   = TCPIP_FTP_CMD_SM_SEND_FILE;
                    }
                    else
                    {
                         pFTPDcpt->ftpCommandSm   = TCPIP_FTP_CMD_SM_SEND_DIR;
                    }
                }
                else
                {
                    pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_FILE_NOT_EXIST;
                    _FTP_ReleaseDataSocket(pFTPDcpt);  
                    cmdWithArg = false;
                    return true;
                }
            }
            else
            {
                if(SYS_FS_FileStat_Wrapper((const char *)pFTPDcpt->ftp_argv[1], &fs_stat) != SYS_FS_HANDLE_INVALID)
                {
                    if(!(fs_stat.fattrib & SYS_FS_ATTR_DIR))
                    {  // transmit the remaining bytes
                        pFTPDcpt->ftpCommandSm   = TCPIP_FTP_CMD_SM_SEND_FILE;
                    }
                    else
                    {
                         pFTPDcpt->ftpCommandSm   = TCPIP_FTP_CMD_SM_SEND_DIR;
                    }
                }
                else
                {
                    pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_FILE_NOT_EXIST;
                    _FTP_ReleaseDataSocket(pFTPDcpt);  
                    cmdWithArg = false;
                    return true;
                }
            }

            break;
        case TCPIP_FTP_CMD_SM_SEND_FILE:
            wCount = TCPIP_TCP_PutIsReady(pFTPDcpt->ftpDataskt);
            memset(fileNameList,0,sizeof(fileNameList));
            // If there is no argument
            if((gFTPNLSTArgIndex == 0xFF)&& (pFTPDcpt->callbackPos==0))
            {
                // Expected wcount should not be less than ftp argument 1
                sprintf((char*)fileNameList,"%s\r\n",(char *)pFTPDcpt->ftp_argv[1]);
                bytestoBeSent = strlen((char*)fileNameList);
                if(bytestoBeSent > wCount)
                {
                    while(bytestoBeSent>0)
                    {
                        TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, fileNameList, mMIN(wCount, strlen((char *)fileNameList)));
                        bytestoBeSent = bytestoBeSent - wCount;
                    }
                }
                else
                {
                     TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, fileNameList,
                                            strlen((char *)fileNameList));
                }
                pFTPDcpt->ftpCommandSm  = TCPIP_FTP_CMD_SM_WAIT_FOR_DISCONNECT;
                break;
            }
            // send the complete details of a File
            // for Windows ftp client , NLST command considers -a as a file argument and
            // it ignores the correct file argument.
            if(pFTPDcpt->callbackPos != 0)
            {
                if(wCount < remainingBytes)
                {
                    TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, fileNameList+(strlen((char *)fileNameList)-remainingBytes), wCount);
                    remainingBytes = remainingBytes - wCount;
                    wCount = 0;
                }
                else
                {
                    TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, fileNameList+(strlen((char *)fileNameList)-remainingBytes), remainingBytes);
                    remainingBytes =0;
                    pFTPDcpt->callbackPos = 0;
                }
                break;
            }
            else
            {
                memset(fileNameList,0,sizeof(fileNameList));
                strncpy((char*)fileNameList,fileHeaderStr,sizeof(fileNameList));
                if(wCount > strlen((char *)fileNameList))
                {
                    TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, fileNameList, strlen((char *)fileNameList));
                    remainingBytes =0;
                    pFTPDcpt->callbackPos = 0;
                }
                else
                {
                    pFTPDcpt->callbackPos = 1;
                    TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, fileNameList, mMIN(wCount, strlen((char *)fileNameList)));
                    remainingBytes = strlen((char *)fileNameList)- wCount;
                    break;
                }
            }
            memset(FileRecordsDateTime,0,sizeof(FileRecordsDateTime));
            memset(FileRecordssize,0,sizeof(FileRecordssize));
            sprintf(FileRecordsDateTime, "%02d-%02d-%04d %02d:%02d:%02d",
                                    (fs_stat.fdate & 0x01E0) >> 5,   // month
                                    (fs_stat.fdate & 0x001F),  // date
                                    ((fs_stat.fdate & 0xFE00) >> 9) + 1980, // year
                                    (fs_stat.ftime & 0xF800) >> 11,  // hour
                                    (fs_stat.ftime & 0x07E0) >> 5, // min
                                    (fs_stat.ftime & 0x001F) << 1 ); // sec

            sprintf(FileRecordssize, "%d", fs_stat.fsize );
            FileRecordInformation = 0;

            memset(fileNameList,0,sizeof(fileNameList));
            sprintf((char*)fileNameList,"%s\t file \t %s\t\t%s\r\n",FileRecordsDateTime,FileRecordssize,pFTPDcpt->ftp_argv[2]);

            if(wCount > strlen((char *)fileNameList))
            {
                TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, fileNameList,
                                    mMIN(wCount, strlen((char *)fileNameList)));
                remainingBytes =0;
                pFTPDcpt->callbackPos = 0;
                pFTPDcpt->ftpCommandSm  = TCPIP_FTP_CMD_SM_WAIT_FOR_DISCONNECT;
            }
            else
            {
                pFTPDcpt->callbackPos = 1;
                TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, fileNameList, mMIN(wCount, strlen((char *)fileNameList)));
                remainingBytes = strlen((char *)fileNameList)- wCount;
                break;
            }
            pFTPDcpt->ftpCommandSm  = TCPIP_FTP_CMD_SM_WAIT_FOR_DISCONNECT;
            break;

        case TCPIP_FTP_CMD_SM_SEND_DIR:
            memset(fileNameList,0,sizeof(fileNameList));
            if(gFTPNLSTArgIndex == 0xFF)
            {
                if(strlen((char*)pFTPDcpt->ftp_argv[1]) == 0)
                {// No specific directory path
                    fp = SYS_FS_DirOpen_Wrapper(LOCAL_WEBSITE_PATH);
                }
                else
                    fp = SYS_FS_DirOpen_Wrapper((const char*)pFTPDcpt->ftp_argv[1]);
            }
            else
            {
                fp = SYS_FS_DirOpen_Wrapper((const char*)pFTPDcpt->ftp_argv[2]);
            }
            if (fp == SYS_FS_HANDLE_INVALID)
            {// File not found, so abort
                pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_FILE_NOT_EXIST;
                _FTP_ReleaseDataSocket(pFTPDcpt);  
                cmdWithArg = false;
                return true;
            }
            pFTPDcpt->fileDescr = fp;
            pFTPDcpt->callbackPos = 0;
            pFTPDcpt->ftpCommandSm  = TCPIP_FTP_CMD_SM_SEND_DIR_HEADER;
         case TCPIP_FTP_CMD_SM_SEND_DIR_HEADER:
             wCount = TCPIP_TCP_PutIsReady(pFTPDcpt->ftpDataskt);
             // send the Header details of the table
            if(pFTPDcpt->callbackPos != 0)
            {
                if(wCount < remainingBytes)
                {
                    TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, fileNameList+(strlen((char *)fileNameList)-remainingBytes), wCount);
                    remainingBytes = remainingBytes - wCount;
                }
                else
                {
                    TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, fileNameList+(strlen((char *)fileNameList)-remainingBytes), remainingBytes);
                    remainingBytes =0;
                    pFTPDcpt->callbackPos = 0;
                }
                break;
            }
            else
            {
                memset(fileNameList,0,sizeof(fileNameList));
                strncpy((char*)fileNameList,fileHeaderStr,sizeof(fileNameList));
                if(wCount > strlen((char *)fileNameList))
                {
                    TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, fileNameList, strlen((char *)fileNameList));
                    remainingBytes =0;
                    pFTPDcpt->callbackPos = 0;
                }
                else
                {
                    pFTPDcpt->callbackPos = 1;
                    TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, fileNameList, mMIN(wCount, strlen((char *)fileNameList)));
                    remainingBytes = strlen((char *)fileNameList)- wCount;
                    break;
                }
            }
            // If long file name is used, the following elements of the "stat"
             // structure needs to be initialized with address of proper buffer.
            fs_stat.lfname = longFileName;
            fs_stat.lfsize = SYS_FS_MAX_PATH;
            fp = pFTPDcpt->fileDescr;
            if(TCPIP_Helper_ProtectedSingleListInitialize(&DirectoryFileList) != true)
            {
                return true;
            }
            while(1)
            {
                memset(fs_stat.fname,0,13);
                memset(longFileName,0,sizeof(longFileName));
                if(SYS_FS_DirSearch(fp, "*", SYS_FS_ATTR_ARC, &fs_stat) == SYS_FS_RES_FAILURE)
                {
                    SYS_FS_DirClose(fp);
                    pFTPDcpt->fileDescr = SYS_FS_HANDLE_INVALID;
                    break;
                }
                newNode = (FTP_LIST_NODE*)TCPIP_FTP_LIST_Add(&DirectoryFileList,sTCPIPFtpMemH , sizeof(*newNode));
                if(newNode == NULL)
                {
                    SYS_FS_DirClose(fp);
                    pFTPDcpt->fileDescr = SYS_FS_HANDLE_INVALID;
                    pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_FILE_NOT_EXIST;
                    _FTP_ReleaseDataSocket(pFTPDcpt);  
                    if(DirectoryFileList.list.nNodes > 0)
                    {	// Free if already there are node in the list
                        while(newNode != 0)
                        {
                            if(newNode->file_stat.lfname != NULL)
                            {
                                TCPIP_HEAP_Free(sTCPIPFtpMemH,newNode->file_stat.lfname);
                            }
                            newNode = (FTP_LIST_NODE*)TCPIP_Helper_ProtectedSingleListHeadRemove(&DirectoryFileList);
                            TCPIP_HEAP_Free(sTCPIPFtpMemH, newNode);
                        }
                    }
                    TCPIP_Helper_ProtectedSingleListDeinitialize(&DirectoryFileList);
                    return true;
                }
                newNode->file_stat.fattrib = fs_stat.fattrib;
                newNode->file_stat.fdate = fs_stat.fdate;
                newNode->file_stat.ftime = fs_stat.ftime;
                memset(newNode->file_stat.fname,0,sizeof(newNode->file_stat.fname));
                strcpy((char*)newNode->file_stat.fname ,(char*)fs_stat.fname);
                newNode->file_stat.fsize = fs_stat.fsize;
                newNode->file_stat.lfsize = fs_stat.lfsize;
                lfNameLen = strlen((char*)fs_stat.lfname);
                newNode->file_stat.lfname = NULL;
                if(lfNameLen>0)
                {
                    lfnamePtr = TCPIP_HEAP_Calloc(sTCPIPFtpMemH,1, lfNameLen+1);
                    if(lfnamePtr != NULL)
                    {
                        newNode->file_stat.lfname = lfnamePtr;
                        memcpy(newNode->file_stat.lfname,fs_stat.lfname,lfNameLen);
                        newNode->file_stat.lfname[lfNameLen+1]='\0';
                    }
                    else
                    {
                        SYS_FS_DirClose(fp);
                        pFTPDcpt->fileDescr = SYS_FS_HANDLE_INVALID;
                        pFTPDcpt->ftpResponse = TCPIP_FTP_RESP_FILE_NOT_EXIST;
                        _FTP_ReleaseDataSocket(pFTPDcpt);  
                        if(DirectoryFileList.list.nNodes > 0)
                        {	// Free if already there are node in the list
                            while(newNode != 0)
                            {
                                if(newNode->file_stat.lfname != NULL)
                                {
                                    TCPIP_HEAP_Free(sTCPIPFtpMemH,newNode->file_stat.lfname);
                                }
                                newNode = (FTP_LIST_NODE*)TCPIP_Helper_ProtectedSingleListHeadRemove(&DirectoryFileList);
                                TCPIP_HEAP_Free(sTCPIPFtpMemH, newNode);
                            }
                        }
                        TCPIP_Helper_ProtectedSingleListDeinitialize(&DirectoryFileList);
                        return true;
                    }
                }
            }
            pFTPDcpt->ftpCommandSm  = TCPIP_FTP_CMD_SM_SEND_DIR_DETAIL;
            // continue
        case TCPIP_FTP_CMD_SM_SEND_DIR_DETAIL:
            wCount = 0;
            newNode = (FTP_LIST_NODE*)DirectoryFileList.list.head;
            wCount = TCPIP_TCP_PutIsReady(pFTPDcpt->ftpDataskt);
            while(newNode != 0)
            {
                if(wCount == 0)
                {
                    return false;
                }
                // transmit the remaining bytes
                if(pFTPDcpt->callbackPos != 0)
                {
                    if(wCount < remainingBytes)
                    {
                        TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, fileNameList+(strlen((char *)fileNameList)-remainingBytes), wCount);
                        remainingBytes = remainingBytes - wCount;
                        wCount=0;
                        continue;
                    }
                    else
                    {
                        TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, fileNameList+(strlen((char *)fileNameList)-remainingBytes), remainingBytes);
                        wCount = wCount - remainingBytes;
                        remainingBytes =0;
                        pFTPDcpt->callbackPos = 0;
                        // remove the head from the list
                        newNode = (FTP_LIST_NODE*)TCPIP_Helper_ProtectedSingleListHeadRemove(&DirectoryFileList);
                        TCPIP_HEAP_Free(sTCPIPFtpMemH, newNode);
                        newNode = (FTP_LIST_NODE*)DirectoryFileList.list.head;
                        if(newNode == 0)
                        {
                            break;
                        }
                    }
                }
                memset(FileRecordsDateTime,0,sizeof(FileRecordsDateTime));
                memset(FileRecordssize,0,sizeof(FileRecordssize));
                memset(fileNameList,0,sizeof(fileNameList));

                sprintf(FileRecordsDateTime, "%02d-%02d-%04d %02d:%02d:%02d",
                                    (newNode->file_stat.fdate & 0x01E0) >> 5,
                                    (newNode->file_stat.fdate & 0x001F),
                                    ((newNode->file_stat.fdate & 0xFE00) >> 9) + 1980,
                                    (newNode->file_stat.ftime & 0xF800) >> 11,
                                    (newNode->file_stat.ftime & 0x07E0) >> 5,
                                    (newNode->file_stat.ftime & 0x001F) << 1 );

                sprintf(FileRecordssize, "%d", newNode->file_stat.fsize );
                if(newNode->file_stat.fattrib & SYS_FS_ATTR_DIR)
                {
                    FileRecordInformation = 1;
                }
                else
                {
                    FileRecordInformation = 0;
                }

                memset(fileNameList,0,sizeof(fileNameList));
                if(newNode->file_stat.lfname != NULL)
                {
                    sprintf((char*)fileNameList,"%s\t %s \t %s\t\t%s\r\n",FileRecordsDateTime,fileType[FileRecordInformation],FileRecordssize,newNode->file_stat.lfname);
                    // Now Free the allocated memory for the long file name
                    TCPIP_HEAP_Free(sTCPIPFtpMemH,newNode->file_stat.lfname);
                }
                else
                {
                    sprintf((char*)fileNameList,"%s\t %s \t %s\t\t%s\r\n",FileRecordsDateTime,fileType[FileRecordInformation],FileRecordssize,newNode->file_stat.fname);
                }

                lfNameLen = strlen((char *)fileNameList);
                if(wCount > lfNameLen)
                {
                    TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, fileNameList,
                                        mMIN(wCount, lfNameLen));
                    remainingBytes =0;
                    pFTPDcpt->callbackPos = 0;
                    wCount = wCount - lfNameLen;
                    //pFTPDcpt->ftpCommandSm  = TCPIP_FTP_CMD_SM_WAIT_FOR_DISCONNECT;
                    // remove the head from the list
                    newNode = (FTP_LIST_NODE*)TCPIP_Helper_ProtectedSingleListHeadRemove(&DirectoryFileList);
                    TCPIP_HEAP_Free(sTCPIPFtpMemH, newNode);
                    newNode = (FTP_LIST_NODE*)DirectoryFileList.list.head;
                    if(newNode == 0)
                    {
                        break;
                    }
                }
                else
                {
                    pFTPDcpt->callbackPos = 1;
                    TCPIP_TCP_ArrayPut(pFTPDcpt->ftpDataskt, fileNameList, wCount);
                    remainingBytes = lfNameLen - wCount;
                    wCount=0;
                    continue;
                   // break;
                }
            }
            TCPIP_Helper_ProtectedSingleListDeinitialize(&DirectoryFileList);
            pFTPDcpt->ftpCommandSm  = TCPIP_FTP_CMD_SM_WAIT_FOR_DISCONNECT;
            //SYS_FS_FileClose(fp);
            break;
        case TCPIP_FTP_CMD_SM_WAIT_FOR_DISCONNECT:
            _FTP_ReleaseDataSocket(pFTPDcpt);  
            pFTPDcpt->ftpResponse   = TCPIP_FTP_RESP_FILE_ACTION_SUCCESSFUL_CLOSING_DATA_CONNECTION;
            gFTPNLSTArgIndex = 0xFF;
            remainingBytes = 0;
            cmdWithArg = false;
            return true;
        default:
            break;
    }
    return false;
}
#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_FTP_SERVER)

