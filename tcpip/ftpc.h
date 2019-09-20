/*******************************************************************************
  File Transfer Protocol Client - FTPC

  Company:
    Microchip Technology Inc.
    
  File Name:
    ftpc.h

  Summary:
    Module for Microchip TCP/IP Stack.
    
  Description:
    The FTPC client module in the TCP/IP Stack lets applications send e-mails 
    to any recipient worldwide. These message could include status information 
    or important alerts. Using the e-mail to SMS gateways provided by most cell 
    phone carriers, these messages can also be delivered directly to cell phone 
    handsets.
*******************************************************************************/
//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.

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
//DOM-IGNORE-END

#ifndef __FTPC_H
#define __FTPC_H
#include "net_pres/pres/net_pres.h"
// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
/*
  Enumeration:
    TCPIP_FTPC_CTRL_EVENT_TYPE

  Summary:
    FTPC Control Socket Events

  Description:
    Enumeration describing the possible event codes
    for FTP client control socket.

  Remarks:
    None
*/
typedef enum
{ 
    //when command thru Control Socket is successful
    TCPIP_FTPC_CTRL_EVENT_SUCCESS = 1,
    //when command thru Control Socket failed        
    TCPIP_FTPC_CTRL_EVENT_FAILURE, 
    //when control socket is disconnected        
    TCPIP_FTPC_CTRL_EVENT_DISCONNECTED, 
    //when control socket received server response        
    TCPIP_FTPC_CTRL_RCV,
    //when command sent thru control socket    
    TCPIP_FTPC_CTRL_SEND,
}TCPIP_FTPC_CTRL_EVENT_TYPE;

// *****************************************************************************
/*
  Enumeration:
    TCPIP_FTPC_DATA_EVENT_TYPE

  Summary:
    FTPC Data Socket Events

  Description:
    Enumeration describing the possible event codes
    for FTP client data socket.

  Remarks:
    None
*/
typedef enum
{
    //when data received on Data Socket
    TCPIP_FTPC_DATA_RCV,
    //when data receive complete
    TCPIP_FTPC_DATA_RCV_DONE,
    //when Data Socket is ready to transmit data
    TCPIP_FTPC_DATA_SEND_READY,
    //when data transmission complete        
    TCPIP_FTPC_DATA_SEND_DONE,

}TCPIP_FTPC_DATA_EVENT_TYPE;

// *****************************************************************************
/*
  Enumeration:
    TCPIP_FTPC_TRANSFER_MODE_TYPE

  Summary:
    FTPC Transfer Modes

  Description:
    Enumeration describing the different Transfer modes for FTP protocol

  Remarks:
    None
*/
typedef enum
{
    TCPIP_FTPC_TRANS_STREAM_MODE = 1,
    TCPIP_FTPC_TRANS_BLOCK_MODE,
    TCPIP_FTPC_TRANS_COMPRESS_MODE,
    TCPIP_FTPC_TRANS_UNSUPPORTED,
}TCPIP_FTPC_TRANSFER_MODE_TYPE;

// *****************************************************************************
/*
  Enumeration:
    TCPIP_FTPC_DATA_REP_TYPE

  Summary:
    FTPC data representation types

  Description:
    Enumeration describing the different Data Representations in FTP protocol

  Remarks:
    None
*/
typedef enum
{
    TCPIP_FTPC_DATA_REP_ASCII,
    TCPIP_FTPC_DATA_REP_EBCDIC,
    TCPIP_FTPC_DATA_REP_IMAGE,
    TCPIP_FTPC_DATA_REP_UNSUPPORTED,
}TCPIP_FTPC_DATA_REP_TYPE;

// *****************************************************************************
/*
  Enumeration:
    TCPIP_FTPC_DATA_STRUCT_TYPE

  Summary:
    FTPC data structure types

  Description:
    Enumeration describing the different Data structure in FTP protocol

  Remarks:
    None
*/
typedef enum
{
    TCPIP_FTPC_STRUCT_FILE = 1,
    TCPIP_FTPC_STRUCT_RECORD,
    TCPIP_FTPC_STRUCT_PAGE,
    TCPIP_FTPC_STRUCT_UNSUPPORTED,
}TCPIP_FTPC_DATA_STRUCT_TYPE;


// *****************************************************************************
/*
  Enumeration:
    TCPIP_FTPC_RETURN_TYPE

  Summary:
    FTPC API return types

  Description:
    Enumeration describing the different return types for external FTPC APIs

  Remarks:
    None
*/
typedef enum
{    
    TCPIP_FTPC_RET_OK = 0,
    TCPIP_FTPC_RET_BUSY = 1,  
    TCPIP_FTPC_RET_FAILURE = -1,
    TCPIP_FTPC_RET_NOT_CONNECT = -2,        
    TCPIP_FTPC_RET_NOT_LOGIN = -3,     
}TCPIP_FTPC_RETURN_TYPE;


// *****************************************************************************
/*
  Enumeration:
    TCPIP_FTPC_ERROR_TYPE

  Summary:
    FTPC Error Types

  Description:
    Enumeration describing the different errors reported by FTP Client

  Remarks:
    None
*/
typedef enum
{
    TCPIP_FTPC_ERROR_NONE,
    TCPIP_FTPC_ERROR_CTRL_RESPONSE,
    TCPIP_FTPC_ERROR_TIMEOUT,
            
    TCPIP_FTPC_ERROR_FILE_OPEN,
    TCPIP_FTPC_ERROR_FILE_ACCESS,
    TCPIP_FTPC_ERROR_FILE_WRITE,
            
    TCPIP_FTPC_ERROR_OPEN_CTRL_SOCKET,
    TCPIP_FTPC_ERROR_BIND_CTRL_SOCKET,
    TCPIP_FTPC_ERROR_CONNECT_CTRL_SOCKET,
    
    TCPIP_FTPC_ERROR_OPEN_DATA_SOCKET,
    TCPIP_FTPC_ERROR_BIND_DATA_SOCKET,
    TCPIP_FTPC_ERROR_CONNECT_DATA_SOCKET,
 
}TCPIP_FTPC_ERROR_TYPE;

// *****************************************************************************
/*
  Enumeration:
    TCPIP_FTPC_CMD

  Summary:
    FTP Client Commands

  Description:
    Enumeration list commands supported by FTP Client

  Remarks:
    None
*/
typedef enum
{
    TCPIP_FTPC_CMD_NONE = 0,
    TCPIP_FTPC_CMD_CONNECT,        
    TCPIP_FTPC_CMD_USER,
    TCPIP_FTPC_CMD_PASS,
    TCPIP_FTPC_CMD_ACCT,
    TCPIP_FTPC_CMD_TYPE,
    TCPIP_FTPC_CMD_STRU,
    TCPIP_FTPC_CMD_MODE, 
    TCPIP_FTPC_CMD_PASV,
    TCPIP_FTPC_CMD_PORT,
    TCPIP_FTPC_CMD_GET,
    TCPIP_FTPC_CMD_PUT,
	TCPIP_FTPC_CMD_NLST,
    TCPIP_FTPC_CMD_LIST,
    TCPIP_FTPC_CMD_DELE,
    TCPIP_FTPC_CMD_CWD,
    TCPIP_FTPC_CMD_CDUP,
    TCPIP_FTPC_CMD_MKD,
    TCPIP_FTPC_CMD_RMD,
    TCPIP_FTPC_CMD_PWD,
}TCPIP_FTPC_CMD;

// *****************************************************************************
/*
  Type:
    TCPIP_FTPC_CONN_HANDLE_TYPE

  Summary:
    Defines a handle for FTP Client Connection

  Description:
    Data type describing a handle that identifies a FTP connection
    This handle is to be used by all FTP Client APIs

    A valid handle is != 0.

  Remarks:
    None
*/
typedef const void* TCPIP_FTPC_CONN_HANDLE_TYPE;


// *****************************************************************************
/*
  Type:
    TCPIP_FTPC_CTRL_EVENT_CALLBACK_TYPE

  Summary:
    Defines a FTP Client callback for control socket events

  Description:
    Data type describing a function that will be called by the FTPC module
    upon control socket events
    FTPC will parse and process the FTP client-server control messages anyway but it can optionally pass
    the information to the user.

  Parameters:
    ftpcHandle  - handle identifying the connection.
    ftpcEvent   - the control socket event
    cmd         - FTP Command
    ctrlbuff    - pointer to control socket data buffer
    ctrllen     - receive data length
  Remarks:
    None

*/
typedef void (*TCPIP_FTPC_CTRL_EVENT_CALLBACK_TYPE)(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
                TCPIP_FTPC_CTRL_EVENT_TYPE ftpcEvent, TCPIP_FTPC_CMD cmd, char * ctrlbuff, uint16_t ctrllen);

// *****************************************************************************
/*
  Type:
    TCPIP_FTPC_DATA_EVENT_CALLBACK_TYPE

  Summary:
    Defines a FTP Client callback for data socket events

  Description:
    Data type describing a function that will be called by the FTPC module
    upon data socket events
    FTPC will parse and process the FTP client-server data messages anyway but it can optionally pass
    the information to the user.

  Parameters:
    ftpcHandle  - handle identifying the connection.
    ftpcEvent   - the data socket event
    cmd         - FTP Command
    databuff    - pointer to Data socket data buffer
    datalen     - pointer to receive data length

  Returns:
    Return 'true' when callback function does the processing of Rx and Tx Data; no need 
    to save data on file system.
    Return 'false' when callback function does not process Rx and Tx Data; should
    save data on file system.
 
  Remarks:
    When FTP Client receives data (for GET, NLST, LIST commands), "databuff" is 
    the address of buffer where received data is stored. The"datalen" is the number 
    of bytes received.
 
    When FTP Client transmits data (for PUT commands), "databuff"  is the address 
    where data to be copied for transmission. The "datalen" is the number of bytes 
    to be transmitted by FTP Client.

*/
typedef bool (*TCPIP_FTPC_DATA_EVENT_CALLBACK_TYPE)(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
                TCPIP_FTPC_DATA_EVENT_TYPE ftpcEvent, TCPIP_FTPC_CMD cmd, char * databuff, uint16_t * datalen);


// *****************************************************************************
/*
  Structure:
    TCPIP_FTPC_MODULE_CONFIG_TYPE

  Summary:
   FTP Client Module Configuration run-time parameters.

  Description:
    This data structure describes the FTPC configuration data.
    It is presented to the module as part of the module initialization.

  Remarks:
    None

*/
typedef struct
{
    int         nMaxClients; 	// maximum number of simultaneous Client supported
                                // This is for multi-client support	   
    uint32_t    ftpcTmo; 		// timeout for reply on FTP Control and Data Connections,
                                // in seconds; Should be adjusted according to the server responsivity
    uint16_t    data_tx_buffsize_dflt; //Data Socket Transmit Buffer default size
    uint16_t    data_rx_buffsize_dflt; //Data Socket Receive Buffer default size
}TCPIP_FTPC_MODULE_CONFIG_TYPE;

// *****************************************************************************
/*
  Structure:
    TCPIP_FTPC_CTRL_CONN_TYPE

  Summary:
   FTP Client control connection parameters.

  Description:
    This data structure describes the FTPC control connection parameters.

  Remarks:
    None

*/
typedef struct
{    
    IP_MULTI_ADDRESS*                   ftpcServerAddr;         // server address;
    IP_ADDRESS_TYPE                     ftpcServerIpAddrType;   // connection type: IPv4/IPv6/ANY	
    uint16_t                            serverCtrlPort;         // port number of control socket
}TCPIP_FTPC_CTRL_CONN_TYPE;

// *****************************************************************************
/*
  Structure:
    TCPIP_FTPC_DATA_CONN_TYPE

  Summary:
   FTP Client data connection parameters.

  Description:
    This data structure describes the FTPC data connection parameters.

  Remarks:
    None

*/
typedef struct
{
    bool                                ftpcIsPassiveMode;      // FTP active/passive mode
    TCPIP_FTPC_TRANSFER_MODE_TYPE       ftpcTransferMode;       // FTP Client data transfer mode
    TCPIP_FTPC_DATA_REP_TYPE            ftpcDataType;           // FTP Client data type representation
    TCPIP_FTPC_DATA_STRUCT_TYPE         ftpcDataStructure;      // FTP Client data structure   
    IP_MULTI_ADDRESS                    dataServerAddr;         // Address of Data Server
    IP_ADDRESS_TYPE                     dataServerIpAddrType;   // connection type: IPv4/IPv6/ANY	
    uint16_t                            dataServerPort;         // Port Number at Data Server
    uint16_t                            ftpcDataTxBuffSize;     // Data Tx Buff Size; Data Socket 
                                                                // Tx Buffer size will be updated
    uint16_t                            ftpcDataRxBuffSize;     // Data Rx Buff Size; Data Socket 
                                                                // Rx Buffer size will be updated
}TCPIP_FTPC_DATA_CONN_TYPE;

// *****************************************************************************
/*
  Structure:
    TCPIP_FTPC_FILE_OPT_TYPE

  Summary:
   FTP file handling options

  Description:
    This data structure describes the FTPC file handling parameters.

  Remarks:
    None

*/
typedef struct
{    
    const char *serverPathName; // pathname/filename to read/write at FTP Server
    const char *clientPathName; // pathname/filename to read/write at FTP Client
    bool    store_unique;       // to store file at server with unique name; STOU command
}TCPIP_FTPC_FILE_OPT_TYPE;

// *****************************************************************************
/*
  Structure:
    TCPIP_FTPC_STATUS_TYPE

  Summary:
   FTP Client status information.

  Description:
    This data structure describes the details of FTPC errors.

  Remarks:
    None

*/
typedef struct
{	
    NET_PRES_SKT_HANDLE_T	ctrlSocket;	//control socket
	NET_PRES_SKT_HANDLE_T   dataSocket;	//data socket
    
    TCPIP_FTPC_CMD     		cmd;  	//currently executing or completed command      
    TCPIP_FTPC_ERROR_TYPE   error;	//last reported error, if any
    
    uint32_t   	dataRxLen;  //Data Socket receive length
    uint32_t    dataTxLen;  //Data Socket transmit length	
	uint16_t    ctrlRxLen;	//Control Socket receive length
	uint16_t    ctrlTxLen;  //Control Socket transmit length    
    
    bool 	busy;   		//client busy/ready
	bool	isConnected;	//client control socket connected to server
	bool	isLoggedIn;		//client logged in to server
	
}TCPIP_FTPC_STATUS_TYPE;


// *****************************************************************************
// *****************************************************************************
// Section: FTPC Function Prototypes
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Function:
    TCPIP_FTPC_CONN_HANDLE_TYPE TCPIP_FTPC_Connect(TCPIP_FTPC_CTRL_CONN_TYPE *pftpcConn, 
        TCPIP_FTPC_CTRL_EVENT_CALLBACK_TYPE ctrlCallback, TCPIP_FTPC_RETURN_TYPE* pResult);

  Summary:
    FTP Client Connect function.

  Description:
    This function opens a control socket and establish connection with the specified server port.

  Precondition:
    FTPC module should have been initialized.

  Parameters:
    pftpcConn       - pointer to a TCPIP_FTPC_CTRL_CONN_TYPE describing the control connection parameters
    ctrlCallback    - callback function during Control Socket Event
    pResult         - address to store the operation result

  Returns:
    a valid handle (!= 0)   - if successfully scheduled opening and connecting a control socket 
    TCPIP_FTPC_RET_FAILURE  - if call failed

    an error code otherwise

  Remarks:
	None
*/
TCPIP_FTPC_CONN_HANDLE_TYPE TCPIP_FTPC_Connect(TCPIP_FTPC_CTRL_CONN_TYPE *pftpcConn, 
        TCPIP_FTPC_CTRL_EVENT_CALLBACK_TYPE ctrlCallback, TCPIP_FTPC_RETURN_TYPE* pResult);

// *****************************************************************************
/*
  Function:
    TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_Disconnect (TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle);

  Summary:
    FTP Client Disconnect function.

  Description:
    This function closes a control socket connection with server port.

  Precondition:
    FTPC module should have been initialized and connected using TCPIP_FTPC_Connect()

  Parameters:
    ftpcHandle      - handle identifying the control connection
 
  Returns:    
    TCPIP_FTPC_RET_OK   - if successfully disconnected control socket 
	TCPIP_FTPC_RET_BUSY - could not execute the function due to busy state
    TCPIP_FTPC_RET_NOT_CONNECT  - Not connected to FTP server
    TCPIP_FTPC_RET_NOT_LOGIN    - Connected, but not Logged in

  Remarks:
    This function will execute after the completion of previous FTPC function; otherwise
    it will return TCPIP_FTPC_RET_BUSY
*/
TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_Disconnect (TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle); 

// *****************************************************************************
/*
  Function:
    TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_Login(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
                const char *username, const char *password, const char *account);

  Summary:
    FTP Client Login function

  Description:
    This function starts the Login process for FTP Client

  Precondition:
    FTPC module should have been initialized and connected

  Parameters:
    ftpcHandle      - handle identifying the control connection
	username        - username for FTP server		
	password        - password for FTP server	
	account         - account for FTP server	
 
  Returns:
    TCPIP_FTPC_RET_OK   - if successfully scheduled login process
	TCPIP_FTPC_RET_BUSY   - could not schedule login process as FTP Client is busy
    TCPIP_FTPC_RET_NOT_CONNECT  - Not connected to FTP server
    TCPIP_FTPC_RET_NOT_LOGIN    - Connected, but not Logged in
 
  Remarks:
	This function will execute after the completion of previous FTPC function; otherwise
    it will return TCPIP_FTPC_RET_BUSY
*/
TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_Login(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
                const char *username, const char *password, const char *account);

// *****************************************************************************
/*
  Function:
    TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_Get_WorkingDir (TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle);

  Summary:
    FTP Client print the name of working directory

  Description:
    This function schedules the process to get the name of working directory

  Precondition:
    FTPC module should have been initialized, connected and logged-in

  Parameters:
    ftpcHandle      - handle identifying the control connection
	
  Returns:
    TCPIP_FTPC_RET_OK   - if successfully scheduled function
	TCPIP_FTPC_RET_BUSY   - could not schedule function as FTP Client is busy
    TCPIP_FTPC_RET_NOT_CONNECT  - Not connected to FTP server
    TCPIP_FTPC_RET_NOT_LOGIN    - Connected, but not Logged in
 
  Remarks:
	This function will execute after the completion of previous FTPC function; otherwise
    it will return TCPIP_FTPC_RET_BUSY
*/
TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_Get_WorkingDir (TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle);

// *****************************************************************************
/*
  Function:
    TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_MakeDir(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
                                                                const char *dirname); 

  Summary:
    FTP Client create a new directory

  Description:
    This function schedules the process to create a new directory inside current working directory

  Precondition:
    FTPC module should have been initialized, connected and logged-in

  Parameters:
    ftpcHandle      - handle identifying the control connection
	dirname         - pointer to string for directory name
	
  Returns:
    TCPIP_FTPC_RET_OK   - if successfully scheduled function
	TCPIP_FTPC_RET_BUSY   - could not schedule function as FTP Client is busy
    TCPIP_FTPC_RET_NOT_CONNECT  - Not connected to FTP server
    TCPIP_FTPC_RET_NOT_LOGIN    - Connected, but not Logged in
 
  Remarks:
	This function will execute after the completion of previous FTPC function; otherwise
    it will return TCPIP_FTPC_RET_BUSY
*/
TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_MakeDir(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
                                                                const char *dirname); 

// *****************************************************************************
/*
  Function:
    TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_Change_Dir(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
                                                                const char *pathname);

  Summary:
    FTP Client change working directory

  Description:
    This function schedules the process to change the working directory to a different one

  Precondition:
    FTPC module should have been initialized, connected and logged-in

  Parameters:
    ftpcHandle      - handle identifying the control connection
	pathname        - pointer to string for new directory path
	
  Returns:
    TCPIP_FTPC_RET_OK   - if successfully scheduled function
	TCPIP_FTPC_RET_BUSY   - could not schedule function as FTP Client is busy
    TCPIP_FTPC_RET_NOT_CONNECT  - Not connected to FTP server
    TCPIP_FTPC_RET_NOT_LOGIN    - Connected, but not Logged in
 
  Remarks:
	This function will execute after the completion of previous FTPC function; otherwise
    it will return TCPIP_FTPC_RET_BUSY
*/
TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_Change_Dir(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
                                                                const char *pathname);

// *****************************************************************************
/*
  Function:
    TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_ChangeToParentDir(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle);

  Summary:
    FTP Client make parent directory as working directory

  Description:
    This function schedules the process to switch the root(parent) directory as working directory

  Precondition:
    FTPC module should have been initialized, connected and logged-in

  Parameters:
    ftpcHandle      - handle identifying the control connection
	
  Returns:
    TCPIP_FTPC_RET_OK   - if successfully scheduled function
	TCPIP_FTPC_RET_BUSY   - could not schedule function as FTP Client is busy
    TCPIP_FTPC_RET_NOT_CONNECT  - Not connected to FTP server
    TCPIP_FTPC_RET_NOT_LOGIN    - Connected, but not Logged in
 
  Remarks:
	This function will execute after the completion of previous FTPC function; otherwise
    it will return TCPIP_FTPC_RET_BUSY
*/
TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_ChangeToParentDir(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle);

// *****************************************************************************
/*
  Function:
    TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_Logout(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle);

  Summary:
    FTP Client instance logout

  Description:
    This function schedules the process to logout from the server

  Precondition:
    FTPC module should have been initialized, connected and logged-in

  Parameters:
    ftpcHandle 	- handle identifying the control connection
	
  Returns:
    TCPIP_FTPC_RET_OK   - if successfully scheduled function
	TCPIP_FTPC_RET_BUSY   - could not schedule function as FTP Client is busy
    TCPIP_FTPC_RET_NOT_CONNECT  - Not connected to FTP server
    TCPIP_FTPC_RET_NOT_LOGIN    - Connected, but not Logged in
 
  Remarks:
	This function will execute after the completion of previous FTPC function; otherwise
    it will return TCPIP_FTPC_RET_BUSY
*/
TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_Logout(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle);

// *****************************************************************************
/*
  Function:
    TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_RemoveDir(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
                                                                const char *pathname);

  Summary:
    FTP Client remove the directory

  Description:
    This function schedules the process to remove a directory at FTP server

  Precondition:
    FTPC module should have been initialized, connected and logged-in

  Parameters:
    ftpcHandle      - handle identifying the control connection	
	pathname        - pointer to directory name to be removed
	
  Returns:
    TCPIP_FTPC_RET_OK   - if successfully scheduled function
	TCPIP_FTPC_RET_BUSY   - could not schedule function as FTP Client is busy
    TCPIP_FTPC_RET_NOT_CONNECT  - Not connected to FTP server
    TCPIP_FTPC_RET_NOT_LOGIN    - Connected, but not Logged in
 
  Remarks:
	This function will execute after the completion of previous FTPC function; otherwise
    it will return TCPIP_FTPC_RET_BUSY
*/
TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_RemoveDir(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
                                                                const char *pathname);

// *****************************************************************************
/*
  Function:
    TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_DeleteFile(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
                                                                const char *pathname);

  Summary:
    FTP Client delete file

  Description:
    This function schedules the process to delete a file in FTP server

  Precondition:
    FTPC module should have been initialized, connected and logged-in

  Parameters:
    ftpcHandle      - handle identifying the control connection	
	pathname        - pointer to file path name to be removed
	
  Returns:
    TCPIP_FTPC_RET_OK   - if successfully scheduled function
	TCPIP_FTPC_RET_BUSY   - could not schedule function as FTP Client is busy
    TCPIP_FTPC_RET_NOT_CONNECT  - Not connected to FTP server
    TCPIP_FTPC_RET_NOT_LOGIN    - Connected, but not Logged in
 
  Remarks:
	This function will execute after the completion of previous FTPC function; otherwise
    it will return TCPIP_FTPC_RET_BUSY
*/
TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_DeleteFile(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
                                                                const char *pathname);


// *****************************************************************************
/*
  Function:
    TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_GetFile(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
        TCPIP_FTPC_DATA_CONN_TYPE *pftpcDataConn, TCPIP_FTPC_FILE_OPT_TYPE *pfileOptions, 
                                        TCPIP_FTPC_DATA_EVENT_CALLBACK_TYPE dataCallback);

  Summary:
    FTP Client Get file function

  Description:
    This function schedules the process to get a file from FTP server

  Precondition:
    FTPC module should have been initialized, connected and logged-in

  Parameters:
    ftpcHandle      - handle identifying the control connection	
	pftpcDataConn 	- pointer to data connection parameters
	pfileOptions    - pointer to file options
    dataCallback    - callback function during Data Socket Event
 
  Returns:
    TCPIP_FTPC_RET_OK   - if successfully scheduled function
	TCPIP_FTPC_RET_BUSY   - could not schedule function as FTP Client is busy
    TCPIP_FTPC_RET_NOT_CONNECT  - Not connected to FTP server
    TCPIP_FTPC_RET_NOT_LOGIN    - Connected, but not Logged in
 
  Remarks:
	This function will execute after the completion of previous FTPC function; otherwise
    it will return TCPIP_FTPC_RET_BUSY
*/
TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_GetFile(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
        TCPIP_FTPC_DATA_CONN_TYPE *pftpcDataConn, TCPIP_FTPC_FILE_OPT_TYPE *pfileOptions, 
                                        TCPIP_FTPC_DATA_EVENT_CALLBACK_TYPE dataCallback);

// *****************************************************************************
/*
  Function:
    TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_PutFile(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
        TCPIP_FTPC_DATA_CONN_TYPE *pftpcDataConn, TCPIP_FTPC_FILE_OPT_TYPE *pfileOptions, 
                                     TCPIP_FTPC_DATA_EVENT_CALLBACK_TYPE dataCallback);

  Summary:
    FTP Client Put file function

  Description:
    This function schedules the process to put a file to FTP server

  Precondition:
    FTPC module should have been initialized, connected and logged-in

  Parameters:
    ftpcHandle 		- handle identifying the control connection	
	pftpcDataConn 	- pointer to data connection parameters
	pfileOptions    - pointer to file options
    dataCallback    - callback function during Data Socket Event
	
  Returns:
    TCPIP_FTPC_RET_OK   - if successfully scheduled function
	TCPIP_FTPC_RET_BUSY   - could not schedule function as FTP Client is busy
    TCPIP_FTPC_RET_NOT_CONNECT  - Not connected to FTP server
    TCPIP_FTPC_RET_NOT_LOGIN    - Connected, but not Logged in
 
  Remarks:
	This function will execute after the completion of previous FTPC function; otherwise
    it will return TCPIP_FTPC_RET_BUSY
*/
TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_PutFile(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
        TCPIP_FTPC_DATA_CONN_TYPE *pftpcDataConn, TCPIP_FTPC_FILE_OPT_TYPE *pfileOptions, 
                                     TCPIP_FTPC_DATA_EVENT_CALLBACK_TYPE dataCallback);

// *****************************************************************************
/*
  Function:
    TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_NameList(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle,
        TCPIP_FTPC_DATA_CONN_TYPE *pftpcDataConn, TCPIP_FTPC_FILE_OPT_TYPE *pfileOptions, 
                                        TCPIP_FTPC_DATA_EVENT_CALLBACK_TYPE dataCallback);

  Summary:
    FTP Client Name List function

  Description:
    This function schedules the process to get the name list of files in a directory

  Precondition:
    FTPC module should have been initialized, connected and logged-in

  Parameters:
    ftpcHandle 		- handle identifying the control connection	
	pftpcDataConn 	- pointer to data connection parameters
	pfileOptions    - pointer to file options
    dataCallback    - callback function during Data Socket Event
	
  Returns:
    TCPIP_FTPC_RET_OK   - if successfully scheduled function
	TCPIP_FTPC_RET_BUSY   - could not schedule function as FTP Client is busy
    TCPIP_FTPC_RET_NOT_CONNECT  - Not connected to FTP server
    TCPIP_FTPC_RET_NOT_LOGIN    - Connected, but not Logged in
 
  Remarks:
	This function will execute after the completion of previous FTPC function; otherwise
    it will return TCPIP_FTPC_RET_BUSY
*/
TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_NameList(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle,
        TCPIP_FTPC_DATA_CONN_TYPE *pftpcDataConn, TCPIP_FTPC_FILE_OPT_TYPE *pfileOptions, 
                                        TCPIP_FTPC_DATA_EVENT_CALLBACK_TYPE dataCallback);

// *****************************************************************************
/*
  Function:
    TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_List(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle,
        TCPIP_FTPC_DATA_CONN_TYPE *pftpcDataConn, TCPIP_FTPC_FILE_OPT_TYPE *pfileOptions, 
                                        TCPIP_FTPC_DATA_EVENT_CALLBACK_TYPE dataCallback);

  Summary:
    FTP Client List function

  Description:
    This function schedules the process to get the list of files in a directory

  Precondition:
    FTPC module should have been initialized, connected and logged-in

  Parameters:
    ftpcHandle 		- handle identifying the control connection	
	pftpcDataConn 	- pointer to data connection parameters
	pfileOptions    - pointer to file options
    dataCallback    - callback function during Data Socket Event
	
  Returns:
    TCPIP_FTPC_RET_OK   - if successfully scheduled function
	TCPIP_FTPC_RET_BUSY   - could not schedule function as FTP Client is busy
    TCPIP_FTPC_RET_NOT_CONNECT  - Not connected to FTP server
    TCPIP_FTPC_RET_NOT_LOGIN    - Connected, but not Logged in
 
  Remarks:
	This function will execute after the completion of previous FTPC function; otherwise
    it will return TCPIP_FTPC_RET_BUSY
*/
TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_List(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle,
        TCPIP_FTPC_DATA_CONN_TYPE *pftpcDataConn, TCPIP_FTPC_FILE_OPT_TYPE *pfileOptions, 
                                        TCPIP_FTPC_DATA_EVENT_CALLBACK_TYPE dataCallback);

// *****************************************************************************
/*
  Function:
    void TCPIP_FTPC_Get_Status(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, TCPIP_FTPC_STATUS_TYPE * ftpcStatus);

  Summary:
    FTP Client status info function

  Description:
    This function returns the current state of FTP Client

  Precondition:
    FTPC module should have been initialized, connected and logged-in

  Parameters:
    ftpcHandle - handle identifying the control connection	
    ftpcStatus - pointer to FTP Client status information for returning the status
	
  Returns:
    None

  Remarks:
	None
*/
void TCPIP_FTPC_Get_Status(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, TCPIP_FTPC_STATUS_TYPE * ftpcStatus);

// *****************************************************************************
/*
  Function:
    void  TCPIP_FTPC_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    this function performs FTPC module tasks in the TCP/IP stack.

  Precondition:
    FTPC module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_FTPC_Task(void);

// *****************************************************************************
/*
  Function:
    TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_SetType(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
                                                    TCPIP_FTPC_DATA_REP_TYPE type);

  Summary:
    Set FTP Client Data representation type

  Description:
    This function schedules the process to set the type of data representation

  Precondition:
    FTPC module should have been initialized, connected and logged-in

  Parameters:
    ftpcHandle      - handle identifying the control connection	
	type            - Data representation type to be set
	
  Returns:
    TCPIP_FTPC_RET_OK   - if successfully scheduled function
	TCPIP_FTPC_RET_BUSY   - could not schedule function as FTP Client is busy
    TCPIP_FTPC_RET_NOT_CONNECT  - Not connected to FTP server
    TCPIP_FTPC_RET_NOT_LOGIN    - Connected, but not Logged in
 
  Remarks:
	This function will execute after the completion of previous FTPC function; otherwise
    it will return TCPIP_FTPC_RET_BUSY
*/
TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_SetType(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
                                                    TCPIP_FTPC_DATA_REP_TYPE type);

// *****************************************************************************
/*
  Function:
    TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_SetStruct(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
                                            TCPIP_FTPC_DATA_STRUCT_TYPE file_struct);

  Summary:
    Set FTP Client File structure type

  Description:
    This function schedules the process to set the file structure

  Precondition:
    FTPC module should have been initialized, connected and logged-in

  Parameters:
    ftpcHandle      - handle identifying the control connection	
	file_struct		- File Structure to be set
	
  Returns:
    TCPIP_FTPC_RET_OK   - if successfully scheduled function
	TCPIP_FTPC_RET_BUSY   - could not schedule function as FTP Client is busy
    TCPIP_FTPC_RET_NOT_CONNECT  - Not connected to FTP server
    TCPIP_FTPC_RET_NOT_LOGIN    - Connected, but not Logged in
 
  Remarks:
	This function will execute after the completion of previous FTPC function; otherwise
    it will return TCPIP_FTPC_RET_BUSY
*/
TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_SetStruct(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
                                            TCPIP_FTPC_DATA_STRUCT_TYPE file_struct);

// *****************************************************************************
/*
  Function:
    TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_SetMode(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
                                                TCPIP_FTPC_TRANSFER_MODE_TYPE mode);

  Summary:
    Set FTP Client Transfer Mode

  Description:
    This function schedules the process to set the Transfer Mode

  Precondition:
    FTPC module should have been initialized, connected and logged-in

  Parameters:
    ftpcHandle      - handle identifying the control connection	
	mode            - File Structure to be set
	
  Returns:
    TCPIP_FTPC_RET_OK   - if successfully scheduled function
	TCPIP_FTPC_RET_BUSY   - could not schedule function as FTP Client is busy
    TCPIP_FTPC_RET_NOT_CONNECT  - Not connected to FTP server
    TCPIP_FTPC_RET_NOT_LOGIN    - Connected, but not Logged in
 
  Remarks:
	This function will execute after the completion of previous FTPC function; otherwise
    it will return TCPIP_FTPC_RET_BUSY
*/
TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_SetMode(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
                                                TCPIP_FTPC_TRANSFER_MODE_TYPE mode);
// *****************************************************************************
/*
  Function:
    TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_SetActiveMode(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
                                                TCPIP_FTPC_DATA_CONN_TYPE *pftpcDataConn);

  Summary:
    Set FTP Client connection mode as Active

  Description:
    This function schedules the process to set active mode for data connection

  Precondition:
    FTPC module should have been initialized, connected and logged-in

  Parameters:
    ftpcHandle      - handle identifying the control connection	
    pftpcDataConn 	- pointer to data connection parameters
	
  Returns:
    TCPIP_FTPC_RET_OK   - if successfully scheduled function
	TCPIP_FTPC_RET_BUSY   - could not schedule function as FTP Client is busy
    TCPIP_FTPC_RET_NOT_CONNECT  - Not connected to FTP server
    TCPIP_FTPC_RET_NOT_LOGIN    - Connected, but not Logged in
 
  Remarks:
	This function will execute after the completion of previous FTPC function; otherwise
    it will return TCPIP_FTPC_RET_BUSY
*/
TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_SetActiveMode(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, 
                                                TCPIP_FTPC_DATA_CONN_TYPE *pftpcDataConn);

// *****************************************************************************
/*
  Function:
    TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_SetPassiveMode(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle); 

  Summary:
    Set FTP Client connection mode as Passive

  Description:
    This function schedules the process to set passive mode for data connection

  Precondition:
    FTPC module should have been initialized, connected and logged-in

  Parameters:
    ftpcHandle      - handle identifying the control connection	
	
  Returns:
    TCPIP_FTPC_RET_OK   - if successfully scheduled function
	TCPIP_FTPC_RET_BUSY   - could not schedule function as FTP Client is busy
    TCPIP_FTPC_RET_NOT_CONNECT  - Not connected to FTP server
    TCPIP_FTPC_RET_NOT_LOGIN    - Connected, but not Logged in
 
  Remarks:
	This function will execute after the completion of previous FTPC function; otherwise
    it will return TCPIP_FTPC_RET_BUSY
*/
TCPIP_FTPC_RETURN_TYPE TCPIP_FTPC_SetPassiveMode(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle); 


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // __FTPC_H

