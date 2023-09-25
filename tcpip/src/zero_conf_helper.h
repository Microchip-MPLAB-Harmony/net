/*******************************************************************************
  Zero Configuration (Zeroconf) Helper Module for Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    zero_conf_helper.h

  Summary:
    
  Description:
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

#ifndef __ZEROCONF_HELPER_H
#define __ZEROCONF_HELPER_H

// Debugging levels
//#define DEBUG_ZCLL  y     /* Debug Enable for Zeroconf Link-Local Module */
//#define INFO_ZCLL   y
//#define WARN_ZCLL   y

// Debugging levels
//#define DEBUG_MDNS y
//#define INFO_MDNS  y
//#define WARN_MDNS  y

extern uint8_t zeroconf_dbg_level;

#if defined(DEBUG_ZCLL) || defined(INFO_ZCLL) || defined(DEBUG_MDNS) || defined(INFO_MDNS)
    /* defined(WARN_ZCLL) || defined(WARN_MDNS */

    #define NEED_TO_DEFINE_zeroconf_dbg_msg     y
    #define ZEROCONF_DBG_MSG_SIZE  256
    
    extern char zeroconf_dbg_msg[ZEROCONF_DBG_MSG_SIZE];

#if defined(INFO_ZCLL)
    extern void info_zcll_print(char * msg);
#endif
#if defined(DEBUG_ZCLL)
    extern void debug_zcll_print(char * msg);
#endif

#if defined(INFO_MDNS)
    extern void info_mdns_print(char * msg);
#endif

#if defined(DEBUG_MDNS)
    extern void debug_mdns_print(char * msg);
#endif

#endif//#if defined(DEBUG_ZCLL) || defined(INFO_ZCLL) || defined(DEBUG_MDNS) || defined(INFO_MDNS)

#define ZGZC_STARTED_WAITING  (1u)
#define ZGZC_KEEP_WAITING     (3u)
#define ZGZC_DONE_WAITING     (5u)

uint8_t zgzc_wait_for(uint32_t *pTicksToWait, uint32_t *pStartingTickTime, uint8_t *pIsStarted);


#endif//__ZEROCONF_HELPER_H
