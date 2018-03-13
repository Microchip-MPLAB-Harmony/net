/*******************************************************************************
  Simple Mail Transfer Protocol (SMTP) Client
  Module for Microchip TCP/IP Stack

  Summary:
    Implementation of SMTP Client protocol
    
  Description:
    - SMTP client provides ability to send user emails.
    - Reference: RFC 5321, 5322, 6152, 6409, 6854, 2034, 3207, 4616, 4648, 4954, 5248,
                 RFC 2045, 2046,2047, 4289, 2049, 2183, 2387, 2557, 6552, 6838

*******************************************************************************/

/*******************************************************************************
File Name:  smtpc.c
Copyright © 2016 released Microchip Technology Inc.  All rights
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
#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_SMTPC

#include "stdlib.h"
#include "ctype.h"

#include "tcpip/src/tcpip_private.h"
#include "net/pres/net_pres_socketapi.h"

#include "tcpip/src/common/sys_fs_wrapper.h"

#if defined(TCPIP_STACK_USE_SMTPC)

#include "tcpip/src/smtpc_private.h"

/****************************************************************************
  Section:
	SMTPC definitions
  ***************************************************************************/

/****************************************************************************
  Section:
	SMTPC Internal Variables
  ***************************************************************************/

static int          smtpcInitCount = 0;       // module initialization count

static uint32_t     smtpcMailCounter;         // sent mail counter; should be unique!

static const char*  smtpcMailBoundary = "bWNocEVtYWlsQ2xpZW50::djEuMA==";   // BASE64 of mchpEmailClient::v1.0
                                                                            // Should be <= 70 chars!
static uint16_t     smtpcSktTxBuffSize = 0;      // size of TX buffer for the associated socket
static uint16_t     smtpcSktRxBuffSize = 0;      // size of RX buffer for the associated socket

static int          smtpcMailRetries;            // number of retries for a mail message
static int          smtpcServerRetryTmo;         // retry interval, seconds
static int          smtpcInternalRetryTmo;       // internal retry interval, seconds

static TCPIP_SMTPC_MESSAGE_DCPT* smtpcAllocPool = 0;    // allocated pool of descriptors
                                                        // also the global SMTPC alive ptr
static SINGLE_LIST  smtpcMessageFreeList = { 0 };       // pool of free descriptors to be used for mail messages
static SINGLE_LIST  smtpcMessageBusyList = { 0 };       // list of messages scheduled to be sent

static tcpipSignalHandle    smtpcSignalHandle = 0;

static uint32_t     smtpcServerGenTmo;                 // general purpose mail server timeout
static uint32_t     smtpcServerDataTmo;                // mail server timeout to acknowledge the mail data
static uint32_t     smtpcTlsHandshakeTmo;              // TLS handshake timeout


// status functions
// processes a current state and returns a new state
typedef TCPIP_SMTPC_STATUS(*TCPIP_SMTPC_STATUS_FNC)(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);

static TCPIP_SMTPC_STATUS smtpDcptStateWaitRetry(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptStateStartDns(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptStateWaitDns(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptStateGetSkt(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptStateSendEhlo(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptStateWaitReply(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptStateStartTls(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptStateWaitTlsSecure(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptStateAuth(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptStateAuthLoginUser(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptStateAuthLoginPass(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptStateMailFrom(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptStateMailRcpt(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptMailDoneReport(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptStateMailData(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);

static TCPIP_SMTPC_STATUS smtpDcptMailMsgDate(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptMailMsgFrom(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptMailMsgTo(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptMailMsgSender(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptMailMsgSubject(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptMailMsgMessageId(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptMailMsgMimeHdr(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptMailMsgPreambleHdr(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptMailMsgBodyHdr(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptMailMsgBody(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptMailMsgBuffHdr(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptMailMsgBuffBody(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptMailMsgFileHdr(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptMailMsgFileBody(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptMailMsgDone(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptTransactionReset(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptTransactionQuit(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);
static TCPIP_SMTPC_STATUS smtpDcptTransactionClose(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);

static const TCPIP_SMTPC_STATUS_FNC smtpDcptStateTbl[] = 
{
    smtpDcptStateWaitRetry,         // TCPIP_SMTPC_STAT_WAIT_RETRY
    smtpDcptStateStartDns,          // TCPIP_SMTPC_STAT_DNS_START
    smtpDcptStateWaitDns,           // TCPIP_SMTPC_STAT_DNS_WAIT
    smtpDcptStateGetSkt,            // TCPIP_SMTPC_STAT_SOCKET_GET
    smtpDcptStateWaitReply,         // TCPIP_SMTPC_STAT_SKT_WAIT_CONNECT
    // TCPIP_SMTPC_STAT_SERVER_CONNECTED
    smtpDcptStateSendEhlo,          // TCPIP_SMTPC_STAT_EHLO_PLAIN
    smtpDcptStateWaitReply,         // TCPIP_SMTPC_STAT_EHLO_PLAIN_WAIT
    smtpDcptStateStartTls,          // TCPIP_SMTPC_STAT_TLS_START
    smtpDcptStateWaitReply,         // TCPIP_SMTPC_STAT_TLS_WAIT
    smtpDcptStateWaitTlsSecure,     // TCPIP_SMTPC_STAT_TLS_WAIT_SECURE
    smtpDcptStateSendEhlo,          // TCPIP_SMTPC_STAT_EHLO_SECURE
    smtpDcptStateWaitReply,         // TCPIP_SMTPC_STAT_EHLO_SECURE_WAIT
    smtpDcptStateAuth,              // TCPIP_SMTPC_STAT_AUTH
    smtpDcptStateWaitReply,         // TCPIP_SMTPC_STAT_AUTH_WAIT_LOGIN_USER
    smtpDcptStateAuthLoginUser,     // TCPIP_SMTPC_STAT_AUTH_LOGIN_USER
    smtpDcptStateWaitReply,         // TCPIP_SMTPC_STAT_AUTH_WAIT_LOGIN_PASS
    smtpDcptStateAuthLoginPass,     // TCPIP_SMTPC_STAT_AUTH_LOGIN_PASS
    smtpDcptStateWaitReply,         // TCPIP_SMTPC_STAT_AUTH_WAIT_FINAL
    // TCPIP_SMTPC_STAT_MAIL_START
    smtpDcptStateMailFrom,          // TCPIP_SMTPC_STAT_MAIL_ENVL_FROM
    smtpDcptStateWaitReply,         // TCPIP_SMTPC_STAT_MAIL_ENVL_FROM_WAIT
    smtpDcptStateMailRcpt,          // TCPIP_SMTPC_STAT_MAIL_ENVL_RCPT
    smtpDcptStateWaitReply,         // TCPIP_SMTPC_STAT_MAIL_ENVL_RCPT_WAIT
    smtpDcptStateMailData,          // TCPIP_SMTPC_STAT_MAIL_ENVL_DATA
    smtpDcptStateWaitReply,         // TCPIP_SMTPC_STAT_MAIL_ENVL_DATA_WAIT
    // TCPIP_SMTPC_STAT_MAIL_MSG_START
    smtpDcptMailMsgDate,            // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_DATE
    smtpDcptMailMsgFrom,            // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_FROM
    smtpDcptMailMsgTo,              // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_TO
    smtpDcptMailMsgSender,          // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_SENDER
    smtpDcptMailMsgSubject,         // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_SUBJECT
    smtpDcptMailMsgMessageId,       // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_MSG_ID
    smtpDcptMailMsgMimeHdr,         // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_MIME_HDR
    smtpDcptMailMsgPreambleHdr,     // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_PREAMBLE
    smtpDcptMailMsgBodyHdr,         // TCPIP_SMTPC_STAT_MAIL_MSG_BODY_HDR
    smtpDcptMailMsgBody,            // TCPIP_SMTPC_STAT_MAIL_MSG_BODY
    smtpDcptMailMsgBuffHdr,         // TCPIP_SMTPC_STAT_MAIL_MSG_BUFF_HDR
    smtpDcptMailMsgBuffBody,        // TCPIP_SMTPC_STAT_MAIL_MSG_BUFF_BODY
    smtpDcptMailMsgFileHdr,         // TCPIP_SMTPC_STAT_MAIL_MSG_FILE_HDR
    smtpDcptMailMsgFileBody,        // TCPIP_SMTPC_STAT_MAIL_MSG_FILE_BODY
    smtpDcptMailMsgDone,            // TCPIP_SMTPC_STAT_MAIL_DONE
    smtpDcptStateWaitReply,         // TCPIP_SMTPC_STAT_MAIL_DONE_WAIT
    smtpDcptMailDoneReport,         // TCPIP_SMTPC_STAT_MAIL_DONE_REPORT
    // after mail done
    smtpDcptTransactionReset,       // TCPIP_SMTPC_STAT_TRANSACTION_RESET
    smtpDcptStateWaitReply,         // TCPIP_SMTPC_STAT_TRANSACTION_RESET_WAIT
    smtpDcptTransactionQuit,        // TCPIP_SMTPC_STAT_TRANSACTION_QUIT
    smtpDcptStateWaitReply,         // TCPIP_SMTPC_STAT_TRANSACTION_QUIT_WAIT
    smtpDcptTransactionClose,       // TCPIP_SMTPC_STAT_TRANSACTION_CLOSE

    
};

// RX buffer processing
static char         smtpcRxBuffer[TCPIP_SMTPC_SERVER_REPLY_BUFFER_SIZE + 1];         // global receive buffer

// function processing SMTP server messages
// should receive only full lines for processing...
// the rx buffer is still used so it won't be changed!
typedef TCPIP_SMTPC_STATUS(*SMTP_DCPT_PROCESS_FNC)(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines);

static TCPIP_SMTPC_STATUS smtpDcptRxProcNone(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines);
static TCPIP_SMTPC_STATUS smtpDcptRxProcConnect(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines);
static TCPIP_SMTPC_STATUS smtpDcptRxProcEhlo(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines);
static TCPIP_SMTPC_STATUS smtpDcptRxProcAuthFinal(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines);
static TCPIP_SMTPC_STATUS smtpDcptRxProcTls(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines);
static TCPIP_SMTPC_STATUS smtpDcptRxProcAuthLoginUser(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines);
static TCPIP_SMTPC_STATUS smtpDcptRxProcAuthLoginPass(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines);
static TCPIP_SMTPC_STATUS smtpDcptRxProcMailFrom(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines);
static TCPIP_SMTPC_STATUS smtpDcptRxProcMailRcpt(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines);
static TCPIP_SMTPC_STATUS smtpDcptRxProcMailData(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines);
static TCPIP_SMTPC_STATUS smtpDcptRxProcMailDoneWait(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines);
static TCPIP_SMTPC_STATUS smtpDcptRxProcTransactResetWait(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines);
static TCPIP_SMTPC_STATUS smtpDcptRxProcTransactQuitWait(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines);

static const SMTP_DCPT_PROCESS_FNC smtpRxProcessTbl[] =
{
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_WAIT_RETRY
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_DNS_START
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_DNS_WAIT
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_SOCKET_GET
    smtpDcptRxProcConnect,          // TCPIP_SMTPC_STAT_SKT_WAIT_CONNECT
    // TCPIP_SMTPC_STAT_SERVER_CONNECTED
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_EHLO_PLAIN
    smtpDcptRxProcEhlo,             // TCPIP_SMTPC_STAT_EHLO_PLAIN_WAIT
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_TLS_START
    smtpDcptRxProcTls,              // TCPIP_SMTPC_STAT_TLS_WAIT
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_TLS_WAIT_SECURE
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_EHLO_SECURE
    smtpDcptRxProcEhlo,             // TCPIP_SMTPC_STAT_EHLO_SECURE_WAIT
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_AUTH
    smtpDcptRxProcAuthLoginUser,    // TCPIP_SMTPC_STAT_AUTH_WAIT_LOGIN_USER
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_AUTH_LOGIN_USER
    smtpDcptRxProcAuthLoginPass,    // TCPIP_SMTPC_STAT_AUTH_WAIT_LOGIN_PASS
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_AUTH_LOGIN_PASS
    smtpDcptRxProcAuthFinal,        // TCPIP_SMTPC_STAT_AUTH_WAIT_FINAL
    // TCPIP_SMTPC_STAT_MAIL_START
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_MAIL_ENVL_FROM
    smtpDcptRxProcMailFrom,         // TCPIP_SMTPC_STAT_MAIL_ENVL_FROM_WAIT
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_MAIL_ENVL_RCPT
    smtpDcptRxProcMailRcpt,         // TCPIP_SMTPC_STAT_MAIL_ENVL_RCPT_WAIT
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_MAIL_ENVL_DATA
    smtpDcptRxProcMailData,         // TCPIP_SMTPC_STAT_MAIL_ENVL_DATA_WAIT
    // TCPIP_SMTPC_STAT_MAIL_MSG_START
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_DATE
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_FROM
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_TO
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_SENDER
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_SUBJECT
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_MSG_ID
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_MIME_HDR
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_PREAMBLE
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_MAIL_MSG_BODY_HDR
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_MAIL_MSG_BODY
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_MAIL_MSG_BUFF_HDR
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_MAIL_MSG_BUFF_BODY
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_MAIL_MSG_FILE_HDR
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_MAIL_MSG_FILE_BODY
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_MAIL_DONE
    smtpDcptRxProcMailDoneWait,     // TCPIP_SMTPC_STAT_MAIL_DONE_WAIT
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_MAIL_DONE_REPORT
    // after mail done
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_TRANSACTION_RESET
    smtpDcptRxProcTransactResetWait,// TCPIP_SMTPC_STAT_TRANSACTION_RESET_WAIT
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_TRANSACTION_QUIT
    smtpDcptRxProcTransactQuitWait, // TCPIP_SMTPC_STAT_TRANSACTION_QUIT_WAIT
    smtpDcptRxProcNone,             // TCPIP_SMTPC_STAT_TRANSACTION_CLOSE
};

// buffer manipulation 
static bool     smtpBufferInit(TCPIP_SMTP_READ_DCPT* pRdDcpt, const char* name, const uint8_t* buffer, size_t buffSize);
static void     smtpBufferDeinit(TCPIP_SMTP_READ_DCPT* pRdDcpt);
static int      smtpBufferRead(TCPIP_SMTP_READ_DCPT* pRdDcpt, uint8_t* pBuffer, size_t nBytes);
static int      smtpBufferPush(TCPIP_SMTP_READ_DCPT* pRdDcpt, size_t nBytes);

static const TCPIP_SMTP_READ_OBJ smtpReadBufferObj = 
{
    smtpBufferInit, smtpBufferDeinit, smtpBufferRead, smtpBufferPush
};

// file manipulation 
static bool     smtpFileInit(TCPIP_SMTP_READ_DCPT* pRdDcpt, const char* name, const uint8_t* buffer, size_t buffSize);
static void     smtpFileDeinit(TCPIP_SMTP_READ_DCPT* pRdDcpt);
static int      smtpFileRead(TCPIP_SMTP_READ_DCPT* pRdDcpt, uint8_t* pBuffer, size_t nBytes);
static int      smtpFilePush(TCPIP_SMTP_READ_DCPT* pRdDcpt, size_t nBytes);

static const TCPIP_SMTP_READ_OBJ smtpReadFileObj = 
{
    smtpFileInit, smtpFileDeinit, smtpFileRead, smtpFilePush
};

// command strings associated with SMTP commands
static const char* smtpCmdStrTbl[] = 
{
    "NONE",         // TCPIP_SMTPC_CMD_NONE
    "EHLO",         // TCPIP_SMTPC_CMD_EHLO
    "HELO",         // TCPIP_SMTPC_CMD_HELO
    "STARTTLS",     // TCPIP_SMTPC_CMD_STARTTLS
    "AUTH LOGIN",   // TCPIP_SMTPC_CMD_AUTH_LOGIN
    "AUTH PLAIN",   // TCPIP_SMTPC_CMD_AUTH_PLAIN
    "MAIL FROM:",   // TCPIP_SMTPC_CMD_MAIL_FROM
    "RCPT TO:",     // TCPIP_SMTPC_CMD_MAIL_RCPT
    "DATA",         // TCPIP_SMTPC_CMD_MAIL_DATA
    // additional commands
    "RSET",         // TCPIP_SMTPC_CMD_RESET
    "QUIT",         // TCPIP_SMTPC_CMD_QUIT
};

// conversion from an internal to an external state table
static const uint8_t smtpStatusTxlateTbl[] = 
{
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_WAIT_RETRY,        // TCPIP_SMTPC_STAT_WAIT_RETRY
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_DNS,               // TCPIP_SMTPC_STAT_DNS_START
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_DNS,               // TCPIP_SMTPC_STAT_DNS_WAIT
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_CONNECT,           // TCPIP_SMTPC_STAT_SOCKET_GET
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_CONNECT,           // TCPIP_SMTPC_STAT_SKT_WAIT_CONNECT
    // TCPIP_SMTPC_STAT_SERVER_CONNECTED
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_HELLO,             // TCPIP_SMTPC_STAT_EHLO_PLAIN
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_HELLO,             // TCPIP_SMTPC_STAT_EHLO_PLAIN_WAIT
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_TLS,               // TCPIP_SMTPC_STAT_TLS_START
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_TLS,               // TCPIP_SMTPC_STAT_TLS_WAIT
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_TLS,               // TCPIP_SMTPC_STAT_TLS_WAIT_SECURE
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_HELLO,             // TCPIP_SMTPC_STAT_EHLO_SECURE
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_HELLO,             // TCPIP_SMTPC_STAT_EHLO_SECURE_WAIT
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_AUTH,              // TCPIP_SMTPC_STAT_AUTH
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_AUTH,              // TCPIP_SMTPC_STAT_AUTH_WAIT_LOGIN_USER
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_AUTH,              // TCPIP_SMTPC_STAT_AUTH_LOGIN_USER
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_AUTH,              // TCPIP_SMTPC_STAT_AUTH_WAIT_LOGIN_PASS
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_AUTH,              // TCPIP_SMTPC_STAT_AUTH_LOGIN_PASS
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_AUTH,              // TCPIP_SMTPC_STAT_AUTH_WAIT_FINAL
    // TCPIP_SMTPC_STAT_MAIL_START
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_ENVELOPE,     // TCPIP_SMTPC_STAT_MAIL_ENVL_FROM
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_ENVELOPE,     // TCPIP_SMTPC_STAT_MAIL_ENVL_FROM_WAIT
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_ENVELOPE,     // TCPIP_SMTPC_STAT_MAIL_ENVL_RCPT
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_ENVELOPE,     // TCPIP_SMTPC_STAT_MAIL_ENVL_RCPT_WAIT
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_ENVELOPE,     // TCPIP_SMTPC_STAT_MAIL_ENVL_DATA
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_ENVELOPE,     // TCPIP_SMTPC_STAT_MAIL_ENVL_DATA_WAIT
    // TCPIP_SMTPC_STAT_MAIL_MSG_START
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_BODY,         // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_DATE
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_BODY,         // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_FROM
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_BODY,         // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_TO
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_BODY,         // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_SENDER
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_BODY,         // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_SUBJECT
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_BODY,         // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_MSG_ID
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_BODY,         // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_MIME_HDR
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_BODY,         // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_PREAMBLE
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_BODY,         // TCPIP_SMTPC_STAT_MAIL_MSG_BODY_HDR
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_BODY,         // TCPIP_SMTPC_STAT_MAIL_MSG_BODY
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_BUFFERS,      // TCPIP_SMTPC_STAT_MAIL_MSG_BUFF_HDR
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_BUFFERS,      // TCPIP_SMTPC_STAT_MAIL_MSG_BUFF_BODY
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_FILES,        // TCPIP_SMTPC_STAT_MAIL_MSG_FILE_HDR
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_FILES,        // TCPIP_SMTPC_STAT_MAIL_MSG_FILE_BODY
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_ACKNOWLEDGE,  // TCPIP_SMTPC_STAT_MAIL_DONE
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_ACKNOWLEDGE,  // TCPIP_SMTPC_STAT_MAIL_DONE_WAIT
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_REPORT,       // TCPIP_SMTPC_STAT_MAIL_DONE_REPORT
    // after mail done
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_CLOSING,           // TCPIP_SMTPC_STAT_TRANSACTION_RESET
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_CLOSING,           // TCPIP_SMTPC_STAT_TRANSACTION_RESET_WAIT
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_CLOSING,           // TCPIP_SMTPC_STAT_TRANSACTION_QUIT
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_CLOSING,           // TCPIP_SMTPC_STAT_TRANSACTION_QUIT_WAIT
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_CLOSING,           // TCPIP_SMTPC_STAT_TRANSACTION_CLOSE
    // no longer
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_NONE,              // TCPIP_SMTPC_STAT_FREED
};


/****************************************************************************
  Section:
	SMTPC Internal Function Prototypes
  ***************************************************************************/

static void TCPIP_SMTPC_Process(void);

static void _SMTPCSocketRxSignalHandler(NET_PRES_SKT_HANDLE_T hTCP, NET_PRES_SIGNAL_HANDLE hNet, uint16_t sigType, const void* param);

static TCPIP_SMTPC_STATUS smtpCommandParam(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, TCPIP_SMTPC_CMD_TYPE cmdType, const char* cmdArg, uint32_t serverTmo);

static TCPIP_SMTPC_STATUS smtpClientWriteCmd(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* cmdStr, const char* cmdArg);

static bool smtpSetEhloString(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, char* pAddBuff, size_t buffSize);

static void smtpcSetDcptTmo(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, uint32_t tmo);

static bool smtpcIsDcptTmo(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);

static TCPIP_SMTPC_STATUS smtpcErrorStop(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, TCPIP_SMTPC_MESSAGE_RESULT res, TCPIP_SMTPC_DCPT_FLAGS retryFlags);

static void smtpcRetryInit(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);

static TCPIP_SMTPC_STATUS smtpcServerErrorStop(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, TCPIP_SMTPC_MESSAGE_RESULT res, int serverCode);

static TCPIP_SMTPC_STATUS smtpAuthLogin(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);

static TCPIP_SMTPC_STATUS smtpAuthPlain(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);

static TCPIP_SMTPC_STATUS   smtpWriteMailEnvAddress(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, TCPIP_SMTPC_ENVELOPE envType);

static bool smtpFormatMailAddress(const char* mailAddr, char* mailAddrBuff, int buffSize);

static TCPIP_SMTPC_STATUS smtpSendPartHdr(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* fileName);

static TCPIP_SMTPC_STATUS smtpSendPartBody(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);

static TCPIP_SMTPC_STATUS smtpSendPartBodyPlain(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);

static TCPIP_SMTPC_STATUS smtpSendPartBodyBase64(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);

// debug functionality
#if ((TCPIP_SMTPC_DEBUG_LEVEL & TCPIP_SMTPC_DEBUG_MASK_BASIC) != 0)
volatile int _SMTPCStayAssertLoop = 0;
static bool _SMTPCAssertCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("SMTPC Assert: %s, in line: %d, \r\n", message, lineNo);
        while(_SMTPCStayAssertLoop != 0);
    }

    return cond;
}
// a debug condition, not really assertion
volatile int _SMTPCStayCondLoop = 0;
static bool _SMTPCDebugCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("SMTPC Cond: %s, in line: %d, \r\n", message, lineNo);
        while(_SMTPCStayCondLoop != 0);
    }
    return cond;
}

#else
#define _SMTPCAssertCond(cond, message, lineNo)  (1)
#define _SMTPCDebugCond(cond, message, lineNo)  (1)
#endif  // (TCPIP_SMTPC_DEBUG_LEVEL & TCPIP_SMTPC_DEBUG_MASK_BASIC)


#if (TCPIP_SMTPC_DEBUG_LEVEL & (TCPIP_SMTPC_DEBUG_MASK_STATE | TCPIP_SMTPC_DEBUG_MASK_ERROR_STATE) != 0)
static const char* const _SMTPCDbgDcptStateTbl[] = 
{
    "idle",                 // TCPIP_SMTPC_STAT_WAIT_RETRY
    "dns_s",                // TCPIP_SMTPC_STAT_DNS_START
    "dns_w",                // TCPIP_SMTPC_STAT_DNS_WAIT
    "skt_g",                // TCPIP_SMTPC_STAT_SOCKET_GET
    "skt_w",                // TCPIP_SMTPC_STAT_SKT_WAIT_CONNECT
    "ehlo_pl",              // TCPIP_SMTPC_STAT_EHLO_PLAIN
    "ehlo_pl_w",            // TCPIP_SMTPC_STAT_EHLO_PLAIN_WAIT
    "tls_s",                // TCPIP_SMTPC_STAT_TLS_START
    "tls_w",                // TCPIP_SMTPC_STAT_TLS_WAIT
    "tls_s_w",              // TCPIP_SMTPC_STAT_TLS_WAIT_SECURE
    "ehlo_s",               // TCPIP_SMTPC_STAT_EHLO_SECURE
    "ehlo_s_w",             // TCPIP_SMTPC_STAT_EHLO_SECURE_WAIT
    "auth",                 // TCPIP_SMTPC_STAT_AUTH
    "auth_usr_w",           // TCPIP_SMTPC_STAT_AUTH_WAIT_LOGIN_USER
    "auth_usr",             // TCPIP_SMTPC_STAT_AUTH_LOGIN_USER
    "auth_pass_w",          // TCPIP_SMTPC_STAT_AUTH_WAIT_LOGIN_PASS
    "auth_pass",            // TCPIP_SMTPC_STAT_AUTH_LOGIN_PASS
    "auth_fin_w",           // TCPIP_SMTPC_STAT_AUTH_WAIT_FINAL
    "envl_from",            // TCPIP_SMTPC_STAT_MAIL_ENVL_FROM
    "envl_from_w",          // TCPIP_SMTPC_STAT_MAIL_ENVL_FROM_WAIT
    "envl_rcpt",            // TCPIP_SMTPC_STAT_MAIL_ENVL_RCPT
    "envl_rcpt_w",          // TCPIP_SMTPC_STAT_MAIL_ENVL_RCPT_WAIT
    "envl_data",            // TCPIP_SMTPC_STAT_MAIL_ENVL_DATA
    "envl_data_w",          // TCPIP_SMTPC_STAT_MAIL_ENVL_DATA_WAIT
    "mail_date",            // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_DATE
    "mail_from",            // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_FROM
    "mail_to",              // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_TO
    "mail_snder",           // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_SENDER
    "mail_subj",            // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_SUBJECT
    "mail_msgid",           // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_MSG_ID
    "mail_mimehdr",         // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_MIME_HDR
    "mail_preamb",          // TCPIP_SMTPC_STAT_MAIL_MSG_HDR_PREAMBLE
    "mail_body_hdr",        // TCPIP_SMTPC_STAT_MAIL_MSG_BODY_HDR
    "mail_body",            // TCPIP_SMTPC_STAT_MAIL_MSG_BODY
    "mail_buff_hdr",        // TCPIP_SMTPC_STAT_MAIL_MSG_BUFF_HDR
    "mail_buff_body",       // TCPIP_SMTPC_STAT_MAIL_MSG_BUFF_BODY
    "mail_file_hdr",        // TCPIP_SMTPC_STAT_MAIL_MSG_FILE_HDR
    "mail_file_body",       // TCPIP_SMTPC_STAT_MAIL_MSG_FILE_BODY
    "mail_done",            // TCPIP_SMTPC_STAT_MAIL_DONE
    "mail_done_w",          // TCPIP_SMTPC_STAT_MAIL_DONE_WAIT
    "mail_report",          // TCPIP_SMTPC_STAT_MAIL_DONE_REPORT
    "mail_reset",           // TCPIP_SMTPC_STAT_TRANSACTION_RESET
    "mail_reset_w",         // TCPIP_SMTPC_STAT_TRANSACTION_RESET_WAIT
    "mail_quit",            // TCPIP_SMTPC_STAT_TRANSACTION_QUIT
    "mail_quit_w",          // TCPIP_SMTPC_STAT_TRANSACTION_QUIT_WAIT
    "mail_tr_close",        // TCPIP_SMTPC_STAT_TRANSACTION_CLOSE
    "mail_freed",           // TCPIP_SMTPC_STAT_FREED
};
#endif  // (TCPIP_SMTPC_DEBUG_LEVEL & (TCPIP_SMTPC_DEBUG_MASK_STATE | TCPIP_SMTPC_DEBUG_MASK_ERROR_STATE) != 0)

#if ((TCPIP_SMTPC_DEBUG_LEVEL & TCPIP_SMTPC_DEBUG_MASK_STATE) != 0)
static void _SMTPCDbgDcptState(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    if(pDcpt->currStat != pDcpt->prevStat)
    {
        SYS_CONSOLE_PRINT("SMTPC dcpt: %d, state: %s\r\n", pDcpt->dcptIx, _SMTPCDbgDcptStateTbl[pDcpt->currStat]);
        pDcpt->prevStat = pDcpt->currStat;
    }
}

#else
#define _SMTPCDbgDcptState(pDcpt)
#endif  // ((TCPIP_SMTPC_DEBUG_LEVEL & TCPIP_SMTPC_DEBUG_MASK_STATE) != 0)

#if ((TCPIP_SMTPC_DEBUG_LEVEL & TCPIP_SMTPC_DEBUG_MASK_ERROR_STATE) != 0)
static void _SMTPCDbgDcptErrorState(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, TCPIP_SMTPC_MESSAGE_RESULT res)
{
    SYS_CONSOLE_PRINT("SMTPC dcpt error: %d, state curr: %s, jump: %s, res: %d\r\n", pDcpt->dcptIx, _SMTPCDbgDcptStateTbl[pDcpt->currStat], 
                                                                                   _SMTPCDbgDcptStateTbl[pDcpt->errorJumpStat], res);
}

#else
#define _SMTPCDbgDcptErrorState(pDcpt, res)
#endif  // ((TCPIP_SMTPC_DEBUG_LEVEL & TCPIP_SMTPC_DEBUG_MASK_ERROR_STATE) != 0)


#if ((TCPIP_SMTPC_DEBUG_LEVEL & TCPIP_SMTPC_DEBUG_MASK_CMD_TO_SRV) != 0)
static void _SMTPCDbgMailStrToSrv(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* srvCommand, int nBytes)
{
    _SMTPCAssertCond(nBytes == strlen(srvCommand), __func__, __LINE__);
    SYS_CONSOLE_PRINT("SMTPC dcpt: %d to Srv:-%sCRLF\r\n", pDcpt->dcptIx, srvCommand);
}
#else
#define _SMTPCDbgMailStrToSrv(pDcpt, srvCommand, nBytes)
#endif  // ((TCPIP_SMTPC_DEBUG_LEVEL & TCPIP_SMTPC_DEBUG_MASK_CMD_TO_SRV) != 0)


#if ((TCPIP_SMTPC_DEBUG_LEVEL & TCPIP_SMTPC_DEBUG_MASK_REPLY_FROM_SRV) != 0)
static void _SMTPCDbgMailStrFromSrv(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* srvReply)
{
    SYS_CONSOLE_PRINT("SMTPC dcpt: %d, from Srv:-%sCRLF\r\n", pDcpt->dcptIx, srvReply);
}
#else
#define _SMTPCDbgMailStrFromSrv(pDcpt, srvReply)
#endif  // ((TCPIP_SMTPC_DEBUG_LEVEL & TCPIP_SMTPC_DEBUG_MASK_REPLY_FROM_SRV) != 0)

static __inline__ void __attribute__((always_inline)) smtpcSetStatus(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, TCPIP_SMTPC_STATUS newStat)
{
    pDcpt->currStat = newStat;
    _SMTPCDbgDcptState(pDcpt);
}

static __inline__ void __attribute__((always_inline)) smtpcSetWarning(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, TCPIP_SMTPC_MESSAGE_WARNING newWarn)
{
    pDcpt->messageWarn |= newWarn;
}

static __inline__ void __attribute__((always_inline)) smtpcSetErrorJump(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, TCPIP_SMTPC_STATUS errorJump)
{
    pDcpt->errorJumpStat = errorJump;
}

static __inline__ OSAL_CRITSECT_DATA_TYPE __attribute__((always_inline)) smtpcThreadLock(void)
{
    return OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
}

static __inline__ void __attribute__((always_inline)) smtpcThreadUnlock(OSAL_CRITSECT_DATA_TYPE critStatus)
{
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critStatus);
}


static __inline__ TCPIP_SMTPC_MESSAGE_STATUS __attribute__((always_inline)) smtpStatusTxlate(TCPIP_SMTPC_STATUS smtpcStat)
{
    return (TCPIP_SMTPC_MESSAGE_STATUS)smtpStatusTxlateTbl[smtpcStat];
}

// returns a retry flag based on the server reply
// basically:
//      codes 5yz mean no retry
//      codes 4yz are transient and message could be retried
//      codes 2yz and 3yz are successful and they's generate retries (if not exactly what SMTPC was expecting)
//      anything else will be fatal error, no retry
static __inline__ TCPIP_SMTPC_DCPT_FLAGS __attribute__((always_inline)) smtpcServerCodeRetryFlag(int serverCode)
{
    return (200 <= serverCode && serverCode < 500) ? TCPIP_SMTPC_DCPT_FLAG_RETRY_SERVER : TCPIP_SMTPC_DCPT_FLAG_RETRY_NONE;
}

/****************************************************************************
  Section:
	SMTPC Function Implementations
  ***************************************************************************/


#if (TCPIP_STACK_DOWN_OPERATION != 0)
// if pNetIf == 0, all connections are closed, stack is going down
static void _SMTPC_CloseConnections(TCPIP_NET_IF* pNetIf)
{
    TCPIP_SMTPC_MESSAGE_DCPT* pDcpt;
    SINGLE_LIST newList;

    TCPIP_Helper_SingleListInitialize(&newList);

    while((pDcpt = (TCPIP_SMTPC_MESSAGE_DCPT*)TCPIP_Helper_SingleListHeadRemove(&smtpcMessageBusyList)) != 0)
    {
        bool closeSkt = false;
        if(pDcpt->skt != NET_PRES_INVALID_SOCKET)
        {   // active socket on this one; get the interface over which we're connected
            if(pNetIf == 0)
            {
                closeSkt = true;
            }
            else
            {
                TCP_SOCKET_INFO sktInfo;
                NET_PRES_SocketInfoGet(pDcpt->skt, &sktInfo);
                closeSkt = sktInfo.hNet == (TCPIP_NET_HANDLE)pNetIf;
            }
        }

        if(closeSkt)
        {
            NET_PRES_SocketClose(pDcpt->skt);
            pDcpt->skt = NET_PRES_INVALID_SOCKET;
        }
        else
        {
            TCPIP_Helper_SingleListTailAdd(&newList, (SGL_LIST_NODE*)pDcpt);
        }
    }

    smtpcMessageBusyList = newList;
}

static void _SMTPCCleanup(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    _SMTPC_CloseConnections(0);

    // free the allocated resources
    if(smtpcAllocPool)
    {
        TCPIP_HEAP_Free(stackCtrl->memH, smtpcAllocPool);
        smtpcAllocPool = 0;
    }
    
    if(smtpcSignalHandle)
    {
        _TCPIPStackSignalHandlerDeregister(smtpcSignalHandle);
        smtpcSignalHandle = 0;
    }
}

void TCPIP_SMTPC_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN) // interface down
    
    if(smtpcInitCount > 0)
    {   // we're up and running
        _SMTPC_CloseConnections(stackCtrl->pNetIf);

        if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
        {   // stack shut down
            if(--smtpcInitCount == 0)
            {   // all closed
                // release resources
                _SMTPCCleanup(stackCtrl);
            }
        }
    }
}
#else
#define _SMTPCCleanup(stackCtrl)
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)
bool TCPIP_SMTPC_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_SMTPC_MODULE_CONFIG* pSmtpcConfig)
{

    int ix;
    TCPIP_SMTPC_MESSAGE_DCPT* pDcpt;

    if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }

    // stack init
    if(smtpcInitCount == 0)
    {   // stack start up; initialize just once
        // check configuration data is not missing
        if(pSmtpcConfig == 0)
        {
            return false;
        }
        
        // critical initialization here...!

        // initialize the active message list
        TCPIP_Helper_SingleListInitialize(&smtpcMessageBusyList);

        // create the SMTPC timer
        smtpcSignalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_SMTPC_Task, TCPIP_SMTPC_TASK_TICK_RATE);
        // allocate the descriptors pool
        smtpcAllocPool = (TCPIP_SMTPC_MESSAGE_DCPT*)TCPIP_HEAP_Calloc(stackCtrl->memH, pSmtpcConfig->nMailConnections, sizeof(*smtpcAllocPool));
    

        if(smtpcSignalHandle == 0 || smtpcAllocPool == 0)
        {   // cannot create the SMTPC timer/mail list
            _SMTPCCleanup(stackCtrl);
            return false;
        }

        // do some other non-critical initialization here...!
        // generate some (unique) counter
        smtpcMailCounter = (uint32_t)pSmtpcConfig + SYS_RANDOM_PseudoGet() % 1000;

        // initialize the message pool
        TCPIP_Helper_SingleListInitialize(&smtpcMessageFreeList);
        pDcpt = smtpcAllocPool;
        for(ix = 0; ix < pSmtpcConfig->nMailConnections; ix++, pDcpt++)
        {
            pDcpt->skt = NET_PRES_INVALID_SOCKET; 
            pDcpt->dcptIx = ix;
            TCPIP_Helper_SingleListTailAdd(&smtpcMessageFreeList, (SGL_LIST_NODE*)pDcpt);
        }

        smtpcSktTxBuffSize = pSmtpcConfig->sktTxBuffSize;
        smtpcSktRxBuffSize = pSmtpcConfig->sktRxBuffSize;
        // timeouts
        smtpcServerGenTmo = pSmtpcConfig->serverReplyTmo > TCPIP_SMTPC_MIN_SERVER_TMO ? pSmtpcConfig->serverReplyTmo : TCPIP_SMTPC_MIN_SERVER_TMO;
        smtpcServerDataTmo = pSmtpcConfig->serverDataTmo > TCPIP_SMTPC_MIN_SERVER_TMO ? pSmtpcConfig->serverDataTmo : TCPIP_SMTPC_MIN_SERVER_TMO;
        smtpcTlsHandshakeTmo = pSmtpcConfig->tlsHandshakeTmo > TCPIP_SMTPC_MIN_SERVER_TMO ? pSmtpcConfig->tlsHandshakeTmo : TCPIP_SMTPC_MIN_SERVER_TMO;
        smtpcMailRetries = pSmtpcConfig->nMailRetries != 0 ? pSmtpcConfig->nMailRetries : TCPIP_SMTPC_MIN_RETRIES;
        smtpcServerRetryTmo = pSmtpcConfig->serverRetryTmo >= TCPIP_SMTPC_MIN_SERVER_RETRY_TMO ? pSmtpcConfig->serverRetryTmo : TCPIP_SMTPC_MIN_SERVER_RETRY_TMO;
        smtpcInternalRetryTmo = pSmtpcConfig->smtpcRetryTmo >= TCPIP_SMTPC_MIN_INTERNAL_RETRY_TMO ? pSmtpcConfig->smtpcRetryTmo : TCPIP_SMTPC_MIN_INTERNAL_RETRY_TMO;
    }
    
    smtpcInitCount++;
    return true;
 
}

TCPIP_SMTPC_MESSAGE_HANDLE TCPIP_SMTPC_MailMessage(const TCPIP_SMTPC_MAIL_MESSAGE* pMailMessage, TCPIP_SMTPC_MESSAGE_RESULT* pRes)
{
    TCPIP_SMTPC_MESSAGE_RESULT res;
    IP_MULTI_ADDRESS    ipAddr;
    IP_ADDRESS_TYPE     addType, existType;
    bool                useDns;
    TCPIP_SMTPC_STATUS  newStat;
    TCPIP_SMTPC_MESSAGE_DCPT* pDcpt = 0;

    while(true)
    {
        if(smtpcAllocPool == 0)
        {   // not up and running!
            res = TCPIP_SMTPC_RES_INITIALIZE_ERROR;
            break;
        }

        // do a minimum sanity check
        if(pMailMessage == 0)
        {
            res = TCPIP_SMTPC_RES_MESSAGE_ERROR;
            break;
        }

        if(pMailMessage->smtpServer == 0 || strlen(pMailMessage->smtpServer) == 0 || pMailMessage->serverPort == 0)
        {
            res = TCPIP_SMTPC_RES_MESSAGE_SERVER_ERROR;
            break;
        }

        if(pMailMessage->from == 0 || pMailMessage->to == 0)
        {
            res = TCPIP_SMTPC_RES_MESSAGE_RCPT_ERROR;
            break;
        }

        if((pMailMessage->nBuffers != 0 && pMailMessage->attachBuffers == 0) || (pMailMessage->nBuffers == 0 && pMailMessage->attachBuffers != 0))
        {
            res = TCPIP_SMTPC_RES_MESSAGE_BUFFER_ERROR;
            break;
        }

        if((pMailMessage->nFiles != 0 && pMailMessage->attachFiles == 0) || (pMailMessage->nFiles == 0 && pMailMessage->attachFiles != 0))
        {
            res = TCPIP_SMTPC_RES_MESSAGE_FILE_ERROR;
            break;
        }

        useDns = false;
        if(TCPIP_Helper_StringToIPAddress(pMailMessage->smtpServer, &ipAddr.v4Add))
        {   // IPv4 address indicated
            addType = IP_ADDRESS_TYPE_IPV4;
        }
        else if(TCPIP_Helper_StringToIPv6Address (pMailMessage->smtpServer, &ipAddr.v6Add))
        {
            addType = IP_ADDRESS_TYPE_IPV6;
        }
        else
        {   // a server name is specified; DNS needed
            useDns = true;
            addType = IP_ADDRESS_TYPE_ANY;
#if defined (TCPIP_STACK_USE_IPV6)
            if((pMailMessage->messageFlags & TCPIP_SMTPC_MAIL_FLAG_USE_IPV6) != 0)
            {   // IPv6 requested
                addType = IP_ADDRESS_TYPE_IPV6;
            }
            else
            {
                existType = IP_ADDRESS_TYPE_IPV6;
            }
#endif // defined (TCPIP_STACK_USE_IPV6)

#if defined (TCPIP_STACK_USE_IPV4)
            existType = IP_ADDRESS_TYPE_IPV4;
#endif  // defined (TCPIP_STACK_USE_IPV4)

            if(addType == IP_ADDRESS_TYPE_ANY)
            {   // IPv4 takes precedence
                addType = existType;
            }
        }

        // start mail scheduling
        OSAL_CRITSECT_DATA_TYPE critStat = smtpcThreadLock();
        pDcpt = (TCPIP_SMTPC_MESSAGE_DCPT*)TCPIP_Helper_SingleListHeadRemove(&smtpcMessageFreeList);
        smtpcThreadUnlock(critStat);

        if(pDcpt == 0)
        {
            res = TCPIP_SMTPC_RES_MAIL_BUSY;
            break;
        }

        // mail descriptor initialization
        pDcpt->smtpMailMessage = *pMailMessage; // make a copy of the message 
        pDcpt->addType = addType;
        pDcpt->retryCnt = smtpcMailRetries;

       if(!useDns)
        {
            pDcpt->serverAdd = ipAddr;
            newStat = TCPIP_SMTPC_STAT_SOCKET_GET;
        }
        else
        {
            pDcpt->dcptFlags |= TCPIP_SMTPC_DCPT_FLAG_DNS;
            newStat = TCPIP_SMTPC_STAT_DNS_START;
        }
        smtpcSetStatus(pDcpt, newStat);
        smtpcRetryInit(pDcpt);

        critStat = smtpcThreadLock();
        TCPIP_Helper_SingleListTailAdd(&smtpcMessageBusyList, (SGL_LIST_NODE*)pDcpt);
        smtpcThreadUnlock(critStat);

        res = TCPIP_SMTPC_RES_OK;
        break;
    }


    if(pRes)
    {
        *pRes = res;
    }

    return pDcpt; 
}

 
TCPIP_SMTPC_MESSAGE_RESULT TCPIP_SMTPC_MessageQuery(TCPIP_SMTPC_MESSAGE_HANDLE messageHandle, TCPIP_SMTPC_MESSAGE_QUERY* pQuery)
{
    TCPIP_SMTPC_MESSAGE_RESULT res = TCPIP_SMTPC_RES_MESSAGE_ERROR; 
    TCPIP_SMTPC_MESSAGE_DCPT* pDcpt = (TCPIP_SMTPC_MESSAGE_DCPT*)messageHandle;

    if(pDcpt)
    {
        OSAL_CRITSECT_DATA_TYPE critStat = smtpcThreadLock();

        if(TCPIP_Helper_SingleListFind(&smtpcMessageBusyList, (SGL_LIST_NODE*)pDcpt))
        {   // OK, one of ours
            if(pQuery)
            {
                pQuery->messageStat = smtpStatusTxlate(pDcpt->currStat);
                pQuery->messageWarn = pDcpt->messageWarn;
                pQuery->messageRetries = pDcpt->retryCnt;
                pQuery->messageSkt = pDcpt->skt;
            }
            res = TCPIP_SMTPC_RES_OK; 
        }
        smtpcThreadUnlock(critStat);
    }

    return res;
}


void  TCPIP_SMTPC_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != 0)
    { // TMO or RX signals occurred
        TCPIP_SMTPC_Process();
    }
}

// send a signal to the SMTP module that data is available
// no manager alert needed since this normally results as a higher layer (TCP) signal
static void _SMTPCSocketRxSignalHandler(NET_PRES_SKT_HANDLE_T hTCP, NET_PRES_SIGNAL_HANDLE hNet, uint16_t sigType, const void* param)
{
    if(sigType == TCPIP_TCP_SIGNAL_RX_DATA)
    {
        _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}

// process SMTPC state machine
// process all the busy mail descriptors according to state
static void TCPIP_SMTPC_Process(void)
{
    TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, *pNext, *pPrev;
    TCPIP_SMTPC_STATUS newStat;

    OSAL_CRITSECT_DATA_TYPE critStat = smtpcThreadLock();
    pDcpt = (TCPIP_SMTPC_MESSAGE_DCPT*)smtpcMessageBusyList.head;
    pNext = pDcpt? pDcpt->next : 0;
    smtpcThreadUnlock(critStat);
    pPrev = 0;


    while(pDcpt != 0)
    {
        // process this descriptor
        _SMTPCAssertCond(0 <= pDcpt->currStat && pDcpt->currStat <= sizeof(smtpDcptStateTbl) / sizeof(*smtpDcptStateTbl), __func__, __LINE__);
        // check for possible disconnect
        if((pDcpt->dcptFlags & TCPIP_SMTPC_DCPT_FLAG_CONNECTED) != 0 && NET_PRES_SocketWasReset(pDcpt->skt))
        {   // lost connection to the server
            pDcpt->dcptFlags &= ~(TCPIP_SMTPC_SERVER_FLAG_CONN_MASK);
            newStat = smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_CONNECTION_CLOSE, TCPIP_SMTPC_DCPT_FLAG_RETRY_SERVER);
        }
        else
        {
            newStat = (*smtpDcptStateTbl[pDcpt->currStat])(pDcpt);
        }
        smtpcSetStatus(pDcpt, newStat);

        critStat = smtpcThreadLock();
        if(newStat == TCPIP_SMTPC_STAT_FREED)
        {
            TCPIP_Helper_SingleListNextRemove(&smtpcMessageBusyList, (SGL_LIST_NODE*)pPrev);
            TCPIP_Helper_SingleListTailAdd(&smtpcMessageFreeList, (SGL_LIST_NODE*)pDcpt);
        }
        else
        {
            pPrev = pDcpt;
        }
        pDcpt = pNext;
        pNext = pDcpt? pDcpt->next : 0;
        smtpcThreadUnlock(critStat);
    }

}

// TCPIP_SMTPC_STAT_WAIT_RETRY
static TCPIP_SMTPC_STATUS smtpDcptStateWaitRetry(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{  
    TCPIP_SMTPC_STATUS newStat;

    if(!smtpcIsDcptTmo(pDcpt))
    {   // wait some more
        newStat = TCPIP_SMTPC_STAT_WAIT_RETRY;
    }
    else
    {
        newStat = (pDcpt->dcptFlags & TCPIP_SMTPC_DCPT_FLAG_DNS) != 0 ? TCPIP_SMTPC_STAT_DNS_START : TCPIP_SMTPC_STAT_SOCKET_GET;
        smtpcRetryInit(pDcpt);
    }

    return newStat;
}

// TCPIP_SMTPC_STAT_DNS_START
static TCPIP_SMTPC_STATUS smtpDcptStateStartDns(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    TCPIP_DNS_RESULT dnsRes = TCPIP_DNS_Resolve(pDcpt->smtpMailMessage.smtpServer, pDcpt->addType == IP_ADDRESS_TYPE_IPV6 ? TCPIP_DNS_TYPE_AAAA : TCPIP_DNS_TYPE_A);
    
    if(dnsRes < 0)
    {
        return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_DNS_ERROR, TCPIP_SMTPC_DCPT_FLAG_RETRY_SMPTC);
    } 

    // ok, move on
    return TCPIP_SMTPC_STAT_DNS_WAIT;
}

// TCPIP_SMTPC_STAT_DNS_WAIT
static TCPIP_SMTPC_STATUS smtpDcptStateWaitDns(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    IP_MULTI_ADDRESS serverIP;

    TCPIP_DNS_RESULT dnsRes = TCPIP_DNS_IsResolved(pDcpt->smtpMailMessage.smtpServer, &serverIP, pDcpt->addType);
    if(dnsRes < 0)
    {
        return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_DNS_ERROR, TCPIP_SMTPC_DCPT_FLAG_RETRY_SMPTC);
    } 
    else if(dnsRes == TCPIP_DNS_RES_PENDING)
    {   // not ready
        return TCPIP_SMTPC_STAT_DNS_WAIT;
    }

    // DNS done; advance
    pDcpt->serverAdd = serverIP;
    return TCPIP_SMTPC_STAT_DNS_WAIT + 1;
}

// TCPIP_SMTPC_STAT_SOCKET_GET
static TCPIP_SMTPC_STATUS smtpDcptStateGetSkt(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    TCPIP_SMTPC_MESSAGE_RESULT res = TCPIP_SMTPC_RES_OK;

    if(pDcpt->skt == NET_PRES_INVALID_SOCKET)
    {   // get a new socket
        NET_PRES_SKT_HANDLE_T newSkt;
        NET_PRES_SKT_T sktType =  (pDcpt->smtpMailMessage.messageFlags &TCPIP_SMTPC_MAIL_FLAG_CONNECT_TLS) != 0 ? NET_PRES_SKT_ENCRYPTED_STREAM_CLIENT : NET_PRES_SKT_UNENCRYPTED_STREAM_CLIENT;
        newSkt = NET_PRES_SocketOpen(0, sktType, (NET_PRES_SKT_ADDR_T)pDcpt->addType, pDcpt->smtpMailMessage.serverPort, 0, 0);

        if(newSkt == NET_PRES_INVALID_SOCKET)
        {   // failed to open a socket
            res = TCPIP_SMTPC_RES_SKT_OPEN_ERROR;
        }
        else
        {   // success; get the socket TX buffer size 
            TCP_SOCKET_INFO sktInfo;
            NET_PRES_SocketInfoGet(newSkt, &sktInfo);

            if(smtpcSktTxBuffSize == 0)
            {   // default is used
                smtpcSktTxBuffSize = sktInfo.txSize;
            }

            if(smtpcSktTxBuffSize < ((TCPIP_SMTPC_PLAIN_LINE_BUFF_SIZE + TCPIP_SMTPC_PLAIN_LINE_BUFF_SIZE_GUARD) * TCPIP_SMTPC_PLAIN_LINE_BUFF_SIZE_RATIO) / 100)
            {
                smtpcSktTxBuffSize = ((TCPIP_SMTPC_PLAIN_LINE_BUFF_SIZE + TCPIP_SMTPC_PLAIN_LINE_BUFF_SIZE_GUARD) * TCPIP_SMTPC_PLAIN_LINE_BUFF_SIZE_RATIO) / 100;
            }

            // finally check the BASE64 settings
            if(smtpcSktTxBuffSize < (TCPIP_SMTP_BASE64_BUFF_SIZE * 4 * 18) / 3 / 10)
            {
                smtpcSktTxBuffSize = (TCPIP_SMTP_BASE64_BUFF_SIZE * 4 * 18) / 3 / 10;
            }

            if(smtpcSktTxBuffSize > sktInfo.txSize)
            {   // not using the default; adjust
                void* tcpBuffSize = (void*)(unsigned int)smtpcSktTxBuffSize;
                NET_PRES_SocketOptionsSet(newSkt, TCP_OPTION_TX_BUFF, tcpBuffSize);
            }
            if(smtpcSktRxBuffSize != 0)
            {
                void* tcpBuffSize = (void*)(unsigned int)smtpcSktRxBuffSize;
                NET_PRES_SocketOptionsSet(newSkt, TCP_OPTION_RX_BUFF, tcpBuffSize);
            }
            
            // alert of incoming traffic
            NET_PRES_SocketSignalHandlerRegister(newSkt, TCPIP_TCP_SIGNAL_RX_DATA, _SMTPCSocketRxSignalHandler, 0);
            pDcpt->skt = newSkt;
        }
    }
    
    if(res == TCPIP_SMTPC_RES_OK)
    {   // point socket to the SMTP server
        if(!NET_PRES_SocketRemoteBind(pDcpt->skt, (NET_PRES_SKT_ADDR_T)pDcpt->addType,  pDcpt->smtpMailMessage.serverPort, (NET_PRES_ADDRESS*)&pDcpt->serverAdd))
        {   // failed
            _SMTPCDebugCond(false, __func__, __LINE__);
            res = TCPIP_SMTPC_RES_SKT_BIND_ERROR; 
        }
    }

    if(res != TCPIP_SMTPC_RES_OK)
    {
        return smtpcErrorStop(pDcpt, res, TCPIP_SMTPC_DCPT_FLAG_RETRY_SMPTC);
    }

    // advance
    // clear the disconnect condition
    NET_PRES_SocketWasReset(pDcpt->skt);
    NET_PRES_SocketConnect(pDcpt->skt);
    smtpcSetDcptTmo(pDcpt, smtpcServerGenTmo);
    return TCPIP_SMTPC_STAT_SOCKET_GET + 1;
}

// TCPIP_SMTPC_STAT_EHLO_PLAIN; TCPIP_SMTPC_STAT_EHLO_SECURE
static TCPIP_SMTPC_STATUS smtpDcptStateSendEhlo(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{   // send EHLO to server...
    char    ehloBuff[50];

    smtpSetEhloString(pDcpt, ehloBuff, sizeof(ehloBuff));

    TCPIP_SMTPC_CMD_TYPE cmdType = ((pDcpt->smtpMailMessage.messageFlags & TCPIP_SMTPC_MAIL_FLAG_GREET_HELO) != 0) ? TCPIP_SMTPC_CMD_HELO : TCPIP_SMTPC_CMD_EHLO;
    return smtpCommandParam(pDcpt, cmdType, ehloBuff, smtpcServerGenTmo);
}


// TCPIP_SMTPC_STAT_TLS_START
// send STARTTLS to the server
static TCPIP_SMTPC_STATUS smtpDcptStateStartTls(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    return smtpCommandParam(pDcpt, TCPIP_SMTPC_CMD_STARTTLS, 0, smtpcServerGenTmo);
}

// TCPIP_SMTPC_STAT_TLS_WAIT_SECURE
// wait for connection to be secured...
static TCPIP_SMTPC_STATUS smtpDcptStateWaitTlsSecure(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    if(NET_PRES_SocketIsNegotiatingEncryption(pDcpt->skt))
    {   // still negotiating...
        return smtpcIsDcptTmo(pDcpt) ? TCPIP_SMTPC_RES_TLS_TMO : TCPIP_SMTPC_STAT_TLS_WAIT_SECURE;
    }

    // negotiation done
    if(!NET_PRES_SocketIsSecure(pDcpt->skt))
    {   // negotiation failed; fatal
        return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_TLS_FAILED, TCPIP_SMTPC_DCPT_FLAG_RETRY_SMPTC);
    }

    // TLS success; advance
    pDcpt->dcptFlags |= TCPIP_SMTPC_DCPT_FLAG_SECURED;
    return TCPIP_SMTPC_STAT_EHLO_SECURE;
}

// TCPIP_SMTPC_STAT_AUTH
// start authentication
static TCPIP_SMTPC_STATUS smtpDcptStateAuth(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    if(pDcpt->smtpMailMessage.username == 0 || strlen(pDcpt->smtpMailMessage.username) == 0 || pDcpt->smtpMailMessage.password == 0 || strlen(pDcpt->smtpMailMessage.password) == 0)
    {
        return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_MESSAGE_AUTH_REQUIRED, TCPIP_SMTPC_DCPT_FLAG_RETRY_NONE);
    }

    if(((pDcpt->dcptFlags & TCPIP_SMTPC_SERVER_FLAG_AUTH_PLAIN) != 0) && (pDcpt->smtpMailMessage.messageFlags & TCPIP_SMTPC_MAIL_FLAG_AUTH_PLAIN) != 0)
    {
        return smtpAuthPlain(pDcpt);
    }
    else if((pDcpt->dcptFlags & TCPIP_SMTPC_SERVER_FLAG_AUTH_LOGIN) != 0)
    {   // start auth login
        return smtpAuthLogin(pDcpt);
    }
    else if((pDcpt->dcptFlags & TCPIP_SMTPC_SERVER_FLAG_AUTH_PLAIN) != 0)
    {
        return smtpAuthPlain(pDcpt);
    }

    // else
    return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_AUTH_UNKNOWN, TCPIP_SMTPC_DCPT_FLAG_RETRY_NONE);
}

// TCPIP_SMTPC_STAT_AUTH
static TCPIP_SMTPC_STATUS smtpAuthLogin(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    return smtpCommandParam(pDcpt, TCPIP_SMTPC_CMD_AUTH_LOGIN, 0, smtpcServerGenTmo);

}

// TCPIP_SMTPC_STAT_AUTH
// provides the plain authentication to the server
static TCPIP_SMTPC_STATUS smtpAuthPlain(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    char authBuff[((TCPIP_SMTPC_CLIENT_AUTH_BUFFER_SIZE + 2) / 3) * 3]; // authentication buffer; M3
    char auth64Buff[(sizeof(authBuff) * 4) / 3];        // base64 conversion buffer

    char* pBuff;
    int usrLen, passLen;
    uint16_t base64Len;
    TCPIP_SMTPC_STATUS newStat;

    usrLen = strlen(pDcpt->smtpMailMessage.username);
    passLen = strlen(pDcpt->smtpMailMessage.password);

    if( (usrLen + usrLen + passLen) >= (sizeof(authBuff) - 3))
    {
        return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_MESSAGE_AUTH_LEN_ERROR, TCPIP_SMTPC_DCPT_FLAG_RETRY_NONE);
    }

    pBuff = authBuff;
    // authorization ID:
    strcpy(pBuff, pDcpt->smtpMailMessage.username);
    pBuff += usrLen;
    *pBuff++ = 0;

    // authentication ID:
    strcpy(pBuff, pDcpt->smtpMailMessage.username);
    pBuff += usrLen;
    *pBuff++ = 0;

    // passwd
    strcpy(pBuff, pDcpt->smtpMailMessage.password);
    pBuff += passLen;

    base64Len = TCPIP_Helper_Base64Encode((uint8_t*)authBuff, pBuff - authBuff, (uint8_t*)auth64Buff, sizeof(auth64Buff));
    auth64Buff[base64Len] = 0;  // end string properly

    
    newStat = smtpCommandParam(pDcpt, TCPIP_SMTPC_CMD_AUTH_PLAIN, auth64Buff, smtpcServerGenTmo);
    if(newStat == TCPIP_SMTPC_STAT_AUTH + 1)
    {   // everything was OK but we'll jump
        newStat = TCPIP_SMTPC_STAT_AUTH_WAIT_FINAL;
    }

    return newStat;
}

// TCPIP_SMTPC_STAT_AUTH_LOGIN_USER
// send the login user to the server
static TCPIP_SMTPC_STATUS smtpDcptStateAuthLoginUser(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    uint16_t base64Len;

    char    auth64Buff[((TCPIP_SMTPC_CLIENT_AUTH_BUFFER_SIZE + 2) / 3) * 4 + 1];

    int userLen = strlen(pDcpt->smtpMailMessage.username);

    if( ((userLen + 2) * 4) / 3 > sizeof(auth64Buff) - 1) 
    {   // exceeding the space in the buffer; truncate
        userLen = (sizeof(auth64Buff) - 1 * 3) / 4 - 2;
        smtpcSetWarning(pDcpt, TCPIP_SMTPC_WARN_AUTH_LOGIN_LEN);
    }

    // send the user
    base64Len = TCPIP_Helper_Base64Encode((uint8_t*)pDcpt->smtpMailMessage.username, userLen, (uint8_t*)auth64Buff, sizeof(auth64Buff));
    auth64Buff[base64Len] = 0;
    return smtpCommandParam(pDcpt, TCPIP_SMTPC_CMD_NONE, auth64Buff, smtpcServerGenTmo);
}

// TCPIP_SMTPC_STAT_AUTH_LOGIN_PASS
// send the login password to the server
static TCPIP_SMTPC_STATUS smtpDcptStateAuthLoginPass(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    uint16_t base64Len;

    char    auth64Buff[((TCPIP_SMTPC_CLIENT_AUTH_BUFFER_SIZE + 2) / 3) * 4 + 1];

    int passLen = strlen(pDcpt->smtpMailMessage.password);

    if( ((passLen + 2) * 4) / 3 > sizeof(auth64Buff) - 1) 
    {   // exceeding the space in the buffer; truncate
        passLen = (sizeof(auth64Buff) - 1 * 3) / 4 - 2;
        smtpcSetWarning(pDcpt, TCPIP_SMTPC_WARN_AUTH_LOGIN_LEN);
    }

    // send the user
    base64Len = TCPIP_Helper_Base64Encode((uint8_t*)pDcpt->smtpMailMessage.password, passLen, (uint8_t*)auth64Buff, sizeof(auth64Buff));
    auth64Buff[base64Len] = 0;
    return smtpCommandParam(pDcpt, TCPIP_SMTPC_CMD_NONE, auth64Buff, smtpcServerGenTmo);
}


// TCPIP_SMTPC_STAT_MAIL_START == TCPIP_SMTPC_STAT_MAIL_ENVL_FROM
// start sending the mail
static TCPIP_SMTPC_STATUS smtpDcptStateMailFrom(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    // initialize the mail status
    // set the buffers and files
    pDcpt->smtpAttachBuffer = 0;
    pDcpt->nSmtpAttachBuffers = 0;
    pDcpt->smtpAttachFile = 0;
    pDcpt->nSmtpAttachFiles = 0;
    
    return smtpWriteMailEnvAddress(pDcpt, TCPIP_SMTPC_ENV_FROM);
}


// TCPIP_SMTPC_STAT_MAIL_ENVL_RCPT
static TCPIP_SMTPC_STATUS smtpDcptStateMailRcpt(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    return smtpWriteMailEnvAddress(pDcpt, TCPIP_SMTPC_ENV_RCPT);
}

// TCPIP_SMTPC_STAT_MAIL_ENVL_DATA
static TCPIP_SMTPC_STATUS smtpDcptStateMailData(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    return smtpCommandParam(pDcpt, TCPIP_SMTPC_CMD_MAIL_DATA, 0, smtpcServerGenTmo);
}

// TCPIP_SMTPC_STAT_MAIL_MSG_HDR_DATE
static TCPIP_SMTPC_STATUS smtpDcptMailMsgDate(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    const char* date = pDcpt->smtpMailMessage.date;

#if defined TCPIP_SMTPC_CLIENT_MESSAGE_DATE
    if(date == 0)
    {
        date = TCPIP_SMTPC_CLIENT_MESSAGE_DATE;
    }
#endif // defined TCPIP_SMTPC_CLIENT_MESSAGE_DATE

    if(date)
    {
        return smtpClientWriteCmd(pDcpt, "Date:", date);
    }

    // skip date field
    return TCPIP_SMTPC_STAT_MAIL_MSG_HDR_FROM;
}

// TCPIP_SMTPC_STAT_MAIL_MSG_HDR_FROM
static TCPIP_SMTPC_STATUS smtpDcptMailMsgFrom(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    return smtpClientWriteCmd(pDcpt, "From:",  pDcpt->smtpMailMessage.from);
}

// TCPIP_SMTPC_STAT_MAIL_MSG_HDR_TO
static TCPIP_SMTPC_STATUS smtpDcptMailMsgTo(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    return smtpClientWriteCmd(pDcpt, "To:",  pDcpt->smtpMailMessage.to);
}

// TCPIP_SMTPC_STAT_MAIL_MSG_HDR_SENDER
static TCPIP_SMTPC_STATUS smtpDcptMailMsgSender(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    return TCPIP_SMTPC_STAT_MAIL_MSG_HDR_SUBJECT;
}

// TCPIP_SMTPC_STAT_MAIL_MSG_HDR_SUBJECT
static TCPIP_SMTPC_STATUS smtpDcptMailMsgSubject(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    return smtpClientWriteCmd(pDcpt, "Subject:",  pDcpt->smtpMailMessage.subject);
}

// TCPIP_SMTPC_STAT_MAIL_MSG_HDR_MSG_ID
static TCPIP_SMTPC_STATUS smtpDcptMailMsgMessageId(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    // create a message ID: <mailCounter@IPaddress>
    char addBuff[20];
    char msgIdBuff[40];
    TCP_SOCKET_INFO sktInfo;

    NET_PRES_SocketInfoGet(pDcpt->skt, &sktInfo);
    TCPIP_Helper_IPAddressToString(&sktInfo.localIPaddress.v4Add, addBuff, sizeof(addBuff));
    sprintf(msgIdBuff, "<%4d@%s>", smtpcMailCounter++, addBuff);

    return smtpClientWriteCmd(pDcpt, "Message-ID:",  msgIdBuff);
}

// TCPIP_SMTPC_STAT_MAIL_MSG_HDR_MIME_HDR
static TCPIP_SMTPC_STATUS smtpDcptMailMsgMimeHdr(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    char mimeHdr[120];
    // Note: the CRLF is added here, last field before the mail body starts!
    // Note: always multipart, even for mesages w/o attachments!
    sprintf(mimeHdr, "MIME-Version: 1.0\r\nContent-Type: multipart/mixed; boundary=\"%s\"\r\n", smtpcMailBoundary);

    return smtpClientWriteCmd(pDcpt, mimeHdr,  0);
}

// TCPIP_SMTPC_STAT_MAIL_MSG_HDR_PREAMBLE
static TCPIP_SMTPC_STATUS smtpDcptMailMsgPreambleHdr(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    // preamble: could include an explanatory note!
    // should be ignored, not displayed, etc.
    // not used for now
    return TCPIP_SMTPC_STAT_MAIL_MSG_BODY_HDR;
}

// TCPIP_SMTPC_STAT_MAIL_MSG_BODY_HDR
// send the part header
static TCPIP_SMTPC_STATUS smtpDcptMailMsgBodyHdr(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    const uint8_t* partBody = pDcpt->smtpMailMessage.body;
    size_t partSize = pDcpt->smtpMailMessage.bodySize;

    if(partBody == 0 || partSize == 0)
    {   // no body mail; skip
        return TCPIP_SMTPC_STAT_MAIL_MSG_BUFF_HDR;
    }

    // set up the body mime parts
    if((pDcpt->smtpMailMessage.messageFlags & TCPIP_SMTPC_MAIL_FLAG_BODY_TYPE_BINARY) != 0) 
    {
        pDcpt->smtpPartAttachType = TCPIP_SMTPC_ATTACH_TYPE_APPLICATION; 
        pDcpt->smtpPartEncodeType = TCPIP_SMTPC_ENCODE_TYPE_BASE64; 
    }
    else
    {
        pDcpt->smtpPartAttachType = TCPIP_SMTPC_ATTACH_TYPE_TEXT; 
        if((pDcpt->smtpMailMessage.messageFlags & TCPIP_SMTPC_MAIL_FLAG_BODY_ENCODE_ASCII) != 0) 
        {
            pDcpt->smtpPartEncodeType = TCPIP_SMTPC_ENCODE_TYPE_7BIT; 
        }
        else
        {   // default is BASE64
            pDcpt->smtpPartEncodeType = TCPIP_SMTPC_ENCODE_TYPE_BASE64; 
        }
    }

    memset(&pDcpt->smtpReadDcpt, 0, sizeof(pDcpt->smtpReadDcpt));
    pDcpt->smtpReadDcpt.pReadObj = &smtpReadBufferObj;
    if(!pDcpt->smtpReadDcpt.pReadObj->init(&pDcpt->smtpReadDcpt, 0, partBody, partSize))
    {
        _SMTPCAssertCond(false, __func__, __LINE__);
    }
    
    return smtpSendPartHdr(pDcpt, 0);    // body is not attachment
}

// TCPIP_SMTPC_STAT_MAIL_MSG_BODY
static TCPIP_SMTPC_STATUS smtpDcptMailMsgBody(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    TCPIP_SMTPC_STATUS newStat = smtpSendPartBody(pDcpt);

    if(newStat == TCPIP_SMTPC_STAT_MAIL_MSG_BODY + 1)
    {   // done; there's only one body
        pDcpt->smtpReadDcpt.pReadObj->deinit(&pDcpt->smtpReadDcpt);
    } 

    return newStat;

}

// TCPIP_SMTPC_STAT_MAIL_MSG_BUFF_HDR
// send the buffer attachment header
static TCPIP_SMTPC_STATUS smtpDcptMailMsgBuffHdr(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{

    bool buffDone = false;

    if(pDcpt->nSmtpAttachBuffers == pDcpt->smtpMailMessage.nBuffers)
    {   // we're done; jump to sending files
        buffDone =  true;
    }
    else if(pDcpt->nSmtpAttachBuffers == 0)
    {   // just starting; prepare the buffer attachments
        if(pDcpt->smtpMailMessage.attachBuffers != 0 && pDcpt->smtpMailMessage.nBuffers != 0)
        {
            pDcpt->smtpAttachBuffer = pDcpt->smtpMailMessage.attachBuffers;
        }
        else
        {
            buffDone = true;
        }
    }

    if(buffDone)
    {   // we're done; skip the TCPIP_SMTPC_STAT_MAIL_MSG_BUFF_BODY
        return TCPIP_SMTPC_STAT_MAIL_MSG_BUFF_HDR + 2;
    }

    // send this buffer
    if((pDcpt->smtpAttachBuffer->attachType & TCPIP_SMTPC_ATTACH_TYPE_TEXT) != 0)
    {
        pDcpt->smtpPartAttachType = TCPIP_SMTPC_ATTACH_TYPE_TEXT;
        pDcpt->smtpPartEncodeType = pDcpt->smtpAttachBuffer->attachEncode ==  TCPIP_SMTPC_ENCODE_TYPE_7BIT ? TCPIP_SMTPC_ENCODE_TYPE_7BIT: TCPIP_SMTPC_ENCODE_TYPE_BASE64; 
    }
    else
    {
        pDcpt->smtpPartAttachType = TCPIP_SMTPC_ATTACH_TYPE_APPLICATION;
        pDcpt->smtpPartEncodeType = TCPIP_SMTPC_ENCODE_TYPE_BASE64; 
    }

    if((pDcpt->smtpAttachBuffer->attachType & TCPIP_SMTPC_ATTACH_TYPE_INLINE) != 0)
    {
        pDcpt->smtpPartAttachType |= TCPIP_SMTPC_ATTACH_TYPE_INLINE; 
    }

    memset(&pDcpt->smtpReadDcpt, 0, sizeof(pDcpt->smtpReadDcpt));
    pDcpt->smtpReadDcpt.pReadObj = &smtpReadBufferObj;
    if(!pDcpt->smtpReadDcpt.pReadObj->init(&pDcpt->smtpReadDcpt, 0, pDcpt->smtpAttachBuffer->attachBuffer, pDcpt->smtpAttachBuffer->attachSize))
    {
        _SMTPCAssertCond(false, __func__, __LINE__);
    }

    return smtpSendPartHdr(pDcpt, pDcpt->smtpAttachBuffer->attachName);
}

// TCPIP_SMTPC_STAT_MAIL_MSG_BUFF_BODY
// send the buffer body
static TCPIP_SMTPC_STATUS smtpDcptMailMsgBuffBody(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    TCPIP_SMTPC_STATUS newStat = smtpSendPartBody(pDcpt);

    if(newStat == TCPIP_SMTPC_STAT_MAIL_MSG_BUFF_BODY + 1)
    {   // done; send another buffer
        pDcpt->smtpReadDcpt.pReadObj->deinit(&pDcpt->smtpReadDcpt);
        pDcpt->nSmtpAttachBuffers++;
        pDcpt->smtpAttachBuffer++;
        newStat = TCPIP_SMTPC_STAT_MAIL_MSG_BUFF_HDR;
    } 

    return newStat;
}

// TCPIP_SMTPC_STAT_MAIL_MSG_FILE_HDR
static TCPIP_SMTPC_STATUS smtpDcptMailMsgFileHdr(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    bool fileDone = false;

    if(pDcpt->nSmtpAttachFiles == pDcpt->smtpMailMessage.nFiles)
    {   // we're done; jump to sending files
        fileDone =  true;
    }
    else if(pDcpt->nSmtpAttachFiles == 0)
    {   // just starting; prepare the file attachments
        if(pDcpt->smtpMailMessage.attachFiles != 0 && pDcpt->smtpMailMessage.nFiles != 0)
        {
            pDcpt->smtpAttachFile = pDcpt->smtpMailMessage.attachFiles;
        }
        else
        {
            fileDone = true;
        }
    }

    if(fileDone)
    {   // we're done; skip the TCPIP_SMTPC_STAT_MAIL_MSG_FILE_BODY
        return TCPIP_SMTPC_STAT_MAIL_MSG_FILE_HDR + 2;
    }

    // send this file
    if((pDcpt->smtpAttachFile->attachType & TCPIP_SMTPC_ATTACH_TYPE_TEXT) != 0)
    {
        pDcpt->smtpPartAttachType = TCPIP_SMTPC_ATTACH_TYPE_TEXT;
        pDcpt->smtpPartEncodeType = pDcpt->smtpAttachFile->attachEncode ==  TCPIP_SMTPC_ENCODE_TYPE_7BIT ? TCPIP_SMTPC_ENCODE_TYPE_7BIT: TCPIP_SMTPC_ENCODE_TYPE_BASE64; 
    }
    else
    {
        pDcpt->smtpPartAttachType = TCPIP_SMTPC_ATTACH_TYPE_APPLICATION;
        pDcpt->smtpPartEncodeType = TCPIP_SMTPC_ENCODE_TYPE_BASE64; 
    }

    if((pDcpt->smtpAttachFile->attachType & TCPIP_SMTPC_ATTACH_TYPE_INLINE) != 0)
    {
        pDcpt->smtpPartAttachType |= TCPIP_SMTPC_ATTACH_TYPE_INLINE; 
    }

    memset(&pDcpt->smtpReadDcpt, 0, sizeof(pDcpt->smtpReadDcpt));
    pDcpt->smtpReadDcpt.fDcpt.fHandle = SYS_FS_HANDLE_INVALID;
    pDcpt->smtpReadDcpt.pReadObj = &smtpReadFileObj;
    if(!pDcpt->smtpReadDcpt.pReadObj->init(&pDcpt->smtpReadDcpt, pDcpt->smtpAttachFile->fileName, 0, 0))
    {   // couldn't find this file
        smtpcSetWarning(pDcpt, TCPIP_SMTPC_WARN_FILE_NOT_FOUND);
        pDcpt->nSmtpAttachFiles++;
        pDcpt->smtpAttachFile++;
        // try the next one
        return TCPIP_SMTPC_STAT_MAIL_MSG_FILE_HDR;
    }

    return smtpSendPartHdr(pDcpt, pDcpt->smtpAttachFile->fileName);
}

// TCPIP_SMTPC_STAT_MAIL_MSG_FILE_BODY
// send the file body
static TCPIP_SMTPC_STATUS smtpDcptMailMsgFileBody(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    TCPIP_SMTPC_STATUS newStat = smtpSendPartBody(pDcpt);

    if(newStat == TCPIP_SMTPC_STAT_MAIL_MSG_FILE_BODY + 1)
    {   // done; send another file
        pDcpt->smtpReadDcpt.pReadObj->deinit(&pDcpt->smtpReadDcpt);
        pDcpt->nSmtpAttachFiles++;
        pDcpt->smtpAttachFile++;
        newStat = TCPIP_SMTPC_STAT_MAIL_MSG_FILE_HDR;
    } 

    return newStat;
}

// TCPIP_SMTPC_STAT_MAIL_DONE
// send mail done/ended to the server
static TCPIP_SMTPC_STATUS smtpDcptMailMsgDone(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    char endBuff[100];
    sprintf(endBuff, "\r\n--%s--\r\n.\r\n", smtpcMailBoundary);

    smtpcSetDcptTmo(pDcpt, smtpcServerDataTmo);
    return smtpClientWriteCmd(pDcpt, endBuff, 0);
}


// TCPIP_SMTPC_STAT_MAIL_DONE_REPORT
static TCPIP_SMTPC_STATUS smtpDcptMailDoneReport(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{

    if(pDcpt->smtpMailMessage.messageCallback != 0)
    {   // report the outcome to the user 
        TCPIP_SMTPC_MESSAGE_REPORT msgReport;

        msgReport.leftRetries = 0;
        if((msgReport.messageRes = pDcpt->messageRes) != TCPIP_SMTPC_RES_OK)
        {   // failed somehow
            if((pDcpt->dcptFlags & TCPIP_SMTPC_DCPT_FLAG_RETRY_MASK) != TCPIP_SMTPC_DCPT_FLAG_RETRY_NONE)
            {   // can be retried
                msgReport.leftRetries = pDcpt->retryCnt - 1;
            }
        }
        
        msgReport.errorStat = pDcpt->errorStat ? smtpStatusTxlate(pDcpt->errorStat) : TCPIP_SMTPC_MESSAGE_STAT_NONE;
        msgReport.messageWarn = pDcpt->messageWarn;
        (*pDcpt->smtpMailMessage.messageCallback)(pDcpt, &msgReport);
    }

    smtpcSetDcptTmo(pDcpt, smtpcServerGenTmo);

    if(pDcpt->messageRes == TCPIP_SMTPC_RES_OK)
    {   // everything went on just fine
        // just quit (for now, at least; if there are more transactions pending
        // with same server/credentials, we may choose to maintain the server connection open
        // to avoid all the authentication handshake...
        return TCPIP_SMTPC_STAT_TRANSACTION_QUIT;
    }

    // something went wrong
    if((pDcpt->dcptFlags & TCPIP_SMTPC_DCPT_FLAG_EHLO) != 0)
    {   // we helloed the server; reset the transaction
        return TCPIP_SMTPC_STAT_TRANSACTION_RESET;
    }

    // failed before getting server hello; terminate it
    return TCPIP_SMTPC_STAT_TRANSACTION_CLOSE;
}

// TCPIP_SMTPC_STAT_TRANSACTION_RESET
// send reset to server
static TCPIP_SMTPC_STATUS smtpDcptTransactionReset(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    if((pDcpt->dcptFlags & TCPIP_SMTPC_DCPT_FLAG_EHLO) != 0)
    {   // tmo error should take us to quit 
        smtpcSetErrorJump(pDcpt, TCPIP_SMTPC_STAT_TRANSACTION_QUIT);
        TCPIP_SMTPC_STATUS newStat = smtpCommandParam(pDcpt, TCPIP_SMTPC_CMD_RESET, 0, smtpcServerGenTmo);
        if(newStat == pDcpt->currStat && smtpcIsDcptTmo(pDcpt))
        {   // make sure we're not stuck here trying to send the reset...
            return TCPIP_SMTPC_STAT_TRANSACTION_CLOSE;
        }

        return newStat;
    }

    // if we're not helloed...
    return TCPIP_SMTPC_STAT_TRANSACTION_CLOSE;
}

// TCPIP_SMTPC_STAT_TRANSACTION_QUIT
// send quit to server
static TCPIP_SMTPC_STATUS smtpDcptTransactionQuit(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    if((pDcpt->dcptFlags & TCPIP_SMTPC_DCPT_FLAG_EHLO) != 0)
    {
        // tmo error should take us to close 
        smtpcSetErrorJump(pDcpt, TCPIP_SMTPC_STAT_TRANSACTION_CLOSE);
        TCPIP_SMTPC_STATUS newStat = smtpCommandParam(pDcpt, TCPIP_SMTPC_CMD_QUIT, 0, smtpcServerGenTmo);
        if(newStat == pDcpt->currStat && smtpcIsDcptTmo(pDcpt))
        {   // make sure we're not stuck here trying to send the quit...
            return TCPIP_SMTPC_STAT_TRANSACTION_CLOSE;
        }

        return newStat;
    }

    // if we're not helloed...
    return TCPIP_SMTPC_STAT_TRANSACTION_CLOSE;
}

// TCPIP_SMTPC_STAT_TRANSACTION_CLOSE
// close the transaction...
static TCPIP_SMTPC_STATUS smtpDcptTransactionClose(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    TCPIP_SMTPC_STATUS newStat;

    if(pDcpt->skt != NET_PRES_INVALID_SOCKET)
    {
        NET_PRES_SocketClose(pDcpt->skt);
        pDcpt->skt = NET_PRES_INVALID_SOCKET;
    } 
    
    newStat = TCPIP_SMTPC_STAT_FREED;

    if(pDcpt->messageRes != TCPIP_SMTPC_RES_OK)
    {   // failed somehow
        if(pDcpt->retryCnt != 0 && (pDcpt->dcptFlags & TCPIP_SMTPC_DCPT_FLAG_RETRY_MASK) != TCPIP_SMTPC_DCPT_FLAG_RETRY_NONE)
        {   // can be retried
            smtpcSetDcptTmo(pDcpt, (pDcpt->dcptFlags & TCPIP_SMTPC_DCPT_FLAG_RETRY_SERVER) != 0 ? smtpcServerRetryTmo : smtpcInternalRetryTmo);
            pDcpt->retryCnt--;
            newStat = TCPIP_SMTPC_STAT_WAIT_RETRY;
        }
    }

    // done 
    return newStat;
}


// RX processing functions
//

// process incoming data and check the timeout
// mail messages are serialized internally
// we use just one global RX buffer for now
// TCPIP_SMTPC_STAT_SKT_WAIT_CONNECT;
// TCPIP_SMTPC_STAT_EHLO_PLAIN_WAIT; TCPIP_SMTPC_STAT_TLS_WAIT; TCPIP_SMTPC_STAT_EHLO_SECURE_WAIT;
// TCPIP_SMTPC_STAT_AUTH_WAIT_LOGIN_USER; TCPIP_SMTPC_STAT_AUTH_WAIT_LOGIN_PASS; TCPIP_SMTPC_STAT_AUTH_WAIT_FINAL;
// TCPIP_SMTPC_STAT_MAIL_ENVL_FROM_WAIT; TCPIP_SMTPC_STAT_MAIL_ENVL_RCPT_WAIT: TCPIP_SMTPC_STAT_MAIL_ENVL_DATA_WAIT
// TCPIP_SMTPC_STAT_MAIL_DONE_WAIT; TCPIP_SMTPC_STAT_TRANSACTION_RESET_WAIT; TCPIP_SMTPC_STAT_TRANSACTION_QUIT_WAIT
static TCPIP_SMTPC_STATUS smtpDcptStateWaitReply(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    char* currRxPtr;
    int      linesNo;
    char    *eolPtr;
    char* lastLinePtr;
    uint16_t avlblBytes, peekBytes;
    int     serverCode;
    char    ch;
    TCPIP_SMTPC_STATUS procStat;

    while((avlblBytes = NET_PRES_SocketReadIsReady(pDcpt->skt)) != 0)
    {
        if(avlblBytes > sizeof(smtpcRxBuffer))
        {   // server replied more than we could store...warning
            smtpcSetWarning(pDcpt, TCPIP_SMTPC_WARN_REPLY_BUFFER_SMALL);
        }

        // using the socket buffer; the smtpcRxBuffer is shared by all mail messages!
        peekBytes = NET_PRES_SocketPeek(pDcpt->skt,  smtpcRxBuffer, sizeof(smtpcRxBuffer) - 1);

        // end properly
        *(smtpcRxBuffer + peekBytes) = 0;

        // check for properly ended lines in the buffer
        // we assume that the server correctly repeats the code on all lines!
        linesNo = 0;
        lastLinePtr = 0;
        currRxPtr = smtpcRxBuffer;
        while(true)
        {
            eolPtr = strstr(currRxPtr, "\r\n");
            if(eolPtr == 0)
            {   // no more
                break;
            }
            lastLinePtr = currRxPtr;
            linesNo++;
            // advance to the next line
            currRxPtr = eolPtr + 2; // strlen("\r\n");
        }

        if(linesNo == 0)
        {   // no full lines yet
            break;
        }

        // there are some lines in the buffer; search for the end line
        serverCode = atoi(lastLinePtr);
        if(serverCode == 0)
        {   // that's weird
            _SMTPCDebugCond(false, __func__, __LINE__);
            smtpcSetWarning(pDcpt, TCPIP_SMTPC_WARN_WRONG_FORMAT);
            break; 
        }

        // check it is the last line
        while(isdigit((ch = *lastLinePtr++)));
        if(ch != ' ')
        {   // last line not in buffer yet
            break; 
        }

        // ok got a full buffer that needs to be processed
        // release the socket buffer
        NET_PRES_SocketRead(pDcpt->skt, smtpcRxBuffer, peekBytes);
        _SMTPCDbgMailStrFromSrv(pDcpt, smtpcRxBuffer);

        // process
        _SMTPCAssertCond(0 <= pDcpt->currStat && pDcpt->currStat <= sizeof(smtpRxProcessTbl) / sizeof(*smtpRxProcessTbl), __func__, __LINE__);
        procStat = (*smtpRxProcessTbl[pDcpt->currStat])(pDcpt, smtpcRxBuffer, linesNo);

        if(pDcpt->smtpMailMessage.replyCallback != 0)
        {   // user wants to be informed of the server messages
            (*pDcpt->smtpMailMessage.replyCallback)(pDcpt, smtpStatusTxlate(pDcpt->currStat), smtpcRxBuffer);
        }

        return procStat;
    }

    // check for timeout
    if(smtpcIsDcptTmo(pDcpt))
    {
        return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_SERVER_TMO, TCPIP_SMTPC_DCPT_FLAG_RETRY_SERVER);
    }

    // wait some more
    return pDcpt->currStat;
}
// unexpected reply
// execution should not get here!
static TCPIP_SMTPC_STATUS smtpDcptRxProcNone(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines)
{
    _SMTPCAssertCond(false, __func__, __LINE__);
    // discard all data!
    NET_PRES_SocketDiscard(pDcpt->skt);
    smtpcSetWarning(pDcpt, TCPIP_SMTPC_WARN_UNEXPECTED_REPLY);

    // but continue
    return pDcpt->currStat;
}

// TCPIP_SMTPC_STAT_SKT_WAIT_CONNECT
// process server connect message
static TCPIP_SMTPC_STATUS smtpDcptRxProcConnect(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines)
{
    TCPIP_SMTPC_STATUS newOkStat = TCPIP_SMTPC_STAT_EHLO_PLAIN;

    // received some reply from the server
    // however, if we're forced to connect securely, check first that we have that
    if((pDcpt->smtpMailMessage.messageFlags &TCPIP_SMTPC_MAIL_FLAG_CONNECT_TLS) != 0)
    {
        if(!NET_PRES_SocketIsSecure(pDcpt->skt))
        {   // negotiation failed; fatal
            return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_TLS_FAILED, TCPIP_SMTPC_DCPT_FLAG_RETRY_SMPTC);
        }

        pDcpt->dcptFlags |= TCPIP_SMTPC_DCPT_FLAG_SECURED;
        newOkStat = TCPIP_SMTPC_STAT_EHLO_SECURE; 
    }

    pDcpt->dcptFlags |= TCPIP_SMTPC_DCPT_FLAG_CONNECTED;

    int serverReplyCode = atoi(replyBuffer);

    if(serverReplyCode != TCPIP_SMTP_CONNECT_SUCCESS_CODE)
    {   // server doesn't accept this connection
        return smtpcServerErrorStop(pDcpt, TCPIP_SMTPC_RES_CONNECTION_REJECT, serverReplyCode);
    }

    // advance
    return newOkStat;
}


// process the server EHLO reply
// TCPIP_SMTPC_STAT_EHLO_PLAIN_WAIT; TCPIP_SMTPC_STAT_EHLO_SECURE_WAIT
static TCPIP_SMTPC_STATUS smtpDcptRxProcEhlo(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines)
{
    int lineIx;
    const char *linePtr, *currPtr, *eolPtr;
    int ch;

    TCPIP_SMTPC_DCPT_FLAGS  srchFlags;
    
    int serverCode = atoi(replyBuffer);

    if(serverCode != TCPIP_SMTP_EHLO_SUCCESS_CODE)
    {   // server doesn't fancy our greeting message..
        return smtpcServerErrorStop(pDcpt, TCPIP_SMTPC_RES_HELLO_REJECT, serverCode);
    }

    // all seems good; see what we got
    pDcpt->dcptFlags |= TCPIP_SMTPC_DCPT_FLAG_EHLO;
    // clear all authentication 
    pDcpt->dcptFlags &= ~TCPIP_SMTPC_SERVER_FLAG_AUTH_MASK;

    // this is what we're looking for
    srchFlags = TCPIP_SMTPC_SERVER_FLAG_TLS | TCPIP_SMTPC_SERVER_FLAG_AUTH;

    linePtr = replyBuffer;
    for(lineIx = 0; lineIx < nLines && (pDcpt->dcptFlags & srchFlags) != srchFlags; lineIx++)
    {
        currPtr = linePtr;
        eolPtr = strstr(currPtr, "\r\n");
        if(eolPtr == 0)
        {   // no more ?
            break;
        }
        
        // skip the reply code and the delimiting chars
        while(isdigit((ch = *currPtr)) || isspace(ch) || ch == '-' )
        {
            currPtr++;
        }

        // search through the codes that interest us
        // check for TLS
        if((pDcpt->dcptFlags & TCPIP_SMTPC_SERVER_FLAG_TLS) == 0 && strncmp(currPtr, "STARTTLS", strlen("STARTTLS")) == 0)
        {   
            pDcpt->dcptFlags |= TCPIP_SMTPC_SERVER_FLAG_TLS;
        }
        else if((pDcpt->dcptFlags & TCPIP_SMTPC_SERVER_FLAG_AUTH) == 0 && strncmp(currPtr, "AUTH", strlen("AUTH")) == 0)
        {   // found the AUTH type
            pDcpt->dcptFlags |= TCPIP_SMTPC_SERVER_FLAG_AUTH;
            currPtr += strlen("AUTH");

            do
            {
                // skip spaces
                while(currPtr != eolPtr && isspace(*currPtr))
                {
                    currPtr++;
                }

                if(currPtr != eolPtr)
                {
                    if((pDcpt->dcptFlags & TCPIP_SMTPC_SERVER_FLAG_AUTH_LOGIN) == 0 && strncmp(currPtr, "LOGIN", strlen("LOGIN")) == 0)
                    {
                        pDcpt->dcptFlags |= TCPIP_SMTPC_SERVER_FLAG_AUTH_LOGIN;
                        currPtr += strlen("LOGIN");
                    }
                    else if((pDcpt->dcptFlags & TCPIP_SMTPC_SERVER_FLAG_AUTH_PLAIN) == 0 && strncmp(currPtr, "PLAIN", strlen("PLAIN")) == 0)
                    {
                        pDcpt->dcptFlags |= TCPIP_SMTPC_SERVER_FLAG_AUTH_PLAIN;
                        currPtr += strlen("PLAIN");
                    }
                    else if((pDcpt->dcptFlags & TCPIP_SMTPC_SERVER_FLAG_AUTH_MD5) == 0 && strncmp(currPtr, "CRAM-MD5", strlen("CRAM-MD5")) == 0)
                    {
                        pDcpt->dcptFlags |= TCPIP_SMTPC_SERVER_FLAG_AUTH_MD5;
                        currPtr += strlen("CRAM-MD5");
                    }
                    else
                    {   // ignore
                        while(currPtr != eolPtr && !isspace((ch = *currPtr)))
                        {
                            currPtr++;
                        }
                    }
                }
            }while(currPtr != eolPtr);

            // check if the server hasn't indicated any authentication method
            if((pDcpt->dcptFlags & TCPIP_SMTPC_SERVER_FLAG_AUTH_SUPPORTED_MASK) == 0)
            {
                if((pDcpt->smtpMailMessage.messageFlags & TCPIP_SMTPC_MAIL_FLAG_FORCE_AUTH) != 0)
                {   // select a default auth scheme
                    pDcpt->dcptFlags |=  (pDcpt->smtpMailMessage.messageFlags & TCPIP_SMTPC_MAIL_FLAG_AUTH_PLAIN) != 0 ? TCPIP_SMTPC_SERVER_FLAG_AUTH_PLAIN : TCPIP_SMTPC_SERVER_FLAG_AUTH_LOGIN;
                }
                else
                {   // no auth...
                    pDcpt->dcptFlags &= ~TCPIP_SMTPC_SERVER_FLAG_AUTH;
                }
            }
        }

        // advance to the next line
        linePtr = eolPtr + 2;  // strlen("\r\n"); 
    }

    // start TLS?
    if(pDcpt->currStat == TCPIP_SMTPC_STAT_EHLO_PLAIN_WAIT && (pDcpt->smtpMailMessage.messageFlags & TCPIP_SMTPC_MAIL_FLAG_SKIP_TLS) == 0) 
    {
        if((pDcpt->smtpMailMessage.messageFlags & TCPIP_SMTPC_MAIL_FLAG_FORCE_TLS) != 0 || (pDcpt->dcptFlags & TCPIP_SMTPC_SERVER_FLAG_TLS) != 0)
        {   // start TLS
            return TCPIP_SMTPC_STAT_TLS_START;
        } 
    }

    if((pDcpt->dcptFlags & TCPIP_SMTPC_SERVER_FLAG_AUTH) != 0)
    {   // server wants authentication
        if((pDcpt->dcptFlags & TCPIP_SMTPC_SERVER_FLAG_AUTH_SUPPORTED_MASK) == 0)
        {   // none of the authentication we support...
            return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_AUTH_UNKNOWN, TCPIP_SMTPC_DCPT_FLAG_RETRY_NONE);
        }
        // proceed with the authentication
        return TCPIP_SMTPC_STAT_AUTH;
    }


    // proceed directly to mail
    return TCPIP_SMTPC_STAT_MAIL_START;
}

// process the server reply to STARTTLS
// TCPIP_SMTPC_STAT_TLS_WAIT
static TCPIP_SMTPC_STATUS smtpDcptRxProcTls(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines)
{
    int serverReplyCode = atoi(replyBuffer);

    if(serverReplyCode != TCPIP_SMTP_STARTTLS_SUCCESS_CODE)
    {   // server doesn't want to start TLS???
        return smtpcServerErrorStop(pDcpt, TCPIP_SMTPC_RES_TLS_REJECT, serverReplyCode);
    }

    if(!NET_PRES_SocketEncryptSocket(pDcpt->skt))
    {   // could not start encryption?
        return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_SKT_TLS_ERROR, TCPIP_SMTPC_DCPT_FLAG_RETRY_SMPTC);
    }

    // all fine
    smtpcSetDcptTmo(pDcpt, smtpcTlsHandshakeTmo);
    return TCPIP_SMTPC_STAT_TLS_WAIT_SECURE;
}

// TCPIP_SMTPC_STAT_AUTH_WAIT_LOGIN_USER
// wait for server to request the LOGIN user
static TCPIP_SMTPC_STATUS smtpDcptRxProcAuthLoginUser(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines)
{
    char    authBuff[20];
    const char*   pBuff;
    char*        crPos;
    uint16_t base64Len;

    
    int serverReplyCode = atoi(replyBuffer);

    if(serverReplyCode != TCPIP_SMTP_AUTH_LOGIN_USER_SUCCESS_CODE)
    {   // server not happy
        return smtpcServerErrorStop(pDcpt, TCPIP_SMTPC_RES_AUTH_LOGIN_REJECT, serverReplyCode);
    }

    // OK, server is happy
    pBuff = replyBuffer;
    while(isdigit(*pBuff) || isspace(*pBuff))
    {
        pBuff++;
    }
    crPos = strchr(pBuff, '\r');


    if(crPos != 0)
    {
        memset(authBuff, 0, sizeof(authBuff));
        base64Len = TCPIP_Helper_Base64Decode((uint8_t*)pBuff, crPos - pBuff, (uint8_t*)authBuff, sizeof(authBuff) - 1);
        authBuff[base64Len] = 0;
        if(strcmp(authBuff, "Username:") == 0)
        {   // all fine
            return TCPIP_SMTPC_STAT_AUTH_WAIT_LOGIN_USER + 1;
        }
    }


    // something went wrong
    return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_AUTH_LOGIN_SERVER_ERROR, TCPIP_SMTPC_DCPT_FLAG_RETRY_SERVER);
}


// TCPIP_SMTPC_STAT_AUTH_WAIT_LOGIN_PASS
static TCPIP_SMTPC_STATUS smtpDcptRxProcAuthLoginPass(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines)
{
    char    authBuff[20];
    const char*   pBuff;
    char*        crPos;
    uint16_t base64Len;

    int serverReplyCode = atoi(replyBuffer);

    if(serverReplyCode != TCPIP_SMTP_AUTH_LOGIN_USER_SUCCESS_CODE)
    {   // server not happy
        return smtpcServerErrorStop(pDcpt, TCPIP_SMTPC_RES_AUTH_LOGIN_REJECT, serverReplyCode);
    }

    // OK, server is happy
    pBuff = replyBuffer;
    while(isdigit(*pBuff) || isspace(*pBuff))
    {
        pBuff++;
    }
    crPos = strchr(pBuff, '\r');


    if(crPos != 0)
    {
        memset(authBuff, 0, sizeof(authBuff));
        base64Len = TCPIP_Helper_Base64Decode((uint8_t*)pBuff, crPos - pBuff, (uint8_t*)authBuff, sizeof(authBuff) - 1);
        authBuff[base64Len] = 0;
        if(strcmp(authBuff, "Password:") == 0)
        {   // all fine
            return TCPIP_SMTPC_STAT_AUTH_WAIT_LOGIN_PASS + 1;
        }
    }

    // something went wrong
    return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_AUTH_LOGIN_SERVER_ERROR, TCPIP_SMTPC_DCPT_FLAG_RETRY_SERVER);
}

// TCPIP_SMTPC_STAT_AUTH_WAIT_FINAL
static TCPIP_SMTPC_STATUS smtpDcptRxProcAuthFinal(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines)
{
    int serverReplyCode = atoi(replyBuffer);

    if(serverReplyCode != TCPIP_SMTP_AUTH_FINAL_SUCCESS_CODE)
    {   // server doesn't recognize our AUTH!
        return smtpcServerErrorStop(pDcpt, TCPIP_SMTPC_RES_AUTH_REJECT, serverReplyCode);
    }

    // all fine; proceed with mail
    return TCPIP_SMTPC_STAT_MAIL_START;
}

// TCPIP_SMTPC_STAT_MAIL_ENVL_FROM_WAIT
static TCPIP_SMTPC_STATUS smtpDcptRxProcMailFrom(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines)
{
    int serverReplyCode = atoi(replyBuffer);

    if(serverReplyCode != TCPIP_SMTP_MAIL_FROM_SUCCESS_CODE)
    {   // server rejected mail from
        return smtpcServerErrorStop(pDcpt, TCPIP_SMTPC_RES_MAIL_FROM_REJECT, serverReplyCode);
    }

    // all fine
    return TCPIP_SMTPC_STAT_MAIL_ENVL_RCPT;

}

// TCPIP_SMTPC_STAT_MAIL_ENVL_RCPT_WAIT
static TCPIP_SMTPC_STATUS smtpDcptRxProcMailRcpt(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines)
{
    int serverReplyCode = atoi(replyBuffer);

    if(serverReplyCode != TCPIP_SMTP_MAIL_RCPT_SUCCESS_CODE && serverReplyCode != TCPIP_SMTP_MAIL_RCPT_SUCCESS_CODE_ALT)
    {   // server rejected mail from
        return smtpcServerErrorStop(pDcpt, TCPIP_SMTPC_RES_MAIL_RCPT_REJECT, serverReplyCode);
    }

    // all fine; proceed with data
    return TCPIP_SMTPC_STAT_MAIL_ENVL_DATA;
}

// TCPIP_SMTPC_STAT_MAIL_ENVL_DATA_WAIT
static TCPIP_SMTPC_STATUS smtpDcptRxProcMailData(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines)
{
    int serverReplyCode = atoi(replyBuffer);

    if(serverReplyCode != TCPIP_SMTP_MAIL_DATA_SUCCESS_CODE)
    {   // server rejected mail data
        return smtpcServerErrorStop(pDcpt, TCPIP_SMTPC_RES_MAIL_DATA_REJECT, serverReplyCode);
    }

    // all fine
    return TCPIP_SMTPC_STAT_MAIL_MSG_HDR_DATE;
    
}

// TCPIP_SMTPC_STAT_MAIL_DONE_WAIT
static TCPIP_SMTPC_STATUS smtpDcptRxProcMailDoneWait(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines)
{
    int serverReplyCode = atoi(replyBuffer);
    if(serverReplyCode != TCPIP_SMTP_MAIL_SENT_SUCCESS_CODE)
    {   // finally it failed...
        return smtpcServerErrorStop(pDcpt, TCPIP_SMTPC_RES_MAIL_BODY_REJECT, serverReplyCode);
    }

    // mark it as successful
    pDcpt->messageRes = TCPIP_SMTPC_RES_OK;
    return TCPIP_SMTPC_STAT_MAIL_DONE_WAIT + 1;
}

// TCPIP_SMTPC_STAT_TRANSACTION_RESET_WAIT
// process the server reply to reset
static TCPIP_SMTPC_STATUS smtpDcptRxProcTransactResetWait(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines)
{
    // no matter what the server replies here we move on
    return TCPIP_SMTPC_STAT_TRANSACTION_RESET_WAIT + 1;
}

// TCPIP_SMTPC_STAT_TRANSACTION_QUIT_WAIT
// process the server reply to quit
static TCPIP_SMTPC_STATUS smtpDcptRxProcTransactQuitWait(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, int nLines)
{
    // no matter what the server replies here we move on
    return TCPIP_SMTPC_STAT_TRANSACTION_QUIT_WAIT + 1;
}


// internal helpers 
//

// sends SMTP command with one parameter
// set timeout if serverTmo != 0
// returns:
//      - same state: if need to wait
//      - next state: if command went through
//      - smtpcErrorStop i.e. errorJumpStat: if smth went wrong
static TCPIP_SMTPC_STATUS smtpCommandParam(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, TCPIP_SMTPC_CMD_TYPE cmdType, const char* cmdArg, uint32_t serverTmo)
{
    const char* smtpCmdStr;
    TCPIP_SMTPC_STATUS newStat;
    
    if(cmdType == TCPIP_SMTPC_CMD_NONE)
    {   // don't issue a command string, just the arg
        smtpCmdStr = 0;
    }
    else
    {
        _SMTPCAssertCond(0 < cmdType && cmdType < sizeof(smtpCmdStrTbl) / sizeof(*smtpCmdStrTbl), __func__, __LINE__);
        smtpCmdStr = smtpCmdStrTbl[cmdType];
        if(!_SMTPCAssertCond(smtpCmdStr != 0 && strlen(smtpCmdStr) != 0, __func__, __LINE__))
        {   // should not happen!
            return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_INTERNAL_ERROR, TCPIP_SMTPC_DCPT_FLAG_RETRY_NONE);
        }
    }

    newStat = smtpClientWriteCmd(pDcpt, smtpCmdStr, cmdArg);

    if(newStat == pDcpt->currStat + 1)
    {   // succeeded; advanced
        if(serverTmo)
        {
            smtpcSetDcptTmo(pDcpt, serverTmo);
        }
    }

    return newStat;
}




// writes "cmdStr" or "cmdStr cmdArg" or "cmdArg" to the output socket
// returns:
//      - same state: if need to wait
//      - next state: if command went through
//      - smtpcErrorStop i.e. errorJumpStat: if smth went wrong
static TCPIP_SMTPC_STATUS smtpClientWriteCmd(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* cmdStr, const char* cmdArg)
{
    int nBytes;
    char sBuff[200];

    if(cmdStr == 0 && cmdArg == 0)
    {   // should NOT happen!
        _SMTPCAssertCond(false, __func__, __LINE__);
        return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_INTERNAL_ERROR, TCPIP_SMTPC_DCPT_FLAG_RETRY_NONE);
    }


    if(cmdStr == 0)
    {   // output just the arg
        nBytes = sprintf(sBuff, "%s\r\n", cmdArg); 
    }
    else if(cmdArg != 0)
    {
        nBytes = sprintf(sBuff, "%s %s\r\n", cmdStr, cmdArg); 
    }
    else
    {
        nBytes = sprintf(sBuff, "%s\r\n", cmdStr); 
    }


    if(NET_PRES_SocketWriteIsReady(pDcpt->skt, nBytes, 0) >= nBytes)
    {
        int sentBytes = NET_PRES_SocketWrite(pDcpt->skt, sBuff, nBytes);
        _SMTPCAssertCond(sentBytes == nBytes, __func__, __LINE__);
        NET_PRES_SocketFlush(pDcpt->skt);
        _SMTPCDbgMailStrToSrv(pDcpt, (char*)sBuff, nBytes);
        // advance
        return pDcpt->currStat + 1;
    }

    // wait some more
    return pDcpt->currStat;
}

// formats a mail address into the supplied buffer
// buffer needs to be large enough to take additional <>\0 characters!
static bool smtpFormatMailAddress(const char* mailAddr, char* mailAddrBuff, int buffSize)
{
    if(mailAddr)
    {
        if(strlen(mailAddr) <= (buffSize - 3))
        {
            mailAddrBuff[buffSize - 1] = 0;
            sprintf(mailAddrBuff, "<%s>", mailAddr);
            return true;
        }
    }

    return false;
}

// writes an envelope address to the output socket
//  "MAIL FROM:<add>", "RCPT TO:<add>"
static TCPIP_SMTPC_STATUS   smtpWriteMailEnvAddress(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, TCPIP_SMTPC_ENVELOPE envType)
{
    char  mailAddrBuff[TCPIP_SMTPC_CLIENT_ADDR_BUFFER_SIZE + 3];     // extra space: < > \0 

    const char* addStr = envType == TCPIP_SMTPC_ENV_FROM ? pDcpt->smtpMailMessage.from : pDcpt->smtpMailMessage.to;
    if(!smtpFormatMailAddress(addStr, mailAddrBuff, sizeof(mailAddrBuff)))
    {   // failed: too long
        return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_MESSAGE_ADDR_LEN_ERROR, TCPIP_SMTPC_DCPT_FLAG_RETRY_NONE);
    }

    return smtpCommandParam(pDcpt, envType == TCPIP_SMTPC_ENV_FROM ? TCPIP_SMTPC_CMD_MAIL_FROM : TCPIP_SMTPC_CMD_MAIL_RCPT, mailAddrBuff, smtpcServerGenTmo);

}


// puts the EHLO address literal for the message descriptor in format
// "[ipv4 dot address]" or "[ipv6 hex address]" into the supplied buffer
// the socket needs to be connected!
// the buffer needs to accomodate an IPv6 address + extra 3 characters,
// i.e. >= 44 characters in size 
static bool smtpSetEhloString(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, char* pAddBuff, size_t buffSize)
{
    bool convSuccess;
    TCP_SOCKET_INFO sktInfo;
    char* pEndBuff;

    // get the interface over which we're connected
    NET_PRES_SocketInfoGet(pDcpt->skt, &sktInfo);

    if(sktInfo.addressType == IP_ADDRESS_TYPE_IPV4)
    {
        convSuccess = TCPIP_Helper_IPAddressToString(&sktInfo.localIPaddress.v4Add, pAddBuff + 1, buffSize - 3);
    }
    else
    {
        convSuccess = TCPIP_Helper_IPv6AddressToString (&sktInfo.localIPaddress.v6Add, pAddBuff + 1, buffSize - 3);
    }

    if(!convSuccess)
    {   // failed; should not happen
        _SMTPCAssertCond(false, __func__, __LINE__);
        return false;
    }

    // OK
    *pAddBuff = '[';
    pEndBuff = pAddBuff + strlen(pAddBuff);
    *pEndBuff++ = ']';
    *pEndBuff = 0;

    return true;
}

// sets the waitTick to the timeout value (in seconds!)
static void smtpcSetDcptTmo(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, uint32_t tmo)
{
    pDcpt->waitTick = SYS_TMR_TickCountGet() + (tmo * SYS_TMR_TickCounterFrequencyGet());
}

// returns true if timeout
static bool smtpcIsDcptTmo(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    return (int32_t)(SYS_TMR_TickCountGet() - pDcpt->waitTick) >= 0;
}

// an error of some sort occurred and processing is aborted
// sets the result and updates the retry flags
static TCPIP_SMTPC_STATUS smtpcErrorStop(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, TCPIP_SMTPC_MESSAGE_RESULT res, TCPIP_SMTPC_DCPT_FLAGS retryFlags)
{
    _SMTPCDbgDcptErrorState(pDcpt, res);
    pDcpt->errorStat = pDcpt->currStat;
    pDcpt->messageRes = res;
    pDcpt->dcptFlags &= ~(TCPIP_SMTPC_DCPT_FLAG_RETRY_MASK);
    pDcpt->dcptFlags |= retryFlags;
    return pDcpt->errorJumpStat;
}

// error, but caused by the server reply
static TCPIP_SMTPC_STATUS smtpcServerErrorStop(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, TCPIP_SMTPC_MESSAGE_RESULT res, int serverCode)
{
    return smtpcErrorStop(pDcpt, res, smtpcServerCodeRetryFlag(serverCode));
}

// initializes a descriptor for a retry round
static void smtpcRetryInit(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    pDcpt->messageRes = TCPIP_SMTPC_RES_PENDING;
    pDcpt->messageWarn = 0;
    pDcpt->errorStat = 0;
    pDcpt->dcptFlags = TCPIP_SMTPC_DCPT_FLAG_NONE;
    smtpcSetErrorJump(pDcpt, TCPIP_SMTPC_STAT_MAIL_DONE_REPORT);
}

static TCPIP_SMTPC_STATUS smtpSendPartHdr(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* fileName)
{
    char partHdr[200];
    size_t hdrLen;
    const char* cntntType;
    const char* encType;
    const char* cntntDisp = 0;


    if((pDcpt->smtpPartAttachType & TCPIP_SMTPC_ATTACH_TYPE_APPLICATION) != 0)
    {
        cntntType = "application/octet-stream";
    }
    else
    {
        cntntType = "text/plain";
    }

    if(pDcpt->smtpPartEncodeType == TCPIP_SMTPC_ENCODE_TYPE_7BIT)
    {
        encType = "7Bit";
    } 
    else
    {
        encType = "base64";
    } 

    if(fileName != 0)
    {   // add content disposition
        if((pDcpt->smtpPartAttachType & TCPIP_SMTPC_ATTACH_TYPE_INLINE) != 0)
        {
           cntntDisp = "inline";
        }
        else
        {
           cntntDisp = "attachment";
        } 
    }

    hdrLen = sprintf(partHdr, "\r\n--%s\r\nContent-type: %s\r\nContent-Transfer-Encoding: %s\r\n", smtpcMailBoundary, cntntType, encType);
    if(cntntDisp != 0)
    {
        snprintf(partHdr + hdrLen, sizeof(partHdr) - hdrLen, "Content-Disposition: %s; filename=\"%s\"\r\n", cntntDisp, fileName);
    }

    return smtpClientWriteCmd(pDcpt, partHdr, 0);
}

static TCPIP_SMTPC_STATUS smtpSendPartBody(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    if(pDcpt->smtpPartEncodeType == TCPIP_SMTPC_ENCODE_TYPE_7BIT)
    {
        return smtpSendPartBodyPlain(pDcpt);
    }

    return smtpSendPartBodyBase64(pDcpt);
}

//
//  Checks made:
//  - replaces bare CR or LF in the mail body with CRLF
//  - replaces lines starting with '.' with '..'
static TCPIP_SMTPC_STATUS smtpSendPartBodyPlain(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    char smtpLineBuff[1 + TCPIP_SMTPC_PLAIN_LINE_BUFF_SIZE + 2 + 1];    // room for extra . at front, extra CR/LF + \0 at end
    char *readStart, *readEnd, *writeStart, *writeEnd;
    int readBytes, xtraBytes;
    int  writeLen;
    char *crPos, *curr, *wsp;
    int  ch;
    uint16_t sktSpace;
    uint8_t    lfChar;

    TCPIP_SMTP_READ_DCPT* readDcpt = &pDcpt->smtpReadDcpt;

    readStart = smtpLineBuff + 1;
    readBytes = (*readDcpt->pReadObj->read)(readDcpt, (uint8_t*)readStart, TCPIP_SMTPC_PLAIN_LINE_BUFF_SIZE);

    if(readBytes == -1)
    {   // part ended; 
        return pDcpt->currStat + 1;
    }

    readEnd = readStart + readBytes;
    *readEnd = 0; // end properly

    writeStart = readStart;
    if(*writeStart == '.')
    {   // line starting with .
        *--writeStart = '.';   // insert extra .
    }

    // search for CR, LF
    crPos = 0;
    curr = readStart;
    while((ch = *curr++) != 0)
    {
        if(ch == '\r' || ch == '\n')
        {
            crPos = curr - 1;
            break;
        }
    }

    if(crPos == 0)
    {   // no line termination in this buffer; find wsp so CRLF can be inserted
        curr = readEnd;
        wsp = 0;
        do
        {
            ch = *--curr;
            if(ch == ' ' || ch == '\t')
            {
                wsp = curr;
                break;
            }
        }while(curr != readStart);

        if(wsp != 0)
        {
            readEnd = wsp + 1;
        }
        // else unlikely situation where the whole buffer does not contain a wsp...; simply insert at end

        writeEnd = readEnd;
        *writeEnd++ = '\r';
        *writeEnd++ = '\n';
    }
    else
    {   // found some terminator
        if(*crPos == '\n')
        {   // pointing to a LF; insert a CR
            readEnd = crPos + 1;    // where line ends
            writeEnd = crPos;
            *writeEnd++ = '\r';
            *writeEnd++ = '\n';
        }
        else
        {   // ok, proper CR
            if(crPos != (readEnd - 1))
            {   // more characters to follow
                writeEnd = readEnd = crPos + 1;    // where line ends
                if(*readEnd == '\n')
                {   // proper sequence in place
                    readEnd++;
                }
                // insert (missing) LF;
                *writeEnd++ = '\n';
            }
            else
            {   // CR right on the buffer edge; read another char
                writeEnd = readEnd;
                if((*readDcpt->pReadObj->read)(readDcpt, &lfChar, 1) != -1)
                {
                    readBytes++;
                    if(lfChar == '\n')
                    {   // was in the in stream as well
                        readEnd++;
                    }
                }
                // insert (missing) LF
                *writeEnd++ = '\n';
            }
        }
    }

    writeLen = writeEnd - writeStart;
    sktSpace = NET_PRES_SocketWriteIsReady(pDcpt->skt, writeLen, 0);

    if(sktSpace >= writeLen)
    {   // whole line can go through
        NET_PRES_SocketWrite(pDcpt->skt, writeStart, writeLen);
        // adjust the xtra space
        if((xtraBytes = readBytes - (readEnd - readStart)) != 0)
        {
            (*readDcpt->pReadObj->push)(readDcpt, xtraBytes);
        }
    }
    else
    {   // couldn't write anything
        (*readDcpt->pReadObj->push)(readDcpt, readBytes);
    }

    // more to write
    return pDcpt->currStat;
}


static TCPIP_SMTPC_STATUS smtpSendPartBodyBase64(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    int readLen, minOutLen, maxOutLen, encLen, xtraLen;
    uint16_t sktSpace;
    uint16_t base64Len;

    uint8_t smtpReadBuff[((TCPIP_SMTP_BASE64_BUFF_SIZE + 2) / 3) * 3];     // keep it multiple of 3!
    uint8_t smtpEncBuff[(sizeof(smtpReadBuff) * 4) / 3];     // encode buffer

    TCPIP_SMTP_READ_DCPT* readDcpt = &pDcpt->smtpReadDcpt;

    
    readLen = (*readDcpt->pReadObj->read)(readDcpt, smtpReadBuff, sizeof(smtpReadBuff));

    if(readLen == -1)
    {   // part ended; 
        return pDcpt->currStat + 1;
    }


    minOutLen = ((TCPIP_SMTP_BASE64_MIN_SKT_SIZE + 2) / 3) * 4;
    maxOutLen = ((readLen + 2) / 3) * 4;

    if(minOutLen > maxOutLen)
    {
        minOutLen = maxOutLen;
    }

    sktSpace = NET_PRES_SocketWriteIsReady(pDcpt->skt, maxOutLen, minOutLen);

    if(sktSpace < minOutLen)
    {   // no write
        encLen = 0;
    }
    else
    {   // can write something
        // encode
        encLen = ((sktSpace + 2) * 3) / 4;
        if(encLen > readLen)
        {
            encLen = readLen;
        }

        base64Len = TCPIP_Helper_Base64Encode(smtpReadBuff, encLen, smtpEncBuff, sizeof(smtpEncBuff));
        NET_PRES_SocketWrite(pDcpt->skt, smtpEncBuff, base64Len);
    }

    if((xtraLen = readLen - encLen) != 0)
    {
        (*readDcpt->pReadObj->push)(readDcpt, xtraLen);
    }
    
    // more to write
    return pDcpt->currStat;
}

// helpers ////////////////////////////////////////////////

// buffer read descriptor
static bool smtpBufferInit(TCPIP_SMTP_READ_DCPT* pRdDcpt, const char* name, const uint8_t* buffer, size_t buffSize)
{
    TCPIP_SMTP_READ_DCPT_BUFFER* pRdBuff = &pRdDcpt->bDcpt;

    if(buffer != 0 && buffSize != 0)
    {
        if(pRdBuff->readBuffer == 0 && pRdBuff->readSize == 0)
        {   // clean
            pRdBuff->readBuffer = buffer;
            pRdBuff->readSize = buffSize;
            pRdBuff->readOffset = 0;
            return true;
        }
        else if(pRdBuff->readBuffer == buffer && pRdBuff->readSize == buffSize)
        {   // reuse
            pRdBuff->readOffset = 0;
            return true;
        }

    }

    return false;
}

// minimal deinit needed
static void smtpBufferDeinit(TCPIP_SMTP_READ_DCPT* pRdDcpt)
{
    TCPIP_SMTP_READ_DCPT_BUFFER* pRdBuff = &pRdDcpt->bDcpt;
    pRdBuff->readBuffer = 0;
    pRdBuff->readSize = 0;
    pRdBuff->readOffset = 0;
}

// returns -1 for eof
// returns >= 0 number of bytes read
static int smtpBufferRead(TCPIP_SMTP_READ_DCPT* pRdDcpt, uint8_t* pBuffer, size_t nBytes)
{
    TCPIP_SMTP_READ_DCPT_BUFFER* pRdBuff = &pRdDcpt->bDcpt;

    size_t leftBytes;

    if(pRdBuff->readBuffer == 0 || (leftBytes = pRdBuff->readSize - pRdBuff->readOffset) == 0)
    {   // nothing left here
        return -1;
    }

    if(pBuffer == 0 || nBytes == 0)
    {
        return 0;
    }

    // read data
    if(nBytes > leftBytes)
    {
        nBytes = leftBytes;
    }
    memcpy(pBuffer, pRdBuff->readBuffer + pRdBuff->readOffset, nBytes);
    pRdBuff->readOffset += nBytes;

    return nBytes;
}

// returns -1 if nothing to push
// returns >=0 number of bytes pushed back 
static int smtpBufferPush(TCPIP_SMTP_READ_DCPT* pRdDcpt, size_t nBytes)
{
    TCPIP_SMTP_READ_DCPT_BUFFER* pRdBuff = &pRdDcpt->bDcpt;

    if(pRdBuff->readBuffer == 0)
    {   // nothing here
        return -1;
    }

    if(nBytes == 0)
    {
        return 0;
    }

    size_t readBytes = pRdBuff->readOffset;

    // offset data
    if(nBytes > readBytes)
    {
        nBytes = readBytes;
    }
    pRdBuff->readOffset -= nBytes;

    return nBytes;
}

// file read descriptor
static bool smtpFileInit(TCPIP_SMTP_READ_DCPT* pRdDcpt, const char* name, const uint8_t* buffer, size_t buffSize)
{
    TCPIP_SMTP_READ_DCPT_FILE* pRdFile = &pRdDcpt->fDcpt;

    if(name != 0)
    {
        if(pRdFile->fName == 0 && pRdFile->fHandle == SYS_FS_HANDLE_INVALID)
        {   // clean
            SYS_FS_HANDLE fHandle = SYS_FS_FileOpen_Wrapper(name, SYS_FS_FILE_OPEN_READ);
            if(fHandle == SYS_FS_HANDLE_INVALID)
            {
                return false;
            }

            pRdFile->fName = name;
            pRdFile->fHandle = fHandle;
            return true;
        }
        else if(pRdFile->fName == name && pRdFile->fHandle != SYS_FS_HANDLE_INVALID)
        {   // reuse
            return true;
        }
    }

    return false;
}

// close the file
static void     smtpFileDeinit(TCPIP_SMTP_READ_DCPT* pRdDcpt)
{
    TCPIP_SMTP_READ_DCPT_FILE* pRdFile = &pRdDcpt->fDcpt;

    if(pRdFile->fHandle != SYS_FS_HANDLE_INVALID)
    {
        SYS_FS_FileClose(pRdFile->fHandle);
        pRdFile->fHandle = SYS_FS_HANDLE_INVALID;
    }
    pRdFile->fName = 0;
}

static int smtpFileRead(TCPIP_SMTP_READ_DCPT* pRdDcpt, uint8_t* pBuffer, size_t nBytes)
{
    TCPIP_SMTP_READ_DCPT_FILE* pRdFile = &pRdDcpt->fDcpt;
    size_t readSize = -1;

    if(pRdFile->fHandle != SYS_FS_HANDLE_INVALID)
    {
        readSize = SYS_FS_FileRead(pRdFile->fHandle, pBuffer, nBytes);
        if(readSize == 0)
        {
            if(SYS_FS_FileEOF(pRdFile->fHandle))
            {   // true EOF
                return -1;
            }
        }
    }

    return readSize;
}

static int smtpFilePush(TCPIP_SMTP_READ_DCPT* pRdDcpt, size_t nBytes)
{
    TCPIP_SMTP_READ_DCPT_FILE* pRdFile = &pRdDcpt->fDcpt;

    if(pRdFile->fHandle != SYS_FS_HANDLE_INVALID)
    {
        return SYS_FS_FileSeek(pRdFile->fHandle, -nBytes, SYS_FS_SEEK_CUR);
    }
    
    return -1;
}


#endif //#if defined(TCPIP_STACK_USE_SMTPC)
