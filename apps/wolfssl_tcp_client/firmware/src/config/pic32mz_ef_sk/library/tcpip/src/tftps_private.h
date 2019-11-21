/*******************************************************************************
  Tftp server private

  Summary:
    
  Description:
*******************************************************************************/

/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

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
#ifndef __TFTPS_PRIVATE_H_
#define __TFTPS_PRIVATE_H_

#include "configuration.h"

#define TCPIP_TFTP_MAX_HOSTNAME_LEN  32
#define TCPIP_TFTP_OPCODE_SIZE        2
#define TCPIP_TFTP_OCTET_SIZE         6
#define TCPIP_TFTP_OPTION_SIZE        8
#define TCPIP_TFTP_HEADER_MINSIZE     4  
#define TCPIP_TFTP_MIN_BUFFER_SIZE   (512+TCPIP_TFTP_HEADER_MINSIZE)


#define TCPIP_TFTP_TRANSFERMODE_OCTET  "octet"
#define TCPIP_TFTP_TRANSFERMODE_ASCII  "netascii"

// Even though the RFC allows blocks of up to 65464 bytes, 
//in practice the limit is set to 1468 bytes: the size of an 
// Ethernet MTU minus the headers of TFTP (4 bytes), UDP (8 bytes) and IP (20 bytes). 
#define TCPIP_TFTP_BLOCK_SIZE_MAX         1468UL
#define TCPIP_TFTP_BLOCK_SIZE_MIN         8

// This includes 2(Opcode)+32 ( sourcefile)+6 (type)+
// 13 (blksize option, ?blksize? string+ the max value will be 1500) + 
// 12(timeout option, ?timeout? string + range should be between 1 to 255 ) + 
// 6 ( tsize option, ?tisze? string + 0 as input value for Read request 
// and around 10 bytes of value during Write request )
// The total size is 81 bytes for the write request. 
// By default the TCPIP_TFTPS_FILENAME_LEN is limited to 64 bytes.
// same size can be used for the ACK and ERROR opcode received packets.
// Error message is not processed. Only the error code is processed.So the Error 
// string is not validated.So the  4 bytes of RX buffer length for 
// ACK and Error packet is required.
#define TCPIP_TFTPS_MIN_UDP_RX_BUFFER_SIZE      (TCPIP_TFTPS_FILENAME_LEN+50)
#define TCPIP_TFTPS_MIN_UDP_TX_BUFFER_SIZE        (TCPIP_TFTP_MIN_BUFFER_SIZE)

/* TFTP Opcodes defined by RFC 783 */
#define TFTPS_RRQ_OPCODE       1
#define TFTPS_WRQ_OPCODE       2
#define TFTPS_DATA_OPCODE      3
#define TFTPS_ACK_OPCODE       4
#define TFTPS_ERROR_OPCODE     5

/* TFTP Opcode defined by RFC 2347 */
#define TFTPS_OACK_OPCODE      6

/* Connection Status Values */
#define TFTPS_CB_FREE                   100
#define TFTPS_WAITINGFOR_OPTIONACK      101
#define TFTPS_TRANSFERRING_FILE         102
#define TFTPS_TRANSFER_COMPLETE         103
#define TFTPS_PROCESS_INPROGRESS        104
#define TFTPS_READY_TO_SENDRECEIVE      105

/* Type Of File Transfer */
#define TFTPS_READ_TYPE   0   
#define TFTPS_WRITE_TYPE  1 
#define TFTPS_TYPE_NONE   2


/* User defined defaults */
#define TFTPS_TIMEOUT_DEFAULT        60
#define TFTPS_NUM_RETRANS            3

typedef enum
{
    TFTP_OPCODE_OFFSET = 0,   // TFTP opcode starts from array offset 0, Opcode is two bytes
    TFTP_FILENAME_OFFSET = 2,  // TFTP File name starts after Opcode and
                                //This is a Null terminated field. so number of bytes varies.
    TFTP_DATA_BLOCKNUM_OFFSET = 2, // TFTP block number offset and this comes with DATA packet
    TFTP_ERROR_VAL_OFFSET = 2, // TFTP error value offset
    TFTP_DATA_OFFSET = 4, // TFTP data packet offset
    TFTP_MODE_OFFSET,           // TFTP file mode starts after file name.
                                // This is a Null terminated field. so number of bytes varies.
    TFTP_OPTION_OFFSET,         //  TFTP option also a Null terminated field 
                                //and the number of option varies  
}TFTP_HEADER_OFFSET;

// result of an TFTP protocol error code enum value
typedef enum
{
    TFTP_ERROR                		= 0,    // Not defined, see error message (if any)
    TFTP_FILE_NOT_FOUND_ERROR       = 1,   // File not found
    TFTP_ACCESS_VIOLATION_ERROR     = 2,   // Access violation
    TFTP_ALLOCATION_ERROR    		= 3,   // Disk full or allocation exceeded
    TFTP_ILLIGAL_OPERATION_ERROR    = 4,   // Illegal TFTP operation
    TFTP_UNKNOWN_ID_ERROR           = 5,   // Unknown transfer ID
    TFTP_FILE_EXISTS_ERROR		    = 6,   // File already exists
	TFTP_NO_SUCH_ERROR				= 7,	  // No such user
	TFTP_BAD_TFTP_ERROR				= 8,	   // Bad TFTP Option
}TFTP_ERROR_CODE;

// The TFTP server state machine for each client
typedef enum
{
    SM_TFTPS_HOME=0,
    SM_TFTPS_RRECV,    // Receive a message from a client and don't return
                       // until data is received from the socket. _TFTPS_Recv_Request_State()
    SM_TFTPS_PROCESS_DATA,  // PUT command , _TFTPS_Process_Data_State()
    SM_TFTPS_SEND_DATA,  // GET command , _TFTPS_Send_Data_State()   
    SM_TFTPS_END,
} TFTPS_STATE;


// Unique variables per interface
typedef struct
{
    UDP_SOCKET          uSkt; // Handle to TFTP Server socket
    //TFTPS_STATE         smState;  // TFTP Server state machine variable
    TCPIP_NET_HANDLE    netH;   // interface handled
    IP_ADDRESS_TYPE     ipAddrType;
    TCPIP_NET_IF*       prefNet; // preferred TFTP server interface, if set
    const void          *memH;
#if (TCPIP_TFTPS_USER_NOTIFICATION != 0)
    PROTECTED_SINGLE_LIST   tftpsRegisteredUsers;
#endif  // (TCPIP_TFTPS_USER_NOTIFICATION != 0)
} TCPIP_TFTPS_DCPT;

// TFTP Server event registration

typedef struct  _TAG_TFTPS_LIST_NODE
{
	struct _TAG_TFTPS_LIST_NODE*    next;		// next node in list
                                                // makes it valid SGL_LIST_NODE node
    TCPIP_TFTPS_EVENT_HANDLER       handler;    // handler to be called for event
    const void*                     hParam;     // handler parameter
    TCPIP_NET_HANDLE                hNet;       // interface that's registered for
                                                // 0 if all    
}TCPIP_TFTPS_LIST_NODE;


// option flag
/*  1) timeout acknowledged  if the server acknowledges the timeout requested 
    2)  blksize acknowledged  if the server acknowledges the blksize requested 
    3) tsize acknowledged  if the server acknowledges the tsize requested */
typedef union
{
    struct
    {
        unsigned char timeout_ack : 1; // acknowledges the timeout requested
        unsigned char blksize_ack : 1;   // acknowledges the blksize requested
        unsigned char tsize_ack : 1;  // acknowledges the tsize requested
       
    } bits;
    uint8_t Val;
} _tftpOptionFlags;

/* RFC 2348, 2349 options */
typedef struct  tftp_options 
{
    uint32_t  tsize;   /* If sent as 0 on a RRQ, server returns the 
                        * size of the file being requested - if value specified 
                        * on a WRQ, server accepts/declines the size of 
                        * the file to be transmitted */

    uint16_t  timeout;   /* Specify the server's timeout value */

    uint16_t  blksize;      /* Specify the size of the data block to be transmitted at a time */

    _tftpOptionFlags tftpoptionAckflag;   /* TFTP server option acknowledge*/
} TFTP_OPTIONS;

typedef struct 
{
    IP_MULTI_ADDRESS            local_addr;             /* Local address for the client block */
    uint8_t*                    trans_buf;              /* Last packet sent. */
    uint32_t                    reply_timeout;          /* timeout for the server to wait for Client response */
    uint32_t                    lastTxPktSize;          /* Last TX packet size */
    SYS_FS_HANDLE               file_desc;              /* File descriptor */
    uint32_t                    callbackPos;            /* Position in the buffer. */
    TCPIP_UDP_SIGNAL_HANDLE     cSigHandlr;             /* UDP socket RX signal handler */
    TFTP_OPTIONS                options;                /* Options requested of the server */
    uint8_t                     status;                 /* Status of communication. */
    uint8_t                     type;                   /* Read or write request */
    uint8_t                     retransmits;            /* Retransmit count for per Client */
    UDP_SOCKET                  cSkt;                   /* Handle to TFTP Client socket */ 
    uint16_t                    tid;                    /* Server's Transfer ID */
    uint16_t                    block_number;           /* Block number of expected packet. */
    uint8_t                     file_name[TCPIP_TFTPS_FILENAME_LEN]; /*File Name */
    uint8_t                     mode[TCPIP_TFTP_OCTET_SIZE]; /* File Transfer Mode Requested by Client. */
    uint8_t                     errCode;                /* client error code */
    uint8_t                     smState;                /* TFTP Server state machine variable */
    SYS_FS_SHELL_OBJ            *tftps_shell_obj;                 /* TFTP server wrapper object */
} TFTPS_CB;

#endif  // __TFTPS_PRIVATE_H_


