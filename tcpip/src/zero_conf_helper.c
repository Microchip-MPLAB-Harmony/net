/*******************************************************************************
  Zero Confiruation (Zeroconf) Helper
  Module for Microchip TCP/IP Stack

  Summary:
    
  Description:
*******************************************************************************/

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









#include "tcpip/src/tcpip_private.h"

#if defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL) 

#include "zero_conf_helper.h"

uint8_t zeroconf_dbg_level = 3; // All levels of debug info are printed.

#if defined(NEED_TO_DEFINE_zeroconf_dbg_msg)

char zeroconf_dbg_msg[ZEROCONF_DBG_MSG_SIZE];

#endif

#if defined(TCPIP_ZC_INFO_ZCLL)
void info_zcll_print(const char * msg)
{
	if (zeroconf_dbg_level >= 1)
		SYS_CONSOLE_MESSAGE(msg);
}
#endif

#if defined(TCPIP_ZC_DEBUG_ZCLL)
void debug_zcll_print(const char * msg)
{
	if (zeroconf_dbg_level >= 2)
		SYS_CONSOLE_MESSAGE(msg);
}
#endif

#if defined(TCPIP_ZC_INFO_MDNS)
void info_mdns_print(const char * msg)
{
	if (zeroconf_dbg_level >= 1)
		SYS_CONSOLE_MESSAGE(msg);
}
#endif

#if defined(TCPIP_ZC_DEBUG_MDNS)
void debug_mdns_print(const char * msg)
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


