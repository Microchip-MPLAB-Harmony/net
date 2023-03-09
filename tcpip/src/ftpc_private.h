/*******************************************************************************
  FTPC Module private definitions for the Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    ftpc_private.h

  Summary:
    FTPC private file

  Description:
    This source file contains the stack internal FTPC definitions
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2019-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef _FTPC_PRIVATE_H_
#define _FTPC_PRIVATE_H_

// defines

// minimum server timeout, seconds
#define TCPIP_FTPC_MIN_TMO              10 
#define TCPIP_FTPC_DFLT_SRV_CTRL_PORT   21 
#define FTPC_BUFF_SIZE_ALIGN_MASK       0xfff0

// *****************************************************************************
/*
  Enumeration:
    TCPIP_FTPC_STATE_TYPE

  Summary:
    FTP Client State index

  Description:
    Index of FTPC state machine

  Remarks:
    None
*/
typedef enum
{
    TCPIP_FTPC_STATE_HOME,          //Initial STATE
    
    //Connect States        
    TCPIP_FTPC_STATE_START_CONNECT, // Starting connection to server control socket
    TCPIP_FTPC_STATE_WAIT_CONNECT,  // Waiting for connection to server control socket
    TCPIP_FTPC_STATE_DONE_CONNECT,  // Successfully connected to server control socket
    
    //Login States        
    TCPIP_FTPC_STATE_LOGIN_SEND_USER,  
    TCPIP_FTPC_STATE_LOGIN_WAIT_USER,
    TCPIP_FTPC_STATE_LOGIN_SEND_PASS,
    TCPIP_FTPC_STATE_LOGIN_WAIT_PASS,
    TCPIP_FTPC_STATE_LOGIN_SEND_ACCT,
    TCPIP_FTPC_STATE_LOGIN_WAIT_ACCT,
    TCPIP_FTPC_STATE_DONE_LOGIN,
    
    //CWD States        
    TCPIP_FTPC_STATE_SEND_CWD,  
    TCPIP_FTPC_STATE_WAIT_CWD,
    
    //CDUP States        
    TCPIP_FTPC_STATE_SEND_CDUP,  
    TCPIP_FTPC_STATE_WAIT_CDUP,
    
    //MKD States        
    TCPIP_FTPC_STATE_SEND_MKD,  
    TCPIP_FTPC_STATE_WAIT_MKD, 
    
    //RMD States        
    TCPIP_FTPC_STATE_SEND_RMD,  
    TCPIP_FTPC_STATE_WAIT_RMD, 
            
    //DELE States          
    TCPIP_FTPC_STATE_SEND_DELE,
    TCPIP_FTPC_STATE_WAIT_DELE, 
            
    //QUIT States          
    TCPIP_FTPC_STATE_SEND_QUIT,
    TCPIP_FTPC_STATE_WAIT_QUIT,
            
    //PWD States        
    TCPIP_FTPC_STATE_SEND_PWD,  
    TCPIP_FTPC_STATE_WAIT_PWD,
    
    //TYPE States        
    TCPIP_FTPC_STATE_SEND_TYPE,  
    TCPIP_FTPC_STATE_WAIT_TYPE,  
            
    //STRU States        
    TCPIP_FTPC_STATE_SEND_STRU,  
    TCPIP_FTPC_STATE_WAIT_STRU, 
            
    //MODE States        
    TCPIP_FTPC_STATE_SEND_MODE,  
    TCPIP_FTPC_STATE_WAIT_MODE, 
            
    //Active mode States        
    TCPIP_FTPC_STATE_SEND_PORT,  
    TCPIP_FTPC_STATE_WAIT_PORT,
    
    //Passive mode States        
    TCPIP_FTPC_STATE_SEND_PASV,  
    TCPIP_FTPC_STATE_WAIT_PASV,
    TCPIP_FTPC_STATE_PASV_WAIT_DATA_CONNECT,
    
    //RETR States        
    TCPIP_FTPC_STATE_SEND_RETR,  
    TCPIP_FTPC_STATE_WAIT_RETR_CTRL_RESPONSE,
    TCPIP_FTPC_STATE_WAIT_RETR_READ_DATA_SOCKET,
    TCPIP_FTPC_STATE_RETR_EOF,    
    
    //STOR States        
    TCPIP_FTPC_STATE_SEND_STOR,  
    TCPIP_FTPC_STATE_WAIT_STOR_CTRL_RESPONSE,
    TCPIP_FTPC_STATE_STOR_WRITE_DATA_SOCKET,
    TCPIP_FTPC_STATE_STOR_EOF,   
    
    //LST States
    TCPIP_FTPC_STATE_START_LST,
    TCPIP_FTPC_STATE_SEND_LST,  
    TCPIP_FTPC_STATE_WAIT_LST_CTRL_RESPONSE,
    TCPIP_FTPC_STATE_WAIT_LST_READ_DATA_SOCKET,
    TCPIP_FTPC_STATE_LST_EOT,   
    
}TCPIP_FTPC_STATE_TYPE;

// *****************************************************************************
/*
  Enumeration:
    TCPIP_FTPC_RESULT_TYPE

  Summary:
    FTP Client State machine return type

  Description:
    Internal return values for FTPC state machine

  Remarks:
    None
*/
typedef enum
{
    // OK, operation successful
    TCPIP_FTPC_RES_OK          = 0,    
    // operation pending; wait for results/timeout
    TCPIP_FTPC_RES_PENDING     = 1, 
    //Server Control Responses
    TCPIP_FTPC_SERVER_POS_PRELIM_RESPONSE,  //100-199
    TCPIP_FTPC_SERVER_POS_COMPL_RESPONSE,   //200-299
    TCPIP_FTPC_SERVER_POS_INTRMDT_RESPONSE, //300-399
    TCPIP_FTPC_SERVER_NEG_TRANS_RESPONSE,   //400-499     
    TCPIP_FTPC_SERVER_NEG_COMPL_RESPONSE,   //500-599
    TCPIP_FTPC_SERVER_ILLEGAL_RESPONSE,        
    // errors, codes < 0
    TCPIP_FTPC_RES_ERROR                        = -1, // generic error       
}TCPIP_FTPC_RESULT_TYPE;

// *****************************************************************************
/*
  Enumeration:
    TCPIP_FTPC_SIGNAL_TYPE

  Summary:
    FTP Client socket signals

  Description:
    FTPC signals for Control and Data Sockets

  Remarks:
    None
*/
typedef enum
{
    TCPIP_FTPC_SIGNAL_NONE                =   0x0000,
    TCPIP_FTPC_SIGNAL_DATA_RX_SUCCESS     =   0x0001,
    TCPIP_FTPC_SIGNAL_DATA_RX_FIN         =   0x0002,
    TCPIP_FTPC_SIGNAL_CTRL_RX_FIN         =   0x0004,        
}TCPIP_FTPC_SIGNAL_TYPE;

// *****************************************************************************
/*
  Enumeration:
    TCPIP_FTPC_FLAG_TYPE

  Summary:
    FTP Client flags

  Description:
    Flags for indicating different milestones in FTPC process

  Remarks:
    None
*/
typedef enum
{
    TCPIP_FTPC_FLAG_NONE        = 0x0000,
    TCPIP_FTPC_FLAG_CONNECTED   = 0x0001,
    TCPIP_FTPC_FLAG_LOGGEDIN    = 0x0002,
}TCPIP_FTPC_FLAG_TYPE;

// *****************************************************************************
/*
  Structure:
    TCPIP_FTPC_SOCKET_TYPE

  Summary:
   FTP Client Control and Data socket

  Description:
    This data structure describes the FTPC control and data sockets.

  Remarks:
    None

*/
typedef struct
{   
    NET_PRES_SKT_HANDLE_T      ftpcCtrlSkt;    // associated FTP Client control connection socket
    NET_PRES_SKT_HANDLE_T      ftpcDataSkt;    // associated FTP Client Data connection socket
}TCPIP_FTPC_SOCKET_TYPE;

// *****************************************************************************
/*
  Structure:
    TCPIP_FTPC_ACCESS_CTRL_TYPE

  Summary:
   TCPIP FTP Client access info for server login

  Description:
    This data structure describes access info for server login

  Remarks:
    None

*/
typedef struct
{   
    const char*      ftpcUserName;      // username for server access
    const char*      ftpcPassword;      // password for server access
    const char*      ftpcAccount;       // user name for server access
}TCPIP_FTPC_ACCESS_CTRL_TYPE;

// *****************************************************************************
/*
  Structure:
    TCPIP_FTPC_MODULE_DCPT_TYPE

  Summary:
   TCPIP FTP Client module descriptor for global configurations

  Description:
    This data structure describes FTP Client global configurations

  Remarks:
    None

*/
typedef struct
{
    int         nMaxClients;    // maximum number of simultaneous Client supported
                                // This is for multi-client support         
    const void* memH;           // memory handle                                 
    uint32_t    ftpcTmo;        // timeout for reply on FTP Control connection and Data Connection,
                                // in seconds; Should be adjusted according to the server responsivity
    uint16_t    data_tx_buffsize_dflt; //Data Socket Transmit Buffer default size
    uint16_t    data_rx_buffsize_dflt; //Data Socket Receive Buffer default size
}TCPIP_FTPC_MODULE_DCPT_TYPE;
// *****************************************************************************
/*
  Structure:
    TCPIP_FTPC_DCPT_TYPE

  Summary:
   TCPIP FTP Client descriptor for storing context of FTPC connection

  Description:
    This data structure saves the context of a FTPC connection

  Remarks:
    None

*/
typedef struct _TCPIP_FTPC_DCPT_TYPE
{
    struct _TCPIP_FTPC_DCPT_TYPE*   next;
    uint16_t                        ftpcDcptIndex;      // descriptor identifier
    TCPIP_FTPC_STATE_TYPE           ftpcState;          // current state of FTP Client state-machine
    TCPIP_FTPC_SOCKET_TYPE          ftpcSocket;         // control and data socket
    TCPIP_FTPC_CTRL_CONN_TYPE       ftpcCtrlConnection; // ftp client control connection details
    TCPIP_FTPC_DATA_CONN_TYPE       ftpcDataConnection; // ftp client data connection details
    TCPIP_FTPC_ACCESS_CTRL_TYPE     ftpcAccessControl;  // ftp client access control parameters    
    TCPIP_FTPC_FILE_OPT_TYPE        ftpcFileOptions;    // file handling options
    
    TCPIP_FTPC_DATA_EVENT_CALLBACK_TYPE dataSktCallback;    // call back for the data channel 
    TCPIP_FTPC_CTRL_EVENT_CALLBACK_TYPE ctrlSktCallback;    // call back for the command channel 
    
    char*       ftpcCtrlTxBuff;     //  Control Socket  Tx buffer pointer
    char*       ftpcCtrlRxBuff;     //  Control Socket  Rx buffer pointer
    char*       ftpcDataTxBuff;     //  Data Socket  Tx buffer pointer
    char*       ftpcDataRxBuff;     //  Data Socket  Rx buffer pointer

    uint16_t    ftpcCtrlRxLen;      //  Control Socket receive length
    uint16_t    ftpcCtrlTxLen;      //  Control Socket transmit length
    uint32_t    ftpcDataRxLen;      //  Data Socket receive length
    uint32_t    ftpcDataTxLen;      //  Data Socket transmit length
    uint16_t    ftpcDataLength;
    
    const char      *ftpcServerPathname;     //  pathname pointer
    const char      *ftpcClientPathname;     //  pathname pointer
    
    const char  *filePathname;  // file name to save files in FS
        
    int32_t     fileDescr;   // file descriptor 
    int32_t     filePos;     // file descriptor 
    
    TCPIP_FTPC_ERROR_TYPE   error;          // ftp internal errors
    TCPIP_FTPC_CMD          ftpcCommand;    // ftp command type
    TCPIP_FTPC_SIGNAL_TYPE  ftpcSignal;     // last response received
    TCPIP_FTPC_FLAG_TYPE    ftpcFlag;       // FTP client status flag
    
    uint32_t    waitTick;     // if a reply is not received, trigger a timeout
    bool        ftpcActive;   // busy/ready flag
}TCPIP_FTPC_DCPT_TYPE;

// *****************************************************************************
//TCPIP FTP Client state machine function type
typedef TCPIP_FTPC_RESULT_TYPE(*TCPIP_FTPC_STATE_FUNC)(TCPIP_FTPC_DCPT_TYPE* pDcpt);
/****************************************************************************
  Section:
    FTPC definitions
  ***************************************************************************/
//MACROS to check FTP response codes
#define FTPC_IS_REPLY_1YZ(code) ((code) >= 100 && (code) < 200)
#define FTPC_IS_REPLY_2YZ(code) ((code) >= 200 && (code) < 300)
#define FTPC_IS_REPLY_3YZ(code) ((code) >= 300 && (code) < 400)
#define FTPC_IS_REPLY_4YZ(code) ((code) >= 400 && (code) < 500)
#define FTPC_IS_REPLY_5YZ(code) ((code) >= 500 && (code) < 600)
// *****************************************************************************

// ftpc internal functions
static void TCPIP_FTPC_Process(void);
static __inline__ OSAL_CRITSECT_DATA_TYPE __attribute__((always_inline)) ftpcEnterCritical(void);
static __inline__ void __attribute__((always_inline)) ftpcExitCritical(OSAL_CRITSECT_DATA_TYPE critStatus);

//////////////////// FTPC state-machine functions ///////////////////////////////
// state machine initial state
static TCPIP_FTPC_RESULT_TYPE _ftpcStateHome(TCPIP_FTPC_DCPT_TYPE* pDcpt);

static TCPIP_FTPC_RESULT_TYPE _ftpcDoneCmd(TCPIP_FTPC_DCPT_TYPE* pDcpt);

//FTPC Connect Routines
static TCPIP_FTPC_RESULT_TYPE _ftpcStateStartConnect(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitConnect(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateDoneConnect(TCPIP_FTPC_DCPT_TYPE* pDcpt);

//FTPC Login Routines
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendUser(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitUser(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendPass(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitPass(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendAcct(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitAcct(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateDoneLogin(TCPIP_FTPC_DCPT_TYPE* pDcpt);

//FTPC CWD Routines
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendCwd(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitCwd(TCPIP_FTPC_DCPT_TYPE* pDcpt);

//FTPC CDUP Routines
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendCdup(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitCdup(TCPIP_FTPC_DCPT_TYPE* pDcpt);

//FTPC MKD Routines
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendMkd(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitMkd(TCPIP_FTPC_DCPT_TYPE* pDcpt);

//FTPC RMD Routines
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendRmd(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitRmd(TCPIP_FTPC_DCPT_TYPE* pDcpt);

//FTPC DELE Routines
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendDele(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitDele(TCPIP_FTPC_DCPT_TYPE* pDcpt);

//FTPC QUIT Routines
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendQuit(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitQuit(TCPIP_FTPC_DCPT_TYPE* pDcpt);

//FTPC PWD Routines
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendPwd(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitPwd(TCPIP_FTPC_DCPT_TYPE* pDcpt);

//FTPC TYPE Routines
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendType(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitType(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateDoneType(TCPIP_FTPC_DCPT_TYPE* pDcpt);

//FTPC STRU Routines
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendStru(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitStru(TCPIP_FTPC_DCPT_TYPE* pDcpt);

//FTPC MODE Routines
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendMode(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitMode(TCPIP_FTPC_DCPT_TYPE* pDcpt);

//FTPC GET routines
static TCPIP_FTPC_RESULT_TYPE _ftpcStateStartGet(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcDoneFileTransfer(TCPIP_FTPC_DCPT_TYPE* pDcpt);

//FTPC PUT routines
static TCPIP_FTPC_RESULT_TYPE _ftpcStateStartPut(TCPIP_FTPC_DCPT_TYPE* pDcpt);

//FTPC Open Data Socket
static TCPIP_FTPC_RESULT_TYPE _ftpcStateOpenDataSkt(TCPIP_FTPC_DCPT_TYPE* pDcpt);

//FTPC PORT Routines
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendPort(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitPort(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateDonePort(TCPIP_FTPC_DCPT_TYPE* pDcpt);

//FTPC PASV Routines
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendPasv(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitPasv(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateDonePasv(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitPasvDataConnect(TCPIP_FTPC_DCPT_TYPE* pDcpt);

//FTPC RETR Routines
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendRetr(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitRetrCtrlResp(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitRetrReadDataSkt(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateRetrEoF(TCPIP_FTPC_DCPT_TYPE* pDcpt);

//FTPC STOR Routines
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendStor(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitStorCtrlResp(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateStorWriteDataSkt(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateStorEoF(TCPIP_FTPC_DCPT_TYPE* pDcpt);

//FTPC LST Routines
static TCPIP_FTPC_RESULT_TYPE _ftpcStateStartLst(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateSendLst(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitLstCtrlResp(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateWaitLstReadDataSkt(TCPIP_FTPC_DCPT_TYPE* pDcpt);
static TCPIP_FTPC_RESULT_TYPE _ftpcStateLstEoT(TCPIP_FTPC_DCPT_TYPE* pDcpt);

/////////////// FTPC internal functions /////////////////////////////////////
// Process Control Socket response
static TCPIP_FTPC_RESULT_TYPE ftpcParseReplyCode(TCPIP_FTPC_DCPT_TYPE* pDcpt);
// Process PASV response
static void ftpc_process_pasv_response(TCPIP_FTPC_DCPT_TYPE* pDcpt);
//  Set Time-Out
static void ftpcSetDcptTmo(TCPIP_FTPC_DCPT_TYPE* pDcpt, uint32_t tmo);
// Check the timer expiry
static bool ftpcIsDcptTmoExpired(TCPIP_FTPC_DCPT_TYPE* pDcpt);
//Error Handling routine
static void _ftpcErrorUpdate(TCPIP_FTPC_DCPT_TYPE* pDcpt);
// read control socket response
static TCPIP_FTPC_RESULT_TYPE ftpcReadCtrlSocket(TCPIP_FTPC_DCPT_TYPE* pDcpt);
// write command to control socket
static TCPIP_FTPC_RESULT_TYPE ftpcWriteCtrlSocket(TCPIP_FTPC_DCPT_TYPE* pDcpt, uint16_t writeByte_count);
// read data from Data socket and write to File system media
static TCPIP_FTPC_RESULT_TYPE ftpcReadDataSocket(TCPIP_FTPC_DCPT_TYPE* pDcpt, uint16_t * rxLen);
// Call control socket callback function
static void ftpcCtrlEvent(TCPIP_FTPC_DCPT_TYPE* pDcpt, TCPIP_FTPC_CTRL_EVENT_TYPE ftpcEvent,
                          char * ctrlbuff, uint16_t ctrllen);
// Call data socket callback function
static bool ftpcDataEvent(TCPIP_FTPC_DCPT_TYPE* pDcpt, TCPIP_FTPC_DATA_EVENT_TYPE ftpcEvent,
                          char * databuff, uint16_t * datalen);
// close control socket
static void _ftpcCtrlDisconnect(TCPIP_FTPC_DCPT_TYPE* pDcpt);
// close data socket
static void _ftpcCloseDataSkt(TCPIP_FTPC_DCPT_TYPE* pDcpt);
// set the default value to FTPC internal Descriptor
static void _ftpcDcptSetDefault(TCPIP_FTPC_DCPT_TYPE* pDcpt, uint16_t index);
// Set Rx/Tx buffer size for Data Socket
static TCPIP_FTPC_RESULT_TYPE _ftpcSetDataSktBuff(TCPIP_FTPC_DCPT_TYPE* pDcpt);
#endif  // _FTPC_PRIVATE_H_

