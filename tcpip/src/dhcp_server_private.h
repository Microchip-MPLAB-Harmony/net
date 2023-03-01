/*******************************************************************************
  DHCPS Private file for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    dhcp_server_private.h

  Summary:
    DHCPS Private file for Microchip TCP/IP Stack

  Description:
    This file provides the TCP/IP Stack DHCPS Private file API definitions.
*******************************************************************************/
// DOM-IGNORE-BEGIN

/*
Copyright (C) 2012-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef _DHCP_SERVER_PRIVATE_H_ 
#define _DHCP_SERVER_PRIVATE_H_


// internal definitions

// number of interfaces the stack is built with
// could be greater than TCPIP_DHCPS_INTERFACE_COUNT (for ex 5 interfaces but server only on 2)
#if defined(TCPIP_STACK_NETWORK_INTERAFCE_COUNT) && (TCPIP_STACK_NETWORK_INTERAFCE_COUNT >= TCPIP_DHCPS_INTERFACE_COUNT)
#define _TCPIP_DHCPS_NET_INTERFACES_COUNT   TCPIP_STACK_NETWORK_INTERAFCE_COUNT
#else
#define _TCPIP_DHCPS_NET_INTERFACES_COUNT   TCPIP_DHCPS_INTERFACE_COUNT
#endif

// mimimum leases to allocate
#define _TCPIP_DHCPS_MIN_LEASES  32

// round up to M32
#if (TCPIP_DHCPS_MAX_LEASES == 0)
#define _TCPIP_DHCPS_USR_MAX_LEASES _TCPIP_DHCPS_MIN_LEASES
#else
#define _TCPIP_DHCPS_USR_MAX_LEASES (((TCPIP_DHCPS_MAX_LEASES + 31) / 32) * 32)
#endif

#if _TCPIP_DHCPS_USR_MAX_LEASES < _TCPIP_DHCPS_MIN_LEASES
#define _TCPIP_DHCPS_MAX_LEASES _TCPIP_DHCPS_MIN_LEASES
#else
#define _TCPIP_DHCPS_MAX_LEASES _TCPIP_DHCPS_USR_MAX_LEASES
#endif

#define _TCPIP_DHCPS_CLIENT_ID_MIN_SIZE     6
#if TCPIP_DHCPS_CLIENT_ID_MAX_SIZE < _TCPIP_DHCPS_CLIENT_ID_MIN_SIZE
#define _TCPIP_DHCPS_CLIENT_ID_SIZE _TCPIP_DHCPS_CLIENT_ID_MIN_SIZE
#else
#define _TCPIP_DHCPS_CLIENT_ID_SIZE TCPIP_DHCPS_CLIENT_ID_MAX_SIZE
#endif


// default probe count and timeout for address conflict detection
#define _TCPIP_DHCPS_DEFAULT_PROBE_COUNT    1
#define _TCPIP_DHCPS_DEFAULT_PROBE_TMO      10  // ms
// number of attempts when conflict was detedted
#define _TCPIP_DHCPS_DEFAULT_REPROBE_COUNT  1

// some defaults: lease min time
#define _TCPIP_DHCPS_LEASE_DEFAULT_MIN_TIME     (60)                // 1 minute

// default unrequested offer timeout
#define _TCPIP_DHCPS_LEASE_DEFAULT_UNREQ_OFFER_TMO      5

// min/max ping buffer size
#define _TCPIP_DHCPS_PING_BUFF_MAX_SIZE     32
#define _TCPIP_DHCPS_PING_BUFF_MIN_SIZE     8

#if (TCPIP_DHCPS_ICMP_ECHO_DATA_SIZE > _TCPIP_DHCPS_PING_BUFF_MAX_SIZE) 
#define _TCPIP_DHCPS_PING_BUFF_SIZE _TCPIP_DHCPS_PING_BUFF_MAX_SIZE
#elif (TCPIP_DHCPS_ICMP_ECHO_DATA_SIZE < _TCPIP_DHCPS_PING_BUFF_MIN_SIZE)
#define _TCPIP_DHCPS_PING_BUFF_SIZE _TCPIP_DHCPS_PING_BUFF_MIN_SIZE
#else
#define _TCPIP_DHCPS_PING_BUFF_SIZE TCPIP_DHCPS_ICMP_ECHO_DATA_SIZE
#endif

// default T1 and T2 factors
#if defined(TCPIP_DHCPS_OPTION_T1_T2_SUPPRESS) && (TCPIP_DHCPS_OPTION_T1_T2_SUPPRESS == true)
#define _TCPIP_DHCPS_OPTION_T1_T2_SUPPRESS  1
#else
#define _TCPIP_DHCPS_OPTION_T1_T2_SUPPRESS  0
#endif

#define _TCPIP_DHCPS_DEFAULT_T1_MULT    1
#define _TCPIP_DHCPS_DEFAULT_T1_DIV     2

#define _TCPIP_DHCPS_DEFAULT_T2_MULT    7
#define _TCPIP_DHCPS_DEFAULT_T2_DIV     8


// notification
#if (TCPIP_DHCPS_REPORT_ERROR_EVENT != 0 || TCPIP_DHCPS_REPORT_CLIENT_EVENT != 0) && (TCPIP_DHCPS_MAX_EVENT_REGISTRATIONS != 0)
#define _TCPIP_DHCPS_NOTIFICATIONS_ENABLE   1
#else
#define _TCPIP_DHCPS_NOTIFICATIONS_ENABLE   0
#endif  // (TCPIP_DHCPS_REPORT_ERROR_EVENT != 0 || TCPIP_DHCPS_REPORT_CLIENT_EVENT != 0) && (TCPIP_DHCPS_MAX_EVENT_REGISTRATIONS != 0)

// ARP injection timeout
#define _TCPIP_DHCPS_ARP_INJECT_TMO         1000   // milliseconds

// debugging
#define _TCPIP_DHCPS_DEBUG_MASK_BASIC           (0x0001)    // prints out assertions and conditions
#define _TCPIP_DHCPS_DEBUG_MASK_STATUS          (0x0002)    // prints out current server status
#define _TCPIP_DHCPS_DEBUG_MASK_ERROR_EVENT     (0x0004)    // prints out error events
#define _TCPIP_DHCPS_DEBUG_MASK_CLIENT_EVENT    (0x0008)    // prints out client events
#define _TCPIP_DHCPS_DEBUG_MASK_SENT_MESSAGE    (0x0010)    // prints out messages sent to clients

// run time tests
#define _TCPIP_DHCPS_DEBUG_MASK_IGNORE_RENEW    (0x10000)    // ignore the renew message, wait for rebind
// enable IPV4 debugging levels
#define _TCPIP_DHCPS_DEBUG_LEVEL                 (0)

// buffer for RX processing
// Note: the output reply is assembled here too!
#define _TCPIP_DHCPS_PROCESS_BUFFER_SIZE        576

// minimum size of a valid DHCPs packet
#define _TCPIP_DHCPS_MIN_PROCESS_PACKET_SIZE     241

// Boot file name, null terminated string; "generic" name or null in DHCPDISCOVER, fully qualified
// directory-path name in DHCPOFFER.
#define _TCPIP_DHCPS_BOOTFILE_NAME_SIZE 128
// Optional server host name, null terminated string.
#define _TCPIP_DHCPS_HOST_NAME_SIZE     64

#define _TCPIP_DHCPS_UNUSED_BYTES_FOR_TX   (_TCPIP_DHCPS_BOOTFILE_NAME_SIZE + _TCPIP_DHCPS_HOST_NAME_SIZE)


// NOTE: without sname, fname or other extended options!
#define _TCPIP_DHCPS_MIN_REPONSE_PACKET_SIZE 300u

// we use the receive buffer to assemble the client packet 
// make sure there's enough space
#if (_TCPIP_DHCPS_MIN_REPONSE_PACKET_SIZE > _TCPIP_DHCPS_PROCESS_BUFFER_SIZE)
#error "DHCPS Parameters incorrectly set!"
#endif

#if defined(TCPIP_DHCPS_ENABLE_STATISTICS) && (TCPIP_DHCPS_ENABLE_STATISTICS != 0)
#define _TCPIP_DHCPS_ENABLE_STATISTICS     1
#else
#define _TCPIP_DHCPS_ENABLE_STATISTICS     0
#endif


#if defined(TCPIP_DHCPS_DYNAMIC_DB_ACCESS) && (TCPIP_DHCPS_DYNAMIC_DB_ACCESS != 0)
#define _TCPIP_DHCPS_DYNAMIC_DB_ACCESS 1
#else
#define _TCPIP_DHCPS_DYNAMIC_DB_ACCESS 0
#endif

#if defined(TCPIP_DHCPS_MULTI_THREADED_ACCESS) && (TCPIP_DHCPS_MULTI_THREADED_ACCESS != 0)
#define _TCPIP_DHCPS_MULTI_THREADED_ACCESS 1
#else
#define _TCPIP_DHCPS_MULTI_THREADED_ACCESS 0
#endif


// DHCP magic cookie, network order
#define _TCPIP_DHCPS_MAGIC_COOKIE_NET              0x63825363ul
// DHCP magic cookie, host order
#define _TCPIP_DHCPS_MAGIC_COOKIE_HOST             0x63538263ul

#define _TCPIP_DHCPS_BOOT_REQUEST                        (1u)    // BOOT_REQUEST DHCP type
#define _TCPIP_DHCPS_BOOT_REPLY                          (2u)    // BOOT_REPLY DHCP type
#define _TCPIP_DHCPS_BOOT_HW_TYPE                        (1u)    // BOOT_HW_TYPE DHCP type
#define _TCPIP_DHCPS_BOOT_LEN_OF_HW_TYPE                 (6u)    // BOOT_LEN_OF_HW_TYPE DHCP type


// DHCP options
//

// list of (supported) DHCP server options
typedef enum
{
    DHCP_OPTION_SUBNET_MASK             = 1,    // subnet option
    DHCP_OPTION_ROUTER                  = 3,    // router option
    DHCP_OPTION_TIME_SERVER             = 4,    // time server option
    DHCP_OPTION_NAME_SERVER             = 5,    // name server option
    DHCP_OPTION_DNS                     = 6,    // domain name server option
    DHCP_OPTION_HOST_NAME               = 12,   // host name option
    DHCP_OPTION_NTP_SERVER              = 42,   // NTP server option
    DHCP_OPTION_REQUEST_IP_ADDRESS      = 50,   // ip address option
    DHCP_OPTION_IP_LEASE_TIME           = 51,   // ip lease time option
    DHCP_OPTION_MESSAGE_TYPE            = 53,   // message type option
    DHCP_OPTION_SERVER_IDENTIFIER       = 54,   // server identifier option
    DHCP_OPTION_PARAM_REQUEST_LIST      = 55,   // param request list option
    DHCP_OPTION_MESSAGE                 = 56,   // message option
    DHCP_OPTION_RENEWAL_T1              = 58,   // renewal T1 time value
    DHCP_OPTION_REBINDING_T2            = 59,   // rebinding T2 time value

    DHCP_OPTION_CLIENT_ID               = 61,   // client ID option

    //
    DHCP_OPTION_END                     = 255,  // end option

}DHCP_OPTIONS;


// types corresponding to the DHCP_OPTION_MESSAGE_TYPE (53) option
typedef enum
{
    DHCP_MESSAGE_TYPE_DISCOVER       = 1,        // DCHP Discover Message
    DHCP_MESSAGE_TYPE_OFFER          = 2,        // DHCP Offer Message
    DHCP_MESSAGE_TYPE_REQUEST        = 3,        // DHCP Request message
    DHCP_MESSAGE_TYPE_DECLINE        = 4,        // DHCP Decline Message
    DHCP_MESSAGE_TYPE_ACK            = 5,        // DHCP ACK Message
    DHCP_MESSAGE_TYPE_NAK            = 6,        // DHCP NAK Message
    DHCP_MESSAGE_TYPE_RELEASE        = 7,        // DCHP Release message
    DHCP_MESSAGE_TYPE_INFORM         = 8,        // DCHP INFORM Message
}DHCP_MESSAGE_TYPE_VAL;



// DHCP option data structures
typedef struct __attribute__((packed))
{
    uint8_t     optionType; // Option type 
    uint8_t     optionLen;  // Option len
    uint8_t     byteVal;    // byte value    
}TCPIP_DHCPS_OPTION_BYTE_DATATYPE;

typedef struct __attribute__((packed))
{
    uint8_t     optionType; // Option type 
    uint8_t     optionLen;  // Option len
    uint16_t    shortVal;   // 16 bit value 
}TCPIP_DHCPS_OPTION_SHORT_DATATYPE;

typedef struct __attribute__((packed))
{
    uint8_t     optionType; // Option type 
    uint8_t     optionLen;  // Option len
    uint32_t    intVal;     // 32 bit value
}TCPIP_DHCPS_OPTION_INT_DATATYPE;

typedef struct __attribute__((packed))
{
    uint8_t     optionType; // Option type 
    uint8_t     optionLen;  // Option len
    uint32_t    intVal[];   // multiple 32 bit values
}TCPIP_DHCPS_OPTION_MULT_INT_DATATYPE;

typedef struct __attribute__((packed))
{
    uint8_t     optionType; // Option type 
    uint8_t     optionLen;  // Option len
    uint8_t     byteVal[];  // multiple 8 bit values
}TCPIP_DHCPS_OPTION_MULT_BYTE_DATATYPE;

// all supported options transmitted by the server
typedef union 
{
    TCPIP_DHCPS_OPTION_BYTE_DATATYPE messageType ;      // Message Type
    TCPIP_DHCPS_OPTION_INT_DATATYPE  subnetMaskType;    // subnet mask type
    TCPIP_DHCPS_OPTION_INT_DATATYPE  serverIdType;      // server Identifier Type
    TCPIP_DHCPS_OPTION_INT_DATATYPE  routerType;        // router Type
    TCPIP_DHCPS_OPTION_INT_DATATYPE  dnsType;           // DNS type
    TCPIP_DHCPS_OPTION_INT_DATATYPE  ipLeaseTimeType;   // IP lease time type
    TCPIP_DHCPS_OPTION_INT_DATATYPE  t1RenewalType;     // T1 renewal time type
    TCPIP_DHCPS_OPTION_INT_DATATYPE  t2RebindingType;   // T2 rebinding time type
    TCPIP_DHCPS_OPTION_MULT_INT_DATATYPE multIntType;   // multiple 32 bit values data type
    TCPIP_DHCPS_OPTION_MULT_BYTE_DATATYPE multByteType; // multiple byte values data type
}TCPIP_DHCPS_TX_OPTIONS;

// descriptor for messages sent by the server
typedef struct
{
    uint32_t    clientIP;       // clientIP address field
    uint32_t    yourIP;         // yourIP address field
    uint32_t    nextServerIP;   // IP address of next server to use in bootstrap
    uint8_t     optionType;     // option to use for the DHCP message type
    uint8_t     optionVal;      // value for this option

}TCPIP_DHCPS_TX_DCPT;

// RX messages 

typedef struct __attribute__((packed))
{
    uint8_t     optionType; // should be DHCP_OPTION_MESSAGE_TYPE
    uint8_t     optionLen;  // 1
    uint8_t     type;       // DHCP message type
}TCPIP_DHCPS_OPTION_DATA_MSG_TYPE;

typedef struct __attribute__((packed))
{
    uint8_t optionType; // DHCP_OPTION_CLIENT_ID
    uint8_t optionLen;  // >= 2
    uint8_t hwType;     // should be BOOT_HW_TYPE                        
    uint8_t clientId[_TCPIP_DHCPS_CLIENT_ID_SIZE];  // client ID
                                                    // Note that a larger ID will be rejected!
    
}TCPIP_DHCPS_OPTION_REQUEST_CLIENT_ID;

typedef struct __attribute__((packed))
{
    uint8_t     optionType;     // DHCP_OPTION_REQUEST_IP_ADDRESS
    uint8_t     optionLen;      // 4
    uint32_t    reqIpAddr;      // client requested IP address
}TCPIP_DHCPS_OPTION_REQUEST_IP_ADDRESS;

typedef struct __attribute__((packed))
{
    uint8_t     optionType;     // should be DHCP_OPTION_IP_LEASE_TIME
    uint8_t     optionLen;      // 4
    uint8_t     lease[4];       // lease time
}TCPIP_DHCPS_OPTION_REQUEST_LEASE_TIME;

typedef struct __attribute__((packed))
{
    uint8_t     optionType;     // should be DHCP_OPTION_SERVER_IDENTIFIER
    uint8_t     optionLen;      // 4
    uint8_t     id[4];          // server identifier
}TCPIP_DHCPS_OPTION_SERVER_IDENTIFIER;


typedef struct __attribute__((packed))
{
    uint8_t     optionType;        // should be DHCP_OPTION_END
}TCPIP_DHCP_OPTION_DATA_END;


// all supported options recccceived/processed by the server 
// an option that's parsed in the message has its type set
// this is what makes it valid
typedef struct
{
    TCPIP_DHCPS_OPTION_DATA_MSG_TYPE        messageType;
    TCPIP_DHCPS_OPTION_REQUEST_CLIENT_ID    requestClientId;
    TCPIP_DHCPS_OPTION_REQUEST_IP_ADDRESS   requestAddress;
    TCPIP_DHCPS_OPTION_REQUEST_LEASE_TIME   requestLeaseTime;
    TCPIP_DHCPS_OPTION_SERVER_IDENTIFIER    serverIdentifier;
    size_t                                  idSize;     // actual size of the following ID
    uint8_t                                 clientId[_TCPIP_DHCPS_CLIENT_ID_SIZE];   // client ID/hw address
}TCPIP_DHCPS_RX_OPTIONS;



// DHCP or BOOTP Header structure

#define _TCPIP_DHCPS_BOOTP_FLAG_BROADCAST        0x80

typedef struct __attribute__((aligned(2), packed))
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
    uint8_t     chaddr[16];     // Client hardware (MAC) Address
} BOOTP_HEADER;

// flags associated with the client message
typedef enum
{
    TCPIP_DHCPS_CLIENT_MESSAGE_FLAG_BROADCAST   = 0x01,     // message is broadcast, otherwise unicast
}TCPIP_DHCPS_CLIENT_MESSAGE_FLAGS;


// extra run time flags for a specific lease/hash entry in the server cache/database
// 8 bits only
typedef enum
{
    DHCPS_RUN_FLAG_NONE         = 0x00,

    DHCPS_RUN_FLAG_ARP_INJECT   = 0x01,     // corresponding ip addr/chaddr has been injected to the ARP cache 
    DHCPS_RUN_FLAG_PERM_LEASE   = 0x02,     // lease marked as permanent (management)

}DHCPS_RUN_FLAGS;

// provide a reason for DHCP_MESSAGE_TYPE_NAK
// currently only 1 reason, but it may be extended
typedef enum
{
    DHCP_NAK_CODE_NONE      = 0,    // none/invalid
    DHCP_NAK_CODE_REQ_ADDRESS_INVALID,      // requested address was invalid
    //
    //
}DHCP_MESSAGE_TYPE_NAK_CODE;



// DHCP Server cache entry
typedef struct  _TAG_DHCPS_HASH_ENTRY 
{
    OA_HASH_ENTRY   hEntry; // hash header;
    struct _tag_TCPIP_DHCPS_INTERFACE_DCPT* parent;    // interface descriptor it belongs to
    uint32_t        leaseStartTime;     // time when bound or when offer was sent, seconds
    uint32_t        probeStartTimeMs;   // ms time when echo probe was sent
    uint32_t        leaseEndTime;       // leaseStartTime + cliLeaseDuration, seconds;
    uint32_t        cliLeaseTime;       // lease time to be sent to the client, seconds;
    uint32_t        ciaddr;             // Client IP in the request we're serving
    uint32_t        giaddr;             // Relay Agent IP in the request we're serving
    uint32_t        xid;                // DHCP Transaction ID
    IPV4_ADDR       ipAddress;          // the assigned IP address;
    uint32_t        injectTimeMs;       // ARP injection time, ms
    IPV4_ADDR       injectAdd;          // the ARP injected IP address
    TCPIP_ICMP_REQUEST_HANDLE probeHandle; // probe handle for address conflict detection 

    uint16_t        cliFlags;           // Client Flags
    uint8_t         srvMsgType;         // DHCP_MESSAGE_TYPE_VAL value for the server reply:
                                        // {DHCP_MESSAGE_TYPE_OFFER, DHCP_MESSAGE_TYPE_ACK, DHCP_MESSAGE_TYPE_NAK} 
    uint8_t         cliMsgType;         // DHCP_MESSAGE_TYPE_VAL value representing the client request:
                                        // {DHCP_MESSAGE_TYPE_DISCOVER, DHCP_MESSAGE_TYPE_REQUEST, DHCP_MESSAGE_TYPE_DECLINE, DHCP_MESSAGE_TYPE_RELEASE, DHCP_MESSAGE_TYPE_INFORM}
    uint8_t         state;              // TCPIP_DHCPS_LEASE_STATE value, the current state
    uint8_t         prevState;          // TCPIP_DHCPS_LEASE_STATE value, the previous state, debugging
    uint8_t         probeCount;         // current probe count when doing address conflict detection
    uint8_t         reprobeCount;       // current reprobe count when doing address conflict detection
    uint8_t         icmpRetries;        // if ICMP echo fails, number of retries
    uint8_t         clientIdLen;        // actual length of the client ID for this entry 
    uint8_t         runFlags;           // DHCPS_RUN_FLAGS value
    uint8_t         nakCode;            // a DHCP_MESSAGE_TYPE_NAK_CODE value
    //
    uint8_t         chaddr[16];         // Client hardware (MAC) Address
    //
    uint8_t         clientId[_TCPIP_DHCPS_CLIENT_ID_SIZE];  // the hash key: 
                                                            // either the hardware MAC address
                                                            // or the client selected ID
                                                            // Since there's no support for variable key size
}DHCPS_HASH_ENTRY;
#define     DHCPS_HASH_PROBE_STEP      1    // step to advance for hash collision
#define     DHCPS_HASH_KEY_SIZE        (sizeof(((DHCPS_HASH_ENTRY*)0)->clientId))


// DHCP event registration
typedef struct
{
    TCPIP_DHCPS_EVENT_HANDLER       handler;    // handler to be called for event
    const void*                     hParam;     // handler parameter
    TCPIP_NET_HANDLE                hNet;       // interface that's registered for
                                                // 0 if all    
}TCPIP_DHCPS_EVENT_NODE;

// supported client options

// flags corresponding to the TCPIP_DHCPS_CLIENT_OPTION_TYPE s
// max 32 supported for now!
typedef enum
{
    TCPIP_DHCPS_CLIENT_OPTION_FLAG_ROUTER       = 0x0001,
    TCPIP_DHCPS_CLIENT_OPTION_FLAG_DNS          = 0x0002,
    TCPIP_DHCPS_CLIENT_OPTION_FLAG_TIME_SERVER  = 0x0004,
    TCPIP_DHCPS_CLIENT_OPTION_FLAG_NAME_SERVER  = 0x0008,
    TCPIP_DHCPS_CLIENT_OPTION_FLAG_NTP_SERVER   = 0x0010,

}TCPIP_DHCPS_CLIENT_OPTION_FLAG;


typedef struct
{
#if (TCPIP_DHCPS_OPTION_ROUTER_VALUES  != 0)
    uint32_t        cliRouter[TCPIP_DHCPS_OPTION_ROUTER_VALUES];// router addresses
#endif  // (TCPIP_DHCPS_OPTION_ROUTER_VALUES  != 0)
#if (TCPIP_DHCPS_OPTION_DNS_VALUES  != 0)
    uint32_t        cliDNS[TCPIP_DHCPS_OPTION_DNS_VALUES];// DNS addresses
#endif  // (TCPIP_DHCPS_OPTION_DNS_VALUES  != 0)
#if (TCPIP_DHCPS_OPTION_TIME_SERVER_VALUES  != 0)
    uint32_t        cliTServer[TCPIP_DHCPS_OPTION_TIME_SERVER_VALUES];// time server addresses
#endif  // (TCPIP_DHCPS_OPTION_TIME_SERVER_VALUES  != 0)
#if (TCPIP_DHCPS_OPTION_NAME_SERVER_VALUES  != 0)
    uint32_t        cliNServer[TCPIP_DHCPS_OPTION_NAME_SERVER_VALUES];// name server addresses
#endif  // (TCPIP_DHCPS_OPTION_NAME_SERVER_VALUES  != 0)
#if (TCPIP_DHCPS_OPTION_NTP_SERVER_VALUES  != 0)
    uint32_t        cliNtpServer[TCPIP_DHCPS_OPTION_NTP_SERVER_VALUES];// time server addresses
#endif  // (TCPIP_DHCPS_OPTION_NTP_SERVER_VALUES  != 0)
#if (_TCPIP_DHCPS_OPTION_T1_T2_SUPPRESS == 0)
    uint16_t        t1Mult;         // multiplication factor of lease time for the T1 renewal time; default 1
    uint16_t        t1Div;          // divide factor of lease time for the T1 renewal time; default 2
                                    // T1 = (leaseTime * mult) / div;
    uint16_t        t2Mult;         // multiplication factor of lease time for the T2 rebinding time; default 7
    uint16_t        t2Div;          // divide factor of lease time for the T2 rebinding time; default 8
                                    // T2 = (leaseTime * mult) / div;
#endif  // (_TCPIP_DHCPS_OPTION_T1_T2_SUPPRESS == 0)
    uint32_t        cliOptFlags;    // TCPIP_DHCPS_CLIENT_OPTION_FLAG value, the (extra) supported client options
}TCPIP_DHCPS_CLIENT_OPTIONS;



typedef struct _tag_TCPIP_DHCPS_INTERFACE_DCPT
{
    uint16_t        configFlags;        // a TCPIP_DHCPS_CONFIG_FLAGS value 
    uint16_t        leaseEntries;       // max number of leases for this interface
    TCPIP_NET_IF*   pNetIf;             // interface it refers to 
    IPV4_ADDR       serverIPAddress;    // Interface IP address when DHCP server is enabled
    IPV4_ADDR       ipMaskAddress;      // Interface NetMask address when DHCP server is enabled
    IPV4_ADDR       startIPAddress;     // Start of IPv4 address of the DHCP server Pool

    OA_HASH_DCPT*   hashDcpt;           // hash descriptor for this interface
    uint32_t        cliLeaseDuration;   // lease duration, seconds 
                                        // this applies for all clients
    uint32_t        minLeaseDuration;   // min lease duration that can be requested, seconds 
    uint32_t        maxLeaseDuration;   // max lease duration that can be requested, seconds 
    uint32_t        unreqOfferTmo;      // timeout for unrequested offer sent to the client, seconds 

    TCPIP_DHCPS_CLIENT_OPTIONS  cliOptions; // associated options to be transmitted to the clients

    uint16_t        ipMapSize;      // number of 32 bit words in the IP bitmap == leaseEntries / 32

    uint8_t         prefixLen;      // CIDR notation prefix len;
    uint8_t         msgFlags;       // TCPIP_DHCPS_CLIENT_MESSAGE_FLAGS: current message flags
    
    // statistics
#if (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 
    TCPIP_DHCPS_STATISTICS_DATA statData;   // statistics counters
#endif  // (_TCPIP_DHCPS_ENABLE_STATISTICS != 0) 

    uint32_t        ipMap[_TCPIP_DHCPS_MAX_LEASES / 32];        // bitmap of IP addresses; a set bit specifies an available address

}TCPIP_DHCPS_INTERFACE_DCPT;    // descriptor per interface

typedef enum __attribute__((packed))
{
    DHCPS_IF_FLAG_MAPPED        = 0x01,     // interface is mapped to DHCPs, in use

    // other flags if needed
    //
}DHCPS_IF_FLAG;


typedef struct
{
    uint8_t     mapFlag;        // a DHCPS_IF_FLAG value
    uint8_t     mapIndex;       // index where this interface is mapped in TCPIP_DHCPS_DCPT::ifDcpt
}DHCPS_IF_MAP;

// DHCP server descriptor table is used to collect DHCP server pool address and all
// the entry are valid entries.
typedef struct
{
    const void*             memH;           // DHCP memory handle
    tcpipSignalHandle       stackSigHandle; // DHCP asyncronous timer handle
    UDP_SOCKET              uSkt;           // Socket used by DHCP Server
    uint16_t                probeTmoMs;     // timeout to wait for reply to an ICMP echo probe, ms - default 10 ms
    uint8_t                 nProbes;        // number of ICMP echo probes to send when doing address conflict detection - default 1
    uint8_t                 nReprobes;      // number of ICMP echo reprobe attempts when address conflict detected
    uint8_t                 ifCount;        // number of interface descriptors == TCPIP_DHCPS_INTERFACE_COUNT
    uint8_t                 accessLock;     // lock for multi-threaded access
    uint16_t                icmpSequenceNo; // current ICMP sequence number
    uint16_t                icmpIdentifier; // current ICMP identifier
    uint16_t                maxLeases;      // max number of leases for any interface
    int16_t                 maxIx;          // maximum index used in the ifDcpt
    uint16_t                stackIfs;       // number of stack interfaces in the current run
    DHCPS_IF_MAP            ifMap[_TCPIP_DHCPS_NET_INTERFACES_COUNT]; // interface mapping in ifDcpt 
#if (_TCPIP_DHCPS_NOTIFICATIONS_ENABLE != 0)
    TCPIP_DHCPS_EVENT_NODE  registeredUsers[TCPIP_DHCPS_MAX_EVENT_REGISTRATIONS]; // users to receive notification 
#endif  // (_TCPIP_DHCPS_NOTIFICATIONS_ENABLE != 0)

    TCPIP_DHCPS_INTERFACE_DCPT ifDcpt[TCPIP_DHCPS_INTERFACE_COUNT];    // each interface descriptor 

}TCPIP_DHCPS_DCPT;    // DHCP server descriptor

// This one is used for UDPGET and UDPPUT
typedef struct
{
    uint8_t* headPtr;   // pointer to the beginning of the allocated buffer
    uint8_t* rdPtr;     // current read pointer
    uint8_t* endPtr;    // end pointer, initialized to head + len when buffer allocated
}TCPIP_DHCPS_DATA;

// helpers for processing the received options
// DHCPS option processing function
typedef bool(*_DHCPS_OPT_FUNC)(uint8_t* pOpt, TCPIP_DHCPS_RX_OPTIONS* pRxOpt);

// descriptor structure for processing options
typedef struct
{
    uint8_t         optionType; // the option type
    uint8_t         minLen;     // minimum allowed length
    uint8_t         maxLen;     // maximum allowed length
    _DHCPS_OPT_FUNC optFunc;    // processing function
}_DHCPS_OPT_DCPT;


// DHCPS message processing function
// returns the new state for the hash entry
typedef TCPIP_DHCPS_LEASE_STATE (*_DHCPS_MSG_PROC_FUNC)(TCPIP_DHCPS_INTERFACE_DCPT* pIDcpt, BOOTP_HEADER* pHeader, TCPIP_DHCPS_RX_OPTIONS* pRxOpt, DHCPS_HASH_ENTRY** ppHe);

typedef enum __attribute__((packed))
{
    DHCPS_PROC_FLAG_NONE        = 0x00,     // no flag
    DHCPS_PROC_FLAG_CHECK_SIZE  = 0x01,     // check socket size

}DHCPS_MSG_PROC_FLAG;

// message processing descriptor
typedef struct
{
    uint8_t                 msgType;    // DHCP_MESSAGE_TYPE_VAL value
                                        // type of DHCPS message processed
    uint8_t                 procFlags;  // DHCPS_MSG_PROC_FLAG value
    _DHCPS_MSG_PROC_FUNC    procFunc;   // message processing function
}DHCPS_MSG_PROC_DCPT; 
    

// flags for extra checking the validity of a DHCP address
typedef enum
{
    DHCPS_VALID_CHECK_NONE          = 0,    // no extra check is done
    DHCPS_VALID_CHECK_FREE          = 0x01, // check that the address is free/available
    DHCPS_VALID_MARK_TAKEN          = 0x02, // mark the address as busy (only if it's available)

    DHCPS_VALID_CHECK_FREE_AND_TAKE = 0x03, // check and take
}DHCPS_VALID_CHECK;


// helpers for processing an entry status
// DHCPS status processing function
typedef void(*_DHCPS_STAT_FUNC)(DHCPS_HASH_ENTRY* he);


#endif  // _DHCP_SERVER_PRIVATE_H_ 


