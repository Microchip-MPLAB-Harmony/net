/*******************************************************************************
  DHCP Private file for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    dhcp_private.h

  Summary:
    DHCP Private file for Microchip TCP/IP Stack

  Description:
    This file provides the TCP/IP Stack DHCPV6 Private file API definitions.
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

#ifndef __DHCPV6_PRIVATE_H_
#define __DHCPV6_PRIVATE_H_



//




// the run substate functions return a result:
//
typedef enum
{
    // results that affect the run substate
    TCPIP_DHCPV6_IA_SUBSTATE_RES_NO_ACTION,     // no action needs to be taken; could include IA delete
    TCPIP_DHCPV6_IA_SUBSTATE_RES_OK,            // everything going on as planned; could advance the run substate
    TCPIP_DHCPV6_IA_SUBSTATE_RES_PENDING,       // basically the same; operation ongoing but no advance
    TCPIP_DHCPV6_IA_SUBSTATE_RES_RETRANSMIT,    // retransmission of the message is needed


    // results that affect the run state 
    TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_NEXT,      // processing done, need to advance to the next run state
                                                // this is usually returned by the Wait substate functions
    TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_PREV,      // go back to the previous run state
    TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_JUMP,      // processing done, advance is done to a new run state
                                                // this is usually returned by the substate jumping to a new run state
    TCPIP_DHCPV6_IA_SUBSTATE_RES_RUN_RESTART,   // process needs to be restarted with the 1st run state:
                                                // ->TCPIP_DHCPV6_IA_STATE_SOLICIT; probably no usable data obtained

    // errors < 0 
    TCPIP_DHCPV6_IA_SUBSTATE_RES_ERROR_TRANSIENT    = -1,   // a transient error occurred
    TCPIP_DHCPV6_IA_SUBSTATE_RES_ERROR_FATAL        = -2,   // a fatal error occurred

}TCPIP_DHCPV6_IA_SUBSTATE_RESULT;



// DHCPV6 debugging levels/masks
// basic debugging
#define TCPIP_DHCPV6_DEBUG_MASK_BASIC           (0x0001)
// report in messages
#define TCPIP_DHCPV6_DEBUG_MASK_IN              (0x0002)
// report out messages
#define TCPIP_DHCPV6_DEBUG_MASK_OUT             (0x0004)
// report changes in the current client state
#define TCPIP_DHCPV6_DEBUG_MASK_CLIENT_STATE    (0x0008)
// report current client statistics: buffers lists and statistics counters 
#define TCPIP_DHCPV6_DEBUG_MASK_CLIENT_STATS    (0x0010)
// advanced: report changes in the current IA state
#define TCPIP_DHCPV6_DEBUG_MASK_IA_STATE        (0x0020)
// advanced: report changes in the current IA state or substate
#define TCPIP_DHCPV6_DEBUG_MASK_IA_SUBSTATE     (0x0040)
// advanced: additional state prints
#define TCPIP_DHCPV6_DEBUG_MASK_ADD_STATE       (0x0080)
// advanced: use static debugging lists
#define TCPIP_DHCPV6_DEBUG_MASK_LISTS           (0x0100)
// advanced: print buffers traces
#define TCPIP_DHCPV6_DEBUG_MASK_BUFF_TRACE      (0x0800)


// enable DHCP debugging levels
#define TCPIP_DHCPV6_DEBUG_LEVEL  (TCPIP_DHCPV6_DEBUG_MASK_BASIC | TCPIP_DHCPV6_DEBUG_MASK_IN | TCPIP_DHCPV6_DEBUG_MASK_IA_STATE | TCPIP_DHCPV6_DEBUG_MASK_CLIENT_STATE)


// don't spit out IAs that don't have valid IAs...?
//#define _TCPIP_DHCPV6_WRITE_IA_WITH_ADDRESSES_ONLY
// definitions
//

// DHCPV6 Parameter Default value/description
//

// IRT, MRT, MRC, MRD values
// constants for all client messages
// order: iDelay, irt, mrt, mrc, mrd
#define TCPIP_DHCPV6_SOLICIT_IDELAY         1       // sec Max delay of first Solicit
#define TCPIP_DHCPV6_SOLICIT_IRT            1       // sec Initial Solicit timeout
#define TCPIP_DHCPV6_SOLICIT_MRT         3600       // secs Max Solicit timeout value - RFC 7083 update
#define TCPIP_DHCPV6_SOLICIT_MRC            0
#define TCPIP_DHCPV6_SOLICIT_MRD            0
#define TCPIP_DHCPV6_SOLICIT_MRT_MIN       60       // RFC 7083 _ min/max values for SOL_MAX_RT
#define TCPIP_DHCPV6_SOLICIT_MRT_MAX    86400



// Request - TCPIP_DHCPV6_MSG_TYPE_REQUEST
#define TCPIP_DHCPV6_REQUEST_IDELAY         0       // sec Initial Request timeout
#define TCPIP_DHCPV6_REQUEST_IRT            1       // sec Initial Request timeout
#define TCPIP_DHCPV6_REQUEST_MRT            30      // secs Max Request timeout value
#define TCPIP_DHCPV6_REQUEST_MRC            10      // Max Request retry attempts
#define TCPIP_DHCPV6_REQUEST_MRD            0       // MRD


// Confirm - TCPIP_DHCPV6_MSG_TYPE_CONFIRM
#define TCPIP_DHCPV6_CONFIRM_IDELAY         1       // sec Max delay of first Confirm
#define TCPIP_DHCPV6_CONFIRM_IRT            1       // sec Initial Confirm timeout
#define TCPIP_DHCPV6_CONFIRM_MRT            4       // secs Max Confirm timeout
#define TCPIP_DHCPV6_CONFIRM_MRC            0       // MRC
#define TCPIP_DHCPV6_CONFIRM_MRD            10      // secs Max Confirm duration

// Renew - TCPIP_DHCPV6_MSG_TYPE_RENEW
#define TCPIP_DHCPV6_RENEW_IDELAY           0      // 
#define TCPIP_DHCPV6_RENEW_IRT              10      // secs Initial Renew timeout
#define TCPIP_DHCPV6_RENEW_MRT              600     // secs Max Renew timeout value
#define TCPIP_DHCPV6_RENEW_MRC              0       // MRC
#define TCPIP_DHCPV6_RENEW_MRD              0       // MRD:  Remaining time until T2!


// Rebind TCPIP_DHCPV6_MSG_TYPE_REBIND
#define TCPIP_DHCPV6_REBIND_IDELAY          0      
#define TCPIP_DHCPV6_REBIND_IRT             10      // secs Initial Rebind timeout
#define TCPIP_DHCPV6_REBIND_MRT             600     // secs Max Rebind timeout value
#define TCPIP_DHCPV6_REBIND_MRC             0
#define TCPIP_DHCPV6_REBIND_MRD             0       // !secs: Remaining time until valid lifetimes of all addresses have expired!

// Info request - TCPIP_DHCPV6_MSG_TYPE_INFO_REQUEST
#define TCPIP_DHCPV6_INFO_REQ_IDELAY        1       // sec Max delay of first Information-request
#define TCPIP_DHCPV6_INFO_REQ_IRT           1       // sec Initial Information-request timeout
#define TCPIP_DHCPV6_INFO_REQ_MRT           3600    // secs Max Information-request timeout value - RFC 7083 update
#define TCPIP_DHCPV6_INFO_REQ_MRC           0    
#define TCPIP_DHCPV6_INFO_REQ_MRD           0    
#define TCPIP_DHCPV6_INFO_REQ_MRT_MIN      60    // RFC 7083 - min/max admisible values for INF_MAX_RT
#define TCPIP_DHCPV6_INFO_REQ_MRT_MAX   86400   


// Release - TCPIP_DHCPV6_MSG_TYPE_RELEASE
#define TCPIP_DHCPV6_RELEASE_IDELAY         0
#define TCPIP_DHCPV6_RELEASE_IRT            1       // sec Initial Release timeout
#define TCPIP_DHCPV6_RELEASE_MRT            0       // MAX Release attempts
#define TCPIP_DHCPV6_RELEASE_MRC            5       // MAX Release attempts
#define TCPIP_DHCPV6_RELEASE_MRD            0     

// Decline - TCPIP_DHCPV6_MSG_TYPE_DECLINE
#define TCPIP_DHCPV6_DECLINE_IDELAY         0
#define TCPIP_DHCPV6_DECLINE_IRT            1       // sec Initial Decline timeout
#define TCPIP_DHCPV6_DECLINE_MRT            0      
#define TCPIP_DHCPV6_DECLINE_MRC            5       // Max Decline attempts
#define TCPIP_DHCPV6_DECLINE_MRD            0       

#define TCPIP_DHCPV6_HOP_COUNT_LIMIT        32      // Max hop count in a Relay-forward message


// infinite timeout for DHCPV6
#define TCPIP_DHCPV6_TIMEOUT_INFINITE       0xffffffff

// minimum value accepted for the buffer size
#define TCPIP_DHCPV6_MESSAGE_MIN_BUFFER_SIZE    128


// DHCPv6 message types
// for  STATELESS DHCP see RFC 3736
typedef enum
{
    TCPIP_DHCPV6_MSG_TYPE_NONE   = 0,       // no message

    TCPIP_DHCPV6_MSG_TYPE_SOLICIT,          // (1) Client Solicit message to locate servers;
                                            // Client -> Server

    TCPIP_DHCPV6_MSG_TYPE_ADVERTISE,        // (2) Server Advertise message ;
                                            // Server -> Client
                                            // RFC 7083 update:
                                            //  the client MUST process an included SOL_MAX_RT option,
                                            //  MUST process an included INF_MAX_RT option
                                            //  even if the message contains Status Code with TCPIP_DHCPV6_SERVER_STAT_NO_ADDRS_AVAIL!
                                            //

    TCPIP_DHCPV6_MSG_TYPE_REQUEST,          // (3) Client Request message to request configuration parameters;
                                            // Client -> Server

    TCPIP_DHCPV6_MSG_TYPE_CONFIRM,          // (4) Client Confirm message to server;
                                            // Client -> Server
                                                // A client sends a Confirm message to any available server to determine whether the
                                                // addresses it was assigned are still appropriate to the link to which the client is connected.
                                                // Client uses Confirm to validate addresses when it may have moved to a new link
                                                // 
    

    TCPIP_DHCPV6_MSG_TYPE_RENEW,            // (5) Client Renew message to the server that originally provided the client’s addresses;
                                            // Client -> Server

    TCPIP_DHCPV6_MSG_TYPE_REBIND,           // (6) Client Rebind message to any available server to extend the lease lifetime;
                                            // Client -> Server

    TCPIP_DHCPV6_MSG_TYPE_REPLY,            // (7) Server Reply message containing assigned addresses and configuration parameters;
                                            // Server -> Client
                                            //  STATELESS!

    TCPIP_DHCPV6_MSG_TYPE_RELEASE,          // (8) Client Release message to the server that assigned addresses;
                                            // Client -> Server

    TCPIP_DHCPV6_MSG_TYPE_DECLINE,          // (9) Client Decline message to a server;
                                            // Client -> Server

    TCPIP_DHCPV6_MSG_TYPE_RECONFIGURE,      // (10) Server Reconfigure message to a client;
                                            // Server -> Client

    TCPIP_DHCPV6_MSG_TYPE_INFO_REQUEST,     // (11) Client Information-request message to a server;
                                            // Client -> Server
                                            //  STATELESS!

    TCPIP_DHCPV6_MSG_TYPE_RELAY_FORW,       // (12) Relay Agent Relay-forward message to servers;
                                            // Relay -> Server
                                            //  STATELESS!

    TCPIP_DHCPV6_MSG_TYPE_RELAY_REPLY,      // (13) Server Relay-reply message to a relay agent;
                                            // Server -> Relay
                                            //  STATELESS!

    //
    //
    TCPIP_DHCPV6_MSG_TYPE_NUMBER            // number of messages

}TCPIP_DHCPV6_MSG_TYPE;

// same list of DHCPv6 messages but for client side only
typedef enum
{
    TCPIP_DHCPV6_CLIENT_MSG_TYPE_SOLICIT,               // ->TCPIP_DHCPV6_MSG_TYPE_SOLICIT

    TCPIP_DHCPV6_CLIENT_MSG_TYPE_REQUEST,               // ->TCPIP_DHCPV6_MSG_TYPE_REQUEST

    TCPIP_DHCPV6_CLIENT_MSG_TYPE_CONFIRM,               // ->TCPIP_DHCPV6_MSG_TYPE_CONFIRM

    TCPIP_DHCPV6_CLIENT_MSG_TYPE_RENEW,                 // ->TCPIP_DHCPV6_MSG_TYPE_RENEW

    TCPIP_DHCPV6_CLIENT_MSG_TYPE_REBIND,                // ->TCPIP_DHCPV6_MSG_TYPE_REBIND

    TCPIP_DHCPV6_CLIENT_MSG_TYPE_RELEASE,               // ->TCPIP_DHCPV6_MSG_TYPE_RELEASE

    TCPIP_DHCPV6_CLIENT_MSG_TYPE_DECLINE,               // ->TCPIP_DHCPV6_MSG_TYPE_DECLINE

    TCPIP_DHCPV6_CLIENT_MSG_TYPE_INFO_REQUEST,          // ->TCPIP_DHCPV6_MSG_TYPE_INFO_REQUEST

    //
    TCPIP_DHCPV6_CLIENT_MSG_TYPE_NUMBER                 // number of messages

}TCPIP_DHCPV6_CLIENT_MSG_TYPE;


// list of DHCPv6 messages sent by the server side only
// that the client expects
typedef enum
{
    TCPIP_DHCPV6_SERVER_MSG_TYPE_ADVERTISE,         // -> TCPIP_DHCPV6_MSG_TYPE_ADVERTISE
    TCPIP_DHCPV6_SERVER_MSG_TYPE_REPLY,             // -> TCPIP_DHCPV6_MSG_TYPE_REPLY
    TCPIP_DHCPV6_SERVER_MSG_TYPE_RECONFIGURE,       // -> TCPIP_DHCPV6_MSG_TYPE_RECONFIGURE

    //
    TCPIP_DHCPV6_SERVER_MSG_TYPE_NUMBER                 // number of messages

}TCPIP_DHCPV6_SEVER_MSG_TYPE_TYPE;



// generic DHCPv6 message header
//
typedef struct __attribute__((packed))
{
    uint8_t     msg_type;       // a TCPIP_DHCPV6_MSG_TYPE value
    uint8_t     transId[3];     // transaction ID
    uint8_t     options[0];     // variable
}TCPIP_DHCPV6_MESSAGE_HEADER;

#define TCPIP_DHCPV6_HW_TYPE                    (1u)    //  RFC 1700 - 1 for ETH hardware type

// DUID based on Link Layer + time: DUID-LLT
typedef struct __attribute__((packed))
{
    uint16_t    duid_type;      // == TCPIP_DHCPV6_DUID_TYPE_LLT
    uint16_t    hw_type;        // hardware type: TCPIP_DHCPV6_HW_TYPE
    uint32_t    time_val;       // time the DUID is generated in seconds from midnight January 2000 % 2^32
    uint8_t     ll_address[6];  // 6 bytes MAC address for ETH
    // Remarks:
    //  - any interface may be used
    //  - the same DUID must be used for ALL interfaces!
    //  - once generated, it must be stored and re-used from then on!
    //  - MUST provide an administrative interface that replaces the existing DUID
    //    with a newly-generated DUID-LLT!
}TCPIP_DHCPV6_DUID_LLT;

// DUID Assigned by  Vendor Based Enterprise Number: DUID_EN
// we impose a max limit on such identifiers to avoid run time malloc
#define TCPIP_DHCPV6_DUID_EN_IDENTIFIER_LENGTH      16

typedef struct __attribute__((packed))
{
    uint16_t    duid_type;      // == TCPIP_DHCPV6_DUID_TYPE_EN
    uint32_t    ent_number;     // Vendor’s registered Private Enterprise Number as maintained by IANA
    uint8_t     identifier[TCPIP_DHCPV6_DUID_EN_IDENTIFIER_LENGTH];   // variable length;
                                

    // Remarks:
    //  - This form of DUID is assigned by the vendor to the device
    // - The identifier MUST be unique to the device that is using it
    // - The identifier MUST be assigned to the device at the time it is manufactured
    // - The identifier MUST be stored in some form of non-volatile storage!
}TCPIP_DHCPV6_DUID_EN;



// DUID based on Link Layer: DUID-LL
typedef struct __attribute__((packed))
{
    uint16_t    duid_type;      // == TCPIP_DHCPV6_DUID_TYPE_LL
    uint16_t    hw_type;        // hardware type: TCPIP_DHCPV6_HW_TYPE
    uint8_t     ll_address[6];  // 6 bytes MAC address for ETH
    // Remarks:
    // - The link-layer address is of any one network interface that is permanently connected
    //   to the client or server device.
    //   For example, a host that has a network interface implemented in a chip that is
    //   unlikely to be removed and used elsewhere could use a DUID-LL
    //  - The choice of network interface can be completely arbitrary
    //    as long as that interface provides a unique link-layer address
    //    and is permanently attached to the device on which the DUID-LL is being generated.
    //  - DUID-LL is recommended for devices that have a permanently-connected
    //    network interface with a link-layer address, and do not have
    //    nonvolatile, writable stable storage!
    //  - DUID-LL MUST NOT be used by DHCP clients or servers that cannot tell
    //    whether or not a network interface is permanently attached to the device
    //    on which the DHCP client is running!
    // 
}TCPIP_DHCPV6_DUID_LL;

// union of all DUID supported types
typedef union
{
    TCPIP_DHCPV6_DUID_LL    duidLL;     // TCPIP_DHCPV6_DUID_TYPE_LL
    TCPIP_DHCPV6_DUID_LLT   duidLLT;    // TCPIP_DHCPV6_DUID_TYPE_LLT
    TCPIP_DHCPV6_DUID_EN    duidEN;     // TCPIP_DHCPV6_DUID_TYPE_EN
}TCPIP_DHCPV6_DUID_UNION;



typedef struct
{
    uint16_t                duidType;   // TCPIP_DHCPV6_DUID_TYPE; current type
    uint16_t                duidLen;    // how many bytes in the current duid
    TCPIP_DHCPV6_DUID_UNION duidBody;   // one of the supported types
}TCPIP_DHCPV6_DUID_DCPT;


// DHCP options
//
// option codes
// !RFC 3315: Unless otherwise noted, each option may appear only in the options
// area of a DHCP message and may appear only once. If an option does
// appear multiple times, each instance is considered separate and the
// data areas of the options MUST NOT be concatenated or otherwise combined.
typedef enum
{
    /* Options 0 - 31 */
    // RFC 3315 - DHCPv6
    TCPIP_DHCPV6_OPT_CODE_NONE           = 0,   // invalid option/none

    TCPIP_DHCPV6_OPT_CODE_CLIENT_ID      = 1,   // TCPIP_DHCPV6_MSG_TYPE_SOLICIT | TCPIP_DHCPV6_MSG_TYPE_ADVERTISE    | TCPIP_DHCPV6_MSG_TYPE_REQUEST | TCPIP_DHCPV6_MSG_TYPE_CONFIRM |
                                                // TCPIP_DHCPV6_MSG_TYPE_RENEW   | TCPIP_DHCPV6_MSG_TYPE_REBIND       | TCPIP_DHCPV6_MSG_TYPE_DECLINE | TCPIP_DHCPV6_MSG_TYPE_RELEASE |
                                                // TCPIP_DHCPV6_MSG_TYPE_REPLY   | TCPIP_DHCPV6_MSG_TYPE_RECONFIGURE  | TCPIP_DHCPV6_MSG_TYPE_INFO_REQUEST
                                                // may be part of STATELESS DHCP (not required) 
                                                // Server, Client
                                                //
    TCPIP_DHCPV6_OPT_CODE_SERVER_ID      = 2,   // TCPIP_DHCPV6_MSG_TYPE_ADVERTISE | TCPIP_DHCPV6_MSG_TYPE_REQUEST | TCPIP_DHCPV6_MSG_TYPE_RENEW       | TCPIP_DHCPV6_MSG_TYPE_DECLINE |
                                                // TCPIP_DHCPV6_MSG_TYPE_RELEASE   | TCPIP_DHCPV6_MSG_TYPE_REPLY   | TCPIP_DHCPV6_MSG_TYPE_RECONFIGURE | TCPIP_DHCPV6_MSG_TYPE_INFO_REQUEST 
                                                // part of STATELESS DHCP! 
                                                // Server, Client
                                                //
    TCPIP_DHCPV6_OPT_CODE_IA_NA          = 3,   // TCPIP_DHCPV6_MSG_TYPE_SOLICIT | TCPIP_DHCPV6_MSG_TYPE_ADVERTISE | TCPIP_DHCPV6_MSG_TYPE_REQUEST | TCPIP_DHCPV6_MSG_TYPE_CONFIRM |
                                                // TCPIP_DHCPV6_MSG_TYPE_RENEW   | TCPIP_DHCPV6_MSG_TYPE_REBIND    | TCPIP_DHCPV6_MSG_TYPE_DECLINE | TCPIP_DHCPV6_MSG_TYPE_RELEASE |
                                                // TCPIP_DHCPV6_MSG_TYPE_REPLY 
                                                // Server, Client
                                                //
    TCPIP_DHCPV6_OPT_CODE_IA_TA          = 4,   // TCPIP_DHCPV6_MSG_TYPE_SOLICIT | TCPIP_DHCPV6_MSG_TYPE_ADVERTISE | TCPIP_DHCPV6_MSG_TYPE_REQUEST | TCPIP_DHCPV6_MSG_TYPE_CONFIRM |
                                                // TCPIP_DHCPV6_MSG_TYPE_RENEW   | TCPIP_DHCPV6_MSG_TYPE_REBIND    | TCPIP_DHCPV6_MSG_TYPE_DECLINE | TCPIP_DHCPV6_MSG_TYPE_RELEASE |
                                                // TCPIP_DHCPV6_MSG_TYPE_REPLY 
                                                // Server, Client
                                                //
    TCPIP_DHCPV6_OPT_CODE_IA_ADDR        = 5,   // TCPIP_DHCPV6_OPT_CODE_IA_NA | TCPIP_DHCPV6_OPT_CODE_IA_TA messages
                                                // Server, Client
                                                //
    TCPIP_DHCPV6_OPT_CODE_OPTION_REQ     = 6,   // TCPIP_DHCPV6_MSG_TYPE_SOLICIT |                               | TCPIP_DHCPV6_MSG_TYPE_REQUEST | TCPIP_DHCPV6_MSG_TYPE_CONFIRM |
                                                // TCPIP_DHCPV6_MSG_TYPE_RENEW   | TCPIP_DHCPV6_MSG_TYPE_REBIND       | TCPIP_DHCPV6_MSG_TYPE_DECLINE | TCPIP_DHCPV6_MSG_TYPE_RELEASE |
                                                // TCPIP_DHCPV6_MSG_TYPE_RECONFIGURE  | TCPIP_DHCPV6_MSG_TYPE_INFO_REQUEST
                                                // part of STATELESS DHCP! 
                                                // Client
                                                // Server (as part of TCPIP_DHCPV6_MSG_TYPE_RECONFIGURE)
                                                //
    TCPIP_DHCPV6_OPT_CODE_PREFERENCE     = 7,   // TCPIP_DHCPV6_MSG_TYPE_ADVERTISE | TCPIP_DHCPV6_MSG_TYPE_REPLY
                                                // Server
                                                // may be part of STATELESS DHCP! 
                                                //
    TCPIP_DHCPV6_OPT_CODE_ELAPSED_TIME   = 8,   // TCPIP_DHCPV6_MSG_TYPE_SOLICIT |                               | TCPIP_DHCPV6_MSG_TYPE_REQUEST | TCPIP_DHCPV6_MSG_TYPE_CONFIRM |
                                                // TCPIP_DHCPV6_MSG_TYPE_RENEW   | TCPIP_DHCPV6_MSG_TYPE_REBIND       | TCPIP_DHCPV6_MSG_TYPE_DECLINE | TCPIP_DHCPV6_MSG_TYPE_RELEASE |
                                                // TCPIP_DHCPV6_MSG_TYPE_INFO_REQUEST
                                                // Client
                                                // may be part of STATELESS DHCP! 
                                                //
    TCPIP_DHCPV6_OPT_CODE_RELAY_MSG      = 9,   // OPTION_NOT_SUPPORTED!
                                                // TCPIP_DHCPV6_MSG_TYPE_RELAY_FORW | TCPIP_DHCPV6_MSG_TYPE_RELAY_REPLY
                                                // Relay, Server
                                                //
    TCPIP_DHCPV6_OPT_CODE_INVALID        = 10,  // unassigned

    TCPIP_DHCPV6_OPT_CODE_AUTHENTICATE   = 11,  // OPTION_NOT_SUPPORTED!
                                                // TCPIP_DHCPV6_MSG_TYPE_SOLICIT | TCPIP_DHCPV6_MSG_TYPE_ADVERTISE    | TCPIP_DHCPV6_MSG_TYPE_REQUEST | TCPIP_DHCPV6_MSG_TYPE_CONFIRM |
                                                // TCPIP_DHCPV6_MSG_TYPE_RENEW   | TCPIP_DHCPV6_MSG_TYPE_REBIND       | TCPIP_DHCPV6_MSG_TYPE_REPLY   | TCPIP_DHCPV6_MSG_TYPE_RELEASE |
                                                // TCPIP_DHCPV6_MSG_TYPE_DECLINE | TCPIP_DHCPV6_MSG_TYPE_RECONFIGURE  | TCPIP_DHCPV6_MSG_TYPE_INFO_REQUEST |
                                                // TCPIP_DHCPV6_MSG_TYPE_RELAY_FORW | TCPIP_DHCPV6_MSG_TYPE_RELAY_REPLY
                                                // Server, Client
                                                // may be part of STATELESS DHCP! 
                                                //  
    
                                                //
    TCPIP_DHCPV6_OPT_CODE_UNICAST        = 12,  // TCPIP_DHCPV6_MSG_TYPE_REPLY
                                                // Server
                                                //
    TCPIP_DHCPV6_OPT_CODE_STATUS_CODE    = 13,  // TCPIP_DHCPV6_MSG_TYPE_ADVERTISE | TCPIP_DHCPV6_MSG_TYPE_REPLY 
                                                // Server
                                                // part of STATELESS DHCP! 
                                                //
    TCPIP_DHCPV6_OPT_CODE_RAPID_COMMIT   = 14,  // TCPIP_DHCPV6_MSG_TYPE_SOLICIT | TCPIP_DHCPV6_MSG_TYPE_REPLY
                                                // Server, Client
                                                //
    TCPIP_DHCPV6_OPT_CODE_USER_CLASS     = 15,  // OPTION_NOT_SUPPORTED!
                                                // TCPIP_DHCPV6_MSG_TYPE_SOLICIT | TCPIP_DHCPV6_MSG_TYPE_ADVERTISE    | TCPIP_DHCPV6_MSG_TYPE_REQUEST | TCPIP_DHCPV6_MSG_TYPE_CONFIRM |
                                                // TCPIP_DHCPV6_MSG_TYPE_RENEW   | TCPIP_DHCPV6_MSG_TYPE_REBIND       | TCPIP_DHCPV6_MSG_TYPE_REPLY   | TCPIP_DHCPV6_MSG_TYPE_RELEASE |
                                                // TCPIP_DHCPV6_MSG_TYPE_DECLINE |                               | TCPIP_DHCPV6_MSG_TYPE_INFO_REQUEST |
                                                // TCPIP_DHCPV6_MSG_TYPE_RELAY_FORW | TCPIP_DHCPV6_MSG_TYPE_RELAY_REPLY
                                                // Client, Relay
                                                // may be part of STATELESS DHCP! 
                                                //
    TCPIP_DHCPV6_OPT_CODE_VENDOR_CLASS   = 16,  // OPTION_NOT_SUPPORTED!
                                                // TCPIP_DHCPV6_MSG_TYPE_SOLICIT | TCPIP_DHCPV6_MSG_TYPE_ADVERTISE    | TCPIP_DHCPV6_MSG_TYPE_REQUEST | TCPIP_DHCPV6_MSG_TYPE_CONFIRM |
                                                // TCPIP_DHCPV6_MSG_TYPE_RENEW   | TCPIP_DHCPV6_MSG_TYPE_REBIND       | TCPIP_DHCPV6_MSG_TYPE_REPLY   | TCPIP_DHCPV6_MSG_TYPE_RELEASE |
                                                // TCPIP_DHCPV6_MSG_TYPE_DECLINE |                               | TCPIP_DHCPV6_MSG_TYPE_INFO_REQUEST |
                                                // TCPIP_DHCPV6_MSG_TYPE_RELAY_FORW | TCPIP_DHCPV6_MSG_TYPE_RELAY_REPLY
                                                // Client, Relay
                                                // may be part of STATELESS DHCP! 
                                                //
    TCPIP_DHCPV6_OPT_CODE_VENDOR_OPTS    = 17,  // OPTION_NOT_SUPPORTED!
                                                // TCPIP_DHCPV6_MSG_TYPE_SOLICIT | TCPIP_DHCPV6_MSG_TYPE_ADVERTISE    | TCPIP_DHCPV6_MSG_TYPE_REQUEST | TCPIP_DHCPV6_MSG_TYPE_CONFIRM |
                                                // TCPIP_DHCPV6_MSG_TYPE_RENEW   | TCPIP_DHCPV6_MSG_TYPE_REBIND       | TCPIP_DHCPV6_MSG_TYPE_REPLY   | TCPIP_DHCPV6_MSG_TYPE_RELEASE |
                                                // TCPIP_DHCPV6_MSG_TYPE_DECLINE |                               | TCPIP_DHCPV6_MSG_TYPE_INFO_REQUEST |
                                                // TCPIP_DHCPV6_MSG_TYPE_RELAY_FORW | TCPIP_DHCPV6_MSG_TYPE_RELAY_REPLY
                                                // Client, Relay
                                                // may be part of STATELESS DHCP! 
                                                //
    TCPIP_DHCPV6_OPT_CODE_INTERFACE_ID   = 18,  // OPTION_NOT_SUPPORTED!
                                                // TCPIP_DHCPV6_MSG_TYPE_RELAY_FORW | TCPIP_DHCPV6_MSG_RELAY_REPL
                                                // Server, relay
                                                // part of STATELESS DHCP for a relay!    
                                                //
    TCPIP_DHCPV6_OPT_CODE_RECONF_MSG     = 19,  // TCPIP_DHCPV6_MSG_TYPE_RECONFIGURE
                                                // Server
                                                // 
    TCPIP_DHCPV6_OPT_CODE_RECONF_ACCEPT  = 20,  // OPTION_NOT_SUPPORTED! (reconfigure messages require authentication)
                                                // TCPIP_DHCPV6_MSG_TYPE_SOLICIT | TCPIP_DHCPV6_MSG_TYPE_ADVERTISE    | TCPIP_DHCPV6_MSG_TYPE_REQUEST |
                                                // TCPIP_DHCPV6_MSG_TYPE_RENEW   | TCPIP_DHCPV6_MSG_TYPE_REBIND       | 
                                                // TCPIP_DHCPV6_MSG_TYPE_REPLY   | TCPIP_DHCPV6_MSG_TYPE_INFO_REQUEST
                                                // Server, Client
                                                //
                                                //
    TCPIP_DHCPV6_OPT_CODES_SUPPORTED,           // number of basic DHCPv6 options supported 

    // Extended options

    // RFC 3319 - DHCPv6 Options for Session Initiation Protocol (SIP) Servers
    TCPIP_DHCPV6_OPT_CODE_SIP_SERVER_D          = 21,       // SIP Servers Domain Name List                   
    TCPIP_DHCPV6_OPT_CODE_SIP_SERVER_A          = 22,       // SIP Servers IPv6 Address List
                                                            // part of STATELESS DHCP! 
    


    // RFC 3646 - DNS Configuration Options for DHCPv6
    TCPIP_DHCPV6_OPT_CODE_DNS_SERVERS           = 23,       // DNS Recursive Name Server
                                                            // TCPIP_DHCPV6_MSG_TYPE_SOLICIT | TCPIP_DHCPV6_MSG_TYPE_ADVERTISE    | TCPIP_DHCPV6_MSG_TYPE_REQUEST |
                                                            // TCPIP_DHCPV6_MSG_TYPE_RENEW   | TCPIP_DHCPV6_MSG_TYPE_REBIND       | 
                                                            // TCPIP_DHCPV6_MSG_TYPE_REPLY   | TCPIP_DHCPV6_MSG_TYPE_INFO_REQUEST
                                                            // part of STATELESS DHCP! 
                                                            //
                                                            //
    TCPIP_DHCPV6_OPT_CODE_DOMAIN_LIST           = 24,       // Domain Search List
                                                            // TCPIP_DHCPV6_MSG_TYPE_SOLICIT | TCPIP_DHCPV6_MSG_TYPE_ADVERTISE    | TCPIP_DHCPV6_MSG_TYPE_REQUEST |
                                                            // TCPIP_DHCPV6_MSG_TYPE_RENEW   | TCPIP_DHCPV6_MSG_TYPE_REBIND       | 
                                                            // TCPIP_DHCPV6_MSG_TYPE_REPLY   | TCPIP_DHCPV6_MSG_TYPE_INFO_REQUEST
                                                            // part of STATELESS DHCP! 
                                                            //
    // RFC 3633 - IPv6 Prefix Options for DHCPv6
    TCPIP_DHCPV6_OPT_CODE_IA_PD                 = 25,       // Identity Association for Prefix Delegation Option
    TCPIP_DHCPV6_OPT_CODE_IAPREFIX              = 26,       // IPv6 address prefixes associated with an IA_PD

    // RFC 3898 - 

    TCPIP_DHCPV6_OPT_CODE_NIS_SERVERS           = 27,       //
    TCPIP_DHCPV6_OPT_CODE_NISP_SERVERS          = 28,       //
    TCPIP_DHCPV6_OPT_CODE_NIS_DOMAIN_NAME       = 29,       //
    TCPIP_DHCPV6_OPT_CODE_NISP_DOMAIN_NAME      = 30,       //

    // RFC 4075 - 
    TCPIP_DHCPV6_OPT_CODE_SNTP_SERVER_LIST      = 31,       //

    /* Options 32 - 63 */

    // RFC 4242
    TCPIP_DHCPV6_OPT_CODE_REFRESH_TIME          = 32,       // Information Refresh Time

    // RFC 4280 - 
    TCPIP_DHCPV6_OPT_CODE_BCMS_CONTROLLER_DOMAIN_LIST  = 33,   // BCMCS Controller Domain Name list.
    TCPIP_DHCPV6_OPT_CODE_BMCS_CONTROLLER_IPV6_ADDR    = 34,   // BCMCS Controller IPv6 address list.

    // 35 ?

    // RFC 4776
    TCPIP_DHCPV6_OPT_CODE_GEOCONF_CIVIC         = 36,       // 

    // RFC 4649
    TCPIP_DHCPV6_OPT_CODE_REMOTE_ID             = 37,       // 	

    // RFC 4580
    TCPIP_DHCPV6_OPT_CODE_RELAY_AGENT_SUBSCRIBER_ID = 38,   // Relay Agent Subscriber-ID.	

    // RFC 4704
    TCPIP_DHCPV6_OPT_CODE_FQDN                  = 39,       // FQDN, Fully Qualified Domain Name.	

    // RFC 5192
    TCPIP_DHCPV6_OPT_CODE_PANA_AUTH_AGENT       = 40,       // PANA Authentication Agent.	


    // 	RFC 4833 - 
    TCPIP_DHCPV6_OPT_CODE_NEW_POSIX_TIMEZONE   = 41,        // 
    TCPIP_DHCPV6_OPT_CODE_NEW_TZDB_TIMEZONE    = 42,        //

    // RFC 4994
    TCPIP_DHCPV6_OPT_CODE_ECHO_REQUEST          = 43,       // Echo Request

    // RFC 5007 - 
    TCPIP_DHCPV6_OPT_CODE_LQ_QUERY              = 44,       // 
    TCPIP_DHCPV6_OPT_CODE_CLIENT_DATA           = 45,       // 
    TCPIP_DHCPV6_OPT_CODE_CLT_TIME              = 46,       // 
    TCPIP_DHCPV6_OPT_CODE_LQ_RELAY_DATA         = 47,       // 
    TCPIP_DHCPV6_OPT_CODE_LQ_CLIENT_LINK        = 48,       //


    // RFC 6610
    TCPIP_DHCPV6_OPT_CODE_MIPV6_HOME_NETWORK_ID                 = 49,   // MIPv6 Home Network ID FQDN.	
    TCPIP_DHCPV6_OPT_CODE_MIPV6_HOME_NETWORK_INFO  = 50,                // MIPv6 Visited Home Network Information

    /* Options 64 - 95 */
    // RFC 6610
    TCPIP_DHCPV6_OPT_CODE_MIPV6_IDENTIFIED_HOME_NETWORK_INFO    = 69,   // MIPv6 Identified Home Network Information
    TCPIP_DHCPV6_OPT_CODE_MIPV6_UNRESTRICTED_HOME_NETWORK_INFO  = 70,   // MIPv6 Unrestricted Home Network Information.
    TCPIP_DHCPV6_OPT_CODE_MIPV6_HOME_NETWORK_PREFIX             = 71,   // MIPv6 Home Network Prefix
    TCPIP_DHCPV6_OPT_CODE_MIPV6_HOME_AGENT_ADDRESS              = 72,   // MIPv6 Home Agent Address
    TCPIP_DHCPV6_OPT_CODE_MIPV6_HOME_AGENT_FQDN                 = 73,   // MIPv6 Home Agent FQDN

    // RFC 5223
    TCPIP_DHCPV6_OPT_CODE_LOST_SERVER           = 51,       // LoST Server.	

    // 	RFC 5417
    TCPIP_DHCPV6_OPT_CODE_CAPWAP_ACCESS_CONTROLLER_ADDRESSES    = 52,   // CAPWAP Access Controller addresses.


    // RFC 5460
    TCPIP_DHCPV6_OPT_CODE_RELAY_ID              = 53,       // 	

    // RFC 5678
    TCPIP_DHCPV6_OPT_CODE_IPV6_ADDRESS_MOS      = 54,       // 	
    TCPIP_DHCPV6_OPT_CODE_IPV6_FQDN_MOS         = 55,       //

    // RFC 5908
    TCPIP_DHCPV6_OPT_CODE_NTP_SERVER            = 56,       // 

    // RFC 5986
    TCPIP_DHCPV6_OPT_CODE_V6_ACCESS_DOMAIN      = 57,       // 	

    // RFC 6011
    TCPIP_DHCPV6_OPT_CODE_SIP_UA_CS_LIST        = 58,       // 	

    // 	RFC 5970

    TCPIP_DHCPV6_OPT_CODE_BOOTFILE_URL          = 59,       // 
    TCPIP_DHCPV6_OPT_CODE_BOOTFILE_PARAM        = 60,       // 
    TCPIP_DHCPV6_OPT_CODE_CLIENT_ARCH_TYPE      = 61,       //
    TCPIP_DHCPV6_OPT_CODE_NII                   = 62,       // 

    // RFC 6225
    TCPIP_DHCPV6_OPT_CODE_GEOLOCATION           = 63,       // 	

    // RFC 6334
    TCPIP_DHCPV6_OPT_CODE_AFTR_NAME             = 64,       // 	

    // RFC 6440
    TCPIP_DHCPV6_OPT_CODE_ERP_LOCAL_DOMAIN_NAME = 65,       // 

    // RFC 6422
    TCPIP_DHCPV6_OPT_CODE_RSOO                  = 66,       // 	

    // 	RFC 6603
    TCPIP_DHCPV6_OPT_CODE_PD_EXCLUDE            = 67,       // 

    // RFC 6607
    TCPIP_DHCPV6_OPT_CODE_VIRTUAL_SUBNET_SELECTION = 68,    // 	

    // RFC 7083
    TCPIP_DHCPV6_OPT_CODE_MAX_RT                = 82,       // server->client; override the TCPIP_DHCPV6_SOLICIT_MRT (SOL_MAX_RT) value
    TCPIP_DHCPV6_OPT_CODE_INFO_MAX_RT           = 83,       // server->client; override the TCPIP_DHCPV6_INFO_REQ_MRT (INF_MAX_RT) value

    //
    TCPIP_DHCPV6_OPT_CODE_NUMBER                // total number of supported option codes

}TCPIP_DHCPV6_OPTION_CODE;



// generic option format
typedef struct __attribute__((packed))
{
    uint16_t        optCode;    // identifying the specific option type carried in this option
    uint16_t        optLen;     // length of the option-data field in this option in octets
    uint32_t        optData[0]; // The data for the option; the format of this data depends on the definition of the option
}TCPIP_DHCPV6_OPTION_GENERIC;


// Client ID option format
typedef struct __attribute__((packed))
{
    uint16_t                optCode;    // = TCPIP_DHCPV6_OPT_CODE_CLIENT_ID
    uint16_t                optLen;     // = length of the DUID
    uint16_t                duid[0];    // DUID_LL, DUID_LLT or DUID_EN
}TCPIP_DHCPV6_OPTION_CLIENT_ID;

// Server ID option format
typedef struct __attribute__((packed))
{
    uint16_t                optCode;    // = TCPIP_DHCPV6_OPT_CODE_SERVER_ID
    uint16_t                optLen;     // = length of the server DUID
    uint16_t                duid[0];    //  DUID_LL, DUID_LLT or DUID_EN
}TCPIP_DHCPV6_OPTION_SERVER_ID;

// Client/Server ID option format
typedef struct __attribute__((packed))
{
    uint16_t                optCode;    // = TCPIP_DHCPV6_OPT_CODE_CLIENT_ID/ TCPIP_DHCPV6_OPT_CODE_SERVER_ID
    uint16_t                optLen;     // = length of the client/server DUID
    uint16_t                duid[0];    //  DUID_LL, DUID_LLT or DUID_EN
}TCPIP_DHCPV6_OPTION_ID;


// IA_NA option format

typedef struct __attribute__((packed))
{
    uint32_t                iaid;       // The unique identifier for this IA_NA;
                                        // the IAID must be unique among the identifiers for all of this client’s IA_NAs.
                                        // The number space for IA_NA IAIDs is separate from the number space for IA_TA IAIDs.
                                        // value == 0 is used to mark IAs as invalid!
    uint32_t                t1;         // The time at which the client contacts the server from which the addresses in the IA_NA
                                        // were obtained to extend the lifetimes of the addresses assigned to the IA_NA;
                                        // T1 is a time duration relative to the current time expressed in units of seconds.
    uint32_t                t2;         // The time at which the client contacts any available server to extend the lifetimes of
                                        // the addresses assigned to the IA_NA;
                                        // T2 is a time duration relative to the current time expressed in units of seconds.
                                        //
                                        // Cli->server: client sets T1 and T2 to 0 if it has no preference for those values
                                        // In a message sent by a server to a client, the client MUST use the values in the T1 and T2 fields
                                        // for the T1 and T2 parameters, unless those values in those fields are 0.
                                        // The values in the T1 and T2 fields are the number of seconds until T1 and T2.
                                        // A client will never attempt to extend the lifetimes of any addresses in an IA with T1 set to 0xffffffff!
                                        // A client will never attempt to use a Rebind message to locate a different server to extend the
                                        // lifetimes of any addresses in an IA with T2 set to 0xffffffff.!
                                        // 
    // A DHCP message may contain multiple IA_NA options!
    // More than one IA Address Option can appear in an IA_NA
    // option or an IA_TA option.!
    //
}TCPIP_DHCPV6_OPTION_IANA_BODY;




typedef struct __attribute__((packed))
{
    uint16_t                        optCode;    // = TCPIP_DHCPV6_OPT_CODE_IA_NA
    uint16_t                        optLen;     // =12 + length of IA_NA-options field
    TCPIP_DHCPV6_OPTION_IANA_BODY   body;
    uint8_t                         ianaOpt[0]; // Options associated with this IA_NA.

}TCPIP_DHCPV6_OPTION_IANA;

typedef struct __attribute__((packed))
{
    uint32_t    iaid;       // The unique identifier for this IA_TA;
                            // the IAID must be unique among the identifiers for all of this client’s IA_NAs.
                            // The number space for IA_NA IAIDs is separate from the number space for IA_TA IAIDs.
                            // value == 0 is used to mark IAs as invalid!
                            //
                            // An IA_TA option does not include values for T1 and T2.!
                            //
                            // DHCP message may contain multiple IA_TA options!
                            // More than one IA Address Option can appear in an IA_NA
                            // option or an IA_TA option.!

}TCPIP_DHCPV6_OPTION_IATA_BODY;

// IA_TA option format
typedef struct __attribute__((packed))
{
    uint16_t                        optCode;    // = TCPIP_DHCPV6_OPT_CODE_IA_TA
    uint16_t                        optLen;     // = 4 + length of IA_TA-options field
                                        //
    TCPIP_DHCPV6_OPTION_IATA_BODY   body;                                        
    uint8_t                         iataOpt[0]; // Options associated with this IA_TA.

}TCPIP_DHCPV6_OPTION_IATA;


// IA address option - specify IPv6 addresses associated with an IA_NA or an IA_TA.
// The IA Address option must be encapsulated in the Options field of an IA_NA or IA_TA option.!
// An IA Address option may appear only in an IA_NA option or an IA_TA option.
// More than one IA Address Option can appear in an IA_NA option or an IA_TA option.
// 

typedef struct
{
    IPV6_ADDR           ipv6Addr;       // 16 bytes IPV6 address
    uint32_t            prefLTime;      // preferred life time for the IPv6 address; seconds
    uint32_t            validLTime;     // valid life time for the IPv6 address; seconds
                                        // cli->server: The client may send 0 if it has no preference for the
                                        // preferred and valid lifetimes
                                        // server->client: the client MUST use the values in the preferred and valid
                                        // lifetime fields for the preferred and valid lifetimes
                                        // 
}TCPIP_DHCPV6_OPTION_IA_ADDR_BODY;



typedef struct __attribute__((packed))
{
    uint16_t                            optCode;        // = TCPIP_DHCPV6_OPT_CODE_IA_ADDR
    uint16_t                            optLen;         // 24 + length of IAaddr-options field
    TCPIP_DHCPV6_OPTION_IA_ADDR_BODY    body;
    uint8_t                             iaaddrOpt[0];   // options associated with this address
}TCPIP_DHCPV6_OPTION_IA_ADDR;

// Option Request option is used to identify a list of options in a
// message between a client and a server
// A client MAY include an Option Request option in a:
//      Solicit, Request, Renew, Rebind, Confirm or Information-request message
//      to inform the server about options the client wants the server to send to the client.
// A server MAY include an Option Request option in a Reconfigure option
// to indicate which options the client should request from the server.
// NEXT_HOP and RT_PREFIX Options found - https://tools.ietf.org/html/draft-ietf-mif-dhcpv6-route-option-05
//
typedef struct __attribute__((packed))
{
    uint16_t            optCode;        // = TCPIP_DHCPV6_OPT_CODE_OPTION_REQ
    uint16_t            optLen;         // 2 * number of requested options.
    uint16_t            reqCode[0];     // option codes for options requested by the client 

}TCPIP_DHCPV6_OPTION_OPTION_REQ;

// Preference option
// The Preference option is sent by a server to a client to affect the
// selection of a server by the client
// A server MAY include a Preference option in an Advertise message to
// control the selection of a server by the client
typedef struct __attribute__((packed))
{
    uint16_t            optCode;        // = TCPIP_DHCPV6_OPT_CODE_PREFERENCE
    uint16_t            optLen;         // 1
    uint8_t             prefValue;      // The preference value for the server in this message

}TCPIP_DHCPV6_OPTION_PREFERENCE;


// Elapsed time Option
// A client MUST include an Elapsed Time option in messages to indicate how long
// the client has been trying to complete a DHCP message exchange.
//
typedef struct __attribute__((packed))
{
    uint16_t            optCode;        // = TCPIP_DHCPV6_OPT_CODE_ELAPSED_TIME
    uint16_t            optLen;         // 2
    uint16_t            elapsedTime;    // The amount of time since the client began its current DHCP transaction.
                                        // This time is expressed in hundredths of a second (10^-2 seconds).
                                        // set to 0 in the 1st message of the exchange;
                                        // set to 0xffff to represent any value greater than it can be represented in this option
                                        //

}TCPIP_DHCPV6_OPTION_ELAPSED_TIME;

// Relay Message Option
typedef struct __attribute__((packed))
{
    uint16_t            optCode;        // = TCPIP_DHCPV6_OPT_CODE_RELAY_MSG
    uint16_t            optLen;         // length of the DHCP relay message
    uint8_t             relayMsg[0];    // The amount of time since the client began its current DHCP transaction.

}TCPIP_DHCPV6_OPTION_RELAY_MSG;



// Authentication Option
typedef struct __attribute__((packed))
{
    uint16_t            optCode;        // = TCPIP_DHCPV6_OPT_CODE_AUTHENTICATE
    uint16_t            optLen;         // 11 + length of authentication information field
    uint8_t             protocol;
    uint8_t             algorithm;
    uint8_t             rdm;            // replay detection method used
    uint64_t            replayDetect;   // replay detection information for the RDM
    uint8_t             authInfo[0];    // authentication information, as specified by the protocol and algorithm
}TCPIP_DHCPV6_OPTION_AUTHENTICATE;



// Unicast option
// The server specifies the IPv6 address to which the client is to send
// unicast messages in the server-address field.
// When a client receives this option, where permissible and appropriate,
// the client sends messages directly to the server using the IPv6 address
// specified in the server-address field of the option.
// 
typedef struct __attribute__((packed))
{
    uint16_t            optCode;        // = TCPIP_DHCPV6_OPT_CODE_UNICAST
    uint16_t            optLen;         // 16
    IPV6_ADDR           serverAddr;     // 16 bytes IPV6 server address
}TCPIP_DHCPV6_OPTION_UNICAST;


// Status Code Option
// This option returns a status indication related to the DHCP message
// or option in which it appears
typedef struct __attribute__((packed))
{
    uint16_t            optCode;        // = TCPIP_DHCPV6_OPT_CODE_STATUS_CODE
    uint16_t            optLen;         // 2 + length of status-message
    uint16_t            statusCode;     // numeric code for the status encoded in this option: TCPIP_DHCPV6_SERVER_STATUS_CODE
    uint8_t             statusMsg[0];   // UTF-8 encoded text string suitable for display to an end user
                                        // which MUST NOT be null-terminated.
    
}TCPIP_DHCPV6_OPTION_STATUS_CODE;


// Rapid Commit
// Is used to signal the use of the two message exchange for address assignment
// A client MAY include this option in a Solicit message if the client
// is prepared to perform the Solicit-Reply message exchange
// KAME: "The use of the Rapid Commit option primarily assumes an environment where at most one
// server is available for the client (e.g., where the client is directly connected to a server over a
// point-to-point link). Although the protocol does not necessarily prohibit the use of this option
// with multiple servers, it would cause problems as discussed in [RFC3315] and is thus inadvisable."
//  
typedef struct __attribute__((packed))
{
    uint16_t            optCode;        // = TCPIP_DHCPV6_OPT_CODE_RAPID_COMMIT
    uint16_t            optLen;         // 0
}TCPIP_DHCPV6_OPTION_RAPID_COMMIT;



// User Class Option
// The User Class option is used by a client to identify the type or category of user or applications it represents.

typedef struct __attribute__((packed))
{
    uint16_t            userClassLen;   // = length of the opaque user class data
    uint8_t             opaqueData[0];  // The user classes carried by the client

}TCPIP_DHCPV6_USER_CLASS_DATA;

typedef struct __attribute__((packed))
{
    uint16_t                        optCode;            // = TCPIP_DHCPV6_OPT_CODE_USER_CLASS
    uint16_t                        optLen;             // Length of user class data field
    TCPIP_DHCPV6_USER_CLASS_DATA    userClassData[1];   // The user classes carried by the client
                                                        // MUST contain one or more instances of user class data.
}TCPIP_DHCPV6_OPTION_USER_CLASS;

// Vendor Class Option
// This option is used by a client to identify the vendor that
// manufactured the hardware on which the client is running

typedef struct __attribute__((packed))
{
    uint16_t            vendorClassLen;     // = length of the opaque vendor class data
    uint8_t             opaqueData[0];      // The vendor classes carried by the client

}TCPIP_DHCPV6_VENDOR_CLASS_DATA;


typedef struct __attribute__((packed))
{
    uint16_t                        optCode;            // = TCPIP_DHCPV6_OPT_CODE_VENDOR_CLASS
    uint16_t                        optLen;             // 4 + length of vendor class data field
    uint32_t                        enterpriseNo;       // The vendor’s registered Enterprise Number as registered with IANA
    TCPIP_DHCPV6_VENDOR_CLASS_DATA  vendorClassData[0]; // The hardware configuration of the host on which the client is running

}TCPIP_DHCPV6_OPTION_VENDOR_CLASS;


// Vendor-specific Information Option
// This option is used by clients and servers to exchange vendor-specific information.
// Multiple instances of the Vendor-specific Information option may appear in a DHCP message.
// Each instance of the option is interpreted according to the option codes defined by the vendor identified by the
// Enterprise Number in that option.

typedef struct __attribute__((packed))
{
    uint16_t            optCode;        // The code for the encapsulated option
    uint16_t            optLen;         // An unsigned integer giving the length of the option-data
    uint8_t             optionData[0];  // The data area for the encapsulated option

}TCPIP_DHCPV6_VENDOR_OPTION_DATA;


typedef struct __attribute__((packed))
{
    uint16_t                        optCode;            // = TCPIP_DHCPV6_OPT_CODE_VENDOR_OPTS
    uint16_t                        optLen;             // 4 + length of option-data field
    uint32_t                        enterpriseNo;       // The vendor’s registered Enterprise Number as registered with IANA
    TCPIP_DHCPV6_VENDOR_OPTION_DATA vendorOptData[0];   // The hardware configuration of the host on which the client is running

}TCPIP_DHCPV6_OPTION_VENDOR_OPTS;


// Interface-Id Option
// This option MUST NOT appear in any message except a Relay-Forward or Relay-Reply message
typedef struct __attribute__((packed))
{
    uint16_t                        optCode;            // = TCPIP_DHCPV6_OPT_CODE_INTERFACE_ID
    uint16_t                        optLen;             // Length of interface-id field
    uint8_t                         interfaceId[0];     // An opaque value of arbitrary length generated by the relay agent to identify one of the
                                                        // relay agent’s interfaces.
}TCPIP_DHCPV6_OPTION_INTERFACE_ID;


// Reconfigure Message Option
// A server includes a Reconfigure Message option in a Reconfigure message
// to indicate to the client whether the client responds with a
// Renew message or an Information-request message
// The Reconfigure Message option can only appear in a Reconfigure message!
// Because of the opportunity for attack through the Reconfigure message,
// a DHCP client MUST discard any Reconfigure message that does not include authentication
// or that does not pass the validation process for the authentication protocol!
// STATELESS!

typedef struct __attribute__((packed))
{
    uint16_t                        optCode;    // = TCPIP_DHCPV6_OPT_CODE_RECONF_MSG
    uint16_t                        optLen;     // 1
    uint8_t                         msgType;    // TCPIP_DHCPV6_MSG_TYPE_RENEW / TCPIP_DHCPV6_MSG_TYPE_INFO_REQUEST 
}TCPIP_DHCPV6_OPTION_RECONF_MSG;


// Reconfigure Accept Option
// A client uses the Reconfigure Accept option to announce to the server whether the client is willing to accept Reconfigure messages
// A server uses this option to tell the client whether or not to accept Reconfigure messages.

typedef struct __attribute__((packed))
{
    uint16_t                        optCode;    // = TCPIP_DHCPV6_OPT_CODE_RECONF_ACCEPT
    uint16_t                        optLen;     // 0
}TCPIP_DHCPV6_OPTION_RECONF_ACCEPT;



// DNS Recursive Name Servers option
// part of STATELESS DHCP!
// Provides a list of one or more IPv6 addresses of DNS recursive name servers
// to which a client’s DNS resolver MAY send DNS queries
// The DNS servers are listed in the order of preference for use by the client resolver
//
typedef struct __attribute__((packed))
{
    uint16_t                        optCode;    // = TCPIP_DHCPV6_OPT_CODE_DNS_SERVERS
    uint16_t                        optLen;     // Length of the list of DNS recursive name servers in octets;
                                                // must be a multiple of 16
                                                //
    IPV6_ADDR                       dnsAddr[0]; // 16 bytes DNS server IPV6 address
    
}TCPIP_DHCPV6_OPTION_DNS_SERVERS;

// Domain Search List option
// part of STATELESS DHCP!
// Specifies the domain search list the client is to use when resolving hostnames with DNS.
// This option does not apply to other name resolution mechanisms.
// 
typedef struct __attribute__((packed))
{
    uint16_t                        optCode;        // = TCPIP_DHCPV6_OPT_CODE_DOMAIN_LIST
    uint16_t                        optLen;         // Length of the ’searchlist’ field in octets;
                                                    //
    uint8_t                         searchList[0];  // The specification of the list of domain names in the Domain Search List
                                                    // The list of domain names in the ’searchlist’ MUST be encoded as specified in section
                                                    // "Representation and use of domain names" of RFC 3315:
                                                    //      A domain name or a list of domain names is encoded using the technique described in
                                                    //      section 3.1 of RFC 1035 [10].
                                                    //      A domain name, or list of domain names, in DHCP MUST NOT be stored in compressed form,
                                                    //      as described in section 4.1.4 of RFC 1035.
                                                    // 
}TCPIP_DHCPV6_OPTION_DOMAIN_SEARCH_LIST;



// SOL_MAX_RT option - RFC 7083
// A DHCPv6 server sends the SOL_MAX_RT option to a client to override
// the default value of TCPIP_DHCPV6_SOLICIT_MRT (SOL_MAX_RT)
// 
typedef struct __attribute__((packed))
{
    uint16_t                        optCode;        // = TCPIP_DHCPV6_OPT_CODE_MAX_RT
    uint16_t                        optLen;         // 4
    uint32_t                        solMaxRt;       // SOL_MAX_RT value:
                                                    // Overriding value for TCPIP_DHCPV6_SOLICIT_MRT (SOL_MAX_RT)
                                                    // in seconds;
                                                    // MUST be in range: 60 <= "value" <= 86400 (1 day)
                                                    // else silently ignored
}TCPIP_DHCPV6_OPTION_MAX_RT;

// INF_MAX_RT Option - RFC 7083
// A server sends the INF_MAX_RT option to a client to override
// the default value of TCPIP_DHCPV6_INFO_REQ_MRT (INF_MAX_RT).
// 
typedef struct __attribute__((packed))
{
    uint16_t                        optCode;        // = TCPIP_DHCPV6_OPT_CODE_INFO_MAX_RT
    uint16_t                        optLen;         // 4
    uint32_t                        infoMaxRt;      // INF_MAX_RT value:
                                                    // Overriding value for TCPIP_DHCPV6_INFO_REQ_MRT (INF_MAX_RT)
                                                    // in seconds;
                                                    // MUST be in range: 60 <= "value" <= 86400 (1 day)
                                                    // else silently ignored
}TCPIP_DHCPV6_OPTION_INFO_MAX_RT;



/*****************************************************************************************************/

// low level data structures
//
//
// message transmission bounds: retries, timeouts, etc.
typedef struct
{
    uint32_t    iDelay; // initial delay; seconds
    uint32_t    irt;    // initial retransmisson time; seconds
    uint32_t    mrt;    // upper bound for the RT; seconds
    uint32_t    mrc;    // max retransmisson count; if 0, no count used
    uint32_t    mrd;    // upper bound for total retransmission time; seconds; if 0, no upper bound enforced;
}TCPIP_DHCPV6_MSG_TRANSMIT_BOUNDS;


// descriptor of a message to be transmitted
typedef struct
{
    TCPIP_DHCPV6_MSG_TRANSMIT_BOUNDS    bounds;     // message bounds
    // current values
    uint32_t    rc;     // current retransmission count: 0 ->mrc; initialize to 0!
    uint32_t    iTime;  // time of the initial message transmission; seconds
    uint32_t    iTickTime;// time of the initial message transmission; ticks
    uint32_t    rt;     // current retramsmission timeout; seconds
    uint32_t    waitTick;   // tick when timeout occurs; corresponds to rt + rand();   

    uint32_t    elapsedTime;    // time from the beginning of this DHCP transaction
                                // in hundredths of a second (10^-2 seconds)
                                // (currTickTime - iTickTime) in 10^-2 sec;
}TCPIP_DHCPV6_MSG_TRANSMIT_DCPT;


// random variation for retries timing, miliseconds
// Standard sets it to 0.1 second 
#define TCPIP_DHCPV6_EXP_RAND_FUZZ     100

// IA address descriptor: address + internal flags
//

typedef union
{
    uint32_t    val;
    struct
    {
        uint8_t        iaUnicast;   // server needs unicast comm
        uint8_t        addInTx;     // address is valid and needs transmitted
        uint8_t        reserved[2]; // not currently in use
    };
}TCPIP_DHCPV6_IA_FLAGS;                                             // address flags

// compressed representation of DHCP options as bit masks
typedef struct
{
    int             nSets;          // number of option sets: usually ((TCPIP_DHCPV6_OPT_CODE_NUMBER - 1) / 32) + 1;

    uint32_t        optionSet[0];   // [((TCPIP_DHCPV6_OPT_CODE_NUMBER - 1) / 32) + 1];   // option set;
                                    // each option set contains 32 option masks:
                                    // set 0: option 0  - 31
                                    // set 1: option 32 - 63
                                    // etc.
                                    //
}TCPIP_DHCPV6_OPTION_MASK_SET;

// compressed representation of DHCP options as bit masks
typedef struct
{
    int             nSets;          // = 1

    uint32_t        optionSet[1];   // option set 0;
                                    // only set 0 containing options: 0  - 31
                                    //
}TCPIP_DHCPV6_OPTION_MASK_SET_0;

// compressed representation of DHCP options as bit masks
typedef struct
{
    int             nSets;          // = 3

    uint32_t        optionSet[3];   // option set 0: 0 - 31
                                    // option set 1: 32 - 63
                                    // option set 2: 64 - 95
}TCPIP_DHCPV6_OPTION_MASK_SET_3;

// data used for the writing options
typedef struct
{
    uint8_t*                    pWrite;     // current write pointer
    int16_t                     writeSpace; // current available write space
    int16_t                     writeBytes; // written bytes
}TCPIP_DHCPV6_MSG_WRITE_DCPT;


// DHCPV6 message buffer for TX/RX of messages

// forward def
struct _tag_TCPIP_DHCPV6_IA_DCPT;

typedef struct _tag_TCPIP_DHCPV6_MSG_BUFFER
{
    struct _tag_TCPIP_DHCPV6_MSG_BUFFER*    next;       // safecast to SGL_LIST_NODE
    struct _tag_TCPIP_DHCPV6_IA_DCPT*       owner;      // IA that owns of the message
    uint16_t                                bufferSize; // allocated size of the buffer 
    uint16_t                                msgLen;     // size of the carried message 
    uint8_t*                                pMsgData;   // pointing to message payload: normally to msgData
    uint16_t                                optLen;     // size of the carried options (whole message w/o TCPIP_DHCPV6_MESSAGE_HEADER 
    uint8_t*                                pOptData;   // pointing to option payload: TCPIP_DHCPV6_MESSAGE_HEADER->options
    uint8_t                                 msgData[0]; // data buffer: pClient->msgBufferSize
}TCPIP_DHCPV6_MSG_BUFFER;



typedef struct
{
    TCPIP_DHCPV6_IA_TYPE    type;       // IANA/IATA
    uint32_t                genId;      // generated IA ID
    uint32_t                tAcquire;   // current time when the IA address was acquired - REPLY 
    union
    {
        TCPIP_DHCPV6_OPTION_IANA_BODY   ianaBody;
        TCPIP_DHCPV6_OPTION_IATA_BODY   iataBody;
    };
}TCPIP_DHCPV6_IA_BODY;

// client descriptor; forward declaration
struct _tag_TCPIP_DHCPV6_CLIENT_DCPT;

// IA descriptor: contains one address only
typedef struct _tag_TCPIP_DHCPV6_IA_DCPT
{
    struct _tag_TCPIP_DHCPV6_IA_DCPT*       next;
    struct _tag_TCPIP_DHCPV6_IA_DCPT*       prev;           // safecast to DBL_LIST_NODE
    // control data
    struct _tag_TCPIP_DHCPV6_CLIENT_DCPT*   pParent;        // parent: access to defaults, clientDuid, etc.
    int                                     parentIx;       // index in the parent array
    TCPIP_DHCPV6_DUID_DCPT                  serverDuid;     // DUID of Server associated with this IA
    IPV6_ADDR                               serverUcastAddr;// Server unicast address
    TCPIP_DHCPV6_CLIENT_MSG_TYPE            cliMsgType;     // current mesage to be transmitted
    TCPIP_UINT32_VAL                        transactionId;  // current transaction ID
                                                            // Note: host order, not converted!
    TCPIP_DHCPV6_MSG_TRANSMIT_DCPT          msgTxDcpt;      // message transmit descriptor 
    TCPIP_DHCPV6_OPTION_MASK_SET_3*         pOroOptMask;    // pointer to current ORO options mask
    TCPIP_DHCPV6_MSG_WRITE_DCPT             wrDcpt;         // current write descriptor
    TCPIP_DHCPV6_MSG_BUFFER*                msgBuffer;      // buffer for the DHCP message to be assembled
    // status
    volatile TCPIP_DHCPV6_IA_STATE          iaState;
    volatile TCPIP_DHCPV6_IA_SUBSTATE       iaSubState;

    //int16_t                                 nAddrs;     // how many addresses associated with this IA_NA
                                                        // Kame: "The primary goal of IAs is to define multiple identities
                                                        // within a single client, each of which is associated with a different IPv6 address.
                                                        // For example, consider a client acting as “virtual hosts” which provide multiple services
                                                        // with different IPv6 addresses.
                                                        // If the client wants to configure itself with these addresses using DHCPv6, it would
                                                        // associate each address with a separate IA!
                                                        // ! Multiple IPv6 addresses can also be associated with a single IA. For instance, a host in
                                                        // a multihomed site that provides a single service may have a single IA containing multiple addresses."
                                                        // But the usual case is one address per IA:
                                                        // this is what this implementation currently supports for both IANA/IATA!
                                                        //
                                                        //
    TCPIP_DHCPV6_IA_FLAGS                   flags;
    // IA data
    TCPIP_DHCPV6_IA_BODY                    iaBody;     // parameters for this IA_NA 
    TCPIP_DHCPV6_OPTION_IA_ADDR_BODY        addBody;    // address body

    uint32_t                                lastT1;     // last known T1 - IANA
    uint32_t                                lastT2;     // last known T2 - IANA
    uint32_t                                lastPrefLTime;  // last known preferred life time for the IPv6 address; seconds
    uint32_t                                lastValidLTime; // last known valid life time for the IPv6 address; seconds
 
    TCPIP_DHCPV6_SERVER_STATUS_CODE         lastStatusCode; // last status code for the IA
    uint8_t                                 lastStatusMsg[TCPIP_DHCPV6_STATUS_CODE_MESSAGE_LEN];// latest status message associated with the IA
                                                            // status Code message    
}TCPIP_DHCPV6_IA_DCPT;

// round generation counters
typedef struct
{
    uint16_t    ianaIaidRound;  // iaid generation counter
    uint16_t    iataIaidRound;  // iaid generation counter
}TCPIP_DHCPV6_IA_ROUND;


// descriptor of a DHCPv6 client
typedef struct _tag_TCPIP_DHCPV6_CLIENT_DCPT
{
    TCPIP_DHCPV6_DUID_DCPT             clientDuid;     // DUID to be used for this client
    
    // current state
    volatile TCPIP_DHCPV6_CLIENT_STATE  state;

    // running interface
    TCPIP_NET_IF*                       pDhcpIf;


    UDP_SOCKET                          dhcpSkt;        // associated socket

    uint32_t                            defaultIanaT1;  // The default time at which the client contacts the IANA server
    uint32_t                            defaultIanaT2;  // The default time at which the client contacts any available server
    uint32_t                            defaultIataT1;  // The default time at which the client contacts the IATA server
    uint32_t                            defaultIataT2;  // The default time at which the client contacts any available server

    uint32_t                            ianaSolicitT1;  // The default T1 time to solicit from the server
    uint32_t                            ianaSolicitT2;  // The default T2 time to solicit from the server
    uint32_t                            solicitPrefLTime;// default addresses preferred lifetime to solicit from the server
    uint32_t                            solicitValidLTime;// default addresses valid lifetime to solicit from the server
    uint32_t                            solMaxRt;         // SOL_MAX_RT override - RFC 7083
    uint32_t                            infoMaxRt;        // INF_MAX_RT override - RFC 7083

    // Identity associations for this client
    TCPIP_DHCPV6_IA_ROUND               iaIaidRound;    // iaid generation counter

    // dynamically allocated Identity associations
    //
    TCPIP_DHCPV6_IA_DCPT*               iaArray;        // array of allocated (IANA + IATA + free) descriptors
    uint16_t                            nIaDcpts;       // total number of allocated (IANA + IATA + free) descriptors this client supports
    uint16_t                            nIanaDcpts;     // number of allocated IA_NA descriptors this client supports
    uint16_t                            nIataDcpts;     // number of allocated IA_TA descriptors this client supports
    uint16_t                            nFreeDcpts;     // number of allocated IA descriptors that are free for use at run time

    uint16_t                            nMsgBuffers;    // number of message buffers
    size_t                              msgBufferSize;  // size of each message buffer
    TCPIP_DHCPV6_MSG_BUFFER*            msgBuffers;     // buffers themselves

    // general lists of buffers, messages, etc
    //
    SINGLE_LIST                         buffFreeList;   // TCPIP_DHCPV6_MSG_BUFFER buffers in a free list for RX/TX transfers
    SINGLE_LIST                         rxMsgList;      // queue of TCPIP_DHCPV6_MSG_BUFFER messages waiting for processing
                                                        // the TCPIP_DHCPV6_MSG_BUFFER::buffer contains the TCPIP_DHCPV6_MESSAGE_HEADER structures. 
    SINGLE_LIST                         txMsgList;      // queue of TCPIP_DHCPV6_MSG_BUFFER messages waiting for transmission
                                                        // the TCPIP_DHCPV6_MSG_BUFFER::buffer contains the TCPIP_DHCPV6_MESSAGE_HEADER data to be transmittted. 

    // specific IA processing lists 
    DOUBLE_LIST                         iaStateList[TCPIP_DHCPV6_IA_STATE_NUMBER];    // queue of IAs in one of the corresponding run state
    DOUBLE_LIST                         iaFreeList;     // queue of free/unused IAs (failed DAD, released, etc.)

    SINGLE_LIST                         advertiseList;  // queue of advertisments received from servers: TCPIP_DHCPV6_MSG_BUFFER
                                                        // the TCPIP_DHCPV6_MSG_BUFFER::buffer contains the TCPIP_DHCPV6_MESSAGE_HEADER structures. 
                                                        // these are global advertisments, for all IAs!
    SINGLE_LIST                         replyList;      // queue of replies received from servers: TCPIP_DHCPV6_MSG_BUFFER
                                                        // the TCPIP_DHCPV6_MSG_BUFFER::buffer contains the TCPIP_DHCPV6_MESSAGE_HEADER structures. 
                                                        // these are global replies, for all IAs!

    TCPIP_MAC_EVENT                     connEvent;      // serialized connection event
                                                        // makes sure that CONN_LOST and CONN_ESTABLISHED are 
                                                        // always executed in sequence
    int                                 nDnsServers;    // how many DNS servers we have
    IPV6_ADDR                           dnsServersAdd[TCPIP_DHCPV6_DNS_SERVERS_NO];   // DNS Servers storage
    int                                 domainSearchListSize;   // size of domainSearchList
    uint8_t                             domainSearchList[TCPIP_DHCPV6_DOMAIN_SEARCH_LIST_SIZE]; // Domain Search list storage 

    TCPIP_DHCPV6_SERVER_STATUS_CODE     lastStatusCode; // last status code for the client
    uint8_t                             lastStatusMsg[TCPIP_DHCPV6_STATUS_CODE_MESSAGE_LEN];// latest status message associated with the client
    
    // statistics counters
    uint32_t                            txBuffFailCnt;  // failed to get a TX buffer count
    uint32_t                            txSktSpaceCnt;  // not enough room in the socket TX buffer count; message was discarded!
    uint32_t                            txSktFlushFailCnt;  // failed to flush the UDP message counter
    uint32_t                            rxBuffFailCnt;  // failed to get a RX buffer count
    uint32_t                            rxBuffSmallCnt; // RX buffer too small for a DHCP packet count


    union
    {
        uint16_t    val;
        struct
        {
            uint16_t    dhcpEnabled:    1;
            uint16_t    connLost:       1;  // CONN_LOST event received
        };
    }flags;
    
    TCPIP_DHCPV6_CONFIG_FLAGS           configFlags;     // configuration/start up flags
}TCPIP_DHCPV6_CLIENT_DCPT;






typedef struct  _TAG_DHCPV6_LIST_NODE
{
	struct _TAG_DHCPV6_LIST_NODE*   next;		// next node in list
                                                // makes it valid SGL_LIST_NODE node
    TCPIP_DHCPV6_EVENT_HANDLER      handler;    // handler to be called for event
    const void*                     hParam;     // handler parameter
    TCPIP_NET_HANDLE                hNet;       // interface that's registered for
                                                // 0 if all    
}TCPIP_DHCPV6_LIST_NODE;







#endif  // __DHCPV6_PRIVATE_H_


