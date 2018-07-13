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
/*******************************************************************************
Copyright © 2012 released Microchip Technology Inc.  All rights reserved.

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

#ifndef __DHCP_PRIVATE_H_
#define __DHCP_PRIVATE_H_

// enable DHCP debugging features
#define TCPIP_DHCP_DEBUG_MASK_LEASE_EVENTS      0x01    // enable lease related events display
#define TCPIP_DHCP_DEBUG_MASK_CONN_EVENTS       0x02    // enable connection related events display
#define TCPIP_DHCP_DEBUG_MASK_ADDRESS_EVENTS    0x04    // enable adress service events display
#define TCPIP_DHCP_DEBUG_MASK_FAIL_TMO_EVENT    0x08    // enable fail timeout events display
#define TCPIP_DHCP_DEBUG_MASK_STATUS            0x10    // enable status display
#define TCPIP_DHCP_DEBUG_MASK_RX_RES_ENABLE     0x20    // enable receive results display
#define TCPIP_DHCP_DEBUG_MASK_TX_MSG_ENABLE     0x40    // enable TX messages display
#define TCPIP_DHCP_DEBUG_MASK_TIME_RES_MS       0x80    // display time with ms resolution


#define TCPIP_DHCP_DEBUG_MASK_FAKE_TMO          0x0100    // enable fake timeout support...
#define TCPIP_DHCP_DEBUG_MASK_LEASE_DECLINE     0x0200    // enable fake lease decline
#define TCPIP_DHCP_DEBUG_MASK_FAIL_ARP          0x0400   // enable ARP failures 
#define TCPIP_DHCP_DEBUG_MASK_FAKE_LEASE_TIME   0x0800   // fake the lease time 
#define TCPIP_DHCP_DEBUG_MASK_FAKE_HOST_NAME    0x1000   // fake the host name 


#define TCPIP_DHCP_DEBUG_MASK                   (0)

#define TCPIP_BOOT_REQUEST                    (1u)	// TCPIP_BOOT_REQUEST DHCP type
#define TCPIP_BOOT_REPLY                      (2u)	// TCPIP_BOOT_REPLY DHCP type
#define TCPIP_BOOT_HW_TYPE                    (1u)	// TCPIP_BOOT_HW_TYPE DHCP type
#define TCPIP_BOOT_LEN_OF_HW_TYPE             (6u)	// TCPIP_BOOT_LEN_OF_HW_TYPE DHCP type

#define TCPIP_DHCP_MESSAGE_TYPE               (53u)	// DHCP Message Type constant

#define TCPIP_DHCP_UNKNOWN_MESSAGE            (0u)	// Code for unknown DHCP message

#define TCPIP_DHCP_DISCOVER_MESSAGE           (1u)	// DCHP Discover Message
#define TCPIP_DHCP_OFFER_MESSAGE              (2u)	// DHCP Offer Message
#define TCPIP_DHCP_REQUEST_MESSAGE            (3u)	// DHCP Request message
#define TCPIP_DHCP_DECLINE_MESSAGE            (4u)	// DHCP Decline Message
#define TCPIP_DHCP_ACK_MESSAGE                (5u)	// DHCP ACK Message
#define TCPIP_DHCP_NAK_MESSAGE                (6u)	// DHCP NAK Message
#define TCPIP_DHCP_RELEASE_MESSAGE            (7u)	// DCHP Release message
#define TCPIP_DHCP_INFORM_MESSAGE             (8u)	// DCHP INFORM Message

#define TCPIP_DHCP_REQUEST_RENEW_MESSAGE      (9u)	// internal message for Request Renew message
#define TCPIP_DHCP_TIMEOUT_MESSAGE            (10u)	// internal message to show no message available

#define TCPIP_DHCP_SERVER_IDENTIFIER          (54u)	// DHCP Server Identifier

#define TCPIP_DHCP_OPTION_ACK_MESSAGE		  (53u)	// DHCP ACK Message
#define TCPIP_DHCP_PARAM_REQUEST_LIST         (55u)	// DHCP_PARAM_REQUEST_LIST Type
#define TCPIP_DHCP_PARAM_REQUEST_IP_ADDRESS   (50u)	// DHCP_PARAM_REQUEST_IP_ADDRESS Type
#define TCPIP_DHCP_PARAM_REQUEST_CLIENT_ID    (61u)	// DHCP_PARAM_REQUEST Client Type

#define TCPIP_DHCP_SUBNET_MASK                (1u)	// DHCP_SUBNET_MASK Type
#define TCPIP_DHCP_ROUTER                     (3u)	// DHCP_ROUTER Type
#define TCPIP_DHCP_DNS						  (6u)	// DHCP_DNS Type
#define TCPIP_DHCP_HOST_NAME			      (12u)	// DHCP_HOST_NAME Type
#define TCPIP_DHCP_IP_LEASE_TIME              (51u)	// DHCP_IP_LEASE_TIME Type
#define TCPIP_DHCP_RENEW_TIME                 (58u)	// DHCP RENEW time (T1)
#define TCPIP_DHCP_REBIND_TIME                (59u)	// DHCP REBIND time (T2)

#define TCPIP_DHCP_END_OPTION                 (255u)	// DHCP_END_OPTION Type



// Default time out value for all DHCP requests; seconds
// This forms the base of the DHCP transactions timeout 
// and it's used in the expornential backoff
// Using a default value for 100Mbps networks
#define TCPIP_DHCP_EXP_BACKOFF_BASE   2

// exponential backoff limit, as set by the standard; seconds
#define TCPIP_DHCP_EXP_BACKOFF_LIMIT    64

// random variation for exponential backoff timing, seconds
// Standard sets it to 1 second 
#define TCPIP_DHCP_EXP_BACKOFF_FUZZ     1

// random variation for lease timeout; seconds
#define TCPIP_DHCP_LEASE_EXPIRE_FUZZ    10

// DHCP RENEWING state retry timeout; seconds
// Standard sets it to 60 seconds
#define TCPIP_DHCP_RENEW_RETRY_TIMEOUT  60

// DHCP REBINDING state retry timeout; seconds
// Standard sets it to 60 seconds
#define TCPIP_DHCP_REBIND_RETRY_TIMEOUT  60

// DHCP default lease time if the server doesn't provide one; seconds
// not likely to be used, the server provides one but just in case
#define TCPIP_DHCP_LEASE_TIME_DEFAULT   120

// Default time out to report the unavailability of a lease
// and switch to a different address service; seconds
// This is used only at the initialization
#define TCPIP_DHCP_INIT_FAIL_TMO        10

// Default time to wait for an ARP probe
// when checking a lease, seconds
#define TCPIP_DHCP_LEASE_CHECK_TMO       2

// Default time to attepmt a retry when the ARP
// validation failed, seconds
// Standard sets it to 10 secs
#define TCPIP_DHCP_WAIT_FAIL_CHECK_TMO   10



// DHCP UDP socket minimum size for being able to carry
// a DHCP frame
// The RFC states that the maximum message from the server 
// may be 576 for lots of options
// It may need update!
#define TCPIP_DHCP_MIN_UDP_TX_BUFFER_SIZE     512


// DHCP UDP socket minimum amount of free TX space
// Shouldn't happen because all DHCP transactions
// are followed by a flush operation
#define TCPIP_DHCP_MIN_UDP_TX_AVLBL_SIZE     300


// DHCP frame minimum packet size
// A samller packet will be padded with 0s
// Has to be < TCPIP_DHCP_MIN_UDP_TX_BUFFER_SIZE!
#define TCPIP_DHCP_MIN_TX_FRAME_SIZE        300

// DHCP host name illegal character replacement
#define TCPIP_DHCP_HOST_REPLACE_CHAR    'x'


// DHCP or BOOTP Header structure
typedef struct
{
    uint8_t	    op;             // Message type for this message
    uint8_t	    htype;	        // Hardware type for this message
    uint8_t	    hlen;	        // Length of hardware type
    uint8_t	    hops;			// Number of hops
    uint32_t    xid;	        // DHCP Transaction ID
    uint16_t	secs;	        // Number of elapsed seconds
    uint16_t	flags;		    // BOOTP Flags
    uint32_t	ciaddr;		    // Client IP
    uint32_t	yiaddr;			// Your IP
    uint32_t	siaddr;	        // Next Server IP
    uint32_t	giaddr;	        // Relay Agent IP
    uint8_t     chaddr[16];		// Client MAC Address
} TCPIP_DHCP_FRAME_HEADER;

typedef struct
{
    uint8_t	    sname[64];      // optional server host name
    uint8_t	    file[128];	    // boot file name
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

// data used for the process/dispatch operation
typedef struct
{
    uint8_t*            pOpt;       // current option pointer
    int32_t             optSize;    // current option size
    unsigned int        msgType;    // current message type
	TCPIP_UINT32_VAL    serverID;   // server ID for the transaction
    TCPIP_UINT32_VAL    leaseTime;  // lease time or 0 if not valid
    TCPIP_UINT32_VAL    renewTime;  // renew time or 0 if not valid
    TCPIP_UINT32_VAL    rebindTime; // rebind time or 0 if not valid
}TCPIP_DHCP_OPTION_PROCESS_DATA;

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
}TCPIP_DHCP_OPTION_DATA_REQUEST_IP_ADDRESS;

typedef struct __attribute__((packed))
{
    uint8_t     opt;        // should be TCPIP_DHCP_PARAM_REQUEST_CLIENT_ID
    uint8_t     len;        // >= 2
    struct
    {
        uint8_t     type;       // hardware type according to "Assigned Numbers": TCPIP_BOOT_HW_TYPE
        uint8_t     id[sizeof(TCPIP_MAC_ADDR) + 2]; // client ID; we use the hw address of the interface + IF number:
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
    uint8_t     optList[0]; // option list
}TCPIP_DHCP_OPTION_DATA_REQUEST_LIST;


typedef struct __attribute__((packed))
{
    uint8_t     opt;        // should be TCPIP_DHCP_HOST_NAME
    uint8_t     len;        // length of the hostname field
    uint8_t     hostname[0]; // host name
}TCPIP_DHCP_OPTION_DATA_HOST_NAME;


typedef struct __attribute__((packed))
{
    uint8_t     opt;        // should be TCPIP_DHCP_END_OPTION
}TCPIP_DHCP_OPTION_DATA_END;


// data used for the send/write operation
typedef struct
{
    uint8_t*            pOpt;       // current option pointer
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

typedef struct  _TAG_DHCP_LIST_NODE
{
	struct _TAG_DHCP_LIST_NODE*		next;		// next node in list
                                                // makes it valid SGL_LIST_NODE node
    TCPIP_DHCP_EVENT_HANDLER        handler;    // handler to be called for event
    const void*                     hParam;     // handler parameter
    TCPIP_NET_HANDLE                hNet;       // interface that's registered for
                                                // 0 if all    
}TCPIP_DHCP_LIST_NODE;




#endif  // __DHCP_PRIVATE_H_



