/*******************************************************************************
  Zero Configuration (zero_conf) Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    zero_conf_config.h

  Summary:
    Zero Configuration Configuration file
    
  Description:
    This file contains the zero configuration module configuration options
    
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
#ifndef _ZEROCONF_CONFIG_H_
#define _ZEROCONF_CONFIG_H_

#define TCPIP_ZC_INFO_ZCLL
#define TCPIP_ZC_DEBUG_ZCLL
#define TCPIP_ZC_INFO_MDNS
#define TCPIP_ZC_DEBUG_MDNS

/* constants from RFC 2937, section 9. ALso found in RFC 5227 Par 1.1*/
#define TCPIP_ZC_LL_PROBE_WAIT           1 /*second  (initial random delay)              */
#define TCPIP_ZC_LL_PROBE_MIN            1 /*second  (minimum delay till repeated probe) */
#define TCPIP_ZC_LL_PROBE_MAX            2 /*seconds (maximum delay till repeated probe) */
#define TCPIP_ZC_LL_PROBE_NUM            3 /*         (number of probe packets)          */
#define TCPIP_ZC_LL_ANNOUNCE_WAIT        2 /*seconds  (delay before announcing)          */
#define TCPIP_ZC_LL_ANNOUNCE_NUM         2 /*        (number of announcement packets)    */
#define TCPIP_ZC_LL_ANNOUNCE_INTERVAL    2 /*seconds (time between announcement packets) */
#define TCPIP_ZC_LL_MAX_CONFLICTS       10 /*        (max conflicts before rate limiting)*/
#define TCPIP_ZC_LL_RATE_LIMIT_INTERVAL 60 /*seconds (delay between successive attempts) */
#define TCPIP_ZC_LL_DEFEND_INTERVAL     10 /*seconds (min. wait between defensive ARPs)  */

/* compilation constants */
#define TCPIP_ZC_LL_IPV4_LLBASE         0xa9fe0100 /* 169.254.1.0 */
#define TCPIP_ZC_LL_IPV4_LLBASE_MASK    0x0000FFFF /* corresponding network mask */

// ZCLL task processing rate in milliseconds.
// The default value is 333 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_ZC_LL_TASK_TICK_RATE 333

// MDNS/Bonjour task processing rate in milliseconds.
// The default value is 63 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_ZC_MDNS_TASK_TICK_RATE     63

#define TCPIP_ZC_MDNS_PORT            5353
#define TCPIP_ZC_MDNS_MAX_HOST_NAME_SIZE   32      //31+'\0'  Max Host name size
#define TCPIP_ZC_MDNS_MAX_LABEL_SIZE       64      //63+'\0'  Maximum size allowed for a label. RFC 1035 (2.3.4) == 63
#define TCPIP_ZC_MDNS_MAX_RR_NAME_SIZE     256   //255+'\0' Max Resource Recd Name size. RFC 1035 (2.3.4) == 255
#define TCPIP_ZC_MDNS_MAX_SRV_TYPE_SIZE    32      //31+'\0'  eg. "_http._tcp.local". Max could be 255, but is an overkill.
#define TCPIP_ZC_MDNS_MAX_SRV_NAME_SIZE    64      //63+'\0'  eg. "My Web server". Max could be 255, but is an overkill.
#define TCPIP_ZC_MDNS_MAX_TXT_DATA_SIZE    128   //127+'\0' eg. "path=/index.htm"
#define TCPIP_ZC_MDNS_RESOURCE_RECORD_TTL_VAL     3600 // Time-To-Live for a Resource-Record in seconds.

#define TCPIP_ZC_MDNS_MAX_RR_NUM  4            // for A, PTR, SRV, and TXT  Max No.of Resource-Records/Service

/* Constants from mdns.txt (IETF Draft)*/
#define TCPIP_ZC_MDNS_PROBE_WAIT             750 // msecs  (initial random delay)
#define TCPIP_ZC_MDNS_PROBE_INTERVAL         250 // msecs (maximum delay till repeated probe)
#define TCPIP_ZC_MDNS_PROBE_NUM                3 //      (number of probe packets)
#define TCPIP_ZC_MDNS_MAX_PROBE_CONFLICT_NUM  30 // max num of conflicts before we insist and move on to announce ...
#define TCPIP_ZC_MDNS_ANNOUNCE_NUM             3 //      (number of announcement packets)
#define TCPIP_ZC_MDNS_ANNOUNCE_INTERVAL      250 // msecs (time between announcement packets)
#define TCPIP_ZC_MDNS_ANNOUNCE_WAIT          250 // msecs (delay before announcing)

#endif
