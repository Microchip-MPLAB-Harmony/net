/*******************************************************************************
  WSC module private header

  Company:
    Microchip Technology Inc.
    
  File Name:
    wsc_private.h

  Summary:
    Header definitions file for WSC module
    
  Description:
    This file contains the private definitions for the WSC module
*******************************************************************************/
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








#ifndef H_WSC_PRIVATE_H_ 
#define H_WSC_PRIVATE_H_ 



// definitions
// 

#if defined(TCPIP_WSC_LARGE_FRAME_SUPPORT) && (TCPIP_WSC_LARGE_FRAME_SUPPORT != false)
#error "WSC large frame support not currently implemented!"
#endif

#define M_WSC_LARGE_FRAME_SUPPORT   0

// fragmented messages: currently not supported
#define M_WSC_FRAGMENT_SUPPORT   0


// type of m-thread synchronization we use
// by default, using a semaphore
// else, it's a critical section but BUSY may be returned to the API calls!
#define M_WSC_SEM_LOCK      1

#if defined(TCPIP_WSC_CONNECTION_SEM) && (TCPIP_WSC_CONNECTION_SEM != 0)
#define M_WSC_SEM_PER_CONNECTION    1
#else
#define M_WSC_SEM_PER_CONNECTION    0
#endif


// discard a pending RX message when user timout 
// only if there's another RX message pending
#define M_WSC_DISCARD_USR_TMO_ON_RX_PENDING   0

// debug
#define WSC_DEBUG_MASK_BASIC            0x0001
#define WSC_DEBUG_MASK_SHOW_STAT        0x0002      // show current status change
#define WSC_DEBUG_MASK_FAIL_CONN        0x0004      // show failed connections
#define WSC_DEBUG_MASK_FAIL_WRITE_HDR   0x0008      // show failed frame header writes

#define WSC_DEBUG_MASK_SHOW_RX_HSHAKE   0x0010      // show server RX-ed handshake messages
#define WSC_DEBUG_MASK_SHOW_RX_MSG      0x0020      // show server RX-ed messages
#define WSC_DEBUG_MASK_SHOW_RX_LOAD     0x0040      // show server RX-ed messages payload too...
#define WSC_DEBUG_MASK_SHOW_RX_DISCARD  0x0080      // show the RX discarded frames
#define WSC_DEBUG_MASK_SHOW_VALIDATE    0x0100      // show the RX validation messages

#define WSC_DEBUG_MASK_SHOW_TX_HSHAKE   0x0200      // show headers sent to the server
#define WSC_DEBUG_MASK_SHOW_TX_FRAME    0x0400      // show the TX frames types

#define WSC_DEBUG_MASK_SHOW_PROTO_LIST  0x0800      // show the list of assembled protocols

#define WSC_DEBUG_MASK_SRV_UPDATE       0x1000      // debug the server update values
#define WSC_DEBUG_MASK_SRV_TMO          0x2000      // debug the server timeout
#define WSC_DEBUG_MASK_USR_TMO          0x4000      // debug the user timeout

#define WSC_DEBUG_MASK_RTOS             0x8000      // show the used RTOS details

#define WSC_DEBUG_MASK_SKT_SIZE         0x10000     // debug the WSC socket buffer size

#define WSC_DEBUG_MASK_SKT_OPEN         0x20000     // show the WSC socket connection details

#define WSC_DEBUG_MASK_BASIC_AUTH       0x40000     // debug the basic authorization generation

#define WSC_DEBUG_LEVEL                 (0x01)

#define WSC_DBG_RX_BUFFER_SIZE          100U    // debugging, when WSC_DEBUG_MASK_SHOW_RX_MSG is defined
                                                // and peaking at the server RX message

// the signals we want to receive from the underlying socket
// they will translate to WSC events to user
#define WSC_RX_SIGNALS  ((uint16_t)TCPIP_TCP_SIGNAL_ESTABLISHED | (uint16_t)TCPIP_TCP_SIGNAL_RX_DATA | (uint16_t)TCPIP_TCP_SIGNAL_RX_FIN | (uint16_t)TCPIP_TCP_SIGNAL_RX_RST)


// the buffer for key generation
// RFC specifies 16 bytes
#define WSC_KEY_BUFFER_SIZE             16
// corresponding base 64 output;
// size multiple of 3 should give 18 * 4 / 3 = 24 
#define WSC_KEY_BASE64_BUFFER_SIZE      (((((WSC_KEY_BUFFER_SIZE + 2U) / 3U) * 3U) * 4U) / 3U)

// make sure that at least the control messages go through without
// spanning multiple socket read/write operations!
#define WSC_MIN_SOCKET_BUFF_SIZE        512U

// size of the buffer to peek into, for finding end of headers, etc.
// notice that if this buffer is too small the connection will fail
// Note: needs to hold a complete control message!
//      Therefore it cannot be < WS_FRAME_CTRL_MAX_PAYLEN == 125!
#define WSC_PEEK_BUFFER_SIZE        256U

// size of the buffer to store the server authorization challenge request
// for some special servers this may have to be added as a configuration parameter
#define WSC_SRV_CHAL_BUFFER_SIZE        128U

// size of the buffer to process a server response line
#define WSC_LINE_BUFFER_SIZE        256U

// size of the buffer to process a server response cweord
#define WSC_WORD_BUFFER_SIZE        40U


// size of the mask buffer - TX data is masked here
// ALWAYS > size of a control frame + its max payload (6 + 125)!
#define WS_MASK_BUFFER_SIZE         256U


// WS frame structure
typedef union 
{
    uint8_t     b8[2];
    uint16_t    val16;
    struct __attribute__((aligned(2), packed))
    {
        unsigned int    opcode: 4;
        unsigned int    rsv3:   1;
        unsigned int    rsv2:   1;
        unsigned int    rsv1:   1;
        unsigned int    fin:    1;
        unsigned int    payLen: 7;
        unsigned int    mask:   1;
    };
}WS_FRAME_HEADER;

typedef struct __attribute__((aligned(2), packed))
{
    WS_FRAME_HEADER hdr;
    uint8_t         maskKey[4];    // mask is 1. Network order
    //uint8_t       data[1];    // if data is present
}WS_CLIENT_FRAME_CTRL;  // client control frame

typedef WS_CLIENT_FRAME_CTRL WS_CLIENT_FRAME_SMALL; // small data frame, masked
// maximum size a control/small frame could have
#define WS_FRAME_CTRL_MAX_PAYLEN    125U
#define WS_FRAME_SMALL_MAX_PAYLEN   125U


typedef struct __attribute__((aligned(2), packed))
{
    WS_FRAME_HEADER hdr;
    //uint8_t       data[1];    // if data is present
}WS_SERVER_FRAME_CTRL;  // client control frame
typedef WS_SERVER_FRAME_CTRL    WS_SERVER_FRAME_SMALL; // small data frame, not masked

typedef struct __attribute__((aligned(2), packed))
{
    WS_FRAME_HEADER hdr;
    uint16_t        extPayLen;
    uint8_t         maskKey[4];    // mask is 1. Network order
    //uint8_t       data[1];    // if data is present
}WS_CLIENT_FRAME_MEDIUM;    // client medium frame, masked

typedef struct __attribute__((aligned(2), packed))
{
    WS_FRAME_HEADER hdr; // payLen == WS_FRAME_MEDIUM_PAYLEN (126) 
    uint16_t        extPayLen;
    //uint8_t       data[1];    // if data is present
}WS_SERVER_FRAME_MEDIUM;   // server medium frame, not masked
// maximum size a medium frame could have
#define WS_FRAME_MEDIUM_PAYLEN 126U
#define WS_FRAME_MEDIUM_MAX_EXT_PAYLEN  0xffffU


typedef struct __attribute__((aligned(2), packed))
{
    WS_FRAME_HEADER hdr; // payLen == WS_FRAME_LARGE_PAYLEN (127)
    uint16_t        extPayLenHi1;
    uint16_t        extPayLenHi2;
    uint16_t        extPayLenLow1;
    uint16_t        extPayLenLow2;
    uint8_t         maskKey[4];    // mask is 1. Network order
    //uint8_t         data[1];    // if data is present
} WS_CLIENT_FRAME_LARGE;  // large client frame, masked
#define WS_FRAME_LARGE_PAYLEN 127U

typedef struct __attribute__((aligned(2), packed))
{
    WS_FRAME_HEADER hdr; // payLen == WS_FRAME_LARGE_PAYLEN (127)
    uint16_t        extPayLenHi1;
    uint16_t        extPayLenHi2;
    uint16_t        extPayLenLow1;
    uint16_t        extPayLenLow2;
    //uint8_t         data[1];    // if data is present
} WS_SERVER_FRAME_LARGE;    // server large frame, not masked
// maximum size a large frame could have
// not currently supported
#define WS_FRAME_LARGE_MAX_EXT_PAYLEN   0xffffffffUL


typedef union
{
    WS_CLIENT_FRAME_CTRL    ctrlFrame;
    WS_CLIENT_FRAME_SMALL   smallFrame;
    WS_CLIENT_FRAME_MEDIUM  medFrame;
    WS_CLIENT_FRAME_LARGE   largeFrame;
}WS_CLIENT_FRAME;

typedef union
{
    WS_CLIENT_FRAME cliFrame;
    char            cFrame[sizeof(WS_CLIENT_FRAME) + 1U];
    uint8_t         uFrame[sizeof(WS_CLIENT_FRAME)];
}U_WS_CLIENT_FRAME;

typedef union
{
    WS_SERVER_FRAME_CTRL    ctrlFrame;
    WS_SERVER_FRAME_SMALL   smallFrame;
    WS_SERVER_FRAME_MEDIUM  medFrame;
    WS_SERVER_FRAME_LARGE   largeFrame;
}WS_SERVER_FRAME;

typedef union
{
    WS_SERVER_FRAME srvFrame;
    char            cFrame[sizeof(WS_SERVER_FRAME) + 1U];
    uint8_t         uFrame[sizeof(WS_SERVER_FRAME)];
}U_WS_SERVER_FRAME;

// minimum server frame to expect
typedef union
{
    WS_SERVER_FRAME_CTRL    srvFrame;
    char            cFrame[sizeof(WS_SERVER_FRAME_CTRL) + 1U];
    uint8_t         uFrame[sizeof(WS_SERVER_FRAME_CTRL)];
}U_WS_SERVER_MIN_FRAME;

#define WS_OPCODE_START_CTRL TCPIP_WS_CTRL_CODE_CLOSE  // start of the control frames   

// WSC event registration

typedef struct  S_TAG_WSC_LIST_NODE
{
    struct S_TAG_WSC_LIST_NODE* next;       // next node in list
                                            // makes it valid SGL_LIST_NODE node
    const void*                 hParam;     // handler parameter
    TCPIP_NET_HANDLE            hNet;       // interface that's registered for
                                            // 1 if all    
}WSC_LIST_NODE;

// internal connection status
typedef enum
{
    WSC_INT_STAT_IDLE      = 0,         // connection is unused
    WSC_INT_STAT_BUSY,                  // intermediary "in use" status
    WSC_INT_STAT_START_DNS,             // start resolving the server name - DNS 
    WSC_INT_STAT_WAIT_DNS,              // wait for DNS to resolve the server name
    WSC_INT_STAT_START_CONNECT,         // start connect to server
    WSC_INT_STAT_WAIT_CONNECT,          // wait to connect to server
    WSC_INT_STAT_WAIT_TLS_CONNECT,      // wait for TLS to complete
    WSC_INT_STAT_START_HSHAKE,          // start the handshake with the server
    WSC_INT_STAT_WAIT_HSHAKE,           // wait for the server to reply to the handshake
    WSC_INT_STAT_AUTHENTICATE,          // send the authentication credentials to the server
    WSC_INT_STAT_OPEN,                  // handshake finished, good to go
    WSC_INT_STAT_CLOSING,               // user closing the connection
    WSC_INT_STAT_CLOSED,                // closed, cleaning up

}WSC_INT_STATUS;

// a pending TX message
// message that needs to be written sequentially because there is not enough room in the socket TX buffer
typedef struct
{
    uint16_t        msgId;      // current message ID; when == 0, this is the beginning of a message
    uint16_t        toSendBytes;// number of the current bytes to send
    const uint8_t*  msgData;    // pointer to current message buffer (adjusted)
    size_t          msgSize;    // total size of the message
    size_t          sentBytes;  // bytes sent so far
    uint8_t         maskKey[4]; // current mask for the pending TX message 
    uint8_t         msgFlags;   // current message flags    
    uint8_t         ctrlCode;   // current message control code
    uint8_t         maskIx;     // current message mask index, modulo 4
}WSC_PEND_TX_MSG;

#define WSC_VALID_MASK_NONE             0x00U   // not mask set
#define WSC_VALID_MASK_STATUS_OK        0x01U   // status line validation mask
#define WSC_VALID_MASK_UPGRADE_OK       0x02U   // upgrade line validation mask
#define WSC_VALID_MASK_CONNECTION_OK    0x04U   // connection line validation mask
#define WSC_VALID_MASK_ACCEPT_OK        0x08U   // accept line validation mask

#define WSC_VALID_MASK_UNAUTHORIZED     0x10U   // unauthorized status line validation mask
#define WSC_VALID_MASK_AUTHENTICATE     0x20U   // authentication requested

#define WSC_VALID_MASK_PROTO_PRESENT    0x40U   // protocol line present mask
#define WSC_VALID_MASK_PROTO_MATCH      0x80U   // protocol line match mask

#define WSC_VALID_MASK_EXT_PRESENT      0x0100U   // extension line present mask

#define WSC_VALID_MASK_MANDATORY        0x0fU   // mask for headers that need to be always present
#define WSC_VALID_MASK_ALL              0x01ffU // mask for all headers

// Stores extended state data for each connection
typedef struct
{
    struct TAG_TCPIP_WSC_MODULE_DCPT* parent;           // the parrent this connection belongs to
    uint16_t                intState;                   // a WSC_INT_STATUS: internal status
    uint16_t                connState;                  // TCPIP_WSC_CONN_STATUS: Current connection state
    uint16_t                connFlags;                  // connection flags, when Open requested: TCPIP_WSC_CONN_FLAGS value
    uint16_t                connIx;                     // index of this connection
    uint16_t                connPort;                   // connecting port
    NET_PRES_SKT_HANDLE_T   netSocket;                  // connection socket

    TCPIP_WSC_AUTH_HANDLER  authHandler;                // authentication callback that will be used when authentication is needed
    uint16_t                srvCloseCode;               // the server close code
    uint8_t                 addType;                    // a IP_ADDRESS_TYPE value
    uint8_t                 peekCtrlLen;                // length of the control message in the peek buffer
    
    uint16_t                nProtocols;                 // active protocols in the protocols field 
    uint8_t                 isConnected;                // 
    uint8_t                 cliCloseSent;               // the client has sent a close frame; 
    uint8_t                 srvCloseRxed;               // the server has sent a close frame;
#if (M_WSC_SEM_LOCK == 1)
#if (M_WSC_SEM_PER_CONNECTION != 0)
    OSAL_SEM_HANDLE_TYPE    connSem;                    // connection sync semaphore
    uint8_t                 connSemValid;               // the connection semaphore is valid
#endif  // (M_WSC_SEM_PER_CONNECTION != 0)
#else
    uint8_t                 connLock;                   // lock flag per connection
#endif  // (M_WSC_SEM_LOCK == 1)

    NET_PRES_SIGNAL_HANDLE  socketSignal;               // socket signal handler
    TCPIP_WSC_PEND_MSG_DCPT pendRxMsg;                  // the pending RX message characteristics
                                                        // data or control
    TCPIP_WSC_PEND_MSG_DCPT* discardRxMsg;              // if != 0 it points to the message to be discarded
                                                        // discardRxMsg = &pendRxMsg, same pending RX message used
    WSC_PEND_TX_MSG         pendTxMsg;                  // the pending TX message

    TCPIP_WSC_EVENT_HANDLER evHandler;                  // handler to be called for event
    const void*             hParam;                     // handler user parameter

    union
    {
        IP_MULTI_ADDRESS    serverIP;                   // server IP address
        NET_PRES_ADDRESS    presServerIP;
    };

    uint32_t                startSrvWaitMs;             // the ms a server wait operation has started
    uint32_t                startUsrWaitMs;             // the ms an user wait operation has started
    uint32_t                valMask;                    // storage for the validation mask when the server's handshake

    int16_t                 protInUseIx;                // index of the current sub-protocol in use
                                                        // < 0 if none
    uint16_t                base64KeyLen;               // size of the base64 key
    union
    {
        uint8_t             uBase64Key[WSC_KEY_BASE64_BUFFER_SIZE + 1U];
        char                cBase64Key[WSC_KEY_BASE64_BUFFER_SIZE + 1U];
    };                                                   // the generated key, base64 encoded
    union
    {
        char                c_peekBuff[WSC_PEEK_BUFFER_SIZE + 1U];  // peek buffer
        uint8_t             u_peekBuff[WSC_PEEK_BUFFER_SIZE];       // used for handhake but also to store the last control frame data message
                                                                    // always used from the RX thread!
    };
    union
    {
        char                c_srvAuthChal[WSC_SRV_CHAL_BUFFER_SIZE + 1U];  // authorization challenge buffer
        uint8_t             u_srvAuthChal[WSC_SRV_CHAL_BUFFER_SIZE];       // stores the authorization (challenge) requested by the server
                                                                    // needs to be presented to the user as part of authentication 
    };

    char                    srvName[TCPIP_WSC_SERVER_MAX_LEN + 1U];     // space to store the server name
    char                    resource[TCPIP_WSC_RESOURCE_MAX_LEN + 1U];  // space to store the resource/uri

    char                    protocols[TCPIP_WSC_PROTO_MAX_NO][TCPIP_WSC_PROTO_MAX_LEN + 1U]; // space to store the WS protocols

    //char                    extensions[TCPIP_WSC_EXTENSION_MAX_NO][TCPIP_WSC_EXTENSION_MAX_LEN + 1U]; // space to store the WS extensions
        
}TCPIP_WSC_CONN_CTRL;

// action taken for a pending message
typedef enum
{
    WSC_MSG_ACTION_NONE         = 0x00,     // invalid/no action set
    WSC_MSG_ACTION_SRV_TMO      = 0x01,     // srv timeout check required
    WSC_MSG_ACTION_USR_TMO      = 0x02,     // user timeout check required
    WSC_MSG_ACTION_CLOSING      = 0x04,     // discard message if status == closing
    WSC_MSG_ACTION_DONE         = 0x08,     // processing done, no other processing needed
                                            // overrides all other actions
}WSC_MSG_ACTION;



#endif  // H_WSC_PRIVATE_H_ 

