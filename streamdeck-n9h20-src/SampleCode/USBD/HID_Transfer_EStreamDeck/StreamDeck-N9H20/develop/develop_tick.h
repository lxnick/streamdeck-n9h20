/****************************************************************
 *                                                              *
 * Copyright (c) Chicony Electronics Co., Ltd                   *
 *                                                              *
 ****************************************************************/
#ifndef DEVELOP_TICK_UTILITY_HEADER
#define DEVELOP_TICK_UTILITY_HEADER

#include "wbio.h"
#include "wbtypes.h"
#include "stdint.h"

// nTimeNo = TIMER0/TIMER1

#define TICK_10US	(100*1000)	//	100*1000 ticks per second

typedef void (*FN_TICK_EVENT)(void);

void develop_tick_start(INT32 nTimeNo, UINT32 ticks_per_second); 
void develop_tick_stop(INT32 nTimeNo);

INT32 develop_tick_set_event(INT32 nTimeNo, UINT32 ticks, FN_TICK_EVENT);
UINT32 develop_tick_get_count(INT32 nTimeNo);
UINT32 develop_tick_get_ms(INT32 nTimeNo);
	

#endif // #ifndef DEVELOP_TICK_UTILITY_HEADER
