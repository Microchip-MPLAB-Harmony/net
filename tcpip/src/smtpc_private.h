/*******************************************************************************
  SMTPC Module private definitions for the Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    smtpc_private.h

  Summary:
    SMTPC manager private file

  Description:
    This source file contains the stack internal SMTPC definitions
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2016-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef H_SMTPC_PRIVATE_H_
#define H_SMTPC_PRIVATE_H_

// definitions
//



// debugging functionality
#define TCPIP_SMTPC_DEBUG_MASK_BASIC           (0x0001)     // basic debugging: assert or condition
#define TCPIP_SMTPC_DEBUG_MASK_STATE           (0x0002)     // displays SMTPC descriptors state change
#define TCPIP_SMTPC_DEBUG_MASK_ERROR_STATE     (0x0004)     // displays SMTPC descriptors entry in error state
#define TCPIP_SMTPC_DEBUG_MASK_CMD_TO_SRV      (0x0008)     // displays commands sent to the server
#define TCPIP_SMTPC_DEBUG_MASK_REPLY_FROM_SRV  (0x0010)     // displays locally the replies from the server


// enable SMTPC debugging levels
#define TCPIP_SMTPC_DEBUG_LEVEL                 (0x1)

// extra guard space needed for the plain line buffer size
// extra 4 bytes are needed: . at beg, crLF + 0 at end
#define SMTPC_PLAIN_LINE_BUFF_SIZE_GUARD       4U
// space needed in the socket buffer for the plain line buffer processing, in %
// Chosen so that a 256 size line + extra 4 bytes 
// will fit into a 512 bytes socket buffer
#define SMTPC_PLAIN_LINE_BUFF_SIZE_RATIO      196U


// Size of buffer for processing BASE64 bodies
// Always (TCPIP_SMTP_BASE64_BUFF_SIZE * 4/3 * 1.8) < socket TX size!
// the 1.8 factor is NET_PRES empyrical thresholds!
#define TCPIP_SMTP_BASE64_BUFF_SIZE     200U
// minimum space to be available in the socket
// if no such space, don't even bother
// always < TCPIP_SMTP_BASE64_BUFF_SIZE!
#define TCPIP_SMTP_BASE64_MIN_SKT_SIZE  100U


// minimum number of retries
// allow for 0 retries
#define TCPIP_SMTPC_MIN_RETRIES             0U 

// minimum server timeout, seconds
#define TCPIP_SMTPC_MIN_SERVER_TMO            10U

// minimum server retry interval, seconds
#define TCPIP_SMTPC_MIN_SERVER_RETRY_TMO       60U 

// minimum internal retry interval, seconds
#define TCPIP_SMTPC_MIN_INTERNAL_RETRY_TMO     10U 


// server reply codes

#define TCPIP_SMTP_CONNECT_SUCCESS_CODE             220
#define TCPIP_SMTP_EHLO_SUCCESS_CODE                250
#define TCPIP_SMTP_STARTTLS_SUCCESS_CODE            220
#define TCPIP_SMTP_AUTH_LOGIN_USER_SUCCESS_CODE     334
#define TCPIP_SMTP_AUTH_FINAL_SUCCESS_CODE          235
#define TCPIP_SMTP_MAIL_FROM_SUCCESS_CODE           250
#define TCPIP_SMTP_MAIL_RCPT_SUCCESS_CODE           250
#define TCPIP_SMTP_MAIL_RCPT_SUCCESS_ALT            251
#define TCPIP_SMTP_MAIL_DATA_SUCCESS_CODE           354
#define TCPIP_SMTP_MAIL_SENT_SUCCESS_CODE           250


// type of SMTP command issued
typedef enum
{
    TCPIP_SMTPC_CMD_NONE,  // no command issued  
    TCPIP_SMTPC_CMD_EHLO,
    TCPIP_SMTPC_CMD_HELO,
    TCPIP_SMTPC_CMD_STARTTLS,
    TCPIP_SMTPC_CMD_AUTH_LOGIN,
    TCPIP_SMTPC_CMD_AUTH_PLAIN,
    TCPIP_SMTPC_CMD_MAIL_FROM,
    TCPIP_SMTPC_CMD_MAIL_RCPT,
    TCPIP_SMTPC_CMD_MAIL_DATA,
    // additional commands
    TCPIP_SMTPC_CMD_RESET, 
    TCPIP_SMTPC_CMD_QUIT, 
}TCPIP_SMTPC_CMD_TYPE;

// values for the SMTP status
#define     SMTPC_STAT_WAIT_RETRY               0U      // waiting for a retry timeout to  pass

#define     SMTPC_STAT_DNS_START                1U      // start DNS resolution  
#define     SMTPC_STAT_DNS_WAIT                 2U      // wait DNS resolution 
#define     SMTPC_STAT_SOCKET_GET               3U      // get communication channel 
#define     SMTPC_STAT_SKT_WAIT_CONNECT         4U      // wait server to respond 
#define     SMTPC_STAT_SERVER_CONNECTED         5U      // from now on the server was connected
// server helo, tls and authentication
#define     SMTPC_STAT_EHLO_PLAIN    SMTPC_STAT_SERVER_CONNECTED // send ehlo in plain mode
#define     SMTPC_STAT_EHLO_PLAIN_WAIT          6U      // wait EHLO reply
#define     SMTPC_STAT_TLS_START                7U      // start TLS command
#define     SMTPC_STAT_TLS_WAIT                 8U      // wait STARTTLS server reply
#define     SMTPC_STAT_TLS_WAIT_SECURE          9U      // wait TLS handshake to be done
#define     SMTPC_STAT_EHLO_SECURE              10U     // send ehlo in encrypted/secure mode
#define     SMTPC_STAT_EHLO_SECURE_WAIT         11U     // wait ehlo reply
#define     SMTPC_STAT_AUTH                     12U     // authentication phase
#define     SMTPC_STAT_AUTH_WAIT_LOGIN_USER     13U     // Login authentication wait for server user request
#define     SMTPC_STAT_AUTH_LOGIN_USER          14U     // Send Login user
#define     SMTPC_STAT_AUTH_WAIT_LOGIN_PASS     15U     // Login authentication wait for server pass request
#define     SMTPC_STAT_AUTH_LOGIN_PASS          16U     // Send Login password
#define     SMTPC_STAT_AUTH_WAIT_FINAL          17U     // wait final authentication response
// mail start
#define     SMTPC_STAT_MAIL_START               18U     // mail start
#define     SMTPC_STAT_MAIL_ENVL_FROM   SMTPC_STAT_MAIL_START // send mail envelope: from
#define     SMTPC_STAT_MAIL_ENVL_FROM_WAIT      19U     // wait mail from response
#define     SMTPC_STAT_MAIL_ENVL_RCPT           20U     // send mail envelope: rcpt
#define     SMTPC_STAT_MAIL_ENVL_RCPT_WAIT      21U     // wait mail rcpt response
#define     SMTPC_STAT_MAIL_ENVL_DATA           22U     // send mail envelope: data
#define     SMTPC_STAT_MAIL_ENVL_DATA_WAIT      23U     // wait mail data response
// mail body 
#define     SMTPC_STAT_MAIL_MSG_START           24U     // mail message body starts; no more answers from the server
#define     SMTPC_STAT_MAIL_MSG_HDR_DATE    SMTPC_STAT_MAIL_MSG_START // send mail header: date
#define     SMTPC_STAT_MAIL_MSG_HDR_FROM        25U     // send mail header: from
#define     SMTPC_STAT_MAIL_MSG_HDR_TO          26U     // send mail header: to
#define     SMTPC_STAT_MAIL_MSG_HDR_SENDER      27U     // send mail header: sender
#define     SMTPC_STAT_MAIL_MSG_HDR_SUBJECT     28U     // send mail header: subject
#define     SMTPC_STAT_MAIL_MSG_HDR_MSG_ID      29U     // send mail header:  message ID
#define     SMTPC_STAT_MAIL_MSG_HDR_MIME_HDR    30U     // send mail header: mime header
#define     SMTPC_STAT_MAIL_MSG_HDR_PREAMBLE    31U     // send mail preamble header
#define     SMTPC_STAT_MAIL_MSG_BODY_HDR        32U     // send mail body header
#define     SMTPC_STAT_MAIL_MSG_BODY            33U     // send mail body
#define     SMTPC_STAT_MAIL_MSG_BUFF_HDR        34U     // send mail buffer header
#define     SMTPC_STAT_MAIL_MSG_BUFF_BODY       35U     // send mail buffer body
#define     SMTPC_STAT_MAIL_MSG_FILE_HDR        36U     // send mail file header
#define     SMTPC_STAT_MAIL_MSG_FILE_BODY       37U     // send mail file body
#define     SMTPC_STAT_MAIL_DONE                38U     // send mail stop/end
#define     SMTPC_STAT_MAIL_DONE_WAIT           39U     // wait mail confirmation from the server
#define     SMTPC_STAT_MAIL_DONE_REPORT         40U     // done; report result to the user
// after mail done
#define     SMTPC_STAT_TRANSACTION_RESET        41U     // reset server transaction if smth went wrong
#define     SMTPC_STAT_TRANSACTION_RESET_WAIT   42U     // wait server reply
#define     SMTPC_STAT_TRANSACTION_QUIT         43U     // quit server transaction
#define     SMTPC_STAT_TRANSACTION_QUIT_WAIT    44U     // wait server reply
#define     SMTPC_STAT_TRANSACTION_CLOSE        45U     // done with this transaction, close everything
// done
#define     SMTPC_STAT_FREED                    46U     // this descriptor/transaction is no longer busy, needs to be freed

typedef unsigned int TCPIP_SMTPC_STATUS;     // one of the SMTPC_STAT_ values

// type of supported mail envelopes
typedef enum
{
    TCPIP_SMTPC_ENV_FROM,                   // from
    TCPIP_SMTPC_ENV_RCPT,                   // recipient
    TCPIP_SMTPC_ENV_DATA,                   // data

}TCPIP_SMTPC_ENVELOPE;

// server supported flags: TLS, authentication, etc.
// 16 bit values only
typedef enum
{
    TCPIP_SMTPC_DCPT_FLAG_NONE          = 0x0000,       // no flag set
    TCPIP_SMTPC_DCPT_FLAG_DNS           = 0x0001,       // DNS needed to get the server address
    TCPIP_SMTPC_DCPT_FLAG_CONNECTED     = 0x0002,       // socket connected to the server
    TCPIP_SMTPC_DCPT_FLAG_EHLO          = 0x0004,       // exchanged helo with the server
    TCPIP_SMTPC_DCPT_FLAG_SECURED       = 0x0008,       // connection is encrypted, TLS is on
    TCPIP_SMTPC_DCPT_FLAG_RETRY_NONE    = 0x0000,       // no retries
    TCPIP_SMTPC_DCPT_FLAG_RETRY_SERVER  = 0x0010,       // needs to be retried, server transient error
    TCPIP_SMTPC_DCPT_FLAG_RETRY_SMPTC   = 0x0020,       // needs to be retried, SMPTC transient error

    // other flags here

    // server related flags
    TCPIP_SMTPC_SERVER_FLAG_TLS         = 0x0100,       // server supports TLS encryption
    TCPIP_SMTPC_SERVER_FLAG_AUTH        = 0x0200,       // server supports authentication

    // other flags here


    // authentication types are embedded in here
    TCPIP_SMTPC_SERVER_FLAG_AUTH_LOGIN  = 0x1000,       // login authentication
    TCPIP_SMTPC_SERVER_FLAG_AUTH_PLAIN  = 0x2000,       // plain authentication
    TCPIP_SMTPC_SERVER_FLAG_AUTH_MD5    = 0x4000,       // CRAM-MD5 authentication

    // supported authentications mask
    TCPIP_SMTPC_SERVER_FLAG_AUTH_SUPPORTED_MASK = ( TCPIP_SMTPC_SERVER_FLAG_AUTH_LOGIN | TCPIP_SMTPC_SERVER_FLAG_AUTH_PLAIN),
    // authentication fields mask
    TCPIP_SMTPC_SERVER_FLAG_AUTH_MASK = ( TCPIP_SMTPC_SERVER_FLAG_AUTH | TCPIP_SMTPC_SERVER_FLAG_AUTH_LOGIN | TCPIP_SMTPC_SERVER_FLAG_AUTH_PLAIN | TCPIP_SMTPC_SERVER_FLAG_AUTH_MD5),

    // connection fields mask
    TCPIP_SMTPC_SERVER_FLAG_CONN_MASK   = (TCPIP_SMTPC_DCPT_FLAG_CONNECTED | TCPIP_SMTPC_DCPT_FLAG_EHLO | TCPIP_SMTPC_SERVER_FLAG_TLS | TCPIP_SMTPC_DCPT_FLAG_SECURED),

    // retry flags
    TCPIP_SMTPC_DCPT_FLAG_RETRY_MASK    = (TCPIP_SMTPC_DCPT_FLAG_RETRY_SERVER | TCPIP_SMTPC_DCPT_FLAG_RETRY_SMPTC),

}TCPIP_SMTPC_DCPT_FLAGS;

// forward reference
struct S_tag_TCPIP_SMTP_READ_DCPT;

// structure for allowing reads from buffers or files to be done in the same way
typedef struct 
{
    // initialization function;
    // name is used for files
    // buffer and buffSize for buffers 
    // returns true for success
    // it should allow multiple init ops with the same buffer/file!
    bool        (*fInit)(struct S_tag_TCPIP_SMTP_READ_DCPT* pReadDcpt, const char* name, const uint8_t* buffer, size_t buffSize);
    // deinitialization function;
    void        (*fDeinit)(struct S_tag_TCPIP_SMTP_READ_DCPT* pReadDcpt);
    // reads data into pBuffer;
    // returns -1 for eof; 
    // returns >= 0  for number of bytes read
    int         (*fRead)(struct S_tag_TCPIP_SMTP_READ_DCPT* pReadDcpt, uint8_t* pBuffer, size_t nBytes);
    // pushes back to the stream the number of bytes
    int         (*fPush)(struct S_tag_TCPIP_SMTP_READ_DCPT* pReadDcpt, size_t nBytes);
}TCPIP_SMTP_READ_OBJ;

// read descriptor for a buffer
typedef struct
{
    // buffer specific stuff
    const uint8_t*              readBuffer;     // buffer to be rendered
    size_t                      readSize;       // total size to be rendered;
    size_t                      readOffset;     // current offset;
}TCPIP_SMTP_READ_DCPT_BUFFER;

// file manipulation 
// read descriptor for a file
typedef struct
{
    // file specific stuff
    const char*                 fName;          // file to be rendered
    SYS_FS_HANDLE               fHandle;        // associated file handle
}TCPIP_SMTP_READ_DCPT_FILE;

// read descriptor encapsulating a read object
typedef struct S_tag_TCPIP_SMTP_READ_DCPT
{
    const TCPIP_SMTP_READ_OBJ*  pReadObj;       // generic
    union
    {
        TCPIP_SMTP_READ_DCPT_BUFFER bDcpt;
        TCPIP_SMTP_READ_DCPT_FILE   fDcpt;
    };
}TCPIP_SMTP_READ_DCPT;


// structure describing a client mail message operation
typedef struct S_tag_SMTPC_MESSAGE_DCPT
{
    struct S_tag_SMTPC_MESSAGE_DCPT*     next;       // safe cast to single list
    uint16_t                            dcptIx;     // descriptor identifier
    NET_PRES_SKT_HANDLE_T               skt;        // comm socket
    union
    {
        IP_MULTI_ADDRESS                serverAdd;
        NET_PRES_ADDRESS                presServerAdd;
    };
    IP_ADDRESS_TYPE                     addType;
    TCPIP_SMTPC_MAIL_MESSAGE            smtpMailMessage;    // current message to be sent 
    TCPIP_SMTPC_STATUS                  currStat;           // current descriptor status
#if ((TCPIP_SMTPC_DEBUG_LEVEL & TCPIP_SMTPC_DEBUG_MASK_STATE) != 0)
    TCPIP_SMTPC_STATUS                  prevStat;           // previous descriptor status
#endif  // ((TCPIP_SMTPC_DEBUG_LEVEL & TCPIP_SMTPC_DEBUG_MASK_STATE) != 0)
    TCPIP_SMTPC_STATUS                  errorStat;      // descriptor status when error was encountered
    TCPIP_SMTPC_STATUS                  errorJumpStat;  // state to jump when error is triggered
    TCPIP_SMTPC_MESSAGE_RESULT          messageRes;     // result of this message
    TCPIP_SMTPC_MESSAGE_WARNING         messageWarn;    // occurred warnings
    uint32_t                            waitTick;       // if a reply is not received, trigger a timeout
    TCPIP_SMTP_READ_DCPT                smtpReadDcpt;
    const TCPIP_SMTPC_ATTACH_BUFFER*    smtpAttachBuffer;       // current attach buffer to be sent
    const TCPIP_SMTPC_ATTACH_FILE*      smtpAttachFile;         // current file attachment to be sent
    uint16_t                            smtpPartAttachType;     // TCPIP_SMTPC_ATTACH_TYPE: type of the part attachment to be transmitted
    uint16_t                            smtpPartEncodeType;     // TCPIP_SMTPC_ENCODE_TYPE: encoding for the part to be transmitted
    uint16_t                            dcptFlags;      // descriptor flags: TCPIP_SMTPC_DCPT_FLAGS value
    uint16_t                            nSmtpAttachBuffers;     // how many attached buffers
    uint16_t                            nSmtpAttachFiles;       // how many attached files
    uint16_t                            retryCnt;               // current retry counter

}TCPIP_SMTPC_MESSAGE_DCPT;


#endif  // H_SMTPC_PRIVATE_H_

