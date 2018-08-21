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

