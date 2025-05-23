/*******************************************************************************
  Private ZCLL file

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    - Provides the private definitions for ZCLL

*******************************************************************************/

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

#ifndef ZERO_CONF_LL_PRIVATE_H
#define ZERO_CONF_LL_PRIVATE_H

#include <stdint.h>

// access to low level ARP definitions
#include "arp_private.h"

#include "zero_conf_helper.h"

// For verbose debugging
#ifdef TCPIP_ZC_DEBUG_MDNS
#define DEBUG_MDNS_PRINT(x) debug_mdns_print(x)
#else
#define DEBUG_MDNS_PRINT(x)
#endif

// For less verbose debugging
#ifdef TCPIP_ZC_INFO_MDNS
#define INFO_MDNS_PRINT(x) info_mdns_print(x)
#else
#define INFO_MDNS_PRINT(x)
#endif

// For warnings, errors, and other exceptions
#ifdef TCPIP_ZC_WARN_MDNS
#define WARN_MDNS_PRINT(x) SYS_CONSOLE_MESSAGE(x)
#else
#define WARN_MDNS_PRINT(x)
#endif

#define DEBUG0_MDNS_PRINT  INFO_MDNS_PRINT


// For verbose debugging
#ifdef TCPIP_ZC_DEBUG_ZCLL
#define DEBUG_ZCLL_PRINT(x) debug_zcll_print(x)
#else
#define DEBUG_ZCLL_PRINT(x)
#endif

// For less verbose debugging
#ifdef TCPIP_ZC_INFO_ZCLL
#define INFO_ZCLL_PRINT(x) info_zcll_print(x)
#else
#define INFO_ZCLL_PRINT(x)
#endif

// For warnings, errors, and other exceptions
#ifdef CONFIG_TCPIP_ZC_WARN_ZCLL
#define WARN_ZCLL_PRINT(x) SYS_CONSOLE_MESSAGE(x)
#else
#define WARN_ZCLL_PRINT(x)
#endif

/* ARP  States Enum */
typedef enum
{
   ZCLL_ARP_PROBE = 0,// Par 2.1.1 RFC 5227.
   ZCLL_ARP_CLAIM,    //
   ZCLL_ARP_DEFEND    //Par 2.4 RFC 5227
} ZCLL_ARP_STATE;     // Zero Config Local Link




/* constants from RFC 2937, section 9. ALso found in RFC 5227 Par 1.1*/
#define PROBE_WAIT           TCPIP_ZC_LL_PROBE_WAIT /*second  (initial random delay)              */
#define PROBE_MIN            TCPIP_ZC_LL_PROBE_MIN /*second  (minimum delay till repeated probe) */
#define PROBE_MAX            TCPIP_ZC_LL_PROBE_MAX /*seconds (maximum delay till repeated probe) */
#define PROBE_NUM            TCPIP_ZC_LL_PROBE_NUM /*         (number of probe packets)          */
#define ANNOUNCE_WAIT        TCPIP_ZC_LL_ANNOUNCE_WAIT /*seconds  (delay before announcing)          */
#define ANNOUNCE_NUM         TCPIP_ZC_LL_ANNOUNCE_NUM /*        (number of announcement packets)    */
#define ANNOUNCE_INTERVAL    TCPIP_ZC_LL_ANNOUNCE_INTERVAL /*seconds (time between announcement packets) */
#define MAX_CONFLICTS       TCPIP_ZC_LL_MAX_CONFLICTS /*        (max conflicts before rate limiting)*/
#define RATE_LIMIT_INTERVAL TCPIP_ZC_LL_RATE_LIMIT_INTERVAL /*seconds (delay between successive attempts) */
#define DEFEND_INTERVAL     TCPIP_ZC_LL_DEFEND_INTERVAL /*seconds (min. wait between defensive ARPs)  */

/* compilation constants */
#define IPV4_LLBASE         TCPIP_ZC_LL_IPV4_LLBASE /* 169.254.1.0 */
#define IPV4_LLBASE_MASK    TCPIP_ZC_LL_IPV4_LLBASE_MASK /* corresponding network mask */


#define ZCLL_TASK_TICK_RATE TCPIP_ZC_LL_TASK_TICK_RATE     // task processing rate, ms

/* Link-Local States Enum */
typedef enum 
{
   SM_INIT = 0,
   SM_ADDR_INIT,
   SM_ADDR_PROBE,
   SM_ADDR_CLAIM,
   SM_ADDR_DEFEND,
   SM_ADDR_RELEASE,
} ZCLL_STATE;


/* ARP-Packet Types defined in RFC 2937 */
typedef enum
{
    ARP_REQUEST_TYPE = 0,
    ARP_PROBE_TYPE,
    ARP_CLAIM_TYPE,
    ARP_DEFEND_TYPE,
    ARP_RESPONSE_TYPE,
    UNKNOWN_TYPE,
}ARP_PKT_TYPE;


/* Flags for the ZCLL State Machine */
typedef union
{
    struct __attribute__((packed))
    {
        unsigned probe_conflict  : 1 ;     // Conflict When selecting IP-addr
        unsigned late_conflict   : 1;      // Conflict When using IP-addr
        unsigned defended        : 1;      // whether or not defended earlier
        //
        unsigned reserved        : 5;        // not used
    };
    uint8_t Val;
}ZCLL_FLAGS;

typedef struct 
{
    uint32_t    event_time; // Internal Timer, to keep track of events
    uint32_t    random_delay;
    uint32_t    rand_state;
    ZCLL_STATE  zcll_state;
    IPV4_ADDR   temp_IP_addr;   // Temporary IP address before checking uniqueness
    uint8_t     time_recorded; // Flag to indicate event_time is loaded
    uint8_t     probe_count;
    uint8_t     conflict_count;
    uint8_t     announce_count;
    TCPIP_MAC_ADDR    temp_MAC_addr;   // Temporary MAC address
    ZCLL_FLAGS  zcll_flags;
    uint8_t     bDefaultIPTried;
    int8_t      arpRegId;
} ZCLL_NET_HANDLE;




#endif
