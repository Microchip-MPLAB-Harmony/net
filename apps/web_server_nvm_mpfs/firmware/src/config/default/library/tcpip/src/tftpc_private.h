/*******************************************************************************
  Tftp client private

  Summary:
    
  Description:
*******************************************************************************/

/*******************************************************************************
FileName:  tftpc_private.h
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

#ifndef __TFTPC_PRIVATE_H_
#define __TFTPC_PRIVATE_H_

#define TCPIP_TFTP_CLIENT_MAX_HOSTNAME_LEN  32
#define TCPIP_TFTP_CLIENT_FILE_NAME_LEN     32
#define TCPIP_TFTP_CLIENT_MAX_BUFFER_SIZE   (512+5)
#define TCPIP_TFTP_CLIENT_OPCODE            2
#define TCPIP_TFTP_CLIENT_OCTET             6

#define TCPIP_TFTPC_DEBUG

// Enum. of results returned by most of the TFTP functions.
typedef enum _TFTP_RESULT
{
    TFTP_OK = 0,
    TFTP_NOT_READY,
    TFTP_END_OF_FILE,
    TFTP_ACK_SEND,
    TFTP_ERROR,
    TFTP_RETRY,    
    TFTP_TIMEOUT
} TFTP_RESULT;

// The TFTP state machine
typedef enum
{
    SM_TFTP_HOME=0,
    SM_TFTP_WAIT_DNS,
    SM_TFTP_PROCESS_COMMAND,
    SM_TFTP_UDP_IS_OPENED,
    SM_TFTP_FILE_OPEN_AND_SEND_REQUEST,
    SM_TFTP_PUT_COMMAND,
    SM_TFTP_GET_COMMAND,
    SM_TFTP_WAIT,
    SM_TFTP_READY,
    SM_TFTP_WAIT_FOR_DATA,
    SM_TFTP_WAIT_FOR_ACK,
    SM_TFTP_DUPLICATE_ACK,
    SM_TFTP_SEND_ACK,
    SM_TFTP_SEND_LAST_ACK,
    SM_TFTP_END,
} TFTP_STATE;

// Enumeration of TFTP opcodes
typedef enum
{
    TFTP_OPCODE_RRQ = 1,        // Get
    TFTP_OPCODE_WRQ,            // Put
    TFTP_OPCODE_DATA,           // Actual data
    TFTP_OPCODE_ACK,            // Ack for Get/Put
    TFTP_OPCODE_ERROR           // Error
} TFTP_OPCODE;

// typedef TFTP File mode
typedef enum
{    
    TFTP_FILE_MODE_READ=1,  // TFTP mode read
    TFTP_FILE_MODE_WRITE,   // TFTP mode write
    TFTP_FILE_MODE_NONE,
}TFTP_FILE_MODE;

// Unique variables per interface
typedef struct
{
    UDP_SOCKET	hSocket; // Handle to TFTP client socket
    TFTP_STATE	smState;  // TFTP client state machine variable
    TCPIP_NET_HANDLE    netH;   // interface handled
    IP_MULTI_ADDRESS tftpServerAddr;  // TFTP Server IP address
    IP_ADDRESS_TYPE  ipAddrType;
    TFTP_FILE_MODE modeType; // mode type either read or write
    char fileName[TCPIP_TFTP_CLIENT_MAX_HOSTNAME_LEN+1]; // file name for upload and download
    int32_t		fileDescr; // File descriptor
    uint32_t 		callbackPos;
} TFTP_CLIENT_VARS;

// TFTP client event registration

typedef struct  _TAG_TFTPC_LIST_NODE
{
	struct _TAG_TFTPC_LIST_NODE*		next;		// next node in list
                                                // makes it valid SGL_LIST_NODE node
    TCPIP_TFTPC_EVENT_HANDLER        handler;    // handler to be called for event
    const void*                     hParam;     // handler parameter
    TCPIP_NET_HANDLE                hNet;       // interface that's registered for
                                                // 0 if all    
}TCPIP_TFTPC_LIST_NODE;

#endif  // __TFTPC_PRIVATE_H_


