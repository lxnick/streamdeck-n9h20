/****************************************************************
 *                                                              *
 * Copyright (c) Chicony Electronics Co., Ltd                   *
 *                                                              *
 ****************************************************************/
#ifndef DEVELOP_UTILITY_HEADER
#define DEVELOP_UTILITY_HEADER

#include "wbio.h"
#include "wbtypes.h"
#include "stdint.h"

typedef enum {
    IMG_OP_ROT_90,
    IMG_OP_ROT_180,
    IMG_OP_ROT_270,
    IMG_OP_MIRROR,   
    IMG_OP_FLIP     
} img_op_t;

void develop_blt2d_clear_buffer(void* buffer, uint32_t fill_color);

void develop_vdma_copy(void* src, void* dest, uint32_t size);

void develop_img_mirror_rgb565(void* src, int w, int h, void* dest);
	


#endif // #ifndef DEVELOP_UTILITY_HEADER


