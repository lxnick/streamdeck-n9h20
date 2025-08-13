/****************************************************************
 *                                                              *
 * Copyright (c) Chicony Electronics Co., Ltd                   *
 *                                                              *
 ****************************************************************/
#ifndef FRAME_BUFFER_UTILITY
#define FRAME_BUFFER_UTILITY

#include "stdint.h"
#include <stdio.h>
#include <string.h>

// ================= LCD / Framebuffer config =================
#define FB_WIDTH					480
#define FB_HEIGHT					272
#define FB_PER_PIXEL			2       // RGB565
#define FB_LINE						(FB_WIDTH*FB_PER_PIXEL)
#define FB_SIZE						(FB_WIDTH * FB_HEIGHT * FB_PER_PIXEL)	

#define DUAL_BUFFER	0

int fb_init(void);
uint8_t *fb_get_draw_buffer(void);
void fb_swap(void);

void fb_animate(void);
void fb_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

void fb_copy_rect(uint8_t* src, uint16_t srcw, uint16_t srch, uint16_t destx, uint16_t desty);

#endif // FRAME_BUFFER_HEADER
