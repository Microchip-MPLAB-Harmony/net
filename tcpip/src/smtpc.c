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








#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_SMTPC

#include "stdlib.h"
#include "ctype.h"
#include <errno.h>

#include "tcpip/src/tcpip_private.h"
#include "net_pres/pres/net_pres_socketapi.h"

#include "system/fs/sys_fs.h"

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
static uint16_t     smtpcSktTxBuffSize = 0U;      // size of TX buffer for the associated socket
static uint16_t     smtpcSktRxBuffSize = 0U;      // size of RX buffer for the associated socket

static size_t       smtpcMailRetries;            // number of retries for a mail message
static size_t       smtpcServerRetryTmo;         // retry interval, seconds
static size_t       smtpcInternalRetryTmo;       // internal retry interval, seconds

static TCPIP_SMTPC_MESSAGE_DCPT* smtpcAllocPool = NULL;    // allocated pool of descriptors
                                                        // also the global SMTPC alive ptr
static SINGLE_LIST  smtpcMessageFreeList = { 0 };       // pool of free descriptors to be used for mail messages
static SINGLE_LIST  smtpcMessageBusyList = { 0 };       // list of messages scheduled to be sent

static TCPIP_SIGNAL_HANDLE    smtpcSignalHandle = NULL;

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
static TCPIP_SMTPC_STATUS smtpDcptTransactionFree(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);

static const TCPIP_SMTPC_STATUS_FNC smtpDcptStateTbl[] = 
{
    &smtpDcptStateWaitRetry,         // SMTPC_STAT_WAIT_RETRY
    &smtpDcptStateStartDns,          // SMTPC_STAT_DNS_START
    &smtpDcptStateWaitDns,           // SMTPC_STAT_DNS_WAIT
    &smtpDcptStateGetSkt,            // SMTPC_STAT_SOCKET_GET
    &smtpDcptStateWaitReply,         // SMTPC_STAT_SKT_WAIT_CONNECT
    // SMTPC_STAT_SERVER_CONNECTED
    &smtpDcptStateSendEhlo,          // SMTPC_STAT_EHLO_PLAIN
    &smtpDcptStateWaitReply,         // SMTPC_STAT_EHLO_PLAIN_WAIT
    &smtpDcptStateStartTls,          // SMTPC_STAT_TLS_START
    &smtpDcptStateWaitReply,         // SMTPC_STAT_TLS_WAIT
    &smtpDcptStateWaitTlsSecure,     // SMTPC_STAT_TLS_WAIT_SECURE
    &smtpDcptStateSendEhlo,          // SMTPC_STAT_EHLO_SECURE
    &smtpDcptStateWaitReply,         // SMTPC_STAT_EHLO_SECURE_WAIT
    &smtpDcptStateAuth,              // SMTPC_STAT_AUTH
    &smtpDcptStateWaitReply,         // SMTPC_STAT_AUTH_WAIT_LOGIN_USER
    &smtpDcptStateAuthLoginUser,     // SMTPC_STAT_AUTH_LOGIN_USER
    &smtpDcptStateWaitReply,         // SMTPC_STAT_AUTH_WAIT_LOGIN_PASS
    &smtpDcptStateAuthLoginPass,     // SMTPC_STAT_AUTH_LOGIN_PASS
    &smtpDcptStateWaitReply,         // SMTPC_STAT_AUTH_WAIT_FINAL
    // SMTPC_STAT_MAIL_START
    &smtpDcptStateMailFrom,          // SMTPC_STAT_MAIL_ENVL_FROM
    &smtpDcptStateWaitReply,         // SMTPC_STAT_MAIL_ENVL_FROM_WAIT
    &smtpDcptStateMailRcpt,          // SMTPC_STAT_MAIL_ENVL_RCPT
    &smtpDcptStateWaitReply,         // SMTPC_STAT_MAIL_ENVL_RCPT_WAIT
    &smtpDcptStateMailData,          // SMTPC_STAT_MAIL_ENVL_DATA
    &smtpDcptStateWaitReply,         // SMTPC_STAT_MAIL_ENVL_DATA_WAIT
    // SMTPC_STAT_MAIL_MSG_START
    &smtpDcptMailMsgDate,            // SMTPC_STAT_MAIL_MSG_HDR_DATE
    &smtpDcptMailMsgFrom,            // SMTPC_STAT_MAIL_MSG_HDR_FROM
    &smtpDcptMailMsgTo,              // SMTPC_STAT_MAIL_MSG_HDR_TO
    &smtpDcptMailMsgSender,          // SMTPC_STAT_MAIL_MSG_HDR_SENDER
    &smtpDcptMailMsgSubject,         // SMTPC_STAT_MAIL_MSG_HDR_SUBJECT
    &smtpDcptMailMsgMessageId,       // SMTPC_STAT_MAIL_MSG_HDR_MSG_ID
    &smtpDcptMailMsgMimeHdr,         // SMTPC_STAT_MAIL_MSG_HDR_MIME_HDR
    &smtpDcptMailMsgPreambleHdr,     // SMTPC_STAT_MAIL_MSG_HDR_PREAMBLE
    &smtpDcptMailMsgBodyHdr,         // SMTPC_STAT_MAIL_MSG_BODY_HDR
    &smtpDcptMailMsgBody,            // SMTPC_STAT_MAIL_MSG_BODY
    &smtpDcptMailMsgBuffHdr,         // SMTPC_STAT_MAIL_MSG_BUFF_HDR
    &smtpDcptMailMsgBuffBody,        // SMTPC_STAT_MAIL_MSG_BUFF_BODY
    &smtpDcptMailMsgFileHdr,         // SMTPC_STAT_MAIL_MSG_FILE_HDR
    &smtpDcptMailMsgFileBody,        // SMTPC_STAT_MAIL_MSG_FILE_BODY
    &smtpDcptMailMsgDone,            // SMTPC_STAT_MAIL_DONE
    &smtpDcptStateWaitReply,         // SMTPC_STAT_MAIL_DONE_WAIT
    &smtpDcptMailDoneReport,         // SMTPC_STAT_MAIL_DONE_REPORT
    // after mail done
    &smtpDcptTransactionReset,       // SMTPC_STAT_TRANSACTION_RESET
    &smtpDcptStateWaitReply,         // SMTPC_STAT_TRANSACTION_RESET_WAIT
    &smtpDcptTransactionQuit,        // SMTPC_STAT_TRANSACTION_QUIT
    &smtpDcptStateWaitReply,         // SMTPC_STAT_TRANSACTION_QUIT_WAIT
    &smtpDcptTransactionClose,       // SMTPC_STAT_TRANSACTION_CLOSE
    // done
    &smtpDcptTransactionFree,       // SMTPC_STAT_FREED

    
};

// RX buffer processing
static char         smtpcRxBuffer[TCPIP_SMTPC_SERVER_REPLY_BUFFER_SIZE + 1];         // global receive buffer

// function processing SMTP server messages
// should receive only full lines for processing...
// the rx buffer is still used so it won't be changed!
typedef TCPIP_SMTPC_STATUS(*SMTP_DCPT_PROCESS_FNC)(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines);

static TCPIP_SMTPC_STATUS smtpDcptRxProcNone(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines);
static TCPIP_SMTPC_STATUS smtpDcptRxProcConnect(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines);
static TCPIP_SMTPC_STATUS smtpDcptRxProcEhlo(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines);
static TCPIP_SMTPC_STATUS smtpDcptRxProcAuthFinal(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines);
static TCPIP_SMTPC_STATUS smtpDcptRxProcTls(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines);
static TCPIP_SMTPC_STATUS smtpDcptRxProcAuthLoginUser(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines);
static TCPIP_SMTPC_STATUS smtpDcptRxProcAuthLoginPass(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines);
static TCPIP_SMTPC_STATUS smtpDcptRxProcMailFrom(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines);
static TCPIP_SMTPC_STATUS smtpDcptRxProcMailRcpt(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines);
static TCPIP_SMTPC_STATUS smtpDcptRxProcMailData(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines);
static TCPIP_SMTPC_STATUS smtpDcptRxProcMailDoneWait(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines);
static TCPIP_SMTPC_STATUS smtpDcptRxProcTransactResetWait(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines);
static TCPIP_SMTPC_STATUS smtpDcptRxProcTransactQuitWait(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines);

static const SMTP_DCPT_PROCESS_FNC smtpRxProcessTbl[] =
{
    &smtpDcptRxProcNone,             // SMTPC_STAT_WAIT_RETRY
    &smtpDcptRxProcNone,             // SMTPC_STAT_DNS_START
    &smtpDcptRxProcNone,             // SMTPC_STAT_DNS_WAIT
    &smtpDcptRxProcNone,             // SMTPC_STAT_SOCKET_GET
    &smtpDcptRxProcConnect,          // SMTPC_STAT_SKT_WAIT_CONNECT
    // SMTPC_STAT_SERVER_CONNECTED
    &smtpDcptRxProcNone,             // SMTPC_STAT_EHLO_PLAIN
    &smtpDcptRxProcEhlo,             // SMTPC_STAT_EHLO_PLAIN_WAIT
    &smtpDcptRxProcNone,             // SMTPC_STAT_TLS_START
    &smtpDcptRxProcTls,              // SMTPC_STAT_TLS_WAIT
    &smtpDcptRxProcNone,             // SMTPC_STAT_TLS_WAIT_SECURE
    &smtpDcptRxProcNone,             // SMTPC_STAT_EHLO_SECURE
    &smtpDcptRxProcEhlo,             // SMTPC_STAT_EHLO_SECURE_WAIT
    &smtpDcptRxProcNone,             // SMTPC_STAT_AUTH
    &smtpDcptRxProcAuthLoginUser,    // SMTPC_STAT_AUTH_WAIT_LOGIN_USER
    &smtpDcptRxProcNone,             // SMTPC_STAT_AUTH_LOGIN_USER
    &smtpDcptRxProcAuthLoginPass,    // SMTPC_STAT_AUTH_WAIT_LOGIN_PASS
    &smtpDcptRxProcNone,             // SMTPC_STAT_AUTH_LOGIN_PASS
    &smtpDcptRxProcAuthFinal,        // SMTPC_STAT_AUTH_WAIT_FINAL
    // SMTPC_STAT_MAIL_START
    &smtpDcptRxProcNone,             // SMTPC_STAT_MAIL_ENVL_FROM
    &smtpDcptRxProcMailFrom,         // SMTPC_STAT_MAIL_ENVL_FROM_WAIT
    &smtpDcptRxProcNone,             // SMTPC_STAT_MAIL_ENVL_RCPT
    &smtpDcptRxProcMailRcpt,         // SMTPC_STAT_MAIL_ENVL_RCPT_WAIT
    &smtpDcptRxProcNone,             // SMTPC_STAT_MAIL_ENVL_DATA
    &smtpDcptRxProcMailData,         // SMTPC_STAT_MAIL_ENVL_DATA_WAIT
    // SMTPC_STAT_MAIL_MSG_START
    &smtpDcptRxProcNone,             // SMTPC_STAT_MAIL_MSG_HDR_DATE
    &smtpDcptRxProcNone,             // SMTPC_STAT_MAIL_MSG_HDR_FROM
    &smtpDcptRxProcNone,             // SMTPC_STAT_MAIL_MSG_HDR_TO
    &smtpDcptRxProcNone,             // SMTPC_STAT_MAIL_MSG_HDR_SENDER
    &smtpDcptRxProcNone,             // SMTPC_STAT_MAIL_MSG_HDR_SUBJECT
    &smtpDcptRxProcNone,             // SMTPC_STAT_MAIL_MSG_HDR_MSG_ID
    &smtpDcptRxProcNone,             // SMTPC_STAT_MAIL_MSG_HDR_MIME_HDR
    &smtpDcptRxProcNone,             // SMTPC_STAT_MAIL_MSG_HDR_PREAMBLE
    &smtpDcptRxProcNone,             // SMTPC_STAT_MAIL_MSG_BODY_HDR
    &smtpDcptRxProcNone,             // SMTPC_STAT_MAIL_MSG_BODY
    &smtpDcptRxProcNone,             // SMTPC_STAT_MAIL_MSG_BUFF_HDR
    &smtpDcptRxProcNone,             // SMTPC_STAT_MAIL_MSG_BUFF_BODY
    &smtpDcptRxProcNone,             // SMTPC_STAT_MAIL_MSG_FILE_HDR
    &smtpDcptRxProcNone,             // SMTPC_STAT_MAIL_MSG_FILE_BODY
    &smtpDcptRxProcNone,             // SMTPC_STAT_MAIL_DONE
    &smtpDcptRxProcMailDoneWait,     // SMTPC_STAT_MAIL_DONE_WAIT
    &smtpDcptRxProcNone,             // SMTPC_STAT_MAIL_DONE_REPORT
    // after mail done
    &smtpDcptRxProcNone,             // SMTPC_STAT_TRANSACTION_RESET
    &smtpDcptRxProcTransactResetWait,// SMTPC_STAT_TRANSACTION_RESET_WAIT
    &smtpDcptRxProcNone,             // SMTPC_STAT_TRANSACTION_QUIT
    &smtpDcptRxProcTransactQuitWait, // SMTPC_STAT_TRANSACTION_QUIT_WAIT
    &smtpDcptRxProcNone,             // SMTPC_STAT_TRANSACTION_CLOSE
    // done
    &smtpDcptRxProcNone,             // SMTPC_STAT_FREED
};

// buffer manipulation 
static bool     smtpBufferInit(TCPIP_SMTP_READ_DCPT* pRdDcpt, const char* name, const uint8_t* dataBuff, size_t buffSize);
static void     smtpBufferDeinit(TCPIP_SMTP_READ_DCPT* pRdDcpt);
static int      smtpBufferRead(TCPIP_SMTP_READ_DCPT* pRdDcpt, uint8_t* pBuffer, size_t nBytes);
static int      smtpBufferPush(TCPIP_SMTP_READ_DCPT* pRdDcpt, size_t nBytes);

static const TCPIP_SMTP_READ_OBJ smtpReadBufferObj = 
{
    .fInit = &smtpBufferInit, .fDeinit = &smtpBufferDeinit, .fRead = &smtpBufferRead, .fPush = &smtpBufferPush
};

// file manipulation 
static bool     smtpFileInit(TCPIP_SMTP_READ_DCPT* pRdDcpt, const char* name, const uint8_t* dataBuff, size_t buffSize);
static void     smtpFileDeinit(TCPIP_SMTP_READ_DCPT* pRdDcpt);
static int      smtpFileRead(TCPIP_SMTP_READ_DCPT* pRdDcpt, uint8_t* pBuffer, size_t nBytes);
static int      smtpFilePush(TCPIP_SMTP_READ_DCPT* pRdDcpt, size_t nBytes);

static const TCPIP_SMTP_READ_OBJ smtpReadFileObj = 
{
    .fInit = &smtpFileInit, .fDeinit = &smtpFileDeinit, .fRead = &smtpFileRead, .fPush = &smtpFilePush
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
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_WAIT_RETRY,        // SMTPC_STAT_WAIT_RETRY
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_DNS,               // SMTPC_STAT_DNS_START
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_DNS,               // SMTPC_STAT_DNS_WAIT
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_CONNECT,           // SMTPC_STAT_SOCKET_GET
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_CONNECT,           // SMTPC_STAT_SKT_WAIT_CONNECT
    // SMTPC_STAT_SERVER_CONNECTED
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_HELLO,             // SMTPC_STAT_EHLO_PLAIN
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_HELLO,             // SMTPC_STAT_EHLO_PLAIN_WAIT
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_TLS,               // SMTPC_STAT_TLS_START
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_TLS,               // SMTPC_STAT_TLS_WAIT
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_TLS,               // SMTPC_STAT_TLS_WAIT_SECURE
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_HELLO,             // SMTPC_STAT_EHLO_SECURE
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_HELLO,             // SMTPC_STAT_EHLO_SECURE_WAIT
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_AUTH,              // SMTPC_STAT_AUTH
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_AUTH,              // SMTPC_STAT_AUTH_WAIT_LOGIN_USER
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_AUTH,              // SMTPC_STAT_AUTH_LOGIN_USER
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_AUTH,              // SMTPC_STAT_AUTH_WAIT_LOGIN_PASS
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_AUTH,              // SMTPC_STAT_AUTH_LOGIN_PASS
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_AUTH,              // SMTPC_STAT_AUTH_WAIT_FINAL
    // SMTPC_STAT_MAIL_START
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_ENVELOPE,     // SMTPC_STAT_MAIL_ENVL_FROM
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_ENVELOPE,     // SMTPC_STAT_MAIL_ENVL_FROM_WAIT
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_ENVELOPE,     // SMTPC_STAT_MAIL_ENVL_RCPT
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_ENVELOPE,     // SMTPC_STAT_MAIL_ENVL_RCPT_WAIT
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_ENVELOPE,     // SMTPC_STAT_MAIL_ENVL_DATA
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_ENVELOPE,     // SMTPC_STAT_MAIL_ENVL_DATA_WAIT
    // SMTPC_STAT_MAIL_MSG_START
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_BODY,         // SMTPC_STAT_MAIL_MSG_HDR_DATE
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_BODY,         // SMTPC_STAT_MAIL_MSG_HDR_FROM
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_BODY,         // SMTPC_STAT_MAIL_MSG_HDR_TO
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_BODY,         // SMTPC_STAT_MAIL_MSG_HDR_SENDER
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_BODY,         // SMTPC_STAT_MAIL_MSG_HDR_SUBJECT
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_BODY,         // SMTPC_STAT_MAIL_MSG_HDR_MSG_ID
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_BODY,         // SMTPC_STAT_MAIL_MSG_HDR_MIME_HDR
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_BODY,         // SMTPC_STAT_MAIL_MSG_HDR_PREAMBLE
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_BODY,         // SMTPC_STAT_MAIL_MSG_BODY_HDR
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_BODY,         // SMTPC_STAT_MAIL_MSG_BODY
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_BUFFERS,      // SMTPC_STAT_MAIL_MSG_BUFF_HDR
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_BUFFERS,      // SMTPC_STAT_MAIL_MSG_BUFF_BODY
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_FILES,        // SMTPC_STAT_MAIL_MSG_FILE_HDR
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_FILES,        // SMTPC_STAT_MAIL_MSG_FILE_BODY
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_ACKNOWLEDGE,  // SMTPC_STAT_MAIL_DONE
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_ACKNOWLEDGE,  // SMTPC_STAT_MAIL_DONE_WAIT
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_MAIL_REPORT,       // SMTPC_STAT_MAIL_DONE_REPORT
    // after mail done
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_CLOSING,           // SMTPC_STAT_TRANSACTION_RESET
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_CLOSING,           // SMTPC_STAT_TRANSACTION_RESET_WAIT
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_CLOSING,           // SMTPC_STAT_TRANSACTION_QUIT
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_CLOSING,           // SMTPC_STAT_TRANSACTION_QUIT_WAIT
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_CLOSING,           // SMTPC_STAT_TRANSACTION_CLOSE
    // no longer
    (uint8_t)TCPIP_SMTPC_MESSAGE_STAT_NONE,              // SMTPC_STAT_FREED
};


/****************************************************************************
  Section:
    SMTPC Internal Function Prototypes
  ***************************************************************************/

static void TCPIP_SMTPC_Process(void);

static void F_SMTPCSocketRxSignalHandler(NET_PRES_SKT_HANDLE_T hTCP, NET_PRES_SIGNAL_HANDLE hNet, uint16_t sigType, const void* param);

static TCPIP_SMTPC_STATUS smtpCommandParam(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, TCPIP_SMTPC_CMD_TYPE cmdType, const char* cmdArg, uint32_t serverTmo);

static TCPIP_SMTPC_STATUS smtpClientWriteCmd(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* cmdStr, const char* cmdArg);

static bool smtpSetEhloString(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, char* pAddBuff, size_t buffSize);

static void smtpcSetDcptTmo(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, uint32_t tmo);

static bool smtpcIsDcptTmo(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);

static TCPIP_SMTPC_STATUS smtpcErrorStop(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, TCPIP_SMTPC_MESSAGE_RESULT res, uint16_t dcptRetryFlags);

static void smtpcRetryInit(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, uint16_t dcptFlags);

static TCPIP_SMTPC_STATUS smtpcServerErrorStop(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, TCPIP_SMTPC_MESSAGE_RESULT res, int serverCode);

static TCPIP_SMTPC_STATUS smtpAuthLogin(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);

static TCPIP_SMTPC_STATUS smtpAuthPlain(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);

static TCPIP_SMTPC_STATUS   smtpWriteMailEnvAddress(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, TCPIP_SMTPC_ENVELOPE envType);

static bool smtpFormatMailAddress(const char* mailAddr, char* mailAddrBuff, size_t buffSize);

static TCPIP_SMTPC_STATUS smtpSendPartHdr(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* fileName);

static TCPIP_SMTPC_STATUS smtpSendPartBody(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);

static TCPIP_SMTPC_STATUS smtpSendPartBodyPlain(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);

static TCPIP_SMTPC_STATUS smtpSendPartBodyBase64(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt);

// debug functionality
#if ((TCPIP_SMTPC_DEBUG_LEVEL & TCPIP_SMTPC_DEBUG_MASK_BASIC) != 0)
static volatile int V_SMTPCStayAssertLoop = 0;
static void F_SMTPCAssertCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("SMTPC Assert: %s, in line: %d, \r\n", message, lineNo);
        while(V_SMTPCStayAssertLoop != 0)
        {
        }
    }
}
// a debug condition, not really assertion
static volatile int V_SMTPCStayCondLoop = 0;
static void F_SMTPCDebugCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("SMTPC Cond: %s, in line: %d, \r\n", message, lineNo);
        while(V_SMTPCStayCondLoop != 0)
        {
        }
    }
}

#else
#define F_SMTPCAssertCond(cond, message, lineNo)
#define F_SMTPCDebugCond(cond, message, lineNo)
#endif  // (TCPIP_SMTPC_DEBUG_LEVEL & TCPIP_SMTPC_DEBUG_MASK_BASIC)


#if ((TCPIP_SMTPC_DEBUG_LEVEL & (TCPIP_SMTPC_DEBUG_MASK_STATE | TCPIP_SMTPC_DEBUG_MASK_ERROR_STATE)) != 0)
static const char* const T_SMTPCDbgDcptStateTbl[] = 
{
    "idle",                 // SMTPC_STAT_WAIT_RETRY
    "dns_s",                // SMTPC_STAT_DNS_START
    "dns_w",                // SMTPC_STAT_DNS_WAIT
    "skt_g",                // SMTPC_STAT_SOCKET_GET
    "skt_w",                // SMTPC_STAT_SKT_WAIT_CONNECT
    "ehlo_pl",              // SMTPC_STAT_EHLO_PLAIN
    "ehlo_pl_w",            // SMTPC_STAT_EHLO_PLAIN_WAIT
    "tls_s",                // SMTPC_STAT_TLS_START
    "tls_w",                // SMTPC_STAT_TLS_WAIT
    "tls_s_w",              // SMTPC_STAT_TLS_WAIT_SECURE
    "ehlo_s",               // SMTPC_STAT_EHLO_SECURE
    "ehlo_s_w",             // SMTPC_STAT_EHLO_SECURE_WAIT
    "auth",                 // SMTPC_STAT_AUTH
    "auth_usr_w",           // SMTPC_STAT_AUTH_WAIT_LOGIN_USER
    "auth_usr",             // SMTPC_STAT_AUTH_LOGIN_USER
    "auth_pass_w",          // SMTPC_STAT_AUTH_WAIT_LOGIN_PASS
    "auth_pass",            // SMTPC_STAT_AUTH_LOGIN_PASS
    "auth_fin_w",           // SMTPC_STAT_AUTH_WAIT_FINAL
    "envl_from",            // SMTPC_STAT_MAIL_ENVL_FROM
    "envl_from_w",          // SMTPC_STAT_MAIL_ENVL_FROM_WAIT
    "envl_rcpt",            // SMTPC_STAT_MAIL_ENVL_RCPT
    "envl_rcpt_w",          // SMTPC_STAT_MAIL_ENVL_RCPT_WAIT
    "envl_data",            // SMTPC_STAT_MAIL_ENVL_DATA
    "envl_data_w",          // SMTPC_STAT_MAIL_ENVL_DATA_WAIT
    "mail_date",            // SMTPC_STAT_MAIL_MSG_HDR_DATE
    "mail_from",            // SMTPC_STAT_MAIL_MSG_HDR_FROM
    "mail_to",              // SMTPC_STAT_MAIL_MSG_HDR_TO
    "mail_snder",           // SMTPC_STAT_MAIL_MSG_HDR_SENDER
    "mail_subj",            // SMTPC_STAT_MAIL_MSG_HDR_SUBJECT
    "mail_msgid",           // SMTPC_STAT_MAIL_MSG_HDR_MSG_ID
    "mail_mimehdr",         // SMTPC_STAT_MAIL_MSG_HDR_MIME_HDR
    "mail_preamb",          // SMTPC_STAT_MAIL_MSG_HDR_PREAMBLE
    "mail_body_hdr",        // SMTPC_STAT_MAIL_MSG_BODY_HDR
    "mail_body",            // SMTPC_STAT_MAIL_MSG_BODY
    "mail_buff_hdr",        // SMTPC_STAT_MAIL_MSG_BUFF_HDR
    "mail_buff_body",       // SMTPC_STAT_MAIL_MSG_BUFF_BODY
    "mail_file_hdr",        // SMTPC_STAT_MAIL_MSG_FILE_HDR
    "mail_file_body",       // SMTPC_STAT_MAIL_MSG_FILE_BODY
    "mail_done",            // SMTPC_STAT_MAIL_DONE
    "mail_done_w",          // SMTPC_STAT_MAIL_DONE_WAIT
    "mail_report",          // SMTPC_STAT_MAIL_DONE_REPORT
    "mail_reset",           // SMTPC_STAT_TRANSACTION_RESET
    "mail_reset_w",         // SMTPC_STAT_TRANSACTION_RESET_WAIT
    "mail_quit",            // SMTPC_STAT_TRANSACTION_QUIT
    "mail_quit_w",          // SMTPC_STAT_TRANSACTION_QUIT_WAIT
    "mail_tr_close",        // SMTPC_STAT_TRANSACTION_CLOSE
    "mail_freed",           // SMTPC_STAT_FREED
};
#endif  // (TCPIP_SMTPC_DEBUG_LEVEL & (TCPIP_SMTPC_DEBUG_MASK_STATE | TCPIP_SMTPC_DEBUG_MASK_ERROR_STATE) != 0)

#if ((TCPIP_SMTPC_DEBUG_LEVEL & TCPIP_SMTPC_DEBUG_MASK_STATE) != 0)
static void F_SMTPCDbgDcptState(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    if(pDcpt->currStat != pDcpt->prevStat)
    {
        SYS_CONSOLE_PRINT("SMTPC dcpt: %d, state: %s\r\n", pDcpt->dcptIx, T_SMTPCDbgDcptStateTbl[pDcpt->currStat]);
        pDcpt->prevStat = pDcpt->currStat;
    }
}

#else
#define F_SMTPCDbgDcptState(pDcpt)
#endif  // ((TCPIP_SMTPC_DEBUG_LEVEL & TCPIP_SMTPC_DEBUG_MASK_STATE) != 0)

#if ((TCPIP_SMTPC_DEBUG_LEVEL & TCPIP_SMTPC_DEBUG_MASK_ERROR_STATE) != 0)
static void F_SMTPCDbgDcptErrorState(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, TCPIP_SMTPC_MESSAGE_RESULT res)
{
    SYS_CONSOLE_PRINT("SMTPC dcpt error: %d, state curr: %s, jump: %s, res: %d\r\n", pDcpt->dcptIx, T_SMTPCDbgDcptStateTbl[pDcpt->currStat], 
                                                                                   T_SMTPCDbgDcptStateTbl[pDcpt->errorJumpStat], res);
}

#else
#define F_SMTPCDbgDcptErrorState(pDcpt, res)
#endif  // ((TCPIP_SMTPC_DEBUG_LEVEL & TCPIP_SMTPC_DEBUG_MASK_ERROR_STATE) != 0)


#if ((TCPIP_SMTPC_DEBUG_LEVEL & TCPIP_SMTPC_DEBUG_MASK_CMD_TO_SRV) != 0)
static void F_SMTPCDbgMailStrToSrv(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* srvCommand, size_t nBytes)
{
    F_SMTPCAssertCond(nBytes == strlen(srvCommand), __func__, __LINE__);
    SYS_CONSOLE_PRINT("SMTPC dcpt: %d to Srv:-%sCRLF\r\n", pDcpt->dcptIx, srvCommand);
}
#else
#define F_SMTPCDbgMailStrToSrv(pDcpt, srvCommand, nBytes)
#endif  // ((TCPIP_SMTPC_DEBUG_LEVEL & TCPIP_SMTPC_DEBUG_MASK_CMD_TO_SRV) != 0)


#if ((TCPIP_SMTPC_DEBUG_LEVEL & TCPIP_SMTPC_DEBUG_MASK_REPLY_FROM_SRV) != 0)
static void F_SMTPCDbgMailStrFromSrv(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* srvReply)
{
    SYS_CONSOLE_PRINT("SMTPC dcpt: %d, from Srv:-%sCRLF\r\n", pDcpt->dcptIx, srvReply);
}
#else
#define F_SMTPCDbgMailStrFromSrv(pDcpt, srvReply)
#endif  // ((TCPIP_SMTPC_DEBUG_LEVEL & TCPIP_SMTPC_DEBUG_MASK_REPLY_FROM_SRV) != 0)

static __inline__ void __attribute__((always_inline)) smtpcSetStatus(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, TCPIP_SMTPC_STATUS newStat)
{
    pDcpt->currStat = newStat;
    F_SMTPCDbgDcptState(pDcpt);
}

static __inline__ void __attribute__((always_inline)) smtpcSetWarning(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, TCPIP_SMTPC_MESSAGE_WARNING newWarn)
{
    uint16_t msgWarn = (uint16_t)pDcpt->messageWarn;
    msgWarn |= (uint16_t)newWarn;
    pDcpt->messageWarn = (TCPIP_SMTPC_MESSAGE_WARNING)msgWarn;
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
static __inline__ uint16_t __attribute__((always_inline)) smtpcServerCodeRetryFlag(int serverCode)
{
    return (200 <= serverCode && serverCode < 500) ? (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_SERVER : (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_NONE;
}

// conversion functions/helpers
//
static __inline__ TCPIP_SMTPC_MESSAGE_DCPT* __attribute__((always_inline)) FC_SglNode2MsgDcpt(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        TCPIP_SMTPC_MESSAGE_DCPT* pMsgDcpt;
    }U_SGL_NODE_MSG_DCPT;

    U_SGL_NODE_MSG_DCPT.node = node;
    return U_SGL_NODE_MSG_DCPT.pMsgDcpt;
}

static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_MsgDcpt2SglNode(TCPIP_SMTPC_MESSAGE_DCPT* pMsgDcpt)
{
    union
    {
        TCPIP_SMTPC_MESSAGE_DCPT* pMsgDcpt;
        SGL_LIST_NODE*  node;
    }U_MSG_DCPT_SGL_NODE;

    U_MSG_DCPT_SGL_NODE.pMsgDcpt = pMsgDcpt;
    return U_MSG_DCPT_SGL_NODE.node;
}

static __inline__ TCPIP_SMTPC_MESSAGE_DCPT* __attribute__((always_inline)) FC_MsgHndl2MsgDcpt(TCPIP_SMTPC_MESSAGE_HANDLE msgHndl)
{
    union
    {
        TCPIP_SMTPC_MESSAGE_HANDLE  msgHndl;
        TCPIP_SMTPC_MESSAGE_DCPT* pMsgDcpt;
    }U_MSG_HNDL_MSG_DCPT;

    U_MSG_HNDL_MSG_DCPT.msgHndl = msgHndl;
    return U_MSG_HNDL_MSG_DCPT.pMsgDcpt;
}

static __inline__ uint32_t __attribute__((always_inline)) FC_SmtpCfg2Ui32(const TCPIP_SMTPC_MODULE_CONFIG* pSmtpcConfig)
{
    union
    {
        const TCPIP_SMTPC_MODULE_CONFIG*  pSmtpcConfig;
        uint32_t                    uVal32;
    }U_MOD_CFG_UI_32;

    U_MOD_CFG_UI_32.pSmtpcConfig = pSmtpcConfig;
    return U_MOD_CFG_UI_32.uVal32;
}

static __inline__ TCPIP_SMTPC_STATUS __attribute__((always_inline)) FC_SmtpStatInc(TCPIP_SMTPC_STATUS smtpcStat, int incVal)
{
    union
    {
        TCPIP_SMTPC_STATUS  smtpcStat;
        int     iStat;
    }U_SMTPC_STAT_I_VAl;

    U_SMTPC_STAT_I_VAl.smtpcStat = smtpcStat;
    U_SMTPC_STAT_I_VAl.iStat += incVal;
    return U_SMTPC_STAT_I_VAl.smtpcStat;
}

static __inline__ bool __attribute__((always_inline)) FC_ChIsDigit(char ch)
{
    return (ch >= '0' && ch <= '9') ? true : false;
}

static __inline__ bool __attribute__((always_inline)) FC_ChIsSpace(char ch)
{
    union
    {
        char ch;
        int  iVal;
    }U_CH_INT;

    U_CH_INT.iVal = 0;
    U_CH_INT.ch = ch;

    return (isspace(U_CH_INT.iVal) == 0) ? false: true;
}

/****************************************************************************
  Section:
    SMTPC Function Implementations
  ***************************************************************************/


#if (TCPIP_STACK_DOWN_OPERATION != 0)
// if pNetIf == NULL, all connections are closed, stack is going down
static void F_SMTPC_CloseConnections(TCPIP_NET_IF* pNetIf)
{
    TCPIP_SMTPC_MESSAGE_DCPT* pDcpt;
    SINGLE_LIST newList;

    TCPIP_Helper_SingleListInitialize(&newList);

    while((pDcpt = FC_SglNode2MsgDcpt(TCPIP_Helper_SingleListHeadRemove(&smtpcMessageBusyList))) != NULL)
    {
        bool closeSkt = false;
        if(pDcpt->skt != NET_PRES_INVALID_SOCKET)
        {   // active socket on this one; get the interface over which we're connected
            if(pNetIf == NULL)
            {
                closeSkt = true;
            }
            else
            {
                TCP_SOCKET_INFO sktInfo;
                (void)NET_PRES_SocketInfoGet(pDcpt->skt, &sktInfo);
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
            TCPIP_Helper_SingleListTailAdd(&newList, FC_MsgDcpt2SglNode(pDcpt));
        }
    }

    smtpcMessageBusyList = newList;
}

static void F_SMTPCCleanup(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    F_SMTPC_CloseConnections(NULL);

    // free the allocated resources
    if(smtpcAllocPool != NULL)
    {
        (void)TCPIP_HEAP_Free(stackCtrl->memH, smtpcAllocPool);
        smtpcAllocPool = NULL;
    }
    
    if(smtpcSignalHandle != NULL)
    {
        TCPIPStackSignalHandlerDeregister(smtpcSignalHandle);
        smtpcSignalHandle = NULL;
    }
}

void TCPIP_SMTPC_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN) // interface down
    
    if(smtpcInitCount > 0)
    {   // we're up and running
        F_SMTPC_CloseConnections(stackCtrl->pNetIf);

        if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
        {   // stack shut down
            if(--smtpcInitCount == 0)
            {   // all closed
                // release resources
                F_SMTPCCleanup(stackCtrl);
            }
        }
    }
}
#else
#define F_SMTPCCleanup(stackCtrl)
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)
bool TCPIP_SMTPC_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* initData)
{

    size_t ix;
    TCPIP_SMTPC_MESSAGE_DCPT* pDcpt;

    if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }

    // stack init
    if(smtpcInitCount == 0)
    {   // stack start up; initialize just once
        // check configuration data is not missing
        if(initData == NULL)
        {
            return false;
        }
        
        const TCPIP_SMTPC_MODULE_CONFIG* pSmtpcConfig = (const TCPIP_SMTPC_MODULE_CONFIG*)initData;
        // critical initialization here...!

        // initialize the active message list
        TCPIP_Helper_SingleListInitialize(&smtpcMessageBusyList);

        // create the SMTPC timer
        smtpcSignalHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_SMTPC_Task, TCPIP_SMTPC_TASK_TICK_RATE);
        // allocate the descriptors pool
        smtpcAllocPool = (TCPIP_SMTPC_MESSAGE_DCPT*)TCPIP_HEAP_Calloc(stackCtrl->memH, pSmtpcConfig->nMailConnections, sizeof(*smtpcAllocPool));
    

        if(smtpcSignalHandle == NULL || smtpcAllocPool == NULL)
        {   // cannot create the SMTPC timer/mail list
            F_SMTPCCleanup(stackCtrl);
            return false;
        }

        // do some other non-critical initialization here...!
        // generate some (unique) counter
        smtpcMailCounter = FC_SmtpCfg2Ui32(pSmtpcConfig) + SYS_RANDOM_PseudoGet() % 1000U;

        // initialize the message pool
        TCPIP_Helper_SingleListInitialize(&smtpcMessageFreeList);
        pDcpt = smtpcAllocPool;
        for(ix = 0; ix < pSmtpcConfig->nMailConnections; ix++)
        {
            pDcpt->skt = NET_PRES_INVALID_SOCKET; 
            pDcpt->dcptIx = (uint16_t)ix;
            TCPIP_Helper_SingleListTailAdd(&smtpcMessageFreeList, FC_MsgDcpt2SglNode(pDcpt));
            pDcpt++;
        }

        smtpcSktTxBuffSize = pSmtpcConfig->sktTxBuffSize;
        smtpcSktRxBuffSize = pSmtpcConfig->sktRxBuffSize;
        // timeouts
        smtpcServerGenTmo = pSmtpcConfig->serverReplyTmo > TCPIP_SMTPC_MIN_SERVER_TMO ? pSmtpcConfig->serverReplyTmo : TCPIP_SMTPC_MIN_SERVER_TMO;
        smtpcServerDataTmo = pSmtpcConfig->serverDataTmo > TCPIP_SMTPC_MIN_SERVER_TMO ? pSmtpcConfig->serverDataTmo : TCPIP_SMTPC_MIN_SERVER_TMO;
        smtpcTlsHandshakeTmo = pSmtpcConfig->tlsHandshakeTmo > TCPIP_SMTPC_MIN_SERVER_TMO ? pSmtpcConfig->tlsHandshakeTmo : TCPIP_SMTPC_MIN_SERVER_TMO;
        smtpcMailRetries = pSmtpcConfig->nMailRetries != 0U ? pSmtpcConfig->nMailRetries : TCPIP_SMTPC_MIN_RETRIES;
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
    IP_ADDRESS_TYPE     addType;
    bool                useDns;
    TCPIP_SMTPC_STATUS  newStat;
    TCPIP_SMTPC_MESSAGE_DCPT* pDcpt = NULL;

    while(true)
    {
        if(smtpcAllocPool == NULL)
        {   // not up and running!
            res = TCPIP_SMTPC_RES_INITIALIZE_ERROR;
            break;
        }

        // do a minimum sanity check
        if(pMailMessage == NULL)
        {
            res = TCPIP_SMTPC_RES_MESSAGE_ERROR;
            break;
        }

        if(pMailMessage->smtpServer == NULL || strlen(pMailMessage->smtpServer) == 0U || pMailMessage->serverPort == 0U)
        {
            res = TCPIP_SMTPC_RES_MESSAGE_SERVER_ERROR;
            break;
        }

        if(pMailMessage->from == NULL || pMailMessage->to == NULL)
        {
            res = TCPIP_SMTPC_RES_MESSAGE_RCPT_ERROR;
            break;
        }

        if((pMailMessage->nBuffers != 0U && pMailMessage->attachBuffers == NULL) || (pMailMessage->nBuffers == 0U && pMailMessage->attachBuffers != NULL))
        {
            res = TCPIP_SMTPC_RES_MESSAGE_BUFFER_ERROR;
            break;
        }

        if((pMailMessage->nFiles != 0U && pMailMessage->attachFiles == NULL) || (pMailMessage->nFiles == 0U && pMailMessage->attachFiles != NULL))
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

#if defined (TCPIP_STACK_USE_IPV6)
            if((pMailMessage->messageFlags & (uint16_t)TCPIP_SMTPC_MAIL_FLAG_USE_IPV6) != 0U)
            {   // IPv6 requested
                addType = IP_ADDRESS_TYPE_IPV6;
            }
            else
            {
#if defined (TCPIP_STACK_USE_IPV4)
                addType = IP_ADDRESS_TYPE_IPV4;
#else
                addType = IP_ADDRESS_TYPE_IPV6;
#endif  // defined (TCPIP_STACK_USE_IPV4)
            }
#else
            // IPv4 takes precedence
            addType = IP_ADDRESS_TYPE_IPV4;
#endif // defined (TCPIP_STACK_USE_IPV6)
        }

        // start mail scheduling
        OSAL_CRITSECT_DATA_TYPE critStat = smtpcThreadLock();
        pDcpt = FC_SglNode2MsgDcpt(TCPIP_Helper_SingleListHeadRemove(&smtpcMessageFreeList));
        smtpcThreadUnlock(critStat);

        if(pDcpt == NULL)
        {
            res = TCPIP_SMTPC_RES_MAIL_BUSY;
            break;
        }

        // mail descriptor initialization
        pDcpt->smtpMailMessage = *pMailMessage; // make a copy of the message 
        pDcpt->addType = addType;
        pDcpt->retryCnt = (uint16_t)smtpcMailRetries;

        uint16_t flags;
        
       if(!useDns)
        {
            pDcpt->serverAdd = ipAddr;
            newStat = SMTPC_STAT_SOCKET_GET;
            flags = (uint16_t)TCPIP_SMTPC_DCPT_FLAG_NONE;
        }
        else
        {
            newStat = SMTPC_STAT_DNS_START;
            flags = (uint16_t)TCPIP_SMTPC_DCPT_FLAG_DNS;
        }
        smtpcSetStatus(pDcpt, newStat);
        smtpcRetryInit(pDcpt, flags);

        critStat = smtpcThreadLock();
        TCPIP_Helper_SingleListTailAdd(&smtpcMessageBusyList, FC_MsgDcpt2SglNode(pDcpt));
        smtpcThreadUnlock(critStat);

        res = TCPIP_SMTPC_RES_OK;
        break;
    }


    if(pRes != NULL)
    {
        *pRes = res;
    }

    return pDcpt; 
}

 
TCPIP_SMTPC_MESSAGE_RESULT TCPIP_SMTPC_MessageQuery(TCPIP_SMTPC_MESSAGE_HANDLE messageHandle, TCPIP_SMTPC_MESSAGE_QUERY* pQuery)
{
    TCPIP_SMTPC_MESSAGE_RESULT res = TCPIP_SMTPC_RES_MESSAGE_ERROR; 
    TCPIP_SMTPC_MESSAGE_DCPT* pDcpt = FC_MsgHndl2MsgDcpt(messageHandle);

    if(pDcpt != NULL)
    {
        OSAL_CRITSECT_DATA_TYPE critStat = smtpcThreadLock();

        if(TCPIP_Helper_SingleListFind(&smtpcMessageBusyList, FC_MsgDcpt2SglNode(pDcpt)))
        {   // OK, one of ours
            if(pQuery != NULL)
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

    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != TCPIP_MODULE_SIGNAL_NONE)
    { // TMO or RX signals occurred
        TCPIP_SMTPC_Process();
    }
}

// send a signal to the SMTP module that data is available
// no manager alert needed since this normally results as a higher layer (TCP) signal
static void F_SMTPCSocketRxSignalHandler(NET_PRES_SKT_HANDLE_T hTCP, NET_PRES_SIGNAL_HANDLE hNet, uint16_t sigType, const void* param)
{
    if(sigType == (uint16_t)TCPIP_TCP_SIGNAL_RX_DATA)
    {
        (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}

// process SMTPC state machine
// process all the busy mail descriptors according to state
static void TCPIP_SMTPC_Process(void)
{
    TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, *pNext, *pPrev;
    TCPIP_SMTPC_STATUS newStat;

    OSAL_CRITSECT_DATA_TYPE critStat = smtpcThreadLock();
    pDcpt = FC_SglNode2MsgDcpt(smtpcMessageBusyList.head);
    pNext = pDcpt != NULL ? pDcpt->next : NULL;
    smtpcThreadUnlock(critStat);
    pPrev = NULL;


    while(pDcpt != NULL)
    {
        // process this descriptor
        if(pDcpt->currStat >= sizeof(smtpDcptStateTbl) / sizeof(*smtpDcptStateTbl))
        {
            F_SMTPCAssertCond(false, __func__, __LINE__);
            newStat = smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_INTERNAL_ERROR, (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_NONE);
        }
        else if((pDcpt->dcptFlags & (uint16_t)TCPIP_SMTPC_DCPT_FLAG_CONNECTED) != 0U && pDcpt->skt != NET_PRES_INVALID_SOCKET && (NET_PRES_SocketWasReset(pDcpt->skt) || NET_PRES_SocketWasDisconnected(pDcpt->skt)))
        {   // check for possible disconnect: lost connection to the server
            pDcpt->dcptFlags &= ~(uint16_t)(TCPIP_SMTPC_SERVER_FLAG_CONN_MASK);
            newStat = smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_CONNECTION_CLOSE, (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_SERVER);
        }
        else
        {
            newStat = (*smtpDcptStateTbl[pDcpt->currStat])(pDcpt);
        }
        smtpcSetStatus(pDcpt, newStat);

        critStat = smtpcThreadLock();
        if(newStat == SMTPC_STAT_FREED)
        {
            (void)TCPIP_Helper_SingleListNextRemove(&smtpcMessageBusyList, FC_MsgDcpt2SglNode(pPrev));
            TCPIP_Helper_SingleListTailAdd(&smtpcMessageFreeList, FC_MsgDcpt2SglNode(pDcpt));
        }
        else
        {
            pPrev = pDcpt;
        }
        pDcpt = pNext;
        pNext = pDcpt != NULL ? pDcpt->next : NULL;
        smtpcThreadUnlock(critStat);
    }

}

// SMTPC_STAT_WAIT_RETRY
static TCPIP_SMTPC_STATUS smtpDcptStateWaitRetry(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{  
    TCPIP_SMTPC_STATUS newStat;

    if(!smtpcIsDcptTmo(pDcpt))
    {   // wait some more
        newStat = SMTPC_STAT_WAIT_RETRY;
    }
    else
    {
        uint16_t flags;
        if((pDcpt->dcptFlags & (uint16_t)TCPIP_SMTPC_DCPT_FLAG_DNS) != 0U)
        {
            newStat = SMTPC_STAT_DNS_START;
            flags = (uint16_t)TCPIP_SMTPC_DCPT_FLAG_DNS;
        }
        else
        {
            newStat = SMTPC_STAT_SOCKET_GET;
            flags = (uint16_t)TCPIP_SMTPC_DCPT_FLAG_NONE;
        }

        smtpcRetryInit(pDcpt, flags);
    }

    return newStat;
}

// SMTPC_STAT_DNS_START
static TCPIP_SMTPC_STATUS smtpDcptStateStartDns(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    TCPIP_DNS_RESULT dnsRes = TCPIP_DNS_Resolve(pDcpt->smtpMailMessage.smtpServer, pDcpt->addType == IP_ADDRESS_TYPE_IPV6 ? TCPIP_DNS_TYPE_AAAA : TCPIP_DNS_TYPE_A);
    
    if((int)dnsRes < 0)
    {
        return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_DNS_ERROR, (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_SMPTC);
    } 

    // ok, move on
    return SMTPC_STAT_DNS_WAIT;
}

// SMTPC_STAT_DNS_WAIT
static TCPIP_SMTPC_STATUS smtpDcptStateWaitDns(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    IP_MULTI_ADDRESS serverIP;

    TCPIP_DNS_RESULT dnsRes = TCPIP_DNS_IsResolved(pDcpt->smtpMailMessage.smtpServer, &serverIP, pDcpt->addType);
    if((int)dnsRes < 0)
    {
        return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_DNS_ERROR, (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_SMPTC);
    } 
    else if(dnsRes == TCPIP_DNS_RES_PENDING)
    {   // not ready
        return SMTPC_STAT_DNS_WAIT;
    }
    else
    {
        // do nothing
    }

    // DNS done; advance
    pDcpt->serverAdd = serverIP;
    return FC_SmtpStatInc(SMTPC_STAT_DNS_WAIT, +1);
}

// SMTPC_STAT_SOCKET_GET
static TCPIP_SMTPC_STATUS smtpDcptStateGetSkt(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    TCPIP_SMTPC_MESSAGE_RESULT res = TCPIP_SMTPC_RES_OK;

    if(pDcpt->skt == NET_PRES_INVALID_SOCKET)
    {   // get a new socket
        NET_PRES_SKT_HANDLE_T newSkt;
        NET_PRES_SKT_T sktType =  (pDcpt->smtpMailMessage.messageFlags & (uint16_t)TCPIP_SMTPC_MAIL_FLAG_CONNECT_TLS) != 0U ? NET_PRES_SKT_ENCRYPTED_STREAM_CLIENT : NET_PRES_SKT_UNENCRYPTED_STREAM_CLIENT;
        newSkt = NET_PRES_SocketOpen(0, sktType, (NET_PRES_SKT_ADDR_T)pDcpt->addType, pDcpt->smtpMailMessage.serverPort, NULL, NULL);

        if(newSkt == NET_PRES_INVALID_SOCKET)
        {   // failed to open a socket
            res = TCPIP_SMTPC_RES_SKT_OPEN_ERROR;
        }
        else
        {   // success; get the socket TX buffer size 
            TCP_SOCKET_INFO sktInfo;
            (void)NET_PRES_SocketInfoGet(newSkt, &sktInfo);

            if(smtpcSktTxBuffSize == 0U)
            {   // default is used
                smtpcSktTxBuffSize = sktInfo.txSize;
            }

            if(smtpcSktTxBuffSize < (uint16_t)((((uint32_t)TCPIP_SMTPC_PLAIN_LINE_BUFF_SIZE + (uint32_t)SMTPC_PLAIN_LINE_BUFF_SIZE_GUARD) * (uint32_t)SMTPC_PLAIN_LINE_BUFF_SIZE_RATIO) / 100U))
            {
                smtpcSktTxBuffSize = (uint16_t)((((uint32_t)TCPIP_SMTPC_PLAIN_LINE_BUFF_SIZE + (uint32_t)SMTPC_PLAIN_LINE_BUFF_SIZE_GUARD) * (uint32_t)SMTPC_PLAIN_LINE_BUFF_SIZE_RATIO) / 100U);
            }

            // finally check the BASE64 settings
            if(smtpcSktTxBuffSize < ((uint16_t)TCPIP_SMTP_BASE64_BUFF_SIZE * 4U * 18U) / 3U / 10U)
            {
                smtpcSktTxBuffSize = ((uint16_t)TCPIP_SMTP_BASE64_BUFF_SIZE * 4U * 18U) / 3U / 10U;
            }

            if(smtpcSktTxBuffSize > sktInfo.txSize)
            {   // not using the default; adjust
                void* tcpBuffSize = FC_Uint162VPtr(smtpcSktTxBuffSize);
                (void)NET_PRES_SocketOptionsSet(newSkt, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_TX_BUFF, tcpBuffSize);
            }
            if(smtpcSktRxBuffSize != 0U)
            {
                void* tcpBuffSize = FC_Uint162VPtr(smtpcSktRxBuffSize);
                (void)NET_PRES_SocketOptionsSet(newSkt, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_RX_BUFF, tcpBuffSize);
            }
            
            // alert of incoming traffic
            (void)NET_PRES_SocketSignalHandlerRegister(newSkt, (uint16_t)TCPIP_TCP_SIGNAL_RX_DATA, &F_SMTPCSocketRxSignalHandler, NULL);
            pDcpt->skt = newSkt;
        }
    }
    
    if(res == TCPIP_SMTPC_RES_OK)
    {   // point socket to the SMTP server
        if(!NET_PRES_SocketRemoteBind(pDcpt->skt, (NET_PRES_SKT_ADDR_T)pDcpt->addType,  pDcpt->smtpMailMessage.serverPort, &pDcpt->presServerAdd))
        {   // failed
            F_SMTPCDebugCond(false, __func__, __LINE__);
            res = TCPIP_SMTPC_RES_SKT_BIND_ERROR; 
        }
    }

    if(res != TCPIP_SMTPC_RES_OK)
    {
        return smtpcErrorStop(pDcpt, res, (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_SMPTC);
    }

    // advance
    // clear the disconnect condition
    (void)NET_PRES_SocketWasReset(pDcpt->skt);
    (void)NET_PRES_SocketConnect(pDcpt->skt);
    smtpcSetDcptTmo(pDcpt, smtpcServerGenTmo);
    return FC_SmtpStatInc(SMTPC_STAT_SOCKET_GET, +1);
}

// SMTPC_STAT_EHLO_PLAIN; SMTPC_STAT_EHLO_SECURE
static TCPIP_SMTPC_STATUS smtpDcptStateSendEhlo(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{   // send EHLO to server...
    char    ehloBuff[50];

    (void)smtpSetEhloString(pDcpt, ehloBuff, sizeof(ehloBuff));

    TCPIP_SMTPC_CMD_TYPE cmdType = ((pDcpt->smtpMailMessage.messageFlags & (uint16_t)TCPIP_SMTPC_MAIL_FLAG_GREET_HELO) != 0U) ? TCPIP_SMTPC_CMD_HELO : TCPIP_SMTPC_CMD_EHLO;
    return smtpCommandParam(pDcpt, cmdType, ehloBuff, smtpcServerGenTmo);
}


// SMTPC_STAT_TLS_START
// send STARTTLS to the server
static TCPIP_SMTPC_STATUS smtpDcptStateStartTls(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    return smtpCommandParam(pDcpt, TCPIP_SMTPC_CMD_STARTTLS, NULL, smtpcServerGenTmo);
}

// SMTPC_STAT_TLS_WAIT_SECURE
// wait for connection to be secured...
static TCPIP_SMTPC_STATUS smtpDcptStateWaitTlsSecure(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    if(NET_PRES_SocketIsNegotiatingEncryption(pDcpt->skt))
    {   // still negotiating...
        if(smtpcIsDcptTmo(pDcpt))
        {   // negotiation failed; fatal
            return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_SERVER_TMO, (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_SMPTC);
        }
        else
        {   // wait some more
            return SMTPC_STAT_TLS_WAIT_SECURE; 
        }
    }

    // negotiation done
    if(!NET_PRES_SocketIsSecure(pDcpt->skt))
    {   // negotiation failed; fatal
        return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_TLS_FAILED, (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_SMPTC);
    }

    // TLS success; advance
    pDcpt->dcptFlags |= (uint16_t)TCPIP_SMTPC_DCPT_FLAG_SECURED;
    return SMTPC_STAT_EHLO_SECURE;
}

// SMTPC_STAT_AUTH
// start authentication
static TCPIP_SMTPC_STATUS smtpDcptStateAuth(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    if(pDcpt->smtpMailMessage.username == NULL || strlen(pDcpt->smtpMailMessage.username) == 0U || pDcpt->smtpMailMessage.password == NULL || strlen(pDcpt->smtpMailMessage.password) == 0U)
    {
        return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_MESSAGE_AUTH_REQUIRED, (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_NONE);
    }

    if(((pDcpt->dcptFlags & (uint16_t)TCPIP_SMTPC_SERVER_FLAG_AUTH_PLAIN) != 0U) && (pDcpt->smtpMailMessage.messageFlags & (uint16_t)TCPIP_SMTPC_MAIL_FLAG_AUTH_PLAIN) != 0U)
    {
        return smtpAuthPlain(pDcpt);
    }
    else if((pDcpt->dcptFlags & (uint16_t)TCPIP_SMTPC_SERVER_FLAG_AUTH_LOGIN) != 0U)
    {   // start auth login
        return smtpAuthLogin(pDcpt);
    }
    else if((pDcpt->dcptFlags & (uint16_t)TCPIP_SMTPC_SERVER_FLAG_AUTH_PLAIN) != 0U)
    {
        return smtpAuthPlain(pDcpt);
    }
    else
    {
        return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_AUTH_UNKNOWN, (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_NONE);
    }
}

// SMTPC_STAT_AUTH
static TCPIP_SMTPC_STATUS smtpAuthLogin(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    return smtpCommandParam(pDcpt, TCPIP_SMTPC_CMD_AUTH_LOGIN, NULL, smtpcServerGenTmo);

}

// SMTPC_STAT_AUTH
// provides the plain authentication to the server
static TCPIP_SMTPC_STATUS smtpAuthPlain(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    char authBuff[((TCPIP_SMTPC_CLIENT_AUTH_BUFFER_SIZE + 2) / 3) * 3]; // authentication buffer; M3
    char auth64Buff[(sizeof(authBuff) * 4) / 3];        // base64 conversion buffer

    char* pBuff;
    size_t usrLen, passLen;
    uint16_t base64Len;
    TCPIP_SMTPC_STATUS newStat;

    usrLen = strlen(pDcpt->smtpMailMessage.username);
    passLen = strlen(pDcpt->smtpMailMessage.password);

    if( (usrLen + usrLen + passLen) >= (sizeof(authBuff) - 3U))
    {
        return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_MESSAGE_AUTH_LEN_ERROR, (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_NONE);
    }

    pBuff = authBuff;
    // authorization ID:
    (void)strcpy(pBuff, pDcpt->smtpMailMessage.username);
    pBuff += usrLen;
    *pBuff++ = '\0';

    // authentication ID:
    (void)strcpy(pBuff, pDcpt->smtpMailMessage.username);
    pBuff += usrLen;
    *pBuff++ = '\0';

    // passwd
    (void)strcpy(pBuff, pDcpt->smtpMailMessage.password);
    pBuff += passLen;

    base64Len = TCPIP_Helper_Base64Encode((uint8_t*)authBuff, FC_ChPtrDiff2UI16(pBuff, authBuff), (uint8_t*)auth64Buff, (uint16_t)sizeof(auth64Buff));
    auth64Buff[base64Len] = '\0';  // end string properly

    
    newStat = smtpCommandParam(pDcpt, TCPIP_SMTPC_CMD_AUTH_PLAIN, auth64Buff, smtpcServerGenTmo);
    if(newStat == FC_SmtpStatInc(SMTPC_STAT_AUTH, +1))
    {   // everything was OK but we'll jump
        newStat = SMTPC_STAT_AUTH_WAIT_FINAL;
    }

    return newStat;
}

// SMTPC_STAT_AUTH_LOGIN_USER
// send the login user to the server
static TCPIP_SMTPC_STATUS smtpDcptStateAuthLoginUser(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    uint16_t base64Len;

    char    auth64Buff[((TCPIP_SMTPC_CLIENT_AUTH_BUFFER_SIZE + 2) / 3) * 4 + 1];

    size_t userLen = strlen(pDcpt->smtpMailMessage.username);

    if( ((userLen + 2U) * 4U) / 3U > sizeof(auth64Buff) - 1U) 
    {   // exceeding the space in the buffer; truncate
        userLen = (sizeof(auth64Buff) - 1U * 3U) / 4U - 2U;
        smtpcSetWarning(pDcpt, TCPIP_SMTPC_WARN_AUTH_LOGIN_LEN);
    }

    // send the user
    base64Len = TCPIP_Helper_Base64Encode((const uint8_t*)pDcpt->smtpMailMessage.username, (uint16_t)userLen, (uint8_t*)auth64Buff, (uint16_t)sizeof(auth64Buff));
    auth64Buff[base64Len] = '\0';
    return smtpCommandParam(pDcpt, TCPIP_SMTPC_CMD_NONE, auth64Buff, smtpcServerGenTmo);
}

// SMTPC_STAT_AUTH_LOGIN_PASS
// send the login password to the server
static TCPIP_SMTPC_STATUS smtpDcptStateAuthLoginPass(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    uint16_t base64Len;

    char    auth64Buff[((TCPIP_SMTPC_CLIENT_AUTH_BUFFER_SIZE + 2) / 3) * 4 + 1];

    size_t passLen = strlen(pDcpt->smtpMailMessage.password);

    if( ((passLen + 2U) * 4U) / 3U > sizeof(auth64Buff) - 1U) 
    {   // exceeding the space in the buffer; truncate
        passLen = (sizeof(auth64Buff) - 1U * 3U) / 4U - 2U;
        smtpcSetWarning(pDcpt, TCPIP_SMTPC_WARN_AUTH_LOGIN_LEN);
    }

    // send the user
    base64Len = TCPIP_Helper_Base64Encode((const uint8_t*)pDcpt->smtpMailMessage.password, (uint16_t)passLen, (uint8_t*)auth64Buff, (uint16_t)sizeof(auth64Buff));
    auth64Buff[base64Len] = '\0';
    return smtpCommandParam(pDcpt, TCPIP_SMTPC_CMD_NONE, auth64Buff, smtpcServerGenTmo);
}


// SMTPC_STAT_MAIL_START == SMTPC_STAT_MAIL_ENVL_FROM
// start sending the mail
static TCPIP_SMTPC_STATUS smtpDcptStateMailFrom(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    // initialize the mail status
    // set the buffers and files
    pDcpt->smtpAttachBuffer = NULL;
    pDcpt->nSmtpAttachBuffers = 0;
    pDcpt->smtpAttachFile = NULL;
    pDcpt->nSmtpAttachFiles = 0;
    
    return smtpWriteMailEnvAddress(pDcpt, TCPIP_SMTPC_ENV_FROM);
}


// SMTPC_STAT_MAIL_ENVL_RCPT
static TCPIP_SMTPC_STATUS smtpDcptStateMailRcpt(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    return smtpWriteMailEnvAddress(pDcpt, TCPIP_SMTPC_ENV_RCPT);
}

// SMTPC_STAT_MAIL_ENVL_DATA
static TCPIP_SMTPC_STATUS smtpDcptStateMailData(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    return smtpCommandParam(pDcpt, TCPIP_SMTPC_CMD_MAIL_DATA, NULL, smtpcServerGenTmo);
}

// SMTPC_STAT_MAIL_MSG_HDR_DATE
static TCPIP_SMTPC_STATUS smtpDcptMailMsgDate(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    const char* date = pDcpt->smtpMailMessage.date;

#if defined TCPIP_SMTPC_CLIENT_MESSAGE_DATE
    if(date == NULL)
    {
        date = TCPIP_SMTPC_CLIENT_MESSAGE_DATE;
    }
    return smtpClientWriteCmd(pDcpt, "Date:", date);
#else
    if(date != NULL)
    {
        return smtpClientWriteCmd(pDcpt, "Date:", date);
    }

    // skip date field
    return SMTPC_STAT_MAIL_MSG_HDR_FROM;
#endif // defined TCPIP_SMTPC_CLIENT_MESSAGE_DATE
}

// SMTPC_STAT_MAIL_MSG_HDR_FROM
static TCPIP_SMTPC_STATUS smtpDcptMailMsgFrom(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    return smtpClientWriteCmd(pDcpt, "From:",  pDcpt->smtpMailMessage.from);
}

// SMTPC_STAT_MAIL_MSG_HDR_TO
static TCPIP_SMTPC_STATUS smtpDcptMailMsgTo(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    return smtpClientWriteCmd(pDcpt, "To:",  pDcpt->smtpMailMessage.to);
}

// SMTPC_STAT_MAIL_MSG_HDR_SENDER
static TCPIP_SMTPC_STATUS smtpDcptMailMsgSender(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    return SMTPC_STAT_MAIL_MSG_HDR_SUBJECT;
}

// SMTPC_STAT_MAIL_MSG_HDR_SUBJECT
static TCPIP_SMTPC_STATUS smtpDcptMailMsgSubject(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    return smtpClientWriteCmd(pDcpt, "Subject:",  pDcpt->smtpMailMessage.subject);
}

// SMTPC_STAT_MAIL_MSG_HDR_MSG_ID
static TCPIP_SMTPC_STATUS smtpDcptMailMsgMessageId(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    // create a message ID: <mailCounter@IPaddress>
    char addBuff[20] = "";
    char msgIdBuff[40];
    TCP_SOCKET_INFO sktInfo;

    (void)NET_PRES_SocketInfoGet(pDcpt->skt, &sktInfo);
    (void)TCPIP_Helper_IPAddressToString(&sktInfo.localIPaddress.v4Add, addBuff, sizeof(addBuff));
    (void)FC_sprintf(msgIdBuff, sizeof(msgIdBuff), "<%4d@%s>", (int)smtpcMailCounter++, addBuff);

    return smtpClientWriteCmd(pDcpt, "Message-ID:",  msgIdBuff);
}

// SMTPC_STAT_MAIL_MSG_HDR_MIME_HDR
static TCPIP_SMTPC_STATUS smtpDcptMailMsgMimeHdr(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    char mimeHdr[120];
    // Note: the CRLF is added here, last field before the mail body starts!
    // Note: always multipart, even for mesages w/o attachments!
    (void)FC_sprintf(mimeHdr, sizeof(mimeHdr), "MIME-Version: 1.0\r\nContent-Type: multipart/mixed; boundary=\"%s\"\r\n", smtpcMailBoundary);

    return smtpClientWriteCmd(pDcpt, mimeHdr,  NULL);
}

// SMTPC_STAT_MAIL_MSG_HDR_PREAMBLE
static TCPIP_SMTPC_STATUS smtpDcptMailMsgPreambleHdr(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    // preamble: could include an explanatory note!
    // should be ignored, not displayed, etc.
    // not used for now
    return SMTPC_STAT_MAIL_MSG_BODY_HDR;
}

// SMTPC_STAT_MAIL_MSG_BODY_HDR
// send the part header
static TCPIP_SMTPC_STATUS smtpDcptMailMsgBodyHdr(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    const uint8_t* partBody = pDcpt->smtpMailMessage.body;
    size_t partSize = pDcpt->smtpMailMessage.bodySize;

    if(partBody == NULL || partSize == 0U)
    {   // no body mail; skip
        return SMTPC_STAT_MAIL_MSG_BUFF_HDR;
    }

    // set up the body mime parts
    if((pDcpt->smtpMailMessage.messageFlags & (uint16_t)TCPIP_SMTPC_MAIL_FLAG_BODY_TYPE_BINARY) != 0U) 
    {
        pDcpt->smtpPartAttachType = (uint16_t)TCPIP_SMTPC_ATTACH_TYPE_APPLICATION; 
        pDcpt->smtpPartEncodeType = (uint16_t)TCPIP_SMTPC_ENCODE_TYPE_BASE64; 
    }
    else
    {
        pDcpt->smtpPartAttachType = (uint16_t)TCPIP_SMTPC_ATTACH_TYPE_TEXT; 
        if((pDcpt->smtpMailMessage.messageFlags & (uint16_t)TCPIP_SMTPC_MAIL_FLAG_BODY_ENCODE_ASCII) != 0U) 
        {
            pDcpt->smtpPartEncodeType = (uint16_t)TCPIP_SMTPC_ENCODE_TYPE_7BIT; 
        }
        else
        {   // default is BASE64
            pDcpt->smtpPartEncodeType = (uint16_t)TCPIP_SMTPC_ENCODE_TYPE_BASE64; 
        }
    }

    (void)memset(&pDcpt->smtpReadDcpt, 0, sizeof(pDcpt->smtpReadDcpt));
    pDcpt->smtpReadDcpt.pReadObj = &smtpReadBufferObj;
    if(!pDcpt->smtpReadDcpt.pReadObj->fInit(&pDcpt->smtpReadDcpt, NULL, partBody, partSize))
    {
        F_SMTPCAssertCond(false, __func__, __LINE__);
    }
    
    return smtpSendPartHdr(pDcpt, NULL);    // body is not attachment
}

// SMTPC_STAT_MAIL_MSG_BODY
static TCPIP_SMTPC_STATUS smtpDcptMailMsgBody(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    TCPIP_SMTPC_STATUS newStat = smtpSendPartBody(pDcpt);

    if(newStat == FC_SmtpStatInc(SMTPC_STAT_MAIL_MSG_BODY, +1))
    {   // done; there's only one body
        pDcpt->smtpReadDcpt.pReadObj->fDeinit(&pDcpt->smtpReadDcpt);
    } 

    return newStat;

}

// SMTPC_STAT_MAIL_MSG_BUFF_HDR
// send the buffer attachment header
static TCPIP_SMTPC_STATUS smtpDcptMailMsgBuffHdr(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{

    bool buffDone = false;

    if(pDcpt->nSmtpAttachBuffers == pDcpt->smtpMailMessage.nBuffers)
    {   // we're done; jump to sending files
        buffDone =  true;
    }
    else if(pDcpt->nSmtpAttachBuffers == 0U)
    {   // just starting; prepare the buffer attachments
        if(pDcpt->smtpMailMessage.attachBuffers != NULL && pDcpt->smtpMailMessage.nBuffers != 0U)
        {
            pDcpt->smtpAttachBuffer = pDcpt->smtpMailMessage.attachBuffers;
        }
        else
        {
            buffDone = true;
        }
    }
    else
    {
        // do nothing
    }

    if(buffDone)
    {   // we're done; skip the SMTPC_STAT_MAIL_MSG_BUFF_BODY
        return FC_SmtpStatInc(SMTPC_STAT_MAIL_MSG_BUFF_HDR, +2);
    }

    // send this buffer
    if(((uint16_t)pDcpt->smtpAttachBuffer->attachType & (uint16_t)TCPIP_SMTPC_ATTACH_TYPE_TEXT) != 0U)
    {
        pDcpt->smtpPartAttachType = (uint16_t)TCPIP_SMTPC_ATTACH_TYPE_TEXT;
        pDcpt->smtpPartEncodeType = pDcpt->smtpAttachBuffer->attachEncode == TCPIP_SMTPC_ENCODE_TYPE_7BIT ? (uint16_t)TCPIP_SMTPC_ENCODE_TYPE_7BIT: (uint16_t)TCPIP_SMTPC_ENCODE_TYPE_BASE64; 
    }
    else
    {
        pDcpt->smtpPartAttachType = (uint16_t)TCPIP_SMTPC_ATTACH_TYPE_APPLICATION;
        pDcpt->smtpPartEncodeType = (uint16_t)TCPIP_SMTPC_ENCODE_TYPE_BASE64; 
    }

    if(((uint16_t)pDcpt->smtpAttachBuffer->attachType & (uint16_t)TCPIP_SMTPC_ATTACH_TYPE_INLINE) != 0U)
    {
        pDcpt->smtpPartAttachType |= (uint16_t)TCPIP_SMTPC_ATTACH_TYPE_INLINE; 
    }

    (void)memset(&pDcpt->smtpReadDcpt, 0, sizeof(pDcpt->smtpReadDcpt));
    pDcpt->smtpReadDcpt.pReadObj = &smtpReadBufferObj;
    if(!pDcpt->smtpReadDcpt.pReadObj->fInit(&pDcpt->smtpReadDcpt, NULL, pDcpt->smtpAttachBuffer->attachBuffer, pDcpt->smtpAttachBuffer->attachSize))
    {
        F_SMTPCAssertCond(false, __func__, __LINE__);
    }

    return smtpSendPartHdr(pDcpt, pDcpt->smtpAttachBuffer->attachName);
}

// SMTPC_STAT_MAIL_MSG_BUFF_BODY
// send the buffer body
static TCPIP_SMTPC_STATUS smtpDcptMailMsgBuffBody(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    TCPIP_SMTPC_STATUS newStat = smtpSendPartBody(pDcpt);

    if(newStat == FC_SmtpStatInc(SMTPC_STAT_MAIL_MSG_BUFF_BODY, +1))
    {   // done; send another buffer
        pDcpt->smtpReadDcpt.pReadObj->fDeinit(&pDcpt->smtpReadDcpt);
        pDcpt->nSmtpAttachBuffers++;
        pDcpt->smtpAttachBuffer++;
        newStat = SMTPC_STAT_MAIL_MSG_BUFF_HDR;
    } 

    return newStat;
}

// SMTPC_STAT_MAIL_MSG_FILE_HDR
static TCPIP_SMTPC_STATUS smtpDcptMailMsgFileHdr(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    bool fileDone = false;

    if(pDcpt->nSmtpAttachFiles == pDcpt->smtpMailMessage.nFiles)
    {   // we're done; jump to sending files
        fileDone =  true;
    }
    else if(pDcpt->nSmtpAttachFiles == 0U)
    {   // just starting; prepare the file attachments
        if(pDcpt->smtpMailMessage.attachFiles != NULL && pDcpt->smtpMailMessage.nFiles != 0U)
        {
            pDcpt->smtpAttachFile = pDcpt->smtpMailMessage.attachFiles;
        }
        else
        {
            fileDone = true;
        }
    }
    else
    {
        // do nothing
    }

    if(fileDone)
    {   // we're done; skip the SMTPC_STAT_MAIL_MSG_FILE_BODY
        return FC_SmtpStatInc(SMTPC_STAT_MAIL_MSG_FILE_HDR, + 2);
    }

    // send this file
    if(((uint16_t)pDcpt->smtpAttachFile->attachType & (uint16_t)TCPIP_SMTPC_ATTACH_TYPE_TEXT) != 0U)
    {
        pDcpt->smtpPartAttachType = (uint16_t)TCPIP_SMTPC_ATTACH_TYPE_TEXT;
        pDcpt->smtpPartEncodeType = pDcpt->smtpAttachFile->attachEncode == TCPIP_SMTPC_ENCODE_TYPE_7BIT ? (uint16_t)TCPIP_SMTPC_ENCODE_TYPE_7BIT: (uint16_t)TCPIP_SMTPC_ENCODE_TYPE_BASE64; 
    }
    else
    {
        pDcpt->smtpPartAttachType = (uint16_t)TCPIP_SMTPC_ATTACH_TYPE_APPLICATION;
        pDcpt->smtpPartEncodeType = (uint16_t)TCPIP_SMTPC_ENCODE_TYPE_BASE64; 
    }

    if(((uint16_t)pDcpt->smtpAttachFile->attachType & (uint16_t)TCPIP_SMTPC_ATTACH_TYPE_INLINE) != 0U)
    {
        pDcpt->smtpPartAttachType |= (uint16_t)TCPIP_SMTPC_ATTACH_TYPE_INLINE; 
    }

    (void)memset(&pDcpt->smtpReadDcpt, 0, sizeof(pDcpt->smtpReadDcpt));
    pDcpt->smtpReadDcpt.fDcpt.fHandle = SYS_FS_HANDLE_INVALID;
    pDcpt->smtpReadDcpt.pReadObj = &smtpReadFileObj;
    if(!pDcpt->smtpReadDcpt.pReadObj->fInit(&pDcpt->smtpReadDcpt, pDcpt->smtpAttachFile->fileName, NULL, 0))
    {   // couldn't find this file
        smtpcSetWarning(pDcpt, TCPIP_SMTPC_WARN_FILE_NOT_FOUND);
        pDcpt->nSmtpAttachFiles++;
        pDcpt->smtpAttachFile++;
        // try the next one
        return SMTPC_STAT_MAIL_MSG_FILE_HDR;
    }

    return smtpSendPartHdr(pDcpt, pDcpt->smtpAttachFile->fileName);
}

// SMTPC_STAT_MAIL_MSG_FILE_BODY
// send the file body
static TCPIP_SMTPC_STATUS smtpDcptMailMsgFileBody(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    TCPIP_SMTPC_STATUS newStat = smtpSendPartBody(pDcpt);

    if(newStat == FC_SmtpStatInc(SMTPC_STAT_MAIL_MSG_FILE_BODY, + 1))
    {   // done; send another file
        pDcpt->smtpReadDcpt.pReadObj->fDeinit(&pDcpt->smtpReadDcpt);
        pDcpt->nSmtpAttachFiles++;
        pDcpt->smtpAttachFile++;
        newStat = SMTPC_STAT_MAIL_MSG_FILE_HDR;
    } 

    return newStat;
}

// SMTPC_STAT_MAIL_DONE
// send mail done/ended to the server
static TCPIP_SMTPC_STATUS smtpDcptMailMsgDone(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    char endBuff[100];
    (void)FC_sprintf(endBuff, sizeof(endBuff), "\r\n--%s--\r\n.\r\n", smtpcMailBoundary);

    smtpcSetDcptTmo(pDcpt, smtpcServerDataTmo);
    return smtpClientWriteCmd(pDcpt, endBuff, NULL);
}


// SMTPC_STAT_MAIL_DONE_REPORT
static TCPIP_SMTPC_STATUS smtpDcptMailDoneReport(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{

    if(pDcpt->smtpMailMessage.messageCallback != NULL)
    {   // report the outcome to the user 
        TCPIP_SMTPC_MESSAGE_REPORT msgReport;

        msgReport.leftRetries = 0U;
        if((msgReport.messageRes = pDcpt->messageRes) != TCPIP_SMTPC_RES_OK)
        {   // failed somehow
            if((pDcpt->dcptFlags & (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_MASK) != (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_NONE)
            {   // can be retried
                msgReport.leftRetries = pDcpt->retryCnt - 1U;
            }
        }
        
        msgReport.errorStat = (bool) pDcpt->errorStat ? smtpStatusTxlate(pDcpt->errorStat) : TCPIP_SMTPC_MESSAGE_STAT_NONE;
        msgReport.messageWarn = pDcpt->messageWarn;
        (*pDcpt->smtpMailMessage.messageCallback)(pDcpt, &msgReport);
    }

    smtpcSetDcptTmo(pDcpt, smtpcServerGenTmo);

    if(pDcpt->messageRes == TCPIP_SMTPC_RES_OK)
    {   // everything went on just fine
        // just quit (for now, at least; if there are more transactions pending
        // with same server/credentials, we may choose to maintain the server connection open
        // to avoid all the authentication handshake...
        return SMTPC_STAT_TRANSACTION_QUIT;
    }

    // something went wrong
    if((pDcpt->dcptFlags & (uint16_t)TCPIP_SMTPC_DCPT_FLAG_EHLO) != 0U)
    {   // we helloed the server; reset the transaction
        return SMTPC_STAT_TRANSACTION_RESET;
    }

    // failed before getting server hello; terminate it
    return SMTPC_STAT_TRANSACTION_CLOSE;
}

// SMTPC_STAT_TRANSACTION_RESET
// send reset to server
static TCPIP_SMTPC_STATUS smtpDcptTransactionReset(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    if((pDcpt->dcptFlags & (uint16_t)TCPIP_SMTPC_DCPT_FLAG_EHLO) != 0U)
    {   // tmo error should take us to quit 
        smtpcSetErrorJump(pDcpt, SMTPC_STAT_TRANSACTION_QUIT);
        TCPIP_SMTPC_STATUS newStat = smtpCommandParam(pDcpt, TCPIP_SMTPC_CMD_RESET, NULL, smtpcServerGenTmo);
        bool isTmo = smtpcIsDcptTmo(pDcpt);
        if(newStat == pDcpt->currStat && isTmo)
        {   // make sure we're not stuck here trying to send the reset...
            return SMTPC_STAT_TRANSACTION_CLOSE;
        }

        return newStat;
    }

    // if we're not helloed...
    return SMTPC_STAT_TRANSACTION_CLOSE;
}

// SMTPC_STAT_TRANSACTION_QUIT
// send quit to server
static TCPIP_SMTPC_STATUS smtpDcptTransactionQuit(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    if((pDcpt->dcptFlags & (uint16_t)TCPIP_SMTPC_DCPT_FLAG_EHLO) != 0U)
    {
        // tmo error should take us to close 
        smtpcSetErrorJump(pDcpt, SMTPC_STAT_TRANSACTION_CLOSE);
        TCPIP_SMTPC_STATUS newStat = smtpCommandParam(pDcpt, TCPIP_SMTPC_CMD_QUIT, NULL, smtpcServerGenTmo);
        bool isTmo = smtpcIsDcptTmo(pDcpt);
        if(newStat == pDcpt->currStat && isTmo)
        {   // make sure we're not stuck here trying to send the quit...
            return SMTPC_STAT_TRANSACTION_CLOSE;
        }
        else if(pDcpt->messageRes == TCPIP_SMTPC_RES_OK)
        {   // done with the connection
            pDcpt->dcptFlags &= ~(uint16_t)TCPIP_SMTPC_DCPT_FLAG_CONNECTED;
        }
        else
        {
            // do nothing
        }
        return newStat;
    }

    // if we're not helloed...
    return SMTPC_STAT_TRANSACTION_CLOSE;
}

// SMTPC_STAT_TRANSACTION_CLOSE
// close the transaction...
static TCPIP_SMTPC_STATUS smtpDcptTransactionClose(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    TCPIP_SMTPC_STATUS newStat;

    if(pDcpt->skt != NET_PRES_INVALID_SOCKET)
    {
        NET_PRES_SocketClose(pDcpt->skt);
        pDcpt->skt = NET_PRES_INVALID_SOCKET;
    } 
    
    newStat = SMTPC_STAT_FREED;

    if(pDcpt->messageRes != TCPIP_SMTPC_RES_OK)
    {   // failed somehow
        if(pDcpt->retryCnt != 0U && (pDcpt->dcptFlags & (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_MASK) != (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_NONE)
        {   // can be retried
            smtpcSetDcptTmo(pDcpt, (pDcpt->dcptFlags & (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_SERVER) != 0U ? smtpcServerRetryTmo : smtpcInternalRetryTmo);
            pDcpt->retryCnt--;
            newStat = SMTPC_STAT_WAIT_RETRY;
        }
    }

    // done 
    return newStat;
}

// SMTPC_STAT_FREED
// we shouldn't get here
static TCPIP_SMTPC_STATUS smtpDcptTransactionFree(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    F_SMTPCAssertCond(false, __func__, __LINE__);
    return SMTPC_STAT_FREED;
}


// RX processing functions
//

// process incoming data and check the timeout
// mail messages are serialized internally
// we use just one global RX buffer for now
// SMTPC_STAT_SKT_WAIT_CONNECT;
// SMTPC_STAT_EHLO_PLAIN_WAIT; SMTPC_STAT_TLS_WAIT; SMTPC_STAT_EHLO_SECURE_WAIT;
// SMTPC_STAT_AUTH_WAIT_LOGIN_USER; SMTPC_STAT_AUTH_WAIT_LOGIN_PASS; SMTPC_STAT_AUTH_WAIT_FINAL;
// SMTPC_STAT_MAIL_ENVL_FROM_WAIT; SMTPC_STAT_MAIL_ENVL_RCPT_WAIT: SMTPC_STAT_MAIL_ENVL_DATA_WAIT
// SMTPC_STAT_MAIL_DONE_WAIT; SMTPC_STAT_TRANSACTION_RESET_WAIT; SMTPC_STAT_TRANSACTION_QUIT_WAIT
static TCPIP_SMTPC_STATUS smtpDcptStateWaitReply(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    char* currRxPtr;
    size_t  linesNo;
    char    *eolPtr;
    char* lastLinePtr;
    uint16_t avlblBytes, peekBytes;
    char    ch;
    TCPIP_SMTPC_STATUS procStat;

    while((avlblBytes = NET_PRES_SocketReadIsReady(pDcpt->skt)) != 0U)
    {
        if(avlblBytes > sizeof(smtpcRxBuffer))
        {   // server replied more than we could store...warning
            smtpcSetWarning(pDcpt, TCPIP_SMTPC_WARN_REPLY_BUFFER_SMALL);
        }

        // using the socket buffer; the smtpcRxBuffer is shared by all mail messages!
        peekBytes = NET_PRES_SocketPeek(pDcpt->skt,  smtpcRxBuffer, (uint16_t)sizeof(smtpcRxBuffer) - 1U);

        // end properly
        *(smtpcRxBuffer + peekBytes) = '\0';

        // check for properly ended lines in the buffer
        // we assume that the server correctly repeats the code on all lines!
        linesNo = 0;
        lastLinePtr = NULL;
        currRxPtr = smtpcRxBuffer;
        while(true)
        {
            eolPtr = strstr(currRxPtr, "\r\n");
            if(eolPtr == NULL)
            {   // no more
                break;
            }
            lastLinePtr = currRxPtr;
            linesNo++;
            // advance to the next line
            currRxPtr = eolPtr + 2; // strlen("\r\n");
        }

        if(linesNo == 0U)
        {   // no full lines yet
            break;
        }

        // there are some lines in the buffer; search for the end line
        int32_t serverCode = 0;
        (void)FC_Str2L(lastLinePtr, 10, &serverCode);

        if(serverCode == 0)
        {   // that's weird
            F_SMTPCDebugCond(false, __func__, __LINE__);
            smtpcSetWarning(pDcpt, TCPIP_SMTPC_WARN_WRONG_FORMAT);
            break; 
        }

        // check it is the last line
        while(true)
        {
            ch = *lastLinePtr++;
            if(!FC_ChIsDigit(ch))
            {
                break;
            }
        }
        if(ch != ' ')
        {   // last line not in buffer yet
            break; 
        }

        // ok got a full buffer that needs to be processed
        // release the socket buffer
        (void)NET_PRES_SocketRead(pDcpt->skt, smtpcRxBuffer, peekBytes);
        F_SMTPCDbgMailStrFromSrv(pDcpt, smtpcRxBuffer);

        // process
        if(pDcpt->currStat >= sizeof(smtpRxProcessTbl) / sizeof(*smtpRxProcessTbl))
        {
            F_SMTPCAssertCond(false, __func__, __LINE__);
            return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_INTERNAL_ERROR, (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_NONE);
        }

        procStat = (*smtpRxProcessTbl[pDcpt->currStat])(pDcpt, smtpcRxBuffer, linesNo);

        if(pDcpt->smtpMailMessage.replyCallback != NULL)
        {   // user wants to be informed of the server messages
            (*pDcpt->smtpMailMessage.replyCallback)(pDcpt, smtpStatusTxlate(pDcpt->currStat), smtpcRxBuffer);
        }

        return procStat;
    }

    // check for timeout
    if(smtpcIsDcptTmo(pDcpt))
    {
        return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_SERVER_TMO, (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_SERVER);
    }

    // wait some more
    return pDcpt->currStat;
}
// unexpected reply
// execution should not get here!
static TCPIP_SMTPC_STATUS smtpDcptRxProcNone(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines)
{
    F_SMTPCAssertCond(false, __func__, __LINE__);
    // discard all data!
    (void)NET_PRES_SocketDiscard(pDcpt->skt);
    smtpcSetWarning(pDcpt, TCPIP_SMTPC_WARN_UNEXPECTED_REPLY);

    // but continue
    return pDcpt->currStat;
}

// SMTPC_STAT_SKT_WAIT_CONNECT
// process server connect message
static TCPIP_SMTPC_STATUS smtpDcptRxProcConnect(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines)
{
    TCPIP_SMTPC_STATUS newOkStat = SMTPC_STAT_EHLO_PLAIN;

    // received some reply from the server
    // however, if we're forced to connect securely, check first that we have that
    if((pDcpt->smtpMailMessage.messageFlags & (uint16_t)TCPIP_SMTPC_MAIL_FLAG_CONNECT_TLS) != 0U)
    {
        if(!NET_PRES_SocketIsSecure(pDcpt->skt))
        {   // negotiation failed; fatal
            return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_TLS_FAILED, (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_SMPTC);
        }

        pDcpt->dcptFlags |= (uint16_t)TCPIP_SMTPC_DCPT_FLAG_SECURED;
        newOkStat = SMTPC_STAT_EHLO_SECURE; 
    }

    pDcpt->dcptFlags |= (uint16_t)TCPIP_SMTPC_DCPT_FLAG_CONNECTED;

    int32_t serverReplyCode = 0;
    (void)FC_Str2L(replyBuffer, 10, &serverReplyCode);

    if(serverReplyCode != TCPIP_SMTP_CONNECT_SUCCESS_CODE)
    {   // server doesn't accept this connection
        return smtpcServerErrorStop(pDcpt, TCPIP_SMTPC_RES_CONNECTION_REJECT, serverReplyCode);
    }

    // advance
    return newOkStat;
}


// process the server EHLO reply
// SMTPC_STAT_EHLO_PLAIN_WAIT; SMTPC_STAT_EHLO_SECURE_WAIT
static TCPIP_SMTPC_STATUS smtpDcptRxProcEhlo(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines)
{
    size_t lineIx;
    const char *linePtr, *currPtr, *eolPtr;
    char ch;

    uint16_t  srchFlags;
    
    int32_t serverCode = 0;
    (void)FC_Str2L(replyBuffer, 10, &serverCode);

    if(serverCode != TCPIP_SMTP_EHLO_SUCCESS_CODE)
    {   // server doesn't fancy our greeting message..
        return smtpcServerErrorStop(pDcpt, TCPIP_SMTPC_RES_HELLO_REJECT, serverCode);
    }

    // all seems good; see what we got
    pDcpt->dcptFlags |= (uint16_t)TCPIP_SMTPC_DCPT_FLAG_EHLO;
    // clear all authentication 
    pDcpt->dcptFlags &= ~(uint16_t)TCPIP_SMTPC_SERVER_FLAG_AUTH_MASK;

    // this is what we're looking for
    srchFlags = (uint16_t)TCPIP_SMTPC_SERVER_FLAG_TLS | (uint16_t)TCPIP_SMTPC_SERVER_FLAG_AUTH;

    linePtr = replyBuffer;
    for(lineIx = 0; lineIx < nLines; lineIx++)
    {
        if((pDcpt->dcptFlags & srchFlags) == srchFlags)
        {
            break;
        }
        currPtr = linePtr;
        eolPtr = strstr(currPtr, "\r\n");
        if(eolPtr == NULL)
        {   // no more ?
            break;
        }
        
        // skip the reply code and the delimiting chars
        while(true)
        {
            ch = *currPtr;
            if(!FC_ChIsDigit(ch) && !FC_ChIsSpace(ch) && (ch != '-'))
            {
                break;
            }

            currPtr++;
        }

        // search through the codes that interest us
        // check for TLS
        if((pDcpt->dcptFlags & (uint16_t)TCPIP_SMTPC_SERVER_FLAG_TLS) == 0U && strncmp(currPtr, "STARTTLS", strlen("STARTTLS")) == 0)
        {   
            pDcpt->dcptFlags |= (uint16_t)TCPIP_SMTPC_SERVER_FLAG_TLS;
        }
        else if((pDcpt->dcptFlags & (uint16_t)TCPIP_SMTPC_SERVER_FLAG_AUTH) == 0U && strncmp(currPtr, "AUTH", strlen("AUTH")) == 0)
        {   // found the AUTH type
            pDcpt->dcptFlags |= (uint16_t)TCPIP_SMTPC_SERVER_FLAG_AUTH;
            currPtr += strlen("AUTH");

            do
            {
                // skip spaces
                while(currPtr != eolPtr && FC_ChIsSpace(*currPtr))
                {
                    currPtr++;
                }

                if(currPtr != eolPtr)
                {
                    if((pDcpt->dcptFlags & (uint16_t)TCPIP_SMTPC_SERVER_FLAG_AUTH_LOGIN) == 0U && strncmp(currPtr, "LOGIN", strlen("LOGIN")) == 0)
                    {
                        pDcpt->dcptFlags |= (uint16_t)TCPIP_SMTPC_SERVER_FLAG_AUTH_LOGIN;
                        currPtr += strlen("LOGIN");
                    }
                    else if((pDcpt->dcptFlags & (uint16_t)TCPIP_SMTPC_SERVER_FLAG_AUTH_PLAIN) == 0U && strncmp(currPtr, "PLAIN", strlen("PLAIN")) == 0)
                    {
                        pDcpt->dcptFlags |= (uint16_t)TCPIP_SMTPC_SERVER_FLAG_AUTH_PLAIN;
                        currPtr += strlen("PLAIN");
                    }
                    else if((pDcpt->dcptFlags & (uint16_t)TCPIP_SMTPC_SERVER_FLAG_AUTH_MD5) == 0U && strncmp(currPtr, "CRAM-MD5", strlen("CRAM-MD5")) == 0)
                    {
                        pDcpt->dcptFlags |= (uint16_t)TCPIP_SMTPC_SERVER_FLAG_AUTH_MD5;
                        currPtr += strlen("CRAM-MD5");
                    }
                    else
                    {   // ignore
                        while(currPtr != eolPtr)
                        {
                            ch = *currPtr; 
                            if(FC_ChIsSpace(ch))
                            {
                                break;
                            }
                            currPtr++;
                        }
                    }
                }
            }while(currPtr != eolPtr);

            // check if the server hasn't indicated any authentication method
            if((pDcpt->dcptFlags & (uint16_t)TCPIP_SMTPC_SERVER_FLAG_AUTH_SUPPORTED_MASK) == 0U)
            {
                if((pDcpt->smtpMailMessage.messageFlags & (uint16_t)TCPIP_SMTPC_MAIL_FLAG_FORCE_AUTH) != 0U)
                {   // select a default auth scheme
                    pDcpt->dcptFlags |=  (pDcpt->smtpMailMessage.messageFlags & (uint16_t)TCPIP_SMTPC_MAIL_FLAG_AUTH_PLAIN) != 0U ? (uint16_t)TCPIP_SMTPC_SERVER_FLAG_AUTH_PLAIN : (uint16_t)TCPIP_SMTPC_SERVER_FLAG_AUTH_LOGIN;
                }
                else
                {   // no auth...
                    pDcpt->dcptFlags &= ~(uint16_t)TCPIP_SMTPC_SERVER_FLAG_AUTH;
                }
            }
        }
        else
        {
            // do nothing
        }

        // advance to the next line
        linePtr = eolPtr + 2;  // strlen("\r\n"); 
    }

    // start TLS?
    bool doTls = false;
    if(pDcpt->currStat == SMTPC_STAT_EHLO_PLAIN_WAIT && (pDcpt->smtpMailMessage.messageFlags & (uint16_t)TCPIP_SMTPC_MAIL_FLAG_SKIP_TLS) == 0U) 
    {
        if((pDcpt->smtpMailMessage.messageFlags & (uint16_t)TCPIP_SMTPC_MAIL_FLAG_FORCE_TLS) != 0U || (pDcpt->dcptFlags & (uint16_t)TCPIP_SMTPC_SERVER_FLAG_TLS) != 0U)
        {   // either TLS is forced or the server expects it
            doTls = true;
        }
    }

    if(doTls)
    {
        bool tlsSupported = NET_PRES_SocketIsOpenModeSupported(0, NET_PRES_SKT_ENCRYPTED_STREAM_CLIENT);
        if(tlsSupported)
        {
            return SMTPC_STAT_TLS_START;
        } 
    }

    if((pDcpt->dcptFlags & (uint16_t)TCPIP_SMTPC_SERVER_FLAG_AUTH) != 0U)
    {   // server wants authentication
        if((pDcpt->dcptFlags & (uint16_t)TCPIP_SMTPC_SERVER_FLAG_AUTH_SUPPORTED_MASK) == 0U)
        {   // none of the authentication we support...
            return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_AUTH_UNKNOWN, (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_NONE);
        }
        // proceed with the authentication
        return SMTPC_STAT_AUTH;
    }


    // proceed directly to mail
    return SMTPC_STAT_MAIL_START;
}

// process the server reply to STARTTLS
// SMTPC_STAT_TLS_WAIT
static TCPIP_SMTPC_STATUS smtpDcptRxProcTls(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines)
{
    int32_t serverReplyCode = 0;
    (void)FC_Str2L(replyBuffer, 10, &serverReplyCode);

    if(serverReplyCode != TCPIP_SMTP_STARTTLS_SUCCESS_CODE)
    {   // server doesn't want to start TLS???
        return smtpcServerErrorStop(pDcpt, TCPIP_SMTPC_RES_TLS_REJECT, serverReplyCode);
    }

    if(!NET_PRES_SocketEncryptSocket(pDcpt->skt))
    {   // could not start encryption?
        return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_SKT_TLS_ERROR, (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_SMPTC);
    }

    // all fine
    smtpcSetDcptTmo(pDcpt, smtpcTlsHandshakeTmo);
    return SMTPC_STAT_TLS_WAIT_SECURE;
}

// SMTPC_STAT_AUTH_WAIT_LOGIN_USER
// wait for server to request the LOGIN user
static TCPIP_SMTPC_STATUS smtpDcptRxProcAuthLoginUser(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines)
{
    char    authBuff[20];
    const char*   pBuff;
    char*        crPos;
    uint16_t base64Len;

    int32_t serverReplyCode = 0;
    (void)FC_Str2L(replyBuffer, 10, &serverReplyCode);

    if(serverReplyCode != TCPIP_SMTP_AUTH_LOGIN_USER_SUCCESS_CODE)
    {   // server not happy
        return smtpcServerErrorStop(pDcpt, TCPIP_SMTPC_RES_AUTH_LOGIN_REJECT, serverReplyCode);
    }

    // OK, server is happy
    pBuff = replyBuffer;
    while(FC_ChIsDigit(*pBuff) || FC_ChIsSpace(*pBuff))
    {
        pBuff++;
    }
    crPos = strchr(pBuff, (int)'\r');


    if(crPos != NULL)
    {
        (void)memset(authBuff, 0, sizeof(authBuff));
        base64Len = TCPIP_Helper_Base64Decode((const uint8_t*)pBuff, FC_ChPtrDiff2UI16(crPos, pBuff), (uint8_t*)authBuff, (uint16_t)sizeof(authBuff) - 1U);
        authBuff[base64Len] = '\0';
        if(strcmp(authBuff, "Username:") == 0)
        {   // all fine
            return FC_SmtpStatInc(SMTPC_STAT_AUTH_WAIT_LOGIN_USER, + 1);
        }
    }


    // something went wrong
    return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_AUTH_LOGIN_SERVER_ERROR, (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_SERVER);
}


// SMTPC_STAT_AUTH_WAIT_LOGIN_PASS
static TCPIP_SMTPC_STATUS smtpDcptRxProcAuthLoginPass(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines)
{
    char    authBuff[20];
    const char*   pBuff;
    char*        crPos;
    uint16_t base64Len;

    int32_t serverReplyCode = 0;
    (void)FC_Str2L(replyBuffer, 10, &serverReplyCode);

    if(serverReplyCode != TCPIP_SMTP_AUTH_LOGIN_USER_SUCCESS_CODE)
    {   // server not happy
        return smtpcServerErrorStop(pDcpt, TCPIP_SMTPC_RES_AUTH_LOGIN_REJECT, serverReplyCode);
    }

    // OK, server is happy
    pBuff = replyBuffer;
    while(FC_ChIsDigit(*pBuff) || FC_ChIsSpace(*pBuff))
    {
        pBuff++;
    }
    crPos = strchr(pBuff, (int)'\r');


    if(crPos != NULL)
    {
        (void)memset(authBuff, 0, sizeof(authBuff));
        base64Len = TCPIP_Helper_Base64Decode((const uint8_t*)pBuff, FC_ChPtrDiff2UI16(crPos, pBuff), (uint8_t*)authBuff, (uint16_t)sizeof(authBuff) - 1U);
        authBuff[base64Len] = '\0';
        if(strcmp(authBuff, "Password:") == 0)
        {   // all fine
            return FC_SmtpStatInc(SMTPC_STAT_AUTH_WAIT_LOGIN_PASS, + 1);
        }
    }

    // something went wrong
    return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_AUTH_LOGIN_SERVER_ERROR, (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_SERVER);
}

// SMTPC_STAT_AUTH_WAIT_FINAL
static TCPIP_SMTPC_STATUS smtpDcptRxProcAuthFinal(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines)
{
    int32_t serverReplyCode = 0;
    (void)FC_Str2L(replyBuffer, 10, &serverReplyCode);

    if(serverReplyCode != TCPIP_SMTP_AUTH_FINAL_SUCCESS_CODE)
    {   // server doesn't recognize our AUTH!
        return smtpcServerErrorStop(pDcpt, TCPIP_SMTPC_RES_AUTH_REJECT, serverReplyCode);
    }

    // all fine; proceed with mail
    return SMTPC_STAT_MAIL_START;
}

// SMTPC_STAT_MAIL_ENVL_FROM_WAIT
static TCPIP_SMTPC_STATUS smtpDcptRxProcMailFrom(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines)
{
    int32_t serverReplyCode = 0;
    if(FC_Str2L(replyBuffer, 10, &serverReplyCode) < 0)
    {
        // serverCode won't match
    }

    if(serverReplyCode != TCPIP_SMTP_MAIL_FROM_SUCCESS_CODE)
    {   // server rejected mail from
        return smtpcServerErrorStop(pDcpt, TCPIP_SMTPC_RES_MAIL_FROM_REJECT, serverReplyCode);
    }

    // all fine
    return SMTPC_STAT_MAIL_ENVL_RCPT;

}

// SMTPC_STAT_MAIL_ENVL_RCPT_WAIT
static TCPIP_SMTPC_STATUS smtpDcptRxProcMailRcpt(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines)
{
    int32_t serverReplyCode = 0;
    (void)FC_Str2L(replyBuffer, 10, &serverReplyCode);

    if(serverReplyCode != TCPIP_SMTP_MAIL_RCPT_SUCCESS_CODE && serverReplyCode != TCPIP_SMTP_MAIL_RCPT_SUCCESS_ALT)
    {   // server rejected mail from
        return smtpcServerErrorStop(pDcpt, TCPIP_SMTPC_RES_MAIL_RCPT_REJECT, serverReplyCode);
    }

    // all fine; proceed with data
    return SMTPC_STAT_MAIL_ENVL_DATA;
}

// SMTPC_STAT_MAIL_ENVL_DATA_WAIT
static TCPIP_SMTPC_STATUS smtpDcptRxProcMailData(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines)
{
    int32_t serverReplyCode = 0;
    (void)FC_Str2L(replyBuffer, 10, &serverReplyCode);

    if(serverReplyCode != TCPIP_SMTP_MAIL_DATA_SUCCESS_CODE)
    {   // server rejected mail data
        return smtpcServerErrorStop(pDcpt, TCPIP_SMTPC_RES_MAIL_DATA_REJECT, serverReplyCode);
    }

    // all fine
    return SMTPC_STAT_MAIL_MSG_HDR_DATE;
    
}

// SMTPC_STAT_MAIL_DONE_WAIT
static TCPIP_SMTPC_STATUS smtpDcptRxProcMailDoneWait(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines)
{
    int32_t serverReplyCode = 0;
    (void)FC_Str2L(replyBuffer, 10, &serverReplyCode);

    if(serverReplyCode != TCPIP_SMTP_MAIL_SENT_SUCCESS_CODE)
    {   // finally it failed...
        return smtpcServerErrorStop(pDcpt, TCPIP_SMTPC_RES_MAIL_BODY_REJECT, serverReplyCode);
    }

    // mark it as successful
    pDcpt->messageRes = TCPIP_SMTPC_RES_OK;
    return FC_SmtpStatInc(SMTPC_STAT_MAIL_DONE_WAIT, + 1);
}

// SMTPC_STAT_TRANSACTION_RESET_WAIT
// process the server reply to reset
static TCPIP_SMTPC_STATUS smtpDcptRxProcTransactResetWait(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines)
{
    // no matter what the server replies here we move on
    return FC_SmtpStatInc(SMTPC_STAT_TRANSACTION_RESET_WAIT, + 1);
}

// SMTPC_STAT_TRANSACTION_QUIT_WAIT
// process the server reply to quit
static TCPIP_SMTPC_STATUS smtpDcptRxProcTransactQuitWait(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* replyBuffer, size_t nLines)
{
    // no matter what the server replies here we move on and close
    return SMTPC_STAT_TRANSACTION_CLOSE;
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
        smtpCmdStr = NULL;
    }
    else
    {
        F_SMTPCAssertCond((size_t)cmdType < sizeof(smtpCmdStrTbl) / sizeof(*smtpCmdStrTbl), __func__, __LINE__);
        smtpCmdStr = smtpCmdStrTbl[cmdType];
        if(smtpCmdStr == NULL || smtpCmdStr[0] == '\0')
        {   // should not happen!
            F_SMTPCAssertCond(false, __func__, __LINE__);
            return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_INTERNAL_ERROR, (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_NONE);
        }
    }

    newStat = smtpClientWriteCmd(pDcpt, smtpCmdStr, cmdArg);

    if(newStat == FC_SmtpStatInc(pDcpt->currStat, + 1))
    {   // succeeded; advanced
        if(serverTmo != 0U)
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
    uint16_t nBytes;
    int      iBytes;
    char sBuff[200];

    if(cmdStr == NULL && cmdArg == NULL)
    {   // should NOT happen!
        F_SMTPCAssertCond(false, __func__, __LINE__);
        return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_INTERNAL_ERROR, (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_NONE);
    }


    if(cmdStr == NULL)
    {   // output just the arg
        iBytes = FC_sprintf(sBuff, sizeof(sBuff), "%s\r\n", cmdArg); 
    }
    else if(cmdArg != NULL)
    {
        iBytes = FC_sprintf(sBuff, sizeof(sBuff), "%s %s\r\n", cmdStr, cmdArg); 
    }
    else
    {
        iBytes = FC_sprintf(sBuff, sizeof(sBuff), "%s\r\n", cmdStr); 
    }

    F_SMTPCAssertCond(iBytes >= 0, __func__, __LINE__);

    nBytes = (uint16_t)iBytes;
    if(NET_PRES_SocketWriteIsReady(pDcpt->skt, nBytes, 0U) >= nBytes)
    {
        uint16_t sentBytes = NET_PRES_SocketWrite(pDcpt->skt, sBuff, nBytes);
        if(sentBytes != nBytes)
        {
            F_SMTPCAssertCond(false, __func__, __LINE__);
        }
        (void)NET_PRES_SocketFlush(pDcpt->skt);
        F_SMTPCDbgMailStrToSrv(pDcpt, (char*)sBuff, (size_t)nBytes);
        // advance
        return FC_SmtpStatInc(pDcpt->currStat, + 1);
    }

    // wait some more
    return pDcpt->currStat;
}

// formats a mail address into the supplied buffer
// buffer needs to be large enough to take additional <>\0 characters!
static bool smtpFormatMailAddress(const char* mailAddr, char* mailAddrBuff, size_t buffSize)
{
    if(mailAddr != NULL)
    {
        if(strlen(mailAddr) <= (buffSize - 3U))
        {
            mailAddrBuff[buffSize - 1U] = '\0';
            (void)FC_sprintf(mailAddrBuff, buffSize, "<%s>", mailAddr);
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
        return smtpcErrorStop(pDcpt, TCPIP_SMTPC_RES_MESSAGE_ADDR_LEN_ERROR, (uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_NONE);
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
    (void)NET_PRES_SocketInfoGet(pDcpt->skt, &sktInfo);

    if(sktInfo.addressType == IP_ADDRESS_TYPE_IPV4)
    {
        convSuccess = TCPIP_Helper_IPAddressToString(&sktInfo.localIPaddress.v4Add, pAddBuff + 1, buffSize - 3U);
    }
    else
    {
        convSuccess = TCPIP_Helper_IPv6AddressToString (&sktInfo.localIPaddress.v6Add, pAddBuff + 1, buffSize - 3U);
    }

    if(!convSuccess)
    {   // failed; should not happen
        F_SMTPCAssertCond(false, __func__, __LINE__);
        return false;
    }

    // OK
    *pAddBuff = '[';
    pEndBuff = pAddBuff + strlen(pAddBuff);
    *pEndBuff++ = ']';
    *pEndBuff = '\0';

    return true;
}

// sets the waitTick to the timeout value (in seconds!)
static void smtpcSetDcptTmo(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, uint32_t tmo)
{
    uint32_t sysFreq = SYS_TMR_TickCounterFrequencyGet(); 
    pDcpt->waitTick = SYS_TMR_TickCountGet() + (tmo * sysFreq);
}

// returns true if timeout
static bool smtpcIsDcptTmo(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    return FC_UI322I32(SYS_TMR_TickCountGet() - pDcpt->waitTick) >= 0;
}

// an error of some sort occurred and processing is aborted
// sets the result and updates the retry flags
static TCPIP_SMTPC_STATUS smtpcErrorStop(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, TCPIP_SMTPC_MESSAGE_RESULT res, uint16_t dcptRetryFlags)
{
    F_SMTPCDbgDcptErrorState(pDcpt, res);
    pDcpt->errorStat = pDcpt->currStat;
    pDcpt->messageRes = res;
    pDcpt->dcptFlags &= ~((uint16_t)TCPIP_SMTPC_DCPT_FLAG_RETRY_MASK);
    pDcpt->dcptFlags |= dcptRetryFlags;
    return pDcpt->errorJumpStat;
}

// error, but caused by the server reply
static TCPIP_SMTPC_STATUS smtpcServerErrorStop(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, TCPIP_SMTPC_MESSAGE_RESULT res, int serverCode)
{
    return smtpcErrorStop(pDcpt, res, smtpcServerCodeRetryFlag(serverCode));
}

// initializes a descriptor for a retry round
static void smtpcRetryInit(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, uint16_t dcptFlags)
{
    pDcpt->messageRes = TCPIP_SMTPC_RES_PENDING;
    pDcpt->messageWarn = TCPIP_SMTPC_WARN_NONE;
    pDcpt->errorStat = 0;
    pDcpt->dcptFlags = dcptFlags;
    smtpcSetErrorJump(pDcpt, SMTPC_STAT_MAIL_DONE_REPORT);
}

static TCPIP_SMTPC_STATUS smtpSendPartHdr(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt, const char* fileName)
{
    char partHdr[200];
    int32_t hdrLen;
    const char* cntntType;
    const char* encType;
    const char* cntntDisp = NULL;


    if((pDcpt->smtpPartAttachType & (uint16_t)TCPIP_SMTPC_ATTACH_TYPE_APPLICATION) != 0U)
    {
        cntntType = "application/octet-stream";
    }
    else
    {
        cntntType = "text/plain";
    }

    if(pDcpt->smtpPartEncodeType == (uint16_t)TCPIP_SMTPC_ENCODE_TYPE_7BIT)
    {
        encType = "7Bit";
    } 
    else
    {
        encType = "base64";
    } 

    if(fileName != NULL)
    {   // add content disposition
        if((pDcpt->smtpPartAttachType & (uint16_t)TCPIP_SMTPC_ATTACH_TYPE_INLINE) != 0U)
        {
           cntntDisp = "inline";
        }
        else
        {
           cntntDisp = "attachment";
        } 
    }

    hdrLen = FC_sprintf(partHdr, sizeof(partHdr), "\r\n--%s\r\nContent-type: %s\r\nContent-Transfer-Encoding: %s\r\n", smtpcMailBoundary, cntntType, encType);
    if(cntntDisp != NULL)
    {
        (void)FC_sprintf(partHdr + hdrLen, sizeof(partHdr) - (size_t)hdrLen, "Content-Disposition: %s; filename=\"%s\"\r\n", cntntDisp, fileName);
    }

    return smtpClientWriteCmd(pDcpt, partHdr, NULL);
}

static TCPIP_SMTPC_STATUS smtpSendPartBody(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    if(pDcpt->smtpPartEncodeType == (uint16_t)TCPIP_SMTPC_ENCODE_TYPE_7BIT)
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
    uint16_t  writeLen;
    char *crPos, *curr, *wsp;
    char  ch;
    uint16_t sktSpace;
    uint8_t    lfChar;

    TCPIP_SMTP_READ_DCPT* readDcpt = &pDcpt->smtpReadDcpt;

    readStart = smtpLineBuff + 1;
    readBytes = (*readDcpt->pReadObj->fRead)(readDcpt, (uint8_t*)readStart, TCPIP_SMTPC_PLAIN_LINE_BUFF_SIZE);

    if(readBytes == -1)
    {   // part ended; 
        return FC_SmtpStatInc(pDcpt->currStat, + 1);
    }

    readEnd = readStart + readBytes;
    *readEnd = '\0'; // end properly

    writeStart = readStart;
    if(*writeStart == '.')
    {   // line starting with .
        *--writeStart = '.';   // insert extra .
    }

    // search for CR, LF
    crPos = NULL;
    curr = readStart;
    while((ch = *curr++) != '\0')
    {
        if(ch == '\r' || ch == '\n')
        {
            crPos = curr - 1;
            break;
        }
    }

    if(crPos == NULL)
    {   // no line termination in this buffer; find wsp so CRLF can be inserted
        curr = readEnd;
        wsp = NULL;
        do
        {
            ch = *--curr;
            if(ch == ' ' || ch == '\t')
            {
                wsp = curr;
                break;
            }
        }while(curr != readStart);

        if(wsp != NULL)
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
                if((*readDcpt->pReadObj->fRead)(readDcpt, &lfChar, 1) != -1)
                {
                    readBytes++;
                    if(lfChar == (uint8_t)'\n')
                    {   // was in the in stream as well
                        readEnd++;
                    }
                }
                // insert (missing) LF
                *writeEnd++ = '\n';
            }
        }
    }

    writeLen = FC_ChPtrDiff2UI16(writeEnd, writeStart);
    sktSpace = NET_PRES_SocketWriteIsReady(pDcpt->skt, writeLen, 0U);

    if(sktSpace >= writeLen)
    {   // whole line can go through
        (void)NET_PRES_SocketWrite(pDcpt->skt, writeStart, writeLen);
        // adjust the xtra space
        if((xtraBytes = readBytes - (readEnd - readStart)) != 0)
        {
            (void)(*readDcpt->pReadObj->fPush)(readDcpt, xtraBytes);
        }
    }
    else
    {   // couldn't write anything
        (void)(*readDcpt->pReadObj->fPush)(readDcpt, readBytes);
    }

    // more to write
    return pDcpt->currStat;
}


static TCPIP_SMTPC_STATUS smtpSendPartBodyBase64(TCPIP_SMTPC_MESSAGE_DCPT* pDcpt)
{
    int readLen;
    uint16_t ureadLen, sktSpace, minOutLen, maxOutLen, encLen, xtraLen;
    uint16_t base64Len;

    uint8_t smtpReadBuff[((TCPIP_SMTP_BASE64_BUFF_SIZE + 2) / 3) * 3];     // keep it multiple of 3!
    uint8_t smtpEncBuff[(sizeof(smtpReadBuff) * 4) / 3];     // encode buffer

    TCPIP_SMTP_READ_DCPT* readDcpt = &pDcpt->smtpReadDcpt;

    
    readLen = (*readDcpt->pReadObj->fRead)(readDcpt, smtpReadBuff, sizeof(smtpReadBuff));

    if(readLen == -1)
    {   // part ended; 
        return FC_SmtpStatInc(pDcpt->currStat, + 1);
    }


    ureadLen = (uint16_t)readLen;
    minOutLen = (((uint16_t)TCPIP_SMTP_BASE64_MIN_SKT_SIZE + 2U) / 3U) * 4U;
    maxOutLen = ((ureadLen + 2U) / 3U) * 4U;

    if(minOutLen > maxOutLen)
    {
        minOutLen = maxOutLen;
    }

    sktSpace = NET_PRES_SocketWriteIsReady(pDcpt->skt, maxOutLen, minOutLen);

    if(sktSpace < minOutLen)
    {   // no write
        encLen = 0U;
    }
    else
    {   // can write something
        // encode
        encLen = ((sktSpace + 2U) * 3U) / 4U;
        if(encLen > ureadLen)
        {
            encLen = ureadLen;
        }

        base64Len = TCPIP_Helper_Base64Encode(smtpReadBuff, encLen, smtpEncBuff, (uint16_t)sizeof(smtpEncBuff));
        (void)NET_PRES_SocketWrite(pDcpt->skt, smtpEncBuff, base64Len);
    }

    if((xtraLen = ureadLen - encLen) != 0U)
    {
        (void)(*readDcpt->pReadObj->fPush)(readDcpt, xtraLen);
    }
    
    // more to write
    return pDcpt->currStat;
}

// helpers ////////////////////////////////////////////////

// buffer read descriptor
static bool smtpBufferInit(TCPIP_SMTP_READ_DCPT* pRdDcpt, const char* name, const uint8_t* dataBuff, size_t buffSize)
{
    TCPIP_SMTP_READ_DCPT_BUFFER* pRdBuff = &pRdDcpt->bDcpt;

    if(dataBuff != NULL && buffSize != 0U)
    {
        if(pRdBuff->readBuffer == NULL && pRdBuff->readSize == 0U)
        {   // clean
            pRdBuff->readBuffer = dataBuff;
            pRdBuff->readSize = buffSize;
            pRdBuff->readOffset = 0U;
            return true;
        }
        else if(pRdBuff->readBuffer == dataBuff && pRdBuff->readSize == buffSize)
        {   // reuse
            pRdBuff->readOffset = 0U;
            return true;
        }
        else
        {
            // do nothing
        }
    }

    return false;
}

// minimal deinit needed
static void smtpBufferDeinit(TCPIP_SMTP_READ_DCPT* pRdDcpt)
{
    TCPIP_SMTP_READ_DCPT_BUFFER* pRdBuff = &pRdDcpt->bDcpt;
    pRdBuff->readBuffer = NULL;
    pRdBuff->readSize = 0U;
    pRdBuff->readOffset = 0U;
}

// returns -1 for eof
// returns >= 0 number of bytes read
static int smtpBufferRead(TCPIP_SMTP_READ_DCPT* pRdDcpt, uint8_t* pBuffer, size_t nBytes)
{
    TCPIP_SMTP_READ_DCPT_BUFFER* pRdBuff = &pRdDcpt->bDcpt;

    if(pRdBuff->readBuffer == NULL)
    {   // nothing left here
        return -1;
    }

    size_t leftBytes = pRdBuff->readSize - pRdBuff->readOffset;
    if(leftBytes == 0U)
    {   // nothing left here
        return -1;
    }

    if(pBuffer == NULL || nBytes == 0U)
    {
        return 0;
    }

    // read data
    if(nBytes > leftBytes)
    {
        nBytes = leftBytes;
    }
    (void)memcpy(pBuffer, pRdBuff->readBuffer + pRdBuff->readOffset, nBytes);
    pRdBuff->readOffset += nBytes;

    return (int)nBytes;
}

// returns -1 if nothing to push
// returns >=0 number of bytes pushed back 
static int smtpBufferPush(TCPIP_SMTP_READ_DCPT* pRdDcpt, size_t nBytes)
{
    TCPIP_SMTP_READ_DCPT_BUFFER* pRdBuff = &pRdDcpt->bDcpt;

    if(pRdBuff->readBuffer == NULL)
    {   // nothing here
        return -1;
    }

    if(nBytes == 0U)
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

    return (int)nBytes;
}

// file read descriptor
static bool smtpFileInit(TCPIP_SMTP_READ_DCPT* pRdDcpt, const char* name, const uint8_t* dataBuff, size_t buffSize)
{
    TCPIP_SMTP_READ_DCPT_FILE* pRdFile = &pRdDcpt->fDcpt;

    if(name != NULL)
    {
        if(pRdFile->fName == NULL && pRdFile->fHandle == SYS_FS_HANDLE_INVALID)
        {   // clean
            SYS_FS_HANDLE fHandle = SYS_FS_FileOpen(name, SYS_FS_FILE_OPEN_READ);
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
        else
        {
            // do nothing
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
        (void)SYS_FS_FileClose(pRdFile->fHandle);
        pRdFile->fHandle = SYS_FS_HANDLE_INVALID;
    }
    pRdFile->fName = NULL;
}

static int smtpFileRead(TCPIP_SMTP_READ_DCPT* pRdDcpt, uint8_t* pBuffer, size_t nBytes)
{
    TCPIP_SMTP_READ_DCPT_FILE* pRdFile = &pRdDcpt->fDcpt;

    if(pRdFile->fHandle == SYS_FS_HANDLE_INVALID)
    {
        return -1;
    }

    size_t readSize = SYS_FS_FileRead(pRdFile->fHandle, pBuffer, nBytes);
    if(readSize == 0U)
    {
        if(SYS_FS_FileEOF(pRdFile->fHandle))
        {   // true EOF
            return -1;
        }
    }

    return (int)readSize;
}

static int smtpFilePush(TCPIP_SMTP_READ_DCPT* pRdDcpt, size_t nBytes)
{
    TCPIP_SMTP_READ_DCPT_FILE* pRdFile = &pRdDcpt->fDcpt;

    if(pRdFile->fHandle != SYS_FS_HANDLE_INVALID)
    {
        return SYS_FS_FileSeek(pRdFile->fHandle, -(int32_t)nBytes, SYS_FS_SEEK_CUR);
    }
    
    return -1;
}


#endif //#if defined(TCPIP_STACK_USE_SMTPC)
