/*******************************************************************************
  Simple Mail Transfer Protocol Client (SMTPC) Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    smtpc_config.h

  Summary:
    SMTPC configuration file

  Description:
    This file contains the SMTPC module configuration options

*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2016 released Microchip Technology Inc.  All rights reserved.

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
#ifndef _SMTPC_CONFIG_H_
#define _SMTPC_CONFIG_H_


// number of mail connections (sockets) to be created
// each mail message requires a mail connection
// adjust based on the number of simultaneous 
// mail messages in transit
#define  TCPIP_SMTPC_MAIL_CONNECTIONS              3

// default string that identifies the SMTPC client mail date
// Currently there is no date/time service available
// The mail server will update with the current date 
// Keep the current data/time format that's accepted by the SMTP servers
#define TCPIP_SMTPC_CLIENT_MESSAGE_DATE         "Thu, 14 July 2016 14:55:06 -0600"

// general server response timeout, seconds
// Should be adjusted according to the server responsivity
// RFC specifies it as 5 min    
// Default is 1 minute, which should normally be enough
#define TCPIP_SMTPC_SERVER_REPLY_TIMEOUT              60

// server acknowledgment of the mail data: body, attachments et all; seconds
// Should be adjusted according to the server responsivity
// RFC specifies it as 10 min    
// Default is 1 minute, which should normally be enough
#define TCPIP_SMTPC_SERVER_DATA_TIMEOUT              60

// the timeout for the TLS handshake to complete, seconds
// when a secure connection is established to the mail server
// Adjust based on the processing speed, key size, etc.
#define TCPIP_SMTPC_TLS_HANDSHAKE_TIMEOUT            10

// How many times to retry sending a mail message
// Retries occur only for server reported transient errors 
// or for internal temporary errors (DNS, socket errors, etc.)
#define TCPIP_SMTPC_MAIL_RETRIES                    3

// The retry interval because of a server transient error, in seconds.
// RFC specifies it should be at least 30 min!
#define TCPIP_SMTPC_SERVER_TRANSIENT_RETRY_TIMEOUT              (10 * 60)

// The retry interval because of a SMTPC temporary error, in seconds.
// These include temporary errors related to: DNS, socket connection, TLS.
#define TCPIP_SMTPC_INTERNAL_RETRY_TIMEOUT              10

// size of the RX buffer for processing the server replies
// Usually the server replies are not very long so a 512 bytes buffer will normally do
#define TCPIP_SMTPC_SERVER_REPLY_BUFFER_SIZE        512

// size of a buffer that can hold the 2* username and password
// Note: this buffer is automatic, created on the stack
// Check your project stack settings if you need a large buffer here!
// If buffer not long enough this could generate TCPIP_SMTPC_RES_MESSAGE_AUTH_LEN_ERROR.
#define TCPIP_SMTPC_CLIENT_AUTH_BUFFER_SIZE        100

// size of a buffer that can hold an email address: user@domain.smth
// Note: this buffer is automatic, created on the stack
// Check your project stack settings if you need a large buffer here!
// If buffer not long enough this could generate TCPIP_SMTPC_RES_MESSAGE_ADDR_LEN_ERROR.
#define TCPIP_SMTPC_CLIENT_ADDR_BUFFER_SIZE        80


// size of an email line when sending the email body as plain text
// SMTP lines are recommended to be 78 chars long and MUST not exceed 998 characters!
// This implementation limits the line size to TCPIP_SMTPC_PLAIN_LINE_BUFF_SIZE (usually 256 or 512).
// So any mail line longer that this size will have inserted an artificial end of line sequence (CRLF)
// after this many characters.
// Note: this buffer is automatic, created on the stack
// Check your project stack settings if you need a large buffer here!
// Note: Lines are sent out as a whole so the line cannot be longer than the TCP socket TX buffer!
// SMTPC will ensure that this symbol does not exceed the size of the socket TX buffer by allocating
// a larger TX buffer, if needed
#define TCPIP_SMTPC_PLAIN_LINE_BUFF_SIZE         256

// Define the size of the TX buffer for the SMTPC socket
// Use 0 for default TCP socket value
// The SMTPC TX does not need high throughput so the default value is usually fine.
// For transferring large files increase this value as needed.
#define     TCPIP_SMTPC_SKT_TX_BUFF_SIZE   0

// Define the size of the RX buffer for the SMTPC socket
// Use 0 for default TCP socket value
// The SMTPC RX does not need high throughput so the default value is usually fine.
#define     TCPIP_SMTPC_SKT_RX_BUFF_SIZE   0

// SMTPC task rate, milliseconds
// The default value is 55 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_SMTPC_TASK_TICK_RATE           (55)


// Use the sample tcpip console "mail" command for sending
// email from the console
#define TCPIP_SMTPC_USE_MAIL_COMMAND          0


#endif  // _SMTPC_CONFIG_H_



