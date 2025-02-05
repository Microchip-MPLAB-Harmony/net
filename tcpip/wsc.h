/*******************************************************************************
  WebSocket Client Protocol (WS) Header file

  Company:
    Microchip Technology Inc.

  File Name:
   wsc.h

  Summary:
    WebSocket Client Protocol (WS) header file.

  Description:
    This source file contains the WS Client module API definitions.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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




#ifndef H_WSC_H_
#define H_WSC_H_

#include <stdbool.h>

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Enumeration:
    TCPIP_WSC_RES

  Summary:
    Result of a WSC operation.

  Description:
    Result codes used for a WSC operation.

  Remarks:
    None
*/
typedef enum
{
    // success codes
    TCPIP_WSC_RES_OK                = 0,    // operation succeeded

    TCPIP_WSC_RES_BUSY              = 1,    // the connection is busy processing an incoming message from the server
                                            // Retry the operation
                                            // Note: this may occur only under an RTOS operation when specific synchronization option is used
                                            // and will be eventually added. It is not currently supported!
    TCPIP_WSC_RES_PENDING           = 2,    // the operation scheduling is pending and the user will be later notified 
                                            // of the outcome
                                            // Note: this is for the asynchronous operation mode that will be eventually added.
                                            // It is not currently supported !
    TCPIP_WSC_RES_MORE_DATA         = 3,    // a message read/write operation has partially completed
                                            // but there's more of the message to be read/written
                                            // Retry: operation should be called again

    TCPIP_WSC_RES_NO_WHOLE_SPACE    = 4,    // Not enough space to write the message as a whole
                                            // (TCPIP_WSC_MSG_FLAG_TX_WHOLE was set)
                                            // Retry the operation.

    // failure codes ( < 0)
    TCPIP_WSC_RES_MODULE_ERROR      = -1,   // module error: not initialized or other fatal state
    TCPIP_WSC_RES_CONNECTING        = -2,   // a connection to the same server already in progress
    TCPIP_WSC_RES_CONN_UNAVAIL      = -3,   // all connections busy, no new one available
    TCPIP_WSC_RES_SRV_NAME_ERROR    = -4,   // the server name is too long
    TCPIP_WSC_RES_RESOURCE_ERROR    = -5,   // the resource string is too long
    TCPIP_WSC_RES_PROTO_ERROR       = -6,   // too many protocols listed or the protocol string is too long
    TCPIP_WSC_RES_EXTENSION_ERROR   = -7,   // too many extensions listed or the extension string is too long
    TCPIP_WSC_RES_SKT_UNAVAIL       = -8,   // could not open a socket
    TCPIP_WSC_RES_SKT_SIG_ERROR     = -9,   // error when trying to register a signal with the socket
    TCPIP_WSC_RES_SKT_TX_BUFF_ERROR = -10,   // could not set the TX buffer size for the socket
    TCPIP_WSC_RES_SKT_RX_BUFF_ERROR = -11,   // could not set the TX buffer size for the socket
    TCPIP_WSC_RES_SKT_WR_ERROR      = -12,  // error when trying to write to the socket
    TCPIP_WSC_RES_SKT_BIND_ERROR    = -13,  // binding to the remote server failed
    TCPIP_WSC_RES_SKT_CONNECT_ERROR = -14,  // connect to the remote server failed
    TCPIP_WSC_RES_SKT_CONNECT_TMO   = -15,  // connect to the remote server timed out
    TCPIP_WSC_RES_PARSE_ERROR       = -16,  // error when parsing the server reply
                                            // line buffer probably too small 
    TCPIP_WSC_RES_VALIDATION_ERROR  = -17,  // error when validating the server reply
                                            // some wrong/missing headers in the server's reply 
    TCPIP_WSC_RES_DNS_ERROR         = -18,   // error when trying to solve the server name
    TCPIP_WSC_RES_KEY_ERROR         = -19,   // error when trying to generate the random key
    TCPIP_WSC_RES_SRV_TMO           = -20,  // timeout waiting for server reply
    TCPIP_WSC_RES_SRV_DISCONNECT    = -21,  // server severed the connection
    TCPIP_WSC_RES_CLOSING           = -22,  // connection is closing
    TCPIP_WSC_RES_USR_TMO           = -23,  // timeout waiting for user reply

    TCPIP_WSC_RES_BAD_CONN_HANDLE   = -24,  // bad connection handle used
    TCPIP_WSC_RES_BAD_STATE         = -25,  // bad connection state, cannot transfer data
    TCPIP_WSC_RES_EXCESS_DATA       = -26,  // too much data supplied for this type of message
    TCPIP_WSC_RES_CRYPTO_ERROR      = -27,  // a crypto library call failed
    TCPIP_WSC_RES_TLS_ERROR         = -28,  // TLS connection failed
    TCPIP_WSC_RES_TLS_NEG_TMO       = -29,  // TLS negotiation timeout
    TCPIP_WSC_RES_TLS_NEG_ERROR     = -30,  // TLS negotiation error
    TCPIP_WSC_RES_BAD_ARG           = -31,  // bad argument, flag , etc.
    TCPIP_WSC_RES_NO_MSG            = -32,  // no message is pending, cannot get info
    TCPIP_WSC_RES_BAD_MSG_HANDLE    = -33,  // bad message handle used
    TCPIP_WSC_RES_BAD_MSG_ID        = -34,  // bad message ID used
    TCPIP_WSC_RES_MSG_PROTO_ERR     = -35,  // protocol error detected in the incoming message/frame
                                            // message cannot be handled, connection closed
    TCPIP_WSC_RES_MSG_TOO_LARGE     = -36,  // message is too large, cannot be handled


    // other error messages
    TCPIP_WSC_RES_STACK_DOWN        = -78,  // stack going down
    TCPIP_WSC_RES_NOT_IMPLEMENTED   = -79,  // feature currently not supported
                                            // to be added later
    TCPIP_WSC_RES_FATAL_ERROR       = -80,  // a fatal error has occurred, not recoverable!

}TCPIP_WSC_RES;


// *****************************************************************************
/* Enumeration:
    TCPIP_WSC_CONN_STATUS

  Summary:
    Status of a WSC connection.

  Description:
    Possible status values of a WSC connection.

  Remarks:
    None
*/
typedef enum
{
    TCPIP_WSC_CONN_STAT_IDLE     = 0,   // connection inactive
    TCPIP_WSC_CONN_STAT_CONNECTING,     // connecting   
    TCPIP_WSC_CONN_STAT_OPEN,           // connection established
    TCPIP_WSC_CONN_STAT_CLOSING,        // connection closing
    TCPIP_WSC_CONN_STAT_CLOSED,         // connection closed
}TCPIP_WSC_CONN_STATUS;

// *****************************************************************************
/* Enumeration:
    TCPIP_WSC_MODULE_FLAGS

  Summary:
    Flags supported by the WSC module.

  Description:
    Flags for the initialization of the WSC module.

  Remarks:
    16 bit values

    Reserved for future development. Currently not supported
*/
typedef enum
{
    TCPIP_WSC_MODULE_FLAG_NONE              = 0x00,    // no flag set, default

    
}TCPIP_WSC_MODULE_FLAGS;



// *****************************************************************************
/* Structure:
    TCPIP_WSC_MODULE_CONFIG

  Summary:
    WSC configuration data.

  Description:
    WSC module initialization/configuration data.
    Specific WSC parameters

  Remarks
    sktTxBuffSize and sktRxBuffSize should be properly dimensioned.
    There is no intermediary buffering in the WSC module itself.
    Basically these values set the maximum size of frame that can be TX-ed/RX-ed in one WSC call.
*/
typedef struct
{
    uint16_t    nConnections;   // number of simultaneous WSC connections allowed
                                // each corresponds to a TCP socket
    uint16_t    sktTxBuffSize;  // size of TX buffer for the associated socket; leave 0 for default
    uint16_t    sktRxBuffSize;  // size of RX buffer for the associated socket; leave 0 for default
        
    uint16_t    srvTmo;         // server reply timeout, milli-seconds; 16 bit value
                                // If 0, there is no server timeout.
    uint16_t    usrTmo;         // user read/retrieve timeout, milli-seconds; 16 bit value
                                // WSC user is required to perform a read of a pending RX message
                                // within this timeout.
                                // Note: it is not a timeout for reading the entire message
                                //      but a minimum read frequency (when the message spans multiple reads) 
                                // If 0, there is no user timeout. Note: this option should be used carefully!
    uint16_t    configFlags;    // a TCPIP_WSC_MODULE_FLAGS value.
    
}TCPIP_WSC_MODULE_CONFIG;


// *****************************************************************************
/* Enumeration:
    TCPIP_WSC_CLOSE_CODE

  Summary:
    Status code used for a WSC connection close operation.

  Description:
    Pre-defined status codes used for a WSC connection close operation.

  Remarks:
    16 bit values
*/
typedef enum
{
   TCPIP_WSC_CLOSE_NONE             = 0,        // no close code is provided
   // RFC close codes
   TCPIP_WSC_CLOSE_NORMAL           = 1000,     // indicates a normal closure
   TCPIP_WSC_CLOSE_GOING_AWAY       = 1001,     // endpoint is "going away"
   TCPIP_WSC_CLOSE_PROTO_ERR        = 1002,     // endpoint terminates the connection due to a protocol error.
   TCPIP_WSC_CLOSE_DATA_ACCEPT_ERR  = 1003,     // endpoint has received a type of data it cannot accept
   TCPIP_WSC_CLOSE_RESERVED         = 1004,     // Reserved. The specific meaning might be defined in the future.
   TCPIP_WSC_CLOSE_NO_CODE          = 1005,     // Reserved.  MUST NOT be set as a status code in a Close control frame by an endpoint.
                                                // Designated for use in applications to indicate that no status code was actually present.

   TCPIP_WSC_CLOSE_ABNORMAL         = 1006,     // Reserved.  MUST NOT be set as a status code in a Close control frame by an endpoint.
                                                // Designated for use in applications to indicate that the connection was closed abnormally,
                                                // e.g., without sending or receiving a Close control frame.
   TCPIP_WSC_CLOSE_DATA_TYPE_ERR    = 1007,     // endpoint has received data within a message that was not
                                                // consistent with the type of the message
   TCPIP_WSC_CLOSE_POLICY_ERR       = 1008,     // endpoint has received a message that violates its policy
   TCPIP_WSC_CLOSE_DATA_SIZE_ERR    = 1009,     // endpoint has received a message that is too big for it to process.
   TCPIP_WSC_CLOSE_EXTENSION_ERR    = 1010,     // endpoint has expected the server to negotiate one or more extension,
                                                // but the server didn’t return them in the response message of the WebSocket handshake.

   TCPIP_WSC_CLOSE_UNEPECTED_ERR    = 1011,     // server is terminating the connection because it encountered an unexpected condition
                                                // that prevented it from fulfilling the request. 

   TCPIP_WSC_CLOSE_TLS_ERROR        = 1015,     // MUST NOT be set as a status code in a Close control frame by an endpoint. 
                                                // It is designated for use in applications to indicate that the connection was closed
                                                // due to a failure to perform a TLS handshake (e.g., the server certificate can’t be verified).
}TCPIP_WSC_CLOSE_CODE;


// WSC Events
// *****************************************************************************
/* Enumeration:
    TCPIP_WSC_EVENT_TYPE

  Summary:
    Events reported by WSC.

  Description:
    List of events reported by WSC.

  Remarks:
    8 bit values
*/
typedef enum
{
    WSC_EVENT_NONE            = 0,      // no event, invalid
    // raw TCP events
    WSC_EVENT_RAW_ESTABLISHED,          // TCP connection to the server established
    WSC_EVENT_RAW_RX_DATA,              // TCP RX data available
    WSC_EVENT_RAW_RX_FIN,               // server sent a FIN to close the connection
    WSC_EVENT_RAW_RX_RST,               // server sent a RST to reset the connection
    WSC_EVENT_RAW_UNKNOWN,              // unknown raw event has occurred

    // WebSocket events
    WSC_EVENT_OPEN,                     // connection was opened
    WSC_EVENT_MSG_READY,                // message from the server is available
    // control messages
    WSC_EVENT_CTRL_CLOSE,               // connection was closed by the server
    WSC_EVENT_CTRL_PING,                // server sent us a ping
    WSC_EVENT_CTRL_PONG,                // server sent us a pong
    WSC_EVENT_CTRL_UNKNOWN,             // server sent us an unknown control message (extension?)
    // other connection events
    WSC_EVENT_CONN_CLOSE,               // the connection was closed
                                        // the connection handle is no longer valid


    // errors
    WSC_EVENT_MSG_ERROR,                // a message/frame was discarded because of ill formatting 
                                        // or processing is not supported
    WSC_EVENT_MSG_DISCARD_USR_TMO,      // a pending message/frame was discarded because of user timeout to read it
    WSC_EVENT_MSG_DISCARD_CLOSE,        // a pending message/frame was discarded because the connection is closing
    WSC_EVENT_FAIL_CONN,                // a connection has failed due to some error
                                        // the connection handle is no longer valid
    WSC_EVENT_ERROR,                    // connection was closed by the client, due to error
    // ... more detailed events will be added

    
}TCPIP_WSC_EVENT_TYPE;


// *****************************************************************************
/* Type:
    TCPIP_WSC_CONN_HANDLE

  Summary:
    WSC connection identifier.

  Description:
    Handle of a WSC connection

  Remarks:
    None
*/
typedef const void*     TCPIP_WSC_CONN_HANDLE;

// *****************************************************************************
/* Structure:
    TCPIP_WSC_OPEN_INFO

  Summary:
    WSC open info

  Description:
    WSC info about a opened connection to a server

  Remarks:
    None
*/
typedef struct
{
    IP_MULTI_ADDRESS    srvAddress; // the server IP address the client is connected to
    TCPIP_NET_HANDLE    hNet;       // Network interface for the connection
    uint16_t            srvPort;    // server port
    uint8_t             ipType;     // an IP_ADDRESS_TYPE value: IP_ADDRESS_TYPE_IPV4/IP_ADDRESS_TYPE_IPV6

}TCPIP_WSC_OPEN_INFO;


// *****************************************************************************
/* Structure:
    TCPIP_WSC_MSG_INFO

  Summary:
    WSC message info

  Description:
    WSC info about a WSC message.

  Remarks:
    None
*/
typedef struct
{
    uint8_t                 flags;          // message flags;   TCPIP_WSC_MSG_FLAGS value
    uint8_t                 opCode;         // a TCPIP_WS_OP_CODE value
    uint8_t                 frameType;      // a TCPIP_WS_FRAME_TYPE value
    uint8_t                 frameLen;       // size of the frame header
}TCPIP_WSC_MSG_INFO;


// *****************************************************************************
/* Structure:
    TCPIP_WSC_PEND_MSG_DCPT

  Summary:
    WSC pending message description.

  Description:
    Data structure describing a WSC pending message from the server

  Remarks:
    No large frame support!
*/
typedef struct
{
    const void*             msgHandle;      // pending message handle/identifier;
    TCPIP_WSC_MSG_INFO      info;           // message info
    uint16_t                payloadLen;     // total payload length that's pending
    uint16_t                renderedLen;    // how much has been passed to the user: [0, payloadLen]
    uint16_t                sktPendLen;     // last data available in the socket buffer for this message: [0, min(payload, skt buff size)]
    uint16_t                srvAvlblLen;    // how much data available (been read) from the server: [0, payloadLen]
}TCPIP_WSC_PEND_MSG_DCPT;

// *****************************************************************************
/* Structure:
    TCPIP_WSC_ERR_MSG_DCPT

  Summary:
    Info about an error message

  Description:
    WSC info about an error message

  Remarks:
    None
*/
typedef struct
{
    TCPIP_WSC_MSG_INFO      info;           // message info
    uint64_t                payloadLen;     // total payload length 
}TCPIP_WSC_ERR_MSG_DCPT;


// *****************************************************************************
/* Structure:
    TCPIP_WSC_FAIL_INFO

  Summary:
    Failure event info

  Description:
    WSC info about a failure event

  Remarks:
    None
*/
typedef struct
{
    TCPIP_WSC_RES   failRes;        // failure result; info about the failure reason
}TCPIP_WSC_FAIL_INFO;

// *****************************************************************************
/* Union:
    TCPIP_WSC_EV_INFO

  Summary:
    WSC event info union

  Description:
    Union describing the possible info carried by a WSC event.

  Remarks:
    Other members will be added for new events
*/
typedef union
{
    const void*                     evPtr;          // for generic events
    const char*                     evCtrlMsg;      // used for WSC_EVENT_CTRL_CLOSE/WSC_EVENT_CTRL_PING/WSC_EVENT_CTRL_PONG/WSC_EVENT_CTRL_UNKNOWN 
    const uint8_t*                  evCtrlBuff;     // used for control events; same as evCtrlMsg but as a byte buffer, if needed.
    const TCPIP_WSC_OPEN_INFO*      openInfo;       // used for WSC_EVENT_OPEN events
    const TCPIP_WSC_PEND_MSG_DCPT*  pendMsgDcpt;    // used for WSC_EVENT_MSG_READY events
    const TCPIP_WSC_ERR_MSG_DCPT*   errMsgDcpt;     // used for WSC_EVENT_MSG_ERROR, WSC_EVENT_MSG_DISCARD_USR_TMO and WSC_EVENT_MSG_DISCARD_CLOSE events
    const TCPIP_WSC_FAIL_INFO*      failInfo;       // used for WSC_EVENT_FAIL_CONN events

}TCPIP_WSC_EV_INFO;


// *****************************************************************************
/* Type:
    TCPIP_WSC_EVENT_HANDLER

  Summary:
    Notification handler that will be called when connection condition will occur.

  Description:
    The format of a notification handler registered with the WSC module.

  Parameters:
    hConn       -  A connection handle, obtained by a call to TCPIP_WSC_ConnOpen 
                    Use hConn == NULL to register on all available connections.
    evType      - the event that has occurred
    evInfo      - additional details about the event:
                    WSC_EVENT_RAW_ESTABLISHED       - NULL
                    WSC_EVENT_RAW_RX_DATA           - NULL
                    WSC_EVENT_RAW_RX_FIN            - NULL
                    WSC_EVENT_RAW_RX_RST            - NULL
                    WSC_EVENT_RAW_UNKNOWN           - NULL

                    WSC_EVENT_OPEN -        pointer to  a constant TCPIP_WSC_OPEN_INFO data structure
                    WSC_EVENT_MSG_READY -   a pointer to a constant TCPIP_WSC_PEND_MSG_DCPT data structure
                    WSC_EVENT_CTRL_CLOSE -  pointer to a constant string, reason for the server closing the connection
                    WSC_EVENT_CTRL_PING  -  pointer to a constant string, the ping message
                    WSC_EVENT_CTRL_PONG  -  pointer to a constant string, the pong message
                    WSC_EVENT_CTRL_UNKNOWN  -  pointer to a constant string, the control message

                    - other connection events
                    WSC_EVENT_CONN_CLOSE    - NULL

                    - errors
                    WSC_EVENT_MSG_ERROR - a pointer to a constant TCPIP_WSC_ERR_MSG_DCPT data structure
                    WSC_EVENT_MSG_DISCARD_USR_TMO - a pointer to a constant TCPIP_WSC_ERR_MSG_DCPT data structure
                    WSC_EVENT_MSG_DISCARD_CLOSE - a pointer to a constant TCPIP_WSC_ERR_MSG_DCPT data structure
                    WSC_EVENT_FAIL_CONN - a pointer to a constant TCPIP_WSC_FAIL_INFO data structure


                    WSC_EVENT_ERROR -       NULL; TBD
                    

    hParam      - The hParam is passed by the client and will be used by the 
                  WSC when the notification is made.


  Remarks:
    The handler is called when an event of some sort occurs for a particular WSC connection.
    It has to be short and quick, without lengthy processing.

    Important:
        When the event passes evInfo, this is a constant structure that is not persistent 
        (probably a variable in the caller stack).
        Once the event handler function returns, the evInfo dat ais no longer valid.
        Make a copy of the evInfo, if needed!

*/
typedef void    (*TCPIP_WSC_EVENT_HANDLER)(TCPIP_WSC_CONN_HANDLE hConn, TCPIP_WSC_EVENT_TYPE evType, TCPIP_WSC_EV_INFO evInfo, const void* param);

// *****************************************************************************
/* Type:
    TCPIP_WSC_EV_HANDLE

  Summary:
    WSC event handle.

  Description:
    A handle that a client needs to use when  deregistering a notification handler. 

  Remarks:
    This handle can be used by the client after the event handler has been registered.
*/
typedef const void* TCPIP_WSC_EV_HANDLE;


// *****************************************************************************
// *****************************************************************************
// Section: WSC Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Enumeration:
    TCPIP_WSC_CONN_FLAGS

  Summary:
    WSC connection open flags.

  Description:
    Additional connection flags used to open a WSC connection.

  Remarks:
    16 bit values
*/
typedef enum
{
    TCPIP_WSC_CONN_FLAG_NONE                = 0x00,     // default value, no flag set
    TCPIP_WSC_CONN_FLAG_NO_DELAY            = 0x01,     // Create the WSC sockets with NO-DELAY option.
                                                                // It will flush data as soon as possible.
    TCPIP_WSC_CONN_FLAG_SECURE_ON           = 0x02,     // All WSC connections have to be secure
                                                                // (supposing the network presentation layer supports encryption)
                                                                // Cannot be used together with TCPIP_WSC_CONN_FLAG_SECURE_OFF

    TCPIP_WSC_CONN_FLAG_SECURE_OFF          = 0x04,     // WSC connections will be non-secure
                                                                // Cannot be used together with TCPIP_WSC_CONN_FLAG_SECURE

    TCPIP_WSC_CONN_FLAG_SECURE_DEFAULT      = 0x00,     //  WSC security is based on the port numbers: default

    TCPIP_WSC_CONN_FLAG_USE_IPV6            = 0x08,     // connect using IPv6
    TCPIP_WSC_CONN_FLAG_USE_IPV4            = 0x00,     // connect using IPv4: default

    TCPIP_WSC_CONN_FLAG_PROTO_OPTIONAL      = 0x10,     // accept a connection if the server does not indicate support
                                                        // for the client requested sub-protocols.
                                                        // By default, if protocols were requested and server doesn't select one
                                                        // the connection will fail.
    TCPIP_WSC_CONN_FLAG_PROTO_ENFORCED      = 0x00,     // the server should select a protocol from our client requested one
                                                        // This is the default

    // 
    TCPIP_WSC_CONN_FLAG_SKIP_VERSION        = 0x80,     // skip the client version header when connecting to the server
                                                        // apparently some servers do not like this (?)

}TCPIP_WSC_CONN_FLAGS;


// *****************************************************************************
/* Enumeration:
    TCPIP_WSC_MSG_FLAGS

  Summary:
    WSC message flags.

  Description:
    Flags used to send a WSC message.

  Remarks:
    TCPIP_WSC_MSG_FLAG_FRAG_MID cannot be used with TCPIP_WSC_MSG_FLAG_FRAG_START or TCPIP_WSC_MSG_FLAG_FRAG_END
        If both the TCPIP_WSC_MSG_FLAG_FRAG_START and TCPIP_WSC_MSG_FLAG_FRAG_END are set,
        then it is a regular 1 fragment message equivalent to having TCPIP_WSC_MSG_FLAG_FRAG_NONE 

    TCPIP_WSC_MSG_FLAG_CTRL denotes a control message and it takes precedence.
        All other flags are irrelevant!

    8 bit values
*/
typedef enum
{
    TCPIP_WSC_MSG_FLAG_NONE                = 0x00,      // default value, no flag set
    TCPIP_WSC_MSG_FLAG_BINARY              = 0x01,      // the message is binary
    TCPIP_WSC_MSG_FLAG_TEXT                = 0x00,      // the message is text, default
    TCPIP_WSC_MSG_FLAG_FRAG_START          = 0x02,      // the message is fragmented, this is the 1st fragment
    TCPIP_WSC_MSG_FLAG_FRAG_END            = 0x04,      // the message is fragmented, this is the last fragment
    TCPIP_WSC_MSG_FLAG_FRAG_MID            = 0x08,      // the message is fragmented, this is a intermediary/middle continuation fragment
    TCPIP_WSC_MSG_FLAG_FRAG_NONE           = 0x00,      // the message is not fragmented, default 
    TCPIP_WSC_MSG_FLAG_CTRL                = 0x10,      // a control message, the ctrlCode field is relevant 
    // RX specific messages
    TCPIP_WSC_MSG_FLAG_RX_UNKNOWN          = 0x20,      // an unknown operation code was received
                                                        // could be an extension?
    // TX specific messages
    TCPIP_WSC_MSG_FLAG_TX_WHOLE            = 0x40,      // the message needs to be transmitted in one single send operation
                                                        // Used internally for control messages
                                                        // Use this carefully, if the socket TX buffer is small
                                                        // or the server is slow in processing the client messages
}TCPIP_WSC_MSG_FLAGS;

// *****************************************************************************
/* Enumeration:
    TCPIP_WS_FRAME_TYPE

  Summary:
    Types of WS data frames

  Description:
    The possible types of WS data frames

  Remarks:
    8 bit values
*/
typedef enum
{
    TCPIP_WS_FRAME_TYPE_NONE = 0,   // none/invalid frame type
    TCPIP_WS_FRAME_TYPE_CTRL,       // control frame
    TCPIP_WS_FRAME_TYPE_SMALL,      // small data frame, size < 125 bytes
    TCPIP_WS_FRAME_TYPE_MEDIUM,     // medium data frame, size [126, 64K]
    TCPIP_WS_FRAME_TYPE_LARGE,      // large data frame, size > 64K
}TCPIP_WS_FRAME_TYPE;


// *****************************************************************************
/* Enumeration:
    TCPIP_WS_OP_CODE

  Summary:
    WS message/frame operation/control codes

  Description:
    Control codes for a WSC message.

  Remarks:
    RFC defines:
        operation codes as values [0, 0x07]
        control codes as values [8, 0x0F]

    For sending control messages use the discrete messages:
        TCPIP_WSC_ConnPing, TCPIP_WSC_ConnPong, TCPIP_WSC_ConnClose 

    8 bit values
*/
typedef enum
{
    // non-control frames
    TCPIP_WS_OPCODE_CONT_FRAME      = 0,    // 0 denotes a continuation frame
    TCPIP_WS_OPCODE_TEXT_FRAME      = 1,    // 1 denotes a text frame
    TCPIP_WS_OPCODE_BINARY_FRAME    = 2,    // 2 denotes a binary frame
    // 3 - 7 are reserved for further non-control frames
    // and should NOT be used!
    TCPIP_WS_CTRL_CODE_CLOSE        = 8,    // 8 denotes a connection close
    TCPIP_WS_CTRL_CODE_PING         = 9,    // 9 denotes a ping
    TCPIP_WS_CTRL_CODE_PONG         = 10,   // 0xA denotes a pong
    // values [0xB-0xF] are reserved for further control frames
    // and should NOT be used!
}TCPIP_WS_OP_CODE;


// *****************************************************************************
/* Structure:
    TCPIP_WSC_CONN_DCPT

  Summary:
    WSC connection description

  Description:
    WSC data structure describing the client connection to a WS server

  Remarks:
    None

*/
typedef struct
{
    const char*     server;         // WS server to connect to 
    const char*     resource;       // resource to be requested; It should not start with a '/' as this is pre-pended automatically
    uint16_t        port;           // connection port to use
    uint16_t        connFlags;      // additional connection flags: TCPIP_WSC_CONN_FLAGS value
    const char**    protocols;      // application-level protocols layered over the WS protocol ordered by preference
                                    // could be NULL
    const char**    extensions;     // the protocol-level extension(s) the client wishes to speak
                                    // currently not used and should be NULL
    uint16_t        nProtocols;     // number of listed protocols 
    uint16_t        nExtensions;    // number of listed extensions
                                    // currently not used and should be 0
}TCPIP_WSC_CONN_DCPT;


// *****************************************************************************
/* Function:
    TCPIP_WSC_CONN_HANDLE TCPIP_WSC_ConnOpen(const TCPIP_WSC_CONN_DCPT* connDcpt, TCPIP_WSC_RES* pRes);

  Summary:
    Opens a WSC connection.

  Description:
    This function will initiate a connection to the supplied server.
    It will do the handshake and set the connection for further data communication.

  Precondition:
    The WSC module should have been initialized.

  Parameters:
    connDcpt    - a connection descriptor indicating:
            server      - WS server to connect to 
            resource    - resource to be requested
            port        - connection port to use 
            connFlags   - additional flags: TCPIP_WSC_CONN_FLAGS value
            protocols   - application-level protocols
            extensions  - the protocol-level extension(s)

    pRes        - optional address to store the operation result.
                  NULL if not needed.

  Returns:
    TCPIP_WSC_CONN_HANDLE 
    - On Success - Returns a valid, NON NULL,  handle
    - On Failure - NULL handle

  Remarks:
    Multi-threaded use:
      - A WSC handle cannot be used concurrently from multiple threads!
        It's ok to pass a handle from one thread to another as long as
        there's is no access from more than one thread at a time!

*/
TCPIP_WSC_CONN_HANDLE TCPIP_WSC_ConnOpen(const TCPIP_WSC_CONN_DCPT* connDcpt, TCPIP_WSC_RES* pRes);

// *****************************************************************************
/* Function:
    TCPIP_WSC_CONN_STATUS   TCPIP_WSC_ConnStatus(TCPIP_WSC_CONN_HANDLE hConn);

  Summary:
    Returns the connection state 

  Description:
    This function will return the current state of the connection

  Precondition:
    The WSC connection should have opened.

  Parameters:
    hConn  - a connection handle obtained by the TCPIP_WSC_ConnOpen call

  Returns:
    - On Success - Returns a TCPIP_WSC_CONN_STATUS value
    - On Failure - TCPIP_WSC_CONN_STAT_IDLE if the connection handle is invalid

  Remarks:
    None
*/
TCPIP_WSC_CONN_STATUS   TCPIP_WSC_ConnStatus(TCPIP_WSC_CONN_HANDLE hConn);

// *****************************************************************************
/* Function:
    const char*   TCPIP_WSC_ConnProtoInUse(TCPIP_WSC_CONN_HANDLE hConn);

  Summary:
    Returns the current in use sub-protocol for the connection

  Description:
    This function will return the current sub-protocol that the server has accepted
    for this connection.

  Precondition:
    The WSC connection should have opened.

  Parameters:
    hConn  - a connection handle obtained by the TCPIP_WSC_ConnOpen call

  Returns:
    - valid pointer to a string containing the sub-protocol
    - NULL if there is no sub-protocol in use

  Remarks:
    Even if the connection was opened requesting a protocol
    the server may chose to ignore it.
    However, the connection should have been opened with the TCPIP_WSC_CONN_FLAG_PROTO_OPTIONAL flag set,
    otherwise the connection would have failed. 
*/
const char*     TCPIP_WSC_ConnProtoInUse(TCPIP_WSC_CONN_HANDLE hConn);

// *****************************************************************************
/* Function:
    size_t TCPIP_WSC_ConnPing(TCPIP_WSC_CONN_HANDLE hConn, const uint8_t* pingData, size_t dataSize, TCPIP_WSC_RES* pRes);

  Summary:
    Pings the server with some user data

  Description:
    This function will ping a server with the supplied data.

  Precondition:
    The WSC connection should have opened.

  Parameters:
    hConn  - a connection handle obtained by the TCPIP_WSC_ConnOpen call
    pingData    - data to be used
                  Can be NULL
    dataSize    - size of the data to be used
    pRes        - optional address to store the operation result.
                  NULL if not needed.

  Returns:
    - the number of bytes successfuly written to the connection output
    - pRes is updated with:
            - TCPIP_WSC_RES_OK on success
            - TCPIP_WSC_RES_BUSY if retry needed
                Note: this may occur only under an RTOS operation and it is not currently supported !
            - a TCPIP_WSC_RES error result on Failure


  Remarks:
    pingData is optional

    Data transfers are allowed only when in TCPIP_WSC_CONN_STAT_OPEN state! 
*/
size_t TCPIP_WSC_ConnPing(TCPIP_WSC_CONN_HANDLE hConn, const uint8_t* pingData, size_t dataSize, TCPIP_WSC_RES* pRes);

// *****************************************************************************
/* Function:
    size_t TCPIP_WSC_ConnPong(TCPIP_WSC_CONN_HANDLE hConn, const uint8_t* pongData, size_t dataSize, TCPIP_WSC_RES* pRes);

  Summary:
    Helper to send a pong message to the server with some user data

  Description:
    This function will pong a server with the supplied data.


  Precondition:
    The WSC connection should have opened.

  Parameters:
    hConn  - a connection handle obtained by the TCPIP_WSC_ConnOpen call
    pongData    - data to be used
                  Can be NULL
    dataSize    - size of the data to be used
    pRes        - optional address to store the operation result.
                  NULL if not needed.

  Returns:
    - the number of bytes successfuly written to the connection output
    - pRes is updated with:
            - TCPIP_WSC_RES_OK on success
            - TCPIP_WSC_RES_BUSY if retry needed
                Note: this may occur only under an RTOS operation and it is not currently supported !
            - a TCPIP_WSC_RES error result on Failure

  Remarks:
    pongData is optional

    Data transfers are allowed only when in TCPIP_WSC_CONN_STAT_OPEN state! 

*/
size_t TCPIP_WSC_ConnPong(TCPIP_WSC_CONN_HANDLE hConn, const uint8_t* pongData, size_t dataSize, TCPIP_WSC_RES* pRes);

// *****************************************************************************
/* Function:
    TCPIP_WSC_RES   TCPIP_WSC_ConnClose(TCPIP_WSC_CONN_HANDLE hConn, TCPIP_WSC_CLOSE_CODE closeCode, const char* reason);

  Summary:
    Closes an opened connection

  Description:
    This function will close a currently open connection.

  Precondition:
    The WSC connection should have opened.

  Parameters:
    hConn       - a connection handle obtained by the TCPIP_WSC_ConnOpen call
    closeCode   - a close code to send to the server
    reason      - some reason for the client closing the connection

  Returns:
    - On Success - Returns  TCPIP_WSC_RES_OK
    - On Failure - a TCPIP_WSC_RES error result

  Remarks:
    Normally the client should not close the connection.
    Server will do that when appropriate.

    If closeCode == TCPIP_WSC_CLOSE_NONE:
        the 'reason' string is irrelevant
        No message will be provided to the server.

    Else the message will consist of the status code followed by the 'reason' string, if !NULL.        

    The closeCode is not checked to be part of the TCPIP_WSC_CLOSE_CODE enumeration.
    However, proper values should be used, according to the RFC. 

*/
TCPIP_WSC_RES   TCPIP_WSC_ConnClose(TCPIP_WSC_CONN_HANDLE hConn, TCPIP_WSC_CLOSE_CODE closeCode, const char* reason);

// *****************************************************************************
/* Structure:
    TCPIP_WSC_SEND_MSG_DCPT

  Summary:
    WSC sending data structure.

  Description:
    WSC structure of the message used for write - sending data

  Remarks:
    None
*/
typedef struct
{
    const uint8_t*  msgData;    // pointer to binary message buffer
    size_t          msgSize;    // total size of the message
                                // No support for large frames 
    uint8_t         msgFlags;   // message flags;   TCPIP_WSC_MSG_FLAGS value
    uint8_t         ctrlCode;   // control code - a TCPIP_WS_OP_CODE value
                                // only when TCPIP_WSC_MSG_FLAG_CTRL is set    
                                // debug/test/extensions support
    uint16_t        msgId;      // when the message needs multiple TCPIP_WSC_MessageSend() calls
                                // this identifies the message.
                                // Needs to be 0 on the first call
                                // and the TCPIP_WSC_MessageSend() will fill with the proper ID
                                // Subsequent calls need to use the returned msgId!
}TCPIP_WSC_SEND_MSG_DCPT;


// *****************************************************************************
/* Function:
    size_t TCPIP_WSC_MessageSend(TCPIP_WSC_CONN_HANDLE hConn, TCPIP_WSC_SEND_MSG_DCPT* msgDcpt, TCPIP_WSC_RES* pRes)

  Summary:
    Sends a message to the server

  Description:
    This function will send a message to the server

  Precondition:
    The WSC connection should have opened.

  Parameters:
    hConn  - a connection handle obtained by the TCPIP_WSC_ConnOpen call
    msgDcpt     - a descriptor for the message to be sent
    pRes        - optional address to store the operation result.
                  NULL if not needed.
                  However, the operation result is detailed in the pRes and it should be used.

  Returns:
    - the number of bytes successfuly written to the output
    - pRes is updated with:
            - TCPIP_WSC_RES_OK on success - message completed
            - TCPIP_WSC_RES_BUSY if retry needed
                Note: this may occur only under an RTOS operation and it is not currently supported !
            - TCPIP_WSC_RES_MORE_DATA - some data was written but more calls are needed
                This is because there is no space available to send the whole message 
            - TCPIP_WSC_RES_NO_WHOLE_SPACE - TCPIP_WSC_MSG_FLAG_TX_WHOLE was set and there is not enough space
                in the socket buffer to transmit all at once. Retry.
            - a TCPIP_WSC_RES error result on Failure

  Remarks:
    This function will send a message (possible including fragments);

    Data transfers are allowed only when in TCPIP_WSC_CONN_STAT_OPEN state! 

    For control messages use the Ping/Pong command directly.

    TCPIP_WSC_MSG_FLAG_TX_WHOLE msgflag behavior:
        If TCPIP_WSC_MSG_FLAG_TX_WHOLE flag is set:
          then the whole message needs to be sent at once in one single send operation
          Use this for small/control messages, that should be smaller than the size of the socket TX buffer
          Use this carefully, if the socket TX buffer is small
          or the server is slow in processing the client messages

        If TCPIP_WSC_MSG_FLAG_TX_WHOLE is not set:
          This is a message that can span multiple send operations if not enough space in the TX buffer
          This is the default case/behavior.

    IMPORTANT:          
    It may happen that only part of the message could be sent and operation should be retried.
    In this case the returned value is less than the msgSize
    When there's need for multiple send operations to send the message:
        the WS client will store internally the amount of data sent for the pending message!
        As long as the message is not fully transmitted, all the calls to TCPIP_WSC_MessageSend
        will be considered to be part of the pending message.


        The msgId will identify the message when multiple TCPIP_WSC_MessageSend() calls are needed.
        Needs to be 0 on the first call to show new message is sent!
        The TCPIP_WSC_MessageSend() will fill with the proper message ID.
        Subsequent calls need to use the returned msgId!

        The pRes will signal that the message was fully transmitted by using TCPIP_WSC_RES_OK .
        pRes will contain TCPIP_WSC_RES_MORE_DATA if more calls are needed.

        For example:
        TCPIP_WSC_SEND_MSG_DCPT msgDcpt;
        msgDcpt.msgData = myBuffer;
        msgDcpt.msgSize = 2000;
        ....
        msgDcpt.msgId = 0;
        TCPIP_WSC_RES res; 
        size_t sentBytes = 0;
        while(true)
        {
            sentBytes += TCPIP_WSC_MessageSend(hConn, &msgDcpt, &res);
            if(res == TCPIP_WSC_RES_OK)
            {   - the whole message was transmitted and we're done;
                - can check that sentBytes == msgDcpt.msgSize
               break;
            }
            else if(res == TCPIP_WSC_RES_MORE_DATA)
            {   - continue sending the data
                sleep();
                continue;
            }
            else
            {   - res < 0 - some error has occurred
                break;
            }
        }
       
    There is currently no way to abort sending a pending message.        

*/
size_t TCPIP_WSC_MessageSend(TCPIP_WSC_CONN_HANDLE hConn, TCPIP_WSC_SEND_MSG_DCPT* msgDcpt, TCPIP_WSC_RES* pRes);

// *****************************************************************************
/* Function:
    size_t TCPIP_WSC_MessageRead(TCPIP_WSC_CONN_HANDLE hConn, const void* msgHandle, uint8_t* pBuffer, size_t bufferSize, TCPIP_WSC_RES* pRes);

  Summary:
    Reads a pending message from the server

  Description:
    This function will read a server message, if available.

  Precondition:
    The WSC connection should have opened.

  Parameters:
    hConn       - a connection handle obtained by the TCPIP_WSC_ConnOpen call
    msgHandle   - message handle obtained through:
                    - a TCPIP_WSC_PEND_MSG_DCPT data of an WSC_EVENT_MSG_READY event notification
                    - a call to TCPIP_WSC_MessageInfo
                    - NULL
    pBuffer     - buffer to store the data
    bufferSize  - buffer size
    pRes        - optional address to store the operation result.
                  NULL if not needed.

  Returns:
    - the number of the read bytes
      Could be less than bufferSize


  Remarks:
    This function will extract data from a pending message if one is available.

    pRes will store additional details
        - TCPIP_WSC_RES_MORE_DATA - the read was successful but there's more data: call needs to be retried
        - TCPIP_WSC_RES_OK - the read was successful and the message has been completed
        - a TCPIP_WSC_RES < 0, if an error occurred

        For example:
        uint8_t myBuffer[1024];
        TCPIP_WSC_RES res; 
        size_t rxBytes = 0;
        const void* msgHandle = NULL;
        while(true)
        {
            rxBytes += TCPIP_WSC_MessageRead(hConn, msgHandle, myBuffer, sizeof(myBuffer), &res);

            if(res == TCPIP_WSC_RES_OK)
            {   - the whole message was received and we're done;
                - can check that rxBytes == sizeof(myBuffer);
               break;
            }
            else if (res == TCPIP_WSC_RES_MORE_DATA)
            {
                - continue receiving the data
                sleep();
                continue;
            }
            else
            {   - res < 0 - some error has occurred
                break;
            }
        }
       

    If msgHandle == NULL the function will read data from the current pending message, if available;

    Once the whole message has been read (and TCPIP_WSC_MessageRead returns TCPIP_WSC_RES_OK)
    the message handle is no longer valid!

    If pBuffer == NULL or bufferSize == 0, the data will simply be discarded.

*/
size_t TCPIP_WSC_MessageRead(TCPIP_WSC_CONN_HANDLE hConn, const void* msgHandle, uint8_t* pBuffer, size_t bufferSize, TCPIP_WSC_RES* pRes);

// *****************************************************************************
/* Function:
    TCPIP_WSC_RES TCPIP_WSC_MessageInfo(TCPIP_WSC_CONN_HANDLE hConn, const void* msgHandle, TCPIP_WSC_PEND_MSG_DCPT* msgDcpt);

  Summary:
    Reads info about a pending message from the server

  Description:
    This function will return details about a pending server message, if available.

  Precondition:
    The WSC connection should have opened.

  Parameters:
    hConn       - a connection handle obtained by the TCPIP_WSC_ConnOpen call
    msgHandle   - message handle obtained through a TCPIP_WSC_PEND_MSG_DCPT data of an WSC_EVENT_MSG_READY event notification
                  or NULL
    msgDcpt     - address of a descriptor to get the message details

  Returns:
    - On Success - Returns  TCPIP_WSC_RES_OK
    - On Failure - a TCPIP_WSC_RES error result

  Remarks:
    If msgHandle == NULL, the function will return the current pending message, if any

*/
TCPIP_WSC_RES TCPIP_WSC_MessageInfo(TCPIP_WSC_CONN_HANDLE hConn, const void* msgHandle, TCPIP_WSC_PEND_MSG_DCPT* msgDcpt);


// *****************************************************************************
/* Function:
    TCPIP_WSC_EV_HANDLE TCPIP_WSC_HandlerRegister(TCPIP_WSC_CONN_HANDLE hConn, TCPIP_WSC_EVENT_HANDLER handler, const void* hParam);

  Summary:
    Register an WSC resolve handler.

  Description:
    This function will register a notification handler with the WSC module.

  Precondition:
    The WSC module should have been initialized.

  Parameters:
    hConn       -  A connection handle, obtained by a call to TCPIP_WSC_ConnOpen 
                   Use hConn == NULL to register on all available connections.
    handler     - Handler to be called for event.
    hParam      - The hParam is passed by the client and will be used by the 
                  WSC when the notification is made.

  Returns:
    TCPIP_WSC_EV_HANDLE 
    - On Success - Returns a valid handle
    - On Failure - Null handle

  Remarks:
    Two event handlers are maintained: a global one and a per connection one.
    The per connection one takes precedence.

    If hConn == 0, the call will register the global handler.
    
    Currently only one global event handler or one handler per connection could be registered.
    To register a new handler, a previous one should be de-registered.
*/
TCPIP_WSC_EV_HANDLE TCPIP_WSC_HandlerRegister(TCPIP_WSC_CONN_HANDLE hConn, TCPIP_WSC_EVENT_HANDLER handler, const void* hParam);

// *****************************************************************************
/* Function:
    bool TCPIP_WSC_HandlerDeRegister(TCPIP_WSC_CONN_HANDLE hConn, TCPIP_WSC_EV_HANDLE hEvent)

   Summary:
    Deregisters the event handler

   Description:
    Deregisters a previously registered WSC handler

   Precondition:
    The WSC module should have been initialized.

   Parameters:
    hConn   -  A connection handle, obtained by a call to TCPIP_WSC_ConnOpen 
                   Use hConn == NULL to de-register on all available connections.
    hEvent  -   An WSC event handle obtained by a call to TCPIP_WSC_HandlerRegister 

   Returns:
    Boolean
    - On Success - true
    - On Failure - false (If no such handler registered)

  Remarks:
    Currently only one global event handler or one handler per connection could be registered.
    To register a new handler, a previous one should be de-registered.
*/
bool   TCPIP_WSC_HandlerDeRegister(TCPIP_WSC_CONN_HANDLE hConn, TCPIP_WSC_EV_HANDLE hEvent);


// *****************************************************************************
/* Function:
    int16_t  TCPIP_WSC_ConnSocketGet(TCPIP_WSC_CONN_HANDLE hConn);

  Summary:
    Returns the connection transport socket

  Description:
    The function returns the network transport socket of the specified WSC connection.

  Precondition:
    The WSC module should have been initialized.

  Parameters:
    hConn   -  A connection handle, obtained by a call to TCPIP_WSC_ConnOpen 

  Returns:
    >= 0 if the connection is opened and has a valid socket
    < 0 otherwise

  Remarks:
    The user gets access to the connection socket which can be used for debugging purposes.
*/
int16_t  TCPIP_WSC_ConnSocketGet(TCPIP_WSC_CONN_HANDLE hConn);

// *****************************************************************************
/* Function:
    void  TCPIP_WSC_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    Performs WSC module tasks in the TCP/IP stack.

  Precondition:
    The WSC module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_WSC_Task(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // H_WSC_H_
