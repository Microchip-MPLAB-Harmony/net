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

#ifndef __ZEROCONF_HELPER_H
#define __ZEROCONF_HELPER_H

// Debugging levels
//#define DEBUG_ZCLL  y	    /* Debug Enable for Zeroconf Link-Local Module */
//#define INFO_ZCLL   y
//#define WARN_ZCLL   y

// Debugging levels
//#define DEBUG_MDNS y
//#define INFO_MDNS  y
//#define WARN_MDNS  y

extern uint8_t zeroconf_dbg_level;

#if defined(DEBUG_ZCLL) || defined(INFO_ZCLL) || defined(DEBUG_MDNS) || defined(INFO_MDNS)
	/* defined(WARN_ZCLL) || defined(WARN_MDNS */

	#define NEED_TO_DEFINE_zeroconf_dbg_msg 	y
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
