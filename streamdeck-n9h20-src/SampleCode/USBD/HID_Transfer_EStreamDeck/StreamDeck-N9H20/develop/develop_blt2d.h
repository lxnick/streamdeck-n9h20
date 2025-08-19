/****************************************************************
 *                                                              *
 * Copyright (c) Chicony Electronics Co., Ltd                   *
 *                                                              *
 ****************************************************************/
#ifndef DEVELOP_BLT2D_UTILITY_HEADER
#define DEVELOP_BLT2D_UTILITY_HEADER

#include "wbio.h"
#include "wbtypes.h"
#include "stdint.h"

#define NONE_CACHE_MASK			((uintptr_t)0x80000000u)
#define MAKE_NONE_CACHE(p) 	((void*)((uintptr_t)(p) | NONE_CACHE_MASK))
#define MAKE_CACHE(p) 			((void*)((uintptr_t)(p) & ~NONE_CACHE_MASK))


void develop_blt2d_test(void);

void _blt2d_clear_buffer(void* buffer, uint32_t fill_color);

#endif // #ifndef DEVELOP_BLT2D_UTILITY_HEADER

