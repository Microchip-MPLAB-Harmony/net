/*
Copyright (C) 2019-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#include "configuration.h"
#include "sys_time_h2_adapter.h"

static SYS_TIME_H2_ADAPTER_OBJ systemAdaptObj;


void sy_time_h2_adapter_callback( uintptr_t context )
{
	systemAdaptObj.callback(context,0);
}

SYS_TMR_HANDLE SYS_TMR_CallbackPeriodic ( uint32_t periodMs, uintptr_t context, SYS_TMR_CALLBACK callback )
{
	systemAdaptObj.callback = callback;
	return SYS_TIME_CallbackRegisterMS((SYS_TIME_CALLBACK)sy_time_h2_adapter_callback, context, periodMs, SYS_TIME_PERIODIC );
}

static uint32_t gTickConv = 0;

uint32_t SYS_TMR_TickCountGet(void)
{
    if(gTickConv == 0)
    {
        gTickConv = SYS_TIME_MSToCount(1);
    }

    return (uint32_t)(SYS_TIME_Counter64Get() / (uint64_t)gTickConv);
}

uint64_t SYS_TMR_TickCountGetLong(void)
{
    if(gTickConv == 0)
    {
        gTickConv = SYS_TIME_MSToCount(1);
    }

	return SYS_TIME_Counter64Get() / (uint64_t)gTickConv;
}

uint32_t SYS_TMR_TickCounterFrequencyGet ( void )
{
    if(gTickConv == 0)
    {
        gTickConv = SYS_TIME_MSToCount(1);
    }

	return SYS_TIME_FrequencyGet() / gTickConv;
}
