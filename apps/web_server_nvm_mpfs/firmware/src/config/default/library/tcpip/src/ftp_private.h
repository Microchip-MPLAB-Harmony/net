/*******************************************************************************
  FTP Private Definitions for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    ftp_private.h

  Summary:
    
  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2012 released Microchip Technology Inc.  All rights reserved.

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
// DOM-IGNORE-END

#ifndef __FTP_PRIVATE_H
#define __FTP_PRIVATE_H


//#define TCPIP_FTP_USER_NAME_LEN 			(10u)
//#define TCPIP_FTP_PASSWD_LEN 				(10u)

// Determines max characters to get from the FTP socket
#define TCPIP_FTP_CMD_MAX_STRING_LEN 		(31u)

// Used to tell ParseFTPString() function when to stop.
#define TCPIP_FTP_MAX_ARGS                 	(7u)

/* FTP private configuration data and this is used */
typedef struct
{
    uint16_t    nConnections;
    uint16_t    dataSktTxBuffSize;  
    uint16_t    dataSktRxBuffSize; 
    char        userName[TCPIP_FTP_USER_NAME_LEN + 1];
    char        password[TCPIP_FTP_PASSWD_LEN];
}TCPIP_FTP_MODULE_CONFIG_DATA;


/*
* FTP initial state machine 
*/
typedef enum _TCPIP_FTP_SM
{
	TCPIP_FTP_SM_HOME,				// if FTP server home
    TCPIP_FTP_SM_NOT_CONNECTED,    // If FTP Server is not connected
    TCPIP_FTP_SM_CONNECTED,  // FTP Server is connected
    TCPIP_FTP_SM_USER_NAME,  // FTP login user name
    TCPIP_FTP_SM_USER_PASS,  // FTP login password
    TCPIP_FTP_SM_RESPOND     // FTP response
} TCPIP_FTP_SM;

/*
* FTP command state machine
*/
typedef enum _TCPIP_FTP_CMD_SM
{
    TCPIP_FTP_CMD_SM_IDLE,
    TCPIP_FTP_CMD_SM_WAIT,
    TCPIP_FTP_CMD_SM_RECEIVE,
    TCPIP_FTP_CMD_SM_SEND,
    TCPIP_FTP_CMD_SM_SEND_FILE,
    TCPIP_FTP_CMD_SM_SEND_DIR,
    TCPIP_FTP_CMD_SM_SEND_DIR_HEADER,
    TCPIP_FTP_CMD_SM_SEND_DIR_DETAIL,
    TCPIP_FTP_CMD_SM_WAIT_FOR_DISCONNECT
} TCPIP_FTP_CMD_SM;

/*
* List of FTP Commands
*/
typedef enum _TCPIP_FTP_CMD
{
    TCPIP_FTP_CMD_USER, 
    TCPIP_FTP_CMD_PASS,
    TCPIP_FTP_CMD_QUIT,
    TCPIP_FTP_CMD_STOR,
    TCPIP_FTP_CMD_PORT,
    TCPIP_FTP_CMD_ABORT,
    TCPIP_FTP_CMD_PWD,
    TCPIP_FTP_CMD_CWD,
    TCPIP_FTP_CMD_TYPE,    
    TCPIP_FTP_CMD_RETR,
    TCPIP_FTP_CMD_SIZE,
    TCPIP_FTP_CMD_PASV,
    TCPIP_FTP_CMD_NLST,
    TCPIP_FTP_CMD_EPSV,
    TCPIP_FTP_CMD_EPRT,
	TCPIP_FTP_CMD_EXTND_LIST,
	TCPIP_FTP_CMD_XPWD,
    TCPIP_FTP_CMD_FEAT,
    TCPIP_FTP_CMD_SYST,
    TCPIP_FTP_CMD_MDTM,
    TCPIP_FTP_CMD_MLST,
    TCPIP_FTP_CMD_MLSD,
    TCPIP_FTP_CMD_UNKNOWN,
    TCPIP_FTP_CMD_NONE,
   
    
} TCPIP_FTP_CMD;

/*
* Enum types for FTP Response 
*/
typedef enum _TCPIP_FTP_RESP
{
    TCPIP_FTP_RESP_BANNER,
    TCPIP_FTP_RESP_USER_OK,
    TCPIP_FTP_RESP_PASS_OK,
    TCPIP_FTP_RESP_QUIT_OK,
    TCPIP_FTP_RESP_STOR_OK,
    TCPIP_FTP_RESP_UNKNOWN,
    TCPIP_FTP_RESP_LOGIN,
    TCPIP_FTP_RESP_DATA_OPEN,
    TCPIP_FTP_RESP_DATA_READY,
    TCPIP_FTP_RESP_DATA_CLOSE,
	TCPIP_FTP_RESP_DATA_NO_SOCKET,
	TCPIP_FTP_RESP_PWD,
    TCPIP_FTP_RESP_OK,
    TCPIP_FTP_RESP_FILE_NOT_EXIST,
    TCPIP_FTP_RESP_FILE_IS_PRESENT,
    TCPIP_FTP_RESP_ENTER_PASV_MODE,
    TCPIP_FTP_RESP_FILE_ACTION_SUCCESSFUL_CLOSING_DATA_CONNECTION,
    TCPIP_FTP_RESP_FILE_STATUS,
    TCPIP_FTP_RESP_EXTND_PORT_FAILURE,
    TCPIP_FTP_RESP_FILESYSTEM_FAIL,
    TCPIP_FTP_RESP_SYST,
    TCPIP_FTP_RESP_NONE ,                      // This must always be the last
                                        // There is no corresponding string.
} TCPIP_FTP_RESP;

// FTP Flags
typedef union _TCPIP_FTP_Flags
{
    struct
    {
        unsigned char userSupplied : 1;
        unsigned char loggedIn: 1;
		unsigned char pasvMode: 1;
    } Bits;
    uint8_t val;
} TCPIP_FTP_Flags;

//TCPIP FTP descriptor details
typedef struct _TCPIP_FTP_DCPT
{
    TCPIP_FTP_SM   		ftpSm;     // current status
    TCP_SOCKET          ftpCmdskt;    // associated FTP command socket
    TCP_SOCKET          ftpDataskt;    // associated FTP Data port socket
    
	uint16_t			ftpDataPort;
// FTP command 	
	TCPIP_FTP_CMD		ftpCommand;
	TCPIP_FTP_RESP		ftpResponse;
//FTP command state machine
	TCPIP_FTP_CMD_SM	ftpCommandSm;

	uint8_t          	ftpUserName[TCPIP_FTP_USER_NAME_LEN+1];
	uint8_t          	ftpCmdString[TCPIP_FTP_CMD_MAX_STRING_LEN+2];
	uint8_t          	ftpStringLen;
	uint8_t          	*ftp_argv[TCPIP_FTP_MAX_ARGS];    // Parameters for a FTP command
	uint8_t          	ftp_argc;       // Total number of params for a FTP command
	uint32_t        	ftpSysTicklastActivity;   // Timeout keeper.
	TCPIP_FTP_Flags     ftpFlag;
	uint32_t 			callbackPos;					// Callback position indicator
	uint16_t			adressFamilyProtocol;   // AF_NUMBER used for EPRT command
	int32_t				fileDescr;
    TCPIP_TCP_SIGNAL_HANDLE ftpTcpCmdSocketSignal;
    TCPIP_TCP_SIGNAL_HANDLE ftpTcpDataSocketSignal;
    
}TCPIP_FTP_DCPT;


typedef struct  _TAG_FTP_LIST_NODE
{
    struct _TAG_DNS_LIST_NODE*      next;	// next node in list
                                                // makes it valid SGL_LIST_NODE node
    SYS_FS_FSTAT        file_stat;
    bool        lfNamePresent;

}FTP_LIST_NODE;

#endif  // __FTP_PRIVATE_H

