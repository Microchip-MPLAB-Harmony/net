/*******************************************************************************
  Microchip TCP/IP Stack Include File

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip.h

  Summary:
    TCP/IP API definitions.
    
  Description:
    This is the global TCP/IP header file that any user of the TCP/IP API should include.
    It contains the basic TCP/IP types and data structures and includes all the 
    of the TCP/IP stack modules.
  
*******************************************************************************/
//DOM-IGNORE-BEGIN
/*
Copyright (C) 2016-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef __TCPIP_H__
#define __TCPIP_H__


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
        
#include "configuration.h"
#include "system/system_common.h"
#include "system/system_module.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  


// TCP/IP stack version
#define TCPIP_STACK_VERSION_MAJOR         8
#define TCPIP_STACK_VERSION_MINOR         1
#define TCPIP_STACK_VERSION_PATCH         0
#define TCPIP_STACK_VERSION_STR           "8.10 - H3"


// *****************************************************************************
// *****************************************************************************
// Section: List of the supported TCP/IP Stack Addresses
//          The following enumeration lists all of the address types
//          supported by the TCP/IP Stack.
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* IPv4 Address type

  Summary:
    Definition to represent an IPv4 address

  Description:
    This type describes the IPv4 address type.

  Remarks:
    None.
*/

typedef union
{
    uint32_t Val;
    uint16_t w[2];
    uint8_t  v[4];
} IPV4_ADDR;

// *****************************************************************************
/* IPv4 Address backward compatible type

  Summary:
    Backward compatible definition to represent an IPv4 address.

  Description:
    This type describes the backward compatible IPv4 address type.

  Remarks:
    None.
*/

typedef IPV4_ADDR   IP_ADDR;


// *****************************************************************************
/* IPv6 Address type

  Summary:
    Definition to represent an IPv6 address.

  Description:
    This type describes the IPv6 address type.

  Remarks:
    None.
*/

typedef union
{
    uint8_t  v[16];
    uint16_t w[8];
    uint32_t d[4];
} IPV6_ADDR;

// *****************************************************************************
/* TCP/IP IP Address type

  Summary:
    Definition of the TCP/IP supported address types.

  Description:
    This type describes the supported IP address types.

  Remarks:
    8 bit value only.
*/

typedef enum
{
    /* either IPv4 or IPv6, unspecified; */
    IP_ADDRESS_TYPE_ANY /*DOM-IGNORE-BEGIN*/ = 0 /*DOM-IGNORE-END*/,
    /* IPv4 address type */
    IP_ADDRESS_TYPE_IPV4,
    /* IPv6 address type */
    IP_ADDRESS_TYPE_IPV6,
}IP_ADDRESS_TYPE;
    

// *****************************************************************************
/* TCP/IP multiple Address type

  Summary:
    Definition to represent multiple IP addresses.

  Description:
    This type describes the TCP/IP multiple address type.

  Remarks:
    None.
*/

typedef union
{
    IPV4_ADDR v4Add;
    IPV6_ADDR v6Add;
}IP_MULTI_ADDRESS;
    
// *****************************************************************************
/* IPv6 Address scope

  Summary:
    Definition to represent the scope of an IPv6 address.

  Description:
    This type describes the IPv6 address scope.

  Remarks:
    None.
*/

typedef enum
{
    IPV6_ADDR_SCOPE_UNKNOWN         /*DOM-IGNORE-BEGIN*/ = 0x00 /*DOM-IGNORE-END*/,
    IPV6_ADDR_SCOPE_INTERFACE_LOCAL /*DOM-IGNORE-BEGIN*/ = 0x01 /*DOM-IGNORE-END*/,
    IPV6_ADDR_SCOPE_LINK_LOCAL      /*DOM-IGNORE-BEGIN*/ = 0x02 /*DOM-IGNORE-END*/,
    IPV6_ADDR_SCOPE_ADMIN_LOCAL     /*DOM-IGNORE-BEGIN*/ = 0x04 /*DOM-IGNORE-END*/,
    IPV6_ADDR_SCOPE_SITE_LOCAL      /*DOM-IGNORE-BEGIN*/ = 0x05 /*DOM-IGNORE-END*/,
    IPV6_ADDR_SCOPE_ORG_LOCAL       /*DOM-IGNORE-BEGIN*/ = 0x08 /*DOM-IGNORE-END*/,
    IPV6_ADDR_SCOPE_GLOBAL          /*DOM-IGNORE-BEGIN*/ = 0x0E /*DOM-IGNORE-END*/,
}IPV6_ADDR_SCOPE;

// *****************************************************************************
/* IPv6 Address type

  Summary:
    Definition to represent the type of an IPv6 address.

  Description:
    This type describes the possible type of an IPv6 address.

  Remarks:
    None.
*/

typedef enum
{
    /* Invalid/unknown address type */
    IPV6_ADDR_TYPE_UNKNOWN                  /*DOM-IGNORE-BEGIN*/ = 0 /*DOM-IGNORE-END*/,
    /* Only link-local and global are currently valid for unicast */
    IPV6_ADDR_TYPE_UNICAST                  /*DOM-IGNORE-BEGIN*/ = 0x01 /*DOM-IGNORE-END*/,
    IPV6_ADDR_TYPE_ANYCAST                  /*DOM-IGNORE-BEGIN*/ = 0x02 /*DOM-IGNORE-END*/,
    IPV6_ADDR_TYPE_MULTICAST                /*DOM-IGNORE-BEGIN*/ = 0x03 /*DOM-IGNORE-END*/,
    IPV6_ADDR_TYPE_SOLICITED_NODE_MULTICAST /*DOM-IGNORE-BEGIN*/ = 0x04 /*DOM-IGNORE-END*/,
    IPV6_ADDR_TYPE_UNICAST_TENTATIVE        /*DOM-IGNORE-BEGIN*/ = 0x05 /*DOM-IGNORE-END*/,
}IPV6_ADDR_TYPE;



// *****************************************************************************
/* IPv6 Address Structure

  Summary:
    COmplex type to represent an IPv6 address.

  Description:
    This type defines all the fields of an IPv6 address.

  Remarks:
    None.
*/

typedef struct __attribute__((__packed__)) _IPV6_ADDR_STRUCT
{
    struct _IPV6_ADDR_STRUCT * next;
    struct _IPV6_ADDR_STRUCT * prev;
    IPV6_ADDR address;
    unsigned long validLifetime;
    unsigned long preferredLifetime;
    unsigned long lastTickTime;
    unsigned char prefixLen;
    struct __attribute__((__packed__))
    {
        /* Allow preferences */
        unsigned char precedence;                  
        /* Link-local, site-local, global. */
        unsigned scope                  :4;
        /* Policy label */
        unsigned label                  :4; 
        /* Uni-, Any-, Multi-cast */
        unsigned type                   :2;
        /* Indicates that the address is temporary (not public) */
        unsigned temporary              :1;
    }flags;
} IPV6_ADDR_STRUCT;

// *****************************************************************************
/* IPv6 Address handle

  Summary:
    Definition to represent an IPv6 address.

  Description:
    This type describes an IPv6 address handle.

  Remarks:
    None.
*/

typedef const void*   IPV6_ADDR_HANDLE;


// *****************************************************************************
/* TCP/IP stack supported modules

  Summary:
    List of the TCP/IP stack supported modules.

  Description:
    The following enumeration lists all the modules supported
    by the TCP/IP stack.

  Remarks:
    16 bit values only.
*/

typedef enum
{
    /* unspecified/unknown module */
    TCPIP_MODULE_NONE              = 0,
    /*DOM-IGNORE-BEGIN*/     // manager: layer 0 module /*DOM-IGNORE-END*/
    TCPIP_MODULE_MANAGER,    /* stack manager + packet allocation manager */ 

    /*DOM-IGNORE-BEGIN*/    TCPIP_MODULE_LAYER1,  // 1st layer modules: 2 - 5 /*DOM-IGNORE-END*/
    TCPIP_MODULE_ARP        /*DOM-IGNORE-BEGIN*/ = TCPIP_MODULE_LAYER1 /*DOM-IGNORE-END*/,
    TCPIP_MODULE_IPV4,
    TCPIP_MODULE_IPV6,
    TCPIP_MODULE_LLDP,              /* LLDP module */

    /*DOM-IGNORE-BEGIN*/    TCPIP_MODULE_LAYER2, // 2nd layer modules: 6 - 11 /*DOM-IGNORE-END*/
    TCPIP_MODULE_ICMP       /*DOM-IGNORE-BEGIN*/ = TCPIP_MODULE_LAYER2 /*DOM-IGNORE-END*/,
    TCPIP_MODULE_ICMPV6,
    TCPIP_MODULE_NDP,
    TCPIP_MODULE_UDP,
    TCPIP_MODULE_TCP,
    TCPIP_MODULE_IGMP,      /* IGMP host module */

    /*DOM-IGNORE-BEGIN*/    TCPIP_MODULE_LAYER3, // 3rd layer modules: 12 - 19 /*DOM-IGNORE-END*/
    TCPIP_MODULE_DHCP_CLIENT  /*DOM-IGNORE-BEGIN*/ = TCPIP_MODULE_LAYER3 /*DOM-IGNORE-END*/,
    TCPIP_MODULE_DHCP_SERVER,
    TCPIP_MODULE_ANNOUNCE,
    TCPIP_MODULE_DNS_CLIENT,
    TCPIP_MODULE_DNS_SERVER,
    TCPIP_MODULE_ZCLL,              /* Zero Config Link Local */
    TCPIP_MODULE_MDNS,              /* Bonjour/mDNS */
    TCPIP_MODULE_NBNS,

    /*DOM-IGNORE-BEGIN*/    // 3rd layer modules: 20 - 29 /*DOM-IGNORE-END*/
    TCPIP_MODULE_SMTP_CLIENT,       /* Obsolete - old SMTP client */
    TCPIP_MODULE_SNTP,
    TCPIP_MODULE_FTP_SERVER,
    TCPIP_MODULE_HTTP_SERVER,
    TCPIP_MODULE_HTTP_NET_SERVER,
    TCPIP_MODULE_HTTP_SERVER_V2,    /* new HTTP server v2 module */
    TCPIP_MODULE_TELNET_SERVER,
    TCPIP_MODULE_SNMP_SERVER,
    TCPIP_MODULE_SNMPV3_SERVER,
    TCPIP_MODULE_DYNDNS_CLIENT,

    /*DOM-IGNORE-BEGIN*/    // 3rd layer modules: 30 - 38 /*DOM-IGNORE-END*/
    TCPIP_MODULE_BERKELEY,
    TCPIP_MODULE_REBOOT_SERVER,
    TCPIP_MODULE_COMMAND,
    TCPIP_MODULE_IPERF,
    TCPIP_MODULE_TFTP_CLIENT,       /* TFTP client module */
    TCPIP_MODULE_DHCPV6_CLIENT,     /* DHCPV6 client */
    TCPIP_MODULE_SMTPC,             /* SMTP (new) client */
    TCPIP_MODULE_TFTP_SERVER,       /* TFTP Server module */
    TCPIP_MODULE_FTP_CLIENT,        /* FTP client */

    /* add other modules here */
    TCPIP_MODULE_MAC_BRIDGE,        /* MAC layer 2 bridge */
    //
    /*  */
    TCPIP_MODULES_NUMBER,       /* number of modules in the TCP/IP stack itself */
    /*  */
    /* starting here is list of supported MAC modules */
    /* and are defined in the tcpip_mac.h  */
    TCPIP_MODULE_MAC_START /*DOM-IGNORE-BEGIN*/ = 0x1000 /*DOM-IGNORE-END*/, 

}TCPIP_STACK_MODULE;


// *****************************************************************************
/* TCP/IP stack supported network interfaces

  Summary:
    Defines the TCP/IP stack supported network interfaces.

  Description:
     The following enumeration lists the names of all the interfaces supported
     by the TCP/IP stack and aliases.

  Remarks:
    None.
*/

#define TCPIP_STACK_IF_NAME_NONE            0
#define TCPIP_STACK_IF_NAME_ENCJ60          "ENCJ60"
#define TCPIP_STACK_IF_NAME_ENCJ600         "ENCJ600"
#define TCPIP_STACK_IF_NAME_97J60           "97J60"
#define TCPIP_STACK_IF_NAME_PIC32INT        "PIC32INT"
#define TCPIP_STACK_IF_NAME_MRF24WN         "MRF24WN"
#define TCPIP_STACK_IF_NAME_WINC            "WINC"
#define TCPIP_STACK_IF_NAME_WILC1000        "WILC1000"
#define TCPIP_STACK_IF_NAME_G3ADP           "G3ADPMAC"

/* alias for unknown interface */
#define TCPIP_STACK_IF_NAME_ALIAS_UNK       "unk"
/* alias for Ethernet interface */
#define TCPIP_STACK_IF_NAME_ALIAS_ETH       "eth"
/* alias for Wi-Fi interface */
#define TCPIP_STACK_IF_NAME_ALIAS_WLAN      "wlan"
/* alias for PPP interface */
#define TCPIP_STACK_IF_NAME_ALIAS_PPP       "ppp"
/* alias for G3 ADP interface */
#define TCPIP_STACK_IF_NAME_ALIAS_G3ADP     "g3adp"

// *****************************************************************************
/* Configuration Power Modes

  Summary:
    Supported TCP/IP network configuration power modes.

  Description:
    This describes the list of the supported TCP/IP network configuration power modes.

  Remarks:
    None.
*/

/* unsupported, invalid */
#define TCPIP_STACK_IF_POWER_NONE       0
/* up and running; */
#define TCPIP_STACK_IF_POWER_FULL       "full"  
/* low power mode; not supported now */
#define TCPIP_STACK_IF_POWER_LOW        "low"   
/* powered down, not started */
#define TCPIP_STACK_IF_POWER_DOWN       "down"  

// *****************************************************************************
/* Network Configuration start-up flags

  Summary:
    Definition of network configuration start-up flags.

  Description:
    This enumerated type describes the list of the supported TCP/IP network 
    configuration start-up flags.

  Remarks:
    DHCPc, DHCPs and ZCLL are conflicting and
    cannot be more than one enabled on the same interface!
    The order of priorities is: DHCPc, ZCLL, DHCPs in case of conflict
    
    Only one of either DNS server or client can be enabled per interface.

    Valid values are 0x0001 - 0x8000; 16 bits are maintained only.
*/

typedef enum
{
    /* Start the interface with a static IP address */
    /* No address service is enabled on this interface (DHCPc, ZCLL or DHCPs) */
    TCPIP_NETWORK_CONFIG_IP_STATIC            /*DOM-IGNORE-BEGIN*/ = 0x0000 /*DOM-IGNORE-END*/,   
    /* DHCP client enabled on this interface */
    TCPIP_NETWORK_CONFIG_DHCP_CLIENT_ON       /*DOM-IGNORE-BEGIN*/ = 0x0001 /*DOM-IGNORE-END*/,   
    /* ZeroConf LinkLocal enabled on this interface */
    TCPIP_NETWORK_CONFIG_ZCLL_ON              /*DOM-IGNORE-BEGIN*/ = 0x0002 /*DOM-IGNORE-END*/,   
    /* DHCP server enabled on this interface */
    TCPIP_NETWORK_CONFIG_DHCP_SERVER_ON       /*DOM-IGNORE-BEGIN*/ = 0x0004 /*DOM-IGNORE-END*/,   

    /* DNS CLIENT enabled on this interface */
    TCPIP_NETWORK_CONFIG_DNS_CLIENT_ON        /*DOM-IGNORE-BEGIN*/ = 0x0008 /*DOM-IGNORE-END*/,   
    /* DNS Server Enabled on this Interface */
    TCPIP_NETWORK_CONFIG_DNS_SERVER_ON        /*DOM-IGNORE-BEGIN*/ = 0x0010 /*DOM-IGNORE-END*/,   
    /* Multicast traffic enabled on this Interface */
    TCPIP_NETWORK_CONFIG_MULTICAST_ON         /*DOM-IGNORE-BEGIN*/ = 0x0020 /*DOM-IGNORE-END*/,   
    /* Packet logging is enabled on this Interface */
    TCPIP_NETWORK_CONFIG_PKT_LOG_ON           /*DOM-IGNORE-BEGIN*/ = 0x0040 /*DOM-IGNORE-END*/,   

    /* the network configuration contains an IPv6 static address and subnet prefix length */
    TCPIP_NETWORK_CONFIG_IPV6_ADDRESS         /*DOM-IGNORE-BEGIN*/ = 0x0100 /*DOM-IGNORE-END*/,   

    /* G3-PLC IPv6 general interface flags */
    /* the network will be part of a G3-PLC network */
    TCPIP_NETWORK_CONFIG_IPV6_G3_NET          /*DOM-IGNORE-BEGIN*/ = 0x0200 /*DOM-IGNORE-END*/,   

    /* The IPv6 will suppress the Duplicate Address Detection on this interface */
    TCPIP_NETWORK_CONFIG_IPV6_NO_DAD          /*DOM-IGNORE-BEGIN*/ = 0x0400 /*DOM-IGNORE-END*/,   
    
    /* G3-PLC IPv6 router/coordinator interface flags */
    /* the network will act as an IPv6 border router/coordinator, replying to solicitations */
    TCPIP_NETWORK_CONFIG_IPV6_ROUTER          /*DOM-IGNORE-BEGIN*/ = 0x0800 /*DOM-IGNORE-END*/,   

    /* when configured as an IPv6 router, sending advertisements is enabled  */
    TCPIP_NETWORK_CONFIG_IPV6_ADV_ENABLED     /*DOM-IGNORE-BEGIN*/ = 0x1000 /*DOM-IGNORE-END*/,   

    /* Suppress the RS (Router Solicitation) messages on this interface */
    TCPIP_NETWORK_CONFIG_IPV6_NO_RS           /*DOM-IGNORE-BEGIN*/ = 0x2000 /*DOM-IGNORE-END*/,   
    
    /* G3-PLC IPv6 device interface flags */
    /* Send RS messages to a router unicast address rather than multicast.
      By default the 'all IPv6 routers' multicast address: 'ff02::02' is used */
    TCPIP_NETWORK_CONFIG_IPV6_UNICAST_RS      /*DOM-IGNORE-BEGIN*/ = 0x4000 /*DOM-IGNORE-END*/,   

    /* add other configuration flags here */
}TCPIP_NETWORK_CONFIG_FLAGS;

// *****************************************************************************
/* Forward declaration of a TCP/IP MAC basic Object

  Summary:
    Definition of a TCP/IP MAC Object.

  Description:
    This data structure identifies the required basic interface of the TCP/IP MAC driver.
    Any dynamic MAC driver has to export this interface.

  Remarks:
    See the structure definition in "tcpip/tcpip_mac_object.h" for details of its members.

*/
struct TCPIP_MAC_OBJECT_TYPE;

// *****************************************************************************
/* Multi-Homed Hosts Network Addressing Configuration Data

  Summary:
    Defines the data required to initialize the network configuration.

  Description:
    This data type defines the data required to initialize the network configuration
    for a specific interface.

  Remarks:
    IPv4 aliased interfaces can be created by specifying the same MAC object member
    TCPIP_NETWORK_CONFIG.pMacObject. 
    An aliased interface is one that shares the same physical interface
    and MAC object with a primary interface.

    Note that the .macAddr is irrelevant for an IPv4 aliased interface.
    The first interface fully configured will be the primary interface, others will
    be aliases.

    An IPv4 alias interface will allow, for example, 
    having a different static/dynamic IPv4 address on the same physical interface.

    Note that the stack won't allow initialization of multiple interfaces
    with the same static IPv4 address.

    It is also recommended that each interface has a different host name.

    For an IPv4 alias interface .powerMode ==  TCPIP_STACK_IF_POWER_DOWN can be used to prevent
    the alias to be started when the stack is initialized and the primary interfaces go up.
 
    A primary interface currently supports only the
    TCPIP_STACK_IF_POWER_FULL and TCPIP_STACK_IF_POWER_DOWN power modes.

    Alias interfaces are not currently supported on IPv6.

    Currently a broadcast message received (on a primary interface)
    is not duplicated on all aliases but it will appear only on the primary interface. 
*/
typedef struct
{
    /* Pointer to the interface name; could be NULL. */
    /* Note: Usually it's been one of the TCPIP_STACK_IF_NAME_xxx symbols: */
    /*        "ENCJ60", "ENCJ600", "97J60", "PIC32INT", "MRF24WN", "WINC", "WILC1000". */
    /*        However, the TCP/IP stack will assign an alias names for each interface */
    /*        (eth0, wlan0, etc. see TCPIP_STACK_IF_NAME_ALIAS_xxx), so this name is maintained */
    /*        for backward compatibility purposes only and will be eventually dropped. */
    /*        The native name of the interface in the form TCPIP_STACK_IF_NAME_xxx is */
    /*        present in the MAC driver for that interface (see the "tcpip/tcpip_mac_object.h" */
    /*        definition of TCPIP_MAC_OBJECT_TYPE). */
    const char*     interface;

    /* Valid Host name for this interface to use.  Ex: "MCHPBOARD" */
    const char*     hostName;

    /* MAC address to use for this interface. */
    /*   Use "00:04:a3:00:00:00" or 0 for the factory preprogrammed address*/
    const char*     macAddr;

    /* Static IP address to use.  Ex: "169.254.1.1"*/
    const char*     ipAddr;

    /* Netmask to use. Ex: "255.255.0.0"*/
    const char*     ipMask;

    /* Static Gateway to use. Ex: "169.254.1.1"*/
    const char*     gateway;

    /* Primary DNS to use.  Ex: "169.254.1.1"*/
    const char*     priDNS;

    /* Secondary DNS to use.  Use "0.0.0.0" for none */
    const char*     secondDNS;  

    /* Power Mode to use.  Use TCPIP_STACK_IF_POWER_NONE, TCPIP_STACK_IF_POWER_FULL, */
    /* TCPIP_STACK_IF_POWER_LOW, or TCPIP_STACK_IF_POWER_DOWN*/
    const char*     powerMode; 

    /* flags for interface start-up */
    TCPIP_NETWORK_CONFIG_FLAGS   startFlags;

    /* Non-volatile pointer to the MAC driver object associated with this network interface */
    /* This is the MAC driver that this interface will use */
    /* Note: This object has to be valid for the whole life of the interface! */
    /*       The TCP/IP stack does not make a private copy of this object */
    const struct TCPIP_MAC_OBJECT_TYPE*    pMacObject;   

    /* static IPv6 address; only if TCPIP_NETWORK_CONFIG_IPV6_ADDRESS specified can be NULL if not needed*/
    const char*     ipv6Addr;   

    /* subnet prefix length; only if TCPIP_NETWORK_CONFIG_IPV6_ADDRESS specified
       0 means default value (64)
       should probably always be 64 as requested by the RFC */
    int             ipv6PrefixLen;

    /* default IPv6 gateway address; only if TCPIP_NETWORK_CONFIG_IPV6_ADDRESS specified
       can be NULL if not needed */
    const char*     ipv6Gateway; 

    /* IPv6 DNS to use; only if TCPIP_NETWORK_CONFIG_IPV6_ADDRESS specified
       can be NULL if not needed */
    const char*     ipv6Dns; 
}TCPIP_NETWORK_CONFIG;

// *****************************************************************************
/* TCP/IP module initialization/configuration structure

  Summary:
    Definition to represent a TCP/IP module initialization/configuration structure.

  Description:
    This type describes a TCP/IP module initialization/configuration data type
    that's passed to a TCP/IP module at the stack initialization time.

  Remarks:
    Each stack module will be configured with a user-defined initialization/configuration 
    structure.
*/

typedef struct
{
    TCPIP_STACK_MODULE      moduleId;
    const void * const      configData;
}TCPIP_STACK_MODULE_CONFIG;

// *****************************************************************************
/* TCP/IP stack initialization callback

  Summary:
    Definition to represent a TCP/IP stack initialization callback function.

  Description:
    This type describes a TCP/IP stack initialization callback function type
    that's passed to the stack at the initialization time.

   Parameters:
    ppStackInit  - Pointer to the address of the initialization data.
              It should be updated to a TCPIP_STACK_INIT pointer that carries the stack initialization data:
                -    pNetConf    - pointer to an array of TCPIP_NETWORK_CONFIG to support
                -    nNets       - number of network configurations in the array
                -    pModConfig  - pointer to an array of TCPIP_STACK_MODULE_CONFIG
                -    nModules    - number of modules to initialize 
  
  Returns:
    >  0    - the initialization is pending. The stack is required to call again the callback

    == 0    - the initialization data is ready and the ppStackInit has been properly updated
              stack initialization should proceed

    <  0    - some error occurred, the stack initialization should be aborted

  Remarks:
    This callback is part of the initialization data that's passed to the stack
    at initialization

    If the callback is NULL, then the stack initialization will proceed immediately with the
    data presented in the TCPIP_STACK_INIT.

    If the callback is not NULL, then the initialization of the stack will be delayed
    and it will be performed in the TCPIP_STACK_Task() function.
    The TCPIP_STACK_Task() will keep calling the callback function until this returns 
    ready or an error condition.

    The ppStackInit will be used in the TCPIP_STACK_Task() context.
    It has to point to a persistent data structure that won't go 
    out of scope until the stack initialization is finished!

*/
struct TCPIP_STACK_INIT;
typedef int (*TCPIP_STACK_INIT_CALLBACK)(const struct TCPIP_STACK_INIT** ppStackInit);


// *****************************************************************************
/* TCP/IP stack initialization/configuration structure

  Summary:
    Definition to represent the TCP/IP stack initialization/configuration structure.

  Description:
    This type describes the TCP/IP stack initialization/configuration data type
    that's passed to the TCP/IP stack at the initialization time.

  Remarks:
    This data structure will be passed to the stack initialization function,
    TCPIP_STACK_Initialize.
*/

typedef struct TCPIP_STACK_INIT
{
    /* system module initialization     */
    SYS_MODULE_INIT                     moduleInit; 
    /* pointer to array of network configurations */
    const TCPIP_NETWORK_CONFIG*         pNetConf;   
    /* number of networks in the configuration array */
    int                                 nNets;      
    /* pointer to array of module configurations */
    const TCPIP_STACK_MODULE_CONFIG*    pModConfig; 
    /* number of modules in the array  */
    int                                 nModules;   
    /* initialization callback */
    TCPIP_STACK_INIT_CALLBACK           initCback;
}TCPIP_STACK_INIT;

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: TCPIP Stack Modules Include
// *****************************************************************************
// *****************************************************************************
#include "tcpip/tcpip_common_ports.h"
#include "tcpip/tcpip_mac.h"
#include "tcpip/tcpip_mac_object.h"
#include "tcpip/tcpip_manager.h"
#include "tcpip/tcpip_heap.h"
#include "tcpip/tcpip_helpers.h"
<#if (tcpipNdp.TCPIP_USE_NDP)?has_content && (tcpipNdp.TCPIP_USE_NDP) == true>
#include "tcpip/ndp.h"
</#if>
#include "tcpip/ipv4.h"
#include "tcpip/dhcp.h"
#include "tcpip/dns.h"
#include "tcpip/ipv6.h"
<#if ((tcpipIPv6.TCPIP_STACK_USE_IPV6)?has_content && (tcpipIPv6.TCPIP_STACK_USE_IPV6) == true) >
#include "tcpip/icmpv6.h"
#include "tcpip/dhcpv6.h"
</#if>
<#if (tcpipArp.TCPIP_USE_ARP)?has_content && (tcpipArp.TCPIP_USE_ARP) == true>
#include "tcpip/arp.h"
</#if>
<#if ((tcpipTcp.TCPIP_USE_TCP)?has_content &&  (tcpipTcp.TCPIP_USE_TCP) == true) ||
((tcpipUdp.TCPIP_USE_UDP)?has_content &&  (tcpipUdp.TCPIP_USE_UDP) == true)>
#include "tcpip/tcp.h"
#include "tcpip/udp.h"
</#if>
<#if (tcpipBerkeleyApi.TCPIP_STACK_USE_BERKELEY_API)?has_content &&  (tcpipBerkeleyApi.TCPIP_STACK_USE_BERKELEY_API) == true>
#include "tcpip/berkeley_api.h"
</#if>
<#if (tcpipDhcps.TCPIP_STACK_USE_DHCP_SERVER)?has_content &&  (tcpipDhcps.TCPIP_STACK_USE_DHCP_SERVER) == true>
#include "tcpip/dhcps.h"
</#if>
<#if (tcpipDhcpServer.TCPIP_STACK_USE_DHCP_SERVER_V2)?has_content &&  (tcpipDhcpServer.TCPIP_STACK_USE_DHCP_SERVER_V2) == true>
#include "tcpip/dhcp_server.h"
</#if>
<#if (tcpipZeroConf.TCPIP_USE_LINK_ZERO_CONFIG)?has_content &&  (tcpipZeroConf.TCPIP_USE_LINK_ZERO_CONFIG) == true>
#include "tcpip/zero_conf_link_local.h"
#include "tcpip/zero_conf_multicast_dns.h"
</#if>
<#if (tcpipDnss.TCPIP_USE_DNSS)?has_content &&  (tcpipDnss.TCPIP_USE_DNSS) == true>
#include "tcpip/dnss.h"
</#if>
<#if (tcpipFtps.TCPIP_USE_FTP_MODULE)?has_content &&  (tcpipFtps.TCPIP_USE_FTP_MODULE) == true>
#include "tcpip/ftp.h"
</#if>
<#if (tcpipFtpc.TCPIP_STACK_USE_FTP_CLIENT)?has_content &&  (tcpipFtpc.TCPIP_STACK_USE_FTP_CLIENT) == true>
#include "tcpip/ftpc.h"
</#if>
<#if (tcpipIcmp.TCPIP_STACK_USE_ICMPV4)?has_content &&  (tcpipIcmp.TCPIP_STACK_USE_ICMPV4) == true>
#include "tcpip/icmp.h"
</#if>
<#if (tcpipNbns.TCPIP_USE_NBNS)?has_content &&  (tcpipNbns.TCPIP_USE_NBNS) == true>
#include "tcpip/nbns.h"
</#if>
<#if (tcpipDdns.TCPIP_USE_DDNS)?has_content &&  (tcpipDdns.TCPIP_USE_DDNS) == true>
#include "tcpip/ddns.h"
</#if>
<#if (tcpipTelnet.TCPIP_USE_TELNET)?has_content &&  (tcpipTelnet.TCPIP_USE_TELNET) == true>
#include "tcpip/telnet.h"
</#if>
<#if (tcpipSntp.TCPIP_USE_SNTP_CLIENT)?has_content &&  (tcpipSntp.TCPIP_USE_SNTP_CLIENT) == true>
#include "tcpip/sntp.h"
</#if>
<#if (tcpipHttp.TCPIP_STACK_USE_HTTP_SERVER)?has_content &&  (tcpipHttp.TCPIP_STACK_USE_HTTP_SERVER) == true>
#include "tcpip/http.h"
</#if>
<#if (tcpipHttpNet.TCPIP_STACK_USE_HTTP_NET_SERVER)?has_content &&  (tcpipHttpNet.TCPIP_STACK_USE_HTTP_NET_SERVER) == true>
#include "tcpip/http_net.h"
</#if>
<#if (tcpipHttpSrvV2.TCPIP_STACK_USE_HTTP_SERVER_V2)?has_content &&  (tcpipHttpSrvV2.TCPIP_STACK_USE_HTTP_SERVER_V2) == true>
#include "tcpip/http_server.h"
</#if>
<#if ((tcpipSnmp.TCPIP_USE_SNMP)?has_content &&  (tcpipSnmp.TCPIP_USE_SNMP) == true) ||
((tcpipSnmpv3.TCPIP_USE_SNMPv3)?has_content &&  (tcpipSnmpv3.TCPIP_USE_SNMPv3) == true)>
#include "tcpip/snmp.h"
#include "tcpip/snmpv3.h"
</#if>
<#if (tcpipAnnounce.TCPIP_USE_ANNOUNCE)?has_content &&  (tcpipAnnounce.TCPIP_USE_ANNOUNCE) == true>
#include "tcpip/tcpip_announce.h"
</#if>
#include "tcpip/lldp.h"
<#if (tcpipSmtpc.TCPIP_USE_SMTPC_CLIENT)?has_content &&  (tcpipSmtpc.TCPIP_USE_SMTPC_CLIENT) == true>
#include "tcpip/smtpc.h"
</#if>
<#if (tcpipIgmp.TCPIP_USE_IGMP)?has_content &&  (tcpipIgmp.TCPIP_USE_IGMP) == true>
#include "tcpip/igmp.h"
</#if>
<#if (tcpipTftpc.TCPIP_USE_TFTPC_MODULE)?has_content &&  (tcpipTftpc.TCPIP_USE_TFTPC_MODULE) == true>
#include "tcpip/tftpc.h"
</#if>
<#if (tcpipTftps.TCPIP_USE_TFTPS_MODULE)?has_content &&  (tcpipTftps.TCPIP_USE_TFTPS_MODULE) == true>
#include "tcpip/tftps.h"
</#if>
<#if (tcpipReboot.TCPIP_USE_REBOOT_SERVER)?has_content &&  (tcpipReboot.TCPIP_USE_REBOOT_SERVER) == true>
#include "tcpip/tcpip_reboot.h"
</#if>
<#if (tcpipIperf.TCPIP_USE_IPERF)?has_content &&  (tcpipIperf.TCPIP_USE_IPERF) == true>
#include "tcpip/iperf.h"
</#if>
<#if (tcpipCmd.TCPIP_STACK_USE_COMMANDS)?has_content &&  (tcpipCmd.TCPIP_STACK_USE_COMMANDS) == true>
#include "tcpip/tcpip_commands.h"
</#if>
<#if (tcpipNetConfig.TCPIP_STACK_USE_MAC_BRIDGE)?has_content && (tcpipNetConfig.TCPIP_STACK_USE_MAC_BRIDGE) == true>
#include "tcpip/tcpip_mac_bridge.h"
</#if>
#endif  // __TCPIP_H__

