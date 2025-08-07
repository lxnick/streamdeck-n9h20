/****************************************************************
 *                                                              *
 * Copyright (c) Chicony Electronics Co., Ltd                   *
 *                                                              *
 ****************************************************************/
#ifndef DEVELOP_UTILS_HEADER
#define DEVELOP_UTILS_HEADER

#include "stdint.h"

struct stream_deck_fill
{
	uint8_t report_id;
	uint8_t	command;	// 0xD0
	uint8_t x_low;
	uint8_t x_hi;
	uint8_t y_low;
	uint8_t y_hi;
	uint8_t size_x_low;
	uint8_t size_x_hi;
	uint8_t size_y_low;
	uint8_t size_y_hi;
	uint8_t color_low;
	uint8_t color_hi;
	uint8_t data[1];
};


void osd_fill_rgb565(uint8_t* p, int width, int height, int x, int y, int size_x, int size_y,  uint16_t pattern);

void command_fill(uint8_t* p);
	
	

#endif // #ifndef DEVELOP_UTILS_HEADER
