/*******************************************************************************
  Zero Confiruation (Zeroconf) Helper
  Module for Microchip TCP/IP Stack

  Summary:
    
  Description:
*******************************************************************************/

/*******************************************************************************
File Name:  ZeroconfHelper.h
Copyright © 2012 released Microchip Technology Inc.  All rights
reserved.

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

#include "tcpip/src/tcpip_private.h"

#if defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL) 

#include "zero_conf_helper.h"

uint8_t zeroconf_dbg_level = 3; // All levels of debug info are printed.

#if defined(NEED_TO_DEFINE_zeroconf_dbg_msg)

char zeroconf_dbg_msg[ZEROCONF_DBG_MSG_SIZE];

#endif

#if defined(TCPIP_ZC_INFO_ZCLL)
void info_zcll_print(char * msg)
{
	if (zeroconf_dbg_level >= 1)
		SYS_CONSOLE_MESSAGE(msg);
}
#endif

#if defined(TCPIP_ZC_DEBUG_ZCLL)
void debug_zcll_print(char * msg)
{
	if (zeroconf_dbg_level >= 2)
		SYS_CONSOLE_MESSAGE(msg);
}
#endif

#if defined(TCPIP_ZC_INFO_MDNS)
void info_mdns_print(char * msg)
{
	if (zeroconf_dbg_level >= 1)
		SYS_CONSOLE_MESSAGE(msg);
}
#endif

#if defined(TCPIP_ZC_DEBUG_MDNS)
void debug_mdns_print(char * msg)
{
	if (zeroconf_dbg_level >= 2)
		SYS_CONSOLE_MESSAGE(msg);
}
#endif

/*
The calling convention is:

	static uint32_t event_time = 0;
	static uint32_t random_delay = 0;
	static uint8_t time_recorded = 0;

	switch ( zgzc_wait_for(&random_delay, &event_time, &time_recorded) )
	{
	case ZGZC_STARTED_WAITING:

		# Set random_delay value;

		// Intentional fall-through

	case ZGZC_KEEP_WAITING:

		// Not Completed the delay proposed
		return;
	}

	// Completed the delay required

	# Do the scheduled work;
*/

uint8_t zgzc_wait_for(uint32_t *pTicksToWait, uint32_t *pStartingTickTime, uint8_t *pWaitingHasStarted)
{
	if ( !(*pWaitingHasStarted) )
	{
		// start a new waiting period
		*pStartingTickTime = SYS_TMR_TickCountGet();// The time we started the waiting.
		*pWaitingHasStarted = 1;			// To indicate that the timer has started.

		return ZGZC_STARTED_WAITING;
	}

	if( (SYS_TMR_TickCountGet() - *pStartingTickTime) < (*pTicksToWait) )
	{
		/* Not Completed the delay proposed */
		return ZGZC_KEEP_WAITING;
	}
	
	// We have completed the required waiting.

	*pStartingTickTime	= 0 ; /* Reset starting time. Not really necessary. */
	*pWaitingHasStarted = 0;  /* Reset timer */

	return ZGZC_DONE_WAITING;
}

#endif // (TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)


