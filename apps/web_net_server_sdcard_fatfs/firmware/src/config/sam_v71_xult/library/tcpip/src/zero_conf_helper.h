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
/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/








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
