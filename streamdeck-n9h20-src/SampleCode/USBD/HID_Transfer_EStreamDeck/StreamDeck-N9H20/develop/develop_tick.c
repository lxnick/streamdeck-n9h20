#include "develop_tick.h"

#include "N9H20.h"

void develop_tick_start(INT32 nTimeNo, UINT32 tick_per_second)
{
	UINT32 u32ExtFreq ;

	u32ExtFreq = sysGetExternalClock()*1000;
	sysSetTimerReferenceClock(nTimeNo, u32ExtFreq); 

	sysStartTimer(nTimeNo, tick_per_second, PERIODIC_MODE);
}

void develop_tick_stop(INT32 nTimeNo)
{
	sysStopTimer(nTimeNo);
}

INT32 develop_tick_set_event(INT32 nTimeNo, UINT32 ticks, FN_TICK_EVENT fn_event)
{
	return sysSetTimerEvent(nTimeNo, ticks, (PVOID) fn_event);
}

UINT32 develop_tick_get_count(INT32 nTimeNo)
{
	return sysGetTicks(nTimeNo);
}

UINT32 develop_tick_get_ms(INT32 nTimeNo)
{
		return sysGetTicks(nTimeNo)/100;
}
