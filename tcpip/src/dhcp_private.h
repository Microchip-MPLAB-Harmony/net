/*******************************************************************************
  DHCP Private file for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    dhcp_private.h

  Summary:
    DHCP Private file for Microchip TCP/IP Stack

  Description:
    This file provides the TCP/IP Stack DHCP Private file API definitions.
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2012-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef H_DHCP_PRIVATE_H_
#define H_DHCP_PRIVATE_H_

// enable DHCP debugging features
#define TCPIP_DHCP_DEBUG_MASK_NOTIFY_EVENTS     0x01    // enable notification events display
#define TCPIP_DHCP_DEBUG_MASK_ADDRESS_EVENTS    0x02    // enable adress service events display
#define TCPIP_DHCP_DEBUG_MASK_FAIL_TMO_EVENT    0x04    // enable fail timeout events display
#define TCPIP_DHCP_DEBUG_MASK_STATUS            0x08    // enable status display
#define TCPIP_DHCP_DEBUG_MASK_RX_RES_ENABLE     0x10    // enable receive results display
#define TCPIP_DHCP_DEBUG_MASK_TX_MSG_ENABLE     0x20    // enable TX messages display


#define TCPIP_DHCP_DEBUG_MASK_FAKE_TMO          0x0100    // enable fake timeout support...
#define TCPIP_DHCP_DEBUG_MASK_LEASE_DECLINE     0x0200    // enable fake lease decline
#define TCPIP_DHCP_DEBUG_MASK_FAIL_ARP          0x0400   // enable ARP failures 
#define TCPIP_DHCP_DEBUG_MASK_FAKE_LEASE_TIME   0x0800   // fake the lease time 
#define TCPIP_DHCP_DEBUG_MASK_FAKE_HOST_NAME    0x1000   // fake the host name 


#define TCPIP_DHCP_DEBUG_MASK                   (0)

#define TCPIP_BOOT_REQUEST                    (1u)  // TCPIP_BOOT_REQUEST DHCP type
#define TCPIP_BOOT_REPLY                      (2u)  // TCPIP_BOOT_REPLY DHCP type
#define TCPIP_BOOT_HW_TYPE                    (1u)  // TCPIP_BOOT_HW_TYPE DHCP type
#define TCPIP_BOOT_LEN_OF_HW_TYPE             (6u)  // TCPIP_BOOT_LEN_OF_HW_TYPE DHCP type

#define TCPIP_DHCP_MESSAGE_TYPE               (53u) // DHCP Message Type constant


// DHCP messages from Client to the server
#define TCPIP_DHCP_DISCOVER_MESSAGE           (1u)  // DCHP Discover Message: Client broadcast to locate available servers
#define TCPIP_DHCP_REQUEST_MESSAGE            (3u)  // DHCP Request message: Client message to servers either
                                                    // (a) requesting offered parameters from one server and implicitly declining offers from all others,
                                                    // (b) confirming correctness of previously allocated address after, e.g., system reboot,
                                                    // (c) extending the lease on a particular network address.
#define TCPIP_DHCP_DECLINE_MESSAGE            (4u)  // DHCP Decline Message: Client to server indicating network address is already in use.
#define TCPIP_DHCP_RELEASE_MESSAGE            (7u)  // DCHP Release message: Client to server relinquishing network address and cancelling remaining lease.
#define TCPIP_DHCP_INFORM_MESSAGE             (8u)  // DCHP INFORM Message: Client to server, asking only for local configuration parameters;
                                                    // client already has externally configured network address.



// DHCP messages from Server to the Client
#define TCPIP_DHCP_OFFER_MESSAGE              (2u)  // DHCP Offer Message: Server to client in response to DHCPDISCOVER with
                                                    // offer of configuration parameters.
#define TCPIP_DHCP_ACK_MESSAGE                (5u)  // DHCP ACK Message: Server to client with configuration parameters, including committed
                                                    // network address.
#define TCPIP_DHCP_NAK_MESSAGE                (6u)  // DHCP NAK Message: Server to client indicating client�s notion of network
                                                    // address is incorrect (e.g., client has moved to new subnet) or client�s lease as expired
// Internal DHCP messages
#define TCPIP_DHCP_UNKNOWN_MESSAGE            (0u)  // Code for unknown DHCP message
#define TCPIP_DHCP_REQUEST_RENEW_MESSAGE      (9u)  // internal message for Request Renew message
#define TCPIP_DHCP_TIMEOUT_MESSAGE            (10u) // internal message to show no message available

// DHCP options, etc.

#define TCPIP_DHCP_SERVER_IDENTIFIER          (54u) // DHCP Server Identifier

#define TCPIP_DHCP_OPTION_ACK_MESSAGE         (53u) // DHCP ACK Message
#define TCPIP_DHCP_PARAM_REQUEST_LIST         (55u) // DHCP_PARAM_REQUEST_LIST Type
#define TCPIP_DHCP_PARAM_REQUEST_IP_ADDRESS   (50u) // DHCP_PARAM_REQUEST_IP_ADDRESS Type
#define TCPIP_DHCP_PARAM_REQUEST_CLIENT_ID    (61u) // DHCP_PARAM_REQUEST Client Type

#define TCPIP_DHCP_SUBNET_MASK                (1u)  // DHCP_SUBNET_MASK Type
#define TCPIP_DHCP_ROUTER                     (3u)  // DHCP_ROUTER Type
#define TCPIP_DHCP_TIME_SERVER                (4u)  // DHCP_TIME_SERVER Type
#define TCPIP_DHCP_DNS                        (6u)  // DHCP_DNS Type
#define TCPIP_DHCP_HOST_NAME                  (12u) // DHCP_HOST_NAME Type
#define TCPIP_DHCP_IP_LEASE_TIME              (51u) // DHCP_IP_LEASE_TIME Type
#define TCPIP_DHCP_RENEW_TIME                 (58u) // DHCP RENEW time (T1)
#define TCPIP_DHCP_REBIND_TIME                (59u) // DHCP REBIND time (T2)
#define TCPIP_DHCP_NTP_SERVER                 (42u) // DHCP_NTP_SERVER Type

#define TCPIP_DHCP_END_OPTION                 (255u)    // DHCP_END_OPTION Type



// Default time out value for all DHCP requests; seconds
// This forms the base of the DHCP transactions timeout 
// and it's used in the expornential backoff
// Using a default value for 100 Mbps networks
#define TCPIP_DHCP_EXP_BACKOFF_BASE   2U

// exponential backoff limit, as set by the standard; seconds
#define TCPIP_DHCP_EXP_BACKOFF_LIMIT    64U

// random variation for exponential backoff timing, seconds
// Standard sets it to 1 second 
#define TCPIP_DHCP_EXP_BACKOFF_FUZZ     1U

// random variation for lease timeout; seconds
#define TCPIP_DHCP_LEASE_EXPIRE_FUZZ    10U

// DHCP RENEWING state retry timeout; seconds
// Standard sets it to 60 seconds
#define TCPIP_DHCP_RENEW_RETRY_TIMEOUT  60U

// DHCP REBINDING state retry timeout; seconds
// Standard sets it to 60 seconds
#define TCPIP_DHCP_REBIND_RETRY_TIMEOUT  60U

// DHCP default lease time if the server doesn't provide one; seconds
// not likely to be used, the server provides one but just in case
#define TCPIP_DHCP_LEASE_TIME_DEFAULT   120U

// Default time out to report the unavailability of a lease
// and switch to a different address service; seconds
// This is used only at the initialization
#define TCPIP_DHCP_INIT_FAIL_TMO        10U


// DHCP UDP socket minimum size for being able to carry
// a DHCP frame
// The RFC states that the maximum message from the server 
// may be 576 for lots of options
// It may need update!
#define TCPIP_DHCP_MIN_UDP_TX_BUFFER_SIZE     512U


// DHCP UDP socket minimum amount of free TX space
// Shouldn't happen because all DHCP transactions
// are followed by a flush operation
#define TCPIP_DHCP_MIN_UDP_TX_AVLBL_SIZE     300U


// DHCP frame minimum packet size
// A samller packet will be padded with 0s
// Has to be < TCPIP_DHCP_MIN_UDP_TX_BUFFER_SIZE!
#define TCPIP_DHCP_MIN_TX_FRAME_SIZE        300U

// DHCP host name illegal character replacement
#define TCPIP_DHCP_HOST_REPLACE_CHAR    (unsigned char)'x'


// DHCP or BOOTP Header structure
typedef union U_TCPIP_DHCP_FRAME_HEADER
{
    uint8_t         v[7 * 4 + 16];  // v[sizeof(union U_TCPIP_DHCP_FRAME_HEADER)];
    struct
    {
        uint8_t     op;             // Message type for this message
        uint8_t     htype;          // Hardware type for this message
        uint8_t     hlen;           // Length of hardware type
        uint8_t     hops;           // Number of hops
        uint32_t    xid;            // DHCP Transaction ID
        uint16_t    secs;           // Number of elapsed seconds
        uint16_t    flags;          // BOOTP Flags
        uint32_t    ciaddr;         // Client IP
        uint32_t    yiaddr;         // Your IP
        uint32_t    siaddr;         // Next Server IP
        uint32_t    giaddr;         // Relay Agent IP
        uint8_t     chaddr[16];     // Client MAC Address
    };
} TCPIP_DHCP_FRAME_HEADER;

typedef struct
{
    uint8_t     sname[64];      // optional server host name
    uint8_t     file[128];      // boot file name
} TCPIP_DHCP_FRAME_OPT_HEADER;

typedef struct
{
    uint8_t     cookie[4];          // DHCP magic cookie
    uint8_t     options[312 - 4];   // max options field 
}TCPIP_DHCP_FRAME_OPTIONS;

// a dispatch operation result
typedef enum
{
    TCPIP_DHCP_OPTION_RES_OK,       // properly processed option or ignored
    TCPIP_DHCP_OPTION_RES_FAIL,     // ill formed option/packet; abort
    TCPIP_DHCP_OPTION_RES_END,      // end of options found; stop
}TCPIP_DHCP_OPTION_RESULT;

typedef struct __attribute__((packed))
{
    uint8_t     opt;    // should be TCPIP_DHCP_MESSAGE_TYPE
    uint8_t     len;    // 1
    uint8_t     type;   // DHCP message type
}TCPIP_DHCP_OPTION_DATA_MSG_TYPE;


typedef struct __attribute__((packed))
{
    uint8_t     opt;        // should be TCPIP_DHCP_PARAM_REQUEST_IP_ADDRESS
    uint8_t     len;        // 4
    uint8_t     reqIpAddr[4];    // requested IP address
}TCPIP_DHCP_OPTION_DATA_REQ_ADDRESS;

typedef struct __attribute__((packed))
{
    uint8_t     opt;        // should be TCPIP_DHCP_PARAM_REQUEST_CLIENT_ID
    uint8_t     len;        // >= 2
    struct
    {
        uint8_t     type;       // hardware type according to "Assigned Numbers"
#if defined(TCPIP_STACK_ALIAS_INTERFACE_SUPPORT) && (TCPIP_STACK_ALIAS_INTERFACE_SUPPORT != 0)
        uint8_t     id[sizeof(TCPIP_MAC_ADDR) + 2]; // client ID; we use the hw address of the interface + IF number:
#else
        uint8_t     id[sizeof(TCPIP_MAC_ADDR)]; // client ID; we use the hw address of the interface
#endif  // defined(TCPIP_STACK_ALIAS_INTERFACE_SUPPORT) && (TCPIP_STACK_ALIAS_INTERFACE_SUPPORT != 0)

    }cliId;
}TCPIP_DHCP_OPTION_DATA_CLIENT_ID;


typedef struct __attribute__((packed))
{
    uint8_t     opt;        // should be TCPIP_DHCP_SUBNET_MASK
    uint8_t     len;        // 4
    uint8_t     mask[4];    // subnet mask
}TCPIP_DHCP_OPTION_DATA_SUBNET_MASK;


typedef struct __attribute__((packed))
{
    uint8_t     opt;        // should be TCPIP_DHCP_ROUTER
    uint8_t     len;        // >=4
    uint8_t     gateway[4];    // gateway/router address
}TCPIP_DHCP_OPTION_DATA_ROUTER;

typedef struct __attribute__((packed))
{
    uint8_t     opt;        // should be TCPIP_DHCP_TIME_SERVER
    uint8_t     len;        // >=4
    uint8_t     tServer[4]; // time server address
}TCPIP_DHCP_OPTION_TIME_SERVER;

typedef struct __attribute__((packed))
{
    uint8_t     opt;        // should be TCPIP_DHCP_NTP_SERVER
    uint8_t     len;        // >=4
    uint8_t     ntpServer[4]; // ntp server address
}TCPIP_DHCP_OPTION_NTP_SERVER;


typedef struct __attribute__((packed))
{
    uint8_t     opt;        // should be TCPIP_DHCP_DNS
    uint8_t     len;        // >= 4
    uint8_t     dns1[4];    // DNS1 address
}TCPIP_DHCP_OPTION_DATA_DNS1;

typedef struct __attribute__((packed))
{
    uint8_t     opt;        // should be TCPIP_DHCP_DNS
    uint8_t     len;        // >= 8
    uint8_t     dns1[4];    // DNS1 address
    uint8_t     dns2[4];    // DNS2 address
}TCPIP_DHCP_OPTION_DATA_DNS;


typedef struct __attribute__((packed))
{
    uint8_t     opt;        // should be TCPIP_DHCP_SERVER_IDENTIFIER
    uint8_t     len;        // 4
    uint8_t     serverId[4];    // server ID
}TCPIP_DHCP_OPTION_DATA_SERVER_ID;


typedef struct __attribute__((packed))
{
    uint8_t     opt;        // should be TCPIP_DHCP_IP_LEASE_TIME
    uint8_t     len;        // 4
    uint8_t     lease[4];    // lease time
}TCPIP_DHCP_OPTION_DATA_LEASE_TIME;

typedef struct __attribute__((packed))
{
    uint8_t     opt;        // should be TCPIP_DHCP_RENEW_TIME
    uint8_t     len;        // 4
    uint8_t     tRenew[4];    // renew time
}TCPIP_DHCP_OPTION_DATA_RENEW_TIME;

typedef struct __attribute__((packed))
{
    uint8_t     opt;        // should be TCPIP_DHCP_REBIND_TIME
    uint8_t     len;        // 4
    uint8_t     tRebind[4];    // rebind time
}TCPIP_DHCP_OPTION_DATA_REBIND_TIME;


typedef struct __attribute__((packed))
{
    uint8_t     opt;        // should be TCPIP_DHCP_PARAM_REQUEST_LIST
    uint8_t     len;        // length of the following options
    uint8_t     optList[2]; // option list: optList[]
}TCPIP_DHCP_OPTION_DATA_REQ_LIST;


typedef struct __attribute__((packed))
{
    uint8_t     opt;        // should be TCPIP_DHCP_HOST_NAME
    uint8_t     len;        // length of the hostname field
    uint8_t     hostname[2]; // host name: hostname[]
}TCPIP_DHCP_OPTION_DATA_HOST_NAME;


typedef struct __attribute__((packed))
{
    uint8_t     opt;        // should be TCPIP_DHCP_END_OPTION
}TCPIP_DHCP_OPTION_DATA_END;

// data used for the process/dispatch operation
typedef struct
{
    // union of the possible options
    union
    {
        uint8_t*            pOpt;       // current option pointer
        TCPIP_DHCP_OPTION_DATA_MSG_TYPE* pMsgType;
        TCPIP_DHCP_OPTION_DATA_SUBNET_MASK* pSubMask;
        TCPIP_DHCP_OPTION_DATA_ROUTER* pRouter;
        TCPIP_DHCP_OPTION_TIME_SERVER* pOptTServer;
        TCPIP_DHCP_OPTION_NTP_SERVER* pOptNtpServer;
        TCPIP_DHCP_OPTION_DATA_DNS* pDns;
        TCPIP_DHCP_OPTION_DATA_SERVER_ID* pServId;
        TCPIP_DHCP_OPTION_DATA_LEASE_TIME* pLease;
        TCPIP_DHCP_OPTION_DATA_RENEW_TIME* pRenew;
        TCPIP_DHCP_OPTION_DATA_REBIND_TIME* pRebind;
    };

    int32_t             optSize;    // current option size
    unsigned int        msgType;    // current message type
    TCPIP_UINT32_VAL    serverID;   // server ID for the transaction
    TCPIP_UINT32_VAL    leaseTime;  // lease time or 0 if not valid
    TCPIP_UINT32_VAL    renewTime;  // renew time or 0 if not valid
    TCPIP_UINT32_VAL    rebindTime; // rebind time or 0 if not valid
}TCPIP_DHCP_OPTION_PROCESS_DATA;


// data used for the send/write operation
typedef struct
{
    // union of the possible write options
    union
    {
        uint8_t*            pOpt;       // current option pointer
        TCPIP_DHCP_OPTION_DATA_MSG_TYPE* pMsgType;
        TCPIP_DHCP_OPTION_DATA_SERVER_ID* pServId;
        TCPIP_DHCP_OPTION_DATA_REQ_LIST* pReqL;
        TCPIP_DHCP_OPTION_DATA_REQ_ADDRESS* pReqAdd;
        TCPIP_DHCP_OPTION_DATA_HOST_NAME* pHost;
        TCPIP_DHCP_OPTION_DATA_CLIENT_ID* pClientId;
        TCPIP_DHCP_OPTION_DATA_END* pEnd; 
    };
    int32_t             writeSpace; // current available write space
    int32_t             writeBytes; // written bytes
    unsigned int        msgType;    // current message type

}TCPIP_DHCP_OPTION_WRITE_DATA;


// current operation performed by the DHCP SM
typedef enum
{
    TCPIP_DHCP_OPER_NONE,             // invalid operation       
    TCPIP_DHCP_OPER_INIT,             // initialization, all new process
    TCPIP_DHCP_OPER_INIT_REBOOT,      // use a known IP address
    TCPIP_DHCP_OPER_RENEW,            // lease renewal
}TCPIP_DHCP_OPERATION_TYPE;

// operation requested by a DHCP user
typedef enum
{
    TCPIP_DHCP_OP_REQ_ENABLE,       // enable the service
    TCPIP_DHCP_OP_REQ_RENEW,        // perform a renewal
    TCPIP_DHCP_OP_REQ_REQUEST,      // perform a request with a known IP address
}TCPIP_DHCP_OPERATION_REQ;

// DHCP send operation flags
typedef enum
{
    TCPIP_DHCP_FLAG_SEND_BCAST    = 0x01,     // send a bcast message; otherwise unicast
    TCPIP_DHCP_FLAG_SEND_ZERO_ADD = 0x02,     // use 0 IP address to send DHCP message; otherwise net if IP address
}TCPIP_DHCP_OPERATION_FLAGS;

// DHCP event registration

typedef struct  S_TAG_DHCP_LIST_EV_NODE
{
    struct S_TAG_DHCP_LIST_EV_NODE* next;       // next node in list
                                                // makes it valid SGL_LIST_NODE node
    TCPIP_DHCP_EVENT_HANDLER        handler;    // if !NULL, handler to be called for event
    TCPIP_DHCP_EVENT_HANDLER_EX     xhandler;   // if !NULL, extended handler to be called for event
    const void*                     hParam;     // handler parameter
    TCPIP_NET_HANDLE                hNet;       // interface that's registered for
                                                // 0 if all    
}TCPIP_DHCP_LIST_EV_NODE;


// event source type: client/server initiated, illegal
// Note: 8 bit value!
typedef enum
{
    TCPIP_DHCP_EV_SRC_NONE = 0,     // none/illegal
    TCPIP_DHCP_EV_SRC_SERVER,       // server initiated event
    TCPIP_DHCP_EV_SRC_CLIENT,       // client initiated event
    TCPIP_DHCP_EV_SRC_CONN,         // connection initiated event
}TCPIP_DHCP_EV_SOURCE;


#endif  // H_DHCP_PRIVATE_H_



