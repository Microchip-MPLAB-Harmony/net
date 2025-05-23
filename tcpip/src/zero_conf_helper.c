/*******************************************************************************
  Zero Confiruation (Zeroconf) Helper
  Module for Microchip TCP/IP Stack

  Summary:
    
  Description:
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









#include "tcpip/src/tcpip_private.h"

#if defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL) 

#include "zero_conf_helper.h"

uint8_t zeroconf_dbg_level = 3; // All levels of debug info are printed.

void info_zcll_print(const char * msg)
{
#if defined(TCPIP_ZC_INFO_ZCLL)
    if (zeroconf_dbg_level >= 1U)
    {
        SYS_CONSOLE_MESSAGE(msg);
    }
#endif
}

void debug_zcll_print(const char * msg)
{
#if defined(TCPIP_ZC_DEBUG_ZCLL)
    if (zeroconf_dbg_level >= 2U)
    {
        SYS_CONSOLE_MESSAGE(msg);
    }
#endif
}

void info_mdns_print(const char * msg)
{
#if defined(TCPIP_ZC_INFO_MDNS)
    if (zeroconf_dbg_level >= 1U)
    {
        SYS_CONSOLE_MESSAGE(msg);
    }
#endif
}

void debug_mdns_print(const char * msg)
{
#if defined(TCPIP_ZC_DEBUG_MDNS)
    if (zeroconf_dbg_level >= 2U)
    {
        SYS_CONSOLE_MESSAGE(msg);
    }
#endif
}

uint8_t zgzc_wait_for(uint32_t *pTicksToWait, uint32_t *pStartingTickTime, uint8_t *pWaitingHasStarted)
{
    if ( (*pWaitingHasStarted) == 0U )
    {
        // start a new waiting period
        *pStartingTickTime = SYS_TMR_TickCountGet();// The time we started the waiting.
        *pWaitingHasStarted = 1;            // To indicate that the timer has started.

        return ZGZC_STARTED_WAITING;
    }

    if( (SYS_TMR_TickCountGet() - *pStartingTickTime) < (*pTicksToWait) )
    {
        /* Not Completed the delay proposed */
        return ZGZC_KEEP_WAITING;
    }
    
    // We have completed the required waiting.

    *pStartingTickTime  = 0 ; /* Reset starting time. Not really necessary. */
    *pWaitingHasStarted = 0;  /* Reset timer */

    return ZGZC_DONE_WAITING;
}

#endif // (TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)


