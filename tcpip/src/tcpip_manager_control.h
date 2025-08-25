/*******************************************************************************
  Microchip TCP/IP Stack Definitions

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_manager_control.h

  Summary:
    
  Description:
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

#ifndef H_TCPIP_MANAGER_CONTROL_H_
#define H_TCPIP_MANAGER_CONTROL_H_

#include "tcpip/tcpip_mac.h"

// supported ETH frame types that are processed by the stack
//

#define TCPIP_ETHER_TYPE_IPV4       (0x0800u)
#define TCPIP_ETHER_TYPE_IPV6       (0x86DDu)
#define TCPIP_ETHER_TYPE_ARP        (0x0806u)
#define TCPIP_ETHER_TYPE_LLDP       (0x88CCu)
#define TCPIP_ETHER_TYPE_C_TPID     (0x8100u)   // C-VLAN TPID
#define TCPIP_ETHER_TYPE_UNKNOWN    (0xFFFFu)

// minimum timeout (maximum rate) for link check, ms
// 5 times per second should be frequent enough
#define M_TCPIP_STACK_LINK_MIN_TMO       200 
// adjust it vs the stack rate

#if (!defined(TCPIP_STACK_LINK_RATE) || TCPIP_STACK_LINK_RATE < TCPIP_STACK_TICK_RATE || TCPIP_STACK_LINK_RATE < M_TCPIP_STACK_LINK_MIN_TMO) 
#define M_TCPIP_STACK_LINK_RATE  M_TCPIP_STACK_LINK_MIN_TMO   // use the minimum value
#else
#define M_TCPIP_STACK_LINK_RATE  TCPIP_STACK_LINK_RATE       // user value
#endif

// check mask for valid vlan Id - 12 bits allowed 
#define M_TCPIP_STACK_VLAN_ID_MASK  0xf000U
// check mask for valid vlan PCP - 3 bits allowed 
#define M_TCPIP_STACK_VLAN_PCP_MASK 0xf8U

// module signal/timeout/asynchronous event handler
// the stack manager calls it when there's an signal/tmo/asynchronous event pending
// it should clear the pending status
typedef void    (*tcpipModuleSignalHandler)(void);

// types of packets/frames processed by this stack
typedef struct
{
    uint16_t        frameType;      // one of valid TCPIP_ETHER_TYPE_ values 
    uint16_t        moduleId;       // TCPIP_STACK_MODULE: corresponding module handling this
    uint32_t        pktTypeFlags;   // user packet flags to set for this type

}TCPIP_FRAME_PROCESS_ENTRY;


// handle for the registration of a signal handler
typedef const void* TCPIP_SIGNAL_HANDLE;



typedef enum
{
    TCPIP_STACK_ADDR_SRVC_NONE    = 0,    // no service
    TCPIP_STACK_ADDR_SRVC_DHCPC,          // DHCP client
    TCPIP_STACK_ADDR_SRVC_ZCLL,           // ZCLL
    TCPIP_STACK_ADDR_SRVC_DHCPS,          // DHCP server
    //
    TCPIP_STACK_ADDR_SRVC_MASK    = 0x7,  // mask of address services
                                                // has to to match the position in the TCPIP_STACK_NET_IF_FLAGS!!!
}TCPIP_STACK_ADDR_SRVC_TYPE;

typedef enum
{
    TCPIP_STACK_DNS_SERVICE_NONE    = 0,        // no service
    TCPIP_STACK_DNS_SERVICE_CLIENT,             // DNS Client
    TCPIP_STACK_DNS_SERVICE_SERVER,             // DNS Server
    TCPIP_STACK_DNS_SERVICE_MASK    = 0x18,     // mask of DNS services
                                                // has to to match the position in the TCPIP_STACK_NET_IF_FLAGS!!!
}TCPIP_STACK_DNS_SERVICE_TYPE;

typedef enum
{
    TCPIP_ADDR_SRVC_EV_NONE    = 0,      // no event
    TCPIP_ADDR_SRVC_EV_RUN_FAIL,         // run time failure event
    TCPIP_ADDR_SRVC_EV_CONN_LOST,        // conn lost event
    TCPIP_ADDR_SRVC_EV_USER_STOP,        // user stop event
    TCPIP_ADDR_SRVC_EV_RUN_RESTORE,      // run time restore event event
                                                        // DHCPc uses this
}TCPIP_ADDR_SRVC_EV;

typedef union
{
    struct __attribute__((packed))
    {
        // stack configuration dynamic flags
        unsigned bIsDHCPEnabled         : 1;    // init: controls the DHCP enable/disable on the interface
                                                // runtime: mirror bit set by the DHCP to reflect the current/last status
        unsigned bIsZcllEnabled         : 1;    // init: controls the ZCLL service enable/disable on the interface
                                                // runtime: mirror bit set by the ZCLL Server to reflect the current/last status
        unsigned bIsDHCPSrvEnabled      : 1;    // init: controls the DHCP Server enable/disable on the interface
                                                // runtime: mirror bit set by the DHCP Server to reflect the current/last status
                                                // TCPIP_STACK_ADDR_SRVC_MASK has to match!!!
        unsigned bIsDnsClientEnabled    : 1;    // DNS client  enable/disable  .
        unsigned bIsDnsServerEnabled    : 1;    // DNS server Enable and Disable
        unsigned bIsDNSServerAuto       : 1;    // DNS Server auto enable/disable on this interface
        unsigned bInterfaceEnabled      : 1;    // 0 when TCPIP_MAC_POWER_DOWN/TCPIP_MAC_POWER_LOW 
        unsigned bIPv6Enabled           : 1;
        unsigned bIPv6InConfig          : 1;
        unsigned bIpv6DnsValid          : 1;    // IPv6 DNS address is valid flag
        unsigned powerMode              : 2;    // current power mode
        unsigned bInConfig              : 1;    // stack is configuring itself
        unsigned bMacProcessOnEvent     : 1;    // TCPIP_MAC_Process function needed on event
        unsigned bMacInitialize         : 1;    // MAC is initializing
        unsigned bMacInitDone           : 1;    // MAC initializing done
    };
    uint16_t v;
}TCPIP_STACK_NET_IF_FLAGS;      // flags per interface

// network interface descriptor
// 4 bytes aligned!
typedef struct
{
    uint16_t        size;                   // structure size; used in the configuration save/restore
    uint16_t        macId;                  // corresponding MAC id
    IPV4_ADDR       netIPAddr;              // IP address; currently only one IP add per interface
    IPV4_ADDR       netMask;                // Subnet mask
    IPV4_ADDR       netGateway;             // Gateway
    IPV4_ADDR       dnsServer[2];           // Primary + Secondary DNS Servers
    IPV4_ADDR       DefaultIPAddr;          // Default IP address
    IPV4_ADDR       DefaultMask;            // Default subnet mask
    IPV4_ADDR       DefaultGateway;         // Default Gateway
    IPV4_ADDR       DefaultDNSServer[2];    // Default DNS Servers; primary and secondary
    uint8_t         NetBIOSName[16];        // NetBIOS name
    TCPIP_MAC_ADDR  netMACAddr;             // MAC address
    TCPIP_STACK_NET_IF_FLAGS Flags;
    const TCPIP_MAC_OBJECT*  pMacObj;   // MAC object associated with this interface
#if defined(TCPIP_STACK_USE_IPV6)
    uint16_t        startFlags;     // TCPIP_NETWORK_CONFIG_FLAGS: flags for interface start up
    uint16_t        ipv6PrefixLen;  // IPv6 subnet ID
    IPV6_ADDR       netIPv6Addr;    // static IPv6 address
    IPV6_ADDR       netIPv6Gateway; // default IPv6 gateway
    IPV6_ADDR       netIPv6Dns[1];  // default IPv6 DNS server; only one value maintained as default/current, as an IPv6 address is fairly big
#if defined(TCPIP_IPV6_G3_PLC_BORDER_ROUTER) && (TCPIP_IPV6_G3_PLC_BORDER_ROUTER != 0)
    uint32_t        advLastSec;     // last second count when advertisement was evaluated
    uint16_t        advTmo;         // advertising timeout, seconds; when expires (reaches 0) new advertisement needs to be sent
    uint16_t        advInitCount;   // number of initial advertisements [0, MAX_INITIAL_RTR_ADVERTISEMENTS]
#endif  // defined(TCPIP_IPV6_G3_PLC_BORDER_ROUTER) && (TCPIP_IPV6_G3_PLC_BORDER_ROUTER != 0)
#else
    uint16_t        startFlags;     // TCPIP_NETWORK_CONFIG_FLAGS: flags for interface start up
    uint16_t        pad;            // not used, padding
#endif
#if (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0) 
#if ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VSTAT) != 0) 
    uint32_t        vlanTxCnt;
    uint32_t        vAdjustTxCnt;
    uint32_t        vOkTxCnt;
    uint32_t        vRxTxCnt;
    uint32_t        vlanRxHitCnt;
    uint32_t        vlanRxMissCnt;
    uint32_t        vUntaggedRxCnt;
#endif  // ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VSTAT) != 0) 
    uint16_t        vlanId;         // VID for this interface
    uint8_t         vlanPcp;        // VLAN PCP for this interface
#endif  // (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0) 
}TCPIP_STACK_NET_IF_DCPT;



#include "tcpip/src/tcpip_helpers_private.h"

// TCPIP network structure containing interface information
//  
typedef struct S_tag_TCPIP_NET_IF
{
    struct  // TCPIP_STACK_NET_IF_DCPT!!!; Anonymous for now;
    // KEEP IN SYNC! ALIGNED! ALWAYS AT BEGINNING!
    {
        uint16_t        size;                   // structure size; used in the configuration save/restore
        uint16_t        macId;                  // corresponding MAC id
        IPV4_ADDR       netIPAddr;              // IP address; currently only one IP add per interface
        IPV4_ADDR       netMask;                // Subnet mask
        IPV4_ADDR       netGateway;             // Gateway
        IPV4_ADDR       dnsServer[2];           // Primary + Secondary DNS Servers
        IPV4_ADDR       DefaultIPAddr;          // Default IP address
        IPV4_ADDR       DefaultMask;            // Default subnet mask
        IPV4_ADDR       DefaultGateway;         // Default Gateway
        IPV4_ADDR       DefaultDNSServer[2];    // Default DNS Servers; primary and secondary
        uint8_t         NetBIOSName[16];        // NetBIOS name
        TCPIP_MAC_ADDR  netMACAddr;             // MAC address
        TCPIP_STACK_NET_IF_FLAGS Flags;
        const TCPIP_MAC_OBJECT*  pMacObj;   // MAC object associated with this interface
#if defined(TCPIP_STACK_USE_IPV6)
        uint16_t        startFlags;  // TCPIP_NETWORK_CONFIG_FLAGS: flags for interface start up
        uint16_t        ipv6PrefixLen;  // IPv6 subnet ID
        IPV6_ADDR       netIPv6Addr;    // static IPv6 address
        IPV6_ADDR       netIPv6Gateway; // default IPv6 gateway
        IPV6_ADDR       netIPv6Dns[1];  // default IPv6 DNS server; only one value maintained as default/current, as an IPv6 address is fairly big
#if defined(TCPIP_IPV6_G3_PLC_BORDER_ROUTER) && (TCPIP_IPV6_G3_PLC_BORDER_ROUTER != 0)
        uint32_t        advLastSec;     // last second count when advertisement was evaluated
        uint16_t        advTmo;         // advertising timeout, seconds; when expires (reaches 0) new advertisement needs to be sent
        uint16_t        advInitCount;   // number of initial advertisements [0, MAX_INITIAL_RTR_ADVERTISEMENTS]
#endif  // defined(TCPIP_IPV6_G3_PLC_BORDER_ROUTER) && (TCPIP_IPV6_G3_PLC_BORDER_ROUTER != 0)
#else
        uint16_t        startFlags;     // TCPIP_NETWORK_CONFIG_FLAGS: flags for interface start up
        uint16_t        pad;            // not used, padding
#endif
#if (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0) 
#if ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VSTAT) != 0) 
        uint32_t        vlanTxCnt;
        uint32_t        vAdjustTxCnt;
        uint32_t        vOkTxCnt;
        uint32_t        vRxTxCnt;
        uint32_t        vlanRxHitCnt;
        uint32_t        vlanRxMissCnt;
        uint32_t        vUntaggedRxCnt;
#endif  // ((M_TCPIP_STACK_DEBUG_LEVEL & M_TCPIP_STACK_DEBUG_MASK_VSTAT) != 0) 
        uint16_t        vlanId;         // VID for this interface
        uint8_t         vlanPcp;        // VLAN PCP for this interface
#endif  // (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0) 
    };

    // NOTE the alignmment!
    SYS_MODULE_OBJ      macObjHandle;           // MAC system wide handle
    TCPIP_MAC_HANDLE    hIfMac;                 // quick reference to which MAC this interface belongs to
    const void*         pMacConfig;             // MAC configuration data save for a restart
#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    struct S_tag_TCPIP_NET_IF* pPriIf;           // pointer to the primary interface
                                                // pPriIf == pIf, means primary
                                                // else alias  
    struct S_tag_TCPIP_NET_IF* pAlias;           // linked list of alias interfaces belonging to this primary
#endif  // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
                                                // ((__aligned__)) !!!
#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION) && (TCPIP_STACK_USER_NOTIFICATION != 0)
    PROTECTED_SINGLE_LIST         registeredClients;      // stack notification clients
                                                // ((__aligned__)) !!!
#endif  // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION) && (TCPIP_STACK_USER_NOTIFICATION != 0)
#if (TCPIP_STACK_EXTERN_PACKET_PROCESS != 0)
    TCPIP_STACK_PACKET_HANDLER      pktHandler;         // external packet handler
    const void*                     pktHandlerParam;    // user parameter
#endif  // (TCPIP_STACK_EXTERN_PACKET_PROCESS != 0)
                                                // ((__aligned__)) !!!
    uint16_t            netIfIx;                // index of this entry in the tcpipNetIf table.
                                                // netIfIx = (this - &tcpipNetIf[0])/sizeof(TCPIP_NET_IF)
                                                // stored to avoid the run time division
    uint16_t            activeEvents;           // TCPIP_MAC_EVENT accumulated event available
                                                // TCPIP_MAC_EVENT fits within 16 bits!
                                                // link up/down events are not 
                                                // necessarily triggered by hardware
    uint16_t            currEvents;             // current TCPIP_MAC_EVENT processed event
    uint16_t            linkMtu;                // current interface link MTU

    uint8_t             txOffload;              // MAC TX TCPIP_MAC_CHECKSUM_OFFLOAD_FLAGS        
    uint8_t             rxOffload;              // MAC RX TCPIP_MAC_CHECKSUM_OFFLOAD_FLAGS        
                                                // Not used. The MAC sets the pktFlags when calculating RX checksums!
    uint8_t             txPriNum;               // number of Tx priorities supported by MAC
    uint8_t             rxPriNum;               // number of Rx priorities supported by MAC
    union
    {
        struct __attribute__((packed))
        {
            unsigned linkPrev       : 1;        // previous status of the link
            unsigned connEvent      : 1;        // when set indicates a connection event
            unsigned connEventType  : 1;        // 0: TCPIP_MAC_EV_CONN_LOST; 1: TCPIP_MAC_EV_CONN_ESTABLISHED
            unsigned bridged        : 1;        // 1: interface is part of a bridge
            unsigned reserved       : 4;        // available
        };
        uint8_t        v;
    }exFlags;                               // additional extended flags      
    uint8_t             macType;            // a TCPIP_MAC_TYPE value: ETH, Wi-Fi, etc; 

#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    uint16_t            priIfIx;            // primary interface index, [0, nIfs - nAliases]
#endif // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    uint8_t             bridgePort;         // bridge port this interface belongs to; < 256
    char                ifName[7];          // native interface name + \0
} TCPIP_NET_IF;


// TCPIP stack event registration

typedef struct  S_TAG_TCPIP_LIST_NODE
{
    struct S_TAG_TCPIP_LIST_NODE *next;  // next node in list
                                                // safe cast to SGL_LIST_NODE node!!!
    TCPIP_STACK_EVENT_HANDLER   handler;    // handler to be called for event
    const void*                 hParam;     // handler parameter
    TCPIP_EVENT                 evMask;     // event that triggers the notification
    TCPIP_NET_IF*               pNetIf;     // interface it belongs to
}TCPIP_EVENT_LIST_NODE;


// network interface action for initialization/deinitialization
// 8 bit values only are supported!
typedef enum
{
    TCPIP_STACK_ACTION_INIT,         // stack is initialized 
    TCPIP_STACK_ACTION_REINIT,       // stack is reinitialized
    TCPIP_STACK_ACTION_DEINIT,       // stack is deinitialized
    TCPIP_STACK_ACTION_IF_UP,        // interface is brought up 
    TCPIP_STACK_ACTION_IF_DOWN,      // interface is brought down
}TCPIP_STACK_ACTION;


// data that's passed as reference to each other module init/deinit, etc.
typedef struct S_TCPIP_STACK_MODULE_CTRL
{
    // permanent data; this data is maintained by the stack for one full session
    // i.e., across StackInit() -> StackDeInit() calls
    // 
    //
    uint16_t    nIfs;       // number of the interfaces supported in this session
    uint16_t    nAliases;   // number of alias interfaces in this session         
    // number of the modules enabled in this session
    uint16_t    nModules;
    uint16_t    reserved;   // padding, not used
    // allocation parameters
    const void* memH;                   // handle to be used in the TCPIP_HEAP_ calls
    TCPIP_STACK_HEAP_TYPE   heapType;   // type of the heap                

    // transient data; contains info for a specific call
    //
    //
    // pointer to the current interface
    TCPIP_NET_IF* pNetIf;
    // index of the current interface addressed
    uint16_t    netIx;
    // current action for the stack
    uint8_t     stackAction;    // TCPIP_STACK_ACTION value
    // the power mode for this interface to go to
    // valid only if stackAction == init/reinit; ignored for deinit
    uint8_t     powerMode;    // TCPIP_MAC_POWER_MODE value
    
}TCPIP_STACK_MODULE_CTRL;


TCPIP_NET_IF*         TCPIP_STACK_IPAddToNet(IPV4_ADDR* pIpAddress, bool useDefault);

TCPIP_NET_IF*         TCPIPStackIpAddFromAnyNet(TCPIP_NET_IF* pNetIf, IPV4_ADDR* pIpAddress);


static __inline__ bool __attribute__((always_inline)) TCPIPStackIpAddFromLAN(const TCPIP_NET_IF* pIf, const IPV4_ADDR* pIpAddress)
{
    return (((pIf->netIPAddr.Val ^ pIpAddress->Val) & pIf->netMask.Val) == 0U);
}

int  TCPIP_STACK_NetIxGet(const TCPIP_NET_IF* pNetIf);

static __inline__ uint16_t __attribute__((always_inline)) TCPIPStackNetIxGet(const TCPIP_NET_IF* pIf)
{
    return pIf->netIfIx;
}

uint32_t  TCPIP_STACK_NetAddressGet(const TCPIP_NET_IF* pNetIf);

const IPV6_ADDR* TCPIP_STACK_NetStaticIPv6AddressGet(const TCPIP_NET_IF* pNetIf, uint8_t* pPrefixLen);

const IPV6_ADDR* TCPIP_STACK_NetDefaultIPv6GatewayGet(const TCPIP_NET_IF* pNetIf);


uint32_t  TCPIP_STACK_NetMaskGet(const TCPIP_NET_IF* pNetIf);

void  TCPIP_STACK_GatewayAddressSet(TCPIP_NET_IF* pNetIf, IPV4_ADDR* ipAddress);
void  TCPIP_STACK_PrimaryDNSAddressSet(TCPIP_NET_IF* pNetIf, IPV4_ADDR* ipAddress);

void  TCPIP_STACK_SecondaryDNSAddressSet(TCPIP_NET_IF* pNetIf, IPV4_ADDR* ipAddress);

TCPIP_NET_IF*   TCPIP_STACK_NetByAddress(const IPV4_ADDR* pIpAddress);

TCPIP_NET_IF*   TCPIP_STACK_MatchNetAddress(const TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pIpAdd);

bool  TCPIP_STACK_AddressIsOfNetUp(const TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pIpAdd);

// detects net-directed bcast
bool  TCPIP_STACK_NetIsBcastAddress(const TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pIpAdd);

// detects limited or net-directed bcast
bool  TCPIP_STACK_IsBcastAddress(const TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pIpAdd);

// detects limited or net-directed bcast
// assumes valid pNetIf
static __inline__ bool __attribute__((always_inline))  TCPIPStack_IsBcastAddress(const TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pIpAdd)
{
    return (pIpAdd->Val == 0xFFFFFFFFU) ||  (pIpAdd->Val == (pNetIf->netIPAddr.Val | ~pNetIf->netMask.Val));
}

// detects limited bcast
static __inline__ bool __attribute__((always_inline))  TCPIPStack_IsLimitedBcast(const IPV4_ADDR* pIpAdd)
{
    return (pIpAdd->Val == 0xFFFFFFFFU);
}

// detects net_directed bcast
// assumes valid pNetIf
static __inline__ bool __attribute__((always_inline))  TCPIPStack_IsDirectedBcast(const TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pIpAdd)
{
    return (pIpAdd->Val == (pNetIf->netIPAddr.Val | ~pNetIf->netMask.Val));
}

// local/non forwardable multicast address (in range [224.0.0.0, 224.0.0.255])
// return true if local, i.e. non-forwardable
// false otherwise
// assumes the address is a multicast address (starts with 0b1110 prefix)!
static __inline__ bool __attribute__((always_inline))  TCPIPStack_IsLocalMcast(const IPV4_ADDR* pIpAdd)
{
    return ((pIpAdd->Val & 0x00ffffe0U) == 0x000000e0U);
}



bool                TCPIP_STACK_NetworkIsLinked(const TCPIP_NET_IF* pNetIf);

TCPIP_NET_IF*       TCPIP_STACK_MACIdToNet(TCPIP_STACK_MODULE macId);

TCPIP_STACK_MODULE  TCPIP_STACK_NetMACId(const TCPIP_NET_IF* pNetIf);

TCPIP_NET_IF*       TCPIP_STACK_MacToNet(TCPIP_MAC_HANDLE hMac);

TCPIP_MAC_HANDLE    TCPIP_STACK_NetToMAC(const TCPIP_NET_IF* pNetIf);

int                 TCPIP_STACK_MacToNetIndex(TCPIP_MAC_HANDLE hMac);



const uint8_t*  TCPIP_STACK_NetUpMACAddressGet(const TCPIP_NET_IF* pNetIf);

static __inline__ uint32_t  __attribute__((always_inline)) TCPIPStackNetAddress(const TCPIP_NET_IF* pNetIf)
{
    return pNetIf->netIPAddr.Val;
}

static __inline__ uint32_t  __attribute__((always_inline)) TCPIPStackNetMask(const TCPIP_NET_IF* pNetIf)
{
    return pNetIf->netMask.Val;
}

static __inline__ uint32_t  __attribute__((always_inline)) TCPIPStackNetGateway(const TCPIP_NET_IF* pNetIf)
{
    return pNetIf->netGateway.Val;
}

static __inline__ uint32_t  __attribute__((always_inline)) TCPIPStackNetNetwork(const TCPIP_NET_IF* pNetIf)
{
    return (pNetIf->netIPAddr.Val & pNetIf->netMask.Val);
}

// returns the host part of an IPv4 address
static __inline__ uint32_t  __attribute__((always_inline)) TCPIPStackHostPartAddress(const TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pIpAdd)
{
    return pIpAdd->Val & (~pNetIf->netMask.Val);
}

// returns the network part of an IPv4 address
static __inline__ uint32_t  __attribute__((always_inline)) TCPIPStackNetPartAddress(const TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pIpAdd)
{
    return pIpAdd->Val & pNetIf->netMask.Val;
}

static __inline__ bool  __attribute__((always_inline)) TCPIP_STACK_AddressIsOfNet(const TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pIpAdd)
{
    return ((pIpAdd->Val == 0x0100007fU) || (pNetIf->netIPAddr.Val == pIpAdd->Val));
}

// conversion functions/helpers

static __inline__ void* __attribute__((always_inline)) FC_NetDcpt2Vptr(TCPIP_STACK_NET_IF_DCPT* pNetDcpt)
{
    union
    {
        TCPIP_STACK_NET_IF_DCPT*    pNetDcpt;
        void*                       vptr;
    }U_NET_DCPT_VPTR;

    U_NET_DCPT_VPTR.pNetDcpt = pNetDcpt;
    return U_NET_DCPT_VPTR.vptr;
}

// convert net Handle to netIf
static __inline__ TCPIP_NET_IF* __attribute__((always_inline)) FC_NetH2NetIf(TCPIP_NET_HANDLE netH)
{
        union
        {
            TCPIP_NET_HANDLE netH;
            TCPIP_NET_IF* pNetIf;
        }U_NET_HNDL_NET_IF;        

    U_NET_HNDL_NET_IF.netH = netH;
    return U_NET_HNDL_NET_IF.pNetIf;
}

// remove const of net
static __inline__ TCPIP_NET_IF* __attribute__((always_inline)) FC_CNetIf2NetIf(const TCPIP_NET_IF* cNetIf)
{
    union
    {
        const TCPIP_NET_IF*  cNetIf;
        TCPIP_NET_IF* pNetIf;
    }U_CNET_IF_NET_IF;

    U_CNET_IF_NET_IF.cNetIf = cNetIf;
    return U_CNET_IF_NET_IF.pNetIf;
}

static __inline__ TCPIP_NET_IF* __attribute__((always_inline)) FC_Cvptr2NetIf(const void* cvptr)
{
        union
        {
            const void*  cvptr;
            TCPIP_NET_IF* pNetIf;
        }U_CVPTR_NET_IF;        

    U_CVPTR_NET_IF.cvptr = cvptr;
    return U_CVPTR_NET_IF.pNetIf;
}

static __inline__  const void* __attribute__((always_inline)) FC_Net2Cvptr(const TCPIP_NET_IF* pNetIf)
{
    union
    {
        const TCPIP_NET_IF* pNetIf;
        const void*   cvptr;
    }U_NET_CV_PTR;

    U_NET_CV_PTR.pNetIf = pNetIf;
    return U_NET_CV_PTR.cvptr; 
}

// end of conversion functions/helpers

// stack internal helpers
static __inline__ TCPIP_NET_IF*  __attribute__((always_inline)) TCPIPStackHandleToNet(TCPIP_NET_HANDLE hNet)
{
    // do some checking
    // if #debug enabled, etc
    TCPIP_NET_IF* pNetIf = FC_NetH2NetIf(hNet); 
    return pNetIf;
}

// more checking, for user passed handles
TCPIP_NET_IF*  TCPIP_Stack_UserHandleToNet(TCPIP_NET_HANDLE hNet);


static __inline__ bool  __attribute__((always_inline)) TCPIP_STACK_NetworkIsUp(const TCPIP_NET_IF* pNetIf)
{
    if((pNetIf != NULL) && (pNetIf->Flags.bInterfaceEnabled != 0U))
    {
        return true;
    }
    return false;
}

static __inline__ const uint8_t*  __attribute__((always_inline)) TCPIPStack_NetMACAddressGet(const TCPIP_NET_IF* pNetIf)
{
    return (pNetIf != NULL) ? pNetIf->netMACAddr.v : NULL;
}

static __inline__ const char*  __attribute__((always_inline)) TCPIPStack_NetBIOSNameGet(const TCPIP_NET_IF* pNetIf)
{
    return (pNetIf != NULL) ? (const char*)pNetIf->NetBIOSName : NULL;
}

static __inline__ char*  __attribute__((always_inline)) TCPIPStack_NetBIOSName(TCPIP_NET_IF* pNetIf)
{
    return (pNetIf != NULL) ? (char*)pNetIf->NetBIOSName : NULL;
}

static __inline__ TCPIP_MAC_TYPE __attribute__((always_inline)) TCPIPStack_NetMacType(const TCPIP_NET_IF* pNetIf)
{
    return (pNetIf != NULL) ? (TCPIP_MAC_TYPE)pNetIf->macType : (TCPIP_MAC_TYPE)0;
}

// checks for valid up interface
static __inline__ TCPIP_NET_IF*  __attribute__((always_inline)) TCPIPStackHandleToNetUp(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNet(hNet);
    if(TCPIP_STACK_NetworkIsUp(pNetIf))
    {
        return pNetIf;
    }

    return NULL;
}

// checks for valid up and linked interface
TCPIP_NET_IF* TCPIPStackHandleToNetLinked(TCPIP_NET_HANDLE hNet);

// returns a valid, linked interface, if any
// can start search with the default one
TCPIP_NET_IF* TCPIPStackAnyNetLinked(bool useDefault);

// returns the interface for which this address is a net_directed address
// 0 in not found
// does NOT check network up, linked, etc.
// does NOT check primary/virtual interface!
TCPIP_NET_IF* TCPIPStackAnyNetDirected(const IPV4_ADDR* pIpAdd);

// converts between an interface name and a MAC (TCPIP_STACK_MODULE) ID 
// TCPIP_MODULE_MAC_NONE - no MAC id could be found
TCPIP_STACK_MODULE    TCPIP_STACK_StringToMACId(const char* str);

// string (interface name) corresponding to the given MAC id.
// NULL if no such MAC id.
const char*     TCPIP_STACK_MACIdToString(TCPIP_STACK_MODULE macId);

// helper to select an address service for interface
TCPIP_STACK_ADDR_SRVC_TYPE TCPIP_STACK_AddressServiceSelect(TCPIP_NET_IF* pNetIf, TCPIP_NETWORK_CONFIG_FLAGS configFlags);
//Helper to select an DNS service  for  a interface
TCPIP_STACK_DNS_SERVICE_TYPE TCPIP_STACK_DNSServiceSelect(TCPIP_NET_IF* pNetIf, TCPIP_NETWORK_CONFIG_FLAGS configFlags);


// helper to decide if an address service can be started
bool    TCPIP_STACK_AddressServiceCanStart(TCPIP_NET_IF* pNetIf, TCPIP_STACK_ADDR_SRVC_TYPE adSvcType);

// helper to decide if an DNS  service can be started
bool TCPIP_STACK_DNSServiceCanStart(TCPIP_NET_IF* pNetIf, TCPIP_STACK_DNS_SERVICE_TYPE dnsSvcType);


// notification helper that an address service has some stop event
void    TCPIP_STACK_AddressServiceEvent(TCPIP_NET_IF* pNetIf, TCPIP_STACK_ADDR_SRVC_TYPE adSvcType, TCPIP_ADDR_SRVC_EV evType);

// address service default status
void    TCPIP_STACK_AddressServiceDefaultSet(TCPIP_NET_IF* pNetIf);

// helper to check if an address service is running
TCPIP_STACK_ADDR_SRVC_TYPE TCPIPStackAddressServiceIsRunning(TCPIP_NET_IF* pNetIf);

static __inline__ bool __attribute__((always_inline)) TCPIPStackIsConfig(const TCPIP_NET_IF* pNetIf)
{
    return (pNetIf->Flags.bInConfig != 0U);
}


void  TCPIPStackSetConfigAddress(TCPIP_NET_IF* pNetIf, const IPV4_ADDR* ipAddress, const IPV4_ADDR* mask, const IPV4_ADDR* gw, bool config);

// finds an interface that has the IPv6 address
TCPIP_NET_IF* TCPIPStackIPv6AddToNet(IPV6_ADDR* pIPv6Address, IPV6_ADDR_TYPE addType, bool useDefault);

// register a signal/timeout handler with the stack manager
// returns 0 if failed, a valid handle otherwise
// the entries are identified by the signalHandler;
// a second call with the same signalHandler will succeed and not create a new signal entry
TCPIP_SIGNAL_HANDLE TCPIPStackSignalHandlerRegister(TCPIP_STACK_MODULE modId, tcpipModuleSignalHandler signalHandler, int16_t asyncTmoMs);


// changes the parameters for a registered signal handler
// asyncTmo == 0 could be used to stop the timer without deregistration
bool           TCPIPStackSignalHandlerSetParams(TCPIP_STACK_MODULE modId, TCPIP_SIGNAL_HANDLE handle, int16_t asyncTmoMs);


// returns the pending module signals
TCPIP_MODULE_SIGNAL  TCPIPStackModuleSignalGet(TCPIP_STACK_MODULE modId, TCPIP_MODULE_SIGNAL clrMask);

// returns the pending module signals + signal parameter
// signalParam should NOT be 0!
// the signal parameter is cleared after this operation, there's is no peek!
#if (M_TCPIP_STACK_INTERFACE_CHANGE_SIGNALING != 0)
TCPIP_MODULE_SIGNAL  TCPIPStackModuleSignalParamGet(TCPIP_STACK_MODULE modId, TCPIP_MODULE_SIGNAL clrMask, uint32_t* signalParam);
#endif

// a stack module requests setting a signal
// This function is mainly intended for requiring attention
// and the signal to be set is TCPIP_MODULE_SIGNAL_ASYNC!
// There is no need for a module to set other signals if there's really
// no pending RX packets or timeouts.
//
// returns true if a valid module ID requested;
// The TCPIP_MODULE_MANAGER itself is invalid!
//
// NOTE:
//  a TCPIP_MODULE_SIGNAL_ASYNC is monitored by the stack manager so that as long as it is active
//  the module gets special attention.
//  This is done to allow a module to set the TCPIP_MODULE_SIGNAL_ASYNC signal just once
//  instead of continuously request it.
//  When done with the critical section the module should clear the TCPIP_MODULE_SIGNAL_ASYNC signal.
//  However, the stack manager keeps a global count of how many times the modules required TCPIP_MODULE_SIGNAL_ASYNC
//  That means that a module should always call
//  TCPIPStackModuleSignalRequest/TCPIPStackModuleSignalGet for TCPIP_MODULE_SIGNAL_ASYNC in pairs!
//
// NOTE:
//  A signal request sends a signal to the module task itself
//  A signal request will also send a signal to the stack task itself, when the TCP/IP modules are executed internally.
//  - unless noMgrAlert is specified
//
bool                TCPIPStackModuleSignalRequest(TCPIP_STACK_MODULE modId, TCPIP_MODULE_SIGNAL signal_t, bool noMgrAlert);


// de-registers a previous registered signal handler
void           TCPIPStackSignalHandlerDeregister(TCPIP_SIGNAL_HANDLE handle); 


// inserts a RX packet into the interface RX queue
// this has to be a fully formatted TCPIP_MAC_PACKET
void           TCPIPStackInsertRxPacket(const TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pRxPkt, bool signal_t);


TCPIP_MAC_RES TCPIPStackPacketTx(const TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET * ptrPacket);


// extracts a packet from a module RX queue
// returns 0 if queue is empty
TCPIP_MAC_PACKET*   TCPIPStackModuleRxExtract(TCPIP_STACK_MODULE modId);

// inserts a packet into a module queue
// and signals if necessary
// returns:
//      true is packet inserted
//      false if the insertion failed (the module is not running, for example)
bool TCPIPStackModuleRxInsert(TCPIP_STACK_MODULE modId, TCPIP_MAC_PACKET* pRxPkt, bool signal_t);


// purges the packets from a module RX queue
// belonging to the pNetIf
void TCPIPStackModuleRxPurge(TCPIP_STACK_MODULE modId, TCPIP_NET_IF* pNetIf);

// return the stack heap configuration parameters
const TCPIP_STACK_HEAP_CONFIG* TCPIPStackHeapConfig(void);
            
static __inline__ uint16_t  __attribute__((always_inline)) TCPIPStackNetLinkMtu(const TCPIP_NET_IF* pNetIf)
{
    return (pNetIf != NULL) ? pNetIf->linkMtu : 0U;
}

#if (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)

static __inline__ bool  __attribute__((always_inline)) TCPIPStackNetIsPrimary(const TCPIP_NET_IF* pNetIf)
{
    return pNetIf == pNetIf->pPriIf;
}

static __inline__ TCPIP_NET_IF*  __attribute__((always_inline)) TCPIPStackNetGetPrimary(const TCPIP_NET_IF* pNetIf)
{
    return pNetIf->pPriIf;
}

static __inline__ TCPIP_NET_IF*  __attribute__((always_inline)) TCPIPStackNetGetAlias(const TCPIP_NET_IF* pNetIf)
{
    return pNetIf->pAlias;
}

// maps the proper alias network a packet belongs to
// based on the interface the packet arrived on
// (probably a primary interface) and on the packet IP address
// No check is done if the interface (p or a) is UP!
TCPIP_NET_IF* TCPIPStackMapAliasInterface(const TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pDestAddress);

#else
static __inline__ bool  __attribute__((always_inline)) TCPIPStackNetIsPrimary(const TCPIP_NET_IF* pNetIf)
{
    return true;
}

static __inline__ TCPIP_NET_IF*  __attribute__((always_inline)) TCPIPStackNetGetPrimary(const TCPIP_NET_IF* cNetIf)
{
    return FC_CNetIf2NetIf(cNetIf);
}

static __inline__ TCPIP_NET_IF*  __attribute__((always_inline)) TCPIPStackNetGetAlias(const TCPIP_NET_IF* pNetIf)
{
    return NULL;
}

static __inline__ const TCPIP_NET_IF*  __attribute__((always_inline)) TCPIPStackMapAliasInterface(const TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pDestAddress)
{
    return pNetIf;
}



#endif  // (M_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)

// marks an interface as bridged
static __inline__ void __attribute__((always_inline))  TCPIPStack_BridgeSetIf(TCPIP_NET_IF* pNetIf, uint8_t portNo)
{
    pNetIf->exFlags.bridged = 1;
    pNetIf->bridgePort = portNo;
}

// marks an interface as unbridged
static __inline__ void __attribute__((always_inline))  TCPIPStack_BridgeClearIf(TCPIP_NET_IF* pNetIf)
{
    pNetIf->exFlags.bridged = 0;
}

// returns true if an interface is bridged
static __inline__ bool __attribute__((always_inline))  TCPIPStack_BridgeCheckIf(const TCPIP_NET_IF* pNetIf)
{
    return (pNetIf->exFlags.bridged != 0U);
}

// returns the bridge port corresponding to this interface
static __inline__ uint8_t __attribute__((always_inline))  TCPIPStack_BridgeGetIfPort(const TCPIP_NET_IF* pNetIf)
{
    return pNetIf->bridgePort;
}

static __inline__ uint8_t  __attribute__((always_inline)) TCPIPStack_TxPriNum(const TCPIP_NET_IF* pNetIf)
{
    return (pNetIf != NULL) ? pNetIf->txPriNum : 0U;
}

static __inline__ uint8_t  __attribute__((always_inline)) TCPIPStack_RxPriNum(const TCPIP_NET_IF* pNetIf)
{
    return (pNetIf != NULL) ? pNetIf->rxPriNum : 0U;
}

#if (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0) 
// helper to map an incoming packet to the interface it belongs to
// based on the incoming vlanId
TCPIP_NET_IF* F_TCPIPMapVlanInterface(const TCPIP_NET_IF* pInIf, uint16_t vlanId);

static __inline__  uint16_t __attribute__((always_inline)) F_TCPIP_NetVlanId(const TCPIP_NET_IF* pNetIf)
{
    return (pNetIf != NULL) ? pNetIf->vlanId : 0U;
}

static __inline__  uint8_t __attribute__((always_inline)) F_TCPIP_NetVlanPcp(const TCPIP_NET_IF* pNetIf)
{
    return (pNetIf != NULL) ? pNetIf->vlanPcp : 0U;
}


#endif // (M_TCPIP_STACK_VLAN_INTERFACE_SUPPORT != 0) 

// local time keeping
uint32_t TCPIP_SecCountGet(void);
uint32_t TCPIP_MsecCountGet(void);


// debugging, tracing, etc.

// enables the measurement of the CPU time taken by the TCPIP_STACK_Task() processing
// Uses the CP0 count register, SYS_FREQ/2 resolution
//#define TCPIP_STACK_TIME_MEASUREMENT


#if defined(TCPIP_STACK_TIME_MEASUREMENT)

void        TCPIP_STACK_TimeMeasureStart(bool reset);

uint64_t    TCPIP_STACK_TimeMeasureGet(bool stop);    


#endif // defined(TCPIP_STACK_TIME_MEASUREMENT)




#endif  // H_TCPIP_MANAGER_CONTROL_H_


