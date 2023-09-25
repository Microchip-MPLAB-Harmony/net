/*******************************************************************************
  Tftp client private

  Summary:
    
  Description:
*******************************************************************************/

/*
Copyright (C) 2012-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef __TFTPC_PRIVATE_H_
#define __TFTPC_PRIVATE_H_

#define TCPIP_TFTP_CLIENT_MAX_HOSTNAME_LEN  32
#define TCPIP_TFTP_CLIENT_FILE_NAME_LEN     32
#define TCPIP_TFTP_BLOCK_SIZE_SUPPORTED     512
#define TCPIP_TFTP_CLIENT_MAX_BUFFER_SIZE   (TCPIP_TFTP_BLOCK_SIZE_SUPPORTED+5)
#define TCPIP_TFTP_CLIENT_OPCODE            2
#define TCPIP_TFTP_CLIENT_OCTET             6

/* This can be enabled to get TFTPC debugging messages */
// #define TCPIP_TFTPC_DEBUG

// Enum. of results returned by most of the TFTP functions.
typedef enum _TFTP_RESULT
{
    TFTP_OK = 0,
    TFTP_NOT_READY,
    TFTP_END_OF_FILE,
    TFTP_ACK_SEND,
    TFTP_ERROR,
    TFTP_RETRY,    
    TFTP_TIMEOUT,
    TFTP_PKT_NOT_RECEIVED,
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
    SM_TFTP_END,
} TFTP_STATE;

// The TFTP Communication state machine
typedef enum
{
    SM_TFTP_COMM_HOME=0,
    SM_TFTP_COMM_WAIT_FOR_DATA,
    SM_TFTP_COMM_WAIT_FOR_ACK,
    SM_TFTP_COMM_DUPLICATE_ACK,
    SM_TFTP_COMM_SEND_ACK,
    SM_TFTP_COMM_SEND_LAST_ACK,
    SM_TFTP_COMM_SEND_NEXT_DATA_PKT,
    SM_TFTP_COMM_END,
} TFTP_COMM_STATE;

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
    UDP_SOCKET  hSocket; // Handle to TFTP client socket
    TFTP_STATE  smState;  // TFTP client state machine variable
    TFTP_COMM_STATE smCommSate; // TFTP Client Communication state
    TCPIP_NET_HANDLE    netH;   // interface handled
    IP_MULTI_ADDRESS tftpServerAddr;  // TFTP Server IP address
    IP_ADDRESS_TYPE  ipAddrType;
    TFTP_FILE_MODE modeType; // mode type either read or write
    char fileName[TCPIP_TFTP_CLIENT_MAX_HOSTNAME_LEN+1]; // file name for upload and download
    int32_t     fileDescr; // File descriptor
    uint32_t        callbackPos;
} TFTP_CLIENT_VARS;

// TFTP client event registration

typedef struct  _TAG_TFTPC_LIST_NODE
{
    struct _TAG_TFTPC_LIST_NODE*        next;       // next node in list
                                                // makes it valid SGL_LIST_NODE node
    TCPIP_TFTPC_EVENT_HANDLER        handler;    // handler to be called for event
    const void*                     hParam;     // handler parameter
    TCPIP_NET_HANDLE                hNet;       // interface that's registered for
                                                // 0 if all    
}TCPIP_TFTPC_LIST_NODE;

#endif  // __TFTPC_PRIVATE_H_


