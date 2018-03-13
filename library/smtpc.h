/*******************************************************************************
  Simple Mail Transfer Protocol Client - SMTPC

  Company:
    Microchip Technology Inc.
    
  File Name:
    smtpc.h

  Summary:
    Module for Microchip TCP/IP Stack.
    
  Description:
    The SMTPC client module in the TCP/IP Stack lets applications send e-mails 
    to any recipient worldwide. These message could include status information 
    or important alerts. Using the e-mail to SMS gateways provided by most cell 
    phone carriers, these messages can also be delivered directly to cell phone 
    handsets.
*******************************************************************************/
//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2016 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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
//DOM-IGNORE-END

#ifndef __SMTPC_H
#define __SMTPC_H

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
    TCPIP_SMTPC_MESSAGE_RESULT

  Summary:
    SMTPC result code

  Description:
    Enumeration describing the possible result codes
    returned by the SMTPC client.

  Remarks:
    None
*/
typedef enum
{
    // positive results
    TCPIP_SMTPC_RES_OK                      = 0,        // OK, operation successful
    TCPIP_SMTPC_RES_PENDING                 = 1,        // operation in progress

    // errors, codes < 0
    // errors with the supplied mail message
    TCPIP_SMTPC_RES_MESSAGE_ERROR           = -1,       // mail message error
    TCPIP_SMTPC_RES_MESSAGE_SERVER_ERROR    = -2,       // message indicated wrong mail server
    TCPIP_SMTPC_RES_MESSAGE_RCPT_ERROR      = -3,       // message mail recipient error: from, to, etc.
    TCPIP_SMTPC_RES_MESSAGE_BUFFER_ERROR    = -4,       // attachment buffer error
    TCPIP_SMTPC_RES_MESSAGE_FILE_ERROR      = -5,       // attachment file error
    TCPIP_SMTPC_RES_MESSAGE_AUTH_REQUIRED   = -6,       // server requires authentication but
                                                        // username or password haven't been provided
    TCPIP_SMTPC_RES_MESSAGE_AUTH_LEN_ERROR  = -7,       // provided credentials are too long, buffer overflow
    TCPIP_SMTPC_RES_MESSAGE_ADDR_LEN_ERROR  = -8,       // email address too long, buffer overflow

    // mail run time errors
    TCPIP_SMTPC_RES_MAIL_BUSY               = -9,       // all mail connections are busy; try later
    TCPIP_SMTPC_RES_DNS_ERROR               = -10,      // failure to resolve server name
    TCPIP_SMTPC_RES_SKT_OPEN_ERROR          = -11,      // failure to open a communication socket
    TCPIP_SMTPC_RES_SKT_BIND_ERROR          = -12,      // failure to bind a socket to the mail server
    TCPIP_SMTPC_RES_SKT_CONNECT_TMO         = -13,      // connection to mail server timeout
    TCPIP_SMTPC_RES_SKT_TLS_ERROR           = -14,      // TLS is required but failed to start TLS on the communication socket
    TCPIP_SMTPC_RES_SERVER_TMO              = -15,      // server timeout
    TCPIP_SMTPC_RES_CONNECTION_REJECT       = -16,      // server rejected the connection
    TCPIP_SMTPC_RES_CONNECTION_CLOSE        = -17,      // server closed the connection
    TCPIP_SMTPC_RES_HELLO_REJECT            = -18,      // server rejected the hello greeting
    TCPIP_SMTPC_RES_AUTH_UNKNOWN            = -19,      // server requires authentication mechanism unsupported by SMTPC
                                                        // Currently LOGIN and PLAIN authentications are supported
    TCPIP_SMTPC_RES_AUTH_LOGIN_REJECT       = -20,      // server rejected the login authentication request
    TCPIP_SMTPC_RES_AUTH_LOGIN_SERVER_ERROR = -21,      // unexpected server reply to login authentication request

    TCPIP_SMTPC_RES_AUTH_REJECT             = -22,      // server rejected the supplied authentication
    TCPIP_SMTPC_RES_TLS_REJECT              = -23,      // server rejected the TLS start
    TCPIP_SMTPC_RES_TLS_FAILED              = -24,      // TLS session negotiation failed
    TCPIP_SMTPC_RES_TLS_TMO                 = -25,      // TLS session timeout
    TCPIP_SMTPC_RES_MAIL_FROM_REJECT        = -26,      // server rejected the "from" address 
    TCPIP_SMTPC_RES_MAIL_RCPT_REJECT        = -27,      // server rejected the "recipient" address 
    TCPIP_SMTPC_RES_MAIL_DATA_REJECT        = -28,      // server rejected the "data" field 
    TCPIP_SMTPC_RES_MAIL_BODY_REJECT        = -29,      // server rejected the mail body 

    // SMTPC errors
    TCPIP_SMTPC_RES_INITIALIZE_ERROR        = -40,      // module is not properly initialized, API call is invalid

    TCPIP_SMTPC_RES_INTERNAL_ERROR          = -41,      // internal failure, should not happen
}TCPIP_SMTPC_MESSAGE_RESULT;

// *****************************************************************************
/*
  Enumeration:
    TCPIP_SMTPC_MESSAGE_WARNING

  Summary:
    SMTPC warning type

  Description:
    Enumeration describing the possible warning types
    occurring during a mail transmission.
    These events are not fatal and SMTPC client will try
    to complete the mail transfer.

  Remarks:
    Multiple warnings could be set during a mail transmission
*/
typedef enum
{
    TCPIP_SMTPC_WARN_REPLY_BUFFER_SMALL     = 0x0001,       // the reply buffer allocated for the server replies is too small
                                                            // server reply exceeded the size of the buffer
                                                            // it should be increased
    TCPIP_SMTPC_WARN_WRONG_FORMAT           = 0x0002,       // server reply was not properly formatted
    TCPIP_SMTPC_WARN_UNEXPECTED_REPLY       = 0x0004,       // unexpected server replied received
    TCPIP_SMTPC_WARN_FILE_NOT_FOUND         = 0x0008,       // file attachment not found; ignored
    TCPIP_SMTPC_WARN_AUTH_LOGIN_LEN         = 0x0010,       // provided credentials for LOGIN authentication are too long and were truncated (buffer overflow)
                                                            // this most likely will result in a server authentication rejection

}TCPIP_SMTPC_MESSAGE_WARNING;


// *****************************************************************************
/*
  Type:
    TCPIP_SMTPC_MESSAGE_HANDLE

  Summary:
    Defines a handle to a mail message

  Description:
    Data type describing a handle that identifies a mail message
    that's scheduled for transmission.
    This handle is to be used for retrieving the message status.
    This handle will be part of the callback that
    the SMTP client will use to notify the user of the mail completion.

    A valid handle is != 0.

  Remarks:
    None
*/
typedef const void* TCPIP_SMTPC_MESSAGE_HANDLE;


// *****************************************************************************
/*
  Enumeration:
    TCPIP_SMTPC_MESSAGE_STATUS

  Summary:
    SMTPC message status

  Description:
    Enumeration describing the possible states of a mail message
    during a mail transmission.
    The message progresses through various states until complete mail transfer or error

  Remarks:
    TCPIP_SMTPC_MESSAGE_STAT_HELLO will occur twice if TLS is supported
    by the server. Once the connection is secured, another HELLO message will be sent to the server.

*/
typedef enum
{
    // there is no transaction started
    TCPIP_SMTPC_MESSAGE_STAT_NONE,
    // doing the DNS resolution
    TCPIP_SMTPC_MESSAGE_STAT_DNS,
    // connecting to the server
    TCPIP_SMTPC_MESSAGE_STAT_CONNECT,
    // saying hello to the server
    TCPIP_SMTPC_MESSAGE_STAT_HELLO,
    // starting the TLS communication with the server
    TCPIP_SMTPC_MESSAGE_STAT_TLS,
    // authenticating to the server
    TCPIP_SMTPC_MESSAGE_STAT_AUTH,
    // sending mail envelope
    TCPIP_SMTPC_MESSAGE_STAT_MAIL_ENVELOPE,
    // sending mail body 
    TCPIP_SMTPC_MESSAGE_STAT_MAIL_BODY,
    // sending mail buffers
    TCPIP_SMTPC_MESSAGE_STAT_MAIL_BUFFERS,
    // sending mail files
    TCPIP_SMTPC_MESSAGE_STAT_MAIL_FILES, 
    // waiting for the mail acknowledge
    TCPIP_SMTPC_MESSAGE_STAT_MAIL_ACKNOWLEDGE,
    // reporting the mail outcome
    TCPIP_SMTPC_MESSAGE_STAT_MAIL_REPORT,
    // closing the mail transaction
    TCPIP_SMTPC_MESSAGE_STAT_CLOSING,
    // waiting for a retry
    TCPIP_SMTPC_MESSAGE_STAT_WAIT_RETRY,
}TCPIP_SMTPC_MESSAGE_STATUS;

// *****************************************************************************
/*
  Structure:
    TCPIP_SMTPC_MESSAGE_REPORT

  Summary:
    Structure defining a mail message report

  Description:
    This data structure describes a mail message report.
    It is used by the SMTPC to report an mail delivery result.
   
  Remarks:
    None

*/

typedef struct
{
    TCPIP_SMTPC_MESSAGE_RESULT  messageRes;     // the result of the message
                                                // TCPIP_SMTPC_RES_OK if the message was successfully transmited
                                                // else some error were encountered
    TCPIP_SMTPC_MESSAGE_STATUS  errorStat;      // the mail message status in which the error occurred, if any
                                                // Else the TCPIP_SMTPC_MESSAGE_STAT_NONE will be used
    TCPIP_SMTPC_MESSAGE_WARNING messageWarn;    // warnings encountered during the message transmission, if any
    int                         leftRetries;    // the number of retries that will still be attempted
                                                // if error was a transient, not fatal
}TCPIP_SMTPC_MESSAGE_REPORT;


// *****************************************************************************
/*
  Type:
    TCPIP_SMTPC_MESSAGE_CALLBACK

  Summary:
    Defines a mail message callback

  Description:
    Data type describing a function that will be called by the SMTPC module
    upon completion of the mail message.

Parameters:
    messageHandle   - handle identifying the message.
                      This handle was obtained with a call to TCPIP_SMTPC_MailMessage().

    pMailReport     - pointer to a TCPIP_SMTPC_MESSAGE_REPORT data structure containing the 
                      result of the message delivery:
                        messageRes  - the result of the message
                        errorStat   - error status    
                        messageWarn - warnings
                        leftRetries - number of times the delivery will be re-attempted.
    
  Remarks:
    A message handle is used rather than the user's TCPIP_SMTPC_MAIL_MESSAGE.
    This allows sending multiple copies of the message without waiting for a previous copy to 
    be done.

    This notification can be called multiple times if the delivery attempt failed for a transient reson.
    The number of left retries tells if another attempt will be retried.

    Once this final notification function is called (left retries == 0),
    the SMTPC no longer uses the TCPIP_SMTPC_MAIL_MESSAGE
    data (unless there are other copies of this message in transit).
    This messageHandle is no longer valid and should not be used again.

    The pMailReport points to a SMTPC owned read only data structure.
    It should be used only to inspect the message delivery result. 

*/
typedef void (*TCPIP_SMTPC_MESSAGE_CALLBACK)(TCPIP_SMTPC_MESSAGE_HANDLE messageHandle, const TCPIP_SMTPC_MESSAGE_REPORT* pMailReport);

// *****************************************************************************
/*
  Type:
    TCPIP_SMTPC_SERVER_REPLY_CALLBACK

  Summary:
    Defines a mail server reply callback

  Description:
    Data type describing a function that will be called by the SMTPC module
    upon receiving a message from the mail server.
    SMTPC will parse and process the server messages anyway but it can optionally pass
    the server replies to the user. 

Parameters:
    messageHandle   - handle identifying the message.
                      This handle was obtained with a call to TCPIP_SMTPC_MailMessage().
    currStat        - the current status for the mail message
    serverReply     - the reply sent by the server

    
  Remarks:
    None

*/
typedef void (*TCPIP_SMTPC_SERVER_REPLY_CALLBACK)(TCPIP_SMTPC_MESSAGE_HANDLE messageHandle, TCPIP_SMTPC_MESSAGE_STATUS currStat, const char* serverReply);

// *****************************************************************************
/*
  Enumeration:
    TCPIP_SMTPC_ATTACH_TYPE

  Summary:
    SMTPC attachment type

  Description:
    Enumeration describing the possible attachment types
    supported by the SMTPC client.

  Remarks:
    Only one attachment type should be set at a time.
*/

typedef enum
{
    TCPIP_SMTPC_ATTACH_TYPE_UNKNOWN     = 0x0000,       // unknown attachment type; will default to binary
    TCPIP_SMTPC_ATTACH_TYPE_APPLICATION = 0x0001,       // application octet stream, binary
                                                        // Use this attachment type when sending binary data
    TCPIP_SMTPC_ATTACH_TYPE_TEXT        = 0x0002,       // plain text
                                                        // Use this attachment type when sending ASCII text
    // other supported types will be added here
    //


    TCPIP_SMTPC_ATTACH_TYPE_INLINE      = 0x8000,       // flag for sending the data as inline instead
                                                        // of attachment

}TCPIP_SMTPC_ATTACH_TYPE;



// *****************************************************************************
/*
  Enumeration:
    TCPIP_SMTPC_ENCODE_TYPE

  Summary:
    SMTPC encoding type

  Description:
    Enumeration describing the possible encoding types
    supported by the SMTPC client.

  Remarks:
    Normally plain text should be sen using the 7bit encoding type, i.e. plain ASCII.
    However, selecting base64 encoding has the advantage that the lines are not limited in
    length and any sequence of characters can be present in the email body without the 
    need of the SMTPC to search and replace the SMTP forbidden characters.

*/

typedef enum
{
    TCPIP_SMTPC_ENCODE_TYPE_UNKNOWN,        // unknown encoding type; defaults to base64
    TCPIP_SMTPC_ENCODE_TYPE_7BIT,           // 7bit encoding type: no encoding done, 7 bit ASCII
                                            // This doesn't do any encoding. Use it for plain text.
    TCPIP_SMTPC_ENCODE_TYPE_BASE64,         // base64 encoding applied
                                            // Mandatory for binary data; could be used for plain data as well.
    // other supported types will be added here
    //
}TCPIP_SMTPC_ENCODE_TYPE;


// *****************************************************************************
/*
  Structure:
    TCPIP_SMTPC_ATTACH_BUFFER

  Summary:
    Buffer attachment to the mail.

  Description:
    This data structure describes a mail buffer attachment.

  Remarks:
   A buffer attachment is attached to the email just like a file.
   But data is taken from the persistent user buffer rather than from a file.

   If the attach type is application octet stream then
   the suggested encoding will be overwritten and BASE64 encoding will be used.

   The user supplied buffer has to be persistent until
   the mail completion callback will be called.

*/

typedef struct
{
    TCPIP_SMTPC_ATTACH_TYPE attachType;         // type of the contained data
    TCPIP_SMTPC_ENCODE_TYPE attachEncode;       // preferred encoding
    const char*             attachName;         // file name to be used for the attachment
    const uint8_t*          attachBuffer;       // buffer containing the data
    size_t                  attachSize;         // buffer size
}TCPIP_SMTPC_ATTACH_BUFFER;


// *****************************************************************************
/*
  Structure:
    TCPIP_SMTPC_ATTACH_FILE

  Summary:
    File attachment to the mail.

  Description:
    This data structure describes a mail file attachment.

  Remarks:
   The specified file will be opened for reading and attached to the email.

   If the attach type is application octet stream then
   the suggested encoding will be overwritten and BASE64 encoding will be used.

   The file has to be persistent until
   the mail completion callback will be called.

   If file could not be opened when the mail is sent
   a warning will be issued and the file will be ignored.
*/

typedef struct
{
    TCPIP_SMTPC_ATTACH_TYPE attachType;         // type of the contained data
    TCPIP_SMTPC_ENCODE_TYPE attachEncode;       // preferred encoding
    const char*             fileName;           // name of the file to be opened
}TCPIP_SMTPC_ATTACH_FILE;

// *****************************************************************************
/*
  Enumeration:
    TCPIP_SMTPC_MAIL_FLAGS

  Summary:
    Mail message flags

  Description:
    Enumeration describing the possible mail message flags
    supported by the SMTPC client.

  Remarks:

    Multiple flags can be set

    Only 16 bit flags supported

    If no flags are set, then the default settings are used for the mail message.

    Normally the mail body is plain text.
    Using these flags, a binary mail body can be sent.
    
    Normally the mail body is plain text so a standard ASCII encoding is used.
    Using these flags, a BASE64 encoding for the mail body can be selected.
    Note that if the mail body is selected to be binary then the BASE64 encoding
    is automatically selected.

    BASE64 encoding has the advantage that the mail body lines are not limited in
    length and any sequence of characters can be present in the email body without the 
    need of the SMTPC to search and replace the SMTP forbidden characters.

    Normally the connection to the server occurs on a standard SMTP port: 25 or (preferred) 587
    as a plain/non-encrypted connection.
    Once the server replies with TLS support, the encryption is started.
    Using the flags you can force the connection to be TLS from the very beginning.
    Or you can avoid engaging in TLS even if the server supports it.
    Or try to start TLS even if the server didn't advertise support for it.

*/

typedef enum
{
    TCPIP_SMTPC_MAIL_FLAG_USE_IPV6          = 0x0001,       // use IPv6 connection to the server, rather than default IPv4
    TCPIP_SMTPC_MAIL_FLAG_BODY_TYPE_BINARY  = 0x0002,       // mail body is binary rather then standard plain text
    TCPIP_SMTPC_MAIL_FLAG_BODY_ENCODE_ASCII = 0x0004,       // send mail body as plain ASCII encoding rather than default BASE64


    TCPIP_SMTPC_MAIL_FLAG_CONNECT_TLS       = 0x0010,       // connect with TLS from the very beginning
                                                            // This flag is useful if the server accepts encrypted connections on port 465, for example
                                                            // Note that port 465 is not really a SMTP port, although supported by many servers!
    TCPIP_SMTPC_MAIL_FLAG_SKIP_TLS          = 0x0020,       // skip TLS even if the server supports it
                                                            // most servers won't accept mail if TLS is skipped though
    TCPIP_SMTPC_MAIL_FLAG_FORCE_TLS         = 0x0040,       // start TLS even if the server does not indicate support for it


    TCPIP_SMTPC_MAIL_FLAG_AUTH_PLAIN        = 0x0100,       // favor PLAIN authentication, if supported
                                                            // by default the LOGIN authentication is selected 

    TCPIP_SMTPC_MAIL_FLAG_FORCE_AUTH        = 0x0200,       // force authentication even if server doesn't specifically request one
                                                            // by default the authentication is skipped 

    TCPIP_SMTPC_MAIL_FLAG_GREET_HELO        = 0x0400,       // use old 'HELO' greeting insted of extended 'EHLO' which is default

    // other supported flags will be added here
    //
}TCPIP_SMTPC_MAIL_FLAGS;


// *****************************************************************************
/*
  Structure:
    TCPIP_SMTPC_MESSAGE_QUERY

  Summary:
    Structure defining a run time query for a mail message

  Description:
    This data structure describes a mail message query at run time.
   
  Remarks:
    The message status is for info purposes only.
    The message status changes as is processed by the SMTPC module.  

*/

typedef struct
{
    TCPIP_SMTPC_MESSAGE_STATUS  messageStat;    // the current status of the mail message
    TCPIP_SMTPC_MESSAGE_WARNING messageWarn;    // the current warnings encountered during the message transmission
    int                         messageRetries; // the current number of retries for this message
    NET_PRES_SKT_HANDLE_T       messageSkt;     // the socket associated with this message
}TCPIP_SMTPC_MESSAGE_QUERY;

// *****************************************************************************
/*
  Structure:
    TCPIP_SMTPC_MAIL_MESSAGE

  Summary:
    Mail message descriptor.

  Description:
    This data structure describes a mail message

  Remarks:
    SMTPC does NOT make private copies of the strings, buffers, etc.
    presented in a TCPIP_SMTPC_MAIL_MESSAGE data structure.
    All fields that are pointers to strings, buffers, files names, etc.
    MUST be persistent until the mail complete notification is received.

    The TCPIP_SMTPC_MAIL_MESSAGE data structure itself does not need
    to be persistent.

    The mail body is not a char* but uint8_t* to allow sending binary data as well
    as part of the mail body.
    This requires though that the bodySize member is present.

   If some file could not be opened when the mail is sent
   a warning will be issued and the file will be ignored.

   Currently the fields: "sender", "cc" and "bcc" are not used.
   They will be eventually added.
*/

typedef struct
{
    // Mandatory Mail Fields
    //
    // recipient return address
    const char*                         from;
    // destination address
    const char*                         to;

    // Optional Mail Fields
    //
    // if sent on behalf of someone else
    const char*                         sender;
    // carbon copy destination address
    const char*                         cc;
    // blind carbon copy destination address
    const char*                         bcc;
    // date for the mail message
    // Should be formatted like: "Wed, 20 July 2016 14:55:06 -0600"
    // if NULL, then the TCPIP_SMTPC_CLIENT_MESSAGE_DATE will be used
    const char*                         date;

    // Regular Mail Fields
    //
    // mail subject line
    const char*                         subject;
    // mail body; main body of the mail
    const uint8_t*                      body;
    size_t                              bodySize;  

    // Mail Attachments
    //
    // number of attached buffers
    int                                 nBuffers;
    // array of buffer attachments
    const TCPIP_SMTPC_ATTACH_BUFFER*    attachBuffers;

    // number of attached files
    int                                 nFiles;
    // array of file attachments
    const TCPIP_SMTPC_ATTACH_FILE*      attachFiles;

    // Mail Authentication
    //
    // username to be presented when the mail server requires authentication
    const char*                         username;
    // associated password to be presented when the mail server requires authentication
    const char*                         password;

    // Mail Server
    //
    // address or name of a server to send the mail to
    // (smtp.gmail.com, 74.125.23.109, etc.)
    const char*                         smtpServer;
    // port on which the SMTP server listens on
    // Standard SMTP ports are 25 or 587
    // For servers supporting TLS (gmail, yahoo, etc.),
    // the recommended port is 587
    uint16_t                            serverPort;

    // Additional message flags
    TCPIP_SMTPC_MAIL_FLAGS              messageFlags;

    // Mail Notifications
    //
    // message completion notification
    // could be NULL if not needed
    TCPIP_SMTPC_MESSAGE_CALLBACK        messageCallback;

    // server reply notification
    // could be NULL if not needed
    TCPIP_SMTPC_SERVER_REPLY_CALLBACK   replyCallback;
}TCPIP_SMTPC_MAIL_MESSAGE;



// *****************************************************************************
/*
  Structure:
    TCPIP_SMTPC_MODULE_CONFIG

  Summary:
    SMTPC configuration data.

  Description:
    This data structure describes the SMTPC configuration data.
    It is presented to the module as part of the module initialization.


  Remarks:
    None

*/
typedef struct
{
    int         nMailConnections;   // number of mail connections to be created
                                    // each mail message to be sent requires a mail connection
                                    // adjust based on the number of simultaneous 
                                    // mail messages in transit
    uint32_t    serverReplyTmo;     // general server response timeout, seconds
                                    // Should be adjusted according to the server responsivity
                                    // RFC specifies it as 5 min    
    uint32_t    serverDataTmo;      // server response timeout to acknowledge a received mail body/data, seconds
                                    // Should be adjusted according to the server responsivity
                                    // RFC specifies it as 10 min    
    uint32_t    tlsHandshakeTmo;    // secure connection establishment timeout, seconds
                                    // usually few seconds

    int         nMailRetries;       // how many times to retry sending a mail message
                                    // Retries occur only for server reported transient errors
    uint32_t    serverRetryTmo;     // The retry interval because of a server transient error, in seconds.
                                    // RFC specifies it should be at least 30 min!    
    uint32_t    smtpcRetryTmo;      // The retry interval because of a SMTPC temporary error, in seconds.
                                    // These include temporary errors related to: DNS, socket connection, TLS. 
    uint16_t    sktTxBuffSize;      // size of TX buffer for the associated socket; leave 0 for default
    uint16_t    sktRxBuffSize;      // size of RX buffer for the associated socket; leave 0 for default
                                    // Note: for the RX buffer, the default setting should be used, as that should be sufficient
                                    // The mail server replies are not very long, however a minimum of 256 bytes should be available
                                    //
                                    //
    
}TCPIP_SMTPC_MODULE_CONFIG;


// *****************************************************************************
// *****************************************************************************
// Section: SMTPC Function Prototypes
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Function:
    TCPIP_SMTPC_MESSAGE_HANDLE  TCPIP_SMTPC_MailMessage(const TCPIP_SMTPC_MAIL_MESSAGE* pMailMessage, TCPIP_SMTPC_MESSAGE_RESULT* pRes);

  Summary:
    SMTPC mail message function.

  Description:
    This function sends a mail message to the specified server.

  Precondition:
    SMTPC module should have been initialized.

  Parameters:
    pMailMessage    - pointer to a TCPIP_SMTPC_MAIL_MESSAGE describing the mail message to be sent

    pRes            - address to store the operation result
                      If the call failed, additional info will be reported with this result
                      Can be NULL if not needed.

  Returns:
    a valid handle (!= 0)   - if mail message scheduled successfully
    0                       - if call failed

    an error code otherwise

  Remarks:

    The returned handle is valid until the final mail notification function is called
    (TCPIP_SMTPC_MESSAGE_CALLBACK) if exists,
    or until the mail transaction is completed.
    After that the messageHandle is no longer valid and should not be used again.

*/
TCPIP_SMTPC_MESSAGE_HANDLE  TCPIP_SMTPC_MailMessage(const TCPIP_SMTPC_MAIL_MESSAGE* pMailMessage, TCPIP_SMTPC_MESSAGE_RESULT* pRes);

// *****************************************************************************
/*
  Function:
    TCPIP_SMTPC_MESSAGE_RESULT  TCPIP_SMTPC_MessageQuery(TCPIP_SMTPC_MESSAGE_HANDLE messageHandle, TCPIP_SMTPC_MESSAGE_QUERY* pQuery);

  Summary:
    SMTPC mail message query function.

  Description:
    This function allows the query of a mail message at run time.
   
  Precondition:
    SMTPC module should have been initialized.

  Parameters:
    messageHandle   - handle identifying the message.
                      This handle was obtained with a call to TCPIP_SMTPC_MailMessage().

    pQuery          - pointer to a TCPIP_SMTPC_MESSAGE_QUERY structure that will be updated with the current
                      status of the mail message

  Returns:
    TCPIP_SMTPC_RES_OK              - if message query updated successfully
    TCPIP_SMTPC_RES_MESSAGE_ERROR   - if no such message exists


  Remarks:
    The message status is for info purposes only.
    The message status changes as is processed by the SMTPC module.  

    Once the mail transfer is complete and the corresponding notification function is called,
    the messageHandle is no longer valid and should not be used again.

*/
TCPIP_SMTPC_MESSAGE_RESULT  TCPIP_SMTPC_MessageQuery(TCPIP_SMTPC_MESSAGE_HANDLE messageHandle, TCPIP_SMTPC_MESSAGE_QUERY* pQuery);

// *****************************************************************************
/*
  Function:
    void  TCPIP_SMTPC_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    this function performs SMTPC module tasks in the TCP/IP stack.

  Precondition:
    SMTPC module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_SMTPC_Task(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // __SMTPC_H

