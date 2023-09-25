/*******************************************************************************
  Dynamic Host Configuration Protocol (DHCP) Server API Header File

  Company:
    Microchip Technology Inc.

  File Name:
    dhcp_server.h

  Summary:
    Dynamic Host Configuration Protocol(DHCP) Server APIs.

  Description:
     The DHCP server module implements the 'RFC 2131 - Dynamic Host Configuration Protocol' standard.
     It assigns an IP address to a requesting DHCP client.
     The server address range is defined when the module is configured.
 
     Some of the known issues and implementation limitations:
        - there is no support for multiple sub-net pools
        - Operation for clients using relay agents may be impacted
            The server will assign a lease in the server network,
            not in the relay agent 'down link' network


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

#ifndef __DHCP_SERVER_H
#define __DHCP_SERVER_H

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
/*
  Enumeration:
    TCPIP_DHCPS_CONFIG_FLAGS

  Summary:
    DHCP server configuration flags for a specific interface.

  Description:
    DHCP server configuration flags

  Remarks:
    Only 16 bits are used for the flags implementation. 
*/

typedef enum
{
    TCPIP_DHCPS_CONFIG_FLAG_NONE                = 0, 
    TCPIP_DHCPS_CONFIG_FLAG_DISABLED            = 0x0001,   // DHCP server should start disabled on this interface
                                                            // Note: this setting is currently not used and overridden by the
                                                            // TCPIP_NETWORK_CONFIG:: startFlags.TCPIP_NETWORK_CONFIG_DHCP_SERVER_ON
    TCPIP_DHCPS_CONFIG_FLAG_DELETE_OLD          = 0x0002,   // DHCP server should delete the existent info for leases
                                                            // when the interface is restarted 
    TCPIP_DHCPS_CONFIG_FLAG_NO_CONFLICT_DETECT  = 0x0004,   // disable DHCP server conflict detection of newly allocated addresses
                                                            // by default the server should send an ICMP echo request when using a new IP address
    TCPIP_DHCPS_CONFIG_FLAG_NO_LEASE_EXTEND     = 0x0008,   // disable DHCP clients request for extending the lease
                                                            // by default the server grants lease extension to the clients
    TCPIP_DHCPS_CONFIG_FLAG_KEEP_UNREQ_OFFERS   = 0x0010,   // keep DB info for offers that haven't been requested by the client
                                                            // i.e. when client selected another server (multi-server environment)
                                                            // By default the info for these clients is deleted to free some space in the DB
    TCPIP_DHCPS_CONFIG_FLAG_ABORT_IF_PROBE_FAILED = 0x0020, // when DHCP server conflict detection is enabled, if sending echo requests failed
                                                            // (ICMP module rejected the calls, TCPIP_DHCPS_EVENT_ECHO_FAIL event)
                                                            // abort offering the lease to the client
                                                            // By default the server will send the offer to the client
    TCPIP_DHCPS_CONFIG_FLAG_NO_RECORD_KEEP_SILENT = 0x0040, // keep silent when receiving a client request for which there is no previous record
                                                            // (i.e. don't verify the requested IP address and send NAK, etc.)
                                                            // By default the server will reply with NAK if the requested lease is invalid

}TCPIP_DHCPS_CONFIG_FLAGS;

// *****************************************************************************
/*
  Enumeration:
    TCPIP_DHCPS_CLIENT_OPTION_TYPE

  Summary:
    DHCP server options to be provided to the clients for a specific interface.

  Description:
    Describes the list of options to be provided to the clients by the DHCP server running an interface.

  Remarks:
    More options will be eventually added as needed

    Maximum 16 options are supported for now.

    The TCPIP_DHCPS_CLIENT_OPTION_T1_RENEWAL and TCPIP_DHCPS_CLIENT_OPTION_T2_REBINDING options
    are controlled by the global build symbol TCPIP_DHCPS_OPTION_T1_T2_SUPPRESS (configuration.h).
    They are enabled by default and have the default values of 1/2 and 7/8 respectively.
    When they are not suppressed, different values than default are configurable using this option.
*/

typedef enum
{
    TCPIP_DHCPS_CLIENT_OPTION_ROUTER           = 0,        // provide router/gateway option
    TCPIP_DHCPS_CLIENT_OPTION_DNS,                         // provide DNS option
    TCPIP_DHCPS_CLIENT_OPTION_TIME_SERVER,                 // provide time server option
    TCPIP_DHCPS_CLIENT_OPTION_NAME_SERVER,                 // provide name server option
    TCPIP_DHCPS_CLIENT_OPTION_T1_RENEWAL,                  // provide T1 renewal option
    TCPIP_DHCPS_CLIENT_OPTION_T2_REBINDING,                // provide T2 rebinding option
    TCPIP_DHCPS_CLIENT_OPTION_NTP_SERVER,                  // provide NTP server option


}TCPIP_DHCPS_CLIENT_OPTION_TYPE;

// *****************************************************************************
/*
  Structure:
    TCPIP_DHCPS_CLIENT_OPTION_CONFIG

  Summary:
    DHCP server configuration of client options

  Description:
    DHCP server configuration of provided client options
    Lists the options to be presented to clients requiring an address lease

  Remarks:
    An option is allowed to appear multiple times.
    For example, using TCPIP_DHCPS_CLIENT_OPTION_DNS 2 times, 2 DN servers will be communicated to the client

    The maximum number of values possible for an option is dictated by:
        TCPIP_DHCPS_OPTION_ROUTER_VALUES     
        TCPIP_DHCPS_OPTION_DNS_VALUES        
        TCPIP_DHCPS_OPTION_TIME_SERVER_VALUES
        TCPIP_DHCPS_OPTION_NAME_SERVER_VALUES
        TCPIP_DHCPS_OPTION_NTP_SERVER_VALUES

        Excess values will be ignored.
    
*/
typedef struct
{
    TCPIP_DHCPS_CLIENT_OPTION_TYPE optType;        // option type
    union
    {
        const char* ipStr;                  // a string describing the option, a valid IP address string
                                            // used for TCPIP_DHCPS_CLIENT_OPTION_ROUTER, TCPIP_DHCPS_CLIENT_OPTION_DNS,
                                            //      TCPIP_DHCPS_CLIENT_OPTION_TIME_SERVER, TCPIP_DHCPS_CLIENT_OPTION_NAME_SERVER, TCPIP_DHCPS_CLIENT_OPTION_NTP_SERVER
        uint32_t    uintVal;                // a uint32_t value

        struct
        {                                   // used for TCPIP_DHCPS_CLIENT_OPTION_T1_RENEWAL and TCPIP_DHCPS_CLIENT_OPTION_T2_REBINDING
            uint16_t mult;                  // the T1 and T2 are obtained as percentages of lease time:
            uint16_t div;                   // T1 = (leaseTime * mult) / div;   default values are mult = 1, div = 2;
                                            // T2 = (leaseTime * mult) / div;   default values are mult = 7, div = 8; 
                                            // Default values are used for mult or div if any of them are 0 
                                            // T2 needs to be greater than T1! 
        };
                                            
    };
}TCPIP_DHCPS_CLIENT_OPTION_CONFIG;


// *****************************************************************************
/*
  Structure:
    TCPIP_DHCPS_INTERFACE_CONFIG

  Summary:
    DHCP server configuration for a specific interface.

  Description:
    DHCP server configuration and network initialization data.
    Configuration is part of tcpip_stack_init.c.
*/
typedef struct
{
    uint8_t     ifIndex;            // interface number to which this configuration refers to
    uint8_t     configFlags;        // a TCPIP_DHCPS_CONFIG_FLAGS value 
    uint16_t    leaseEntries;       // max number of leases for this interface
                                    // Must be <= TCPIP_DHCPS_MAX_LEASES
    uint32_t    leaseDuration;      // default lease duration to be granted to clients, seconds 
    uint32_t    minLeaseDuration;   // minimum lease duration that can be requested by a client, seconds 
                                    // if 0, then a default of 60 seconds will be used
                                    // should be <= leaseDuration
    uint32_t    maxLeaseDuration;   // maximum lease duration that can be requested by a client, seconds 
                                    // if 0, then the leaseDuration will be used
                                    // should be >= leaseDuration
    uint32_t    unreqOfferTmo;      // timeout for unrequested offer sent to the client, seconds
                                    // Note: used if TCPIP_DHCPS_CONFIG_FLAG_KEEP_UNREQ_OFFERS is not set 
                                    // if 0, a default of 5 seconds will be used 
    const char* serverIPAddress;    // server IP address on this interface
    const char* startIPAddress;     // start lease IP address
                                    // the pool of addresses will be [startIPAddress, endIPAddress]
                                    // where endIPAddress is given by the prefixLen
    size_t      prefixLen;          // the number of the leading 1 bits in the network mask
                                    // number <= 32
                                    // the address is specified in the CIDR format: add/prefix_len
                                    // the maximum size of the pool is dictated by the prefixLen
                                    // For example: 192.168.0.0/24 will have 254 leases available 
                                    // (192.168.0.0 and 192.168.0.255 are excluded)
                                    // and endIPAddress == 192.168.0.254
                                    // Note: the number of possible leases == min(leaseEntries, endIPAddress - startIPAddress).
                                    // The number of leases derived from startIPAddress/prefixLen should be > leaseEntries. 
    const TCPIP_DHCPS_CLIENT_OPTION_CONFIG* pOptConfig; // pointer to array of client options for the DHCP server
    uint16_t    nOptConfigs;        // number of option configurations in the pOptConfig array
}TCPIP_DHCPS_INTERFACE_CONFIG;

// *****************************************************************************
/*
  Structure:
    TCPIP_DHCPS_MODULE_CONFIG

  Summary:
    DHCP Server module runtime and initialization configuration data.

  Description:
    DHCP server configuration and initialization data.
    Configuration is part of tcpip_stack_init.c.

  Remarks:
    The number of interfaces that DHCPs supports cannot exceed the number of interfaces in the stack!
*/

typedef struct
{
    const TCPIP_DHCPS_INTERFACE_CONFIG* pIfConfig;      // array of DHCP server configurations per interface
    uint16_t    nConfigs;                               // number of DHCP server configurations in the pIfConfig array, one per interface
                                                        // Cannot exceed the TCPIP_DHCPS_INTERFACE_COUNT, which is the number of interfaces the server can support
    uint8_t     nProbes;                                // number of ICMP echo probes to send when doing address conflict detection - default 1
    uint8_t     conflictAttempts;                       // number of attempts to find a new IP address when ICMP detected an address conflict - default 1
} TCPIP_DHCPS_MODULE_CONFIG;

// *****************************************************************************
/*
  Enumeration:
    TCPIP_DHCPS_RES

  Summary:
    DHCPs operation result

  Description:
    During the initialization process the DHCPs module will print
    an error code if the initialization failed.
    This is a quick pointer to what went wrong in the initialization process.

    Error codes will be also returned by the DHCP server API.

*/

typedef enum
{
    TCPIP_DHCPS_RES_OK                 = 0,    // initialization/operation successful
    // errors

    TCPIP_DHCPS_RES_NO_INIT_DATA       = -1,   // missing initialization data
    TCPIP_DHCPS_RES_IF_CONFIG_ERR      = -2,   // interface configuration number error
    TCPIP_DHCPS_RES_NOT_AVAILABLE      = -3,   // DHCP service does not support the functionality
    TCPIP_DHCPS_RES_IF_REPEAT_ERR      = -4,   // same interface specified twice
    TCPIP_DHCPS_RES_LEASE_PARAM_ERR    = -5,   // wrong lease entries or lease duration
    TCPIP_DHCPS_RES_PREFIX_LEN_ERR     = -6,   // invalid prefix length
    TCPIP_DHCPS_RES_INVALID_ADDRESS    = -7,   // invalid IP address supplied
    TCPIP_DHCPS_RES_NULL_ADDRESS       = -8,   // server or start IP address are NULL
    TCPIP_DHCPS_RES_NOT_ENOUGH_LEASES  = -9,   // not enough leases in the address range
    TCPIP_DHCPS_RES_SUBNET_ERR         = -10,  // server address not in the same subnet with the start IP address
    TCPIP_DHCPS_RES_OPTION_ERR         = -11,  // unsupported/unknown client option in the configuration
    TCPIP_DHCPS_RES_T1_T2_ERR          = -12,  // bad T2, T1 values, T2 <= T1
    TCPIP_DHCPS_RES_ALLOC_ERR          = -13,  // memory allocation error
    TCPIP_DHCPS_RES_SIGNAL_ERR         = -14,  // could not register signal handler
    TCPIP_DHCPS_RES_SKT_CREATE_ERR     = -15,  // could not open the socket
    TCPIP_DHCPS_RES_SKT_SIGNAL_ERR     = -16,  // could not register socket signal handler
    TCPIP_DHCPS_RES_SKT_OPTION_ERR     = -17,  // could not get/set options for the socket
    TCPIP_DHCPS_RES_ACCESS_LOCKED      = -18,  // DHCP DB locked by another thread
    TCPIP_DHCPS_RES_INVALID_IF         = -19,  // invalid interface specified
    TCPIP_DHCPS_RES_INVALID_INDEX      = -20,  // invalid lease index specified
    TCPIP_DHCPS_RES_UNUSED_INDEX       = -21,  // no such entry exists/is populated 
    TCPIP_DHCPS_RES_PARAM_ERROR        = -22,  // a bad parameter was supplied
    TCPIP_DHCPS_RES_NO_LEASE           = -23,  // no such lease exists
    TCPIP_DHCPS_RES_DB_FULL            = -24,  // another lease could not be added, the DB (cache) is full
    TCPIP_DHCPS_RES_STATE_ERROR        = -25,  // a bad state was specified when adding a lease
    TCPIP_DHCPS_RES_SERVICE_START_ERROR = -26, // DHCP service could not be started
    TCPIP_DHCPS_RES_SERVICE_STOP_ERROR  = -27, // DHCP service could not be stopped


    TCPIP_DHCPS_RES_SERVICE_ERROR      = -77,  // DHCP service internal error; Should not happen


}TCPIP_DHCPS_RES;

// *****************************************************************************
/*
  Enumeration:
    TCPIP_DHCPS_REQ_FORMAT_ERR_MASK

  Summary:
    DHCPs error mask for a TCPIP_DHCPS_EVENT_REQ_FORMAT_ERROR event

  Description:
    List of errors that can be signaled by a TCPIP_DHCPS_EVENT_REQ_FORMAT_ERROR event.
    More that one error could be set.

*/

typedef enum
{
    TCPIP_DHCPS_REQ_FORMAT_ERR_NONE             = 0,    // no error occurred
    TCPIP_DHCPS_REQ_FORMAT_ERR_SRV_ID_SET       = 0x0001,   // server identifier set when it should have been cleared
    TCPIP_DHCPS_REQ_FORMAT_ERR_SRV_ID_MISS      = 0x0002,   // server identifier missing when it should have been set
    TCPIP_DHCPS_REQ_FORMAT_ERR_REQUEST_IP_SET   = 0x0004,   // requested IP address set when it should have been cleared
    TCPIP_DHCPS_REQ_FORMAT_ERR_REQUEST_IP_MISS  = 0x0008,   // requested IP address missing
    TCPIP_DHCPS_REQ_FORMAT_ERR_CIADDR_ERR       = 0x0010,   // wrong 'ciaddr' field




}TCPIP_DHCPS_REQ_FORMAT_ERR_MASK;


// *****************************************************************************
/* Enumeration: TCPIP_DHCPS_EVENT_TYPE

  Summary:
    DHCP Event Type

  Description:
    None.
 */
typedef enum
{
    TCPIP_DHCPS_EVENT_NONE     = 0,     // DHCP no event

    // client lease events > 0

    TCPIP_DHCPS_EVENT_ECHO_PROBE_SENT,          // sent an ICMP probe 

    TCPIP_DHCPS_EVENT_DISCOVER,                 // DHCP discovery received 

    TCPIP_DHCPS_EVENT_REQUEST_OFFERRED,         // lease ACK for offer request 

    TCPIP_DHCPS_EVENT_REQUEST_RENEW,            // lease ACK sent for renew request 
    
    TCPIP_DHCPS_EVENT_REQUEST_REBIND,           // lease ACK sent for rebind request 

    TCPIP_DHCPS_EVENT_REQUEST_EXPIRED,          // lease ACK sent for an expired request 

    TCPIP_DHCPS_EVENT_OTHER_SELECT,             // DHCP client selected a different server 

    TCPIP_DHCPS_EVENT_INFORM,                   // DHCPINFORM from a client, with an IP address already assigned 
    // error events < 0
    TCPIP_DHCPS_EVENT_PROCESS_LOCK      = -1,   // DHCP message processing postponed as the DB access was locked 

    TCPIP_DHCPS_EVENT_TICK_LOCK         = -2,   // DHCP timeout processing postponed as the DB access was locked 

    TCPIP_DHCPS_EVENT_MSG_OVERFLOW      = -3,   // DHCP message received is > than the internally allocated buffer  

    TCPIP_DHCPS_EVENT_MSG_UNDERFLOW     = -4,   // DHCP message received is < than the minimum DHCP valid message 

    TCPIP_DHCPS_EVENT_IF_DISABLED       = -5,   // DHCP message received on an interface that has DHCP server disabled 

    TCPIP_DHCPS_EVENT_IF_ERROR          = -6,   // DHCP message received on an interface that the DHCP server has not mapped internally 
                                                    // This should not normally happen

    TCPIP_DHCPS_EVENT_MSG_FORMAT_ERROR  = -7,   // DHCP message received is badly formatted: 
                                                    // operation != 1 or header type != 1 or header length != 6 or bad header MAC address or bad magic cookie

    TCPIP_DHCPS_EVENT_POOL_EMPTY        = -8,   // DHCP pool was empty, cannot allocate a new lease 
                                                    // Note: this could remove a new client from the DB so the TCPIP_DHCPS_LEASE_INFO won't be available any longer

    TCPIP_DHCPS_EVENT_CACHE_FULL        = -9,   // DHCP cache for the interface was full, cannot store a new lease 

    TCPIP_DHCPS_EVENT_ECHO_PROBE_FAIL   = -10,   // Attempt to send an ICMP probe failed: ICMP module rejected the call 

    TCPIP_DHCPS_EVENT_ECHO_FAIL         = -11,   // ICMP probes failed completely; probes aborted 

    TCPIP_DHCPS_EVENT_REPROBE_FAIL      = -12,   // ICMP reprobes failed completely; probes aborted 
                                                    // Note: this will remove the client from the DB so the TCPIP_DHCPS_LEASE_INFO won't be available any longer

    TCPIP_DHCPS_EVENT_ARP_FAIL          = -13,  // attempt to inject to ARP cache failed 
    TCPIP_DHCPS_EVENT_REQ_ADDRESS_ERROR = -14,  // DHCPREQUEST from client with wrong requested IP address 

    TCPIP_DHCPS_EVENT_REQ_UNKNOWN       = -15,  // DHCPREQUEST from an unknown client, no entry in the DB for it 

    TCPIP_DHCPS_EVENT_REQ_FORMAT_ERROR  = -16,  // DHCPREQUEST from client with wrong format 

    TCPIP_DHCPS_EVENT_INFO_FORMAT_ERROR = -17,  // DHCPINFORM from client with wrong format 

    TCPIP_DHCPS_EVENT_REQ_UNEXPECT      = -18,  // unexpected DHCPREQUEST received a client. ignored 

    TCPIP_DHCPS_EVENT_DECLINED          = -19,  // client issues a DHCPDECLINE; address was in use  
                                                    // Note: this will remove the client from the DB so the TCPIP_DHCPS_LEASE_INFO won't be available any longer

    TCPIP_DHCPS_EVENT_INFORM_INVALID    = -20,  // DHCPINFORM from a client, with an IP address not in our network/pool 


} TCPIP_DHCPS_EVENT_TYPE;


// *****************************************************************************
/*
  Type:
    TCPIP_DHCPS_EVENT_HANDLE

  Summary:
    DHCP server handle.

  Description:
    A handle that a client can use after the event handler has been registered.
 */

typedef const void* TCPIP_DHCPS_EVENT_HANDLE;


// *****************************************************************************
/*
  Structure:
    TCPIP_DHCPS_EVENT_DATA_INFO

  Summary:
    DHCP Server event associated data information.

  Description:
    DHCP server data info and flags that's passed when an event occurred

  Remarks:
    None
    
*/

typedef struct
{
    union
    {
        uint16_t    val;
        struct
        {
            uint16_t    infoIpValid:    1;  // if 1, then the ipAddress value is valid 
            uint16_t    infoSize:       1;  // if 1, then the data value represents message size 
            uint16_t    infoSlots:      1;  // if 1, then the data value represents the number of slots in the cache
            uint16_t    infoFmtMask:    1;  // if 1, then the data value represents a TCPIP_DHCPS_REQ_FORMAT_ERR_MASK value
            uint16_t    infoState:      1;  // if 1, then the data value represents a TCPIP_DHCPS_LEASE_STATE value
            uint16_t    infoReqIp:      1; // if 1, then the data value represents the requested IP value
            uint16_t    infoTargetIp:   1; // if 1, then the data value represents a target IP value (ARP/ICMP)
            uint16_t    infoIcmpCount:  1; // if 1, then the data value represents the number of ICMP retries/probes
            uint16_t    reserved:       6;  // reserved for future use
            uint16_t    leaseIndexValid:1;  // if 1, then the leaseIndex value is valid
            uint16_t    macAddValid:    1;  // if 1, then the macAdd value is valid 
        };
    }flags;
    uint32_t    ipAddress;  // if flags.infoIpValid != 0, the IP address of the client
                                // otherwise not applicable/available
    uint32_t    data;       // additional event info data as specified by flags.info...
}TCPIP_DHCPS_EVENT_DATA_INFO;



// *****************************************************************************
/*
  Structure:
    TCPIP_DHCPS_EVENT_DATA

  Summary:
    DHCP Server event associated data.

  Description:
    DHCP server data that's passed when an event occurred

  Remarks:
    None
    
*/

typedef struct
{
    TCPIP_NET_HANDLE        hNet;       // Interface handle on which the event has occurred
                                        // 0 if not applicable
    uint32_t                evTime;     // the current time at which the event occurred                                        
    int16_t                 evType;     // a TCPIP_DHCPS_EVENT_TYPE value: the type of event that has occurred
    TCPIP_DHCPS_EVENT_DATA_INFO evInfo; // additional event data and flags for the validity of fields

    uint16_t                leaseIndex; // if flags.leaseIndexValid != 0, an index hat can be used for a query with TCPIP_DHCPS_LeaseGetInfo
                                        // otherwise not applicable/available
    TCPIP_MAC_ADDR          macAdd;     // if flags.macAddValid != 0, then the client MAC address for this event
                                        // otherwise not applicable/available
}TCPIP_DHCPS_EVENT_DATA;

// *****************************************************************************
/*
  Type:
    TCPIP_DHCPS_EVENT_HANDLER

  Summary:
    DHCP event handler prototype.

  Description:
    Prototype of a DHCP server event handler. Clients can register a handler with the
    DHCP service. Once an DHCP event occurs the DHCP service will called the
    registered handler.

  Parameters:
    evData  - data associated with this event
    param   - user supplied handle parameter
              Not used by the DHCP module.

  Remarks:
    The handler has to be short and fast. It is meant for
    setting an event flag, <i>not</i> for lengthy processing!

    evData is available only in the context of the event handler call!
    User should make a copy of the needed fields for future use.
 */

typedef void    (*TCPIP_DHCPS_EVENT_HANDLER)(TCPIP_DHCPS_EVENT_DATA* evData, const void* param);

// *****************************************************************************
/*
  Enumeration:
    TCPIP_DHCPS_LEASE_STATE

  Summary:
    DHCP Server lease state.

  Description:
    DHCP server state associated with a specific lease in the server database

  Remarks:
    8 bit values only

*/
typedef enum
{
    TCPIP_DHCPS_LEASE_STATE_IDLE      = 0,    // none/invalid state

    TCPIP_DHCPS_LEASE_STATE_SEND_PROBE,       // send probe with IP address (ICMP echo)
    TCPIP_DHCPS_LEASE_STATE_WAIT_PROBE,       // wait for ICMP probe result
    TCPIP_DHCPS_LEASE_STATE_REPROBE,          // probe failed, re-issue new IP address and probe
    TCPIP_DHCPS_LEASE_STATE_SEND_OFFER,       // offer address to the client
    TCPIP_DHCPS_LEASE_STATE_OFFERED,          // address offered, waiting for the client request
    TCPIP_DHCPS_LEASE_STATE_BOUND,            // client bound to this IP address
    TCPIP_DHCPS_LEASE_STATE_RELEASED,         // client released the IP address
    TCPIP_DHCPS_LEASE_STATE_EXPIRED,          // client lease expired 
    TCPIP_DHCPS_LEASE_STATE_INFORM,           // client acquired an IP address by some other means and informed the server

}TCPIP_DHCPS_LEASE_STATE;

// *****************************************************************************
/*
  Structure:
    TCPIP_DHCPS_LEASE_INFO

  Summary:
    DHCP Server module lease data.

  Description:
    DHCP server lease info data

  Remarks:
    The clientId field should hold at least TCPIP_DHCPS_CLIENT_ID_MAX_SIZE characters
    otherwise truncation will occur!
*/
typedef struct
{
    TCPIP_NET_HANDLE hNet;          // Interface on which this lease is granted
    IPV4_ADDR       ipAddress;      // leased IP address
    uint32_t        leaseTime;      // remaining lease time
    TCPIP_MAC_ADDR  macAdd;         // Client MAC address
    uint8_t         leaseState;     // A TCPIP_DHCPS_LEASE_STATE value, the current state 
    uint8_t         clientIdLen;    // on input this stores the size of the clientId
                                    // on return, this field stores the actual client ID size
    uint8_t         clientId[];     // the client ID used for this lease
                                    // should hold >= than TCPIP_DHCPS_CLIENT_ID_MAX_SIZE
                                    // otherwise it will be truncated
                                    // normally a hardware MAC address
                                    // but clients can use other IDs
}TCPIP_DHCPS_LEASE_INFO;

// *****************************************************************************
/*
  Structure:
    TCPIP_DHCPS_LEASE_SET

  Summary:
    DHCP Server module lease set data.

  Description:
    Structure used to set a DHCP server lease

  Remarks:
    If clientIdLen > than TCPIP_DHCPS_CLIENT_ID_MAX_SIZE characters
    then truncation will occur!
*/
typedef struct
{
    IPV4_ADDR       ipAddress;      // IP address to set
    uint32_t        leaseTime;      // lease time
                                    // use 0xffffffff for permanent lease
    TCPIP_MAC_ADDR  macAdd;         // client MAC address to set
    uint8_t         leaseState;     // The state of the lease.
                                    // Normally should be TCPIP_DHCPS_LEASE_STATE_BOUND
                                    // Only TCPIP_DHCPS_LEASE_STATE_BOUND, TCPIP_DHCPS_LEASE_STATE_RELEASED or TCPIP_DHCPS_LEASE_STATE_EXPIRED allowed
    uint8_t         clientIdLen;    // the size of the clientId
    uint8_t         clientId[];     // the client ID used for this lease
}TCPIP_DHCPS_LEASE_SET;

// *****************************************************************************
/*
  Structure:
    TCPIP_DHCPS_STATISTICS_DATA

  Summary:
    DHCP Server module statistics data.

  Description:
    DHCP server statistics maintained at run time

  Remarks:
    The symbol TCPIP_DHCPS_ENABLE_STATISTICS needs to be defined and != 0
*/
typedef struct  __attribute__((packed)) 
{
    uint16_t        releasedDelCount;   // number of released DB entries that have been deleted to make room for other clients
    uint16_t        expiredDelCount;    // number of expired DB entries that have been deleted to make room for other clients
    uint16_t        cacheFullCount;     // number of times the allocation of a new hash entry in the DB failed
    uint16_t        poolEmptyCount;     // number of times the allocation of a new IP address from the pool failed
    uint16_t        declinedCount;      // number of times an IP address has been declined by a client
    uint16_t        arpFailCount;       // number of times an ARP set operation failed
    uint16_t        echoFailCount;      // number of times an ping operation failed 
    uint16_t        icmpFailCount;      // number of times ICMP rejected a ping operation
    uint16_t        msgOvflCount;       // number of times a DHCP packet > then the reserved buffer for it was received
    uint16_t        icmpProbeCount;     // number of ICMP probes successfully sent
    uint16_t        arpInjectCount;     // number of ARP injections to send replies directly to clients
    uint16_t        sktNotReadyCount;   // number of failed send messagges because of no socket TX buffer
}TCPIP_DHCPS_STATISTICS_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: DHCP Server Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Function:
    bool TCPIP_DHCPS_IsEnabled(TCPIP_NET_HANDLE hNet)

  Summary:
    Determines if the DHCP Server is enabled on the specified interface.

  Description:
    This function returns the current state of the DHCP Server on the specified 
    interface.

  Precondition:
    The DHCP Server module must be initialized.

  Parameters:
    hNet - Interface to query

  Returns:
    - true  - server is enabled on the selected interface
    - false - server is disabled; or wrong interface, service is down, etc.

  Remarks:
    In multi-threaded environments the returned information is info only
    and can change at any time.

*/
bool TCPIP_DHCPS_IsEnabled(TCPIP_NET_HANDLE hNet);

//*****************************************************************************
/*
  Function:
    TCPIP_DHCPS_RES TCPIP_DHCPS_Disable(TCPIP_NET_HANDLE hNet)

  Summary:
    Disables the DHCP Server for the specified interface.

  Description:
    This function disables the DHCP Server for the specified interface.
    If it is already disabled, no action is taken.

  Precondition:
    The DHCP Server module must be initialized.

  Parameters:
    hNet - Interface on which to disable the DHCP Server

  Returns:
    - TCPIP_DHCPS_RES_OK - the call is successful and the DHCP server has been disabled on the selected interface
    - < 0 - An error code if DHCP server could not be stopped, wrong interface, etc.
    - TCPIP_DHCPS_RES_ACCESS_LOCKED - if access to the DHCP server is locked by another thread and the the call can be retried.
                            Multi-threaded access is enabled (build symbol TCPIP_DHCPS_MULTI_THREADED_ACCESS != 0)

  Remarks:
    When the server is disabled, clients will still hold valid leases.
    The clients should eventually contact the server to renew their leases
    but this operation will fail if the server is disabled at that time.

 */
TCPIP_DHCPS_RES TCPIP_DHCPS_Disable(TCPIP_NET_HANDLE hNet);

//*****************************************************************************
/*
  Function:
    TCPIP_DHCPS_RES TCPIP_DHCPS_Enable(TCPIP_NET_HANDLE hNet)

  Summary:
    Enables the DHCP Server for the specified interface.

  Description:
    This function enables the DHCP Server for the specified interface, if it is disabled.
    If it is already enabled, no action is taken.

  Precondition:
    The DHCP Server module must be initialized.

  Parameters:
    hNet - Interface on which to enable the DHCP Server

  Returns:
    - TCPIP_DHCPS_RES_OK - the call is successful and the DHCP server has been enabled on the selected interface
    - < 0 - An error code if DHCP server could not be started, wrong interface, etc.
    - TCPIP_DHCPS_RES_ACCESS_LOCKED - if access to the DHCP server is locked by another thread and the the call can be retried.
                            Multi-threaded access is enabled (build symbol TCPIP_DHCPS_MULTI_THREADED_ACCESS != 0)
*/
TCPIP_DHCPS_RES TCPIP_DHCPS_Enable(TCPIP_NET_HANDLE hNet);

//*****************************************************************************
/*
  Function:
    TCPIP_DHCPS_RES  TCPIP_DHCPS_LeaseEntriesGet(TCPIP_NET_HANDLE netH, uint16_t* pLeases, uint16_t* pInUse);
        
  Summary:
    Get the total and in use number of lease entries for the selected interface.

  Description:
    This function returns the total and in use number of entries in the DB for the  selected interface.

  Precondition:
    The DHCP Server module must be initialized.

  Parameters:
    netH    - the requested interface
    pLeases - address to store the total number of leases
              Can be NULL if not needed
    pInUse  - address to store the number of in use lease entries
              Can be NULL if not needed


  Returns:
    - TCPIP_DHCPS_RES_OK - the call is successful and the number of entries in the DB for the selected interface
                           have been updated
    - < 0 - An error code if wrong interface, or DHCP server is not running on that interface
    - TCPIP_DHCPS_RES_ACCESS_LOCKED - if access to the DHCP server DB is locked by another thread and the the call can be retried.
                            Multi-threaded access is enabled (build symbol TCPIP_DHCPS_MULTI_THREADED_ACCESS != 0)

    

  Remarks:
    The returned value in pLeases should match the corresponding TCPIP_DHCPS_INTERFACE_CONFIG::leaseEntries; 

    
*/
TCPIP_DHCPS_RES  TCPIP_DHCPS_LeaseEntriesGet(TCPIP_NET_HANDLE netH, uint16_t* pLeases, uint16_t* pInUse);

//*****************************************************************************
/*
  Function:
    TCPIP_DHCPS_RES  TCPIP_DHCPS_LeaseGetInfo(TCPIP_NET_HANDLE netH, TCPIP_DHCPS_LEASE_INFO* pLeaseInfo, uint16_t leaseIx);
        
  Summary:
    Get the lease entry details for the requested lease index

  Description:
    This function returns a DB lease entry info for the selected index on the selected interface

  Precondition:
    The DHCP Server module must be initialized.

  Parameters:
    netH - the selected interface
    pLeaseInfo - address to store the lease entry details
                  Could be NULL if details not needed
    leaseIx - lease index 

  Returns:
    - TCPIP_DHCPS_RES_OK - the call is successful and the pLeaseInfo has been updated, if not NULL
    - < 0 - if wrong interface or DHCP server is not running on that interface, or wrong/unused index
    - TCPIP_DHCPS_RES_ACCESS_LOCKED - if access to the DHCP server DB is locked by another thread and the the call can be retried.
                            Multi-threaded access is enabled (build symbol TCPIP_DHCPS_MULTI_THREADED_ACCESS != 0)


  Remarks:
    None
    
*/
TCPIP_DHCPS_RES  TCPIP_DHCPS_LeaseGetInfo(TCPIP_NET_HANDLE netH, TCPIP_DHCPS_LEASE_INFO* pLeaseInfo, uint16_t leaseIx);

//******************************************************************************
/*
 Function:
    TCPIP_DHCPS_RES TCPIP_DHCPS_LeaseGetInfoById(TCPIP_NET_HANDLE netH, const uint8_t* clientId, size_t idLen, TCPIP_DHCPS_LEASE_INFO* pLeaseInfo);

  Summary:
    Get the lease entry details for the requested client ID

  Description:
    This function returns a DB lease entry info for the selected ID on the selected interface

  Precondition:
    The DHCP Server module should have been initialized.

  Parameters:
    netH        -  selected interface handle
    clientId    -  the client ID, usually a MAC address
    idLen       - length of the ID
    pLeaseInfo - address to store the lease entry details
                  Could be NULL if details not needed

  Returns:
    - TCPIP_DHCPS_RES_OK - the call is successful and the pLeaseInfo has been updated, if not NULL
    - < 0 - if wrong interface or DHCP server is not running on that interface, or wrong/unused index
    - TCPIP_DHCPS_RES_ACCESS_LOCKED - if access to the DHCP server DB is locked by another thread and the the call can be retried.
                            Multi-threaded access is enabled (build symbol TCPIP_DHCPS_MULTI_THREADED_ACCESS != 0)

  Remarks:
    The maximum size of the client ID should be TCPIP_DHCPS_CLIENT_ID_MAX_SIZE.
    Extra characters will be truncated.


    Usually the client ID is a MAC address.
    However some clients can use different, longer IDs.

*/

TCPIP_DHCPS_RES TCPIP_DHCPS_LeaseGetInfoById(TCPIP_NET_HANDLE netH, const uint8_t* clientId, size_t idLen, TCPIP_DHCPS_LEASE_INFO* pLeaseInfo);

//******************************************************************************
/*
 Function:
    TCPIP_DHCPS_RES TCPIP_DHCPS_LeaseRemove(TCPIP_NET_HANDLE netH, uint16_t leaseIx, bool keepAddBusy);

  Summary:
    Remove one entry from the DHCP server DB

  Description:
    This function is used to remove one entry from the DB of the selected interface

  Precondition:
    The DHCP Server module should have been initialized.

  Parameters:
    netH    -   selected interface handle
    leaseIx - lease index 
    keepAddBusy - if true, the corresponding IP address (if the entry exists) will continue to be marked as busy
                    and it won't be used for other client leases
                - otherwise, the IP address will be marked as available
                    This should be the default

  Returns:
    - TCPIP_DHCPS_RES_OK - if the selected entry was found and deleted from the selected interface
    - < 0 - if wrong interface or DHCP server is not running on that interface, or wrong/unused index
    - TCPIP_DHCPS_RES_ACCESS_LOCKED - if access to the DHCP server DB is locked by another thread and the the call can be retried.
                            Multi-threaded access is enabled (build symbol TCPIP_DHCPS_MULTI_THREADED_ACCESS != 0)

  Remarks:
    
    Function exists only when build symbol TCPIP_DHCPS_DYNAMIC_DB_ACCESS != 0 

    The function will delete permanent leases too.

    When a server lease is removed, a corresponding client (if exists) will still hold a valid lease.
    When the client eventually contacts the server to renew its lease
    the operation will fail as the server has no knwoledge of the client lease.

*/

TCPIP_DHCPS_RES TCPIP_DHCPS_LeaseRemove(TCPIP_NET_HANDLE netH, uint16_t leaseIx, bool keepAddBusy);

//******************************************************************************
/*
 Function:
    TCPIP_DHCPS_RES TCPIP_DHCPS_LeaseRemoveById(TCPIP_NET_HANDLE netH, const uint8_t* clientId, size_t idLen, bool keepAddBusy);

  Summary:
    Remove one entry from the DHCP server DB

  Description:
    This function is used to remove one entry from the DB of the selected interface

  Precondition:
    The DHCP Server module should have been initialized.

  Parameters:
    netH        -  selected interface handle
    clientId    -  the client ID, usually a MAC address
    idLen       - length of the ID
    keepAddBusy - if true, the corresponding IP address (if the entry exists) will continue to be marked as busy
                    and it won't be used for other client leases
                - otherwise, the IP address will be marked as available
                    This should be the default

  Returns:
    - TCPIP_DHCPS_RES_OK - if the selected entry was found and deleted from the selected interface
    - < 0 - if wrong interface or DHCP server is not running on that interface, or client entry not found
    - TCPIP_DHCPS_RES_ACCESS_LOCKED - if access to the DHCP server DB is locked by another thread and the the call can be retried.
                            Multi-threaded access is enabled (build symbol TCPIP_DHCPS_MULTI_THREADED_ACCESS != 0)


  Remarks:
    The maximum size of the client ID should be TCPIP_DHCPS_CLIENT_ID_MAX_SIZE.
    Extra characters will be truncated.

    Usually the client ID is a MAC address.
    However some clients can use different, longer IDs.

    Function exists only when build symbol TCPIP_DHCPS_DYNAMIC_DB_ACCESS != 0 

    The function will delete permanent leases too.

    When a server lease is removed, a corresponding client (if exists) will still hold a valid lease.
    When the client eventually contacts the server to renew its lease
    the operation will fail as the server has no knwoledge of the client lease.

*/

TCPIP_DHCPS_RES TCPIP_DHCPS_LeaseRemoveById(TCPIP_NET_HANDLE netH, const uint8_t* clientId, size_t idLen, bool keepAddBusy);

//******************************************************************************
/*
 Function:
    TCPIP_DHCPS_RES TCPIP_DHCPS_LeaseRemoveAll(TCPIP_NET_HANDLE netH, bool keepPerm, bool keepAddBusy);

  Summary:
    Remove all entries from the DHCP server DB

  Description:
    This function is used to remove all entries from the DB of the selected interface

  Precondition:
    The DHCP Server module should have been initialized.

  Parameters:
    netH        - selected interface handle
    keepPerm    - if true, the permanent entries will not be deleted
                - otherwise the permanent entries will be deleted as well
    keepAddBusy - if true, the corresponding IP address (if the entry exists) will continue to be marked as busy
                    and it won't be used for other client leases
                - otherwise, the IP address will be marked as available
                    This should be the default

  Returns:
    - TCPIP_DHCPS_RES_OK - if the operation completed successfully
    - < 0 - if wrong interface or DHCP server is not running on that interface
    - TCPIP_DHCPS_RES_ACCESS_LOCKED - if access to the DHCP server DB is locked by another thread and the the call can be retried.
                            Multi-threaded access is enabled (build symbol TCPIP_DHCPS_MULTI_THREADED_ACCESS != 0)

  Remarks:
    
    Function exists only when build symbol TCPIP_DHCPS_DYNAMIC_DB_ACCESS != 0 

    When the server leases are removed, the corresponding clients (if they exist) will still hold valid leases.
    When the clients eventually contact the server to renew their leases
    the operation will fail as the server has no more knwoledge of the client lease.
*/

TCPIP_DHCPS_RES TCPIP_DHCPS_LeaseRemoveAll(TCPIP_NET_HANDLE netH, bool keepPerm, bool keepAddBusy);

//******************************************************************************
/*
 Function:
    TCPIP_DHCPS_RES TCPIP_DHCPS_LeaseAddById(TCPIP_NET_HANDLE netH, const TCPIP_DHCPS_LEASE_SET* pLeaseSet);

  Summary:
    Add a lease to the DHCP server DB

  Description:
    This function is used to add one entry to the DB of the selected interface

  Precondition:
    The DHCP Server module should have been initialized.

  Parameters:
    netH        -  selected interface handle
    pLeaseSet   -  the lease data to be set

  Returns:
    - TCPIP_DHCPS_RES_OK - if the selected lease was found and updated
    - < 0 - if wrong interface or DHCP server is not running on that interface, or lease could not be added
    - TCPIP_DHCPS_RES_ACCESS_LOCKED - if access to the DHCP server DB is locked by another thread and the the call can be retried.
                            Multi-threaded access is enabled (build symbol TCPIP_DHCPS_MULTI_THREADED_ACCESS != 0)


  Remarks:
    The maximum size of the client ID should be TCPIP_DHCPS_CLIENT_ID_MAX_SIZE.
    Extra characters will be truncated.

    Function exists only when build symbol TCPIP_DHCPS_DYNAMIC_DB_ACCESS != 0 

    An existing lease will be overwritten by this call.
    If the lease does not exist, it will be added.
*/

TCPIP_DHCPS_RES TCPIP_DHCPS_LeaseAddById(TCPIP_NET_HANDLE netH, const TCPIP_DHCPS_LEASE_SET* pLeaseSet);


//******************************************************************************
/*
 Function:
    TCPIP_DHCPS_RES TCPIP_DHCPS_Configure(const TCPIP_DHCPS_INTERFACE_CONFIG* pIfConfig, uint16_t nConfigs);

  Summary:
    Configure the DHCP server on multiple interfaces

  Description:
    This function is used to configure the DHCP server at run time for the selected interfaces

  Precondition:
    The DHCP Server module should have been initialized.

  Parameters:
    pIfConfig   -  pointer to an array of configuration parameters
    nConfigs    -  number of configurations in the pIfConfig array

  Returns:
    - TCPIP_DHCPS_RES_OK - if the interfaces were properly configured
    - < 0 - if wrong interface or allocation failed, etc.
    - TCPIP_DHCPS_RES_ACCESS_LOCKED - if access to the DHCP server is locked by another thread and the the call should be retried.
                            Multi-threaded access is enabled (build symbol TCPIP_DHCPS_MULTI_THREADED_ACCESS != 0)


  Remarks:
    The DHCP server for the selected interfaces will be completely reconfigured by this call.
    All the information about the existing leases will be lost.

    Preferrably the call should be done with the DHCP server disabled.


    The call will attempt to lock access to the DHCP server DB.
    During this time, the server will not reply to clients and any received packets
    are ignored.

*/

TCPIP_DHCPS_RES TCPIP_DHCPS_Configure(const TCPIP_DHCPS_INTERFACE_CONFIG* pIfConfig, uint16_t nConfigs);


//******************************************************************************
/*
 Function:
    TCPIP_DHCPS_RES TCPIP_DHCPS_StatisticsDataGet(TCPIP_NET_HANDLE netH, TCPIP_DHCPS_STATISTICS_DATA* pStatData);

  Summary:
    Returns the DHCPS statistics data

  Description:
    This function is used to return the DHCPS statistics data

  Precondition:
    The DHCP Server module should have been initialized.

  Parameters:
    netH        -  selected interface handle
    pStatData   - address to store the statistics
                  Could be NULL

  Returns:
    - TCPIP_DHCPS_RES_OK - if the DHCP server runs on the selected interface and statistics are copied to pStatData
    - < 0 - if wrong interface or DHCP server is not running on that interface
    - TCPIP_DHCPS_RES_ACCESS_LOCKED - if access to the DHCP server DB is locked by another thread and the the call can be retried.
                            Multi-threaded access is enabled (build symbol TCPIP_DHCPS_MULTI_THREADED_ACCESS != 0)


  Remarks:
    The symbol TCPIP_DHCPS_ENABLE_STATISTICS needs to be defined and != 0
    for this function to exist.

*/

TCPIP_DHCPS_RES TCPIP_DHCPS_StatisticsDataGet(TCPIP_NET_HANDLE netH, TCPIP_DHCPS_STATISTICS_DATA* pStatData);


// *****************************************************************************
/* Function:
    TCPIP_DHCPS_EVENT_HANDLE TCPIP_DHCPS_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_DHCPS_EVENT_HANDLER handler, 
                               const void* hParam)

  Summary:
    Registers a DHCP server Handler.

  Description:
    This function registers a DHCP event handler.
    The DHCP module will call the registered handler when a
    DHCP event (TCPIP_DHCPS_EVENT_TYPE) occurs.

  Precondition:
    The DHCPs module must be initialized.

  Parameters:
    hNet    - Interface handle.
              Use hNet == 0 to register on all interfaces available.
    handler - Handler to be called when a DHCP event occurs.
    hParam  - Parameter to be used in the handler call.
              This is user supplied and is not used by the DHCP module.


  Returns:
    Returns a valid handle if the call succeeds, or a null handle if
    the call failed (out of memory, for example).

  Remarks:
    The handler has to be short and fast. It is meant for
    setting an event flag, not for lengthy processing!

    The hParam is passed by the client and will be used by the DHCP when the
    notification is made. It is used for per-thread content or if more modules,
    for example, share the same handler and need a way to differentiate the
    callback.
 */

TCPIP_DHCPS_EVENT_HANDLE TCPIP_DHCPS_HandlerRegister(TCPIP_NET_HANDLE hNet, TCPIP_DHCPS_EVENT_HANDLER handler, const void* hParam);


// *****************************************************************************
/* Function:
    bool TCPIP_DHCPS_HandlerDeRegister(TCPIP_DHCPS_EVENT_HANDLE hDhcp)

  Summary:
    Deregisters a previously registered DHCP handler.
    
  Description:
    This function deregisters the DHCP event handler.

  Precondition:
    The DHCP module must be initialized.

  Parameters:
    hDhcp   - A handle returned by a previous call to TCPIP_DHCPS_HandlerRegister.

  Returns:
    - true  - if the call succeeds
    - false - if no such handler is registered
 */

bool             TCPIP_DHCPS_HandlerDeRegister(TCPIP_DHCPS_EVENT_HANDLE hDhcp);


// *****************************************************************************
/*
  Function:
    void  TCPIP_DHCPS_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs DHCP Server module tasks in the TCP/IP stack.

  Precondition:
    The DHCP Server module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_DHCPS_Task(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // __DHCP_SERVER_H
