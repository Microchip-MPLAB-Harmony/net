/*******************************************************************************
  IPv6 Dynamic Host Configuration Protocol (DHCPv6) Client API Header File

  Company:
    Microchip Technology Inc.

  File Name:
    dhcpv6.h

  Summary:
    Dynamic Host Configuration Protocol (DHCPv6) client API definitions.

  Description:
    Provides automatic IP address, subnet mask, gateway address,
    DNS server address, and other configuration parameters on DHCPv6
    enabled networks.
*******************************************************************************/

//DOM-IGNORE-BEGIN
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








//DOM-IGNORE-END

#ifndef __DHCPV6_H
#define __DHCPV6_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Enumeration: TCPIP_DHCPV6_CLIENT_RES

  Summary:
    DHCPv6 Operation Result

  Description:
    This enumeration lists the return codes for a DHCPv6 module operation.
 */
typedef enum
{
    TCPIP_DHCPV6_CLIENT_RES_OK         = 0,    // operation successful
    TCPIP_DHCPV6_CLIENT_RES_BUSY       = 1,    // module currently busy
                                        // operation could be retried

    // errors
    TCPIP_DHCPV6_CLIENT_RES_DOWN       = -1,   // service is down
    TCPIP_DHCPV6_CLIENT_RES_DISABLED   = -2,   // service is disabled, not active
    TCPIP_DHCPV6_CLIENT_RES_IF_DOWN    = -3,   // interface is down
    TCPIP_DHCPV6_CLIENT_RES_ARG_ERR    = -4,   // bad argument supplied
    TCPIP_DHCPV6_CLIENT_RES_IX_ERR     = -5,   // bad index supplied
    TCPIP_DHCPV6_CLIENT_RES_NO_ADDR    = -6,   // no such address

    

}TCPIP_DHCPV6_CLIENT_RES;

// *****************************************************************************
/* Enumeration: TCPIP_DHCPV6_CLIENT_STATE

  Summary:
    DHCPv6 Current Status

  Description:
    This enumeration lists the current status of the DHCPv6 module.
    Used in getting info about the DHCPv6 state machine. 
 */
typedef enum
{
    TCPIP_DHCPV6_CLIENT_STATE_INIT = 0,         // initialization state
    TCPIP_DHCPV6_CLIENT_STATE_IDLE,             // idle/inactive state
    TCPIP_DHCPV6_CLIENT_STATE_START,            // service start state

    TCPIP_DHCPV6_CLIENT_STATE_RUN,              // up and running in one of the run states
    TCPIP_DHCPV6_CLIENT_STATE_WAIT_LINK,        // up and running, waiting for a connection
    TCPIP_DHCPV6_CLIENT_STATE_REINIT,           // re-initialization state

    TCPIP_DHCPV6_CLIENT_STATE_NUMBER            // number of states
}TCPIP_DHCPV6_CLIENT_STATE;




// *****************************************************************************
// supported types of IA
typedef enum
{
    TCPIP_DHCPV6_IA_TYPE_NONE,          // unused IA association
    TCPIP_DHCPV6_IA_TYPE_IANA,          // IANA association
    TCPIP_DHCPV6_IA_TYPE_IATA,          // IATA association

    TCPIP_DHCPV6_IA_TYPE_NUMBER         // number of types
}TCPIP_DHCPV6_IA_TYPE;


// *****************************************************************************
// IA run states
// 16 bits only
typedef enum
{
    TCPIP_DHCPV6_IA_STATE_SOLICIT,         // solicitation 
    TCPIP_DHCPV6_IA_STATE_REQUEST,         // perform request
    TCPIP_DHCPV6_IA_STATE_DAD,             // start the DAD state
    TCPIP_DHCPV6_IA_STATE_DECLINE,         // decline the DAD addresses
    TCPIP_DHCPV6_IA_STATE_BOUND,           // bound; 
    TCPIP_DHCPV6_IA_STATE_RENEW,           // renew address
    TCPIP_DHCPV6_IA_STATE_REBIND,          // rebind address
    TCPIP_DHCPV6_IA_STATE_CONFIRM,         // confirm address
    TCPIP_DHCPV6_IA_STATE_RELEASE,         // release address
    TCPIP_DHCPV6_IA_STATE_ERROR_TRANSIENT, // an error occurred for which either user intervention is required
    TCPIP_DHCPV6_IA_STATE_ERROR_FATAL,     // fatal error occurred; not properly configured options/buffers, etc;
    //
    //
    TCPIP_DHCPV6_IA_STATE_NUMBER           // number of run states
}TCPIP_DHCPV6_IA_STATE;



// *****************************************************************************
// IA run substates
// most IA run states that must send a message go through these substates
// 16 bits only
typedef enum
{
    TCPIP_DHCPV6_IA_SUBSTATE_START,        // message start/preparation
    TCPIP_DHCPV6_IA_SUBSTATE_IDELAY,       // message wait for iDelay
    TCPIP_DHCPV6_IA_SUBSTATE_TRANSMIT,     // send/transmit message
    TCPIP_DHCPV6_IA_SUBSTATE_WAIT_REPLY,   // wait message reply
    //
    //

    TCPIP_DHCPV6_IA_SUBSTATE_NUMBER,      // number of standard message sub-states  

}TCPIP_DHCPV6_IA_SUBSTATE;


// *****************************************************************************
// types of DUID for DHCPv6
typedef enum
{
    TCPIP_DHCPV6_DUID_TYPE_NONE     = 0,    // invalid
    TCPIP_DHCPV6_DUID_TYPE_LLT      = 1,    // LinkLayer + time        
    TCPIP_DHCPV6_DUID_TYPE_EN       = 2,    // Enterprise Number
    TCPIP_DHCPV6_DUID_TYPE_LL       = 3,    // Link Layer Address

}TCPIP_DHCPV6_DUID_TYPE;


// *****************************************************************************
// DHCPV6  server status code
// 16 bits only
typedef enum
{
    TCPIP_DHCPV6_SERVER_STAT_SUCCESS           = 0,     // success
    TCPIP_DHCPV6_SERVER_STAT_UNSPEC_FAIL       = 1,     // Failure, reason unspecified;
                                                        // this status code is sent by either a client or a server
                                                        // to indicate a failure not explicitly specified in the RFC. 


    TCPIP_DHCPV6_SERVER_STAT_NO_ADDRS_AVAIL    = 2,     //  Server has no addresses available to assign to the IA(s).

    TCPIP_DHCPV6_SERVER_STAT_NO_BINDING        = 3,     // Client record (binding) unavailable.

    TCPIP_DHCPV6_SERVER_STAT_NOT_ON_LINK       = 4,     // The prefix for the address is not appropriate for the link
                                                        // to which the client is attached.


    TCPIP_DHCPV6_SERVER_STAT_USE_MULTICAST     = 5,     // Sent by a server to a client to force the client to send
                                                        // messages to the server using the All_DHCP_Relay_Agents_and_Servers address.

    // external codes, internal usage
    //
    TCPIP_DHCPV6_SERVER_STAT_MAX_CODE          = 5,     // maximum valid value 
    TCPIP_DHCPV6_SERVER_STAT_EXT_ERROR         = -1,    // an error occurred, status code not found, etc.


}TCPIP_DHCPV6_SERVER_STATUS_CODE;




// *****************************************************************************
// DHCPV6 start up flags
typedef enum
{
    TCPIP_DHCPV6_FLAG_NONE                      = 0,
    TCPIP_DHCPV6_FLAG_START_ENABLE              = 0x01,     // enable the DHCPv6 at stack start up
                                                            //
    TCPIP_DHCPV6_FLAG_DAD_DISABLE               = 0x02,     // disable the DAD processing for DHCP generated addresses
                                                            // Use only for testing or in special cases
                                                            // Default should be disabled
                                                            //
    TCPIP_DHCPV6_FLAG_IA_IGNORE_RENEW_LTIME     = 0x04,     // if enabled, the IA (and its associated address) renew process will be valid as dictated by t1/defaultIataT1 and
                                                            // its address preferred lifetime will be ignored
                                                            // If disabled, the IA and its address will attempt renew when the minimum of address preferred lifetime and t1/defaultIataT1 expired
                                                            // Default should be disabled
                                                            //
    TCPIP_DHCPV6_FLAG_IA_IGNORE_REBIND_LTIME    = 0x08,     // if enabled, the IA (and its associated address) rebind process will be valid as dictated by t2/defaultIataT2 and
                                                            // its address valid lifetime will be ignored
                                                            // If disabled, the IA and its address will attempt rebind when the minimum of address valid lifetime and t2/defaultIataT2 expired
                                                            // Default should be disabled
                                                            //
    TCPIP_DHCPV6_FLAG_IA_NOTIFY_SUB_STATE       = 0x80,     // if enabled, the IA notifications will be generated for IA substate changes too (finer grain)
                                                            // if disabled, notifications will be generated for IA state changes only
                                                            // Default should be disabled
}TCPIP_DHCPV6_CONFIG_FLAGS;



// *****************************************************************************
// DHCPv6 module configuration
//
typedef struct
{
    TCPIP_DHCPV6_CONFIG_FLAGS   configFlags;    // DHCPv6 client configuration flags 
    uint16_t                    dhcpCliPort;    // client port for DHCPv6 client transactions
    uint16_t                    dhcpSrvPort;    // remote server port for DHCPv6 server messages
    uint16_t                    duidType;       // TCPIP_DHCPV6_DUID_TYPE: type to use for the DHCPv6 clients
                                                // Note: TCPIP_DHCPV6_DUID_TYPE_LL supported for now!
    uint16_t                    nIanaDcpts;     // number of IANAs per client; default should be 1
    uint16_t                    nIataDcpts;     // number of IATAs per client; default should be 0
    uint16_t                    nFreeDcpts;     // number of free IAs per client - they could be added at run time;
                                                // default should be 0

    uint32_t                    defaultIanaT1;  // The default time at which the client contacts the server
                                                // to extend the lifetimes of the assigned IA_NA addresses
                                                // If the IANA t1 value received from the server is 0,
                                                // then this value will be used to override
                                                // A value of 0 means the t1 is infinite
                                                // default value should be 0
    uint32_t                    defaultIanaT2;  // The default time at which the client contacts any available server
                                                // to extend the lifetimes of the assigned IA_NA addresses
                                                // If the IANA t2 value received from the server is 0,
                                                // then this value will be used to override
                                                // if !0 it should be > defaultIanaT1!
                                                // Has to be > t1
                                                // A value of 0 means the t2 is infinite
                                                // default value should be 0
                                                //
    uint32_t                    defaultIataT1;  // The default time at which the client contacts the server
                                                // to extend the lifetimes of the assigned IATA addresses
                                                // If 0, the timeout will be infinite (0xffffffff)
    uint32_t                    defaultIataT2;  // The default time at which the client contacts any available server
                                                // to extend the lifetimes of the assigned IA_TA addresses
                                                // if !0 it should be > defaultIataT1!
                                                // If 0, the timeout will be infinite (0xffffffff)
                                                // 
    uint32_t                    ianaSolicitT1;  // The default T1 time to solicit from the server
                                                // default should be 0
    uint32_t                    ianaSolicitT2;  // The default T2 time to solicit from the server
                                                // default should be 0
                                                //
    uint32_t                    solicitPrefLTime;// default addresses preferred lifetime to solicit from the server
                                                // default should be 0
    uint32_t                    solicitValidLTime;// default addresses valid lifetime to solicit from the server
                                                // default should be 0

    int                         nMsgBuffers;    // number of message buffers to allocate for this client
                                                // these buffers are used for the TX/RX operations
                                                // Enough buffers need to be allocated for gathering server advertisements
                                                // and being able to transmit messages
                                                // default should be 4
    int                         msgBufferSize;  // size of the message buffers
                                                // default is 512 bytes
                                                //
}TCPIP_DHCPV6_MODULE_CONFIG;

// *****************************************************************************
// IA event info
typedef union
{
    uint32_t    eventVal;
    struct
    {
        uint8_t     iaType;         // a TCPIP_DHCPV6_IA_TYPE value 
        uint8_t     iaState;        // a TCPIP_DHCPV6_IA_STATE value
        uint8_t     iaSubState;     // a TCPIP_DHCPV6_IA_SUBSTATE value 
        uint8_t     iaIndex;        // index/ID of this IA for this client
    };
}TCPIP_DHCPV6_IA_EVENT;


// *****************************************************************************
// DHCPv6 IA info

typedef struct
{
    // input parameters
    int                         iaIndex;        // index of the IA to look for
                                                // if < 0, it will just return the 1st index available
    TCPIP_DHCPV6_IA_STATE       iaState;        // IA state to look for
                                                // if < 0 the search will look into the free (not currently used) IA list
    void*                       statusBuff;     //  buffer to copy the latest status message associated with this IA
    size_t                      statusBuffSize; // size of this buffer

    // output parameters
    int                         nextIndex;      // the next index that could be used to query for
                                                // Note that this is transient since the IAs can dynamically change
                                                // If < 0, no other entry exists
    TCPIP_DHCPV6_IA_TYPE        iaType;         // IA type 
    TCPIP_DHCPV6_IA_SUBSTATE    iaSubState;     // IA substate 
    uint32_t                    iaId;           // ID of this IA
    // the following fields are meaningful only for iaState >= TCPIP_DHCPV6_IA_STATE_BOUND
    uint32_t                    tAcquire;       // time of which the address was acquired
    uint32_t                    t1;             // IANA only: extend lifetime contact server time 
    uint32_t                    t2;             // IANA only: extend lifetime contact any server time 
    IPV6_ADDR                   ipv6Addr;       // 16 bytes IPV6 address associated with this IA
    uint32_t                    prefLTime;      // preferred life time for the IPv6 address; seconds
    uint32_t                    validLTime;     // valid life time for the IPv6 address; seconds
    // other info
    TCPIP_DHCPV6_SERVER_STATUS_CODE lastStatusCode; // last status code for this IA
    void*                       msgBuffer;      // if an associated message buffer exists
}TCPIP_DHCPV6_IA_INFO;

// *****************************************************************************
// DHCPv6 client info
typedef struct
{
    // client state at the moment of the call
    TCPIP_DHCPV6_CLIENT_STATE   clientState;
    // total number of IANAs
    size_t                      totIanas;
    // total number of IATAs
    size_t                      totIatas;
    // current number of bound IANAs
    size_t                      nIanas;
    // current number of bound IATAs
    size_t                      nIatas;
    // current number of free IAs the client has
    size_t                      nFreeIas;
    // number of solicit IAs
    size_t                      solIas;
    // number of request IAs
    size_t                      reqIas;
    // number of DAD IAs
    size_t                      dadIas;
    // number of decline IAs
    size_t                      declineIas;
    // number of bound IAs
    size_t                      boundIas;
    // number of renew IAs
    size_t                      renewIas;
    // number of rebind IAs
    size_t                      rebindIas;
    // number of confirm IAs
    size_t                      confirmIas;
    // number of release IAs
    size_t                      releaseIas;
    // number of transient error IAs
    size_t                      transIas;

    // total number of message buffers
    size_t                      totBuffers;
    // current number of free message buffers
    size_t                      freeBuffers;
    // current DHCPV6 time; milli-seconds
    uint32_t                    dhcpTime;
    // last status code for the client
    TCPIP_DHCPV6_SERVER_STATUS_CODE lastStatusCode;
    // buffer to copy the latest status message associated with the client
    void*                       statusBuff;
    // size of this buffer
    size_t                      statusBuffSize;
    // number of DNS servers
    size_t                      nDnsServers;
    // buffer to copy the DNS Servers obtained from the DHCPV6 server
    IPV6_ADDR*                  dnsBuff;
    // size of this buffer
    size_t                      dnsBuffSize;
    // size of domainSearchList
    size_t                      domainSearchListSize;
    // buffer to store the domain Search list obtained from the DHCPv6 server
    void*                       domainBuff;
    // size of this buffer
    size_t                      domainBuffSize;
    
}TCPIP_DHCPV6_CLIENT_INFO;


// *****************************************************************************
// DHCPv6 reported event structure:
/*
  Type:
    TCPIP_DHCPV6_EVENT_HANDLER

  Summary:
    DHCPv6 event handler prototype.

  Description:
    Prototype of a DHCP event handler. Clients can register a handler with the
    DHCP service. Once an DHCP event occurs the DHCP service will called the
    registered handler.
    The handler has to be short and fast. It is meant for
    setting an event flag, <i>not</i> for lengthy processing!
 */
// if pDhcpIaEvent == 0, no info is carried for a specific IA 
// if pDhcpIaEvent != 0, the info carried by this pointer is not persistent and is valid only within the context of this
// event handler!
typedef void    (*TCPIP_DHCPV6_EVENT_HANDLER)(TCPIP_NET_HANDLE hNet, TCPIP_DHCPV6_CLIENT_STATE clientState, const TCPIP_DHCPV6_IA_EVENT* pDhcpIaEv, const void* param);

// a DHCPV6 handle
typedef const void* TCPIP_DHCPV6_HANDLE;


// *****************************************************************************
// DHCPv6 client statistics
// Note: all members should be uint32_t!
typedef struct
{
    // number of total message buffers
    uint32_t    msgBuffers;
    // number of free buffers
    uint32_t    freeBuffers;
    // number of pending RX messages
    uint32_t    rxMessages;
    // number of pending TX messages
    uint32_t    txMessages;
    // number of advertise messages
    uint32_t    advMessages;
    // number of pending reply messages
    uint32_t    replyMessages;

    // failed TX operations due to missing buffers
    uint32_t    txBuffFailCnt;
    // failed TX operations due to not enough space in the socket TX buffer
    uint32_t    txSpaceFailCnt;
    // failed TX operations: amount of data written less than submitted 
    uint32_t    txSktFlushFailCnt;

    // failed RX operations due to missing buffers
    uint32_t    rxBuffFailCnt;
    // failed RX operations due packets larger than the received message
    uint32_t    rxBuffSpaceFailCnt;
    
}TCPIP_DHCPV6_CLIENT_STATISTICS;

// *****************************************************************************
// *****************************************************************************
// Section: API functions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// client status reporting
TCPIP_DHCPV6_CLIENT_RES TCPIP_DHCPV6_ClientInfoGet(TCPIP_NET_HANDLE hNet, TCPIP_DHCPV6_CLIENT_INFO* pClientInfo);


// *****************************************************************************
// IA status reporting
TCPIP_DHCPV6_CLIENT_RES TCPIP_DHCPV6_IaInfoGet(TCPIP_NET_HANDLE hNet, TCPIP_DHCPV6_IA_INFO* pIaInfo);


// DHCPV6 event registration
TCPIP_DHCPV6_HANDLE TCPIP_DHCPV6_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_DHCPV6_EVENT_HANDLER handler, const void* hParam);


// DHCPV6 event deregistration
bool TCPIP_DHCPV6_HandlerDeRegister(TCPIP_DHCPV6_HANDLE hDhcp);

// *****************************************************************************
// Disables the service at run time for an interface
TCPIP_DHCPV6_CLIENT_RES TCPIP_DHCPV6_Disable(TCPIP_NET_HANDLE hNet);


// *****************************************************************************
// Enables the service at run time for an interface
TCPIP_DHCPV6_CLIENT_RES TCPIP_DHCPV6_Enable(TCPIP_NET_HANDLE hNet);

// *****************************************************************************
// Releases a DHCPv6 IPv6 address for an interface
// Note: the address type could be IANA or IATA
TCPIP_DHCPV6_CLIENT_RES TCPIP_DHCPV6_AddrRelease(TCPIP_NET_HANDLE hNet, const IPV6_ADDR* addr);

// *****************************************************************************
// Get the server statistics for an interface
// Note: exists only if the TCPIP_DHCPV6_STATISTICS_ENABLE is defined and !0 
TCPIP_DHCPV6_CLIENT_RES TCPIP_DHCPV6_Statistics(TCPIP_NET_HANDLE hNet, TCPIP_DHCPV6_CLIENT_STATISTICS* pStat);


// *****************************************************************************
/*
  Function:
    void  TCPIP_DHCPV6_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs DHCPv6 module tasks in the TCP/IP stack.

  Precondition:
    DHCPv6 module should have been initialized

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_DHCPV6_Task(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // __DHCPV6_H

/*
 End of File
 */

